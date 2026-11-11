/******************************************************************************/
/* Component Name   Testing the ICU-M                                         */
/******************************************************************************/
/* Product      : ICU-M Firmware                                              */
/******************************************************************************/
/*******************************************************************************
* Copyright(C) 2021-2022 Renesas Electronics Corporation.                       
*                                                                               
*          RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY                     
*                                                                               
* These instructions, statements, and programs are the confidential information 
* of Renesas Electronics Corporation. They must be used and modified solely for 
* the purpose for which it was furnished by Renesas Electronics Corporation.    
* All part of them must not be reproduced nor disclosed to others in any form,  
* without the prior written permission of Renesas Electronics Corporation.      
*******************************************************************************/

// ---------------------------------------------------------------------------
// interrupt definition symbols
// ---------------------------------------------------------------------------

// INTCH_ICUP  : channel for interrupts coming from ICUP
// INTCH_PE2   : channel for interrupts coming from PE2
// INTHAND_PE2 : handler for interrupts coming from PE2
// INTHAND_ICUP : handler for interrupts coming from ICUP

#define INTCH_FROM_PE2              1
#define INTCH_TO_PE2                2
#define INTHAND_PE2                 EVALUATOR(INTIPIR, INTCH_FROM_PE2)
#define INTHAND_ICUP                INTICU2PES

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
