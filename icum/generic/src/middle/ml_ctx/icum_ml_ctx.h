/******************************************************************************/
/* Component Name   ML Context Manager header file                            */
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

#if !defined(ICUM_ML_CTX_H)
#define ICUM_ML_CTX_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Number of intermediate value area */
#define CTX_NB_AES           (8uL)   /* Secured 8 intermediate value areas for AES         */
#define CTX_NB_RSA_ECC       (1uL)   /* Secured 1 intermediate value areas for RSA or ECC  */
#define CTX_NB_HASH          (2uL)   /* Secured 2 intermediate value areas for HASH        */

/* Byte size of intermediate value area */
#define CTX_SIZE_AES         (28u)    /* Secure 112 byte area with 32 bit type   */
#define CTX_SIZE_RSA_ECC     (2491u)  /* Secure 2648 byte area with 32 bit type */
#define CTX_SIZE_HASH        (128u)   /* Secure 364 byte area with 32 bit type  */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* Intermediate value area for AES */
typedef struct
{
    uint32_t ctx_addr;
    uint32_t context[CTX_SIZE_AES];
    uint32_t reserve;
} ctx_aes_t;

/* Intermediate value area for RSA or ECC*/
typedef struct
{
    uint32_t ctx_addr;
    uint32_t context[CTX_SIZE_RSA_ECC];
    uint32_t reserve;
} ctx_rsa_ecc_t;

/* Intermediate value area for HASH */
typedef struct
{
    uint32_t ctx_addr;
    uint32_t context[CTX_SIZE_HASH];
} ctx_hash_t;

#endif /* #if !defined(ICUM_ML_CTX_H) */
