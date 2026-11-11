/******************************************************************************/
/* Component Name   Crypto library(PRNG) header file                          */
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

#ifndef ICUM_ML_PRNG_PUB_H_
#define ICUM_ML_PRNG_PUB_H_

/**********************************************************************************
* Define
***********************************************************************************/
/* Error code */
#define ER_ML_PRNG_OK        ((uint32_t)ER_OK)
#define ER_ML_PRNG_PARAMETER ((uint32_t)ER_PARAM)
#define ER_ML_PRNG_STATUS    ((uint32_t)ER_ML_PRNG_BASE)
#define ER_ML_PRNG_RESEED    ((uint32_t)(ER_ML_PRNG_BASE + 0x10uL))
#define ER_ML_PRNG_TRN       ((uint32_t)(ER_ML_PRNG_BASE + 0x11uL))
#define ER_ML_PRNG_TIMEOUT   ((uint32_t)(ER_ML_PRNG_BASE + 0x12uL))
#define ER_ML_PRNG_AES_PRM   ((uint32_t)(ER_ML_PRNG_BASE + 0x13uL))

/* OperationMode(Init) */
#define ML_PRNG_MODE_INSTANTIATE ((uint32_t)0uL)   /* CTR_DRBG type(Instantiate) */
#define ML_PRNG_MODE_RESEED      ((uint32_t)1uL)   /* CTR_DRBG type(Reseed) */
#define ML_PRNG_MODE_KAT         ((uint32_t)2uL)   /* CTR_DRBG type(KAT) */

/* True random number generation size */
#define ML_TRNG_SIZE_SHE   ((uint32_t)4uL)  /* SHE (4word) */
#define ML_TRNG_SIZE_NIST  ((uint32_t)8uL)  /* NIST (8word) */
#define ML_TRNG_SIZE_TRNG  ((uint32_t)8uL)  /* TRNG size (8word) */
#define ML_TRNG_SIZE_TEST  ((uint32_t)10uL) /* On Line Test(10word) */

/* Initialize Complete Status */
#define ML_PRNG_INIT_NOT_COMPLETE  ((uint32_t)0uL)  /* Initialization not completion */
#define ML_PRNG_INIT_COMPLETE      ((uint32_t)1uL)  /* Initialization completion */

/* TRNG Status */
#define ML_PRNG_TRN_NOT_GENERATING ((uint32_t)0uL)  /* TRNG Not generation */
#define ML_PRNG_TRN_GENERATING     ((uint32_t)1uL)  /* TRNG Generating */
#define ML_PRNG_TRN_TEST_ERR       ((uint32_t)2uL)  /* TRNG Test Error */

/**********************************************************************************
* Function prototype
***********************************************************************************/
uint32_t ML_PRNG_GenTrng(uint32_t test_mode, uint32_t trng_size);
uint32_t ML_PRNG_InitSHE(const uint32_t* p_EntropySecret_ptr);
uint32_t ML_PRNG_ExtendSHE(const uint32_t *p_prng_Entropy);
uint32_t ML_PRNG_GenSHE(uint32_t *p_prng_DataOut);
uint32_t ML_PRNG_Gen(uint32_t *p_prng_DataOut);
uint32_t ML_PRNG_IsInitialized(uint32_t *p_prng_state);
uint32_t ML_PRNG_GetTrngState(uint32_t* p_trng_state);

#endif /* ICUM_ML_PRNG_PUB_H_ */
