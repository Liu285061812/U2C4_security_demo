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

// simple test vectors
// source: https://github.com/pyca/cryptography/blob/master/vectors/cryptography_vectors/asymmetric/RSA/pkcs-1v2-1d2-vec/pss-vect.txt

static const uint8_t *strE10001 =(void *)
    "010001";

#define STATIC static
#include "pe1_rsa_test_msg_PSS_11.h"
// from "message.c"

    /* test_salt_PSS_11 [] is described only to determine the SALT length.*/
    /* Use the definition SALT_BYTES instead of sizeof (test_salt_PSS_11) */
    #define SALT_BYTES  20

// ********************************************************************************
// Testing RSA Signature Verification  (PSS)
// ********************************************************************************
void icum_rsa_verif_pss(void)
{
    r_icumif_isd_t *p_ISD;
    int i;

    // pause life signal management
    pause_life_signal();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nTesting RSA(PSS) signature verification...\n\n");

    p_ISD->service_id     = SERVICE_01_RSA_VERIFY;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.RSA_VERIFY.key_group               = KEY_GRP_RSA_PUB;
    p_ISD->prm.RSA_VERIFY.key_id                  = 1;
    p_ISD->prm.RSA_VERIFY.p_source               = (void *)test_msg_PSS_11;
    p_ISD->prm.RSA_VERIFY.source_size_in_bytes   = sizeof(test_msg_PSS_11);
    p_ISD->prm.RSA_VERIFY.hash_primitive          = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.RSA_VERIFY.salt_size_in_bytes      = 20;
    p_ISD->prm.RSA_VERIFY.hash_encoding_format    = ENCODING_FORMAT_EMSA_PSS;
    p_ISD->prm.RSA_VERIFY.job_slice_hash          = job_slice[JOB_H];
    p_ISD->prm.RSA_VERIFY.job_slice_mod_exp       = job_slice[JOB_RSA];
    p_ISD->prm.RSA_VERIFY.verification_result     = VERIFICATION_FAIL;

    printf("Key group: KEY_GRP_RSA_PUB\n");
    printf("Key ID   : %d\n\n", p_ISD->prm.RSA_VERIFY.key_id);

    printf("Job slice sha: %d block(s)\n", p_ISD->prm.RSA_VERIFY.job_slice_hash);
    printf("Job slice rsa: %d block(s)\n", p_ISD->prm.RSA_VERIFY.job_slice_mod_exp);

    for(i = 0; i < 2; i++)
    {
        switch(i)
        {
            case 0:
                printf("Using string formats...\n");
                
                p_ISD->prm.RSA_VERIFY.p_signature              = (void *)test_sig_PSS_11;
                p_ISD->prm.RSA_VERIFY.signature_size_in_bits   = (strlen((char *)test_sig_PSS_11) / 2) * 8;
                p_ISD->prm.RSA_VERIFY.signature_format         = FORMAT_STRING_HEXA;

                break;

            case 1:
                printf("Using plain binary formats...\n");

                p_ISD->prm.RSA_VERIFY.p_signature              = (void *)test_sig_PSS_11_plain;
                p_ISD->prm.RSA_VERIFY.signature_size_in_bits   = sizeof(test_sig_PSS_11_plain) * 8;
                p_ISD->prm.RSA_VERIFY.signature_format         = FORMAT_BINARY_PLAIN;

                break;

             default:
                break;
        }
    
        // trigger the security service
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        
        // fetch execution time in vrun_chrono_xxx variables
        vrun_chrono_gettimestamp();
        
        printf("Processed in : "); PCHRONO(); NL(); FLUSH();
        printf("Verification result : ");
        if(p_ISD->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
        {
            printf("PASS!\n\n");
        }
        else    
        {
            printf("*FAIL*!\a\n\n");
        }
    }
    
    // restart life signal
    restart_life_signal();

    printf("Done!\n");
}


// ********************************************************************************
// Testing RSA 2048 bit key
// ********************************************************************************

static const uint8_t t2048_msg[]={
    0x88, 0x31, 0x77, 0xe5, 0x12, 0x6b, 0x9b, 0xe2, 0xd9, 0xa9, 0x68, 0x03, 0x27, 0xd5, 0x37, 0x0c, 
    0x6f, 0x26, 0x86, 0x1f, 0x58, 0x20, 0xc4, 0x3d, 0xa6, 0x7a, 0x3a, 0xd6, 0x09};

static const uint8_t t2048_sig_plain[]= {
    0x82, 0xc2, 0xb1, 0x60, 0x09, 0x3b, 0x8a, 0xa3, 0xc0, 0xf7, 0x52, 0x2b, 0x19, 0xf8, 0x73, 0x54,
    0x06, 0x6c, 0x77, 0x84, 0x7a, 0xbf, 0x2a, 0x9f, 0xce, 0x54, 0x2d, 0x0e, 0x84, 0xe9, 0x20, 0xc5,
    0xaf, 0xb4, 0x9f, 0xfd, 0xfd, 0xac, 0xe1, 0x65, 0x60, 0xee, 0x94, 0xa1, 0x36, 0x96, 0x01, 0x14,
    0x8e, 0xba, 0xd7, 0xa0, 0xe1, 0x51, 0xcf, 0x16, 0x33, 0x17, 0x91, 0xa5, 0x72, 0x7d, 0x05, 0xf2,
    0x1e, 0x74, 0xe7, 0xeb, 0x81, 0x14, 0x40, 0x20, 0x69, 0x35, 0xd7, 0x44, 0x76, 0x5a, 0x15, 0xe7,
    0x9f, 0x01, 0x5c, 0xb6, 0x6c, 0x53, 0x2c, 0x87, 0xa6, 0xa0, 0x59, 0x61, 0xc8, 0xbf, 0xad, 0x74,
    0x1a, 0x9a, 0x66, 0x57, 0x02, 0x28, 0x94, 0x39, 0x3e, 0x72, 0x23, 0x73, 0x97, 0x96, 0xc0, 0x2a,
    0x77, 0x45, 0x5d, 0x0f, 0x55, 0x5b, 0x0e, 0xc0, 0x1d, 0xdf, 0x25, 0x9b, 0x62, 0x07, 0xfd, 0x0f,
    0xd5, 0x76, 0x14, 0xce, 0xf1, 0xa5, 0x57, 0x3b, 0xaa, 0xff, 0x4e, 0xc0, 0x00, 0x69, 0x95, 0x16,
    0x59, 0xb8, 0x5f, 0x24, 0x30, 0x0a, 0x25, 0x16, 0x0c, 0xa8, 0x52, 0x2d, 0xc6, 0xe6, 0x72, 0x7e,
    0x57, 0xd0, 0x19, 0xd7, 0xe6, 0x36, 0x29, 0xb8, 0xfe, 0x5e, 0x89, 0xe2, 0x5c, 0xc1, 0x5b, 0xeb,
    0x3a, 0x64, 0x75, 0x77, 0x55, 0x92, 0x99, 0x28, 0x0b, 0x9b, 0x28, 0xf7, 0x9b, 0x04, 0x09, 0x00,
    0x0b, 0xe2, 0x5b, 0xbd, 0x96, 0x40, 0x8b, 0xa3, 0xb4, 0x3c, 0xc4, 0x86, 0x18, 0x4d, 0xd1, 0xc8,
    0xe6, 0x25, 0x53, 0xfa, 0x1a, 0xf4, 0x04, 0x0f, 0x60, 0x66, 0x3d, 0xe7, 0xf5, 0xe4, 0x9c, 0x04,
    0x38, 0x8e, 0x25, 0x7f, 0x1c, 0xe8, 0x9c, 0x95, 0xda, 0xb4, 0x8a, 0x31, 0x5d, 0x9b, 0x66, 0xb1,
    0xb7, 0x62, 0x82, 0x33, 0x87, 0x6f, 0xf2, 0x38, 0x52, 0x30, 0xd0, 0x70, 0xd0, 0x7e, 0x16, 0x66
    };

static const uint8_t *t2048_key=(void *)
    "a5dd867ac4cb02f90b9457d48c14a770"
    "ef991c56c39c0ec65fd11afa8937cea5"
    "7b9be7ac73b45c0017615b82d622e318"
    "753b6027c0fd157be12f8090fee2a7ad"
    "cd0eef759f88ba4997c7a42d58c9aa12"
    "cb99ae001fe521c13bb5431445a8d5ae"
    "4f5e4c7e948ac227d3604071f20e577e"
    "905fbeb15dfaf06d1de5ae6253d63a6a"
    "2120b31a5da5dabc9550600e20f27d37"
    "39e2627925fea3cc509f21dff04e6eea"
    "4549c540d6809ff9307eede91fff5873"
    "3d8385a237d6d3705a33e39190099207"
    "0df7adf1357cf7e3700ce3667de83f17"
    "b8df1778db381dce09cb4ad058a51100"
    "1a738198ee27cf55a13b754539906582"
    "ec8b174bd58d5d1f3d767c613721ae05";

void icum_rsa_vefify_pss_2048(void)
{
    r_icumif_isd_t *p_ISD;

    pause_life_signal();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nImporting 2048-bit RSA public key #0...\n");

    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_RSA_PUB;
    p_ISD->prm.PK_IMPORT.key_id                   = 0;
    p_ISD->prm.PK_IMPORT.p_pub_exp                = strE10001;
    p_ISD->prm.PK_IMPORT.p_modulus                = t2048_key;
    p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = 2048;
    p_ISD->prm.PK_IMPORT.pub_exp_size_in_bits     = 17;
    p_ISD->prm.PK_IMPORT.priv_exp_size_in_bits    = 0;
    p_ISD->prm.PK_IMPORT.key_format               = FORMAT_STRING_HEXA;
    p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
    p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;
    
    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result != SERV_OK) goto end;

    printf("\nVerify signature over test message...\n\n");

    p_ISD->service_id     = SERVICE_01_RSA_VERIFY;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.RSA_VERIFY.key_group               = KEY_GRP_RSA_PUB;
    p_ISD->prm.RSA_VERIFY.key_id                  = 0;
    p_ISD->prm.RSA_VERIFY.p_source               = (void *)t2048_msg;
    p_ISD->prm.RSA_VERIFY.source_size_in_bytes   = sizeof(t2048_msg);
    p_ISD->prm.RSA_VERIFY.hash_primitive          = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.RSA_VERIFY.salt_size_in_bytes      = 20;
    p_ISD->prm.RSA_VERIFY.hash_encoding_format    = ENCODING_FORMAT_EMSA_PSS;
    p_ISD->prm.RSA_VERIFY.job_slice_hash          = job_slice[JOB_H];
    p_ISD->prm.RSA_VERIFY.job_slice_mod_exp       = job_slice[JOB_RSA];
    p_ISD->prm.RSA_VERIFY.p_signature             = (void *)t2048_sig_plain;
    p_ISD->prm.RSA_VERIFY.signature_size_in_bits  = sizeof(t2048_sig_plain) * 8;
    p_ISD->prm.RSA_VERIFY.signature_format        = FORMAT_BINARY_PLAIN;

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
        
    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();
        
    if(p_ISD->service_result != SERV_OK) goto end;
    
    printf("Verification result : ");
    if(p_ISD->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
    {
        printf("PASS!\n\n");
    }
    else    
    {
        printf("*FAIL*!\a\n\n");
    }

end:
    // restart life signal
    restart_life_signal();

    printf("\nDone!\n");
}

// ********************************************************************************
// Testing RSA 3072 bit key
// ********************************************************************************

static const uint8_t t3072_sig_plain[] = {
#if !defined(CAVP_TEST)
    0x58, 0x3b, 0x85, 0x92, 0x4d, 0x78, 0x8c, 0x8f, 0xaf, 0xff, 0xff, 0x81, 0x94, 0xc7, 0xe7, 0x03,
    0x43, 0x14, 0xa8, 0x03, 0xaf, 0xa8, 0xdd, 0x97, 0xf8, 0xf9, 0x7d, 0x25, 0x8f, 0xec, 0x52, 0x94,
    0x67, 0x31, 0x12, 0xbf, 0x04, 0xb6, 0x88, 0xe4, 0x9f, 0x06, 0xcd, 0x03, 0xa6, 0x0e, 0x8c, 0x5a,
    0x0b, 0x01, 0xba, 0xd1, 0x99, 0xc0, 0x86, 0xf4, 0x1b, 0x43, 0xcb, 0xfa, 0x5f, 0xe7, 0x64, 0x19,
    0x59, 0xab, 0x34, 0x3c, 0xd5, 0x5b, 0x06, 0xd4, 0x53, 0x1f, 0xce, 0xb2, 0x25, 0x22, 0x06, 0x08,
    0xc2, 0x6e, 0x3c, 0x5e, 0x31, 0xee, 0x30, 0x12, 0x9d, 0x1b, 0x1d, 0x00, 0xb7, 0x09, 0x24, 0x9a,
    0x47, 0x4f, 0xb6, 0x13, 0x92, 0xda, 0x47, 0x9b, 0x65, 0xd2, 0x27, 0xb3, 0xf4, 0x3f, 0x33, 0xcd,
    0x90, 0x8e, 0xd6, 0xae, 0x08, 0x0f, 0xd6, 0x08, 0x06, 0x7c, 0x57, 0x59, 0x0b, 0x8f, 0x55, 0x4b,
    0x4f, 0xe4, 0x50, 0x95, 0x74, 0x77, 0xad, 0x5b, 0xc7, 0x88, 0x04, 0xe4, 0x32, 0x60, 0x9f, 0x16,
    0x9c, 0x53, 0xc3, 0x35, 0x29, 0x99, 0x1c, 0x3e, 0xf6, 0x08, 0x9e, 0x55, 0x7d, 0xc1, 0xee, 0xe6,
    0x04, 0x23, 0x58, 0x10, 0x0b, 0x80, 0x2c, 0x88, 0x3f, 0xdb, 0x42, 0xd6, 0x35, 0x85, 0x71, 0x37,
    0x97, 0x69, 0xe3, 0x3e, 0x2d, 0x95, 0x90, 0x29, 0xf0, 0xb5, 0x93, 0x90, 0xbd, 0x6a, 0x48, 0x0d,
    0x79, 0x67, 0xfc, 0xd7, 0xdf, 0xcd, 0xba, 0x4f, 0xe9, 0x32, 0x36, 0xc8, 0xb3, 0x6e, 0x2e, 0x3f,
    0x31, 0xa6, 0x01, 0x47, 0x10, 0x65, 0x27, 0xfe, 0xa3, 0x67, 0x39, 0xf4, 0xe9, 0x8e, 0xf1, 0x13,
    0xb3, 0x0b, 0xca, 0xa8, 0x9c, 0x80, 0xde, 0x01, 0x7f, 0xdc, 0x8b, 0x70, 0x7c, 0xe7, 0x72, 0x6c,
    0xa8, 0xd6, 0xca, 0x43, 0xfa, 0xe5, 0xc0, 0xc9, 0x54, 0x14, 0x01, 0xf6, 0xa9, 0x4f, 0xe9, 0x0e,
    0x5f, 0x09, 0x91, 0x32, 0x3c, 0xac, 0xd3, 0x67, 0x86, 0xd6, 0xa5, 0x8f, 0x55, 0xbc, 0x8a, 0x1d,
    0xc6, 0xcd, 0x97, 0xce, 0x74, 0xf0, 0x45, 0xd6, 0x70, 0x55, 0x1a, 0xb9, 0x3a, 0xc9, 0x09, 0x8f,
    0x16, 0xd8, 0x0e, 0x41, 0xaa, 0xdc, 0x06, 0x56, 0x70, 0xb1, 0x20, 0xed, 0xb3, 0xe2, 0x9b, 0xee,
    0xe3, 0x44, 0xde, 0xc8, 0xd3, 0x85, 0x07, 0x0c, 0x6d, 0xec, 0xd0, 0x59, 0xd3, 0x57, 0x7f, 0xae,
    0x8f, 0xcb, 0x09, 0xa8, 0xc5, 0x7a, 0xda, 0x50, 0xa0, 0x29, 0x17, 0x4c, 0xbd, 0x54, 0xbe, 0x96,
    0xb4, 0xd1, 0xcc, 0x0f, 0xd6, 0xdb, 0x5f, 0x5f, 0xa6, 0x81, 0xf6, 0x72, 0xc8, 0xf6, 0xb7, 0x3c,
    0x47, 0xb6, 0xd7, 0xd2, 0x66, 0x4c, 0xc1, 0xc5, 0x05, 0x6f, 0x35, 0x2c, 0xe5, 0x9a, 0x92, 0xfa,
    0x39, 0xad, 0x3e, 0x0d, 0x54, 0x49, 0x51, 0xd0, 0x7a, 0x89, 0x2d, 0x60, 0x2e, 0x8d, 0xd7, 0xb9
#else
    0x86, 0xe0, 0x72, 0x83, 0x8e, 0xcd, 0xf9, 0x78, 0x05, 0x76, 0x1f, 0x42, 0x5c, 0xf1, 0xac, 0x7b,
    0xe3, 0xd1, 0x72, 0xff, 0xd5, 0x16, 0x6a, 0xfb, 0x31, 0x9f, 0x0f, 0xc6, 0xb3, 0x8a, 0xbc, 0x08,
    0xd9, 0x21, 0x92, 0xc0, 0x6f, 0xed, 0x5a, 0x44, 0x92, 0x7d, 0xff, 0x52, 0xea, 0xa5, 0xf3, 0xfd,
    0x93, 0x5c, 0x69, 0xcc, 0x87, 0xb3, 0xe9, 0x3e, 0x20, 0xce, 0xa0, 0x10, 0x3e, 0x76, 0xc8, 0x1e,
    0x34, 0x4c, 0x46, 0xe2, 0xb6, 0x3d, 0x86, 0x08, 0x8d, 0x14, 0x2d, 0x69, 0xbb, 0xe7, 0x0e, 0xa8,
    0x9a, 0xf0, 0xad, 0xa5, 0x81, 0x25, 0x53, 0xb0, 0xb1, 0xcc, 0x28, 0x83, 0x78, 0x7d, 0x16, 0x70,
    0x33, 0xb0, 0x79, 0xb1, 0x44, 0x53, 0xe5, 0xae, 0x4b, 0xbd, 0x34, 0x55, 0xcc, 0x6c, 0xe5, 0x40,
    0xc8, 0xa7, 0xdf, 0x03, 0x17, 0x3b, 0x7e, 0x62, 0x53, 0x6c, 0xb5, 0x8b, 0xe9, 0x8b, 0xd2, 0x98,
    0x23, 0xae, 0x02, 0x56, 0x83, 0x72, 0xc1, 0x26, 0xdf, 0xf3, 0xbe, 0x14, 0x12, 0xec, 0x86, 0xbd,
    0x73, 0x33, 0x48, 0xba, 0xf3, 0x3b, 0x5a, 0x2a, 0x02, 0x77, 0x5c, 0x41, 0x68, 0x34, 0x30, 0x07,
    0xb0, 0xa6, 0x77, 0x48, 0xcd, 0x75, 0x3f, 0x77, 0xf2, 0x42, 0xea, 0x1e, 0xb4, 0x8e, 0x69, 0x9b,
    0x36, 0xd2, 0xbb, 0xfe, 0x7e, 0x3c, 0x86, 0x8a, 0xa4, 0xe0, 0x57, 0x0d, 0x0d, 0x6b, 0x6b, 0x84,
    0xd9, 0x8a, 0xfc, 0xde, 0xba, 0xe9, 0x82, 0x58, 0x66, 0x29, 0x07, 0xf1, 0x73, 0xbc, 0x2a, 0x4f,
    0x72, 0xf1, 0x77, 0xe7, 0xeb, 0x65, 0x1c, 0x9f, 0x52, 0x9a, 0x71, 0xe5, 0x15, 0x77, 0x0e, 0xd6,
    0x70, 0x3d, 0xaf, 0x75, 0x01, 0x0d, 0xe4, 0x81, 0xe3, 0x6a, 0x0e, 0xbc, 0x4d, 0x03, 0xfa, 0x7d,
    0xce, 0xd6, 0x8c, 0x54, 0xa8, 0x32, 0x7f, 0xfa, 0x3c, 0x29, 0xe0, 0xbf, 0xe8, 0xdf, 0x80, 0xba,
    0x30, 0x6e, 0x25, 0xdc, 0x67, 0x35, 0x34, 0xbe, 0x71, 0x31, 0x7e, 0x5d, 0xd9, 0xfe, 0xea, 0xd2,
    0x8b, 0x07, 0x14, 0xb9, 0x77, 0x9c, 0x6b, 0xef, 0xa2, 0x33, 0x95, 0xa4, 0xa5, 0x9d, 0x10, 0xbc,
    0x23, 0x73, 0x96, 0xb6, 0xeb, 0x71, 0x21, 0x9a, 0x38, 0x0d, 0x3e, 0x25, 0x21, 0x3b, 0x31, 0xdd,
    0xb3, 0xa9, 0x1b, 0xf3, 0x5c, 0x59, 0x5e, 0x3e, 0x3d, 0xc3, 0xe9, 0x78, 0x7a, 0x96, 0x35, 0x3e,
    0x70, 0xbf, 0x6f, 0x53, 0xec, 0x4d, 0x0a, 0x55, 0xb8, 0x8a, 0xff, 0x44, 0x56, 0xac, 0xe9, 0xf2,
    0xb6, 0xb3, 0x1d, 0x31, 0xcf, 0x73, 0x67, 0xb3, 0x50, 0xd0, 0x04, 0x64, 0x68, 0x9e, 0x56, 0xb5,
    0xef, 0x2d, 0xea, 0x35, 0x8d, 0xb8, 0xc1, 0x70, 0x91, 0x13, 0x2f, 0xa3, 0xb9, 0x1c, 0xce, 0xca,
    0xf1, 0xd0, 0xa2, 0xe0, 0x24, 0xd7, 0x82, 0x55, 0x40, 0xd1, 0xe5, 0xa8, 0x41, 0xec, 0xac, 0xc0
#endif
};

static const uint8_t *t3072_key=(void *)
#if !defined(CAVP_TEST)
    "ba168fad96a9f2d6e32e01f44dff"
    "2e245cfe372aa751dc77ffa437d4e8"
    "ef68b63f985d3286ebd6bde8b01fa0"
    "e928ca316f989255bec6586fc81a8c"
    "f76210620d865796d9ff92bba56b25"
    "9d0918b88601fe86a122464da16709"
    "875dda7fe4c1f0c111af60564b4539"
    "2240ac24afa778e3e7fffeb945f07c"
    "5668c017ec073e7ce84e9b6c7efa18"
    "9048c422ed310f6de948d474ad6f43"
    "a92e70bcd6e310e8c576cae9f6dd3f"
    "97b37816580ea6977014e1c1f9b705"
    "eb3278666b5d71900c26f1e26dd0f9"
    "dffd4100d84b0d3ed00809edefdc40"
    "f8f11e5acba248656440200e80bed5"
    "ea244956a764f9760723743641e14b"
    "b3d93edc69e0e21d492ce3ac59ed2e"
    "cdda72c940c58168ee22cbcb399c73"
    "08a14ec6d9d15d7ceb14b031bca1c6"
    "0a34560133581233716a8755a0c9ac"
    "e8a24fa41c1da41ad381e4ccda88a8"
    "6d0af9a991049761d9d890168e75a8"
    "294c6a1a923adab5ab5ed087ade7e9"
    "d3ccd6bfa75dc1a7468eefbab60b02"
    "c141e059825942034fa16679d04376"
    "6904422272cae145b47f";
#else
    "a5f3da0aaf54b45f99a5d7085f213c37"
    "21cbe7e83b3e6c3fe0f5a84c7e387ba5"
    "13392c28a9010d3b618c03847e6b11bb"
    "bbe4d5e47fc97ea696250699e96ecd91"
    "1404f7b806957038a68bb59a520f2d90"
    "182d183e035204a914e6ac03c2bc6d3f"
    "9d7856b25f9041b56df310de3feb30aa"
    "468a0668a1e5da9cdb185956caa5d75e"
    "1cdcac2db823173495619105367231b7"
    "f2de7528a8a79ec9fdbbab601178a204"
    "a5aa4e19759eb16ea4bab87bf48bb179"
    "0f9fc6eb4d5674d3fbc11b922558d4e5"
    "68e454b26a7178f3e147beb0c8ca6ecf"
    "f5e52af248ac07d6a189393e17232adf"
    "f2f7423f56b94b9a7d61fde23a9558ac"
    "7a3bc7c06748a5da11759f92baf4e386"
    "bb0212565b5beecf31d063cfab71af89"
    "6b3d734750d9bca07343bfb3c2864522"
    "6e9dad3070fc247c71c078e974934941"
    "000a79d01abab14d21f5e608c4e4d13d"
    "eec1aef298e1247c50b47bfee6162f35"
    "2f41cdba8628d1d628848c876cfb102d"
    "acce7fa160c04d3aabc8667a142a710b"
    "7f495fd350c4862a653d15c33d9266fd";
#endif

void icum_rsa_verify_pss_3072(void)
{
    r_icumif_isd_t *p_ISD;

    pause_life_signal();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nImporting 3072-bit RSA public key #0...\n");

    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_RSA_PUB;
    p_ISD->prm.PK_IMPORT.key_id                   = 0;
#if !defined(CAVP_TEST)
    p_ISD->prm.PK_IMPORT.p_pub_exp                = strE10001;
    p_ISD->prm.PK_IMPORT.pub_exp_size_in_bits     = 17;
#else
    p_ISD->prm.PK_IMPORT.p_pub_exp                = SMALL_E;
    p_ISD->prm.PK_IMPORT.pub_exp_size_in_bits     = 5;
#endif
    p_ISD->prm.PK_IMPORT.p_modulus                = t3072_key;
    p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = 3072;
    p_ISD->prm.PK_IMPORT.priv_exp_size_in_bits    = 0;
    p_ISD->prm.PK_IMPORT.key_format               = FORMAT_STRING_HEXA;
    p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
    p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;
    
    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result != SERV_OK) goto end;

    printf("\nHash the test message...\n\n");

    p_ISD->service_id                     = SERVICE_01_HASH;
    p_ISD->p_callbackfunc                 = 0;
    p_ISD->job_id                         = 0;
#if !defined(CAVP_TEST)
    p_ISD->prm.HASH.p_message             = test_msg_PSS_11;
    p_ISD->prm.HASH.message_size_in_bytes = sizeof(test_msg_PSS_11);
#else
    p_ISD->prm.HASH.p_message             = t3072_msg;
    p_ISD->prm.HASH.message_size_in_bytes = sizeof(t3072_msg);
#endif
    p_ISD->prm.HASH.hash_primitive        = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.HASH.job_slice             = job_slice[JOB_H];
    p_ISD->prm.HASH.p_hash                = BIG_BUFFER[0];

    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result != SERV_OK) goto end;

    printf("hash size:       %d bytes\n", p_ISD->prm.HASH.hash_size_in_bytes);
    printf("hash value:     ");
    PMEM(p_ISD->prm.HASH.p_hash, p_ISD->prm.HASH.hash_size_in_bytes);

    printf("\nVerify signature over test message...\n\n");

    p_ISD->service_id     = SERVICE_01_RSA_VERIFY;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.RSA_VERIFY.key_group               = KEY_GRP_RSA_PUB;
    p_ISD->prm.RSA_VERIFY.key_id                  = 0;
#if !defined(CAVP_TEST)
    p_ISD->prm.RSA_VERIFY.p_source               = (void *)test_msg_PSS_11;
    p_ISD->prm.RSA_VERIFY.source_size_in_bytes   = sizeof(test_msg_PSS_11);
#else
    p_ISD->prm.RSA_VERIFY.p_source               = (void *)t3072_msg;
    p_ISD->prm.RSA_VERIFY.source_size_in_bytes   = sizeof(t3072_msg);
#endif
    p_ISD->prm.RSA_VERIFY.hash_primitive          = HASH_PRIMITIVE_SHA1;
#if !defined(CAVP_TEST)
    p_ISD->prm.RSA_VERIFY.salt_size_in_bytes      = 20;
#else
    p_ISD->prm.RSA_VERIFY.salt_size_in_bytes      = 10;
#endif
    p_ISD->prm.RSA_VERIFY.hash_encoding_format    = ENCODING_FORMAT_EMSA_PSS;
    p_ISD->prm.RSA_VERIFY.job_slice_hash          = job_slice[JOB_H];
    p_ISD->prm.RSA_VERIFY.job_slice_mod_exp       = job_slice[JOB_RSA];
    p_ISD->prm.RSA_VERIFY.p_signature             = (void *)t3072_sig_plain;
    p_ISD->prm.RSA_VERIFY.signature_size_in_bits  = sizeof(t3072_sig_plain) * 8;
    p_ISD->prm.RSA_VERIFY.signature_format        = FORMAT_BINARY_PLAIN;

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
        
    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();
        
    if(p_ISD->service_result != SERV_OK) goto end;

    printf("Verification result : ");
    if(p_ISD->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
    {
        printf("PASS!\n\n");
    }
    else    
    {
        printf("*FAIL*!\a\n\n");
    }

end:
    // restart life signal
    restart_life_signal();

    printf("\nDone!\n");
}

// ********************************************************************************
// Testing RSA 2048 bit key with SHA2-256
// ********************************************************************************

static const uint8_t t2048_msg_2[]={
    0x35, 0xa3, 0x79, 0x46, 0xe5, 0x26, 0x78, 0xee, 0x37, 0x8f, 0x5f, 0x17, 0x68, 0x38, 0xef, 0x08,
    0xf3, 0xc2, 0x13, 0x92, 0xb1, 0xad, 0x20, 0x46, 0x45, 0x25, 0x5b, 0xe5, 0xb7, 0x1f, 0xbc, 0x18,
    0x5f, 0xa5, 0xf1, 0x61, 0x05, 0x6e, 0xa6, 0x52, 0x46, 0xb2, 0x04, 0xfd, 0x39, 0x3c, 0x77, 0xab,
    0x53, 0xc1, 0xb5, 0xd1, 0x88, 0x70, 0xfc, 0x3f, 0xb3, 0xca, 0x9a, 0x9b, 0x38, 0xb4, 0xb3, 0x0e,
    0xe8, 0xcb, 0x3f, 0x3d, 0x25, 0xf7, 0x52, 0x7b, 0x46, 0x43, 0xa0, 0x3c, 0x3d, 0xec, 0x40, 0xcd,
    0x76, 0xb7, 0xb0, 0x43, 0x03, 0x88, 0x1a, 0xb2, 0xf7, 0x31, 0xd5, 0x9f, 0x0f, 0x88, 0x2f, 0xb7,
    0x98, 0xbc, 0x6a, 0xc1, 0x8c, 0xe9, 0x04, 0xd1, 0xff, 0xe9, 0x3c, 0xbe, 0xb9, 0x6e, 0xd1, 0xd7,
    0x25, 0x4d, 0x0d, 0xd2, 0x6a, 0x1d, 0x02, 0x05, 0xd7, 0x01, 0x14, 0xd9, 0x84, 0xc2, 0xb7, 0x7b,
};

static const uint8_t *t2048_sig_2=(void *)
    "56279ccd2d37e8113625732cd3f3b61b"
    "4ef9325160c7f6af7077c25049d32742"
    "607ae3f845bd66cd6752813c26067fdc"
    "23f08008cf6a531124e9ebc9264f7cfd"
    "6d6eeff15daf97dad22565ec36b69125"
    "e7b27fc93892f6ff42ce8f265dc2cf2e"
    "5758ba0d67968e800e73fd47131008f5"
    "adc863919ea0cc153cf7efff134b0bcb"
    "d0af5505ab49af7b75d63a8aa7976b8f"
    "e77baf5a699a7e38a60eb7ca64e834f3"
    "e0f89da5b6a343a01f7657fd842c091a"
    "6208503bc75de8f95de0d871a6fc114b"
    "594ff99d615825fd3b89693338145253"
    "6d68d9e034f65abf3412e8e32002689e"
    "102a8bb69991e04a7ff681b62e48ee68"
    "7badf8690b2ccee4bf245cd0a25dcf21";

static const uint8_t *t2048_key_2=(void *)
    "c6e0ed537a2d85cf1c4effad6419884d"
    "824ceabf5200e755691cb7328acd6a75"
    "5fe85798502ccaec9e55d47afd0cf325"
    "8ebe920b50c5fd9d72897462bd0e459b"
    "bdf902b63d17195b2ef54908980be12a"
    "a7489f8af274b92c0cbc16aed2fa46f7"
    "82d5517b666edfb2e5e5efeaff7e2496"
    "5e26472e51980b0cfe457d297e6aa5da"
    "cb8e728dc6f58130f925a13275c3cace"
    "62f820db1e13cc5274c58ff4d7837671"
    "a1bf5f80d6ad8699c568df8d24dd0f15"
    "2ded36ef4861f59b354bba96a076913a"
    "25facf4722737e6deed95b69a00fb2bc"
    "ed0feeedea4ff01a92605cfe26a6b395"
    "53d0c74e5650eb3779705e135c4b2fa5"
    "18a8d4339c53efab4bb0058238def555";

void icum_rsa_verify_pss_2048_sha512(void)
{
    r_icumif_isd_t *p_ISD;

    pause_life_signal();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nImporting 2048-bit RSA public key #0...\n");

    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_RSA_PUB;
    p_ISD->prm.PK_IMPORT.key_id                   = 0;
    p_ISD->prm.PK_IMPORT.p_pub_exp                = strE10001;
    p_ISD->prm.PK_IMPORT.pub_exp_size_in_bits     = 17;
    p_ISD->prm.PK_IMPORT.p_modulus                = t2048_key_2;
    p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = 2048;
    p_ISD->prm.PK_IMPORT.priv_exp_size_in_bits    = 0;
    p_ISD->prm.PK_IMPORT.key_format               = FORMAT_STRING_HEXA;
    p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
    p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;

    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result != SERV_OK) goto end;

    printf("\nVerify signature over test message...\n\n");

    p_ISD->service_id     = SERVICE_01_RSA_VERIFY;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.RSA_VERIFY.key_group               = KEY_GRP_RSA_PUB;
    p_ISD->prm.RSA_VERIFY.key_id                  = 0;
    p_ISD->prm.RSA_VERIFY.p_source               = (void *)t2048_msg_2;
    p_ISD->prm.RSA_VERIFY.source_size_in_bytes   = sizeof(t2048_msg_2);
    p_ISD->prm.RSA_VERIFY.hash_primitive          = HASH_PRIMITIVE_SHA2_512;
    p_ISD->prm.RSA_VERIFY.salt_size_in_bytes      = 10;
    p_ISD->prm.RSA_VERIFY.hash_encoding_format    = ENCODING_FORMAT_EMSA_PSS;
    p_ISD->prm.RSA_VERIFY.job_slice_hash          = job_slice[JOB_H];
    p_ISD->prm.RSA_VERIFY.job_slice_mod_exp       = job_slice[JOB_RSA];
    p_ISD->prm.RSA_VERIFY.p_signature             = (void *)t2048_sig_2;
    p_ISD->prm.RSA_VERIFY.signature_size_in_bits  = (strlen((char *)t2048_sig_2) / 2) * 8;
    p_ISD->prm.RSA_VERIFY.signature_format        = FORMAT_STRING_HEXA;

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
        
    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();
        
    if(p_ISD->service_result != SERV_OK) goto end;

    printf("Verification result : ");
    if(p_ISD->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
    {
        printf("PASS!\n\n");
    }
    else    
    {
        printf("*FAIL*!\a\n\n");
    }

end:
    // restart life signal
    restart_life_signal();

    printf("\nDone!\n");
}


static const uint8_t test_msg_emsa_pkcsv15[] =
{
    0x85, 0x13, 0x84, 0xcd, 0xfe, 0x81, 0x9c, 0x22, 0xed, 0x6c, 0x4c, 0xcb, 0x30, 0xda, 0xeb, 0x5c,
    0xf0, 0x59, 0xbc, 0x8e, 0x11, 0x66, 0xb7, 0xe3, 0x53, 0x0c, 0x4c, 0x23, 0x3e, 0x2b, 0x5f, 0x8f,
    0x71, 0xa1, 0xcc, 0xa5, 0x82, 0xd4, 0x3e, 0xcc, 0x72, 0xb1, 0xbc, 0xa1, 0x6d, 0xfc, 0x70, 0x13,
    0x22, 0x6b, 0x9e
};
static const uint8_t test_sig_pkcsv15_plain[] =
{
    0x84, 0xfd, 0x2c, 0xe7, 0x34, 0xec, 0x1d, 0xa8, 0x28, 0xd0, 0xf1, 0x5b, 0xf4, 0x9a, 0x87, 0x07,
    0xc1, 0x5d, 0x05, 0x94, 0x81, 0x36, 0xde, 0x53, 0x7a, 0x3d, 0xb4, 0x21, 0x38, 0x41, 0x67, 0xc8,
    0x6f, 0xae, 0x02, 0x25, 0x87, 0xee, 0x9e, 0x13, 0x7d, 0xae, 0xe7, 0x54, 0x73, 0x82, 0x62, 0x93,
    0x2d, 0x27, 0x1c, 0x74, 0x4c, 0x6d, 0x3a, 0x18, 0x9a, 0xd4, 0x31, 0x1b, 0xdb, 0x02, 0x04, 0x92,
    0xe3, 0x22, 0xfb, 0xdd, 0xc4, 0x04, 0x06, 0xea, 0x86, 0x0d, 0x4e, 0x8e, 0xa2, 0xa4, 0x08, 0x4a,
    0xa9, 0x8b, 0x96, 0x22, 0xa4, 0x46, 0x75, 0x6f, 0xdb, 0x74, 0x0d, 0xdb, 0x3d, 0x91, 0xdb, 0x76,
    0x70, 0xe2, 0x11, 0x66, 0x1b, 0xbf, 0x87, 0x09, 0xb1, 0x1c, 0x08, 0xa7, 0x07, 0x71, 0x42, 0x2d,
    0x1a, 0x12, 0xde, 0xf2, 0x9f, 0x06, 0x88, 0xa1, 0x92, 0xae, 0xbd, 0x89, 0xe0, 0xf8, 0x96, 0xf8,
};

static const uint8_t *test_sig_pkcsv15 =(void *)
    "84fd2ce734ec1da828d0f15bf49a8707"
    "c15d05948136de537a3db421384167c8"
    "6fae022587ee9e137daee75473826293"
    "2d271c744c6d3a189ad4311bdb020492"
    "e322fbddc40406ea860d4e8ea2a4084a"
    "a98b9622a446756fdb740ddb3d91db76"
    "70e211661bbf8709b11c08a70771422d"
    "1a12def29f0688a192aebd89e0f896f8";

// ********************************************************************************
// Testing RSA Signature Verification PKCS#1 v1.5
// ********************************************************************************
void icum_rsa_verify_pkcs1_v15(void)
{
    r_icumif_isd_t *p_ISD;
    int i;

    // pause life signal management
    pause_life_signal();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nTesting RSA(PKCS#1 v1.5) signature verification...\n\n");

    p_ISD->service_id     = SERVICE_01_RSA_VERIFY;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.RSA_VERIFY.key_group               = KEY_GRP_RSA_PUB;
    p_ISD->prm.RSA_VERIFY.key_id                  = 1;
    p_ISD->prm.RSA_VERIFY.p_source               = (void *)test_msg_emsa_pkcsv15;
    p_ISD->prm.RSA_VERIFY.source_size_in_bytes   = sizeof(test_msg_emsa_pkcsv15);
    p_ISD->prm.RSA_VERIFY.hash_primitive          = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.RSA_VERIFY.hash_encoding_format    = ENCODING_FORMAT_EMSA_PKCS1_V15;
    p_ISD->prm.RSA_VERIFY.job_slice_hash          = job_slice[JOB_H];
    p_ISD->prm.RSA_VERIFY.job_slice_mod_exp       = job_slice[JOB_RSA];
    p_ISD->prm.RSA_VERIFY.verification_result     = VERIFICATION_FAIL;

    printf("Key group: KEY_GRP_RSA_PUB\n");
    printf("Key ID   : %d\n\n", p_ISD->prm.RSA_VERIFY.key_id);

    printf("Job slice sha: %d block(s)\n", p_ISD->prm.RSA_VERIFY.job_slice_hash);
    printf("Job slice rsa: %d block(s)\n", p_ISD->prm.RSA_VERIFY.job_slice_mod_exp);

    for(i = 0; i < 2; i++)
    {
        switch(i)
        {
            case 0:
                printf("Using string formats...\n");
                
                p_ISD->prm.RSA_VERIFY.p_signature              = (void *)test_sig_pkcsv15;
                p_ISD->prm.RSA_VERIFY.signature_size_in_bits   = (strlen((char *)test_sig_pkcsv15) / 2) * 8;
                p_ISD->prm.RSA_VERIFY.signature_format         = FORMAT_STRING_HEXA;

                break;

            case 1:
                printf("Using plain binary formats...\n");

                p_ISD->prm.RSA_VERIFY.p_signature              = (void *)test_sig_pkcsv15_plain;
                p_ISD->prm.RSA_VERIFY.signature_size_in_bits   = sizeof(test_sig_pkcsv15_plain) * 8;
                p_ISD->prm.RSA_VERIFY.signature_format         = FORMAT_BINARY_PLAIN;

                break;

             default:
                break;
        }
    
        // trigger the security service
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        
        // fetch execution time in vrun_chrono_xxx variables
        vrun_chrono_gettimestamp();
        
        printf("Processed in : "); PCHRONO(); NL(); FLUSH();
        printf("Verification result : ");
        if(p_ISD->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
        {
            printf("PASS!\n\n");
        }
        else    
        {
            printf("*FAIL*!\a\n\n");
        }
    }
    
    // restart life signal
    restart_life_signal();

    printf("Done!\n");
}

#include "rsa_pubkey_for_verify_test.h"

void icum_rsa_verify_test(struct _rsa_verify_testdata *p)
{
    uint8_t *pwork ;
    unsigned char wrk[3] ;
    int i;

    r_icumif_isd_t *p_ISD;

    pause_life_signal();

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\n%s", p->title);
    printf("\nImporting RSA public key #0...\n");

    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_RSA_PUB;
    p_ISD->prm.PK_IMPORT.key_id                   = 0;
    p_ISD->prm.PK_IMPORT.p_modulus                = (void *)p->p_modulus;
    p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = p->modulusBits ? p->modulusBits : (strlen(p->p_modulus) /2) * 8;
    p_ISD->prm.PK_IMPORT.p_pub_exp                = (void *)p->p_pub_e;
    p_ISD->prm.PK_IMPORT.pub_exp_size_in_bits     = p->pub_eBits;
    p_ISD->prm.PK_IMPORT.priv_exp_size_in_bits    = 0;
    p_ISD->prm.PK_IMPORT.key_format               = FORMAT_STRING_HEXA;
    p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
    p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;
    
    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result != SERV_OK) goto end;

    printf("\nVerify signature over test message...\n\n");

    p_ISD->service_id     = SERVICE_01_RSA_VERIFY;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.RSA_VERIFY.key_group               = KEY_GRP_RSA_PUB;
    p_ISD->prm.RSA_VERIFY.key_id                  = 0;
    p_ISD->prm.RSA_VERIFY.p_source               = (void *)p->bin_message;
    p_ISD->prm.RSA_VERIFY.source_size_in_bytes   = p->messageBytes;
    p_ISD->prm.RSA_VERIFY.hash_primitive          = p->hash_primitive;
    p_ISD->prm.RSA_VERIFY.salt_size_in_bytes      = p->salt_size_in_bytes;
    p_ISD->prm.RSA_VERIFY.hash_encoding_format    = p->hash_encoding_format;
    p_ISD->prm.RSA_VERIFY.job_slice_hash          = job_slice[JOB_H];
    p_ISD->prm.RSA_VERIFY.job_slice_mod_exp       = job_slice[JOB_RSA];
    p_ISD->prm.RSA_VERIFY.p_signature             = (void *)p->p_signature;
    p_ISD->prm.RSA_VERIFY.signature_size_in_bits  = p->signatureBits ? p->signatureBits : (strlen((void *)p->p_signature) /2) * 8;
    p_ISD->prm.RSA_VERIFY.signature_format        = FORMAT_STRING_HEXA;

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
        
    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();
        
    if(p_ISD->service_result != SERV_OK) goto end;
    
    printf("Verification result (FORMAT_STRING_HEXA)  : ");
    if(p_ISD->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
    {
        printf("PASS!\n");
    }
    else    
    {
        printf("*FAIL*!\a\n");
    }

    /* FORMAT_BINARY_PLAIN */
    pwork = BIG_BUFFER[0] ;
    for( i=0; i<(strlen((void *)p->p_signature) /2); i++ )
    {
        wrk[0] = p->p_signature[2*i + 0] ;
        wrk[1] = p->p_signature[2*i + 1] ;
        wrk[2] = '\0' ;
        sscanf( (void *)wrk, "%x", &pwork[i] );
    }
    p_ISD->prm.RSA_VERIFY.p_signature             = pwork;
    p_ISD->prm.RSA_VERIFY.signature_format        = FORMAT_BINARY_PLAIN;

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
        
    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();
        
    if(p_ISD->service_result != SERV_OK) goto end;
    
    printf("Verification result (FORMAT_BINARY_PLAIN) : ");
    if(p_ISD->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
    {
        printf("PASS!\n\n");
    }
    else    
    {
        printf("*FAIL*!\a\n\n");
    }

    /* FORMAT_STRING_DECIMAL */
    if(NULL != p->p_signature_dec)
    {
        p_ISD->prm.RSA_VERIFY.p_signature         = (void *)p->p_signature_dec;
        p_ISD->prm.RSA_VERIFY.signature_format    = FORMAT_STRING_DECIMAL;
        
        /* trigger the security service */
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        
        /* fetch execution time in vrun_chrono_xxx variables */
        vrun_chrono_gettimestamp();
        printf("Processed in : "); PCHRONO(); NL(); FLUSH();
        
        if(p_ISD->service_result != SERV_OK) goto end;
        
        printf("Verification result (FORMAT_STRING_DECIMAL) : ");
        if(p_ISD->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
        {
            printf("PASS!\n");
        }
        else
        {
            printf("*FAIL*!\a\n");
        }
    }

    printf("\n");

end:
    // restart life signal
    restart_life_signal();

    printf("\nDone!\n");
}

void icum_rsa_verify_all_pss()
{
    struct _rsa_verify_testdata *pKeyData = (struct _rsa_verify_testdata *)&pss_pubkeys[0] ;

    for( ; pKeyData->p_modulus != NULL ; pKeyData++ )
    {
        icum_rsa_verify_test(pKeyData);
    }
}

void icum_rsa_verify_all_pkcs1_v15()
{
    struct _rsa_verify_testdata *pKeyData = (struct _rsa_verify_testdata *)&pkcs15_pubkeys[0] ;

    for( ; pKeyData->p_modulus != NULL ; pKeyData++ )
    {
        icum_rsa_verify_test(pKeyData);
    }
}

