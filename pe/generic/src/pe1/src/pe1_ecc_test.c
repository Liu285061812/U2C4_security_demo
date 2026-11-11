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

/**********************************************************************************
Headers
***********************************************************************************/
#include <stdio.h>

#undef NULL
#include "r_icumif_api.h"
#include "r_icumif_pub.h"
#include "pe1.h"
#include "shared.h"
#include "vrun.api.h"

/**********************************************************************************
Test functions
***********************************************************************************/
void icum_start_ecc_test(void)
{
    r_icumif_isd_t *p_ISD;
    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD->service_id                        = SERVICE_00_ECC_TEST;
    p_ISD->p_callbackfunc                    = 0;
    p_ISD->job_id                            = 0;
    p_ISD->prm.ECC_TEST.ecc_test_operation   = ECC_TEST_START;
    p_ISD->prm.ECC_TEST.ecc_test_lock_status = LAST_ITEM_STATUS;
    /* service trigger */
    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result == SERV_OK)
    {
        printf("SUCCEEDED : Started ECC test.\n");
    }
}

void icum_end_ecc_test(void)
{
    r_icumif_isd_t *p_ISD;
    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD->service_id                        = SERVICE_00_ECC_TEST;
    p_ISD->p_callbackfunc                    = 0;
    p_ISD->job_id                            = 0;
    p_ISD->prm.ECC_TEST.ecc_test_operation   = ECC_TEST_END;
    p_ISD->prm.ECC_TEST.ecc_test_lock_status = LAST_ITEM_STATUS;
    /* service trigger */
    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result == SERV_OK)
    {
        printf("SUCCEEDED : Ended ECC test.\n");
    }
}

void icum_get_ecc_test_lock_status(void)
{
    r_icumif_isd_t *p_ISD;
    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD->service_id                        = SERVICE_00_ECC_TEST;
    p_ISD->p_callbackfunc                    = 0;
    p_ISD->job_id                            = 0;
    p_ISD->prm.ECC_TEST.ecc_test_operation   = ECC_TEST_STATUS;
    p_ISD->prm.ECC_TEST.ecc_test_lock_status = LAST_ITEM_STATUS;
    /* service trigger */
    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result == SERV_OK)
    {
        if(p_ISD->prm.ECC_TEST.ecc_test_lock_status == 0)
        {
            printf("SUCCEEDED : Lock status is ECC_TEST_UNLOCKED.\n");
        }
        else if(p_ISD->prm.ECC_TEST.ecc_test_lock_status == 1)
        {
            printf("SUCCEEDED : Lock status is ECC_TEST_LOCKED.\n");
        }
        else
        {
            printf("FAILED : Undefined status !!\n");
        }
    }
}
