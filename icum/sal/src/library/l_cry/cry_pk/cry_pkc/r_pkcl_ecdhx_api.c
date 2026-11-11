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
/* PRQA S 3383,3384 ++ # D012 Have confirmed in advance that the value doesn't wrap around. */
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
 * \addtogroup    PKCLECDHX
 * \{
 * \addtogroup    PKCLECDHXDefines             ECDHX Definitions
 * \addtogroup    PKCLECDHXTypes               ECDHX Types
 * \addtogroup    PKCLECDHXConstants           ECDHX Constants
 * \addtogroup    PKCLECDHXAPIs                ECDHX APIs
 * \addtogroup    PKCLECDHXDefinesInternal     ECDHX Internal Definitions
 * \addtogroup    PKCLECDHXTypesInternal       ECDHX Internal Types
 * \addtogroup    PKCLECDHXVariablesInternal   ECDHX Internal Variables
 * \addtogroup    PKCLECDHXAPIsInternal        ECDHX Internal APIs
 * \}******************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCLECDHXDefinesInternal
 * \{******************************************************************/
#define PKCL_ECDHX_CURVE_LEN_CURVE25519 (32U)   /**< Curve length Ed25519 */
#define PKCL_ECDHX_CURVE_LEN_CURVE448   (56U)   /**< Curve length Ed448 */
#define PKCL_ECDHX_CURVE_LEN_MAX        (PKCL_ECDHX_CURVE_LEN_CURVE448)   /**< Maximum length of curve length */

#define PKCL_ECDHX_CURVE_OFFSET_GU      (2U)    /**< Start address offset of Base point u coordinate data of curve */
#define PKCL_ECDHX_CURVE_PARAM_NUM      (3U)    /**< Number of ECDHX curve parameters */

/*! \}*/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCLECDHXTypesInternal
 * \{******************************************************************/
/** A save parameter structure of ecdhx */
typedef struct
{
    const void * p_priv_key; /**< A private key of key binary format */
    const void * p_pub_key;  /**< A public key of key binary format */
} st_pkcl_ecdhx_t;

/** A save parameter structure of ecdhx key generation */
typedef struct
{
    uint32_t priv_scalar_len;                             /**< A length of private scalar */
    uint8_t  priv_scalar_data[PKCL_ECDHX_CURVE_LEN_MAX];  /**< A private scalar */
    uint32_t comp_point_len;                              /**< A length of compress point */
    uint8_t  comp_point[PKCL_ECDHX_CURVE_LEN_MAX];        /**< A compress point */
} st_pkcl_ecdhx_keygen_t;

/*! \}*/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ecdhx_chk_curve (const st_pkc_ec_curve_t * const p_curve);
static pkcl_ret_t r_pkcl_ecdhx_chk_ecdhx_init_arg (const void * const p_priv_key,
                                                    const void * const p_pub_key,
                                                    const st_pkc_ecdhx_ctx_t * const p_ctx);
static pkcl_ret_t r_pkcl_ecdhx_chk_ecdhx_operate_arg (const st_pkc_ecdhx_ctx_t * const p_ctx,
                                                        const uint32_t processing_bits);
static pkcl_ret_t r_pkcl_ecdhx_chk_ecdhx_final_arg (const st_pkc_ecdhx_ctx_t * const p_ctx,
                                                    const st_pkc_data_t * const p_shared);
static pkcl_ret_t r_pkcl_ecdhx_chk_keygen_operate_arg (const st_pkc_ecdhx_ctx_t * const p_ctx,
                                                        const uint32_t processing_bits);
static pkcl_ret_t r_pkcl_ecdhx_chk_keygen_final_arg (const st_pkc_ecdhx_ctx_t * const p_ctx,
                                                        const void * const p_priv_key,
                                                        const void * const p_pub_key);
static void r_pkcl_ecdhx_encode_data (uint8_t * const p_data, const uint32_t len);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhXInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdhXInit(const void* const p_priv_key, const void* const p_pub_key,
                            st_pkc_ecdhx_ctx_t* const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_ECDHX != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkcl_state_t pkcl_state;
    st_pkcl_ecdhx_t * p_ecdhx;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ecdhx_chk_ecdhx_init_arg(p_priv_key, p_pub_key, p_ctx);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to ecdh buffer type */
            p_ecdhx = (st_pkcl_ecdhx_t*)p_ctx->p_buf;

            /* Save p_priv_key */
            p_ecdhx->p_priv_key = p_priv_key;

            /* Save p_pub_key */
            p_ecdhx->p_pub_key = p_pub_key;

            /* State transition */
            r_pkcl_cmn_set_state(PKCL_STATE_ECDHX_INIT_DONE);
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
#else  /* PKCL_CFG_ENABLE_DRV_ECDHX != 1U */
    /* Not supported by  PKC-Lib_V.1.10 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDHX != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdhXInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhXOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdhXOperate(st_pkc_ecdhx_ctx_t* const p_ctx,  const uint32_t processing_bits)
{
#if (PKCL_CFG_ENABLE_DRV_ECDHX != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    const st_pkcl_ecdhx_t * p_ecdhx;
    st_pkc_data_t privkey_data;
    st_pkc_data_t pubkey_data;
    st_pkc_select_data_t privkey_sel;
    st_pkc_select_data_t pubkey_sel;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_ECDHX_INIT_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ecdhx_chk_ecdhx_operate_arg(p_ctx, processing_bits);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to ecdh buffer type */
            p_ecdhx = (const st_pkcl_ecdhx_t*)p_ctx->p_buf;

            /* Prepare to pass private key to PKC-Driver */
            privkey_sel.data_type = PKC_DATA_TYPE_KEY_BINARY;
            privkey_sel.p_sel_data = &privkey_data;
            privkey_sel.p_sel_data->p_data =
                (uint8_t*)p_ecdhx->p_priv_key;    /* No problem with casting to pointer size */

            /* Prepare to pass public key to PKC-Driver */
            pubkey_sel.data_type = PKC_DATA_TYPE_KEY_BINARY;
            pubkey_sel.p_sel_data = &pubkey_data;
            pubkey_sel.p_sel_data->p_data =
                (uint8_t*)p_ecdhx->p_pub_key;     /* No problem with casting to pointer size */

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_ecdhx_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcdhXScalarMultInit(&privkey_sel, &pubkey_sel, p_ctx, p_buf_drv);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EcdhXScalarMultOperate(p_ctx, p_buf_drv, processing_bits);

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
    else if (PKCL_STATE_ECDHX_OPERATE_PROCESSING == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ecdhx_chk_ecdhx_operate_arg(p_ctx, processing_bits);
        if (PKCL_RET_OK == ret)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_ecdhx_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcdhXScalarMultOperate(p_ctx, p_buf_drv, processing_bits);

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
        r_pkcl_cmn_set_state(PKCL_STATE_ECDHX_OPERATE_DONE);
    }
    else if (PKCL_RET_PROCESSING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_ECDHX_OPERATE_PROCESSING);
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
#else  /* PKCL_CFG_ENABLE_DRV_ECDHX != 1U */
    /* Not supported by  PKC-Lib_V.1.10 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDHX != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdhXOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhXFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdhXFinal(const st_pkc_ecdhx_ctx_t* const p_ctx, st_pkc_data_t* const p_shared)
{
#if (PKCL_CFG_ENABLE_DRV_ECDHX != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    uint8_t * p_buf_drv;
    uint32_t kdf_ret;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_ECDHX_OPERATE_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ecdhx_chk_ecdhx_final_arg(p_ctx, p_shared);
        if (PKCL_RET_OK == ret)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_ecdhx_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcdhXScalarMultFinal(p_ctx, p_buf_drv, p_shared);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if ((PKCL_RET_OK == ret) && (NULL != p_ctx->p_kdf_info))
            {
                /* Call KDF API */
                kdf_ret = p_ctx->p_kdf_info->p_kdf_api(p_ctx->p_kdf_info->p_kdf_ctx, p_shared->p_data,
                                                        p_shared->len, p_ctx->p_kdf_info->out_kdf_len,
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
#else  /* PKCL_CFG_ENABLE_DRV_ECDHX != 1U */
    /* Not supported by  PKC-Lib_V.1.10 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDHX != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdhXFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhXKeyGenInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdhXKeyGenInit(st_pkc_ecdhx_ctx_t* const p_ctx)
{
#if (PKCL_CFG_ENABLE_DRV_ECDHX_KEYGEN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkcl_state_t pkcl_state;
    st_pkcl_ecdhx_keygen_t * p_ecdhx_keygen;
    uint32_t external_ret;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* check if the argument is null */
        if (NULL != p_ctx)
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_ecdhx_chk_curve(p_ctx->p_curve_info);
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
            /* Cast p_buf to ECDHX keygen buffer type */
            p_ecdhx_keygen = (st_pkcl_ecdhx_keygen_t*)p_ctx->p_buf;

            /* buffer clear */
            r_pkcl_cmn_set_byte(p_ecdhx_keygen->priv_scalar_data, 0U, sizeof(p_ecdhx_keygen->priv_scalar_data));

            /* generation private key value */
            p_ecdhx_keygen->priv_scalar_len = p_ctx->p_curve_info->curve_len;
            external_ret = p_ctx->p_drbg_api(p_ecdhx_keygen->priv_scalar_len, p_ecdhx_keygen->priv_scalar_data);

            if (PKCL_EXTERNAL_API_OK == external_ret)
            {
                /* Do nothing */
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
        r_pkcl_cmn_set_state(PKCL_STATE_ECDHX_KEYGEN_INIT_DONE);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
#else  /* PKCL_CFG_ENABLE_DRV_ECDHX_KEYGEN != 1U */
    /* Not supported by  PKC-Lib_V.1.10 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDHX_KEYGEN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdhXKeyGenInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhXKeyGenOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdhXKeyGenOperate(st_pkc_ecdhx_ctx_t* const p_ctx, const uint32_t processing_bits)
{
#if (PKCL_CFG_ENABLE_DRV_ECDHX_KEYGEN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_ecdhx_keygen_t * p_ecdhx_keygen;
    st_pkc_data_t scalar_data;
    st_pkc_data_t comp_point_data;
    st_pkc_select_data_t scalar_sel;
    st_pkc_select_data_t comp_point_sel;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_ECDHX_KEYGEN_INIT_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ecdhx_chk_keygen_operate_arg(p_ctx, processing_bits);

        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to ECDHX keygen buffer type */
            p_ecdhx_keygen = (st_pkcl_ecdhx_keygen_t*)p_ctx->p_buf;

            /* Prepare to pass sacalar to PKC-Driver */
            scalar_data.p_data = p_ecdhx_keygen->priv_scalar_data;
            scalar_data.len = p_ecdhx_keygen->priv_scalar_len;

            scalar_sel.data_type = PKC_DATA_TYPE_BYTE;
            scalar_sel.p_sel_data = &scalar_data;

            /* Prepare to pass sacalar to PKC-Driver */
            r_pkcl_cmn_cpy_byte(p_ecdhx_keygen->comp_point,
                                &p_ctx->p_curve_info->curve_params.p_data[p_ctx->p_curve_info->curve_len *
                                                                                        PKCL_ECDHX_CURVE_OFFSET_GU],
                                p_ctx->p_curve_info->curve_len);
            p_ecdhx_keygen->comp_point_len = p_ctx->p_curve_info->curve_len;
            r_pkcl_ecdhx_encode_data(p_ecdhx_keygen->comp_point, p_ecdhx_keygen->comp_point_len);
            comp_point_data.p_data = p_ecdhx_keygen->comp_point;
            comp_point_data.len = p_ecdhx_keygen->comp_point_len;

            comp_point_sel.data_type = PKC_DATA_TYPE_BYTE;
            comp_point_sel.p_sel_data = &comp_point_data;

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_ecdhx_keygen_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcdhXScalarMultInit(&scalar_sel, &comp_point_sel, p_ctx, p_buf_drv);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EcdhXScalarMultOperate(p_ctx, p_buf_drv, processing_bits);

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
    else if (PKCL_STATE_ECDHX_KEYGEN_OPERATE_PROCESSING == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ecdhx_chk_keygen_operate_arg(p_ctx, processing_bits);

        if (PKCL_RET_OK == ret)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_ecdhx_keygen_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcdhXScalarMultOperate(p_ctx, p_buf_drv, processing_bits);

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
        r_pkcl_cmn_set_state(PKCL_STATE_ECDHX_KEYGEN_OPERATE_DONE);
    }
    else if (PKCL_RET_PROCESSING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_ECDHX_KEYGEN_OPERATE_PROCESSING);
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
#else  /* PKCL_CFG_ENABLE_DRV_ECDHX_KEYGEN != 1U */
    /* Not supported by  PKC-Lib_V.1.10 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDHX_KEYGEN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdhXKeyGenOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhXKeyGenFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_EcdhXKeyGenFinal(const st_pkc_ecdhx_ctx_t* const p_ctx,
                                    void* const p_priv_key, void* const p_pub_key)
{
#if (PKCL_CFG_ENABLE_DRV_ECDHX_KEYGEN != 1U)
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_ecdhx_keygen_t * p_ecdhx_keygen;
    st_pkc_data_t priv_scalar_data;
    st_pkc_data_t pubkey_data;
    uint8_t pub_key_val[PKCL_ECDHX_CURVE_LEN_MAX];
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_ECDHX_KEYGEN_OPERATE_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_ecdhx_chk_keygen_final_arg(p_ctx, p_priv_key, p_pub_key);

        if (PKCL_RET_OK == ret)
        {
            /* Prepare to pass SB to PKC-Driver */
            pubkey_data.p_data = pub_key_val;
            pubkey_data.len = sizeof(pub_key_val);

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_ecdhx_keygen_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_EcdhXScalarMultFinal(p_ctx, p_buf_drv, &pubkey_data);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

            if (PKCL_RET_OK == ret)
            {
                /* Cast p_buf to ECDHX keygen buffer type */
                p_ecdhx_keygen = (st_pkcl_ecdhx_keygen_t*)p_ctx->p_buf;

                /* Prepare to pass public key to PKC-Driver */
                priv_scalar_data.p_data = p_ecdhx_keygen->priv_scalar_data;
                priv_scalar_data.len = p_ecdhx_keygen->priv_scalar_len;

                /* Call PKC-Driver API */
                drv_ret = R_PKCD_EcdhXKeyWrap(p_ctx, p_buf_drv, &priv_scalar_data,
                                                &pubkey_data, p_priv_key, p_pub_key);

                /* Convert to PKC-Lib return code */
                ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

                /* Clear the private key value stored in p_buf */
                r_pkcl_cmn_set_byte(p_ecdhx_keygen->priv_scalar_data, 0U, sizeof(p_ecdhx_keygen->priv_scalar_data));
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
#else  /* PKCL_CFG_ENABLE_DRV_ECDHX_KEYGEN != 1U */
    /* Not supported by  PKC-Lib_V.1.10 */
#endif /* PKCL_CFG_ENABLE_DRV_ECDHX_KEYGEN != 1U */
}
/**********************************************************************************************************************
* End of function R_PKCL_EcdhXKeyGenFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ecdhx_chk_curve
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCLECDHXAPIsInternal
 * \brief       Check the arguments of the ECDHX curve.
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
 *    -# If ((#PKCL_ECDHX_CURVE_LEN_CURVE25519 == p_curve->curve_len) or
 *           (#PKCL_ECDHX_CURVE_LEN_CURVE448 == p_curve->curve_len))
 *      -# Set ret to #PKCL_RET_OK.
 *    -# Else
 *      -# Set ret to #PKCL_RET_LENGTHERR.
 *    -# If (#PKCL_RET_OK == ret)
 *      -# If (NULL != p_curve->curve_params.p_data)
 *        -# If ((p_curve->curve_len * #PKCL_ECDHX_CURVE_PARAM_NUM) == p_curve->curve_params.len)
 *          -# Do nothing.
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
static pkcl_ret_t r_pkcl_ecdhx_chk_curve(const st_pkc_ec_curve_t* const p_curve)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if (NULL != p_curve)
    {
        /* check curve_len */
        if ((PKCL_ECDHX_CURVE_LEN_CURVE25519 == p_curve->curve_len) ||
            (PKCL_ECDHX_CURVE_LEN_CURVE448 == p_curve->curve_len))
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
                if ((p_curve->curve_len * PKCL_ECDHX_CURVE_PARAM_NUM) == p_curve->curve_params.len)
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
* End of function r_pkcl_ecdhx_chk_curve()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ecdhx_chk_ecdhx_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCLECDHXAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EcdhXInit.
 * \param       [in]     p_priv_key    A pointer to the ECDHX private key in key binary format.
 * \param       [in]     p_pub_key     Key A pointer to the ECDHX public key in binary format.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by ECDHX.
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
 *      - Call #r_pkcl_ecdhx_chk_curve to check the curve information of p_ctx.
 *      - If p_ctx-> p_kdf_info is not NULL, make sure p_kdf_api is not NULL.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_priv_key) and (NULL != p_pub_key) and (NULL != p_ctx))
 *    -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_curve_info))
 *      -# Check curve information p_ctx->p_curve_info with #r_pkcl_ecdhx_chk_curve,
 *         and set ret to the return value.
 *      -# If (#PKCL_RET_OK == ret)
 *        -# If (NULL != p_ctx->p_kdf_info)
 *          -# If (NULL != p_ctx->p_kdf_info->p_kdf_api)
 *            -# Do nothing.
 *          -# Else
 *            -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ecdhx_chk_ecdhx_init_arg(const void* const p_priv_key,
                                                    const void* const p_pub_key,
                                                    const st_pkc_ecdhx_ctx_t* const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_priv_key) && (NULL != p_pub_key) && (NULL != p_ctx))
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
        {
            /* check curve info */
            ret = r_pkcl_ecdhx_chk_curve(p_ctx->p_curve_info);
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
* End of function r_pkcl_ecdhx_chk_ecdhx_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ecdhx_chk_ecdhx_operate_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCLECDHXAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EcdhXOperate.
 * \param       [in] p_ctx              A pointer to a structure that summarizes the parameters required by ECDHX.
 * \param       [in] processing_bits    Number of bits processed at one time when the operation is executed
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
 *      - Call #r_pkcl_ecdhx_chk_curve to check the curve information of p_ctx.
 *      - If p_ctx-> p_kdf_info is not NULL, make sure p_kdf_api is not NULL.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If (NULL != p_ctx)
 *    -# If ((0U < processing_bits) and (#PKCL_PROCESSING_BITS_MAX >= processing_bits))
 *      -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_curve_info))
 *        -# Check curve information p_ctx->p_curve_info with #r_pkcl_ecdhx_chk_curve,
 *           and set ret to the return value.
 *        -# If (#PKCL_RET_OK == ret)
 *          -# If (NULL != p_ctx->p_kdf_info)
 *            -# If (NULL != p_ctx->p_kdf_info->p_kdf_api)
 *              -# Do nothing.
 *            -# Else
 *              -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_LENGTHERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ecdhx_chk_ecdhx_operate_arg(const st_pkc_ecdhx_ctx_t* const p_ctx,
                                                        const uint32_t processing_bits)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null or out of range */
    if ((NULL != p_ctx))
    {
        /* check processing_bits */
        if ((0U < processing_bits) && (PKCL_PROCESSING_BITS_MAX >= processing_bits))
        {
            /* check context */
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_ecdhx_chk_curve(p_ctx->p_curve_info);
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
* End of function r_pkcl_ecdhx_chk_ecdhx_operate_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ecdhx_chk_ecdhx_final_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCLECDHXAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EcdhXFinal.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by ECDHX.
 * \param       [in,out] p_shared       A structure consisting of a shared secret or common key byte length len
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
 *      - Call #r_pkcl_ecdhx_chk_curve to check the curve information of p_ctx.
 *      - If p_ctx->p_kdf_info is not NULL, make sure p_kdf_api is not NULL.
 *      - If p_ctx->p_kdf_info is NULL, make sure the length of p_shared is greater than or equal to the curve length.
 *      - If p_ctx->p_kdf_info is not NULL, make sure the length of p_shared is greater than or equal to out_kdf_len.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_shared))
 *    -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_curve_info))
 *      -# Check curve information p_ctx->p_curve_info with #r_pkcl_ecdhx_chk_curve,
 *         and set ret to the return value.
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
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ecdhx_chk_ecdhx_final_arg(const st_pkc_ecdhx_ctx_t* const p_ctx,
                                                    const st_pkc_data_t* const p_shared)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_shared))
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info))
        {
            /* check curve info */
            ret = r_pkcl_ecdhx_chk_curve(p_ctx->p_curve_info);
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
                /* if shared key */
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
                /* if shared secret */
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
* End of function r_pkcl_ecdhx_chk_ecdhx_final_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ecdhx_chk_keygen_operate_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCLECDHXAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EcdhXKeyGenOperate.
 * \param       [in] p_ctx              A pointer to a structure that summarizes the parameters required by ECDHX.
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
 *      - Call #r_pkcl_ec_chk_digest to check the argument p_digest
 *      - Call #r_pkcl_ecdhx_chk_curve to check the curve information of p_ctx.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If (NULL != p_ctx)
 *    -# If ((0U < processing_bits) and (#PKCL_PROCESSING_BITS_MAX >= processing_bits))
 *      -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_drbg_api) and (NULL != p_ctx->p_curve_info))
 *        -# Check curve information p_ctx->p_curve_info with #r_pkcl_ecdhx_chk_curve,
 *           and set ret to the return value.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_LENGTHERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ecdhx_chk_keygen_operate_arg(const st_pkc_ecdhx_ctx_t* const p_ctx,
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
            if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info))
            {
                /* check curve info */
                ret = r_pkcl_ecdhx_chk_curve(p_ctx->p_curve_info);
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
* End of function r_pkcl_ecdhx_chk_keygen_operate_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ecdhx_chk_keygen_final_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCLECDHXAPIsInternal
 * \brief       Check the arguments of #R_PKCL_EcdhXKeyGenFinal.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by ECDHX.
 * \param       [in]     p_priv_key     A pointer to a memory area allocated by the user SW to store the key binary
 *                                       of the ECDHX private key.
 * \param       [in]     p_pub_key      A pointer to a memory area allocated by the user SW to store the key binary
 *                                       of the ECDHX public key.
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
 *      - Call #r_pkcl_ec_chk_digest to check the argument p_digest
 *      - Call #r_pkcl_ecdhx_chk_curve to check the curve information of p_ctx.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_priv_key) and (NULL != p_pub_key))
 *    -# If ((NULL != p_ctx->p_buf) and (NULL != p_ctx->p_drbg_api) and (NULL != p_ctx->p_curve_info))
 *      -# Check curve information p_ctx->p_curve_info with #r_pkcl_ecdhx_chk_curve,
 *         and set ret to the return value.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_ecdhx_chk_keygen_final_arg(const st_pkc_ecdhx_ctx_t* const p_ctx,
                                                    const void* const p_priv_key,
                                                    const void* const p_pub_key)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_priv_key) && (NULL != p_pub_key))
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info))
        {
            /* check curve info */
            ret = r_pkcl_ecdhx_chk_curve(p_ctx->p_curve_info);
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
* End of function r_pkcl_ecdhx_chk_keygen_final_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_ecdhx_encode_data
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECDHXAPIsInternal
 * \brief       Reverse the endianness.
 * \param       [in,out] p_data           A top address of input data to be encoded.
 *                                        At the time of output, encoded data is set.
 * \param       [in]  len                 Byte size of data to be encoded
 * \return
 *  - None
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Reverse the byte order of an input byte array.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Acquire a buffer tmp_data for temporary data.
 *  -# Loop while i is less than len. <br>
 *     Increment i for each loop.
 *    -# Set p_data[(len - 1U) - i] into temp_data[i].
 *  -# If (0U != len)
 *    -# Get encoded data from tmp_data, and set the data and its length to p_data and len respectively
 *       with #r_pkcl_cmn_cpy_byte.
 *
 * \callgraph
 *********************************************************************************************************************/
static void r_pkcl_ecdhx_encode_data(uint8_t* const p_data, const uint32_t len)
{
    uint32_t i;
    uint8_t tmp_data[PKCL_ECDHX_CURVE_LEN_MAX];

    for (i = 0U; i < len; i++ )
    {
        tmp_data[i] = p_data[(len - 1U) - i];
    }
    if (0U != len)
    {
        r_pkcl_cmn_cpy_byte(p_data, tmp_data, len);
    }
    else
    {
        /* Do nothing */
    }
}
/**********************************************************************************************************************
* End of function r_pkcl_ecdhx_encode_data()
**********************************************************************************************************************/
