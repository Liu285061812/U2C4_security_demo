#include <stdio.h>
#include <string.h>

#undef NULL
#include "r_aceu.h"
#include "boot_auth.h"

static void boot_auth_halt(aceu_result_t result)
{
    printf("PE ACEU boot CMAC FAIL: %s; halted\n", R_ACEU_ResultName(result));
    while (1);
}

void BootAuth_VerifyPe(void)
{
    aceu_request_t request;
    aceu_result_t result;
    uint8_t expected[BOOT_AUTH_TAG_BYTES];
    memset(&request, 0, sizeof(request));
    memcpy(expected, BOOT_AUTH_DATA->pe_cmac, sizeof(expected));
    request.mode = ACEU_CMAC;
    request.decrypt = 1u;
    request.key_slot = BOOT_AUTH_ACEU_SLOT;
    request.input = BOOT_AUTH_DATA->input;
    request.length = BOOT_AUTH_INPUT_BYTES;
    request.tag = expected;
    request.tag_length = sizeof(expected);
    result = R_ACEU_Execute(&request);
    if (result != ACEU_OK) boot_auth_halt(result);
    printf("PE ACEU boot CMAC PASS (10240 bytes)\n");
}
