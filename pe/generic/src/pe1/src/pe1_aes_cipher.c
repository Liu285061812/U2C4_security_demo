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

// ********************************************************************************
// Test functions
// ********************************************************************************

/*===========================================================================
  ICU-M test function
  [PUBLIC]
  ===========================================================================*/
void icum_test_aes_cipher(int direction)
{
    r_icumif_isd_t *p_ISD;
    
    printf("\n");
    
    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    
    if(direction == 1)
    {
        printf("Operation:    AES ENCRYPTION\n");
    }
    else
    {
        printf("Operation:    AES DECRYPTION\n");
    }
    
    printf("Key used:     ");
    printf(keys[global_key_id].key_name); NL(); FLUSH();

    p_ISD->service_id                    = SERVICE_01_AES_CIPHER;
    p_ISD->p_callbackfunc                = 0;
    p_ISD->job_id                        = 0;
    p_ISD->prm.AES_CIPHER.direction      = (direction == 1)?CIPHER_DIR_ENCRYPTION:CIPHER_DIR_DECRYPTION;
    p_ISD->prm.AES_CIPHER.cipher_mode    = (cipher_modes_t)global_cipher_mode;
    p_ISD->prm.AES_CIPHER.key_group      = keys[global_key_id].key_group;
    p_ISD->prm.AES_CIPHER.key_id         = keys[global_key_id].key_id;
    p_ISD->prm.AES_CIPHER.p_iv           = (uint32_t *)TESTVECTOR0;
    p_ISD->prm.AES_CIPHER.p_block_in     = (uint32_t *)BIG_BUFFER[(direction == 1)?0:1];
    p_ISD->prm.AES_CIPHER.p_block_out    = (uint32_t *)BIG_BUFFER[(direction == 1)?1:0];
    p_ISD->prm.AES_CIPHER.size.nb_blocks = (SIZE_OF_BIG_BUFFER / AES_BLOCK_SIZE_IN_BYTES);
    p_ISD->prm.AES_CIPHER.job_slice      = job_slice[JOB_AES];
    p_ISD->prm.AES_CIPHER.job_cycle      = JOB_ALL_AT_ONCE;
    
    if(global_cipher_mode == CIPHER_MOD_XTS)
    {
        p_ISD->prm.AES_CIPHER.key_group_2             = p_ISD->prm.AES_CIPHER.key_group;
        p_ISD->prm.AES_CIPHER.key_id_2                = p_ISD->prm.AES_CIPHER.key_id;
        p_ISD->prm.AES_CIPHER.size.data_size_in_bits  = (SIZE_OF_BIG_BUFFER * 8);
    }

    // clear result buffer to 0
    memset(p_ISD->prm.AES_CIPHER.p_block_out, 0, (p_ISD->prm.AES_CIPHER.size.nb_blocks * AES_BLOCK_SIZE_IN_BYTES));
        
    printf("Block cipher: %s\n", get_cipher_mode_name(global_cipher_mode));
    printf("IV:           "); PMEM((const unsigned char *)p_ISD->prm.AES_CIPHER.p_iv, AES_BLOCK_SIZE_IN_BYTES); NL(); FLUSH();
    printf("Job slice:    %d block(s)\n", p_ISD->prm.AES_CIPHER.job_slice);

    // pause life signal management
    pause_life_signal();

    // start timing measurement
    vrun_chrono_start();

    // trigger the security service
    trigger_service_and_wait(p_ISD);
    
    // stop timing measurement
    vrun_chrono_stop();
    
    // in case of error, stop here
    if(p_ISD->service_result != SERV_OK) goto end;

    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    
    // print throughput
    print_throughput(SIZE_OF_BIG_BUFFER);
    
end:    
    // restart life signal
    restart_life_signal();

    printf("Done!\n");
}

static const uint8_t K1[16] = {
    0xC4, 0xCC, 0xF0, 0x9B, 0xE5, 0xFB, 0xC2, 0xB6, 0x21, 0x38, 0xA2, 0x1D, 0x16, 0x90, 0xA6, 0xA2, 
};
static const uint8_t K2[16] = {
    0xA3, 0xBE, 0x54, 0x8B, 0xC3, 0xD3, 0x50, 0x09, 0xFB, 0x0A, 0x8D, 0xB4, 0xAD, 0xD9, 0x52, 0x25, 
};

void icum_test_aes_cipher_in_cycle(void)
{
    r_icumif_isd_t *p_ISD;
    int direction;
    uint8_t *p_source;
    uint8_t *p_dest;
    uint32_t n;
    uint32_t p;
    int i;
    
    printf("\nTesting AES ciphering operation in cycles\n\n");
    
    // target half of big buffer
    direction = 1;
    p_source  = BIG_BUFFER[(direction == 1)?0:1];
    p_dest    = BIG_BUFFER[(direction == 1)?1:0];
    n         = (SIZE_OF_BIG_BUFFER / 2);
    
    // copy 1st half of big buffer into 2nd half
    printf("\nCopying 1st half of big buffer into its 2nd half...\n\n");
    memcpy(p_source + n,  p_source, n);

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    
    if((cipher_modes_t)global_cipher_mode == CIPHER_MOD_XTS)
    {
        printf("\nXTS mode: using RAM keys...\n\n");
        
        // initialize the RAM key 0
        p_ISD->service_id                  = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD->p_callbackfunc              = 0;
        p_ISD->job_id                      = 0;
        
        p_ISD->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES_RAM;
        p_ISD->prm.KEY_UPDATE_PLAIN.key_id                   = 0;
        p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value              = K1;    // dummy test vector
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 0;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 0;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
    
        printf("Updating RAM key #1...\n"); FLUSH();
        trigger_service_and_wait(p_ISD);

        // initialize the RAM key 1
        p_ISD->service_id                  = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD->p_callbackfunc              = 0;
        p_ISD->job_id                      = 0;
        
        p_ISD->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES_RAM;
        p_ISD->prm.KEY_UPDATE_PLAIN.key_id                   = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value              = K2;    // dummy test vector
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 0;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 0;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
    
        printf("Updating RAM key #2...\n"); FLUSH();
        trigger_service_and_wait(p_ISD);
    }
    
    printf("\nEncrypt 1st half of big buffer in one call...\n");

    if((cipher_modes_t)global_cipher_mode != CIPHER_MOD_XTS)
    {
        printf("Key used:     ");
        printf(keys[global_key_id].key_name); NL(); FLUSH();
    }
        
    p_ISD->service_id                    = SERVICE_01_AES_CIPHER;
    p_ISD->p_callbackfunc                = 0;
    p_ISD->job_id                        = 0;
    p_ISD->prm.AES_CIPHER.direction      = (direction == 1)?CIPHER_DIR_ENCRYPTION:CIPHER_DIR_DECRYPTION;
    p_ISD->prm.AES_CIPHER.cipher_mode    = (cipher_modes_t)global_cipher_mode;

    if((cipher_modes_t)global_cipher_mode != CIPHER_MOD_XTS)
    {
        p_ISD->prm.AES_CIPHER.key_group      = keys[global_key_id].key_group;
        p_ISD->prm.AES_CIPHER.key_id         = keys[global_key_id].key_id;
    }
    else
    {
        p_ISD->prm.AES_CIPHER.key_group      = KEY_GRP_AES_RAM;
        p_ISD->prm.AES_CIPHER.key_id         = 0;
        p_ISD->prm.AES_CIPHER.key_group_2    = KEY_GRP_AES_RAM;
        p_ISD->prm.AES_CIPHER.key_id_2       = 1;
    }
    p_ISD->prm.AES_CIPHER.p_iv           = (uint32_t *)TESTVECTOR0;
    p_ISD->prm.AES_CIPHER.p_block_in     = (uint32_t *)p_source;
    p_ISD->prm.AES_CIPHER.p_block_out    = (uint32_t *)p_dest;

    if((cipher_modes_t)global_cipher_mode != CIPHER_MOD_XTS)
    {
        p_ISD->prm.AES_CIPHER.size.nb_blocks = (n / AES_BLOCK_SIZE_IN_BYTES);
    }
    else
    {
        p_ISD->prm.AES_CIPHER.size.data_size_in_bits = (n * 8);
    }
    
    p_ISD->prm.AES_CIPHER.job_slice      = job_slice[JOB_AES];
    p_ISD->prm.AES_CIPHER.job_cycle      = JOB_ALL_AT_ONCE;
    
    printf("Block cipher: %s\n", get_cipher_mode_name(global_cipher_mode));
    printf("Job slice:    %d block(s)\n", p_ISD->prm.AES_CIPHER.job_slice);

    // trigger the security service
    trigger_service_and_wait(p_ISD);

    printf("\n\nNow encrypt 2nd half of big buffer in cycles...\n");

    p_ISD->service_id                    = SERVICE_01_AES_CIPHER;
    p_ISD->p_callbackfunc                = 0;
    p_ISD->job_id                        = 0;
    p_ISD->prm.AES_CIPHER.job_cycle      = JOB_INIT;

    // trigger the security service
    trigger_service_and_wait(p_ISD);
    
    p_source += n;
    p_dest   += n;
    p         = n / 4;
    
    for(i = 0; i < 4; i++)
    {
        p_ISD->service_id                    = SERVICE_01_AES_CIPHER;
        p_ISD->p_callbackfunc                = 0;
      // do not update the job id!
      // this field is required to link the calls!
        p_ISD->prm.AES_CIPHER.p_block_in     = (uint32_t*)p_source;
        p_ISD->prm.AES_CIPHER.p_block_out    = (uint32_t*)p_dest;
        if((cipher_modes_t)global_cipher_mode != CIPHER_MOD_XTS)
        {
            p_ISD->prm.AES_CIPHER.size.nb_blocks = (p / AES_BLOCK_SIZE_IN_BYTES);
        }
        else
        {
            p_ISD->prm.AES_CIPHER.size.data_size_in_bits = (p * 8);
        }
        p_ISD->prm.AES_CIPHER.job_slice      = job_slice[JOB_AES];
        p_ISD->prm.AES_CIPHER.job_cycle      = (i < 3)?(JOB_UPDATE):(JOB_FINISH);

        // trigger the security service
        trigger_service_and_wait(p_ISD);

        p_source += p;
        p_dest   += p;
    }
    
    printf("\nCompare the two encryption results: ");

    p_dest = BIG_BUFFER[(direction == 1)?1:0];
    if(memcmp(p_dest + n,  p_dest, n) == 0)
    {
        printf(" identical!\n");
    }
    else
    {
        printf(" different! ****\a\n");
    }

    printf("Done!\n");
}
#pragma alignvar(8)

typedef struct
{
    uint8_t                 output_text[32];
    cipher_modes_t          cipher_mode;
    cipher_direction_t      direction;
    r_key_group_t           key_group;
    uint8_t                 key_id;
    uint8_t                 length_plain_block;
    r_job_slice_t           job_slice;
    r_job_cycle_t           job_cycle;
    uint32_t                iv[4];
    uint32_t                plain_text[4*4];
    uint32_t                cipher_text[4*4];
} type_test_vector;

static const type_test_vector test_vector[] =
{
    {
        {"CBC ENC"},
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_CBC,        CIPHER_DIR_ENCRYPTION,        KEY_GRP_AES_RAM,    0,                4,                        0,                JOB_ALL_AT_ONCE,
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cipher text
        { 0xacab4976, 0x46b21981, 0x9b8ee9ce, 0x7d19e912,
          0x9bcb8650, 0xee197250, 0x3a11db95, 0xb2787691,
          0xb8d6be73, 0x3b74c1e3, 0x9ee61671, 0x16952222,
          0xa1caf13f, 0x09ac1f68, 0x30ca0e12, 0xa7e18675 },
    },
    {
        {"CBC DEC"},
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_CBC,        CIPHER_DIR_DECRYPTION,        KEY_GRP_AES_RAM,     0,                4,                        0,                JOB_ALL_AT_ONCE,
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher text
        { 0xacab4976, 0x46b21981, 0x9b8ee9ce, 0x7d19e912,
          0x9bcb8650, 0xee197250, 0x3a11db95, 0xb2787691,
          0xb8d6be73, 0x3b74c1e3, 0x9ee61671, 0x16952222,
          0xa1caf13f, 0x09ac1f68, 0x30ca0e12, 0xa7e18675 },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
    },
    // ECB Enc ROMKey 0
    {
        {"ECB ENC"},
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_ECB,        CIPHER_DIR_ENCRYPTION,        KEY_GRP_AES_RAM,     0,                4,                        0,                JOB_ALL_AT_ONCE,
        // iv
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cipher text
        { 0xb47bd73a, 0x60367a0d, 0xf3ca9ea8, 0x97ef6624,
          0x85d5d3f5, 0x9d69b903, 0x5a8985e7, 0xafbafd96,
          0x7fcdb143, 0x23ce8e59, 0xe3001b88, 0x880603ed,
          0x5e780c7b, 0x3fade827, 0x71202382, 0xd45d7204 },
    },
    // ECB Dec RAMKey 2
    {
        {"ECB DEC"},
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_ECB,        CIPHER_DIR_DECRYPTION,        KEY_GRP_AES_RAM,     0,                4,                        0,                JOB_ALL_AT_ONCE,
        // iv
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // cipher text
        { 0xb47bd73a, 0x60367a0d, 0xf3ca9ea8, 0x97ef6624,
          0x85d5d3f5, 0x9d69b903, 0x5a8985e7, 0xafbafd96,
          0x7fcdb143, 0x23ce8e59, 0xe3001b88, 0x880603ed,
          0x5e780c7b, 0x3fade827, 0x71202382, 0xd45d7204 },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
    },
    // CFB Enc AESKey 19
    {
        {"CFB ENC"},
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_CFB,        CIPHER_DIR_ENCRYPTION,        KEY_GRP_AES_RAM,    0,                4,                        0,                JOB_ALL_AT_ONCE,
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cipher text
        { 0x2ed93f3b, 0x20ad2db7, 0xf8493433, 0x4afb3ce8,
          0x3745a6c8, 0x3fa9b3a0, 0xadcde3cd, 0x8be51c9f,
          0x671f7526, 0x40b1cba3, 0xf18c80b1, 0xdff4a487,
          0x35054bc0, 0x0e1c5d7c, 0x6fc6c4ea, 0xe6f2f79f },
    },
    // CFB Dec ROMKey 1
    {
        {"CFB DEC"},
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_CFB,        CIPHER_DIR_DECRYPTION,        KEY_GRP_AES_RAM,    0,                4,                        0,                JOB_ALL_AT_ONCE,
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher text
        { 0x2ed93f3b, 0x20ad2db7, 0xf8493433, 0x4afb3ce8,
          0x3745a6c8, 0x3fa9b3a0, 0xadcde3cd, 0x8be51c9f,
          0x671f7526, 0x40b1cba3, 0xf18c80b1, 0xdff4a487,
          0x35054bc0, 0x0e1c5d7c, 0x6fc6c4ea, 0xe6f2f79f },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
    },
    // OFB Enc RAMKey 0
    {
        {"OFB ENC"},
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_OFB,        CIPHER_DIR_ENCRYPTION,        KEY_GRP_AES_RAM,     0,                4,                        0,                JOB_ALL_AT_ONCE,
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cipher text
        { 0x2ed93f3b, 0x20ad2db7, 0xf8493433, 0x4afb3ce8,
          0x8d508977, 0x038f9116, 0xda523cf5, 0x25d84ec5,
          0x1e054097, 0xf6ec5f9c, 0xa8f74443, 0xcced6022,
          0x28654c30, 0x78c759f6, 0xd910a566, 0x5eaed6c1 },
    },
    // OFB Dec AESKey 0
    {
        {"OFB DEC"},
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_OFB,        CIPHER_DIR_DECRYPTION,        KEY_GRP_AES_RAM,    0,                4,                        0,                JOB_ALL_AT_ONCE,
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher text
        { 0x2ed93f3b, 0x20ad2db7, 0xf8493433, 0x4afb3ce8,
          0x8d508977, 0x038f9116, 0xda523cf5, 0x25d84ec5,
          0x1e054097, 0xf6ec5f9c, 0xa8f74443, 0xcced6022,
          0x28654c30, 0x78c759f6, 0xd910a566, 0x5eaed6c1 },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
    },
};
static const uint8_t key_data[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };

void icum_aes_cipher_test(void)
{
    r_icumif_isd_t *p_ISD;

    int n = 8;
    int i;

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nSet AES 9 Key\n"); FLUSH();
    p_ISD->service_id                             = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD->p_callbackfunc                         = 0;
    p_ISD->job_id                                 = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_group         = KEY_GRP_AES;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_id            = 2;
    p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value       = key_data;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes = 16;

    p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.write_protected    = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.no_wild_card       = 0;

    trigger_service_and_wait(p_ISD);
    // in case of error, stop here
    if(p_ISD->service_result == SERV_OK)
    {
        printf("key Registration : OK\n");
    }
    printf("\nSet AES_RAM 0 Key\n"); FLUSH();
    p_ISD->prm.KEY_UPDATE_PLAIN.key_group         = KEY_GRP_AES_RAM;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_id            = 0;

    trigger_service_and_wait(p_ISD);
    // in case of error, stop here
    if(p_ISD->service_result == SERV_OK)
    {
        printf("key Registration : OK\n");
    }

    printf("\nCipher Test Start...\n\n"); FLUSH();
    for(i = 0; i < n; i++)
    {
        p_ISD->service_id = SERVICE_01_AES_CIPHER;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id = 0;
        p_ISD->prm.AES_CIPHER.direction = test_vector[i].direction;
        p_ISD->prm.AES_CIPHER.cipher_mode = test_vector[i].cipher_mode;
        p_ISD->prm.AES_CIPHER.key_group = test_vector[i].key_group;
        p_ISD->prm.AES_CIPHER.key_id = test_vector[i].key_id;
        p_ISD->prm.AES_CIPHER.p_iv = test_vector[i].iv;
        p_ISD->prm.AES_CIPHER.p_block_in = (uint32_t *)test_vector[i].plain_text;
        p_ISD->prm.AES_CIPHER.p_block_out = (uint32_t*)((((uint32_t)BIG_BUFFER[1] + 7UL) / 8UL) * 8UL);   // 8-byte alignment
        p_ISD->prm.AES_CIPHER.size.nb_blocks = test_vector[i].length_plain_block;
        p_ISD->prm.AES_CIPHER.job_slice = job_slice[JOB_AES];
        p_ISD->prm.AES_CIPHER.job_cycle = test_vector[i].job_cycle;

        /* trigger the security service */
        vrun_chrono_start();
        /* trigger the security service */
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        /* fetch execution time in vrun_chrono_xxx variables */
        vrun_chrono_gettimestamp();
        printf("\rtiming : %3dus", vrun_chrono_microsec);
        NL();


        // in case of error, stop here
        if( p_ISD->service_result == SERV_OK 
            && ( 0 == memcmp((uint32_t*)((((uint32_t)BIG_BUFFER[1] + 7UL) / 8UL) * 8UL),test_vector[i].cipher_text,(AES_BLOCK_SIZE_IN_BYTES * test_vector[i].length_plain_block))))
        {
            printf("AES %s Test Result : OK\n",test_vector[i].output_text); FLUSH();
        }
        else
        {
            printf("AES %s Test Result : NG\n",test_vector[i].output_text); FLUSH();
        }
    }
}
