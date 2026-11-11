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
#if !defined(R_ICUMIF_API_HMAC_H)
#define R_ICUMIF_API_HMAC_H

/**********************************************************************************/
/* Type definition                                                                */
/**********************************************************************************/

typedef enum
{
    HMAC_GENERATION,
    HMAC_VERIFICATION,

    LAST_ITEM_HMAC = 0x01000000
} hmac_operation_t;

/**********************************************************************************/
/* Service parameters                                                             */
/**********************************************************************************/

typedef struct ISD_HMAC
{
    r_key_index_t               key_id;
    hmac_operation_t            hmac_operation;
    r_hash_primitive_t          hash_primitive;
    union
    {
        const uint8_t           *p_msg;
        uint64_t                padding_sys64_1;
    };
    uint32_t                    msg_size_in_bytes;
    union
    {
        uint8_t                 *p_hmac;
        uint64_t                padding_sys64_2;
    };
    uint8_t                     hmac_size_in_bytes;
    r_job_slice_t               job_slice;
    volatile r_verif_result_t   verification_result;
} ISD_HMAC_t;

#endif /* #if !defined(R_ICUMIF_API_HMAC_H) */
