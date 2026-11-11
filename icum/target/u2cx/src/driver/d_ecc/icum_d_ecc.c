/******************************************************************************/
/* Component Name   ICUM ECC Driver                                           */
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

/* Header Files                         */
#include ICUM_DEVICE_FILE
#include "renesas_types.h"

#include "icum_common_pub.h"
#include "icum_d_misc_pub.h"

#include "icum_d_ecc_pub.h"
#include "icum_d_ecc.h"

/***************************************************************************************/
/* Internal Definition                                                                 */
/***************************************************************************************/
/* None */

/***************************************************************************************/
/* Static                                                                              */
/***************************************************************************************/
/* None */

/***************************************************************************************/
/* Prototypes (Internal)                                                               */
/***************************************************************************************/
/* None */

/***************************************************************************************
* Functions
***************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_ECC_0001]:[ICUMFW_UD_U2B_D_ECC_0001]
* Function name: D_ECC_Init
* Description  : ECC Driver Initialization
* Arguments    : None
*                
* Return Value : ER_OK
***************************************************************************************/
uint32_t D_ECC_Init(ICUM_ECC_CB_REGIST_t *p_regist_info)
{
    volatile uint32_t    *p_eccreg;

    /* 1. Release DF ECC Register Protection */
    UNLOCK_REG_PROT(ICUM_DFECC.ICUM_DFKCPROT.UINT32);

    /* 2. Data Flash ECC : Disable Error Notification */
    /*      (1bit Error, 2bit Error, Overflow Error) */
    p_eccreg = (volatile uint32_t *)&ICUM_DFECC.ICUM_DFERRINT;
    *p_eccreg = (uint32_t)ECC_DISABLE_NOTIFY;

    /* Dummy read & Synchronize pipeline to wait for the completion of */
    /* writing to the ICUM_DEFRRINT enable register                    */
    (void)(*p_eccreg);
    SYNC_PIPELINE();

    /* 3. Set DF ECC Register Protection */
    LOCK_REG_PROT(ICUM_DFECC.ICUM_DFKCPROT.UINT32);

    /* Following other ECC Function Registers is not set because of useing value */
    /* after Reset.                                                              */
    /* Code Flash ECC Function, Local RAM ECC Function, Cache RAM ECC Function,  */
    /* SIC ECC Function, and SICAB ECC Function                                  */

    return (ER_OK);
}

#pragma ghs section text=".icum_driver_ram"
#pragma ghs section rodata=".icum_driver_const_ram"
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_ECC_0002]:[ICUMFW_UD_U2B_D_ECC_0002]
* Function name: D_ECC_SetECCTestProtection
* Description  : Set protection status of ECC test control register
* Arguments    : icum_ecc_test_ctrl_reg_type_t  type   : Type of ECC test control reg
*                icum_ecc_test_protect_status_t status : Protection status
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_ECC_SetECCTestProtection(icum_ecc_test_ctrl_reg_type_t type, icum_ecc_test_protect_status_t status)
{
    volatile uint32_t   *p_reg;
    uint32_t            uiRet = ER_OK;

    /* 2. According to value of parameter type,      */
    /*    execute respective corresponding processes */
    switch (type)
    {
        case ICUM_ECC_TEST_TYPE_COLLECTIVE:
            /* 2-1. In case of collective designation of ECC test control register */
            /* Set address of the register "Lock Control Register" for      */
            /* protecting OFF/ON of ECC test control registers collectively */
            p_reg = (volatile uint32_t *)&ICUM_CTLREG.ICUM_LOCKCTRL;
            /* According to value of parameter status,    */
            /* execute respective corresponding processes */
            switch ((uint32_t)status)
            {
                case ICUM_ECC_TEST_PROTECT_OFF:
                    /* 2-1-1. In case of Protect OFF */
                    /* Set "ECC test control register lock flag" of */
                    /* "Lock Control Register" to OFF               */
                    *p_reg = (*p_reg & ~ECC_TEST_CTRL_REG_LOCK_FLAG);
                    break;
                case ICUM_ECC_TEST_PROTECT_ON:
                    /* 2-1-2. In case of Protect ON */
                    /* Set "ECC test control register lock flag" of */
                    /* "Lock Control Register" to ON                */
                    *p_reg = (*p_reg | ECC_TEST_CTRL_REG_LOCK_FLAG);
                    break;
                default:
                    /* 2-1-3. In other cases */
                    uiRet = ER_PARAM;
                    break;
            }
            break;
        default:
            /* 2-2. In other cases */
            uiRet = ER_PARAM;
            break;
    }

    /* 3. Return protection status setting result */
    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_ECC_0003]:[ICUMFW_UD_U2B_D_ECC_0003]
* Function name: D_ECC_GetECCTestProtection
* Description  : Get protection status of ECC test control register
* Arguments    : icum_ecc_test_ctrl_reg_type_t  type      : Type of ECC test control reg
*                icum_ecc_test_protect_status_t *p_status : Addr of protection status
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_ECC_GetECCTestProtection(icum_ecc_test_ctrl_reg_type_t type, icum_ecc_test_protect_status_t *p_status)
{
    volatile uint32_t   *p_reg;
    uint32_t            uiRet = ER_OK;

    /* 2. According to value of parameter type,      */
    /*    execute respective corresponding processes */
    switch (type)
    {
        case ICUM_ECC_TEST_TYPE_COLLECTIVE:
            /* 2-1. In case of collective designation of ECC test control register */
            /* Execute NULL check of parameter p_status and     */
            /* execute processing corresponding to check result */
            if (NULL != p_status)
            {
                /* 2-1-1. In case of p_status is not NULL */
                /* Set address of the register "Lock Control Register" for      */
                /* protecting OFF/ON of ECC test control registers collectively */
                p_reg = (volatile uint32_t *)&ICUM_CTLREG.ICUM_LOCKCTRL;
                /* Judge the value of "ECC test control register lock flag" on */
                /* "Lock Control Register"                                     */
                if (ECC_TEST_CTRL_REG_LOCK_FLAG == (*p_reg & ECC_TEST_CTRL_REG_LOCK_FLAG))
                {
                    /* 2-1-1-1. In case of Protect ON */
                    /* Set is "Protect ON" to the store address of protection status */
                    *p_status = ICUM_ECC_TEST_PROTECT_ON;
                }
                else
                {
                    /* 2-1-1-2. In other cases */
                    /* Set is "Protect OFF" to the store address of protection status */
                    *p_status = ICUM_ECC_TEST_PROTECT_OFF;
                }
            }
            else
            {
                /* 2-1-2. In other cases */
                uiRet = ER_PARAM;
                break;
            }
            break;
        default:
            /* 2-2. In other cases */
            uiRet = ER_PARAM;
            break;
    }

    /* 3. Return protection status acquisition result */
    return (uiRet);
}
#pragma ghs section text=default
#pragma ghs section rodata=default

