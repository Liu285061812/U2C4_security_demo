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
#if !defined(R_ICUMIF_API_RSA_ENCRYPT_H)
#define R_ICUMIF_API_RSA_ENCRYPT_H

/**********************************************************************************/
/* Service parameters                                                             */
/**********************************************************************************/

typedef struct ISD_RSA_ENCRYPT
{
    union
    {
        const uint8_t*          p_message;                      /* in */
        uint64_t                padding_sys64_1;
    };
    uint32_t                    message_size_in_bytes;          /* in */
    union
    {
        uint8_t*                p_encrypted_message;            /* out */
        uint64_t                padding_sys64_2;
    };
    uint16_t                    encrypted_message_size_in_bits; /* out */
    union
    {
        const uint8_t*          p_label;                        /* in (option for OAEP) */
        uint64_t                padding_sys64_3;
    };
    uint32_t                    label_size_in_bytes;            /* in (option for OAEP) */
    r_hash_primitive_t          hash_primitive;                 /* in */
    r_message_encoding_format_t message_encoding_format;        /* in */
    r_key_group_t               key_group;                      /* in */
    r_key_index_t               key_id;                         /* in */
    r_number_format_t           encrypted_message_format;       /* in */
    r_job_slice_t               job_slice_mod_exp;              /* in */
} ISD_RSA_ENCRYPT_t;

#endif /* #if !defined(R_ICUMIF_API_RSA_ENCRYPT_H) */
