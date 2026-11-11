/******************************************************************************/
/* Component Name   Service 01 AES Common file                                */
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
#include "icum_ml_ctx_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include MM_FLASH_PUBLIC_H

/**********************************************************************************
* common variable
***********************************************************************************/
uint32_t g_ContextAddress;

static const uint32_t AES_ERR_CORRESPOND[AES_CIPHER_NB_ERR][ERR_IDX_MAX] =
{
    /* Common error */
    {ER_OK,                         (uint32_t)SERV_OK               },
    {ER_PARAM,                      (uint32_t)SERV_PRM_ERROR        },
    {ER_PENDING,                    (uint32_t)SERV_PENDING          },
    {ER_IN_PROCESS,                 (uint32_t)SERV_IN_PROCESS       },
    /* AES Driver */
    {ER_D_AES_ALIGN_ERROR,          (uint32_t)SERV_AES_PRM_ERROR    },
    {ER_D_AES_TIMEOUT_ERROR,        (uint32_t)SERV_AES_TIMEOUT_ERROR},
    {ER_D_AES_SEQUENCE_ERROR,       (uint32_t)SERV_SYS_ERROR        },
    /* ER_ML_CTX */
    {ER_ML_CTX_ALL_USE,             (uint32_t)SERV_PENDING          },
    {ER_ML_CTX_NOT_FOUND,           (uint32_t)SERV_SYS_ERROR        },
    /* ER_ML_ABTR */
    {ER_ML_ABTR_EMPTY,              (uint32_t)SERV_PENDING          },
    {ER_ML_ABTR_NOT_RELEASED,       (uint32_t)SERV_SYS_ERROR        },
    {ER_ML_ABTR_INVALID_ID,         (uint32_t)SERV_SYS_ERROR        },
    {ER_ML_ABTR_TIMER_SET_FAILURE,  (uint32_t)SERV_SYS_ERROR        },
    {ER_ML_ABTR_NOT_INITIALIZED,    (uint32_t)SERV_SYS_ERROR        },
    /* ER_MH_KEY */
    {ER_MH_KEY_SLOT_EMPTY,          (uint32_t)SERV_KEY_SLOT_EMPTY   },
    {ER_MH_KEY_INVALID_KEY_GROUP,   (uint32_t)SERV_INVALID_KEY_GROUP},
    {ER_MH_KEY_INVALID_KEY_INDEX,   (uint32_t)SERV_INVALID_KEY_INDEX},
    {ER_MH_KEY_DATA_NOT_FOUND,      (uint32_t)SERV_SYS_ERROR        },
    {ER_MH_KEY_USER_OVER,           (uint32_t)SERV_KEY_NOT_USABLE   },
    {ER_MH_KEY_IN_USE,              (uint32_t)SERV_KEY_NOT_USABLE   },
    {ER_MH_KEY_NOT_USABLE,          (uint32_t)SERV_KEY_NOT_USABLE   },
    {ER_MH_KEY_AES_TIMEOUT_ERROR,   (uint32_t)SERV_AES_TIMEOUT_ERROR},
    {ER_MH_KEY_NVM_UNAVAILABLE,     (uint32_t)SERV_NVM_UNAVAILABLE  },
    {ER_MH_KEY_DATA_INVALID,        (uint32_t)SERV_DATA_INVALID     },
    {ER_MH_KEY_NVM_SYS_CRITICAL,    (uint32_t)SERV_NVM_SYS_ERROR_CRITICAL},
    {ER_MH_KEY_NVM_SYS,             (uint32_t)SERV_NVM_SYS_ERROR    },
    {ER_MH_KEY_NVM_SIZE,            (uint32_t)SERV_NVM_SIZE_ERROR   },
    {ER_MH_KEY_SRC_MEM_ERROR,       (uint32_t)SERV_SRC_MEM_ERROR    },
    {ER_MH_KEY_SYS_ERROR,           (uint32_t)SERV_SYS_ERROR        },
    {ER_MH_KEY_AES_PRM_ERROR,       (uint32_t)SERV_AES_PRM_ERROR    }
};

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0022]:[ICUMFW_UD_S_01_AES_0022]
* Function name: s_01_CIPHER_ConvertErr
* Description  : Error conversion processing
* Arguments    : base_err
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
***************************************************************************************/
uint32_t s_01_CIPHER_ConvertErr(uint32_t base_err)
{
    uint32_t err = base_err;
    uint32_t err_idx;

    for(err_idx = 0u; err_idx < AES_CIPHER_NB_ERR; err_idx++)
    {
        if(base_err == AES_ERR_CORRESPOND[err_idx][ERR_IDX_BASE])
        {
            err = AES_ERR_CORRESPOND[err_idx][ERR_IDX_SERV];
            break;
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0021]:[ICUMFW_UD_S_01_AES_0021]
* Function name: s_01_CIPHER_GetAESKey
* Description  : Get AES Key.
* Arguments    : p_job : job parameter
* Arguments    : s_01_com_key_param_t        : Get Key Parameter
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_D_AES_ALIGN_ERROR
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
uint32_t s_01_CIPHER_GetAESKey(icum_job_t *p_job, s_01_com_key_param_t *p_KeyParam)
{
    aes_key_t *p_stAeskeyLocal = NULL;
    aes_key_t **p_aeskey;
    r_key_group_t key_group;
    uint32_t debug_status;
    uint32_t err = (uint32_t)SERV_OK;
    r_key_index_t key_id;
    uint8_t GetKeyFlag = 0;
    uint32_t skip_flg = FLG_OFF;

    /* Preparation */
    key_group       = p_KeyParam->key_group;
    key_id         = p_KeyParam->key_id;
    p_aeskey     = p_KeyParam->p_aeskey;

    /* SHE key group Check */
    if (KEY_GRP_SHE == key_group)
    {
        if ((key_id == (uint8_t)SHE_BOOT_MAC) || (key_id >= NB_SHE_KEYS))
        {
            /* error on SHE key index */
            err = (uint32_t)SERV_INVALID_KEY_INDEX;
            skip_flg = FLG_ON;
        }
        else
        {
            /* convert from SHE index to internal group / id */
            (void)MH_KEY_ConvertSheKey(key_id, &key_group, &key_id);
        }
    }
    else
    {
        /* do nothing */
    }
    if(skip_flg == FLG_OFF)
    {
        switch(key_group)
        {
            case KEY_GRP_AES:
                if ((AES_MASTER_ECU_KEY == key_id) || (AES_BOOT_MAC_KEY == key_id))
                {
                    /* Not Used MASTER_ECU_KEY and AES_BOOT_MAC_KEY */
                    err = (uint32_t)SERV_INVALID_KEY_INDEX;
                }
                else
                {
                    /* do nothing */
                }
                break;
            case KEY_GRP_AES_RAM:
                /* do nothing */
                break;
            default:
                err = (uint32_t)SERV_INVALID_KEY_GROUP;
                break;
        }
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
    if(skip_flg == FLG_OFF)
    {
        /* Get Key */
        if (0u == p_KeyParam->NotUseFlag)
        {
            /* Need to release key */
            err = MH_KEY_GetAesKey(p_job, key_group, key_id, &p_stAeskeyLocal, true);
        }
        else
        {
            /* No need to release key */
            err = MH_KEY_GetAesKey(p_job, key_group, key_id, &p_stAeskeyLocal, false);
        }

        /* Check Error */
        if (ER_OK != err)
        {
            skip_flg = FLG_ON;
        }
        else
        {
            /* Get Key Success */
            if (0u == p_KeyParam->NotUseFlag)
            {
                /* Release required */
                GetKeyFlag = 1u;
            }
        }
    }
    else
    {
        /* do nothing */
    }
    if(skip_flg == FLG_OFF)
    {
        /* Check Key Property Flag */
        if (KEY_GRP_AES == key_group)
        {
            /* Check if the key is available in Encryption */
            if ((MH_KEY_FLAG_KEY_NOT_UDSABLE == p_stAeskeyLocal->properties.flags.encryption) &&
                    (0u == p_KeyParam->direction))
            {
                /* key not usable: abort */
                err = (uint32_t)SERV_KEY_NOT_USABLE;
                skip_flg = FLG_ON;
            }
            if(skip_flg == FLG_OFF)
            {
                /* Check if the key is available in Decryption */
                if ((MH_KEY_FLAG_KEY_NOT_UDSABLE == p_stAeskeyLocal->properties.flags.decryption) &&
                        (1u == p_KeyParam->direction))
                {
                    /* key not usable: abort */
                    err = (uint32_t)SERV_KEY_NOT_USABLE;
                    skip_flg = FLG_ON;
                }
            }
            else
            {
                /* do nothing */
            }
            if(skip_flg == FLG_OFF)
            {
                /* check if the key is available in debugger access */
                (void)D_COMM_GetICUStatusInfo(&debug_status);
                if((p_stAeskeyLocal->properties.flags.disable_on_debug == MH_KEY_FLAG_KEY_USE_PROHIBIT) &&
                        ((debug_status & ICUM_COMM_INF_ICU_ST_EXT_DEBUG) == ICUM_COMM_INF_ICU_ST_EXT_DEBUG))
                {
                    /* key not usable: abort */
                    err = (uint32_t)SERV_KEY_NOT_USABLE;
                    skip_flg = FLG_ON;
                }
            }
            else
            {
                /* do nothing */
            }
        }
        if(skip_flg == FLG_OFF)
        {
            /* Key Get Success! */
            *p_aeskey = p_stAeskeyLocal;
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

    if ((1u == GetKeyFlag) && (ER_OK != err))
    {
        /* Key Release */
        (void)MH_KEY_FreeKeyCache(p_job, key_group, key_id);
    }

    return (err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0023]:[ICUMFW_UD_S_01_AES_0023]
* Function name: s_01_CIPHER_Sizing
* Description  : Padding characters with '0' after the specified size
* Arguments    : lastb
*                length
* Return Value : SERV_OK
***************************************************************************************/
uint32_t s_01_CIPHER_Sizing(uint8_t *lastb, uint8_t length)
{
    uint8_t calc_cnt;
    uint8_t calc_temp1;
    uint8_t calc_temp2;

    calc_temp1 = length / AES_BYTE_SIZE_IN_BITS;   /* number of bytes */
    calc_temp2 = length % AES_BYTE_SIZE_IN_BITS;   /* number of bits */

    lastb[calc_temp1] = lastb[calc_temp1] & (uint8_t)((uint16_t)0xFF00 >> calc_temp2);

    for (calc_cnt = (calc_temp1 + 1u); calc_cnt < (uint8_t)AES_BLOCK_SIZE_IN_BYTES; calc_cnt++)
    {
        lastb[calc_cnt] = 0x00; /* Substitute 0 for the remaining area */
    }
    return((uint32_t)SERV_OK);
}

CHANGE_SEC(text, ".icum_library_ram")
CHANGE_SEC(rodata, ".icum_library_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_UnifiedAPI_C2_0034]:[ICUMFW_UD_UnifiedAPI_C2_0034]
* Function name: S_01_CIPHER_CallbackAes
* Description  : Stores AES calculation result.
* Arguments    : cry_result
* Return Value : ER_OK
* Return Value : ER_D_AES_ALIGN_ERROR
* Return Value : ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
uint32_t S_01_CIPHER_CallbackAes(uint32_t cry_result)
{
    ml_ctx_aes_context_t *p_ctx;

    if (AES_STATE_INIT != g_ContextAddress)
    {
        /* Callback during asynchronous processing */
        /* the result is stored in the Context specified by g_ContextAddress.*/
        p_ctx = (ml_ctx_aes_context_t*)g_ContextAddress;
        if ((ER_IN_PROCESS == p_ctx->result)
        || (ER_OK == p_ctx->result)
        || (ER_D_AES_INIT_ERROR == p_ctx->result))
        {
            if (ER_OK == cry_result)
            {
                p_ctx->result = ER_OK;
            }
            else if (ER_D_AES_STATUS_ERROR == cry_result)
            {
                p_ctx->result = ER_D_AES_ALIGN_ERROR;
            }
            else
            {
                p_ctx->result = ER_D_AES_TIMEOUT_ERROR;
            }
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* Callback during synchronous processing */
        /* the result is stored in g_ContextAddress.*/
        if (ER_OK == cry_result)
        {
            g_ContextAddress = ER_OK;
        }
        else if (ER_D_AES_STATUS_ERROR == cry_result)
        {
            g_ContextAddress = ER_D_AES_ALIGN_ERROR;
        }
        else
        {
            g_ContextAddress = ER_D_AES_TIMEOUT_ERROR;
        }
    }
    return ER_OK;
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)
