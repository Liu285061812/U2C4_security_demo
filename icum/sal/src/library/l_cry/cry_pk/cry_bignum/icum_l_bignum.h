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

#ifndef ICUM_L_BIGNUM_H
#define ICUM_L_BIGNUM_H

#include "icum_l_cry_pk_pub.h"
#include "../cry_common/icum_l_my_toolbox.h"

#ifndef MIN
   #define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef MAX
   #define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#ifndef FP_MAX_SIZE
/**********************************************************************************/
/* - symbol BIGNUM_SIZE_IN_BITS defined outside at project level                  */
/* #define FP_MAX_SIZE           (4096+(8*DIGIT_BIT))                             */
   #define FP_MAX_SIZE           ( (R_CRYPT_BIGNUM_BITSIZE * 2u) + (8u * DIGIT_BIT) )
/**********************************************************************************/
#endif


typedef uint32_t     fp_digit;
typedef uint64_t     fp_word;

/* # of digits this is */
#define DIGIT_BIT  ( 8u * sizeof(fp_digit) )
#define FP_MASK    (fp_digit)(-1)

/* signs */
#define FP_ZPOS     (0u)
#define FP_NEG      (1u)

/* less than */
#define ER_L_BIGNUM_LT      (-1)
/* equal to */
#define ER_L_BIGNUM_EQ      (0)
/* greater than */
#define ER_L_BIGNUM_GT      (1)

/**********************************************************************************
* Type definition
***********************************************************************************/
typedef struct big_num
{
    uint32_t   used;
    uint32_t   sign;
    uint32_t   value[BIGNUM_LONG_SIZE];
} big_num_t;

/**********************************************************************************
* global variable to replace the use of the symbol BIGNUM_LONG_SIZE
* this is to allow the use of smaller type fp_int_2
***********************************************************************************/
extern uint32_t g_crypt_fp_size;

/* functions */

/* initialize [or zero] an fp int */
/**********************************************************************************
* - to enjoy the use of smaller fp_int_2
***********************************************************************************/
#define FP_INIT(a)  do {                                                                                \
                        (a)->used = 0u;                                                                 \
                        (a)->sign = 0u;                                                                 \
                        l_CMN_MemSet((uint8_t *)(a)->value, 0, sizeof(fp_digit) * g_crypt_fp_size);     \
                    } while(0)

/* zero/even/odd ? */
#define FP_ISZERO(a) ( ((a)->used == 0u) ? true : false )

/* copy from a to b */
/**********************************************************************************
* - to enjoy the use of smaller fp_int_2
***********************************************************************************/

#define FP_COPY(a, b)   \
    do {                                                                                                            \
        if( (a) != (b) ) {                                                                                          \
            (b)->used = (a)->used;                                                                                  \
            (b)->sign = (a)->sign;                                                                                  \
            l_CMN_MemCpy((uint8_t *)(b)->value, (const uint8_t *)(a)->value, sizeof(fp_digit) * g_crypt_fp_size);   \
        }                                                                                                           \
        else                                                                                                        \
        {                                                                                                           \
            /* Do nothing */                                                                                        \
        }                                                                                                           \
    } while(0)

/* clamp digits */
#define FP_CLAMP(a) \
    do {                                                                \
        while ((a)->used != 0u && (a)->value[(a)->used - 1u] == 0u)     \
        {                                                               \
            ((a)->used)--;                                              \
        }                                                               \
        (a)->sign = ((a)->used != 0u) ? (a)->sign : FP_ZPOS;            \
    } while(0)


/* set to a small digit */
uint32_t l_BIGNUM_Set(big_num_t *p_a, fp_digit b);

/* right shift x digits */
uint32_t l_BIGNUM_ShiftRight(big_num_t *p_a, int32_t x);

/* power of 2 operations */
uint32_t l_BIGNUM_DivideByPowerOf2  (big_num_t *p_a, uint32_t exponent, big_num_t *p_quotient, big_num_t *p_remainder);
uint32_t l_BIGNUM_ModByPowerOf2     (big_num_t *p_a, uint32_t exponent, big_num_t *p_result);

/* Counts the number of lsbs which are zero before the first zero bit */
uint32_t l_BIGNUM_CountBits (const big_num_t *p_a, uint32_t *p_bit_size);

/* result = a + b */
uint32_t l_BIGNUM_Add       (big_num_t *p_a, big_num_t *p_b, big_num_t *p_result);
/* result = ||a|| + ||b|| */
uint32_t l_BIGNUM_AddAbs    (big_num_t *p_a, big_num_t *p_b, big_num_t *p_result);
/* result = a + b */
uint32_t l_BIGNUM_AddDigit  (big_num_t *p_a, fp_digit b, big_num_t *p_result);

/* result = ||a|| - ||b|| */
uint32_t l_BIGNUM_SubAbs    (big_num_t *p_a, big_num_t *p_b, big_num_t *p_result);

/* unsigned comparison */
int32_t l_BIGNUM_CmpAbs     (big_num_t *p_a, big_num_t *p_b);

/* result = a * b */
uint32_t l_BIGNUM_MultiplyByDigit   (big_num_t *p_a, fp_digit b, big_num_t *p_result);

/* a/b => quotient*b + remaider == a */
uint32_t l_BIGNUM_DivideByDigit (big_num_t *p_a, fp_digit b, big_num_t *p_quotient, fp_digit *p_remaider);
uint32_t l_PKC_KickCallBackWDT(void);


/* radix conversions */
uint32_t l_BIGNUM_ConvertFromBinary     (big_num_t *p_out, const uint8_t *p_in, int32_t size_in_bytes);
uint32_t l_BIGNUM_ConvertFromStrHex     (big_num_t *p_out, const char    *p_str);
uint32_t l_BIGNUM_ConvertFromStrRadix   (big_num_t *p_out, const char    *p_str, uint32_t radix);

uint32_t l_BIGNUM_ConvertToBinary       (const big_num_t *p_in, uint8_t *p_out, uint32_t *p_size_in_bytes);
uint32_t l_BIGNUM_ConvertToStrHex       (const big_num_t *p_in, char    *p_str);
uint32_t l_BIGNUM_ConvertToStrRadix     (const big_num_t *p_in, char    *p_str, uint32_t radix);
uint32_t l_BIGNUM_ConvertToStrRadixSlice(big_num_t *p_t, char *p_str, uint32_t radix, uint32_t str_index);

uint32_t l_BIGNUM_ReverseArray (uint8_t *p_array, uint32_t len);


#endif /* ICUM_L_BIGNUM_H */

