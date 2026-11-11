/******************************************************************************/
/* Component Name   Service 01 AES Cipher file                                */
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
#include "icum_s_01_aes_common.h"
/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include MM_FLASH_PUBLIC_H

/* DATA SIZE */
#define DATA_ONE            (0x01u)
#define BLOCK_BYTE_SIZE     (16uL)
#define BLOCK_BITS_SIZE     (128uL)
#define BITS_SIZE_192       (192u)
#define BLOCKS2             (2uL)
#define BYTE_SIZE_24        (24u)
#define ROUND_UP            (1u)
#define INITIALIZATION      (0uL)

extern uint32_t g_ContextAddress;

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t s_01_CIPHER_InitSrcDest(icum_job_t *p_job, uint8_t job_cycle);
static uint32_t s_01_CIPHER_InitAesCipher(icum_job_t *p_job);
static uint32_t s_01_CIPHER_ProcessUpdate(icum_job_t *p_job, uint16_t finish);
static uint32_t s_01_CIPHER_ProcessFinish(icum_job_t *p_job);
static uint32_t s_01_CIPHER_ProcessUpdateXts(icum_job_t *p_job, uint16_t finish);
static uint32_t s_01_CIPHER_ReleaseKeyContext(icum_job_t *p_job);
static uint32_t s_01_CIPHER_PrepareAtOnce(icum_job_t *p_job, aes_key_t **p_key1, aes_key_t **p_key2,
    uint16_t cipher_convert);
static uint32_t s_01_CIPHER_AtOnceAes(icum_job_t *p_job);
static uint32_t s_01_CIPHER_Init(icum_job_t *p_job, uint8_t job_cycle, uint32_t *p_skip_flg);
static uint32_t s_01_CIPHER_Update(icum_job_t *p_job, uint8_t job_cycle);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0004]:[ICUMFW_UD_S_01_AES_0004]
* Function name: s_01_CIPHER_InitSrcDest
* Description  : Set number and address for Update process
* Arguments    : p_job
*              : job_cycle
* Return Value : ER_OK
*                ER_PARAM
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
*                SERV_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t s_01_CIPHER_InitSrcDest(icum_job_t *p_job, uint8_t job_cycle)
{
    r_icumif_isd_t *p_ISD;
    uint32_t err;
    uint32_t check_size;
    uint8_t mem_check;
    uint32_t skip_flg = FLG_OFF;

    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;

    /* When context is 0, it means that init processing has not been executed yet */
    if (p_job->prm.job_aes.get_context == 0u)
    {
        err = (uint32_t)SERV_SEQUENCE_ERROR;
        skip_flg = FLG_ON;
    }
    if (FLG_OFF == skip_flg)
    {
        /* make sure at least one block is to be processed */
        if(p_ISD->prm.AES_CIPHER.size.nb_blocks == 0u)
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
    if (FLG_OFF == skip_flg)
    {
        /* get the job slice configured */
        p_job->prm.job_aes.nb_slice = p_ISD->prm.AES_CIPHER.job_slice;
        if (CIPHER_MOD_XTS != p_ISD->prm.AES_CIPHER.cipher_mode)
        {
            /*Case: Not XTS Mode */
            p_job->prm.job_aes.p_source  = p_ISD->prm.AES_CIPHER.p_block_in;
            p_job->prm.job_aes.p_dest    = p_ISD->prm.AES_CIPHER.p_block_out;
            p_job->prm.job_aes.nb_blocks = p_ISD->prm.AES_CIPHER.size.nb_blocks;
            p_job->prm.job_aes.rem       = 0uL;
            check_size = p_ISD->prm.AES_CIPHER.size.nb_blocks * AES_BLOCK_SIZE_IN_BYTES;
        }
        else
        {
            /* Case: XTS Mode */
            if((JOB_UPDATE == job_cycle ) &&
                    (0u != (p_ISD->prm.AES_CIPHER.size.data_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS)))
            {
                /* Case Update Only, must be specified in block units. */
                err = ER_PARAM;
                skip_flg = FLG_ON;
            }
            else
            {
                /* do nothing */
            }
            if (FLG_OFF == skip_flg)
            {
                p_job->prm.job_aes.p_source  = p_ISD->prm.AES_CIPHER.p_block_in;
                p_job->prm.job_aes.p_dest    = p_ISD->prm.AES_CIPHER.p_block_out;

                /* 4-1-2-2. Calculate Block Number */
                p_job->prm.job_aes.nb_blocks = p_ISD->prm.AES_CIPHER.size.data_size_in_bits / (uint32_t)AES_BLOCK_SIZE_IN_BITS;
                p_job->prm.job_aes.rem       = p_ISD->prm.AES_CIPHER.size.data_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS;

                if (0uL == p_job->prm.job_aes.rem)
                {
                    p_job->prm.job_aes.rem   = 128uL;
                }
                else
                {
                    p_job->prm.job_aes.nb_blocks += 1uL;
                }
                if (((job_cycle & (uint8_t)JOB_FINISH) != 0u)
                        && (128uL != p_job->prm.job_aes.rem)
                        && (2uL > p_job->prm.job_aes.nb_blocks))
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
            if (FLG_OFF == skip_flg)
            {
                check_size = ((p_ISD->prm.AES_CIPHER.size.data_size_in_bits
                        + (AES_BYTE_SIZE_IN_BITS - 1u)) / AES_BYTE_SIZE_IN_BITS);
            }
            else
            {
                /* do nothing */
            }
        }
    }
    else
    {
        /* do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* make sure the source address range is valid */
        if(p_job->expansion_service == FROM_NATIVE_SERVICE)
        {
            err = MM_FLS_CheckMemory((uint32_t)p_job->prm.job_aes.p_source,
                check_size, FLS_CHK_MEM_SOURCE, p_job->expansion_service, &mem_check);
        }
        else /* FROM_EXPANSION_SERVICE */
        {
            err = MM_FLS_CheckMemoryCISM((uint32_t)p_job->prm.job_aes.p_source,
                    check_size, FLS_CHK_MEM_SOURCE, &mem_check);
        }
        if(err == ER_OK)
        {
            if(mem_check != FLS_CHK_OK)
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
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* make sure the destination address range is valid */
        if(p_job->expansion_service == FROM_NATIVE_SERVICE)
        {
            err = MM_FLS_CheckMemory((uint32_t)p_job->prm.job_aes.p_dest,
                check_size, FLS_CHK_MEM_DESTINATION, p_job->expansion_service, &mem_check);
        }
        else /* FROM_EXPANSION_SERVICE */
        {
            err = MM_FLS_CheckMemoryCISM((uint32_t)p_job->prm.job_aes.p_dest,
                    check_size, FLS_CHK_MEM_DESTINATION, &mem_check);
        }
        if(err == ER_OK)
        {
            if(mem_check != FLS_CHK_OK)
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
            /* do nothing */
        }
    }
    else
    {
        /* do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0003]:[ICUMFW_UD_S_01_AES_0003]
* Function name: s_01_CIPHER_InitAesCipher
* Description  : Initialization of the ciphering operation
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                ER_ML_CTX_ALL_USE
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                SERV_KEY_NOT_USABLE
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_SRC_MEM_ERROR
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_PENDING
***************************************************************************************/
static uint32_t s_01_CIPHER_InitAesCipher(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    aes_init_t     init_param;
    aes_init_xts_t init_param_xts;
    ml_ctx_aes_context_t *p_ctx;
    uint32_t iv_logical;

    aes_key_t *p_key1 = NULL;
    aes_key_t *p_key2 = NULL;
    s_01_com_key_param_t KeyParam1;
    s_01_com_key_param_t KeyParam2;
    uint32_t err = (uint32_t)SERV_OK;
    uint32_t resource_id;
    uint16_t cipher_convert;
    uint8_t mem_check;
    uint32_t skip_flg = FLG_OFF;

    L_Toolbox_MemSet((uint8_t *)&init_param,     DATA_ZERO, (int16_t)sizeof(aes_init_t));
    L_Toolbox_MemSet((uint8_t *)&init_param_xts, DATA_ZERO, (int16_t)sizeof(aes_init_xts_t));

    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;

    /* Check range chiper mode */
    switch(p_ISD->prm.AES_CIPHER.cipher_mode)
    {
        case CIPHER_MOD_CBC:
            cipher_convert = D_AES_CIPHER_MODE_CBC;
            break;
        case CIPHER_MOD_ECB:
            cipher_convert = D_AES_CIPHER_MODE_ECB;
            break;
        case CIPHER_MOD_CFB:
            cipher_convert = D_AES_CIPHER_MODE_CFB;
            break;
        case CIPHER_MOD_OFB:
            cipher_convert = D_AES_CIPHER_MODE_OFB;
            break;
        case CIPHER_MOD_CTR:
            cipher_convert = D_AES_CIPHER_MODE_CTR;
            break;
        case CIPHER_MOD_XTS:
            cipher_convert = D_AES_CIPHER_MODE_XTS;
            break;
        default:
            err = ER_PARAM;
            skip_flg = FLG_ON;
            break;
    }
    if(FLG_OFF == skip_flg)
    {
        /* Check range direction */
        if((p_ISD->prm.AES_CIPHER.direction != CIPHER_DIR_ENCRYPTION)
                && (p_ISD->prm.AES_CIPHER.direction != CIPHER_DIR_DECRYPTION))
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            /* make sure the iv address range is valid */
            if(cipher_convert != D_AES_CIPHER_MODE_ECB)
            {
                /* make sure the source address range is valid */
                if(p_job->expansion_service == FROM_NATIVE_SERVICE)
                {
                    err = MM_FLS_CheckMemory((uint32_t)p_ISD->prm.AES_CIPHER.p_iv, AES_BLOCK_SIZE_IN_BYTES,
                            FLS_CHK_MEM_SOURCE, p_job->expansion_service, &mem_check);
                }
                else /* FROM_EXPANSION_SERVICE */
                {
                    err = MM_FLS_CheckMemoryCISM((uint32_t)p_ISD->prm.AES_CIPHER.p_iv, AES_BLOCK_SIZE_IN_BYTES,
                            FLS_CHK_MEM_SOURCE, &mem_check);
                }
                if(err == ER_OK)
                {
                    if(mem_check != FLS_CHK_OK)
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
                    skip_flg = FLG_ON;
                }
            }
            else
            {
                /* do nothing */
            }
        }
    }
    else
    {
        /* do nothing */
    }
            
    if(FLG_OFF == skip_flg)
    {
        /* Get AES Key */
        KeyParam1.key_group      = p_ISD->prm.AES_CIPHER.key_group;
        KeyParam1.key_id         = p_ISD->prm.AES_CIPHER.key_id;
        KeyParam1.NotUseFlag     = 0;
        KeyParam1.direction      = (uint8_t)p_ISD->prm.AES_CIPHER.direction;
        KeyParam1.p_aeskey       = &p_key1;

        err = s_01_CIPHER_GetAESKey(p_job, &KeyParam1);
        if(err == ER_OK)
        {
            /* Get Key success. */
            p_job->prm.job_aes.get_key = RESOURCES_HOLD;
            if ((CIPHER_MOD_XTS == p_ISD->prm.AES_CIPHER.cipher_mode)
                && ((uint16_t)BYTE_SIZE_24 == p_key1->properties.size_in_bytes))
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
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }
    if(FLG_OFF == skip_flg)
    {
        if (CIPHER_MOD_XTS == p_ISD->prm.AES_CIPHER.cipher_mode)
        {
            /* Case: XTS Mode */
            /* Get AES Key(XTS) */
            KeyParam2.key_group      = p_ISD->prm.AES_CIPHER.key_group_2;
            KeyParam2.key_id         = p_ISD->prm.AES_CIPHER.key_id_2;
            KeyParam2.NotUseFlag     = 1;
            KeyParam2.direction      = (uint8_t)p_ISD->prm.AES_CIPHER.direction;
            KeyParam2.p_aeskey       = &p_key2;

            /* Get AES Key2 */
            err = s_01_CIPHER_GetAESKey(p_job, &KeyParam2);
            if (ER_OK == err)
            {
                if (((uint16_t)BYTE_SIZE_24 == p_key2->properties.size_in_bytes)
                    || (p_key1->properties.size_in_bytes != p_key2->properties.size_in_bytes))
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
                skip_flg = FLG_ON;
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

    if (FLG_OFF == skip_flg)
    {
        err = ML_CTX_Get(CTX_TYPE_AES, (uint32_t**)&p_job->prm.job_aes.get_context);
        p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes.get_context;
        if (ER_OK == err)
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
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
        if (ER_OK == err)
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
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* Get Remap(Convert Physical address -> Logical Address) */
        if (NULL != p_ISD->prm.AES_CIPHER.p_iv)
        {
            /* 5-1. Get Remap */
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_CIPHER.p_iv, &iv_logical);
        }
        else
        {
            /* 5-2. NULL is allowed(ECB mode) */
            iv_logical = 0uL;
        }
    }
    else
    {
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        if (CIPHER_MOD_XTS != p_ISD->prm.AES_CIPHER.cipher_mode)
        {
            /* Case: Not XTS Mode */
            g_ContextAddress       = AES_STATE_INIT;
            init_param.cipher_mode = cipher_convert;
            init_param.direction   = (uint8_t)p_ISD->prm.AES_CIPHER.direction;
            init_param.p_aes_key   = (uint32_t *)p_key1->value;
            init_param.key_length  = p_key1->properties.size_in_bytes;
            init_param.p_aes_iv    = (uint32_t *)iv_logical;
            init_param.p_context   = p_ctx->context;
            err = D_AES_InitJob(&init_param);
            p_ctx->block_info = D_AES_FIRST_CIPHER;
        }
        else
        {
            /* Case: XTS Mode */
            g_ContextAddress         = AES_STATE_INIT;
            init_param_xts.direction = (uint8_t)p_ISD->prm.AES_CIPHER.direction;
            init_param_xts.p_aes_key = (uint32_t *)p_key1->value;
            init_param_xts.p_aes_key_tweak = (uint32_t *)p_key2->value;
            init_param_xts.key_length = p_key1->properties.size_in_bytes;
            init_param_xts.p_aes_iv   = (uint32_t *)iv_logical;
            init_param_xts.p_context  = p_ctx->context;
            err = D_AES_InitJobXts(&init_param_xts);
            p_ctx->block_info = D_AES_FIRST_CIPHER;
        }

        if (ER_OK == err)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        }
        else
        {
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        }
    }
    else
    {
        /* do nothing */
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0005]:[ICUMFW_UD_S_01_AES_0005]
* Function name: s_01_CIPHER_ProcessUpdate
* Description  : Processing update Cipher
* Arguments    : p_job
*                finish
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_ML_CTX_ALL_USE
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                R_SALIF_ER_CRY_TIMEOUT
***************************************************************************************/
static uint32_t s_01_CIPHER_ProcessUpdate(icum_job_t *p_job, uint16_t finish)
{
    aes_update_t update_param;
    ml_ctx_aes_context_t *p_ctx;

    uint32_t err;
    uint32_t nb_slice;
    uint32_t p_resource_id;
    uint32_t skip_flg = FLG_OFF;
    uint32_t uiSource_logical;
    uint32_t uiDest_logical;
    uint32_t src_byte_size;

    L_Toolbox_MemSet((uint8_t *)&update_param, DATA_ZERO, (int16_t)sizeof(aes_update_t));

    err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &p_resource_id);
    if(err != ER_OK)
    {
        skip_flg = FLG_ON;
    }
    else
    {
        /* do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes.p_source, &uiSource_logical);
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes.p_dest, &uiDest_logical);
    }
    else
    {
        /* do nothing */
    }
    if(FLG_OFF == skip_flg)
    {
        p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes.get_context;

        nb_slice = p_job->prm.job_aes.nb_slice;
        if(finish == JOB_NOT_COMPLETE)
        {
             src_byte_size = ((uint32_t)p_job->prm.job_aes.nb_slice * BLOCK_BYTE_SIZE);
        }
        else
        {
             src_byte_size = (p_job->prm.job_aes.nb_blocks * BLOCK_BYTE_SIZE);
        }
        g_ContextAddress        = AES_STATE_INIT;
        update_param.number     = (uint32_t)(src_byte_size / (uint32_t)AES_BLOCK_SIZE_IN_BYTES);
        update_param.first      = p_ctx->block_info;
        update_param.p_aes_src  = (uint32_t*)uiSource_logical;
        update_param.p_context  = p_ctx->context;
        update_param.p_aes_dest = (uint32_t*)uiDest_logical;
        err = D_AES_UpdateJob(&update_param);
        p_ctx->block_info = D_AES_NOT_FIRST_CIPHER;

        /* any error? */
        if (ER_OK == err)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, p_resource_id);
            if(err == ER_OK)
            {
                if(finish == JOB_COMPLETE)
                {
                    err = (uint32_t)SERV_OK;
                }
                else
                {
                    p_job->prm.job_aes.nb_blocks -= nb_slice;
                    p_job->prm.job_aes.p_source  += (4u * nb_slice);
                    p_job->prm.job_aes.p_dest    += (4u * nb_slice);
                    /* not finished */
                    err = (uint32_t)SERV_IN_PROCESS;
                }
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            /* Prioritize L_CRY_UpdateAes error,do not get ML_ABTR_FreeResource error */
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, p_resource_id);
        }
    }
    else
    {
        /* do nothing */
    }

    return (err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0033]:[ICUMFW_UD_S_01_AES_0033]
* Function name: s_01_CIPHER_ProcessFinish
* Description  : Processing Finish Cipher
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                R_SALIF_ER_CRY_TIMEOUT
***************************************************************************************/
static uint32_t s_01_CIPHER_ProcessFinish(icum_job_t *p_job)
{
    aes_finish_t finish_param;
    ml_ctx_aes_context_t *p_ctx;

    uint32_t err;
    uint32_t p_resource_id;
    uint32_t skip_flg = FLG_OFF;

    p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes.get_context;
    L_Toolbox_MemSet((uint8_t *)&finish_param, DATA_ZERO, (int16_t)sizeof(aes_finish_t));

    err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &p_resource_id);
    if(err != ER_OK)
    {
        skip_flg = FLG_ON;
    }
    else
    {
        /* do nothing */
    }
    if(FLG_OFF == skip_flg)
    {
        g_ContextAddress        = AES_STATE_INIT;
        finish_param.number     = (uint32_t)(DATA_ZERO / (uint32_t)AES_BLOCK_SIZE_IN_BYTES);
        finish_param.first      = p_ctx->block_info;
        finish_param.p_aes_src  = (uint32_t*)NULL;
        finish_param.p_context  = p_ctx->context;
        finish_param.p_aes_dest = (uint32_t*)NULL;
        finish_param.last_block_in_bits = (uint16_t)NUM0;
        err = D_AES_FinishJob(&finish_param);

        if (ER_OK == err)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, p_resource_id);
        }
        else
        {
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, p_resource_id);
        }
    }
    else
    {
        /* do nothing */
    }

    return (err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0006]:[ICUMFW_UD_S_01_AES_0006]
* Function name: s_01_CIPHER_ProcessUpdateXts
* Description  : Processing update Cipher
* Arguments    : p_job
*                finish
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_ML_CTX_ALL_USE
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                R_SALIF_ER_CRY_TIMEOUT
***************************************************************************************/
static uint32_t s_01_CIPHER_ProcessUpdateXts(icum_job_t *p_job, uint16_t finish)
{
    aes_update_t update_param;
    ml_ctx_aes_context_t *p_ctx;

    uint32_t err;
    uint32_t nb_slice;
    uint32_t p_resource_id;
    uint32_t skip_flg = FLG_OFF;
    uint32_t uiSource_logical;
    uint32_t uiDest_logical;
    uint32_t msg_size_in_bits;

    p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes.get_context;
    L_Toolbox_MemSet((uint8_t *)&update_param, DATA_ZERO, (int16_t)sizeof(aes_update_t));

    err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &p_resource_id);
    if(err != ER_OK)
    {
        skip_flg = FLG_ON;
    }
    else
    {
        /* do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes.p_source, &uiSource_logical);
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes.p_dest, &uiDest_logical);
    }
    else
    {
        /* do nothing */
    }
    if(FLG_OFF == skip_flg)
    {
        nb_slice = p_job->prm.job_aes.nb_slice;
       if(finish == JOB_NOT_COMPLETE)
        {
             msg_size_in_bits = ((uint32_t)p_job->prm.job_aes.nb_slice * BLOCK_BITS_SIZE);
        }
        else
        {
             msg_size_in_bits = ((uint32_t)p_job->prm.job_aes.nb_blocks * BLOCK_BITS_SIZE);
        }
        g_ContextAddress        = AES_STATE_INIT;
        update_param.number     = (msg_size_in_bits / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
        update_param.first      = p_ctx->block_info;
        update_param.p_aes_src  = (uint32_t*)uiSource_logical;
        update_param.p_context  = p_ctx->context;
        update_param.p_aes_dest = (uint32_t*)uiDest_logical;
        err = D_AES_UpdateJobXts(&update_param);
        p_ctx->block_info = D_AES_NOT_FIRST_CIPHER;

        /* any error? */
        if (ER_OK == err)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, p_resource_id);
            if(err == ER_OK)
            {
                if(finish == JOB_COMPLETE)
                {
                    err = (uint32_t)SERV_OK;
                }
                else
                {
                    p_job->prm.job_aes.nb_blocks -= nb_slice;
                    p_job->prm.job_aes.p_source  += (4u * nb_slice);
                    p_job->prm.job_aes.p_dest    += (4u * nb_slice);
                    /* not finished */
                    err = (uint32_t)SERV_IN_PROCESS;
                }
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            /* Prioritize L_CRY_UpdateAes error,do not get ML_ABTR_FreeResource error */
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, p_resource_id);
        }
    }
    else
    {
        /* do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0007]:[ICUMFW_UD_S_01_AES_0007]
* Function name: s_01_CIPHER_ProcessFinishXts
* Description  : Processing finish AES Cipher(XTS)
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_ML_CTX_ALL_USE
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                R_SALIF_ER_CRY_TIMEOUT
***************************************************************************************/
static uint32_t s_01_CIPHER_ProcessFinishXts(icum_job_t *p_job)
{
    aes_update_t update_param;
    aes_finish_t finish_param;
    ml_ctx_aes_context_t *p_ctx;

    uint32_t cipher_data[AES_BLOCK_SIZE * 2];
    uint32_t resource_id = 0;
    uint32_t err;
    uint32_t skip_flg = FLG_OFF;
    uint32_t number;
    uint32_t uiSource_logical;
    uint32_t uiDest_logical;
    uint32_t msg_size_in_bits;
    uint16_t last_block;

    p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes.get_context;
    L_Toolbox_MemSet((uint8_t *)&update_param, DATA_ZERO, (int16_t)sizeof(aes_update_t));
    L_Toolbox_MemSet((uint8_t *)&finish_param, DATA_ZERO, (int16_t)sizeof(aes_finish_t));

    /* Get ABTR Resource */
    err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
    if(err != ER_OK)
    {
        skip_flg = FLG_ON;
    }
    else
    {
        /* do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes.p_source, &uiSource_logical);
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes.p_dest, &uiDest_logical);
    }
    else
    {
        /* do nothing */
    }
    if(FLG_OFF == skip_flg)
    {
        if (p_job->prm.job_aes.rem != 128u)
        {
            /* If there is a fraction, adjust the output size of the final block */
            if (p_job->prm.job_aes.nb_blocks > 2u)
            {
                /* Calculate with the Update function until the remaining 2 blocks */
                msg_size_in_bits = ((p_job->prm.job_aes.nb_blocks - BLOCKS2) * BLOCK_BITS_SIZE);
                number = (uint32_t)(p_job->prm.job_aes.nb_blocks - BLOCKS2);

                g_ContextAddress        = AES_STATE_INIT;
                update_param.number     = (msg_size_in_bits / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
                update_param.first      = p_ctx->block_info;
                update_param.p_aes_src  = (uint32_t*)uiSource_logical;
                update_param.p_context  = p_ctx->context;
                update_param.p_aes_dest = (uint32_t*)uiDest_logical;
                err = D_AES_UpdateJobXts(&update_param);
                p_ctx->block_info = D_AES_NOT_FIRST_CIPHER;

                /* any error? */
                if (ER_OK == err)
                {
                    p_job->prm.job_aes.nb_blocks -= number;
                    p_job->prm.job_aes.p_source  += (4u * number);
                    p_job->prm.job_aes.p_dest    += (4u * number);
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
            if (FLG_OFF == skip_flg)
            {
                (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes.p_source, &uiSource_logical);
                (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes.p_dest, &uiDest_logical);
                /* Set finish parameter */
                msg_size_in_bits   = (p_job->prm.job_aes.nb_blocks * BLOCK_BITS_SIZE)
                                            - (BLOCK_BITS_SIZE - p_job->prm.job_aes.rem);

                finish_param.number = ((msg_size_in_bits + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1))
                                            / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
                last_block = (uint16_t)(msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS);
                if (NUM0 == (uint32_t)last_block)
                {
                    finish_param.last_block_in_bits = (uint16_t)AES_BLOCK_SIZE_IN_BITS;
                }
                else
                {
                    finish_param.last_block_in_bits = last_block;
                }
                g_ContextAddress        = AES_STATE_INIT;
                finish_param.first      = p_ctx->block_info;
                finish_param.p_aes_src  = (uint32_t*)uiSource_logical;
                finish_param.p_context  = p_ctx->context;
                finish_param.p_aes_dest = (uint32_t*)cipher_data;
                err = D_AES_FinishJobXts(&finish_param);

                /* Padding invalid data of the last block with 0 */
                (void)s_01_CIPHER_Sizing((uint8_t*)&cipher_data[4], p_job->prm.job_aes.rem);
                /* Output only valid sizes */
                L_Toolbox_MemCpy((uint8_t *)uiDest_logical,
                        (const uint8_t *)cipher_data,
                        (AES_BLOCK_SIZE_IN_BYTES + ((p_job->prm.job_aes.rem
                                + (AES_BYTE_SIZE_IN_BITS - 1u)) / AES_BYTE_SIZE_IN_BITS)));
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            /* Set finish parameter */
            msg_size_in_bits    = ((uint32_t)p_job->prm.job_aes.nb_blocks * BLOCK_BITS_SIZE);
            finish_param.number = ((msg_size_in_bits + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1))
                                    / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
            last_block = (uint16_t)(msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS);
            if (NUM0 == (uint32_t)last_block)
            {
                finish_param.last_block_in_bits = (uint16_t)AES_BLOCK_SIZE_IN_BITS;
            }
            else
            {
                finish_param.last_block_in_bits = last_block;
            }
            g_ContextAddress        = AES_STATE_INIT;
            finish_param.first      = p_ctx->block_info;
            finish_param.p_aes_src  = (uint32_t*)uiSource_logical;
            finish_param.p_context  = p_ctx->context;
            finish_param.p_aes_dest = (uint32_t*)uiDest_logical;
            err = D_AES_FinishJobXts(&finish_param);
        }
    }
    else
    {
        /* do nothing */
    }

    /* any error? */
    if (resource_id != 0)
    {
        if(err == ER_OK)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        }
        else
        {
            /* Prioritize L_CRY_UpdateAes error,do not get ML_ABTR_FreeResource error */
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        }
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0008]:[ICUMFW_UD_S_01_AES_0008]
* Function name: s_01_CIPHER_ReleaseKeyContext
* Description  : Process of releasing the key and context in use
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_DATA_NOT_FOUND
*                ER_ML_CTX_NOT_FOUND
***************************************************************************************/
static uint32_t s_01_CIPHER_ReleaseKeyContext(icum_job_t *p_job)
{
    r_key_group_t  key_group;
    r_key_index_t  key_id;
    uint32_t err = (uint32_t)SERV_OK;
    uint32_t skip_flg = FLG_OFF;
    ml_ctx_aes_context_t *p_ctx;
    aes_release_t release_param;

    L_Toolbox_MemSet((uint8_t *)&release_param, DATA_ZERO, (int16_t)sizeof(aes_release_t));

    if(p_job->prm.job_aes.get_context != 0u)
    {
        p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes.get_context;

        release_param.key_length_id = *((uint32_t*)p_ctx->context);
        release_param.key_length    = *((uint16_t*)(p_ctx->context + ADDR_1ST));
        err = D_AES_ReleaseKeyRange(&release_param);
        if(err != ER_OK)
        {
            skip_flg = FLG_ON;
        }
        else
        {
            err = ML_CTX_Free(CTX_TYPE_AES, (uint32_t*)p_job->prm.job_aes.get_context);
            p_job->prm.job_aes.get_context = 0u;
            if(err != ER_OK)
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* do nothing */
            }
        }
    }
    else
    {
        /* do nothing */
    }

    if(FLG_OFF == skip_flg)
    {
        if(p_job->prm.job_aes.get_key == RESOURCES_HOLD)
        {
            key_group = p_job->p_ISD->prm.AES_CIPHER.key_group;
            key_id    = p_job->p_ISD->prm.AES_CIPHER.key_id;

            if(key_group == KEY_GRP_SHE)
            {
                /* convert from SHE index to internal group / id */
                (void)MH_KEY_ConvertSheKey(key_id, &key_group, &key_id);
            }
            else
            {
                /* do nothing */
            }
            err = MH_KEY_FreeKeyCache(p_job, key_group, key_id);
            p_job->prm.job_aes.get_key = RESOURCES_NOT_HOLD;
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
    err = s_01_CIPHER_ConvertErr(err);
    return(err);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0026]:[ICUMFW_UD_S_01_AES_0026]
* Function ID  : Function ID
* Function name: s_01_CIPHER_PrepareAtOnce
* Description  : Initialization of the ciphering operation
* Arguments    : p_job
*                p_key1
*                p_key2
*                cipher_convert
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_ML_CTX_ALL_USE
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                SERV_KEY_NOT_USABLE
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_SRC_MEM_ERROR
***************************************************************************************/
static uint32_t s_01_CIPHER_PrepareAtOnce(icum_job_t *p_job, aes_key_t **p_key1, aes_key_t **p_key2,
    uint16_t cipher_convert)
{
    r_icumif_isd_t *p_ISD;
    s_01_com_key_param_t KeyParam1;
    s_01_com_key_param_t KeyParam2;
    uint32_t check_size;
    uint8_t mem_check;
    uint32_t err;
    uint32_t skip_flg;
    
    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;
    
    check_size = INITIALIZATION;
    err = (uint32_t)SERV_OK;
    skip_flg = FLG_OFF;
    
    /* make sure the iv address range is valid */
    if ((uint16_t)D_AES_CIPHER_MODE_ECB != cipher_convert)
    {
        /* make sure the source address range is valid */
        if ((uint8_t)FROM_NATIVE_SERVICE == p_job->expansion_service)
        {
            err = MM_FLS_CheckMemory((uint32_t)p_ISD->prm.AES_CIPHER.p_iv, (uint32_t)AES_BLOCK_SIZE_IN_BYTES,
                    FLS_CHK_MEM_SOURCE, p_job->expansion_service, &mem_check);
        }
        else /* FROM_EXPANSION_SERVICE */
        {
            err = MM_FLS_CheckMemoryCISM((uint32_t)p_ISD->prm.AES_CIPHER.p_iv, (uint32_t)AES_BLOCK_SIZE_IN_BYTES,
                    FLS_CHK_MEM_SOURCE, &mem_check);
        }
        if (ER_OK == err)
        {
            if ((uint8_t)FLS_CHK_OK != mem_check)
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
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* Get AES Key */
        KeyParam1.key_group      = p_ISD->prm.AES_CIPHER.key_group;
        KeyParam1.key_id         = p_ISD->prm.AES_CIPHER.key_id;
        KeyParam1.NotUseFlag     = (uint8_t)DATA_ZERO;
        KeyParam1.direction      = (uint8_t)p_ISD->prm.AES_CIPHER.direction;
        KeyParam1.p_aeskey       = p_key1;

        if (CIPHER_MOD_XTS != p_ISD->prm.AES_CIPHER.cipher_mode)
        {
            err = s_01_CIPHER_GetAESKey(p_job, &KeyParam1);
            if (ER_OK == err)
            {
                /* Get Key success. */
                p_job->prm.job_aes.get_key = RESOURCES_HOLD;
            }
            else
            {
                skip_flg = FLG_ON;
            }
        }
        else
        {
            err = s_01_CIPHER_GetAESKey(p_job, &KeyParam1);
            if (ER_OK == err)
            {
                /* Get Key success. */
                p_job->prm.job_aes.get_key = RESOURCES_HOLD;

                if ((uint16_t)BYTE_SIZE_24 == (*p_key1)->properties.size_in_bytes)
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
                skip_flg = FLG_ON;
            }
        }
    }
    else
    {
        /* do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        if (CIPHER_MOD_XTS == p_ISD->prm.AES_CIPHER.cipher_mode)
        {
            /* Case: XTS Mode */
            /* Get AES Key(XTS) */
            KeyParam2.key_group      = p_ISD->prm.AES_CIPHER.key_group_2;
            KeyParam2.key_id         = p_ISD->prm.AES_CIPHER.key_id_2;
            KeyParam2.NotUseFlag     = (uint8_t)DATA_ONE;
            KeyParam2.direction      = (uint8_t)p_ISD->prm.AES_CIPHER.direction;
            KeyParam2.p_aeskey       = p_key2;

            /* Get AES Key2 */
            err = s_01_CIPHER_GetAESKey(p_job, &KeyParam2);
            if (ER_OK == err)
            {
                if ((uint16_t)BYTE_SIZE_24 == (*p_key2)->properties.size_in_bytes)
                {
                    err = ER_PARAM;
                    skip_flg = FLG_ON;
                }
                else
                {
                    if ((*p_key1)->properties.size_in_bytes == (*p_key2)->properties.size_in_bytes)
                    {
                         /* do nothing */
                    }
                    else
                    {
                        err = ER_PARAM;
                        skip_flg = FLG_ON;
                    }
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
    }
    else
    {
        /* do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* make sure at least one block is to be processed */
        if ((uint32_t)DATA_ZERO == p_ISD->prm.AES_CIPHER.size.nb_blocks)
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

    if (FLG_OFF == skip_flg)
    {
        if (CIPHER_MOD_XTS != p_ISD->prm.AES_CIPHER.cipher_mode)
        {
            check_size = (uint32_t)p_ISD->prm.AES_CIPHER.size.nb_blocks * (uint32_t)AES_BLOCK_SIZE_IN_BYTES;
        }
        else
        {
            check_size = ((p_ISD->prm.AES_CIPHER.size.data_size_in_bits
            + (uint32_t)(AES_BYTE_SIZE_IN_BITS - ROUND_UP)) / (uint32_t)AES_BYTE_SIZE_IN_BITS);
        }
        /* make sure the source address range is valid */
        if ((uint8_t)FROM_NATIVE_SERVICE == p_job->expansion_service)
        {
            err = MM_FLS_CheckMemory((uint32_t)p_ISD->prm.AES_CIPHER.p_block_in,
                                    check_size, FLS_CHK_MEM_SOURCE, p_job->expansion_service, &mem_check);
        }
        else /* FROM_EXPANSION_SERVICE */
        {
            err = MM_FLS_CheckMemoryCISM((uint32_t)p_ISD->prm.AES_CIPHER.p_block_in,
                                        check_size, FLS_CHK_MEM_SOURCE, &mem_check);
        }
        if (ER_OK == err)
        {
            if ((uint8_t)FLS_CHK_OK == mem_check)
            {
                 /* do nothing */
            }
            else
            {
                err = (uint32_t)SERV_SRC_MEM_ERROR;
                skip_flg = FLG_ON;
            }
        }
        else
        {
            skip_flg = FLG_ON;
        }
        if (FLG_OFF == skip_flg)
        {
            /* make sure the source address range is valid */
            if ((uint8_t)FROM_NATIVE_SERVICE == p_job->expansion_service)
            {
                err = MM_FLS_CheckMemory((uint32_t)p_ISD->prm.AES_CIPHER.p_block_out,
                                        check_size, FLS_CHK_MEM_DESTINATION, p_job->expansion_service, &mem_check);
            }
            else /* FROM_EXPANSION_SERVICE */
            {
                err = MM_FLS_CheckMemoryCISM((uint32_t)p_ISD->prm.AES_CIPHER.p_block_out,
                                            check_size, FLS_CHK_MEM_DESTINATION, &mem_check);
            }
            if (ER_OK == err)
            {
                if (FLS_CHK_OK == mem_check)
                {
                    /* do nothing */
                }
                else
                {
                    err = (uint32_t)SERV_DEST_MEM_ERROR;
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
    }
    else
    {
        /* do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0002]:[ICUMFW_UD_S_01_AES_0002]
* Function ID  : Function ID
* Function name: s_01_CIPHER_AtOnceAes
* Description  : Initialization of the ciphering operation
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_ML_CTX_ALL_USE
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                SERV_KEY_NOT_USABLE
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_SRC_MEM_ERROR
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_PENDING
*                R_SALIF_ER_IN_PROCESS
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                R_SALIF_ER_CRY_TIMEOUT
***************************************************************************************/
static uint32_t s_01_CIPHER_AtOnceAes(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    aes_all_at_once_t     at_once_param;
    aes_all_at_once_xts_t at_once_param_xts;
    ml_ctx_aes_context_t  *p_ctx;
    uint32_t uiSource_logical;
    uint32_t uiDest_logical;
    uint32_t iv_logical;
    uint32_t saved_psw;

    aes_key_t *p_key1 = NULL;
    aes_key_t *p_key2 = NULL;
    uint32_t err = (uint32_t)SERV_OK;
    uint32_t get_resource_id;
    static uint32_t set_resource_id;
    uint16_t cipher_convert;
    uint32_t skip_flg = FLG_OFF;
    uint32_t src_byte_size;
    uint32_t msg_size_in_bits;
    uint16_t last_block;

    L_Toolbox_MemSet((uint8_t *)&at_once_param,     DATA_ZERO, (int16_t)sizeof(aes_all_at_once_t));
    L_Toolbox_MemSet((uint8_t *)&at_once_param_xts, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_xts_t));

    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;

    /* Check DTF processing */
    if (SERV_IN_PROCESS == p_ISD->service_result)
    {
        if (NULL != p_job->prm.job_aes.get_context)
        {
            err = ((ml_ctx_aes_context_t*)p_job->prm.job_aes.get_context)->result;
            if (ER_OK == err)
            {
                err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
            }
            else if (ER_IN_PROCESS == err)
            {
                /* Do nothing */
            }
            else
            {
                (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
            }
        }
        else
        {
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
            err = SERV_SEQUENCE_ERROR;
        }

        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* Check range chiper mode */
        switch (p_ISD->prm.AES_CIPHER.cipher_mode)
        {
            case CIPHER_MOD_CBC:
                cipher_convert = D_AES_CIPHER_MODE_CBC;
                break;
            case CIPHER_MOD_ECB:
                cipher_convert = D_AES_CIPHER_MODE_ECB;
                break;
            case CIPHER_MOD_CFB:
                cipher_convert = D_AES_CIPHER_MODE_CFB;
                break;
            case CIPHER_MOD_OFB:
                cipher_convert = D_AES_CIPHER_MODE_OFB;
                break;
            case CIPHER_MOD_CTR:
                cipher_convert = D_AES_CIPHER_MODE_CTR;
                break;
           case CIPHER_MOD_XTS:
                cipher_convert = D_AES_CIPHER_MODE_XTS;
                break;
            default:
                err = ER_PARAM;
                break;
        }/* check cipher_mode*/
        if (ER_OK == err)
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
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* Check range direction */
        if ((CIPHER_DIR_ENCRYPTION == p_ISD->prm.AES_CIPHER.direction)
                || (CIPHER_DIR_DECRYPTION == p_ISD->prm.AES_CIPHER.direction))
        {
            /* do nothing */
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

    if (FLG_OFF == skip_flg)
    {
        err = s_01_CIPHER_PrepareAtOnce(p_job, &p_key1, &p_key2, cipher_convert);
        if (ER_OK == err)
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
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        err = ML_CTX_Get(CTX_TYPE_AES, (uint32_t**)&p_job->prm.job_aes.get_context);
        p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes.get_context;
        if (ER_OK == err)
        {
            ((ml_ctx_aes_context_t*)p_job->prm.job_aes.get_context)->result = ER_D_AES_INIT_ERROR;
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

    if (FLG_OFF == skip_flg)
    {
        err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &get_resource_id);
        if (ER_OK == err)
        {
            /* Get Remap(Convert Physical address -> Logical Address) */
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_CIPHER.p_block_in, &uiSource_logical);
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_CIPHER.p_block_out, &uiDest_logical);
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_CIPHER.p_iv, &iv_logical);
            set_resource_id = get_resource_id;
            if (CIPHER_MOD_XTS != p_ISD->prm.AES_CIPHER.cipher_mode)
            {
                /* Case: Not XTS Mode */
                src_byte_size = (p_ISD->prm.AES_CIPHER.size.nb_blocks * BLOCK_BYTE_SIZE);

                at_once_param.cipher_mode = cipher_convert;
                at_once_param.direction   = (uint32_t)p_ISD->prm.AES_CIPHER.direction;
                at_once_param.number      = (uint32_t)(src_byte_size / (uint32_t)AES_BLOCK_SIZE_IN_BYTES);
                at_once_param.key_type    = D_AES_KEYTYPE_PLAIN;
                at_once_param.p_aes_key   = (uint32_t *)p_key1->value;
                at_once_param.key_length  = p_key1->properties.size_in_bytes;
                at_once_param.p_aes_iv    = (uint32_t *)iv_logical;
                at_once_param.p_aes_src   = (uint32_t *)uiSource_logical;
                at_once_param.p_aes_dest  = (uint32_t *)uiDest_logical;
                at_once_param.last_block_in_bits = (uint16_t)NUM0;
                if (NULL == p_ctx)
                {
                    at_once_param.async = D_AES_TRANS_SYNC;
                    g_ContextAddress    = AES_STATE_INIT;
                }
                else
                {
                    /* If Context is specified and All processing is specified, specify asynchronous */
                    at_once_param.async = D_AES_TRANS_ASYNC;
                    g_ContextAddress    = (uint32_t)p_ctx;
                }
                err = D_AES_ExecutionAtOnce(&at_once_param);
            }
            else
            {
                /* Case: XTS Mode */
                msg_size_in_bits = p_ISD->prm.AES_CIPHER.size.data_size_in_bits;

                at_once_param_xts.direction = (uint8_t)p_ISD->prm.AES_CIPHER.direction;
                at_once_param_xts.number    = ((msg_size_in_bits + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1))
                                                / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
                last_block = (uint16_t)(msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS);
                if (NUM0 == (uint32_t)last_block)
                {
                    at_once_param_xts.last_block_in_bits = (uint16_t)AES_BLOCK_SIZE_IN_BITS;
                }
                else
                {
                    at_once_param_xts.last_block_in_bits = last_block;
                }
                at_once_param_xts.p_aes_key = (uint32_t *)p_key1->value;
                at_once_param_xts.p_aes_key_tweak = (uint32_t *)p_key2->value;
                at_once_param_xts.key_length = p_key1->properties.size_in_bytes;
                at_once_param_xts.p_aes_iv   = (uint32_t *)iv_logical;
                at_once_param_xts.p_aes_src  = (uint32_t *)uiSource_logical;
                at_once_param_xts.p_aes_dest = (uint32_t *)uiDest_logical;
                at_once_param_xts.p_context  = p_ctx->context;
                if (NULL == p_ctx)
                {
                    at_once_param_xts.async = D_AES_TRANS_SYNC;
                    g_ContextAddress        = AES_STATE_INIT;
                }
                else
                {
                    /* If Context is specified and All processing is specified, specify asynchronous */
                    at_once_param_xts.async = D_AES_TRANS_ASYNC;
                    g_ContextAddress        = (uint32_t)p_ctx;
                }
                err = D_AES_ExecutionAtOnceXts(&at_once_param_xts);
            }

            if (NULL != p_ctx)
            {
                /* In case of asynchronous processing, store the operation result in Context */
                /* disable interrupts (if not already disabled) */
                DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

                if (ER_D_AES_INIT_ERROR == p_ctx->result)
                {
                    p_ctx->result = err;
                }
                /* re-enable interrupts (if necessary) */
                ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

                err = ER_OK;
            }
            else
            {
                err = g_ContextAddress;
            }

            if (ER_OK == err)
            {
                /* Prioritize L_CRY_UpdateAes error,do not get ML_ABTR_FreeResource error */
                err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
            }
            else if (ER_IN_PROCESS == err)
            {
               /* do nothing */
            }
            else
            {
                (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
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
    return (err);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0024]:[ICUMFW_UD_S_01_AES_0024]
* Function name: s_01_CIPHER_Init
* Description  : Initialize part of an AES encryption / decryption operation
*                Call processing by the job_cycle argument
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                ER_ML_CTX_ALL_USE
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                SERV_KEY_NOT_USABLE
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
*                SERV_SEQUENCE_ERROR
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_PENDING
***************************************************************************************/
static uint32_t s_01_CIPHER_Init(icum_job_t *p_job, uint8_t job_cycle, uint32_t *p_skip_flg)
{
    r_icumif_isd_t *p_ISD;
    uint32_t err;
    
    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;
    err = (uint32_t)SERV_OK;
    
    /* service just triggered? */
    if((p_ISD->service_result == SERV_REQUEST) || (p_ISD->service_result == SERV_PENDING))
    {
        if((job_cycle & (uint8_t)JOB_INIT) != (uint8_t)DATA_ZERO)
        {
            /* initialize the job */
            err = s_01_CIPHER_InitAesCipher(p_job);
            if(err != ER_OK)
            {
                *p_skip_flg = FLG_ON;
            }
            else
            {
                /* When job_cycle is init only, the processing is end */
                if((job_cycle & ((uint8_t)JOB_UPDATE | (uint8_t)JOB_FINISH)) == (uint8_t)DATA_ZERO)
                {
                    *p_skip_flg = FLG_ON;
                }
                else
                {
                    /* do nothing */
                }
            }
        }
        else
        {
            /* do nothing */
        }
        if (FLG_OFF == *p_skip_flg)
        {
            /* updating? */
            /* Case: Not XTS Mode */
            if (CIPHER_MOD_XTS != p_ISD->prm.AES_CIPHER.cipher_mode)
            {
                if((job_cycle & (uint8_t)JOB_UPDATE) != (uint8_t)DATA_ZERO)
                {
                    err = s_01_CIPHER_InitSrcDest(p_job, job_cycle);
                    if(err != ER_OK)
                    {
                        *p_skip_flg = FLG_ON;
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
                if(*p_skip_flg == FLG_OFF)
                {
                    /* When job_cycle is finish only */
                    if(job_cycle == (uint8_t)JOB_FINISH)
                    {
                        /* When context is 0, it means that init processing has not been executed yet */
                        if(p_job->prm.job_aes.get_context == NULL)
                        {
                            err = (uint32_t)SERV_SEQUENCE_ERROR;
                            *p_skip_flg = FLG_ON;
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
                    /* do nothing */
                }
            }
            /* Case: XTS Mode */
            else
            {
                if((job_cycle & ((uint8_t)JOB_UPDATE | (uint8_t)JOB_FINISH)) != (uint8_t)DATA_ZERO)
                {
                    err = s_01_CIPHER_InitSrcDest(p_job, job_cycle);
                    if(err != ER_OK)
                    {
                        *p_skip_flg = FLG_ON;
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
    
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0025]:[ICUMFW_UD_S_01_AES_0025]
* Function name: s_01_CIPHER_Update
* Description  : Update part of an AES encryption / decryption operation
*                Call processing by the job_cycle argument
* Arguments    : p_job
* Return Value : SERV_OK
*                ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_ML_CTX_ALL_USE
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                R_SALIF_ER_CRY_TIMEOUT
***************************************************************************************/
static uint32_t s_01_CIPHER_Update(icum_job_t *p_job, uint8_t job_cycle)
{
    r_icumif_isd_t *p_ISD;
    uint32_t err;
    
    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;
    err = (uint32_t)SERV_OK;
    
    /* Case: Not XTS Mode */
    if (CIPHER_MOD_XTS != p_ISD->prm.AES_CIPHER.cipher_mode)
    {
        if((job_cycle & (uint8_t)JOB_UPDATE) != 0u)
        {
            if((p_job->prm.job_aes.nb_slice == 0u)
            || (p_job->prm.job_aes.nb_slice >= p_job->prm.job_aes.nb_blocks))
            {
                /* Operation can be completed by one processing */
                err = s_01_CIPHER_ProcessUpdate(p_job, JOB_COMPLETE);
            }
            else
            {
                err = s_01_CIPHER_ProcessUpdate(p_job, JOB_NOT_COMPLETE);
            }
        }
        else
        {
            /* do nothing */
        }

        if (SERV_OK == err)
        {
            if((job_cycle & (uint8_t)JOB_FINISH) != 0u)
            {
                err = s_01_CIPHER_ProcessFinish(p_job);
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
    /* Case: XTS Mode */
    else
    {
        if ((job_cycle & (uint8_t)JOB_FINISH) != 0u)
        {
            if((p_job->prm.job_aes.nb_slice == 0u)
            || (p_job->prm.job_aes.nb_slice >= p_job->prm.job_aes.nb_blocks))
            {
                /* Operation can be completed by one processing */
                err = s_01_CIPHER_ProcessFinishXts(p_job);
            }
            else
            {
                if((128u != p_job->prm.job_aes.rem)
                && ((p_job->prm.job_aes.nb_blocks == (uint32_t)(p_job->prm.job_aes.nb_slice + 1u))))
                {
                    /* If there is a fraction in input, the last one needs to be processed with 2 blocks */
                    /* Round by slice so that the end does not become 1 block */
                    err = s_01_CIPHER_ProcessFinishXts(p_job);
                }
                else
                {
                    /* Perform 1 slice processing */
                    err = s_01_CIPHER_ProcessUpdateXts(p_job, JOB_NOT_COMPLETE);
                }
            }
        }
        else
        {
            if((p_job->prm.job_aes.nb_slice == 0u)
            || (p_job->prm.job_aes.nb_slice >= p_job->prm.job_aes.nb_blocks))
            {
                /* Operation can be completed by one processing */
                err = s_01_CIPHER_ProcessUpdateXts(p_job, JOB_COMPLETE);
            }
            else
            {
                /* Perform 1 slice processing */
                err = s_01_CIPHER_ProcessUpdateXts(p_job, JOB_NOT_COMPLETE);
            }
        }
    }
    
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0001]:[ICUMFW_UD_S_01_AES_0001]
* Function name: S_01_CipherAes
* Description  : Run an AES encryption / decryption operation
*                Call processing by the job_cycle argument
* Arguments    : p_job
* Return Value : SERV_OK
*                SERV_PRM_ERROR
*                SERV_PENDING
*                SERV_IN_PROCESS
*                SERV_KEY_NOT_USABLE
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_KEY_SLOT_EMPTY
*                SERV_SYS_ERROR
*                SERV_AES_TIMEOUT_ERROR
*                SERV_AES_PRM_ERROR
*                SERV_NVM_UNAVAILABLE
*                SERV_DATA_INVALID
*                SERV_NVM_SYS_ERROR_CRITICAL
*                SERV_NVM_SYS_ERROR
*                SERV_NVM_SIZE_ERROR
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
*                SERV_SEQUENCE_ERROR
***************************************************************************************/
void S_01_CipherAes(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    uint8_t job_cycle = 0;
    uint32_t err = (uint32_t)SERV_OK;
    uint32_t skip_flg = FLG_OFF;

    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;

    if ((uint8_t)JOB_CANCEL_REQUESTED == p_job->cancel_request)
    {
        if (((uint8_t)JOB_ALL_AT_ONCE == p_ISD->prm.AES_CIPHER.job_cycle)
            && ((uint16_t)NO_SLICE == p_ISD->prm.AES_CIPHER.job_slice)
            && (SERV_IN_PROCESS == p_ISD->service_result))
        {
            /* DTF running */
            p_job->cancel_request = (uint8_t)JOB_CANCEL_NO_REQUEST;
        }
        else
        {
            /* DTF no running */
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* No cancellation request */
        /* do nothing */
    }

    if(FLG_OFF == skip_flg)
    {
        job_cycle = (uint8_t)p_ISD->prm.AES_CIPHER.job_cycle;
        if(job_cycle > ((uint8_t)JOB_INIT | (uint8_t)JOB_UPDATE | (uint8_t)JOB_FINISH))
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

    if(FLG_OFF == skip_flg)
    {
        /* job_cycle check */
        if ((uint8_t)JOB_ALL_AT_ONCE == job_cycle)
        {
            job_cycle = ((uint8_t)JOB_INIT | (uint8_t)JOB_UPDATE | (uint8_t)JOB_FINISH);
            if(0uL == (uint32_t)p_ISD->prm.AES_CIPHER.job_slice)
            {
                err = s_01_CIPHER_AtOnceAes(p_job);
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
        /* do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        err = s_01_CIPHER_Init(p_job, job_cycle, &skip_flg);
    }
    else
    {
        /* do nothing */
    }
    if(FLG_OFF == skip_flg)
    {
        err = s_01_CIPHER_Update(p_job, job_cycle);
    }
    else
    {
        /* do nothing */
    }

    err = s_01_CIPHER_ConvertErr(err);

    if(p_job->cancel_request == JOB_CANCEL_REQUESTED)
    {
        p_job->keep.job_cycle = 0u;
        err = s_01_CIPHER_ReleaseKeyContext(p_job);
    }
    else
    {
        if((uint32_t)job_cycle != ((uint32_t)JOB_INIT | (uint32_t)JOB_UPDATE | (uint32_t)JOB_FINISH))
        {
            if ((((uint8_t)JOB_INIT & job_cycle) != 0u) && (SERV_PENDING == err))
            {
                p_job->keep.job_cycle = 0u;
                (void)s_01_CIPHER_ReleaseKeyContext(p_job);
            }
            /* processing in cycles (init / update / finish) */
            else if((err == (uint32_t)SERV_IN_PROCESS) || (err == (uint32_t)SERV_PENDING))
            {
                /* memory not available OR processing in job slices: need to come back */
            }
            else if((err == ER_OK) && ((job_cycle & (uint8_t)JOB_FINISH) == 0u))
            {
                /* not finished yet: keep a ghost of the job */
                p_job->keep.job_cycle = 1u;
            }
            else
            {
                p_job->keep.job_cycle = 0u;
                if(err == ER_OK)
                {
                    err = s_01_CIPHER_ReleaseKeyContext(p_job);
                }
                else
                {
                    /* Prioritize errors that occurred before coming here*/
                    /* do not get s_01_CIPHER_ReleaseKeyContext error */
                    (void)s_01_CIPHER_ReleaseKeyContext(p_job);
                }
            }
        }
        else
        {
            /* processing in cycles all at once */
            if(err == (uint32_t)SERV_IN_PROCESS)
            {
                /* memory not available OR processing in job slices: need to come back */
            }
            else
            {
                if(err == ER_OK)
                {
                    err = s_01_CIPHER_ReleaseKeyContext(p_job);
                }
                else
                {
                    /* Prioritize errors that occurred before coming here */
                    /* do not get s_01_CIPHER_ReleaseKeyContext error */
                    (void)s_01_CIPHER_ReleaseKeyContext(p_job);
                }
            }
        }
    }
    p_ISD->service_result = (r_icumif_service_result_t)err;
}

