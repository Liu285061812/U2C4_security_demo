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
#if !defined(R_ICUMIF_API_ECDSA_SIGN_H)
#define R_ICUMIF_API_ECDSA_SIGN_H

/**********************************************************************************/
/* Definition                                                                     */
/**********************************************************************************/
#define ECDSA_SIGNATURE_NUM     (2u)

/**********************************************************************************/
/* Service parameters                                                             */
/**********************************************************************************/

typedef struct ISD_ECDSA_SIGN
{
    union
    {
        const uint8_t          *p_source;
        uint64_t                padding_sys64;
    };
    union
    {
        uint8_t                *p_data;
        uint64_t                padding_sys64;
    } signature[ECDSA_SIGNATURE_NUM];
    uint32_t                    source_size_in_bytes;
    r_hash_primitive_t          hash_primitive;
    r_number_format_t           signature_format;
    r_key_group_t               key_group;
    r_key_index_t               key_id;
    r_job_slice_t               job_slice_hash;
    r_job_slice_t               job_slice_point_mul;
    volatile uint16_t           signature_size_in_bits[ECDSA_SIGNATURE_NUM];
    uint8_t                     is_precalculated_hash;
} ISD_ECDSA_SIGN_t;

#endif /* R_ICUMIF_API_ECDSA_SIGN_H */
