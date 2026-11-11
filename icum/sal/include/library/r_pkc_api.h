/******************************************************************************/
/* Component Name   PKC Library / PKC Driver share header file                */
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

#ifndef R_PKC_API_H
/* Multiple inclusion protection macro */
#define R_PKC_API_H

/* PRQA S 3630 ++ # D016 For use in upper program. */
/* PRQA S 5004 ++ # D018 No problem because it is a function pointer */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "renesas_types.h"

/*!********************************************************************
 * \addtogroup    PKCCommon
 * \{
 * \addtogroup    PKCCommonTypes               Common Types
 * \}******************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \addtogroup  PKCCommonTypes               Common Types
 * \{******************************************************************/
/**********************************************************************************************************************
* Function Name : PKC_HASH_INIT_T
**********************************************************************************************************************/
/** HASH INIT function pointer type */
typedef uint32_t (*PKC_HASH_INIT_T)(void * const p_hash_ctx);

/**********************************************************************************************************************
* Function Name : PKC_HASH_UPDATE_T
**********************************************************************************************************************/
/** HASH UPDATE function pointer type */
typedef uint32_t (*PKC_HASH_UPDATE_T)(void * const p_hash_ctx,
                                    const uint8_t * const p_data,
                                    const uint32_t data_len);

/**********************************************************************************************************************
* Function Name : PKC_HASH_FINAL_T
**********************************************************************************************************************/
/** HASH FINAL function pointer type */
typedef uint32_t (*PKC_HASH_FINAL_T)(void * const p_hash_ctx,
                                    uint8_t * const p_md,
                                    uint32_t * const p_md_len);

/**********************************************************************************************************************
* Function Name : PKC_DRBG_T
**********************************************************************************************************************/
/** DRBG function pointer type */
typedef uint32_t (*PKC_DRBG_T)(const uint32_t rand_len,
                                uint8_t * const p_random);

/**********************************************************************************************************************
* Function Name : PKC_KDF_T
**********************************************************************************************************************/
/** KDF function pointer type */
typedef uint32_t (*PKC_KDF_T)(void * const p_kdf_ctx,
                            const uint8_t * const p_shared_secret,
                            const uint32_t shared_secret_len,
                            const uint32_t out_len,
                            uint8_t * const p_out_key);

/**********************************************************************************************************************
* Function Name : PKC_CALLBACK_T
**********************************************************************************************************************/
/** General callback function pointer type */
typedef void (*PKC_CALLBACK_T)(void);

/** Synchronous / asynchronous execution option of target operation */
typedef enum
{
    PKC_SYNC_OPT_SYNC  = 0,                         /**< Synchronous processing */
    PKC_SYNC_OPT_ASYNC                              /**< Asynchronous processing */
} e_pkc_sync_opt_t;

/** Boolean type */
typedef enum
{
    PKC_BOOL_TRUE  = 0x555,                        /**< True value */
    PKC_BOOL_FALSE = 0xAAA                         /**< False value */
} e_pkc_bool_t;

/** Hash type */
typedef enum
{
    PKC_HASH_TYPE_SHA1 = 0,                         /**< SHA1 */
    PKC_HASH_TYPE_SHA2_224,                         /**< SHA2_224 */
    PKC_HASH_TYPE_SHA2_256,                         /**< SHA2_256 */
    PKC_HASH_TYPE_SHA2_384,                         /**< SHA2_384 */
    PKC_HASH_TYPE_SHA2_512,                         /**< SHA2_512 */
    PKC_HASH_TYPE_SHA2_512_224,                     /**< SHA2_512_224 */
    PKC_HASH_TYPE_SHA2_512_256,                     /**< SHA2_512_256 */
    PKC_HASH_TYPE_SHA3_224,                         /**< SHA3_224 */
    PKC_HASH_TYPE_SHA3_256,                         /**< SHA3_256 */
    PKC_HASH_TYPE_SHA3_384,                         /**< SHA3_384 */
    PKC_HASH_TYPE_SHA3_512,                         /**< SHA3_512 */
} e_pkc_hash_type_t;

/** RSA padding scheme */
typedef enum
{
    PKC_RSA_PADDING_NO = 0,                         /**< No padding */
    PKC_RSA_PADDING_PKCS1,                          /**< PKCS#1 V1.5 */
    PKC_RSA_PADDING_PKCS1_OAEP,                     /**< RSAES-OEAP */
    PKC_RSA_PADDING_PKCS1_PSS                       /**< RSASSA-PSS */
} e_pkc_rsa_padding_t;

/** Selective type indicating whether the data is in byte array or key binary format */
typedef enum
{
    PKC_DATA_TYPE_BYTE = 0,                      /**< DataType Byte array */
    PKC_DATA_TYPE_KEY_BINARY                     /**< DataType Key binary */
} e_pkc_data_type_t;

/** Flag values for ed25519ctx/ph and ed448ph */
typedef enum
{
    PKC_EDDSA_VARIANT_PURE = 0,                  /**< EdDSA variant of ed25519 and ed448 */
    PKC_EDDSA_VARIANT_HASH,                      /**< EdDSA variant of ed25519ph and ed448ph */
    PKC_EDDSA_VARIANT_CTX                        /**< EdDSA variant of ed25519ctx */
} e_pkc_eddsa_variant_t;

/** A structure that specifies the data and its size */
typedef struct
{
    uint32_t    len;                                /**< Data byte length */
    uint8_t *   p_data;                             /**< Start address of data */
} st_pkc_data_t;

/** Options specified by the operate function \n
    Whether to set sync_opt or processing_bits depends on the condition in the operate function. */
typedef struct
{
    e_pkc_sync_opt_t    sync_opt;                   /**< Synchronous / asynchronous execution options */
    uint32_t            processing_bits;            /**< Bit length processed per operation when executing an operation
                                                         in multiple times with PKCC */
} st_pkc_operate_opt_t;

/** A structure that summarizes information about Hash */
typedef struct
{
    PKC_HASH_INIT_T     p_hash_init_api;            /**< Hash Init function pointer */
    PKC_HASH_UPDATE_T   p_hash_update_api;          /**< Hash Update function pointer */
    PKC_HASH_FINAL_T    p_hash_final_api;           /**< Hash Final function pointer */
    void *              p_hash_ctx;                 /**< Hash information context */
} st_pkc_hash_t;

/** A structure that summarizes information about KDF */
typedef struct
{
    uint32_t    out_kdf_len;                        /**< KDF output size */
    PKC_KDF_T   p_kdf_api;                          /**< KDF function pointer */
    void *      p_kdf_ctx;                          /**< KDF information context */
} st_pkc_kdf_t;

/** Mask generation options */
typedef struct
{
    uint32_t            salt_len;                   /**< Salt length */
    e_pkc_hash_type_t   mask_type;                  /**< Types of Hash functions used to generate masks */
    st_pkc_hash_t *     p_hash_info;                /**< Hash information */
} st_pkc_rsa_mask_opt_t;

/** Context used by RSA API */
typedef struct
{
    uint8_t *               p_buf;                   /**< A buffer for storing data during calculation */
    e_pkc_bool_t            is_constant_time_mode;   /**< Select whether to use constant time mode when calculating
                                                          exponentiation */
    e_pkc_hash_type_t       msg_digest_type;         /**< Types of HASH functions used to generate signature generation
                                                          or signature verification message digests */
    st_pkc_rsa_mask_opt_t * p_mask_option;           /**< Information on mask generation function required when using
                                                          OAEP and PSS */
    st_pkc_data_t           label;                   /**< A label for OAEP encoding */
    PKC_DRBG_T              p_drbg_api;              /**< DRBG function pointer */
    e_pkc_bool_t            is_crt_mode;             /**< Choice of whether to use CRT */
    e_pkc_rsa_padding_t     padding_option;          /**< Selection of padding method */
    e_pkc_bool_t            verified_gen_plain;      /**< Verifying generated plaintext flag */
    e_pkc_bool_t            verified_gen_sign;       /**< Verifying generated signature flag */
    e_pkc_bool_t            sec_mod_exp;             /**< A variable for choosing whether to execute
                                                        the anti-leakage modular exponentiation function */
    PKC_CALLBACK_T          p_callback_api;          /**< General callback function pointer */
} st_pkc_rsa_ctx_t;

/** DH domain parameters structure */
typedef struct
{
    st_pkc_data_t *      p_p;                       /**< modulus p */
    st_pkc_data_t *      p_q;                       /**< order q */
    st_pkc_data_t *      p_g;                       /**< generator g */
} st_pkc_dh_param_t;

/** Context used by DH API */
typedef struct
{
    uint8_t *               p_buf;                  /**< A buffer for storing data during calculation */
    e_pkc_bool_t            is_constant_time_mode;  /**< Select whether to use constant time mode when calculating
                                                        exponentiation */
    PKC_DRBG_T              p_drbg_api;             /**< DRBG function pointer */
    st_pkc_kdf_t *          p_kdf_info;             /**< KDF information */
    st_pkc_dh_param_t *     p_dh_param;             /**< DH domain parameters */
} st_pkc_dh_ctx_t;

/** Curve information structure */
typedef struct
{
    uint32_t            curve_len;                  /**< Elliptic curve byte length */
    st_pkc_data_t       curve_params;               /**< Binary data concatenating elliptic curve domain parameters */
} st_pkc_ec_curve_t;

/** Contexts used by ECDSA / ECDH API */
typedef struct
{
    uint8_t *           p_buf;                      /**< Buffer for storing data during calculation */
    st_pkc_hash_t *     p_hash_info;                /**< Hash information */
    PKC_DRBG_T          p_drbg_api;                 /**< DRBG function pointer */
    st_pkc_kdf_t *      p_kdf_info;                 /**< KDF information */
    st_pkc_ec_curve_t * p_curve_info;               /**< Curve information */
    e_pkc_bool_t        verified_gen_sign;          /**< Verifying generated signature flag */
    e_pkc_bool_t        sec_scalar_mul;             /**< A variable for choosing whether to execute
                                                        the blinded scalar multiplication function */
} st_pkc_ec_ctx_t;

/** A structure that selects the data format and sets the data */
typedef struct
{
    e_pkc_data_type_t    data_type;                 /**< data form */
    st_pkc_data_t *      p_sel_data;                /**< Data and data length in the format selected by data_type */
} st_pkc_select_data_t;

/** Contexts used by EDDSA API */
typedef struct
{
    uint8_t *             p_buf;                      /**< Buffer for storing data during calculation */
    st_pkc_hash_t *       p_hash_info;                /**< Hash information */
    PKC_DRBG_T            p_drbg_api;                 /**< DRBG function pointer */
    st_pkc_kdf_t *        p_kdf_info;                 /**< KDF information */
    e_pkc_eddsa_variant_t eddsa_variant;              /**< EdDSA Variant */
    st_pkc_data_t *       p_ed_ctx;                   /**< EdDSA signature generation / validation Ed context data */
    st_pkc_ec_curve_t *   p_curve_info;               /**< Curve information */
    e_pkc_bool_t          verifed_gen_sign;           /**< Verified generated signature flag
                                                        (Use only R_PKCL_EddsaSignInit/Operate/Final) */
} st_pkc_eddsa_ctx_t;

/** Contexts used by ECDHX API */
typedef struct
{
    uint8_t *           p_buf;                      /**< Buffer for storing data during calculation */
    st_pkc_hash_t *     p_hash_info;                /**< Hash information */
    PKC_DRBG_T          p_drbg_api;                 /**< DRBG function pointer */
    st_pkc_kdf_t *      p_kdf_info;                 /**< KDF information */
    st_pkc_ec_curve_t * p_curve_info;               /**< Curve information */
} st_pkc_ecdhx_ctx_t;

/*! \}*/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#endif /* R_PKC_API_H */
