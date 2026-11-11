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
#include "icum_l_cry_pk_pub.h"
#include "icum_common_pub.h"
#include "renesas_types.h"
#include "icum_s_pub.h"
#include "icum_ml_ctx_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_k_issd_pub.h"
#include "icum_d_comm_pub.h"
/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"

#include "icum_s_01_common.h"
#include "icum_l_toolbox_pub.h"
#include "icum_service_common.h"

/**********************************************************************************
* Local variables
**********************************************************************************/

/**********************************************************************************
* Functions
***********************************************************************************/


/*===========================================================================
  Initialize the hmac process
  [PRIVATE]
  ===========================================================================*/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_HMAC_0002]:[ICUMFW_UD_S_01_HMAC_0002]
* Function name: s_01_InitHmac()
* Description  : # HASH type is determined and HMAC initialization processing is performed.
* Arguments    : (IN-OUT)  p_job : Pointer to Job_parameter
* Return Value : ER_PARAM
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
*                SERV_CRYPT_ERROR
*                SERV_IN_PROCESS
*                SERV_PRM_ERROR
*                SERV_INVALID_KEY_INDEX
*                SERV_KEY_NOT_USABLE
*                SERV_KEY_SLOT_EMPTY
****************************************************************************************/
static uint32_t s_01_InitHmac(icum_job_t *p_job)
{
    r_icumif_isd_t      *p_ISD;
    hash_ctx_t          *p_hash_ctx;
    hash_type_t          hash_type;
    void                *p_key;
    uint16_t             hash_size;
    uint32_t             err;
    uint8_t              result;
    uint32_t            skip_flg = FLG_OFF;
    remap_ptr_manip_t       msg_logical;

    err = ER_OK;

    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;

    /* retrieve the hmac context */
    p_hash_ctx = (hash_ctx_t *)p_job->prm.job_hmac.p_hash_ctx;

    (void)L_HASH_GetInternalHashType(p_ISD->prm.HMAC.hash_primitive, &hash_type, &hash_size);

    if (hash_size == (uint16_t)0)
    {
        /* error on hash type */
        err = ER_PARAM;
        skip_flg = FLG_ON;
    }
    else
    {
        /* do nothing */
    }

    if (skip_flg == FLG_OFF)
    {
        p_hash_ctx->hmac_state.hash_type = hash_type;
    
        if (p_ISD->prm.HMAC.hmac_size_in_bytes == (uint8_t)0)
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
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

    if (skip_flg == FLG_OFF)
    {
        /* note: in case of internal service: do not check the destination range */
        /* make sure the source address range is valid */
        err = MM_FLS_CheckMemory((uint32_t)p_ISD->prm.HMAC.p_msg,
                                 p_ISD->prm.HMAC.msg_size_in_bytes,
                                 FLS_CHK_MEM_SOURCE,
                                 p_job->expansion_service,
                                 &result);

        if (err != ER_OK)
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else if (result != FLS_CHK_OK)
        {
            err = (uint32_t)SERV_SRC_MEM_ERROR;
            skip_flg = FLG_ON;
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

    if (skip_flg == FLG_OFF)
    {
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.HMAC.p_msg, &msg_logical.val);

        /* copy the source address within the job description */
        p_job->prm.job_hmac.p_source = msg_logical.p_ptr;

        /* indicates how many bytes to process in the job */
        p_job->prm.job_hmac.nb_bytes = p_ISD->prm.HMAC.msg_size_in_bytes;

        if (p_ISD->prm.HMAC.hmac_operation == HMAC_VERIFICATION)
        {
            err = MM_FLS_CheckMemory((uint32_t)p_ISD->prm.HMAC.p_hmac,
                                     p_ISD->prm.HMAC.hmac_size_in_bytes,
                                     FLS_CHK_MEM_SOURCE,
                                     p_job->expansion_service,
                                     &result);

            if (err != ER_OK)
            {
                err = ER_PARAM;
                skip_flg = FLG_ON;
            }
            else if (result != FLS_CHK_OK)
            {
                err = (uint32_t)SERV_SRC_MEM_ERROR;
                skip_flg = FLG_ON;
            }
            else
            {
                /* do nothing */
            }
        }
        else if (p_ISD->prm.HMAC.hmac_operation == HMAC_GENERATION)
        {
            err = MM_FLS_CheckMemory((uint32_t)p_ISD->prm.HMAC.p_hmac,
                                     (uint32_t)hash_size,
                                     FLS_CHK_MEM_DESTINATION,
                                     p_job->expansion_service,
                                     &result);

            if (err != ER_OK)
            {
                err = ER_PARAM;
                skip_flg = FLG_ON;
            }
            else if (result != FLS_CHK_OK)
            {
                err = (uint32_t)SERV_DEST_MEM_ERROR;
                skip_flg = FLG_ON;
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }

    if (skip_flg == FLG_OFF)
    {
        err = MH_KEY_GetHmacKey(p_job, p_ISD->prm.HMAC.key_id, (hmac_key_t **)&p_key);
        if (err != ER_OK)
        {
            skip_flg = FLG_ON;
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

    if (skip_flg == FLG_OFF)
    {
        /* initialize the hmac */
        err = L_HMAC_Init(p_hash_ctx, (uint8_t*)((hmac_key_t *)p_key)->p_key, ((hmac_key_t *)p_key)->key_size_in_byte);
        if (ER_OK!= err)
        {
            (void)MH_KEY_FreeKeyCache(p_job, KEY_GRP_HMAC, p_ISD->prm.HMAC.key_id);
            err = (uint32_t)SERV_CRYPT_ERROR;
            skip_flg = FLG_ON;
        }
        else
        {
            err = ER_OK;
        }
    }
    else
    {
        /* do nothing */
    }

    return (err);
}

/*===========================================================================
  Finalize the hmac process
  [PRIVATE]
  ===========================================================================*/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_HMAC_0003]:[ICUMFW_UD_S_01_HMAC_0003]
* Function name: s_01_FinalizeHmac()
* Description  : # Performs HMAC termination processing, stores or collates HMAC values,
*                  and releases the context.
* Arguments    : (IN-OUT)  p_job : Pointer to Job_parameter
* Return Value : ER_OK
*                SERV_CRYPT_ERROR
****************************************************************************************/
static uint32_t s_01_FinalizeHmac(icum_job_t *p_job)
{
    r_icumif_isd_t     *p_ISD;
    uint8_t             p_hmac[64];    /* HMAC MAX Size 64 */
    hash_ctx_t         *p_hash_ctx;
    uint8_t             *hmac_gen;
    uint32_t            hmac_size;
    uint32_t            err;
    uint8_t             cmp_result;
    remap_ptr_manip_t       hmac_logical;


    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;

    /* indicate service completion */
    err = ER_OK;

    /* no more data to process! */
    p_job->prm.job_hmac.nb_bytes = 0;

    hmac_size = p_ISD->prm.HMAC.hmac_size_in_bytes;

    p_hash_ctx = (hash_ctx_t *)p_job->prm.job_hmac.p_hash_ctx;

    /* Get Remap(Convert Physical address -> Logical Address) */
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.HMAC.p_hmac, &hmac_logical.val);

    if (p_ISD->prm.HMAC.hmac_operation == HMAC_VERIFICATION)
    {
        /* terminate: get the hmac value */
        err = L_HMAC_Finish(p_hash_ctx, p_hmac, &(hmac_size));

        if (ER_OK != err)
        {
            err = (uint32_t)SERV_CRYPT_ERROR;
        }
        else if (p_ISD->prm.HMAC.hmac_size_in_bytes != hmac_size)
        {
            err = ER_PARAM;
        }
        else
        {
            hmac_gen = hmac_logical.p_ptr;

            cmp_result = L_Toolbox_MemCmp(hmac_gen, p_hmac, (int16_t)p_ISD->prm.HMAC.hmac_size_in_bytes);
            /* Results are only 0 or 1 */
            if (cmp_result == (uint8_t)1)
            {
                p_ISD->prm.HMAC.verification_result = VERIFICATION_FAIL;
            }
            else
            {
                p_ISD->prm.HMAC.verification_result = VERIFICATION_PASS;
            }
        }
    }
    else
    {
        /* terminate: get the hmac value */
        err = L_HMAC_Finish(p_hash_ctx, hmac_logical.p_ptr, &(hmac_size));

        if (ER_OK != err)
        {
            err = (uint32_t)SERV_CRYPT_ERROR;
        }
        else
        {
            p_ISD->prm.HMAC.hmac_size_in_bytes = (uint8_t)hmac_size;
        }
    }

    return (err);
}


/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_HMAC_0001]:[ICUMFW_UD_S_01_HMAC_0001]
* Function name: S_01_ProcessHmac()
* Description  : # A service that generates or collates HMAC values.
* Arguments    : (IN-OUT)  p_job : Pointer to Job_parameter
* Return Value : None
****************************************************************************************/
void S_01_ProcessHmac(icum_job_t *p_job)
{
    r_icumif_isd_t  *p_ISD;
    hash_ctx_t      *p_hash_ctx;

    uint32_t err;
    uint32_t remaining_byte_size;
    uint32_t job_slice_size;
    uint32_t skip_flg = FLG_OFF;

    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;

    /* service just triggered? */
    if ((p_ISD->service_result == SERV_REQUEST) || (p_ISD->service_result == SERV_PENDING))
    {
        /* get a Hash execution context */
        err = ML_CTX_Get(CTX_TYPE_HASH, (uint32_t **)&p_hash_ctx);
        if (err == ER_OK)
        {
            p_job->prm.job_hmac.p_hash_ctx = (uint32_t *)p_hash_ctx;

            err = s_01_InitHmac(p_job);
            /* test for errors after the initialization phase */
            if (err != ER_OK)
            {
                (void)ML_CTX_Free(CTX_TYPE_HASH, (uint32_t *)p_hash_ctx);
                (void)s_01_ConvertErr(err, (r_icumif_service_result_t *)&p_ISD->service_result);
                skip_flg = FLG_ON;
            }
            else
            {
                /* do nothing */
            }
            if (skip_flg == FLG_OFF)
            {
                p_ISD->service_result = SERV_IN_PROCESS;
                /* get the job slice configured */
                p_job->prm.job_hmac.nb_bytes_per_round = p_ISD->prm.HMAC.job_slice;
            }
            else
            {
                /* do nothing */
            }
        }
        else 
        {
            (void)s_01_ConvertErr(err, (r_icumif_service_result_t *)&p_ISD->service_result);
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }

    if (skip_flg == FLG_OFF)
    {
        p_hash_ctx = (hash_ctx_t *)p_job->prm.job_hmac.p_hash_ctx;

        /* retrieves the (remaining) number of remaining bytes to process */
        /* and the number of bytes to process per job round */
        remaining_byte_size = p_job->prm.job_hmac.nb_bytes;
        job_slice_size = p_job->prm.job_hmac.nb_bytes_per_round;

        if ((job_slice_size == 0uL) || (remaining_byte_size <= job_slice_size))
        {
            if (remaining_byte_size > 0uL)
            {
                /* process  all at once */
                err = L_HMAC_Update(p_hash_ctx, (uint8_t*)p_job->prm.job_hmac.p_source, remaining_byte_size);
                if (ER_OK != err)
                {
                    /* error on hash processing */
                    p_ISD->service_result = SERV_CRYPT_ERROR;
                    (void)MH_KEY_FreeKeyCache(p_job, KEY_GRP_HMAC, p_ISD->prm.HMAC.key_id);
                    (void)ML_CTX_Free(CTX_TYPE_HASH, (uint32_t *)p_hash_ctx);
                    skip_flg = FLG_ON;
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

            if (skip_flg == FLG_OFF)
            {
                /* final step */
                err = s_01_FinalizeHmac(p_job);

                (void)s_01_ConvertErr(err, (r_icumif_service_result_t *)&p_ISD->service_result);

                (void)MH_KEY_FreeKeyCache(p_job, KEY_GRP_HMAC, p_ISD->prm.HMAC.key_id);

                /* free the hash context */
                (void)ML_CTX_Free(CTX_TYPE_HASH, (uint32_t *)p_hash_ctx);
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            /* next job round */
            err = L_HMAC_Update(p_hash_ctx, (uint8_t*)p_job->prm.job_hmac.p_source, job_slice_size);
            if (ER_OK != err)
            {
                /* error on hash processing */
                p_ISD->service_result = SERV_CRYPT_ERROR;
                (void)MH_KEY_FreeKeyCache(p_job, KEY_GRP_HMAC, p_ISD->prm.HMAC.key_id);
                (void)ML_CTX_Free(CTX_TYPE_HASH, (uint32_t *)p_hash_ctx);
            }
            else
            {
                /* update pointers and number of bytes remaining */
                p_job->prm.job_hmac.nb_bytes -= job_slice_size;
                p_job->prm.job_hmac.p_source += job_slice_size;
                p_ISD->service_result = SERV_IN_PROCESS;
            }
        }
    }
    else
    {
        /* do nothing */
    }
}

