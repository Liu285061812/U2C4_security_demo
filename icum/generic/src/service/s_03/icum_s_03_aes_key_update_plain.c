/******************************************************************************/
/* Component Name  Service03 Key Update Plain                                 */
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
#include "icum_common_pub.h"
#include "icum_d_comm_pub.h"
#if defined(FSW_REMAP_DRIVER)
/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"
#endif
#include "icum_l_toolbox_pub.h"
#include "icum_l_cry_pk_pub.h"
#include "icum_s_pub.h"
/* For the get config of VM. */
#include "icum_ml_cfg_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mh_sysinfo_pub.h"
#include "icum_mh_key_pub.h"
#include "icum_s_03_common.h"
#include "icum_service_common.h"

#define AES_KEY_UPDATE_PLAIN_NB_ERR    (22uL)
#define ERR_IDX_BASE                   (0uL)
#define ERR_IDX_SERV                   (1uL)
#define ERR_IDX_MAX                    (2uL)

/* process phase */
#define PHASE0                         ((uint8_t)0u)
#define PHASE1                         ((uint8_t)1u)
#define PHASE2                         ((uint8_t)2u)
#define PHASE3                         ((uint8_t)3u)

static uint32_t s_03_UpdatePlain_ConvertErr(uint32_t base_err);
static uint32_t s_03_UpdatePlain_CheckFirst(icum_job_t *p_job);
static uint32_t s_03_UpdatePlain_CheckSecond(icum_job_t *p_job);

/**********************************************************************************
* Error conversion table
***********************************************************************************/
static const uint32_t aes_key_update_plain_err_table[AES_KEY_UPDATE_PLAIN_NB_ERR][ERR_IDX_MAX] =
{
    /* Common error */
    {ER_OK,                       (uint32_t)SERV_OK                 },
    {ER_PARAM,                    (uint32_t)SERV_PRM_ERROR          },
    {ER_PENDING,                  (uint32_t)SERV_PENDING            },
    {ER_IN_PROCESS,               (uint32_t)SERV_IN_PROCESS         },
    /* ER_MH_KEY */
    {ER_MH_KEY_INVALID_KEY_GROUP, (uint32_t)SERV_INVALID_KEY_GROUP  },
    {ER_MH_KEY_INVALID_KEY_INDEX, (uint32_t)SERV_INVALID_KEY_INDEX  },
    {ER_MH_KEY_WRITE_PROTECTED,   (uint32_t)SERV_KEY_WRITE_PROTECTED},
    {ER_MH_KEY_IN_USE,            (uint32_t)SERV_KEY_NOT_USABLE     },
    {ER_MH_KEY_NOT_USABLE,        (uint32_t)SERV_KEY_NOT_USABLE     },
    {ER_MH_KEY_UPDATE_ERROR,      (uint32_t)SERV_KEY_UPDATE_ERROR   },
    {ER_MH_KEY_WRITING,           (uint32_t)SERV_PENDING            },
    {ER_MH_KEY_AES_TIMEOUT_ERROR, (uint32_t)SERV_AES_TIMEOUT_ERROR  },
    {ER_MH_KEY_NVM_UNAVAILABLE,   (uint32_t)SERV_NVM_UNAVAILABLE    },
    {ER_MH_KEY_DATA_INVALID,      (uint32_t)SERV_DATA_INVALID       },
    {ER_MH_KEY_NVM_SYS_CRITICAL,  (uint32_t)SERV_NVM_SYS_ERROR_CRITICAL},
    {ER_MH_KEY_NVM_SYS,           (uint32_t)SERV_NVM_SYS_ERROR      },
    {ER_MH_KEY_NVM_PROG,          (uint32_t)SERV_NVM_PROG_ERROR     },
    {ER_MH_KEY_NVM_SIZE,          (uint32_t)SERV_NVM_SIZE_ERROR     },
    {ER_MH_KEY_SRC_MEM_ERROR,     (uint32_t)SERV_SRC_MEM_ERROR      },
    {ER_MH_KEY_DEST_MEM_ERROR,    (uint32_t)SERV_DEST_MEM_ERROR     },
    {ER_MH_KEY_SYS_ERROR,         (uint32_t)SERV_SYS_ERROR          },
    {ER_MH_KEY_AES_PRM_ERROR,     (uint32_t)SERV_AES_PRM_ERROR      }
};

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_AESKeyUpdate_0002]:[ICUMFW_UD_S_03_AESKeyUpdate_0002]
* Function name: s_03_UpdatePlain_ConvertErr
* Description  : Error conversion processing
* Arguments    : base_err
* Return Value : SERV_OK
*              : SERV_PRM_ERROR
*              : SERV_PENDING
*              : SERV_IN_PROCESS
*              : SERV_INVALID_KEY_GROUP
*              : SERV_KEY_UPDATE_ERROR
*              : SERV_INVALID_KEY_INDEX
*              : SERV_KEY_WRITE_PROTECTED
*              : SERV_KEY_NOT_USABLE
*              : SERV_AES_TIMEOUT_ERROR
*              : SERV_NVM_UNAVAILABLE
*              : SERV_DATA_INVALID
*              : SERV_NVM_SYS_ERROR_CRITICAL
*              : SERV_NVM_SYS_ERROR
*              : SERV_NVM_PROG_ERROR
*              : SERV_NVM_SIZE_ERROR
*              : SERV_SRC_MEM_ERROR
*              : SERV_DEST_MEM_ERROR
*              : SERV_SYS_ERROR
*              : SERV_AES_PRM_ERROR
***************************************************************************************/
static uint32_t s_03_UpdatePlain_ConvertErr(uint32_t base_err)
{
    uint32_t err = base_err;
    uint32_t err_idx;

    for(err_idx = 0u; err_idx < AES_KEY_UPDATE_PLAIN_NB_ERR; err_idx++)
    {
        if(base_err == aes_key_update_plain_err_table[err_idx][ERR_IDX_BASE])
        {
            err = (uint32_t)aes_key_update_plain_err_table[err_idx][ERR_IDX_SERV];
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_AESKeyUpdate_0014]:[ICUMFW_UD_S_03_AESKeyUpdate_0014]
* Function name: s_03_UpdatePlain_CheckFirst
* Description  : Perform process check first(parameter & life cycle)
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_INVALID_KEY_GROUP
***************************************************************************************/
static uint32_t s_03_UpdatePlain_CheckFirst(icum_job_t *p_job)
{
    uint32_t err;
    uint8_t  mem_check;
    r_icumif_isd_t *p_ISD;
    sys_cfg_vm_t vm_cfg;

    /* Initialize local variable */
    err = ER_OK;
    mem_check = FLS_CHK_NG;
    p_ISD = p_job->p_ISD;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));

    /* make sure the source address range is valid */
    if((uint32_t)p_job->expansion_service == (uint32_t)FROM_NATIVE_SERVICE)
    {
        err = MM_FLS_CheckMemory((uint32_t)p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value,
            (uint32_t)p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes, FLS_CHK_MEM_SOURCE,
            p_job->expansion_service, &mem_check);
    }
    else /* FROM_EXPANSION_SERVICE */
    {
        err = MM_FLS_CheckMemoryCISM((uint32_t)p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value,
            (uint32_t)p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes, FLS_CHK_MEM_SOURCE, &mem_check);
    }

    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else if ((uint32_t)FLS_CHK_NG == (uint32_t)mem_check)
    {
        err = ER_MH_KEY_SRC_MEM_ERROR;
    }

    /* check key size */
    else if (((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_128BIT != (uint32_t)p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes)
#if defined(FSW_AESKEY_256)
        && ((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_192BIT != (uint32_t)p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes)
        && ((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_256BIT != (uint32_t)p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes)
#endif /* FSW_AESKEY_256 */
        )
    {
        /* error on key size */
        err = ER_PARAM;
    }

    /* CR key only supported 128bit */
    else if (KEY_GRP_AES_CR == p_ISD->prm.KEY_UPDATE_PLAIN.key_group)
    {
        /* Get VM config */
        (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);

        if ((uint32_t)vm_cfg.privilege_vm_id != (uint32_t)p_job->vm_id)
        {
            err = ER_PARAM;
        }
        else if ((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_128BIT != (uint32_t)p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes)
        {
            err = ER_PARAM;
        }
        else
        {
            /* Do nothing */
        }
    } /* end of else if (KEY_GRP_AES_CR == p_ISD->prm.KEY_UPDATE_PLAIN.key_group) */
    else
    {
        /* Do nothing */
    }

    if (ER_OK == err)
    {
        /* For KEY_GRP_SHE, only RAM_KEY can be used */
        if (KEY_GRP_SHE == p_ISD->prm.KEY_UPDATE_PLAIN.key_group)
        {
            if ((uint32_t)SHE_RAM_KEY == (uint32_t)p_ISD->prm.KEY_UPDATE_PLAIN.key_id)
            {
                /* Set job paramerter convert key group and key index */
                p_job->prm.job_aes_update.key_group = KEY_GRP_AES_RAM;
                p_job->prm.job_aes_update.key_id = 0u;
            }
            else
            {
                /* error: abort */
                err = ER_MH_KEY_INVALID_KEY_INDEX;
            }
        } /* end of if (KEY_GRP_SHE == p_ISD->prm.KEY_UPDATE_PLAIN.key_group) */
        else
        {
            /* Set job paramerter in key group and key index */
            p_job->prm.job_aes_update.key_group = p_job->p_ISD->prm.KEY_UPDATE_PLAIN.key_group;
            p_job->prm.job_aes_update.key_id = p_job->p_ISD->prm.KEY_UPDATE_PLAIN.key_id;
        }
    } /* end of else */
    else
    {
        /* Do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_AESKeyUpdate_0015]:[ICUMFW_UD_S_03_AESKeyUpdate_0015]
* Function name: s_03_UpdatePlain_CheckSecond
* Description  : Perform process check second(current key property)
* Arguments    : p_job
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_INVALID_KEY_GROUP
*              : ER_MH_KEY_INVALID_KEY_INDEX
*              : ER_MH_KEY_IN_USE
*              : ER_MH_KEY_WRITE_PROTECTED
*              : ER_MH_KEY_NOT_USABLE
*              : ER_MH_KEY_UPDATE_ERROR
*              : ER_MH_KEY_NVM_SYS_CRITICAL
*              : ER_MH_KEY_NVM_SYS
*              : ER_MH_KEY_DATA_INVALID
*              : ER_MH_KEY_SYS_ERROR
*              : ER_MH_KEY_NVM_UNAVAILABLE
*              : ER_MH_KEY_NVM_SIZE
*              : ER_MH_KEY_SRC_MEM_ERROR
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
***************************************************************************************/
static uint32_t s_03_UpdatePlain_CheckSecond(icum_job_t *p_job)
{
    uint32_t err;
    aes_key_t *p_current_key;

    /* Initialize local variable */
    err = ER_OK;
    p_current_key = NULL;

    switch (p_job->prm.job_aes_update.key_group)
    {
        case KEY_GRP_AES:
            err = MH_KEY_GetAesKey(p_job,
                                   p_job->prm.job_aes_update.key_group,
                                   p_job->prm.job_aes_update.key_id,
                                   &p_current_key, false);

            if (ER_OK == err)
            {
                if ((uint32_t)MH_KEY_FLAG_WRITE_PROTECT == (uint32_t)p_current_key->properties.flags.write_protected)
                {
                    err = ER_MH_KEY_WRITE_PROTECTED;
                }
                /* firmware can not change the key added in SHE format specification (except RAM key) */
                else if ((uint32_t)MH_KEY_FLAG_UPDATE_SHE == (uint32_t)p_current_key->properties.flags.plain_key)
                {
                    err = ER_MH_KEY_UPDATE_ERROR;
                }
                else
                {
                    /* Do nothing */
                }
            } /* end of if (ER_OK == err) */
            else
            {
                /* Do nothing */
            }
            break;

        case KEY_GRP_AES_RAM:
        case KEY_GRP_AES_CR:
            err = MH_KEY_GetAesKey(p_job,
                                   p_job->prm.job_aes_update.key_group,
                                   p_job->prm.job_aes_update.key_id,
                                   &p_current_key, false);
            break;
        default:
            /* key type not supported */
            err = ER_MH_KEY_INVALID_KEY_GROUP;
            break;
    } /* end of switch (p_job->prm.job_aes_update.key_group) */

    if (ER_MH_KEY_SLOT_EMPTY == err)
    {
        /* In the case of key empty, flag checking is unnecessary */
        err = ER_OK;
    }
    else
    {
        /* Do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_AESKeyUpdate_0001]:[ICUMFW_UD_S_03_AESKeyUpdate_0001]
* Function name: S_03_UpdateAesKeyPlain
* Description  : Key update of AES Key in plain format
* Arguments    : p_job
* Return Value : SERV_OK
*              : SERV_PRM_ERROR
*              : SERV_PENDING
*              : SERV_IN_PROCESS
*              : SERV_INVALID_KEY_GROUP
*              : SERV_INVALID_KEY_INDEX
*              : SERV_KEY_WRITE_PROTECTED
*              : SERV_KEY_NOT_USABLE
*              : SERV_AES_TIMEOUT_ERROR
*              : SERV_NVM_UNAVAILABLE
*              : SERV_DATA_INVALID
*              : SERV_NVM_SYS_ERROR_CRITICAL
*              : SERV_NVM_SYS_ERROR
*              : SERV_NVM_PROG_ERROR
*              : SERV_NVM_SIZE_ERROR
*              : SERV_SRC_MEM_ERROR
*              : SERV_DEST_MEM_ERROR
*              : SERV_SYS_ERROR
*              : SERV_AES_PRM_ERROR
*              : SERV_KEY_UPDATE_ERROR
***************************************************************************************/
void S_03_UpdateAesKeyPlain(icum_job_t *p_job)
{
    uint32_t             err;
    r_icumif_isd_t       *p_ISD;
    aes_key_t            target_key;
    uint32_t             key_value_logical;

    /* Initialize local variable */
    err = ER_MH_KEY_SYS_ERROR;
    p_ISD = p_job->p_ISD;
    L_Toolbox_MemSet((uint8_t *)&target_key, 0x00u, (int16_t)sizeof(aes_key_t));

    /* PHASE0 : Perform check parameter & life cycle */
    if ((uint32_t)PHASE0 == (uint32_t)p_job->process_phase) 
    {
        err = s_03_UpdatePlain_CheckFirst(p_job);
        if (ER_OK == err)
        {
            /* next process */
            p_job->process_phase = PHASE1;
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

    /* PHASE1 : Perform check current key property */
    if ((uint32_t)PHASE1 == (uint32_t)p_job->process_phase)
    {
        err = s_03_UpdatePlain_CheckSecond(p_job);
        if (ER_OK == err)
        {
            /* next process */
            p_job->process_phase = PHASE2;
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

    /* PHASE2 : Perform write process(first) */
    if ((uint32_t)PHASE2 == (uint32_t)p_job->process_phase)
    {
#if defined(FSW_REMAP_DRIVER)
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value, &key_value_logical);
#else
        key_value_logical = (uint32_t)p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value;
#endif /* defined(FSW_REMAP_DRIVER) */
        /* initialize the key parameters */
        L_Toolbox_MemSet((uint8_t*)&target_key, 0x00u, (int16_t)sizeof(aes_key_t));
        target_key.key_group = p_job->prm.job_aes_update.key_group;
        target_key.key_id = p_job->prm.job_aes_update.key_id;
        L_Toolbox_MemCpy((uint8_t *)target_key.value,
                        (const uint8_t *)key_value_logical,
                        (int16_t)p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes);
        /* For key update in plain format, set update_counter to 0 */
        /* update_counter is incremented by keyMNG processing */
        target_key.properties.update_counter   = 0u;
        target_key.properties.flags.invalidate = MH_KEY_FLAG_NOT_INVALID;
        target_key.properties.size_in_bytes    = p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes;
        target_key.properties.flags.plain_key  = MH_KEY_FLAG_UPDATE_PLAIN;

        /* AES_KEY */
        if (KEY_GRP_AES == p_job->prm.job_aes_update.key_group)
        {
            target_key.properties.flags.write_protected  = p_ISD->prm.KEY_UPDATE_PLAIN.flags.write_protected;
            target_key.properties.flags.disable_on_debug = p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug;
            target_key.properties.flags.no_wild_card     = p_ISD->prm.KEY_UPDATE_PLAIN.flags.no_wild_card;

            /* MASTER_ECU_KEY, BOOT_MAC_KEY do not have key usage flag */
            if (((uint32_t)AES_MASTER_ECU_KEY == (uint32_t)p_job->prm.job_aes_update.key_id) ||
                ((uint32_t)AES_BOOT_MAC_KEY == (uint32_t)p_job->prm.job_aes_update.key_id))
            {
                target_key.properties.flags.encryption        = MH_KEY_FLAG_KEY_NOT_UDSABLE;
                target_key.properties.flags.decryption        = MH_KEY_FLAG_KEY_NOT_UDSABLE;
                target_key.properties.flags.cmac_generation   = MH_KEY_FLAG_KEY_NOT_UDSABLE;
                target_key.properties.flags.cmac_verification = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            }
            else
            {
                target_key.properties.flags.encryption        = p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption;
                target_key.properties.flags.decryption        = p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption;
                target_key.properties.flags.cmac_generation   = p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation;
                target_key.properties.flags.cmac_verification = p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification;
            }
        } /* end of if (KEY_GRP_AES == p_job->prm.job_aes_update.key_group) */
        /* RAM_KEY,CR_KEY do not have flag information */
        else
        {
            target_key.properties.flags.encryption        = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            target_key.properties.flags.decryption        = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            target_key.properties.flags.cmac_generation   = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            target_key.properties.flags.cmac_verification = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            target_key.properties.flags.disable_on_debug  = MH_KEY_FLAG_KEY_USE_POSSIBLE;
            target_key.properties.flags.no_wild_card      = MH_KEY_FLAG_WILD_CARD_USABLE;
            target_key.properties.flags.write_protected   = MH_KEY_FLAG_NOT_WRITE_PROTECT;
        }
        err = MH_KEY_SetAesKey(p_job, E_KEY_OPE_FIRST, &target_key);

        if (ER_IN_PROCESS == err)
        {
            p_job->process_phase = PHASE3;
        }
        else
        {
           /* Do nothing */
        }
    } /* end of if ((uint32_t)PHASE2 == (uint32_t)p_job->process_phase) */
    /* PHASE3 : Perform write process(after the second) */
    else if ((uint32_t)PHASE3 == (uint32_t)p_job->process_phase)
    {
        /* RAM_KEY will not come here */
        target_key.key_group = p_job->prm.job_aes_update.key_group;
        target_key.key_id = p_job->prm.job_aes_update.key_id;
        err = MH_KEY_SetAesKey(p_job, E_KEY_OPE_CONTINUE, &target_key);
    }
    else
    {
        /* Do nothing */
    }

    err = s_03_UpdatePlain_ConvertErr(err);
    p_ISD->service_result = (r_icumif_service_result_t)err;
}
