/******************************************************************************/
/* Component Name   Key Manager                                               */
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
#include "icum_common_pub.h"
/* For ICU-M service descriptor (ISD). */
#include "r_icumif_api.h"
#include "icum_d_cr_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_ml_abtr_api_pub.h"
/* For call functions of config manager. */
#include "icum_ml_cfg_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mh_key_pub.h"
#include "icum_mh_key.h"
#include "icum_mh_sysinfo_pub.h"

/**********************************************************************************
* Local defines
**********************************************************************************/
/* base addr TLS master secret ram keys of each vm */
#define TLS_MASTER_KEYS_BASE(vm_id, key_id)     (ML_CFG_TLS_MASTER_START_ADDR \
                                                    + (MH_KEY_TLS_MASTER_DATA_SIZE \
                                                        * ((uint32_t)ML_CFG_VM_TLS_MASTER_IDX_OFFSET(vm_id) \
                                                            + (uint32_t)(key_id))))

/* start addr of TLS master secret ram keys of each vm */
#define TLS_MASTER_KEYS(vm_id, key_id)              ((master_secret_t *)(TLS_MASTER_KEYS_BASE(vm_id, key_id)))

/* the key value start addr of TLS master secret ram keys for each vm */
#define TLS_MASTER_KEYS_VALUE(vm_id, key_id)        ((void *)(TLS_MASTER_KEYS_BASE(vm_id, key_id) \
                                                        + (uint32_t)sizeof(master_secret_t)))
/* the in-use counter start addr of TLS master secret ram key for each vm */
#define TLS_MASTER_KEYS_INUSE_COUNT(vm_id, key_id)  ((uint8_t *)(TLS_MASTER_KEYS_BASE(vm_id, key_id) \
                                                        + (uint32_t)sizeof(master_secret_t) \
                                                        + (uint32_t)MH_KEY_MASTER_SECRET_SIZE_IN_BYTES))

/**********************************************************************************
* Local const
***********************************************************************************/

/*********************************************************************************
* Local type definition
**********************************************************************************/


/**********************************************************************************
* Global variables
***********************************************************************************/

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/


/*===========================================================================
  Initialization of the master_secret key slots in the ICU-M
  [PUBLIC]
  ===========================================================================*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0006]:[ICUMFW_UD_MH_KeyMNG_0006]
* Function name: mh_KEY_InitMasterSecret
* Description  : Initialization process
* Arguments    : none
* Return Value : ER_OK
****************************************************************************************/
uint32_t mh_KEY_InitMasterSecret(void)
{
    uint32_t i;
    uint32_t vm_id;
    uint8_t key_number;
    uint8_t *p_tls_master_use_cnt;
    master_secret_t *p_tls_master_keys;

    /* Initialize local variable */
    i = NUM0;
    vm_id = NUM0;
    key_number = 0x00u;
    p_tls_master_use_cnt = NULL;
    p_tls_master_keys = NULL;

    /* init master_secret key */
    for (vm_id = NUM0; vm_id < (uint32_t)ML_CFG_TOTAL_NB_VMS_MAX; vm_id++)
    {
        (void)mh_KEY_MapSizeVM((uint8_t)vm_id, KEY_MAPPING_TLS_MASTER, &key_number);
        for (i = NUM0; i < (uint32_t)key_number; i++)
        {
            p_tls_master_keys = TLS_MASTER_KEYS(vm_id, i);
            p_tls_master_keys->p_value = TLS_MASTER_KEYS_VALUE(vm_id, i);
            p_tls_master_use_cnt = TLS_MASTER_KEYS_INUSE_COUNT(vm_id, i);

            /* init the value of key */
            L_Toolbox_MemSet((uint8_t *)p_tls_master_keys->p_value,
                             0x00u,
                             (int16_t)MH_KEY_MASTER_SECRET_SIZE_IN_BYTES);
            /* init availability */
            p_tls_master_keys->availability  = E_NOT_AVAILABLE;
            p_tls_master_keys->flags.write_protected   = 0u;
            p_tls_master_keys->flags.disable_on_debug  = 0u;
            p_tls_master_keys->flags.invalidate        = 0u;

            /* init the use frequency counter */
            *p_tls_master_use_cnt = 0x00u;
        }
    }

    return (ER_OK);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0037]:[ICUMFW_UD_MH_KeyMNG_0037]
* Function name: MH_KEY_GetMasterSecret
* Description  : Retrieve the ptr to the master_secret key value for a given key idx
* Arguments    : (IN)   p_job
*                (IN)   key_id
*                (OUT)  p_key
* Return Value : ER_OK
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NOT_USABLE
****************************************************************************************/
uint32_t MH_KEY_GetMasterSecret(icum_job_t *p_job, r_key_index_t key_id, master_secret_t **p_key)
{
    uint32_t err;
    uint8_t vm_id;
    uint8_t key_number;
    uint8_t *p_tls_master_use_cnt;
    master_secret_t *p_tls_master_keys;

    /* Initialize local variable */
    vm_id = p_job->vm_id;
    key_number = 0x00u;
    p_tls_master_keys = TLS_MASTER_KEYS(vm_id, key_id);
    p_tls_master_use_cnt = TLS_MASTER_KEYS_INUSE_COUNT(vm_id, key_id);
    *p_key = NULL;

    /* Checks range of vm_id */
    err = mh_KEY_CheckVMID(vm_id);
    if (ER_OK == err)
    {
        /* check that the key index is valid */
        (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_TLS_MASTER, &key_number);
        if ((uint32_t)key_id >= (uint32_t)key_number)
        {
            /* invalid key index: stop here */
            err = ER_MH_KEY_INVALID_KEY_INDEX;
        }
        /* is the key available (previously initialized)? */
        else if (p_tls_master_keys->availability != E_AVAILABLE)
        {
            /* RAM key not available (not initialized): stop here */
            err = ER_MH_KEY_SLOT_EMPTY;
        }
        else if ((uint32_t)NB_IN_USE_COUNTER_MAX <= (uint32_t)(*p_tls_master_use_cnt))
        {
            err = ER_MH_KEY_USER_OVER;
        }
        else
        {
            /* update the in use counter */
            *p_tls_master_use_cnt = (uint8_t)((uint32_t)(*p_tls_master_use_cnt) + NUM1);
            /* found: stop here */
            *p_key = p_tls_master_keys;
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0038]:[ICUMFW_UD_MH_KeyMNG_0038]
* Function name: MH_KEY_SetMasterSecret
* Description  : Register the pointer of master_secret key value
* Arguments    : (IN)   p_job
*                (IN)   p_new_key
* Return Value : ER_OK
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_WRITE_PROTECTED
****************************************************************************************/
uint32_t MH_KEY_SetMasterSecret(icum_job_t *p_job, master_secret_t *p_new_key)
{
    uint32_t err;
    uint8_t vm_id;
    uint8_t key_number;
    uint8_t *p_tls_master_use_cnt;
    master_secret_t *p_tls_master_keys;

    /* Initialize local variable */
    err = ER_OK;
    vm_id = p_job->vm_id;
    key_number = 0x00u;
    p_tls_master_keys = TLS_MASTER_KEYS(vm_id, p_new_key->key_id);
    p_tls_master_keys->p_value = TLS_MASTER_KEYS_VALUE(vm_id, p_new_key->key_id);
    p_tls_master_use_cnt = TLS_MASTER_KEYS_INUSE_COUNT(vm_id, p_new_key->key_id);

    /* Checks range of vm_id */
    err = mh_KEY_CheckVMID(vm_id);
    if (ER_OK == err)
    {
        /* check that the key index is valid */
        (void)mh_KEY_MapSizeVM(p_job->vm_id, KEY_MAPPING_TLS_MASTER, &key_number);
        if ((uint32_t)p_new_key->key_id >= (uint32_t)key_number)
        {
            /* invalid key index: stop here */
            err = ER_MH_KEY_INVALID_KEY_INDEX;
        }
        else if (NUM0 != (uint32_t)(*p_tls_master_use_cnt))
        {
            err = ER_MH_KEY_IN_USE;
        }
        else
        {
            /* don't care : p_tls_master_keys->flags.write_protected */
            /* set values */
            L_Toolbox_MemCpy((uint8_t *)p_tls_master_keys->p_value,
                             (const uint8_t *)p_new_key->p_value,
                             (int16_t)MH_KEY_MASTER_SECRET_SIZE_IN_BYTES);
            p_tls_master_keys->availability = E_AVAILABLE;
            p_tls_master_keys->key_id       = p_new_key->key_id;
            p_tls_master_keys->vm_id        = vm_id;
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0060]:[ICUMFW_UD_MH_KeyMNG_0060]
* Function name: mh_KEY_FreeCacheMasterSecret
* Description  : Decrement counter of used key
* Arguments    : p_job
* Return Value : ER_OK
*                ER_PARAM
*                ER_MH_KEY_DATA_NOT_FOUND
****************************************************************************************/
uint32_t mh_KEY_FreeCacheMasterSecret(icum_job_t *p_job, r_key_index_t key_id)
{
    int32_t i;
    uint32_t err;
    uint8_t vm_id;
    uint8_t key_number;
    uint8_t *p_tls_master_use_cnt;
    master_secret_t *p_tls_master_keys;

    /* Initialize local variable */
    i = 0L;
    err = ER_OK;
    vm_id = p_job->vm_id;
    key_number = 0x00u;
    p_tls_master_keys = NULL;
    p_tls_master_use_cnt = NULL;

    (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_TLS_MASTER, &key_number);
    for (i = (int32_t)key_number - 1L; i >= 0L; i--)
    {
        p_tls_master_keys = TLS_MASTER_KEYS(vm_id, i);
        p_tls_master_use_cnt = TLS_MASTER_KEYS_INUSE_COUNT(vm_id, i);
        if ((E_AVAILABLE == p_tls_master_keys->availability) &&
            ((uint32_t)vm_id == (uint32_t)p_tls_master_keys->vm_id) &&
            ((uint32_t)key_id == (uint32_t)p_tls_master_keys->key_id))
        {
            if (NUM0 != (uint32_t)(*p_tls_master_use_cnt))
            {
                *p_tls_master_use_cnt = (uint8_t)((uint32_t)(*p_tls_master_use_cnt) - NUM1);
            }
            else
            {
                err = ER_PARAM;
            }
            break;
        }
        else
        {
            /* Do nothing */
        }
    }
    if (i < 0L)
    {
        err = ER_MH_KEY_DATA_NOT_FOUND;
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0064]:[ICUMFW_UD_MH_KeyMNG_0064]
* Function name: mh_KEY_DeleteMasterSecretRamKey
* Description  : Delete master Secret in ram
* Arguments    : void
* Return Value : ER_OK
*              : ER_MH_KEY_IN_USE
***************************************************************************************/
uint32_t mh_KEY_DeleteMasterSecretRamKey(void)
{
    uint32_t i;
    uint32_t err;
    uint32_t vm_id;
    uint8_t key_number;
    sys_cfg_vm_t vm_cfg;
    uint8_t *p_tls_master_use_cnt;
    master_secret_t *p_tls_master_keys;

    /* Initialize local variable */
    i = NUM0;
    err = ER_OK;
    vm_id = NUM0;
    p_tls_master_use_cnt = NULL;
    p_tls_master_keys = NULL;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));

    (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
    for (vm_id = NUM0; (vm_id < (uint32_t)vm_cfg.nb_vms) && (ER_OK == err); vm_id++)
    {
        (void)mh_KEY_MapSizeVM((uint8_t)vm_id, KEY_MAPPING_TLS_MASTER, &key_number);
        for (i = NUM0; (i < (uint32_t)key_number) && (ER_OK == err); i++)
        {
            /* Checks in-use counter */
            p_tls_master_use_cnt = TLS_MASTER_KEYS_INUSE_COUNT(vm_id, i);
            if (NUM0 == (uint32_t)(*p_tls_master_use_cnt))
            {
                p_tls_master_keys = TLS_MASTER_KEYS(vm_id, i);
                p_tls_master_keys->p_value = TLS_MASTER_KEYS_VALUE(vm_id, i);
                p_tls_master_keys->availability = E_NOT_AVAILABLE;
                p_tls_master_keys->key_id = 0x00u;
                p_tls_master_keys->vm_id = 0x00u;
                p_tls_master_keys->flags.write_protected   = 0u;
                p_tls_master_keys->flags.disable_on_debug  = 0u;
                p_tls_master_keys->flags.invalidate        = 0u;

                L_Toolbox_MemSet((uint8_t*)p_tls_master_keys->p_value,
                                 0x00u,
                                 (int16_t)MH_KEY_MASTER_SECRET_SIZE_IN_BYTES);
                *p_tls_master_use_cnt = 0u;
            }
            else
            {
                err = ER_MH_KEY_IN_USE;
            }
        }
    }

    return (err);
}
