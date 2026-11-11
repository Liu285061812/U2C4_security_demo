/******************************************************************************/
/* Component Name   ICU-M Interface Library (ICUMIF)                          */
/**********************************************************************************************************************/
/* Product      : ICU-M Low Level Driver (Demo Part)                                                                  */
/**********************************************************************************************************************/
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

#if !defined(R_ICUMIF_API_H)
#define R_ICUMIF_API_H

/**********************************************************************************/
/* Ensure the structure alignment                                                 */
/**********************************************************************************/

#pragma pack(8)

/**********************************************************************************/
/* Headers                                                                        */
/**********************************************************************************/

/**********************************************************************************/
/* Defines                                                                        */
/**********************************************************************************/

#include "renesas_types.h"

/*===========================================================================*/
/* Transition state of ISD service request / response queue                  */
/*===========================================================================*/
#define R_ICUMIF_REQRES_PE_REQ_ENQUE        (0x01U)     /* PE enqueued ISD to service request queue      */
#define R_ICUMIF_REQRES_ICUM_REQ_DEQUE      (0x02U)     /* ICU-M dequeued ISD from service request queue */
#define R_ICUMIF_REQRES_ICUM_RES_ENQUE      (0x03U)     /* ICU-M enqueued ISD to service response queue  */
#define R_ICUMIF_REQRES_PE_RES_DEQUE        (0x04U)     /* PE dequeued ISD from service response queue   */

#define R_ICUMIF_REQRES_INTERRPUT           (0x00U)
#define R_ICUMIF_REQRES_NOINTERRPUT         (0x01U)

/*===========================================================================*/
/* Priority of ICU-M service                                                 */
/*===========================================================================*/
#define R_ICUMIF_SERV_PRIORITY_HIGHEST      (0x00U)
#define R_ICUMIF_SERV_PRIORITY_HIGH         (0x3FU)
#define R_ICUMIF_SERV_PRIORITY_NORMAL       (0x7fU)
#define R_ICUMIF_SERV_PRIORITY_LOWEST       (0xffU)

/*===========================================================================*/
/* Identifier of the interrupt factor                                        */
/*===========================================================================*/
#define R_ICUMIF_FACT_SYSTEM_ERROR          (0UL)
#define R_ICUMIF_FACT_WDT_OVERFLOW          (1UL)
#define R_ICUMIF_FACT_SEC_TICK_TIMER        (2UL)

/* Abnormality factor to be detected by inside the security service handler */
#define R_ICUMIF_FACT_ISD_ADDR_INVALID      (128UL)

/**********************************************************************************/
/* Macros                                                                         */
/**********************************************************************************/

/**********************************************************************************/
/* ICU-M interface: types                                                         */
/**********************************************************************************/

/*===========================================================================*/
/* Enumerates: ICU-M service ID                                              */
/*===========================================================================*/

        /*******************************************/
        /* Note: the below enumerates should align */
        /*       with the table service_handler[]  */
        /*       declared in issd_01_dispatch.c    */
        /*******************************************/

typedef enum
{
    SERVICE_00_ECC_TEST                 = 0x00,

    SERVICE_01_AES_CIPHER               = 0x10,
    SERVICE_01_AES_AUTH_CIPHER,
    SERVICE_01_AES_CMAC,
    SERVICE_01_RSA_VERIFY,
    SERVICE_01_RSA_SIGN,
    SERVICE_01_RSA_ENCRYPT,
    SERVICE_01_RSA_DECRYPT,
    SERVICE_01_ECDSA_VERIFY,
    SERVICE_01_ECDSA_SIGN,
    SERVICE_01_ECDH,
    SERVICE_01_HASH,
    SERVICE_01_HMAC,
    
    SERVICE_02_RAND_INIT_RNG            = 0x20,
    SERVICE_02_RAND_EXTEND_SEED,
    SERVICE_02_RAND_RND,

    SERVICE_03_AES_KEY_UPDATE_PLAIN     = 0x30,
    SERVICE_03_PK_IMPORT,
    SERVICE_03_RSA_KEY_GENERATE,
    SERVICE_03_ECC_KEY_GENERATE,
    SERVICE_03_HMAC_IMPORT,

    SERVICE_04_ICUMDF_GC                = 0x40,

    SERVICE_01_EDDSA_VERIFY             = 0x50,
    SERVICE_01_EDDSA_SIGN,

    LAST_ITEM_SERV_ID = 0x01000000
} r_icumif_service_id_t;

/*===========================================================================*/
/* Enumerates: ICU-M service results                                         */
/*===========================================================================*/

typedef enum
{
    SERV_OK,                        /* service completed with no error               */
    SERV_SYS_CFG_NOT_INITIALIZED,   /* system configuration not initialized          */
    SERV_ID_ERROR,                  /* wrong service ID                              */
    SERV_REQUESTER_ID_ERROR,        /* wrong requester ID                            */
    SERV_PRM_ERROR,                 /* wrong service parameter error                 */
    SERV_KEY_INVALID,               /* key invalid                                   */
    SERV_KEY_NOT_USABLE,            /* key not usable for the service                */
    SERV_INVALID_KEY_GROUP,         /* key group is not valid                        */
    SERV_INVALID_KEY_INDEX,         /* wrong key index provided                      */
    SERV_KEY_SLOT_EMPTY,            /* key slot empty                                */
    SERV_KEY_WRITE_PROTECTED,       /* key is write protected                        */
    SERV_KEY_UPDATE_ERROR,          /* key update error                              */
    SERV_SRC_MEM_ERROR,             /* error on source memory range                  */
    SERV_DEST_MEM_ERROR,            /* error on destination memory range             */
    SERV_JOB_QUEUE_FULL,            /* cannot accept another service request         */
    SERV_UNAVAILABLE,               /* service unavailable                           */
    SERV_INSTANCE_ALREADY_RUNNING,  /* service instance already running              */
    SERV_SYS_ERROR,                 /* the service caused a system error (exception) */
    SERV_MPU_ERROR,                 /* the service caused a MPU error (exception)    */

    SERV_NVM_PROG_ERROR,            /* error while programming the Flash                     */
    SERV_NVM_SYS_ERROR,             /* error in the Flash sequencer                          */
    SERV_NVM_SYS_ERROR_CRITICAL,    /* error in the Flash sequencer                          */
    SERV_NVM_UNAVAILABLE,           /* Flash memory unavailable (e.g. PE1 reprogramming it)  */
    SERV_DATA_INVALID,              /* error on the DVC (data inconsistency)                 */
    SERV_NVM_DATA_WARNING,          /* one data slot broken                                  */
    SERV_NVM_SIZE_ERROR,            /* secure DF too small for the number of keys to manage  */

    SERV_CRYPT_ERROR,               /* error in the cryptographic library                             */
    SERV_CLUSTER_VERIF_STATUS_LOCK, /* memory verification status locked                              */
    SERV_BKGROUND_CHECK_RUNNING,    /* background check running - unable to continue                  */

    SERV_FW_ACTIVATION_FAILED,      /* firmware activation sequence invalid   */
    SERV_INVALID_ENCODING_FORMAT,   /* invalid data encoding format           */
    SERV_ISD_NOT_ACCESSIBLE,        /* service request not accessible by ICUP */
    SERV_RAM_SIZE_ERROR,            /* secure config ram too small for the number of keys to manage */

    SERV_KEY_GENERATION_ERROR,      /* error while generating RSA / ECC private keys */
    SERV_KEY_VERIFICATION_FAIL,     /* error on signature key verification           */
    SERV_INVALID_CURVE_ID,          /* invalid ECC curve id                          */
    SERV_ECC_POINT_INVALID,         /* invalid ECC public key                        */

    SERV_MEMORY_FULL,               /* no more memory space */

    SERV_INIT_DELAY,                /* service initialization delayed (e.g. no AES context available) */

    SERV_MYDATA_SET_EMPTY,          /* no custom data set available */
    SERV_MYDATA_SLOT_EMPTY,         /* data slot available          */
    SERV_MYDATA_SIZE_ERR,           /* invalid data size            */
    SERV_MYDATA_INVALID_IDX,        /* invalid data slot index      */

    SERV_REQUEST_QUEUE_FULL,        /* service request queue full */

    SERV_INVALID_OPERATION,         /* invalid operation                               */

    SERV_KEY_CACHE_FULL,            /* unable to fix another key in cache            */
    SERV_SEQUENCE_ERROR,            /* Wrong calling sequence                        */

    SERV_CR_VERIFY_FAIL,            /* Failure on challenge & response */

    SERV_AES_TIMEOUT_ERROR,         /* wrong aes driver time out error  */
    SERV_AES_STATUS_ERROR,          /* wrong aes driver status error    */
    SERV_AES_PRM_ERROR,             /* wrong aes driver parameter error */

    SERV_NO_COMP_TRNG,              /* Call initialization services during the true random number generation */
    SERV_NO_INIT_RNG,               /* Using random number generation service not initialized */
    SERV_TRN_ERROR,                 /* online test NG, or non-online test 3 times in consecutive random values match */
    SERV_NEED_RESEED,               /* A reseed is required(CTR_DRBG method) */

    SERV_RESTRICTED,                /* service is restricted by life cycle stage */

    SERV_NVM_DATA_FULL_WARNING1,    /* flash memory free area warning 1     */
    SERV_NVM_DATA_FULL_WARNING2,    /* flash memory free area warning 2     */

    /* Error ID unique to the R-Car series */
    SERV_NVM_REJECTED = 0xff00,     /* Failed to acquire flash access authority */

    SERV_REQUEST    = 0xfff0,
    SERV_NEW_REQUEST,
    SERV_PENDING,
    SERV_IN_PROCESS = 0xffff,

    LAST_ITEM_SERV_ERR = 0x01000000
} r_icumif_service_result_t;


/*===========================================================================*/
/* Enumerates: service segmentation                                          */
/*===========================================================================*/

typedef enum
{
    JOB_ALL_AT_ONCE,
    JOB_INIT            = 0x01,
    JOB_UPDATE          = 0x02,
    JOB_FINISH          = 0x04,

    LAST_ITEM_JOB = 0x01000000
} r_job_cycle_t;

/*===========================================================================*/
/* Enumerates: verification result (CMAC, signature)                         */
/*===========================================================================*/

typedef enum
{
    VERIFICATION_PASS,
    VERIFICATION_FAIL,

    LAST_ITEM_VERIF_STATUS = 0x01000000
} r_verif_result_t;

/*===========================================================================*/
/* Enumerates: number formats                                                */
/*===========================================================================*/

typedef enum
{
    FORMAT_STRING_DECIMAL,
    FORMAT_STRING_HEXA,
    FORMAT_BINARY_PLAIN,

    LAST_ITEM_FORMAT_TYPE = 0x01000000
} r_number_format_t;

/*===========================================================================*/
/* Enumerates: hash encoding formats (rsa sign/verify)                       */
/*===========================================================================*/

typedef enum
{
    ENCODING_FORMAT_EMSA_PSS,
    ENCODING_FORMAT_EMSA_PKCS1_V15,

    LAST_ITEM_HASH_ENCODING_TYPE = 0x01000000
} r_hash_encoding_format_t;

/*===========================================================================*/
/* Enumerates: message encoding formats (rsa encrypt/decrypt)                */
/*===========================================================================*/

typedef enum
{
    ENCODING_FORMAT_EME_OAEP,
    ENCODING_FORMAT_EME_PKCS1_V15,

    LAST_ITEM_MSG_ENCODING_TYPE = 0x01000000
} r_message_encoding_format_t;

/*===========================================================================*/
/* Enumerates: hash primitives                                               */
/*===========================================================================*/

typedef enum
{
    HASH_PRIMITIVE_MD5,
    HASH_PRIMITIVE_RIPEMD_128,
    HASH_PRIMITIVE_RIPEMD_160,
    HASH_PRIMITIVE_RIPEMD_256,
    HASH_PRIMITIVE_RIPEMD_320,
    HASH_PRIMITIVE_SHA1,
    HASH_PRIMITIVE_SHA2_224,
    HASH_PRIMITIVE_SHA2_256,
    HASH_PRIMITIVE_SHA2_384,
    HASH_PRIMITIVE_SHA2_512,
    HASH_PRIMITIVE_SHA2_512_224,
    HASH_PRIMITIVE_SHA2_512_256,
    HASH_PRIMITIVE_SHA3_256 = 13,
    HASH_PRIMITIVE_SHA3_384,
    HASH_PRIMITIVE_SHA3_512,

    LAST_ITEM_HASH_PRIMITIVE = 0x01000000
} r_hash_primitive_t;

/*===========================================================================*/
/* Enumerates: signature algorithms (for PK key imports/exports)             */
/*===========================================================================*/

typedef enum
{
    SIGALGO_NONE,
    SIGALGO_RSA_PSS,
    SIGALGO_RSA_PKCS1_V15,

    LAST_ITEM_SIGNATURE_ALGO = 0x01000000
} r_signature_algo_t;

/*===========================================================================*/
/* Enumerates: ECC curves                                                    */
/*===========================================================================*/

typedef enum
{
    ECC_CURVE_NIST_P192,
    ECC_CURVE_NIST_P224,
    ECC_CURVE_NIST_P256,
    ECC_CURVE_NIST_P384,
    ECC_CURVE_NIST_P521,
    ECC_CURVE_BRAINPOOL_P192,
    ECC_CURVE_BRAINPOOL_P224,
    ECC_CURVE_BRAINPOOL_P256,
    ECC_CURVE_BRAINPOOL_P320,
    ECC_CURVE_BRAINPOOL_P384,
    ECC_CURVE_BRAINPOOL_P512,
    ECC_CURVE_BRAINPOOL_P256_T,
    ECC_CURVE_BRAINPOOL_P320_T,
    ECC_CURVE_BRAINPOOL_P384_T,
    ECC_CURVE_BRAINPOOL_P512_T,
    ECC_CURVE_CURVE25519,
    ECC_CURVE_CURVE448,
    ECC_CURVE_ED25519,
    ECC_CURVE_ED448,

    LAST_ITEM_ECC_TYPE = 0x01000000
} r_ecc_curve_t;

/*===========================================================================*/
/* Enumerates: KDF algorithms                                                */
/*===========================================================================*/

typedef enum
{
    KDF_ALGO_KDF1,
    KDF_ALGO_KDF2,
    KDF_ALGO_KDF3,
    KDF_ALGO_PKDF1,
    KDF_ALGO_PKDF2,

    LAST_ITEM_KDF_ALGO = 0x01000000
} r_kdf_algo_t;

/*===========================================================================*/
/* Enumerates: key group                                                     */
/*===========================================================================*/
typedef enum
{
    KEY_GRP_AES_ROM,
    KEY_GRP_AES,
    KEY_GRP_AES_RAM,
    KEY_GRP_SHE,
    KEY_GRP_RSA,
    KEY_GRP_RSA_PUB,
    KEY_GRP_RSA_ROM_PUB,
    KEY_GRP_ECC,
    KEY_GRP_ECC_RAM,
    KEY_GRP_ECC_PUB,
    KEY_GRP_ECC_ROM_PUB,
    KEY_GRP_AES_CR,
    KEY_GRP_HMAC,
    KEY_GRP_TLS_MASTER,
    KEY_GRP_RSA_PUB_EXT,
    KEY_GRP_ECC_PUB_EXT,

    LAST_ITEM_KEY_GRP = 0x01000000
} r_key_group_t;

/**********************************************************************************/
/* Type definition                                                                */
/**********************************************************************************/

/*===========================================================================*/
/* Type definition: requester ID                                             */
/*===========================================================================*/
typedef uint8_t r_icumif_requester_id_t;

/*===========================================================================*/
/* Type definition: vm ID                                                    */
/*===========================================================================*/
typedef uint8_t r_icumif_vm_id_t;

/*===========================================================================*/
/* Type definition: host interrupt channel definition                        */
/*===========================================================================*/
typedef uint8_t r_icumif_host_intch_t;

/*===========================================================================*/
/* Type definition: transition state of ISD service request / response queue */
/*===========================================================================*/
typedef uint8_t r_icumif_req_res_status_t;

/*===========================================================================*/
/* Type definition: priority of ICU-M service                                */
/*===========================================================================*/
typedef uint8_t r_icumif_service_priority_t;

/*===========================================================================*/
/* Type definition: request no interrupt                                     */
/*===========================================================================*/
typedef uint8_t r_icumif_req_nointerrupt_t;

/*===========================================================================*/
/* Type definition: responset no interrupt                                   */
/*===========================================================================*/
typedef uint8_t r_icumif_res_nointerrupt_t;

/*===========================================================================*/
/* Type definition: job ID                                                   */
/*===========================================================================*/
typedef uint16_t r_icumif_job_id_t;

/*===========================================================================*/
/* Type definition: pointer to call back function                            */
/*===========================================================================*/
typedef void (*p_func_t)(void *p_ISD);

/*===========================================================================*/
/* Job slice                                                                 */
/*===========================================================================*/
typedef uint16_t r_job_slice_t;

/*===========================================================================*/
/* Type definition: key index                                                */
/*===========================================================================*/
typedef uint8_t r_key_index_t;

/*===========================================================================*/
/* Type definition: key property flag set                                    */
/*===========================================================================*/
typedef uint16_t r_key_property_flags_t;

/**********************************************************************************/
/* Header files inclusion for specific service properties                         */
/**********************************************************************************/

#include "r_icumif_api_sys_init.h"
#include "r_icumif_api_get_info.h"
#include "r_icumif_api_ecc_test.h"
#include "r_icumif_api_access_secreg.h"
#include "r_icumif_api_secreg_acc.h"
#include "r_icumif_api_temprelease_upgp.h"
#include "r_icumif_api_aes_key_update_plain.h"
#include "r_icumif_api_aes_key_update_she.h"
#include "r_icumif_api_aes_key_export_she.h"
#include "r_icumif_api_aes_cipher.h"
#include "r_icumif_api_aes_auth_cipher.h"
#include "r_icumif_api_aes_cmac.h"
#include "r_icumif_api_rand_generate.h"
#include "r_icumif_api_pk_import.h"
#include "r_icumif_api_pk_import_ext.h"
#include "r_icumif_api_pk_export.h"
#include "r_icumif_api_rsa_key_generate.h"
#include "r_icumif_api_ecc_key_generate.h"
#include "r_icumif_api_rsa_verify.h"
#include "r_icumif_api_rsa_sign.h"
#include "r_icumif_api_rsa_encrypt.h"
#include "r_icumif_api_rsa_decrypt.h"
#include "r_icumif_api_ecdh.h"
#include "r_icumif_api_ecdsa_verify.h"
#include "r_icumif_api_ecdsa_sign.h"
#include "r_icumif_api_hash.h"
#include "r_icumif_api_hmac.h"
#include "r_icumif_api_hmac_import.h"
#include "r_icumif_api_sys_cfg.h"
#include "r_icumif_api_key_mgmt.h"
#include "r_icumif_api_cancel_she.h"
#include "r_icumif_api_user_flash.h"
#include "r_icumif_api_cmd_debug.h"
#include "r_icumif_api_secure_flash_mgmt.h"
#include "r_icumif_api_eddsa_verify.h"
#include "r_icumif_api_eddsa_sign.h"

/**********************************************************************************/
/* Header files inclusion for SHE specific parameters                             */
/**********************************************************************************/

#include "r_icumif_api_she.h"

/*===========================================================================*/
/* Type definition: ICU-M service descriptor (ISD)                           */
/*===========================================================================*/

typedef struct service_descriptor
{
    r_icumif_service_id_t               service_id;
    r_icumif_requester_id_t             requester_id;
    r_icumif_vm_id_t                    vm_id;
    r_icumif_host_intch_t               host_int_ch;
    volatile r_icumif_req_res_status_t  req_res_status;
    r_icumif_service_priority_t         service_priority;
    volatile r_icumif_service_result_t  service_result;
    volatile r_icumif_req_nointerrupt_t   req_nointerrupt;
    volatile r_icumif_res_nointerrupt_t   res_nointerrupt;
    volatile r_icumif_job_id_t          job_id;
    union
    {
        p_func_t                        p_callbackfunc;
        uint64_t                        padding_sys64;
    };
    union service_parameters
    {
        ISD_SYSTEM_INIT_t               SYSTEM_INIT;
        ISD_SYSTEM_CONFIG_t             SYSTEM_CONFIG;
        ISD_ECC_TEST_t                  ECC_TEST;
        ISD_GET_INFO_t                  GET_INFO;
        ISD_ACCESS_SECREG_t             ACCESS_SECREG;
        ISD_SECREG_ACCESS_t             SECREG_ACCESS;
        ISD_TEMPRELEASE_UPGP_t          TEMPRELEASE_UPGP;
        ISD_KEY_UPDATE_PLAIN_t          KEY_UPDATE_PLAIN;
        ISD_KEY_UPDATE_SHE_t            KEY_UPDATE_SHE;
        ISD_KEY_AES_RAM_EXP_t           RAM_KEY_EXPORT_SHE;
        ISD_AES_CIPHER_t                AES_CIPHER;
        ISD_AES_AUTH_CIPHER_t           AES_AUTH_CIPHER;
        ISD_AES_CMAC_t                  AES_CMAC;
        ISD_KEY_MNGT_t                  KEY_MGMT;
        ISD_CANCEL_SHE_t                CANCEL_SHE;
        ISD_RAND_INIT_t                 RAND_INIT;
        ISD_RAND_EXTEND_SEED_t          RAND_EXTEND_SEED;
        ISD_RAND_GENERATE_t             RAND_GENERATE;
        ISD_PK_IMPORT_t                 PK_IMPORT;
        ISD_PK_EXPORT_t                 PK_EXPORT;
        ISD_RSA_KEY_GENERATE_t          RSA_KEY_GENERATE;
        ISD_ECC_KEY_GENERATE_t          ECC_KEY_GENERATE;
        ISD_RSA_VERIFY_t                RSA_VERIFY;
        ISD_RSA_SIGN_t                  RSA_SIGN;
        ISD_RSA_ENCRYPT_t               RSA_ENCRYPT;
        ISD_RSA_DECRYPT_t               RSA_DECRYPT;
        ISD_ECDH_t                      ECDH;
        ISD_ECDSA_VERIFY_t              ECDSA_VERIFY;
        ISD_ECDSA_SIGN_t                ECDSA_SIGN;
        ISD_EDDSA_VERIFY_t              EDDSA_VERIFY;
        ISD_EDDSA_SIGN_t                EDDSA_SIGN;
        ISD_HASH_t                      HASH;
        ISD_SYSTEM_INFO_t               SYSTEM_INFO;
        ISD_USER_FLASH_MGMT_t           USER_FLASH_MGMT;
        ISD_HMAC_t                      HMAC;
        ISD_HMAC_IMPORT_t               HMAC_IMPORT;
        ISD_PK_IMPORT_EXT_t             PK_IMPORT_EXT;
        ISD_CMD_DEBUG_t                 CMD_DEBUG;
        ISD_SECURE_FLASH_MGMT_t         SECURE_FLASH_MGMT;
    } prm;
} r_icumif_isd_t;

/*===========================================================================*/
/* Type definition: ICU-M firmware status                                    */
/*===========================================================================*/

typedef struct r_icumif_sts
{
    uint32_t BUSY:1;
    uint32_t SECURE_BOOT:1;
    uint32_t BOOT_INIT:1;
    uint32_t BOOT_FINISHED:1;
    uint32_t BOOT_OK:1;
    uint32_t RND_INIT:1;
    uint32_t EXT_DEBUGGER:1;
    uint32_t INT_DEBUGGER:1;
    uint32_t :2;
    uint32_t MONERR:1;
    uint32_t BUSERR:1;
    uint32_t ROMERR:1;
    uint32_t RAMERR:1;
    uint32_t WDTERR:1;
    uint32_t SYSTEMERR:1;
    uint32_t :1;
    uint32_t EXTERR:1;
    uint32_t FCUERR:1;
    uint32_t RNDERR:1;
    uint32_t DFWARNING_B:1;
    uint32_t DFWARNING_A:1;
    uint32_t STARTUP_BANK:1;
    uint32_t BOOTERR:1;
    uint32_t RPC_WAIT:1;
    uint32_t TRNG:1;
    uint32_t FLS_WAIT:1;
    uint32_t FACIACC:1;
    uint32_t SERVAVAIL:1;
    uint32_t FCUINIT:1;
    uint32_t PLLSET:1;
    uint32_t STARTPE:1;
} r_icumif_sts_t;



#endif /* #if !defined(R_ICUMIF_API_H) */

/**********************************************************************************/
/* Structure alignment back to default configuration                              */
/**********************************************************************************/

#pragma pack()

