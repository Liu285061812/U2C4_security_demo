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
#include "shared.h"

#include "vrun.api.h"

// ********************************************************************************
// Public constants
// ********************************************************************************

// ********************************************************************************
// Local constants
// ********************************************************************************


// ********************************************************************************
// Testing RSA Key Generate
// ********************************************************************************
void icum_rsa_key_generate(void)
{
    r_icumif_isd_t *p_ISD;
    int key_len;
    int i;
    int mem_c;

    // pause life signal management
    pause_life_signal();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nTesting RSA key generation...\n\n");

    /* Normalcy system */
    for(key_len = 512, i = 0; key_len <= 1024; key_len *= 2)
    {
        printf("\nGenerating %d-bit key...\n", key_len);
        
        for(mem_c = 0; mem_c <= 2; mem_c++)
        {
            printf("\nTest No. %d", (i + 1));
            p_ISD->service_id     = SERVICE_03_RSA_KEY_GENERATE;
            p_ISD->p_callbackfunc = 0;
            p_ISD->job_id         = 0;

            p_ISD->prm.RSA_KEY_GENERATE.key_id                  = i;
            p_ISD->prm.RSA_KEY_GENERATE.modulus_size_in_bits    = key_len;

            p_ISD->prm.RSA_KEY_GENERATE.flags.disable_on_debug  = 0;

            // trigger the security service
            vrun_chrono_start();
            trigger_service_and_wait(p_ISD);
            vrun_chrono_stop();

            // fetch execution time in vrun_chrono_xxx variables
            vrun_chrono_gettimestamp();
            printf("Processed in : "); PCHRONO(); NL(); FLUSH();
            if(p_ISD->service_result == SERV_OK)
            {
                printf("This test is Pass !!\n");
            }
            else
            {
                printf("This test is False ....\n");
            }
            i++;
        }
    }

    /* Abnormality system */

    /* Override */
    printf("\nTest No. %d", (i + 1));
    printf("\nGenerating key (index : 0) (Override)...\n");

    p_ISD->service_id     = SERVICE_03_RSA_KEY_GENERATE;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.RSA_KEY_GENERATE.key_id                  = 0;
    p_ISD->prm.RSA_KEY_GENERATE.modulus_size_in_bits    = 512;
    
    p_ISD->prm.RSA_KEY_GENERATE.flags.disable_on_debug  = 0;
    
    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("result: %s\n", R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();

    if(p_ISD->service_result == SERV_INVALID_KEY_INDEX)
    {
        printf("This test is Pass !!\n");
    }
    else
    {
        printf("This test is False ....\n");
    }

    /* Debugger connection */
    printf("\nTest No. %d", (i + 2));
    printf("\nGenerating key index :%d (Debugger connection)...\n", i);

    p_ISD->service_id     = SERVICE_03_RSA_KEY_GENERATE;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.RSA_KEY_GENERATE.key_id                  = i;
    p_ISD->prm.RSA_KEY_GENERATE.modulus_size_in_bits    = 512;

    p_ISD->prm.RSA_KEY_GENERATE.flags.disable_on_debug  = 1;

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();

    printf("result: %s\n", R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();

    if(p_ISD->service_result == SERV_OK)
    {
        printf("This test is Pass !!\n");
    }
    else
    {
        printf("This test is False ....\n");
    }

    /* Other than a multiple of 8 modulus_size_in_bits */
    i++;
    printf("\nTest No. %d", (i + 2));
    printf("\nGenerating %d-bit key index :%d (Other than a multiple of 8 modulus_size_in_bits)...\n", 1001, i);

    p_ISD->service_id     = SERVICE_03_RSA_KEY_GENERATE;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.RSA_KEY_GENERATE.key_id                  = i;
    p_ISD->prm.RSA_KEY_GENERATE.modulus_size_in_bits    = 1001;

    p_ISD->prm.RSA_KEY_GENERATE.flags.disable_on_debug  = 0;

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();

    printf("result: %s\n", R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();

    if(p_ISD->service_result == SERV_OK)
    {
        printf("This test is Pass !!\n");
    }
    else
    {
        printf("This test is False ....\n");
    }

    /* The Key id range outside */
    printf("\nTest No. %d", (i + 3));
    printf("\nGenerating key (The Key id range outside / index:%d)...\n", 10);

    p_ISD->service_id     = SERVICE_03_RSA_KEY_GENERATE;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.RSA_KEY_GENERATE.key_id                  = 10;
    p_ISD->prm.RSA_KEY_GENERATE.modulus_size_in_bits    = 512;

    p_ISD->prm.RSA_KEY_GENERATE.flags.disable_on_debug  = 0;

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();

    printf("result: %s\n", R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();

    if(p_ISD->service_result == SERV_INVALID_KEY_INDEX)
    {
        printf("This test is Pass !!\n");
    }
    else
    {
        printf("This test is False ....\n");
    }

    /* The lower limit range outside */
    i++;
    printf("\nTest No. %d", (i + 3));
    printf("\nGenerating %d-bit key index :%d (The lower limit range outside)...\n", 511, i);

    p_ISD->service_id     = SERVICE_03_RSA_KEY_GENERATE;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.RSA_KEY_GENERATE.key_id                  = i;
    p_ISD->prm.RSA_KEY_GENERATE.modulus_size_in_bits    = 511;

    p_ISD->prm.RSA_KEY_GENERATE.flags.disable_on_debug  = 0;

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();

    printf("result: %s\n", R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();

    if(p_ISD->service_result == SERV_PRM_ERROR)
    {
        printf("This test is Pass !!\n");
    }
    else
    {
        printf("This test is False ....\n");
    }

    /* The upper limit range outside */
    i++;
    printf("\nTest No. %d", (i + 3));
    printf("\nGenerating %d-bit key index :%d (The upper limit range outside)...\n", 1025, i);

    p_ISD->service_id     = SERVICE_03_RSA_KEY_GENERATE;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.RSA_KEY_GENERATE.key_id                  = i;
    p_ISD->prm.RSA_KEY_GENERATE.modulus_size_in_bits    = 1025;

    p_ISD->prm.RSA_KEY_GENERATE.flags.disable_on_debug  = 0;

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();

    printf("result: %s\n", R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();

    if(p_ISD->service_result == SERV_PRM_ERROR)
    {
        printf("This test is Pass !!\n");
    }
    else
    {
        printf("This test is False ....\n");
    }


    // restart life signal
    restart_life_signal();

    printf("\nDone!\n");
}


