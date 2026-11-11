/******************************************************************************/
/* Component Name   Cryptographic Library PKC Library                         */
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

/* PRQA S 0310,3305 ++ # D002 In order not to disclose the structure used internally,
                            after receiving it with type uint8_t*, cast it to the type of the structure. */
/* PRQA S 0751 ++ # D006 Cast required to match the type of the calling function. */
/* PRQA S 1503 ++ # D010 No problem if it is unused because it is an API called from outside the library  */
/* PRQA S 2912,2913,3383,3384 ++ # D012 Have confirmed in advance that the value doesn't wrap around. */
/* PRQA S 2981 ++ # D013 Local variable ret of APIs sets INTERNALERR to the initial value
                        as a countermeasure against attacks that pass through processing. */
/* PRQA S 2984 ++ # D014 Because it is calculated in consideration of future expansion */
/* PRQA S 2985, 2995 ++ # D015 Required if key length not divisible by 8bit is supported. */
/* PRQA S 3673 ++ # D017 Because future extensions may edit values in context. */
/* PRQA S 5002 ++ # D019 There is no problem because it is used in a system that can distinguish
                    file names of 8 characters or more. */
/* PRQA S 2880,2992,2996 ++ # D020 Double check in signature verification as a countermeaseure
                    against attacks that skip a certain process */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "renesas_types.h"
#include "r_pkc_api.h"
#include "r_pkcl_cfg.h"
#include "r_pkcl_api.h"
#include "r_pkcl_cmn.h"

/*!********************************************************************
 * \addtogroup    PKCLRSA
 * \{
 * \addtogroup    PKCLRSADefines             RSA Definitions
 * \addtogroup    PKCLRSATypes               RSA Types
 * \addtogroup    PKCLRSAAPIs                RSA APIs
 * \addtogroup    PKCLRSADefinesInternal     RSA Internal Definition
 * \addtogroup    PKCLRSATypesInternal       RSA Internal Types
 * \addtogroup    PKCLRSAVariablesInternal   RSA Internal Variables
 * \addtogroup    PKCLRSAAPIsInternal        RSA Internal APIs
 * \}******************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \addtogroup PKCLRSADefinesInternal
 * \{*****************************************************************/
#define PKCL_RSA_KEY_LEN_1024          (128U)   /**< Byte size with RSA key length of 1024 bits */
#define PKCL_RSA_KEY_LEN_2048          (256U)   /**< Byte size with RSA key length of 2048 bits */
#define PKCL_RSA_KEY_LEN_3072          (384U)   /**< Byte size with RSA key length of 3072 bits */
#define PKCL_RSA_KEY_LEN_4096          (512U)   /**< Byte size with RSA key length of 4096 bits */
#define PKCL_RSA_KEY_LEN_MAX           (512U)   /**< Maximum byte size of RSA key length */

#define PKCL_RSA_DOUBLE_LEN            (2U)     /**< Definition to calculate double length */

#define PKCL_RSA_PS_OFFSET             (2U)     /**< Offset of PS in the PKCS v1.5 format */
#define PKCL_RSA_EME_PKCS_PAD_LEN_MIN  (11U)    /**< Minimum value padded with EME-PKCS1-v1_5 encoding */
#define PKCL_RSA_PKCS_PS_LEN_MIN       (8U)     /**< Minimum value with PS length */
#define PKCL_RSA_PKCS_PS_LEN_CALC      (3U)     /**< Value used when calculating PS length with PKCS encoding */
#define PKCL_RSA_PKCS_PADDING_NUM      (0x02U)  /**< Fixed value to pad with EME-PKCS1-v1_5 encoding */
#define PKCL_RSA_PKCS_PS_NUM           (0xFFU)  /**< PS value of EME-PKCS1-v1_5 encoding */
#define PKCL_RSA_PSS_PS_LEN_CALC       (2U)     /**< Value used when calculating PS length with PSS encoding */
#define PKCL_RSA_PKCS_PS_ZERO_LOOP_MAX (10000U) /**< Maximum number of attempts to generate an alternative
                                                       random number if the PS contains zeros */
#define PKCL_RSA_PKCS_DER_LEN_MAX      (19U)    /**< Maximum DER encoding length of DigestInfo */
#define PKCL_RSA_PSS_PAD_LEN           (8U)     /**< padded length with EMSA-PSS encoding */
#define PKCL_RSA_PSS_EM_LEN_CALC       (2U)     /**< The value used to calculate emLen */
#define PKCL_RSA_PSS_PADDING_NUM       (0xBCU)  /**< Fixed value to pad with EMSA-PSS encoding */
#define PKCL_RSA_OCTET                 (8U)     /**< octet num */
#define PKCL_RSA_BIT_CLEAR_NUM         (0xFFU)  /**< Value used to clear the most significant bit */

#define PKCL_RSA_DATA_CHK_MODE_MSG     (0U)     /**< st_pkc_data_t type argument check, PlainText and MessageDigest */
#define PKCL_RSA_DATA_CHK_MODE_OTHER   (1U)     /**< st_pkc_data_t type argument check,
                                                      Other than PlainText and MessageDigest */
#define PKCL_RSA_DATA_CHK_IN_ARG       (0U)     /**< If the argument is an input argument,
                                                     then returns PKCL_RSA_DATA_CHK_IN_ARG */
#define PKCL_RSA_DATA_CHK_OUT_ARG      (1U)     /**< If the argument to check is an output argument,
                                                     then returns PKCL_RSA_DATA_CHK_OUT_ARG */

#define PKCL_RSA_HASH_LEN_SHA1         (20U)    /**< Byte size of the output of SHA1 */
#define PKCL_RSA_HASH_LEN_SHA224       (28U)    /**< Byte size of the output of SHA2-224, SHA2-512/224, SHA3-224 */
#define PKCL_RSA_HASH_LEN_SHA256       (32U)    /**< Byte size of the output of SHA2-256, SHA2-512/256, SHA3-256 */
#define PKCL_RSA_HASH_LEN_SHA384       (48U)    /**< Byte size of the output of SHA2-384, SHA3-384 */
#define PKCL_RSA_HASH_LEN_SHA512       (64U)    /**< Byte size of the output of SHA2-512, SHA3-512 */
#define PKCL_RSA_HASH_LEN_MIN          (20U)    /**< Byte size of the smallest hash of the supported Hash algorithms */
#define PKCL_RSA_HASH_LEN_MAX          (64U)    /**< Byte size of the biggest hash of the supported Hash algorithms */

#define PKCL_RSA_SALT_LEN_CALC         (2U)     /**< The value used to calculate salt_len */
/** Maximum byte length of salt used in EMSA-PSS */
#define PKCL_RSA_SALT_LEN_MAX          (PKCL_RSA_KEY_LEN_MAX - PKCL_RSA_HASH_LEN_MIN - 2U)

#define PKCL_RSA_DER_SHA1_LEN          (15U)  /**< DER length of SHA1 digest info */
#define PKCL_RSA_DER_SHA2_AND_SHA3_LEN (19U)  /**< DER length of SHA2 and SHA3 digest info */

#define PKCL_RSA_PRIME_TEST_NUM_MIN    (0x1U)     /**< Min number of trials to generate a prime number. */
#define PKCL_RSA_PRIME_TEST_NUM_MAX    (0xF4240U) /**< Max number of trials to generate a prime number. */
#define PKCL_RSA_ITERATION_NUM_MIN     (0x1U)     /**< Maximum number of Miller-Rabin test trials used
                                                        in primality test */
#define PKCL_RSA_ITERATION_NUM_MAX     (0x14U)    /**< Maximum number of Miller-Rabin test trials used
                                                        in primality test */
/*! \}*/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \addtogroup PKCLRSATypesInternal
 * \{*****************************************************************/
/** A structure of values that needs to be buffered in RSA Encrypt */
typedef struct
{
    uint32_t mod_len;
} st_pkcl_rsa_enc_t;

/** A structure of values that needs to be buffered in RSA Decrypt */
typedef struct
{
    uint32_t        mod_len;
    uint8_t         cipher[PKCL_RSA_KEY_LEN_MAX];
    uint32_t        cipher_len;
    uint8_t         plain_temp[PKCL_RSA_KEY_LEN_MAX];
    const void *    p_priv_key;
} st_pkcl_rsa_dec_t;

/** A structure of values that needs to be buffered in RSA encryption after RSA decryption */
typedef struct
{
    uint8_t  em[PKCL_RSA_KEY_LEN_MAX];
    uint8_t  cipher[PKCL_RSA_KEY_LEN_MAX];
} st_pkcl_rsa_verified_plain_t;


/** A structure of values that needs to be buffered in RSA Signature generation */
typedef struct
{
    uint32_t        mod_len;
    uint8_t         digest[PKCL_RSA_KEY_LEN_MAX];
    uint32_t        digest_len;
    uint8_t         sign_temp[PKCL_RSA_KEY_LEN_MAX];
    const void *    p_priv_key;
} st_pkcl_rsa_sign_t;

/** A structure of values that needs to be buffered in RSA signature verification after signature generation */
typedef struct
{
    uint8_t em[PKCL_RSA_KEY_LEN_MAX];
} st_pkcl_rsa_verified_sign_t;

/** A structure of values that needs to be buffered in RSA Signature verification */
typedef struct
{
    uint32_t  mod_len;
    uint32_t  digest_len;
    uint8_t   digest_data[PKCL_RSA_KEY_LEN_MAX];
} st_pkcl_rsa_verify_t;

/*! \}*/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_pkc_data (const st_pkc_data_t * const p_pkc_data,
                                            const uint32_t key_len, const uint32_t chk_mode, const uint32_t arg_type);
static pkcl_ret_t r_pkcl_rsa_chk_mask_option (const st_pkc_rsa_mask_opt_t * const p_mask_opt);
static pkcl_ret_t r_pkcl_rsa_chk_hash_type (const e_pkc_hash_type_t hash_type);
static pkcl_ret_t r_pkcl_rsa_chk_salt (const uint32_t salt_len, const uint32_t key_len,
                                        const e_pkc_hash_type_t hash_type);
static pkcl_ret_t r_pkcl_rsa_chk_enc_init_arg (const st_pkc_data_t * const p_plain,
                                                const void * const p_pub_key,
                                                const uint32_t mod_len,
                                                const st_pkc_rsa_ctx_t * const p_ctx);
static pkcl_ret_t r_pkcl_rsa_chk_enc_operate_arg (const st_pkc_rsa_ctx_t * const p_ctx,
                                                    const st_pkc_operate_opt_t * const p_operate_opt);
static pkcl_ret_t r_pkcl_rsa_chk_enc_final_arg (const st_pkc_rsa_ctx_t * const p_ctx,
                                                const st_pkc_data_t * const p_cipher);
static pkcl_ret_t r_pkcl_rsa_chk_dec_init_arg (const st_pkc_data_t * const p_cipher,
                                                const void * const p_priv_key,
                                                const uint32_t mod_len,
                                                const st_pkc_rsa_ctx_t * const p_ctx);
static pkcl_ret_t r_pkcl_rsa_chk_dec_operate_arg (const st_pkc_rsa_ctx_t * const p_ctx,
                                                    const st_pkc_operate_opt_t * const p_operate_opt);
static pkcl_ret_t r_pkcl_rsa_chk_dec_final_arg (const st_pkc_rsa_ctx_t * const p_ctx,
                                                const st_pkc_data_t * const p_plain);
static pkcl_ret_t r_pkcl_rsa_chk_sign_init_arg (const st_pkc_data_t * const p_digest,
                                                const void * const p_priv_key,
                                                const uint32_t mod_len,
                                                const st_pkc_rsa_ctx_t * const p_ctx);
static pkcl_ret_t r_pkcl_rsa_chk_sign_operate_arg (const st_pkc_rsa_ctx_t * const p_ctx,
                                                    const st_pkc_operate_opt_t * const p_operate_opt);
static pkcl_ret_t r_pkcl_rsa_chk_sign_final_arg (const st_pkc_rsa_ctx_t * const p_ctx,
                                                const st_pkc_data_t * const p_sign);
static pkcl_ret_t r_pkcl_rsa_chk_verify_init_arg (const st_pkc_data_t * const p_digest,
                                                    const st_pkc_data_t * const p_sign,
                                                    const void * const p_pub_key,
                                                    const uint32_t mod_len,
                                                    const st_pkc_rsa_ctx_t * const p_ctx);
static pkcl_ret_t r_pkcl_rsa_chk_verify_operate_arg (const st_pkc_rsa_ctx_t * const p_ctx,
                                                    const st_pkc_operate_opt_t * const p_operate_opt);
static pkcl_ret_t r_pkcl_rsa_chk_verify_final_arg (const st_pkc_rsa_ctx_t * const p_ctx);
static uint32_t r_pkcl_rsa_get_hash_len (const e_pkc_hash_type_t hash_type);
static void r_pkcl_rsa_padding_no (const st_pkc_data_t * const p_plain,
                                    const uint32_t mod_len,
                                    const st_pkc_data_t * const p_em);
static pkcl_ret_t r_pkcl_rsa_enc_scheme_pkcs1 (const st_pkc_data_t * const p_plain,
                                                const st_pkc_rsa_ctx_t * const p_ctx,
                                                const uint32_t mod_len,
                                                st_pkc_data_t * const p_em,
                                                const uint8_t * const p_check_ps);
static pkcl_ret_t r_pkcl_rsa_enc_scheme_oaep (const st_pkc_data_t * const p_plain,
                                                const st_pkc_rsa_ctx_t * const p_ctx,
                                                const uint32_t mod_len,
                                                st_pkc_data_t * const p_em,
                                                const uint8_t * const p_check_seed);
static pkcl_ret_t r_pkcl_rsa_dec_scheme_pkcs1 (const st_pkc_data_t * const p_em,
                                                const st_pkc_rsa_ctx_t* const p_ctx,
                                                const uint32_t mod_len,
                                                st_pkc_data_t * const p_plain);
static pkcl_ret_t r_pkcl_rsa_dec_scheme_oaep (const st_pkc_data_t * const p_em,
                                                const st_pkc_rsa_ctx_t * const p_ctx,
                                                const uint32_t mod_len,
                                                st_pkc_data_t * const p_plain);
static pkcl_ret_t r_pkcl_rsa_sign_scheme_pkcs1 (const st_pkc_data_t * const p_digest,
                                                const e_pkc_hash_type_t hash_type,
                                                const uint32_t mod_len,
                                                const st_pkc_data_t * const p_em);
static pkcl_ret_t r_pkcl_rsa_sign_scheme_pss (const st_pkc_data_t * const p_digest,
                                                const st_pkc_rsa_ctx_t * const p_ctx,
                                                const uint32_t mod_len,
                                                st_pkc_data_t * const p_em);
static pkcl_ret_t r_pkcl_rsa_verify_scheme_pkcs1 (const st_pkc_data_t * const p_em,
                                                    const st_pkc_data_t * const p_digest,
                                                    const e_pkc_hash_type_t hash_type,
                                                    const uint32_t mod_len);
static pkcl_ret_t r_pkcl_rsa_verify_scheme_pss (const st_pkc_data_t * const p_em,
                                                const st_pkc_data_t * const p_digest,
                                                const st_pkc_rsa_ctx_t * const p_ctx,
                                                const uint32_t mod_len);
static pkcl_ret_t r_pkcl_rsa_mgf1 (const st_pkc_rsa_ctx_t * const p_ctx,
                                    const uint8_t * const p_seed,
                                    const uint32_t seed_len,
                                    uint8_t * const p_mask,
                                    const uint32_t mask_len);
static pkcl_ret_t r_pkcl_rsa_verified_gen_sign (const st_pkc_rsa_ctx_t * const p_ctx,
                                                const st_pkc_data_t * const p_sign_data);
static pkcl_ret_t r_pkcl_rsa_verified_gen_plain (const st_pkc_rsa_ctx_t * const p_ctx,
                                                const st_pkc_data_t * const p_plain_data);

/*!********************************************************************
 * \addtogroup PKCLRSAVariablesInternal
 * \{*****************************************************************/
/** DER of SHA1 digest info */
static const uint8_t s_der_sha1[PKCL_RSA_DER_SHA1_LEN] =
{
    0x30U, 0x21U, 0x30U, 0x09U, 0x06U, 0x05U, 0x2bU, 0x0eU, 0x03U, 0x02U, 0x1aU, 0x05U, 0x00U, 0x04U, 0x14U
};

/** DER of SHA224 digest info */
static const uint8_t s_der_sha224[PKCL_RSA_DER_SHA2_AND_SHA3_LEN] =
{
    0x30U, 0x2dU, 0x30U, 0x0dU, 0x06U, 0x09U, 0x60U, 0x86U, 0x48U, 0x01U, 0x65U, 0x03U, 0x04U, 0x02U, 0x04U, 0x05U,
    0x00U, 0x04U, 0x1cU
};

/** DER of SHA256 digest info */
static const uint8_t s_der_sha256[PKCL_RSA_DER_SHA2_AND_SHA3_LEN] =
{
    0x30U, 0x31U, 0x30U, 0x0dU, 0x06U, 0x09U, 0x60U, 0x86U, 0x48U, 0x01U, 0x65U, 0x03U, 0x04U, 0x02U, 0x01U, 0x05U,
    0x00U, 0x04U, 0x20U
};

/** DER of SHA384 digest info */
static const uint8_t s_der_sha384[PKCL_RSA_DER_SHA2_AND_SHA3_LEN] =
{
    0x30U, 0x41U, 0x30U, 0x0dU, 0x06U, 0x09U, 0x60U, 0x86U, 0x48U, 0x01U, 0x65U, 0x03U, 0x04U, 0x02U, 0x02U, 0x05U,
    0x00U, 0x04U, 0x30U
};

/** DER of SHA512 digest info */
static const uint8_t s_der_sha512[PKCL_RSA_DER_SHA2_AND_SHA3_LEN] =
{
    0x30U, 0x51U, 0x30U, 0x0dU, 0x06U, 0x09U, 0x60U, 0x86U, 0x48U, 0x01U, 0x65U, 0x03U, 0x04U, 0x02U, 0x03U, 0x05U,
    0x00U, 0x04U, 0x40U
};

/** DER of SHA512/224 digest info */
static const uint8_t s_der_sha512_224[PKCL_RSA_DER_SHA2_AND_SHA3_LEN] =
{
    0x30U, 0x2dU, 0x30U, 0x0dU, 0x06U, 0x09U, 0x60U, 0x86U, 0x48U, 0x01U, 0x65U, 0x03U, 0x04U, 0x02U, 0x05U, 0x05U,
    0x00U, 0x04U, 0x1cU
};

/** DER of SHA512/256 digest info */
static const uint8_t s_der_sha512_256[PKCL_RSA_DER_SHA2_AND_SHA3_LEN] =
{
    0x30U, 0x31U, 0x30U, 0x0dU, 0x06U, 0x09U, 0x60U, 0x86U, 0x48U, 0x01U, 0x65U, 0x03U, 0x04U, 0x02U, 0x06U, 0x05U,
    0x00U, 0x04U, 0x20U
};

/** DER of SHA3-224 digest info */
static const uint8_t s_der_sha3_224[PKCL_RSA_DER_SHA2_AND_SHA3_LEN] =
{
    0x30U, 0x2dU, 0x30U, 0x0dU, 0x06U, 0x09U, 0x60U, 0x86U, 0x48U, 0x01U, 0x65U, 0x03U, 0x04U, 0x02U, 0x07U, 0x05U,
    0x00U, 0x04U, 0x1cU
};

/** DER of SHA3-256 digest info */
static const uint8_t s_der_sha3_256[PKCL_RSA_DER_SHA2_AND_SHA3_LEN] =
{
    0x30U, 0x31U, 0x30U, 0x0dU, 0x06U, 0x09U, 0x60U, 0x86U, 0x48U, 0x01U, 0x65U, 0x03U, 0x04U, 0x02U, 0x08U, 0x05U,
    0x00U, 0x04U, 0x20U
};

/** DER of SHA3-384 digest info */
static const uint8_t s_der_sha3_384[PKCL_RSA_DER_SHA2_AND_SHA3_LEN] =
{
    0x30U, 0x41U, 0x30U, 0x0dU, 0x06U, 0x09U, 0x60U, 0x86U, 0x48U, 0x01U, 0x65U, 0x03U, 0x04U, 0x02U, 0x09U, 0x05U,
    0x00U, 0x04U, 0x30U
};

/** DER of SHA3-512 digest info */
static const uint8_t s_der_sha3_512[PKCL_RSA_DER_SHA2_AND_SHA3_LEN] =
{
    0x30U, 0x51U, 0x30U, 0x0dU, 0x06U, 0x09U, 0x60U, 0x86U, 0x48U, 0x01U, 0x65U, 0x03U, 0x04U, 0x02U, 0x0aU, 0x05U,
    0x00U, 0x04U, 0x40U
};

/*! \}*/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaEncryptInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaEncryptInit(const st_pkc_data_t* const p_plain, const void* const p_pub_key,
                                    const uint32_t mod_len, st_pkc_rsa_ctx_t* const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_RSA_ENC != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkc_data_t em_data;
    uint8_t em_val[PKCL_RSA_KEY_LEN_MAX];
    st_pkcl_rsa_enc_t * p_rsa_enc;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_rsa_chk_enc_init_arg(p_plain, p_pub_key, mod_len, p_ctx);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to RSA KeyGen buffer type */
            p_rsa_enc = (st_pkcl_rsa_enc_t*)p_ctx->p_buf;

            /* Save mod_len */
            p_rsa_enc->mod_len = mod_len;

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_enc_t)];

            /* set em_data */
            em_data.p_data = em_val;
            em_data.len = mod_len;

            if (PKC_RSA_PADDING_NO == p_ctx->padding_option)
            {
                /* If the message size is less than mod_len, 0 padding will be performed. */
                r_pkcl_rsa_padding_no(p_plain, mod_len, &em_data);
            }
            else if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
            {
                /* Execute the scheme of EME-PKCS1-v1_5 */
                ret = r_pkcl_rsa_enc_scheme_pkcs1(p_plain, p_ctx, mod_len, &em_data, NULL);
            }
            else /* PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option */
            {
                /* Execute the scheme of EME-OAEP */
                ret = r_pkcl_rsa_enc_scheme_oaep(p_plain, p_ctx, mod_len, &em_data, NULL);
            }

            if (PKCL_RET_OK == ret)
            {
                /* Set anti-tampering mod exp flag to FALSE */
                p_ctx->sec_mod_exp = PKC_BOOL_FALSE;

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_RsaModExpInit(&em_data, p_pub_key, mod_len,
                                                PKCD_RSA_EXEC_TYPE_ENC, p_ctx, p_buf_drv);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
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
        ret = PKCL_RET_STATUSERR;
    }

    if (PKCL_RET_OK == ret)
    {
        /* State transition */
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_ENC_INIT_DONE);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_RSA_ENC != 1U */
    /* Not supported by  PKC-Lib_V.1.10 */
#endif /* PKCL_CFG_ENABLE_DRV_RSA_ENC != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaEncryptInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaEncryptOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaEncryptOperate(st_pkc_rsa_ctx_t* const p_ctx,
                                    const st_pkc_operate_opt_t* const p_operate_opt)
{
#if (PKCL_CFG_ENABLE_DRV_RSA_ENC != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if ((PKCL_STATE_RSA_ENC_INIT_DONE == pkcl_state) ||
        (PKCL_STATE_RSA_ENC_OPERATE_PROCESSING == pkcl_state))
    {
        /* Check argument */
        ret = r_pkcl_rsa_chk_enc_operate_arg(p_ctx, p_operate_opt);
        if (PKCL_RET_OK == ret)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_enc_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_RsaModExpOperate(p_ctx, p_buf_drv, p_operate_opt);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if (PKCL_RET_OK == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_ENC_OPERATE_DONE);
    }
    else if (PKCL_RET_PROCESSING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_ENC_OPERATE_PROCESSING);
    }
    else if ((PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_RSA_ENC != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_RSA_ENC != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaEncryptOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaEncryptFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaEncryptFinal(const st_pkc_rsa_ctx_t* const p_ctx, st_pkc_data_t* const p_cipher)

{
#if (PKCL_CFG_ENABLE_DRV_RSA_ENC != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_RSA_ENC_OPERATE_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_rsa_chk_enc_final_arg(p_ctx, p_cipher);
        if (PKCL_RET_OK == ret)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_enc_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_RsaModExpFinal(p_ctx, p_buf_drv, p_cipher);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if ((PKCL_RET_OK == ret) || (PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_RSA_ENC != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_RSA_ENC != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaEncryptFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaDecryptInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaDecryptInit(const st_pkc_data_t* const p_cipher, const void* const p_priv_key,
                                    const uint32_t mod_len, st_pkc_rsa_ctx_t* const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_RSA_DEC != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_rsa_dec_t * p_rsa_dec;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_rsa_chk_dec_init_arg(p_cipher, p_priv_key, mod_len, p_ctx);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to RSA KeyGen buffer type */
            p_rsa_dec = (st_pkcl_rsa_dec_t*)p_ctx->p_buf;

            /* Save mod_len */
            p_rsa_dec->mod_len = mod_len;

            /* Save a private key */
            p_rsa_dec->p_priv_key = p_priv_key;

            /* Save digest */
            p_rsa_dec->cipher_len = p_cipher->len;
            r_pkcl_cmn_cpy_byte(p_rsa_dec->cipher, p_cipher->p_data, p_cipher->len);

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_dec_t)];

            /* Check crt mode */
            if (PKC_BOOL_TRUE == p_ctx->is_crt_mode)
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_RsaModExpCRTInit(p_cipher, p_priv_key, mod_len,
                                                    PKCD_RSA_EXEC_TYPE_DEC, p_ctx, p_buf_drv);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
            }
            else /* PKCL_FALSE == p_ctx->is_crt_mode */
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_RsaModExpInit(p_cipher, p_priv_key, mod_len,
                                                PKCD_RSA_EXEC_TYPE_DEC, p_ctx, p_buf_drv);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    if (PKCL_RET_OK == ret)
    {
        /* State transition */
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_DEC_INIT_DONE);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_RSA_DEC != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_RSA_DEC != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaDecryptInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaDecryptOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaDecryptOperate(st_pkc_rsa_ctx_t* const p_ctx,
                                    const st_pkc_operate_opt_t* const p_operate_opt)
{
#if (PKCL_CFG_ENABLE_DRV_RSA_DEC != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if ((PKCL_STATE_RSA_DEC_INIT_DONE == pkcl_state) ||
        (PKCL_STATE_RSA_DEC_OPERATE_PROCESSING == pkcl_state))
    {
        /* Check argument */
        ret = r_pkcl_rsa_chk_dec_operate_arg(p_ctx, p_operate_opt);
        if (PKCL_RET_OK == ret)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_dec_t)];

            /* Check crt mode */
            if (PKC_BOOL_TRUE == p_ctx->is_crt_mode)
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_RsaModExpCRTOperate(p_ctx, p_buf_drv, p_operate_opt->sync_opt);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
            }
            else /* PKCL_FALSE == p_ctx->is_crt_mode */
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_RsaModExpOperate(p_ctx, p_buf_drv, p_operate_opt);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if (PKCL_RET_OK == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_DEC_OPERATE_DONE);
    }
    else if (PKCL_RET_PROCESSING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_DEC_OPERATE_PROCESSING);
    }
    else if ((PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_RSA_DEC != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_RSA_DEC != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaDecryptOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaDecryptFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaDecryptFinal(const st_pkc_rsa_ctx_t* const p_ctx, st_pkc_data_t* const p_plain)
{
#if (PKCL_CFG_ENABLE_DRV_RSA_DEC != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkc_data_t em_data;
    uint8_t em_val[PKCL_RSA_KEY_LEN_MAX];
    st_pkcl_rsa_dec_t * p_rsa_dec;
    uint8_t * p_buf_drv;
    st_pkc_data_t plain_temp_data;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_RSA_DEC_OPERATE_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_rsa_chk_dec_final_arg(p_ctx, p_plain);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to RSA Decrypt buffer type */
            p_rsa_dec = (st_pkcl_rsa_dec_t*)p_ctx->p_buf;

            /* Prepare to pass encode message to PKC-Driver */
            em_data.p_data = em_val;
            em_data.len = p_rsa_dec->mod_len;

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_dec_t)];

            /* Whether CRT mode is enabled */
            if (PKC_BOOL_TRUE == p_ctx->is_crt_mode)
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_RsaModExpCRTFinal(p_ctx, p_buf_drv, &em_data);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
            }
            else /* PKCL_FALSE == p_ctx->is_crt_mode */
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_RsaModExpFinal(p_ctx, p_buf_drv, &em_data);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
            }

            if (PKCL_RET_OK == ret)
            {
                /* Set the temporal buffer for a generated plain */
                plain_temp_data.p_data = p_rsa_dec->plain_temp;
                plain_temp_data.len    = sizeof(p_rsa_dec->plain_temp);

                /* Performs scheme processing according to the padding option settings */
                if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
                {
                    ret = r_pkcl_rsa_dec_scheme_pkcs1(&em_data, p_ctx, p_rsa_dec->mod_len, &plain_temp_data);
                }
                else if (PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option)
                {
                    ret = r_pkcl_rsa_dec_scheme_oaep(&em_data, p_ctx, p_rsa_dec->mod_len, &plain_temp_data);
                }
                else /* PKC_RSA_PADDING_NO == p_ctx->padding_option */
                {
                    plain_temp_data.len = em_data.len;
                    r_pkcl_cmn_cpy_byte(plain_temp_data.p_data, em_data.p_data, plain_temp_data.len);
                }

                if (PKCL_RET_OK == ret)
                {
                    if (PKC_BOOL_FALSE != p_ctx->verified_gen_plain)
                    {
                        ret = r_pkcl_rsa_verified_gen_plain(p_ctx, &plain_temp_data);
                    }
                    else
                    {
                        /* Do nothing */
                    }

                    if (PKCL_RET_OK == ret)
                    {
                        /* Output plain text */
                        r_pkcl_cmn_cpy_byte(p_plain->p_data, plain_temp_data.p_data, plain_temp_data.len);
                        p_plain->len = plain_temp_data.len;
                    }
                    else
                    {
                        /* Do nothing */
                    }

                    /* Clear temporal plain text data */
                    r_pkcl_cmn_set_byte(plain_temp_data.p_data, 0U, plain_temp_data.len);
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
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if ((PKCL_RET_OK == ret) || (PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_RSA_DEC != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_RSA_DEC != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaDecryptFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaSignInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaSignInit(const st_pkc_data_t* const p_digest, const void* const p_priv_key,
                                const uint32_t mod_len, st_pkc_rsa_ctx_t* const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_RSA_SIGN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_rsa_sign_t * p_rsa_sign;
    st_pkc_data_t em_data;
    uint8_t em_val[PKCL_RSA_KEY_LEN_MAX];
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_rsa_chk_sign_init_arg(p_digest, p_priv_key, mod_len, p_ctx);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to RSA KeyGen buffer type */
            p_rsa_sign = (st_pkcl_rsa_sign_t*)p_ctx->p_buf;

            /* Save mod_len */
            p_rsa_sign->mod_len = mod_len;

            /* Save a private key */
            p_rsa_sign->p_priv_key = p_priv_key;

            /* Save digest */
            p_rsa_sign->digest_len = p_digest->len;
            r_pkcl_cmn_cpy_byte(p_rsa_sign->digest, p_digest->p_data, p_digest->len);

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_sign_t)];

            /* set em_data */
            em_data.p_data = em_val;
            em_data.len = mod_len;

            if (PKC_RSA_PADDING_NO == p_ctx->padding_option)
            {
                /* If the message size is less than mod_len, 0 padding will be performed. */
                r_pkcl_rsa_padding_no(p_digest, mod_len, &em_data);

                /* Check crt mode */
                if (PKC_BOOL_TRUE == p_ctx->is_crt_mode)
                {
                    /* Call PKC-Driver API */
                    drv_ret = R_PKCD_RsaModExpCRTInit(&em_data, p_priv_key, mod_len,
                                                        PKCD_RSA_EXEC_TYPE_SIGN, p_ctx, p_buf_drv);

                    /* Convert to PKC-Lib return code */
                    ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
                }
                else /* PKCL_FALSE == p_ctx->is_crt_mode */
                {
                    /* Call PKC-Driver API */
                    drv_ret = R_PKCD_RsaModExpInit(&em_data, p_priv_key, mod_len,
                                                    PKCD_RSA_EXEC_TYPE_SIGN, p_ctx, p_buf_drv);

                    /* Convert to PKC-Lib return code */
                    ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
                }
            }
            else /* PKC_RSA_PADDING_PKCS1 or PKC_RSA_PADDING_PKCS1_PSS */
            {
                if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
                {
                    ret = r_pkcl_rsa_sign_scheme_pkcs1(p_digest, p_ctx->msg_digest_type, mod_len, &em_data);
                }
                else /* PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option */
                {
                    ret = r_pkcl_rsa_sign_scheme_pss(p_digest, p_ctx, mod_len, &em_data);
                }

                if (PKCL_RET_OK == ret)
                {
                    /* Check crt mode */
                    if (PKC_BOOL_TRUE == p_ctx->is_crt_mode)
                    {
                        /* Call PKC-Driver API */
                        drv_ret = R_PKCD_RsaModExpCRTInit(&em_data, p_priv_key, mod_len,
                                                            PKCD_RSA_EXEC_TYPE_SIGN, p_ctx, p_buf_drv);

                        /* Convert to PKC-Lib return code */
                        ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
                    }
                    else /* PKCL_FALSE == p_ctx->is_crt_mode */
                    {
                        /* Call PKC-Driver API */
                        drv_ret = R_PKCD_RsaModExpInit(&em_data, p_priv_key, mod_len,
                                                        PKCD_RSA_EXEC_TYPE_SIGN, p_ctx, p_buf_drv);

                        /* Convert to PKC-Lib return code */
                        ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
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
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    if (PKCL_RET_OK == ret)
    {
        /* State transition */
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_SIGN_INIT_DONE);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_RSA_SIGN != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_RSA_SIGN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaSignInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaSignOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaSignOperate(st_pkc_rsa_ctx_t* const p_ctx,
                                    const st_pkc_operate_opt_t* const p_operate_opt)
{
#if (PKCL_CFG_ENABLE_DRV_RSA_SIGN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if ((PKCL_STATE_RSA_SIGN_INIT_DONE == pkcl_state) ||
        (PKCL_STATE_RSA_SIGN_OPERATE_PROCESSING == pkcl_state))
    {
        /* Check argument */
        ret = r_pkcl_rsa_chk_sign_operate_arg(p_ctx, p_operate_opt);
        if (PKCL_RET_OK == ret)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_sign_t)];

            /* Check crt mode */
            if (PKC_BOOL_TRUE == p_ctx->is_crt_mode)
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_RsaModExpCRTOperate(p_ctx, p_buf_drv, p_operate_opt->sync_opt);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
            }
            else /* PKCL_FALSE == p_ctx->is_crt_mode */
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_RsaModExpOperate(p_ctx, p_buf_drv, p_operate_opt);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if (PKCL_RET_OK == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_SIGN_OPERATE_DONE);
    }
    else if (PKCL_RET_PROCESSING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_SIGN_OPERATE_PROCESSING);
    }
    else if ((PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_RSA_SIGN != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_RSA_SIGN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaSignOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaSignFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaSignFinal(const st_pkc_rsa_ctx_t* const p_ctx,
                                st_pkc_data_t* const p_sign)
{
#if (PKCL_CFG_ENABLE_DRV_RSA_SIGN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    uint8_t * p_buf_drv;
    st_pkcl_rsa_sign_t * p_rsa_sign;
    st_pkc_data_t sign_temp_data;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_RSA_SIGN_OPERATE_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_rsa_chk_sign_final_arg(p_ctx, p_sign);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to RSA sign buffer type */
            p_rsa_sign = (st_pkcl_rsa_sign_t *)p_ctx->p_buf;

            /* Set the temporal buffer for a generated signature */
            sign_temp_data.p_data   = p_rsa_sign->sign_temp;
            sign_temp_data.len      = sizeof(p_rsa_sign->sign_temp);

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_sign_t)];

            /* Whether CRT mode is enabled */
            if (PKC_BOOL_TRUE == p_ctx->is_crt_mode)
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_RsaModExpCRTFinal(p_ctx, p_buf_drv, &sign_temp_data);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
            }
            else /* PKCL_FALSE == p_ctx->is_crt_mode */
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_RsaModExpFinal(p_ctx, p_buf_drv, &sign_temp_data);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
            }

            if (PKCL_RET_OK == ret)
            {
                if (PKC_BOOL_FALSE != p_ctx->verified_gen_sign)
                {
                    ret = r_pkcl_rsa_verified_gen_sign(p_ctx, &sign_temp_data);
                }
                else
                {
                    /* Do nothing */
                }

                if (PKCL_RET_OK == ret)
                {
                    /* Output sign */
                    r_pkcl_cmn_cpy_byte(p_sign->p_data, sign_temp_data.p_data, sign_temp_data.len);
                    p_sign->len = sign_temp_data.len;
                }
                else
                {
                    /* Do nothing */
                }

                /* Clear temporal signature data */
                r_pkcl_cmn_set_byte(sign_temp_data.p_data, 0U, sign_temp_data.len);
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
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if ((PKCL_RET_OK == ret) || (PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_RSA_SIGN != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_RSA_SIGN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaSignFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaVerifyInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaVerifyInit(const st_pkc_data_t* const p_digest, const st_pkc_data_t* const p_sign,
                                const void* const p_pub_key, const uint32_t mod_len,
                                st_pkc_rsa_ctx_t* const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_RSA_VERIFY != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_rsa_verify_t * p_rsa_verify;
    st_pkc_data_t em_data;
    uint8_t em_val[PKCL_RSA_KEY_LEN_MAX];
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_rsa_chk_verify_init_arg(p_digest, p_sign, p_pub_key, mod_len, p_ctx);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to RSA Verification buffer type */
            p_rsa_verify = (st_pkcl_rsa_verify_t*)p_ctx->p_buf;

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_verify_t)];

            /* Save mod_len */
            p_rsa_verify->mod_len = mod_len;

            /* Save p_digest */
            if (PKC_RSA_PADDING_NO == p_ctx->padding_option)
            {
                /* set em_data */
                em_data.p_data = em_val;
                em_data.len = mod_len;

                /* If the message size is less than mod_len, 0 padding will be performed. */
                r_pkcl_rsa_padding_no(p_digest, mod_len, &em_data);
                p_rsa_verify->digest_len = em_data.len;
                r_pkcl_cmn_cpy_byte(p_rsa_verify->digest_data, em_data.p_data, em_data.len);
            }
            else
            {
                p_rsa_verify->digest_len = p_digest->len;
                r_pkcl_cmn_cpy_byte(p_rsa_verify->digest_data, p_digest->p_data, p_digest->len);
            }

            /* Set anti-tampering mod exp flag to FALSE */
            p_ctx->sec_mod_exp = PKC_BOOL_FALSE;

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_RsaModExpInit(p_sign, p_pub_key, mod_len,
                                                PKCD_RSA_EXEC_TYPE_VERIFY, p_ctx, p_buf_drv);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    if (PKCL_RET_OK == ret)
    {
        /* State transition */
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_VERIFY_INIT_DONE);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_RSA_VERIFY != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_RSA_VERIFY != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaVerifyInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaVerifyOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaVerifyOperate(st_pkc_rsa_ctx_t* const p_ctx,
                                    const st_pkc_operate_opt_t* const p_operate_opt)
{
#if (PKCL_CFG_ENABLE_DRV_RSA_VERIFY != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if ((PKCL_STATE_RSA_VERIFY_INIT_DONE == pkcl_state) ||
        (PKCL_STATE_RSA_VERIFY_OPERATE_PROCESSING == pkcl_state))
    {
        /* Check argument */
        ret = r_pkcl_rsa_chk_verify_operate_arg(p_ctx, p_operate_opt);
        if (PKCL_RET_OK == ret)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_verify_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_RsaModExpOperate(p_ctx, p_buf_drv, p_operate_opt);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if (PKCL_RET_OK == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_VERIFY_OPERATE_DONE);
    }
    else if (PKCL_RET_PROCESSING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_VERIFY_OPERATE_PROCESSING);
    }
    else if ((PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_RSA_VERIFY != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_RSA_VERIFY != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaVerifyOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaVerifyFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaVerifyFinal(const st_pkc_rsa_ctx_t* const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_RSA_VERIFY != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkc_bool_t cmp_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_rsa_verify_t * p_rsa_verify;
    st_pkc_data_t digest_data;
    st_pkc_data_t em_data;
    uint8_t em_val[PKCL_RSA_KEY_LEN_MAX];
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_RSA_VERIFY_OPERATE_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_rsa_chk_verify_final_arg(p_ctx);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to RSA KeyGen buffer type */
            p_rsa_verify = (st_pkcl_rsa_verify_t*)p_ctx->p_buf;

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_verify_t)];

            /* Prepare to pass encode message to PKC-Driver */
            em_data.p_data = em_val;
            em_data.len = p_rsa_verify->mod_len;

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_RsaModExpFinal(p_ctx, p_buf_drv, &em_data);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Prepare to pass message digest to scheme */
                digest_data.len = p_rsa_verify->digest_len;
                digest_data.p_data = p_rsa_verify->digest_data;

                /* Performs scheme processing according to the padding option settings */
                if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
                {
                    ret = r_pkcl_rsa_verify_scheme_pkcs1(&em_data, &digest_data, p_ctx->msg_digest_type,
                                                            p_rsa_verify->mod_len);
                }
                else if (PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option)
                {
                    ret = r_pkcl_rsa_verify_scheme_pss(&em_data, &digest_data, p_ctx, p_rsa_verify->mod_len);
                }
                else /* PKC_RSA_PADDING_NO == p_ctx->padding_option */
                {
                    /* Set ret to 0 for double check */
                    ret = (pkcl_ret_t)0U;

                    cmp_ret = r_pkcl_cmn_cmp_byte(em_data.p_data, digest_data.p_data, em_data.len);
                    if (PKC_BOOL_TRUE == cmp_ret)
                    {
                        ret += PKCL_CMN_RET_OK_HALF_1;

                        cmp_ret = r_pkcl_cmn_cmp_byte(em_data.p_data, digest_data.p_data, em_data.len);

                        if (PKC_BOOL_TRUE == cmp_ret)
                        {
                            ret += PKCL_CMN_RET_OK_HALF_2;
                        }
                        else
                        {
                            ret = PKCL_RET_RSA_VERIFAIL;
                        }
                    }
                    else
                    {
                        ret = PKCL_RET_RSA_VERIFAIL;
                    }

                    /* Set an internal error when an invalid result is set */
                    if (((pkcl_ret_t)0U == ret) || (PKCL_CMN_RET_OK_HALF_1 == ret) || (PKCL_CMN_RET_OK_HALF_2 == ret))
                    {
                        ret = PKCL_RET_TAMPERERR;
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
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if ((PKCL_RET_OK == ret) || (PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_RSA_VERIFY != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_RSA_VERIFY != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaVerifyFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaKeyGenInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaKeyGenInit(st_pkc_rsa_ctx_t* const p_ctx, const uint32_t key_len,
                                const uint32_t prime_test_num, const uint32_t iteration_num)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* Check argument */
        if (NULL != p_ctx)
        {
            if (NULL != p_ctx->p_buf)
            {
                /* check key_len */
                if ((PKCL_RSA_KEY_LEN_1024 == key_len) || (PKCL_RSA_KEY_LEN_2048 == key_len) ||
                    (PKCL_RSA_KEY_LEN_3072 == key_len) || (PKCL_RSA_KEY_LEN_4096 == key_len))
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_LENGTHERR;
                }
            }
            else
            {
                ret = PKCL_RET_PARAMERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    if (PKCL_RET_OK == ret)
    {
        /* check prime_test_num */
        if ((PKCL_RSA_PRIME_TEST_NUM_MIN <= prime_test_num) && (PKCL_RSA_PRIME_TEST_NUM_MAX >= prime_test_num))
        {
            /* check iteration_num */
            if ((PKCL_RSA_ITERATION_NUM_MIN <= iteration_num) && (PKCL_RSA_ITERATION_NUM_MAX >= iteration_num))
            {
                ret = PKCL_RET_OK;
            }
            else
            {
                ret = PKCL_RET_LENGTHERR;
            }
        }
        else
        {
            ret = PKCL_RET_LENGTHERR;
        }
    }
    else
    {
        /* Do nothing */
    }

    if (PKCL_RET_OK == ret)
    {
        p_buf_drv = p_ctx->p_buf;
        drv_ret = R_PKCD_RsaKeyGenInit(p_ctx, p_buf_drv, key_len, prime_test_num, iteration_num);

        /* Convert drv_ret to PKC-Lib return code */
        ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

        /* State transition */
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_KEYGEN_INIT_DONE);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaKeyGenInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaKeyGenOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaKeyGenOperate(st_pkc_rsa_ctx_t* const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if ((PKCL_STATE_RSA_KEYGEN_INIT_DONE == pkcl_state) ||
        (PKCL_STATE_RSA_KEYGEN_OPERATE_PROCESSING == pkcl_state))
    {
        /* Check argument */
        if (NULL != p_ctx)
        {
            if (NULL != p_ctx->p_buf)
            {
                ret = PKCL_RET_OK;
            }
            else
            {
                ret = PKCL_RET_PARAMERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }

        if (PKCL_RET_OK == ret)
        {
            p_buf_drv = p_ctx->p_buf;

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_RsaKeyGenOperate(p_ctx, p_buf_drv);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if (PKCL_RET_OK == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_KEYGEN_OPERATE_DONE);
    }
    else if (PKCL_RET_PRIME_GENERATING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_RSA_KEYGEN_OPERATE_PROCESSING);
    }
    else if ((PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaKeyGenOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaKeyGenFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_RsaKeyGenFinal(const st_pkc_rsa_ctx_t* const p_ctx, void* const p_priv_key, void* const p_pub_key)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_RSA_KEYGEN_OPERATE_DONE == pkcl_state)
    {
        /* Check argument */
        if ((NULL != p_ctx) && (NULL != p_priv_key) && (NULL != p_pub_key))
        {
            if (NULL != p_ctx->p_buf)
            {
                ret = PKCL_RET_OK;
            }
            else
            {
                ret = PKCL_RET_PARAMERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }

        if (PKCL_RET_OK == ret)
        {
            p_buf_drv = p_ctx->p_buf;

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_RsaKeyGenFinal(p_ctx, p_buf_drv, p_priv_key, p_pub_key);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if ((PKCL_RET_OK == ret) || (PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_RsaKeyGenFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_pkc_data
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check arguments of type st_pkc_data_t
 * \param       [in] p_pkc_data             Data of type st_pkc_data_t
 * \param       [in] key_len                RSA key byte length
 * \param       [in] chk_mode               Argument check mode
 * \param       [in] arg_type               Input argument or output argument
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - For arg_type, set #PKCL_RSA_DATA_CHK_IN_ARG for input arguments and #PKCL_RSA_DATA_CHK_OUT_ARG for
 *         output arguments.
 *      - If the argument is plaintext or a digest message, set #PKCL_RSA_DATA_CHK_MODE_MSG to chk_mode.
 *      - if the argument is not plaintext or digest message, set #PKCL_RSA_DATA_CHK_MODE_OTHER to chk_mode.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If (NULL != p_pkc_data) then,
 *    -# If (NULL != p_pkc_data->p_data) then,
 *      -# If (#PKCL_RSA_DATA_CHK_IN_ARG == arg_type) then,
 *        -# If (#PKCL_RSA_DATA_CHK_MODE_MSG == chk_mode) then,
 *          -# If (p_pkc_data->len <= key_len) then,
 *            -# Set ret to #PKCL_RET_OK.
 *          -# Else
 *            -# Set ret to #PKCL_RET_LENGTHERR.
 *        -# Else if (#PKCL_RSA_DATA_CHK_MODE_OTHER == chk_mode) then,
 *          -# If (p_pkc_data->len == key_len) then,
 *            -# Set ret to #PKCL_RET_OK.
 *          -# Else
 *            -# Set ret to #PKCL_RET_LENGTHERR.
 *        -# Else
 *          -# Set ret to #PKCL_RET_INTERNALERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_INTERNALERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_pkc_data(const st_pkc_data_t* const p_pkc_data,
                                            const uint32_t key_len, const uint32_t chk_mode, const uint32_t arg_type)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if (NULL != p_pkc_data)
    {
        if (NULL != p_pkc_data->p_data)
        {
            /* Input argument or output argument */
            if (PKCL_RSA_DATA_CHK_IN_ARG == arg_type)
            {
                /* msg data check */
                if (PKCL_RSA_DATA_CHK_MODE_MSG == chk_mode)
                {
                    if (p_pkc_data->len <= key_len)
                    {
                        ret = PKCL_RET_OK;
                    }
                    else
                    {
                        ret = PKCL_RET_LENGTHERR;
                    }
                }
                else if (PKCL_RSA_DATA_CHK_MODE_OTHER == chk_mode)
                {
                    if (p_pkc_data->len == key_len)
                    {
                        ret = PKCL_RET_OK;
                    }
                    else
                    {
                        ret = PKCL_RET_LENGTHERR;
                    }
                }
                else
                {
                    ret = PKCL_RET_INTERNALERR;
                }
            }
            else if (PKCL_RSA_DATA_CHK_OUT_ARG == arg_type)
            {
                if (p_pkc_data->len >= key_len)
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_LENGTHERR;
                }
            }
            else
            {
                ret = PKCL_RET_INTERNALERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_pkc_data()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_mask_option
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of the RSA mask option.
 * \param       [in] p_mask_opt        RSA mask option structure
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - Check the hash type of mask_type by calling r_pkcl_rsa_chk_hash_type.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If (NULL != p_mask_opt) then,
 *    -# If (NULL != p_mask_opt->p_hash_info) then,
 *      -# If ((NULL != p_mask_opt->p_hash_info->p_hash_init_api) &&
 *             (NULL != p_mask_opt->p_hash_info->p_hash_update_api) &&
 *             (NULL != p_mask_opt->p_hash_info->p_hash_final_api)) then,
 *        -# Check if gthe given hash type is valid with #r_pkcl_rsa_chk_hash_type and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_mask_option(const st_pkc_rsa_mask_opt_t* const p_mask_opt)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if (NULL != p_mask_opt)
    {
        if (NULL != p_mask_opt->p_hash_info)
        {
            /* check hash info */
            if ((NULL != p_mask_opt->p_hash_info->p_hash_init_api) &&
                (NULL != p_mask_opt->p_hash_info->p_hash_update_api) &&
                (NULL != p_mask_opt->p_hash_info->p_hash_final_api))
            {
                ret = r_pkcl_rsa_chk_hash_type(p_mask_opt->mask_type);
            }
            else
            {
                ret = PKCL_RET_PARAMERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_mask_option()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_hash_type
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check if the hash type is compatible.
 * \param       [in] hash_type         Type of hash to check.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Returns #PKCL_RET_PARAMERR if non-compliant type is entered.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((#PKC_HASH_TYPE_SHA1 == hash_type) ||
 *         (#PKC_HASH_TYPE_SHA2_224     == hash_type) ||
 *         (#PKC_HASH_TYPE_SHA2_256     == hash_type) ||
 *         (#PKC_HASH_TYPE_SHA2_384     == hash_type) ||
 *         (#PKC_HASH_TYPE_SHA2_512     == hash_type) ||
 *         (#PKC_HASH_TYPE_SHA2_512_224 == hash_type) ||
 *         (#PKC_HASH_TYPE_SHA2_512_256 == hash_type) ||
 *         (#PKC_HASH_TYPE_SHA3_224     == hash_type) ||
 *         (#PKC_HASH_TYPE_SHA3_256     == hash_type) ||
 *         (#PKC_HASH_TYPE_SHA3_384     == hash_type) ||
 *         (#PKC_HASH_TYPE_SHA3_512     == hash_type)) then,
 *    -# Set ret to #PKCL_RET_OK.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_hash_type(const e_pkc_hash_type_t hash_type)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check hash type */
    if ((PKC_HASH_TYPE_SHA1 == hash_type) ||
        (PKC_HASH_TYPE_SHA2_224     == hash_type) ||
        (PKC_HASH_TYPE_SHA2_256     == hash_type) ||
        (PKC_HASH_TYPE_SHA2_384     == hash_type) ||
        (PKC_HASH_TYPE_SHA2_512     == hash_type) ||
        (PKC_HASH_TYPE_SHA2_512_224 == hash_type) ||
        (PKC_HASH_TYPE_SHA2_512_256 == hash_type) ||
        (PKC_HASH_TYPE_SHA3_224     == hash_type) ||
        (PKC_HASH_TYPE_SHA3_256     == hash_type) ||
        (PKC_HASH_TYPE_SHA3_384     == hash_type) ||
        (PKC_HASH_TYPE_SHA3_512     == hash_type))
    {
        ret = PKCL_RET_OK;
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_hash_type()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_salt
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of the salt length.
 * \param       [in] salt_len          salt length
 * \param       [in] key_len           key length
 * \param       [in] hash_type         use mask generation hash type.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the salt length is less than or equal to (key_len - hash_len - 2)
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# Set hash_len to the length of the given hash type with #r_pkcl_rsa_get_hash_len.
 *  -# If (0U != hash_len) then,
 *    -# If (salt_len <= ((key_len - hash_len) - #PKCL_RSA_SALT_LEN_CALC)) then,
 *      -# Set ret to #PKCL_RET_OK.
 *    -# Else
 *      -# Set ret to #PKCL_RET_LENGTHERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_salt(const uint32_t salt_len, const uint32_t key_len,
                                        const e_pkc_hash_type_t hash_type)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    uint32_t hash_len;

    /* hash type */
    hash_len = r_pkcl_rsa_get_hash_len(hash_type);

    if (0U != hash_len)
    {
        /* check salt_len (key_len - hash_len - 2) */
        if (salt_len <= ((key_len - hash_len) - PKCL_RSA_SALT_LEN_CALC))
        {
            ret = PKCL_RET_OK;
        }
        else
        {
            ret = PKCL_RET_LENGTHERR;
        }
    }
    else
    {
        ret = PKCL_RET_INTERNALERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_salt()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_enc_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_RsaEncryptInit.
 * \param       [in]     p_plain       A structure consisting of plaintext byte length len and data pointer p_data.
 * \param       [in]     p_pub_key     A structure consisting of one of the following byte lengths and a pointer
 *                                      to a binary. \n
 *                                      - Key Binary RSA public key \n
 *                                      - Key containing public key Binary RSA private key
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - Call #r_pkcl_rsa_chk_pkc_data to check the argument p_plain.
 *      - Call #r_pkcl_rsa_chk_mask_option to check the RSA mask option of p_ctx->p_mask_option.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_plain) && (NULL != p_pub_key) && (NULL != p_ctx)) then,
 *    -# If ((NULL != p_ctx->p_buf) &&
 *          ((#PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) ||
 *             (#PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))) then,
 *      -# If ((#PKCL_RSA_KEY_LEN_1024 == mod_len) ||
 *             (#PKCL_RSA_KEY_LEN_2048 == mod_len) ||
 *             (#PKCL_RSA_KEY_LEN_3072 == mod_len) ||
 *             (#PKCL_RSA_KEY_LEN_4096 == mod_len)) then,
 *        -# Check the arguments p_plain, mod_len with #r_pkcl_rsa_chk_pkc_data and set ret to the result.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# If ((#PKC_RSA_PADDING_NO == p_ctx->padding_option) ||
 *                 (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)) then,
 *            -# If (NULL == p_ctx->p_mask_option) then,
 *              -# If (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option) then,
 *                -# If (NULL != p_ctx->p_drbg_api) then,
 *                  -# Do nothing.
 *                -# Else
 *                  -# Set ret to #PKCL_RET_PARAMERR.
 *              -# Else
 *                -# Do nothing.
 *            -# Else
 *              -# Set ret to #PKCL_RET_PARAMERR.
 *          -# Else if (#PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option) then,
 *            -# If ((NULL != p_ctx->p_mask_option) && (NULL != p_ctx->p_drbg_api)) then,
 *              -# Check if the iven mask option is valid with #r_pkcl_rsa_chk_mask_option and set ret to the result.
 *            -# Else
 *              -# Set ret to #PKCL_RET_PARAMERR.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *        -# Else
 *          -# Do nothing.
 *      -# Else
 *        -# Set ret to #PKCL_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_enc_init_arg(const st_pkc_data_t* const p_plain,
                                                const void* const p_pub_key,
                                                const uint32_t mod_len,
                                                const st_pkc_rsa_ctx_t* const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_plain) && (NULL != p_pub_key) && (NULL != p_ctx))
    {
        if ((NULL != p_ctx->p_buf) &&
            ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
        {
            /* check mod_len */
            if ((PKCL_RSA_KEY_LEN_1024 == mod_len) || (PKCL_RSA_KEY_LEN_2048 == mod_len) ||
                (PKCL_RSA_KEY_LEN_3072 == mod_len) || (PKCL_RSA_KEY_LEN_4096 == mod_len))
            {
                /* check p_plain */
                ret = r_pkcl_rsa_chk_pkc_data(p_plain, mod_len, PKCL_RSA_DATA_CHK_MODE_MSG, PKCL_RSA_DATA_CHK_IN_ARG);

                if (PKCL_RET_OK == ret)
                {
                    /* check padding option */
                    if ((PKC_RSA_PADDING_NO == p_ctx->padding_option) ||
                        (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option))
                    {
                        /* check mask_option */
                        if (NULL == p_ctx->p_mask_option)
                        {
                            if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
                            {
                                if (NULL != p_ctx->p_drbg_api)
                                {
                                    /* Do nothing */
                                }
                                else
                                {
                                    ret = PKCL_RET_PARAMERR;
                                }
                            }
                            else
                            {
                                /* Do nothing */
                            }
                        }
                        else
                        {
                            ret = PKCL_RET_PARAMERR;
                        }
                    }
                    else if (PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option)
                    {
                        /* NULL check for OAEP */
                        if ((NULL != p_ctx->p_mask_option) && (NULL != p_ctx->p_drbg_api))
                        {
                            /* check mask_option */
                            ret = r_pkcl_rsa_chk_mask_option(p_ctx->p_mask_option);
                        }
                        else
                        {
                            ret = PKCL_RET_PARAMERR;
                        }
                    }
                    else
                    {
                        ret = PKCL_RET_PARAMERR;
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                ret = PKCL_RET_LENGTHERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_enc_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_enc_operate_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_RsaEncryptOperate.
 * \param       [in]     p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_operate_opt    Operate option selection.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - Call #r_pkcl_rsa_chk_mask_option to check the RSA mask option of p_ctx->p_mask_option.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_operate_opt)) then,
 *    -# If ((NULL != p_ctx->p_buf) and
 *          ((0U < p_operate_opt->processing_bits) and
 *           (#PKCL_PROCESSING_BITS_MAX >= p_operate_opt->processing_bits)) and
 *          ((#PKC_SYNC_OPT_SYNC == p_operate_opt->sync_opt) or
 *           (#PKC_SYNC_OPT_ASYNC == p_operate_opt->sync_opt)) and
 *          ((#PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) or
 *           (#PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))) then,
 *      -# If ((#PKC_RSA_PADDING_NO == p_ctx->padding_option) or
 *             (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)) then,
 *        -# If (NULL == p_ctx->p_mask_option) then,
 *          -# If (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option) then,
 *            -# If (NULL != p_ctx->p_drbg_api) then,
 *              -# Set ret to #PKCL_RET_OK.
 *            -# Else
 *              -# Set ret to #PKCL_RET_PARAMERR.
 *          -# Else
 *            -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else if (#PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option) then,
 *        -# If ((NULL != p_ctx->p_mask_option) and (NULL != p_ctx->p_drbg_api)) then,
 *          -# Check if the given mask option is valid with #r_pkcl_rsa_chk_mask_option and set ret to the result.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_enc_operate_arg(const st_pkc_rsa_ctx_t* const p_ctx,
                                                    const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_operate_opt))
    {
        if ((NULL != p_ctx->p_buf) &&
            ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
        {
            /* check padding option */
            if ((PKC_RSA_PADDING_NO == p_ctx->padding_option) || (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option))
            {
                /* check mask_option */
                if (NULL == p_ctx->p_mask_option)
                {
                    if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
                    {
                        if (NULL != p_ctx->p_drbg_api)
                        {
                            ret = PKCL_RET_OK;
                        }
                        else
                        {
                            ret = PKCL_RET_PARAMERR;
                        }
                    }
                    else
                    {
                        ret = PKCL_RET_OK;
                    }
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else if (PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option)
            {
                /* NULL check for OAEP */
                if ((NULL != p_ctx->p_mask_option) && (NULL != p_ctx->p_drbg_api))
                {
                    /* check mask_option */
                    ret = r_pkcl_rsa_chk_mask_option(p_ctx->p_mask_option);
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else
            {
                ret = PKCL_RET_PARAMERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_enc_operate_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_enc_final_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_RsaEncryptFinal.
 * \param       [in]     p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_cipher         Output ciphertext data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - Call #r_pkcl_rsa_chk_pkc_data to check the argument p_cipher.
 *      - Call #r_pkcl_rsa_chk_mask_option to check the RSA mask option of p_ctx->p_mask_option.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) && (NULL != p_cipher)) then,
 *    -# If ((NULL != p_ctx->p_buf) &&
 *          ((#PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) ||
 *             (#PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))) then,
 *      -# Check if the argument p_cipher is valid with #r_pkcl_rsa_chk_pkc_data and set ret to the result.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# If ((#PKC_RSA_PADDING_NO == p_ctx->padding_option) ||
 *                 (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)) then,
 *          -# If (NULL == p_ctx->p_mask_option) then,
 *            -# If (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option) then,
 *              -# If (NULL != p_ctx->p_drbg_api) then,
 *                -# Do nothing.
 *              -# Else
 *                -# Set ret to #PKCL_RET_PARAMERR.
 *            -# Else
 *              -# Do nothing.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *        -# Else if (#PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option) then,
 *          -# If ((NULL != p_ctx->p_mask_option) && (NULL != p_ctx->p_drbg_api)) then,
 *            -# Check if the given mask option is valid with #r_pkcl_rsa_chk_mask_option and set ret to the result.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else
 *        -# Do nothing.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_enc_final_arg(const st_pkc_rsa_ctx_t* const p_ctx,
                                                const st_pkc_data_t* const p_cipher)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_cipher))
    {
        if ((NULL != p_ctx->p_buf) &&
            ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
        {
            /* check p_cipher */
            ret = r_pkcl_rsa_chk_pkc_data(p_cipher, ((st_pkcl_rsa_enc_t*)p_ctx->p_buf)->mod_len,
                                            PKCL_RSA_DATA_CHK_MODE_OTHER, PKCL_RSA_DATA_CHK_OUT_ARG);
            if (PKCL_RET_OK == ret)
            {
                /* check padding option */
                if ((PKC_RSA_PADDING_NO == p_ctx->padding_option) || (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option))
                {
                    /* check mask_option */
                    if (NULL == p_ctx->p_mask_option)
                    {
                        if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
                        {
                            if (NULL != p_ctx->p_drbg_api)
                            {
                                /* Do nothing */
                            }
                            else
                            {
                                ret = PKCL_RET_PARAMERR;
                            }
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    else
                    {
                        ret = PKCL_RET_PARAMERR;
                    }
                }
                else if (PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option)
                {
                    /* NULL check for OAEP */
                    if ((NULL != p_ctx->p_mask_option) && (NULL != p_ctx->p_drbg_api))
                    {
                        /* check mask_option */
                        ret = r_pkcl_rsa_chk_mask_option(p_ctx->p_mask_option);
                    }
                    else
                    {
                        ret = PKCL_RET_PARAMERR;
                    }
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_enc_final_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_dec_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_RsaDecryptInit.
 * \param       [in]     p_cipher      A structure consisting of the ciphertext byte length len and the data pointer
 *                                      p_data.
 * \param       [in]     p_priv_key    RSA private key A structure consisting of the binary byte length len and
 *                                      a pointer to the binary.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - Call #r_pkcl_rsa_chk_pkc_data to check the argument p_cipher.
 *      - Call #r_pkcl_rsa_chk_mask_option to check the RSA mask option of p_ctx->p_mask_option.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_cipher) && (NULL != p_priv_key) && (NULL != p_ctx)) then,
 *    -# If ((NULL != p_ctx->p_buf) &&
 *          ((#PKC_BOOL_TRUE == p_ctx->is_crt_mode) || (#PKC_BOOL_FALSE == p_ctx->is_crt_mode)) &&
 *          ((#PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) ||
 *             (#PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))) then,
 *      -# If (#PKCL_RSA_KEY_LEN_1024 == mod_len) || (#PKCL_RSA_KEY_LEN_2048 == mod_len) ||
 *            (#PKCL_RSA_KEY_LEN_3072 == mod_len) || (#PKCL_RSA_KEY_LEN_4096 == mod_len)) then,
 *        -# Check if the argument p_cipher is valid with #r_pkcl_rsa_chk_pkc_data and set ret to the result.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# If ((#PKC_RSA_PADDING_NO == p_ctx->padding_option) ||
 *                 (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)) then,
 *            -# If (NULL == p_ctx->p_mask_option) then,
 *              -# Do nothing.
 *            -# Else
 *              -# Set ret to #PKCL_RET_PARAMERR.
 *          -# Else if (#PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option)
 *            -# If (NULL != p_ctx->p_mask_option) then,
 *              -# Check if the given mask option is valid with #r_pkcl_rsa_chk_mask_option and set ret to the result.
 *            -# Else
 *              -# Set ret to #PKCL_RET_PARAMERR.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *        -# Else
 *          -# Do nothing
 *      -# Else
 *        -# Set ret to #PKCL_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_dec_init_arg(const st_pkc_data_t* const p_cipher,
                                                const void* const p_priv_key,
                                                const uint32_t mod_len,
                                                const st_pkc_rsa_ctx_t* const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_cipher) && (NULL != p_priv_key) && (NULL != p_ctx))
    {
        if ((NULL != p_ctx->p_buf) &&
            ((PKC_BOOL_TRUE == p_ctx->is_crt_mode) || (PKC_BOOL_FALSE == p_ctx->is_crt_mode)) &&
            ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
        {
            /* check mod_len */
            if ((PKCL_RSA_KEY_LEN_1024 == mod_len) || (PKCL_RSA_KEY_LEN_2048 == mod_len) ||
                (PKCL_RSA_KEY_LEN_3072 == mod_len) || (PKCL_RSA_KEY_LEN_4096 == mod_len))
            {
                /* check p_cipher */
                ret = r_pkcl_rsa_chk_pkc_data(p_cipher, mod_len,
                                                PKCL_RSA_DATA_CHK_MODE_OTHER, PKCL_RSA_DATA_CHK_IN_ARG);

                if (PKCL_RET_OK == ret)
                {
                    /* check padding option */
                    if ((PKC_RSA_PADDING_NO == p_ctx->padding_option) ||
                        (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option))
                    {
                        /* check mask_option */
                        if (NULL == p_ctx->p_mask_option)
                        {
                            /* Do nothing */
                        }
                        else
                        {
                            ret = PKCL_RET_PARAMERR;
                        }
                    }
                    else if (PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option)
                    {
                        /* NULL check for OAEP */
                        if (NULL != p_ctx->p_mask_option)
                        {
                            /* check mask_option */
                            ret = r_pkcl_rsa_chk_mask_option(p_ctx->p_mask_option);
                        }
                        else
                        {
                            ret = PKCL_RET_PARAMERR;
                        }
                    }
                    else
                    {
                        ret = PKCL_RET_PARAMERR;
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                ret = PKCL_RET_LENGTHERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_dec_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_dec_operate_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_RsaDecryptOperate.
 * \param       [in]     p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_operate_opt    Operate option selection.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - Call #r_pkcl_rsa_chk_mask_option to check the RSA mask option of p_ctx->p_mask_option.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_operate_opt)) then,
 *    ((NULL != p_ctx->p_buf) and
 *     ((0U < p_operate_opt->processing_bits) and (#PKCL_PROCESSING_BITS_MAX >= p_operate_opt->processing_bits)) and
 *     ((#PKC_SYNC_OPT_SYNC == p_operate_opt->sync_opt) or (#PKC_SYNC_OPT_ASYNC == p_operate_opt->sync_opt)) and
 *     ((#PKC_BOOL_TRUE == p_ctx->is_crt_mode) or (#PKC_BOOL_FALSE == p_ctx->is_crt_mode)) and
 *     ((#PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) or (#PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))) then,
 *    -# If ((#PKC_RSA_PADDING_NO == p_ctx->padding_option) or
 *           (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)) then,
 *      -# If (NULL == p_ctx->p_mask_option) then,
 *        -# Set ret to #PKCL_RET_OK.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else if (#PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option) then,
 *      -# If (NULL != p_ctx->p_mask_option) then,
 *        -# Check if the given mask option is valid with #r_pkcl_rsa_chk_mask_option and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_dec_operate_arg(const st_pkc_rsa_ctx_t* const p_ctx,
                                                    const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_operate_opt))
    {
        if ((NULL != p_ctx->p_buf) &&
            ((PKC_BOOL_TRUE == p_ctx->is_crt_mode) || (PKC_BOOL_FALSE == p_ctx->is_crt_mode)) &&
            ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
        {
            /* check padding option */
            if ((PKC_RSA_PADDING_NO == p_ctx->padding_option) || (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option))
            {
                /* check mask_option */
                if (NULL == p_ctx->p_mask_option)
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else if (PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option)
            {
                /* NULL check for OAEP */
                if (NULL != p_ctx->p_mask_option)
                {
                    /* check mask_option */
                    ret = r_pkcl_rsa_chk_mask_option(p_ctx->p_mask_option);
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else
            {
                ret = PKCL_RET_PARAMERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_dec_operate_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_dec_final_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_RsaDecryptFinal.
 * \param       [in]     p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_plain          Output plaintext data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - Call #r_pkcl_rsa_chk_pkc_data to check the argument p_plain.
 *      - Call #r_pkcl_rsa_chk_mask_option to check the RSA mask option of p_ctx->p_mask_option.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) && (NULL != p_plain)) then,
 *    -# If ((NULL != p_ctx->p_buf) &&
 *          ((#PKC_BOOL_TRUE == p_ctx->is_crt_mode) || (#PKC_BOOL_FALSE == p_ctx->is_crt_mode)) &&
 *          ((#PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) ||
 *             (#PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))) then,
 *      -# Check if the argument p_plain is valid with #r_pkcl_rsa_chk_pkc_data and set ret to the result.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# If ((#PKC_RSA_PADDING_NO == p_ctx->padding_option) ||
 *                 (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)) then,
 *          -# If (NULL == p_ctx->p_mask_option) then,
 *            -# Do nothing.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *        -# Else if (#PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option) then,
 *          -# If (NULL != p_ctx->p_mask_option) then,
 *            -# Check if the given mask option is valid with #r_pkcl_rsa_chk_mask_option and set ret to the result.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else
 *        -# Do nothing.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_dec_final_arg(const st_pkc_rsa_ctx_t* const p_ctx,
                                                const st_pkc_data_t* const p_plain)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_plain))
    {
        if ((NULL != p_ctx->p_buf) &&
            ((PKC_BOOL_TRUE == p_ctx->is_crt_mode) || (PKC_BOOL_FALSE == p_ctx->is_crt_mode)) &&
            ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
        {
            /* check p_plain */
            ret = r_pkcl_rsa_chk_pkc_data(p_plain, ((st_pkcl_rsa_dec_t*)p_ctx->p_buf)->mod_len,
                                            PKCL_RSA_DATA_CHK_MODE_MSG, PKCL_RSA_DATA_CHK_OUT_ARG);

            if (PKCL_RET_OK == ret)
            {
                /* check padding option */
                if ((PKC_RSA_PADDING_NO == p_ctx->padding_option) || (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option))
                {
                    /* check mask_option */
                    if (NULL == p_ctx->p_mask_option)
                    {
                        /* Do nothing */
                    }
                    else
                    {
                        ret = PKCL_RET_PARAMERR;
                    }
                }
                else if (PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option)
                {
                    /* NULL check for OAEP */
                    if (NULL != p_ctx->p_mask_option)
                    {
                        /* check mask_option */
                        ret = r_pkcl_rsa_chk_mask_option(p_ctx->p_mask_option);
                    }
                    else
                    {
                        ret = PKCL_RET_PARAMERR;
                    }
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_dec_final_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_sign_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_RsaSignInit.
 * \param       [in]     p_digest      A structure consisting of the byte length len of the message digest and
 *                                      the data pointer p_data.
 * \param       [in]     p_priv_key    RSA private key A structure consisting of the binary byte length len and
 *                                      a pointer to the binary.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - Call #r_pkcl_rsa_chk_pkc_data to check the argument p_digest.
 *      - Call #r_pkcl_rsa_chk_hash_type to check the message digest hash type.
 *      - Call #r_pkcl_rsa_chk_mask_option to check the RSA mask option of p_ctx->p_mask_option.
 *      - Call #r_pkcl_rsa_chk_salt to check the salt length of p_ctx->p_mask_option->salt_len.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_digest) && (NULL != p_priv_key) && (NULL != p_ctx)) then,
 *    -# ((NULL != p_ctx->p_buf) &&
 *        ((#PKC_BOOL_TRUE == p_ctx->is_crt_mode) || (#PKC_BOOL_FALSE == p_ctx->is_crt_mode)) &&
 *        ((#PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) ||
 *           (#PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))) then,
 *      -# If ((#PKCL_RSA_KEY_LEN_1024 == mod_len) || (#PKCL_RSA_KEY_LEN_2048 == mod_len) ||
 *             (#PKCL_RSA_KEY_LEN_3072 == mod_len) || (#PKCL_RSA_KEY_LEN_4096 == mod_len)) then,
 *        -# Check if the argument p_digest is valid with #r_pkcl_rsa_chk_pkc_data and set ret to the result.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# If ((#PKC_RSA_PADDING_NO == p_ctx->padding_option) ||
 *                 (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)) then,
 *            -# If (NULL == p_ctx->p_mask_option) then,
 *              -# If (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option) then,
 *                -# Check if the hash type is valid with #r_pkcl_rsa_chk_hash_type and set ret to the result.
 *              -# Else
 *                -# Do nothing.
 *            -# Else
 *              -# Set ret to #PKCL_RET_PARAMERR.
 *          -# Else if (#PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option) then,
 *            -# If ((NULL != p_ctx->p_mask_option) && (NULL != p_ctx->p_drbg_api)) then,
 *              -# Check if the given mask option is valid with #r_pkcl_rsa_chk_mask_option and set ret to the result.
 *              -# If #PKCL_RET_OK == ret then,
 *                -# Check if the given salt length is valid with #r_pkcl_rsa_chk_salt and set ret to the result.
 *              -# Else
 *                -# Do nothing.
 *            -# Else
 *              -# Set ret to #PKCL_RET_PARAMERR.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *        -# Else
 *          -# Do nothing.
 *      -# Else
 *        -# Set ret to #PKCL_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_sign_init_arg(const st_pkc_data_t* const p_digest,
                                                const void* const p_priv_key,
                                                const uint32_t mod_len,
                                                const st_pkc_rsa_ctx_t* const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_digest) && (NULL != p_priv_key) && (NULL != p_ctx))
    {
        if ((NULL != p_ctx->p_buf) &&
            ((PKC_BOOL_TRUE == p_ctx->is_crt_mode) || (PKC_BOOL_FALSE == p_ctx->is_crt_mode)) &&
            ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
        {
            /* check mod_len */
            if ((PKCL_RSA_KEY_LEN_1024 == mod_len) || (PKCL_RSA_KEY_LEN_2048 == mod_len) ||
                (PKCL_RSA_KEY_LEN_3072 == mod_len) || (PKCL_RSA_KEY_LEN_4096 == mod_len))
                {
                /* check p_digest */
                ret = r_pkcl_rsa_chk_pkc_data(p_digest, mod_len, PKCL_RSA_DATA_CHK_MODE_MSG, PKCL_RSA_DATA_CHK_IN_ARG);

                if (PKCL_RET_OK == ret)
                {
                    /* check padding option */
                    if ((PKC_RSA_PADDING_NO == p_ctx->padding_option) ||
                        (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option))
                    {
                        /* check mask_option */
                        if (NULL == p_ctx->p_mask_option)
                        {
                            if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
                            {
                                ret = r_pkcl_rsa_chk_hash_type(p_ctx->msg_digest_type);
                            }
                            else
                            {
                                /* Do nothing */
                            }
                        }
                        else
                        {
                            ret = PKCL_RET_PARAMERR;
                        }
                    }
                    else if (PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option)
                    {
                        /* NULL check for PSS */
                        if ((NULL != p_ctx->p_mask_option) && (NULL != p_ctx->p_drbg_api))
                        {
                            /* check mask_option */
                            ret = r_pkcl_rsa_chk_mask_option(p_ctx->p_mask_option);

                            if (PKCL_RET_OK == ret)
                            {
                                ret = r_pkcl_rsa_chk_salt(p_ctx->p_mask_option->salt_len, mod_len,
                                                            p_ctx->p_mask_option->mask_type);
                            }
                            else
                            {
                                /* Do nothing */
                            }
                        }
                        else
                        {
                            ret = PKCL_RET_PARAMERR;
                        }
                    }
                    else
                    {
                        ret = PKCL_RET_PARAMERR;
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                ret = PKCL_RET_LENGTHERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_sign_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_sign_operate_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_RsaSignOperate.
 * \param       [in]     p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_operate_opt    Operate option selection.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - Call #r_pkcl_rsa_chk_hash_type to check the message digest hash type.
 *      - Call #r_pkcl_rsa_chk_mask_option to check the RSA mask option of p_ctx->p_mask_option.
 *      - Call #r_pkcl_rsa_chk_salt to check the salt length of p_ctx->p_mask_option->salt_len.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) && (NULL != p_operate_opt))
 *    -# If ((NULL != p_ctx->p_buf) and
 *          ((0U < p_operate_opt->processing_bits) and
 *           (#PKCL_PROCESSING_BITS_MAX >= p_operate_opt->processing_bits)) and
 *          ((#PKC_SYNC_OPT_SYNC == p_operate_opt->sync_opt) or (PKC_SYNC_OPT_ASYNC == p_operate_opt->sync_opt)) and
 *          ((#PKC_BOOL_TRUE == p_ctx->is_crt_mode) or (#PKC_BOOL_FALSE == p_ctx->is_crt_mode)) and
 *          ((#PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) or (#PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
 *      -# If (#PKC_RSA_PADDING_NO == p_ctx->padding_option)
 *        -# If (NULL == p_ctx->p_mask_option)
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else if (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
 *        -# If (NULL == p_ctx->p_mask_option)
 *          -# Check the argument p_ctx->msg_digest_type with #r_pkcl_rsa_chk_hash_type,
 *             and set ret to the return value.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else if (#PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option)
 *        -# If ((NULL != p_ctx->p_mask_option) and (NULL != p_ctx->p_drbg_api))
 *          -# Check the argument p_ctx->p_mask_option with #r_pkcl_rsa_chk_mask_option,
 *               and set ret to the return value.
 *            -# If (#PKCL_RET_OK == ret)
 *              -# Check the argument p_ctx->p_mask_option->salt_len with #r_pkcl_rsa_chk_salt,
 *                 and set ret to the return value.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_sign_operate_arg(const st_pkc_rsa_ctx_t* const p_ctx,
                                                    const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_operate_opt))
    {
        if ((NULL != p_ctx->p_buf) &&
            ((PKC_BOOL_TRUE == p_ctx->is_crt_mode) || (PKC_BOOL_FALSE == p_ctx->is_crt_mode)) &&
            ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
        {
            /* check padding option */
            if (PKC_RSA_PADDING_NO == p_ctx->padding_option)
            {
                /* check mask_option */
                if (NULL == p_ctx->p_mask_option)
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
            {
                if (NULL == p_ctx->p_mask_option)
                {
                    ret = r_pkcl_rsa_chk_hash_type(p_ctx->msg_digest_type);
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else if (PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option)
            {
                /* NULL check for PSS */
                if ((NULL != p_ctx->p_mask_option) && (NULL != p_ctx->p_drbg_api))
                {
                    /* check mask_option */
                    ret = r_pkcl_rsa_chk_mask_option(p_ctx->p_mask_option);

                    if (PKCL_RET_OK == ret)
                    {
                        ret = r_pkcl_rsa_chk_salt(p_ctx->p_mask_option->salt_len,
                            ((st_pkcl_rsa_sign_t*)p_ctx->p_buf)->mod_len, /* No problem with casting to pointer size */
                            p_ctx->p_mask_option->mask_type);
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else
            {
                ret = PKCL_RET_PARAMERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_sign_operate_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_sign_final_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_RsaSignFinal.
 * \param       [in]     p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_sign           A structure consisting of the byte length len of the data concatenated
 *                                         with the signature values R and S and the data pointer p_data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - Call #r_pkcl_rsa_chk_pkc_data to check the argument p_sign.
 *      - Call #r_pkcl_rsa_chk_hash_type to check the message digest hash type.
 *      - Call #r_pkcl_rsa_chk_mask_option to check the RSA mask option of p_ctx->p_mask_option.
 *      - Call #r_pkcl_rsa_chk_salt to check the salt length of p_ctx->p_mask_option->salt_len.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) && (NULL != p_sign))
 *    -# If ((NULL != p_ctx->p_buf) &&
 *          ((#PKC_BOOL_TRUE == p_ctx->is_crt_mode) or (#PKC_BOOL_FALSE == p_ctx->is_crt_mode)) and
 *          ((#PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) or (#PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
 *      -# Check the argument p_sign with #r_pkcl_rsa_chk_pkc_data, and set ret to the return value.
 *      -# If (#PKCL_RET_OK == ret)
 *        -# If ((#PKC_RSA_PADDING_NO == p_ctx->padding_option) or (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option))
 *          -# If (NULL == p_ctx->p_mask_option)
 *            -# If (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
 *              -# Check the argument p_ctx->msg_digest_type with #r_pkcl_rsa_chk_hash_type,
 *                 and set ret to the return value.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *        -# Else if (#PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option)
 *          -# If ((NULL != p_ctx->p_mask_option) and (NULL != p_ctx->p_drbg_api))
 *            -# Check the argument p_ctx->p_mask_option with #r_pkcl_rsa_chk_mask_option,
 *               and set ret to the return value.
 *            -# If (#PKCL_RET_OK == ret)
 *              -# Check the argument p_ctx->p_mask_option->salt_len with #r_pkcl_rsa_chk_salt,
 *                 and set ret to the return value.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_sign_final_arg(const st_pkc_rsa_ctx_t* const p_ctx,
                                                const st_pkc_data_t* const p_sign)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_sign))
    {
        if ((NULL != p_ctx->p_buf) &&
            ((PKC_BOOL_TRUE == p_ctx->is_crt_mode) || (PKC_BOOL_FALSE == p_ctx->is_crt_mode)) &&
            ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
        {
            /* check p_sign */
            ret = r_pkcl_rsa_chk_pkc_data(p_sign, ((st_pkcl_rsa_sign_t*)p_ctx->p_buf)->mod_len,
                                            PKCL_RSA_DATA_CHK_MODE_OTHER, PKCL_RSA_DATA_CHK_OUT_ARG);

            if (PKCL_RET_OK == ret)
            {
                /* check padding option */
                if ((PKC_RSA_PADDING_NO == p_ctx->padding_option) || (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option))
                {
                    /* check mask_option */
                    if (NULL == p_ctx->p_mask_option)
                    {
                        if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
                        {
                            ret = r_pkcl_rsa_chk_hash_type(p_ctx->msg_digest_type);
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    else
                    {
                        ret = PKCL_RET_PARAMERR;
                    }
                }
                else if (PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option)
                {
                    /* NULL check for PSS */
                    if ((NULL != p_ctx->p_mask_option) && (NULL != p_ctx->p_drbg_api))
                    {
                        /* check mask_option */
                        ret = r_pkcl_rsa_chk_mask_option(p_ctx->p_mask_option);

                        if (PKCL_RET_OK == ret)
                        {
                            ret = r_pkcl_rsa_chk_salt(p_ctx->p_mask_option->salt_len,
                            ((st_pkcl_rsa_sign_t*)p_ctx->p_buf)->mod_len, /* No problem with casting to pointer size */
                            p_ctx->p_mask_option->mask_type);
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    else
                    {
                        ret = PKCL_RET_PARAMERR;
                    }
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_sign_final_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_verify_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_RsaVerifyInit.
 * \param       [in]     p_digest      A structure consisting of the byte length len of the message digest and the
 *                                      data pointer p_data.
 * \param       [in]     p_sign        A structure consisting of the byte length len of the signature to be verified
 *                                      and the data pointer p_data.
 * \param       [in]     p_pub_key     A structure consisting of one of the following byte lengths and a pointer
 *                                      to a binary. \n
 *                                      - Key Binary RSA public key \n
 *                                      - Key containing public key Binary RSA private key
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - Call #r_pkcl_rsa_chk_pkc_data to check the argument p_digest and p_sign.
 *      - Call #r_pkcl_rsa_chk_hash_type to check the message digest hash type.
 *      - Call #r_pkcl_rsa_chk_mask_option to check the RSA mask option of p_ctx->p_mask_option.
 *      - Call #r_pkcl_rsa_chk_salt to check the salt length of p_ctx->p_mask_option->salt_len.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_digest) && (NULL != p_sign) && (NULL != p_pub_key) && (NULL != p_ctx))
 *    -# If ((NULL != p_ctx->p_buf) and
 *          ((#PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) or (#PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
 *      -# If ((#PKCL_RSA_KEY_LEN_1024 == mod_len) or (#PKCL_RSA_KEY_LEN_2048 == mod_len) or
 *             (#PKCL_RSA_KEY_LEN_3072 == mod_len) or (#PKCL_RSA_KEY_LEN_4096 == mod_len))
 *        -# Check the argument p_digest with #r_pkcl_rsa_chk_pkc_data, and set ret to the return value.
 *        -# If (#PKCL_RET_OK == ret)
 *          -# Check the argument p_sign with #r_pkcl_rsa_chk_pkc_data, and set ret to the return value.
 *      -# Else
 *        -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# If (#PKCL_RET_OK == ret)
 *        -# If ((#PKC_RSA_PADDING_NO == p_ctx->padding_option) or (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option))
 *          -# If (NULL == p_ctx->p_mask_option)
 *            -# If (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
 *              -# Check the argument p_ctx->msg_digest_type, and set ret to the return value.
 *          -# Else
 *            -# Set ret to #PKCL_RET_LENGTHERR into ret.
 *        -# Else if (#PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option)
 *          -# If (NULL != p_ctx->p_mask_option)
 *            -# Check the argument p_ctx->p_mask_option with #r_pkcl_rsa_chk_mask_option,
 *               and set ret to the return value.
 *              -# If (#PKCL_RET_OK == ret)
 *                -# Check the argument p_ctx->p_mask_option->salt_len with #r_pkcl_rsa_chk_salt,
 *                   and set ret to the return value.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_verify_init_arg(const st_pkc_data_t* const p_digest,
                                                    const st_pkc_data_t* const p_sign,
                                                    const void* const p_pub_key,
                                                    const uint32_t mod_len,
                                                    const st_pkc_rsa_ctx_t* const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_digest) && (NULL != p_sign) && (NULL != p_pub_key) && (NULL != p_ctx))
    {
        if ((NULL != p_ctx->p_buf) &&
            ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
        {
            /* check mod_len */
            if ((PKCL_RSA_KEY_LEN_1024 == mod_len) || (PKCL_RSA_KEY_LEN_2048 == mod_len) ||
                (PKCL_RSA_KEY_LEN_3072 == mod_len) || (PKCL_RSA_KEY_LEN_4096 == mod_len))
            {
                /* check p_digest */
                ret = r_pkcl_rsa_chk_pkc_data(p_digest, mod_len, PKCL_RSA_DATA_CHK_MODE_MSG, PKCL_RSA_DATA_CHK_IN_ARG);

                if (PKCL_RET_OK == ret)
                {
                    /* check p_sign */
                    ret = r_pkcl_rsa_chk_pkc_data(p_sign, mod_len, PKCL_RSA_DATA_CHK_MODE_OTHER,
                                                    PKCL_RSA_DATA_CHK_IN_ARG);
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                ret = PKCL_RET_LENGTHERR;
            }

            if (PKCL_RET_OK == ret)
            {
                /* check padding option */
                if ((PKC_RSA_PADDING_NO == p_ctx->padding_option) || (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option))
                {
                    /* check mask_option */
                    if (NULL == p_ctx->p_mask_option)
                    {
                        if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
                        {
                            ret = r_pkcl_rsa_chk_hash_type(p_ctx->msg_digest_type);
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    else
                    {
                        ret = PKCL_RET_PARAMERR;
                    }
                }
                else if (PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option)
                {
                    /* NULL check for PSS */
                    if (NULL != p_ctx->p_mask_option)
                    {
                        /* check mask_option */
                        ret = r_pkcl_rsa_chk_mask_option(p_ctx->p_mask_option);

                        if (PKCL_RET_OK == ret)
                        {
                            ret = r_pkcl_rsa_chk_salt(p_ctx->p_mask_option->salt_len, mod_len,
                                                        p_ctx->p_mask_option->mask_type);
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    else
                    {
                        ret = PKCL_RET_PARAMERR;
                    }
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_verify_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_verify_operate_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_RsaVerifyOperate.
 * \param       [in]     p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_operate_opt    Operate option selection.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - Call #r_pkcl_rsa_chk_hash_type to check the message digest hash type.
 *      - Call #r_pkcl_rsa_chk_mask_option to check the RSA mask option of p_ctx->p_mask_option.
 *      - Call #r_pkcl_rsa_chk_salt to check the salt length of p_ctx->p_mask_option->salt_len.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_operate_opt))
 *    -# If ((NULL != p_ctx->p_buf) and
 *           ((0U < p_operate_opt->processing_bits) and
 *            (#PKCL_PROCESSING_BITS_MAX >= p_operate_opt->processing_bits)) and
 *           ((#PKC_SYNC_OPT_SYNC == p_operate_opt->sync_opt) or (#PKC_SYNC_OPT_ASYNC == p_operate_opt->sync_opt)) and
 *           ((#PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) or (#PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
 *      -# If (#PKC_RSA_PADDING_NO == p_ctx->padding_option)
 *        -# If (NULL == p_ctx->p_mask_option)
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else if (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
 *        -# If (NULL == p_ctx->p_mask_option)
 *          -# Check the argument p_ctx->msg_digest_type with #r_pkcl_rsa_chk_hash_type,
 *             and set ret to the return value.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else if (#PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option)
 *        -# If (NULL != p_ctx->p_mask_option)
 *          -# Check the argument p_ctx->p_mask_option with #r_pkcl_rsa_chk_mask_option.
 *          -# If (#PKCL_RET_OK == ret)
 *            -# Check the argument p_ctx->p_mask_option->salt_len with #r_pkcl_rsa_chk_salt.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_verify_operate_arg(const st_pkc_rsa_ctx_t* const p_ctx,
                                                    const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_operate_opt))
    {
        if ((NULL != p_ctx->p_buf) &&
            ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
        {
            /* check padding option */
            if (PKC_RSA_PADDING_NO == p_ctx->padding_option)
            {
                /* check mask_option */
                if (NULL == p_ctx->p_mask_option)
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
            {
                if (NULL == p_ctx->p_mask_option)
                {
                    ret = r_pkcl_rsa_chk_hash_type(p_ctx->msg_digest_type);
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else if (PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option)
            {
                /* NULL check for PSS */
                if (NULL != p_ctx->p_mask_option)
                {
                    /* check mask_option */
                    ret = r_pkcl_rsa_chk_mask_option(p_ctx->p_mask_option);

                    if (PKCL_RET_OK == ret)
                    {
                        ret = r_pkcl_rsa_chk_salt(p_ctx->p_mask_option->salt_len,
                        ((st_pkcl_rsa_verify_t*)p_ctx->p_buf)->mod_len, /* No problem with casting to pointer size */
                        p_ctx->p_mask_option->mask_type);
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else
            {
                ret = PKCL_RET_PARAMERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_verify_operate_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_chk_verify_final_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_RsaVerifyFinal.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by EC.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the argument is not NULL.
 *      - Call #r_pkcl_rsa_chk_hash_type to check the message digest hash type.
 *      - Call #r_pkcl_rsa_chk_mask_option to check the RSA mask option of p_ctx->p_mask_option.
 *      - Call #r_pkcl_rsa_chk_salt to check the salt length of p_ctx->p_mask_option->salt_len.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If (NULL != p_ctx)
 *    -# If ((NULL != p_ctx->p_buf) and
 *           ((#PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) or (#PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
 *      -# If (#PKC_RSA_PADDING_NO == p_ctx->padding_option)
 *        -# If (NULL == p_ctx->p_mask_option)
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else if (#PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
 *        -# If (NULL == p_ctx->p_mask_option)
 *          -# Check the argument p_ctx->msg_digest_type with #r_pkcl_rsa_chk_hash_type,
 *             and set ret to the return value.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else if (#PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option)
 *        -# If (NULL != p_ctx->p_mask_option)
 *          -# Check the argument p_ctx->p_mask_option with #r_pkcl_rsa_chk_mask_option,
 *             and set ret to the return value.
 *          -# If (#PKCL_RET_OK == ret)
 *            -# Check the argument p_ctx->p_mask_option->salt_len with #r_pkcl_rsa_chk_salt.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_chk_verify_final_arg(const st_pkc_rsa_ctx_t* const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if (NULL != p_ctx)
    {
        if ((NULL != p_ctx->p_buf) &&
            ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode)))
        {
            /* check padding option */
            if (PKC_RSA_PADDING_NO == p_ctx->padding_option)
            {
                /* check mask_option */
                if (NULL == p_ctx->p_mask_option)
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
            {
                if (NULL == p_ctx->p_mask_option)
                {
                    ret = r_pkcl_rsa_chk_hash_type(p_ctx->msg_digest_type);
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else if (PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option)
            {
                /* NULL check for PSS */
                if (NULL != p_ctx->p_mask_option)
                {
                    /* check mask_option */
                    ret = r_pkcl_rsa_chk_mask_option(p_ctx->p_mask_option);

                    if (PKCL_RET_OK == ret)
                    {
                        ret = r_pkcl_rsa_chk_salt(p_ctx->p_mask_option->salt_len,
                        ((st_pkcl_rsa_verify_t*)p_ctx->p_buf)->mod_len, /* No problem with casting to pointer size */
                        p_ctx->p_mask_option->mask_type);
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    ret = PKCL_RET_PARAMERR;
                }
            }
            else
            {
                ret = PKCL_RET_PARAMERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_chk_verify_final_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_get_hash_len
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Get hash byte lengh.
 * \param       [in]   hash_type            Hash type
 * \return
 *  - Hash byte length of the specified type
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Get hash byte lengh of specified type.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# If (#PKC_HASH_TYPE_SHA1 == hash_type)
 *    -# Set #PKCL_RSA_HASH_LEN_SHA1 into the variable hash_len.
 *  -# Else if ((#PKC_HASH_TYPE_SHA2_224 == hash_type) or (#PKC_HASH_TYPE_SHA2_512_224 == hash_type) or
 *              (#PKC_HASH_TYPE_SHA3_224 == hash_type))
 *    -# Set #PKCL_RSA_HASH_LEN_SHA224 into hash_len.
 *  -# Else if ((#PKC_HASH_TYPE_SHA2_256 == hash_type) or (#PKC_HASH_TYPE_SHA2_512_256 == hash_type) or
 *              (#PKC_HASH_TYPE_SHA3_256 == hash_type))
 *    -# Set #PKCL_RSA_HASH_LEN_SHA256 into hash_len.
 *  -# Else if ((#PKC_HASH_TYPE_SHA2_384 == hash_type) or (#PKC_HASH_TYPE_SHA3_384 == hash_type))
 *    -# Set #PKCL_RSA_HASH_LEN_SHA384 into hash_len.
 *  -# Else if ((#PKC_HASH_TYPE_SHA2_512 == hash_type) or (#PKC_HASH_TYPE_SHA3_512 == hash_type))
 *    -# Set #PKCL_RSA_HASH_LEN_SHA512 into hash_len.
 *  -# Else
 *    -# Set 0 into hash_len.
 *  -# Return hash_len.
 *
 * \callgraph
 *********************************************************************************************************************/
static uint32_t r_pkcl_rsa_get_hash_len(const e_pkc_hash_type_t hash_type)
{
    uint32_t hash_len;

    if (PKC_HASH_TYPE_SHA1 == hash_type)
    {
        hash_len = PKCL_RSA_HASH_LEN_SHA1;
    }
    else if ((PKC_HASH_TYPE_SHA2_224     == hash_type) ||
            (PKC_HASH_TYPE_SHA2_512_224 == hash_type) ||
            (PKC_HASH_TYPE_SHA3_224     == hash_type))
    {
        hash_len = PKCL_RSA_HASH_LEN_SHA224;
    }
    else if ((PKC_HASH_TYPE_SHA2_256     == hash_type) ||
            (PKC_HASH_TYPE_SHA2_512_256 == hash_type) ||
            (PKC_HASH_TYPE_SHA3_256     == hash_type))
    {
        hash_len = PKCL_RSA_HASH_LEN_SHA256;
    }
    else if ((PKC_HASH_TYPE_SHA2_384 == hash_type) ||
            (PKC_HASH_TYPE_SHA3_384 == hash_type))
    {
        hash_len = PKCL_RSA_HASH_LEN_SHA384;
    }
    else if ((PKC_HASH_TYPE_SHA2_512 == hash_type) ||
            (PKC_HASH_TYPE_SHA3_512 == hash_type))
    {
        hash_len = PKCL_RSA_HASH_LEN_SHA512;
    }
    else
    {
        hash_len = 0U;
    }

    return hash_len;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_get_hash_len()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_padding_no
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Padding process when RSA padding scheme is not used.
 * \param       [in]     p_plain       A structure consisting of plaintext byte length len and data pointer p_data.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [out]    p_em          Output encode message.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - If the message size is smaller than mod_len, embed 0 at the beginning of the message.
 *      - If the message size is the same as mod_len, copy it to p_em as it is.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set 0 into the variable pad_loop.
 *  -# Set (mod_len - p_plain->len) into the variable padding_len.
 *  -# If (mod_len > p_plain->len)
 *    -# Loop while (pad_loop < padding_len)
 *      -# Set 0x00 into p_em->p_data[pad_loop].
 *      -# Increment pad_loop.
 *  -# Concatenate p_em->p_data[padding_len] with p_plain->p_data #r_pkcl_cmn_cpy_byte.
 *
 * \callgraph
 *********************************************************************************************************************/
static void r_pkcl_rsa_padding_no(const st_pkc_data_t* const p_plain, const uint32_t mod_len,
                                    const st_pkc_data_t* const p_em)
{
    uint32_t padding_len;
    uint32_t pad_loop = 0U;

    /* Calculate the size you need to pad */
    padding_len = (mod_len - p_plain->len);

    if (mod_len > p_plain->len)
    {
        while (pad_loop < padding_len)
        {
            p_em->p_data[pad_loop] = 0x00U;
            pad_loop++;
        }
    }
    else
    {
        /* Do nothing */
    }

    r_pkcl_cmn_cpy_byte(&p_em->p_data[padding_len], p_plain->p_data, p_plain->len);

}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_padding_no()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_enc_scheme_pkcs1
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Perform the EME-PKCS1-v1_5 encoding scheme for RSA encryption.
 * \param       [in]     p_plain       A structure consisting of plaintext byte length len and data pointer p_data.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in,out] p_em          Output encode message.
 * \param       [in]     p_check_ps    A pointer to PS. If p_check_em is not NULL, PS is obtained from this parameter.
 *                                     This parameter is used when verifying the validity of the decryption operation.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRBGAPIERR:               External DRBG API returned error
 *  - #PKCL_RET_RSA_SCHEMEERR:            RSA scheme processing failed
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Call the external DRBG API to generate PS (padding string).
 *        If the generated PS contains 0x00, repeat random number generation until it becomes a number other than 0x00.
 *      - Create EM (encode message) by concatenating fixed value padding, PS, and message.
 * \par Remarks
 *  - If the argument p_check_ps is not null, use the value of p_check_ps for PS.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((p_plain->len + #PKCL_RSA_EME_PKCS_PAD_LEN_MIN) <= mod_len)
 *    -# Set the adress of p_em->p_data[PKCL_RSA_PS_OFFSET] into p_ps.
 *    -# Set ((mod_len - p_plain->len) - #PKCL_RSA_PKCS_PS_LEN_CALC) into ps_len.
 *    -# If (NULL != p_check_em)
 *      -# Get the PS value from p_check_em and set p_ps to it with # r_pkcl_cmn_cpy_byte.
 *      -# Set external_ret to #PKCL_EXTERNAL_API_OK.
 *    -# Else
 *      -# Generate a pseudo-random bit sequence with p_ctx->p_drbg_api and set the generated
 *         pseudo-random value into p_ps, and set the return value into external_ret.
 *    -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *      -# Loop while ((ps_loop < ps_len) and (#PKCL_EXTERNAL_API_OK == external_ret) and (#PKCL_RET_OK == ret))
 *        -# Set 0 into zero_loop.
 *        -# Loop while ((p_ps[ps_loop] == 0) and (zero_loop < #PKCL_RSA_PKCS_PS_ZERO_LOOP_MAX) and
 *                       (#PKCL_EXTERNAL_API_OK == external_ret))
 *          -# Generate a pseudo-random bit sequence with p_ctx->p_drbg_api and set the generated
 *             pseudo-random value into p_ps[ps_loop], and set the return value into external_ret.
 *          -# Increment zero_loop.
 *        -# If (#PKCL_RSA_PKCS_PS_ZERO_LOOP_MAX > zero_loop)
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_RSA_SCHEMEERR.
 *        -# Increment ps_loop.
 *      -# If (#PKCL_RET_OK == ret)
 *        -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *          -# Set 0x00 into p_em->p_data[0].
 *          -# Set #PKCL_RSA_PKCS_PADDING_NUM into p_em->p_data[1].
 *          -# Set 0x00 into p_em->p_data[#PKCL_RSA_PS_OFFSET + ps_len]
 *          -# Concatenate p_plain->p_data with p_em->p_data[#PKCL_RSA_PS_OFFSET + ps_len + 1]
 *             with #r_pkcl_cmn_cpy_byte.
 *        -# Else
 *          -# Set ret to #PKCL_RET_DRBGAPIERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_DRBGAPIERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_LENGTHERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_enc_scheme_pkcs1(const st_pkc_data_t* const p_plain, const st_pkc_rsa_ctx_t* const p_ctx,
                                                const uint32_t mod_len, st_pkc_data_t* const p_em,
                                                const uint8_t* const p_check_ps)
{
    pkcl_ret_t ret = PKCL_RET_OK;
    uint32_t external_ret;
    uint8_t * p_ps;
    uint32_t ps_len;
    uint32_t ps_loop = 0U;
    uint32_t zero_loop;

    /* message size check */
    if ((p_plain->len + PKCL_RSA_EME_PKCS_PAD_LEN_MIN) <= mod_len)
    {
        /* Set PS */
        p_ps = &p_em->p_data[PKCL_RSA_PS_OFFSET];
        ps_len = ((mod_len - p_plain->len) - PKCL_RSA_PKCS_PS_LEN_CALC);

        /* Generate PS */
        if (NULL != p_check_ps)
        {
            r_pkcl_cmn_cpy_byte(p_ps, p_check_ps, ps_len);
            external_ret = PKCL_EXTERNAL_API_OK;
        }
        else
        {
            external_ret = p_ctx->p_drbg_api(ps_len, p_ps);
        }

        if (PKCL_EXTERNAL_API_OK == external_ret)
        {
            /* Transform zero bytes (if any) to non-zero random bytes */
            while ((ps_loop < ps_len) && (PKCL_EXTERNAL_API_OK == external_ret) && (PKCL_RET_OK == ret))
            {
                zero_loop = 0U;

                /* No problem for casting to support usual arithmetic conversion */
                while (((uint32_t)p_ps[ps_loop] == 0U) && (zero_loop < PKCL_RSA_PKCS_PS_ZERO_LOOP_MAX) &&
                        (PKCL_EXTERNAL_API_OK == external_ret))
                {
                    external_ret = p_ctx->p_drbg_api(1U, &p_ps[ps_loop]);
                    zero_loop++;
                }
                if (PKCL_RSA_PKCS_PS_ZERO_LOOP_MAX > zero_loop)
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_RSA_SCHEMEERR;
                }
                ps_loop++;
            }

            if (PKCL_RET_OK == ret)
            {
                if (PKCL_EXTERNAL_API_OK == external_ret)
                {
                    /* Create an EM */
                    p_em->p_data[0] = 0x00U;
                    p_em->p_data[1] = PKCL_RSA_PKCS_PADDING_NUM;
                    p_em->p_data[PKCL_RSA_PS_OFFSET + ps_len] = 0x00U;
                    r_pkcl_cmn_cpy_byte(&p_em->p_data[PKCL_RSA_PS_OFFSET + ps_len + 1U],
                                        p_plain->p_data, p_plain->len);
                }
                else
                {
                    ret = PKCL_RET_DRBGAPIERR;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCL_RET_DRBGAPIERR;
        }
    }
    else
    {
        ret = PKCL_RET_LENGTHERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_enc_scheme_pkcs1()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_enc_scheme_oaep
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Perform the EME-OAEP encoding scheme for RSA encryption
 * \param       [in]     p_plain       A structure consisting of plaintext byte length len and data pointer p_data.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in,out] p_em          Output encode message.
 * \param       [in]     p_check_seed  A pointer to the random seed. If p_check_seed is not NULL,
 *                                     seed value is obtained from this parameter.
 *                                     This parameter is used when verifying the validity of the decryption operation.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_HASHAPIERR:               External HASH API returned error
 *  - #PKCL_RET_DRBGAPIERR:               External DRBG API returned error
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Calculate Lhash by calling the external HASH API.
 *      - Create DB by concatenating Lhash, PS, fixed value padding, and messages.
 *      - Call the external DRBG API to generate a seed.
 *      - Call #r_pkcl_rsa_mgf1 to generate a mask from the DB and seed, and xor the generated mask and DB,
 *         or seed to generate maskedSeed and maskedDB.
 *      - Create EM (encode message) by concatenating 0x00 with maskedSeed and maskedDB.
 * \par Remarks
 *  - If the argument p_check_seed is not null, use the value of p_check_seed for seed.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# Initialize the hash function with p_ctx->p_mask_option->p_hash_info->p_hash_init_api and set the return value
 *     into the variable external_ret.
 *  -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *    -# If NULL != p_ctx->label.p_data Then,
 *      -# Set p_label to p_ctx->label.p_data.
 *      -# Set label_len to p_ctx->label.len.
 *    -# Else
 *      -# Set p_label to label_empty.
 *      -# Set label_len to 0U.
 *    -# Input p_label into the hash function with p_ctx->p_mask_option->p_hash_info->p_hash_update_api,
 *       and set the return value into external_ret.
 *    -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *      -# Get the hashed value with p_ctx->p_mask_option->p_hash_info->p_hash_final_api,
 *         set the hashed value and its length into lhash_val and lhash_len respectively,
 *         and set the return value into external_ret.
 *      -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *        -# Set ret to #PKCL_RET_OK.
 *      -# Else
 *        -# Set ret to #PKCL_RET_HASHAPIERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_HASHAPIERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_HASHAPIERR.
 *  -# If (#PKCL_RET_OK == ret)
 *    -# If (((#PKCL_RSA_DOUBLE_LEN * lhash_len) <= (mod_len - #PKCL_RSA_PS_OFFSET)) and
 *           (p_plain->len <= ((mod_len - (#PKCL_RSA_DOUBLE_LEN * lhash_len)) - #PKCL_RSA_PS_OFFSET)))
 *      -# Copy lhash_val to db with #r_pkcl_cmn_cpy_byte.
 *      -# Set (((mod_len - p_plain->len) - (#PKCL_RSA_DOUBLE_LEN * lhash_len)) - #PKCL_RSA_PS_OFFSET) into ps_len.
 *      -# Concatenate ps_len bytes of zeros with db[lhash_len] using #r_pkcl_cmn_set_byte.
 *      -# Set 0x01 into db[lhash_len + ps_len].
 *      -# Concatenate p_plain->p_data with db[lhash_len + ps_len + 1] using #r_pkcl_cmn_cpy_byte.
 *      -# If (NULL != p_check_seed)
 *        -# Get the PS value from p_check_seed and set seed_val to it with # r_pkcl_cmn_cpy_byte.
 *        -# Set external_ret to #PKCL_EXTERNAL_API_OK.
 *      -# Else
 *        -# Generate seed with p_ctx->p_drbg_api, set the seed into seed_val, and set the return value into
 *           the variable external_ret.
 *      -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *        -# Generate the masked value of seed_val, set the masked value into mask with #r_pkcl_rsa_mgf1,
 *           and set ret to the return value.
 *        -# If (#PKCL_RET_OK == ret)
 *          -# Loop while (xor_loop < ((mod_len - lhash_len) - 1U)), and increment xor_loop for each loop.
 *            -# Set exclusive OR of (db[xor_loop] and mask[xor_loop]) into db[xor_loop].
 *          -# Generate the masked value of db, set the masked value into mask with #r_pkcl_rsa_mgf1,
 *             and set ret to the return value.
 *          -# If (#PKCL_RET_OK == ret)
 *            -# Loop while xor_loop < is less than lhash_len, and increment xor_loop for each loop.
 *              -# Set exclusive OR of (seed_val[xor_loop] and mask[xor_loop]) into seed_val[xor_loop].
 *            -# Set 0 into em_pos.
 *            -# p_em->p_data[em_pos].
 *            -# Set 1 into em_pos.
 *            -# Concatenate seed_val with p_em->p_data[em_pos] using #r_pkcl_cmn_cpy_byte.
 *            -# Set (em_pos + lhash_len) into em_pos.
 *            -# Concatenate db with p_em->p_data[em_pos] using #r_pkcl_cmn_cpy_byte.
 *            -# Set (em_pos + (mod_len - lhash_len) - 1) into em_pos.
 *      -# Else
 *        -# Set ret to #PKCL_RET_DRBGAPIERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_LENGTHERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_enc_scheme_oaep(const st_pkc_data_t* const p_plain, const st_pkc_rsa_ctx_t* const p_ctx,
                                                const uint32_t mod_len, st_pkc_data_t* const p_em,
                                                const uint8_t* const p_check_seed)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    uint32_t external_ret;
    uint8_t db[PKCL_RSA_KEY_LEN_MAX];
    uint8_t mask[PKCL_RSA_KEY_LEN_MAX];
    const uint8_t * p_label;
    uint32_t label_len;
    const uint8_t label_empty[1] = "";
    uint8_t lhash_val[PKCL_RSA_HASH_LEN_MAX];
    uint8_t seed_val[PKCL_RSA_HASH_LEN_MAX];
    uint32_t lhash_len;
    uint32_t ps_len;
    uint32_t xor_loop;
    uint32_t em_pos;

    /* calc lhash */
    external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_init_api(p_ctx->p_mask_option->p_hash_info->p_hash_ctx);
    if (PKCL_EXTERNAL_API_OK == external_ret)
    {
        if (NULL != p_ctx->label.p_data)
        {
            p_label = p_ctx->label.p_data;
            label_len = p_ctx->label.len;
        }
        else
        {
            p_label = (const uint8_t*)label_empty; /* No problem with casting to pointer size */
            label_len = 0U;
        }
        external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_update_api(
                                                                    p_ctx->p_mask_option->p_hash_info->p_hash_ctx,
                                                                    p_label, label_len);
        if (PKCL_EXTERNAL_API_OK == external_ret)
        {
            external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_final_api(
                                                                    p_ctx->p_mask_option->p_hash_info->p_hash_ctx,
                                                                    lhash_val, &lhash_len);
            if (PKCL_EXTERNAL_API_OK == external_ret)
            {
                ret = PKCL_RET_OK;
            }
            else
            {
                ret = PKCL_RET_HASHAPIERR;
            }
        }
        else
        {
            ret = PKCL_RET_HASHAPIERR;
        }
    }
    else
    {
        ret = PKCL_RET_HASHAPIERR;
    }

    if (PKCL_RET_OK == ret)
    {
        /* message size check */
        if (((PKCL_RSA_DOUBLE_LEN * lhash_len) <= (mod_len - PKCL_RSA_PS_OFFSET)) &&
            (p_plain->len <= ((mod_len - (PKCL_RSA_DOUBLE_LEN * lhash_len)) - PKCL_RSA_PS_OFFSET)))
        {
            /* Create DB == lhash || PS || 0x01 || M */
            /* Set lhash in DB */
            r_pkcl_cmn_cpy_byte(db, lhash_val, lhash_len);

            /* Set PS in DB */
            /* generate a string PS of 0x00 bytes */
            ps_len = (((mod_len - p_plain->len) - (PKCL_RSA_DOUBLE_LEN * lhash_len)) - PKCL_RSA_PS_OFFSET);
            r_pkcl_cmn_set_byte(&db[lhash_len], 0x00U, ps_len);

            /* Set 0x01 in DB */
            db[lhash_len + ps_len] = 0x01U;

            /* Set M in DB */
            r_pkcl_cmn_cpy_byte(&db[lhash_len + ps_len + 1U], p_plain->p_data, p_plain->len);

            /* generate seed */
            if (NULL != p_check_seed)
            {
                r_pkcl_cmn_cpy_byte(seed_val, p_check_seed, lhash_len);
                external_ret = PKCL_EXTERNAL_API_OK;
            }
            else
            {
                external_ret = p_ctx->p_drbg_api(lhash_len, seed_val);
            }

            if (PKCL_EXTERNAL_API_OK == external_ret)
            {
                /* create maskedDB */
                ret = r_pkcl_rsa_mgf1(p_ctx, seed_val, lhash_len, mask, ((mod_len - lhash_len) - 1U));
                if (PKCL_RET_OK == ret)
                {
                    /* xor against DB */
                    for (xor_loop = 0U; xor_loop < ((mod_len - lhash_len) - 1U); xor_loop++ )
                    {
                        /* No problem for casting to support usual arithmetic conversion */
                        db[xor_loop] = (uint8_t)((uint32_t)db[xor_loop] ^ (uint32_t)mask[xor_loop]);
                    }

                    /* create maskedSeed */
                    ret = r_pkcl_rsa_mgf1(p_ctx, db, ((mod_len - lhash_len) - 1U), mask, lhash_len);
                    if (PKCL_RET_OK == ret)
                    {
                        /* xor against seed */
                        for (xor_loop = 0U; xor_loop < lhash_len; xor_loop++ )
                        {
                            /* No problem for casting to support usual arithmetic conversion */
                            seed_val[xor_loop] = (uint8_t)((uint32_t)seed_val[xor_loop] ^ (uint32_t)mask[xor_loop]);
                        }

                        /* Output EM == 0x00 || maskedSeed || maskedDB */
                        em_pos = 0U;

                        /* Set 0x00 in EM */
                        p_em->p_data[em_pos] = 0x00U;
                        em_pos = 1U;

                        /* Set maskedSeed in EM */
                        r_pkcl_cmn_cpy_byte(&p_em->p_data[em_pos], seed_val, lhash_len);
                        em_pos += lhash_len;

                        /* Set maskedDB in EM */
                        r_pkcl_cmn_cpy_byte(&p_em->p_data[em_pos], db, ((mod_len - lhash_len) - 1U));
                        em_pos += ((mod_len - lhash_len) - 1U);

                        /* Set EM length */
                        p_em->len = em_pos;
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
                ret = PKCL_RET_DRBGAPIERR;
            }
        }
        else
        {
            ret = PKCL_RET_LENGTHERR;
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_enc_scheme_oaep()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_dec_scheme_pkcs1
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Perform the EME-PKCS1-v1_5 scheme for RSA decryption
 * \param       [in]     p_em          A structure consisting of the byte length len of the encoded message and
 *                                      the data pointer p_data.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in,out] p_plain       Output plaintext.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_RSA_SCHEMEERR:            RSA scheme processing failed
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - The plaintext is separated from the encoded message and output.
 * \par Remarks
 *  - If p_ctx-> verified_gen_plain is not FALSE, save the PS value in p_ctx-> p_buf.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If !(p_em->len > mod_len)
 *    -# If ((p_em->p_data[0] == 0x00) and (p_em->p_data[1] == #PKCL_RSA_PKCS_PADDING_NUM))
 *      -# Set #PKCL_RSA_PS_OFFSET into ps_loop.
 *      -# Loop while ((ps_loop < mod_len) and (p_em->p_data[ps_loop] != 0x00))
 *        -# Increment ps_loop.
 *      -# Set (ps_loop - #PKCL_RSA_PS_OFFSET) into ps_len.
 *      -# If (((ps_len - #PKCL_RSA_PS_OFFSET) < mod_len) and (ps_len >= #PKCL_RSA_PKCS_PS_LEN_MIN))
 *        -# If (p_plain->len >= (p_em->len - (#PKCL_RSA_PS_OFFSET + ps_len + 1U)))
 *          -# Set (p_em->len - (#PKCL_RSA_PS_OFFSET + ps_len + 1U)) into p_plain->len.
 *          -# Set p_em->p_data[#PKCL_RSA_PS_OFFSET + ps_len + 1]-p_em->p_data[#PKCL_RSA_PS_OFFSET +
 *            ps_len + p_plain->len] into p_plain->p_data with #r_pkcl_cmn_cpy_byte.
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *        -# If (#PKC_BOOL_FALSE != p_ctx->verified_gen_plain) && (#PKCL_RET_OK == ret)
 *          -# Store the PS value in the buffer p_verified_plain->cipher.
 *        -# Else
 *          -# Do nothing.
 *      -# Else
 *        -# Set ret to #PKCL_RET_RSA_SCHEMEERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_RSA_SCHEMEERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_LENGTHERR.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_dec_scheme_pkcs1(const st_pkc_data_t* const p_em, const st_pkc_rsa_ctx_t* const p_ctx,
                                                const uint32_t mod_len, st_pkc_data_t* const p_plain)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    uint32_t ps_loop;
    uint32_t ps_len;
    st_pkcl_rsa_verified_plain_t * p_verified_plain;

    /* message size check */
    if (!(p_em->len > mod_len))
    {
        /* check padding value */
        if (((uint32_t)p_em->p_data[0] == 0x00U) && ((uint32_t)p_em->p_data[1] == PKCL_RSA_PKCS_PADDING_NUM))
        {
            ps_loop = PKCL_RSA_PS_OFFSET;

            /* Find the 0x00 bytes after the PS to identify ps_len */
            while ((ps_loop < mod_len) && ((uint32_t)p_em->p_data[ps_loop] != 0x00U))
            {
                ps_loop++;
            }
            ps_len = (ps_loop - PKCL_RSA_PS_OFFSET);

            /* check ps_len */
            if (((ps_len + PKCL_RSA_PS_OFFSET) < mod_len) && (ps_len >= PKCL_RSA_PKCS_PS_LEN_MIN))
            {
                /* check plaintext buffer */
                if (p_plain->len >= (p_em->len - (PKCL_RSA_PS_OFFSET + ps_len + 1U)))
                {
                    p_plain->len = (p_em->len - (PKCL_RSA_PS_OFFSET + ps_len + 1U));
                    r_pkcl_cmn_cpy_byte(p_plain->p_data,
                                        &p_em->p_data[PKCL_RSA_PS_OFFSET + ps_len + 1U], p_plain->len);
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_LENGTHERR;
                }

                /* If verified_gen_plain == TRUE, store PS */
                if ((PKC_BOOL_FALSE != p_ctx->verified_gen_plain) && (PKCL_RET_OK == ret))
                {
                    /* Cast p_buf to RSA plain verification buffer type */
                    p_verified_plain = (st_pkcl_rsa_verified_plain_t *)(&p_ctx->p_buf[sizeof(st_pkcl_rsa_dec_t)]);

                    /* Use p_verified_plain->cipher  as a temporary buffer for the PS */
                    r_pkcl_cmn_cpy_byte(p_verified_plain->cipher , &p_em->p_data[PKCL_RSA_PS_OFFSET], ps_len);
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                ret = PKCL_RET_RSA_SCHEMEERR;
            }
        }
        else
        {
            ret = PKCL_RET_RSA_SCHEMEERR;
        }
    }
    else
    {
        ret = PKCL_RET_LENGTHERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_dec_scheme_pkcs1()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_dec_scheme_oaep
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Perform the EME-OAEP scheme for RSA decryption
 * \param       [in]     p_em          A structure consisting of the byte length len of the encoded message and
 *                                      the data pointer p_data.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in,out] p_plain       Output plaintext.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_HASHAPIERR:               External HASH API returned error
 *  - #PKCL_RET_RSA_SCHEMEERR:            RSA scheme processing failed
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Calculate Lhash by calling the external HASH API.
 *      - Calculate seed and DB by separating maskedDB and maskedSeed from the encoded message.
 *      - Get Lhash'from DB and compare with Lhash. \n
 *        In case of inconsistency, #PKCL_RET_RSA_SCHEMEERR is returned.
 *      - Get plaintext from DB.
 * \par Remarks
 *  - If p_ctx-> verified_gen_plain is not FALSE, save the seed value in p_ctx-> p_buf.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# Initialize the hash function with p_ctx->p_mask_option->p_hash_info->p_hash_init_api and set the return value
 *     to the variable external_ret.
 *  -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *    -# If NULL != p_ctx->label.p_data Then,
 *      -# Set p_label to p_ctx->label.p_data.
 *      -# Set label_len to p_ctx->label.len.
 *    -# Else
 *      -# Set p_label to label_empty.
 *      -# Set label_len to 0U.
 *    -# Input p_label into the hash function with p_ctx->p_mask_option->p_hash_info->p_hash_update_api,
 *       and set the return value into external_ret.
 *    -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *      -# Get the hashed value with p_ctx->p_mask_option->p_hash_info->p_hash_final_api,
 *         set the hashed value and its length into lhash_val and lhash_len respectively,
 *         and set the return value into external_ret.
 *      -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *        -# Set ret to #PKCL_RET_OK.
 *      -# Else
 *        -# Set ret to #PKCL_RET_HASHAPIERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_HASHAPIERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_HASHAPIERR.
 *  -# If (#PKCL_RET_OK == ret)
 *    -# Set 0 into em_pos.
 *    -# Set p_em->p_data[em_pos] into y.
 *    -# Set 1 into em_pos.
 *    -# If (0x00 == y)
 *      -# Set p_em->p_data[em_pos]-p_em->p_data[em_pos+lhash_len-1] into seed_val with #r_pkcl_cmn_cpy_byte.
 *      -# Set em_pos+lhash_len into em_pos.
 *      -# Set ((mod_len - lhash_len) - 1) into db_len.
 *      -# Set p_em->p_data[em_pos]-p_em->p_data[em_pos+db_len-1] into db with #r_pkcl_cmn_cpy_byte.
 *      -# Generate the masked value of db with #r_pkcl_rsa_mgf1, set the masked value into mask,
 *         and set the return velue into ret.
 *      -# If (#PKCL_RET_OK == ret)
 *        -# Loop while xor_loop is less than db_len, and increment xor_loop for each loop.
 *          -# Set exclusive OR of (seed_val[xor_loop] and mask[xor_loop]) into seed_val[xor_loop].
 *        -# If (PKC_BOOL_FALSE != p_ctx->verified_gen_plain), then
 *          -# Store the calculated seed value in the buffer p_verified_plain->cipher.
 *        -# Else
 *          -# Do nothing.
 *        -# Generate the masked value of seed_val with #r_pkcl_rsa_mgf1, set the masked value into mask,
 *           and set the return velue into ret.
 *        -# If (#PKCL_RET_OK == ret)
 *          -# Loop while xor_loop is less than db_len, and increment xor_loop for each loop.
 *            -# Set exclusive OR of (db[xor_loop] and mask[xor_loop]) into db[xor_loop].
 *          -# Set db[0]-db[lhash_len-1] into an_lhash_val with #r_pkcl_cmn_cpy_byte.
 *          -# Compare an_lhash_val with lhash_val with #r_pkcl_cmn_cmp_byte, and set the return value into cmp_ret.
 *          -# If (#PKC_BOOL_TRUE == cmp_ret)
 *            -# Set lhash_len into db_pos.
 *            -# Loop while ((db_pos < db_len) and (db[db_pos] != 0x01))
 *              -# Increment db_pos.
 *            -# If (db[db_pos] == 0x01)
 *              -# Set db_pos+1 into dp_pos.
 *              -# Set db_len - db_pos into p_plain->len.
 *              -# Set db[db_pos]-db[db_pos+p_plain->len-1] into p_plain->p_data with #r_pkcl_cmn_cpy_byte.
 *            -# Else
 *              -# Set ret to #PKCL_RET_RSA_SCHEMEERR.
 *          -# Else
 *            -# Set ret to #PKCL_RET_RSA_SCHEMEERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_RSA_SCHEMEERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_dec_scheme_oaep(const st_pkc_data_t* const p_em, const st_pkc_rsa_ctx_t* const p_ctx,
                                                const uint32_t mod_len, st_pkc_data_t* const p_plain)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    uint32_t external_ret;
    e_pkc_bool_t cmp_ret;
    uint8_t db[PKCL_RSA_KEY_LEN_MAX];
    uint8_t mask[PKCL_RSA_KEY_LEN_MAX];
    const uint8_t * p_label;
    const uint8_t label_empty[1] = "";
    uint32_t label_len;
    uint8_t lhash_val[PKCL_RSA_HASH_LEN_MAX];
    uint8_t an_lhash_val[PKCL_RSA_HASH_LEN_MAX];
    uint8_t seed_val[PKCL_RSA_HASH_LEN_MAX];
    uint32_t y;
    uint32_t lhash_len;
    uint32_t db_len;
    uint32_t xor_loop;
    uint32_t em_pos;
    uint32_t db_pos;
    st_pkcl_rsa_verified_plain_t * p_verified_plain;

    /* calc lhash */
    external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_init_api(p_ctx->p_mask_option->p_hash_info->p_hash_ctx);
    if (PKCL_EXTERNAL_API_OK == external_ret)
    {
        if (NULL != p_ctx->label.p_data)
        {
            p_label = p_ctx->label.p_data;
            label_len = p_ctx->label.len;
        }
        else
        {
            p_label = (const uint8_t*)label_empty; /* No problem with casting to pointer size */
            label_len = 0U;
        }
        external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_update_api(
                                                                    p_ctx->p_mask_option->p_hash_info->p_hash_ctx,
                                                                    p_label, label_len);
        if (PKCL_EXTERNAL_API_OK == external_ret)
        {
            external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_final_api(
                                                                    p_ctx->p_mask_option->p_hash_info->p_hash_ctx,
                                                                    lhash_val, &lhash_len);
            if (PKCL_EXTERNAL_API_OK == external_ret)
            {
                ret = PKCL_RET_OK;
            }
            else
            {
                ret = PKCL_RET_HASHAPIERR;
            }
        }
        else
        {
            ret = PKCL_RET_HASHAPIERR;
        }
    }
    else
    {
        ret = PKCL_RET_HASHAPIERR;
    }

    if (PKCL_RET_OK == ret)
    {
        /* separate EM */
        em_pos = 0U;
        y = p_em->p_data[em_pos];
        em_pos = 1U;

        if (0x00U == y)
        {
            /* separate maskedSeed */
            r_pkcl_cmn_cpy_byte(seed_val, &p_em->p_data[em_pos], lhash_len);
            em_pos += lhash_len;

            /* separate maskedDB */
            db_len = ((mod_len - lhash_len) - 1U);
            r_pkcl_cmn_cpy_byte(db, &p_em->p_data[em_pos], db_len);

            /* create seedmask */
            ret = r_pkcl_rsa_mgf1(p_ctx, db, db_len, mask, lhash_len);
            if (PKCL_RET_OK == ret)
            {
                /* xor against seed */
                for (xor_loop = 0U; xor_loop < lhash_len; xor_loop++ )
                {
                    /* No problem for casting to support usual arithmetic conversion */
                    seed_val[xor_loop] = (uint8_t)((uint32_t)seed_val[xor_loop] ^ (uint32_t)mask[xor_loop]);
                }

                /* If verified_gen_plain == TRUE, store seed */
                if (PKC_BOOL_FALSE != p_ctx->verified_gen_plain)
                {
                    /* Cast p_buf to RSA plaintext verification buffer type */
                    p_verified_plain = (st_pkcl_rsa_verified_plain_t *)(&p_ctx->p_buf[sizeof(st_pkcl_rsa_dec_t)]);

                    /* Use p_verified_plain->cipher as a temporary buffer for the seed */
                    r_pkcl_cmn_cpy_byte(p_verified_plain->cipher, seed_val, lhash_len);
                }
                else
                {
                    /* Do nothing */
                }

                /* create dbmask */
                ret = r_pkcl_rsa_mgf1(p_ctx, seed_val, lhash_len, mask, db_len);

                if (PKCL_RET_OK == ret)
                {
                    /* xor against seed */
                    for (xor_loop = 0U; xor_loop < db_len; xor_loop++ )
                    {
                        /* No problem for casting to support usual arithmetic conversion */
                        db[xor_loop] = (uint8_t)((uint32_t)db[xor_loop] ^ (uint32_t)mask[xor_loop]);
                    }

                    /* separate lhash' in DB */
                    r_pkcl_cmn_cpy_byte(an_lhash_val, db, lhash_len);

                    /* Compare lhash with lhash' */
                    cmp_ret = r_pkcl_cmn_cmp_byte(lhash_val, an_lhash_val, lhash_len);

                    if (PKC_BOOL_TRUE == cmp_ret)
                    {
                        db_pos = lhash_len;

                        /* Find the 0x01 byte after the PS to locate the message */
                        while ((db_pos < db_len) && ((uint32_t)db[db_pos] != 0x01U))
                        {
                            db_pos++;
                        }

                        /* No problem for casting to support usual arithmetic conversion */
                        if ((uint32_t)db[db_pos] == 0x01U)
                        {
                            /* Position one behind 0x01 */
                            db_pos += 1U;

                            /* output plain text */
                            p_plain->len = db_len - db_pos;
                            r_pkcl_cmn_cpy_byte(p_plain->p_data, &db[db_pos], p_plain->len);
                        }
                        else
                        {
                            ret = PKCL_RET_RSA_SCHEMEERR;
                        }
                    }
                    else
                    {
                        ret = PKCL_RET_RSA_SCHEMEERR;
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
        }
        else
        {
            ret = PKCL_RET_RSA_SCHEMEERR;
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_dec_scheme_oaep()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_sign_scheme_pkcs1
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Perform the EMSA-PKCS1-v1_5 scheme for RSA signature generation
 * \param       [in]     p_digest      A structure consisting of the byte length len of the message digest and
 *                                      the data pointer p_data.
 * \param       [in]     hash_type     Hash type used for p_digest.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in,out] p_em          Output encode message.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_RSA_SCHEMEERR:            RSA scheme processing failed
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_der_sha1, #s_der_sha224, #s_der_sha256, #s_der_sha384, #s_der_sha512, #s_der_sha512_224,
 *    #s_der_sha512_256, #s_der_sha3_224, #s_der_sha3_256, #s_der_sha3_384, #s_der_sha3_512
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Get the DER of the DigestInfo value from the hash type.
 *      - Create T, which is a concatenation of DER and hash value.
 *      - Create EM (encode message) by concatenating fixed value padding and PS, T.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If (p_em->len >= mod_len)
 *    -# If (#PKC_HASH_TYPE_SHA1 == hash_type)
 *      -# Copy #s_der_sha1 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *      -# Set the byte size of #s_der_sha1 into der_len.
 *    -# Else if (#PKC_HASH_TYPE_SHA2_224 == hash_type)
 *      -# Copy #s_der_sha224 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *      -# Set the byte size of #s_der_sha224 into der_len.
 *    -# Else if (#PKC_HASH_TYPE_SHA2_256 == hash_type)
 *      -# Copy #s_der_sha256 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *      -# Set the byte size of #s_der_sha256 into der_len.
 *    -# Else if (#PKC_HASH_TYPE_SHA2_512 == hash_type)
 *      -# Copy #s_der_sha512 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *      -# Set the byte size of #s_der_sha512 into der_len.
 *    -# Else if (#PKC_HASH_TYPE_SHA2_512_224 == hash_type)
 *      -# Copy #s_der_sha512_224 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *      -# Set the byte size of #s_der_sha512_224 into der_len.
 *    -# Else if (#PKC_HASH_TYPE_SHA2_256 == hash_type)
 *      -# Copy #s_der_sha512_256 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *      -# Set the byte size of #s_der_sha512_256 into der_len.
 *    -# Else if (#PKC_HASH_TYPE_SHA3_224 == hash_type)
 *      -# Copy #s_der_sha3_224 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *      -# Set the byte size of #s_der_sha224 into der_len.
 *    -# Else if (#PKC_HASH_TYPE_SHA3_256 == hash_type)
 *      -# Copy #s_der_sha3_256 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *      -# Set the byte size of #s_der_sha3_256 into der_len.
 *    -# Else if (#PKC_HASH_TYPE_SHA3_384 == hash_type)
 *      -# Copy #s_der_sha3_384 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *      -# Set the byte size of #s_der_sha3_384 into der_len.
 *    -# Else
 *      -# Copy #s_der_sha3_512 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *      -# Set the byte size of #s_der_sha3_512 into der_len.
 *    -# Get the hash length with #r_pkcl_rsa_get_hash_len and set the length into the variable hash_len.
 *    -# If (hash_len == p_digest->len)
 *      -# Set (der_len + p_digest->len) into the variable t_len.
 *      -# Set ((p_em->len - t_len) - #PKCL_RSA_PKCS_PS_LEN_CALC) into the variable ps_len.
 *      -# Concatenate ps_len bytes of fixed value (#PKCL_RSA_PKCS_PS_NUM) with p_em->p_data[#PKCL_RSA_PS_OFFSET]
 *         using #r_pkcl_cmn_cpy_byte.
 *      -# Concatenate der_data with p_em->p_data[#PKCL_RSA_PS_OFFSET + ps_len + 1] using #r_pkcl_cmn_cpy_byte.
 *      -# Concatenate p_digest->p_data with p_em->p_data[#PKCL_RSA_PS_OFFSET + ps_len + 1U + der_len]
 *         using #r_pkcl_cmn_cpy_byte.
 *      -# Set 0x00 into p_em->p_data[0].
 *      -# Set 0x01 into p_em->p_data[1].
 *      -# Set 0x00 into p_em->p_data[#PKCL_RSA_PS_OFFSET + ps_len].
 *      -# Set ret to #PKCL_RET_OK.
 *    -# Else
 *      -# Set ret to #PKCL_RET_RSA_SCHEMEERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_LENGTHERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_sign_scheme_pkcs1(const st_pkc_data_t* const p_digest, const e_pkc_hash_type_t hash_type,
                                                const uint32_t mod_len, const st_pkc_data_t* const p_em)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    uint32_t ps_len;
    uint8_t  der_data[PKCL_RSA_PKCS_DER_LEN_MAX];
    uint32_t der_len;
    uint32_t t_len;
    uint32_t hash_len;

    /* encode message buffer check */
    if (p_em->len >= mod_len)
    {
        /* hash type */
        if (PKC_HASH_TYPE_SHA1 == hash_type)
        {
            r_pkcl_cmn_cpy_byte(der_data, s_der_sha1, sizeof(s_der_sha1));
            der_len = sizeof(s_der_sha1);
        }
        else if (PKC_HASH_TYPE_SHA2_224 == hash_type)
        {
            r_pkcl_cmn_cpy_byte(der_data, s_der_sha224, sizeof(s_der_sha224));
            der_len = sizeof(s_der_sha224);
        }
        else if (PKC_HASH_TYPE_SHA2_256 == hash_type)
        {
            r_pkcl_cmn_cpy_byte(der_data, s_der_sha256, sizeof(s_der_sha256));
            der_len = sizeof(s_der_sha256);
        }
        else if (PKC_HASH_TYPE_SHA2_384 == hash_type)
        {
            r_pkcl_cmn_cpy_byte(der_data, s_der_sha384, sizeof(s_der_sha384));
            der_len = sizeof(s_der_sha384);
        }
        else if (PKC_HASH_TYPE_SHA2_512 == hash_type)
        {
            r_pkcl_cmn_cpy_byte(der_data, s_der_sha512, sizeof(s_der_sha512));
            der_len = sizeof(s_der_sha512);
        }
        else if (PKC_HASH_TYPE_SHA2_512_224 == hash_type)
        {
            r_pkcl_cmn_cpy_byte(der_data, s_der_sha512_224, sizeof(s_der_sha512_224));
            der_len = sizeof(s_der_sha512_224);
        }
        else if (PKC_HASH_TYPE_SHA2_512_256 == hash_type)
        {
            r_pkcl_cmn_cpy_byte(der_data, s_der_sha512_256, sizeof(s_der_sha512_256));
            der_len = sizeof(s_der_sha512_256);
        }
        else if (PKC_HASH_TYPE_SHA3_224 == hash_type)
        {
            r_pkcl_cmn_cpy_byte(der_data, s_der_sha3_224, sizeof(s_der_sha3_224));
            der_len = sizeof(s_der_sha3_224);
        }
        else if (PKC_HASH_TYPE_SHA3_256 == hash_type)
        {
            r_pkcl_cmn_cpy_byte(der_data, s_der_sha3_256, sizeof(s_der_sha3_256));
            der_len = sizeof(s_der_sha3_256);
        }
        else if (PKC_HASH_TYPE_SHA3_384 == hash_type)
        {
            r_pkcl_cmn_cpy_byte(der_data, s_der_sha3_384, sizeof(s_der_sha3_384));
            der_len = sizeof(s_der_sha3_384);
        }
        else /* PKC_HASH_TYPE_SHA3_512 == hash_type */
        {
            r_pkcl_cmn_cpy_byte(der_data, s_der_sha3_512, sizeof(s_der_sha3_512));
            der_len = sizeof(s_der_sha3_512);
        }

        /* Compares the hash length of the specified HashType with the digest length */
        hash_len = r_pkcl_rsa_get_hash_len(hash_type);
        if (hash_len == p_digest->len)
        {
            /* set t_len */
            t_len = der_len + p_digest->len;

            /* generate a string PS of 0xff bytes */
            ps_len = ((p_em->len - t_len) - PKCL_RSA_PKCS_PS_LEN_CALC);
            r_pkcl_cmn_set_byte(&p_em->p_data[PKCL_RSA_PS_OFFSET], PKCL_RSA_PKCS_PS_NUM, ps_len);

            /* output T */
            r_pkcl_cmn_cpy_byte(&p_em->p_data[PKCL_RSA_PS_OFFSET + ps_len + 1U],
                                der_data, der_len);
            r_pkcl_cmn_cpy_byte(&p_em->p_data[PKCL_RSA_PS_OFFSET + ps_len + 1U + der_len],
                                p_digest->p_data, p_digest->len);

            /* Other fixed value padding */
            p_em->p_data[0] = 0x00U;
            p_em->p_data[1] = 0x01U;
            p_em->p_data[PKCL_RSA_PS_OFFSET + ps_len] = 0x00U;

            ret = PKCL_RET_OK;
        }
        else
        {
            ret = PKCL_RET_RSA_SCHEMEERR;
        }
    }
    else
    {
        ret = PKCL_RET_LENGTHERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_sign_scheme_pkcs1()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_verify_scheme_pkcs1
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Perform the EMSA-PKCS1-v1_5 scheme for RSA signature verification
 * \param       [in]     p_em          A structure consisting of the byte length len of the encoded message and
 *                                      the data pointer p_data.
 * \param       [in]     p_digest      A structure consisting of the byte length len of the message digest and
 *                                      the data pointer p_data.
 * \param       [in]     hash_type     Hash type used for p_digest.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_RSA_SCHEMEERR             RSA scheme processing failed
 *  - #PKCL_RET_RSA_VERIFAIL:             RSA verification failed
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_der_sha1, #s_der_sha224, #s_der_sha256, #s_der_sha384, #s_der_sha512, #s_der_sha512_224,
 *    #s_der_sha512_256, #s_der_sha3_224, #s_der_sha3_256, #s_der_sha3_384, #s_der_sha3_512
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Get the DER of the DigestInfo value from the hash type.
 *      - Create T, which is a concatenation of DER and hash value.
 *      - Concatenate fixed value padding with PS and T and compare the created EM'with EM. \n
 *        In case of inconsistency, #PKCL_RET_RSA_VERIFAIL is returned.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# Set a buffer adress and mod_len to another_em, which is a #st_pkc_data_t type structure variable.
 *  -# If (#PKC_HASH_TYPE_SHA1 == hash_type)
 *    -# Copy #s_der_sha1 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *    -# Set the byte size of #s_der_sha1 into der_len.
 *  -# Else if (#PKC_HASH_TYPE_SHA2_224 == hash_type)
 *    -# Copy #s_der_sha224 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *    -# Set the byte size of #s_der_sha224 into der_len.
 *  -# Else if (#PKC_HASH_TYPE_SHA2_256 == hash_type)
 *    -# Copy #s_der_sha256 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *    -# Set the byte size of #s_der_sha256 into der_len.
 *  -# Else if (#PKC_HASH_TYPE_SHA2_512 == hash_type)
 *    -# Copy #s_der_sha512 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *    -# Set the byte size of #s_der_sha512 into der_len.
 *  -# Else if (#PKC_HASH_TYPE_SHA2_512_224 == hash_type)
 *    -# Copy #s_der_sha512_224 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *    -# Set the byte size of #s_der_sha512_224 into der_len.
 *  -# Else if (#PKC_HASH_TYPE_SHA2_256 == hash_type)
 *    -# Copy #s_der_sha512_256 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *    -# Set the byte size of #s_der_sha512_256 into der_len.
 *  -# Else if (#PKC_HASH_TYPE_SHA3_224 == hash_type)
 *    -# Copy #s_der_sha3_224 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *    -# Set the byte size of #s_der_sha224 into der_len.
 *  -# Else if (#PKC_HASH_TYPE_SHA3_256 == hash_type)
 *    -# Copy #s_der_sha3_256 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *    -# Set the byte size of #s_der_sha3_256 into der_len.
 *  -# Else if (#PKC_HASH_TYPE_SHA3_384 == hash_type)
 *    -# Copy #s_der_sha3_384 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *    -# Set the byte size of #s_der_sha3_384 into der_len.
 *  -# Else
 *    -# Copy #s_der_sha3_512 to the array der_data with #r_pkcl_cmn_cpy_byte.
 *    -# Set the byte size of #s_der_sha3_512 into der_len.
 *  -# Get the hash length with #r_pkcl_rsa_get_hash_len and set the variable hash_len to the length.
 *  -# If (hash_len == p_digest->len)
 *    -# Set ret to 0 fof double check.
 *    -# Set (der_len + p_digest->len) into the variable t_len.
 *    -# Set ((p_em->len - t_len) - #PKCL_RSA_PKCS_PS_LEN_CALC) into the variable ps_len.
 *    -# Concatenate ps_len bytes of fixed value (#PKCL_RSA_PKCS_PS_NUM) with another_em.p_data[#PKCL_RSA_PS_OFFSET]
 *       using #r_pkcl_cmn_cpy_byte.
 *    -# Concatenate der_data with another_em.p_data[#PKCL_RSA_PS_OFFSET + ps_len + 1] using #r_pkcl_cmn_cpy_byte.
 *    -# Concatenate p_digest->p_data with another_em.p_data[#PKCL_RSA_PS_OFFSET + ps_len + 1 + der_len]
 *       using #r_pkcl_cmn_cpy_byte.
 *    -# Set 0x00 into p_em->p_data[0].
 *    -# Set 0x01 into p_em->p_data[1].
 *    -# Set 0x00 into p_em->p_data[#PKCL_RSA_PS_OFFSET + ps_len].
 *    -# Compare p_em->p_data with another_em.p_data using #r_pkcl_cmn_cmp_byte and set cmp_ret to the return value.
 *    -# If (#PKC_BOOL_TRUE == cmp_ret)
 *      -# Add #PKCL_CMN_RET_OK_HALF_1 to ret.
 *      -# Compare p_em->p_data with another_em.p_data using #r_pkcl_cmn_cmp_byte and set cmp_ret to the return value.
 *      -# If (#PKC_BOOL_TRUE == cmp_ret)
 *        -# Add #PKCL_CMN_RET_OK_HALF_2 to ret.
 *      -# Else
 *        -# Set ret to #PKCL_RET_RSA_VERIFAIL.
 *    -# Else
 *      -# Set ret to #PKCL_RET_RSA_VERIFAIL.
 *  -# Else
 *    -# Set ret to #PKCL_RET_RSA_SCHEMEERR.
 *  -# If (((#pkcl_ret_t)0U == ret) || (#PKCL_CMN_RET_OK_HALF_1 == ret) || (#PKCL_CMN_RET_OK_HALF_2 == ret)), then
 *    -# Set ret to #PKCL_RET_TAMPERERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_verify_scheme_pkcs1(const st_pkc_data_t* const p_em, const st_pkc_data_t* const p_digest,
                                                    const e_pkc_hash_type_t hash_type, const uint32_t mod_len)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkc_bool_t cmp_ret;
    st_pkc_data_t another_em;
    uint8_t  an_em_val[PKCL_RSA_KEY_LEN_MAX];
    uint32_t ps_len;
    uint8_t  der_data[PKCL_RSA_PKCS_DER_LEN_MAX];
    uint32_t der_len;
    uint32_t t_len;
    uint32_t hash_len;

    another_em.p_data = an_em_val;
    another_em.len = mod_len;

    /* hash type */
    if (PKC_HASH_TYPE_SHA1 == hash_type)
    {
        r_pkcl_cmn_cpy_byte(der_data, s_der_sha1, sizeof(s_der_sha1));
        der_len = sizeof(s_der_sha1);
    }
    else if (PKC_HASH_TYPE_SHA2_224 == hash_type)
    {
        r_pkcl_cmn_cpy_byte(der_data, s_der_sha224, sizeof(s_der_sha224));
        der_len = sizeof(s_der_sha224);
    }
    else if (PKC_HASH_TYPE_SHA2_256 == hash_type)
    {
        r_pkcl_cmn_cpy_byte(der_data, s_der_sha256, sizeof(s_der_sha256));
        der_len = sizeof(s_der_sha256);
    }
    else if (PKC_HASH_TYPE_SHA2_384 == hash_type)
    {
        r_pkcl_cmn_cpy_byte(der_data, s_der_sha384, sizeof(s_der_sha384));
        der_len = sizeof(s_der_sha384);
    }
    else if (PKC_HASH_TYPE_SHA2_512 == hash_type)
    {
        r_pkcl_cmn_cpy_byte(der_data, s_der_sha512, sizeof(s_der_sha512));
        der_len = sizeof(s_der_sha512);
    }
    else if (PKC_HASH_TYPE_SHA2_512_224 == hash_type)
    {
        r_pkcl_cmn_cpy_byte(der_data, s_der_sha512_224, sizeof(s_der_sha512_224));
        der_len = sizeof(s_der_sha512_224);
    }
    else if (PKC_HASH_TYPE_SHA2_512_256 == hash_type)
    {
        r_pkcl_cmn_cpy_byte(der_data, s_der_sha512_256, sizeof(s_der_sha512_256));
        der_len = sizeof(s_der_sha512_256);
    }
    else if (PKC_HASH_TYPE_SHA3_224 == hash_type)
    {
        r_pkcl_cmn_cpy_byte(der_data, s_der_sha3_224, sizeof(s_der_sha3_224));
        der_len = sizeof(s_der_sha3_224);
    }
    else if (PKC_HASH_TYPE_SHA3_256 == hash_type)
    {
        r_pkcl_cmn_cpy_byte(der_data, s_der_sha3_256, sizeof(s_der_sha3_256));
        der_len = sizeof(s_der_sha3_256);
    }
    else if (PKC_HASH_TYPE_SHA3_384 == hash_type)
    {
        r_pkcl_cmn_cpy_byte(der_data, s_der_sha3_384, sizeof(s_der_sha3_384));
        der_len = sizeof(s_der_sha3_384);
    }
    else /* PKC_HASH_TYPE_SHA3_512 == hash_type */
    {
        r_pkcl_cmn_cpy_byte(der_data, s_der_sha3_512, sizeof(s_der_sha3_512));
        der_len = sizeof(s_der_sha3_512);
    }

    /* Compares the hash length of the specified HashType with the digest length */
    hash_len = r_pkcl_rsa_get_hash_len(hash_type);
    if (hash_len == p_digest->len)
    {
        /* Set ret to 0 for double check */
        ret = (pkcl_ret_t)0U;

        /* set t_len */
        t_len = der_len + p_digest->len;

        /* generate a string PS of 0xff bytes */
        ps_len = ((another_em.len - t_len) - PKCL_RSA_PKCS_PS_LEN_CALC);
        r_pkcl_cmn_set_byte(&another_em.p_data[PKCL_RSA_PS_OFFSET], PKCL_RSA_PKCS_PS_NUM, ps_len);

        /* output T */
        r_pkcl_cmn_cpy_byte(&another_em.p_data[PKCL_RSA_PS_OFFSET + ps_len + 1U],
                            der_data, der_len);
        r_pkcl_cmn_cpy_byte(&another_em.p_data[PKCL_RSA_PS_OFFSET + ps_len + 1U + der_len],
                            p_digest->p_data, p_digest->len);

        /* Other fixed value padding */
        another_em.p_data[0] = 0x00U;
        another_em.p_data[1] = 0x01U;
        another_em.p_data[PKCL_RSA_PS_OFFSET + ps_len] = 0x00U;

        /* Compare em with another_em */
        cmp_ret = r_pkcl_cmn_cmp_byte(p_em->p_data, another_em.p_data, p_em->len);
        if (PKC_BOOL_TRUE == cmp_ret)
        {
            ret += PKCL_CMN_RET_OK_HALF_1;

            cmp_ret = r_pkcl_cmn_cmp_byte(p_em->p_data, another_em.p_data, p_em->len);

            if (PKC_BOOL_TRUE == cmp_ret)
            {
                ret += PKCL_CMN_RET_OK_HALF_2;
            }
            else
            {
                ret = PKCL_RET_RSA_VERIFAIL;
            }
        }
        else
        {
            ret = PKCL_RET_RSA_VERIFAIL;
        }
    }
    else
    {
        ret = PKCL_RET_RSA_SCHEMEERR;
    }

    /* Set an internal error when an invalid result is set */
    if (((pkcl_ret_t)0U == ret) || (PKCL_CMN_RET_OK_HALF_1 == ret) || (PKCL_CMN_RET_OK_HALF_2 == ret))
    {
        ret = PKCL_RET_TAMPERERR;
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_verify_scheme_pkcs1()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_sign_scheme_pss
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Perform the EMSA-PSS scheme for RSA signature generation.
 * \param       [in]     p_digest      A structure consisting of the byte length len of the message digest and
 *                                      the data pointer p_data.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in,out] p_em          Output encode message.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRBGAPIERR:               External DRBG API returned error
 *  - #PKCL_RET_HASHAPIERR:               External HASH API returned error
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Create M'by concatenating fixed value padding with digest and salt.
 *      - Hash M'to create H.
 *      - Create a DB by concatenating PS, 0x01, and salt, and calculate the masked DB by xoring with the mask created
 *         from H.
 *      - Create EM by concatenating DB, H and 0xbc.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# Set salt length p_ctx->p_mask_option->salt_len into the variable salt_len.
 *  -# Set (mod_len * #PKCL_RSA_OCTET) into embits_len.
 *  -# Set hash_len to the hash length used for the mask with #r_pkcl_rsa_get_hash_len.
 *  -# If (hash_len != 0U)
 *    -# If ((p_em->len >= mod_len) && (mod_len >= (salt_len + hash_len + #PKCL_RSA_PSS_PS_LEN_CALC)))
 *      -# Generate a pseudo-random bit sequence with p_ctx->p_drbg_api and set the generated
 *         pseudo-random value into salt_val, and set the return value into external_ret.
 *      -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *        -# Set #PKCL_RSA_PSS_PAD_LEN bytes of zeros to m_data with #r_pkcl_cmn_set_byte.
 *        -# Concatenate p_digest->p_data with m_data[#PKCL_RSA_PSS_PAD_LEN] using #r_pkcl_cmn_cpy_byte.
 *        -# Concatenate salt_val with m_data[PKCL_RSA_PSS_PAD_LEN + p_digest->len]
 *           using #r_pkcl_cmn_cpy_byte.
 *        -# Set (#PKCL_RSA_PSS_PAD_LEN + p_digest->len + salt_len) to m_len.
 *        -# Initialize the hash function with p_ctx->p_mask_option->p_hash_info->p_hash_init_api,
 *           and set the return value into external_ret.
 *        -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *          -# Input m_data into the hash function with p_ctx->p_mask_option->p_hash_info->p_hash_update_api,
 *             and set the return value into external_ret.
 *          -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *            -# Get the hashed value with p_ctx->p_mask_option->p_hash_info->p_hash_final_api,
 *               set the hashed value and its length into hash_data and hash_len,
 *               and set the return value into external_ret.
 *            -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *              -# Set ((mod_len - hash_len) - 1) into db_len.
 *              -# Set (((mod_len - salt_len) - hash_len) - #PKCL_RSA_PSS_PS_LEN_CALC) into ps_len.
 *              -# Set ps_len bytes of zeros to the array db with #r_pkcl_cmn_set_byte.
 *              -# Set 0x01 into db[ps_len].
 *              -# Concatenate salt_val with db[ps_len+1] using #r_pkcl_cmn_cpy_byte.
 *              -# Generate the masked value of hash_data with #r_pkcl_rsa_mgf1, and set the return velue into ret.
 *              -# If (#PKCL_RET_OK == ret)
 *                -# Loop while xor_loop is less than db_len, and increment xor_loop for each loop.
 *                  -# Set exclusive OR of (db[xor_loop] and mask[xor_loop]) into db[xor_loop].
 *                -# Set (db[0] & (#PKCL_RSA_BIT_CLEAR_NUM >> (mod_len * #PKCL_RSA_OCTET - (embits_len - 1))))
 *                   into db[0].
 *                -# Set db into p_em->p_data with #r_pkcl_cmn_cpy_byte.
 *                -# Concatenate hash_data with p_em->p_data[db_len] with #r_pkcl_cmn_cpy_byte.
 *                -# Set #PKCL_RSA_PSS_PADDING_NUM into p_em->p_data[db_len + hash_len].
 *                -# Set (db_len + hash_len + 1U) into p_em->len.
 *            -# Else
 *              -# Set ret to #PKCL_RET_HASHAPIERR.
 *          -# Else
 *            -# Set ret to #PKCL_RET_HASHAPIERR.
 *        -# Else
 *          -# Set ret to #PKCL_RET_HASHAPIERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_DRBGAPIERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_LENGTHERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_sign_scheme_pss(const st_pkc_data_t* const p_digest,
                                                const st_pkc_rsa_ctx_t* const p_ctx,
                                                const uint32_t mod_len, st_pkc_data_t* const p_em)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    uint32_t external_ret;
    uint8_t db[PKCL_RSA_KEY_LEN_MAX];
    uint8_t mask[PKCL_RSA_KEY_LEN_MAX];
    uint8_t m_data[PKCL_RSA_PSS_PAD_LEN + PKCL_RSA_KEY_LEN_MAX + PKCL_RSA_SALT_LEN_MAX];
    uint8_t hash_data[PKCL_RSA_HASH_LEN_MAX];
    uint8_t salt_val[PKCL_RSA_SALT_LEN_MAX];
    const uint32_t salt_len = p_ctx->p_mask_option->salt_len;
    uint32_t m_len;
    uint32_t hash_len;
    uint32_t db_len;
    uint32_t ps_len;
    uint32_t xor_loop;
    const uint32_t embits_len = (mod_len * PKCL_RSA_OCTET); /* Currently, the corresponding key lengths are 1024,
                                                            2056, 3078, 4096, so embitsLen is (mod_len * 8). */

    /* set hash_len */
    hash_len = r_pkcl_rsa_get_hash_len(p_ctx->p_mask_option->mask_type);

    if (0U != hash_len)
    {
        if ((p_em->len >= mod_len) && (mod_len >= (salt_len + hash_len + PKCL_RSA_PSS_PS_LEN_CALC)))
        {
            external_ret = p_ctx->p_drbg_api(salt_len, salt_val);
            if (PKCL_EXTERNAL_API_OK == external_ret)
            {
                /* create M' == (0x)00 00 00 00 00 00 00 00 || mHash || salt */
                /* Set 0x00 for 8 bytes as padding */
                r_pkcl_cmn_set_byte(m_data, 0x00U, PKCL_RSA_PSS_PAD_LEN);

                /* Set p_digest */
                r_pkcl_cmn_cpy_byte(&m_data[PKCL_RSA_PSS_PAD_LEN], p_digest->p_data, p_digest->len);

                /* Set salt */
                r_pkcl_cmn_cpy_byte(&m_data[PKCL_RSA_PSS_PAD_LEN + p_digest->len], salt_val, salt_len);
                m_len = (PKCL_RSA_PSS_PAD_LEN + p_digest->len + salt_len);

                /* calc H */
                external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_init_api(
                                                                        p_ctx->p_mask_option->p_hash_info->p_hash_ctx);
                if (PKCL_EXTERNAL_API_OK == external_ret)
                {
                    external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_update_api(
                                                                        p_ctx->p_mask_option->p_hash_info->p_hash_ctx,
                                                                        m_data, m_len);
                    if (PKCL_EXTERNAL_API_OK == external_ret)
                    {
                        external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_final_api(
                                                                        p_ctx->p_mask_option->p_hash_info->p_hash_ctx,
                                                                        hash_data, &hash_len);
                        if (PKCL_EXTERNAL_API_OK == external_ret)
                        {
                            /* Create DB == PS || 0x01 || salt */
                            db_len = ((mod_len - hash_len) - 1U);

                            /* generate PS */
                            ps_len = (((mod_len - salt_len) - hash_len) - PKCL_RSA_PSS_PS_LEN_CALC);
                            r_pkcl_cmn_set_byte(db, 0x00U, ps_len);

                            /* Set 0x01 in DB */
                            db[ps_len] = 0x01U;

                            /* Set salt in DB */
                            r_pkcl_cmn_cpy_byte(&db[ps_len+1U], salt_val, salt_len);

                            /* create maskedDB */
                            ret = r_pkcl_rsa_mgf1(p_ctx, hash_data, hash_len, mask, db_len);
                            if (PKCL_RET_OK == ret)
                            {
                                /* xor against DB */
                                for (xor_loop = 0U; xor_loop < db_len; xor_loop++ )
                                {
                                    /* No problem for casting to support usual arithmetic conversion */
                                    db[xor_loop] = (uint8_t)((uint32_t)db[xor_loop] ^ (uint32_t)mask[xor_loop]);
                                }

                                /* now clear the 8*mod_len - embits_len-1 most significant bits */
                                db[0] = (uint8_t)((uint32_t)db[0] & ((uint32_t)PKCL_RSA_BIT_CLEAR_NUM >>
                                                                    ((mod_len * PKCL_RSA_OCTET) - (embits_len - 1U))));

                                /* output EM */
                                r_pkcl_cmn_cpy_byte(p_em->p_data, db, db_len);
                                r_pkcl_cmn_cpy_byte(&p_em->p_data[db_len], hash_data, hash_len);
                                p_em->p_data[db_len + hash_len] = PKCL_RSA_PSS_PADDING_NUM;
                                p_em->len = (db_len + hash_len + 1U);
                            }
                            else
                            {
                                /* Do nothing */
                            }
                        }
                        else
                        {
                            ret = PKCL_RET_HASHAPIERR;
                        }
                    }
                    else
                    {
                        ret = PKCL_RET_HASHAPIERR;
                    }
                }
                else
                {
                    ret = PKCL_RET_HASHAPIERR;
                }
            }
            else
            {
                ret = PKCL_RET_DRBGAPIERR;
            }
        }
        else
        {
            ret = PKCL_RET_LENGTHERR;
        }
    }
    else
    {
        ret = PKCL_RET_INTERNALERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_sign_scheme_pss()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_verify_scheme_pss
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief
 * \param       [in]     p_em          A structure consisting of the byte length len of the encoded message and
 *                                      the data pointer p_data.
 * \param       [in]     p_digest      A structure consisting of the byte length len of the message digest and
 *                                      the data pointer p_data.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_HASHAPIERR:               External HASH API returned error
 *  - #PKCL_RET_RSA_SCHEMEERR:            RSA scheme processing failed
 *  - #PKCL_RET_RSA_VERIFAIL:             RSA verification failed
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Separate the masked DB from the EM and get the DB.
 *      - Create M'by concatenating fixed value padding with digest and salt.
 *      - Hash M'to create H'.
 *      - Compare H and H'. In case of inconsistency, #PKCL_RET_RSA_VERIFAIL is returned.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# Set salt length p_ctx->p_mask_option->salt_len into the variable salt_len.
 *  -# Set (mod_len * #PKCL_RSA_OCTET) into embits_len.
 *  -# Set hash_len to the hash length used for the mask with #r_pkcl_rsa_get_hash_len.
 *  -# If (hash_len != 0U)
 *    -# If (p_em->len >=  (salt_len + hash_len + #PKCL_RSA_PSS_PS_LEN_CALC))
 *      -# If (#PKCL_RSA_PSS_PADDING_NUM == p_em->p_data[p_em->len-1])
 *        -# Set ((mod_len - hash_len) - 1) into db_len.
 *        -# Set p_em->p_data[0]-p_em->p_data[db_len-1] into db with #r_pkcl_cmn_cpy_byte.
 *        -# Set p_em->p_data[db_len]-p_em->p_data[db_len+hash_len-1] into hash_val with #r_pkcl_cmn_cpy_byte.
 *        -# If ((p_em->p_data[0] &
 *                (~(#PKCL_RSA_BIT_CLEAR_NUM >> ((mod_len * #PKCL_RSA_OCTET) - (embits_len - 1))))) == 0U)
 *          -# Generate the masked variable of hash_val with #r_pkcl_rsa_mgf1, and set ret to the return value.
 *          -# If (#PKCL_RET_OK == ret)
 *            -# Loop while xor_loop is less than db_len, and increment xor_loop for each loop.
 *              -# Set exclusive OR of (db[xor_loop] and mask[xor_loop]) into db[xor_loop].
 *            -# Set (db[0] & (#PKCL_RSA_BIT_CLEAR_NUM >> (mod_len * #PKCL_RSA_OCTET - (embits_len - 1)))) into db[0].
 *            -# Set (((mod_len - salt_len) - hash_len) - #PKCL_RSA_PSS_PS_LEN_CALC) into ps_len.
 *            -# Set db[ps_len + 1U]-db[ps_len + salt_len + 1U] into salt_val with #r_pkcl_cmn_cpy_byte.
 *            -# Set #PKCL_RSA_PSS_PAD_LEN bytes of zeros to m_data with #r_pkcl_cmn_set_byte.
 *            -# Concatenate p_digest->p_data with m_data[#PKCL_RSA_PSS_PAD_LEN] using #r_pkcl_cmn_cpy_byte.
 *            -# Concatenate salt_val with m_data[#PKCL_RSA_PSS_PAD_LEN + p_digest->len] using #r_pkcl_cmn_cpy_byte.
 *            -# Set (#PKCL_RSA_PSS_PAD_LEN + p_digest->len + salt_len) to m_len.
 *            -# Initialize the hash function with p_ctx->p_mask_option->p_hash_info->p_hash_init_api,
 *               and set external_ret to the return value.
 *            -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *              -# Input m_data into the hash function with p_ctx->p_mask_option->p_hash_info->p_hash_update_api,
 *                 and set external_ret to the return value.
 *              -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *                -# Get the hashed value with p_ctx->p_mask_option->p_hash_info->p_hash_final_api,
 *                   set the hashed value and its length into an_hash_val and hash_len,
 *                   and set external_ret to the return value.
 *                -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *                  -# Set ret to 0 for double check.
 *                  -# Compare hash_val an_hash_val using #r_pkcl_cmn_cmp_byte and set cmp_ret to the return value.
 *                  -# If #PKC_BOOL_TRUE == cmp_ret, then
 *                    -# Add ret to #PKCL_CMN_RET_OK_HALF_1.
 *                    -# Compare hash_val an_hash_val using #r_pkcl_cmn_cmp_byte and set cmp_ret to the return value.
 *                    -# If #PKC_BOOL_TRUE == cmp_ret, then
 *                      -# Add ret to #PKCL_CMN_RET_OK_HALF_2.
 *                    -# Else
 *                      -# Set ret to #PKCL_RET_RSA_VERIFAIL.
 *                  -# Else
 *                    -# Set ret to #PKCL_RET_RSA_VERIFAIL.
 *                -# Else
 *                  -# Set ret to #PKCL_RET_HASHAPIERR.
 *              -# Else
 *                -# Set ret to #PKCL_RET_HASHAPIERR.
 *            -# Else
 *              -# Set ret to #PKCL_RET_HASHAPIERR.
 *        -# Else
 *          -# Set ret to #PKCL_RET_RSA_SCHEMEERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_RSA_SCHEMEERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_LENGTHERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# If ((pkcl_ret_t)0U == ret) || (#PKCL_CMN_RET_OK_HALF_1 == ret) || (#PKCL_CMN_RET_OK_HALF_2 == ret), then
 *    -# Set ret to #PKCL_RET_TAMPERERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_verify_scheme_pss(const st_pkc_data_t* const p_em,
                                                const st_pkc_data_t* const p_digest,
                                                const st_pkc_rsa_ctx_t* const p_ctx,
                                                const uint32_t mod_len)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    uint32_t external_ret;
    e_pkc_bool_t cmp_ret;
    uint8_t db[PKCL_RSA_KEY_LEN_MAX];
    uint8_t mask[PKCL_RSA_KEY_LEN_MAX];
    uint8_t m_data[PKCL_RSA_PSS_PAD_LEN + PKCL_RSA_KEY_LEN_MAX + PKCL_RSA_SALT_LEN_MAX];
    uint8_t hash_val[PKCL_RSA_HASH_LEN_MAX];
    uint8_t an_hash_val[PKCL_RSA_HASH_LEN_MAX];
    uint8_t salt_val[PKCL_RSA_SALT_LEN_MAX];
    const uint32_t salt_len = p_ctx->p_mask_option->salt_len;
    uint32_t m_len;
    uint32_t hash_len;
    uint32_t db_len;
    uint32_t ps_len;
    uint32_t xor_loop;
    const uint32_t embits_len = (mod_len * PKCL_RSA_OCTET); /* Currently, the corresponding key lengths are 1024,
                                                            2056, 3078, 4096, so embitsLen is (mod_len * 8). */

    /* set hash_len */
    hash_len = r_pkcl_rsa_get_hash_len(p_ctx->p_mask_option->mask_type);

    if (0U != hash_len)
    {
        if (p_em->len >= (hash_len + salt_len + PKCL_RSA_PSS_EM_LEN_CALC))
        {
            /* No problem for casting to support usual arithmetic conversion */
            if (PKCL_RSA_PSS_PADDING_NUM == (uint32_t)p_em->p_data[(p_em->len - 1U)])
            {
                /* separate maskedDB */
                db_len = ((mod_len - hash_len) - 1U);
                r_pkcl_cmn_cpy_byte(db, p_em->p_data, db_len);

                /* separate H */
                r_pkcl_cmn_cpy_byte(hash_val, &p_em->p_data[db_len], hash_len);

                /* No problem for casting to support usual arithmetic conversion */
                if (((uint32_t)p_em->p_data[0] &
                    (~((uint32_t)PKCL_RSA_BIT_CLEAR_NUM >> ((mod_len * PKCL_RSA_OCTET) - /* Same cast as above */
                        (embits_len - 1U))))) == 0U)
                {
                    /* create dbmask */
                    ret = r_pkcl_rsa_mgf1(p_ctx, hash_val, hash_len, mask, db_len);
                    if (PKCL_RET_OK == ret)
                    {
                        /* xor against seed */
                        for (xor_loop = 0U; xor_loop < db_len; xor_loop++ )
                        {
                            /* No problem for casting to support usual arithmetic conversion */
                            db[xor_loop] =  (uint8_t)((uint32_t)db[xor_loop] ^ (uint32_t)mask[xor_loop]);
                        }

                        /* now clear the 8*mod_len - embits_len-1 most significant bits */
                        /* No problem for casting to support usual arithmetic conversion */
                        db[0] = (uint8_t)((uint32_t)db[0] &
                            ((uint32_t)PKCL_RSA_BIT_CLEAR_NUM >> ((mod_len * PKCL_RSA_OCTET) - /* Same cast as above */
                                (embits_len - 1U))));

                        /* separate salt in DB == PS || 0x01 || salt */
                        ps_len = (((mod_len - salt_len) - hash_len) - PKCL_RSA_PSS_PS_LEN_CALC);
                        r_pkcl_cmn_cpy_byte(salt_val, &db[ps_len + 1U], salt_len);

                        /* create M' == (0x)00 00 00 00 00 00 00 00 || mHash || salt */
                        /* Set 0x00 for 8 bytes as padding */
                        r_pkcl_cmn_set_byte(m_data, 0x00U, PKCL_RSA_PSS_PAD_LEN);

                        /* Set p_digest */
                        r_pkcl_cmn_cpy_byte(&m_data[PKCL_RSA_PSS_PAD_LEN], p_digest->p_data, p_digest->len);

                        /* Set salt */
                        r_pkcl_cmn_cpy_byte(&m_data[PKCL_RSA_PSS_PAD_LEN + p_digest->len], salt_val, salt_len);
                        m_len = (PKCL_RSA_PSS_PAD_LEN + p_digest->len + salt_len);

                        /* calc H */
                        external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_init_api(
                                                                        p_ctx->p_mask_option->p_hash_info->p_hash_ctx);
                        if (PKCL_EXTERNAL_API_OK == external_ret)
                        {
                            external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_update_api(
                                                                        p_ctx->p_mask_option->p_hash_info->p_hash_ctx,
                                                                        m_data, m_len);
                            if (PKCL_EXTERNAL_API_OK == external_ret)
                            {
                                external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_final_api(
                                                                        p_ctx->p_mask_option->p_hash_info->p_hash_ctx,
                                                                        an_hash_val, &hash_len);
                                if (PKCL_EXTERNAL_API_OK == external_ret)
                                {
                                    /* Set ret to 0 for double check */
                                    ret = (pkcl_ret_t)0U;

                                    /* Compare H with H' */
                                    cmp_ret = r_pkcl_cmn_cmp_byte(hash_val, an_hash_val, hash_len);
                                    if (PKC_BOOL_TRUE == cmp_ret)
                                    {
                                        ret += PKCL_CMN_RET_OK_HALF_1;

                                        /* Compare H with H' */
                                        cmp_ret = r_pkcl_cmn_cmp_byte(hash_val, an_hash_val, hash_len);

                                        if (PKC_BOOL_TRUE == cmp_ret)
                                        {
                                            ret += PKCL_CMN_RET_OK_HALF_2;
                                        }
                                        else
                                        {
                                            ret = PKCL_RET_RSA_VERIFAIL;
                                        }
                                    }
                                    else
                                    {
                                        ret = PKCL_RET_RSA_VERIFAIL;
                                    }
                                }
                                else
                                {
                                    ret = PKCL_RET_HASHAPIERR;
                                }
                            }
                            else
                            {
                                ret = PKCL_RET_HASHAPIERR;
                            }
                        }
                        else
                        {
                            ret = PKCL_RET_HASHAPIERR;
                        }
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    ret = PKCL_RET_RSA_SCHEMEERR;
                }
            }
            else
            {
                ret = PKCL_RET_RSA_SCHEMEERR;
            }
        }
        else
        {
            ret = PKCL_RET_LENGTHERR;
        }
    }
    else
    {
        ret = PKCL_RET_INTERNALERR;
    }

    /* Set an internal error when an invalid result is set */
    if (((pkcl_ret_t)0U == ret) || (PKCL_CMN_RET_OK_HALF_1 == ret) || (PKCL_CMN_RET_OK_HALF_2 == ret))
    {
        ret = PKCL_RET_TAMPERERR;
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_verify_scheme_pss()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_mgf1
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Calculate the mask used for RSA scheme processing.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_seed        Seed used to calculate mask generation
 * \param       [in]     seed_len      seed length
 * \param       [out]    p_mask        Output mask
 * \param       [in]     mask_len      Length of mask to generate
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_HASHAPIERR:               External HASH API returned error
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Hash the seed and counter values to generate a mask.
 *      - The counter value is converted to big endian by calling #r_pkcl_cmn_ch_big_endian32.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable mask_pos with 0.
 *  -# Set mask_len into the variable mask_length.
 *  -# Set 0 into the variable counter.
 *  -# Loop while ((mask_length > 0) and (#PKCL_RET_OK == ret))
 *    -# Get the big endian format of counter, and set it into ch_counter with #r_pkcl_cmn_ch_big_endian32.
 *    -# Increment counter.
 *    -# Initialize the hash function with p_ctx->p_mask_option->p_hash_info->p_hash_init_api,
 *       and set the return value into external_ret.
 *    -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *      -# Input p_seed into the hash function with p_ctx->p_mask_option->p_hash_info->p_hash_update_api,
 *         and set the return value into external_ret.
 *      -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *        -# Input ch_counter into the hash function with p_ctx->p_mask_option->p_hash_info->p_hash_update_api,
 *           and set the return value into external_ret.
 *        -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *          -# Get the hashed value with p_ctx->p_mask_option->p_hash_info->p_hash_final_api,
 *                 set the hashed value and its length into buf and hash_len respectively,
 *                 and set the return value into external_ret.
 *          -# If (#PKCL_EXTERNAL_API_OK == external_ret)
 *            -# Set ret to #PKCL_RET_OK.
 *          -# Else
 *            -# Set ret to #PKCL_RET_HASHAPIERR.
 *        -# Else
 *          -# Set ret to #PKCL_RET_HASHAPIERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_HASHAPIERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_HASHAPIERR.
 *    -# If (#PKCL_RET_OK == ret)
 *      -# Loop while (store_loop < hash_len) and (mask_length > 0), and increment store_loop for each loop.
 *        -# Set buf[store_loop] into p_mask[mask_pos].
 *        -# Decrement mask_length.
 *        -# Increment mask_pos.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_mgf1(const st_pkc_rsa_ctx_t* const p_ctx, const uint8_t* const p_seed,
                                    const uint32_t seed_len, uint8_t* const p_mask, const uint32_t mask_len)
{
    pkcl_ret_t ret = PKCL_RET_OK;  /* Since nothing may be assigned to ret, set PKCL_RET_OK as the initial value. */
    uint32_t external_ret;
    uint32_t hash_len;
    uint32_t store_loop;
    uint32_t counter;
    uint32_t ch_counter;
    uint32_t mask_pos = 0U;
    uint32_t mask_length = mask_len;
    uint8_t buf[PKCL_RSA_HASH_LEN_MAX];

    /* start counter */
    counter = 0U;

    while ((mask_length > 0U) && (PKCL_RET_OK == ret))
    {
        /* handle counter */
        ch_counter = r_pkcl_cmn_ch_big_endian32(counter);
        counter++;

        /* get hash of p_seed || counter */
        external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_init_api(
                                                                        p_ctx->p_mask_option->p_hash_info->p_hash_ctx);
        if (PKCL_EXTERNAL_API_OK == external_ret)
        {
            external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_update_api(
                                                                        p_ctx->p_mask_option->p_hash_info->p_hash_ctx,
                                                                        p_seed, seed_len);
            if (PKCL_EXTERNAL_API_OK == external_ret)
            {
                external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_update_api(
                                p_ctx->p_mask_option->p_hash_info->p_hash_ctx,
                                (uint8_t*)&ch_counter, sizeof(uint32_t)); /* No problem with casting to pointer size */

                if (PKCL_EXTERNAL_API_OK == external_ret)
                {
                    external_ret = p_ctx->p_mask_option->p_hash_info->p_hash_final_api(
                                                                        p_ctx->p_mask_option->p_hash_info->p_hash_ctx,
                                                                        buf, &hash_len);
                    if (PKCL_EXTERNAL_API_OK == external_ret)
                    {
                        ret = PKCL_RET_OK;
                    }
                    else
                    {
                        ret = PKCL_RET_HASHAPIERR;
                    }
                }
                else
                {
                    ret = PKCL_RET_HASHAPIERR;
                }
            }
            else
            {
                ret = PKCL_RET_HASHAPIERR;
            }
        }
        else
        {
            ret = PKCL_RET_HASHAPIERR;
        }

        if (PKCL_RET_OK == ret)
        {
            /* store it */
            for (store_loop = 0U; (store_loop < hash_len) && (mask_length > 0U); store_loop++ )
            {
                p_mask[mask_pos] = buf[store_loop];
                mask_length--;
                mask_pos++;
            }
        }
        else
        {
            /* Do nothing */
        }
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_mgf1()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_verified_gen_sign
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Verify the signature generated by #R_PKCL_RsaSignInit ~ #R_PKCL_RsaSignFinal.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_sign_data   A structure consisting of the pointer to the signature p_data
 *                                     and its byte length.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaModExpInit
 *  - R_PKCD_RsaModExpOperate
 *  - R_PKCD_RsaModExpFinal
 * \par Precondition
 *  - None
 * \par Detailed description
 *      -
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# Set digest_data to the digest data and its byte length entered in #R_PKCL_RsaSignInit.
 *  -# If #PKC_RSA_PADDING_NO == p_ctx->padding_option, then
 *    -# Encode the plaintext data with #r_pkcl_rsa_padding_no.
 *    -# Set the encoded digest and its byte to digest_data.
 *  -# Else
 *    -# Do nothing.
 *  -# Initialize RSA modular exponentiation operation with R_PKCD_RsaModExpInit, and set drv_ret to the return value.
 *  -# If PKCD_RET_OK == drv_ret, then
 *    -# Calculate (the sinature value in p_sign_data) ^ e mod n, and set drv_ret to the return value.
 *    -# If PKCD_RET_OK == drv_ret, then
 *      -# Get the reslut of the above calculation with R_PKCD_RsaModExpFinal, and set drv_ret to the return value.
 *      -# Set em_data to the result of the above calculation.
 *      -# If PKCD_RET_OK == drv_ret, then
 *        -# If #PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option, then
 *          -# Verify that data in digest_data and em_data are consistent with #r_pkcl_rsa_verify_scheme_pkcs1,
 *             and set ret to the verification result.
 *        -# Else if #PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option, then
 *          -# Verify that data in digest_data and em_data are consistent with #r_pkcl_rsa_verify_scheme_pss,
 *             and set ret to the verification result.
 *        -# Else
 *          -# Set ret to 0 for double check.
 *          -# Compare data in digest_data and em_data with #r_pkcl_cmn_cmp_byte,
 *             and set cmp_ret to the comparison result.
 *          -# If #PKC_BOOL_TRUE == cmp_ret
 *            -# Add #PKCL_CMN_RET_OK_HALF_1 to ret.
 *            -# If #PKC_BOOL_TRUE == cmp_ret, then
 *              -# Add #PKCL_CMN_RET_OK_HALF_2 to ret.
 *            -# Else
 *              -# Do nothing.
 *          -# Else
 *            -# Do nothing.
 *      -# Else
 *        -# Do nothing.
 *    -# Else
 *      -# Do nothing.
 *  -# Else
 *    -# Do nothing.
 *  -# If #PKCL_RET_OK != ret
 *    -# Set ret to #PKCL_RET_TAMPERERR.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_verified_gen_sign(const st_pkc_rsa_ctx_t * const p_ctx,
                                                const st_pkc_data_t * const p_sign_data)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret = PKCD_RET_INTERNALERR;
    st_pkcl_rsa_sign_t * p_rsa_sign;
    st_pkcl_rsa_verified_sign_t * p_rsa_verify;
    st_pkc_data_t digest_data;
    uint8_t * p_buf_drv;
    st_pkc_data_t em_data;
    e_pkc_bool_t cmp_ret;
    st_pkc_rsa_ctx_t ctx_temp;
    const st_pkc_operate_opt_t operate_opt = {PKC_SYNC_OPT_SYNC, PKCL_PROCESSING_BITS_MAX};

    /* Set context */
    ctx_temp = *p_ctx;
    ctx_temp.sec_mod_exp = PKC_BOOL_FALSE;

    /* Cast p_buf to RSA sign buffer type */
    p_rsa_sign = (st_pkcl_rsa_sign_t *)p_ctx->p_buf;

    /* Cast p_buf to RSA verify buffer type */
    p_rsa_verify = (st_pkcl_rsa_verified_sign_t *) &p_ctx->p_buf[sizeof(st_pkcl_rsa_sign_t)];

    /* Clear the buffer */
    r_pkcl_cmn_set_byte((uint8_t *)p_rsa_verify, 0U, sizeof(st_pkcl_rsa_verified_sign_t));

    /* Pass the p_buf address for PKC-Driver */
    p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_sign_t) + sizeof(st_pkcl_rsa_verified_sign_t)];

    digest_data.p_data  = p_rsa_sign->digest;
    digest_data.len     = p_rsa_sign->digest_len;

    /* Set em_data */
    em_data.p_data = p_rsa_verify->em;
    em_data.len    = p_rsa_sign->mod_len;

    if (PKC_RSA_PADDING_NO == p_ctx->padding_option)
    {
        /* If the message size is less than mod_len, 0 padding will be performed. */
        r_pkcl_rsa_padding_no(&digest_data, p_rsa_sign->mod_len, &em_data);
        digest_data.len = em_data.len;
        r_pkcl_cmn_cpy_byte(digest_data.p_data, em_data.p_data, digest_data.len);
    }
    else
    {
        /* Do nothing */
    }

    /* Calculate em with PKC-Driver APIs */
    drv_ret = R_PKCD_RsaModExpInit(p_sign_data, p_rsa_sign->p_priv_key, p_rsa_sign->mod_len,
                                    PKCD_RSA_EXEC_TYPE_VERIFY, &ctx_temp, p_buf_drv);

    if (PKCD_RET_OK == drv_ret)
    {
        drv_ret = R_PKCD_RsaModExpOperate(&ctx_temp, p_buf_drv, &operate_opt);

        if (PKCD_RET_OK == drv_ret)
        {
            drv_ret = R_PKCD_RsaModExpFinal(&ctx_temp, p_buf_drv, &em_data);

            if (PKCD_RET_OK == drv_ret)
            {
                /* Performs scheme processing according to the padding option settings */
                if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
                {
                    ret = r_pkcl_rsa_verify_scheme_pkcs1(&em_data, &digest_data, p_ctx->msg_digest_type,
                                                            p_rsa_sign->mod_len);
                }
                else if (PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option)
                {
                    ret = r_pkcl_rsa_verify_scheme_pss(&em_data, &digest_data, p_ctx, p_rsa_sign->mod_len);
                }
                else /* PKC_RSA_PADDING_NO == p_ctx->padding_option */
                {
                    /* Set ret to 0 for double check */
                    ret = (pkcl_ret_t)0U;

                    cmp_ret = r_pkcl_cmn_cmp_byte(em_data.p_data, digest_data.p_data, em_data.len);

                    if (PKC_BOOL_TRUE == cmp_ret)
                    {
                        ret += PKCL_CMN_RET_OK_HALF_1;

                        cmp_ret = r_pkcl_cmn_cmp_byte(em_data.p_data, digest_data.p_data, em_data.len);

                        if (PKC_BOOL_TRUE == cmp_ret)
                        {
                            ret += PKCL_CMN_RET_OK_HALF_2;
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

    if (PKCL_RET_OK != ret)
    {
        ret = PKCL_RET_TAMPERERR;
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_verified_gen_sign()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_rsa_verified_gen_plain
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIsInternal
 * \brief       Verify the plaintext generated by #R_PKCL_RsaDecryptInit ~ #R_PKCL_RsaDecryptFinal.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_plain_data  A structure consisting of plaintext byte length len and data pointer p_data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaModExpInit
 *  - R_PKCD_RsaModExpOperate
 *  - R_PKCD_RsaModExpFinal
 * \par Precondition
 *  - None
 * \par Detailed description
 *      -
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# Set cipher_data_in to the ciphertext and its length entered in #R_PKCL_RsaDecryptInit.
 *  -# If #PKC_RSA_PADDING_NO == p_ctx->padding_option, then
 *    -# Encode the plaintext data with #r_pkcl_rsa_padding_no, and set ret to #PKCL_RET_OK.
 *  -# Else if #PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option, then
 *    -# Encode the plaintext data with #r_pkcl_rsa_enc_scheme_pkcs1, and set temp_ret to the return value.
 *  -# Else
 *    -# Encode the plaintext data with #r_pkcl_rsa_enc_scheme_oaep, and set temp_ret to the return value.
 *  -# If #PKCL_RET_OK == temp_ret, then
 *    -# Initialize RSA modular exponentiation operation with R_PKCD_RsaModExpInit,
 *       and set drv_ret to the return value.
 *    -# If PKCD_RET_OK == drv_ret, then
 *      -# Calculate ciphertext = (encoded message) ^ e mod n with R_PKCD_RsaModExpOperate,
 *         and set drv_ret to the return value.
 *      -# Get the generated ciphertext with R_PKCD_RsaModExpFinal and set drv_ret to the return value.
 *      -# Set cipher_data_out to the generated data and its byte length.
 *  -# If PKCD_RET_OK == drv_ret, then
 *    -# Set ret to 0 for double check.
 *    -# If cipher_data_in.len == cipher_data_out.len
 *      -# Compare data in cipher_data_out.p_data and cipher_data_in.p_data with #r_pkcl_cmn_cmp_byte,
 *         and set cmp_ret to the comparison result.
 *      -# If #PKC_BOOL_TRUE == cmp_ret
 *        -# Add #PKCL_CMN_RET_OK_HALF_1 to ret.
 *        -# Compare data in cipher_data_out.p_data and cipher_data_in.p_data with #r_pkcl_cmn_cmp_byte,
 *           and set cmp_ret to the comparison result.
 *        -# If #PKC_BOOL_TRUE == cmp_ret, then
 *          -# Add #PKCL_CMN_RET_OK_HALF_2 to ret.
 *        -# Else
 *            -# Do nothing.
 *      -# Else
 *        -# Do nothing.
 *    -# Else
 *      -# Do nothing.
 *  -# Else
 *    -# Do nothing.
 *  -# If #PKCL_RET_OK != ret
 *    -# Set ret to #PKCL_RET_TAMPERERR.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_rsa_verified_gen_plain(const st_pkc_rsa_ctx_t * const p_ctx,
                                                const st_pkc_data_t * const p_plain_data)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcl_ret_t temp_ret;
    pkcd_ret_t drv_ret = PKCD_RET_INTERNALERR;
    st_pkcl_rsa_dec_t * p_rsa_dec;
    st_pkcl_rsa_verified_plain_t * p_rsa_enc;
    st_pkc_data_t cipher_data_in;
    st_pkc_data_t cipher_data_out;
    st_pkc_data_t em_data;
    uint8_t * p_buf_drv;
    st_pkc_rsa_ctx_t ctx_temp;
    const st_pkc_operate_opt_t operate_opt = {PKC_SYNC_OPT_SYNC, PKCL_PROCESSING_BITS_MAX};
    e_pkc_bool_t cmp_ret;

    /* Set context */
    ctx_temp = *p_ctx;
    ctx_temp.sec_mod_exp = PKC_BOOL_FALSE;

    /* Cast p_buf to RSA decryption buffer type */
    p_rsa_dec = (st_pkcl_rsa_dec_t *)p_ctx->p_buf;

    /* Cast p_buf to RSA encryption (for verification of generated plaintext) buffer type */
    p_rsa_enc = (st_pkcl_rsa_verified_plain_t *) &p_ctx->p_buf[sizeof(st_pkcl_rsa_dec_t)];

    /* Pass the p_buf address for PKC-Driver */
    p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_rsa_dec_t) + sizeof(st_pkcl_rsa_verified_plain_t)];

    /* Set cipher data to be compared */
    cipher_data_in.p_data  = p_rsa_dec->cipher;
    cipher_data_in.len     = p_rsa_dec->cipher_len;

    em_data.len    = p_rsa_dec->mod_len;
    em_data.p_data = p_rsa_enc->em;

    if (PKC_RSA_PADDING_NO == p_ctx->padding_option)
    {
        /* If the message size is less than mod_len, 0 padding will be performed. */
        r_pkcl_rsa_padding_no(p_plain_data, p_rsa_dec->mod_len, &em_data);
        temp_ret = PKCL_RET_OK;
    }
    else if (PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option)
    {
        /* Execute the scheme of EME-PKCS1-v1_5 */
        /*  p_rsa_enc->cipher stores the PS value obtained using r_pkcl_rsa_dec_scheme_pkcs1. */
        temp_ret = r_pkcl_rsa_enc_scheme_pkcs1(p_plain_data, p_ctx, p_rsa_dec->mod_len, &em_data,
                                                p_rsa_enc->cipher);
    }
    else /* PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option */
    {
        /* Execute the scheme of EME-OAEP */
        /*  p_rsa_enc->cipher stores the seed value obtained using r_pkcl_rsa_dec_scheme_oaep. */
        temp_ret = r_pkcl_rsa_enc_scheme_oaep(p_plain_data, p_ctx, p_rsa_dec->mod_len, &em_data,
                                                p_rsa_enc->cipher);
    }

    if (PKCL_RET_OK == temp_ret)
    {
        /* Call PKC-Driver API */
        drv_ret = R_PKCD_RsaModExpInit(&em_data, p_rsa_dec->p_priv_key, p_rsa_dec->mod_len,
                                        PKCD_RSA_EXEC_TYPE_ENC, &ctx_temp, p_buf_drv);

        if (PKCD_RET_OK == drv_ret)
        {
            drv_ret = R_PKCD_RsaModExpOperate(&ctx_temp, p_buf_drv, &operate_opt);

            if (PKCD_RET_OK == drv_ret)
            {
                cipher_data_out.p_data = p_rsa_enc->cipher;
                cipher_data_out.len    = sizeof(p_rsa_enc->cipher);

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_RsaModExpFinal(p_ctx, p_buf_drv, &cipher_data_out);
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

    if (PKCD_RET_OK == drv_ret)
    {
        /* Set ret to 0 for double check */
        ret = (pkcl_ret_t)0U;

        if ((cipher_data_in.len) == (cipher_data_out.len))
        {
            cmp_ret = r_pkcl_cmn_cmp_byte(cipher_data_out.p_data, cipher_data_in.p_data, p_rsa_dec->mod_len);

            if (PKC_BOOL_TRUE == cmp_ret)
            {
                ret += PKCL_CMN_RET_OK_HALF_1;

                cmp_ret = r_pkcl_cmn_cmp_byte(cipher_data_out.p_data, cipher_data_in.p_data, p_rsa_dec->mod_len);

                if (PKC_BOOL_TRUE == cmp_ret)
                {
                    ret += PKCL_CMN_RET_OK_HALF_2;
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
    }
    else
    {
        /* Do nothing */
    }

    if (PKCL_RET_OK != ret)
    {
        ret = PKCL_RET_TAMPERERR;
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_rsa_verified_gen_plain()
**********************************************************************************************************************/
