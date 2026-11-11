/******************************************************************************/
/* Component Name   Cryptographic Library X509                                */
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

#include "icum_l_cry_pk_pub.h"
#include "icum_l_x509.h"

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_X509_0011]:[ICUMFW_UD_L_CRY_X509_0011]
* Function name: l_X509_CountBitsOfLong
* Description  : Get MSB of x
* Arguments    : x          [in]   :target long word
*                p_bitlen   [out]  :The number of significant bits
* Return Value : ER_OK
***************************************************************************************/
static uint32_t l_X509_CountBitsOfLong(uint32_t x, uint32_t *p_bitlen)
{
    uint32_t counter;
    
    x &= 0xFFFFFFFFu;
    counter  = 0;
    while (x != 0u)
    {
        counter++;
        x >>= 1u;
    }
    *p_bitlen = counter;
    
    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_X509_0012]:[ICUMFW_UD_L_CRY_X509_0012]
* Function name: l_X509_GetLengthOfOidDerFormat
* Description  : Gets length of DER encoding of Object Identifier
* Arguments    : p_words    [in]   :The actual OID words to get the size of
*                nwords     [in]   :The number of OID words
*                p_outlen   [out]  :The length of the DER encoding for the given string
* Return Value : ER_OK
*                ER_L_X509_INVALID_ARG
***************************************************************************************/
uint32_t l_X509_GetLengthOfOidDerFormat ( const uint32_t *p_words, 
                                                uint32_t nwords,
                                                uint32_t *p_outlen)
{
    uint32_t    y, z, bitlen, wordbuf;
    uint32_t    err = ER_OK;
    
    if (p_words == NULL || p_outlen == NULL)
    {
        err = ER_L_X509_INVALID_ARG;
    }
    /* must be >= 2 words */
    else if (nwords < 2u)
    {
        err = ER_L_X509_INVALID_ARG;
    }
    else if ((p_words[0] > 3u) || ((p_words[0] < 2u) && (p_words[1] > 39u)))
    {
        err = ER_L_X509_INVALID_ARG;
    }
    else
    {
        /* leading word is the first two */
        z = 0u;
        wordbuf = p_words[0] * 40u + p_words[1];
        for (y = 1; y < nwords; y++)
        {
            (void)l_X509_CountBitsOfLong(wordbuf, &bitlen);
            z += (bitlen / 7u) + (((bitlen % 7u) != 0u)  ? 1u : 0u) + ((wordbuf == 0u) ? 1u : 0u);
            if (y < (nwords - 1u))
            {
                /* grab next word */
                wordbuf = p_words[y + 1u];
            }
            else
            {
                /* Do nothing */
            }
        }

        /* now depending on the length our length encoding changes */
        if (z < 128u)
        {
            z += 2u;
        }
        else if (z < 256u)
        {
            z += 3u;
        }
        else if (z < 65536u)
        {
            z += 4u;
        }
        else
        {
            err = ER_L_X509_INVALID_ARG;
        }

        if (ER_OK == err)
        {
            *p_outlen = z;
        }
        else
        {
            /* Do nothing */
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_X509_0010]:[ICUMFW_UD_L_CRY_X509_0010]
* Function name: l_X509_EncodeOidToDerFormat
* Description  : Encode an OID to der format
* Arguments    : p_words    [in]       :The words to encode  (upto 32-bits each)
*                nwords     [in]       :The number of words in the OID
*                p_out      [out]      :Destination of OID data
                 p_outlen   [in/out]   :The max and resulting size of the OID
* Return Value : ER_OK
*                ER_L_X509_INVALID_ARG
*                ER_L_X509_BUFFER_OVERFLOW
***************************************************************************************/
uint32_t l_X509_EncodeOidToDerFormat (const uint32_t   *p_words,
                                            uint32_t   nwords,
                                            uint8_t    *p_out,
                                            uint32_t   *p_outlen)
{
    uint32_t    i, x, y, z, bitlen, mask, wordbuf;
    uint32_t    err;

    if ((p_words == NULL) || (p_out == NULL) || (p_outlen == NULL))
    {
        err = ER_L_X509_INVALID_ARG;
    }
    else
    {
        /* check length */
        err = l_X509_GetLengthOfOidDerFormat(p_words, nwords, &x);
    }

    if (err != ER_OK)
    {
        /* Do nothing */
    }
    else if (x > *p_outlen)
    {
        *p_outlen = x;
        err = ER_L_X509_BUFFER_OVERFLOW;
    }
    else
    {
        /* compute length to store OID data */
        z = 0;
        wordbuf = p_words[0] * 40u + p_words[1];
        for (y = 1; y < nwords; y++)
        {
            (void)l_X509_CountBitsOfLong(wordbuf, &bitlen);
            z += (bitlen / 7u) +  (((bitlen % 7u) != 0u) ? 1u : 0u) + ((wordbuf == 0u) ? 1u : 0u);
            if (y < (nwords - 1u))
            {
                wordbuf = p_words[y + 1u];
            }
            else
            {
                /* Do nothing */
            }
        }

        /* store header + length */
        p_out[0] = ASN1_OBJECT_IDENTIFIER;
        if (z < 128u)
        {
            p_out[1] = (uint8_t)z;
            x = 2;
        }
        else if (z < 256u)
        {
            p_out[1] = 0x81;
            p_out[2] = (uint8_t)z;
            x = 3;
        }
        else if (z < 65536u)
        {
            p_out[1] = 0x82;
            p_out[2] = (uint8_t)((z >> 8u) & 255u);
            p_out[3] = (uint8_t)(z & 255u);
            x = 4;
        }
        else
        {
            err = ER_L_X509_INVALID_ARG;
        }

        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            /* store first byte */
            wordbuf = p_words[0] * 40u + p_words[1];
            for (i = 1; i < nwords; i++)
            {
                /* store 7 bit words in little endian */
                bitlen    = wordbuf & 0xFFFFFFFFU;
                if (bitlen != 0u)
                {
                    y    = x;
                    mask = 0;
                    while (bitlen != 0u)
                    {
                        p_out[x++] = (uint8_t)((bitlen & 0x7Fu) | mask);
                        bitlen >>= 7u;
                        /* upper bit is set on all but the last byte */
                        mask |= 0x80u;
                    }
                    /* now swap bytes y...x-1 */
                    z = x - 1u;
                    while (y < z)
                    {
                        bitlen   = p_out[y];
                        p_out[y] = p_out[z];
                        p_out[z] = (uint8_t)bitlen;
                        y++;
                        z--;
                    }
                }
                else
                {
                /* zero word */
                p_out[x++] = 0x00;
                }

                if (i < (nwords - 1u)) {
                    wordbuf = p_words[i + 1u];
                }
                else
                {
                    /* Do nothing */
                }
            }

            *p_outlen = x;
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_X509_0009]:[ICUMFW_UD_L_CRY_X509_0009]
* Function name: l_X509_EncodeOidToDerFormat
* Description  : Decode OID data and store the array of integers in words
* Arguments    : p_in       [in]       :The OID DER encoded data
*                inlen      [in]       :The length of the OID data
*                p_words    [out]      :The destination of the OID words
                 p_outlen   [in/out]   :he number of OID words
* Return Value : ER_OK
*                ER_L_X509_INVALID_ARG
*                ER_L_X509_BUFFER_OVERFLOW
***************************************************************************************/
uint32_t l_X509_DecodeOidFromDerFormat(const uint8_t  *p_in,
                                             uint32_t  inlen,
                                             uint32_t *p_words,
                                             uint32_t *p_outlen)
{
    uint32_t    x, y, t, len;
    uint32_t    err = ER_OK;

    if ((p_in == NULL) || (p_words == NULL) || (p_outlen == NULL))
    {
        err = ER_L_X509_INVALID_ARG;
    }
    else
    {
        x = 0;
        len = inlen;

        /* decode words */
        y = 0u;
        t = 0u;
        while (len != 0u)
        {
            t = (t << 7u) | (p_in[x] & 0x7Fu);
            if ((p_in[x] & 0x80u) == 0u)
            {
                /* store t */
                if (y >= *p_outlen)
                {
                    err = ER_L_X509_BUFFER_OVERFLOW;
                    break;
                }
                else
                {
                    /* Do nothing */
                }
                if (y == 0u)
                {
                    p_words[0] = t / 40u;
                    p_words[1] = t % 40u;
                    y = 2;
                }
                else
                {
                    p_words[y++] = t;
                }
                t = 0;
            }
            else
            {
                /* Do nothing */
            }
            x++;
            len--;
        }

        if (ER_OK == err)
        {
            *p_outlen = y;
        }
        else
        {
            /* Do nothing */
        }
    }

    return (err);
}
