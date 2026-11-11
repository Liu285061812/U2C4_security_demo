/******************************************************************************/
/* Component Name   Cryptographic Library HASH                                */
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

#ifndef ICUM_L_HASH_H
#define ICUM_L_HASH_H

#include "icum_l_cry_pk_pub.h"
#include "r_icum_hash_api.h"

/*================================================================================*/
/* hash size                                                                      */
/*================================================================================*/
#define HASH_BITLEN_224         (224u)
#define HASH_BITLEN_256         (256u)
#define HASH_BITLEN_384         (384u)
#define HASH_BITLEN_512         (512u)

#define BLOCKSIZE_SHA2_224      (64u)
#define BLOCKSIZE_SHA2_256      (64u)
#define BLOCKSIZE_SHA2_384      (128u)
#define BLOCKSIZE_SHA2_512      (128u)
#define BLOCKSIZE_SHA2_512_224  (128u)
#define BLOCKSIZE_SHA2_512_256  (128u)
#define BLOCKSIZE_SHA3_256      (136u)
#define BLOCKSIZE_SHA3_384      (104u)
#define BLOCKSIZE_SHA3_512      (72u)

/* ---- HASH FUNCTIONS ---- */
typedef struct sha512_state
{
    uint64_t    length;
    uint64_t    state[8];
    uint32_t    curlen;
    uint8_t     buf[128];
} sha512_state_t;

typedef struct sha256_state
{
    uint64_t    length;
    uint32_t    state[8];
    uint32_t    curlen;
    uint8_t     buf[64];
} sha256_state_t;


union hash_inner_state
{
    sha512_state_t sha512;
    sha256_state_t sha256;
} ;

uint32_t l_HASH_Register(hash_type_t hash_type, hash_descriptor_t *p_hash_desc);

uint32_t l_HASH_Sha512Init(hash_inner_state_t * md);
uint32_t l_HASH_Sha512Process(hash_inner_state_t * md, const uint8_t *in, uint32_t inlen);
uint32_t l_HASH_Sha512Done(hash_inner_state_t * md, uint8_t *out);

uint32_t l_HASH_Sha384Init(hash_inner_state_t * md);
#define l_HASH_Sha384Process l_HASH_Sha512Process
uint32_t l_HASH_Sha384Done(hash_inner_state_t * md, uint8_t *out);

uint32_t l_HASH_Sha256Init(hash_inner_state_t * md);
uint32_t l_HASH_Sha256Process(hash_inner_state_t * md, const uint8_t *in, uint32_t inlen);
uint32_t l_HASH_Sha256ProcessSlow(hash_inner_state_t * md, const uint8_t *in, uint32_t inlen);
uint32_t l_HASH_Sha256Done(hash_inner_state_t * md, uint8_t *out);

uint32_t l_HASH_Sha224Init(hash_inner_state_t * md);
#define l_HASH_Sha224Process l_HASH_Sha256Process
uint32_t l_HASH_Sha224Done(hash_inner_state_t * md, uint8_t *out);

uint32_t l_hash_convert_type(hash_type_t hash_type, e_r_hash_type_t *p_hash_type);

#endif /* ICUM_L_HASH_H */

