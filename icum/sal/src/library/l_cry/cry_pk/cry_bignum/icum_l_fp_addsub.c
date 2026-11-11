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
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0001]:[ICUMFW_UD_L_CRY_BignumMath_0001]
* Function name: l_BIGNUM_Add
* Description  : a and b are addition.
* Arguments    : *p_a        : Address of addition target a
*                              Range: 0x0 - 0xFFFFFFFF
*                *p_b        : Address of addition target b
*                              Range: 0x0 - 0xFFFFFFFF
*                *p_result   : Addition result storage address
*                              Range: 0x0 - 0xFFFFFFFF
* Return Value : uint32_t  (ER_OK)
***************************************************************************************/
uint32_t l_BIGNUM_Add(big_num_t *p_a, big_num_t *p_b, big_num_t *p_result)
{
    uint32_t    sa, sb;
    int32_t     cmp;

    /* get sign of both inputs */
    sa = p_a->sign;
    sb = p_b->sign;

    /* handle two cases, not four */
    if (sa == sb)
    {
        /* both positive or both negative */
        /* add their magnitudes, copy the sign */
        p_result->sign = sa;
        (void)l_BIGNUM_AddAbs (p_a, p_b, p_result);
    }
    else
    {
        /* one positive, the other negative */
        /* subtract the one with the greater magnitude from */
        /* the one of the lesser magnitude.  The result gets */
        /* the sign of the one with the greater magnitude. */
        cmp = l_BIGNUM_CmpAbs (p_a, p_b);
        if (cmp == ER_L_BIGNUM_LT)
        {
            p_result->sign = sb;
            (void)l_BIGNUM_SubAbs (p_b, p_a, p_result);
        }
        else
        {
            p_result->sign = sa;
            (void)l_BIGNUM_SubAbs (p_a, p_b, p_result);
        }
    }
    return (ER_OK);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0002]:[ICUMFW_UD_L_CRY_BignumMath_0002]
* Function name: l_BIGNUM_AddDigit
* Description  : a(big_num_t type) and b(32bit data) are addition.
*                result = a + b
* Arguments    : *p_a        : Address of addition target a
*                              Range: 0x0 - 0xFFFFFFFF
*                 b          : Addition target b
*                              Range: 0 - 4,294,967,295
*                *p_result   : Addition result storage address
*                              Range: 0x0 - 0xFFFFFFFF
* Return Value : uint32_t  (ER_OK)
***************************************************************************************/
uint32_t l_BIGNUM_AddDigit(big_num_t *p_a, fp_digit b, big_num_t *p_result)
{
    big_num_t tmp;
    (void)l_BIGNUM_Set(&tmp, b);
    (void)l_BIGNUM_Add(p_a, &tmp, p_result);
    return (ER_OK);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0003]:[ICUMFW_UD_L_CRY_BignumMath_0003]
* Function name: l_BIGNUM_AddAbs
* Description  : Add the absolute values of a and b.
*                unsigned addition
* Arguments    : *p_a        : Address of addition target a
*                              Range: 0x0 - 0xFFFFFFFF
*                *p_b        : Address of addition target b
*                              Range: 0x0 - 0xFFFFFFFF
*                *p_result   : Addition result storage address
                               (When the number of used digits of result is larger than the 
                               result of addition, it is not used and 0 is inserted in the digit.)
*                              Range: 0x0 - 0xFFFFFFFF
* Return Value : uint32_t  (ER_OK)
***************************************************************************************/
uint32_t l_BIGNUM_AddAbs(big_num_t *p_a, big_num_t *p_b, big_num_t *p_result)
{
    uint32_t    x, y, oldused;
    register    fp_word  t;

    y               = MAX(p_a->used, p_b->used);
    oldused         = p_result->used;
    p_result->used    = y;

    t = 0;
    for (x = 0u; x < y; x++)
    {
        t                  += ((fp_word)p_a->value[x]) + ((fp_word)p_b->value[x]);
        p_result->value[x]  = (fp_digit)t;
        t                 >>= (fp_digit)DIGIT_BIT;
    }

    if (t != (fp_word)0 && x < g_crypt_fp_size)
    {
        p_result->value[p_result->used++] = (fp_digit)t;
        x++;
    }
    else
    {
        /* Do nothing */
    }

    p_result->used = x;
    for (; x < oldused; x++)
    {
        p_result->value[x] = 0;
    }
    FP_CLAMP(p_result);
    return (ER_OK);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0006]:[ICUMFW_UD_L_CRY_BignumMath_0006]
* Function name: l_BIGNUM_SubAbs
* Description  : Subtract the absolute value of b from a.
*                unsigned subtraction ||a|| >= ||b|| ALWAYS!
* Arguments    : *p_a        : The address of the minuend
*                              Range: 0x0 - 0xFFFFFFFF
*                *p_b        : The address of the subtrahend
*                              Range: 0 - 4,294,967,295
*                *p_result   : The subtraction result storage address
*                              Range: 0x0 - 0xFFFFFFFF
* Return Value : uint32_t  (ER_OK)
***************************************************************************************/
uint32_t l_BIGNUM_SubAbs(big_num_t *p_a, big_num_t *p_b, big_num_t *p_result)
{
    uint32_t    x, oldbused, oldused;
    fp_word     t;

    oldused         = p_result->used;
    oldbused        = p_b->used;
    p_result->used  = p_a->used;
    
    t = 0;
    for (x = 0; x < oldbused; x++)
    {
        t                   = ((fp_word)p_a->value[x]) - (((fp_word)p_b->value[x]) + t);
        p_result->value[x]  = (fp_digit)t;
        t                   = (t >> (fp_digit)DIGIT_BIT) & 1u;
    }
    for (; x < p_a->used; x++)
    {
        t                   = ((fp_word)p_a->value[x]) - t;
        p_result->value[x]  = (fp_digit)t;
        t                   = (t >> (fp_digit)DIGIT_BIT) & 1u;
    }
    for (; x < oldused; x++)
    {
        p_result->value[x] = 0;
    }
    FP_CLAMP(p_result);
    return (ER_OK);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0009]:[ICUMFW_UD_L_CRY_BignumMath_0009]
* Function name: l_BIGNUM_CmpAbs
* Description  : The absolute value of a and b is compared, and it's the same price, ER_L_BIGNUM_EQ.
*                a > b, ER_L_BIGNUM_GT.
*                a < b returns ER_L_BIGNUM_LT.
* Arguments    : *p_a        : Address of comparison target a
*                              Range: 0x0 - 0xFFFFFFFF
*                *p_b        : Address of comparison target b
*                              Range: 0x0 - 0xFFFFFFFF
* Return Value : ER_L_BIGNUM_EQ
*                ER_L_BIGNUM_GT
*                ER_L_BIGNUM_LT
***************************************************************************************/
int32_t l_BIGNUM_CmpAbs(big_num_t *p_a, big_num_t *p_b)
{
    int32_t x;
    int32_t ret = ER_L_BIGNUM_EQ;

    if (p_a->used > p_b->used)
    {
        ret = ER_L_BIGNUM_GT;
    }
    else if (p_a->used < p_b->used)
    {
        ret = ER_L_BIGNUM_LT;
    }
    else
    {
        for (x = (int32_t)(p_a->used - 1u); x >= 0; x--)
        {
            if (p_a->value[x] > p_b->value[x])
            {
                ret = ER_L_BIGNUM_GT;
            }
            else if (p_a->value[x] < p_b->value[x])
            {
                ret = ER_L_BIGNUM_LT;
            }
            else
            {
                /* do nothing */
            }

            if (ER_L_BIGNUM_EQ != ret)
            {
                break;
            }
            else
            {
                /* do nothing */
            }
        }
    }

    return (ret);
}

