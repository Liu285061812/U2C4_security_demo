/******************************************************************************/
/* Component Name   ICU-M Service                                             */
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

/**********************************************************************************
* Include
***********************************************************************************/
#include "r_icumif_api.h"
#include "renesas_types.h"
#include "icum_l_toolbox_pub.h"
#include "icum_mh_key_pub.h"
#include "icum_s_01_rsa_common.h"

/**********************************************************************************
* Define
***********************************************************************************/

/**********************************************************************************
* Structure
***********************************************************************************/

/**********************************************************************************
* Variable
***********************************************************************************/

/**********************************************************************************
* Function
***********************************************************************************/
/*===========================================================================
  [PUBLIC]
  ===========================================================================*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0052]:[ICUMFW_UD_S_01_PK_0052]
* Function name: s_01_RsaKeyConvertErr
* Description  : Returns service error status from error status
* Arguments    : (IN-OUT)  p_err
* Return Value : ER_OK
****************************************************************************************/
uint32_t s_01_RsaKeyConvertErr(uint32_t *p_err)
{
    switch (*p_err)
    {
        case ER_OK:
            *p_err = (uint32_t)SERV_OK;
            break;
        case ER_PENDING:
        case ER_MH_KEY_IN_USE:
            *p_err = (uint32_t)SERV_PENDING;
            break;
        /* key not available: abort */
        case ER_MH_KEY_SLOT_EMPTY:
            *p_err = (uint32_t)SERV_KEY_SLOT_EMPTY;
            break;
        case ER_MH_KEY_INVALID_KEY_GROUP:
            *p_err = (uint32_t)SERV_INVALID_KEY_GROUP;
            break;
        case ER_MH_KEY_INVALID_KEY_INDEX:
            *p_err = (uint32_t)SERV_INVALID_KEY_INDEX;
            break;
        case ER_MH_KEY_NVM_UNAVAILABLE:
            *p_err = (uint32_t)SERV_NVM_UNAVAILABLE;
            break;
        default:
            *p_err = (uint32_t)SERV_KEY_NOT_USABLE;
            break;
    } /* end of switch (*p_err) */

    return (ER_OK);
}
