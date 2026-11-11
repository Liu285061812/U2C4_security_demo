/******************************************************************************/
/* Component Name   ICUM MPU Driver                                           */
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

#if !defined(ICUM_D_MPU_H)
#define ICUM_D_MPU_H

extern uint8_t __ghsbegin_start_icum_ram_protect[];
extern uint8_t __ghsend_end_icum_ram_protect[];
extern uint8_t __ghsbegin_start_icum_startup_protect[];
extern uint8_t __ghsend_end_icum_startup_protect[];
extern uint8_t __ghsbegin_start_Protect_A1[];
extern uint8_t __ghsbegin_end_Protect_A1[];
extern uint8_t __ghsbegin_start_Protect_A2[];
extern uint8_t __ghsbegin_end_Protect_A2[];
extern uint8_t __ghsbegin_start_gRAM_BANK_C_ICUM[];
extern uint8_t __ghsbegin_end_gRAM_BANK_C_ICUM[];
/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/

/* ***Internal Definition*** */
#define MPU_WORDBOUNDARY                (0x00000000UL)      /* Word Boundary */

/* Setting Value to MPM Register */
  /* Disable Value of MPM Register */
#define MPU_MPM_DISABLE                 (0x00000000UL)
  /* DX=Enable, DW=Enable, DR=Enable, */
  /* SVP=Restrict access according to the SX,SW,and SR of MPAT Register, */
  /* MPE=Enable*/
#define MPU_MPM_ENABLE                  (0x00000703UL)

/* Setting Value to MPRC Register */
  /* Reset Value of MPRC Register */
#define MPU_MPRC_RESET                  (0x00000000UL)
/* Enable Region 0, 1, 2, 3 */
#define MPU_MPRC_ENABLEREG              (0x0000000FUL)

/* Region Address Value of MPLAx and MPUAx Registers */
#define MPU_REG0_STARTADRS  __ghsbegin_start_icum_ram_protect  /* Region 0 : Start Address (Code Area on RAM) */
#define MPU_REG0_ENDADRS    __ghsend_end_icum_ram_protect      /* Region 0 : End Address */

#define MPU_REG1_STARTADRS  __ghsbegin_start_icum_startup_protect  /* Region 1 : Start Address (Code Area on Startup ROM) */
#define MPU_REG1_ENDADRS    __ghsend_end_icum_startup_protect      /* Region 1 : End Address */

#define MPU_REG2_STARTADRS  __ghsbegin_start_Protect_A1        /* Region 2 : Start Address */
#define MPU_REG2_ENDADRS    __ghsbegin_end_Protect_A1          /* Region 2 : End Address */

#define MPU_REG3_STARTADRS  __ghsbegin_start_Protect_A2        /* Region 3 : Start Address */
#define MPU_REG3_ENDADRS    __ghsbegin_end_Protect_A2          /* Region 3 : End Address */

/* start address of cluster RAM for ICUM */
#define MPU_CRAM_ICUM_START_ADDR        __ghsbegin_start_gRAM_BANK_C_ICUM
#define MPU_CRAM_ICUM_SIZE              (uint32_t)(__ghsbegin_end_gRAM_BANK_C_ICUM - __ghsbegin_start_gRAM_BANK_C_ICUM)
#define MPU_CRAM_INIT_VALUE             (0UL)

/* Protection Area Attribute Value of MPATx Registers */
/* E=Ena, G=1, SX=Dis, SW=Dis, SR=Dis, UX=Dis, UW=Dis, UR=Dis */
#define E_ENA                           (0x00000080UL)      /* Protection area setting of Region is enabled */
#define G_ENA                           (0x00000040UL)      /* Always 1 */
#define SX_DIS                          (0x00000000UL)      /* Execution is disabled in SV mode */
#define SW_DIS                          (0x00000000UL)      /* Writing is disabled in SV mode */
#define SR_DIS                          (0x00000000UL)      /* Reading is disabled in SV mode */
#define UX_DIS                          (0x00000000UL)      /* Execution is disabled in User mode */
#define UW_DIS                          (0x00000000UL)      /* Writing is disabled in User mode */
#define UR_DIS                          (0x00000000UL)      /* Reading is disabled in User mode */

#define SX_ENA                          (0x00000020UL)      /* Execution is enabled in SV mode */
#define SW_ENA                          (0x00000010UL)      /* Writing is enabled in SV mode */
#define SR_ENA                          (0x00000008UL)      /* Reading is enabled in SV mode */
#define UX_ENA                          (0x00000004UL)      /* Execution is enabled in User mode */
#define UW_ENA                          (0x00000002UL)      /* Writing is enabled in User mode */
#define UR_ENA                          (0x00000001UL)      /* Reading is enabled in User mode */

#define MPU_REG0_PROTATR                E_ENA+G_ENA+SX_ENA+SW_DIS+SR_ENA+UX_ENA+UW_DIS+UR_ENA  /* Region 0 : Attribute */
#define MPU_REG1_PROTATR                E_ENA+G_ENA+SX_ENA+SW_DIS+SR_ENA+UX_ENA+UW_DIS+UR_ENA  /* Region 1 : Attribute */
#define MPU_REG2_PROTATR                E_ENA+G_ENA+SX_DIS+SW_ENA+SR_ENA+UX_DIS+UW_ENA+UR_ENA  /* Region 2 : Attribute */
#define MPU_REG3_PROTATR                E_ENA+G_ENA+SX_DIS+SW_ENA+SR_ENA+UX_DIS+UW_ENA+UR_ENA  /* Region 3 : Attribute */

/* ***for MIP_MDP Handler*** */
#define MPU_FEIC_MASK                   (0x0000FFFFUL)      /* Mask Value of FEIC Register */
#define MPU_MIP_EXCEPT                  (0x00000090UL)      /* MIP Exception Code */

/* Return Value from Function */
#define MPU_NO_SPECIFY                  (0x00000000UL)      /* No Specified */
#define MPU_ADRS_BOUNDARY               (0x00000001UL)      /* Return Value (Address) from CallBack 16bit Boundary CHeck */

/* PSW.EP bit Mask Value */
#define MPU_EP_MASK                     (0xFFFFFFBFUL)      /* EP MASK of PSW */

/* for MEI Register */
#define MPU_MEI_MASK                    (0x0000003EUL)      /* Extruct ITYPE */
#define MPU_MEI_ITYPE00                 (0x00000000UL)      /* ITYPE = SLD and SST Instruction Group */
#define MPU_MEI_ITYPE01                 (0x00000001UL)      /* ITYPE = LD(disp16) and ST(disp16) Instruction Group */
#define MPU_MEI_ITYPE02                 (0x00000002UL)      /* ITYPE = LD(disp23) and ST(disp23) Instruction Group */
#define MPU_MEI_ITYPE07                 (0x00000007UL)      /* ITYPE = LDL.W and STC.W Instruction */
#define MPU_MEI_ITYPE08                 (0x00000008UL)      /* ITYPE = CAXI Instruction */
#define MPU_MEI_ITYPE09                 (0x00000009UL)      /* ITYPE = Bit Manipulation Group */
#define MPU_MEI_ITYPE0C                 (0x0000000CUL)      /* ITYPE = PREPARE and DISPOSE Instruction */
#define MPU_MEI_ITYPE0D                 (0x0000000DUL)      /* ITYPE = PUSHSP and POPSP Instruction */
#define MPU_MEI_ITYPE10                 (0x00000010UL)      /* ITYPE = SWITCH Instruction */
#define MPU_MEI_ITYPE11                 (0x00000011UL)      /* ITYPE = CALLT Instruction */
#define MPU_MEI_ITYPE12                 (0x00000012UL)      /* ITYPE = SYSCALL Instruction */
#define MPU_MEI_ITYPE15                 (0x00000015UL)      /* ITYPE = An exception occurs */

#define MPU_INSTRUCT_SIZ02              (0x00000002UL)      /* Instruction Size = 2 Bytes */
#define MPU_INSTRUCT_SIZ04              (0x00000004UL)      /* Instruction Size = 4 Bytes */
#define MPU_INSTRUCT_SIZ06              (0x00000006UL)      /* Instruction Size = 6 Bytes */
#define MPU_INSTRUCT_SIZ08              (0x00000008UL)      /* Instruction Size = 8 Bytes */
#define MPU_INSTRUCT_SIZERR             (0x00000000UL)      /* Instruction Size Error */

/* For Analyzing PREPARE Instruction */
#define MPU_PREPARE_MASK                (0x07C0U)           /* MASK for PREPARE Instruction */
#define MPU_PREPARE_INSTRUCT            (0x0780U)           /* PREPARE Instruction Code */
#define MPU_PREPARE_FFEXT               (0x0018U)           /* Extruct FF Bits */
#define MPU_PREPARE_FFEXTSHFT           (3U)                /* Shift Right for FF Bits */
#define MPU_PREPARE_FF00                (0x0000U)           /* FF Bits = 00b */
#define MPU_PREPARE_FF01                (0x0001U)           /* FF Bits = 01b */
#define MPU_PREPARE_FF02                (0x0002U)           /* FF Bits = 02b */
#define MPU_PREPARE_FF03                (0x0003U)           /* FF Bits = 03b */

/* Upper limit address correction value of protected area */
#define MPU_MPUAREG_UPLMT_CORRECTION    (4UL)

/* CRAM guard register */
#define REG_GUARD_CRAMCSG0_BASE         (0xFFC6E000UL)
#define REG_CSGPROT0                    (*(volatile uint32_t *)REG_GUARD_CRAMCSG0_BASE)
#define REG_CSGSPID0                    (*(volatile uint32_t *)(REG_GUARD_CRAMCSG0_BASE + 0x04U))
#define REG_CSGBAD0                     (*(volatile uint32_t *)(REG_GUARD_CRAMCSG0_BASE + 0x08U))
#define REG_CSGADV0                     (*(volatile uint32_t *)(REG_GUARD_CRAMCSG0_BASE + 0x0cU))

/* Value definition for set to SPIDMSK */
#define MPU_VALUE_SPIDMSK_ICUMDBGMST    (0x80000000UL)
#define MPU_VALUE_SPIDMSK_ICUM          (0x04000000UL)
#define MPU_VALUE_SPIDMSK_OTHER         (~(0x06000000UL))
#define MPU_VALUE_SPIDMSKLOCK           (0x00000001UL)

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* None */

/*====================================================================================*/
/* Union for pointer manipulation                                                     */
/*====================================================================================*/
typedef union ptr_pc
{
    uint16_t    *p_ptr;
    uint32_t    val;
} ptr_pc_t;

typedef union ptr_adrs
{
    uint8_t     *p_adrs;
    uint32_t    val;
} ptr_adrs_t;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
static void d_MPU_AnalyzeFactor(void);

#endif /* #if !defined(ICUM_D_MPU_H) */
