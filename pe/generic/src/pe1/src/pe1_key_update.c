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

#include "vrun.api.h"

#include "renesas_types.h"

// ********************************************************************************
// Local variables
// ********************************************************************************

static const uint8_t TEST_VECTOR[][16] =
{
     {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF},
};

// ********************************************************************************
// Test functions
// ********************************************************************************
void icum_key_update(void)
{
    r_icumif_isd_t *p_ISD;
    int done;
    int i;
    int j;
    int n;

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nTesting key update...\n\n");
    
    printf("\n| Key to update:\n");
    printf("|-----------------------------\n");
    i = 0;
    do
    {
        printf("| %i: %s\n", i, keys[i].key_name);
        i++;
    } while(*(uint8_t *)keys[i].key_name != 0);
    printf("  ?> ");
    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &n);

    if(!(n < i))
    {
        printf("Invalid key number - aborting\n\n");
        goto end;
    }
    printf("\nKey selected: %s\n\n", keys[n].key_name);
    FLUSH();

    // initialize the service structure
    p_ISD->service_id                             = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD->p_callbackfunc                         = 0;
    p_ISD->job_id                                 = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_group         = keys[n].key_group;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_id            = keys[n].key_id;
    p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value       = (const uint8_t *)(p_ISD) + sizeof(r_icumif_isd_t) + 4;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes = AES_BLOCK_SIZE_IN_BYTES;
    
    memset((void *)p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value, 0x55, AES_BLOCK_SIZE_IN_BYTES);

    p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;     
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;     
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;

    done = 0;
    
    do
    {
        printf("\n| Current key properties:\n");
        printf("|-----------------------------\n");
        printf("| Key: %s\n\n", keys[n].key_name);

        if(keys[n].key_group != KEY_GRP_AES_ROM)
        {
            printf("| key value              : ");
            PMEM((const uint8_t *)p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value, AES_BLOCK_SIZE_IN_BYTES);
            NL(); FLUSH();
        }
            
        printf("| flag encryption        : %i\n",   p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption       );
        printf("| flag decryption        : %i\n",   p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption       );
        printf("| flag cmac_generation   : %i\n",   p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation  );
        printf("| flag cmac_verification : %i\n",   p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification);
        printf("| flag disable_on_debug  : %i\n",   p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug );
        NL(); FLUSH();
    
        printf("\n| Select an action:\n");
        printf("|-----------------------------\n");
        if(keys[n].key_group != KEY_GRP_AES_ROM)
        {
            printf("|  0    : Set key value to 0x0011...FF\n");
            printf("|  1    : Set key value to 0xAAAA...AA\n");
        }
        printf("| 11    : Set encryption flag\n");
        printf("| 12    : Set decryption flag\n");
        printf("| 13    : Set CMAC generation flag\n");
        printf("| 14    : Set CMAC verification flag\n");
        printf("| 20    : Update key\n");
        printf("| 99    : Exit\n");
        printf("  ?> ");
        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &i);
        
        if((i >= 10) && (i < 20))
        {
            printf("\n| Select flag value (0/1):\n");
            printf("|-----------------------------\n");
            printf("  ?> ");
            vrun_console_get();
            sscanf(vrun_iobuff_readparam(0), "%i", &j);
            if((j == 0) || (j == 1))
            {
                switch(i)
                {
                    case 11: p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption        = j; break; 
                    case 12: p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption        = j; break;
                    case 13: p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation   = j; break;
                    case 14: p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification = j; break;
                    default: break;
                }
            }
        }
        else
        {        
            switch(i)
            {
                case 0:
                    memcpy((void *)p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value, TEST_VECTOR[0], AES_BLOCK_SIZE_IN_BYTES);
                    break;
                case 1:
                    memset((void *)p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value, 0xaa, AES_BLOCK_SIZE_IN_BYTES);
                    break;
                case 20:
                    printf("Updating key...\n");
                    pause_life_signal();
                    vrun_chrono_start();
                    trigger_service_and_wait(p_ISD);
                    vrun_chrono_stop();
                    vrun_chrono_gettimestamp();
                    printf("Processed in : "); PCHRONO(); NL(); FLUSH();
                    printf("Done\n");
                    restart_life_signal();
                    break;
                case 99:
                    done = 1;
                    break;
                default:
                    printf("Invalid action\n\n");
                    break;
            }
        }
    } while(done == 0);
    
end:
    printf("Done!\n");
    FLUSH();
}
