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

static const unsigned char msg_P192_0_gen[] = {
0x66, 0xe9, 0x8a, 0x16, 0x58, 0x54, 0xcd, 0x07, 0x98, 0x9b, 0x1e, 0xe0, 0xec, 0x3f, 0x8d, 0xbe,
0x0e, 0xe3, 0xc2, 0xfb, 0x00, 0x51, 0xef, 0x53, 0xa0, 0xbe, 0x03, 0x45, 0x7c, 0x4f, 0x21, 0xbc,
0xe7, 0xdc, 0x50, 0xef, 0x4d, 0xf3, 0x74, 0x86, 0xc3, 0x20, 0x7d, 0xfe, 0xe2, 0x6b, 0xde, 0x4e,
0xd6, 0x23, 0x40, 0xcb, 0xb2, 0xda, 0x78, 0x49, 0x06, 0xb1, 0xb7, 0x83, 0xb4, 0xd6, 0x01, 0xbd,
0xff, 0x4a, 0xe1, 0xa7, 0xe5, 0xe8, 0x5a, 0x85, 0xaf, 0xa3, 0x20, 0x8d, 0xc6, 0x0f, 0x09, 0x90,
0xc8, 0x23, 0xbe, 0xdd, 0xdb, 0x3d, 0xb6, 0x63, 0x42, 0x66, 0x65, 0x15, 0x2e, 0xd7, 0xb0, 0x93,
0xd6, 0xbd, 0xa5, 0x06, 0xc9, 0x3a, 0x69, 0x4b, 0x83, 0xac, 0x71, 0x55, 0x3f, 0x31, 0xf5, 0xcc,
0x0d, 0x6b, 0xa2, 0xfa, 0x24, 0x80, 0x90, 0xe8, 0x79, 0x65, 0x73, 0xc4, 0x91, 0x5d, 0x15, 0x86,
};

static const uint8_t priv_scalar_2[] = {
    0x01
};
static const uint8_t pub_point_x_2[] = {
    0x6B, 0x17, 0xD1, 0xF2, 0xE1, 0x2C, 0x42, 0x47, 0xF8, 0xBC, 0xE6, 0xE5, 0x63, 0xA4, 0x40, 0xF2,
    0x77, 0x03, 0x7D, 0x81, 0x2D, 0xEB, 0x33, 0xA0, 0xF4, 0xA1, 0x39, 0x45, 0xD8, 0x98, 0xC2, 0x96,
};
static const uint8_t pub_point_y_2[] = {
    0x4F, 0xE3, 0x42, 0xE2, 0xFE, 0x1A, 0x7F, 0x9B, 0x8E, 0xE7, 0xEB, 0x4A, 0x7C, 0x0F, 0x9E, 0x16,
    0x2B, 0xCE, 0x33, 0x57, 0x6B, 0x31, 0x5E, 0xCE, 0xCB, 0xB6, 0x40, 0x68, 0x37, 0xBF, 0x51, 0xF5,
};

#ifdef UNUSED
static const uint8_t point_x_sha1_P256[] = {0xf3, 0x03, 0x3d, 0x1e, 0x54, 0x8d, 0x24, 0x5b, 0x5e, 0x45, 0xff, 0x11, 0x47, 0xdb, 0x8c, 0xd4, 0x4d, 0xb8, 0xa1, 0xf2, 0x82, 0x3c, 0x3c, 0x16, 0x41, 0x25, 0xbe, 0x88, 0xf9, 0xa9, 0x82, 0xc2};

static const uint8_t point_y_sha1_P256[] = {0x3c, 0x07, 0x8f, 0x6c, 0xee, 0x2f, 0x50, 0xe9, 0x5e, 0x89, 0x16, 0xaa, 0x9c, 0x4e, 0x93, 0xde, 0x3f, 0xdf, 0x9b, 0x04, 0x5a, 0xba, 0xc6, 0xf7, 0x07, 0xcf, 0xcb, 0x22, 0xd0, 0x65, 0x63, 0x8e};
#endif
static const uint8_t point_x_sha1_P192[] = {0x72, 0xa8, 0x3b, 0x1e, 0xe3, 0xf8, 0x30, 0x34, 0x32, 0x4d, 0xb4, 0x37, 0x76, 0x63, 0xc9, 0x33, 0xb4, 0x79, 0x95, 0x64, 0xb2, 0x33, 0x5b, 0xea};

static const uint8_t point_y_sha1_P192[] = {0x76, 0xb0, 0xc9, 0x87, 0x4b, 0x94, 0xda, 0xff, 0x7e, 0x78, 0x88, 0x1d, 0x22, 0xe5, 0xfc, 0xd5, 0x3a, 0x3e, 0xa2, 0xaf, 0xd0, 0xd1, 0x18, 0xf4};

#ifdef UNUSED
static const uint8_t ecdsa_msg_sha1[] = {
    0x48, 0x5f, 0x37, 0x2d, 0x91, 0xb7, 0x62, 0x63, 0x5d, 0x3f, 0xdb, 0xc6, 0xd8, 0x0c, 0x52, 0x63, 0xfa, 0xfd, 0x5f, 0x59, 0x08, 0xca, 0xb5, 0x48, 0xa7, 0x8a, 0x74, 0xea, 0x6b, 0xf0, 0x76, 0x57,
    0xa1, 0x2a, 0x61, 0xc8, 0x71, 0x4d, 0xd4, 0x1d, 0x6c, 0x67, 0x0b, 0xdb, 0x70, 0x0e, 0x31, 0x5b, 0x48, 0x3f, 0x83, 0xef, 0xc1, 0x82, 0x1a, 0xb1, 0x9e, 0x56, 0x81, 0x0f, 0xf3, 0x6a, 0xa8, 0xc4,
    0x62, 0xa1, 0xa0, 0xf5, 0x6e, 0x26, 0x9e, 0x12, 0x1e, 0xf5, 0x6e, 0xfe, 0xf1, 0xbb, 0x83, 0xc6, 0x49, 0x41, 0xe5, 0xcf, 0x33, 0x89, 0x4f, 0xab, 0xb8, 0x21, 0x55, 0x7f, 0x8c, 0xfe, 0x71, 0xcd,
    0xb8, 0xe6, 0x01, 0x5d, 0xf4, 0xdf, 0x41, 0xe8, 0x5d, 0x8a, 0xe9, 0x36, 0xd9, 0xcd, 0x54, 0x55, 0x10, 0x45, 0xed, 0x40, 0x4e, 0x79, 0xa6, 0x9a, 0xbb, 0xd9, 0x09, 0x07, 0x14, 0x75, 0xc6, 0xcb};
#endif
static const uint8_t ecdsa_msg_sha1_P192_temp[] = {
    0xbf, 0x82, 0x64, 0x47, 0xa6, 0x65, 0x16, 0x5a, 0x83, 0x7e, 0xd3, 0x2a, 0x13, 0xc4, 0x9e, 0x3b, 0x57, 0xa9, 0xe9, 0xbc, 0xe2, 0x63, 0xd1, 0x49, 0x2b, 0xcc, 0x41, 0x8b, 0x0e, 0xef, 0xd4, 0x09,
    0x30, 0x32, 0xb6, 0x2e, 0xcd, 0x27, 0xf1, 0xa2, 0x03, 0x1a, 0xf4, 0x54, 0x07, 0x7f, 0x78, 0x58, 0xf1, 0xe3, 0x97, 0x00, 0x50, 0xe9, 0xb4, 0x4b, 0x98, 0xb3, 0x88, 0xb2, 0x7f, 0x34, 0x87, 0xfd,
    0xf2, 0x7a, 0xdc, 0xaa, 0xe0, 0x7d, 0xc7, 0xab, 0x19, 0x13, 0xdd, 0x79, 0x83, 0xa9, 0x74, 0x40, 0x63, 0xdd, 0x01, 0xe9, 0x76, 0xcb, 0x81, 0x8c, 0xc7, 0xc3, 0xa8, 0x38, 0xb5, 0x0b, 0xc5, 0x55,
    0x88, 0xd4, 0x12, 0x40, 0xd9, 0x7b, 0x71, 0x4d, 0x2c, 0x2d, 0xab, 0x55, 0x08, 0x14, 0x72, 0x42, 0x50, 0xa5, 0xa4, 0x78, 0xad, 0x44, 0x5e, 0x15, 0x4b, 0xc8, 0x95, 0x0f, 0x8f, 0x1a, 0xaa, 0x37};

static const unsigned char ecdsa_msg_sha1_P192[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,};

static const unsigned char sha256_26byte_msg[26] =
{0x47,0xc7,0x70,0xeb,0x45,0x49,0xb6,0xef,0xf6,0x38,0x1d,0x62,0xe9,0xbe,0xb4,0x64,0xcd,0x98,0xd3,0x41,0xcc,0x1c,0x09,0x98,0x1a,0x7a};

static const unsigned char sha2_256_26byte_rslt[32] = 
{0x67,0x33,0x80,0x9c,0x73,0xe5,0x36,0x66,0xc7,0x35,0xb3,0xbd,0x3d,0xaf,0x87,0xeb,0xc7,0x7c,0x72,0x75,0x61,0x50,0xa6,0x16,0xa1,0x94,0x10,0x8d,0x71,0x23,0x12,0x72};

#ifdef UNUSED
static const uint8_t ecdsa_hex_sig0_sha1[] =     "d4255db86a416a5a688de4e238071ef16e5f2a20e31b9490c03dee9ae6164c34";
static const uint8_t ecdsa_hex_sig1_sha1[] =     "4e0ac1e1a6725bf7c6bd207439b2d370c5f2dea1ff4decf1650ab84c7769efc0";
#endif

static r_ecc_curve_t curve_id_keygen = ECC_CURVE_NIST_P256;


void measure_ecc(void)
{
    r_icumif_isd_t *p_ISD;
    int i;
    int j;
    int m[2];
    r_key_group_t key_group;
    r_key_index_t key_id;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nTesting ECC key generation...\n\n");

    for(i = 0; i < 2; i++)
    {
        if(i == 0) {printf("Generate RAM key\n"); key_group = KEY_GRP_ECC_RAM; }
        if(i == 1) {printf("Generate NVM key\n"); key_group = KEY_GRP_ECC; }

        for(j = 0; j < 3; j ++)
        {
            if(j == 0) 
            {
                printf("Using NIST P-224 curve...\n\n");
                key_id = j;
            }
            if(j == 1) 
            {
                printf("Using NIST P-256 curve...\n\n");
                key_id = j;
            }
            if(j == 2)
            {
                printf("Using Brainpool_256 curve...\n\n");
                key_id = (key_group == KEY_GRP_ECC_RAM) ? 0 : j;
            }
    
            p_ISD->service_id     = SERVICE_03_ECC_KEY_GENERATE;
            p_ISD->p_callbackfunc = 0;
            p_ISD->job_id         = 0;
        
            p_ISD->prm.ECC_KEY_GENERATE.key_group               = key_group;
            p_ISD->prm.ECC_KEY_GENERATE.key_id                  = key_id;
            p_ISD->prm.ECC_KEY_GENERATE.curve_id                = curve_id_keygen;
            p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;
        
            // trigger the security service
            vrun_chrono_start();
            trigger_service_and_wait(p_ISD);
            vrun_chrono_stop();
            if(p_ISD->service_result != SERV_OK) goto end;
        
            // fetch execution time in vrun_chrono_xxx variables
            vrun_chrono_gettimestamp();
            printf("Processed in : "); PCHRONO(); NL(); NL(); FLUSH();

            p_ISD->service_id     = SERVICE_01_ECDSA_SIGN;
            p_ISD->p_callbackfunc = 0;
            p_ISD->job_id         = 0;
        
            p_ISD->prm.ECDSA_SIGN.key_group               = key_group;
            p_ISD->prm.ECDSA_SIGN.key_id                  = key_id;
            p_ISD->prm.ECDSA_SIGN.p_source                = (void *)msg_P192_0_gen; // just a test message!
            p_ISD->prm.ECDSA_SIGN.source_size_in_bytes    = sizeof(msg_P192_0_gen);
            p_ISD->prm.ECDSA_SIGN.hash_primitive          = HASH_PRIMITIVE_SHA2_256;
            p_ISD->prm.ECDSA_SIGN.job_slice_hash          = job_slice[JOB_H];
            p_ISD->prm.ECDSA_SIGN.job_slice_point_mul     = job_slice[JOB_ECC];
            p_ISD->prm.ECDSA_SIGN.signature[0].p_data     = BIG_BUFFER[0];
            p_ISD->prm.ECDSA_SIGN.signature[1].p_data     = BIG_BUFFER[0] + 1024;
            p_ISD->prm.ECDSA_SIGN.signature_format        = FORMAT_BINARY_PLAIN;
    
            printf("Start signature generation...\n\n");
    
            printf("Message size:  %d bytes\n",    p_ISD->prm.ECDSA_SIGN.source_size_in_bytes );
            printf("Job slice sha: %d block(s)\n", p_ISD->prm.ECDSA_SIGN.job_slice_hash);
            printf("Job slice ecc: %d block(s)\n", p_ISD->prm.ECDSA_SIGN.job_slice_point_mul);
    
            // trigger the security service
            vrun_chrono_start();
            trigger_service_and_wait(p_ISD);
            vrun_chrono_stop();
        
            if(p_ISD->service_result != SERV_OK) goto end;
        
            // fetch execution time in vrun_chrono_xxx variables
            vrun_chrono_gettimestamp();
    
            m[0] = p_ISD->prm.ECDSA_SIGN.signature_size_in_bits[0];
            m[1] = p_ISD->prm.ECDSA_SIGN.signature_size_in_bits[1];
    
            printf("Signature size (r): %d bits\n", m[0]);
            printf("Signature size (s): %d bits\n", m[1]);
            printf("Signature (r):      "); PMEM((const unsigned char *)p_ISD->prm.ECDSA_SIGN.signature[0].p_data, p_ISD->prm.ECDSA_SIGN.signature_size_in_bits[0] / 8); NL(); FLUSH();
            printf("Signature (s):      "); PMEM((const unsigned char *)p_ISD->prm.ECDSA_SIGN.signature[1].p_data, p_ISD->prm.ECDSA_SIGN.signature_size_in_bits[1] / 8); NL(); FLUSH();
            printf("Processed in :      "); PCHRONO(); NL(); FLUSH();
        
            p_ISD->service_id     = SERVICE_01_ECDSA_VERIFY;
            p_ISD->p_callbackfunc = 0;
            p_ISD->job_id         = 0;
        
            p_ISD->prm.ECDSA_VERIFY.key_group                   = key_group;
            p_ISD->prm.ECDSA_VERIFY.key_id                      = key_id;
            p_ISD->prm.ECDSA_VERIFY.p_source                    = (void *)msg_P192_0_gen;   // just a test message!
            p_ISD->prm.ECDSA_VERIFY.source_size_in_bytes        = sizeof(msg_P192_0_gen);
            p_ISD->prm.ECDSA_VERIFY.hash_primitive              = HASH_PRIMITIVE_SHA2_256;
            p_ISD->prm.ECDSA_VERIFY.job_slice_hash              = job_slice[JOB_H];
            p_ISD->prm.ECDSA_VERIFY.job_slice_point_mul         = job_slice[JOB_ECC];
            p_ISD->prm.ECDSA_VERIFY.signature[0].p_data         = BIG_BUFFER[0];
            p_ISD->prm.ECDSA_VERIFY.signature[1].p_data         = BIG_BUFFER[0] + 1024;
            p_ISD->prm.ECDSA_VERIFY.signature_size_in_bits[0]   = m[0];
            p_ISD->prm.ECDSA_VERIFY.signature_size_in_bits[1]   = m[1];
            p_ISD->prm.ECDSA_VERIFY.signature_format            = FORMAT_BINARY_PLAIN;
            p_ISD->prm.ECDSA_VERIFY.verification_result         = VERIFICATION_FAIL;
    
            printf("\nStart signature verification...\n\n");
    
            // trigger the security service
            vrun_chrono_start();
            trigger_service_and_wait(p_ISD);
            vrun_chrono_stop();
        
            // fetch execution time in vrun_chrono_xxx variables
            vrun_chrono_gettimestamp();
        
            printf("Processed in : "); PCHRONO(); NL(); FLUSH();
            printf("Verification result : ");
            if(p_ISD->prm.ECDSA_VERIFY.verification_result == VERIFICATION_PASS)
            {
                printf("PASS!\n\n");
            }
            else
            {
                printf("*FAIL*!\a\n\n");
            }
        }
    }
end:

    printf("\nDone!\n");
}

#define Init_Test  0
#define ECDSA_Test 1  //zhxk 
#define EDDSA_Test 1  //zhxk 
#define EDDSA_Test_temp 0

void ecdsa_sign_verify_test(void)
{
    r_icumif_isd_t *p_ISD0;
    r_icumif_isd_t *p_ISD1;
    r_icumif_isd_t *p_ISD2;
    unsigned int test_no = 1;
    int n = 0;
    int m[2];
    int ai_loop, ai_loop2;
    r_number_format_t sig_form [] = {
                                         FORMAT_STRING_DECIMAL,
                                         FORMAT_STRING_HEXA,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN
                                    };

    r_key_group_t key_group_set;
    int cntPass = 0 ;
    int cntFail = 0 ;
    int cntSum = 0 ;
#if ECDSA_Test //zhxk
    printf("\nTesting ECDSA sign...\n\n");

    icum_test_prng_init_rng();
    icum_test_prng_extend_seed();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 3));    // clear the ISD_BUFFER
    p_ISD0 = (r_icumif_isd_t *)BIG_BUFFER[1];
    p_ISD1 = p_ISD0 + 1;
    p_ISD2 = p_ISD1 + 1;
 
    for(ai_loop = 2; ai_loop <= 2; ai_loop++) //zhxk ECC_CURVE_BRAINPOOL_P320
    {
        p_ISD0->service_id     = SERVICE_03_ECC_KEY_GENERATE;
        p_ISD0->p_callbackfunc = 0;
        p_ISD0->job_id         = 0;

        p_ISD0->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;

        switch(ai_loop)
        {
            case 0:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P192;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve NIST P-192    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 1:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P224;
                key_group_set                                        = KEY_GRP_ECC;
                n                                                    = 1;
                printf("Testing on curve NIST P-224    Key group KEY_GRP_ECC ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 2:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P256;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve NIST P-256  ECDSA  Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 3:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_BRAINPOOL_P192;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve Brainpool P-192    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 4:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_BRAINPOOL_P224;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve Brainpool P-224    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 5:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_BRAINPOOL_P256;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve Brainpool P-256    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 6:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_BRAINPOOL_P320;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve Brainpool P-320    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            default:
                break;
        }

        p_ISD0->prm.ECC_KEY_GENERATE.key_group               = key_group_set;
        p_ISD0->prm.ECC_KEY_GENERATE.key_id                  = n;


        printf("Generate ECDSA ephemeral key (key index %d)...\n\n", n);

        // trigger the security service
        trigger_service_and_wait(p_ISD0);
        if(p_ISD0->service_result != SERV_OK) goto end;

        for(ai_loop2 = HASH_PRIMITIVE_SHA2_256; ai_loop2 <= HASH_PRIMITIVE_SHA2_256; ai_loop2++) // ZHXK HASH_PRIMITIVE_MD5--HASH_PRIMITIVE_SHA2_512
        {
            p_ISD1->service_id     = SERVICE_01_ECDSA_SIGN;
            p_ISD1->p_callbackfunc = 0;
            p_ISD1->job_id         = 0;

            p_ISD1->prm.ECDSA_SIGN.key_group               = key_group_set;
            p_ISD1->prm.ECDSA_SIGN.key_id                  = n;
            p_ISD1->prm.ECDSA_SIGN.p_source                = (void *)sha256_26byte_msg; // just a test message!
            p_ISD1->prm.ECDSA_SIGN.source_size_in_bytes    = sizeof(sha256_26byte_msg);
            p_ISD1->prm.ECDSA_SIGN.hash_primitive          = (r_hash_primitive_t)(ai_loop2);
            p_ISD1->prm.ECDSA_SIGN.job_slice_hash          = (ai_loop2 > 2) ? 0 : 1;
            p_ISD1->prm.ECDSA_SIGN.job_slice_point_mul     = ((ai_loop2 % 2) == 0) ? 0 : 1;
            p_ISD1->prm.ECDSA_SIGN.signature[0].p_data     = BIG_BUFFER[0];
            p_ISD1->prm.ECDSA_SIGN.signature[1].p_data     = BIG_BUFFER[0] + 1024;
            p_ISD1->prm.ECDSA_SIGN.signature_format        = sig_form[ai_loop2];
            p_ISD1->prm.ECDSA_SIGN.is_precalculated_hash   = 0u;

            printf("\n*------ ECDSA Test No. %02d\n", test_no);
            test_no++;

            printf("Start ECDSA signature generation zhxk\n[Count %02d] ...\n\n", ((ai_loop * 10) + (ai_loop2 + 1)));

            printf("Message size:    %d bytes\n",  p_ISD1->prm.ECDSA_SIGN.source_size_in_bytes );
            printf("HASH Primitive:  %d  \n",      p_ISD1->prm.ECDSA_SIGN.hash_primitive);
            printf("Job slice sha: %d block(s)\n", p_ISD1->prm.ECDSA_SIGN.job_slice_hash);
            printf("Job slice ecc: %d block(s)\n", p_ISD1->prm.ECDSA_SIGN.job_slice_point_mul);

            // trigger the security service
            vrun_chrono_start();
            trigger_service_and_wait(p_ISD1);
            vrun_chrono_stop();

            if(p_ISD1->service_result != SERV_OK)
            {
                continue;
            }

            // fetch execution time in vrun_chrono_xxx variables
            vrun_chrono_gettimestamp();

            m[0] = p_ISD1->prm.ECDSA_SIGN.signature_size_in_bits[0];
            m[1] = p_ISD1->prm.ECDSA_SIGN.signature_size_in_bits[1];

            printf("Signature format: %d  (0:DEC  1:HEXA  2:BIN)\n", sig_form[ai_loop2]);
            printf("Signature size (r): %d bits\n", m[0]);
            printf("Signature size (s): %d bits\n", m[1]);
            switch (sig_form[ai_loop2])
            {
                case FORMAT_BINARY_PLAIN:
                    printf("BINARY_PLAIN Signature (r):      "); PMEM((const unsigned char *)p_ISD1->prm.ECDSA_SIGN.signature[0].p_data, p_ISD1->prm.ECDSA_SIGN.signature_size_in_bits[0] / 8); NL(); FLUSH();
                    printf(" BINARY_PLAINSignature (s):      "); PMEM((const unsigned char *)p_ISD1->prm.ECDSA_SIGN.signature[1].p_data, p_ISD1->prm.ECDSA_SIGN.signature_size_in_bits[1] / 8); NL(); FLUSH();
                    break;
                case FORMAT_STRING_DECIMAL:
                case FORMAT_STRING_HEXA:
                    printf("STRING Signature (r):      %s", p_ISD1->prm.ECDSA_SIGN.signature[0].p_data); NL(); FLUSH();
                    printf("STRING Signature (s):      %s", p_ISD1->prm.ECDSA_SIGN.signature[1].p_data); NL(); FLUSH();
                    break;
                default:
                    break;
            }
            printf("Processed in :      "); PCHRONO(); NL(); FLUSH();
            
            p_ISD2->service_id     = SERVICE_01_ECDSA_VERIFY;
            p_ISD2->p_callbackfunc = 0;
            p_ISD2->job_id         = 0;
            
            p_ISD2->prm.ECDSA_VERIFY.key_group                   = key_group_set;
            p_ISD2->prm.ECDSA_VERIFY.key_id                      = n;
            p_ISD2->prm.ECDSA_VERIFY.p_source                    = (void *)sha256_26byte_msg;   // just a test message!
            p_ISD2->prm.ECDSA_VERIFY.source_size_in_bytes        = sizeof(sha256_26byte_msg);
            p_ISD2->prm.ECDSA_VERIFY.hash_primitive              = (r_hash_primitive_t)(ai_loop2);
            p_ISD2->prm.ECDSA_VERIFY.job_slice_hash              = (ai_loop2 < 2) ? 0 : 1;
            p_ISD2->prm.ECDSA_VERIFY.job_slice_point_mul         = ((ai_loop2 % 2) == 0) ? 0 : 1;
            p_ISD2->prm.ECDSA_VERIFY.signature[0].p_data         = BIG_BUFFER[0];
            p_ISD2->prm.ECDSA_VERIFY.signature[1].p_data         = BIG_BUFFER[0] + 1024;
            p_ISD2->prm.ECDSA_VERIFY.signature_size_in_bits[0]   = m[0];
            p_ISD2->prm.ECDSA_VERIFY.signature_size_in_bits[1]   = m[1];
            p_ISD2->prm.ECDSA_VERIFY.signature_format            = sig_form[ai_loop2];
            p_ISD2->prm.ECDSA_VERIFY.is_precalculated_hash       = 0u;
            p_ISD2->prm.ECDSA_VERIFY.verification_result         = VERIFICATION_FAIL;

            printf("\nECDSA Start signature verification... zhxk \n\n");

            // trigger the security service
            vrun_chrono_start();
            trigger_service_and_wait(p_ISD2);
            vrun_chrono_stop();
            
            // fetch execution time in vrun_chrono_xxx variables
            vrun_chrono_gettimestamp();

            printf("Processed in : "); PCHRONO(); NL(); FLUSH();
            printf("ECDSA Verification result : ");
            cntSum ++ ;
            if(p_ISD2->prm.ECDSA_VERIFY.verification_result == VERIFICATION_PASS)
            {
                printf("PASS!\n\n");
                cntPass ++ ;
            }
            else
            {
                printf("*FAIL*!\a\n\n");
                cntFail ++ ;
                printf( " Test is in Fail.\n" );
            }
        }
    }
    
end:
    printf("\nECDSA_SIGN ...");
    printf( " Test Result : All %d, Pass = %d, Fail = %d\n", cntSum, cntPass, cntFail );

    printf("\nDone!\n");
#endif
}

void eddsa_sign_verify_test(void)
{
    r_icumif_isd_t *p_ISD0;
    r_icumif_isd_t *p_ISD1;
    r_icumif_isd_t *p_ISD2;
    unsigned int test_no = 1;
    int n = 0;
    int m[2];
    int ai_loop, ai_loop2;
    r_number_format_t sig_form [] = {
                                         FORMAT_STRING_DECIMAL,
                                         FORMAT_STRING_HEXA,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN
                                    };

    r_key_group_t key_group_set;
    int cntPass = 0 ;
    int cntFail = 0 ;
    int cntSum = 0 ;

#if EDDSA_Test //zhxk 
    printf("\nTesting EDDSA sign...\n\n");

    icum_test_prng_init_rng();
    icum_test_prng_extend_seed();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 3));    // clear the ISD_BUFFER
    p_ISD0 = (r_icumif_isd_t *)BIG_BUFFER[1];
    p_ISD1 = p_ISD0 + 1;
    p_ISD2 = p_ISD1 + 1;
 
    for(ai_loop = 7; ai_loop <= 7; ai_loop++) //zhxk ECC_CURVE_BRAINPOOL_P320
    {
        p_ISD0->service_id     = SERVICE_03_ECC_KEY_GENERATE;
        p_ISD0->p_callbackfunc = 0;
        p_ISD0->job_id         = 0;

        p_ISD0->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;

        switch(ai_loop)
        {
            case 0:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P192;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve NIST P-192    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 1:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P224;
                key_group_set                                        = KEY_GRP_ECC;
                n                                                    = 1;
                printf("Testing on curve NIST P-224    Key group KEY_GRP_ECC ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 2:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P256;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve NIST P-256  ZHXK  Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 3:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_BRAINPOOL_P192;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve Brainpool P-192    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 4:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_BRAINPOOL_P224;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve Brainpool P-224    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 5:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_BRAINPOOL_P256;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve Brainpool P-256    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 6:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_BRAINPOOL_P320;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve Brainpool P-320    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 7:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_ED25519;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on Curve25519  EDDSA   Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            default:
                break;
        }

        p_ISD0->prm.ECC_KEY_GENERATE.key_group               = key_group_set;
        p_ISD0->prm.ECC_KEY_GENERATE.key_id                  = n;


        printf("Generate EDDSA ephemeral key (key index %d)...\n\n", n);

        // trigger the security service
        trigger_service_and_wait(p_ISD0);
        if(p_ISD0->service_result != SERV_OK) goto end;

        for(ai_loop2 = HASH_PRIMITIVE_SHA2_512; ai_loop2 <= HASH_PRIMITIVE_SHA2_512; ai_loop2++) // ZHXK HASH_PRIMITIVE_MD5--HASH_PRIMITIVE_SHA2_512
        {
            p_ISD1->service_id     = SERVICE_01_EDDSA_SIGN;
            p_ISD1->p_callbackfunc = 0;
            p_ISD1->job_id         = 0;

            p_ISD1->prm.EDDSA_SIGN.key_group               = key_group_set;
            p_ISD1->prm.EDDSA_SIGN.key_id                  = n;
            p_ISD1->prm.EDDSA_SIGN.p_source                = (void *)ecdsa_msg_sha1_P192; // just a test message!
            p_ISD1->prm.EDDSA_SIGN.source_size_in_bytes    = sizeof(ecdsa_msg_sha1_P192);
            p_ISD1->prm.EDDSA_SIGN.hash_primitive          = (r_hash_primitive_t)(ai_loop2);
            p_ISD1->prm.EDDSA_SIGN.job_slice_hash          = (ai_loop2 > 2) ? 0 : 1;
            p_ISD1->prm.EDDSA_SIGN.job_slice_point_mul     = ((ai_loop2 % 2) == 0) ? 0 : 1;
            p_ISD1->prm.EDDSA_SIGN.signature[0].p_data     = BIG_BUFFER[0];
            p_ISD1->prm.EDDSA_SIGN.signature[1].p_data     = BIG_BUFFER[0] + 1024;
            p_ISD1->prm.EDDSA_SIGN.signature_format        = sig_form[ai_loop2];
            p_ISD1->prm.EDDSA_SIGN.is_precalculated_hash   = 0u;

            printf("\n*------ EDDSA Test No. %02d\n", test_no);
            test_no++;

            printf("Start EDDSA signature generation \n[Count %02d] ...\n\n", ((ai_loop * 10) + (ai_loop2 + 1)));

            printf("Message size:    %d bytes\n",  p_ISD1->prm.EDDSA_SIGN.source_size_in_bytes );
            printf("HASH Primitive:  %d  \n",      p_ISD1->prm.EDDSA_SIGN.hash_primitive);
            printf("Job slice sha: %d block(s)\n", p_ISD1->prm.EDDSA_SIGN.job_slice_hash);
            printf("Job slice ecc: %d block(s)\n", p_ISD1->prm.EDDSA_SIGN.job_slice_point_mul);

            // trigger the security service
            vrun_chrono_start();
            trigger_service_and_wait(p_ISD1);
            vrun_chrono_stop();

            if(p_ISD1->service_result != SERV_OK)
            {
                continue;
            }

            // fetch execution time in vrun_chrono_xxx variables
            vrun_chrono_gettimestamp();

            m[0] = p_ISD1->prm.EDDSA_SIGN.signature_size_in_bits[0];
            m[1] = p_ISD1->prm.EDDSA_SIGN.signature_size_in_bits[1];

            printf("EDDSA Signature format: %d  (0:DEC  1:HEXA  2:BIN)\n", sig_form[ai_loop2]);
            printf("EDDSA Signature size (r): %d bits\n", m[0]);
            printf("EDDSA Signature size (s): %d bits\n", m[1]);
            switch (sig_form[ai_loop2])
            {
                case FORMAT_BINARY_PLAIN:
                    printf("BINARY_PLAIN Signature (r):      "); PMEM((const unsigned char *)p_ISD1->prm.EDDSA_SIGN.signature[0].p_data, p_ISD1->prm.EDDSA_SIGN.signature_size_in_bits[0] / 8); NL(); FLUSH();
                    printf("BINARY_PLAIN Signature (s):      "); PMEM((const unsigned char *)p_ISD1->prm.EDDSA_SIGN.signature[1].p_data, p_ISD1->prm.EDDSA_SIGN.signature_size_in_bits[1] / 8); NL(); FLUSH();
                    break;
                case FORMAT_STRING_DECIMAL:
                case FORMAT_STRING_HEXA:
                    printf("STRING Signature (r):      %s", p_ISD1->prm.EDDSA_SIGN.signature[0].p_data); NL(); FLUSH();
                    printf("STRING Signature (s):      %s", p_ISD1->prm.EDDSA_SIGN.signature[1].p_data); NL(); FLUSH();
                    break;
                default:
                    break;
            }
            printf("Processed in :      "); PCHRONO(); NL(); FLUSH();
            
            p_ISD2->service_id     = SERVICE_01_EDDSA_VERIFY;
            p_ISD2->p_callbackfunc = 0;
            p_ISD2->job_id         = 0;
            
            p_ISD2->prm.EDDSA_VERIFY.key_group                   = key_group_set;
            p_ISD2->prm.EDDSA_VERIFY.key_id                      = n;
            p_ISD2->prm.EDDSA_VERIFY.p_source                    = (void *)ecdsa_msg_sha1_P192;   // just a test message!
            p_ISD2->prm.EDDSA_VERIFY.source_size_in_bytes        = sizeof(ecdsa_msg_sha1_P192);
            p_ISD2->prm.EDDSA_VERIFY.hash_primitive              = (r_hash_primitive_t)(ai_loop2);
            p_ISD2->prm.EDDSA_VERIFY.job_slice_hash              = (ai_loop2 < 2) ? 0 : 1;
            p_ISD2->prm.EDDSA_VERIFY.job_slice_point_mul         = ((ai_loop2 % 2) == 0) ? 0 : 1;
            p_ISD2->prm.EDDSA_VERIFY.signature[0].p_data         = BIG_BUFFER[0];
            p_ISD2->prm.EDDSA_VERIFY.signature[1].p_data         = BIG_BUFFER[0] + 1024;
            p_ISD2->prm.EDDSA_VERIFY.signature_size_in_bits[0]   = m[0];
            p_ISD2->prm.EDDSA_VERIFY.signature_size_in_bits[1]   = m[1];
            p_ISD2->prm.EDDSA_VERIFY.signature_format            = sig_form[ai_loop2];
            p_ISD2->prm.EDDSA_VERIFY.is_precalculated_hash       = 0u;
            p_ISD2->prm.EDDSA_VERIFY.verification_result         = VERIFICATION_FAIL;

            printf("\n EDDSA Start signature verification... \n\n");

            // trigger the security service
            vrun_chrono_start();
            trigger_service_and_wait(p_ISD2);
            vrun_chrono_stop();
            
            // fetch execution time in vrun_chrono_xxx variables
            vrun_chrono_gettimestamp();

            printf("Processed in : "); PCHRONO(); NL(); FLUSH();
            printf("EDDSA Verification result : ");
            cntSum ++ ;
            if(p_ISD2->prm.EDDSA_VERIFY.verification_result == VERIFICATION_PASS)
            {
                printf("PASS!\n\n");
                cntPass ++ ;
            }
            else
            {
                printf("*FAIL*!\a\n\n");
                cntFail ++ ;
                printf( " Test is in Fail.\n" );
            }
        }
    }
    
end:
    printf("\nEDDSA_SIGN ...");
    printf( " Test Result : All %d, Pass = %d, Fail = %d\n", cntSum, cntPass, cntFail );

    printf("\nDone!\n");
#endif
}

void ecdsa_sign_nml(void)
{
    r_icumif_isd_t *p_ISD0;
    r_icumif_isd_t *p_ISD1;
    r_icumif_isd_t *p_ISD2;
    unsigned int test_no = 1;
    int n = 0;
    int m[2];
    int ai_loop, ai_loop2;
    r_number_format_t sig_form [] = {
                                         FORMAT_STRING_DECIMAL,
                                         FORMAT_STRING_HEXA,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN,
                                         FORMAT_BINARY_PLAIN
                                    };

    r_key_group_t key_group_set;
    int cntPass = 0 ;
    int cntFail = 0 ;
    int cntSum = 0 ;

#if Init_Test

    for(ai_loop = 0; ai_loop <= ECC_CURVE_BRAINPOOL_P320; ai_loop++)
    {
        p_ISD0->service_id     = SERVICE_03_ECC_KEY_GENERATE;
        p_ISD0->p_callbackfunc = 0;
        p_ISD0->job_id         = 0;

        p_ISD0->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;

        switch(ai_loop)
        {
            case 0:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P192;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve NIST P-192    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 1:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P224;
                key_group_set                                        = KEY_GRP_ECC;
                n                                                    = 1;
                printf("Testing on curve NIST P-224    Key group KEY_GRP_ECC ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 2:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P256;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve NIST P-256    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 3:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_BRAINPOOL_P192;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve Brainpool P-192    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 4:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_BRAINPOOL_P224;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve Brainpool P-224    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 5:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_BRAINPOOL_P256;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve Brainpool P-256    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            case 6:
                p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_BRAINPOOL_P320;
                key_group_set                                        = KEY_GRP_ECC_RAM;
                n                                                    = 0;
                printf("Testing on curve Brainpool P-320    Key group KEY_GRP_ECC_RAM ...\n");
                printf("--------------------------------------------\n\n");
                break;
            default:
                break;
        }

        p_ISD0->prm.ECC_KEY_GENERATE.key_group               = key_group_set;
        p_ISD0->prm.ECC_KEY_GENERATE.key_id                  = n;


        printf("Generate ephemeral key (key index %d)...\n\n", n);

        // trigger the security service
        trigger_service_and_wait(p_ISD0);
        if(p_ISD0->service_result != SERV_OK) goto end;

        for(ai_loop2 = HASH_PRIMITIVE_MD5; ai_loop2 <= HASH_PRIMITIVE_SHA2_512; ai_loop2++)
        {
            p_ISD1->service_id     = SERVICE_01_ECDSA_SIGN;
            p_ISD1->p_callbackfunc = 0;
            p_ISD1->job_id         = 0;

            p_ISD1->prm.ECDSA_SIGN.key_group               = key_group_set;
            p_ISD1->prm.ECDSA_SIGN.key_id                  = n;
            p_ISD1->prm.ECDSA_SIGN.p_source                = (void *)ecdsa_msg_sha1_P192; // just a test message!
            p_ISD1->prm.ECDSA_SIGN.source_size_in_bytes    = sizeof(ecdsa_msg_sha1_P192);
            p_ISD1->prm.ECDSA_SIGN.hash_primitive          = (r_hash_primitive_t)(ai_loop2);
            p_ISD1->prm.ECDSA_SIGN.job_slice_hash          = (ai_loop2 > 2) ? 0 : 1;
            p_ISD1->prm.ECDSA_SIGN.job_slice_point_mul     = ((ai_loop2 % 2) == 0) ? 0 : 1;
            p_ISD1->prm.ECDSA_SIGN.signature[0].p_data     = BIG_BUFFER[0];
            p_ISD1->prm.ECDSA_SIGN.signature[1].p_data     = BIG_BUFFER[0] + 1024;
            p_ISD1->prm.ECDSA_SIGN.signature_format        = sig_form[ai_loop2];
            p_ISD1->prm.ECDSA_SIGN.is_precalculated_hash   = 0u;

            printf("\n*------ Test No. %02d\n", test_no);
            test_no++;

            printf("Start signature generation\n[Count %02d] ...\n\n", ((ai_loop * 10) + (ai_loop2 + 1)));

            printf("Message size:    %d bytes\n",  p_ISD1->prm.ECDSA_SIGN.source_size_in_bytes );
            printf("HASH Primitive:  %d  \n",      p_ISD1->prm.ECDSA_SIGN.hash_primitive);
            printf("Job slice sha: %d block(s)\n", p_ISD1->prm.ECDSA_SIGN.job_slice_hash);
            printf("Job slice ecc: %d block(s)\n", p_ISD1->prm.ECDSA_SIGN.job_slice_point_mul);

            // trigger the security service
            vrun_chrono_start();
            trigger_service_and_wait(p_ISD1);
            vrun_chrono_stop();

            if(p_ISD1->service_result != SERV_OK)
            {
                continue;
            }

            // fetch execution time in vrun_chrono_xxx variables
            vrun_chrono_gettimestamp();

            m[0] = p_ISD1->prm.ECDSA_SIGN.signature_size_in_bits[0];
            m[1] = p_ISD1->prm.ECDSA_SIGN.signature_size_in_bits[1];

            printf("Signature format: %d  (0:DEC  1:HEXA  2:BIN)\n", sig_form[ai_loop2]);
            printf("Signature size (r): %d bits\n", m[0]);
            printf("Signature size (s): %d bits\n", m[1]);
            switch (sig_form[ai_loop2])
            {
                case FORMAT_BINARY_PLAIN:
                    printf("Signature (r):      "); PMEM((const unsigned char *)p_ISD1->prm.ECDSA_SIGN.signature[0].p_data, p_ISD1->prm.ECDSA_SIGN.signature_size_in_bits[0] / 8); NL(); FLUSH();
                    printf("Signature (s):      "); PMEM((const unsigned char *)p_ISD1->prm.ECDSA_SIGN.signature[1].p_data, p_ISD1->prm.ECDSA_SIGN.signature_size_in_bits[1] / 8); NL(); FLUSH();
                    break;
                case FORMAT_STRING_DECIMAL:
                case FORMAT_STRING_HEXA:
                    printf("Signature (r):      %s", p_ISD1->prm.ECDSA_SIGN.signature[0].p_data); NL(); FLUSH();
                    printf("Signature (s):      %s", p_ISD1->prm.ECDSA_SIGN.signature[1].p_data); NL(); FLUSH();
                    break;
                default:
                    break;
            }
            printf("Processed in :      "); PCHRONO(); NL(); FLUSH();
            
            p_ISD2->service_id     = SERVICE_01_ECDSA_VERIFY;
            p_ISD2->p_callbackfunc = 0;
            p_ISD2->job_id         = 0;
            
            p_ISD2->prm.ECDSA_VERIFY.key_group                   = key_group_set;
            p_ISD2->prm.ECDSA_VERIFY.key_id                      = n;
            p_ISD2->prm.ECDSA_VERIFY.p_source                    = (void *)ecdsa_msg_sha1_P192;   // just a test message!
            p_ISD2->prm.ECDSA_VERIFY.source_size_in_bytes        = sizeof(ecdsa_msg_sha1_P192);
            p_ISD2->prm.ECDSA_VERIFY.hash_primitive              = (r_hash_primitive_t)(ai_loop2);
            p_ISD2->prm.ECDSA_VERIFY.job_slice_hash              = (ai_loop2 < 2) ? 0 : 1;
            p_ISD2->prm.ECDSA_VERIFY.job_slice_point_mul         = ((ai_loop2 % 2) == 0) ? 0 : 1;
            p_ISD2->prm.ECDSA_VERIFY.signature[0].p_data         = BIG_BUFFER[0];
            p_ISD2->prm.ECDSA_VERIFY.signature[1].p_data         = BIG_BUFFER[0] + 1024;
            p_ISD2->prm.ECDSA_VERIFY.signature_size_in_bits[0]   = m[0];
            p_ISD2->prm.ECDSA_VERIFY.signature_size_in_bits[1]   = m[1];
            p_ISD2->prm.ECDSA_VERIFY.signature_format            = sig_form[ai_loop2];
            p_ISD2->prm.ECDSA_VERIFY.is_precalculated_hash       = 0u;
            p_ISD2->prm.ECDSA_VERIFY.verification_result         = VERIFICATION_FAIL;

            printf("\nStart signature verification...\n\n");

            // trigger the security service
            vrun_chrono_start();
            trigger_service_and_wait(p_ISD2);
            vrun_chrono_stop();
            
            // fetch execution time in vrun_chrono_xxx variables
            vrun_chrono_gettimestamp();

            printf("Processed in : "); PCHRONO(); NL(); FLUSH();
            printf("Verification result : ");
            cntSum ++ ;
            if(p_ISD2->prm.ECDSA_VERIFY.verification_result == VERIFICATION_PASS)
            {
                printf("PASS!\n\n");
                cntPass ++ ;
            }
            else
            {
                printf("*FAIL*!\a\n\n");
                cntFail ++ ;
                printf( " Test is in Fail.\n" );
            }
        }
    }
    
end:
    printf("\nECDSA_SIGN ...");
    printf( " Test Result : All %d, Pass = %d, Fail = %d\n", cntSum, cntPass, cntFail );

    printf("\nDone!\n");
#endif
}


static void ecdsa_sign_dbg(void)
{
    r_icumif_isd_t *p_ISD0;
    r_icumif_isd_t *p_ISD1;
    int n = 0;

    printf("\n*------ Test No. 71 | 72  (Debugger  Disconnected : 71    Connected : 72)\n");
    printf("\nTesting ECDSA sign Debugger check...\n\n");


    icum_test_prng_init_rng();
    icum_test_prng_extend_seed();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 2));    // clear the ISD_BUFFER
    p_ISD0 = (r_icumif_isd_t *)BIG_BUFFER[1];
    p_ISD1 = p_ISD0 + 1;

    p_ISD0->service_id     = SERVICE_03_ECC_KEY_GENERATE;
    p_ISD0->p_callbackfunc = 0;
    p_ISD0->job_id         = 0;
    
    p_ISD0->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC;
    p_ISD0->prm.ECC_KEY_GENERATE.key_id                  = n;
    p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P192;
    p_ISD0->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 1;                       // debug ON !!

    printf("Generate ephemeral key (key index %d)...\n\n", n);

    // trigger the security service
    trigger_service_and_wait(p_ISD0);
    if(p_ISD0->service_result != SERV_OK) goto end;


    p_ISD1->service_id     = SERVICE_01_ECDSA_SIGN;
    p_ISD1->p_callbackfunc = 0;
    p_ISD1->job_id         = 0;

    p_ISD1->prm.ECDSA_SIGN.key_group               = KEY_GRP_ECC;
    p_ISD1->prm.ECDSA_SIGN.key_id                  = n;
    p_ISD1->prm.ECDSA_SIGN.p_source                = (void *)ecdsa_msg_sha1_P192;
    p_ISD1->prm.ECDSA_SIGN.source_size_in_bytes    = sizeof(ecdsa_msg_sha1_P192);
    p_ISD1->prm.ECDSA_SIGN.hash_primitive          = HASH_PRIMITIVE_SHA1;
    p_ISD1->prm.ECDSA_SIGN.job_slice_hash          = 0;
    p_ISD1->prm.ECDSA_SIGN.job_slice_point_mul     = 1;
    p_ISD1->prm.ECDSA_SIGN.signature[0].p_data     = BIG_BUFFER[0];
    p_ISD1->prm.ECDSA_SIGN.signature[1].p_data     = BIG_BUFFER[0] + 1024;
    p_ISD1->prm.ECDSA_SIGN.signature_format        = FORMAT_BINARY_PLAIN;
    p_ISD1->prm.ECDSA_SIGN.is_precalculated_hash   = 0u;

    printf("Start signature generation...\n[Debugger check]\n\n");

    printf("Message size:    %d bytes\n",    p_ISD1->prm.ECDSA_SIGN.source_size_in_bytes );
    printf("HASH Primitive:  %d  (0 : MD5)\n",    p_ISD1->prm.ECDSA_SIGN.hash_primitive);

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD1);
    vrun_chrono_stop();

    if(p_ISD1->service_result != SERV_OK)
    {
        goto end;
    }


    
end:

    printf("\nDone!\n");
}


static void ecdsa_sign_cluster(void)
{
    r_icumif_isd_t *p_ISD0;
    r_icumif_isd_t *p_ISD1;
    int n = 0;

    printf("\n*------ Test No. 73\n");
    printf("\nTesting ECDSA sign...\n\n");

    icum_test_prng_init_rng();
    icum_test_prng_extend_seed();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 2));    // clear the ISD_BUFFER
    p_ISD0 = (r_icumif_isd_t *)BIG_BUFFER[1];
    p_ISD1 = p_ISD0 + 1;

    printf("\nECC key pair import Done!\n");

    p_ISD0->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD0->p_callbackfunc = 0;
    p_ISD0->job_id         = 0;

    p_ISD0->prm.PK_IMPORT.key_group                = KEY_GRP_ECC_PUB;
    p_ISD0->prm.PK_IMPORT.key_id                   = 0;

    p_ISD0->prm.PK_IMPORT.p_pub_point_x            = point_x_sha1_P192;
    p_ISD0->prm.PK_IMPORT.p_pub_point_y            = point_y_sha1_P192;
    p_ISD0->prm.PK_IMPORT.hash_primitive           = HASH_PRIMITIVE_SHA1;

    p_ISD0->prm.PK_IMPORT.point_x_size_in_bits     = 192;
    p_ISD0->prm.PK_IMPORT.point_y_size_in_bits     = 192;
    p_ISD0->prm.PK_IMPORT.priv_scalar_size_in_bits = 0;

    p_ISD0->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
    p_ISD0->prm.PK_IMPORT.curve_id                 = ECC_CURVE_NIST_P192;
    p_ISD0->prm.PK_IMPORT.sig_key_id               = n;

    p_ISD0->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
    p_ISD0->prm.PK_IMPORT.skip_verify              = 1;    /* without valification */
    p_ISD0->prm.PK_IMPORT.flags.disable_on_debug   = 0;
    p_ISD0->prm.PK_IMPORT.flags.write_protected    = 0;

    printf("Generate ephemeral key (key index %d)...\n\n", n);

    // trigger the security service
    trigger_service_and_wait(p_ISD0);
    if(p_ISD0->service_result != SERV_OK) goto end;

    p_ISD1->service_id     = SERVICE_01_ECDSA_SIGN;
    p_ISD1->p_callbackfunc = 0;
    p_ISD1->job_id         = 0;

    p_ISD1->prm.ECDSA_SIGN.key_group               = KEY_GRP_ECC_PUB;
    p_ISD1->prm.ECDSA_SIGN.key_id                  = n;
    p_ISD1->prm.ECDSA_SIGN.p_source                = (void *)ecdsa_msg_sha1_P192;
    p_ISD1->prm.ECDSA_SIGN.source_size_in_bytes    = sizeof(ecdsa_msg_sha1_P192);
    p_ISD1->prm.ECDSA_SIGN.hash_primitive          = HASH_PRIMITIVE_SHA1;
    p_ISD1->prm.ECDSA_SIGN.job_slice_hash          = 0;
    p_ISD1->prm.ECDSA_SIGN.job_slice_point_mul     = 1;
    p_ISD1->prm.ECDSA_SIGN.signature[0].p_data     = BIG_BUFFER[0];
    p_ISD1->prm.ECDSA_SIGN.signature[1].p_data     = BIG_BUFFER[0] + 1024;
    p_ISD1->prm.ECDSA_SIGN.signature_format        = FORMAT_BINARY_PLAIN;
    p_ISD1->prm.ECDSA_SIGN.is_precalculated_hash   = 0u;

    printf("Start signature generation...\n\n");

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD1);
    vrun_chrono_stop();

    if(p_ISD1->service_result != SERV_OK)
    {
        goto end;
    }


    
end:

    printf("\nDone!\n");
}


static void ecdsa_sign_curve(void)
{
    r_icumif_isd_t *p_ISD0;
    r_icumif_isd_t *p_ISD1;
    int n = 0;

    printf("\n*------ Test No. 74\n");
    printf("\nTesting ECDSA sign...\n[Curve25519 check]\n\n");

    icum_test_prng_init_rng();
    icum_test_prng_extend_seed();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 2));    // clear the ISD_BUFFER
    p_ISD0 = (r_icumif_isd_t *)BIG_BUFFER[1];
    p_ISD1 = p_ISD0 + 1;

    p_ISD0->service_id     = SERVICE_03_ECC_KEY_GENERATE;
    p_ISD0->p_callbackfunc = 0;
    p_ISD0->job_id         = 0;
    
    p_ISD0->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC_RAM;
    p_ISD0->prm.ECC_KEY_GENERATE.key_id                  = n;
    p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_CURVE25519;
    p_ISD0->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;

    printf("Generate ephemeral key (key index %d)...\n\n", n);

    // trigger the security service
    trigger_service_and_wait(p_ISD0);
    if(p_ISD0->service_result != SERV_OK) goto end;


    p_ISD1->service_id     = SERVICE_01_ECDSA_SIGN;
    p_ISD1->p_callbackfunc = 0;
    p_ISD1->job_id         = 0;

    p_ISD1->prm.ECDSA_SIGN.key_group               = KEY_GRP_ECC_RAM;
    p_ISD1->prm.ECDSA_SIGN.key_id                  = n;
    p_ISD1->prm.ECDSA_SIGN.p_source                = (void *)ecdsa_msg_sha1_P192;
    p_ISD1->prm.ECDSA_SIGN.source_size_in_bytes    = sizeof(ecdsa_msg_sha1_P192);
    p_ISD1->prm.ECDSA_SIGN.hash_primitive          = HASH_PRIMITIVE_SHA1;
    p_ISD1->prm.ECDSA_SIGN.job_slice_hash          = 0;
    p_ISD1->prm.ECDSA_SIGN.job_slice_point_mul     = 1;
    p_ISD1->prm.ECDSA_SIGN.signature[0].p_data     = BIG_BUFFER[0];
    p_ISD1->prm.ECDSA_SIGN.signature[1].p_data     = BIG_BUFFER[0] + 1024;
    p_ISD1->prm.ECDSA_SIGN.signature_format        = FORMAT_BINARY_PLAIN;
    p_ISD1->prm.ECDSA_SIGN.is_precalculated_hash   = 0u;

    printf("Start signature generation Curve...\n");
    printf("Message size:    %d bytes\n",    p_ISD1->prm.ECDSA_SIGN.source_size_in_bytes );
    printf("HASH Primitive:  %d  (0 : MD5)\n",    p_ISD1->prm.ECDSA_SIGN.hash_primitive);

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD1);
    vrun_chrono_stop();

    if(p_ISD1->service_result != SERV_OK)
    {
        goto end;
    }


end:

    printf("\nDone!\n");
}


static void ecdsa_sign_mem(void)
{
    r_icumif_isd_t *p_ISD0;
    r_icumif_isd_t *p_ISD1;
    unsigned int test_no = 75;
    int n = 0;
    int ai_loop;
    uint8_t ecdsa_msg_sha1_P224_AUTO[] = {0xed, 0xfb, 0x1e, 0x8f, 0x6d, 0x45, 0x34, 0x5d, 0x23, 0xb1, 0x94, 0xf9, 0xb2, 0x5c, 0x4f, 0xfd, 0xea, 0x45, 0x27, 0x77, 0x15, 0x36, 0x3f, 0xe4, 0x7b, 0x96, 0x4a, 0x52, 0x02, 0x0c, 0xfc, 0x4e, 0x20, 0x21, 0x44, 0x5c, 0xa8, 0x50, 0x83, 0x63, 0x40, 0xa2, 0x82, 0x6e, 0xfd, 0x84, 0xed, 0x74, 0x24, 0xa2, 0xc0, 0x9e, 0xf0, 0x28, 0x71, 0xe5, 0x59, 0x4d, 0xaf, 0xe2, 0x5d, 0x56, 0x31, 0xd6, 0xb3, 0x2c, 0x38, 0x5d, 0x9b, 0xe2, 0x01, 0x70, 0x15, 0xc1, 0x7f, 0xcf, 0xde, 0x20, 0xa9, 0xcb, 0x2b, 0xa2, 0x25, 0x0c, 0xa3, 0x56, 0xbd, 0xec, 0x17, 0x70, 0xc8, 0x10, 0xc2, 0x2c, 0x64, 0x7e, 0x83, 0x43, 0xf3, 0x74, 0x80, 0x87, 0x75, 0x99, 0x54, 0x25, 0x8d, 0x85, 0x6d, 0x6e, 0x2e, 0x5f, 0x13, 0xd8, 0xdf, 0x4a, 0x07, 0xb3, 0xea, 0x03, 0x6c, 0xbf, 0x21, 0x5c, 0x30, 0x99, 0x22, 0x4c};

    printf("\nTesting ECDSA sign mem check...\n\n");

    icum_test_prng_init_rng();
    icum_test_prng_extend_seed();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 2));    // clear the ISD_BUFFER
    p_ISD0 = (r_icumif_isd_t *)BIG_BUFFER[1];
    p_ISD1 = p_ISD0 + 1;

    p_ISD0->service_id     = SERVICE_03_ECC_KEY_GENERATE;
    p_ISD0->p_callbackfunc = 0;
    p_ISD0->job_id         = 0;
    
    p_ISD0->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC_RAM;
    p_ISD0->prm.ECC_KEY_GENERATE.key_id                  = n;
    p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P192;
    p_ISD0->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;

    printf("Generate ephemeral key (key index %d)...\n\n", n);

    // trigger the security service
    trigger_service_and_wait(p_ISD0);
    if(p_ISD0->service_result != SERV_OK) goto end;


    for (ai_loop = 0; ai_loop < 8; ai_loop++)
    {
        p_ISD1->service_id     = SERVICE_01_ECDSA_SIGN;
        p_ISD1->p_callbackfunc = 0;
        p_ISD1->job_id         = 0;
    
        p_ISD1->prm.ECDSA_SIGN.key_group               = KEY_GRP_ECC_RAM;
        p_ISD1->prm.ECDSA_SIGN.key_id                  = n;
        p_ISD1->prm.ECDSA_SIGN.p_source                = (void *)ecdsa_msg_sha1_P192;
        p_ISD1->prm.ECDSA_SIGN.source_size_in_bytes    = sizeof(ecdsa_msg_sha1_P192);
        p_ISD1->prm.ECDSA_SIGN.hash_primitive          = HASH_PRIMITIVE_SHA1;
        p_ISD1->prm.ECDSA_SIGN.job_slice_hash          = 0;
        p_ISD1->prm.ECDSA_SIGN.job_slice_point_mul     = 1;
        p_ISD1->prm.ECDSA_SIGN.signature[0].p_data     = BIG_BUFFER[0];
        p_ISD1->prm.ECDSA_SIGN.signature[1].p_data     = BIG_BUFFER[0] + 1024;
        p_ISD1->prm.ECDSA_SIGN.signature_format        = FORMAT_BINARY_PLAIN;
        p_ISD1->prm.ECDSA_SIGN.is_precalculated_hash   = 0u;

        switch(ai_loop)
        {
            case 0:
                p_ISD1->prm.ECDSA_SIGN.p_source  = (void *)NULL;                       // just a test message!
                break;
            case 1:
                p_ISD1->prm.ECDSA_SIGN.signature[0].p_data = (void *)NULL;
                break;
            case 2:
                p_ISD1->prm.ECDSA_SIGN.signature[1].p_data = (void *)NULL;
                break;
            case 3:
                p_ISD1->prm.ECDSA_SIGN.p_source  = ecdsa_msg_sha1_P224_AUTO;
                break;
            case 4:
                p_ISD1->prm.ECDSA_SIGN.hash_primitive = (r_hash_primitive_t)(HASH_PRIMITIVE_SHA2_512 + 1);
                break;
            case 5:
                p_ISD1->prm.ECDSA_SIGN.signature_format = (r_number_format_t)(FORMAT_BINARY_PLAIN + 1);
                break;
            case 6:
                p_ISD1->prm.ECDSA_SIGN.key_group = KEY_GRP_RSA;
                break;
            case 7:
                p_ISD1->prm.ECDSA_SIGN.key_id = n + 6;
                break;
            default:
                break;
        }

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("---------\nStart signature generation\n[mem check %02d]...\n\n", (ai_loop + 1));

        // trigger the security service
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD1);
        vrun_chrono_stop();
        printf("Result : %s\n",R_ICUMIF_GetErrorCodeName(p_ISD1->service_result));
    }

end:

    printf("\nDone!\n");
}


void ecdsa_sign_multi(void)
{
    r_icumif_isd_t *p_ISD0;
    r_icumif_isd_t *p_ISD1;
    r_icumif_isd_t *p_ISD2;
    int n = 0;

    printf("\n*------ Test No. 83\n");
    printf("\nTesting ECDSA sign...\n[Multi start-up]\n\n");

    icum_test_prng_init_rng();
    icum_test_prng_extend_seed();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 3));    // clear the ISD_BUFFER
    p_ISD0 = (r_icumif_isd_t *)BIG_BUFFER[1];
    p_ISD1 = p_ISD0 + 1;
    p_ISD2 = p_ISD1 + 1;

    p_ISD0->service_id     = SERVICE_03_ECC_KEY_GENERATE;
    p_ISD0->p_callbackfunc = 0;
    p_ISD0->job_id         = 0;
    
    p_ISD0->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC_RAM;
    p_ISD0->prm.ECC_KEY_GENERATE.key_id                  = n;
    p_ISD0->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P192;
    p_ISD0->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;

    printf("Generate ephemeral key (key index %d)...\n\n", n);

    // trigger the security service
    trigger_service_and_wait(p_ISD0);
    if(p_ISD0->service_result != SERV_OK) goto end;


    p_ISD1->service_id     = SERVICE_01_ECDSA_SIGN;
    p_ISD1->p_callbackfunc = 0;
    p_ISD1->job_id         = 0;
    p_ISD1->prm.ECDSA_SIGN.key_group               = KEY_GRP_ECC_RAM;
    p_ISD1->prm.ECDSA_SIGN.key_id                  = n;
    p_ISD1->prm.ECDSA_SIGN.p_source                = (void *)ecdsa_msg_sha1_P192;
    p_ISD1->prm.ECDSA_SIGN.source_size_in_bytes    = sizeof(ecdsa_msg_sha1_P192);
    p_ISD1->prm.ECDSA_SIGN.hash_primitive          = HASH_PRIMITIVE_SHA1;
    p_ISD1->prm.ECDSA_SIGN.job_slice_hash          = 0;
    p_ISD1->prm.ECDSA_SIGN.job_slice_point_mul     = 1;
    p_ISD1->prm.ECDSA_SIGN.signature[0].p_data     = BIG_BUFFER[0];
    p_ISD1->prm.ECDSA_SIGN.signature[1].p_data     = BIG_BUFFER[0] + 1024;
    p_ISD1->prm.ECDSA_SIGN.signature_format        = FORMAT_BINARY_PLAIN;
    p_ISD1->prm.ECDSA_SIGN.is_precalculated_hash   = 0u;

    p_ISD2->service_id     = SERVICE_01_ECDSA_SIGN;
    p_ISD2->p_callbackfunc = 0;
    p_ISD2->job_id         = 0;
    p_ISD2->prm.ECDSA_SIGN.key_group               = KEY_GRP_ECC_RAM;
    p_ISD2->prm.ECDSA_SIGN.key_id                  = n;
    p_ISD2->prm.ECDSA_SIGN.p_source                = (void *)ecdsa_msg_sha1_P192;
    p_ISD2->prm.ECDSA_SIGN.source_size_in_bytes    = sizeof(ecdsa_msg_sha1_P192);
    p_ISD2->prm.ECDSA_SIGN.hash_primitive          = HASH_PRIMITIVE_SHA1;
    p_ISD2->prm.ECDSA_SIGN.job_slice_hash          = 0;
    p_ISD2->prm.ECDSA_SIGN.job_slice_point_mul     = 1;
    p_ISD2->prm.ECDSA_SIGN.signature[0].p_data     = BIG_BUFFER[0];
    p_ISD2->prm.ECDSA_SIGN.signature[1].p_data     = BIG_BUFFER[0] + 1024;
    p_ISD2->prm.ECDSA_SIGN.signature_format        = FORMAT_BINARY_PLAIN;
    p_ISD2->prm.ECDSA_SIGN.is_precalculated_hash   = 0u;

    printf("Start signature generation Multi...\n");

    // trigger the security service
    vrun_chrono_start();
    trigger_2service_and_wait(p_ISD1, p_ISD2);
    vrun_chrono_stop();

    printf("ISD1 Result : %s\n",R_ICUMIF_GetErrorCodeName(p_ISD1->service_result));
    printf("ISD2 Result : %s\n",R_ICUMIF_GetErrorCodeName(p_ISD2->service_result));

end:

    printf("\nDone!\n");
}

void ecdsa_sign_syscfg(void)
{
    r_icumif_isd_t *p_ISD1;
    int n = 0;

    printf("\n*------ Test No. 84\n");
    printf("\nTesting ECDSA sign...\n[Multi start-up]\n\n");

    icum_test_prng_init_rng();
    icum_test_prng_extend_seed();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD1 = (r_icumif_isd_t *)BIG_BUFFER[1];

    p_ISD1->service_id     = SERVICE_01_ECDSA_SIGN;
    p_ISD1->p_callbackfunc = 0;
    p_ISD1->job_id         = 0;
    p_ISD1->prm.ECDSA_SIGN.key_group               = KEY_GRP_ECC_RAM;
    p_ISD1->prm.ECDSA_SIGN.key_id                  = n;
    p_ISD1->prm.ECDSA_SIGN.p_source                = (void *)ecdsa_msg_sha1_P192;
    p_ISD1->prm.ECDSA_SIGN.source_size_in_bytes    = sizeof(ecdsa_msg_sha1_P192);
    p_ISD1->prm.ECDSA_SIGN.hash_primitive          = HASH_PRIMITIVE_SHA1;
    p_ISD1->prm.ECDSA_SIGN.job_slice_hash          = 0;
    p_ISD1->prm.ECDSA_SIGN.job_slice_point_mul     = 1;
    p_ISD1->prm.ECDSA_SIGN.signature[0].p_data     = BIG_BUFFER[0];
    p_ISD1->prm.ECDSA_SIGN.signature[1].p_data     = BIG_BUFFER[0] + 1024;
    p_ISD1->prm.ECDSA_SIGN.signature_format        = FORMAT_BINARY_PLAIN;
    p_ISD1->prm.ECDSA_SIGN.is_precalculated_hash   = 0u;

    printf("Start signature generation syscfg...\n");

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD1);
    vrun_chrono_stop();

    printf("ISD1 Result : %s\n",R_ICUMIF_GetErrorCodeName(p_ISD1->service_result));

    printf("\nDone!\n");
}


void icum_ecc_key_pair_import(void)
{
    r_icumif_isd_t *p_ISD;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nTesting ECC key pair import index #0 / curve NIST P-256 (pub & priv)...\n\n");

    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_ECC;
    p_ISD->prm.PK_IMPORT.key_id                   = 0;

    p_ISD->prm.PK_IMPORT.p_pub_point_x            = pub_point_x_2;
    p_ISD->prm.PK_IMPORT.p_pub_point_y            = pub_point_y_2;
    p_ISD->prm.PK_IMPORT.p_priv_scalar            = priv_scalar_2;
    p_ISD->prm.PK_IMPORT.curve_id                 = ECC_CURVE_NIST_P256;

    p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = sizeof(pub_point_x_2) * 8;
    p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = sizeof(pub_point_y_2) * 8;
    p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = sizeof(priv_scalar_2) * 8;

    p_ISD->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
    p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
    p_ISD->prm.PK_IMPORT.hash_primitive           = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;
    
    // trigger the security service
    trigger_service_and_wait(p_ISD);
    
    printf("\nDone!\n");
}


void icum_ecc_key_pair_import_dbg_on(void)
{
    r_icumif_isd_t *p_ISD;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nTesting ECC key pair import index #0 / curve NIST P-256 (pub & priv), debug ON...\n\n");

    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_ECC;
    p_ISD->prm.PK_IMPORT.key_id                   = 0;

    p_ISD->prm.PK_IMPORT.p_pub_point_x            = pub_point_x_2;
    p_ISD->prm.PK_IMPORT.p_pub_point_y            = pub_point_y_2;
    p_ISD->prm.PK_IMPORT.p_priv_scalar            = priv_scalar_2;
    p_ISD->prm.PK_IMPORT.curve_id                 = ECC_CURVE_NIST_P256;

    p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = sizeof(pub_point_x_2) * 8;
    p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = sizeof(pub_point_y_2) * 8;
    p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = sizeof(priv_scalar_2) * 8;

    p_ISD->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
    p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
    p_ISD->prm.PK_IMPORT.hash_primitive           = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 1;
    
    // trigger the security service
    trigger_service_and_wait(p_ISD);
    
    printf("\nDone!\n");
}


void icum_ecc_key_pair_import_25519(void)
{
    r_icumif_isd_t *p_ISD;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nTesting ECC key pair import index #0 / Curve25519 (pub & priv)...\n\n");

    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_ECC;
    p_ISD->prm.PK_IMPORT.key_id                   = 0;

    p_ISD->prm.PK_IMPORT.p_pub_point_x            = pub_point_x_2;
    p_ISD->prm.PK_IMPORT.p_pub_point_y            = pub_point_y_2;
    p_ISD->prm.PK_IMPORT.p_priv_scalar            = priv_scalar_2;
    p_ISD->prm.PK_IMPORT.curve_id                 = ECC_CURVE_CURVE25519;

    p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = sizeof(pub_point_x_2) * 8;
    p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = sizeof(pub_point_y_2) * 8;
    p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = sizeof(priv_scalar_2) * 8;

    p_ISD->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
    p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
    p_ISD->prm.PK_IMPORT.hash_primitive           = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;
    
    // trigger the security service
    trigger_service_and_wait(p_ISD);
    
    printf("\nDone!\n");
}


void icum_ecdsa_sign(void)
{
    int n;
    
    printf("\n\nTest ECDSA sign...\n");
       
    while(1)
    {
        printf("\n| select ECDSA sign test\n");
        printf("|---------------------------------\n");
        printf("| 0   : sign normal\n");
        printf("| 1   : debugger check\n");
        printf("| 2   : cluster check\n");
        printf("| 3   : curve check\n");
        printf("| 4   : pointer check\n");
        printf("| 5   : Multi start-up\n");
        printf("| 6   : sys config\n");
        printf("| 7   : icum ecdh setting\n");
        printf("| 8   : icum ecdh test\n");
        printf("| 9   : ecdsa sign verify test\n");
        printf("| 10   : ecdsa sign verify test\n");
        printf("| q   : Quit\n\n");
        printf("    ?> ");
        vrun_console_get();

        if(*(char *)vrun_iobuff_readparam(0) == 'q')
        {
            break;
        }
        else
        {
            sscanf(vrun_iobuff_readparam(0), "%i", &n);
        }

        switch(n)
        {
            case 0: ecdsa_sign_nml();          break;
            case 1: ecdsa_sign_dbg();          break;
            case 2: ecdsa_sign_cluster();      break;
            case 3: ecdsa_sign_curve();        break;
            case 4: ecdsa_sign_mem();          break;
            case 5: ecdsa_sign_multi();        break;
            case 6: ecdsa_sign_syscfg();       break;
            case 7: icum_ecdh_setting();       break;
            case 8: icum_ecdh_test();          break;
            case 9: ecdsa_sign_verify_test();          break;
            case 10: eddsa_sign_verify_test();          break;
            default:    break;
        }
    }
}

