/******************************************************************************/
/* Component Name  Key Manager (COMMON)                                       */
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
#include "renesas_types.h"
/* For the used common define */
#include "icum_common_pub.h"
#include "icum_k_issd_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_l_cry_pk_pub.h"
#include "icum_d_cry_aes_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include "icum_ml_cfg_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mh_key_pub.h"
#include "icum_mh_key.h"

/*===========================================================================
  storing key data being writing
  ===========================================================================*/
static key_writing_t g_key_writing;
/**********************************************************************************
* Local defines
***********************************************************************************/
#define KEY_MNG_NB_ERR         (23u)
#define ERR_IDX_BASE           (0u)
#define ERR_IDX_SERV           (1u)
#define ERR_IDX_MAX            (2u)

/**********************************************************************************
* Error conversion table
***********************************************************************************/
static const uint32_t key_mng_err_table[KEY_MNG_NB_ERR][ERR_IDX_MAX] =
{
    /* ER_MM_FLS */
    {ER_MM_FLS_SYS_CRITICAL,        ER_MH_KEY_NVM_SYS_CRITICAL  },
    {ER_MM_FLS_SYS,                 ER_MH_KEY_NVM_SYS           },
    {ER_MM_FLS_DATA_INVALID,        ER_MH_KEY_DATA_INVALID      },
    {ER_MM_FLS_PROG,                ER_MH_KEY_NVM_PROG          },
    {ER_MM_FLS_SIZE,                ER_MH_KEY_NVM_SIZE          },
    {ER_MM_FLS_MEM_SOURCE,          ER_MH_KEY_SRC_MEM_ERROR     },
    {ER_MM_FLS_MEM_DEST,            ER_MH_KEY_DEST_MEM_ERROR    },
    {ER_MM_FLS_AES_TIME_OUT,        ER_MH_KEY_AES_TIMEOUT_ERROR },
    {ER_MM_FLS_DATA_BLANK,          ER_MH_KEY_SLOT_EMPTY        },
    {ER_MM_FLS_IN_PROGRESS,         ER_IN_PROCESS               },
    {ER_MM_FLS_AES_UNAVAILABLE,     ER_MH_KEY_SYS_ERROR         },
    {ER_MM_FLS_UNAVAILABLE,         ER_MH_KEY_NVM_UNAVAILABLE   },
    {ER_MM_FLS_AES_PRM_ERROR,       ER_MH_KEY_AES_PRM_ERROR     },
    /* Library Driver */
    {ER_L_RSA_INVALID_PARM,         ER_PARAM                    },
    {ER_D_AES_ALIGN_ERROR,          ER_MH_KEY_AES_PRM_ERROR     },
    {ER_D_AES_TIMEOUT_ERROR,        ER_MH_KEY_AES_TIMEOUT_ERROR },
    /* ER_ML_ABTR */
    {ER_ML_ABTR_EMPTY,              ER_PENDING                  },
    {ER_ML_ABTR_NOT_RELEASED,       ER_MH_KEY_SYS_ERROR         },
    {ER_ML_ABTR_NOT_INITIALIZED,    ER_MH_KEY_SYS_ERROR         },
    {ER_ML_ABTR_INVALID_ID,         ER_MH_KEY_SYS_ERROR         },
    {ER_ML_ABTR_TIMER_SET_FAILURE,  ER_MH_KEY_SYS_ERROR         },
    /* L_BIGNUM */
    {ER_L_BIGNUM_BUFFER_OVERFLOW,   ER_PARAM                    },
    {ER_L_BIGNUM_INVALID_ARG,       ER_PARAM                    },
    
};

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0076]:[ICUMFW_UD_MH_KeyMNG_0076]
* Function name: mh_KEY_ConvertErr
* Description  : Key Mng Error conversion processing
* Arguments    : base_err
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_IN_PROCESS
*              : ER_MH_KEY_SLOT_EMPTY
*              : ER_MH_KEY_NOT_USABLE
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
***************************************************************************************/
uint32_t mh_KEY_ConvertErr( uint32_t base_err )
{
    uint32_t err = base_err;
    uint32_t err_idx;

    for(err_idx = 0; err_idx < KEY_MNG_NB_ERR; err_idx++)
    {
        if(base_err == key_mng_err_table[err_idx][ERR_IDX_BASE])
        {
            err = key_mng_err_table[err_idx][ERR_IDX_SERV];
            break;
        }
    }

    return err;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0001]:[ICUMFW_UD_MH_KeyMNG_0001]
* Function name: MH_KEY_Init
* Description  : Key Mng initialization processing (common)
* Arguments    : void
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_KEY_Init(void)
{
    (void)mh_KEY_InitAes();
    (void)mh_KEY_InitRsa();
    (void)mh_KEY_InitEcc();
    (void)mh_KEY_InitHmac();
    (void)mh_KEY_InitMasterSecret();

    (void)mh_KEY_WritingSet(ML_CFG_MASTER_VM_ID,
                            LAST_ITEM_KEY_GRP,
                            KEY_ID_LAST_ITEM);

    return(ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0090]:[ICUMFW_UD_MH_KeyMNG_0090]
* Function name: MH_KEY_InitRamCfg
* Description  : Key Mng initialization processing for config RAM item  (common)
* Arguments    : void
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_KEY_InitRamCfg(void)
{
    (void)mh_KEY_InitAesRamCfg();
    (void)mh_KEY_InitEccRamCfg();
    (void)mh_KEY_InitHmac();
    (void)mh_KEY_InitMasterSecret();

    return(ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0055]:[ICUMFW_UD_MH_KeyMNG_0055]
* Function name: MH_KEY_FreeKeyCache
* Description  : Clear processing of key in-use counter (common)
* Arguments    : p_job
*              : key_group
*              : key_id
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_MH_KEY_NOT_USABLE
*              : ER_MH_KEY_INVALID_KEY_GROUP
*              : ER_MH_KEY_DATA_NOT_FOUND
***************************************************************************************/
uint32_t MH_KEY_FreeKeyCache(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id)
{
    uint32_t err;

    /* Checks the range of VMID */
    err = mh_KEY_CheckVMID(p_job->vm_id);
    if (ER_OK == err)
    {
        switch (key_group)
        {
            case KEY_GRP_AES:
            case KEY_GRP_AES_RAM:
                err = mh_KEY_FreeCacheAes(p_job, key_group, key_id);
                break;
            case KEY_GRP_RSA:
            case KEY_GRP_RSA_PUB:
            case KEY_GRP_RSA_ROM_PUB:
            case KEY_GRP_RSA_PUB_EXT:
                err = mh_KEY_FreeCacheRsa(p_job, key_group, key_id);
                break;
            case KEY_GRP_ECC:
            case KEY_GRP_ECC_PUB:
            case KEY_GRP_ECC_ROM_PUB:
            case KEY_GRP_ECC_RAM:
            case KEY_GRP_ECC_PUB_EXT:
                err = mh_KEY_FreeCacheEcc(p_job, key_group, key_id);
                break;
            case KEY_GRP_HMAC:
                err = mh_KEY_FreeCacheHmac(p_job, key_id);
                break;
            case KEY_GRP_TLS_MASTER:
                err = mh_KEY_FreeCacheMasterSecret(p_job, key_id);
                break;
            default:
                err = ER_MH_KEY_INVALID_KEY_GROUP;
                break;
        } /* end of switch (key_group) */
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    return err;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0061]:[ICUMFW_UD_MH_KeyMNG_0061]
* Function name: MH_KEY_RamKeyDeleteNotAes
* Description  : Delete RAM key(Except AES Key)
* Arguments    : void
* Return Value : ER_OK
*              : ER_MH_KEY_IN_USE
***************************************************************************************/
uint32_t MH_KEY_RamKeyDeleteNotAes(void)
{
    uint32_t err;
    
    err = mh_KEY_DeleteEccRamKey();
    if( ER_OK == err )
    {
      err = mh_KEY_DeleteHmacRamKey();
    }
    if( ER_OK == err )
    {
      err = mh_KEY_DeleteMasterSecretRamKey();
    }
    return err;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0065]:[ICUMFW_UD_MH_KeyMNG_0065]
* Function name: MH_KEY_CacheKeyDeleteNotAes
* Description  : Delete cache key(Except AES Key)
* Arguments    : void
* Return Value : ER_OK
*              : ER_MH_KEY_IN_USE
***************************************************************************************/
uint32_t MH_KEY_CacheKeyDeleteNotAes(void)
{
    uint32_t err;
    
    err = mh_KEY_DeleteRsaCacheKey();
    if( ER_OK == err )
    {
      err = mh_KEY_DeleteEccCacheKey();
    }
    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0068]:[ICUMFW_UD_MH_KeyMNG_0068]
* Function name: mh_KEY_WritingSet
* Description  : Set key of writing
* Arguments    : vm_id
*              : group
*              : id
* Return Value : ER_OK
***************************************************************************************/
uint32_t mh_KEY_WritingSet(uint8_t vm_id, r_key_group_t group, r_key_index_t id)
{
    g_key_writing.vm_id = vm_id;
    g_key_writing.key_group = group;
    g_key_writing.key_id = id;
    return( ER_OK );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0069]:[ICUMFW_UD_MH_KeyMNG_0069]
* Function name: mh_KEY_WritingStsGet
* Description  : Get key write state
* Arguments    : p_write_sts
* Return Value : ER_OK
***************************************************************************************/
uint32_t mh_KEY_WritingStsGet(uint16_t *p_write_sts)
{
    if (g_key_writing.key_group == LAST_ITEM_KEY_GRP)
    {
        *p_write_sts = KEY_NOT_WRITING;
    }
    else
    {
        *p_write_sts = KEY_WRITING;
    }
    return( ER_OK );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0070]:[ICUMFW_UD_MH_KeyMNG_0070]
* Function name: mh_KEY_WritingJudgment
* Description  : Judge that the argument key is in the write state
* Arguments    : p_job
*              : group
*              : id
*              : p_write_sts
* Return Value : ER_OK
***************************************************************************************/
uint32_t mh_KEY_WritingJudgment(icum_job_t *p_job, r_key_group_t group, r_key_index_t id, uint16_t *p_write_sts)
{
    if ((g_key_writing.key_group == group) &&
        ((uint32_t)g_key_writing.vm_id == (uint32_t)p_job->vm_id) &&
        ((uint32_t)g_key_writing.key_id == (uint32_t)id))
    {
        *p_write_sts = KEY_WRITING;
    }
    else
    {
        *p_write_sts = KEY_NOT_WRITING;
    }
    return( ER_OK );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0086]:[ICUMFW_UD_MH_KeyMNG_0086]
* Function name: mh_KEY_MapSizeVM
* Description  : Returns the number of keys set in config of target VM
* Arguments    : vm_id
*              : key_type
*              : p_key_number
* Return Value : ER_OK
***************************************************************************************/
uint32_t mh_KEY_MapSizeVM(uint8_t vm_id, uint8_t key_type, uint8_t *p_key_number)
{
    sys_cfg_key_mapping_t key_mapping;

    /* Initialize local variable */
    L_Toolbox_MemSet((uint8_t *)&key_mapping, 0x00u, (int16_t)sizeof(sys_cfg_key_mapping_t));

    (void)ML_CFG_GetSysVM(vm_id, ML_CFG_SYS_KEY_MAPPING, (uint32_t *)&key_mapping);

    switch ((uint32_t)key_type)
    {
        case KEY_MAPPING_AES:
            *p_key_number = key_mapping.nb_aes_keys;
            break;
        case KEY_MAPPING_RSA_PRIV:
            *p_key_number = key_mapping.nb_rsa_priv_keys;
            break;
        case KEY_MAPPING_RSA_PUB:
            *p_key_number = key_mapping.nb_rsa_pub_keys;
            break;
        case KEY_MAPPING_ECC_PRIV:
            *p_key_number = key_mapping.nb_ecc_priv_keys;
            break;
        case KEY_MAPPING_ECC_PUB:
            *p_key_number = key_mapping.nb_ecc_pub_keys;
            break;
        case KEY_MAPPING_RSA_PUB_EXT:
            *p_key_number = key_mapping.nb_rsa_pub_ext_keys;
            break;
        case KEY_MAPPING_ECC_PUB_EXT:
            *p_key_number = key_mapping.nb_ecc_pub_ext_keys;
            break;
        case KEY_MAPPING_AES_RAM:
            *p_key_number = key_mapping.nb_aes_ram_keys;
            break;
        case KEY_MAPPING_AES_BUFFER:
            *p_key_number = key_mapping.nb_aes_buffer_keys;
            break;
        case KEY_MAPPING_ECC_RAM:
            *p_key_number = key_mapping.nb_ecc_ram_keys;
            break;
        case KEY_MAPPING_HMAC:
            *p_key_number = key_mapping.nb_hmac_keys;
            break;
        case KEY_MAPPING_TLS_MASTER:
            *p_key_number = key_mapping.nb_tls_master_keys;
            break;
        default:
            *p_key_number = (uint8_t)0u;
            break;
    } /* end of switch (key_type) */

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0071]:[ICUMFW_UD_MH_KeyMNG_0071]
* Function name: mh_KEY_MapSize
* Description  : Returns the number of keys set in Config
* Arguments    : key_type
*              : p_key_number
* Return Value : ER_OK
***************************************************************************************/
uint32_t mh_KEY_MapSize(uint8_t key_type, uint8_t *p_key_number)
{
    uint8_t key_number;
    uint32_t vm_id;
    sys_cfg_vm_t vm_cfg;

    /* Initialize variable */
    vm_id = 0uL;
    key_number = 0x00u;
    *p_key_number = 0x00u;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));

    (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
    for (vm_id = 0uL; vm_id < (uint32_t)vm_cfg.nb_vms; vm_id++)
    {
        (void)mh_KEY_MapSizeVM((uint8_t)vm_id, key_type, &key_number);
        *p_key_number += key_number;
    }

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0087]:[ICUMFW_UD_MH_KeyMNG_0087]
* Function name: mh_KEY_CheckVMID
* Description  : Check the range of VMID
* Arguments    : vm_id
* Return Value : ER_OK
*              : ER_MH_KEY_NOT_USABLE
***************************************************************************************/
uint32_t mh_KEY_CheckVMID(uint8_t vm_id)
{
    uint32_t err;
    sys_cfg_vm_t vm_cfg;

    /* Initialize local variable */
    err = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));

    (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
    if ((uint32_t)vm_id >= (uint32_t)vm_cfg.nb_vms)
    {
        err = ER_MH_KEY_NOT_USABLE;
    }
    else
    {
        /* Do nothing */
    }

    return err;
}
