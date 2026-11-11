/******************************************************************************/
/* Component Name   ICU-M Service                                             */
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

/**********************************************************************************
* Header files
***********************************************************************************/
#include "renesas_types.h"
#include "r_icumif_api.h"
#include "icum_d_comm_pub.h"
#include "icum_k_issd_pub.h"

/**********************************************************************************
* Define
***********************************************************************************/
/* Internal service id */
#define SERVICE_20_CR       ((r_icumif_service_id_t)((uint32_t)SERVICE_02_RAND_RND + 1))

#define NUM0                ((uint32_t)0uL)          /* value : 0 */
#define NUM1                ((uint32_t)1uL)          /* value : 1 */
#define DATA_ZERO           ((uint8_t)0u)            /* value : 0 used for initialization */

#define AES_GCM_IV_SIZE_96  (96uL)                   /* IV size 96 in GCM mode */
#define AES_BIT_SIFT4       (4uL)                    /* 4bit sift constant*/
#define AES_BIT_SIFT8       (8uL)                    /* 8bit sift constant*/
#define COUNTER_MAX         ((uint32_t)0xFFFFFFFFu)  /* unsigned long max number */

#define AES_STATE_INIT      ((uint32_t)0xFFFFFFFFu)  /* initial state */

/**********************************************************************************
* Structure
***********************************************************************************/
typedef struct id_ISD_CR_t
{
    uint32_t trigger_type;
    uint32_t pe_type;
} ISD_CR_t;

/**********************************************************************************/
/* Prototypes                                                                     */
/**********************************************************************************/
void S_00_EccTest(icum_job_t *p_job);
void S_01_CipherAes(icum_job_t *p_job);
void S_01_AuthenticateAes(icum_job_t *p_job);
void S_01_EncryptByRsa(icum_job_t *p_job);
void S_01_DecryptByRsa(icum_job_t *p_job);
void S_01_ProcessAesCmac(icum_job_t *p_job);
void S_01_VerifyRsaSignature(icum_job_t *p_job);
void S_01_MakeRsaSignature(icum_job_t *p_job);
void S_01_GenerateHash(icum_job_t *p_job);
void S_01_AuthenticateAes(icum_job_t *p_job);
void S_01_VerifyEcdsaSignature(icum_job_t *p_job);
void S_01_MakeEcdsaSignature(icum_job_t *p_job);
void S_01_MakeEcdhSecretKey(icum_job_t *p_job);
void S_01_ProcessHmac(icum_job_t *p_job);
void S_01_VerifyEddsaSignature(icum_job_t *p_job);
void S_01_MakeEddsaSignature(icum_job_t *p_job);
uint32_t S_01_CIPHER_CallbackAes(uint32_t cry_result);
void S_02_RAND_InitRng(icum_job_t *p_rand_Job);
void S_02_RAND_ExtendSeed(icum_job_t *p_rand_Job);
void S_02_RAND_GenerateRnd(icum_job_t *p_rand_Job);
void S_03_UpdateAesKeyPlain(icum_job_t *p_job);
void S_03_ImportPublicKey(icum_job_t *p_job);
void S_03_GenerateRsaKey(icum_job_t *p_job);
void S_03_GenerateEccKey(icum_job_t *p_job);
void S_03_ImportHmacKey(icum_job_t *p_job);
void S_04_CollectGarbage(icum_job_t *p_job);
void S_20_CR_ExecuteProcess(icum_job_t *p_cr_Job);
