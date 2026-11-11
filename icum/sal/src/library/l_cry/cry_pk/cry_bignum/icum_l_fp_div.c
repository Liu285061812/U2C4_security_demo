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
/* For the used GHS macro functions */
#include "icum_d_misc_pub.h"

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t l_BIGNUM_IsPowerOfTwo(fp_digit b, int32_t *p_digit, bool *p_result);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0027]:[ICUMFW_UD_L_CRY_BignumMath_0027]
* Function name: l_BIGNUM_IsPowerOfTwo
* Description  : It is judged whether b is a power of 2 or not.
* Arguments    :  b         : Value to verify
*                             Range : 0 - 4,294,967,295
*                *p_digit   : Address of the number of digits
*                             Range : 0x0 - 0xFFFFFFFF
*                *p_result  :
* Return Value : true(b is a power of 2)
*                false(b is not a power of 2)
***************************************************************************************/
static uint32_t l_BIGNUM_IsPowerOfTwo(fp_digit b, int32_t *p_digit, bool *p_result)
{
    int32_t x;

    *p_result = false;

    if ((b != 0u) && ((b & (b - 1u)) == 0u))
    {
        *p_result = false;
        for (x = 0; x < (int32_t)DIGIT_BIT; x++)
        {
            if (b == (1u << (uint32_t)x))
            {
                *p_digit  = x;
                *p_result = true;
                break;
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    else
    {
        /* Do nothing */
    }
    return (ER_OK);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0028]:[ICUMFW_UD_L_CRY_BignumMath_0028]
* Function name: l_BIGNUM_DivideByDigit
* Description  : a/b => quotient*b + remaider == a
*                big_num_t *quotient : should not be NULL
*                Divide a (big_num_t type) by b (32 bit data) to find the 
*                quotient and remainder.
* Arguments    : *p_a         : Address of dividend
*                               Range : 0x0 - 0xFFFFFFFF
*                 b           : Divisor
*                               Range : 0 - 4,294,967,295
*                *p_quotient  : Address of division quotient
*                               Range : 0x0 - 0xFFFFFFFF
*                *p_remaider  : Address of division remainder
*                               Range : 0x0 - 0xFFFFFFFF
* Return Value : ER_OK
*                ER_L_BIGNUM_VAL
***************************************************************************************/
uint32_t l_BIGNUM_DivideByDigit(big_num_t *p_a, fp_digit b, big_num_t *p_quotient, fp_digit *p_remaider)
{
    big_num_t  *p_q = p_quotient ;
    fp_word     w;
    fp_digit    t;
    int32_t     ix;
    uint32_t    ret;
    bool        flag;

    /* cannot divide by zero */
    if (b == 0u)
    {
        ret = ER_L_BIGNUM_VAL;
    }
    else if(p_quotient == NULL)
    {
        ret =  ER_L_BIGNUM_VAL;
    }
    else
    {
        /* quick outs */
        flag = FP_ISZERO(p_a);
        if (b == 1u || flag == true)
        {
            if (p_remaider != NULL)
            {
                *p_remaider = 0;
            }
            else
            {
                /* Do nothing */
            }
            FP_COPY(p_a, p_quotient);
            ret = ER_OK;
        }
        else
        {
            /* power of two ? */
            (void)l_BIGNUM_IsPowerOfTwo(b, &ix, &flag);
            if (flag == true)
            {
                if (p_remaider != NULL)
                {
                    *p_remaider = p_a->value[0] & ((1u << (uint32_t)ix) - 1u);
                }
                else
                {
                    /* Do nothing */
                }
                (void)l_BIGNUM_DivideByPowerOf2(p_a, (uint32_t)ix, p_quotient, NULL);
                ret = ER_OK;
            }
            else
            {
                /* no easy answer [c'est la vie].  Just division */

                p_q->used = p_a->used;
                p_q->sign = p_a->sign;
                w = 0;
                for (ix = (int32_t)(p_a->used - 1u); ix >= 0; ix--)
                {
                    w = (w << ((fp_word)DIGIT_BIT)) | ((fp_word)p_a->value[ix]);

                    if (w >= b)
                    {
                        t = (fp_digit)(w / b);
                        w -= ((fp_word)t) * ((fp_word)b);
                    }
                    else
                    {
                        t = 0;
                    }
                    p_q->value[ix] = (fp_digit)t;
                }

                if (p_remaider != NULL)
                {
                    *p_remaider = (fp_digit)w;
                }
                else
                {
                    /* Do nothing */
                }
                FP_CLAMP(p_q);
                ret = ER_OK;
            }
        }
    }

    return (ret);
}
