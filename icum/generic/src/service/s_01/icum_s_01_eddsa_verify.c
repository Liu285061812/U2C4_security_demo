/******************************************************************************/
/* Component Name   Service 01 EdDSA Verify                                   */
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
/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_s_pub.h"
#include "icum_k_issd_pub.h"
#include "icum_s_01_common.h"
#include "icum_service_common.h"
#include "icum_s_01_eddsa_common.h"

/**********************************************************************************
* Prototype
***********************************************************************************/
static uint32_t s_01_CheckformatEddsaVerify(icum_job_t *p_job, ecc_key_t *p_key, hash_type_t hash_type);
static uint32_t s_01_CheckParamEddsaVerify(icum_job_t *p_job);
static uint32_t s_01_VerifyEddsaSignatureInit(icum_job_t *p_job);
static uint32_t s_01_VerifyEddsaSignatureUpdate(icum_job_t *p_job);
static uint32_t s_01_VerifyEddsaSignatureFinish(icum_job_t *p_job);

/**********************************************************************************
* Internal definition
***********************************************************************************/
/* Process phase */
#define PHASE_ON_HOLD           (0u)
#define PHASE_PREPARE_PROCESS   (1u)
#define PHASE_1                 (2u)
#define PHASE_2                 (3u)
#define PHASE_3                 (4u)

/* unsigned data size */
#define NUM2                    (2u)
#define NUM3                    (3u)
#define NUM4                    (4u)

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0046]:[ICUMFW_UD_S_01_EDDSA_0046]
* Function name: s_01_CheckformatEddsaVerify
* Description  : Check Eddsa format
* Arguments    : (IN)p_job : job parameter
*              : (IN)p_key : key
*              : (IN)hash_type : hash type
* Return Value : ER_OK
*                ER_PARAM
****************************************************************************************/
static uint32_t s_01_CheckformatEddsaVerify(icum_job_t *p_job, ecc_key_t *p_key, hash_type_t hash_type)
{
    ISD_EDDSA_VERIFY_t  *p_srv_param;
    uint32_t            ret;

    ret = ER_OK;
    p_srv_param = &(p_job->p_ISD->prm.EDDSA_VERIFY);

    if (ECC_CURVE_ED25519 == p_key->p_properties->ecc_curve)
    {
        if ((E_SHA2_512 == hash_type) &&
            ((FORMAT_STRING_DECIMAL  == p_srv_param->signature_format) ||
             (FORMAT_STRING_HEXA     == p_srv_param->signature_format) ||
             (FORMAT_BINARY_PLAIN    == p_srv_param->signature_format)) &&
            ((uint32_t)MAX_EDDSA_MESSAGE_SIZE >= p_srv_param->source_size_in_bytes))
        {
            /* do nothing */
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
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0004]:[ICUMFW_UD_S_01_EDDSA_0004]
* Function name: s_01_CheckParamEddsaVerify
* Description  : Check parameter of service request
* Arguments    : (IN)p_job : job parameter
*              : (OUT)p_job->p_ISD->service_result
*                SERV_OK
*                SERV_PRM_ERROR
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
* Return Value : ER_OK
****************************************************************************************/
static uint32_t s_01_CheckParamEddsaVerify(icum_job_t *p_job)
{
    ISD_EDDSA_VERIFY_t  *p_srv_param;
    hash_type_t         hash_type;
    uint16_t            hash_size;
    uint32_t            skip_flg;
    uint32_t            sig_size_in_bytes[NUM2];
    uint16_t            dec_string_bytes;
    uint8_t             mem_chk_result;
    uint8_t             stringCheck;
    remap_ptr_manip_t   p_sig0data_logical;
    remap_ptr_manip_t   p_sig1data_logical;
    uint32_t            ret;
    ecc_key_t           *p_key;

    ret = ER_OK;
    p_key = NULL;
    p_srv_param   = &(p_job->p_ISD->prm.EDDSA_VERIFY);
    hash_size = VARIABLE_INITIALIZATION;
    skip_flg = FLG_OFF;

    mem_chk_result = VARIABLE_INITIALIZATION;
    L_Toolbox_MemSet((uint8_t *)&sig_size_in_bytes, (uint8_t)ZERO, (int16_t)sizeof(sig_size_in_bytes));

    /* prepare key */
    ret = s_01_PrepareKeyEddsa(p_job, &p_key);
    if (ER_OK == ret)
    {
        /* check hash primitive, signature format, size of message */
        (void)L_HASH_GetInternalHashType(p_srv_param->hash_primitive, &hash_type, &hash_size);
        ret = s_01_CheckformatEddsaVerify(p_job, p_key, hash_type);
        if (ER_OK == ret)
    {
        /* do nothing */
    }
    else
    {
            skip_flg = FLG_ON;
        }
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

    /* check source address */
    if (FLG_OFF == skip_flg)
    {
        if (0u < (uint32_t)p_srv_param->source_size_in_bytes)
        {
            (void)MM_FLS_CheckMemory((uint32_t)p_srv_param->p_source, p_srv_param->source_size_in_bytes,
                                    FLS_CHK_MEM_SOURCE, p_job->expansion_service, &mem_chk_result);
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
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    /* check signature address */
    if (FLG_OFF == skip_flg)
    {
        if (FORMAT_BINARY_PLAIN == p_srv_param->signature_format)
        {
            sig_size_in_bytes[0] = ((uint32_t)p_srv_param->signature_size_in_bits[0] + ROUNDING) / BYTE_CONVERSION;
            sig_size_in_bytes[1] = ((uint32_t)p_srv_param->signature_size_in_bits[1] + ROUNDING) / BYTE_CONVERSION;
        }
        else if (FORMAT_STRING_DECIMAL == p_srv_param->signature_format)
        {
            (void)s_01_GetDecimalStrlenFromBitCnt(p_srv_param->signature_size_in_bits[0], &dec_string_bytes);
            sig_size_in_bytes[0] = (uint32_t)dec_string_bytes + NUM1;
            (void)s_01_GetDecimalStrlenFromBitCnt(p_srv_param->signature_size_in_bits[1], &dec_string_bytes);
            sig_size_in_bytes[1] = (uint32_t)dec_string_bytes + NUM1;
        }
        else
        {
            /* FORMAT_STRING_HEXA */
            sig_size_in_bytes[0] = (((uint32_t)p_srv_param->signature_size_in_bits[0] + NUM3) / NUM4) + NUM1;
            sig_size_in_bytes[1] = (((uint32_t)p_srv_param->signature_size_in_bits[1] + NUM3) / NUM4) + NUM1;
        }

        (void)MM_FLS_CheckMemory((uint32_t)p_srv_param->signature[0].p_data,
                                    sig_size_in_bytes[0], FLS_CHK_MEM_SOURCE,
                                    p_job->expansion_service, &mem_chk_result);
        if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_chk_result)
        {
            (void)MM_FLS_CheckMemory((uint32_t)p_srv_param->signature[1].p_data,
                                    sig_size_in_bytes[1], FLS_CHK_MEM_SOURCE,
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
        }
        else
        {
            p_job->p_ISD->service_result = SERV_SRC_MEM_ERROR;
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }

    /* check string format */
    if (FLG_OFF == skip_flg)
    {
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_srv_param->signature[0].p_data, &p_sig0data_logical.val);
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_srv_param->signature[1].p_data, &p_sig1data_logical.val);
        stringCheck = TRUE;
        if (FORMAT_STRING_DECIMAL == p_srv_param->signature_format)
        {
            /* check decimal string. */
            (void)s_01_IsDigitString(p_sig0data_logical.p_ptr, sig_size_in_bytes[0], &stringCheck);
            if (TRUE == (uint32_t)stringCheck)
            {
                (void)s_01_IsDigitString(p_sig1data_logical.p_ptr, sig_size_in_bytes[1], &stringCheck);
                if (TRUE == (uint32_t)stringCheck)
                {
                    /* Do nothing */
                }
                else
                {
                    p_job->p_ISD->service_result = SERV_PRM_ERROR;
                }
            }
            else
            {
                p_job->p_ISD->service_result = SERV_PRM_ERROR;
            }
        }
        else if(FORMAT_STRING_HEXA == p_srv_param->signature_format)
        {
            /* check hexadecimal string. */
            (void)s_01_IsHexDigitString(p_sig0data_logical.p_ptr, sig_size_in_bytes[0], &stringCheck);
            if (TRUE == (uint32_t)stringCheck)
            {
                (void)s_01_IsHexDigitString(p_sig1data_logical.p_ptr, sig_size_in_bytes[1], &stringCheck);
                if (TRUE == (uint32_t)stringCheck)
                {
                    /* do nothing */
                }
                else
                {
                    p_job->p_ISD->service_result = SERV_PRM_ERROR;
                }
            }
            else
            {
                p_job->p_ISD->service_result = SERV_PRM_ERROR;
            }
        }
        else
        {
            /* do nothing */
        }
    }
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
        p_job->prm.job_ecc.p_priv_key = NULL;
        p_job->prm.job_ecc.p_pub_key = (uint8_t *)p_key;
    }
    /* check result is in service_result */
    return ER_OK;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0043]:[ICUMFW_UD_S_01_EDDSA_0043]
* Function name: s_01_VerifyEddsaSignatureInit
* Description  : Perform initialize processing of verification EdDSA signature
* Arguments    : (IN/OUT)p_job : job parameter
* Return Value : ER_OK
*                ER_PARAM
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
static uint32_t s_01_VerifyEddsaSignatureInit(icum_job_t *p_job)
{
    uint32_t                ret;
    job_ecc_t           *p_job_param;
    ecc_ctx_t           *p_ecc_ctx;
    hash_ctx_t          *p_hash_ctx;
    ISD_EDDSA_VERIFY_t  *p_srv_param;
    remap_ptr_manip_t   p_source_logical;
    st_pkc_data_t       message;
    st_pkc_data_t       signature;
    hash_type_t         hash_type;
    uint16_t            hash_size;
    uint32_t            key_size;
    uint32_t            signature_size;
    uint8_t             dummy_data[BYTE_LEN_ED448_KEY];

    /* Initialize variable */
    ret = ER_OK;
    p_job_param    = &p_job->prm.job_ecc;
    p_ecc_ctx      = (ecc_ctx_t *)p_job->prm.job_ecc.p_ecc_ctx;
    p_hash_ctx     = (hash_ctx_t *)p_job->prm.job_ecc.p_hash_ctx;
    p_srv_param    = &p_job->p_ISD->prm.EDDSA_VERIFY;
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
        signature_size = BYTE_LEN_ED448_SIGN;
    }
    else
    {
        key_size = BYTE_LEN_ED25519_KEY;
        signature_size = BYTE_LEN_ED25519_SIGN;
    }

    L_Toolbox_MemCpy(&p_ecc_ctx->edwards.rs_buff[0],
                     (const uint8_t *)&p_ecc_ctx->edwards.sign_r[0],
                     (int16_t)signature_size);
    L_Toolbox_MemCpy(&p_ecc_ctx->edwards.rs_buff[signature_size],
                     (const uint8_t *)&p_ecc_ctx->edwards.sign_s[0],
                     (int16_t)signature_size);
    signature.len = signature_size * NUM2;
    signature.p_data = p_ecc_ctx->edwards.rs_buff;

    p_ecc_ctx->ed_ctx.len = key_size;
    p_ecc_ctx->ed_ctx.p_data =  &p_ecc_ctx->edwards.context[0];
    p_ecc_ctx->ctx.eddsa.p_ed_ctx = &p_ecc_ctx->ed_ctx;
    p_ecc_ctx->eddsa_pub_key.key_length = key_size;
    p_ecc_ctx->eddsa_pub_key.p_private = NULL;
    p_ecc_ctx->eddsa_pub_key.p_public = p_job_param->p_pub_key;
    /* Setup the base parameter of EdDSA for PKC library */
    ret = L_PKC_EddsaSetup(p_ecc_ctx, p_hash_ctx, p_srv_param->hash_primitive);
    if (ER_OK == ret)
    {
        p_ecc_ctx->ctx.eddsa.eddsa_variant = PKC_EDDSA_VARIANT_PURE;
        /* Perform EdDSA signature verification initial processing for PKC library */
        ret = L_PKC_EddsaVerifyInit(&message,
                                    &signature,
                                    (const void *)&p_ecc_ctx->eddsa_pub_key,
                                    &p_ecc_ctx->ctx.eddsa);
    if (ER_OK == ret)
    {
        /* move to next phase */
    }
    else
    {
        /* abort for error or processing */
            p_job->p_ISD->service_result = SERV_REQUEST;
    }
    }
    else
    {
        /* error case : do nothing */
        p_job->p_ISD->service_result = SERV_REQUEST;
    }

    return ret;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0044]:[ICUMFW_UD_S_01_EDDSA_0044]
* Function name: s_01_VerifyEddsaSignatureUpdate
* Description  : Perform update processing of verification EdDSA signature
* Arguments    : (IN/OUT)p_job : job parameter
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
static uint32_t s_01_VerifyEddsaSignatureUpdate(icum_job_t *p_job)
{
    uint32_t            ret;
    uint32_t            processing_bits;
    ecc_ctx_t           *p_ecc_ctx;
    ISD_EDDSA_VERIFY_t  *p_srv_param;

    /* Initialize variable */
    ret = ER_OK;
    p_ecc_ctx      = (ecc_ctx_t *)p_job->prm.job_ecc.p_ecc_ctx;
    p_srv_param    = &p_job->p_ISD->prm.EDDSA_VERIFY;

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
    /* Perform EdDSA signature verification processing for PKC library */
    ret = L_PKC_EddsaVerifyOperate(&p_ecc_ctx->ctx.eddsa, processing_bits);
    if ((ER_OK == ret) || (ER_IN_PROCESS == ret))
    {
        /* move to next phase */
    }
    else
    {
        /* abort for error or processing */
        p_job->p_ISD->service_result = SERV_REQUEST;
    }

    return ret;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0045]:[ICUMFW_UD_S_01_EDDSA_0045]
* Function name: s_01_VerifyEddsaSignatureFinish
* Description  : Perform termination processing of verification EdDSA signature
* Arguments    : (IN/OUT)p_job : job parameter
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_L_ECC_VERIFY_FAILED
****************************************************************************************/
static uint32_t s_01_VerifyEddsaSignatureFinish(icum_job_t *p_job)
{
    uint32_t            ret;
    ecc_ctx_t           *p_ecc_ctx;
    ISD_EDDSA_VERIFY_t  *p_srv_param;

    /* Initialize variable */
    ret = ER_OK;
    p_ecc_ctx      = (ecc_ctx_t *)p_job->prm.job_ecc.p_ecc_ctx;
    p_srv_param    = &p_job->p_ISD->prm.EDDSA_VERIFY;

    /* Perform EdDSA signature verification termination processing for PKC library */
    ret = L_PKC_EddsaVerifyFinal(&p_ecc_ctx->ctx.eddsa);
    if (ER_OK == ret)
    {
        p_srv_param->verification_result = VERIFICATION_PASS;
    }
    else if (ER_L_ECC_VERIFY_FAILED == ret)
    {
        p_srv_param->verification_result = VERIFICATION_FAIL;
        ret = ER_OK;
    }
    else
    {
        /* abort for error or processing */
        p_job->p_ISD->service_result = SERV_REQUEST;
    }

    return ret;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_EDDSA_0003]:[ICUMFW_UD_S_01_EDDSA_0003]
* Function name: S_01_VerifyEddsaSignature
* Description  : Generate EdDSA signature Service (service entry point)
* Arguments    : (IN-OUT)  p_job
* Return Value : void
****************************************************************************************/
void S_01_VerifyEddsaSignature(icum_job_t *p_job)
{
    uint32_t            ret;
    job_ecc_t           *p_job_param;
    ecc_ctx_t           *p_ecc_ctx;

    ret = ER_OK;
    p_job_param    = &p_job->prm.job_ecc;
    p_ecc_ctx      = (ecc_ctx_t *)p_job->prm.job_ecc.p_ecc_ctx;

    /* set initial value */
    p_job->p_ISD->service_result = SERV_REQUEST;
    p_job->p_ISD->prm.EDDSA_VERIFY.verification_result = VERIFICATION_FAIL;

    switch ((uint32_t)p_job->process_phase)
    {
        case PHASE_ON_HOLD:
            /* check service parameters */
            ret = s_01_CheckParamEddsaVerify(p_job);
            if (SERV_REQUEST == p_job->p_ISD->service_result)
            {
                /* move to next phase */
            }
            else
            {
                /* abort for error */
                break;
            }

        case PHASE_PREPARE_PROCESS:
            /* prepare for generate EdDSA signature */
            ret = s_01_PrepareProcessEddsa(p_job);
            if (ER_OK == ret)
            {
                /* set hash parameters for signature.R */
                p_job_param->hash_source_index = ZERO;
                if (ECC_CURVE_ED448 == p_ecc_ctx->curve_id)
                {
                    p_job_param->hash_remain_bytes = BYTE_LEN_ED448_SIGN;
                }
                else
                {
                    p_job_param->hash_remain_bytes = BYTE_LEN_ED25519_SIGN;
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
            /* Perform initialize processing of verifying ECDSA signature */
            ret = s_01_VerifyEddsaSignatureInit(p_job);
            if (ER_OK == ret)
            {
                /* move to next phase */
            }
            else
            {
                /* abort for error or processing */
                p_job->process_phase = PHASE_1;
                break;
            }

        case PHASE_2:
            /* Perform update processing of verifying ECDSA signature */
            ret = s_01_VerifyEddsaSignatureUpdate(p_job);
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
            /* Perform final processing of verifying ECDSA signature */
            ret = s_01_VerifyEddsaSignatureFinish(p_job);
            if (ER_OK == ret)
            {
                /* process done */
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
    }
    /* end of switch (p_job->process_phase) */

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
                                    p_job->p_ISD->prm.EDDSA_VERIFY.key_group,
                                    p_job->p_ISD->prm.EDDSA_VERIFY.key_id);
    }
    else
    {
        /* do nothing */
    }
}

