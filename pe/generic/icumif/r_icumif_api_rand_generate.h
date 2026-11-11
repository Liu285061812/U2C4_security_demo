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
#if !defined(R_ICUMIF_API_RAND_GENERATE_H)
#define R_ICUMIF_API_RAND_GENERATE_H

/**********************************************************************************/
/* Service parameters                                                             */
/**********************************************************************************/

typedef enum
{
    RAND_NO_TEST,
    RAND_DIAG_P1_TEST,
    RAND_DIAG_P2_TEST,
    RAND_HEALTH_TEST,

    LAST_ITEM_RAND_TEST = 0x01000000
} r_rand_test_t;

typedef struct _ISD_RAND_INIT
{
    r_rand_test_t   online_test;
}
ISD_RAND_INIT_t;

typedef struct _ISD_RAND_EXTEND_SEED
{
    union
    {
        const uint32_t  *p_entropy;
        const uint64_t  padding_sys64;
    };
}
ISD_RAND_EXTEND_SEED_t;

typedef struct ISD_RAND_GENERATE
{
    union
    {
        uint32_t        *p_block_out;
        uint64_t        padding_sys64;
    };
    uint32_t            nb_blocks;
    r_job_slice_t       job_slice;
    uint8_t             padding[2];
}
ISD_RAND_GENERATE_t;

#endif /* #if !defined(R_ICUMIF_API_RAND_GENERATE_H) */
