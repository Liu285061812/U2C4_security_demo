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
#pragma alignvar (4)
static const uint8_t test_eddsa_msg0[] = {
    0xaf, 0x82
};

#pragma alignvar(4)
static const uint8_t test_eddsa_signature_00[] = {
    0x62, 0x91, 0xd6, 0x57, 0xde, 0xec, 0x24, 0x02, 0x48, 0x27, 0xe6, 0x9c, 0x3a, 0xbe, 0x01, 0xa3,
    0x0c, 0xe5, 0x48, 0xa2, 0x84, 0x74, 0x3a, 0x44, 0x5e, 0x36, 0x80, 0xd7, 0xdb, 0x5a, 0xc3, 0xac
};

#pragma alignvar(4)
static const uint8_t test_eddsa_signature_10[] = {
    0x18, 0xff, 0x9b, 0x53, 0x8d, 0x16, 0xf2, 0x90, 0xae, 0x67, 0xf7, 0x60, 0x98, 0x4d, 0xc6, 0x59,
    0x4a, 0x7c, 0x15, 0xe9, 0x71, 0x6e, 0xd2, 0x8d, 0xc0, 0x27, 0xbe, 0xce, 0xea, 0x1e, 0xc4, 0x0a
};

static const uint8_t eddsa_msg[] = {
    0xbf, 0x82, 0x64, 0x47, 0xa6, 0x65, 0x16, 0x5a, 0x83, 0x7e, 0xd3, 0x2a, 0x13, 0xc4, 0x9e, 0x3b, 0x57, 0xa9, 0xe9, 0xbc, 0xe2, 0x63, 0xd1, 0x49, 0x2b, 0xcc, 0x41, 0x8b, 0x0e, 0xef, 0xd4, 0x09,
    0x30, 0x32, 0xb6, 0x2e, 0xcd, 0x27, 0xf1, 0xa2, 0x03, 0x1a, 0xf4, 0x54, 0x07, 0x7f, 0x78, 0x58, 0xf1, 0xe3, 0x97, 0x00, 0x50, 0xe9, 0xb4, 0x4b, 0x98, 0xb3, 0x88, 0xb2, 0x7f, 0x34, 0x87, 0xfd,
    0xf2, 0x7a, 0xdc, 0xaa, 0xe0, 0x7d, 0xc7, 0xab, 0x19, 0x13, 0xdd, 0x79, 0x83, 0xa9, 0x74, 0x40, 0x63, 0xdd, 0x01, 0xe9, 0x76, 0xcb, 0x81, 0x8c, 0xc7, 0xc3, 0xa8, 0x38, 0xb5, 0x0b, 0xc5, 0x55,
    0x88, 0xd4, 0x12, 0x40, 0xd9, 0x7b, 0x71, 0x4d, 0x2c, 0x2d, 0xab, 0x55, 0x08, 0x14, 0x72, 0x42, 0x50, 0xa5, 0xa4, 0x78, 0xad, 0x44, 0x5e, 0x15, 0x4b, 0xc8, 0x95, 0x0f, 0x8f, 0x1a, 0xaa, 0x37};


#define TEST_INIT 0 //zhxk
#define TEST_EDDSA_KEY 1

void icum_ecc_key_generate(void)
{
    int i,j;
    int m[2];
    r_icumif_isd_t *p_ISD1;
    r_icumif_isd_t *p_ISD2;

    r_icumif_isd_t  *p_ISD  = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD1 = p_ISD + 1;
    p_ISD2 = p_ISD1 + 1;

    pause_life_signal();
    
    printf("\nTesting EDDSA ECC key generation...\n\n");

    printf("This might fail if the key already exists...\n\n");

    icum_test_prng_init_rng();
    icum_test_prng_extend_seed();

#if TEST_EDDSA_KEY
    for(i = 0; i < 1; i++)
    {
        printf("Key_Grp_Ecc_Generate test(test No %d)...\n",i + 1);
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_ECC_KEY_GENERATE;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;

        p_ISD->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC;//KEY_GRP_ECC
        p_ISD->prm.ECC_KEY_GENERATE.key_id                  = i;

        switch(i)
        {
            case 7: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_NIST_P192;      printf("Using NIST_P192 curve...\n");
                    break;
            case 1: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_NIST_P224;      printf("Using NIST_P224 curve...\n");
                    break;
            case 2: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_NIST_P256;      printf("Using NIST_P256 curve...\n");
                    break;
            case 3: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_BRAINPOOL_P192; printf("Using BRAINPOOL_P192 curve...\n"); break;
            case 4: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_BRAINPOOL_P224; printf("Using BRAINPOOL_P224 curve...\n"); break;
            case 5: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_BRAINPOOL_P256; printf("Using BRAINPOOL_P256 curve...\n"); break;
            case 6: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_BRAINPOOL_P320; printf("Using BRAINPOOL_P320 curve...\n"); break;
            case 0: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_ED25519;     printf("Using Curve25519 curve...\n"); break;
        }
        p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;
        if (i == 0)
        {
            p_ISD->prm.ECC_KEY_GENERATE.flags.write_protected   = 1;
        }
        else
        {
            p_ISD->prm.ECC_KEY_GENERATE.flags.write_protected   = 0;
        }

        // trigger the security service
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        printf("result: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

        // fetch execution time in vrun_chrono_xxx variables
        vrun_chrono_gettimestamp();
        printf("EDDSA Processed in : "); PCHRONO(); NL(); NL(); FLUSH();

    }

    printf("\nTesting EDDSA sign...\n\n");

                // SERVICE_01_EDDSA_SIGN
            p_ISD1->service_id                                   = SERVICE_01_EDDSA_SIGN;
            p_ISD1->job_id                                       = 0;
            p_ISD1->p_callbackfunc                               = 0;
            p_ISD1->prm.EDDSA_SIGN.p_source                      = (void *)eddsa_msg;
            p_ISD1->prm.EDDSA_SIGN.signature[0].p_data           = BIG_BUFFER[0];
            p_ISD1->prm.EDDSA_SIGN.signature[1].p_data           = BIG_BUFFER[0] + 1024;
            p_ISD1->prm.EDDSA_SIGN.source_size_in_bytes          = sizeof(eddsa_msg);
            p_ISD1->prm.EDDSA_SIGN.hash_primitive                = HASH_PRIMITIVE_SHA2_512;
            p_ISD1->prm.EDDSA_SIGN.signature_format              = FORMAT_BINARY_PLAIN;
            p_ISD1->prm.EDDSA_SIGN.key_group                     = KEY_GRP_ECC;
            p_ISD1->prm.EDDSA_SIGN.key_id                        = 0;
            p_ISD1->prm.EDDSA_SIGN.job_slice_hash                = 0;
            p_ISD1->prm.EDDSA_SIGN.job_slice_point_mul           = 0;
            p_ISD1->prm.EDDSA_SIGN.signature_size_in_bits[0]     = 256;
            p_ISD1->prm.EDDSA_SIGN.signature_size_in_bits[1]     = 256;
            p_ISD1->prm.EDDSA_SIGN.is_precalculated_hash         = 0;

            printf("Message size:    %d bytes\n",  p_ISD1->prm.EDDSA_SIGN.source_size_in_bytes );
            printf("HASH Primitive:  %d  \n",      p_ISD1->prm.EDDSA_SIGN.hash_primitive);
            printf("Job slice sha: %d block(s)\n", p_ISD1->prm.EDDSA_SIGN.job_slice_hash);
            printf("Job slice ecc: %d block(s)\n", p_ISD1->prm.EDDSA_SIGN.job_slice_point_mul);

            // trigger the security service
            vrun_chrono_start();
            trigger_service_and_wait(p_ISD1);
            vrun_chrono_stop();

            if(p_ISD->service_result != SERV_OK) goto end;
        
            // fetch execution time in vrun_chrono_xxx variables
            vrun_chrono_gettimestamp();
    
            m[0] = p_ISD1->prm.EDDSA_SIGN.signature_size_in_bits[0];
            m[1] = p_ISD1->prm.EDDSA_SIGN.signature_size_in_bits[1];
    
            printf("Signature size (r): %d bits\n", m[0]);
            printf("Signature size (s): %d bits\n", m[1]);
            printf("Signature (r):      "); PMEM((const unsigned char *)p_ISD1->prm.EDDSA_SIGN.signature[0].p_data, p_ISD1->prm.EDDSA_SIGN.signature_size_in_bits[0] / 8); NL(); FLUSH();
            printf("Signature (s):      "); PMEM((const unsigned char *)p_ISD1->prm.EDDSA_SIGN.signature[1].p_data, p_ISD1->prm.EDDSA_SIGN.signature_size_in_bits[1] / 8); NL(); FLUSH();
            printf("Processed in :      "); PCHRONO(); NL(); FLUSH();

            if(p_ISD1->service_result != SERV_OK)
            {
                //continue;
                printf("\n EDDSA sign OK\n\n");

            }
    
    printf("\nTesting EDDSA sign verify...\n\n");
    // SERVICE_01_EDDSA_VERIFY
            p_ISD2->service_id                                   = SERVICE_01_EDDSA_VERIFY;
            p_ISD2->job_id                                       = 0;
            p_ISD2->p_callbackfunc                               = 0;
            p_ISD2->prm.EDDSA_VERIFY.p_source                    = (void *)eddsa_msg;;
            p_ISD2->prm.EDDSA_VERIFY.signature[0].p_data         = BIG_BUFFER[0];
            p_ISD2->prm.EDDSA_VERIFY.signature[1].p_data         = BIG_BUFFER[0] + 1024;
            p_ISD2->prm.EDDSA_VERIFY.source_size_in_bytes        = sizeof(eddsa_msg);
            p_ISD2->prm.EDDSA_VERIFY.signature_size_in_bits[0]   = 256;
            p_ISD2->prm.EDDSA_VERIFY.signature_size_in_bits[1]   = 256;
            p_ISD2->prm.EDDSA_VERIFY.hash_primitive              = HASH_PRIMITIVE_SHA2_512;
            p_ISD2->prm.EDDSA_VERIFY.signature_format            = FORMAT_BINARY_PLAIN;
            p_ISD2->prm.EDDSA_VERIFY.key_group                   = KEY_GRP_ECC;
            p_ISD2->prm.EDDSA_VERIFY.key_id                      = 0;
            p_ISD2->prm.EDDSA_VERIFY.job_slice_hash              = 0;
            p_ISD2->prm.EDDSA_VERIFY.job_slice_point_mul         = 0;
            p_ISD2->prm.EDDSA_VERIFY.is_precalculated_hash       = 0;

     printf("\nStart signature verification...\n\n");

            // trigger the security service
            vrun_chrono_start();
            trigger_service_and_wait(p_ISD2);
            vrun_chrono_stop();
            
            // fetch execution time in vrun_chrono_xxx variables
            vrun_chrono_gettimestamp();
            if(p_ISD2->prm.EDDSA_VERIFY.verification_result == VERIFICATION_PASS)
            {
                printf("PASS!\n\n");
            }
            else
            {
                printf( " Test is in Fail.\n" );
            }




end:
    restart_life_signal();

    printf("\n EDDSA TEST Done!\n");
#endif

#if TEST_INIT
    for(i = 0; i < 8; i++)
    {
        printf("Key_Grp_Ecc_Generate test(test No %d)...\n",i + 1);
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_ECC_KEY_GENERATE;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;

        p_ISD->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC;
        p_ISD->prm.ECC_KEY_GENERATE.key_id                  = i;

        switch(i)
        {
            case 0: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_NIST_P192;      printf("Using NIST_P192 curve...\n");
                    break;
            case 1: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_NIST_P224;      printf("Using NIST_P224 curve...\n");
                    break;
            case 2: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_NIST_P256;      printf("Using NIST_P256 curve...\n");
                    break;
            case 3: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_BRAINPOOL_P192; printf("Using BRAINPOOL_P192 curve...\n"); break;
            case 4: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_BRAINPOOL_P224; printf("Using BRAINPOOL_P224 curve...\n"); break;
            case 5: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_BRAINPOOL_P256; printf("Using BRAINPOOL_P256 curve...\n"); break;
            case 6: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_BRAINPOOL_P320; printf("Using BRAINPOOL_P320 curve...\n"); break;
            case 7: p_ISD->prm.ECC_KEY_GENERATE.curve_id    = ECC_CURVE_CURVE25519;     printf("Using Curve25519 curve...\n"); break;
        }
        p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;
        if (i == 0)
        {
            p_ISD->prm.ECC_KEY_GENERATE.flags.write_protected   = 1;
        }
        else
        {
            p_ISD->prm.ECC_KEY_GENERATE.flags.write_protected   = 0;
        }

        // trigger the security service
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        printf("result: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

        // fetch execution time in vrun_chrono_xxx variables
        vrun_chrono_gettimestamp();
        printf("Processed in : "); PCHRONO(); NL(); NL(); FLUSH();

    }
/*------------------------------------ No.9, No.10 ------------------------------------*/
    
    printf("Key_Grp_Ecc_Generate test(test No 9 err)...\n");
    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
    p_ISD->service_id     = SERVICE_03_ECC_KEY_GENERATE;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC;
    p_ISD->prm.ECC_KEY_GENERATE.key_id                  = 0; 
    p_ISD->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P192;
    
    p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;
    p_ISD->prm.ECC_KEY_GENERATE.flags.write_protected   = 0;
    printf("Using NIST_P192 curve...\n"); 

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    printf("result: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); NL(); FLUSH();


    printf("Key_Grp_Ecc_Generate test(test No 10 )...\n");
    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
    p_ISD->service_id     = SERVICE_03_ECC_KEY_GENERATE;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC;
    p_ISD->prm.ECC_KEY_GENERATE.key_id                  = 2; 
    p_ISD->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P256;
    
    p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;
    p_ISD->prm.ECC_KEY_GENERATE.flags.write_protected   = 0;
    printf("Using NIST_P256 curve...\n"); 

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    printf("result: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); NL(); FLUSH();

/*------------------------------------No.11 - No.18 ------------------------------------*/

    for(i = 0; i < 8; i++)
    {
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        printf("Key_Grp_Ecc_Ram_Generate test (test No %d)\n", i + 11);
        if(0 == i % 2)
        {
            j = 0;
        }
        else
        {
            j = 1;
        }
        p_ISD->service_id     = SERVICE_03_ECC_KEY_GENERATE;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC_RAM;
        p_ISD->prm.ECC_KEY_GENERATE.key_id                  = j;
        switch(i)
        {
            case 0: p_ISD->prm.ECC_KEY_GENERATE.curve_id = ECC_CURVE_NIST_P192;      printf("Using NIST_P192 curve...\n"); break;
            case 1: p_ISD->prm.ECC_KEY_GENERATE.curve_id = ECC_CURVE_NIST_P224;      printf("Using NIST_P224 curve...\n"); break;
            case 2: p_ISD->prm.ECC_KEY_GENERATE.curve_id = ECC_CURVE_NIST_P256;      printf("Using NIST_P256 curve...\n"); break;
            case 3: p_ISD->prm.ECC_KEY_GENERATE.curve_id = ECC_CURVE_BRAINPOOL_P192; printf("Using BRAINPOOL_P192 curve...\n"); break;
            case 4: p_ISD->prm.ECC_KEY_GENERATE.curve_id = ECC_CURVE_BRAINPOOL_P224; printf("Using BRAINPOOL_P224 curve...\n"); break;
            case 5: p_ISD->prm.ECC_KEY_GENERATE.curve_id = ECC_CURVE_BRAINPOOL_P256; printf("Using BRAINPOOL_P256 curve...\n"); break;
            case 6: p_ISD->prm.ECC_KEY_GENERATE.curve_id = ECC_CURVE_BRAINPOOL_P320; printf("Using BRAINPOOL_P320 curve...\n"); break;
            case 7: p_ISD->prm.ECC_KEY_GENERATE.curve_id = ECC_CURVE_CURVE25519;     printf("Using Curve25519 curve...\n"); break;
        }
        p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;
        
        // trigger the security service
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        printf("result: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        if(p_ISD->service_result != SERV_OK) goto end;
        
        // fetch execution time in vrun_chrono_xxx variables
        vrun_chrono_gettimestamp();
        printf("Processed in : "); PCHRONO(); NL(); NL(); FLUSH();
    }
    

/*------------------------------------ No.19, No.20 ------------------------------------*/

    printf("debug_flag on...(test No 19)\n");

    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id     = SERVICE_03_ECC_KEY_GENERATE;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC_RAM;
    p_ISD->prm.ECC_KEY_GENERATE.key_id                  = 0;
    p_ISD->prm.ECC_KEY_GENERATE.curve_id = ECC_CURVE_NIST_P192;
    p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 1;
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    printf("result: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    if(p_ISD->service_result != SERV_OK) goto end;

    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); NL(); FLUSH();
    
    
    printf("key_id Exceed the upper limit...(test No 20)\n");
    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id     = SERVICE_03_ECC_KEY_GENERATE;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC;
    p_ISD->prm.ECC_KEY_GENERATE.key_id                  = 10;
    p_ISD->prm.ECC_KEY_GENERATE.curve_id = ECC_CURVE_NIST_P192;
    p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    printf("result: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); NL(); FLUSH();

/*------------------------------------ No.21 - No.35 ------------------------------------*/

    for(i = 0; i < 15; i++)
    {
        printf("err_key_group(test No %d)...\n",i + 21);
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_ECC_KEY_GENERATE;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;

        switch(i)
        {
            case 0: p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_AES;            printf("Using AES group...\n"); break;
            case 1: p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_AES_ROM;        printf("Using AES_ROM group...\n"); break;
            case 2: p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_AES_RAM;        printf("Using AES_RAM group...\n"); break;
            case 3: p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_SHE;            printf("Using SHE group...\n"); break;
            case 4: p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_RSA;            printf("Using RSA group...\n"); break;
            case 5: p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_RSA_PUB;        printf("Using RSA_PUB group...\n"); break;
            case 6: p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_RSA_ROM_PUB;    printf("Using RSA_ROM_PUB group...\n"); break;
            case 7: p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_ECC_PUB;        printf("Using ECC_PUB group...\n"); break;
            case 8: p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_ECC_ROM_PUB;    printf("Using ECC_ROM_PUB group...\n"); break;
            case 9: p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_AES_CR;         printf("Using CR group...\n"); break;
            case 10:p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_HMAC;           printf("Using HMAC group...\n"); break;
            case 11:p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_TLS_MASTER;     printf("Using TLS_MASTER group...\n"); break;
            case 12:p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_RSA_PUB_EXT;    printf("Using RSA_PUB_EXT group...\n"); break;
            case 13:p_ISD->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_ECC_PUB_EXT;    printf("Using ECC_PUB_EXT group...\n"); break;
            case 14:p_ISD->prm.ECC_KEY_GENERATE.key_group = (r_key_group_t)16;      printf("Using ECC_PUB_EXT + 1 group...\n"); break;
        }
        p_ISD->prm.ECC_KEY_GENERATE.key_id                  = 0;
        p_ISD->prm.ECC_KEY_GENERATE.curve_id = ECC_CURVE_NIST_P192;
        p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;

        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        printf("result: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

        vrun_chrono_gettimestamp();
        printf("Processed in : "); PCHRONO(); NL(); NL(); FLUSH();
    }

/*------------------------------------ No.36 ------------------------------------*/

    printf("err_curve_id...(test No 36)\n");

    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);

    p_ISD->service_id     = SERVICE_03_ECC_KEY_GENERATE;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC_RAM;
    p_ISD->prm.ECC_KEY_GENERATE.key_id                  = 0;
    p_ISD->prm.ECC_KEY_GENERATE.curve_id                = (r_ecc_curve_t)8;
    p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;

    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    printf("result: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); NL(); FLUSH();

end:
    restart_life_signal();

    printf("\nDone!\n");

#endif
}
