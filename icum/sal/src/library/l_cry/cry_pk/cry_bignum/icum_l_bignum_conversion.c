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

/*******************************************************************************
* Headers
********************************************************************************/

#include "icum_l_cry_pk_pub.h"

#include "icum_l_bignum.h"
#include "../cry_common/icum_l_my_toolbox.h"

/**********************************************************************************
 * Local defines
 **********************************************************************************/
#define MAX_SIZE    ((g_crypt_fp_size - 8u) * 2u)

/* unsigned data size */
#define NUM0                        (0u)
#define NUM4                        (4u)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BIGNUM_0009]:[ICUMFW_UD_L_CRY_BIGNUM_0009]
* Function name: L_BIGNUM_PaddingData
* Description  : Perform data padding.
* Arguments    : p_size_in_bytes:[in/out]   :byte size of p_value (befor encode / decode)
*                size_in_bytes  :[in]       :byte size of p_value (after encode / decode)
*                p_value        :[out]      :address of conversion value
*                zero           :[in]       :zero data
* Return Value : ER_OK
*                ER_L_BIGNUM_BUFFER_OVERFLOW
***************************************************************************************/
uint32_t L_BIGNUM_PaddingData(uint32_t *p_size_in_bytes, uint32_t size_in_bytes,
                                     uint8_t *p_value, uint8_t zero)
{
    uint32_t err;
    int32_t i;
    int32_t x;

    err = ER_OK;

    if (0u == *p_size_in_bytes)
    {
        /* provide the byte size resulting from the conversion */
        *p_size_in_bytes = size_in_bytes;
    }
    else
    {
        if (*p_size_in_bytes == size_in_bytes)
        {
            /* perfect! */
            /* nothing to do! */
        }
        else if ((*p_size_in_bytes) > size_in_bytes)
        {
            x = (int32_t)(*p_size_in_bytes) - (int32_t)size_in_bytes;

            /* there is less bytes than the byte number expected */
            /* fill-in with leading zeroes */
            for (i = ((int32_t)*p_size_in_bytes) - 1; i >= 0; i--)
            {
                *(uint8_t *)((uint8_t *)p_value + x + i) = *(uint8_t *)((uint8_t *)p_value + i );
            }
            for (i = 0; i < x; i++)
            {
                *((uint8_t *)p_value + i) = (uint8_t)zero;
            }
            err = ER_OK;
        }
        else
        {
            /* more bytes than the number of bytes expected */
            /* that's an error */
            *p_size_in_bytes = size_in_bytes;
            err = ER_L_BIGNUM_BUFFER_OVERFLOW;
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BIGNUM_0003]:[ICUMFW_UD_L_CRY_BIGNUM_0003]
* Function name: l_BIGNUM_Encode
* Description  : Convert to an internal number (big_num_t) from byte tables or strings(hexa, decimal).
* Arguments    : p_value        :[in]   :address of conversion source
*                value_type     :[in]   :big number type of p_value
*                size_in_bytes  :[in]   :byte size of p_value (only for E_BINARY_PLAIN)
*                p_big_num      :[out]  :address of output big number value
* Return Value : ER_OK
*                ER_PARAM
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
***************************************************************************************/
static uint32_t l_BIGNUM_Encode (const void              *p_value,
                                       big_num_type_t     value_type,
                                       uint32_t           size_in_bytes,
                                       pkc_data_t        *p_big_num)
{
    uint32_t len;
    uint32_t err;
    uint32_t n;
    big_num_t big_num;

    l_CMN_MemSet((uint8_t *)&big_num, NUM0, sizeof(big_num_t));

    switch (value_type)
    {
        case E_STRING_DECIMAL:
            /* this is a string of decimals to convert to big_num_t */
            /* check the size of the string first, then convert     */
            /* note: 2048-bit integer has max 617 decimals          */
            /*           2^2048 == 10^n                             */
            /*      i.e. 2048 * log(2) == n * log(10)               */
            /*      i.e. n == 2048 * log(2)                         */
            /* note: 3072-bit integer has max 924 decimals          */
            /*           n == 3072 * log(2)                         */
            /* n below is calculated with approx value of log(2)    */
            /* i.e. 0,30102999 = 3010 * 10000                       */
            /*                                                      */
            /* size_in_bytes is the number of characters in the string */
            /* so it should be compared to n                           */
            /*                                                         */
            n = ((3010u * MAX_SIZE * 8u) / 10000u) + 1u;
            len = l_CMN_StrLen(p_value);
            if (len <= n)
            {
                if (len != size_in_bytes)
                {
                    err = ER_PARAM;
                }
                else
                {
                    err = l_BIGNUM_ConvertFromStrRadix(&big_num, (const char *)p_value, 10u);
                }

                if (ER_OK == err)
                {
                    (void)l_BIGNUM_ConvertToBinary(&big_num, (uint8_t *)p_big_num->value, &p_big_num->size);
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                err = ER_L_BIGNUM_BUFFER_OVERFLOW;
            }
            break;
        case E_STRING_HEXA:
            /* this is a string of hexadecimals to convert to big_num_t */
            /* check the size of the string first, then convert */
            /* note: 2 characters per byte ("F1F2F3" -> 6/2 = 3 bytes) */
            len = l_CMN_StrLen(p_value);
            if (len <= (MAX_SIZE * 2u))
            {
                if (len != size_in_bytes)
                {
                    err = ER_PARAM;
                }
                else
                {
                    err = l_BIGNUM_ConvertFromStrHex(&big_num, (const char *)p_value);
                }

                if (ER_OK == err)
                {
                    (void)l_BIGNUM_ConvertToBinary(&big_num, (uint8_t *)p_big_num->value, &p_big_num->size);
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                err = ER_L_BIGNUM_BUFFER_OVERFLOW;
            }
            break;
        case E_BINARY_PLAIN:
            /* this is a value in bits: its size should be given in size_in_bytes */
            if ((size_in_bytes != 0u) && (size_in_bytes <= MAX_SIZE))
            {
                l_CMN_MemCpy((uint8_t *)p_big_num->value, (const uint8_t *)p_value, size_in_bytes);
                p_big_num->size = size_in_bytes;
                err = ER_OK;
            }
            else
            {
                err = ER_L_BIGNUM_BUFFER_OVERFLOW;
            }
            break;
        default:
            /* unsupported type */
            err = ER_L_BIGNUM_INVALID_ARG;
            break;
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BIGNUM_0006]:[ICUMFW_UD_L_CRY_BIGNUM_0006]
* Function name: l_BIGNUM_Decode
* Description  : Convert to byte tables or strings(hexa, decimal) from an internal number .
*                The type and size of the big number (if converting to a binary form)
*                should be provided before the call 
* Arguments    : p_big_num          :[in]       :address of conversion source
*                value_type         :[in]       :big number type of p_value
*                p_value            :[out]      :address of output value
*                p_size_in_bytes    :[in/out]   :byte size of p_value
* Return Value : ER_OK
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
***************************************************************************************/
static uint32_t l_BIGNUM_Decode (const pkc_data_t      *p_big_num,
                                       big_num_type_t   value_type,
                                       void            *p_value,
                                       uint32_t        *p_size_in_bytes)
{
    uint32_t  size_in_bytes;
    uint32_t  err;
    int8_t    zero;
    big_num_t big_num;

    /* Initialize parameters */
    err = ER_OK;
    size_in_bytes = 0u;
    zero = 0;
    l_CMN_MemSet((uint8_t *)&big_num, NUM0, sizeof(big_num_t));

    switch (value_type)
    {
        case E_STRING_DECIMAL:
            (void)l_BIGNUM_ConvertFromBinary(&big_num, (const uint8_t *)p_big_num->value, (int32_t)p_big_num->size);
            err = l_BIGNUM_ConvertToStrRadix(&big_num, p_value, 10u);
            size_in_bytes = l_CMN_StrLen(p_value);
            zero = (int8_t)'0';
            break;
        case E_STRING_HEXA:
            (void)l_BIGNUM_ConvertFromBinary(&big_num, (const uint8_t *)p_big_num->value, (int32_t)p_big_num->size);
            err = l_BIGNUM_ConvertToStrHex(&big_num, p_value);
            size_in_bytes = l_CMN_StrLen(p_value);
            zero = (int8_t)'0';
            break;
        case E_BINARY_PLAIN:
            (void)l_CMN_MemCpy(p_value, (const uint8_t *)p_big_num->value, (uint32_t)p_big_num->size);
            size_in_bytes = p_big_num->size;
            zero = (int8_t)0;
            break;
        default:
            /* unsupported type */
            err = ER_L_BIGNUM_INVALID_ARG;
            break;
    }

    if (err != ER_L_BIGNUM_INVALID_ARG)
    {
        err = L_BIGNUM_PaddingData(p_size_in_bytes, size_in_bytes, p_value, (uint8_t)zero);
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BIGNUM_0001]:[ICUMFW_UD_L_CRY_BIGNUM_0001]
* Function name: L_BIGNUM_EncodeToBigNum
* Description  : Convert to big number from byte tables or strings(hexa, decimal).
* Arguments    : p_value        :[in]       : address of conversion source
*                value_type     :[in]       : big number type of p_value
*                size_in_bytes  :[in]       : byte size of p_value (only for E_BINARY_PLAIN)
*                p_big_num      :[out]      : address of output big number value
* Return Value : ER_OK
*                ER_PARAM
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
***************************************************************************************/
uint32_t L_BIGNUM_EncodeToBigNum(const void             *p_value,
                                       big_num_type_t    value_type,
                                       uint32_t          size_in_bytes,
                                       pkc_data_t       *p_big_num)
{
    uint32_t    err;
    g_crypt_fp_size = BIGNUM_LONG_SIZE;
    err = l_BIGNUM_Encode(p_value, value_type, size_in_bytes, p_big_num);
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BIGNUM_0002]:[ICUMFW_UD_L_CRY_BIGNUM_0002]
* Function name: L_BIGNUM_EncodeToSmallBigNum
* Description  : Convert to "small" big number from byte tables or strings(hexa, decimal).
* Arguments    : p_value        :[in]       : address of conversion source
*                value_type     :[in]       : big number type of p_value
*                size_in_bytes  :[in]       : byte size of p_value (only for E_BINARY_PLAIN)
*                p_big_num      :[out]      : address of output "small" big number value
* Return Value : ER_OK
*                ER_PARAM
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
***************************************************************************************/
uint32_t L_BIGNUM_EncodeToSmallBigNum(const void            *p_value,
                                            big_num_type_t   value_type,
                                            uint32_t         size_in_bytes,
                                            pkc_small_data_t *p_big_num)
{
    uint32_t    err;
    /* use the smaller size of fp_int to save space in stack */
    g_crypt_fp_size = SMALLBIGNUM_LONG_SIZE;
    err = l_BIGNUM_Encode(p_value, value_type, size_in_bytes, (pkc_data_t *)p_big_num);
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BIGNUM_0004]:[ICUMFW_UD_L_CRY_BIGNUM_0004]
* Function name: L_BIGNUM_DecodeFromBigNum
* Description  : Convert to byte tables or strings(hexa, decimal) from big number .
* Arguments    : p_big_num      :[in]       : address of conversion source
*                value_type     :[in]       : big number type of p_value
*                p_value        :[out]      : address of output value
*                p_size_in_bytes:[in/out]   : byte size of p_value
* Return Value : ER_OK
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
***************************************************************************************/
uint32_t L_BIGNUM_DecodeFromBigNum(      pkc_data_t         *p_big_num,
                                         big_num_type_t      value_type,
                                         void               *p_value,
                                         uint32_t           *p_size_in_bytes)
{
    uint32_t    err;
    g_crypt_fp_size = BIGNUM_LONG_SIZE;
    err = l_BIGNUM_Decode(p_big_num, value_type, p_value, p_size_in_bytes);
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BIGNUM_0005]:[ICUMFW_UD_L_CRY_BIGNUM_0005]
* Function name: L_BIGNUM_DecodeFromSmallBigNum
* Description  : Convert to byte tables or strings(hexa, decimal) from "small" big number .
* Arguments    : p_big_num      :[in]       : address of conversion source
*                value_type     :[in]       : big number type of p_value
*                p_value        :[out]      : address of output value
*                p_size_in_bytes:[in/out]   : byte size of p_value
* Return Value : ER_OK
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
***************************************************************************************/
uint32_t L_BIGNUM_DecodeFromSmallBigNum(      pkc_small_data_t *p_big_num,
                                              big_num_type_t    value_type,
                                              void             *p_value,
                                              uint32_t         *p_size_in_bytes)
{
    uint32_t    err;
    /* use the smaller size of fp_int to save space in stack */
    g_crypt_fp_size = SMALLBIGNUM_LONG_SIZE;
    err = l_BIGNUM_Decode((const pkc_data_t *)p_big_num, value_type, p_value, p_size_in_bytes);
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BIGNUM_0007]:[ICUMFW_UD_L_CRY_BIGNUM_0007]
* Function name: L_BIGNUM_DecodeFromBigNumSlice
* Description  : Calling this function once divides big_num once in radix. 
*                We set the remainder at that time to p_value [str_index].
*                ER_OK is returned when big_num becomes 0.
*                Note: The user must increment str_index every time this function is called
* Arguments    : p_big_num  :[in/out]   :address of conversion source
*                value_type :[in]       :big number type of p_value
*                                        Range: E_STRING_DECIMAL
*                                               E_STRING_HEXA
*                p_value    :[out]      :address of output value
*                str_index  :[in]       :pointer of p_value
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_L_BIGNUM_INVALID_ARG
***************************************************************************************/
uint32_t L_BIGNUM_DecodeFromBigNumSlice(      pkc_data_t       *p_big_num,
                                              big_num_type_t   value_type,
                                              void             *p_value,
                                              uint32_t         str_index)
{
    uint32_t radix;
    uint32_t err = ER_OK;
    big_num_t big_num;

    l_CMN_MemSet((uint8_t *)&big_num, NUM0, sizeof(big_num));

    g_crypt_fp_size = BIGNUM_LONG_SIZE;
    switch (value_type)
    {
        case E_STRING_DECIMAL:
            radix = 10u;
            break;
        case E_STRING_HEXA:
            radix = 16u;
            break;
        default:
            err = ER_L_BIGNUM_INVALID_ARG;
            break;
    }
    
    if(err == ER_OK)
    {
        (void)l_BIGNUM_ConvertFromBinary(&big_num, (const uint8_t *)p_big_num->value, (int32_t)p_big_num->size);
        err = l_BIGNUM_ConvertToStrRadixSlice(&big_num, (char *)p_value, radix, str_index);
        (void)l_BIGNUM_ConvertToBinary(&big_num, (uint8_t *)p_big_num->value, &p_big_num->size);
    }
    else
    {
        /* Do nothing */
    }
    
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BIGNUM_0008]:[ICUMFW_UD_L_CRY_BIGNUM_0008]
* Function name: L_BIGNUM_CountBits
* Description  : Count the number of effective bits of a.
* Arguments    : *a          :[in]    :Address of BigNum data for which bit size is to be obtained
*              : *p_bit_size :[out]   :Bit size of BigNum data
* Return Value : ER_OK
***************************************************************************************/
uint32_t L_BIGNUM_CountBits (const pkc_data_t *p_a, uint32_t *p_bit_size)
{
    big_num_t big_num;

    g_crypt_fp_size = BIGNUM_LONG_SIZE;
    (void)l_BIGNUM_ConvertFromBinary(&big_num, (const uint8_t *)p_a->value, (int32_t)p_a->size);
    (void)l_BIGNUM_CountBits(&big_num, p_bit_size);

    return ER_OK;
}
