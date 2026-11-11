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
#if !defined(R_ICUMIF_API_GET_INFO_H)
#define R_ICUMIF_API_GET_INFO_H

/**********************************************************************************/
/* Type definition                                                                */
/**********************************************************************************/

typedef enum
{
    INFO_GET_FW_NAME,
    INFO_GET_FW_VERSION,
    INFO_GET_FW_BUILD_TYPE,
    INFO_GET_JOB_COUNT,
    INFO_GET_KEY_MAPPING,
    INFO_GET_MYDATA_SET,
    INFO_GET_SECURE_TICK,
    INFO_GET_SECURE_TICK_FREQ,
    INFO_GET_TIMING_1ST_STAGE_BOOT,
    INFO_GET_CMAC_MIN,
    INFO_GET_BOOT_STAGE_SLICE,
    INFO_GET_BOOT_STAGE_SANCTION,
    INFO_GET_SECURE_BOOT,
    INFO_GET_WDT,
    INFO_GET_RND,
    INFO_GET_DEBUG,
    INFO_GET_SECURE_TICK_INTERVAL,
    INFO_GET_CUSTOMER_CODE,
    INFO_GET_START_PE,
    INFO_GET_DEVICE_ID,
    INFO_GET_RAM_EXECUTE,
    INFO_GET_ECC_TEST_LOCK,
    INFO_GET_NVM_DATA_WARNING,
    INFO_GET_FLS_CFG,
    INFO_GET_FREE_BLOCK,
    INFO_GET_VM_CFG,

    LAST_ITEM_INFO_FW = 0x01000000
} info_type_t;

typedef enum
{
    DATASET_SYS_CFG,
    DATASET_AES_KEY,
    DATASET_CR_KEY,
    DATASET_RSA_PRIV_KEY,
    DATASET_RSA_PUB_KEY,
    DATASET_ECC_PRIV_KEY,
    DATASET_ECC_PUB_KEY,
    DATASET_MYDATA,
    DATASET_RSA_PUB_EXT_KEY,
    DATASET_ECC_PUB_EXT_KEY,
    
    DATASET_FW_INTERNAL = 0xFE,
    DATASET_NONE = 0xFF,

    LAST_ITEM_DATASET = 0x01000000
} info_dataset_type_t;

typedef struct
{
    info_dataset_type_t  dataset;
    uint16_t             index;
} info_warning_data_t;

typedef struct
{
    info_warning_data_t data[16];
} info_warning_t;

/* 16 is the maximum number of secure data flash physical blocks 
   assumed by the target microcomputer */
typedef struct
{
    uint16_t total_num;
    uint16_t num[16];
} info_free_block_t;

typedef struct
{
    uint64_t    counter;
} secure_tick_info_t;

/**********************************************************************************/
/* Service parameters                                                             */
/**********************************************************************************/

typedef struct ISD_GET_INFO
{
    info_type_t         info_id;
    union
    {
        void           *p_info;
        uint64_t        padding_sys64;
    };
    volatile uint8_t    data_size_in_bytes;
} ISD_GET_INFO_t;

#endif /* #if !defined(R_ICUMIF_API_GET_INFO_H) */
