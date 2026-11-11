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
// Local constants
// ********************************************************************************

#pragma alignvar(8)

static const uint8_t TESTVECTOR0[16] =
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

// self-test: CMAC based on test vectors from the CMAC standard
// source: https://tools.ietf.org/html/rfc4493#page-11

static const uint8_t KEY[16] =
    {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

// ********************************************************************************
// Test functions
// ********************************************************************************

/*===========================================================================
  ICU-M self-test function
  [PUBLIC]
  ===========================================================================*/

int icum_cancel_test(void)
{
    r_icumif_isd_t *p_ISD[6];
    uint8_t *p_cmac[2];
    int32_t errno;
   
    printf("\n");
    
    // initialize the global icum service header
    p_ISD[0]  = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD[1]  = p_ISD[0] + sizeof(r_icumif_isd_t);
    p_ISD[2]  = p_ISD[1] + sizeof(r_icumif_isd_t);
    p_ISD[3]  = p_ISD[2] + sizeof(r_icumif_isd_t);
    p_ISD[4]  = p_ISD[3] + sizeof(r_icumif_isd_t);
    p_ISD[5]  = p_ISD[4] + sizeof(r_icumif_isd_t);
    p_cmac[0] = (uint8_t *)p_ISD[5] + sizeof(r_icumif_isd_t);
    p_cmac[1] = p_cmac[0] + 16;

    pause_life_signal();

    printf("Cancel Service Test\n\n");

    memset(p_cmac[0],     0,    16);
    memset(p_cmac[1],     0,    16);

    printf("Test cancel aes cipher...\n");

    p_ISD[0]->service_id                             = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD[0]->p_callbackfunc                         = 0;
    p_ISD[0]->job_id                                 = 0;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.key_group         = KEY_GRP_AES;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.key_id            = 2;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.p_key_value       = KEY;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes = 16;

    p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.write_protected    = 0;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.no_wild_card       = 0;

    trigger_service_and_wait(p_ISD[0]);
    // in case of error, stop here
    if(p_ISD[0]->service_result == SERV_OK)
    {
        printf("key Registration : OK\n");
    }
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);

    p_ISD[0]->service_id                    = SERVICE_01_AES_CIPHER;
    p_ISD[0]->p_callbackfunc                = 0;
    p_ISD[0]->job_id                        = 0;
    p_ISD[0]->prm.AES_CIPHER.direction      = CIPHER_DIR_ENCRYPTION;
    p_ISD[0]->prm.AES_CIPHER.cipher_mode    = CIPHER_MOD_CBC;
    p_ISD[0]->prm.AES_CIPHER.key_group      = KEY_GRP_AES;
    p_ISD[0]->prm.AES_CIPHER.key_id         = 2;
    p_ISD[0]->prm.AES_CIPHER.p_iv           = (uint32_t *)TESTVECTOR0;
    p_ISD[0]->prm.AES_CIPHER.p_block_in     = (uint32_t *)BIG_BUFFER[0];
    p_ISD[0]->prm.AES_CIPHER.p_block_out    = (uint32_t *)BIG_BUFFER[1];
    p_ISD[0]->prm.AES_CIPHER.size.nb_blocks = (SIZE_OF_BIG_BUFFER / AES_BLOCK_SIZE_IN_BYTES);
    p_ISD[0]->prm.AES_CIPHER.job_cycle      = JOB_ALL_AT_ONCE;
    p_ISD[0]->prm.AES_CIPHER.job_slice = 64;
    errno = R_ICUMIF_ServiceRequest(p_ISD[0]);
    if(errno != SERV_OK)
    {
        // error
    }

    while(1)
    {
        if(p_ISD[0]->service_result == SERV_IN_PROCESS)
        {
            break;
        }
    }

    printf("Test cancel aes cmac...\n");

    p_ISD[2]->service_id                        = SERVICE_01_AES_CMAC;
    p_ISD[2]->p_callbackfunc                    = 0;
    p_ISD[2]->job_id                            = 0;
    p_ISD[2]->prm.AES_CMAC.key_group            = KEY_GRP_AES;
    p_ISD[2]->prm.AES_CMAC.key_id               = 2;
    p_ISD[2]->prm.AES_CMAC.cmac_operation       = CMAC_GENERATION;
    p_ISD[2]->prm.AES_CMAC.p_msg                = (uint32_t*)BIG_BUFFER[0];
    p_ISD[2]->prm.AES_CMAC.msg_size_in_bits     = SIZE_OF_BIG_BUFFER * 8;
    p_ISD[2]->prm.AES_CMAC.p_cmac               = (uint32_t*)p_cmac[0];
    p_ISD[2]->prm.AES_CMAC.cmac_size_in_bits    = 16 * 8;
    p_ISD[2]->prm.AES_CMAC.job_cycle      = JOB_ALL_AT_ONCE;
    p_ISD[2]->prm.AES_CMAC.job_slice = 32;
    p_ISD[2]->service_priority                  = R_ICUMIF_SERV_PRIORITY_NORMAL;
    errno = R_ICUMIF_ServiceRequest(p_ISD[2]);
    if(errno != SERV_OK)
    {
        // error
    }

    while(1)
    {
        if(p_ISD[2]->service_result == SERV_IN_PROCESS)
        {
            break;
        }
    }

    // restart life signal
    restart_life_signal();

    printf("Done!\n");

    return (0);
}
