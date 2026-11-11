/******************************************************************************/
/* Component Name   ICU-M Service 00 ecc test                                 */
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
#include "r_icumif_api.h"
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "icum_d_ecc_pub.h"
#include "icum_ml_cfg_pub.h"
#include "icum_mh_sysinfo_pub.h"
#include "icum_s_pub.h"
#include "icum_k_issd_pub.h"
#include "icum_s_00_common.h"
#include "icum_service_common.h"

/**************************************************************************************/
/* Defines                                                                            */
/**************************************************************************************/
#define ECC_TEST_EXE_OFF  (uint8_t)0x00
#define ECC_TEST_EXE_ON   (uint8_t)0x01

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
static uint32_t s_00_EccTestCheckExeState(uint8_t *p_exe_status);

/**************************************************************************************/
/* Functions                                                                          */
/**************************************************************************************/
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0063]:[ICUMFW_UD_S_00_SystemConfig_0063]
* Function name: S_00_EccTest
* Description  : Set Unlocked(0) or Locked(1) to ECCTEST_LOCK of ICUM_LOCKCTRL Register.
* Arguments    : p_job
* Return Value : N/A
***************************************************************************************/
#pragma ghs section text = ".icum_service_ram"
#pragma ghs section rodata = ".icum_service_const_ram"
void S_00_EccTest(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    uint32_t err;
    icum_ecc_test_protect_status_t get_state;
    uint8_t sys_cfg;
    uint8_t status;
    uint32_t skip_flg;

    p_ISD = p_job->p_ISD;
    skip_flg = FLG_OFF;
    
    /* Check the execute state. */
    err = s_00_EccTestCheckExeState(&status);

    if(err != ER_OK)
    {
        skip_flg = FLG_ON;
    }
    else
    {
        /* do nothing */
    }
    if (skip_flg == FLG_OFF)
    {
        /* Check value of ECC test operation. */
        switch(p_ISD->prm.ECC_TEST.ecc_test_operation)
        {
            case ECC_TEST_START:
                /* First call */
                if(p_ISD->service_result == SERV_REQUEST)
                {
                    if(status == ECC_TEST_EXE_OFF)
                    {
                        /* Notify ISSD of mode event ECC_TEST_START. */
                        (void)K_ISSD_SendModeEvent(ISSD_EVT_ECC_TEST_START);
                        err = ER_IN_PROCESS;
                    }
                    else
                    {
                        err = ER_PARAM;
                    }
                }
                /* Second call*/
                else
                {
                    if(status == ECC_TEST_EXE_ON)
                    {
                        /* Set Unlocked to ECCTEST_LOCK of ICUM_LOCKCTRL Register. */
                        (void)D_ECC_SetECCTestProtection(ICUM_ECC_TEST_TYPE_COLLECTIVE,
                                ICUM_ECC_TEST_PROTECT_OFF);
                        /* Store ECC test state unlocked. */
                        p_ISD->prm.ECC_TEST.ecc_test_lock_status = ECC_TEST_UNLOCKED;
                    }
                    else
                    {
                        err = ER_PARAM;
                    }
                }
                break;

            case ECC_TEST_END:
                /* First call */
                if(p_ISD->service_result == SERV_REQUEST)
                {
                    if(status == ECC_TEST_EXE_ON)
                    {
                        /* Get default value of ECC test state from system configuration. */
                        (void)ML_CFG_GetSys_ram(ML_CFG_SYS_TEST_ECC_LOCK,(uint32_t *)&sys_cfg);
                        if(sys_cfg == ECC_TEST_UNLOCK)
                        {
                            /* Set Unlocked to ECCTEST_LOCK of ICUM_LOCKCTRL Register. */
                            (void)D_ECC_SetECCTestProtection(ICUM_ECC_TEST_TYPE_COLLECTIVE,
                                    ICUM_ECC_TEST_PROTECT_OFF);
                            /* Store ECC test state unlocked. */
                            p_ISD->prm.ECC_TEST.ecc_test_lock_status = ECC_TEST_UNLOCKED;
                        }
                        else
                        {
                            /* Set Locked to ECCTEST_LOCK of ICUM_LOCKCTRL Register. */
                            (void)D_ECC_SetECCTestProtection(ICUM_ECC_TEST_TYPE_COLLECTIVE,
                                    ICUM_ECC_TEST_PROTECT_ON);
                            /* Store ECC test state locked. */
                            p_ISD->prm.ECC_TEST.ecc_test_lock_status = ECC_TEST_LOCKED;
                        }
                        /* Notify ISSD of mode event ECC_TEST_END. */
                        (void)K_ISSD_SendModeEvent(ISSD_EVT_ECC_TEST_END);
                    }
                    else
                    {
                        err = ER_PARAM;
                    }
                }
                /* Second call */
                else
                {
                    if(status == ECC_TEST_EXE_ON)
                    {
                        err = ER_PARAM;
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
                break;

            case ECC_TEST_STATUS:
                /* Get lock flag value from ECCTEST_LOCK of ICUM_LOCKCTRL Register. */
                (void)D_ECC_GetECCTestProtection(ICUM_ECC_TEST_TYPE_COLLECTIVE,&get_state);
                if(get_state != ICUM_ECC_TEST_PROTECT_OFF)
                {
                    /* Store ECC test state locked. */
                    p_ISD->prm.ECC_TEST.ecc_test_lock_status = ECC_TEST_LOCKED;
                }
                else
                {
                    /* Store ECC test state unlocked. */
                    p_ISD->prm.ECC_TEST.ecc_test_lock_status = ECC_TEST_UNLOCKED;
                }
                break;

            default:
                err = ER_PARAM;
                break;
        }
    }
    else
    {
        /* do nothing */
    }

    /* Error conversion */
    err = s_00_ConvertErr(err);
    /* Store service result */
    p_ISD->service_result = (r_icumif_service_result_t)err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0064]:[ICUMFW_UD_S_00_SystemConfig_0064]
* Function name: s_00_EccTestCheckExeState
* Description  : Check the state(ROM execution state or RAM execution state,
                 and ECC test state or non ECC test state).
* Arguments    : p_exe_status
* Return Value : ER_OK
                 ER_UNAVAILABLE
                 ER_S_00_SYS
***************************************************************************************/
static uint32_t s_00_EccTestCheckExeState(uint8_t *p_exe_status)
{
    uint32_t  err = ER_OK;
    uint8_t   ecc_test_state;
    uint8_t   ram_exe_state;

    /* Get RAM execution state */
    (void)MH_SYSINFO_GetRamExeState(&ram_exe_state);
    /* Get ECC test state */
    (void)K_ISSD_GetECCTestState(&ecc_test_state);

    /* Set initial value */
    *p_exe_status = ECC_TEST_EXE_OFF;

    if(ram_exe_state == SYSINFO_STATE_ON)
    {
        /* Check ECC test state */
        if(ecc_test_state == ISSD_ECC_TEST_ON)
        {
            /* Set ECC_TEST_EXE_ON to p_exe_status */
            *p_exe_status = ECC_TEST_EXE_ON;
        }
        else
        {
            /* In progress another RAM execution service. */
            err = ER_UNAVAILABLE;
        }
    }
    else
    {
        if(ecc_test_state == ISSD_ECC_TEST_ON)
        {
            /* System error */
            err = ER_S_00_SYS;
        }
        else
        {
            /* do nothing */
        }
    }

    return err;
}

#pragma ghs section text = default
#pragma ghs section rodata = default
