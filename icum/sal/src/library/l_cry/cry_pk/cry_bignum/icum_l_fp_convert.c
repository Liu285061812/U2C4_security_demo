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
#include "../cry_common/icum_l_my_toolbox.h"

/* chars used in radix conversions */
static const char *fp_s_rmap = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0010]:[ICUMFW_UD_L_CRY_BignumMath_0010]
* Function name: l_BIGNUM_ConvertFromStrRadix
* Description  : Convert to an internal number (big_num_t) from radix string.
*                Note: The assumed use case is only decimal numbers (radix 10).
*                      In case of hexa number, please use 'l_BIGNUM_ConvertFromStrHex'.
* Arguments    : p_out    [out]  :address of output value
*                p_str    [in]   :address of conversion source (string)
*                radix    [in]   :radix
* Return Value : ER_OK
*                ER_L_BIGNUM_INVALID_ARG
***************************************************************************************/
uint32_t l_BIGNUM_ConvertFromStrRadix(big_num_t *p_out, const char *p_str, uint32_t radix)
{
    uint32_t    y, neg;
    int8_t      ch;
    uint32_t    err;
    uint32_t    map_len;
    bool        flag;

    /* make sure the radix is ok */
    if ((radix < 2u) || (radix > 64u))
    {
        err = ER_L_BIGNUM_INVALID_ARG;
    }
    else
    {
        /* if the leading digit is a minus set the sign to negative. */
        if (*p_str == '-')
        {
            p_str++;
            neg = FP_NEG;
        }
        else
        {
            neg = FP_ZPOS;
        }

        /* set the integer to the default of zero */
        FP_INIT (p_out);

        /* process each digit of the string */
        while (*p_str != (char)0)
        {
            /* if the radix < 36 the conversion is case insensitive
            * this allows numbers like 1AB and 1ab to represent the same  value
            * [e.g. in hex]
            */
            ch = (radix <= 36u) ? (int8_t)l_CMN_ToUpper(*p_str) : (int8_t)*p_str;
            map_len = l_CMN_StrLen(fp_s_rmap);
            for (y = 0u; y < map_len; y++)
            {
                if (ch == fp_s_rmap[y])
                {
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }

            /* if the char was found in the map
            * and is less than the given radix add it
            * to the number, otherwise exit the loop.
            */
            if (y < radix)
            {
                (void)l_BIGNUM_MultiplyByDigit (p_out, (fp_digit)radix, p_out);
                (void)l_BIGNUM_AddDigit (p_out, (fp_digit)y, p_out);
            }
            else
            {
                break;
            }
            p_str++;
        }

        /* set the sign only if p_out != 0 */
        flag = FP_ISZERO(p_out);
        if (flag != true)
        {
            p_out->sign = neg;
        }
        else
        {
            /* Do nothing */
        }
        
        err = ER_OK;
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0058]:[ICUMFW_UD_L_CRY_BignumMath_0058]
* Function name: l_BIGNUM_HexCharToNum
* Description  : Convert to integer from hexa char.
* Arguments    : c  :[in]  :hexa char
*                           Range '0'-'9'
*                                 'a'-'z'
*                                 'A'-'Z'
* Return Value : converted number
                 -1
***************************************************************************************/
static uint32_t l_BIGNUM_HexCharToNum(const char c, int32_t *p_int_value)
{
    int32_t ret;
    if ('0' <= c && c <= '9')
    {
        ret = c - '0';
    }
    else if ('A' <= c && c <= 'Z')
    {
        ret = c - 'A' + 10;
    }
    else if ('a' <= c && c <= 'z')
    {
        ret = c - 'a' + 10;
    }
    else
    {
        ret = -1;
    }
    *p_int_value = ret;
    
    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0011]:[ICUMFW_UD_L_CRY_BignumMath_0011]
* Function name: l_BIGNUM_ConvertFromStrHex
* Description  : Convert to an internal number (big_num_t) from hexa string.
* Arguments    : p_out     :[out]  :address of output value
*                p_str     :[in]   :address of conversion source (hexa string)
* Return Value : ER_OK
***************************************************************************************/
uint32_t l_BIGNUM_ConvertFromStrHex(big_num_t *p_out, const char *p_str)
{
    uint32_t    neg;
    uint32_t    digit;
    int32_t     value_index, str_offset, str_length, str_index, int_value;
    int8_t      c;
    bool        flag;
    
    /* set the integer to the default of zero */
    FP_INIT (p_out);
    
    if ((p_str == NULL) || (*p_str == '\0'))
    {
        /* Do nothing */
    }
    else
    {
        /* if the leading digit is p_out
        * minus set the sign to negative.
        */
        if (*p_str == '-')
        {
            p_str++;
            neg = FP_NEG;
        }
        else
        {
            neg = FP_ZPOS;
        }
        str_length = (int32_t)l_CMN_StrLen(p_str);
        
        /* process each digit of the string */
        str_index = str_length;
        str_offset = 0;
        value_index = 0;
        while (str_index > 0)
        {
            str_offset = ((int32_t)(sizeof(uint32_t) * 2u) <= str_index) ? (int32_t)(sizeof(uint32_t) * 2u) : str_index;
            digit = 0u;
            while (true)
            {
                c = (int8_t)p_str[str_index - str_offset];
                (void) l_BIGNUM_HexCharToNum(c, &int_value);
                if (int_value < 0)
                {
                    int_value = 0;
                }
                else
                {
                    /* Do nothing */
                }
                digit = (digit << 4) | (uint32_t)int_value;
                str_offset--;
                if (str_offset <= 0)
                {
                    p_out->value[value_index++] = digit;
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
            str_index -= (int32_t)(sizeof(uint32_t) * 2u);
        }
        p_out->used = (uint32_t)value_index;
        FP_CLAMP(p_out);
        /* set the sign only if p_out != 0 */
        flag = FP_ISZERO(p_out);
        if (flag != true)
        {
            p_out->sign = neg;
        }
        else
        {
            /* Do nothing */
        }
    }

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0012]:[ICUMFW_UD_L_CRY_BignumMath_0012]
* Function name: l_BIGNUM_ConvertFromBinary
* Description  : Convert to an internal number (big_num_t) from byte tables.
* Arguments    : p_out          :[out]  :address of output value
*                p_in           :[in]   :address of conversion source (byte tables)
*                size_in_bytes  :[in]   :byte size of p_in
* Return Value : uint32_t  (ER_OK)
***************************************************************************************/
uint32_t l_BIGNUM_ConvertFromBinary(big_num_t *p_out, const uint8_t *p_in, int32_t size_in_bytes)
{
    /* zero the int */
    uint8_t *pd;
    FP_INIT (p_out);
    pd = (uint8_t *)p_out->value;
    
    if ((uint32_t)size_in_bytes > (g_crypt_fp_size * sizeof(fp_digit)))
    {
        int32_t excess = (int32_t)(size_in_bytes - (g_crypt_fp_size * sizeof(fp_digit)));
        size_in_bytes -= excess;
        p_in          += excess;
    }
    else
    {
        /* Do nothing */
    }

    p_out->used = (size_in_bytes + sizeof(fp_digit) - 1u) / sizeof(fp_digit);
    
    /* read the bytes in */
    for (size_in_bytes -= 1; size_in_bytes >= 0; size_in_bytes -= 1)
    {
        pd[size_in_bytes] = *p_in++;
    }
    
    FP_CLAMP (p_out);
    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0013]:[ICUMFW_UD_L_CRY_BignumMath_0013]
* Function name: l_BIGNUM_ConvertToStrRadix
* Description  : Convert to radix string from an internal number (big_num_t).
*                Note: The assumed use case is only decimal numbers (radix 10).
*                      In case of hexa number, please use 'l_BIGNUM_ConvertToStrHex'.
* Arguments    : p_in     [in]   :address of conversion source
*                p_str    [out]  :address of output value
*                radix    [in]   :radix
* Return Value : ER_OK
*                ER_L_BIGNUM_INVALID_ARG
***************************************************************************************/
uint32_t l_BIGNUM_ConvertToStrRadix(const big_num_t *p_in, char *p_str, uint32_t radix)
{
    uint32_t    digs;
    big_num_t   t;
    fp_digit    d;
    int8_t      *p_s = (int8_t *)p_str;
    bool        flag;
    uint32_t    err;
    
    /* set the integer to the default of zero */
    FP_INIT (&t);

    /* check range of the radix */
    if ((radix < 2u) || (radix > 64u))
    {
        err = ER_L_BIGNUM_INVALID_ARG;
    }
    else
    {
        /* quick out if its zero */
        flag = FP_ISZERO(p_in);
        if (flag == true)
        {
            *p_str++ = '0';
            *p_str = '\0';
            err = ER_OK;
        }
        else
        {
            FP_COPY(p_in, &t);

            /* if it is negative output a - */
            if (t.sign == FP_NEG)
            {
                p_s++;
                *p_str++ = '-';
                t.sign = FP_ZPOS;
            }
            else
            {
                /* Do nothing */
            }

            digs = 0u;
            while (FP_ISZERO (&t) == false)
            {
                (void)l_BIGNUM_DivideByDigit (&t, (fp_digit) radix, &t, &d);
                *p_str++ = fp_s_rmap[d];
                digs++;
            }

            /* reverse the digits of the string.  In this case p_s points
            * to the first digit [excluding the sign] of the number]
            */
            (void)l_BIGNUM_ReverseArray ((uint8_t *)p_s, digs);

            /* append a NULL so the string is properly terminated */
            *p_str = '\0';
            err = ER_OK;
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0014]:[ICUMFW_UD_L_CRY_BignumMath_0014]
* Function name: l_BIGNUM_ConvertToStrHex
* Description  : Convert to hexa string from an internal number (big_num_t).
* Arguments    : p_in      [in]   :address of conversion source
*                p_str     [out]  :address of output value
* Return Value : ER_OK
***************************************************************************************/
uint32_t l_BIGNUM_ConvertToStrHex(const big_num_t *p_in, char *p_str)
{
    int32_t     i;
    int32_t     shiftbit;
    uint8_t     nibble;
    bool        isleadingZero = true;
    bool        flag;

    /* quick out if its zero */
    flag = FP_ISZERO(p_in);
    if (flag == true)
    {
        *p_str++ = '0';
        *p_str = '\0';
    }
    else
    {
        /* if it is negative output a - */
        if (p_in->sign == FP_NEG)
        {
            *(p_str++) = '-';
        }
        else
        {
            /* Do nothing */
        }

        for (i = (int32_t)(p_in->used - 1u); i >= 0; i--)
        {
            for (shiftbit = (int32_t)(DIGIT_BIT - 4u); shiftbit >= 0; shiftbit -= 4)
            {
                nibble = ((uint8_t)(p_in->value[i] >> (uint32_t)shiftbit)) & 0x0fu;
                if ((isleadingZero == false) || (nibble != 0))
                {
                    /* lower 4bit */
                    *(p_str++) = fp_s_rmap[nibble];
                    isleadingZero = false;
                }
                else
                {
                    /* Do nothing */
                }
            }
        }

        /* append a NULL so the string is properly terminated */
        *p_str = '\0';
    }

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0056]:[ICUMFW_UD_L_CRY_BignumMath_0056]
* Function name: l_BIGNUM_ConvertToStrRadixSlice
* Description  : Calling this function once divides big_num once in radix. 
*                We set the remainder at that time to p_value [str_index].
*                ER_OK is returned when big_num becomes 0.
*                Note: The user must increment str_index every time this function is called
* Arguments    : p_t         [in/out]   :address of conversion source
*                p_str       [out]      :address of output value
*                radix       [in]       :radix
*                str_index   [in]       :pointer of 'str'
* Return Value : ER_OK
*                ER_IN_PROCESS
***************************************************************************************/
uint32_t l_BIGNUM_ConvertToStrRadixSlice(big_num_t *p_t, char *p_str, uint32_t radix, uint32_t str_index)
{
    bool        flag;
    fp_digit    d;
    uint32_t    err;

    flag = FP_ISZERO(p_t);
    if (flag == true)
    {
        /* done */
        if (str_index == 0u)
        {
            /* quick out if its zero */
            p_str[0] = '0';
            p_str[1] = '\0';
        }
        else
        {
            /* reverse the digits of the string.  In this case _s points
             * to the first digit [excluding the sign] of the number]
             */
            (void)l_BIGNUM_ReverseArray((uint8_t *)&p_str[0], str_index);

            /* append a NULL so the string is properly terminated */
            p_str[str_index] = '\0';
        }
        err = ER_OK;
    }
    else
    {
        (void)l_BIGNUM_DivideByDigit(p_t, (fp_digit)radix, p_t, &d);
        p_str[str_index] = fp_s_rmap[d];

        err = ER_IN_PROCESS;
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0015]:[ICUMFW_UD_L_CRY_BignumMath_0015]
* Function name: l_BIGNUM_ConvertToBinary
* Description  : Convert to byte tables from an internal number (big_num_t).
* Arguments    : p_in              [in]   :address of conversion source
*                p_out             [out]  :address of output value
*                p_size_in_bytes   [out]  :the number of bytes written
* Return Value : ER_OK
***************************************************************************************/
uint32_t l_BIGNUM_ConvertToBinary(const big_num_t *p_in, uint8_t *p_out, uint32_t *p_size_in_bytes)
{
    uint32_t    remain_bytes, out_size, in_bit_size;
    fp_digit    tmp;
    
    out_size = 0u;

    (void)l_BIGNUM_CountBits(p_in, &in_bit_size);
    remain_bytes = (in_bit_size + 7u) / 8u;
    while(remain_bytes != 0u)
    {
        remain_bytes--;
        tmp = p_in->value[remain_bytes / sizeof(fp_digit)];
        /* extract 8bit */
        *(p_out++) = (uint8_t)(tmp >> (8u * (remain_bytes % sizeof(fp_digit)))) & 255u;
        out_size++;
    }
    *p_size_in_bytes = out_size;

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0016]:[ICUMFW_UD_L_CRY_BignumMath_0016]
* Function name: l_BIGNUM_ReverseArray
* Description  : reverse an array, used for radix code.
* Arguments    : p_array    [in/out]   :address of reversed array
*                len        [in]       :length of 's'
* Return Value : uint32_t  (ER_OK)
***************************************************************************************/
uint32_t l_BIGNUM_ReverseArray(uint8_t *p_array, uint32_t len)
{
    int32_t     ix, iy;
    uint8_t     t;

    ix = 0;
    iy = (int32_t)len - 1;
    while (ix < iy)
    {
        t           = p_array[ix];
        p_array[ix] = p_array[iy];
        p_array[iy] = t;
        ix++;
        iy--;
    }
    return (ER_OK);
}
