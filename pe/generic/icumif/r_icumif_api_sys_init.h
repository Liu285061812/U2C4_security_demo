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
#if !defined(R_ICUMIF_API_SYS_INIT_H)
#define R_ICUMIF_API_SYS_INIT_H

/**********************************************************************************/
/* Type definition                                                                */
/**********************************************************************************/

typedef enum
{
    SYS_INIT_FW_INITIALIZATION,
    SYS_INIT_FW_ACTIVATION,

    LAST_ITEM_SYS_INIT = 0x01000000
} sys_init_t;

/**********************************************************************************/
/* Service parameters                                                             */
/**********************************************************************************/

typedef struct ISD_SYSTEM_INIT
{
    sys_init_t          sys_init_id;
    union
    {
        const uint8_t  *p_cid;
        uint64_t        padding_sys64_1;
    };
    uint16_t            cid_size_in_bytes;
    union
    {
        const uint8_t  *p_data;
        uint64_t        padding_sys64_2;
    };
    uint16_t            data_size_in_bytes;
    r_number_format_t   data_format;
} ISD_SYSTEM_INIT_t;

#endif /* #if !defined(R_ICUMIF_API_SYS_INIT_H) */
