/******************************************************************************/
/* Component Name  Flash Manager                                              */
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

#if !defined(ICUM_MM_FLASH_PUB_H)
#define ICUM_MM_FLASH_PUB_H

/**************************************************************************************/
/* Headers                                                                            */
/**************************************************************************************/
#include "r_icumif_api.h"
#include "icum_l_flash_pub.h"

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Return Value Definition */

/* error detection from Flash operation except FACI */
#define ER_MM_FLS_SYS                   (ER_MM_FLS_BASE + 0x00000000uL)
/* error detection from FACI */
#define ER_MM_FLS_SYS_CRITICAL          (ER_MM_FLS_BASE + 0x00000001uL)
/* error detection in flash memory programming */
#define ER_MM_FLS_PROG                  (ER_MM_FLS_BASE + 0x00000002uL)
/* data flash is two-side broken */
#define ER_MM_FLS_DATA_INVALID          (ER_MM_FLS_BASE + 0x00000003uL)
 /* data flash is two-side blank */
#define ER_MM_FLS_DATA_BLANK            (ER_MM_FLS_BASE + 0x00000004uL)
 /* AES is unavailable */
#define ER_MM_FLS_AES_UNAVAILABLE       (ER_MM_FLS_BASE + 0x00000005uL)
/* Flash unusable (no Ack from PE side etc.) */
#define ER_MM_FLS_UNAVAILABLE           (ER_MM_FLS_BASE + 0x00000006uL)
/* data flash memory is size over or size is zero */
#define ER_MM_FLS_SIZE                  (ER_MM_FLS_BASE + 0x00000007uL)
/* source address error */
#define ER_MM_FLS_MEM_SOURCE            (ER_MM_FLS_BASE + 0x00000008uL)
/* destination address error */
#define ER_MM_FLS_MEM_DEST              (ER_MM_FLS_BASE + 0x00000009uL)
/* processing in progress(read/write/erase) */
#define ER_MM_FLS_IN_PROGRESS           (ER_MM_FLS_BASE + 0x0000000AuL)
/* AES timeout error occurred */
#define ER_MM_FLS_AES_TIME_OUT          (ER_MM_FLS_BASE + 0x0000000BuL)
/* data slot is not mapping */
#define ER_MM_FLS_AES_PRM_ERROR         (ER_MM_FLS_BASE + 0x0000000EuL)
/* data map is full */
#define ER_MM_FLS_FULL_DATA_MAP         (ER_MM_FLS_BASE + 0x00000011uL)
/* unknown flash state */
#define ER_MM_FLS_FORMAT_ERR            (ER_MM_FLS_BASE + 0x00000012uL)
/* flash memory is not formatted yet */
#define ER_MM_FLS_ALL_BLANK             (ER_MM_FLS_BASE + 0x00000013uL)

/* target flash memory */
#define FLS_SELECT_CF                   ((uint8_t)0x00)
#define FLS_SELECT_DF                   ((uint8_t)0x01)
#define FLS_SELECT_ICUM_DF              ((uint8_t)0x02)

/* target flash area */
#define FLS_SELECT_USER                 ((uint8_t)0x00)
#define FLS_SELECT_SECURE               ((uint8_t)0x01)

/* Kind of data set */
#define FLS_DS_SYS_CFG                  ((uint8_t)0x00)
#define FLS_DS_AES_KEY                  ((uint8_t)0x03)
#define FLS_DS_CR_KEY                   ((uint8_t)0x04)
#define FLS_DS_RSA_ROM_PUB              ((uint8_t)0x05)
#define FLS_DS_RSA_PRIV_KEY             ((uint8_t)0x06)
#define FLS_DS_RSA_PUB_KEY              ((uint8_t)0x07)
#define FLS_DS_ECC_ROM_PUB              ((uint8_t)0x08)
#define FLS_DS_ECC_PRIV_KEY             ((uint8_t)0x09)
#define FLS_DS_ECC_PUB_KEY              ((uint8_t)0x0A)
#define FLS_DS_MYDATA                   ((uint8_t)0x0B)
#define FLS_DS_RSA_PUB_EXT_KEY          ((uint8_t)0x0C)
#define FLS_DS_ECC_PUB_EXT_KEY          ((uint8_t)0x0D)
#define FLS_DS_NB_DATA_SET              ((uint8_t)0x10)
                                                       /* number of data set */
#define FLS_DS_INIT                     ((uint8_t)0xFF)
                                                       /* invalid data set type */

#define FLS_DATA_SLOT_MGMT_SIZE         (24uL)
    /* DVC(16byte) + ds_type(1byte) + slot_num(1byte) + data_size(2byte) + update_counter(4byte) */

/* number of copies */
#define FLS_DS_NB_COPIES                ((uint16_t)0x0002)

/* direction select */
#define FLS_CHK_MEM_SOURCE              L_FLS_CHK_MEM_SOURCE
                                                        /* specify a source address */
#define FLS_CHK_MEM_DESTINATION         L_FLS_CHK_MEM_DESTINATION
                                                        /* specify a destination address */

/* for memory boundaries */
#define MM_FLS_LOWER_LIMIT_CF_SIDEA        L_FLS_LOWER_LIMIT_CF_SIDEA
#define MM_FLS_LOWER_LIMIT_SCF_DOUBLE      L_FLS_LOWER_LIMIT_SCF_DOUBLE
#define MM_FLS_UPPER_LIMIT_CF_SIDEA        L_FLS_UPPER_LIMIT_CF_SIDEA
#define MM_FLS_LOWER_LIMIT_CF_SIDEB        L_FLS_LOWER_LIMIT_CF_SIDEB
#define MM_FLS_LOWER_LIMIT_SCF_SINGLE      L_FLS_LOWER_LIMIT_SCF_SINGLE
#define MM_FLS_UPPER_LIMIT_CF_SIDEB        L_FLS_UPPER_LIMIT_CF_SIDEB

#define MM_FLS_LOWER_LIMIT_CRAM            L_FLS_LOWER_LIMIT_CRAM
#define MM_FLS_UPPER_LIMIT_CRAM            L_FLS_UPPER_LIMIT_CRAM
#define MM_FLS_LOWER_LIMIT_SLRAM           L_FLS_LOWER_LIMIT_SLRAM
#define MM_FLS_UPPER_LIMIT_SLRAM           L_FLS_UPPER_LIMIT_SLRAM

#define MM_FLS_LOWER_LIMIT_DF_BANKA        L_FLS_LOWER_LIMIT_DF_BANKA
#define MM_FLS_UPPER_LIMIT_DF_BANKA        L_FLS_UPPER_LIMIT_DF_BANKA
#define MM_FLS_LOWER_LIMIT_DF_BANKB        L_FLS_LOWER_LIMIT_DF_BANKB
#define MM_FLS_UPPER_LIMIT_DF_BANKB        L_FLS_UPPER_LIMIT_DF_BANKB
#define MM_FLS_LOWER_LIMIT_DF_SDF          L_FLS_LOWER_LIMIT_DF_SDF
#define MM_FLS_UPPER_LIMIT_DF_SDF          L_FLS_UPPER_LIMIT_DF_SDF

#define MM_FLS_LOWER_LIMIT_SCF_SIDEA       L_FLS_LOWER_LIMIT_SCF_SIDEA
#define MM_FLS_UPPER_LIMIT_SCF_SIDEA       L_FLS_UPPER_LIMIT_SCF_SIDEA
#define MM_FLS_LOWER_LIMIT_SCF_SIDEB       L_FLS_LOWER_LIMIT_SCF_SIDEB
#define MM_FLS_UPPER_LIMIT_SCF_SIDEB       L_FLS_UPPER_LIMIT_SCF_SIDEB

#define MM_FLS_LOWER_LIMIT_FIRMWARE        L_FLS_LOWER_LIMIT_FIRMWARE
#define MM_FLS_UPPER_LIMIT_FIRMWARE        L_FLS_UPPER_LIMIT_FIRMWARE
#define MM_FLS_LOWER_LIMIT_FIRMWARE_A      L_FLS_LOWER_LIMIT_FIRMWARE_A
#define MM_FLS_UPPER_LIMIT_FIRMWARE_A      L_FLS_UPPER_LIMIT_FIRMWARE_A
#define MM_FLS_LOWER_LIMIT_FIRMWARE_B      L_FLS_LOWER_LIMIT_FIRMWARE_B
#define MM_FLS_UPPER_LIMIT_FIRMWARE_B      L_FLS_UPPER_LIMIT_FIRMWARE_B

#define MM_FLS_LOWER_LIMIT_SECRET_DATA_SECTION  L_FLS_LOWER_LIMIT_SECRET_DATA_SECTION
#define MM_FLS_UPPER_LIMIT_SECRET_DATA_SECTION  L_FLS_UPPER_LIMIT_SECRET_DATA_SECTION

#define MM_FLS_LOWER_LIMIT_CF_BANKA        L_FLS_LOWER_LIMIT_CF_BANKA
#define MM_FLS_UPPER_LIMIT_CF_BANKA        L_FLS_UPPER_LIMIT_CF_BANKA
#define MM_FLS_LOWER_LIMIT_CF_BANKB        L_FLS_LOWER_LIMIT_CF_BANKB
#define MM_FLS_UPPER_LIMIT_CF_BANKB        L_FLS_UPPER_LIMIT_CF_BANKB
#define MM_FLS_LOWER_LIMIT_CF_BANKC        L_FLS_LOWER_LIMIT_CF_BANKC
#define MM_FLS_UPPER_LIMIT_CF_BANKC        L_FLS_UPPER_LIMIT_CF_BANKC
#define MM_FLS_LOWER_LIMIT_CF_BANKD        L_FLS_LOWER_LIMIT_CF_BANKD
#define MM_FLS_UPPER_LIMIT_CF_BANKD        L_FLS_UPPER_LIMIT_CF_BANKD
#define MM_FLS_LOWER_LIMIT_CF_BANKE        L_FLS_LOWER_LIMIT_CF_BANKE
#define MM_FLS_UPPER_LIMIT_CF_BANKE        L_FLS_UPPER_LIMIT_CF_BANKE
#define MM_FLS_LOWER_LIMIT_CF_BANKF        L_FLS_LOWER_LIMIT_CF_BANKF
#define MM_FLS_UPPER_LIMIT_CF_BANKF        L_FLS_UPPER_LIMIT_CF_BANKF


/* for program of option byte */
#define FLS_PROG_ICUMUPGP               ((uint8_t)0x00)
#define FLS_PROG_ICUMRESV               ((uint8_t)0x01)
#define FLS_PROG_ICUMRACS0              ((uint8_t)0x02)
#define FLS_PROG_ICUMRAD                ((uint8_t)0x03)
#define FLS_PROG_CR_RES                 ((uint8_t)0x04)
#define FLS_PROG_DEGRADATION            ((uint8_t)0x05)
#define FLS_PROG_ICUMRACE0              ((uint8_t)0x06)
#define FLS_PROG_ICUMRACS1              ((uint8_t)0x07)
#define FLS_PROG_ICUMRACE1              ((uint8_t)0x08)

#define FLS_PROG_OPBT_CR_ICUP_SKIP      (0x00000001uL)
#define FLS_PROG_OPBT_CR_CPU_SKIP       (0x00000002uL)
#define FLS_PROG_OPBT_CR_ALL_SKIP       (0x00000003uL)
#define FLS_PROG_OPBT_CR_ICUP_ENABLED   (0x00000004uL)
#define FLS_PROG_OPBT_CR_CPU_ENABLED    (0x00000005uL)
#define FLS_PROG_OPBT_CR_ALL_ENABLE     (0x00000006uL)
#define FLS_PROG_OPBT_UPGP1_ENABLE      (0x00000007uL)
#define FLS_PROG_OPBT_UPGP1_DISABLE     (0x00000008uL)
#define FLS_PROG_OPBT_UPGP2_ENABLE      (0x00000009uL)
#define FLS_PROG_OPBT_UPGP2_DISABLE     (0x0000000AuL)
#define FLS_PROG_OPBT_PEDBG_ENABLE      (0x0000000BuL)
#define FLS_PROG_OPBT_PEDBG_DISABLE     (0x0000000CuL)
#define FLS_PROG_OPBT_GPIO_DISABLE      (0x0000000DuL)
#define FLS_PROG_OPBT_GPIO_ENABLE       (0x0000000EuL)
#define FLS_PROG_OPBT_RHSIF_NOT_NEED    (0x0000000FuL)
#define FLS_PROG_OPBT_RHSIF_NEED        (0x00000011uL)
#define FLS_PROG_OPBT_WDTA_DISABLE      (0x00000012uL)
#define FLS_PROG_OPBT_WDTA_ENABLE       (0x00000013uL)
#define FLS_PROG_DEGRADATION_DCF        (0x00000014uL)

/* Flash check result */
#define FLS_CHK_OK                      L_FLS_CHK_OK
#define FLS_CHK_NG                      L_FLS_CHK_NG

/* Flash block definition */
#define FLS_DF_BLOCK_SIZE               (64uL)
#define FLS_NB_BLOCK                    (L_FLS_NB_BLOCK - 1uL)
#define FLS_ERASE_BLOCK_SIZE            (L_FLS_ERASE_BLOCK_SIZE)
#define FLS_NB_SDF_SLOT                 ((1920uL / FLS_DF_BLOCK_SIZE) * (FLS_NB_BLOCK - 1uL))

/* max size of data set   */
/* see icum_mh_key_pub.h  */
#define FLS_SLOT_MAX                    (FLS_DF_BLOCK_SIZE * (uint32_t)MH_KEY_RSA_PRIV_DS_NB_BLOCKS)

/* for NVM Data Warning flag information */
  /* NVM Data Warning not occurred */
#define NVM_DATA_NO_WARNING             ((uint8_t)0x00u)
  /* NVM Data Warning occurred */
#define NVM_DATA_WARNING                ((uint8_t)0x01u)

/* Garbage Collection Warning information */
#define FLS_NO_FULL_WARNING             (NVM_DATA_NO_WARNING)
#define FLS_FULL_WARNING1               ((uint8_t)0x02u)
#define FLS_FULL_WARNING2               ((uint8_t)0x03u)

/* for NVM Data Warning information */
  /* NVM Data Warning ds_type initialize */
#define NVM_WARN_DSTYPE_INIT            (0xFFu)
  /* NVM Data Warning slot_num initialize */
#define NVM_WARN_SLOTNUM_INIT           (0xFFu)
  /* MAX NVM Warning Data buffer number */
#define NVM_WARN_DATA_NUM               (16uL)

/* GC warning level default value */
#define FLS_DEFAULT_WARNING1_LEVEL      ((uint16_t)0x000Fu)
#define FLS_DEFAULT_WARNING2_LEVEL      ((uint16_t)0x001Eu)

/* Partition Infomation */
#define FLS_PINFO_A                 (0x00u)
#define FLS_PINFO_B                 (0x01u)
#define FLS_PINFO_COUNTER           (0x01uL)
#define FLS_PINFO_ACTIVE            (0x02uL)
#define FLS_PINFO_REVOKE            (0x04uL)
#define FLS_PINFO_INVALID           (0x08uL)

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Type definition                                                                    */
/**************************************************************************************/

/* Block index in Flash memory */
/* note: a block is the smallest erasable unit in Flash */
typedef uint16_t flash_block_idx_t;

/* Word index in Flash block */
/* note: a word is the smallest writable unit in Flash */
typedef uint16_t flash_word_idx_t;

/* type of garbage collection warning information */
typedef uint8_t gc_warning_t;

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/

/* Flash location : combination of block index and word index for secure data flash only */
/* when specify the code flash and the user data flash, set to direct_addr */
typedef struct flash_location
{
    flash_block_idx_t   block_idx;
    flash_word_idx_t    word_idx;
} flash_location_t;

/* ring buffer elements of NVM Data Warning information */
typedef struct nvm_warning_data
{
    /* Kind of data set */
    uint8_t ds_type;

    /* Slot index */
    uint16_t slot_index;
} nvm_warning_data_t;

/* NVM Data Warning information ring buffer */
typedef struct nvm_warning_buff
{
    nvm_warning_data_t data[NVM_WARN_DATA_NUM];
} nvm_warning_buff_t;

/* Number of free logical blocks information */
typedef struct free_block
{
    /* Total number of free logical blocks */
    flash_block_idx_t   total_free_block_num;

    /* Number of free logical blocks per physical block */
    flash_block_idx_t   free_block_num[FLS_NB_BLOCK];
} free_block_t;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
uint32_t MM_FLS_Init(void);

uint32_t MM_FLS_Read(uint8_t target_flash, uint8_t target_area,
                     uint8_t ds_type, uint8_t slot_num,
                     uint16_t size, uint8_t *p_data);
uint32_t MM_FLS_Write(r_icumif_job_id_t job_id, uint8_t target_flash,
                      uint8_t target_area, uint8_t ds_type, uint8_t slot_num,
                      uint16_t size, uint8_t *p_data);
uint32_t MM_FLS_Erase(r_icumif_job_id_t job_id, uint8_t target_flash,
                      uint8_t target_area, uint8_t ds_type, uint8_t slot_num);
uint32_t MM_FLS_WriteFlashCfg(r_icumif_job_id_t job_id, uint8_t target_func, uint32_t set_data);

uint32_t MM_FLS_DeclareDataSet(uint8_t ds_type, uint8_t nb_slots, uint16_t nb_blocks_per_slot,
                               uint16_t nb_flash_copies);
uint32_t MM_FLS_MapDataSet(void);

uint32_t MM_FLS_CheckMemory(const uint32_t start_addr, const uint32_t size, const uint8_t direction,
                            uint8_t ex_service, uint8_t *p_result);
uint32_t MM_FLS_CheckMemoryCISM(const uint32_t start_addr, const uint32_t size, const uint8_t direction,
                                uint8_t *p_result);
uint32_t MM_FLS_IsChangeCFPE(uint8_t *p_result);
uint32_t MM_FLS_GetMemBoundary(const uint8_t boundary, uint32_t *p_addr);
uint32_t MM_FLS_CheckMemoryUserBoot(uint32_t start_addr, uint32_t size, uint8_t direction, uint8_t *p_result);
uint32_t MM_FLS_CheckMemoryCISMUserBoot(uint32_t start_addr, uint32_t size, uint8_t direction, uint8_t *p_result);
uint32_t MM_FLS_CheckDataSetSize(uint8_t *p_result);

/* NVM Data Warning */
uint32_t MM_FLS_GetNVMWarnFlg(uint8_t *p_nvm_warn_flg);
uint32_t MM_FLS_ClrNVMWarnFlg(void);
uint32_t MM_FLS_GetNVMWarnData(nvm_warning_buff_t *p_nvm_warn_buff);

/* Code Flash P/E */
uint32_t MM_FLS_EraseCF(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority, uint8_t target_area,
                        const uint32_t cf_addr, uint32_t size);
uint32_t MM_FLS_WriteCF(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority, uint8_t target_area,
                        const uint32_t cf_addr, uint32_t size, const uint32_t *p_data);

uint32_t MM_FLS_CollectGarbage(r_icumif_job_id_t job_id);
uint32_t MM_FLS_CollectGarbageAll(r_icumif_job_id_t job_id);
uint32_t MM_FLS_DeleteMapData(uint8_t ds_type, uint8_t slot_num);
uint8_t MM_FLS_GET_FormatPhase(void);
uint8_t MM_FLS_GET_WritePhase(void);
uint32_t MM_FLS_Format(r_icumif_job_id_t job_id);

uint32_t MM_FLS_GetMaxSlotNum(const uint8_t ds_type, uint8_t *p_nb_slots);
uint32_t MM_FLS_Config(void);
uint32_t MM_FLS_GetFreeCapacity(free_block_t *p_free_block);

uint32_t MM_FLS_RemapStart(const uint32_t p_uiTargetAddr, uint32_t **p_uiRemapAddr);
void     MM_FLS_RemapEnd(void);
uint32_t MM_FLS_IsUserBootArea(const uint32_t start_addr, const uint32_t size);

/* User area access APIs */
uint32_t MM_FLS_CheckRamExecRequired(uint8_t target_flash, uint32_t *p_addr, uint8_t *p_result);
uint32_t MM_FLS_ReadUser(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority, uint8_t target_flash,
                         uint8_t target_area, uint32_t *p_addr, uint32_t size, uint8_t *p_data);

uint32_t MM_FLS_EraseUser(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority, uint8_t target_flash,
                          uint8_t target_area, uint32_t *p_addr, uint32_t size);

uint32_t MM_FLS_WriteUser(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority, uint8_t target_flash,
                          uint8_t target_area, uint32_t *p_addr, uint32_t size, const uint32_t *p_data);

uint32_t MM_FLS_BlankCheckUser(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                               uint8_t target_flash, uint8_t target_area, uint32_t *p_addr, uint32_t size,
                               uint8_t *p_is_blank);

/* Flash resource get/release APIs */
uint32_t MM_FLS_GetFlashResource(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                                 uint8_t target_flash, uint8_t target_area, uint32_t addr, uint32_t size);
uint32_t MM_FLS_FreeFlashResource(uint8_t target_flash, uint8_t target_area, uint32_t addr, uint32_t size);

uint32_t MM_FLS_WritePartitionInfo(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                                   uint8_t target, uint32_t info, uint32_t *p_data);
uint32_t MM_FLS_ReadPartitionInfo(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                                  uint8_t target, uint32_t *p_info, uint32_t *p_data);

/* #if !defined(ICUM_MM_FLASH_PUB_H) */
#endif
