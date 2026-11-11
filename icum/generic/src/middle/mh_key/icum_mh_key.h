/******************************************************************************/
/* Component Name  Key Manager                                                */
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

#if !defined(ICUM_MH_KEY_MNG_H)
#define ICUM_MH_KEY_MNG_H

/**********************************************************************************
*  General purpose common define
**********************************************************************************/
#define NUM0            ((uint32_t)0uL)       /* value : 0 */
#define NUM1            ((uint32_t)1uL)       /* value : 1 */
#define NUM2            ((uint32_t)2uL)       /* value : 2 */
#define NUM3            ((uint32_t)3uL)       /* value : 3 */
#define NUM4            ((uint32_t)4uL)       /* value : 4 */
#define NUM5            ((uint32_t)5uL)       /* value : 5 */
#define NUM6            ((uint32_t)6uL)       /* value : 6 */
#define NUM8            ((uint32_t)8uL)       /* value : 8 */
#define NUM12           ((uint32_t)12uL)      /* value : 12 */
#define NUM14           ((uint32_t)14uL)      /* value : 14 */
#define NUM15           ((uint32_t)15uL)      /* value : 15 */
#define NUM16           ((uint32_t)16uL)      /* value : 16 */
#define NUM17           ((uint32_t)17uL)      /* value : 17 */
#define NUM18           ((uint32_t)18uL)      /* value : 18 */
#define NUM19           ((uint32_t)19uL)      /* value : 19 */
#define NUM20           ((uint32_t)20uL)      /* value : 20 */
#define NUM95           ((uint32_t)95uL)      /* value : 95 */

/**********************************************************************************
* Number of keys supported common
**********************************************************************************/
/* number of copies of each key slot in Data Flash */
#define NB_FLASH_COPIES           2u
#define NB_FLASH_BLOCK_NUMBER    64u

/* Max number of in use counter */
#define NB_IN_USE_COUNTER_MAX       16u

/**********************************************************************************
* Number of AES keys supported - Kohaku variant
**********************************************************************************/
/* number of ROM keys */
#define NB_ROM_AES_KEYS             2u

/* number of AES keys (updated by the application) */
#define NB_AES_KEYS_DEFAULT         22u

/* number of RAM AES keys (updated by the application) */
#define NB_RAM_AES_KEYS_DEFAULT      (6u)

/* number of key copies managed in RAM */
#if defined(FSW_NB_AES_CACHE_1)
#define NB_RAM_COPIES_AES_KEYS_CHANGE   (1uL)
#else
#define NB_RAM_COPIES_AES_KEYS_CHANGE   (16uL)
#endif

/**********************************************************************************
* Number of RSA keys supported - Kohaku variant
**********************************************************************************/
/* number of RSA private keys (internally generated) */
#define NB_RSA_PRIV_KEYS_DEFAULT    2u

/* number of RSA public keys (imported by the application) */
#define NB_RSA_PUB_KEYS_DEFAULT     5u

/* number of RSA public extension keys (imported by the application) */
#define NB_RSA_PUB_EXT_KEYS_DEFAULT  0u

/* number of key copies managed in RAM */
#define NB_RAM_COPIES_RSA_KEYS      1u

/**********************************************************************************
* Number of ECC keys supported - Kohaku variant
**********************************************************************************/
/* number of ECC private keys (internally generated) */
#define NB_ECC_PRIV_KEYS_DEFAULT    0u

/* number of ECC ROM public keys (provided by Renesas) */
/* #define NB_ECC_ROM_PUB_KEYS        MH_KEY_NB_ECC_ROM_PUB_KEYS */

/* number of ECC public keys (imported by the application) */
#define NB_ECC_PUB_KEYS_DEFAULT     0u

/* number of ECC public extension keys (imported by the application) */
#define NB_ECC_PUB_EXT_KEYS_DEFAULT 0u

/* number of RAM ECC keys (ephemeral keys) (updated by the application) */
#define NB_RAM_ECC_KEYS_DEFAULT     (2u)

/* number of key copies managed in RAM */
#define NB_RAM_COPIES_ECC_KEYS      1u

/**********************************************************************************
* Number of HMAC keys supported - Kohaku variant
**********************************************************************************/
/* number of RAM HMAC keys (ephemeral keys) (updated by the application) */
#define NB_RAM_HMAC_KEYS_DEFAULT    (4u)

/**********************************************************************************
* Number of Master Secret supported - Kohaku variant
**********************************************************************************/
/* number of Master Secret keys (ephemeral keys) (updated by the application) */
#define NB_MASTER_SECRET_DEFAULT    (1u)

/**********************************************************************************
* Number of key mapping
**********************************************************************************/
#define KEY_MAPPING_AES                  (0u)
#define KEY_MAPPING_RSA_PRIV             (1u)
#define KEY_MAPPING_RSA_PUB              (2u)
#define KEY_MAPPING_ECC_PRIV             (3u)
#define KEY_MAPPING_ECC_PUB              (4u)
#define KEY_MAPPING_RSA_PUB_EXT          (5u)
#define KEY_MAPPING_ECC_PUB_EXT          (6u)
#define KEY_MAPPING_AES_RAM              (7u)
#define KEY_MAPPING_AES_BUFFER           (8u)
#define KEY_MAPPING_ECC_RAM              (9u)
#define KEY_MAPPING_HMAC                (10u)
#define KEY_MAPPING_TLS_MASTER          (11u)

/**********************************************************************************
* The end of the key id
**********************************************************************************/
#define KEY_ID_LAST_ITEM              ((r_key_index_t)0xFFu)

/**********************************************************************************
* For storing key data being written
**********************************************************************************/
typedef struct key_writing
{
    uint8_t                 vm_id;
    r_key_group_t           key_group;
    r_key_index_t           key_id;
} key_writing_t;

#define KEY_WRITING         0x01u
#define KEY_NOT_WRITING     0x00u

/**********************************************************************************
* Link to global constants
**********************************************************************************/
extern const uint32_t RANDOMNUMBER_SECRET_C[NUM17][NUM4];


/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
uint32_t mh_KEY_ConvertErr( uint32_t base_err );
uint32_t mh_KEY_InitAes(void);
uint32_t mh_KEY_InitAesRamCfg(void);
uint32_t mh_KEY_InitRsa(void);
uint32_t mh_KEY_InitEcc(void);
uint32_t mh_KEY_InitEccRamCfg(void);
uint32_t mh_KEY_InitHmac(void);
uint32_t mh_KEY_InitMasterSecret(void);
uint32_t mh_KEY_FreeCacheAes(icum_job_t *p_job, r_key_group_t key_group,r_key_index_t key_id);
uint32_t mh_KEY_FreeCacheRsa(icum_job_t *p_job, r_key_group_t key_group,r_key_index_t key_id);
uint32_t mh_KEY_FreeCacheEcc(icum_job_t *p_job, r_key_group_t key_group,r_key_index_t key_id);
uint32_t mh_KEY_FreeCacheHmac(icum_job_t *p_job, r_key_index_t key_id);
uint32_t mh_KEY_FreeCacheMasterSecret(icum_job_t *p_job, r_key_index_t key_id);
uint32_t mh_KEY_DeleteRsaCacheKey(void);
uint32_t mh_KEY_DeleteEccCacheKey(void);
uint32_t mh_KEY_DeleteEccRamKey(void);
uint32_t mh_KEY_DeleteHmacRamKey(void);
uint32_t mh_KEY_DeleteMasterSecretRamKey(void);
uint32_t mh_KEY_WritingSet(uint8_t vm_id, r_key_group_t group, r_key_index_t id);
uint32_t mh_KEY_WritingStsGet(uint16_t *p_write_sts);
uint32_t mh_KEY_WritingJudgment(icum_job_t *p_job, r_key_group_t group, r_key_index_t id, uint16_t *p_write_sts);
uint32_t mh_KEY_MapSize(uint8_t key_type, uint8_t *p_key_number);
uint32_t mh_KEY_MapSizeVM(uint8_t vm_id, uint8_t key_type, uint8_t *p_key_number);
uint32_t mh_KEY_CheckVMID(uint8_t vm_id);

#endif /* #if !defined() */
