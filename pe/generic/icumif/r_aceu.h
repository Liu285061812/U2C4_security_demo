#ifndef R_ACEU_H
#define R_ACEU_H

#include "renesas_types.h"

#define ACEU_BASE             0xFF1E8000UL
#define ACEU_KEYCTRL_BASE     0xFF1E9000UL
#define ACEU_KEYRAM_BASE      0xFF1EA000UL
#define ACEU_DEMO_AES128      0u
#define ACEU_DEMO_AES192      1u
#define ACEU_DEMO_AES256      2u
#define ACEU_DEMO_TWEAK128    3u
#define ACEU_DEMO_MP_ZERO     4u
#define ACEU_DEMO_TWEAK256    5u

typedef enum
{
    ACEU_ECB, ACEU_CBC, ACEU_CTR, ACEU_CFB128, ACEU_OFB,
    ACEU_GCM, ACEU_XTS, ACEU_CCM, ACEU_CMAC, ACEU_MP,
    ACEU_GMAC, ACEU_PRNG
} aceu_mode_t;

typedef enum
{
    ACEU_OK, ACEU_ERR_PARAM, ACEU_ERR_BUSY, ACEU_ERR_TIMEOUT,
    ACEU_ERR_KEY, ACEU_ERR_HARDWARE, ACEU_ERR_AUTH, ACEU_ERR_FAULT
} aceu_result_t;

typedef struct
{
    aceu_mode_t mode;
    uint32_t decrypt;
    uint32_t key_slot;
    uint32_t tweak_slot;
    const uint8_t *input;
    uint8_t *output;
    uint32_t length;
    const uint8_t *iv;
    uint32_t iv_length;
    const uint8_t *aad;
    uint32_t aad_length;
    uint8_t *tag;
    uint32_t tag_length;
    uint32_t prng_restart;
    uint32_t dtf_blocks;
    uint32_t cpu_blocks;
} aceu_request_t;

/* PE1 foreground only. Errors leave output invalid; AUTH errors erase plaintext. */
aceu_result_t R_ACEU_Execute(aceu_request_t *request);
const char *R_ACEU_ResultName(aceu_result_t result);
void aceu_test_command(int selection);

#endif
