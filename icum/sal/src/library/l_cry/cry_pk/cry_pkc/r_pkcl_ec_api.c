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
/* PRQA S 0751 ++ # D006 Cast required to match the type of the calling function */
/* PRQA S 1503 ++ # D010 No problem if it is unused because it is an API called from outside the library  */
/* PRQA S 3383 ++ # D012 Have confirmed in advance that the value doesn't wrap around. */
/* PRQA S 2981 ++ # D013 Local variable ret of APIs sets INTERNALERR to the initial value
                        as a countermeasure against attacks that pass through processing. */
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
 * \addtogroup    PKCLEC
 * \{
 * \addtogroup    PKCLECDefines             EC Definitions
 * \addtogroup    PKCLECTypes               EC Types
 * \addtogroup    PKCLECConstants           EC Constants
 * \addtogroup    PKCLECAPIs                EC APIs
 * \addtogroup    PKCLECDefinesInternal     EC Internal Definitions
 * \addtogroup    PKCLECTypesInternal       EC Internal Types
 * \addtogroup    PKCLECVariablesInternal   EC Internal Variables
 * \addtogroup    PKCLECAPIsInternal        EC Internal APIs
 * \}******************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \addtogroup PKCLECDefinesInternal
 * \{*****************************************************************/
#define PKCL_EC_CURVE_LEN_MIN      (24U)      /**< Minimum length of curve length */
#define PKCL_EC_CURVE_LEN_MAX      (68U)      /**< Maximum length of curve length */

#define PKCL_EC_CURVE_PARAM_NUM    (6U)       /**< Number of EC curve parameters */
#define PKCL_EC_CURVE_OFFSET_GX    (4U)       /**< Start address offset of GX coordinate point data of EC curve */
#define PKCL_EC_PRIV_SCALAR_NUM    (8U)       /**< 8 bytes extra space to take when generating priv_scalar */
#define PKCL_EC_DOUBLE_LEN         (2U)       /**< Definition to calculate twice the length */

#define PKCL_EC_SELECT_MODE_SCALAR (0U)       /**< Argument check mode selection definition, scalar */
#define PKCL_EC_SELECT_MODE_POINT  (1U)       /**< Argument check mode selection definition, coordinate point */

#define PKCL_EC_DATA_CHK_IN_ARG    (0U)       /**< If the argument to check is an input argument */
#define PKCL_EC_DATA_CHK_OUT_ARG   (1U)       /**< If the argument to check is an output argument */

#define PKCL_EC_DIGEST_LEN_MAX     (0x80000000U) /**< Maximum digest message length */

#define PKCL_EC_KDF_OUT_LEN_MAX    (1024U)    /**< Maximum size of output KDF */
/*! \}*/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \addtogroup PKCLECTypesInternal
 * \{*****************************************************************/
/** A structure of values that needs to be buffered in EC Signature generation */
typedef struct
{
    uint32_t        digest_len;
    uint8_t         digest_data[PKCL_EC_CURVE_LEN_MAX];
    const void *    p_priv_key;
    uint32_t        priv_scalar_len;
    uint8_t         priv_scalar_data[PKCL_EC_CURVE_LEN_MAX];
    uint8_t         sign_temp[PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN];
} st_pkcl_ec_sign_t;

/** A structure of values that needs to be buffered in EC signature verification after sigature generation */
typedef struct
{
    uint8_t     base_point[PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN];
    uint8_t     u1[PKCL_EC_CURVE_LEN_MAX];
    uint8_t     u2[PKCL_EC_CURVE_LEN_MAX];
    uint8_t     u1g[PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN];
    uint8_t     u2q[PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN];
} st_pkcl_ec_verified_sign_t;

/** A structure of values that needs to be buffered in EC Signature verification */
typedef struct
{
    uint32_t        digest_len;
    uint8_t         digest_data[PKCL_EC_CURVE_LEN_MAX];
    uint32_t        sign_len;
    uint8_t         sign_data[PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN];
    const void *    p_pub_key;
    uint32_t        u2_len;
    uint8_t         u2_data[PKCL_EC_CURVE_LEN_MAX];
    uint32_t        u1g_len;
    uint8_t         u1g_data[PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN];
    e_pkc_bool_t    is_u1_zero;
} st_pkcl_ec_verify_t;

/** A structure of values that needs to be buffered in ECDH */
typedef struct
{
    const void *    p_priv_key;
    const void *    p_pub_key;
    uint8_t         shared_data[PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN];
} st_pkcl_ec_ecdh_t;

/*! \}*/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ec_chk_digest (const st_pkc_data_t * const p_digest);
static pkcl_ret_t r_pkcl_ec_chk_curve (const st_pkc_ec_curve_t * const p_curve);
static pkcl_ret_t r_pkcl_ec_chk_sign_or_point (const st_pkc_data_t * const p_sign_or_point, const uint32_t curve_len,
                                                const uint32_t arg_type);
static pkcl_ret_t r_pkcl_ec_chk_select_data (const st_pkc_select_data_t * const p_select_data,
                                            const uint32_t curve_len, const uint32_t chk_mode);
static pkcl_ret_t r_pkcl_ec_chk_verify_operate_arg (const st_pkc_ec_ctx_t * const p_ctx,
                                                    const e_pkc_sync_opt_t sync_opt);
static pkcl_ret_t r_pkcl_ec_chk_make_sign_arg (const st_pkc_ec_ctx_t * const p_ctx,
                                                const st_pkc_data_t * const p_digest,
                                                const st_pkc_data_t * const p_point,
                                                const st_pkc_data_t * const p_random,
                                                const void * const p_priv_key,
                                                const st_pkc_data_t * const p_sign);
static pkcl_ret_t r_pkcl_ec_chk_calc_u1u2_arg (const st_pkc_ec_ctx_t * const p_ctx,
                                                const st_pkc_data_t * const p_digest,
                                                const st_pkc_data_t * const p_sign,
                                                const st_pkc_data_t * const p_u1,
                                                const st_pkc_data_t * const p_u2);
static pkcl_ret_t r_pkcl_ec_chk_compare_sign_arg (const st_pkc_ec_ctx_t * const p_ctx,
                                                    const st_pkc_data_t * const p_u1g,
                                                    const st_pkc_data_t * const p_u2q,
                                                    const st_pkc_data_t * const p_sign);
static pkcl_ret_t r_pkcl_ec_chk_ecdh_operate_arg (const st_pkc_ec_ctx_t * const p_ctx,
                                                    const e_pkc_sync_opt_t sync_opt);
static pkcl_ret_t r_pkcl_ec_chk_ecdh_final_arg (const st_pkc_ec_ctx_t * const p_ctx,
                                                const st_pkc_data_t * const p_shared);
static pkcl_ret_t r_pkcl_ec_verified_gen_sign (const st_pkc_ec_ctx_t * const p_ctx, uint8_t * const p_buf,
                                            const void * const p_priv_key,
                                            const st_pkc_data_t * const p_digest_data,
                                            const st_pkc_data_t * const p_sign_data);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdsaSignInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdsaSignInit(const st_pkc_data_t* const p_digest, const void* const p_priv_key,
                                st_pkc_ec_ctx_t* const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_ECDSA_SIGN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkcl_state_t pkcl_state;
    st_pkcl_ec_sign_t * p_ec_sign;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* check if the argument is null */
        if ((NULL != p_digest) && (NULL != p_priv_key) && (NULL != p_ctx))
        {
            /* check digest */
            ret = r_pkcl_ec_chk_digest(p_digest);
            if (PKCL_RET_OK == ret)
            {
                /* check context */
                if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info))
                {
                    /* check curve info */
                    ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
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

        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to EC sign buffer type */
            p_ec_sign = (st_pkcl_ec_sign_t*)p_ctx->p_buf;

            /* Save p_digest */
            if (PKCL_EC_CURVE_LEN_MAX < p_digest->len)
            {
                p_ec_sign->digest_len = PKCL_EC_CURVE_LEN_MAX;
            }
            else
            {
                p_ec_sign->digest_len = p_digest->len;
            }
            r_pkcl_cmn_cpy_byte(&p_ec_sign->digest_data[0], p_digest->p_data, p_ec_sign->digest_len);

            /* Save p_priv_key */
            p_ec_sign->p_priv_key = p_priv_key;

            /* State transition */
            r_pkcl_cmn_set_state(PKCL_STATE_EC_SIGN_INIT_DONE);
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
#else  /* PKCL_CFG_ENABLE_DRV_ECDSA_SIGN != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDSA_SIGN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdsaSignInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdsaSignOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdsaSignOperate(st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt)
{
#if (PKCL_CFG_ENABLE_DRV_ECDSA_SIGN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_ec_sign_t * p_ec_sign;
    st_pkc_data_t random_data;
    st_pkc_data_t priv_scalar_data;
    st_pkc_data_t base_point_data;
    st_pkc_select_data_t priv_scalar_sel;
    st_pkc_select_data_t base_point_sel;
    uint8_t random_val[PKCL_EC_CURVE_LEN_MAX + PKCL_EC_PRIV_SCALAR_NUM];
    uint8_t base_point_val[PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN];
    uint8_t * p_buf_drv;
    uint32_t drbg_ret;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EC_SIGN_INIT_DONE == pkcl_state)
    {
        /* check if the argument is null or out of range */
        if ((NULL != p_ctx) && ((PKC_SYNC_OPT_SYNC == sync_opt) || (PKC_SYNC_OPT_ASYNC == sync_opt)))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
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
            /* Cast p_buf to EC sign buffer type */
            p_ec_sign = (st_pkcl_ec_sign_t*)p_ctx->p_buf;

            /* generation Random value */
            random_data.p_data = random_val;
            random_data.len = p_ctx->p_curve_info->curve_len + PKCL_EC_PRIV_SCALAR_NUM;
            drbg_ret = p_ctx->p_drbg_api(random_data.len, random_data.p_data);

            if (PKCL_EXTERNAL_API_OK == drbg_ret)
            {
                /* Prepare to pass private scalar to PKC-Driver */
                priv_scalar_data.p_data = p_ec_sign->priv_scalar_data;
                priv_scalar_data.len = sizeof(p_ec_sign->priv_scalar_data);

                /* Pass the p_buf address for PKC-Driver */
                p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_ec_sign_t)];

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EcPrivateScalarGen(p_ctx, p_buf_drv, &random_data, &priv_scalar_data);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                /* Clear the random value used for signature generation stored on the stack */
                r_pkcl_cmn_set_byte(random_val, 0U, sizeof(random_val));

                if (PKCL_RET_OK == ret)
                {
                    /* Save private scalar */
                    p_ec_sign->priv_scalar_len = priv_scalar_data.len;

                    /* Prepare to pass private scalar to PKC-Driver */
                    priv_scalar_sel.data_type = PKC_DATA_TYPE_BYTE;
                    priv_scalar_sel.p_sel_data = &priv_scalar_data;

                    /* Prepare to pass curve base point to PKC-Driver */
                    r_pkcl_cmn_cpy_byte(&base_point_val[0],
                                        &p_ctx->p_curve_info->curve_params.p_data[p_ctx->p_curve_info->curve_len *
                                                                                    PKCL_EC_CURVE_OFFSET_GX],
                                        p_ctx->p_curve_info->curve_len * PKCL_EC_DOUBLE_LEN);
                    base_point_data.p_data = base_point_val;
                    base_point_data.len = p_ctx->p_curve_info->curve_len * PKCL_EC_DOUBLE_LEN;

                    base_point_sel.data_type = PKC_DATA_TYPE_BYTE;
                    base_point_sel.p_sel_data = &base_point_data;

                    /* Call PKC-Driver API */
                    drv_ret = R_PKCD_EcScalarMultInit(&priv_scalar_sel, &base_point_sel, p_ctx, p_buf_drv);

                    /* Convert to PKC-Lib return code */
                    ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                    if (PKCL_RET_OK == ret)
                    {
                        /* Call PKC-Driver API */
                        drv_ret = R_PKCD_EcScalarMultOperate(p_ctx, p_buf_drv, sync_opt);

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
        r_pkcl_cmn_set_state(PKCL_STATE_EC_SIGN_OPERATE_DONE);
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
#else  /* PKCL_CFG_ENABLE_DRV_ECDSA_SIGN != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDSA_SIGN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdsaSignOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdsaSignFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdsaSignFinal(const st_pkc_ec_ctx_t* const p_ctx, st_pkc_data_t* const p_sign)
{
#if (PKCL_CFG_ENABLE_DRV_ECDSA_SIGN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_ec_sign_t * p_ec_sign;
    st_pkc_data_t digest_data;
    st_pkc_data_t priv_scalar_data;
    st_pkc_data_t point_data;
    uint8_t point_val[PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN];
    uint8_t * p_buf_drv;
    st_pkc_data_t sign_temp_data;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();

    if (PKCL_STATE_EC_SIGN_OPERATE_DONE == pkcl_state)
    {
        /* check if the argument is null */
        if ((NULL != p_ctx) && (NULL != p_sign))
        {
            /* check p_ctx */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info))
            {
                /* check p_curve_info */
                ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);

                if (PKCL_RET_OK == ret)
                {
                    /* check p_sign */
                    ret = r_pkcl_ec_chk_sign_or_point(p_sign, p_ctx->p_curve_info->curve_len,
                                                        PKCL_EC_DATA_CHK_OUT_ARG);
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
            /* Prepare to pass point_data to PKC-Driver */
            point_data.len = sizeof(point_val);
            point_data.p_data = point_val;

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_ec_sign_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcScalarMultFinal(p_ctx, p_buf_drv, &point_data);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Cast p_buf to EC sign buffer type */
                p_ec_sign = (st_pkcl_ec_sign_t*)p_ctx->p_buf;

                /* Buffer data retrieval */
                digest_data.len = p_ec_sign->digest_len;
                digest_data.p_data = p_ec_sign->digest_data;
                priv_scalar_data.len = p_ec_sign->priv_scalar_len;
                priv_scalar_data.p_data = p_ec_sign->priv_scalar_data;

                /* Set the temporal buffer for generated signature */
                sign_temp_data.len = sizeof(p_ec_sign->sign_temp);
                sign_temp_data.p_data = p_ec_sign->sign_temp;

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EcdsaMakeSign(p_ctx, p_buf_drv, &digest_data, &point_data,
                                                &priv_scalar_data, p_ec_sign->p_priv_key, &sign_temp_data);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                /* Clear the value of (random number mod p) used in signature generation stored in p_buf */
                r_pkcl_cmn_set_byte(p_ec_sign->priv_scalar_data, 0U, sizeof(p_ec_sign->priv_scalar_data));

                if (PKCL_RET_OK == ret)
                {
                    if (PKC_BOOL_FALSE != p_ctx->verified_gen_sign)
                    {
                        ret = r_pkcl_ec_verified_gen_sign(p_ctx, &p_ctx->p_buf[sizeof(st_pkcl_ec_sign_t)],
                                                            p_ec_sign->p_priv_key, &digest_data, &sign_temp_data);
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
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if ((PKCL_RET_OK == ret) || (PKCL_RET_PKCCTIMEOUTERR == ret) ||
        (PKCL_RET_PKCCERR == ret) || (PKCL_RET_EC_ZERO_SIGNATURE == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_ECDSA_SIGN != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDSA_SIGN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdsaSignFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdsaVerifyInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdsaVerifyInit(const st_pkc_data_t* const p_digest, const st_pkc_data_t* const p_sign,
                                    const void* const p_pub_key, st_pkc_ec_ctx_t* const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkcl_state_t pkcl_state;
    st_pkcl_ec_verify_t * p_ec_verify;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* check if the argument is null */
        if ((NULL != p_digest) && (NULL != p_sign) && (NULL != p_pub_key) && (NULL != p_ctx))
        {
            /* check digest */
            ret = r_pkcl_ec_chk_digest(p_digest);
            if (PKCL_RET_OK == ret)
            {
                /* check context */
                if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
                {
                    /* check curve info */
                    ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
                    if (PKCL_RET_OK == ret)
                    {
                        /* check p_sign */
                        ret = r_pkcl_ec_chk_sign_or_point(p_sign, p_ctx->p_curve_info->curve_len,
                                                            PKCL_EC_DATA_CHK_IN_ARG);
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
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }

        if (PKCL_RET_OK == ret)
        {
            /* Set anti-tampering scalar multiplication option to FALSE */
            p_ctx->sec_scalar_mul = PKC_BOOL_FALSE;

            /* Cast p_buf to EC verify buffer type */
            p_ec_verify = (st_pkcl_ec_verify_t*)p_ctx->p_buf;

            /* Save p_digest */
            if (PKCL_EC_CURVE_LEN_MAX < p_digest->len)
            {
                p_ec_verify->digest_len = PKCL_EC_CURVE_LEN_MAX;
            }
            else
            {
                p_ec_verify->digest_len = p_digest->len;
            }
            r_pkcl_cmn_cpy_byte(&p_ec_verify->digest_data[0], p_digest->p_data, p_ec_verify->digest_len);

            /* Save p_sign */
            p_ec_verify->sign_len = p_sign->len;
            r_pkcl_cmn_cpy_byte(&p_ec_verify->sign_data[0], p_sign->p_data, p_ec_verify->sign_len);

            /* Save p_pub_key */
            p_ec_verify->p_pub_key = p_pub_key;

            /* State transition */
            r_pkcl_cmn_set_state(PKCL_STATE_EC_VERIFY_INIT_DONE);
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
#else  /* PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdsaVerifyInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdsaVerifyOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdsaVerifyOperate(st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt)
{
#if (PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_ec_verify_t * p_ec_verify;
    st_pkc_data_t digest_data;
    st_pkc_data_t sign_data;
    st_pkc_data_t u1_data;
    st_pkc_data_t u2_data;
    st_pkc_data_t u1g_data;
    st_pkc_data_t base_point_data;
    st_pkc_data_t pubkey_data;
    st_pkc_select_data_t u1_sel;
    st_pkc_select_data_t u2_sel;
    st_pkc_select_data_t base_point_sel;
    st_pkc_select_data_t pubkey_sel;
    uint8_t u1_val[PKCL_EC_CURVE_LEN_MAX];
    uint8_t base_point_val[PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN];
    uint8_t * p_buf_drv;
    uint32_t loop_num = 0U;
    const uint32_t * p_u1_data32;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EC_VERIFY_INIT_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ec_chk_verify_operate_arg(p_ctx, sync_opt);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to EC verify buffer type */
            p_ec_verify = (st_pkcl_ec_verify_t*)p_ctx->p_buf;

            /* Prepare to pass digest_data to PKC-Driver */
            digest_data.p_data = p_ec_verify->digest_data;
            digest_data.len = p_ec_verify->digest_len;

            /* Prepare to pass sign_data to PKC-Driver */
            sign_data.p_data = p_ec_verify->sign_data;
            sign_data.len = p_ec_verify->sign_len;

            /* Prepare to pass u1,u2 to PKC-Driver */
            u1_data.p_data = u1_val;
            u1_data.len    = PKCL_EC_CURVE_LEN_MAX;
            u2_data.p_data = p_ec_verify->u2_data;
            u2_data.len    = PKCL_EC_CURVE_LEN_MAX;

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_ec_verify_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcdsaCalcU1U2(p_ctx, p_buf_drv, &digest_data, &sign_data, &u1_data, &u2_data);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            /* Check if u1_data = 0 */
            p_ec_verify->is_u1_zero = PKC_BOOL_TRUE;
            p_u1_data32 = (uint32_t*)u1_data.p_data; /* No problem with casting to pointer size */

            while (loop_num < (p_ctx->p_curve_info->curve_len / sizeof(uint32_t)))
            {
                if (0U != p_u1_data32[loop_num])
                {
                    p_ec_verify->is_u1_zero = PKC_BOOL_FALSE;
                    break;
                }
                else
                {
                    /* Do nothing */
                }

                loop_num++;
            }

            if (PKCL_RET_OK == ret)
            {
                /* Save u2 */
                p_ec_verify->u2_len = u2_data.len;

                if (PKC_BOOL_FALSE == p_ec_verify->is_u1_zero)
                {
                    /* Prepare to pass u1 to PKC-Driver */
                    u1_sel.data_type = PKC_DATA_TYPE_BYTE;
                    u1_sel.p_sel_data = &u1_data;

                    /* Prepare to pass curve base point to PKC-Driver */
                    r_pkcl_cmn_cpy_byte(&base_point_val[0],
                                        &p_ctx->p_curve_info->curve_params.p_data[p_ctx->p_curve_info->curve_len *
                                                                                    PKCL_EC_CURVE_OFFSET_GX],
                                        p_ctx->p_curve_info->curve_len * PKCL_EC_DOUBLE_LEN);
                    base_point_data.p_data = &base_point_val[0];
                    base_point_data.len = p_ctx->p_curve_info->curve_len * PKCL_EC_DOUBLE_LEN;

                    base_point_sel.data_type = PKC_DATA_TYPE_BYTE;
                    base_point_sel.p_sel_data = &base_point_data;

                    /* Call PKC-Driver API */
                    drv_ret = R_PKCD_EcScalarMultInit(&u1_sel, &base_point_sel, p_ctx, p_buf_drv);

                    /* Convert to PKC-Lib return code */
                    ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                    if (PKCL_RET_OK == ret)
                    {
                        /* Call PKC-Driver API */
                        drv_ret = R_PKCD_EcScalarMultOperate(p_ctx, p_buf_drv, sync_opt);

                        /* Convert to PKC-Lib return code */
                        ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                        if (PKCL_RET_OK == ret)
                        {
                            ret = PKCL_RET_PROCESSING;
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
                    ret = PKCL_RET_PROCESSING;
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
    else if (PKCL_STATE_EC_VERIFY_OPERATE_PROCESSING == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ec_chk_verify_operate_arg(p_ctx, sync_opt);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to EC verify buffer type */
            p_ec_verify = (st_pkcl_ec_verify_t*)p_ctx->p_buf;

            /* Prepare to pass point_data to PKC-Driver */
            u1g_data.len = sizeof(p_ec_verify->u1g_data);
            u1g_data.p_data = p_ec_verify->u1g_data;

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_ec_verify_t)];

            if (PKC_BOOL_FALSE == p_ec_verify->is_u1_zero)
            {
                /* Call PKC-Driver API to get u1*G */
                drv_ret = R_PKCD_EcScalarMultFinal(p_ctx, p_buf_drv, &u1g_data);
            }
            else
            {
                u1g_data.len = PKCL_EC_DOUBLE_LEN * p_ctx->p_curve_info->curve_len;
                r_pkcl_cmn_set_byte(u1g_data.p_data, 0U, PKCL_EC_DOUBLE_LEN * p_ctx->p_curve_info->curve_len);
                drv_ret = PKCD_RET_OK;
            }

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Save u1g */
                p_ec_verify->u1g_len = u1g_data.len;

                /* Prepare to pass u2 to PKC-Driver */
                u2_data.p_data = p_ec_verify->u2_data;
                u2_data.len    = p_ec_verify->u2_len;
                u2_sel.data_type = PKC_DATA_TYPE_BYTE;
                u2_sel.p_sel_data = &u2_data;

                /* Prepare to pass public key to PKC-Driver */
                pubkey_sel.data_type = PKC_DATA_TYPE_KEY_BINARY;
                pubkey_sel.p_sel_data = &pubkey_data;

                /* No problem with casting to pointer size */
                pubkey_sel.p_sel_data->p_data = (uint8_t *)p_ec_verify->p_pub_key;

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EcScalarMultInit(&u2_sel, &pubkey_sel, p_ctx, p_buf_drv);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                if (PKCL_RET_OK == ret)
                {
                    /* Call PKC-Driver API */
                    drv_ret = R_PKCD_EcScalarMultOperate(p_ctx, p_buf_drv, sync_opt);

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
        ret = PKCL_RET_STATUSERR;
    }

    /* State transition */
    if (PKCL_RET_OK == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_EC_VERIFY_OPERATE_DONE);
    }
    else if (PKCL_RET_PROCESSING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_EC_VERIFY_OPERATE_PROCESSING);
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
#else  /* PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdsaVerifyOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdsaVerifyFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdsaVerifyFinal(const st_pkc_ec_ctx_t* const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_ec_verify_t * p_ec_verify;
    st_pkc_data_t u1g_data;
    st_pkc_data_t u2q_data;
    st_pkc_data_t sign_data;
    uint8_t u2q_val[PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN];
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EC_VERIFY_OPERATE_DONE == pkcl_state)
    {
        /* check if the argument is null */
        if (NULL != p_ctx)
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
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
            /* Cast p_buf to EC verify buffer type */
            p_ec_verify = (st_pkcl_ec_verify_t*)p_ctx->p_buf;

            /* Prepare to pass point_data to PKC-Driver */
            u2q_data.len = sizeof(u2q_val);
            u2q_data.p_data = u2q_val;

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_ec_verify_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcScalarMultFinal(p_ctx, p_buf_drv, &u2q_data);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Prepare to pass u1g to PKC-Driver */
                u1g_data.p_data = p_ec_verify->u1g_data;
                u1g_data.len    = p_ec_verify->u1g_len;

                /* Prepare to pass sign to PKC-Driver */
                sign_data.p_data = p_ec_verify->sign_data;
                sign_data.len    = p_ec_verify->sign_len;

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EcdsaCompareSign(p_ctx, p_buf_drv, &u1g_data, &u2q_data, &sign_data);

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
    if ((PKCL_RET_OK == ret) || (PKCL_RET_PKCCTIMEOUTERR == ret) ||
        (PKCL_RET_PKCCERR == ret) || (PKCL_RET_EC_VERIFAIL == ret))
    {
        r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdsaVerifyFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEcScalarMultInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEcScalarMultInit(const st_pkc_select_data_t* const p_scalar_or_key,
                                        const st_pkc_select_data_t* const p_point_or_key,
                                        st_pkc_ec_ctx_t* const p_ctx)
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
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info)
                && ((NULL != p_ctx->p_drbg_api) || (PKC_BOOL_FALSE == p_ctx->sec_scalar_mul)))
            {
                /* check curve info */
                ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
            }
            else
            {
                ret = PKCL_RET_PARAMERR;
            }

            if (PKCL_RET_OK == ret)
            {
                /* check scalar select data param */
                ret = r_pkcl_ec_chk_select_data(p_scalar_or_key,
                                                p_ctx->p_curve_info->curve_len, PKCL_EC_SELECT_MODE_SCALAR);
                if (PKCL_RET_OK == ret)
                {
                    /* check point select data param */
                    ret = r_pkcl_ec_chk_select_data(p_point_or_key,
                                                    p_ctx->p_curve_info->curve_len, PKCL_EC_SELECT_MODE_POINT);
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
            drv_ret = R_PKCD_EcScalarMultInit(p_scalar_or_key, p_point_or_key, p_ctx, p_ctx->p_buf);

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
        r_pkcl_cmn_set_state(PKCL_STATE_EC_SMULT_INIT_DONE);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_LLEcScalarMultInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEcScalarMultOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEcScalarMultOperate(st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EC_SMULT_INIT_DONE == pkcl_state)
    {
        /* check if the argument is null or out of range */
        if ((NULL != p_ctx) && ((PKC_SYNC_OPT_SYNC == sync_opt) || (PKC_SYNC_OPT_ASYNC == sync_opt)))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
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
            drv_ret = R_PKCD_EcScalarMultOperate(p_ctx, p_ctx->p_buf, sync_opt);

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
        r_pkcl_cmn_set_state(PKCL_STATE_EC_SMULT_OPERATE_DONE);
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
* End of function R_PKCL_LLEcScalarMultOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEcScalarMultFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEcScalarMultFinal(const st_pkc_ec_ctx_t* const p_ctx, st_pkc_data_t* const p_point_out)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EC_SMULT_OPERATE_DONE == pkcl_state)
    {
        /* check if the argument is null */
        if ((NULL != p_ctx) && (NULL != p_point_out))
        {
            /* check p_ctx */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                /* check p_curve_info */
                ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);

                if (PKCL_RET_OK == ret)
                {
                    /* check p_sign */
                    ret = r_pkcl_ec_chk_sign_or_point(p_point_out, p_ctx->p_curve_info->curve_len,
                                                        PKCL_EC_DATA_CHK_OUT_ARG);
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
            drv_ret = R_PKCD_EcScalarMultFinal(p_ctx, p_ctx->p_buf, p_point_out);

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
* End of function R_PKCL_LLEcScalarMultFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEcdsaMakeSign
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEcdsaMakeSign(const st_pkc_ec_ctx_t* const p_ctx, const st_pkc_data_t* const p_digest,
                                    const st_pkc_data_t* const p_point, const st_pkc_data_t* const p_random,
                                    const void* const p_priv_key, st_pkc_data_t* const p_sign)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkc_data_t sign_temp_data;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ec_chk_make_sign_arg(p_ctx, p_digest, p_point, p_random, p_priv_key, p_sign);
        if (PKCL_RET_OK == ret)
        {
            sign_temp_data.p_data = p_ctx->p_buf;
            sign_temp_data.len    = PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN;

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcdsaMakeSign(p_ctx, &p_ctx->p_buf[PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN], p_digest,
                                            p_point, p_random, p_priv_key, &sign_temp_data);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                if (PKC_BOOL_FALSE != p_ctx->verified_gen_sign)
                {
                    ret = r_pkcl_ec_verified_gen_sign(p_ctx, &p_ctx->p_buf[PKCL_EC_CURVE_LEN_MAX * PKCL_EC_DOUBLE_LEN],
                                                        p_priv_key, p_digest, &sign_temp_data);
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

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_LLEcdsaMakeSign()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEcdsaCalcU1U2
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEcdsaCalcU1U2(const st_pkc_ec_ctx_t* const p_ctx,
                                    const st_pkc_data_t* const p_digest, const st_pkc_data_t* const p_sign,
                                    st_pkc_data_t* const p_u1, st_pkc_data_t* const p_u2)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ec_chk_calc_u1u2_arg(p_ctx, p_digest, p_sign, p_u1, p_u2);
        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcdsaCalcU1U2(p_ctx, p_ctx->p_buf, p_digest, p_sign, p_u1, p_u2);

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
* End of function R_PKCL_LLEcdsaCalcU1U2()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEcdsaCompareSign
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_LLEcdsaCompareSign(const st_pkc_ec_ctx_t* const p_ctx, const st_pkc_data_t* const p_u1g,
                                        const st_pkc_data_t* const p_u2q, const st_pkc_data_t* const p_sign)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ec_chk_compare_sign_arg(p_ctx, p_u1g, p_u2q, p_sign);
        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcdsaCompareSign(p_ctx, p_ctx->p_buf, p_u1g, p_u2q, p_sign);

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
* End of function R_PKCL_LLEcdsaCompareSign()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdhInit(const void* const p_priv_key, const void* const p_pub_key,
                            st_pkc_ec_ctx_t* const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_ECDH != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkcl_state_t pkcl_state;
    st_pkcl_ec_ecdh_t * p_ec_ecdh;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* check if the argument is null */
        if ((NULL != p_priv_key) && (NULL != p_pub_key) && (NULL != p_ctx))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
                if (PKCL_RET_OK == ret)
                {
                    /* check kdf info */
                    if (NULL != p_ctx->p_kdf_info)
                    {
                        if (PKCL_EC_KDF_OUT_LEN_MAX >= p_ctx->p_kdf_info->out_kdf_len)
                        {
                            if (NULL != p_ctx->p_kdf_info->p_kdf_api)
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
                            ret = PKCL_RET_LENGTHERR;
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
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }

        if (PKCL_RET_OK == ret)
        {
            /* Set anti-tampering scalar multiplication option to FALSE */
            p_ctx->sec_scalar_mul = PKC_BOOL_FALSE;

            /* Cast p_buf to ecdh buffer type */
            p_ec_ecdh = (st_pkcl_ec_ecdh_t*)p_ctx->p_buf;

            /* Save p_priv_key */
            p_ec_ecdh->p_priv_key = p_priv_key;

            /* Save p_pub_key */
            p_ec_ecdh->p_pub_key = p_pub_key;

            /* State transition */
            r_pkcl_cmn_set_state(PKCL_STATE_EC_ECDH_INIT_DONE);
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
#else  /* PKCL_CFG_ENABLE_DRV_ECDH != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDH != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdhInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdhOperate(st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt)
{
#if (PKCL_CFG_ENABLE_DRV_ECDH != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    const st_pkcl_ec_ecdh_t * p_ec_ecdh;
    st_pkc_data_t privkey_data;
    st_pkc_data_t pubkey_data;
    st_pkc_select_data_t privkey_sel;
    st_pkc_select_data_t pubkey_sel;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EC_ECDH_INIT_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ec_chk_ecdh_operate_arg(p_ctx, sync_opt);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to ecdh buffer type */
            p_ec_ecdh = (const st_pkcl_ec_ecdh_t*)p_ctx->p_buf;

            /* Prepare to pass private key to PKC-Driver */
            privkey_sel.data_type = PKC_DATA_TYPE_KEY_BINARY;
            privkey_sel.p_sel_data = &privkey_data;

            /* No problem with casting to pointer size */
            privkey_sel.p_sel_data->p_data = (uint8_t *)p_ec_ecdh->p_priv_key;

            /* Prepare to pass public key to PKC-Driver */
            pubkey_sel.data_type = PKC_DATA_TYPE_KEY_BINARY;
            pubkey_sel.p_sel_data = &pubkey_data;

            /* No problem with casting to pointer size */
            pubkey_sel.p_sel_data->p_data = (uint8_t *)p_ec_ecdh->p_pub_key;

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_ec_ecdh_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcScalarMultInit(&privkey_sel, &pubkey_sel, p_ctx, p_buf_drv);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EcScalarMultOperate(p_ctx, p_buf_drv, sync_opt);

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
    if (PKCL_RET_OK == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_EC_ECDH_OPERATE_DONE);
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
#else  /* PKCL_CFG_ENABLE_DRV_ECDH != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDH != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdhOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdhFinal(const st_pkc_ec_ctx_t* const p_ctx, st_pkc_data_t* const p_shared)
{
#if (PKCL_CFG_ENABLE_DRV_ECDH != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_ec_ecdh_t * p_ec_ecdh;
    st_pkc_data_t shared_data;
    uint8_t * p_buf_drv;
    uint32_t kdf_ret;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EC_ECDH_OPERATE_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ec_chk_ecdh_final_arg(p_ctx, p_shared);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to ecdh buffer type */
            p_ec_ecdh = (st_pkcl_ec_ecdh_t*)p_ctx->p_buf;

            /* Prepare to pass shared secret to PKC-Driver */
            shared_data.p_data = p_ec_ecdh->shared_data;
            shared_data.len = sizeof(p_ec_ecdh->shared_data);

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_ec_ecdh_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcScalarMultFinal(p_ctx, p_buf_drv, &shared_data);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                if (NULL != p_ctx->p_kdf_info)
                {
                    /* Call KDF API */
                    kdf_ret = p_ctx->p_kdf_info->p_kdf_api(p_ctx->p_kdf_info->p_kdf_ctx, shared_data.p_data,
                                                            p_ctx->p_curve_info->curve_len,
                                                            p_ctx->p_kdf_info->out_kdf_len,
                                                            p_shared->p_data);
                    if (PKCL_EXTERNAL_API_OK == kdf_ret)
                    {
                        /* set shared_key length */
                        p_shared->len = p_ctx->p_kdf_info->out_kdf_len;
                    }
                    else
                    {
                        ret = PKCL_RET_KDFAPIERR;
                    }
                }
                else
                {
                    /* Output shared secret */
                    r_pkcl_cmn_cpy_byte(p_shared->p_data, shared_data.p_data, p_ctx->p_curve_info->curve_len);
                    p_shared->len = p_ctx->p_curve_info->curve_len;
                }

                /* Clear shared secret results stored in p_buf */
                r_pkcl_cmn_set_byte(p_ec_ecdh->shared_data, 0U, sizeof(p_ec_ecdh->shared_data));
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
#else  /* PKCL_CFG_ENABLE_DRV_ECDH != 1U */
    /* Not supported by  PKC-Lib_V.1.20 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDH != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdhFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcKeyGenInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcKeyGenInit(st_pkc_ec_ctx_t* const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkcl_state_t pkcl_state;
    pkcd_ret_t drv_ret;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* Check argument */
        if (NULL != p_ctx)
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
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

            /* Call the PKC-Driver API */
            drv_ret = R_PKCD_EcKeyGenInit(p_ctx, p_buf_drv);

            /* Convert drv_ret to a corresponding PKC-Lib return code */
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
        r_pkcl_cmn_set_state(PKCL_STATE_EC_KEYGEN_INIT_DONE);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_EcKeyGenInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcKeyGenOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcKeyGenOperate(st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EC_KEYGEN_INIT_DONE == pkcl_state)
    {
        /* Check argument */
        if ((NULL != p_ctx) && ((PKC_SYNC_OPT_SYNC == sync_opt) || (PKC_SYNC_OPT_ASYNC == sync_opt)))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
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
            /* Pass the p_buf address to p_buf_drv */
            p_buf_drv = p_ctx->p_buf;

            /* Call the PKC-Driver API */
            drv_ret = R_PKCD_EcKeyGenOperate(p_ctx, p_buf_drv, sync_opt);

            /* Convert drv_ret to a corresponding PKC-Lib return code */
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
        r_pkcl_cmn_set_state(PKCL_STATE_EC_KEYGEN_OPERATE_DONE);
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
* End of function R_PKCL_EcKeyGenOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcKeyGenFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcKeyGenFinal(const st_pkc_ec_ctx_t* const p_ctx, void* const p_priv_key, void* const p_pub_key)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_EC_KEYGEN_OPERATE_DONE == pkcl_state)
    {
        /* Check argument */
        if ((NULL != p_ctx) && (NULL != p_priv_key) && (NULL != p_pub_key))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
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
            /* Pass the p_buf adress to p_buf_drv */
            p_buf_drv = p_ctx->p_buf;

            /* Call the PKC-Driver API */
            drv_ret = R_PKCD_EcKeyGenFinal(p_ctx, p_buf_drv, p_priv_key, p_pub_key);

            /* Convert drv_ret to a corresponding PKC-Lib return code */
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
* End of function R_PKCL_EcKeyGenFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ec_chk_digest
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIsInternal
 * \brief       Check the arguments of the digest message.
 * \param       [in] p_digest          A structure consisting of the byte length len of the message digest and
 *                                      the data pointer p_data.
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
 *      - Make sure the byte length is a multiple of 4.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR.
 *  -# If (NULL != p_digest)
 *    -# If (NULL != p_digest->p_data)
 *      -# If (#PKCL_EC_DIGEST_LEN_MAX >= p_digest->len)
 *        -# Check if digest length p_digest->len is multipila with #r_pkcl_cmn_is_multiple_4,
 *           and set the return value into is_multiple.
 *        -# If PKC_BOOL_FALSE != is_multiple
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
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
static pkcl_ret_t r_pkcl_ec_chk_digest(const st_pkc_data_t* const p_digest)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkc_bool_t is_multiple;

    /* check if the argument is null */
    if (NULL != p_digest)
    {
        if (NULL != p_digest->p_data)
        {
            if (PKCL_EC_DIGEST_LEN_MAX >= p_digest->len)
            {
                /* check the digest length is a multiple of 4 */
                is_multiple = r_pkcl_cmn_is_multiple_4(p_digest->len);
                if (PKC_BOOL_FALSE != is_multiple)
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
* End of function r_pkcl_ec_chk_digest()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ec_chk_curve
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIsInternal
 * \brief       Check the arguments of the EC curve.
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
 *  -# Initialize ret with #PKCL_RET_INTERNALERR.
 *  -# Initialize is_multiple_4 with #PKC_BOOL_FALSE.
 *  -# If (NULL != p_curve)
 *    -# Check that the length of the curve is a multiple of 4 with #r_pkcl_cmn_is_multiple_4,
 *       and set is_multiple_4 to the return value.
 *    -# If ((#PKCL_EC_CURVE_LEN_MIN <= p_curve->curve_len) && (p_curve->curve_len <= #PKCL_EC_CURVE_LEN_MAX) &&
 *          (is_multiple_4 == #PKC_BOOL_TRUE))
 *      -# Set ret to #PKCL_RET_OK.
 *    -# Else
 *      -# Set ret to #PKCL_RET_LENGTHERR.
 *    -# If (#PKCL_RET_OK == ret)
 *      -# If (NULL != p_curve->curve_params.p_data)
 *        -# If ((p_curve->curve_len * #PKCL_EC_CURVE_PARAM_NUM) == p_curve->curve_params.len)
 *          -# Compare curve params field p and 0 using #r_pkcl_cmn_cmp_byte and set the return value to cmp_ret.
 *          -# If (#PKC_BOOL_TRUE != cmp_ret)
 *            -# Compare curve params order n and 0 using #r_pkcl_cmn_cmp_byte and set the return value to cmp_ret.
 *            -# If (#PKC_BOOL_TRUE != cmp_ret)
 *              -# Do nothing.
 *            -# Else
 *              -# Set ret to #PKCL_RET_PARAMERR.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *        -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ec_chk_curve(const st_pkc_ec_curve_t* const p_curve)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkc_bool_t is_multiple_4 = PKC_BOOL_FALSE;
    e_pkc_bool_t cmp_ret;
    const uint8_t cmp_zero[PKCL_EC_CURVE_LEN_MAX] = {0};

    /* check if the argument is null */
    if (NULL != p_curve)
    {
        /* check curve_len */
        is_multiple_4 = r_pkcl_cmn_is_multiple_4(p_curve->curve_len);
        if ((PKCL_EC_CURVE_LEN_MIN <= p_curve->curve_len) && (p_curve->curve_len <= PKCL_EC_CURVE_LEN_MAX) &&
            (PKC_BOOL_TRUE == is_multiple_4))
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
                if ((p_curve->curve_len * PKCL_EC_CURVE_PARAM_NUM) == p_curve->curve_params.len)
                {
                    /* If field p is 0, PKCL_RET_PARAMERR */
                    cmp_ret = r_pkcl_cmn_cmp_byte(&p_curve->curve_params.p_data[0],
                                                    cmp_zero, p_curve->curve_len);
                    if (PKC_BOOL_TRUE != cmp_ret)
                    {
                        /* If order n is 0, PKCL_RET_PARAMERR */
                        cmp_ret = r_pkcl_cmn_cmp_byte(&p_curve->curve_params.p_data[p_curve->curve_len],
                                                        cmp_zero, p_curve->curve_len);
                        if (PKC_BOOL_TRUE != cmp_ret)
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
* End of function r_pkcl_ec_chk_curve()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ec_chk_sign_or_point
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIsInternal
 * \brief       Check the arguments of the Signature data or coordinate point data.
 * \param       [in] p_sign_or_point        Signature data or coordinate point data
 * \param       [in] curve_len              EC curve length
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
 *      - Make sure the bite length is at least twice the curve length.
 * \par Remarks
 *  - The signature data and the coordinate point data have the same length range,
 *    so they are grouped into the same function.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR.
 *  -# If (NULL != p_sign_or_point)
 *    -# If (NULL != p_sign_or_point->p_data)
 *      -# If (#PKCL_EC_DATA_CHK_IN_ARG == arg_type)
 *        -# If (p_sign_or_point->len == (curve_len * #PKCL_EC_DOUBLE_LEN))
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else
 *        -# If (p_sign_or_point->len >= (curve_len * #PKCL_EC_DOUBLE_LEN))
 *          -# Set ret to #PKCL_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ec_chk_sign_or_point(const st_pkc_data_t* const p_sign_or_point, const uint32_t curve_len,
                                                const uint32_t arg_type)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if (NULL != p_sign_or_point)
    {
        if (NULL != p_sign_or_point->p_data)
        {
            /* Input argument or output argument */
            if (PKCL_EC_DATA_CHK_IN_ARG == arg_type)
            {
                if ((curve_len * PKCL_EC_DOUBLE_LEN) == p_sign_or_point->len)
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_LENGTHERR;
                }
            }
            else /* PKCL_EC_DATA_CHK_OUT_ARG == arg_type */
            {
                if ((curve_len * PKCL_EC_DOUBLE_LEN) <= p_sign_or_point->len)
                {
                    ret = PKCL_RET_OK;
                }
                else
                {
                    ret = PKCL_RET_LENGTHERR;
                }
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
* End of function r_pkcl_ec_chk_sign_or_point()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ec_chk_select_data
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIsInternal
 * \brief       Check the argument of type st_pkc_select_data_t.
 * \param       [in]   p_select_data       Signature data or coordinate point data
 * \param       [in]   curve_len           EC curve length
 * \param       [in]   chk_mode            Select whether the data to be checked is scalar data or coordinate points
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
 *      - If chk_mode is #PKCL_EC_SELECT_MODE_SCALAR,
 *        make sure the byte length is less than or equal to the curve length.
 *      - If chk_mode is #PKCL_EC_SELECT_MODE_POINT,
 *        make sure the byte length is the same as twice the curve length.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR.
 *  -# If (NULL != p_select_data)
 *    -# If (((#PKC_DATA_TYPE_BYTE == p_select_data->data_type) or
 *            (#PKC_DATA_TYPE_KEY_BINARY == p_select_data->data_type)) and
 *            (NULL != p_select_data->p_sel_data))
 *      -# If (NULL != p_select_data->p_sel_data->p_data)
 *        -# If (#PKC_DATA_TYPE_BYTE == p_select_data->data_type)
 *          -# If (#PKCL_EC_SELECT_MODE_SCALAR == chk_mode)
 *            -# If (p_select_data->p_sel_data->len <= curve_len)
 *              -# Set ret to #PKCL_RET_OK.
 *            -# Else
 *              -# Set ret to #PKCL_RET_LENGTHERR.
 *          -# Else if (#PKCL_EC_SELECT_MODE_POINT == chk_mode)
 *            -# If ((curve_len * #PKCL_EC_DOUBLE_LEN) == p_select_data->p_sel_data->len)
 *              -# Set ret to #PKCL_RET_OK.
 *            -# Else
 *              -# Set ret to #PKCL_RET_LENGTHERR.
 *          -# Else
 *            -# Set ret to #PKCL_RET_INTERNALERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ec_chk_select_data(const st_pkc_select_data_t* const p_select_data,
                                            const uint32_t curve_len, const uint32_t chk_mode)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the p_select_data members */
    if (NULL != p_select_data)
    {
        if (((PKC_DATA_TYPE_BYTE == p_select_data->data_type) ||
            (PKC_DATA_TYPE_KEY_BINARY == p_select_data->data_type)) &&
            (NULL != p_select_data->p_sel_data))
        {
            /* NULL check p_data */
            if (NULL != p_select_data->p_sel_data->p_data)
            {
                if (PKC_DATA_TYPE_BYTE == p_select_data->data_type)
                {
                    /* scalar or point check mode */
                    if (PKCL_EC_SELECT_MODE_SCALAR == chk_mode)
                    {
                        /* check length */
                        if (p_select_data->p_sel_data->len <= curve_len)
                        {
                            ret = PKCL_RET_OK;
                        }
                        else
                        {
                            ret = PKCL_RET_LENGTHERR;
                        }
                    }
                    else if (PKCL_EC_SELECT_MODE_POINT == chk_mode)
                    {
                        /* check length */
                        if ((curve_len * PKCL_EC_DOUBLE_LEN) == p_select_data->p_sel_data->len)
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
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_ec_chk_select_data()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ec_chk_verify_operate_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EcdsaVerifyOperate.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]     sync_opt       Select synchronous/asynchronous execution
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
 *      - Call #r_pkcl_ec_chk_curve to check the curve information of p_ctx.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and ((#PKC_SYNC_OPT_SYNC == sync_opt) or (#PKC_SYNC_OPT_ASYNC == sync_opt)))
 *    -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_curve_info))
 *      -# Check curve information p_ctx->p_curve_info with #r_pkcl_ec_chk_curve, and set ret to the return value.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ec_chk_verify_operate_arg(const st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null or out of range */
    if ((NULL != p_ctx) && ((PKC_SYNC_OPT_SYNC == sync_opt) || (PKC_SYNC_OPT_ASYNC == sync_opt)))
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
        {
            /* check curve info */
            ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
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
* End of function r_pkcl_ec_chk_verify_operate_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ec_chk_make_sign_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIsInternal
 * \brief       Check the arguments of #R_PKCL_LLEcdsaMakeSign.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]     p_digest       A structure consisting of the byte length len of the message digest and
 *                                       the data pointer p_data.
 * \param       [in]     p_point        A structure consisting of the byte length len of the data that concatenates
 *                                       the x and y coordinates and the data pointer p_data.
 * \param       [in]     p_random       A structure consisting of the byte length len of the random number k and
 *                                       the data pointer p_data.
 * \param       [in]     p_priv_key     ECC private key A structure consisting of the binary byte length len and
 *                                       a pointer to the binary.
 * \param       [in]     p_sign         A structure consisting of the byte length len of the data concatenated
 *                                       with the signature values R and S and the data pointer p_data.
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
 *      - Call #r_pkcl_ec_chk_curve to check the curve information of p_ctx.
 *      - Call #r_pkcl_ec_chk_digest to check the argument p_digest
 *      - Call #r_pkcl_ec_chk_sign_or_point to check the argument p_point.
 *      - Call #r_pkcl_ec_chk_sign_or_point to check the argument p_sign.
 *      - Confirm that the byte length of p_random is less than or equal to the curve length.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_digest) and (NULL != p_point) and
 *         (NULL != p_random) and (NULL != p_priv_key) and (NULL != p_sign))
 *    -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_curve_info))
 *      -# Check curve information p_ctx->p_curve_info with #r_pkcl_ec_chk_curve, and set ret to the return value.
 *      -# If (#PKCL_RET_OK == ret)
 *        -# Check if p_digest with #r_pkcl_ec_chk_digest is valid, and set ret to the return value.
 *        -# If (#PKCL_RET_OK == ret)
 *          -# Check if p_point with #r_pkcl_ec_chk_sign_or_point is valid, and set ret to the return value.
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If (#PKCL_RET_OK == ret)
 *      -# If (NULL != p_random->p_data)
 *        -# If (p_random->len <= (p_ctx->p_curve_info->curve_len))
 *          -# Check if p_sign with #r_pkcl_ec_chk_sign_or_point is valid, and set ret to the return value.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ec_chk_make_sign_arg(const st_pkc_ec_ctx_t* const p_ctx,
                                                const st_pkc_data_t* const p_digest,
                                                const st_pkc_data_t* const p_point,
                                                const st_pkc_data_t* const p_random,
                                                const void* const p_priv_key,
                                                const st_pkc_data_t* const p_sign)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_digest) && (NULL != p_point)
        && (NULL != p_random) && (NULL != p_priv_key) && (NULL != p_sign))
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
        {
            /* check curve info */
            ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
            if (PKCL_RET_OK == ret)
            {
                /* check p_digest */
                ret = r_pkcl_ec_chk_digest(p_digest);
                if (PKCL_RET_OK == ret)
                {
                    /* check p_point */
                    ret = r_pkcl_ec_chk_sign_or_point(p_point, p_ctx->p_curve_info->curve_len,
                                                        PKCL_EC_DATA_CHK_IN_ARG);
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
            /* check p_random */
            if (NULL != p_random->p_data)
            {
                if (p_random->len <= (p_ctx->p_curve_info->curve_len))
                {
                    /* check p_sign */
                    ret = r_pkcl_ec_chk_sign_or_point(p_sign, p_ctx->p_curve_info->curve_len,
                                                        PKCL_EC_DATA_CHK_OUT_ARG);
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
* End of function r_pkcl_ec_chk_make_sign_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ec_chk_calc_u1u2_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIsInternal
 * \brief       Check the arguments of #R_PKCL_LLEcdsaCalcU1U2.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]     p_digest       A structure consisting of the byte length len of the message digest and
 *                                       the data pointer p_data.
 * \param       [in]     p_sign         A structure consisting of the byte length len of the data concatenated
 *                                       with the signature values R and S and the data pointer p_data.
 * \param       [in]     p_u1           Structure consisting of byte length len of intermediate data u1 and
 *                                       data pointer p_data.
 * \param       [in]     p_u2           Structure consisting of byte length len of intermediate data u2 and
 *                                       data pointer p_data.
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
 *      - Call #r_pkcl_ec_chk_curve to check the curve information of p_ctx.
 *      - Call #r_pkcl_ec_chk_digest to check the argument p_digest.
 *      - Call #r_pkcl_ec_chk_sign_or_point to check the argument p_sign.
 *      - Make sure that the byte lengths of p_u1 and p_u2 are greater than or equal to the length of the curve.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_digest) and (NULL != p_sign) and (NULL != p_u1) and (NULL != p_u2))
 *    -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_curve_info))
 *      -# Check curve information p_ctx->p_curve_info with #r_pkcl_ec_chk_curve, and set ret to the return value.
 *      -# If (#PKCL_RET_OK == ret)
 *        -# Check if p_digest is valid with #r_pkcl_ec_chk_digest, and set ret to the return value.
 *        -# If (#PKCL_RET_OK == ret)
 *          -# Check if p_sign is valid with #r_pkcl_ec_chk_sign_or_point, and set ret to the return value.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If (#PKCL_RET_OK == ret)
 *      -# If ((NULL != p_u1->p_data) and (NULL != p_u2->p_data))
 *        -# If ((p_u1->len >= (p_ctx->p_curve_info->curve_len)) and (p_u2->len >= (p_ctx->p_curve_info->curve_len)))
 *          -# Do nothing.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ec_chk_calc_u1u2_arg(const st_pkc_ec_ctx_t* const p_ctx,
                                                const st_pkc_data_t* const p_digest,
                                                const st_pkc_data_t* const p_sign,
                                                const st_pkc_data_t* const p_u1, const st_pkc_data_t* const p_u2)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_digest) && (NULL != p_sign) && (NULL != p_u1) && (NULL != p_u2))
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
        {
            /* check curve info */
            ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
            if (PKCL_RET_OK == ret)
            {
                /* check p_digest */
                ret = r_pkcl_ec_chk_digest(p_digest);
                if (PKCL_RET_OK == ret)
                {
                    /* check p_sign */
                    ret = r_pkcl_ec_chk_sign_or_point(p_sign, p_ctx->p_curve_info->curve_len, PKCL_EC_DATA_CHK_IN_ARG);
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
            /* check p_u1, p_u2 */
            if ((NULL != p_u1->p_data) && (NULL != p_u2->p_data))
            {
                if ((p_u1->len >= (p_ctx->p_curve_info->curve_len)) &&
                    (p_u2->len >= (p_ctx->p_curve_info->curve_len)))
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
* End of function r_pkcl_ec_chk_calc_u1u2_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ec_chk_compare_sign_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIsInternal
 * \brief       Check the arguments of #R_PKCL_LLEcdsaCompareSign.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in] p_u1g          Points derived from u1 * G (x, y) \n
 *                                  A structure consisting of the byte length len of the data that concatenates the x
 *                                   and y coordinates and the data pointer p_data.
 * \param       [in] p_u2q          Points derived from u2 * Q (x, y) \n
 *                                  A structure consisting of the byte length len of the data that concatenates the x
 *                                   and y coordinates and the data pointer p_data.
 * \param       [in] p_sign         A structure consisting of the byte length len of the data concatenated with
 *                                   the signature values R and S and the data pointer p_data.
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
 *      - Call #r_pkcl_ec_chk_curve to check the curve information of p_ctx.
 *      - Call #r_pkcl_ec_chk_digest to check the argument p_digest.
 *      - Call #r_pkcl_ec_chk_sign_or_point to check the argument p_u1g and p_u2q.
 *      - Call #r_pkcl_ec_chk_sign_or_point to check the argument p_sign.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_u1g) and (NULL != p_u2q) and (NULL != p_sign))
 *    -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_curve_info))
 *      -# Check curve information p_ctx->p_curve_info with #r_pkcl_ec_chk_curve, and set ret to the return value.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If (#PKCL_RET_OK == ret)
 *      -# Check if p_u1g is valid with #r_pkcl_ec_chk_sign_or_point, and set ret to the return value.
 *      -# If (#PKCL_RET_OK == ret)
 *        -# Check if p_u2q is valid with #r_pkcl_ec_chk_sign_or_point, and set ret to the return value.
 *        -# If (#PKCL_RET_OK == ret)
 *          -# Check if p_sign is valid with #r_pkcl_ec_chk_sign_or_point, and set ret to the return value.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ec_chk_compare_sign_arg(const st_pkc_ec_ctx_t* const p_ctx,
                                                    const st_pkc_data_t* const p_u1g,
                                                    const st_pkc_data_t* const p_u2q,
                                                    const st_pkc_data_t* const p_sign)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_u1g) && (NULL != p_u2q) && (NULL != p_sign))
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
        {
            /* check curve info */
            ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }

        if (PKCL_RET_OK == ret)
        {
            ret = r_pkcl_ec_chk_sign_or_point(p_u1g, p_ctx->p_curve_info->curve_len, PKCL_EC_DATA_CHK_IN_ARG);
            if (PKCL_RET_OK == ret)
            {
                ret = r_pkcl_ec_chk_sign_or_point(p_u2q, p_ctx->p_curve_info->curve_len, PKCL_EC_DATA_CHK_IN_ARG);
                if (PKCL_RET_OK == ret)
                {
                    ret = r_pkcl_ec_chk_sign_or_point(p_sign, p_ctx->p_curve_info->curve_len, PKCL_EC_DATA_CHK_IN_ARG);
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

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_ec_chk_compare_sign_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ec_chk_ecdh_operate_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EcdhOperate.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]     sync_opt       Select synchronous/asynchronous execution
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
 *      - Call #r_pkcl_ec_chk_curve to check the curve information of p_ctx.
 *      - If p_ctx-> p_kdf_info is not NULL, make sure p_kdf_api is not NULL.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and ((#PKC_SYNC_OPT_SYNC == sync_opt) or (#PKC_SYNC_OPT_ASYNC == sync_opt)))
 *    -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_curve_info))
 *      -# Check curve information p_ctx->p_curve_info with #r_pkcl_ec_chk_curve, and set ret to the return value.
 *        -# If (#PKCL_RET_OK == ret)
 *          -# If (NULL != p_ctx->p_kdf_info)
 *            -# If (NULL != p_ctx->p_kdf_info->p_kdf_api)
 *              -# Do nothing.
 *            -# Else
 *              -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ec_chk_ecdh_operate_arg(const st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null or out of range */
    if ((NULL != p_ctx) && ((PKC_SYNC_OPT_SYNC == sync_opt) || (PKC_SYNC_OPT_ASYNC == sync_opt)))
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
        {
            /* check curve info */
            ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
            if (PKCL_RET_OK == ret)
            {
                /* check kdf info */
                if (NULL != p_ctx->p_kdf_info)
                {
                    if (NULL != p_ctx->p_kdf_info->p_kdf_api)
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
* End of function r_pkcl_ec_chk_ecdh_operate_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ec_chk_ecdh_final_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EcdhFinal.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]     p_shared       A structure consisting of a shared secret or common key byte length len
 *                                       and a data pointer p_data.
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
 *      - Call #r_pkcl_ec_chk_curve to check the curve information of p_ctx.
 *      - If p_ctx->p_kdf_info is not NULL, make sure p_kdf_api is not NULL.
 *      - If p_ctx->p_kdf_info is NULL, make sure the length of p_shared is greater than or equal to the curve length.
 *      - If p_ctx->p_kdf_info is not NULL, make sure the length of p_shared is greater than or equal to out_kdf_len.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_shared))
 *    -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_curve_info))
 *      -# Check curve information p_ctx->p_curve_info with #r_pkcl_ec_chk_curve, and set ret to the return value.
 *      -# If (#PKCL_RET_OK == ret)
 *        -# If (NULL != p_ctx->p_kdf_info)
 *          -# If (NULL != p_ctx->p_kdf_info->p_kdf_api)
 *            -# Do nothing.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If (#PKCL_RET_OK == ret)
 *      -# If (NULL != p_shared->p_data)
 *        -# If (NULL != p_ctx->p_kdf_info)
 *          -# If (p_shared->len >= p_ctx->p_kdf_info->out_kdf_len)
 *            -# Do nothing.
 *          -# Else
 *            -# Set ret to #PKCL_RET_LENGTHERR.
 *        -# Else
 *          -# If (p_shared->len >= p_ctx->p_curve_info->curve_len)
 *            -# Do nothing.
 *          -# Else
 *            -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ec_chk_ecdh_final_arg(const st_pkc_ec_ctx_t* const p_ctx, const st_pkc_data_t* const p_shared)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_shared))
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
        {
            /* check curve info */
            ret = r_pkcl_ec_chk_curve(p_ctx->p_curve_info);
            if (PKCL_RET_OK == ret)
            {
                /* check kdf info */
                if (NULL != p_ctx->p_kdf_info)
                {
                    if (NULL != p_ctx->p_kdf_info->p_kdf_api)
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
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCL_RET_PARAMERR;
        }

        if (PKCL_RET_OK == ret)
        {
            /* check p_shared */
            if (NULL != p_shared->p_data)
            {
                /* if shared secret */
                if (NULL != p_ctx->p_kdf_info)
                {
                    if (p_shared->len >= p_ctx->p_kdf_info->out_kdf_len)
                    {
                        /* Do nothing */
                    }
                    else
                    {
                        ret = PKCL_RET_LENGTHERR;
                    }
                }
                /* if shared key */
                else
                {
                    if (p_shared->len >= p_ctx->p_curve_info->curve_len)
                    {
                        /* Do nothing */
                    }
                    else
                    {
                        ret = PKCL_RET_LENGTHERR;
                    }
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
* End of function r_pkcl_ec_chk_ecdh_final_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ec_verified_gen_sign
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCLECAPIsInternal
 * \brief       Verify a signature generated by #R_PKCL_EcdsaSignInit ~ #R_PKCL_EcdsaSignFinal.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by ECDSA.
 * \param       [in]     p_buf          Buffer used for internal processing of PKC-Lib.
 * \param       [in]     p_priv_key     EC private key of key binary format.
 * \param       [in]     p_digest_data  An address of a message digest.
 * \param       [in]     p_sign_data    An address of a signature to be verified in this function.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdsaCalcU1U2
 *  - R_PKCD_EcScalarMultInit
 *  - R_PKCD_EcScalarMultOperate
 *  - R_PKCD_EcScalarMultFinal
 *  - R_PKCD_EcdsaCompareSign
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Verify a signature generated by #R_PKCL_EcdsaSignInit ~ #R_PKCL_EcdsaSignFinal.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# Calculate u1, u2 with R_PKCD_EcdsaCalcU1U2, and set drv_ret to the return value.
 *  -# If (PKCD_RET_OK == drv_ret)
 *    -# Prepare the scalar multiplication with R_PKCD_EcScalarMultInit, and set drv_ret to the return value.
 *    -# If (PKCD_RET_OK == drv_ret)
 *      -# Calculate  u1 * G (G represents the base point) with R_PKCD_EcScalarMultOperate,
 *         and set drv_ret to the return value.
 *      -# If (PKCD_RET_OK == drv_ret)
 *        -# Get the resulting u1 * G with R_PKCD_EcScalarMultFinal, and set ret to the return value.
 *      -# Else
 *        -# Do nothing.
 *    -# Else
 *      -# Do nothing.
 *  -# Else
 *    -# Do nothing.
 *  -# If (PKCD_RET_OK == drv_ret)
 *    -# Prepare the scalar multiplication with R_PKCD_EcScalarMultInit, and set ret to the return value.
 *    -# If (PKCD_RET_OK == drv_ret)
 *      -# Calculate  u2 * Q (Q represents the public key) with R_PKCD_EcScalarMultOperate,
 *         and set ret to the return value.
 *      -# If (PKCD_RET_OK == drv_ret)
 *        -# Get the resulting u2 * Q with R_PKCD_EcScalarMultFinal.
 *        -# If (PKCD_RET_OK == drv_ret)
 *          -# Verify the signature with R_PKCD_EcdsaCompareSign and set drv_ret to the result.
 *          -# Convert drv_ret to the cooresponding PKC-Lib return value with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret.
 *        -# Else
 *          -# Do nothing.
 *      -# Else
 *        -# Do nothing.
 *    -# Else
 *      -# Do nothing.
 *  -# If (#PKCL_RET_OK != ret)
 *    -# Set ret to #PKCL_RET_TAMPERERR.
 *  -# Else
 *    -# Do nothing.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ec_verified_gen_sign(const st_pkc_ec_ctx_t* const p_ctx, uint8_t* const p_buf,
                                            const void * const p_priv_key,
                                            const st_pkc_data_t* const p_digest_data,
                                            const st_pkc_data_t* const p_sign_data)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    st_pkc_data_t pubkey_data;
    st_pkc_data_t base_point_data;
    st_pkc_data_t u1_data;
    st_pkc_data_t u2_data;
    st_pkc_data_t u1g_data;
    st_pkc_data_t u2q_data;
    st_pkc_select_data_t pubkey_sel;
    st_pkc_select_data_t base_point_sel;
    st_pkc_select_data_t u1_sel;
    st_pkc_select_data_t u2_sel;
    uint8_t * p_buf_drv;
    st_pkc_ec_ctx_t ctx_temp;
    st_pkcl_ec_verified_sign_t * const p_ec_verify = (st_pkcl_ec_verified_sign_t *)p_buf; /* No problem with casting to
                                                                                        pointer size */

    ctx_temp                = *p_ctx;
    ctx_temp.sec_scalar_mul = PKC_BOOL_FALSE;

    /* Clear the buffer */
    r_pkcl_cmn_set_byte(p_buf, 0U, sizeof(st_pkcl_ec_verified_sign_t));

    /* Set the buffer for PKC-Driver APIs */
    p_buf_drv = &p_buf[sizeof(st_pkcl_ec_verified_sign_t)];

    /* Prepare buffers for u1, u2 */
    u1_data.p_data      = p_ec_verify->u1;
    u1_data.len         = sizeof(p_ec_verify->u1);
    u1_sel.data_type    = PKC_DATA_TYPE_BYTE;
    u1_sel.p_sel_data   = &u1_data;
    u2_data.p_data      = p_ec_verify->u2;
    u2_data.len         = sizeof(p_ec_verify->u2);
    u2_sel.data_type    = PKC_DATA_TYPE_BYTE;
    u2_sel.p_sel_data   = &u2_data;

    /* Calculate U1, U2 */
    drv_ret = R_PKCD_EcdsaCalcU1U2(&ctx_temp, p_buf_drv, p_digest_data, p_sign_data, &u1_data, &u2_data);

    if (PKCD_RET_OK == drv_ret)
    {
        /* Prepare to pass curve base point to PKC-Driver */
        r_pkcl_cmn_cpy_byte(p_ec_verify->base_point,
                            &p_ctx->p_curve_info->curve_params.p_data[p_ctx->p_curve_info->curve_len *
                                                                        PKCL_EC_CURVE_OFFSET_GX],
                            p_ctx->p_curve_info->curve_len * PKCL_EC_DOUBLE_LEN);
        base_point_data.p_data      = p_ec_verify->base_point;
        base_point_data.len         = p_ctx->p_curve_info->curve_len * PKCL_EC_DOUBLE_LEN;
        base_point_sel.data_type    = PKC_DATA_TYPE_BYTE;
        base_point_sel.p_sel_data   = &base_point_data;

        /* Calculate U1 * G */
        drv_ret = R_PKCD_EcScalarMultInit(&u1_sel, &base_point_sel, &ctx_temp, p_buf_drv);

        if (PKCD_RET_OK == drv_ret)
        {
            drv_ret = R_PKCD_EcScalarMultOperate(&ctx_temp, p_buf_drv, PKC_SYNC_OPT_SYNC);

            if (PKCD_RET_OK == drv_ret)
            {
                /* Prepare buffers for u1*G */
                u1g_data.p_data = p_ec_verify->u1g;
                u1g_data.len    = sizeof(p_ec_verify->u1g);

                drv_ret = R_PKCD_EcScalarMultFinal(&ctx_temp, p_buf_drv, &u1g_data);
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
        /* Prepare to pass the private key including the public key to PKC-Driver */
        pubkey_sel.data_type    = PKC_DATA_TYPE_KEY_BINARY;
        pubkey_sel.p_sel_data   = &pubkey_data;

        pubkey_data.p_data = (uint8_t *)p_priv_key;  /* No problem with casting to pointer size */

        /* Calculate U2 * Q (Q: public key) */
        drv_ret = R_PKCD_EcScalarMultInit(&u2_sel, &pubkey_sel, &ctx_temp, p_buf_drv);

        if (PKCD_RET_OK == drv_ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcScalarMultOperate(&ctx_temp, p_buf_drv, PKC_SYNC_OPT_SYNC);

            if (PKCD_RET_OK == drv_ret)
            {
                /* Prepare buffers for u2*Q (Q: public key) */
                u2q_data.p_data = p_ec_verify->u2q;
                u2q_data.len    = sizeof(p_ec_verify->u2q);

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EcScalarMultFinal(&ctx_temp, p_buf_drv, &u2q_data);

                if (PKCD_RET_OK == drv_ret)
                {
                    /* Call PKC-Driver API */
                    drv_ret = R_PKCD_EcdsaCompareSign(p_ctx, p_buf_drv, &u1g_data, &u2q_data, p_sign_data);

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
* End of function r_pkcl_ec_verified_gen_sign()
**********************************************************************************************************************/
