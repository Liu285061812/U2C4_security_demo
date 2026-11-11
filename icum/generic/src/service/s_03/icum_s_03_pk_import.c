/******************************************************************************/
/* Component Name   Service 03 PK_IMPORT                                      */
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

/********************************************************************************
* Header files
********************************************************************************/
#include "r_icumif_api.h"
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_l_cry_pk_pub.h"
#include "icum_ml_ctx_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mh_key_pub.h"
#include "icum_mh_sysinfo_pub.h"
#include "icum_s_pub.h"
#include "icum_k_issd_pub.h"

/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"

#include "icum_s_01_common.h"
#include "icum_s_03_common.h"
#include "icum_service_common.h"

/**********************************************************************************
* Local defines
**********************************************************************************/
#define ECC_KEY_BUFFER_SIZE  ((R_CRYPT_SMALLBIGNUM_BITSIZE + 7u) / 8u)

#define SALT_SIZE           (20u)

/* process_phase */
#define ON_HOLD             (0u)
#define PROCESS_DF_WRITE    (1u)

/* resource control */
#define GET_NONE            (0x00u)
#define GET_CTX_RSA         (0x01u)
#define GET_CTX_HASH        (0x02u)
#define GET_KEY_RSA         (0x04u)

/* key_part_size */
#define KEY_PART_SIZE       (3u)

/* x509 */
#define X509_OFF            (0u)
#define X509_ON             (1u)

/* data size*/
#define DATA_NOT_ZERO       (0x01u)

/* unsigned data size */
#define NUM2                (2u)
#define NUM3                (3u)
#define NUM4                (4u)
#define NUM7                (7u)
#define NUM8                (8u)
#define NUM16               (16u)
#define NUM512              (512u)

/**********************************************************************************
* Local types
**********************************************************************************/
typedef struct
{
    pkc_data_t      *p_key_part[KEY_PART_SIZE];
    uint16_t        key_part_size_in_bytes[KEY_PART_SIZE];
    uint8_t         *p_info;
    uint16_t        info_size;
} sig_input_t;

/* for the parameters of s_03_VerifyKeySub() */
typedef struct
{
    int8_t          x509_flag;              /* use or not use of x509 certification */
    sig_input_t     *p_sig_input;           /* the key material and information to be verified */
    big_num_type_t  bignum_type;            /* the type of bignum */
    uint32_t        input_size_in_bytes;    /* the size of input data */
    rsa_key_t       *p_sig_key;             /* the key for signature */
    rsa_ctx_t       *p_rsa_ctx;             /* RSA context */
} verify_key_t;

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t s_03_GenerateHash (const sig_input_t    *p_input,
                                         hash_type_t    hash_type,
                                         int8_t         x509_flag,
                                         uint8_t        *p_hash);
static uint32_t s_03_VerifyRsaPss (const sig_input_t    *p_input, 
                                   const rsa_key_t      *p_key,
                                         pkc_data_t     *p_signature,
                                         rsa_ctx_t      *p_rsa_ctx,
                                         r_hash_primitive_t hash_primitive,
                                         int8_t         x509_flag,
                                         int32_t        *p_status);
static uint32_t s_03_VerifyRsaPkcsv15 (const sig_input_t *p_input,
                                       const rsa_key_t   *p_key,
                                             pkc_data_t  *p_signature,
                                             rsa_ctx_t   *p_rsa_ctx,
                                             r_hash_primitive_t hash_primitive,
                                             int8_t      x509_flag,
                                             int32_t     *p_status);
static uint32_t s_03_CheckKeyVerifyKey(icum_job_t      *p_job,
                                       ISD_PK_IMPORT_t *p_ISD_PARAM,
                                       rsa_key_t       **p_sig_key,
                                       uint32_t        *p_resource_ctrl);
static uint32_t s_03_CheckMemVerifyKey(ISD_PK_IMPORT_t *p_ISD_PARAM, uint16_t input_size_in_bytes, 
                                       uint8_t expansion_service);
static uint32_t s_03_VerifyKeySub(ISD_PK_IMPORT_t *p_ISD_PARAM, verify_key_t *p_verify_key_param);
static uint32_t s_03_VerifyKey(icum_job_t *p_job, ISD_PK_IMPORT_t *p_ISD_PARAM, int8_t x509_flag,
                               sig_input_t *p_sig_input, uint8_t expansion_service);
static uint32_t s_03_ImportRsaKeyChkPrmX509(icum_job_t *p_job,
                                            ISD_PK_IMPORT_t *p_isd_copy,
                                            int8_t *p_x509_flag,
                                            x509_cert_prm_t *p_x509_cert_prm);
static uint32_t s_03_ImportRsaKeyChkKeyPrm(icum_job_t *p_job,
                                           ISD_PK_IMPORT_t *p_isd_copy,
                                           uint32_t *p_key_size_in_bytes,
                                           big_num_type_t *p_temp_type);
static uint32_t s_03_ImportRsaKeyChkPrm(icum_job_t *p_job,
                                        ISD_PK_IMPORT_t *p_isd_copy,
                                        uint32_t *p_key_size_in_bytes, 
                                        int8_t x509_flag);
static uint32_t s_03_ImportRsaKeyVerify(icum_job_t *p_job,
                                        ISD_PK_IMPORT_t *p_isd_copy,
                                        pkc_data_t *p_modulus_big_num,
                                        const pkc_small_data_t *p_pub_exp_big_num,
                                        const pkc_data_t *p_priv_exp_big_num,
                                        const x509_cert_prm_t *p_x509_cert_prm,
                                        int8_t x509_flag,
                                        const uint32_t *p_key_size_in_bytes);
static uint32_t s_03_ImportRsaKeySetKey(icum_job_t *p_job,
                                        const ISD_PK_IMPORT_t *p_isd_copy,
                                        pkc_data_t *p_modulus_big_num, 
                                        const pkc_small_data_t *p_pub_exp_big_num,
                                        pkc_data_t *p_priv_exp_big_num);
static uint32_t s_03_ImportRsaKey(icum_job_t *p_job);
static uint32_t s_03_ImportEccKeyChkPrmX509(icum_job_t *p_job,
                                            ISD_PK_IMPORT_t *p_isd_copy,
                                            int8_t *p_x509_flag,
                                            x509_cert_prm_t *p_x509_cert_prm);
static uint32_t s_03_CheckParamImportEccKey(icum_job_t *p_job, ISD_PK_IMPORT_t *p_isd_copy,
                                            uint32_t *p_key_size_in_bytes,
                                            big_num_type_t *p_temp_type, int8_t x509_flag);
static uint32_t s_03_ImportEccKeyChkKeyPrm(icum_job_t *p_job,
                                           ISD_PK_IMPORT_t *p_isd_copy,
                                           uint32_t *p_key_size_in_bytes,
                                           big_num_type_t *temp_type);
static uint32_t s_03_ImportEccKeyChkMemPrm(icum_job_t *p_job, ISD_PK_IMPORT_t *p_isd_copy,
                                           uint32_t *p_key_size_in_bytes, uint32_t *p_mem_check_size);
static uint32_t s_03_ImportEccKeyChkPrm(icum_job_t *p_job,
                                        ISD_PK_IMPORT_t *p_isd_copy,
                                        uint32_t *p_key_size_in_bytes, 
                                        int8_t x509_flag);
static uint32_t s_03_ImportEccKeyVerify(icum_job_t *p_job,
                                        ISD_PK_IMPORT_t *p_isd_copy,
                                        pkc_small_data_t *p_point_x_big_num,
                                        const pkc_small_data_t *p_point_y_big_num,
                                        const pkc_small_data_t *p_priv_scalar_big_num,
                                        const x509_cert_prm_t *p_x509_cert_prm,
                                        int8_t x509_flag,
                                        const uint32_t *p_key_size_in_bytes);
static uint32_t s_03_ImportEccKeySetKey(icum_job_t *p_job,
                                        const ISD_PK_IMPORT_t *p_isd_copy,
                                        const pkc_small_data_t *p_point_x_big_num,
                                        const pkc_small_data_t *p_point_y_big_num,
                                        pkc_small_data_t *p_priv_scalar_big_num);
static uint32_t s_03_ImportEccKey(icum_job_t *p_job);
static void s_03_GetHexStringByteSize(uint32_t bit_size, uint32_t *p_string_byte_size);
static uint32_t s_03_EncodeEccKeyValue(icum_job_t *p_job, ISD_PK_IMPORT_t *p_isd_copy,
                                       pkc_small_data_t *p_point_x, pkc_small_data_t *p_point_y,
                                       pkc_small_data_t *p_priv_scalar);
static uint32_t s_03_EncodeImportEccKey(icum_job_t *p_job, ISD_PK_IMPORT_t* p_isd_copy,
                                        uint32_t *p_key_size_in_bytes, big_num_type_t temp_type,
                                        pkc_small_data_t *p_point_x_big_num, pkc_small_data_t *p_point_y_big_num,
                                        pkc_small_data_t *p_priv_scalar_big_num);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0011]:[ICUMFW_UD_S_03_PK_0011]
* Function name: s_03_GenerateHash
* Description  : Generate a hash for signature verification.
* Arguments    : p_input    :[in]   : The address of the key material and information to be verified
*                hash_type  :[in]   : HASH function used for signature verification
*                                  Range: HASH_PRIMITIVE_SHA2_224
*                                         HASH_PRIMITIVE_SHA2_256
*                                         HASH_PRIMITIVE_SHA2_384
*                                         HASH_PRIMITIVE_SHA2_512
*                                         HASH_PRIMITIVE_SHA2_512_224
*                                         HASH_PRIMITIVE_SHA2_512_256
*                x509_flag  :[in]   : flag of using x509 certificate 
*                p_hash     :[out]  : output buffer for the result of hash
* Return Value : ER_OK
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_PENDING
*                R_SALIF_ER_IN_PROCESS
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                R_SALIF_ER_CRY_TIMEOUT
*                R_SALIF_ER_CRY_AES_SEQUENCE_ERROR
*                R_SALIF_ER_CRY_RUNNING
*                R_SALIF_ER_CRY_INVALID_ARG
*                R_SALIF_ER_CRY_ERROR
*                R_SALIF_ER_CRY_BUFFER_OVERFLOW
*                R_SALIF_ER_CRY_INVALID_SIZE
*                R_SALIF_ER_CRY_INVALID_PACKET
*                R_SALIF_ER_CRY_KEYGEN_RETRYOVER
*                R_SALIF_ER_CRY_IN_PROGRESS
*                R_SALIF_ER_CRY_NOT_SUPPORT
*                R_SALIF_ER_CRY_STATUS_ERROR
*                R_SALIF_ER_CRY_PKCC_ERROR
*                R_SALIF_ER_CRY_DECOMP_ERROR
***************************************************************************************/
static uint32_t s_03_GenerateHash (const sig_input_t    *p_input,
                                         hash_type_t    hash_type,
                                         int8_t         x509_flag,
                                         uint8_t        *p_hash)
{
    uint8_t     buffer[R_CRYPT_BIGNUM_BYTESIZE];
    hash_ctx_t  hash_ctx;
    uint32_t    i;
    uint32_t    byte_size = 0u;
    uint32_t    err;

    L_Toolbox_MemSet((uint8_t*)&hash_ctx, NUM0, (int16_t)sizeof(hash_ctx_t));

    /* initialize the hash */
    err = L_HASH_Init(&hash_ctx, hash_type);
    if (ER_OK != err)
    {
        goto end;
    }
    else
    {
        /* Do nothing */
    }

    /* hash the information field (if any) */
    if (NUM0 != (uint32_t)p_input->info_size)
    {
        /* 65535byte data provided, not occur WDT timeout */
        err = L_HASH_Update(&hash_ctx, p_input->p_info, p_input->info_size);
        if (ER_OK != err)
        {
            goto end;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (X509_OFF == (uint32_t)x509_flag)
    {
        /* hash the key parts */
        for (i = 0u; i < KEY_PART_SIZE; i++)
        {
            if (p_input->p_key_part[i] != NULL)
            {
                err = L_HASH_Update(&hash_ctx, (uint8_t *)buffer, byte_size);
                if (ER_OK != err)
                {
                    goto end;
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    else
    {
        /* Do nothing */
    }
    /* retrieve the hash */
    err = L_HASH_Finish(&hash_ctx, p_hash);

end:
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0012]:[ICUMFW_UD_S_03_PK_0012]
* Function name: s_03_VerifyRsaPss
* Description  : Verify the RSA (PSS) signature over the key material.
* Arguments    : p_input       :[in]       : The address of the key material and information to be verified
*                p_key         :[in]       : The RSA public key for signature verification
*                p_signature   :[in]       : Provided signature
*                p_rsa_ctx     :[in/out]   : Address to store the context
*                hash_primitive:[in]       : HASH function used for signature verification
*                                            Range: HASH_PRIMITIVE_SHA2_224
*                                                   HASH_PRIMITIVE_SHA2_256
*                                                   HASH_PRIMITIVE_SHA2_384
*                                                   HASH_PRIMITIVE_SHA2_512
*                                                   HASH_PRIMITIVE_SHA2_512_224
*                                                   HASH_PRIMITIVE_SHA2_512_256
*                x509_flag     :[in]       : flag of using x509 certificate
*                p_status      :[out]      : the result of verification
*                                            Range: 0 (not verified)
*                                                   1 (verified)
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
*                R_SALIF_ER_OK
*                R_SALIF_ER_CRY_INVALID_ARG
*                R_SALIF_ER_CRY_TIMEOUT
*                ER_L_BIGNUM_VAL
***************************************************************************************/
static uint32_t s_03_VerifyRsaPss (const sig_input_t    *p_input, 
                                   const rsa_key_t      *p_key,
                                         pkc_data_t     *p_signature,
                                         rsa_ctx_t      *p_rsa_ctx,
                                         r_hash_primitive_t hash_primitive,
                                         int8_t         x509_flag,
                                         int32_t        *p_status)
{
    uint8_t             hash[HASH_512_SIZE_IN_BYTES];
    hash_type_t         hash_type;
    uint16_t            hash_size;
    hash_ctx_t          hash_ctx;
    uint32_t            err;

    /* PKC Lib Parameter */
    st_pkc_data_t digest;
    st_pkc_data_t sign;
    st_pkcd_rsa_key_bin_t rsa_key;
    st_pkc_operate_opt_t operate_opt;

    /* Initialize parameters for PKC API */
    L_Toolbox_MemSet((uint8_t *)&rsa_key, DATA_ZERO, (int16_t)sizeof(st_pkcd_rsa_key_bin_t));
    L_Toolbox_MemSet((uint8_t *)&operate_opt, DATA_ZERO, (int16_t)sizeof(st_pkc_operate_opt_t));
    L_Toolbox_MemSet((uint8_t *)&hash_ctx, DATA_ZERO, (int16_t)sizeof(hash_ctx_t));

    /* generate the hash */
    (void)L_HASH_GetInternalHashType(hash_primitive, &hash_type, &hash_size);
    err = s_03_GenerateHash(p_input, hash_type, x509_flag, hash);
    if (ER_OK == err)
    {
        /* RSA Init */
        (void)L_PKC_RsaSignSetup(p_rsa_ctx, ENCODING_FORMAT_EMSA_PSS, &hash_ctx, hash_primitive);
        p_rsa_ctx->pkc_ctx.p_mask_option->salt_len = SALT_SIZE;

        rsa_key.key_length = (uint32_t)p_key->p_properties->modulus_size_in_bytes;
        rsa_key.p_n = p_key->p_modulus;
        rsa_key.e_length = (uint32_t)p_key->p_properties->pub_exp_size_in_bytes;
        rsa_key.p_e = p_key->p_pub_exponent;
        digest.p_data = hash;
        digest.len = (uint32_t)hash_size;
        sign.len = p_signature->size;
        sign.p_data = (uint8_t *)p_signature->value;
        err = L_PKC_RsaVerifyInit(&digest, &sign, (void *)&rsa_key, rsa_key.key_length, &p_rsa_ctx->pkc_ctx);
    }
    else
    {
        /* Do nothing */
    }
 
    if (ER_OK == err)
    {
        /* RSA Update */
        operate_opt.sync_opt = PKC_SYNC_OPT_SYNC;
        do
        {
            err = L_PKC_RsaVerifyOperate(&p_rsa_ctx->pkc_ctx, &operate_opt);
        }
        while (ER_IN_PROCESS == err);

        if (ER_OK == err)
        {
            /* RSA Finish */
            err = L_PKC_RsaVerifyFinal(&p_rsa_ctx->pkc_ctx);
            if (ER_OK == err)
            {
                *p_status = VERIFY_OK;
            }
            else if (ER_L_RSA_VERIFY_FAILED == err)
            {
                *p_status = VERIFY_NG;
                err = ER_OK;
        }
        else
        {
                *p_status = VERIFY_NG;
        }
    }
    else
    {
        /* Do nothing */
    }
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0013]:[ICUMFW_UD_S_03_PK_0013]
* Function name: s_03_VerifyRsaPkcsv15
* Description  : Verify the RSA (PKCS#1 v1.5) signature over the key material
* Arguments    : p_input    :[in]       : The address of the key material and information to be verified
*                p_key      :[in]       : The RSA public key for signature verification
*                p_signature   :[in]       : Provided signature
*                p_rsa_ctx     :[in/out]   : Address to store the context
*                hash_primitive:[in]       : HASH function used for signature verification
*                                            Range: HASH_PRIMITIVE_SHA2_224
*                                                   HASH_PRIMITIVE_SHA2_256
*                                                   HASH_PRIMITIVE_SHA2_384
*                                                   HASH_PRIMITIVE_SHA2_512
*                                                   HASH_PRIMITIVE_SHA2_512_224
*                                                   HASH_PRIMITIVE_SHA2_512_256
*                x509_flag     :[in]       : flag of using x509 certificate
*                p_status      :[out]      : the result of verification
*                                            Range: 0 (not verified)
*                                                   1 (verified)
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
*                R_SALIF_ER_OK
*                R_SALIF_ER_CRY_INVALID_ARG
*                R_SALIF_ER_CRY_TIMEOUT
*                ER_L_BIGNUM_VAL
***************************************************************************************/
static uint32_t s_03_VerifyRsaPkcsv15 (const sig_input_t *p_input,
                                       const rsa_key_t   *p_key,
                                             pkc_data_t  *p_signature,
                                             rsa_ctx_t   *p_rsa_ctx,
                                             r_hash_primitive_t hash_primitive,
                                             int8_t      x509_flag,
                                             int32_t     *p_status)
{
    uint8_t             hash[HASH_512_SIZE_IN_BYTES];
    hash_type_t         hash_type;
    uint16_t            hash_size;
    hash_ctx_t          hash_ctx;
    uint32_t        err;

    /* PKC Lib Parameter */
    st_pkc_data_t digest;
    st_pkc_data_t sign;
    st_pkcd_rsa_key_bin_t rsa_key;
    st_pkc_operate_opt_t operate_opt;

    /* Initialize parameters for PKC API */
    L_Toolbox_MemSet((uint8_t *)&rsa_key, DATA_ZERO, (int16_t)sizeof(st_pkcd_rsa_key_bin_t));
    L_Toolbox_MemSet((uint8_t *)&operate_opt, DATA_ZERO, (int16_t)sizeof(st_pkc_operate_opt_t));
    L_Toolbox_MemSet((uint8_t *)&hash_ctx, DATA_ZERO, (int16_t)sizeof(hash_ctx_t));

    /* generate the hash */
    (void)L_HASH_GetInternalHashType(hash_primitive, &hash_type, &hash_size);
    err = s_03_GenerateHash(p_input, hash_type, x509_flag, hash);
    if (ER_OK == err)
    {
        /* RSA Init */
        (void)L_PKC_RsaSignSetup(p_rsa_ctx, ENCODING_FORMAT_EMSA_PKCS1_V15, &hash_ctx, hash_primitive);

        rsa_key.key_length = (uint32_t)p_key->p_properties->modulus_size_in_bytes;
        rsa_key.p_n = p_key->p_modulus;
        rsa_key.p_e = p_key->p_pub_exponent;
        rsa_key.e_length = (uint32_t)p_key->p_properties->pub_exp_size_in_bytes;
        digest.p_data = hash;
        digest.len = (uint32_t)hash_size;
        sign.len = p_signature->size;
        sign.p_data = (uint8_t *)p_signature->value;
        err = L_PKC_RsaVerifyInit(&digest, &sign, (void *)&rsa_key, rsa_key.key_length, &p_rsa_ctx->pkc_ctx);
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK == err)
    {
        /* RSA Update */
        operate_opt.sync_opt = PKC_SYNC_OPT_SYNC;
        do
        {
            err = L_PKC_RsaVerifyOperate(&p_rsa_ctx->pkc_ctx, &operate_opt);
        }
        while (ER_IN_PROCESS == err);

        if (ER_OK == err)
        {
            /* RSA Finish */
            err = L_PKC_RsaVerifyFinal(&p_rsa_ctx->pkc_ctx);
            if (ER_OK == err)
            {
                *p_status = VERIFY_OK;
            }
            else if (ER_L_RSA_VERIFY_FAILED == err)
            {
                *p_status = VERIFY_NG;
                err = ER_OK;
        }
        else
        {
                *p_status = VERIFY_NG;
        }
    }
    else
    {
        /* Do nothing */
    }
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0034]:[ICUMFW_UD_S_03_PK_0034]
* Function name: s_03_CheckKeyVerifyKey
* Description  : Get key to verify the RSA signature
* Arguments    : p_job            :[in]   : job parameter
*                p_ISD_PARAM      :[in]   : ISD parameter
*                p_sig_key        :[out]  : key about signature
*                p_resource_ctrl:[in-out] : Flag for used resource
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                SERV_KEY_NOT_USABLE
***************************************************************************************/
static uint32_t s_03_CheckKeyVerifyKey(icum_job_t      *p_job,
                                       ISD_PK_IMPORT_t *p_ISD_PARAM,
                                       rsa_key_t       **p_sig_key,
                                       uint32_t        *p_resource_ctrl)
{
    uint32_t            err;
    uint8_t             is_allowed;

    /* retrieve the key to be used for verifying the signature */
    switch (p_ISD_PARAM->sig_algo)
    {
        case SIGALGO_RSA_PSS:
        case SIGALGO_RSA_PKCS1_V15:
            err = MH_KEY_GetRsaKey(p_job, p_ISD_PARAM->sig_key_group, p_ISD_PARAM->sig_key_id, p_sig_key);
            if (err != ER_OK)
            {
                /* key not available: abort */
                break;
            }
            else
            {
                /* Do nothing */
            }
            *p_resource_ctrl |= GET_KEY_RSA;

            (void)s_03_CanUseKeyIfDbgConnected((key_flags_t)(*p_sig_key)->p_properties->flags.disable_on_debug,
                                               &is_allowed);
            if (FALSE == (uint32_t)is_allowed)
            {
                /* key not usable: abort */
                err = (uint32_t)SERV_KEY_NOT_USABLE;
                break;
            }
            else
            {
                /* Do nothing */
            }
            break;
        case SIGALGO_NONE:
        default:
            /* error on signature algorithm */
            err = ER_PARAM;
            break;
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0035]:[ICUMFW_UD_S_03_PK_0035]
* Function name: s_03_CheckMemVerifyKey
* Description  : Memory check of verify the RSA signature over the key material
* Arguments    : p_ISD_PARAM      :[in]   : ISD parameter
*                input_size_in_bytes  :[in]   : size for memory check
*                expansion_service:[in] : Flag to select function to use
* Return Value : ER_OK
*                ER_PARAM
*                SERV_SRC_MEM_ERROR
***************************************************************************************/
static uint32_t s_03_CheckMemVerifyKey(ISD_PK_IMPORT_t *p_ISD_PARAM, uint16_t input_size_in_bytes, 
    uint8_t expansion_service)
{
    uint32_t err;
    uint8_t result_of_memCheck;
    uint8_t stringCheck;
    remap_ptr_manip_t   signature_logical;

    err = ER_OK;

    if (0u < (uint32_t)input_size_in_bytes)
    {
        (void)MM_FLS_CheckMemory((uint32_t)p_ISD_PARAM->p_signature,
                                 (uint32_t)input_size_in_bytes,
                                FLS_CHK_MEM_SOURCE,
                                 expansion_service,
                                &result_of_memCheck);

        if ((uint32_t)FLS_CHK_NG == (uint32_t)result_of_memCheck)
        {
            err = (uint32_t)SERV_SRC_MEM_ERROR;
        }
        else
        {
            /* Do nothing */
        }
    } /* end of if (0u != (uint32_t)input_size_in_bytes) */
    else
    {
        /* Do nothing */
    }

    if ((ER_OK == err) &&
        (0u < (uint32_t)p_ISD_PARAM->info_size_in_bytes))
    {
        (void)MM_FLS_CheckMemory((uint32_t)p_ISD_PARAM->p_info,
                                (uint32_t)p_ISD_PARAM->info_size_in_bytes,
                                FLS_CHK_MEM_SOURCE,
                                expansion_service,
                                &result_of_memCheck);
        if ((uint32_t)FLS_CHK_NG == (uint32_t)result_of_memCheck)
        {
            err = (uint32_t)SERV_SRC_MEM_ERROR;
        }
        else
        {
            /* Do nothing */
        }
    } /* end of if ((ER_OK == err) && ... */
    else
    {
        /* Do nothing */
    }

    if (ER_OK == err)
    {
           /* Get Remap(Convert Physical address -> Logical Address) */
           (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD_PARAM->p_signature, &signature_logical.val);

            /* check string format */
        stringCheck = TRUE;
        if (FORMAT_STRING_DECIMAL == p_ISD_PARAM->sig_format)
        {
            /* check decimal string. */
            (void)s_01_IsDigitString(signature_logical.p_ptr, (uint32_t)input_size_in_bytes, &stringCheck);
        }
        else if (FORMAT_STRING_HEXA == p_ISD_PARAM->sig_format)
        {
            /* check hexadecimal string. */
            (void)s_01_IsHexDigitString(signature_logical.p_ptr, (uint32_t)input_size_in_bytes, &stringCheck);
        }
        else
        {
            /* do nothing */
        }
        if (FALSE == (uint32_t)stringCheck)
        {
            err = ER_PARAM;
        }
        else
        {
            /* Do nothing */
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0037]:[ICUMFW_UD_S_03_PK_0037]
* Function name: s_03_VerifyKeySub
* Description  : Sub Routine of verify the RSA signature over the key material
* Arguments    : p_ISD_PARAM         :[in] : ISD parameter
*                p_verify_key_param  :[in] : Parameters of verifying key
* Return Value : ER_OK
*                ER_PARAM
*                SERV_CRYPT_ERROR
*                SERV_KEY_VERIFICATION_FAIL
***************************************************************************************/
static uint32_t s_03_VerifyKeySub(ISD_PK_IMPORT_t *p_ISD_PARAM, verify_key_t *p_verify_key_param)
{
#if !defined(FSW_USE_CRAM)
    pkc_data_t          sig_big_num;
#endif
    pkc_data_t          *p_sig_big_num;
    uint32_t            input_size_in_bytes;
    int32_t             verification_status;
    uint32_t            err;
    hash_type_t         hash_type;
    uint16_t            hash_size;
    uint32_t            skip_flg;
    uint32_t            signature_size;
    remap_ptr_manip_t   signature_logical;

    input_size_in_bytes = (uint32_t)NUM0;
    verification_status = VERIFY_OK;
    err = ER_OK;
    hash_type = (hash_type_t)NUM0;
    hash_size = (uint16_t)NUM0;
    skip_flg = FLG_OFF;

#if defined(FSW_USE_CRAM)
    p_sig_big_num = &g_work_bignum[NUM2];
#else
    p_sig_big_num = &sig_big_num;
#endif

    L_Toolbox_MemSet((uint8_t *)p_sig_big_num, (uint8_t)NUM0, (int16_t)sizeof(pkc_data_t));

    if (X509_OFF == (uint32_t)p_verify_key_param->x509_flag)
    {
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD_PARAM->p_signature,
                &signature_logical.val);
    }
    else
    {
        signature_logical.p_ptr = (uint8_t *)p_ISD_PARAM->p_signature;
    }

    if (FORMAT_STRING_DECIMAL == p_ISD_PARAM->sig_format)
    {
        /* get again length of decimal string. */
        input_size_in_bytes = L_Toolbox_StrLen((const char *)signature_logical.p_ptr);
    }
    else
    {
        input_size_in_bytes = p_verify_key_param->input_size_in_bytes;
    }

    /* convert to big number */
    err = L_BIGNUM_EncodeToBigNum(signature_logical.p_ptr, p_verify_key_param->bignum_type,
                input_size_in_bytes, p_sig_big_num);
    if (ER_OK != err)
    {
        err = ER_PARAM;
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }

    signature_size = ((uint32_t)p_ISD_PARAM->sig_size_in_bits + NUM7) / NUM8;
    err = L_BIGNUM_PaddingData(&signature_size, p_sig_big_num->size,
                               (uint8_t *)p_sig_big_num->value, NUM0);
    p_sig_big_num->size = signature_size;
    if (ER_OK != err)
    {
        err = ER_PARAM;
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        verification_status = VERIFY_NG;

        (void)s_03_GetInternalHashType(p_ISD_PARAM->hash_primitive, &hash_type, &hash_size);
        if (NUM0 == (uint32_t)hash_size)
        {
            /* error on hash algorithm */
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* verify the signature */
        switch (p_ISD_PARAM->sig_algo)
        {
            case SIGALGO_RSA_PSS:
                err = s_03_VerifyRsaPss(p_verify_key_param->p_sig_input, p_verify_key_param->p_sig_key,
                                        p_sig_big_num, p_verify_key_param->p_rsa_ctx, p_ISD_PARAM->hash_primitive,
                                        p_verify_key_param->x509_flag, &verification_status);
                break;
            case SIGALGO_RSA_PKCS1_V15:
                err = s_03_VerifyRsaPkcsv15(p_verify_key_param->p_sig_input, p_verify_key_param->p_sig_key,
                                            p_sig_big_num, p_verify_key_param->p_rsa_ctx, p_ISD_PARAM->hash_primitive,
                                            p_verify_key_param->x509_flag, &verification_status);
                break;
            default:
                /* never occur */
                err = ER_PARAM;
                break;
        }
        if (ER_OK != err)
        {
            /* signature verification failed */
            err = (uint32_t)SERV_CRYPT_ERROR;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        if (VERIFY_OK != verification_status)
        {
            /* signature verification failed */
            err = (uint32_t)SERV_KEY_VERIFICATION_FAIL;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    return err;

}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0010]:[ICUMFW_UD_S_03_PK_0010]
* Function name: s_03_VerifyKey
* Description  : Main Routine of verify the RSA signature over the key material
* Arguments    : p_job             :[in] : Job parameter
*                p_ISD_PARAM       :[in] : ISD parameter
*                x509_flag         :[in] : Flag of using x509 certificate
*                p_sig_input       :[in] : The address of the key material and information to be verified
*                expansion_service :[in] : Flag to select function to use
* Return Value : ER_OK
*                ER_PENDING
*                ER_PARAM
*                SERV_INVALID_KEY_GROUP
*                SERV_KEY_NOT_USABLE
*                SERV_SRC_MEM_ERROR
*                SERV_INVALID_KEY_INDEX
*                SERV_CRYPT_ERROR
*                SERV_KEY_VERIFICATION_FAIL
***************************************************************************************/
static uint32_t s_03_VerifyKey(icum_job_t *p_job, ISD_PK_IMPORT_t *p_ISD_PARAM, int8_t x509_flag,
                               sig_input_t *p_sig_input, uint8_t expansion_service)
{
    rsa_key_t           *p_sig_key;
    big_num_type_t      bignum_type;
    uint32_t            input_size_in_bytes;
    uint32_t            hex_string_bytes;
    uint16_t            dec_string_bytes;
    uint16_t            sig_size_in_bytes;
    uint16_t            mem_check_bytes;
    /* bit0:RSA context, bit1:HASH context, bit2:rsa key */
    uint32_t            resource_ctrl;
    rsa_ctx_t           *p_rsa_ctx;
    uint32_t            err;
    uint32_t            skip_flg;
    verify_key_t        verify_key_param;

    p_sig_key = NULL;
    resource_ctrl = GET_NONE;
    p_rsa_ctx = NULL;
    skip_flg = FLG_OFF;

    /* is there a signature provided? */
    if (SIGALGO_NONE == p_ISD_PARAM->sig_algo)
    {
        /* no: is this authorized? */
        if (NUM0 == (uint32_t)p_ISD_PARAM->key_id)
        {
            /* importing RSA/ECC key index #0 without certificate */
            err = ER_OK;
            skip_flg = FLG_ON;
        }
        else
        {
            /* not authorized for the selected key */
            err = (uint32_t)SERV_INVALID_KEY_INDEX;
            skip_flg = FLG_ON;
        }
    }
    /* end of if (SIGALGO_NONE == p_ISD_PARAM->sig_algo) */
    else
    {
        /* Do nothing */
    }
    /* yes: signature is provided */

    if (FLG_OFF == skip_flg)
    {
        /* get rsa context */
        err = ML_CTX_Get(CTX_TYPE_RSA, (uint32_t **)&p_rsa_ctx);
        if (ER_OK == err)
        {
            resource_ctrl |= GET_CTX_RSA;
            err = s_03_CheckKeyVerifyKey(p_job, p_ISD_PARAM, &p_sig_key, &resource_ctrl);
            if (ER_OK == err)
            {
                /* Do nothing */
            }
            else
            {
                skip_flg = FLG_ON;
            }
        }
        /* end of if (ER_OK == err) */
        else
        {
            skip_flg = FLG_ON;
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        sig_size_in_bytes   = (uint16_t)(((uint32_t)p_ISD_PARAM->sig_size_in_bits + NUM7) / NUM8);
        /* check format of the input (signature) */
        if (FORMAT_STRING_DECIMAL == p_ISD_PARAM->sig_format)
        {
            bignum_type         = E_STRING_DECIMAL;
            (void)s_01_GetDecimalStrlenFromBitCnt(p_ISD_PARAM->sig_size_in_bits, &dec_string_bytes);
            input_size_in_bytes = (uint32_t)dec_string_bytes;
            mem_check_bytes = (uint16_t)(input_size_in_bytes + NUM1);
        }
        else if (FORMAT_STRING_HEXA == p_ISD_PARAM->sig_format)
        {
            bignum_type         = E_STRING_HEXA;
            s_03_GetHexStringByteSize((uint32_t)p_ISD_PARAM->sig_size_in_bits, &hex_string_bytes);
            input_size_in_bytes = hex_string_bytes;
            mem_check_bytes = (uint16_t)(input_size_in_bytes + NUM1);
        }
        else if (FORMAT_BINARY_PLAIN == p_ISD_PARAM->sig_format)
        {
            bignum_type         = E_BINARY_PLAIN;
            input_size_in_bytes = ((uint32_t)p_ISD_PARAM->sig_size_in_bits + NUM7) / NUM8;
            mem_check_bytes = (uint16_t)input_size_in_bytes;
        }
        else
        {
            /* error on number format */
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }
    
    if (FLG_OFF == skip_flg)
    {
        /* make sure the source address of the signature is valid (either RSA or ECC key) */
        /* Note: below check only for plain key, except for x509 cert */
        if (X509_OFF == (uint32_t)x509_flag)
        {
            err = s_03_CheckMemVerifyKey(p_ISD_PARAM, mem_check_bytes, expansion_service);
            if (ER_OK != err)
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
        /* end of if (X509_OFF == x509_flag) */
        else
        {
            /* Do nothing */
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* make sure the modulus size of the signing key and the signature size match */
        if ((((uint32_t)p_sig_key->p_properties->modulus_size_in_bits + NUM7) / NUM8) != (uint32_t)sig_size_in_bytes)
        {
            /* wrong key */
            err = (uint32_t)SERV_INVALID_KEY_INDEX;
            skip_flg = FLG_ON;
        }
        else
        {
            /* sanity check on the signature size */
            if ((uint32_t)sig_size_in_bytes > (R_CRYPT_BIGNUM_BYTESIZE))
            {
                err = ER_PARAM;
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
        /* end of else */
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        verify_key_param.x509_flag           = x509_flag;
        verify_key_param.p_sig_input         = p_sig_input;
        verify_key_param.bignum_type         = bignum_type;
        verify_key_param.input_size_in_bytes = input_size_in_bytes;
        verify_key_param.p_sig_key           = p_sig_key;
        verify_key_param.p_rsa_ctx           = p_rsa_ctx;

        err = s_03_VerifyKeySub(p_ISD_PARAM, &verify_key_param);
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    /* resource release */
    if (GET_CTX_RSA == (resource_ctrl & GET_CTX_RSA))
    {
        (void)ML_CTX_Free(CTX_TYPE_RSA, (uint32_t *)p_rsa_ctx);
    }
    else
    {
        /* Do nothing */
    }
    if (GET_KEY_RSA == (resource_ctrl & GET_KEY_RSA))
    {
        (void)MH_KEY_FreeKeyCache(p_job, p_ISD_PARAM->sig_key_group, p_ISD_PARAM->sig_key_id);
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0023]:[ICUMFW_UD_S_03_PK_0023]
* Function name: s_03_ImportRsaKeyChkPrmX509
* Description  : Check parameter of X509 certificate
* Arguments    : p_job      :[in]       : job parameter
*                p_isd_copy :[in-out]   : ISD parameter
*                p_x509_flag  :[out]    : flag of using x509 certificate
*                p_x509_cert_prm :[out] : x.509 certificate parameter
* Return Value : ER_OK
*                ER_PARAM
*                SERV_SRC_MEM_ERROR
*                SERV_INVALID_KEY_GROUP
***************************************************************************************/
static uint32_t s_03_ImportRsaKeyChkPrmX509(icum_job_t *p_job,
                                            ISD_PK_IMPORT_t *p_isd_copy,
                                            int8_t *p_x509_flag,
                                            x509_cert_prm_t *p_x509_cert_prm)
{
    uint8_t             result_of_memCheck;
    uint32_t            err;
    uint32_t            skip_flg;
    remap_ptr_manip_t   x509_certificate_logical;
    
    skip_flg = FLG_OFF;
    
    p_isd_copy->key_format = FORMAT_BINARY_PLAIN;
    p_isd_copy->sig_format = FORMAT_BINARY_PLAIN;
    p_isd_copy->priv_exp_size_in_bits = 0u;
    *p_x509_flag = (int8_t)X509_ON;

    /* make sure the source address of the x509 cert is valid */
    (void)MM_FLS_CheckMemory((uint32_t)p_isd_copy->p_x509_certificate, 
                             (uint32_t)p_isd_copy->x509_certificate_size_in_bytes,
                              FLS_CHK_MEM_SOURCE,
                              p_job->expansion_service,
                              &result_of_memCheck);

    if ((uint32_t)FLS_CHK_NG == (uint32_t)result_of_memCheck)
    {
        err = (uint32_t)SERV_SRC_MEM_ERROR;
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
       /* Get Remap(Convert Physical address -> Logical Address) */
       (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_copy->p_x509_certificate,
               &x509_certificate_logical.val);

        p_x509_cert_prm->p_cert                = (const uint8_t *)x509_certificate_logical.p_ptr;
        p_x509_cert_prm->cert_size_in_bytes    = p_isd_copy->x509_certificate_size_in_bytes;
        
        err = L_X509_ParseCertificate(p_x509_cert_prm);
        if (ER_OK != err)
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* make sure that extracted key is the RSA key */
        if (E_KEY_TYPE_RSA_PUB_KEY != p_x509_cert_prm->key_type)
        {
            err = (uint32_t)SERV_INVALID_KEY_GROUP;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        p_isd_copy->p_modulus             = p_x509_cert_prm->key.rsa.p_modulus;
        p_isd_copy->modulus_size_in_bits = (uint16_t)(NUM8 *
                                                    (uint32_t)p_x509_cert_prm->key.rsa.modulus_size_in_bytes);
        p_isd_copy->p_pub_exp             = p_x509_cert_prm->key.rsa.p_pub_exp;
        p_isd_copy->pub_exp_size_in_bits = (uint16_t)(NUM8 *
                                                    (uint32_t)p_x509_cert_prm->key.rsa.pub_exp_size_in_bytes);
        p_isd_copy->p_signature           = p_x509_cert_prm->p_signature;
        p_isd_copy->sig_size_in_bits           = (uint16_t)(NUM8 *
                                                    (uint32_t)p_x509_cert_prm->sign_size_in_bytes);

        /* importing RSA/ECC key index #0 without certificate */
        if (NUM0 == (uint32_t)p_isd_copy->key_id)
        {
            p_isd_copy->sig_algo = SIGALGO_NONE; 
        }
        else
        {
            /* Do nothing */
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0024]:[ICUMFW_UD_S_03_PK_0024]
* Function name: s_03_ImportRsaKeyChkKeyPrm
* Description  : Check parameter of RSA public key
* Arguments    : p_job                 :[in]    : job parameter
*                p_isd_copy            :[in-out]: ISD parameter
*                key_size_in_bytes     :[out]   : key size
*                temp_type             :[out]   : temporary key format
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
static uint32_t s_03_ImportRsaKeyChkKeyPrm(icum_job_t *p_job,
                                           ISD_PK_IMPORT_t *p_isd_copy,
                                           uint32_t *p_key_size_in_bytes,
                                           big_num_type_t *p_temp_type)
{
    uint32_t            err;
    uint32_t            skip_flg;
    uint16_t            length;

    err = ER_OK;
    skip_flg = FLG_OFF;

    /* retrieve the user selected key format */
    if (FORMAT_STRING_DECIMAL == p_isd_copy->key_format)
    {
        *p_temp_type = E_STRING_DECIMAL;
        /* get decimal figure length from bit_length. */
        (void)s_01_GetDecimalStrlenFromBitCnt(p_isd_copy->modulus_size_in_bits, &length);
        p_key_size_in_bytes[0] = length;
        (void)s_01_GetDecimalStrlenFromBitCnt(p_isd_copy->pub_exp_size_in_bits, &length);
        p_key_size_in_bytes[1] = length;
        if (0U != (uint32_t)p_isd_copy->priv_exp_size_in_bits)
        {
            (void)s_01_GetDecimalStrlenFromBitCnt(p_isd_copy->priv_exp_size_in_bits, &length);
            p_key_size_in_bytes[NUM2] = length;
        }
        else
        {
            p_key_size_in_bytes[NUM2] = 0u;
        }
    }
    else if (FORMAT_STRING_HEXA == p_isd_copy->key_format)
    {
        *p_temp_type = E_STRING_HEXA;
        s_03_GetHexStringByteSize((uint32_t)p_isd_copy->modulus_size_in_bits, &(p_key_size_in_bytes[0]));
        s_03_GetHexStringByteSize((uint32_t)p_isd_copy->pub_exp_size_in_bits, &(p_key_size_in_bytes[1]));
        if (0U != (uint32_t)p_isd_copy->priv_exp_size_in_bits)
        {
            s_03_GetHexStringByteSize((uint32_t)p_isd_copy->priv_exp_size_in_bits, &(p_key_size_in_bytes[2]));
        }
        else
        {
            p_key_size_in_bytes[NUM2] = 0u;
        }
    }
    else if (FORMAT_BINARY_PLAIN == p_isd_copy->key_format)
    {
        *p_temp_type = E_BINARY_PLAIN;
        p_key_size_in_bytes[0] = (((uint32_t)p_isd_copy->modulus_size_in_bits + NUM7) / NUM8);
        p_key_size_in_bytes[1] = (((uint32_t)p_isd_copy->pub_exp_size_in_bits + NUM7) / NUM8);
        if (0U != (uint32_t)p_isd_copy->priv_exp_size_in_bits)
        {
            p_key_size_in_bytes[NUM2] = (((uint32_t)p_isd_copy->priv_exp_size_in_bits + NUM7) / NUM8);
        }
        else
        {
            p_key_size_in_bytes[NUM2] = 0u;
        }
    }
    else
    {
        /* error on number format */
        err = ER_PARAM;
        skip_flg = FLG_ON;
    }

    if (FLG_OFF == skip_flg)
    {
        if ((0u == 	p_key_size_in_bytes[0]) || (0u == p_key_size_in_bytes[1]))
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* make sure priv_exp/priv_scalar is provided */
        if (KEY_GRP_RSA == p_isd_copy->key_group)
        {
            if ((0u == p_key_size_in_bytes[NUM2]) || (NULL == p_isd_copy->p_priv_exp))
            {
                err = ER_PARAM;
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0025]:[ICUMFW_UD_S_03_PK_0025]
* Function name: s_03_ImportRsaKeyChkPrm
* Description  : Check parameter of RSA public key
* Arguments    : p_job                 :[in]    : job parameter
*                p_isd_copy            :[in-out]: ISD parameter
*                key_size_in_bytes     :[in]    : key size
*                x509_flag             :[in]    : flag of using x509 certificate 
* Return Value : ER_OK
*                ER_PARAM
*                SERV_SRC_MEM_ERROR
***************************************************************************************/
static uint32_t s_03_ImportRsaKeyChkPrm(icum_job_t *p_job,
                                        ISD_PK_IMPORT_t *p_isd_copy,
                                        uint32_t *p_key_size_in_bytes, 
                                        int8_t x509_flag)
{
    uint8_t             result_of_memCheck;
    uint32_t            err;
    uint32_t            skip_flg;
    uint8_t             stringCheck;
    uint32_t            mem_check_size[NUM3];
    remap_ptr_manip_t   modulus_logical;
    remap_ptr_manip_t   pub_exp_logical;
    remap_ptr_manip_t   priv_exp_logical;

    err = ER_OK;
    skip_flg = FLG_OFF;

    /* make sure the source address of the key material is valid (either RSA or ECC key) */
    /* Note: below check only for plain key, except for x509 cert */
    if (X509_OFF == (uint32_t)x509_flag)
    {
        mem_check_size[NUM0] = p_key_size_in_bytes[NUM0];
        mem_check_size[NUM1] = p_key_size_in_bytes[NUM1];
        mem_check_size[NUM2] = p_key_size_in_bytes[NUM2];

        if ((FORMAT_STRING_DECIMAL == p_isd_copy->key_format) ||
            (FORMAT_STRING_HEXA == p_isd_copy->key_format))
        {
            /* increment for terminator(NULL) */
            mem_check_size[NUM0]++;
            mem_check_size[NUM1]++;
            mem_check_size[NUM2]++;
        }
        else
        {
            /* Do nothing */
        }
        (void)MM_FLS_CheckMemory((uint32_t)p_isd_copy->p_modulus,
                mem_check_size[NUM0], FLS_CHK_MEM_SOURCE,
                p_job->expansion_service, &result_of_memCheck);
        if ((uint32_t)FLS_CHK_OK == (uint32_t)result_of_memCheck)
        {
            (void)MM_FLS_CheckMemory((uint32_t)p_isd_copy->p_pub_exp,
                    mem_check_size[NUM1], FLS_CHK_MEM_SOURCE,
                    p_job->expansion_service, &result_of_memCheck);
            if ((uint32_t)FLS_CHK_OK == (uint32_t)result_of_memCheck)
            {
                if (0u < p_key_size_in_bytes[NUM2])
                {
                    (void)MM_FLS_CheckMemory((uint32_t)p_isd_copy->p_priv_exp,
                            mem_check_size[NUM2], FLS_CHK_MEM_SOURCE,
                            p_job->expansion_service, &result_of_memCheck);
                    if ((uint32_t)FLS_CHK_OK == (uint32_t)result_of_memCheck)
                    {
                        /* Do nothing */
                    }
                    else
                    {
                        err = (uint32_t)SERV_SRC_MEM_ERROR;
                        skip_flg = FLG_ON;
                    }
                } /* end of if (0u < p_key_size_in_bytes[NUM2]) */
                else
                {
                    /* Do nothing */
                }
            } /* end of if ((uint32_t)FLS_CHK_OK == (uint32_t)result_of_memCheck) */
            else
            {
                err = (uint32_t)SERV_SRC_MEM_ERROR;
                skip_flg = FLG_ON;
            }
        } /* end of if ((uint32_t)FLS_CHK_OK == (uint32_t)result_of_memCheck) */
        else
        {
            err = (uint32_t)SERV_SRC_MEM_ERROR;
            skip_flg = FLG_ON;
        }

        if (FLG_OFF == skip_flg)
        {
            /* Get Remap(Convert Physical address -> Logical Address) */
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_copy->p_modulus, &modulus_logical.val);
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_copy->p_pub_exp, &pub_exp_logical.val);
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_copy->p_priv_exp, &priv_exp_logical.val);

            /* check string format */
            stringCheck = TRUE;
            if(p_isd_copy->key_format == FORMAT_STRING_DECIMAL)
            {
                /* check decimal string. */
                (void)s_01_IsDigitString(modulus_logical.p_ptr, mem_check_size[NUM0], &stringCheck);
                if (TRUE == (uint32_t)stringCheck)
                {
                    (void)s_01_IsDigitString(pub_exp_logical.p_ptr, mem_check_size[NUM1], &stringCheck);
                    if (TRUE == (uint32_t)stringCheck)
                    {
                        if (0u < p_key_size_in_bytes[NUM2])
                        {
                            (void)s_01_IsDigitString(priv_exp_logical.p_ptr, mem_check_size[NUM2], &stringCheck);
                            if (TRUE == (uint32_t)stringCheck)
                            {
                                /* do nothing */
                            }
                            else
                            {
                                err = ER_PARAM;
                            }
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    else
                    {
                        err = ER_PARAM;
                    }
                }
                else
                {
                    err = ER_PARAM;
                }
            }
            else if(p_isd_copy->key_format == FORMAT_STRING_HEXA)
            {
                /* check hexadecimal string. */
                (void)s_01_IsHexDigitString(modulus_logical.p_ptr, mem_check_size[NUM0], &stringCheck);
                if (TRUE == (uint32_t)stringCheck)
                {
                    (void)s_01_IsHexDigitString(pub_exp_logical.p_ptr, mem_check_size[NUM1], &stringCheck);
                    if (TRUE == (uint32_t)stringCheck)
                    {
                        if (0u < p_key_size_in_bytes[NUM2])
                        {
                            (void)s_01_IsHexDigitString(priv_exp_logical.p_ptr, mem_check_size[NUM2], &stringCheck);
                            if (TRUE == (uint32_t)stringCheck)
                            {
                                /* do nothing */
                            }
                            else
                            {
                                err = ER_PARAM;
                            }
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    else
                    {
                        err = ER_PARAM;
                    }
                }
                else
                {
                    err = ER_PARAM;
                }
            }
            else
            {
                /* do nothing */
            }
        } /* end of if (FLG_OFF == skip_flg) */
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0026]:[ICUMFW_UD_S_03_PK_0026]
* Function name: s_03_ImportRsaKeyVerify
* Description  : Verify signature
* Arguments    : p_job      :[in]   : job parameter
*              : p_isd_copy :[in]   : ISD parameter
*              : p_modulus_big_num  :[in]   : key material
*              : p_pub_exp_big_num  :[in]   : key material
*              : p_priv_exp_big_num :[in]   : key material
*              : p_x509_cert_prm    :[in]   : x.509 certificate parameter
*              : x509_flag          :[in]   : flag of using x509 certificate 
*              : key_size_in_bytes  :[in]   : key size
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_ML_CTX_ALL_USE
*                SERV_KEY_NOT_USABLE
*                SERV_INVALID_KEY_INDEX
*                SERV_CRYPT_ERROR
*                SERV_KEY_VERIFICATION_FAIL
***************************************************************************************/
static uint32_t s_03_ImportRsaKeyVerify(icum_job_t *p_job,
                                        ISD_PK_IMPORT_t *p_isd_copy,
                                        pkc_data_t *p_modulus_big_num,
                                        const pkc_small_data_t *p_pub_exp_big_num,
                                        const pkc_data_t *p_priv_exp_big_num,
                                        const x509_cert_prm_t *p_x509_cert_prm,
                                        int8_t x509_flag,
                                        const uint32_t *p_key_size_in_bytes)
{
    sig_input_t sig_input;
    uint32_t err;
    remap_ptr_manip_t   info_logical;

    err = ER_OK;

    /* do verification signature */
    if (0u == (uint32_t)p_isd_copy->skip_verify)
    {
        /* prepare the material for the signature verification */
        sig_input.p_key_part[NUM0] = p_modulus_big_num;
        sig_input.p_key_part[NUM1] = (p_key_size_in_bytes[NUM1] != 0u) ? (pkc_data_t *)p_pub_exp_big_num  : NULL;
        sig_input.p_key_part[NUM2] = (p_key_size_in_bytes[NUM2] != 0u) ? (pkc_data_t *)p_priv_exp_big_num : NULL;
        
        if (FORMAT_BINARY_PLAIN == p_isd_copy->key_format)
        {
            sig_input.key_part_size_in_bytes[NUM0] = (uint16_t)p_key_size_in_bytes[NUM0];
            sig_input.key_part_size_in_bytes[NUM1] = 0u;
            sig_input.key_part_size_in_bytes[NUM2] = (uint16_t)p_key_size_in_bytes[NUM2];
        }
        else
        {
            sig_input.key_part_size_in_bytes[NUM0] = 0u;
            sig_input.key_part_size_in_bytes[NUM1] = 0u;
            sig_input.key_part_size_in_bytes[NUM2] = 0u;
        }
        
        if (X509_OFF == (uint32_t)x509_flag)
        {
           /* Get Remap(Convert Physical address -> Logical Address) */
           (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_copy->p_info, &info_logical.val);

            sig_input.p_info    = info_logical.p_ptr;
            sig_input.info_size = p_isd_copy->info_size_in_bytes;
        }
        else
        {
            sig_input.p_info    = p_x509_cert_prm->p_signed_data;
            sig_input.info_size = (uint16_t)(p_x509_cert_prm->signed_data_size_in_bytes);
        }

        err = s_03_VerifyKey(p_job, p_isd_copy, x509_flag, &sig_input, p_job->expansion_service);
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0027]:[ICUMFW_UD_S_03_PK_0027]
* Function name: s_03_ImportRsaKeySetKey
* Description  : Update the key in memory
* Arguments    : p_job      :[in]   : job parameter
*              : p_isd_copy :[in]   : ISD parameter
*              : p_modulus_big_num  :[in]   :
*              : p_pub_exp_big_num  :[in]   :
*              : p_priv_exp_big_num :[in]   :
* Return Value : ER_OK
*                ER_IN_PROCESS
*                SERV_INVALID_KEY_GROUP
***************************************************************************************/
static uint32_t s_03_ImportRsaKeySetKey(icum_job_t *p_job,
                                        const ISD_PK_IMPORT_t *p_isd_copy,
                                        pkc_data_t *p_modulus_big_num, 
                                        const pkc_small_data_t *p_pub_exp_big_num, 
                                        pkc_data_t *p_priv_exp_big_num)
{
    rsa_key_t           rsa_key;
    rsa_key_prop_t      rsa_key_properties;
    uint32_t            modulus_size_in_bits;
    uint32_t            err;

    L_Toolbox_MemSet((uint8_t*)&rsa_key,              NUM0, (int16_t)sizeof(rsa_key));
    L_Toolbox_MemSet((uint8_t*)&rsa_key_properties,   NUM0, (int16_t)sizeof(rsa_key_properties));

    /* at this point, the import operation can actually take place */
    switch (p_isd_copy->key_group)
    {
        case KEY_GRP_RSA_PUB:
            /* initialize the key parameters */
            rsa_key.key_group       = p_isd_copy->key_group;
            rsa_key.key_id          = p_isd_copy->key_id;
            rsa_key.p_properties    = &rsa_key_properties;
            rsa_key.p_modulus       = (uint8_t *)p_modulus_big_num->value;
            rsa_key.p_pub_exponent  = (uint8_t *)p_pub_exp_big_num->value;
            
            rsa_key_properties.update_counter          = 0u;

            L_BIGNUM_CountBits((const pkc_data_t *)p_modulus_big_num, &modulus_size_in_bits);
            rsa_key_properties.modulus_size_in_bits    = modulus_size_in_bits;
            rsa_key_properties.modulus_size_in_bytes   = (uint16_t)p_modulus_big_num->size;
            rsa_key_properties.pub_exp_size_in_bytes   = (uint16_t)p_pub_exp_big_num->size;
            rsa_key_properties.priv_exp_size_in_bytes  = 0u;
            rsa_key_properties.flags.disable_on_debug  = p_isd_copy->flags.disable_on_debug;
            rsa_key_properties.flags.write_protected   = p_isd_copy->flags.write_protected;
            rsa_key_properties.flags.invalidate        = 0u;

            /* update the key in memory */
            err = MH_KEY_SetRsaKey(p_job, E_KEY_OPE_FIRST, &rsa_key);
            if (ER_IN_PROCESS != err)
            {
                /* error during the key update */
            }
            else
            {
                /* key updated in memory */
                err = ER_OK;
                p_job->process_phase = PROCESS_DF_WRITE;
            }
            break;
        case KEY_GRP_RSA:
            /* importing a certificate for one the RSA key? */
            
            /* initialize the key parameters */
            rsa_key.key_group       = p_isd_copy->key_group;
            rsa_key.key_id          = p_isd_copy->key_id;
            rsa_key.p_properties    = &rsa_key_properties;
            rsa_key.p_modulus       = (uint8_t *)p_modulus_big_num->value;
            rsa_key.p_pub_exponent  = (uint8_t *)p_pub_exp_big_num->value;
            rsa_key.p_priv_exponent = (uint8_t *)p_priv_exp_big_num->value;

            rsa_key_properties.update_counter           = 0u;
            
            L_BIGNUM_CountBits((const pkc_data_t *)p_modulus_big_num, &modulus_size_in_bits);
            rsa_key_properties.modulus_size_in_bits    = modulus_size_in_bits;
            rsa_key_properties.modulus_size_in_bytes    = (uint16_t)p_modulus_big_num->size;
            rsa_key_properties.pub_exp_size_in_bytes    = (uint16_t)p_pub_exp_big_num->size;
            rsa_key_properties.priv_exp_size_in_bytes   =
                (uint16_t)(((uint32_t)p_isd_copy->priv_exp_size_in_bits + NUM7) / NUM8);
            rsa_key_properties.flags.disable_on_debug   = p_isd_copy->flags.disable_on_debug;
            rsa_key_properties.flags.write_protected    = p_isd_copy->flags.write_protected;
            rsa_key_properties.flags.invalidate         = 0u;

            /* update the key in memory */
            err = MH_KEY_SetRsaKey(p_job, E_KEY_OPE_FIRST, &rsa_key);
            if (ER_IN_PROCESS != err)
            {
                /* error during the key update */
            }
            else
            {
                /* key updated in memory */
                err = ER_OK;
                p_job->process_phase = PROCESS_DF_WRITE;
            }
            break;
        default:
            /* cannot occur */
            err = (uint32_t)SERV_INVALID_KEY_GROUP;
            break;
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0008]:[ICUMFW_UD_S_03_PK_0008]
* Function name: s_03_ImportRsaKey
* Description  : Main Routine of Import RSA Key
* Arguments    : p_job      :[in]   : job parameter
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_ML_CTX_ALL_USE
*                SERV_KEY_NOT_USABLE
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_UNAVAILABLE
*                SERV_SRC_MEM_ERROR
*                SERV_CRYPT_ERROR
*                SERV_KEY_VERIFICATION_FAIL
***************************************************************************************/
static uint32_t s_03_ImportRsaKey(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_ISD;
    ISD_PK_IMPORT_t     isd_copy;
    big_num_type_t      temp_type;
    uint32_t            key_size_in_bytes[NUM3];
    rsa_key_t           rsa_key;
    rsa_key_prop_t      rsa_key_properties;

    uint8_t             x509_signature[R_CRYPT_BIGNUM_BYTESIZE];
    uint8_t             x509_modulus[R_CRYPT_BIGNUM_BYTESIZE];
    uint8_t             x509_pub_exp[NUM16];
    int8_t              x509_flag;
    x509_cert_prm_t     x509_cert_prm;

#if !defined(FSW_USE_CRAM)
    pkc_data_t          modulus_big_num;
    pkc_data_t          priv_exp_big_num;
#endif
    pkc_data_t         *p_modulus_big_num;
    pkc_data_t         *p_priv_exp_big_num;

    pkc_small_data_t    pub_exp_big_num;

    uint32_t            err;
    uint32_t            skip_flg = FLG_OFF;
    remap_ptr_manip_t   modulus_logical;
    remap_ptr_manip_t   pub_exp_logical;
    remap_ptr_manip_t   priv_exp_logical;

    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;

    x509_flag = (int8_t)X509_OFF;

    L_Toolbox_MemSet((uint8_t *)&isd_copy,            NUM0, (int16_t)sizeof(ISD_PK_IMPORT_t));

    L_Toolbox_MemSet(x509_signature,                  NUM0, (int16_t)sizeof(x509_signature));
    L_Toolbox_MemSet(x509_modulus,                    NUM0, (int16_t)sizeof(x509_modulus));
    L_Toolbox_MemSet(x509_pub_exp,                    NUM0, (int16_t)sizeof(x509_pub_exp));

    /* Once the X509 certificate is provided, the isd parameter will be changed and copied once */
    /* In this case(pk_import) the isd parameter is read-only so there is no problem */
    L_Toolbox_MemCpy((uint8_t *)&isd_copy, (const uint8_t *)&p_ISD->prm.PK_IMPORT, (int16_t)sizeof(ISD_PK_IMPORT_t));

    L_Toolbox_MemSet((uint8_t*)&rsa_key,              NUM0, (int16_t)sizeof(rsa_key));
    L_Toolbox_MemSet((uint8_t*)&rsa_key_properties,   NUM0, (int16_t)sizeof(rsa_key_properties));

#if defined(FSW_USE_CRAM)
    p_modulus_big_num = &g_work_bignum[0];
    p_priv_exp_big_num = &g_work_bignum[1];
#else
    p_modulus_big_num = &modulus_big_num;
    p_priv_exp_big_num = &priv_exp_big_num;
#endif
    
    L_Toolbox_MemSet((uint8_t *)p_modulus_big_num,  NUM0, (int16_t)sizeof(pkc_data_t));
    L_Toolbox_MemSet((uint8_t *)p_priv_exp_big_num, NUM0, (int16_t)sizeof(pkc_data_t));
    L_Toolbox_MemSet((uint8_t *)&pub_exp_big_num, NUM0, (int16_t)sizeof(pkc_small_data_t));

    if (PROCESS_DF_WRITE == (uint32_t)p_job->process_phase)
    {
        rsa_key.key_group    = isd_copy.key_group;
        rsa_key.key_id       = isd_copy.key_id;
        /* lc_stage is fixed. If you specify E_KEY_OPE_CONTINUE for operation, lc_stage is not referenced */
        err = MH_KEY_SetRsaKey(p_job, E_KEY_OPE_CONTINUE, &rsa_key);
        if (err == ER_OK)
        {
            p_job->process_phase = ON_HOLD;
        }
        else
        {
            /* Do nothing */
        }
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* X.509 certificate is provided ? */
        if (NUM0 != (uint32_t)isd_copy.x509_certificate_size_in_bytes)
        {
            /* RSA public key extraction from X.509 certificate */
            x509_cert_prm.p_signature                   = &x509_signature[0];
            x509_cert_prm.sign_size_in_bytes            = sizeof(x509_signature);
            x509_cert_prm.key.rsa.p_modulus             = &x509_modulus[0];
            x509_cert_prm.key.rsa.modulus_size_in_bytes = sizeof(x509_modulus);
            x509_cert_prm.key.rsa.p_pub_exp             = &x509_pub_exp[0];
            x509_cert_prm.key.rsa.pub_exp_size_in_bytes = sizeof(x509_pub_exp);

            err = s_03_ImportRsaKeyChkPrmX509(p_job, &isd_copy, &x509_flag, &x509_cert_prm);
            if ((uint32_t)ER_OK != err)
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* check the key size and pointers */
        if (((uint32_t)isd_copy.modulus_size_in_bits > R_CRYPT_BIGNUM_BITSIZE) ||
            ((uint32_t)isd_copy.pub_exp_size_in_bits > R_CRYPT_BIGNUM_BITSIZE) ||
            ((uint32_t)isd_copy.priv_exp_size_in_bits > R_CRYPT_BIGNUM_BITSIZE) ||
            (NULL == isd_copy.p_modulus) ||
            (NULL == isd_copy.p_pub_exp))
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* Check parameter of RSA public key */
        err = s_03_ImportRsaKeyChkKeyPrm(p_job,
                                         &isd_copy,
                                         key_size_in_bytes,
                                         &temp_type);
        if ((uint32_t)ER_OK == err)
        {
            /* Check parameter of RSA public key */
            err = s_03_ImportRsaKeyChkPrm(p_job,
                                          &isd_copy,
                                          key_size_in_bytes,
                                          x509_flag);
            if ((uint32_t)ER_OK == err)
            {
                /* Do nothing */
            }
            else
            {
                skip_flg = FLG_ON;
            }
        }
        else
        {
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
       /* Get Remap(Convert Physical address -> Logical Address) */
       (void)D_REMAP_GetLogicalAddrs((uint32_t)isd_copy.p_modulus, &modulus_logical.val);
       (void)D_REMAP_GetLogicalAddrs((uint32_t)isd_copy.p_pub_exp, &pub_exp_logical.val);
       (void)D_REMAP_GetLogicalAddrs((uint32_t)isd_copy.p_priv_exp, &priv_exp_logical.val);

        /* convert plain key material to big numbers */
        if (FORMAT_STRING_DECIMAL == isd_copy.key_format)
        {
            /* get again length of decimal string. */
            key_size_in_bytes[0] = (uint16_t)L_Toolbox_StrLen((const char *)modulus_logical.p_ptr);
            key_size_in_bytes[1] = (uint16_t)L_Toolbox_StrLen((const char *)pub_exp_logical.p_ptr);
        }
        else
        {
            /* Do nothing */
        }
        err  = L_BIGNUM_EncodeToBigNum(modulus_logical.p_ptr, temp_type,
                key_size_in_bytes[0], p_modulus_big_num);
        err |= L_BIGNUM_EncodeToSmallBigNum(pub_exp_logical.p_ptr,
                                            temp_type,
                                            key_size_in_bytes[1],
                                            &pub_exp_big_num);
        if (key_size_in_bytes[NUM2] != 0u)
        {
            if (FORMAT_STRING_DECIMAL == isd_copy.key_format)
            {
                /* get again length of decimal string. */
                key_size_in_bytes[NUM2] = (uint16_t)L_Toolbox_StrLen((const char *)priv_exp_logical.p_ptr);
            }
            else
            {
                /* Do nothing */
            }
            err |= L_BIGNUM_EncodeToBigNum(priv_exp_logical.p_ptr,
                                        temp_type,
                                        key_size_in_bytes[NUM2],
                                        p_priv_exp_big_num);
        }
        else
        {
            /* Do nothing */
        }
        
        /* error can occur if the size of the data is too big for the internal buffers */
        if (err != ER_OK)
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* do signature verification */
        err = s_03_ImportRsaKeyVerify(p_job, 
                                      &isd_copy,
                                      p_modulus_big_num,
                                      &pub_exp_big_num,
                                      p_priv_exp_big_num,
                                      &x509_cert_prm,
                                      x509_flag,
                                      key_size_in_bytes);
        if ((uint32_t)ER_OK == err)
        {
            /* Distribute processing by key group of import destination    */
            err = s_03_ImportRsaKeySetKey(p_job,
                                          &isd_copy, 
                                          p_modulus_big_num, 
                                          &pub_exp_big_num, 
                                          p_priv_exp_big_num);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0029]:[ICUMFW_UD_S_03_PK_0029]
* Function name: s_03_ImportEccKeyChkPrmX509
* Description  : Check parameter of X509 certificate
* Arguments    : p_job      :[in]   : job parameter
*                p_isd_copy :[in-out]   : ISD parameter
*                p_x509_flag  :[out]       : flag of using x509 certificate
*                p_x509_cert_prm  :[out]       : flag of using x509 certificate 
* Return Value : ER_OK
*                ER_PARAM
*                SERV_SRC_MEM_ERROR
*                SERV_INVALID_KEY_GROUP
***************************************************************************************/
static uint32_t s_03_ImportEccKeyChkPrmX509(icum_job_t *p_job,
                                            ISD_PK_IMPORT_t *p_isd_copy,
                                            int8_t *p_x509_flag,
                                            x509_cert_prm_t *p_x509_cert_prm)
{
    uint8_t             result_of_memCheck;
    uint32_t            err;
    uint32_t            skip_flg;
    uint32_t            key_byte_size;
    remap_ptr_manip_t   x509_certificate_logical;

    p_isd_copy->key_format = FORMAT_BINARY_PLAIN;
    p_isd_copy->sig_format = FORMAT_BINARY_PLAIN;
    p_isd_copy->priv_exp_size_in_bits = 0u;
    key_byte_size = 0u;
    *p_x509_flag = (int8_t)X509_ON;
    skip_flg = FLG_OFF;

    /* make sure the source address of the x509 cert is valid */
    (void)MM_FLS_CheckMemory((uint32_t)p_isd_copy->p_x509_certificate,
                             (uint32_t)p_isd_copy->x509_certificate_size_in_bytes,
                              FLS_CHK_MEM_SOURCE,
                              p_job->expansion_service,
                              &result_of_memCheck);
    if ((uint32_t)FLS_CHK_NG == (uint32_t)result_of_memCheck)
    {
        err = (uint32_t)SERV_SRC_MEM_ERROR;
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }
    /********************************************************************
    * ECC public key extraction from X.509 certificate
    ********************************************************************/

    if (FLG_OFF == skip_flg)
    {
       /* Get Remap(Convert Physical address -> Logical Address) */
       (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_copy->p_x509_certificate,
               &x509_certificate_logical.val);

        p_x509_cert_prm->p_cert                        = (const uint8_t *)x509_certificate_logical.p_ptr;
        p_x509_cert_prm->cert_size_in_bytes            = p_isd_copy->x509_certificate_size_in_bytes;

        err = L_X509_ParseCertificate(p_x509_cert_prm);
        if (ER_OK != err)
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        err = MH_KEY_GetEccKeySize(p_x509_cert_prm->curve_id, &key_byte_size);
        if (ER_OK != err)
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        p_isd_copy->p_pub_point_x         = p_x509_cert_prm->key.ecc.p_pub_point_x;
        p_isd_copy->point_x_size_in_bits =
            (uint16_t)(NUM8 * (uint32_t)p_x509_cert_prm->key.ecc.point_x_size_in_bytes);
        p_isd_copy->p_pub_point_y         = p_x509_cert_prm->key.ecc.p_pub_point_y;
        p_isd_copy->point_y_size_in_bits =
            (uint16_t)(NUM8 * (uint32_t)p_x509_cert_prm->key.ecc.point_y_size_in_bytes);
        p_isd_copy->p_signature           = p_x509_cert_prm->p_signature;
        p_isd_copy->sig_size_in_bits           =
            (uint16_t)(NUM8 * (uint32_t)p_x509_cert_prm->sign_size_in_bytes);
        
        /* make sure that extracted key is the ECC key */
        if (E_KEY_TYPE_ECC_PUB_KEY != p_x509_cert_prm->key_type)
        {
            err = (uint32_t)SERV_INVALID_KEY_GROUP;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* importing RSA/ECC key index #0 without certificate */
        if (0u == (uint32_t)p_isd_copy->key_id)
        {
            p_isd_copy->sig_algo = SIGALGO_NONE; 
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0030]:[ICUMFW_UD_S_03_PK_0030]
* Function name: s_03_ImportEccKeyChkKeyPrm
* Description  : Check parameter of ECC public key
* Arguments    : p_job                 :[in]    : job parameter
*                p_isd_copy            :[in-out]: ISD parameter
*                key_size_in_bytes     :[out]   : key size
*                temp_type             :[out]   : temporary key format
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
static uint32_t s_03_ImportEccKeyChkKeyPrm(icum_job_t *p_job,
                                           ISD_PK_IMPORT_t *p_isd_copy,
                                           uint32_t *p_key_size_in_bytes,
                                           big_num_type_t *temp_type)
{
    uint32_t            err;
    uint32_t            skip_flg;
    uint16_t            length;
    uint32_t            curve_key_size_in_bits;

    err = ER_OK;
    skip_flg = FLG_OFF;

    /* retrieve the user selected key format */
    if (FORMAT_STRING_DECIMAL == p_isd_copy->key_format)
    {
        *temp_type = E_STRING_DECIMAL;
        /* get decimal figure length from bit_length. */
        (void)s_01_GetDecimalStrlenFromBitCnt(p_isd_copy->point_x_size_in_bits, &length);
        p_key_size_in_bytes[NUM0] = length;
        (void)s_01_GetDecimalStrlenFromBitCnt(p_isd_copy->point_y_size_in_bits, &length);
        p_key_size_in_bytes[NUM1] = length;
        if (NUM0 != (uint32_t)p_isd_copy->priv_scalar_size_in_bits)
        {
            (void)s_01_GetDecimalStrlenFromBitCnt(p_isd_copy->priv_scalar_size_in_bits, &length);
             p_key_size_in_bytes[NUM2] = length;
        }
        else
        {
            p_key_size_in_bytes[NUM2] = 0u;
        }
    }
    else if (FORMAT_STRING_HEXA == p_isd_copy->key_format)
    {
        *temp_type = E_STRING_HEXA;
        s_03_GetHexStringByteSize((uint32_t)p_isd_copy->point_x_size_in_bits, &(p_key_size_in_bytes[NUM0]));
        s_03_GetHexStringByteSize((uint32_t)p_isd_copy->point_y_size_in_bits, &(p_key_size_in_bytes[NUM1]));
        if (NUM0 != (uint32_t)p_isd_copy->priv_scalar_size_in_bits)
        {
            s_03_GetHexStringByteSize((uint32_t)p_isd_copy->priv_scalar_size_in_bits, &(p_key_size_in_bytes[NUM2]));
        }
        else
        {
            p_key_size_in_bytes[NUM2] = 0u;
        }
    }
    else if (FORMAT_BINARY_PLAIN == p_isd_copy->key_format)
    {
        *temp_type = E_BINARY_PLAIN;
        p_key_size_in_bytes[NUM0] = (((uint32_t)p_isd_copy->point_x_size_in_bits + NUM7) / NUM8);
        p_key_size_in_bytes[NUM1] = (((uint32_t)p_isd_copy->point_y_size_in_bits + NUM7) / NUM8);
        if (0u != (uint32_t)p_isd_copy->priv_scalar_size_in_bits)
        {
            p_key_size_in_bytes[NUM2] = (((uint32_t)p_isd_copy->priv_scalar_size_in_bits + NUM7) / NUM8);
        }
        else
        {
            p_key_size_in_bytes[NUM2] = 0u;
        }
    }
    else
    {
        /* error on number format */
        err = ER_PARAM;
        skip_flg = FLG_ON;
    }


    if (FLG_OFF == skip_flg)
    {
        if ((ECC_CURVE_CURVE25519 == p_isd_copy->curve_id) ||
            (ECC_CURVE_CURVE448 == p_isd_copy->curve_id) ||
            (ECC_CURVE_ED25519 == p_isd_copy->curve_id) ||
            (ECC_CURVE_ED448 == p_isd_copy->curve_id))
        {
            /* curve25519 is only x-coordinate */
            if (0u == p_key_size_in_bytes[0])
            {
                err = ER_PARAM;
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            if ((0u == p_key_size_in_bytes[NUM0]) || (0u == p_key_size_in_bytes[NUM1]))
            {
                err = ER_PARAM;
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* make sure priv_exp/priv_scalar is provided */
        if (KEY_GRP_ECC == p_isd_copy->key_group)
        {
            if ((0u == p_key_size_in_bytes[NUM2]) || (NULL == p_isd_copy->p_priv_scalar))
            {
                err = ER_PARAM;
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        (void)MH_KEY_GetEccKeySize(p_isd_copy->curve_id, &curve_key_size_in_bits);
        curve_key_size_in_bits *= NUM8;

        if (((NUM0 != (uint32_t)p_isd_copy->point_x_size_in_bits) &&
             (curve_key_size_in_bits != (uint32_t)p_isd_copy->point_x_size_in_bits)) ||
            ((NUM0 != (uint32_t)p_isd_copy->point_y_size_in_bits) &&
             (curve_key_size_in_bits != (uint32_t)p_isd_copy->point_y_size_in_bits)) ||
            ((NUM0 != (uint32_t)p_isd_copy->priv_scalar_size_in_bits) &&
             (curve_key_size_in_bits != (uint32_t)p_isd_copy->priv_scalar_size_in_bits)))
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0036]:[ICUMFW_UD_S_03_PK_0036]
* Function name: s_03_ImportEccKeyChkMemPrm
* Description  : Check memory parameter of ECC public key
* Arguments    : p_job                 :[in]    : job parameter
*                p_isd_copy            :[in-out]: ISD parameter
*                key_size_in_bytes     :[in]    : key size
*                p_mem_check_size      :[in]    : memory size
* Return Value : ER_OK
*                ER_MH_KEY_SRC_MEM_ERROR
***************************************************************************************/
static uint32_t s_03_ImportEccKeyChkMemPrm(icum_job_t *p_job, ISD_PK_IMPORT_t *p_isd_copy,
                                           uint32_t *key_size_in_bytes, uint32_t *p_mem_check_size)
{
    uint8_t             result_of_memCheck;
    uint32_t            err;

    err = ER_OK;

    (void)MM_FLS_CheckMemory((uint32_t)p_isd_copy->p_pub_point_x,
            p_mem_check_size[NUM0], FLS_CHK_MEM_SOURCE,
            p_job->expansion_service, &result_of_memCheck);
    if ((uint32_t)FLS_CHK_OK == (uint32_t)result_of_memCheck)
    {
        if ((ECC_CURVE_CURVE25519 == p_isd_copy->curve_id) ||
            (ECC_CURVE_CURVE448 == p_isd_copy->curve_id) ||
            (ECC_CURVE_ED25519 == p_isd_copy->curve_id) ||
            (ECC_CURVE_ED448 == p_isd_copy->curve_id))
        {
            /* curve25519 is only x-coordinate */
            /* Do nothing */
        }
        else
        {
            (void)MM_FLS_CheckMemory((uint32_t)p_isd_copy->p_pub_point_y,
                    p_mem_check_size[NUM1], FLS_CHK_MEM_SOURCE,
                    p_job->expansion_service, &result_of_memCheck);
        }

        if ((uint32_t)FLS_CHK_OK == (uint32_t)result_of_memCheck)
        {
            if (NUM0 < key_size_in_bytes[NUM2])
            {
                (void)MM_FLS_CheckMemory((uint32_t)p_isd_copy->p_priv_scalar,
                        p_mem_check_size[NUM2], FLS_CHK_MEM_SOURCE,
                        p_job->expansion_service, &result_of_memCheck);
                if ((uint32_t)FLS_CHK_OK == (uint32_t)result_of_memCheck)
                {
                    /* Do nothing */
                }
                else
                {
                    err = (uint32_t)SERV_SRC_MEM_ERROR;
                }
            } /* end of if (NUM0 < key_size_in_bytes[NUM2]) */
            else
            {
                /* Do nothing */
            }
        } /* end of if ((uint32_t)FLS_CHK_OK == (uint32_t)result_of_memCheck) */
        else
        {
            err = (uint32_t)SERV_SRC_MEM_ERROR;
        }
    } /* end of if ((uint32_t)FLS_CHK_OK == (uint32_t)result_of_memCheck) */
    else
    {
        err = (uint32_t)SERV_SRC_MEM_ERROR;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0031]:[ICUMFW_UD_S_03_PK_0031]
* Function name: s_03_ImportEccKeyChkPrm
* Description  : Check parameter of ECC public key
* Arguments    : p_job                 :[in]    : job parameter
*                p_isd_copy            :[in-out]: ISD parameter
*                key_size_in_bytes     :[out]   : key size
*                x509_flag             :[in]    : flag of using x509 certificate 
* Return Value : ER_OK
*                ER_PARAM
*                ER_MH_KEY_SRC_MEM_ERROR
***************************************************************************************/
static uint32_t s_03_ImportEccKeyChkPrm(icum_job_t *p_job,
                                        ISD_PK_IMPORT_t *p_isd_copy,
                                        uint32_t *p_key_size_in_bytes, 
                                        int8_t x509_flag)
{
    uint32_t            err;
    uint8_t             stringCheck;
    uint32_t            mem_check_size[NUM3];
    remap_ptr_manip_t   pub_point_x_logical;
    remap_ptr_manip_t   pub_point_y_logical;
    remap_ptr_manip_t   priv_scalar_logical;

    err = ER_OK;

    /* make sure the source address of the key material is valid (either RSA or ECC key) */
    /* Note: below check only for plain key, except for x509 cert */
    if (X509_OFF == (uint32_t)x509_flag)
    {
        mem_check_size[NUM0] = p_key_size_in_bytes[NUM0];
        mem_check_size[NUM1] = p_key_size_in_bytes[NUM1];
        mem_check_size[NUM2] = p_key_size_in_bytes[NUM2];

        if ((FORMAT_STRING_DECIMAL == p_isd_copy->key_format) ||
            (FORMAT_STRING_HEXA == p_isd_copy->key_format))
        {
            /* increment for terminator(NULL) */
            mem_check_size[NUM0]++;
            mem_check_size[NUM1]++;
            mem_check_size[NUM2]++;
        }
        else
        {
            /* do nothing */
        }

        err = s_03_ImportEccKeyChkMemPrm(p_job, p_isd_copy, p_key_size_in_bytes, mem_check_size);
        if (ER_OK == err)
        {
           /* Get Remap(Convert Physical address -> Logical Address) */
           (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_copy->p_pub_point_x,
                   &pub_point_x_logical.val);
           (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_copy->p_pub_point_y,
                   &pub_point_y_logical.val);
           (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_copy->p_priv_scalar,
                   &priv_scalar_logical.val);

            /* check string format */
            stringCheck = TRUE;
            if (FORMAT_STRING_DECIMAL == p_isd_copy->key_format)
            {
                /* check decimal string. */
                (void)s_01_IsDigitString(pub_point_x_logical.p_ptr, mem_check_size[NUM0], &stringCheck);
                if (TRUE == (uint32_t)stringCheck)
                {
                    if ((ECC_CURVE_CURVE25519 == p_isd_copy->curve_id) ||
                        (ECC_CURVE_CURVE448 == p_isd_copy->curve_id) ||
                        (ECC_CURVE_ED25519 == p_isd_copy->curve_id) ||
                        (ECC_CURVE_ED448 == p_isd_copy->curve_id))
                    {
                        /* curve25519 is only x-coordinate */
                        /* Do nothing */
                    }
                    else
                    {
                        (void)s_01_IsDigitString(pub_point_y_logical.p_ptr, mem_check_size[NUM1], &stringCheck);
                    }
                    if (TRUE == (uint32_t)stringCheck)
                    {
                        if (0u < p_key_size_in_bytes[NUM2])
                        {
                            (void)s_01_IsDigitString(priv_scalar_logical.p_ptr, mem_check_size[NUM2], &stringCheck);
                            if (TRUE == (uint32_t)stringCheck)
                            {
                                /* do nothing */
                            }
                            else
                            {
                                err = ER_PARAM;
                            }
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    /* end of if (TRUE == (uint32_t)stringCheck) */
                    else
                    {
                        err = ER_PARAM;
                    }
                }
                else
                {
                    err = ER_PARAM;
                }
            }
            /* end of if (FORMAT_STRING_DECIMAL == p_isd_copy->key_format) */
            else if (FORMAT_STRING_HEXA == p_isd_copy->key_format)
            {
                /* check hexadecimal string. */
                (void)s_01_IsHexDigitString(pub_point_x_logical.p_ptr, mem_check_size[NUM0], &stringCheck);
                if (TRUE == (uint32_t)stringCheck)
                {
                    if ((ECC_CURVE_CURVE25519 == p_isd_copy->curve_id) ||
                        (ECC_CURVE_CURVE448 == p_isd_copy->curve_id) ||
                        (ECC_CURVE_ED25519 == p_isd_copy->curve_id) ||
                        (ECC_CURVE_ED448 == p_isd_copy->curve_id))
                    {
                        /* curve25519 is only x-coordinate */
                        /* Do nothing */
                    }
                    else
                    {
                        (void)s_01_IsHexDigitString(pub_point_y_logical.p_ptr, mem_check_size[NUM1], &stringCheck);
                    }
                    if (TRUE == (uint32_t)stringCheck)
                    {
                        if (0u < p_key_size_in_bytes[NUM2])
                        {
                            (void)s_01_IsHexDigitString(priv_scalar_logical.p_ptr, mem_check_size[NUM2], &stringCheck);
                            if (TRUE == (uint32_t)stringCheck)
                            {
                                /* do nothing */
                            }
                            else
                            {
                                err = ER_PARAM;
                            }
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    /* end of if (TRUE == (uint32_t)stringCheck) */
                    else
                    {
                        err = ER_PARAM;
                    }
                }
                else
                {
                    err = ER_PARAM;
                }
            }
            /* end of else if (FORMAT_STRING_HEXA == p_isd_copy->key_format) */
            else
            {
                /* do nothing */
            }
        }
        /* end of if (ER_OK == err) */
        else
        {
            /* do nothing */
        }
    }
    /* end of if (X509_OFF == x509_flag) */
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0032]:[ICUMFW_UD_S_03_PK_0032]
* Function name: s_03_ImportEccKeyVerify
* Description  : Verify signature
* Arguments    : p_job      :[in]   : job parameter
*              : p_isd_copy :[in]   : ISD parameter
*              : p_point_x_big_num :[in]   : key material
*              : p_point_y_big_num :[in]   : key material
*              : p_priv_scalar_big_num :[in]   : key material
*              : p_x509_cert_prm    :[in]   : x.509 certificate parameter
*              : x509_flag          :[in]   : flag of using x509 certificate 
*              : key_size_in_bytes  :[in]   : key size
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_ML_CTX_ALL_USE
*                SERV_KEY_NOT_USABLE
*                SERV_INVALID_KEY_INDEX
*                SERV_CRYPT_ERROR
*                SERV_KEY_VERIFICATION_FAIL
***************************************************************************************/
static uint32_t s_03_ImportEccKeyVerify(icum_job_t *p_job,
                                        ISD_PK_IMPORT_t *p_isd_copy,
                                        pkc_small_data_t *p_point_x_big_num,
                                        const pkc_small_data_t *p_point_y_big_num,
                                        const pkc_small_data_t *p_priv_scalar_big_num,
                                        const x509_cert_prm_t *p_x509_cert_prm,
                                        int8_t x509_flag,
                                        const uint32_t *p_key_size_in_bytes)
{
    sig_input_t sig_input;
    uint32_t err;
    remap_ptr_manip_t   info_logical;

    err = ER_OK;

    /* do signature verification */
    if (0u == (uint32_t)p_isd_copy->skip_verify)
    {
        /* prepare the material for the signature verification */
        sig_input.p_key_part[NUM0] = (pkc_data_t *)p_point_x_big_num;
        sig_input.p_key_part[NUM1] = (p_key_size_in_bytes[NUM1] != 0u) ? (pkc_data_t *)p_point_y_big_num  : NULL;
        sig_input.p_key_part[NUM2] = (p_key_size_in_bytes[NUM2] != 0u) ? (pkc_data_t *)p_priv_scalar_big_num : NULL;
        
        if (FORMAT_BINARY_PLAIN == p_isd_copy->key_format)
        {
            sig_input.key_part_size_in_bytes[NUM0] = (uint16_t)p_key_size_in_bytes[NUM0];
            sig_input.key_part_size_in_bytes[NUM1] = (uint16_t)p_key_size_in_bytes[NUM1];
            sig_input.key_part_size_in_bytes[NUM2] = (uint16_t)p_key_size_in_bytes[NUM2];
        }
        else
        {
            sig_input.key_part_size_in_bytes[NUM0] = 0u;
            sig_input.key_part_size_in_bytes[NUM1] = 0u;
            sig_input.key_part_size_in_bytes[NUM2] = 0u;
        }
        
        if (X509_OFF == (uint32_t)x509_flag)
        {
           /* Get Remap(Convert Physical address -> Logical Address) */
           (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_copy->p_info, &info_logical.val);

            sig_input.p_info    = info_logical.p_ptr;
            sig_input.info_size = p_isd_copy->info_size_in_bytes;
        }
        else
        {
            sig_input.p_info    = p_x509_cert_prm->p_signed_data;
            sig_input.info_size = (uint16_t)(p_x509_cert_prm->signed_data_size_in_bytes);
        }
        err = s_03_VerifyKey(p_job, p_isd_copy, x509_flag, &sig_input, p_job->expansion_service);
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0033]:[ICUMFW_UD_S_03_PK_0033]
* Function name: s_03_ImportEccKeySetKey
* Description  : Update the key in memory
* Arguments    : p_job      :[in]   : job parameter
*              : p_isd_copy :[in]   : ISD parameter
*              : p_point_x_big_num :[in]   : key material
*              : p_point_y_big_num :[in]   : key material
*              : p_priv_scalar_big_num     :[in]   : key material
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_WRITING
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_PROG
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_DEST_MEM_ERROR
*                ER_MH_KEY_WRITE_PROTECTED
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_CURVE_ID
*                SERV_INVALID_KEY_GROUP
***************************************************************************************/
static uint32_t s_03_ImportEccKeySetKey(icum_job_t *p_job,
                                        const ISD_PK_IMPORT_t *p_isd_copy,
                                        const pkc_small_data_t *p_point_x_big_num,
                                        const pkc_small_data_t *p_point_y_big_num,
                                        pkc_small_data_t *p_priv_scalar_big_num)
{
    ecc_point_affine_t  pub_point;
    ecc_key_t           ecc_key;
    ecc_key_prop_t      ecc_key_properties;
    uint32_t            err;

    L_Toolbox_MemSet((uint8_t *)&pub_point,           NUM0, (int16_t)sizeof(pub_point));
    L_Toolbox_MemSet((uint8_t*)&ecc_key,              NUM0, (int16_t)sizeof(ecc_key));
    L_Toolbox_MemSet((uint8_t*)&ecc_key_properties,   NUM0, (int16_t)sizeof(ecc_key_properties));
    L_Toolbox_MemCpy((uint8_t *)&pub_point.x, (const uint8_t *)p_point_x_big_num, (int16_t)sizeof(pkc_small_data_t));
    
    if ((ECC_CURVE_CURVE25519 != p_isd_copy->curve_id) &&
        (ECC_CURVE_CURVE448 != p_isd_copy->curve_id) &&
        (ECC_CURVE_ED25519 != p_isd_copy->curve_id) &&
        (ECC_CURVE_ED448 != p_isd_copy->curve_id))
    {
        L_Toolbox_MemCpy((uint8_t *)&pub_point.y, (const uint8_t *)p_point_y_big_num, (int16_t)sizeof(pkc_small_data_t));
    }
    else
    {
        /* Do nothing */
    }

    /* at this point, the import operation can actually take place */
    switch (p_isd_copy->key_group)
    {
        case KEY_GRP_ECC_PUB:
            /* initialize the key parameters */
            ecc_key.key_group    = p_isd_copy->key_group;
            ecc_key.key_id       = p_isd_copy->key_id;
            ecc_key.p_properties = &ecc_key_properties;
            ecc_key.p_pub_point  = &pub_point;
            ecc_key.p_priv_scalar = p_priv_scalar_big_num;

            ecc_key_properties.update_counter           = 0u;
            ecc_key_properties.ecc_curve                = p_isd_copy->curve_id;
            ecc_key_properties.priv_scalar_size_in_bits = 0u;
            ecc_key_properties.flags.disable_on_debug   = p_isd_copy->flags.disable_on_debug;
            ecc_key_properties.flags.write_protected    = p_isd_copy->flags.write_protected;
            ecc_key_properties.flags.invalidate         = 0u;

            /* update the key in memory */
            err = MH_KEY_SetEccKey(p_job, E_KEY_OPE_FIRST, &ecc_key);
            if (err != ER_IN_PROCESS)
            {
                /* error during the key update */
            }
            else
            {
                /* key updated in memory */
                p_job->process_phase = PROCESS_DF_WRITE;
            }
            break;

        case KEY_GRP_ECC:
        case KEY_GRP_ECC_RAM:
            /* initialize the key parameters */
            ecc_key.key_group     = p_isd_copy->key_group;
            ecc_key.key_id        = p_isd_copy->key_id;
            ecc_key.p_properties  = &ecc_key_properties;
            ecc_key.p_pub_point   = &pub_point;
            ecc_key.p_priv_scalar = p_priv_scalar_big_num;

            ecc_key_properties.update_counter           = 0u;
            ecc_key_properties.ecc_curve                = p_isd_copy->curve_id;
            ecc_key_properties.priv_scalar_size_in_bits = p_isd_copy->priv_scalar_size_in_bits;
            ecc_key_properties.flags.disable_on_debug   = p_isd_copy->flags.disable_on_debug;
            ecc_key_properties.flags.write_protected    = p_isd_copy->flags.write_protected;
            ecc_key_properties.flags.invalidate         = 0u;

            /* update the key in memory */
            err = MH_KEY_SetEccKey(p_job, E_KEY_OPE_FIRST, &ecc_key);
            if (ER_IN_PROCESS == err)
            {
                /* OK, Go to the next phase */
                p_job->process_phase = PROCESS_DF_WRITE;
            }
            else
            {
                /* error during the key update */
            }
            break;
        default:
            /* never occur */
            err = (uint32_t)SERV_INVALID_KEY_GROUP;
            break;
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0038]:[ICUMFW_UD_S_03_PK_0038]
* Function name: s_03_CheckParamImportEccKey
* Description  : Parameter check of Import ECC Key
* Arguments    : p_job               :[in]  : Job parameter
*              : p_isd_copy          :[in]  : ISD parameter
*              : p_key_size_in_bytes :[in]  : Key size
*              : p_temp_type         :[out] : Temporary key format
*              : x509_flag           :[in]  : X509 certificate is provided or not
* Return Value : ER_OK
*                ER_PARAM
*                ER_MH_KEY_SRC_MEM_ERROR
***************************************************************************************/
static uint32_t s_03_CheckParamImportEccKey(icum_job_t *p_job, ISD_PK_IMPORT_t *p_isd_copy,
                                            uint32_t *p_key_size_in_bytes,
                                            big_num_type_t *p_temp_type, int8_t x509_flag)
{
    uint32_t            err;
    uint32_t            skip_flg;

    err = ER_OK;
    skip_flg = FLG_OFF;

    /* check the key size  */
    if ((R_CRYPT_SMALLBIGNUM_BITSIZE < (uint32_t)p_isd_copy->point_x_size_in_bits) ||
        (R_CRYPT_SMALLBIGNUM_BITSIZE < (uint32_t)p_isd_copy->point_y_size_in_bits) ||
        (R_CRYPT_SMALLBIGNUM_BITSIZE < (uint32_t)p_isd_copy->priv_scalar_size_in_bits))
    {
        err = ER_PARAM;
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        if ((ECC_CURVE_CURVE25519 == p_isd_copy->curve_id) ||
            (ECC_CURVE_CURVE448 == p_isd_copy->curve_id) ||
            (ECC_CURVE_ED25519 == p_isd_copy->curve_id) ||
            (ECC_CURVE_ED448 == p_isd_copy->curve_id))
        {
            /* curve25519 is only x-coordinate */
            if (NULL == p_isd_copy->p_pub_point_x)
            {
                err = ER_PARAM;
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
        /* end of if ((ECC_CURVE_CURVE25519 == p_isd_copy->curve_id) || ...*/
        else
        {
            if ((NULL == p_isd_copy->p_pub_point_x) ||
                (NULL == p_isd_copy->p_pub_point_y))
            {
                err = ER_PARAM;
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
        /* end of else */
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* Check parameter of ECC public key */
        err = s_03_ImportEccKeyChkKeyPrm(p_job,
                             p_isd_copy,
                             p_key_size_in_bytes,
                             p_temp_type);
        if ((uint32_t)ER_OK == err)
        {
            /* Check parameter of ECC public key */
            err = s_03_ImportEccKeyChkPrm(p_job,
                                          p_isd_copy,
                                          p_key_size_in_bytes,
                                          x509_flag);
            if ((uint32_t)ER_OK == err)
            {
                /* Do nothing */
            }
            else
            {
                skip_flg = FLG_ON;
            }
        }
        /* end of if ((uint32_t)ER_OK == err) */
        else
        {
            skip_flg = FLG_ON;
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0042]:[ICUMFW_UD_S_03_PK_0042]
* Function name: s_03_EccKeyValue
* Description  : Encode of ECC Key value
* Arguments    : p_job          :[in]   job parameter
*                p_isd_copy     :[in]   ISD parameter
*                p_point_x      :[out]  point x
*                p_point_y      :[out]  point y
*                p_priv_scalar  :[out]  private scalar
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
static uint32_t s_03_EncodeEccKeyValue(icum_job_t *p_job, ISD_PK_IMPORT_t *p_isd_copy,
                                       pkc_small_data_t *p_point_x, pkc_small_data_t *p_point_y,
                                       pkc_small_data_t *p_priv_scalar)
{
    uint32_t    err;
    uint32_t    curve_key_size_in_bytes;

    err = ER_OK;
    curve_key_size_in_bytes = NUM0;

    (void)MH_KEY_GetEccKeySize(p_isd_copy->curve_id, &curve_key_size_in_bytes);

    err  = L_BIGNUM_PaddingData(&curve_key_size_in_bytes, p_point_x->size,
                                (uint8_t *)p_point_x->value, NUM0);
    p_point_x->size = curve_key_size_in_bytes;

    err |= L_BIGNUM_PaddingData(&curve_key_size_in_bytes, p_point_y->size,
                                (uint8_t *)p_point_y->value, NUM0);
    p_point_y->size = curve_key_size_in_bytes;

    err |= L_BIGNUM_PaddingData(&curve_key_size_in_bytes, p_priv_scalar->size,
                                (uint8_t *)p_priv_scalar->value, NUM0);
    p_priv_scalar->size = curve_key_size_in_bytes;

    if (ER_OK != err)
    {
        err = ER_PARAM;
    }
    else
    {
        /* do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0043]:[ICUMFW_UD_S_03_PK_0043]
* Function name: s_03_EncodeImportEccKey
* Description  : Encode the Import ECC Key
* Arguments    : p_job                 :[in]   : job parameter
*                p_isd_copy            :[in]   : isd parameter
*                p_key_size_in_bytes   :[in]   : key size
*                temp_type             :[in]   : bignum conversion type
*                p_point_x_big_num     :[out]  : public key (x coordinate)
*                p_point_y_big_num     :[out]  : public key (y coordinate)
*                p_priv_scalar_big_num :[out]  : private key (scalar)
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
static uint32_t s_03_EncodeImportEccKey(icum_job_t *p_job, ISD_PK_IMPORT_t* p_isd_copy,
                                        uint32_t *p_key_size_in_bytes, big_num_type_t temp_type,
                                        pkc_small_data_t *p_point_x_big_num, pkc_small_data_t *p_point_y_big_num,
                                        pkc_small_data_t *p_priv_scalar_big_num)
{
    uint32_t            err;
    remap_ptr_manip_t   pub_point_x_logical;
    remap_ptr_manip_t   pub_point_y_logical;
    remap_ptr_manip_t   priv_scalar_logical;

    err = ER_OK;

    /* Get Remap(Convert Physical address -> Logical Address) */
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_copy->p_pub_point_x,
            &pub_point_x_logical.val);
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_copy->p_pub_point_y,
            &pub_point_y_logical.val);
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_copy->p_priv_scalar,
            &priv_scalar_logical.val);

    /* convert plain key material to big numbers */
    if (FORMAT_STRING_DECIMAL == p_isd_copy->key_format)
    {
        /* get again length of decimal string. */
        p_key_size_in_bytes[NUM0] =
            (uint16_t)L_Toolbox_StrLen((const char *)pub_point_x_logical.p_ptr);
    }
    else
    {
        /* do nothing */
    }
    err  = L_BIGNUM_EncodeToSmallBigNum(pub_point_x_logical.p_ptr,
                                        temp_type,
                                        p_key_size_in_bytes[NUM0],
                                        p_point_x_big_num);

    if ((ECC_CURVE_CURVE25519 != p_isd_copy->curve_id) &&
        (ECC_CURVE_CURVE448 != p_isd_copy->curve_id) &&
        (ECC_CURVE_ED25519 != p_isd_copy->curve_id) &&
        (ECC_CURVE_ED448 != p_isd_copy->curve_id))
    {
        if (FORMAT_STRING_DECIMAL == p_isd_copy->key_format)
        {
            /* get again length of decimal string. */
            p_key_size_in_bytes[NUM1] =
                (uint16_t)L_Toolbox_StrLen((const char *)pub_point_y_logical.p_ptr);
        }
        else
        {
            /* do nothing */
        }
        err |= L_BIGNUM_EncodeToSmallBigNum(pub_point_y_logical.p_ptr,
                                            temp_type,
                                            p_key_size_in_bytes[NUM1],
                                            p_point_y_big_num);
    }
    /* end of if ((ECC_CURVE_CURVE25519 != isd_copy.curve_id) && ...*/
    else
    {
        /* Do nothing */
    }

    if (NUM0 != p_key_size_in_bytes[NUM2])
    {
        if (FORMAT_STRING_DECIMAL == p_isd_copy->key_format)
        {
            /* get again length of decimal string. */
            p_key_size_in_bytes[NUM2] =
                (uint16_t)L_Toolbox_StrLen((const char *)priv_scalar_logical.p_ptr);
        }
        else
        {
            /* Do nothing */
        }
        err |= L_BIGNUM_EncodeToSmallBigNum(priv_scalar_logical.p_ptr,
                                            temp_type,
                                            p_key_size_in_bytes[NUM2],
                                            p_priv_scalar_big_num);
    }
    /* end of if (NUM0 != key_size_in_bytes[NUM2]) */
    else
    {
        /* Do nothing */
    }

    /* error can occur if the size of the data is too big for the internal buffers */
    if (ER_OK != err)
    {
        err = ER_PARAM;
    }
    else
    {
        err = s_03_EncodeEccKeyValue(p_job, p_isd_copy,
                                     p_point_x_big_num, p_point_y_big_num, p_priv_scalar_big_num);
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0009]:[ICUMFW_UD_S_03_PK_0009]
* Function name: s_03_ImportEccKey
* Description  : Main Routine of Import ECC Key
* Arguments    : p_job      :[in]   : job parameter
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_WRITING
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_PROG
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_DEST_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_MH_KEY_WRITE_PROTECTED
*                ER_MH_KEY_INVALID_CURVE_ID
*                ER_ML_CTX_ALL_USE
*                SERV_UNAVAILABLE
*                SERV_KEY_NOT_USABLE
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_SRC_MEM_ERROR
*                SERV_CRYPT_ERROR
*                SERV_KEY_VERIFICATION_FAIL
***************************************************************************************/
static uint32_t s_03_ImportEccKey(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_isd;
    ISD_PK_IMPORT_t     isd_copy;
    big_num_type_t      temp_type;
    uint32_t            key_size_in_bytes[NUM3];
    uint32_t            curve_key_size_in_bytes;
    ecc_key_t           ecc_key;
    uint32_t            err;
    uint32_t            skip_flg;

    uint8_t             x509_signature[R_CRYPT_BIGNUM_BYTESIZE];
    uint8_t             x509_pub_point_x[ECC_KEY_BUFFER_SIZE];
    uint8_t             x509_pub_point_y[ECC_KEY_BUFFER_SIZE];
    int8_t              x509_flag;
    x509_cert_prm_t     x509_cert_prm;

    pkc_small_data_t    point_x_big_num;
    pkc_small_data_t    point_y_big_num;
    pkc_small_data_t    priv_scalar_big_num;

    /* retrieve the service handler */
    p_isd = p_job->p_ISD;
    skip_flg = FLG_OFF;
    x509_flag = (int8_t)X509_OFF;
    curve_key_size_in_bytes = NUM0;

    L_Toolbox_MemSet(x509_signature,                NUM0, (int16_t)sizeof(x509_signature));
    L_Toolbox_MemSet(x509_pub_point_x,              NUM0, (int16_t)sizeof(x509_pub_point_x));
    L_Toolbox_MemSet(x509_pub_point_y,              NUM0, (int16_t)sizeof(x509_pub_point_y));
    /* Once the X509 certificate is provided, the isd parameter will be changed and copied once */
    /* In this case(pk_import) the isd parameter is read-only so there is no problem */
    L_Toolbox_MemCpy((uint8_t *)&isd_copy, (const uint8_t *)&p_isd->prm.PK_IMPORT, (int16_t)sizeof(ISD_PK_IMPORT_t));
    L_Toolbox_MemSet((uint8_t *)&ecc_key,             NUM0, (int16_t)sizeof(ecc_key));
    L_Toolbox_MemSet((uint8_t *)&point_x_big_num,     NUM0, (int16_t)sizeof(point_x_big_num));
    L_Toolbox_MemSet((uint8_t *)&point_y_big_num,     NUM0, (int16_t)sizeof(point_y_big_num));
    L_Toolbox_MemSet((uint8_t *)&priv_scalar_big_num, NUM0, (int16_t)sizeof(priv_scalar_big_num));

    if (PROCESS_DF_WRITE == (uint32_t)p_job->process_phase)
    {
        ecc_key.key_group    = isd_copy.key_group;
        ecc_key.key_id       = isd_copy.key_id;
        err = MH_KEY_SetEccKey(p_job, E_KEY_OPE_CONTINUE, &ecc_key);
        if (ER_OK == err)
        {
            p_job->process_phase = ON_HOLD;
        }
        else
        {
            /* Do nothing */
        }
        skip_flg = FLG_ON;
    }
    /* end of if (PROCESS_DF_WRITE == p_job->process_phase) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        p_job->process_phase = ON_HOLD;
        if (NUM0 == (uint32_t)isd_copy.x509_certificate_size_in_bytes)
        {
            /* check the curve type */
             err = MH_KEY_GetEccKeySize(isd_copy.curve_id, &curve_key_size_in_bytes);
            if (ER_OK != err)
            {
                /* invalid curve type: abort */
                err = ER_PARAM;
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
        /* end of if (NUM0 == isd_copy.x509_certificate_size_in_bytes) */
        else
        {
            x509_cert_prm.p_signature                   = &x509_signature[NUM0];
            x509_cert_prm.sign_size_in_bytes            = sizeof(x509_signature);
            x509_cert_prm.key.ecc.p_pub_point_x         = &x509_pub_point_x[NUM0];
            x509_cert_prm.key.ecc.point_x_size_in_bytes = sizeof(x509_pub_point_x);
            x509_cert_prm.key.ecc.p_pub_point_y         = &x509_pub_point_y[NUM0];
            x509_cert_prm.key.ecc.point_y_size_in_bytes = sizeof(x509_pub_point_y);
            
            err = s_03_ImportEccKeyChkPrmX509(p_job, &isd_copy, &x509_flag, &x509_cert_prm);
            if ((uint32_t)ER_OK != err)
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
        /* end of else */
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        err = s_03_CheckParamImportEccKey(p_job, &isd_copy, key_size_in_bytes, &temp_type, x509_flag);
        if (ER_OK != err)
        {
            skip_flg = FLG_ON;
        }
        else
        {
            /* do nothing */
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        err = s_03_EncodeImportEccKey(p_job, &isd_copy, key_size_in_bytes, temp_type,
                                      &point_x_big_num, &point_y_big_num, &priv_scalar_big_num);
        if (ER_OK != err)
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }

        if (FLG_OFF == skip_flg)
        {
            err = s_03_EncodeEccKeyValue(p_job, &isd_copy,
                                         &point_x_big_num, &point_y_big_num, &priv_scalar_big_num);
            if (ER_OK != err)
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* do nothing */
            }
        }
        /* end of if (FLG_OFF == skip_flg) */
        else
        {
            /* Do nothing */
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* do signature verification */
        err = s_03_ImportEccKeyVerify(p_job,
                                      &isd_copy,
                                      &point_x_big_num,
                                      &point_y_big_num,
                                      &priv_scalar_big_num,
                                      &x509_cert_prm,
                                      x509_flag,
                                      key_size_in_bytes);
        if (ER_OK == err)
        {
            /* at this point, the import operation can actually take place */
            err = s_03_ImportEccKeySetKey(p_job,
                                          &isd_copy,
                                          &point_x_big_num,
                                          &point_y_big_num, 
                                          &priv_scalar_big_num);
        }
        else
        {
            /* Do nothing */
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0007]:[ICUMFW_UD_S_03_PK_0007]
* Function name: S_03_ImportPublicKey
* Description  : Import RSA/ECC keys service (service entry point)
* Arguments    : p_job :[in/out]    : job parameter
* Return Value : void
***************************************************************************************/
void S_03_ImportPublicKey(icum_job_t *p_job)
{
    uint32_t err;
    /* indicate service is in process */
    p_job->p_ISD->service_result = SERV_IN_PROCESS;
    
    switch (p_job->p_ISD->prm.PK_IMPORT.key_group)
    {
        case KEY_GRP_RSA:
        case KEY_GRP_RSA_PUB:
        err = s_03_ImportRsaKey(p_job);
        break;
        case KEY_GRP_ECC:
        case KEY_GRP_ECC_PUB:
        case KEY_GRP_ECC_RAM:
        err = s_03_ImportEccKey(p_job);
        break;
        default:
        err = (uint32_t)SERV_INVALID_KEY_GROUP;
        break;
    }

    if (err != ER_OK)
    {
        /* error: terminate the service */
        (void)s_03_ConvertErr(err, &p_job->p_ISD->service_result);
    }
    else
    {
        /* all good - terminate the service or keep it alive */
        /* depending on the processing phase */
        if (ON_HOLD == (uint32_t)p_job->process_phase)
        {
            p_job->p_ISD->service_result = SERV_OK;
        }
        else
        {
            /* Do nothing */
        }
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0028]:[ICUMFW_UD_S_03_PK_0028]
* Function name: s_03_GetHexStringByteSize
* Description  : Get hex string byte size by bit size
* Arguments    : bit_size           :[in]   : bit size
*              : p_string_byte_size :[out]  : byte size
* Return Value : none
***************************************************************************************/
static void s_03_GetHexStringByteSize(uint32_t bit_size, uint32_t *p_string_byte_size)
{
    *p_string_byte_size = (bit_size + NUM3) / NUM4;
    return;
}
