/******************************************************************************/
/* Component Name   Service 01 AES CMAC file                                  */
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
#include "icum_ml_cfg_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include MM_FLASH_PUBLIC_H

/* DATA SIZE */
#define BLOCK_BYTE_SIZE     (16uL)
#define BLOCK_BITS_SIZE     (128uL)

extern uint32_t g_ContextAddress;

/**********************************************************************************
* common variable
***********************************************************************************/
static const uint32_t aes_cmac_err_correspond[AES_CMAC_NB_ERR][ERR_IDX_MAX] =
{
    /* Common error */
    {ER_OK,                         (uint32_t)SERV_OK               },
    {ER_PARAM,                      (uint32_t)SERV_PRM_ERROR        },
    {ER_PENDING,                    (uint32_t)SERV_PENDING          },
    {ER_IN_PROCESS,                 (uint32_t)SERV_IN_PROCESS       },
    /* AES Driver */
    {ER_D_AES_ALIGN_ERROR,          (uint32_t)SERV_AES_PRM_ERROR    },
    {ER_D_AES_TIMEOUT_ERROR,        (uint32_t)SERV_AES_TIMEOUT_ERROR},
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

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t s_01_CMAC_Sizing(uint8_t *lastb, uint8_t length);
static uint32_t s_01_CMAC_Division(uint32_t size_in_bit, uint32_t *p_block, uint16_t *p_last);
static uint32_t s_01_CMAC_ConvertErr(uint32_t base_err);
static uint32_t s_01_CMAC_ProcessVerif(icum_job_t *p_job, uint32_t* input);
static uint32_t s_01_CMAC_ProcessKeyAtOnce(icum_job_t *p_job, aes_key_t **p_key);
static uint32_t s_01_CMAC_CheckMemAtOnce(icum_job_t *p_job);
static uint32_t s_01_CMAC_AtOnceAesCmac(icum_job_t *p_job);
static uint32_t s_01_CMAC_AesCmacJobInit(icum_job_t *p_job);
static uint32_t s_01_CMAC_InitAesCmac(icum_job_t *p_job);
static uint32_t s_01_CMAC_ProcessUpdate(icum_job_t *p_job, uint16_t finish);
static uint32_t s_01_CMAC_ProcessFinish(icum_job_t *p_job);
static uint32_t s_01_CMAC_ReleaseKeyContext(icum_job_t *p_job);
static uint32_t s_01_CMAC_CheckDtf(icum_job_t *p_job, uint32_t *tmp, uint32_t resource_id);
static uint32_t s_01_CMAC_InitCheckParam(icum_job_t *p_job);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0017]:[ICUMFW_UD_S_01_AES_0017]
* Function name: s_01_CMAC_Sizing
* Description  : Padding characters with '0' after the specified size
* Arguments    : lastb
*                length
* Return Value : ER_OK
***************************************************************************************/
static uint32_t s_01_CMAC_Sizing(uint8_t *lastb, uint8_t length)
{
    uint32_t calc_cnt;
    uint32_t calc_temp1;
    uint32_t calc_temp2;

    calc_temp1 = (uint32_t)length / (uint32_t)AES_BYTE_SIZE_IN_BITS;   /* number of bytes */
    calc_temp2 = (uint32_t)length % (uint32_t)AES_BYTE_SIZE_IN_BITS;   /* number of bits */

    lastb[calc_temp1] = (uint8_t)((uint32_t)lastb[calc_temp1] & (0xFF00uL >> calc_temp2));

    for (calc_cnt = (calc_temp1 + 1uL); calc_cnt < (uint32_t)AES_BLOCK_SIZE_IN_BYTES; calc_cnt++)
    {
        lastb[calc_cnt] = 0x00u; /* Substitute 0 for the remaining area */
    }
    return(ER_OK);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0018]:[ICUMFW_UD_S_01_AES_0018]
* Function name: s_01_CMAC_Division
* Description  : Convert size to block unit for AES operation (bit to block)
* Arguments    : size_in_bit
*                p_block
*                p_last
* Return Value : ER_OK
***************************************************************************************/
static uint32_t s_01_CMAC_Division(uint32_t size_in_bit, uint32_t *p_block, uint16_t *p_last)
{
    uint32_t block_size = 0u;
    uint16_t last_size = 0u;

    /* get the number of blocks to process (block_size) */
    /* get the number of bits used in the last block (last_size) */
    if(size_in_bit != 0uL)
    {
        block_size = (size_in_bit / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
        last_size = (uint16_t)(size_in_bit % (uint32_t)AES_BLOCK_SIZE_IN_BITS);
        if((uint32_t)last_size == 0uL)
        {
            /* size_in_bit is a multiple of AES_BLOCK_SIZE_IN_BITS */
            /* all bits used in the last block */
            last_size = (uint16_t)AES_BLOCK_SIZE_IN_BITS;
        }
        else
        {
            /* size_in_bit is *NOT* a multiple of AES_BLOCK_SIZE_IN_BITS */
            /* not all bits are used in the last block */
            /* correct the value of block_size (+1) */
            block_size++;
        }
    }
    else
    {
        /* Do nothing */
    }
    *p_block = block_size;
    *p_last = last_size;
    return(ER_OK);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0019]:[ICUMFW_UD_S_01_AES_0019]
* Function name: s_01_CMAC_ConvertErr
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
*                SERV_DATA_INVALID
*                SERV_NVM_SYS_ERROR_CRITICAL
*                SERV_NVM_SYS_ERROR
*                SERV_NVM_SIZE_ERROR
*                SERV_NVM_UNAVAILABLE
*                SERV_SRC_MEM_ERROR
***************************************************************************************/
static uint32_t s_01_CMAC_ConvertErr(uint32_t base_err)
{
    uint32_t err = base_err;
    uint32_t err_idx;

    for(err_idx = 0uL; err_idx < (uint32_t)AES_CMAC_NB_ERR; err_idx++)
    {
        if(base_err == aes_cmac_err_correspond[err_idx][ERR_IDX_BASE])
        {
            err = aes_cmac_err_correspond[err_idx][ERR_IDX_SERV];
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0015]:[ICUMFW_UD_S_01_AES_0015]
* Function name: s_01_CMAC_ProcessVerif
* Description  : Processing CMAC verify
* Arguments    : p_job
*                input
* Return Value : ER_OK
***************************************************************************************/
static uint32_t s_01_CMAC_ProcessVerif(icum_job_t *p_job, uint32_t* input)
{
    uint32_t exe_blocks;
    uint32_t last_block;
    uint8_t  cmp_result;
    uint32_t uiCmac_logical;

    if((uint32_t)p_job->p_ISD->prm.AES_CMAC.cmac_size_in_bits != (uint32_t)CMAC_SIZE_MIN_VERIFICATION)
    {
        exe_blocks = p_job->p_ISD->prm.AES_CMAC.cmac_size_in_bits;
    }
    /* cmac_size = 0 : compare all bits of the MAC. */
    else
    {
        exe_blocks = CMAC_SIZE_MAX;
    }
    last_block = (exe_blocks % (uint32_t)AES_BYTE_SIZE_IN_BITS);
    exe_blocks = (exe_blocks / (uint32_t)AES_BYTE_SIZE_IN_BITS);
    if(last_block != 0uL)
    {
        (void)s_01_CMAC_Sizing((uint8_t*)input, p_job->p_ISD->prm.AES_CMAC.cmac_size_in_bits);
        exe_blocks++;
    }
    else
    {
        /* Do nothing */
    }
    /* Get Remap(Convert Physical address -> Logical Address) */
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->p_ISD->prm.AES_CMAC.p_cmac, &uiCmac_logical);
    /* proceed with generation of verification */
    if(p_job->p_ISD->prm.AES_CMAC.cmac_operation == CMAC_VERIFICATION)
    {
        /* verify calculated CMAC against reference CMAC provided */
        /* provide the verification result */
        cmp_result = L_Toolbox_MemCmp((const uint8_t *)uiCmac_logical,
                                         (const uint8_t *)input, (int16_t)exe_blocks);
        if((uint32_t)cmp_result == 0uL)
        {
            p_job->p_ISD->prm.AES_CMAC.verification_result = VERIFICATION_PASS;
        }
        else
        {
            p_job->p_ISD->prm.AES_CMAC.verification_result = VERIFICATION_FAIL;
        }
    }
    else
    {
        /* provide calculated CMAC */
        L_Toolbox_MemCpy((uint8_t *)uiCmac_logical, (const uint8_t *)input, (int16_t)exe_blocks);
    }
    return(ER_OK);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0027]:[ICUMFW_UD_S_01_AES_0027]
* Function name: s_01_CMAC_AtOnceAesCmac
* Description  : Execute init, update, and finish of CMAC processing at once
* Arguments    : p_job
* Return Value : ER_OK
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
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
***************************************************************************************/
static uint32_t s_01_CMAC_ProcessKeyAtOnce(icum_job_t *p_job, aes_key_t **p_key)
{
    r_icumif_isd_t *p_ISD;
    r_key_group_t key_group;
    r_key_index_t key_id;
    uint32_t debug_status;
    uint32_t err;
    uint32_t skip_flg;
    
    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;
    
    skip_flg = FLG_OFF;
    key_group = p_ISD->prm.AES_CMAC.key_group;
    key_id    = p_ISD->prm.AES_CMAC.key_id;
    
    /* using SHE key group? */
    /* in that case, the key index given is within the SHE table */
    /* transform into internal key group & key id */
    if(key_group == KEY_GRP_SHE)
    {
        if(((uint32_t)key_id == (uint32_t)SHE_BOOT_MAC) || ((uint32_t)key_id >= (uint32_t)NB_SHE_KEYS))
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
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* retrieve the properties of the key to be used */
        switch(key_group)
        {
            case KEY_GRP_AES:
                if(((uint32_t)key_id == (uint32_t)AES_MASTER_ECU_KEY) ||
                        (((uint32_t)key_id == (uint32_t)AES_BOOT_MAC_KEY) &&
                                (p_ISD->prm.AES_CMAC.cmac_operation == CMAC_GENERATION)))
                {
                    err = (uint32_t)SERV_INVALID_KEY_INDEX;
                }
                else
                {
                    err = MH_KEY_GetAesKey(p_job, key_group, key_id, p_key, true);
                }
                break;
            case KEY_GRP_AES_RAM:
                err = MH_KEY_GetAesKey(p_job, key_group, key_id, p_key, true);
                break;
            default:
                /* key type not supported */
                err = (uint32_t)SERV_INVALID_KEY_GROUP;
                break;
        }
        if(err != ER_OK)
        {
            /* key not available: abort */
            skip_flg = FLG_ON;
        }
        else
        {
            p_job->prm.job_cmac.get_key = RESOURCES_HOLD;
        }
    }
    else
    {
        /* Do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* make sure the key can be used */
        /* RAM_KEY has no flag information, so there is no need to check */
        if(key_group == KEY_GRP_AES)
        {
            /* BOOT_MAC_KEY checks only the disable_on_debug flag */
            if((uint32_t)key_id != (uint32_t)AES_BOOT_MAC_KEY)
            {
                if(((*p_key)->properties.flags.cmac_generation == MH_KEY_FLAG_KEY_NOT_UDSABLE) &&
                        ((uint32_t)p_ISD->prm.AES_CMAC.cmac_operation == (uint32_t)CMAC_GENERATION))
                {
                    /* key not usable: abort */
                    err = (uint32_t)SERV_KEY_NOT_USABLE;
                    skip_flg = FLG_ON;
                }
                else
                {
                    /* Do nothing */
                }
                if (FLG_OFF == skip_flg)
                {
                    if(((*p_key)->properties.flags.cmac_verification == MH_KEY_FLAG_KEY_NOT_UDSABLE) &&
                            ((uint32_t)p_ISD->prm.AES_CMAC.cmac_operation == (uint32_t)CMAC_VERIFICATION))
                    {
                        /* key not usable: abort */
                        err = (uint32_t)SERV_KEY_NOT_USABLE;
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
            }
            else
            {
                /* Do nothing */
            }

            if (FLG_OFF == skip_flg)
            {
                (void)D_COMM_GetICUStatusInfo(&debug_status);
                if(((*p_key)->properties.flags.disable_on_debug == MH_KEY_FLAG_KEY_USE_PROHIBIT) &&
                        ((debug_status & ICUM_COMM_INF_ICU_ST_EXT_DEBUG) == ICUM_COMM_INF_ICU_ST_EXT_DEBUG))
                {
                    /* key not usable: abort */
                    err = (uint32_t)SERV_KEY_NOT_USABLE;
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


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0028]:[ICUMFW_UD_S_01_AES_0028]
* Function name: s_01_CMAC_CheckMemAtOnce
* Description  : Execute init, update, and finish of CMAC processing at once
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
***************************************************************************************/
static uint32_t s_01_CMAC_CheckMemAtOnce(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    uint32_t exe_blocks;
    uint8_t mem_check;
    uint32_t err;
    uint32_t skip_flg;
    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;
    
    err = ER_OK;
    skip_flg = FLG_OFF;
    
    exe_blocks = (p_job->p_ISD->prm.AES_CMAC.msg_size_in_bits / (uint32_t)AES_BYTE_SIZE_IN_BITS);
    if((p_job->p_ISD->prm.AES_CMAC.msg_size_in_bits % (uint32_t)AES_BYTE_SIZE_IN_BITS) != 0uL)
    {
        exe_blocks++;
    }
    else
    {
        /* Do nothing */
    }
    /* make sure the source address range is valid */
    if((uint32_t)p_job->expansion_service == (uint32_t)FROM_NATIVE_SERVICE)
    {
        err = MM_FLS_CheckMemoryUserBoot((uint32_t)p_ISD->prm.AES_CMAC.p_msg, exe_blocks, 
                                    FLS_CHK_MEM_SOURCE, &mem_check);
    }
    else /* FROM_EXPANSION_SERVICE */
    {
        err = MM_FLS_CheckMemoryCISMUserBoot((uint32_t)p_ISD->prm.AES_CMAC.p_msg, exe_blocks, 
                                    FLS_CHK_MEM_SOURCE, &mem_check);
    }
    if(err == ER_OK)
    {
        if((uint32_t)mem_check != (uint32_t)FLS_CHK_OK)
        {
            err = (uint32_t)SERV_SRC_MEM_ERROR;
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        skip_flg = FLG_ON;
    }
    if (FLG_OFF == skip_flg)
    {
        exe_blocks = (uint32_t)p_job->p_ISD->prm.AES_CMAC.cmac_size_in_bits / (uint32_t)AES_BYTE_SIZE_IN_BITS;
        if(((uint32_t)p_job->p_ISD->prm.AES_CMAC.cmac_size_in_bits % (uint32_t)AES_BYTE_SIZE_IN_BITS) != 0uL)
        {
            exe_blocks++;
        }
        else
        {
            /* Do nothing */
        }
        if(p_job->p_ISD->prm.AES_CMAC.cmac_operation == CMAC_GENERATION)
        {
            if((uint32_t)p_job->expansion_service == (uint32_t)FROM_NATIVE_SERVICE)
            {
                err = MM_FLS_CheckMemory((uint32_t)p_job->p_ISD->prm.AES_CMAC.p_cmac, exe_blocks,
                        FLS_CHK_MEM_DESTINATION, p_job->expansion_service, &mem_check);
            }
            else /* FROM_EXPANSION_SERVICE */
            {
                err = MM_FLS_CheckMemoryCISM((uint32_t)p_job->p_ISD->prm.AES_CMAC.p_cmac, exe_blocks,
                        FLS_CHK_MEM_DESTINATION, &mem_check);
            }
            if(err == ER_OK)
            {
                if((uint32_t)mem_check != (uint32_t)FLS_CHK_OK)
                {
                    err = (uint32_t)SERV_DEST_MEM_ERROR;
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
        else
        {
            if((uint32_t)p_job->expansion_service == (uint32_t)FROM_NATIVE_SERVICE)
            {
                err = MM_FLS_CheckMemory((uint32_t)p_job->p_ISD->prm.AES_CMAC.p_cmac, exe_blocks,
                        FLS_CHK_MEM_SOURCE, p_job->expansion_service, &mem_check);
            }
            else /* FROM_EXPANSION_SERVICE */
            {
                err = MM_FLS_CheckMemoryCISM((uint32_t)p_job->p_ISD->prm.AES_CMAC.p_cmac, exe_blocks,
                        FLS_CHK_MEM_SOURCE, &mem_check);
            }
            if(err == ER_OK)
            {
                if((uint32_t)mem_check != (uint32_t)FLS_CHK_OK)
                {
                    err = (uint32_t)SERV_SRC_MEM_ERROR;
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
    }
    else
    {
        /* Do nothing */
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0032]:[ICUMFW_UD_S_01_AES_0032]
* Function name: s_01_CMAC_CheckDtf
* Description  : Check if DTF transfer is in progress
* Arguments    : p_job
*                input
*                p_resource_id
* Return Value : ER_OK
*                ER_ML_ABTR_INVALID_ID
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_SYS_ERROR
*                R_SALIF_ER_OK
*                SERV_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t s_01_CMAC_CheckDtf(icum_job_t *p_job, uint32_t *input, uint32_t resource_id)
{
    uint32_t err;

    err = ER_OK;

    /* Check DTF processing */
    if (NULL != p_job->prm.job_cmac.get_context)
    {
        err = ((ml_ctx_aes_context_t*)p_job->prm.job_cmac.get_context)->result;
        if (ER_OK == err)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
            if (ER_OK == err)
            {
                (void)s_01_CMAC_ProcessVerif(p_job, input);
            }
            else
            {
                /* Do nothing */
            }
            (void)MM_FLS_RemapEnd();
        }
        else if (ER_IN_PROCESS == err)
        {
            /* Do nothing */
        }
        else
        {
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
            (void)MM_FLS_RemapEnd();
        }
    }
    else
    {
        (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        (void)MM_FLS_RemapEnd();
        err = SERV_SEQUENCE_ERROR;
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0010]:[ICUMFW_UD_S_01_AES_0010]
* Function name: s_01_CMAC_AtOnceAesCmac
* Description  : Execute init, update, and finish of CMAC processing at once
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
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
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_PENDING
*                R_SALIF_ER_IN_PROCESS
***************************************************************************************/
static uint32_t s_01_CMAC_AtOnceAesCmac(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    sys_cfg_cmac_min_t cmac_min;
    aes_key_t *p_key;
#pragma alignvar(8)
    static uint32_t temp[4];
    uint32_t err;
    uint32_t get_resource_id;
    static uint32_t set_resource_id;
    uint32_t skip_flg;
    uint32_t uiMsg_logical;
    uint16_t last_block;
    uint32_t saved_psw;

    aes_all_at_once_t    at_once_param;
    ml_ctx_aes_context_t *p_ctx;

    p_key = NULL;
    skip_flg = FLG_OFF;
    get_resource_id = DATA_ZERO;

    L_Toolbox_MemSet((uint8_t *)&at_once_param, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_t));

    cmac_min.mc_length_min_val = 0u;
    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;

    /* Check DTF processing */
    if (SERV_IN_PROCESS == p_ISD->service_result)
    {
        err = s_01_CMAC_CheckDtf(p_job, temp, set_resource_id);
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* confirm the operation type */
        if((p_ISD->prm.AES_CMAC.cmac_operation != CMAC_GENERATION) &&
           (p_ISD->prm.AES_CMAC.cmac_operation != CMAC_VERIFICATION))
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
    if (FLG_OFF == skip_flg)
    {
        /* confirm the value of the cmac length */
        if((uint32_t)p_ISD->prm.AES_CMAC.cmac_size_in_bits > (uint32_t)CMAC_SIZE_MAX)
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

    if (FLG_OFF == skip_flg)
    {
        /* When operation = GENERATION, cmac_size = 0 is prohibited */
        if((uint32_t)p_ISD->prm.AES_CMAC.cmac_size_in_bits == (uint32_t)CMAC_SIZE_MIN_VERIFICATION)
        {
            if(p_ISD->prm.AES_CMAC.cmac_operation == CMAC_GENERATION)
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
    }
    else
    {
        /* Do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* When operation = VERIFICATION, cmac_size needs to be larger than the minimum value setting */
        if(p_ISD->prm.AES_CMAC.cmac_operation == CMAC_VERIFICATION)
        {
            err = ML_CFG_GetSys(ML_CFG_SYS_CMAC_MIN,(uint32_t*)&cmac_min);
            if(err != ER_OK)
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
            if (FLG_OFF == skip_flg)
            {
                if((uint32_t)cmac_min.mc_length_min_ctr == 1uL)
                {
                    if((uint32_t)cmac_min.mc_length_min_val > (uint32_t)p_ISD->prm.AES_CMAC.cmac_size_in_bits)
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
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        err = s_01_CMAC_CheckMemAtOnce(p_job);
        if (err == ER_OK)
        {
            err = s_01_CMAC_ProcessKeyAtOnce(p_job, &p_key);
            if (err == ER_OK)
            {
                /* Do nothing */
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
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        err = ML_CTX_Get(CTX_TYPE_AES, (uint32_t**)&p_job->prm.job_cmac.get_context);
        p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_cmac.get_context;
        if (ER_OK == err)
        {
            ((ml_ctx_aes_context_t*)p_job->prm.job_cmac.get_context)->result = ER_D_AES_INIT_ERROR;
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
    if(FLG_OFF == skip_flg) 
    {
        err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &get_resource_id);
        if(err != ER_OK)
        {
            skip_flg = FLG_ON;
        }
        else
        {
            set_resource_id = get_resource_id;
        }
    }
    else
    {
        /* Do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* Get Remap(Convert Physical address -> Logical Address) */
        if (NULL != p_ISD->prm.AES_CMAC.p_msg)
        {
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.AES_CMAC.p_msg, &uiMsg_logical);
        }   /* end of if (NULL != p_ISD->prm.AES_CMAC.p_msg) */
        else
        {
            /* Null is allowed */
            uiMsg_logical = 0UL;
        }
    }   /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }
    if (FLG_OFF == skip_flg)
    {

        if(err != ER_OK)
        {
            err = (uint32_t)SERV_SRC_MEM_ERROR;
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
        }
        else
        {
            at_once_param.cipher_mode = D_AES_CIPHER_MODE_CMAC;
            at_once_param.direction   = D_AES_DIR_ENCRYPTION;
            at_once_param.number      = ((p_ISD->prm.AES_CMAC.msg_size_in_bits
                                    + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1)) / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
            at_once_param.key_type    = D_AES_KEYTYPE_PLAIN;
            last_block = (uint16_t)(p_ISD->prm.AES_CMAC.msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS);
            if (NUM0 == (uint32_t)last_block)
            {
                if (NUM0 != p_ISD->prm.AES_CMAC.msg_size_in_bits)
                {
                    at_once_param.last_block_in_bits = (uint16_t)AES_BLOCK_SIZE_IN_BITS;
                }
                else
                {
                    at_once_param.last_block_in_bits = (uint16_t)NUM0;
                }
            }
            else
            {
                at_once_param.last_block_in_bits = last_block;
            }
            at_once_param.p_aes_key  = (uint32_t *)p_key->value;
            at_once_param.key_length = p_key->properties.size_in_bytes;
            at_once_param.p_aes_iv   = NULL;
            at_once_param.p_aes_src  = (uint32_t*)uiMsg_logical;
            at_once_param.p_aes_dest = (uint32_t*)temp;
            if (NULL == p_ctx)
            {
                at_once_param.async = D_AES_TRANS_SYNC;
                g_ContextAddress    = (uint32_t)p_ctx;
            }
            else
            {
                /* If Context is specified and All processing is specified, specify asynchronous */
                at_once_param.async = D_AES_TRANS_ASYNC;
                g_ContextAddress    = AES_STATE_INIT;
            }
            err = MM_FLS_RemapStart((uint32_t)uiMsg_logical, (uint32_t**)&at_once_param.p_aes_src);
            err = D_AES_ExecutionAtOnce(&at_once_param);
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
            else {
                err = g_ContextAddress;
            }

            if (ER_OK == err)
            {
                err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
                if(err == ER_OK)
                {
                    (void)s_01_CMAC_ProcessVerif(p_job, temp);
                }
                else
                {
                    /* Do nothing */
                }
                (void)MM_FLS_RemapEnd();
            }
            else if (ER_IN_PROCESS == err)
            {
                /* Do nothing */
            }
            else
            {
                /* Prioritize L_CRY_ExecutionAesAtOnce error,do not get ML_ABTR_FreeResource error */
                (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, set_resource_id);
                (void)MM_FLS_RemapEnd();
            }
        }
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0012]:[ICUMFW_UD_S_01_AES_0012]
* Function name: aes_cmac_job_init
* Description  : Set number and address for Update/Finish process
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
*                SERV_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t s_01_CMAC_AesCmacJobInit(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    uint32_t err = ER_OK;
    uint32_t exe_blocks;
    uint16_t last_block;
    uint8_t mem_check;
    uint32_t skip_flg = FLG_OFF;

    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;

    /* When context is 0, it means that init processing has not been executed yet */
    if(p_job->prm.job_cmac.get_context == NULL)
    {
        err = (uint32_t)SERV_SEQUENCE_ERROR;
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        if((p_ISD->prm.AES_CMAC.job_cycle == JOB_UPDATE) &&
                ((p_ISD->prm.AES_CMAC.msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS) != 0uL))
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
    if (FLG_OFF == skip_flg)
    {
        /* get the job slice configured */
        p_job->prm.job_cmac.nb_slice = p_ISD->prm.AES_CMAC.job_slice;

        /* copy the source address with the job description */
        p_job->prm.job_cmac.p_source  = p_ISD->prm.AES_CMAC.p_msg;

        exe_blocks = p_job->p_ISD->prm.AES_CMAC.msg_size_in_bits / (uint32_t)AES_BYTE_SIZE_IN_BITS;
        if((p_job->p_ISD->prm.AES_CMAC.msg_size_in_bits % (uint32_t)AES_BYTE_SIZE_IN_BITS) != 0uL)
        {
            exe_blocks++;
        }
        else
        {
            /* Do nothing */
        }
        /* make sure the source address range is valid */
        if((uint32_t)p_job->expansion_service == (uint32_t)FROM_NATIVE_SERVICE)
        {
            err = MM_FLS_CheckMemoryUserBoot((uint32_t)p_job->prm.job_cmac.p_source, exe_blocks,
                                        FLS_CHK_MEM_SOURCE, &mem_check);
        }
        else /* FROM_EXPANSION_SERVICE */
        {
            err = MM_FLS_CheckMemoryCISMUserBoot((uint32_t)p_job->prm.job_cmac.p_source, exe_blocks,
                                        FLS_CHK_MEM_SOURCE, &mem_check);
        }
        if(err == ER_OK)
        {
            if((uint32_t)mem_check != (uint32_t)FLS_CHK_OK)
            {
                err = (uint32_t)SERV_SRC_MEM_ERROR;
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            skip_flg = FLG_ON;
        }
        if (FLG_OFF == skip_flg)
        {
            exe_blocks = (uint32_t)p_job->p_ISD->prm.AES_CMAC.cmac_size_in_bits / (uint32_t)AES_BYTE_SIZE_IN_BITS;
            if(((uint32_t)p_job->p_ISD->prm.AES_CMAC.cmac_size_in_bits % (uint32_t)AES_BYTE_SIZE_IN_BITS) != 0uL)
            {
                exe_blocks++;
            }
            else
            {
                /* Do nothing */
            }
            if(p_job->p_ISD->prm.AES_CMAC.cmac_operation == CMAC_GENERATION)
            {
                if((uint32_t)p_job->expansion_service == (uint32_t)FROM_NATIVE_SERVICE)
                {
                    err = MM_FLS_CheckMemory((uint32_t)p_job->p_ISD->prm.AES_CMAC.p_cmac, exe_blocks,
                            FLS_CHK_MEM_DESTINATION, p_job->expansion_service, &mem_check);
                }
                else /* FROM_EXPANSION_SERVICE */
                {
                    err = MM_FLS_CheckMemoryCISM((uint32_t)p_job->p_ISD->prm.AES_CMAC.p_cmac, exe_blocks,
                            FLS_CHK_MEM_DESTINATION, &mem_check);
                }
                if(err == ER_OK)
                {
                    if((uint32_t)mem_check != (uint32_t)FLS_CHK_OK)
                    {
                        err = (uint32_t)SERV_DEST_MEM_ERROR;
                        skip_flg = FLG_ON;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    skip_flg = FLG_ON;
                }
            }
            else
            {
                if((uint32_t)p_job->expansion_service == (uint32_t)FROM_NATIVE_SERVICE)
                {
                    err = MM_FLS_CheckMemoryUserBoot((uint32_t)p_job->p_ISD->prm.AES_CMAC.p_cmac,
                                                        exe_blocks,
                                                        FLS_CHK_MEM_SOURCE,
                                                        &mem_check);
                }
                else /* FROM_EXPANSION_SERVICE */
                {
                    err = MM_FLS_CheckMemoryCISMUserBoot((uint32_t)p_job->p_ISD->prm.AES_CMAC.p_cmac,
                                                            exe_blocks,
                                                            FLS_CHK_MEM_SOURCE,
                                                            &mem_check);
                }
                if(err == ER_OK)
                {
                    if((uint32_t)mem_check != (uint32_t)FLS_CHK_OK)
                    {
                        err = (uint32_t)SERV_SRC_MEM_ERROR;
                        skip_flg = FLG_ON;
                    }
                    else
                    {
                        /* Do nothing */
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
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        (void)s_01_CMAC_Division(p_ISD->prm.AES_CMAC.msg_size_in_bits,&exe_blocks, &last_block);

        /* save local parameters within the job description */
        p_job->prm.job_cmac.nb_blocks = exe_blocks;
        p_job->prm.job_cmac.last_bit  = (uint8_t)last_block;
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0034]:[ICUMFW_UD_S_01_AES_0034]
* Function name: s_01_CMAC_InitCheckParam
* Description  : Initialization of the CMAC operation parameter check.
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
static uint32_t s_01_CMAC_InitCheckParam(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    uint32_t err;
    uint32_t skip_flg;
    sys_cfg_cmac_min_t cmac_min;

    err = ER_OK;
    skip_flg = FLG_OFF;
    cmac_min.mc_length_min_val = DATA_ZERO;

    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;

    /* confirm the operation type */
    if((p_ISD->prm.AES_CMAC.cmac_operation != CMAC_GENERATION) &&
       (p_ISD->prm.AES_CMAC.cmac_operation != CMAC_VERIFICATION))
    {
        err = ER_PARAM;
        skip_flg = FLG_ON;
    }
    else
    {
        /* confirm the value of the cmac length */
        if((uint32_t)p_ISD->prm.AES_CMAC.cmac_size_in_bits > (uint32_t)CMAC_SIZE_MAX)
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            if((uint32_t)p_ISD->prm.AES_CMAC.cmac_size_in_bits == (uint32_t)CMAC_SIZE_MIN_VERIFICATION)
            {
                if(p_ISD->prm.AES_CMAC.cmac_operation == CMAC_GENERATION)
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
        }
    }

    if (FLG_OFF == skip_flg)
    {
        if(p_ISD->prm.AES_CMAC.cmac_operation == CMAC_VERIFICATION)
        {
            err = ML_CFG_GetSys(ML_CFG_SYS_CMAC_MIN,(uint32_t*)&cmac_min);
            if(err == ER_OK)
            {
                if((uint32_t)cmac_min.mc_length_min_ctr == 1uL)
                {
                    if((uint32_t)cmac_min.mc_length_min_val > (uint32_t)p_ISD->prm.AES_CMAC.cmac_size_in_bits)
                    {
                        err = ER_PARAM;
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
    else
    {
        /* Do nothing */
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0011]:[ICUMFW_UD_S_01_AES_0011]
* Function name: s_01_CMAC_InitAesCmac
* Description  : Initialization of the CMAC operation
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_ML_CTX_ALL_USE
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
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
*                SERV_PENDING
*                SERV_KEY_NOT_USABLE
*                SERV_INVALID_KEY_INDEX
*                SERV_INVALID_KEY_GROUP
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_PENDING
*                R_SALIF_ER_CRY_ALIGN_ERROR
***************************************************************************************/
static uint32_t s_01_CMAC_InitAesCmac(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    r_key_group_t key_group;
    r_key_index_t key_id;
    aes_key_t *p_key = NULL;
    uint32_t debug_status;
    uint32_t err;
    uint32_t skip_flg = FLG_OFF;
    uint32_t resource_id;

    ml_ctx_aes_context_t *p_ctx;
    aes_init_t           init_param;

    /* correspond Cert-C */
    resource_id = DATA_ZERO;

    /* retrieve the global service handler via the status word in host */
    p_ISD = p_job->p_ISD;
    L_Toolbox_MemSet((uint8_t *)&init_param, DATA_ZERO, (int16_t)sizeof(aes_init_t));

    err = s_01_CMAC_InitCheckParam(p_job);
    if (ER_OK == err)
    {
        /* Do nothing */
    }
    else
    {
        skip_flg = FLG_ON;
    }

    if (FLG_OFF == skip_flg)
    {
        key_group = p_ISD->prm.AES_CMAC.key_group;
        key_id    = p_ISD->prm.AES_CMAC.key_id;

        /* using SHE key group? */
        /* in that case, the key index given is within the SHE table */
        /* transform into internal key group & key id */
        if(key_group == KEY_GRP_SHE)
        {
            if(((uint32_t)key_id == (uint32_t)SHE_BOOT_MAC) || ((uint32_t)key_id >= (uint32_t)NB_SHE_KEYS))
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
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* retrieve the properties of the key to be used */
        switch(key_group)
        {
            case KEY_GRP_AES:
                if(((uint32_t)key_id == (uint32_t)AES_MASTER_ECU_KEY) ||
                        (((uint32_t)key_id == (uint32_t)AES_BOOT_MAC_KEY) &&
                                (p_ISD->prm.AES_CMAC.cmac_operation == CMAC_GENERATION)))
                {
                    err = (uint32_t)SERV_INVALID_KEY_INDEX;
                }
                else
                {
                    err = MH_KEY_GetAesKey(p_job, key_group, key_id, &p_key, true);
                }
                break;
            case KEY_GRP_AES_RAM:
                err = MH_KEY_GetAesKey(p_job, key_group, key_id, &p_key, true);
                break;
            default:
                /* key type not supported */
                err = (uint32_t)SERV_INVALID_KEY_GROUP;
                break;
        }
        if(err != ER_OK)
        {
            /* key not available: abort */
            skip_flg = FLG_ON;
        }
        else
        {
            p_job->prm.job_cmac.get_key = RESOURCES_HOLD;
        }
    }
    else
    {
        /* Do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* make sure the key can be used */
        /* RAM_KEY has no flag information, so there is no need to check */
        if(key_group == KEY_GRP_AES)
        {
            /* BOOT_MAC_KEY checks only the disable_on_debug flag */
            if((uint32_t)key_id != (uint32_t)AES_BOOT_MAC_KEY)
            {
                if((p_key->properties.flags.cmac_generation == MH_KEY_FLAG_KEY_NOT_UDSABLE) &&
                        ((uint32_t)p_ISD->prm.AES_CMAC.cmac_operation == (uint32_t)CMAC_GENERATION))
                {
                    /* key not usable: abort */
                    err = (uint32_t)SERV_KEY_NOT_USABLE;
                    skip_flg = FLG_ON;
                }
                else
                {
                    /* Do nothing */
                }
                if (FLG_OFF == skip_flg)
                {
                    if((p_key->properties.flags.cmac_verification == MH_KEY_FLAG_KEY_NOT_UDSABLE) &&
                            ((uint32_t)p_ISD->prm.AES_CMAC.cmac_operation == (uint32_t)CMAC_VERIFICATION))
                    {
                        /* key not usable: abort */
                        err = (uint32_t)SERV_KEY_NOT_USABLE;
                        skip_flg = FLG_ON;
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
            if (FLG_OFF == skip_flg)
            {
                (void)D_COMM_GetICUStatusInfo(&debug_status);
                if((p_key->properties.flags.disable_on_debug == MH_KEY_FLAG_KEY_USE_PROHIBIT) &&
                        ((debug_status & ICUM_COMM_INF_ICU_ST_EXT_DEBUG) == ICUM_COMM_INF_ICU_ST_EXT_DEBUG))
                {
                    /* key not usable: abort */
                    err = (uint32_t)SERV_KEY_NOT_USABLE;
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
    if (FLG_OFF == skip_flg)
    {
        err = ML_CTX_Get(CTX_TYPE_AES, (uint32_t**)&p_job->prm.job_cmac.get_context);
        p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_cmac.get_context;
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
        g_ContextAddress       = AES_STATE_INIT;
        init_param.cipher_mode = D_AES_CIPHER_MODE_CMAC;
        init_param.direction   = D_AES_DIR_ENCRYPTION;
        init_param.p_aes_key   = (uint32_t *)p_key->value;
        init_param.key_length  = p_key->properties.size_in_bytes;
        init_param.p_aes_iv    = NULL;
        init_param.p_context   = p_ctx->context;

        err = D_AES_InitJob(&init_param);
        p_ctx->block_info = D_AES_FIRST_CIPHER;
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
        /* Do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0013]:[ICUMFW_UD_S_01_AES_0013]
* Function name: s_01_CMAC_ProcessUpdate
* Description  : Processing update CMAC
* Arguments    : p_job
*                finish
* Return Value : ER_OK
*                ER_PARAM
*                SERV_IN_PROCESS
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                SERV_SRC_MEM_ERROR
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                R_SALIF_ER_CRY_TIMEOUT
***************************************************************************************/
static uint32_t s_01_CMAC_ProcessUpdate(icum_job_t *p_job, uint16_t finish)
{
    uint32_t err;
    uint32_t p_resource_id;
    uint32_t nb_slice;
    uint32_t skip_flg = FLG_OFF;
    aes_update_t update_param;
    ml_ctx_aes_context_t *p_ctx;
    uint32_t  uiMsg_logical;
    uint32_t  msg_size_in_bits;

    p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_cmac.get_context;
    L_Toolbox_MemSet((uint8_t *)&update_param, DATA_ZERO, (int16_t)sizeof(aes_update_t));

    /* Remap(Convert Physical address -> Logical Address) */
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_cmac.p_source, &uiMsg_logical);
    err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &p_resource_id);
    if(err != ER_OK)
    {
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        nb_slice = p_job->prm.job_cmac.nb_slice;
        if((uint32_t)finish == (uint32_t)JOB_NOT_COMPLETE)
        {
            msg_size_in_bits = (p_job->prm.job_cmac.nb_slice * BLOCK_BITS_SIZE);
        }
        else
        {
            msg_size_in_bits = (uint32_t)(p_job->prm.job_cmac.nb_blocks * BLOCK_BITS_SIZE);
        }
        g_ContextAddress        = AES_STATE_INIT;
        update_param.number     = (msg_size_in_bits / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
        update_param.first      = p_ctx->block_info;
        update_param.p_aes_src  = (uint32_t*)uiMsg_logical;
        update_param.p_context  = p_ctx->context;
        update_param.p_aes_dest = NULL;
        err = D_AES_UpdateJob(&update_param);
        p_ctx->block_info = D_AES_NOT_FIRST_CIPHER;
        /* any error? */
        if (ER_OK == err)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, p_resource_id);
            if(err == ER_OK)
            {
                if((uint32_t)finish == (uint32_t)JOB_COMPLETE)
                {
                    err = ER_OK;
                }
                else
                {
                    /* update pointer and number of blocks remaining */
                    p_job->prm.job_cmac.nb_blocks -= nb_slice;
                    p_job->prm.job_cmac.p_source  += 4u * nb_slice;
                    /* not finished */
                    err = (uint32_t)SERV_IN_PROCESS;
                }
            }
            else
            {
                /* Do nothing */
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
        /* Do nothing */
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0014]:[ICUMFW_UD_S_01_AES_0014]
* Function name: s_01_CMAC_ProcessFinish
* Description  : Processing finish CMAC
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_ML_CTX_ALL_USE
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_INVALID_ID
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                R_SALIF_ER_OK
*                R_SALIF_ER_PARAM
*                R_SALIF_ER_CRY_ALIGN_ERROR
*                R_SALIF_ER_CRY_TIMEOUT
***************************************************************************************/
static uint32_t s_01_CMAC_ProcessFinish(icum_job_t *p_job)
{
    ml_ctx_aes_context_t *p_ctx;
    aes_finish_t         finish_param;

    uint32_t temp[4];
    uint32_t err;
    uint32_t p_resource_id;
    uint32_t skip_flg = FLG_OFF;
    uint32_t  uiMsg_logical;
    uint32_t msg_size_in_bits;
    uint16_t last_block;

    p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_cmac.get_context;
    L_Toolbox_MemSet((uint8_t *)&finish_param, DATA_ZERO, (int16_t)sizeof(aes_finish_t));

    /* Remap(Convert Physical address -> Logical Address) */
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_job->prm.job_cmac.p_source, &uiMsg_logical);
    err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &p_resource_id);
    if(err != ER_OK)
    {
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        if (1uL < p_job->prm.job_cmac.nb_blocks)
        {
            msg_size_in_bits = (uint32_t)(p_job->prm.job_cmac.nb_blocks * AES_BLOCK_SIZE_IN_BITS)
                                            - (AES_BLOCK_SIZE_IN_BITS - p_job->prm.job_cmac.last_bit);
        }
        else
        {
            msg_size_in_bits = p_job->prm.job_cmac.last_bit;
        }
        finish_param.number = ((msg_size_in_bits 
                                + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1)) / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
        last_block = (uint16_t)(msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS);
        if (NUM0 == (uint32_t)last_block)
        {
            if (NUM0 != msg_size_in_bits)
            {
                finish_param.last_block_in_bits = (uint16_t)AES_BLOCK_SIZE_IN_BITS;
            }
            else
            {
                finish_param.last_block_in_bits = (uint16_t)NUM0;
            }
        }
        else
        {
            finish_param.last_block_in_bits = last_block;
        }
        g_ContextAddress        = AES_STATE_INIT;
        finish_param.first      = p_ctx->block_info;
        finish_param.p_aes_src  = (uint32_t*)uiMsg_logical;
        finish_param.p_context  = p_ctx->context;
        finish_param.p_aes_dest = (uint32_t*)temp;
        err = D_AES_FinishJob(&finish_param);
        if (ER_OK == err)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, p_resource_id);
            if(err == ER_OK)
            {
                (void)s_01_CMAC_ProcessVerif(p_job, temp);
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Prioritize L_CRY_FinishAes error,do not get ML_ABTR_FreeResource error */
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, p_resource_id);
        }
    }
    else
    {
        /* Do nothing */
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0016]:[ICUMFW_UD_S_01_AES_0016]
* Function name: s_01_CMAC_ReleaseKeyContext
* Description  : Process of releasing the key and context in use
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_DATA_NOT_FOUND
*                ER_ML_CTX_NOT_FOUND
***************************************************************************************/
static uint32_t s_01_CMAC_ReleaseKeyContext(icum_job_t *p_job)
{
    r_key_group_t  key_group;
    r_key_index_t  key_id;
    uint32_t err = ER_OK;
    uint32_t skip_flg = FLG_OFF;
    aes_release_t release_param;
    ml_ctx_aes_context_t *p_ctx;

    p_ctx = (ml_ctx_aes_context_t*)p_job->prm.job_cmac.get_context;
    L_Toolbox_MemSet((uint8_t *)&release_param, DATA_ZERO, (int16_t)sizeof(aes_release_t));

    if(p_job->prm.job_cmac.get_context != NULL)
    {
        release_param.key_length_id = *((uint32_t*)p_ctx->context);
        release_param.key_length    = *((uint16_t*)(p_ctx->context + ADDR_1ST));
        err = D_AES_ReleaseKeyRange(&release_param);
        err = ML_CTX_Free(CTX_TYPE_AES, (uint32_t*)p_job->prm.job_cmac.get_context);
        p_job->prm.job_cmac.get_context = 0u;
        if(err != ER_OK)
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
        /* Do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        if((uint32_t)p_job->prm.job_cmac.get_key == (uint32_t)RESOURCES_HOLD)
        {
            key_group = p_job->p_ISD->prm.AES_CMAC.key_group;
            key_id    = p_job->p_ISD->prm.AES_CMAC.key_id;

            if(key_group == KEY_GRP_SHE)
            {
                /* convert from SHE index to internal group / id */
                (void)MH_KEY_ConvertSheKey(key_id, &key_group, &key_id);
            }
            else
            {
                /* Do nothing */
            }
            err = MH_KEY_FreeKeyCache(p_job, key_group, key_id);
            p_job->prm.job_cmac.get_key = RESOURCES_NOT_HOLD;
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
    err = s_01_CMAC_ConvertErr(err);
    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_AES_0009]:[ICUMFW_UD_S_01_AES_0009]
* Function name: S_01_ProcessAesCmac
* Description  : Run an AES CMAC operation
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
*                SERV_SEQUENCE_ERROR
*                SERV_AES_TIMEOUT_ERROR
*                SERV_AES_PRM_ERROR
*                SERV_DATA_INVALID
*                SERV_NVM_SYS_ERROR_CRITICAL
*                SERV_NVM_SYS_ERROR
*                SERV_NVM_SIZE_ERROR
*                SERV_NVM_UNAVAILABLE
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
***************************************************************************************/
void S_01_ProcessAesCmac(icum_job_t *p_job)
{
    r_icumif_isd_t *p_ISD;
    uint8_t job_cycle = 0u;
    uint32_t err = ER_OK;
    uint32_t skip_flg = FLG_OFF;

    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;

    if ((uint32_t)JOB_CANCEL_REQUESTED == (uint32_t)p_job->cancel_request)
    {
        if (((uint8_t)JOB_ALL_AT_ONCE == p_ISD->prm.AES_CMAC.job_cycle)
            && ((uint16_t)NO_SLICE == p_ISD->prm.AES_CMAC.job_slice)
            && (SERV_IN_PROCESS == p_ISD->service_result))
        {
            /* DTF running */
            p_job->cancel_request = (uint8_t)JOB_CANCEL_NO_REQUEST;
        }
        else
        {
            /* DTF no running */
            p_ISD->prm.AES_CMAC.verification_result = VERIFICATION_FAIL;
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* No cancellation request */
        /* Do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        job_cycle = (uint8_t)p_ISD->prm.AES_CMAC.job_cycle;
        if((uint32_t)job_cycle > ((uint32_t)JOB_INIT | (uint32_t)JOB_UPDATE | (uint32_t)JOB_FINISH))
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
    if (FLG_OFF == skip_flg)
    {
        if((uint32_t)JOB_ALL_AT_ONCE == (uint32_t)job_cycle)
        {
            job_cycle = (uint8_t)((uint32_t)JOB_INIT | (uint32_t)JOB_UPDATE | (uint32_t)JOB_FINISH);
            if(0uL == (uint32_t)p_ISD->prm.AES_CMAC.job_slice)
            {
                err = s_01_CMAC_AtOnceAesCmac(p_job);
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
    }
    else
    {
        /* Do nothing */
    }
    if (FLG_OFF == skip_flg)
    {
        /* service just triggered? */
        if((p_ISD->service_result == SERV_REQUEST) || (p_ISD->service_result == SERV_PENDING))
        {
            if(((uint32_t)job_cycle & (uint32_t)JOB_INIT) != 0uL)
            {
                /* initialize the job */
                err = s_01_CMAC_InitAesCmac(p_job);
                /* any error? */
                if(err != ER_OK)
                {
                    skip_flg = FLG_ON;
                }
                else
                {
                    /* indicate this is a new streaming starting */
                    if(((uint32_t)job_cycle & ((uint32_t)JOB_UPDATE | (uint32_t)JOB_FINISH)) == 0uL)
                    {
                        skip_flg = FLG_ON;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
            }
            else
            {
                /* Do nothing */
            }
            if (FLG_OFF == skip_flg)
            {
                /* updating / finishing? */
                if(((uint32_t)job_cycle & ((uint32_t)JOB_UPDATE | (uint32_t)JOB_FINISH)) != 0uL)
                {
                    err = s_01_CMAC_AesCmacJobInit(p_job);
                    if(err != ER_OK)
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
                    /* Do nothing */
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
    else
    {
        /* Do nothing */
    }

    /* -------------------------------------------------------------------------------- */
    /* CMAC main processing                                                             */
    /* -------------------------------------------------------------------------------- */
    if (FLG_OFF == skip_flg)
    {
        if(((uint32_t)job_cycle & ((uint32_t)JOB_UPDATE | (uint32_t)JOB_FINISH)) != 0uL)
        {
            if(((uint32_t)p_job->prm.job_cmac.nb_slice == 0uL)
                    || ((uint32_t)p_job->prm.job_cmac.nb_slice >= (uint32_t)p_job->prm.job_cmac.nb_blocks))
            {
                /* no job slice*/
                if(((uint32_t)job_cycle & (uint32_t)JOB_FINISH) != 0uL)
                {
                    err = s_01_CMAC_ProcessFinish(p_job);
                }
                else
                {
                    err = s_01_CMAC_ProcessUpdate(p_job, JOB_COMPLETE);
                }
            }
            else
            {   /* Not completed in one operation */
                err = s_01_CMAC_ProcessUpdate(p_job, JOB_NOT_COMPLETE);
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

    err = s_01_CMAC_ConvertErr(err);

    if ((uint32_t)p_job->cancel_request == (uint32_t)JOB_CANCEL_REQUESTED)
    {
        p_job->keep.job_cycle = 0u;
        (void)s_01_CMAC_ReleaseKeyContext(p_job);
    }
    else
    {
        if((uint32_t)job_cycle != ((uint32_t)JOB_INIT | (uint32_t)JOB_UPDATE | (uint32_t)JOB_FINISH))
        {
            if ((((uint8_t)JOB_INIT & job_cycle) != 0u) && (SERV_PENDING == err))
            {
                p_job->keep.job_cycle = 0u;
                (void)s_01_CMAC_ReleaseKeyContext(p_job);
            }
            /* processing in cycles (init / update / finish) */
            else if((err == (uint32_t)SERV_IN_PROCESS) || (err == (uint32_t)SERV_PENDING))
            {
                /* memory not available OR processing in job slices: need to come back */
            }
            else if((err == ER_OK) && (((uint32_t)job_cycle & (uint32_t)JOB_FINISH) == 0uL))
            {
                /* not finished yet: keep a ghost of the job */
                p_job->keep.job_cycle = 1u;
            }
            else
            {
                p_job->keep.job_cycle = 0u;
                if(err == ER_OK)
                {
                    err = s_01_CMAC_ReleaseKeyContext(p_job);
                }
                else
                {
                    /* Prioritize errors that occurred before coming here */
                    /* do not get s_01_CMAC_ReleaseKeyContext error */
                    (void)s_01_CMAC_ReleaseKeyContext(p_job);
                }
            }
        }
        else
        {
            /* processing in cycles all at once */
            if(err != (uint32_t)SERV_IN_PROCESS)
            {
                if(err == ER_OK)
                {
                    err = s_01_CMAC_ReleaseKeyContext(p_job);
                }
                else
                {
                    /* Prioritize errors that occurred before coming here */
                    /* do not get s_01_CMAC_ReleaseKeyContext error */
                    (void)s_01_CMAC_ReleaseKeyContext(p_job);
                }
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    p_ISD->service_result = (r_icumif_service_result_t)err;
}
