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

/* PRQA S 0310 ++ # D002 In order not to disclose the structure used internally,
                            after receiving it with type uint8_t*, cast it to the type of the structure. */
/* PRQA S 1503 ++ # D010 No problem if it is unused because it is an API called from outside the library  */
/* PRQA S 2981 ++ # D013 Local variable ret of APIs sets INTERNALERR to the initial value
                        as a countermeasure against attacks that pass through processing. */
/* PRQA S 3673 ++ # D017 Because future extensions may edit values in context. */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "renesas_types.h"
#include "r_pkc_api.h"
#include "r_pkcl_api.h"
#include "r_pkcl_cmn.h"

/*!********************************************************************
 * \addtogroup    PKCLDH
 * \{
 * \addtogroup    PKCLDHDefines             DH Definitions
 * \addtogroup    PKCLDHTypes               DH Types
 * \addtogroup    PKCLDHConstants           DH Constants
 * \addtogroup    PKCLDHAPIs                DH APIs
 * \addtogroup    PKCLDHDefinesInternal     DH Internal Definitions
 * \addtogroup    PKCLDHTypesInternal       DH Internal Types
 * \addtogroup    PKCLDHVariablesInternal   DH Internal Variables
 * \addtogroup    PKCLDHAPIsInternal        DH Internal APIs
 * \}******************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCLDHDefinesInternal
 * \{******************************************************************/
#define PKCL_DH_MOD_LEN_MAX      (512U)      /**< Maximum byte length of modulus p */

#define PKCL_DH_KEY_LEN_1024                   (128U)           /**< DH1024 key byte length */
#define PKCL_DH_KEY_LEN_2048                   (256U)           /**< DH2048 key byte length */
#define PKCL_DH_KEY_LEN_3072                   (384U)           /**< DH3072 key byte length */
#define PKCL_DH_KEY_LEN_4096                   (512U)           /**< DH4096 key byte length */
#define PKCL_DH_Q_LEN_1024_MIN                 (20U)            /**< DH parameters q minimum byte length when DH1024 */
#define PKCL_DH_Q_LEN_2048_MIN                 (28U)            /**< DH parameters q minimum byte length when DH2048 */
#define PKCL_DH_Q_LEN_3072_MIN                 (32U)            /**< DH parameters q minimum byte length when DH3072 */
#define PKCL_DH_Q_LEN_4096_MIN                 (40U)            /**< DH parameters q minimum byte length when DH4096 */
#define PKCL_DH_G_LEN_MIN                      (4U)             /**< DH parameters g minimum byte length */

/** The #r_pkcl_dh_chk_dh_param does not check the DH parameters Q and G. */
#define PKCL_DH_CHK_DH_PARAM_NO_CHECK_QG       (0x00000000U)
/** The #r_pkcl_dh_chk_dh_param check the DH parameters Q */
#define PKCL_DH_CHK_DH_PARAM_CHECK_Q           (0x00000001U)
/** The #r_pkcl_dh_chk_dh_param check the DH parameters G */
#define PKCL_DH_CHK_DH_PARAM_CHECK_G           (0x00000002U)
/** The #r_pkcl_dh_chk_dh_param check the DH parameters Q and G */
#define PKCL_DH_CHK_DH_PARAM_CHECK_QG          (PKCL_DH_CHK_DH_PARAM_CHECK_Q | PKCL_DH_CHK_DH_PARAM_CHECK_G)

/*! \}*/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCLDHTypesInternal
 * \{******************************************************************/
/** A structure of values that needs to be buffered in DH */
typedef struct
{
    uint8_t shared_data[PKCL_DH_MOD_LEN_MAX];   /**< Buffer to store Shared Secret */
} st_pkcl_dh_t;

/*! \}*/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_dh_chk_dh_init_arg (const void * const p_priv_key, const void * const p_pub_key,
                                            const st_pkc_dh_ctx_t * const p_ctx);
static pkcl_ret_t r_pkcl_dh_chk_dh_operate_arg (const st_pkc_dh_ctx_t * const p_ctx,
                                                const st_pkc_operate_opt_t * const p_operate_opt);
static pkcl_ret_t r_pkcl_dh_chk_dh_final_arg (const st_pkc_dh_ctx_t * const p_ctx,
                                                    const st_pkc_data_t * const p_shared);
static pkcl_ret_t r_pkcl_dh_chk_dh_keygen_init_arg (const st_pkc_dh_ctx_t * const p_ctx);
static pkcl_ret_t r_pkcl_dh_chk_dh_keygen_operate_arg (const st_pkc_dh_ctx_t * const p_ctx,
                                                        const st_pkc_operate_opt_t * const p_operate_opt);
static pkcl_ret_t r_pkcl_dh_chk_dh_keygen_final_arg (const st_pkc_dh_ctx_t * const p_ctx,
                                                    const void * const p_priv_key,
                                                    const void * const p_pub_key);
static pkcl_ret_t r_pkcl_dh_chk_dh_param (const st_pkc_dh_param_t * const p_dh_param, const uint32_t chk_qg);
static pkcl_ret_t r_pkcl_dh_chk_dh_param_qg (const st_pkc_data_t * const p_q_or_g, const uint32_t len_min,
                                            const uint32_t p_len);

/**********************************************************************************************************************
* Function Name : R_PKCL_DhInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_DhInit(const void* const p_priv_key, const void* const p_pub_key,
                            st_pkc_dh_ctx_t* const p_ctx)
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
        ret = r_pkcl_dh_chk_dh_init_arg(p_priv_key, p_pub_key, p_ctx);
        if (PKCL_RET_OK == ret)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_dh_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_DhInit(p_priv_key, p_pub_key, p_ctx, p_buf_drv);

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
        r_pkcl_cmn_set_state(PKCL_STATE_DH_INIT_DONE);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_DhInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_DhOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_DhOperate(st_pkc_dh_ctx_t* const p_ctx,  const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    uint8_t * p_buf_drv;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if ((PKCL_STATE_DH_INIT_DONE == pkcl_state) ||
        (PKCL_STATE_DH_OPERATE_PROCESSING == pkcl_state))
    {
        /* Check argument */
        ret = r_pkcl_dh_chk_dh_operate_arg(p_ctx, p_operate_opt);
        if (PKCL_RET_OK == ret)
        {
            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_dh_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_DhOperate(p_ctx, p_buf_drv, p_operate_opt);

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
        r_pkcl_cmn_set_state(PKCL_STATE_DH_OPERATE_DONE);
    }
    else if (PKCL_RET_PROCESSING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_DH_OPERATE_PROCESSING);
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
* End of function R_PKCL_DhOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_DhFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_DhFinal(const st_pkc_dh_ctx_t* const p_ctx, st_pkc_data_t* const p_shared)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;
    st_pkcl_dh_t * p_dh;
    st_pkc_data_t shared_data;
    uint8_t * p_buf_drv;
    uint32_t kdf_ret;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_DH_OPERATE_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_dh_chk_dh_final_arg(p_ctx, p_shared);
        if (PKCL_RET_OK == ret)
        {
            /* Cast p_buf to ecdh buffer type */
            p_dh = (st_pkcl_dh_t*)p_ctx->p_buf;

            /* Prepare to pass shared secret to PKC-Driver */
            shared_data.p_data = p_dh->shared_data;
            shared_data.len = sizeof(p_dh->shared_data);

            /* Pass the p_buf address for PKC-Driver */
            p_buf_drv = &p_ctx->p_buf[sizeof(st_pkcl_dh_t)];

            /* Call PKC-Driver API */
            drv_ret = R_PKCD_DhFinal(p_ctx, p_buf_drv, &shared_data);

            /* Convert to PKC-Lib return code */
            ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);
            if (PKCL_RET_OK == ret)
            {
                if (NULL != p_ctx->p_kdf_info)
                {
                    /* Call KDF API */
                    kdf_ret = p_ctx->p_kdf_info->p_kdf_api(p_ctx->p_kdf_info->p_kdf_ctx, shared_data.p_data,
                                                            shared_data.len, p_ctx->p_kdf_info->out_kdf_len,
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
                    r_pkcl_cmn_cpy_byte(p_shared->p_data, shared_data.p_data, shared_data.len);
                    p_shared->len = shared_data.len;
                }

                /* Clear shared secret results stored in p_buf */
                r_pkcl_cmn_set_byte(p_dh->shared_data, 0U, sizeof(p_dh->shared_data));
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
}
/**********************************************************************************************************************
* End of function R_PKCL_DhFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_DhKeyGenInit
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_DhKeyGenInit(st_pkc_dh_ctx_t* const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkcl_state_t pkcl_state;
    pkcd_ret_t drv_ret;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_NO_EXEC_OPERATION == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_dh_chk_dh_keygen_init_arg(p_ctx);
        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_DhKeyGenInit(p_ctx, p_ctx->p_buf);

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
        r_pkcl_cmn_set_state(PKCL_STATE_DH_KEYGEN_INIT_DONE);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_DhKeyGenInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_DhKeyGenOperate
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_DhKeyGenOperate(st_pkc_dh_ctx_t* const p_ctx,  const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if ((PKCL_STATE_DH_KEYGEN_INIT_DONE == pkcl_state) ||
        (PKCL_STATE_DH_KEYGEN_OPERATE_PROCESSING == pkcl_state))
    {
        /* Check argument */
        ret = r_pkcl_dh_chk_dh_keygen_operate_arg(p_ctx, p_operate_opt);
        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_DhKeyGenOperate(p_ctx, p_ctx->p_buf, p_operate_opt);

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
        r_pkcl_cmn_set_state(PKCL_STATE_DH_KEYGEN_OPERATE_DONE);
    }
    else if (PKCL_RET_PROCESSING == ret)
    {
        r_pkcl_cmn_set_state(PKCL_STATE_DH_KEYGEN_OPERATE_PROCESSING);
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
* End of function R_PKCL_DhKeyGenOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_DhKeyGenFinal
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_DhKeyGenFinal(const st_pkc_dh_ctx_t* const p_ctx,
                                    void* const p_priv_key, void* const p_pub_key)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_DH_KEYGEN_OPERATE_DONE == pkcl_state)
    {
        /* Check argument */
        ret = r_pkcl_dh_chk_dh_keygen_final_arg(p_ctx, p_priv_key, p_pub_key);
        if (PKCL_RET_OK == ret)
        {
            /* Call PKC-Driver API */
            drv_ret = R_PKCD_DhKeyGenFinal(p_ctx, p_ctx->p_buf, p_priv_key, p_pub_key);

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
* End of function R_PKCL_DhKeyGenFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_dh_chk_dh_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLDHAPIsInternal
 * \brief       Check the arguments of #R_PKCL_DhInit.
 * \param       [in]     p_priv_key     DH private key of key binary format
 * \param       [in]     p_pub_key      DH public key of key binary format
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by DH.
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
 *  - Check the arguments received by the #R_PKCL_DhInit function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 * -# If ((NULL != p_priv_key) && (NULL != p_pub_key) && (NULL != p_ctx))
 *   -# If ((NULL != p_ctx->p_buf))
 *     -# If ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
 *       -# Check DH domain parameters with #r_pkcl_dh_chk_dh_param, and set ret to the return value.
 *       -# If #PKCL_RET_OK == ret
 *         -# If NULL != p_ctx->p_kdf_info
 *           -# If NULL != p_ctx->p_kdf_info->p_kdf_api
 *             -# Do nothing
 *           -# Else
 *             -# Set ret to #PKCL_RET_PARAMERR
 *         -# Else
 *           -# Do nothing
 *       -# Else
 *         -# Do nothing.
 *     -# Else
 *       -# Set ret to #PKCL_RET_PARAMERR
 *   -# Else
 *     -# Set ret to #PKCL_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCL_RET_PARAMERR
 * -# return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_dh_chk_dh_init_arg(const void* const p_priv_key, const void* const p_pub_key,
                                            const st_pkc_dh_ctx_t* const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_priv_key) && (NULL != p_pub_key) && (NULL != p_ctx))
    {
        /* check context */
        if (NULL != p_ctx->p_buf)
        {
            /* Check constant time mode */
            if ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
            {
                /* Check the DH domain parameters */
                ret = r_pkcl_dh_chk_dh_param(p_ctx->p_dh_param, PKCL_DH_CHK_DH_PARAM_NO_CHECK_QG);
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
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_dh_chk_dh_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_dh_chk_dh_operate_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCLDHAPIsInternal
 * \brief       Check the arguments of #R_PKCL_DhOperate.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by DH.
 * \param       [in]     p_operate_opt  Operate option selection.
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
 *  - Check the arguments received by the #R_PKCL_DhOperate function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 * -# If ((NULL != p_ctx) && (NULL != p_operate_opt))
 *   -# If ((NULL != p_ctx->p_buf))
 *     -# If ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
 *       -# Check DH domain parameters with #r_pkcl_dh_chk_dh_param, and set ret to the return value.
 *       -# If #PKCL_RET_OK == ret
 *         -# If NULL != p_ctx->p_kdf_info
 *           -# If NULL != p_ctx->p_kdf_info->p_kdf_api
 *             -# Do nothing
 *           -# Else
 *             -# Set ret to #PKCL_RET_PARAMERR
 *         -# Else
 *           -# Do nothing
 *       -# Else
 *         -# Do nothing.
 *     -# Else
 *       -# Set ret to #PKCL_RET_PARAMERR
 *   -# Else
 *     -# Set ret to #PKCL_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCL_RET_PARAMERR
 * -# return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_dh_chk_dh_operate_arg(const st_pkc_dh_ctx_t* const p_ctx,
                                                const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_operate_opt))
    {
        /* check context */
        if (NULL != p_ctx->p_buf)
        {
            /* Check constant time mode */
            if ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
            {
                /* Check the DH domain parameters */
                ret = r_pkcl_dh_chk_dh_param(p_ctx->p_dh_param, PKCL_DH_CHK_DH_PARAM_NO_CHECK_QG);
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
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_dh_chk_dh_operate_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_dh_chk_dh_final_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCLDHAPIsInternal
 * \brief       Check the arguments of #R_PKCL_DhFinal.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by DH.
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
 *  - Check the arguments received by the #R_PKCL_DhFinal function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 * -# If ((NULL != p_ctx) && (NULL != p_shared))
 *   -# If ((NULL != p_ctx->p_buf))
 *     -# Check DH domain parameters with #r_pkcl_dh_chk_dh_param, and set ret to the return value.
 *     -# If #PKCL_RET_OK == ret
 *       -# If NULL != p_ctx->p_kdf_info
 *         -# If NULL != p_ctx->p_kdf_info->p_kdf_api
 *           -# If p_shared->len >= p_ctx->p_kdf_info->out_kdf_len
 *             -# Do nothing
 *           -# Else
 *             -# Set ret to #PKCL_RET_LENGTHERR
 *         -# Else
 *           -# Set ret to #PKCL_RET_PARAMERR
 *       -# Else
 *         -# If p_shared->len >= p_ctx->p_dh_param->p_p->len
 *           -# Do nothing
 *         -# Else
 *           -# Set ret to #PKCL_RET_LENGTHERR
 *     -# Else
 *       -# Do nothing.
 *   -# Else
 *     -# Set ret to #PKCL_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCL_RET_PARAMERR
 * -# return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_dh_chk_dh_final_arg(const st_pkc_dh_ctx_t* const p_ctx,
                                                    const st_pkc_data_t* const p_shared)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_shared))
    {
        /* check context */
        if (NULL != p_ctx->p_buf)
        {
            /* check p_shared */
            if (NULL != p_shared->p_data)
            {
                /* Check the DH domain parameters */
                ret = r_pkcl_dh_chk_dh_param(p_ctx->p_dh_param, PKCL_DH_CHK_DH_PARAM_NO_CHECK_QG);
                if (PKCL_RET_OK == ret)
                {
                    /* check kdf info */
                    if (NULL != p_ctx->p_kdf_info)
                    {
                        /* if shared key */
                        if (NULL != p_ctx->p_kdf_info->p_kdf_api)
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
                        else
                        {
                            ret = PKCL_RET_PARAMERR;
                        }
                    }
                    else
                    {
                        /* if shared secret */
                        if (p_shared->len >= p_ctx->p_dh_param->p_p->len)
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
                    /* DH domain parameters error. Do nothing */
                }
            }
            else
            {
                /* p_shared is NULL */
                ret = PKCL_RET_PARAMERR;
            }
        }
        else
        {
            /* p_ctx->p_buf is NULL */
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        /* p_ctx is NULL */
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_dh_chk_dh_final_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_dh_chk_dh_keygen_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCLDHAPIsInternal
 * \brief       Check the arguments of #R_PKCL_DhKeyGenInit.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by ECDHX.
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
 *  - Check the arguments received by the #R_PKCL_DhKeyGenInit function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 * -# If (NULL != p_ctx)
 *   -# If ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api))
 *     -# If ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
 *       -# Check DH domain parameters with #r_pkcl_dh_chk_dh_param, and set ret to the return value.
 *     -# Else
 *       -# Set ret to #PKCL_RET_PARAMERR
 *   -# Else
 *     -# Set ret to #PKCL_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCL_RET_PARAMERR
 * -# return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_dh_chk_dh_keygen_init_arg(const st_pkc_dh_ctx_t* const p_ctx)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if (NULL != p_ctx)
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api))
        {
            /* Check constant time mode */
            if ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
            {
                /* Check the DH domain parameters */
                ret = r_pkcl_dh_chk_dh_param(p_ctx->p_dh_param, PKCL_DH_CHK_DH_PARAM_CHECK_QG);
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
* End of function r_pkcl_dh_chk_dh_keygen_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_dh_chk_dh_keygen_operate_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCLDHAPIsInternal
 * \brief       Check the arguments of #R_PKCL_DhKeyGenOperate.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by ECDHX.
 * \param       [in]     p_operate_opt  Operate option selection.
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
 *  - Check the arguments received by the #R_PKCL_DhKeyGenOperate function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 * -# If ((NULL != p_ctx) && (NULL != p_operate_opt))
 *   -# If ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api))
 *     -# If ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
 *       -# Check DH domain parameters with #r_pkcl_dh_chk_dh_param, and set ret to the return value.
 *     -# Else
 *       -# Set ret to #PKCL_RET_PARAMERR
 *   -# Else
 *     -# Set ret to #PKCL_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCL_RET_PARAMERR
 * -# return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_dh_chk_dh_keygen_operate_arg(const st_pkc_dh_ctx_t* const p_ctx,
                                                        const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_operate_opt))
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api))
        {
            /* Check constant time mode */
            if ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
            {
                /* Check the DH domain parameters */
                ret = r_pkcl_dh_chk_dh_param(p_ctx->p_dh_param, PKCL_DH_CHK_DH_PARAM_CHECK_QG);
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
* End of function r_pkcl_dh_chk_dh_keygen_operate_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_dh_chk_dh_keygen_final_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCLDHAPIsInternal
 * \brief       Check the arguments of #R_PKCL_DhKeyGenFinal.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by ECDHX.
 * \param       [in]     p_priv_key     DH private key of key binary format
 * \param       [in]     p_pub_key      DH public key of key binary format
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
 *  - Check the arguments received by the #R_PKCL_DhKeyGenFinal function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 * -# If ((NULL != p_ctx) && (NULL != p_priv_key) && (NULL != p_pub_key))
 *   -# If ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api))
 *     -# If ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
 *       -# Check DH domain parameters with #r_pkcl_dh_chk_dh_param, and set ret to the return value.
 *     -# Else
 *       -# Set ret to #PKCL_RET_PARAMERR
 *   -# Else
 *     -# Set ret to #PKCL_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCL_RET_PARAMERR
 * -# return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_dh_chk_dh_keygen_final_arg(const st_pkc_dh_ctx_t* const p_ctx,
                                                    const void* const p_priv_key,
                                                    const void* const p_pub_key)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* check if the argument is null */
    if ((NULL != p_ctx) && (NULL != p_priv_key) && (NULL != p_pub_key))
    {
        /* check context */
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api))
        {
            /* Check constant time mode */
            if ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
            {
                /* Check the DH domain parameters */
                ret = r_pkcl_dh_chk_dh_param(p_ctx->p_dh_param, PKCL_DH_CHK_DH_PARAM_CHECK_QG);
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
* End of function r_pkcl_dh_chk_dh_keygen_final_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_dh_chk_dh_param
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCLDHAPIsInternal
 * \brief        Check the DH domain parameters
 * \param       [in]   p_dh_param        DH domain parameters.
 * \param       [in]   chk_qg            Flags to set when checking DH parameters q and g.
 * \return
 *  - #PKCL_RET_OK:                     Successfully done.
 *  - #PKCL_RET_INTERNALERR:            Internal processing failed
 *  - #PKCL_RET_PARAMERR:               Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:              Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check if the data and length of the members of the DH parameter structure are set correctly.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 * -# If NULL != p_dh_param
 *   -# If NULL != p_dh_param->p_p
 *     -# If NULL != p_dh_param->p_p->p_data
 *       -# switch p_dh_param->p_p->len
 *         -# p_dh_param->p_p->len == #PKCL_DH_KEY_LEN_1024
 *           -# Set q_len_min to #PKCL_DH_Q_LEN_1024_MIN
 *           -# Set ret to #PKCL_RET_OK
 *         -# p_dh_param->p_p->len == #PKCL_DH_KEY_LEN_2048
 *           -# Set q_len_min to #PKCL_DH_Q_LEN_2048_MIN
 *           -# Set ret to #PKCL_RET_OK
 *         -# p_dh_param->p_p->len == #PKCL_DH_KEY_LEN_3072
 *           -# Set q_len_min to #PKCL_DH_Q_LEN_3072_MIN
 *           -# Set ret to #PKCL_RET_OK
 *         -# p_dh_param->p_p->len == #PKCL_DH_KEY_LEN_4096
 *           -# Set q_len_min to #PKCL_DH_Q_LEN_4096_MIN
 *           -# Set ret to #PKCL_RET_OK
 *         -# default
 *           -# Set ret to #PKCL_RET_LENGTHERR
 *     -# Else
 *       -# Set ret to #PKCL_RET_PARAMERR
 *   -# Else
 *     -# Set ret to #PKCL_RET_PARAMERR
 *   -# If #PKCL_RET_OK == ret
 *     -# If((chk_qg & #PKCL_DH_CHK_DH_PARAM_CHECK_Q) == #PKCL_DH_CHK_DH_PARAM_CHECK_Q)
 *       -# Check order q in DH parameters with #r_pkcl_dh_chk_dh_param_qg, and set ret to the return value.
 *     -# Else
 *       -# Do nothing
 *     -# If #PKCL_RET_OK == ret
 *       -# If((chk_qg & #PKCL_DH_CHK_DH_PARAM_CHECK_G) == #PKCL_DH_CHK_DH_PARAM_CHECK_G)
 *         -# Check order g in DH parameters with #r_pkcl_dh_chk_dh_param_qg, and set ret to the return value.
 *       -# Else
 *         -# Do nothing
 * -# Else
 *   -# Set ret to #PKCL_RET_PARAMERR
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_dh_chk_dh_param(const st_pkc_dh_param_t* const p_dh_param, const uint32_t chk_qg)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    uint32_t q_len_min;

    /* Check the key length range */
    if (NULL != p_dh_param)
    {
        /* Check the modulus p (p is always checkd) */
        if (NULL != p_dh_param->p_p)
        {
            if ((NULL != p_dh_param->p_p->p_data))
            {
                /* Check modulus p length */
                switch (p_dh_param->p_p->len)
                {
                    case PKCL_DH_KEY_LEN_1024:
                        q_len_min = PKCL_DH_Q_LEN_1024_MIN;
                        ret = PKCL_RET_OK;
                        break;
                    case PKCL_DH_KEY_LEN_2048:
                        q_len_min = PKCL_DH_Q_LEN_2048_MIN;
                        ret = PKCL_RET_OK;
                        break;
                    case PKCL_DH_KEY_LEN_3072:
                        q_len_min = PKCL_DH_Q_LEN_3072_MIN;
                        ret = PKCL_RET_OK;
                        break;
                    case PKCL_DH_KEY_LEN_4096:
                        q_len_min = PKCL_DH_Q_LEN_4096_MIN;
                        ret = PKCL_RET_OK;
                        break;
                    default:
                        ret = PKCL_RET_LENGTHERR;
                        break;
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

        /* Check the order q */
        if (PKCL_RET_OK == ret)
        {
            if ((chk_qg & PKCL_DH_CHK_DH_PARAM_CHECK_Q) == PKCL_DH_CHK_DH_PARAM_CHECK_Q)
            {
                ret = r_pkcl_dh_chk_dh_param_qg(p_dh_param->p_q, q_len_min, p_dh_param->p_p->len);
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

        /* Check the generator g */
        if (PKCL_RET_OK == ret)
        {
            if ((chk_qg & PKCL_DH_CHK_DH_PARAM_CHECK_G) == PKCL_DH_CHK_DH_PARAM_CHECK_G)
            {
                ret = r_pkcl_dh_chk_dh_param_qg(p_dh_param->p_g, PKCL_DH_G_LEN_MIN, p_dh_param->p_p->len);
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
/***********************************************************************************************************************
* End of function r_pkcl_dh_chk_dh_param()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_dh_chk_dh_param_qg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCLDHAPIsInternal
 * \brief        Check the DH domain parameters q or g
 * \param       [in]   p_q_or_g        A q or g data in DH parameters.
 * \param       [in]   len_min         Minimaum length of q or g.
 * \param       [in]   p_len           A length of modulus p in DH parameters.
 * \return
 *  - #PKCL_RET_OK:                     Successfully done.
 *  - #PKCL_RET_INTERNALERR:            Internal processing failed
 *  - #PKCL_RET_PARAMERR:               Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:              Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check if the data and length of the DH parameter q or g are set correctly.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 * -# If NULL != p_q_or_g
 *   -# If NULL != p_q_or_g->p_data
 *     -# Check if the length is a multiple of 4 with #r_pkcl_cmn_is_multiple_4,
 *        and set is_multiple_4 to the return value.
 *     -If (len_min <= p_q_or_g->len) && (p_q_or_g->len <= p_len) && (#PKC_BOOL_TRUE == is_multiple_4)
 *       -# Set ret to #PKCL_RET_OK
 *     -# Else
 *       -# Set ret to #PKCL_RET_LENGTHERR
 *   -# Else
 *     -# Set ret to #PKCL_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCL_RET_PARAMERR
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcl_ret_t r_pkcl_dh_chk_dh_param_qg(const st_pkc_data_t* const p_q_or_g, const uint32_t len_min,
                                            const uint32_t p_len)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    e_pkc_bool_t is_multiple_4;

    if (NULL != p_q_or_g)
    {
        if ((NULL != p_q_or_g->p_data))
        {
            /* Check length */
            is_multiple_4 = r_pkcl_cmn_is_multiple_4(p_q_or_g->len);
            if ((len_min <= p_q_or_g->len) && (p_q_or_g->len <= p_len) &&
                (PKC_BOOL_TRUE == is_multiple_4))
            {
                ret = PKCL_RET_OK;
            }
            else
            {
                /* Invalid range */
                ret = PKCL_RET_LENGTHERR;
            }

        }
        else
        {
            /* p_q_or_g->p_data is NULL */
            ret = PKCL_RET_PARAMERR;
        }
    }
    else
    {
        /* p_q_or_g is NULL */
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcl_dh_chk_dh_param_qg()
**********************************************************************************************************************/
