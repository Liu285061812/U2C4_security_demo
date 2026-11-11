/******************************************************************************/
/* Component Name   ICU-M Interface Library (ICUMIF)                          */
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
#if !defined(R_ICUMIF_API_PK_IMPORT_H)
#define R_ICUMIF_API_PK_IMPORT_H

/**********************************************************************************/
/* Service parameters                                                             */
/**********************************************************************************/

typedef struct ISD_PK_IMPORT
{
    union
    {
        const uint8_t          *p_modulus;
        const uint8_t          *p_pub_point_x;
        uint64_t                padding_sys64_1;
    };
    union
    {
        const uint8_t          *p_pub_exp;
        const uint8_t          *p_pub_point_y;
        uint64_t                padding_sys64_2;
    };
    /* added for private key value import */
    union
    {
        const uint8_t          *p_priv_exp;
        const uint8_t          *p_priv_scalar;
        uint64_t                padding_sys64_3;
    };
    union
    {
        const uint8_t          *p_signature;
        uint64_t                padding_sys64_4;
    };
    union
    {
        uint8_t                *p_info;
        uint64_t                padding_sys64_5;
    };
    r_number_format_t       key_format;
    r_number_format_t       sig_format;
    r_key_group_t           key_group;          /* need for x.509 import */
    r_key_index_t           key_id;             /* need for x.509 import */
    r_key_group_t           sig_key_group;      /* need for x.509 import */
    r_key_index_t           sig_key_id;         /* need for x.509 import */
    r_signature_algo_t      sig_algo;           /* need for x.509 import */
    r_hash_primitive_t      hash_primitive;     /* need for x.509 import */
    r_ecc_curve_t           curve_id;
    union
    {
        uint16_t                point_x_size_in_bits;
        uint16_t                modulus_size_in_bits;
    };
    union
    {
        uint16_t                point_y_size_in_bits;
        uint16_t                pub_exp_size_in_bits;
    };
    /* added for private key value import */
    union
    {
        uint16_t                priv_scalar_size_in_bits;
        uint16_t                priv_exp_size_in_bits;
    };
    uint16_t                sig_size_in_bits;
    uint16_t                info_size_in_bytes;
    union
    {
        const uint8_t          *p_x509_certificate;
        uint64_t                padding_sys64_6;
    };
    uint16_t                x509_certificate_size_in_bytes;
    struct
    {
        r_key_property_flags_t disable_on_debug:1;
        r_key_property_flags_t write_protected:1;
    } flags;
    uint8_t                 skip_verify;
} ISD_PK_IMPORT_t;

#endif /* #if !defined(R_ICUMIF_API_PK_IMPORT_H) */
