/******************************************************************************/
/* Component Name   ICUM FLS Library                                          */
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

#if !defined(ICUM_L_FLASH_PUB_H)
#define ICUM_L_FLASH_PUB_H


/********************************************************************************
 Includes <System Includes> , "Project Includes"
********************************************************************************/
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "icum_d_flash_pub.h"

/********************************************************************************
 System Macro definitions
********************************************************************************/

/********************************************************************************
 Macro definitions
********************************************************************************/
/* Target area accessed by FlashLib(arg) */
#define AG_L_FLS_CODE                   AG_D_FLS_CODE
                                        /* Code Flash */
#define AG_L_FLS_DATA                   AG_D_FLS_DATA
                                        /* Data Flash */
#define AG_L_FLS_SECURITY               AG_D_FLS_SECURITY
                                        /* Security setting area */
/* Target bank accessed by FlashLib(arg) */
#define AG_L_FLS_BANK_EXT               AG_D_FLS_BANK_EXT
                                        /* Extended area (Config or Security) */
#define AG_L_FLS_BANK_ICUM              AG_D_FLS_BANK_ICUM
                                        /* Bank for ICUM */

/* Option byte accessed by FlashLib(arg) */
#define AG_L_FLS_ICUMUPGP               AG_D_FLS_ICUMUPGP
                                        /* UPGP setting */
#define AG_L_FLS_ICUMRESV               AG_D_FLS_ICUMRESV
                                        /* ICUMD reset vector */
#define AG_L_FLS_ICUMRACS0              AG_D_FLS_ICUMRACS0
                                        /* ICUMD secure start address for code flash area region 0. */
#define AG_L_FLS_ICUMRACE0              AG_D_FLS_ICUMRACE0
                                        /* ICUMD secure end address for code flash area region 0. */
#define AG_L_FLS_ICUMRACS1              AG_D_FLS_ICUMRACS1
                                        /* ICUMD secure start address for code flash area region 1. */
#define AG_L_FLS_ICUMRACE1              AG_D_FLS_ICUMRACE1
                                        /* ICUMD secure end address for code flash area region 1. */
#define AG_L_FLS_ICUMRAD                AG_D_FLS_ICUMRAD
                                        /* ICUMD secure address for data flash */
#define AG_L_FLS_CHGRESP                AG_D_FLS_CHGRESP
                                        /* ICUMD Challenge & response, WDT related settings */
#define AG_L_FLS_DEGRADATION            AG_D_FLS_DEGRADATION
                                        /* DEGRADATION setting */
#define AG_L_FLS_OPBT_UPGP1_ENABLE      AG_D_FLS_ICUMUPGP1_OFF
                                        /* UPGP1 is enable, setting value is Other all 1 */
#define AG_L_FLS_OPBT_UPGP1_DISABLE     AG_D_FLS_ICUMUPGP1_ON
                                        /* UPGP2 is disable, setting value is all 1 */
#define AG_L_FLS_OPBT_UPGP2_ENABLE      AG_D_FLS_ICUMUPGP2_OFF
                                        /* UPGP2 is enable, setting value is Other all 1 */
#define AG_L_FLS_OPBT_UPGP2_DISABLE     AG_D_FLS_ICUMUPGP2_ON
                                        /* UPGP2 is disable, setting value is all 1 */
#define AG_L_FLS_CR_RES_ICUP_SKIPPED    AG_D_FLS_CR_RES_ICUP_SKIPPED
                                        /* C&R authentication is skipped for ICUP */
#define AG_L_FLS_CR_RES_CPU_SKIPPED     AG_D_FLS_CR_RES_CPU_SKIPPED
                                        /* C&R authentication is skipped for CPU */
#define AG_L_FLS_CR_RES_ALL_SKIPPED     AG_D_FLS_CR_RES_ALL_SKIPPED
                                        /* C&R authentication is skipped for ICUP & CPU */
#define AG_L_FLS_CR_RES_ICUP_ENABLED    AG_D_FLS_CR_RES_ICUP_ENABLED
                                        /* C&R authentication is enabled for ICUP */
#define AG_L_FLS_CR_RES_CPU_ENABLED     AG_D_FLS_CR_RES_CPU_ENABLED
                                        /* C&R authentication is enabled for CPU */
#define AG_L_FLS_CR_RES_ALL_ENABLED     AG_D_FLS_CR_RES_ALL_ENABLED
                                        /* C&R authentication is enabled for ICUP & CPU */
#define AG_L_FLS_OPBT_PEDBG_DISABLE     AG_D_FLS_ICUMPEDBG_ON
                                        /* ICUMPEDBG is disable */
#define AG_L_FLS_OPBT_PEDBG_ENABLE      AG_D_FLS_ICUMPEDBG_OFF
                                        /* ICUMPEDBG is enable */
#define AG_L_FLS_OPBT_GPIO_ENABLE       AG_D_FLS_ICUMOPGPIO_ON
                                        /* ICUMOPGPIO is enable */
#define AG_L_FLS_OPBT_GPIO_DISABLE      AG_D_FLS_ICUMOPGPIO_OF
                                        /* ICUMOPGPIO is disable */
#define AG_L_FLS_OPBT_RHSIF_NEED        AG_D_FLS_ICUMHSIF_ON
                                        /* ICUMHSIF C&R is need */
#define AG_L_FLS_OPBT_RHSIF_NOT_NEED    AG_D_FLS_ICUMHSIF_OFF
                                        /* ICUMHSIF C&R is no need */
#define AG_L_FLS_OPBT_WDTA_ENABLE       AG_D_FLS_ICUMOPWDEN_ON
                                        /* ICUMWDT is enable */
#define AG_L_FLS_OPBT_WDTA_DISABLE      AG_D_FLS_ICUMOPWDEN_OFF
                                        /* ICUMWDT is disable */
#define AG_L_FLS_DEGRADATION_DCF        AG_D_FLS_DEGRADATION_DCF
                                        /* DEGARADATION Service */
/*
 FlashLib return value
*/

#define ER_L_FLS_FLOW           (ER_L_FLS_BASE | (0x00000020UL))
           /* The current request was not executed because the initialization has not been executed. */

#define ER_L_FLS_PROTECTION     (ER_L_FLS_BASE | (0x00000022UL))
           /* The requested command was not executed because the security function is valid */
           /* (erase protection is enabled, lock bit set, etc.). */
#define ER_L_FLS_REJECTED       (ER_L_FLS_BASE | (0x00000023UL))
           /* The requested command Req was not executed because another command Req is in operation. */
           /* The flash mode was already in other than the read mode, */
           /* but tried to set the mode other than the read mode. */
#define ER_L_FLS_PROGRAM        (ER_L_FLS_BASE | (0x00000024UL))
           /* An error occurred in the requested write,erase command because Not erasable area or */
           /* hardware failure or the state of the FLMD pin was unstable. */
           /* The host made a transition to the P/E mode, so the data read is invalid. */
           /* for Write Configure setting area ,Security setting area                  */
#define ER_L_FLS_CANCEL         (ER_L_FLS_BASE | (0x00000025UL))
           /* An error occurred in the requested cancel command because hardware failure. */
#define ER_L_FLS_INVALID_DATA   (ER_L_FLS_BASE | (0x00000026UL))
           /* The host made a transition to the P/E mode, so the data read is invalid. */
           /* for Write Configure setting area ,Security setting area                  */
#define ER_L_FLS_INTERNAL       (ER_L_FLS_BASE | (0x00000027UL))
           /* An unexpected error occurred. */
#define ER_L_FLS_ECC1EDF1       (ER_L_FLS_BASE | (0x00000028UL))
           /* DF1 1Bits of ECC Error is Detected. */
#define ER_L_FLS_ECC2EDF1       (ER_L_FLS_BASE | (0x00000029UL))
           /* DF1 2Bits of ECC Error is Detected. */
#define ER_L_FLS_UNAVAILABLE    (ER_L_FLS_BASE | (0x0000002aUL))
           /* Flash unavailability detected. */

/* Timeout of FACI command */
#define ER_L_FLS_FACI_TIME_OUT  (ER_L_FLS_BASE | (0x0000002bUL))
/* Timeout of SICREMAP */
#define ER_L_FLS_REMAP          (ER_L_FLS_BASE | (0x0000002cUL))

/*
 FlashLib status
*/
#define ST_L_FLS_BLANK          (ER_L_FLS_BASE | (0x00000040UL))
           /* As a result of the blank check, the specified area is not erased. */
#define ST_L_FLS_NON_BLANK      (ER_L_FLS_BASE | (0x00000041UL))
           /* As a result of the blank check, the specified area is erased. */

/* the flag to detect if the PE side was in DF_PE mode setflg definition */
#define AG_L_FLS_DF_PE_SET      (0x01U)
                                        /* flag set */
#define AG_L_FLS_DF_PE_CLR      (0x00U)
                                        /* flag clr */

/* Blank check direction */
#define L_FLS_BLKCA_INC         (0x0U)
                                        /* Blank Checking (Incremental mode) */
#define L_FLS_BLKCA_DEC         (0x1U)
                                        /* Blank Checking (Decremental mode) */

#define L_FLS_NB_BLOCK                    D_FLS_NB_BLOCK
#define L_FLS_ERASE_BLOCK_SIZE            D_FLS_DF_ERASE_BLOCK_SIZE
#define L_FLS_CF_2ND_BLOCK_SIZE           D_FLS_CF_ERASE_BLOCK_SIZE

/* for memory boundaries */
#define L_FLS_LOWER_LIMIT_CF_SIDEA        D_FLS_LOWER_LIMIT_CF_SIDEA
#define L_FLS_LOWER_LIMIT_SCF_DOUBLE      D_FLS_LOWER_LIMIT_SCF_DOUBLE
#define L_FLS_UPPER_LIMIT_CF_SIDEA        D_FLS_UPPER_LIMIT_CF_SIDEA
#define L_FLS_LOWER_LIMIT_CF_SIDEB        D_FLS_LOWER_LIMIT_CF_SIDEB
#define L_FLS_LOWER_LIMIT_SCF_SINGLE      D_FLS_LOWER_LIMIT_SCF_SINGLE
#define L_FLS_UPPER_LIMIT_CF_SIDEB        D_FLS_UPPER_LIMIT_CF_SIDEB

#define L_FLS_LOWER_LIMIT_CRAM            D_FLS_LOWER_LIMIT_CRAM
#define L_FLS_UPPER_LIMIT_CRAM            D_FLS_UPPER_LIMIT_CRAM
#define L_FLS_LOWER_LIMIT_SLRAM           D_FLS_LOWER_LIMIT_SLRAM
#define L_FLS_UPPER_LIMIT_SLRAM           D_FLS_UPPER_LIMIT_SLRAM

#define L_FLS_LOWER_LIMIT_DF_BANKA        D_FLS_LOWER_LIMIT_DF_BANKA
#define L_FLS_UPPER_LIMIT_DF_BANKA        D_FLS_UPPER_LIMIT_DF_BANKA
#define L_FLS_LOWER_LIMIT_DF_BANKB        D_FLS_LOWER_LIMIT_DF_BANKB
#define L_FLS_UPPER_LIMIT_DF_BANKB        D_FLS_UPPER_LIMIT_DF_BANKB
#define L_FLS_LOWER_LIMIT_DF_SDF          D_FLS_LOWER_LIMIT_DF_SDF
#define L_FLS_UPPER_LIMIT_DF_SDF          D_FLS_UPPER_LIMIT_DF_SDF

#define L_FLS_LOWER_LIMIT_SCF_SIDEA       D_FLS_LOWER_LIMIT_SCF_SIDEA
#define L_FLS_UPPER_LIMIT_SCF_SIDEA       D_FLS_UPPER_LIMIT_SCF_SIDEA
#define L_FLS_LOWER_LIMIT_SCF_SIDEB       D_FLS_LOWER_LIMIT_SCF_SIDEB
#define L_FLS_UPPER_LIMIT_SCF_SIDEB       D_FLS_UPPER_LIMIT_SCF_SIDEB

#define L_FLS_LOWER_LIMIT_FIRMWARE        D_FLS_LOWER_LIMIT_FIRMWARE
#define L_FLS_UPPER_LIMIT_FIRMWARE        D_FLS_UPPER_LIMIT_FIRMWARE
#define L_FLS_LOWER_LIMIT_FIRMWARE_A      D_FLS_LOWER_LIMIT_FIRMWARE_A
#define L_FLS_UPPER_LIMIT_FIRMWARE_A      D_FLS_UPPER_LIMIT_FIRMWARE_A
#define L_FLS_LOWER_LIMIT_FIRMWARE_B      D_FLS_LOWER_LIMIT_FIRMWARE_B
#define L_FLS_UPPER_LIMIT_FIRMWARE_B      D_FLS_UPPER_LIMIT_FIRMWARE_B

#define L_FLS_LOWER_LIMIT_SECRET_DATA_SECTION   D_FLS_LOWER_LIMIT_SECRET_DATA_SECTION
#define L_FLS_UPPER_LIMIT_SECRET_DATA_SECTION   D_FLS_UPPER_LIMIT_SECRET_DATA_SECTION

#define L_FLS_LOWER_LIMIT_CF_BANKA        D_FLS_LOWER_LIMIT_CF_BANKA
#define L_FLS_UPPER_LIMIT_CF_BANKA        D_FLS_UPPER_LIMIT_CF_BANKA
#define L_FLS_LOWER_LIMIT_CF_BANKB        D_FLS_LOWER_LIMIT_CF_BANKB
#define L_FLS_UPPER_LIMIT_CF_BANKB        D_FLS_UPPER_LIMIT_CF_BANKB
#define L_FLS_LOWER_LIMIT_CF_BANKC        D_FLS_LOWER_LIMIT_CF_BANKC
#define L_FLS_UPPER_LIMIT_CF_BANKC        D_FLS_UPPER_LIMIT_CF_BANKC
#define L_FLS_LOWER_LIMIT_CF_BANKD        D_FLS_LOWER_LIMIT_CF_BANKD
#define L_FLS_UPPER_LIMIT_CF_BANKD        D_FLS_UPPER_LIMIT_CF_BANKD
#define L_FLS_LOWER_LIMIT_CF_BANKE        D_FLS_LOWER_LIMIT_CF_BANKE
#define L_FLS_UPPER_LIMIT_CF_BANKE        D_FLS_UPPER_LIMIT_CF_BANKE
#define L_FLS_LOWER_LIMIT_CF_BANKF        D_FLS_LOWER_LIMIT_CF_BANKF
#define L_FLS_UPPER_LIMIT_CF_BANKF        D_FLS_UPPER_LIMIT_CF_BANKF

/* Flash check result */
#define L_FLS_CHK_OK                      D_FLS_CHK_OK
#define L_FLS_CHK_NG                      D_FLS_CHK_NG

/* direction select */
#define L_FLS_CHK_MEM_SOURCE              D_FLS_CHK_MEM_SOURCE
                                                        /* specify a source address */
#define L_FLS_CHK_MEM_DESTINATION         D_FLS_CHK_MEM_DESTINATION
                                                        /* specify a destination address */

/* Dummy lock_handle(context ID) */
#define L_FLS_DUMMY_LOCK_HANDLE           (0xFFFFFFFFUL)
/********************************************************************************
 Typedef definitions
********************************************************************************/
/*
 FlashLib API Parameter
*/

/* ISSD callback registration function to be executed at the start of   */
/* CF_PE mode and at read mode interrupt from the PE side               */
typedef void (p_l_fls_cfint_addr) (uint32_t status);

/********************************************************************************
 Exported global variables
********************************************************************************/


/********************************************************************************
 Link to external constants
********************************************************************************/


/********************************************************************************
 Exported global functions (to be accessed by other files)
********************************************************************************/
/*
 FlashLib API
*/
/* icum_l_flash.c */
uint32_t L_FLS_Init(void);
uint32_t L_FLS_ReadFlash_ram(const uint32_t target_area, const uint32_t bank, const uint32_t offset, const uint32_t data_size_in_bytes, uint8_t *p_data);
uint32_t L_FLS_REQ_EraseFlash_ram(const uint32_t target_area, const uint32_t bank, const uint32_t offset, const uint32_t data_size_in_bytes);
uint32_t L_FLS_REQ_WriteFlash_ram(const uint32_t target_area, const uint32_t bank, const uint32_t offset, const uint32_t data_size_in_bytes, const uint8_t *p_data);
uint32_t L_FLS_REQ_WriteSecurity_ram(const uint32_t target_info, const uint32_t set_data);
uint32_t L_FLS_IsFinished_ram(const uint16_t context_id, const uint32_t target_area, const uint32_t bank);
uint32_t L_FLS_REQ_Cancel_ram(const uint32_t target_area, const uint32_t bank);
uint32_t L_FLS_GetFlashResource_ram(const uint32_t context_id, const uint8_t priority, const uint32_t target_area, const uint32_t bank);
uint32_t L_FLS_FreeFlashResource_ram(const uint32_t target_area, const uint32_t bank);
uint32_t L_FLS_ReadBlankCheckArea(const uint32_t target_area, const uint32_t bank, const uint32_t offset, 
                        const uint32_t data_size_in_bytes, const uint8_t blkc_mode, uint32_t *p_result_status);
uint32_t L_FLS_CheckMemory(const uint32_t start_addr, const uint32_t size, const uint8_t direction, uint8_t *p_result);
uint32_t L_FLS_CheckMemoryCISM(const uint32_t start_addr, const uint32_t size, const uint8_t direction, uint8_t *p_result);
uint32_t L_FLS_GetMemBoundary(const uint8_t boundary, uint32_t *p_addr);
uint32_t L_FLS_CheckSCFBoundary(const uint32_t cf_addr, const uint32_t size, uint32_t *p_bank, uint32_t *p_offset);
void L_FLS_RetrieveIcumRacRad(void);

uint32_t L_FLS_CheckUserCFBoundary(const uint32_t addr, const uint32_t size, uint32_t *p_bank, uint32_t *p_offset);
uint32_t L_FLS_CheckUserDFBoundary(const uint32_t addr, const uint32_t size, uint32_t *p_bank, uint32_t *p_offset);
uint32_t L_FLS_CheckContext_ram(const uint16_t context_id, const uint32_t target_area, const uint32_t bank);
uint32_t L_FLS_CheckRamExecRequired(const uint32_t target_area, const uint32_t addr, uint8_t *p_result);
uint32_t L_FLS_SetFACIStatus_ram(void);
uint32_t L_FLS_ClrFACIStatus_ram(void);

uint32_t L_FLS_CheckMemoryUserBoot(const uint32_t start_addr, const uint32_t size, const uint8_t direction, uint8_t *p_result);
uint32_t L_FLS_CheckMemoryCISMUserBoot(const uint32_t start_addr, const uint32_t size, const uint8_t direction, uint8_t *p_result);
uint32_t L_FLS_IsUserBootArea(const uint32_t start_addr, const uint32_t size);
uint32_t L_FLS_RemapStart(const uint32_t p_uiTargetAddr ,uint32_t **p_uiRemapAddr);
void L_FLS_RemapEnd(void);

#endif /* #if !defined(ICUM_L_FLASH_PUB_H) */


