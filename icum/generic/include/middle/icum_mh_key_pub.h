/******************************************************************************/
/* Component Name ML Key Manager public header file                           */
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

#if !defined(ICUM_MH_KEY_PUB_H)
#define ICUM_MH_KEY_PUB_H

/**************************************************************************************/
/* Headers                                                                            */
/**************************************************************************************/
#include "r_icumif_api.h"
#include "icum_l_cry_pk_pub.h"
/* For the p_job structure of ISSD. */
#include "icum_k_issd_pub.h"


/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
#define DVC_SIZE_IN_BYTES 16
/* data availability */
#define E_NOT_AVAILABLE                     (0x00000000uL)
#define E_AVAILABLE                         (0xA55A5AA5uL)

/* key MNG result */
#define ER_MH_KEY_SLOT_EMPTY                (ER_MH_KEY_BASE + (0x00000001uL))
#define ER_MH_KEY_INVALID_KEY_GROUP         (ER_MH_KEY_BASE + (0x00000002uL))
#define ER_MH_KEY_INVALID_KEY_INDEX         (ER_MH_KEY_BASE + (0x00000003uL))
#define ER_MH_KEY_IN_USE                    (ER_MH_KEY_BASE + (0x00000004uL))
#define ER_MH_KEY_WRITE_PROTECTED           (ER_MH_KEY_BASE + (0x00000005uL))
#define ER_MH_KEY_DATA_NOT_FOUND            (ER_MH_KEY_BASE + (0x00000006uL))
#define ER_MH_KEY_WRITING                   (ER_MH_KEY_BASE + (0x00000007uL))
#define ER_MH_KEY_USER_OVER                 (ER_MH_KEY_BASE + (0x00000008uL))
#define ER_MH_KEY_NOT_USABLE                (ER_MH_KEY_BASE + (0x00000009uL))
#define ER_MH_KEY_UPDATE_ERROR              (ER_MH_KEY_BASE + (0x0000000auL))
#define ER_MH_KEY_AES_TIMEOUT_ERROR         (ER_MH_KEY_BASE + (0x0000000buL))
#define ER_MH_KEY_DATA_INVALID              (ER_MH_KEY_BASE + (0x0000000cuL))
#define ER_MH_KEY_NVM_UNAVAILABLE           (ER_MH_KEY_BASE + (0x0000000duL))
#define ER_MH_KEY_NVM_SYS_CRITICAL          (ER_MH_KEY_BASE + (0x0000000euL))
#define ER_MH_KEY_NVM_SYS                   (ER_MH_KEY_BASE + (0x0000000fuL))
#define ER_MH_KEY_NVM_PROG                  (ER_MH_KEY_BASE + (0x00000010uL))
#define ER_MH_KEY_NVM_SIZE                  (ER_MH_KEY_BASE + (0x00000011uL))
#define ER_MH_KEY_SRC_MEM_ERROR             (ER_MH_KEY_BASE + (0x00000012uL))
#define ER_MH_KEY_DEST_MEM_ERROR            (ER_MH_KEY_BASE + (0x00000013uL))
#define ER_MH_KEY_SYS_ERROR                 (ER_MH_KEY_BASE + (0x00000014uL))
#define ER_MH_KEY_AES_PRM_ERROR             (ER_MH_KEY_BASE + (0x00000015uL))
#define ER_MH_KEY_INVALID_CURVE_ID          (ER_MH_KEY_BASE + (0x00000016uL))

#define MH_KEY_AES_KEY_SIZE_IN_BYTES_128BIT    (16u)
#define MH_KEY_AES_KEY_SIZE_IN_BYTES_192BIT    (24u)
#define MH_KEY_AES_KEY_SIZE_IN_BYTES_256BIT    (32u)
#define MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON    (32u)

#define MH_KEY_HMAC_KEY_SIZE_IN_BYTES          (64u)
#define MH_KEY_MASTER_SECRET_SIZE_IN_BYTES     (48u)

/* number of SHE extend ids */
#define NB_SHE_EXTEND_IDS_MAX   (8u)
#define NB_SHE_EXTEND_ID_0      (0u)
#define NB_SHE_EXTEND_ID_1      (1u)
#define NB_SHE_EXTEND_ID_2      (2u)
#define NB_SHE_EXTEND_ID_3      (3u)
#define NB_SHE_EXTEND_ID_4      (4u)
#define NB_SHE_EXTEND_ID_5      (5u)
#define NB_SHE_EXTEND_ID_6      (6u)
#define NB_SHE_EXTEND_ID_7      (7u)
#define NB_SHE_EXTEND_ID_8      (8u)

/* number of SHE keys */
#define NB_SHE_KEYS         (15u +(NB_SHE_EXTEND_IDS_MAX*10u))

/* start position of aes buffer key */
#define MH_KEY_AES_BUFFER_KEY_ID_START    (2uL)

/* number of blocks used per data slot */
#define MH_KEY_AES_DS_NB_BLOCKS         (0x0002u)
#define MH_KEY_RSA_ROM_PUB_DS_NB_BLOCKS (0x0001u)
#define MH_KEY_RSA_PRIV_DS_NB_BLOCKS    (0x0011u)
#define MH_KEY_RSA_PUB_DS_NB_BLOCKS     (0x0009u)
#define MH_KEY_RSA_PUB_EXT_NB_BLOCKS    (0x0001u)
#define MH_KEY_ECC_ROM_PUB_DS_NB_BLOCKS (0x0001u)
#define MH_KEY_ECC_PRIV_DS_NB_BLOCKS    (0x0004u)
#define MH_KEY_ECC_PUB_DS_NB_BLOCKS     (0x0004u)
#define MH_KEY_ECC_PUB_EXT_NB_BLOCKS    (0x0001u)

/* number of CR keys */
#define MH_KEY_NB_CR_KEYS                  4u
/* number of RSA ROM public keys (provided by Renesas) */
#define MH_KEY_NB_RSA_ROM_PUB_KEYS         2u

/* Key flag check result (Update SHE) */
#define MH_KEY_FLAG_CHECK_NG                    (uint16_t)(0x0000u)
#define MH_KEY_FLAG_CHECK_OK                    (uint16_t)(0x0001u)

/* define key_usage of key flag */
#define MH_KEY_FLAG_KEY_NOT_UDSABLE             (key_flags_t)(0x00u)
#define MH_KEY_FLAG_KEY_USABLE                  (key_flags_t)(0x01u)

/* define write_protected of key flag */
#define MH_KEY_FLAG_NOT_WRITE_PROTECT           (key_flags_t)(0x00u)
#define MH_KEY_FLAG_WRITE_PROTECT               (key_flags_t)(0x01u)

/* define disable_on_debug of key flag */
#define MH_KEY_FLAG_KEY_USE_POSSIBLE            (key_flags_t)(0x00u)
#define MH_KEY_FLAG_KEY_USE_PROHIBIT            (key_flags_t)(0x01u)

/* define no_wild_card of key flag */
#define MH_KEY_FLAG_WILD_CARD_USABLE            (key_flags_t)(0x00u)
#define MH_KEY_FLAG_WILD_CARD_NOT_USABLE        (key_flags_t)(0x01u)

/* define invalidate of key flag */
#define MH_KEY_FLAG_NOT_INVALID                 (key_flags_t)(0x00u)
#define MH_KEY_FLAG_INVALID                     (key_flags_t)(0x01u)

/* define plain_key of key flag */
#define MH_KEY_FLAG_UPDATE_SHE                  (key_flags_t)(0x00u)
#define MH_KEY_FLAG_UPDATE_PLAIN                (key_flags_t)(0x01u)

/* mh_KEY_FindKeyIdInUseMngTable */
#define MH_KEY_IN_USE_MNG_TABLE_FOUND           ((uint8_t)0x01u)
#define MH_KEY_IN_USE_MNG_TABLE_NOT_FOUND       ((uint8_t)0x00u)

/* Key Size Ext */
#define MH_KEY_SIZE_NOT_EXP                     ((uint8_t)0x00u)
#define MH_KEY_SIZE_EXP                         ((uint8_t)0x01u)

/* definition the size of one item size (config RAM) */
/* 60 byte the size of AES RAM key */
#define MH_KEY_AES_RAM_DATA_SIZE                ((uint32_t)sizeof(aes_key_t))
/* 60 byte the size of AES BUFFER key */
#define MH_KEY_AES_BUFFER_DATA_SIZE             ((uint32_t)sizeof(aes_key_t))
/* 460 byte the size of ECC RAM key */
#define MH_KEY_ECC_RAM_DATA_SIZE                ((uint32_t)sizeof(ecc_key_t) \
                                                    + (uint32_t)sizeof(ecc_key_prop_t) \
                                                    + (uint32_t)sizeof(pkc_small_data_t) \
                                                    + (uint32_t)sizeof(ecc_point_affine_t) \
                                                    + (uint32_t)sizeof(uint32_t))
/* 80 byte the size of HMAC key */
#define MH_KEY_HMAC_DATA_SIZE                   ((uint32_t)sizeof(hmac_key_t) \
                                                    + (uint32_t)MH_KEY_HMAC_KEY_SIZE_IN_BYTES \
                                                    + (uint32_t)sizeof(uint32_t))
/* 68 byte the size of TLS MASTER secret key */
#define MH_KEY_TLS_MASTER_DATA_SIZE             ((uint32_t)sizeof(master_secret_t) \
                                                    + (uint32_t)MH_KEY_MASTER_SECRET_SIZE_IN_BYTES \
                                                    + (uint32_t)sizeof(uint32_t))

#define R_KEY_SIZE_IN_BYTES         (32u)

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* rkey type */
typedef enum
{
    E_RKEY_TYPE_SPECIFIC,
    E_RKEY_TYPE_COMMON
} rkey_type_t;

/* del key type */
typedef enum
{
    E_DELKEY_TYPE_RAM,
    E_DELKEY_TYPE_TEMP
} del_key_type_t;

/* key cache op */
typedef enum
{
    E_KEYCACHE_IN_BUFFER,
    E_KEYCACHE_CLEAR
} key_cache_op_t;

/* set operation */
typedef enum
{
    E_KEY_OPE_FIRST,
    E_KEY_OPE_CONTINUE
} set_operation_t;


/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
typedef uint32_t key_counter_t;
typedef uint16_t key_size_t;
typedef uint16_t key_flags_t;

/* aes flags */
typedef struct
{
    key_flags_t             encryption:1;
    key_flags_t             decryption:1;
    key_flags_t             cmac_generation:1;
    key_flags_t             cmac_verification:1;
    key_flags_t             write_protected:1;
    key_flags_t             disable_on_debug:1;
    key_flags_t             no_wild_card:1;
    key_flags_t             invalidate:1;
    key_flags_t             plain_key:1;
} aes_flags_t;

/* aes key property */
typedef struct
{
    key_counter_t           update_counter;
    key_size_t              size_in_bytes;
    aes_flags_t             flags;
} aes_key_prop_t;

/* aes key */
typedef struct
{
    uint32_t                availability;
    r_key_group_t           key_group;
    r_key_index_t           key_id;
    uint8_t                 vm_id;
    aes_key_prop_t          properties;
    uint8_t                 value[MH_KEY_AES_KEY_SIZE_IN_BYTES_COMMON];
} aes_key_t;

/* rsa flags */
typedef struct
{
    key_flags_t             write_protected:1;
    key_flags_t             disable_on_debug:1;
    key_flags_t             invalidate:1;
} rsa_flags_t;

/* rsa key property */
typedef struct
{
    key_counter_t           update_counter;
    key_size_t              modulus_size_in_bits;
    uint16_t                modulus_size_in_bytes;
    uint16_t                priv_exp_size_in_bytes;
    uint16_t                pub_exp_size_in_bytes;
    rsa_flags_t             flags;
} rsa_key_prop_t;

/* rsa key */
typedef struct
{
    uint32_t                availability;
    r_key_group_t           key_group;
    r_key_index_t           key_id;
    uint8_t                 vm_id;
    rsa_key_prop_t          *p_properties;
    uint8_t                 *p_priv_exponent;
    uint8_t                 *p_pub_exponent;
    uint8_t                 *p_modulus;
} rsa_key_t;

/* rsa pub ext */
typedef struct
{
    r_key_index_t           key_id;
    key_counter_t           update_counter;
    rsa_flags_t             flags;
    uint32_t                x509_cert_address;
    uint32_t                x509_cert_size_in_bytes;
    uint8_t                 x509_dvc[DVC_SIZE_IN_BYTES];
} rsa_pub_ext_t;

/* ecc flags */
typedef struct
{
    key_flags_t             write_protected:1;
    key_flags_t             disable_on_debug:1;
    key_flags_t             invalidate:1;
} ecc_flags_t;

/* ecc key property */
typedef struct
{
    key_counter_t           update_counter;
    key_size_t              priv_scalar_size_in_bits;
    r_ecc_curve_t           ecc_curve;
    ecc_flags_t             flags;
} ecc_key_prop_t;

/* ecc key */
typedef struct
{
    uint32_t                availability;
    r_key_group_t           key_group;
    r_key_index_t           key_id;
    uint8_t                 vm_id;
    ecc_key_prop_t          *p_properties;
    pkc_small_data_t        *p_priv_scalar;
    ecc_point_affine_t      *p_pub_point;
} ecc_key_t;

/* ecc pub ext */
typedef struct
{
    r_key_index_t           key_id;
    key_counter_t           update_counter;
    ecc_flags_t             flags;
    uint32_t                x509_cert_address;
    uint32_t                x509_cert_size_in_bytes;
    uint8_t                 x509_dvc[DVC_SIZE_IN_BYTES];
} ecc_pub_ext_t;

/* hmac key */
typedef struct
{
    uint32_t                availability;
    r_key_index_t           key_id;
    uint8_t                 vm_id;
    key_size_t              key_size_in_byte;
    void                    *p_key;
} hmac_key_t;

/* master secret */
typedef rsa_flags_t     master_secret_flags_t;

typedef struct
{
    uint32_t                availability;
    r_key_index_t           key_id;
    uint8_t                 vm_id;
    void                    *p_value;
    master_secret_flags_t   flags;
} master_secret_t;

/* SHE key correspondence */
typedef struct she_key
{
    r_key_group_t       key_group;
    r_key_index_t       key_id;
} she_key_t;

/* Boot Mac Info */
typedef struct bootmac_info
{
    uint32_t            uiUpdate_counter;
    struct
    {
        uint8_t         no_wild_card:1;         /* for SHE key update */
        uint8_t         write_protected:1;      /* for SHE key update */
        uint8_t         disable_on_debug:1;     /* for SHE key update */
    } flags;
} bootmac_info_t;

/**********************************************************************************
* Link to global constants
**********************************************************************************/
#pragma alignvar(4)
extern const uint8_t CONST_ZERO[];
extern const uint8_t SHE_KEY_UPDATE_ENC_C[];
extern const uint8_t SHE_KEY_UPDATE_MAC_C[];
extern const uint8_t SHE_KEY_UPDATE_ENC_C_2[];
extern const uint8_t SHE_KEY_UPDATE_MAC_C_2[];
extern const uint8_t SHE_KEY_UPDATE_ENC_C_3[];
extern const uint8_t SHE_KEY_UPDATE_MAC_C_3[];
extern const uint8_t SHE_KEY_UPDATE_ENC_C_4[];
extern const uint8_t SHE_KEY_UPDATE_MAC_C_4[];
extern const uint8_t SHE_KEY_UPDATE_ENC_C_5[];
extern const uint8_t SHE_KEY_UPDATE_MAC_C_5[];
extern const uint8_t SHE_KEY_UPDATE_ENC_C_6[];
extern const uint8_t SHE_KEY_UPDATE_MAC_C_6[];
extern const uint8_t SHE_KEY_UPDATE_ENC_C_7[];
extern const uint8_t SHE_KEY_UPDATE_MAC_C_7[];
extern const uint8_t SHE_KEY_UPDATE_ENC_C_8[];
extern const uint8_t SHE_KEY_UPDATE_MAC_C_8[];
extern const uint8_t SHE_KEY_UPDATE_ENC_C_9[];
extern const uint8_t SHE_KEY_UPDATE_MAC_C_9[];
extern const uint8_t N_0_1024[];
extern const uint8_t N_1_2048[];
extern const uint8_t E[3];

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
uint32_t MH_KEY_Init(void);
uint32_t MH_KEY_InitRamCfg(void);
uint32_t MH_KEY_GetAesKey(icum_job_t *p_job,
                          r_key_group_t key_group,
                          r_key_index_t key_id,
                          aes_key_t **p_key,
                          uint8_t in_use_enable_flag);
uint32_t MH_KEY_GetAesKeyCache_fast(uint8_t vm_id, r_key_index_t key_id, aes_key_t **p_key);
uint32_t MH_KEY_GetAesRam_fast(uint8_t vm_id, r_key_index_t key_id, aes_key_t **p_key);
uint32_t MH_KEY_SetAesKey(icum_job_t *p_job, set_operation_t operation, aes_key_t *p_new_key);
uint32_t MH_KEY_GetRsaKey(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id, rsa_key_t **p_key);
uint32_t MH_KEY_SetRsaKey(icum_job_t *p_job, set_operation_t operation, rsa_key_t *p_new_key);
uint32_t MH_KEY_GetEccKey(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id, ecc_key_t **p_key);
uint32_t MH_KEY_SetEccKey(icum_job_t *p_job, set_operation_t operation, ecc_key_t *p_new_key);
uint32_t MH_KEY_SetRsaPubExt(icum_job_t *p_job,
                             set_operation_t operation,
                             rsa_pub_ext_t *p_new_key);
uint32_t MH_KEY_SetEccPubExt(icum_job_t *p_job,
                             set_operation_t operation,
                             ecc_pub_ext_t *p_new_key);
uint32_t MH_KEY_GetHmacKey(icum_job_t *p_job, r_key_index_t key_id, hmac_key_t **p_key);
uint32_t MH_KEY_SetHmacKey(icum_job_t *p_job, hmac_key_t *p_new_key);
uint32_t MH_KEY_GetMasterSecret(icum_job_t *p_job, r_key_index_t key_id, master_secret_t **p_key);
uint32_t MH_KEY_SetMasterSecret(icum_job_t *p_job, master_secret_t *p_new_key);
uint32_t MH_KEY_GetSecretKey(uint8_t vm_id, uint32_t* p_keydata);
uint32_t MH_KEY_DeleteRamKeyData(icum_job_t *p_job, del_key_type_t key_type);
uint32_t MH_KEY_ConfigKeyCache(icum_job_t *p_job, key_cache_op_t select);
uint32_t MH_KEY_DecryptSheKey(icum_job_t *p_job,
                              ISD_KEY_UPDATE_SHE_t she_data,
                              aes_key_t *p_aes_key,
                              uint8_t *p_chip_id,
                              uint16_t check_key_flag,
                              uint8_t key_size_ext);
uint32_t MH_KEY_GenerateAESExportKey(icum_job_t *p_job,
                                     ISD_KEY_AES_RAM_EXP_t she_data,
                                     uint8_t *p_chip_id,
                                     uint16_t check_key_flag,
                                     uint8_t key_size_ext);
uint32_t MH_KEY_GenerateRsaKey(ISD_RSA_KEY_GENERATE_t rsa_key);
uint32_t MH_KEY_GenerateEccKey(ISD_ECC_KEY_GENERATE_t ecc_key);
uint32_t MH_KEY_InstallRsaRomPub(void);
uint32_t MH_KEY_FreeKeyCache(icum_job_t *p_job, r_key_group_t key_group, r_key_index_t key_id);
uint32_t MH_KEY_RamKeyDeleteNotAes(void);
uint32_t MH_KEY_CacheKeyDeleteNotAes(void);
uint32_t MH_KEY_ConvertSheKey( r_key_index_t input_key_id, r_key_group_t* key_group, r_key_index_t* key_id);
uint32_t MH_KEY_GetEccKeySize(r_ecc_curve_t ecc_curve_id, uint32_t *p_key_size);

#endif /* #if !defined(ICUM_MH_KEY_PUB_H) */
