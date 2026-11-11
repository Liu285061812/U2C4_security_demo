/******************************************************************************/
/* Component Name   ICUM INFO Driver                                          */
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

#include "u2b_reg_adr.h"

#include "icum_common_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_info_pub.h"
#include "icum_d_info.h"

/***************************************************************************************/
/* Internal Definition                                                                 */
/***************************************************************************************/
/* None */

/***************************************************************************************/
/* Static                                                                              */
/***************************************************************************************/
/* Area for Unique Chip ID(UCID) */
static uint32_t s_UCID_buf[4];
/* detection flag for multiple initialization function (=D_INFO_Init) call */
static uint32_t s_uid_get_flg = INFO_INIT_NOT_CALLED_YET;

/***************************************************************************************/
/* Local macro                                                                         */
/***************************************************************************************/
/* None */

/***************************************************************************************/
/* Prototypes (Internal)                                                               */
/***************************************************************************************/
/* None */

/***************************************************************************************
* Functions
***************************************************************************************/
/* None */

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INFO_0001]:[ICUMFW_UD_U2B_D_INFO_0001]
* Function name: D_INFO_Init
* Description  : INFO Driver Initialization
* Arguments    : None
*                
* Return Value : ER_OK
*                ER_D_INFO_INVALID_STATE
***************************************************************************************/
uint32_t D_INFO_Init(void)
{
    /* 1. Declaration and initial setting */
    uint32_t             ret = ER_OK;
    uint32_t             count;

    /* 2. Check to be called already */
    if (INFO_INIT_NOT_CALLED_YET == s_uid_get_flg)
    {
        /* 3. Set Remap Area */
        count = MISC_PROT_RETRY;
        WRITE_PROT_REG(ICUM_REG_CTLREG_SICREMAP, INFO_UCID_ADDRESS_FOR_SICREMAP, count);

        /* 4. Get UCID (After Remap) */
        s_UCID_buf[0] = REG_UCID.ucid_reg0;
        s_UCID_buf[1] = REG_UCID.ucid_reg1;
        s_UCID_buf[2] = REG_UCID.ucid_reg2;
        s_UCID_buf[3] = REG_UCID.ucid_reg3;

        /* 5. Set Flag to 1 */
        s_uid_get_flg = INFO_INIT_ALREADY_CALLED;
        
        /* 6. Initialization Remap Area */
        count = MISC_PROT_RETRY;
        WRITE_PROT_REG(ICUM_REG_CTLREG_SICREMAP, INFO_SICREMAP_DEFAULT_ADDRESS, count);
    }
    else
    {
        /* invalid state error due to multiple call */
        ret = ER_D_INFO_INVALID_STATE;
    }
    /* 7. Return Result */
    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INFO_0002]:[ICUMFW_UD_U2B_D_INFO_0002]
* Function name: D_INFO_GetInfo
* Description  : Get Fixed Information
* Arguments    : param    Getting Fixed Information ID
*                *p_info  Area Address for setting Data
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_INFO_INVALID_STATE
***************************************************************************************/
uint32_t D_INFO_GetInfo(uint32_t param, uint32_t *p_info)
{
    /* 1. Declaration and initial setting */
    uint32_t             ret = ER_OK;

    /* 2. Procedure each param */
    switch (param)
    {
        /* 2-1. Unique Chip ID */
        case INFO_UCID:
            /* if D_INFO_Init() hasn't called yet, return with invalid state error */
            if (INFO_INIT_ALREADY_CALLED == s_uid_get_flg)
            {
                /* 2-1-1. Yes, Execute Following Procedure */
                L_Toolbox_MemCpy((uint8_t *)((void *)p_info), (const uint8_t *)((void *)s_UCID_buf), (int16_t)sizeof(s_UCID_buf));
            }
            else
            {
                /* 2-1-2. return invalid state error because Init() has not called yet */
                ret = ER_D_INFO_INVALID_STATE;
            }
            break;
        /* 2-2. UpgradeProtection1 */
        case INFO_UPGP1:
            *p_info = ((ICUM_CTLREGICUM_CFG0 & INFO_BIT_CALC_GET_20_TO_23) >> INFO_BIT_SHIFT_20);
            break;
        /* 2-3. UpgradeProtection2 */
        case INFO_UPGP2:
            *p_info = ((ICUM_CTLREGICUM_CFG0 & INFO_BIT_CALC_GET_12_TO_15) >> INFO_BIT_SHIFT_12);
            break;
        /* 2-4. Reset Vector Address */
        case INFO_RESV:
            *p_info = ICUM_CTLREGICUM_CFG1;
            break;
        /* 2-5. Starting Address of Secure Code Flash area region 0 */
        case INFO_RACS0:
            *p_info = ICUM_CTLREGICUM_CFG2;
            break;
        /* 2-6. Starting Address of Secure Data Flash area region 0 */
        case INFO_RAD:
            *p_info = ICUM_CTLREGICUM_CFG3;
            break;
        /* 2-7. PEDBG Enable/Disable Value */
        case INFO_ICUMPEDBG:
            *p_info = ((ICUM_CTLREGICUM_CFG4  & INFO_BIT_CALC_GET_BIT28) >> INFO_BIT_SHIFT_28);
            break;
        /* 2-8. C&R Skip/Enable Value of PE */
        case INFO_ICUMOPCRF1:
            *p_info = ((ICUM_CTLREGICUM_CFG4 & INFO_BIT_CALC_GET_BIT24) >> INFO_BIT_SHIFT_24);
            break;
        /* 2-9. C&R Skip/Enable Value of ICUP */
        case INFO_ICUMOPCRF2:
            *p_info = ((ICUM_CTLREGICUM_CFG4 & INFO_BIT_CALC_GET_BIT25) >> INFO_BIT_SHIFT_25);
            break;
        /* 2-10. GPIO Control Enable/Disable Value */
        case INFO_ICUMOPGPIO:
            *p_info = ((ICUM_CTLREGICUM_CFG4 & INFO_BIT_CALC_GET_BIT17) >> INFO_BIT_SHIFT_17);
            break;
        /* 2-11. C&R Value of RHSIF Link Partner */
        case INFO_CRAUTH_NEED:
            *p_info = ((ICUM_CTLREGICUM_CFG4 & INFO_BIT_CALC_GET_BIT15) >> INFO_BIT_SHIFT_15);
            break;
        /* 2-12. WDTA VAC Value */
        case INFO_ICUMOPWDVAC:
            *p_info = ((ICUM_CTLREGICUM_CFG4 & INFO_BIT_CALC_GET_BIT05) >> INFO_BIT_SHIFT_05);
            break;
        /* 2-13. WDTA Starting Mode Value */
        case INFO_ICUMOPWDRUN:
            *p_info = ((ICUM_CTLREGICUM_CFG4 & INFO_BIT_CALC_GET_BIT04) >> INFO_BIT_SHIFT_04);
            break;
        /* 2-14. WDTA Enable/Disable Value */
        case INFO_ICUMOPWDEN:
            *p_info = ((ICUM_CTLREGICUM_CFG4 & INFO_BIT_CALC_GET_BIT03) >> INFO_BIT_SHIFT_03);
            break;
        /* 2-15. WDTA Count-Lock Value */
        case INFO_ICUMOPWDOVF:
            *p_info = (ICUM_CTLREGICUM_CFG4 & INFO_BIT_CALC_GET_00_TO_02);
            break;
        /* 2-16. ICUMLRAM Value */
        case INFO_ICUMLRAM:
            *p_info = ((ICUM_CTLREGICUM_CFG4 & INFO_BIT_CALC_GET_10_TO_11) >> INFO_BIT_SHIFT_10);
            break;
        /* 2-17. WDTA Count-Lock Value */
        case INFO_ICUMPKCCA:
            *p_info = ((ICUM_CTLREGICUM_CFG4 & INFO_BIT_CALC_GET_08_TO_09) >> INFO_BIT_SHIFT_08);
            break;
        /* 2-18. Ending Address of Secure Code Flash area region 0 */
        case INFO_RACE0:
            //*p_info = ICUM_CTLREG.ICUM_CFG5;
            break;
        /* 2-19. Starting Address of Secure Code Flash area region 1 */
        case INFO_RACS1:
            //*p_info = ICUM_CTLREG.ICUM_CFG6;
            break;
        /* 2-20. Ending Address of Secure Code Flash area region 1 */
        case INFO_RACE1:
            //*p_info = ICUM_CTLREG.ICUM_CFG7;
            break;
       /* 2-21. param = Other */
        default:
            /* parameter error */
            ret = ER_PARAM;
            break;
    }
    /* 3. Return Result */
    return (ret);
}
