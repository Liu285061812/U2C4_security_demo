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

#include "icum_l_bignum.h"

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0040]:[ICUMFW_UD_L_CRY_BignumMath_0040]
* Function name: l_BIGNUM_MultiplyByDigit
* Description  : result = a * b
*                Multiply a(big_num_t type) and b(32 bit data)
* Arguments    : *p_a      : Address of multiplicand
*                            Range : 0x0 - 0xFFFFFFFF
*                 b        : multiplier
*                            Range : 0 - 4,294,967,295
*                *p_result : Multiplication result storage address
*                            Range : 0x0 - 0xFFFFFFFF
* Return Value : uint32_t  (ER_OK)
***************************************************************************************/
uint32_t l_BIGNUM_MultiplyByDigit(big_num_t *p_a, fp_digit b, big_num_t *p_result)
{
    fp_word     w;
    uint32_t    x, oldused;

    oldused         = p_result->used;
    p_result->used  = p_a->used;
    p_result->sign  = p_a->sign;

    w = 0uLL;
    for (x = 0; x < p_a->used; x++)
    {
        w                   = ((fp_word)p_a->value[x]) * ((fp_word)b) + w;
        p_result->value[x]  = (fp_digit)w;
        w                   = w >> DIGIT_BIT;
    }

    if (w != 0uLL && (p_a->used != g_crypt_fp_size))
    {
        p_result->value[p_result->used++] = (uint32_t)w;
        x++;
    }
    else
    {
        /* Do nothing */
    }

    for (; x < oldused; x++)
    {
        p_result->value[x] = 0u;
    }
    FP_CLAMP(p_result);
    return (ER_OK);
}
