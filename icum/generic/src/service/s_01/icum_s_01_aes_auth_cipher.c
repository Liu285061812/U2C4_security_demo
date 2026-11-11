/******************************************************************************/
/* Component Name   ICU-M Service 01                                          */
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

#define GCM_IV_MAX               ((uint32_t)1024u)       /* Maximum size of IV value in GCM mode */
/* DATA SIZE */
#define DATA_NOT_ZERO       (0x01u)
#define GCM_AUTH_SET_SIZE   (32u)
#define SIZE_IN_BITS        (128u)
#define NUM3                (3u)
#define NUM4                (4UL)
#define NUM8                (8UL)
#define NUM_FF              (0xffUL)
#define NUM7                (0x07UL)
#define NUM32                         (32u)
#define NUM128                       (128u)

/* TAG SIZE */
#define TAG_SIZE_32         ((uint8_t)32u)
#define TAG_SIZE_48         ((uint8_t)48u)
#define TAG_SIZE_64         ((uint8_t)64u)
#define TAG_SIZE_80         ((uint8_t)80u)
#define TAG_SIZE_96         ((uint8_t)96u)
#define TAG_SIZE_104        ((uint8_t)104u)
#define TAG_SIZE_112        ((uint8_t)112u)
#define TAG_SIZE_120        ((uint8_t)120u)
#define TAG_SIZE_128        ((uint8_t)128u)

/* IV SIZE */
#define IV_SIZE_56          ((uint32_t)56u)
#define IV_SIZE_64          ((uint32_t)64u)
#define IV_SIZE_72          ((uint32_t)72u)
#define IV_SIZE_80          ((uint32_t)80u)
#define IV_SIZE_88          ((uint32_t)88u)
#define IV_SIZE_96          ((uint32_t)96u)
#define IV_SIZE_104         ((uint32_t)104u)

extern uint32_t g_ContextAddress;
/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t s_01_AUTH_ProcessUpdateCcmEnd(icum_job_t *p_job, uint32_t step, uint32_t blocks,
                                              uint32_t finish, uint8_t job_cycle);
static uint32_t s_01_AUTH_CheckDtfProcessing(icum_job_t *p_job, uint32_t set_resource_id,
                                             uint8_t *p_auth_tag, uint8_t *p_cmp_target, uint16_t cmp_size);
static uint32_t s_01_AUTH_AtOnceAesCcmCheckParam(icum_job_t *p_job);
static uint32_t s_01_AUTH_Gcm(icum_job_t *p_job, uint8_t Local_job_cycle);
static uint32_t s_01_AUTH_Ccm(icum_job_t *p_job, uint8_t Local_job_cycle);
static uint32_t s_01_AUTH_CheckMemory(uint32_t data_size, const uint32_t* data_add,
                                      uint8_t direction, uint8_t expansion);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0017]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0017]
* Function name: s_01_AUTH_ReleaseKeyContext
* Description  : Process of releasing the key and context in use
* Arguments    : p_job
* Return Value : SERV_OK(ER_OK)
*                ER_PARAM
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_DATA_NOT_FOUND
*                ER_ML_CTX_NOT_FOUND
***************************************************************************************/
static uint32_t s_01_AUTH_ReleaseKeyContext(icum_job_t *p_job)
{
    aes_release_t        release_param;
    aes_finish_ccm_t     finish_param;
    ml_ctx_aes_context_t *p_ctx;

    r_key_group_t key_group;
    r_key_index_t key_id;
    uint32_t err = (uint32_t)SERV_OK;
    uint32_t skip_flg = FLG_OFF;

    L_Toolbox_MemSet((uint8_t *)&release_param, DATA_ZERO, (int16_t)sizeof(aes_release_t));
    L_Toolbox_MemSet((uint8_t *)&finish_param,  DATA_ZERO, (int16_t)sizeof(aes_finish_ccm_t));

    if (p_job->prm.job_aes_auth.get_context != NULL)
    {
        p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes_auth.get_context;
        release_param.key_length_id = *((uint32_t*)p_ctx->context);
        release_param.key_length    = *((uint16_t*)(p_ctx->context + ADDR_1ST));
        err = D_AES_ReleaseKeyRange(&release_param);

        err = ML_CTX_Free(CTX_TYPE_AES, (uint32_t*)p_job->prm.job_aes_auth.get_context);
        p_job->prm.job_aes_auth.get_context = NULL;
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

    if (FLG_OFF == skip_flg)
    {
        if ((uint32_t)p_job->prm.job_aes_auth.get_key == (uint32_t)RESOURCES_HOLD)
        {
            key_group = p_job->p_ISD->prm.AES_AUTH_CIPHER.key_group;
            key_id    = p_job->p_ISD->prm.AES_AUTH_CIPHER.key_id;

            if (key_group == KEY_GRP_SHE)
            {
                /* convert from SHE index to internal group / id */
                (void)MH_KEY_ConvertSheKey(key_id, &key_group, &key_id);
            }
            else
            {
                /* do nothing */
            }
            err = MH_KEY_FreeKeyCache(p_job, key_group, key_id);
            p_job->prm.job_aes_auth.get_key = RESOURCES_NOT_HOLD;
        }
        else
        {
            /* do nothing */
        }
        if (AUTH_CIPHER_MOD_CCM == p_job->p_ISD->prm.AES_AUTH_CIPHER.cipher_mode
                && ((uint32_t)p_job->prm.job_aes_auth.ccm_cancel == 1UL))
        {
            /* Common Parameter */
            g_ContextAddress       = AES_STATE_INIT;
            finish_param.operation = D_AES_OPE_CANCEL;
            err = D_AES_FinishJobCcm(&finish_param);
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
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0004]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0004]
* Function name: s_01_CIPHER_InitAesCipher
* Description  : Initialization of the ciphering operation
* Arguments    : p_job
* Return Value : SERV_OK(ER_OK)
*                ER_PARAM
*                ER_PENDING
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
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
static uint32_t s_01_AUTH_InitAesCipherGcm(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    ml_ctx_aes_context_t *p_ctx;
    aes_init_gcm_t        init_param;

    s_01_com_key_param_t KeyParam1;
    aes_key_t *p_key;
    uint32_t err;
    uint32_t resource_id;
    uint32_t skip_flg;
    uint32_t iv_logical;

    p_key = NULL;
    err = ER_OK;
    skip_flg = FLG_OFF;

    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;
    L_Toolbox_MemSet((uint8_t *)&init_param, DATA_ZERO, (int16_t)sizeof(aes_init_gcm_t));

    /* Check range direction */
    if (CIPHER_DIR_ENCRYPTION == p_ISD->prm.AES_AUTH_CIPHER.direction)
    {
        p_job->prm.job_aes_auth.direction = D_AES_DIR_ENCRYPTION;
    }
    else if (CIPHER_DIR_DECRYPTION == p_ISD->prm.AES_AUTH_CIPHER.direction)
    {
        p_job->prm.job_aes_auth.direction = D_AES_DIR_DECRYPTION;
    }
    else
    {
        err = ER_PARAM;
        skip_flg = FLG_ON;
    }

    if (FLG_OFF == skip_flg)
    {
        /* Address check of auth_tag size */
        if(p_ISD->prm.AES_AUTH_CIPHER.direction == CIPHER_DIR_ENCRYPTION)
        {
            err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits,
                                            p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag,
                                        FLS_CHK_MEM_DESTINATION,
                                            p_job->expansion_service);
        }
        else
        {
            err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits,
                                            p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag,
                                        FLS_CHK_MEM_SOURCE,
                                            p_job->expansion_service);
        }
        if(err == ER_OK)
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
        /* Address check of iv size */
        err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits, p_ISD->prm.AES_AUTH_CIPHER.p_iv,
                                        FLS_CHK_MEM_SOURCE, p_job->expansion_service);
        if(err == ER_OK)
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
        /* GCM mode size check */
        if((1UL > p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits)
                || (p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits > GCM_IV_MAX))
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            if (((uint32_t)TAG_SIZE_32 == (uint32_t)p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits) ||
                ((uint32_t)TAG_SIZE_64 == (uint32_t)p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits) ||
                ((uint32_t)TAG_SIZE_96 == (uint32_t)p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits) ||
                ((uint32_t)TAG_SIZE_104 == (uint32_t)p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits) ||
                ((uint32_t)TAG_SIZE_112 == (uint32_t)p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits) ||
                ((uint32_t)TAG_SIZE_120 == (uint32_t)p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits) ||
                ((uint32_t)TAG_SIZE_128 == (uint32_t)p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits))
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
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* Get AES Key */
        KeyParam1.key_group      = p_ISD->prm.AES_AUTH_CIPHER.key_group;
        KeyParam1.key_id         = p_ISD->prm.AES_AUTH_CIPHER.key_id;
        KeyParam1.NotUseFlag     = 0U;
        KeyParam1.direction      = p_job->prm.job_aes_auth.direction;
        KeyParam1.p_aeskey       = &p_key;

        err = s_01_CIPHER_GetAESKey(p_job, &KeyParam1);
        if(err == ER_OK)
        {
            /* Get Key success. */
            p_job->prm.job_aes_auth.get_key = RESOURCES_HOLD;
        }
        else
        {
            /*  Error Occurred */
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        err = ML_CTX_Get(CTX_TYPE_AES, (uint32_t**)&p_job->prm.job_aes_auth.get_context);
        p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes_auth.get_context;
        if(err != ER_OK)
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
    if (FLG_OFF == skip_flg)
    {
        err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
        if(ER_OK != err)
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
    if (FLG_OFF == skip_flg)
    {
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_AUTH_CIPHER.p_iv, &iv_logical);
        g_ContextAddress  = AES_STATE_INIT;
        if (NULL != p_ctx)
        {
            if (NUM0 == p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits)
            {
                err = ER_PARAM;
            }
            else
            {
                p_ctx->auth_step  = D_AES_GCM_STEP_IV;
                p_ctx->block_info = D_AES_BLOCK_INFO_FIRST;
                p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info | (uint32_t)D_AES_BLOCK_INFO_LAST);
                p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info
                                                 | (uint32_t)(p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS));
                if (p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits == AES_GCM_IV_SIZE_96)
                {
                    p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info | (uint32_t)D_AES_BLOCK_INFO_GCM_IV96);
                }
                else
                {
                    /* do Nothing*/
                }

                init_param.number     = ((p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits
                                        + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1))/ (uint32_t)AES_BLOCK_SIZE_IN_BITS);
                init_param.block_info = p_ctx->block_info;
                init_param.direction  = (uint8_t)p_job->prm.job_aes_auth.direction;
                init_param.p_aes_key  = (uint32_t *)p_key->value;
                init_param.key_length = p_key->properties.size_in_bytes;
                init_param.p_aes_iv   = (uint32_t *)iv_logical;
                init_param.p_iv_size_in_bits = &p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits;
                init_param.p_ctr_j0   = p_ctx->gcm_j0;
                init_param.p_context  = p_ctx->context;
                err = D_AES_InitJobGcm(&init_param);
                p_ctx->block_info     = (uint16_t)((uint32_t)p_ctx->block_info & (uint32_t)(~D_AES_BLOCK_INFO_FIRST));
                p_ctx->block_info     = (uint16_t)((uint32_t)p_ctx->block_info & (uint32_t)(~D_AES_BLOCK_INFO_SIZE));
                p_ctx->auth_step      = D_AES_GCM_STEP_AAD_FIRST;
            }
        }
        else
        {
            err = ER_PARAM;
        }

        (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
    }
    else
    {
        /* do nothing */
    }
    return (err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0005]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0005]
* Function name: s_01_CIPHER_InitSrcDest
* Description  : Set number and address for Update process
* Arguments    : p_job
* Return Value : SERV_OK(ER_OK)
*                ER_PARAM
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
*                SERV_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_InitSrcDestGcm(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    uint32_t err = (uint32_t)SERV_OK;
    uint32_t skip_flg = FLG_OFF;

    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;

    /* When context is 0, it means that init processing has not been executed yet */
    if(p_job->prm.job_aes_auth.get_context == NULL)
    {
        err = (uint32_t)SERV_SEQUENCE_ERROR;
        skip_flg = FLG_ON;
    }
    else
    {
        /* do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        p_job->prm.job_aes_auth.p_source     = p_ISD->prm.AES_AUTH_CIPHER.p_data_in;
        p_job->prm.job_aes_auth.p_auth_data  = p_ISD->prm.AES_AUTH_CIPHER.p_auth_data;
        p_job->prm.job_aes_auth.p_dest       = p_ISD->prm.AES_AUTH_CIPHER.p_data_out;
        /* get the job slice configured */
        p_job->prm.job_aes_auth.nb_slice = p_ISD->prm.AES_AUTH_CIPHER.job_slice;
        /* Calculation of block number of cipher */
        err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits
                                    , p_job->prm.job_aes_auth.p_source
                                    , FLS_CHK_MEM_SOURCE, p_job->expansion_service);
        if( err == ER_OK)
        {
            err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits
                                        , p_job->prm.job_aes_auth.p_dest
                                        , FLS_CHK_MEM_DESTINATION, p_job->expansion_service);
            if( err == ER_OK )
            {
                p_job->prm.job_aes_auth.src_size_bits = p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits;
            }
            else
            {
                skip_flg = FLG_ON;
            }
        }
        else
        {
            skip_flg = FLG_ON;
        }

        if (FLG_OFF == skip_flg)
        {
            /* Calculation of block number of AAD */
            if (0UL == (uint32_t)p_job->prm.job_aes_auth.not_reverse)
            {
               err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits
                                            , p_job->prm.job_aes_auth.p_auth_data
                                            , FLS_CHK_MEM_SOURCE, p_job->expansion_service);
                if( err == ER_OK )
                {
                    p_job->prm.job_aes_auth.auth_size_bits = p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits;
                }
                else
                {
                    skip_flg = FLG_ON;
                }
            }
            else
            {
                /* Ignore as it is invalid data */
                p_job->prm.job_aes_auth.auth_size_bits = 0U;
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
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0007]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0007]
* Function name: s_01_AUTH_ProcessUpdateGcm
* Description  : Processing update Cipher(GCM)
* Arguments    : p_job
*                step
*                blocks
*                finish
*                job_cycle
* Return Value : SERV_OK(ER_OK)
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_ProcessUpdateGcm(icum_job_t *p_job, uint8_t step, uint32_t blocks,
                                           uint16_t finish, uint8_t job_cycle)
{
    ml_ctx_aes_context_t *p_ctx;
    aes_update_gcm_t     update_param;

    uint32_t err;
    uint32_t resource_id;
    uint32_t skip_flg = FLG_OFF;
    uint32_t src_logical;
    uint32_t dest_logical;
    uint32_t auth_logical;

    uint32_t* p_auth_data = NULL;
    uint32_t  msg_size_in_bits = 0;
    uint32_t  aad_size_in_bits = 0;
    uint8_t*  p_dst = NULL;
    uint8_t*  p_src = NULL;
    uint32_t *p_counter;

    p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes_auth.get_context;
    L_Toolbox_MemSet((uint8_t *)&update_param, DATA_ZERO, (int16_t)sizeof(aes_update_gcm_t));

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
        if ((uint32_t)D_AES_GCM_STEP_AAD == (uint32_t)step)
        {
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes_auth.p_auth_data, &auth_logical);
            p_auth_data      = (uint32_t*)auth_logical;
            p_dst            = NULL;
            aad_size_in_bits = blocks;
        }
        else
        {
           (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes_auth.p_source, &src_logical);
           (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes_auth.p_dest, &dest_logical);
            p_src = (uint8_t*)src_logical;
            p_dst = (uint8_t*)dest_logical;
            msg_size_in_bits = blocks;
        }

        g_ContextAddress = AES_STATE_INIT;
        if (NULL != p_ctx)
        {
            if (( (uint32_t)p_ctx->auth_step == (uint32_t)D_AES_GCM_STEP_AAD)
                || ( (uint32_t)p_ctx->auth_step == (uint32_t)D_AES_GCM_STEP_AAD_FIRST))
            {
                if (NUM0 == aad_size_in_bits)
                {
                    if ((uint32_t)p_ctx->auth_step == (uint32_t)D_AES_GCM_STEP_AAD_FIRST)
                    {
                        p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info | (uint32_t)D_AES_BLOCK_INFO_GCM_ADD0);
                    }
                    else
                    {
                        /* do nothing */
                    }
                    p_ctx->auth_step = D_AES_GCM_STEP_CIPHER;
                }
                else
                {
                    p_ctx->auth_step = D_AES_GCM_STEP_AAD;
                    if (NUM0 != (aad_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS))
                    {
                        p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info | (uint32_t)D_AES_BLOCK_INFO_LAST);
                        p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info | (uint32_t)(aad_size_in_bits 
                                                                        % (uint32_t)AES_BLOCK_SIZE_IN_BITS));
                    }
                    else
                    {
                        /* do nothing */
                    }
                    update_param.number = ((aad_size_in_bits 
                                    + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1)) / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
                    update_param.block_info = p_ctx->block_info;
                    update_param.step       = p_ctx->auth_step;
                    update_param.p_aes_src  = p_auth_data;
                    update_param.p_iv_size_in_bits = NULL;
                    update_param.p_context         = p_ctx->context;
                    update_param.p_ctr_j0   = NULL;
                    update_param.p_aes_dest = (uint32_t*)p_dst;
                    err = D_AES_UpdateJobGcm(&update_param);
                    p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info & (uint32_t)(~D_AES_BLOCK_INFO_FIRST));
                    p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info & (uint32_t)(~D_AES_BLOCK_INFO_LAST));
                    p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info & (uint32_t)(~D_AES_BLOCK_INFO_SIZE));

                    p_counter       = &p_ctx->gcm_total_size[NUM0];
                    p_counter[NUM1] = L_Toolbox_ByteSwap(p_counter[NUM1]);
                    /* If p_counter[1] overflows */
                    if ((COUNTER_MAX - p_counter[NUM1]) < aad_size_in_bits)
                    {
                        p_counter[NUM0] = L_Toolbox_ByteSwap(p_counter[NUM0]);
                        ++p_counter[NUM0];
                        p_counter[NUM0] = L_Toolbox_ByteSwap(p_counter[NUM0]);
                    }
                   else
                    {
                         /* do nothing */
                    }
                    p_counter[NUM1] += aad_size_in_bits;
                    p_counter[NUM1]  = L_Toolbox_ByteSwap(p_counter[NUM1]);

                    if (NUM0 != msg_size_in_bits)
                    {
                        p_ctx->auth_step = D_AES_GCM_STEP_CIPHER;
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

            if ((uint32_t)D_AES_GCM_STEP_CIPHER == (uint32_t)p_ctx->auth_step)
            {
                if ((NUM0 == msg_size_in_bits) 
                    || (NUM0 != (msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS)))
                {
                    err = ER_PARAM;
                }
                else
                {
                    update_param.number = ((msg_size_in_bits
                                    + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1)) / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
                    update_param.block_info = p_ctx->block_info;
                    update_param.step       = p_ctx->auth_step;
                    update_param.p_aes_src  = (uint32_t*)p_src;
                    update_param.p_iv_size_in_bits = NULL;
                    update_param.p_context         = p_ctx->context;
                    update_param.p_ctr_j0          = NULL;
                    update_param.p_aes_dest        = (uint32_t*)p_dst;
                    err = D_AES_UpdateJobGcm(&update_param);
                    p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info & (uint32_t)(~D_AES_BLOCK_INFO_FIRST));
                }

                p_counter       = &p_ctx->gcm_total_size[2];
                p_counter[NUM1] = L_Toolbox_ByteSwap(p_counter[NUM1]);
                /* If p_counter[1] overflows */
                if ((COUNTER_MAX - p_counter[NUM1]) < msg_size_in_bits)
                {
                    p_counter[NUM0] = L_Toolbox_ByteSwap(p_counter[NUM0]);
                    ++p_counter[NUM0];
                    p_counter[NUM0] = L_Toolbox_ByteSwap(p_counter[NUM0]);
                }
                else
                {
                     /* do nothing */
                }
                p_counter[NUM1] += msg_size_in_bits;
                p_counter[NUM1]  = L_Toolbox_ByteSwap(p_counter[NUM1]);
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            err = ER_PARAM;
        }

        /* any error? */
        if(ER_OK == err)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
            if(ER_OK == err)
            {
                if((uint32_t)finish == (uint32_t)JOB_COMPLETE)
                {
                    if( (uint32_t)D_AES_GCM_STEP_AAD == (uint32_t)step )
                    {
                        p_job->prm.job_aes_auth.auth_size_bits = 0UL;
                        if((p_job->prm.job_aes_auth.src_size_bits == 0UL)
                                && (((uint32_t)job_cycle & (uint32_t)JOB_FINISH) == 0UL))
                        {
                            /* End processing (there is no next processing) */
                            err = (uint32_t)SERV_OK;
                        }
                        else
                        {
                            err = (uint32_t)SERV_IN_PROCESS;
                        }
                    }
                    else
                    {
                        p_job->prm.job_aes_auth.src_size_bits = 0u;
                        if(((uint32_t)job_cycle & (uint32_t)JOB_FINISH) == 0UL)
                        {
                            /* End processing (there is no next processing) */
                            err = (uint32_t)SERV_OK;
                        }
                        else
                        {
                            err = (uint32_t)SERV_IN_PROCESS;
                        }
                    }
                }
                else
                {
                    if ((uint32_t)D_AES_GCM_STEP_AAD == (uint32_t)step)
                    {
                        p_job->prm.job_aes_auth.auth_size_bits -= blocks;
                        p_job->prm.job_aes_auth.p_auth_data    += (blocks/32);
                    }
                    else
                    {
                        p_job->prm.job_aes_auth.src_size_bits -= blocks;
                        p_job->prm.job_aes_auth.p_source  += (blocks/32);
                        p_job->prm.job_aes_auth.p_dest    += (blocks/32);
                    }
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
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0008]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0008]
* Function name: s_01_AUTH_ProcessFinishGcm
* Description  : Processing finish AES Cipher(GCM)
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                ER_D_AES_ALIGN_ERROR
*                ER_S01_SRC_MEM_ERROR
*                ER_S01_DEST_MEM_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_ProcessFinishGcm(icum_job_t *p_job)
{
    aes_update_gcm_t      update_param;
    aes_finish_gcm_t      finish_param;
    ml_ctx_aes_context_t *p_ctx;

    uint32_t auth_tag[4];
    uint32_t resource_id;
    uint32_t err;
    uint32_t cipher_data[AES_BLOCK_SIZE];
    uint8_t cmp_result;
    uint32_t skip_flg = FLG_OFF;
    uint32_t src_logical = 0;
    uint32_t dest_logical = 0;
    uint32_t tag_logical = 0;
    uint32_t size;
    uint32_t *p_counter;
    uint32_t msg_size_in_bits = 0;

    p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes_auth.get_context;
    L_Toolbox_MemSet((uint8_t *)&finish_param, DATA_ZERO, (int16_t)sizeof(aes_finish_gcm_t));
    L_Toolbox_MemSet((uint8_t *)&update_param, DATA_ZERO, (int16_t)sizeof(aes_update_gcm_t));

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
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes_auth.p_source, &src_logical);
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes_auth.p_dest, &dest_logical);
        if ((p_job->prm.job_aes_auth.src_size_bits % 128UL) != 0)
        {
            /* If there is a fraction, adjust the output size of the final block */
            if (p_job->prm.job_aes_auth.src_size_bits > 128UL)
            {
                /* Calculate with the Update function until the remaining 1block */
                msg_size_in_bits = ((p_job->prm.job_aes_auth.src_size_bits / (uint32_t)SIZE_IN_BITS)
                                                                               * (uint32_t)SIZE_IN_BITS);

                g_ContextAddress = AES_STATE_INIT;
                if (NULL != p_ctx)
                {
                    if (( (uint32_t)p_ctx->auth_step == (uint32_t)D_AES_GCM_STEP_AAD)
                        || ( (uint32_t)p_ctx->auth_step == (uint32_t)D_AES_GCM_STEP_AAD_FIRST))
                    {
                        if ((uint32_t)p_ctx->auth_step == (uint32_t)D_AES_GCM_STEP_AAD_FIRST)
                        {
                            p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info | (uint32_t)D_AES_BLOCK_INFO_GCM_ADD0);
                        }
                        else
                        {
                            /* do nothing */
                        }
                        p_ctx->auth_step = D_AES_GCM_STEP_CIPHER;
                    }
                    else
                    {
                        /* do nothing */
                    }

                    if ((uint32_t)D_AES_GCM_STEP_CIPHER == (uint32_t)p_ctx->auth_step)
                    {
                        if ((NUM0 == msg_size_in_bits) || (NUM0 != (msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS)))
                        {
                            err = ER_PARAM;
                        }
                        else
                        {
                            update_param.number = ((msg_size_in_bits
                                            + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1)) / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
                            update_param.block_info = p_ctx->block_info;
                            update_param.step       = p_ctx->auth_step;
                            update_param.p_aes_src  = (uint32_t*)src_logical;
                            update_param.p_iv_size_in_bits = NULL;
                            update_param.p_context         = p_ctx->context;
                            update_param.p_ctr_j0          = NULL;
                            update_param.p_aes_dest        = (uint32_t*)dest_logical;
                            err = D_AES_UpdateJobGcm(&update_param);
                            p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info & (uint32_t)(~D_AES_BLOCK_INFO_FIRST));
                        }

                        p_counter       = &p_ctx->gcm_total_size[2];
                        p_counter[NUM1] = L_Toolbox_ByteSwap(p_counter[NUM1]);
                        /* If p_counter[1] overflows */
                        if ((COUNTER_MAX - p_counter[NUM1]) < msg_size_in_bits)
                        {
                            p_counter[NUM0] = L_Toolbox_ByteSwap(p_counter[NUM0]);
                            ++p_counter[NUM0];
                            p_counter[NUM0] = L_Toolbox_ByteSwap(p_counter[NUM0]);
                        }
                        else
                        {
                            /* do nothing */
                        }
                        p_counter[NUM1] += msg_size_in_bits;
                        p_counter[NUM1]  = L_Toolbox_ByteSwap(p_counter[NUM1]);
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
                else
                {
                    err = ER_PARAM;
                }

                /* any error? */
                if(ER_OK == err)
                {
                    p_job->prm.job_aes_auth.block_info &= ~D_AES_BLOCK_INFO_FIRST;
                    p_job->prm.job_aes_auth.src_size_bits -= msg_size_in_bits;
                    p_job->prm.job_aes_auth.p_source      += (msg_size_in_bits / 32UL);
                    p_job->prm.job_aes_auth.p_dest        += (msg_size_in_bits / 32UL);
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
                /* Set finish parameter */
                (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes_auth.p_source, &src_logical);
                (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes_auth.p_dest, &dest_logical);
                if (NULL != p_ctx)
                {
                    /* AES Finish */
                    msg_size_in_bits = p_job->prm.job_aes_auth.src_size_bits;
                    p_counter        = &p_ctx->gcm_total_size[2];
                    p_counter[NUM1]  = L_Toolbox_ByteSwap(p_counter[NUM1]);
                    /* If p_counter[1] overflows */
                    if ((COUNTER_MAX - p_counter[NUM1]) < msg_size_in_bits)
                    {
                        p_counter[NUM0] = L_Toolbox_ByteSwap(p_counter[NUM0]);
                        ++p_counter[NUM0];
                        p_counter[NUM0] = L_Toolbox_ByteSwap(p_counter[NUM0]);
                    }
                    else
                    {
                        /* do nothing */
                    }
                    p_counter[NUM1] += msg_size_in_bits;
                    p_counter[NUM1]  = L_Toolbox_ByteSwap(p_counter[NUM1]);

                    p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info | (uint32_t)D_AES_BLOCK_INFO_LAST);
                    p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info
                                                    | (uint32_t)(msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS));
                    g_ContextAddress    = AES_STATE_INIT;
                    finish_param.number = ((msg_size_in_bits 
                                            + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1)) / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
                    finish_param.block_info = p_ctx->block_info;
                    finish_param.p_aes_src  = (uint32_t*)src_logical;
                    finish_param.p_total_size_in_bits = p_ctx->gcm_total_size;
                    finish_param.p_ctr_j0   = p_ctx->gcm_j0;
                    finish_param.p_context  = p_ctx->context;
                    finish_param.p_aes_dest = (uint32_t*)cipher_data;
                    finish_param.p_auth_tag = auth_tag;
                    err = D_AES_FinishJobGcm(&finish_param);
                }
                else
                {
                    err = ER_PARAM;
                }

                /* Output only valid sizes */
                L_Toolbox_MemCpy((uint8_t *)dest_logical,
                        (const uint8_t *)cipher_data,
                        (((uint32_t)p_job->prm.job_aes_auth.src_size_bits + ((uint32_t)AES_BYTE_SIZE_IN_BITS - 1UL))
                                / (uint32_t)AES_BYTE_SIZE_IN_BITS));
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            if (NULL != p_ctx)
            {
                /* AES Finish */
                msg_size_in_bits = p_job->prm.job_aes_auth.src_size_bits;
                p_counter        = &p_ctx->gcm_total_size[2];
                p_counter[NUM1]  = L_Toolbox_ByteSwap(p_counter[NUM1]);
                /* If p_counter[1] overflows */
                if ((COUNTER_MAX - p_counter[NUM1]) < msg_size_in_bits)
                {
                    p_counter[NUM0] = L_Toolbox_ByteSwap(p_counter[NUM0]);
                    ++p_counter[NUM0];
                    p_counter[NUM0] = L_Toolbox_ByteSwap(p_counter[NUM0]);
                }
                else
                {
                    /* do nothing */
                }
                p_counter[NUM1] += msg_size_in_bits;
                p_counter[NUM1]  = L_Toolbox_ByteSwap(p_counter[NUM1]);

                p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info | (uint32_t)D_AES_BLOCK_INFO_LAST);
                p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info
                                                | (uint32_t)(msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS));
                finish_param.number = ((msg_size_in_bits 
                                        + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM3)) / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
                g_ContextAddress        = AES_STATE_INIT;
                finish_param.block_info = p_ctx->block_info;
                finish_param.p_aes_src  = (uint32_t*)src_logical;
                finish_param.p_total_size_in_bits = p_ctx->gcm_total_size;
                finish_param.p_ctr_j0   = p_ctx->gcm_j0;
                finish_param.p_context  = p_ctx->context;
                finish_param.p_aes_dest = (uint32_t*)dest_logical;
                finish_param.p_auth_tag = auth_tag;
                err = D_AES_FinishJobGcm(&finish_param);
            }
            else
            {
                err = ER_PARAM;
            }
        }
    }
    else
    {
        /* do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        if(ER_OK == err)
        {
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, &tag_logical);
            size = (uint32_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / (uint32_t)AES_BYTE_SIZE_IN_BITS;
            if((uint32_t)D_AES_DIR_DECRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction)
            {
                cmp_result = L_Toolbox_MemCmp((uint8_t*)auth_tag, (uint8_t*)tag_logical, (int16_t)size);
                if(0UL == (uint32_t)cmp_result)
                {
                    p_job->p_ISD->prm.AES_AUTH_CIPHER.verification_result = VERIFICATION_PASS;
                }
                else
                {
                    p_job->p_ISD->prm.AES_AUTH_CIPHER.verification_result = VERIFICATION_FAIL;
                }
            }
            else
            {
                L_Toolbox_MemCpy((uint8_t *)tag_logical, (const uint8_t *)auth_tag, (int16_t)size);
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

    if(0UL != resource_id)
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
    else
    {
        /* do nothing */
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0006]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0006]
* Function name: s_01_AUTH_ExecutionProcessGcm
* Description  : Process of releasing the key and context in use
* Arguments    : p_job
*              : job_cycle
* Return Value : SERV_OK(ER_OK)
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_ExecutionProcessGcm(icum_job_t *p_job, uint8_t job_cycle)
{
    uint32_t err = (uint32_t)SERV_OK;
    uint32_t skip_flg = FLG_OFF;

    if((uint32_t)p_job->prm.job_aes_auth.nb_slice != 0UL)
    {
        /* AAD operation processing */
        if(p_job->prm.job_aes_auth.auth_size_bits != 0UL )
        {
            if(p_job->prm.job_aes_auth.auth_size_bits > (uint32_t)(p_job->prm.job_aes_auth.nb_slice * 128UL))
            {
                /* Continue AAD operation */
                err = s_01_AUTH_ProcessUpdateGcm(p_job, D_AES_GCM_STEP_AAD
                                                    ,(uint32_t)(p_job->prm.job_aes_auth.nb_slice * 128UL)
                                                    , JOB_NOT_COMPLETE, job_cycle);
                skip_flg = FLG_ON;
            }
            else
            {
                /* AAD operation is completed */
                err = s_01_AUTH_ProcessUpdateGcm(p_job, D_AES_GCM_STEP_AAD, p_job->prm.job_aes_auth.auth_size_bits
                                                , JOB_COMPLETE, job_cycle);
                skip_flg = FLG_ON;
            }
        }
        else
        {
            /* do nothing */
        }
        if (FLG_OFF == skip_flg)
        {
            /* Cipher operation processing */
            if( p_job->prm.job_aes_auth.src_size_bits != 0 )
            {
                p_job->prm.job_aes_auth.not_reverse = 1u;
                if(p_job->prm.job_aes_auth.src_size_bits > (uint32_t)(p_job->prm.job_aes_auth.nb_slice * 128UL))
                {
                    /* Continue Cipher operation */
                    err = s_01_AUTH_ProcessUpdateGcm(p_job,D_AES_GCM_STEP_CIPHER
                                                        , (uint32_t)(p_job->prm.job_aes_auth.nb_slice * 128UL)
                                                        , JOB_NOT_COMPLETE, job_cycle);
                    skip_flg = FLG_ON;
                }
                else
                {
                    /* Cipher operation is completed */
                    if((job_cycle & (uint8_t)JOB_FINISH) != 0u)
                    {
                        /* Including job finish */
                        err = s_01_AUTH_ProcessFinishGcm(p_job);
                        skip_flg = FLG_ON;
                    }
                    else
                    {
                        /* slice is over but not complete */
                        err = s_01_AUTH_ProcessUpdateGcm(p_job, D_AES_GCM_STEP_CIPHER
                                                            , p_job->prm.job_aes_auth.src_size_bits
                                                            , JOB_COMPLETE, job_cycle);
                        skip_flg = FLG_ON;
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
    }
    else
    {
        /* AAD operation processing */
        if( p_job->prm.job_aes_auth.auth_size_bits != 0 )
        {
            /* AAD operation is completed */
            err = s_01_AUTH_ProcessUpdateGcm(p_job, D_AES_GCM_STEP_AAD, p_job->prm.job_aes_auth.auth_size_bits
                                            , JOB_COMPLETE, job_cycle);
            if(err != SERV_IN_PROCESS)
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
        if (FLG_OFF == skip_flg)
        {
            /* Cipher operation processing */
            if( p_job->prm.job_aes_auth.src_size_bits != 0 )
            {
                p_job->prm.job_aes_auth.not_reverse = 1u;
                /* Cipher operation is completed */
                if((job_cycle & (uint8_t)JOB_FINISH) != 0u)
                {
                    /* Including job finish */
                    err = s_01_AUTH_ProcessFinishGcm(p_job);
                    skip_flg = FLG_ON;
                }
                else
                {
                    /* slice is over but not complete */
                    err = s_01_AUTH_ProcessUpdateGcm(p_job, D_AES_GCM_STEP_CIPHER
                                                        , p_job->prm.job_aes_auth.src_size_bits
                                                        , JOB_COMPLETE, job_cycle);
                    skip_flg = FLG_ON;
                }
            } /* end of if (p_job->prm.job_aes_auth.src_size_bits != 0u) */
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
    if (FLG_OFF == skip_flg)
    {
        /* If auth and cipher are 0 or cipher is 0, finish is done here */
        if((p_job->prm.job_aes_auth.auth_size_bits == 0)
                && (p_job->prm.job_aes_auth.src_size_bits == 0)
                && ((job_cycle & (uint8_t)JOB_FINISH) != 0u))
        {
            /* Including job finish */
            err = s_01_AUTH_ProcessFinishGcm(p_job);
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

    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0019]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0019]
* Function name: s_01_AUTH_CheckRange
* Description  : Check range
* Arguments    : p_job
*                p_skip_flg
* Return Value : ER_OK
*                ER_PARAM
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_CheckRange(icum_job_t *p_job, uint32_t *p_skip_flg)
{
    uint32_t err;
    uint32_t skip_flg;
    r_icumif_isd_t *p_ISD;

    err = ER_OK;
    skip_flg = FLG_OFF;

    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;

    /* Process 1 : Encryption / decryption mode (direction) range judgment */
    switch (p_ISD->prm.AES_AUTH_CIPHER.direction)
    {
        case CIPHER_DIR_ENCRYPTION:
            p_job->prm.job_aes_auth.direction = D_AES_DIR_ENCRYPTION;
            break;
        case CIPHER_DIR_DECRYPTION:
            p_job->prm.job_aes_auth.direction = D_AES_DIR_DECRYPTION;
            break;
        default:
            err = ER_PARAM;
            break;
    }
    if (ER_OK == err)
    {
        /* Process 2 : Check the address range of TAG data */
        /* Address check of auth_tag size */
        if (p_ISD->prm.AES_AUTH_CIPHER.direction == CIPHER_DIR_ENCRYPTION)
        {
            err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits, 
                p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, FLS_CHK_MEM_DESTINATION, p_job->expansion_service);
        }
        else
        {
            err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits, 
                p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, FLS_CHK_MEM_SOURCE, p_job->expansion_service);
        }
        if (ER_OK == err)
        {
            /* Process 3 : IV data address range check */
            /* Address check of iv size */
            err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits, 
                p_ISD->prm.AES_AUTH_CIPHER.p_iv, FLS_CHK_MEM_SOURCE, p_job->expansion_service);
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
            skip_flg = FLG_ON;
        }
    }
    else
    {
        skip_flg = FLG_ON;
    }

    /* Process 4 : Range check and data generation for each mode */
    if (FLG_OFF == skip_flg)
    {
        /* GCM mode size check */
        if (((uint32_t)DATA_NOT_ZERO > p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits) 
           || (p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits > GCM_IV_MAX))
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            switch (p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits)
            {
                case TAG_SIZE_32:
                    break;
                case TAG_SIZE_64:
                    break;
                case TAG_SIZE_96:
                    break;
                case TAG_SIZE_104:
                    break;
                case TAG_SIZE_112:
                    break;
                case TAG_SIZE_120:
                    break;
                case TAG_SIZE_128:
                    break;
                default:
                    err = ER_PARAM;
                    break;
            }/* check auth_tag_size_in_bits */
            if (ER_OK == err)
            {
                /* do nothing */
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

    *p_skip_flg = skip_flg;
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0020]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0020]
* Function name: s_01_AUTH_CheckDtfProcessing
* Description  : Check DTF Processing
* Arguments    : p_job
*                set_resource_id
*                p_auth_tag
*                p_cmp_target
*                cmp_size
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_PENDING
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR2
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_ML_CTX_ALL_USE
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                R_SALIF_ER_CRY_TIMEOUT
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
*                SERV_KEY_NOT_USABLE
*                SERV_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_CheckDtfProcessing(icum_job_t *p_job, uint32_t set_resource_id,
                                             uint8_t *p_auth_tag, uint8_t *p_cmp_target, uint16_t cmp_size)
{
    uint32_t    err;
    uint8_t     cmp_result;

    err = ER_OK;
    cmp_result = (uint8_t)NUM0;

    if (NULL != p_job->prm.job_aes_auth.get_context)
    {
        err = ((ml_ctx_aes_context_t*)p_job->prm.job_aes_auth.get_context)->result;
        if (ER_OK == err)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
            if ((ER_OK == err) &&
               ((uint32_t)D_AES_DIR_DECRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction))
            {
                cmp_result = L_Toolbox_MemCmp(p_auth_tag, p_cmp_target, (int16_t)cmp_size);
                if ((uint32_t)NUM0 == (uint32_t)cmp_result)
                {
                    p_job->p_ISD->prm.AES_AUTH_CIPHER.verification_result = VERIFICATION_PASS;
                }
                else
                {
                    p_job->p_ISD->prm.AES_AUTH_CIPHER.verification_result = VERIFICATION_FAIL;
                }
            }
            else
            {
                /* do nothing */
            }
        }
        else if (err == ER_IN_PROCESS)
        {
            /* Do nothing */
        }
        else
        {
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
        } /* end of else */
    } /* end of if (NULL != p_job->prm.job_aes_auth.get_context) */
    else
    {
        (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
        err = (uint32_t)SERV_SEQUENCE_ERROR;
    } /* end of else */

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0003]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0003]
* Function name: s_01_AUTH_AtOnceAesGcm
* Description  : AES_GCM All_at_Once
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_PENDING
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR2
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_ML_CTX_ALL_USE
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                R_SALIF_ER_CRY_TIMEOUT
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
*                SERV_KEY_NOT_USABLE
***************************************************************************************/
static uint32_t s_01_AUTH_AtOnceAesGcm(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    aes_all_at_once_gcm_t at_once_param;
    ml_ctx_aes_context_t  *p_ctx;
    uint32_t saved_psw = 0UL;

    s_01_com_key_param_t KeyParam1;
    static uint32_t auth_tag[NUM4];
    aes_key_t *p_key;
    uint32_t err;
    uint32_t get_resource_id;
    static uint32_t set_resource_id;
    uint32_t skip_flg;
    uint8_t cmp_result;
    uint32_t src_logical;
    uint32_t dest_logical;
    uint32_t iv_logical;
    uint32_t auth_logical;
    uint32_t tag_logical;
    uint16_t cmp_size;

    get_resource_id = DATA_ZERO;
    p_key = NULL;
    err = ER_OK;
    skip_flg = FLG_OFF;

    L_Toolbox_MemSet((uint8_t *)&at_once_param, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_gcm_t));

    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;

    /* Check DTF processing */
    if (SERV_IN_PROCESS == p_ISD->service_result)
    {
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag,
                &tag_logical);
        cmp_size = (uint16_t)((uint32_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits /
                              AES_BYTE_SIZE_IN_BITS);
        err = s_01_AUTH_CheckDtfProcessing(p_job, set_resource_id,
                                           (uint8_t *)auth_tag, (uint8_t *)tag_logical, cmp_size);
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        err = s_01_AUTH_CheckRange(p_job, &skip_flg);
    }
    else
    {
        /* do nothing */
    }

    /* Process 5 : Key data acquisition Key acquisition */
    if (FLG_OFF == skip_flg)
    {
        /* Get AES Key */
        KeyParam1.key_group      = p_ISD->prm.AES_AUTH_CIPHER.key_group;
        KeyParam1.key_id         = p_ISD->prm.AES_AUTH_CIPHER.key_id; 
        KeyParam1.NotUseFlag     = (uint8_t)DATA_ZERO;
        KeyParam1.direction      = p_job->prm.job_aes_auth.direction;
        KeyParam1.p_aeskey       = &p_key;

        err = s_01_CIPHER_GetAESKey(p_job, &KeyParam1);
        if (ER_OK == err)
        {
            /* Get Key success. */
            p_job->prm.job_aes_auth.get_key = RESOURCES_HOLD;
        }
        else
        {
            /*  Error Occurred */
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }

    /* Process 6 : Get Context */
    if (FLG_OFF == skip_flg)
    {
        err = ML_CTX_Get(CTX_TYPE_AES, (uint32_t**)&p_job->prm.job_aes_auth.get_context);
        p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes_auth.get_context;
        if (ER_OK == err)
        {
            /* Process 8 : Input,output memory check */
            err = s_01_AUTH_InitSrcDestGcm(p_job);
            if (ER_OK == err)
            {
                /* Process 7 :Get ABTR  */
                err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &get_resource_id);
                if (ER_OK == err)
                {
                    ((ml_ctx_aes_context_t*)p_job->prm.job_aes_auth.get_context)->result = ER_D_AES_INIT_ERROR;
                    set_resource_id = get_resource_id;
                }
                else
                {
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
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }

    /* Process 9 : Parameter settings */
    if (FLG_OFF == skip_flg)
    {
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_AUTH_CIPHER.p_data_in, &src_logical);
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_AUTH_CIPHER.p_data_out, &dest_logical);
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_AUTH_CIPHER.p_iv, &iv_logical);
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_AUTH_CIPHER.p_auth_data, &auth_logical);
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, &tag_logical);
        if (NULL != p_ctx)
        {
            /* In case of asynchronous processing, store the Context address of the result notification destination */
            g_ContextAddress = (uint32_t)p_ctx;
            /* If Context is specified and All processing is specified, specify asynchronous */
            at_once_param.async = D_AES_TRANS_ASYNC;
        }
        else
        {
            /* In case of synchronous processing, Used to initialize and store results. */
            g_ContextAddress = AES_STATE_INIT;
            at_once_param.async = D_AES_TRANS_SYNC;
        }
         at_once_param.direction  = (uint8_t)p_job->prm.job_aes_auth.direction;
        at_once_param.p_aes_key  = (uint32_t *)p_key->value;
        at_once_param.key_length = p_key->properties.size_in_bytes;
        at_once_param.p_aes_iv   = (uint32_t *)iv_logical;
        at_once_param.iv_size_in_bits = p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits;
        at_once_param.p_auth_data     = (uint32_t *)auth_logical;
        at_once_param.auth_data_size_in_bits = p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits;
        at_once_param.p_aes_src              = (uint32_t*)src_logical;
        at_once_param.data_size_in_bits      = p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits;
        at_once_param.auth_tag_size_in_bits  = p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits;
        at_once_param.p_aes_dest             = (uint32_t*)dest_logical;
        at_once_param.p_context              = p_ctx->context;
        if((uint32_t)D_AES_DIR_DECRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction)
        {
            at_once_param.p_auth_tag = auth_tag;
        }
        else
        {
            at_once_param.p_auth_tag = (uint32_t *)tag_logical;
        }
        /* Process 10 : Call AES_GCM All_at_Once */
        err = D_AES_ExecutionAtOnceGcm(&at_once_param);
        if (NULL != p_ctx)
        {
            /* In case of asynchronous processing, store the operation result in Context */
            /* disable interrupts (if not already disabled) */
            DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

            if (ER_D_AES_INIT_ERROR == p_ctx->result)
            {
                p_ctx->result = err;
            }
            else
            {
                /* do nothing */
            }

            /* re-enable interrupts (if necessary) */
            ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

            err = ER_OK;
        }
        else
        {
            /* In case of asynchronous processing, check if an error is set by interrupt. */
            if (AES_STATE_INIT == g_ContextAddress)
            {
                /* Do nothing */
            }
            else
            {
                if ( ER_OK == err)
                {
                    err = g_ContextAddress;
                }
                else
                {
                    /* Do nothing */
                }
            }
        }

        err = p_ctx->result;

        if (ER_OK == err)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
            if (ER_OK == err)
            {
                if((uint32_t)D_AES_DIR_DECRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction)
                {
                    cmp_size = (uint16_t)(p_job->p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits /
                        (uint32_t)AES_BYTE_SIZE_IN_BITS);
                    cmp_result = L_Toolbox_MemCmp((uint8_t*)auth_tag, (uint8_t*)tag_logical, (int16_t)cmp_size);
                    if(0UL == (uint32_t)cmp_result)
                    {
                        p_job->p_ISD->prm.AES_AUTH_CIPHER.verification_result = VERIFICATION_PASS;
                    }
                    else
                    {
                        p_job->p_ISD->prm.AES_AUTH_CIPHER.verification_result = VERIFICATION_FAIL;
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
        }
        else if (ER_IN_PROCESS == err)
        {
            /* Do nothing */
        }
        else
        {
            /* Prioritize L_CRY_UpdateAes error,do not get ML_ABTR_FreeResource error */
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
        }
    }
    else
    {
        /* do nothing */
    }

    /* Process 11 : return err */
    return (err);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0002]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0002]
* Function name: S_01_AuthenticateAes
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
static uint32_t s_01_AUTH_Gcm(icum_job_t *p_job, uint8_t Local_job_cycle)
{
    r_icumif_isd_t *p_ISD;
    uint32_t err = (uint32_t)SERV_OK;
    uint32_t skip_flg = FLG_OFF;

    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;

    /* Process 1 : job_slice check */
    if (((uint8_t)JOB_ALL_AT_ONCE == p_ISD->prm.AES_AUTH_CIPHER.job_cycle)
        && ((uint32_t)p_ISD->prm.AES_AUTH_CIPHER.job_slice == 0uL))
    {
        /* GCM All At Once */
        err = s_01_AUTH_AtOnceAesGcm(p_job);
    }
    else
    {
        /* Process 2 : Implementation of INIT processing */
        /* service just triggered? */
        if((p_ISD->service_result == SERV_REQUEST) || (p_ISD->service_result == SERV_PENDING))
        {
            if((Local_job_cycle & (uint8_t)JOB_INIT) != 0u)
            {
                /* initialize the job */
                err = s_01_AUTH_InitAesCipherGcm(p_job);
                if(err != ER_OK)
                {
                    skip_flg = FLG_ON;
                }
                else
                {
                    /* When job_cycle is init only, the processing is end */
                    if((Local_job_cycle & ((uint8_t)JOB_UPDATE | (uint8_t)JOB_FINISH)) == 0u)
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
            /* Process 3 : UPDATE, FINISH preparation process */
            if(FLG_OFF == skip_flg)
            {
                /* updating? */
                if((Local_job_cycle & ((uint8_t)JOB_UPDATE | (uint8_t)JOB_FINISH)) != 0u)
                {
                    err = s_01_AUTH_InitSrcDestGcm(p_job);
                    if(err != ER_OK)
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
        /* Process 4 : Implementation of UPDATE and FINISH processing */
        if(FLG_OFF == skip_flg)
        {
            if((Local_job_cycle & ((uint8_t)JOB_UPDATE | (uint8_t)JOB_FINISH)) != 0u)
            {
                err = s_01_AUTH_ExecutionProcessGcm(p_job, Local_job_cycle);
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
    /* Process 5 : return err */
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_UnifiedAPI_C2_0041]:[ICUMFW_UD_UnifiedAPI_C2_0041]
* Function name: s_01_AUTH_CcmMakeB0
* Description  : AES CCM mode calculation.
* Arguments    : p_ccm
*              : p_ccm_B0
*              : p_iv_data
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t s_01_AUTH_CcmMakeB0(aes_all_at_once_ccm_t *p_ccm, uint8_t* p_ccm_B0, uint8_t* p_iv_data)
{
    uint32_t err;
    uint32_t local_size;
    uint8_t  sift;

    err        = ER_OK;
    local_size = (uint32_t)DATA_ZERO;
    sift       = DATA_ZERO;

    if ((NULL != p_ccm_B0)
        && (NULL != p_iv_data))
    {
        L_Toolbox_MemSet((uint8_t*)p_ccm_B0, 0x00u, (int16_t)AES_BLOCK_SIZE_IN_BYTES);
        switch (p_ccm->iv_size_in_bits)
        {
            case 56U:
                p_ccm_B0[5] = (uint8_t)((uint32_t)D_AES_NLEN_BIT_56 << AES_BIT_SIFT4);
                break;
            case 64U:
                p_ccm_B0[5] = (uint8_t)((uint32_t)D_AES_NLEN_BIT_64 << AES_BIT_SIFT4);
                break;
            case 72U:
                p_ccm_B0[5] = (uint8_t)((uint32_t)D_AES_NLEN_BIT_72 << AES_BIT_SIFT4);
                break;
            case 80U:
                p_ccm_B0[5] = (uint8_t)((uint32_t)D_AES_NLEN_BIT_80 << AES_BIT_SIFT4);
                break;
            case 88U:
                p_ccm_B0[5] = (uint8_t)((uint32_t)D_AES_NLEN_BIT_88 << AES_BIT_SIFT4);
                break;
            case 96U:
                p_ccm_B0[5] = (uint8_t)((uint32_t)D_AES_NLEN_BIT_96 << AES_BIT_SIFT4);
                break;
            case 104U:
                p_ccm_B0[5] = (uint8_t)((uint32_t)D_AES_NLEN_BIT_104 << AES_BIT_SIFT4);
                break;
            default:
                err = ER_PARAM;
                break;
        }

        if (ER_OK == err)
        {
            switch (p_ccm->auth_tag_size_in_bits)
            {
                case 32u:
                    p_ccm_B0[5] = (uint8_t)((uint32_t)p_ccm_B0[5] | (uint32_t)D_AES_TLEN_BIT_32);
                    break;
                case 48u:
                    p_ccm_B0[5] = (uint8_t)((uint32_t)p_ccm_B0[5] | (uint32_t)D_AES_TLEN_BIT_48);
                    break;
                case 64u:
                    p_ccm_B0[5] = (uint8_t)((uint32_t)p_ccm_B0[5] | (uint32_t)D_AES_TLEN_BIT_64);
                    break;
                case 80u:
                    p_ccm_B0[5] = (uint8_t)((uint32_t)p_ccm_B0[5] | (uint32_t)D_AES_TLEN_BIT_80);
                    break;
                case 96u:
                    p_ccm_B0[5] = (uint8_t)((uint32_t)p_ccm_B0[5] | (uint32_t)D_AES_TLEN_BIT_96);
                    break;
                case 112u:
                    p_ccm_B0[5] = (uint8_t)((uint32_t)p_ccm_B0[5] | (uint32_t)D_AES_TLEN_BIT_112);
                    break;
                case 128u:
                    p_ccm_B0[5] = (uint8_t)((uint32_t)p_ccm_B0[5] | (uint32_t)D_AES_TLEN_BIT_128);
                    break;
                default:
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
            local_size   = p_ccm->data_size_in_bits;
            p_ccm_B0[15] = (uint8_t)(local_size & 0xffUL); local_size = local_size >> AES_BIT_SIFT8;
            p_ccm_B0[14] = (uint8_t)(local_size & 0xffUL); local_size = local_size >> AES_BIT_SIFT8;
            p_ccm_B0[13] = (uint8_t)(local_size & 0xffUL); local_size = local_size >> AES_BIT_SIFT8;
            p_ccm_B0[12] = (uint8_t)(local_size & 0xffUL);

            local_size  = p_ccm->auth_data_size_in_bits;
            p_ccm_B0[3] = (uint8_t)(local_size & 0xffUL); local_size = local_size >> AES_BIT_SIFT8;
            p_ccm_B0[2] = (uint8_t)(local_size & 0xffUL); local_size = local_size >> AES_BIT_SIFT8;
            p_ccm_B0[1] = (uint8_t)(local_size & 0x07UL);

            L_Toolbox_MemSet((uint8_t*)p_iv_data, 0x00u, (int16_t)AES_BLOCK_SIZE_IN_BYTES);
            sift = (uint8_t)((uint32_t)AES_BLOCK_SIZE_IN_BYTES
                                - (p_ccm->iv_size_in_bits / (uint32_t)AES_BYTE_SIZE_IN_BITS));
            L_Toolbox_MemCpy(&p_iv_data[sift],
                    (const uint8_t *)p_ccm->p_aes_iv,
                    (int16_t)(p_ccm->iv_size_in_bits / (uint32_t)AES_BYTE_SIZE_IN_BITS));
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        err = ER_PARAM;
    }
    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0011]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0011]
* Function name: s_01_CIPHER_InitAesCipher
* Description  : Initialization of the ciphering operation
* Arguments    : p_job
* Return Value : SERV_OK(ER_OK)
*                ER_PARAM
*                ER_PENDING
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
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
static uint32_t s_01_AUTH_InitAesCipherCcm(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    ml_ctx_aes_context_t  *p_ctx;
    aes_init_ccm_t        init_param;
    aes_all_at_once_ccm_t ccm_prm;
    uint8_t ccm_B0[AES_BLOCK_SIZE_IN_BYTES];
    uint8_t iv_data[AES_BLOCK_SIZE_IN_BYTES];
    s_01_com_key_param_t KeyParam1;
    aes_key_t *p_key = NULL;
    uint32_t err = ER_OK;
    uint32_t resource_id;
    uint32_t skip_flg = FLG_OFF;
    uint32_t iv_logical;

    L_Toolbox_MemSet((uint8_t *)&init_param, DATA_ZERO, (int16_t)sizeof(aes_init_ccm_t));
    L_Toolbox_MemSet((uint8_t *)ccm_B0, DATA_ZERO, (int16_t)sizeof(ccm_B0));
    L_Toolbox_MemSet((uint8_t *)iv_data, DATA_ZERO, (int16_t)sizeof(iv_data));
    L_Toolbox_MemSet((uint8_t *)&ccm_prm, (uint8_t)DATA_ZERO, (int16_t)sizeof(aes_all_at_once_ccm_t));

    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;

    /* Check range direction */
    switch(p_ISD->prm.AES_AUTH_CIPHER.direction)
    {
        case CIPHER_DIR_ENCRYPTION:
            p_job->prm.job_aes_auth.direction = D_AES_DIR_ENCRYPTION;
            break;
        case CIPHER_DIR_DECRYPTION:
            p_job->prm.job_aes_auth.direction = D_AES_DIR_DECRYPTION;
            break;
        default:
            err = ER_PARAM;
            break;
    }
    if(err != (uint32_t)SERV_OK)
    {
        skip_flg = FLG_ON;
    }
    else
    {
        /* do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* Address check of auth_tag size */
        if(p_ISD->prm.AES_AUTH_CIPHER.direction == CIPHER_DIR_ENCRYPTION)
        {
            err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits
                                            , p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, FLS_CHK_MEM_DESTINATION
                                            , p_job->expansion_service);
        }
        else
        {
            err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits
                                            , p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag
                                            , FLS_CHK_MEM_SOURCE, p_job->expansion_service);
        }
        if(err == ER_OK)
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
        /* Address check of iv size */
        err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits
                                        , p_ISD->prm.AES_AUTH_CIPHER.p_iv
                                        , FLS_CHK_MEM_SOURCE, p_job->expansion_service);
        if(err == ER_OK)
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
        /* Get AES Key */
        KeyParam1.key_group      = p_ISD->prm.AES_AUTH_CIPHER.key_group;
        KeyParam1.key_id         = p_ISD->prm.AES_AUTH_CIPHER.key_id;
        KeyParam1.NotUseFlag     = 0U;
        KeyParam1.direction      = p_job->prm.job_aes_auth.direction;
        KeyParam1.p_aeskey       = &p_key;

        err = s_01_CIPHER_GetAESKey(p_job, &KeyParam1);
        if(err == ER_OK)
        {
            /* Get Key success. */
            p_job->prm.job_aes_auth.get_key = RESOURCES_HOLD;
        }
        else
        {
            /*  Error Occurred */
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        err = ML_CTX_Get(CTX_TYPE_AES, (uint32_t**)&p_job->prm.job_aes_auth.get_context);
        p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes_auth.get_context;
        if(err != ER_OK)
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
    if (FLG_OFF == skip_flg)
    {
        err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
        if(ER_OK != err)
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
    if (FLG_OFF == skip_flg)
    {
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_AUTH_CIPHER.p_iv, &iv_logical);
        /* Case: CCM Mode */
        if ((uint32_t)D_AES_DIR_DECRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction)
        {
            p_job->prm.job_aes_auth.p_ccm_p_add = p_ISD->prm.AES_AUTH_CIPHER.p_data_out;
            p_job->prm.job_aes_auth.nb_ccm_p_blocks = p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits;
        }
        else
        {
            /* do nothing */
        }

        g_ContextAddress = AES_STATE_INIT;
        if (NULL != p_ctx)
        {
            if (NUM0 != p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits)
            {
                ccm_prm.data_size_in_bits      = p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits;
                ccm_prm.auth_data_size_in_bits = p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits;
                ccm_prm.iv_size_in_bits = p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits;
                ccm_prm.auth_tag_size_in_bits = p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits;
                ccm_prm.p_aes_iv = (uint32_t*)iv_logical;

                err = s_01_AUTH_CcmMakeB0(&ccm_prm, ccm_B0, iv_data);
                if ( err != ER_OK )
                {
                    /* Do Nothing */
                }
                else
                {
                    if (NUM0 == p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits)
                    {
                        p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info | (uint32_t)D_AES_BLOCK_INFO_CCM_PLEN0);
                    }
                    else
                    {
                        /* do Nothing*/
                    }
                    g_ContextAddress       = AES_STATE_INIT;
                    init_param.direction   = (uint8_t)p_job->prm.job_aes_auth.direction;
                    init_param.p_size_data = (uint32_t*)ccm_B0;
                    init_param.p_aes_key   = (uint32_t *)p_key->value;
                    init_param.key_length  = p_key->properties.size_in_bytes;
                    init_param.p_aes_iv    = (uint32_t*)iv_data;
                    init_param.p_context   = p_ctx->context;
                    err = D_AES_InitJobCcm(&init_param);
                    
                    if ((uint32_t)D_AES_DIR_ENCRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction)
                    {
                        p_ctx->auth_step = D_AES_CCM_STEP_A;
                    }
                    else
                    {
                        p_ctx->auth_step = D_AES_CCM_STEP_C;
                    }
                }
            }
            else
            {
                err = ER_PARAM;
            }
        }
        else
        {
            err = ER_PARAM;
        }

        if (ER_OK == err)
        {
            p_job->prm.job_aes_auth.ccm_cancel = 1u;
        }
        else
        {
            /* do nothing */
        }
        (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
    }
    else
    {
        /* do nothing */
    }
    return (err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0012]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0012]
* Function name: s_01_CIPHER_InitSrcDest
* Description  : Set number and address for Update process
* Arguments    : p_job
* Return Value : SERV_OK(ER_OK)
*                ER_PARAM
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
*                SERV_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_InitSrcDestCcm(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    uint32_t err = (uint32_t)SERV_OK;
    uint32_t skip_flg = FLG_OFF;

    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;

    /* When context is 0, it means that init processing has not been executed yet */
    if(p_job->prm.job_aes_auth.get_context == NULL)
    {
        err = (uint32_t)SERV_SEQUENCE_ERROR;
        skip_flg = FLG_ON;
    }
    else
    {
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        p_job->prm.job_aes_auth.p_source     = p_ISD->prm.AES_AUTH_CIPHER.p_data_in;
        p_job->prm.job_aes_auth.p_auth_data  = p_ISD->prm.AES_AUTH_CIPHER.p_auth_data;
        p_job->prm.job_aes_auth.p_dest       = p_ISD->prm.AES_AUTH_CIPHER.p_data_out;
        /* get the job slice configured */
        p_job->prm.job_aes_auth.nb_slice = p_ISD->prm.AES_AUTH_CIPHER.job_slice;
        if (p_ISD->prm.AES_AUTH_CIPHER.direction == CIPHER_DIR_ENCRYPTION)
        {
            /* Calculation of block number of cipher */
            err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits
                                        , p_job->prm.job_aes_auth.p_source
                                        , FLS_CHK_MEM_SOURCE, p_job->expansion_service);
            if( err == ER_OK)
            {
                err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits
                                            , p_job->prm.job_aes_auth.p_dest
                                            , FLS_CHK_MEM_DESTINATION, p_job->expansion_service);
                if( err == ER_OK )
                {
                    p_job->prm.job_aes_auth.src_size_bits = p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits;
                }
                else
                {
                    skip_flg = FLG_ON;
                }
            }
            else
            {
                skip_flg = FLG_ON;
            }

            if (FLG_OFF == skip_flg)
            {
                /* Calculation of block number of AAD */
                if (0UL == (uint32_t)p_job->prm.job_aes_auth.not_reverse)
                {
                    err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits
                                                , p_job->prm.job_aes_auth.p_auth_data
                                                , FLS_CHK_MEM_SOURCE, p_job->expansion_service);
                    if( err == ER_OK )
                    {
                        p_job->prm.job_aes_auth.auth_size_bits = p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits;
                    }
                    else
                    {
                        skip_flg = FLG_ON;
                    }
                }
                else
                {
                    /* Ignore as it is invalid data */
                    p_job->prm.job_aes_auth.auth_size_bits     = 0U;
                }
            } /* end of if (FLG_OFF == skip_flg) */
            else
            {
                /* do nothing */
            }
        }
        else
        {
            err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits
                                        , p_job->prm.job_aes_auth.p_auth_data
                                        , FLS_CHK_MEM_SOURCE, p_job->expansion_service);
            if( err == ER_OK )
            {
                p_job->prm.job_aes_auth.auth_size_bits     = p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits;
            }
            else
            {
                skip_flg = FLG_ON;
            }

            if (FLG_OFF == skip_flg)
            {
                /* Calculation of block number of AAD */
                if (0UL == (uint32_t)p_job->prm.job_aes_auth.not_reverse)
                {
                    /* Calculation of block number of cipher */
                    err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits
                                                , p_job->prm.job_aes_auth.p_source
                                                , FLS_CHK_MEM_SOURCE, p_job->expansion_service);
                    if( err == ER_OK)
                    {
                        err = s_01_AUTH_CheckMemory(p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits
                                                    , p_job->prm.job_aes_auth.p_dest
                                                    , FLS_CHK_MEM_DESTINATION, p_job->expansion_service);
                        if( err == ER_OK )
                        {
                            p_job->prm.job_aes_auth.src_size_bits = p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits;
                        }
                        else
                        {
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
                    /* Ignore as it is invalid data */
                    p_job->prm.job_aes_auth.src_size_bits = 0U;
                }
            } /* end of if (FLG_OFF == skip_flg) */
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
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_UnifiedAPI_C2_0048]:[ICUMFW_UD_UnifiedAPI_C2_0048]
* Function name: s_01_AUTH_CcmUpdateStepA
* Description  : CCM mode Update(Auth Data) conversion process
* Arguments    : p_cipher (Cipher operation parameter)
*              : p_ctx (Context data)
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
uint32_t s_01_AUTH_CcmUpdateStepA(aes_all_at_once_ccm_t *p_cipher, ml_ctx_aes_context_t *p_ctx)
{
    aes_update_ccm_t update_param;
    uint32_t         err;
    
    L_Toolbox_MemSet((uint8_t *)&update_param, DATA_ZERO, (int16_t)sizeof(aes_update_ccm_t));
    err = ER_OK;

    if (NUM0 != p_cipher->auth_data_size_in_bits)
    {
        if (NUM0 != (p_cipher->auth_data_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS))
        {
            p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info | (uint32_t)D_AES_BLOCK_INFO_LAST);
            p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info 
                                            | (p_cipher->auth_data_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS));
        }
        else
        {
            /* do nothing */
        }
        update_param.number = ((p_cipher->auth_data_size_in_bits 
                            + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1)) / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
        update_param.block_info = p_ctx->block_info;
        update_param.step       = p_ctx->auth_step;
        update_param.p_aes_src  = p_cipher->p_auth_data;
        update_param.p_context  = p_ctx->context;
        update_param.p_aes_dest = (uint32_t*)p_cipher->p_aes_dest;
        err = D_AES_UpdateJobCcm(&update_param);
        p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info & (uint32_t)(~D_AES_BLOCK_INFO_LAST));
        p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info & (uint32_t)(~D_AES_BLOCK_INFO_SIZE));
    }
    else
    {
        if ((uint32_t)D_AES_DIR_ENCRYPTION == (uint32_t)p_cipher->direction)
        {
            p_ctx->auth_step = D_AES_CCM_STEP_P;
        }
        else
        {
            /* do nothing */
        }
    }
    return err;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_UnifiedAPI_C2_0049]:[ICUMFW_UD_UnifiedAPI_C2_0049]
* Function name: s_01_AUTH_CcmUpdateStepCP
* Description  : CCM mode Update(Cipher Data) conversion process
* Arguments    : p_cipher (Cipher operation parameter)
*              : p_ctx (Context data)
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
uint32_t s_01_AUTH_CcmUpdateStepCP(aes_all_at_once_ccm_t *p_cipher, ml_ctx_aes_context_t *p_ctx)
{
    aes_update_ccm_t update_param;
    uint32_t         err;
    
    L_Toolbox_MemSet((uint8_t *)&update_param, DATA_ZERO, (int16_t)sizeof(aes_update_ccm_t));
    err = ER_OK;

    if (NUM0 != p_cipher->data_size_in_bits)
    {
        if (NUM0 != (p_cipher->data_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS))
        {
            p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info | (uint32_t)D_AES_BLOCK_INFO_LAST);
            p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info 
                                            | (p_cipher->data_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS));
        }
        else
        {
            /* do nothing */
        }
        update_param.number = ((p_cipher->data_size_in_bits 
                            + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1)) / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
        update_param.block_info = p_ctx->block_info;
        update_param.step       = p_ctx->auth_step;
        update_param.p_aes_src  = (uint32_t*)p_cipher->p_aes_src;
        update_param.p_context  = p_ctx->context;
        update_param.p_aes_dest = (uint32_t*)p_cipher->p_aes_dest;
        err = D_AES_UpdateJobCcm(&update_param);
        p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info & (uint32_t)(~D_AES_BLOCK_INFO_LAST));
        p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info & (uint32_t)(~D_AES_BLOCK_INFO_SIZE));
    }
    else
    {
        if ((uint32_t)D_AES_DIR_DECRYPTION == (uint32_t)p_cipher->direction)
        {
            p_ctx->auth_step = D_AES_CCM_STEP_A;
        }
        else
        {
            /* do nothing */
        }
    }
    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0013]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0013]
* Function name: s_01_AUTH_UpdateStepCcm
* Description  : Processing update Cipher(CCM)
* Arguments    : p_cipher
*                p_cipher
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_UpdateStepCcm(aes_all_at_once_ccm_t *p_cipher, ml_ctx_aes_context_t *p_ctx)
{
    uint32_t err;
    
    err = ER_OK;

    if ((uint32_t)D_AES_DIR_DECRYPTION == (uint32_t)p_cipher->direction)
    {
                    if ((uint32_t)D_AES_CCM_STEP_C == (uint32_t)p_ctx->auth_step)
        {
            err = s_01_AUTH_CcmUpdateStepCP(p_cipher, p_ctx);
        }
        else
        {
            /* do nothing */
        }

        if ((uint32_t)D_AES_CCM_STEP_A == (uint32_t)p_ctx->auth_step)
        {
            err = s_01_AUTH_CcmUpdateStepA(p_cipher, p_ctx);
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        if ((uint32_t)D_AES_CCM_STEP_A == (uint32_t)p_ctx->auth_step)
        {
            err = s_01_AUTH_CcmUpdateStepA(p_cipher, p_ctx);
        }
        else
        {
            /* do nothing */
        }

        if ((uint32_t)D_AES_CCM_STEP_P == (uint32_t)p_ctx->auth_step)
        {
            err = s_01_AUTH_CcmUpdateStepCP(p_cipher, p_ctx);
        }
        else
        {
            /* do nothing */
        }
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0022]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0022]
* Function name: s_01_AUTH_ProcessUpdateCcmEnd
* Description  : Processing update Cipher(CCM) End process
* Arguments    : p_job
*                step
*                blocks
*                finish
*                job_cycle
* Return Value : SERV_OK
*                SERV_IN_PROCESS
***************************************************************************************/
static uint32_t s_01_AUTH_ProcessUpdateCcmEnd(icum_job_t *p_job, uint32_t step, uint32_t blocks,
                                              uint32_t finish, uint8_t job_cycle)
{
    uint32_t err;

    err = ER_OK;
    if ((uint32_t)JOB_COMPLETE == finish)
    {
        if ((uint32_t)D_AES_CCM_STEP_A == step)
        {
            p_job->prm.job_aes_auth.auth_size_bits = NUM0;
            if ((uint32_t)D_AES_DIR_ENCRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction)
            {
                if (((uint32_t)NUM0 == p_job->prm.job_aes_auth.src_size_bits)
                    && ((uint32_t)NUM0 == ((uint32_t)job_cycle & (uint32_t)JOB_FINISH)))
                {
                    /* End processing (there is no next processing) */
                    err = (uint32_t)SERV_OK;
                }
                else
                {
                    err = (uint32_t)SERV_IN_PROCESS;
                }
            }
            else
            {
                if ((uint32_t)NUM0 == ((uint32_t)job_cycle & (uint32_t)JOB_FINISH))
                {
                    /* End processing (there is no next processing) */
                    err = (uint32_t)SERV_OK;
                }
                else
                {
                    err = (uint32_t)SERV_IN_PROCESS;
                }
            }
        } /* end of if ((uint32_t)D_AES_CCM_STEP_A == step) */
        /* ENC + STEP_P or DEC + STEP_C */
        else
        {
            p_job->prm.job_aes_auth.src_size_bits = (uint32_t)NUM0;
            /* ENC + STEP_P */
            if ((uint32_t)D_AES_DIR_ENCRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction)
            {
                if ((uint32_t)NUM0 == ((uint32_t)job_cycle & (uint32_t)JOB_FINISH))
                {
                    /* End processing (there is no next processing) */
                    err = (uint32_t)SERV_OK;
                }
                else
                {
                    err = (uint32_t)SERV_IN_PROCESS;
                }
            }
            /* DEC + STEP_C */
            else
            {
                if (((uint32_t)NUM0 == p_job->prm.job_aes_auth.auth_size_bits)
                        && ((uint32_t)NUM0 ==((uint32_t)job_cycle & (uint32_t)JOB_FINISH)))
                {
                    /* End processing (there is no next processing) */
                    err = (uint32_t)SERV_OK;
                }
                else
                {
                    err = (uint32_t)SERV_IN_PROCESS;
                }
            }
        } /* end of else */
    } /* end of if ((uint32_t)JOB_COMPLETE == finish) */
    else
    {
         if ((uint32_t)D_AES_CCM_STEP_A == (uint32_t)step)
        {
            p_job->prm.job_aes_auth.auth_size_bits -= blocks;
            p_job->prm.job_aes_auth.p_auth_data    += (blocks / NUM32);
        }
        /* ENC + STEP_P or DEC + STEP_C */
        else
        {
            p_job->prm.job_aes_auth.src_size_bits -= blocks;
            p_job->prm.job_aes_auth.p_source  += (blocks / NUM32);
            p_job->prm.job_aes_auth.p_dest    += (blocks / NUM32);
        }
        /* not finished */
        err = (uint32_t)SERV_IN_PROCESS;
    } /* end of else */

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0013]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0013]
* Function name: s_01_AUTH_ProcessUpdateCcm
* Description  : Processing update Cipher(CCM)
* Arguments    : p_job
*                step
*                blocks
*                finish
*                job_cycle
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_ProcessUpdateCcm(icum_job_t *p_job, uint8_t step, uint32_t blocks,
                                           uint16_t finish, uint8_t job_cycle)
{
    aes_all_at_once_ccm_t ccm_prm;
    ml_ctx_aes_context_t  *p_ctx;
    uint32_t err;
    uint32_t resource_id;
    uint32_t cipher_data[AES_BLOCK_SIZE];
    uint32_t skip_flg;
    uint32_t src_logical;
    uint32_t dest_logical;
    uint32_t auth_logical;

    resource_id = 0u;
    skip_flg = FLG_OFF;

    p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes_auth.get_context;
    L_Toolbox_MemSet((uint8_t *)&ccm_prm, (uint8_t)DATA_ZERO, (int16_t)sizeof(aes_all_at_once_ccm_t));

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
        if ((uint32_t)D_AES_CCM_STEP_A == (uint32_t)step )
        {
            if (NULL != p_ctx)
            {
                (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes_auth.p_auth_data, &auth_logical);
                ccm_prm.p_context = p_ctx->context;
                ccm_prm.auth_data_size_in_bits = blocks;
                ccm_prm.p_auth_data = (uint32_t*)auth_logical;
                ccm_prm.direction   = p_job->prm.job_aes_auth.direction;
                ccm_prm.p_aes_dest  = NULL;
                g_ContextAddress = AES_STATE_INIT;
                err = s_01_AUTH_UpdateStepCcm(&ccm_prm, p_ctx);
            }
            else
            {
                err = ER_PARAM;
            }
        }
        else if (((uint32_t)D_AES_DIR_ENCRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction
                && (uint32_t)D_AES_CCM_STEP_P == (uint32_t)step)
                || ((uint32_t)D_AES_DIR_DECRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction
                        && (uint32_t)D_AES_CCM_STEP_C == (uint32_t)step))
        {
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes_auth.p_source, &src_logical);
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes_auth.p_dest, &dest_logical);
            if ((uint32_t)finish == (uint32_t)JOB_COMPLETE)
            {
                if (0UL == (blocks % (uint32_t)SIZE_IN_BITS))
                {
                    if (NULL != p_ctx)
                    {
                        ccm_prm.p_context = p_ctx->context;
                        ccm_prm.data_size_in_bits = blocks;
                        ccm_prm.p_aes_src  = (uint32_t*)src_logical;
                        ccm_prm.p_aes_dest = (uint32_t*)dest_logical;
                        ccm_prm.direction  = p_job->prm.job_aes_auth.direction;
                        g_ContextAddress = AES_STATE_INIT;
                        err = s_01_AUTH_UpdateStepCcm(&ccm_prm, p_ctx);
                    }
                    else
                    {
                        err = ER_PARAM;
                    }
                }
                else
                {
                    /* If there is a fraction, adjust the output size of the final block */
                    if (blocks > 128UL)
                    {
                        /* Calculate with the Update function until the remaining 1 blocks */
                        if (NULL != p_ctx)
                        {
                            ccm_prm.p_context         = p_ctx->context;
                            ccm_prm.data_size_in_bits = ((blocks / (uint32_t)SIZE_IN_BITS) * (uint32_t)SIZE_IN_BITS);
                            ccm_prm.p_aes_src  = (uint32_t*)src_logical;
                            ccm_prm.p_aes_dest = (uint32_t*)dest_logical;
                            ccm_prm.direction  = p_job->prm.job_aes_auth.direction;
                            g_ContextAddress = AES_STATE_INIT;
                            err = s_01_AUTH_UpdateStepCcm(&ccm_prm, p_ctx);
                        }
                        else
                        {
                            err = ER_PARAM;
                        }

                        if (ER_OK == err)
                        {
                            p_job->prm.job_aes_auth.src_size_bits -= ccm_prm.data_size_in_bits;
                            p_job->prm.job_aes_auth.p_source  += (ccm_prm.data_size_in_bits / 32UL);
                            p_job->prm.job_aes_auth.p_dest    += (ccm_prm.data_size_in_bits / 32UL);
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
                        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes_auth.p_source, &src_logical);
                        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes_auth.p_dest, &dest_logical);
                        if (NULL != p_ctx)
                        {
                            ccm_prm.p_context         = p_ctx->context;
                            ccm_prm.data_size_in_bits = p_job->prm.job_aes_auth.src_size_bits;
                            ccm_prm.p_aes_src  = (uint32_t*)src_logical;
                            ccm_prm.p_aes_dest = (uint32_t*)cipher_data;
                            ccm_prm.direction  = p_job->prm.job_aes_auth.direction;
                            g_ContextAddress = AES_STATE_INIT;
                            err = s_01_AUTH_UpdateStepCcm(&ccm_prm, p_ctx);
                        }
                        else
                        {
                            err = ER_PARAM;
                        }

                        /* Output only valid sizes */
                        L_Toolbox_MemCpy((uint8_t *)dest_logical,
                                (const uint8_t *)cipher_data,
                                (int16_t)(((uint32_t)p_job->prm.job_aes_auth.src_size_bits
                                        + ((uint32_t)AES_BYTE_SIZE_IN_BITS - 1UL)) / (uint32_t)AES_BYTE_SIZE_IN_BITS));
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
            }
            else
            {
                if (NULL != p_ctx)
                {
                    ccm_prm.p_context = p_ctx->context;
                    ccm_prm.data_size_in_bits = blocks;
                    ccm_prm.p_aes_src  = (uint32_t*)src_logical;
                    ccm_prm.p_aes_dest = (uint32_t*)dest_logical;
                    ccm_prm.direction  = p_job->prm.job_aes_auth.direction;
                    g_ContextAddress = AES_STATE_INIT;
                    err = s_01_AUTH_UpdateStepCcm(&ccm_prm, p_ctx);
                }
                else
                {
                    err = ER_PARAM;
                }
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
    if (FLG_OFF == skip_flg)
    {
        /* any error? */
        if(err == ER_OK)
        {
            err = s_01_AUTH_ProcessUpdateCcmEnd(p_job, (uint32_t)step, blocks, (uint32_t)finish, job_cycle);
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        /* Do Nothing */
    }

    if(0UL != resource_id)
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
    else
    {
        /* do nothing */
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0014]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0014]
* Function name: s_01_AUTH_ProcessFinishCcm
* Description  : Processing finish AES Cipher(CCM)
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_ProcessFinishCcm(icum_job_t *p_job)
{
    ml_ctx_aes_context_t *p_ctx;
    aes_update_ccm_t update_param;
    aes_finish_ccm_t finish_param;
    uint32_t resource_id = 0u;
    uint32_t auth_tag[4];
    uint32_t auth_yr[4];
    uint32_t err;
    uint8_t cmp_result;
    uint32_t skip_flg = FLG_OFF;
    uint32_t tag_logical;
    uint32_t src_logical;
    uint32_t msg_size_in_bits;

    p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes_auth.get_context;
    L_Toolbox_MemSet((uint8_t *)&update_param, DATA_ZERO, (int16_t)sizeof(aes_update_ccm_t));
    L_Toolbox_MemSet((uint8_t *)&finish_param, DATA_ZERO, (int16_t)sizeof(aes_finish_ccm_t));

    /* Get ABTR Resource */
    err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
    if (err != ER_OK)
    {
        skip_flg = FLG_ON;
    }
    else
    {
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* In CCM mode, only tag generation is done by finish processing */
        if((uint32_t)D_AES_DIR_DECRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction)
        {
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, &tag_logical);
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_aes_auth.p_ccm_p_add, &src_logical);
            L_Toolbox_MemCpy((uint8_t *)auth_tag, (const uint8_t *)tag_logical,
                    (int16_t)((uint32_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits
                            / (uint32_t)AES_BYTE_SIZE_IN_BITS));
            msg_size_in_bits       = p_job->prm.job_aes_auth.nb_ccm_p_blocks;
            update_param.p_aes_src = (uint32_t*)src_logical;
            finish_param.p_auth_yr = auth_yr;
        }
        else
        {
            update_param.p_aes_src = NULL;
            finish_param.p_auth_yr = NULL;
        }

        /*  AES Finish */
        g_ContextAddress = AES_STATE_INIT;
        if (NULL != p_ctx)
        {
            if (((uint32_t)D_AES_DIR_DECRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction)
                && (NUM0 != msg_size_in_bits))
            {
                if (NUM0 != (msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS))
                {
                    p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info | (uint32_t)D_AES_BLOCK_INFO_LAST);
                    p_ctx->block_info = (uint16_t)((uint32_t)p_ctx->block_info
                                                    | (msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS));
                }
                else
                {
                    /* do nothing */
                }
                update_param.number = ((msg_size_in_bits 
                                    + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1)) / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
                update_param.block_info = p_ctx->block_info;
                update_param.step       = D_AES_CCM_STEP_P;
                update_param.p_context  = p_ctx->context;
                update_param.p_aes_dest = NULL;
                err = D_AES_UpdateJobCcm(&update_param);
            }
            else
            {
                /* do nothing */
            }
            if (ER_OK == err)
            {
                finish_param.operation  = D_AES_OPE_CONTINUE;
                finish_param.p_aes_src  = (uint32_t*)auth_tag;
                finish_param.p_context  = p_ctx->context;
                finish_param.p_auth_tag = auth_tag;
                err = D_AES_FinishJobCcm(&finish_param);
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            err = ER_PARAM;
        }

        /* any error? */
        if (ER_OK == err)
        {
            if ((uint32_t)D_AES_DIR_DECRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction)
            {
                cmp_result = L_Toolbox_MemCmp((uint8_t*)auth_tag, (uint8_t*)auth_yr,
                        (int16_t)((uint16_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits
                                / AES_BYTE_SIZE_IN_BITS));
                if (0UL == (uint32_t)cmp_result)
                {
                    p_job->p_ISD->prm.AES_AUTH_CIPHER.verification_result = VERIFICATION_PASS;
                }
                else
                {
                    p_job->p_ISD->prm.AES_AUTH_CIPHER.verification_result = VERIFICATION_FAIL;
                }
            }
            else
            {
                (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, &tag_logical);
                L_Toolbox_MemCpy((uint8_t *)tag_logical,
                        (const uint8_t *)auth_tag,
                        (int16_t)((uint32_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits
                                / (uint32_t)AES_BYTE_SIZE_IN_BITS));
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

    if (0UL != resource_id)
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
    else
    {
        /* do nothing */
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0015]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0015]
* Function name: s_01_AUTH_ExecutionProcessCcmEnc
* Description  : Process of releasing the key and context in use
* Arguments    : p_job
*              : job_cycle
* Return Value : SERV_OK(ER_OK)
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_ExecutionProcessCcmEnc(icum_job_t *p_job, uint8_t job_cycle)
{
    uint32_t err = (uint32_t)SERV_OK;
    uint32_t skip_flg = FLG_OFF;

    if (p_job->prm.job_aes_auth.nb_slice != 0u)
    {
        /* AAD operation processing */
        if ( p_job->prm.job_aes_auth.auth_size_bits != 0 )
        {
            if (p_job->prm.job_aes_auth.auth_size_bits > (p_job->prm.job_aes_auth.nb_slice * 128UL))
            {
                /* Continue AAD operation */
                err = s_01_AUTH_ProcessUpdateCcm(p_job, D_AES_CCM_STEP_A, (p_job->prm.job_aes_auth.nb_slice * 128UL)
                        , JOB_NOT_COMPLETE, job_cycle);
                skip_flg = FLG_ON;
            }
            else
            {
                /* AAD operation is completed */
                err = s_01_AUTH_ProcessUpdateCcm(p_job, D_AES_CCM_STEP_A, p_job->prm.job_aes_auth.auth_size_bits
                        , JOB_COMPLETE, job_cycle);
                skip_flg = FLG_ON;
            }
        }
        if (FLG_OFF == skip_flg)
        {
            /* Cipher operation processing */
            if (p_job->prm.job_aes_auth.src_size_bits != 0 )
            {
                p_job->prm.job_aes_auth.not_reverse = 1u;
                if (p_job->prm.job_aes_auth.src_size_bits > (p_job->prm.job_aes_auth.nb_slice * 128UL))
                {
                    /* Continue Cipher operation */
                    err = s_01_AUTH_ProcessUpdateCcm(p_job,D_AES_CCM_STEP_P, (p_job->prm.job_aes_auth.nb_slice * 128UL), JOB_NOT_COMPLETE, job_cycle);
                    skip_flg = FLG_ON;
                }
                else
                {
                    /* slice is over but not complete */
                    err = s_01_AUTH_ProcessUpdateCcm(p_job, D_AES_CCM_STEP_P, p_job->prm.job_aes_auth.src_size_bits
                            , JOB_COMPLETE, job_cycle);
                    if (err != SERV_IN_PROCESS)
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
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* AAD operation processing */
        if ( p_job->prm.job_aes_auth.auth_size_bits != 0 )
        {
            /* AAD operation is completed */
            err = s_01_AUTH_ProcessUpdateCcm(p_job, D_AES_CCM_STEP_A, p_job->prm.job_aes_auth.auth_size_bits
                    , JOB_COMPLETE, job_cycle);
            if (err != SERV_IN_PROCESS)
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* do nothing */
            }
        } /* end of if (p_job->prm.job_aes_auth.auth_size_bits != 0u) */
        else
        {
            /* do nothing */
        }
        if (FLG_OFF == skip_flg)
        {
            /* Cipher operation processing */
            if ( p_job->prm.job_aes_auth.src_size_bits != 0 )
            {
                p_job->prm.job_aes_auth.not_reverse = 1u;
                /* slice is over but not complete */
                err = s_01_AUTH_ProcessUpdateCcm(p_job, D_AES_CCM_STEP_P, p_job->prm.job_aes_auth.src_size_bits
                        , JOB_COMPLETE, job_cycle);
                if (err != SERV_IN_PROCESS)
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
        }
        else
        {
            /* do nothing */
        }
    }
    if (FLG_OFF == skip_flg)
    {
        /* If auth and cipher are 0 or cipher is 0, finish is done here */
        if ( p_job->prm.job_aes_auth.auth_size_bits == 0
                && p_job->prm.job_aes_auth.src_size_bits == 0
                && (job_cycle & (uint8_t)JOB_FINISH) != 0u)
        {
            /* Including job finish */
            err = s_01_AUTH_ProcessFinishCcm(p_job);
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

    return(err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0016]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0016]
* Function name: s_01_AUTH_ExecutionProcessCcmDec
* Description  : Process of releasing the key and context in use
* Arguments    : p_job
*              : job_cycle
* Return Value : SERV_OK(ER_OK)
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_ExecutionProcessCcmDec(icum_job_t *p_job, uint8_t job_cycle)
{
    uint32_t err = (uint32_t)SERV_OK;
    uint32_t skip_flg = FLG_OFF;

    if (p_job->prm.job_aes_auth.nb_slice != 0u)
    {
        /* Cipher operation processing */
        if ( p_job->prm.job_aes_auth.src_size_bits != 0 )
        {
            if (p_job->prm.job_aes_auth.src_size_bits > (p_job->prm.job_aes_auth.nb_slice * 128UL))
            {
                /* Continue Cipher operation */
                err = s_01_AUTH_ProcessUpdateCcm(p_job,D_AES_CCM_STEP_C, (p_job->prm.job_aes_auth.nb_slice * 128UL)
                        , JOB_NOT_COMPLETE, job_cycle);
                skip_flg = FLG_ON;
            }
            else
            {
                /* Cipher operation is completed */
                err = s_01_AUTH_ProcessUpdateCcm(p_job, D_AES_CCM_STEP_C, p_job->prm.job_aes_auth.src_size_bits
                        , JOB_COMPLETE, job_cycle);
                skip_flg = FLG_ON;
            }
        }
        else
        {
            /* do nothing */
        }
        if (FLG_OFF == skip_flg)
        {
            /* AAD operation processing */
            if ( p_job->prm.job_aes_auth.auth_size_bits != 0 )
            {
                p_job->prm.job_aes_auth.not_reverse = 1u;
                if (p_job->prm.job_aes_auth.auth_size_bits > (p_job->prm.job_aes_auth.nb_slice * 128UL))
                {
                    /* Continue AAD operation */
                    err = s_01_AUTH_ProcessUpdateCcm(p_job, D_AES_CCM_STEP_A,(p_job->prm.job_aes_auth.nb_slice * 128UL), JOB_NOT_COMPLETE, job_cycle);
                    skip_flg = FLG_ON;
                }
                else
                {
                    /* AAD operation is completed */
                    err = s_01_AUTH_ProcessUpdateCcm(p_job, D_AES_CCM_STEP_A, p_job->prm.job_aes_auth.auth_size_bits
                            , JOB_COMPLETE, job_cycle);
                    if (err != SERV_IN_PROCESS)
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
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* Cipher operation processing */
        if ( p_job->prm.job_aes_auth.src_size_bits != 0 )
        {
            /* Cipher operation is completed */
            err = s_01_AUTH_ProcessUpdateCcm(p_job, D_AES_CCM_STEP_C, p_job->prm.job_aes_auth.src_size_bits
                                            , JOB_COMPLETE, job_cycle);
            if (err != SERV_IN_PROCESS)
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* do nothing */
            }
        } /* end of if (0u != p_job->prm.job_aes_auth.src_size_bits) */
        else
        {
            /* do nothing */
        }
        if (FLG_OFF == skip_flg)
        {
            /* AAD operation processing */
            if ( p_job->prm.job_aes_auth.auth_size_bits != 0 )
            {
                p_job->prm.job_aes_auth.not_reverse = 1u;
                /* AAD operation is completed */
                err = s_01_AUTH_ProcessUpdateCcm(p_job, D_AES_CCM_STEP_A, p_job->prm.job_aes_auth.auth_size_bits
                        , JOB_COMPLETE, job_cycle);
                if (err != SERV_IN_PROCESS)
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
        }
        else
        {
            /* do nothing */
        }
    }
    if (FLG_OFF == skip_flg)
    {
        /* If auth and cipher are 0 or cipher is 0, finish is done here */
        if( p_job->prm.job_aes_auth.auth_size_bits == 0
                && p_job->prm.job_aes_auth.src_size_bits == 0
                && (job_cycle & (uint8_t)JOB_FINISH) != 0u)
        {
            /* Including job finish */
            err = s_01_AUTH_ProcessFinishCcm(p_job);
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

    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0021]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0021]
* Function name: s_01_Auth_AtOnceAesCcmCheckParam
* Description  : Parameter check of AES_CCM All_at_Once
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                SERV_OK
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_AtOnceAesCcmCheckParam(icum_job_t *p_job)
{
    r_icumif_isd_t *p_isd;
    uint32_t err;
    uint32_t skip_flg;

    err = ER_OK;
    skip_flg = FLG_OFF;

    /* retrieve the global service handler via the status word in host */
    p_isd = p_job->p_ISD;

    /* Process 1 : Encryption / decryption mode (direction) range judgment */
    switch (p_isd->prm.AES_AUTH_CIPHER.direction)
    {
        case CIPHER_DIR_ENCRYPTION:
            p_job->prm.job_aes_auth.direction = D_AES_DIR_ENCRYPTION;
            break;
        case CIPHER_DIR_DECRYPTION:
            p_job->prm.job_aes_auth.direction = D_AES_DIR_DECRYPTION;
            break;
        default:
            err = ER_PARAM;
            skip_flg = FLG_ON;
            break;
    }

    /* Process 2 : Check the address range of TAG data */
    if (FLG_OFF == skip_flg)
    {
        /* Address check of auth_tag size */
        if (p_isd->prm.AES_AUTH_CIPHER.direction == CIPHER_DIR_ENCRYPTION)
        {
            err = s_01_AUTH_CheckMemory((uint32_t)p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits,
                p_isd->prm.AES_AUTH_CIPHER.p_auth_tag, FLS_CHK_MEM_DESTINATION,
                p_job->expansion_service);
        }
        else
        {
            err = s_01_AUTH_CheckMemory((uint32_t)p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits,
                p_isd->prm.AES_AUTH_CIPHER.p_auth_tag, FLS_CHK_MEM_SOURCE,
                p_job->expansion_service);
        }
        if (ER_OK == err)
        {
            /* do nothing */
        }
        else
        {
            skip_flg = FLG_ON;
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    /* Process 3 : IV data address range check */
    if (FLG_OFF == skip_flg)
    {
        /* Address check of iv size */
        err = s_01_AUTH_CheckMemory(p_isd->prm.AES_AUTH_CIPHER.iv_size_in_bits,
                                      p_isd->prm.AES_AUTH_CIPHER.p_iv,
                                      FLS_CHK_MEM_SOURCE, p_job->expansion_service);
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

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0010]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0010]
* Function name: s_01_Auth_AtOnceAesCcm
* Description  : AES_CCM All_at_Once
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_PENDING
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR2
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_ML_CTX_ALL_USE
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                R_SALIF_ER_CRY_TIMEOUT
*                SERV_INVALID_KEY_GROUP
*                SERV_INVALID_KEY_INDEX
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
*                SERV_KEY_NOT_USABLE
***************************************************************************************/
static uint32_t s_01_AUTH_AtOnceAesCcm(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    aes_all_at_once_ccm_t at_once_param;
    ml_ctx_aes_context_t  *p_ctx;

    s_01_com_key_param_t KeyParam1;
    aes_key_t *p_key;
    uint32_t err;
    uint32_t skip_flg;
    static uint32_t auth_tag[4];
    static uint32_t auth_yr[4];
    uint8_t cmp_result;
    uint32_t get_resource_id;
    static uint32_t set_resource_id;
    uint32_t src_logical;
    uint32_t dest_logical;
    uint32_t iv_logical;
    uint32_t auth_logical;
    uint32_t tag_logical;
    uint32_t saved_psw = 0UL;
    uint16_t cmp_size;

    get_resource_id = DATA_ZERO;
    p_key = NULL;
    err = ER_OK;
    skip_flg = FLG_OFF;
    L_Toolbox_MemSet((uint8_t *)&at_once_param, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_ccm_t));

    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;

    /* Check DTF processing */
    if (SERV_IN_PROCESS == p_ISD->service_result)
    {
        cmp_size = (uint16_t)((uint32_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits /
                              AES_BYTE_SIZE_IN_BITS);
        err = s_01_AUTH_CheckDtfProcessing(p_job, set_resource_id,
                                           (uint8_t *)auth_tag, (uint8_t *)auth_yr, cmp_size);
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* Process 1 - 3 */
        err = s_01_AUTH_AtOnceAesCcmCheckParam(p_job);
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

    /* Process 4 : Key data acquisition Key acquisition */
    if (FLG_OFF == skip_flg)
    {
        /* Get AES Key */
        KeyParam1.key_group      = p_ISD->prm.AES_AUTH_CIPHER.key_group;
        KeyParam1.key_id         = p_ISD->prm.AES_AUTH_CIPHER.key_id; 
        KeyParam1.NotUseFlag     = (uint8_t)DATA_ZERO;
        KeyParam1.direction      = p_job->prm.job_aes_auth.direction;
        KeyParam1.p_aeskey       = &p_key;

        err = s_01_CIPHER_GetAESKey(p_job, &KeyParam1);
        if (ER_OK == err)
        {
            /* Get Key success. */
            p_job->prm.job_aes_auth.get_key = RESOURCES_HOLD;
        }
        else
        {
            /*  Error Occurred */
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }

    /* Process 5 : Get Context */
    if (FLG_OFF == skip_flg)
    {
        err = ML_CTX_Get(CTX_TYPE_AES, (uint32_t**)&p_job->prm.job_aes_auth.get_context);
        p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_aes_auth.get_context;
        if (ER_OK == err)
        {
            /* Process 7 : Input,output memory check */
            err = s_01_AUTH_InitSrcDestCcm(p_job);
            if (ER_OK == err)
            {
                err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &get_resource_id);
                if (ER_OK == err)
                {
                    ((ml_ctx_aes_context_t*)p_job->prm.job_aes_auth.get_context)->result = ER_D_AES_INIT_ERROR;
                    set_resource_id = get_resource_id;
                }
                else
                {
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
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }

    /* Process 8 : Parameter settings */
    if (FLG_OFF == skip_flg)
    {
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_AUTH_CIPHER.p_data_in, &src_logical);
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_AUTH_CIPHER.p_data_out, &dest_logical);
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_AUTH_CIPHER.p_iv, &iv_logical);
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_AUTH_CIPHER.p_auth_data, &auth_logical);
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, &tag_logical);
        if((uint32_t)D_AES_DIR_DECRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction)
        {
            L_Toolbox_MemCpy((uint8_t *)auth_tag, (const uint8_t *)tag_logical,
                    (int16_t)((uint32_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits
                            / (uint32_t)AES_BYTE_SIZE_IN_BITS));
            at_once_param.p_auth_tag = auth_tag;
            at_once_param.p_auth_yr  = auth_yr;
        }
        else
        {
            at_once_param.p_auth_tag = (uint32_t *)tag_logical;
            at_once_param.p_auth_yr  = NULL;
        }
        /*  AES Finish */
        if (NULL != p_ctx)
        {
            /* In case of asynchronous processing, store the Context address of the result notification destination */
            g_ContextAddress = (uint32_t)p_ctx;
            /* If Context is specified and All processing is specified, specify asynchronous */
            at_once_param.async = D_AES_TRANS_ASYNC;
        }
        else
        {
            /* In case of synchronous processing, Used to initialize and store results. */
            g_ContextAddress = AES_STATE_INIT;
            at_once_param.async = D_AES_TRANS_SYNC;
        }
        at_once_param.direction  = (uint8_t)p_job->prm.job_aes_auth.direction;
        at_once_param.p_aes_key  = (uint32_t *)p_key->value;
        at_once_param.key_length = p_key->properties.size_in_bytes;
        at_once_param.p_aes_iv   = (uint32_t *)iv_logical;
        at_once_param.iv_size_in_bits = p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits;
        at_once_param.p_auth_data     = (uint32_t *)auth_logical;
        at_once_param.auth_data_size_in_bits = p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits;
        at_once_param.p_aes_src  = (uint32_t*)src_logical;
        at_once_param.data_size_in_bits = p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits;
        at_once_param.auth_tag_size_in_bits = p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits;
        at_once_param.p_aes_dest = (uint32_t*)dest_logical;
        at_once_param.p_context  =  p_ctx->context;
        err = D_AES_ExecutionAtOnceCcm(&at_once_param);
        if (NULL != p_ctx)
        {
            /* In case of asynchronous processing, store the operation result in Context */
            /* disable interrupts (if not already disabled) */
            DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

            if (ER_D_AES_INIT_ERROR == p_ctx->result)
            {
                p_ctx->result = err;
            }
            else
            {
                /* do nothing */
            }

            /* re-enable interrupts (if necessary) */
            ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

            err = ER_OK;
        }
        else
        {
            /* In case of asynchronous processing, check if an error is set by interrupt. */
            if (AES_STATE_INIT == g_ContextAddress)
            {
                /* Do nothing */
            }
            else
            {
                if ( ER_OK == err)
                {
                    err = g_ContextAddress;
                }
                else
                {
                    /* Do nothing */
                }
            }
        }

        err = p_ctx->result;
        /* any error? */
        if (ER_OK == err)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
            if (ER_OK == err)
            {
                if ((uint32_t)D_AES_DIR_DECRYPTION == (uint32_t)p_job->prm.job_aes_auth.direction)
                {
                    cmp_result = L_Toolbox_MemCmp((uint8_t*)auth_tag, (uint8_t*)auth_yr,
                            (int16_t)((uint16_t)p_job->p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits
                                    / AES_BYTE_SIZE_IN_BITS));
                    if (0UL == (uint32_t)cmp_result)
                    {
                        p_job->p_ISD->prm.AES_AUTH_CIPHER.verification_result = VERIFICATION_PASS;
                    }
                    else
                    {
                        p_job->p_ISD->prm.AES_AUTH_CIPHER.verification_result = VERIFICATION_FAIL;
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
        else if (ER_IN_PROCESS == err)
        {
            /* Do nothing */
        }
        else
        {
            /* Prioritize L_CRY_ExecutionAesAtOnce error,do not get ML_ABTR_FreeResource error */
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
        }
    }
    else
    {
        /* do nothing */
    }

    /* Process 10 : return err */
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0009]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0009]
* Function name: S_01_AuthenticateAes
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
static uint32_t s_01_AUTH_Ccm(icum_job_t *p_job, uint8_t Local_job_cycle)
{
    r_icumif_isd_t *p_ISD;
    uint32_t err = (uint32_t)SERV_OK;
    uint32_t skip_flg = FLG_OFF;

    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;

    /* Process 1 : job_slice check */
    if (((uint8_t)JOB_ALL_AT_ONCE == p_ISD->prm.AES_AUTH_CIPHER.job_cycle)
        && ((uint32_t)p_ISD->prm.AES_AUTH_CIPHER.job_slice == 0uL))
    {
        err = s_01_AUTH_AtOnceAesCcm(p_job);
    }
    else
    {
        /* Process 2 : Implementation of INIT processing */
        /* service just triggered? */
        if((p_ISD->service_result == SERV_REQUEST) || (p_ISD->service_result == SERV_PENDING))
        {
            if((Local_job_cycle & (uint8_t)JOB_INIT) != 0u)
            {
                /* initialize the job */
                err = s_01_AUTH_InitAesCipherCcm(p_job);
                if(err != ER_OK)
                {
                    skip_flg = FLG_ON;
                }
                else
                {
                    /* When job_cycle is init only, the processing is end */
                    if((Local_job_cycle & ((uint8_t)JOB_UPDATE | (uint8_t)JOB_FINISH)) == 0u)
                    {
                        skip_flg = FLG_ON;
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
            } /* end of if (0u != ((uint32_t)Local_job_cycle & (uint32_t)JOB_INIT)) */
            else
            {
                /* do nothing */
            }
            /* Process 3 : UPDATE, FINISH preparation process */
            if(FLG_OFF == skip_flg)
            {
                /* updating? */
                if((Local_job_cycle & ((uint8_t)JOB_UPDATE | (uint8_t)JOB_FINISH)) != 0u)
                {
                    err = s_01_AUTH_InitSrcDestCcm(p_job);
                    if(err != ER_OK)
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
        /* Process 4 : Implementation of UPDATE and FINISH processing */
        if(FLG_OFF == skip_flg)
        {
            if (D_AES_DIR_ENCRYPTION == p_job->prm.job_aes_auth.direction)
            {
                err = s_01_AUTH_ExecutionProcessCcmEnc(p_job, Local_job_cycle);
            }
            else
            {
                err = s_01_AUTH_ExecutionProcessCcmDec(p_job, Local_job_cycle);
            }
        }
        else
        {
            /* do nothing */
        }
    }
    return(err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0001]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0001]
* Function name: S_01_AuthenticateAes
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
void S_01_AuthenticateAes(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    uint32_t err = (uint32_t)SERV_OK;
    uint8_t job_cycle = 0;
    uint32_t skip_flg = FLG_OFF;

    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;

    /* Process 1 : Confirm Cancel request */
    if (JOB_CANCEL_REQUESTED == p_job->cancel_request)
    {
        if (((uint8_t)JOB_ALL_AT_ONCE == p_ISD->prm.AES_AUTH_CIPHER.job_cycle)
            && ((uint16_t)NO_SLICE == p_ISD->prm.AES_AUTH_CIPHER.job_slice)
            && (SERV_IN_PROCESS == p_ISD->service_result))
        {
            /* DTF running */
            p_job->cancel_request = (uint8_t)JOB_CANCEL_NO_REQUEST;
        }
        else
        {
            /* DTF no running */
            p_ISD->prm.AES_AUTH_CIPHER.verification_result = VERIFICATION_FAIL;
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* No cancellation request */
        /* do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* Process 2 : Range check of argument job_cycle */
        job_cycle = (uint8_t)p_ISD->prm.AES_AUTH_CIPHER.job_cycle;
        if(job_cycle > ((uint8_t)JOB_INIT | (uint8_t)JOB_UPDATE | (uint8_t)JOB_FINISH))
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            if(job_cycle == (uint8_t)JOB_ALL_AT_ONCE)
            {
                job_cycle = ((uint8_t)JOB_INIT | (uint8_t)JOB_UPDATE | (uint8_t)JOB_FINISH);
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
    /* Process 3 : Performs arithmetic processing for each mode */
    if(FLG_OFF == skip_flg)
    {
        /* Case: GCM Mode */
        if (AUTH_CIPHER_MOD_GCM == p_ISD->prm.AES_AUTH_CIPHER.cipher_mode)
        {
            err = s_01_AUTH_Gcm(p_job, job_cycle);
        }
        /* Case: CCM Mode */
        else if (AUTH_CIPHER_MOD_CCM == p_ISD->prm.AES_AUTH_CIPHER.cipher_mode)
        {
            err = s_01_AUTH_Ccm(p_job, job_cycle);
        }
        else
        {
            err = ER_PARAM;
        }
    }
    else
    {
        /* do nothing */
    }

    /* Process 4 : Job completion, continuous processing */
    err = s_01_CIPHER_ConvertErr(err);

    if(p_job->cancel_request == JOB_CANCEL_REQUESTED)
    {
        p_job->keep.job_cycle = 0u;
        err = s_01_AUTH_ReleaseKeyContext(p_job);
    }
    else
    {
        if(job_cycle != ((uint8_t)JOB_INIT | (uint8_t)JOB_UPDATE | (uint8_t)JOB_FINISH))
        {
            if ((((uint8_t)JOB_INIT & job_cycle) != 0u) && (SERV_PENDING == err))
            {
                p_job->keep.job_cycle = 0u;
                (void)s_01_AUTH_ReleaseKeyContext(p_job);
            }
            /* processing in cycles (init / update / finish) */
            else if((err == (uint32_t)SERV_IN_PROCESS) || (err == (uint32_t)SERV_PENDING))
            {
                /* memory not available OR processing in job slices: need to come back */
            }
            else if((err == SERV_OK) && ((job_cycle & (uint8_t)JOB_FINISH) == 0u))
            {
                /* not finished yet: keep a ghost of the job */
                p_job->keep.job_cycle = 1u;
            }
            else
            {
                p_job->keep.job_cycle = 0u;
                if(err == SERV_OK)
                {
                    err = s_01_AUTH_ReleaseKeyContext(p_job);
                }
                else
                {
                    /* Prioritize errors that occurred before coming here*/
                    /* do not get s_01_AUTH_ReleaseKeyContext error */
                    (void)s_01_AUTH_ReleaseKeyContext(p_job);
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
                if(err == SERV_OK)
                {
                    err = s_01_AUTH_ReleaseKeyContext(p_job);
                }
                else
                {
                    /* Prioritize errors that occurred before coming here */
                    /* do not get s_01_AUTH_ReleaseKeyContext error */
                    (void)s_01_AUTH_ReleaseKeyContext(p_job);
                }
            }
        }
    }
    /* Process 5 : Assign err to service_result */
    p_ISD->service_result = (r_icumif_service_result_t)err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_Auth_Cipher_0018]:[ICUMFW_UD_S_01_AES_Auth_Cipher_0018]
* Function name: s_01_AUTH_CheckMemory
* Description  : CheckMemory
* Arguments    : data_size
*              : data_add
*              : direction (FLS_CHK_MEM_DESTINATION / FLS_CHK_MEM_SOURCE)
*              : expansion (FROM_NATIVE_SERVICE / FROM_EXPANSION_SERVICE)
* Return Value : SERV_OK
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
***************************************************************************************/
static uint32_t s_01_AUTH_CheckMemory(uint32_t data_size, const uint32_t* data_add,
                                      uint8_t direction, uint8_t expansion)
{
    uint8_t mem_check;
    uint32_t chk_size;
    uint32_t err;

    /* Convert bit size to byte size */
    chk_size = ((data_size + ((uint32_t)AES_BYTE_SIZE_IN_BITS - 1U))
                / (uint32_t)AES_BYTE_SIZE_IN_BITS);

    /* make sure the destination address range is valid */
    err = MM_FLS_CheckMemory((uint32_t)data_add, chk_size, direction, expansion, &mem_check);
    if (ER_OK == err)
    {
        if ((uint32_t)FLS_CHK_OK != (uint32_t)mem_check)
        {
            if ((uint32_t)FLS_CHK_MEM_SOURCE == (uint32_t)direction)
            {
                err = (uint32_t)SERV_SRC_MEM_ERROR;
            }
            else
            {
                err = (uint32_t)SERV_DEST_MEM_ERROR;
            }
        }
        else
        {
            err = (uint32_t)SERV_OK;
        }
    }
    else
    {
    }
    return(err);
}
