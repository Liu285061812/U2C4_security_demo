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

#if !defined(ICUM_D_FLASH_H)
#define ICUM_D_FLASH_H

/********************************************************************************
 Debug Macro definitions
  Activate it according to your needs during integration testing.
********************************************************************************/
#if defined(__WinAMS__)
#define STATIC
#else
#define STATIC static
#endif

/********************************************************************************
 System Macro definitions
********************************************************************************/
/*
 ICUP clock (CLK_HBUS)
*/
#define CPU_CLOCK_MIN                   (25U)
#define CPU_CLOCK_MAX                   (100U)

/* Clear cache register check loop timeout(40usec loop counts) */
/* clocks per timeout loop */
#define D_FLS_TIMEOUT_CC_CLK_P_LOOP     (3UL)
/* timeout loop_count: (40us * f_MHz) / (clk per loop) ---  but rounding up to be considered */
#define D_FLS_TIMEOUT_CC                (uint32_t)(( (40UL * (uint32_t)g_fls_cpu_frequency_MHz) \
                                        + (D_FLS_TIMEOUT_CC_CLK_P_LOOP - 1UL) ) / D_FLS_TIMEOUT_CC_CLK_P_LOOP )

/* IO register check loop timeout(100usec loop counts) */
/* clocks per timeout loop */
#define D_FLS_TIMEOUT_CHKREG_CLK_P_LOOP (19UL)
/* timeout loop_count: (100us * f_MHz) / (clk per loop) ---  but rounding up to be considered */
#define D_FLS_TIMEOUT_CHKREG            (uint32_t)(( (100UL * (uint32_t)g_fls_cpu_frequency_MHz) \
                                        + (D_FLS_TIMEOUT_CHKREG_CLK_P_LOOP - 1UL) ) / D_FLS_TIMEOUT_CHKREG_CLK_P_LOOP )

/* IO register check loop timeout(30usec loop counts) force stop command timeout max(20usec) + alpha */
/* timeout loop_count: (30us * f_MHz) / (clk per loop) ---  but rounding up to be considered */
#define D_FLS_TIMEOUT_CHKREG_30u       (uint32_t)(( (30UL * (uint32_t)g_fls_cpu_frequency_MHz) \
                                       + (D_FLS_TIMEOUT_CHKREG_CLK_P_LOOP - 1UL) ) / D_FLS_TIMEOUT_CHKREG_CLK_P_LOOP )

/* Suspend/Resume Max time = HM Electrical Characteristics Table 10.3 and Table 10.6 */
#define D_FLS_TIMEOUT_SUSRDY           (uint32_t)(( (8UL * (uint32_t)g_fls_cpu_frequency_MHz) \
                                       + (D_FLS_TIMEOUT_CHKREG_CLK_P_LOOP - 1UL) ) / D_FLS_TIMEOUT_CHKREG_CLK_P_LOOP )

#define D_FLS_TIMEOUT_SUSPEND          (uint32_t)(( (1700UL * (uint32_t)g_fls_cpu_frequency_MHz) \
                                       + (D_FLS_TIMEOUT_CHKREG_CLK_P_LOOP - 1UL) ) / D_FLS_TIMEOUT_CHKREG_CLK_P_LOOP )

/* Property Programming Max time = 4.215 msec */
#define D_FLS_TIMEOUT_SEC_PRG          (uint32_t)(( (4215UL * (uint32_t)g_fls_cpu_frequency_MHz) \
                                       + (D_FLS_TIMEOUT_CHKREG_CLK_P_LOOP - 1UL) ) / D_FLS_TIMEOUT_CHKREG_CLK_P_LOOP )

/* Wait for DF mode switching response interrupt loop timeout (150usec loop couts) */
/* clocks per timeout loop */
#define D_FLS_TIMEOUT_DFPROGRES_CLK_P_LOOP (19UL)
/* timeout loop_count: (150us * f_MHz) / (clk per loop) ---  but rounding up to be considered */
#define D_FLS_TIMEOUT_DFPROGRES (uint32_t)(( (150UL * (uint32_t)g_fls_cpu_frequency_MHz) \
                                + (D_FLS_TIMEOUT_DFPROGRES_CLK_P_LOOP - 1UL) ) / D_FLS_TIMEOUT_DFPROGRES_CLK_P_LOOP )

#define ICUM_CMDREG_BASE                (0xFF1F0000UL)
#define ICUM_CTLREG_BASE                (0xFFFEE200UL)

/*
 Bank A/B/C/D address
*/
/* Data Flash BankA for FACI0 start address */
#define START_ADDR_DF_FPSYS0            ((uint32_t)__ghsbegin_start_DF_BANK_A)
/* Data Flash BankB start address */
#define START_ADDR_DF_FPSYS1            ((uint32_t)__ghsbegin_start_DF_BANK_B)
/* Data Flash Bank for ICUM FACI2 start address */
#define START_ADDR_DF_FPSYS2            ((uint32_t)__ghsbegin_start_DF_BANK_ICUM)
/* Data Flash Bank for ICUM FACI2 end address */
#define START_ADDR_DF_FPSYS3            ((uint32_t)(__ghsbegin_end_DF_BANK_ICUM + 1))

/* Code Flash BankA for FACI0 start address */
#define START_ADDR_CF_FPSYS0            ((uint32_t)__ghsbegin_start_iROM_BANK_A)
/* Code Flash BankB start address */
#define START_ADDR_CF_FPSYS1            ((uint32_t)__ghsbegin_start_iROM_BANK_B)
/* Code Flash BankB end address */
#define START_ADDR_CF_FPSYS2            ((uint32_t)(__ghsbegin_end_iROM_BANK_B + 1 ))

/* Code Flash Boot Area Size */
#define CF_BOOT_AREA_SIZE (0x00020000UL)
/* From address 0x00000000 (BANK_A) */
#define CF_BOOT_AREA_SIZE_BANKA_START       ((uint32_t)__ghsbegin_start_iROM_BANK_A)
/* Up to address 0x00020000 (BANK_A) */
#define CF_BOOT_AREA_SIZE_BANKA_END         (CF_BOOT_AREA_SIZE_BANKA_START + CF_BOOT_AREA_SIZE)
/* From address 0x00400000 (BANK_B) */
#define CF_BOOT_AREA_SIZE_BANKB_START       ((uint32_t)__ghsbegin_start_iROM_BANK_B)
/* Up to address 0x00420000 (BANK_B) */
#define CF_BOOT_AREA_SIZE_BANKB_END         (CF_BOOT_AREA_SIZE_BANKB_START + CF_BOOT_AREA_SIZE)

/* Code Flash Erase Block Size (From address 0x00000000 to 0x0001FFFF) */
#define CF_ERASE_BLOCK_SIZE_BOOT        (uint32_t)(16 * 1024)
/* Code Flash Erase Block Size (Other)                                 */
#define CF_ERASE_BLOCK_SIZE             (D_FLS_CF_ERASE_BLOCK_SIZE)
/* Code Flash Write Data Size */
#define CF_WBUFFER_SIZE                 (512UL)

/* Data Flash Erase Block Size */
#define DF_ERASE_BLOCK_SIZE             (D_FLS_DF_ERASE_BLOCK_SIZE)
/* Data Flash Write Data Size  */
#define DF_WBUFFER_SIZE                 (4UL)
/* Data Flash Multi Write Data Size  */
#define DF_MULTIW8_SIZE                 (DF_WBUFFER_SIZE + DF_WBUFFER_SIZE)
#define DF_MULTIW16_SIZE                (DF_MULTIW8_SIZE + DF_MULTIW8_SIZE)
#define DF_MULTIW32_SIZE                (DF_MULTIW16_SIZE + DF_MULTIW16_SIZE)
#define DF_MULTIW64_SIZE                (DF_MULTIW32_SIZE + DF_MULTIW32_SIZE)
#define DF_MULTIW128_SIZE               (DF_MULTIW64_SIZE + DF_MULTIW64_SIZE)

/* Alignment Check */
#define ALIGN_CHK_8                     (0x00000007UL)
#define ALIGN_CHK_16                    (0x0000000FUL)
#define ALIGN_CHK_32                    (0x0000001FUL)
#define ALIGN_CHK_64                    (0x0000003FUL)
#define ALIGN_CHK_128                   (0x0000007FUL)
#define ALIGN_CHK_OK                    (0x00000000UL)

/* Split Size for processing of Data Flash Blank Check  */
#define DF_BLANK_CHECK_SIZE             (0x1000UL)

/*
 FACI command code definition
*/
#define D_FACI_CMD_NONE                 (0x00U)
                                                        /* initial state */
#define D_FACI_CMD_PROGRAM              (0xE8U)
                                                        /* Program (user/data area) */
#define D_FACI_CMD_MULTI_PROGRAM        (0xEDU)
                                                        /* Multi Program (user/data area) */
#define D_FACI_CMD_BLOCK_ERASE          (0x20U)
                                                        /* Block Erase              */
#define D_FACI_CMD_PE_RESUME            (0xD0U)
                                                        /* Program/Erasure Resume   */
#define D_FACI_CMD_STATUS_CLEAR         (0x50U)
                                                        /* Status Clear             */
#define D_FACI_CMD_FORCED_STOP          (0xB3U)
                                                        /* Forced Stop              */
#define D_FACI_CMD_BLANK_CHECK          (0x71U)
                                                        /* Blank Check              */
#define D_FACI_CMD_SECURITY_ERASE       (0x47U)
                                                        /* Security setting Erase   */
#define D_FACI_CMD_SECURITY_PROGRAM     (0x45U)
                                                        /* Security setting Program */
#define D_FACI_CMD_SWITCH_ERASE         (0x87U)
                                                        /* Switch area Erase        */
#define D_FACI_CMD_SWITCH_PROGRAM       (0x85U)
                                                        /* Switch area Program      */
#define D_FACI_CMD_TAG_ERASE            (0x89U)
                                                        /* TAG area Erase           */
#define D_FACI_CMD_TAG_UPDATE           (0x83U)
                                                        /* TAG area Update          */
#define D_FACI_CMD_SUSPENSION           (0xB0U)
                                                        /* Programming/Erasure Suspension */
#define D_FACI_CMD_RESUMPTION           (0xD0U)
                                                        /* Programming/Erasure Resumption */
#define D_FACI_CMD_DUMMY_CMD            (0xFFU)
                                                        /* DUMM Command */

/*
 FACI sub command code definition
*/
#define D_FACI_SUB_CMD_BLK_INC_MODE     (0x0U)
                                                        /* Blank Check (Incremental mode) */
#define D_FACI_SUB_CMD_BLK_DEC_MODE     (0x1U)
                                                        /* Blank Check (Decremental mode) */

/* Blank Check Area */
#define D_FLS_BLKCA_OFFSET_DF           (0x00200000UL)
                                        /* Blank Check Area offset of data flash */
#define D_FLS_BF_ON                     (0x00000001UL << 15U)
                                        /* Blank Flag bit */

/* ICU-M firmware start address */
#define D_FLS_ICUM_FIRMWARE_START_ADDRESS   ((uint32_t)__ghsbegin_start_firmware)
#define D_FLS_ICUM_FIRMWARE_END_ADDRESS     ((uint32_t)__ghsbegin_end_firmware)
#define D_FLS_ICUM_FIRMWARE_A_START_ADDRESS ((uint32_t)__ghsbegin_start_firmware_a)
#define D_FLS_ICUM_FIRMWARE_A_END_ADDRESS   ((uint32_t)__ghsbegin_end_firmware_a)
#define D_FLS_ICUM_FIRMWARE_B_START_ADDRESS ((uint32_t)__ghsbegin_start_firmware_b)
#define D_FLS_ICUM_FIRMWARE_B_END_ADDRESS   ((uint32_t)__ghsbegin_end_firmware_b)

/* Memory section boundaries */
#define D_FLS_SCF_SIDEA_START_ADDRESS   ((uint32_t)__ghsbegin_start_sROM_SIDE_A)
#define D_FLS_SCF_SIDEA_END_ADDRESS     ((uint32_t)__ghsbegin_end_sROM_SIDE_A)
#define D_FLS_SCF_SIDEB_START_ADDRESS   ((uint32_t)__ghsbegin_start_sROM_SIDE_B)
#define D_FLS_SCF_SIDEB_END_ADDRESS     ((uint32_t)__ghsbegin_end_sROM_SIDE_B)

#define D_FLS_MAX_CF_BANK_NUM           (2u)
#define D_FLS_CF_BANKA_START_ADDRESS    ((uint32_t)__ghsbegin_start_iROM_BANK_A)
#define D_FLS_CF_BANKA_END_ADDRESS      ((uint32_t)__ghsbegin_end_iROM_BANK_A)
#define D_FLS_CF_BANKB_START_ADDRESS    ((uint32_t)__ghsbegin_start_iROM_BANK_B)
#define D_FLS_CF_BANKB_END_ADDRESS      ((uint32_t)__ghsbegin_end_iROM_BANK_B)
#define D_FLS_CF_BANKC_START_ADDRESS    ((uint32_t)__ghsbegin_start_iROM_BANK_C)
#define D_FLS_CF_BANKC_END_ADDRESS      ((uint32_t)__ghsbegin_end_iROM_BANK_C)
#define D_FLS_CF_BANKD_START_ADDRESS    ((uint32_t)__ghsbegin_start_iROM_BANK_D)
#define D_FLS_CF_BANKD_END_ADDRESS      ((uint32_t)__ghsbegin_end_iROM_BANK_D)
#define D_FLS_CF_BANKE_START_ADDRESS    ((uint32_t)__ghsbegin_start_iROM_BANK_E)
#define D_FLS_CF_BANKE_END_ADDRESS      ((uint32_t)__ghsbegin_end_iROM_BANK_E)
#define D_FLS_CF_BANKF_START_ADDRESS    ((uint32_t)__ghsbegin_start_iROM_BANK_F)
#define D_FLS_CF_BANKF_END_ADDRESS      ((uint32_t)__ghsbegin_end_iROM_BANK_F)

#define D_FLS_DF_BANKA_START_ADDRESS    ((uint32_t)__ghsbegin_start_DF_BANK_A)
#define D_FLS_DF_BANKA_END_ADDRESS      ((uint32_t)__ghsbegin_end_DF_BANK_A)
#define D_FLS_DF_BANKB_START_ADDRESS    ((uint32_t)__ghsbegin_start_DF_BANK_B)
#define D_FLS_DF_BANKB_END_ADDRESS      ((uint32_t)__ghsbegin_end_DF_BANK_B)
#define D_FLS_DF_BANKICUM_START_ADDRESS ((uint32_t)__ghsbegin_start_DF_BANK_ICUM)
#define D_FLS_DF_BANKICUM_END_ADDRESS   ((uint32_t)__ghsbegin_end_DF_BANK_ICUM)

#define D_FLS_CRAM_BANK_START_ADDRESS   ((uint32_t)__ghsbegin_start_gRAM)
#define D_FLS_CRAM_BANK_END_ADDRESS     ((uint32_t)__ghsbegin_end_gRAM)

#define D_FLS_CISM_BSS_START_ADDRESS    ((uint32_t)__ghsbegin_cism_bss)
#define D_FLS_CISM_BSS_END_ADDRESS      ((uint32_t)__ghsend_cism_bss)

#define D_FLS_MAX_ADDRESS               (0xFFFFFFFFUL)

#define ADDR_LOWER_CF_BANKA             (D_FLS_CF_BANKA_START_ADDRESS)
#define ADDR_UPPER_CF_BANKA             (D_FLS_CF_BANKA_END_ADDRESS - 1uL)
#define ADDR_LOWER_CF_BANKB             (D_FLS_CF_BANKB_START_ADDRESS)
#define ADDR_UPPER_CF_BANKB             (D_FLS_CF_BANKB_END_ADDRESS - 1uL)
#define ADDR_LOWER_CF_BANKC             (D_FLS_CF_BANKC_START_ADDRESS)
#define ADDR_UPPER_CF_BANKC             (D_FLS_CF_BANKC_END_ADDRESS - 1uL)
#define ADDR_LOWER_CF_BANKD             (D_FLS_CF_BANKD_START_ADDRESS)
#define ADDR_UPPER_CF_BANKD             (D_FLS_CF_BANKD_END_ADDRESS - 1uL)
#define ADDR_LOWER_CF_BANKE             (D_FLS_CF_BANKE_START_ADDRESS)
#define ADDR_UPPER_CF_BANKE             (D_FLS_CF_BANKE_END_ADDRESS - 1uL)
#define ADDR_LOWER_CF_BANKF             (D_FLS_CF_BANKF_START_ADDRESS)
#define ADDR_UPPER_CF_BANKF             (D_FLS_CF_BANKF_END_ADDRESS - 1uL)

#define ADDR_LOWER_SCF_SIDEA            (D_FLS_SCF_SIDEA_START_ADDRESS)
#define ADDR_UPPER_SCF_SIDEA            (D_FLS_SCF_SIDEA_END_ADDRESS - 1uL)
#define ADDR_LOWER_SCF_SIDEB            (D_FLS_SCF_SIDEB_START_ADDRESS)
#define ADDR_UPPER_SCF_SIDEB            (D_FLS_SCF_SIDEB_END_ADDRESS - 1uL)

#define ADDR_LOWER_CRAM                 ((uint32_t)__ghsbegin_start_gRAM)
#define ADDR_UPPER_CRAM                 (((uint32_t)__ghsbegin_end_gRAM) - 1uL)

#define ADDR_LOWER_SLRAM                ((uint32_t)__ghsbegin_start_iRAM_Self)
#define ADDR_UPPER_SLRAM                (((uint32_t)__ghsbegin_end_iRAM_Self) - 1uL)

#define ADDR_LOWER_CISM_BSS             ((uint32_t)__ghsbegin_cism_bss)
#define ADDR_UPPER_CISM_BSS             (((uint32_t)__ghsend_cism_bss) - 1uL)

#define ADDR_LOWER_DF_BANKA             (D_FLS_DF_BANKA_START_ADDRESS)
#define ADDR_UPPER_DF_BANKA             (D_FLS_DF_BANKA_END_ADDRESS - 1uL)
#define ADDR_LOWER_DF_BANKB             (D_FLS_DF_BANKB_START_ADDRESS)
#define ADDR_UPPER_DF_BANKB             (D_FLS_DF_BANKB_END_ADDRESS - 1uL)
#define ADDR_LOWER_DF_SDF_DEF           (D_FLS_DF_BANKICUM_START_ADDRESS)
#define ADDR_UPPER_DF_SDF               (D_FLS_DF_BANKICUM_END_ADDRESS - 1uL)

#define ADDR_LOWER_FIRMWARE             ((uint32_t)__ghsbegin_start_firmware)
#define ADDR_UPPER_FIRMWARE             ((uint32_t)__ghsbegin_end_firmware)
#define ADDR_LOWER_FIRMWARE_A           ((uint32_t)__ghsbegin_start_firmware_a)
#define ADDR_UPPER_FIRMWARE_A           (((uint32_t)__ghsbegin_end_firmware_a) - 1uL)
#define ADDR_LOWER_FIRMWARE_B           ((uint32_t)__ghsbegin_start_firmware_b)
#define ADDR_UPPER_FIRMWARE_B           (((uint32_t)__ghsbegin_end_firmware_b) - 1uL)

#define ADDR_LOWER_USER_BOOT0        ((uint32_t)__ghsbegin_start_iROM_BANK_S0)
#define ADDR_UPPER_USER_BOOT0        ((uint32_t)__ghsbegin_end_iROM_BANK_S0 -1uL)
#define ADDR_LOWER_USER_BOOT1        ((uint32_t)__ghsbegin_start_iROM_BANK_S1)
#define ADDR_UPPER_USER_BOOT1        ((uint32_t)__ghsbegin_end_iROM_BANK_S1 -1uL)

#define ADDR_LOWER_SECRET_DATA_SECTION  ((uint32_t)__ghsbegin_SECRET_DATA_SECTION)
#define ADDR_UPPER_SECRET_DATA_SECTION  ((uint32_t)__ghsend_SECRET_DATA_SECTION)

/* Target Flash Side  */
#define AG_D_FLS_BANKA                  (0x00000000UL)
                                        /* Bank A */
#define AG_D_FLS_BANKB                  (0x00000001UL)
                                        /* Bank B */
#define AG_D_FLS_BANKC                  (0x00000002UL)
                                        /* Bank C */
#define AG_D_FLS_BANKD                  (0x00000003UL)
                                        /* Bank D */
#define AG_D_FLS_BANKE                  (0x00000004UL)
                                        /* Bank E */
#define AG_D_FLS_BANKF                  (0x00000005UL)
                                        /* Bank F */

/* Target Flash Bank ( Not open to the public ) */
#define AG_D_FLS_BANK_DATA0             (0x00000000UL)
                                        /* Bank for Data Area 0 */
#define AG_D_FLS_BANK_DATA1             (0x00000001UL)
                                        /* Bank for Data Area 1 */

/* Security Area info ( Not open to the public ) */
#define SECURITY_SETTING_INVAL_ST_ADDR  (0xFF322000UL)
                                        /* Start address of security setting area (invalid area) */
#define SECURITY_SETTING_ERASE_SIZE     (2048UL)
                                        /* Erase size of security setting area */
#define SECURITY_SETTING_PROGRAM_SIZE   (32UL)
                                        /* Write size of security area */
#define SECURITY_SETTING_PRO_END_ADDR   (28UL)
                                        /* Write end Address of security area */
#define SECURITY_SETTING_WRITE_SIZE     (0x08U)
                                        /* Write size */
#define SECURITY_SETTING_VALID_ST_ADDR  (0xFF321800UL)

/* Switch Area info ( Not open to the public ) */
#define SWITCH_AREA_INVAL_ST_ADDR       (0xFF374000UL)
                                        /* Start address of switch area (invalid area) */
#define SWITCH_AREA_INVAL_VALID_INFO    (SWITCH_AREA_INVAL_ST_ADDR + 0x0200UL)
                                        /* Start address of valid side information of switch area (invalid area) */
#define SWITCH_AREA_VALID_ST_ADDR       (0xFF373800UL)
                                        /* Start address of switch area (invalid area) */
#define SWITCH_AREA_VALID_VALID_INFO    (SWITCH_AREA_VALID_ST_ADDR + 0x0200UL)
                                        /* Start address of valid side information of switch area (valid area) */
#define SWITCH_AREA_VALID_INDEX_SVA     (1)
                                        /* The position of the SVA within the switch area (in units of 4 bytes) */
#define SWITCH_AREA_ERASE_SIZE          (2048UL)
                                        /* Erase size of switch area */
#define SWITCH_AREA_PROGRAM_SIZE        (32UL)
                                        /* Write size of switch area */

/* TAG Area info ( Not open to the public ) */
#define TAG_AREA_ST_ADDR                (0xFF374800UL)
                                        /* Start address of TAG area */
#define TAG_AREA_VAF_ADDR               (TAG_AREA_ST_ADDR + 0x0200UL)
                                        /* Address of Valid Area Flag of TAG area */
#define TAG_AREA_ERASE_SIZE             (2048UL)
                                        /* Erase size of TAG area */
#define TAG_AREA_UPDATE_SIZE            (4UL)
                                        /* Update size of TAG area */
#define VAPC_ADDR                       (*(volatile uint32_t *)(TAG_AREA_ST_ADDR))

#define VAPC_CHECK                      (0x5AA5A55A)

/*
 LWFL private error code definition
*/
#define FLS_ERR_NONE           (0UL)
                                    /* No error detected                                                      */
#define FLS_ERR_CMD            (1UL << 31UL)
                                    /* Flash sequencer has detected an illegal command.                       */
#define FLS_ERR_ACCESS         (1UL << 30UL)
                                    /* Flash sequencer has detected an illegal flash memory access.           */
#define FLS_ERR_PROG           (1UL << 29UL)
                                    /* Indicates error has occurred during of code or data flash programming  */
                                    /*   or Erasure by flash sequencer.                                       */
                                    /* Indicates a violation of the flash memory overwrite protection         */
                                    /*   due to FHVE15 or FHVE3 register.                                     */
#define FLS_ERR_FCU            (1UL << 28UL)
                                    /* Indicates that a 2-bit error has been detected on reading the          */
                                    /*   Configure Program value or erase counter area.                       */
                                    /* Indicates that a 2-bit error has been detected on reading the          */
                                    /*   overwrite parameter table.                                           */
#define FLS_ERR_CANCEL_END     (1UL << 27UL)
                                    /* force stop command normal end.                                         */
#define FLS_ERR_DBFULL_TIMEOUT (1UL << 26UL)
                                    /* Data Buffer Full timeout detected.                                     */
#define FLS_ERR_PROTECTION     (1UL << 25UL)
                                    /* FHVE3CNT bit in FHVE3 register has changed to 0 while command          */
                                    /* processing is provided by the flash sequencer.                         */
#define FLS_ERR_FENTRYR        (1UL << 24UL)
                                    /* The value set in FENTRYR_n is not 0000H, 0001H, or 0080H.              */
                                    /* The FENTRYR_n setting for resuming operation does not match            */
                                    /* that for suspending operation.                                         */
#define FLS_ERR_SECURITY       (1UL << 23UL)
                                    /* Flash access protection err.                                           */
                                    /* A command has been issued against OTP setting area.                    */
                                    /* The error that security function of ICUMD caused.                      */
#define FLS_ERR_OTHER          (1UL << 22UL)
                                    /* FACI command-issuing area has been accessed in read mode.              */
                                    /* FACI command-issuing area has been read in code flash                  */
                                    /* programming/erasure mode or data flash programming/erasure mode.       */

/* The number of FACI */
#define NB_FPSYS    (3U)

#define ICUM_S_2ND_REGION_OFFSET        (D_FLS_SCF_SIDEB_END_ADDRESS - D_FLS_SCF_SIDEB_START_ADDRESS)

/* fill the block with 0xFF(4byte) */
#define D_FLS_FILL_DATA        (0xFFFFFFFFUL)

#define D_FLS_ZERO_U8                 ((uint8_t)0)
#define D_FLS_ZERO_U16                (0U)
#define D_FLS_ZERO_U32                (0UL)
#define D_FLS_SET1_U16                (1U)
#define D_FLS_SET1_U32                (1UL)
#define D_FLS_WRITE_COUNT             (8UL)
#define D_FLS_MANDATORY_WRITE_COUNT   (14UL)

#define D_FLS_SHIT_1BIT               (1U)

#define D_FLS_PE_CHECK               (0x00000001UL)

#define D_FLS_CMDREQ_OFF              ((uint8_t)0U)
#define D_FLS_CMDREQ_ON               ((uint8_t)1U)

/* UPGPS register operate parametar */
#define D_FLS_UPGP1_PROTECT           (1UL)
#define D_FLS_UPGP2_PROTECT           (2UL)
#define D_FLS_UPGP_RETRY              (3UL)

/* callback function status */
#define CF_PE_END   ((uint8_t)0x0U)
#define CF_PE_START ((uint8_t)0x1U)
#define CF_PE_INIT  ((uint8_t)0xFFU)
/*
 FACI registers
*/

#define FCU_REG_BASE 0xFF1A0000
#define FACI_CMD_BASE (FCU_REG_BASE + 0x00A0)
#if defined(__WinAMS__)
    extern uint8_t FACICMD;
                            /* FACI command-issuing area */
    extern volatile uint32_t FACICMDDT;
                            /* FACI command-issuing area (only data)*/

    extern  fastat_t FASTAT;
                            /* Flash Access Status Register */
    #define FASTAT_ADR  (volatile uint8_t *)(&FASTAT)

    extern volatile uint32_t FSADDR;
                            /* FACI Command Start Address Register */
    extern volatile uint32_t FEADDR;
                            /* FACI Command End Address Register */

    extern fstatr_t FSTATR;
                            /* Flash Status Register */
    #define FSTATR_ADR  (volatile uint32_t *)(&FSTATR)

    extern uint16_t FENTRYR;
                            /* Flash P/E Mode Entry Register */
    #define FENTRYR_ADR (volatile uint16_t *)(&FENTRYR)

    extern  uint16_t FSUINITR;
                            /* Flash Sequencer Set-Up Initialize Register */

    extern volatile uint16_t  FPESTAT;
                            /* Flash P/E Status Register */
    extern fbccnt_t  FBCCNT;
                            /* Data Flash Blank Check Control Register */
    extern volatile fbcstat_t FBCSTAT;
                            /* Data Flash Blank Check Status Register */
    extern volatile uint32_t  FPSADDR;
                            /* Data Flash write start address Register */
    extern uint16_t  FECCTMD;
                            /* Flash ECC test mode register */


    /* R_LWFL_InitSection */
    extern uint8_t ghssize_icum_driver_ram_text;
    extern uint8_t ghssize_icum_driver_ram_rodata;
    extern uint8_t ghssize_icum_driver_ram_data;
    extern uint8_t ghssize_icum_driver_bss;

    #define __ghssize_icum_driver_ram_text ghssize_ghssize_icum_driver_ram_text;
    #define __ghssize_icum_driver_ram_rodata ghssize_icum_driver_ram_rodata;
    #define __ghssize_icum_driver_ram_data ghssize_icum_driver_ram_data;
    #define __ghssize_icum_driver_bss ghssize_icum_driver_bss;

#else /* !defined(__WinAMS__) */
    #define FACICMD     (*(volatile uint8_t *)(FACI_CMD_BASE))
                            /* FACI command-issuing area */
    #define FACICMDDT   (*(volatile uint32_t *)(FACI_CMD_BASE))
                            /* FACI command-issuing area (only data)*/
    #define FASTAT_ADR  ((volatile uint8_t *)(FCU_REG_BASE + 0x10UL))
                            /* Flash Access Status Register */
    #define FASTAT      (*(volatile uint8_t *)(FASTAT_ADR))
                            /* Register value */

    #define FSADDR      (*(volatile uint32_t *)(FCU_REG_BASE + 0x30UL))
                            /* FACI Command Start Address Register */
    #define FEADDR      (*(volatile uint32_t *)(FCU_REG_BASE + 0x34UL))
                            /* FACI Command End Address Register */

    #define FSTATR_ADR  ((volatile uint32_t *)(FCU_REG_BASE + 0x80UL))
                            /* Flash Status Register */
    #define FSTATR      (*(volatile uint32_t *)(FSTATR_ADR))
                            /* Register value */

    #define FENTRYR_ADR ((volatile uint16_t *)(FCU_REG_BASE + 0x84UL))
                            /* Flash P/E Mode Entry Register */
    #define FENTRYR     (*(volatile uint16_t *)(FENTRYR_ADR))
                            /* Register value */

    #define FSUINITR    (*(volatile uint16_t *)(FCU_REG_BASE + 0x8CUL))
                            /* Flash Sequencer Set-Up Initialize Register */

    #define FPESTAT     (*(volatile uint16_t *)(FCU_REG_BASE + 0xC0UL))
                            /* Flash P/E Status Register */
    #define FBCCNT      (*(volatile uint8_t *)(FCU_REG_BASE + 0xD0UL))
                            /* Data Flash Blank Check Control Register */
    #define FBCSTAT     (*(volatile uint8_t *)(FCU_REG_BASE + 0xD4UL))
                            /* Data Flash Blank Check Status Register */
    #define FPSADDR     (*(volatile uint32_t *)(FCU_REG_BASE + 0xD8UL))
                            /* Data Flash write start address Register */
    #define FECCTMD     (*(volatile uint16_t *)(FCU_REG_BASE + 0x104UL))
                            /* Flash ECC test mode register */

    #define DFLS_PEFSACS0REQ     (*(volatile uint32_t *)(ICUM_CMDREG_BASE + 0xC0UL))
                            /* Flash sequencer 0 Req from CPU P/E Change case */
    #define DFLS_PEFSACS1REQ     (*(volatile uint32_t *)(ICUM_CMDREG_BASE + 0xC8UL))
                            /* Flash sequencer 1 Req from CPU P/E Change case */
    #define DFLS_ICUM_FLPRG0ACK  (*(volatile uint32_t *)(ICUM_CTLREG_BASE + 0x58UL))
                            /* Flash sequencer 0 Ack from CPU P/E Change case */
    #define DFLS_ICUM_FLPRG1ACK  (*(volatile uint32_t *)(ICUM_CTLREG_BASE + 0x5CUL))
                            /* Flash sequencer 1 Ack from CPU P/E Change case */

    #define FCMDR       (*(volatile uint16_t *)(FCU_REG_BASE + 0xA0UL))
                            /* FACI COMMAND Register */

#endif /* defined(__WinAMS__) */

/* Flash Status Register bit */
#define FSTATR_SECDTCT  ((1UL << 25UL))
                            /* Security setting area ECC 1-Bit Error Correction Monitoring Bit */
#define FSTATR_SECCRCT  ((1UL << 24UL))
                            /* Security setting area ECC 1-Bit Error Correction Monitoring Bit */
#define FSTATR_ILGCOMERR ((1UL << 23UL))
                            /* Illegal Command Error                    */
#define FSTATR_FESETERR ((1UL << 22UL))
                            /* FENTRY Setting Error                     */
#define FSTATR_SECERR   ((1UL << 21UL))
                            /* Security Error                           */
#define FSTATR_OTERR    ((1UL << 20UL))
                            /* Other Error                              */
#define FSTATR_EBFULL   ((1UL << 18UL))
                            /* FDMYECC Buffer FULL                      */
#define FSTATR_BPLDTCT  ((1UL << 17UL))
                            /* Block Protection Area ECC 2-Bit Error Detection Monitoring Bit  */
#define FSTATR_BPLCRCT  ((1UL << 16UL))
                            /* Block Protection Area ECC 1-Bit Error Correction Monitoring Bit */
#define FSTATR_FRDY     ((1UL << 15UL))
                            /* Flash Ready bit                          */
#define FSTATR_ILGLERR  ((1UL << 14UL))
                            /* Illegal-command error                    */
#define FSTATR_ERSERR   ((1UL << 13UL))
                            /* Erase command result                     */
#define FSTATR_PRGERR   ((1UL << 12UL))
                            /* Program command result                   */
#define FSTATR_SUSRDY   ((1UL << 11UL))
                            /* Suspend Ready                            */
#define FSTATR_DBFULL   ((1UL << 10UL))
                            /* Data Buffer Full                         */
#define FSTATR_ERSSPD   ((1UL << 9UL))
                            /* Erase-suspend                            */
#define FSTATR_PRGSPD   ((1UL << 8UL))
                            /* Program-suspend                          */
#define FSTATR_FHVEERR  ((1UL << 6UL))
                            /* Flash P/E_protection-error               */
#define FSTATR_CFGDTCT  ((1UL << 5UL))
                            /* Ecc 2 bit error (CFG read)               */
#define FSTATR_CFGCRCT  ((1UL << 4UL))
                            /* Ecc 1 bit error correction(CFG read)     */
#define FSTATR_TBLDTCT  ((1UL << 3UL))
                            /* Ecc 2 bit error (WPT read)               */
#define FSTATR_TBLCRCT  ((1UL << 2UL))
                            /* Ecc 1 bit error correction(WPT read)     */

/* Flash Access Status Register bit */
#define FASTAT_CMDLK   ((uint8_t)(1UL << 4UL))
                            /* Command Lock bit */
#define FASTAT_DFAE    ((uint8_t)(1UL << 3UL))
                            /* Data Flash Access Error bit */
#define FASTAT_CFAE    ((uint8_t)(1UL << 7UL))
                            /* Code Flash Access Error bit */

/* Data Flash Blank Check Status Register bit */
#define FBCSTAT_BCST   ((uint8_t)(1UL << 0UL))
                            /* Blank Check Status bit */
#define FPSADDR_VALID_BIT   (0x001FFFFFUL)
                            /* FPSADDR valid bit */

/* Flash Cache/BWC Buffer Control Register bit */
#define FCBWCTL_ICHCLR  ((1UL << 1UL))
                            /* clears the instruction cache and BWC Buffer ( Buffer for the Code FlashInterface). */

/* Flash Sequencer Access Request Register 0 bit */
#define PEFSACS0REQ_ACSREQ          (1UL << 0UL)
                                                            /* ACS_REQ   */
#define PEFSACS1REQ_ACSREQ          (1UL << 0UL)
                                                            /* ACS_REQ   */
/* Flash Programming Request Acknowledge register 0 bit */
#define ICUM_FLPRG0ACK_ACSACKA0     (1UL << 2UL)
                                                            /* ACS_ACKA0 */
#define ICUM_FLPRG0ACK_ACSACK0      (1UL << 1UL)
                                                            /* ACS_ACK0  */
/* Flash Programming Request Acknowledge register 1 bit */
#define ICUM_FLPRG1ACK_ACSACKA1     (1UL << 2UL)
                                                            /* ACS_ACKA1 */
#define ICUM_FLPRG1ACK_ACSACK1      (1UL << 1UL)
                                                            /* ACS_ACK1  */

/* Key code protection register */
/* bit 31 to 1 KCPROT[31:1] */
/*                Enable or disable modification of the KCE bit. */
/*                The value written is not retained. These bits are always read as 0.*/
/* bit 0       KCE Key :  Code Enable bit */
/*                1: Enables write access of protected registers. */
#define KCE_EN          (0xa5a5a501UL)

/* bit 0       KCE Key :  Code Enable bit */
/*   0: Disables write access of protected registers. */
#define KCE_DIS         (0xa5a5a500UL)

/* ECC Error disable */
#define ICUM_ECC_ERROR_DIS (0x00000003UL)

/* Data Flash ECC Error Notification Enable */
#define ICUM_DFERRINT_EN (0x00000007UL)

/* Enables bus guard setting */
#define ICUM_PBGPROT_EN (0x00000141UL)

#define FBCCNT_INC_MODE (0UL)
                                            /* Blank Check (Incremental mode) */
#define FBCCNT_DEC_MODE (1UL)
                                            /* Blank Check (Decremental mode) */

/* FSAADR & FEAADR bit20-bit0 MASK */
#define FSEADDR_MASK   (0x001FFFFFUL)

/* FENTRYR related definition */
#define FENTRYR_KEY       (0xAA00UL)
#define FENTRYR_BIT_MASK  (0x00FFU)

#define FSUINITR_KEY      (0x2D00U)
#define FECCTMD_VAL       (0xA630U)

#define D_FLS_LSB_MASK      (0xFFUL)
#define D_FLS_SHIFT_8BIT    (8UL)
#define D_FLS_SHIFT_16BIT   (16UL)
#define D_FLS_SHIFT_24BIT   (24UL)
#define D_FLS_INDEX_1       (1UL)
#define D_FLS_INDEX_2       (2UL)
#define D_FLS_INDEX_3       (3UL)

/*
 Data Flash ECC Error
*/

/* ECC Error Clear */
#define DFSERSTC_ERR_CLR (0x0000000FUL)
#define DFDERSTC_ERR_CLR (0x00000001UL)
/* ECC 1bit Error */
#define DFERSTR_ECC_SEDF (0x00000001UL)
/* ECC 2bit Error */
#define DFERSTR_ECC_DEDF (0x00000002UL)

/*
 FACI timeout [count] = (Max time x 1.5[Margin] x OST count / 1msec)
*/
/* CF Max time */
#define TIMEOUT_ERASE_CF    (396000UL / 2UL * 3UL * OST_COUNTS_OF_USEC)
#define TIMEOUT_PROGRAM_CF  (  3200UL / 2UL * 3UL * OST_COUNTS_OF_USEC)

/* DF Max time */
#define TIMEOUT_ERASE_DF    (212000UL / 2UL * 3UL * OST_COUNTS_OF_USEC)
#define TIMEOUT_PROGRAM_DF  (  1400UL / 2UL * 3UL * OST_COUNTS_OF_USEC)
#define TIMEOUT_ERASE_SEC   (120000UL / 2UL * 3UL * OST_COUNTS_OF_USEC)
#define TIMEOUT_PROGRAM_SEC (  2810UL / 2UL * 3UL * OST_COUNTS_OF_USEC)
#define TIMEOUT_ERASE_SW    (121800UL / 2UL * 3UL * OST_COUNTS_OF_USEC)
#define TIMEOUT_PROGRAM_SW  (  1900UL / 2UL * 3UL * OST_COUNTS_OF_USEC)
#define TIMEOUT_ERASE_TAG   (121800UL / 2UL * 3UL * OST_COUNTS_OF_USEC)
#define TIMEOUT_UPDATE_TAG  (  1820UL / 2UL * 3UL * OST_COUNTS_OF_USEC)

/* Blank check Max time */
#define TIMEOUT_BLANK_CHECK (   500UL / 2UL * 3UL * OST_COUNTS_OF_USEC)

/* Max counts of check FRDY bit */
#define LIMIT_CHECK_FRDY    (200000UL)

/*
 FACI Resource Management Status
*/
#define D_FLS_IF0_IDLE      (0x00U)
                            /* CF:Free, DF:Free, FACI0 User:None (Idle state) */
#define D_FLS_IF0_HOSTCF    (0x01U)
                            /* CF:Lock, DF:Free, FACI0 User:Host (Code flash accessing state by Host) */
#define D_FLS_IF0_HOSTDF    (0x02U)
                            /* CF:Free, DF:Lock, FACI0 User:Host (Data flash accessing state by Host) */
#define D_FLS_IF0_ICUMCF    (0x03U)
                            /* CF:Lock, DF:Free, FACI0 User:ICUM (Code flash accessing state by ICUM) */
#define D_FLS_IF0_ICUMDF    (0x04U)
                            /* CF:Free, DF:Lock, FACI0 User:ICUM (Data flash accessing state by ICUM) */
#define D_FLS_IF0_ICUMALL   (0x05U)
                            /* CF:Lock, DF:Lock, FACI0 User:ICUM (All region accessing state by ICUM) */
#define D_FLS_IF0_CONFIG    (0x06U)
                            /* CF:Lock, DF:Lock, FACI0 Config (All region accessing state by ICUM) */
#define D_FLS_IF0_STA_TERM  (0x07U)

#define D_FLS_IF2_IDLE      (0x00U)
                            /* SDF:Free, FACI2 User:None (Idle state) */
#define D_FLS_IF2_ICUMDF    (0x01U)
                            /* SDF:Lock, FACI2 User:ICUM (Secure data flash accessing state by ICUM) */
#define D_FLS_IF2_STA_TERM  (0x02U)

/*
 FACI Resource Management Event
*/
#define D_FLS_IF0_HOST_CF_ACQ       (0x00U)
#define D_FLS_IF0_HOST_CF_REL       (0x01U)
#define D_FLS_IF0_HOST_DF_ACQ       (0x02U)
#define D_FLS_IF0_HOST_DF_REL       (0x03U)
#define D_FLS_IF0_CF_PE_ACK         (0x04U)
#define D_FLS_IF0_ICUM_CF_ACQ       (0x05U)
#define D_FLS_IF0_ICUM_CF_REL       (0x06U)
#define D_FLS_IF0_ICUM_DF_ACQ       (0x07U)
#define D_FLS_IF0_ICUM_DF_REL       (0x08U)
#define D_FLS_IF0_ICUM_EXT_ACQ      (0x09U)
#define D_FLS_IF0_ICUM_EXT_REL      (0x0aU)
#define D_FLS_IF0_EVT_TERM          (0x0bU)

#define D_FLS_IF2_ICUM_DF_ACQ       (0x00U)
#define D_FLS_IF2_ICUM_DF_REL       (0x01U)
#define D_FLS_IF2_EVT_TERM          (0x02U)

/* code that means rac_double is invalid  (non OTA) */
#define D_FLS_INVALID  (0xFFFFFFFFuL)

/* flash status definition */
#define D_FLS_STS_UNINITIALIZE      (0x00000000UL)
                                        /* Uninitialize */
#define D_FLS_STS_DRV_INITIALIZED   (0x00000001UL)
                                        /* Driver Initialized */
#define D_FLS_STS_MAP_INITIALIZED   (0x00000002UL)
                                        /* Map Initialized */
#define D_FLS_STS_AVAILABLE         (D_FLS_STS_DRV_INITIALIZED | D_FLS_STS_MAP_INITIALIZED)
                                        /* Available status */

/* address used by SIC remap function */
#define SICREMAP_DEFAULT_ADDRESS   (0xFC000000uL)
#define SICREMAP_UPPER_ADDRESS     (0xFDFFFFFFuL)

/* FACI base addresses for ICUM */
/* Area containing the various registers of the hardware base addresses */
#define FACI_HW_BASE_ADDR_IF0   (0xFFB10000UL)      /* FACI0 */
#define FACI_HW_BASE_ADDR_IF2   (0xFFB18000UL)      /* FACI2 */
/* FACI command-issuing area base addresses */
#define FACI_CMD_BASE_ADDR_IF0  (0xFFB20000UL)      /* FACI0 */
#define FACI_CMD_BASE_ADDR_IF2  (0xFFB40000UL)      /* FACI2 */

/* flash mode switching response */
#define FLS_MODE_SW_RESP_CLR        (0x00U)
#define FLS_MODE_SW_RESP_ACK        (0x01U)
#define FLS_MODE_SW_RESP_NACK       (0x80U)

/********************************************************************************
 Typedef definitions
********************************************************************************/

/*
 FACI register structure
*/

typedef struct
{
    /* command parameter */
    /* command status                                       */
    uint32_t d_status;

    /* FCU Operation mode                                   */
    /*  0x00 : read mode                                    */
    /*  0x80 : DF P/E mode                                  */
    /*  0x01 : CF P/E mode                                  */
    uint16_t ope_mode;

    /* command request                                      */
    /*  1 : accepted the request by the following API       */
    /*         D_FLS_CancelProc                             */
    /*         D_FLS_ProgramDf                              */
    /*         D_FLS_EraseDf                                */
    /*         D_FLS_ProgramCf                              */
    /*         D_FLS_EraseCf                                */
    /*         D_FLS_UpdateSecurity                         */
    uint8_t commad_req;

    /* FACI command code                                    */
    uint8_t command_code;

    /* FACI sub command code                                */
    uint8_t sub_command_code;

    /* flash memory address                                 */
    /*  Code flash / data flash address to be processed in  */
    /*  FACI sequence. (relative address from the beginning */
    /*  of each area)                                       */
    uint32_t flash_addr;

    /* data buffer address                                  */
    /*  Set the address of the buffer where the data to be  */
    /*  written is stored. For erase and blank check, set   */
    /*  NULL address.                                       */
    const uint8_t *p_data;

    /* total data size [byte]                               */
    /*  Total number of bytes processed in FACI sequence    */
    uint32_t data_size_in_bytes;

    /* pre processing function address                      */
    /*  Register a function that performs a procedure       */
    /*  specific to a command to be executed before         */
    /*  switching to PE mode.                               */
    uint32_t (*p_pre_processing)(void);

    /* FACI command set function address                    */
    /*  Register functions that generate command-specific   */
    /*  FACI command packets.                               */
    uint32_t (*p_faci_commad_set)(void);

    /* write data size [byte] get function address          */
    /*  Register a function to acquire the number of bytes  */
    /*  to be processed in one FACI command packet.         */
    uint32_t (*p_write_size_in_bytes)(const uint32_t faddr, uint32_t *p_size);

    /* P/E abort check function address                     */
    /*  Register functions that perform error checking and  */
    /*  processing continuation determination procedures.   */
    uint32_t (*p_error_check)(void);

    /* Response at normal termination                       */
    /*  Sets the status value to be set to the LWFL status  */
    /*  when the normal termination condition is satisfied. */
    /*  ER_OK or E_R_LWFL_CANCELLED or E_R_LWFL_NON_BLANK   */
    uint32_t success_resp;

    /* Number of processed data [byte]                      */
    /*  Number of processed data bytes in FACI sequence.    */
    /*  Clear 0 at the start of the FACI sequence in API    */
    /*  Function                                            */
    uint32_t p_num;

    /* Start of FACI command [OST hard count value]         */
    uint32_t faci_start_time;

    /* Number of checks FACI command Completion             */
    uint32_t faci_check_counter;

    /* Max time [number of OSTcount] with Margin            */
    uint32_t max_time;

    /* Driver control parameter setting function address    */
    /*  Register the driver control parameter setting       */
    /*  processing function of the FACI command that is     */
    /*  continuously requested.                             */
    uint32_t (*p_set_ctrl_prm)(void);

} s_FLS_Ctrl_Prm_Type_t;

/* job information */
typedef struct fls_ctx
{
    uint16_t        id;                         /* Context Id               */
    uint8_t         priority;                   /* Priority                 */
    uint32_t        target_area;                /* Target Area              */
    uint32_t        target_bank;                /* Target Bank              */
} fls_ctx_t;

/* D_FLS management information */
typedef struct fls_job_ctrl
{
    uint8_t                 status;            /* FlashDrv status       */
    fls_ctx_t               context;           /* FlashDrv context      */
    s_FLS_Ctrl_Prm_Type_t   prm;               /* FLS Control Parameter */
} fls_ctrl_t;

/* Flash address infomation */
typedef struct d_flash_info
{
    uint32_t     area_top_addr;
    uint32_t     area_end_addr;
} d_flash_info_t;

/* Save area for D_FLS_UpdateSecurity function parameters */
typedef struct s_FLS_Update_Sec_Prm_Type
{
    /* Start address of security setting area             */
    uint32_t        target_addr;
    /* Start address of the security buffer to be updated */
    const uint8_t   *p_updated_field;
    /* Size to update                                     */
    uint32_t        size_field;
} s_FLS_Update_Sec_Prm_Type_t;

/********************************************************************************
 Link to external constants
********************************************************************************/
extern uint8_t  __ghsbegin_start_DF_BANK_A[];
extern uint8_t  __ghsbegin_end_DF_BANK_A[];
extern uint8_t  __ghsbegin_start_DF_BANK_B[];
extern uint8_t  __ghsbegin_end_DF_BANK_B[];
extern uint8_t  __ghsbegin_start_DF_BANK_ICUM[];
extern uint8_t  __ghsbegin_end_DF_BANK_ICUM[];

extern uint8_t  __ghsbegin_start_gRAM[];
extern uint8_t  __ghsbegin_end_gRAM[];
extern uint8_t  __ghsbegin_start_iRAM_Self[];
extern uint8_t  __ghsbegin_end_iRAM_Self[];

extern uint8_t  __ghsbegin_cism_bss[];
extern uint8_t  __ghsend_cism_bss[];

extern uint8_t  __ghsbegin_start_sROM_SIDE_A[];
extern uint8_t  __ghsbegin_end_sROM_SIDE_A[];
extern uint8_t  __ghsbegin_start_sROM_SIDE_B[];
extern uint8_t  __ghsbegin_end_sROM_SIDE_B[];

extern uint8_t  __ghsbegin_start_iROM_BANK_A[];
extern uint8_t  __ghsbegin_end_iROM_BANK_A[];
extern uint8_t  __ghsbegin_start_iROM_BANK_B[];
extern uint8_t  __ghsbegin_end_iROM_BANK_B[];
extern uint8_t  __ghsbegin_start_iROM_BANK_C[];
extern uint8_t  __ghsbegin_end_iROM_BANK_C[];
extern uint8_t  __ghsbegin_start_iROM_BANK_D[];
extern uint8_t  __ghsbegin_end_iROM_BANK_D[];
extern uint8_t  __ghsbegin_start_iROM_BANK_E[];
extern uint8_t  __ghsbegin_end_iROM_BANK_E[];
extern uint8_t  __ghsbegin_start_iROM_BANK_F[];
extern uint8_t  __ghsbegin_end_iROM_BANK_F[];

extern uint8_t  __ghsbegin_start_iROM_BANK_S0[];
extern uint8_t  __ghsbegin_end_iROM_BANK_S0[];
extern uint8_t  __ghsbegin_start_iROM_BANK_S1[];
extern uint8_t  __ghsbegin_end_iROM_BANK_S1[];

extern uint8_t __ghsbegin_SECRET_DATA_SECTION[];
extern uint8_t __ghsend_SECRET_DATA_SECTION[];

/* ICU-M firmware start address */
extern uint8_t __ghsbegin_start_firmware[];
extern uint8_t __ghsbegin_end_firmware[];
extern uint8_t __ghsbegin_start_firmware_a[];
extern uint8_t __ghsbegin_end_firmware_a[];
extern uint8_t __ghsbegin_start_firmware_b[];
extern uint8_t __ghsbegin_end_firmware_b[];

/********************************************************************************
 Exported global variables (extern).
********************************************************************************/

/* selected fpsys FACI0 or FACI2 */
extern uint32_t g_fls_selected_fpsys;
/* FLS control parameter */
extern fls_ctrl_t g_fls_ctrl[NB_FPSYS];
extern fls_ctrl_t g_fls_ctrl_bak[NB_FPSYS];

extern uint32_t g_fls_status;

extern uint8_t g_d_cf_pe_notify;

extern uint32_t g_d_cf_pe_notify_faci;

extern uint8_t g_usr_df_prog_req;

extern uint8_t g_sec_df_prog_res;

extern p_d_fls_cfint_addr *p_d_fls_cbfunc_addr;

extern uint16_t g_fls_cpu_frequency_MHz;

/********************************************************************************
 Exported global functions (to be accessed by other files)
********************************************************************************/
void __SYNCI(void);
void __SYNCP(void);
void __SYNCM(void);

/*
 icum_d_flash.c
*/
uint32_t d_FLS_CheckFaciErrors_ram(void);
uint32_t d_FLS_IssueCommand_ram(void);
uint32_t d_FLS_SelectFpsys_ram(const uint32_t flash_addr);
uint32_t d_FLS_Handler_ram(const uint32_t fpsys);
void d_FLS_SetBusGuardFromPE_ram(void);
void d_FLS_ClrBusGuardFromPE_ram(void);
uint32_t d_FLS_HandlerCommandReq_ram(uint32_t fpsys);

/*
 icum_d_flash_map.c
*/
uint32_t d_FLS_GetCFBankNumber(const uint32_t cf_addr, uint32_t *p_bank);

/*
 icum_d_flash_state.c
*/
uint32_t d_FLS_StateMachineIf0(const uint8_t event_code);
uint32_t d_FLS_StateMachineIf1(const uint8_t event_code);
uint32_t d_FLS_StateMachineIf2(const uint8_t event_code);

/*
 icum_d_asm.850
*/
uint32_t d_FLS_CheckFcuReg_ram(volatile uint32_t *p_reg_adr, uint32_t mask, uint32_t chk_reg_data,
                                                                                               uint32_t time_out_cnt);
uint32_t d_FLS_CheckFcuRegOrState_ram(volatile uint32_t *p_reg_adr, uint32_t mask, uint32_t chk_reg_data,
                                                                                               uint32_t time_out_cnt);
uint32_t d_FLS_CheckFcuRegU16_ram(volatile uint16_t *p_reg_adr, uint16_t mask, uint16_t chk_reg_data,
                                                                                               uint32_t time_out_cnt);

#endif /* #if !defined(ICUM_D_FLASH_H) */

