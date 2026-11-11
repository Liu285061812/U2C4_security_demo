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
/* PRQA S 0311,0316 ++ # D004 The cast needed to pass to the driver API.
                         There is no problem because it recognizes that the cast value is passed on the driver side. */
/* PRQA S 1503 ++ # D010 No problem if it is unused because it is an API called from outside the library  */
/* PRQA S 3383 ++ # D012 Have confirmed in advance that the value doesn't wrap around. */
/* PRQA S 2981 ++ # D013 Local variable ret of APIs sets INTERNALERR to the initial value
                        as a countermeasure against attacks that pass through processing. */
/* PRQA S 2984 ++ # D014 Because it is calculated in consideration of future expansion */
/* PRQA S 3673 ++ # D017 Because future extensions may edit values in context. */
/* PRQA S 5002 ++ # D019 There is no problem because it is used in a system that can distinguish
                    file names of 8 characters or more. */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "renesas_types.h"
#include "r_pkc_api.h"
#include "r_pkcl_cfg.h"
#include "r_pkcl_api.h"
#include "r_pkcl_cmn.h"

/*!********************************************************************
 * \addtogroup    PKCLEDDSA
 * \{
 * \addtogroup    PKCLEDDSADefines             EDDSA Definitions
 * \addtogroup    PKCLEDDSATypes               EDDSA Types
 * \addtogroup    PKCLEDDSAConstants           EDDSA Constants
 * \addtogroup    PKCLEDDSAAPIs                EDDSA APIs
 * \addtogroup    PKCLEDDSADefinesInternal     EDDSA Internal Definitions
 * \addtogroup    PKCLEDDSATypesInternal       EDDSA Internal Types
 * \addtogroup    PKCLEDDSAVariablesInternal   EDDSA Internal Variables
 * \addtogroup    PKCLEDDSAAPIsInternal        EDDSA Internal APIs
 * \}******************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \addtogroup PKCLEDDSADefinesInternal
 * \{*****************************************************************/
#define PKCL_EDDSA_ED25519_CURVE_LEN    (32U)  /**< Curve length Ed25519 */
#define PKCL_EDDSA_ED448_CURVE_LEN      (56U)  /**< Curve length Ed448 */

#define PKCL_EDDSA_ED25519_DIGEST_LEN   (64U)  /**< Ed25519 digest length  with SHA-512 */
#define PKCL_EDDSA_ED448_DIGEST_LEN     (116U) /**< Ed448 digest length  with SHAKE256 */

#define PKCL_EDDSA_SHAKE256BYTES        (114U) /**< SHAKE256 byte size used by Ed448 */
#define PKCL_EDDSA_SHAKE256BYTES_PH     (64U)  /**< SHAKE256 byte size used by Ed448 Pre Hash */

#define PKCL_EDDSA_ED25519_KEY_LEN      (32U)  /**< Edwards25519 key length */
#define PKCL_EDDSA_ED448_KEY_LEN        (57U)  /**< Edwards448 key length */
#define PKCL_EDDSA_ED448_RU4_KEY_LEN    (60U)  /**< Ed448 key length rounded up to a multiple of 4 */

#define PKCL_EDDSA_CURVE_LEN_MAX        (PKCL_EDDSA_ED448_CURVE_LEN)   /**< Maximum length of curve */
#define PKCL_EDDSA_DIGEST_LEN_MAX       (PKCL_EDDSA_ED448_DIGEST_LEN)  /**< Maximum length of message digest */
#define PKCL_EDDSA_KEY_LEN_MAX          (PKCL_EDDSA_ED448_RU4_KEY_LEN) /**< Maximum length of key length */

#define PKCL_EDDSA_UNCOMP_LEN_MAX       (PKCL_EDDSA_CURVE_LEN_MAX * 3U) /**< Maximum length of uncompress data */

#define PKCL_EDDSA_CURVE_PARAM_NUM      (8U)   /**< Number of EdDSA curve parameters */
#define PKCL_EDDSA_DOUBLE_LEN           (2U)   /**< Definition to calculate twice the length */
#define PKCL_EDDSA_HALF_LEN             (2U)   /**< Definition to calculate half length */

#define PKCL_EDDSA_DATA_CHK_IN_ARG      (0U) /**< If the argument to check is an input argument */
#define PKCL_EDDSA_DATA_CHK_OUT_ARG     (1U) /**< If the argument to check is an output argument */

#define PKCL_EDDSA_MASK_F8 (0xF8U) /**< the value used for mask the scalar value */
#define PKCL_EDDSA_MASK_3F (0x3FU) /**< the value used for mask the scalar value */
#define PKCL_EDDSA_MASK_40 (0x40U) /**< the value used for mask the scalar value */
#define PKCL_EDDSA_MASK_FC (0xFCU) /**< the value used for mask the scalar value */
#define PKCL_EDDSA_MASK_80 (0x80U) /**< the value used for mask the scalar value */

#define PKCL_EDDSA_ZERO_PADDING_3   (3U)   /**< Set zero padding for 3 octets on Ed448 */

#define PKCL_EDDSA_TRIPLE_LEN       (3U)   /**< Definition to calculate length times three */

#define PKCL_EDDSA_ED448_SECOND_TO_LAST_IDX   (PKCL_EDDSA_ED448_KEY_LEN - 2U)  /**< Second to last index */

#define PKCL_EDDSA_ED_CTX_LEN_MIN   (1U)   /**< Minimum Ed context length when Ed context is required */
#define PKCL_EDDSA_ED_CTX_LEN_MAX   (255U) /**< Maximum length of Ed context */

#define PKCL_EDDSA_VERIFY_OPE_1ST_PROCESSING  (0U)  /**< R_PKCL_EddsaVerifyOperate state flag, 1st processing */
#define PKCL_EDDSA_VERIFY_OPE_1ST_DONE        (1U)  /**< R_PKCL_EddsaVerifyOperate state flag, 1st done */
#define PKCL_EDDSA_VERIFY_OPE_2ND_PROCESSING  (2U)  /**< R_PKCL_EddsaVerifyOperate state flag, 2nd processing */

#define PKCL_EDDSA_MSG_LEN_MAX (0x80000000U) /**< Maximum message length */
/*! \}*/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \addtogroup PKCLEDDSATypesInternal
 * \{*****************************************************************/
/** A structure of values that needs to be buffered in EdDSA Signature generation */
typedef struct
{
    const void * p_priv_key;
    const void * p_pub_key;
    uint32_t     scalar_s_len;
    uint8_t      scalar_s_data[PKCL_EDDSA_KEY_LEN_MAX];
    uint32_t     scalar_r_len;
    uint8_t      scalar_r_data[PKCL_EDDSA_DIGEST_LEN_MAX];
    const uint8_t * p_msg;
    uint32_t     msg_len;
    uint8_t      hash_msg_data[PKCL_EDDSA_DIGEST_LEN_MAX];
    uint32_t     hash_msg_len;
} st_pkcl_eddsa_sign_t;

/** A structure of values that needs to be buffered in EdDSA Signature verification */
typedef struct
{
    uint32_t sign_len;
    uint8_t  sign_data[PKCL_EDDSA_KEY_LEN_MAX * PKCL_EDDSA_DOUBLE_LEN];
    const void * p_pub_key;
    uint32_t ha_len;
    uint8_t  ha_data[PKCL_EDDSA_UNCOMP_LEN_MAX];
    const uint8_t * p_msg;
    uint32_t msg_len;
    uint32_t ope_flg;
} st_pkcl_eddsa_verify_t;

/** A structure of values that needs to be buffered in EdDSA key generation */
typedef struct
{
    uint32_t priv_scalar_len;
    uint8_t  priv_scalar_data[PKCL_EDDSA_KEY_LEN_MAX];
    uint32_t hash_scalar_len;
    uint8_t  hash_scalar_data[PKCL_EDDSA_DIGEST_LEN_MAX];
} st_pkcl_eddsa_keygen_t;

/*! \}*/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_curve (const st_pkc_ec_curve_t * const p_curve);
static pkcl_ret_t r_pkcl_eddsa_chk_msg(const st_pkc_data_t *const p_pkc_data);
static pkcl_ret_t r_pkcl_eddsa_chk_point(const st_pkc_data_t *const p_pkc_data, const uint32_t curve_len,
                                            const e_pkc_bool_t comp_flag, const uint32_t arg_type);
static pkcl_ret_t r_pkcl_eddsa_chk_scalar(const st_pkc_data_t *const p_pkc_data, const uint32_t curve_len,
                                            const uint32_t arg_type);
static pkcl_ret_t r_pkcl_eddsa_chk_sign(const st_pkc_data_t *const p_pkc_data, const uint32_t curve_len,
                                        const uint32_t arg_type);
static pkcl_ret_t r_pkcl_eddsa_chk_digest(const st_pkc_data_t *const p_pkc_data, const uint32_t curve_len,
                                            const uint32_t arg_type);
static pkcl_ret_t r_pkcl_eddsa_chk_select_data (const st_pkc_select_data_t * const p_select_data,
                                                const uint32_t curve_len);
static pkcl_ret_t r_pkcl_eddsa_chk_variant (const e_pkc_eddsa_variant_t eddsa_variant,
                                            const st_pkc_data_t * const p_ed_ctx, const uint32_t curve_len);
static pkcl_ret_t r_pkcl_eddsa_chk_sign_init_arg (const st_pkc_data_t * const p_msg,
                                                    const void * const p_priv_key,
                                                    const void * const p_pub_key,
                                                    const st_pkc_eddsa_ctx_t * const p_ctx);
static pkcl_ret_t r_pkcl_eddsa_chk_sign_verify_operate_arg (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                            const uint32_t processing_bits);
static pkcl_ret_t r_pkcl_eddsa_chk_sign_final_arg (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                    const st_pkc_data_t * const p_sign);
static pkcl_ret_t r_pkcl_eddsa_chk_verify_init_arg (const st_pkc_data_t * const p_msg,
                                                    const st_pkc_data_t * const p_sign,
                                                    const void * const p_pub_key,
                                                    const st_pkc_eddsa_ctx_t * const p_ctx);
static pkcl_ret_t r_pkcl_eddsa_chk_keygen_operate_arg (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                        const uint32_t processing_bits);
static pkcl_ret_t r_pkcl_eddsa_chk_keygen_final_arg (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                    const void * const p_priv_key,
                                                    const void * const p_pub_key);
static pkcl_ret_t r_pkcl_eddsa_chk_scalar_mult_operate_arg (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                            const uint32_t processing_bits);
static pkcl_ret_t r_pkcl_eddsa_verified_gen_sign (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                    const st_pkc_data_t * const p_sign_r_data,
                                                    const st_pkc_data_t * const p_sign_s_data);
static pkcl_ret_t r_pkcl_eddsa_verify_operate_processing (const st_pkc_eddsa_ctx_t *const p_ctx,
                                                        const uint32_t processing_bits);
static pkcl_ret_t r_pkcl_eddsa_verify_operate_init_done (const st_pkc_eddsa_ctx_t *const p_ctx,
                                                        const uint32_t processing_bits);

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaSignInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EddsaSignInit(const st_pkc_data_t *const p_msg, const void *const p_priv_key,
                                const void *const p_pub_key, st_pkc_eddsa_ctx_t *const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_EDDSA_SIGN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkcl_state_t pkcl_state;
    st_pkcl_eddsa_sign_t * p_eddsa_sign;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_eddsa_chk_sign_init_arg(p_msg, p_priv_key, p_pub_key, p_ctx);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to EdDSA sign buffer type */
            p_eddsa_sign = (st_pkcl_eddsa_sign_t *)p_ctx->p_buf;

            /* buffer clear */
            r_pkcl_cmn_set_byte(p_eddsa_sign->scalar_s_data, 0U, sizeof(p_eddsa_sign->scalar_s_data));
            r_pkcl_cmn_set_byte(p_eddsa_sign->scalar_r_data, 0U, sizeof(p_eddsa_sign->scalar_r_data));

            /* Save p_msg */
            p_eddsa_sign->p_msg = p_msg->p_data;
            p_eddsa_sign->msg_len = p_msg->len;

            /* Save p_priv_key */
            p_eddsa_sign->p_priv_key = p_priv_key;

            /* Save p_pub_key */
            p_eddsa_sign->p_pub_key = p_pub_key;

            /* State transition */
            r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_SIGN_INIT_DONE);
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

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_EDDSA_SIGN != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_EDDSA_SIGN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EddsaSignInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaSignOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EddsaSignOperate(st_pkc_eddsa_ctx_t *const p_ctx, const uint32_t processing_bits)
{
#if (PKCL_CFG_ENABLE_DRV_EDDSA_SIGN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    uint32_t hash_ret;
    void * p_hash_ctx;
    e_pkcl_state_t pkcl_state;
    st_pkcl_eddsa_sign_t * p_eddsa_sign;
    st_pkc_data_t msg_data;
    st_pkc_data_t hmsg_data;
    st_pkc_data_t scalar_s_data;
    st_pkc_data_t scalar_r_data;
    st_pkc_select_data_t scalar_r_sel;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EDDSA_SIGN_INIT_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_eddsa_chk_sign_verify_operate_arg(p_ctx, processing_bits);

        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to EdDSA sign buffer type */
            p_eddsa_sign = (st_pkcl_eddsa_sign_t *)p_ctx->p_buf;

            /* Prepare to pass message to PKC-Driver */
            msg_data.p_data = (uint8_t *)p_eddsa_sign->p_msg;
            msg_data.len = p_eddsa_sign->msg_len;

            /* Prepare to pass scalar s to PKC-Driver */
            scalar_s_data.p_data = p_eddsa_sign->scalar_s_data;
            scalar_s_data.len = sizeof(p_eddsa_sign->scalar_s_data);

            /* Prepare to pass scalar r to PKC-Driver */
            scalar_r_data.p_data = p_eddsa_sign->scalar_r_data;
            scalar_r_data.len = sizeof(p_eddsa_sign->scalar_r_data);

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_eddsa_sign_t)];

            /* HashEdDSA pre-hashing */
            if (PKC_EDDSA_VARIANT_HASH == p_ctx->eddsa_variant)
            {
                if (PKCL_EDDSA_ED448_CURVE_LEN == p_ctx->p_curve_info->curve_len)
                {
                    hmsg_data.len = PKCL_EDDSA_SHAKE256BYTES_PH;
                }
                else
                {
                    /* Do nothing */
                }

                p_hash_ctx = p_ctx->p_hash_info->p_hash_ctx;

                hash_ret = p_ctx->p_hash_info->p_hash_init_api(p_hash_ctx);
                if (PKCL_EXTERNAL_API_OK == hash_ret)
                {
                    hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_hash_ctx,
                                                                    msg_data.p_data,
                                                                    msg_data.len);
                    if (PKCL_EXTERNAL_API_OK == hash_ret)
                    {
                        hmsg_data.p_data = p_eddsa_sign->hash_msg_data;
                        hash_ret = p_ctx->p_hash_info->p_hash_final_api(p_hash_ctx, hmsg_data.p_data,
                                                                        &hmsg_data.len);
                        if (PKCL_EXTERNAL_API_OK == hash_ret)
                        {
                            /* Save hash message */
                            p_eddsa_sign->hash_msg_len = hmsg_data.len;

                            /* Call PKC-Driver API */
                            drv_ret = R_PKCD_EddsaMakeScalarSandR(p_ctx, p_buf_drv, p_eddsa_sign->p_priv_key,
                                                                    &hmsg_data, &scalar_s_data, &scalar_r_data);

                            /* Convert to PKC-Lib return code */
                            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
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
            else if ((PKC_EDDSA_VARIANT_PURE == p_ctx->eddsa_variant) ||
                    (PKC_EDDSA_VARIANT_CTX == p_ctx->eddsa_variant))
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EddsaMakeScalarSandR(p_ctx, p_buf_drv, p_eddsa_sign->p_priv_key,
                                                        &msg_data, &scalar_s_data, &scalar_r_data);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
            }
            else
            {
                ret = PKCL_RET_INTERNALERR;
            }

            if (PKCL_RET_OK == ret)
            {
                /* Save scalar s,r */
                p_eddsa_sign->scalar_s_len = scalar_s_data.len;
                p_eddsa_sign->scalar_r_len = scalar_r_data.len;

                /* Prepare to pass sacalar r to PKC-Driver */
                scalar_r_sel.data_type = PKC_DATA_TYPE_BYTE;
                scalar_r_sel.p_sel_data = &scalar_r_data;

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EddsaScalarMultBaseInit(&scalar_r_sel, p_ctx, p_buf_drv);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                if (PKCL_RET_OK == ret)
                {
                    /* Call PKC-Driver API */
                    drv_ret = R_PKCD_EddsaScalarMultBaseOperate(p_ctx, p_buf_drv, processing_bits);

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
            /* Do nothing */
        }
    }
    else if (PKCL_STATE_EDDSA_SIGN_OPERATE_PROCESSING == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_eddsa_chk_sign_verify_operate_arg(p_ctx, processing_bits);

        if (PKCL_RET_OK == ret)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_eddsa_sign_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultBaseOperate(p_ctx, p_buf_drv, processing_bits);

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
        r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_SIGN_OPERATE_DONE);
    }
    else if (PKCL_RET_PROCESSING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_SIGN_OPERATE_PROCESSING);
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
#else  /* PKCL_CFG_ENABLE_DRV_EDDSA_SIGN != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_EDDSA_SIGN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EddsaSignOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaSignFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EddsaSignFinal(const st_pkc_eddsa_ctx_t *const p_ctx, st_pkc_data_t *const p_sign)
{
#if (PKCL_CFG_ENABLE_DRV_EDDSA_SIGN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_eddsa_sign_t * p_eddsa_sign;
    st_pkc_data_t msg_data;
    st_pkc_data_t sign_s_data;
    st_pkc_data_t sign_r_data;
    st_pkc_data_t scalar_s_data;
    st_pkc_data_t scalar_r_data;
    st_pkc_data_t hash_ram_data;
    uint8_t sign_s_val[PKCL_EDDSA_ED448_RU4_KEY_LEN] = {0};
    uint8_t sign_r_val[PKCL_EDDSA_ED448_RU4_KEY_LEN] = {0};
    uint8_t hash_ram_val[PKCL_EDDSA_DIGEST_LEN_MAX] = {0};
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EDDSA_SIGN_OPERATE_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_eddsa_chk_sign_final_arg(p_ctx, p_sign);

        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to EdDSA sign buffer type */
            p_eddsa_sign = (st_pkcl_eddsa_sign_t *)p_ctx->p_buf;

            /* Prepare to pass sign r to PKC-Driver */
            sign_r_data.p_data = sign_r_val;
            sign_r_data.len = sizeof(sign_r_val);

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_eddsa_sign_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultBaseFinal(p_ctx, p_buf_drv, PKC_BOOL_TRUE, &sign_r_data);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* If HashEdDSA variant */
                if ((PKC_EDDSA_VARIANT_HASH == p_ctx->eddsa_variant) ||
                    (PKC_EDDSA_VARIANT_PURE == p_ctx->eddsa_variant) ||
                    (PKC_EDDSA_VARIANT_CTX == p_ctx->eddsa_variant))
                {
                    if (PKC_EDDSA_VARIANT_HASH == p_ctx->eddsa_variant)
                    {
                        /* Prepare to pass hash message to PKC-Driver */
                        msg_data.p_data = p_eddsa_sign->hash_msg_data;
                        msg_data.len = p_eddsa_sign->hash_msg_len;
                    }
                    else
                    {
                        /* Prepare to pass message to PKC-Driver */
                        msg_data.p_data = (uint8_t *)p_eddsa_sign->p_msg;
                        msg_data.len = p_eddsa_sign->msg_len;
                    }

                    /* Prepare to pass H(R,A,M) s to PKC-Driver */
                    hash_ram_data.p_data = hash_ram_val;
                    hash_ram_data.len = sizeof(hash_ram_val);

                    /* Call PKC-Driver API */
                    drv_ret = R_PKCD_EddsaHashRAM(p_ctx, p_buf_drv, &sign_r_data,
                                                    p_eddsa_sign->p_pub_key, &msg_data, &hash_ram_data);

                    /* Convert to PKC-Lib return code */
                    ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
                }
                else
                {
                    ret = PKCL_RET_INTERNALERR;
                }

                if (PKCL_RET_OK == ret)
                {
                    /* Prepare to pass scalar s to PKC-Driver */
                    scalar_s_data.p_data = p_eddsa_sign->scalar_s_data;
                    scalar_s_data.len = p_eddsa_sign->scalar_s_len;

                    /* Prepare to pass scalar r to PKC-Driver */
                    scalar_r_data.p_data = p_eddsa_sign->scalar_r_data;
                    scalar_r_data.len = p_eddsa_sign->scalar_r_len;

                    /* Prepare to pass sign s to PKC-Driver */
                    sign_s_data.p_data = sign_s_val;
                    sign_s_data.len = sizeof(sign_s_val);

                    /* Call PKC-Driver API */
                    drv_ret = R_PKCD_EddsaDeriveS(p_ctx, p_buf_drv, &scalar_s_data, &scalar_r_data,
                                                    &hash_ram_data, &sign_s_data);

                    /* Convert to PKC-Lib return code */
                    ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                    if (PKCL_RET_OK == ret)
                    {
                        if (PKC_BOOL_FALSE != p_ctx->verifed_gen_sign)
                        {
                            ret = r_pkcl_eddsa_verified_gen_sign(p_ctx, &sign_r_data, &sign_s_data);
                        }
                        else
                        {
                            /* Do nothing */
                        }

                        if (PKCL_RET_OK == ret)
                        {
                            /* Sign R,S output */
                            r_pkcl_cmn_cpy_byte(&p_sign->p_data[0], sign_r_data.p_data, sign_r_data.len);
                            r_pkcl_cmn_cpy_byte(&p_sign->p_data[sign_r_data.len], sign_s_data.p_data, sign_s_data.len);
                            p_sign->len = (sign_r_data.len + sign_s_data.len);
                        }
                        else
                        {
                            /* Do nothing */
                        }

                        /* signature clear */
                        r_pkcl_cmn_set_byte(sign_r_val, 0U, sizeof(sign_r_val));
                        r_pkcl_cmn_set_byte(sign_s_val, 0U, sizeof(sign_s_val));
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
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if ((PKCL_RET_OK == ret) || (PKCL_RET_EDDSA_ZERO_SIGNATURE == ret) ||
        (PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_EDDSA_SIGN != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_EDDSA_SIGN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EddsaSignFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaVerifyInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EddsaVerifyInit(const st_pkc_data_t *const p_msg, const st_pkc_data_t *const p_sign,
                                    const void *const p_pub_key, st_pkc_eddsa_ctx_t *const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkcl_state_t pkcl_state;
    st_pkcl_eddsa_verify_t * p_eddsa_verify;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_eddsa_chk_verify_init_arg(p_msg, p_sign, p_pub_key, p_ctx);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to EdDSA verify buffer type */
            p_eddsa_verify = (st_pkcl_eddsa_verify_t *)p_ctx->p_buf;

            /* buffer clear */
            r_pkcl_cmn_set_byte(p_eddsa_verify->sign_data, 0U, sizeof(p_eddsa_verify->sign_data));
            r_pkcl_cmn_set_byte(p_eddsa_verify->ha_data, 0U, sizeof(p_eddsa_verify->ha_data));

            /* Save p_msg */
            p_eddsa_verify->p_msg = p_msg->p_data;
            p_eddsa_verify->msg_len = p_msg->len;

            /* Save p_sign */
            p_eddsa_verify->sign_len = p_sign->len;
            r_pkcl_cmn_cpy_byte(&p_eddsa_verify->sign_data[0], p_sign->p_data, p_eddsa_verify->sign_len);

            /* Save p_pub_key */
            p_eddsa_verify->p_pub_key = p_pub_key;

            /* Initialize ope_flg */
            p_eddsa_verify->ope_flg = PKCL_EDDSA_VERIFY_OPE_1ST_PROCESSING;

            /* State transition */
            r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_VERIFY_INIT_DONE);
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

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EddsaVerifyInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaVerifyOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EddsaVerifyOperate(const st_pkc_eddsa_ctx_t *const p_ctx, const uint32_t processing_bits)
{
#if (PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EDDSA_VERIFY_INIT_DONE == pkcl_state)
    {
        /* Operate process */
        ret = r_pkcl_eddsa_verify_operate_init_done(p_ctx, processing_bits);
    }
    else if (PKCL_STATE_EDDSA_VERIFY_OPERATE_PROCESSING == pkcl_state)
    {
        /* Operate process */
        ret = r_pkcl_eddsa_verify_operate_processing(p_ctx, processing_bits);
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if ((PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EddsaVerifyOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaVerifyFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EddsaVerifyFinal(const st_pkc_eddsa_ctx_t *const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_eddsa_verify_t * p_eddsa_verify;
    st_pkc_data_t ha_data;
    st_pkc_data_t sb_data;
    st_pkc_data_t sign_r_data;
    uint8_t sign_r_val[PKCL_EDDSA_ED448_RU4_KEY_LEN] = {0};
    uint8_t sb_val[PKCL_EDDSA_UNCOMP_LEN_MAX] = {0};
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EDDSA_VERIFY_OPERATE_DONE == pkcl_state)
    {
        /* check if the argument is null */
        if (NULL != p_ctx)
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_hash_info) && (NULL != p_ctx->p_curve_info))
            {
                /* check hash info */
                if ((NULL != p_ctx->p_hash_info->p_hash_init_api) &&
                    (NULL != p_ctx->p_hash_info->p_hash_update_api) &&
                    (NULL != p_ctx->p_hash_info->p_hash_final_api))
                {
                    /* check curve info */
                    ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
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

        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to EdDSA verify buffer type */
            p_eddsa_verify = (st_pkcl_eddsa_verify_t *)p_ctx->p_buf;

            /* Prepare to pass SB to PKC-Driver */
            sb_data.p_data = sb_val;
            sb_data.len = sizeof(sb_val);

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_eddsa_verify_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultBaseFinal(p_ctx, p_buf_drv, PKC_BOOL_FALSE, &sb_data);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Prepare to pass hA to PKC-Driver */
                ha_data.p_data = p_eddsa_verify->ha_data;
                ha_data.len = p_eddsa_verify->ha_len;

                /* Prepare to pass sign r to PKC-Driver */
                sign_r_data.len = (p_eddsa_verify->sign_len / PKCL_EDDSA_HALF_LEN);
                r_pkcl_cmn_cpy_byte(sign_r_val, &p_eddsa_verify->sign_data[0], sign_r_data.len);
                sign_r_data.p_data = sign_r_val;

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EddsaCompareSign(p_ctx, p_buf_drv, &ha_data, &sb_data, &sign_r_data);

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

    /* State transition */
    if ((PKCL_RET_OK == ret) || (PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret) ||
        (PKCL_RET_EDDSA_VERIFAIL == ret) || (PKCL_RET_EDDSA_DECOMPERR == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EddsaVerifyFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaKeyGenInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EddsaKeyGenInit(st_pkc_eddsa_ctx_t *const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_EDDSA_KEYGEN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkcl_state_t pkcl_state;
    st_pkcl_eddsa_keygen_t * p_eddsa_keygen;
    uint32_t external_ret;
    uint32_t curve_len;
    uint8_t * p_hscalar;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* check if the argument is null */
        if (NULL != p_ctx)
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_hash_info) &&
                (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info))
            {
                /* check hash info */
                if ((NULL != p_ctx->p_hash_info->p_hash_init_api) &&
                    (NULL != p_ctx->p_hash_info->p_hash_update_api) &&
                    (NULL != p_ctx->p_hash_info->p_hash_final_api))
                {
                    /* check curve info */
                    ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
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

        if (PKCL_RET_OK == ret)
        {

            /* Cast p_buf to EdDSA keygen buffer type */
            p_eddsa_keygen = (st_pkcl_eddsa_keygen_t *)p_ctx->p_buf;

            /* buffer clear */
            r_pkcl_cmn_set_byte(p_eddsa_keygen->priv_scalar_data, 0U, sizeof(p_eddsa_keygen->priv_scalar_data));
            r_pkcl_cmn_set_byte(p_eddsa_keygen->hash_scalar_data, 0U, sizeof(p_eddsa_keygen->hash_scalar_data));

            if (PKCL_EDDSA_ED25519_CURVE_LEN == p_ctx->p_curve_info->curve_len)
            {
                p_eddsa_keygen->priv_scalar_len = PKCL_EDDSA_ED25519_KEY_LEN;
            }
            else
            {
                /* set ed448 option value */
                p_eddsa_keygen->hash_scalar_len = PKCL_EDDSA_SHAKE256BYTES;
                p_eddsa_keygen->priv_scalar_len = PKCL_EDDSA_ED448_KEY_LEN;
            }

            /* generation private key value */
            external_ret = p_ctx->p_drbg_api(p_eddsa_keygen->priv_scalar_len, p_eddsa_keygen->priv_scalar_data);

            if (PKCL_EXTERNAL_API_OK == external_ret)
            {
                /* calc hash scalar */
                external_ret = p_ctx->p_hash_info->p_hash_init_api(p_ctx->p_hash_info->p_hash_ctx);
                if (PKCL_EXTERNAL_API_OK == external_ret)
                {
                    external_ret = p_ctx->p_hash_info->p_hash_update_api(p_ctx->p_hash_info->p_hash_ctx,
                                                                            p_eddsa_keygen->priv_scalar_data,
                                                                            p_eddsa_keygen->priv_scalar_len);
                    if (PKCL_EXTERNAL_API_OK == external_ret)
                    {
                        external_ret = p_ctx->p_hash_info->p_hash_final_api(p_ctx->p_hash_info->p_hash_ctx,
                                                                            p_eddsa_keygen->hash_scalar_data,
                                                                            &p_eddsa_keygen->hash_scalar_len);
                        if (PKCL_EXTERNAL_API_OK == external_ret)
                        {
                            /* Prune the buffer */
                            curve_len = p_ctx->p_curve_info->curve_len;
                            p_hscalar = p_eddsa_keygen->hash_scalar_data;
                            if (PKCL_EDDSA_ED25519_CURVE_LEN == curve_len)
                            {
                                /* No problem for casting to support usual arithmetic conversion */
                                p_hscalar[0U] = (uint8_t)((uint32_t)p_hscalar[0U] & PKCL_EDDSA_MASK_F8);
                                p_hscalar[curve_len - 1U] =
                                    (uint8_t)((uint32_t)p_hscalar[curve_len - 1U] & PKCL_EDDSA_MASK_3F);
                                p_hscalar[curve_len - 1U] =
                                    (uint8_t)((uint32_t)p_hscalar[curve_len - 1U] | PKCL_EDDSA_MASK_40);

                                p_eddsa_keygen->hash_scalar_len = curve_len; /* Use 0-31byte */
                            }
                            else /* Case Ed448 */
                            {
                                /* No problem for casting to support usual arithmetic conversion */
                                p_hscalar[0U] = (uint8_t)((uint32_t)p_hscalar[0U] & PKCL_EDDSA_MASK_FC);
                                p_hscalar[PKCL_EDDSA_ED448_KEY_LEN - 1U] =
                                    (uint8_t)((uint32_t)p_hscalar[PKCL_EDDSA_ED448_KEY_LEN - 1U] & 0x00U);
                                p_hscalar[PKCL_EDDSA_ED448_SECOND_TO_LAST_IDX] =
                                    (uint8_t)((uint32_t)p_hscalar[PKCL_EDDSA_ED448_SECOND_TO_LAST_IDX] |
                                    PKCL_EDDSA_MASK_80);

                                /* use 0-56byte + zero padding 3byte = 60byte */
                                r_pkcl_cmn_set_byte(&p_hscalar[PKCL_EDDSA_ED448_KEY_LEN],
                                                    0U, PKCL_EDDSA_ZERO_PADDING_3);
                                p_eddsa_keygen->hash_scalar_len = PKCL_EDDSA_ED448_RU4_KEY_LEN;
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
        r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_KEYGEN_INIT_DONE);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_EDDSA_KEYGEN != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_EDDSA_KEYGEN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EddsaKeyGenInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaKeyGenOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EddsaKeyGenOperate(st_pkc_eddsa_ctx_t *const p_ctx, const uint32_t processing_bits)
{
#if (PKCL_CFG_ENABLE_DRV_EDDSA_KEYGEN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_eddsa_keygen_t * p_eddsa_keygen;
    st_pkc_data_t hash_scalar_data;
    st_pkc_select_data_t hash_scalar_sel;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EDDSA_KEYGEN_INIT_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_eddsa_chk_keygen_operate_arg(p_ctx, processing_bits);

        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to EdDSA keygen buffer type */
            p_eddsa_keygen = (st_pkcl_eddsa_keygen_t *)p_ctx->p_buf;

            /* Prepare to pass hash sacalar to PKC-Driver */
            hash_scalar_data.p_data = p_eddsa_keygen->hash_scalar_data;
            hash_scalar_data.len = p_eddsa_keygen->hash_scalar_len;

            hash_scalar_sel.data_type = PKC_DATA_TYPE_BYTE;
            hash_scalar_sel.p_sel_data = &hash_scalar_data;

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_eddsa_keygen_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultBaseInit(&hash_scalar_sel, p_ctx, p_buf_drv);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EddsaScalarMultBaseOperate(p_ctx, p_buf_drv, processing_bits);

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
    else if (PKCL_STATE_EDDSA_KEYGEN_OPERATE_PROCESSING == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_eddsa_chk_keygen_operate_arg(p_ctx, processing_bits);

        if (PKCL_RET_OK == ret)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_eddsa_keygen_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultBaseOperate(p_ctx, p_buf_drv, processing_bits);

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
        r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_KEYGEN_OPERATE_DONE);
    }
    else if (PKCL_RET_PROCESSING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_KEYGEN_OPERATE_PROCESSING);
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
#else  /* PKCL_CFG_ENABLE_DRV_EDDSA_KEYGEN != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_EDDSA_KEYGEN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EddsaKeyGenOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaKeyGenFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EddsaKeyGenFinal(const st_pkc_eddsa_ctx_t *const p_ctx,
                                    void *const p_priv_key, void *const p_pub_key)
{
#if (PKCL_CFG_ENABLE_DRV_EDDSA_KEYGEN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_eddsa_keygen_t * p_eddsa_keygen;
    st_pkc_data_t priv_key_raw_data;
    st_pkc_data_t pub_key_raw_data;
    uint8_t pub_key_val[PKCL_EDDSA_ED448_RU4_KEY_LEN] = {0};
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EDDSA_KEYGEN_OPERATE_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_eddsa_chk_keygen_final_arg(p_ctx, p_priv_key, p_pub_key);

        if (PKCL_RET_OK == ret)
        {
            /* Prepare to pass SB to PKC-Driver */
            pub_key_raw_data.p_data = pub_key_val;
            pub_key_raw_data.len = sizeof(pub_key_val);

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_eddsa_keygen_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultBaseFinal(p_ctx, p_buf_drv, PKC_BOOL_TRUE, &pub_key_raw_data);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Cast p_buf to EdDSA keygen buffer type */
                p_eddsa_keygen = (st_pkcl_eddsa_keygen_t *)p_ctx->p_buf;

                /* Prepare to pass public key to PKC-Driver */
                priv_key_raw_data.p_data = p_eddsa_keygen->priv_scalar_data;
                priv_key_raw_data.len = (PKCL_EDDSA_ED25519_CURVE_LEN == p_ctx->p_curve_info->curve_len) ?
                                        PKCL_EDDSA_ED25519_KEY_LEN : PKCL_EDDSA_ED448_RU4_KEY_LEN;

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EddsaKeyWrap(p_ctx, p_buf_drv, &priv_key_raw_data, &pub_key_raw_data,
                                                p_priv_key, p_pub_key);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                /* Clear the private key value stored in p_buf */
                r_pkcl_cmn_set_byte(p_eddsa_keygen->priv_scalar_data, 0U, sizeof(p_eddsa_keygen->priv_scalar_data));
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
#else  /* PKCL_CFG_ENABLE_DRV_EDDSA_KEYGEN != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_EDDSA_KEYGEN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EddsaKeyGenFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaScalarMultBaseInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEddsaScalarMultBaseInit(const st_pkc_select_data_t *const p_scalar_or_key,
                                            st_pkc_eddsa_ctx_t *const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* check if the argument is null */
        if ((NULL != p_scalar_or_key) && (NULL != p_ctx))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
                if (PKCL_RET_OK == ret)
                {
                    /* check p_scalar_or_key */
                    ret = r_pkcl_eddsa_chk_select_data(p_scalar_or_key, p_ctx->p_curve_info->curve_len);
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

        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultBaseInit(p_scalar_or_key, p_ctx, p_ctx->p_buf);

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
        r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_SMULTBASE_INIT_DONE);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_LLEddsaScalarMultBaseInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaScalarMultBaseOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEddsaScalarMultBaseOperate(st_pkc_eddsa_ctx_t *const p_ctx, const uint32_t processing_bits)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if ((PKCL_STATE_EDDSA_SMULTBASE_INIT_DONE == pkcl_state) ||
        (PKCL_STATE_EDDSA_SMULTBASE_OPERATE_PROCESSING == pkcl_state))
    {
        /* Check argument */
        ret = r_pkcl_eddsa_chk_scalar_mult_operate_arg(p_ctx, processing_bits);
        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultBaseOperate(p_ctx, p_ctx->p_buf, processing_bits);

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
        r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_SMULTBASE_OPERATE_DONE);
    }
    else if (PKCL_RET_PROCESSING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_SMULTBASE_OPERATE_PROCESSING);
    }
    else if ((PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret) || (PKCL_RET_EDDSA_DECOMPERR == ret))
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
* End of function R_PKCL_LLEddsaScalarMultBaseOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaScalarMultBaseFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEddsaScalarMultBaseFinal(const st_pkc_eddsa_ctx_t *const p_ctx, const e_pkc_bool_t comp_flag,
                                            st_pkc_data_t *const p_point)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EDDSA_SMULTBASE_OPERATE_DONE == pkcl_state)
    {
        /* check if the argument is null */
        if ((NULL != p_ctx) && (NULL != p_point))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                if ((PKC_BOOL_TRUE == comp_flag) || (PKC_BOOL_FALSE == comp_flag))
                {
                    /* check curve info */
                    ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
                    if (PKCL_RET_OK == ret)
                    {
                        /* check p_point */
                        ret = r_pkcl_eddsa_chk_point(p_point, p_ctx->p_curve_info->curve_len,
                                                        comp_flag, PKCL_EDDSA_DATA_CHK_OUT_ARG);
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

        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultBaseFinal(p_ctx, p_ctx->p_buf, comp_flag, p_point);

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
* End of function R_PKCL_LLEddsaScalarMultBaseFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaScalarMultInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEddsaScalarMultInit(const st_pkc_select_data_t *const p_scalar_or_key,
                                        const st_pkc_select_data_t *const p_point_or_key,
                                        st_pkc_eddsa_ctx_t *const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* check if the argument is null */
        if ((NULL != p_scalar_or_key) && (NULL != p_point_or_key) && (NULL != p_ctx))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
                if (PKCL_RET_OK == ret)
                {
                    /* check p_scalar_or_key */
                    ret = r_pkcl_eddsa_chk_select_data(p_scalar_or_key, p_ctx->p_curve_info->curve_len);
                    if (PKCL_RET_OK == ret)
                    {
                        /* check p_point_or_key */
                        ret = r_pkcl_eddsa_chk_select_data(p_point_or_key, p_ctx->p_curve_info->curve_len);
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
                ret = PKCL_RET_PARAMERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }

        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultInit(p_scalar_or_key, p_point_or_key, p_ctx, p_ctx->p_buf);

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
        r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_SMULT_INIT_DONE);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_LLEddsaScalarMultInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaScalarMultOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEddsaScalarMultOperate(st_pkc_eddsa_ctx_t *const p_ctx, const uint32_t processing_bits)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if ((PKCL_STATE_EDDSA_SMULT_INIT_DONE == pkcl_state) ||
        (PKCL_STATE_EDDSA_SMULT_OPERATE_PROCESSING == pkcl_state))
    {
        /* Check argument */
        ret = r_pkcl_eddsa_chk_scalar_mult_operate_arg(p_ctx, processing_bits);
        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultOperate(p_ctx, p_ctx->p_buf, processing_bits);

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
        r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_SMULT_OPERATE_DONE);
    }
    else if (PKCL_RET_PROCESSING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_SMULT_OPERATE_PROCESSING);
    }
    else if ((PKCL_RET_PKCCTIMEOUTERR == ret) || (PKCL_RET_PKCCERR == ret) || (PKCL_RET_EDDSA_DECOMPERR == ret))
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
* End of function R_PKCL_LLEddsaScalarMultOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaScalarMultFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEddsaScalarMultFinal(const st_pkc_eddsa_ctx_t *const p_ctx, const e_pkc_bool_t comp_flag,
                                        st_pkc_data_t *const p_point)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EDDSA_SMULT_OPERATE_DONE == pkcl_state)
    {
        /* check if the argument is null */
        if ((NULL != p_ctx) && (NULL != p_point))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                if ((PKC_BOOL_TRUE == comp_flag) || (PKC_BOOL_FALSE == comp_flag))
                {
                    /* check curve info */
                    ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
                    if (PKCL_RET_OK == ret)
                    {
                        /* check p_point */
                        ret = r_pkcl_eddsa_chk_point(p_point, p_ctx->p_curve_info->curve_len,
                                                        comp_flag, PKCL_EDDSA_DATA_CHK_OUT_ARG);
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

        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultFinal(p_ctx, p_ctx->p_buf, comp_flag, p_point);

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
* End of function R_PKCL_LLEddsaScalarMultFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaDeriveS
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEddsaDeriveS(const st_pkc_eddsa_ctx_t *const p_ctx, const st_pkc_data_t *const p_scalar_s,
                                    const st_pkc_data_t *const p_scalar_r, const st_pkc_data_t *const p_digest_r_a_m,
                                    st_pkc_data_t *const p_sign_s)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* check if the argument is null */
        if ((NULL != p_ctx) && (NULL != p_scalar_s) && (NULL != p_scalar_r) &&
            (NULL != p_digest_r_a_m) && (NULL != p_sign_s))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
                if (PKCL_RET_OK == ret)
                {
                    /* check p_scalar_s */
                    ret = r_pkcl_eddsa_chk_scalar(p_scalar_s, p_ctx->p_curve_info->curve_len,
                                                    PKCL_EDDSA_DATA_CHK_IN_ARG);
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

            if (PKCL_RET_OK == ret)
            {
                /* check p_scalar_r */
                ret = r_pkcl_eddsa_chk_digest(p_scalar_r, p_ctx->p_curve_info->curve_len,
                                                PKCL_EDDSA_DATA_CHK_IN_ARG);
                if (PKCL_RET_OK == ret)
                {
                    /* check p_digest_r_a_m */
                    ret = r_pkcl_eddsa_chk_digest(p_digest_r_a_m, p_ctx->p_curve_info->curve_len,
                                                    PKCL_EDDSA_DATA_CHK_IN_ARG);
                    if (PKCL_RET_OK == ret)
                    {
                        /* check p_sign_s */
                        ret = r_pkcl_eddsa_chk_scalar(p_sign_s, p_ctx->p_curve_info->curve_len,
                                                        PKCL_EDDSA_DATA_CHK_OUT_ARG);
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
            ret = PKCL_RET_PARAMERR;
        }

        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaDeriveS(p_ctx, p_ctx->p_buf, p_scalar_s, p_scalar_r, p_digest_r_a_m, p_sign_s);

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

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_LLEddsaDeriveS()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaCompareSign
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEddsaCompareSign(const st_pkc_eddsa_ctx_t *const p_ctx,
                                        const st_pkc_data_t *const p_point_ha,
                                        const st_pkc_data_t *const p_point_sb,
                                        const st_pkc_data_t *const p_sign_r)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* check if the argument is null */
        if ((NULL != p_ctx) && (NULL != p_point_ha) && (NULL != p_point_sb) && (NULL != p_sign_r))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
            }
            else
            {
                ret = PKCL_RET_PARAMERR;
            }

            if (PKCL_RET_OK == ret)
            {
                /* check p_point_ha */
                ret = r_pkcl_eddsa_chk_point(p_point_ha, p_ctx->p_curve_info->curve_len,
                                                PKC_BOOL_FALSE, PKCL_EDDSA_DATA_CHK_IN_ARG);
                if (PKCL_RET_OK == ret)
                {
                    /* check p_point_sb */
                    ret = r_pkcl_eddsa_chk_point(p_point_sb, p_ctx->p_curve_info->curve_len,
                                                    PKC_BOOL_FALSE, PKCL_EDDSA_DATA_CHK_IN_ARG);
                    if (PKCL_RET_OK == ret)
                    {
                        /* check p_sign_r */
                        ret = r_pkcl_eddsa_chk_scalar(p_sign_r, p_ctx->p_curve_info->curve_len,
                                                        PKCL_EDDSA_DATA_CHK_IN_ARG);
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
            ret = PKCL_RET_PARAMERR;
        }

        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaCompareSign(p_ctx, p_ctx->p_buf, p_point_ha, p_point_sb, p_sign_r);

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

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_LLEddsaCompareSign()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_curve
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check the arguments of the EdDSA curve.
 * \param       [in] p_curve          Curve information structure
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
 *      - Make sure the length of the curve is the same as the length of the corresponding curve.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If (NULL != p_curve)
 *    -# If ((#PKCL_EDDSA_ED25519_CURVE_LEN == p_curve->curve_len) or
 *           (#PKCL_EDDSA_ED448_CURVE_LEN == p_curve->curve_len))
 *      -# Set ret to #PKCL_RET_OK.
 *    -# Else
 *      -# Set ret to #PKCL_RET_LENGTHERR.
 *    -# If (#PKCL_RET_OK == ret)
 *      -# If (NULL != p_curve->curve_params.p_data)
 *        -# If ((p_curve->curve_len * #PKCL_EDDSA_CURVE_PARAM_NUM) == p_curve->curve_params.len)
 *          -# Do nothing.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_curve(const st_pkc_ec_curve_t *const p_curve)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if (NULL != p_curve)
    {
        /* check curve_len */
        if ((PKCL_EDDSA_ED25519_CURVE_LEN == p_curve->curve_len) ||
            (PKCL_EDDSA_ED448_CURVE_LEN == p_curve->curve_len))
        {
            ret = PKCL_RET_OK;
        }
        else
        {
            ret = PKCL_RET_LENGTHERR;
        }

        if (PKCL_RET_OK == ret)
        {
            if (NULL != p_curve->curve_params.p_data)
            {
                if ((p_curve->curve_len * PKCL_EDDSA_CURVE_PARAM_NUM) == p_curve->curve_params.len)
                {
                    /* Do nothing */
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
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_eddsa_chk_curve()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_msg
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check arguments of type st_pkc_data_t
 * \param       [in] p_pkc_data             Data of type st_pkc_data_t
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
 *      - Check maximum message length.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR
 *  -# If (NULL != p_pkc_data)
 *    -# If (NULL != p_pkc_data->p_data)
 *      -# If (#PKCL_EDDSA_MSG_LEN_MAX >= p_pkc_data->len)
 *        -# Set ret to #PKCL_RET_OK.
 *      -# Else
 *        -# Set ret to #PKCL_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_msg(const st_pkc_data_t *const p_pkc_data)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if (NULL != p_pkc_data)
    {
        if (NULL != p_pkc_data->p_data)
        {
            if (PKCL_EDDSA_MSG_LEN_MAX >= p_pkc_data->len)
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

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_eddsa_chk_msg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_point
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check arguments of type st_pkc_data_t
 * \param       [in] p_pkc_data             Data of type st_pkc_data_t
 * \param       [in] curve_len              EdDSA curve length
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
 *      - Check data based on curve length.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR
 *  -# If #PKCL_EDDSA_ED25519_CURVE_LEN == curve_len
 *    -# Set expected_data_len to #PKCL_EDDSA_ED25519_KEY_LEN.
 *  -# Else
 *    -# Set expected_data_len to #PKCL_EDDSA_ED448_RU4_KEY_LEN.
 *  -# If (NULL != p_pkc_data)
 *    -# If (NULL != p_pkc_data->p_data)
 *      -# If (#PKCL_EDDSA_DATA_CHK_IN_ARG == arg_type)
 *        -# If (p_pkc_data->len == expected_data_len)
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else if (#PKCL_EDDSA_DATA_CHK_OUT_ARG == arg_type)
 *        -# If (p_pkc_data->len >= expected_data_len)
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_INTERNALERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_point(const st_pkc_data_t *const p_pkc_data, const uint32_t curve_len,
                                            const e_pkc_bool_t comp_flag, const uint32_t arg_type)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    uint32_t expected_data_len;

    if (PKC_BOOL_TRUE == comp_flag)
    {
        if (PKCL_EDDSA_ED25519_CURVE_LEN == curve_len)
        {
            expected_data_len = PKCL_EDDSA_ED25519_KEY_LEN;
        }
        else
        {
            expected_data_len = PKCL_EDDSA_ED448_RU4_KEY_LEN;
        }
    }
    else
    {
        expected_data_len = (curve_len * PKCL_EDDSA_TRIPLE_LEN);
    }

    /* check if the argument is null */
    if (NULL != p_pkc_data)
    {
        if (NULL != p_pkc_data->p_data)
        {
            /* Input argument or output argument */
            if (PKCL_EDDSA_DATA_CHK_IN_ARG == arg_type)
            {
                if (p_pkc_data->len == expected_data_len)
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_LENGTHERR;
                }
            }
            else if (PKCL_EDDSA_DATA_CHK_OUT_ARG == arg_type)
            {
                if (p_pkc_data->len >= expected_data_len)
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
* End of function r_pkcl_eddsa_chk_point()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_scalar
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check arguments of type st_pkc_data_t
 * \param       [in] p_pkc_data             Data of type st_pkc_data_t
 * \param       [in] curve_len              EdDSA curve length
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
 *      - Check data based on curve length.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR
 *  -# If #PKCL_EDDSA_ED25519_CURVE_LEN == curve_len
 *    -# Set expected_data_len to #PKCL_EDDSA_ED25519_KEY_LEN.
 *  -# Else
 *    -# Set expected_data_len to #PKCL_EDDSA_ED448_RU4_KEY_LEN.
 *  -# If (NULL != p_pkc_data)
 *    -# If (NULL != p_pkc_data->p_data)
 *      -# If (#PKCL_EDDSA_DATA_CHK_IN_ARG == arg_type)
 *        -# If (p_pkc_data->len == expected_data_len)
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else if (#PKCL_EDDSA_DATA_CHK_OUT_ARG == arg_type)
 *        -# If (p_pkc_data->len >= expected_data_len)
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_INTERNALERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_scalar(const st_pkc_data_t *const p_pkc_data, const uint32_t curve_len,
                                            const uint32_t arg_type)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    uint32_t expected_data_len;

    if (PKCL_EDDSA_ED25519_CURVE_LEN == curve_len)
    {
        expected_data_len   = PKCL_EDDSA_ED25519_KEY_LEN;
    }
    else
    {
        expected_data_len   = PKCL_EDDSA_ED448_RU4_KEY_LEN;
    }

    /* check if the argument is null */
    if (NULL != p_pkc_data)
    {
        if (NULL != p_pkc_data->p_data)
        {
            /* Input argument or output argument */
            if (PKCL_EDDSA_DATA_CHK_IN_ARG == arg_type)
            {
                if (p_pkc_data->len == expected_data_len)
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_LENGTHERR;
                }
            }
            else if (PKCL_EDDSA_DATA_CHK_OUT_ARG == arg_type)
            {
                if (p_pkc_data->len >= expected_data_len)
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
* End of function r_pkcl_eddsa_chk_scalar()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_sign
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check arguments of type st_pkc_data_t
 * \param       [in] p_pkc_data             Data of type st_pkc_data_t
 * \param       [in] curve_len              EdDSA curve length
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
 *      - Check signature data.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR
 *  -# If #PKCL_EDDSA_ED25519_CURVE_LEN == curve_len
 *    -# Set expected_data_len to #PKCL_EDDSA_ED25519_KEY_LEN.
 *  -# Else
 *    -# Set expected_data_len to #PKCL_EDDSA_ED448_RU4_KEY_LEN.
 *  -# If (NULL != p_pkc_data)
 *    -# If (NULL != p_pkc_data->p_data)
 *      -# If (#PKCL_EDDSA_DATA_CHK_IN_ARG == arg_type)
 *        -# If (p_pkc_data->len == (expected_data_len * #PKCL_EDDSA_DOUBLE_LEN))
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else if (#PKCL_EDDSA_DATA_CHK_OUT_ARG == arg_type)
 *        -# If (p_pkc_data->len >= (expected_data_len * #PKCL_EDDSA_DOUBLE_LEN))
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_INTERNALERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_sign(const st_pkc_data_t *const p_pkc_data, const uint32_t curve_len,
                                        const uint32_t arg_type)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    uint32_t expected_data_len;

    if (PKCL_EDDSA_ED25519_CURVE_LEN == curve_len)
    {
        expected_data_len   = PKCL_EDDSA_ED25519_KEY_LEN;
    }
    else
    {
        expected_data_len   = PKCL_EDDSA_ED448_RU4_KEY_LEN;
    }

    /* check if the argument is null */
    if (NULL != p_pkc_data)
    {
        if (NULL != p_pkc_data->p_data)
        {
            /* Input argument or output argument */
            if (PKCL_EDDSA_DATA_CHK_IN_ARG == arg_type)
            {
                if ((expected_data_len * PKCL_EDDSA_DOUBLE_LEN) == p_pkc_data->len)
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_LENGTHERR;
                }
            }
            else if (PKCL_EDDSA_DATA_CHK_OUT_ARG == arg_type)
            {
                if ((expected_data_len * PKCL_EDDSA_DOUBLE_LEN) <= p_pkc_data->len)
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
* End of function r_pkcl_eddsa_chk_sign()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_digest
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check arguments of type st_pkc_data_t
 * \param       [in] p_pkc_data             Data of type st_pkc_data_t
 * \param       [in] curve_len              EdDSA curve length
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
 *      - Check the digest data.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR
 *  -# If #PKCL_EDDSA_ED25519_CURVE_LEN == curve_len
 *    -# Set expected_digest_len to #PKCL_EDDSA_ED25519_DIGEST_LEN.
 *  -# Else
 *    -# Set expected_digest_len to #PKCL_EDDSA_ED448_DIGEST_LEN.
 *  -# If (NULL != p_pkc_data)
 *    -# If (NULL != p_pkc_data->p_data)
 *      -# If (#PKCL_EDDSA_DATA_CHK_IN_ARG == arg_type)
 *        -# If (p_pkc_data->len == expected_digest_len)
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else if (#PKCL_EDDSA_DATA_CHK_OUT_ARG == arg_type)
 *        -# If (p_pkc_data->len >= expected_digest_len)
 *          -# Do nothing.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_INTERNALERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_digest(const st_pkc_data_t *const p_pkc_data, const uint32_t curve_len,
                                            const uint32_t arg_type)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    uint32_t expected_digest_len;

    if (PKCL_EDDSA_ED25519_CURVE_LEN == curve_len)
    {
        expected_digest_len = PKCL_EDDSA_ED25519_DIGEST_LEN;
    }
    else
    {
        expected_digest_len = PKCL_EDDSA_ED448_DIGEST_LEN;
    }

    /* check if the argument is null */
    if (NULL != p_pkc_data)
    {
        if (NULL != p_pkc_data->p_data)
        {
            /* Input argument or output argument */
            if (PKCL_EDDSA_DATA_CHK_IN_ARG == arg_type)
            {
                if (p_pkc_data->len == expected_digest_len)
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_LENGTHERR;
                }
            }
            else if (PKCL_EDDSA_DATA_CHK_OUT_ARG == arg_type)
            {
                if (p_pkc_data->len >= expected_digest_len)
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
* End of function r_pkcl_eddsa_chk_digest()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_select_data
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check the argument of type st_pkc_select_data_t.
 * \param       [in]   p_select_data       Data of type st_pkc_select_data_t
 * \param       [in]   curve_len           EdDSA curve length
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
 *      - Make sure data_type is #PKC_DATA_TYPE_BYTE or #PKC_DATA_TYPE_KEY_BINARY.
 *      - If data_type is #PKC_DATA_TYPE_BYTE, check the following.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR.
 *  -# If #PKCL_EDDSA_ED25519_CURVE_LEN == curve_len
 *    -# Set expected_scalar_len to #PKCL_EDDSA_ED25519_KEY_LEN.
 *    -# Set expected_digest_len to #PKCL_EDDSA_ED25519_DIGEST_LEN.
 *  -# Else
 *    -# Set expected_scalar_len to #PKCL_EDDSA_ED448_RU4_KEY_LEN.
 *    -# Set expected_digest_len to #PKCL_EDDSA_ED448_DIGEST_LEN.
 *  -# If (((#PKC_DATA_TYPE_BYTE == p_select_data->data_type) or<br>
 *          (#PKC_DATA_TYPE_KEY_BINARY == p_select_data->data_type)) and<br>
 *          (NULL != p_select_data->p_sel_data))
 *    -# If (NULL != p_select_data->p_sel_data->p_data)
 *      -# If (#PKC_DATA_TYPE_BYTE == p_select_data->data_type)
 *        -# If PKCL_EDDSA_ED25519_CURVE_LEN == curve_len
 *          -# If (p_select_data->p_sel_data->len == curve_len) or
 *                (p_select_data->p_sel_data->len == #PKCL_EDDSA_ED25519_DIGEST_LEN)
 *            -# Set ret to #PKCL_RET_OK.
 *          -# Else
 *            -# Set ret to #PKCL_RET_LENGTHERR.
 *        -# Else
 *          -# If (p_select_data->p_sel_data->len == #PKCL_EDDSA_ED448_RU4_KEY_LEN) or
 *                (p_select_data->p_sel_data->len == #PKCL_EDDSA_ED448_DIGEST_LEN)
 *            -# Set ret to #PKCL_RET_OK.
 *          -# Else
 *            -# Set ret to #PKCL_RET_LENGTHERR.
 *        -# If (p_select_data->p_sel_data->len == expected_scalar_len) or
 *               p_select_data->p_sel_data->len == expected_digest_len
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_OK.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_select_data(const st_pkc_select_data_t *const p_select_data,
                                                const uint32_t curve_len)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    uint32_t expected_scalar_len;
    uint32_t expected_digest_len;

    if (PKCL_EDDSA_ED25519_CURVE_LEN == curve_len)
    {
        expected_scalar_len = PKCL_EDDSA_ED25519_KEY_LEN;
        expected_digest_len = PKCL_EDDSA_ED25519_DIGEST_LEN;
    }
    else
    {
        expected_scalar_len = PKCL_EDDSA_ED448_RU4_KEY_LEN;
        expected_digest_len = PKCL_EDDSA_ED448_DIGEST_LEN;
    }

    /* check if the p_select_data member */
    if (((PKC_DATA_TYPE_BYTE == p_select_data->data_type) || (PKC_DATA_TYPE_KEY_BINARY == p_select_data->data_type)) &&
        (NULL != p_select_data->p_sel_data))
    {
        /* NULL check p_data */
        if (NULL != p_select_data->p_sel_data->p_data)
        {
            if (PKC_DATA_TYPE_BYTE == p_select_data->data_type)
            {
                /* check length */
                if ((p_select_data->p_sel_data->len == expected_scalar_len) ||
                    (p_select_data->p_sel_data->len == expected_digest_len))
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
                ret = PKCL_RET_OK;
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
* End of function r_pkcl_eddsa_chk_select_data()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_variant
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check EdDSA variant settings.
 * \param       [in]   eddsa_variant       EdDSA variant
 * \param       [in]   p_ed_ctx            Up to 255 bytes of context set by signers and verifiers in EdDSA
 * \param       [in]   curve_len           EdDSA curve length
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
 *      - Check the Ed context for each EdDSA variant setting.
 *      - For Ed25519, no context check.
 *      - For Ed25519ctx, check the length of the context length and make sure the context data is not null.
 *      - For Ed25519ph, Ed448, Ed448ph, check the length of the context.
 *        If the length is non-zero, make sure the context data is not null.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR.
 *  -# If ((#PKCL_EDDSA_ED25519_CURVE_LEN == curve_len) and (#PKC_EDDSA_VARIANT_PURE == eddsa_variant))
 *    -# Do nothing
 *  -# Else if ((#PKCL_EDDSA_ED25519_CURVE_LEN == curve_len) and (#PKC_EDDSA_VARIANT_CTX == eddsa_variant))
 *    -# If NULL != p_ed_ctx
 *      -# If ((#PKCL_EDDSA_ED_CTX_LEN_MIN <= p_ed_ctx->len) and (#PKCL_EDDSA_ED_CTX_LEN_MAX >= p_ed_ctx->len))
 *        -# If (NULL != p_ed_ctx->p_data)
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else if (((#PKCL_EDDSA_ED25519_CURVE_LEN == curve_len) and (#PKC_EDDSA_VARIANT_HASH == eddsa_variant)) or<br>
 *              ((#PKCL_EDDSA_ED448_CURVE_LEN == curve_len) and (#PKC_EDDSA_VARIANT_PURE == eddsa_variant)) or<br>
 *              ((#PKCL_EDDSA_ED448_CURVE_LEN == curve_len) and (#PKC_EDDSA_VARIANT_HASH == eddsa_variant)))
 *    -# If NULL != p_ed_ctx
 *      -# If (#PKCL_EDDSA_ED_CTX_LEN_MAX >= p_ed_ctx->len)
 *        -# If (0UL == p_ed_ctx->len)
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# If (NULL != p_ed_ctx->p_data)
 *            -# Set ret to #PKCL_RET_OK.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_variant(const e_pkc_eddsa_variant_t eddsa_variant,
                                            const st_pkc_data_t *const p_ed_ctx, const uint32_t curve_len)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* Ed25519 */
    if ((PKCL_EDDSA_ED25519_CURVE_LEN == curve_len) && (PKC_EDDSA_VARIANT_PURE == eddsa_variant))
    {
        ret = PKCL_RET_OK;
    }
    /* Ed25519ctx */
    else if ((PKCL_EDDSA_ED25519_CURVE_LEN == curve_len) && (PKC_EDDSA_VARIANT_CTX == eddsa_variant))
    {
        if (NULL != p_ed_ctx)
        {
            if ((PKCL_EDDSA_ED_CTX_LEN_MIN <= p_ed_ctx->len) && (PKCL_EDDSA_ED_CTX_LEN_MAX >= p_ed_ctx->len))
            {
                if (NULL != p_ed_ctx->p_data)
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
                ret = PKCL_RET_LENGTHERR;
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }
    }
    /* Ed25519ph, Ed448, Ed448ph */
    else if (((PKCL_EDDSA_ED25519_CURVE_LEN == curve_len) && (PKC_EDDSA_VARIANT_HASH == eddsa_variant)) ||
            ((PKCL_EDDSA_ED448_CURVE_LEN == curve_len) && (PKC_EDDSA_VARIANT_PURE == eddsa_variant)) ||
            ((PKCL_EDDSA_ED448_CURVE_LEN == curve_len) && (PKC_EDDSA_VARIANT_HASH == eddsa_variant)))
    {
        if (NULL != p_ed_ctx)
        {
            if (PKCL_EDDSA_ED_CTX_LEN_MAX >= p_ed_ctx->len)
            {
                if (0U == p_ed_ctx->len)
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    if (NULL != p_ed_ctx->p_data)
                    {
                        ret = PKCL_RET_OK;
                    }
                    else
                    {
                        ret = PKCL_RET_PARAMERR;
                    }
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
* End of function r_pkcl_eddsa_chk_variant()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_sign_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EddsaSignInit.
 * \param       [in]     p_msg         A structure consisting of the byte length len of the message data and
 *                                      the data pointer p_data.
 * \param       [in]     p_priv_key    EdDSA private key A structure consisting of the binary byte length len and
 *                                      a pointer to the binary.
 * \param       [in]     p_pub_key     EdDSA public key A structure consisting of the binary byte length len and
 *                                      a pointer to the binary.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by EdDSA.
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
 *      - Call #r_pkcl_eddsa_chk_msg to check the argument p_msg
 *      - Call #r_pkcl_eddsa_chk_curve to check the curve information of p_ctx.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_msg) and (NULL != p_priv_key) and (NULL != p_pub_key) and (NULL != p_ctx))
 *    -# Check the argument p_msg with #r_pkcl_eddsa_chk_msg, and set ret to the return value.
 *      -# If (#PKCL_RET_OK == ret)
 *        -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_hash_info) and (NULL != p_ctx->p_curve_info))
 *          -# If ((NULL != p_ctx->p_hash_info->p_hash_init_api) and
 *                 (NULL != p_ctx->p_hash_info->p_hash_update_api) and
 *                 (NULL != p_ctx->p_hash_info->p_hash_final_api))
 *            -# Check the curve information p_ctx->p_curve_info with #r_pkcl_eddsa_chk_curve.
 *            -# If (#PKCL_RET_OK == ret)
 *              -# Check the eddsa_variant information with #r_pkcl_eddsa_chk_variant.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_sign_init_arg(const st_pkc_data_t *const p_msg,
                                                    const void *const p_priv_key,
                                                    const void *const p_pub_key,
                                                    const st_pkc_eddsa_ctx_t *const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_msg) && (NULL != p_priv_key) && (NULL != p_pub_key) && (NULL != p_ctx))
    {
        /* check msg */
        ret = r_pkcl_eddsa_chk_msg(p_msg);
        if (PKCL_RET_OK == ret)
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_hash_info) && (NULL != p_ctx->p_curve_info))
            {
                /* check hash info */
                if ((NULL != p_ctx->p_hash_info->p_hash_init_api) &&
                    (NULL != p_ctx->p_hash_info->p_hash_update_api) &&
                    (NULL != p_ctx->p_hash_info->p_hash_final_api))
                {
                    /* check curve info */
                    ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
                    if (PKCL_RET_OK == ret)
                    {
                        ret = r_pkcl_eddsa_chk_variant(p_ctx->eddsa_variant, p_ctx->p_ed_ctx,
                                                        p_ctx->p_curve_info->curve_len);
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

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_eddsa_chk_sign_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_sign_verify_operate_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EddsaSignOperate or #R_PKCL_EddsaVerifyOperate.
 * \param       [in] p_ctx             A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in] processing_bits   Number of bits processed at one time when the operation is executed
 *                                      in multiple times in PKCC.
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
 *      - Call #r_pkcl_eddsa_chk_curve to check the curve information of p_ctx.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If (NULL != p_ctx)
 *    -# If ((0U < processing_bits) and (#PKCL_PROCESSING_BITS_MAX >= processing_bits))
 *      -# If ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_hash_info) && (NULL != p_ctx->p_curve_info))
 *        -# If ((NULL != p_ctx->p_hash_info->p_hash_init_api) and
 *               (NULL != p_ctx->p_hash_info->p_hash_update_api) and
 *               (NULL != p_ctx->p_hash_info->p_hash_final_api))
 *          -# Check the curve information p_ctx->p_curve_info with #r_pkcl_eddsa_chk_curve,
 *             and set ret to the return value.
 *          -# If (#PKCL_RET_OK == ret)
 *            -# Check the eddsa_variant information with #r_pkcl_eddsa_chk_variant.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_LENGTHERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_sign_verify_operate_arg(const st_pkc_eddsa_ctx_t *const p_ctx,
                                                            const uint32_t processing_bits)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if (NULL != p_ctx)
    {
        /* check processing_bits */
        if ((0U < processing_bits) && (PKCL_PROCESSING_BITS_MAX >= processing_bits))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_hash_info) && (NULL != p_ctx->p_curve_info))
            {
                /* check hash info */
                if ((NULL != p_ctx->p_hash_info->p_hash_init_api) &&
                    (NULL != p_ctx->p_hash_info->p_hash_update_api) &&
                    (NULL != p_ctx->p_hash_info->p_hash_final_api))
                {
                    /* check curve info */
                    ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
                    if (PKCL_RET_OK == ret)
                    {
                        ret = r_pkcl_eddsa_chk_variant(p_ctx->eddsa_variant, p_ctx->p_ed_ctx,
                                                        p_ctx->p_curve_info->curve_len);
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
            ret = PKCL_RET_LENGTHERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_eddsa_chk_sign_verify_operate_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_sign_final_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EddsaSignFinal.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in]     p_sign        A structure consisting of the byte length len of the data concatenated
 *                                      with the signature values R and S and the data pointer p_data.
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
 *      - Call #r_pkcl_eddsa_chk_curve to check the curve information of p_ctx.
 *      - Call #r_pkcl_eddsa_chk_sign to check the argument p_sign.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_sign))
 *    -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_hash_info) and (NULL != p_ctx->p_curve_info))
 *      -# If ((NULL != p_ctx->p_hash_info->p_hash_init_api) and
 *             (NULL != p_ctx->p_hash_info->p_hash_update_api) and
 *             (NULL != p_ctx->p_hash_info->p_hash_final_api))
 *        -# Check the curve information p_ctx->p_curve_info with #r_pkcl_eddsa_chk_curve,
 *           and set ret to the return value.
 *        -# If (#PKCL_RET_OK == ret)
 *          -# Check the eddsa_variant information with #r_pkcl_eddsa_chk_variant.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If (#PKCL_RET_OK == ret)
 *      -# Check the argument p_sign with #r_pkcl_eddsa_chk_sign, and set ret to the return value.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_sign_final_arg(const st_pkc_eddsa_ctx_t *const p_ctx,
                                                    const st_pkc_data_t *const p_sign)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_sign))
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_hash_info) && (NULL != p_ctx->p_curve_info))
        {
            /* check hash info */
            if ((NULL != p_ctx->p_hash_info->p_hash_init_api) &&
                (NULL != p_ctx->p_hash_info->p_hash_update_api) &&
                (NULL != p_ctx->p_hash_info->p_hash_final_api))
            {
                /* check curve info */
                ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
                if (PKCL_RET_OK == ret)
                {
                    ret = r_pkcl_eddsa_chk_variant(p_ctx->eddsa_variant, p_ctx->p_ed_ctx,
                                                    p_ctx->p_curve_info->curve_len);
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

        if (PKCL_RET_OK == ret)
        {
            /* check p_sign */
            ret = r_pkcl_eddsa_chk_sign(p_sign, p_ctx->p_curve_info->curve_len, PKCL_EDDSA_DATA_CHK_OUT_ARG);
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

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_eddsa_chk_sign_final_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_verify_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EddsaVerifyInit.
 * \param       [in]     p_msg         A structure consisting of the byte length len of the message data and
 *                                      the data pointer p_data.
 * \param       [in]     p_sign        A structure consisting of the byte length len of the data concatenated
 *                                      with the signature values R and S and the data pointer p_data.
 * \param       [in]     p_pub_key     EdDSA public key A structure consisting of the binary byte length len and
 *                                      a pointer to the binary.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by EdDSA.
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
 *      - Call #r_pkcl_eddsa_chk_msg to check the argument p_msg
 *      - Call #r_pkcl_eddsa_chk_sign to check the argument p_sign.
 *      - Call #r_pkcl_eddsa_chk_curve to check the curve information of p_ctx.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_msg) and (NULL != p_sign) and (NULL != p_pub_key) and (NULL != p_ctx))
 *    -# Check the argument p_msg with #r_pkcl_eddsa_chk_msg, and set ret to the return value.
 *    -# If (#PKCL_RET_OK == ret)
 *      -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_hash_info) and (NULL != p_ctx->p_curve_info))
 *        -# If ((NULL != p_ctx->p_hash_info->p_hash_init_api) and
 *               (NULL != p_ctx->p_hash_info->p_hash_update_api) and
 *               (NULL != p_ctx->p_hash_info->p_hash_final_api))
 *          -# Check the curve information p_ctx->p_curve_info with #r_pkcl_eddsa_chk_curve,
 *             and set ret to the return value.
 *          -# If (#PKCL_RET_OK == ret)
 *            -# Check the eddsa_variant information with #r_pkcl_eddsa_chk_variant.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *      -# If (#PKCL_RET_OK == ret)
 *        -# Check the argument p_sign with #r_pkcl_eddsa_chk_sign, and set ret to the return value.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_verify_init_arg(const st_pkc_data_t *const p_msg,
                                                    const st_pkc_data_t *const p_sign,
                                                    const void *const p_pub_key,
                                                    const st_pkc_eddsa_ctx_t *const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_msg) && (NULL != p_sign) && (NULL != p_pub_key) && (NULL != p_ctx))
    {
        /* check msg */
        ret = r_pkcl_eddsa_chk_msg(p_msg);
        if (PKCL_RET_OK == ret)
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_hash_info) && (NULL != p_ctx->p_curve_info))
            {
                /* check hash info */
                if ((NULL != p_ctx->p_hash_info->p_hash_init_api) &&
                    (NULL != p_ctx->p_hash_info->p_hash_update_api) &&
                    (NULL != p_ctx->p_hash_info->p_hash_final_api))
                {
                    /* check curve info */
                    ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
                    if (PKCL_RET_OK == ret)
                    {
                        ret = r_pkcl_eddsa_chk_variant(p_ctx->eddsa_variant, p_ctx->p_ed_ctx,
                                                        p_ctx->p_curve_info->curve_len);
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

        if (PKCL_RET_OK == ret)
        {
            /* check p_sign */
            ret = r_pkcl_eddsa_chk_sign(p_sign, p_ctx->p_curve_info->curve_len, PKCL_EDDSA_DATA_CHK_IN_ARG);
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

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_eddsa_chk_verify_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_keygen_operate_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EddsaKeyGenOperate.
 * \param       [in] p_ctx              A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in] processing_bits    Number of bits processed at one time when the operation is executed
 *                                       in multiple times in PKCC.
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
 *      - Call #r_pkcl_eddsa_chk_curve to check the curve information of p_ctx.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If (NULL != p_ctx)
 *    -# If ((0U < processing_bits) and (#PKCL_PROCESSING_BITS_MAX >= processing_bits))
 *      -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_hash_info) and
 *             (NULL != p_ctx->p_drbg_api) and (NULL != p_ctx->p_curve_info))
 *        -# Check the curve information p_ctx->p_curve_info with #r_pkcl_eddsa_chk_curve,
 *           and set ret to the return value.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_LENGTHERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_keygen_operate_arg(const st_pkc_eddsa_ctx_t *const p_ctx,
                                                        const uint32_t processing_bits)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if (NULL != p_ctx)
    {
        /* check processing_bits */
        if ((0U < processing_bits) && (PKCL_PROCESSING_BITS_MAX >= processing_bits))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_hash_info) &&
                (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info))
            {
                /* check hash info */
                if ((NULL != p_ctx->p_hash_info->p_hash_init_api) &&
                    (NULL != p_ctx->p_hash_info->p_hash_update_api) &&
                    (NULL != p_ctx->p_hash_info->p_hash_final_api))
                {
                    /* check curve info */
                    ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
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
            ret = PKCL_RET_LENGTHERR;
        }
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_eddsa_chk_keygen_operate_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_keygen_final_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EddsaKeyGenFinal.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in]     p_priv_key     EdDSA private key A structure consisting of the binary byte length len and
 *                                       a pointer to the binary.
 * \param       [in]     p_pub_key      EdDSA public key A structure consisting of the binary byte length len and
 *                                       a pointer to the binary.
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
 *      - Call #r_pkcl_eddsa_chk_curve to check the curve information of p_ctx.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_priv_key) and (NULL != p_pub_key))
 *    -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_hash_info) and
 *           (NULL != p_ctx->p_drbg_api) and (NULL != p_ctx->p_curve_info))
 *      -# If ((NULL != p_ctx->p_hash_info->p_hash_init_api) and
 *             (NULL != p_ctx->p_hash_info->p_hash_update_api) and
 *             (NULL != p_ctx->p_hash_info->p_hash_final_api))
 *        -# Check the curve information p_ctx->p_curve_info with #r_pkcl_eddsa_chk_curve,
 *           and set ret to the return value.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Retuen ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_keygen_final_arg(const st_pkc_eddsa_ctx_t *const p_ctx,
                                                    const void *const p_priv_key,
                                                    const void *const p_pub_key)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_priv_key) && (NULL != p_pub_key))
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_hash_info) &&
            (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info))
        {
            /* check hash info */
            if ((NULL != p_ctx->p_hash_info->p_hash_init_api) &&
                (NULL != p_ctx->p_hash_info->p_hash_update_api) &&
                (NULL != p_ctx->p_hash_info->p_hash_final_api))
            {
                /* check curve info */
                ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
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
* End of function r_pkcl_eddsa_chk_keygen_final_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_chk_scalar_mult_operate_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Check the arguments of #R_PKCL_LLEddsaScalarMultBaseOperate.
 * \param       [in] p_ctx              A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in] processing_bits    Number of bits processed at one time when the operation is executed
 *                                       in multiple times in PKCC.
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
 *      - Call #r_pkcl_eddsa_chk_curve to check the curve information of p_ctx.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If (NULL != p_ctx)
 *    -# If ((0U < processing_bits) and (#PKCL_PROCESSING_BITS_MAX >= processing_bits))
 *      -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_curve_info))
 *        -# Check the curve information p_ctx->p_curve_info with #r_pkcl_eddsa_chk_curve,
 *           and set ret to the return value.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_LENGTHERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_chk_scalar_mult_operate_arg(const st_pkc_eddsa_ctx_t *const p_ctx,
                                                            const uint32_t processing_bits)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if (NULL != p_ctx)
    {
        /* check processing_bits */
        if ((0U < processing_bits) && (PKCL_PROCESSING_BITS_MAX >= processing_bits))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_eddsa_chk_curve(p_ctx->p_curve_info);
            }
            else
            {
                ret = PKCL_RET_PARAMERR;
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

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_eddsa_chk_scalar_mult_operate_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_eddsa_verified_gen_sign
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLEDDSAAPIsInternal
 * \brief       Function that performs signature verification after signature generation
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in]     p_sign_r_data  A structure consisting of the byte length len with the signature value R
 *                                       and the data pointer p_data.
 * \param       [in]     p_sign_s_data  A structure consisting of the byte length len with the signature value S
 *                                       and the data pointer p_data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
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
 *      - Make sure the argument is not NULL.
 *      - Call #r_pkcl_eddsa_chk_curve to check the curve information of p_ctx.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# Assign a buffer for the EdDSA operation as p_eddsa_sign from p_ctx->p_buf.
 *  -# Set p_msg to p_eddsa_sign->p_msg.
 *  -# Set msg_data.p_data to p_msg.
 *  -# Set msg_data.len to p_eddsa_sign->msg_len.
 *  -# Set hash_ram_data.p_data to hash_ram_val.
 *  -# Set hash_ram_data.len to sizeof(hash_ram_val).
 *  -# Set p_buf_drv to p_ctx->p_buf[sizeof(st_pkcl_eddsa_sign_t)] address.
 *  -# If #PKC_EDDSA_VARIANT_HASH == p_ctx->eddsa_variant
 *    -# If #PKCL_EDDSA_ED448_CURVE_LEN == p_ctx->p_curve_info->curve_len
 *      -# Set hmsg_data.len to #PKCL_EDDSA_SHAKE256BYTES_PH.
 *    -# Set p_hash_ctx to p_ctx->p_hash_info->p_hash_ctx.
 *    -# Execute an external Hash init function p_ctx->p_hash_info->p_hash_init_api to initialize a hash context,
 *       and set the return value to hash_ret.
 *    -# If #PKCL_EXTERNAL_API_OK == hash_ret
 *      -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *         to hash a message, and set the return value to hash_ret.
 *      -# If #PKCL_EXTERNAL_API_OK == hash_ret
 *        -# Execute an external hash final function p_ctx->p_hash_info->p_hash_final_api
 *           to set the hased value and its byte size to hmsg_data.p_data,
 *           and set the return value to hash_ret.
 *        -# If #PKCL_EXTERNAL_API_OK == hash_ret
 *          -# Calculate H(R,A,M) value from the signature r, the public key and the message with
 *             R_PKCD_EddsaHashRAM and set drv_ret to the result.
 *          -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *        -# Else
 *          -# Set ret to #PKCL_RET_HASHAPIERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_HASHAPIERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_HASHAPIERR.
 *  -# Else if (#PKC_EDDSA_VARIANT_PURE == p_ctx->eddsa_variant) or<br>
 *             (#PKC_EDDSA_VARIANT_CTX == p_ctx->eddsa_variant)
 *    -# Calculate H(R,A,M) value from the signature r, the public key and the message with R_PKCD_EddsaHashRAM
 *       and set drv_ret to the result.
 *    -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set H(R,A,M) structure type hash_ram_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *    -# Set H(R,A,M) structure content address hash_ram_sel.p_sel_data to &hash_ram_data.
 *    -# Set the public key structure type pubkey_sel.data_type to #PKC_DATA_TYPE_KEY_BINARY.
 *    -# Set the public key structure content address pubkey_sel.p_sel_data to &pubkey_data.
 *    -# Set the public key address pubkey_sel.p_sel_data to p_eddsa_sign->p_pub_key.
 *    -# Initialize the multiplication of the public key by H(R,A,M) with R_PKCD_EddsaScalarMultInit
 *       and set drv_ret to the result.
 *    -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Operate the multiplication of the public key by H(R,A,M) with R_PKCD_EddsaScalarMultOperate
 *         and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Set H(R,A,M)*A address ha_data.p_data to ha_val.
 *        -# Set H(R,A,M)*A length ha_data.len to sizeof(ha_val).
 *        -# Finalize the multiplication of the public key by H(R,A,M) with R_PKCD_EddsaScalarMultFinal
 *           and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set sign_s_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *    -# Set sign_s_sel.p_sel_data to p_sign_s_data.
 *    -# Initialize the multiplicaion of the base point by signature s with R_PKCD_EddsaScalarMultBaseInit
 *       and set drv_ret to the result.
 *    -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Operate the multiplication of the base point by signature s with R_PKCD_EddsaScalarMultBaseOperate
 *           and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Set S*B address sb_data.p_data to sb_val.
 *          -# Set S*B length sb_data.len to sizeof(sb_val).
 *          -# Finalize the multiplication of the base point by the signature s with R_PKCD_EddsaScalarMultBaseFinal
 *             and set drv_ret to the result.
 *          -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *          -# If #PKCL_RET_OK == ret then,
 *            -# Verify the signature comparing the H(R,A,M)*A, S*B and the signature r with
 *               R_PKCD_EddsaCompareSign and set drv_ret to the result.
 *            -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# If #PKCL_RET_OK != ret
 *    -# Set ret to #PKCL_RET_TAMPERERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_eddsa_verified_gen_sign(const st_pkc_eddsa_ctx_t * const p_ctx,
                                                const st_pkc_data_t * const p_sign_r_data,
                                                const st_pkc_data_t * const p_sign_s_data)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    void * p_hash_ctx;
    st_pkc_data_t msg_data;
    st_pkc_data_t hmsg_data;
    st_pkc_data_t hash_ram_data;
    st_pkc_data_t ha_data;
    st_pkc_data_t sb_data;
    st_pkc_data_t pubkey_data;
    st_pkc_select_data_t pubkey_sel;
    st_pkc_select_data_t hash_ram_sel;
    st_pkc_select_data_t sign_s_sel;
    uint8_t hash_msg[PKCL_EDDSA_DIGEST_LEN_MAX] = {0U};
    uint8_t hash_ram_val[PKCL_EDDSA_DIGEST_LEN_MAX] = {0U};
    uint8_t ha_val[PKCL_EDDSA_UNCOMP_LEN_MAX] = {0U};
    uint8_t sb_val[PKCL_EDDSA_UNCOMP_LEN_MAX] = {0U};
    uint8_t * p_msg;
    uint8_t * p_buf_drv;
    uint32_t hash_ret;

    /* Cast p_buf to EdDSA sign buffer type */
    const st_pkcl_eddsa_sign_t * const p_eddsa_sign = (st_pkcl_eddsa_sign_t *)p_ctx->p_buf;

    /* Set verify parameters */
    p_msg = (uint8_t *)p_eddsa_sign->p_msg;
    msg_data.p_data = p_msg;
    msg_data.len = p_eddsa_sign->msg_len;

    /* Prepare to pass H(R,A,M) s to PKC-Driver */
    hash_ram_data.p_data = hash_ram_val;
    hash_ram_data.len = sizeof(hash_ram_val);

    /* Pass the p_buf address for PKC-Driver */
    p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_eddsa_sign_t)];

    /* HashEdDSA pre-hashing */
    if (PKC_EDDSA_VARIANT_HASH == p_ctx->eddsa_variant)
    {
        if (PKCL_EDDSA_ED448_CURVE_LEN == p_ctx->p_curve_info->curve_len)
        {
            hmsg_data.len = PKCL_EDDSA_SHAKE256BYTES_PH;
        }
        else
        {
            /* Do nothing */
        }

        p_hash_ctx = p_ctx->p_hash_info->p_hash_ctx;

        hash_ret = p_ctx->p_hash_info->p_hash_init_api(p_hash_ctx);
        if (PKCL_EXTERNAL_API_OK == hash_ret)
        {
            hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_hash_ctx,
                                                            msg_data.p_data,
                                                            msg_data.len);
            if (PKCL_EXTERNAL_API_OK == hash_ret)
            {
                hmsg_data.p_data = hash_msg;
                hash_ret = p_ctx->p_hash_info->p_hash_final_api(p_hash_ctx, hmsg_data.p_data,
                                                                &hmsg_data.len);
                if (PKCL_EXTERNAL_API_OK == hash_ret)
                {
                    /* Call PKC-Driver API */
                    drv_ret = R_PKCD_EddsaHashRAM(p_ctx, p_buf_drv, p_sign_r_data,
                                                    p_eddsa_sign->p_pub_key, &hmsg_data, &hash_ram_data);

                    /* Convert to PKC-Lib return code */
                    ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
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
    else if ((PKC_EDDSA_VARIANT_PURE == p_ctx->eddsa_variant) ||
            (PKC_EDDSA_VARIANT_CTX == p_ctx->eddsa_variant))
    {
        /* Call PKC-Driver API */
        drv_ret = R_PKCD_EddsaHashRAM(p_ctx, p_buf_drv, p_sign_r_data,
                                        p_eddsa_sign->p_pub_key, &msg_data, &hash_ram_data);

        /* Convert to PKC-Lib return code */
        ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
    }
    else
    {
        ret = PKCL_RET_INTERNALERR;
    }

    if (PKCL_RET_OK == ret)
    {
        /* Prepare to pass H(R,A,M) to PKC-Driver */
        hash_ram_sel.data_type = PKC_DATA_TYPE_BYTE;
        hash_ram_sel.p_sel_data = &hash_ram_data;

        /* Prepare to pass public key to PKC-Driver */
        pubkey_sel.data_type = PKC_DATA_TYPE_KEY_BINARY;
        pubkey_sel.p_sel_data = &pubkey_data;
        pubkey_sel.p_sel_data->p_data = (uint8_t *)p_eddsa_sign->p_pub_key; // No problem with casting to pointer size

        /* Call PKC-Driver API */
        drv_ret = R_PKCD_EddsaScalarMultInit(&hash_ram_sel, &pubkey_sel, p_ctx, p_buf_drv);

        /* Convert to PKC-Lib return code */
        ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultOperate(p_ctx, p_buf_drv, PKCL_PROCESSING_BITS_MAX);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Prepare to pass hA to PKC-Driver */
                ha_data.p_data = ha_val;
                ha_data.len = sizeof(ha_val);

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EddsaScalarMultFinal(p_ctx, p_buf_drv, PKC_BOOL_FALSE, &ha_data);

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
        /* Do nothing */
    }

    if (PKCL_RET_OK == ret)
    {
        sign_s_sel.data_type = PKC_DATA_TYPE_BYTE;
        sign_s_sel.p_sel_data = (st_pkc_data_t *)p_sign_s_data;  /* No problem with casting to pointer size */

        /* Call PKC-Driver API */
        drv_ret = R_PKCD_EddsaScalarMultBaseInit(&sign_s_sel, p_ctx, p_buf_drv);

        /* Convert to PKC-Lib return code */
        ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultBaseOperate(p_ctx, p_buf_drv, PKCL_PROCESSING_BITS_MAX);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            /* State transition */
            if (PKCL_RET_OK == ret)
            {
                /* Prepare to pass SB to PKC-Driver */
                sb_data.p_data = sb_val;
                sb_data.len = sizeof(sb_val);

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EddsaScalarMultBaseFinal(p_ctx, p_buf_drv, PKC_BOOL_FALSE, &sb_data);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                if (PKCL_RET_OK == ret)
                {
                    /* Call PKC-Driver API */
                    drv_ret = R_PKCD_EddsaCompareSign(p_ctx, p_buf_drv, &ha_data, &sb_data, p_sign_r_data);

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
* End of function r_pkcl_eddsa_verified_gen_sign()
**********************************************************************************************************************/


static pkcl_ret_t r_pkcl_eddsa_verify_operate_init_done(const st_pkc_eddsa_ctx_t *const p_ctx,
                                                        const uint32_t processing_bits)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    uint32_t hash_ret;
    void * p_hash_ctx;
    st_pkcl_eddsa_verify_t * p_eddsa_verify;
    st_pkc_data_t msg_data;
    st_pkc_data_t hmsg_data;
    st_pkc_data_t sign_r_data;
    st_pkc_data_t hash_ram_data;
    st_pkc_data_t pubkey_data;
    st_pkc_select_data_t pubkey_sel;
    st_pkc_select_data_t hash_ram_sel;
    uint8_t hash_msg[PKCL_EDDSA_DIGEST_LEN_MAX];
    uint8_t sign_r_val[PKCL_EDDSA_ED448_RU4_KEY_LEN];
    uint8_t hash_ram_val[PKCL_EDDSA_DIGEST_LEN_MAX];
    uint8_t * p_buf_drv;

    /* Check argument */
    ret = r_pkcl_eddsa_chk_sign_verify_operate_arg(p_ctx, processing_bits);
    if (PKCL_RET_OK == ret)
    {
        /* Cast p_buf to EdDSA verify buffer type */
        p_eddsa_verify = (st_pkcl_eddsa_verify_t *)p_ctx->p_buf;

        /* Prepare to pass message to PKC-Driver */
        msg_data.p_data = (uint8_t *)p_eddsa_verify->p_msg;
        msg_data.len = p_eddsa_verify->msg_len;

        /* Prepare to pass sign r to PKC-Driver */
        sign_r_data.len = (p_eddsa_verify->sign_len / PKCL_EDDSA_HALF_LEN);
        r_pkcl_cmn_cpy_byte(sign_r_val, &p_eddsa_verify->sign_data[0], sign_r_data.len);
        sign_r_data.p_data = sign_r_val;

        /* Prepare to pass H(R,A,M) s to PKC-Driver */
        hash_ram_data.p_data = hash_ram_val;
        hash_ram_data.len = sizeof(hash_ram_val);

        /* Pass the p_buf address for PKC-Driver */
        p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_eddsa_verify_t)];

        /* HashEdDSA pre-hashing */
        if (PKC_EDDSA_VARIANT_HASH == p_ctx->eddsa_variant)
        {
            if (PKCL_EDDSA_ED448_CURVE_LEN == p_ctx->p_curve_info->curve_len)
            {
                hmsg_data.len = PKCL_EDDSA_SHAKE256BYTES_PH;
            }
            else
            {
                /* Do nothing */
            }

            p_hash_ctx = p_ctx->p_hash_info->p_hash_ctx;

            hash_ret = p_ctx->p_hash_info->p_hash_init_api(p_hash_ctx);
            if (PKCL_EXTERNAL_API_OK == hash_ret)
            {
                hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_hash_ctx,
                                                                msg_data.p_data,
                                                                msg_data.len);
                if (PKCL_EXTERNAL_API_OK == hash_ret)
                {
                    hmsg_data.p_data = hash_msg;
                    hash_ret = p_ctx->p_hash_info->p_hash_final_api(p_hash_ctx, hmsg_data.p_data,
                                                                    &hmsg_data.len);
                    if (PKCL_EXTERNAL_API_OK == hash_ret)
                    {
                        /* Call PKC-Driver API */
                        drv_ret = R_PKCD_EddsaHashRAM(p_ctx, p_buf_drv, &sign_r_data,
                                                        p_eddsa_verify->p_pub_key, &hmsg_data, &hash_ram_data);

                        /* Convert to PKC-Lib return code */
                        ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
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
        else if ((PKC_EDDSA_VARIANT_PURE == p_ctx->eddsa_variant) ||
                (PKC_EDDSA_VARIANT_CTX == p_ctx->eddsa_variant))
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaHashRAM(p_ctx, p_buf_drv, &sign_r_data,
                                            p_eddsa_verify->p_pub_key, &msg_data, &hash_ram_data);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
        }
        else
        {
            ret = PKCL_RET_INTERNALERR;
        }

        if (PKCL_RET_OK == ret)
        {
            /* Prepare to pass H(R,A,M) to PKC-Driver */
            hash_ram_sel.data_type = PKC_DATA_TYPE_BYTE;
            hash_ram_sel.p_sel_data = &hash_ram_data;

            /* Prepare to pass public key to PKC-Driver */
            pubkey_sel.data_type = PKC_DATA_TYPE_KEY_BINARY;
            pubkey_sel.p_sel_data = &pubkey_data;
            pubkey_sel.p_sel_data->p_data =
                            (uint8_t *)p_eddsa_verify->p_pub_key; /* No problem with casting to pointer size */

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultInit(&hash_ram_sel, &pubkey_sel, p_ctx, p_buf_drv);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EddsaScalarMultOperate(p_ctx, p_buf_drv, processing_bits);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                /* State transition */
                r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_VERIFY_OPERATE_PROCESSING);

                if (PKCL_RET_OK == ret)
                {
                    p_eddsa_verify->ope_flg = PKCL_EDDSA_VERIFY_OPE_1ST_DONE;
                    ret = PKCL_RET_PROCESSING;
                }
                else if (PKCL_RET_PROCESSING == ret)
                {
                    p_eddsa_verify->ope_flg = PKCL_EDDSA_VERIFY_OPE_1ST_PROCESSING;
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

    return ret;
}


static pkcl_ret_t r_pkcl_eddsa_verify_operate_processing(const st_pkc_eddsa_ctx_t *const p_ctx,
                                                        const uint32_t processing_bits)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    st_pkcl_eddsa_verify_t * p_eddsa_verify;
    st_pkc_data_t sign_s_data;
    st_pkc_data_t ha_data;
    st_pkc_select_data_t sign_s_sel;
    uint8_t sign_s_val[PKCL_EDDSA_ED448_RU4_KEY_LEN];
    uint8_t * p_buf_drv;

    /* Check argument */
    ret = r_pkcl_eddsa_chk_sign_verify_operate_arg(p_ctx, processing_bits);
    if (PKCL_RET_OK == ret)
    {
        /* Cast p_buf to EdDSA verify buffer type */
        p_eddsa_verify = (st_pkcl_eddsa_verify_t *)p_ctx->p_buf;

        if (PKCL_EDDSA_VERIFY_OPE_1ST_PROCESSING == p_eddsa_verify->ope_flg)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_eddsa_verify_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultOperate(p_ctx, p_buf_drv, processing_bits);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            /* State transition */
            if (PKCL_RET_OK == ret)
            {
                p_eddsa_verify->ope_flg = PKCL_EDDSA_VERIFY_OPE_1ST_DONE;
                ret = PKCL_RET_PROCESSING;
            }
            else
            {
                /* Do nothing */
            }
        }
        else if (PKCL_EDDSA_VERIFY_OPE_1ST_DONE == p_eddsa_verify->ope_flg)
        {
            /* Prepare to pass hA to PKC-Driver */
            ha_data.p_data = p_eddsa_verify->ha_data;
            ha_data.len = sizeof(p_eddsa_verify->ha_data);

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_eddsa_verify_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultFinal(p_ctx, p_buf_drv, PKC_BOOL_FALSE, &ha_data);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Save hA */
                p_eddsa_verify->ha_len = ha_data.len;

                /* Prepare to pass sign s to PKC-Driver */
                sign_s_data.len = (p_eddsa_verify->sign_len / PKCL_EDDSA_HALF_LEN);
                r_pkcl_cmn_cpy_byte(sign_s_val,
                                    &p_eddsa_verify->sign_data[p_eddsa_verify->sign_len / PKCL_EDDSA_HALF_LEN],
                                    sign_s_data.len);
                sign_s_data.p_data = sign_s_val;

                sign_s_sel.data_type = PKC_DATA_TYPE_BYTE;
                sign_s_sel.p_sel_data = &sign_s_data;

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EddsaScalarMultBaseInit(&sign_s_sel, p_ctx, p_buf_drv);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                if (PKCL_RET_OK == ret)
                {
                    /* Call PKC-Driver API */
                    drv_ret = R_PKCD_EddsaScalarMultBaseOperate(p_ctx, p_buf_drv, processing_bits);

                    /* Convert to PKC-Lib return code */
                    ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                    /* State transition */
                    if (PKCL_RET_OK == ret)
                    {
                        r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_VERIFY_OPERATE_DONE);
                    }
                    else if (PKCL_RET_PROCESSING == ret)
                    {
                        p_eddsa_verify->ope_flg = PKCL_EDDSA_VERIFY_OPE_2ND_PROCESSING;
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
        else if (PKCL_EDDSA_VERIFY_OPE_2ND_PROCESSING == p_eddsa_verify->ope_flg)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_eddsa_verify_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EddsaScalarMultBaseOperate(p_ctx, p_buf_drv, processing_bits);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            /* State transition */
            if (PKCL_RET_OK == ret)
            {
                r_pkcl_cmn_set_state(PKCL_STATE_EDDSA_VERIFY_OPERATE_DONE);
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCL_RET_INTERNALERR;
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
