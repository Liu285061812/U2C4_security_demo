/******************************************************************************/
/* Component Name   ML Config Manager private header file                     */
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

#if !defined(ICUM_ML_CFG_H)
#define ICUM_ML_CFG_H

/**************************************************************************************/
/* Headers                                                                            */
/**************************************************************************************/
#include "r_icumif_api.h"
#include MM_FLASH_PUBLIC_H

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* system configuration */
typedef struct
{
    uint8_t                     dummy[FLS_DATA_SLOT_MGMT_SIZE];
    uint8_t                     firmware_name[16];
    r_job_slice_t               job_slice_boot;
    r_sanction_t                sanction_boot[2];
    sys_cfg_key_mapping_t       key_mapping[ML_CFG_TOTAL_NB_VMS_MAX];
    sys_cfg_mydata_set_t        mydata_set;
    uint32_t                    service_patch_authentic;
    uint32_t                    service_expansion_authentic;
    sys_cfg_watchdog_t          watchdog;
    sys_cfg_cmac_min_t          mac_length;
    sys_cfg_boot_t              sys_cfg_boot;
    sys_cfg_debug_t             sys_cfg_debug;
    sys_cfg_rand_t              sys_cfg_rand;
    uint32_t                    s_timer_freq;
    uint8_t                     used_ac[32];
    uint8_t                     start_pe_set;
    uint8_t                     test_lock;
    sys_cfg_fls_t               gc_warning_threshold;
    sys_cfg_sys_int_t           sys_interrupt;
    sys_cfg_vm_t                vm_cfg;
    uint8_t                     reserved[26];
} cfg_sys_t;

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
#define CFG_INIT_KEY_MAP_AES_KEYS         (1u)          /* Initial value of number of AES keys                */
#define CFG_INIT_KEY_MAP_RSA_PRIV_KEYS    (1u)          /* Initial value of number of RSA private keys        */
#define CFG_INIT_KEY_MAP_RSA_PUB_KEYS     (1u)          /* Initial value of number of RSA public keys         */
#define CFG_INIT_KEY_MAP_ECC_PRIV_KEYS    (1u)          /* Initial value of number of ECC private keys        */
#define CFG_INIT_KEY_MAP_ECC_PUB_KEYS     (1u)          /* Initial value of number of ECC public keys         */
#define CFG_INIT_KEY_MAP_RSA_PUB_EXT_KEYS (0u)          /* Initial value of number of RSA public extend keys  */
#define CFG_INIT_KEY_MAP_ECC_PUB_EXT_KEYS (0u)          /* Initial value of number of ECC public extend keys  */
#define CFG_INIT_KEY_MAP_AES_RAM_KEYS     (1u)          /* Initial value of number of AES ram keys            */
#define CFG_INIT_KEY_MAP_AES_BUFFER_KEYS  (0u)          /* Initial value of number of AES buffer keys         */
#define CFG_INIT_KEY_MAP_ECC_RAM_KEYS     (1u)          /* Initial value of number of ECC RAM keys            */
#define CFG_INIT_KEY_MAP_HMAC_KEYS        (1u)          /* Initial value of number of HMAC keys               */
#define CFG_INIT_KEY_MAP_TLS_MASTER_KEYS  (0u)          /* Initial value of number of TLS master secret keys  */
#define CFG_INIT_MAC_LEN_MIN_VAL        (128u)          /* Initial value of MAC length minimum value          */
#define CFG_INIT_SHORT_MAC_LEN_MIN_VAL  (63u)           /* Initial value of short MAC length minimum value    */
#define CFG_INIT_BOOT_NSBF_PSM          (1u)            /* Initial value of secure boot executability status  */
#define CFG_INIT_S_TIMER_FREQ           (10000u)        /* Initial value of secure tick timer interrupt time  */
#define CFG_INIT_TEST_ECC_LOCK          (1u)            /* Initial value of ECC Test Lock (protect on)        */
#define CFG_INIT_FLS_WARNING1_LEVEL     (30u)           /* Initial value of garbage collection warning 1 threshold  */
#define CFG_INIT_FLS_WARNING2_LEVEL     (15u)           /* Initial value of garbage collection warning 2 threshold  */
#define CFG_INIT_MAP_BOOTMODE           (0xFFFFFFFFuL)  /* Initial value of BootMode (BothMode)               */
#define CFG_INIT_NB_VMS                 (1u)            /* Initial value of number of VM in each PE           */
#define CFG_INIT_PRIVILEGE_VM_ID        (0x00u)         /* Initial value of privileged VM                     */

#endif /* ICUM_ML_CFG_H */
