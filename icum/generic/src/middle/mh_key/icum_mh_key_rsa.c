/******************************************************************************/
/* Component Name  Key Manager (RSA)                                          */
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
#include "icum_mh_key_pub.h"
#include "icum_mh_key.h"
#include MM_FLASH_PUBLIC_H
#include "icum_l_toolbox_pub.h"
#include "icum_ml_dvc_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include "icum_ml_cfg_pub.h"

/**********************************************************************************
* Local defines
**********************************************************************************/
#define RSA_ROM_JOB_ID          (0x0087u)

/* unsigned data size */
#define NUM7                        (7u)
#define NUM1024                     (1024u)
#define NUM2048                     (2048u)

/**********************************************************************************
* Local type definition
**********************************************************************************/

typedef struct rsa_priv_key_slot
{
    uint8_t             dummy[FLS_DATA_SLOT_MGMT_SIZE];
    rsa_key_prop_t      properties;
    uint8_t             pub_exponent[PUB_EXP_SIZE_IN_BYTES];
    uint8_t             modulus[MODULUS_SIZE_IN_BYTES];
    uint8_t             priv_exponent[PRIV_EXP_SIZE_IN_BYTES];
    uint8_t             reserved[FLS_DF_BLOCK_SIZE - ((FLS_DATA_SLOT_MGMT_SIZE
                                                    + (uint32_t)sizeof(rsa_key_prop_t)
                                                    + PUB_EXP_SIZE_IN_BYTES
                                                    + MODULUS_SIZE_IN_BYTES
                                                    + PRIV_EXP_SIZE_IN_BYTES
                                                    ) % FLS_DF_BLOCK_SIZE)];
} rsa_priv_key_slot_t;

typedef struct rsa_pub_ext_slot
{
    uint8_t                 dummy[FLS_DATA_SLOT_MGMT_SIZE];
    key_counter_t           update_counter;
    rsa_flags_t             flags;
    uint32_t                x509_cert_address;
    uint32_t                x509_cert_size_in_bytes;
    uint8_t                 x509_dvc[DVC_T_SIZE];
    uint8_t                 reserved[FLS_DF_BLOCK_SIZE - ((FLS_DATA_SLOT_MGMT_SIZE
                                                    + (uint32_t)sizeof(key_counter_t)
                                                    + (uint32_t)sizeof(rsa_flags_t)
                                                    + (uint32_t)(sizeof(rsa_flags_t) % sizeof(uint32_t))
                                                    + (uint32_t)sizeof(uint32_t)
                                                    + (uint32_t)sizeof(uint32_t)
                                                    + DVC_T_SIZE
                                                    ) % FLS_DF_BLOCK_SIZE)];
} rsa_pub_ext_key_slot_t;
/**********************************************************************************
* Local constants
**********************************************************************************/

static const uint16_t  rsa_rom_key_mod_size[NUM2] = {NUM1024,     NUM2048};
static const uint8_t  *rsa_rom_key_mod_addr[NUM2] = {N_0_1024, N_1_2048};

/**********************************************************************************
* Global variables
***********************************************************************************/
/*===========================================================================
  RAM copy of some of some of the RSA keys stored in Flash
  ===========================================================================*/
#pragma alignvar(4)
static rsa_key_t g_copy_rsa_keys[NB_RAM_COPIES_RSA_KEYS];

/*===========================================================================
  Buffer for key values and properties
  ===========================================================================*/
#pragma alignvar(4)
static rsa_key_prop_t   g_buffer_key_properties_rsa[NB_RAM_COPIES_RSA_KEYS];

/* note: the public exponent is limited in size to 16 bytes = 128 bits */
/*       thus it is possible to use a SMALLBIGNUM_LONG_SIZE vs. BIGNUM_LONG_SIZE */
static uint32_t         g_buffer_priv_exponent[NB_RAM_COPIES_RSA_KEYS][BIGNUM_LONG_SIZE];
static uint32_t         g_buffer_pub_exponent[NB_RAM_COPIES_RSA_KEYS][SMALLBIGNUM_LONG_SIZE];
static uint32_t         g_buffer_modulus[NB_RAM_COPIES_RSA_KEYS][BIGNUM_LONG_SIZE];

/*===========================================================================
  Write RSA key data storage area
  ===========================================================================*/
#pragma alignvar(4)
static rsa_priv_key_slot_t      g_storage_keys_rsa;
#pragma alignvar(4)
static rsa_pub_ext_key_slot_t   g_storage_keys_rsa_ext;


/*===========================================================================
  Index in table g_copy_rsa_keys[]
  ===========================================================================*/
static uint8_t g_current_idx_rsa;

/*===========================================================================
  Used state management counter of the rsa key in the cache
  ===========================================================================*/
static uint8_t g_in_use_conter_rsa[NB_RAM_COPIES_RSA_KEYS];

static uint8_t g_demo_rsa_modulus[MODULUS_SIZE_IN_BYTES] =
{
    0xb1u, 0x0cu, 0xfcu, 0x21u, 0x20u, 0x4eu, 0x38u, 0x74u, 0xb9u, 0x44u, 0x54u, 0xb2u, 0xc1u, 0x4cu, 0xf1u, 0x0fu,
    0x64u, 0x1du, 0x91u, 0xefu, 0xcfu, 0x5eu, 0x8bu, 0xb6u, 0x55u, 0x51u, 0xc9u, 0x04u, 0x1cu, 0xd6u, 0x43u, 0xf5u,
    0x8du, 0xffu, 0x87u, 0xa5u, 0x6bu, 0x67u, 0xcdu, 0x36u, 0x26u, 0xe8u, 0x71u, 0x7bu, 0xfeu, 0x19u, 0xbeu, 0x93u,
    0x64u, 0x60u, 0xb5u, 0x75u, 0xf3u, 0x64u, 0xbdu, 0x4bu, 0x39u, 0xf7u, 0xfau, 0x45u, 0x8bu, 0x14u, 0xabu, 0x71u,
    0xdcu, 0x97u, 0x6bu, 0x1cu, 0xc9u, 0x1bu, 0x7fu, 0xd1u, 0x97u, 0x3au, 0x0fu, 0x9cu, 0x20u, 0xceu, 0x33u, 0x7bu,
    0x6fu, 0x04u, 0x33u, 0x85u, 0x2fu, 0xfbu, 0x8du, 0x0au, 0x77u, 0xcfu, 0x8au, 0x6du, 0xeau, 0x71u, 0x89u, 0x08u,
    0x13u, 0x50u, 0x13u, 0x05u, 0x2bu, 0xa7u, 0xa7u, 0xe8u, 0x8bu, 0x5bu, 0x6cu, 0x60u, 0xf5u, 0xc9u, 0x0fu, 0x5eu,
    0xbdu, 0xeeu, 0xacu, 0x98u, 0x5cu, 0xb2u, 0xedu, 0x2eu, 0xccu, 0xc8u, 0xfdu, 0xc5u, 0x2fu, 0x25u, 0x12u, 0x68u,
    0xb5u, 0x15u, 0x8bu, 0x98u, 0x02u, 0x41u, 0x74u, 0xaau, 0x86u, 0xe7u, 0x47u, 0x8du, 0x75u, 0x47u, 0xbeu, 0x1fu,
    0xc8u, 0xa8u, 0xdeu, 0xfau, 0x91u, 0x77u, 0x55u, 0xadu, 0x31u, 0x2eu, 0x30u, 0x5cu, 0x2au, 0x8du, 0xd7u, 0x6eu,
    0x0du, 0x0du, 0x81u, 0xf1u, 0x04u, 0x3au, 0xf4u, 0xe6u, 0x6bu, 0x09u, 0x7bu, 0xbfu, 0xa8u, 0xd0u, 0xf8u, 0x64u,
    0x42u, 0x8au, 0x72u, 0x0eu, 0x62u, 0xbdu, 0x87u, 0xa9u, 0xa1u, 0xebu, 0x18u, 0xeeu, 0xf0u, 0x84u, 0x99u, 0x37u,
    0xc8u, 0xafu, 0x5eu, 0xbcu, 0xc4u, 0x52u, 0x30u, 0x18u, 0x4au, 0x7du, 0x9cu, 0xfcu, 0xd1u, 0xfau, 0x13u, 0xb7u,
    0x88u, 0x6fu, 0x6du, 0xf1u, 0x3eu, 0x7bu, 0x75u, 0x58u, 0x45u, 0x16u, 0xa8u, 0xd1u, 0x75u, 0x8au, 0xe6u, 0x1bu,
    0x53u, 0x4bu, 0x6eu, 0x37u, 0xb1u, 0x90u, 0x28u, 0x56u, 0xb5u, 0x32u, 0x52u, 0x23u, 0xd3u, 0xa6u, 0xffu, 0x00u,
    0xfau, 0xbbu, 0xcdu, 0xf3u, 0xfbu, 0x97u, 0xd0u, 0xa6u, 0xa9u, 0x5du, 0xa3u, 0x48u, 0xeeu, 0x6cu, 0x46u, 0x35u
};
static uint8_t g_demo_rsa_private_exponent[PRIV_EXP_SIZE_IN_BYTES] =
{
    0x18u, 0x4du, 0x93u, 0xfeu, 0x68u, 0x14u, 0xd4u, 0xa3u, 0x11u, 0x1bu, 0x54u, 0x15u, 0x05u, 0x7bu, 0xaeu, 0x4bu,
    0x52u, 0x8eu, 0xccu, 0x8du, 0x82u, 0x85u, 0x93u, 0xdfu, 0xffu, 0x6eu, 0x2eu, 0x8du, 0xa5u, 0xf7u, 0xb3u, 0x74u,
    0x6du, 0x7du, 0x79u, 0xfdu, 0x36u, 0xf5u, 0x14u, 0xb2u, 0x2eu, 0x3du, 0x11u, 0xf6u, 0xd7u, 0x36u, 0x69u, 0x76u,
    0x89u, 0xfcu, 0xccu, 0x6bu, 0xd8u, 0x71u, 0x27u, 0x9au, 0xbfu, 0xe6u, 0x4eu, 0x30u, 0x65u, 0x9au, 0xfcu, 0xbdu,
    0xb0u, 0x6eu, 0x08u, 0x81u, 0xf7u, 0x09u, 0x13u, 0x4eu, 0x8du, 0xd7u, 0xa6u, 0x74u, 0xe3u, 0xfbu, 0xeau, 0x0du,
    0x33u, 0x7bu, 0xfcu, 0x26u, 0x78u, 0x23u, 0xabu, 0x84u, 0x98u, 0x55u, 0xa0u, 0xc6u, 0xddu, 0xbbu, 0x9fu, 0xf7u,
    0xbfu, 0xceu, 0xeau, 0x0du, 0x77u, 0xa4u, 0x97u, 0xe3u, 0xcfu, 0xddu, 0x48u, 0xcfu, 0x36u, 0x6du, 0xd6u, 0xf2u,
    0x46u, 0x58u, 0x10u, 0xd1u, 0x09u, 0x48u, 0x76u, 0x4eu, 0x4au, 0x45u, 0xfcu, 0xafu, 0xa8u, 0x7du, 0x0au, 0x3du,
    0xa2u, 0xbdu, 0x38u, 0x90u, 0xc8u, 0x94u, 0xc6u, 0x82u, 0x6eu, 0xf8u, 0x31u, 0x0du, 0x81u, 0x2cu, 0x97u, 0xcbu,
    0x10u, 0xb2u, 0xd6u, 0x60u, 0xeau, 0x38u, 0xdeu, 0x3bu, 0x08u, 0x21u, 0x3eu, 0x5fu, 0x7au, 0x5au, 0x5au, 0x07u,
    0xeau, 0x52u, 0x8au, 0x83u, 0xe7u, 0xecu, 0x37u, 0xabu, 0x92u, 0x09u, 0x67u, 0x23u, 0x35u, 0x14u, 0xc6u, 0x03u,
    0x0fu, 0xffu, 0x67u, 0x27u, 0xc7u, 0x87u, 0x37u, 0xbfu, 0x55u, 0xcdu, 0xedu, 0xdeu, 0x11u, 0x83u, 0x9cu, 0x50u,
    0x67u, 0xeau, 0xdbu, 0x51u, 0x0eu, 0x04u, 0x81u, 0x65u, 0xeeu, 0x12u, 0x9au, 0xb9u, 0x97u, 0x16u, 0x65u, 0xf7u,
    0xddu, 0x46u, 0x94u, 0x93u, 0x4bu, 0x22u, 0xb3u, 0x6bu, 0xa8u, 0x7du, 0x0du, 0x81u, 0xdfu, 0x95u, 0x2fu, 0xecu,
    0xb5u, 0xccu, 0x85u, 0x6bu, 0xa9u, 0x4eu, 0x62u, 0xd6u, 0x57u, 0x5fu, 0x13u, 0xbdu, 0x13u, 0x01u, 0x81u, 0x93u,
    0x13u, 0x92u, 0x66u, 0x61u, 0x9cu, 0x1fu, 0xbfu, 0x00u, 0xcau, 0xe3u, 0xe0u, 0x83u, 0x80u, 0xa7u, 0x92u, 0xb1u
};
static uint8_t g_demo_rsa_public_exponent[PUB_EXP_SIZE_IN_BYTES] = {0x01u, 0x00u, 0x01u};

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0003]:[ICUMFW_UD_MH_KeyMNG_0003]
* Function name: mh_KEY_InitRsa
* Description  : Initialization of the RSA key slots in the ICU-M
* Arguments    : void
* Return Value : ER_OK
***************************************************************************************/
uint32_t mh_KEY_InitRsa(void)
{
    uint8_t key_number;
    uint32_t i;

    /* init copy of RSA keys */
    for (i = 0u; i < NB_RAM_COPIES_RSA_KEYS; i++)
    {
        /* init pointers to buffers */
        g_copy_rsa_keys[i].p_properties    = &g_buffer_key_properties_rsa[i];
        g_copy_rsa_keys[i].p_priv_exponent = (uint8_t *)g_buffer_priv_exponent[i];
        g_copy_rsa_keys[i].p_pub_exponent  = (uint8_t *)g_buffer_pub_exponent[i];
        g_copy_rsa_keys[i].p_modulus       = (uint8_t *)g_buffer_modulus[i];
        /* init availability */
        g_copy_rsa_keys[i].availability    = E_NOT_AVAILABLE;
    }

    if (0u == NB_RAM_COPIES_RSA_KEYS)
    {
        return ER_MH_KEY_INVALID_KEY_INDEX;
    }

    g_copy_rsa_keys[0].key_group = KEY_GRP_RSA;
    g_copy_rsa_keys[0].key_id = 0u;
    g_copy_rsa_keys[0].vm_id = 0u;
    g_copy_rsa_keys[0].p_properties->modulus_size_in_bits = 2048u;
    g_copy_rsa_keys[0].p_properties->modulus_size_in_bytes = MODULUS_SIZE_IN_BYTES;
    g_copy_rsa_keys[0].p_properties->priv_exp_size_in_bytes = PRIV_EXP_SIZE_IN_BYTES;
    g_copy_rsa_keys[0].p_properties->pub_exp_size_in_bytes = (uint16_t)sizeof(g_demo_rsa_public_exponent);
    g_copy_rsa_keys[0].p_properties->flags.write_protected = 1u;
    g_copy_rsa_keys[0].p_modulus = g_demo_rsa_modulus;
    g_copy_rsa_keys[0].p_priv_exponent = g_demo_rsa_private_exponent;
    g_copy_rsa_keys[0].p_pub_exponent = g_demo_rsa_public_exponent;
    g_copy_rsa_keys[0].availability = E_AVAILABLE;
    
    /* index on tables g_copy_rsa_keys[] */
    g_current_idx_rsa = NB_RAM_COPIES_RSA_KEYS - 1u;

    /* declare the data set (Flash mapping) */
    (void)MM_FLS_DeclareDataSet(FLS_DS_RSA_ROM_PUB,     MH_KEY_NB_RSA_ROM_PUB_KEYS,
                                   MH_KEY_RSA_ROM_PUB_DS_NB_BLOCKS, FLS_DS_NB_COPIES);

    (void)mh_KEY_MapSize(KEY_MAPPING_RSA_PRIV, &key_number);
    (void)MM_FLS_DeclareDataSet(FLS_DS_RSA_PRIV_KEY,    key_number,
                                MH_KEY_RSA_PRIV_DS_NB_BLOCKS, FLS_DS_NB_COPIES);

    (void)mh_KEY_MapSize(KEY_MAPPING_RSA_PUB, &key_number);
    (void)MM_FLS_DeclareDataSet(FLS_DS_RSA_PUB_KEY,     key_number,
                                MH_KEY_RSA_PUB_DS_NB_BLOCKS, FLS_DS_NB_COPIES);

    (void)mh_KEY_MapSize(KEY_MAPPING_RSA_PUB_EXT, &key_number);
    (void)MM_FLS_DeclareDataSet(FLS_DS_RSA_PUB_EXT_KEY, key_number,
                                   MH_KEY_RSA_PUB_EXT_NB_BLOCKS, FLS_DS_NB_COPIES);

    return ER_OK;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0020]:[ICUMFW_UD_MH_KeyMNG_0020]
* Function name: mh_KEY_GetRsa
* Description  : Retrieve the p_key to the RSA key value & properties for a given key idx
*              : Returns empty if the key is not available (i.e. not initialized in Flash)
* Arguments    : p_job
*              : key_group
*              : key_id
*              : p_key
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_SLOT_EMPTY
*              : ER_MH_KEY_INVALID_KEY_GROUP
*              : ER_MH_KEY_INVALID_KEY_INDEX
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
***************************************************************************************/
static uint32_t mh_KEY_GetRsa(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id, rsa_key_t **p_key)
{
#pragma alignvar(4)
    rsa_priv_key_slot_t key_data;
    uint8_t             data_set_type;
    uint16_t  block_size;
    int32_t i;
    uint8_t vm_id;
    uint8_t slot_num;
    uint8_t key_number;
    uint32_t err = ER_OK;
    
    *p_key = NULL;
    vm_id = p_job->vm_id;
    slot_num = 0u;

    /* correspond Cert-C */
    L_Toolbox_MemSet((uint8_t *)&key_data.pub_exponent, NUM0, (int16_t)sizeof(key_data.pub_exponent));
    key_data.properties.pub_exp_size_in_bytes  = (uint16_t)0;
    L_Toolbox_MemSet((uint8_t *)&key_data.modulus, NUM0, (int16_t)sizeof(key_data.modulus));
    key_data.properties.priv_exp_size_in_bytes = (uint16_t)0;
    key_data.properties.modulus_size_in_bytes  = (uint16_t)0;
    data_set_type                              = (uint8_t)0;

    L_Toolbox_MemSet((uint8_t *)&key_data, NUM0, (int16_t)sizeof(rsa_priv_key_slot_t));

    /* scan the table g_copy_rsa_keys[] (it may be there!) */
    for (i = (int32_t)NB_RAM_COPIES_RSA_KEYS - 1; i >= 0; i--)
    {
        if (g_copy_rsa_keys[i].availability == E_AVAILABLE)
        {
            if ((g_copy_rsa_keys[i].key_group == key_group) &&
                ((uint32_t)g_copy_rsa_keys[i].vm_id == (uint32_t)vm_id) &&
                ((uint32_t)g_copy_rsa_keys[i].key_id == (uint32_t)key_id))
            {
                if ((uint32_t)g_in_use_conter_rsa[i] < NB_IN_USE_COUNTER_MAX)
                {
                    /* update the in use counter */
                    g_in_use_conter_rsa[i]++;
                    /* found: stop here */
                    *p_key = &g_copy_rsa_keys[i];
                }
                else
                {
                    err = ER_MH_KEY_USER_OVER;
                }
                goto end;
            }
            else
            {
                if (0u != (uint32_t)g_in_use_conter_rsa[i])
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
    switch (key_group)
    {
        case KEY_GRP_RSA:
            (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_RSA_PRIV, &key_number);
            if ((uint32_t)key_id >= (uint32_t)key_number)
            {
                /* invalid key index: stop here */
                err = ER_MH_KEY_INVALID_KEY_INDEX;
            }
            else
            {
                data_set_type  = FLS_DS_RSA_PRIV_KEY;
                block_size     = (uint16_t)MH_KEY_RSA_PRIV_DS_NB_BLOCKS;
                slot_num       = (uint8_t)((uint32_t)ML_CFG_VM_RSA_PRI_IDX_OFFSET(vm_id) + (uint32_t)key_id);
            }
            break;
        case KEY_GRP_RSA_PUB:
            (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_RSA_PUB, &key_number);
            if ((uint32_t)key_id >= (uint32_t)key_number)
            {
                /* invalid key index: stop here */
                err = ER_MH_KEY_INVALID_KEY_INDEX;
            }
            else
            {
                data_set_type  = FLS_DS_RSA_PUB_KEY;
                block_size     = (uint16_t)MH_KEY_RSA_PUB_DS_NB_BLOCKS;
                slot_num       = (uint8_t)((uint32_t)ML_CFG_VM_RSA_PUB_IDX_OFFSET(vm_id) + (uint32_t)key_id);
            }
            break;
        case KEY_GRP_RSA_ROM_PUB:
            if ((uint32_t)key_id >= MH_KEY_NB_RSA_ROM_PUB_KEYS)
            {
                /* invalid key index: stop here */
                err = ER_MH_KEY_INVALID_KEY_INDEX;
            }
            else
            {
                slot_num       = (uint8_t)key_id;
                data_set_type  = FLS_DS_RSA_ROM_PUB;
                block_size     = (uint16_t)MH_KEY_RSA_ROM_PUB_DS_NB_BLOCKS;
            }
            break;
        default:
            err = ER_MH_KEY_INVALID_KEY_GROUP;
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

    /* allocate some space for the key value */
    *p_key = &g_copy_rsa_keys[g_current_idx_rsa];
    (*p_key)->availability = E_NOT_AVAILABLE;
    
    /* read the RSA key properties (incl. pub exp)*/
    err = MM_FLS_Read(FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE,
                         data_set_type, slot_num, (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                         (uint8_t*)&key_data);
    if (err == ER_OK)
    {
        
        if (key_group == KEY_GRP_RSA_ROM_PUB)
        {
            /* copy the modulus value from ROM */
            L_Toolbox_MemSet((uint8_t *)&key_data.modulus, 0x00u, (int16_t)MODULUS_SIZE_IN_BYTES);
            L_Toolbox_MemCpy((uint8_t *)&key_data.modulus, rsa_rom_key_mod_addr[key_id],
                                ((int16_t)((int32_t)rsa_rom_key_mod_size[key_id] / NUM8)));
        }
        else
        {
            /* do nothing */
        }

        /* copy the key properties */
        L_Toolbox_MemCpy((uint8_t *)(*p_key)->p_properties, (const uint8_t *)&key_data.properties,
                            (int16_t)sizeof(rsa_key_prop_t));

        /* copy public exponent */
        L_Toolbox_MemSet((*p_key)->p_pub_exponent, 0x00u, (int16_t)PUB_EXP_SIZE_IN_BYTES);
        err = L_Toolbox_MemCpy_Sec((*p_key)->p_pub_exponent, key_data.pub_exponent,
                                    (int16_t)key_data.properties.pub_exp_size_in_bytes);

        if (ER_OK == err)
        {
            /* copy modulus */
            L_Toolbox_MemSet((*p_key)->p_modulus, 0x00u, (int16_t)MODULUS_SIZE_IN_BYTES);
            err = L_Toolbox_MemCpy_Sec((*p_key)->p_modulus, key_data.modulus,
                                        (int16_t)key_data.properties.modulus_size_in_bytes);
        }
        else
        {
            /* Do nothing */
        }

        if ((ER_OK == err) && (0u != (uint32_t)key_data.properties.priv_exp_size_in_bytes))
        {
            /* copy private exponent */
            L_Toolbox_MemSet((*p_key)->p_priv_exponent, 0x00u, (int16_t)PRIV_EXP_SIZE_IN_BYTES);
            err = L_Toolbox_MemCpy_Sec((*p_key)->p_priv_exponent, key_data.priv_exponent,
                                        (int16_t)key_data.properties.priv_exp_size_in_bytes);
        }
        else
        {
            /* do nothing */
        }
        
        if (err == ER_OK)
        {
            /* update the in use counter */
            g_in_use_conter_rsa[g_current_idx_rsa]++;
            
            /* copy the key data in the local table g_copy_rsa_keys[] */
            (*p_key)->availability = E_AVAILABLE;
            (*p_key)->key_group    = key_group;
            (*p_key)->vm_id        = vm_id;
            (*p_key)->key_id       = key_id;
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
end:
    if (err != ER_OK)
    {
        err = mh_KEY_ConvertErr(err);
        *p_key = NULL;
    }
    else
    {
        /* do nothing */
    }
    return( err );
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0022]:[ICUMFW_UD_MH_KeyMNG_0022]
* Function name: mh_KEY_ExtractRsaPubKey
* Description  : Extract public key from X509 certificate
* Arguments    : x509_cert_address
*              : x509_cert_size_in_bytes
*              : p_modulus
*              : p_pub_exp
*              : modulus_size_in_bytes
*              : pub_exp_size_in_bytes
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_MH_KEY_INVALID_KEY_GROUP
***************************************************************************************/
static uint32_t mh_KEY_ExtractRsaPubKey(uint32_t         x509_cert_address, 
                                        uint32_t         x509_cert_size_in_bytes,
                                        uint8_t         *p_modulus,
                                        uint8_t         *p_pub_exp,
                                        uint16_t        *p_modulus_size_in_bytes,
                                        uint16_t        *p_pub_exp_size_in_bytes)
{
    uint32_t err;
    uint32_t byte_size;
    x509_cert_prm_t x509_cert_prm;
    
    uint8_t         signature[R_CRYPT_BIGNUM_BYTESIZE];
    uint8_t         modulus[R_CRYPT_BIGNUM_BYTESIZE];
    uint8_t         pub_exp[NUM16];
    pkc_data_t          x509_modulus;
    pkc_small_data_t    x509_pub_exp;

    x509_cert_prm.p_cert = (const uint8_t *)x509_cert_address;
    x509_cert_prm.cert_size_in_bytes = (uint16_t)x509_cert_size_in_bytes;
    
    x509_cert_prm.p_signature                   = &signature[0];
    x509_cert_prm.sign_size_in_bytes            = sizeof(signature);
    x509_cert_prm.key.rsa.p_modulus             = &modulus[0];
    x509_cert_prm.key.rsa.modulus_size_in_bytes = sizeof(modulus);
    x509_cert_prm.key.rsa.p_pub_exp             = &pub_exp[0];
    x509_cert_prm.key.rsa.pub_exp_size_in_bytes = sizeof(pub_exp);

    err = L_X509_ParseCertificate(&x509_cert_prm);
    if (err != ER_OK)
    {
        err = ER_PARAM;
    }
    else if (x509_cert_prm.key_type != E_KEY_TYPE_RSA_PUB_KEY)
    {
        err = ER_MH_KEY_INVALID_KEY_GROUP;
    }
    else
    {
        byte_size = x509_cert_prm.key.rsa.modulus_size_in_bytes;
        (void)L_Toolbox_MemSet((uint8_t *)&x509_modulus, NUM0, (int16_t)sizeof(x509_modulus));
        err = L_BIGNUM_EncodeToBigNum(x509_cert_prm.key.rsa.p_modulus, E_BINARY_PLAIN, byte_size, &x509_modulus);
        if (err != ER_OK)
        {
            err = ER_PARAM;
        }
        else
        {
            (void)L_Toolbox_MemCpy(p_modulus, (const uint8_t *)x509_modulus.value, (int16_t)x509_modulus.size);
        }
    }
    if (ER_OK == err)
    {
        byte_size = x509_cert_prm.key.rsa.pub_exp_size_in_bytes;
        (void)L_Toolbox_MemSet((uint8_t *)&x509_pub_exp, NUM0, (int16_t)sizeof(x509_pub_exp));
        err = L_BIGNUM_EncodeToSmallBigNum(x509_cert_prm.key.rsa.p_pub_exp, E_BINARY_PLAIN, byte_size, &x509_pub_exp);
        if (err != ER_OK)
        {
            err = ER_PARAM;
        }
        else
        {
            (void)L_Toolbox_MemCpy(p_pub_exp, (const uint8_t *)x509_pub_exp.value, (int16_t)x509_pub_exp.size);
            *p_modulus_size_in_bytes = (uint16_t)x509_modulus.size;
            *p_pub_exp_size_in_bytes = (uint16_t)x509_pub_exp.size;
        }
    }
    else
    {
        /* do nothing */
    }
   
    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0021]:[ICUMFW_UD_MH_KeyMNG_0021]
* Function name: mh_KEY_GetRsaPubExt
* Description  : Retrieve the p_key to the RSA Pub Ext key value & properties for a given key idx
* Arguments    : p_job
*              : key_group
*              : key_id
*              : p_key
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*                ER_IN_PROCESS
*              : ER_MH_KEY_USER_OVER
*              : ER_MH_KEY_INVALID_KEY_INDEX
*              : ER_MH_KEY_INVALID_KEY_GROUP
*              : ER_MH_KEY_SYS_ERROR
*              : ER_MH_KEY_NVM_SYS
*              : ER_MH_KEY_NVM_SYS_CRITICAL
*              : ER_MH_KEY_NVM_UNAVAILABLE
*              : ER_MH_KEY_SRC_MEM_ERROR
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
*              : ER_MH_KEY_DATA_INVALID
***************************************************************************************/
static uint32_t mh_KEY_GetRsaPubExt(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id, rsa_key_t **p_key)
{
#pragma alignvar(4)
    rsa_pub_ext_key_slot_t  key_data;
    uint8_t             vm_id;
    uint8_t             slot_num;
    uint8_t             data_set_type;
    uint16_t            block_size;
    uint16_t            modulus_size_in_bytes;
    uint16_t            pub_exp_size_in_bytes;
    dvc_calculate_t     dvc_param;
    dvc_t               calculated_dvc;
    uint32_t            resource_id;
    int32_t             i;
    uint32_t            err = ER_OK;
    uint8_t            key_number;
    uint8_t             cmp_result;

    *p_key = NULL;

    /* correspond Cert-C */
    vm_id = p_job->vm_id;
    slot_num = 0u;
    key_data.x509_cert_address       = (uint32_t)              0;
    key_data.x509_cert_size_in_bytes = (uint32_t)              0;
    L_Toolbox_MemSet((uint8_t *)&key_data.x509_dvc, NUM0, (int16_t)sizeof(key_data.x509_dvc));
    key_data.flags.write_protected   = (key_flags_t)           0;
    key_data.flags.disable_on_debug  = (key_flags_t)           0;
    key_data.flags.invalidate        = (key_flags_t)           0;

    L_Toolbox_MemSet((uint8_t *)&key_data, NUM0, (int16_t)sizeof(rsa_pub_ext_key_slot_t));

    /* scan the table g_copy_rsa_keys[] (it may be there!) */
    for (i = (int32_t)NB_RAM_COPIES_RSA_KEYS - 1; i >= 0; i--)
    {
        if (g_copy_rsa_keys[i].availability == E_AVAILABLE)
        {
            if ((g_copy_rsa_keys[i].key_group == key_group) &&
                ((uint32_t)g_copy_rsa_keys[i].vm_id == (uint32_t)vm_id) &&
                ((uint32_t)g_copy_rsa_keys[i].key_id == (uint32_t)key_id))
            {
                if ((uint32_t)g_in_use_conter_rsa[i] < NB_IN_USE_COUNTER_MAX)
                {
                    /* update the in use counter */
                    g_in_use_conter_rsa[i]++;
                    /* found: stop here */
                    *p_key = &g_copy_rsa_keys[i];
                }
                else
                {
                    err = ER_MH_KEY_USER_OVER;
                }
                goto end;
            }
            else
            {
                if (0u != (uint32_t)g_in_use_conter_rsa[i])
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
    (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_RSA_PUB_EXT, &key_number);
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
    slot_num       = (uint8_t)((uint32_t)ML_CFG_VM_RSA_PUB_EXT_IDX_OFFSET(vm_id) + (uint32_t)key_id);
    data_set_type  = FLS_DS_RSA_PUB_EXT_KEY;
    block_size     = MH_KEY_RSA_PUB_EXT_NB_BLOCKS;
    
    /* allocate some space for the key value */
    *p_key = &g_copy_rsa_keys[g_current_idx_rsa];
    (*p_key)->availability = E_NOT_AVAILABLE;
    
    /* read the RSA key properties (incl. pub exp) */
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
        err = mh_KEY_ConvertErr(err);
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
    err = ML_DVC_Calculate( &dvc_param, ML_DVC_CALC_TYPE_CMAC );
    
    /* release AES resource(Do not check for errors) */
    (void)ML_ABTR_FreeResource( ABTR_RESOURCE_TYPE_AES, resource_id );
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
    cmp_result = L_Toolbox_MemCmp((const uint8_t *)calculated_dvc.dvc, (const uint8_t *)key_data.x509_dvc, (int16_t)DVC_T_SIZE);
    if (0u != (uint32_t)cmp_result)
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
    
    /* RSA public key extraction from X.509 certificate */
    err = mh_KEY_ExtractRsaPubKey(key_data.x509_cert_address,
                                  key_data.x509_cert_size_in_bytes,
                                  (*p_key)->p_modulus,
                                  (*p_key)->p_pub_exponent,
                                  &modulus_size_in_bytes,
                                  &pub_exp_size_in_bytes);
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
    (*p_key)->p_properties->modulus_size_in_bytes   = modulus_size_in_bytes;
    (*p_key)->p_properties->modulus_size_in_bits    = (uint32_t)modulus_size_in_bytes * NUM8;
    (*p_key)->p_properties->priv_exp_size_in_bytes  = 0u;
    (*p_key)->p_properties->pub_exp_size_in_bytes   = pub_exp_size_in_bytes;
    (*p_key)->p_properties->flags.write_protected   = key_data.flags.write_protected;
    (*p_key)->p_properties->flags.disable_on_debug  = key_data.flags.disable_on_debug;
    (*p_key)->p_properties->flags.invalidate        = key_data.flags.invalidate;

    /* update the in use counter */
    g_in_use_conter_rsa[g_current_idx_rsa]++;

    /* copy the key data in the local table g_copy_rsa_keys[] */
    (*p_key)->availability = E_AVAILABLE;
    (*p_key)->key_group    = key_group;
    (*p_key)->vm_id        = vm_id;
    (*p_key)->key_id       = key_id;

end:
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0019]:[ICUMFW_UD_MH_KeyMNG_0019]
* Function name: MH_KEY_GetRsaKey
* Description  : Retrieve the pointer to the RSA key value & properties for a given key idx
* Arguments    : p_job
*              : key_group
*              : key_id
*              : p_key
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_SLOT_EMPTY
*              : ER_MH_KEY_INVALID_KEY_GROUP
*              : ER_MH_KEY_INVALID_KEY_INDEX
*              : ER_MH_KEY_IN_USE
*              : ER_MH_KEY_USER_OVER
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
uint32_t MH_KEY_GetRsaKey(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id, rsa_key_t **p_key)
{
    uint16_t write_sts;
    uint32_t err;

    /* Initialize local variable */
    err = ER_OK;
    write_sts = KEY_NOT_WRITING;

    (void)mh_KEY_WritingJudgment(p_job, key_group, key_id, &write_sts);
    if ((uint32_t)KEY_WRITING == (uint32_t)write_sts)
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
            case KEY_GRP_RSA:
            case KEY_GRP_RSA_PUB:
            case KEY_GRP_RSA_ROM_PUB:
                err = mh_KEY_GetRsa(p_job, key_group, key_id, p_key);
                break;
            case KEY_GRP_RSA_PUB_EXT:
                err = mh_KEY_GetRsaPubExt(p_job, key_group, key_id, p_key);
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

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0023]:[ICUMFW_UD_MH_KeyMNG_0023]
* Function name: MH_KEY_SetRsaKey
* Description  : Update RSA key data to Data Flash
* Arguments    : p_job
*              : operation
*              : p_new_key
*              : lc_stage
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_WRITING
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_WRITE_PROTECTED
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_AES_PRM_ERROR
***************************************************************************************/
uint32_t MH_KEY_SetRsaRam(icum_job_t *p_job, rsa_key_t *p_new_key)
{
    uint32_t err;
    uint32_t modulus_size;
    uint32_t private_exponent_size;
    uint32_t public_exponent_size;

    if ((NULL == p_job) || (NULL == p_new_key) ||
        (NULL == p_new_key->p_properties) || (NULL == p_new_key->p_modulus) ||
        (NULL == p_new_key->p_priv_exponent) || (NULL == p_new_key->p_pub_exponent))
    {
        return ER_PARAM;
    }

    if ((KEY_GRP_RSA != p_new_key->key_group) || (0u != p_new_key->key_id) ||
        (0u != p_job->vm_id) || (0u == NB_RAM_COPIES_RSA_KEYS))
    {
        return ER_MH_KEY_INVALID_KEY_INDEX;
    }

    if (0u != g_in_use_conter_rsa[0])
    {
        return ER_MH_KEY_IN_USE;
    }

    modulus_size = p_new_key->p_properties->modulus_size_in_bytes;
    private_exponent_size = p_new_key->p_properties->priv_exp_size_in_bytes;
    public_exponent_size = p_new_key->p_properties->pub_exp_size_in_bytes;
    if ((0u == modulus_size) || (modulus_size > MODULUS_SIZE_IN_BYTES) ||
        (0u == private_exponent_size) || (private_exponent_size > PRIV_EXP_SIZE_IN_BYTES) ||
        (0u == public_exponent_size) || (public_exponent_size > PUB_EXP_SIZE_IN_BYTES))
    {
        return ER_PARAM;
    }

    g_copy_rsa_keys[0].availability = E_NOT_AVAILABLE;
    L_Toolbox_MemSet(g_demo_rsa_modulus, 0u, (int16_t)sizeof(g_demo_rsa_modulus));
    L_Toolbox_MemSet(g_demo_rsa_private_exponent, 0u, (int16_t)sizeof(g_demo_rsa_private_exponent));
    L_Toolbox_MemSet(g_demo_rsa_public_exponent, 0u, (int16_t)sizeof(g_demo_rsa_public_exponent));

    err = L_Toolbox_MemCpy_Sec((uint8_t *)g_copy_rsa_keys[0].p_properties,
                               (const uint8_t *)p_new_key->p_properties,
                               (int16_t)sizeof(rsa_key_prop_t));
    if (ER_OK == err)
    {
        err = L_Toolbox_MemCpy_Sec(g_demo_rsa_modulus, p_new_key->p_modulus, (int16_t)modulus_size);
    }
    if (ER_OK == err)
    {
        err = L_Toolbox_MemCpy_Sec(g_demo_rsa_private_exponent, p_new_key->p_priv_exponent,
                                   (int16_t)private_exponent_size);
    }
    if (ER_OK == err)
    {
        err = L_Toolbox_MemCpy_Sec(g_demo_rsa_public_exponent, p_new_key->p_pub_exponent,
                                   (int16_t)public_exponent_size);
    }
    if (ER_OK == err)
    {
        g_copy_rsa_keys[0].key_group = KEY_GRP_RSA;
        g_copy_rsa_keys[0].key_id = 0u;
        g_copy_rsa_keys[0].vm_id = 0u;
        g_copy_rsa_keys[0].availability = E_AVAILABLE;
    }

    return err;
}

uint32_t MH_KEY_SetRsaKey(icum_job_t *p_job, set_operation_t operation, rsa_key_t *p_new_key)
{
    rsa_key_t          *p_key;
#pragma alignvar(4)
    rsa_priv_key_slot_t key_data;
    uint8_t             data_set_type;
    uint16_t  block_size;
    uint32_t            size_in_bytes;
    uint32_t            size_in_bits;
    int32_t i;
    uint8_t key_number;
    uint16_t write_sts;
    uint32_t err;
    uint8_t vm_id;
    uint8_t slot_num;

    /* Initialize local variable */
    err = ER_OK;
    vm_id = p_job->vm_id;
    slot_num = 0u;
    key_data.properties.flags.write_protected = (key_flags_t)0;
    L_Toolbox_MemSet((uint8_t *)&key_data, NUM0, (int16_t)sizeof(rsa_priv_key_slot_t));

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
    switch (p_new_key->key_group)
    {
        case KEY_GRP_RSA:
            (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_RSA_PRIV, &key_number);
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
            slot_num       = (uint8_t)((uint32_t)ML_CFG_VM_RSA_PRI_IDX_OFFSET(vm_id) + (uint32_t)p_new_key->key_id);
            data_set_type  = FLS_DS_RSA_PRIV_KEY;
            block_size     = (uint16_t)MH_KEY_RSA_PRIV_DS_NB_BLOCKS;
            break;
        case KEY_GRP_RSA_PUB:
            (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_RSA_PUB, &key_number);
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
            slot_num       = (uint8_t)((uint32_t)ML_CFG_VM_RSA_PUB_IDX_OFFSET(vm_id) + (uint32_t)p_new_key->key_id);
            data_set_type  = FLS_DS_RSA_PUB_KEY;
            block_size     = (uint16_t)MH_KEY_RSA_PUB_DS_NB_BLOCKS;
            break;
        default:
            err = ER_MH_KEY_INVALID_KEY_GROUP;
            goto end;
    }

    /* read the RSA key properties (incl. pub exp) */
    if (operation == E_KEY_OPE_FIRST)
    {
        (void)mh_KEY_WritingStsGet( &write_sts );
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
        
        /* scan the table g_copy_rsa_keys[] (it may be there!) */
        for (i = (int32_t)NB_RAM_COPIES_RSA_KEYS - 1; i >= 0; i--)
        {
            if ((g_copy_rsa_keys[i].availability == E_AVAILABLE) &&
                (g_copy_rsa_keys[i].key_group == p_new_key->key_group) &&
                ((uint32_t)g_copy_rsa_keys[i].vm_id == (uint32_t)vm_id) &&
                ((uint32_t)g_copy_rsa_keys[i].key_id == (uint32_t)p_new_key->key_id))
            {
                /* found: stop here */
                if (0u == (uint32_t)g_in_use_conter_rsa[i])
                {
                    /* key is not use */
                    p_key = &g_copy_rsa_keys[i];
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

        err = MM_FLS_Read(FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE,
                             data_set_type, slot_num, (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                             (uint8_t*)&key_data);
        if ((ER_OK == err) &&
            ((uint32_t)MH_KEY_FLAG_WRITE_PROTECT == (uint32_t)key_data.properties.flags.write_protected))
            {
                err = ER_MH_KEY_WRITE_PROTECTED;
                goto end;
            }
        else if ((ER_OK == err) || (ER_MM_FLS_DATA_BLANK == err))
            {
            /* in case of read ok or blank No need to check */
        }
        else
        {
            goto end;
        }
        /* make sure the public exponent fit in the small buffer size */
        if (PUB_EXP_SIZE_IN_BYTES < (uint32_t)p_new_key->p_properties->pub_exp_size_in_bytes)
        {
            /* invalid size */
            err = ER_PARAM;
            goto end;
        }
        else
        {
            /* do nothing */
        }
        
        L_Toolbox_MemSet((uint8_t *)&g_storage_keys_rsa, NUM0, (int16_t)sizeof(rsa_priv_key_slot_t));
        /* copy the public exponent */
        size_in_bytes = p_new_key->p_properties->pub_exp_size_in_bytes;
        L_Toolbox_MemCpy(g_storage_keys_rsa.pub_exponent, p_new_key->p_pub_exponent, (int16_t)size_in_bytes);
        g_storage_keys_rsa.properties.pub_exp_size_in_bytes = (uint16_t)size_in_bytes;

        /* copy the modulus */
        size_in_bits = (uint32_t)p_new_key->p_properties->modulus_size_in_bits;
        size_in_bytes = (size_in_bits + NUM7) / NUM8;
        if ((size_in_bytes > (uint32_t)MODULUS_SIZE_IN_BYTES) || (NUM0 != (size_in_bits % NUM1024)))
        {
            /* invalid size */
            err = ER_PARAM; 
            goto end;
        }
        else
        {
            L_Toolbox_MemCpy(g_storage_keys_rsa.modulus, p_new_key->p_modulus, (int16_t)size_in_bytes);
        }
        g_storage_keys_rsa.properties.modulus_size_in_bytes = (uint16_t)size_in_bytes;
        g_storage_keys_rsa.properties.modulus_size_in_bits  = p_new_key->p_properties->modulus_size_in_bits;

        /* copy the private exponent */
        if (p_new_key->key_group == KEY_GRP_RSA)
        {
            size_in_bytes = p_new_key->p_properties->priv_exp_size_in_bytes;
            if (size_in_bytes > (uint32_t)PRIV_EXP_SIZE_IN_BYTES)
            {
                /* invalid size */
                err = ER_PARAM;
                goto end;
            }
            else
            {
                L_Toolbox_MemCpy(g_storage_keys_rsa.priv_exponent, p_new_key->p_priv_exponent, (int16_t)size_in_bytes);
            }
            /* set the private exponent size */
            g_storage_keys_rsa.properties.priv_exp_size_in_bytes  = (uint16_t)size_in_bytes;
        }
        else
        {
            g_storage_keys_rsa.properties.priv_exp_size_in_bytes  = 0u;
        }
        
        /* set the values in the key property slot */
        /* set the property values */
        g_storage_keys_rsa.properties.update_counter          = 0u;
        g_storage_keys_rsa.properties.flags.write_protected   = p_new_key->p_properties->flags.write_protected; 
        g_storage_keys_rsa.properties.flags.disable_on_debug  = p_new_key->p_properties->flags.disable_on_debug;
        g_storage_keys_rsa.properties.flags.invalidate        = p_new_key->p_properties->flags.invalidate;

        (void)mh_KEY_WritingSet(vm_id, p_new_key->key_group, p_new_key->key_id);

        err = MM_FLS_Write(p_job->job_id, FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, data_set_type, slot_num,
                           (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                              (uint8_t*)&g_storage_keys_rsa);
    }
    else
    {
        err = MM_FLS_Write(p_job->job_id, FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, data_set_type, slot_num,
                           (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                              (uint8_t*)&g_storage_keys_rsa);
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
    err = mh_KEY_ConvertErr( err );
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0032]:[ICUMFW_UD_MH_KeyMNG_0032]
* Function name: MH_KEY_SetRsaPubExt
* Description  : Update RSA Pub Ext key data to Data Flash
* Arguments    : p_job
*              : operation
*              : p_new_key
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_IN_PROCESS
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
*              : ER_MH_KEY_AES_PRM_ERROR
*              : ER_MH_KEY_DATA_INVALID
*              : ER_MH_KEY_INVALID_KEY_INDEX
*              : ER_MH_KEY_IN_USE
*              : ER_MH_KEY_NOT_USABLE
*              : ER_MH_KEY_WRITING
*              : ER_MH_KEY_SYS_ERROR
*              : ER_MH_KEY_NVM_SYS
*              : ER_MH_KEY_NVM_SYS_CRITICAL
*              : ER_MH_KEY_NVM_UNAVAILABLE
*              : ER_MH_KEY_NVM_PROG
*              : ER_MH_KEY_NVM_SIZE
*              : ER_MH_KEY_SRC_MEM_ERROR
*              : ER_MH_KEY_DEST_MEM_ERROR
*              : ER_MH_KEY_SLOT_EMPTY
*              : ER_MH_KEY_WRITE_PROTECTED
***************************************************************************************/
uint32_t MH_KEY_SetRsaPubExt(icum_job_t *p_job, set_operation_t operation, rsa_pub_ext_t *p_new_key)
{
    rsa_key_t          *p_key;
#pragma alignvar(4)
    rsa_pub_ext_key_slot_t  key_data;
    uint8_t vm_id;
    uint8_t slot_num;
    uint8_t             data_set_type;
    uint16_t            block_size;
    dvc_calculate_t     dvc_param;
    dvc_t               calculated_dvc;
    uint32_t            resource_id;
    int32_t             i;
    uint16_t            write_sts;
    uint32_t            err;
    uint8_t key_number;

    vm_id = p_job->vm_id;
    slot_num = 0u;
    key_data.flags.write_protected = (key_flags_t)0;
    L_Toolbox_MemSet((uint8_t *)&key_data, NUM0, (int16_t)sizeof(rsa_pub_ext_key_slot_t));

    /* Checks range of VMID */
    err = mh_KEY_CheckVMID(vm_id);
    if (ER_OK != err)
    {
        /* invalid vm index: stop here */
        goto end;
    }
    else
    {
        /* do nothing */
    }

    /* check key group & index */
    (void)mh_KEY_MapSizeVM(vm_id, KEY_MAPPING_RSA_PUB_EXT, &key_number);
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
    slot_num       = (uint8_t)((uint32_t)ML_CFG_VM_RSA_PUB_EXT_IDX_OFFSET(vm_id) + (uint32_t)p_new_key->key_id);
    data_set_type  = FLS_DS_RSA_PUB_EXT_KEY;
    block_size     = MH_KEY_RSA_PUB_EXT_NB_BLOCKS;
    
    /* read the RSA key properties (incl. pub exp) */
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
        
        /* scan the table g_copy_rsa_keys[] (it may be there!) */
        for (i = (int32_t)NB_RAM_COPIES_RSA_KEYS - 1; i >= 0; i--)
        {
            if ((g_copy_rsa_keys[i].availability == E_AVAILABLE) &&
                (g_copy_rsa_keys[i].key_group == KEY_GRP_RSA_PUB_EXT) &&
                ((uint32_t)g_copy_rsa_keys[i].vm_id == (uint32_t)vm_id) &&
                ((uint32_t)g_copy_rsa_keys[i].key_id == (uint32_t)p_new_key->key_id))
            {
                /* found: stop here */
                if (0u == (uint32_t)g_in_use_conter_rsa[i])
                {
                    /* key is not use */
                    p_key = &g_copy_rsa_keys[i];
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

        err = MM_FLS_Read(FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE,
                             data_set_type, slot_num,
                             (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                             (uint8_t*)&key_data);
        if (err == ER_OK)
        {
            if ((uint32_t)key_data.flags.write_protected == (uint32_t)1u)
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

        L_Toolbox_MemSet((uint8_t *)&g_storage_keys_rsa_ext, NUM0, (int16_t)sizeof(rsa_pub_ext_key_slot_t));
        
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
        
        /* set the values in the key property slot */
        /* set the property values */
        g_storage_keys_rsa_ext.update_counter          = 0u;
        g_storage_keys_rsa_ext.flags.write_protected   = p_new_key->flags.write_protected; 
        g_storage_keys_rsa_ext.flags.disable_on_debug  = p_new_key->flags.disable_on_debug;
        g_storage_keys_rsa_ext.flags.invalidate        = p_new_key->flags.invalidate;
        g_storage_keys_rsa_ext.x509_cert_address       = p_new_key->x509_cert_address;
        g_storage_keys_rsa_ext.x509_cert_size_in_bytes = p_new_key->x509_cert_size_in_bytes;
        
        L_Toolbox_MemCpy(g_storage_keys_rsa_ext.x509_dvc, &calculated_dvc.dvc[0], (int16_t)DVC_T_SIZE);
        
        (void)mh_KEY_WritingSet(vm_id, KEY_GRP_RSA_PUB_EXT, p_new_key->key_id);

        err = MM_FLS_Write(p_job->job_id, FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, data_set_type, slot_num,
                           (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                              (uint8_t*)&g_storage_keys_rsa_ext);
    }
    else
    {
        err = MM_FLS_Write(p_job->job_id, FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, data_set_type, slot_num,
                           (uint16_t)((uint32_t)block_size * (uint32_t)NB_FLASH_BLOCK_NUMBER),
                              (uint8_t*)&g_storage_keys_rsa_ext);
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
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0054]:[ICUMFW_UD_MH_KeyMNG_0054]
* Function name: MH_KEY_InstallRsaRomPub
* Description  : Generate management information of RSA Rom Pub key and store it in Data Flash
*              : Loop in the function until writing to the Data Flash is completed
* Arguments    : void
* Return Value : ER_OK
*              : ER_MH_KEY_WRITING
*              : ER_MH_KEY_NOT_USABLE
*              : ER_MH_KEY_AES_TIMEOUT_ERROR
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_PENDING
*              : ER_MH_KEY_WRITING
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
uint32_t MH_KEY_InstallRsaRomPub(void)
{
    r_icumif_job_id_t       job_id;
    uint32_t i;
    uint16_t write_sts;
    uint32_t err;

    (void)mh_KEY_WritingStsGet(&write_sts);

    if ((uint32_t)write_sts == (uint32_t)KEY_WRITING)
    {
        /* Key writing in progress */
        err = ER_MH_KEY_WRITING;
    }
    else
    {
        job_id = RSA_ROM_JOB_ID;

        /* read the RSA key properties (incl. pub exp) */
        for (i = 0u; i < MH_KEY_NB_RSA_ROM_PUB_KEYS; i++)
        {
            /* initialize the key properties */
            L_Toolbox_MemSet((uint8_t *)&g_storage_keys_rsa, NUM0, (int16_t)sizeof(rsa_priv_key_slot_t));
            g_storage_keys_rsa.properties.update_counter         = 0u;
            g_storage_keys_rsa.properties.modulus_size_in_bits   = rsa_rom_key_mod_size[i];
            g_storage_keys_rsa.properties.modulus_size_in_bytes  = (uint16_t)((uint32_t)rsa_rom_key_mod_size[i] / NUM8);
            g_storage_keys_rsa.properties.priv_exp_size_in_bytes = 0u;
            g_storage_keys_rsa.properties.pub_exp_size_in_bytes  = (uint16_t)sizeof(E);
            g_storage_keys_rsa.properties.flags.write_protected  = 1u;
            g_storage_keys_rsa.properties.flags.disable_on_debug = 0u;
            g_storage_keys_rsa.properties.flags.invalidate       = 0u;

            L_Toolbox_MemSet((uint8_t *)&(g_storage_keys_rsa.pub_exponent), 0x00u, (int16_t)PUB_EXP_SIZE_IN_BYTES);
            L_Toolbox_MemCpy((uint8_t *)&(g_storage_keys_rsa.pub_exponent), E,    (int16_t)sizeof(E));

            do
            {
                err = MM_FLS_Write(job_id, FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE,
                                    FLS_DS_RSA_ROM_PUB, i, MH_KEY_RSA_ROM_PUB_DS_NB_BLOCKS * NB_FLASH_BLOCK_NUMBER,
                                    (uint8_t*)&g_storage_keys_rsa);
            } while ((err == ER_IN_PROCESS) || (err == ER_MM_FLS_IN_PROGRESS));
        }
    }

    err = mh_KEY_ConvertErr( err );
    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0057]:[ICUMFW_UD_MH_KeyMNG_0057]
* Function name: mh_KEY_FreeCacheRsa
* Description  : Clear processing of AES key in-use counter
* Arguments    : p_job
*              : key_group
*              : key_id
* Return Value : ER_OK
*              : ER_PARAM
*              : ER_MH_KEY_DATA_NOT_FOUND
***************************************************************************************/
uint32_t mh_KEY_FreeCacheRsa(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id)
{
    int32_t i;
    uint32_t err = ER_OK;

    /* scan the table copy_aes_keys[] (it may be there!) */
    for (i = (int32_t)NB_RAM_COPIES_RSA_KEYS - 1; i >= 0; i--)
    {
        if ((g_copy_rsa_keys[i].availability == E_AVAILABLE) &&
            (g_copy_rsa_keys[i].key_group == key_group) && 
            ((uint32_t)g_copy_rsa_keys[i].vm_id == (uint32_t)p_job->vm_id) && 
            ((uint32_t)g_copy_rsa_keys[i].key_id == (uint32_t)key_id))
        {
            if ((uint32_t)0u != (uint32_t)g_in_use_conter_rsa[i])
            {
                g_in_use_conter_rsa[i]--;
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
    }
    
    if (i < 0)
    {
        err = ER_MH_KEY_DATA_NOT_FOUND;
    }
    else
    {
        /* do nothing */
    }
    
    return err;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_KeyMNG_0066]:[ICUMFW_UD_MH_KeyMNG_0066]
* Function name: mh_KEY_DeleteRsaCacheKey
* Description  : Delete RSA key in cache
* Arguments    : void
* Return Value : ER_OK
*              : ER_MH_KEY_IN_USE
***************************************************************************************/
uint32_t mh_KEY_DeleteRsaCacheKey(void)
{
    uint32_t i;
    uint32_t err = ER_OK;
    
    for (i = 0u; i < NB_RAM_COPIES_RSA_KEYS; i++)
    {
        if (0U != (uint32_t)g_in_use_conter_rsa[i])
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
        for (i = 0u; i < NB_RAM_COPIES_RSA_KEYS; i++)
        {
            g_copy_rsa_keys[i].availability = E_NOT_AVAILABLE;
            g_copy_rsa_keys[i].key_group = (r_key_group_t)0u;
            g_copy_rsa_keys[i].vm_id = 0u;
            g_copy_rsa_keys[i].key_id = 0u;
            L_Toolbox_MemSet((uint8_t*)&g_buffer_key_properties_rsa[i], NUM0,(int16_t)sizeof(rsa_key_prop_t));
            L_Toolbox_MemSet((uint8_t*)&g_buffer_priv_exponent[i], NUM0, (int16_t)sizeof(g_buffer_priv_exponent));
            L_Toolbox_MemSet((uint8_t*)&g_buffer_pub_exponent[i], NUM0, (int16_t)sizeof(g_buffer_pub_exponent));
            L_Toolbox_MemSet((uint8_t*)&g_buffer_modulus[i], NUM0, (int16_t)sizeof(g_buffer_modulus));
            g_in_use_conter_rsa[i] = 0u;
        }
    }
    else
    {
        /* Do nothing */
    }
    return (err);
}
