/******************************************************************************/
/* Component Name   ML Config Manager public header file                      */
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

#if !defined(ICUM_ML_CFG_PUB_H)
#define ICUM_ML_CFG_PUB_H

/********************************************************************************
 Link to external constants
********************************************************************************/
extern uint8_t  __ghsbegin_start_icum_config_ram[];
extern uint8_t  __ghsbegin_end_icum_config_ram[];

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Return Value Definition                                                            */
#define ER_ML_CFG_RAM_SIZE_ERROR          (ER_ML_CFG_BASE + (0uL))
#define ER_ML_CFG_NVM_SIZE_ERROR          (ER_ML_CFG_BASE + (1uL))

/* system configuration ID */
#define ML_CFG_SYS_FW_NAME                (0u)
#define ML_CFG_SYS_VERIF_METHOD           (3u)
#define ML_CFG_SYS_JOB_SLICE              (4u)
#define ML_CFG_SYS_SANCTION               (5u)
#define ML_CFG_SYS_KEY_MAPPING            (6u)
#define ML_CFG_SYS_MYDATA                 (7u)
#define ML_CFG_SYS_SERV_PATCH_AUTH        (9u)
#define ML_CFG_SYS_SERV_EXP_AUTH          (10u)
#define ML_CFG_SYS_WDT                    (11u)
#define ML_CFG_SYS_CMAC_MIN               (12u)
#define ML_CFG_SYS_SECURE_BOOT            (13u)
#define ML_CFG_SYS_DEBUG                  (14u)
#define ML_CFG_SYS_RAND                   (15u)
#define ML_CFG_SYS_SECURE_TICK            (17u)
#define ML_CFG_SYS_USED_AC                (18u)
#define ML_CFG_SYS_START_PE               (19u)
#define ML_CFG_SYS_TEST_ECC_LOCK          (20u)
#define ML_CFG_SYS_FLS                    (21u)
#define ML_CFG_SYS_INTERRUPT              (23u)
#define ML_CFG_SYS_VM                     (24u)

/* system config nvm block size definition */
#define ML_CFG_DS_NB_BLOCKS               (0x0006u)

/* definition total number of VMs min or max */
#define ML_CFG_TOTAL_NB_VMS_MIN           ((uint8_t)1u)
#define ML_CFG_TOTAL_NB_VMS_MAX           ((uint8_t)17u)

/* definition master VM id */
#define ML_CFG_MASTER_VM_ID               (0x00u)

/* definition key max slot */
#define ML_CFG_KEY_MAX_SLOTS              (256uL)
/* definition key max slot 1000 */
#define ML_CFG_KEY_MAX_1000               (1000u)

/* defintion wrap around check plus */
#define ML_CFG_CHK_WRAP_AROUND_PLUS(a, b, max)    (((uint32_t)(a) > ((uint32_t)(a) + (uint32_t)(b))) || \
                                                   ((uint32_t)(max) <= ((uint32_t)(a) + (uint32_t)(b))))

/* definition start - end address (config RAM) */
#define ML_CFG_CONFIG_RAM_START_ADDR      ((uint32_t)__ghsbegin_start_icum_config_ram)
#define ML_CFG_CONFIG_RAM_END_ADDR        ((uint32_t)__ghsbegin_end_icum_config_ram)

/********************************************************************************
 Exported global variables (extern).
********************************************************************************/
extern uint8_t g_cfg_aes_key_idx[ML_CFG_TOTAL_NB_VMS_MAX];
extern uint8_t g_cfg_aes_ram_key_idx[ML_CFG_TOTAL_NB_VMS_MAX];
extern uint8_t g_cfg_aes_buffer_key_idx[ML_CFG_TOTAL_NB_VMS_MAX];
extern uint8_t g_cfg_rsa_pri_key_idx[ML_CFG_TOTAL_NB_VMS_MAX];
extern uint8_t g_cfg_rsa_pub_key_idx[ML_CFG_TOTAL_NB_VMS_MAX];
extern uint8_t g_cfg_rsa_pub_ext_key_idx[ML_CFG_TOTAL_NB_VMS_MAX];
extern uint8_t g_cfg_ecc_pri_key_idx[ML_CFG_TOTAL_NB_VMS_MAX];
extern uint8_t g_cfg_ecc_pub_key_idx[ML_CFG_TOTAL_NB_VMS_MAX];
extern uint8_t g_cfg_ecc_pub_ext_key_idx[ML_CFG_TOTAL_NB_VMS_MAX];
extern uint8_t g_cfg_ecc_ram_key_idx[ML_CFG_TOTAL_NB_VMS_MAX];
extern uint8_t g_cfg_hmac_key_idx[ML_CFG_TOTAL_NB_VMS_MAX];
extern uint8_t g_cfg_tls_master_key_idx[ML_CFG_TOTAL_NB_VMS_MAX];

extern uint32_t g_cfg_aes_ram_start_addr;
extern uint32_t g_cfg_aes_buffer_start_addr;
extern uint32_t g_cfg_ecc_ram_start_addr;
extern uint32_t g_cfg_hmac_start_addr;
extern uint32_t g_cfg_tls_master_start_addr;

/********************************************************************************
 Macro definitions
********************************************************************************/
#define ML_CFG_VM_AES_IDX_OFFSET(vm_id)             (g_cfg_aes_key_idx[vm_id])
#define ML_CFG_VM_AES_RAM_IDX_OFFSET(vm_id)         (g_cfg_aes_ram_key_idx[vm_id])
#define ML_CFG_VM_AES_BUFFER_IDX_OFFSET(vm_id)      (g_cfg_aes_buffer_key_idx[vm_id])
#define ML_CFG_VM_RSA_PRI_IDX_OFFSET(vm_id)         (g_cfg_rsa_pri_key_idx[vm_id])
#define ML_CFG_VM_RSA_PUB_IDX_OFFSET(vm_id)         (g_cfg_rsa_pub_key_idx[vm_id])
#define ML_CFG_VM_RSA_PUB_EXT_IDX_OFFSET(vm_id)     (g_cfg_rsa_pub_ext_key_idx[vm_id])
#define ML_CFG_VM_ECC_PRI_IDX_OFFSET(vm_id)         (g_cfg_ecc_pri_key_idx[vm_id])
#define ML_CFG_VM_ECC_PUB_IDX_OFFSET(vm_id)         (g_cfg_ecc_pub_key_idx[vm_id])
#define ML_CFG_VM_ECC_PUB_EXT_IDX_OFFSET(vm_id)     (g_cfg_ecc_pub_ext_key_idx[vm_id])
#define ML_CFG_VM_ECC_RAM_IDX_OFFSET(vm_id)         (g_cfg_ecc_ram_key_idx[vm_id])
#define ML_CFG_VM_HMAC_IDX_OFFSET(vm_id)            (g_cfg_hmac_key_idx[vm_id])
#define ML_CFG_VM_TLS_MASTER_IDX_OFFSET(vm_id)      (g_cfg_tls_master_key_idx[vm_id])

#define ML_CFG_AES_RAM_START_ADDR                   (g_cfg_aes_ram_start_addr)
#define ML_CFG_AES_BUFFER_START_ADDR                (g_cfg_aes_buffer_start_addr)
#define ML_CFG_ECC_RAM_START_ADDR                   (g_cfg_ecc_ram_start_addr)
#define ML_CFG_HMAC_START_ADDR                      (g_cfg_hmac_start_addr)
#define ML_CFG_TLS_MASTER_START_ADDR                (g_cfg_tls_master_start_addr)

/**********************************************************************************
* Function prototypes
**********************************************************************************/
uint32_t ML_CFG_Init(void);
uint32_t ML_CFG_GetSys(uint16_t sys_id, uint32_t *p_sys_cfg);
uint32_t ML_CFG_GetSys_ram(uint16_t sys_id, uint32_t *p_sys_cfg);
uint32_t ML_CFG_SetSys(uint16_t sys_id, uint32_t *p_new_sys_cfg);
uint32_t ML_CFG_GetSysAllConfig(uint32_t **p_sys_all_cfg, uint16_t *p_sys_size);
uint32_t ML_CFG_GetSysVM(uint8_t vm_id, uint16_t sys_id, uint32_t *p_sys_cfg);
uint32_t ML_CFG_GetSysVM_ram(uint8_t vm_id, uint16_t sys_id, uint32_t *p_sys_cfg);
uint32_t ML_CFG_SetSysVM(uint8_t vm_id, uint16_t sys_id, uint32_t *p_new_sys_cfg);
uint32_t ML_CFG_GetKeyMappingData(sys_cfg_key_mapping_t *p_key_map);
uint32_t ML_CFG_SetIndexMappingVM(void);

#endif /* ICUM_ML_CFG_PUB_H */
