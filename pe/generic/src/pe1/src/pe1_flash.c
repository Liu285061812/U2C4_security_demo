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
#include "r_icumif_api.h"
#include "r_icumif_pub.h"

#include "pe1.h"
#include "pe1.int.h"
#include "shared.h"
#include "pe_reg.h"
#include "vrun.api.h"

#include "renesas_types.h"

// ********************************************************************************
// Link to external references
// ********************************************************************************
#define BIT_PE_FLMDFPLOCK_FACI0             (0x00000001UL)
#define BIT_PE_FLMDFPLOCK_MASK              (0x00000003UL)

#define TEST_FPLOCK_KEEP_LOCK               (1UL)

// ********************************************************************************
// Local constants
// ********************************************************************************
uint32_t g_fplock_next_action;
uint32_t g_fplock_pre_status;

// ********************************************************************************
// Test functions
// ********************************************************************************

/*===========================================================================
  Initializing the FCU
  [PUBLIC]
  ===========================================================================*/
#define SET_FLMDFPLOCK(value)    { \
    int i; \
    REG_PE_FLMDFPLOCK = (value & BIT_PE_FLMDFPLOCK_MASK); \
    for(i = 10000; (i > 0) && (REG_PE_FLMDFPLOCK != (value & BIT_PE_FLMDFPLOCK_MASK)); i--); \
}

void flash_callback(void)
{
    if(g_fplock_next_action == TEST_FPLOCK_KEEP_LOCK)
    {
        /* IT error case */
        /* do nothing */
    }
    else
    {
        if(0 != (REG_PE_FLMDFPLOCK & BIT_PE_FLMDFPLOCK_FACI0))
        {
            g_fplock_pre_status = REG_PE_FLMDFPLOCK;
            SET_FLMDFPLOCK(g_fplock_pre_status & ~BIT_PE_FLMDFPLOCK_FACI0);
        }
        else
        {
            SET_FLMDFPLOCK(g_fplock_pre_status);
        }
    }
    return;
}

void pe_flash_set_flash_callbackfunc(void)
{
    int32_t errno;
    R_ICUMIF_CB_REGIST_t cb_regist;

    cb_regist.p_func_addr = &flash_callback;
    errno = R_ICUMIF_SetFlashCallBackFunc(&cb_regist);
    if (errno != 0)
    {
        printf( "CallbackFunc Setting Failed. errno=%d.\n" ,errno);
    }
    g_fplock_next_action = 0UL;
}
void pe_flash_initialize(void)
{
    pe_flash_set_flash_callbackfunc();
    g_fplock_pre_status = REG_PE_FLMDFPLOCK;
}
