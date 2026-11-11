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
#define NUM16           ((uint32_t)16)      /* value : 16 */
#define NUM24           ((uint32_t)24)      /* value : 24 */
#define NUM32           ((uint32_t)32)      /* value : 32 */

/* process phase */
#define ON_HOLD    (0u)
#define PHASE_1    (1u)
#define PHASE_2a   (2u)
#define PHASE_2    (3u)
#define PHASE_3    (4u)

/* resource control */
#define GET_CTX_NONE        (0x00u)
#define GET_CTX_ECC         (0x01u)
#define GET_PUB_KEY_ECC     (0x04u)

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t s_01_ParamCheckJobEcdh(icum_job_t *p_job, uint32_t *p_skip_flg);
static uint32_t s_01_InitializeJobEcdh(icum_job_t *p_job);
static uint32_t s_01_ECDH_Init(icum_job_t *p_job);
static uint32_t s_01_ECDH_CalcScalar(icum_job_t *p_job);
static uint32_t s_01_ECDH_DeriveSharedKey(icum_job_t *p_job);
static void s_01_ECDH_Finish(icum_job_t *p_job, const uint32_t err);

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDH_0008]:[ICUMFW_UD_S_01_ECDH_0008]
* Function name: s_01_ParamCheckJobEcdh
* Description  : Parameter check for the initialization processing
* Arguments    : (IN)   p_job
* Arguments    : (OUT)  p_skip_flg
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
****************************************************************************************/
static uint32_t s_01_ParamCheckJobEcdh(icum_job_t *p_job, uint32_t *p_skip_flg)
{
    r_icumif_isd_t     *p_isd;
    ISD_ECDH_t         *p_isd_param;

    aes_key_t          *p_aes_key;
    kdf_type_t          kdf_type;
    hash_type_t         hash_type;
    uint16_t            hash_size;
    uint8_t             mem_check_result;
    uint32_t            err;
    uint32_t            skip_flg;

    p_isd       = p_job->p_ISD;
    p_isd_param = &p_isd->prm.ECDH;
    
    kdf_type = E_LAST_ITEM_KDF_TYPE;
    err = ER_OK;
    skip_flg = FLG_OFF;

    (void)L_ECC_ToInternalKdfType(p_isd_param->kdf_algo, &kdf_type);
    if (E_LAST_ITEM_KDF_TYPE == kdf_type)
    {
        err = ER_PARAM;
        skip_flg = FLG_ON;
    }
    else
    {
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
    }
    if (FLG_OFF == skip_flg)
    {
        if (NUM16 == (uint32_t)p_isd_param->key_size_in_bytes)
        {
            /* Do nothing */
        }
#if defined(FSW_AESKEY_256)
        else if ((NUM24 == (uint32_t)p_isd_param->key_size_in_bytes) ||
                 (NUM32 == (uint32_t)p_isd_param->key_size_in_bytes))
        {
            /* Do nothing */
        }
#endif /* FSW_AESKEY_256 */
        else
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        if (NUM0 < (uint32_t)p_isd_param->info_size_in_bytes)
        {
            /* make sure the source address range is valid */
            (void)MM_FLS_CheckMemory((uint32_t)p_isd_param->p_info, (uint32_t)p_isd_param->info_size_in_bytes,
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
        } /* end of if (0u < (uint32_t)p_ISD_PARAM->info_size_in_bytes) */
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
        /* retrieve the shared key to initialize (i.e. to calculate) */
        switch (p_isd_param->shared_key_group)
        {
            case KEY_GRP_AES_RAM:
                err = MH_KEY_GetAesKey(p_job, KEY_GRP_AES_RAM, p_isd_param->shared_key_id, &p_aes_key, FALSE);
                break;
            default:
                /* invalid key group: stop here */
                err = ER_MH_KEY_INVALID_KEY_GROUP;
                break;
        }
        if ((ER_MH_KEY_INVALID_KEY_INDEX == err) || (ER_MH_KEY_INVALID_KEY_GROUP == err))
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
        /* make sure the ECC public key and the private key are different */
        if((p_isd_param->ecc_priv_key_group == p_isd_param->ecc_pub_key_group) &&
           ((uint32_t)p_isd_param->ecc_priv_key_id == (uint32_t)p_isd_param->ecc_pub_key_id))
        {
            err = ER_MH_KEY_INVALID_KEY_INDEX;
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

    *p_skip_flg = skip_flg;

    return err;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDH_0002]:[ICUMFW_UD_S_01_ECDH_0002]
* Function name: s_01_InitializeJobEcdh
* Description  : Initialization processing
* Arguments    : hash_primitive          :[in]
*                *p_info                 :[in]
*                info_size_in_bytes      :[in]
*                shared_key_group        :[in]
*                shared_key_id           :[in]
*                ecc_pub_key_group       :[in]
*                ecc_pub_key_id          :[in]
*                ecc_priv_key_group      :[in]
*                ecc_priv_key_id         :[in]
*                r_job_slice_t job_slice :[in]
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_INVALID_CURVE_ID
****************************************************************************************/
static uint32_t s_01_InitializeJobEcdh(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_isd;
    job_ecc_t          *p_job_param;
    ISD_ECDH_t         *p_isd_param;

    ecc_key_t          *p_ecc_key;
    ecc_key_t          *p_pub_ecc_key;
    r_ecc_curve_t       priv_ecc_curve_id;
    uint8_t             is_allowed;
    uint32_t            err;
    uint32_t            skip_flg;

    p_isd       = p_job->p_ISD;
    p_job_param = &p_job->prm.job_ecc;
    p_isd_param = &p_isd->prm.ECDH;

    p_ecc_key     = NULL;
    p_pub_ecc_key = NULL;
    skip_flg = FLG_OFF;
    
    /* parameter check */
    err = s_01_ParamCheckJobEcdh(p_job, &skip_flg);

    if (FLG_OFF == skip_flg)
    {
        /* retrieve the ECC private key to be used */
        err = MH_KEY_GetEccKey(p_job, p_isd_param->ecc_priv_key_group, p_isd_param->ecc_priv_key_id, &p_ecc_key);
        if (ER_OK != err)
        {
            /* key not available: abort */
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
        /* --- Free ECC_Priv Key -------- */
        (void)MH_KEY_FreeKeyCache(p_job, p_isd_param->ecc_priv_key_group, p_isd_param->ecc_priv_key_id);
    }
    else
    {
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        (void)s_01_CanUseKeyIfDbgConnected((key_flags_t)p_ecc_key->p_properties->flags.disable_on_debug,
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
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }
    
    if (FLG_OFF == skip_flg)
    {
        /* make sure the key has a private scalar */
        if (NUM0 != (uint32_t)p_ecc_key->p_properties->priv_scalar_size_in_bits)
        {
            priv_ecc_curve_id = p_ecc_key->p_properties->ecc_curve;

            err = MH_KEY_GetEccKey(p_job,
                                   p_isd_param->ecc_pub_key_group,
                                   p_isd_param->ecc_pub_key_id,
                                   &p_pub_ecc_key);
            if (ER_OK != err)
            {
                /* key not available: abort */
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        } /* end of if (NUM0 != p_ecc_key->p_properties->priv_scalar_size_in_bits) */
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

    if (FLG_OFF == skip_flg)
    {
        p_job_param->resource_ctrl |= GET_PUB_KEY_ECC;
        
        (void)s_01_CanUseKeyIfDbgConnected((key_flags_t)p_pub_ecc_key->p_properties->flags.disable_on_debug,
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
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* make sure the public and the private keys are on the same curve */
        if (priv_ecc_curve_id != p_pub_ecc_key->p_properties->ecc_curve)
        {
            err = ER_MH_KEY_INVALID_CURVE_ID;
        }
        /* ED25519 and Ed448 can use by only eddsa service */
        else if ((ECC_CURVE_ED25519 == priv_ecc_curve_id) ||
                     (ECC_CURVE_ED448 == priv_ecc_curve_id))
        {
            err = ER_MH_KEY_INVALID_KEY_INDEX;
        }
        else
        {
            /* init the job slice */
            p_job->prm.job_ecc.job_slice = p_isd_param->job_slice;

            /* save the keys */
            p_job->prm.job_ecc.p_priv_key = (uint8_t *)p_ecc_key;
            p_job->prm.job_ecc.p_pub_key  = (uint8_t *)p_pub_ecc_key;
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    return (err);
}


/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDH_0007]:[ICUMFW_UD_S_01_ECDH_0007]
* Function name: s_01_ECDH_Init
* Description  : Initialization part of generating ECDH secret key
* Arguments    : p_job :[in-out]
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_ML_CTX_ALL_USE
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_INVALID_CURVE_ID
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_ECDH_Init(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_ISD;
    job_ecc_t          *p_JOB_PARAM;

    uint32_t            err;

    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_ecc;

    switch((uint32_t)p_job->process_phase)
    {
        case ON_HOLD:
            if (GET_CTX_NONE == ((uint32_t)p_JOB_PARAM->resource_ctrl & GET_CTX_ECC))
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

            /* ************************************************************************* */
            /* Initialization                                                        */
            /* ************************************************************************* */
            err = s_01_InitializeJobEcdh(p_job);
            if ((uint32_t)SERV_OK != err)
            {
                /* init error - break here */
                break;
            }
            else
            {
                /* Do nothing */
            }

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
* Function ID  : [ICUMFW_CD_S_01_ECDH_0004]:[ICUMFW_UD_S_01_ECDH_0004]
* Function name: s_01_ECDH_CalcScalar
* Description  : Calculation part of generating ECDH secret key
* Arguments    : p_job :[in-out]
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_L_ECC_SYS_ERROR
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
*                SERV_SYS_ERROR
****************************************************************************************/
static uint32_t s_01_ECDH_CalcScalar(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_isd;
    job_ecc_t          *p_job_param;
    ISD_ECDH_t         *p_isd_param;
    ecc_key_t          *p_ecc_key;
    ecc_key_t          *p_pub_ecc_key;
    ecc_ctx_t          *p_ecc_ctx;
    uint32_t            err;
    kdf_type_t          kdf_type;
    uint32_t            byte_size;
    hash_type_t         hash_type;
    uint16_t            hash_size;
    remap_ptr_manip_t   p_info_logical;
    uint32_t            processing_bits;
    e_pkc_sync_opt_t    sync_opt;

    /* Initialize parameter */
    p_isd         = p_job->p_ISD;
    p_job_param   = &p_job->prm.job_ecc;
    p_isd_param   = &p_isd->prm.ECDH;
    kdf_type      = E_LAST_ITEM_KDF_TYPE;

    /* get the pointer the the ECC public key used */
    p_ecc_key     = (ecc_key_t  *)p_job_param->p_priv_key;
    p_pub_ecc_key = (ecc_key_t  *)p_job_param->p_pub_key;
    p_ecc_ctx     = (ecc_ctx_t  *)p_job_param->p_ecc_ctx;

    switch((uint32_t)p_job->process_phase)
    {
        case PHASE_1:
            /* Get Ecc key size (Invalid curve id error does not occur because checked when update the key) */
            (void)MH_KEY_GetEccKeySize(p_ecc_key->p_properties->ecc_curve, &byte_size);
            /* convert internal Enum(kdf type) */
            (void)L_ECC_ToInternalKdfType(p_isd_param->kdf_algo, &kdf_type);
            /* convert internal Enum(hash type) */
            (void)L_HASH_GetInternalHashType(p_isd_param->hash_primitive, &hash_type, &hash_size);
            /* Get Remap(Convert Physical address -> Logical Address) */
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_isd_param->p_info, &p_info_logical.val);

            /* Set KDF parameter */
            p_ecc_ctx->kdf_ctx.kdf_algo = kdf_type;
            p_ecc_ctx->kdf_ctx.hash_type = hash_type;
            p_ecc_ctx->kdf_ctx.secret.shared_secret.size_in_bytes = byte_size;
            p_ecc_ctx->kdf_ctx.addition.info.p_data = p_info_logical.p_ptr;
            p_ecc_ctx->kdf_ctx.addition.info.size_in_bytes = p_isd_param->info_size_in_bytes;
            p_ecc_ctx->kdf_ctx.count.counter = p_isd_param->counter;
            p_ecc_ctx->kdf_ctx.deriv_key_size_in_bytes = (uint8_t)p_isd_param->key_size_in_bytes;
            /* The following parameters are set in the PKC library */
            p_ecc_ctx->kdf_ctx.secret.shared_secret.p_data = NULL;
            p_ecc_ctx->kdf_ctx.p_deriv_key = NULL;
            /* Set ecc curve id */
            p_ecc_ctx->curve_id = p_ecc_key->p_properties->ecc_curve;
            /* Set ecc private key */
            p_ecc_ctx->scalar_bin.key_length = p_ecc_key->p_priv_scalar->size;
            p_ecc_ctx->scalar_bin.p_d = (uint8_t *)&p_ecc_key->p_priv_scalar->value[0u];
            p_ecc_ctx->scalar_bin.p_qx = (uint8_t *)&p_pub_ecc_key->p_pub_point->x.value[0u];
            p_ecc_ctx->scalar_bin.p_qy = (uint8_t *)&p_pub_ecc_key->p_pub_point->y.value[0u];
            /* Set ecc public key */
            p_ecc_ctx->point_bin.key_length = p_pub_ecc_key->p_pub_point->x.size;
            p_ecc_ctx->point_bin.p_d = NULL;
            p_ecc_ctx->point_bin.p_qx = (uint8_t *)&p_pub_ecc_key->p_pub_point->x.value[0u];
            p_ecc_ctx->point_bin.p_qy = (uint8_t *)&p_pub_ecc_key->p_pub_point->y.value[0u];

            if ((ECC_CURVE_CURVE25519 == p_ecc_ctx->curve_id) ||
                (ECC_CURVE_CURVE448 == p_ecc_ctx->curve_id))
            {
                /* Setup the base parameter of ECDHx for PKC library */
                err = L_PKC_EcdhXSetup(p_ecc_ctx, &p_ecc_ctx->kdf_info);
                if (ER_OK == err)
                {
                    /* Perform ECDHX initial processing for PKC library */
                    err = L_PKC_EcdhXInit(&p_ecc_ctx->scalar_bin,
                                        &p_ecc_ctx->point_bin,
                                        &p_ecc_ctx->ctx.ecdhx);
                }
                else
                {
                    /* abort error */
                }
            }
            else
            {
                /* Setup the base parameter of ECDH for PKC library */
                err = L_PKC_EcdhSetup(p_ecc_ctx, &p_ecc_ctx->kdf_info);
                if (ER_OK == err)
                {
                    /* Perform ECDH initial processing for PKC library */
                    err = L_PKC_EcdhInit(&p_ecc_ctx->scalar_bin,
                                        &p_ecc_ctx->point_bin,
                                        &p_ecc_ctx->ctx.ec);
                }
                else
                {
                    /* abort error */
                }
            }

            if (ER_OK == err)
                    {
                        p_job->process_phase = PHASE_2a;
                    }
                    else
                    {
                /* Do nothing */
                    }
                    break;

        case PHASE_2a:
            if ((ECC_CURVE_CURVE25519 == p_ecc_ctx->curve_id) ||
                (ECC_CURVE_CURVE448 == p_ecc_ctx->curve_id))
            {
                if (NUM0 == (uint32_t)p_job_param->job_slice)
                {
                    processing_bits = L_PKC_PROCESSING_BITS_MAX;
                }
                else
                {
                    processing_bits = NUM1;
                }
                /* Perform ECDHX processing for PKC library */
                err = L_PKC_EcdhXOperate(&p_ecc_ctx->ctx.ecdhx, processing_bits);
                if (ER_OK == err)
                {
                    p_job->process_phase = PHASE_3;
                }
                else if (ER_IN_PROCESS == err)
                {
                    p_job->process_phase = PHASE_2;
                    err = ER_OK;
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                if (NUM0 == (uint32_t)p_job_param->job_slice)
                {
                    sync_opt = PKC_SYNC_OPT_SYNC;
                }
                else
                {
                    sync_opt = PKC_SYNC_OPT_ASYNC;
                }
                /* Perform ECDH processing for PKC library */
                err = L_PKC_EcdhOperate(&p_ecc_ctx->ctx.ec, sync_opt);
                if (ER_OK == err)
                {
                    if (PKC_SYNC_OPT_SYNC == sync_opt)
                    {
                        p_job->process_phase = PHASE_3;
                    }
                    else
                    {
                        p_job->process_phase = PHASE_2;
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
            break;

        case PHASE_2:
            if ((ECC_CURVE_CURVE25519 == p_ecc_ctx->curve_id) ||
                (ECC_CURVE_CURVE448 == p_ecc_ctx->curve_id))
            {
                /* Perform ECDHX processing for PKC library */
                err = L_PKC_EcdhXOperate(&p_ecc_ctx->ctx.ecdhx, NUM1);
            }
            else
            {
                /* Perform ECDH processing for PKC library */
                err = L_PKC_EccChkPkccBusy();
            }
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
                /* Do nothing */
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
* Function ID  : [ICUMFW_CD_S_01_ECDH_0005]:[ICUMFW_UD_S_01_ECDH_0005]
* Function name: s_01_ECDH_DeriveSharedKey
* Description  : Key derivation part of generating ECDH secret key
* Arguments    : p_job :[in-out]
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_WRITE_PROTECTED
*                ER_MH_KEY_WRITING
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_NVM_PROG
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_DEST_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                SERV_CRYPT_ERROR
****************************************************************************************/
static uint32_t s_01_ECDH_DeriveSharedKey(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_isd;
    job_ecc_t          *p_job_param;
    ISD_ECDH_t         *p_isd_param;
    ecc_ctx_t          *p_ecc_ctx;
    aes_key_t           aes_key;
    uint8_t             shared_key[R_CRYPT_SMALLBIGNUM_BYTESIZE];
    uint32_t            err;
    st_pkc_data_t       shared_key_data;

    /* Initialize parameter */
    p_isd         = p_job->p_ISD;
    p_job_param   = &p_job->prm.job_ecc;
    p_isd_param   = &p_isd->prm.ECDH;
    L_Toolbox_MemSet((uint8_t *)&aes_key, DATA_ZERO, (int16_t)sizeof(aes_key_t));
    L_Toolbox_MemSet(&shared_key[0],      DATA_ZERO, (int16_t)sizeof(shared_key));
    p_ecc_ctx     = (ecc_ctx_t  *)p_job_param->p_ecc_ctx;

    /* Set shared key */
    shared_key_data.len = sizeof(shared_key);
    shared_key_data.p_data = shared_key;

    if ((ECC_CURVE_CURVE25519 == p_ecc_ctx->curve_id) ||
        (ECC_CURVE_CURVE448 == p_ecc_ctx->curve_id))
    {
        /* Perform ECDHX termination processing for PKC library */
        err = L_PKC_EcdhXFinal(&p_ecc_ctx->ctx.ecdhx, &shared_key_data);
    }
    else
    {
        /* Perform ECDH termination processing for PKC library */
        err = L_PKC_EcdhFinal(&p_ecc_ctx->ctx.ec, &shared_key_data);
    }

    if (ER_OK == err)
    {
            /* ************************************************************************* */
            /* Initialize the RAM key */
            /* ************************************************************************* */
        aes_key.key_group    = p_isd_param->shared_key_group;
        aes_key.key_id       = p_isd_param->shared_key_id;
        L_Toolbox_MemCpy((uint8_t *)aes_key.value, shared_key_data.p_data,
                            (int16_t)p_ecc_ctx->kdf_ctx.deriv_key_size_in_bytes);

        aes_key.properties.size_in_bytes           = (key_size_t)shared_key_data.len;
        aes_key.properties.flags.encryption        = 1u;
        aes_key.properties.flags.decryption        = 1u;
        aes_key.properties.flags.cmac_generation   = 1u;
        aes_key.properties.flags.cmac_verification = 1u;
        aes_key.properties.flags.disable_on_debug  = 0u;
        aes_key.properties.flags.no_wild_card      = 0u;
        aes_key.properties.flags.write_protected   = 0u;
        aes_key.properties.flags.invalidate        = 0u;
        aes_key.properties.flags.plain_key         = 1u;
        err = MH_KEY_SetAesKey(p_job, E_KEY_OPE_FIRST, &aes_key);
        /* ************************************************************************* */
        /* Finished */
        /* ************************************************************************* */
        /* --- Free ECC_Context ---- */
        (void)ML_CTX_Free(CTX_TYPE_ECC, p_job_param->p_ecc_ctx);
        p_job_param->resource_ctrl &= ~GET_CTX_ECC;

        /* --- Free ECC_Pub Key -------- */
        (void)MH_KEY_FreeKeyCache(p_job, p_isd_param->ecc_pub_key_group, p_isd_param->ecc_pub_key_id);
        p_job_param->resource_ctrl &= ~GET_PUB_KEY_ECC;

        p_job->process_phase = ON_HOLD;
    }
    /* end of if (ER_OK == err) */
    else
    {
        err = (uint32_t)SERV_CRYPT_ERROR;
    }

    return (err);
}


/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_ECDH_0006]:[ICUMFW_UD_S_01_ECDH_0006]
* Function name: s_01_ECDH_Finish
* Description  : Finish part of generating ECDH secret key
* Arguments    : p_job :[in-out]
*                err :[in]
* Return Value : none
****************************************************************************************/
static void s_01_ECDH_Finish(icum_job_t *p_job, const uint32_t err)
{
    r_icumif_isd_t     *p_ISD;
    job_ecc_t          *p_JOB_PARAM;
    ISD_ECDH_t         *p_ISD_PARAM;

    p_ISD         = p_job->p_ISD;
    p_JOB_PARAM   = &p_job->prm.job_ecc;
    p_ISD_PARAM   = &p_ISD->prm.ECDH;

    if ((uint32_t)SERV_OK != err)
    {
        /* error: terminate the service */
        (void)s_01_ConvertErr(err, &p_ISD->service_result);
        p_job->process_phase = ON_HOLD;

        /* free context when illegal end */
        if (GET_CTX_ECC == ((uint32_t)p_JOB_PARAM->resource_ctrl & GET_CTX_ECC))
        {
            (void)ML_CTX_Free(CTX_TYPE_ECC, p_JOB_PARAM->p_ecc_ctx);
        }
        else
        {
            /* Do nothing */
        }

        if (GET_PUB_KEY_ECC == ((uint32_t)p_JOB_PARAM->resource_ctrl & GET_PUB_KEY_ECC))
        {
            (void)MH_KEY_FreeKeyCache(p_job, p_ISD_PARAM->ecc_pub_key_group, p_ISD_PARAM->ecc_pub_key_id);
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
        if (ON_HOLD == (uint32_t)p_job->process_phase)
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
* Function ID  : [ICUMFW_CD_S_01_ECDH_0001]:[ICUMFW_UD_S_01_ECDH_0001]
* Function name: S_01_MakeEcdhSecretKey
* Description  : Using the key agreement protocol ECDH, a shared secret key between the 
*                two parties is generated and the AES RAM key is updated.
* Arguments    : ecc_priv_key_group :[in]
*                ecc_priv_key_id    :[in]
*                ecc_pub_key_group  :[in]
*                ecc_pub_key_id     :[in]
*                shared_key_group   :[in]
*                shared_key_id      :[in]
*                kdf_algo           :[in]
*                hash_primitive     :[in]
*                job_slice          :[in]
*                *p_info            :[in]
*                counter            :[in]
*                info_size_in_bytes :[in]
*                service_result     :[out]
* Return Value : none
****************************************************************************************/
void S_01_MakeEcdhSecretKey(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_ISD;
    uint32_t            err;

    p_ISD         = p_job->p_ISD;

    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;

    /**********************************************************************************
    * Initialization phase
    ***********************************************************************************/

    /* indicate service is in process */
    p_ISD->service_result = SERV_IN_PROCESS;

    switch ((uint32_t)p_job->process_phase)
    {
        case ON_HOLD:
            err = s_01_ECDH_Init(p_job);
            break;
        case PHASE_1:
        case PHASE_2a:
        case PHASE_2:
            err = s_01_ECDH_CalcScalar(p_job);
            break;
        case PHASE_3:
            err = s_01_ECDH_DeriveSharedKey(p_job);
            break;
        default:
            /* should never occur */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }

    s_01_ECDH_Finish(p_job, err);

    return;
}

