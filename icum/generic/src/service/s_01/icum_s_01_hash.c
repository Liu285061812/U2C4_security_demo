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
#include "r_icumif_api.h"
#include "icum_s_pub.h"
#include "icum_ml_ctx_pub.h"
#include MM_FLASH_PUBLIC_H
/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"

#include "renesas_types.h"
#include "icum_s_01_common.h"
#include "icum_l_toolbox_pub.h"
#include "icum_service_common.h"

/**********************************************************************************
* Local types
**********************************************************************************/
#define MAX_U32                 (0xFFFFFFFFu)

/**********************************************************************************
* Local variables
**********************************************************************************/

/**********************************************************************************
* Functions
***********************************************************************************/
static uint32_t s_01_InitHash(icum_job_t *p_job, hash_ctx_t *p_hash_ctx);
static uint32_t s_01_FinalizeHash(icum_job_t *p_job, hash_ctx_t *p_hash_ctx);
static uint32_t s_01_GenerateHashSub(icum_job_t *p_job, r_job_cycle_t job_cycle);
static uint32_t s_01_IsHashTypeSha3(r_hash_primitive_t hash_primitive);

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0002]:[ICUMFW_UD_S_01_PK_0002]
* Function name: s_01_InitHash()
* Description  : # Initialize the hash process.
*                  Set hash_type and hash_size, and initialize HASH processing.
* Arguments    : (IN-OUT)  p_job : Pointer to Job_parameter
*                (IN-OUT)  p_hash_ctx : Pointer to HASH Context
* Return Value : SERV_PRM_ERROR
*              : SERV_SRC_MEM_ERROR
*              : SERV_IN_PROCESS
*              : SERV_DEST_MEM_ERROR
*              : SERV_CRYPT_ERROR
****************************************************************************************/
static uint32_t s_01_InitHash(icum_job_t *p_job, hash_ctx_t *p_hash_ctx)
{
    r_icumif_isd_t      *p_ISD;
    hash_type_t          hash_type;
    uint16_t             hash_size;
    uint32_t             err;
    uint8_t              result;
    uint32_t             skip_flg;
    hash_ope_prm_t       hash_ope_prm;
    uint32_t             out_area_size;

    skip_flg = FLG_OFF;
    out_area_size = NUM0;

    L_Toolbox_MemSet((uint8_t *)&hash_ope_prm, (uint8_t)NUM0, (int16_t)sizeof(hash_ope_prm_t));
    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;

    /* copy the source address within the job description */
    p_job->prm.job_hash.p_source = p_ISD->prm.HASH.p_message;

    /* indicates how many bytes to process in the job */
    p_job->prm.job_hash.nb_bytes = p_ISD->prm.HASH.message_size_in_bytes;

    /* determin hash_type */
    err = L_HASH_GetInternalHashType(p_ISD->prm.HASH.hash_primitive, &hash_type, &hash_size);

    if(hash_size == (uint16_t)0)
    {
        /* error on hash type */
        p_ISD->service_result = SERV_PRM_ERROR;
        skip_flg = FLG_ON;
    }
    else
    {
        if (NUM0 == (uint32_t)p_ISD->prm.HASH.hash_size_in_bytes)
        {
            out_area_size = (uint32_t)hash_size;
        }
        else
        {
            out_area_size = (uint32_t)p_ISD->prm.HASH.hash_size_in_bytes;
        }
    }

    /* reset hash size to zero */
    p_ISD->prm.HASH.hash_size_in_bytes = NUM0;
    if (skip_flg == FLG_OFF)
    {
        /* make sure the destination address range is valid */
        (void)MM_FLS_CheckMemory((uint32_t)p_ISD->prm.HASH.p_hash,
                                 (uint32_t)hash_size,
                                 FLS_CHK_MEM_DESTINATION,
                                 p_job->expansion_service,
                                 &result);

        if(result != FLS_CHK_OK)
        {
            p_ISD->service_result = SERV_DEST_MEM_ERROR;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Check if DMA transfer is applicable and set the transfer information in the operating parameters */
            if ((JOB_ALL_AT_ONCE == p_ISD->prm.HASH.job_cycle) && (NUM0 == (uint32_t)p_ISD->prm.HASH.job_slice))
            {
                hash_ope_prm.use_dma_transfer = L_HASH_ChkDMAApplicable(hash_type, 
                                            p_ISD->prm.HASH.p_message, p_ISD->prm.HASH.message_size_in_bytes, 
                                            p_ISD->prm.HASH.p_hash,    out_area_size);
            }
            else
            {
                /* do nothing */
            }

            hash_ope_prm.total_msg_size_in_bytes = p_ISD->prm.HASH.total_msg_size_in_bytes;
            err =  L_HASH_SetOpePrm(p_hash_ctx, &hash_ope_prm);
            if (ER_OK == err)
            {
                p_job->prm.job_hash.processed_data_size = NUM0;
                p_job->prm.job_hash.total_msg_size_in_bytes = p_ISD->prm.HASH.total_msg_size_in_bytes;

                /* in process */
                p_ISD->service_result = SERV_IN_PROCESS;
                err = L_HASH_Init(p_hash_ctx, hash_type);
                if (ER_OK != err)
                {
                    p_ISD->service_result = SERV_CRYPT_ERROR;
                }
                else
                {
                    /* do nothing */
                }
            } /* end of if (ER_OK == err) */
            else
            {
                p_ISD->service_result = SERV_PRM_ERROR;
            }
        }
    }
    else
    {
        /* do nothing */
    }

    return(err);
}

/*===========================================================================
  Finalize the hash process - get the plain hash value
  [PRIVATE]
  ===========================================================================*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0003]:[ICUMFW_UD_S_01_PK_0003]
* Function name: s_01_FinalizeHash()
* Description  : # Call HASH termination processing and release the context.
* Arguments    : (IN-OUT)  p_job : Pointer to Job_parameter
*                (IN-OUT)  p_hash_ctx : Pointer to HASH Context
* Return Value : ER_OK
*              : R_SALIF_ER_CRY_INVALID_ARG
****************************************************************************************/
static uint32_t s_01_FinalizeHash(icum_job_t *p_job, hash_ctx_t *p_hash_ctx)
{
    r_icumif_isd_t      *p_ISD;
    hash_descriptor_t   *p_hash_desc;
    uint32_t             err;
    uint32_t             type_check_sha3;
    remap_ptr_manip_t     hash_logical;

    type_check_sha3 = NUM0;
    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;

    /* no more data to process! */
    p_job->prm.job_hash.nb_bytes = 0;

    /* retrieve the hash context */
    p_hash_desc  = &(p_hash_ctx->hash_desc);

    if (p_job->keep.job_cycle == 1u)
    {
        p_job->keep.job_cycle = 0u;
    }
    else
    {
        /* do nothing */
    }

    type_check_sha3 = s_01_IsHashTypeSha3(p_ISD->prm.HASH.hash_primitive);

    if ((NUM1 != type_check_sha3) ||
        (p_job->prm.job_hash.total_msg_size_in_bytes == p_job->prm.job_hash.processed_data_size))
    {
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.HASH.p_hash, &hash_logical.val);
        /* terminate: get the hash value */
        err = L_HASH_Finish(p_hash_ctx, hash_logical.p_ptr);

        /* indicate the size of the hash */
        p_ISD->prm.HASH.hash_size_in_bytes = (uint16_t)(p_hash_desc->hash_bitlen / 8UL);

    } /* end of if ((NUM1 != type_check_sha3) || */
      /*            (p_job->prm.job_hash.total_msg_size_in_bytes == p_job->prm.job_hash.processed_data_size)) */
    else
    {
        err = ER_PARAM;
    }

    /* free the hash context */
    (void)ML_CTX_Free(CTX_TYPE_HASH, (uint32_t *)p_hash_ctx);

    if (ER_OK == err)
    {
        /* indicate service completion */
        p_ISD->service_result = SERV_OK;
    }
    else if (ER_PARAM == err)
    {
        p_ISD->service_result = SERV_PRM_ERROR;
    }
    else
    {
        p_ISD->service_result = SERV_CRYPT_ERROR;
    }

    return(err);
}

/**********************************************************************************
* ICU-M service
***********************************************************************************/

/*===========================================================================
  Hash a message and format according to specific standard
  Command parameters extracted from the ICU-M service descriptor (ISD)
  [PUBLIC]
  ===========================================================================*/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0046]:[ICUMFW_UD_S_01_PK_0046]
* Function name: s_01_GenerateHashSub()
* Description  : Sub routine of the hash generation
* Arguments    : (IN-OUT)  p_job               : Pointer to Job_parameter
*                (IN)      job_cycle           : Job cycle
* Return Value : R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_PENDING
*                R_SALIF_ER_IN_PROCESS
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                R_SALIF_ER_CRY_TIMEOUT
*                R_SALIF_ER_CRY_AES_SEQUENCE_ERROR
*                R_SALIF_ER_CRY_RUNNING
*                R_SALIF_ER_CRY_INVALID_ARG
*                R_SALIF_ER_CRY_ERROR
*                R_SALIF_ER_CRY_BUFFER_OVERFLOW
*                R_SALIF_ER_CRY_INVALID_SIZE
*                R_SALIF_ER_CRY_INVALID_PACKET
*                R_SALIF_ER_CRY_KEYGEN_RETRYOVER
*                R_SALIF_ER_CRY_IN_PROGRESS
*                R_SALIF_ER_CRY_NOT_SUPPORT
*                R_SALIF_ER_CRY_STATUS_ERROR
*                R_SALIF_ER_CRY_PKCC_ERROR
*                R_SALIF_ER_CRY_DECOMP_ERROR
****************************************************************************************/
static uint32_t s_01_GenerateHashSub(icum_job_t *p_job, r_job_cycle_t job_cycle)
{
    r_icumif_isd_t  *p_ISD;
    uint32_t        *p_hash_ctx;

    uint32_t         err;
    uint8_t          result;
    uint32_t         skip_flg;
    uint32_t         remaining_byte_size;
    uint32_t         job_slice_size;
    uint32_t         type_check_sha3;

    remap_ptr_manip_t     source_logical;
    type_check_sha3 = NUM0;


    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;
    err = ER_OK;
    skip_flg = FLG_OFF;

    if (NULL != p_job->prm.job_hash.p_hash_ctx)
    {
        p_hash_ctx = p_job->prm.job_hash.p_hash_ctx;
    
        /* retrieves the (remaining) number of remaining bytes to process */
        /* and the number of bytes to process per job round */
        
        /* All previously received messages are processed. or first JOB_UPDATE */
        if (p_job->prm.job_hash.nb_bytes == 0UL)
        {
            /* copy the source address within the job description */
            p_job->prm.job_hash.p_source = p_ISD->prm.HASH.p_message;
        
            /* indicates how many bytes to process in the job */
            p_job->prm.job_hash.nb_bytes = p_ISD->prm.HASH.message_size_in_bytes;
        }
        else
        {
            /* do nothing */
        }
        
        remaining_byte_size = p_job->prm.job_hash.nb_bytes;
        job_slice_size = (uint32_t)p_job->prm.job_hash.nb_bytes_per_round;
        
        /* If "remaining_byte_size" is greater than 0, L_HASH_Update() is always performed below. */
        if (remaining_byte_size > 0UL)
        {
            /* make sure the source address range is valid */
            (void)MM_FLS_CheckMemory((uint32_t)p_job->prm.job_hash.p_source,
                                      p_job->prm.job_hash.nb_bytes,
                                      FLS_CHK_MEM_SOURCE,
                                      p_job->expansion_service,
                                      &result);
            if(result != FLS_CHK_OK)
            {
                p_ISD->service_result = SERV_SRC_MEM_ERROR;
                (void)ML_CTX_Free(CTX_TYPE_HASH, p_hash_ctx);
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
    }
    else
    {
        p_ISD->service_result = SERV_SEQUENCE_ERROR;
        skip_flg = FLG_ON;
    }

    if (skip_flg == FLG_OFF)
    {
        type_check_sha3 = s_01_IsHashTypeSha3(p_ISD->prm.HASH.hash_primitive);

        if ((NUM1 != type_check_sha3) ||
            (((MAX_U32 - p_job->prm.job_hash.processed_data_size) >= remaining_byte_size) &&
             (p_job->prm.job_hash.total_msg_size_in_bytes >=
             (remaining_byte_size + p_job->prm.job_hash.processed_data_size))))
        {
            /* Get Remap(Convert Physical address -> Logical Address) */
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_hash.p_source, &source_logical.val);
            if(job_slice_size == 0uL)
            {
                if (remaining_byte_size > 0uL)
                {
                    /* process the hash all at once */
                    err = L_HASH_Update((hash_ctx_t *)p_hash_ctx, source_logical.p_ptr, remaining_byte_size);
                    if (ER_OK != err)
                    {
                        /* error on hash processing */
                        p_ISD->service_result = SERV_CRYPT_ERROR;
                        (void)ML_CTX_Free(CTX_TYPE_HASH, p_hash_ctx);
                        skip_flg = FLG_ON;
                    }
                    else
                    {
                        p_job->prm.job_hash.nb_bytes = 0UL;
                        p_job->prm.job_hash.processed_data_size =
                        p_job->prm.job_hash.processed_data_size + remaining_byte_size;
                    }
                }
                else
                {
                    /* do nothing */
                }

                if (skip_flg == FLG_OFF)
                {
                    if (((uint8_t)job_cycle & (uint8_t)JOB_FINISH) != (uint8_t)0)
                    {
                        /* final step */
                        (void)s_01_FinalizeHash(p_job, (hash_ctx_t *)p_hash_ctx);
                    }
                    else 
                    {
                        p_job->keep.job_cycle = 1u;
                        p_ISD->service_result = SERV_OK;
                    }
                }
                else
                {
                    /* do nothing */
                }

            }
            else
            {
                /* The remaining bytes is larger than the bytes processed at one time, and JOB_UPDATE is omitted.  */
                /* Same as specifying to process all at once(job_slice = 0). Therefore, even if the n is less than */
                /* or equal to the m, termination processing is performed if it is JOB_FINISH.                     */
                if(remaining_byte_size <= job_slice_size)
                {
                    if (remaining_byte_size > 0uL)
                    {
                        /* last job round */
                        err = L_HASH_Update((hash_ctx_t *)p_hash_ctx, source_logical.p_ptr, remaining_byte_size);
                        if (ER_OK != err)
                        {
                            /* error on hash processing */
                            p_ISD->service_result = SERV_CRYPT_ERROR;
                            (void)ML_CTX_Free(CTX_TYPE_HASH, p_hash_ctx);
                            skip_flg = FLG_ON;
                        }
                        else
                        {
                            p_job->prm.job_hash.nb_bytes = 0UL;
                            p_job->prm.job_hash.processed_data_size =
                            p_job->prm.job_hash.processed_data_size + remaining_byte_size;
                        }
                    }
                    else
                    {
                        /* do nothing */
                    }

                    if (skip_flg == FLG_OFF)
                    {
                        if (((uint8_t)job_cycle & (uint8_t)JOB_FINISH) != 0)
                        {
                            /* final step */
                            (void)s_01_FinalizeHash(p_job, (hash_ctx_t *)p_hash_ctx);
                        }
                        else 
                        {
                            p_job->keep.job_cycle = 1u;
                            p_ISD->service_result = SERV_OK;
                        }
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
                else
                {
                    /* next job round */
                    err = L_HASH_Update((hash_ctx_t *)p_hash_ctx, source_logical.p_ptr, job_slice_size);
                    if (ER_OK != err)
                    {
                        /* error on hash processing */
                        p_ISD->service_result = SERV_CRYPT_ERROR;
                        (void)ML_CTX_Free(CTX_TYPE_HASH, p_hash_ctx);
                        skip_flg = FLG_ON;
                    }
                    else
                    {
                        /* update pointers and number of bytes remaining */
                        p_job->prm.job_hash.nb_bytes -= job_slice_size;
                        p_job->prm.job_hash.p_source += job_slice_size;
                        p_job->prm.job_hash.processed_data_size =
                        p_job->prm.job_hash.processed_data_size + job_slice_size;

                        p_ISD->service_result = SERV_IN_PROCESS;
                    }
                } /* end of else */
            } /* end of else */
        } /* end of if ((NUM1 != type_check_sha3) ||                                                     */
          /*            (((MAX_U32 - p_job->prm.job_hash.processed_data_size) >= remaining_byte_size) && */
          /*             (p_job->prm.job_hash.total_msg_size_in_bytes >=                                 */
          /*             (remaining_byte_size + p_job->prm.job_hash.processed_data_size))))              */
        else
        {
            p_ISD->service_result = SERV_PRM_ERROR;
            (void)ML_CTX_Free(CTX_TYPE_HASH, p_hash_ctx);
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    return err;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0053]:[ICUMFW_UD_S_01_PK_0053]
* Function name: s_01_IsHashTypeSha3()
* Description  : # Check if the Hash primitive is SHA3 or not.
* Arguments    : (IN) hash_primitive : hash primitive
* Return Value : NUM0 : Hash type is not SHA3
*                NUM1 : Hash type is SHA3
****************************************************************************************/
static uint32_t s_01_IsHashTypeSha3(r_hash_primitive_t hash_primitive)
{
    uint32_t rtn;

    rtn = NUM0;

    if (HASH_PRIMITIVE_SHA2_512_256 >= hash_primitive)
    {
        /* do nothing */
    }
    else
    {
        rtn = NUM1;
    }

    return (rtn);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0001]:[ICUMFW_UD_S_01_PK_0001]
* Function name: S_01_GenerateHash()
* Description  : # Data is exchanged with dispatcher, and in it,
*                  control of HASH initialization, HASH intermediate processing,
*                  HASH termination processing is performed by processing result (state)
*                  and values such as job_cycle.
* Arguments    : (IN-OUT)  p_job : Pointer to Job_parameter
* Return Value : SERV_REQUEST
*              : SERV_PENDING
*              : SERV_IN_PROCESS
*              : SERV_CRYPT_ERROR
*              : SERV_SRC_MEM_ERROR
*              : SERV_DEST_MEM_ERROR
*              : SERV_PRM_ERROR
****************************************************************************************/
void S_01_GenerateHash(icum_job_t *p_job)
{
    r_icumif_isd_t  *p_isd;
    uint32_t        *p_hash_ctx;
    r_job_cycle_t    job_cycle;

    uint32_t         err;
    uint32_t         skip_flg;

    /* retrieve the service handler */
    p_isd = p_job->p_ISD;
    err = ER_OK;
    skip_flg = FLG_OFF;

    /* 1. range check of job cycle */
    if ((JOB_INIT | JOB_UPDATE | JOB_FINISH) < p_isd->prm.HASH.job_cycle)
    {
        p_isd->service_result = SERV_PRM_ERROR;
        skip_flg = FLG_ON;
    }
    /* 2. When JOB_ALL_AT_ONCE is specified for job_cycle */
    else if (JOB_ALL_AT_ONCE == p_isd->prm.HASH.job_cycle)
    {
        job_cycle = (r_job_cycle_t)(JOB_INIT | JOB_UPDATE | JOB_FINISH);
    }
    else
    {
        job_cycle = p_isd->prm.HASH.job_cycle;
    }

    if (FLG_OFF == skip_flg)
    {
        /* service just triggered? */
        if ((NUM0 != (job_cycle & JOB_INIT))
         && ((p_isd->service_result == SERV_REQUEST) || (p_isd->service_result == SERV_PENDING)))
        {
            /* get a Hash execution context */
            err = ML_CTX_Get(CTX_TYPE_HASH, &p_hash_ctx);

            if (ER_ML_CTX_ALL_USE == err)
            {
                p_isd->service_result = SERV_PENDING;
                skip_flg = FLG_ON;
            }
            else if (ER_OK != err)
            {
                /* should never occur */
                p_isd->service_result = SERV_UNAVAILABLE;
                skip_flg = FLG_ON;
            }
            else
            {
                p_job->prm.job_hash.p_hash_ctx = p_hash_ctx;

                /* have an Hash context to use: initialize it */
                (void)s_01_InitHash(p_job, (hash_ctx_t *)p_hash_ctx);

                /* test for errors after the initialization phase */
                if (SERV_IN_PROCESS != p_isd->service_result)
                {
                    (void)ML_CTX_Free(CTX_TYPE_HASH, p_hash_ctx);
                    skip_flg = FLG_ON;
                }
                else
                {
                    /* do nothing */
                }

                /* get the job slice configured */
                p_job->prm.job_hash.nb_bytes_per_round = p_isd->prm.HASH.job_slice;
            }
        } /* end of if ((NUM0 != (job_cycle & JOB_INIT))... */
        else
        {
            /* do nothing */
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* job_cycle : only JOB_INIT */
        if (JOB_INIT == job_cycle)
        {
            p_job->prm.job_hash.nb_bytes = NUM0;
            p_job->keep.job_cycle = 1u;
            p_isd->service_result = SERV_OK;
            skip_flg = FLG_ON;
        }
        else
        {
            /* do nothing */
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        err = s_01_GenerateHashSub(p_job, job_cycle);
    }
    else
    {
        /* do nothing */
    }

    if ((ER_OK != err) && (1u == (uint32_t)p_job->keep.job_cycle))
    {
        p_job->keep.job_cycle = 0u;
    }
    else
    {
        /* do nothing */
    }
}

