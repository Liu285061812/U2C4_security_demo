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

#ifndef ICUM_L_X509_H
#define ICUM_L_X509_H

#include "renesas_types.h"

/* DER handling */
/* Tag number */
#define ASN1_BOOLEAN            (0x01u)
#define ASN1_INTEGER            (0x02u)
#define ASN1_BIT_STRING         (0x03u)
#define ASN1_OCTET_STRING       (0x04u)
#define ASN1_NULL               (0x05u)
#define ASN1_OBJECT_IDENTIFIER  (0x06u)
#define ASN1_UTF8_STRING        (0x0cu)
#define ASN1_SEQUENCE           (0x10u)
#define ASN1_SET                (0x11u)
#define ASN1_PRINTABLE_STRING   (0x13u)
#define ASN1_T61_STRING         (0x14u)
#define ASN1_IA5_STRING         (0x16u)
#define ASN1_UTCTIME            (0x17u)
/* P/C (Primitive/Constructed) */
#define ASN1_CONSTRUTURED_TAG   (0x20u)

typedef struct asn1
{
    uint32_t     size;           /* size of the input or resulting output */
    uint8_t     *p_data;         /* data to encode or place for decoding */
    uint8_t      type;           /* ASN.1 enumerated type identifier */
    uint8_t      constructed;    /* 1 if this is a constructed encoding, 0 otherwise */
} asn1_t;


/* OBJECT IDENTIFIER */
uint32_t l_X509_EncodeOidToDerFormat (const uint32_t *p_words,
                                            uint32_t  nwords,
                                            uint8_t  *p_out,
                                            uint32_t *p_outlen);

uint32_t l_X509_DecodeOidFromDerFormat (const uint8_t  *p_in,
                                              uint32_t  inlen,
                                              uint32_t *p_words,
                                              uint32_t *p_outlen);

uint32_t l_X509_GetLengthOfOidDerFormat (const uint32_t *p_words,
                                               uint32_t nwords,
                                               uint32_t *p_outlen);

uint32_t l_X509_ParseDerFormat (const uint8_t    *p_in,
                                      uint32_t   inlen,
                                      asn1_t     *p_list,
                                      uint32_t   *p_nb_items);

uint32_t l_X509_DecodeFromDerFormat (const asn1_t   *p_object,
                                           uint8_t  *p_out,
                                           uint32_t *p_outlen);

uint32_t l_X509_EncodeToDerFormat (       uint8_t   type,
                                    const void      *p_in,
                                          uint32_t  inlen,
                                          uint8_t   *p_out,
                                          uint32_t  *p_outlen);

#endif /* ICUM_L_X509_H */

