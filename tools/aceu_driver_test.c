#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#undef NULL
#include "r_aceu.h"

typedef struct
{
    unsigned int attr, last, valid, slot;
    unsigned char input[16], output[16];
} step_t;

typedef struct
{
    unsigned int mode, slot, length, aad_length, iv_length, tag_length;
    unsigned int decrypt, bad, start, count, case_index;
    unsigned char tag[16];
} vector_t;

#include "aceu_trace.inc"
#include "aceu_board_vectors.inc"

static unsigned long regs[32], key_regs[8];
static unsigned char in_block[16], out_block[16], iv_block[16];
static unsigned int in_word, out_word, position, end_position;
static unsigned int key_slot, key_new, failures, checks, gh_delay, invalid_key;
static unsigned int hang, io_error, gh_stuck, started, current_vector, prng_new, prng_iv;
static const vector_t *vector;
static unsigned long dtf_end_eic;
static unsigned int dtf_defer, dtf_pending_reads, dtf_active_reads;
static unsigned int dtf_ignore_start, dtf_suppress_end, dtf_hold_run, dtf_bus_error;
#pragma alignvar(8)
static unsigned char shared_ram[0x70000];

static void check(int condition, const char *message)
{
    checks++;
    if (!condition)
    {
        failures++;
        if (failures < 20u)
            printf("FAIL vector=%u step=%u: %s\n", current_vector, position, message);
    }
}

static unsigned long fake_read(unsigned int offset);
static void fake_write(unsigned int offset, unsigned long value);
static unsigned long fake_key_read(unsigned int offset);
static void fake_key_write(unsigned int offset, unsigned long value);
static void complete_dtf(void);

#define ACEU_READ(offset) fake_read(offset)
#define ACEU_WRITE(offset, value) fake_write(offset, value)
#define ACEU_KEY_READ(offset) fake_key_read(offset)
#define ACEU_KEY_WRITE(offset, value) fake_key_write(offset, value)
#define ACEU_BARRIER() ((void)0)
#define ACEU_DTF_RAM_BASE ((uint32_t)shared_ram)
#define ACEU_DTF_RAM_END ((uint32_t)shared_ram + sizeof(shared_ram))
#define ACEU_DTF_END_READ() dtf_end_eic
#define ACEU_DTF_END_WRITE(value) (dtf_end_eic = (value))
#include "aceu.c"

static unsigned long fake_key_read(unsigned int offset)
{
    if (offset == 0x1Cu)
    {
        unsigned long code = key_slot <= 2u ? key_slot : (key_slot == 5u ? 2u : 0u);
        unsigned long usage = ((regs[1] & 1u) != 0u) != (key_new != 0u) ? 0x10u : 0u;
        return (invalid_key ? 0u : 0x8000u) | (code << 8) | usage;
    }
    return key_regs[offset / 4u];
}

static void fake_key_write(unsigned int offset, unsigned long value)
{
    check(offset == 0x10u, "context #0 key selector");
    key_slot = (unsigned int)value;
    key_new = 1u;
    key_regs[offset / 4u] = value;
}

static void consume(const unsigned char *source, unsigned char *output, int dma)
{
    const step_t *step;
    unsigned int mode = vector->mode == ACEU_GMAC ? ACEU_GCM : vector->mode;
    if (position >= end_position)
    {
        check(0, "unexpected hardware block");
        memset(output, 0, 16u);
        return;
    }
    step = &steps[position++];
    check(vector->mode == ACEU_PRNG || (regs[2] & 15u) == mode, "mode selection");
    check(vector->mode == ACEU_PRNG || (regs[2] & 0x700u) == 0x100u, "KEYRAM selection");
    check(vector->mode == ACEU_PRNG || key_slot == step->slot, "key slot");
    check((regs[7] & 3u) == (step->slot <= 2u ? step->slot : (step->slot == 5u ? 2u : 0u)),
          "key length");
    if (vector->mode != ACEU_PRNG)
        check(((regs[2] >> 4) & 1u) ==
              (vector->mode == ACEU_CMAC || vector->mode == ACEU_MP ? 0u : vector->decrypt),
              "encryption / decryption direction");
    check(((regs[1] >> 16) & 15u) == step->attr, "data attribute");
    check(memcmp(source, step->input, 16u) == 0, "hardware input / padding / B0");
    check(vector->mode != ACEU_PRNG || ((regs[0] >> 16) & 7u) == 7u, "PRNG context");
    if (vector->mode == ACEU_PRNG)
    {
        check((regs[1] & 1u) == prng_new, "PRNG NEW_KEY continuation");
        check(((regs[1] >> 2) & 1u) == prng_iv, "PRNG IV continuation");
        prng_new = 0u;
        prng_iv = 0u;
    }
    if (mode == ACEU_GCM)
    {
        unsigned char expected_iv[16];
        unsigned int i;
        for (i = 0u; i < 12u; i++) expected_iv[i] = (unsigned char)i;
        memset(expected_iv + 12u, 0, 4u);
        expected_iv[15] = step->attr == 2u ? 1u : 2u;
        check(memcmp(iv_block, expected_iv, 16u) == 0, "GCM J0 / J1 byte order");
    }
    if (mode == ACEU_CCM || (mode >= ACEU_CBC && mode <= ACEU_OFB))
    {
        unsigned char expected_iv[16] = {0};
        unsigned int i, length = mode == ACEU_CCM ? vector->iv_length : 16u;
        for (i = 0u; i < length; i++) expected_iv[16u - length + i] = (unsigned char)i;
        check(memcmp(iv_block, expected_iv, 16u) == 0, "IV / CCM nonce placement");
    }
    if (!dma)
    {
        check((regs[0] & 0x200u) != 0u, "CPU byte order");
        check(((regs[1] >> 15) & 1u) == step->last, "CPU LAST flag");
        if (mode == ACEU_CMAC || mode == ACEU_XTS || (mode == ACEU_GCM && step->attr == 1u))
            check(((regs[1] >> 8) & 127u) == ((step->valid * 8u) & 127u), "CPU REM bits");
    }
    memcpy(output, step->output, 16u);
}

static void start_cpu(void)
{
    check(!hang && !io_error, "no input while hardware failed");
    if (vector->mode == ACEU_GCM && !vector->decrypt)
        check(gh_delay == 0u, "previous GHASH complete before next CPU operation");
    if (vector->mode != ACEU_PRNG)
        check((fake_key_read(0x1Cu) & 0x3Fu) == 0u, "NEW_KEY lifetime");
    consume(in_block, out_block, 0);
    key_new = 0u;
    out_word = 0u;
    started++;
    regs[4] &= ~0x100u;
    if (vector->mode == ACEU_GCM && !vector->decrypt &&
        ((regs[1] >> 16) & 15u) == 1u && (regs[1] & 0x8000u) == 0u)
        gh_delay = 3u;
}

static unsigned long fake_read(unsigned int offset)
{
    if (offset == 0x10u)
    {
        if (dtf_pending_reads != 0u)
        {
            if (--dtf_pending_reads == 0u)
            {
                regs[4] |= 0x01000000u;
                dtf_active_reads = 3u;
            }
        }
        else if (dtf_active_reads != 0u && --dtf_active_reads == 0u)
            complete_dtf();
        if (gh_delay != 0u && !gh_stuck && --gh_delay == 0u) regs[4] |= 0x100u;
        return regs[4] | (hang ? 1u : 0u) | (io_error ? 0x1000u : 0u);
    }
    if (offset == 0x24u)
    {
        unsigned long value;
        check(out_word < 4u, "BCO read exactly once");
        value = load_be(out_block + (out_word % 4u) * 4u);
        out_word++;
        return value;
    }
    return regs[offset / 4u];
}

static void fake_write(unsigned int offset, unsigned long value)
{
    if (offset == 0x14u)
    {
        regs[4] &= ~value;
        return;
    }
    if (offset == 0x20u)
    {
        unsigned int destination = (unsigned int)((regs[0] >> 24) & 3u);
        store_be(in_block + in_word * 4u, value);
        if (++in_word == 4u)
        {
            in_word = 0u;
            if (destination == 0u) start_cpu();
            else if (destination == 2u)
            {
                if ((vector->mode == ACEU_GCM || vector->mode == ACEU_GMAC) &&
                    position != vector->start && vector->length != 0u && !vector->decrypt)
                    check(gh_delay == 0u, "GCM waits for GHASH before replacing J0");
                memcpy(iv_block, in_block, 16u);
            }
            else check(0, "PE must not write plaintext keys");
        }
        return;
    }
    check(in_word == 0u, "no register change inside four-word access");
    regs[offset / 4u] = value;
    if (offset == 0u)
    {
        /* Soft reset bits are self-clearing. */
        regs[0] &= ~0x2Fu;
        return;
    }
    if (offset == 0x60u)
    {
        /* Manual table 3.24: bit 16 starts DTF; writes to reserved bits do nothing. */
        if ((value & 0x00010000UL) == 0u) return;
        check((value & ~0x00010000UL) == 0u, "DTF reserved bits remain zero");
        check((dtf_end_eic & 0x1000u) == 0u, "clear stale DTF completion before start");
        check((dtf_end_eic & 0x80u) != 0u, "mask DTF interrupt while polling its pending flag");
        if (dtf_ignore_start) return;
        if (dtf_bus_error)
        {
            regs[4] |= 0x1000u;
            return;
        }
        if (dtf_defer) dtf_pending_reads = 2u;
        else complete_dtf();
    }
}

static void complete_dtf(void)
{
        unsigned int i, blocks = (unsigned int)regs[0x6Cu / 4u];
        unsigned char *input = (unsigned char *)regs[0x70u / 4u];
        unsigned char *output = (unsigned char *)regs[0x74u / 4u];
        unsigned int attr = (unsigned int)((regs[1] >> 16) & 15u);
        check((regs[0] & 0x10000000u) != 0u, "DTF selects BCO_DTF");
        check((regs[0] & 0x400u) == 0u, "DTF converts memory byte order");
        check((fake_key_read(0x1Cu) & 0x3Fu) == 0u, "DTF NEW_KEY lifetime");
        check(vector->mode != ACEU_MP && vector->mode != ACEU_PRNG, "unsupported DTF excluded");
        for (i = 0u; i < blocks; i++)
        {
            unsigned char result[16];
            consume(input + i * 16u, result, 1);
            if (output != NULL) memcpy(output + i * 16u, result, 16u);
            memcpy(out_block, result, 16u);
        }
        if (vector->mode == ACEU_GCM || vector->mode == ACEU_GMAC ||
            vector->mode == ACEU_CCM || vector->mode == ACEU_CMAC)
            check(position == end_position || steps[position].attr != attr,
                  "native DTF finalizes the complete authentication phase");
        if (vector->mode == ACEU_CMAC ||
            (vector->mode == ACEU_CCM && vector->decrypt && attr == 4u))
        {
            check((regs[0] & 0x100000u) != 0u, "suppress authentication DTF destination");
            out_word = 0u;
        }
        key_new = 0u;
        started += blocks;
        if (!dtf_suppress_end) dtf_end_eic |= 0x1000u;
        if (dtf_hold_run) regs[4] |= 0x01000000u;
        else regs[4] &= ~0x01000000u;
}

static void run_vectors(void)
{
    unsigned int index, i;
    for (index = 0u; index < sizeof(vectors) / sizeof(vectors[0]); index++)
    {
        aceu_request_t r;
        aceu_result_t result;
        unsigned char tag[16], iv[16];
        unsigned char *input = shared_ram + 0x1000u;
        unsigned char *output = shared_ram + 0x11000u;
        unsigned char *aad = shared_ram + 0x21000u;
        current_vector = index;
        vector = &vectors[index];
        position = vector->start;
        end_position = position + vector->count;
        memset(regs, 0, sizeof(regs));
        in_word = out_word = gh_delay = started = key_new = 0u;
        dtf_end_eic = 0x1045u;
        prng_new = prng_iv = 1u;
        for (i = 0u; i < 16u; i++) iv[i] = (unsigned char)i;
        for (i = 0u; i < vector->length; i++) input[i] = (unsigned char)i;
        for (i = 0u; i < vector->aad_length; i++) aad[i] = (unsigned char)(0xA0u + i);
        memset(output, 0xA5, vector->length + 16u);
        if (vector->decrypt && vector->mode <= ACEU_CCM && vector->mode != ACEU_GMAC)
        {
            unsigned int source_position = vector->start;
            for (i = 0u; i < vector->length; i += 16u)
            {
                const step_t *step;
                unsigned int bytes = vector->length - i;
                while (source_position < end_position)
                {
                    step = &steps[source_position++];
                    if ((vector->mode <= ACEU_OFB && step->attr == 0u) ||
                        (vector->mode == ACEU_XTS && step->attr != 1u) ||
                        (vector->mode == ACEU_GCM && step->attr == 1u) ||
                        (vector->mode == ACEU_CCM && step->attr == 2u)) break;
                }
                if (bytes > 16u) bytes = 16u;
                memcpy(input + i, step->input, bytes);
            }
        }
        memcpy(tag, vector->tag, sizeof(tag));
        if (vector->bad) tag[0] ^= 1u;
        memset(&r, 0, sizeof(r));
        r.mode = (aceu_mode_t)vector->mode;
        r.decrypt = vector->decrypt;
        r.key_slot = vector->mode == ACEU_MP ? 4u : vector->slot;
        r.tweak_slot = vector->slot == 2u ? 5u : 3u;
        r.input = input;
        r.output = output;
        r.length = vector->length;
        r.iv = iv;
        r.iv_length = vector->iv_length;
        r.aad = aad;
        r.aad_length = vector->aad_length;
        r.tag = tag;
        r.tag_length = vector->tag_length;
        r.prng_restart = vector->mode == ACEU_PRNG;
        result = R_ACEU_Execute(&r);
        check(result == (vector->bad ? ACEU_ERR_AUTH : ACEU_OK), "operation status");
        check(position == end_position, "all expected hardware blocks consumed");
        check(r.cpu_blocks + r.dtf_blocks == vector->count, "transfer accounting");
        if (!vector->decrypt && vector->length != 0u &&
            (vector->mode <= ACEU_CCM || vector->mode == ACEU_PRNG))
            check(memcmp(output, s_aceu_vectors[vector->case_index].expected, vector->length) == 0,
                  "complete independent ciphertext oracle");
        for (i = vector->length; i < vector->length + 16u; i++)
            check(output[i] == 0xA5u, "no output overrun");
        if (vector->bad && (vector->mode == ACEU_GCM || vector->mode == ACEU_CCM))
            for (i = 0u; i < vector->length; i++) check(output[i] == 0u, "erase unauthenticated plaintext");
        else if (vector->decrypt && vector->mode <= ACEU_CCM)
            for (i = 0u; i < vector->length; i++) check(output[i] == (unsigned char)i, "recovered plaintext");
        if (!vector->decrypt && vector->mode >= ACEU_CMAC && vector->mode != ACEU_PRNG)
            check(memcmp(tag, vector->tag, vector->tag_length) == 0, "MAC / MP oracle");
        if (!vector->decrypt && (vector->mode == ACEU_GCM || vector->mode == ACEU_CCM))
            check(memcmp(tag, vector->tag, vector->tag_length) == 0, "AEAD tag oracle");
        if (vector->length == 10240u && vector->mode != ACEU_MP && vector->mode != ACEU_PRNG)
            check(r.dtf_blocks != 0u, "10KB actually uses DTF");
    }
}

static void failures_and_bounds(void)
{
    aceu_request_t r;
    unsigned char input[32] = {0}, output[48], iv[16] = {0}, tag[16];
    memset(&r, 0, sizeof(r));
    r.input = input;
    r.output = output;
    r.iv = iv;
    r.iv_length = 16u;
    r.tag = tag;
    r.tag_length = 16u;
    r.length = 16u;
    check(R_ACEU_Execute(NULL) == ACEU_ERR_PARAM, "null request");
    r.length = 15u;
    check(R_ACEU_Execute(&r) == ACEU_ERR_PARAM, "ECB short block");
    r.length = 16u;
    r.output = input + 1u;
    check(R_ACEU_Execute(&r) == ACEU_ERR_PARAM, "partial overlap");
    r.output = output;
    r.mode = ACEU_CCM;
    r.iv_length = 12u;
    r.aad = input;
    r.aad_length = 65280u;
    check(R_ACEU_Execute(&r) == ACEU_ERR_PARAM, "CCM AAD upper bound");
    r.aad_length = 0u;
    r.mode = ACEU_XTS;
    r.iv_length = 16u;
    check(R_ACEU_Execute(&r) == ACEU_ERR_PARAM, "XTS duplicate keys");
    r.mode = ACEU_ECB;
    invalid_key = 1u;
    check(R_ACEU_Execute(&r) == ACEU_ERR_KEY, "invalid KEYRAM slot");
    invalid_key = 0u;
    s_active = 1u;
    check(R_ACEU_Execute(&r) == ACEU_ERR_BUSY, "busy engine");
    s_active = 0u;
    hang = 1u;
    check(R_ACEU_Execute(&r) == ACEU_ERR_TIMEOUT, "bounded hardware timeout");
    hang = 0u;
    check(R_ACEU_Execute(&r) == ACEU_ERR_FAULT, "timeout latches fault");
    s_faulted = 0u;
    io_error = 1u;
    check(R_ACEU_Execute(&r) == ACEU_ERR_HARDWARE, "AXI error explicit");
    io_error = 0u;
    check(R_ACEU_Execute(&r) == ACEU_ERR_FAULT, "AXI error latches fault");
    s_faulted = 0u;
    check(dtf_address(shared_ram, sizeof(shared_ram)), "DTF exact RAM range");
    check(!dtf_address(shared_ram + 1u, 16u), "DTF rejects unaligned memory");
    check(!dtf_address(shared_ram, sizeof(shared_ram) + 1u), "DTF rejects range overrun");
    check(!dtf_address(shared_ram + sizeof(shared_ram), 16u), "DTF rejects RAM end");
}

static void prng_continuation(void)
{
    unsigned int index, part, i;
    aceu_request_t r;
    unsigned char output[32];
    for (index = 0u; index < sizeof(vectors) / sizeof(vectors[0]); index++)
        if (vectors[index].mode == ACEU_PRNG && vectors[index].length == 10240u) break;
    vector = &vectors[index];
    position = vector->start;
    end_position = position + 4u;
    prng_new = prng_iv = 1u;
    memset(regs, 0, sizeof(regs));
    in_word = out_word = gh_delay = 0u;
    memset(&r, 0, sizeof(r));
    r.mode = ACEU_PRNG;
    r.key_slot = vector->slot;
    r.output = output;
    r.length = sizeof(output);
    for (part = 0u; part < 2u; part++)
    {
        r.prng_restart = part == 0u;
        prng_new = 1u;
        check(R_ACEU_Execute(&r) == ACEU_OK, "PRNG split request");
        for (i = 0u; i < 32u; i++)
            check(output[i] == steps[vector->start + part * 2u + i / 16u].output[i % 16u],
                  "PRNG split requests do not repeat the stream");
    }
}

static void dtf_control_failures(void)
{
    unsigned int scenario, i;
    aceu_request_t r;
    unsigned char *input = shared_ram + 0x1000u;
    unsigned char *output = shared_ram + 0x11000u;
    vector = &vectors[0];
    check(vector->mode == ACEU_ECB && vector->length == 32u, "two-block DTF fixture");
    for (scenario = 0u; scenario < 6u; scenario++)
    {
        aceu_result_t result;
        position = vector->start;
        end_position = position + vector->count;
        in_word = out_word = gh_delay = key_new = 0u;
        dtf_pending_reads = dtf_active_reads = 0u;
        dtf_ignore_start = scenario == 2u;
        dtf_defer = scenario == 1u;
        dtf_suppress_end = scenario == 3u;
        dtf_hold_run = scenario == 4u;
        dtf_bus_error = scenario == 5u;
        dtf_end_eic = 0x1045u;
        memset(regs, 0, sizeof(regs));
        memset(output, 0xA5, 32u);
        for (i = 0u; i < 32u; i++) input[i] = (unsigned char)i;
        memset(&r, 0, sizeof(r));
        r.mode = ACEU_ECB;
        r.input = input;
        r.output = output;
        r.length = 32u;
        s_faulted = 0u;
        if (scenario == 0u)
        {
            fake_write(0x60u, 1u);
            check(position == vector->start && dtf_end_eic == 0x1045u,
                  "writing bit 0 cannot start DTF or generate a new completion");
        }
        result = R_ACEU_Execute(&r);
        if (scenario <= 1u)
        {
            check(result == ACEU_OK, "fast and deferred DTF completion");
            check(r.dtf_blocks == 2u && r.cpu_blocks == 0u, "count only completed DTF blocks");
            check(memcmp(output, s_aceu_vectors[vector->case_index].expected, 32u) == 0,
                  "DTF ciphertext after proven completion");
            check((dtf_end_eic & ~0x1080u) == 0x45u, "preserve unrelated EIC fields");
        }
        else
        {
            check(result == (scenario == 5u ? ACEU_ERR_HARDWARE : ACEU_ERR_TIMEOUT),
                  "missing start/end or running transfer cannot succeed");
            check(r.dtf_blocks == 0u, "do not credit incomplete DTF blocks");
            check(R_ACEU_Execute(&r) == ACEU_ERR_FAULT, "failed DTF latches driver fault");
        }
    }
    dtf_ignore_start = dtf_defer = dtf_suppress_end = dtf_hold_run = dtf_bus_error = 0u;
    memset(regs, 0, sizeof(regs));
    s_faulted = 0u;
}

#define MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON 32
#define ER_OK 0u
#define ER_PARAM 1u
#define ER_MH_KEY_SYS_ERROR 2u
#define ER_MH_KEY_IN_USE 3u
static uint32_t provision_keyram[0x1810u / 4u], provision_regs[64];
static uint32_t provision_addresses[64], provision_count;
static uint32_t provision_fifo[16], provision_fifo_count, provision_previous;
static uint32_t *provision_last;
static uint32_t provision_key_writes;

static void provision_flush(void)
{
    if (provision_last != NULL && provision_previous == ACEU_BASE + 0x20u)
    {
        check(provision_fifo_count < 16u, "provision FIFO limit");
        provision_fifo[provision_fifo_count++ % 16u] = *provision_last;
    }
    provision_last = NULL;
}

static uint32_t *provision_ref(uint32_t address)
{
    uint32_t i;
    uint32_t *result;
    provision_flush();
    if (address >= ACEU_KEYRAM_BASE && address < ACEU_KEYRAM_BASE + sizeof(provision_keyram))
    {
        uint32_t offset = address - ACEU_KEYRAM_BASE;
        result = &provision_keyram[offset / 4u];
        if (offset < 0x1000u)
        {
            check(provision_keyram[(0x1000u + (offset / 32u) * 4u) / 4u] == 0u,
                  "invalidate slot before key write");
            provision_key_writes++;
        }
    }
    else
    {
        for (i = 0u; i < provision_count; i++)
            if (provision_addresses[i] == address) break;
        check(i < 64u, "provision register limit");
        if (i == provision_count) provision_addresses[provision_count++] = address;
        result = &provision_regs[i];
    }
    provision_last = result;
    provision_previous = address;
    return result;
}

#define ACEU_REG(address) (*provision_ref(address))
#include "aceu_provision.inc"
#undef ACEU_REG

static void provisioning(void)
{
    uint32_t slot, word, bits;
    uint32_t initial_writes;
    memset(provision_keyram, 0xA5, sizeof(provision_keyram));
    *provision_ref(0xFF0A8004UL) = 1u;
    for (bits = 128u; bits <= 256u; bits += 64u)
    {
        provision_fifo_count = 0u;
        check(MH_KEY_PrepareAceu(bits) == ER_OK, "ICUM provisioning status");
        provision_flush();
        check(*provision_ref(0xFF0A1508UL) == 0x80000100UL, "KEYRAM secure locked guard");
        check(*provision_ref(0xFF0A150CUL) == 0x04000000UL, "KEYRAM ICUM SPID only");
        check(*provision_ref(0xFF0A1510UL) == 0x80000180UL, "engine nonsecure locked guard");
        check(*provision_ref(0xFF0A1514UL) == 0x04000003UL, "engine PE SPIDs and ICUM only");
        check(*provision_ref(0xFF986820UL) == 0u, "ACEU clock enable");
        check(*provision_ref(0xFF986700UL) == 0xA5A5A500UL, "clock protection relocked");
        check(*provision_ref(0xFF0A1618UL) == 0xA5A5A500UL, "guard protection relocked");
        check(provision_keyram[0x180Cu / 4u] == 0u, "key management enabled and DTF nonsecure");
        check(provision_fifo_count == bits / 32u + 4u, "PRNG key and seed FIFO length");
        for (word = 0u; word < provision_fifo_count; word++)
        {
            uint32_t offset = word < bits / 32u ? word * 4u : (word - bits / 32u) * 4u;
            uint32_t expected = (offset << 24) | ((offset + 1u) << 16) |
                                ((offset + 2u) << 8) | (offset + 3u);
            check(provision_fifo[word] == expected, "PRNG key / seed byte order");
        }
        for (slot = 0u; slot < 128u; slot++)
        {
            uint32_t key_code = slot <= 2u ? slot : (slot == 5u ? 2u : 0u);
            uint32_t size = 16u + key_code * 8u;
            check(provision_keyram[(0x1000u + slot * 4u) / 4u] ==
                  (slot <= BOOT_AUTH_ACEU_SLOT ? 8u | (key_code << 8) : 0u), "KEYRAM slot flag");
            for (word = 0u; word < 8u; word++)
            {
                uint32_t expected = 0u;
                if (slot <= BOOT_AUTH_ACEU_SLOT && slot != 4u && word * 4u < size)
                {
                    uint32_t offset = size - 4u - word * 4u +
                        (slot == BOOT_AUTH_ACEU_SLOT ? 0xE0u : (slot >= 3u ? 0x20u : 0u));
                    expected = (offset << 24) | ((offset + 1u) << 16) |
                               ((offset + 2u) << 8) | (offset + 3u);
                }
                check(provision_keyram[slot * 8u + word] == expected, "KEYRAM data / zero padding");
            }
        }
    }
    initial_writes = provision_key_writes;
    check(MH_KEY_PrepareAceu(64u) == ER_PARAM, "invalid PRNG size");
    *provision_ref(ACEU_BASE + 0x10u) = 1u;
    check(MH_KEY_PrepareAceu(128u) == ER_MH_KEY_IN_USE, "do not rekey busy engine");
    check(provision_key_writes == initial_writes, "busy engine preserves keys");
    *provision_ref(ACEU_BASE + 0x10u) = 0u;
    *provision_ref(0xFF0A1508UL) = 0x80000180UL;
    check(MH_KEY_PrepareAceu(128u) == ER_MH_KEY_SYS_ERROR, "reject incompatible locked guard");
    check(provision_key_writes == initial_writes, "guard failure preserves keys");
}

int main(void)
{
    run_vectors();
    dtf_control_failures();
    failures_and_bounds();
    prng_continuation();
    provisioning();
    printf("ACEU driver: %lu checks, %u operations, %s (%u failures)\n",
           (unsigned long)checks, current_vector + 1u, failures ? "FAIL" : "PASS", failures);
    return failures ? 1 : 0;
}
