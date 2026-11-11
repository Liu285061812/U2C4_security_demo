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

typedef struct
{
    uint8_t  test_char[64];
    uint8_t  key[16];
    uint8_t  IV[16];
    uint8_t  aad[128];
    uint8_t  plain_text[128];
    uint8_t  cipher_text[128];
    uint8_t  auth_tag[16];
    uint32_t length_aad;
    uint32_t length_plain_text;
    uint32_t length_iv_text;
    uint8_t  length_auth_tag;
} type_test_vector;

static const type_test_vector test_vector[] =
{
/* from NIST Test Vector DVPT128.rsp */
/* [Alen = 32, Plen = 24, Nlen = 13, Tlen = 16] Count = 225 */
    {
        {"Alen = 256, Plen = 192, Nlen = 104, Tlen = 128"},
        // key
        { 0x26, 0x51, 0x1f, 0xb5, 0x1f, 0xcf, 0xa7, 0x5c, 0xb4, 0xb4, 0x4d, 0xa7, 0x5a, 0x6e, 0x5a, 0x0e },
        // Nonce
        { 0x5a, 0x8a, 0xa4, 0x85, 0xc3, 0x16, 0xe9, 0x40, 0x3a, 0xff, 0x85, 0x9f, 0xbb },
        // Associated data
        { 0xa1, 0x6a, 0x2e, 0x74, 0x1f, 0x1c, 0xd9, 0x71, 0x72, 0x85, 0xb6, 0xd8, 0x82, 0xc1, 0xfc, 0x53,
          0x65, 0x5e, 0x97, 0x73, 0x76, 0x1a, 0xd6, 0x97, 0xa7, 0xee, 0x64, 0x10, 0x18, 0x4c, 0x79, 0x82 },
        // Payload
        { 0x87, 0x39, 0xb4, 0xbe, 0xa1, 0xa0, 0x99, 0xfe, 0x54, 0x74, 0x99, 0xcb, 0xc6, 0xd1, 0xb1, 0x3d,
          0x84, 0x9b, 0x80, 0x84, 0xc9, 0xb6, 0xac, 0xc5 },
        // cipher text
        { 0x50, 0x03, 0x8b, 0x5f, 0xdd, 0x36, 0x4e, 0xe7, 0x47, 0xb7, 0x0d, 0x00, 0xbd, 0x36, 0x84, 0x0e,
          0xce, 0x4e, 0xa1, 0x99, 0x98, 0x12, 0x33, 0x75 },
        // tag
        {  0xc0, 0xa4, 0x58, 0xbf, 0xca, 0xfa, 0x3b, 0x26, 0x09, 0xaf, 0xe0, 0xf8, 0x25, 0xcb, 0xf5, 0x03 },
        // Alen
        256,
        // Plen
        192,
        // Nlen
        104,
        // Tlen
        128
    },
/* from NIST Test Vector DVPT128.rsp */
/* [Alen = 0, Plen = 24, Nlen = 7, Tlen = 4] Count = 60 */
    {
        {"Alen = 0, Plen = 192, Nlen = 56, Tlen = 32"},
        // key
        { 0x19, 0xeb, 0xfd, 0xe2, 0xd5, 0x46, 0x8b, 0xa0, 0xa3, 0x03, 0x1b, 0xde, 0x62, 0x9b, 0x11, 0xfd },
        // Nonce
        { 0x5a, 0x8a, 0xa4, 0x85, 0xc3, 0x16, 0xe9 },
        // Associated data
        { 0x00 },
        // Payload
        { 0x37, 0x96, 0xcf, 0x51, 0xb8, 0x72, 0x66, 0x52, 0xa4, 0x20, 0x47, 0x33, 0xb8, 0xfb, 0xb0, 0x47,
          0xcf, 0x00, 0xfb, 0x91, 0xa9, 0x83, 0x7e, 0x22 },
        // cipher text
        { 0xa9, 0x0e, 0x8e, 0xa4, 0x40, 0x85, 0xce, 0xd7, 0x91, 0xb2, 0xfd, 0xb7, 0xfd, 0x44, 0xb5, 0xcf,
          0x0b, 0xd7, 0xd2, 0x77, 0x18, 0x02, 0x9b, 0xb7,  },
        // tag
        { 0x03, 0xe1, 0xfa, 0x6b },
        // Alen
        0,
        // Plen
        192,
        // Nlen
        56,
        // Tlen
        32
    },
// from NIST Test Vector VPT128.rsp
/* [Alen = 32, Plen = 0, Nlen =13, Tlen = 16] Count = 0 */
    {
        {"Alen = 256, Plen = 0, Nlen = 104, Tlen = 128"},
        // key
        { 0x2e, 0xbf, 0x60, 0xf0, 0x96, 0x90, 0x13, 0xa5, 0x4a, 0x3d, 0xed, 0xb1, 0x9d, 0x20, 0xf6, 0xc8 },
        // Nonce
        { 0x1d, 0xe8, 0xc5, 0xe2, 0x1f, 0x9d, 0xb3, 0x31, 0x23, 0xff, 0x87, 0x0a, 0xdd },
        // Associated data
        { 0xe1, 0xde, 0x6c, 0x61, 0x19, 0xd7, 0xdb, 0x47, 0x11, 0x36, 0x28, 0x5d, 0x10, 0xb4, 0x7a, 0x45,
          0x02, 0x21, 0xb1, 0x69, 0x78, 0x56, 0x91, 0x90, 0xef, 0x6a, 0x22, 0xb0, 0x55, 0x29, 0x56, 0x03 },
        // Payload
        { 0x00 },
        // cipher text
        { 0x00 },
        // tag
        { 0x0e, 0xad, 0x29, 0xef, 0x20, 0x5f, 0xbb, 0x86, 0xd1, 0x1a, 0xbe, 0x5e, 0xd7, 0x04, 0xb8, 0x80 },
        // Alen
        256,
        // Plen
        0,
        // Nlen
        104,
        // Tlen
        128
    },
/* from NIST Test Vector DVPT128.rsp */
/* [Alen = 0, Plen = 0, Nlen = 7, Tlen = 4] Count = 0 */
    {
        {"Alen = 0, Plen = 0, Nlen = 56, Tlen = 32"},
        // key
        { 0x4a, 0xe7, 0x01, 0x10, 0x3c, 0x63, 0xde, 0xca, 0x5b, 0x5a, 0x39, 0x39, 0xd7, 0xd0, 0x59, 0x92 },
        // Nonce
        { 0x5a, 0x8a, 0xa4, 0x85, 0xc3, 0x16, 0xe9 },
        // Associated data
        { 0x00 },
        // Payload
        { 0x00 },
        // cipher text
        { 0x00 },
        // tag
        { 0x02, 0x20, 0x9f, 0x55 },
        // Alen
        0,
        // Plen
        0,
        // Nlen
        56,
        // Tlen
        32
    },
/* from NIST Test Vector VADT128.rsp */
/* [Alen = 19, Plen = 24, Nlen = 13, Tlen = 16] Count = 190 */
    {
        {"Alen = 152, Plen = 192, Nlen = 104, Tlen = 128"},
        // key
        { 0xf1, 0x90, 0x83, 0x28, 0xed, 0xf2, 0x99, 0x6e, 0xbf, 0xc9, 0x65, 0x54, 0x72, 0xca, 0x5a, 0xd0 },
        // Nonce
        { 0x4c, 0x69, 0x33, 0x64, 0x54, 0x69, 0x30, 0xb6, 0xc5, 0x25, 0x0e, 0x26, 0x99 },
        // Associated data
        { 0x4a, 0x36, 0x34, 0xe5, 0x02, 0x8d, 0xf9, 0x7f, 0xbe, 0x00, 0xeb, 0x01, 0x6e, 0x8e, 0xa4, 0xf1,
          0x91, 0x8f, 0xaa },
        // Payload
        { 0xee, 0xde, 0x01, 0xb0, 0x8f, 0x9a, 0x30, 0x3c, 0xdf, 0x14, 0xc9, 0x9d, 0x7a, 0x45, 0x73, 0x29,
          0x72, 0xc6, 0xef, 0xf2, 0xa1, 0xdb, 0x06, 0xeb },
        // cipher text
        { 0x90, 0xc8, 0x50, 0x79, 0x0b, 0x0b, 0x38, 0x0f, 0x5a, 0xeb, 0x24, 0x88, 0xfd, 0xf4, 0x3c, 0x9d,
          0x5e, 0xf1, 0x75, 0x98, 0x61, 0xe8, 0x6f, 0x6e },
        // tag
        { 0x52, 0x57, 0x0e, 0x76, 0x96, 0x29, 0xdc, 0xc2, 0xe5, 0x68, 0x73, 0x7b, 0xa5, 0x3a, 0x11, 0x95 },
        // Alen
        152,
        // Plen
        192,
        // Nlen
        104,
        // Tlen
        128
    },
/* from NIST Test Vector VPT128.rsp */
/* [Alen = 32, Plen = 11, Nlen = 13, Tlen = 16] Count = 110 */
    {
        {"Alen = 256, Plen = 88, Nlen = 104, Tlen = 128"},
        // key
        { 0x00, 0x6f, 0xf7, 0xd3, 0x15, 0x3c, 0xaf, 0x90, 0x6e, 0xc7, 0x92, 0x9f, 0x5a, 0xef, 0x92, 0x76 },
        // Nonce
        { 0x57, 0xdb, 0x15, 0x41, 0xa1, 0x85, 0xbd, 0x9c, 0xdc, 0x34, 0xd6, 0x20, 0x25 },
        // Associated data
        { 0x7d, 0x96, 0x81, 0xca, 0xc3, 0x8e, 0x77, 0x8f, 0xba, 0x11, 0xf4, 0x46, 0x4f, 0x69, 0xed, 0x9e,
          0xbf, 0xea, 0x31, 0xb7, 0xff, 0xca, 0xf2, 0x92, 0x5b, 0x33, 0x81, 0xc6, 0x5d, 0x97, 0x59, 0x74 },
        // Payload
        { 0x31, 0xbe, 0x1b, 0x24, 0x1c, 0xae, 0x79, 0xc5, 0x4c, 0x24, 0x46 },
        // cipher text
        { 0x9d, 0xd8, 0xa4, 0x24, 0x4f, 0xbd, 0xb3, 0x0b, 0x62, 0x45, 0x78 },
        // tag
        { 0xa6, 0x25, 0xc4, 0x32, 0x33, 0x47, 0x6b, 0xbb, 0x95, 0x9a, 0xcd, 0x9e, 0xde, 0xbe, 0x28, 0x83 },
        // Alen
        256,
        // Plen
        88,
        // Nlen
        104,
        // Tlen
        128
    },
};

// ********************************************************************************
// Test functions
// ********************************************************************************

/*===========================================================================
  ICU-M self-test function
  [PUBLIC]
  ===========================================================================*/

int icum_self_test_ccm(void)
{
    r_icumif_isd_t *p_ISD;
    int n = 6;
    int i;
    int err;
    
    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    // clear error counter
    err = 0;
    
    printf("Testing CCM...\n\n"); FLUSH();
    printf("Note: job slice = %d block(s)\n\n", job_slice[JOB_AES]);

    // testing GCM encryption
    for(i = 0; i < n; i++)
    {
        printf("Test vector #%d...\n", i);

        // initialize the RAM key 0
        p_ISD->service_id                  = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD->p_callbackfunc              = 0;
        p_ISD->job_id                      = 0;
        
        p_ISD->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES_RAM;
        p_ISD->prm.KEY_UPDATE_PLAIN.key_id                   = 0;
        p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value              = test_vector[i].key;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 0;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 0;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
    
        printf("Updating RAM key...\n"); FLUSH();
        trigger_service_and_wait(p_ISD);

        printf("Length data [%s] \n",test_vector[i].test_char); FLUSH();
        printf("Testing ciphering & tag generation: ");
        FLUSH();

        p_ISD->service_id                  = SERVICE_01_AES_AUTH_CIPHER;
        p_ISD->p_callbackfunc              = 0;
        p_ISD->job_id                      = 0;
    
        p_ISD->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES_RAM;
        p_ISD->prm.AES_AUTH_CIPHER.key_id                  = 0;
        p_ISD->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_ENCRYPTION;
        p_ISD->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
        p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[i].IV;
        p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[i].length_iv_text;
        p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[i].plain_text;
        p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = test_vector[i].length_plain_text;
        p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[i].aad;
        p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[i].length_aad;
        p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
        p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)BIG_BUFFER[1];
        p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[i].length_auth_tag;
        p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
        p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 0;
    
        trigger_service_and_wait(p_ISD);
    
        if(p_ISD->service_result == SERV_OK)
        {
            if(memcmp(test_vector[i].auth_tag , p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, (p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)) != 0)
            {
                printf(" failed\n");
                err++;
            }
            else
            {
                if(memcmp(test_vector[i].cipher_text, p_ISD->prm.AES_AUTH_CIPHER.p_data_out, (test_vector[i].length_plain_text / 8)) != 0)
                {
                    printf(" failed\n");
                    err++;
                }
                else
                {
                    printf(" pass\n");
                }
            }
        }

        printf("Testing deciphering & tag generation: ");
        FLUSH();

        p_ISD->service_id                  = SERVICE_01_AES_AUTH_CIPHER;
        p_ISD->p_callbackfunc              = 0;
        p_ISD->job_id                      = 0;
    
        p_ISD->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES_RAM;
        p_ISD->prm.AES_AUTH_CIPHER.key_id                  = 0;
        p_ISD->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_DECRYPTION;
        p_ISD->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
        p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[i].IV;
        p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[i].length_iv_text;
        p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[i].cipher_text;
        p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = test_vector[i].length_plain_text;
        p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[i].aad;
        p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[i].length_aad;
        p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
        p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t*)test_vector[i].auth_tag;
        p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[i].length_auth_tag;
        p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
        p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 0;
        trigger_service_and_wait(p_ISD);
    
        if(p_ISD->service_result == SERV_OK)
        {
            if(p_ISD->prm.AES_AUTH_CIPHER.verification_result != VERIFICATION_PASS)
            {
                printf(" failed\n");
                err++;
            }
            else
            {
                if(memcmp(test_vector[i].plain_text, p_ISD->prm.AES_AUTH_CIPHER.p_data_out, (test_vector[i].length_plain_text / 8)) != 0)
                {
                    printf(" failed\n");
                    err++;
                }
                else
                {
                    printf(" pass\n");
                }
            }
        }
        NL(); FLUSH();
    }

    printf("Note: job slice = 1 block(s)\n\n");
    printf("Length data [%s] \n",test_vector[0].test_char); FLUSH();

    // initialize the RAM key 0
    p_ISD->service_id                  = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD->p_callbackfunc              = 0;
    p_ISD->job_id                      = 0;
    
    p_ISD->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES_RAM;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_id                   = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
    p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value              = test_vector[0].key;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;

    printf("Updating RAM key...\n"); FLUSH();
    trigger_service_and_wait(p_ISD);

    printf("Testing ciphering & tag generation: ");
    FLUSH();

    p_ISD->service_id                  = SERVICE_01_AES_AUTH_CIPHER;
    p_ISD->p_callbackfunc              = 0;
    p_ISD->job_id                      = 0;

    p_ISD->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES_RAM;
    p_ISD->prm.AES_AUTH_CIPHER.key_id                  = 0;
    p_ISD->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_ENCRYPTION;
    p_ISD->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[0].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[0].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[0].plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = test_vector[0].length_plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[0].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[0].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)BIG_BUFFER[1];
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[0].length_auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
    p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 1;

    trigger_service_and_wait(p_ISD);

    if(p_ISD->service_result == SERV_OK)
    {
        if(memcmp(test_vector[0].auth_tag , p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, (p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)) != 0)
        {
            printf(" failed\n");
            err++;
        }
        else
        {
                if(memcmp(test_vector[i].cipher_text, p_ISD->prm.AES_AUTH_CIPHER.p_data_out, (test_vector[i].length_plain_text / 8)) != 0)
                {
                    printf(" failed\n");
                    err++;
                }
                else
                {
                    printf(" pass\n");
                }
        }
    }

    printf("Testing deciphering & tag generation: ");
    FLUSH();

    p_ISD->service_id                  = SERVICE_01_AES_AUTH_CIPHER;
    p_ISD->p_callbackfunc              = 0;
    p_ISD->job_id                      = 0;

    p_ISD->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES_RAM;
    p_ISD->prm.AES_AUTH_CIPHER.key_id                  = 0;
    p_ISD->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_DECRYPTION;
    p_ISD->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[0].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[0].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[0].cipher_text;
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = test_vector[0].length_plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[0].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[0].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t*)test_vector[0].auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[0].length_auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
    p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 2;

    trigger_service_and_wait(p_ISD);

    if(p_ISD->service_result == SERV_OK)
    {
        if(p_ISD->prm.AES_AUTH_CIPHER.verification_result != VERIFICATION_PASS)
        {
            printf(" failed\n");
            err++;
        }
        else
        {
                if(memcmp(test_vector[i].plain_text, p_ISD->prm.AES_AUTH_CIPHER.p_data_out, (test_vector[i].length_plain_text / 8)) != 0)
                {
                    printf(" failed\n");
                    err++;
                }
                else
                {
                    printf(" pass\n");
                }
        }
    }

    printf("Note: job cycle\n\n");
    printf("Length data [%s] \n",test_vector[0].test_char); FLUSH();
    printf("Testing ciphering & tag generation\n ");
    FLUSH();

    p_ISD->service_id                  = SERVICE_01_AES_AUTH_CIPHER;
    p_ISD->p_callbackfunc              = 0;
    p_ISD->job_id                      = 0;

    p_ISD->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES_RAM;
    p_ISD->prm.AES_AUTH_CIPHER.key_id                  = 0;
    p_ISD->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_ENCRYPTION;
    p_ISD->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[0].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[0].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[0].plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = test_vector[0].length_plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[0].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[0].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)BIG_BUFFER[1];
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[0].length_auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_INIT;
    p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 0;

    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result == SERV_OK)
    {
        printf(" JOB_INIT : pass\n");
    }
    else
    {
        printf(" JOB_INIT : failed\n");
    }

    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[0].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[0].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[0].plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = 128;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[0].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[0].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)BIG_BUFFER[1];
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[0].length_auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_UPDATE;
    p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 1;

    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result == SERV_OK)
    {
        if(memcmp(BIG_BUFFER[0] , (test_vector[0].cipher_text), 16) != 0)
        {
            printf(" JOB_UPDATE : failed\n");
            err++;
        }
        else
        {
            printf(" JOB_UPDATE : pass\n");
        }
    }

    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[0].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[0].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)&test_vector[0].plain_text[16];
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = 64;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[0].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[0].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)BIG_BUFFER[1];
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[0].length_auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_FINISH;
    p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 1;

    trigger_service_and_wait(p_ISD);

    if(p_ISD->service_result == SERV_OK)
    {
        if(memcmp(BIG_BUFFER[0] , (&test_vector[0].cipher_text[16]), 8) != 0)
        {
            printf(" JOB_FINISH : failed\n");
            err++;
        }
        else
        {
            printf(" JOB_FINISH : pass\n");
        }
       if(memcmp(test_vector[0].auth_tag , p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, (p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)) != 0)
        {
            printf(" TAG : failed\n");
            err++;
        }
        else
        {
            printf(" TAG : pass\n");
        }
    }

    printf("Testing deciphering & tag generation\n");
    FLUSH();

    p_ISD->service_id                  = SERVICE_01_AES_AUTH_CIPHER;
    p_ISD->p_callbackfunc              = 0;
    p_ISD->job_id                      = 0;

    p_ISD->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES_RAM;
    p_ISD->prm.AES_AUTH_CIPHER.key_id                  = 0;
    p_ISD->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_DECRYPTION;
    p_ISD->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[0].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[0].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[0].cipher_text;
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = test_vector[0].length_plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[0].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[0].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t*)test_vector[0].auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[0].length_auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_INIT;
    p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 0;

    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result == SERV_OK)
    {
        printf(" JOB_INIT : pass\n");
    }
    else
    {
        printf(" JOB_INIT : failed\n");
    }

    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[0].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[0].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[0].cipher_text;
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = 192;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[0].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = 0;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t*)test_vector[0].auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[0].length_auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_UPDATE;
    p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 1;

    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result == SERV_OK)
    {
        if(memcmp(BIG_BUFFER[0] , (test_vector[0].plain_text), 24) != 0)
        {
            printf(" JOB_UPDATE : failed\n");
            err++;
        }
        else
        {
            printf(" JOB_UPDATE : pass\n");
        }
    }

    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[0].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[0].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)&test_vector[0].cipher_text;
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = 0;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[0].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[0].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t*)test_vector[0].auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[0].length_auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_FINISH;
    p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 1;

    trigger_service_and_wait(p_ISD);

    if(p_ISD->service_result == SERV_OK)
    {
        if(p_ISD->prm.AES_AUTH_CIPHER.verification_result != VERIFICATION_PASS)
        {
            printf(" TAG : failed\n");
            err++;
        }
        else
        {
            printf(" TAG : pass\n");
        }
    }

    // return the error counter
    return(err);
}

