/******************************************************************************/
/* Component Name   Service 01 EdDSA Common                                   */
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

#ifndef ICUM_S_01_EDDSA_COMMON_H
#define ICUM_S_01_EDDSA_COMMON_H

/**********************************************************************************
* Internal definition
***********************************************************************************/
/* resource control */
#define GET_CTX_NONE            (0x00u)
#define GET_CTX_ECC             (0x01u)
#define GET_CTX_HASH            (0x02u)
#define GET_KEY_ECC             (0x04u)

/* max message size */
#define MAX_EDDSA_MESSAGE_SIZE  (0xFFFFFFBFu)

/* Ed25519 and ED448 key */
#define BYTE_LEN_ED25519_KEY    (32u)
#define BYTE_LEN_ED448_KEY      (60u)
#define BIT_LEN_ED25519_KEY     (256u)
#define BIT_LEN_ED448_KEY       (480u)

/* Ed25519 and ED448 signature length */
#define BYTE_LEN_ED25519_SIGN   (32u)
#define BYTE_LEN_ED448_SIGN     (60u)
#define BIT_LEN_ED25519_SIGN    (256u)
#define BIT_LEN_ED448_SIGN      (480u)

/* setting of 0 */
#define VARIABLE_INITIALIZATION (0u)
#define ZERO                    (0u)
#define NOT_ZERO                (1u)

/* Byte conversion */
#define ROUNDING                (7u)
#define BYTE_CONVERSION         (8u)
#define DOUBLE                  (2u)
#define BYTE_SIZE_IN_BITS       (8u)       /* 1 byte is 8 bits                         */



/**********************************************************************************
* Prototype
***********************************************************************************/
uint32_t s_01_PrepareKeyEddsa(icum_job_t *p_job, ecc_key_t **p_key);
uint32_t s_01_PrepareProcessEddsa(icum_job_t *p_job);
uint32_t s_01_PostProcessEddsa(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id);

#endif /* ICUM_S_01_EDDSA_COMMON_H */
