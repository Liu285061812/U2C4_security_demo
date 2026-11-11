/******************************************************************************/
/* Component Name  Key Manager (ECC)                                          */
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
#include "icum_common_pub.h"
#include "icum_mh_key_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_l_toolbox_pub.h"
#include "icum_mh_sysinfo_pub.h"
#include "icum_ml_dvc_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include "icum_ml_cfg_pub.h"

#include "icum_mh_key.h"
/**********************************************************************************
 * Local defines
 **********************************************************************************/
#define SCALAR_SIZE_IN_BYTES    ((R_CRYPT_SMALLBIGNUM_BITSIZE + 7u) / 8u)
#define DATA_ZERO               ((uint8_t)0u)

/* unsigned data size */
#define NUM7                         (7u)
#define NUM192                       (192u)
#define NUM224                       (224u)
#define NUM256                       (256u)
#define NUM320                       (320u)
#define NUM384                       (384u)
#define NUM448                       (448u)
#define NUM480                       (480u)
#define NUM512                       (512u)
#define NUM544                       (544u)

/* base addr ECC ram keys of each vm */
#define ECC_RAM_KEYS_BASE(vm_id, key_id)   (ML_CFG_ECC_RAM_START_ADDR \
                                               + (MH_KEY_ECC_RAM_DATA_SIZE \
                                                   * ((uint32_t)ML_CFG_VM_ECC_RAM_IDX_OFFSET(vm_id) \
                                                       + (uint32_t)(key_id))))

/* start addr of ECC ram keys of each vm */
#define ECC_RAM_KEYS(vm_id, key_id)              ((ecc_key_t *)(ECC_RAM_KEYS_BASE(vm_id, key_id)))

/* the property start addr of ECC ram keys for each vm */
#define ECC_RAM_KEYS_PROPERTY(vm_id, key_id)     ((ecc_key_prop_t *)(ECC_RAM_KEYS_BASE(vm_id, key_id) \
                                                     + sizeof(ecc_key_t)))
/* the private scalar start addr of ECC ram keys for each vm */
#define ECC_RAM_KEYS_PRIV_SCALAR(vm_id, key_id)  ((pkc_small_data_t *)(ECC_RAM_KEYS_BASE(vm_id, key_id) \
                                                     + sizeof(ecc_key_t) \
                                                     + sizeof(ecc_key_prop_t)))
/* the public points start addr of ECC ram key for each vm */
#define ECC_RAM_KEYS_PUB_POINT(vm_id, key_id)    ((ecc_point_affine_t *)(ECC_RAM_KEYS_BASE(vm_id, key_id) \
                                                     + sizeof(ecc_key_t) \
                                                     + sizeof(ecc_key_prop_t) \
                                                     + sizeof(pkc_small_data_t)))
/* the in-use counter start addr of ECC ram key for each vm */
#define ECC_RAM_KEYS_INUSE_COUNT(vm_id, key_id)  ((uint8_t *)(ECC_RAM_KEYS_BASE(vm_id, key_id) \
                                                     + sizeof(ecc_key_t) \
                                                     + sizeof(ecc_key_prop_t) \
                                                     + sizeof(pkc_small_data_t) \
                                                     + sizeof(ecc_point_affine_t)))
/**********************************************************************************
 * Local type definition
 **********************************************************************************/
typedef struct ecc_priv_key_slot
{
    uint8_t             dummy[FLS_DATA_SLOT_MGMT_SIZE];
    ecc_key_prop_t      properties;
    uint8_t             pub_point_x[SCALAR_SIZE_IN_BYTES];
    uint8_t             pub_point_y[SCALAR_SIZE_IN_BYTES];
    uint8_t             priv_scalar[SCALAR_SIZE_IN_BYTES];
    uint8_t             reserved[FLS_DF_BLOCK_SIZE - ((FLS_DATA_SLOT_MGMT_SIZE
                                                     + (uint32_t)sizeof(ecc_key_prop_t)
                                                     + SCALAR_SIZE_IN_BYTES
                                                     + SCALAR_SIZE_IN_BYTES
                                                     + SCALAR_SIZE_IN_BYTES)
                                                     % FLS_DF_BLOCK_SIZE)];
} ecc_priv_key_slot_t;

typedef struct ecc_pub_key_slot
{
    ecc_key_prop_t      properties;
    uint8_t             pub_point_x[SCALAR_SIZE_IN_BYTES];
    uint8_t             pub_point_y[SCALAR_SIZE_IN_BYTES];
} ecc_pub_key_slot_t;

typedef struct ecc_pub_ext_key_slot
{
    uint8_t                 dummy[FLS_DATA_SLOT_MGMT_SIZE];
    key_counter_t           update_counter;
    ecc_flags_t             flags;
    uint32_t                x509_cert_address;
    uint32_t                x509_cert_size_in_bytes;
    uint8_t                 x509_dvc[DVC_T_SIZE];
    uint8_t                 reserved[FLS_DF_BLOCK_SIZE - ((FLS_DATA_SLOT_MGMT_SIZE
                                                    + (uint32_t)sizeof(key_counter_t)
                                                    + (uint32_t)sizeof(ecc_flags_t)
                                                    + (uint32_t)(sizeof(ecc_flags_t) % sizeof(uint32_t))
                                                    + (uint32_t)sizeof(uint32_t)
                                                    + (uint32_t)sizeof(uint32_t)
                                                    + DVC_T_SIZE
                                                    ) % FLS_DF_BLOCK_SIZE)];
} ecc_pub_ext_key_slot_t;

/**********************************************************************************
 * Local constants
 **********************************************************************************/
/* RAM copy of some of some of the ECC keys stored in Flash (Key Cache) */
#pragma alignvar(4)
static ecc_key_t    g_copy_ecc_keys[NB_RAM_COPIES_ECC_KEYS];

/* Buffer for key values and properties */
#pragma alignvar(4)
static ecc_key_prop_t       g_buffer_key_properties_ecc[NB_RAM_COPIES_ECC_KEYS];

static pkc_small_data_t      g_buffer_priv_scalar[NB_RAM_COPIES_ECC_KEYS];
static ecc_point_affine_t   g_buffer_pub_point[NB_RAM_COPIES_ECC_KEYS];

/* Write ECC key data storage area */
#pragma alignvar(4)
static ecc_priv_key_slot_t      g_storage_keys_ecc;

#pragma alignvar(4)
static ecc_pub_ext_key_slot_t   g_storage_keys_ecc_ext;

/* Index in table copy_ecc_keys[] */
static uint8_t g_current_idx_ecc;

/* Used state management counter of the ecc key in the cache */
static uint8_t g_in_use_counter_ecc[NB_RAM_COPIES_ECC_KEYS];

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t mh_KEY_GetEcc(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id, ecc_key_t **p_key);
static uint32_t mh_KEY_GetEccRam(icum_job_t *p_job, r_key_index_t key_id, ecc_key_t **p_key);
static uint32_t mh_KEY_GetEccPubExt(icum_job_t *p_job, r_key_group_t key_group,
    r_key_index_t key_id, ecc_key_t **p_key);
static uint32_t mh_KEY_UpdateEcc(icum_job_t *p_job, set_operation_t operation, ecc_key_t *p_new_key);
static uint32_t mh_KEY_UpdateEccRam(icum_job_t *p_job, ecc_key_t *p_new_key);
static uint32_t mh_KEY_CheckUpdateEccParam(ecc_key_t *p_new_key, uint32_t *p_key_size);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0004]:[ICUMFW_UD_MH_KeyMNG_0004]
* Function name: mh_KEY_InitEcc
* Description  : Initialization of the ECC key slots in the ICU-M
* Arguments    : void
* Return Value : ER_OK
***************************************************************************************/
uint32_t mh_KEY_InitEcc(void)
{
    uint32_t i;
    uint8_t key_number;

    /* Initialize local variable */
    i = NUM0;
    key_number = 0u;

    /* init ECC Config RAM item */
    (void)mh_KEY_InitEccRamCfg();

    /* init copy of ECC keys */
    for (i = NUM0; i < (uint32_t)NB_RAM_COPIES_ECC_KEYS; i++)
    {
        /* init pointers to buffers */
        g_copy_ecc_keys[i].p_properties     = &g_buffer_key_properties_ecc[i];
        g_copy_ecc_keys[i].p_priv_scalar    = &g_buffer_priv_scalar[i];
        g_copy_ecc_keys[i].p_pub_point      = &g_buffer_pub_point[i];
        /* init availability */
        g_copy_ecc_keys[i].availability     = E_NOT_AVAILABLE;
        /* init the use counter */
        g_in_use_counter_ecc[i]              = 0u;
    }

    /* index on tables g_copy_ecc_keys[] */
    g_current_idx_ecc = NB_RAM_COPIES_ECC_KEYS - 1u;

    /* declare the data set (Flash mapping) */
    (void)mh_KEY_MapSize(KEY_MAPPING_ECC_PRIV, &key_number);
    (void)MM_FLS_DeclareDataSet(FLS_DS_ECC_PRIV_KEY, key_number,
                                MH_KEY_ECC_PRIV_DS_NB_BLOCKS, FLS_DS_NB_COPIES);

    (void)mh_KEY_MapSize(KEY_MAPPING_ECC_PUB, &key_number);
    (void)MM_FLS_DeclareDataSet(FLS_DS_ECC_PUB_KEY, key_number,
                                MH_KEY_ECC_PUB_DS_NB_BLOCKS, FLS_DS_NB_COPIES);

    (void)mh_KEY_MapSize(KEY_MAPPING_ECC_PUB_EXT, &key_number);
    (void)MM_FLS_DeclareDataSet(FLS_DS_ECC_PUB_EXT_KEY, key_number,
                                MH_KEY_ECC_PUB_EXT_NB_BLOCKS, FLS_DS_NB_COPIES);

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0089]:[ICUMFW_UD_MH_KeyMNG_0089]
* Function name: mh_KEY_InitEccRamCfg
* Description  : Initialization of the ECC key (config RAM) in the ICU-M
* Arguments    : void
* Return Value : ER_OK
***************************************************************************************/
uint32_t mh_KEY_InitEccRamCfg(void)
{
    uint32_t i;
    uint32_t vm_id;
    uint8_t key_number;
    uint8_t *p_ecc_ram_use_cnt;
    ecc_key_t *p_ecc_ram_keys;

    /* Initialize local variable */
    i = NUM0;
    vm_id = NUM0;
    key_number = 0u;
    p_ecc_ram_use_cnt = NULL;
    p_ecc_ram_keys = NULL;

    /* init ECC RAM key */
    for (vm_id = NUM0; vm_id < (uint32_t)ML_CFG_TOTAL_NB_VMS_MAX; vm_id++)
    {
        (void)mh_KEY_MapSizeVM((uint8_t)vm_id, KEY_MAPPING_ECC_RAM, &key_number);
        for (i = NUM0; i < (uint32_t)key_number; i++)
        {
            p_ecc_ram_keys = ECC_RAM_KEYS(vm_id, i);
            p_ecc_ram_keys->p_properties = ECC_RAM_KEYS_PROPERTY(vm_id, i);
            p_ecc_ram_keys->p_priv_scalar = ECC_RAM_KEYS_PRIV_SCALAR(vm_id, i);
            p_ecc_ram_keys->p_pub_point = ECC_RAM_KEYS_PUB_POINT(vm_id, i);
            p_ecc_ram_use_cnt = ECC_RAM_KEYS_INUSE_COUNT(vm_id, i);

            /* init the properties, the private scalar, the public points */
            L_Toolbox_MemSet((uint8_t *)p_ecc_ram_keys->p_properties, 0x00u, (int16_t)sizeof(ecc_key_prop_t));
            L_Toolbox_MemSet((uint8_t *)p_ecc_ram_keys->p_priv_scalar, 0x00u, (int16_t)sizeof(pkc_small_data_t));
            L_Toolbox_MemSet((uint8_t *)p_ecc_ram_keys->p_pub_point, 0x00u, (int16_t)sizeof(ecc_point_affine_t));
            /* init availability */
            p_ecc_ram_keys->availability  = E_NOT_AVAILABLE;
            /* init the use counter */
            *p_ecc_ram_use_cnt = 0x00u;
        }
    }

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0025]:[ICUMFW_UD_MH_KeyMNG_0025]
* Function name: mh_KEY_GetEcc
* Description  : Retrieve the pointer to the ECC key value & properties for a given key idx.
*                Returns 0 if the key is not available (i.e. not initialized in Flash) case inside icup
* Arguments    : (in) p_job             : the pointer of job parameter
*                (in) key_group         :      Range: KEY_GRP_ECC
*                                                     KEY_GRP_ECC_PUB
*                (in) key_id            :
*                (out) **p_key          : the pointer to the ECC key value & properties
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_AES_PRM_ERROR
***************************************************************************************/
static uint32_t mh_KEY_GetEcc(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id, ecc_key_t **p_key)
{
#pragma alignvar(4)
    ecc_priv_key_slot_t key_data;
    uint8_t             vm_id;
    uint8_t             slot_num;
    uint8_t            key_number;
    uint8_t             data_set_type;
    uint32_t            err;
    uint32_t            key_size;
    uint16_t            block_size;
    int32_t             i;

    *p_key = NULL;
    err = ER_OK;
    vm_id = p_job->vm_id;
    slot_num = 0u;
    data_set_type = 0u;
    key_size = 0u;
    block_size = 0u;
    L_Toolbox_MemSet((uint8_t *)&key_data, DATA_ZERO, (int16_t)sizeof(ecc_priv_key_slot_t));

    /* scan the table g_copy_ecc_keys[] (it may be there!) */
    for (i = (int32_t)NB_RAM_COPIES_ECC_KEYS - 1; (i >= 0) && (ER_OK == err); i--)
    {
        if (g_copy_ecc_keys[i].availability == E_AVAILABLE)
        {
            if ((g_copy_ecc_keys[i].key_group == key_group) &&
                ((uint32_t)g_copy_ecc_keys[i].vm_id == (uint32_t)vm_id) &&
                ((uint32_t)g_copy_ecc_keys[i].key_id == (uint32_t)key_id))
            {
                if ((uint32_t)g_in_use_counter_ecc[i] < NB_IN_USE_COUNTER_MAX)
                {
                    /* update the in use counter */
                    g_in_use_counter_ecc[i]++;
                    /* found: stop here */
                    *p_key = &g_copy_ecc_keys[i];
                    break;
                }
                else
                {
                    err = ER_MH_KEY_USER_OVER;
                }
            }
            else
            {
                if (0u != (uint32_t)g_in_use_counter_ecc[i])
                {
                    err = ER_PENDING;
                }
                else
                {
                    /* do nothing */
                }
            }
        }
        else
        {
            /* do nothing */
        }
    }
    
    /* Not found cache key */
    if ((NULL == *p_key) && (ER_OK == err))
    {
        /* check key group & index */
        switch (key_group)
        {
        case KEY_GRP_ECC:
            (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_ECC_PRIV, &key_number);
            if ((uint32_t)key_id >= (uint32_t)key_number)
            {
                /* invalid key index: stop here */
                err = ER_MH_KEY_INVALID_KEY_INDEX;
                break;
            }
            else
            {
                /* do nothing */
            }
            slot_num       = (uint8_t)((uint32_t)ML_CFG_VM_ECC_PRI_IDX_OFFSET(vm_id) + (uint32_t)key_id);
            data_set_type  = FLS_DS_ECC_PRIV_KEY;
            block_size     = MH_KEY_ECC_PRIV_DS_NB_BLOCKS;
            break;
        case KEY_GRP_ECC_PUB:
            (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_ECC_PUB, &key_number);
            if ((uint32_t)key_id >= (uint32_t)key_number)
            {
                /* invalid key index: stop here */
                err = ER_MH_KEY_INVALID_KEY_INDEX;
                break;
            }
            else
            {
                /* do nothing */
            }
            slot_num       = (uint8_t)((uint32_t)ML_CFG_VM_ECC_PUB_IDX_OFFSET(vm_id) + (uint32_t)key_id);
            data_set_type  = FLS_DS_ECC_PUB_KEY;
            block_size     = MH_KEY_ECC_PUB_DS_NB_BLOCKS;
            break;
        default:
            err = ER_MH_KEY_INVALID_KEY_GROUP;
            break;
    }

        if (ER_OK == err)
    {
            /* allocate some space for the key value */
            *p_key = &g_copy_ecc_keys[g_current_idx_ecc];
            (*p_key)->availability = E_NOT_AVAILABLE;

            /***********************************************
            * read the ECC key properties (incl. pub exp)
            ***********************************************/
            err = MM_FLS_Read(FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE,
                                data_set_type, slot_num, (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                                (uint8_t*)&key_data);
        }
        else
        {
            /* abort error */
        }

        if (ER_OK == err)
        {
            /* copy the key properties */
            L_Toolbox_MemCpy((uint8_t *)(*p_key)->p_properties, (const uint8_t *)&key_data.properties, (int16_t)sizeof(ecc_key_prop_t));
            /* Get Ecc key size (Invalid curve id error does not occur because checked when update the key */
            (void)MH_KEY_GetEccKeySize(key_data.properties.ecc_curve, &key_size);
            /* copy public point x */
            (*p_key)->p_pub_point->x.size = key_size;
            err = L_Toolbox_MemCpy_Sec((uint8_t *)(*p_key)->p_pub_point->x.value,
                                    (const uint8_t *)key_data.pub_point_x,
                                    (int16_t)key_size);
        }
        else
        {
            /* abort error */
        }

        if (ER_OK == err)
    {
            /* copy public point y */
            (*p_key)->p_pub_point->y.size = key_size;
            err = L_Toolbox_MemCpy_Sec((uint8_t *)(*p_key)->p_pub_point->y.value,
                                       (const uint8_t *)key_data.pub_point_y,
                                       (int16_t)key_size);
        }
        else
        {
            /* abort error */
        }

        if (ER_OK == err)
        {
            /* copy private scalar */
            if ((uint32_t)key_data.properties.priv_scalar_size_in_bits != 0u)
            {
                (*p_key)->p_priv_scalar->size = key_size;
                err = L_Toolbox_MemCpy_Sec((uint8_t *)(*p_key)->p_priv_scalar->value,
                                        (const uint8_t *)key_data.priv_scalar,
                                        (int16_t)key_size);
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            /* abort error */
        }

        if (ER_OK != err)
        {
            err = mh_KEY_ConvertErr(err);
            *p_key = NULL;
        }
        else
        {
            /* update the in use counter */
            g_in_use_counter_ecc[g_current_idx_ecc]++;

            /* copy the key data in the local table g_copy_ecc_keys[] */
            (*p_key)->availability = E_AVAILABLE;
            (*p_key)->key_group    = key_group;
            (*p_key)->key_id       = key_id;
            (*p_key)->vm_id        = vm_id;
        }

    }
    /* end if ((NULL == *p_key) && (ER_OK == err)) */
    else
    {
        /* abort error or found cache key */
    }
    
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0026]:[ICUMFW_UD_MH_KeyMNG_0026]
* Function name: mh_KEY_GetEccRam
* Description  : Retrieve the pointer to the RAM ECC key value & properties for a given idx
*                Returns 0 if the key is not available (i.e. not initialized in Flash)
* Arguments    : (in) p_job             : the pointer of job parameter
*                (in) key_id            : key id
*                (out) **p_key          : the pointer to the RAM ECC key value & properties
* Return Value : ER_OK
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_SLOT_EMPTY
***************************************************************************************/
static uint32_t mh_KEY_GetEccRam(icum_job_t *p_job, r_key_index_t key_id, ecc_key_t **p_key)
{
    uint32_t err;
    uint8_t key_number;
    uint8_t *p_ecc_ram_use_cnt;
    ecc_key_t *p_ecc_ram_keys;

    /* Initialize local variable */
    err = ER_OK;
    key_number = 0u;
    p_ecc_ram_keys = ECC_RAM_KEYS(p_job->vm_id, key_id);
    p_ecc_ram_use_cnt = ECC_RAM_KEYS_INUSE_COUNT(p_job->vm_id, key_id);
    *p_key = NULL;

    /* check that the key index is valid */
    (void)mh_KEY_MapSizeVM(p_job->vm_id, KEY_MAPPING_ECC_RAM, &key_number);
    if ((uint32_t)key_id >= (uint32_t)key_number)
    {
        /* invalid key index: stop here */
        err = ER_MH_KEY_INVALID_KEY_INDEX;
    }
    else
    {
        /* is the key available (previously initialized)? */
        if (E_AVAILABLE == p_ecc_ram_keys->availability)
        {
            if ((uint32_t)NB_IN_USE_COUNTER_MAX > (uint32_t)(*p_ecc_ram_use_cnt))
            {
                /* update the in use counter */
                *p_ecc_ram_use_cnt = (uint8_t)((uint32_t)(*p_ecc_ram_use_cnt) + NUM1);
                /* found: stop here */
                *p_key = p_ecc_ram_keys;
            }
            else
            {
                err = ER_MH_KEY_USER_OVER;
                *p_key = NULL;
            }
        }
        else
        {
            /* RAM key not available (not initialized): stop here */
            err = ER_MH_KEY_SLOT_EMPTY;
        }
    }

    return (err);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0028]:[ICUMFW_UD_MH_KeyMNG_0028]
* Function name: mh_KEY_ExtractEccPubKey
* Description  : Extract ECC public key from X509 certificate
* Arguments    : (in)  x509_cert_address        : address of X.509 certificate
*                (in)  x509_cert_size_in_bytes  : byte size of X.509 certificate
*                (out) *p_x                     : the pointer to ECC Public Key (x coordinate)
*                (out) *p_y                     : the pointer to ECC Public Key (y coordinate)
*                (out) *p_curve_id              : the pointer to curve id
* Return Value : ER_OK
*                ER_PARAM
*                ER_MH_KEY_INVALID_KEY_GROUP
***************************************************************************************/
static uint32_t mh_KEY_ExtractEccPubKey(uint32_t         x509_cert_address,
                                        uint32_t         x509_cert_size_in_bytes,
                                        pkc_small_data_t *p_x,
                                        pkc_small_data_t *p_y,
                                        r_ecc_curve_t    *p_curve_id)
{
    x509_cert_prm_t x509_cert_prm;
    uint32_t        byte_size;
    uint32_t        err;
    
     /* expected RSA Signature, not support ECDSA Signature */
    uint8_t         signature[R_CRYPT_BIGNUM_BYTESIZE];
    uint8_t         pub_point_x[SCALAR_SIZE_IN_BYTES];
    uint8_t         pub_point_y[SCALAR_SIZE_IN_BYTES];
    
    x509_cert_prm.p_cert = (const uint8_t *)x509_cert_address;
    x509_cert_prm.cert_size_in_bytes = (uint16_t)x509_cert_size_in_bytes;
    
    x509_cert_prm.p_signature                   = &signature[0];
    x509_cert_prm.sign_size_in_bytes            = sizeof(signature);
    x509_cert_prm.key.ecc.p_pub_point_x         = &pub_point_x[0];
    x509_cert_prm.key.ecc.point_x_size_in_bytes = sizeof(pub_point_x);
    x509_cert_prm.key.ecc.p_pub_point_y         = &pub_point_y[0];
    x509_cert_prm.key.ecc.point_y_size_in_bytes = sizeof(pub_point_y);

    err = L_X509_ParseCertificate(&x509_cert_prm);
    if (err != ER_OK)
    {
        err = ER_PARAM;
    }
    else if (x509_cert_prm.key_type != E_KEY_TYPE_ECC_PUB_KEY)
    {
        err = ER_MH_KEY_INVALID_KEY_GROUP;
    }
    else
    {
        byte_size = x509_cert_prm.key.ecc.point_x_size_in_bytes;
        err = L_BIGNUM_EncodeToSmallBigNum(&pub_point_x[0], E_BINARY_PLAIN, byte_size, p_x);
        if (err != ER_OK)
        {
            err = ER_PARAM;
        }
        else
        {
            byte_size = x509_cert_prm.key.ecc.point_y_size_in_bytes;
            err = L_BIGNUM_EncodeToSmallBigNum(&pub_point_y[0], E_BINARY_PLAIN, byte_size, p_y);
            if (err != ER_OK)
            {
                err = ER_PARAM;
            }
            else
            {
                *p_curve_id = x509_cert_prm.curve_id;
            }
        }
    }

    return (err);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0027]:[ICUMFW_UD_MH_KeyMNG_0027]
* Function name: mh_KEY_GetEccPubExt
* Description  : Retrieve the pointer to the ECC key values & properties from X.509 certificate
* Arguments    : (in) p_job             : the pointer of job parameter
*              : (in) key_group         : key group
*              : (in) key_id            : key id
*                (out) **p_key          : the pointer to the ECC key value & properties
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_MH_KEY_DATA_INVALID
***************************************************************************************/
static uint32_t mh_KEY_GetEccPubExt(icum_job_t *p_job, r_key_group_t key_group,
    r_key_index_t key_id, ecc_key_t **p_key)
{
#pragma alignvar(4)
    ecc_pub_ext_key_slot_t  key_data;
    dvc_calculate_t     dvc_param;
    dvc_t               calculated_dvc;
    uint8_t             vm_id;
    uint8_t             slot_num;
    uint8_t             data_set_type;
    uint8_t            key_number;
    uint16_t            block_size;
    uint32_t            resource_id;
    int32_t             i;
    uint32_t            err = ER_OK;

    /* Initialize variable */
    vm_id = p_job->vm_id;
    slot_num = 0u;
    *p_key = NULL;

    key_data.x509_cert_address         = 0u;
    key_data.x509_cert_size_in_bytes   = 0u;
    for (i = 0; i < DVC_T_SIZE; i++)
    {
        key_data.x509_dvc[i] = 0u;
    }
    key_data.flags.write_protected     = 0u;
    key_data.flags.disable_on_debug    = 0u;
    key_data.flags.invalidate          = 0u;

    L_Toolbox_MemSet((uint8_t *)&key_data, NUM0, (int16_t)sizeof(ecc_pub_ext_key_slot_t));

    /* scan the table g_copy_ecc_keys[] (it may be there!) */
    for (i = (int32_t)NB_RAM_COPIES_ECC_KEYS - 1; i >= 0; i--)
    {
        if (g_copy_ecc_keys[i].availability == E_AVAILABLE)
        {
            if ((g_copy_ecc_keys[i].key_group == key_group) &&
                ((uint32_t)g_copy_ecc_keys[i].vm_id == (uint32_t)vm_id) &&
                ((uint32_t)g_copy_ecc_keys[i].key_id == (uint32_t)key_id))
            {
                if ((uint32_t)g_in_use_counter_ecc[i] < NB_IN_USE_COUNTER_MAX)
                {
                    /* update the in use counter */
                    g_in_use_counter_ecc[i]++;
                    /* found: stop here */
                    *p_key = &g_copy_ecc_keys[i];
                }
                else
                {
                    err = ER_MH_KEY_USER_OVER;
                }
                goto end;
            }
            else
            {
                if (0u != (uint32_t)g_in_use_counter_ecc[i])
                {
                    err = ER_PENDING;
                    goto end;
                }
                else
                {
                    /* do nothing */
                }
            }
        }
        else
        {
            /* do nothing */
        }
    }
    
    /* check key group & index */
    (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_ECC_PUB_EXT, &key_number);
    if ((uint32_t)key_id >= (uint32_t)key_number)
    {
        /* invalid key index: stop here */
        err = ER_MH_KEY_INVALID_KEY_INDEX;
        goto end;
    }
    else
    {
        /* do nothing */
    }
    slot_num       = (uint8_t)((uint32_t)ML_CFG_VM_ECC_PUB_EXT_IDX_OFFSET(vm_id) + (uint32_t)key_id);
    data_set_type  = FLS_DS_ECC_PUB_EXT_KEY;
    block_size     = MH_KEY_ECC_PUB_EXT_NB_BLOCKS;
    
    /* allocate some space for the key value */
    *p_key = &g_copy_ecc_keys[g_current_idx_ecc];
    (*p_key)->availability = E_NOT_AVAILABLE;
    
    /***********************************************
    * read the ECC key properties (incl. pub exp)
    ***********************************************/
    err = MM_FLS_Read(FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE,
                         data_set_type, slot_num, (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                         (uint8_t*)&key_data);
    if (err != ER_OK)
    {
        err = mh_KEY_ConvertErr(err);
        *p_key = NULL;
        goto end;
    }
    else
    {
        /* do nothing */
    }
    
    /* get AES resource */
    err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
    if (err != ER_OK)
    {
        *p_key = NULL;
        goto end;
    }
    else
    {
        /* do nothing */
    }

    /* calculate the data validation cipher (DVC) of x.509 */
    dvc_param.p_input   = (const uint8_t*)key_data.x509_cert_address;
    dvc_param.data_size = (uint16_t)key_data.x509_cert_size_in_bytes;
    dvc_param.p_output  = &calculated_dvc;
    err = ML_DVC_Calculate(&dvc_param, ML_DVC_CALC_TYPE_CMAC);
    
    /* release AES resource(Do not check for errors) */
    (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
    switch (err)
    {
        case ER_OK:
        case ER_PENDING:
        case ER_IN_PROCESS:
            break;
        case ER_ML_DVC_AES_TIMEOUT:
            err = ER_MH_KEY_AES_TIMEOUT_ERROR;
            break;
        case ER_ML_DVC_ALIGN:
            err = ER_MH_KEY_AES_PRM_ERROR;
            break;
        default:
            err = ER_PARAM;
            break;
    }
    if (err != ER_OK)
    {
        *p_key = NULL;
        goto end;
    }
    else
    {
        /* do nothing */
    }
    
    /* compare with the value read from user data flash */
    err = (uint32_t)L_Toolbox_MemCmp((const uint8_t *)calculated_dvc.dvc, (const uint8_t *)key_data.x509_dvc, (int16_t)DVC_T_SIZE);
    if (err != 0u)
    {
        /* user data flash of x.509 is broken */
        *p_key = NULL;
        err = ER_MH_KEY_DATA_INVALID;
        goto end;
    }
    else
    {
        /* do nothing */
    }
    
    /*******************************************************************
    * ECC public key extraction from X.509 certificate
    ********************************************************************/
    err = mh_KEY_ExtractEccPubKey(key_data.x509_cert_address,
                                  key_data.x509_cert_size_in_bytes,
                                  &((*p_key)->p_pub_point->x),
                                  &((*p_key)->p_pub_point->y),
                                  &((*p_key)->p_properties->ecc_curve) );
    if (err != ER_OK)
    {
        /* failed to parse x.509 */
        *p_key = NULL;
        goto end;
    }
    else
    {
        /* do nothing */
    }
    
    /* copy the key properties */
    (*p_key)->p_properties->update_counter          = key_data.update_counter;
    (*p_key)->p_properties->priv_scalar_size_in_bits= 0u;
    (*p_key)->p_properties->flags.write_protected   = key_data.flags.write_protected;
    (*p_key)->p_properties->flags.disable_on_debug  = key_data.flags.disable_on_debug;
    (*p_key)->p_properties->flags.invalidate        = key_data.flags.invalidate;

    /* update the in use counter */
    g_in_use_counter_ecc[g_current_idx_ecc]++;
    
    /* copy the key data in the local table g_copy_ecc_keys[] */
    (*p_key)->availability = E_AVAILABLE;
    (*p_key)->key_group    = key_group;
    (*p_key)->key_id       = key_id;
    (*p_key)->vm_id        = vm_id;

end:
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0024]:[ICUMFW_UD_MH_KeyMNG_0024]
* Function name: MH_KEY_GetEccKey
* Description  : Retrieve the pointer to the ECC key values & properties
* Arguments    : (in) p_job             : the pointer of job parameter
*              : (in) key_group         : key group
*              : (in) key_id            : key id
*                (out) **p_key          : the pointer to the ECC key value & properties
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_USER_OVER
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SLOT_EMPTY
*                SERV_INVALID_CURVE_ID
***************************************************************************************/
uint32_t MH_KEY_GetEccKey(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id, ecc_key_t **p_key)
{
    uint16_t write_sts;
    uint32_t err;

    /* Initialize variable */
    err = ER_OK;
    write_sts = KEY_NOT_WRITING;
    *p_key = NULL;

    (void)mh_KEY_WritingJudgment(p_job, key_group, key_id, &write_sts);
    if (KEY_WRITING == (uint32_t)write_sts)
    {
        err = ER_MH_KEY_IN_USE;
    }
    else
    {
        /* Checks the range of VMID */
        err = mh_KEY_CheckVMID(p_job->vm_id);
    }

    if (ER_OK == err)
    {
        switch (key_group)
        {
            case KEY_GRP_ECC:
            case KEY_GRP_ECC_PUB:
                err = mh_KEY_GetEcc(p_job, key_group, key_id, p_key);
                break;
            case KEY_GRP_ECC_PUB_EXT:
                err = mh_KEY_GetEccPubExt(p_job, key_group, key_id, p_key);
                break;
            case KEY_GRP_ECC_RAM:
                err = mh_KEY_GetEccRam(p_job, key_id, p_key);
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

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0031]:[ICUMFW_UD_MH_KeyMNG_0031]
* Function name: mh_KEY_UpdateEccRam
* Description  : Update an ECC RAM key
* Arguments    : (in) p_job
*              : (in) p_new_key
* Return Value : ER_OK
*                ER_PARAM
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_INVALID_CURVE_ID
***************************************************************************************/
static uint32_t mh_KEY_UpdateEccRam(icum_job_t *p_job, ecc_key_t *p_new_key)
{
    uint32_t err;
    uint8_t vm_id;
    uint8_t *p_ecc_ram_use_cnt;
    ecc_key_t *p_ecc_ram_keys;
    uint32_t curve_size_in_bytes;

    /* Initialize local variable */
    err = ER_OK;
    vm_id = p_job->vm_id;
    curve_size_in_bytes = 0u;
    p_ecc_ram_keys = ECC_RAM_KEYS(vm_id, p_new_key->key_id);
    p_ecc_ram_keys->p_properties = ECC_RAM_KEYS_PROPERTY(vm_id, p_new_key->key_id);
    p_ecc_ram_keys->p_priv_scalar = ECC_RAM_KEYS_PRIV_SCALAR(vm_id, p_new_key->key_id);
    p_ecc_ram_keys->p_pub_point = ECC_RAM_KEYS_PUB_POINT(vm_id, p_new_key->key_id);
    p_ecc_ram_use_cnt = ECC_RAM_KEYS_INUSE_COUNT(vm_id, p_new_key->key_id);

    err = mh_KEY_CheckUpdateEccParam(p_new_key, &curve_size_in_bytes);
    if (ER_OK != err)
    {
        /* invalid curve id or parameter error */
    }
    /* the key is used */
    else if (NUM0 != (uint32_t)(*p_ecc_ram_use_cnt))
    {
        err = ER_MH_KEY_IN_USE;
    }
    else
    {
        p_ecc_ram_keys->p_properties->ecc_curve                = p_new_key->p_properties->ecc_curve;
        p_ecc_ram_keys->p_properties->priv_scalar_size_in_bits = p_new_key->p_properties->priv_scalar_size_in_bits;
        p_ecc_ram_keys->p_properties->update_counter           = 0u;
        p_ecc_ram_keys->p_properties->flags.write_protected    = 0u;
        p_ecc_ram_keys->p_properties->flags.disable_on_debug   = p_new_key->p_properties->flags.disable_on_debug;
        p_ecc_ram_keys->p_properties->flags.invalidate         = 0u;

        /* memory clean up */
        L_Toolbox_MemSet((uint8_t *)p_ecc_ram_keys->p_priv_scalar, 0x00u,   (int16_t)sizeof(pkc_small_data_t));
        L_Toolbox_MemSet((uint8_t *)p_ecc_ram_keys->p_pub_point,   0x00u,   (int16_t)sizeof(ecc_point_affine_t));
        
        /* public point */
        L_Toolbox_MemCpy((uint8_t *)p_ecc_ram_keys->p_pub_point,
                        (const uint8_t *)p_new_key->p_pub_point,
                        (int16_t)sizeof(ecc_point_affine_t));
        /* private scalar */
        if ((uint32_t)p_new_key->p_properties->priv_scalar_size_in_bits > 0u)
        {
            L_Toolbox_MemCpy((uint8_t *)p_ecc_ram_keys->p_priv_scalar,
                            (const uint8_t *)p_new_key->p_priv_scalar,
                            (int16_t)sizeof(pkc_small_data_t));
        }
        else
        {
            /* do nothing */
        }
        
        /* set the property and values */
        p_ecc_ram_keys->availability = E_AVAILABLE;
        p_ecc_ram_keys->key_group    = p_new_key->key_group;
        p_ecc_ram_keys->key_id       = p_new_key->key_id;
        p_ecc_ram_keys->vm_id        = vm_id;
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0030]:[ICUMFW_UD_MH_KeyMNG_0030]
* Function name: mh_KEY_UpdateEcc
* Description  : Update an ECC NVM key (KEY_GRP_ECC, KEY_GRP_ECC_PUB)
* Arguments    : (in) p_job
*                (in) operation
*                (in) job_id
*                (in) p_new_key
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_WRITING
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_WRITE_PROTECTED
*                ER_MH_KEY_INVALID_CURVE_ID
***************************************************************************************/
static uint32_t mh_KEY_UpdateEcc(icum_job_t *p_job, set_operation_t operation, ecc_key_t *p_new_key)
{
    ecc_key_t          *p_key;
#pragma alignvar(4)
    ecc_priv_key_slot_t key_data;
    uint8_t         vm_id;
    uint8_t         slot_num;
    uint8_t         data_set_type;
    uint16_t        write_sts;
    uint16_t        block_size;
    int32_t         i;
    uint32_t        curve_size_in_bytes;
    uint32_t        err = ER_OK;

    /* Initialize local variable */
    vm_id = p_job->vm_id;
    slot_num = 0u;
    curve_size_in_bytes = 0u;

    /* check key group & index */
    if (KEY_GRP_ECC == p_new_key->key_group)
    {
        slot_num       = (uint8_t)((uint32_t)ML_CFG_VM_ECC_PRI_IDX_OFFSET(vm_id) + (uint32_t)p_new_key->key_id);
        data_set_type  = FLS_DS_ECC_PRIV_KEY;
        block_size     = MH_KEY_ECC_PRIV_DS_NB_BLOCKS;
    }
    else
    {
        slot_num       = (uint8_t)((uint32_t)ML_CFG_VM_ECC_PUB_IDX_OFFSET(vm_id) + (uint32_t)p_new_key->key_id);
        data_set_type  = FLS_DS_ECC_PUB_KEY;
        block_size     = MH_KEY_ECC_PUB_DS_NB_BLOCKS;
    }

    /* read the ECC key properties (incl. pub exp) */
    if (operation == E_KEY_OPE_FIRST)
    {
        (void)mh_KEY_WritingStsGet(&write_sts);
        if (KEY_WRITING == (uint32_t)write_sts)
        {
            /* Key writing in progress */
            err = ER_MH_KEY_WRITING;
            goto end;
        }
        else
        {
            /* do nothing */
        }

        /* scan the table g_copy_ecc_keys[] (it may be there!) */
        for (i = (int32_t)NB_RAM_COPIES_ECC_KEYS - 1; i >= 0; i--)
        {
            if ((g_copy_ecc_keys[i].availability == E_AVAILABLE) &&
                (g_copy_ecc_keys[i].key_group == p_new_key->key_group) &&
                ((uint32_t)g_copy_ecc_keys[i].vm_id == (uint32_t)vm_id) &&
                ((uint32_t)g_copy_ecc_keys[i].key_id == (uint32_t)p_new_key->key_id))
            {
                /* found: stop here */
                if (0u == (uint32_t)g_in_use_counter_ecc[i])
                {
                    /* key is not use */
                    p_key = &g_copy_ecc_keys[i];
                    /* mark it as "not available" */
                    p_key->availability = E_NOT_AVAILABLE;
                    break;
                }
                else
                {
                    err = ER_MH_KEY_IN_USE;
                    goto end;
                }
            }
            else
            {
                /* do nothing */
            }
        }

        key_data.properties.flags.write_protected = 0u;
        err = MM_FLS_Read(FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE,
                             data_set_type, slot_num, (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                             (uint8_t*)&key_data);
        if (err == ER_OK)
        {
            if ((uint32_t)key_data.properties.flags.write_protected == 1)
            {
                err = ER_MH_KEY_WRITE_PROTECTED;
                goto end;
            }
            else
            {
                /* do nothing */
            }
        }
        else if (err == ER_MM_FLS_DATA_BLANK)
        {
            /* in case of blank No need to check */
        }
        else
        {
            goto end;
        }

        err = mh_KEY_CheckUpdateEccParam(p_new_key, &curve_size_in_bytes);
        if (ER_OK != err)
        {
            /* invalid curve id or parameter error */
            goto end;
        }
        else
        {
            /* do nothing */
        }

        L_Toolbox_MemSet((uint8_t *)&g_storage_keys_ecc, NUM0, (int16_t)sizeof(ecc_priv_key_slot_t));
        /* copy the public point x */
        err = L_Toolbox_MemCpy_Sec((uint8_t *)g_storage_keys_ecc.pub_point_x,
                                   (const uint8_t *)p_new_key->p_pub_point->x.value,
                                   (int16_t)curve_size_in_bytes);
        if (ER_OK != err)
        {
            /* copy error */
            goto end;
        }
        else
        {
            /* do nothing */
        }

        /* convert the modulus from big number into binary plain */
        if ((ECC_CURVE_CURVE25519 == p_new_key->p_properties->ecc_curve) ||
            (ECC_CURVE_CURVE448 == p_new_key->p_properties->ecc_curve))
        {
            /* Only the public point x */
        }
        else
        {
            /* copy the public point y */
            err = L_Toolbox_MemCpy_Sec((uint8_t *)g_storage_keys_ecc.pub_point_y,
                                       (const uint8_t *)p_new_key->p_pub_point->y.value,
                                       (int16_t)curve_size_in_bytes);
            if (ER_OK != err)
            {
                /* copy error */
                goto end;
            }
            else
            {
                /* do nothing */
            }
        }

        /* copy the private scaler */
        if (p_new_key->key_group == KEY_GRP_ECC)
        {
            err = L_Toolbox_MemCpy_Sec((uint8_t *)g_storage_keys_ecc.priv_scalar,
                                       (const uint8_t *)p_new_key->p_priv_scalar->value,
                                       (int16_t)curve_size_in_bytes);
            if (ER_OK != err)
            {
                /* copy error */
                goto end;
            }
            else
            {
                /* do nothing */
            }
            /* set the private exponent size */
            g_storage_keys_ecc.properties.priv_scalar_size_in_bits  = curve_size_in_bytes * NUM8;
        }
        else
        {
            g_storage_keys_ecc.properties.priv_scalar_size_in_bits  = 0u;
        }
        
        /* set the property values */
        g_storage_keys_ecc.properties.update_counter          = 0u;
        g_storage_keys_ecc.properties.ecc_curve               = p_new_key->p_properties->ecc_curve;
        g_storage_keys_ecc.properties.flags.write_protected   = p_new_key->p_properties->flags.write_protected; 
        g_storage_keys_ecc.properties.flags.disable_on_debug  = p_new_key->p_properties->flags.disable_on_debug;
        g_storage_keys_ecc.properties.flags.invalidate        = p_new_key->p_properties->flags.invalidate;

        (void)mh_KEY_WritingSet(p_job->vm_id, p_new_key->key_group, p_new_key->key_id);

        err = MM_FLS_Write(p_job->job_id, FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, data_set_type, slot_num,
                           (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                              (uint8_t*)&g_storage_keys_ecc);
    }
    else
    {
        err = MM_FLS_Write(p_job->job_id, FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, data_set_type, slot_num,
                           (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                              (uint8_t*)&g_storage_keys_ecc);
    }
    if ((err != ER_IN_PROCESS) && (err != ER_MM_FLS_IN_PROGRESS))
    {
        /* Key writing is completed. Clear write information */
        (void)mh_KEY_WritingSet(0x00u, LAST_ITEM_KEY_GRP, KEY_ID_LAST_ITEM);
    }
    else
    {
        /* do nothing */
    }
    
end:
    err = mh_KEY_ConvertErr(err);
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0029]:[ICUMFW_UD_MH_KeyMNG_0029]
* Function name: MH_KEY_SetEccKey
* Description  : Update an ECC NVM key (KEY_GRP_ECC, KEY_GRP_ECC_PUB) & ECC RAM key
* Arguments    : (in) p_job
*                (in) operation
*                (in) job_id
*                (in) p_new_key
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_WRITING
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_PROG
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_DEST_MEM_ERROR
*                ER_MH_KEY_WRITE_PROTECTED
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_SLOT_EMPTY
*                ER_MH_KEY_INVALID_CURVE_ID
***************************************************************************************/
uint32_t MH_KEY_SetEccKey(icum_job_t *p_job, set_operation_t operation, ecc_key_t *p_new_key)
{
    uint32_t err;
    uint8_t  key_number;

    /* Initialize variable */
    err = ER_OK;
    key_number = 0u;

    /* Checks the range of VMID */
    err = mh_KEY_CheckVMID(p_job->vm_id);
    if (ER_OK == err)
    {
        /* retrieve the key to be used */
        switch (p_new_key->key_group)
        {
            case KEY_GRP_ECC:
                (void)mh_KEY_MapSizeVM(p_job->vm_id, KEY_MAPPING_ECC_PRIV, &key_number);
                if ((uint32_t)p_new_key->key_id >= (uint32_t)key_number)
                {
                    err = ER_MH_KEY_INVALID_KEY_INDEX;
                }
                else
                {
                    err = mh_KEY_UpdateEcc(p_job, operation, p_new_key);
                }
                break;
            case KEY_GRP_ECC_PUB:
                (void)mh_KEY_MapSizeVM(p_job->vm_id, KEY_MAPPING_ECC_PUB, &key_number);
                if ((uint32_t)p_new_key->key_id >= (uint32_t)key_number)
                {
                    err = ER_MH_KEY_INVALID_KEY_INDEX;
                }
                else
                {
                    err = mh_KEY_UpdateEcc(p_job, operation, p_new_key);
                }
                break;
            case KEY_GRP_ECC_RAM:
                (void)mh_KEY_MapSizeVM(p_job->vm_id, KEY_MAPPING_ECC_RAM, &key_number);
                if ((uint32_t)p_new_key->key_id >= (uint32_t)key_number)
                {
                    err = ER_MH_KEY_INVALID_KEY_INDEX;
                }
                else
                {
                    err = mh_KEY_UpdateEccRam(p_job, p_new_key);
                }
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

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0033]:[ICUMFW_UD_MH_KeyMNG_0033]
* Function name: MH_KEY_SetEccPubExt
* Description  : Update an ECC NVM key (KEY_GRP_ECC_PUB_EXT) 
* Arguments    : (in) p_job
*                (in) operation
*                (in) job_id
*                (in) p_new_key
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MH_KEY_WRITE_PROTECTED
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_WRITING
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_PROG
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_DEST_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SLOT_EMPTY
***************************************************************************************/
uint32_t MH_KEY_SetEccPubExt(icum_job_t *p_job, set_operation_t operation, ecc_pub_ext_t *p_new_key)
{
    ecc_key_t          *p_key;
#pragma alignvar(4)
    ecc_pub_ext_key_slot_t  key_data;
    dvc_calculate_t     dvc_param;
    dvc_t               calculated_dvc;
    uint8_t             vm_id;
    uint8_t             slot_num;
    uint8_t             data_set_type;
    uint8_t            key_number;
    uint16_t            write_sts;
    uint16_t            block_size;
    uint32_t            resource_id;
    int32_t             i;
    uint32_t            err;

    /* Initialize local variable */
    err = ER_OK;
    vm_id = p_job->vm_id;
    slot_num = 0u;

    /* Checks the range of VMID */
    err = mh_KEY_CheckVMID(vm_id);
    if (ER_OK != err)
    {
        goto end;
    }
    else
    {
        /* do nothing */
    }

    /* check key group & index */
    (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_ECC_PUB_EXT, &key_number);
    if ((uint32_t)p_new_key->key_id >= (uint32_t)key_number)
    {
        /* invalid key index: stop here */
        err = ER_MH_KEY_INVALID_KEY_INDEX;
        goto end;
    }
    else
    {
        /* do nothing */
    }
    slot_num       = (uint8_t)((uint32_t)ML_CFG_VM_ECC_PUB_EXT_IDX_OFFSET(vm_id) + (uint32_t)p_new_key->key_id);
    data_set_type  = FLS_DS_ECC_PUB_EXT_KEY;
    block_size     = MH_KEY_ECC_PUB_EXT_NB_BLOCKS;
    
    /* read the ECC key properties (incl. pub exp) */
    if (operation == E_KEY_OPE_FIRST)
    {
        (void)mh_KEY_WritingStsGet(&write_sts);
        if (KEY_WRITING == (uint32_t)write_sts)
        {
            /* Key writing in progress */
            err = ER_MH_KEY_WRITING;
            goto end;
        }
        else
        {
            /* do nothing */
        }
        /* scan the table g_copy_ecc_keys[] (it may be there!) */
        for (i = (int32_t)NB_RAM_COPIES_ECC_KEYS - 1; i >= 0; i--)
        {
            if ((g_copy_ecc_keys[i].availability == E_AVAILABLE) &&
                (g_copy_ecc_keys[i].key_group == KEY_GRP_ECC_PUB_EXT) &&
                ((uint32_t)g_copy_ecc_keys[i].vm_id == (uint32_t)vm_id) &&
                ((uint32_t)g_copy_ecc_keys[i].key_id == (uint32_t)p_new_key->key_id))
            {
                /* found: stop here */
                if (0u == (uint32_t)g_in_use_counter_ecc[i])
                {
                    /* key is not use */
                    p_key = &g_copy_ecc_keys[i];
                    /* mark it as "not available" */
                    p_key->availability = E_NOT_AVAILABLE;
                    break;
                }
                else
                {
                    err = ER_MH_KEY_IN_USE;
                    goto end;
                }
            }
            else
            {
                /* do nothing */
            }
        }

        key_data.flags.write_protected = 0u;
        err = MM_FLS_Read(FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE,
                             data_set_type, slot_num, (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                             (uint8_t*)&key_data);
        if (err == ER_OK)
        {
            if ((uint32_t)key_data.flags.write_protected == 1)
            {
                err = ER_MH_KEY_WRITE_PROTECTED;
                goto end;
            }
            else
            {
                /* do nothing */
            }
        }
        else if (err == ER_MM_FLS_DATA_BLANK)
        {
            /* in case of blank No need to check */
        }
        else
        {
            goto end;
        }

        L_Toolbox_MemSet((uint8_t *)&g_storage_keys_ecc_ext, NUM0,
            (int16_t)sizeof(ecc_pub_ext_key_slot_t));
        
        /* get AES resource */
        err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
        if (err != ER_OK)
        {
            goto end;
        }
        else
        {
            /* do nothing */
        }

        /* calculate the data validation cipher (DVC) of x.509 */
        dvc_param.p_input   = (const uint8_t*)p_new_key->x509_cert_address;
        dvc_param.data_size = (uint16_t)p_new_key->x509_cert_size_in_bytes;
        dvc_param.p_output  = &calculated_dvc;
        err = ML_DVC_Calculate(&dvc_param, ML_DVC_CALC_TYPE_CMAC);
        
        /* release AES resource(Do not check for errors) */
        (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
        switch (err)
        {
            case ER_OK:
            case ER_PENDING:
            case ER_IN_PROCESS:
                break;
            case ER_ML_DVC_AES_TIMEOUT:
                err = ER_MH_KEY_AES_TIMEOUT_ERROR;
                break;
            case ER_ML_DVC_ALIGN:
                err = ER_MH_KEY_AES_PRM_ERROR;
                break;
            default:
                err = ER_PARAM;
                break;
        }
        if (err != ER_OK)
        {
            goto end;
        }
        else
        {
            /* do nothing */
        }
        
        /* set the property values */
        g_storage_keys_ecc_ext.update_counter          = 0u;
        g_storage_keys_ecc_ext.flags.write_protected   = p_new_key->flags.write_protected; 
        g_storage_keys_ecc_ext.flags.disable_on_debug  = p_new_key->flags.disable_on_debug;
        g_storage_keys_ecc_ext.flags.invalidate        = p_new_key->flags.invalidate;
        g_storage_keys_ecc_ext.x509_cert_address       = p_new_key->x509_cert_address;
        g_storage_keys_ecc_ext.x509_cert_size_in_bytes = p_new_key->x509_cert_size_in_bytes;
        
        L_Toolbox_MemCpy(g_storage_keys_ecc_ext.x509_dvc, &calculated_dvc.dvc[0], (int16_t)DVC_T_SIZE);
        
        (void)mh_KEY_WritingSet(vm_id, KEY_GRP_ECC_PUB_EXT, p_new_key->key_id);

        err = MM_FLS_Write(p_job->job_id, FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, data_set_type, slot_num,
                           (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                              (uint8_t*)&g_storage_keys_ecc_ext);
    }
    else
    {
        err = MM_FLS_Write(p_job->job_id, FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, data_set_type, slot_num,
                           (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                              (uint8_t*)&g_storage_keys_ecc_ext);
    }
    if ((err != ER_IN_PROCESS) && (err != ER_MM_FLS_IN_PROGRESS))
    {
        /* Key writing is completed. Clear write information */
        (void)mh_KEY_WritingSet(0x00u, LAST_ITEM_KEY_GRP, KEY_ID_LAST_ITEM);
    }
    else
    {
        /* do nothing */
    }
    
end:
    err = mh_KEY_ConvertErr(err);
    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0058]:[ICUMFW_UD_MH_KeyMNG_0058]
* Function name: mh_KEY_FreeCacheEcc
* Description  : Decrement the in-use counter from the specified key group, key id
* Arguments    : (in)  p_job            : the pointer of job parameter
* Arguments    : (in)  key_group        : ECC Key Group
*                                           Range: KEY_GRP_ECC
*                                                  KEY_GRP_ECC_PUB
*                                                  KEY_GRP_ECC_RAM
*                                                  KEY_GRP_ECC_PUB_EXT
*                (in)  key_id
* Return Value : ER_OK
*                ER_PARAM
*                ER_MH_KEY_DATA_NOT_FOUND
****************************************************************************************/
uint32_t mh_KEY_FreeCacheEcc(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id)
{
    int32_t i;
    uint32_t err;
    uint8_t vm_id;
    uint8_t key_number;
    uint8_t *p_ecc_ram_use_cnt;
    ecc_key_t *p_ecc_ram_keys;

    /* Initialize local variable */
    i = 0;
    err = ER_OK;
    vm_id = p_job->vm_id;
    key_number = 0x00u;
    p_ecc_ram_use_cnt = NULL;
    p_ecc_ram_keys = NULL;

    if (key_group == KEY_GRP_ECC_RAM)
    {
        (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_ECC_RAM, &key_number);
        for (i = ((int32_t)key_number) - 1; i >= 0; i--)
        {
            p_ecc_ram_keys = ECC_RAM_KEYS(vm_id, i);
            p_ecc_ram_use_cnt = ECC_RAM_KEYS_INUSE_COUNT(vm_id, i);
            if ((p_ecc_ram_keys->availability == E_AVAILABLE) &&
                (p_ecc_ram_keys->key_group == key_group) &&
                ((uint32_t)p_ecc_ram_keys->vm_id == (uint32_t)vm_id) &&
                ((uint32_t)p_ecc_ram_keys->key_id == (uint32_t)key_id))
            {
                if (NUM0 != (uint32_t)(*p_ecc_ram_use_cnt))
                {
                    *p_ecc_ram_use_cnt = (uint8_t)((uint32_t)(*p_ecc_ram_use_cnt) - NUM1);
                }
                else
                {
                    err = ER_PARAM;
                }
                break;
            }
            else
            {
                /* do nothing */
            }
        } /* end of for (i = ((int32_t)key_mapping.nb_ecc_ram_keys) - 1; i >= 0; i--) */
        if (i < 0)
        {
            err = ER_MH_KEY_DATA_NOT_FOUND;
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* scan the table copy_ecc_keys[] (it may be there!) */
        for (i = ((int32_t)NB_RAM_COPIES_ECC_KEYS - 1); i >= 0; i--)
        {
            if ((g_copy_ecc_keys[i].availability == E_AVAILABLE) &&
                (g_copy_ecc_keys[i].key_group == key_group) &&
                ((uint32_t)g_copy_ecc_keys[i].vm_id == (uint32_t)vm_id) &&
                ((uint32_t)g_copy_ecc_keys[i].key_id == (uint32_t)key_id))
            {
                if (0u != (uint32_t)g_in_use_counter_ecc[i])
                {
                    g_in_use_counter_ecc[i]--;
                }
                else
                {
                    err = ER_PARAM;
                }
                break;
            }
            else
            {
                /* do nothing */
            }
        } /* end of for (i = ((int32_t)NB_RAM_COPIES_ECC_KEYS - 1); i >= 0; i--) */
        if (i < 0)
        {
            err = ER_MH_KEY_DATA_NOT_FOUND;
        }
        else
        {
            /* do nothing */
        }
    }
    return (err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0062]:[ICUMFW_UD_MH_KeyMNG_0062]
* Function name: mh_KEY_DeleteEccRamKey
* Description  : Delete ECC key in ram
* Arguments    : void
* Return Value : ER_OK
*              : ER_MH_KEY_IN_USE
***************************************************************************************/
uint32_t mh_KEY_DeleteEccRamKey(void)
{
    int32_t i;
    uint32_t err;
    uint32_t vm_id;
    uint8_t key_number;
    uint8_t *p_ecc_ram_use_cnt;
    ecc_key_t *p_ecc_ram_keys;
    sys_cfg_vm_t vm_cfg;

    /* Initialize local variable */
    i = 0;
    err = ER_OK;
    key_number = 0x00u;
    p_ecc_ram_keys = NULL;
    p_ecc_ram_use_cnt = NULL;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));

    (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
    for (vm_id = NUM0; (vm_id < (uint32_t)vm_cfg.nb_vms) && (ER_OK == err); vm_id++)
    {
        (void)mh_KEY_MapSizeVM((uint8_t)vm_id, KEY_MAPPING_ECC_RAM, &key_number);
        for (i = ((int32_t)key_number) - 1; i >= 0; i--)
        {
            p_ecc_ram_keys = ECC_RAM_KEYS(vm_id, i);
            p_ecc_ram_keys->p_properties = ECC_RAM_KEYS_PROPERTY(vm_id, i);
            p_ecc_ram_keys->p_priv_scalar = ECC_RAM_KEYS_PRIV_SCALAR(vm_id, i);
            p_ecc_ram_keys->p_pub_point = ECC_RAM_KEYS_PUB_POINT(vm_id, i);
            p_ecc_ram_use_cnt = ECC_RAM_KEYS_INUSE_COUNT(vm_id, i);
            if ((uint32_t)(*p_ecc_ram_use_cnt) != NUM0)
            {
                err = ER_MH_KEY_IN_USE;
                break;
            }
            else
            {
                p_ecc_ram_keys->availability = E_NOT_AVAILABLE;
                p_ecc_ram_keys->key_group = (r_key_group_t)0u;
                p_ecc_ram_keys->key_id = 0x00u;
                p_ecc_ram_keys->vm_id = 0x00u;
                L_Toolbox_MemSet((uint8_t*)p_ecc_ram_keys->p_properties, NUM0,(int16_t)sizeof(ecc_key_prop_t));
                L_Toolbox_MemSet((uint8_t*)p_ecc_ram_keys->p_priv_scalar, NUM0, (int16_t)sizeof(pkc_small_data_t));
                L_Toolbox_MemSet((uint8_t*)p_ecc_ram_keys->p_pub_point, NUM0, (int16_t)sizeof(ecc_point_affine_t));
                *p_ecc_ram_use_cnt = 0x00u;
            }
        }
    }

    return (err);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0067]:[ICUMFW_UD_MH_KeyMNG_0067]
* Function name: mh_KEY_DeleteEccCacheKey
* Description  : Delete ECC key in cache
* Arguments    : void
* Return Value : ER_OK
*              : ER_MH_KEY_IN_USE
***************************************************************************************/
uint32_t mh_KEY_DeleteEccCacheKey(void)
{
    uint32_t i;
    uint32_t err = ER_OK;
    
    for (i = 0u; i < NB_RAM_COPIES_ECC_KEYS; i++)
    {
        if (0U != (uint32_t)g_in_use_counter_ecc[i])
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
        for (i = 0u; i < NB_RAM_COPIES_ECC_KEYS; i++)
        {
            g_copy_ecc_keys[i].availability = E_NOT_AVAILABLE;
            g_copy_ecc_keys[i].key_group = (r_key_group_t)0u;
            g_copy_ecc_keys[i].key_id = 0u;
            g_copy_ecc_keys[i].vm_id = 0x00u;
            L_Toolbox_MemSet((uint8_t*)&g_buffer_key_properties_ecc[i], NUM0,(int16_t)sizeof(ecc_key_prop_t));
            L_Toolbox_MemSet((uint8_t*)&g_buffer_priv_scalar[i], NUM0, (int16_t)sizeof(pkc_small_data_t));
            L_Toolbox_MemSet((uint8_t*)&g_buffer_pub_point[i], NUM0, (int16_t)sizeof(ecc_point_affine_t));
            g_in_use_counter_ecc[i] = 0u;
        }
    }
    else
    {
        /* do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0082]:[ICUMFW_UD_MH_KeyMNG_0082]
* Function name: MH_KEY_GetEccKeySize
* Description  : Get ECC key size
* Arguments    : (in)  ecc_curve_id
*              : (out) p_key_size
* Return Value : ER_OK
*                ER_MH_KEY_INVALID_CURVE_ID
***************************************************************************************/
uint32_t MH_KEY_GetEccKeySize(r_ecc_curve_t ecc_curve_id, uint32_t *p_key_size)
{
    uint32_t err;

    err = ER_OK;

    switch (ecc_curve_id)
    {
        case ECC_CURVE_NIST_P192:
            *p_key_size = (NUM192 / NUM8);
            break;
        case ECC_CURVE_NIST_P224:
            *p_key_size = (NUM224 / NUM8);
            break;
        case ECC_CURVE_NIST_P256:
        case ECC_CURVE_BRAINPOOL_P256:
        case ECC_CURVE_BRAINPOOL_P256_T:
        case ECC_CURVE_CURVE25519:
        case ECC_CURVE_ED25519:
            *p_key_size = (NUM256 / NUM8);
            break;
        case ECC_CURVE_BRAINPOOL_P320:
        case ECC_CURVE_BRAINPOOL_P320_T:
            *p_key_size = (NUM320 / NUM8);
            break;
        case ECC_CURVE_NIST_P384:
        case ECC_CURVE_BRAINPOOL_P384:
        case ECC_CURVE_BRAINPOOL_P384_T:
            *p_key_size = (NUM384 / NUM8);
            break;
        case ECC_CURVE_CURVE448:
            *p_key_size = (NUM448 / NUM8);
            break;
        case ECC_CURVE_ED448:
            *p_key_size = (NUM480 / NUM8);
            break;
        case ECC_CURVE_BRAINPOOL_P512:
        case ECC_CURVE_BRAINPOOL_P512_T:
            *p_key_size = (NUM512 / NUM8);
            break;
        case ECC_CURVE_NIST_P521:
            *p_key_size = (NUM544 / NUM8);
            break;
        case ECC_CURVE_BRAINPOOL_P192:
        case ECC_CURVE_BRAINPOOL_P224:
        default:
            /* never occur */
            err = ER_MH_KEY_INVALID_CURVE_ID;
            *p_key_size = 0u;
            break;
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0098]:[ICUMFW_UD_MH_KeyMNG_0098]
* Function name: mh_KEY_CheckUpdateEccParam
* Description  : Checks update ECC key data
* Arguments    : (in)  p_new_key
*              : (out) p_key_size
* Return Value : ER_OK
*                ER_PARAM
*                ER_MH_KEY_INVALID_CURVE_ID
***************************************************************************************/
static uint32_t mh_KEY_CheckUpdateEccParam(ecc_key_t *p_new_key, uint32_t *p_key_size)
{
    uint32_t err;
    uint32_t new_key_size_in_bytes;

    /* Initialize parameter */
    err = ER_OK;
    *p_key_size = 0u;
    new_key_size_in_bytes = 0u;

    /* Get Ecc key size */
    err = MH_KEY_GetEccKeySize(p_new_key->p_properties->ecc_curve, p_key_size);
    if (ER_OK == err)
    {
        /* Checks the private scalar key size of key property */
        if (0u < (uint32_t)p_new_key->p_properties->priv_scalar_size_in_bits)
        {
            new_key_size_in_bytes = (p_new_key->p_properties->priv_scalar_size_in_bits + NUM7) / NUM8;
            if (*p_key_size == new_key_size_in_bytes)
            {
                /* OK parameter : do nothing */
            }
            else
            {
                err = ER_PARAM;
            }
        }
        else
        {
            /* do nothing */
        }

        /* Checks the private scalar key size */
        if ((ER_OK == err) && (0u < (uint32_t)p_new_key->p_priv_scalar->size))
        {
            if (*p_key_size == (uint32_t)p_new_key->p_priv_scalar->size)
            {
                /* OK parameter : do nothing */
            }
            else
            {
                err = ER_PARAM;
            }
        }
        else
        {
            /* do nothing */
        }

        /* Checks the public point x key size */
        if ((ER_OK == err) && (0u < (uint32_t)p_new_key->p_pub_point->x.size))
        {
            if (*p_key_size == (uint32_t)p_new_key->p_pub_point->x.size)
            {
                /* OK parameter : do nothing */
            }
            else
            {
                err = ER_PARAM;
            }
        }
        else
        {
            /* do nothing */
        }

        /* Checks the public point y key size */
        if ((ER_OK == err) && (0u < (uint32_t)p_new_key->p_pub_point->y.size))
        {
            if (*p_key_size == (uint32_t)p_new_key->p_pub_point->y.size)
            {
                /* OK parameter : do nothing */
            }
            else
            {
                err = ER_PARAM;
            }
        }
        else
        {
            /* do nothing */
        }

    }
    else
    {
        /* invalid curve id */
    }

    return (err);
}
