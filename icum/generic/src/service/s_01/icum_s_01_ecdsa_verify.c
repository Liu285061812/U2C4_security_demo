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

/* Common */
#define NUM2            ((uint32_t)2)       /* value : 2 */
#define NUM7            ((uint32_t)7)       /* value : 7 */
#define NUM8            ((uint32_t)8)       /* value : 8 */
#define NUM512          ((uint32_t)512u)    /* value : 512 */

/* process_phase */
#define ON_HOLD             (0u)
#define PHASE_1             (1u)
#define PHASE_2             (2u)
#define PHASE_2a            (3u)
#define PHASE_3             (4u)
#define PHASE_4             (5u)
#define PHASE_4DIV          (6u)
#define PHASE_5             (7u)
#define PHASE_5DIV          (8u)
#define PHASE_6             (9u)

/* resource control */
#define GET_CTX_NONE        (0x00u)
#define GET_CTX_ECC         (0x01u)
#define GET_CTX_HASH        (0x02u)
#define GET_KEY_ECC         (0x04u)

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t s_01_MemCheckJobEcdsaVerify(icum_job_t *p_job, uint32_t *p_sig_size_in_bytes);
static uint32_t s_01_InitializeJobEcdsaVerify(icum_job_t *p_job);
static uint32_t s_01_ECDSA_VERIFY_Init(icum_job_t *p_job);
static uint32_t s_01_ECDSA_VERIFY_GenHash(icum_job_t *p_job);
static uint32_t s_01_ECDSA_VERIFY_InitSign(icum_job_t *p_job);
static uint32_t s_01_ECDSA_VERIFY_VerifySign(icum_job_t *p_job);
static void s_01_ECDSA_VERIFY_Finish(icum_job_t *p_job, const uint32_t err);

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0018]:[ICUMFW_UD_S_01_ECDSA_0018]
* Function name: s_01_MemCheckJobEcdsaVerify
* Description  : Memory check of the initialization processing
* Arguments    : (IN)   p_job
*                (IN)   p_sig_size_in_bytes
* Return Value : ER_OK
*                ER_PARAM
*                ER_MH_KEY_SRC_MEM_ERROR
****************************************************************************************/
static uint32_t s_01_MemCheckJobEcdsaVerify(icum_job_t *p_job, uint32_t *p_sig_size_in_bytes)
{
    r_icumif_isd_t     *p_isd;
    ISD_ECDSA_VERIFY_t *p_isd_param;

    uint32_t            err;
    uint8_t             mem_check_result;
    uint8_t             stringCheck;
    remap_ptr_manip_t   p_sig0data_logical;
    remap_ptr_manip_t   p_sig1data_logical;

    p_isd       = p_job->p_ISD;
    p_isd_param = &p_isd->prm.ECDSA_VERIFY;

    err = ER_OK;

    /* make sure the source address range is valid */
    (void)MM_FLS_CheckMemory((uint32_t)p_isd_param->p_source, p_isd_param->source_size_in_bytes,
                                FLS_CHK_MEM_SOURCE, p_job->expansion_service, &mem_check_result);
    if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_check_result)
    {
        (void)MM_FLS_CheckMemory((uint32_t)p_isd_param->signature[0].p_data, p_sig_size_in_bytes[0],
                                    FLS_CHK_MEM_SOURCE, p_job->expansion_service, &mem_check_result);
        if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_check_result)
        {
            (void)MM_FLS_CheckMemory((uint32_t)p_isd_param->signature[1].p_data, p_sig_size_in_bytes[1],
                                        FLS_CHK_MEM_SOURCE, p_job->expansion_service, &mem_check_result);
            if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_check_result)
            {
                /* Do nothing */
            }
            else
            {
                err = ER_MH_KEY_SRC_MEM_ERROR;
            }
        } /* end of if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_check_result) */
        else
        {
            err = ER_MH_KEY_SRC_MEM_ERROR;
        } /* end of else */
    } /* end of if ((uint32_t)FLS_CHK_OK == (uint32_t)mem_check_result) */
    else
    {
        err = ER_MH_KEY_SRC_MEM_ERROR;
    } /* end of else */

    if (ER_OK == err)
    {
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_param->signature[0].p_data, &p_sig0data_logical.val);

        /* check string format */
        stringCheck = TRUE;
        if (FORMAT_STRING_DECIMAL == p_isd_param->signature_format)
        {
            /* check decimal string. */
            (void)s_01_IsDigitString(p_sig0data_logical.p_ptr, p_sig_size_in_bytes[0], &stringCheck);
        }
        else if (FORMAT_STRING_HEXA == p_isd_param->signature_format)
        {
            /* check hexadecimal string. */
            (void)s_01_IsHexDigitString(p_sig0data_logical.p_ptr, p_sig_size_in_bytes[0], &stringCheck);
        }
        else
        {
            /* do nothing */
        }

        if (TRUE == (uint32_t)stringCheck)
        {
            /* Get Remap(Convert Physical address -> Logical Address) */
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_param->signature[1].p_data, &p_sig1data_logical.val);
            if (FORMAT_STRING_DECIMAL == p_isd_param->signature_format)
            {
                /* check decimal string. */
                (void)s_01_IsDigitString(p_sig1data_logical.p_ptr, p_sig_size_in_bytes[1], &stringCheck);
            }
            else if (FORMAT_STRING_HEXA == p_isd_param->signature_format)
            {
                /* check hexadecimal string. */
                (void)s_01_IsHexDigitString(p_sig1data_logical.p_ptr, p_sig_size_in_bytes[1], &stringCheck);
            }
            else
            {
                /* do nothing */
            }

            if (TRUE == (uint32_t)stringCheck)
            {
                /* Do nothing */
            }
            else
            {
                err = ER_PARAM;
            }
        } /* end of if (TRUE == (uint32_t)stringCheck) */
        else
        {
            err = ER_PARAM;
        } /* end of else */
    } /* end of if (ER_OK == err) */
    else
    {
        /* do nothing */
    }

    return err;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0004]:[ICUMFW_UD_S_01_ECDSA_0004]
* Function name: s_01_InitializeJobEcdsaVerify
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
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SLOT_EMPTY
****************************************************************************************/
static uint32_t s_01_InitializeJobEcdsaVerify(icum_job_t *p_job)
{
    /* retrieve the service handler */
    r_icumif_isd_t     *p_isd;
    job_ecc_t          *p_job_param;
    ISD_ECDSA_VERIFY_t *p_isd_param;

    ecc_key_t          *p_key;
    uint32_t            sig_size_in_bytes[NUM2];
    uint16_t            hash_size;
    hash_type_t         hash_type;
    uint16_t            max;
    uint8_t             is_allowed;
    uint32_t            err;
    uint32_t            skip_flg;

    p_isd       = p_job->p_ISD;
    p_job_param = &p_job->prm.job_ecc;
    p_isd_param = &p_isd->prm.ECDSA_VERIFY;

    p_key = NULL;
    err = ER_OK;
    skip_flg = FLG_OFF;

    /* test which hash primitive is to be used */
    (void)L_HASH_GetInternalHashType(p_isd_param->hash_primitive, &hash_type, &hash_size);
    if (NUM0 != (uint32_t)hash_size)
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
    } /* end of if (NUM0 != (uint32_t)hash_size) */
    else
    {
        /* error on hash type */
        err = ER_PARAM;
        skip_flg = FLG_ON;
    } /* end of else */
    
    if (FLG_OFF == skip_flg)
    {
        /* calculate size of input buffer (bytes) */
        switch (p_isd_param->signature_format)
        {
            case FORMAT_STRING_DECIMAL:
                /* get decimal figure length from bit_length. */
                (void)s_01_GetDecimalStrlenFromBitCnt(p_isd_param->signature_size_in_bits[0], &max);
                sig_size_in_bytes[0] = max;
                (void)s_01_GetDecimalStrlenFromBitCnt(p_isd_param->signature_size_in_bits[1], &max);
                sig_size_in_bytes[1] = max;
                /* increment for terminator(NULL) */
                sig_size_in_bytes[0]++;
                sig_size_in_bytes[1]++;
                break;
            case FORMAT_STRING_HEXA:
                sig_size_in_bytes[0] = ((uint32_t)p_isd_param->signature_size_in_bits[0] + NUM7) / NUM8;
                sig_size_in_bytes[0] *= NUM2;
                sig_size_in_bytes[1] = ((uint32_t)p_isd_param->signature_size_in_bits[1] + NUM7) / NUM8;
                sig_size_in_bytes[1] *= NUM2;
                /* increment for terminator(NULL) */
                sig_size_in_bytes[0]++;
                sig_size_in_bytes[1]++;
                break;
            case FORMAT_BINARY_PLAIN:
                sig_size_in_bytes[0] = ((uint32_t)p_isd_param->signature_size_in_bits[0] + NUM7) / NUM8;
                sig_size_in_bytes[1] = ((uint32_t)p_isd_param->signature_size_in_bits[1] + NUM7) / NUM8;
                break;
            default:
                /* error on number format */
                err = ER_PARAM;
                break;
        }
        if (ER_OK != err)
        {
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
        err = s_01_MemCheckJobEcdsaVerify(p_job, sig_size_in_bytes);
        if (ER_OK != err)
        {
            skip_flg = FLG_ON;
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

    if (FLG_OFF == skip_flg)
    {
        /* retrieve the ECC public key to be used */
        err = MH_KEY_GetEccKey(p_job, p_isd_param->key_group, p_isd_param->key_id, &p_key);
        if (ER_OK == err)
        {
            p_job_param->resource_ctrl |= GET_KEY_ECC;

            (void)s_01_CanUseKeyIfDbgConnected((key_flags_t)p_key->p_properties->flags.disable_on_debug,
                                                &is_allowed);
            if (TRUE == (uint32_t)is_allowed)
            {
                /* CURVE25519 and CURVE448 can use by only ecdh service, ED25519 and Ed448 can use by only eddsa service */
                if ((ECC_CURVE_CURVE25519 == p_key->p_properties->ecc_curve) ||
                    (ECC_CURVE_CURVE448 == p_key->p_properties->ecc_curve) ||
                    (ECC_CURVE_ED25519 == p_key->p_properties->ecc_curve) ||
                    (ECC_CURVE_ED448 == p_key->p_properties->ecc_curve))
                {
                    err = ER_MH_KEY_INVALID_KEY_INDEX;
                }
                else
                {
                    /* init the job slice */
                    p_job_param->job_slice = p_isd_param->job_slice_point_mul;

                    /* save the private key to be used in subsequent calculation */
                    p_job_param->p_pub_key = (uint8_t *)p_key;
                    p_job_param->p_priv_key = NULL;
                }
            }
            else
            {
                /* key not usable: abort */
                err = ER_MH_KEY_NOT_USABLE;
            }
        } /* end of if (ER_OK == err) */
        else
        {
            /* key not available: abort */
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0010]:[ICUMFW_UD_S_01_ECDSA_0010]
* Function name: s_01_ECDSA_VERIFY_Init
* Description  : Initialization part of verifying ECDSA signature service
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
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SLOT_EMPTY
*                ER_ML_CTX_ALL_USE
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_ECDSA_VERIFY_Init(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_ISD;
    job_ecc_t          *p_JOB_PARAM;
    ISD_ECDSA_VERIFY_t *p_ISD_PARAM;
    uint32_t            err;

     /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_ecc;
    p_ISD_PARAM   = &p_ISD->prm.ECDSA_VERIFY;

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

            /* reset verification result to fail */
            p_ISD_PARAM->verification_result = VERIFICATION_FAIL;

            err = s_01_InitializeJobEcdsaVerify(p_job);
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
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0011]:[ICUMFW_UD_S_01_ECDSA_0011]
* Function name: s_01_ECDSA_VERIFY_GenHash
* Description  : Hash generation part of verifying ECDSA signature service
* Arguments    : (IN-OUT)  p_job
* Return Value : ER_OK
*                SERV_CRYPT_ERROR
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_ECDSA_VERIFY_GenHash(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_ISD;
    job_ecc_t          *p_JOB_PARAM;
    ISD_ECDSA_VERIFY_t *p_ISD_PARAM;
    uint32_t            err;

    hash_type_t         hash_type;
    uint16_t            dmy_hash_size;
    hash_ctx_t          *p_hash_ctx;
    remap_ptr_manip_t   p_source_logical;

    /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_ecc;
    p_ISD_PARAM   = &p_ISD->prm.ECDSA_VERIFY;

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
            /* all at once hash */
            err = L_HASH_Update(p_hash_ctx, p_source_logical.p_ptr, p_ISD_PARAM->source_size_in_bytes);
            if (ER_OK != err)
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
            }
            else
            {
                p_job->process_phase = PHASE_3;
            }
            break;
        case PHASE_2a:
            /* job slice hash */
            if (p_JOB_PARAM->hash_remain_bytes <= (uint32_t)p_ISD_PARAM->job_slice_hash)
            {
                err = L_HASH_Update(p_hash_ctx, &p_source_logical.p_ptr[p_JOB_PARAM->hash_source_index],
                                    p_JOB_PARAM->hash_remain_bytes);
                if (ER_OK != err)
                {
                    err = (uint32_t)SERV_CRYPT_ERROR;
                    break;
                }
                else
                {
                    /* Do nothing */
                }

                p_JOB_PARAM->hash_remain_bytes = 0u;
                p_job->process_phase = PHASE_3;
                /* return dispatcher */
                break;
            }
            else
            {
                /* Do nothing */
            }

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
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0012]:[ICUMFW_UD_S_01_ECDSA_0012]
* Function name: s_01_ECDSA_VERIFY_InitSign
* Description  : Scalar calculation part 1 of verifying ECDSA signature service
* Arguments    : (IN-OUT)  p_job
* Return Value : ER_OK
*                ER_PARAM
*                SERV_CRYPT_ERROR
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_ECDSA_VERIFY_InitSign(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_ISD;
    job_ecc_t          *p_JOB_PARAM;
    ISD_ECDSA_VERIFY_t *p_ISD_PARAM;
    uint8_t            *p_hashed_message;
    uint32_t            hash_size;

    uint32_t            byte_size;
    pkc_small_data_t    sig[NUM2];
    big_num_type_t      temp_type;
    uint8_t             plain_hash[HASH_512_SIZE_IN_BYTES];
    int32_t             n;
    uint32_t            err;

    ecc_key_t           *p_key;
    ecc_ctx_t           *p_ecc_ctx;
    hash_ctx_t          *p_hash_ctx;
    remap_ptr_manip_t   p_source_logical;
    remap_ptr_manip_t   p_signdata_logical;
    st_pkc_data_t       digest_data;
    st_pkc_data_t       sign_data;
    uint32_t            sign_value[SMALLBIGNUM_LONG_SIZE * NUM2];
    uint32_t            sign_size;

    /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_ecc;
    p_ISD_PARAM   = &p_ISD->prm.ECDSA_VERIFY;

    /* get the pointer the the ECC public key, ECC context, or hash context used */
    p_key      = (ecc_key_t  *)p_JOB_PARAM->p_pub_key;
    p_ecc_ctx  = (ecc_ctx_t  *)p_JOB_PARAM->p_ecc_ctx;
    p_hash_ctx = (hash_ctx_t *)p_JOB_PARAM->p_hash_ctx;

    /* Initialize variables */
    err = ER_OK;
    L_Toolbox_MemSet(&plain_hash[0],          NUM0, (int16_t)sizeof(plain_hash));
    L_Toolbox_MemSet((uint8_t *)&sign_value,  NUM0, (int16_t)sizeof(sign_value));
    L_Toolbox_MemSet((uint8_t *)&sig,         NUM0, (int16_t)sizeof(sig));
    L_Toolbox_MemSet((uint8_t *)&digest_data, NUM0, (int16_t)sizeof(digest_data));
    L_Toolbox_MemSet((uint8_t *)&sign_data,   NUM0, (int16_t)sizeof(sign_data));

    switch ((uint32_t)p_job->process_phase)
    {
        case PHASE_3:
            if (NUM0 != (uint32_t)p_ISD_PARAM->is_precalculated_hash)
            {
                /* Get Remap(Convert Physical address -> Logical Address) */
                (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD_PARAM->p_source, &p_source_logical.val);
                p_hashed_message    = p_source_logical.p_ptr;
                hash_size = p_ISD_PARAM->source_size_in_bytes;
            }
            else
            {
                /* if p_source is message, retrieve the hash value */
                err = L_HASH_Finish(p_hash_ctx, plain_hash);
                if (ER_OK != err)
                {
                    err = (uint32_t)SERV_CRYPT_ERROR;
                    break;
                }
                else
                {
                    /* Do nothing */
                }
                p_hashed_message = plain_hash;
                hash_size  = (p_hash_ctx->hash_desc.hash_bitlen + NUM7) / NUM8;
            }
            /* ************************************************************************* */
            /* Convert the signature to a big number */
            /* ************************************************************************* */
            for (n = 0; n < (int32_t)NUM2; n++)
            {
                /* Get Remap(Convert Physical address -> Logical Address) */
                (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD_PARAM->signature[n].p_data,
                        &p_signdata_logical.val);
                switch (p_ISD_PARAM->signature_format)
                {
                    case FORMAT_STRING_DECIMAL:
                        temp_type = E_STRING_DECIMAL;
                        byte_size = (uint32_t)L_Toolbox_StrLen((const char *)p_signdata_logical.p_ptr);
                        break;
                    case FORMAT_STRING_HEXA:
                        temp_type = E_STRING_HEXA;
                        byte_size = (uint32_t)L_Toolbox_StrLen((const char *)p_signdata_logical.p_ptr);
                        break;
                    case FORMAT_BINARY_PLAIN:
                        temp_type = E_BINARY_PLAIN;
                        byte_size = (((uint32_t)p_ISD_PARAM->signature_size_in_bits[n] + NUM7) / NUM8);
                        break;
                    default:
                        /* Never Occur */
                        err = ER_PARAM;
                        break;
                }
                if (ER_OK != err)
                {
                    break;
                }
                else
                {
                    /* Do nothing */
                }

                err = L_BIGNUM_EncodeToSmallBigNum(p_signdata_logical.p_ptr, temp_type, byte_size, &sig[n]);
                if ((ER_OK != err) || (NUM0 == sig[n].size))
                {
                    /* error on conversion */
                    err = ER_PARAM;
                    break;
                }
                else
                {
                    /* Do nothing */
                }

                sign_size = ((uint32_t)p_ISD_PARAM->signature_size_in_bits[n] + NUM7) / NUM8;
                err = L_BIGNUM_PaddingData(&sign_size, sig[n].size, (uint8_t *)sig[n].value, NUM0);
                sig[n].size = sign_size;
                if (ER_OK != err)
                {
                    /* error on conversion */
                    err = ER_PARAM;
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
            if (ER_OK != err)
            {
                break;
            }
            else
            {
                /* Do nothing */
            }

            /* ************************************************************************* */
            /* Calculate the scalars for the signature verification (u1, u2) */
            /* ************************************************************************* */
            p_ecc_ctx->curve_id = p_key->p_properties->ecc_curve;
            p_ecc_ctx->ecc_key.key_length = p_key->p_pub_point->x.size;
            p_ecc_ctx->ecc_key.p_qx = (uint8_t *)p_key->p_pub_point->x.value;
            p_ecc_ctx->ecc_key.p_qy = (uint8_t *)p_key->p_pub_point->y.value;
            digest_data.p_data = p_hashed_message;
            digest_data.len = hash_size;
            sign_data.p_data = (uint8_t *)&sign_value[0];
            sign_data.len = sig[NUM0].size + sig[NUM1].size;
            L_Toolbox_MemCpy(&sign_data.p_data[NUM0], (const uint8_t *)&sig[NUM0].value, (int16_t)sig[NUM0].size);
            L_Toolbox_MemCpy(&sign_data.p_data[sig[NUM0].size], (const uint8_t *)&sig[NUM1].value, (int16_t)sig[NUM1].size);
            /* Setup base parameter of ECC for PKC library */
            err = L_PKC_EccSetup(p_ecc_ctx);
            if (ER_OK == err)
            {
                /* Perform ECDSA signature verification initial processing for PKC library */
                err = L_PKC_EcdsaVerifyInit((const st_pkc_data_t *)&digest_data,
                                            (const st_pkc_data_t *)&sign_data,
                                            &p_ecc_ctx->ecc_key,
                                            &p_ecc_ctx->ctx.ec);
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
                /* error case : return value of L_PKC_EccSetup */
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
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0015]:[ICUMFW_UD_S_01_ECDSA_0015]
* Function name: s_01_ECDSA_VERIFY_VerifySign
* Description  : Verification part of verifying ECDSA signature service
* Arguments    : (IN-OUT)  p_job
* Return Value : ER_OK
*                SERV_CRYPT_ERROR
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_ECDSA_VERIFY_VerifySign(icum_job_t *p_job)
{
    uint32_t            err;
    r_icumif_isd_t     *p_ISD;
    job_ecc_t          *p_JOB_PARAM;
    ISD_ECDSA_VERIFY_t *p_ISD_PARAM;
    ecc_ctx_t          *p_ecc_ctx;
    e_pkc_sync_opt_t    sync_opt;

    /* Initialize variable */
    err = ER_OK;
    /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_ecc;
    p_ISD_PARAM   = &p_ISD->prm.ECDSA_VERIFY;
    /* get the pointer the the ECC public key used */
    p_ecc_ctx  = (ecc_ctx_t  *)p_JOB_PARAM->p_ecc_ctx;

    switch ((uint32_t)p_job->process_phase)
    {
        case PHASE_4:
            if (NUM0 == (uint32_t)p_JOB_PARAM->job_slice)
            {
                sync_opt = PKC_SYNC_OPT_SYNC;
            }
            else
            {
                sync_opt = PKC_SYNC_OPT_ASYNC;
            }
            /* Perform ECDSA signature verification processing (1st) for PKC library */
            err = L_PKC_EcdsaVerifyOperate(&p_ecc_ctx->ctx.ec, sync_opt);
            if (ER_OK == err)
            {
                p_job->process_phase = PHASE_6;
            }
            else if (ER_IN_PROCESS == err)
            {
                if (PKC_SYNC_OPT_SYNC == sync_opt)
                {
                    p_job->process_phase = PHASE_5;
                }
                else
                {
                    p_job->process_phase = PHASE_4DIV;
                }
                err = ER_OK;
            }
            else
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
            }
            break;

        case PHASE_4DIV:
            /* Check execution state of PKC */
            err = L_PKC_EccChkPkccBusy();
            if (ER_IN_PROCESS == err)
            {
                err = ER_OK;
            }
            else if (ER_OK == err)
            {
                /* all bits processed */
                p_job->process_phase = PHASE_5;
            }
            else
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
            }
            break;

        case PHASE_5:
            if (NUM0 == (uint32_t)p_JOB_PARAM->job_slice)
            {
                sync_opt = PKC_SYNC_OPT_SYNC;
            }
            else
            {
                sync_opt = PKC_SYNC_OPT_ASYNC;
            }
            /* Perform ECDSA signature verification processing (2nd) for PKC library */
            err = L_PKC_EcdsaVerifyOperate(&p_ecc_ctx->ctx.ec, sync_opt);
            if (ER_OK == err)
            {
                if (PKC_SYNC_OPT_SYNC == sync_opt)
                {
                    p_job->process_phase = PHASE_6;
                }
                else
                {
                    p_job->process_phase = PHASE_5DIV;
                }
            }
            else
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
            }
            break;

        case PHASE_5DIV:
            /* Check execution state of PKC */
            err = L_PKC_EccChkPkccBusy();
            if (ER_IN_PROCESS == err)
            {
                err = ER_OK;
            }
            else if (ER_OK == err)
            {
                /* all bits processed */
                p_job->process_phase = PHASE_6;
            }
            else
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
            }
            break;

        case PHASE_6:
            /* Perform ECDSA signature verification termination processing for PKC library */
            err = L_PKC_EcdsaVerifyFinal(&p_ecc_ctx->ctx.ec);
            if (ER_OK == err)
            {
                p_ISD_PARAM->verification_result = VERIFICATION_PASS;
            }
            else if (ER_L_ECC_VERIFY_FAILED == err)
            {
                p_ISD_PARAM->verification_result = VERIFICATION_FAIL;
            }
            else
            {
                err = (uint32_t)SERV_CRYPT_ERROR;
                break;
            }

            /* --- Free ECC_Context ---- */
            (void)ML_CTX_Free( CTX_TYPE_ECC, p_JOB_PARAM->p_ecc_ctx);
            p_JOB_PARAM->resource_ctrl &= ~GET_CTX_ECC;

            /* ---- Free HASH Context -------*/
            (void)ML_CTX_Free( CTX_TYPE_HASH, p_JOB_PARAM->p_hash_ctx);
            p_JOB_PARAM->resource_ctrl &= ~GET_CTX_HASH;

            /* --- Free ECC_Key -------- */
            (void)MH_KEY_FreeKeyCache(p_job, p_ISD_PARAM->key_group, p_ISD_PARAM->key_id);
            p_JOB_PARAM->resource_ctrl &= ~GET_KEY_ECC;

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
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0016]:[ICUMFW_UD_S_01_ECDSA_0016]
* Function name: s_01_ECDSA_VERIFY_Finish
* Description  : Finish part of verifying ECDSA signature service
* Arguments    : (IN-OUT)  p_job
*                (IN)  err
* Return Value : void
****************************************************************************************/
static void s_01_ECDSA_VERIFY_Finish(icum_job_t *p_job, const uint32_t err)
{
    r_icumif_isd_t     *p_ISD;
    job_ecc_t          *p_JOB_PARAM;
    ISD_ECDSA_VERIFY_t *p_ISD_PARAM;

    /* retrieve the service handler */
    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_ecc;
    p_ISD_PARAM   = &p_ISD->prm.ECDSA_VERIFY;

    if (ER_OK != err)
    {
        /* error: terminate the service */
        (void)s_01_ConvertErr(err, &p_ISD->service_result);
        p_ISD->prm.ECDSA_VERIFY.verification_result = VERIFICATION_FAIL;

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

}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDSA_0003]:[ICUMFW_UD_S_01_ECDSA_0003]
* Function name: S_01_VerifyEcdsaSignature
* Description  : Verify ECDSA signature Service (service entry point)
* Arguments    : (IN-OUT)  p_job
* Return Value : void
****************************************************************************************/
void S_01_VerifyEcdsaSignature(icum_job_t *p_job)
{
    /* retrieve the service handler */
    r_icumif_isd_t     *p_ISD;
    job_ecc_t          *p_JOB_PARAM;
    uint32_t            err;

    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_ecc;
    err = ER_OK;

    /**********************************************************************************
    * Initialization phase
    ***********************************************************************************/
    /* already initialized? */
    if (p_ISD->service_result == SERV_REQUEST)
    {
        p_JOB_PARAM->resource_ctrl = GET_CTX_NONE;
    }
    else
    {
        /* Do nothing */
    }
    /* indicate service is in process */
    p_ISD->service_result = SERV_IN_PROCESS;

    switch ((uint32_t)p_job->process_phase)
    {
        case ON_HOLD:
            err = s_01_ECDSA_VERIFY_Init(p_job);
            break;
        case PHASE_1:
        case PHASE_2:
        case PHASE_2a:
            err = s_01_ECDSA_VERIFY_GenHash(p_job);
            break;
        case PHASE_3:
            err = s_01_ECDSA_VERIFY_InitSign(p_job);
            break;
        case PHASE_4:
        case PHASE_4DIV:
        case PHASE_5:
        case PHASE_5DIV:
        case PHASE_6:
            err = s_01_ECDSA_VERIFY_VerifySign(p_job);
            break;
        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }

    s_01_ECDSA_VERIFY_Finish(p_job, err);

    return;
}

