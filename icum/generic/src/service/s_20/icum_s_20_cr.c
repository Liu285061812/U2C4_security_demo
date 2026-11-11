/******************************************************************************/
/* Component Name   Random number generation service source file              */
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
* Include
***********************************************************************************/
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "r_icumif_api.h"
#include "icum_d_misc_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_d_wdt_pub.h"
#include "icum_d_cr_pub.h"
#include "icum_d_cry_aes_pub.h"
#if defined(FSW_REMAP_DRIVER)
/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"
#endif
#include "icum_l_toolbox_pub.h"
#include "icum_ml_prng_pub.h"
#include "icum_ml_cfg_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include "icum_ml_ctx_pub.h"
#include "icum_mm_cr_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mh_sysinfo_pub.h"
#include "icum_mh_key_pub.h"
#include "icum_k_issd_pub.h"
#include "icum_s_pub.h"
#include "icum_s_20_cr_pub.h"
#include "icum_service_common.h"

/**********************************************************************************
* Define
***********************************************************************************/
#define NUM2            ((uint32_t)2)   /* value : 2 */
#define NUM3            ((uint32_t)3)   /* value : 3 */

#define OFF             ((uint32_t)0)   /* off(disable) */
#define ON              ((uint32_t)1)   /* on(enable) */

/* Array */
#define ANUM_PER_BLK    ((uint32_t)4)   /* 32bit array number per block(128bit) */

#define T_CHALLENGE     ((uint32_t)0)   /* trigger (challenge request) */

#define CR_NB_ERR              (42u)
#define ERR_IDX_BASE            (0u)
#define ERR_IDX_SERV            (1u)
#define ERR_IDX_MAX             (2u)

#define BLOCK_BYTE_SIZE        (16uL)
#define BLOCK_BITS_SIZE        (128uL)

extern uint32_t g_ContextAddress;

/**********************************************************************************
* Structure
***********************************************************************************/
typedef struct id_s_20_CR_cfg_t
{
    uint32_t    cr_auth_key_type;
    uint32_t    cr_auth_dlt;
} s_20_CR_cfg_t;

/**********************************************************************************
* Variable
***********************************************************************************/
#pragma ghs section data=".BEFORE_SECRET_DATA_SECTION"
static uint32_t g_sds_start = (uint32_t)(&g_sds_start + NUM1);
#pragma ghs section data=".AFTER_SECRET_DATA_SECTION"
static uint32_t g_sds_end = (uint32_t)&g_sds_end;
#pragma ghs section data=default
static s_20_CR_cfg_t g_cfg;
static uint32_t g_challenge[NUM2 * ANUM_PER_BLK];
static uint32_t g_k_debug[ANUM_PER_BLK];

/**********************************************************************************
* common variable
***********************************************************************************/
static const uint32_t cr_err_correspond[CR_NB_ERR][ERR_IDX_MAX] =
{
    /* Common error */
    {ER_OK,                         (uint32_t)SERV_OK                    },
    {ER_PARAM,                      (uint32_t)SERV_PRM_ERROR             },
    {ER_PENDING,                    (uint32_t)SERV_PENDING               },
    {ER_IN_PROCESS,                 (uint32_t)SERV_IN_PROCESS            },
    /* AES Driver */
    {ER_D_AES_ALIGN_ERROR,          (uint32_t)SERV_AES_PRM_ERROR         },
    {ER_D_AES_TIMEOUT_ERROR,        (uint32_t)SERV_AES_TIMEOUT_ERROR     },
    /* ER_ML_ABTR */
    {ER_ML_ABTR_EMPTY,              (uint32_t)SERV_PENDING               },
    {ER_ML_ABTR_NOT_RELEASED,       (uint32_t)SERV_SYS_ERROR             },
    {ER_ML_ABTR_INVALID_ID,         (uint32_t)SERV_SYS_ERROR             },
    {ER_ML_ABTR_TIMER_SET_FAILURE,  (uint32_t)SERV_SYS_ERROR             },
    {ER_ML_ABTR_NOT_INITIALIZED,    (uint32_t)SERV_SYS_ERROR             },
    /* ER_MH_KEY */
    {ER_MH_KEY_SLOT_EMPTY,          (uint32_t)SERV_KEY_SLOT_EMPTY        },
    {ER_MH_KEY_INVALID_KEY_GROUP,   (uint32_t)SERV_INVALID_KEY_GROUP     },
    {ER_MH_KEY_INVALID_KEY_INDEX,   (uint32_t)SERV_INVALID_KEY_INDEX     },
    {ER_MH_KEY_DATA_NOT_FOUND,      (uint32_t)SERV_SYS_ERROR             },
    {ER_MH_KEY_USER_OVER,           (uint32_t)SERV_KEY_NOT_USABLE        },
    {ER_MH_KEY_IN_USE,              (uint32_t)SERV_KEY_NOT_USABLE        },
    {ER_MH_KEY_NOT_USABLE,          (uint32_t)SERV_KEY_NOT_USABLE        },
    {ER_MH_KEY_AES_TIMEOUT_ERROR,   (uint32_t)SERV_AES_TIMEOUT_ERROR     },
    {ER_MH_KEY_NVM_UNAVAILABLE,     (uint32_t)SERV_NVM_UNAVAILABLE       },
    {ER_MH_KEY_DATA_INVALID,        (uint32_t)SERV_DATA_INVALID          },
    {ER_MH_KEY_NVM_SYS_CRITICAL,    (uint32_t)SERV_NVM_SYS_ERROR_CRITICAL},
    {ER_MH_KEY_NVM_SYS,             (uint32_t)SERV_NVM_SYS_ERROR         },
    {ER_MH_KEY_NVM_SIZE,            (uint32_t)SERV_NVM_SIZE_ERROR        },
    {ER_MH_KEY_SRC_MEM_ERROR,       (uint32_t)SERV_SRC_MEM_ERROR         },
    {ER_MH_KEY_SYS_ERROR,           (uint32_t)SERV_SYS_ERROR             },
    {ER_MH_KEY_AES_PRM_ERROR,       (uint32_t)SERV_AES_PRM_ERROR         },
    /* ER_MM_FLS */
    {ER_MM_FLS_SYS,                 (uint32_t)SERV_NVM_SYS_ERROR         },
    {ER_MM_FLS_SYS_CRITICAL,        (uint32_t)SERV_NVM_SYS_ERROR_CRITICAL},
    {ER_MM_FLS_UNAVAILABLE,         (uint32_t)SERV_NVM_UNAVAILABLE       },
    {ER_MM_FLS_MEM_DEST,            (uint32_t)SERV_DEST_MEM_ERROR        },
    {ER_MM_FLS_PROG,                (uint32_t)SERV_NVM_PROG_ERROR        },
    {ER_MM_FLS_SIZE,                (uint32_t)SERV_NVM_SIZE_ERROR        },
    /* MH_SYSINFO */
    {ER_MH_SYSINFO_NVM_SYS,           (uint32_t)SERV_NVM_SYS_ERROR         },
    {ER_MH_SYSINFO_NVM_SYS_CRITICAL,  (uint32_t)SERV_NVM_SYS_ERROR_CRITICAL},
    {ER_MH_SYSINFO_NVM_PROG,          (uint32_t)SERV_NVM_PROG_ERROR        },
    {ER_MH_SYSINFO_NVM_DATA_INVALID,  (uint32_t)SERV_DATA_INVALID          },
    {ER_MH_SYSINFO_NVM_UNAVAILABLE,   (uint32_t)SERV_NVM_UNAVAILABLE       },
    {ER_MH_SYSINFO_UNKNOWN,           (uint32_t)SERV_SYS_ERROR             },
    /* ML_PRNG */
    {ER_ML_PRNG_STATUS,             (uint32_t)SERV_NO_INIT_RNG           },
    {ER_ML_PRNG_RESEED,             (uint32_t)SERV_NEED_RESEED           },
    {ER_ML_PRNG_TIMEOUT,            (uint32_t)SERV_AES_TIMEOUT_ERROR     }
};

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t s_20_CR_ConvertErr(uint32_t base_err);
static uint32_t s_20_CR_WriteProtectChk_rsa(icum_job_t *p_job, sys_cfg_key_mapping_t *p_key_mapping);
static uint32_t s_20_CR_WriteProtectChk_ecc(icum_job_t *p_job, sys_cfg_key_mapping_t *p_key_mapping);
static uint32_t s_20_CR_WriteProtectChk(icum_job_t *p_job);
static uint32_t s_20_CR_CacheRamContextChk(icum_job_t *p_job);
static uint32_t s_20_CR_FlsErase(uint8_t slot_num, uint8_t data_set_type
                                 , uint32_t erase_size, r_icumif_job_id_t job_id);
static uint32_t s_20_CR_FlsEraseVM(uint8_t vm_id, uint8_t slot_num, uint8_t data_set_type, uint32_t erase_size);
static uint32_t s_20_CR_EraseKey(icum_job_t *p_job);
static uint32_t s_20_CR_ProcessChallenge(icum_job_t *p_cr_Job);
static uint32_t s_20_CR_ProcessResponse(icum_job_t *p_cr_Job);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_20_CR_0001]:[ICUMFW_UD_S_20_CR_0001]
* Function name: S_20_CR_Init
* Description  : Initialize the service
* Arguments    : N/A
* Return Value : SERV_OK
***************************************************************************************/
uint32_t S_20_CR_Init(void)
{
    sys_cfg_debug_t sys_cfg;

    sys_cfg.cr_auth_key_type = 0u;
    sys_cfg.cr_auth_dlt = 0u;

    (void)ML_CFG_GetSys(ML_CFG_SYS_DEBUG, (uint32_t*)&sys_cfg);
    g_cfg.cr_auth_key_type  = sys_cfg.cr_auth_key_type;
    g_cfg.cr_auth_dlt       = sys_cfg.cr_auth_dlt;

    return (uint32_t)SERV_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_20_CR_0007]:[ICUMFW_UD_S_20_CR_0007]
* Function name: s_20_CR_ConvertErr
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
*                SERV_NVM_PROG_ERROR
*                SERV_SRC_MEM_ERROR
*                SERV_DEST_MEM_ERROR
*                SERV_NO_INIT_RNG
*                SERV_NEED_RESEED
*                SERV_UNAVAILABLE
***************************************************************************************/
static uint32_t s_20_CR_ConvertErr(uint32_t base_err)
{
    uint32_t err = base_err;
    uint32_t err_idx;

    for(err_idx = 0uL; err_idx < (uint32_t)CR_NB_ERR; err_idx++)
    {
        if(base_err == cr_err_correspond[err_idx][ERR_IDX_BASE])
        {
            err = cr_err_correspond[err_idx][ERR_IDX_SERV];
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
* Function ID  : [ICUMFW_CD_S_20_CR_0011]:[ICUMFW_UD_S_20_CR_0011]
* Function name: s_20_CR_WriteProtectChk_rsa
* Description  : NVM data write protection check of RSA key
* Arguments    : p_job
*              : p_key_mapping
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_SLOT_EMPTY
*              : ER_MH_KEY_INVALID_KEY_GROUP
*              : ER_MH_KEY_INVALID_KEY_INDEX
*              : ER_MH_KEY_IN_USE
*              : ER_MH_KEY_USER_OVER
*              : ER_MH_KEY_NVM_SYS_CRITICAL
*              : ER_MH_KEY_NVM_SYS
*              : ER_MH_KEY_DATA_INVALID
*              : ER_MH_KEY_SYS_ERROR
*              : ER_MH_KEY_NVM_UNAVAILABLE
*              : ER_MH_KEY_NVM_SIZE
*              : ER_MH_KEY_SRC_MEM_ERROR
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
*              : SERV_KEY_WRITE_PROTECTED
***************************************************************************************/
static uint32_t s_20_CR_WriteProtectChk_rsa(icum_job_t *p_job, sys_cfg_key_mapping_t *p_key_mapping)
{
    rsa_key_t *p_rsa_key;
    uint32_t ct;
    uint32_t ret_err;
    
    ret_err = ER_OK;
    
    /** RSA Priv key(KEY_GRP_RSA) **/
    for (ct = NUM0; ct < p_key_mapping->nb_rsa_priv_keys; ++ct)
    {
        ret_err = MH_KEY_GetRsaKey(p_job, KEY_GRP_RSA, (r_key_index_t)ct, &p_rsa_key);
        if (ER_MH_KEY_SLOT_EMPTY != ret_err)
        {
            if (ER_OK == ret_err)
            {
                (void)MH_KEY_FreeKeyCache(p_job, KEY_GRP_RSA, (r_key_index_t)ct);
                if (OFF != p_rsa_key->p_properties->flags.write_protected)
                {
                    ret_err = (uint32_t)SERV_KEY_WRITE_PROTECTED;
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            ret_err = ER_OK;
        }
    }
    
    if (ER_OK == ret_err)
    {
        /** RSA Pub key(KEY_GRP_RSA_PUB) **/
        for (ct = NUM0; ct < p_key_mapping->nb_rsa_pub_keys; ++ct)
        {
            ret_err = MH_KEY_GetRsaKey(p_job, KEY_GRP_RSA_PUB, (r_key_index_t)ct, &p_rsa_key);
            if (ER_MH_KEY_SLOT_EMPTY != ret_err)
            {
                if (ER_OK == ret_err)
                {
                    (void)MH_KEY_FreeKeyCache(p_job, KEY_GRP_RSA_PUB, (r_key_index_t)ct);
                    if (OFF != p_rsa_key->p_properties->flags.write_protected)
                    {
                        ret_err = (uint32_t)SERV_KEY_WRITE_PROTECTED;
                        break;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                ret_err = ER_OK;
            }
        }
    }
    else
    {
        /* Do nothing */
    }
    
    if (ER_OK == ret_err)
    {
        /** RSA Pub Ext key(KEY_GRP_RSA_PUB_EXT) **/
        for (ct = NUM0; ct < p_key_mapping->nb_rsa_pub_ext_keys; ++ct)
        {
            ret_err = MH_KEY_GetRsaKey(p_job, KEY_GRP_RSA_PUB_EXT, (r_key_index_t)ct, &p_rsa_key);
            if (ER_MH_KEY_SLOT_EMPTY != ret_err)
            {
                if (ER_OK == ret_err)
                {
                    (void)MH_KEY_FreeKeyCache(p_job, KEY_GRP_RSA_PUB_EXT, (r_key_index_t)ct);
                    if (OFF != p_rsa_key->p_properties->flags.write_protected)
                    {
                        ret_err = (uint32_t)SERV_KEY_WRITE_PROTECTED;
                        break;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                ret_err = ER_OK;
            }
        }
    }
    else
    {
        /* Do nothing */
    }
    
    return (ret_err);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_20_CR_0012]:[ICUMFW_UD_S_20_CR_0012]
* Function name: s_20_CR_WriteProtectChk_ecc
* Description  : NVM data write protection check of ECC key
* Arguments    : p_job
*              : p_key_mapping
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_SLOT_EMPTY
*              : ER_MH_KEY_INVALID_KEY_GROUP
*              : ER_MH_KEY_INVALID_KEY_INDEX
*              : ER_MH_KEY_IN_USE
*              : ER_MH_KEY_USER_OVER
*              : ER_MH_KEY_NVM_SYS_CRITICAL
*              : ER_MH_KEY_NVM_SYS
*              : ER_MH_KEY_DATA_INVALID
*              : ER_MH_KEY_SYS_ERROR
*              : ER_MH_KEY_NVM_UNAVAILABLE
*              : ER_MH_KEY_NVM_SIZE
*              : ER_MH_KEY_SRC_MEM_ERROR
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
*              : SERV_KEY_WRITE_PROTECTED
***************************************************************************************/
static uint32_t s_20_CR_WriteProtectChk_ecc(icum_job_t *p_job, sys_cfg_key_mapping_t *p_key_mapping)
{
    ecc_key_t *p_ecc_key;
    uint32_t ct;
    uint32_t ret_err;
    
    ret_err = ER_OK;
    
    /** ECC Priv key(KEY_GRP_ECC) **/
    for (ct = NUM0; ct < p_key_mapping->nb_ecc_priv_keys; ++ct)
    {
        ret_err = MH_KEY_GetEccKey(p_job, KEY_GRP_ECC, (r_key_index_t)ct, &p_ecc_key);
        if (ER_MH_KEY_SLOT_EMPTY != ret_err)
        {
            if (ER_OK == ret_err)
            {
                (void)MH_KEY_FreeKeyCache(p_job, KEY_GRP_ECC, (r_key_index_t)ct);
                if (OFF != p_ecc_key->p_properties->flags.write_protected)
                {
                    ret_err = (uint32_t)SERV_KEY_WRITE_PROTECTED;
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            ret_err = ER_OK;
        }
    }
    
    if (ER_OK == ret_err)
    {
        /** ECC Pub key(KEY_GRP_ECC_PUB) **/
        for (ct = NUM0; ct < p_key_mapping->nb_ecc_pub_keys; ++ct)
        {
            ret_err = MH_KEY_GetEccKey(p_job, KEY_GRP_ECC_PUB, (r_key_index_t)ct, &p_ecc_key);
            if (ER_MH_KEY_SLOT_EMPTY != ret_err)
            {
                if (ER_OK == ret_err)
                {
                    (void)MH_KEY_FreeKeyCache(p_job, KEY_GRP_ECC_PUB, (r_key_index_t)ct);
                    if (OFF != p_ecc_key->p_properties->flags.write_protected)
                    {
                        ret_err = (uint32_t)SERV_KEY_WRITE_PROTECTED;
                        break;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                ret_err = ER_OK;
            }
        }
    }
    else
    {
        /* Do nothing */
    }
    
    if (ER_OK == ret_err)
    {
        /** ECC Pub Ext key(KEY_GRP_ECC_PUB_EXT) **/
        for (ct = NUM0; ct < p_key_mapping->nb_ecc_pub_ext_keys; ++ct)
        {
            ret_err = MH_KEY_GetEccKey(p_job, KEY_GRP_ECC_PUB_EXT, (r_key_index_t)ct, &p_ecc_key);
            if (ER_MH_KEY_SLOT_EMPTY != ret_err)
            {
                if (ER_OK == ret_err)
                {
                    (void)MH_KEY_FreeKeyCache(p_job, KEY_GRP_ECC_PUB_EXT, (r_key_index_t)ct);
                    if (OFF != p_ecc_key->p_properties->flags.write_protected)
                    {
                        ret_err = (uint32_t)SERV_KEY_WRITE_PROTECTED;
                        break;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                ret_err = ER_OK;
            }
        }
    }
    else
    {
        /* Do nothing */
    }
    
    return (ret_err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_20_CR_0003]:[ICUMFW_UD_S_20_CR_0003]
* Function name: s_20_CR_WriteProtectChk
* Description  : NVM data write protection check
* Arguments    : icum_job_t *p_job
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_SLOT_EMPTY
*              : ER_MH_KEY_INVALID_KEY_GROUP
*              : ER_MH_KEY_INVALID_KEY_INDEX
*              : ER_MH_KEY_IN_USE
*              : ER_MH_KEY_USER_OVER
*              : ER_MH_KEY_NVM_SYS_CRITICAL
*              : ER_MH_KEY_NVM_SYS
*              : ER_MH_KEY_DATA_INVALID
*              : ER_MH_KEY_SYS_ERROR
*              : ER_MH_KEY_NVM_UNAVAILABLE
*              : ER_MH_KEY_NVM_SIZE
*              : ER_MH_KEY_SRC_MEM_ERROR
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
*              : SERV_KEY_WRITE_PROTECTED
***************************************************************************************/
static uint32_t s_20_CR_WriteProtectChk(icum_job_t *p_job)
{
    aes_key_t *p_aes_key;
    sys_cfg_key_mapping_t key_mapping;
    uint32_t ret_err;
    uint32_t ct;
    bool wdt_flag;
    sys_cfg_vm_t vm_cfg;
    uint32_t vm_id;
    icum_job_t local_job;

    /* Initialize local variable */
    ret_err = ER_OK;
    local_job = *p_job;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));
    L_Toolbox_MemSet((uint8_t *)&key_mapping, 0x00u, (int16_t)sizeof(sys_cfg_key_mapping_t));

    /** AES NVM data write protection check **/
    (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
    for (vm_id = NUM0; (vm_id < (uint32_t)vm_cfg.nb_vms) && (ER_OK == ret_err); vm_id++)
    {
        local_job.vm_id = (uint8_t)vm_id;
        (void)ML_CFG_GetSysVM((uint8_t)vm_id, ML_CFG_SYS_KEY_MAPPING, (uint32_t*)&key_mapping);
        /** AES NVM data write protection check **/
        for (ct = NUM0; (ct < (uint32_t)key_mapping.nb_aes_keys) && (ER_OK == ret_err); ++ct)
        {
            ret_err = MH_KEY_GetAesKey(&local_job, KEY_GRP_AES, (r_key_index_t)ct, &p_aes_key, false);
            if (ER_MH_KEY_SLOT_EMPTY == ret_err)
            {
                ret_err = ER_OK;
            }
            else if (ER_OK == ret_err)
            {
                /* Write protect on */
                if ((uint32_t)OFF != (uint32_t)p_aes_key->properties.flags.write_protected)
                {
                    /* The AES key is MASTER_ECU_KEY and the deletion setting is NOT deletion (3) */
                    /* or                                                                         */
                    /* the AES key is NOT MASTER_ECU_KEY                                          */
                    if ((NUM0 != ct) ||
                        ((NUM0 == ct) &&
                            (NUM3 != g_cfg.cr_auth_dlt)))
                    {
                        ret_err = (uint32_t)SERV_KEY_WRITE_PROTECTED;
                    }
                    /* The AES key is MASTER_ECU_KEY and the deletion setting is deletion (3) */
                    else
                    {
                        /* Do nothing */
                    }
                } /* end of if ((uint32_t)OFF != (uint32_t)p_aes_key->properties.flags.write_protected) */
                else
                {
                    /* Do nothing */
                }
            } /* end of else if (ER_OK == ret_err) */
            else
            {
                /* Do nothing */
            }
            /* Get Watch Dog Timer Status */
            D_WDT_GetIntRequest(&wdt_flag);
            if ((uint32_t)true == (uint32_t)wdt_flag)
            {
                /* 75% Interruption occurred(Reset WDT) */
                D_WDT_Start();
            }
            else
            {
                /* Do nothing */
            }
        } /* end of for (ct = NUM0; (ct < (uint32_t)key_mapping.nb_aes_keys) && (ER_OK == ret_err); ++ct) */

        /* RSA data write protection check */
        if (ER_OK == ret_err)
        {
            ret_err = s_20_CR_WriteProtectChk_rsa(&local_job, &key_mapping);
        }
        else
        {
            /* Do nothing */
        }

        /* ECC data write protection check */
        if (ER_OK == ret_err)
        {
            ret_err = s_20_CR_WriteProtectChk_ecc(&local_job, &key_mapping);
        }
        else
        {
            /* Do nothing */
        }
    } /* end of for (vm_id = NUM0; (vm_id < (uint32_t)vm_cfg.nb_vms) && (ER_OK == ret_err); vm_id++) */

    return(ret_err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_20_CR_0004]:[ICUMFW_UD_S_20_CR_0004]
* Function name: s_20_CR_CacheRamContextChk
* Description  : Check that Cache_key, RAM_KEY, Context are unused
* Arguments    : p_job
* Return Value : ER_OK
*              : ER_MH_KEY_IN_USE
***************************************************************************************/
static uint32_t s_20_CR_CacheRamContextChk(icum_job_t *p_job)
{
    uint32_t ret_err;
    uint8_t judge_ctx_use;
    uint32_t skip_flg = FLG_OFF;

    (void)ML_CTX_InUseCheckAes(&judge_ctx_use);
    if(CTX_FREE != judge_ctx_use)
    {
        ret_err = ER_MH_KEY_IN_USE;
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }
    
    if (FLG_OFF == skip_flg)
    {
        (void)ML_CTX_InUseCheckNotAes(&judge_ctx_use);
        if(CTX_FREE != judge_ctx_use)
        {
            ret_err = ER_MH_KEY_IN_USE;
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
        /* AES Key cache erase */
        ret_err = MH_KEY_ConfigKeyCache(p_job, E_KEYCACHE_CLEAR);

        if (ER_OK != ret_err)
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
        /* Except AES Key cache erase */
        ret_err = MH_KEY_CacheKeyDeleteNotAes();
        if (ER_OK != ret_err)
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
        /** AES RAM key erase **/
        ret_err = MH_KEY_DeleteRamKeyData(p_job, E_DELKEY_TYPE_RAM);
        if (ER_OK != ret_err)
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
        /** Except AES RAM key erase **/
        ret_err = MH_KEY_RamKeyDeleteNotAes();
    }
    else
    {
        /* Do nothing */
    }
    
    return(ret_err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_20_CR_0005]:[ICUMFW_UD_S_20_CR_0005]
* Function name: s_20_CR_FlsErase
* Description  : Data flash erase processing
* Arguments    : slot_num
*              : data_set_type
*              : erase_size
* Return Value : ER_OK
***************************************************************************************/
static uint32_t s_20_CR_FlsErase(uint8_t slot_num, uint8_t data_set_type
                                 , uint32_t erase_size, r_icumif_job_id_t job_id)
{
    /* A parameter which specifies the last index of the key to be deleted */
    uint32_t last_deleted_slot_index;
    uint32_t fls_slot_num;

    /* Initialize the parameter */
    last_deleted_slot_index = NUM0;
    fls_slot_num = (uint32_t)slot_num;

    if ((0uL != fls_slot_num) && (0uL < erase_size))
    {
        /* If the data type is AES key, and the deletion setting is deletion (3), retain MASTER_ECU_KEY. */
        if (((uint32_t)FLS_DS_AES_KEY == (uint32_t)data_set_type)
            && (NUM3 == g_cfg.cr_auth_dlt))
        {
            last_deleted_slot_index = NUM1;
        }
        else
        {
            /* Do nothing */
        }

        /* Continuous MM_FLS_Erase */
        for ( ;last_deleted_slot_index < fls_slot_num ; fls_slot_num--)
        {
            /* Erase the AES NVM data slot */
            (void)MM_FLS_DeleteMapData( data_set_type, (uint8_t)(fls_slot_num - 1uL) );
        } 
    }
    else
    {
        /* Do nothing */
    }

    return(ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_20_CR_0015]:[ICUMFW_UD_S_20_CR_0015]
* Function name: s_20_CR_FlsEraseVM
* Description  : Data flash erase processing
* Arguments    : vm_id
*              : slot_num
*              : data_set_type
*              : erase_size
* Return Value : ER_OK
***************************************************************************************/
static uint32_t s_20_CR_FlsEraseVM(uint8_t vm_id, uint8_t slot_num, uint8_t data_set_type, uint32_t erase_size)
{
    /* A parameter which specifies the last index of the key to be deleted */
    uint32_t fls_slot_num;
    uint32_t last_deleted_slot_index;

    /* Initialize the parameter */
    last_deleted_slot_index = NUM0;
    fls_slot_num = slot_num;

    if ((NUM0 != fls_slot_num) && (NUM0 < erase_size))
    {
        /* If the data type is AES key, and the deletion setting is deletion (3), retain MASTER_ECU_KEY. */
        if (((uint32_t)FLS_DS_AES_KEY == (uint32_t)data_set_type) &&
             (NUM3 == g_cfg.cr_auth_dlt))
        {
            last_deleted_slot_index = NUM1;
        }
        else
        {
            /* Do nothing */
        }

        /* Continuous MM_FLS_Erase */
        for (; last_deleted_slot_index < fls_slot_num; fls_slot_num--)
        {
            /* Erase the AES NVM data slot */
            (void)MM_FLS_DeleteMapData(data_set_type,
                                       (uint8_t)((uint32_t)ML_CFG_VM_AES_IDX_OFFSET(vm_id) + (fls_slot_num - NUM1)));
        }
    } /* end of if ((0uL != fls_slot_num) && (0uL < erase_size)) */
    else
    {
        /* Do nothing */
    }

    return(ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_20_CR_0006]:[ICUMFW_UD_S_20_CR_0006]
* Function name: s_20_CR_EraseKey
* Description  : Key erase processing
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_MM_FLS_SYS
*                ER_MM_FLS_SYS_CRITICAL
*                ER_MM_FLS_UNAVAILABLE
*                ER_MM_FLS_MEM_DEST
*                ER_MM_FLS_PROG
*                ER_MM_FLS_SIZE
***************************************************************************************/
static uint32_t s_20_CR_EraseKey(icum_job_t *p_job)
{
    uint32_t ret_err;
    uint32_t vm_id;
    uint8_t fls_slot_num;
    sys_cfg_vm_t vm_cfg;
    sys_cfg_key_mapping_t key_mapping;
    bool wdt_flag;
    uint32_t skip_flg = FLG_OFF;

    /* Initialize local variable */
    fls_slot_num = 0u;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));
    L_Toolbox_MemSet((uint8_t *)&key_mapping, 0x00u, (int16_t)sizeof(sys_cfg_key_mapping_t));

    /* Check that Cache_key, RAM_KEY, Context are unused */
    ret_err = s_20_CR_CacheRamContextChk(p_job);
    if (ER_OK != ret_err)
    {
        skip_flg = FLG_ON;
    }
    else
    {
        /* Do nothing */
    }
    
    if (FLG_OFF == skip_flg)
    {
        do
        {
            ret_err = MH_SYSINFO_UpdateSystemConfig(p_job->job_id);
            /* Get Watch Dog Timer Status */
            D_WDT_GetIntRequest(&wdt_flag);
            if ((uint32_t)true == (uint32_t)wdt_flag)
            {
                /* 75% Interruption occurred(Reset WDT) */
                D_WDT_Start();
            }
            else
            {
                /* Do nothing */
            }
        }
        while (ER_IN_PROCESS == ret_err);

        if (ER_OK != ret_err)
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
        /** AES NVM data erase **/
        (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
        for (vm_id = NUM0; vm_id < (uint32_t)vm_cfg.nb_vms; vm_id++)
        {
            (void)ML_CFG_GetSysVM((uint8_t)vm_id, ML_CFG_SYS_KEY_MAPPING, (uint32_t*)&key_mapping);

            (void)s_20_CR_FlsEraseVM((uint8_t)vm_id,
                                     key_mapping.nb_aes_keys,
                                     FLS_DS_AES_KEY,
                                     (uint32_t)MH_KEY_AES_DS_NB_BLOCKS * FLS_DF_BLOCK_SIZE);
        }

        /** CR_KEY erase **/
        /* If the deletion setting is deletion (3), retain CR_KEY. */
        if (NUM3 == g_cfg.cr_auth_dlt)
        {
            /* Do nothing */
        }
        /* If the deletion setting is NOT deletion (3), delete CR_KEY */
        else
        {
            (void)s_20_CR_FlsErase(MH_KEY_NB_CR_KEYS, 
                                    FLS_DS_CR_KEY, 
                                    (uint32_t)MH_KEY_AES_DS_NB_BLOCKS * FLS_DF_BLOCK_SIZE, p_job->job_id);
        }

        /* Except AES NVM data erase */
        /** RSA Priv NVM data erase **/
        (void)MM_FLS_GetMaxSlotNum(FLS_DS_RSA_PRIV_KEY, &fls_slot_num);
        (void)s_20_CR_FlsErase(fls_slot_num,
                                FLS_DS_RSA_PRIV_KEY, 
                                (uint32_t)MH_KEY_RSA_PRIV_DS_NB_BLOCKS * FLS_DF_BLOCK_SIZE, p_job->job_id);

        /** RSA Pub NVM data erase **/
        (void)MM_FLS_GetMaxSlotNum(FLS_DS_RSA_PUB_KEY, &fls_slot_num);
        (void)s_20_CR_FlsErase(fls_slot_num,
                                FLS_DS_RSA_PUB_KEY, 
                                (uint32_t)MH_KEY_RSA_PUB_DS_NB_BLOCKS * FLS_DF_BLOCK_SIZE, p_job->job_id);

        /** RSA Pub Ext NVM data erase **/
        (void)MM_FLS_GetMaxSlotNum(FLS_DS_RSA_PUB_EXT_KEY, &fls_slot_num);
        (void)s_20_CR_FlsErase(fls_slot_num,
                                FLS_DS_RSA_PUB_EXT_KEY, 
                                (uint32_t)MH_KEY_RSA_PUB_EXT_NB_BLOCKS * FLS_DF_BLOCK_SIZE, p_job->job_id);

        /** ECC Priv NVM data erase **/
        (void)MM_FLS_GetMaxSlotNum(FLS_DS_ECC_PRIV_KEY, &fls_slot_num);
        (void)s_20_CR_FlsErase(fls_slot_num,
                                FLS_DS_ECC_PRIV_KEY, 
                                (uint32_t)MH_KEY_ECC_PRIV_DS_NB_BLOCKS * FLS_DF_BLOCK_SIZE, p_job->job_id);

        /** ECC Pub NVM data erase **/
        (void)MM_FLS_GetMaxSlotNum(FLS_DS_ECC_PUB_KEY, &fls_slot_num);
        (void)s_20_CR_FlsErase(fls_slot_num,
                                FLS_DS_ECC_PUB_KEY, 
                                (uint32_t)MH_KEY_ECC_PUB_DS_NB_BLOCKS * FLS_DF_BLOCK_SIZE, p_job->job_id);

        /** ECC Pub Ext NVM data erase **/
        (void)MM_FLS_GetMaxSlotNum(FLS_DS_ECC_PUB_EXT_KEY, &fls_slot_num);
        (void)s_20_CR_FlsErase(fls_slot_num,
                                FLS_DS_ECC_PUB_EXT_KEY, 
                                (uint32_t)MH_KEY_ECC_PUB_EXT_NB_BLOCKS * FLS_DF_BLOCK_SIZE, p_job->job_id);
    }
    else
    {
        /* Do nothing */
    }

    return(ret_err);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_20_CR_0009]:[ICUMFW_UD_S_20_CR_0009]
* Function name: s_20_CR_ProcessChallenge
* Description  : Challenge part of Service entry function of SERVICE_20_CR
* Arguments    : p_cr_Job (Pointer to the job structure)
*              : life_cycle
* Return Value : ER_OK                      
*              : ER_ML_ABTR_EMPTY           
*              : ER_ML_ABTR_NOT_RELEASED    
*              : ER_ML_ABTR_NOT_INITIALIZED
*              : ER_ML_ABTR_TIMER_SET_FAILURE 
*              : ER_ML_ABTR_SYS_ERROR
*              : ER_ML_PRNG_OK
*              : ER_ML_PRNG_TIMEOUT
*              : ER_ML_PRNG_RESEED
*              : ER_ML_PRNG_STATUS
*              : ER_ML_PRNG_PARAMETER
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_SLOT_EMPTY
*              : ER_MH_KEY_INVALID_KEY_GROUP
*              : ER_MH_KEY_INVALID_KEY_INDEX
*              : ER_MH_KEY_IN_USE
*              : ER_MH_KEY_NOT_USABLE
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
static uint32_t s_20_CR_ProcessChallenge(icum_job_t *p_cr_Job)
{
    uint32_t cr_key[NUM2][ANUM_PER_BLK] = 
    {
        {   /* CR_KEY */
            NUM0, NUM0, NUM0, NUM0
        },
        {   /* DEBUG_KEY_C */
            0x48530301uL, 0x00800045uL, 0x00000000uL, 0xB0000000uL
        }
    };
    uint32_t aes_zero[ANUM_PER_BLK] =
    {
        NUM0, NUM0, NUM0, NUM0
    };
    r_icumif_isd_t *p_ISD;
    ISD_CR_t *p_handler;
    aes_key_t *p_aes_key;
    uint32_t resource_id;
    uint32_t ret_err;
    uint32_t skip_flg;
    aes_all_at_once_t at_once_param;

    L_Toolbox_MemSet((uint8_t *)&at_once_param, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_t));
    L_Toolbox_MemSet((uint8_t *)g_challenge, (uint8_t)NUM0, (int16_t)sizeof(g_challenge));
    L_Toolbox_MemSet((uint8_t *)g_k_debug, (uint8_t)NUM0, (int16_t)sizeof(g_k_debug));

    p_ISD = p_cr_Job->p_ISD;
    p_handler = (ISD_CR_t*)&p_ISD->prm.RAND_GENERATE;
    
    ret_err = ER_OK;
    skip_flg = FLG_OFF;
    
    p_ISD->service_result = SERV_IN_PROCESS;

    /* If the deletion setting is deletion (0),                       */
    /* or the deletion setting is deletion (1)                      , */
    /* keys are deleted.                                              */
    /* If the deletion setting is deletion (3),                       */
    /* keys EXCEPT MASTER_ECU_KEY are deleted                         */
    if (D_CR_MAIN != p_handler->pe_type)
    {
        if ((NUM0 == g_cfg.cr_auth_dlt)
            || (NUM3 == g_cfg.cr_auth_dlt))
        {
            /** NVM data write protection check **/
            ret_err = s_20_CR_WriteProtectChk(p_cr_Job);
            if (ER_OK != ret_err)
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
    
    if (FLG_OFF == skip_flg)
    {
        ret_err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
        if (ER_OK != ret_err)
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
        /* initialization check and get for PRN */
        ret_err = ML_PRNG_Gen(g_challenge);
        (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        if (ER_ML_PRNG_OK != ret_err)
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
        /* C&R authorization key get */
        if ((D_CR_MAIN != p_handler->pe_type) && (NUM0 == g_cfg.cr_auth_key_type))  /* MASTER_ECU_KEY */
        {
            ret_err = MH_KEY_GetAesKey(p_cr_Job, KEY_GRP_AES,
                                       (r_key_index_t)NUM0, &p_aes_key, false);
        }
        else                                                                        /* Original key */
        {
            ret_err = MH_KEY_GetAesKey(p_cr_Job, KEY_GRP_AES_CR,
                                       (r_key_index_t)p_handler->pe_type, &p_aes_key, false);
        }
        if (ER_OK != ret_err)
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
        L_Toolbox_MemCpy((uint8_t*)cr_key, (uint8_t*)p_aes_key->value, (int16_t)sizeof(cr_key[NUM0]));
        (void)MH_KEY_DeleteRamKeyData(p_cr_Job, E_DELKEY_TYPE_TEMP);

        /* KDEBUG = KDF(CR_KEY, DEBUG_KEY_C) */
        ret_err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
        if (ER_OK != ret_err)
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
        g_ContextAddress          = AES_STATE_INIT;
        at_once_param.cipher_mode = D_AES_CIPHER_MODE_MP;
        at_once_param.direction   = (uint8_t)D_AES_DIR_ENCRYPTION;
        at_once_param.number      = (uint32_t)((NUM2 * BLOCK_BYTE_SIZE) / (uint32_t)AES_BLOCK_SIZE_IN_BYTES);
        at_once_param.key_type    = D_AES_KEYTYPE_PLAIN;
        at_once_param.p_aes_key   = aes_zero;
        at_once_param.key_length  = D_AES_KEY_SIZE_IN_BYTES_128BIT;
        at_once_param.p_aes_iv    = aes_zero;
        at_once_param.p_aes_src   = (uint32_t*)&cr_key[NUM0][NUM0];
        at_once_param.p_aes_dest  = (uint32_t*)g_k_debug;
        at_once_param.last_block_in_bits = (uint16_t)NUM0;
        at_once_param.async       = D_AES_TRANS_SYNC;
        D_AES_ExecutionAtOnce(&at_once_param);

        (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        L_Toolbox_MemSet((uint8_t*)cr_key, (uint8_t)NUM0, (int16_t)sizeof(cr_key[NUM0]));
        
        if (ER_D_AES_TIMEOUT_ERROR == ret_err)  /* Timeout */
        {
            skip_flg = FLG_ON;
        }
        else if (ER_PENDING == ret_err)
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
        /* Challenge data set */
        (void)D_CR_SetChallenge(p_handler->pe_type, g_challenge);
        p_ISD->service_result = SERV_OK;
    }
    else
    {
        /* Do nothing */
    }
    
    return (ret_err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_20_CR_0010]:[ICUMFW_UD_S_20_CR_0010]
* Function name: s_20_CR_ProcessResponse
* Description  : Response part of Service entry function of SERVICE_20_CR
* Arguments    : p_cr_Job (Pointer to the job structure)
*                life_cycle
* Return Value : ER_OK
*                ER_PARAM
*                ER_MM_FLS_SYS
*                ER_MM_FLS_SYS_CRITICAL
*                ER_MM_FLS_UNAVAILABLE
*                ER_MM_FLS_MEM_DEST
*                ER_MM_FLS_PROG
*                ER_MM_FLS_SIZE
*                ER_ML_ABTR_EMPTY           : No empty resources.
*                ER_ML_ABTR_NOT_RELEASED    : Resource not released.
*                ER_ML_ABTR_NOT_INITIALIZED : ABTR not initialized.
*                ER_ML_ABTR_TIMER_SET_FAILURE : OST DRV registration failure.
*                ER_ML_ABTR_SYS_ERROR       : System error.
*                R_SALIF_ER_CRY_TIMEOUT
*                R_SALIF_ER_PENDING
***************************************************************************************/
static uint32_t s_20_CR_ProcessResponse(icum_job_t *p_cr_Job)
{
    uint32_t key_zero[ANUM_PER_BLK] =
    {
        NUM0, NUM0, NUM0, NUM0
    };
    uint32_t authorization[NUM2][ANUM_PER_BLK];
    r_icumif_isd_t *p_ISD;
    ISD_CR_t *p_handler;
    uint32_t resource_id;
    uint8_t uid_size;
    volatile uint32_t ret_err;
    uint32_t ct;
    uint32_t skip_flg;
    uint32_t cmp_result;
    uint32_t msg_size_in_bits;
    uint16_t last_block;

    aes_all_at_once_t at_once_param;
    L_Toolbox_MemSet((uint8_t *)&at_once_param, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_t));

    p_ISD = p_cr_Job->p_ISD;
    p_handler = (ISD_CR_t*)&p_ISD->prm.RAND_GENERATE;
    
    ret_err = ER_OK;
    skip_flg = FLG_OFF;
    
    if (SERV_PENDING == p_ISD->service_result)
    {
        cmp_result = L_Toolbox_MemCmp((const uint8_t*)g_k_debug, (const uint8_t*)key_zero, (int16_t)sizeof(key_zero));
        /* If g_k_debug is zero, it means that an error has occurred in the Challenge process. */
        if (NUM0 == cmp_result)
        {
            p_ISD->service_result   = SERV_CR_VERIFY_FAIL;
            skip_flg = FLG_ON;
        }
        else
        {
            p_ISD->service_result = SERV_IN_PROCESS;

            ret_err = (uint32_t)ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
            if (ER_OK != ret_err)
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do nothing */
            }
        }
        
        if (FLG_OFF == skip_flg)
        {
            /* Response Compare(CMAC) */
            (void)MH_SYSINFO_GetChipId((uint8_t*)&g_challenge[ANUM_PER_BLK], &uid_size);
            msg_size_in_bits = (uint32_t)(NUM2 * BLOCK_BITS_SIZE) - (BLOCK_BITS_SIZE - (uid_size << NUM3));
            last_block = (uint16_t)(msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS);

            at_once_param.cipher_mode = D_AES_CIPHER_MODE_CMAC;
            at_once_param.direction   = D_AES_DIR_ENCRYPTION;
            at_once_param.number      = ((msg_size_in_bits + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1))
                                          / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
            at_once_param.key_type    = D_AES_KEYTYPE_PLAIN;
            if (NUM0 == (uint32_t)last_block)
            {
                if (NUM0 != msg_size_in_bits)
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
            g_ContextAddress         = AES_STATE_INIT;
            at_once_param.p_aes_key  = g_k_debug;
            at_once_param.key_length = D_AES_KEY_SIZE_IN_BYTES_128BIT;
            at_once_param.p_aes_iv   = NULL;
            at_once_param.p_aes_src  = (uint32_t*)g_challenge;
            at_once_param.p_aes_dest = (uint32_t*)authorization[NUM1];
            at_once_param.async = D_AES_TRANS_SYNC;
            D_AES_ExecutionAtOnce(&at_once_param);

            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
            
            if (ER_D_AES_TIMEOUT_ERROR == ret_err)  /* Timeout */
            {
                skip_flg = FLG_ON;
            }
            else if (ER_PENDING == ret_err)
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
            /* Response data get */
            (void)D_CR_GetResponse(authorization[NUM0]);
            ret_err = (uint32_t)L_Toolbox_MemCmp((const uint8_t*)authorization[NUM0],
                                       (const uint8_t*)authorization[NUM1],
                                       (int16_t)sizeof(authorization[NUM0]));
            if ((NUM0 != (uint32_t)ret_err) || (~NUM0 != ~(uint32_t)ret_err))
            {
                p_ISD->service_result   = SERV_CR_VERIFY_FAIL;
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
    
    if ((FLG_OFF == skip_flg) && (SERV_IN_PROCESS == p_ISD->service_result))
    {
        /* If the deletion setting is deletion (0),                       */
        /* or the deletion setting is deletion (1) and Life_cycle is LC2, */
        /* keys are deleted.                                              */
        /* If the deletion setting is deletion (3),                       */
        /* keys EXCEPT MASTER_ECU_KEY are deleted                         */
        if (D_CR_MAIN != p_handler->pe_type)
        {
            if ((NUM0 == g_cfg.cr_auth_dlt)
                || (NUM3 == g_cfg.cr_auth_dlt))
            {
                /* Key data erase */
                ret_err = (uint32_t)s_20_CR_EraseKey(p_cr_Job);
                if (ER_OK != ret_err)
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
        
        if (FLG_OFF == skip_flg)
        {
            if (D_CR_MAIN != p_handler->pe_type)
            {
                /* SECRET_DATA_SECTION erase */
                for (ct = (uint32_t)(&g_sds_start + NUM1); ct < (uint32_t)&g_sds_end; ct += sizeof(uint32_t))
                {
                    (*((uint32_t*)ct)) = NUM0;   /* Not use L_Toolbox_MemSet() */
                }

                /* SREG set */
                (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_RND_INIT, OFF);
                (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_INT_DEBUG, ON);
            }
            else
            {
                /* Do nothing */
            }

            /* Unlock */
            (void)D_CR_OpenDebugger(p_handler->pe_type);
            p_ISD->service_result = SERV_OK;
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
    
    return ((uint32_t)ret_err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_20_CR_0002]:[ICUMFW_UD_S_20_CR_0002]
* Function name: S_20_CR_ExecuteProcess
* Description  : Service entry function of SERVICE_20_CR
* Arguments    : p_cr_Job (Pointer to the job structure)
* Return Value : N/A
***************************************************************************************/
void S_20_CR_ExecuteProcess(icum_job_t *p_cr_Job)
{
    uint32_t ret_err = ER_OK;
    r_icumif_isd_t *p_ISD;
    ISD_CR_t *p_handler;
    uint32_t skip_flg = FLG_OFF;

    p_ISD = p_cr_Job->p_ISD;
    p_handler = (ISD_CR_t*)&p_ISD->prm.RAND_GENERATE;

    if ((SERV_REQUEST == p_ISD->service_result) || (SERV_PENDING == p_ISD->service_result))
    {
        if ((uint32_t)MISC_PEID_ICUP != (uint32_t)p_cr_Job->requester_id)
        {
            p_ISD->service_result = SERV_REQUESTER_ID_ERROR;
            skip_flg = FLG_ON;
        }
        else
        {
            p_ISD->service_result = SERV_PENDING;
        }
    }
    else
    {
        /* Do nothing */
    }
    
    if (FLG_OFF == skip_flg)
    {
        /* Challenge request */
        if (T_CHALLENGE == p_handler->trigger_type)
        {
            ret_err = s_20_CR_ProcessChallenge(p_cr_Job);
        }
        /* Response request */
        else
        {
            ret_err = s_20_CR_ProcessResponse(p_cr_Job);
        }
    }
    else
    {
        /* Do nothing */
    }
    
    (void)MM_CR_ClearState();
    /* Error code conversion */
    if (SERV_IN_PROCESS == p_ISD->service_result)
    {
        p_ISD->service_result = (r_icumif_service_result_t)s_20_CR_ConvertErr(ret_err);
    }
    else
    {
        /* Do nothing */
    }
    return;
}
