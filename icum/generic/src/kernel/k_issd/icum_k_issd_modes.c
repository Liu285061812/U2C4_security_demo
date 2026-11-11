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
#include "icum_d_misc_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_d_int_pub.h"
#include "icum_mh_init_pub.h"
#include "icum_mh_sysinfo_pub.h"
#include "icum_mh_key_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_k_issd_pub.h"
#include "icum_d_info_pub.h"
#include "icum_ml_cfg_pub.h"
#include "icum_k_issd.h"

/***************************************************************************************
 Macro definitions
***************************************************************************************/

/***************************************************************************************
* Global Variable                                                                      *
***************************************************************************************/

static uint32_t g_current_icum_mode;
static uint32_t g_next_icum_mode;
static uint32_t g_background_verify_state;

/**********************************************************************************
* Function
***********************************************************************************/
static void k_ISSD_Init(void);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0035]:[ICUMFW_UD_K_ISSD_0035]
* Function name: k_ISSD_InitRunMode
* Description  : Initialize execution mode
* Arguments    : none
* Return Value : none
***************************************************************************************/
void k_ISSD_InitRunMode(void)
{
    /* initial execution mode */
    g_current_icum_mode = ISSD_MODE_START;
    /* initial next execution mode*/
    g_next_icum_mode    = g_current_icum_mode;
    /* initial background verify status */
    g_background_verify_state = BACKGROUND_VERIFY_OFF;
    /* Initialization of the job queue */
    k_ISSD_InitJobQueue();
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0033]:[ICUMFW_UD_K_ISSD_0033]
* Function name: k_ISSD_Init
* Description  : Initialize ISSD
* Arguments    : none
* Return Value : none
***************************************************************************************/
static void k_ISSD_Init(void)
{
    /* Initialize of generate service request */
    (void)k_ISSD_InitGenService();
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0039]:[ICUMFW_UD_K_ISSD_0039]
* Function name: k_ISSD_IsBootMode
* Description  : Test for secure boot mode
* Arguments    : (OUT) *p_boot_mode :
*                      1 : secure boot mode
*                      0 : not secure boot mode
* Return Value : none
***************************************************************************************/
void k_ISSD_IsBootMode(bool *p_boot_mode)
{
    /* 1. set boot mode */
    *p_boot_mode = (bool)((ISSD_MODE_VERIFY_BOOT_1ST == g_current_icum_mode) ||
                          (ISSD_MODE_VERIFY_BOOT_2ND == g_current_icum_mode) ||
                          (BACKGROUND_VERIFY_ON == g_background_verify_state));
    
    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0036]:[ICUMFW_UD_K_ISSD_0036]
* Function name: k_ISSD_GetRunMode
* Description  : Return the current mode in integer
* Arguments    : (OUT) *p_run_mode : the current mode in integer
* Return Value : none
***************************************************************************************/
#pragma ghs section text   = ".icum_kernel_ram"
#pragma ghs section rodata = ".icum_kernel_const_ram"
void k_ISSD_GetRunMode(uint32_t *p_run_mode)
{
    /* 1. Get current mode */
    *p_run_mode = g_current_icum_mode;
    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0037]:[ICUMFW_UD_K_ISSD_0037]
* Function name: k_ISSD_ModeChangeRequested
* Description  : Test for mode change request
* Arguments    : (OUT) *p_change_request :
*                      1 : if execution mode must be changed
*                      0 : otherwise
* Return Value : none
***************************************************************************************/
void k_ISSD_ModeChangeRequested(bool *p_change_request)
{
    /* 1. Get mode change requested */
    *p_change_request = (bool)(g_next_icum_mode != g_current_icum_mode);

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0041]:[ICUMFW_UD_K_ISSD_0041]
* Function name: K_ISSD_GetECCTestState
* Description  : return of ECC test state
* Arguments    : none
* Return Value : ISSD_ECC_TEST_OFF/ISSD_ECC_TEST_ON
***************************************************************************************/
uint32_t K_ISSD_GetECCTestState(uint8_t *p_state)
{
    if(ISSD_MODE_ECC_TEST == g_current_icum_mode)
    {
        *p_state = ISSD_ECC_TEST_ON;
    }
    else
    {
        *p_state = ISSD_ECC_TEST_OFF;
    }
    return(ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0040]:[ICUMFW_UD_K_ISSD_0040]
* Function name: k_ISSD_SetBackgroundVerifyState
* Description  : set background verify state
* Arguments    : (IN) state:
* Return Value : none
***************************************************************************************/
void k_ISSD_SetBackgroundVerifyState(uint32_t state)
{
    uint32_t saved_psw;

    /* disable interrupts (if not already disabled) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    g_background_verify_state = state;

    /* re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
    return;
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0034]:[ICUMFW_UD_K_ISSD_0034]
* Function name: K_ISSD_SendModeEvent
* Description  : Execution event triggered. Check what is the next execution mode
* Arguments    : (IN) : event
*                ER_PARAM
* Return Value : ER_OK
*                ER_K_ISSD_MODE_REJECT
***************************************************************************************/
uint32_t K_ISSD_SendModeEvent(uint32_t event)
{
    uint32_t new_mode;
    uint32_t rtn = ER_OK;
    uint32_t saved_psw;

    /* disable interrupts (if not already disabled) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);
    new_mode = g_current_icum_mode;

    switch(event)
    {
        case ISSD_EVT_SYS_INIT_COMPLETED:
        case ISSD_EVT_SYS_INIT_FAILED:
            switch(g_current_icum_mode)
            {
                case ISSD_MODE_START:
                    new_mode = ISSD_MODE_VERIFY_BOOT_1ST;
                    (void)MH_SYSINFO_SaveTickCount(SYSINFO_TICK_COUNT_ID_00);
                    break;
                default:
                    rtn = ER_K_ISSD_MODE_REJECT;
                    break;
            }
            break;

        case ISSD_EVT_START_PE_COMPLETED:
        case ISSD_EVT_START_PE_FAILED:
            switch(g_current_icum_mode)
            {
                case ISSD_MODE_VERIFY_BOOT_1ST:
                    new_mode = ISSD_MODE_VERIFY_BOOT_2ND;
                    break;
                default:
                    rtn = ER_K_ISSD_MODE_REJECT;
                    break;
            }
            break;

        case ISSD_EVT_TRANSITION_TO_ROM:
            switch(g_current_icum_mode)
            {
                case ISSD_MODE_VERIFY_BOOT_2ND:
                    new_mode = ISSD_MODE_ROM_EXEC;
                    break;
                default:
                    rtn = ER_K_ISSD_MODE_REJECT;
                    break;
            }
            break;

        case ISSD_EVT_ECC_TEST_START:
            switch(g_current_icum_mode)
            {
                case ISSD_MODE_ROM_EXEC:
                    new_mode = ISSD_MODE_ECC_TEST;
                    /* Stop background verify */
                    if(BACKGROUND_VERIFY_ON == g_background_verify_state)
                    {
                        k_ISSD_SetBackgroundVerifyState(BACKGROUND_VERIFY_OFF);
                    }
                    else
                    {
                        /* do nothing */
                    }
                    break;
                default:
                    rtn = ER_K_ISSD_MODE_REJECT;
                    break;
            }
            break;

        case ISSD_EVT_ECC_TEST_END:
            switch(g_current_icum_mode)
            {
                case ISSD_MODE_ECC_TEST:
                    new_mode = ISSD_MODE_ROM_EXEC;
                    break;
                default:
                    rtn = ER_K_ISSD_MODE_REJECT;
                    break;
            }
            break;

        default:
            rtn = ER_PARAM;
            /* should never occur */
            ASSERT(0);
            break;
    }

    /* trigger a change mode request here */
    if (ER_OK == rtn)
    {
        g_next_icum_mode = new_mode;
    }
    else
    {
        /* Do nothing  */
    }

    /* re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return(rtn);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0032]:[ICUMFW_UD_K_ISSD_0032]
* Function name: k_ISSD_Run
* Description  : Branch to dispatch loop depending on execution mode
* Arguments    : none
* Return Value : none
***************************************************************************************/
void k_ISSD_Run(void)
{
    uint32_t result;
    uint8_t ram_state;
    uint8_t fls_err;
    bool gen_request;
    uint32_t saved_psw;

    /* change mode here */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);
    g_current_icum_mode = g_next_icum_mode;
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* run the dispatch loop */
    switch(g_current_icum_mode)
    {
        case ISSD_MODE_START:
            /* first system initialization */
            result = MH_INIT_Init();
            /* check return code */
            if(ER_OK == result)
            {
                (void)K_ISSD_SendModeEvent(ISSD_EVT_SYS_INIT_COMPLETED);
            }
            else
            {
                /* transition to HALT */
                MH_SYSINFO_Stop();
            }
            /* ISSD Initialize */
            k_ISSD_Init();

            /* Check data set size */
            result = MM_FLS_CheckDataSetSize(&fls_err);
            if(fls_err == FLS_CHK_OK)
            {
                /* Check and set vm mapping */
                result = ML_CFG_SetIndexMappingVM();
                if (ER_OK != result)
                {
                    result = ER_MH_INIT_M_CFG_INITFAIL;
                }
            }
            else
            {
                result = ER_MH_INIT_D_FLS_INITFAIL;
            }

            /* Install RSA ROM PUBLIC Key. Don't care if error occurred */
            (void)MH_KEY_InstallRsaRomPub();

            result = MH_SYSINFO_UpdateSystemConfig(12);
            if(result == ER_OK)
            {
                /* Notify secure data flash is initialized */
                result = MH_SYSINFO_SetInitState(SYSINFO_DF_INITIALIZED);
            }

            break;

        case ISSD_MODE_VERIFY_BOOT_1ST:
            /* secure boot exec. */
            (void)MH_SYSINFO_SaveTickCount(SYSINFO_TICK_COUNT_ID_01);
                /* start PE request */
                result = MH_INIT_StartPE();
                /* check return code */
                if(ER_OK == result)
                {
                    (void)K_ISSD_SendModeEvent(ISSD_EVT_START_PE_COMPLETED);
                }
                else
                {
                    (void)K_ISSD_SendModeEvent(ISSD_EVT_START_PE_FAILED);
                }
            break;

        case ISSD_MODE_VERIFY_BOOT_2ND:
            /* secure boot exec. */
                (void)K_ISSD_SendModeEvent(ISSD_EVT_TRANSITION_TO_ROM);
            break;

        case ISSD_MODE_ROM_EXEC:
            /* get from SYSINFO MNG*/
            result = MH_SYSINFO_GetRamExeState(&ram_state);
            if(ER_OK == result)
            {
                if((uint32_t)(SYSINFO_STATE_ON) == (uint32_t)ram_state)
                {
                    /* change to RAM mode of interrupt handler*/
                    (void)D_INT_ChgExecMode(ICUM_INT_ROMMODE);

                    /* notify to SYSINFO MNG*/
                    (void)MH_SYSINFO_SetRamExeState(SYSINFO_STATE_OFF);
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* do nothing */
            }

            /* get request of service generate */
            (void)k_ISSD_IsRegistGenService(&gen_request);
            if((uint32_t)true == (uint32_t)gen_request)
            {
                /* generate service execute */
                (void)k_ISSD_GenerateServiceExec();
            }
            else
            {
                /* do nothing */
            }

            /* set icu status */
            (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_SERVAVAIL, ICU_STATUS_ON);

            /* check background verify state */
            if(BACKGROUND_VERIFY_ON == g_background_verify_state)
            {
                /* background verify exec. */
                    k_ISSD_SetBackgroundVerifyState(BACKGROUND_VERIFY_OFF);
                }
            else
            {
                /* jump to dispatch loop in ROM */
                k_ISSD_MainDispatch();
            }
            break;

        case ISSD_MODE_ECC_TEST:
            /* get from SYSINFO MNG*/
            result = MH_SYSINFO_GetRamExeState(&ram_state);
            if(ER_OK == result)
            {
                if((uint32_t)(SYSINFO_STATE_OFF) == (uint32_t)ram_state)
                {
                    /* change to RAM mode of interrupt handler*/
                    (void)D_INT_ChgExecMode(ICUM_INT_RAMMODE);

                    /* notify to SYSINFO MNG*/
                    (void)MH_SYSINFO_SetRamExeState(SYSINFO_STATE_ON);
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* do nothing */
            }
            /* move the dispatch loop to RAM */
            k_ISSD_RamDispatch();
            break;

        default:
            /* should never occur */
            break;
    }
}

#pragma ghs section text    = default
#pragma ghs section rodata  = default

