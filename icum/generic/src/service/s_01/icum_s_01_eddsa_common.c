/******************************************************************************/
/* Component Name   ICU-M Service                                             */
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

/**********************************************************************************
* Header files
***********************************************************************************/
#include "r_icumif_api.h"
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_l_cry_pk_pub.h"
#include "icum_ml_ctx_pub.h"
#include "icum_mh_key_pub.h"
#include "icum_s_pub.h"
#include "icum_k_issd_pub.h"
#include "icum_s_01_common.h"
#include "icum_service_common.h"
#include "icum_s_01_eddsa_common.h"
/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t s_01_DecodeSignEddsa(const uint8_t *p_src, uint8_t *p_dst,
                                     r_number_format_t signature_format, uint32_t signature_size);

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0014]:[ICUMFW_UD_S_01_EDDSA_0014]
* Function name: s_01_DecodeSignEddsa
* Description  : Decode signature
* Arguments    : (IN)p_src
*                (OUT)p_dst
*                (IN)signature_format
*                (IN)signature_size
* Return Value : ER_OK
*                ER_PARAM
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
****************************************************************************************/
static uint32_t s_01_DecodeSignEddsa(const uint8_t *p_src, uint8_t *p_dst,
                                     r_number_format_t signature_format, uint32_t signature_size)
{
    uint32_t        ret;
    big_num_type_t  bignum_type;
    pkc_small_data_t bignum_data;
    uint32_t        string_bytes;

    ret = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&bignum_data, (uint8_t)ZERO, (int16_t)sizeof(pkc_small_data_t));

    if ((int32_t)E_BINARY_PLAIN == (int32_t)signature_format)
    {
        /* no need conversion */
        L_Toolbox_MemCpy(p_dst, p_src, (int16_t)signature_size);
    }
    else
    {
        switch (signature_format)
        {
            case FORMAT_STRING_DECIMAL:
                bignum_type = E_STRING_DECIMAL;
                string_bytes = L_Toolbox_StrLen((const char *)p_src);
                break;
            case FORMAT_STRING_HEXA:
                bignum_type = E_STRING_HEXA;
                string_bytes = (signature_size << 1u);
                break;
            default:
                /* error on number format */
                ret = ER_PARAM;
                break;
        }
        if (ER_OK == ret)
        {
            /* encode to big number */
            ret = L_BIGNUM_EncodeToSmallBigNum((const void *)p_src,
                                                bignum_type,
                                                string_bytes,
                                                &bignum_data);
            if (ER_OK == ret)
            {
                L_Toolbox_MemCpy(p_dst, (uint8_t *)bignum_data.value, (int16_t)bignum_data.size);
                ret = L_BIGNUM_PaddingData(&signature_size, bignum_data.size, p_dst, NUM0);
            }
            else
            {
                /* do nothing */
            }
        }
        /* end of if (ER_OK == ret) */
        else
        {
            /* do nothing */
        }
    }
    return ret;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0015]:[ICUMFW_UD_S_01_EDDSA_0015]
* Function name: s_01_PrepareKeyEddsa
* Description  : Prepare ECC key
* Arguments    : (IN)p_job
*                (IN/OUT)p_key
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SLOT_EMPTY
****************************************************************************************/
uint32_t s_01_PrepareKeyEddsa(icum_job_t *p_job, ecc_key_t **p_key)
{
    uint32_t            ret;
    uint8_t             is_allowed;
    r_key_group_t       key_group;
    r_key_index_t       key_id;
    job_ecc_t           *p_job_param;

    ret = ER_OK;
    p_job_param = &p_job->prm.job_ecc;

    /* retrieve the ECC private key to be used */
    if (SERVICE_01_EDDSA_SIGN == p_job->p_ISD->service_id)
    {
        key_group = p_job->p_ISD->prm.EDDSA_SIGN.key_group;
        if ((KEY_GRP_ECC == key_group) ||
            (KEY_GRP_ECC_RAM == key_group))
        {
            key_id = p_job->p_ISD->prm.EDDSA_SIGN.key_id;
            p_job_param->job_slice = p_job->p_ISD->prm.EDDSA_SIGN.job_slice_hash;
        }
        else
        {
            p_job->p_ISD->service_result = SERV_INVALID_KEY_GROUP;
            ret = ER_PARAM;
        }
    }
    else
    {
        /* SERVICE_01_EDDSA_VERIFY */
        key_group = p_job->p_ISD->prm.EDDSA_VERIFY.key_group;
        if ((KEY_GRP_ECC == key_group) ||
            (KEY_GRP_ECC_RAM == key_group) ||
            (KEY_GRP_ECC_PUB == key_group) ||
            (KEY_GRP_ECC_PUB_EXT == key_group))
        {
            key_id = p_job->p_ISD->prm.EDDSA_VERIFY.key_id;
            p_job_param->job_slice = p_job->p_ISD->prm.EDDSA_VERIFY.job_slice_hash;
        }
        else
        {
            p_job->p_ISD->service_result = SERV_INVALID_KEY_GROUP;
            ret = ER_PARAM;
        }
    }

    if (ER_OK == ret)
    {
        /* get ECC key */
        ret = MH_KEY_GetEccKey(p_job, key_group, key_id, p_key);
        if (ER_OK == ret)
        {
            p_job_param->resource_ctrl |= GET_KEY_ECC;
            (void)s_01_CanUseKeyIfDbgConnected((key_flags_t)(*p_key)->p_properties->flags.disable_on_debug,
                                               &is_allowed);
            if (TRUE == (uint32_t)is_allowed)
            {
                /* do nothing */
            }
            else
            {
                /* key not usable: abort */
                p_job->p_ISD->service_result = SERV_KEY_NOT_USABLE;
                ret = ER_PARAM;
            }
        }
        /* end of if (ER_OK == ret) */
        else
        {
            /* do nothing */
        }
    }
    /* end of if (ER_OK == ret) */
    else
    {
        /* do nothing */
    }

    if (ER_OK == ret)
    {
        /* make sure the key has a private scalar */
        if ((SERVICE_01_EDDSA_VERIFY == p_job->p_ISD->service_id) ||
            (ZERO != (uint32_t)((*p_key)->p_properties->priv_scalar_size_in_bits)))
        {
            /* do nothing */
        }
        else
        {
            /* key invalidated: abort */
            p_job->p_ISD->service_result = SERV_KEY_NOT_USABLE;
            ret = ER_PARAM;
        }
    }
    /* end of if (ER_OK == ret) */
    else
    {
        /* do nothing */
    }

    return ret;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0006]:[ICUMFW_UD_S_01_EDDSA_0006]
* Function name: s_01_PrepareProcessEddsa
* Description  : Perform prepare process of EdDSA (parameter check, get context)
* Arguments    : (IN/OUT)p_job : job parameter
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SLOT_EMPTY
*                ER_ML_CTX_ALL_USE
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
****************************************************************************************/
uint32_t s_01_PrepareProcessEddsa(icum_job_t *p_job)
{
    uint32_t            ret;
    ecc_key_t           *p_key;
    job_ecc_t           *p_job_param;
    uint32_t            skip_flg;
    uint32_t            key_size;
    uint32_t            signature_size;
    ecc_ctx_t           *p_ecc_ctx;
    remap_ptr_manip_t   p_sig0data_logical;
    remap_ptr_manip_t   p_sig1data_logical;

    ret = ER_OK;
    p_key = NULL;
    skip_flg = FLG_OFF;
    p_job_param = &p_job->prm.job_ecc;

    p_job->p_ISD->service_result = SERV_REQUEST;

    if (SERVICE_01_EDDSA_SIGN == p_job->p_ISD->service_id)
    {
        p_key = (ecc_key_t *)p_job->prm.job_ecc.p_priv_key;
    }
    else
    {
        p_key = (ecc_key_t *)p_job->prm.job_ecc.p_pub_key;
    }

    ret = ML_CTX_Get(CTX_TYPE_ECC, &p_job_param->p_ecc_ctx);
    if (ER_OK == ret)
    {
        p_job_param->resource_ctrl |= GET_CTX_ECC;
    }
    else
    {
        skip_flg = FLG_ON;
    }

    /* get HASH context */
    if (FLG_OFF == skip_flg)
    {
        ret = ML_CTX_Get(CTX_TYPE_HASH, &p_job_param->p_hash_ctx);
        if (ER_OK == ret)
        {
            p_job_param->resource_ctrl |= GET_CTX_HASH;
        }
        else
        {
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }

    /* decode public key data */
    if (FLG_OFF == skip_flg)
    {
        p_ecc_ctx  = (ecc_ctx_t  *)p_job_param->p_ecc_ctx;
        /* Set the curve_id of ecc curve id */
        p_ecc_ctx->curve_id = p_key->p_properties->ecc_curve;
        if (ECC_CURVE_ED448 == p_ecc_ctx->curve_id)
        {
            key_size = BYTE_LEN_ED448_KEY;
            signature_size = BYTE_LEN_ED448_SIGN;
        }
        else
        {
            key_size = BYTE_LEN_ED25519_KEY;
            signature_size = BYTE_LEN_ED25519_SIGN;
        }
        /* decode of public key and save to context */
        L_Toolbox_MemCpy((uint8_t *)&p_ecc_ctx->edwards.pub_key_buf[0],
                         (const uint8_t *)p_key->p_pub_point->x.value,
                         (int16_t)key_size);
        p_job_param->p_pub_key = (uint8_t *)&p_ecc_ctx->edwards.pub_key_buf[0];
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    /* decode private key data */
    if (FLG_OFF == skip_flg)
    {
        if (SERVICE_01_EDDSA_SIGN == p_job->p_ISD->service_id)
        {
            /* decode of private key and save to context */
            L_Toolbox_MemCpy((uint8_t *)&p_ecc_ctx->edwards.prv_key_buf[0],
                             (const uint8_t *)p_key->p_priv_scalar->value,
                             (int16_t)key_size);
            p_job_param->p_priv_key = (uint8_t *)&p_ecc_ctx->edwards.prv_key_buf[0];
        }
        /* end of if (SERVICE_01_EDDSA_SIGN == p_job->p_ISD->service_id) */
        else
        {
            /* Get Remap(Convert Physical address -> Logical Address) */
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->p_ISD->prm.EDDSA_VERIFY.signature[0].p_data,
                    &p_sig0data_logical.val);

            /* decode signature.R */
            ret = s_01_DecodeSignEddsa((const uint8_t *)p_sig0data_logical.p_ptr,
                                       &p_ecc_ctx->edwards.sign_r[0],
                                       p_job->p_ISD->prm.EDDSA_VERIFY.signature_format,
                                       signature_size);
            if (ER_OK == ret)
            {
                /* Get Remap(Convert Physical address -> Logical Address) */
                (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->p_ISD->prm.EDDSA_VERIFY.signature[1].p_data,
                        &p_sig1data_logical.val);

                /* decode signature.S */
                ret = s_01_DecodeSignEddsa((const uint8_t *)p_sig1data_logical.p_ptr,
                                           &p_ecc_ctx->edwards.sign_s[0],
                                           p_job->p_ISD->prm.EDDSA_VERIFY.signature_format,
                                           signature_size);
                if (ER_OK == ret)
                {
                    /* do nothing */
                }
                else
                {
                    p_job->p_ISD->service_result = SERV_CRYPT_ERROR;
                    skip_flg = FLG_ON;
                }
            }
            /* end of if (ER_OK == ret) */
            else
            {
                p_job->p_ISD->service_result = SERV_CRYPT_ERROR;
                skip_flg = FLG_ON;
            }
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    /* total judgment */
    if (FLG_ON == skip_flg)
    {
        (void)s_01_PostProcessEddsa(p_job, p_key->key_group, p_key->key_id);
    }
    else
    {
        /* do nothing */
    }

    return (ret);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0007]:[ICUMFW_UD_S_01_EDDSA_0007]
* Function name: s_01_PostProcessEddsa
* Description  : Post process of EdDSA
* Arguments    : (IN/OUT)p_job : job parameter
*                (IN)key_group
*                (IN)key_id
* Return Value : ER_OK
****************************************************************************************/
uint32_t s_01_PostProcessEddsa(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id)
{
    job_ecc_t *p_job_param;

    p_job_param = &p_job->prm.job_ecc;

    if (GET_KEY_ECC == ((uint32_t)p_job_param->resource_ctrl & GET_KEY_ECC))
    {
        /* --- Free ECC_Key -------- */
        (void)MH_KEY_FreeKeyCache(p_job, key_group, key_id);
    }
    else
    {
        /* do nothing */
    }
    if (GET_CTX_ECC == ((uint32_t)p_job_param->resource_ctrl & GET_CTX_ECC))
    {
        /* --- Free ECC_Context ---- */
        (void)ML_CTX_Free(CTX_TYPE_ECC, p_job_param->p_ecc_ctx);
    }
    else
    {
        /* do nothing */
    }
    if (GET_CTX_HASH == ((uint32_t)p_job_param->resource_ctrl & GET_CTX_HASH))
    {
        /* --- Free HASH_Context ---- */
        (void)ML_CTX_Free(CTX_TYPE_HASH, p_job_param->p_hash_ctx);
    }
    else
    {
        /* do nothing */
    }
    p_job_param->resource_ctrl = GET_CTX_NONE;

    return ER_OK;
}

