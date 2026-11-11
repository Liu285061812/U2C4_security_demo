/******************************************************************************/
/* Component Name   Testing the ICUM                                          */
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
#include "renesas_types.h"

#include "r_icumif_api.h"

#include "pe1.h"
#include "shared.h"

#include "vrun.api.h"

// ********************************************************************************
// Local variables
// ********************************************************************************

#define ALIGNVAR(n) __attribute__ ((aligned(n)))

typedef struct
{
    uint8_t                   input_key[256];
    r_key_group_t             key_group;
    uint8_t                   key_id;
    uint16_t                  key_size_in_bytes;
    r_key_property_flags_t    encryption;
    r_key_property_flags_t    decryption;
    r_key_property_flags_t    cmac_generation;
    r_key_property_flags_t    cmac_verification;
    r_key_property_flags_t    write_protected:1;
    r_key_property_flags_t    no_wild_card:1;
    r_key_property_flags_t    disable_on_debug:1;
    uint8_t                   padding[8];
} type_keyregistration;

static const type_keyregistration ALIGNVAR(32) update_key[] =
{
    { { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c },                                                                                                 KEY_GRP_AES_RAM,     0,  AES_BLOCK_SIZE_IN_BYTES, 1, 1, 1, 1, 0, 0, 0, 0x00000000, { 0 } },
    { { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c },                                                                                                 KEY_GRP_AES_RAM,     1,  AES_BLOCK_SIZE_IN_BYTES, 1, 1, 1, 1, 0, 0, 0, 0x00000000, { 0 } },
    { { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c },                                                                                                 KEY_GRP_AES,         0,  AES_BLOCK_SIZE_IN_BYTES, 1, 1, 1, 1, 0, 0, 0, 0x00000000, { 0 } },
    { { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c },                                                                                                 KEY_GRP_AES,         1,  AES_BLOCK_SIZE_IN_BYTES, 1, 1, 1, 1, 0, 0, 0, 0x00000000, { 0 } },
    { { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c },                                                                                                 KEY_GRP_AES,         2,  AES_BLOCK_SIZE_IN_BYTES, 1, 1, 1, 1, 0, 0, 0, 0x00000000, { 0 } },
    { { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c },                                                                                                 KEY_GRP_AES,         3,  AES_BLOCK_SIZE_IN_BYTES, 1, 1, 1, 1, 0, 0, 0, 0x00000000, { 0 } },
    { { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5, 0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b },                                                 KEY_GRP_AES,         5,  24,                      1, 1, 1, 1, 0, 0, 0, 0x00000000, { 0 } },
    { { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 }, KEY_GRP_AES,         5,  32,                      1, 1, 1, 1, 0, 0, 0, 0x00000000, { 0 } },
};

// ********************************************************************************
// Test functions
// ********************************************************************************
void icum_key_update_test(void)
{
    r_icumif_isd_t *p_ISD;
    int i;

    /* initialize the global icum service header */
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nKey update Start...\n\n");

    for(i = 0; i < 7; i++)
    {
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
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.write_protected    = update_key[i].write_protected;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = update_key[i].disable_on_debug;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.no_wild_card       = update_key[i].no_wild_card;

        trigger_service_and_wait(p_ISD);
        /* in case of error, stop here */
        if(p_ISD->service_result == SERV_OK)
        {
            printf("key Registration %d : OK\n", i );
        }
    }
}
