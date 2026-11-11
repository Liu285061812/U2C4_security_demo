/******************************************************************************/
/* Component Name  Key Manager (AES)                                          */
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
#include "icum_l_toolbox_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mh_key_pub.h"
#include "icum_mh_key.h"
#include "icum_ml_cfg_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_wdt_pub.h"
#include "icum_d_cry_aes_pub.h"

/**********************************************************************************
* Local defines
**********************************************************************************/
#define DATA_ZERO       ((uint8_t)0u)            /* value : 0 used for initialization */
#define AES_STATE_INIT  ((uint32_t)0xFFFFFFFFu)  /* initial state */

/* Chip_id Size */
#define CHIPID_SIZE            (0x000Fu)
/* SHE Flag Data position */
#define SHE_FLAG_WRITE_PROTECTION       (uint8_t)(1U << 5U)
#define SHE_FLAG_BOOT_PROTECTION        (uint8_t)(1U << 4U)
#define SHE_FLAG_DEBUGGER_PROTECTION    (uint8_t)(1U << 3U)
#define SHE_FLAG_KEY_USAGE              (uint8_t)(1U << 2U)
#define SHE_FLAG_NO_WILD_CARD           (uint8_t)(1U << 1U)
#define SHE_FLAG_CMAC_USAGE             (uint8_t)(1U << 0U)
/* In-use management table (AES) */
#define NB_AES_IN_USE_MNG_TABLE         ((uint32_t)ISSD_MAX_NB_OF_JOBS)

#define CONVERT_ERR     ((uint32_t)255uL)     /* Convert Error */

/* start addr of aes ram key of each vm */
#define RAM_AES_KEYS(vm_id, key_id)          (((aes_key_t *)ML_CFG_AES_RAM_START_ADDR \
                                                + ML_CFG_VM_AES_RAM_IDX_OFFSET(vm_id)) \
                                                + (key_id))
/* start addr of aes buffer key of each vm */
#define COPY_AES_KEYS_FIX(vm_id, key_id)     (((aes_key_t *)ML_CFG_AES_BUFFER_START_ADDR \
                                                + (uint32_t)ML_CFG_VM_AES_BUFFER_IDX_OFFSET(vm_id)) \
                                                + ((uint32_t)(key_id) - MH_KEY_AES_BUFFER_KEY_ID_START))

/**********************************************************************************
* Local const
***********************************************************************************/
#pragma alignvar(4)
static const uint8_t EXPORT_CID[16] =
    {0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};

/* AES key slot as stored in Data Flash */
typedef struct aes_key_slot
{
    uint8_t             dummy[FLS_DATA_SLOT_MGMT_SIZE];
    aes_key_prop_t      properties;
    uint8_t             value[MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON];
    uint8_t             reserved[FLS_DF_BLOCK_SIZE - ((FLS_DATA_SLOT_MGMT_SIZE
                                                    + (uint32_t)sizeof(aes_key_prop_t)
                                                    + (uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON) % FLS_DF_BLOCK_SIZE)];
} aes_key_slot_t;

/* In-use management table (AES) */
typedef struct aes_in_use_mng_table
{
    uint32_t        availability;
    r_key_group_t   key_group;
    uint8_t         vm_id;
    r_key_index_t   key_id;
} aes_in_use_mng_table_t;

/**********************************************************************************
* Global variables
***********************************************************************************/
extern uint32_t g_ContextAddress;

/*===========================================================================
  Write AES key data storage area
  ===========================================================================*/
#pragma alignvar(4)
static aes_key_slot_t g_storage_keys_aes;

#pragma alignvar(4)
static aes_key_t g_copy_aes_keys_change[NB_RAM_COPIES_AES_KEYS_CHANGE];

/*===========================================================================
  Index in table g_copy_aes_keys_change of the next slot to receive a key copy
  ===========================================================================*/
 static uint8_t g_current_idx_aes;

/*===========================================================================
  RAM copy of the Special Key
  ===========================================================================*/
#pragma alignvar(4)
static aes_key_t g_copy_CR_key;

/*===========================================================================
  In-use management table(AES)
  ===========================================================================*/
static aes_in_use_mng_table_t g_aes_in_use_mng_table[NB_AES_IN_USE_MNG_TABLE];

/*===========================================================================
  number of AES RAM in each VM for Fast Cmac
  ===========================================================================*/
static uint8_t g_nb_aes_ram[ML_CFG_TOTAL_NB_VMS_MAX];

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
static uint32_t mh_KEY_UpdateInUseMngTable(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id);
static uint32_t mh_KEY_FindKeyIdInUseMngTable(uint8_t vm_id,
                                              r_key_group_t key_group,
                                              r_key_index_t key_id,
                                              uint8_t *p_result);
static uint32_t mh_KEY_GetAesRam(icum_job_t *p_job, r_key_index_t key_id,
    aes_key_t **p_key, uint8_t in_use_enable_flag);
static uint32_t mh_KEY_GetAesCommon(icum_job_t *p_job, r_key_index_t key_id,
    aes_key_t **p_key, uint8_t in_use_enable_flag);
static uint32_t mh_KEY_GetAesCommonFix(icum_job_t *p_job, r_key_index_t key_id, aes_key_t **p_key);
static uint32_t mh_KEY_GetAesCommonChange(icum_job_t *p_job, r_key_index_t key_id, aes_key_t **p_key);
static uint32_t mh_KEY_GetAesCr_NotUse( r_key_index_t key_id, aes_key_t **p_key);
static uint32_t mh_KEY_UpdateAesRam(icum_job_t *p_job, aes_key_t *p_new_key);
static uint32_t mh_KEY_UpdateAes(icum_job_t *p_job, set_operation_t operation, aes_key_t *p_new_key);
static uint32_t mh_KEY_DeleteRamKey(icum_job_t *p_job);
static uint32_t mh_KEY_KeyCacheSet(icum_job_t *p_job);
static uint32_t mh_KEY_DeleteCacheKey(void);
static uint32_t mh_KEY_CalculateMp(
                    const uint32_t *p_key_value, const uint8_t *p_const, uint32_t block_number
                    , uint32_t *p_deriv_key_value);
static uint32_t mh_KEY_CalculateCmac(
                    const uint32_t *p_key, const uint32_t *p_msg, uint32_t block_number, uint32_t *p_cmac);
static uint32_t mh_KEY_CalculateCbc(
                    cipher_direction_t direction, const uint32_t *p_key, const uint32_t *p_input, uint32_t *p_output
                    , uint32_t block_number);
static uint32_t mh_KEY_CalculateEcb( const uint32_t *p_key, const uint32_t *p_input, uint32_t *p_output);
static uint32_t mh_KEY_UpShe_KeyCombination(
                    uint8_t *p_key_id, uint8_t *p_auth_id, uint8_t extend_key, uint8_t *p_const);
static uint32_t mh_KEY_UpShe_GetCurrentKey(icum_job_t *p_job,
                    uint8_t current_key_id, key_counter_t *p_current_counter, aes_flags_t *p_current_flags);
static uint32_t mh_KEY_UpShe_GetAuthKey(icum_job_t *p_job,
                    uint8_t current_key_id, uint8_t auth_key_id, uint32_t *p_auth_value, uint8_t key_size_ext);
static uint32_t mh_KEY_UpShe_CmpM3Data( 
                    const uint8_t *p_M1, const uint8_t *p_M2, const uint8_t *p_M3, uint8_t *p_temp
                    , uint32_t *p_auth_value, const uint8_t *p_const_mac, uint8_t key_size_ext);
static uint32_t mh_KEY_UpShe_CheckUid(
                    const uint8_t *p_M1, key_counter_t current_counter, aes_flags_t current_flags, uint8_t *p_chip_id);
static uint32_t mh_KEY_UpShe_DecM2(
                    const uint8_t *p_M2, uint32_t *p_auth_value, uint8_t *p_temp
                    , const uint8_t *p_const_enc, uint8_t key_size_ext, uint16_t* p_key_size);
static uint32_t mh_KEY_UpShe_CheckCounterFlags(
                    key_counter_t current_counter, uint32_t *p_counter, uint8_t *p_flags
                    , uint8_t *p_temp,uint8_t key_id);
static uint32_t mh_KEY_UpShe_FormKeyData(
                    uint8_t key_id, aes_key_t *p_aes_key, uint8_t *p_temp, uint32_t new_counter
                    , uint8_t new_flags, uint16_t key_size);
static uint32_t mh_KEY_PrepareGenExportKey(icum_job_t *p_job,
    uint32_t *p_secret_key_value, uint8_t *p_she_auth_key_id,
    uint8_t *p_she_key_id, uint32_t *p_resource_id, uint8_t *p_abtr_resource, uint32_t *p_ramkey_value,
    uint16_t check_key_flag, uint8_t key_size_ext, uint32_t *p_ramkey_size);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0002]:[ICUMFW_UD_MH_KeyMNG_0002]
* Function name: mh_KEY_InitAes
* Description  : Initialization of the AES key slots in the ICU-M
* Arguments    : void
* Return Value : ER_OK
***************************************************************************************/
uint32_t mh_KEY_InitAes(void)
{
    uint32_t i;
    uint8_t key_number;

    /* Initialize local variable */
    i = NUM0;
    key_number = 0x00u;

    /* init Config RAM item */
    (void)mh_KEY_InitAesRamCfg();

    /* init copy of AES keys(change) - init availability */
    for (i = NUM0; i < NB_RAM_COPIES_AES_KEYS_CHANGE; i++)
    {
        g_copy_aes_keys_change[i].availability = E_NOT_AVAILABLE;
    }

    /* init copy of CR(challenge & response ) keys - init availability */
    g_copy_CR_key.availability = E_NOT_AVAILABLE;

    /* In-use management table update */
    for (i = NUM0; i < NB_AES_IN_USE_MNG_TABLE; i++)
    {
        g_aes_in_use_mng_table[i].availability = E_NOT_AVAILABLE;
    }

    /* declare the data set (Flash mapping) */
    (void)mh_KEY_MapSize(KEY_MAPPING_AES, &key_number);
    (void)MM_FLS_DeclareDataSet(FLS_DS_AES_KEY,     key_number,             MH_KEY_AES_DS_NB_BLOCKS, FLS_DS_NB_COPIES);
    (void)MM_FLS_DeclareDataSet(FLS_DS_CR_KEY,      MH_KEY_NB_CR_KEYS,      MH_KEY_AES_DS_NB_BLOCKS, FLS_DS_NB_COPIES);

    return(ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0088]:[ICUMFW_UD_MH_KeyMNG_0088]
* Function name: mh_KEY_InitAesRamCfg
* Description  : Initialization of the AES key(config RAM) in the ICU-M
* Arguments    : void
* Return Value : ER_OK
***************************************************************************************/
uint32_t mh_KEY_InitAesRamCfg(void)
{
    uint32_t i;
    uint32_t vm_id;
    uint8_t key_number;
    aes_key_t *p_ram_aes_keys;
    sys_cfg_vm_t vm_cfg;

    /* Initialize local variable */
    i = NUM0;
    vm_id = NUM0;
    key_number = 0x00u;
    p_ram_aes_keys = NULL;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));

    (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
    for (vm_id = NUM0; vm_id < (uint32_t)vm_cfg.nb_vms; vm_id++)
    {
        /* init AES RAM key */
        (void)mh_KEY_MapSizeVM((uint8_t)vm_id, KEY_MAPPING_AES_RAM, &key_number);
        for (i = NUM0; i < (uint32_t)key_number; i++)
        {
            /* init availability */
            p_ram_aes_keys = RAM_AES_KEYS(vm_id, i);
            p_ram_aes_keys->availability = E_NOT_AVAILABLE;
        }

        /* Set number of AES RAM keys of Each VM for Fast Cmac*/
        g_nb_aes_ram[vm_id] = key_number;

        /* init copy of AES keys(fix) */
        (void)mh_KEY_MapSizeVM((uint8_t)vm_id, KEY_MAPPING_AES_BUFFER, &key_number);
        for(i = MH_KEY_AES_BUFFER_KEY_ID_START; i < ((uint32_t)key_number + MH_KEY_AES_BUFFER_KEY_ID_START); i++)
        {
            /* init availability */
            COPY_AES_KEYS_FIX(vm_id, i)->availability = E_NOT_AVAILABLE;
        }
    } /* end of for (vm_id = NUM0; vm_id < (uint32_t)vm_cfg.nb_vms; vm_id++) */

    return(ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0077]:[ICUMFW_UD_MH_KeyMNG_0077]
* Function name: mh_KEY_UpdateInUseMngTable
* Description  : Update / add in-use management table
* Arguments    : p_job
*              : key_group
*              : key_id
* Return Value : ER_OK
*              : ER_MH_KEY_SYS_ERROR
***************************************************************************************/
static uint32_t mh_KEY_UpdateInUseMngTable(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id)
{
    uint32_t err;
    int32_t idx;

    /* Initialize local variable */
    err = ER_MH_KEY_SYS_ERROR;
    idx = 0L;

    /* Search blank */
    for (idx = (int32_t)(NB_AES_IN_USE_MNG_TABLE - NUM1); idx >= 0L; idx--)
    {
        if (E_NOT_AVAILABLE == g_aes_in_use_mng_table[idx].availability)
        {
            /* found a blank */
            /* Update in-use management table */
            g_aes_in_use_mng_table[idx].key_group = key_group;
            g_aes_in_use_mng_table[idx].vm_id = p_job->vm_id;
            g_aes_in_use_mng_table[idx].key_id = key_id;
            g_aes_in_use_mng_table[idx].availability = E_AVAILABLE;
            err = ER_OK;
            break;
         }
         else
         {
             /* do nothing */
         }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0078]:[ICUMFW_UD_MH_KeyMNG_0078]
* Function name: mh_KEY_FindKeyIdInUseMngTable
* Description  : Search in-use management table
* Arguments    : vm_id
*              : key_group
*              : key_id
*              : p_result
* Return Value : ER_OK
***************************************************************************************/
static uint32_t mh_KEY_FindKeyIdInUseMngTable(uint8_t vm_id,
                                              r_key_group_t key_group,
                                              r_key_index_t key_id,
                                              uint8_t *p_result)
{
    int32_t idx;

    /* Initialize local variable */
    idx = 0L;
    *p_result = MH_KEY_IN_USE_MNG_TABLE_NOT_FOUND;

    /* Look for key_group/key_id on the in-use table */
    for (idx = (int32_t)(NB_AES_IN_USE_MNG_TABLE - NUM1); idx >= 0L; idx--)
    {
        if ((E_AVAILABLE == g_aes_in_use_mng_table[idx].availability) &&
            (key_group == g_aes_in_use_mng_table[idx].key_group) &&
            ((uint32_t)vm_id == (uint32_t)g_aes_in_use_mng_table[idx].vm_id) &&
            ((uint32_t)key_id == (uint32_t)g_aes_in_use_mng_table[idx].key_id))
        {
            /* Found */
            *p_result = MH_KEY_IN_USE_MNG_TABLE_FOUND;
            break;
        }
        else
        {
            /* do nothing */
        }
    }

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0011]:[ICUMFW_UD_MH_KeyMNG_0011]
* Function name: mh_KEY_GetAesRam
* Description  : Retrieve the p_key to the AES RAM key value & properties for a given key idx
*              : Returns empty if the key is not available (i.e. not initialized in Flash)
*              : Note: the key should have been initialized prior to calling this function
* Arguments    : p_job
*              : key_id
*              : p_key
*              : in_use_enable_flag
* Return Value : ER_OK
*              : ER_MH_KEY_SYS_ERROR
*              : ER_MH_KEY_SLOT_EMPTY
***************************************************************************************/
static uint32_t mh_KEY_GetAesRam(icum_job_t *p_job, r_key_index_t key_id,
    aes_key_t **p_key, uint8_t in_use_enable_flag)
{
    uint32_t err;
    uint8_t key_number;
    aes_key_t *p_ram_aes_keys;

    /* Initialize local variable */
    err = ER_OK;
    key_number = 0x00u;
    *p_key = NULL;
    p_ram_aes_keys = RAM_AES_KEYS(p_job->vm_id, key_id);

   (void)mh_KEY_MapSizeVM(p_job->vm_id, KEY_MAPPING_AES_RAM, &key_number);
    if ((uint32_t)key_number <= (uint32_t)key_id)
    {
        /* invalid key index: stop here */
        err = ER_MH_KEY_INVALID_KEY_INDEX;
    }
    /* is the key available (previously initialized)? */
    else if (p_ram_aes_keys->availability == E_AVAILABLE)
    {
        if (true == in_use_enable_flag)
        {
            /* Update / add in-use management table */
            err = mh_KEY_UpdateInUseMngTable(p_job, KEY_GRP_AES_RAM, key_id);
        }
        else
        {
            /* do nothing */
        }

        if (ER_OK == err)
        {
            /* found: stop here */
            *p_key = p_ram_aes_keys;
        }
        else
        {
            /* do nothing */
        }
    } /* end of else if (p_ram_aes_keys->availability == E_AVAILABLE) */
    else
    {
        /* RAM key not available (not initialized) */
        err = ER_MH_KEY_SLOT_EMPTY;
    }
    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0008]:[ICUMFW_UD_MH_KeyMNG_0008]
* Function name: mh_KEY_GetAesCommon
* Description  : Retrieve the p_key to the AES key value & properties for a given key idx
*              : Returns empty if the key is not available (i.e. not initialized in Flash)
* Arguments    : p_job
*              : key_group
*              : key_id
*              : p_key
*              : in_use_enable_flag
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_INVALID_KEY_INDEX
*              : ER_MH_KEY_SLOT_EMPTY
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
static uint32_t mh_KEY_GetAesCommon(icum_job_t *p_job, r_key_index_t key_id,
    aes_key_t **p_key, uint8_t in_use_enable_flag)
{
    uint32_t err;
    sys_cfg_key_mapping_t key_mapping;

    /* Initialize local variable */
    err = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&key_mapping, 0x00u, (int16_t)sizeof(sys_cfg_key_mapping_t));

   (void)ML_CFG_GetSysVM(p_job->vm_id, ML_CFG_SYS_KEY_MAPPING, (uint32_t *)&key_mapping);
    if ((uint32_t)key_mapping.nb_aes_keys <= (uint32_t)key_id)
    {
        /* invalid key index: stop here */
        err = ER_MH_KEY_INVALID_KEY_INDEX;
        *p_key = NULL;
    }
    else if ((MH_KEY_AES_BUFFER_KEY_ID_START <= (uint32_t)key_id) &&
             ((MH_KEY_AES_BUFFER_KEY_ID_START + (uint32_t)key_mapping.nb_aes_buffer_keys) > (uint32_t)key_id))
    {
        /* Key acquisition from Cache(fix) */
        err = mh_KEY_GetAesCommonFix(p_job, key_id, p_key);
    }
    else
    {
        /* Key acquisition from Cache(change) */
        err = mh_KEY_GetAesCommonChange(p_job, key_id, p_key);
    }

    if ((ER_OK == err) && (true == in_use_enable_flag))
    {
        /* Update / add in-use management table */
        err = mh_KEY_UpdateInUseMngTable(p_job, KEY_GRP_AES, key_id);
    }
    else
    {
        /* do nothing */
    }

    if (ER_OK != err)
    {
        err = mh_KEY_ConvertErr(err);
        *p_key = NULL;
    }
    else
    {
        /* do nothing */
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0009]:[ICUMFW_UD_MH_KeyMNG_0009]
* Function name: mh_KEY_GetAesCommonFix
* Description  : Get AES key in fixed key area
* Arguments    : key_id
*              : p_key
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_SLOT_EMPTY
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
static uint32_t mh_KEY_GetAesCommonFix(icum_job_t *p_job, r_key_index_t key_id, aes_key_t **p_key)
{
    aes_key_slot_t      key_slot;
    uint32_t            err = ER_OK;
    uint8_t             vm_id;
    aes_key_t*          p_copy_aes_keys_fix;

    /* Initialize local variable */
    vm_id = p_job->vm_id;
    key_slot.value[0] = 0x00u;
    p_copy_aes_keys_fix = COPY_AES_KEYS_FIX(vm_id, key_id);

    /* Check if there is key data in the cache */
    if ((E_AVAILABLE == p_copy_aes_keys_fix->availability) &&
        (KEY_GRP_AES == p_copy_aes_keys_fix->key_group) &&
        ((uint32_t)vm_id == (uint32_t)p_copy_aes_keys_fix->vm_id) &&
        ((uint32_t)key_id == (uint32_t)p_copy_aes_keys_fix->key_id))
    {
        /* There is key data in the cache */
        *p_key = p_copy_aes_keys_fix;
    }
    else
    {
        /* No key data in cache */
        /* Get key data from Flash */
        err = MM_FLS_Read(FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, FLS_DS_AES_KEY,
                          (uint8_t)((uint32_t)ML_CFG_VM_AES_IDX_OFFSET(vm_id) + (uint32_t)key_id),
                          MH_KEY_AES_DS_NB_BLOCKS * NB_FLASH_BLOCK_NUMBER, (uint8_t*)&key_slot);
        if (ER_OK == err)
        {
            p_copy_aes_keys_fix->vm_id        = vm_id;
            p_copy_aes_keys_fix->key_group    = KEY_GRP_AES;
            p_copy_aes_keys_fix->key_id       = key_id;
            L_Toolbox_MemCpy((uint8_t *)&p_copy_aes_keys_fix->properties,
                             (const uint8_t *)&key_slot.properties,
                             (int16_t)sizeof(aes_key_prop_t));
            (void)L_Toolbox_MemCpy_Sec((uint8_t *)p_copy_aes_keys_fix->value,
                             (const uint8_t *)key_slot.value,
                             (int16_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON);
            p_copy_aes_keys_fix->availability = E_AVAILABLE;
            *p_key = p_copy_aes_keys_fix;
        }
        else
        {
            /* do nothing */
        }
    }

    if (ER_OK != err)
    {
        err = mh_KEY_ConvertErr( err );
        *p_key = NULL;
    }
    else
    {
        /* do nothing */
    }
    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0010]:[ICUMFW_UD_MH_KeyMNG_0010]
* Function name: mh_KEY_GetAesCommonChange
* Description  : Get AES key in change key area
* Arguments    : p_job
*              : key_id
*              : p_key
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_SLOT_EMPTY
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
static uint32_t mh_KEY_GetAesCommonChange(icum_job_t *p_job, r_key_index_t key_id, aes_key_t **p_key)
{
    aes_key_slot_t      key_slot;
    uint32_t            err = ER_OK;
    uint32_t i;
    uint8_t idx;
    uint8_t find_result;

    i = NUM0;
    key_slot.value[0] = 0x00u;
    *p_key = NULL;
    find_result = MH_KEY_IN_USE_MNG_TABLE_NOT_FOUND;
    idx = g_current_idx_aes;

    /* Check if there is key data in the cache */
    for (i = NUM0; i < NB_RAM_COPIES_AES_KEYS_CHANGE; i++)
    {
        if ((E_AVAILABLE == g_copy_aes_keys_change[idx].availability) &&
            (KEY_GRP_AES == g_copy_aes_keys_change[idx].key_group) &&
            ((uint32_t)p_job->vm_id == (uint32_t)g_copy_aes_keys_change[idx].vm_id) &&
            ((uint32_t)key_id == (uint32_t)g_copy_aes_keys_change[idx].key_id))
        {
            /* There is key data in the cache */
            *p_key = &g_copy_aes_keys_change[idx];
            g_current_idx_aes = idx;
            break;
        }
        else if ((E_AVAILABLE == g_copy_aes_keys_change[idx].availability))
        {
            /* Is cache update possible? */
            (void)mh_KEY_FindKeyIdInUseMngTable(g_copy_aes_keys_change[idx].vm_id,
                                                g_copy_aes_keys_change[idx].key_group,
                                                g_copy_aes_keys_change[idx].key_id,
                                                &find_result);
            if ((uint32_t)MH_KEY_IN_USE_MNG_TABLE_FOUND == (uint32_t)find_result)
            {
#if defined(FSW_NB_AES_CACHE_1)
                /* Key exchange not possible */
                err = ER_MH_KEY_NOT_USABLE;
#endif /* FSW_NB_AES_CACHE_1 */
            }
            else
            {
                g_current_idx_aes = idx;
            }
        }
        else
        {
            g_current_idx_aes = idx;
        }

        if (NB_RAM_COPIES_AES_KEYS_CHANGE <= ((uint32_t)idx + NUM1))
        {
            idx = 0x00u;
        }
        else
        {
            idx = (uint8_t)((uint32_t)idx + NUM1);
        }
    } /* end of for (i = 0uL; i < NB_RAM_COPIES_AES_KEYS_CHANGE; i++) */

    if ((NULL == *p_key) && (ER_OK == err))
    {
        /* Get key data from Flash */
        err = MM_FLS_Read(FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, FLS_DS_AES_KEY,
                            (uint8_t)((uint32_t)ML_CFG_VM_AES_IDX_OFFSET(p_job->vm_id) + (uint32_t)key_id),
                            MH_KEY_AES_DS_NB_BLOCKS * NB_FLASH_BLOCK_NUMBER, (uint8_t*)&key_slot);
        if (ER_OK == err)
        {
            g_copy_aes_keys_change[g_current_idx_aes].vm_id        = p_job->vm_id;
            g_copy_aes_keys_change[g_current_idx_aes].key_group    = KEY_GRP_AES;
            g_copy_aes_keys_change[g_current_idx_aes].key_id       = key_id;
            L_Toolbox_MemCpy((uint8_t *)&g_copy_aes_keys_change[g_current_idx_aes].properties,
                                (const uint8_t *)&key_slot.properties,
                                (int16_t)sizeof(aes_key_prop_t));
            (void)L_Toolbox_MemCpy_Sec((uint8_t *) g_copy_aes_keys_change[g_current_idx_aes].value,
                                (const uint8_t *)key_slot.value,
                                (int16_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON);
            *p_key = &g_copy_aes_keys_change[g_current_idx_aes];
            g_copy_aes_keys_change[g_current_idx_aes].availability = E_AVAILABLE;
        } /* end of if (ER_OK == err) */
        else
        {
            /* do nothing */
        }
    } /* end of if ((NULL == *p_key) && (ER_OK == err)) */
    else
    {
        /* do nothing */
    }

    if (ER_OK != err)
    {
        err = mh_KEY_ConvertErr(err);
        *p_key = NULL;
    }
    else
    {
        /* do nothing */
    }
    return (err);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0007]:[ICUMFW_UD_MH_KeyMNG_0007]
* Function name: MH_KEY_GetAesKey
* Description  : AES key acquisition function (Use key in-use counter)
* Arguments    : p_job
*              : key_group
*              : key_id
*              : p_key
*              : in_use_enable_flag
* Return Value : ER_OK
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
uint32_t MH_KEY_GetAesKey(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id,
    aes_key_t **p_key, uint8_t in_use_enable_flag)
{
    uint32_t err;
    uint16_t write_sts;
    sys_cfg_vm_t vm_cfg;

    /* Initialize local variable */
    err = ER_OK;
    write_sts = KEY_NOT_WRITING;
    *p_key = NULL;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));

    (void)mh_KEY_WritingJudgment(p_job, key_group, key_id, &write_sts);
    if ((uint32_t)KEY_WRITING == (uint32_t)write_sts)
    {
        err = ER_MH_KEY_IN_USE;
    }
    else
    {
        /* Check the range of VMID */
        err = mh_KEY_CheckVMID(p_job->vm_id);
    }

    if (ER_OK == err)
    {
        /* retrieve the key to be used */
        switch (key_group)
        {
            case KEY_GRP_AES:
                err = mh_KEY_GetAesCommon(p_job, key_id, p_key, in_use_enable_flag);
                break;
            case KEY_GRP_AES_RAM:
                err = mh_KEY_GetAesRam(p_job, key_id, p_key, in_use_enable_flag);
                break;
            case KEY_GRP_AES_CR:
                /* Get VM config */
                (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
                /* Only privileged VM can get AES CR */
                if ((uint32_t)vm_cfg.privilege_vm_id != (uint32_t)p_job->vm_id)
                {
                    /* not privileged VM: stop here */
                    err = ER_MH_KEY_NOT_USABLE;
                    *p_key = NULL;
                }
                else if ((uint32_t)MH_KEY_NB_CR_KEYS <= (uint32_t)key_id)
                {
                    /* invalid key index: stop here */
                    err = ER_MH_KEY_INVALID_KEY_INDEX;
                    *p_key = NULL;
                }
                else
                {
                    err = mh_KEY_GetAesCr_NotUse(key_id, p_key);
                }
                break;
            default:
                err = ER_MH_KEY_INVALID_KEY_GROUP;
                *p_key = NULL;
                break;
        } /* end of switch (key_group) */
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    return(err);
}
#pragma ghs section text    = ".icum_middle_ram"
#pragma ghs section rodata = ".icum_middle_const_ram"
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0014]:[ICUMFW_UD_MH_KeyMNG_0014]
* Function name: MH_KEY_GetAesRam_fast
* Description  : Retrieve the p_key to the AES RAM key value & properties for a given key idx
*              : Returns empty if the key is not available (i.e. not initialized in Flash)
*              : High-speed processing version dedicated to CMAC Fast
* Arguments    : vm_id
*              : key_id
*              : p_key
* Return Value : ER_OK
*              : ER_MH_KEY_SLOT_EMPTY
*              : ER_MH_KEY_INVALID_KEY_INDEX
***************************************************************************************/
uint32_t MH_KEY_GetAesRam_fast(uint8_t vm_id, r_key_index_t key_id, aes_key_t **p_key)
{
    uint32_t err = ER_OK;
    aes_key_t *p_ram_aes_keys;

    if ((uint32_t)g_nb_aes_ram[vm_id] <= (uint32_t)key_id)
    {
        /* invalid key index: stop here */
        err = ER_MH_KEY_INVALID_KEY_INDEX;
         *p_key = NULL;
    }
    else
    {
        p_ram_aes_keys = RAM_AES_KEYS(vm_id, key_id);

        /* is the key available (previously initialized)? */
        if (p_ram_aes_keys->availability == E_AVAILABLE)
        {
            /* found: stop here */
            *p_key = p_ram_aes_keys;
        }
        else
        {
            /* RAM key not available (not initialized): stop here */
            err = ER_MH_KEY_SLOT_EMPTY;
            *p_key = NULL;
        }
    } /* end of else */

    return (err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0013]:[ICUMFW_UD_MH_KeyMNG_0013]
* Function name: MH_KEY_GetAesKeyCache_fast
* Description  : Retrieve the p_key to the AES key value & properties for a given key idx
*              : Returns empty if the key is not available (i.e. not initialized in Flash)
*              : High-speed processing version dedicated to CMAC Fast
*              : Check only the keys on the Cache area
* Arguments    : vm_id
*              : key_id
*              : p_key
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_MH_KEY_NOT_USABLE
*              : ER_MH_KEY_SLOT_EMPTY
***************************************************************************************/
uint32_t MH_KEY_GetAesKeyCache_fast(uint8_t vm_id, r_key_index_t key_id, aes_key_t **p_key)
{
    uint32_t i;
    uint32_t err;
    aes_key_t* p_copy_aes_keys_fix;
    sys_cfg_vm_t vm_cfg;
    sys_cfg_key_mapping_t key_mapping;

    /* Initialize local variable */
    i = 0uL;
    err = ER_MH_KEY_SLOT_EMPTY;
    p_copy_aes_keys_fix = NULL;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));
    L_Toolbox_MemSet((uint8_t *)&key_mapping, 0x00u, (int16_t)sizeof(sys_cfg_key_mapping_t));

    /* Check the range of VMID */
    (void)ML_CFG_GetSys_ram(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
    if ((uint32_t)vm_id >= (uint32_t)vm_cfg.nb_vms)
    {
        err = ER_MH_KEY_NOT_USABLE;
    }
    else
    {
        err = ER_MH_KEY_SLOT_EMPTY;
        /* Get Key mapping for VM */
        (void)ML_CFG_GetSysVM_ram(vm_id, ML_CFG_SYS_KEY_MAPPING, (uint32_t *)&key_mapping);
        if ((uint32_t)key_mapping.nb_aes_keys <= (uint32_t)key_id)
        {
            err = ER_PARAM;
        }
        else if ((MH_KEY_AES_BUFFER_KEY_ID_START <= (uint32_t)key_id) &&
                ((MH_KEY_AES_BUFFER_KEY_ID_START + (uint32_t)key_mapping.nb_aes_buffer_keys) > (uint32_t)key_id))
        {
            p_copy_aes_keys_fix = COPY_AES_KEYS_FIX(vm_id, key_id);
            /* fix : AES buffer key */
            if ((E_AVAILABLE == p_copy_aes_keys_fix->availability) &&
                (KEY_GRP_AES == p_copy_aes_keys_fix->key_group) &&
                ((uint32_t)vm_id == (uint32_t)p_copy_aes_keys_fix->vm_id) &&
                ((uint32_t)key_id == (uint32_t)p_copy_aes_keys_fix->key_id))
            {
                /* found: stop here */
                *p_key = p_copy_aes_keys_fix;
                err = ER_OK;
            }
            else
            {
                /* do nothing */
            }
        } /* end of else if ((MH_KEY_AES_BUFFER_KEY_ID_START <= (uint32_t)key_id) && ... */
        else
        {
            /* Change : AES cache key */
            for (i = 0uL; i < (uint32_t)NB_RAM_COPIES_AES_KEYS_CHANGE; i++)
            {
                if ((E_AVAILABLE == g_copy_aes_keys_change[i].availability) &&
                    (KEY_GRP_AES == g_copy_aes_keys_change[i].key_group) &&
                    ((uint32_t)vm_id == (uint32_t)g_copy_aes_keys_change[i].vm_id) &&
                    ((uint32_t)key_id == (uint32_t)g_copy_aes_keys_change[i].key_id))

                {
                    *p_key = &g_copy_aes_keys_change[i];
                    err = ER_OK;
                    break;
                }
                else
                {
                    /* do nothing */
                }
            } /* end of for (i = 0uL; i < (uint32_t)NB_RAM_COPIES_AES_KEYS_CHANGE; i++) */
        } /* end of else */
    } /* end of else */

    return (err);
}

#pragma ghs section text    = default
#pragma ghs section rodata  = default
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0012]:[ICUMFW_UD_MH_KeyMNG_0012]
* Function name: mh_KEY_GetAesCr_NotUse
* Description  : Retrieve the p_key to the AES C&R key value & properties for a given key idx
*              : Returns empty if the key is not available (i.e. not initialized in Flash)
* Arguments    : key_id
*              : p_key
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_SLOT_EMPTY
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
static uint32_t mh_KEY_GetAesCr_NotUse( r_key_index_t key_id, aes_key_t **p_key)
{
    aes_key_slot_t      key_slot;
    uint32_t err;
    key_slot.value[0] = 0x00u;

    /* allocate space for the RAM copy of the CR_KEY */
    *p_key = &g_copy_CR_key;

    err = MM_FLS_Read(FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, FLS_DS_CR_KEY,
                         key_id, MH_KEY_AES_DS_NB_BLOCKS * NB_FLASH_BLOCK_NUMBER,
                         (uint8_t*)&key_slot);
    if( err == ER_OK )
    {
        (*p_key)->key_group    = KEY_GRP_AES_CR;
        (*p_key)->key_id       = key_id;
        (void)L_Toolbox_MemCpy_Sec((uint8_t *)(*p_key)->value, (const uint8_t *)key_slot.value,
                            MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON);
        (*p_key)->availability = E_AVAILABLE;
    }
    else
    {
        /* Do nothing */
    }

    if( err != ER_OK )
    {
        err = mh_KEY_ConvertErr( err );
        *p_key = NULL;
    }
    else
    {
        /* Do nothing */
    }

    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0018]:[ICUMFW_UD_MH_KeyMNG_0018]
* Function name: mh_KEY_UpdateAesRam
* Description  : Update an AES RAM key
* Arguments    : p_job
*              : p_new_key
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_MH_KEY_IN_USE
***************************************************************************************/
static uint32_t mh_KEY_UpdateAesRam(icum_job_t *p_job, aes_key_t *p_new_key)
{
    aes_key_t *p_key;
    uint32_t err;
    uint8_t vm_id;
    uint8_t key_number;
    uint8_t find_result;

    /* Initialize local variable */
    err = ER_OK;
    vm_id = p_job->vm_id;
    key_number = 0x00u;
    find_result = MH_KEY_IN_USE_MNG_TABLE_NOT_FOUND;
    p_key = NULL;

    (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_AES_RAM, &key_number);
    if ((uint32_t)key_number <= (uint32_t)p_new_key->key_id)
    {
        /* invalid key index */
        err = ER_MH_KEY_INVALID_KEY_INDEX;
    }
    else
    {
        /* Is cache update possible? */
        (void)mh_KEY_FindKeyIdInUseMngTable(vm_id,
                                            p_new_key->key_group, p_new_key->key_id, &find_result);
        if ((uint32_t)MH_KEY_IN_USE_MNG_TABLE_NOT_FOUND == (uint32_t)find_result)
        {
            /* key id and key group are missing */
            /* Updatable */
            /* allocate the key in RAM */
            p_key = RAM_AES_KEYS(vm_id, p_new_key->key_id);

            /* set the property and values */
            L_Toolbox_MemCpy((uint8_t *)&p_key->properties, (const uint8_t *)&p_new_key->properties,
                                (int16_t)sizeof(aes_key_prop_t));
            L_Toolbox_MemCpy((uint8_t *)p_key->value, p_new_key->value, (int16_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON);

            p_key->key_group    = p_new_key->key_group;
            p_key->key_id       = p_new_key->key_id;
            p_key->vm_id        = vm_id;
            p_key->availability = E_AVAILABLE;
        } /* end of if ((uint32_t)MH_KEY_IN_USE_MNG_TABLE_NOT_FOUND == (uint32_t)find_result) */
        else
        {
            err = ER_MH_KEY_IN_USE;
        }
    } /* end of else */

    return(err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0017]:[ICUMFW_UD_MH_KeyMNG_0017]
* Function name: mh_KEY_UpdateAesCr
* Description  : Update C&R key data to Data Flash
* Arguments    : p_job
*              : operation
*              : p_new_key
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_IN_PROCESS
*              : ER_MH_KEY_NVM_SYS_CRITICAL
*              : ER_MH_KEY_NVM_SYS
*              : ER_MH_KEY_NVM_PROG
*              : ER_MH_KEY_DATA_INVALID
*              : ER_MH_KEY_SYS_ERROR
*              : ER_MH_KEY_NVM_UNAVAILABLE
*              : ER_MH_KEY_NVM_SIZE
*              : ER_MH_KEY_DEST_MEM_ERROR
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
***************************************************************************************/
uint32_t mh_KEY_UpdateAesCr(icum_job_t *p_job, set_operation_t operation, aes_key_t *p_new_key)
{
    uint32_t err;

    /* update sequence */
    if (E_KEY_OPE_FIRST == operation)
    {
        (void)mh_KEY_WritingSet(p_job->vm_id, p_new_key->key_group, p_new_key->key_id);
        L_Toolbox_MemSet((uint8_t*)&g_storage_keys_aes, 0x00u, (int16_t)sizeof(aes_key_slot_t));
        (void)L_Toolbox_MemCpy_Sec(g_storage_keys_aes.value, p_new_key->value,
                            (int16_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON);
    }
    else
    {
        /* Do nothing */
    }

    err = MM_FLS_Write(p_job->job_id, FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, FLS_DS_CR_KEY,
                       p_new_key->key_id, MH_KEY_AES_DS_NB_BLOCKS * NB_FLASH_BLOCK_NUMBER,
                       (uint8_t*)&g_storage_keys_aes);

    if ((ER_IN_PROCESS != err) && (ER_MM_FLS_IN_PROGRESS != err))
    {
        /* Key writing is completed. Clear write information */
        (void)mh_KEY_WritingSet((uint8_t)NUM0, LAST_ITEM_KEY_GRP, KEY_ID_LAST_ITEM);
    }
    else
    {
        /* Do nothing */
    }
    err = mh_KEY_ConvertErr(err);
    return(err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0016]:[ICUMFW_UD_MH_KeyMNG_0016]
* Function name: mh_KEY_UpdateAes
* Description  : Update AES key data to Data Flash
* Arguments    : p_job
*              : operation
*              : p_new_key
* Return Value : ER_OK
*              : ER_PENDING
*              : ER_IN_PROCESS
*              : ER_MH_KEY_IN_USE
*              : ER_MH_KEY_WRITE_PROTECTED
*              : ER_MH_KEY_NVM_SYS_CRITICAL
*              : ER_MH_KEY_NVM_SYS
*              : ER_MH_KEY_NVM_PROG
*              : ER_MH_KEY_DATA_INVALID
*              : ER_MH_KEY_SYS_ERROR
*              : ER_MH_KEY_NVM_UNAVAILABLE
*              : ER_MH_KEY_NVM_SIZE
*              : ER_MH_KEY_SRC_MEM_ERROR
*              : ER_MH_KEY_DEST_MEM_ERROR
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
***************************************************************************************/
static uint32_t mh_KEY_UpdateAes(icum_job_t *p_job, set_operation_t operation, aes_key_t *p_new_key)
{
#pragma alignvar(4)
    aes_key_slot_t key_slot;
    uint32_t i;
    uint32_t err;
    uint8_t find_result;
    sys_cfg_key_mapping_t key_mapping;
    uint8_t vm_id;

    /* Initialize local variable */
    i = NUM0;
    err = ER_OK;
    vm_id = p_job->vm_id;
    find_result = MH_KEY_IN_USE_MNG_TABLE_NOT_FOUND;
    L_Toolbox_MemSet((uint8_t *)&key_mapping, 0x00u, (int16_t)sizeof(sys_cfg_key_mapping_t));
    key_slot.properties.flags.write_protected = MH_KEY_FLAG_NOT_WRITE_PROTECT;

    if (E_KEY_OPE_FIRST == operation)
    {
        (void)ML_CFG_GetSysVM(vm_id, ML_CFG_SYS_KEY_MAPPING, (uint32_t *)&key_mapping);
        if ((uint32_t)key_mapping.nb_aes_keys <= (uint32_t)p_new_key->key_id)
        {
            /* invalid key index: stop here */
            err = ER_MH_KEY_INVALID_KEY_INDEX;
        }
        else
        {
            /* check if the key to update is in RAM cache */
            /* scan the in-use table (it may be there!) */
            /* Is cache update possible? */
            (void)mh_KEY_FindKeyIdInUseMngTable(vm_id, p_new_key->key_group, p_new_key->key_id, &find_result);
            if ((uint32_t)MH_KEY_IN_USE_MNG_TABLE_NOT_FOUND == (uint32_t)find_result)
            {
                /* key id and key group are missing */
                /* Updatable */
                /* mark the key as "not available" */
                if ((MH_KEY_AES_BUFFER_KEY_ID_START <= (uint32_t)p_new_key->key_id) &&
                    ((MH_KEY_AES_BUFFER_KEY_ID_START + (uint32_t)key_mapping.nb_aes_buffer_keys) > (uint32_t)p_new_key->key_id))
                {
                    COPY_AES_KEYS_FIX(vm_id, p_new_key->key_id)->availability = E_NOT_AVAILABLE;
                }
                else
                {
                    /* Check if there is key data in the cache */
                    for (i = 0uL; i < NB_RAM_COPIES_AES_KEYS_CHANGE; i++)
                    {
                        if ((E_AVAILABLE == g_copy_aes_keys_change[i].availability) &&
                            (p_new_key->key_group == g_copy_aes_keys_change[i].key_group) &&
                            ((uint32_t)p_job->vm_id == (uint32_t)g_copy_aes_keys_change[i].vm_id) &&
                            ((uint32_t)p_new_key->key_id == (uint32_t)g_copy_aes_keys_change[i].key_id))
                        {
                            /* There is key data in the cache */
                            g_copy_aes_keys_change[i].availability = E_NOT_AVAILABLE;
                        }
                        else
                        {
                            /* do nothing */
                        }
                    }
                } /* end of else */
            } /* end of if ((uint32_t)MH_KEY_IN_USE_MNG_TABLE_NOT_FOUND == (uint32_t)find_result) */
            else
            {
                err = ER_MH_KEY_IN_USE;
            }

            if (ER_OK == err)
            {
                /* retrieve the current slot value (if available) */
                err = MM_FLS_Read(FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, FLS_DS_AES_KEY,
                                (uint8_t)((uint32_t)ML_CFG_VM_AES_IDX_OFFSET(vm_id) + (uint32_t)p_new_key->key_id),
                                MH_KEY_AES_DS_NB_BLOCKS * NB_FLASH_BLOCK_NUMBER,
                                (uint8_t*)&key_slot);
                if (ER_OK == err)
                {
                    /* make sure the key is not write protected */
                    if ((uint32_t)MH_KEY_FLAG_WRITE_PROTECT == (uint32_t)key_slot.properties.flags.write_protected)
                    {
                        err = ER_MH_KEY_WRITE_PROTECTED;
                    }
                    else
                    {
                        /* in case of plain key update, increase the counter */
                        /* in case of SHE key update, use update_counter value */
                        /* If update_counter is 0, it means "plain key update" */
                        if (0uL == p_new_key->properties.update_counter)
                        {
                            p_new_key->properties.update_counter = (key_slot.properties.update_counter + 1u);
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                } /* end of if (ER_OK == err) */
                else if (ER_MM_FLS_DATA_BLANK == err)
                {
                    /* in case of plain key update, increase the counter */
                    /* in case of SHE key update, use update_counter value */
                    /* If update_counter is 0, it means "plain key update" */
                    if (0uL == p_new_key->properties.update_counter)
                    {
                        p_new_key->properties.update_counter = 1U;
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
            } /* end of if (ER_OK == err) */
            else
            {
                /* Do nothing */
            }

            if ((ER_OK == err) || (ER_MM_FLS_DATA_BLANK == err))
            {
                (void)mh_KEY_WritingSet(vm_id, p_new_key->key_group, p_new_key->key_id);
                L_Toolbox_MemCpy((uint8_t *)&g_storage_keys_aes.properties, (const uint8_t *)&p_new_key->properties,
                                    (int16_t)sizeof(aes_key_prop_t));
                (void)L_Toolbox_MemCpy_Sec(g_storage_keys_aes.value, p_new_key->value,
                                    MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON);
                /* update sequence */
                err = MM_FLS_Write(p_job->job_id, FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, FLS_DS_AES_KEY,
                                (uint8_t)((uint32_t)ML_CFG_VM_AES_IDX_OFFSET(vm_id) + (uint32_t)p_new_key->key_id),
                                MH_KEY_AES_DS_NB_BLOCKS * NB_FLASH_BLOCK_NUMBER, (uint8_t*)&g_storage_keys_aes);

                if ((ER_IN_PROCESS != err) && (ER_MM_FLS_IN_PROGRESS != err))
                {
                    (void)mh_KEY_WritingSet((uint8_t)NUM0, LAST_ITEM_KEY_GRP, KEY_ID_LAST_ITEM);
                }
                else
                {
                    /* Do nothing */
                }
            } /* end of if ((ER_OK == err) || (ER_MM_FLS_DATA_BLANK == err)) */
            else
            {
                /* Do nothing */
            }
        } /* end of else */
    } /* end of if (E_KEY_OPE_FIRST == operation) */
    else
    {
        /* update sequence */
        err = MM_FLS_Write(p_job->job_id, FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, FLS_DS_AES_KEY,
                           (uint8_t)((uint32_t)ML_CFG_VM_AES_IDX_OFFSET(vm_id) + (uint32_t)p_new_key->key_id),
                           MH_KEY_AES_DS_NB_BLOCKS * NB_FLASH_BLOCK_NUMBER,
                           (uint8_t*)&g_storage_keys_aes);

        if ((err != ER_IN_PROCESS) && (err != ER_MM_FLS_IN_PROGRESS))
        {
            (void)mh_KEY_WritingSet((uint8_t)NUM0, LAST_ITEM_KEY_GRP, KEY_ID_LAST_ITEM);
        }
        else
        {
            /* Do nothing */
        }
    }

    err = mh_KEY_ConvertErr(err);
    return(err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0015]:[ICUMFW_UD_MH_KeyMNG_0015]
* Function name: MH_KEY_SetAesKey
* Description  : AES key Update(Sort processing by key_group)
* Arguments    : p_job
*              : operation
*              : p_new_key
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_IN_PROCESS
*              : ER_MH_KEY_INVALID_KEY_GROUP
*              : ER_MH_KEY_INVALID_KEY_INDEX
*              : ER_MH_KEY_IN_USE
*              : ER_MH_KEY_NOT_USABLE
*              : ER_MH_KEY_WRITE_PROTECTED
*              : ER_MH_KEY_WRITING
*              : ER_MH_KEY_NVM_SYS_CRITICAL
*              : ER_MH_KEY_NVM_SYS
*              : ER_MH_KEY_NVM_PROG
*              : ER_MH_KEY_DATA_INVALID
*              : ER_MH_KEY_SYS_ERROR
*              : ER_MH_KEY_NVM_UNAVAILABLE
*              : ER_MH_KEY_NVM_SIZE
*              : ER_MH_KEY_SRC_MEM_ERROR
*              : ER_MH_KEY_DEST_MEM_ERROR
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
***************************************************************************************/
uint32_t MH_KEY_SetAesKey(icum_job_t *p_job, set_operation_t operation, aes_key_t *p_new_key)
{
    uint32_t err;
    uint16_t write_sts;
    sys_cfg_vm_t vm_cfg;

    /* Initialize local variable */
    err = ER_OK;
    write_sts = KEY_NOT_WRITING;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));

    (void)mh_KEY_WritingStsGet(&write_sts);
    if (((uint32_t)KEY_WRITING == (uint32_t)write_sts) && (E_KEY_OPE_FIRST == operation))
    {
        err = ER_MH_KEY_IN_USE;
    }
    else
    {
        /* Check the range of VMID */
        err = mh_KEY_CheckVMID(p_job->vm_id);
    }

    if (ER_OK == err)
    {
        /* retrieve the key to be used */
        switch(p_new_key->key_group)
        {
            case KEY_GRP_AES:
                err = mh_KEY_UpdateAes(p_job, operation, p_new_key);
                break;
            case KEY_GRP_AES_CR:
                /* Get VM config */
                (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
                /* Only privileged VM can get AES CR */
                if ((uint32_t)vm_cfg.privilege_vm_id != (uint32_t)p_job->vm_id)
                {
                    /* not privileged VM: stop here */
                    err = ER_MH_KEY_NOT_USABLE;
                }
                else if ((uint32_t)MH_KEY_NB_CR_KEYS <= (uint32_t)p_new_key->key_id)
                {
                    /* invalid key index: stop here */
                    err = ER_MH_KEY_INVALID_KEY_INDEX;
                }
                else
                {
                    err = mh_KEY_UpdateAesCr(p_job, operation, p_new_key);
                }
                break;
            case KEY_GRP_AES_RAM:
                err = mh_KEY_UpdateAesRam(p_job, p_new_key);
                break;
            default:
                err = ER_MH_KEY_INVALID_KEY_GROUP;
                break;
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }
    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0039]:[ICUMFW_UD_MH_KeyMNG_0039]
* Function name: MH_KEY_GetSecretKey
* Description  : constants for SECRET_KEY generation
* Preconditions: AES usage authority already acquired
* Arguments    : vm_id
*              : p_keydata
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_MH_KEY_NOT_USABLE
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
*              : SERV_PENDING
***************************************************************************************/
uint32_t MH_KEY_GetSecretKey(uint8_t vm_id, uint32_t* p_keydata)
{
    uint32_t err;
    aes_all_at_once_t at_once_param;

    L_Toolbox_MemSet((uint8_t *)&at_once_param, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_t));

    /* Checks the range of VMID */
    err = mh_KEY_CheckVMID(vm_id);
    if (ER_OK == err)
    {
        g_ContextAddress          = AES_STATE_INIT;
        at_once_param.cipher_mode = D_AES_CIPHER_MODE_ECB;
        at_once_param.direction   = (uint8_t)D_AES_DIR_ENCRYPTION;
        at_once_param.number      = (uint32_t)((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_128BIT
                                        / (uint32_t)AES_BLOCK_SIZE_IN_BYTES);
        at_once_param.key_type    = D_AES_KEYTYPE_ROMKEY_0;
        at_once_param.p_aes_key   = (uint32_t *)CONST_ZERO;
        at_once_param.key_length  = D_AES_KEY_SIZE_IN_BYTES_128BIT;
        at_once_param.p_aes_iv    = (uint32_t *)CONST_ZERO;
        at_once_param.p_aes_src   = (uint32_t*)&RANDOMNUMBER_SECRET_C[vm_id];
        at_once_param.p_aes_dest  = (uint32_t*)p_keydata;
        at_once_param.last_block_in_bits = (uint16_t)NUM0;
        at_once_param.async              = D_AES_TRANS_SYNC;

        err = D_AES_ExecutionAtOnce(&at_once_param);

        err = mh_KEY_ConvertErr( err );
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0041]:[ICUMFW_UD_MH_KeyMNG_0041]
* Function name: mh_KEY_DeleteRamKey
* Description  : Delete AES RAM key
* Arguments    : p_job
* Return Value : ER_OK
*              : ER_MH_KEY_IN_USE
***************************************************************************************/
static uint32_t mh_KEY_DeleteRamKey(icum_job_t *p_job)
{
    uint32_t i;
    uint32_t vm_id;
    uint32_t err;
    uint8_t key_number;
    sys_cfg_vm_t vm_cfg;
    aes_key_t *p_ram_aes_keys;

    /* Initialize local variable */
    i = NUM0;
    vm_id = NUM0;
    err = ER_OK;
    key_number = 0x00u;
    p_ram_aes_keys = NULL;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));

    for (i = NUM0; i < NB_AES_IN_USE_MNG_TABLE; i++)
    {
        if (E_AVAILABLE == g_aes_in_use_mng_table[i].availability)
        {
            err = ER_MH_KEY_IN_USE;
            break;
        }
        else
        {
            /* do nothing */
        }
    } /* end of for (i = NUM0; i < NB_AES_IN_USE_MNG_TABLE; i++) */

    if (ER_OK == err)
    {
        (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
        for (vm_id = NUM0; vm_id < (uint32_t)vm_cfg.nb_vms; vm_id++)
        {
            (void)mh_KEY_MapSizeVM((uint8_t)vm_id, KEY_MAPPING_AES_RAM, &key_number);
            /** AES NVM data write protection check **/
            for (i = NUM0; i < (uint32_t)key_number; i++)
            {
                p_ram_aes_keys = RAM_AES_KEYS(vm_id, i);
                p_ram_aes_keys->availability = E_NOT_AVAILABLE;
                p_ram_aes_keys->key_group = KEY_GRP_AES_ROM;
                p_ram_aes_keys->key_id = 0x00u;
                p_ram_aes_keys->vm_id = 0x00u;
                L_Toolbox_MemSet((uint8_t*)&p_ram_aes_keys->properties, 0x00u, (int16_t)sizeof(aes_key_prop_t));
                L_Toolbox_MemSet(p_ram_aes_keys->value, 0x00u, (int16_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON);
            }
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* do nothing */
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0040]:[ICUMFW_UD_MH_KeyMNG_0040]
* Function name: MH_KEY_DeleteRamKeyData
* Description  : Clear key data in RAM area
* Arguments    : p_job
*              : key_type
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_MH_KEY_IN_USE
***************************************************************************************/
uint32_t MH_KEY_DeleteRamKeyData(icum_job_t *p_job, del_key_type_t key_type)
{
    uint32_t err = ER_OK;

    switch( key_type )
    {
        case E_DELKEY_TYPE_RAM:
            err = mh_KEY_DeleteRamKey(p_job);
            break;
        case E_DELKEY_TYPE_TEMP:
            g_copy_CR_key.availability = E_NOT_AVAILABLE;
            L_Toolbox_MemSet(g_copy_CR_key.value, 0x00u, (int16_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON);
            break;
        default:
            err = ER_PARAM;
            break;
    }
    
    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0079]:[ICUMFW_UD_MH_KeyMNG_0079]
* Function name: mh_KEY_KeyCacheSet
* Description  : Set an AES key in cache
* Arguments    : p_job
* Return Value : ER_OK
*              : ER_PENDING
*              : ER_MH_KEY_IN_USE
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
static uint32_t mh_KEY_KeyCacheSet(icum_job_t *p_job)
{
    uint32_t err;
    aes_key_t *p_key;
    uint32_t i;
    bool wdt_flag;
    sys_cfg_key_mapping_t key_mapping;
    uint32_t loop_counter;

    /* Initialize local variable */
    i = NUM0;
    err = ER_OK;
    loop_counter = NUM0;
    L_Toolbox_MemSet((uint8_t *)&key_mapping, 0x00u, (int16_t)sizeof(sys_cfg_key_mapping_t));

    /* Get the mapping of keys */
    (void)ML_CFG_GetSysVM(p_job->vm_id, ML_CFG_SYS_KEY_MAPPING, (uint32_t *)&key_mapping);

    if ((NUM0 >= (uint32_t)key_mapping.nb_aes_buffer_keys) || 
        (MH_KEY_AES_BUFFER_KEY_ID_START >= (uint32_t)key_mapping.nb_aes_keys))
    {
        loop_counter = NUM0;
    }
    /* When the number of AES keys is less than the number of aes buffer key */
    else if (((uint32_t)MH_KEY_AES_BUFFER_KEY_ID_START + (uint32_t)key_mapping.nb_aes_buffer_keys) >= (uint32_t)key_mapping.nb_aes_keys)
    {
        loop_counter = (uint32_t)key_mapping.nb_aes_keys;
    }
    else
    {
        loop_counter = (uint32_t)key_mapping.nb_aes_buffer_keys + MH_KEY_AES_BUFFER_KEY_ID_START;
    }
    /* Loop for the number of key slots available in RAM */
    for (i = MH_KEY_AES_BUFFER_KEY_ID_START; (i < loop_counter) && (ER_OK == err); i++)
    {
        /* Import AES Key into buffer area */
        err = MH_KEY_GetAesKey(p_job, KEY_GRP_AES, (r_key_index_t)i, &p_key, false);
        D_WDT_GetIntRequest(&wdt_flag);
        if ((uint32_t)true == (uint32_t)wdt_flag)
        {
            /* restart watchdog timer */
            D_WDT_Start();
        }
        else
        {
            /* Do nothing */
        }
        /*  check err: is it ER_MH_KEY_SLOT_EMPTY?*/
        if (ER_MH_KEY_SLOT_EMPTY == err)
        {
            err = ER_OK;
        }
        else
        {
            /* Do nothing */
        }
    } /* end of for (i = MH_KEY_AES_BUFFER_KEY_ID_START; (i < loop_counter) && (ER_OK == err); i++) */

     return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0080]:[ICUMFW_UD_MH_KeyMNG_0080]
* Function name: mh_KEY_DeleteCacheKey
* Description  : Delete AES key in cache
* Arguments    : void
* Return Value : ER_OK
*              : ER_MH_KEY_IN_USE
***************************************************************************************/
static uint32_t mh_KEY_DeleteCacheKey(void)
{
    uint32_t i;
    uint32_t vm_id;
    uint32_t err;
    uint8_t key_number;
    sys_cfg_vm_t vm_cfg;
    aes_key_t* p_copy_aes_keys_fix;

    /* Initialize local variable */
    i = NUM0;
    err = ER_OK;
    vm_id = NUM0;
    key_number = 0x00u;
    p_copy_aes_keys_fix = NULL;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));

    /* Loop for the number of In-use management table (AES) */
    for (i = NUM0; i < NB_AES_IN_USE_MNG_TABLE; i++)
    {
        /* when there is the key in use */
        if (E_AVAILABLE == g_aes_in_use_mng_table[i].availability)
        {
            err = ER_MH_KEY_IN_USE;
            break;
        }
        else
        {
            /* do nothing */
        }
    }

    if (ER_OK == err)
    {
        /* Erase key data, repeat for number of Aes buffer keys */
        /* fix */
        (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
        for (vm_id = NUM0; vm_id < (uint32_t)vm_cfg.nb_vms; vm_id++)
        {
            (void)mh_KEY_MapSizeVM((uint8_t)vm_id, KEY_MAPPING_AES_BUFFER, &key_number);
            for (i = MH_KEY_AES_BUFFER_KEY_ID_START;
                 i < ((uint32_t)key_number + MH_KEY_AES_BUFFER_KEY_ID_START);
                 i++)
            {
                p_copy_aes_keys_fix = COPY_AES_KEYS_FIX(vm_id, i);
                p_copy_aes_keys_fix->availability = E_NOT_AVAILABLE;
                p_copy_aes_keys_fix->key_group = KEY_GRP_AES_ROM;
                p_copy_aes_keys_fix->vm_id = 0x00u;
                p_copy_aes_keys_fix->key_id = 0x00u;
                L_Toolbox_MemSet((uint8_t*)&p_copy_aes_keys_fix->properties, 0x00U, (int16_t)sizeof(aes_key_prop_t));
                L_Toolbox_MemSet(p_copy_aes_keys_fix->value, 0x00u, (int16_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON);
            }
        } /* end of for (vm_id = NUM0; vm_id < (uint32_t)vm_cfg.nb_vms; vm_id++) */
        /* change */
        for (i = NUM0; i < NB_RAM_COPIES_AES_KEYS_CHANGE; i++)
        {
            g_copy_aes_keys_change[i].availability = E_NOT_AVAILABLE;
            g_copy_aes_keys_change[i].key_group = KEY_GRP_AES_ROM;
            g_copy_aes_keys_change[i].vm_id = 0x00u;
            g_copy_aes_keys_change[i].key_id = 0x00u;
            L_Toolbox_MemSet((uint8_t*)&g_copy_aes_keys_change[i].properties, 0x00u, (int16_t)sizeof(aes_key_prop_t));
            L_Toolbox_MemSet(g_copy_aes_keys_change[i].value, 0x00u, (int16_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON);
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* do nothing */
    }
    return (err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0042]:[ICUMFW_UD_MH_KeyMNG_0042]
* Function name: MH_KEY_ConfigKeyCache
* Description  : Set or clear an AES key in cache
* Arguments    : p_job
*              : select
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
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
uint32_t MH_KEY_ConfigKeyCache(icum_job_t *p_job, key_cache_op_t select)
{
    uint32_t err;
    sys_cfg_vm_t vm_cfg;

    /* Initialize local variable */
    err = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));

    /* Get VM config */
    (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
    if ((uint32_t)p_job->vm_id >= (uint32_t)vm_cfg.nb_vms)
    {
        err = ER_MH_KEY_NOT_USABLE;
    }
    else
    {
        /* check parameter of select:set or clear AES key */
        switch (select)
        {
            /* In case of set AES key */
            case E_KEYCACHE_IN_BUFFER:
                err = mh_KEY_KeyCacheSet(p_job);
                break;
            /* In case of delete AES key */
            case E_KEYCACHE_CLEAR:
                if ((uint32_t)vm_cfg.privilege_vm_id == (uint32_t)p_job->vm_id)
                {
                    /* Only privileged VM can clear cache keys for all VMs */
                    err = mh_KEY_DeleteCacheKey();
                }
                else
                {
                    err = ER_MH_KEY_NOT_USABLE;
                }
                break;
            default:
                err = ER_PARAM;
                break;
        } /* end of switch (select) */
    } /* end of else */
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0072]:[ICUMFW_UD_MH_KeyMNG_0072]
* Function name: mh_KEY_CalculateMp
* Description  : Miyaguchi-Preneel calculation implementation
* Note         : Only supported when block_number is 2 and 3
* Arguments    : p_key_value
*              : p_const
*              : block_number
*              : p_deriv_key_value
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_D_AES_ALIGN_ERROR
*              : ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t mh_KEY_CalculateMp(
                const uint32_t *p_key_value, const uint8_t *p_const
                , uint32_t block_number, uint32_t *p_deriv_key_value)
{
    uint32_t mp_sorce[NUM12];
    uint32_t err;
    aes_all_at_once_t at_once_param;

    L_Toolbox_MemSet((uint8_t *)&at_once_param, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_t));

    if ((NUM2 == block_number)
        || (NUM3 == block_number))
    {
        if (NUM2 == block_number)
        {
            L_Toolbox_MemCpy((uint8_t *)&mp_sorce[NUM0], (const uint8_t *)p_key_value, AES_BLOCK_SIZE_IN_BYTES);
            L_Toolbox_MemCpy((uint8_t *)&mp_sorce[NUM4], p_const, AES_BLOCK_SIZE_IN_BYTES);
        }
        else
        {
            L_Toolbox_MemCpy((uint8_t *)&mp_sorce[NUM0], (const uint8_t *)p_key_value
                            , (int16_t)(NUM2 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
            L_Toolbox_MemCpy((uint8_t *)&mp_sorce[NUM8], p_const, AES_BLOCK_SIZE_IN_BYTES);
        }

        g_ContextAddress          = AES_STATE_INIT;
        at_once_param.cipher_mode = D_AES_CIPHER_MODE_MP;
        at_once_param.direction   = (uint8_t)D_AES_DIR_ENCRYPTION;
        at_once_param.number      = (uint32_t)((block_number * (uint32_t)AES_BLOCK_SIZE_IN_BYTES)
                                        / (uint32_t)AES_BLOCK_SIZE_IN_BYTES);
        at_once_param.key_type    = D_AES_KEYTYPE_PLAIN;
        at_once_param.p_aes_key   = (uint32_t *)CONST_ZERO;
        at_once_param.key_length  = D_AES_KEY_SIZE_IN_BYTES_128BIT;
        at_once_param.p_aes_iv    = (uint32_t *)CONST_ZERO;
        at_once_param.p_aes_src   = (uint32_t*)mp_sorce;
        at_once_param.p_aes_dest  = (uint32_t*)p_deriv_key_value;
        at_once_param.last_block_in_bits = (uint16_t)NUM0;
        at_once_param.async              = D_AES_TRANS_SYNC;

        err = D_AES_ExecutionAtOnce(&at_once_param);
        err = mh_KEY_ConvertErr(err);
    }
    else
    {
        err = ER_PARAM;
    }

    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0073]:[ICUMFW_UD_MH_KeyMNG_0073]
* Function name: mh_KEY_CalculateCmac
* Description  : CMAC calculation implementation
* Arguments    : p_key
*              : p_msg
*              : block_number
*              : p_cmac
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_D_AES_ALIGN_ERROR
*              : ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t mh_KEY_CalculateCmac(
                    const uint32_t *p_key, const uint32_t *p_msg, uint32_t block_number, uint32_t *p_cmac)
{
    uint32_t err;
    uint32_t msg_size_in_bits;
    uint16_t last_block;
    aes_all_at_once_t at_once_param;

    L_Toolbox_MemSet((uint8_t *)&at_once_param, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_t));

    /* AES_Cmac*/
    msg_size_in_bits = (block_number * (uint32_t)AES_BLOCK_SIZE_IN_BITS);

    at_once_param.cipher_mode = D_AES_CIPHER_MODE_CMAC;
    at_once_param.direction   = D_AES_DIR_ENCRYPTION;
    at_once_param.number      = ((msg_size_in_bits 
                                   + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1)) / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
    at_once_param.key_type    = D_AES_KEYTYPE_PLAIN;
    last_block = (uint16_t)(msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS);
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
    at_once_param.p_aes_key  = (uint32_t *)p_key;
    at_once_param.key_length = D_AES_KEY_SIZE_IN_BYTES_128BIT;
    at_once_param.p_aes_iv   = NULL;
    at_once_param.p_aes_src  = (uint32_t*)p_msg;
    at_once_param.p_aes_dest = (uint32_t*)p_cmac;
    at_once_param.async      = D_AES_TRANS_SYNC;

    err = D_AES_ExecutionAtOnce(&at_once_param);
    err = mh_KEY_ConvertErr(err);

    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0074]:[ICUMFW_UD_MH_KeyMNG_0074]
* Function name: mh_KEY_CalculateCbc
* Description  : CBC calculation implementation
* Arguments    : direction
*              : p_key
*              : p_input
*              : p_output
*              : block_number
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_D_AES_ALIGN_ERROR
*              : ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t mh_KEY_CalculateCbc(cipher_direction_t direction, const uint32_t *p_key,
                                       const uint32_t *p_input, uint32_t *p_output, uint32_t block_number)
{
    uint32_t err;
    aes_all_at_once_t at_once_param;

    L_Toolbox_MemSet((uint8_t *)&at_once_param, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_t));

    g_ContextAddress          = AES_STATE_INIT;
    at_once_param.cipher_mode = D_AES_CIPHER_MODE_CBC;
    at_once_param.direction   = (uint8_t)direction;
    at_once_param.number      = (uint32_t)((block_number * (uint32_t)AES_BLOCK_SIZE_IN_BYTES)
                                    / (uint32_t)AES_BLOCK_SIZE_IN_BYTES);
    at_once_param.key_type    = D_AES_KEYTYPE_PLAIN;
    at_once_param.p_aes_key   = (uint32_t *)p_key;
    at_once_param.key_length  = D_AES_KEY_SIZE_IN_BYTES_128BIT;
    at_once_param.p_aes_iv    = (uint32_t *)CONST_ZERO;
    at_once_param.p_aes_src   = (uint32_t*)p_input;
    at_once_param.p_aes_dest  = (uint32_t*)p_output;
    at_once_param.last_block_in_bits = (uint16_t)NUM0;
    at_once_param.async              = D_AES_TRANS_SYNC;
    err = D_AES_ExecutionAtOnce(&at_once_param);
    err = mh_KEY_ConvertErr(err);

    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0075]:[ICUMFW_UD_MH_KeyMNG_0075]
* Function name: mh_KEY_CalculateEcb
* Description  : ECB calculation implementation
* Arguments    : p_key
*              : p_input
*              : p_output
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_D_AES_ALIGN_ERROR
*              : ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t mh_KEY_CalculateEcb( const uint32_t *p_key, const uint32_t *p_input, uint32_t *p_output)
{
    uint32_t err;
    aes_all_at_once_t at_once_param;

    L_Toolbox_MemSet((uint8_t *)&at_once_param, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_t));

    g_ContextAddress          = AES_STATE_INIT;
    at_once_param.cipher_mode = D_AES_CIPHER_MODE_ECB;
    at_once_param.direction   = (uint8_t)D_AES_DIR_ENCRYPTION;
    at_once_param.number      = (uint32_t)((uint32_t)NB_AES_IN_USE_MNG_TABLE / (uint32_t)AES_BLOCK_SIZE_IN_BYTES);
    at_once_param.key_type    = D_AES_KEYTYPE_PLAIN;
    at_once_param.p_aes_key   = (uint32_t *)p_key;
    at_once_param.key_length  = D_AES_KEY_SIZE_IN_BYTES_128BIT;
    at_once_param.p_aes_iv    = (uint32_t *)CONST_ZERO;
    at_once_param.p_aes_src   = (uint32_t*)p_input;
    at_once_param.p_aes_dest  = (uint32_t*)p_output;
    at_once_param.last_block_in_bits = (uint16_t)NUM0;
    at_once_param.async              = D_AES_TRANS_SYNC;

    err = D_AES_ExecutionAtOnce(&at_once_param);
    err = mh_KEY_ConvertErr(err);

    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0044]:[ICUMFW_UD_MH_KeyMNG_0044]
* Function name: mh_KEY_UpShe_KeyCombination
* Description  : Confirm that key combination is valid in SHE standard
* Arguments    : p_key_id
*              : p_auth_id
*              : extend_key
*              : p_const
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_MH_KEY_INVALID_KEY_INDEX
***************************************************************************************/
static uint32_t mh_KEY_UpShe_KeyCombination( 
                    uint8_t *p_key_id, uint8_t *p_auth_id, uint8_t extend_key, uint8_t *p_const)
{
    uint32_t err = ER_OK;

    /* make sure that the key ID combination is valid */
    /* (refer to SHE specification table 5) */

    if ((NULL != p_const)
        && (NULL != p_key_id)
        && (NULL != p_auth_id))
    {
        *p_const = (uint8_t)NUM0;
        switch ((uint32_t)*p_key_id)
        {
            case (uint8_t)SHE_MASTER_ECU_KEY:
                if ((uint32_t)SHE_MASTER_ECU_KEY != (uint32_t)*p_auth_id)
                {
                    err = ER_MH_KEY_INVALID_KEY_INDEX;
                }
                else
                {
                    /* Do nothing */
                }
                if ((uint32_t)NB_SHE_EXTEND_ID_0 < (uint32_t)extend_key)
                {
                    /* Incorrect usage of extended */
                    err = ER_PARAM;
                }
                else
                {
                    /* Do nothing */
                }
                break;
            case (uint8_t)SHE_BOOT_MAC_KEY:
            case (uint8_t)SHE_BOOT_MAC:
                if (((uint32_t)SHE_MASTER_ECU_KEY != (uint32_t)*p_auth_id)
                    && ((uint32_t)SHE_BOOT_MAC_KEY != (uint32_t)*p_auth_id))
                {
                    err = ER_MH_KEY_INVALID_KEY_INDEX;
                }
                else
                {
                    /* Do nothing */
                }
                if ((uint32_t)NB_SHE_EXTEND_ID_0 < (uint32_t)extend_key)
                {
                    /* Incorrect usage of extended */
                    err = ER_PARAM;
                }
                else
                {
                    /* Do nothing */
                }
                break;
            case (uint8_t)SHE_KEY_1:
            case (uint8_t)SHE_KEY_2:
            case (uint8_t)SHE_KEY_3:
            case (uint8_t)SHE_KEY_4:
            case (uint8_t)SHE_KEY_5:
            case (uint8_t)SHE_KEY_6:
            case (uint8_t)SHE_KEY_7:
            case (uint8_t)SHE_KEY_8:
            case (uint8_t)SHE_KEY_9:
            case (uint8_t)SHE_KEY_10:
                if (((uint32_t)SHE_MASTER_ECU_KEY != (uint32_t)*p_auth_id) 
                    && ((uint32_t)*p_auth_id != (uint32_t)*p_key_id))
                {
                    err = ER_MH_KEY_INVALID_KEY_INDEX;
                }
                else if ((uint32_t)NB_SHE_EXTEND_IDS_MAX < (uint32_t)extend_key)
                {
                    /* Incorrect usage of extended */
                    err = ER_PARAM;
                }
                else
                {
                    /* update the index */
                    if ((uint32_t)NB_SHE_EXTEND_ID_1 == (uint32_t)extend_key)
                    {
                        /* using extended key 11 to 20 */
                        *p_const = NB_SHE_EXTEND_ID_1;
                        *p_key_id = (uint8_t)((uint32_t)*p_key_id + (uint32_t)(SHE_KEY_11 - SHE_KEY_1));
                    }
                    else if ((uint32_t)NB_SHE_EXTEND_ID_2 == (uint32_t)extend_key)
                    {
                        /* using extended key 21 to 30 */
                        *p_const = NB_SHE_EXTEND_ID_2;
                        *p_key_id = (uint8_t)((uint32_t)*p_key_id + (uint32_t)(SHE_KEY_21 - SHE_KEY_1));
                    }
                    else if ((uint32_t)NB_SHE_EXTEND_ID_3 == (uint32_t)extend_key)
                    {
                        /* using extended key 31 to 40 */
                        *p_const = NB_SHE_EXTEND_ID_3;
                        *p_key_id = (uint8_t)((uint32_t)*p_key_id + (uint32_t)(SHE_KEY_31 - SHE_KEY_1));
                    }
                    else if ((uint32_t)NB_SHE_EXTEND_ID_4 == (uint32_t)extend_key)
                    {
                        /* using extended key 41 to 50 */
                        *p_const = NB_SHE_EXTEND_ID_4;
                        *p_key_id = (uint8_t)((uint32_t)*p_key_id + (uint32_t)(SHE_KEY_41 - SHE_KEY_1));
                    }
                    else if ((uint32_t)NB_SHE_EXTEND_ID_5 == (uint32_t)extend_key)
                    {
                        /* using extended key 51 to 60 */
                        *p_const = NB_SHE_EXTEND_ID_5;
                        *p_key_id = (uint8_t)((uint32_t)*p_key_id + (uint32_t)(SHE_KEY_51 - SHE_KEY_1));
                    }
                    else if ((uint32_t)NB_SHE_EXTEND_ID_6 == (uint32_t)extend_key)
                    {
                        /* using extended key 61 to 70 */
                        *p_const = NB_SHE_EXTEND_ID_6;
                        *p_key_id = (uint8_t)((uint32_t)*p_key_id + (uint32_t)(SHE_KEY_61 - SHE_KEY_1));
                    }
                    else if ((uint32_t)NB_SHE_EXTEND_ID_7 == (uint32_t)extend_key)
                    {
                        /* using extended key 71 to 80 */
                        *p_const = NB_SHE_EXTEND_ID_7;
                        *p_key_id = (uint8_t)((uint32_t)*p_key_id + (uint32_t)(SHE_KEY_71 - SHE_KEY_1));
                    }
                    else if ((uint32_t)NB_SHE_EXTEND_ID_8 == (uint32_t)extend_key)
                    {
                        /* using extended key 81 to 90 */
                        *p_const = NB_SHE_EXTEND_ID_8;
                        *p_key_id = (uint8_t)((uint32_t)*p_key_id + (uint32_t)(SHE_KEY_81 - SHE_KEY_1));
                    }
                    else
                    {
                        /* Do nothing (key 0 to 11) */
                    }

                    /* update the index */
                    if ((uint32_t)NB_SHE_EXTEND_ID_0 != (uint32_t)extend_key)
                    {
                        if ((uint32_t)SHE_MASTER_ECU_KEY != (uint32_t)*p_auth_id)
                        {
                            *p_auth_id = *p_key_id;
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
                break;
            case (uint8_t)SHE_RAM_KEY:
                if (((uint32_t)SHE_SECRET_KEY != (uint32_t)*p_auth_id)
                    && (! (((uint32_t)SHE_KEY_1 <= (uint32_t)*p_auth_id) 
                    && ((uint32_t)SHE_KEY_10 >= (uint32_t)*p_auth_id)) ))
                {
                    err = ER_MH_KEY_INVALID_KEY_INDEX;
                }
                else
                {
                    /* Do nothing */
                }
                if ((uint32_t)NB_SHE_EXTEND_ID_0 < (uint32_t)extend_key)
                {
                    /* Incorrect usage of extended */
                    err = ER_PARAM;
                }
                else
                {
                    /* Do nothing */
                }
                break;
            default:
                err = ER_MH_KEY_INVALID_KEY_INDEX;
                break;
        }
    }
    else
    {
        err = ER_PARAM;
    }
    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0045]:[ICUMFW_UD_MH_KeyMNG_0045]
* Function name: mh_KEY_UpShe_GetCurrentKey
* Description  : Get the current value of the key to be updated
*              : Use the values of counter, wild_card, write_protect
* Arguments    : vm_id
*              : key_id
*              : p_current_counter
*              : p_current_flags
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_SLOT_EMPTY
*              : ER_MH_KEY_INVALID_KEY_GROUP
*              : ER_MH_KEY_INVALID_KEY_INDEX
*              : ER_MH_KEY_IN_USE
*              : ER_MH_KEY_NVM_SYS_CRITICAL
*              : ER_MH_KEY_NVM_SYS
*              : ER_MH_KEY_DATA_INVALID
*              : ER_MH_KEY_SYS_ERROR
*              : ER_MH_KEY_NVM_UNAVAILABLE
*              : ER_MH_KEY_NVM_SIZE
*              : ER_MH_KEY_SRC_MEM_ERROR
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
*              : ER_MH_KEY_WRITE_PROTECTED
***************************************************************************************/
static uint32_t mh_KEY_UpShe_GetCurrentKey(icum_job_t *p_job,
                    uint8_t current_key_id, key_counter_t *p_current_counter, aes_flags_t *p_current_flags)
{
    aes_key_t           *p_key = NULL;
    r_key_group_t        key_group;
    r_key_index_t        key_id;
    uint32_t err;
    
    if ((NULL != p_current_counter)
        && (NULL != p_current_flags))
    {
        /* retrieve the corresponding key */
        (void)MH_KEY_ConvertSheKey(current_key_id, &key_group, &key_id);
        switch (key_group)
        {
            case KEY_GRP_AES:
            case KEY_GRP_AES_RAM:
                err = MH_KEY_GetAesKey(p_job, key_group, key_id, &p_key, (uint8_t)false);
                break;
            default:
                err = ER_PARAM;
                break;
        }
        /* check for errors */
        if (ER_OK != err)
        {
            if (ER_MH_KEY_SLOT_EMPTY == err)
            {
                /* When the key is empty, it is regarded as new registration */
                /* Set counter to 0 and use it for judgment of new registration */
                *p_current_counter   = 0u;
                err = ER_OK;
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* key slot not empty */
            *p_current_counter   = p_key->properties.update_counter;
            *p_current_flags     = p_key->properties.flags;
            /* check if the key to update is write protected (no check in case of RAM key) */
            if ((KEY_GRP_AES == key_group) 
                && ((uint32_t)MH_KEY_FLAG_WRITE_PROTECT == (uint32_t)p_current_flags->write_protected))
            {
                err = ER_MH_KEY_WRITE_PROTECTED;
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    else
    {
        err = ER_PARAM;
    }
    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0046]:[ICUMFW_UD_MH_KeyMNG_0046]
* Function name: mh_KEY_UpShe_GetAuthKey
* Description  : Get the key value of the authentication key
* Arguments    : p_job
*              : current_key_id
*              : auth_key_id
*              : p_auth_value
*              : key_size_ext
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_SLOT_EMPTY
*              : ER_MH_KEY_INVALID_KEY_GROUP
*              : ER_MH_KEY_INVALID_KEY_INDEX
*              : ER_MH_KEY_IN_USE
*              : ER_MH_KEY_NVM_SYS_CRITICAL
*              : ER_MH_KEY_NVM_SYS
*              : ER_MH_KEY_DATA_INVALID
*              : ER_MH_KEY_SYS_ERROR
*              : ER_MH_KEY_NVM_UNAVAILABLE
*              : ER_MH_KEY_NVM_SIZE
*              : ER_MH_KEY_SRC_MEM_ERROR
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
*              : ER_ML_ABTR_EMPTY
*              : ER_ML_ABTR_NOT_RELEASED
*              : ER_ML_ABTR_NOT_INITIALIZED
*              : ER_ML_ABTR_INVALID_ID
*              : ER_ML_ABTR_TIMER_SET_FAILURE
***************************************************************************************/
static uint32_t mh_KEY_UpShe_GetAuthKey(icum_job_t *p_job,
                    uint8_t current_key_id, uint8_t auth_key_id, uint32_t *p_auth_value, uint8_t key_size_ext)
{
    aes_key_t           *p_auth_key = NULL;
    r_key_group_t        key_group;
    r_key_index_t        key_id;
    uint32_t             resource_id;
    uint32_t             err;
    
    if (NULL != p_auth_value)
    {
        /* if the authenticating key is the SHE_SECRET_KEY */
        if ((uint32_t)SHE_SECRET_KEY == (uint32_t)auth_key_id)
        {
            /* Acquire ABTR for secret key acquisition */
            err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id );
            if (ER_OK == err)
            {
                /* retrieve the she secret authentication key */
                err = MH_KEY_GetSecretKey(p_job->vm_id, p_auth_value);
                /* check for errors */
                if (ER_OK != err)
                {
                    /* Prioritize Error currently occurring,do not get ML_ABTR_FreeResource error */
                    (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
                }
                else
                {
                    err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
                }
            }
            else
            {
                /* do nothing */
            }
        }
        /* the authenticating key is either an NVM key or a RAM key */
        else
        {
            /* retrieve the corresponding key */
            (void)MH_KEY_ConvertSheKey(auth_key_id, &key_group, &key_id);
            switch (key_group)
            {
                case KEY_GRP_AES:
                case KEY_GRP_AES_RAM:
                    err = MH_KEY_GetAesKey(p_job, key_group, key_id, &p_auth_key, false);
                    break;
                default:
                    /* may need a better error code */
                    err = ER_PARAM;
                    break;
            }
            /* check for errors */
            if (ER_OK == err)
            {
                /* authentication key not empty: use it */
                if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
                {
                    if (((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_128BIT != (uint32_t)p_auth_key->properties.size_in_bytes)
                        && ((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_192BIT != (uint32_t)p_auth_key->properties.size_in_bytes)
                        && ((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_256BIT != (uint32_t)p_auth_key->properties.size_in_bytes))
                    {
                        err = ER_PARAM;
                    }
                    else
                    {
                        L_Toolbox_MemCpy((uint8_t *)p_auth_value, (uint8_t *)p_auth_key->value
                                            , (NUM2 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
                    }
                }
                else
                {
                    if ((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_128BIT != (uint32_t)p_auth_key->properties.size_in_bytes)
                    {
                        err = ER_PARAM;
                    }
                    else
                    {
                        L_Toolbox_MemCpy((uint8_t *)p_auth_value, (uint8_t *)p_auth_key->value
                                            , AES_BLOCK_SIZE_IN_BYTES);
                    }
                }
                
            }
            else
            {
                /* check if the authentication key is empty */
                if (ER_MH_KEY_SLOT_EMPTY == err)
                {
                    /* only OK if key ID and key auth ID are equal */
                    if ((uint32_t)current_key_id == (uint32_t)auth_key_id)
                    {
                        /* authentication key empty: use all 0 */
                        L_Toolbox_MemCpy((uint8_t *)p_auth_value, CONST_ZERO
                                            , (int16_t)(NUM2 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
                        err = ER_OK;
                    }
                    else
                    {
                        /* It ends with an ER_MH_KEY_SLOT_EMPTY, because the key IDs are not equal */
                    }
                }
                else
                {
                    /* Key acquisition failed */
                    /* do nothing */
                }
            }
        }
    }
    else
    {
        err = ER_PARAM;
    }
    return( err );
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0047]:[ICUMFW_UD_MH_KeyMNG_0047]
* Function name: mh_KEY_UpShe_CmpM3Data
* Description  : Verification processing using M3 data
* Arguments    : p_M1
*              : p_M2
*              : p_M3
*              : p_temp
*              : p_auth_value
*              : p_const_mac
*              : key_size_ext
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_MH_KEY_UPDATE_ERROR
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
***************************************************************************************/
static uint32_t mh_KEY_UpShe_CmpM3Data( 
                const uint8_t *p_M1, const uint8_t *p_M2, const uint8_t *p_M3, uint8_t *p_temp
                , uint32_t *p_auth_value, const uint8_t *p_const_mac, uint8_t key_size_ext)
{
    uint32_t            calculation_key[AES_BLOCK_SIZE];
    uint32_t            temp_cmac[AES_BLOCK_SIZE];
    uint8_t             cmp_result;
    uint32_t            err;
    /**********************************************************************************
    * Step #5:
    * - calculate K2 = KDF(key auth, CMAC)
    * - calculate CMAC over M1|M2
    * - compare with M3
    **********************************************************************************/

    if (NULL != p_temp)
    {
        /* do a key derivation of the authentication key for the CMAC check (K2) */
        if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
        {
            err = mh_KEY_CalculateMp( p_auth_value, p_const_mac, 3u, calculation_key);
        }
        else
        {
            err = mh_KEY_CalculateMp( p_auth_value, p_const_mac, 2u, calculation_key);
        }
        
        if (ER_OK == err)
        {
            /* perform a CMAC calculation over M1 and M2 */
            L_Toolbox_MemCpy(&p_temp[0], p_M1, AES_BLOCK_SIZE_IN_BYTES);
            if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
            {
                L_Toolbox_MemCpy(&p_temp[AES_BLOCK_SIZE_IN_BYTES], p_M2
                                    , (int16_t)(NUM3 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
                err = mh_KEY_CalculateCmac( calculation_key, (uint32_t*)p_temp, NUM4, temp_cmac);
            }
            else
            {
                L_Toolbox_MemCpy(&p_temp[AES_BLOCK_SIZE_IN_BYTES], p_M2
                                    , (int16_t)(NUM2 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
                err = mh_KEY_CalculateCmac( calculation_key, (uint32_t*)p_temp, NUM3, temp_cmac);
            }
            if (ER_OK == err)
            {
                /* compare the calculated CMAC with the CMAC provided in input */
                cmp_result = L_Toolbox_MemCmp(p_M3, (uint8_t*)temp_cmac, AES_BLOCK_SIZE_IN_BYTES);
                if (NUM0 != (uint32_t)cmp_result)
                {
                    /* invalid CMAC: abort the key update */
                    err = ER_MH_KEY_UPDATE_ERROR;
                }
                else
                {
                    err = ER_OK;
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
        err = ER_PARAM;
    }
    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0048]:[ICUMFW_UD_MH_KeyMNG_0048]
* Function name: mh_KEY_UpShe_CheckUid
* Description  : Check wild card and UID
* Arguments    : p_M1
*              : current_counter
*              : current_flags
*              : p_chip_id
* Return Value : ER_OK
*              : ER_MH_KEY_UPDATE_ERROR
***************************************************************************************/
static uint32_t mh_KEY_UpShe_CheckUid(
                    const uint8_t *p_M1, key_counter_t current_counter, aes_flags_t current_flags, uint8_t *p_chip_id)
{
    uint8_t  cmp_result;
    uint32_t err = ER_OK;
    /**********************************************************************************
    * Step #6: check UID'
    **********************************************************************************/

    /* check UID' (in M1) */
    cmp_result = L_Toolbox_MemCmp(p_M1, CONST_ZERO, (int16_t)CHIPID_SIZE);
    if (NUM0 == (uint32_t)cmp_result)
    {
        /* UID' == 0 -> confirm the wildcard flag is set for the key to update */
        if ((NUM0 != (uint32_t)current_counter)
            && ((uint32_t)MH_KEY_FLAG_WILD_CARD_NOT_USABLE == (uint32_t)current_flags.no_wild_card))
        {
            /* cannot have a UID' set to 0 (no wild card): abort */
            err = ER_MH_KEY_UPDATE_ERROR;
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* UID' is not 0 -> compare with internal value */
        cmp_result = L_Toolbox_MemCmp((const uint8_t *)p_M1, (const uint8_t *)p_chip_id, (int16_t)CHIPID_SIZE);
        if (NUM0 != (uint32_t)cmp_result)
        {
            /* wrong UID provided: abort */
            err = ER_MH_KEY_UPDATE_ERROR;
        }
        else
        {
            /* do nothing */
        }
    }
    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0049]:[ICUMFW_UD_MH_KeyMNG_0049]
* Function name: mh_KEY_UpShe_DecM2
* Description  : Decrypt M2 data
* Arguments    : p_M2
*              : p_auth_value
*              : p_temp
*              : p_const_enc
*              : key_size_ext
*              : p_key_size
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
***************************************************************************************/
static uint32_t mh_KEY_UpShe_DecM2(
                const uint8_t *p_M2, uint32_t *p_auth_value, uint8_t *p_temp
                , const uint8_t *p_const_enc, uint8_t key_size_ext, uint16_t* p_key_size)
{
    uint32_t            calculation_key[AES_BLOCK_SIZE];
    uint32_t            temp_M2[(NUM3 * (uint32_t)AES_BLOCK_SIZE)];
    uint32_t            err;
    uint8_t             size;

    /**********************************************************************************
    * Step #7:
    * - calculate K1 = KDF(key auth, ENC)
    * - decrypt M2 with K1
    **********************************************************************************/

    if (NULL != p_key_size)
    {
        /* do a key derivation of the authentication key for the encryption (K1) */
        if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
        {
            err = mh_KEY_CalculateMp( p_auth_value, p_const_enc, NUM3, calculation_key);
        }
        else
        {
            err = mh_KEY_CalculateMp( p_auth_value, p_const_enc, NUM2, calculation_key);
        }
        if (ER_OK == err)
        {
        
            /* decrypt M2 (256 bits) */
            if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
            {
                L_Toolbox_MemCpy((uint8_t *)temp_M2, p_M2, (int16_t)(NUM3 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES ));
                err = mh_KEY_CalculateCbc(CIPHER_DIR_DECRYPTION,calculation_key,temp_M2,(uint32_t *)p_temp, NUM3);
                /* Key size data setting */
                size = (uint8_t)((p_temp[NUM4] >> NUM4) & NUM3);
                switch ((uint32_t)size)
                {
                    case NUM0:
                        *p_key_size = (uint16_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_128BIT;
                        break;
                    case NUM1:
                        *p_key_size = (uint16_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_192BIT;
                        break;
                    case NUM2:
                        *p_key_size = (uint16_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_256BIT;
                        break;
                    default:
                        err = ER_PARAM;
                        break;
                }
            }
            else
            {
                L_Toolbox_MemCpy((uint8_t *)temp_M2, p_M2, (int16_t)(NUM2 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES ));
                err = mh_KEY_CalculateCbc(CIPHER_DIR_DECRYPTION,calculation_key,temp_M2,(uint32_t *)p_temp, NUM2);
                /* Key size data setting */
                *p_key_size = (uint16_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_128BIT;
            }
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
    return( err );
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0050]:[ICUMFW_UD_MH_KeyMNG_0050]
* Function name: mh_KEY_UpShe_CheckCounterFlags
* Description  : Check that Counter and Flag meet the SHE standard
* Arguments    : current_counter
*              : p_counter
*              : p_flags
*              : p_temp
*              : key_id
* Return Value : ER_OK
*              : ER_MH_KEY_UPDATE_ERROR
***************************************************************************************/
static uint32_t mh_KEY_UpShe_CheckCounterFlags( 
                    key_counter_t current_counter, uint32_t *p_counter, uint8_t *p_flags
                    , uint8_t *p_temp,uint8_t key_id)
{
    uint32_t err = ER_OK;
    /**********************************************************************************
    * Step #8: compare the counter value provided with the actual counter value
    *          of the key to udpate
    **********************************************************************************/

    if ((NULL != p_counter)
        && (NULL != p_flags))
    {
        /* M2 = CID (28 bits) | FID (5 bits in SHE, 6 bits in SHE+) | 0-0 */
        /* retrieve the counter value from M2 */
        *p_counter  = ((uint32_t)p_temp[NUM0] << NUM20);
        *p_counter |= ((uint32_t)p_temp[NUM1] << NUM12);
        *p_counter |= ((uint32_t)p_temp[NUM2] << NUM4);
        *p_counter |= ((uint32_t)p_temp[NUM3] >> NUM4);

        /* retrieve the flags from M2 (for later usage) */
        /* flags = WRITE_PROTECTION | BOOT_PROTECTION | DEBUGGER_PROTECTION | KEY_USAGE | WILDCARD | CMAC USAGE */
        *p_flags = (uint8_t)(((uint32_t)p_temp[NUM3] & 0x0FUL) << NUM2) | (((uint32_t)p_temp[NUM4] & 0xC0UL) >> NUM6);

        if ((uint32_t)SHE_RAM_KEY == (uint32_t)key_id)
        {
            /* if loading a RAM key, discard the counter and flags */
            if ((NUM0 != *p_counter)
                || (NUM0 != (uint32_t)*p_flags))
            {
                /* invalid counter value: abort */
                err = ER_MH_KEY_UPDATE_ERROR;
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            /* compare with the counter value in memory */
            if (NUM0 != current_counter)
            {
                /* If it is not newly registered, the "current counter < new counter" must be used */
                if (*p_counter <= current_counter)
                {
                    /* invalid counter value: abort */
                    err = ER_MH_KEY_UPDATE_ERROR;
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* If it is newly registered, the "new counter != 0" must be used */
                if (NUM0 == *p_counter)
                {
                    /* invalid counter value: abort */
                    err = ER_MH_KEY_UPDATE_ERROR;
                }
                else
                {
                    /* do nothing */
                }
            }
        }
    }
    else
    {
        err = ER_PARAM;
    }
    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0051]:[ICUMFW_UD_MH_KeyMNG_0051]
* Function name: mh_KEY_UpShe_FormKeyData
* Description  : Formation of key data
* Arguments    : key_id
*              : p_aes_key
*              : p_temp
*              : new_counter
*              : new_flags
*              : key_size
* Return Value : ER_OK
*              : ER_MH_KEY_INVALID_KEY_INDEX
***************************************************************************************/
static uint32_t mh_KEY_UpShe_FormKeyData(
                    uint8_t key_id, aes_key_t *p_aes_key, uint8_t *p_temp, uint32_t new_counter
                    , uint8_t new_flags, uint16_t key_size)
{
    uint32_t err = ER_OK;
    /**********************************************************************************
    * Step #9: prepare the key values and properties for the update
    **********************************************************************************/

    if ((NULL != p_aes_key)
        && (NULL != p_temp))
       {
        (void)MH_KEY_ConvertSheKey(key_id, &p_aes_key->key_group, &p_aes_key->key_id);
        L_Toolbox_MemCpy((uint8_t *)p_aes_key->value, &p_temp[NUM16]
                        , (int16_t)(NUM2 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES ));

        p_aes_key->properties.update_counter    = new_counter;
        p_aes_key->properties.size_in_bytes     = key_size;

        if ((uint32_t)SHE_MASTER_ECU_KEY == (uint32_t)key_id)
        {
            p_aes_key->properties.flags.write_protected   =
                ((uint32_t)((uint32_t)new_flags & (uint32_t)SHE_FLAG_WRITE_PROTECTION)
                != (uint32_t)MH_KEY_FLAG_NOT_WRITE_PROTECT) ?
                    (MH_KEY_FLAG_WRITE_PROTECT) : (MH_KEY_FLAG_NOT_WRITE_PROTECT);
            p_aes_key->properties.flags.disable_on_debug  =
                ((uint32_t)((uint32_t)new_flags & (uint32_t)SHE_FLAG_DEBUGGER_PROTECTION)
                != (uint32_t)MH_KEY_FLAG_KEY_USE_POSSIBLE) ?
                (MH_KEY_FLAG_KEY_USE_PROHIBIT) : (MH_KEY_FLAG_KEY_USE_POSSIBLE);
            p_aes_key->properties.flags.no_wild_card      =
                ((uint32_t)((uint32_t)new_flags & (uint32_t)SHE_FLAG_NO_WILD_CARD)
                != (uint32_t)MH_KEY_FLAG_WILD_CARD_USABLE) ?
                (MH_KEY_FLAG_WILD_CARD_NOT_USABLE) : (MH_KEY_FLAG_WILD_CARD_USABLE);
            p_aes_key->properties.flags.encryption        = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            p_aes_key->properties.flags.decryption        = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            p_aes_key->properties.flags.cmac_generation   = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            p_aes_key->properties.flags.cmac_verification = MH_KEY_FLAG_KEY_NOT_UDSABLE;
        }
        else if (((uint32_t)SHE_BOOT_MAC_KEY == (uint32_t)key_id) 
                || ((uint32_t)SHE_BOOT_MAC == (uint32_t)key_id))
        {
            p_aes_key->properties.flags.write_protected   =
                ((uint32_t)((uint32_t)new_flags & (uint32_t)SHE_FLAG_WRITE_PROTECTION)
                != (uint32_t)MH_KEY_FLAG_NOT_WRITE_PROTECT) ?
                (MH_KEY_FLAG_WRITE_PROTECT) : (MH_KEY_FLAG_NOT_WRITE_PROTECT);
            p_aes_key->properties.flags.disable_on_debug  =
                ((uint32_t)((uint32_t)new_flags & (uint32_t)SHE_FLAG_DEBUGGER_PROTECTION)
                != (uint32_t)MH_KEY_FLAG_KEY_USE_POSSIBLE) ?
                (MH_KEY_FLAG_KEY_USE_PROHIBIT) : (MH_KEY_FLAG_KEY_USE_POSSIBLE);
            p_aes_key->properties.flags.no_wild_card      =
                ((uint32_t)((uint32_t)new_flags & (uint32_t)SHE_FLAG_NO_WILD_CARD)
                != (uint32_t)MH_KEY_FLAG_WILD_CARD_USABLE) ?
                (MH_KEY_FLAG_WILD_CARD_NOT_USABLE) : (MH_KEY_FLAG_WILD_CARD_USABLE);
            p_aes_key->properties.flags.encryption        = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            p_aes_key->properties.flags.decryption        = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            p_aes_key->properties.flags.cmac_generation   = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            p_aes_key->properties.flags.cmac_verification = MH_KEY_FLAG_KEY_NOT_UDSABLE;
        }
        else if ((((uint32_t)SHE_KEY_1  <= (uint32_t)key_id) 
                    && ((uint32_t)SHE_KEY_10 >= (uint32_t)key_id)) 
                || (((uint32_t)SHE_KEY_11 <= (uint32_t)key_id) 
                    && ((uint32_t)SHE_KEY_90 >= (uint32_t)key_id)))
        {
            p_aes_key->properties.flags.write_protected   =
                ((uint32_t)((uint32_t)new_flags & (uint32_t)SHE_FLAG_WRITE_PROTECTION)
                != (uint32_t)MH_KEY_FLAG_NOT_WRITE_PROTECT) ?
                (MH_KEY_FLAG_WRITE_PROTECT) : (MH_KEY_FLAG_NOT_WRITE_PROTECT);
            p_aes_key->properties.flags.disable_on_debug  =
                ((uint32_t)((uint32_t)new_flags & (uint32_t)SHE_FLAG_DEBUGGER_PROTECTION)
                != (uint32_t)MH_KEY_FLAG_KEY_USE_POSSIBLE) ?
                (MH_KEY_FLAG_KEY_USE_PROHIBIT) : (MH_KEY_FLAG_KEY_USE_POSSIBLE);
            p_aes_key->properties.flags.no_wild_card      =
                ((uint32_t)((uint32_t)new_flags & (uint32_t)SHE_FLAG_NO_WILD_CARD)
                != (uint32_t)MH_KEY_FLAG_WILD_CARD_USABLE) ?
                (MH_KEY_FLAG_WILD_CARD_NOT_USABLE) : (MH_KEY_FLAG_WILD_CARD_USABLE);
            if ((uint32_t)((uint32_t)new_flags & (uint32_t)SHE_FLAG_KEY_USAGE) != NUM0)
            {
                /* KEY_n used for CMAC generation and/or verification only */
                p_aes_key->properties.flags.encryption        = MH_KEY_FLAG_KEY_NOT_UDSABLE;
                p_aes_key->properties.flags.decryption        = MH_KEY_FLAG_KEY_NOT_UDSABLE;
                p_aes_key->properties.flags.cmac_generation   =
                    (((uint32_t)new_flags & (uint32_t)SHE_FLAG_CMAC_USAGE) != NUM0) ?
                    (MH_KEY_FLAG_KEY_NOT_UDSABLE) : (MH_KEY_FLAG_KEY_USABLE);
                p_aes_key->properties.flags.cmac_verification = MH_KEY_FLAG_KEY_USABLE;
            }
            else
            {
                /* KEY_n used for encryption and decryption only */
                p_aes_key->properties.flags.encryption        = MH_KEY_FLAG_KEY_USABLE;
                p_aes_key->properties.flags.decryption        = MH_KEY_FLAG_KEY_USABLE;
                p_aes_key->properties.flags.cmac_generation   = MH_KEY_FLAG_KEY_NOT_UDSABLE;
                p_aes_key->properties.flags.cmac_verification = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            }
        }
        else if ((uint32_t)SHE_RAM_KEY == (uint32_t)key_id)
        {
            p_aes_key->properties.flags.write_protected   = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            p_aes_key->properties.flags.disable_on_debug  = MH_KEY_FLAG_KEY_USE_POSSIBLE;
            p_aes_key->properties.flags.no_wild_card      = MH_KEY_FLAG_WILD_CARD_USABLE;
            p_aes_key->properties.flags.encryption        = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            p_aes_key->properties.flags.decryption        = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            p_aes_key->properties.flags.cmac_generation   = MH_KEY_FLAG_KEY_NOT_UDSABLE;
            p_aes_key->properties.flags.cmac_verification = MH_KEY_FLAG_KEY_NOT_UDSABLE;
        }
        else
        {
            err = ER_MH_KEY_INVALID_KEY_INDEX;
        }
        p_aes_key->properties.flags.invalidate      = MH_KEY_FLAG_NOT_INVALID;
        p_aes_key->properties.flags.plain_key       = MH_KEY_FLAG_UPDATE_SHE;
    }
    else
    {
        err = ER_PARAM;
    }
    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0043]:[ICUMFW_UD_MH_KeyMNG_0043]
* Function name: MH_KEY_DecryptSheKey
* Description  : SHE KEY UPDATE processing
*              : Key data is extracted from M1, M2, M3 data
* Arguments    : p_job
*              : she_data
*              : p_aes_key
*              : p_chip_id
*              : key_size_ext
*              : check_key_flag
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_SLOT_EMPTY
*              : ER_MH_KEY_INVALID_KEY_GROUP
*              : ER_MH_KEY_INVALID_KEY_INDEX
*              : ER_MH_KEY_IN_USE
*              : ER_MH_KEY_WRITE_PROTECTED
*              : ER_MH_KEY_NOT_USABLE
*              : ER_MH_KEY_NVM_SYS_CRITICAL
*              : ER_MH_KEY_NVM_SYS
*              : ER_MH_KEY_DATA_INVALID
*              : ER_MH_KEY_SYS_ERROR
*              : ER_MH_KEY_NVM_UNAVAILABLE
*              : ER_MH_KEY_NVM_SIZE
*              : ER_MH_KEY_SRC_MEM_ERROR
*              : ER_MH_KEY_UPDATE_ERROR
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
***************************************************************************************/
uint32_t MH_KEY_DecryptSheKey(icum_job_t *p_job, ISD_KEY_UPDATE_SHE_t she_data, aes_key_t *p_aes_key,
    uint8_t *p_chip_id, uint16_t check_key_flag, uint8_t key_size_ext)
{
    uint32_t            err;
    uint32_t            auth_key_value[(NUM2 * (uint32_t)AES_BLOCK_SIZE)];
    uint32_t            resource_id;
    uint32_t            counter;
    key_counter_t       id_now_counter;
    aes_flags_t         id_now_flags;
    uint8_t             she_key_id;
    uint8_t             she_auth_key_id;
    uint8_t             flags;
    uint8_t             abtr_resource = 0u;
    uint8_t             judge_const;
    uint16_t            key_size;
    const uint8_t       *p_key_udpate_enc_c;
    const uint8_t       *p_key_udpate_mac_c;
#pragma alignvar(4)
    uint8_t             base_data[(NUM4 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES)];

    id_now_flags.encryption = NUM0;
    id_now_flags.decryption = NUM0;
    id_now_flags.cmac_generation = NUM0;
    id_now_flags.cmac_verification = NUM0;
    id_now_flags.write_protected = NUM0;
    id_now_flags.disable_on_debug = NUM0;
    id_now_flags.invalidate = NUM0;
    id_now_flags.plain_key = NUM0;

    L_Toolbox_MemSet((uint8_t *)auth_key_value, (uint8_t)NUM0, (int16_t)(NUM2 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
    L_Toolbox_MemSet(base_data, (uint8_t)NUM0, (int16_t)(NUM4 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));

    /**********************************************************************************
    * Step #1: confirm the values of key ID and key authentication ID
    **********************************************************************************/
    /* retrieve the ID of the authentication key and of the key to update */
    she_auth_key_id  = she_data.p_M1[15];
    she_key_id       = she_auth_key_id >> 4U;
    she_auth_key_id &= 0x0FU;
    
    if ((uint32_t)NB_SHE_EXTEND_IDS_MAX < (uint32_t)she_data.extended_key_id)
    {
        err = ER_PARAM;
    }
    else
    {
        /* make sure that the key ID combination is valid */
        /* (refer to SHE specification table 5) */
        err = mh_KEY_UpShe_KeyCombination( &she_key_id, &she_auth_key_id, she_data.extended_key_id, &judge_const);
        if (ER_OK == err)
        {
            if ((uint32_t)NB_SHE_EXTEND_ID_0 == (uint32_t)judge_const)
            {
                /* Use regular constants */
                p_key_udpate_enc_c = SHE_KEY_UPDATE_ENC_C;
                p_key_udpate_mac_c = SHE_KEY_UPDATE_MAC_C;
            }
            else if ((uint32_t)NB_SHE_EXTEND_ID_1 == (uint32_t)judge_const)
            {
                /* Use extension constants */
                p_key_udpate_enc_c = SHE_KEY_UPDATE_ENC_C_2;
                p_key_udpate_mac_c = SHE_KEY_UPDATE_MAC_C_2;
            }
            else if ((uint32_t)NB_SHE_EXTEND_ID_2 == (uint32_t)judge_const)
            {
                /* Use extension constants */
                p_key_udpate_enc_c = SHE_KEY_UPDATE_ENC_C_3;
                p_key_udpate_mac_c = SHE_KEY_UPDATE_MAC_C_3;
            }
            else if ((uint32_t)NB_SHE_EXTEND_ID_3 == (uint32_t)judge_const)
            {
                /* Use extension constants */
                p_key_udpate_enc_c = SHE_KEY_UPDATE_ENC_C_4;
                p_key_udpate_mac_c = SHE_KEY_UPDATE_MAC_C_4;
            }
            else if ((uint32_t)NB_SHE_EXTEND_ID_4 == (uint32_t)judge_const)
            {
                /* Use extension constants */
                p_key_udpate_enc_c = SHE_KEY_UPDATE_ENC_C_5;
                p_key_udpate_mac_c = SHE_KEY_UPDATE_MAC_C_5;
            }
            else if ((uint32_t)NB_SHE_EXTEND_ID_5 == (uint32_t)judge_const)
            {
                /* Use extension constants */
                p_key_udpate_enc_c = SHE_KEY_UPDATE_ENC_C_6;
                p_key_udpate_mac_c = SHE_KEY_UPDATE_MAC_C_6;
            }
            else if ((uint32_t)NB_SHE_EXTEND_ID_6 == (uint32_t)judge_const)
            {
                /* Use extension constants */
                p_key_udpate_enc_c = SHE_KEY_UPDATE_ENC_C_7;
                p_key_udpate_mac_c = SHE_KEY_UPDATE_MAC_C_7;
            }
            else if ((uint32_t)NB_SHE_EXTEND_ID_7 == (uint32_t)judge_const)
            {
                /* Use extension constants */
                p_key_udpate_enc_c = SHE_KEY_UPDATE_ENC_C_8;
                p_key_udpate_mac_c = SHE_KEY_UPDATE_MAC_C_8;
            }
            else if ((uint32_t)NB_SHE_EXTEND_ID_8 == (uint32_t)judge_const)
            {
                /* Use extension constants */
                p_key_udpate_enc_c = SHE_KEY_UPDATE_ENC_C_9;
                p_key_udpate_mac_c = SHE_KEY_UPDATE_MAC_C_9;
            }
            else
            {
                err = ER_PARAM;
            }
        }
        else
        {
            /* Do nothing */
        }

        /**********************************************************************************
        * Step #2: Get the current value of the key to be updated
        * Use the values of counter, wild_card, write_protect
        **********************************************************************************/
        /* BOOT_MAC receives data from the service layer with arguments */
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else if ((uint32_t)SHE_BOOT_MAC == (uint32_t)she_key_id)
        {
            if (NULL == p_aes_key)
            {
                err = ER_PARAM;
            }
            else
            {
                /* key 0 to 11 */
                id_now_counter   = p_aes_key->properties.update_counter;
                id_now_flags     = p_aes_key->properties.flags;
                if ((uint32_t)MH_KEY_FLAG_WRITE_PROTECT == (uint32_t)id_now_flags.write_protected)
                {
                    err = ER_MH_KEY_WRITE_PROTECTED;
                }
                else
                {
                    /* Do nothing (key 0 to 11) */
                }
            }
        }
        else
        {
            err = mh_KEY_UpShe_GetCurrentKey(p_job, she_key_id, &id_now_counter, &id_now_flags);
        }

        /**********************************************************************************
        * Step #3: retrieve the values of the authentication key
        **********************************************************************************/
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            err = mh_KEY_UpShe_GetAuthKey(p_job, she_key_id, she_auth_key_id, auth_key_value, key_size_ext);
        }

        /**********************************************************************************
        * Step #4:
        * The debugger protection and secure boot protection have to be checked if ID = RAM_KEY
        * The check is done in the upper layer
        **********************************************************************************/
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else if ((uint32_t)MH_KEY_FLAG_CHECK_NG == (uint32_t)check_key_flag)
        {
            err = ER_MH_KEY_NOT_USABLE;
        }
        else
        {
            /* Acquire ABTR for AES operation */
            err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id );
            if (ER_OK == err)
            {
                abtr_resource = 1u;
            }
            else
            {
                /* Do nothing */
            }
        }
        /**********************************************************************************
        * Step #5:
        * - calculate K2 = KDF(key auth, CMAC)
        * - calculate CMAC over M1|M2
        * - compare with M3
        **********************************************************************************/
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            err = mh_KEY_UpShe_CmpM3Data(
                    she_data.p_M1,she_data.p_M2,she_data.p_M3, base_data
                    , auth_key_value, p_key_udpate_mac_c, key_size_ext);
        }

        /**********************************************************************************
        * Step #6: check UID'
        **********************************************************************************/
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            err = mh_KEY_UpShe_CheckUid(she_data.p_M1, id_now_counter, id_now_flags, p_chip_id);
        }

        /**********************************************************************************
        * Step #7:
        * - calculate K1 = KDF(key auth, ENC)
        * - decrypt M2 with K1
        **********************************************************************************/
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            L_Toolbox_MemSet(base_data, (uint8_t)NUM0, (int16_t)(NUM4 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
            err = mh_KEY_UpShe_DecM2(she_data.p_M2, auth_key_value, base_data
                                    , p_key_udpate_enc_c, key_size_ext, &key_size);
        }

        /**********************************************************************************
        * Additional steps associated with key length expansion:
        * - Key size must be 128bit to update BOOT_MAC.
        * - If it is not an extension, 128bit is always set.
        **********************************************************************************/
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            if (((uint32_t)SHE_BOOT_MAC == (uint32_t)she_key_id)
                && (MH_KEY_AES_KEY_SIZE_IN_BYTES_128BIT != key_size))
            {
                err = ER_PARAM;
            }
            else
            {
                /* Do nothing */
            }
        }

        /**********************************************************************************
        * Step #8: compare the counter value provided with the actual counter value
        *          of the key to udpate
        **********************************************************************************/
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            err = mh_KEY_UpShe_CheckCounterFlags(id_now_counter, &counter, &flags, base_data, she_key_id);
        }

        /**********************************************************************************
        * Step #9: prepare the key values and properties for the update
        **********************************************************************************/
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            err = mh_KEY_UpShe_FormKeyData( she_key_id, p_aes_key, base_data, counter, flags, key_size);
        }
    }

    if (NUM1 == (uint32_t)abtr_resource)
    {
        if (ER_OK == err)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        }
        else
        {
            /* Prioritize Error currently occurring,do not get ML_ABTR_FreeResource error */
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        }
    }
    else
    {
        /* Do nothing */
    }
    err = mh_KEY_ConvertErr( err );
    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0085]:[ICUMFW_UD_MH_KeyMNG_0085]
* Function name: mh_KEY_PrepareGenExportKey
* Description  : SHE KEY Export processing prepare
*              : Encrypts the RAM_KEY data, and generates M1 to M5 data
* Arguments    : p_job
*              : p_secret_key_value
*              : p_she_auth_key_id
*              : p_she_key_id
*              : p_resource_id
*              : p_abtr_resource
*              : p_ramkey_value
*              : check_key_flag
*              : key_size_ext
*              : p_ramkey_size
* Return Value : ER_OK
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
*              : SERV_PENDING
***************************************************************************************/
static uint32_t mh_KEY_PrepareGenExportKey(icum_job_t *p_job,
    uint32_t *p_secret_key_value, uint8_t *p_she_auth_key_id,
    uint8_t *p_she_key_id, uint32_t *p_resource_id, uint8_t *p_abtr_resource, uint32_t *p_ramkey_value,
    uint16_t check_key_flag, uint8_t key_size_ext, uint32_t *p_ramkey_size)
{
    aes_key_t            *p_key;
    aes_key_prop_t       ramkey_properties;
    uint32_t             err;
    
    p_key = NULL;
    ramkey_properties.flags.plain_key = NUM0;
    ramkey_properties.size_in_bytes = (uint16_t)NUM0;
    /**********************************************************************************
    * Step #0:
    * - load AES ROM key and AES RAM key
    * perform prechecks to validate the user input
    * - if keys conditions are not correct, return error and terminate service
    **********************************************************************************/
    err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, p_resource_id );
    if (ER_OK == err)
    {
        *p_abtr_resource = NUM1;
        /* retrieve the she secret authentication key */
        err = MH_KEY_GetSecretKey(p_job->vm_id, p_secret_key_value);
        if (ER_OK != err)
        {
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, *p_resource_id);
        }
        else
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, *p_resource_id);
        }
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        err = MH_KEY_GetAesKey(p_job, KEY_GRP_AES_RAM, 0u, &p_key, false);
        if (ER_OK != err)
        {
            /* SHE SECRET key is not readable or has not been initialized!! */
        }
        else
        {
            L_Toolbox_MemCpy((uint8_t *)&ramkey_properties, (uint8_t *)&p_key->properties,
                                (int16_t)sizeof( aes_key_prop_t ));
            L_Toolbox_MemCpy((uint8_t *)p_ramkey_value, (uint8_t *)p_key->value,
                                (int16_t)(NUM2 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
        }
    }

    if (ER_OK != err)
    {
        /* Do nothing */
    }
    /*only if the key is in plain text can it be exported out */
    else if ((uint32_t)MH_KEY_FLAG_UPDATE_SHE == (uint32_t)ramkey_properties.flags.plain_key)
    {
        err = ER_MH_KEY_NOT_USABLE;
    }
    /* check the RAM key size */
    else if (((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
        && (((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_128BIT != (uint32_t)ramkey_properties.size_in_bytes)
            &&((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_192BIT != (uint32_t)ramkey_properties.size_in_bytes)
            &&((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_256BIT != (uint32_t)ramkey_properties.size_in_bytes)))
    {
        /* error in key size */
        err = ER_PARAM;
    }
    else if (((uint32_t)MH_KEY_SIZE_NOT_EXP == (uint32_t)key_size_ext)
        && ((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_128BIT != (uint32_t)ramkey_properties.size_in_bytes))
    {
        /* error in key size */
        err = ER_PARAM;
    }
    /* SECRET_KEY inherits protection flags from MASTER_ECU_KEY */
    /* The check is done in the upper layer */
    else if ((uint32_t)MH_KEY_FLAG_CHECK_NG == (uint32_t)check_key_flag)
    {
        err = ER_MH_KEY_NOT_USABLE;
    }
    else
    {
        /*the Device Secret Key is the authenticating Key */
        *p_she_auth_key_id = (uint8_t)SHE_SECRET_KEY;
        *p_she_key_id = (uint8_t)SHE_RAM_KEY;
        if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
        {
            if((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_128BIT == (uint32_t)ramkey_properties.size_in_bytes)
            {
                *p_ramkey_size = NUM0;
            }
            else if((uint32_t)MH_KEY_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)ramkey_properties.size_in_bytes)
            {
                *p_ramkey_size = NUM1;
            }
            else
            {
                *p_ramkey_size = NUM2;
            }
        }
        else
        {
            *p_ramkey_size = NUM0;
        }
    }
    
    return (err);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0053]:[ICUMFW_UD_MH_KeyMNG_0053]
* Function name: MH_KEY_GenerateAESExportKey
* Description  : SHE KEY Export processing
*              : Encrypts the RAM_KEY data, and generates M1 to M5 data
* Arguments    : p_job
*              : she_data
*              : p_chip_id
*              : check_key_flag
*              : key_size_ext
* Return Value : ER_OK
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
uint32_t MH_KEY_GenerateAESExportKey(icum_job_t *p_job,
    ISD_KEY_AES_RAM_EXP_t she_data, uint8_t *p_chip_id, uint16_t check_key_flag, uint8_t key_size_ext)
{
    uint8_t              she_key_id;
    uint8_t              she_auth_key_id;
    uint32_t             calculation_key[AES_BLOCK_SIZE];
    uint32_t             base_data[NUM4 * AES_BLOCK_SIZE];
    uint32_t             temp_cmac[AES_BLOCK_SIZE];
    uint32_t             ramkey_value[(NUM2 * (uint32_t)AES_BLOCK_SIZE)];
    uint32_t             resource_id;
    uint32_t             secret_key_value[(NUM2 * (uint32_t)AES_BLOCK_SIZE)];
    uint32_t             ramkey_size;
    uint32_t             err;
    uint8_t             abtr_resource = NUM0;

    L_Toolbox_MemSet((uint8_t *)base_data, (uint8_t)NUM0, (int16_t)(NUM4 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
    L_Toolbox_MemSet((uint8_t *)ramkey_value, (uint8_t)NUM0, (int16_t)(NUM2 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
    L_Toolbox_MemSet((uint8_t *)secret_key_value, (uint8_t)NUM0, (int16_t)(NUM2 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));

    /**********************************************************************************
    * Step #0:
    * - load AES ROM key and AES RAM key
    * perform prechecks to validate the user input
    * - if keys conditions are not correct, return error and terminate service
    **********************************************************************************/
    err = mh_KEY_PrepareGenExportKey(p_job, secret_key_value, &she_auth_key_id, &she_key_id,
        &resource_id, &abtr_resource, ramkey_value, check_key_flag, key_size_ext, &ramkey_size);

    /**********************************************************************************
    * Step #1:
    * - calculate M1 = UID|AuthID|KeyID
    *
    **********************************************************************************/
    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        L_Toolbox_MemCpy(she_data.p_M1, (const uint8_t *)p_chip_id, (int16_t)CHIPID_SIZE);
        /*append the KeyID|AuthID */
        *(she_data.p_M1 + NUM15) = (uint8_t)((((uint32_t)she_key_id << NUM4) & 0xF0uL)
                                                | ((uint32_t)she_auth_key_id & 0x0FuL));
        L_Toolbox_MemCpy((uint8_t*)base_data, she_data.p_M1, AES_BLOCK_SIZE_IN_BYTES);
    }

    /* check all the key properties before proceeding*/
    /**********************************************************************************
    * Step #2:
    * - calculate K1 = KDF(key auth, ENC)
    * - encrypt M2 with K1
    **********************************************************************************/
    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id );
        if ( ER_OK == err )
        {
            abtr_resource = NUM1;
        }
        else
        {
            /* Do nothing */
        }
    }

    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        /* do a key derivation of the authentication key for the encryption (K1) */
        if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
        {
            err = mh_KEY_CalculateMp(secret_key_value, SHE_KEY_UPDATE_ENC_C, NUM3, calculation_key);
        }
        else
        {
            err = mh_KEY_CalculateMp(secret_key_value, SHE_KEY_UPDATE_ENC_C, NUM2, calculation_key);
        }
    }

    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        /*construct M2 */
        /*first 16 bytes are zeroes */
        L_Toolbox_MemCpy((uint8_t*)&base_data[NUM4], CONST_ZERO, AES_BLOCK_SIZE_IN_BYTES);

        if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
        {
            /* Set key size data */
            base_data[NUM5] |= (ramkey_size << NUM4);
            /*next 16 bytes are the RAM key in plaintext */
            L_Toolbox_MemCpy((uint8_t*)&base_data[NUM8], (uint8_t*)ramkey_value
                                , (int16_t)(NUM2 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
        }
        else
        {
            /*next 16 bytes are the RAM key in plaintext */
            L_Toolbox_MemCpy((uint8_t*)&base_data[NUM8], (uint8_t*)ramkey_value, AES_BLOCK_SIZE_IN_BYTES);
        }

        /* encrypt M2 (256 bits) */
        if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
        {
            err = mh_KEY_CalculateCbc(CIPHER_DIR_ENCRYPTION,calculation_key,&base_data[NUM4],&base_data[NUM4], NUM3);
        }
        else
        {
            err = mh_KEY_CalculateCbc(CIPHER_DIR_ENCRYPTION,calculation_key,&base_data[NUM4],&base_data[NUM4], NUM2);
        }
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            /*now temp contains the encrypted M2 so copy it to M2 */
            if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
            {
                L_Toolbox_MemCpy((she_data.p_M2), (uint8_t*)&base_data[NUM4]
                                    , (int16_t)(NUM3 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
            }
            else
            {
                L_Toolbox_MemCpy((she_data.p_M2), (uint8_t*)&base_data[NUM4]
                                    , (int16_t)(NUM2 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
            }
        }
    }

    /**********************************************************************************
    * Step #3:
    * - calculate K2 = KDF(key auth, CMAC)
    * - calculate CMAC over M1|M2
    * -
    **********************************************************************************/
    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        /* do a key derivation of the authentication key for the CMAC check (K2) */
        /* authentication key not empty: use it */
        if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
        {
            err = mh_KEY_CalculateMp(secret_key_value, SHE_KEY_UPDATE_MAC_C, NUM3, calculation_key);
        }
        else
        {
            err = mh_KEY_CalculateMp(secret_key_value, SHE_KEY_UPDATE_MAC_C, NUM2, calculation_key);
        }
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
            {
                err = mh_KEY_CalculateCmac( calculation_key, base_data, NUM4, temp_cmac);
            }
            else
            {
                err = mh_KEY_CalculateCmac( calculation_key, base_data, NUM3, temp_cmac);
            }
            if (ER_OK != err)
            {
                /* Do nothing */
            }
            else
            {
                /* copy result into M3 */
                L_Toolbox_MemCpy(she_data.p_M3,(uint8_t*)temp_cmac, AES_BLOCK_SIZE_IN_BYTES);
            }
        }
    }

    /**********************************************************************************
    * Step #4: generate M4
    **********************************************************************************/
    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        /* do a key derivation of the key for the encryption (K3) */
        if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
        {
            err = mh_KEY_CalculateMp(ramkey_value, SHE_KEY_UPDATE_ENC_C, NUM3, calculation_key);
        }
        else
        {
            err = mh_KEY_CalculateMp(ramkey_value, SHE_KEY_UPDATE_ENC_C, NUM2, calculation_key);
        }
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            /* first part of M4 (UID | KEY_ID | KEY_AUTH_ID) */
            /* second part of M4: encrypted counter value (after padding) */
            err = mh_KEY_CalculateEcb(calculation_key,(const uint32_t*)EXPORT_CID,&base_data[NUM4]);
            if (ER_OK != err)
            {
                /* Do nothing */
            }
            else
            {
                L_Toolbox_MemCpy(she_data.p_M4, (uint8_t*)base_data
                                    , (int16_t)(NUM2 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES));
            }
        }
    }

    /**********************************************************************************
    * Step #5: generate M5
    **********************************************************************************/
    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        /* do a key derivation of the key for the encryption (K4) */
        if ((uint32_t)MH_KEY_SIZE_EXP == (uint32_t)key_size_ext)
        {
            err = mh_KEY_CalculateMp(ramkey_value, SHE_KEY_UPDATE_MAC_C, NUM3, calculation_key);
        }
        else
        {
            err = mh_KEY_CalculateMp(ramkey_value, SHE_KEY_UPDATE_MAC_C, NUM2, calculation_key);
        }
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            /* perform a CMAC calculation over M4 */
            err = mh_KEY_CalculateCmac( calculation_key, base_data, NUM2, temp_cmac);
            if (ER_OK != err)
            {
                /* Do nothing */
            }
            else
            {
                L_Toolbox_MemCpy(she_data.p_M5, (uint8_t*)temp_cmac, AES_BLOCK_SIZE_IN_BYTES);
            }
        }
    }

    if (NUM1 == abtr_resource)
    {
        if (ER_OK == err)
        {
            err = ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        }
        else
        {
            /* Prioritize Error currently occurring,do not get ML_ABTR_FreeResource error */
            (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        }
    }
    else
    {
        /* do nothing */
    }
    err = mh_KEY_ConvertErr( err );
    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0056]:[ICUMFW_UD_MH_KeyMNG_0056]
* Function name: mh_KEY_FreeCacheAes
* Description  : Clear processing of AES key in-use counter
* Arguments    : p_job
*              : key_group
*              : key_id
* Return Value : ER_OK
*              : ER_MH_KEY_DATA_NOT_FOUND
***************************************************************************************/
uint32_t mh_KEY_FreeCacheAes(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id)
{
    int32_t idx;
    uint32_t err;

    /* Initialize local variable */
    idx = 0L;
    err = ER_MH_KEY_DATA_NOT_FOUND;

    /* scan the table aes in use manage table (it may be there!) */
    for (idx = (int32_t)(NB_AES_IN_USE_MNG_TABLE - NUM1); idx >= (int32_t)NUM0; idx--)
    {
        if ((E_AVAILABLE == g_aes_in_use_mng_table[idx].availability) &&
            (key_group == g_aes_in_use_mng_table[idx].key_group) &&
            ((uint32_t)p_job->vm_id == (uint32_t)g_aes_in_use_mng_table[idx].vm_id) &&
            ((uint32_t)key_id == (uint32_t)g_aes_in_use_mng_table[idx].key_id))
        {
            g_aes_in_use_mng_table[idx].availability = E_NOT_AVAILABLE;
            err = ER_OK;
            break;
        }
        else
        {
            /* do nothing */
        }
    } /* end of for (idx = (int32_t)(NB_AES_IN_USE_MNG_TABLE - NUM1); idx >= (int32_t)NUM0; idx--) */

    return err;
}

/* Store in RAM statement */
CHANGE_SEC(text, ".icum_middle_ram")
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0081]:[ICUMFW_UD_MH_KeyMNG_0081]
* Function name: MH_KEY_ConvertSheKey
* Description  : Convert SHE format key value
* Arguments    : input_key_id
*              : p_key_group
*              : p_key_id
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_KEY_ConvertSheKey( r_key_index_t input_key_id, r_key_group_t* p_key_group, r_key_index_t* p_key_id)
{
    if ((NULL != p_key_group)
        && (NULL != p_key_id))
    {
        if ((NUM0 < (uint32_t)input_key_id)
            && (NUM3 > (uint32_t)input_key_id))
        {
            *p_key_group = KEY_GRP_AES;
            *p_key_id = (r_key_index_t)((uint32_t)input_key_id - NUM1);
        }
        else if ((NUM3 < (uint32_t)input_key_id)
                && (NUM14 > (uint32_t)input_key_id))
        {
            *p_key_group = KEY_GRP_AES;
            *p_key_id = (r_key_index_t)((uint32_t)input_key_id - NUM2);
        }
        else if (NUM14 == (uint32_t)input_key_id)
        {
            *p_key_group = KEY_GRP_AES_RAM;
            *p_key_id = (r_key_index_t)NUM0;
        }
        else if ((NUM14 < (uint32_t)input_key_id)
                && (NUM95 > (uint32_t)input_key_id))
        {
            *p_key_group = KEY_GRP_AES;
            *p_key_id = (r_key_index_t)((uint32_t)input_key_id - NUM3);
        }
        else /* in_key_id == 0 or 3 or over95*/
        {
            *p_key_group = (r_key_group_t)CONVERT_ERR;
            *p_key_id = (r_key_index_t)CONVERT_ERR;
        }
    }
    else
    {
        /* do nothing */
    }
    return ER_OK;
}
/* Store in RAM statement ends*/
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

