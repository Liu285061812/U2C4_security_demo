/******************************************************************************/
/* Component Name   ICU-M Crypto AES Driver                                   */
/******************************************************************************/
/* Product      : ICU-M Low Level Driver                                      */
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

#if !defined(ICUM_D_CRY_AES_PUB_H)
#define ICUM_D_CRY_AES_PUB_H

/**************************************************************************************/
/* Headers                                                                            */
/**************************************************************************************/
#include "icum_common_pub.h"

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* AES result */
#define ER_D_AES_TIMEOUT_ERROR      (ER_D_CRY_AES_BASE + (0x00000001u))
#define ER_D_AES_SEQUENCE_ERROR     (ER_D_CRY_AES_BASE + (0x00000002u))
#define ER_D_AES_SUSPEND_ERROR      (ER_D_CRY_AES_BASE + (0x00000003u))
#define ER_D_AES_ALIGN_ERROR        (ER_D_CRY_AES_BASE + (0x00000004u))
#define ER_D_AES_DTF_ERROR          (ER_D_CRY_AES_BASE + (0x00000005u))
#define ER_D_AES_CMP_ERROR          (ER_D_CRY_AES_BASE + (0x00000006u))
#define ER_D_AES_STATUS_ERROR       (ER_D_CRY_AES_BASE + (0x00000007u))
#define ER_D_AES_INIT_ERROR         (ER_D_CRY_AES_BASE + (0x00000010uL))

/* AES cipher modes */
#define D_AES_CIPHER_MODE_ECB       (0u)
#define D_AES_CIPHER_MODE_CBC       (1u)
#define D_AES_CIPHER_MODE_CTR       (2u)
#define D_AES_CIPHER_MODE_CFB       (3u)
#define D_AES_CIPHER_MODE_OFB       (4u)
#define D_AES_CIPHER_MODE_GCM       (5u)
#define D_AES_CIPHER_MODE_XTS       (6u)
#define D_AES_CIPHER_MODE_CCM       (7u)
#define D_AES_CIPHER_MODE_CMAC      (8u)
#define D_AES_CIPHER_MODE_MP        (9u)

/* AES key types */
#define D_AES_KEYTYPE_PLAIN         (0u)
#define D_AES_KEYTYPE_ROMKEY_0      (1u)
#define D_AES_KEYTYPE_ROMKEY_1      (2u)

/* AES key size */
#define D_AES_KEY_SIZE_IN_BYTES_128BIT    (16u)
#define D_AES_KEY_SIZE_IN_BYTES_192BIT    (24u)
#define D_AES_KEY_SIZE_IN_BYTES_256BIT    (32u)
#define D_AES_KEY_SIZE_IN_BYTES_COMMON    D_AES_KEY_SIZE_IN_BYTES_256BIT    /* common use */
#define D_AES_KEY_SIZE_COMMON             (8u)    /* common use */

/* AES directions */
#define D_AES_DIR_ENCRYPTION        (0u)
#define D_AES_DIR_DECRYPTION        (1u)

/* AES first block */
#define D_AES_NOT_FIRST_CIPHER      (0u)
#define D_AES_FIRST_CIPHER          (1u)

/* AES minimum number of blocks */
#define D_AES_NUMBER_MIN           (0x00000001u)

/* block info */
#define D_AES_BLOCK_INFO_SIZE       (0x00FFu)
#define D_AES_BLOCK_INFO_SIZE_MIN   (0x0000u)
#define D_AES_BLOCK_INFO_SIZE_MAX   (0x0080u)
#define D_AES_BLOCK_INFO_FIRST      (0x0100u)
#define D_AES_BLOCK_INFO_LAST       (0x0200u)
#define D_AES_BLOCK_INFO_GCM_IV96   (0x0400u)
#define D_AES_BLOCK_INFO_GCM_ADD0   (0x0800u)
#define D_AES_BLOCK_INFO_GCM_TXT0   (0x1000u)
#define D_AES_BLOCK_INFO_CCM_PLEN0  (0x1000u)

/* GCM operating procedure */
#define D_AES_GCM_STEP_IV           (0u)
#define D_AES_GCM_STEP_AAD          (1u)
#define D_AES_GCM_STEP_CIPHER       (2u)
#define D_AES_GCM_STEP_AAD_FIRST    (uint8_t)(3u)

/* CCM nonce length */
#define D_AES_NLEN_BIT_56           (0u)
#define D_AES_NLEN_BIT_64           (1u)
#define D_AES_NLEN_BIT_72           (2u)
#define D_AES_NLEN_BIT_80           (3u)
#define D_AES_NLEN_BIT_88           (4u)
#define D_AES_NLEN_BIT_96           (5u)
#define D_AES_NLEN_BIT_104          (6u)

/* CCM tag length */
#define D_AES_TLEN_BIT_32           (0u)
#define D_AES_TLEN_BIT_48           (1u)
#define D_AES_TLEN_BIT_64           (2u)
#define D_AES_TLEN_BIT_80           (3u)
#define D_AES_TLEN_BIT_96           (4u)
#define D_AES_TLEN_BIT_112          (5u)
#define D_AES_TLEN_BIT_128          (6u)

/* CCM operating procedure */
#define D_AES_CCM_STEP_C            (0u)
#define D_AES_CCM_STEP_A            (1u)
#define D_AES_CCM_STEP_P            (2u)

/* CCM operation */
#define D_AES_OPE_CONTINUE          (0u)
#define D_AES_OPE_CANCEL            (1u)

/* sync/async transfer mode */
#define D_AES_TRANS_SYNC            (0u)
#define D_AES_TRANS_ASYNC           (1u)

/* AES operation */
#define D_AES_OPE_RELEASE           (2u)

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
typedef void (init_callback_t)(uint32_t result);

/* AES set parameters */
typedef struct
{
    uint8_t                cipher_mode;
    uint8_t                direction;
    uint8_t                key_type;
    uint8_t                first;
    uint32_t*              p_aes_key;
    uint32_t*              p_aes_iv;
    uint16_t               key_length;
}aes_setparam_t;

/* AES ProcessNBlocks parameters */
typedef struct
{
    uint32_t               number;
    uint16_t               block_info;
    uint32_t*              p_aes_src;
    uint32_t*              p_aes_dest;
}aes_nblocks_t;

/* AES GCM set parameters */
typedef struct
{
    uint8_t                direction;
    uint8_t                first;
    uint32_t*              p_gcm_key;
    uint32_t*              p_gcm_ghash;
    uint16_t               key_length;
}aes_gcm_setparam_t;

/* AES GCM IV parameters */
typedef struct
{
    uint32_t               iv_number;
    uint16_t               iv_block_info;
    uint32_t*              p_iv_size_in_bits;
    uint32_t*              p_gcm_iv;
    uint32_t*              p_ctr_j0;
}aes_gcm_iv_t;

/* AES GCM AAD parameters */
typedef struct
{
    uint32_t               aad_number;
    uint16_t               aad_block_info;
    uint32_t*              p_gcm_aad;
}aes_gcm_aad_t;

/* AES GCM parameters */
typedef struct
{
    uint32_t               gcm_number;
    uint16_t               gcm_block_info;
    uint32_t*              p_ctr_j;
    uint32_t*              p_gcm_src;
    uint32_t*              p_gcm_dest;
}aes_gcm_t;

/* AES GCM tag parameters */
typedef struct
{
    uint32_t*              p_ctr_j0;
    uint32_t*              p_total_size_in_bits;
    uint32_t*              p_auth_tag;
}aes_gcm_tag_t;

/* AES CCM set parameters */
typedef struct
{
    uint8_t                direction;
    uint32_t*              p_ccm_key;
    uint32_t*              p_ccm_b0;
    uint32_t*              p_ccm_nonce;
    uint16_t               key_length;
}aes_ccm_setparam_t;

/* AES CCM associated data parameters */
typedef struct
{
    uint8_t                direction;
    uint16_t               a_block_info;
    uint32_t               a_number;
    uint32_t*              p_associated;
}aes_ccm_a_t;

/* AES CCM parameters */
typedef struct
{
    uint8_t                direction;
    uint16_t               ccm_block_info;
    uint32_t               ccm_number;
    uint32_t*              p_ccm_src;
    uint32_t*              p_ccm_dest;
}aes_ccm_t;

/* AES CCM payload data parameters */
typedef struct
{
    uint16_t               payload_block_info;
    uint32_t               payload_number;
    uint32_t*              p_payload;
}aes_ccm_p_t;

/* AES CCM decryption tag parameters */
typedef struct
{
    uint32_t*              p_tag_part;
    uint32_t*              p_auth_yr;
    uint32_t*              p_auth_tag;
}aes_ccm_tag_dec_t;

/* AES init param */
typedef struct
{
    uint8_t                cipher_mode;
    uint8_t                direction;
    uint8_t                key_type;
    uint32_t*              p_aes_key;
    uint32_t*              p_aes_iv;
    uint32_t*              p_context;
    uint16_t               key_length;
}aes_init_t;

/* AES update param */
typedef struct
{
    uint32_t               number;
    uint16_t               first;
    uint32_t*              p_aes_src;
    uint32_t*              p_context;
    uint32_t*              p_aes_dest;
}aes_update_t;

/* AES finish param */
typedef struct
{
    uint32_t               number;
    uint16_t               last_block_in_bits;
    uint16_t               first;
    uint32_t*              p_aes_src;
    uint32_t*              p_context;
    uint32_t*              p_aes_dest;
}aes_finish_t;

/* AES release param */
typedef struct
{
    uint16_t               key_length;
    uint32_t               key_length_id;
}aes_release_t;

/* AES all at once param */
typedef struct
{
    uint8_t                cipher_mode;
    uint8_t                direction;
    uint32_t               number;
    uint8_t                key_type;
    uint16_t               last_block_in_bits;
    uint32_t*              p_aes_key;
    uint32_t*              p_aes_iv;
    uint32_t*              p_aes_src;
    uint32_t*              p_aes_dest;
    uint16_t               key_length;
    uint8_t                async;
}aes_all_at_once_t;

/* XTS-AES init param */
typedef struct
{
    uint8_t                direction;
    uint32_t*              p_aes_key;
    uint32_t*              p_aes_key_tweak;
    uint32_t*              p_aes_iv;
    uint32_t*              p_context;
    uint16_t               key_length;
}aes_init_xts_t;

/* XTS-AES all at once param */
typedef struct
{
    uint8_t                direction;
    uint32_t               number;
    uint16_t               last_block_in_bits;
    uint32_t*              p_aes_key;
    uint32_t*              p_aes_key_tweak;
    uint16_t               key_length;
    uint32_t*              p_aes_iv;
    uint32_t*              p_aes_src;
    uint8_t                async;
    uint32_t*              p_aes_dest;
    uint32_t*              p_context;
}aes_all_at_once_xts_t;

/* AES GCM init param */
typedef struct
{
    uint32_t               number;
    uint16_t               block_info;
    uint8_t                direction;
    uint32_t*              p_aes_key;
    uint32_t*              p_aes_iv;
    uint32_t*              p_iv_size_in_bits;
    uint32_t*              p_context;
    uint32_t*              p_ctr_j0;
    uint16_t               key_length;
}aes_init_gcm_t;

/* AES GCM update param */
typedef struct
{
    uint32_t               number;
    uint16_t               block_info;
    uint8_t                step;
    uint32_t*              p_iv_size_in_bits;
    uint32_t*              p_aes_src;
    uint32_t*              p_context;
    uint32_t*              p_aes_dest;
    uint32_t*              p_ctr_j0;
}aes_update_gcm_t;

/* AES GCM finish param */
typedef struct
{
    uint32_t               number;
    uint16_t               block_info;
    uint32_t*              p_aes_src;
    uint32_t*              p_total_size_in_bits;
    uint32_t*              p_ctr_j0;
    uint32_t*              p_context;
    uint32_t*              p_aes_dest;
    uint32_t*              p_auth_tag;
}aes_finish_gcm_t;

/* AES GCM AES all at once param */
typedef struct
{
    uint8_t                direction;
    uint32_t*              p_aes_key;
    uint16_t               key_length;
    uint32_t*              p_aes_iv;
    uint32_t               iv_size_in_bits;
    uint32_t*              p_auth_data;
    uint32_t               auth_data_size_in_bits;
    uint32_t*              p_aes_src;
    uint32_t               data_size_in_bits;
    uint32_t               auth_tag_size_in_bits;
    uint8_t                async;
    uint32_t*              p_aes_dest;
    uint32_t*              p_auth_tag;
    uint32_t*              p_context;
}aes_all_at_once_gcm_t;

/* AES CCM init param */
typedef struct
{
    uint8_t                direction;
    uint32_t*              p_size_data;
    uint32_t*              p_aes_key;
    uint32_t*              p_aes_iv;
    uint32_t*              p_context;
    uint16_t               key_length;
}aes_init_ccm_t;

/* AES CCM update param */
typedef struct
{
    uint8_t                step;
    uint16_t               block_info;
    uint32_t               number;
    uint32_t*              p_aes_src;
    uint32_t*              p_context;
    uint32_t*              p_aes_dest;
}aes_update_ccm_t;

/* AES CCM finish param */
typedef struct
{
    uint8_t                operation;
    uint32_t*              p_aes_src;
    uint32_t*              p_context;
    uint32_t*              p_auth_yr;
    uint32_t*              p_auth_tag;
}aes_finish_ccm_t;

/* AES CCM AES all at once param */
typedef struct
{
    uint8_t                direction;
    uint32_t*              p_aes_key;
    uint16_t               key_length;
    uint32_t*              p_aes_iv;
    uint32_t               iv_size_in_bits;
    uint32_t*              p_auth_data;
    uint32_t               auth_data_size_in_bits;
    uint32_t*              p_aes_src;
    uint32_t               data_size_in_bits;
    uint32_t               auth_tag_size_in_bits;
    uint8_t                async;
    uint32_t*              p_aes_dest;
    uint32_t*              p_auth_yr;
    uint32_t*              p_auth_tag;
    uint32_t*              p_context;
}aes_all_at_once_ccm_t;

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
uint32_t D_AES_Init(init_callback_t* p_callback);

uint32_t D_AES_ReleaseKeyRange(aes_release_t* p_param);

uint32_t D_AES_InitJob(aes_init_t* p_param);
uint32_t D_AES_UpdateJob(aes_update_t* p_param);
uint32_t D_AES_FinishJob(aes_finish_t* p_param);
uint32_t D_AES_ExecutionAtOnce(aes_all_at_once_t* p_param);

uint32_t D_AES_InitJobXts(aes_init_xts_t* p_param);
uint32_t D_AES_UpdateJobXts(aes_update_t* p_param);
uint32_t D_AES_FinishJobXts(aes_finish_t* p_param);
uint32_t D_AES_ExecutionAtOnceXts(aes_all_at_once_xts_t* p_param);

uint32_t D_AES_InitJobGcm(aes_init_gcm_t* p_param);
uint32_t D_AES_UpdateJobGcm(aes_update_gcm_t* p_param);
uint32_t D_AES_FinishJobGcm(aes_finish_gcm_t* p_param);
uint32_t D_AES_ExecutionAtOnceGcm(aes_all_at_once_gcm_t* p_param);

uint32_t D_AES_InitJobCcm(aes_init_ccm_t* p_param);
uint32_t D_AES_UpdateJobCcm(aes_update_ccm_t* p_param);
uint32_t D_AES_FinishJobCcm(aes_finish_ccm_t* p_param);
uint32_t D_AES_ExecutionAtOnceCcm(aes_all_at_once_ccm_t* p_param);


#endif /* ICUM_D_CRY_AES_PUB_H */
