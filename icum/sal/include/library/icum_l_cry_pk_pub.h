/******************************************************************************/
/* Component Name   Public Key Cryptographic Library                          */
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

#ifndef ICUM_L_CRY_PK_PUB_H
#define ICUM_L_CRY_PK_PUB_H

#include "renesas_types.h"
#include "icum_common_pub.h"
#include "r_icumif_api.h"
#include "r_pkc_api.h"
#include "r_pkcl_api.h"
#include "r_pkcd_dev_api.h"

/********************************************************************************
 Macro definitions
********************************************************************************/
#define ER_L_BIGNUM_INVALID_ARG         (ER_L_CRY_BIGNUM_BASE | (0x00000001u))
#define ER_L_BIGNUM_BUFFER_OVERFLOW     (ER_L_CRY_BIGNUM_BASE | (0x00000002u))
#define ER_L_BIGNUM_VAL                 (ER_L_CRY_BIGNUM_BASE | (0x00000003u))

#define ER_L_RSA_INVALID_ARG            (ER_L_CRY_RSA_BASE    | (0x00000001u))
#define ER_L_RSA_OVERFLOW               (ER_L_CRY_RSA_BASE    | (0x00000002u))
#define ER_L_RSA_INVALID_SIZE           (ER_L_CRY_RSA_BASE    | (0x00000003u))
#define ER_L_RSA_INVALID_PACKET         (ER_L_CRY_RSA_BASE    | (0x00000004u))
#define ER_L_RSA_KEYGEN_RETRYOVER       (ER_L_CRY_RSA_BASE    | (0x00000005u))
#define ER_L_RSA_INVALID_PARM           (ER_L_CRY_RSA_BASE    | (0x00000006u))
#define ER_L_RSA_VERIFY_FAILED          (ER_L_CRY_RSA_BASE    | (0x00000007u))
#define ER_L_RSA_ERROR                  (ER_L_CRY_RSA_BASE    | (0x00000010u))
#define ER_L_RSA_RUNNING                (ER_L_CRY_RSA_BASE    | (0x00000020u))
#define ER_L_RSA_PROGRESS               (ER_L_CRY_RSA_BASE    | (0x00000040u))
#define ER_L_RSA_LENGTHERR              (ER_L_CRY_RSA_BASE    | (0x00000080u))
#define ER_L_RSA_SYS_ERROR              (ER_L_CRY_RSA_BASE    | (0x00000090u))

#define ER_L_HASH_INVALID_ARG           (ER_L_CRY_HASH_BASE   | (0x00000001u))

#define ER_L_HMAC_INVALID_ARG           (ER_L_CRY_HMAC_BASE   | (0x00000001u))
#define ER_L_HMAC_BUFFER_OVERFLOW       (ER_L_CRY_HMAC_BASE   | (0x00000002u))
#define ER_L_HMAC_INVALID_KEYSIZE       (ER_L_CRY_HMAC_BASE   | (0x00000003u))

#define ER_L_X509_INVALID_ARG           (ER_L_CRY_X_509_BASE  | (0x00000001u))
#define ER_L_X509_BUFFER_OVERFLOW       (ER_L_CRY_X_509_BASE  | (0x00000002u))
#define ER_L_X509_KEYTYPE_UNKOWN        (ER_L_CRY_X_509_BASE  | (0x00000003u))
#define ER_L_X509_INVALID_CURVE         (ER_L_CRY_X_509_BASE  | (0x00000004u))

#define ER_L_ECC_INVALID_ARG            (ER_L_CRY_ECC_BASE    | (0x00000001u))
#define ER_L_ECC_OVERFLOW               (ER_L_CRY_ECC_BASE    | (0x00000002u))
#define ER_L_ECC_NOT_SUPPORT            (ER_L_CRY_ECC_BASE    | (0x00000003u))
#define ER_L_ECC_VERIFY_FAILED          (ER_L_CRY_ECC_BASE    | (0x00000007u))
#define ER_L_ECC_ERROR                  (ER_L_CRY_ECC_BASE    | (0x00000010u))
#define ER_L_ECC_RUNNING                (ER_L_CRY_ECC_BASE    | (0x00000020u))
#define ER_L_ECC_PROGRESS               (ER_L_CRY_ECC_BASE    | (0x00000040u))
#define ER_L_ECC_LENGTHERR              (ER_L_CRY_ECC_BASE    | (0x00000080u))
#define ER_L_ECC_SYS_ERROR              (ER_L_CRY_ECC_BASE    | (0x00000090u))

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
#define FLG_OFF (0UL)
#define FLG_ON  (1UL)

/*================================================================================*/
/* Internal number representation (big numbers)                                   */
/*================================================================================*/
/* the symbols R_CRYPT_BIGNUM_BITSIZE and R_CRYPT_SMALLBIGNUM_BITSIZE             */
/* are defined externally (e.g. in the top project structure)                     */
/*================================================================================*/
#define R_CRYPT_BIGNUM_BITSIZE           (4096u)
#define R_CRYPT_BIGNUM_BYTESIZE          (R_CRYPT_BIGNUM_BITSIZE / 8u)
#define R_CRYPT_SMALLBIGNUM_BITSIZE      (544u)
#define R_CRYPT_SMALLBIGNUM_BYTESIZE     (R_CRYPT_SMALLBIGNUM_BITSIZE / 8u)

#define BIGNUM_LONG_SIZE         ((R_CRYPT_BIGNUM_BITSIZE / 16u) + 8u)

#if   ((R_CRYPT_SMALLBIGNUM_BITSIZE % 16u) == 0u)
    #define SMALLBIGNUM_LONG_SIZE    ((R_CRYPT_SMALLBIGNUM_BITSIZE / 16u) + 8u)
#else
    #define SMALLBIGNUM_LONG_SIZE    ((R_CRYPT_SMALLBIGNUM_BITSIZE / 16u) + 9u)
#endif

/*================================================================================*/
/* bit length of RSA private key                                                  */
/*================================================================================*/
/* minimum size of RSA private keys */
#define MIN_BIT_LEN_RSA_PRIV_KEY    (1024u)

/* maximum size of RSA private keys */
#define MAX_BIT_LEN_RSA_PRIV_KEY    (4096u)

/* RSA calculation processing type */
#define RSA_PROC_SYNC               (0u)
#define RSA_PROC_ASYNC              (1u)

/* result of signature verification */
#define VERIFY_NG                   (0)
#define VERIFY_OK                   (1)

/*================================================================================*/
/* max rsa signature byte size                                                    */
/*================================================================================*/
#define RSA_SIGNATURE_MAXSIZE_BIN   (R_CRYPT_BIGNUM_BYTESIZE)
#define RSA_SIGNATURE_MAXSIZE_HEX   (R_CRYPT_BIGNUM_BYTESIZE * 2u)
/* decimal size is bitsize * log(2) */
#define RSA_SIGNATURE_MAXSIZE_DEC   (((R_CRYPT_BIGNUM_BITSIZE * 30103u) + 99999u) / 100000u)

/*================================================================================*/
/* hash byte size                                                                 */
/*================================================================================*/
#define HASH_BYTE_SIZE_224          (28u)
#define HASH_BYTE_SIZE_256          (32u)
#define HASH_BYTE_SIZE_384          (48u)
#define HASH_BYTE_SIZE_512          (64u)

/* Defines the maximum value of processing bits */
#define L_PKC_PROCESSING_BITS_MAX  (0xFFFFu)

/**********************************************************************************
* Ensure the structure alignment
***********************************************************************************/
#pragma pack(8)

/**********************************************************************************
* Type definition
***********************************************************************************/
typedef struct
{
    uint32_t   size;
    uint32_t   value[BIGNUM_LONG_SIZE];
} pkc_data_t;

typedef struct
{
    uint32_t   size;
    uint32_t   value[SMALLBIGNUM_LONG_SIZE];
} pkc_small_data_t;

typedef uint32_t (p_func_get_res) (uint32_t resource_type, uint32_t *p_resource_id);
typedef uint32_t (p_func_free_res) (uint32_t resource_type, uint32_t resource_id);
typedef uint32_t (p_func_generate) (uint32_t *p_prng_DataOut);
typedef struct
{
    uint32_t            err;
    uint32_t            res_id;
    p_func_get_res      *p_get;
    p_func_free_res     *p_free;
    p_func_generate     *p_generate;
} pkc_prng_func_t;

/*================================================================================*/
/* Big number types for public key cryptography                                   */
/*================================================================================*/
/* big_num_type_t defines how the big number value is represented                 */
/* Note: whatever the data representation, the numbers are                        */
/*       always read with the MSB first                                           */
/*                                                                                */
/* - STRING_DECIMAL : a string of character (terminated by the null character)    */
/*                    that represents the value in decimal form                   */
/*                    -> one byte codes the ASCII representation of a decimal     */
/*                       number (0 to 9)                                          */
/*                    example: "1328"                                             */
/* - STRING_HEXA    : a string of character (terminated by the null character)    */
/*                    that represents the value in hexadecimal form               */
/*                    -> one byte codes the ASCII representation of an hexadecimal*/
/*                       number (0x0 to 0xF)                                      */
/*                    example: "EF200"                                            */
/* - BINARY_PLAIN   : a table of bytes                                            */
/*                    -> one byte codes a value between 0 and 255                 */
/*================================================================================*/

typedef enum
{
    E_STRING_DECIMAL,
    E_STRING_HEXA,
    E_BINARY_PLAIN,

    E_LAST_ITEM_BIGNUM_TYPE = 0x01000000
} big_num_type_t;

/*================================================================================*/
/* Hash types                                                                     */
/*================================================================================*/

typedef enum
{
    E_SHA2_224,
    E_SHA2_256,
    E_SHA2_384,
    E_SHA2_512,

    E_SHA2_256_SLOW_RAM,

    E_SHA2_512_224 = 5,
    E_SHA2_512_256,

    E_SHA3_256 = 8,
    E_SHA3_384,
    E_SHA3_512,
    E_SHAKE128,
    E_SHAKE256,

    E_LAST_ITEM_HASH_TYPE = 0x01000000
} hash_type_t;


typedef struct
{
    uint64_t    length;
    uint64_t    state[8];
    uint32_t    current_length;
    uint8_t     buffer[272];
} hash_state_t;

/* Forward declaration */
union hash_inner_state;
typedef union hash_inner_state hash_inner_state_t;

typedef uint32_t (*p_func_hash_init_t)      (hash_inner_state_t *hash_state);
typedef uint32_t (*p_func_hash_process_t)   (hash_inner_state_t *hash_state, const uint8_t *p_message, uint32_t size_in_bytes);
typedef uint32_t (*p_func_hash_terminate_t) (hash_inner_state_t *hash_state, uint8_t *p_result);


typedef struct
{
    uint8_t     nb_words;
    uint32_t    words[16];
} oid_hash_t;

typedef struct
{
    uint32_t                hash_bitlen;
    uint32_t                blocksize;
    const oid_hash_t       *p_oid;
    p_func_hash_init_t      p_hash_init;
    p_func_hash_process_t   p_hash_process;
    p_func_hash_terminate_t p_hash_terminate;
} hash_descriptor_t;

typedef struct
{
    uint32_t total_msg_size_in_bytes;
    uint32_t use_dma_transfer;
} hash_ope_prm_t;

/*================================================================================*/
/* HMAC types                                                                     */
/*================================================================================*/

typedef struct hmac_state {
    hash_type_t         hash_type;
    uint8_t             key[128];
    uint32_t            keylen;
} hmac_state_t;


typedef struct hash_ctx
{
    hash_type_t         hash_type;
    hash_descriptor_t   hash_desc;
    hash_state_t        hash_state;
    hmac_state_t        hmac_state;
} hash_ctx_t;

/*================================================================================*/
/* KDF types                                                                      */
/*================================================================================*/
typedef enum
{
    E_KDF1,
    E_KDF2,
    E_KDF3,
    E_PKDF1,
    E_PKDF2,
    E_CONCAT_KDF,
    
    E_LAST_ITEM_KDF_TYPE = 0x01000000
} kdf_type_t;

/*===========================================================================
  Type definition: key derivation function parameters
  Notes:
  - shared secret and information should be defined for KDFn functions
  - password and salt should be defined for PBKDFn functions
  - counter should be defined for PBKDFn functions
  - pAmt should be defined for KDF3 only
  - define password or shared secret (no simultaneous definition)
  - define salt or information (no simultaneous definition)
  - define counter or pAmt (no simultaneous definition)
  ===========================================================================*/
typedef struct kdf_data_set
{
    uint8_t    *p_data;
    uint32_t    size_in_bytes;
} kdf_data_set_t;

typedef struct kdf
{
    kdf_type_t      kdf_algo;
    hash_type_t     hash_type;
    union
    {
        kdf_data_set_t  shared_secret;
        kdf_data_set_t  pwd;
    } secret;
    union
    {
        kdf_data_set_t  info;
        kdf_data_set_t  salt;
    } addition;
    union
    {
        uint16_t        counter;
        uint16_t        pAmt;
    } count;
    uint8_t         *p_deriv_key;
    uint8_t         deriv_key_size_in_bytes;
} kdf_t;

/*================================================================================*/
/* RSA types                                                                      */
/*================================================================================*/

typedef struct rsa_ctx
{
    pkc_data_t  W;
    uint32_t    tmp_exp;
    uint8_t     tls_buffer[48];
    st_pkc_rsa_ctx_t pkc_ctx;
    st_pkc_rsa_mask_opt_t mask_option;
    st_pkc_hash_t hash_info;
} rsa_ctx_t;

/*================================================================================*/
/* ECC types                                                                      */
/*================================================================================*/

/* Affine point coordinates on an ECC curve */
/* - x : the x coordinate of the point */
/* - y : the y coordinate of the point */
typedef struct ecc_point_affine
{
    pkc_small_data_t    x;
    pkc_small_data_t    y;
} ecc_point_affine_t;

typedef struct edwards_ctx
{
    uint8_t     prv_key_buf[60];
    uint8_t     pub_key_buf[60];
    uint8_t     rs_buff[128];
    uint8_t     sign_r[60];
    uint8_t     sign_s[60];
    uint8_t     context[255];
} edwards_ctx_t;

typedef struct ecc_ctx
{
    r_ecc_curve_t curve_id;
    union
    {
        st_pkc_ec_ctx_t     ec;    /* ecc */
        st_pkc_ecdhx_ctx_t  ecdhx; /* ecdhx */
        st_pkc_eddsa_ctx_t  eddsa; /* edwards */
    } ctx;
    edwards_ctx_t           edwards;
    st_pkc_data_t           ed_ctx;
    st_pkc_hash_t           hash_info;
    st_pkc_kdf_t            kdf_info;
    kdf_t                   kdf_ctx;
    st_pkcd_ec_key_bin_t    ecc_key;
    st_pkcd_ec_key_bin_t    scalar_bin;
    st_pkcd_ec_key_bin_t    point_bin;
    st_pkcd_eddsa_key_bin_t eddsa_priv_key;
    st_pkcd_eddsa_key_bin_t eddsa_pub_key;
} ecc_ctx_t;

/*===========================================================================
    Type definition: X.509
  ===========================================================================*/
typedef enum
{
    E_KEY_TYPE_UNKNOWN,
    E_KEY_TYPE_RSA_PUB_KEY,
    E_KEY_TYPE_ECC_PUB_KEY
} key_type_t;

typedef struct x509_rsa_key
{
    uint8_t         *p_modulus;
    uint16_t         modulus_size_in_bytes;
    uint8_t         *p_pub_exp;
    uint16_t         pub_exp_size_in_bytes;
} x509_rsa_key_t;

typedef struct x509_ecc_key
{
    uint8_t         *p_pub_point_x;
    uint16_t         point_x_size_in_bytes;
    uint8_t         *p_pub_point_y;
    uint16_t         point_y_size_in_bytes;
} x509_ecc_key_t;

typedef struct x509_cert_prm
{
    const   uint8_t         *p_cert;                /* (in)  x.509 certificate     */
            uint16_t        cert_size_in_bytes;     /* (in)  certificate byte size */
            uint8_t         *p_signature;           /* (out) signature             */
            uint16_t        sign_size_in_bytes;     /* (out) signature size        */
            key_type_t      key_type;               /* (out) key type              */
            union
            {
                x509_rsa_key_t  rsa;
                x509_ecc_key_t  ecc;
            } key;
            r_ecc_curve_t  curve_id;
            uint8_t         *p_signed_data;
            uint16_t        signed_data_size_in_bytes;
} x509_cert_prm_t;

typedef struct
{
    r_kdf_algo_t        kdf_algo;
    r_hash_primitive_t  hash_primitive;
    uint8_t             *p_secret_key_value;
    uint32_t            secret_key_size_in_bytes;
    uint8_t             *p_info;
    uint32_t            info_size_in_bytes;
    uint16_t            counter;
    uint8_t             *p_deriv_key_value;
    uint8_t             deriv_key_size_in_bytes;
} r_kdf_t;

/**********************************************************************************
* Prototypes
***********************************************************************************/

/*================================================================================*/
/* Module identification and information                                          */
/*================================================================================*/

/*================================================================================*/
/* Hash functions                                                                 */
/*================================================================================*/

uint32_t L_HASH_Init(                hash_ctx_t      *p_hash_ctx,
                                     hash_type_t      hash_type);

uint32_t L_HASH_Update(              hash_ctx_t      *p_hash_ctx,
                               const uint8_t         *p_data,
                                     uint32_t         size_in_bytes);

uint32_t L_HASH_Finish(              hash_ctx_t      *p_hash_ctx,
                                     uint8_t         *p_result);

#if defined(FSW_DVC_HASH)
uint32_t L_HASH_CalculateDVC(  const uint8_t         *p_key,
                               const uint8_t         *p_data,
                                     uint32_t         data_size,
                                     uint8_t         *p_result);
#endif /* defined(FSW_DVC_HASH) */

uint32_t L_HASH_SetOpePrm(           hash_ctx_t      *p_hash_ctx,
                                     hash_ope_prm_t  *const p_prm);

uint32_t L_HASH_ChkDMAApplicable(    hash_type_t      hash_type,
                               const uint8_t         *p_input_area,
                                     uint32_t         input_size_in_bytes,
                                     uint8_t         *p_output_area,
                                     uint32_t         output_size_in_bytes);

/*================================================================================*/
/* HMAC functions                                                                 */
/*================================================================================*/
uint32_t L_HMAC_Init(       hash_ctx_t      *p_hash_ctx,
                      const uint8_t         *p_key,
                            uint32_t         keylen);

uint32_t L_HMAC_Update(     hash_ctx_t      *p_hash_ctx,
                      const uint8_t         *p_data,
                            uint32_t         size_in_bytes);

uint32_t L_HMAC_Finish(     hash_ctx_t      *p_hash_ctx,
                            uint8_t         *p_hmac,
                            uint32_t        *p_outlen);

/*================================================================================*/
/* Big number encoding / decoding functions                                       */
/*================================================================================*/
uint32_t L_BIGNUM_EncodeToBigNum(        const  void            *p_value,
                                                big_num_type_t   value_type,
                                                uint32_t         size_in_bytes,
                                                pkc_data_t      *p_big_num);

uint32_t L_BIGNUM_EncodeToSmallBigNum(   const  void             *p_value,
                                                big_num_type_t    value_type,
                                                uint32_t          size_in_bytes,
                                                pkc_small_data_t *p_big_num);

uint32_t L_BIGNUM_DecodeFromBigNum(             pkc_data_t       *p_big_num,
                                                big_num_type_t    value_type,
                                                void             *p_value,
                                                uint32_t         *p_size_in_bytes);

uint32_t L_BIGNUM_DecodeFromSmallBigNum(        pkc_small_data_t *p_big_num,
                                                big_num_type_t   value_type,
                                                void             *p_value,
                                                uint32_t         *p_size_in_bytes);
/* only for STRING_HEXA or STRING_DECIMAL */
uint32_t L_BIGNUM_DecodeFromBigNumSlice(        pkc_data_t       *p_big_num,
                                                big_num_type_t   value_type,
                                                void             *p_value,
                                                uint32_t         str_index);
uint32_t L_BIGNUM_CountBits (const pkc_data_t *p_a, uint32_t *p_bit_size);

uint32_t L_BIGNUM_PaddingData(uint32_t *p_size_in_bytes, uint32_t size_in_bytes,
                              uint8_t *p_value, uint8_t zero);

/*================================================================================*/
/* KDF functions                                                                  */
/*================================================================================*/

uint32_t L_ECC_Kdf(kdf_t *p_kdf);

r_icumif_service_result_t CISM_DerivKeyValue(r_kdf_t *p_kdf);

/*================================================================================*/
/* PKC Library functions                                                          */
/*================================================================================*/

uint32_t L_PKC_Init(const pkc_prng_func_t * const p_func);
uint32_t L_PKC_RsaChkPkccBusy(void);
uint32_t L_PKC_EccChkPkccBusy(void);
uint32_t L_PKC_RsaContextSetup(rsa_ctx_t *p_rsa_ctx);
uint32_t L_PKC_RsaCryptSetup(rsa_ctx_t *p_rsa_ctx, r_message_encoding_format_t format,
                             hash_ctx_t *p_hash_ctx, r_hash_primitive_t primitive);
uint32_t L_PKC_RsaSignSetup(rsa_ctx_t *p_rsa_ctx, r_hash_encoding_format_t format,
                            hash_ctx_t *p_hash_ctx, r_hash_primitive_t primitive);
uint32_t L_PKC_RsaEncryptInit(const st_pkc_data_t* const p_plain,const void* const p_pub_key,
                              const uint32_t mod_len, st_pkc_rsa_ctx_t* const p_ctx);
uint32_t L_PKC_RsaEncryptOperate(st_pkc_rsa_ctx_t* const p_ctx, const st_pkc_operate_opt_t* const p_operate_opt);
uint32_t L_PKC_RsaEncryptFinal(const st_pkc_rsa_ctx_t* const p_ctx, st_pkc_data_t* const p_cipher);
uint32_t L_PKC_RsaDecryptInit(const st_pkc_data_t* const p_cipher, const void* const p_priv_key,
                              const uint32_t mod_len, st_pkc_rsa_ctx_t* const p_ctx);
uint32_t L_PKC_RsaDecryptOperate(st_pkc_rsa_ctx_t* const p_ctx, const st_pkc_operate_opt_t* const p_operate_opt);
uint32_t L_PKC_RsaDecryptFinal(const st_pkc_rsa_ctx_t* const p_ctx, st_pkc_data_t* const p_plain);
uint32_t L_PKC_RsaSignInit(const st_pkc_data_t* const p_digest, const void* const p_priv_key,
                           const uint32_t mod_len, st_pkc_rsa_ctx_t* const p_ctx);
uint32_t L_PKC_RsaSignOperate(st_pkc_rsa_ctx_t* const p_ctx, const st_pkc_operate_opt_t* const p_rsa_operate_opt);
uint32_t L_PKC_RsaSignFinal(const st_pkc_rsa_ctx_t* const p_ctx, st_pkc_data_t*  const p_sign);
uint32_t L_PKC_RsaVerifyInit(const st_pkc_data_t * const p_digest, const st_pkc_data_t * const p_sign,
                             const void * const p_pub_key, const uint32_t mod_len, st_pkc_rsa_ctx_t * const p_ctx);
uint32_t L_PKC_RsaVerifyOperate(st_pkc_rsa_ctx_t* const p_ctx, const st_pkc_operate_opt_t* const p_operate_opt);
uint32_t L_PKC_RsaVerifyFinal(const st_pkc_rsa_ctx_t* const p_ctx);
uint32_t L_PKC_RsaKeyGenInit(st_pkc_rsa_ctx_t* const  p_ctx, const uint32_t key_len,
                             const uint32_t prime_test_num, const uint32_t iteration_num);
uint32_t L_PKC_RsaKeyGenOperate(st_pkc_rsa_ctx_t* const p_ctx);
uint32_t L_PKC_RsaKeyGenFinal(const st_pkc_rsa_ctx_t* const p_ctx, void* const p_priv_key, void* const p_pub_key);
uint32_t L_PKC_LLEcScalarMultInit(const st_pkc_select_data_t* const p_scalar_or_key,
                                  const st_pkc_select_data_t* const p_point_or_key,
                                  st_pkc_ec_ctx_t* const p_ctx);
uint32_t L_PKC_LLEcScalarMultOperate(st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt);
uint32_t L_PKC_LLEcScalarMultFinal(const st_pkc_ec_ctx_t* const p_ctx, st_pkc_data_t* const p_point_out);
uint32_t L_PKC_EcdhSetup(ecc_ctx_t *p_ecc_ctx, st_pkc_kdf_t *p_kdf_info);
uint32_t L_PKC_EcdhInit(const void* const p_priv_key, const void* const p_pub_key, st_pkc_ec_ctx_t* const p_ctx);
uint32_t L_PKC_EcdhOperate(st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt);
uint32_t L_PKC_EcdhFinal(const st_pkc_ec_ctx_t* const p_ctx, st_pkc_data_t* const p_shared);
uint32_t L_PKC_EcdhXSetup(ecc_ctx_t *p_ecc_ctx, st_pkc_kdf_t *p_kdf_info);
uint32_t L_PKC_EcdhXInit(const void* const p_priv_key, const void* const p_pub_key, st_pkc_ecdhx_ctx_t* const p_ctx);
uint32_t L_PKC_EcdhXOperate(st_pkc_ecdhx_ctx_t* const p_ctx, const uint32_t processing_bits);
uint32_t L_PKC_EcdhXFinal(const st_pkc_ecdhx_ctx_t* const p_ctx, st_pkc_data_t* const p_shared);
uint32_t L_PKC_EccSetup(ecc_ctx_t *p_ecc_ctx);
uint32_t L_PKC_EcdsaSignInit(const st_pkc_data_t* const p_digest, const void* const p_priv_key,
                             st_pkc_ec_ctx_t* const p_ctx);
uint32_t L_PKC_EcdsaSignOperate(st_pkc_ec_ctx_t* const p_ctx, e_pkc_sync_opt_t sync_opt);
uint32_t L_PKC_EcdsaSignFinal(const st_pkc_ec_ctx_t* const p_ctx, st_pkc_data_t* const p_sign);
uint32_t L_PKC_EcdsaVerifyInit(const st_pkc_data_t* const p_digest, const st_pkc_data_t* const p_sign,
                               const void* const p_pub_key, st_pkc_ec_ctx_t* const p_ctx);
uint32_t L_PKC_EcdsaVerifyOperate(st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt);
uint32_t L_PKC_EcdsaVerifyFinal(const st_pkc_ec_ctx_t* const p_ctx);
uint32_t L_PKC_EcKeyGenInit(st_pkc_ec_ctx_t* const p_ctx);
uint32_t L_PKC_EcKeyGenOperate(st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt);
uint32_t L_PKC_EcKeyGenFinal(const st_pkc_ec_ctx_t* const p_ctx, void* const p_priv_key, void* const p_pub_key);
uint32_t L_PKC_EcdhXKeyGenInit(st_pkc_ecdhx_ctx_t* const p_ctx);
uint32_t L_PKC_EcdhXKeyGenOperate(st_pkc_ecdhx_ctx_t* const p_ctx, const uint32_t processing_bits);
uint32_t L_PKC_EcdhXKeyGenFinal(const st_pkc_ecdhx_ctx_t* const p_ctx,
                                void* const p_priv_key,
                                void* const p_pub_key);
uint32_t L_PKC_EddsaSetup(ecc_ctx_t *p_ecc_ctx, hash_ctx_t *p_hash_ctx, r_hash_primitive_t hash_primitive);
uint32_t L_PKC_EddsaSignInit(const st_pkc_data_t* const p_msg, const void* const p_priv_key,
                             const void* const p_pub_key, st_pkc_eddsa_ctx_t* const p_ctx);
uint32_t L_PKC_EddsaSignOperate(st_pkc_eddsa_ctx_t* const p_ctx, const uint32_t processing_bits);
uint32_t L_PKC_EddsaSignFinal(const st_pkc_eddsa_ctx_t* const p_ctx, st_pkc_data_t* const p_sign);
uint32_t L_PKC_EddsaVerifyInit(const st_pkc_data_t* const p_msg, const st_pkc_data_t* const p_sign,
                               const void* const p_pub_key, st_pkc_eddsa_ctx_t* const p_ctx);
uint32_t L_PKC_EddsaVerifyOperate(st_pkc_eddsa_ctx_t* const p_ctx, const uint32_t processing_bits);
uint32_t L_PKC_EddsaVerifyFinal(const st_pkc_eddsa_ctx_t* const p_ctx);
uint32_t L_PKC_EddsaKeyGenInit(st_pkc_eddsa_ctx_t* const p_ctx);
uint32_t L_PKC_EddsaKeyGenOperate(st_pkc_eddsa_ctx_t* const p_ctx, const uint32_t processing_bits);
uint32_t L_PKC_EddsaKeyGenFinal(const st_pkc_eddsa_ctx_t* const p_ctx,
                                void* const p_priv_key, void* const p_pub_key);

/*================================================================================*/
/* X.509 functions                                                                */
/*================================================================================*/
uint32_t L_X509_ParseCertificate(x509_cert_prm_t *p_x509_cert_prm);

/*================================================================================*/
/* Utility functions                                                              */
/*================================================================================*/
uint32_t L_ECC_ToInternalKdfType(r_kdf_algo_t kdf_algo, kdf_type_t *kdf_type);
uint32_t L_HASH_GetInternalHashType(r_hash_primitive_t hash_primitive, hash_type_t *p_hash_type,
                                    uint16_t *p_hash_size_in_bytes);

/**********************************************************************************
* Structure alignment back to default configuration
***********************************************************************************/

#pragma pack()

#endif /* ICUM_L_CRY_PK_PUB_H */
