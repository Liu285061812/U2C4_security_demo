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
#if !defined(R_ICUMIF_API_AES_CIPHER_H)
#define R_ICUMIF_API_AES_CIPHER_H

/**********************************************************************************/
/* Defines                                                                        */
/**********************************************************************************/

#define AES_BLOCK_SIZE_IN_BYTES  16

/**********************************************************************************/
/* Type definition                                                                */
/**********************************************************************************/

typedef enum
{
    CIPHER_DIR_ENCRYPTION,
    CIPHER_DIR_DECRYPTION,

    LAST_ITEM_CIPHER_DIR = 0x01000000
} cipher_direction_t;

typedef enum
{
    CIPHER_MOD_CBC,
    CIPHER_MOD_ECB,
    CIPHER_MOD_CFB,
    CIPHER_MOD_OFB,
    CIPHER_MOD_CTR,
    CIPHER_MOD_XTS,

    LAST_ITEM_CIPHER_MOD = 0x01000000
} cipher_modes_t;

/**********************************************************************************/
/* Service parameters                                                             */
/**********************************************************************************/

typedef struct ISD_AES_CIPHER
{
    union
    {
        const uint32_t  *p_iv;
        uint64_t        padding_sys64_1;
    };
    union
    {
        const uint32_t  *p_block_in;
        uint64_t        padding_sys64_2;
    };
    union
    {
        uint32_t        *p_block_out;
        uint64_t        padding_sys64_3;
    };
    union
    {
        uint32_t        nb_blocks;
        uint32_t        data_size_in_bits;
    } size;
    r_key_group_t       key_group;
    r_key_index_t       key_id;
    r_key_group_t       key_group_2;
    r_key_index_t       key_id_2;
    cipher_direction_t  direction;
    cipher_modes_t      cipher_mode;
    r_job_slice_t       job_slice;
    r_job_cycle_t       job_cycle;
} ISD_AES_CIPHER_t;

#endif /* #if !defined(R_ICUMIF_API_AES_CIPHER_H) */
