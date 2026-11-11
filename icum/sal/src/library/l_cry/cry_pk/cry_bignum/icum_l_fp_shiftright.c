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
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0020]:[ICUMFW_UD_L_CRY_BignumMath_0020]
* Function name: l_BIGNUM_DivideByPowerOf2
* Description  : quotient = a / (2**exponent)
*                Divide 2 by a value raised to the power of exponent to find 
*                the quotient and remainder.
* Arguments    : *p_a        : Address of dividend
*                              Range : 0x0 - 0xFFFFFFFF
*                 exponent   : The exponent of 2 which is the divisor
*                              Range : -2147483648 - 2147483647
*                *p_quotient : The address of the quotient obtained by multiplying a by a power of 2
*                              Range : 0x0 - 0xFFFFFFFF
*                *p_remainder: The address of the remainder obtained by multiplying a by a power of 2
*                              Range : 0x0 - 0xFFFFFFFF
* Return Value : ER_OK
***************************************************************************************/
uint32_t l_BIGNUM_DivideByPowerOf2(big_num_t *p_a, uint32_t exponent, big_num_t *p_quotient, big_num_t *p_remainder)
{
    fp_digit    D, r, rr;
    int32_t     x;

    /* if the shift count is <= 0 then we do no work */
    if (exponent <= 0u)
    {
        FP_COPY (p_a, p_quotient);
        if (p_remainder != NULL)
        {
            FP_INIT (p_remainder);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* get the remainder */
        if (p_remainder != NULL)
        {
            (void)l_BIGNUM_ModByPowerOf2(p_a, exponent, p_remainder);
        }
        else
        {
            /* Do nothing */
        }

        /* copy */
        FP_COPY(p_a, p_quotient);

        /* shift by as many digits in the bit count */
        if (exponent >= DIGIT_BIT)
        {
            (void)l_BIGNUM_ShiftRight(p_quotient, (int32_t)(exponent / DIGIT_BIT));
        }
        else
        {
            /* Do nothing */
        }

        /* shift any bit count < DIGIT_BIT */
        D = (fp_digit) ((fp_digit)exponent % (fp_digit)DIGIT_BIT);
        if (D != 0u)
        {
            register fp_digit *tmpc, mask, shift;

            /* mask */
            mask = (((fp_digit)1) << D) - 1u;

            /* shift for lsb */
            shift = (fp_digit)DIGIT_BIT - D;

            /* alias */
            tmpc = &p_quotient->value[p_quotient->used - 1u];

            /* carry */
            r = 0;
            for (x = (int32_t)(p_quotient->used - 1u); x >= 0; x--)
            {
                /* get the lower  bits of this word in a temp */
                rr = *tmpc & mask;

                /* shift the current word and mix in the carry bits from the previous word */
                *tmpc = (*tmpc >> D) | (r << shift);
                tmpc--;

                /* set the carry to the carry bits of the current word found above */
                r = rr;
            }
        }
        else
        {
            /* Do nothing */
        }

        FP_CLAMP (p_quotient);
    }

    return (ER_OK);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0021]:[ICUMFW_UD_L_CRY_BignumMath_0021]
* Function name: l_BIGNUM_ShiftRight
* Description  : Shift "a" to the right by "x".
* Arguments    : *p_a : Address of data to be shifted
*                       Range : 0x0 - 0xFFFFFFFF
*                 x   : Number of right shift bits
*                       Range : -2147483648 - 2147483647
* Return Value : ER_OK
***************************************************************************************/
uint32_t l_BIGNUM_ShiftRight(big_num_t *p_a, int32_t x)
{
    int32_t y;

    /* too many digits just zero and return */
    if ((uint32_t)x >= p_a->used)
    {
        FP_INIT(p_a);
    }
    else
    {
        /* shift */
        for (y = 0; (uint32_t)y < p_a->used - (uint32_t)x; y++)
        {
            p_a->value[y] = p_a->value[y + x];
        }

        /* zero rest */
        for (; (uint32_t)y < p_a->used; y++)
        {
            p_a->value[y] = 0;
        }
        
        /* decrement count */
        p_a->used -= (uint32_t)x;
        FP_CLAMP(p_a);
    }

    return (ER_OK);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0022]:[ICUMFW_UD_L_CRY_BignumMath_0022]
* Function name: l_BIGNUM_ModByPowerOf2
* Description  : result = a mod 2**exponent
* Arguments    : *p_a       : Address of modulo calculation target data
*                             Range : 0x0 - 0xFFFFFFFF
*                 exponent  : The exponent of 2 which is the modulo value
*                             Range : -2147483648 - 2147483647
*                *p_result  : Modulo calculation result storage address
*                             Range : 0x0 - 0xFFFFFFFF
* Return Value : ER_OK
***************************************************************************************/
uint32_t l_BIGNUM_ModByPowerOf2(big_num_t *p_a, uint32_t exponent, big_num_t *p_result)
{
    uint32_t    x;

    /* zero if count less than or equal to zero */
    if (exponent <= 0u)
    {
        FP_INIT(p_result);
    }
    else
    {
        /* get copy of input */
        FP_COPY(p_a, p_result);

        /* if 2**exponent is larger than we just return */
        if (exponent >= (DIGIT_BIT * p_a->used))
        {
            /* Do nothing */
        }
        else
        {
            /* zero digits above the last digit of the modulus */
            x = (exponent / DIGIT_BIT) + ((exponent % DIGIT_BIT) == 0u ? 0u : 1u);
            for (; x < p_result->used; x++)
            {
                p_result->value[x] = 0;
            }
            /* clear the digit that is not completely outside/inside the modulus */
            /* result->value[exponent / DIGIT_BIT] &= ~(0u) >> (DIGIT_BIT - exponent); */
            p_result->value[exponent / DIGIT_BIT] &= 0xFFFFFFFFu >> (DIGIT_BIT - exponent); 
            FP_CLAMP (p_result);
        }
    }

    return (ER_OK);
}

