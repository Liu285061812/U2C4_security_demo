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

#include "icum_d_comm_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_l_cry_pk_pub.h"
#include "icum_ml_ctx_pub.h"
/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mh_key_pub.h"
#include "icum_mh_sysinfo_pub.h"
#include "icum_s_pub.h"
#include "icum_k_issd_pub.h"

#include "icum_s_01_common.h"
#include "icum_service_common.h"

#if   ((R_CRYPT_SMALLBIGNUM_BITSIZE % 32u) == 0u)
#define PRNG_BUFFER_SIZE    ((R_CRYPT_SMALLBIGNUM_BITSIZE / 32u) + (128u / 32u))
#else
#define PRNG_BUFFER_SIZE    (((R_CRYPT_SMALLBIGNUM_BITSIZE / 32u) + 1u) + (128u / 32u))
#endif

/* process_phase */
#define ON_HOLD             (0u)
#define PHASE_1             (1u)
#define PHASE_2             (2u)
#define PHASE_2a            (3u)
#define PHASE_3             (4u)
#define PHASE_4             (5u)
#define PHASE_4a            (6u)
#define PHASE_5             (7u)

/* resource control */
#define GET_CTX_NONE        (0x00u)
#define GET_CTX_ECC         (0x01u)
#define GET_CTX_HASH        (0x02u)
#define GET_KEY_ECC         (0x04u)

/* unsigned data size */
#define NUM2                (2u)
#define NUM7                (7u)
#define NUM8                (8u)
#define NUM512              (512u)

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t s_01_ParamCheckJobEcdsaSign(icum_job_t *p_job, ecc_key_t **p_key);
static uint32_t s_01_InitializeJobEcdsaSign(icum_job_t *p_job);
static uint32_t s_01_ECDSA_SIGN_Init(icum_job_t *p_job);
static uint32_t s_01_ECDSA_SIGN_GenHash(icum_job_t *p_job);
static uint32_t s_01_ECDSA_SIGN_CalcSignInit(icum_job_t *p_job);
static uint32_t s_01_ECDSA_SIGN_CalcSign(icum_job_t *p_job);
static void s_01_ECDSA_SIGN_Finish(icum_job_t *p_job, const uint32_t err);

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0019]:[ICUMFW_UD_S_01_ECDSA_0019]
* Function name: s_01_ParamCheckJobEcdsaSign
* Description  : Parameter check of the initialization processing
* Arguments    : (IN)   p_job
*                (OUT)  p_key
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
static uint32_t s_01_ParamCheckJobEcdsaSign(icum_job_t *p_job, ecc_key_t **p_key)
{
    /* retrieve the service handler */
    r_icumif_isd_t     *p_isd;
    job_ecc_t          *p_job_param;
    ISD_ECDSA_SIGN_t   *p_isd_param;

    uint16_t            hash_size;
    hash_type_t         hash_type;
    uint8_t             mem_check_result;
    uint8_t             is_allowed;
    uint32_t            err;
    uint32_t            skip_flg;

    p_isd       = p_job->p_ISD;
    p_job_param = &p_job->prm.job_ecc;
    p_isd_param = &p_isd->prm.ECDSA_SIGN;

    err = ER_OK;
    skip_flg = FLG_OFF;

    /* test which hash primitive is to be used */
    (void)L_HASH_GetInternalHashType(p_isd_param->hash_primitive, &hash_type, &hash_size);
    if (NUM0 == (uint32_t)hash_size)
    {
        /* error on hash type */
        err = ER_PARAM;
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* when hash provided, make sure source_size_in_bytes equals hash length */
        if (NUM0 != (uint32_t)p_isd_param->is_precalculated_hash)
        {
            if (p_isd_param->source_size_in_bytes != (uint32_t)hash_size)
            {
                err = ER_PARAM;
                skip_flg = FLG_ON;
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
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }
    
    if (FLG_OFF == skip_flg)
    {
        /* make sure the source address range is valid */
        (void)MM_FLS_CheckMemory((uint32_t)p_isd_param->p_source, p_isd_param->source_size_in_bytes,
                                        FLS_CHK_MEM_SOURCE, p_job->expansion_service, &mem_check_result);
        if ((uint32_t)FLS_CHK_OK != (uint32_t)mem_check_result)
        {
            err = ER_MH_KEY_SRC_MEM_ERROR;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* retrieve the ECC private key to be used */
        err = MH_KEY_GetEccKey(p_job, p_isd_param->key_group, p_isd_param->key_id, p_key);
        if (ER_OK == err)
        {
            p_job_param->resource_ctrl |= GET_KEY_ECC;
            
            (void)s_01_CanUseKeyIfDbgConnected((key_flags_t)(*p_key)->p_properties->flags.disable_on_debug,
                                                &is_allowed);
            if (FALSE == (uint32_t)is_allowed)
            {
                /* key not usable: abort */
                err = ER_MH_KEY_NOT_USABLE;
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* key not available: abort */
            skip_flg = FLG_ON;
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* make sure the key has a private scalar */
        if (NUM0 != (uint32_t)(*p_key)->p_properties->priv_scalar_size_in_bits)
        {
                /* CURVE25519 and CURVE448 can use by only ecdh service, ED25519 and Ed448 can use by only eddsa service */
            if ((ECC_CURVE_CURVE25519 == (*p_key)->p_properties->ecc_curve) ||
                    (ECC_CURVE_CURVE448 == (*p_key)->p_properties->ecc_curve) ||
                    (ECC_CURVE_ED25519 == (*p_key)->p_properties->ecc_curve) ||
                    (ECC_CURVE_ED448 == (*p_key)->p_properties->ecc_curve))
            {
                err = ER_MH_KEY_INVALID_KEY_INDEX;
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        } /* end of if (NUM0 != (uint32_t)(*p_key)->p_properties->priv_scalar_size_in_bits) */
        else
        {
            /* key invalidated: abort */
            err = ER_MH_KEY_INVALID_KEY_GROUP;
            skip_flg = FLG_ON;
        } /* end of else */
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }
    
    return err;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0002]:[ICUMFW_UD_S_01_ECDSA_0002]
* Function name: s_01_InitializeJobEcdsaSign
* Description  : Initialization processing
* Arguments    : (IN)   p_job
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
*                ER_MH_KEY_DEST_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SLOT_EMPTY
****************************************************************************************/
static uint32_t s_01_InitializeJobEcdsaSign(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_isd;
    job_ecc_t          *p_job_param;
    ISD_ECDSA_SIGN_t   *p_isd_param;

    ecc_key_t          *p_key;
    uint32_t            output_buf_size_in_bytes;
    uint16_t            max_size_in_bytes;
    uint8_t             mem_check_result;
    uint32_t            err;

    /* retrieve the service handler */
    p_isd       = p_job->p_ISD;
    p_job_param = &p_job->prm.job_ecc;
    p_isd_param = &p_isd->prm.ECDSA_SIGN;

    p_key = NULL;
    err = ER_OK;
    mem_check_result = FLS_CHK_NG;
    output_buf_size_in_bytes = NUM0;
    max_size_in_bytes = NUM0;

    err = s_01_ParamCheckJobEcdsaSign(p_job, &p_key);
    if (ER_OK == err)
    {
        /* set the signature size with the modulus size */
        p_isd_param->signature_size_in_bits[0] = (uint16_t)p_key->p_properties->priv_scalar_size_in_bits;
        p_isd_param->signature_size_in_bits[1] = (uint16_t)p_key->p_properties->priv_scalar_size_in_bits;

        /* calculate size of output buffer (bytes) */
        switch (p_isd_param->signature_format)
        {
            case FORMAT_STRING_DECIMAL:
                /* get decimal figure length from bit_length. */
                (void)s_01_GetDecimalStrlenFromBitCnt(p_isd_param->signature_size_in_bits[0], &max_size_in_bytes);
                output_buf_size_in_bytes = max_size_in_bytes;
                /* increment for terminator(NULL) */
                output_buf_size_in_bytes++;
                break;
            case FORMAT_STRING_HEXA:
                output_buf_size_in_bytes = ((uint32_t)p_isd_param->signature_size_in_bits[0] + NUM7) / NUM8;
                output_buf_size_in_bytes *= NUM2;
                /* increment for terminator(NULL) */
                output_buf_size_in_bytes++;
                break;
            case FORMAT_BINARY_PLAIN:
                output_buf_size_in_bytes = ((uint32_t)p_isd_param->signature_size_in_bits[0] + NUM7) / NUM8;
                break;
            default:
                /* error on number format */
                err = ER_PARAM;
                break;
        }
    }
    else
    {
        /* do nothing */
    }

    if (ER_OK == err)
    {
        /* make sure the destination address range is valid */
        (void)MM_FLS_CheckMemory((uint32_t)p_isd_param->signature[0].p_data,
                                 output_buf_size_in_bytes,
                                 FLS_CHK_MEM_DESTINATION,
                                 p_job->expansion_service, &mem_check_result);
        if ((uint32_t)FLS_CHK_OK != (uint32_t)mem_check_result)
        {
            err = ER_MH_KEY_DEST_MEM_ERROR;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* do nothing */
    }

    if (ER_OK == err)
    {
        (void)MM_FLS_CheckMemory((uint32_t)p_isd_param->signature[1].p_data,
                                 output_buf_size_in_bytes,
                                 FLS_CHK_MEM_DESTINATION,
                                 p_job->expansion_service, &mem_check_result);
        if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_check_result)
                {
                    /* init the job slice */
            p_job_param->job_slice = p_isd_param->job_slice_point_mul;
                    
                    /* save the private key to be used in subsequent calculation */
            p_job_param->p_pub_key  = NULL;
            p_job_param->p_priv_key = (uint8_t *)p_key;
        }
        else
        {
            err = ER_MH_KEY_DEST_MEM_ERROR;
        }
    }
    else
    {
        /* do nothing */
    }
    
    return (err);
}


/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0005]:[ICUMFW_UD_S_01_ECDSA_0005]
* Function name: s_01_ECDSA_SIGN_Init
* Description  : Initialization part of generating ECDSA signature service
* Arguments    : (IN-OUT)  p_job
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
*                ER_MH_KEY_DEST_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SLOT_EMPTY
*                ER_ML_CTX_ALL_USE
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_ECDSA_SIGN_Init(icum_job_t *p_job)
{
    r_icumif_isd_t      *p_ISD;
    job_ecc_t           *p_JOB_PARAM;
    ISD_ECDSA_SIGN_t    *p_ISD_PARAM;
    uint32_t            err;

    /* retrieve the service handler */
    p_ISD          = p_job->p_ISD;
    p_JOB_PARAM    = &p_job->prm.job_ecc;
    p_ISD_PARAM    = &p_ISD->prm.ECDSA_SIGN;

    switch ((uint32_t)p_job->process_phase)
    {
        case ON_HOLD:
            if (((uint32_t)p_JOB_PARAM->resource_ctrl & GET_CTX_ECC) == GET_CTX_NONE)
            {
                /* get ecc context */
                err = ML_CTX_Get(CTX_TYPE_ECC, &p_JOB_PARAM->p_ecc_ctx);
                if (ER_OK != err)
                {
                    break;
                }
                else
                {
                    /* Do nothing */
                }
                p_JOB_PARAM->resource_ctrl |= GET_CTX_ECC;
                p_ISD->service_result = SERV_IN_PROCESS;
            }
            else
            {
                /* Do nothing */
            }

            if (NUM0 == (uint32_t)p_ISD_PARAM->is_precalculated_hash)
            {
                /* p_source is message, then get hash context */
                err = ML_CTX_Get(CTX_TYPE_HASH, &p_JOB_PARAM->p_hash_ctx);
                if (ER_OK != err)
                {
                    break;
                }
                else
                {
                    /* Do nothing */
                }
                p_JOB_PARAM->resource_ctrl |= GET_CTX_HASH;
                p_ISD->service_result = SERV_IN_PROCESS;
            }
            else
            {
                /* Do nothing */
            }

            err = s_01_InitializeJobEcdsaSign(p_job);
            if (ER_OK != err)
            {
                break;
            }
            else
            {
                /* Do nothing */
            }

            /* test if the hash has been already calculated */
            if (NUM0 != (uint32_t)p_ISD_PARAM->is_precalculated_hash)
            {
                /* skip the hash calculation */
                p_job->process_phase = PHASE_3;
            }
            else
            {
                p_job->process_phase = PHASE_1;
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
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0006]:[ICUMFW_UD_S_01_ECDSA_0006]
* Function name: s_01_ECDSA_SIGN_GenHash
* Description  : Hash generation part of generating ECDSA signature service
* Arguments    : (IN-OUT)  p_job
* Return Value : ER_OK
*                SERV_CRYPT_ERROR
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_ECDSA_SIGN_GenHash(icum_job_t *p_job)
{
    r_icumif_isd_t      *p_ISD;
    job_ecc_t           *p_JOB_PARAM;
    ISD_ECDSA_SIGN_t    *p_ISD_PARAM;
    
    hash_ctx_t          *p_hash_ctx;
    hash_type_t         hash_type;
    uint16_t            dmy_hash_size;
    uint32_t            err;
    remap_ptr_manip_t   p_source_logical;

    /* retrieve the service handler */
    p_ISD          = p_job->p_ISD;
    p_JOB_PARAM    = &p_job->prm.job_ecc;
    p_ISD_PARAM    = &p_ISD->prm.ECDSA_SIGN;

    /* get the pointer the the ECC public key used */
    p_hash_ctx = (hash_ctx_t *)p_JOB_PARAM->p_hash_ctx;

    /* Get Remap(Convert Physical address -> Logical Address) */
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD_PARAM->p_source, &p_source_logical.val);

    switch ((uint32_t)p_job->process_phase)
    {
        case PHASE_1:
            /* ************************************************************************* */
            /* Start a hash job */
            /* ************************************************************************* */
            (void)L_HASH_GetInternalHashType(p_ISD_PARAM->hash_primitive, &hash_type, &dmy_hash_size);

            err = L_HASH_Init(p_hash_ctx, hash_type);
            if (ER_OK != err)
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
                break;
            }
            else
            {
                /* Do nothing */
            }
                
            if (NUM0 != (uint32_t)p_ISD_PARAM->job_slice_hash)
            {
                p_JOB_PARAM->hash_source_index    = 0u;
                p_JOB_PARAM->hash_remain_bytes    = p_ISD_PARAM->source_size_in_bytes;
                p_job->process_phase = PHASE_2a;
            }
            else
            {
                p_job->process_phase = PHASE_2;
            }
            break;
        case PHASE_2:
            err = L_HASH_Update(p_hash_ctx, p_source_logical.p_ptr, p_ISD_PARAM->source_size_in_bytes);
            if (ER_OK == err)
            {
                p_job->process_phase = PHASE_3;
            }
            else
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
            }
            break;
        case PHASE_2a:
            /* job slice hash */
            if (p_JOB_PARAM->hash_remain_bytes <= (uint32_t)p_ISD_PARAM->job_slice_hash)
            {
                err = L_HASH_Update(p_hash_ctx, &p_source_logical.p_ptr[p_JOB_PARAM->hash_source_index],
                                    p_JOB_PARAM->hash_remain_bytes);
                if (ER_OK == err)
                {
                    p_JOB_PARAM->hash_remain_bytes = NUM0;
                    p_job->process_phase = PHASE_3;
                }
                else
                {
                    err = (uint32_t)SERV_CRYPT_ERROR;
                }
                break;
            }
            else
            {
                /* Do nothing */
            }
            /* end of if (p_JOB_PARAM->hash_remain_bytes <= p_ISD_PARAM->job_slice_hash) */

            err = L_HASH_Update(p_hash_ctx, &p_source_logical.p_ptr[p_JOB_PARAM->hash_source_index],
                                p_ISD_PARAM->job_slice_hash);
            if (ER_OK != err)
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
            }
            else
            {
                /* update pointers and number of bytes remaining */
                p_JOB_PARAM->hash_remain_bytes -= (uint32_t)p_ISD_PARAM->job_slice_hash;
                p_JOB_PARAM->hash_source_index += (uint32_t)p_ISD_PARAM->job_slice_hash;
            }
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
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0008]:[ICUMFW_UD_S_01_ECDSA_0008]
* Function name: s_01_ECDSA_SIGN_CalcSignInit
* Description  : ECDSA Sign calculation initial processing
* Arguments    : (IN-OUT)  p_job
* Return Value : ER_OK
*                ER_PARAM
*                SERV_CRYPT_ERROR
****************************************************************************************/
static uint32_t s_01_ECDSA_SIGN_CalcSignInit(icum_job_t *p_job)
{
    r_icumif_isd_t      *p_isd;
    job_ecc_t           *p_job_param;
    ISD_ECDSA_SIGN_t    *p_isd_param;
    ecc_key_t           *p_key;
    ecc_ctx_t           *p_ecc_ctx;
    hash_ctx_t          *p_hash_ctx;
    uint32_t            err;
    remap_ptr_manip_t   p_source_logical;
    uint8_t             plain_hash[HASH_512_SIZE_IN_BYTES];
    uint8_t             *p_hashed_message;
    uint32_t            hash_size;
    st_pkc_data_t       digest_data;

    /* retrieve the service handler */
    p_isd          = p_job->p_ISD;
    p_job_param    = &p_job->prm.job_ecc;
    p_isd_param    = &p_isd->prm.ECDSA_SIGN;

    /* get the pointer the the ECC public key used */
    p_key      = (ecc_key_t  *)p_job_param->p_priv_key;
    p_ecc_ctx  = (ecc_ctx_t  *)p_job_param->p_ecc_ctx;
    p_hash_ctx = (hash_ctx_t *)p_job_param->p_hash_ctx;

    err = ER_OK;

    /* Get Remap(Convert Physical address -> Logical Address) */
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_param->p_source, &p_source_logical.val);
    if (NUM0 != (uint32_t)p_isd_param->is_precalculated_hash)
    {
        p_hashed_message    = p_source_logical.p_ptr;
        hash_size           = p_isd_param->source_size_in_bytes;
    }
    else
    {
        /* if p_source is message, retrieve the hash value */
        err = L_HASH_Finish(p_hash_ctx, plain_hash);
        if (ER_OK == err)
        {
            p_hashed_message = plain_hash;
            hash_size        = (p_hash_ctx->hash_desc.hash_bitlen + NUM7) /NUM8;
        }
        else
        {
            err = (uint32_t)SERV_CRYPT_ERROR;
        }
    }
    /* end of if (NUM0 != p_ISD_PARAM->is_precalculated_hash) */

    if (ER_OK == err)
    {
        p_ecc_ctx->curve_id = p_key->p_properties->ecc_curve;
        p_ecc_ctx->ecc_key.key_length = p_key->p_priv_scalar->size;
        p_ecc_ctx->ecc_key.p_d = (uint8_t *)p_key->p_priv_scalar->value;
        digest_data.p_data = p_hashed_message;
        digest_data.len = hash_size;
        /* Setup base parameter of ECC for PKC library */
        err = L_PKC_EccSetup(p_ecc_ctx);
        if (ER_OK == err)
        {
            /* Perform ECDSA signature generation initial processing for PKC library */
            err = L_PKC_EcdsaSignInit(&digest_data, &p_ecc_ctx->ecc_key, &p_ecc_ctx->ctx.ec);
            if (ER_OK == err)
            {
                p_job->process_phase = PHASE_4;
            }
            else
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
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
    
    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0007]:[ICUMFW_UD_S_01_ECDSA_0007]
* Function name: s_01_ECDSA_SIGN_CalcSign
* Description  : Signature calculation part of generating ECDSA signature service
* Arguments    : (IN-OUT)  p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
*                SERV_CRYPT_ERROR
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_ECDSA_SIGN_CalcSign(icum_job_t *p_job)
{
    r_icumif_isd_t      *p_ISD;
    job_ecc_t           *p_JOB_PARAM;
    ISD_ECDSA_SIGN_t    *p_ISD_PARAM;
    ecc_key_t          *p_key;
    ecc_ctx_t           *p_ecc_ctx;

    uint32_t            byte_size;
    big_num_type_t      temp_type;
    pkc_small_data_t    r;
    pkc_small_data_t    s;
    uint32_t            err;
    remap_ptr_manip_t   p_sig0data_logical;
    remap_ptr_manip_t   p_sig1data_logical;
    st_pkc_data_t       sign_data;
    uint32_t            sign[SMALLBIGNUM_LONG_SIZE * NUM2];
    e_pkc_sync_opt_t    sync_opt;

    /* retrieve the service handler */
    p_ISD          = p_job->p_ISD;
    p_JOB_PARAM    = &p_job->prm.job_ecc;
    p_ISD_PARAM    = &p_ISD->prm.ECDSA_SIGN;

    /* get the pointer the the ECC public key used */
    p_key      = (ecc_key_t  *)p_JOB_PARAM->p_priv_key;
    p_ecc_ctx  = (ecc_ctx_t  *)p_JOB_PARAM->p_ecc_ctx;

    switch ((uint32_t)p_job->process_phase)
    {
        case PHASE_3:
            err = s_01_ECDSA_SIGN_CalcSignInit(p_job);
            break;

        case PHASE_4:
            if (NUM0 == (uint32_t)p_JOB_PARAM->job_slice)
            {
                sync_opt = PKC_SYNC_OPT_SYNC;
            }
            else
            {
                sync_opt = PKC_SYNC_OPT_ASYNC;
            }
            /* Perform ECDSA signature generation processing for PKC library */
            err = L_PKC_EcdsaSignOperate(&p_ecc_ctx->ctx.ec, sync_opt);
            if (ER_OK == err)
            {
                if (PKC_SYNC_OPT_SYNC == sync_opt)
                {
                    p_job->process_phase = PHASE_5;
                }
                else
                {
                    p_job->process_phase = PHASE_4a;
                }
            }
            else
            {
                /* Do nothing */
            }
            break;

        case PHASE_4a:
            /* Check execution state of PKC */
            err = L_PKC_EccChkPkccBusy();
            if (ER_IN_PROCESS == err)
            {
                err = ER_OK;
            }
            else if (ER_OK == err)
            {
                p_job->process_phase = PHASE_5;
            }
            else
            {
                /* Do nothing */
            }
            break;

        case PHASE_5:
            L_Toolbox_MemSet((uint8_t *)sign, NUM0, (int16_t)sizeof(sign));
            L_Toolbox_MemSet((uint8_t *)&r, NUM0, (int16_t)sizeof(r));
            L_Toolbox_MemSet((uint8_t *)&s, NUM0, (int16_t)sizeof(s));
            sign_data.len = sizeof(sign);
            sign_data.p_data = (uint8_t *)sign;
            /* Perform ECDSA signature generation termination processing for PKC library */
            err = L_PKC_EcdsaSignFinal(&p_ecc_ctx->ctx.ec, &sign_data);
            L_Toolbox_MemCpy((uint8_t *)r.value,
                                (const uint8_t *)sign_data.p_data,
                                (int16_t)(sign_data.len / NUM2));
            L_Toolbox_MemCpy((uint8_t *)s.value,
                                (const uint8_t *)&sign_data.p_data[(sign_data.len / NUM2)],
                                (int16_t)(sign_data.len / NUM2));
            r.size = sign_data.len / NUM2;
            s.size = sign_data.len / NUM2;

            /* --- Free ECC_Context ---- */
            (void)ML_CTX_Free(CTX_TYPE_ECC, p_JOB_PARAM->p_ecc_ctx);
            p_JOB_PARAM->resource_ctrl &= ~GET_CTX_ECC;

            /* --- Free Hash_Context ---- */
            (void)ML_CTX_Free(CTX_TYPE_HASH, p_JOB_PARAM->p_hash_ctx);
            p_JOB_PARAM->resource_ctrl &= ~GET_CTX_HASH;

            /* --- Free ECC_Key -------- */
            (void)MH_KEY_FreeKeyCache(p_job, p_ISD_PARAM->key_group, p_ISD_PARAM->key_id);
            p_JOB_PARAM->resource_ctrl &= ~GET_KEY_ECC;

            /* ************************************************************************* */
            /* Convert the signature into the desired output format */
            /* ************************************************************************* */
            switch (p_ISD_PARAM->signature_format)
            {
                case FORMAT_STRING_DECIMAL:
                    temp_type = E_STRING_DECIMAL;
                    break;
                case FORMAT_STRING_HEXA:
                    temp_type = E_STRING_HEXA;
                    break;
                case FORMAT_BINARY_PLAIN:
                    temp_type = E_BINARY_PLAIN;
                    break;
                default:
                    /* error on number format */
                    err = ER_PARAM;
                    break;
            }
            if (ER_OK != err)
            {
                /* error on conversion: break here */
                break;
            }
            else
            {
                /* Do nothing */
            }
            
            /* convert from big number into the required format */
            byte_size = 0u;
            if(p_ISD_PARAM->signature_format == FORMAT_BINARY_PLAIN)
            {
                byte_size = ((uint32_t)p_key->p_properties->priv_scalar_size_in_bits + NUM7) / NUM8;
            }
            else
            {
                /* Do nothing */
            }
            /* Get Remap(Convert Physical address -> Logical Address) */
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD_PARAM->signature[0].p_data,
                    &p_sig0data_logical.val);
            err = L_BIGNUM_DecodeFromSmallBigNum(&r, temp_type, p_sig0data_logical.p_ptr, &byte_size);
            if (ER_OK != err)
            {
                /* error on number format */
                err = (uint32_t)SERV_CRYPT_ERROR;
                break;
            }
            else
            {
                /* Do nothing */
            }
            p_ISD_PARAM->signature_size_in_bits[0] = (uint16_t)p_key->p_properties->priv_scalar_size_in_bits;
            byte_size = 0u;
            if(p_ISD_PARAM->signature_format == FORMAT_BINARY_PLAIN)
            {
                byte_size = ((uint32_t)p_key->p_properties->priv_scalar_size_in_bits + NUM7) / NUM8;
            }
            else
            {
                /* Do nothing */
            }
            /* Get Remap(Convert Physical address -> Logical Address) */
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD_PARAM->signature[1].p_data,
                    &p_sig1data_logical.val);
            err = L_BIGNUM_DecodeFromSmallBigNum(&s, temp_type, p_sig1data_logical.p_ptr, &byte_size);
            if (ER_OK != err)
            {
                /* error on number format */
                err = (uint32_t)SERV_CRYPT_ERROR;
                break;
            }
            else
            {
                /* Do nothing */
            }
            p_ISD_PARAM->signature_size_in_bits[1] = (uint16_t)p_key->p_properties->priv_scalar_size_in_bits;

            /* done! */
            p_job->process_phase = ON_HOLD;
            err = ER_OK;
            break;
        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0009]:[ICUMFW_UD_S_01_ECDSA_0009]
* Function name: s_01_ECDSA_SIGN_Finish
* Description  : Finish part of generating ECDSA signature service
* Arguments    : (IN-OUT)  p_job
*                (IN)  err
* Return Value : void
****************************************************************************************/
static void s_01_ECDSA_SIGN_Finish(icum_job_t *p_job, const uint32_t err)
{
    r_icumif_isd_t      *p_ISD;
    job_ecc_t           *p_JOB_PARAM;
    ISD_ECDSA_SIGN_t    *p_ISD_PARAM;
    
    /* retrieve the service handler */
    p_ISD          = p_job->p_ISD;
    p_JOB_PARAM    = &p_job->prm.job_ecc;
    p_ISD_PARAM    = &p_ISD->prm.ECDSA_SIGN;
    
    if (err != ER_OK)
    {
        /* error: terminate the service */
        (void)s_01_ConvertErr(err, &p_ISD->service_result);
            
        /* free context when illegal end */
        if (((uint32_t)p_JOB_PARAM->resource_ctrl & GET_CTX_ECC) == GET_CTX_ECC)
        {
            (void)ML_CTX_Free(CTX_TYPE_ECC, p_JOB_PARAM->p_ecc_ctx);
        }
        else
        {
            /* Do nothing */
        }
        if (((uint32_t)p_JOB_PARAM->resource_ctrl & GET_CTX_HASH) == GET_CTX_HASH)
        {
            (void)ML_CTX_Free(CTX_TYPE_HASH, p_JOB_PARAM->p_hash_ctx);
        }
        else
        {
            /* Do nothing */
        }
        if (((uint32_t)p_JOB_PARAM->resource_ctrl & GET_KEY_ECC) == GET_KEY_ECC)
        {
            (void)MH_KEY_FreeKeyCache(p_job, p_ISD_PARAM->key_group, p_ISD_PARAM->key_id);
        }
        else
        {
            /* Do nothing */
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
            /* Do nothing */
        }
    }
    return;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0001]:[ICUMFW_UD_S_01_ECDSA_0001]
* Function name: S_01_MakeEcdsaSignature
* Description  : Generate ECDSA signature Service (service entry point)
* Arguments    : (IN-OUT)  p_job
* Return Value : void
****************************************************************************************/
void S_01_MakeEcdsaSignature(icum_job_t *p_job)
{
    uint32_t            err;

    /**********************************************************************************
    * Initialization phase
    ***********************************************************************************/
    /* indicate service is in process */
    p_job->p_ISD->service_result = SERV_IN_PROCESS;

    switch ((uint32_t)p_job->process_phase)
    {
        case ON_HOLD:
            err = s_01_ECDSA_SIGN_Init(p_job);
            break;
        case PHASE_1:
        case PHASE_2:
        case PHASE_2a:
            err = s_01_ECDSA_SIGN_GenHash(p_job);
            break;
        case PHASE_3:
        case PHASE_4:
        case PHASE_4a:
        case PHASE_5:
            err = s_01_ECDSA_SIGN_CalcSign(p_job);
            break;
        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }

    s_01_ECDSA_SIGN_Finish(p_job, err);
}

