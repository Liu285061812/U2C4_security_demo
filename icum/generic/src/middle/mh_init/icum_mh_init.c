/******************************************************************************/
/* Component Name  INIT Manager                                               */
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
/**************************************************************************************/
/* Header files                                                                       */
/**************************************************************************************/
/* PE I/F */
#include "r_icumif_api.h"

/* COMMOM */
#include "icum_common_pub.h"
#include "icum_irq_define_pub.h"
#include "renesas_types.h"

/* DRIVER */
#include "icum_d_comm_pub.h"
#include "icum_d_ecc_pub.h"
#include "icum_d_int_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_ost_pub.h"
#include "icum_d_wdt_pub.h"
#include "icum_d_cr_pub.h"
#include "icum_d_clk_pub.h"
#include "icum_d_flash_pub.h"
#include "icum_d_mpu_pub.h"
#include "icum_d_cry_aes_pub.h"
#include "icum_d_info_pub.h"
#include "icum_d_trng_pub.h"
#include "icum_d_reset_pub.h"

#if defined(FSW_REMAP_DRIVER)
/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"
#endif

/* LIBRARY */
#include "icum_l_toolbox_pub.h"

/* MIDDLE */
#include "icum_mh_init_pub.h"
#include "icum_mh_key_pub.h"
#include "icum_mh_sysinfo_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mm_cr_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include "icum_ml_cfg_pub.h"
#include "icum_ml_ctx_pub.h"
#include "icum_ml_dvc_pub.h"
#include "icum_ml_prng_pub.h"
#include "icum_mh_mydata_pub.h"

/* SERVICE */
#include "icum_s_02_rand_pub.h"
#include "icum_s_20_cr_pub.h"
#include "icum_s_pub.h"

/* KERNEL */
#include "icum_k_issd_pub.h"

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
#define SET                         (0x00000001uL)
#define CLR                         (0x00000000uL)

#define EXT_DEBUGGER_CONNECT        (0x00000001uL)
#define TRNG_PRE_START_VALID        (0x00000001uL)

#define TRNG_SHE_MODE               (0x00000000uL)
#define TRNG_CTR_DRBG_MODE          (0x00000001uL)
#define TRNG_OLT_MODE               (0x00000004uL)
#define TRNG_OLT_ENABLE             (0x00000001uL)

#define AUTH_SIZE                   (4u)
#define HEADER_TAG_SIZE             (3u)
#define AUTH_ENABLE                 (0x00000001uL)
#define AUTH_DISABLE                (0x00000000uL)

#define TAG_MARKER                  (0u)
#define TAG_BUILD                   (1u)
#define TAG_CHECKSUM                (2u)
#define TAG_OFFSET_1                (4uL)
#define TAG_OFFSET_2                (8uL)

#define CISM_MARKER                 "CISM"
#define BUID_TYPE_DBG               "-DBG"
#define BUID_TYPE_EV                "-EV-"
#define BUID_TYPE_MP                "-MP-"

#define INIT_AUTH_SIDEA_MASK        (0xFFFF0000uL)
#define INIT_AUTH_SIDEB_MASK        (0x0000FFFFuL)

#define INIT_AUTH_SIDEA_MASK        (0xFFFF0000uL)
#define INIT_AUTH_SIDEB_MASK        (0x0000FFFFuL)

extern uint8_t __ghsend_icum_middle_text[];
#define CHECKSUM_MIDDLETEXT __ghsend_icum_middle_text
#define CHECKSUM_OFFSET             (4uL)

#define LOGI_ADDR_MFISWPCNTR        (0xFD860900uL)
#define MFISWPCNTR_WPD_DIS          (0xACCE0001uL)
#define LOGI_ADDR_MFISWACNTR        (0xFD860904uL)
#define MFISWACNTR_BTSTSR_ADDR      (0xACCE0604uL)

#define LOGI_ADDR_MFISBTSTSR        (0xFD860604uL)
#define ICUMH_START_KEY_VAL         (0x5AuL << 24uL)
#define ICUMH_START_KEY_MASK        (0xFFuL << 24uL)

#define SIZE_OF_INIT_BUFFER      (128*25)

/* Callback functions for first initialization. */
typedef struct
{
    /* Registration information of the function to call back when receiving an interrupt */
    /* of clock monitor error for ICUM.                                                  */
    icum_clma_cb_regist_t *p_regist_info_clk;
    /* Registration information of the function to call back when receiving an interrupt */
    /* of system error.                                                                  */
    ICUM_INT_CB_REGIST_t *p_regist_info_int;
} r_mh_Init_InitParamFirst_t;

/* Callback functions for second initialization. */
typedef struct
{
    /* Registration information of the function to call back when receiving an interrupt */
    /* of watchdog timer.                                                                */
    s_WDT_init_prm_t *p_regist_info_timer;
    /* Registration information of the function to call back when receiving an interrupt */
    /* of memory protection unit.                                                        */
    icum_mpu_cb_regist_t *p_regist_info_mpu;
} r_mh_Init_InitParamSecond_t;

static void mh_Init_JudgeStartupSide(void);
static uint32_t mh_INIT_IsEnableUpdate(uint32_t *result);
static uint32_t mh_INIT_InitFirst(r_mh_Init_InitParamFirst_t* p_initprm);
static uint32_t mh_Init_InitSecond(r_mh_Init_InitParamSecond_t* p_initprm);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_INIT_MNG_0001]:[ICUMFW_UD_MH_INIT_MNG_0001]
* Function name: MH_INIT_Init
* Description  : Initialize INIT MNG
* Arguments    : N/A
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_INIT_Init(void)
{
    uint32_t err;
    r_mh_Init_InitParamFirst_t initprm_first;
    icum_clma_cb_regist_t regist_info_clk;
    ICUM_INT_CB_REGIST_t regist_info_int;
    r_mh_Init_InitParamSecond_t initprm_second;
    s_WDT_init_prm_t regist_info_timer;
    icum_mpu_cb_regist_t regist_info_mpu;
    ICUM_COMM_CB_REGIST_t comm_cb_regist;
    ICUM_COMM_CB_REGIST_t addjob_cb_regist;
    pkc_prng_func_t pkc_prng_func;
    uint32_t check_debugger = CLR;
    uint32_t status;
    uint8_t sys_cfg_testlock;
    uint32_t update_result;

    initprm_first.p_regist_info_clk = &regist_info_clk;
    initprm_first.p_regist_info_int = &regist_info_int;
    initprm_second.p_regist_info_timer = &regist_info_timer;
    initprm_second.p_regist_info_mpu = &regist_info_mpu;

#if defined(FSW_REMAP_DRIVER)
    /* Initialize the REMAP driver */
    (void)D_REMAP_Init();
#endif

    /********** SHE status register setting start **********/
    /* 1-1. Clr SECURE_BOOT bit - ICU2PES register */
    (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_SEC_BOOT , CLR);

    /* 1-2. Clr BOOT_OK bit - ICU2PES register */
    (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_BOOT_OK , CLR);
    /* 1-3. Clr BOOT_FINISHED bit - ICU2PES register */
    (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_BOOT_FIN , CLR);

    /* 1-4. Clr BOOT_INIT bit - ICU2PES register */
    (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_BOOT_INIT , CLR);

    /* 2. Ext debugger check */
    (void)D_CR_GetDCS(&check_debugger);
    if (EXT_DEBUGGER_CONNECT == check_debugger)
    {
        /* 2-1. Set EXT_DEBUGGER bit - ICU2PES register */
        (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_EXT_DEBUG , SET);
    }
    else
    {
        /* 2-2. Clr EXT_DEBUGGER bit - ICU2PES register */
        (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_EXT_DEBUG , CLR);
    }

    /********** SHE status register setting end **********/
    /********** First init **********/
    initprm_first.p_regist_info_clk->p_cbfunc = (icum_clma_interrupt_type_t *)MH_SYSINFO_InterruptCLMACallBack;
    initprm_first.p_regist_info_int->p_cbfunc = (icum_intc_interrupt_type_t *)MH_SYSINFO_InterruptCallBack;
    (void)mh_INIT_InitFirst(&initprm_first);

    /* 4-2. Set serv_request handler */
    comm_cb_regist.channel = ICUM_COMM_ICH_FM_SERV_REQUEST;
    comm_cb_regist.p_cbfunc = (p_func_addr *)K_ISSD_HandleRequest;

    (void)D_COMM_SetIntCallBackFunc(&comm_cb_regist);

    /* Set Callback function for single interrupt */
    addjob_cb_regist.channel = ICUM_COMM_ICH_FM_REGIST_ADDJOBFUNC;
    addjob_cb_regist.p_cbfunc = (p_func_addr *)K_ISSD_FastestCmacAddJobQue;

    (void)D_COMM_SetIntCallBackFunc(&addjob_cb_regist);

    /* 5. AES & Context & ABTR & INFO Init <- Prepare for DVC */
     /* return value is ER_OK only */
    (void)ML_ABTR_Init();
     /* return value is ER_OK only */
    (void)ML_CTX_Init();

    /* 6. DVC Init */
    (void)ML_DVC_Init();

    /* 7-4. Flash mng Initialize */
    (void)MM_FLS_Init();

    /* xx. Prepare get System Configuration */
    /* xx-1. CFG mng Initialize */
    /* Setting default system comfig value. This func call before MH_SYSINFO_Init().*/ /* return value is ER_OK only */
    (void)ML_CFG_Init();

    /* xx. Set Secure Data Flash mapping */
    err = MM_FLS_MapDataSet();
    if ((ER_OK == err) || (ER_MM_FLS_ALL_BLANK == err) || (ER_MM_FLS_FORMAT_ERR == err))
    {
        /* xx-2. SYSINFO mng Initialize */
        /* Get system comfig value.*/
        err = MH_SYSINFO_Init();

        if (ER_OK == err)
        {
            /* Do nothing */
        }
        else
        {
            /* Stop initialization */
            err = ER_MH_INIT_M_SYSINFO_INITFAIL;
        }
    }
    else
    {
        /* Stop initialization */
        err = ER_MH_INIT_M_SYSINFO_INITFAIL;
    }

    if (ER_OK == err)
    {

        /* xx. Side select for OTA */
        /* xx-1. Get Current Startup Side Status */
        (void)D_COMM_GetICUStatusInfo(&status);

        /* xx-2. Judge Startup Side for OTA */
        mh_Init_JudgeStartupSide();

        /**********************************************************************************************/

        /********** Firmware state check **********/
        /* this version no check */

        /********** PISM and CISM check **********/
        /* this version no check */

        /**********************************************************************************************/

        /********** DRIVER Init **********/
        /* xx-2. Set WDT param */
        initprm_second.p_regist_info_timer->mode = WDT_ERM_NMI;
        initprm_second.p_regist_info_timer->ovf = WDT_OVF_256MS;
        initprm_second.p_regist_info_timer->p_callback = (p_wdt_func_addr_t *)MH_SYSINFO_InterruptNMICallBack;
        /* xx-2. */
        initprm_second.p_regist_info_mpu->p_cbf_addr = (icum_mpu_interrupt_type_t *)MH_SYSINFO_InterruptMPUCallBack;

        (void)mh_Init_InitSecond(&initprm_second);

        /********** LIBRARY Init **********/
        pkc_prng_func.res_id = ABTR_RESOURCE_TYPE_AES;
        pkc_prng_func.p_get = ML_ABTR_GetResource;
        pkc_prng_func.p_free = ML_ABTR_FreeResource;
        pkc_prng_func.p_generate = ML_PRNG_Gen;
        (void)L_PKC_Init(&pkc_prng_func);

        /********** MIDDLE Init **********/
        /* xx. */
        /* xx-1. KEY mng Initialize */
        /* Init Key value & mapping */ /* return value is ER_OK only */
        (void)MH_KEY_Init();

        /* xx-3. Flash mng configuration */
        (void)MM_FLS_Config();

        /**************************************************/
        /* Other MNG(MYDATA,MTC) is not init this version */
        /**************************************************/
        /* xx. */
        /* xx-2. MyData mng Initialize */
        (void)MH_MYDATA_Init();

        /* Initialization of variables in CF_MNG. return value is ER_OK only */
        (void)MM_CR_Init();

#if defined (FSW_EXTERNAL_FLASH)
        /* xx. Set Secure Data Flash mapping */
        (void)MM_FLS_MapDataSetEx();
#endif

        /********** SERVICE **********/
        /* xx. */
        /* xx-1. S_02 RAND Initialize */
        /* return value is ER_OK only */
        (void)S_02_RAND_Init();

#if defined(CFG_USE_S_20_CR)
        /* return value is ER_OK only */
        /* xx-2. S_20 CR Initialize */
        (void)S_20_CR_Init();
#endif

        /********** Prepare StartPE **********/
        /* xx. */
        /* ICUM_LOCKCTRL register setting */
        (void)ML_CFG_GetSys(ML_CFG_SYS_TEST_ECC_LOCK , (uint32_t *)&sys_cfg_testlock);

        if ((uint32_t)ECC_TEST_LOCK == (uint32_t)sys_cfg_testlock)
        {
            (void)D_ECC_SetECCTestProtection(ICUM_ECC_TEST_TYPE_COLLECTIVE , ICUM_ECC_TEST_PROTECT_ON);
        }
        else
        {
            (void)D_ECC_SetECCTestProtection(ICUM_ECC_TEST_TYPE_COLLECTIVE , ICUM_ECC_TEST_PROTECT_OFF);
        }

        /* xx-n. Start OS Timer */
        D_OST_ContinueCount();

        /* xx-n. Enable all Interrupt */
        (void)D_INT_EnableInterrupts();

        /* xx-n. ICUM_WDT Start */
        D_WDT_Start();

        /****** Prepare Secure boot ******/
        /* xx.  */
        /* xx-n. Retrieve the CMAC value from Data Flash */
        /* set cism state */
        (void)mh_INIT_IsEnableUpdate(&update_result);

        if (AUTH_ENABLE == update_result)
        {
            (void)MH_SYSINFO_SetCISMState(SYSINFO_STATE_ON);
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

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_INIT_MNG_0005]:[ICUMFW_UD_MH_INIT_MNG_0005]
* Function name: MH_INIT_StartPE
* Description  : Startup PE
* Arguments    : N/A
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MH_INIT_StartPE(void)
{
    uint32_t err;
    sys_cfg_rand_t sys_cfg_rand;

    sys_cfg_rand.rand_olt_no_ds_reset = CLR;
    sys_cfg_rand.rand_first_init_type = CLR;
    
    /* 1. Startup PE0 */
    D_COMM_StartupPE();

    /* 2. Set STARTPE bit - ICU2PES register */
    err = D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_STARTPE , SET);
    if(err == ER_OK)
    {
        /* 3. Get System Configuration */
        err = ML_CFG_GetSys(ML_CFG_SYS_RAND , (uint32_t*)&sys_cfg_rand);
        if(err == ER_OK)
        {
            /* Clear TRNG bit - ICU2PES register */
            (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_TRNG, CLR);
            /* 4. TRNG pre start check */
            if(sys_cfg_rand.rand_trng_pre_init_start == TRNG_PRE_START_VALID)
            {
                /* 5. Start TRNG */
                /* 5-1. Setting Online Test ON mode */
                if ((uint32_t)RAND_NO_TEST != sys_cfg_rand.rand_olt_no_ds_reset)
                {
                    (void)ML_PRNG_GenTrng(sys_cfg_rand.rand_olt_no_ds_reset, ML_TRNG_SIZE_TEST);
                }
                /* 5-2. Setting SHE mode */
                else if(sys_cfg_rand.rand_first_init_type == TRNG_SHE_MODE)
                {
                    (void)ML_PRNG_GenTrng(NUM0, ML_TRNG_SIZE_SHE);
                }
                /* 5-3. Setting CTR DRBG mode */
                else
                {
                    (void)ML_PRNG_GenTrng(NUM0, ML_TRNG_SIZE_NIST);
                }
            }
            else
            {
                /* Do nothing */
            }
            /* 6. Set SERVAVAIL bit - ICU2PES register */
            err = D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_SERVAVAIL , SET);
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

    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_INIT_MNG_0003]:[ICUMFW_UD_MH_INIT_MNG_0003]
* Function name: mh_INIT_IsEnableUpdate
* Description  :
* Arguments    : uint32_t update
*              * uint32_t *result
* Return Value : ER_OK
***************************************************************************************/
static uint32_t mh_INIT_IsEnableUpdate(uint32_t *result)
{
    if (NULL == result)
    {
        return ER_PARAM;
    }
    *result = AUTH_DISABLE;
    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_UnifiedAPI_C3_0001]:[ICUMFW_UD_UnifiedAPI_C3_0001]
* Function name: mh_INIT_InitFirst
* Description  : 
* Arguments    : p_initprm
* Return Value : ER_OK
***************************************************************************************/
static uint32_t mh_INIT_InitFirst(r_mh_Init_InitParamFirst_t* p_initprm)
{
    /* Return value. */
    uint32_t err;
    /* Registration information of the function to call back when receiving an interrupt */
    /* of clock monitor error for ICUM.                                                  */
    icum_clma_cb_regist_t regist_info_clk;
    /* Registration information of the function to call back when receiving an interrupt */
    /* of system error.                                                                  */
    ICUM_INT_CB_REGIST_t regist_info_int;

    /* 1. Initialize clock driver. */
    err = D_CLK_Init();
    if (ER_OK == err)
    {
        /* 1-1. Set PLLSET bit - ICU2PES register */
        (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_PLLSET, SET);
    }
    else
    {
        /* 1-2. Clear PLLSET bit - ICU2PES register */
        (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_PLLSET, CLR);
    }

    /* 2. Start clock monitor. */
    regist_info_clk = *(p_initprm->p_regist_info_clk);
    (void)D_CLK_StartCLMA(&regist_info_clk);

    /* 4. Initialize communication driver. */
    D_COMM_Init();

#if defined(FSW_USE_CRAM)
    /* Set CRAM guard */
    (void)D_MPU_SetCramGuard();
#endif

    /* 5. Initialize the AES driver. */
    (void)D_AES_Init((init_callback_t *)S_01_CIPHER_CallbackAes);

    /* 7. Initialize the information driver. */
    (void)D_INFO_Init();

    /* 8. Initialize the OS timer of timer driver. */
    D_OST_Init();

    /* 9. Initialize the interrupt driver. */
    regist_info_int = *(p_initprm->p_regist_info_int);
    (void)D_INT_Init(&regist_info_int);

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_UnifiedAPI_C3_0002]:[ICUMFW_UD_UnifiedAPI_C3_0002]
* Function name: mh_Init_InitSecond
* Description  : 
* Arguments    : p_initprm
* Return Value : ER_OK
***************************************************************************************/
static uint32_t mh_Init_InitSecond(r_mh_Init_InitParamSecond_t* p_initprm)
{
    /* Registration information of the function to call back when receiving an interrupt */
    /* of watchdog timer.                                                                */
    s_WDT_init_prm_t regist_info_timer;
    /* Registration information of the function to call back when receiving an interrupt */
    /* of memory protection unit.                                                        */
    icum_mpu_cb_regist_t regist_info_mpu;
    /* Structure for initializing the ECC driver. */
    ICUM_ECC_CB_REGIST_t regist_info_ecc;

    /* 1. Initialize the watchdog timer of timer driver. */
    if ((NULL != p_initprm) && (NULL != p_initprm->p_regist_info_timer))
    {
        /* 1-1. Initialize with the input value. */
        regist_info_timer = *(p_initprm->p_regist_info_timer);
    }
    else
    {
        /* 1-2. Initialize with default value. */
        regist_info_timer.mode = WDT_ERM_NMI;
        regist_info_timer.ovf = WDT_OVF_256MS;
        regist_info_timer.p_callback = (p_wdt_func_addr_t *)NULL;
    }
    D_WDT_Init(&regist_info_timer);

    /* 2. Initialize MPU driver. */
    regist_info_mpu = *(p_initprm->p_regist_info_mpu);
    (void)D_MPU_Init(&regist_info_mpu);

    /* 3. Initialize flash driver. */
    /* 4. Initialize ECC driver. */
    regist_info_ecc.p_cbfunc = (p_eccfunc_addr *)NULL;
    (void)D_ECC_Init(&regist_info_ecc);

    /* 4. Initialize TRNG driver. */
    (void)D_TRNG_Init();

    /* 5. Initialize the challenge and response driver. */
    (void)D_CR_Init();

    /* 6. Initialize reset driver. */
    (void)D_RESET_Init();

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_INIT_MNG_0002]:[ICUMFW_UD_MH_INIT_MNG_0002]
* Function name: mh_Init_JudgeStartupSide
* Description  : Judge Startup Side for OTA
* Arguments    : N/A
* Return Value : N/A
***************************************************************************************/
static void mh_Init_JudgeStartupSide(void)
{
    (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_STARTUP, CLR);

    return;
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)
