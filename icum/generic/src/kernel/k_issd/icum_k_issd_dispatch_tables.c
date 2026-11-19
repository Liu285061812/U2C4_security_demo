/******************************************************************************/
/* Component Name   ISSD (ICU-M Start-up & Service Dispatcher)                */
/******************************************************************************/
/* Product      : ICU-M Low Level Driver (Demo Part)                          */
/******************************************************************************/
/***********************************************************************************************************************
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
* 
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* www.renesas.com/disclaimer
*
* Copyright (C) 2021-2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/* Header files                     */
#include "renesas_types.h"
#include "r_icumif_api.h"
#include "icum_common_pub.h"
#include ICUM_DEVICE_FILE
#include "icum_d_misc_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_d_int_pub.h"
#include "icum_d_info_pub.h"
#include "icum_k_issd_pub.h"
#include "icum_s_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mh_sysinfo_pub.h"
#include "icum_k_issd.h"
#include "icum_mh_key_pub.h"

/***************************************************************************************
* Service Configuration                                                                *
***************************************************************************************/

#define S_00_EccTest    NULL

static void S_00_PrepareAceu(icum_job_t *p_job)
{
    uint32_t result = MH_KEY_PrepareAceu(p_job->p_ISD->prm.ACEU_PREPARE.prng_key_bits);
    if (result == ER_OK) p_job->p_ISD->service_result = SERV_OK;
    else if (result == ER_PARAM) p_job->p_ISD->service_result = SERV_PRM_ERROR;
    else if (result == ER_MH_KEY_IN_USE) p_job->p_ISD->service_result = SERV_UNAVAILABLE;
    else p_job->p_ISD->service_result = SERV_SYS_ERROR;
}

#define TEST_GPIO_DOUT0  (0x00000001UL)
#define TEST_GPIO_EOUT0  (0x00000002UL)
#define TEST_GPIO_CTLEN0 (0x00000004UL)

static bool s_test_gpio_ready;
static uint32_t s_test_gpio_active_count;

/***************************************************************************************
* Global Variable                                                                      *
***************************************************************************************/
#pragma ghs section rodata = ".icum_kernel_const_ram"

/* Service Group #00 */
static service_t g_ism_table_00[3] =
{
    /* SERVICE ID                            Function                       Function at RAM execution       High priority Function  Permission   */
    { /* SERVICE_00_ECC_TEST */              S_00_EccTest,                  S_00_EccTest,                   NULL,                   (ISSD_PERMIT_NORMAL | ISSD_PERMIT_RAMEXE | ISSD_PERMIT_ADMIN) },
    { /* reserved 0x01 */                    NULL,                          NULL,                           NULL,                   0u },
    { /* SERVICE_00_ACEU_PREPARE */          S_00_PrepareAceu,               NULL,                           NULL,                   ISSD_PERMIT_NORMAL },
};

/* Service Group #01: AES */
static service_t g_ism_table_01[3] =
{
    { /* SERVICE_01_AES_CIPHER */            S_01_CipherAes,                NULL,                           NULL,                   (ISSD_PERMIT_NORMAL | ISSD_PERMIT_ADMIN) },
    { /* SERVICE_01_AES_AUTH_CIPHER */       S_01_AuthenticateAes,          NULL,                           NULL,                   (ISSD_PERMIT_NORMAL | ISSD_PERMIT_ADMIN) },
    { /* SERVICE_01_AES_CMAC */              S_01_ProcessAesCmac,           NULL,                           NULL,                   (ISSD_PERMIT_NORMAL | ISSD_PERMIT_ADMIN) },
};

/* Service Group #02: RSA */
static service_t g_ism_table_02[5] =
{
    { /* SERVICE_02_RSA_VERIFY */            S_01_VerifyRsaSignature,       NULL,                           NULL,                   (ISSD_PERMIT_NORMAL | ISSD_PERMIT_ADMIN) },
    { /* SERVICE_02_RSA_SIGN */              S_01_MakeRsaSignature,         NULL,                           NULL,                   (ISSD_PERMIT_NORMAL) },
    { /* SERVICE_02_RSA_ENCRYPT */           S_01_EncryptByRsa,             NULL,                           NULL,                   (ISSD_PERMIT_NORMAL) },
    { /* SERVICE_02_RSA_DECRYPT */           S_01_DecryptByRsa,             NULL,                           NULL,                   (ISSD_PERMIT_NORMAL) },
    { /* SERVICE_02_RSA_KEY_GENERATE */      S_03_GenerateRsaKey,           NULL,                           NULL,                   (ISSD_PERMIT_NORMAL) }
};

/* Service Group #03: random number */
static service_t g_ism_table_03[3] =
{
    { /* SERVICE_03_RAND_INIT_RNG */         S_02_RAND_InitRng,             NULL,                           NULL,                   (ISSD_PERMIT_NORMAL | ISSD_PERMIT_ADMIN) },
    { /* SERVICE_03_RAND_EXTEND_SEED */      S_02_RAND_ExtendSeed,          NULL,                           NULL,                   (ISSD_PERMIT_NORMAL | ISSD_PERMIT_ADMIN) },
    { /* SERVICE_03_RAND_RND */              S_02_RAND_GenerateRnd,         NULL,                           NULL,                   (ISSD_PERMIT_NORMAL | ISSD_PERMIT_ADMIN) }
};

/* Service Group #04: ECC, ECDSA, ECDH and EdDSA */
static service_t g_ism_table_04[6] =
{
    { /* SERVICE_04_ECDSA_VERIFY */          S_01_VerifyEcdsaSignature,     NULL,                           NULL,                   (ISSD_PERMIT_NORMAL | ISSD_PERMIT_ADMIN) },
    { /* SERVICE_04_ECDSA_SIGN */            S_01_MakeEcdsaSignature,       NULL,                           NULL,                   (ISSD_PERMIT_NORMAL) },
    { /* SERVICE_04_ECDH */                  S_01_MakeEcdhSecretKey,        NULL,                           NULL,                   (ISSD_PERMIT_NORMAL) },
    { /* SERVICE_04_EDDSA_VERIFY */          S_01_VerifyEddsaSignature,     NULL,                           NULL,                   (ISSD_PERMIT_NORMAL | ISSD_PERMIT_ADMIN) },
    { /* SERVICE_04_EDDSA_SIGN */            S_01_MakeEddsaSignature,       NULL,                           NULL,                   (ISSD_PERMIT_NORMAL) },
    { /* SERVICE_04_ECC_KEY_GENERATE */      S_03_GenerateEccKey,           NULL,                           NULL,                   (ISSD_PERMIT_NORMAL) }
};

/* Service Group #05: hash and HMAC */
static service_t g_ism_table_05[3] =
{
    { /* SERVICE_05_HASH */                  S_01_GenerateHash,             NULL,                           NULL,                   (ISSD_PERMIT_NORMAL | ISSD_PERMIT_ADMIN) },
    { /* SERVICE_05_HMAC */                  S_01_ProcessHmac,              NULL,                           NULL,                   (ISSD_PERMIT_NORMAL) },
    { /* SERVICE_20_CR */                    S_20_CR_ExecuteProcess,        NULL,                           NULL,                   (ISSD_PERMIT_NORMAL) }
};

/* MAX ISM Table definition */
#define MAX_ISM_TABLE_00 (sizeof(g_ism_table_00)/sizeof(service_t))
#define MAX_ISM_TABLE_01 (sizeof(g_ism_table_01)/sizeof(service_t))
#define MAX_ISM_TABLE_02 (sizeof(g_ism_table_02)/sizeof(service_t))
#define MAX_ISM_TABLE_03 (sizeof(g_ism_table_03)/sizeof(service_t))
#define MAX_ISM_TABLE_04 (sizeof(g_ism_table_04)/sizeof(service_t))
#define MAX_ISM_TABLE_05 (sizeof(g_ism_table_05)/sizeof(service_t))

/* Beginning of each service group */
static service_t *g_ism_table_begin[6] =
{
    g_ism_table_00,
    g_ism_table_01,
    g_ism_table_02,
    g_ism_table_03,
    g_ism_table_04,
    g_ism_table_05,
};

/* MAX Service Group definition */
#define MAX_ISM_GROUP (sizeof(g_ism_table_begin)/sizeof(service_t*))

static uint8_t g_max_ism_index[6] =
{
    MAX_ISM_TABLE_00,
    MAX_ISM_TABLE_01,
    MAX_ISM_TABLE_02,
    MAX_ISM_TABLE_03,
    MAX_ISM_TABLE_04,
    MAX_ISM_TABLE_05,
};
#pragma ghs section rodata = default

/**************************************************************************************/
/* Local definition                                                                   */
/**************************************************************************************/

#pragma ghs section text = ".icum_kernel_ram"
#pragma ghs section rodata = ".icum_kernel_const_ram"

void k_ISSD_InitTestGpio(void)
{
    uint32_t enabled;
    uint32_t gpio;

    s_test_gpio_ready = false;
    s_test_gpio_active_count = 0u;
    if ((D_INFO_GetInfo(INFO_ICUMOPGPIO, &enabled) == ER_OK) && (enabled == 1u))
    {
        /* Set DOUT0/CTLEN0 before enabling output (hardware manual section 1.15.4.1). */
        gpio = (ICUM_CTLREGICUM_GPIO & ~(TEST_GPIO_DOUT0 | TEST_GPIO_EOUT0)) | TEST_GPIO_CTLEN0;
        ICUM_CTLREGICUM_GPIO = gpio;
        SYNC_MEMORY();
        ICUM_CTLREGICUM_GPIO = gpio | TEST_GPIO_EOUT0;
        SYNC_MEMORY();
        s_test_gpio_ready = true;
    }
}

bool K_ISSD_AlgorithmGpioReady(void)
{
    return s_test_gpio_ready;
}

void K_ISSD_BeginAlgorithm(icum_job_t *p_job)
{
    uint32_t saved_psw;

    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);
    if (p_job->algorithm_active == 0u)
    {
        p_job->algorithm_active = 1u;
        if (s_test_gpio_active_count == 0u)
        {
            ICUM_CTLREGICUM_GPIO |= TEST_GPIO_DOUT0;
            SYNC_MEMORY();
        }
        s_test_gpio_active_count++;
    }
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
}

void K_ISSD_EndAlgorithm(icum_job_t *p_job)
{
    uint32_t saved_psw;

    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);
    if (p_job->algorithm_active != 0u)
    {
        p_job->algorithm_active = 0u;
        s_test_gpio_active_count--;
        if (s_test_gpio_active_count == 0u)
        {
            ICUM_CTLREGICUM_GPIO &= ~TEST_GPIO_DOUT0;
            SYNC_MEMORY();
        }
    }
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0028]:[ICUMFW_UD_K_ISSD_0028]
* Function name: k_ISSD_GetServiceDispatch
* Description  : Retrieve a service function from the dispatch table
* Arguments    : (IN) : service_id
*                (IN) : id_in_group
*                (OUT) : p_service_func_t *p_serv_func
* Return Value : none
***************************************************************************************/
static uint32_t k_ISSD_GetServiceDispatch(r_icumif_service_id_t service_id, p_service_func_t *p_serv_func)
{
    uint32_t            permission;
    uint8_t             ram_state;
    p_service_func_t   p_func = NULL;
    uint32_t rtn;
    uint32_t run_mode;
    uint32_t result = ER_OK;
    uint32_t service_temp;
    uint32_t service_group;
    uint32_t id_in_group;

    service_temp = (uint32_t)service_id;
    /* Get service group */
    service_group = ((service_temp >> 4u) & 0x000000FFuL);
    /* Get service ID */
    id_in_group    = (service_temp & 0x0000000FuL);
    /* Initialize a service permission */
    permission = 0uL;

    /* Range check of service group */
    if(MAX_ISM_GROUP <= service_group)
    {
        /* Out of range service group */
        result = ER_SERV_ID_ERROR;
    }
    /* Range check of service id in group */
    else if((uint32_t)g_max_ism_index[service_group] <= id_in_group)
    {
        /* Out of range service id in group */
        result = ER_SERV_ID_ERROR;
    }
    else
    {
        permission = ISSD_PERMIT_NORMAL;
    }

    if (ER_OK == result)
    {
        /* get of RAM execution state */
        rtn = MH_SYSINFO_GetRamExeState(&ram_state);
        if(ER_OK != rtn)
        {
            result = ER_SERV_UNAVAILABLE;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK == result)
    {
        if((uint32_t)SYSINFO_STATE_ON == (uint32_t)ram_state)
        {
            /* Function limit corresponding to RAM execution mode */
            permission |= ISSD_PERMIT_RAMEXE;
        }
        else
        {
            /* Do nothing */
        }

        /* secure boot verify failed and sanction ADMIN */
        k_ISSD_GetRunMode(&run_mode);
        if (ISSD_MODE_VERIFY_BOOT_2ND == run_mode)
        {
            /* Function limit corresponding to sanction ADMIN */
            permission |= ISSD_PERMIT_ADMIN;
        }
        else
        {
            /* Do nothing */
        }

        /* Check service execution permit */
        if ((permission & g_ism_table_begin[service_group][id_in_group].permission) == permission)
        {
            if((uint32_t)SYSINFO_STATE_ON == (uint32_t)ram_state)
            {
                p_func = g_ism_table_begin[service_group][id_in_group].p_service_func_ram;
            }
            else
            {
                p_func = g_ism_table_begin[service_group][id_in_group].p_service_func;
            }
            if(NULL == p_func)
            {
                /* Not supported */
                result = ER_SERV_ID_ERROR;
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Conditions for service execution not satisfied */
            result = ER_SERV_UNAVAILABLE;
        }
    }
    else
    {
        /* Do nothing */
    }

    *p_serv_func = p_func;
    return result;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0029]:[ICUMFW_UD_K_ISSD_0029]
* Function name: k_ISSD_GetExpansionDispatch
* Description  : Retrieve a service function from CISM table
* Arguments    : (IN) : service_id
*                (OUT) : p_service_func_t *p_serv_func
* Return Value : ER_OK/ER_SERV_ID_ERROR/ER_SERV_UNAVAILABLE/ER_SERV_SYS_ERROR
***************************************************************************************/
static uint32_t k_ISSD_GetExpansionDispatch(r_icumif_service_id_t service_id, p_service_func_t *p_serv_func)
{
    (void)service_id;
    if (NULL == p_serv_func)
    {
        return ER_PARAM;
    }
    *p_serv_func = NULL;
    return ER_SERV_ID_ERROR;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0027]:[ICUMFW_UD_K_ISSD_0027]
* Function name: k_ISSD_GetServiceFunc
* Description  : Retrieve a service function from the dispatch table
* Arguments    : (IN) : service_id
*                (OUT) p_service_func_t *p_serv_func
* Return Value : none
***************************************************************************************/
uint32_t k_ISSD_GetServiceFunc(r_icumif_service_id_t in_service_id, p_service_func_t *p_serv_func)
{
    p_service_func_t   p_func;
    uint32_t result;

    /* Check expansion service */
    if(EXPANSION_SERVICE_BIT == (in_service_id & EXPANSION_SERVICE_BIT))
    {
        result = k_ISSD_GetExpansionDispatch(in_service_id, &p_func);
    }
    else
    /* Native service */
    {
        result = k_ISSD_GetServiceDispatch(in_service_id, &p_func);
    }
    /* Return function address */
    *p_serv_func = p_func;
    return result;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0030]:[ICUMFW_UD_K_ISSD_0030]
* Function name: k_ISSD_HighestDispatch
* Description  : Dispatcher of highest priority service request
* Arguments    : (IN) : service_id
*                (OUT) : *p_job_exec
*                        1 : execution result is success or failure
*                        0 : execution result is pending or processing
* Return Value : none
***************************************************************************************/
void k_ISSD_HighestDispatch(r_icumif_isd_t *p_ISD, bool *p_job_exec)
{
    uint32_t service_temp;
    uint32_t service_group = MAX_ISM_GROUP;
    uint32_t id_in_group;
    p_high_priority_func_t  p_func;
    bool result = (bool)false;

    /* Check expansion service */
    if(EXPANSION_SERVICE_BIT == (p_ISD->service_id & EXPANSION_SERVICE_BIT))
    {
        /* Expansion service are not covered */
    }
    else
    {
        service_temp = (uint32_t)p_ISD->service_id;
        service_group = ((service_temp >> 4u) & 0x0000000FuL);
        id_in_group    = (service_temp & 0x0000000FuL);

        /* Range check of service group */
        if(MAX_ISM_GROUP <= service_group)
        {
            /* Out of range service group */
            service_group = MAX_ISM_GROUP;
        }
        else
        {
            /* Range check of service id in group */
            if((uint32_t)g_max_ism_index[service_group] <= id_in_group)
            {
                /* Out of range service id in group */
                service_group = MAX_ISM_GROUP;
            }
            else
            {
                /* Do nothing */
            }
        }
    }

    if (MAX_ISM_GROUP > service_group)
    {
        p_func = g_ism_table_begin[service_group][id_in_group].p_high_priority_func;
        if(NULL != p_func)
        {
            p_func(p_ISD);
            /* execution result is success or failure */
            if((SERV_REQUEST > p_ISD->service_result) ||
            (SERV_IN_PROCESS < p_ISD->service_result))
            {
                /* convert undefined result code to SYSTEM ERROR */
                if(SERV_IN_PROCESS < p_ISD->service_result)
                {
                    p_ISD->service_result = SERV_SYS_ERROR;
                }
                else
                {
                    /* Do nothing */
                }
                result = (bool)true;
            }
            /* execution result is pending or processing */
            else
            {
                /* update result for normal dispatch */
                p_ISD->service_result = SERV_REQUEST;
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    *p_job_exec = result;
    return;
}

#pragma ghs section text    = default
#pragma ghs section rodata  = default
