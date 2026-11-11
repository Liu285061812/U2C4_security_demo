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
#include "icum_d_comm_pub.h"
#include "icum_d_wdt_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_ml_prng_pub.h"
#include "icum_ml_cfg_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mm_cr_pub.h"
#include "icum_mh_key_pub.h"
#include "icum_mh_sysinfo_pub.h"
#include "icum_k_issd_pub.h"
#include "icum_s_pub.h"
#include "icum_s_02_rand_pub.h"
#include "icum_service_common.h"
/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"

/**********************************************************************************
* Define
***********************************************************************************/
#define NUM3            ((uint32_t)3)       /* value : 3 */
#define NUM4            ((uint32_t)4)       /* value : 4 */
#define NUM4096         ((uint32_t)4096)    /* value : 4096 */

#define ALIGN4          ((uint32_t)0x00000003)  /* 4byte alignment check */
#define OFF             ((uint32_t)0)           /* off(disable) */
#define ON              ((uint32_t)1)           /* on(enable) */
#define FLAG_ON         (1u)

/* Array */
#define ANUM_PER_BLK             (4u)   /* 32bit array number per block(128bit) */
/* Defines the job phase of RAND_TRNG service */
#define TRNG_PHASE_ON_HOLD       (0u)
#define TRNG_PHASE_GENERATE      (1u)
#define TRNG_PHASE_GET           (2u)
/* Defines the number of max block of RAND_TRNG service */
#define TRNG_GEN_MAX_BLOCK_NUM  (32u)


/**********************************************************************************
* Variable
***********************************************************************************/
static uint32_t g_trng_pre_init_start;
static uint32_t g_usage_status;

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t s_02_RAND_InitCommon(uint32_t trng_size, const icum_job_t *p_rand_Job,
                                     const uint32_t online_test);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_02_RNG_0003]:[ICUMFW_UD_S_02_RNG_0003]
* Function name: s_02_RAND_InitCommon
* Description  : Common processing of pseudo random number generator initialization 
*              : service
* Arguments    : trng_size (Pseudo random number size)
*              : p_rand_Job (Pointer to the job structure)
*              : online_test (Online test number)
* Return Value : ER_OK
***************************************************************************************/
static uint32_t s_02_RAND_InitCommon(uint32_t trng_size, const icum_job_t *p_rand_Job,
                                     const uint32_t online_test)
{
    uint32_t ret_err;
    uint32_t trng_status;

    if (SERV_REQUEST == p_rand_Job->p_ISD->service_result)
    {
        p_rand_Job->p_ISD->service_result = SERV_PENDING;
    }
    else
    {
        /* Do nothing */
    }

    if (SERV_PENDING == p_rand_Job->p_ISD->service_result)
    {
        /* Not use RNG service : Start seed generation */
        if (OFF == g_usage_status)
        {
            /* Generate seed */
            ret_err = ML_PRNG_GenTrng(online_test, trng_size);
            /* TRN error */
            if (ER_ML_PRNG_TRN == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_TRN_ERROR;
            }
            /* During background TRNG */
            else if (ER_ML_PRNG_STATUS == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_NO_COMP_TRNG;
            }
            else if (ER_ML_PRNG_PARAMETER == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_PRM_ERROR;
            }
            else if (ER_IN_PROCESS == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_IN_PROCESS;
                g_usage_status = ON;
            }
            else
            {
                p_rand_Job->p_ISD->service_result = SERV_OK;
                g_usage_status = ON;
            }
        }
        else
        {
            /* do nothing */
        }
    }
    else if(SERV_IN_PROCESS == p_rand_Job->p_ISD->service_result)
    {
        (void)ML_PRNG_GetTrngState(&trng_status);
        /* Error during seed generation */
        if (ML_PRNG_TRN_TEST_ERR == trng_status)
        {
            p_rand_Job->p_ISD->service_result = SERV_TRN_ERROR;
            g_usage_status = OFF;
        }
        else if (ML_PRNG_TRN_GENERATING == trng_status)
        {
            p_rand_Job->p_ISD->service_result = SERV_IN_PROCESS;
        }
        else
        {
            p_rand_Job->p_ISD->service_result = SERV_OK;
        }
    }
    else
    {
        p_rand_Job->p_ISD->service_result = SERV_SYS_ERROR;
    }
    
    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_02_RNG_0001]:[ICUMFW_UD_S_02_RNG_0001]
* Function name: S_02_RAND_Init
* Description  : Initialize the service
* Arguments    : N/A
* Return Value : SERV_OK
***************************************************************************************/
uint32_t S_02_RAND_Init(void)
{
    sys_cfg_rand_t  sys_cfg;

    sys_cfg.rand_trng_pre_init_start = NUM0;
    sys_cfg.rand_olt_no_ds_reset = NUM0;
    sys_cfg.rand_olt_ds_reset = NUM0;
    sys_cfg.rand_first_init_type = NUM0;

    (void)ML_CFG_GetSys(ML_CFG_SYS_RAND, (uint32_t*)&sys_cfg);
    g_trng_pre_init_start   = sys_cfg.rand_trng_pre_init_start;

    return (uint32_t)SERV_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_02_RNG_0002]:[ICUMFW_UD_S_02_RNG_0002]
* Function name: S_02_RAND_InitRng
* Description  : Service entry function of SERVICE_02_RAND_INIT_RNG
* Arguments    : p_rand_Job (Pointer to the job structure)
* Return Value : N/A
***************************************************************************************/
void S_02_RAND_InitRng(icum_job_t *p_rand_Job)
{
    uint32_t secret_key[ANUM_PER_BLK];
    uint32_t resource_id;
    uint32_t ret_err;
    sys_cfg_vm_t vm_cfg;
    uint32_t skip_flg;

    /* Initialize local variable */
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));
    skip_flg = FLG_OFF;

    if (SERV_REQUEST == p_rand_Job->p_ISD->service_result)
    {
        p_rand_Job->prm.job_aes.rem = OFF;
    }
    else
    {
        /* do nothing */
    }

    if (JOB_CANCEL_REQUESTED == (uint32_t)p_rand_Job->cancel_request)
    {
        p_rand_Job->p_ISD->service_result = SERV_OK;
        skip_flg = FLG_ON;

        if (ON == (uint32_t)p_rand_Job->prm.job_aes.rem)
        {
            /* If use RNG service in this job,  release status */
            g_usage_status = OFF;
        }
        else
        {
            /* do nothing */
        }
    }
     /* Online test selection in non-background */
    else if (NUM3 < (uint32_t)p_rand_Job->p_ISD->prm.RAND_INIT.online_test)
    {
        p_rand_Job->p_ISD->service_result = SERV_PRM_ERROR;
    }
    else if ((OFF == g_trng_pre_init_start)
        && (RAND_NO_TEST != p_rand_Job->p_ISD->prm.RAND_INIT.online_test))
    {
        (void)s_02_RAND_InitCommon(ML_TRNG_SIZE_TEST, p_rand_Job,
                                   (uint32_t)p_rand_Job->p_ISD->prm.RAND_INIT.online_test);
    }
    else
    {
        (void)s_02_RAND_InitCommon(ML_TRNG_SIZE_SHE, p_rand_Job, NUM0);
    }

    if ((FLG_OFF == skip_flg) 
    && ((SERV_IN_PROCESS == p_rand_Job->p_ISD->service_result)
    || (SERV_OK == p_rand_Job->p_ISD->service_result)))
    {
        if (OFF == (uint32_t)p_rand_Job->prm.job_aes.rem)
        {
            p_rand_Job->prm.job_aes.rem = ON;
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

    if ((FLG_OFF == skip_flg) && (SERV_OK == p_rand_Job->p_ISD->service_result))
    {
        /* Complete seed generation : Get AES context */
        ret_err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
        if (ER_OK == ret_err)
        {
            (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
            /* Done seed generation */
            ret_err = MH_KEY_GetSecretKey(vm_cfg.privilege_vm_id, secret_key);
            if (ER_MH_KEY_AES_TIMEOUT_ERROR == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_AES_TIMEOUT_ERROR;
            }
            else if (ER_MH_KEY_AES_PRM_ERROR == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_AES_PRM_ERROR;
            }
            else if (ER_PENDING == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_PENDING;
            }
            else if (ER_PARAM == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_PRM_ERROR;
            }
            /* ret_err == ER_OK */
            else
            {
                ret_err = ML_PRNG_InitSHE(secret_key);
                L_Toolbox_MemSet((uint8_t*)secret_key, (uint8_t)NUM0, (int16_t)sizeof(secret_key));
                if (ER_ML_PRNG_TIMEOUT == ret_err)
                {
                    p_rand_Job->p_ISD->service_result = SERV_AES_TIMEOUT_ERROR;
                }
                else if (ER_PENDING == ret_err)
                {
                    p_rand_Job->p_ISD->service_result = SERV_PENDING;
                }
                else
                {
                    p_rand_Job->p_ISD->service_result = SERV_OK;
                }
            }
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
            g_usage_status = OFF;
        }
        else if (ER_ML_ABTR_EMPTY == ret_err)
        {
            p_rand_Job->p_ISD->service_result = SERV_PENDING;
        }
        else if(ER_PARAM == ret_err)
        {
            p_rand_Job->p_ISD->service_result = SERV_PRM_ERROR;
            g_usage_status = OFF;
        }
        else
        {
            p_rand_Job->p_ISD->service_result = SERV_SYS_ERROR;
            g_usage_status = OFF;
        }
    }
    else
    {
        /* do nothing */
    }
    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_02_RNG_0004]:[ICUMFW_UD_S_02_RNG_0004]
* Function name: S_02_RAND_ExtendSeed
* Description  : Service entry function of SERVICE_02_RAND_EXTEND_SEED
* Arguments    : p_rand_Job (Pointer to the job structure)
* Return Value : N/A
***************************************************************************************/
void S_02_RAND_ExtendSeed(icum_job_t *p_rand_Job)
{
    uint32_t ret_err;
    uint8_t mem_check_result;
    uint32_t resource_id;
    uint32_t skip_flg = FLG_OFF;
    remap_ptr_manip_t   source_logical;
    
    resource_id = NUM0;

    if (JOB_CANCEL_REQUESTED == (uint32_t)p_rand_Job->cancel_request)
    {
        p_rand_Job->p_ISD->service_result = SERV_OK;
        skip_flg = FLG_ON;

        if (ON == (uint32_t)p_rand_Job->prm.job_aes.rem)
        {
            /* If use RNG service in this job,  release status */
            g_usage_status = OFF;
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

    if ((FLG_OFF == skip_flg) && (SERV_REQUEST == p_rand_Job->p_ISD->service_result))
    {
        p_rand_Job->prm.job_aes.p_source = p_rand_Job->p_ISD->prm.RAND_EXTEND_SEED.p_entropy;
        if (NUM0 != (ALIGN4 & (uint32_t)p_rand_Job->prm.job_aes.p_source))
        {
            p_rand_Job->p_ISD->service_result = SERV_PRM_ERROR;
            skip_flg = FLG_ON;
        }
        else
        {
            if ((uint32_t)FROM_NATIVE_SERVICE == (uint32_t)p_rand_Job->expansion_service)
            {
                (void)MM_FLS_CheckMemory((uint32_t)p_rand_Job->prm.job_aes.p_source, (uint32_t)AES_BLOCK_SIZE_IN_BYTES,
                                          FLS_CHK_MEM_SOURCE, p_rand_Job->expansion_service, &mem_check_result);
            }
            else /* FROM_EXPANSION_SERVICE */
            {
                (void)MM_FLS_CheckMemoryCISM((uint32_t)p_rand_Job->prm.job_aes.p_source, 
                                            (uint32_t)AES_BLOCK_SIZE_IN_BYTES,
                                              FLS_CHK_MEM_SOURCE, &mem_check_result);
            }
            if ((uint32_t)FLS_CHK_OK != (uint32_t)mem_check_result)
            {
                p_rand_Job->p_ISD->service_result = SERV_SRC_MEM_ERROR;
                skip_flg = FLG_ON;
            }
            else
            {
                /* do nothing */
            }
        }

        if (FLG_OFF == skip_flg)
        {
            p_rand_Job->prm.job_aes.rem = (uint8_t)OFF;
            p_rand_Job->p_ISD->service_result = SERV_PENDING;
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

    if ((FLG_OFF == skip_flg) && (SERV_PENDING == p_rand_Job->p_ISD->service_result))
    {
        /* Not use RNG service : Get AES context */
        if (OFF == g_usage_status)
        {
            g_usage_status = ON;
            p_rand_Job->prm.job_aes.rem = (uint8_t)ON;
        }
        else
        {
            /* do nothing */
        }
        /* Top priority of the service */
        if (OFF != (uint32_t)p_rand_Job->prm.job_aes.rem)
        {
            /* Complete seed generation : Get AES context */
            ret_err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
            if (ER_OK == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_IN_PROCESS;
            }
            else if (ER_ML_ABTR_EMPTY == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_PENDING;
                skip_flg = FLG_ON;
            }
            else if(ER_PARAM == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_PRM_ERROR;
                g_usage_status = OFF;
                skip_flg = FLG_ON;
            }
            else
            {
                p_rand_Job->p_ISD->service_result = SERV_SYS_ERROR;
                g_usage_status = OFF;
                skip_flg = FLG_ON;
            }
        }
        else
        {
            /* do nothing */
        }
        if ((FLG_OFF == skip_flg) && (SERV_PENDING == p_rand_Job->p_ISD->service_result))
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
#if defined(FSW_REMAP_DRIVER)
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_rand_Job->prm.job_aes.p_source, &source_logical.val);
#else   /* defined(FSW_REMAP_DRIVER) */
        source_logical.p_ptr = (uint8_t *)p_rand_Job->prm.job_aes.p_source;
#endif  /* defined(FSW_REMAP_DRIVER) */

        ret_err = ML_PRNG_ExtendSHE((const uint32_t *)source_logical.p_ptr);
        (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        g_usage_status = OFF;
        if (ER_ML_PRNG_STATUS == ret_err)
        {
            p_rand_Job->p_ISD->service_result = SERV_NO_INIT_RNG;
        }
        else if (ER_ML_PRNG_TIMEOUT == ret_err)
        {
            p_rand_Job->p_ISD->service_result = SERV_AES_TIMEOUT_ERROR;
        }
        else if (ER_PENDING == ret_err)
        {
            p_rand_Job->p_ISD->service_result = SERV_PENDING;
        }
        else
        {
            p_rand_Job->p_ISD->service_result = SERV_OK;
            (void)MM_CR_InitCompletionNotice();
        }
    }
    else
    {
        /* do nothing */
    }
    
    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_02_RNG_0005]:[ICUMFW_UD_S_02_RNG_0005]
* Function name: S_02_RAND_GenerateRnd
* Description  : Service entry function of SERVICE_02_RAND_RND
* Arguments    : p_rand_Job (Pointer to the job structure)
* Return Value : N/A
***************************************************************************************/
void S_02_RAND_GenerateRnd(icum_job_t *p_rand_Job)
{
    uint32_t ret_err;
    uint8_t mem_check_result;
    uint32_t resource_id;
    uint32_t skip_flg = FLG_OFF;
    remap_ptr_manip_t   dest_logical;

    resource_id = NUM0;

    if (JOB_CANCEL_REQUESTED == (uint32_t)p_rand_Job->cancel_request)
    {
        p_rand_Job->p_ISD->service_result = SERV_OK;
        skip_flg = FLG_ON;

        if (ON == (uint32_t)p_rand_Job->prm.job_aes.rem)
        {
            /* If use RNG service in this job,  release status */
            g_usage_status = OFF;
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

    if ((FLG_OFF == skip_flg) && (SERV_REQUEST == p_rand_Job->p_ISD->service_result))
    {
        p_rand_Job->prm.job_aes.p_dest       = p_rand_Job->p_ISD->prm.RAND_GENERATE.p_block_out;
        p_rand_Job->prm.job_aes.nb_blocks    = p_rand_Job->p_ISD->prm.RAND_GENERATE.nb_blocks;
        p_rand_Job->prm.job_aes.nb_slice     = p_rand_Job->p_ISD->prm.RAND_GENERATE.job_slice;

        if ((NUM0 != (ALIGN4 & (uint32_t)p_rand_Job->prm.job_aes.p_dest))
        || ((NUM1 != p_rand_Job->prm.job_aes.nb_blocks) || (NUM1 < (uint32_t)p_rand_Job->prm.job_aes.nb_slice)))
        {
            p_rand_Job->p_ISD->service_result = SERV_PRM_ERROR;
            skip_flg = FLG_ON;
        }
        else
        {
            /* do nothing */
        }

        if (FLG_OFF == skip_flg)
        {
            if((uint32_t)FROM_NATIVE_SERVICE == (uint32_t)p_rand_Job->expansion_service)
            {
                (void)MM_FLS_CheckMemory((uint32_t)p_rand_Job->prm.job_aes.p_dest,
                                          (p_rand_Job->prm.job_aes.nb_blocks * (uint32_t)AES_BLOCK_SIZE_IN_BYTES),
                                          FLS_CHK_MEM_DESTINATION, p_rand_Job->expansion_service, &mem_check_result);
            }
            else /* FROM_EXPANSION_SERVICE */
            {
                (void)MM_FLS_CheckMemoryCISM((uint32_t)p_rand_Job->prm.job_aes.p_dest,
                                          (p_rand_Job->prm.job_aes.nb_blocks * (uint32_t)AES_BLOCK_SIZE_IN_BYTES),
                                          FLS_CHK_MEM_DESTINATION, &mem_check_result);
            }
            
            if ((uint32_t)FLS_CHK_OK != (uint32_t)mem_check_result)
            {
                p_rand_Job->p_ISD->service_result = SERV_DEST_MEM_ERROR;
                skip_flg = FLG_ON;
            }
            else
            {
                p_rand_Job->prm.job_aes.rem = (uint8_t)OFF;
                p_rand_Job->p_ISD->service_result = SERV_PENDING;
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

    if ((FLG_OFF == skip_flg) && (SERV_PENDING == p_rand_Job->p_ISD->service_result))
    {
        /* Not use RNG service : Get AES context */
        if (OFF == g_usage_status)
        {
            g_usage_status = ON;
            p_rand_Job->prm.job_aes.rem = (uint8_t)ON;
        }
        else
        {
            /* do nothing */
        }
        /* Top priority of the service */
        if (OFF != (uint32_t)p_rand_Job->prm.job_aes.rem)
        {
            /* Complete seed generation : Get AES context */
            ret_err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
            if (ER_OK == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_IN_PROCESS;
            }
            else if (ER_ML_ABTR_EMPTY == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_PENDING;
                skip_flg = FLG_ON;
            }
            else if(ER_PARAM == ret_err)
            {
                p_rand_Job->p_ISD->service_result = SERV_PRM_ERROR;
                g_usage_status = OFF;
                skip_flg = FLG_ON;
            }
            else
            {
                p_rand_Job->p_ISD->service_result = SERV_SYS_ERROR;
                g_usage_status = OFF;
                skip_flg = FLG_ON;
            }
        }
        else
        {
            /* do nothing */
        }

        if ((FLG_OFF == skip_flg) && (SERV_PENDING == p_rand_Job->p_ISD->service_result))
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
#if defined(FSW_REMAP_DRIVER)
        /* Get Remap(Convert Physical address -> Logical Address) */
        (void)D_REMAP_GetLogicalAddrs((uint32_t)p_rand_Job->prm.job_aes.p_dest, &dest_logical.val);
#else   /* defined(FSW_REMAP_DRIVER) */
        dest_logical.p_ptr = (uint8_t *)p_rand_Job->prm.job_aes.p_dest;
#endif  /* defined(FSW_REMAP_DRIVER) */

        ret_err = ML_PRNG_GenSHE((uint32_t *)dest_logical.p_ptr);
        if (ER_ML_PRNG_STATUS == ret_err)
        {
            p_rand_Job->p_ISD->service_result = SERV_NO_INIT_RNG;
        }
        else if (ER_ML_PRNG_RESEED == ret_err)
        {
            p_rand_Job->p_ISD->service_result = SERV_NEED_RESEED;
        }
        else if (ER_ML_PRNG_TIMEOUT == ret_err)
        {
            p_rand_Job->p_ISD->service_result = SERV_AES_TIMEOUT_ERROR;
        }
        else if (ER_PENDING == ret_err)
        {
            p_rand_Job->p_ISD->service_result = SERV_PENDING;
        }
        else
        {
            p_rand_Job->p_ISD->service_result = SERV_OK;
        }
        (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        g_usage_status = OFF;
    }
    else
    {
        /* do nothing */
    }
    return;
}
