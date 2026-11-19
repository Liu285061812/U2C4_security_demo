#include "r_aceu.h"
#include <stddef.h>
#include <string.h>

#ifndef ACEU_READ
#include DEVICE_FILE
#define ACEU_READ(offset) (*(volatile uint32_t *)(ACEU_BASE + (offset)))
#define ACEU_WRITE(offset, value) (ACEU_READ(offset) = (value))
#define ACEU_KEY_READ(offset) (*(volatile uint32_t *)(ACEU_KEYCTRL_BASE + (offset)))
#define ACEU_KEY_WRITE(offset, value) (ACEU_KEY_READ(offset) = (value))
#define ACEU_BARRIER() __asm("syncm")
#define ACEU_DTF_RAM_BASE 0xFE000000u
#define ACEU_DTF_RAM_END  0xFE070000u
#define ACEU_DTF_END_READ() ((uint32_t)INTC2EIC317)
#define ACEU_DTF_END_WRITE(value) (INTC2EIC317 = (uint16_t)(value))
#endif

#define CTRL       0x00u
#define DCTL       0x04u
#define CCTL0      0x08u
#define KEY        0x1Cu
#define STR        0x10u
#define IDAT       0x20u
#define ODAT       0x24u
#define DTF_START  0x60u
#define DTF_START_BIT 0x00010000u
#define DTF_COUNT  0x6Cu
#define DTF_INPUT  0x70u
#define DTF_OUTPUT 0x74u
#define CPU_ORDER  0x200u
#define DTF_RUN    0x01000000u
#define ENGINE_BUSY (DTF_RUN | 0x10000000u | 1u)
#define IO_ERRORS 0x0000F000u
#define IV_USE     4u
#define NEW_KEY    1u
#define LAST       0x8000u
#define POLL_LIMIT 1000000u
#define DTF_END_PENDING 0x1000u
#define DTF_END_MASK    0x0080u

static uint32_t s_active;
static uint32_t s_faulted;
static uint32_t s_new_key;
static uint32_t s_key_length;
static uint32_t s_iv_use;
static aceu_request_t *s_request;
static uint8_t s_last_output[16];

static uint32_t load_be(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | p[3];
}

static void store_be(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

static void store_length(uint8_t *p, uint32_t bytes)
{
    store_be(p, bytes >> 29);
    store_be(p + 4, bytes << 3);
}

static aceu_result_t wait_status(uint32_t mask, uint32_t expected)
{
    uint32_t count;
    for (count = 0u; count < POLL_LIMIT; count++)
    {
        uint32_t status = ACEU_READ(STR);
        if ((status & IO_ERRORS) != 0u)
        {
            s_faulted = 1u;
            return ACEU_ERR_HARDWARE;
        }
        if ((status & mask) == expected) return ACEU_OK;
    }
    s_faulted = 1u;
    return ACEU_ERR_TIMEOUT;
}

static aceu_result_t select_key(uint32_t slot)
{
    uint32_t count;
    ACEU_KEY_WRITE(0x10u, slot);
    for (count = 0u; count < POLL_LIMIT; count++)
    {
        uint32_t info = ACEU_KEY_READ(0x1Cu);
        if ((info & 0x8000u) != 0u)
        {
            s_key_length = (info >> 8) & 3u;
            if (s_key_length == 3u) return ACEU_ERR_KEY;
            s_new_key = 1u;
            ACEU_WRITE(KEY, s_key_length);
            return ACEU_OK;
        }
    }
    return ACEU_ERR_KEY;
}

static aceu_result_t wait_dtf_end(void)
{
    uint32_t count;
    for (count = 0u; count < POLL_LIMIT; count++)
    {
        uint32_t status = ACEU_READ(STR);
        if ((status & IO_ERRORS) != 0u)
        {
            s_faulted = 1u;
            return ACEU_ERR_HARDWARE;
        }
        /* The pending bit also proves completion if the whole RUN pulse was missed. */
        if ((status & DTF_RUN) == 0u &&
            (ACEU_DTF_END_READ() & DTF_END_PENDING) != 0u)
            return ACEU_OK;
    }
    s_faulted = 1u;
    return ACEU_ERR_TIMEOUT;
}

static void write_register_block(uint32_t destination, const uint8_t *data)
{
    uint32_t i;
    uint32_t control = ACEU_READ(CTRL) & ~0x33000000u;
    ACEU_WRITE(CTRL, control | (destination << 24));
    for (i = 0u; i < 16u; i += 4u) ACEU_WRITE(IDAT, load_be(data + i));
}

static void read_block(uint8_t *output)
{
    uint32_t i;
    for (i = 0u; i < 16u; i += 4u)
    {
        uint32_t word = ACEU_READ(ODAT);
        if (output != NULL) store_be(output + i, word);
    }
}

static uint32_t data_control(uint32_t attr, uint32_t valid, uint32_t last)
{
    if (s_request->mode == ACEU_CCM ||
        ((s_request->mode == ACEU_GCM || s_request->mode == ACEU_GMAC) && attr == 0u))
        valid = 16u;
    uint32_t value = (attr << 16) | (last ? LAST : 0u) |
                     (((valid * 8u) & 127u) << 8) |
                     (s_iv_use ? IV_USE : 0u) | (s_new_key ? NEW_KEY : 0u);
    return value;
}

static aceu_result_t block(uint32_t attr, uint32_t last, const uint8_t *input,
                           uint32_t size, uint8_t *output)
{
    uint8_t in[16] = {0};
    uint8_t out[16];
    uint32_t i;
    aceu_result_t result;
    if (size != 0u) memcpy(in, input, size);
    ACEU_WRITE(CTRL, ACEU_READ(CTRL) & ~0x33000000u);
    ACEU_WRITE(DCTL, data_control(attr, size, last));
    /* KEYUERR becomes set after NEW_KEY is consumed; check it before starting. */
    if ((s_request->mode != ACEU_PRNG) && ((ACEU_KEY_READ(0x1Cu) & 0x3Fu) != 0u))
        return ACEU_ERR_KEY;
    if (s_request->mode == ACEU_GCM && attr == 1u && !s_request->decrypt && !last)
        ACEU_WRITE(0x14u, 0x100u);
    for (i = 0u; i < 16u; i += 4u) ACEU_WRITE(IDAT, load_be(in + i));
    ACEU_BARRIER();
    result = wait_status(1u, 0u);
    if (result != ACEU_OK) return result;
    read_block(out);
    if (s_request->mode == ACEU_GCM && attr == 1u && !s_request->decrypt && !last)
    {
        result = wait_status(0x100u, 0x100u);
        if (result != ACEU_OK) return result;
    }
    s_new_key = 0u;
    s_iv_use = 0u;
    s_request->cpu_blocks++;
    memcpy(s_last_output, out, sizeof(out));
    if (output != NULL) memcpy(output, out, sizeof(out));
    return ACEU_OK;
}

static int dtf_address(const void *p, uint32_t size)
{
    uint32_t address = (uint32_t)p;
    return ((address & 3u) == 0u) && (address >= ACEU_DTF_RAM_BASE) &&
           (address < ACEU_DTF_RAM_END) && (size <= ACEU_DTF_RAM_END - address);
}

static aceu_result_t dtf(const uint8_t *input, uint8_t *output,
                         uint32_t size, uint32_t attr)
{
    aceu_result_t result;
    uint32_t control = (ACEU_READ(CTRL) & ~0x33000400u) | 0x10000000u;
    ACEU_WRITE(CTRL, control | 0x20u);
    /* A null destination suppresses intermediate authentication output. */
    if (output == NULL) control |= 0x00100000u;
    else control &= ~0x00100000u;
    ACEU_WRITE(CTRL, control);
    ACEU_WRITE(DCTL, data_control(attr, 16u, 0u));
    if ((ACEU_KEY_READ(0x1Cu) & 0x3Fu) != 0u) return ACEU_ERR_KEY;
    ACEU_WRITE(DTF_COUNT, size / 16u);
    ACEU_WRITE(DTF_INPUT, (uint32_t)input);
    ACEU_WRITE(DTF_OUTPUT, (uint32_t)output);
    ACEU_DTF_END_WRITE((ACEU_DTF_END_READ() | DTF_END_MASK) & ~DTF_END_PENDING);
    ACEU_BARRIER();
    if ((ACEU_DTF_END_READ() & DTF_END_PENDING) != 0u)
    {
        s_faulted = 1u;
        return ACEU_ERR_HARDWARE;
    }
    ACEU_WRITE(DTF_START, DTF_START_BIT);
    ACEU_BARRIER();
    result = wait_dtf_end();
    ACEU_BARRIER();
    if (result != ACEU_OK) return result;
    s_request->dtf_blocks += size / 16u;
    s_new_key = 0u;
    s_iv_use = 0u;
    if (s_request->mode == ACEU_CMAC ||
        (s_request->mode == ACEU_CCM && s_request->decrypt && attr == 4u))
        read_block(s_last_output);
    /* Subsequent CPU reads must select BCO, not BCO_DTF. */
    ACEU_WRITE(CTRL, control & ~0x33100000u);
    return ACEU_OK;
}

static aceu_result_t stream(const uint8_t *input, uint8_t *output, uint32_t size,
                            uint32_t attr, uint32_t last)
{
    uint32_t bulk = size & ~15u;
    uint32_t offset = 0u;
    uint32_t authenticated = s_request->mode == ACEU_GCM ||
                             s_request->mode == ACEU_GMAC || s_request->mode == ACEU_CCM;
    aceu_result_t result;
    if (authenticated && (size & 15u)) bulk = 0u;
    if ((s_request->mode != ACEU_MP) && (s_request->mode != ACEU_PRNG) &&
        (s_request->mode != ACEU_CMAC) &&
        (bulk >= 32u) && dtf_address(input, bulk) &&
        ((output == NULL) || dtf_address(output, bulk)))
    {
        result = dtf(input, output, bulk, attr);
        if (result != ACEU_OK) return result;
        offset = bulk;
    }
    while (offset < size)
    {
        uint8_t out[16];
        uint32_t count = size - offset;
        if (count > 16u) count = 16u;
        result = block(attr, last && (offset + count == size), input + offset,
                       count, output == NULL ? NULL : out);
        if (result != ACEU_OK) return result;
        if (output != NULL) memcpy(output + offset, out, count);
        offset += count;
    }
    return ACEU_OK;
}

static aceu_result_t initialize(uint32_t mode, uint32_t decrypt, uint32_t slot)
{
    aceu_result_t result = wait_status(ENGINE_BUSY, 0u);
    if (result != ACEU_OK) return result;
    result = select_key(slot);
    if (result != ACEU_OK) return result;
    ACEU_WRITE(CTRL, CPU_ORDER | 1u);
    ACEU_WRITE(CTRL, CPU_ORDER);
    ACEU_WRITE(KEY, s_key_length);
    ACEU_WRITE(0x18u, 0u);
    ACEU_WRITE(CCTL0, 0x100u | (decrypt << 4) | mode);
    s_iv_use = 1u;
    return ACEU_OK;
}

static aceu_result_t simple_cipher(void)
{
    aceu_request_t *r = s_request;
    aceu_result_t result = initialize((uint32_t)r->mode, r->decrypt, r->key_slot);
    if (result != ACEU_OK) return result;
    if (r->mode != ACEU_ECB) write_register_block(2u, r->iv);
    return stream(r->input, r->output, r->length, 0u, 0u);
}

static aceu_result_t xts(void)
{
    aceu_request_t *r = s_request;
    uint8_t intermediate[16], tail[16] = {0}, penultimate[16];
    uint32_t remainder = r->length & 15u;
    uint32_t prefix = remainder ? (r->length & ~15u) - 16u :
                      (r->length >= 32u ? r->length - 32u : 0u);
    uint32_t key_length;
    aceu_result_t result = initialize(ACEU_XTS, r->decrypt, r->tweak_slot);
    if (result != ACEU_OK) return result;
    key_length = s_key_length;
    if (key_length == 1u) return ACEU_ERR_KEY;
    result = block(1u, 0u, r->iv, 16u, NULL);
    if (result != ACEU_OK) return result;
    result = select_key(r->key_slot);
    if (result != ACEU_OK) return result;
    if (key_length != s_key_length) return ACEU_ERR_KEY;
    result = stream(r->input, r->output, prefix, 0u, 0u);
    if (result != ACEU_OK) return result;
    if (remainder == 0u)
    {
        if (r->length - prefix == 32u)
        {
            result = block(0u, 0u, r->input + prefix, 16u, r->output + prefix);
            if (result != ACEU_OK) return result;
            prefix += 16u;
        }
        return block(0u, 1u, r->input + prefix, 16u, r->output + prefix);
    }
    memcpy(penultimate, r->input + prefix, 16u);
    memcpy(tail, r->input + prefix + 16u, remainder);
    result = block(r->decrypt ? 2u : 0u, 0u, penultimate, 16u, intermediate);
    if (result != ACEU_OK) return result;
    /* CTS produces its last two output blocks in reverse order. */
    memcpy(r->output + prefix + 16u, intermediate, remainder);
    return block(0u, 1u, tail, remainder, r->output + prefix);
}

static aceu_result_t gcm(void)
{
    aceu_request_t *r = s_request;
    uint8_t j0[16] = {0}, j1[16], lengths[16] = {0};
    uint8_t expected[16];
    uint32_t i, difference = 0u;
    aceu_result_t result = initialize(ACEU_GCM, r->decrypt, r->key_slot);
    if (result != ACEU_OK) return result;
    memcpy(j0, r->iv, 12u);
    j0[15] = 1u;
    memcpy(j1, j0, sizeof(j1));
    j1[15] = 2u;
    write_register_block(2u, j1);
    result = stream(r->aad, NULL, r->aad_length, 0u, 0u);
    if (result != ACEU_OK) return result;
    /* First GCTR uses IV even when AAD has already consumed NEW_KEY. */
    s_iv_use = 1u;
    result = stream(r->input, r->output, r->length, 1u, 1u);
    if (result != ACEU_OK) return result;
    write_register_block(2u, j0);
    s_iv_use = (r->length == 0u);
    store_length(lengths, r->aad_length);
    store_length(lengths + 8, r->length);
    result = block(2u, 0u, lengths, 16u, expected);
    if (result != ACEU_OK) return result;
    if (r->decrypt)
    {
        for (i = 0u; i < r->tag_length; i++) difference |= expected[i] ^ r->tag[i];
        if (difference != 0u) return ACEU_ERR_AUTH;
    }
    else memcpy(r->tag, expected, r->tag_length);
    return ACEU_OK;
}

static aceu_result_t ccm(void)
{
    aceu_request_t *r = s_request;
    uint8_t nonce[16] = {0}, b0[16] = {0}, tag[16] = {0}, yr[16] = {0};
    uint32_t i, difference = 0u;
    aceu_result_t result = initialize(ACEU_CCM, r->decrypt, r->key_slot);
    if (result != ACEU_OK) return result;
    memcpy(nonce + 16u - r->iv_length, r->iv, r->iv_length);
    write_register_block(2u, nonce);
    store_be(b0, r->aad_length * 8u);
    b0[5] = (uint8_t)(((r->iv_length - 7u) << 4) | ((r->tag_length - 4u) / 2u));
    store_length(b0 + 8, r->length);
    result = block(1u, 0u, b0, 16u, NULL);
    if (result != ACEU_OK) return result;
    if (r->decrypt)
    {
        result = stream(r->input, r->output, r->length, 2u, 0u);
        if (result != ACEU_OK) return result;
    }
    result = stream(r->aad, NULL, r->aad_length, 3u, 0u);
    if (result != ACEU_OK) return result;
    if (r->decrypt)
    {
        /* The native CCM decrypt sequence authenticates the recovered plaintext. */
        result = stream(r->output, NULL, r->length, 4u, 0u);
        if (result != ACEU_OK) return result;
        memcpy(yr, s_last_output, sizeof(yr));
        memcpy(tag, r->tag, r->tag_length);
        result = block(13u, 0u, tag, 16u, tag);
        if (result != ACEU_OK) return result;
        for (i = 0u; i < r->tag_length; i++) difference |= tag[i] ^ yr[i];
        return difference == 0u ? ACEU_OK : ACEU_ERR_AUTH;
    }
    result = stream(r->input, r->output, r->length, 4u, 0u);
    if (result != ACEU_OK) return result;
    result = block(8u, 0u, tag, 16u, tag);
    if (result == ACEU_OK) memcpy(r->tag, tag, r->tag_length);
    return result;
}

static aceu_result_t mac_or_mp(void)
{
    aceu_request_t *r = s_request;
    uint8_t result_block[16], padded[16] = {0};
    uint32_t i, difference = 0u;
    uint32_t prefix = r->length == 0u ? 0u : (r->length - 1u) & ~15u;
    uint32_t tail = r->length - prefix;
    aceu_result_t result = initialize((uint32_t)r->mode, 0u, r->key_slot);
    if (result != ACEU_OK) return result;
    if ((r->mode == ACEU_MP) && (s_key_length != 0u)) return ACEU_ERR_KEY;
    if (r->mode == ACEU_CMAC && r->length >= 32u && !(r->length & 15u) &&
        dtf_address(r->input, r->length))
    {
        result = dtf(r->input, NULL, r->length, 0u);
        if (result != ACEU_OK) return result;
        memcpy(result_block, s_last_output, sizeof(result_block));
    }
    else
    {
        result = stream(r->input, NULL, prefix, 0u, 0u);
        if (result != ACEU_OK) return result;
        if (tail != 0u) memcpy(padded, r->input + prefix, tail);
        result = block(r->length == 0u ? 1u : 0u, r->mode == ACEU_CMAC,
                       padded, tail, result_block);
        if (result != ACEU_OK) return result;
    }
    if (r->decrypt)
    {
        for (i = 0u; i < r->tag_length; i++) difference |= result_block[i] ^ r->tag[i];
        return difference == 0u ? ACEU_OK : ACEU_ERR_AUTH;
    }
    memcpy(r->tag, result_block, r->tag_length);
    return ACEU_OK;
}

static int buffer_valid(const void *p, uint32_t size)
{
    return size == 0u || (p != NULL && (uint32_t)p <= 0xFFFFFFFFu - size);
}

static int overlap(const void *a, uint32_t an, const void *b, uint32_t bn)
{
    return an != 0u && bn != 0u &&
           (uint32_t)a < (uint32_t)b + bn && (uint32_t)b < (uint32_t)a + an;
}

static aceu_result_t validate(const aceu_request_t *r)
{
    uint32_t has_output = r->mode <= ACEU_CCM || r->mode == ACEU_PRNG;
    if ((uint32_t)r->mode > ACEU_PRNG || r->decrypt > 1u || r->prng_restart > 1u ||
        r->key_slot >= 128u || r->tweak_slot >= 128u ||
        !buffer_valid(r->input, r->mode == ACEU_PRNG ? 0u : r->length) ||
        !buffer_valid(r->output, has_output ? r->length : 0u) ||
        !buffer_valid(r->aad, r->aad_length) || !buffer_valid(r->iv, r->iv_length))
        return ACEU_ERR_PARAM;
    if (has_output && r->input != r->output &&
        overlap(r->input, r->mode == ACEU_PRNG ? 0u : r->length, r->output, r->length))
        return ACEU_ERR_PARAM;
    if (has_output && overlap(r->aad, r->aad_length, r->output, r->length))
        return ACEU_ERR_PARAM;
    if (r->mode <= ACEU_OFB)
    {
        if (r->length == 0u || ((r->mode <= ACEU_CBC) && (r->length & 15u)))
            return ACEU_ERR_PARAM;
        if (r->mode != ACEU_ECB && r->iv_length != 16u) return ACEU_ERR_PARAM;
    }
    if (r->mode == ACEU_XTS && (r->length < 16u || r->iv_length != 16u ||
        r->key_slot == r->tweak_slot)) return ACEU_ERR_PARAM;
    if (r->mode == ACEU_MP && (r->length == 0u || (r->length & 15u) ||
        r->tag_length != 16u || r->decrypt)) return ACEU_ERR_PARAM;
    if (r->mode == ACEU_GCM || r->mode == ACEU_GMAC)
    {
        if (r->iv_length != 12u || !((r->tag_length >= 12u && r->tag_length <= 16u) ||
            r->tag_length == 4u || r->tag_length == 8u)) return ACEU_ERR_PARAM;
        if (r->mode == ACEU_GMAC && r->length != 0u) return ACEU_ERR_PARAM;
    }
    if (r->mode == ACEU_CCM)
    {
        if (r->iv_length < 7u || r->iv_length > 13u || r->aad_length >= 65280u ||
            r->tag_length < 4u || r->tag_length > 16u || (r->tag_length & 1u) ||
            (r->iv_length == 13u && r->length >= 65536u) ||
            (r->iv_length == 12u && r->length >= 16777216u)) return ACEU_ERR_PARAM;
    }
    if (r->mode == ACEU_CMAC && (r->tag_length == 0u || r->tag_length > 16u))
        return ACEU_ERR_PARAM;
    if (r->mode >= ACEU_GCM && r->mode != ACEU_XTS && r->mode != ACEU_PRNG)
    {
        if (!buffer_valid(r->tag, r->tag_length) ||
            overlap(r->tag, r->tag_length, r->input, r->length) ||
            overlap(r->tag, r->tag_length, r->output, has_output ? r->length : 0u))
            return ACEU_ERR_PARAM;
    }
    if (r->mode == ACEU_PRNG && (r->length == 0u || (r->length & 15u) || r->decrypt))
        return ACEU_ERR_PARAM;
    if (r->mode == ACEU_PRNG && r->key_slot > 2u) return ACEU_ERR_PARAM;
    return ACEU_OK;
}

aceu_result_t R_ACEU_Execute(aceu_request_t *r)
{
    aceu_result_t result;
    if (r == NULL) return ACEU_ERR_PARAM;
    r->dtf_blocks = r->cpu_blocks = 0u;
    result = validate(r);
    if (result != ACEU_OK) return result;
    if (s_active) return ACEU_ERR_BUSY;
    if (s_faulted) return ACEU_ERR_FAULT;
    s_active = 1u;
    s_request = r;
    if (r->mode <= ACEU_OFB) result = simple_cipher();
    else if (r->mode == ACEU_XTS) result = xts();
    else if (r->mode == ACEU_GCM || r->mode == ACEU_GMAC) result = gcm();
    else if (r->mode == ACEU_CCM) result = ccm();
    else if (r->mode == ACEU_PRNG)
    {
        uint8_t zero[16] = {0};
        uint32_t offset;
        result = wait_status(ENGINE_BUSY, 0u);
        if (result == ACEU_OK)
        {
            ACEU_WRITE(CTRL, CPU_ORDER | 0x70000u);
            ACEU_WRITE(KEY, r->key_slot);
            s_new_key = 1u;
            s_iv_use = r->prng_restart;
            for (offset = 0u; offset < r->length; offset += 16u)
            {
                result = block(0u, 0u, zero, 16u, r->output + offset);
                if (result != ACEU_OK) break;
            }
        }
    }
    else result = mac_or_mp();
    if (result == ACEU_ERR_AUTH && r->decrypt &&
        (r->mode == ACEU_GCM || r->mode == ACEU_CCM) && r->length != 0u)
        memset(r->output, 0, r->length);
    s_request = NULL;
    s_active = 0u;
    return result;
}

const char *R_ACEU_ResultName(aceu_result_t result)
{
    static const char * const names[] =
        {"OK", "PARAM", "BUSY", "TIMEOUT", "KEY", "HARDWARE", "AUTH", "FAULT"};
    return (uint32_t)result < sizeof(names) / sizeof(names[0]) ? names[result] : "UNKNOWN";
}
