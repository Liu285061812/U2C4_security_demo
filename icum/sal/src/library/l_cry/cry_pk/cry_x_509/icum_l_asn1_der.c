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
#include "../cry_common/icum_l_my_toolbox.h"

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_X509_0006]:[ICUMFW_UD_L_CRY_X509_0006]
* Function name: l_X509_ParseDerFormat
* Description  : Parse the ASN1 DER encoded input (in) with inlen bytes
*                Output the list of ASN1 DER objects found
*               - when calling: nb_items should be set with the maximum number in p_list
*               - when returning: nb_items is set with the number of objects found
* Arguments    : p_in        [in]       :DER encoded data
*                inlen       [in]       :bytes of 'in'
*                p_list      [out]      :list of ASN1 DER objects found
*                p_nb_items  [in/out]   :the number of objects found
* Return Value : ER_OK
*                ER_L_X509_INVALID_ARG
***************************************************************************************/
uint32_t l_X509_ParseDerFormat(const uint8_t    *p_in,
                                     uint32_t   inlen,
                                     asn1_t     *p_list,
                                     uint32_t   *p_nb_items)
{
    uint32_t    i, size, x, n;
    uint8_t     type;
    uint32_t    err;

    /* get block size */
    if (inlen < 2u)
    {
        err = ER_L_X509_INVALID_ARG;
    }
    else
    {
        x = 0u;

        /* parse the data, fill-in the types & sizes */
        i = 0u;
        while (x < (inlen - 1u))
        {
            type = p_in[x];
            x++;
            size = p_in[x];
            x++;
            if ((size & 0x80u) != 0u)
            {
                n = size & 0x7Fu;
                size = 0;
                while (n > 0u)
                {
                    n--;
                    size = (size * 256u) + p_in[x];
                    x++;
                }
            }
            if (i < *(p_nb_items))
            {
                p_list[i].type          = type & 0x1Fu;
                p_list[i].constructed   = (type & 0x20u) >> 5u;
                p_list[i].size          = size;
                p_list[i].p_data        = (uint8_t *)&p_in[x];
            }
            else
            {
                break;
            }
            i++;
            x += size;
        }

        /* return number of items parsed */
        *(p_nb_items) = i;
        err = ER_OK;
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_X509_0007]:[ICUMFW_UD_L_CRY_X509_0007]
* Function name: l_X509_DecodeFromDerFormat
* Description  : Decode the ASN1 DER object pointed by p_object
*                Output the result of the decoding in the buffer out
*                - when calling: outlen should be set with the buffer size
*                - when returning: outlen is set with the number of bytes written in buffer
* Arguments    : p_object   [in]       :ASN1 DER object
*                p_out      [out]      :the result of the decoding in the buffer out
*                p_outlen   [in/out]   :number of bytes written in buffer
* Return Value : ER_OK
*                ER_L_X509_INVALID_ARG
***************************************************************************************/
uint32_t l_X509_DecodeFromDerFormat  (const asn1_t      *p_object,
                                            uint8_t     *p_out,
                                            uint32_t    *p_outlen)
{
    uint8_t     unused_bits;
    uint32_t    err = ER_OK;

    if (p_outlen == NULL)
    {
        /* should never occur */
        err = ER_L_X509_INVALID_ARG;
    }
    else if (p_object == NULL)
    {
        /* should never occur */
        err = ER_L_X509_INVALID_ARG;
    }
    else
    {
        switch (p_object->type)
        {
            case ASN1_OBJECT_IDENTIFIER:
                /* decode OID */
                err = l_X509_DecodeOidFromDerFormat(p_object->p_data, p_object->size, (uint32_t *)p_out, p_outlen);
                break;
            case ASN1_BIT_STRING:
                /* retrieve the bit string */
                if ((*p_outlen) < p_object->size - 1u)
                {
                    err = ER_L_X509_BUFFER_OVERFLOW;
                }
                else
                {
                    unused_bits = *(p_object->p_data);
                    l_CMN_MemCpy(p_out, p_object->p_data + 1, p_object->size - 1u);
                    *(uint8_t *)(p_out + p_object->size - 2) &= (0xFFu << unused_bits);
                }
                *p_outlen = p_object->size - 1u;
                break;
            case ASN1_INTEGER:
            case ASN1_OCTET_STRING:
                /* retrieve the integer */
                /* see if it's negative */
                if ((*(p_object->p_data) == 0x00u) && (*(p_object->p_data + 1) & 0x80u))
                {
                    if ((*p_outlen) < p_object->size - 1u)
                    {
                        err = ER_L_X509_BUFFER_OVERFLOW;
                        *p_outlen = p_object->size;
                    }
                    else
                    {
                        l_CMN_MemCpy(p_out, (p_object->p_data) + 1, p_object->size - 1u);
                        *p_outlen = p_object->size - 1u;
                    }
                }
                else
                {
                    if ((*p_outlen) < p_object->size)
                    {
                        err = ER_L_X509_BUFFER_OVERFLOW;
                        *p_outlen = p_object->size;
                    }
                    else
                    {
                        l_CMN_MemCpy(p_out, p_object->p_data, p_object->size);
                        *p_outlen = p_object->size;
                        err = ER_OK;
                    }
                }
                break;
            case ASN1_BOOLEAN:
                *(uint8_t *)p_out = (*p_object->p_data == 0xFFu) ? 1u : 0u;
                *p_outlen = 1u;
                break;
            case ASN1_NULL:             /* not support type, same as default case*/
            case ASN1_PRINTABLE_STRING: /* not support type, same as default case*/
            case ASN1_T61_STRING:       /* not support type, same as default case*/
            case ASN1_IA5_STRING:       /* not support type, same as default case*/
            case ASN1_UTCTIME:          /* not support type, same as default case*/
            case ASN1_SEQUENCE:         /* not support type, same as default case*/
            case ASN1_SET:              /* not support type, same as default case*/
            default:
                err = ER_L_X509_INVALID_ARG;
                break;
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_X509_0008]:[ICUMFW_UD_L_CRY_X509_0008]
* Function name: l_X509_EncodeToDerFormat
* Description  : Encode a data string to an ASN1 DER encoded object
*                Output the result of the encoding in the buffer out
*                   - when calling: outlen should be set with the buffer size
*                   - when returning: outlen is set with the number of bytes written in buffer
* Arguments    : type     [in]       :ASN1 DER object type
*                p_in     [in]       :a data string
*                inlen    [in]       :number of bytes written in buffer
*                p_out    [out]      :the result of the encoding in the buffer out
*                p_outlen [in/out]   :number of bytes written in buffer
* Return Value : ER_OK
*                ER_L_X509_INVALID_ARG
***************************************************************************************/
uint32_t l_X509_EncodeToDerFormat (       uint8_t   type,
                                    const void      *p_in,
                                          uint32_t  inlen,
                                          uint8_t   *p_out,
                                          uint32_t  *p_outlen)
{
    uint32_t err;
    
    switch (type)
    {
        case ASN1_OBJECT_IDENTIFIER:
            /* encode OID */
            /* in = pointer to OID words */
            /* inlen = number of OID words to encode */
            err = l_X509_EncodeOidToDerFormat((const uint32_t *)p_in, inlen, p_out, p_outlen);
            break;
        case ASN1_BOOLEAN:          /* not support type, same as default case*/
        case ASN1_INTEGER:          /* not support type, same as default case*/
        case ASN1_BIT_STRING:       /* not support type, same as default case*/
        case ASN1_OCTET_STRING:     /* not support type, same as default case*/
        case ASN1_NULL:             /* not support type, same as default case*/
        case ASN1_PRINTABLE_STRING: /* not support type, same as default case*/
        case ASN1_T61_STRING:       /* not support type, same as default case*/
        case ASN1_IA5_STRING:       /* not support type, same as default case*/
        case ASN1_UTCTIME:          /* not support type, same as default case*/
        case ASN1_SEQUENCE:         /* not support type, same as default case*/
        case ASN1_SET:              /* not support type, same as default case*/
        default:
            err = ER_L_X509_INVALID_ARG;
            break;
    }
    
    return (err);
}
