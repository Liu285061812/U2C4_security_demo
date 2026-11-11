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
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0018]:[ICUMFW_UD_L_CRY_BignumMath_0018]
* Function name: l_BIGNUM_CountBits
* Description  : Count the number of effective bits of a.
* Arguments    : *a : Address of BigNum data for which bit size is to be obtained
*              : *p_bit_size : Bit size of BigNum data
* Return Value : ER_OK
***************************************************************************************/
uint32_t l_BIGNUM_CountBits (const big_num_t *p_a, uint32_t *p_bit_size)
{
    uint32_t    r;
    fp_digit    q;

    /* shortcut */
    if (p_a->used == 0u)
    {
        *p_bit_size = 0u;
    }
    else
    {
        /* get number of digits and add that */
        r = (p_a->used - 1u) * DIGIT_BIT;

        /* take the last digit and count the bits in it */
        q = p_a->value[p_a->used - 1u];
        while (q > ((fp_digit) 0))
        {
            r++;
            q >>= ((fp_digit) 1);
        }
        *p_bit_size = r;
    }
    
    return (ER_OK);
}

