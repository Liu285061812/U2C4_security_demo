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
#if !defined(R_ICUMIF_API_AES_KET_UPDATE_PLAIN_H)
#define R_ICUMIF_API_AES_KET_UPDATE_PLAIN_H

/**********************************************************************************/
/* Service parameters                                                             */
/**********************************************************************************/

typedef struct ISD_KEY_UPDATE_PLAIN
{
    union
    {
        const uint8_t      *p_key_value;
        uint64_t            padding_sys64;
    };
    r_key_group_t           key_group;
    r_key_index_t           key_id;
    uint16_t                key_size_in_bytes;
    struct
    {
        r_key_property_flags_t encryption:1;
        r_key_property_flags_t decryption:1;
        r_key_property_flags_t cmac_generation:1;
        r_key_property_flags_t cmac_verification:1;
        r_key_property_flags_t write_protected:1;
        r_key_property_flags_t disable_on_debug:1;
        r_key_property_flags_t no_wild_card:1;
    } flags;
} ISD_KEY_UPDATE_PLAIN_t;

#endif /* #if !defined(R_ICUMIF_API_AES_KET_UPDATE_PLAIN_H) */
