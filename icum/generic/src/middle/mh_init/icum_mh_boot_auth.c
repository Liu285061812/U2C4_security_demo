#include DEVICE_FILE
#include "boot_auth.h"
#include "boot_auth_demo_keys.h"
#include "icum_common_pub.h"
#include "icum_d_cry_aes_pub.h"
#include "icum_d_int_pub.h"
#include "icum_d_wdt_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_mh_key_pub.h"
#include "icum_ml_ctx_pub.h"

#define BOOT_UART_TIMEOUT (1000000UL)
#define BOOT_CLOCK_KEY (*(volatile uint32_t *)0xFF980700UL)
#ifndef BOOT_AUTH_READ_BLANK
#define BOOT_AUTH_READ_BLANK(address) (*(volatile const uint32_t *)(address))
#endif

#pragma alignvar(4)
static uint8_t s_boot_icum_key[16] = {BOOT_AUTH_ICUM_KEY_BYTES};
volatile uint32_t g_boot_auth_error;
volatile uint32_t g_boot_auth_stage;
volatile uint32_t g_boot_auth_address;
volatile uint32_t g_boot_auth_uart_step;

static int boot_uart_init(void)
{
    uint32_t timeout;
    g_boot_auth_uart_step = 1u;
    BOOT_CLOCK_KEY = 0xA5A5A501UL;
    SYSCTRLCKS_RLINC = 1u;
    for (timeout = BOOT_UART_TIMEOUT; timeout != 0u; timeout--)
    {
        D_WDT_Start();
        if ((SYSCTRLCKS_RLINS & 3u) == 1u) break;
    }
    BOOT_CLOCK_KEY = 0xA5A5A500UL;
    if (timeout == 0u) return 0;
    g_boot_auth_uart_step = 2u;
    SYSCTRLMSRKCPROT = 0xA5A5A501UL;
    SYSCTRLMSR_RLIN3 |= 0x80000000UL;
    SYSCTRLMSR_RLIN3 &= ~2UL;
    SYSCTRLMSRKCPROT = 0xA5A5A500UL;
    RLN31LUOER = 0u;
    RLN31LCUC = 0u;
    g_boot_auth_uart_step = 3u;
    for (timeout = BOOT_UART_TIMEOUT; timeout != 0u; timeout--)
    {
        D_WDT_Start();
        if (RLN31LMST == 0u) break;
    }
    if (timeout == 0u) return 0;
    RLN31LMD = 1u;
    RLN31LBFC = 0u;
    RLN31LEDE = 0u;
    RLN31LWBR = 0x50u;
    RLN31LBRP01 = (80000000UL / (115200UL * 6u)) - 1u;
    RLN31LCUC = 1u;
    g_boot_auth_uart_step = 4u;
    for (timeout = BOOT_UART_TIMEOUT; timeout != 0u; timeout--)
    {
        D_WDT_Start();
        if (RLN31LMST == 1u) break;
    }
    if (timeout == 0u) return 0;
    RLN31LUOER = 1u;
    g_boot_auth_uart_step = 5u;
    PORT0PKCPROT = 0xA5A5A501UL;
    PORT0PWE |= 0x400u;
    PORT0PIPC17 &= ~4u;
    PORT0PFC17 &= ~4u;
    PORT0PFCE17 |= 4u;
    PORT0PFCAE17 &= ~4u;
    PORT0PFCEAE17 &= ~4u;
    PORT0PMC17 |= 4u;
    PORT0PM17 &= ~4u;
    PORT0PWE &= ~0x400u;
    PORT0PKCPROT = 0xA5A5A500UL;
    for (timeout = 0x10000u; timeout != 0u; timeout--)
        D_WDT_Start();
    g_boot_auth_uart_step = 6u;
    return 1;
}

static int boot_uart_puts(const char *text)
{
    uint32_t timeout;
    g_boot_auth_uart_step = 7u;
    do
    {
        for (timeout = BOOT_UART_TIMEOUT; timeout != 0u; timeout--)
        {
            D_WDT_Start();
            if (RLN31UTS == 0u) break;
        }
        if (timeout == 0u) return 0;
        if (*text == '\0')
        {
            g_boot_auth_uart_step = 8u;
            return 1;
        }
        RLN31LUTDR = (uint8_t)*text++;
    } while (1);
}

static void boot_auth_halt(const char *message, uint32_t error)
{
    char code[] = " code=0x00000000\r\n";
    static const char digits[] = "0123456789ABCDEF";
    uint32_t i;
    g_boot_auth_error = error;
    g_boot_auth_stage = BOOT_AUTH_STAGE_FAILED;
    (void)D_INT_DisableInterrupts();
    for (i = 0u; i < 8u; i++)
        code[8u + i] = digits[(error >> ((7u - i) * 4u)) & 15u];
    if (!boot_uart_init() || !boot_uart_puts(message) || !boot_uart_puts(code))
        g_boot_auth_error |= 0x80000000UL;
    while (1)
        D_WDT_Start();
}

void BootAuth_VerifyIcum(void)
{
    uint32_t context[AES_CTX_SIZE] = {0u};
    uint32_t tag[4] = {0u};
    aes_init_t init = {0};
    aes_finish_t finish = {0};
    uint32_t i, result;

    g_boot_auth_error = ER_OK;
    g_boot_auth_stage = BOOT_AUTH_STAGE_BLANK;
    /* U2C4 manual 53.10.2: read-only BF mirrors, not FACI P/E commands. */
    for (i = 0u; i < sizeof(boot_auth_data_t); i += 4u)
    {
        D_WDT_Start();
        g_boot_auth_address = BOOT_AUTH_ADDRESS + i;
        if ((BOOT_AUTH_READ_BLANK(BOOT_AUTH_ADDRESS + 0x00200000UL + i) & 0x8000u) != 0u)
            boot_auth_halt("ICUM boot data missing/blank; PE blocked", ER_PARAM);
    }

    D_WDT_Start();
    g_boot_auth_stage = BOOT_AUTH_STAGE_CMAC;
    g_boot_auth_address = BOOT_AUTH_ADDRESS;
    init.cipher_mode = D_AES_CIPHER_MODE_CMAC;
    init.direction = D_AES_DIR_ENCRYPTION;
    init.p_aes_key = (uint32_t *)s_boot_icum_key;
    init.key_length = sizeof(s_boot_icum_key);
    init.p_context = context;
    result = D_AES_InitJob(&init);
    if (result != ER_OK)
        boot_auth_halt("ICUM boot CMAC init ERROR; PE blocked", result);
    finish.number = BOOT_AUTH_INPUT_BYTES / 16u;
    finish.last_block_in_bits = 128u;
    finish.first = D_AES_FIRST_CIPHER;
    finish.p_aes_src = (uint32_t *)BOOT_AUTH_DATA->input;
    finish.p_context = context;
    finish.p_aes_dest = tag;
    result = D_AES_FinishJob(&finish);
    if (result != ER_OK)
        boot_auth_halt("ICUM boot CMAC hardware ERROR; PE blocked", result);
    if (L_Toolbox_MemCmp((const uint8_t *)tag, BOOT_AUTH_DATA->icum_cmac,
                        BOOT_AUTH_TAG_BYTES) != 0)
        boot_auth_halt("ICUM boot CMAC FAIL; PE blocked", ER_PARAM);
    D_WDT_Start();
    g_boot_auth_stage = BOOT_AUTH_STAGE_KEY;
    result = MH_KEY_PrepareAceu(128u);
    if (result != ER_OK)
        boot_auth_halt("ICUM boot ACEU key provisioning ERROR; PE blocked", result);
    g_boot_auth_stage = BOOT_AUTH_STAGE_PASSED;
}
