/******************************************************************************/
/* Component Name   ICU-M Interface Library (ICUMIF)                          */
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
#if !defined(R_ICUMIF_API_SYS_CFG_H)
#define R_ICUMIF_API_SYS_CFG_H

/**********************************************************************************/
/* Defines                                                                        */
/**********************************************************************************/
#define STARTUP_BANK_A  (0xA5u)
#define STARTUP_BANK_B  (0x00u)

#define ECC_TEST_UNLOCK (0x00u)
#define ECC_TEST_LOCK   (0x01u)

/**********************************************************************************/
/* Type definition                                                                */
/**********************************************************************************/
typedef enum
{
    SET_KEY_MAPPING,
    SET_BOOT_STAGE,
    SET_MYDATA_MAPPING,
    SET_SECURE_TICK,
    SET_SYS_INTERRUPT,
    SET_WDT,
    SET_CMAC_MIN,
    SET_SECURE_BOOT_CFG,
    SET_DEBUG,
    SET_RAND,
    SET_START_PE,
    SET_ECC_TEST_LOCK,
    SET_FLS_CFG,
    SET_VM_CFG,

    LAST_ITEM_CONFIG = 0x01000000
} sys_config_t;

typedef struct
{
    uint8_t     nb_aes_keys;
    uint8_t     nb_rsa_priv_keys;
    uint8_t     nb_rsa_pub_keys;
    uint8_t     nb_ecc_priv_keys;
    uint8_t     nb_ecc_pub_keys;
    uint8_t     nb_rsa_pub_ext_keys;
    uint8_t     nb_ecc_pub_ext_keys;
    uint8_t     nb_aes_ram_keys;
    uint8_t     nb_aes_buffer_keys;
    uint8_t     nb_ecc_ram_keys;
    uint8_t     nb_hmac_keys;
    uint8_t     nb_tls_master_keys;
} sys_cfg_key_mapping_t;

typedef struct
{
    uint8_t     nb_items;
    uint16_t    item_size_in_bytes;
} sys_cfg_mydata_set_t;


typedef enum
{
    SANCTION_DO_NOTHING,
    SANCTION_DEVICE_STOP,
    SANCTION_ICUM_ADMIN,

    LAST_ITEM_SANCTION = 0x01000000
} r_sanction_t;

typedef struct
{
    uint8_t     peid:4;
    uint8_t     channel:4;
} sys_cfg_sys_int_t;

typedef struct
{
    uint16_t    int_freq;
} sys_cfg_secure_tick_t;

typedef struct
{
    uint32_t    erm;
} sys_cfg_watchdog_t;

typedef struct
{
    uint8_t    bclr_psm;
    uint8_t    nsbf_psm;
    uint16_t    enable_skip_secureboot;
} sys_cfg_boot_t;

typedef struct
{
    uint8_t    mc_length_min_ctr;
    uint8_t    mc_length_min_val;
    uint8_t    short_mac_length_min_ctr;
    uint8_t    short_mac_length_min_val;
} sys_cfg_cmac_min_t;

typedef struct
{
    uint32_t    rand_trng_pre_init_start;
    uint32_t    rand_olt_no_ds_reset;
    uint32_t    rand_olt_ds_reset;
    uint32_t    rand_first_init_type;
} sys_cfg_rand_t;

typedef struct
{
    uint32_t    cr_auth_key_type;
    uint32_t    cr_auth_dlt;
} sys_cfg_debug_t;

typedef struct
{
    uint16_t    warning1;
    uint16_t    warning2;
} sys_cfg_fls_t;

typedef enum
{
    NONE_VM,
    UNIQUE_VM_ID_ALL_CORE,

    LAST_ITEM_VM_TYPE = 0x01000000
} sys_cfg_vm_type_t;

typedef struct
{
    sys_cfg_vm_type_t  vm_type;
    uint8_t            nb_vms;
    uint8_t            privilege_vm_id;
} sys_cfg_vm_t;

typedef enum
{
    LC_CR_NONE,
    LC_CR_CHALLENGE,
    LC_CR_RESPONSE,

    LAST_ITEM_LC_CR_REQ = 0x01000000
} r_cr_request_t;

/**********************************************************************************/
/* Sstem parameters                                                             */
/**********************************************************************************/
typedef struct ISD_SYSTEM_INFO
{
    r_cr_request_t      cr_request;
    union
    {
        uint8_t         *p_challenge;
        uint64_t        padding_sys64_1;
    };
    union
    {
        uint8_t         *p_response;
        uint64_t        padding_sys64_2;
    };
} ISD_SYSTEM_INFO_t;


/**********************************************************************************/
/* Service parameters                                                             */
/**********************************************************************************/

typedef struct ISD_SYSTEM_CONFIG
{
    sys_config_t        sys_cfg_id;
    struct config_parameters
    {
        sys_cfg_key_mapping_t   key_mapping;
        sys_cfg_mydata_set_t    mydata_set;
        sys_cfg_sys_int_t       sys_interrupt;
        sys_cfg_secure_tick_t   secure_tick;
        sys_cfg_watchdog_t      watchdog;
        sys_cfg_boot_t          boot;
        sys_cfg_cmac_min_t      mac_length;
        sys_cfg_debug_t         debug;
        sys_cfg_rand_t          rand;
        struct
        {
            r_sanction_t        sanction;
            r_job_slice_t       job_slice;
        };
        uint8_t                 start_pe_set;
        uint8_t                 test_lock;
        sys_cfg_fls_t           fls_cfg;
        sys_cfg_vm_t            vm_cfg;
    } cfg;
} ISD_SYSTEM_CONFIG_t;

#endif /* #if !defined(R_ICUMIF_API_SYS_CFG_H) */
