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

#include "icum_d_comm_pe_pub.h"

#include "pe1.h"
#include "pe1.int.h"
#include "shared.h"

#include "vrun.api.h"
#include "pe_reg.h"

#include "renesas_types.h"

void icum_garbage_collection_all(void)
{
    r_icumif_isd_t *p_ISD;

    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nTesting garbage collection all block...\n\n");

    p_ISD->service_id               = SERVICE_04_ICUMDF_GC;
    p_ISD->p_callbackfunc           = 0;
    p_ISD->job_id                   = 0;
    p_ISD->prm.SECURE_FLASH_MGMT.gc_operation   = GC_ALL_BLOCK;

    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();

    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); NL(); FLUSH();

    printf("\nDone!\n");
}

void icum_garbage_collection_1block(void)
{
    r_icumif_isd_t *p_ISD;

    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nTesting garbage collection all block...\n\n");

    p_ISD->service_id               = SERVICE_04_ICUMDF_GC;
    p_ISD->p_callbackfunc           = 0;
    p_ISD->job_id                   = 0;
    p_ISD->prm.SECURE_FLASH_MGMT.gc_operation   = GC_1_BLOCK;
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();

    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); NL(); FLUSH();

    printf("\nDone!\n");
}
