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

// ********************************************************************************
// Headers
// ********************************************************************************

#include <stdio.h>
#include <string.h>

#undef NULL
#include "pe1.h"
#include "pe1.int.h"
#include "shared.h"

#include "vrun.api.h"

// ********************************************************************************
// Local constants
// ********************************************************************************

#pragma ghs section rodata=".testconst0"
#pragma ghs section rodata=default

// ********************************************************************************
// Link to external symbols
// ********************************************************************************

extern uint8_t __ghsbegin_start_DF_BANK_A[];

// ********************************************************************************
// Local variables
// ********************************************************************************

uint8_t fpsys_init = 0;

// ********************************************************************************
// Test functions
// ********************************************************************************

// ------------------------------------
// Flash init
// ------------------------------------
void icum_init_fcu_fw(void)
{
    /* Not available on PE1 core */
}

// ------------------------------------
// testing DF write
// ------------------------------------
#ifdef UNUSED
static void test_df_write(void)
{
    /* Not available on PE1 core */
}

// ------------------------------------
// testing CF read
// ------------------------------------
static void test_cf_read(void)
{
    /* Not available on PE1 core */
}

// ------------------------------------
// testing CF write
// ------------------------------------
static void test_cf_write(void)
{
    /* Not available on PE1 core */
}
#endif

// ------------------------------------
// test sub routine
// ------------------------------------
void sub_flash_test(void)
{
    
    /* Not available on PE1 core */
}

// ------------------------------------
// test sub routine
// ------------------------------------
void sub_flash_test_2(void)
{
    /* Not available on PE1 core */
}

// ------------------------------------
// test entry point
// ------------------------------------
void flash_test(void)
{
    /* Not available on PE1 core */
}
