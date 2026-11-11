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

#ifndef DEBUG_PE1
#define DEBUG_PE1

/*#include  "icu.h"*/
/*#include  "proc_com_pe1.h"*/

/*------------------------------------------------------------------------------*/
/*                      Enumeration type definition                             */
/*------------------------------------------------------------------------------*/
enum    ENUM_DEBUG_PHASE    {
    DEBUG_PHASE_INIT = 0,
    DEBUG_PHASE_WAIT,
    
    DEBUG_PHASE_END
};

enum {
    UPPER_NIBBLE = 0,
    LOWER_NIBBLE
};

/*#define PROTCMD0               (*(volatile unsigned int  *)0xFFF80000)*/
/*#define SWRESA                 (*(volatile unsigned int  *)0xFFF80A04)*/

#endif  /* DEBUG_PE1 */
