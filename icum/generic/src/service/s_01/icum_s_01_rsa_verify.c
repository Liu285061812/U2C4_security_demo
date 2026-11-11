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
**********************************************************************************/
#include "renesas_types.h"
#include "r_icumif_api.h"

#include "icum_s_pub.h"
#include "icum_s_01_common.h"
#include "icum_s_01_rsa_common.h"

#include "icum_ml_ctx_pub.h"
#include "icum_ml_prng_pub.h"
#include MM_FLASH_PUBLIC_H
/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"

#include "icum_l_toolbox_pub.h"
#include "icum_l_cry_pk_pub.h"
#include "icum_service_common.h"

/**********************************************************************************
* Local define
**********************************************************************************/
/* unsigned data size */
#define NUM2                           (2u)
#define NUM3                           (3u)
#define NUM7                           (7u)
#define NUM8                           (8u)
#define NUM512                       (512u)

/**********************************************************************************
* Functions
**********************************************************************************/
static uint32_t s_01_InitializeJobVerify(icum_job_t *p_job);
static uint32_t s_01_RSA_VERIFY_Init(icum_job_t *p_job);
static uint32_t s_01_RSA_VERIFY_GenHash(icum_job_t *p_job);
static uint32_t s_01_RSA_VERIFY_PrepareSign(icum_job_t *p_job);
static uint32_t s_01_RSA_VERIFY_RsaInit(icum_job_t *p_job);
static uint32_t s_01_RSA_VERIFY_CalcExpMod(icum_job_t *p_job);
static uint32_t s_01_RSA_VERIFY_VerifySign(icum_job_t *p_job);
static void s_01_RSA_VERIFY_Finish(icum_job_t *p_job, const uint32_t err);

/*===========================================================================
  Run a RSA signature verification
  Command parameters extracted from the ICU-M service descriptor (ISD)
  [PUBLIC]
  ===========================================================================*/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0015]:[ICUMFW_UD_S_01_PK_0015]
* Function name: s_01_InitializeJobVerify
* Description  : Initialization processing
* Arguments    : (IN)   p_job
* Return Value : SERV_OK
*                SERV_PENDING
*                SERV_PRM_ERROR
*                SERV_INVALID_ENCODING_FORMAT
*                SERV_KEY_SLOT_EMPTY
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_NVM_UNAVAILABLE
*                SERV_KEY_NOT_USABLE
*                SERV_SRC_MEM_ERROR
****************************************************************************************/
static uint32_t s_01_InitializeJobVerify(icum_job_t *p_job)
{
    /* retrieve the service handler */
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_VERIFY_t   *p_ISD_PARAM;

    rsa_key_t       *p_key = (rsa_key_t *)0;
    hash_type_t     hash_type;
    uint8_t         is_allowed;
    uint8_t         stringCheck;
    uint32_t        signature_size_in_bytes;
    uint8_t         mem_check_result;
    uint16_t        hash_size_in_bytes;
    uint16_t        max;
    uint32_t        err;
    uint32_t        skip_flg = FLG_OFF;
    remap_ptr_manip_t   signature_logical;

    err = ER_OK;

    p_ISD       = p_job->p_ISD;
    p_JOB_PARAM = &p_job->prm.job_rsa;
    p_ISD_PARAM = &p_ISD->prm.RSA_VERIFY;

    /* make sure hash_encoding_format */
    switch (p_ISD_PARAM->hash_encoding_format)
    {
        case ENCODING_FORMAT_EMSA_PSS:
        case ENCODING_FORMAT_EMSA_PKCS1_V15:
            (void)L_HASH_GetInternalHashType(p_ISD_PARAM->hash_primitive, &hash_type, &hash_size_in_bytes);
            if (0u == (uint32_t)hash_size_in_bytes)
            {
                err = (uint32_t)SERV_PRM_ERROR;
                skip_flg = FLG_ON;
            }
            /* when hash provided, make sure source_size_in_bytes equals hash length */
            else if ((0u != (uint32_t)p_ISD_PARAM->is_precalculated_hash) &&
                     (p_ISD_PARAM->source_size_in_bytes != (uint32_t)hash_size_in_bytes))
            {
                err = (uint32_t)SERV_PRM_ERROR;
                skip_flg = FLG_ON;
            }
            else
            {
                /* do nothing */
            }
            break;
        default:
            err = (uint32_t)SERV_INVALID_ENCODING_FORMAT;
            skip_flg = FLG_ON;
            break;
    }
    
    if (FLG_OFF == skip_flg)
    {
        /* retrieve the key to be used */
        err = MH_KEY_GetRsaKey(p_job, p_ISD_PARAM->key_group, p_ISD_PARAM->key_id, &p_key);
        (void)s_01_RsaKeyConvertErr(&err);
        
        if (ER_OK == err)
        {
            p_JOB_PARAM->resource_ctrl |= GET_KEY_RSA;
            
            (void)s_01_CanUseKeyIfDbgConnected((key_flags_t)p_key->p_properties->flags.disable_on_debug,
                                               &is_allowed);
            if (TRUE == (uint32_t)is_allowed)
            {
                /* check modulus size: should be equal to the signature size */
                if ((uint32_t)p_ISD_PARAM->signature_size_in_bits !=
                    (uint32_t)p_key->p_properties->modulus_size_in_bits)
                {
                    err = (uint32_t)SERV_PRM_ERROR;
                    skip_flg = FLG_ON;
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* key not usable: abort */
                err = (uint32_t)SERV_KEY_NOT_USABLE;
                skip_flg = FLG_ON;
            }
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

    if (skip_flg == FLG_OFF)
    {
        /* calculate size of input buffer (bytes) */
        switch(p_ISD_PARAM->signature_format)
        {
            case FORMAT_STRING_DECIMAL:
                /* get decimal figure length from bit_length. */
                (void)s_01_GetDecimalStrlenFromBitCnt(p_ISD_PARAM->signature_size_in_bits, &max);
                signature_size_in_bytes = max;
                /* increment for terminator(NULL) */
                signature_size_in_bytes++;
                break;
            case FORMAT_STRING_HEXA:
                signature_size_in_bytes = ((uint32_t)p_ISD_PARAM->signature_size_in_bits + NUM7) / NUM8;
                signature_size_in_bytes *= NUM2;
                /* increment for terminator(NULL) */
                signature_size_in_bytes++;
                break;
            case FORMAT_BINARY_PLAIN:
                signature_size_in_bytes = ((uint32_t)p_ISD_PARAM->signature_size_in_bits + NUM7) / NUM8;
                break;
            default:
                /* error on number format */
                err = (uint32_t)SERV_PRM_ERROR;
                skip_flg = FLG_ON;
                break;
        }
    }
    else
    {
        /* do nothing */
    }

    if (skip_flg == FLG_OFF)
    {
        /* make sure the source address range is valid */
        (void)MM_FLS_CheckMemory((uint32_t)p_ISD_PARAM->p_source,
                p_ISD_PARAM->source_size_in_bytes, FLS_CHK_MEM_SOURCE,
                p_job->expansion_service, &mem_check_result);

        if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_check_result)
        {
            /* make sure the source address range is valid */
            (void)MM_FLS_CheckMemory((uint32_t)p_ISD_PARAM->p_signature,
                    signature_size_in_bytes, FLS_CHK_MEM_SOURCE,
                    p_job->expansion_service, &mem_check_result);

            if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_check_result)
            {
                /* Get Remap(Convert Physical address -> Logical Address) */
                (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD_PARAM->p_signature, &signature_logical.val);

                /* check string format */
                stringCheck = TRUE;
                if(p_ISD_PARAM->signature_format == FORMAT_STRING_DECIMAL)
                {
                    /* check decimal string. */
                    (void)s_01_IsDigitString(signature_logical.p_ptr, signature_size_in_bytes, &stringCheck);
                }
                else if(p_ISD_PARAM->signature_format == FORMAT_STRING_HEXA)
                {
                    /* check hexadecimal string. */
                    (void)s_01_IsHexDigitString(signature_logical.p_ptr, signature_size_in_bytes, &stringCheck);
                }
                else
                {
                    /* do nothing */
                }
                if (FALSE == (uint32_t)stringCheck)
                {
                    err = (uint32_t)SERV_PRM_ERROR;
                }
                else
                {
                    /* init the job slice */
                    p_JOB_PARAM->job_slice = p_ISD_PARAM->job_slice_mod_exp;
                    p_JOB_PARAM->p_key = (uint8_t *)p_key;
                }
            }
            else
            {
                err = (uint32_t)SERV_SRC_MEM_ERROR;
            }
        }
        else
        {
            err = (uint32_t)SERV_SRC_MEM_ERROR;
        }
    }
    else
    {
        /* do nothing */
    }
    
    return(err);
}

/**********************************************************************************
* ICU-M service
**********************************************************************************/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0040]:[ICUMFW_UD_S_01_PK_0040]
* Function name: s_01_RSA_VERIFY_Init
* Description  : Initialization part of verifying RSA signature service
* Arguments    : (IN-OUT)  p_job
* Return Value : SERV_OK
*                SERV_PENDING
*                SERV_SYS_ERROR
*                SERV_PRM_ERROR
*                SERV_INVALID_ENCODING_FORMAT
*                SERV_KEY_SLOT_EMPTY
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_NVM_UNAVAILABLE
*                SERV_KEY_NOT_USABLE
*                SERV_SRC_MEM_ERROR
****************************************************************************************/
static uint32_t s_01_RSA_VERIFY_Init(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_VERIFY_t   *p_ISD_PARAM;
    
    uint32_t    err;
    
    /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    p_ISD_PARAM   = &p_ISD->prm.RSA_VERIFY;
    
    switch ((uint32_t)p_job->process_phase)
    {
        case ON_HOLD:
            if (GET_CTX_NONE == ((uint32_t)p_JOB_PARAM->resource_ctrl & GET_CTX_RSA))
            {
                /* get rsa context */
                err = ML_CTX_Get(CTX_TYPE_RSA, &p_JOB_PARAM->p_rsa_ctx);
                if (ER_ML_CTX_ALL_USE == err)
                {
                    err = (uint32_t)SERV_PENDING;
                    break;
                }
                else if (ER_OK != err)
                {
                    /* should never occur */
                    err = (uint32_t)SERV_SYS_ERROR;
                    break;
                }
                else
                {
                    /* do nothing */
                }
                p_JOB_PARAM->resource_ctrl |= GET_CTX_RSA;
                p_ISD->service_result = SERV_IN_PROCESS;
            }
            else
            {
                /* do nothing */
            }

            /* Get hash context */
            if (GET_CTX_NONE == ((uint32_t)p_JOB_PARAM->resource_ctrl & GET_CTX_HASH))
            {
                err = ML_CTX_Get(CTX_TYPE_HASH, &p_JOB_PARAM->p_hash_ctx);
                if (ER_ML_CTX_ALL_USE == err)
                {
                    err = (uint32_t)SERV_PENDING;
                    break;
                }
                else if (ER_OK != err)
                {
                    /* should never occur */
                    err = (uint32_t)SERV_SYS_ERROR;
                    break;
                }
                else
                {
                    /* do nothing */
                }
                p_JOB_PARAM->resource_ctrl |= GET_CTX_HASH;
                p_ISD->service_result = SERV_IN_PROCESS;
            }
            else
            {
                /* do nothing */
            }

            /* ************************************************************************* */
            /* Initialization */
            /* ************************************************************************* */
            /* reset verification result to fail */
            p_ISD_PARAM->verification_result = VERIFICATION_FAIL;

            err = s_01_InitializeJobVerify(p_job);
            if ((uint32_t)SERV_OK != err)
            {
                /* init error - break here */
                break;
            }
            else
            {
                /* do nothing */
            }

            /* get the pointer the the RSA public key used */

            /* test if the hash has been already calculated */
            /* it could be the case when the service is triggered internally */
            /* e.g. via the memory cluster installation service */
            if (NUM0 != p_ISD_PARAM->is_precalculated_hash)
            {
                /* yes: the service has been triggered internally */
                /* skip the hash calculation */
                p_job->process_phase = PHASE_1;
            }
            else
            {
                /* no: the service has been triggered by the application */
                /* proceed as normal */
                p_job->process_phase = PHASE_H1;
            }
            break;
        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    
    return (err);
}


/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0041]:[ICUMFW_UD_S_01_PK_0041]
* Function name: s_01_RSA_VERIFY_GenHash
* Description  : Hash generation part of verifying RSA signature service
* Arguments    : (IN-OUT)  p_job
* Return Value : SERV_OK
*                SERV_CRYPT_ERROR
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_RSA_VERIFY_GenHash(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_VERIFY_t   *p_ISD_PARAM;
    
    hash_ctx_t *p_hash_ctx;
        
    uint32_t    err;
    remap_ptr_manip_t   source_logical;
    
    /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    p_ISD_PARAM   = &p_ISD->prm.RSA_VERIFY;
    
    /* get the pointer the the RSA public key used */
    p_hash_ctx = (hash_ctx_t *)p_JOB_PARAM->p_hash_ctx;
    
    /* Get Remap(Convert Physical address -> Logical Address) */
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD_PARAM->p_source, &source_logical.val);

    switch ((uint32_t)p_job->process_phase)
    {
        case PHASE_H1:
        {
            hash_type_t         hash_type;
            uint16_t            dmy_hash_size;

            (void)L_HASH_GetInternalHashType(p_ISD_PARAM->hash_primitive, &hash_type, &dmy_hash_size);

            /* ************************************************************************* */
            /* Start a hash job */
            /* ************************************************************************* */
            err = L_HASH_Init(p_hash_ctx, hash_type);
            if (ER_OK != err)
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
                break;
            }
            else
            {
                /* do nothing */
            }

            if (0u != (uint32_t)p_ISD_PARAM->job_slice_hash)
            {
                p_JOB_PARAM->hash_source_index    = 0u;
                p_JOB_PARAM->hash_remain_bytes    = p_ISD_PARAM->source_size_in_bytes;

                p_job->process_phase = PHASE_H2DIV;
            }
            else
            {
                p_job->process_phase = PHASE_H2;
            }

            break;
        }

        case PHASE_H2:
            /* all at once hash */
            err = L_HASH_Update(p_hash_ctx, source_logical.p_ptr, p_ISD_PARAM->source_size_in_bytes);
            if (ER_OK != err)
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
                break;
            }
            else
            {
                /* do nothing */
            }

            /****************************************************************************************************
             * In RSA_VERIFY , there is not PHASE_H3.
             * "Get plain_hash" and "Free CTX_TYPE_HASH" are dealt in PHASE_5
             ****************************************************************************************************/
            p_job->process_phase = PHASE_1;
            break;

        case PHASE_H2DIV:
            /* job slice hash */
            if (p_JOB_PARAM->hash_remain_bytes <= (uint32_t)p_ISD_PARAM->job_slice_hash)
            {
                err = L_HASH_Update(p_hash_ctx, &source_logical.p_ptr[p_JOB_PARAM->hash_source_index],
                                    p_JOB_PARAM->hash_remain_bytes);
                if (ER_OK != err)
                {
                    err = (uint32_t)SERV_CRYPT_ERROR;
                    break;
                }
                else
                {
                    /* do nothing */
                }

                p_JOB_PARAM->hash_remain_bytes = 0u;
                p_job->process_phase = PHASE_1;
                /* return dispatcher */
                break;
            }
            else
            {
                /* do nothing */
            }

            err = L_HASH_Update(p_hash_ctx, &source_logical.p_ptr[p_JOB_PARAM->hash_source_index],
                                p_ISD_PARAM->job_slice_hash);
            if (ER_OK != err)
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
                break;
            }
            else
            {
                /* do nothing */
            }
            /* update pointers and number of bytes remaining */
            p_JOB_PARAM->hash_remain_bytes -= (uint32_t)p_ISD_PARAM->job_slice_hash;
            p_JOB_PARAM->hash_source_index += (uint32_t)p_ISD_PARAM->job_slice_hash;
            /* return dispatcher */
            break;
        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    
    return (err);
}


/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0042]:[ICUMFW_UD_S_01_PK_0042]
* Function name: s_01_RSA_VERIFY_PrepareSign
* Description  : Prepare RSA signature
* Arguments    : (IN-OUT)  p_job
* Return Value : SERV_OK
*                SERV_CRYPT_ERROR
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_RSA_VERIFY_PrepareSign(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_VERIFY_t   *p_ISD_PARAM;
    
    rsa_ctx_t  *p_rsa_ctx;
    uint32_t    err;
    
    big_num_type_t  temp_type;
    uint32_t        bn_area_len;
    uint32_t        sign_len;
    remap_ptr_manip_t   signature_logical;
    
    /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    p_ISD_PARAM   = &p_ISD->prm.RSA_VERIFY;
    
    /* get the pointer the the RSA public key used */
    p_rsa_ctx  = (rsa_ctx_t  *)p_JOB_PARAM->p_rsa_ctx;
    
    temp_type   = E_BINARY_PLAIN;
    bn_area_len = 0u;
    
    /* Get Remap(Convert Physical address -> Logical Address) */
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD_PARAM->p_signature, &signature_logical.val);

    switch ((uint32_t)p_job->process_phase)
    {
        case PHASE_1:
            /* ************************************************************************* */
            /* Prepare the signature for processing */
            /* ************************************************************************* */
            /* check format of the input signature */
            switch(p_ISD_PARAM->signature_format)
            {
                case FORMAT_STRING_DECIMAL:
                    temp_type = E_STRING_DECIMAL;
                    bn_area_len = L_Toolbox_StrLen((const char *)signature_logical.p_ptr);
                    break;
                case FORMAT_STRING_HEXA:
                    temp_type = E_STRING_HEXA;
                    bn_area_len = L_Toolbox_StrLen((const char *)signature_logical.p_ptr);
                    break;
                case FORMAT_BINARY_PLAIN:
                    temp_type = E_BINARY_PLAIN;
                    bn_area_len = (((uint32_t)p_ISD_PARAM->signature_size_in_bits + NUM7) >> NUM3);
                    break;
                default:
                    /* should never occur */
                    break;
            }

            /* convert signature to a big number */
            /* use W for temporary big_num */
            err = L_BIGNUM_EncodeToBigNum(signature_logical.p_ptr, temp_type, bn_area_len, &p_rsa_ctx->W);
            if (ER_OK != err)
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
                break;
            }
            else
            {
                /* do nothing */
            }

            sign_len = ((uint32_t)p_ISD_PARAM->signature_size_in_bits + NUM7) >> NUM3;
            err = L_BIGNUM_PaddingData(&sign_len, p_rsa_ctx->W.size,
                                       (uint8_t *)p_rsa_ctx->W.value, NUM0);
            p_rsa_ctx->W.size = sign_len;
            if (ER_OK != err)
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
                break;
            }
            else
            {
                /* do nothing */
            }

            if (NUM0 != (uint32_t)p_ISD_PARAM->job_slice_mod_exp)
            {
                /* I can break, because temp_big_num is saved in W */
                p_JOB_PARAM->proc_type = RSA_PROC_ASYNC;
            }
            else
            {
                p_JOB_PARAM->proc_type = RSA_PROC_SYNC;
            }
            p_job->process_phase = PHASE_2;

            break;
        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    
    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0054]:[ICUMFW_UD_S_01_PK_0054]
* Function name: s_01_RSA_VERIFY_RsaInit
* Description  : RSA verification initial processing
* Arguments    : (IN-OUT)  p_job
* Return Value : SERV_OK
*                SERV_CRYPT_ERROR
*                SERV_NO_INIT_RNG
****************************************************************************************/
static uint32_t s_01_RSA_VERIFY_RsaInit(icum_job_t *p_job)
{
    uint32_t err;
    r_icumif_isd_t *p_isd;
    job_rsa_t *p_job_param;
    ISD_RSA_VERIFY_t *p_isd_param;
    rsa_key_t  *p_key;
    rsa_ctx_t  *p_rsa_ctx;
    hash_ctx_t *p_hash_ctx;
    
    /* PKC Lib Parameter */
    st_pkc_data_t digest;
    st_pkc_data_t sign;
    st_pkcd_rsa_key_bin_t rsa_key;
    
    hash_type_t hash_type;
    uint16_t hash_size;
    uint8_t *p_hashed_message;
    uint8_t plain_hash_buff[HASH_512_SIZE_IN_BYTES];
    remap_ptr_manip_t source_logical;

    err         = ER_OK;
    p_isd       = p_job->p_ISD;
    p_job_param = &p_job->prm.job_rsa;
    p_isd_param = &p_isd->prm.RSA_VERIFY;
    p_key       = (rsa_key_t  *)p_job_param->p_key;
    p_rsa_ctx   = (rsa_ctx_t  *)p_job_param->p_rsa_ctx;
    p_hash_ctx  = (hash_ctx_t *)p_job_param->p_hash_ctx;

    /* Initialize parameters for PKC API */
    L_Toolbox_MemSet((uint8_t *)&rsa_key, NUM0, (int16_t)sizeof(st_pkcd_rsa_key_bin_t));
    
    if (NUM0 != (uint32_t)p_isd_param->is_precalculated_hash)
    {
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_param->p_source,
                &source_logical.val);
        p_hashed_message = source_logical.p_ptr;
    }
    else
    {
        /* if p_source is message, retrieve the hash value */
        err = L_HASH_Finish(p_hash_ctx, plain_hash_buff);
        if (ER_OK != err)
        {
            err = (uint32_t)SERV_CRYPT_ERROR;
        }
        else
        {
            /* do nothing */
        }
        p_hashed_message = (uint8_t *)plain_hash_buff;
    }
    /* end of else */

    if (ER_OK == err)
    {
        (void)L_HASH_GetInternalHashType(p_isd_param->hash_primitive, &hash_type, &hash_size);
        (void)L_PKC_RsaSignSetup(p_rsa_ctx, p_isd_param->hash_encoding_format,
                                    (hash_ctx_t *)p_job_param->p_hash_ctx, p_isd_param->hash_primitive);
        if (ENCODING_FORMAT_EMSA_PSS == p_isd_param->hash_encoding_format)
        {
            p_rsa_ctx->pkc_ctx.p_mask_option->salt_len = (uint32_t)p_isd_param->salt_size_in_bytes;
        }
        else
        {
            /* Do nothing */
        }

        rsa_key.key_length = (uint32_t)p_key->p_properties->modulus_size_in_bytes;
        rsa_key.p_n = p_key->p_modulus;
        rsa_key.p_e = p_key->p_pub_exponent;
        rsa_key.e_length = (uint32_t)p_key->p_properties->pub_exp_size_in_bytes;
        digest.p_data = p_hashed_message;
        digest.len = (uint32_t)hash_size;
        sign.len = p_rsa_ctx->W.size;
        sign.p_data = (uint8_t *)&p_rsa_ctx->W.value;
        err = L_PKC_RsaVerifyInit(&digest, &sign, (void *)&rsa_key, rsa_key.key_length, &p_rsa_ctx->pkc_ctx);
        if (ER_OK == err)
        {
            p_job->process_phase = PHASE_3;
        }
        else if (ER_ML_PRNG_STATUS == err)
        {
            err = (uint32_t)SERV_NO_INIT_RNG;
        }
        else
        {
            err = (uint32_t)SERV_CRYPT_ERROR;
        }
        /* end of switch (err) */
    }
    /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0043]:[ICUMFW_UD_S_01_PK_0043]
* Function name: s_01_RSA_VERIFY_CalcExpMod
* Description  : Calculation part of verifying RSA signature service
* Arguments    : (IN-OUT)  p_job
* Return Value : SERV_OK
*                SERV_CRYPT_ERROR
*                SERV_NO_INIT_RNG
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_RSA_VERIFY_CalcExpMod(icum_job_t *p_job)
{
    job_rsa_t  *p_job_param;
    rsa_ctx_t  *p_rsa_ctx;

    uint32_t    err;
    st_pkc_operate_opt_t operate_opt;
    
    /* retrieve the service handler */
    p_job_param   = &p_job->prm.job_rsa;
    p_rsa_ctx  = (rsa_ctx_t  *)p_job_param->p_rsa_ctx;

    switch((uint32_t)p_job->process_phase)
    {
        case PHASE_2:

            /* ************************************************************************* */
            /* Initialize the modular exponentiation. all at once */
            /* ************************************************************************* */
            err = s_01_RSA_VERIFY_RsaInit(p_job);
            break;

        case PHASE_3:
            /* ************************************************************************* */
            /* Process the modular exponentiation. all at once */
            /* ************************************************************************* */

            /* RSA Update */
            if (RSA_PROC_SYNC == (uint32_t)p_job_param->proc_type)
            {
                operate_opt.sync_opt = PKC_SYNC_OPT_SYNC;
                operate_opt.processing_bits = L_PKC_PROCESSING_BITS_MAX;
            }
            else
            {
                operate_opt.sync_opt = PKC_SYNC_OPT_ASYNC;
                operate_opt.processing_bits = NUM1;
            }
            err = L_PKC_RsaVerifyOperate(&p_rsa_ctx->pkc_ctx, &operate_opt);
            if (ER_OK == err)
            {
                if (RSA_PROC_SYNC == (uint32_t)p_job_param->proc_type)
                {
                    p_job->process_phase = PHASE_4;
                }
                else
                {
                    p_job->process_phase = PHASE_3DIV;
                }
            }
            else if (ER_IN_PROCESS == err)
            {
                err = ER_OK;
            }
            else
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
            }
            break;

        case PHASE_3DIV:
            /* ************************************************************************* */
            /* Synchronization wait for modular exponentiation processing */
            /* ************************************************************************* */

            /* RSA Update */
            err = L_PKC_RsaChkPkccBusy();
            if (ER_IN_PROCESS == err)
            {
                err = ER_OK;
            }
            else if (ER_OK == err)
            {
                p_job->process_phase = PHASE_4;
            }
            else
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
            }
            break;

        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    
    return (err);
}


/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0044]:[ICUMFW_UD_S_01_PK_0044]
* Function name: s_01_RSA_VERIFY_VerifySign
* Description  : Verification part of verifying RSA signature service
* Arguments    : (IN-OUT)  p_job
* Return Value : SERV_OK
*                SERV_CRYPT_ERROR
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_RSA_VERIFY_VerifySign(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_VERIFY_t   *p_ISD_PARAM;
    
    rsa_ctx_t  *p_rsa_ctx;
    uint32_t    err;
    
    /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    p_ISD_PARAM   = &p_ISD->prm.RSA_VERIFY;
    
    /* get the pointer the the RSA public key used */
    p_rsa_ctx  = (rsa_ctx_t  *)p_JOB_PARAM->p_rsa_ctx;
    
    switch ((uint32_t)p_job->process_phase)
    {
        case PHASE_4:
            /* ************************************************************************* */
            /* Compare the encoded hash and the plain hash calculated over the message */
            /* ************************************************************************* */
            err = L_PKC_RsaVerifyFinal(&p_rsa_ctx->pkc_ctx);
            if (ER_OK == err)
            {
                p_ISD_PARAM->verification_result = VERIFICATION_PASS;
            }
            else if (ER_L_RSA_VERIFY_FAILED == err)
            {
                p_ISD_PARAM->verification_result = VERIFICATION_FAIL;
                err = ER_OK;
            }
            else
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
            }

            /* --- Free RSA_Key -------- */
            (void)MH_KEY_FreeKeyCache(p_job, p_ISD_PARAM->key_group, p_ISD_PARAM->key_id);
            p_JOB_PARAM->resource_ctrl &= ~GET_KEY_RSA;

            /* --- Free RSA_Context ---- */
            (void)ML_CTX_Free(CTX_TYPE_RSA, p_JOB_PARAM->p_rsa_ctx);
            p_JOB_PARAM->resource_ctrl &= ~GET_CTX_RSA;

                /* --- Free HASH Context --- */
                (void)ML_CTX_Free(CTX_TYPE_HASH, p_JOB_PARAM->p_hash_ctx);
                p_JOB_PARAM->resource_ctrl &= ~GET_CTX_HASH;

            /* done! */
            p_job->process_phase = ON_HOLD;
            break;

        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    
    return (err);
}



/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0045]:[ICUMFW_UD_S_01_PK_0045]
* Function name: s_01_RSA_VERIFY_Finish
* Description  : Finish part of verifying RSA signature service
* Arguments    : (IN-OUT)  p_job
*                (IN)  err
* Return Value : none
****************************************************************************************/
static void s_01_RSA_VERIFY_Finish(icum_job_t *p_job, const uint32_t err)
{
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_VERIFY_t   *p_ISD_PARAM;
    
    /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    p_ISD_PARAM   = &p_ISD->prm.RSA_VERIFY;
    
    if ((uint32_t)SERV_OK != err)
    {
        /* error: terminate the service */
        p_ISD->service_result = (r_icumif_service_result_t)err;
        p_job->process_phase = ON_HOLD;

        /* free context when illegal end */
        if (GET_CTX_RSA == ((uint32_t)p_JOB_PARAM->resource_ctrl & GET_CTX_RSA))
        {
            (void)ML_CTX_Free(CTX_TYPE_RSA, p_JOB_PARAM->p_rsa_ctx);
        }
        else
        {
            /* do nothing */
        }
        if (GET_CTX_HASH == ((uint32_t)p_JOB_PARAM->resource_ctrl & GET_CTX_HASH))
        {
            (void)ML_CTX_Free(CTX_TYPE_HASH, p_JOB_PARAM->p_hash_ctx);
        }
        else
        {
            /* do nothing */
        }
        if (GET_KEY_RSA == ((uint32_t)p_JOB_PARAM->resource_ctrl & GET_KEY_RSA))
        {
            (void)MH_KEY_FreeKeyCache(p_job, p_ISD_PARAM->key_group, p_ISD_PARAM->key_id);
        }
        else
        {
            /* do nothing */
        }
        p_JOB_PARAM->resource_ctrl = GET_CTX_NONE;
    }
    else
    {
        /* all good - terminate the service or keep it alive */
        /* depending on the processing phase */
        if (ON_HOLD == (uint32_t)p_job->process_phase)
        {
            p_ISD->service_result = SERV_OK;
        }
        else
        {
            /* do nothing */
        }
    }
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0013]:[ICUMFW_UD_S_01_PK_0013]
* Function name: S_01_VerifyRsaSignature
* Description  : Verify RSA signature Service
* Arguments    : (IN-OUT)  p_job
* Return Value : none
****************************************************************************************/
void S_01_VerifyRsaSignature(icum_job_t *p_job)
{
    /* retrieve the service handler */
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;

    uint32_t    err;
    
    /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    
    err = ER_OK;

    /**********************************************************************************
    * Initialization phase
    **********************************************************************************/

    /* already initialized? */
    if (SERV_REQUEST == p_ISD->service_result)
    {
        p_JOB_PARAM->resource_ctrl = GET_CTX_NONE;
    }
    else
    {
        /* do nothing */
    }

    /* indicate service is in process */
    p_ISD->service_result = SERV_IN_PROCESS;

    switch ((uint32_t)p_job->process_phase)
    {
        case ON_HOLD:
            err = s_01_RSA_VERIFY_Init(p_job);
            break;
        case PHASE_H1:
        case PHASE_H2:
        case PHASE_H2DIV:
            err = s_01_RSA_VERIFY_GenHash(p_job);
            break;
        case PHASE_1:
            err = s_01_RSA_VERIFY_PrepareSign(p_job);
            break;
        case PHASE_2:
        case PHASE_3:
        case PHASE_3DIV:
            err = s_01_RSA_VERIFY_CalcExpMod(p_job);
            break;
        case PHASE_4:
            err = s_01_RSA_VERIFY_VerifySign(p_job);
            break;
        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }

    if (ER_OK == err)
    {
        err = (uint32_t)SERV_OK;
    }
    else
    {
        /* do nothing */
    }

    s_01_RSA_VERIFY_Finish(p_job, err);
}
