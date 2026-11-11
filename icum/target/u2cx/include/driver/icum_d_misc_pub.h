/******************************************************************************/
/* Component Name   ICUM MISC Driver                                          */
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

#if !defined(ICUM_D_MISC_PUB_H)
#define ICUM_D_MISC_PUB_H
#include ICUM_DEVICE_FILE

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/*  System register definition */
/* to be used with above defined macro LDSR / STSR */
#define PSW         5, 0
#define EIPSW       1, 0
#define INTBP       4, 1
#define MCTL        5, 1
#define ICCFG       26, 4
#define DCCFG       27, 4
#define MCFG0       0, 1

#define EIPC        0, 0
#define FEPC        2, 0
#define EIIC        13, 0
#define FEIC        14, 0
#define EIPSW       1, 0
#define FEPSW       3, 0
#define RBASE       2, 1
#define EBASE       3, 1

#define ISPR        10, 2
#define PMR         11, 2
#define ICSR        12, 2
#define INTCFG      13, 2

#define HTCFG0      0, 2
#define MEA         6, 2
#define MEI         8, 2

/* below registers for MPU management */

#define MPM         0, 5
#define MPRC        1, 5
#define MPBRGN      4, 5
#define MPTRGN      5, 5
#define MCA         8, 5
#define MCS         9, 5
#define MCC        10, 5
#define MCR        11, 5

#define MPAT0       2, 6
#define MPAT1       6, 6
#define MPAT2      10, 6
#define MPAT3      14, 6
#define MPAT4      18, 6
#define MPAT5      22, 6
#define MPAT6      26, 6
#define MPAT7      30, 6
#define MPAT8       2, 7
#define MPAT9       6, 7
#define MPAT10     10, 7
#define MPAT11     14, 7
#define MPAT12     18, 7
#define MPAT13     22, 7
#define MPAT14     26, 7
#define MPAT15     30, 7

#define MPLA0       0, 6
#define MPLA1       4, 6
#define MPLA2       8, 6
#define MPLA3      12, 6
#define MPLA4      16, 6
#define MPLA5      20, 6
#define MPLA6      24, 6
#define MPLA7      28, 6
#define MPLA8       0, 7
#define MPLA9       4, 7
#define MPLA10      8, 7
#define MPLA11     12, 7
#define MPLA12     16, 7
#define MPLA13     20, 7
#define MPLA14     24, 7
#define MPLA15     28, 7

#define MPUA0       1, 6
#define MPUA1       5, 6
#define MPUA2       9, 6
#define MPUA3      13, 6
#define MPUA4      17, 6
#define MPUA5      21, 6
#define MPUA6      25, 6
#define MPUA7      29, 6
#define MPUA8       1, 7
#define MPUA9       5, 7
#define MPUA10      9, 7
#define MPUA11     13, 7
#define MPUA12     17, 7
#define MPUA13     21, 7
#define MPUA14     25, 7
#define MPUA15     29, 7


/* WRITE_PROT_REG Retry Count */
#define MISC_PROT_RETRY    (0x000000FFUL)

/* Various flag settings for the system registers */
/* PSW flags */
#define SET_UM_FLAG        (0x40000000UL)

/* Unprotecting/Protecting Data */
#define UNLOCK_DATA        (0xA5A5A501UL)
#define LOCK_DATA          (0xA5A5A500UL)

/* Flash preload enable/disable */
#define PRELOAD_ON         (1UL)
#define PRELOAD_OFF        (0UL)

/* PEID Definition */
#define MISC_PEID_PE0      (0u)         /* PE0 */
#define MISC_PEID_PE1      (1u)         /* PE1 */
#define MISC_PEID_PE2      (2u)         /* PE2 */
#define MISC_PEID_ICUP     (7u)         /* ICUP{Fixed] */
#define TOTAL_PE_NUM       (2U)

/* Definition of Return value for D_MISC_CheckArea API */
#define AREA_CFWINDOW         (0U)                /* CF Remap Access Window Area */
#define AREA_REMAP            (1U)                /* Remap Area */
#define AREA_SICWINDOW        (2U)                /* SIC Remap Access Window Area */
#define AREA_LOCALRAM         (3U)                /* Local RAM Area */
#define AREA_PKCCRAM          (4U)                /* PKCC RAM Area */
#define AREA_LOCALOTHER       (5U)                /* Other Local RAM */

/**************************************************************************************/
/* GHS Macros                                                                         */
/**************************************************************************************/
/* _Pragma Operator */
#define PRAGMA(x) _Pragma(#x)
/* Change section */
#define CHANGE_SEC(sec, name) PRAGMA( ghs section sec = name )
/* Align to n bytes boundary. */
#define ALIGNVAR(n) PRAGMA( alignvar(n) )

/**************************************************************************************/
/* ASM Macros                                                                         */
/**************************************************************************************/
/* ASM macro */
#define ASM_MACRO(x)   __asm(x)

/* Prototypes */
static uint32_t BSW(uint32_t data);
static void NOPS(uint16_t count);
static void SET_HALT(void);
static void DISABLE_INTERRUPT(void);
static void ENABLE_INTERRUPT(void);
static void LDSR(uint32_t regValue, int8_t regID, int8_t selID);
static uint32_t STSR(int8_t regID, int8_t selID);
static uint32_t GETLP(void);
static void SYNC_PIPELINE(void);
static void SYNC_INSTRUCTION(void);
static void SYNC_MEMORY(void);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0001]:[ICUMFW_UD_U2B_D_MISC_0001]
* Function name: BSW
* Description  : Byte Swap
* Arguments    : data    UINT 32 bii Source Data
*                
* Return Value : Byte Swapped Data
***************************************************************************************/
static asm uint32_t BSW(uint32_t data)
{
%reg data
    bsw    data, r10
%nearmem data
    ld.w   data, r10
    bsw    r10, r10
%con data
    mov    data, r10
    bsw    r10, r10
%farsprel data
    movhi  hi(%SPOFF(data)), sp, r6
    ld.w   lo(%SPOFF(data))[r6], r10
    bsw    r10, r10
%farmem data
    movhi  hi(data), r0, r6
    ld.w   lo(data)[r6], r10
    bsw    r10, r10
%mem data
    ld.w   data[r0], r10
    bsw    r10, r10
%error
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0002]:[ICUMFW_UD_U2B_D_MISC_0002]
* Function name: NOPS
* Description  : Repeat NOP Instruction 
* Arguments    : count  Number of NOP Instruction
*                
* Return Value : None
***************************************************************************************/
static asm void NOPS(uint16_t count)
{
%con count
.rept count
    nop
.endr
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0003]:[ICUMFW_UD_U2B_D_MISC_0003]
* Function name: SET_HALT
* Description  : Holt Instruction
* Arguments    : None
*                
* Return Value : None
***************************************************************************************/
static asm void SET_HALT(void)
{
    #ifndef DEBUG
    halt
    #else
    nop
    #endif
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0004]:[ICUMFW_UD_U2B_D_MISC_0004]
* Function name: DISABLE_INTERRUPT
* Description  : Disable EI Level Maskable Interrupt
* Arguments    : None
*                
* Return Value : None
***************************************************************************************/
static asm void DISABLE_INTERRUPT(void)
{
    di
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0005]:[ICUMFW_UD_U2B_D_MISC_0005]
* Function name: ENABLE_INTERRUPT
* Description  : Enable EI Level Maskable Interrupt
* Arguments    : None
*                
* Return Value : None
***************************************************************************************/
static asm void ENABLE_INTERRUPT(void)
{
    ei
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0006]:[ICUMFW_UD_U2B_D_MISC_0006]
* Function name: LDSR
* Description  : Set Value to Basic System Register
* Arguments    : regValue  : Setting Data
*                regID     : Register ID
*                selID     : Selection ID
* Return Value : None
***************************************************************************************/
static asm void LDSR(uint32_t regValue, int8_t regID, int8_t selID)
{
%nearmem regValue, %con regID, selID
    ld.w    regValue, r10
    ldsr    r10, regID, selID
%reg regValue, %con regID, selID
    ldsr    regValue, regID, selID
%error
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0007]:[ICUMFW_UD_U2B_D_MISC_0007]
* Function name: STSR
* Description  : Get Data from Basic System Register
* Arguments    : regID     : Register ID
*                selID     : Selection ID
* Return Value : Reading Data
***************************************************************************************/
static asm uint32_t STSR(int8_t regID, int8_t selID)
{
%con regID, selID
    stsr    regID, r10, selID
%error
}

/***************************************************************************************
* Function ID  : (not supported)
* Function name: GETLP
* Description  : Get Data from LP Register
* Arguments    : None
*                
* Return Value : Reading Data(Address) from LP Register
***************************************************************************************/
static asm uint32_t GETLP(void)
{
    mov     lp, r10
}

/***************************************************************************************
* Function ID  : (not supported)
* Function name: SUPERVISOR_MODE
* Description  : Set Superviser Mode (PSW Register)
* Arguments    : None
*                
* Return Value : None
***************************************************************************************/
#define SUPERVISOR_MODE() \
    LDSR((STSR(PSW) & (~SET_UM_FLAG)), PSW)

/***************************************************************************************
* Function ID  : (not supported)
* Function name: USER_MODE
* Description  : Set User Mode (PSW Register)
* Arguments    : None
*                
* Return Value : None
***************************************************************************************/
#define USER_MODE() \
    LDSR((STSR(PSW) | SET_UM_FLAG), PSW)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0011]:[ICUMFW_UD_U2B_D_MISC_0011]
* Function name: GET_MY_PEID Macro
* Description  : Retrieve the PEID from the thread configurator register
* Arguments    : None
*                
* Return Value : Reading Data(PEID) from HTCFG0 Register
***************************************************************************************/
#define GET_MY_PEID     (STSR(HTCFG0) >> 16U)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0012]:[ICUMFW_UD_U2B_D_MISC_0012]
* Function name: ASSERT Macro
* Description  : Assertion
* Arguments    : Conditional Sentence  (ex. A != B)
*                
* Return Value : None
***************************************************************************************/
#if defined(_RELEASE_EVAL_) || defined(_RELEASE_MP_)
#define ASSERT(X)
#else
#define ASSERT(X) \
    do\
    {\
        if ((X))\
        {\
        }\
        else\
        {\
            while (1)\
            {\
            }\
        }\
    } while (0)
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0013]:[ICUMFW_UD_U2B_D_MISC_0013]
* Function name: UNLOCK_REG_PROT Macro
* Description  : Unlock Protection Register
* Arguments    : reg  : Protection Register
*                
* Return Value : None
***************************************************************************************/
#define UNLOCK_REG_PROT(reg)  ((reg) = (uint32_t)UNLOCK_DATA)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0014]:[ICUMFW_UD_U2B_D_MISC_0014]
* Function name: LOCK_REG_PROT Macro
* Description  : Lock Protection Register
* Arguments    : reg  : Protection Register
*                
* Return Value : None
***************************************************************************************/
#define LOCK_REG_PROT(reg)  ((reg) = (uint32_t)LOCK_DATA)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0015]:[ICUMFW_UD_U2B_D_MISC_0015]
* Function name: WRITE_PROT_REG Macro
* Description  : Write Data to Protected Register
* Arguments    : reg    : Gurded(Protected) Register for writing data
*                value  : Writing Data
* Return Value : None
***************************************************************************************/
#define WRITE_PROT_REG(reg, value, count) \
    do\
    {\
        do\
        {\
            (ICU_GEN0ICUM_PROT0PCMD) = 0xA5UL;\
            (reg) = (value);\
            (reg) = ~(value);\
            (reg) = (value);\
            if (ICU_GEN0ICUM_PROT0PS == 0UL)\
            {\
                break;\
            }\
            else\
            {\
            }\
            (count) -= 1UL;\
        } while ((count) != 0UL);\
    } while (0)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0016]:[ICUMFW_UD_U2B_D_MISC_0016]
* Function name: DISABLE_INTERRUPT_WITH_CHECK
* Description  : Check ID bit and Set Disable Interrupt
* Arguments    : Output : X = PSW Register Value before executing this function
* Return Value : None
***************************************************************************************/
#define DISABLE_INTERRUPT_WITH_CHECK(X) \
    do\
    {\
        (X) = STSR(PSW);\
        if (0UL == ((X) & 0x20UL))\
        {\
            DISABLE_INTERRUPT();\
        }\
        else\
        {\
        }\
    } while (0)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0017]:[ICUMFW_UD_U2B_D_MISC_0017]
* Function name: ENABLE_INTERRUPT_WITH_CHECK
* Description  : Check old ID bit and Set Enable Interrupt
* Arguments    : Input : X = PSW Register Value gotten
*                                       by DISABLE_INTERRUPT_WITH_CHECK Macro
* Return Value : None
***************************************************************************************/
#define ENABLE_INTERRUPT_WITH_CHECK(X) \
    do\
    {\
        if (0UL == ((X) & 0x20UL))\
        {\
            ENABLE_INTERRUPT();\
        }\
        else\
        {\
        }\
    } while (0)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0018]:[ICUMFW_UD_U2B_D_MISC_0018]
* Function name: SYNC_PIPELINE
* Description  : Synchronize Pipeline
* Arguments    : None
*                
* Return Value : None
***************************************************************************************/
static asm void SYNC_PIPELINE(void)
{
    syncp
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0019]:[ICUMFW_UD_U2B_D_MISC_0019]
* Function name: SYNC_INSTRUCTION
* Description  : Synchronize Instruction pipeline
* Arguments    : None
*                
* Return Value : None
***************************************************************************************/
static asm void SYNC_INSTRUCTION(void)
{
    synci
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MISC_0020]:[ICUMFW_UD_U2B_D_MISC_0020]
* Function name: SYNC_MEMORY
* Description  : Synchronize memory
* Arguments    : None
* Return Value : None
***************************************************************************************/
static asm void SYNC_MEMORY(void)
{
    syncm
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_MISC_0021]:[ICUMFW_UD_U2B_D_MISC_0021]
* Function Name : SET_CFPRELOAD
* Description   : Set Code Flash Preload setting
* Arguments     : Input : X = preload enable(PRELOAD_ON) / disable(PRELOAD_OFF)
* Return Value  : none
*********************************************************************************/
#define SET_CFPRELOAD(X) \
    do\
    {\
        ICUM_FCCTLICUM_FLPLCTL = (X);\
        SYNC_MEMORY();\
    } while (0)

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
uint32_t D_MISC_CheckArea(uint32_t addr);

#endif /* #if !defined(ICUM_D_MISC_PUB_H) */
