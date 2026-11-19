#include <stdio.h>
#include <string.h>

#undef NULL
#include "pe1.h"
#include "shared.h"
#include "sys.api.h"
#include "r_aceu.h"
#include "vrun.api.h"

#define ACEU_TEST_BYTES (10u * 1024u)

typedef struct
{
    uint32_t mode, slot, length, aad_length, iv_length, tag_length;
    const uint8_t *expected;
    uint8_t tag[16];
} aceu_vector_t;

#include "pe1_aceu_vectors.h"

extern unsigned char VTIMER0_CLK;

static const char * const s_mode_names[] =
    {"ECB", "CBC", "CTR", "CFB128", "OFB", "GCM", "XTS", "CCM", "CMAC", "MP", "GMAC", "PRNG"};

static int prepare(uint32_t key_bits)
{
    r_icumif_isd_t *isd = (r_icumif_isd_t *)ISD_BUFFER;
    pe1_service_response_t response;
    memset(isd, 0, sizeof(*isd));
    isd->service_id = SERVICE_00_ACEU_PREPARE;
    isd->prm.ACEU_PREPARE.prng_key_bits = key_bits;
    trigger_service_and_wait_deferred(isd, &response);
    if (response.interface_error != R_ICUMIF_ER_OK || isd->service_result != SERV_OK)
    {
        printf("ACEU ICUM provisioning FAIL: interface=%ld service=%s\n",
               (long)response.interface_error, R_ICUMIF_GetErrorCodeName(isd->service_result));
        return 0;
    }
    return 1;
}

static aceu_result_t execute(aceu_request_t *r, const char *operation, int print)
{
    uint32_t start = (uint32_t)sys_vtimer0_get_pclk_counter();
    aceu_result_t result = R_ACEU_Execute(r);
    uint32_t ticks = (uint32_t)sys_vtimer0_get_pclk_counter() - start;
    if (print || result != ACEU_OK)
    {
        char throughput[48] = "";
        uint32_t bytes = r->mode == ACEU_GMAC ? r->aad_length : r->length;
        uint32_t key_bits = r->mode == ACEU_MP ? 128u : 128u + 64u * r->key_slot;
        uint32_t us = VTIMER0_CLK == 0u ? 0u : ticks / VTIMER0_CLK;
        if (VTIMER0_CLK == 0u)
        {
            printf("ACEU timer configuration ERROR (VTIMER0_CLK=0)\n");
            return ACEU_ERR_HARDWARE;
        }
        if (bytes != 0u && r->mode != ACEU_PRNG)
        {
            uint32_t rate = ticks == 0u ? 0u :
                (uint32_t)((uint64_t)bytes * VTIMER0_CLK * 1000u / ticks);
            if (ticks == 0u) strcpy(throughput, " throughput=n/a");
            else snprintf(throughput, sizeof(throughput), " throughput=%lu.%03lu MB/s",
                          (unsigned long)(rate / 1000u), (unsigned long)(rate % 1000u));
        }
        printf("ACEU %-6s %-7s key=%lu bytes=%lu result=%s time=%lu.%03lu %s%s CPU=%lu DTF=%lu\n",
               s_mode_names[r->mode], operation, (unsigned long)key_bits,
               (unsigned long)bytes, R_ACEU_ResultName(result),
               (unsigned long)(us / 1000u), (unsigned long)(us % 1000u),
               r->mode == ACEU_PRNG ? "ms/ops" : "ms", throughput,
               (unsigned long)r->cpu_blocks, (unsigned long)r->dtf_blocks);
    }
    return result;
}

static int run_vector(const aceu_vector_t *v)
{
    uint8_t iv[16], tag[16];
    uint8_t *input = BIG_BUFFER[0], *cipher = BIG_BUFFER[1], *plain = BIG_BUFFER[2];
    uint8_t *aad = v->mode == ACEU_GMAC ? BIG_BUFFER[2] : BIG_BUFFER[0] + ACEU_TEST_BYTES;
    uint32_t i;
    int payload = v->mode <= ACEU_CCM || v->mode == ACEU_PRNG;
    int show = v->length == ACEU_TEST_BYTES || v->aad_length == ACEU_TEST_BYTES;
    aceu_request_t r;
    if (v->mode == ACEU_PRNG && !prepare(128u + v->slot * 64u)) return 0;
    for (i = 0u; i < 16u; i++) iv[i] = (uint8_t)i;
    for (i = 0u; i < v->length; i++) input[i] = (uint8_t)i;
    for (i = 0u; i < v->aad_length; i++) aad[i] = (uint8_t)(0xA0u + i);
    memset(&r, 0, sizeof(r));
    memset(tag, 0, sizeof(tag));
    r.mode = (aceu_mode_t)v->mode;
    r.key_slot = v->slot;
    r.tweak_slot = v->slot == 2u ? ACEU_DEMO_TWEAK256 : ACEU_DEMO_TWEAK128;
    if (r.mode == ACEU_MP) r.key_slot = ACEU_DEMO_MP_ZERO;
    r.input = input;
    r.output = cipher;
    r.length = v->length;
    r.iv = iv;
    r.iv_length = v->iv_length;
    r.aad = aad;
    r.aad_length = v->aad_length;
    r.tag = tag;
    r.tag_length = v->tag_length;
    r.prng_restart = r.mode == ACEU_PRNG;
    if (execute(&r, "forward", show) != ACEU_OK) return 0;
    if (show && r.mode != ACEU_MP && r.mode != ACEU_PRNG && r.dtf_blocks == 0u)
    {
        printf("ACEU %s FAIL: 10KB test did not use DTF\n", s_mode_names[v->mode]);
        return 0;
    }
    if (payload)
    {
        for (i = 0u; i < v->length; i++)
            if (cipher[i] != v->expected[i])
            {
                printf("ACEU %s key-slot=%lu length=%lu independent oracle FAIL: output[%lu]=%02X expected=%02X\n",
                       s_mode_names[v->mode], (unsigned long)v->slot, (unsigned long)v->length,
                       (unsigned long)i, (unsigned int)cipher[i], (unsigned int)v->expected[i]);
                return 0;
            }
    }
    if ((!payload || r.mode == ACEU_GCM || r.mode == ACEU_CCM) &&
        memcmp(tag, v->tag, v->tag_length) != 0)
    {
        printf("ACEU %s key-slot=%lu length=%lu independent oracle FAIL: tag mismatch\n",
               s_mode_names[v->mode], (unsigned long)v->slot, (unsigned long)v->length);
        return 0;
    }
    if (r.mode == ACEU_MP || r.mode == ACEU_PRNG) return 1;
    r.decrypt = 1u;
    r.input = cipher;
    r.output = plain;
    if (!payload) r.input = input;
    if (execute(&r, payload ? "decrypt" : "verify", show) != ACEU_OK ||
        (payload && memcmp(plain, input, v->length) != 0))
    {
        printf("ACEU %s reverse verification FAIL\n", s_mode_names[v->mode]);
        return 0;
    }
    if (r.mode == ACEU_GCM || r.mode == ACEU_CCM || r.mode == ACEU_CMAC || r.mode == ACEU_GMAC)
    {
        aceu_result_t result;
        tag[0] ^= 1u;
        memset(plain, 0xA5, v->length);
        result = R_ACEU_Execute(&r);
        if (result != ACEU_ERR_AUTH)
        {
            printf("ACEU %s tampered tag FAIL: %s\n", s_mode_names[v->mode], R_ACEU_ResultName(result));
            return 0;
        }
        if (payload)
        {
            for (i = 0u; i < v->length; i++)
                if (plain[i] != 0u)
                {
                    printf("ACEU %s authentication failure left plaintext at %lu\n",
                           s_mode_names[v->mode], (unsigned long)i);
                    return 0;
                }
        }
    }
    return 1;
}

void aceu_test_command(int selection)
{
    uint32_t i, passed = 0u, failed = 0u;
    static const uint8_t selections[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 9, 11};
    if (selection < 0 || selection > 12)
    {
        printf("ACEU selection must be 0..12\n");
        return;
    }
    printf("ACEU tests: public DEMO keys; PRNG is deterministic, NOT an entropy source.\n");
    if (!prepare(128u)) return;
    for (i = 0u; i < sizeof(s_aceu_vectors) / sizeof(s_aceu_vectors[0]); i++)
    {
        const aceu_vector_t *v = &s_aceu_vectors[i];
        if (selection != 0 && v->mode != selections[selection - 1]) continue;
        if (run_vector(v)) passed++;
        else failed++;
    }
    printf("ACEU tests complete: PASS=%lu FAIL=%lu (includes KAT, tails and tampered tags)\n",
           (unsigned long)passed, (unsigned long)failed);
}
