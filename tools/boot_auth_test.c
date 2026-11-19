#include <stdio.h>
#include <string.h>
#include <setjmp.h>

#undef NULL
#include "r_aceu.h"
#define BOOT_AUTH_DATA (&test_data)
#include "boot_auth.h"
#include "boot_auth_demo_keys.h"
#include "icum_common_pub.h"
#include "icum_d_cry_aes_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_ml_cfg_pub.h"

#define AES_CTX_SIZE 17u
#define CLR 0u
#define SET 1u
#define NUM0 0u
#define TRNG_PRE_START_VALID 1u
#define RAND_NO_TEST 0u
#define TRNG_SHE_MODE 0u
#define ML_TRNG_SIZE_TEST 0u
#define ML_TRNG_SIZE_SHE 0u
#define ML_TRNG_SIZE_NIST 0u
#define SYSINFO_ON 1u
#define SYSINFO_OFF 0u
#define SYSINFO_START_PE 1u

extern const boot_auth_data_t g_boot_auth_data;
static boot_auth_data_t test_data;
static uint8_t s_boot_icum_key[16] = {BOOT_AUTH_ICUM_KEY_BYTES};
static jmp_buf halt_target;
static unsigned int failures, checks, released, provisioned, initialised, finished, pe_calls;
static unsigned int input_variant, blank_calls, blank_index, halt_stage, wdt_refreshes;
static uint32_t init_result, finish_result, provision_result;
static aceu_result_t pe_result;
volatile uint32_t g_boot_auth_stage, g_boot_auth_error, g_boot_auth_address, g_boot_auth_uart_step;

#define BOOT_UART_TIMEOUT 32u
#define BOOT_CLOCK_KEY uart_clock_key
static volatile uint32_t uart_clock_key, SYSCTRLCKS_RLINC, SYSCTRLCKS_RLINS;
static volatile uint32_t SYSCTRLMSRKCPROT, SYSCTRLMSR_RLIN3, PORT0PKCPROT, PORT0PWE;
static volatile uint16_t PORT0PIPC17, PORT0PFC17, PORT0PFCE17, PORT0PFCAE17;
static volatile uint16_t PORT0PFCEAE17, PORT0PMC17, PORT0PM17, RLN31LBRP01, RLN31LUTDR;
static volatile uint8_t RLN31LUOER, RLN31LCUC, RLN31LMST, RLN31LMD;
static volatile uint8_t RLN31LBFC, RLN31LEDE, RLN31LWBR, RLN31UTS;
static unsigned int uart_model, uart_scenario, fatal_model;

#include "boot_tags.inc"

static void check(int condition, const char *message)
{
    checks++;
    if (!condition)
    {
        failures++;
        printf("FAIL: %s\n", message);
    }
}

void D_WDT_Start(void)
{
    wdt_refreshes++;
    if (!uart_model) return;
    if (g_boot_auth_uart_step == 1u && uart_scenario != 1u) SYSCTRLCKS_RLINS = 1u;
    if (g_boot_auth_uart_step == 3u && uart_scenario != 2u) RLN31LMST = 0u;
    if (g_boot_auth_uart_step == 4u && uart_scenario != 3u) RLN31LMST = 1u;
    if (g_boot_auth_uart_step == 7u && uart_scenario != 4u) RLN31UTS = 0u;
    if (fatal_model && (g_boot_auth_uart_step == 8u ||
                       (g_boot_auth_error & 0x80000000UL) != 0u))
        longjmp(halt_target, 1);
}

uint32_t D_INT_DisableInterrupts(void) { return 0u; }

static uint32_t read_blank(uint32_t address)
{
    check(address == 0xFF43C000UL + blank_calls * 4u &&
          g_boot_auth_address == BOOT_AUTH_ADDRESS + blank_calls * 4u,
          "read blank mirror, not raw flash, for every programming unit");
    check(g_boot_auth_stage == BOOT_AUTH_STAGE_BLANK && wdt_refreshes > blank_calls,
          "blank scan refreshes watchdog and publishes its stage");
    return blank_calls++ == blank_index ? 0x807Fu : 0x7Fu;
}
#define BOOT_AUTH_READ_BLANK(address) read_blank(address)

uint32_t D_AES_InitJob(aes_init_t *p)
{
    static const uint8_t key[16] = {BOOT_AUTH_ICUM_KEY_BYTES};
    initialised++;
    check(g_boot_auth_stage == BOOT_AUTH_STAGE_CMAC, "CMAC stage is observable");
    check(blank_calls == sizeof(test_data) / 4u, "blank scan completes before CMAC");
    check(p->cipher_mode == D_AES_CIPHER_MODE_CMAC &&
          p->key_length == 16u && memcmp(p->p_aes_key, key, 16u) == 0,
          "ICUM uses its distinct CMAC key");
    return init_result;
}

uint32_t D_AES_FinishJob(aes_finish_t *p)
{
    finished++;
    check(initialised == 1u && released == 0u, "ICUM authenticates before releasing PE");
    check(p->number == BOOT_AUTH_INPUT_BYTES / 16u && p->last_block_in_bits == 128u &&
          p->first == D_AES_FIRST_CIPHER && (const void *)p->p_aes_src == test_data.input,
          "ICUM authenticates the complete 10KB plaintext");
    memcpy(p->p_aes_dest, calculated_tags[input_variant][0], 16u);
    return finish_result;
}

uint8_t L_Toolbox_MemCmp(const uint8_t *a, const uint8_t *b, int16_t size)
{
    return memcmp(a, b, size) != 0;
}

uint32_t MH_KEY_PrepareAceu(uint32_t bits)
{
    check(finished == 1u && released == 0u && bits == 128u,
          "provision ACEU only after ICUM CMAC succeeds");
    provisioned++;
    check(g_boot_auth_stage == BOOT_AUTH_STAGE_KEY, "key stage is observable");
    return provision_result;
}

static void icum_halt(const char *message, uint32_t error)
{
    check(message != NULL && error != ER_OK, "ICUM error is explicit");
    halt_stage = 1u;
    g_boot_auth_error = error;
    g_boot_auth_stage = BOOT_AUTH_STAGE_FAILED;
    longjmp(halt_target, 1);
}

static void pe_halt(aceu_result_t error)
{
    check(error != ACEU_OK, "PE error is explicit");
    halt_stage = 2u;
    longjmp(halt_target, 1);
}

void D_COMM_StartupPE(void)
{
    check(provisioned == 1u && finished == 1u && halt_stage == 0u &&
          g_boot_auth_stage == BOOT_AUTH_STAGE_PASSED,
          "PE release follows authentication and provisioning");
    released++;
}

uint32_t D_COMM_SetICUStatusInfo(icum_comm_info_icu_status_t flag, uint32_t value) { return ER_OK; }
uint32_t ML_CFG_GetSys(uint16_t index, uint32_t *output)
{
    memset(output, 0, sizeof(sys_cfg_rand_t));
    return ER_OK;
}
void ML_PRNG_GenTrng(uint32_t mode, uint32_t size) {}

uint32_t D_COMM_GetICUStatusInfo(uint32_t *status)
{
    *status = 0u;
    return ER_OK;
}

uint32_t ML_CFG_GetSys_ram(uint16_t index, uint32_t *output)
{
    *(uint8_t *)output = 1u;
    return ER_OK;
}

static void mh_SYSINFO_SendSysInterrupt(void) {}
static uint32_t mh_SYSINFO_LoopInfinity(void)
{
    halt_stage = 3u;
    longjmp(halt_target, 1);
    return ER_OK;
}

aceu_result_t R_ACEU_Execute(aceu_request_t *r)
{
    pe_calls++;
    check(released == 1u && provisioned == 1u, "PE check runs after ICUM boot gate");
    check(r->mode == ACEU_CMAC && r->decrypt == 1u && r->key_slot == BOOT_AUTH_ACEU_SLOT &&
          r->length == BOOT_AUTH_INPUT_BYTES && r->input == test_data.input &&
          r->tag_length == 16u, "ACEU verifies all input bytes with the separate PE key");
    if (pe_result != ACEU_OK) return pe_result;
    return memcmp(r->tag, calculated_tags[input_variant][1], 16u) == 0 ? ACEU_OK : ACEU_ERR_AUTH;
}

#define boot_auth_halt icum_halt
#include "boot_icum.inc"
#undef boot_auth_halt
#define boot_auth_halt pe_halt
#include "boot_pe.inc"
#undef boot_auth_halt
#include "boot_uart.inc"
#include "boot_fatal.inc"

static void reset(void)
{
    memcpy(&test_data, &g_boot_auth_data, sizeof(test_data));
    released = provisioned = initialised = finished = pe_calls = input_variant = blank_calls = halt_stage = 0u;
    wdt_refreshes = uart_model = 0u;
    g_boot_auth_stage = BOOT_AUTH_STAGE_RESET;
    g_boot_auth_error = ER_OK;
    blank_index = ~0u;
    init_result = finish_result = provision_result = ER_OK;
    pe_result = ACEU_OK;
}

int main(void)
{
    unsigned int scenario, expected_halt;
    static const unsigned int offsets[] = {0u, BOOT_AUTH_INPUT_BYTES / 2u, BOOT_AUTH_INPUT_BYTES - 1u};
    for (scenario = 0u; scenario < 17u; scenario++)
    {
        reset();
        expected_halt = scenario == 0u ? 0u :
            (scenario == 5u || (scenario >= 12u && scenario <= 15u) ? 2u : 1u);
        if (scenario >= 1u && scenario <= 3u)
        {
            input_variant = scenario;
            test_data.input[offsets[scenario - 1u]] ^= 1u;
        }
        if (scenario == 4u) test_data.icum_cmac[0] ^= 1u;
        if (scenario == 5u) test_data.pe_cmac[15] ^= 1u;
        if (scenario == 6u) blank_index = 0u;
        if (scenario == 7u) blank_index = sizeof(test_data) / 4u - 1u;
        if (scenario == 8u) blank_index = sizeof(test_data) / 8u;
        if (scenario == 9u) init_result = ER_PARAM;
        if (scenario == 10u) finish_result = ER_PARAM;
        if (scenario == 11u) provision_result = ER_PARAM;
        if (scenario == 12u) pe_result = ACEU_ERR_HARDWARE;
        if (scenario == 16u) test_data.icum_cmac[15] ^= 1u;
        if (setjmp(halt_target) == 0)
        {
            check(MH_INIT_StartPE() == ER_OK, "normal boot status");
            if (scenario >= 13u && scenario <= 15u)
            {
                input_variant = scenario - 12u;
                test_data.input[offsets[scenario - 13u]] ^= 1u;
            }
            BootAuth_VerifyPe();
        }
        check(halt_stage == expected_halt, "correct stage rejects corrupted data/tags and hardware failures");
        check(released == (expected_halt == 1u ? 0u : 1u), "failed ICUM verification never releases PE");
        check(pe_calls == (expected_halt == 1u ? 0u : 1u), "failed ICUM verification never reaches PE check");
    }
    for (scenario = BOOT_AUTH_STAGE_RESET; scenario <= BOOT_AUTH_STAGE_FAILED; scenario++)
    {
        reset();
        g_boot_auth_stage = scenario;
        if (scenario == BOOT_AUTH_STAGE_PASSED) provisioned = finished = 1u;
        if (setjmp(halt_target) == 0) (void)mh_SYSINFO_Interrupt(SYSINFO_ON);
        check(halt_stage == 3u, "system-error path still halts");
        check(released == (scenario == BOOT_AUTH_STAGE_PASSED ? 1u : 0u),
              "system errors cannot bypass mandatory boot authentication");
    }
    uart_model = 1u;
    for (uart_scenario = 0u; uart_scenario < 5u; uart_scenario++)
    {
        int ready;
        wdt_refreshes = 0u;
        SYSCTRLCKS_RLINS = 0u;
        RLN31LMST = 2u;
        RLN31UTS = 1u;
        ready = boot_uart_init();
        check(ready == (uart_scenario == 0u || uart_scenario == 4u),
              "UART initialization success or bounded clock/reset timeout");
        check(wdt_refreshes != 0u, "watchdog is refreshed during UART initialization waits");
        check(uart_clock_key == 0xA5A5A500UL, "clock protection is relocked on timeout");
        if (ready)
        {
            wdt_refreshes = 0u;
            check(boot_uart_puts("ERROR") == (uart_scenario == 0u),
                  "UART transmission success or bounded busy timeout");
            check(wdt_refreshes >= 5u, "watchdog is refreshed during UART transmit waits");
        }
    }
    fatal_model = 1u;
    for (scenario = 0u; scenario < 3u; scenario++)
    {
        uart_scenario = scenario == 2u ? 4u : scenario;
        g_boot_auth_uart_step = 0u;
        g_boot_auth_error = ER_OK;
        g_boot_auth_stage = BOOT_AUTH_STAGE_CMAC;
        SYSCTRLCKS_RLINS = 0u;
        RLN31LMST = 2u;
        RLN31UTS = 1u;
        if (setjmp(halt_target) == 0) boot_auth_halt("ERROR", ER_PARAM);
        check(g_boot_auth_stage == BOOT_AUTH_STAGE_FAILED,
              "real fatal handler latches failed state before UART access");
        check(g_boot_auth_error == (ER_PARAM | (scenario == 0u ? 0u : 0x80000000UL)),
              "real fatal handler preserves original error and flags UART timeout");
    }
    printf("Boot authentication: 17 boot, 6 exception, 5 UART, 3 fatal scenarios, %u checks, %s (%u failures)\n",
           checks, failures ? "FAIL" : "PASS", failures);
    return failures ? 1 : 0;
}
