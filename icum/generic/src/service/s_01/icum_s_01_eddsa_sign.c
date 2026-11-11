/******************************************************************************/
/* Component Name   Service 01 EdDSA Sign                                     */
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
#include MM_FLASH_PUBLIC_H
#include "icum_s_pub.h"
#include "icum_k_issd_pub.h"
#include "icum_s_01_common.h"
#include "icum_service_common.h"
#include "icum_s_01_eddsa_common.h"
/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"

/**********************************************************************************
* Prototype
***********************************************************************************/
static uint32_t s_01_CheckformatEddsaSign(icum_job_t *p_job, ecc_key_t *p_key,
                                          hash_type_t hash_type, uint32_t *p_key_size_in_bits);
static uint32_t s_01_CheckParamEddsaSign(icum_job_t *p_job);
static uint32_t s_01_EncodeSignEddsa(const uint8_t *p_src, uint8_t *p_dst,
                                     r_number_format_t signature_format, uint32_t signature_size);
static uint32_t s_01_MakeEddsaSignatureInit(icum_job_t *p_job);
static uint32_t s_01_MakeEddsaSignatureUpdate(icum_job_t *p_job);
static uint32_t s_01_MakeEddsaSignatureFinish(icum_job_t *p_job);
static uint32_t s_01_MakeEddsaSignatureSub(icum_job_t *p_job);

/**********************************************************************************
* Internal definition
***********************************************************************************/
/* Process phase */
#define PHASE_ON_HOLD           (0u)
#define PHASE_PREPARE_PROCESS   (1u)
#define PHASE_1                 (2u)
#define PHASE_2                 (3u)
#define PHASE_3                 (4u)

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0047]:[ICUMFW_UD_S_01_EDDSA_0047]
* Function name: s_01_CheckformatEddsaSign
* Description  : Check Eddsa format
* Arguments    : (IN)p_job : job parameter
*              : (IN)p_key : key
*              : (IN)hash_type : hash type
*              : (OUT)p_key_size_in_bits : key size
* Return Value : ER_OK
*                ER_PARAM
****************************************************************************************/
static uint32_t s_01_CheckformatEddsaSign(icum_job_t *p_job, ecc_key_t *p_key,
                                          hash_type_t hash_type, uint32_t *p_key_size_in_bits)
{
    uint32_t ret;

    ret = ER_OK;

    if (ECC_CURVE_ED25519 == p_key->p_properties->ecc_curve)
    {
        if (E_SHA2_512 == hash_type)
        {
            *p_key_size_in_bits = BIT_LEN_ED25519_KEY;
        }
        else
        {
            /* error on hash type */
            p_job->p_ISD->service_result = SERV_PRM_ERROR;
            ret = ER_PARAM;
        }
    }
    else if (ECC_CURVE_ED448 == p_key->p_properties->ecc_curve)
    {
        /* error on hash type */
        p_job->p_ISD->service_result = SERV_PRM_ERROR;
        ret = ER_PARAM;
    }
    else
    {
        /* error on curve */
        p_job->p_ISD->service_result = SERV_INVALID_KEY_INDEX;
        ret = ER_PARAM;
    }

    return ret;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0002]:[ICUMFW_UD_S_01_EDDSA_0002]
* Function name: s_01_CheckParamEddsaSign
* Description  : Check parameter of service request
* Arguments    : (IN)p_job : job parameter
*              : (OUT)p_job->p_ISD->service_result
*                        Range: SERV_REQUEST
*                               SERV_PRM_ERROR
*                               SERV_SRC_MEM_ERROR
*                               SERV_DEST_MEM_ERROR
* Return Value : ER_OK
****************************************************************************************/
static uint32_t s_01_CheckParamEddsaSign(icum_job_t *p_job)
{
    ISD_EDDSA_SIGN_t    *p_srv_param;
    hash_type_t         hash_type;
    uint16_t            hash_size;
    uint32_t            skip_flg;
    uint8_t             mem_chk_result;
    uint32_t            output_buf_size_in_bytes;
    uint32_t            key_size_in_bits;
    uint16_t            max;
    uint32_t            ret;
    ecc_key_t           *p_key;

    ret = ER_OK;
    p_key = NULL;
    p_srv_param   = &(p_job->p_ISD->prm.EDDSA_SIGN);
    hash_size = VARIABLE_INITIALIZATION;
    skip_flg = FLG_OFF;
    mem_chk_result = VARIABLE_INITIALIZATION;
    output_buf_size_in_bytes = VARIABLE_INITIALIZATION;
    max = VARIABLE_INITIALIZATION;

    /* prepare key */
    ret = s_01_PrepareKeyEddsa(p_job, &p_key);
    if (ER_OK == ret)
    {
        /* do nothing */
    }
    else
    {
        if (SERV_REQUEST == p_job->p_ISD->service_result)
        {
            (void)s_01_ConvertErr(ret, &p_job->p_ISD->service_result);
        }
        else
        {
            /* do nothing */
        }
        skip_flg = FLG_ON;
    }

    /* check hash primitive */
    if (FLG_OFF == skip_flg)
    {
        (void)L_HASH_GetInternalHashType(p_srv_param->hash_primitive, &hash_type, &hash_size);
        ret = s_01_CheckformatEddsaSign(p_job, p_key, hash_type, &key_size_in_bits);
        if (ER_OK == ret)
        {
            /* do nothing */
        }
        else
        {
            skip_flg = FLG_ON;
        }
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    /* check signature format */
    if (FLG_OFF == skip_flg)
    {
        /* calculate size of output buffer (bytes) */
        switch (p_srv_param->signature_format)
        {
            case FORMAT_STRING_DECIMAL:
                /* get decimal figure length from bit_length. */
                (void)s_01_GetDecimalStrlenFromBitCnt((uint16_t)key_size_in_bits, &max);
                output_buf_size_in_bytes = max;
                /* increment for terminator(NULL) */
                output_buf_size_in_bytes++;
                break;
            case FORMAT_STRING_HEXA:
                output_buf_size_in_bytes = ((key_size_in_bits + ROUNDING) / BYTE_CONVERSION);
                output_buf_size_in_bytes *= DOUBLE;
                /* increment for terminator(NULL) */
                output_buf_size_in_bytes++;
                break;
            case FORMAT_BINARY_PLAIN:
                output_buf_size_in_bytes = ((key_size_in_bits + ROUNDING) / BYTE_CONVERSION);
                break;
            default:
                /* error on signature format */
                p_job->p_ISD->service_result = SERV_PRM_ERROR;
                skip_flg = FLG_ON;
                break;
        }
        /*  end of switch (p_srv_param->signature_format) */
    }
    /*  end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    /* check size of message */
    if (FLG_OFF == skip_flg)
    {
        if ((uint32_t)MAX_EDDSA_MESSAGE_SIZE >= p_srv_param->source_size_in_bytes)
        {
            /* do nothing */
        }
        else
        {
            /* error on message size */
            p_job->p_ISD->service_result = SERV_PRM_ERROR;
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }

    /* check source address */
    if (FLG_OFF == skip_flg)
    {
        if (0u < (uint32_t)p_srv_param->source_size_in_bytes)
        {
            (void)MM_FLS_CheckMemory((uint32_t)p_srv_param->p_source,
                    p_srv_param->source_size_in_bytes, FLS_CHK_MEM_SOURCE,
                    p_job->expansion_service, &mem_chk_result);
            if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_chk_result)
        {
            /* do nothing */
        }
        else
        {
            p_job->p_ISD->service_result = SERV_SRC_MEM_ERROR;
            skip_flg = FLG_ON;
        }
        } /* end of if (0u < (uint32_t)p_srv_param->source_size_in_bytes) */
        else
        {
            /* do nothing */
    }
    }
    /*  end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    /* check destination address 1 */
    if (FLG_OFF == skip_flg)
    {
        (void)MM_FLS_CheckMemory((uint32_t)p_srv_param->signature[0].p_data, output_buf_size_in_bytes,
                                  FLS_CHK_MEM_DESTINATION, p_job->expansion_service, &mem_chk_result);
        if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_chk_result)
        {
            /* do nothing */
        }
        else
        {
            p_job->p_ISD->service_result = SERV_DEST_MEM_ERROR;
            skip_flg = FLG_ON;
        }
    }
    /*  end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    /* check destination address 2 */
    if (FLG_OFF == skip_flg)
    {
        (void)MM_FLS_CheckMemory((uint32_t)p_srv_param->signature[1].p_data, output_buf_size_in_bytes,
                                  FLS_CHK_MEM_DESTINATION, p_job->expansion_service, &mem_chk_result);
        if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_chk_result)
        {
            /* do nothing */
        }
        else
        {
            p_job->p_ISD->service_result = SERV_DEST_MEM_ERROR;
        }
    }
    /*  end of if (FLG_OFF == skip_flg) */
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
        p_job->prm.job_ecc.p_priv_key = (uint8_t *)p_key;
        p_job->prm.job_ecc.p_pub_key = NULL;
    }
    return ER_OK;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0029]:[ICUMFW_UD_S_01_EDDSA_0029]
* Function name: s_01_EncodeSignEddsa
* Description  : Encode EdDSA signature.
* Arguments    : (IN)p_src
*                (OUT)p_dst
*                (IN)signature_format
*                (IN)signature_size
* Return Value : ER_OK
*                ER_PARAM
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
****************************************************************************************/
static uint32_t s_01_EncodeSignEddsa(const uint8_t *p_src, uint8_t *p_dst,
                                     r_number_format_t signature_format, uint32_t signature_size)
{
    uint32_t        ret;
    big_num_type_t  bignum_type;
    pkc_small_data_t bignum_data;
    uint32_t        size_in_bytes;

    /* Initialize variable */
    ret = ER_OK;
    bignum_type = E_STRING_DECIMAL;
    size_in_bytes = VARIABLE_INITIALIZATION;
    L_Toolbox_MemSet((uint8_t *)&bignum_data, DATA_ZERO, (int16_t)sizeof(pkc_small_data_t));

    if (FORMAT_BINARY_PLAIN == signature_format)
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
                size_in_bytes = ZERO;
                break;
            case FORMAT_STRING_HEXA:
                bignum_type = E_STRING_HEXA;
                size_in_bytes = (signature_size << 1u);
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
                                                E_BINARY_PLAIN,
                                                signature_size,
                                                &bignum_data);
            if (ER_OK == ret)
            {
                /* convert from big number into the required format */
                ret = L_BIGNUM_DecodeFromSmallBigNum(&bignum_data,
                                                     bignum_type,
                                                     (void *)p_dst,
                                                     &size_in_bytes);
            }
            else
            {
                /* do nothing */
            }
        }
        /*  end of if (ER_OK == ret) */
        else
        {
            /* do nothing */
        }
    }
    return ret;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0040]:[ICUMFW_UD_S_01_EDDSA_0040]
* Function name: s_01_MakeEddsaSignatureInit
* Description  : Perform initialize processing for generation EdDSA signature
* Arguments    : (IN)p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
static uint32_t s_01_MakeEddsaSignatureInit(icum_job_t *p_job)
{
    uint32_t            ret;
    job_ecc_t           *p_job_param;
    ecc_ctx_t           *p_ecc_ctx;
    hash_ctx_t        *p_hash_ctx;
    ISD_EDDSA_SIGN_t    *p_srv_param;
    remap_ptr_manip_t  p_source_logical;
    st_pkc_data_t      message;
    hash_type_t        hash_type;
    uint16_t           hash_size;
    uint32_t           key_size;
    uint8_t            dummy_data[BYTE_LEN_ED448_KEY];

    /* Initialize variable */
    ret = ER_OK;
    p_job_param    = &p_job->prm.job_ecc;
    p_ecc_ctx      = (ecc_ctx_t *)p_job->prm.job_ecc.p_ecc_ctx;
    p_hash_ctx     = (hash_ctx_t *)p_job->prm.job_ecc.p_hash_ctx;
    p_srv_param    = &p_job->p_ISD->prm.EDDSA_SIGN;
    L_Toolbox_MemSet(&dummy_data[0], DATA_ZERO, (int16_t)sizeof(dummy_data));

    /* Get Remap(Convert Physical address -> Logical Address) */
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_srv_param->p_source, &p_source_logical.val);
    (void)L_HASH_GetInternalHashType(p_srv_param->hash_primitive, &hash_type, &hash_size);

    /* Set message data */
    message.len = p_srv_param->source_size_in_bytes;
    if (NUM0 < p_srv_param->source_size_in_bytes)
        {
        message.p_data = p_source_logical.p_ptr;
    }
    else
    {
        message.p_data = &dummy_data[0];
    }

    if (ECC_CURVE_ED448 == p_ecc_ctx->curve_id)
    {
        key_size = BYTE_LEN_ED448_KEY;
    }
    else
    {
        key_size = BYTE_LEN_ED25519_KEY;
    }

    /* Setup the base parameter of EdDSA for PKC library */
    p_ecc_ctx->ed_ctx.len = key_size;
    p_ecc_ctx->ed_ctx.p_data =  &p_ecc_ctx->edwards.context[0];
    p_ecc_ctx->ctx.eddsa.p_ed_ctx = &p_ecc_ctx->ed_ctx;
    p_ecc_ctx->eddsa_priv_key.key_length = key_size;
    p_ecc_ctx->eddsa_priv_key.p_private = p_job_param->p_priv_key;
    p_ecc_ctx->eddsa_priv_key.p_public = p_job_param->p_pub_key;
    p_ecc_ctx->eddsa_pub_key.key_length = key_size;
    p_ecc_ctx->eddsa_pub_key.p_private = NULL;
    p_ecc_ctx->eddsa_pub_key.p_public = p_job_param->p_pub_key;
    /* Setup the common parameter of EdDSA for PKC library */
    ret = L_PKC_EddsaSetup(p_ecc_ctx, p_hash_ctx, p_srv_param->hash_primitive);
    if (ER_OK == ret)
    {
        p_ecc_ctx->ctx.eddsa.eddsa_variant = PKC_EDDSA_VARIANT_PURE;
        /* EdDSA signature generation initial processing for PKC library */
        ret = L_PKC_EddsaSignInit(&message,
                                  (const void *)&p_ecc_ctx->eddsa_priv_key,
                                  (const void *)&p_ecc_ctx->eddsa_pub_key,
                                  &p_ecc_ctx->ctx.eddsa);
    }
    else
    {
        /* abort for error */
    }

    return ret;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0041]:[ICUMFW_UD_S_01_EDDSA_0041]
* Function name: s_01_MakeEddsaSignatureUpdate
* Description  : Perform update processing for generation EdDSA signature
* Arguments    : (IN)p_job
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
static uint32_t s_01_MakeEddsaSignatureUpdate(icum_job_t *p_job)
{
    uint32_t            ret;
    ecc_ctx_t           *p_ecc_ctx;
    ISD_EDDSA_SIGN_t    *p_srv_param;
    uint32_t          processing_bits;

    /* Initialize variable */
    ret = ER_OK;
    p_ecc_ctx      = (ecc_ctx_t *)p_job->prm.job_ecc.p_ecc_ctx;
    p_srv_param    = &p_job->p_ISD->prm.EDDSA_SIGN;

    /* If job_slice_point_mul is zero, sets to processing bits the curve len size */
    if (ZERO == (uint32_t)p_srv_param->job_slice_point_mul)
    {
        processing_bits  = (p_ecc_ctx->ctx.eddsa.p_curve_info->curve_len * BYTE_SIZE_IN_BITS);
    }
    /* Other than those above, sets to processing bits the one bit */
    else
    {
        processing_bits  = NUM1;
    }

    /* Perform EdDSA signature generation processing for PKC library */
    ret = L_PKC_EddsaSignOperate(&p_ecc_ctx->ctx.eddsa, processing_bits);

    return ret;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0042]:[ICUMFW_UD_S_01_EDDSA_0042]
* Function name: s_01_MakeEddsaSignatureFinish
* Description  : Perform final processing for generation EdDSA signature
* Arguments    : (IN-OUT)p_job
* Return Value : ER_OK
*                ER_PARAM;
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
****************************************************************************************/
static uint32_t s_01_MakeEddsaSignatureFinish(icum_job_t *p_job)
{
    uint32_t            ret;
    ecc_ctx_t          *p_ecc_ctx;
    ISD_EDDSA_SIGN_t   *p_srv_param;
    st_pkc_data_t       signature;
    remap_ptr_manip_t   p_sig0data_logical;
    remap_ptr_manip_t   p_sig1data_logical;
    uint32_t            signature_size;
    uint32_t            signature_size_in_bits;

    /* Initialize variable */
    ret = ER_OK;
    p_ecc_ctx      = (ecc_ctx_t *)p_job->prm.job_ecc.p_ecc_ctx;
    p_srv_param    = &p_job->p_ISD->prm.EDDSA_SIGN;
    signature.p_data = (uint8_t *)p_ecc_ctx->edwards.rs_buff;
    signature.len = sizeof(p_ecc_ctx->edwards.rs_buff);
    if (ECC_CURVE_ED448 == p_ecc_ctx->curve_id)
    {
        signature_size = BYTE_LEN_ED448_SIGN;
        signature_size_in_bits = BIT_LEN_ED448_SIGN;
    }
    else
    {
        signature_size = BYTE_LEN_ED25519_SIGN;
        signature_size_in_bits = BIT_LEN_ED25519_SIGN;
    }

    /* EdDSA signature generation termination processing for PKC library */
    ret = L_PKC_EddsaSignFinal(&p_ecc_ctx->ctx.eddsa, &signature);
    if (ER_OK == ret)
    {
        /* Get signature R / S after execution is completed */
        L_Toolbox_MemCpy(&p_ecc_ctx->edwards.sign_r[NUM0],
                         &p_ecc_ctx->edwards.rs_buff[NUM0],
                         (int16_t)signature_size);
        L_Toolbox_MemCpy(&p_ecc_ctx->edwards.sign_s[NUM0],
                         &p_ecc_ctx->edwards.rs_buff[signature_size],
                         (int16_t)signature_size);

        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_srv_param->signature[0].p_data, &p_sig0data_logical.val);

        /* convert signature R */
        ret = s_01_EncodeSignEddsa((const uint8_t *)&p_ecc_ctx->edwards.sign_r[0],
                                   p_sig0data_logical.p_ptr,
                                   p_srv_param->signature_format,
                                   signature_size);
        p_srv_param->signature_size_in_bits[0] = signature_size_in_bits;
        if (ER_OK == ret)
        {
            /* Get Remap(Convert Physical address -> Logical Address) */
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_srv_param->signature[1].p_data, &p_sig1data_logical.val);

            /* convert signature S */
            ret = s_01_EncodeSignEddsa((const uint8_t *)&p_ecc_ctx->edwards.sign_s[0],
                                       p_sig1data_logical.p_ptr,
                                       p_srv_param->signature_format,
                                       signature_size);
            p_srv_param->signature_size_in_bits[1] = signature_size_in_bits;
        }
        else
        {
            /* abort for error */
        }
    }
    else
    {
        /* abort for error */
    }

    return ret;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0031]:[ICUMFW_UD_S_01_EDDSA_0031]
* Function name: s_01_MakeEddsaSignatureSub
* Description  : Perform sub process for generate EdDSA signature Service
* Arguments    : (IN-OUT)  p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
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
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_MakeEddsaSignatureSub(icum_job_t *p_job)
{
    uint32_t            ret;
    job_ecc_t           *p_job_param;
    ecc_ctx_t *p_ecc_ctx;

    /* Initialize variable */
    ret = ER_OK;
    p_job_param    = &p_job->prm.job_ecc;
    p_ecc_ctx = (ecc_ctx_t *)p_job->prm.job_ecc.p_ecc_ctx;

    switch ((uint32_t)p_job->process_phase)
    {
        case PHASE_PREPARE_PROCESS:
            /* Perform prepare for generate EdDSA signature */
            ret = s_01_PrepareProcessEddsa(p_job);
            if (ER_OK == ret)
            {
                /* set hash parameters */
                p_job_param->hash_source_index = ZERO;
                if (ECC_CURVE_ED448 == p_ecc_ctx->curve_id)
                {
                    p_job_param->hash_remain_bytes = BYTE_LEN_ED448_KEY;
                }
                else
                {
                    p_job_param->hash_remain_bytes = BYTE_LEN_ED25519_KEY;
                }
                /* move to next phase */
            }
            else
            {
                /* abort for error or pending */
                p_job->process_phase = PHASE_PREPARE_PROCESS;
                break;
            }

        case PHASE_1:
            /* Perform initialize processing for generate EdDSA signature */
            ret = s_01_MakeEddsaSignatureInit(p_job);
            if (ER_OK == ret)
            {
                /* move to next phase */
            }
            else
            {
                /* abort for error */
                p_job->process_phase = PHASE_1;
                break;
            }

        case PHASE_2:
            /* Perform update processing for generate EdDSA signature */
            ret = s_01_MakeEddsaSignatureUpdate(p_job);
            if (ER_OK == ret)
            {
                /* move to next phase */
            }
            else
            {
                /* abort for error or processing */
                p_job->process_phase = PHASE_2;
                break;
            }

        case PHASE_3:
            /* Perform final processing for generate EdDSA signature */
            ret = s_01_MakeEddsaSignatureFinish(p_job);
            if (ER_OK == ret)
            {
                /* move to next phase */
            }
            else
            {
                /* abort for error */
                p_job->process_phase = PHASE_3;
            }
            break;

        default:
            /* should never occur */
            p_job->p_ISD->service_result = SERV_SYS_ERROR;
            break;
    } /* end of switch (p_job->process_phase) */

    return ret;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0001]:[ICUMFW_UD_S_01_EDDSA_0001]
* Function name: S_01_MakeEddsaSignature
* Description  : Generate EdDSA signature Service (service entry point)
* Arguments    : (IN-OUT)  p_job
* Return Value : void
****************************************************************************************/
void S_01_MakeEddsaSignature(icum_job_t *p_job)
{
    uint32_t ret;
    uint32_t skip_flg;

    /* Initialize variable */
    ret = ER_OK;
    skip_flg = FLG_OFF;

    /* Set initial value to service result */
    p_job->p_ISD->service_result = SERV_REQUEST;

    if (PHASE_ON_HOLD == (uint32_t)p_job->process_phase)
    {
        /* check service parameters */
        ret = s_01_CheckParamEddsaSign(p_job);
        if (SERV_REQUEST == p_job->p_ISD->service_result)
        {
            /* move to next phase */
            p_job->process_phase = PHASE_PREPARE_PROCESS;
        }
        else
        {
            skip_flg = FLG_ON;
        }
    } /* end of if (PHASE_ON_HOLD == p_job->process_phase) */
    else
    {
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        ret = s_01_MakeEddsaSignatureSub(p_job);
    }
    else
    {
        /* do nothing */
    }

    if (SERV_REQUEST == p_job->p_ISD->service_result)
    {
        /* error: terminate the service */
        (void)s_01_ConvertErr(ret, &p_job->p_ISD->service_result);
    }
    else
    {
        /* do nothing */
    }
    if (SERV_IN_PROCESS != p_job->p_ISD->service_result)
    {
        (void)s_01_PostProcessEddsa(p_job,
                                    p_job->p_ISD->prm.EDDSA_SIGN.key_group,
                                    p_job->p_ISD->prm.EDDSA_SIGN.key_id);
    }
    else
    {
        /* do nothing */
    }
}

