#ifndef BOOT_AUTH_H
#define BOOT_AUTH_H

#include "renesas_types.h"

#define BOOT_AUTH_ADDRESS       (0xFF23C000UL)
#define BOOT_AUTH_INPUT_BYTES   (10u * 1024u)
#define BOOT_AUTH_TAG_BYTES     (16u)
#define BOOT_AUTH_ACEU_SLOT     (6u)
#define BOOT_AUTH_STAGE_RESET   (0u)
#define BOOT_AUTH_STAGE_BLANK   (1u)
#define BOOT_AUTH_STAGE_CMAC    (2u)
#define BOOT_AUTH_STAGE_KEY     (3u)
#define BOOT_AUTH_STAGE_PASSED  (4u)
#define BOOT_AUTH_STAGE_FAILED  (5u)

extern volatile uint32_t g_boot_auth_stage;

typedef struct
{
    uint8_t input[BOOT_AUTH_INPUT_BYTES];
    uint8_t icum_cmac[BOOT_AUTH_TAG_BYTES];
    uint8_t pe_cmac[BOOT_AUTH_TAG_BYTES];
} boot_auth_data_t;

#ifndef BOOT_AUTH_DATA
#define BOOT_AUTH_DATA ((const boot_auth_data_t *)BOOT_AUTH_ADDRESS)
#endif

void BootAuth_VerifyIcum(void);
void BootAuth_VerifyPe(void);

#endif
