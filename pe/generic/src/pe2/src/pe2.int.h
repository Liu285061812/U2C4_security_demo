/******************************************************************************/
/* Component Name   Testing the ICU-M                                         */
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

// ---------------------------------------------------------------------------
// interrupt definition
// ---------------------------------------------------------------------------

// INTCH_PE1   : channel for interrupts coming from PE1
// INTHAND_PE1 : handler for interrupts coming from PE1

#define INTCH_FROM_PE1              2
#define INTCH_TO_PE1                1
#define INTHAND_PE1                 EVALUATOR(INTIPIR, INTCH_FROM_PE1)
#define INTHAND_ICUP                EVALUATOR(INTIPIR, INTCH_ICUP)

#define IPIR_BIT_PE1    0
#define IPIR_BIT_PE2    1

// ---------------------------------------------------------------------------
// interrupt control register (pe1 & pe2)
// ---------------------------------------------------------------------------
typedef struct
{
    unsigned char EIP:4;
    unsigned char :1;
    unsigned char :1;
    unsigned char EITB:1;
    unsigned char EIMK:1;
    unsigned char :1;
    unsigned char :1;
    unsigned char :1;
    unsigned char :1;
    unsigned char EIRF:1;
    unsigned char :1;
    unsigned char :1;
    unsigned char EICT:1;
} tEICn;

#define _EICn(channel, bit)         ((volatile tEICn *)&(EVALUATOR(EIC, channel)))->bit

// ********************************************************************************
// ASM macros
// ********************************************************************************

/*===========================================================================
  Macros used for symbol concatenation
  ===========================================================================*/

#define PASTER(x, y)        x ## y
#define EVALUATOR(x, y)     PASTER(x, y)

/*===========================================================================
  Macro to put additional latency
  Inserts "n" NOPs where it is declared
  ===========================================================================*/
asm void R_ASM_NOPS(n) /* PRQA S 3002, 3408 */ /* ADC 1, 2*/
{
%con n
.rept n
    nop
.endr
}
asm void R_ASM_HALT()
{
    #ifndef DEBUG
    halt
    #else
    nop
    #endif
}

#define NOPS(x)     R_ASM_NOPS(x)
#define HALT()      R_ASM_HALT()
