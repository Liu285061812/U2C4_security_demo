/******************************************************************************/
/* Component Name   Cryptographic Library BIGNUM                              */
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
* Headers
***********************************************************************************/
#include "icum_l_bignum.h"


/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0033]:[ICUMFW_UD_L_CRY_BignumMath_0033]
* Function name: l_BIGNUM_Set
* Description  : Set b(32bit data) to a(big_num_t).
* Arguments    : *p_a      : The setting address of the digit
*                            Range: 0x0 - 0xFFFFFFFF
*                 b        : Set digit
*                            Range: 0x0 - 0xFFFFFFFF
* Return Value : uint32_t  (ER_OK)
***************************************************************************************/
uint32_t l_BIGNUM_Set(big_num_t *p_a, fp_digit b)
{
    FP_INIT(p_a);
    p_a->value[0] = b;
    p_a->used  = (p_a->value[0] != 0u) ? 1u : 0u;
    return (ER_OK);
}

