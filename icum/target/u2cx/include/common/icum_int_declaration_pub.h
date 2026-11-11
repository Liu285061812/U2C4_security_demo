/******************************************************************************/
/* Component Name   ICUM INT Driver                                           */
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

#if !defined(ICUM_INT_DECLARATION_PUB_H)
#define ICUM_INT_DECLARATION_PUB_H

/**************************************************************************************/
/* Interrupt handlers (declaration)                                                   */
/**************************************************************************************/
#define PRAGMA(x)   _Pragma(#x)
#define REF(x)      PRAGMA( ghs reference x )

#define IRQ_HANDLER_NON_REENTRANT_FE(INTERRUPT_NAME) \
        REF(INTERRUPT_NAME)  \
        _Pragma("ghs interrupt(nonreentrant)") \
        _Pragma("ghs interrupt(FE)") \
        void INTERRUPT_NAME(void)

#define IRQ_HANDLER_NON_REENTRANT(INTERRUPT_NAME) \
        REF(INTERRUPT_NAME)  \
        _Pragma("ghs interrupt(nonreentrant)") \
        void INTERRUPT_NAME(void)

#define DECLARE_IRQ_HANDLER_NON_REENTRANT(INTERRUPT_NAME) \
        _Pragma("ghs interrupt(nonreentrant)") \
        void INTERRUPT_NAME(void)

#define IRQ_HANDLER(INTERRUPT_NAME)    IRQ_HANDLER_NON_REENTRANT(INTERRUPT_NAME)
#define IRQ_HANDLER_FE(INTERRUPT_NAME) IRQ_HANDLER_NON_REENTRANT_FE(INTERRUPT_NAME)

#define IRQ_HANDLER_REENTRANT(INTERRUPT_NAME) \
        void INTERRUPT_NAME(void)

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/

/**************************************************************************************/
/* interrupt module extern Declaration  for FE LEVEL                                  */
/**************************************************************************************/
DECLARE_IRQ_HANDLER_NON_REENTRANT(SYSERR);
DECLARE_IRQ_HANDLER_NON_REENTRANT(FETRAPEX);
DECLARE_IRQ_HANDLER_NON_REENTRANT(EITRAP0);
DECLARE_IRQ_HANDLER_NON_REENTRANT(EITRAP1);
DECLARE_IRQ_HANDLER_NON_REENTRANT(RIEX);
DECLARE_IRQ_HANDLER_NON_REENTRANT(MIP_MDP);
DECLARE_IRQ_HANDLER_NON_REENTRANT(PIEX);
DECLARE_IRQ_HANDLER_NON_REENTRANT(MAEX);
DECLARE_IRQ_HANDLER_NON_REENTRANT(FENMI);
DECLARE_IRQ_HANDLER_NON_REENTRANT(FEINT);
DECLARE_IRQ_HANDLER_NON_REENTRANT(EINTPRIO_0);
DECLARE_IRQ_HANDLER_NON_REENTRANT(FETRAPEX_RAM);
DECLARE_IRQ_HANDLER_NON_REENTRANT(EITRAP0_RAM);
DECLARE_IRQ_HANDLER_NON_REENTRANT(EITRAP1_RAM);
DECLARE_IRQ_HANDLER_NON_REENTRANT(RIEX_RAM);
DECLARE_IRQ_HANDLER_NON_REENTRANT(MIP_MDP_RAM);
DECLARE_IRQ_HANDLER_NON_REENTRANT(PIEX_RAM);
DECLARE_IRQ_HANDLER_NON_REENTRANT(MAEX_RAM);
DECLARE_IRQ_HANDLER_NON_REENTRANT(FENMI_RAM);
DECLARE_IRQ_HANDLER_NON_REENTRANT(FEINT_RAM);
DECLARE_IRQ_HANDLER_NON_REENTRANT(EINTPRIO_0_RAM);
DECLARE_IRQ_HANDLER_NON_REENTRANT(infinite_loop);

/**************************************************************************************/
/* interrupt module Declaration  for EI LEVEL                                         */
/**************************************************************************************/
void ICPE0(void);
void ICPE1(void);
void ICPE2(void);
void ICPE3(void);
void ICPE4(void);
void ICPE5(void);
void ICPE6(void);
void ICPE7(void);
void ICPES(void);
void ICPE0_RAM(void);
void ICPE1_RAM(void);
void ICPE2_RAM(void);
void ICPE3_RAM(void);
void ICPE4_RAM(void);
void ICPE5_RAM(void);
void ICPE6_RAM(void);
void ICPE7_RAM(void);
void ICPES_RAM(void);
void ICICUTRNGG0(void);
void ICICUTRNGHT(void);
void INTICUOSTM0(void);
void INTICUOSTM0_RAM(void);
void INTFSACSREQ0(void);
void INTFSACSREQ0_RAM(void);
void INTFSACSREQ1(void);
void INTFSACSREQ1_RAM(void);
void INTAPBGERR(void);
void INTAPBGERR_RAM(void);
void INTEXTDCU20CDRQ(void);
void INTEXTDCU20RDRQ(void);
void INTEXTDCU20CDRQPE(void);
void ICVDDHLDET(void);
void ICVDDHLDET_RAM(void);
void ICOTULI_OTABE(void);
void ICOTULI_OTABE_RAM(void);
void INTICUCLMA(void);
void INTICUCLMA_RAM(void);
void ICDCUDEGRAD(void);
void ICICUAESE0DTFEND(void);
void ICICUAESE0DTFERR(void);
void ICICUAESE0CMPERR(void);

#endif /* #if !defined(ICUM_INT_DECLARATION_PUB_H) */
