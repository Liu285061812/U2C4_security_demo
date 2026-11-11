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

/* Header Files                         */
#include "renesas_types.h"

#include "u2b_reg_adr.h"

#include "icum_common_pub.h"
#include "icum_int_declaration_pub.h"
#include "icum_d_misc_pub.h"

#include "icum_d_mpu_pub.h"
#include "icum_d_mpu.h"

/****************************************************************************************
* Global Variable                                                                       *
****************************************************************************************/
/* Callback registration at interrupt occurrence */
static icum_mpu_cb_regist_t s_CbReg_mpu;

/***************************************************************************************/
/* Internal Definition                                                                 */
/***************************************************************************************/
/* None */

/***************************************************************************************/
/* Static                                                                              */
/***************************************************************************************/
/* None */

/***************************************************************************************/
/* Prototypes (Internal)                                                               */
/***************************************************************************************/
/* None */
/***************************************************************************************
* Functions
***************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MPU_0001]:[ICUMFW_UD_U2B_D_MPU_0001]
* Function name: D_MPU_Init
* Description  : MPU Driver Initialization
* Arguments    : icum_mpu_cb_regist_t Address of Call Back Routine
*                
* Return Value : ER_OK
***************************************************************************************/
uint32_t D_MPU_Init(icum_mpu_cb_regist_t *p_regist_info)
{
    ptr_adrs_t           prot_adrs;
    uint32_t             reg_data;

    /* 1. Check the pointer to callback function */
    if (NULL != p_regist_info)
    {
        /* 1-1. Call Back Address is specified */
        /* Save Call Back Address */
        s_CbReg_mpu.p_cbf_addr = p_regist_info->p_cbf_addr;
    }
    else
    {
        /* 1-2. Call Back Address is specified NULL */
        /* Don't Call Back */
        s_CbReg_mpu.p_cbf_addr = NULL;
    }

    /* 2. Disable all MPU functions */
    reg_data = MPU_MPM_DISABLE;
    LDSR(reg_data, MPM);
    SYNC_INSTRUCTION();
    reg_data = MPU_MPRC_RESET;
    LDSR(reg_data, MPRC);

    /* 3. Set Address to each Region */
    /* 3-0. Set the address of protection area Region 0 (Code Area on RAM) */
    prot_adrs.p_adrs = MPU_REG0_STARTADRS;
    reg_data = prot_adrs.val;
    LDSR(reg_data, MPLA0);
    prot_adrs.p_adrs = MPU_REG0_ENDADRS;
    reg_data = prot_adrs.val - MPU_MPUAREG_UPLMT_CORRECTION;
    LDSR(reg_data, MPUA0);

    /* 3-1. Set the address of protection area Region 1(Startup Code Area on ROM) */
    prot_adrs.p_adrs = MPU_REG1_STARTADRS;
    reg_data = prot_adrs.val;
    LDSR(reg_data, MPLA1);
    prot_adrs.p_adrs = MPU_REG1_ENDADRS;
    reg_data = prot_adrs.val - MPU_MPUAREG_UPLMT_CORRECTION;
    LDSR(reg_data, MPUA1);

    /* 3-2. Set the address of protection area Region 2 */
    prot_adrs.p_adrs = MPU_REG2_STARTADRS;
    reg_data = prot_adrs.val;
    LDSR(reg_data, MPLA2);
    prot_adrs.p_adrs = MPU_REG2_ENDADRS;
    reg_data = prot_adrs.val - MPU_MPUAREG_UPLMT_CORRECTION;
    LDSR(reg_data, MPUA2);

    /* 3-3. Set the address of protection area Region 3 */
    prot_adrs.p_adrs = MPU_REG3_STARTADRS;
    reg_data = prot_adrs.val;
    LDSR(reg_data, MPLA3);
    prot_adrs.p_adrs = MPU_REG3_ENDADRS;
    reg_data = prot_adrs.val - MPU_MPUAREG_UPLMT_CORRECTION;
    LDSR(reg_data, MPUA3);

    /* 4. Set the Protection Area Attribute */
    /* Region 0, 1 : E=Ena, G=1, SW=Dis, UW=Dis */
    reg_data = MPU_REG0_PROTATR;
    LDSR(reg_data, MPAT0);
    reg_data = MPU_REG1_PROTATR;
    LDSR(reg_data, MPAT1);
    
    /* Region 2, 3 : E=Ena, G=1, SX=Dis, UX=Dis */
    reg_data = MPU_REG2_PROTATR;
    LDSR(reg_data, MPAT2);
    reg_data = MPU_REG3_PROTATR;
    LDSR(reg_data, MPAT3);

    /* 5. Set Region Control */
    /* Enable Region 0, 1, 2, 3 */
    reg_data = MPU_MPRC_ENABLEREG;
    LDSR(reg_data, MPRC);

    /* 6. Enable MPU Function */
    /* DX=Enable, DW=Enable, DR=Enable, */
    /* SVP=Restrict access according to the SX,SW,and SR of MPAT Register, */
    /* MPE=Enable*/
    reg_data = MPU_MPM_ENABLE;
    LDSR(reg_data, MPM);
    SYNC_INSTRUCTION();

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MPU_0005]:[ICUMFW_UD_U2B_D_MPU_0005]
* Function name: D_MPU_RAM_SetGuard
* Description  : Set guard to CRAM
* Arguments    : None
*
* Return Value : None
***************************************************************************************/
uint32_t D_MPU_SetCramGuard(void)
{
#if defined(__RH850_U2C4__)
    uint32_t index;
    uint32_t guard_size = MPU_CRAM_ICUM_SIZE;
    uint32_t guard_start = (uint32_t)MPU_CRAM_ICUM_START_ADDR;
    volatile uint32_t *write_address;
    volatile uint32_t *mask_register;
    volatile uint32_t *lock_register;
    uint32_t mask;
    const uint8_t masters[] = {0U, 1U, 10U, 11U, 22U, 23U, 29U, 30U, 32U, 34U,
                               35U, 36U, 37U, 38U, 39U, 40U, 42U, 43U};

    if ((guard_size < 256UL) || ((guard_size & (guard_size - 1UL)) != 0UL) ||
        ((guard_start & (guard_size - 1UL)) != 0UL) ||
        (guard_start < 0xFE000000UL) || (guard_start + guard_size > 0xFE070000UL))
    {
        return ER_PARAM;
    }
    write_address = (volatile uint32_t *)guard_start;
    for (index = 0UL; index < guard_size / sizeof(uint32_t); index++)
    {
        write_address[index] = MPU_CRAM_INIT_VALUE;
    }

    UNLOCK_REG_PROT(GUARD_CRAMCRG00.CRGKCPROT.UINT32);
    REG_CSGPROT0 = 0UL;
    REG_CSGSPID0 = 0x84000000UL;
    REG_CSGBAD0 = guard_start & 0x0007FF00UL;
    REG_CSGADV0 = (~(guard_size - 1UL)) & 0x0007FF00UL;
    REG_CSGPROT0 = 0x80000100UL;
    LOCK_REG_PROT(GUARD_CRAMCRG00.CRGKCPROT.UINT32);

    UNLOCK_REG_PROT(SPIDCTL.SPIDKCPROT.UINT32);
    for (index = 0UL; index < sizeof(masters) / sizeof(masters[0]); index++)
    {
        mask = ~0x84000000UL;
        if ((masters[index] >= 35U) && (masters[index] <= 37U))
        {
            mask = 0x04000000UL;
        }
        else if (masters[index] == 43U)
        {
            mask = 0x80000000UL;
        }
        mask_register = (volatile uint32_t *)(REG_SPIDCTL_BASE + 0x100UL + 4UL * masters[index]);
        lock_register = (volatile uint32_t *)(REG_SPIDCTL_BASE + 0x200UL + 4UL * masters[index]);
        *mask_register = mask;
        *lock_register = MPU_VALUE_SPIDMSKLOCK;
    }
    LOCK_REG_PROT(SPIDCTL.SPIDKCPROT.UINT32);
    return ER_OK;
#else
    uint32_t    i;
    uint32_t    *p_write;

    /* Initialize CRAM for ICU */
    p_write = (uint32_t*)MPU_CRAM_ICUM_START_ADDR;
    for (i = 0; i < (MPU_CRAM_ICUM_SIZE/(sizeof(uint32_t))); i++)
    {
        *p_write = MPU_CRAM_INIT_VALUE;
        p_write++;
    }

    /* Unlock key protection of the Cluster RAM guard register */
    UNLOCK_REG_PROT(GUARD_CRAMCRG0.CRGKCPROT.UINT32);

    /* Unlock */
    REG_CSGPROT0 &= (0x7FFFFFFFUL);

    /* Set SPID #26,#31 */
    REG_CSGSPID0 = (0x84000000UL);

    /* Set guard address offset */
    REG_CSGBAD0 = (0x00056000UL);

    /* Set channel valid bit setting register */
    REG_CSGADV0 = (0x001FE000UL);

    /* Lock */
    REG_CSGPROT0 = (0x80000100UL);

    /* Lock key protection of the Cluster RAM guard register */
    LOCK_REG_PROT(GUARD_CRAMCRG0.CRGKCPROT.UINT32);

    /* Unlock key protection of BMnSPIDMSK register */
    UNLOCK_REG_PROT(SPIDCTL.SPIDKCPROT.UINT32);

    /* Set SPID Mask Register */
    SPIDCTL.BM00SPIDMSK.UINT32 = MPU_VALUE_SPIDMSK_OTHER;
    SPIDCTL.BM01SPIDMSK.UINT32  = MPU_VALUE_SPIDMSK_OTHER;
    SPIDCTL.BM02SPIDMSK.UINT32  = MPU_VALUE_SPIDMSK_OTHER;
    //SPIDCTL.BM10SPIDMSK.UINT32  = MPU_VALUE_SPIDMSK_OTHER;
    //SPIDCTL.BM11SPIDMSK.UINT32  = MPU_VALUE_SPIDMSK_OTHER;
    //SPIDCTL.BM12SPIDMSK.UINT32  = MPU_VALUE_SPIDMSK_OTHER;
    SPIDCTL.BM23SPIDMSK.UINT32  = MPU_VALUE_SPIDMSK_OTHER;
    SPIDCTL.BM25SPIDMSK.UINT32  = MPU_VALUE_SPIDMSK_OTHER;
    SPIDCTL.BM26SPIDMSK.UINT32  = MPU_VALUE_SPIDMSK_ICUM;
    SPIDCTL.BM27SPIDMSK.UINT32  = MPU_VALUE_SPIDMSK_OTHER;
    SPIDCTL.BM28SPIDMSK.UINT32  = MPU_VALUE_SPIDMSK_OTHER;
    SPIDCTL.BM29SPIDMSK.UINT32  = MPU_VALUE_SPIDMSK_OTHER;
    SPIDCTL.BM31SPIDMSK.UINT32  = MPU_VALUE_SPIDMSK_ICUMDBGMST;

    /* Set SPID Mask Lock Register to write disable */
    SPIDCTL.BM00SPIDMSKLOCK.UINT32  = MPU_VALUE_SPIDMSKLOCK;
    SPIDCTL.BM01SPIDMSKLOCK.UINT32  = MPU_VALUE_SPIDMSKLOCK;
    SPIDCTL.BM02SPIDMSKLOCK.UINT32  = MPU_VALUE_SPIDMSKLOCK;
    //SPIDCTL.BM10SPIDMSKLOCK.UINT32  = MPU_VALUE_SPIDMSKLOCK;
    //SPIDCTL.BM11SPIDMSKLOCK.UINT32  = MPU_VALUE_SPIDMSKLOCK;
    //SPIDCTL.BM12SPIDMSKLOCK.UINT32  = MPU_VALUE_SPIDMSKLOCK;
    SPIDCTL.BM23SPIDMSKLOCK.UINT32  = MPU_VALUE_SPIDMSKLOCK;
    SPIDCTL.BM25SPIDMSKLOCK.UINT32  = MPU_VALUE_SPIDMSKLOCK;
    SPIDCTL.BM26SPIDMSKLOCK.UINT32  = MPU_VALUE_SPIDMSKLOCK;
    SPIDCTL.BM27SPIDMSKLOCK.UINT32  = MPU_VALUE_SPIDMSKLOCK;
    SPIDCTL.BM28SPIDMSKLOCK.UINT32  = MPU_VALUE_SPIDMSKLOCK;
    SPIDCTL.BM29SPIDMSKLOCK.UINT32  = MPU_VALUE_SPIDMSKLOCK;
    SPIDCTL.BM31SPIDMSKLOCK.UINT32  = MPU_VALUE_SPIDMSKLOCK;

    /* Lock key protection of BMnSPIDMSK register */
    LOCK_REG_PROT(SPIDCTL.SPIDKCPROT.UINT32);

    return (ER_OK);
#endif
}

#pragma ghs section text=".icum_handler.text"
#pragma ghs section rodata=".icum_handler.rodata"
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MPU_0002]:[ICUMFW_UD_U2B_D_MPU_0002]
* Function name: IRQ_HANDLER(MIP_MDP)
* Description  : FEINT MPU Handler
* Arguments    : None
*                
* Return Value : None
***************************************************************************************/
IRQ_HANDLER_FE(MIP_MDP)
{

    /* 1. analyze MIP/MDP exception factor */
    d_MPU_AnalyzeFactor();
}

#pragma ghs section text=default
#pragma ghs section rodata=default
/***********************************************************************************/
/* System exception handlers ** IN RAM **                                          */
/***********************************************************************************/
#pragma ghs section text=".icum_handler_ram"
#pragma ghs section rodata=".icum_handler_const_ram"

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MPU_0003]:[ICUMFW_UD_U2B_D_MPU_0003]
* Function name: IRQ_HANDLER(MIP_MDP_RAM)
* Description  : FEINT MPU Handler in RAM
* Arguments    : None
*                
* Return Value : None
***************************************************************************************/
IRQ_HANDLER_FE(MIP_MDP_RAM)
{

    /* 1. analyze MIP/MDP exception factor */
    d_MPU_AnalyzeFactor();
    
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_MPU_0004]:[ICUMFW_UD_U2B_D_MPU_0004]
* Function name: d_MPU_AnalyzeFactor
* Description  : Analyze exception factor and decide return address
* Arguments    : None
*                
* Return Value : None
***************************************************************************************/
static void d_MPU_AnalyzeFactor(void)
{
    volatile uint32_t       feic_reg;
    icum_mpu_errfactor_t    errorfact;

    /* 1. Retreive Exception Factor and generate Factor code */
    feic_reg = STSR(FEIC);
    if (MPU_MIP_EXCEPT == (feic_reg & MPU_FEIC_MASK))
    {
        /* 1-1. Set MIP Factor Code to CallBack Parameter */
        errorfact = MPU_MIP_INTERRUPT;
    }
    else
    {
        /* 1-2. Set MDP Factor Code to CallBack Parameter */
        errorfact = MPU_MDP_INTERRUPT;
    }

    /* 2. Check CallBack Registering and Call */
    if (NULL != s_CbReg_mpu.p_cbf_addr)
    {
        /* 2-1. When CallBack Routine is registered, Call CallBack Routine. */
        (*s_CbReg_mpu.p_cbf_addr)(errorfact);
    }
    else
    {
        /* Do nothing */
    }
}


#pragma ghs section text=default
#pragma ghs section rodata=default

