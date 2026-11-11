/******************************************************************************/
/* Component Name   C&R driver source file                                    */
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
#include ICUM_DEVICE_FILE
#include "renesas_types.h"

#include "u2b_reg_adr.h"

#include "icum_common_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_cr_pub.h"

/**********************************************************************************
* Define
***********************************************************************************/
/* Common */
#define ALIGN4          ((uint32_t)0x00000003uL)  /* Check 4 alignment */
#define NUM0            ((uint32_t)0uL)           /* value : 0 */
#define NUM1            ((uint32_t)1uL)           /* value : 1 */
#define NUM2            ((uint32_t)2uL)           /* value : 2 */
#define NUM3            ((uint32_t)3uL)           /* value : 3 */

/* Debug I/F control register bit */
#define CRLOCK_ICUM_ON  ((uint32_t)0x00000001uL)  /* CRLOCK_ICUM  bit:0 */
#define CRLOCK_DBGIF_ON ((uint32_t)0x00000002uL)  /* CRLOCK_DBGIF bit:1 */
#define IDLOCK_OFF      ((uint32_t)0x00000000uL)  /* IDLOCK bit:0 */
#define IDLOCK_ON       ((uint32_t)0x00010000uL)  /* IDLOCK bit:1 */
#define CDATSEL_ON      ((uint32_t)0x01000000uL)  /* CDATSEL bit:1 */
#define LK_DBGIF_CTL_ON ((uint32_t)0x00200000uL)  /* LOCK_DBGIF_CTL bit:1 */
#define LK_ICUM_CTL_ON  ((uint32_t)0x00100000uL)  /* LOCK_ICUM_CTL  bit:1 */

/**********************************************************************************
* Function
***********************************************************************************/
/* --------------------------------------------------------------------------------;
   Function ID          : [ICUMFW_CD_U2B_D_CR_0001]:[ICUMFW_UD_U2B_D_CR_0001]
   Name                 : D_CR_Init
   Description          : Initialize this library.
   Input                : -
   Return               : D_CR_EC_SUCCESS ... success.
   --------------------------------------------------------------------------------;*/
uint32_t D_CR_Init(void)
{
    return D_CR_EC_SUCCESS;
}

#pragma ghs section text=".icum_driver_ram"
#pragma ghs section rodata=".icum_driver_const_ram"
/* --------------------------------------------------------------------------------;
   Function ID          : [ICUMFW_CD_U2B_D_CR_0002]:[ICUMFW_UD_U2B_D_CR_0002]
   Name                 : D_CR_GetDCS
   Description          : Gets the connection/non-connection state of the debugger 
                        : to the area indicated by the argument p_cr_status.
   Input                : p_cr_status ... a pointer to debugger 
                        : connection/non-connection state buffer.
   Return               : D_CR_EC_SUCCESS ... success.
                        : D_CR_EC_INVALID_PARAM ... parameter error.
   --------------------------------------------------------------------------------;*/
uint32_t D_CR_GetDCS(uint32_t *p_cr_Status)
{
    uint32_t ret_err = D_CR_EC_SUCCESS;

    if ((NULL == p_cr_Status) || (NUM0 != (ALIGN4 & (uint32_t)p_cr_Status)))
    {
        ret_err = D_CR_EC_INVALID_PARAM;
    }
    else
    {
        if( IDLOCK_OFF == (IDLOCK_ON & ICUM_CTLREGICUM_IDAS))
        {
            *p_cr_Status = NUM1;
        }
        else
        {
            *p_cr_Status = NUM0;
        }
    }
    return ret_err;
}
#pragma ghs section text=default
#pragma ghs section rodata=default

/* --------------------------------------------------------------------------------;
   Function ID          : [ICUMFW_CD_U2B_D_CR_0003]:[ICUMFW_UD_U2B_D_CR_0003]
   Name                 : D_CR_SetChallenge
   Description          : Set challenge data into Challenge Data Register.
   Input                : AuthenticateFor ... authenticate for MAIN/ICUM
                        : p_cr_ChallengeData ... a pointer to setting challenge data.
   Return               : D_CR_EC_SUCCESS ... success.
                        : D_CR_EC_INVALID_PARAM ... parameter error.
   --------------------------------------------------------------------------------;*/
uint32_t D_CR_SetChallenge(uint32_t AuthenticateFor, const uint32_t *p_cr_ChallengeData)
{
    uint32_t ret_err = D_CR_EC_SUCCESS;
    uint32_t ct = MISC_PROT_RETRY;
    uint32_t write_data;

    if ((D_CR_ICUM < AuthenticateFor) || (NUM0 != (ALIGN4 & (uint32_t)p_cr_ChallengeData)))
    {
        ret_err = D_CR_EC_INVALID_PARAM;
    }
    else
    {
        if (D_CR_MAIN == AuthenticateFor)
        {
            /* Set challenge for MAIN_CPU authentication */
            write_data = (~CDATSEL_ON) & ICUM_CTLREGICUM_IDAS;
        }
        else    /* if (D_CR_ICUM == AuthenticateFor) */
        {
            /* Set challenge for ICUM authentication */
            write_data = CDATSEL_ON | ICUM_CTLREGICUM_IDAS;
        }
        WRITE_PROT_REG(ICUM_CTLREGICUM_IDAS, write_data, ct);
        /* Chalenge(AddressSpace, BigEndian) ---> Challenge(Register, LittleEndian) */
        ICUM_CTLREGICUM_CDAT0 = BSW(p_cr_ChallengeData[NUM3]);
        ICUM_CTLREGICUM_CDAT1 = BSW(p_cr_ChallengeData[NUM2]);
        ICUM_CTLREGICUM_CDAT2 = BSW(p_cr_ChallengeData[NUM1]);
        ICUM_CTLREGICUM_CDAT3 = BSW(p_cr_ChallengeData[NUM0]);
    }

    return ret_err;
}

/* --------------------------------------------------------------------------------;
   Function ID          : [ICUMFW_CD_U2B_D_CR_0004]:[ICUMFW_UD_U2B_D_CR_0004]
   Name                 : D_CR_GetResponse
   Description          : Get Response Register data.
   Input                : p_cr_ResponseData ... a pointer to response data buffer.
   Return               : D_CR_EC_SUCCESS ... success.
                        : D_CR_EC_INVALID_PARAM ... parameter error.
   --------------------------------------------------------------------------------;*/
uint32_t D_CR_GetResponse(uint32_t *p_cr_ResponseData)
{
    uint32_t ret_err = D_CR_EC_SUCCESS;

    if ((NULL == p_cr_ResponseData) || (NUM0 != (ALIGN4 & (uint32_t)p_cr_ResponseData)))
    {
        ret_err = D_CR_EC_INVALID_PARAM;
    }
    else
    {
        /* Response(Register, LittleEndian) ---> Response(AddressSpace, BigEndian) */
        p_cr_ResponseData[NUM3] = BSW(ICUM_CTLREGICUM_RDAT0);
        p_cr_ResponseData[NUM2] = BSW(ICUM_CTLREGICUM_RDAT1);
        p_cr_ResponseData[NUM1] = BSW(ICUM_CTLREGICUM_RDAT2);
        p_cr_ResponseData[NUM0] = BSW(ICUM_CTLREGICUM_RDAT3);
    }
    return ret_err;
}

/* --------------------------------------------------------------------------------;
   Function ID          : [ICUMFW_CD_U2B_D_CR_0005]:[ICUMFW_UD_U2B_D_CR_0005]
   Name                 : D_CR_OpenDebugger
   Description          : Enable debugging access for MAIN/ICUM.
   Input                : AccessFor ... enable debugging access for MAIN/ICUM
   Return               : D_CR_EC_SUCCESS ... success.
                        : D_CR_EC_INVALID_PARAM ... parameter error.
   --------------------------------------------------------------------------------;*/
uint32_t D_CR_OpenDebugger(uint32_t AccessFor)
{
    uint32_t ret_err = D_CR_EC_SUCCESS;
    uint32_t ct = MISC_PROT_RETRY;
    uint32_t write_data;

    if (D_CR_ICUM < AccessFor)
    {
        ret_err = D_CR_EC_INVALID_PARAM;
    }
    else
    {
        if (D_CR_MAIN == AccessFor)
        {
            /* Enable debugging for MAIN_CPU */
            write_data = (~CRLOCK_DBGIF_ON) & ICUM_CTLREGICUM_IDAS;
        }
        else    /* if (D_CR_ICUM == AccessFor) */
        {
            /* Enable debugging for ICUM */
            write_data = (~CRLOCK_ICUM_ON) & ICUM_CTLREGICUM_IDAS;
        }
        WRITE_PROT_REG(ICUM_CTLREGICUM_IDAS, write_data, ct);
    }

    return ret_err;
}
