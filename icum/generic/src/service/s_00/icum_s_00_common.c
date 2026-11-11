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
#include "icum_common_pub.h"
#include "r_icumif_api.h"
#include "icum_s_00_common.h"
#include "icum_d_comm_pub.h"
#include "icum_d_info_pub.h"
#include "icum_d_cry_aes_pub.h"
#include "icum_k_issd_pub.h"
#include "icum_ml_prng_pub.h"
#include "icum_l_cry_pk_pub.h"
#include "icum_ml_ctx_pub.h"
/* For the error codes of ML_CFG */
#include "icum_ml_cfg_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mh_key_pub.h"
#include "icum_mh_sysinfo_pub.h"

#pragma ghs section text=".icum_service_ram"
#pragma ghs section rodata=".icum_service_const_ram"

/**********************************************************************************
* ISM Common definition
***********************************************************************************/

/* Error code mask filder(Kernel layer)  */
#define MASK_KERNEL_ERR  0xF0000000UL

/* Error code mask filder(Middle layer)  */
#define MASK_MID_ERR  0x00F00000UL

/* Error code mask filder(Library layer)  */
#define MASK_LIB_ERR  0x000F0000UL

/* Error code mask filder(Driver layer)  */
#define MASK_DRV_ERR  0x00000F00UL

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t s_00_ConvertErrInfo(uint32_t err);
static uint32_t s_00_ConvertErrPrng(uint32_t err);
static uint32_t s_00_ConvertErrBignum(uint32_t err);
static uint32_t s_00_ConvertErrCtx(uint32_t err);
static uint32_t s_00_ConvertErrAbtr(uint32_t err);
static uint32_t s_00_ConvertErrFls(uint32_t err);
static uint32_t s_00_ConvertErrCfg(uint32_t err);
static uint32_t s_00_ConvertErrKey(uint32_t err);
static uint32_t s_00_ConvertErrSysinfo(uint32_t err);
static uint32_t s_00_ConvertErrLib(uint32_t err);
static uint32_t s_00_ConvertErrMid(uint32_t err);
static uint32_t s_00_ConvertErrDrv(uint32_t err);
static uint32_t s_00_ConvertErrIssd(uint32_t err);
static uint32_t s_00_ConvertErrKernel(uint32_t err);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0046]:[ICUMFW_UD_S_00_SystemConfig_0046]
* Function name: s_00_ConvertErrInfo()
* Description  : Error Convert of Info Driver
* Arguments    : err : Error code to be converted
* Return Value : SERV_UNAVAILABLE
*              : SERV_SYS_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrInfo(uint32_t err)
{
    uint32_t    err2;
    /* Error Conversion */
    switch(err)
    {
        case ER_D_INFO_INVALID_STATE:
            err2 = (uint32_t)SERV_UNAVAILABLE;
            break;
        default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0048]:[ICUMFW_UD_S_00_SystemConfig_0048]
* Function name: s_00_ConvertErrPrng()
* Description  : Error Convert of CRY RND Library
* Arguments    : err : Error code to be converted
* Return Value : SERV_NO_INIT_RNG
*              : SERV_NEED_RESEED
*              : SERV_AES_TIMEOUT_ERROR
*              : SERV_SYS_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrPrng(uint32_t err)
{
    uint32_t    err2;

    /* Error Conversion */
    switch(err)
    {
        case ER_ML_PRNG_STATUS:
            err2 = (uint32_t)SERV_NO_INIT_RNG;
            break;
        case ER_ML_PRNG_RESEED:
            err2 = (uint32_t)SERV_NEED_RESEED;
            break;
        case ER_ML_PRNG_TIMEOUT:
            err2 = (uint32_t)SERV_AES_TIMEOUT_ERROR;
            break;
        default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0051]:[ICUMFW_UD_S_00_SystemConfig_0051]
* Function name: s_00_ConvertErrBignum()
* Description  : Error Convert of CRY BIGNUM Library
* Arguments    : err : Error code to be converted
* Return Value : SERV_PRM_ERROR
*              : SERV_SYS_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrBignum(uint32_t err)
{
    uint32_t    err2;

    /* Error Conversion */
    switch(err)
    {
        case ER_L_BIGNUM_INVALID_ARG:
        case ER_L_BIGNUM_BUFFER_OVERFLOW:
        case ER_L_BIGNUM_VAL:
            err2 = (uint32_t)SERV_PRM_ERROR;
            break;
        default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0052]:[ICUMFW_UD_S_00_SystemConfig_0052]
* Function name: s_00_ConvertErrCtx()
* Description  : Error Convert of CTX Manager
* Arguments    : err : Error code to be converted
* Return Value : SERV_PENDING
*              : SERV_SYS_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrCtx(uint32_t err)
{
    uint32_t    err2;

    /* Error Conversion */
    switch(err)
    {
        case ER_ML_CTX_ALL_USE:
            err2 = (uint32_t)SERV_PENDING;
            break;
        case ER_ML_CTX_NOT_FOUND:
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
        default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0053]:[ICUMFW_UD_S_00_SystemConfig_0053]
* Function name: s_00_ConvertErrAbtr()
* Description  : Error Convert of ABTR Manager
* Arguments    : err : Error code to be converted
* Return Value : SERV_PENDING
*              : SERV_SYS_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrAbtr(uint32_t err)
{
    uint32_t    err2;

    /* Error Conversion */
    switch(err)
    {
        case ER_ML_ABTR_EMPTY:
            err2 = (uint32_t)SERV_PENDING;
            break;
        case ER_ML_ABTR_NOT_RELEASED:
        case ER_ML_ABTR_NOT_INITIALIZED:
        case ER_ML_ABTR_TIMER_SET_FAILURE:
        case ER_ML_ABTR_INVALID_ID:
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
        default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0054]:[ICUMFW_UD_S_00_SystemConfig_0054]
* Function name: s_00_ConvertErrFls()
* Description  : Error Convert of FLASH Manager
* Arguments    : err : Error code to be converted
* Return Value : SERV_NVM_SYS_ERROR
*              : SERV_NVM_SYS_ERROR_CRITICAL
*              : SERV_NVM_PROG_ERROR
*              : SERV_NVM_UNAVAILABLE
*              : SERV_NVM_SIZE_ERROR
*              : SERV_DEST_MEM_ERROR
*              : SERV_IN_PROCESS
*              : SERV_SYS_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrFls(uint32_t err)
{
    uint32_t    err2;

    /* Error Conversion */
    switch(err)
    {
        case ER_MM_FLS_SYS:
            err2 = (uint32_t)SERV_NVM_SYS_ERROR;
            break;
        case ER_MM_FLS_SYS_CRITICAL:
            err2 = (uint32_t)SERV_NVM_SYS_ERROR_CRITICAL;
            break;
        case ER_MM_FLS_PROG:
            err2 = (uint32_t)SERV_NVM_PROG_ERROR;
            break;
        case ER_MM_FLS_UNAVAILABLE:
            err2 = (uint32_t)SERV_NVM_UNAVAILABLE;
            break;
        case ER_MM_FLS_SIZE:
            err2 = (uint32_t)SERV_NVM_SIZE_ERROR;
            break;
        case ER_MM_FLS_MEM_DEST:
            err2 = (uint32_t)SERV_DEST_MEM_ERROR;
            break;
        case ER_MM_FLS_IN_PROGRESS:
            err2 = (uint32_t)SERV_IN_PROCESS;
            break;
        default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0074]:[ICUMFW_UD_S_00_SystemConfig_0074]
* Function name: s_00_ConvertErrCfg()
* Description  : Error Convert of KEY Manager
* Arguments    : err : Error code to be converted
* Return Value : SERV_RAM_SIZE_ERROR
*              : SERV_NVM_SIZE_ERROR
*              : SERV_SYS_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrCfg(uint32_t err)
{
    uint32_t err2;

    /* Error Conversion */
    switch(err)
    {
        case ER_ML_CFG_RAM_SIZE_ERROR:
            err2 = (uint32_t)SERV_RAM_SIZE_ERROR;
            break;
        case ER_ML_CFG_NVM_SIZE_ERROR:
            err2 = (uint32_t)SERV_NVM_SIZE_ERROR;
            break;
         default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0055]:[ICUMFW_UD_S_00_SystemConfig_0055]
* Function name: s_00_ConvertErrKey()
* Description  : Error Convert of KEY Manager
* Arguments    : err : Error code to be converted
* Return Value : SERV_KEY_SLOT_EMPTY
*              : SERV_INVALID_KEY_GROUP
*              : SERV_INVALID_KEY_INDEX
*              : SERV_KEY_NOT_USABLE
*              : SERV_KEY_WRITE_PROTECTED
*              : SERV_KEY_CACHE_FULL
*              : SERV_NVM_UNAVAILABLE
*              : SERV_NVM_SYS_ERROR_CRITICAL
*              : SERV_NVM_SYS_ERROR
*              : SERV_DATA_INVALID
*              : SERV_NVM_SIZE_ERROR
*              : SERV_SRC_MEM_ERROR
*              : SERV_DEST_MEM_ERROR
*              : SERV_SYS_ERROR
*              : SERV_AES_PRM_ERROR
*              : SERV_PENDING
*              : SERV_NVM_PROG_ERROR
*              : SERV_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrKey(uint32_t err)
{
    uint32_t    err2;

    /* Error Conversion */
    switch(err)
    {
        case ER_MH_KEY_SLOT_EMPTY:
            err2 = (uint32_t)SERV_KEY_SLOT_EMPTY;
            break;
        case ER_MH_KEY_INVALID_KEY_GROUP:
            err2 = (uint32_t)SERV_INVALID_KEY_GROUP;
            break;
        case ER_MH_KEY_INVALID_KEY_INDEX:
            err2 = (uint32_t)SERV_INVALID_KEY_INDEX;
            break;
        case ER_MH_KEY_IN_USE:
            err2 = (uint32_t)SERV_KEY_NOT_USABLE;
            break;
        case ER_MH_KEY_WRITE_PROTECTED:
            err2 = (uint32_t)SERV_KEY_WRITE_PROTECTED;
            break;
        case ER_MH_KEY_USER_OVER:
        case ER_MH_KEY_NOT_USABLE:
            err2 = (uint32_t)SERV_KEY_NOT_USABLE;
            break;
        case ER_MH_KEY_NVM_UNAVAILABLE:
            err2 = (uint32_t)SERV_NVM_UNAVAILABLE;
            break;
        case ER_MH_KEY_NVM_SYS_CRITICAL:
            err2 = (uint32_t)SERV_NVM_SYS_ERROR_CRITICAL;
            break;
        case ER_MH_KEY_NVM_SYS:
            err2 = (uint32_t)SERV_NVM_SYS_ERROR;
            break;
        case ER_MH_KEY_DATA_INVALID:
            err2 = (uint32_t)SERV_DATA_INVALID;
            break;
        case ER_MH_KEY_NVM_SIZE:
            err2 = (uint32_t)SERV_NVM_SIZE_ERROR;
            break;
        case ER_MH_KEY_SRC_MEM_ERROR:
            err2 = (uint32_t)SERV_SRC_MEM_ERROR;
            break;
        case ER_MH_KEY_DEST_MEM_ERROR:
            err2 = (uint32_t)SERV_DEST_MEM_ERROR;
            break;
        case ER_MH_KEY_SYS_ERROR:
        case ER_MH_KEY_DATA_NOT_FOUND:
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
        case ER_MH_KEY_AES_PRM_ERROR:
            err2 = (uint32_t)SERV_AES_PRM_ERROR;
            break;
        case ER_MH_KEY_WRITING:
            err2 = (uint32_t)SERV_PENDING;
            break;
        case ER_MH_KEY_NVM_PROG:
            err2 = (uint32_t)SERV_NVM_PROG_ERROR;
            break;
        case ER_MH_KEY_AES_TIMEOUT_ERROR:
            err2 = (uint32_t)SERV_AES_TIMEOUT_ERROR;
            break;
        default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0057]:[ICUMFW_UD_S_00_SystemConfig_0057]
* Function name: s_00_ConvertErrSysinfo()
* Description  : Error Convert of SYSLC Manager
* Arguments    : err : Error code to be converted
* Return Value : SERV_NVM_SYS_ERROR
*              : SERV_NVM_SYS_ERROR_CRITICAL
*              : SERV_NVM_PROG_ERROR
*              : SERV_DATA_INVALID
*              : SERV_NVM_UNAVAILABLE
*              : SERV_SYS_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrSysinfo(uint32_t err)
{
    uint32_t    err2;

    /* Error Conversion */
    switch(err)
    {
        case ER_MH_SYSINFO_NVM_SYS:
            err2 = (uint32_t)SERV_NVM_SYS_ERROR;
            break;
        case ER_MH_SYSINFO_NVM_SYS_CRITICAL:
            err2 = (uint32_t)SERV_NVM_SYS_ERROR_CRITICAL;
            break;
        case ER_MH_SYSINFO_NVM_PROG:
            err2 = (uint32_t)SERV_NVM_PROG_ERROR;
            break;
        case ER_MH_SYSINFO_NVM_DATA_INVALID:
            err2 = (uint32_t)SERV_DATA_INVALID;
            break;
        case ER_MH_SYSINFO_NVM_UNAVAILABLE:
            err2 = (uint32_t)SERV_NVM_UNAVAILABLE;
            break;
        case ER_MH_SYSINFO_UNKNOWN:
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
        default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0058]:[ICUMFW_UD_S_00_SystemConfig_0058]
* Function name: s_00_ConvertErrLib()
* Description  : Service 00 Library Error Convert
* Arguments    : err : Error code to be converted
* Return Value : SERV_AES_PRM_ERROR
*              : SERV_AES_TIMEOUT_ERROR
*              : SERV_PRM_ERROR
*              : SERV_CRYPT_ERROR
*              : SERV_PENDING
*              : SERV_SYS_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrLib(uint32_t err)
{
    uint32_t    err_base;
    uint32_t    err2;

    err_base = err & MASK_LIB_ERR;

    /* Error Conversion */
    switch(err_base)
    {
        case ER_L_CRY_BIGNUM_BASE:
            err2 = s_00_ConvertErrBignum(err);
            break;
        default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0059]:[ICUMFW_UD_S_00_SystemConfig_0059]
* Function name: s_00_ConvertErrMid()
* Description  : Service 00 Middle Error Convert
* Arguments    : err : Error code to be converted
* Return Value : SERV_PENDING
*              : SERV_PRM_ERROR
*              : SERV_UNAVAILABLE
*              : SERV_SYS_ERROR
*              : SERV_NVM_SYS_ERROR
*              : SERV_NVM_SYS_ERROR_CRITICAL
*              : SERV_NVM_PROG_ERROR
*              : SERV_NVM_UNAVAILABLE
*              : SERV_NVM_SIZE_ERROR
*              : SERV_IN_PROCESS
*              : SERV_KEY_SLOT_EMPTY
*              : SERV_INVALID_KEY_GROUP
*              : SERV_INVALID_KEY_INDEX
*              : SERV_KEY_NOT_USABLE
*              : SERV_KEY_WRITE_PROTECTED
*              : SERV_KEY_CACHE_FULL
*              : SERV_DATA_INVALID
*              : SERV_SRC_MEM_ERROR
*              : SERV_DEST_MEM_ERROR
*              : SERV_INVALID_MEM_CLUST_INDEX
*              : SERV_CLUSTER_VERIF_STATUS_LOCK
*              : SERV_CLUSTER_NOT_DEFINED
*              : SERV_AES_TIMEOUT_ERROR
*              : SERV_AES_PRM_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrMid(uint32_t err)
{
    uint32_t    err_base;
    uint32_t    err2;

    err_base = err & MASK_MID_ERR;

    /* Error Conversion */
    switch(err_base)
    {
        case ER_ML_CTX_BASE:
            err2 = s_00_ConvertErrCtx(err);
            break;
        case ER_ML_ABTR_BASE:
            err2 = s_00_ConvertErrAbtr(err);
            break;
        case ER_MM_FLS_BASE:
            err2 = s_00_ConvertErrFls(err);
            break;
        case ER_ML_CFG_BASE:
            err2 = s_00_ConvertErrCfg(err);
            break;
        case ER_MH_KEY_BASE:
            err2 = s_00_ConvertErrKey(err);
            break;
        case ER_MH_SYSINFO_BASE:
            err2 = s_00_ConvertErrSysinfo(err);
            break;
        case ER_ML_PRNG_BASE:
            err2 = s_00_ConvertErrPrng(err);
            break;
        default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0060]:[ICUMFW_UD_S_00_SystemConfig_0060]
* Function name: s_00_ConvertErrDrv()
* Description  : Service 00 Driver Error Convert
* Arguments    : err : Error code to be converted
* Return Value : SERV_UNAVAILABLE
*              : SERV_SYS_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrDrv(uint32_t err)
{
    uint32_t    err_base;
    uint32_t    err2;

    err_base = err & MASK_DRV_ERR;

    /* Driver Error Conversion */
    switch(err_base)
    {
        case ER_D_INFO_BASE:
            err2 = s_00_ConvertErrInfo(err);
            break;
        default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0061]:[ICUMFW_UD_S_00_SystemConfig_0061]
* Function name: s_00_ConvertErrIssd()
* Description  : Error Convert of ISSD
* Arguments    : err : Error code to be converted
* Return Value : SERV_OK
*              : SERV_SYS_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrIssd(uint32_t err)
{
    uint32_t    err2;

    /* Error Conversion */
    switch(err)
    {
        case ER_K_ISSD_JOB_NOT_FOUND:
            err2 = (uint32_t)SERV_SEQUENCE_ERROR;
            break;
        default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0062]:[ICUMFW_UD_S_00_SystemConfig_0062]
* Function name: s_00_ConvertErrKernel()
* Description  : Service 00 Kernel Error Convert
* Arguments    : err : Error code to be converted
* Return Value : SERV_OK
*              : SERV_SYS_ERROR
***************************************************************************************/
static uint32_t s_00_ConvertErrKernel(uint32_t err)
{
    uint32_t    err_base;
    uint32_t    err2;

    err_base = err & MASK_KERNEL_ERR;

    /* Driver Error Conversion */
    switch(err_base)
    {
        case ER_K_ISSD_BASE:
            err2 = s_00_ConvertErrIssd(err);
            break;
        default:
            /* Never occur */
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    return err2;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_00_SystemConfig_0045]:[ICUMFW_UD_S_00_SystemConfig_0045]
* Function name: s_00_ConvertErr()
* Description  : Service 00 Error Convert
* Arguments    : err : Error code to be converted
* Return Value : SERV_OK
*              : SERV_PRM_ERROR
*              : SERV_PENDING
*              : SERV_IN_PROCESS
*              : SERV_NVM_SIZE_ERROR
*              : SERV_SYS_CFG_NOT_INITIALIZED
*              : SERV_UNAVAILABLE
*              : SERV_DEST_MEM_ERROR
*              : SERV_SRC_MEM_ERROR
*              : SERV_FW_ACTIVATION_FAILED
*              : SERV_CR_VERIFY_FAIL
*              : SERV_AES_PRM_ERROR
*              : SERV_AES_TIMEOUT_ERROR
*              : SERV_NO_INIT_RNG
*              : SERV_NEED_RESEED
*              : SERV_CRYPT_ERROR
*              : SERV_SYS_ERROR
*              : SERV_NVM_SYS_ERROR
*              : SERV_NVM_SYS_ERROR_CRITICAL
*              : SERV_NVM_PROG_ERROR
*              : SERV_NVM_UNAVAILABLE
*              : SERV_NVM_SIZE_ERROR
*              : SERV_KEY_SLOT_EMPTY
*              : SERV_INVALID_KEY_GROUP
*              : SERV_INVALID_KEY_INDEX
*              : SERV_KEY_NOT_USABLE
*              : SERV_KEY_WRITE_PROTECTED
*              : SERV_KEY_CACHE_FULL
*              : SERV_DATA_INVALID
***************************************************************************************/
uint32_t s_00_ConvertErr(uint32_t err)
{
    uint32_t    err2 = (uint32_t)SERV_SYS_ERROR;

    /* Checking Normal sequence */
    switch(err)
    {
        case ER_OK:
            err2 = (uint32_t)SERV_OK;
            break;
        case ER_PARAM:
            err2 = (uint32_t)SERV_PRM_ERROR;
            break;
        case ER_PENDING_ARB:
            err2 = (uint32_t)SERV_PENDING;
            break;
        case ER_PENDING:
            err2 = (uint32_t)SERV_PENDING;
            break;
        case ER_IN_PROCESS:
            err2 = (uint32_t)SERV_IN_PROCESS;
            break;
        case ER_NVM_SIZE_ERROR:
            err2 = (uint32_t)SERV_NVM_SIZE_ERROR;
            break;
        case ER_SYS_CFG_NOT_INITIALIZED:
            err2 = (uint32_t)SERV_SYS_CFG_NOT_INITIALIZED;
            break;
        case ER_UNAVAILABLE:
            err2 = (uint32_t)SERV_UNAVAILABLE;
            break;
        case ER_DEST_MEM_ERROR:
            err2 = (uint32_t)SERV_DEST_MEM_ERROR;
            break;
        case ER_SRC_MEM_ERROR:
            err2 = (uint32_t)SERV_SRC_MEM_ERROR;
            break;
        case ER_FW_ACTIVATION_FAILED:
            err2 = (uint32_t)SERV_FW_ACTIVATION_FAILED;
            break;
        case ER_CR_VERIFY_FAIL:
            err2 = (uint32_t)SERV_CR_VERIFY_FAIL;
            break;
        case ER_S_00_SYS:
            err2 = (uint32_t)SERV_SYS_ERROR;
            break;
        case ER_INVALID_KEY_GROUP:
            err2 = (uint32_t)SERV_INVALID_KEY_GROUP;
            break;
        case ER_KEY_NOT_USABLE:
            err2 = (uint32_t)SERV_KEY_NOT_USABLE;
            break;
        case ER_RESTRICTED:
            err2 = (uint32_t)SERV_RESTRICTED;
            break;
        case ER_INVALID_KEY_INDEX:
            err2 = (uint32_t)SERV_INVALID_KEY_INDEX;
            break;
        case ER_SEQUENCE:
            err2 = (uint32_t)SERV_SEQUENCE_ERROR;
            break;
       default:
            break;
    }

    /* Error Dispatch */
    if((err & MASK_MID_ERR) != 0UL)
    {
        /* Middle error */
        err2 = s_00_ConvertErrMid(err);
    }
    else if((err & MASK_LIB_ERR) != 0UL)
    {
        /* Library error */
        err2 = s_00_ConvertErrLib(err);
    }
    else if((err & MASK_DRV_ERR) != 0UL)
    {
        /* Driver error */
        err2 = s_00_ConvertErrDrv(err);
    }
    else if((err & MASK_KERNEL_ERR) != 0UL)
    {
        /* Kernel error */
        err2 = s_00_ConvertErrKernel(err);
    }
    else
    {
        /* Never occur */
    }
    return err2;
}

#pragma ghs section text=default
#pragma ghs section rodata=default

