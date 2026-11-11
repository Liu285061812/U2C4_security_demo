/******************************************************************************/
/* Component Name   ICUM Flash Driver                                         */
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

#if !defined(ICUM_D_FLASH_PUB_H)
#define ICUM_D_FLASH_PUB_H


/********************************************************************************
 Includes <System Includes> , "Project Includes"
********************************************************************************/
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "r_icumif_api.h"
#include "icum_d_comm_pub.h"

/********************************************************************************
 Macro definitions
********************************************************************************/

/* the flag to detect if the PE side was in DF_PE mode setflg definition */
#define AG_D_FLS_DF_PE_SET              (0x01U)
                                            /* flag set */
#define AG_D_FLS_DF_PE_CLR              (0x00U)
                                            /* flag clr */

/*
 FlashDrv return value
*/

#define ER_D_FLS_NON_BLANK              (ER_D_FLS_BASE | (0x00000001UL))
                                            /* As a result of the blank check, the specified area is not erased. */
                                            /* > ER_OK & ST_L_FLS_NON_BLANK */
#define ER_D_FLS_CANCEL                 (ER_D_FLS_BASE | (0x00000003UL))
                                            /* The requested operation was cancelled. */
                                            /* > ER_L_FLS_CANCEL */
#define ER_D_FLS_PROTECTION             (ER_D_FLS_BASE | (0x00000004UL))
                                            /* The requested command was not executed because the security function */
                                            /* is valid (erase protection is enabled, lock bit set, etc.). */
                                            /* > ER_L_FLS_PROTECTION */
#define ER_D_FLS_REJECTED               (ER_D_FLS_BASE | (0x00000005UL))
                                            /* The flash mode was already in other than the read mode, but tried to */
                                            /* set the mode other than the read mode. */
                                            /*  > ER_L_FLS_REJECTED */
#define ER_D_FLS_WRITE                  (ER_D_FLS_BASE | (0x00000006UL))
                                            /* An error occurred in the requested write command because writing to */
                                            /* the unerased area or hardware failure or the state of the FLMD pin  */
                                            /* was unstable. */
                                            /*  > ER_L_FLS_PROGRAM */
#define ER_D_FLS_ERASE                  (ER_D_FLS_BASE | (0x00000007UL))
                                            /* An error occurred in the requested erase command due to hardware */
                                            /* erasure error. */
                                            /*  > ER_L_FLS_PROGRAM */
#define ER_D_FLS_INTERNAL               (ER_D_FLS_BASE | (0x00000008UL))
                                            /* An unexpected error occurred. */
                                            /*  > ER_L_FLS_INTERNAL */
#define ER_D_FLS_FSTOPTOUT              (ER_D_FLS_BASE | (0x00000009UL))
                                            /* ForcedStopCommand TimeOut. */
                                            /*  > ER_L_FLS_INTERNAL */
#define ER_D_FLS_SETOPRDYTOUT           (ER_D_FLS_BASE | (0x0000000aUL))
                                            /* SetOperatingMode Ready TimeOut. */
                                            /*  > ER_L_FLS_INTERNAL */
#define ER_D_FLS_SETOPMODTOUT           (ER_D_FLS_BASE | (0x0000000bUL))
                                            /* SetOperatingMode Mode change TimeOut. */
                                            /*  > ER_L_FLS_INTERNAL */
#define ER_D_FLS_UNAVAILABLE            (ER_D_FLS_BASE | (0x0000000cUL))
                                            /* Nak was received from PE side in response to PE mode request from */
                                            /* ICUP side. */
                                            /*  > ER_L_FLS_UNAVAILABLE */
#define ER_D_FLS_ECC1EDF1               (ER_D_FLS_BASE | (0x0000000dUL))
                                            /* DF1 1Bits of ECC Error is Detected. */
                                            /*  > ER_L_FLS_ECC1EDF1 */
#define ER_D_FLS_ECC2EDF1               (ER_D_FLS_BASE | (0x0000000eUL))
                                            /* DF1 2Bits of ECC Error is Detected. */
                                            /*  > ER_L_FLS_ECC2EDF1 */
#define ER_D_FLS_CONFIGURATION          (ER_D_FLS_BASE | (0x0000000fUL))
                                            /* The operation parameter of FlashLib is invalid. */
#define ER_D_FLS_FACITOUT               (ER_D_FLS_BASE | (0x00000010UL))
                                            /* Timeout of FACI command */
#define ER_D_FLS_SUSPEND                (ER_D_FLS_BASE | (0x00000011UL))
                                            /* Flash suspend error */
#define ER_D_FLS_RESUME                 (ER_D_FLS_BASE | (0x00000012UL))
                                            /* Flash Resume error */
#define ER_D_FLS_REMAP                  (ER_D_FLS_BASE | (0x00000013UL))
                                            /* SICREMAP TIMEOUT */
#define ER_D_FLS_DFPRGINTTOUT           (ER_D_FLS_BASE | (0x00000014UL))
                                            /* Response wait timeout occurred in the DF mode switching request / */
                                            /* response handshake process. */
#define ER_D_FLS_ACCPROHIBIT            (ER_D_FLS_BASE | (0x00000015UL))
                                            /* NACK was responded in the DF mode switching request / response */
                                            /* handshake process. */
#define ER_D_FLS_REQUEST                (ER_D_FLS_BASE | (0x000000ffUL))
                                            /* Flash command request */

/* Security Area info   */
#define D_FLS_SEC_SETTING_SIZE              (32UL)
/* Front side */
#define D_FLS_SEC_SETTING_START_ADDRESS     (0xFF321800UL)
#define D_FLS_SEC_SETTING_END_ADDRESS       (0xFF3227FFUL)
#define D_FLS_ADR_ICUMUPGP                  (0xFF321F00UL)
#define D_FLS_ADR_ICUMRESV                  (0xFF321F04UL)
#define D_FLS_ADR_ICUMRACS0                 (0xFF321F08UL)
#define D_FLS_ADR_ICUMRAD                   (0xFF321F0CUL)
#define D_FLS_ADR_CHGRESP                   (0xFF321F10UL)
#define D_FLS_ADR_ICUMRACE0                 (0xFF321F14UL)
#define D_FLS_ADR_ICUMRACS1                 (0xFF321F18UL)
#define D_FLS_ADR_ICUMRACE1                 (0xFF321F1CUL)

#define D_FLS_ADR_DEGRADATION               (0xFF321AFCUL)
/* Back side */
#define D_FLS_SEC_SETTING_START_BACK_ADDRESS (0xFF322000UL)
#define D_FLS_SEC_SETTING_END_BACK_ADDRESS   (0xFF322FFFUL)
#define D_FLS_ADR_BACK_ICUMUPGP              (0xFF322700UL)
#define D_FLS_ADR_BACK_ICUMRESV              (0xFF322704UL)
#define D_FLS_ADR_BACK_ICUMRACS0             (0xFF322708UL)
#define D_FLS_ADR_BACK_ICUMRAD               (0xFF32270CUL)
#define D_FLS_ADR_BACK_CHGRESP               (0xFF322710UL)
#define D_FLS_ADR_BACK_ICUMRACE0             (0xFF322714UL)
#define D_FLS_ADR_BACK_ICUMRACS1             (0xFF322718UL)
#define D_FLS_ADR_BACK_ICUMRACE1             (0xFF32271CUL)

#define D_FLS_ADR_BACK_DEGRADATION           (0xFF3222FCUL)

/* OPBT off set value */
#define D_FLS_SEC_SETTING_OFFSET_ICUMUPGP   (D_FLS_ADR_BACK_ICUMUPGP - D_FLS_SEC_SETTING_START_BACK_ADDRESS)
#define D_FLS_SEC_SETTING_OFFSET_ICUMRESV   (D_FLS_ADR_BACK_ICUMRESV - D_FLS_SEC_SETTING_START_BACK_ADDRESS)
#define D_FLS_SEC_SETTING_OFFSET_ICUMRACS0  (D_FLS_ADR_BACK_ICUMRACS0 - D_FLS_SEC_SETTING_START_BACK_ADDRESS)
#define D_FLS_SEC_SETTING_OFFSET_ICUMRAD    (D_FLS_ADR_BACK_ICUMRAD - D_FLS_SEC_SETTING_START_BACK_ADDRESS)
#define D_FLS_SEC_SETTING_OFFSET_CHGRESP    (D_FLS_ADR_BACK_CHGRESP - D_FLS_SEC_SETTING_START_BACK_ADDRESS)
#define D_FLS_SEC_SETTING_OFFSET_ICUMRACE0  (D_FLS_ADR_BACK_ICUMRACE0 - D_FLS_SEC_SETTING_START_BACK_ADDRESS)
#define D_FLS_SEC_SETTING_OFFSET_ICUMRACS1  (D_FLS_ADR_BACK_ICUMRACS1 - D_FLS_SEC_SETTING_START_BACK_ADDRESS)
#define D_FLS_SEC_SETTING_OFFSET_ICUMRACE1  (D_FLS_ADR_BACK_ICUMRACE1 - D_FLS_SEC_SETTING_START_BACK_ADDRESS)
#define D_FLS_SEC_SETTING_OFFSET_DEGRADATION (D_FLS_ADR_BACK_DEGRADATION - D_FLS_SEC_SETTING_START_BACK_ADDRESS)

/* Data Flash ECC Error Clear mode */
#define AG_D_FLS_CLR                    (0x01U)
#define AG_D_FLS_NOTCLR                 (0x00U)

/* Function arg UNUSED */
#if defined(COMPILER_DIPEND_CODE)
#define UNUSED __attribute__((unused))
#else /*  defined(__WinAMS__) */
#define UNUSED
#endif

/*
 FACI sub command code definition
*/
#define D_FACI_SUB_CMD_BLK_INC_MODE     (0x0U)
                                            /* Blank Check (Incremental mode) */
#define D_FACI_SUB_CMD_BLK_DEC_MODE     (0x1U)
                                            /* Blank Check (Decremental mode) */

/* Blank Check Area */
#define D_FLS_BLKCA_INC                 (0x0U)
                                            /* Blank Checking (Incremental mode) */
#define D_FLS_BLKCA_DEC                 (0x1U)
                                            /* Blank Checking (Decremental mode) */
#define ST_D_FLS_BLANK                  (0x00000000UL)
                                            /* Target area is all Blank */
#define ST_D_FLS_NON_BLANK              (0x00000001UL)
                                            /* Target area includes Non Blank Area */

/* number of uint32_t buffer  */
#define D_FLS_CFGBUFF_NUM               (8U)

/* enbale macro for OPBT */
#define D_FLS_OPBT4_ICUMOPCRF2          (0x02000000UL)
#define D_FLS_OPBT4_ICUMOPCRF1          (0x01000000UL)
#define D_FLS_ICUMPEDBG                 (0x10000000UL)
#define D_FLS_ICUMOPGPIO                (0x00020000UL)
#define D_FLS_ICUMHSIF                  (0x00008000UL)
#define D_FLS_ICUMOPWDEN                (0x00000008UL)
#define D_FLS_ICUMUPGP1                 (0x00F00000UL)
#define D_FLS_ICUMUPGP2                 (0x0000F000UL)
#define D_FLS_DEGRADATION_DCF           (0x00000002UL)

#define D_FLS_CF_BANK_NUM               ((uint32_t)D_CF_BANK_NUM)
#define FLS_DF_BANK_NUM                 (0x00000003U)
                                        /* Data Flash Bank NUM */

#define D_FLS_CHK_OK                    ((uint8_t)0x01)
#define D_FLS_CHK_NG                    ((uint8_t)0x00)
/* direction select */
#define D_FLS_CHK_MEM_SOURCE            ((uint8_t)0x00)
                                            /* specify a source address */
#define D_FLS_CHK_MEM_DESTINATION       ((uint8_t)0x01)
                                            /* specify a destination address */

#define D_FLS_NB_BLOCK                  ((uint32_t)D_DF_BLOCK_NUM)

#define D_FLS_CF_ERASE_BLOCK_SIZE       (64uL * 1024uL)
#define D_FLS_DF_ERASE_BLOCK_SIZE       (4096uL)

/* for memory boundaries */
#define D_FLS_LOWER_LIMIT_CF_SIDEA      ((uint8_t)0x00)
#define D_FLS_LOWER_LIMIT_SCF_DOUBLE_S0 ((uint8_t)0x01)
#define D_FLS_LOWER_LIMIT_SCF_DOUBLE    ((uint8_t)0x01)
#define D_FLS_UPPER_LIMIT_CF_SIDEA      ((uint8_t)0x02)
#define D_FLS_LOWER_LIMIT_CF_SIDEB      ((uint8_t)0x03)
#define D_FLS_LOWER_LIMIT_SCF_SINGLE_S0 ((uint8_t)0x04)
#define D_FLS_LOWER_LIMIT_SCF_SINGLE    ((uint8_t)0x04)
#define D_FLS_UPPER_LIMIT_CF_SIDEB      ((uint8_t)0x05)

#define D_FLS_LOWER_LIMIT_CRAM          ((uint8_t)0x06)
#define D_FLS_UPPER_LIMIT_CRAM          ((uint8_t)0x07)
#define D_FLS_LOWER_LIMIT_SLRAM         ((uint8_t)0x08)
#define D_FLS_UPPER_LIMIT_SLRAM         ((uint8_t)0x09)

#define D_FLS_LOWER_LIMIT_DF_BANKA      ((uint8_t)0x0A)
#define D_FLS_UPPER_LIMIT_DF_BANKA      ((uint8_t)0x0B)
#define D_FLS_LOWER_LIMIT_DF_BANKB      ((uint8_t)0x0C)
#define D_FLS_UPPER_LIMIT_DF_BANKB      ((uint8_t)0x0D)
#define D_FLS_LOWER_LIMIT_DF_SDF        ((uint8_t)0x0E)
#define D_FLS_UPPER_LIMIT_DF_SDF        ((uint8_t)0x0F)

#define D_FLS_LOWER_LIMIT_SCF_SIDEA     ((uint8_t)0x10)
#define D_FLS_UPPER_LIMIT_SCF_SIDEA     ((uint8_t)0x11)
#define D_FLS_LOWER_LIMIT_SCF_SIDEB     ((uint8_t)0x12)
#define D_FLS_UPPER_LIMIT_SCF_SIDEB     ((uint8_t)0x13)

#define D_FLS_LOWER_LIMIT_FIRMWARE      ((uint8_t)0x14)
#define D_FLS_UPPER_LIMIT_FIRMWARE      ((uint8_t)0x15)
#define D_FLS_LOWER_LIMIT_FIRMWARE_A    ((uint8_t)0x16)
#define D_FLS_UPPER_LIMIT_FIRMWARE_A    ((uint8_t)0x17)
#define D_FLS_LOWER_LIMIT_FIRMWARE_B    ((uint8_t)0x18)
#define D_FLS_UPPER_LIMIT_FIRMWARE_B    ((uint8_t)0x19)

#define D_FLS_LOWER_LIMIT_SECRET_DATA_SECTION   ((uint8_t)0x1A)
#define D_FLS_UPPER_LIMIT_SECRET_DATA_SECTION   ((uint8_t)0x1B)

#define D_FLS_LOWER_LIMIT_CF_BANKA      ((uint8_t)0x1C)
#define D_FLS_UPPER_LIMIT_CF_BANKA      ((uint8_t)0x1D)
#define D_FLS_LOWER_LIMIT_CF_BANKB      ((uint8_t)0x1E)
#define D_FLS_UPPER_LIMIT_CF_BANKB      ((uint8_t)0x1F)
#define D_FLS_LOWER_LIMIT_CF_BANKC      ((uint8_t)0x20)
#define D_FLS_UPPER_LIMIT_CF_BANKC      ((uint8_t)0x21)
#define D_FLS_LOWER_LIMIT_CF_BANKD      ((uint8_t)0x22)
#define D_FLS_UPPER_LIMIT_CF_BANKD      ((uint8_t)0x23)
#define D_FLS_LOWER_LIMIT_CF_BANKE      ((uint8_t)0x24)
#define D_FLS_UPPER_LIMIT_CF_BANKE      ((uint8_t)0x25)
#define D_FLS_LOWER_LIMIT_CF_BANKF      ((uint8_t)0x26)
#define D_FLS_UPPER_LIMIT_CF_BANKF      ((uint8_t)0x27)
#define D_FLS_BOUNDARY_INDEX_MAX        ((uint8_t)0x28)

/* Target Flash Area */
#define AG_D_FLS_CODE                   (0x00010001UL)
                                            /* Code Flash */
#define AG_D_FLS_DATA                   (0x00010002UL)
                                            /* Data Flash */
#define AG_D_FLS_SECURITY               (0x00010003UL)
                                            /* Security setting area */
/* Target Flash Bank  */
#define AG_D_FLS_BANK_EXT               (0x00000000UL)
                                            /* Extened area (Config or Security) */
#define AG_D_FLS_BANK_ICUM              (0x00000001UL)
                                            /* Bank for ICUM */

/* Option byte  */
#define AG_D_FLS_ICUMUPGP               (0x00010021UL)
                                            /* ICUMHA UPGP */
#define AG_D_FLS_ICUMRESV               (0x00010022UL)
                                            /* ICUMHA reset vector */
#define AG_D_FLS_ICUMRACS0              (0x00010023UL)
                                            /* ICUMHA secure start address for code flash area region 0. */
#define AG_D_FLS_ICUMRAD                (0x00010024UL)
                                            /* ICUMHA secure address for data flash */
#define AG_D_FLS_CHGRESP                (0x00010025UL)
                                            /* ICUMHA Challenge & response, WDT related settings */
#define AG_D_FLS_ICUMRACE0              (0x00010026UL)
                                            /* ICUMHA secure end address for code flas area region 0. */
#define AG_D_FLS_ICUMRACS1              (0x00010027UL)
                                            /* ICUMHA secure start address for code flash area region 1. */
#define AG_D_FLS_ICUMRACE1              (0x00010028UL)
                                            /* ICUMHA secure end address for code flash area region 1. */
#define AG_D_FLS_DEGRADATION            (0x00010029UL)
                                            /* ICUMHA DEGRADATION */
#define AG_D_FLS_CR_RES_ICUP_SKIPPED    (0x00000001uL)
                                            /* C&R authentication is skipped for ICUP */
#define AG_D_FLS_CR_RES_CPU_SKIPPED     (0x00000002uL)
                                            /* C&R authentication is skipped for CPU */
#define AG_D_FLS_CR_RES_ALL_SKIPPED     (0x00000003uL)
                                            /* C&R authentication is skipped for ICUP & CPU */
#define AG_D_FLS_CR_RES_ICUP_ENABLED    (0x00000004uL)
                                            /* C&R authentication is enabled for ICUP */
#define AG_D_FLS_CR_RES_CPU_ENABLED     (0x00000005uL)
                                            /* C&R authentication is enabled for CPU */
#define AG_D_FLS_CR_RES_ALL_ENABLED     (0x00000006uL)
                                            /* C&R authentication is enabled for ICUP & CPU */
#define AG_D_FLS_ICUMPEDBG_ON           (0x00000007uL)
                                            /* PEDBG is enabled */
#define AG_D_FLS_ICUMPEDBG_OFF          (0x00000008uL)
                                            /* PEDBG is disabled */
#define AG_D_FLS_ICUMOPGPIO_ON          (0x00000009uL)
                                            /* OPGPIO is enabled */
#define AG_D_FLS_ICUMOPGPIO_OF          (0x0000000AuL)
                                            /* OPGPIO is disabled */
#define AG_D_FLS_ICUMHSIF_ON            (0x0000000BuL)
                                            /* RHSIF C&R for LinkPartner access is needed */
#define AG_D_FLS_ICUMHSIF_OFF           (0x0000000CuL)
                                            /* RHSIF C&R for LinkPartner access is not needed */
#define AG_D_FLS_ICUMOPWDEN_ON          (0x0000000DuL)
                                            /* ICUM WDTA is enabled */
#define AG_D_FLS_ICUMOPWDEN_OFF         (0x0000000EuL)
                                            /* ICUM WDTA is disabled */
#define AG_D_FLS_ICUMUPGP1_ON           (0x0000000FuL)
                                            /* UPGP1 protection is disabled */
#define AG_D_FLS_ICUMUPGP1_OFF          (0x00000010uL)
                                            /* UPGP1 protection is enabled */
#define AG_D_FLS_ICUMUPGP2_ON           (0x00000011uL)
                                            /* UPGP2 protection is disabled */
#define AG_D_FLS_ICUMUPGP2_OFF          (0x00000012uL)
                                            /* UPGP2 protection is enabled */
#define AG_D_FLS_DEGRADATION_DCF        (0x00000013uL)
                                            /* DEGRADATION */
#define D_FLS_UPGP_OFF                  (0x00000000UL)
                                            /* UPGP protect flag disabled */
#define D_FLS_UPGP_ON                   (0x00000001UL)
                                            /* UPGP protect flag enabled */
/* Flash preload enable/disable */
#define D_FLS_PRELOAD_ON    (1UL)
#define D_FLS_PRELOAD_OFF   (0UL)

/********************************************************************************
 Typedef definitions
********************************************************************************/
/*
 FlashDrv API Parameter
*/

/* FlashDrv Operating parameters */
typedef struct icum_fls_init_prm
{
    uint16_t cpu_frequency_MHz;
                                        /* Specify the operation clock of ICUP. */
} icum_fls_init_prm_t;

/* ISSD callback registration function to be executed at the start of   */
/* CF_PE mode and at read mode interrupt from the PE side               */
typedef void (p_d_fls_cfint_addr)(uint32_t status);

/********************************************************************************
 Exported global functions (to be accessed by other files)
********************************************************************************/
/*
 FlashDrv API
*/
/* icum_d_flash.c */
uint32_t D_FLS_InitFirst(const icum_fls_init_prm_t *p_init_prm_pstr);
uint32_t D_FLS_InitSecond(p_d_fls_cfint_addr *p_d_fls_cbfunc);
uint32_t D_FLS_CancelReq_ram(const uint32_t target_area, const uint32_t bank);
void D_FLS_PrgPrepareOperation_ram(void);
void D_FLS_PrgDisposeOperation_ram(void);
uint32_t D_FLS_GetFlashResource_ram(const uint16_t context_id, const uint8_t priority, const uint32_t target_area,
                                    const uint32_t bank);
uint32_t D_FLS_FreeFlashResource_ram(const uint32_t target_area, const uint32_t bank);
uint32_t D_FLS_GetDfProgReq_ram(uint8_t *p_d_usr_df_prog_req);
uint32_t D_FLS_SetDfProgReq_ram(const uint8_t d_usr_df_prog_req);
uint32_t D_FLS_CheckECC_ram(const uint8_t clearflg);

void D_FLS_SetCfIntCallBackFunc_ram(p_d_fls_cfint_addr *p_d_fls_cbfunc);
void D_FLS_SetCFPEAck_ram(void);
uint32_t D_FLS_CheckBlankFlag(const uint32_t target_addr, const uint32_t data_size_in_bytes, const uint8_t blkc_mode,
                              uint32_t *p_result_status);
uint32_t D_FLS_IsFinished_ram(const uint16_t context_id, const uint32_t target_area, const uint32_t bank);
uint32_t D_FLS_CheckStatus_ram(void);
uint32_t D_FLS_CheckContext_ram(const uint16_t context_id, const uint32_t target_area, const uint32_t bank);

/* Remap support for UserBoot area */
uint32_t D_FLS_RemapStart(const uint32_t p_uiTargetAddr, uint32_t **p_uiRemapAddr);
void D_FLS_RemapEnd(void);
uint32_t D_FLS_IsUserBootArea(const uint32_t start_addr, const uint32_t size);
void D_FLS_DFModeSwitchResInt_ram(icum_comm_int_from_channel_t int_channel, r_icumif_isd_t *p_isd);
uint32_t D_FLS_ChangeNoRemapAddr(const uint32_t p_uiTargetAddr ,uint32_t **p_uiNoRemapAddr);

/* icum_d_df_flash.c */
uint32_t D_FLS_ProgramDf(const uint32_t flash_addr, const uint8_t *p_data, const uint32_t data_size_in_bytes);
uint32_t D_FLS_EraseDf(const uint32_t flash_addr, const uint32_t data_size_in_bytes);
uint32_t D_FLS_GetDfProgramWriteSize(UNUSED const uint32_t flash_addr, uint32_t *p_size);
uint32_t D_FLS_GetDfEraseBlockSize(UNUSED const uint32_t flash_addr, uint32_t *p_size);

/* icum_d_cf_flash.c */
uint32_t D_FLS_ProgramCf_ram(const uint32_t flash_addr, const uint8_t *p_data, const uint32_t data_size_in_bytes);
uint32_t D_FLS_EraseCf_ram(const uint32_t flash_addr, const uint32_t data_size_in_bytes);
uint32_t D_FLS_GetCfProgramWriteSize_ram(UNUSED const uint32_t flash_addr, uint32_t *p_size);
uint32_t D_FLS_GetCfEraseBlockSize_ram(const uint32_t flash_addr, uint32_t *p_size);
void D_FLS_CfPreLoad(const uint32_t status);

/* icum_d_cfg_flash.c */
uint32_t D_FLS_UpdateSecurity(const uint32_t target_addr, const uint8_t *p_updated_field, const uint32_t size_field);
uint32_t D_FLS_GetWriteSizeSec(UNUSED const uint32_t flash_addr, uint32_t *p_size);
uint32_t D_FLS_SetUPGPProtect_ram(uint32_t flag1, uint32_t flag2);
uint32_t D_FLS_PostProgramSec(void);

/* icum_d_flash_map.c */
void D_FLS_SetIcumRacRad(const uint32_t *icum_racs0, const uint32_t *icum_race0, const uint32_t *icum_racs1, const uint32_t *icum_race1, const uint32_t *icum_rad);
uint32_t D_FLS_GetMemBoundary(const uint8_t boundary, uint32_t *p_addr);
uint32_t D_FLS_CheckMemory(const uint32_t start_addr, const uint32_t size, const uint8_t direction, uint8_t *p_result);
uint32_t D_FLS_CheckMemoryCISM(const uint32_t start_addr, const uint32_t size, const uint8_t direction,
                               uint8_t *p_result);
uint32_t D_FLS_CheckAddr_ram(const uint32_t target_area, const uint32_t bank, const uint32_t offset,
                             const uint32_t size, uint32_t *p_addr);
uint32_t D_FLS_CheckSCFBoundary(const uint32_t cf_addr, const uint32_t size, uint32_t *p_bank, uint32_t *p_offset);
uint32_t D_FLS_GetOPBTAddress(const uint32_t target_info, uint32_t *p_addr);
uint32_t D_FLS_CheckMemoryUserBoot(const uint32_t start_addr, const uint32_t size, const uint8_t direction,
                                   uint8_t *p_result);
uint32_t D_FLS_CheckMemoryCISMUserBoot(const uint32_t start_addr, const uint32_t size, const uint8_t direction,
                                       uint8_t *p_result);
uint32_t D_FLS_CheckUserCFBoundary(const uint32_t addr, const uint32_t size, uint32_t *p_bank, uint32_t *p_offset);
uint32_t D_FLS_CheckUserDFBoundary(const uint32_t addr, const uint32_t size, uint32_t *p_bank, uint32_t *p_offset);
uint32_t D_FLS_CheckRamExecRequired(const uint32_t target_area, const uint32_t addr, uint8_t *p_result);

#endif /* #if !defined(ICUM_D_FLASH_PUB_H) */

