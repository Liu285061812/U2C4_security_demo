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
#include "icum_service_common.h"


/**********************************************************************************
* Local define
**********************************************************************************/
/* unsigned data size */
#define NUM2                           (2u)
#define NUM3                           (3u)
#define NUM7                           (7u)
#define NUM8                           (8u)

/**********************************************************************************
* Functions
**********************************************************************************/
static uint32_t s_01_InitializeJobRsaDecrypt(icum_job_t *p_job);
static uint32_t s_01_InitDecryptByRsa(icum_job_t *p_job, uint32_t *p_skip_flg);
static uint32_t s_01_PrepareDecryptByRsa(icum_job_t *p_job);
static uint32_t s_01_CalcExpModDecryptByRsa(icum_job_t *p_job);
static void s_01_FinishDecryptByRsa(icum_job_t *p_job, const uint32_t err);
static uint32_t s_01_InitializeJobRsaDecryptSub(icum_job_t *p_job, uint32_t input_msg_size_in_bytes, rsa_key_t *p_key);

/*===========================================================================
  Run a RSA message decryption
  Command parameters extracted from the ICU-M service descriptor (ISD)
  [PUBLIC]
  ===========================================================================*/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0008]:[ICUMFW_UD_S_01_PK_0008]
* Function name: s_01_InitializeJobRsaDecrypt
* Description  : Initialization processing
* Arguments    : *p_job                         : Pointer of icum_job_t
*                                                 Range : 0x0 - 0xFFFFFFFF
*                hash_primitive                 : HASH value generation algorithm
*                                                 Range : HASH_PRIMITIVE_SHA2_224
*                                                         HASH_PRIMITIVE_SHA2_256
*                                                         HASH_PRIMITIVE_SHA2_384
*                                                         HASH_PRIMITIVE_SHA2_512
*                                                         HASH_PRIMITIVE_SHA2_512_224
*                                                         HASH_PRIMITIVE_SHA2_512_256
*                message_encoding_format        : padding algorithm
*                                                 Range : ENCODING_FORMAT_EME_OAEP
*                                                         ENCODING_FORMAT_EME_PKCS1_V15
*                key_id                         : Index number in key group KEY_GRP_RSA
*                                                 Range : 0x0 - 0xFF
*                encrypted_message_format       : Format of encrypted data
*                                                 Range : FORMAT_STRING_DECIMAL
*                                                         FORMAT_STRING_HEXA
*                                                         FORMAT_BINARY_PLAIN
*                *p_encrypted_message           : Start address of encrypted data
*                                                 Range : 0x0 - 0xFFFFFFFF
*                *p_message                     : Start address of decrypted data storage location
*                                                 Range : 0x0 - 0xFFFFFFFF
*                message_size_in_bytes          : Decoded data size (in bytes)
*                                                 Range : 0x0 - 0xFFFFFFFF
*                job_slice_mod_exp              : A flag indicating whether to process by units of 1 bit or all bits
*                                                 when implementing modular exponentiation
*                                                 Range : Other than "0" or "0"
*                encrypted_message_size_in_bits : Encrypted data size (in bit units)
*                                                 Range : 512 - 3072
* Return Value : SERV_OK
*                SERV_PENDING
*                SERV_INVALID_ENCODING_FORMAT
*                SERV_KEY_SLOT_EMPTY
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_KEY_NOT_USABLE
*                SERV_NVM_UNAVAILABLE
*                SERV_PRM_ERROR
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
***************************************************************************************/
static uint32_t s_01_InitializeJobRsaDecrypt(icum_job_t *p_job)
{
    /* retrieve the service handler */
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_DECRYPT_t  *p_ISD_PARAM;

    rsa_key_t       *p_key;
    hash_type_t     hash_type;
    uint8_t         is_allowed;
    uint32_t        input_msg_size_in_bytes;
    uint16_t        n;
    uint16_t        max;
    uint32_t        err;

    err = ER_OK;

    p_ISD       = p_job->p_ISD;
    p_JOB_PARAM = &p_job->prm.job_rsa;
    p_ISD_PARAM = &p_ISD->prm.RSA_DECRYPT;

    p_key = (rsa_key_t *)0;

    /* make sure message_encoding_format */
    switch (p_ISD_PARAM->message_encoding_format)
    {
        case ENCODING_FORMAT_EME_OAEP:
            (void)L_HASH_GetInternalHashType(p_ISD_PARAM->hash_primitive, &hash_type, &n);
            if (0u == (uint32_t)n)
            {
                err = (uint32_t)SERV_PRM_ERROR;
            }
            else
            {
                /* do nothing */
            }
            break;
        case ENCODING_FORMAT_EME_PKCS1_V15:
            break;
        default:
            err = (uint32_t)SERV_INVALID_ENCODING_FORMAT;
            break;
    } /* end of switch (p_ISD_PARAM->message_encoding_format)*/

    if (ER_OK == err)
    {
        /* retrieve the key to be used */
        err = MH_KEY_GetRsaKey(p_job, KEY_GRP_RSA, p_ISD_PARAM->key_id, &p_key);
        (void)s_01_RsaKeyConvertErr(&err);
        
        if (ER_OK == err)
        {
            p_JOB_PARAM->resource_ctrl |= GET_KEY_RSA;
            (void)s_01_CanUseKeyIfDbgConnected((key_flags_t)p_key->p_properties->flags.disable_on_debug,
                                               &is_allowed);
            if (TRUE == (uint32_t)is_allowed)
            {
                    /* do nothing */
            }
            else
            {
                /* key not usable: abort */
                err = (uint32_t)SERV_KEY_NOT_USABLE;
            }
        } /* end of if (ER_OK == err) */
        else
        {
            /* do nothing */
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* do nothing */
    }

    if (ER_OK == err)
    {
        /* calculate size of input buffer (bytes) */
        switch (p_ISD_PARAM->encrypted_message_format)
        {
            case FORMAT_STRING_DECIMAL:
                /* get decimal figure length from bit_length. */
                (void)s_01_GetDecimalStrlenFromBitCnt(p_ISD_PARAM->encrypted_message_size_in_bits, &max);
                input_msg_size_in_bytes = max;
                /* increment for terminator(NULL) */
                input_msg_size_in_bytes++;
                break;
            case FORMAT_STRING_HEXA:
                input_msg_size_in_bytes = ((uint32_t)p_ISD_PARAM->encrypted_message_size_in_bits + NUM7) / NUM8;
                input_msg_size_in_bytes *= NUM2;
                /* increment for terminator(NULL) */
                input_msg_size_in_bytes++;
                break;
            case FORMAT_BINARY_PLAIN:
                input_msg_size_in_bytes = ((uint32_t)p_ISD_PARAM->encrypted_message_size_in_bits + NUM7) / NUM8;
                break;
            default:
                /* error on number format */
                err = (uint32_t)SERV_PRM_ERROR;
                break;
        } /* end of switch (p_ISD_PARAM->encrypted_message_format)*/
    } /* end of if (ER_OK == err) */
    else
    {
        /* do nothing */
    }

    if (ER_OK == err)
    {
        err = s_01_InitializeJobRsaDecryptSub(p_job, input_msg_size_in_bytes, p_key);
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
* Function ID  : [ICUMFW_CD_S_01_PK_0030]:[ICUMFW_UD_S_01_PK_0030]
* Function name: s_01_InitDecryptByRsa
* Description  : Initialization part of RSA decryption service
* Arguments    : (IN-OUT)  p_job
*              : (OUT) p_skip_flg
* Return Value : SERV_OK
*                SERV_PENDING
*                SERV_INVALID_ENCODING_FORMAT
*                SERV_KEY_SLOT_EMPTY
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_KEY_NOT_USABLE
*                SERV_NVM_UNAVAILABLE
*                SERV_PRM_ERROR
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_InitDecryptByRsa(icum_job_t *p_job, uint32_t *p_skip_flg)
{
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_DECRYPT_t  *p_isd_param;
    uint32_t    err;
    
    /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    p_isd_param   = &p_ISD->prm.RSA_DECRYPT;
    
    err = ER_OK;
    
    switch ((uint32_t)p_job->process_phase)
    {
        case ON_HOLD:
            if (GET_CTX_NONE == ((uint32_t)p_JOB_PARAM->resource_ctrl & GET_CTX_RSA))
            {
                /* get rsa context */
                err = ML_CTX_Get(CTX_TYPE_RSA, &p_JOB_PARAM->p_rsa_ctx);
                if (ER_ML_CTX_ALL_USE == err)
                {
                    p_ISD->service_result = SERV_PENDING;
                    *p_skip_flg = FLG_ON;
                }
                else if (ER_OK != err)
                {
                    /* should never occur */
                    err = (uint32_t)SERV_SYS_ERROR;
                    break;
                }
                else
                {
                    p_JOB_PARAM->resource_ctrl |= GET_CTX_RSA;
                    p_ISD->service_result = SERV_IN_PROCESS;
                }
            }
            else
            {
                /* do nothing */
            }

            if (ENCODING_FORMAT_EME_OAEP == p_isd_param->message_encoding_format)
            {
                if (GET_CTX_NONE == ((uint32_t)p_JOB_PARAM->resource_ctrl & GET_CTX_HASH))
                {
                    err = ML_CTX_Get(CTX_TYPE_HASH, &p_JOB_PARAM->p_hash_ctx);
                    if (ER_ML_CTX_ALL_USE == err)
                    {
                        p_ISD->service_result = SERV_PENDING;
                        *p_skip_flg = FLG_ON;
                    }
                    else if (ER_OK != err)
                    {
                        /* should never occur */
                        err = (uint32_t)SERV_SYS_ERROR;
                        break;
                    }
                    else
                    {
                        p_JOB_PARAM->resource_ctrl |= GET_CTX_HASH;
                        p_ISD->service_result = SERV_IN_PROCESS;
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

            /* ************************************************************************* */
            /* Initialization */
            /* ************************************************************************* */
            if (FLG_OFF == *p_skip_flg)
            {
                err = s_01_InitializeJobRsaDecrypt(p_job);
                if (err != (uint32_t)SERV_OK)
                {
                    /* init error - break here */
                    break;
                }
                else
                {
                    /* do nothing */
                }

                p_job->process_phase = PHASE_1;
            }
            else
            {
                /* do nothing */
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
* Function ID  : [ICUMFW_CD_S_01_PK_0031]:[ICUMFW_UD_S_01_PK_0031]
* Function name: s_01_PrepareDecryptByRsa
* Description  : Preparation part of RSA decryption service
* Arguments    : (IN-OUT)  p_job
* Return Value : ER_OK
*                SERV_CRYPT_ERROR
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_PrepareDecryptByRsa(icum_job_t *p_job)
{
    /* retrieve the service handler */
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_DECRYPT_t  *p_ISD_PARAM;
    
    rsa_ctx_t  *p_rsa_ctx;
    
    uint32_t    err;
    big_num_type_t  temp_type;
    uint32_t        bn_area_len;
    remap_ptr_manip_t   encrypted_message_logical;
    
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    p_ISD_PARAM   = &p_ISD->prm.RSA_DECRYPT;
    
    /* get the pointer the the RSA public key used */
    p_rsa_ctx  = (rsa_ctx_t  *)p_JOB_PARAM->p_rsa_ctx;
    
    temp_type   = E_BINARY_PLAIN;
    bn_area_len = 0u;
    
    /* Get Remap(Convert Physical address -> Logical Address) */
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD_PARAM->p_encrypted_message,
            &encrypted_message_logical.val);

    switch ((uint32_t)p_job->process_phase)
    {
        case PHASE_1:
            /* ************************************************************************* */
            /* Prepare the ciphered message for processing                               */
            /* ************************************************************************* */
            /* check format of the encrypted message                                     */
            switch (p_ISD_PARAM->encrypted_message_format)
            {
                case FORMAT_STRING_DECIMAL:
                    temp_type = E_STRING_DECIMAL;
                    bn_area_len = L_Toolbox_StrLen((const char *)encrypted_message_logical.p_ptr);
                    break;
                case FORMAT_STRING_HEXA:
                    temp_type = E_STRING_HEXA;
                    bn_area_len = L_Toolbox_StrLen((const char *)encrypted_message_logical.p_ptr);
                    break;
                case FORMAT_BINARY_PLAIN:
                    temp_type = E_BINARY_PLAIN;
                    bn_area_len = (((uint32_t)p_ISD_PARAM->encrypted_message_size_in_bits + NUM7) >> NUM3);
                    break;
                default:
                    /* should never occur */
                    break;
            }

            /* convert encrypted_message to a big number */
            /* use W for temporary big_num */
            err = L_BIGNUM_EncodeToBigNum(encrypted_message_logical.p_ptr,
                    temp_type, bn_area_len, &p_rsa_ctx->W);
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
* Function ID  : [ICUMFW_CD_S_01_PK_0032]:[ICUMFW_UD_S_01_PK_0032]
* Function name: s_01_CalcExpModDecryptByRsa
* Description  : Calculation part of RSA decryption service
* Arguments    : (IN-OUT)  p_job
* Return Value : SERV_OK
*                SERV_CRYPT_ERROR
*                SERV_NO_INIT_RNG
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_CalcExpModDecryptByRsa(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_isd;
    job_rsa_t          *p_job_param;
    ISD_RSA_DECRYPT_t  *p_isd_param;
    rsa_key_t  *p_key;
    rsa_ctx_t  *p_rsa_ctx;
    uint32_t    err;
    st_pkc_data_t       plain_data;
    st_pkc_data_t       cipher;
    st_pkcd_rsa_key_bin_t rsa_key;
    st_pkc_operate_opt_t operate_opt;
    remap_ptr_manip_t   message_logical;
    remap_ptr_manip_t   label_logical;

    /* retrieve the service handler */
    p_isd         = p_job->p_ISD;
    p_job_param   = &p_job->prm.job_rsa;
    p_isd_param   = &p_isd->prm.RSA_DECRYPT;
    p_rsa_ctx     = (rsa_ctx_t *)p_job_param->p_rsa_ctx;
    p_key         = (rsa_key_t *)p_job_param->p_key;

    err = ER_OK;

    /* Initialize parameters for PKC API */
    L_Toolbox_MemSet((uint8_t *)&rsa_key, NUM0, (int16_t)sizeof(st_pkcd_rsa_key_bin_t));
    
    switch ((uint32_t)p_job->process_phase)
    {
        case PHASE_2:
            /* ************************************************************************* */
            /* Initialize the modular exponentiation.  */
            /* ************************************************************************* */

            (void)L_PKC_RsaCryptSetup(p_rsa_ctx, p_isd_param->message_encoding_format,
                                        (hash_ctx_t *)p_job_param->p_hash_ctx, p_isd_param->hash_primitive);

            /* Get Remap(Convert Physical address -> Logical Address) */
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_param->p_label, &label_logical.val);
            /* Get Remap(Convert Physical address -> Logical Address) */
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_param->p_message, &message_logical.val);
            p_rsa_ctx->pkc_ctx.label.p_data = (uint8_t *)label_logical.p_ptr;
            p_rsa_ctx->pkc_ctx.label.len = p_isd_param->label_size_in_bytes;

            cipher.p_data = (uint8_t *)p_rsa_ctx->W.value;
            cipher.len = p_rsa_ctx->W.size;

            rsa_key.key_length = (uint32_t)p_key->p_properties->modulus_size_in_bytes;
            rsa_key.p_n = p_key->p_modulus;
            rsa_key.p_d = p_key->p_priv_exponent;

            err = L_PKC_RsaDecryptInit(&cipher, &rsa_key, rsa_key.key_length, &p_rsa_ctx->pkc_ctx);
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
            break;

        case PHASE_3:
            /* ************************************************************************* */
            /* Process the modular exponentiation. all at once */
            /* ************************************************************************* */
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
            err = L_PKC_RsaDecryptOperate(&p_rsa_ctx->pkc_ctx, &operate_opt);
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

        case PHASE_4:
            /* ************************************************************************* */
            /* Get the result of the modular exponentiation */
            /* i.e. the encoded hash */
            /* ************************************************************************* */
            plain_data.p_data = (uint8_t *)&p_rsa_ctx->W.value;
            plain_data.len = (uint32_t)p_rsa_ctx->W.size;
            err = L_PKC_RsaDecryptFinal(&p_rsa_ctx->pkc_ctx, &plain_data);
            if (ER_OK == err)
            {
                (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_param->p_message, &message_logical.val);
                L_Toolbox_MemCpy((uint8_t *)message_logical.p_ptr,
                                    (const uint8_t *)plain_data.p_data, (int16_t)plain_data.len);
                p_isd_param->message_size_in_bytes = plain_data.len;
                p_job->process_phase = ON_HOLD;
            }
            else
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
                break;
            }

            /* --- Free RSA Key -------- */
            (void)MH_KEY_FreeKeyCache(p_job, KEY_GRP_RSA, p_isd_param->key_id);
            p_job_param->resource_ctrl &= ~GET_KEY_RSA;
            /* --- Free RSA Context ---- */
            (void)ML_CTX_Free(CTX_TYPE_RSA, p_job_param->p_rsa_ctx);
            p_job_param->resource_ctrl &= ~GET_CTX_RSA;
            /* --- Free Hash Context ---- */
            (void)ML_CTX_Free(CTX_TYPE_HASH, p_job_param->p_hash_ctx);
            p_job_param->resource_ctrl &= ~GET_CTX_HASH;
            break;
        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    
    return (err);
}


/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0034]:[ICUMFW_UD_S_01_PK_0034]
* Function name: s_01_FinishDecryptByRsa
* Description  : Finish part of RSA decryption service
* Arguments    : (IN-OUT)  p_job
*                (IN)  error
* Return Value : none
****************************************************************************************/
static void s_01_FinishDecryptByRsa(icum_job_t *p_job, const uint32_t err)
{
    /* retrieve the service handler */
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_DECRYPT_t  *p_ISD_PARAM;
    
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    p_ISD_PARAM   = &p_ISD->prm.RSA_DECRYPT;
    
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
            (void)MH_KEY_FreeKeyCache(p_job, KEY_GRP_RSA, p_ISD_PARAM->key_id);
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

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0051]:[ICUMFW_UD_S_01_PK_0051]
* Function name: s_01_InitializeJobRsaDecryptSub
* Description  : Initialization processing Sub
* Arguments    : *p_job                         : Pointer of icum_job_t
*                                                 Range : 0x0 - 0xFFFFFFFF
*                hash_primitive                 : HASH value generation algorithm
*                                                 Range : HASH_PRIMITIVE_SHA2_224
*                                                         HASH_PRIMITIVE_SHA2_256
*                                                         HASH_PRIMITIVE_SHA2_384
*                                                         HASH_PRIMITIVE_SHA2_512
*                                                         HASH_PRIMITIVE_SHA2_512_224
*                                                         HASH_PRIMITIVE_SHA2_512_256
*                message_encoding_format        : padding algorithm
*                                                 Range : ENCODING_FORMAT_EME_OAEP
*                                                         ENCODING_FORMAT_EME_PKCS1_V15
*                key_id                         : Index number in key group KEY_GRP_RSA
*                                                 Range : 0x0 - 0xFF
*                encrypted_message_format       : Format of encrypted data
*                                                 Range : FORMAT_STRING_DECIMAL
*                                                         FORMAT_STRING_HEXA
*                                                         FORMAT_BINARY_PLAIN
*                *p_encrypted_message           : Start address of encrypted data
*                                                 Range : 0x0 - 0xFFFFFFFF
*                *p_message                     : Start address of decrypted data storage location
*                                                 Range : 0x0 - 0xFFFFFFFF
*                message_size_in_bytes          : Decoded data size (in bytes)
*                                                 Range : 0x0 - 0xFFFFFFFF
*                job_slice_mod_exp              : A flag indicating whether to process by units of 1 bit or all bits
*                                                 when implementing modular exponentiation
*                                                 Range : Other than "0" or "0"
*                encrypted_message_size_in_bits : Encrypted data size (in bit units)
*                                                 Range : 512 - 3072
*                input_msg_size_in_bytes        : input msg size size (in bytes)
*                                                 Range : 384 - 925
*                p_key                          : Index number in key group KEY_GRP_RSA
*                                                 Range : 0x0 - 0xFF
* Return Value : SERV_OK
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
*                SERV_PRM_ERROR
***************************************************************************************/
static uint32_t s_01_InitializeJobRsaDecryptSub(icum_job_t *p_job, uint32_t input_msg_size_in_bytes, rsa_key_t *p_key)
{
    /* retrieve the service handler */
    r_icumif_isd_t     *p_isd;
    job_rsa_t          *p_job_param;
    ISD_RSA_DECRYPT_t  *p_isd_param;
    uint8_t             mem_check_result;
    remap_ptr_manip_t   encrypted_message_logical;
    uint8_t             string_check;
    uint32_t            err;

    p_isd         = p_job->p_ISD;
    p_job_param   = &p_job->prm.job_rsa;
    p_isd_param   = &p_isd->prm.RSA_DECRYPT;

    err = ER_OK;

    /* make sure the source address range is valid */
    (void)MM_FLS_CheckMemory((uint32_t)p_isd_param->p_encrypted_message,
            input_msg_size_in_bytes, FLS_CHK_MEM_SOURCE,
            p_job->expansion_service, &mem_check_result);
    if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_check_result)
    {
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_param->p_encrypted_message,
                &encrypted_message_logical.val);

        /* check string format */
        string_check = TRUE;
        if (FORMAT_STRING_DECIMAL == p_isd_param->encrypted_message_format)
        {
            /* check decimal string. */
            (void)s_01_IsDigitString(encrypted_message_logical.p_ptr, input_msg_size_in_bytes, &string_check);
        }
        else if (FORMAT_STRING_HEXA == p_isd_param->encrypted_message_format)
        {
            /* check hexadecimal string. */
            (void)s_01_IsHexDigitString(encrypted_message_logical.p_ptr, input_msg_size_in_bytes, &string_check);
        }
        else
        {
            /* do nothing */
        }

        if (TRUE == (uint32_t)string_check)
        {
            /* make sure the destination address range is valid */
            (void)MM_FLS_CheckMemory((uint32_t)p_isd_param->p_message,
                    p_isd_param->message_size_in_bytes, FLS_CHK_MEM_DESTINATION,
                    p_job->expansion_service, &mem_check_result);
            if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_check_result)
            {
                if ((ENCODING_FORMAT_EME_OAEP == p_isd_param->message_encoding_format) &&
                    (NUM0 < p_isd_param->label_size_in_bytes))
                {
                    /* Label : make sure the source address range is valid */
                    (void)MM_FLS_CheckMemory((uint32_t)p_isd_param->p_label,
                            p_isd_param->label_size_in_bytes, FLS_CHK_MEM_SOURCE,
                            p_job->expansion_service, &mem_check_result);
                    if ((uint32_t)FLS_CHK_OK != (uint32_t)mem_check_result)
                    {
                        err = (uint32_t)SERV_SRC_MEM_ERROR;
                    }
                    else
                    {
                        /* init the job slice */
                        p_job_param->job_slice = p_isd_param->job_slice_mod_exp;
                        p_job_param->p_key = (uint8_t *)p_key;
                    }
                } /* end of if ((ENCODING_FORMAT_EME_OAEP == p_ISD_PARAM->message_encoding_format) && ...*/
                else
                {
                    /* init the job slice */
                    p_job_param->job_slice = p_isd_param->job_slice_mod_exp;
                    p_job_param->p_key = (uint8_t *)p_key;
                }
            } /* end of if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_check_result) */
            else
            {
                err = (uint32_t)SERV_DEST_MEM_ERROR;
            }  /* end of else */
        } /* end of if (TRUE == (uint32_t)string_check) */
        else
        {
            err = (uint32_t)SERV_PRM_ERROR;
        } /* end of else */
    } /* end of if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_check_result) */
    else
    {
        err = (uint32_t)SERV_SRC_MEM_ERROR;
    } /* end of else */
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0007]:[ICUMFW_UD_S_01_PK_0007]
* Function name: S_01_DecryptByRsa
* Description  : Service that decryption by RSA
* Arguments    : *p_job                         : Pointer of icum_job_t
*                                                 Range : 0x0 - 0xFFFFFFFF
*                *p_message                     : Start address of decrypted data storage location
*                                                 Range : 0x0 - 0xFFFFFFFF
*                message_size_in_bytes          : Decoded data size (in bytes)
*                                                 Range : 0x0 - 0xFFFFFFFF
*                *p_encrypted_message           : Start address of encrypted data
*                                                 Range : 0x0 - 0xFFFFFFFF
*                encrypted_message_size_in_bits : Encrypted data size (in bit units)
*                                                 Range : 512 - 3072
*                *p_label                       : Start address of label storage location
*                                                 Range : 0x0 - 0xFFFFFFFF
*                label_size_in_bytes            : Start Label size (in bytes)
*                                                 Range : 0x0 - 0xFFFFFFFF
*                hash_primitive                 : HASH value generation algorithm
*                                                 Range : HASH_PRIMITIVE_SHA2_224
*                                                         HASH_PRIMITIVE_SHA2_256
*                                                         HASH_PRIMITIVE_SHA2_384
*                                                         HASH_PRIMITIVE_SHA2_512
*                                                         HASH_PRIMITIVE_SHA2_512_224
*                                                         HASH_PRIMITIVE_SHA2_512_256
*                message_encoding_format        : padding algorithm
*                                                 Range : ENCODING_FORMAT_EME_OAEP
*                                                         ENCODING_FORMAT_EME_PKCS1_V15
*                key_id                         : Index number in key group KEY_GRP_RSA
*                                                 Range : 0x0 - 0xFF
*                encrypted_message_format       : Format of encrypted data
*                                                 Range : FORMAT_STRING_DECIMAL
*                                                         FORMAT_STRING_HEXA
*                                                         FORMAT_BINARY_PLAIN
*                job_slice_mod_exp              : A flag indicating whether to process by units of 1 bit or all bits
*                                                 when implementing modular exponentiation
*                                                 Range : Other than "0" or "0"
*                service_result                 : Error code
*                                                 Range : SERV_OK
*                                                         SERV_IN_PROCESS
*                                                         SERV_PENDING
*                                                         SERV_INVALID_ENCODING_FORMAT
*                                                         SERV_KEY_SLOT_EMPTY
*                                                         SERV_INVALID_KEY_GROUP
*                                                         SERV_INVALID_KEY_INDEX
*                                                         SERV_KEY_NOT_USABLE
*                                                         SERV_NVM_UNAVAILABLE
*                                                         SERV_PRM_ERROR
*                                                         SERV_SRC_MEM_ERROR
*                                                         SERV_DEST_MEM_ERROR
*                                                         SERV_SYS_ERROR
*                                                         SERV_CRYPT_ERROR
*                                                         SERV_NO_INIT_RNG
* Return Value : none
***************************************************************************************/
void S_01_DecryptByRsa(icum_job_t *p_job)
{
    /* retrieve the service handler */
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;

    uint32_t    err;
    uint32_t    skip_flg;

    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    err = ER_OK;
    skip_flg = FLG_OFF;
    
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
            err = s_01_InitDecryptByRsa(p_job, &skip_flg);
            break;
        case PHASE_1:
            err = s_01_PrepareDecryptByRsa(p_job);
            break;
        case PHASE_2:
        case PHASE_3:
        case PHASE_3DIV:
        case PHASE_4:
            err = s_01_CalcExpModDecryptByRsa(p_job);
            break;
        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }

    if (FLG_OFF == skip_flg)
    {
        if (ER_OK == err)
        {
            err = (uint32_t)SERV_OK;
        }
        else
        {
            /* do nothing */
        }
        s_01_FinishDecryptByRsa(p_job, err);
    }
    else
    {
        /* do nothing */
    }
}

