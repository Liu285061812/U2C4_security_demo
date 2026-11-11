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
#if !defined(R_ICUMIF_API_PK_EXPORT_H)
#define R_ICUMIF_API_PK_EXPORT_H

/**********************************************************************************/
/* Service parameters                                                             */
/**********************************************************************************/

typedef struct ISD_PK_EXPORT
{
    union
    {
        uint8_t                *p_modulus;
        uint8_t                *p_pub_point_x;
        uint64_t                padding_sys64_1;
    };
    union
    {
        uint8_t                *p_pub_exp;
        uint8_t                *p_pub_point_y;
        uint64_t                padding_sys64_2;
    };
    union
    {
        uint8_t                *p_signature;
        uint64_t                padding_sys64_3;
    };
    r_number_format_t       key_format;
    r_number_format_t       sig_format;
    r_key_group_t           key_group;
    r_key_index_t           key_id;
    r_key_group_t           sig_key_group;
    r_key_index_t           sig_key_id;
    r_signature_algo_t      sig_algo;
    r_hash_primitive_t      hash_primitive;
    volatile r_ecc_curve_t  curve_id;
    union
    {
        volatile uint16_t       point_x_size_in_bits;
        volatile uint16_t       modulus_size_in_bits;
    };
    union
    {
        volatile uint16_t       point_y_size_in_bits;
        volatile uint16_t       pub_exp_size_in_bits;
    };
    volatile uint16_t       sig_size_in_bits;
    uint8_t                 skip_sign;
} ISD_PK_EXPORT_t;

#endif /* #if !defined(R_ICUMIF_API_PK_EXPORT_H) */
