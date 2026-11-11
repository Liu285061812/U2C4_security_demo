/******************************************************************************/
/* Component Name   ML Configuration Manager file                             */
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
* Headers
**********************************************************************************/

#include "renesas_types.h"
#include "icum_common_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_d_misc_pub.h"
/* For the item size config RAM of MH_KEY. */
#include "icum_mh_key_pub.h"
#include "icum_ml_cfg_pub.h"
#include "icum_ml_cfg.h"

/**********************************************************************************
* Define
***********************************************************************************/
#define ML_CFG_ALIGN4                        (4uL)
#define ML_CFG_SIZE_OF_ITEM(size, align)     ((((size) + ((align) - 1uL)) / (align)) * (align))

/**********************************************************************************
* Local variables
**********************************************************************************/
uint8_t g_cfg_aes_key_idx[ML_CFG_TOTAL_NB_VMS_MAX] = {0u};
uint8_t g_cfg_aes_ram_key_idx[ML_CFG_TOTAL_NB_VMS_MAX] = {0u};
uint8_t g_cfg_aes_buffer_key_idx[ML_CFG_TOTAL_NB_VMS_MAX] = {0u};
uint8_t g_cfg_rsa_pri_key_idx[ML_CFG_TOTAL_NB_VMS_MAX] = {0u};
uint8_t g_cfg_rsa_pub_key_idx[ML_CFG_TOTAL_NB_VMS_MAX] = {0u};
uint8_t g_cfg_rsa_pub_ext_key_idx[ML_CFG_TOTAL_NB_VMS_MAX] = {0u};
uint8_t g_cfg_ecc_pri_key_idx[ML_CFG_TOTAL_NB_VMS_MAX] = {0u};
uint8_t g_cfg_ecc_pub_key_idx[ML_CFG_TOTAL_NB_VMS_MAX] = {0u};
uint8_t g_cfg_ecc_pub_ext_key_idx[ML_CFG_TOTAL_NB_VMS_MAX] = {0u};
uint8_t g_cfg_ecc_ram_key_idx[ML_CFG_TOTAL_NB_VMS_MAX] = {0u};
uint8_t g_cfg_hmac_key_idx[ML_CFG_TOTAL_NB_VMS_MAX] = {0u};
uint8_t g_cfg_tls_master_key_idx[ML_CFG_TOTAL_NB_VMS_MAX] = {0u};

uint32_t g_cfg_aes_ram_start_addr = ML_CFG_CONFIG_RAM_START_ADDR;
uint32_t g_cfg_aes_buffer_start_addr = ML_CFG_CONFIG_RAM_START_ADDR;
uint32_t g_cfg_ecc_ram_start_addr = ML_CFG_CONFIG_RAM_START_ADDR;
uint32_t g_cfg_hmac_start_addr = ML_CFG_CONFIG_RAM_START_ADDR;
uint32_t g_cfg_tls_master_start_addr = ML_CFG_CONFIG_RAM_START_ADDR;

/*===========================================================================
  RAM copy of the system configuration data
  ===========================================================================*/
static cfg_sys_t g_sys_cfg;
CHANGE_SEC(bss, ".icum_kernel.bss")     /* Arrange far away from the above variable */
CHANGE_SEC(bss, default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ConfigMNG_0001]:[ICUMFW_UD_ML_ConfigMNG_0001]
* Function name: ML_CFG_Init
* Description  : Prepare Secure RAM for the system configuration
* Arguments    : N/A
* Return Value : ER_OK
***************************************************************************************/
uint32_t ML_CFG_Init(void)
{
    L_Toolbox_MemClr64((uint64_t *)&g_sys_cfg, (int32_t)sizeof(cfg_sys_t));
    /* set default system configuration value */
    g_sys_cfg.mac_length.mc_length_min_val           = CFG_INIT_MAC_LEN_MIN_VAL;
    g_sys_cfg.mac_length.short_mac_length_min_val    = CFG_INIT_SHORT_MAC_LEN_MIN_VAL;
    g_sys_cfg.sys_cfg_boot.nsbf_psm                  = CFG_INIT_BOOT_NSBF_PSM;
    g_sys_cfg.s_timer_freq                           = CFG_INIT_S_TIMER_FREQ;
    g_sys_cfg.test_lock                              = CFG_INIT_TEST_ECC_LOCK;

    /* master vm is set the initial values for configurable key */
    g_sys_cfg.key_mapping[ML_CFG_MASTER_VM_ID].nb_aes_keys      = CFG_INIT_KEY_MAP_AES_KEYS;
    g_sys_cfg.key_mapping[ML_CFG_MASTER_VM_ID].nb_rsa_priv_keys = CFG_INIT_KEY_MAP_RSA_PRIV_KEYS;
    g_sys_cfg.key_mapping[ML_CFG_MASTER_VM_ID].nb_rsa_pub_keys  = CFG_INIT_KEY_MAP_RSA_PUB_KEYS;
    g_sys_cfg.key_mapping[ML_CFG_MASTER_VM_ID].nb_ecc_priv_keys    = CFG_INIT_KEY_MAP_ECC_PRIV_KEYS;
    g_sys_cfg.key_mapping[ML_CFG_MASTER_VM_ID].nb_ecc_pub_keys     = CFG_INIT_KEY_MAP_ECC_PUB_KEYS;
    g_sys_cfg.key_mapping[ML_CFG_MASTER_VM_ID].nb_rsa_pub_ext_keys = CFG_INIT_KEY_MAP_RSA_PUB_EXT_KEYS;
    g_sys_cfg.key_mapping[ML_CFG_MASTER_VM_ID].nb_ecc_pub_ext_keys = CFG_INIT_KEY_MAP_ECC_PUB_EXT_KEYS;
    g_sys_cfg.key_mapping[ML_CFG_MASTER_VM_ID].nb_aes_ram_keys  = CFG_INIT_KEY_MAP_AES_RAM_KEYS;
    g_sys_cfg.key_mapping[ML_CFG_MASTER_VM_ID].nb_aes_buffer_keys = CFG_INIT_KEY_MAP_AES_BUFFER_KEYS;
    g_sys_cfg.key_mapping[ML_CFG_MASTER_VM_ID].nb_ecc_ram_keys = CFG_INIT_KEY_MAP_ECC_RAM_KEYS;
    g_sys_cfg.key_mapping[ML_CFG_MASTER_VM_ID].nb_hmac_keys = CFG_INIT_KEY_MAP_HMAC_KEYS;
    g_sys_cfg.key_mapping[ML_CFG_MASTER_VM_ID].nb_tls_master_keys = CFG_INIT_KEY_MAP_TLS_MASTER_KEYS;

    g_sys_cfg.vm_cfg.vm_type = NONE_VM;
    g_sys_cfg.vm_cfg.nb_vms = CFG_INIT_NB_VMS;
    g_sys_cfg.vm_cfg.privilege_vm_id = CFG_INIT_PRIVILEGE_VM_ID;

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ConfigMNG_0002]:[ICUMFW_UD_ML_ConfigMNG_0002]
* Function name: ML_CFG_GetSys
* Description  : Get the system configuration value on secure RAM
* Arguments    : sys_id    : System configuration ID
*                            Range: ML_CFG_SYS_FW_NAME
*                                   ML_CFG_SYS_FW_ACT
*                                   ML_CFG_SYS_CLUSTER_MAPPING
*                                   ML_CFG_SYS_VERIF_METHOD
*                                   ML_CFG_SYS_JOB_SLICE
*                                   ML_CFG_SYS_SANCTION
*                                   ML_CFG_SYS_MYDATA
*                                   ML_CFG_SYS_MONOCTR
*                                   ML_CFG_SYS_SERV_PATCH_AUTH
*                                   ML_CFG_SYS_SERV_EXP_AUTH
*                                   ML_CFG_SYS_WDT
*                                   ML_CFG_SYS_CMAC_MIN
*                                   ML_CFG_SYS_SECURE_BOOT
*                                   ML_CFG_SYS_DEBUG
*                                   ML_CFG_SYS_RAND
*                                   ML_CFG_SYS_LIFE_CYCLE
*                                   ML_CFG_SYS_OSC_CLK
*                                   ML_CFG_SYS_PLL0C
*                                   ML_CFG_SYS_OSC_STIME
*                                   ML_CFG_SYS_SECURE_TICK
*                                   ML_CFG_SYS_USED_AC
*                                   ML_CFG_SYS_START_PE
*                                   ML_CFG_SYS_STARTUP
*                                   ML_CFG_SYS_TEST_ECC_LOCK
*                                   ML_CFG_SYS_FLS
*                                   ML_CFG_SYS_VM
*                p_sys_cfg : System configuration value on Secure RAM corresponding to ID
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t ML_CFG_GetSys(uint16_t sys_id, uint32_t *p_sys_cfg)
{
    uint32_t err = ER_OK;

    switch((uint32_t)sys_id)
    {
        case ML_CFG_SYS_CMAC_MIN:
            *((sys_cfg_cmac_min_t *)p_sys_cfg) = g_sys_cfg.mac_length;
            break;
        case ML_CFG_SYS_FW_NAME:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.firmware_name,
                             (int16_t)sizeof(g_sys_cfg.firmware_name));
            break;
        case ML_CFG_SYS_JOB_SLICE:
            *((r_job_slice_t *)p_sys_cfg) = g_sys_cfg.job_slice_boot;
            break;
        case ML_CFG_SYS_SANCTION:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.sanction_boot,
                             (int16_t)sizeof(g_sys_cfg.sanction_boot));
            break;
        case ML_CFG_SYS_MYDATA:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.mydata_set,
                             (int16_t)sizeof(g_sys_cfg.mydata_set));
            break;
        case ML_CFG_SYS_SERV_PATCH_AUTH:
            *p_sys_cfg = g_sys_cfg.service_patch_authentic;
            break;
        case ML_CFG_SYS_SERV_EXP_AUTH:
            *p_sys_cfg = g_sys_cfg.service_expansion_authentic;
            break;
        case ML_CFG_SYS_WDT:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.watchdog,
                             (int16_t)sizeof(g_sys_cfg.watchdog));
            break;
        case ML_CFG_SYS_SECURE_BOOT:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.sys_cfg_boot,
                             (int16_t)sizeof(g_sys_cfg.sys_cfg_boot));
            break;
        case ML_CFG_SYS_DEBUG:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.sys_cfg_debug,
                             (int16_t)sizeof(g_sys_cfg.sys_cfg_debug));
            break;
        case ML_CFG_SYS_RAND:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.sys_cfg_rand,
                             (int16_t)sizeof(g_sys_cfg.sys_cfg_rand));
            break;
        case ML_CFG_SYS_SECURE_TICK:
            *p_sys_cfg = g_sys_cfg.s_timer_freq;
            break;
        case ML_CFG_SYS_USED_AC:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.used_ac,
                             (int16_t)sizeof(g_sys_cfg.used_ac));
            break;
        case ML_CFG_SYS_START_PE:
            *((uint8_t *)p_sys_cfg) = g_sys_cfg.start_pe_set;
            break;
        case ML_CFG_SYS_TEST_ECC_LOCK:
            *((uint8_t *)p_sys_cfg) = g_sys_cfg.test_lock;
            break;
        case ML_CFG_SYS_INTERRUPT:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.sys_interrupt,
                             (int16_t)sizeof(g_sys_cfg.sys_interrupt));
            break;
        case ML_CFG_SYS_VM:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                            (const uint8_t *)&g_sys_cfg.vm_cfg,
                            (int16_t)sizeof(sys_cfg_vm_t));
            break;
        default:
            err = ER_PARAM;
            break;
    }

    return err;
}

CHANGE_SEC(text, ".icum_middle_ram")
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ConfigMNG_0005]:[ICUMFW_UD_ML_ConfigMNG_0005]
* Function name: ML_CFG_GetSys_ram
* Description  : Get the system configuration value on secure RAM
* Arguments    : sys_id    : System configuration ID
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t ML_CFG_GetSys_ram(uint16_t sys_id, uint32_t *p_sys_cfg)
{
    uint32_t err = ER_OK;

    switch((uint32_t)sys_id)
    {
        case ML_CFG_SYS_WDT:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.watchdog,
                             (int16_t)sizeof(g_sys_cfg.watchdog));
            break;
        case ML_CFG_SYS_SECURE_BOOT:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.sys_cfg_boot,
                             (int16_t)sizeof(g_sys_cfg.sys_cfg_boot));
            break;
        case ML_CFG_SYS_START_PE:
            *((uint8_t *)p_sys_cfg) = g_sys_cfg.start_pe_set;
            break;
        case ML_CFG_SYS_TEST_ECC_LOCK:
            *((uint8_t *)p_sys_cfg) = g_sys_cfg.test_lock;
            break;
        case ML_CFG_SYS_INTERRUPT:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.sys_interrupt,
                             (int16_t)sizeof(g_sys_cfg.sys_interrupt));
            break;
        case ML_CFG_SYS_VM:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.vm_cfg,
                             (int16_t)sizeof(sys_cfg_vm_t));
            break;
        default:
            err = ER_PARAM;
            break;
    }

    return err;
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ConfigMNG_0003]:[ICUMFW_UD_ML_ConfigMNG_0003]
* Function name: ML_CFG_SetSys
* Description  : Set the system configuration value on secure RAM
* Arguments    : sys_id      : System configuration ID
*                              Range: ML_CFG_SYS_FW_NAME
*                                     ML_CFG_SYS_FW_ACT
*                                     ML_CFG_SYS_CLUSTER_MAPPING
*                                     ML_CFG_SYS_VERIF_METHOD
*                                     ML_CFG_SYS_JOB_SLICE
*                                     ML_CFG_SYS_SANCTION
*                                     ML_CFG_SYS_MYDATA
*                                     ML_CFG_SYS_MONOCTR
*                                     ML_CFG_SYS_SERV_PATCH_AUTH
*                                     ML_CFG_SYS_SERV_EXP_AUTH
*                                     ML_CFG_SYS_WDT
*                                     ML_CFG_SYS_CMAC_MIN
*                                     ML_CFG_SYS_SECURE_BOOT
*                                     ML_CFG_SYS_DEBUG
*                                     ML_CFG_SYS_RAND
*                                     ML_CFG_SYS_LIFE_CYCLE
*                                     ML_CFG_SYS_OSC_CLK
*                                     ML_CFG_SYS_PLL0C
*                                     ML_CFG_SYS_OSC_STIME
*                                     ML_CFG_SYS_SECURE_TICK
*                                     ML_CFG_SYS_USED_AC
*                                     ML_CFG_SYS_START_PE
*                                     ML_CFG_SYS_STARTUP
*                                     ML_CFG_SYS_TEST_ECC_LOCK
*                                     ML_CFG_SYS_FLS
*                                     ML_CFG_SYS_VM
*                p_new_sys_cfg : System configuration value on Secure RAM corresponding to ID
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t ML_CFG_SetSys(uint16_t sys_id, uint32_t *p_new_sys_cfg)
{
    uint32_t err = ER_OK;

    switch((uint32_t)sys_id)
    {
        case ML_CFG_SYS_CMAC_MIN:
            L_Toolbox_MemCpy((uint8_t *)&g_sys_cfg.mac_length,
                             (const uint8_t *)p_new_sys_cfg,
                             (int16_t)sizeof(g_sys_cfg.mac_length));
            break;
        case ML_CFG_SYS_FW_NAME:
            L_Toolbox_MemCpy((uint8_t *)&g_sys_cfg.firmware_name,
                             (const uint8_t *)p_new_sys_cfg,
                             (int16_t)sizeof(g_sys_cfg.firmware_name));
            break;
        case ML_CFG_SYS_JOB_SLICE:
            g_sys_cfg.job_slice_boot = (r_job_slice_t)*p_new_sys_cfg;
            break;
        case ML_CFG_SYS_SANCTION:
            L_Toolbox_MemCpy((uint8_t *)&g_sys_cfg.sanction_boot,
                             (const uint8_t *)p_new_sys_cfg,
                             (int16_t)sizeof(g_sys_cfg.sanction_boot));
            break;
        case ML_CFG_SYS_MYDATA:
            L_Toolbox_MemCpy((uint8_t *)&g_sys_cfg.mydata_set,
                             (const uint8_t *)p_new_sys_cfg,
                             (int16_t)sizeof(g_sys_cfg.mydata_set));
            break;
        case ML_CFG_SYS_SERV_PATCH_AUTH:
            g_sys_cfg.service_patch_authentic = *p_new_sys_cfg;
            break;
        case ML_CFG_SYS_SERV_EXP_AUTH:
            g_sys_cfg.service_expansion_authentic = *p_new_sys_cfg;
            break;
        case ML_CFG_SYS_WDT:
            L_Toolbox_MemCpy((uint8_t *)&g_sys_cfg.watchdog,
                             (const uint8_t *)p_new_sys_cfg,
                             (int16_t)sizeof(g_sys_cfg.watchdog));
            break;
        case ML_CFG_SYS_SECURE_BOOT:
            L_Toolbox_MemCpy((uint8_t *)&g_sys_cfg.sys_cfg_boot,
                             (const uint8_t *)p_new_sys_cfg,
                             (int16_t)sizeof(g_sys_cfg.sys_cfg_boot));
            break;
        case ML_CFG_SYS_DEBUG:
            L_Toolbox_MemCpy((uint8_t *)&g_sys_cfg.sys_cfg_debug,
                             (const uint8_t *)p_new_sys_cfg,
                             (int16_t)sizeof(g_sys_cfg.sys_cfg_debug));
            break;
        case ML_CFG_SYS_RAND:
            L_Toolbox_MemCpy((uint8_t *)&g_sys_cfg.sys_cfg_rand,
                             (const uint8_t *)p_new_sys_cfg,
                             (int16_t)sizeof(g_sys_cfg.sys_cfg_rand));
            break;
        case ML_CFG_SYS_SECURE_TICK:
            g_sys_cfg.s_timer_freq = *p_new_sys_cfg;
            break;
        case ML_CFG_SYS_USED_AC:
            L_Toolbox_MemCpy((uint8_t *)&g_sys_cfg.used_ac,
                             (const uint8_t *)p_new_sys_cfg,
                             (int16_t)sizeof(g_sys_cfg.used_ac));
            break;
        case ML_CFG_SYS_START_PE:
            g_sys_cfg.start_pe_set = (uint8_t)*p_new_sys_cfg;
            break;
        case ML_CFG_SYS_TEST_ECC_LOCK:
            g_sys_cfg.test_lock = (uint8_t)*p_new_sys_cfg;
            break;
        case ML_CFG_SYS_INTERRUPT:
            L_Toolbox_MemCpy((uint8_t *)&g_sys_cfg.sys_interrupt,
                             (const uint8_t *)p_new_sys_cfg,
                             (int16_t)sizeof(g_sys_cfg.sys_interrupt));
            break;
        case ML_CFG_SYS_VM:
            L_Toolbox_MemCpy((uint8_t *)&g_sys_cfg.vm_cfg,
                             (const uint8_t *)p_new_sys_cfg,
                             (int16_t)sizeof(sys_cfg_vm_t));
            break;
        default:
            err = ER_PARAM;
            break;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ConfigMNG_0006]:[ICUMFW_UD_ML_ConfigMNG_0006]
* Function name: ML_CFG_GetSysVM
* Description  : Get the system configuration value on secure RAM
* Arguments    : vm_id     : VM ID
*                sys_id    : System configuration ID
*                            Range: ML_CFG_SYS_KEY_MAPPING
*                p_sys_cfg : System configuration value on Secure RAM corresponding to ID
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t ML_CFG_GetSysVM(uint8_t vm_id, uint16_t sys_id, uint32_t *p_sys_cfg)
{
    uint32_t err;

    /* Initialize local variable */
    err = ER_OK;

    switch ((uint32_t)sys_id)
    {
        case ML_CFG_SYS_KEY_MAPPING:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.key_mapping[vm_id],
                             (int16_t)sizeof(sys_cfg_key_mapping_t));
            break;
        default:
            err = ER_PARAM;
            break;
    }

    return err;
}

CHANGE_SEC(text, ".icum_middle_ram")
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ConfigMNG_0007]:[ICUMFW_UD_ML_ConfigMNG_0007]
* Function name: ML_CFG_GetSysVM_ram
* Description  : Get the system configuration value on secure RAM
* Arguments    : vm_id       : VM ID
*                sys_id      : System configuration ID
*                              Range: ML_CFG_SYS_KEY_MAPPING
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t ML_CFG_GetSysVM_ram(uint8_t vm_id, uint16_t sys_id, uint32_t *p_sys_cfg)
{
    uint32_t err;

    /* Initialize local variable */
    err = ER_OK;

    switch ((uint32_t)sys_id)
    {
        case ML_CFG_SYS_KEY_MAPPING:
            L_Toolbox_MemCpy((uint8_t *)p_sys_cfg,
                             (const uint8_t *)&g_sys_cfg.key_mapping[vm_id],
                             (int16_t)sizeof(sys_cfg_key_mapping_t));
            break;
        default:
            err = ER_PARAM;
            break;
    }

    return err;
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ConfigMNG_0008]:[ICUMFW_UD_ML_ConfigMNG_0008]
* Function name: ML_CFG_SetSysVM
* Description  : Set the system configuration value on secure RAM
* Arguments    : vm_id       : VM ID
*                sys_id      : System configuration ID
*                              Range: ML_CFG_SYS_KEY_MAPPING
*                p_new_sys_cfg : System configuration value on Secure RAM corresponding to ID
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t ML_CFG_SetSysVM(uint8_t vm_id, uint16_t sys_id, uint32_t *p_new_sys_cfg)
{
    uint32_t err;

    /* Initialize local variable */
    err = ER_OK;

    switch ((uint32_t)sys_id)
    {
        case ML_CFG_SYS_KEY_MAPPING:
            L_Toolbox_MemCpy((uint8_t *)&g_sys_cfg.key_mapping[vm_id],
                             (const uint8_t *)p_new_sys_cfg,
                             (int16_t)sizeof(sys_cfg_key_mapping_t));
            break;
        default:
            err = ER_PARAM;
            break;
    }

    return err;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ConfigMNG_0004]:[ICUMFW_UD_ML_ConfigMNG_0004]
* Function name: ML_CFG_GetSysAllConfig
* Description  : Get address and size of the system configuration on secure RAM
* Arguments    : p_sys_all_cfg : Start address of the system configuration on Secure RAM
*                p_sys_size    : Size of the system configuration on Secure RAM
* Return Value : ER_OK
***************************************************************************************/
uint32_t ML_CFG_GetSysAllConfig(uint32_t **p_sys_all_cfg, uint16_t *p_sys_size)
{
    *p_sys_all_cfg = (uint32_t*)&g_sys_cfg;
    *p_sys_size = (uint16_t)sizeof(cfg_sys_t);

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ConfigMNG_0009]:[ICUMFW_UD_ML_ConfigMNG_0009]
* Function name: ML_CFG_GetKeyMappingData
* Description  : Get key mapping data set size
* Arguments    : p_key_map
* Return Value : ER_OK
*                ER_ML_CFG_RAM_SIZE_ERROR
*                ER_ML_CFG_NVM_SIZE_ERROR
***************************************************************************************/
uint32_t ML_CFG_GetKeyMappingData(sys_cfg_key_mapping_t *p_key_map)
{
    uint32_t err;
    uint32_t vm_id;
    uint32_t item_size;
    uint32_t end_addr;
    sys_cfg_vm_t vm_cfg;
    sys_cfg_key_mapping_t key_map;
    uint32_t nb_aes_keys;
    uint32_t nb_rsa_priv_keys;
    uint32_t nb_rsa_pub_keys;
    uint32_t nb_ecc_priv_keys;
    uint32_t nb_ecc_pub_keys;
    uint32_t nb_rsa_pub_ext_keys;
    uint32_t nb_ecc_pub_ext_keys;
    uint32_t nb_aes_ram_keys;
    uint32_t nb_aes_buffer_keys;
    uint32_t nb_ecc_ram_keys;
    uint32_t nb_hmac_keys;
    uint32_t nb_tls_master_keys;

    /* Initialize variable */
    err = ER_OK;
    vm_id = 0uL;
    item_size = 0uL;
    end_addr = ML_CFG_CONFIG_RAM_START_ADDR;
    nb_aes_keys = 0uL;
    nb_rsa_priv_keys = 0uL;
    nb_rsa_pub_keys = 0uL;
    nb_ecc_priv_keys = 0uL;
    nb_ecc_pub_keys = 0uL;
    nb_rsa_pub_ext_keys = 0uL;
    nb_ecc_pub_ext_keys = 0uL;
    nb_aes_ram_keys = 0uL;
    nb_aes_buffer_keys = 0uL;
    nb_ecc_ram_keys = 0uL;
    nb_hmac_keys = 0uL;
    nb_tls_master_keys = 0uL;
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));
    L_Toolbox_MemSet((uint8_t *)&key_map, 0x00u, (int16_t)sizeof(sys_cfg_key_mapping_t));
    L_Toolbox_MemSet((uint8_t *)g_cfg_aes_key_idx, 0x00u, (int16_t)sizeof(g_cfg_aes_key_idx));
    L_Toolbox_MemSet((uint8_t *)g_cfg_aes_ram_key_idx, 0x00u, (int16_t)sizeof(g_cfg_aes_ram_key_idx));
    L_Toolbox_MemSet((uint8_t *)g_cfg_aes_buffer_key_idx, 0x00u, (int16_t)sizeof(g_cfg_aes_buffer_key_idx));
    L_Toolbox_MemSet((uint8_t *)g_cfg_rsa_pri_key_idx, 0x00u, (int16_t)sizeof(g_cfg_rsa_pri_key_idx));
    L_Toolbox_MemSet((uint8_t *)g_cfg_rsa_pub_key_idx, 0x00u, (int16_t)sizeof(g_cfg_rsa_pub_key_idx));
    L_Toolbox_MemSet((uint8_t *)g_cfg_rsa_pub_ext_key_idx, 0x00u, (int16_t)sizeof(g_cfg_rsa_pub_ext_key_idx));
    L_Toolbox_MemSet((uint8_t *)g_cfg_ecc_pri_key_idx, 0x00u, (int16_t)sizeof(g_cfg_ecc_pri_key_idx));
    L_Toolbox_MemSet((uint8_t *)g_cfg_ecc_pub_key_idx, 0x00u, (int16_t)sizeof(g_cfg_ecc_pub_key_idx));
    L_Toolbox_MemSet((uint8_t *)g_cfg_ecc_pub_ext_key_idx, 0x00u, (int16_t)sizeof(g_cfg_ecc_pub_ext_key_idx));
    L_Toolbox_MemSet((uint8_t *)g_cfg_ecc_ram_key_idx, 0x00u, (int16_t)sizeof(g_cfg_ecc_ram_key_idx));
    L_Toolbox_MemSet((uint8_t *)g_cfg_hmac_key_idx, 0x00u, (int16_t)sizeof(g_cfg_hmac_key_idx));
    L_Toolbox_MemSet((uint8_t *)g_cfg_tls_master_key_idx, 0x00u, (int16_t)sizeof(g_cfg_tls_master_key_idx));

    (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);
    for (vm_id = 0uL; (vm_id < (uint32_t)vm_cfg.nb_vms) && (ER_OK == err); vm_id++)
    {
        /* NVM */
        (void)ML_CFG_GetSysVM((uint8_t)vm_id, ML_CFG_SYS_KEY_MAPPING, (uint32_t *)&key_map);
        /* Check wrap around for number of total of kemapping item for all VM */
        if ((ML_CFG_CHK_WRAP_AROUND_PLUS(nb_aes_keys,         key_map.nb_aes_keys,         ML_CFG_KEY_MAX_SLOTS)) ||
            (ML_CFG_CHK_WRAP_AROUND_PLUS(nb_rsa_priv_keys,    key_map.nb_rsa_priv_keys,    ML_CFG_KEY_MAX_SLOTS)) ||
            (ML_CFG_CHK_WRAP_AROUND_PLUS(nb_rsa_pub_keys,     key_map.nb_rsa_pub_keys,     ML_CFG_KEY_MAX_SLOTS)) ||
            (ML_CFG_CHK_WRAP_AROUND_PLUS(nb_ecc_priv_keys,    key_map.nb_ecc_priv_keys,    ML_CFG_KEY_MAX_SLOTS)) ||
            (ML_CFG_CHK_WRAP_AROUND_PLUS(nb_ecc_pub_keys,     key_map.nb_ecc_pub_keys,     ML_CFG_KEY_MAX_SLOTS)) ||
            (ML_CFG_CHK_WRAP_AROUND_PLUS(nb_rsa_pub_ext_keys, key_map.nb_rsa_pub_ext_keys, ML_CFG_KEY_MAX_SLOTS)) ||
            (ML_CFG_CHK_WRAP_AROUND_PLUS(nb_ecc_pub_ext_keys, key_map.nb_ecc_pub_ext_keys, ML_CFG_KEY_MAX_SLOTS)))
        {
            err = ER_ML_CFG_NVM_SIZE_ERROR;
        }
        /* RAM */
        else if ((ML_CFG_CHK_WRAP_AROUND_PLUS(nb_aes_ram_keys,    key_map.nb_aes_ram_keys,    ML_CFG_KEY_MAX_SLOTS)) ||
                 (ML_CFG_CHK_WRAP_AROUND_PLUS(nb_aes_buffer_keys, key_map.nb_aes_buffer_keys, ML_CFG_KEY_MAX_SLOTS)) ||
                 (ML_CFG_CHK_WRAP_AROUND_PLUS(nb_ecc_ram_keys,    key_map.nb_ecc_ram_keys,    ML_CFG_KEY_MAX_SLOTS)) ||
                 (ML_CFG_CHK_WRAP_AROUND_PLUS(nb_hmac_keys,       key_map.nb_hmac_keys,       ML_CFG_KEY_MAX_SLOTS)) ||
                 (ML_CFG_CHK_WRAP_AROUND_PLUS(nb_tls_master_keys, key_map.nb_tls_master_keys, ML_CFG_KEY_MAX_SLOTS)))
        {
            err = ER_ML_CFG_RAM_SIZE_ERROR;
        }
        else
        {
            /* Set offset of idx to access for each VM */
            g_cfg_aes_key_idx[vm_id] = (uint8_t)nb_aes_keys;
            g_cfg_aes_ram_key_idx[vm_id] = (uint8_t)nb_aes_ram_keys;
            g_cfg_aes_buffer_key_idx[vm_id] = (uint8_t)nb_aes_buffer_keys;
            g_cfg_rsa_pri_key_idx[vm_id] = (uint8_t)nb_rsa_priv_keys;
            g_cfg_rsa_pub_key_idx[vm_id] = (uint8_t)nb_rsa_pub_keys;
            g_cfg_rsa_pub_ext_key_idx[vm_id] = (uint8_t)nb_rsa_pub_ext_keys;
            g_cfg_ecc_pri_key_idx[vm_id] = (uint8_t)nb_ecc_priv_keys;
            g_cfg_ecc_pub_key_idx[vm_id] = (uint8_t)nb_ecc_pub_keys;
            g_cfg_ecc_pub_ext_key_idx[vm_id] = (uint8_t)nb_ecc_pub_ext_keys;
            g_cfg_ecc_ram_key_idx[vm_id] = (uint8_t)nb_ecc_ram_keys;
            g_cfg_hmac_key_idx[vm_id] = (uint8_t)nb_hmac_keys;
            g_cfg_tls_master_key_idx[vm_id] = (uint8_t)nb_tls_master_keys;
            /* Set number of total of kemapping for all VM */
            nb_aes_keys         += (uint32_t)key_map.nb_aes_keys;
            nb_aes_ram_keys     += (uint32_t)key_map.nb_aes_ram_keys;
            nb_aes_buffer_keys  += (uint32_t)key_map.nb_aes_buffer_keys;
            nb_rsa_priv_keys    += (uint32_t)key_map.nb_rsa_priv_keys;
            nb_rsa_pub_keys     += (uint32_t)key_map.nb_rsa_pub_keys;
            nb_ecc_priv_keys    += (uint32_t)key_map.nb_ecc_priv_keys;
            nb_ecc_pub_keys     += (uint32_t)key_map.nb_ecc_pub_keys;
            nb_rsa_pub_ext_keys += (uint32_t)key_map.nb_rsa_pub_ext_keys;
            nb_ecc_pub_ext_keys += (uint32_t)key_map.nb_ecc_pub_ext_keys;
            nb_ecc_ram_keys     += (uint32_t)key_map.nb_ecc_ram_keys;
            nb_hmac_keys        += (uint32_t)key_map.nb_hmac_keys;
            nb_tls_master_keys  += (uint32_t)key_map.nb_tls_master_keys;
        } /* end of else */
    } /* end of for (vm_id = 0uL; (vm_id < (uint32_t)vm_cfg.nb_vms) && (ER_OK == err); vm_id++) */

    if (ER_OK == err)
    {
        /* checks addr for ConfigRAM(AES ram key) */
        g_cfg_aes_ram_start_addr = end_addr;
        item_size = ML_CFG_SIZE_OF_ITEM((MH_KEY_AES_RAM_DATA_SIZE), ML_CFG_ALIGN4);
        /* checks the addr range for the ConfigRam number of the AES ram key */
        end_addr = end_addr + (item_size * nb_aes_ram_keys);
        if (ML_CFG_CONFIG_RAM_END_ADDR <= end_addr)
        {
            err = ER_ML_CFG_RAM_SIZE_ERROR;
        }
        else
        {
            /* Do nothing */
        }

        /* checks addr for ConfigRAM(AES buffer key) */
        if (ER_OK == err)
        {
            /* AES buffer key start addr (4byte alignment) */
            g_cfg_aes_buffer_start_addr = end_addr;
            item_size = ML_CFG_SIZE_OF_ITEM((MH_KEY_AES_BUFFER_DATA_SIZE), ML_CFG_ALIGN4);
            end_addr = end_addr + (item_size * nb_aes_buffer_keys);
            /* checks the addr range for the config number of the AES buffer key */
            if (ML_CFG_CONFIG_RAM_END_ADDR <= end_addr)
            {
                err = ER_ML_CFG_RAM_SIZE_ERROR;
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

        /* checks addr for ConfigRAM(ECC RAM key) */
        if (ER_OK == err)
        {
            /* ECC RAM key start addr (4byte alignment) */
            g_cfg_ecc_ram_start_addr = end_addr;
            item_size = ML_CFG_SIZE_OF_ITEM((MH_KEY_ECC_RAM_DATA_SIZE), ML_CFG_ALIGN4);
            end_addr = end_addr + (item_size * nb_ecc_ram_keys);
            /* checks the addr range for the config number of the ECC RAM key */
            if (ML_CFG_CONFIG_RAM_END_ADDR <= end_addr)
            {
                err = ER_ML_CFG_RAM_SIZE_ERROR;
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

        /* checks addr for ConfigRAM(HMAC key) */
        if (ER_OK == err)
        {
            /* HMAC key start addr (4byte alignment) */
            g_cfg_hmac_start_addr = end_addr;
            item_size = ML_CFG_SIZE_OF_ITEM((MH_KEY_HMAC_DATA_SIZE), ML_CFG_ALIGN4);
            end_addr = end_addr + (item_size * nb_hmac_keys);

            /* checks the addr range for the config number of the HMAC key */
            if (ML_CFG_CONFIG_RAM_END_ADDR <= end_addr)
            {
                err = ER_ML_CFG_RAM_SIZE_ERROR;
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

        /* checks addr for ConfigRAM(TLS master RAM key) */
        if (ER_OK == err)
        {
            /* TLS master key start addr (4byte alignment) */
            g_cfg_tls_master_start_addr = end_addr;
            item_size = ML_CFG_SIZE_OF_ITEM((MH_KEY_TLS_MASTER_DATA_SIZE), ML_CFG_ALIGN4);
            end_addr = end_addr + (item_size * nb_tls_master_keys);

            /* checks the addr range for the config number of the TLS master key */
            if (ML_CFG_CONFIG_RAM_END_ADDR <= end_addr)
            {
                err = ER_ML_CFG_RAM_SIZE_ERROR;
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
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    if (ER_OK == err)
    {
        /* Set the value of calculated key mapping dataset */
        p_key_map->nb_aes_keys = (uint8_t)nb_aes_keys;
        p_key_map->nb_rsa_priv_keys = (uint8_t)nb_rsa_priv_keys;
        p_key_map->nb_rsa_pub_keys = (uint8_t)nb_rsa_pub_keys;
        p_key_map->nb_ecc_priv_keys = (uint8_t)nb_ecc_priv_keys;
        p_key_map->nb_ecc_pub_keys = (uint8_t)nb_ecc_pub_keys;
        p_key_map->nb_rsa_pub_ext_keys = (uint8_t)nb_rsa_pub_ext_keys;
        p_key_map->nb_ecc_pub_ext_keys = (uint8_t)nb_ecc_pub_ext_keys;
        p_key_map->nb_aes_ram_keys = (uint8_t)nb_aes_ram_keys;
        p_key_map->nb_aes_buffer_keys = (uint8_t)nb_aes_buffer_keys;
        p_key_map->nb_ecc_ram_keys = (uint8_t)nb_ecc_ram_keys;
        p_key_map->nb_hmac_keys = (uint8_t)nb_hmac_keys;
        p_key_map->nb_tls_master_keys = (uint8_t)nb_tls_master_keys;
    }
    else
    {
        /* Do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ConfigMNG_0010]:[ICUMFW_UD_ML_ConfigMNG_0010]
* Function name: ML_CFG_SetIndexMappingVM
* Description  : Create an index map to access the resource of each VM.
* Return Value : ER_OK
*                ER_ML_CFG_RAM_SIZE_ERROR
*                ER_ML_CFG_NVM_SIZE_ERROR
***************************************************************************************/
uint32_t ML_CFG_SetIndexMappingVM(void)
{
    uint32_t err;
    sys_cfg_key_mapping_t key_map;

    /* Initialize local variable */
    err = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&key_map, 0x00u, (int16_t)sizeof(sys_cfg_key_mapping_t));

    /* Update keys index map of each VM by getting key mapping data */
    err = ML_CFG_GetKeyMappingData(&key_map);

    return err;
}
