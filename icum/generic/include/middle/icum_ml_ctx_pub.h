/******************************************************************************/
/* Component Name   ML Context Manager public header file                     */
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

#if !defined(ICUM_ML_CTX_PUB_H)
#define ICUM_ML_CTX_PUB_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Context MNG result */
#define ER_ML_CTX_ALL_USE                   (ER_ML_CTX_BASE + (0x00000001uL))
#define ER_ML_CTX_NOT_FOUND                 (ER_ML_CTX_BASE + (0x00000002uL))

/* Context type */
#define CTX_TYPE_AES                        (0u)
#define CTX_TYPE_RSA                        (1u)
#define CTX_TYPE_ECC                        (2u)
#define CTX_TYPE_HASH                       (3u)
#define CTX_TYPE_HMAC                       (4u)

/* Context use */
#define CTX_FREE                            (0uL)
#define CTX_IN_USE                          (1uL)

#define BLOCK_SIZE_IN_LONG                  (4u)     /* 1 block is 4 long size  */
#define AES_CTX_SIZE                        (17uL)   /* Secure 68 byte area with 32 bit type   */

/* AES context param */
typedef struct
{
    uint32_t result;
    uint16_t block_info;                         /* crypto state */
    uint8_t  auth_step;                          /* auth Cipher Crypto Step */
    uint32_t gcm_j0[BLOCK_SIZE_IN_LONG];         /* Hold initial value data for Tag generation(for GCM)*/
    uint32_t gcm_total_size[BLOCK_SIZE_IN_LONG]; /* Hold the sum of sizes for Tag generation(for GCM)*/
    uint32_t context[AES_CTX_SIZE];
} ml_ctx_aes_context_t;

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
uint32_t ML_CTX_Init(void);
uint32_t ML_CTX_Get(uint16_t type, uint32_t **p_context);
uint32_t ML_CTX_Free(uint16_t type, uint32_t *p_context);
uint32_t ML_CTX_InUseCheckAes(uint8_t *p_judge_use);
uint32_t ML_CTX_InUseCheckNotAes(uint8_t *p_judge_use);

#endif /* #if !defined(ICUM_ML_CTX_PUB_H) */
