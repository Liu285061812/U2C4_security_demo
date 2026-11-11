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

// ********************************************************************************
// Local constants
// ********************************************************************************

#pragma alignvar(4)
static const uint8_t test_plain_text[16] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };


typedef struct
{
    r_key_group_t           key_group;
    uint8_t                 key_id;
    uint16_t                key_size_in_bytes;
    uint8_t                 input_key[128];
    r_key_property_flags_t  encryption;
    r_key_property_flags_t  decryption;
    r_key_property_flags_t  cmac_generation;
    r_key_property_flags_t  cmac_verification;
    r_key_property_flags_t  disable_on_debug;
} type_keyregistration;

const type_keyregistration update_key[] =
{
    { KEY_GRP_AES, 10, AES_BLOCK_SIZE_IN_BYTES, { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }, 1, 1, 1, 1, 0 },
    { KEY_GRP_AES, 11, AES_BLOCK_SIZE_IN_BYTES, { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }, 1, 1, 1, 1, 0 },
    { KEY_GRP_AES, 12, AES_BLOCK_SIZE_IN_BYTES, { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }, 1, 1, 1, 1, 0 },
    { KEY_GRP_AES, 13, AES_BLOCK_SIZE_IN_BYTES, { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }, 1, 1, 1, 1, 0 },
    { KEY_GRP_AES, 14, AES_BLOCK_SIZE_IN_BYTES, { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }, 1, 1, 1, 1, 0 },
    { KEY_GRP_AES, 15, AES_BLOCK_SIZE_IN_BYTES, { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }, 1, 1, 1, 1, 0 },
    { KEY_GRP_AES, 16, AES_BLOCK_SIZE_IN_BYTES, { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }, 1, 1, 1, 1, 0 },
    { KEY_GRP_AES, 17, AES_BLOCK_SIZE_IN_BYTES, { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }, 1, 1, 1, 1, 0 },
    { KEY_GRP_AES, 18, AES_BLOCK_SIZE_IN_BYTES, { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }, 1, 1, 1, 1, 0 },
    { KEY_GRP_AES, 19, AES_BLOCK_SIZE_IN_BYTES, { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }, 1, 1, 1, 1, 0 },
    { KEY_GRP_AES, 20, AES_BLOCK_SIZE_IN_BYTES, { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }, 1, 1, 1, 1, 0 },
};

//
/*===========================================================================
  Exe key update KEY_GRP_AES
  [PUBLIC]
  ===========================================================================*/
void icum_key_buffer_aes_key_update(void)
{
    r_icumif_isd_t *p_ISD;
    int n;
    int i;


    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    printf("\nKey update Start...\n\n");


    n = 11;
    // store KEY_GRP_AES key id
    for(i = 0; i < n; i++)
    {
        // initialize the service structure
        p_ISD->service_id                             = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD->p_callbackfunc                         = 0;
        p_ISD->job_id                                 = 0;
        p_ISD->prm.KEY_UPDATE_PLAIN.key_group         = update_key[i].key_group;
        p_ISD->prm.KEY_UPDATE_PLAIN.key_id            = update_key[i].key_id;
        p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value       = update_key[i].input_key;
        p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes = update_key[i].key_size_in_bytes;

        p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = update_key[i].encryption;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = update_key[i].decryption;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = update_key[i].cmac_generation;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = update_key[i].cmac_verification;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = update_key[i].disable_on_debug;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.write_protected   = 0;

        printf("update key id = %d \n", p_ISD->prm.KEY_UPDATE_PLAIN.key_id);
        trigger_service_and_wait(p_ISD);
        // in case of error, stop here
        if(p_ISD->service_result == SERV_OK)
        {
            printf("Update Test Result : OK \n");
        }
        else
        {
            printf("Update Test Result : NG \n");
        }
    }
}

// test cmac pattern
/*===========================================================================
  Testing key buffer CMAC key id dependence
  [PUBLIC]
  ===========================================================================*/
void icum_key_buffer_aes_key_cmac_test(void)
{
    r_icumif_isd_t *p_ISD;
    int i;

    printf("\n Check cmac performance  \n");
    printf("Several key id.....   \n");
    // When buffer key is used, The processing paformance of CMAC is about 70us
    // When buffer key is not used, The processing paformance of CMAC is about 260us

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    for(i = 0; i < 10 ; i++)
    {

        // CMAC Generation
        memset(ISD_BUFFER, 0, sizeof(r_icumif_isd_t));
        p_ISD->service_id                       = SERVICE_01_AES_CMAC;
        p_ISD->p_callbackfunc                   = 0;
        p_ISD->job_id                           = 0;
        p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_AES;
        p_ISD->prm.AES_CMAC.key_id              = update_key[i].key_id;
        p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
        p_ISD->prm.AES_CMAC.p_msg               = (uint32_t*)test_plain_text;
        p_ISD->prm.AES_CMAC.msg_size_in_bits    = 16*8;
        p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t*)BIG_BUFFER[0];
        p_ISD->prm.AES_CMAC.cmac_size_in_bits  = 16*8;
        p_ISD->prm.AES_CMAC.job_slice           = 0;

        printf("\nkey id : %d \n", p_ISD->prm.AES_CMAC.key_id);
        // trigger the security service
        vrun_chrono_start();
        // trigger the security service
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        // fetch execution time in vrun_chrono_xxx variables
       vrun_chrono_gettimestamp();
        printf("\rtiming : %3dus", vrun_chrono_microsec);
        if(vrun_chrono_microsec >= 150) printf(" (!!)");
        NL();
    }

}



