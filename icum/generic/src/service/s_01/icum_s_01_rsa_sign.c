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
/*******************************/
/* EMSA PSS : salt buffer size */
/*******************************/
/* Data length after padding equals to size of RSA Key. It contains size of hash and fixed area. */
/* Minimum of hash size is 16 bytes(MD5, RIPEMD_128). fix area uses 2 bytes. */
/* So, salt buffer area uses (sizeof(RSA Key)-16-2) bytes, at maximum. */
/* if BIGNUM_BITSIZE is 3072, maximum of salt buffer size is (3072/8-16-2 = 366) bytes. */
/* count of long words for salt buffer is 366 divided by 4. (round up) */
/* in this case, salt buffer requires 92 longwords. */
#define SALT_BUFFER_SIZE        ((((R_CRYPT_BIGNUM_BITSIZE / 8u) - 16u - 2u) + 3u) / 4u)

/* unsigned data size */
#define NUM2                           (2u)
#define NUM4                           (4u)
#define NUM7                           (7u)
#define NUM8                           (8u)
#define NUM10                         (10u)
#define NUM512                       (512u)

/**********************************************************************************
* Functions
**********************************************************************************/
static uint32_t s_01_InitializeJobRsaSign(icum_job_t *p_job);
static uint32_t s_01_RSA_SIGN_Init(icum_job_t *p_job);
static uint32_t s_01_RSA_SIGN_GenHash(icum_job_t *p_job);
static uint32_t s_01_RSA_SIGN_CalcExpMod(icum_job_t *p_job);
static void s_01_RSA_SIGN_Finish(icum_job_t *p_job, const uint32_t error);
static uint32_t s_01_InitExpMod(icum_job_t *p_job);
static uint32_t s_01_UpdateOnceExpMod(icum_job_t *p_job);
static uint32_t s_01_UpdateSlicesExpMod(icum_job_t *p_job);
static uint32_t s_01_FinishExpMod(icum_job_t *p_job);

/*===========================================================================
  Run a RSA signature generate
  Command parameters extracted from the ICU-M service descriptor (ISD)
  [PUBLIC]
  ===========================================================================*/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0011]:[ICUMFW_UD_S_01_PK_0011]
* Function name: s_01_InitializeJobRsaSign
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
*                SERV_DEST_MEM_ERROR
****************************************************************************************/
static uint32_t s_01_InitializeJobRsaSign(icum_job_t *p_job)
{
    /* retrieve the service handler */
    r_icumif_isd_t     *p_isd;
    job_rsa_t          *p_job_param;
    ISD_RSA_SIGN_t     *p_isd_param;

    rsa_key_t       *p_key;
    hash_type_t     hash_type;
    uint8_t         is_allowed;
    uint32_t        output_buf_size_in_bytes;
    uint8_t         mem_check_result;
    uint16_t        hash_size_in_bytes;
    uint16_t        max;
    uint32_t        err;

    err = ER_OK;
    p_key = NULL;

    p_isd         = p_job->p_ISD;
    p_job_param   = &p_job->prm.job_rsa;
    p_isd_param   = &p_isd->prm.RSA_SIGN;

    /* make sure hash_encoding_format */
    switch (p_isd_param->hash_encoding_format)
    {
        case ENCODING_FORMAT_EMSA_PSS:
        case ENCODING_FORMAT_EMSA_PKCS1_V15:
            (void)L_HASH_GetInternalHashType(p_isd_param->hash_primitive, &hash_type, &hash_size_in_bytes);
            if (NUM0 == (uint32_t)hash_size_in_bytes)
            {
                err = (uint32_t)SERV_PRM_ERROR;
            }
            else
            {
                /* do nothing */
            }
            break;
        default:
            err = (uint32_t)SERV_INVALID_ENCODING_FORMAT;
            break;
    } /* end of switch (p_ISD_PARAM->hash_encoding_format) */

    if (ER_OK == err)
    {
        /* when hash provided, make sure source_size_in_bytes equals hash length */
        if ((NUM0 != (uint32_t)p_isd_param->is_precalculated_hash) &&
            (p_isd_param->source_size_in_bytes != (uint32_t)hash_size_in_bytes))
           {
                err = (uint32_t)SERV_PRM_ERROR;
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

    if (ER_OK == err)
    {
        /* make sure that salt_size_in_bytes does not exceed the maximum value. */
        if ((uint32_t)p_isd_param->salt_size_in_bytes > (SALT_BUFFER_SIZE * NUM4))
        {
            err = (uint32_t)SERV_PRM_ERROR;
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

    if (ER_OK == err)
    {
        /* retrieve the key to be used */
        err = MH_KEY_GetRsaKey(p_job, KEY_GRP_RSA, p_isd_param->key_id, &p_key);
        (void)s_01_RsaKeyConvertErr(&err);
    }
    else
    {
        /* do nothing */
    }

    if (ER_OK == err)
    {
        p_job_param->resource_ctrl |= GET_KEY_RSA;
    
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

    if (ER_OK == err)
    {
        /* calculate size of output buffer (bytes) */
        switch (p_isd_param->signature_format)
        {
            case FORMAT_STRING_DECIMAL:
                /* get decimal figure length from bit_length. */
                (void)s_01_GetDecimalStrlenFromBitCnt(p_isd_param->signature_size_in_bits, &max);
                output_buf_size_in_bytes = max;

                /* increment for terminator(NULL) */
                output_buf_size_in_bytes++;
                break;
            case FORMAT_STRING_HEXA:
                output_buf_size_in_bytes = ((uint32_t)p_isd_param->signature_size_in_bits + NUM7) / NUM8;
                output_buf_size_in_bytes *= NUM2;

                /* increment for terminator(NULL) */
                output_buf_size_in_bytes++;
                break;
            case FORMAT_BINARY_PLAIN:
                output_buf_size_in_bytes = ((uint32_t)p_isd_param->signature_size_in_bits + NUM7) / NUM8;
                break;
            default:
                /* error on number format */
                err = (uint32_t)SERV_PRM_ERROR;
                break;
        } /* end of switch (p_ISD_PARAM->signature_format) */
    } /* end of if (ER_OK == err) */
    else
    {
        /* do nothing */
    }

    if (ER_OK == err)
    {
        /* make sure the source address range is valid */
        (void)MM_FLS_CheckMemory((uint32_t)p_isd_param->p_source,
                p_isd_param->source_size_in_bytes, FLS_CHK_MEM_SOURCE,
                p_job->expansion_service, &mem_check_result);
        if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_check_result)
        {
            /* make sure the destination address range is valid */
            (void)MM_FLS_CheckMemory((uint32_t)p_isd_param->p_signature,
                    output_buf_size_in_bytes, FLS_CHK_MEM_DESTINATION,
                    p_job->expansion_service, &mem_check_result);
            if ((uint32_t)FLS_CHK_OK != (uint32_t)mem_check_result)
            {
                err = (uint32_t)SERV_DEST_MEM_ERROR;
            }
            else
            {
                /* init the job slice */
                p_job_param->job_slice = p_isd_param->job_slice_mod_exp;
                p_job_param->p_key = (uint8_t *)p_key;
            }
        }
        else
        {
            err = (uint32_t)SERV_SRC_MEM_ERROR;
        }
    } /* end of if (ER_OK == err) */
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
* Function ID  : [ICUMFW_CD_S_01_PK_0035]:[ICUMFW_UD_S_01_PK_0035]
* Function name: s_01_RSA_SIGN_Init
* Description  : Initialization part of generating RSA signature service
* Arguments    : (IN-OUT)  p_job
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
*                SERV_DEST_MEM_ERROR
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_RSA_SIGN_Init(icum_job_t *p_job)
{
    
    /* retrieve the service handler */
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_SIGN_t     *p_ISD_PARAM;
    uint32_t    err;
    
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    p_ISD_PARAM   = &p_ISD->prm.RSA_SIGN;
    
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
            err = s_01_InitializeJobRsaSign(p_job);
            if ((uint32_t)SERV_OK != err)
            {
                /* init error - break here */
                break;
            }
            else
            {
                /* do nothing */
            }

            /* test if the hash has been already calculated */
            /* it could be the case when the service is triggered internally */
            /* e.g. via the memory cluster installation service */
            if (NUM0 != (uint32_t)p_ISD_PARAM->is_precalculated_hash)
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
* Function ID  : [ICUMFW_CD_S_01_PK_0036]:[ICUMFW_UD_S_01_PK_0036]
* Function name: s_01_RSA_SIGN_GenHash
* Description  : Hash generation part of generating RSA signature service
* Arguments    : (IN-OUT)  p_job
* Return Value : SERV_OK
*                SERV_CRYPT_ERROR
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_RSA_SIGN_GenHash(icum_job_t *p_job)
{
    /* retrieve the service handler */
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_SIGN_t     *p_ISD_PARAM;
    uint32_t    err;
    uint8_t     plain_hash_buff[HASH_512_SIZE_IN_BYTES];
    remap_ptr_manip_t   source_logical;
    
    rsa_ctx_t  *p_rsa_ctx;
    hash_ctx_t *p_hash_ctx;
    
    hash_type_t     hash_type;
    uint16_t        hash_size;

    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    p_ISD_PARAM   = &p_ISD->prm.RSA_SIGN;
    
    /* get the pointer the the RSA public key used */
    p_rsa_ctx  = (rsa_ctx_t  *)p_JOB_PARAM->p_rsa_ctx;
    p_hash_ctx = (hash_ctx_t *)p_JOB_PARAM->p_hash_ctx;
    
    /* Get Remap(Convert Physical address -> Logical Address) */
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD_PARAM->p_source, &source_logical.val);

    switch ((uint32_t)p_job->process_phase)
    {
        case PHASE_H1:
        {
            (void)L_HASH_GetInternalHashType(p_ISD_PARAM->hash_primitive, &hash_type, &hash_size);

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

            p_job->process_phase = PHASE_H3;
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
                p_job->process_phase = PHASE_H3;
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

        case PHASE_H3:
            /* Get plain_hash and Free CTX_TYPE_HASH */
            err = L_HASH_Finish(p_hash_ctx, plain_hash_buff);
            if (ER_OK != err)
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
                break;
            }
            else
            {
                /* do nothing */
            }

            (void)L_HASH_GetInternalHashType(p_ISD_PARAM->hash_primitive, &hash_type, &hash_size);

            /* remember plain_hash_buff[] in W */
            L_Toolbox_MemCpy((uint8_t *)&p_rsa_ctx->W.value, (const uint8_t *)plain_hash_buff,
                             (int16_t)hash_size);
            p_rsa_ctx->W.size = hash_size;

            p_job->process_phase = PHASE_1;
            break;
        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    
    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0038]:[ICUMFW_UD_S_01_PK_0038]
* Function name: s_01_RSA_SIGN_CalcExpMod
* Description  : Calculation part of generating RSA signature service
* Arguments    : (IN-OUT)  p_job
* Return Value : SERV_OK
*                SERV_CRYPT_ERROR
*                SERV_SYS_ERROR
*                SERV_NO_INIT_RNG
****************************************************************************************/
static uint32_t s_01_RSA_SIGN_CalcExpMod(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_SIGN_t     *p_ISD_PARAM;
    
    rsa_ctx_t  *p_rsa_ctx;
    
    uint32_t    err;
    remap_ptr_manip_t   signature_logical;
    
    /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    p_ISD_PARAM   = &p_ISD->prm.RSA_SIGN;
    
    err = ER_OK;
    
    /* get the pointer the the RSA public key used */
    p_rsa_ctx  = (rsa_ctx_t  *)p_JOB_PARAM->p_rsa_ctx;
    
    /* Get Remap(Convert Physical address -> Logical Address) */
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD_PARAM->p_signature, &signature_logical.val);

    switch ((uint32_t)p_job->process_phase)
    {
        case PHASE_1:
            /* ************************************************************************* */
            /* Initialize the modular exponentiation. */
            /* ************************************************************************* */
            err = s_01_InitExpMod(p_job);
            break;
        case PHASE_2:
            /* ************************************************************************* */
            /* Process the modular exponentiation. all at once */
            /* ************************************************************************* */
            err = s_01_UpdateOnceExpMod(p_job);
            break;
        case PHASE_2DIV:
            /* ************************************************************************* */
            /* Process the modular exponentiation in slices */
            /* ************************************************************************* */
            err = s_01_UpdateSlicesExpMod(p_job);
            break;
        case PHASE_3:
            /* ************************************************************************* */
            /* Get the result of the modular exponentiation */
            /* i.e. the encoded hash */
            /* ************************************************************************* */
            err = s_01_FinishExpMod(p_job);
            break;

        case PHASE_4:
        {
            big_num_type_t      temp_type   = E_BINARY_PLAIN;
            uint32_t            bn_area_len;

            /* ************************************************************************* */
            /* Convert the signature into the desired output format */
            /* ************************************************************************* */
            bn_area_len = NUM0;
            switch (p_ISD_PARAM->signature_format)
            {
                case FORMAT_STRING_HEXA:
                    temp_type = E_STRING_HEXA;
                    break;
                case FORMAT_BINARY_PLAIN:
                    temp_type = E_BINARY_PLAIN;
                    bn_area_len = (((uint32_t)p_ISD_PARAM->signature_size_in_bits + NUM7) / NUM8);
                    break;
                case FORMAT_STRING_DECIMAL:
                default:
                    /* should never occur */
                    /* case of FORMAT_STRING_DECIMAL, branch to PHASE_4DIV */
                    break;
            }

            /* convert from big number into the required format */

            /* result big_num is in W */
            err = L_BIGNUM_DecodeFromBigNum(&p_rsa_ctx->W, temp_type,
                                            signature_logical.p_ptr, &bn_area_len);
            if (ER_OK != err)
            {
                /* error on conversion: break here */
                err = (uint32_t)SERV_CRYPT_ERROR;
                break;
            }
            else
            {
                /* do nothing */
            }

            /* --- Free RSA_Context ---- */
            (void)ML_CTX_Free(CTX_TYPE_RSA, p_JOB_PARAM->p_rsa_ctx);
            p_JOB_PARAM->resource_ctrl &= ~GET_CTX_RSA;

            /* --- Free HASH Context --- */
            (void)ML_CTX_Free(CTX_TYPE_HASH, p_JOB_PARAM->p_hash_ctx);
            p_JOB_PARAM->resource_ctrl &= ~GET_CTX_HASH;

            /* done! */
            p_job->process_phase = ON_HOLD;
            err = ER_OK;
            break;
        }

        case PHASE_4DIV:
        {
            /* case FORMAT_STRING_DECIMAL */
            uint32_t in_process;
            uint32_t i;

            for (i = NUM0; i < NUM10; i++)
            {
                in_process = L_BIGNUM_DecodeFromBigNumSlice(&p_rsa_ctx->W, E_STRING_DECIMAL,
                                                            signature_logical.p_ptr, p_rsa_ctx->tmp_exp);
                p_rsa_ctx->tmp_exp++;
                if (ER_OK == in_process)
                {
                    /* --- Free RSA_Context ---- */
                    (void)ML_CTX_Free(CTX_TYPE_RSA, p_JOB_PARAM->p_rsa_ctx);
                    p_JOB_PARAM->resource_ctrl &= ~GET_CTX_RSA;

                    /* --- Free HASH Context --- */
                    (void)ML_CTX_Free(CTX_TYPE_HASH, p_JOB_PARAM->p_hash_ctx);
                    p_JOB_PARAM->resource_ctrl &= ~GET_CTX_HASH;

                    /* done! */
                    p_job->process_phase = ON_HOLD;
                    err = ER_OK;
                    break;
                }
                else
                {
                    /* do nothing */
                }
            }

            break;
        }
        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    } /* end of switch (p_job->process_phase) */
    
    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0039]:[ICUMFW_UD_S_01_PK_0039]
* Function name: s_01_RSA_SIGN_Finish
* Description  : Finish part of generating RSA signature service
* Arguments    : (IN-OUT)  p_job
*                (IN)  error
* Return Value : none
****************************************************************************************/
static void s_01_RSA_SIGN_Finish(icum_job_t *p_job, const uint32_t error)
{
    r_icumif_isd_t     *p_ISD;
    job_rsa_t          *p_JOB_PARAM;
    ISD_RSA_SIGN_t     *p_ISD_PARAM;
    uint32_t    err;
    
    /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_rsa;
    p_ISD_PARAM   = &p_ISD->prm.RSA_SIGN;
    
    err = error;
    
    if (ER_OK == err)
    {
        err = (uint32_t)SERV_OK;
    }
    else
    {
        /* do nothing */
    }
    if ((uint32_t)SERV_OK != err)
    {
        /* error: terminate the service */
        p_ISD->service_result = (r_icumif_service_result_t)err;
        p_job->process_phase = ON_HOLD;

        /* free context when illegal end */
        if (((uint32_t)p_JOB_PARAM->resource_ctrl & GET_CTX_RSA) == GET_CTX_RSA)
        {
            (void)ML_CTX_Free(CTX_TYPE_RSA, p_JOB_PARAM->p_rsa_ctx);
        }
        else
        {
            /* do nothing */
        }
        if (((uint32_t)p_JOB_PARAM->resource_ctrl & GET_CTX_HASH) == GET_CTX_HASH)
        {
            (void)ML_CTX_Free(CTX_TYPE_HASH, p_JOB_PARAM->p_hash_ctx);
        }
        else
        {
            /* do nothing */
        }
        if (((uint32_t)p_JOB_PARAM->resource_ctrl & GET_KEY_RSA) == GET_KEY_RSA)
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
        if ((uint32_t)p_job->process_phase == ON_HOLD)
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
* Function ID  : [ICUMFW_CD_S_01_PK_0010]:[ICUMFW_UD_S_01_PK_0010]
* Function name: S_01_MakeRsaSignature
* Description  : Generate RSA signature Service
* Arguments    : (IN-OUT)  p_job
* Return Value : none
****************************************************************************************/
void S_01_MakeRsaSignature(icum_job_t *p_job)
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
            err = s_01_RSA_SIGN_Init(p_job);
            break;

        case PHASE_H1:
        case PHASE_H2:
        case PHASE_H2DIV:
        case PHASE_H3:
            err = s_01_RSA_SIGN_GenHash(p_job);
            break;

        case PHASE_1:
        case PHASE_2:
        case PHASE_2DIV:
        case PHASE_3:
        case PHASE_4:
        case PHASE_4DIV:
            err = s_01_RSA_SIGN_CalcExpMod(p_job);
            break;
        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }

    if (skip_flg == FLG_OFF)
    {
        s_01_RSA_SIGN_Finish(p_job, err);
    }
    else
    {
        /* do nothing */
    }
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0047]:[ICUMFW_UD_S_01_PK_0047]
* Function name: s_01_InitExpMod
* Description  : Initialize the modular exponentiation
* Arguments    : (IN-OUT) p_job
* Return Value : SERV_OK
*                SERV_CRYPT_ERROR
*                SERV_NO_INIT_RNG
****************************************************************************************/
static uint32_t s_01_InitExpMod(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_isd;
    job_rsa_t          *p_job_param;
    ISD_RSA_SIGN_t     *p_isd_param;
    uint32_t            err;
    remap_ptr_manip_t   source_logical;
    st_pkc_data_t       digest;
    st_pkcd_rsa_key_bin_t rsa_key;
    hash_type_t         hash_type;
    uint16_t            hash_size;
    uint8_t             plain_hash_buff[HASH_512_SIZE_IN_BYTES];
    rsa_key_t          *p_key;
    rsa_ctx_t          *p_rsa_ctx;

    /* retrieve the service handler */
    p_isd         = p_job->p_ISD;
    p_job_param   = &p_job->prm.job_rsa;
    p_isd_param   = &p_isd->prm.RSA_SIGN;

    err = ER_OK;

    /* get the pointer the the RSA public key used */
    p_key      = (rsa_key_t  *)p_job_param->p_key;
    p_rsa_ctx  = (rsa_ctx_t  *)p_job_param->p_rsa_ctx;

    /* Initialize parameters for PKC API */
    L_Toolbox_MemSet((uint8_t *)&rsa_key, NUM0, (int16_t)sizeof(st_pkcd_rsa_key_bin_t));

    if (NUM0 != (uint32_t)p_isd_param->job_slice_mod_exp)
    {
        p_job_param->proc_type = RSA_PROC_ASYNC;
    }
    else
    {
        p_job_param->proc_type = RSA_PROC_SYNC;
    }

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

    if (NUM0 != (uint32_t)p_isd_param->is_precalculated_hash)
    {
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_param->p_source, &source_logical.val);
        L_Toolbox_MemCpy((uint8_t *)plain_hash_buff, (const uint8_t *)source_logical.p_ptr, (int16_t)hash_size);
    }
    else
    {
        /* restore plain_hash_buff[] from W */
        L_Toolbox_MemCpy((uint8_t *)plain_hash_buff, (const uint8_t *)&p_rsa_ctx->W.value, (int16_t)hash_size);
    }
    digest.p_data = (uint8_t *)plain_hash_buff;
    digest.len = hash_size;

    rsa_key.key_length = (uint32_t)p_key->p_properties->modulus_size_in_bytes;
    rsa_key.p_n = p_key->p_modulus;
    rsa_key.p_d = p_key->p_priv_exponent;

    err = L_PKC_RsaSignInit(&digest, (void *)&rsa_key, rsa_key.key_length, &p_rsa_ctx->pkc_ctx);
    if (ER_OK == err)
    {
        p_job->process_phase = PHASE_2;
    }
    else if (ER_ML_PRNG_STATUS == err)
    {
        err = (uint32_t)SERV_NO_INIT_RNG;
    }
    else
    {
        err = (uint32_t)SERV_CRYPT_ERROR;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0048]:[ICUMFW_UD_S_01_PK_0048]
* Function name: s_01_UpdateOnceExpMod
* Description  : Process the modular exponentiation. all at once
* Arguments    : (IN-OUT) p_job
* Return Value : SERV_OK
*                SERV_CRYPT_ERROR
****************************************************************************************/
static uint32_t s_01_UpdateOnceExpMod(icum_job_t *p_job)
{
    uint32_t                err;
    job_rsa_t               *p_job_param;
    st_pkc_operate_opt_t    operate_opt;
    rsa_ctx_t               *p_rsa_ctx;

    p_job_param   = &p_job->prm.job_rsa;

    err = ER_OK;

    /* get the pointer the the RSA public key used */
    p_rsa_ctx  = (rsa_ctx_t  *)p_job_param->p_rsa_ctx;

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

    err = L_PKC_RsaSignOperate(&p_rsa_ctx->pkc_ctx, &operate_opt);
    if (ER_OK == err)
    {
        if (RSA_PROC_SYNC == (uint32_t)p_job_param->proc_type)
        {
            p_job->process_phase = PHASE_3;
        }
        else
        {
            p_job->process_phase = PHASE_2DIV;
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
    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0049]:[ICUMFW_UD_S_01_PK_0049]
* Function name: s_01_UpdateSlicesExpMod
* Description  : Process the modular exponentiation in slices
* Arguments    : (IN-OUT) p_job
* Return Value : SERV_OK
*                SERV_CRYPT_ERROR
****************************************************************************************/
static uint32_t s_01_UpdateSlicesExpMod(icum_job_t *p_job)
{
    uint32_t                err;

    err = L_PKC_RsaChkPkccBusy();
    if (ER_IN_PROCESS == err)
    {
        err = ER_OK;
    }
    else if (ER_OK == err)
    {
        p_job->process_phase = PHASE_3;
    }
    else
    {
        err = (uint32_t)SERV_CRYPT_ERROR;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0050]:[ICUMFW_UD_S_01_PK_0050]
* Function name: s_01_FinishExpMod
* Description  : Get the result of the modular exponentiation
* Arguments    : (IN-OUT) p_job
* Return Value : SERV_OK
*                SERV_CRYPT_ERROR
****************************************************************************************/
static uint32_t s_01_FinishExpMod(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_isd;
    job_rsa_t          *p_job_param;
    ISD_RSA_SIGN_t     *p_isd_param;

    rsa_ctx_t          *p_rsa_ctx;
    uint32_t            err;
    st_pkc_data_t       sign_data;

   /* retrieve the service handler */
    p_isd         = p_job->p_ISD;
    p_job_param   = &p_job->prm.job_rsa;
    p_isd_param   = &p_isd->prm.RSA_SIGN;

    err = ER_OK;

    /* get the pointer the the RSA public key used */
    p_rsa_ctx  = (rsa_ctx_t  *)p_job_param->p_rsa_ctx;

    L_Toolbox_MemSet((uint8_t *)&p_rsa_ctx->W.value, NUM0, sizeof(p_rsa_ctx->W.value));
    sign_data.len = (uint32_t)sizeof(p_rsa_ctx->W.value);
    sign_data.p_data = (uint8_t *)&p_rsa_ctx->W.value;
    err = L_PKC_RsaSignFinal(&p_rsa_ctx->pkc_ctx, &sign_data);
    p_rsa_ctx->W.size = sign_data.len;
    p_isd_param->signature_size_in_bits = (uint16_t)(sign_data.len * NUM8);

    /* --- Free RSA_Key -------- */
    (void)MH_KEY_FreeKeyCache(p_job, KEY_GRP_RSA, p_isd_param->key_id);
    p_job_param->resource_ctrl &= ~GET_KEY_RSA;

    if (ER_OK == err)
    {
        /* do nothing */
    }
    else
    {
        err = (uint32_t)SERV_CRYPT_ERROR;
    }

    /*****************************************/
    /* Not Free RSA_Context                  */
    /* Free RSA_Context is done in PHASE_5   */
    /*****************************************/
    if (ER_OK == err)
    {
        if(p_isd_param->signature_format == FORMAT_STRING_DECIMAL)
        {
            /* initialize tmp_exp, to use as string index of L_BIGNUM_DecodeFromBigNumSlice() */
            p_rsa_ctx->tmp_exp = 0u;
            p_job->process_phase = PHASE_4DIV;
        }
        else
        {
            p_job->process_phase = PHASE_4;
        }
    }
    else
    {
        /* do nothing */
    }
    /* I can break, because temp_big_num is saved in W[0] */
    return (err);
}
