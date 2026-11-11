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
#include "r_icumif_api.h"
/* For call functions of config manager. */
#include "icum_ml_cfg_pub.h"
#include "icum_mh_key_pub.h"
#include "icum_mh_key.h"
#include "icum_l_toolbox_pub.h"

/**********************************************************************************
* Local defines
**********************************************************************************/
/* base addr hmac keys of each vm */
#define HMAC_KEYS_BASE(vm_id, key_id)   (ML_CFG_HMAC_START_ADDR \
                                            + (MH_KEY_HMAC_DATA_SIZE \
                                                * ((uint32_t)ML_CFG_VM_HMAC_IDX_OFFSET(vm_id) \
                                                    + (uint32_t)(key_id))))

/* start addr of hmac keys of each vm */
#define HMAC_KEYS(vm_id, key_id)                    ((hmac_key_t *)(HMAC_KEYS_BASE(vm_id, key_id)))

/* the key value start addr of hmac keys for each vm */
#define HMAC_KEYS_VALUE(vm_id, key_id)              ((void *)(HMAC_KEYS_BASE(vm_id, key_id) \
                                                        + (uint32_t)sizeof(hmac_key_t)))
/* the in-use counter start addr of hmac key for each vm */
#define HMAC_KEYS_INUSE_COUNT(vm_id, key_id)        ((uint8_t *)(HMAC_KEYS_BASE(vm_id, key_id) \
                                                        + (uint32_t)sizeof(hmac_key_t) \
                                                        + (uint32_t)MH_KEY_HMAC_KEY_SIZE_IN_BYTES))

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
static uint32_t mh_KEY_UpdateHmacRam(icum_job_t *p_job, hmac_key_t *p_new_key);

/*===========================================================================
  Initialization of the HMAC key slots in the ICU-M
  [PUBLIC]
  ===========================================================================*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0005]:[ICUMFW_UD_MH_KeyMNG_0005]
* Function name: mh_KEY_InitHmac()
* Description  : # Perform initialization of HMAC key information.
* Arguments    : none
* Return Value : ER_OK
****************************************************************************************/
uint32_t mh_KEY_InitHmac(void)
{
    uint32_t i;
    uint32_t vm_id;
    uint8_t key_number;
    uint8_t *p_hmac_use_cnt;
    hmac_key_t *p_hmac_keys;

    /* Initialize local variable */
    i = NUM0;
    vm_id = NUM0;
    key_number = 0x00u;
    p_hmac_use_cnt = NULL;
    p_hmac_keys = NULL;

    /* init RAM HMAC key */
    for (vm_id = NUM0; vm_id < (uint32_t)ML_CFG_TOTAL_NB_VMS_MAX; vm_id++)
    {
        (void)mh_KEY_MapSizeVM((uint8_t)vm_id, KEY_MAPPING_HMAC, &key_number);
        for (i = NUM0; i < (uint32_t)key_number; i++)
        {
            p_hmac_keys = HMAC_KEYS(vm_id, i);
            p_hmac_keys->p_key = HMAC_KEYS_VALUE(vm_id, i);
            p_hmac_use_cnt = HMAC_KEYS_INUSE_COUNT(vm_id, i);

            L_Toolbox_MemSet((uint8_t *)p_hmac_keys->p_key, 0x00u, (int16_t)sizeof(ecc_key_prop_t));

            /* init availability */
            p_hmac_keys->availability = E_NOT_AVAILABLE;
            /* init the use frequency counter */
            *p_hmac_use_cnt = 0x00u;
        }
    }

    return (ER_OK);
}


/*===========================================================================
  Retrieve the pointer to the RAM HMAC key value & properties for a given idx
  Returns 0 if the key is not available (i.e. not initialized in Flash)
  ===========================================================================*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0034]:[ICUMFW_UD_MH_KeyMNG_0034]
* Function name: MH_KEY_GetHmacKey()
* Description  : # Get the specified HMAC key.
* Arguments    : (IN)  p_job : pointer of job parameter
                 (IN)  key_id : Index number in KEY_GRP_HMAC
*                (IN-OUT)  p_key : Pointer of key data storage address
* Return Value : ER_OK
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_SLOT_EMPTY
****************************************************************************************/
uint32_t MH_KEY_GetHmacKey(icum_job_t *p_job, r_key_index_t key_id, hmac_key_t **p_key)
{
    uint32_t err;
    uint8_t key_number;
    uint8_t *p_hmac_use_cnt;
    hmac_key_t *p_hmac_keys;

    /* Initialize local variable */
    err = ER_OK;
    key_number = 0x00u;
    p_hmac_use_cnt = HMAC_KEYS_INUSE_COUNT(p_job->vm_id, key_id);
    p_hmac_keys = HMAC_KEYS(p_job->vm_id, key_id);
    *p_key = NULL;

    /* Checks the range of VMID */
    err = mh_KEY_CheckVMID(p_job->vm_id);
    if (ER_OK == err)
    {
        /* check that the key index is valid */
        (void)mh_KEY_MapSizeVM(p_job->vm_id, KEY_MAPPING_HMAC, &key_number);
        if ((uint32_t)key_id >= (uint32_t)key_number)
        {
            /* invalid key index: stop here */
            err = ER_MH_KEY_INVALID_KEY_INDEX;
        }
        /* is the key available (previously initialized)? */
        else if (p_hmac_keys->availability != E_AVAILABLE)
        {
            /* RAM key not available (not initialized): stop here */
            err = ER_MH_KEY_SLOT_EMPTY;
        }
        else if ((uint32_t)NB_IN_USE_COUNTER_MAX <= (uint32_t)(*p_hmac_use_cnt))
        {
            err = ER_MH_KEY_USER_OVER;
        }
        else
        {
            /* update the in use counter */
            *p_hmac_use_cnt = (uint8_t)((uint32_t)(*p_hmac_use_cnt) + NUM1);
            /* found it: stop here */
            *p_key = p_hmac_keys;
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    return (err);
}


/*===========================================================================
  Update an HMAC RAM key
  [PUBLIC]
  ===========================================================================*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0036]:[ICUMFW_UD_MH_KeyMNG_0036]
* Function name: mh_KEY_UpdateHmacRam()
* Description  : # Regarding / updating the specified HMAC key if the status is unused.
* Arguments    : (IN)  p_job : pointer of job parameter
*                (IN)  p_new_key : Pointer to Key_parameter
* Return Value : ER_OK
*                ER_MH_KEY_IN_USE
****************************************************************************************/
static uint32_t mh_KEY_UpdateHmacRam(icum_job_t *p_job, hmac_key_t *p_new_key)
{
    uint32_t err = ER_OK;
    uint8_t *p_hmac_use_cnt;
    hmac_key_t *p_hmac_keys;

    /* Initialize local variable */
    p_hmac_use_cnt = HMAC_KEYS_INUSE_COUNT(p_job->vm_id, p_new_key->key_id);
    p_hmac_keys = HMAC_KEYS(p_job->vm_id, p_new_key->key_id);
    p_hmac_keys->p_key = HMAC_KEYS_VALUE(p_job->vm_id, p_new_key->key_id);

    if (NUM0 == (uint32_t)(*p_hmac_use_cnt))
    {
        L_Toolbox_MemSet((uint8_t *)p_hmac_keys->p_key, 0x00u, (int16_t)MH_KEY_HMAC_KEY_SIZE_IN_BYTES);

        p_hmac_keys->availability                 = E_AVAILABLE;
        p_hmac_keys->key_id                       = p_new_key->key_id;
        p_hmac_keys->vm_id                        = p_job->vm_id;
        p_hmac_keys->key_size_in_byte             = p_new_key->key_size_in_byte;
        L_Toolbox_MemCpy((uint8_t *)p_hmac_keys->p_key,
                         (const uint8_t *)p_new_key->p_key,
                         (int16_t)(p_new_key->key_size_in_byte));
    }
    else
    {
        err = ER_MH_KEY_IN_USE;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0035]:[ICUMFW_UD_MH_KeyMNG_0035]
* Function name: MH_KEY_SetHmacKey()
* Description  : # Register / update the specified HMAC key.
* Arguments    : (IN)  p_job : pointer of job parameter
*                (IN)  p_new_key : Pointer to Key_parameter
* Return Value : ER_OK
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_NOT_USABLE
****************************************************************************************/
uint32_t MH_KEY_SetHmacKey(icum_job_t *p_job, hmac_key_t *p_new_key)
{
    uint32_t err;
    uint8_t key_number;

    /* Initialize local variable */
    err = ER_OK;
    key_number = 0x00u;

    /* Checks the range of VMID */
    err = mh_KEY_CheckVMID(p_job->vm_id);
    if (ER_OK == err)
    {
        /* check that the key index is valid */
        (void)mh_KEY_MapSizeVM(p_job->vm_id, KEY_MAPPING_HMAC, &key_number);
        if ((uint32_t)p_new_key->key_id >= (uint32_t)key_number)
        {
            /* invalid key index: stop here */
            err = ER_MH_KEY_INVALID_KEY_INDEX;
        }
        else
        {
            err = mh_KEY_UpdateHmacRam(p_job, p_new_key);
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0059]:[ICUMFW_UD_MH_KeyMNG_0059]
* Function name: mh_KEY_FreeCacheHmac()
* Description  : # Notify that the use of the Hmac key has ended.
* Arguments    : (IN)  p_job : pointer of job paramater
*              : (IN)  key_id : Index number in KEY_GRP_HMAC
* Return Value : ER_OK
*                ER_MH_KEY_DATA_NOT_FOUND
****************************************************************************************/
uint32_t mh_KEY_FreeCacheHmac(icum_job_t *p_job, r_key_index_t key_id)
{
    int32_t i;
    uint32_t err;
    uint8_t vm_id;
    uint8_t key_number;
    uint8_t *p_hmac_use_cnt;
    hmac_key_t *p_hmac_keys;

    /* Initialize local variable */
    i = 0L;
    err = ER_OK;
    vm_id = p_job->vm_id;
    key_number = 0x00u;
    p_hmac_use_cnt = NULL;
    p_hmac_keys = NULL;

    (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_HMAC, &key_number);
    for (i = (int32_t)key_number - 1L; i >= 0L; i--)
    {
        p_hmac_use_cnt = HMAC_KEYS_INUSE_COUNT(vm_id, i);
        p_hmac_keys = HMAC_KEYS(vm_id, i);
        if ((E_AVAILABLE == p_hmac_keys->availability) &&
            ((uint32_t)vm_id == (uint32_t)p_hmac_keys->vm_id) &&
            ((uint32_t)key_id == (uint32_t)p_hmac_keys->key_id))
        {
            if (NUM0 != (uint32_t)(*p_hmac_use_cnt))
            {
                *p_hmac_use_cnt = (uint8_t)((uint32_t)(*p_hmac_use_cnt) - NUM1);
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
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0063]:[ICUMFW_UD_MH_KeyMNG_0063]
* Function name: mh_KEY_DeleteHmacRamKey
* Description  : Delete HMAC key in ram
* Arguments    : void
* Return Value : ER_OK
*              : ER_MH_KEY_IN_USE
***************************************************************************************/
uint32_t mh_KEY_DeleteHmacRamKey(void)
{
    uint32_t i;
    uint32_t err;
    uint32_t vm_id;
    uint8_t key_number;
    uint8_t *p_hmac_use_cnt;
    hmac_key_t *p_hmac_keys;
    sys_cfg_vm_t vm_cfg;

    /* Initialize local variable */
    i = NUM0;
    err = ER_OK;
    vm_id = NUM0;
    p_hmac_use_cnt = NULL;
    p_hmac_keys = NULL;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));

    (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
    for (vm_id = NUM0; (vm_id < (uint32_t)vm_cfg.nb_vms) && (ER_OK == err); vm_id++)
    {
        (void)mh_KEY_MapSizeVM((uint8_t)vm_id, KEY_MAPPING_HMAC, &key_number);
        for (i = NUM0; (i < (uint32_t)key_number) && (ER_OK == err); i++)
        {
            /* Checks in-use counter */
            p_hmac_use_cnt = HMAC_KEYS_INUSE_COUNT(vm_id, i);
            if (NUM0 == (uint32_t)(*p_hmac_use_cnt))
            {
                p_hmac_keys = HMAC_KEYS(vm_id, i);
                p_hmac_keys->availability = E_NOT_AVAILABLE;
                p_hmac_keys->key_id = 0u;
                p_hmac_keys->vm_id = 0u;
                p_hmac_keys->p_key = HMAC_KEYS_VALUE(vm_id, i);
                L_Toolbox_MemSet((uint8_t*)p_hmac_keys->p_key, 0x00u, (int16_t)MH_KEY_HMAC_KEY_SIZE_IN_BYTES);
                *p_hmac_use_cnt = 0u;
            }
            else
            {
                err = ER_MH_KEY_IN_USE;
            }
        }
    }

    return (err);
}
