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

#include "pe1_ecdh.h"



// ********************************************************************************
// Test functions
// ********************************************************************************
void icum_ecdh_setting(void);
void icum_ecdh_test(void);


void icum_ecdh(void)
{
    int n;
    
    while(1)
    {
        printf("\n|serect ecdh test menu\n");
        printf("|---------------------------------------|\n");
        printf("|0          :ecdh_setting\n");
        printf("|1          :ecdh_test\n");
        printf("|q          : Quit\n\n");
        printf("    ?> ");
        
        vrun_console_get();
        if(*(char *)vrun_iobuff_readparam(0) == 'q'){
            break;
        }
        else{
            sscanf(vrun_iobuff_readparam(0), "%i", &n);
        }
        // pause life signal management
        pause_life_signal();
        
        switch(n)
        {
            case 0: icum_ecdh_setting();break;
            case 1: icum_ecdh_test();break;
            default: break;
        }
        // restart life signal
        restart_life_signal();
    }
}

#define IMPORT_ECC_KEY_TEST 0

static const testVectorForPkImport_t  testVector_pkImport_zhxk[] = {
//   test_no,    p_pub_point_x,  p_pub_point_y, p_priv_scalar,  p_signature,   p_info,   key_format,             sig_format,             key_group,       key_id, sig_key_group,     sig_key_id,   sig_algo,                hash_primitive,            curve_id,                   point_x_size_in_bits,      point_y_size_in_bits,       priv_scalar_size_in_bits,   sig_size_in_bits,   info_size_in_bytes,   p_x509_certificate,     x509_certificate_size_in_bytes   disable_on_debug,   write_protected,  test_skip_verify
//ECC_pub
    {1 ,        {point_x_P192 ,  point_y_P192,  0,              (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_RAM, 0,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P192,        sizeof(point_x_P192)  * 8, sizeof(point_y_P192) * 8,   0,                          0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {2 ,        {point_x_P224 ,  point_y_P224,  0,              (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_RAM, 0,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P224,        sizeof(point_x_P224)  * 8, sizeof(point_y_P224) * 8,   0,                          0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {3 ,        {point_x_P256 ,  point_y_P256,  0,              (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_RAM, 0,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P256,        sizeof(point_x_P256)  * 8, sizeof(point_y_P256) * 8,   0,                          0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {4 ,        {point_x_B256 ,  point_y_B256,  0,              (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_RAM, 0,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_BRAINPOOL_P256,   sizeof(point_x_B256)  * 8, sizeof(point_y_B256) * 8,   0,                          0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {5 ,        {point_x_25519,  0           ,  0,              (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_RAM, 0,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_CURVE25519,       sizeof(point_x_25519) * 8, 0                       ,   0,                          0,                  0,                    0,                      0,                               {0,                  0 },           1}},
//ECC_priv
    {6,        {point_x_P192 ,  point_y_P192,  priv_key_P192,  (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_RAM,     1,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P192     ,   sizeof(point_x_P192)  * 8, sizeof(point_y_P192) * 8,   sizeof(priv_key_P192) * 8,  0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {7,        {point_x_P224 ,  point_y_P224,  priv_key_P224,  (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_RAM,     1,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P224     ,   sizeof(point_x_P224)  * 8, sizeof(point_y_P224) * 8,   sizeof(priv_key_P224) * 8,  0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {8,        {point_x_P256 ,  point_y_P256,  priv_key_P256,  (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_RAM,     1,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P256     ,   sizeof(point_x_P256)  * 8, sizeof(point_y_P256) * 8,   sizeof(priv_key_P256) * 8,  0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {9,        {point_x_B256 ,  point_y_B256,  priv_key_B256,  (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_RAM,     1,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_BRAINPOOL_P256,   sizeof(point_x_B256)  * 8, sizeof(point_y_B256) * 8,   sizeof(priv_key_B256) * 8,  0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {10,        {point_x_25519,  0           ,  priv_key_25519, (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_RAM,     1,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_CURVE25519    ,   sizeof(point_x_25519) * 8, 0                       ,   sizeof(priv_key_25519) * 8, 0,                  0,                    0,                      0,                               {0,                  0 },           1}},
};


static const testVectorForEcdh_t testVectorForEcdh_zhxk[] = {
//  test_no,     test_ecc_priv_key_group,   test_ecc_priv_key_id,   test_ecc_pub_key_group,   test_ecc_pub_key_id,   test_shared_key_group,  test_shared_key_id,   test_kdf_algo,   test_hash_primitive,         test_job_slice   test_p_info,          test_counter,   test_info_size_in_bytes,   key_size_in_bytes,   expected_service_result
    {1 ,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_RAM,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_OK},
    {2 ,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_RAM,          1,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_KEY_SLOT_EMPTY},

    {3 ,        {KEY_GRP_ECC_RAM,               1,                      KEY_GRP_ECC_RAM,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF2,   HASH_PRIMITIVE_SHA2_224,     0,               0,                    4,              0,                         16},                 SERV_OK},
    {4 ,        {KEY_GRP_ECC_RAM,               1,                      KEY_GRP_ECC_RAM,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF3,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_OK},
    {5 ,        {KEY_GRP_ECC_RAM,               1,                      KEY_GRP_ECC_RAM,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_PKDF1,  HASH_PRIMITIVE_SHA2_384,     0,               LOREM_IPSUM,          4,              16,                        16},                 SERV_OK},
    {6 ,        {KEY_GRP_ECC_RAM,               1,                      KEY_GRP_ECC_RAM,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_PKDF2,  HASH_PRIMITIVE_SHA2_512,     0,               0,                    4,              0,                         16},                 SERV_OK},
    {7 ,        {KEY_GRP_ECC_RAM,               1,                      KEY_GRP_ECC_RAM,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_OK},
    {8 ,        {KEY_GRP_ECC,               6,                      KEY_GRP_ECC_PUB,          6,                     KEY_GRP_AES_RAM,        0,                    (r_kdf_algo_t)0, HASH_PRIMITIVE_SHA2_224,     0,               0,                    4,              0,                         16},                 SERV_OK},
    {9 ,        {KEY_GRP_ECC,               7,                      KEY_GRP_ECC_PUB,          7,                     KEY_GRP_AES_RAM,        0,                    (r_kdf_algo_t)0, HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_OK},

    {10,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB_EXT,      0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_384,     1,               0,                    4,              0,                         16},                 SERV_OK},

    {11,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_ROM_PUB,      0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_384,     0,               0,                    4,              0,                         16},                 SERV_OK},

    {12,        {KEY_GRP_ECC_RAM,           0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_512,     0,               0,                    4,              0,                         16},                 SERV_OK},
    {13,        {KEY_GRP_ECC_RAM,           0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_OK},
    {14,        {KEY_GRP_ECC_RAM,           0,                      KEY_GRP_ECC_PUB_EXT,      0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_OK},
    {15,        {KEY_GRP_ECC_RAM,           0,                      KEY_GRP_ECC_ROM_PUB,      0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_OK},

    {16,        {KEY_GRP_ECC_RAM,           0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               (uint8_t*)0x009B0000, 4,              16,                        16},                 SERV_SRC_MEM_ERROR},

    //Double activation of service
    {17,        {KEY_GRP_ECC_RAM,           0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_OK},

    {18,        {KEY_GRP_ECC,               8,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_KEY_NOT_USABLE},
    {19,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC,              8,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_KEY_NOT_USABLE},
    {20,        {KEY_GRP_ECC,               9,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_KEY_NOT_USABLE},
    {21,        {KEY_GRP_ECC_RAM,           0,                      KEY_GRP_ECC,              9,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_KEY_NOT_USABLE},

    {22,        {KEY_GRP_ECC,               10,                     KEY_GRP_ECC_PUB,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_INDEX},
    {23,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          10,                    KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_INDEX},
    {24,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_AES_RAM,        6,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_INDEX},

    {25,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_INDEX},

    {26,        {KEY_GRP_AES_ROM,           0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {27,        {KEY_GRP_AES,               0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {28,        {KEY_GRP_AES_RAM,           0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {29,        {KEY_GRP_SHE,               0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {30,        {KEY_GRP_RSA,               0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {31,        {KEY_GRP_RSA_PUB,           0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {32,        {KEY_GRP_RSA_ROM_PUB,       0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {33,        {KEY_GRP_ECC_PUB,           0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {34,        {KEY_GRP_ECC_ROM_PUB,       0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {35,        {KEY_GRP_AES_CR,            0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {36,        {KEY_GRP_HMAC,              0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {37,        {KEY_GRP_TLS_MASTER,        0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {38,        {KEY_GRP_RSA_PUB_EXT,       0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {39,        {KEY_GRP_ECC_PUB_EXT,       0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {40,        {(r_key_group_t)16,         0,                      KEY_GRP_ECC,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},

    {41,        {KEY_GRP_ECC,               0,                      KEY_GRP_AES_ROM,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {42,        {KEY_GRP_ECC,               0,                      KEY_GRP_AES,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {43,        {KEY_GRP_ECC,               0,                      KEY_GRP_AES_RAM,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {44,        {KEY_GRP_ECC,               0,                      KEY_GRP_SHE,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {45,        {KEY_GRP_ECC,               0,                      KEY_GRP_RSA,              0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {46,        {KEY_GRP_ECC,               0,                      KEY_GRP_RSA_PUB,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {47,        {KEY_GRP_ECC,               0,                      KEY_GRP_RSA_ROM_PUB,      0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {48,        {KEY_GRP_ECC,               0,                      KEY_GRP_AES_CR,           0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {49,        {KEY_GRP_ECC,               0,                      KEY_GRP_HMAC,             0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {50,        {KEY_GRP_ECC,               0,                      KEY_GRP_TLS_MASTER,       0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {51,        {KEY_GRP_ECC,               0,                      KEY_GRP_RSA_PUB_EXT,      0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {52,        {KEY_GRP_ECC,               0,                      (r_key_group_t)16,        0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},

    {53,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_AES_ROM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {54,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_AES,            0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {55,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_SHE,            0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {56,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_RSA,            0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {57,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_RSA_PUB,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {58,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_RSA_ROM_PUB,    0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {59,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_ECC,            0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {60,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_ECC_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {61,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_ECC_PUB,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {62,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_ECC_ROM_PUB,    0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {64,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_HMAC,           0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {65,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_TLS_MASTER,     0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {66,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_RSA_PUB_EXT,    0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {67,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_ECC_PUB_EXT,    0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},
    {68,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     (r_key_group_t)16,      0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_KEY_GROUP},

    {69,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_AES_RAM,        0,                    (r_kdf_algo_t)5, HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_PRM_ERROR},
    {70,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   (r_hash_primitive_t)10,      0,               0,                    4,              0,                         16},                 SERV_PRM_ERROR},
    {71,        {KEY_GRP_ECC,               2,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_INVALID_CURVE_ID},
    {72,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_PUB,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF3,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    3,              0,                         16},                 SERV_CRYPT_ERROR}

};

void icum_ecdh_setting()
{
    int i;
    int err_info = 0xFFF;
    r_icumif_service_result_t expected = SERV_OK;
    r_icumif_isd_t *p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    icum_test_prng_init_rng();
    icum_test_prng_extend_seed();

    //ECC Generate ECC RAM key---test ok
#if 0
    for(i= 0; i<5; i++)
    {
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
    
        printf("zhxk: Generate ECC RAM key Test_No . %d\n" , i);
        p_ISD->service_id     = SERVICE_03_ECC_KEY_GENERATE;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;

        p_ISD->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC_RAM;
        p_ISD->prm.ECC_KEY_GENERATE.key_id                  = 0;
        
        p_ISD->prm.ECC_KEY_GENERATE.curve_id                = i;
        p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;
        
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK) goto end;

        //zhxk
        //printf("Generate ECC RAM key  NIST_P192 Done\n");
        printf("zhxk: End Generate ECC RAM key Test_No . %d\n" , i);
    }

    for(i= 7; i<19; i++)
    {
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
    
        printf("zhxk: Generate ECC RAM key Test_No . %d\n" , i);
        p_ISD->service_id     = SERVICE_03_ECC_KEY_GENERATE;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;

        p_ISD->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC_RAM;
        p_ISD->prm.ECC_KEY_GENERATE.key_id                  = 0;
        
        p_ISD->prm.ECC_KEY_GENERATE.curve_id                = i;
        p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;
        
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK) goto end;

        //zhxk
        //printf("Generate ECC RAM key  NIST_P192 Done\n");
        printf("zhxk: End Generate ECC RAM key Test_No . %d\n" , i);
    }
#endif


    //zhxk
    
    printf("zhxk: Import ECC-P192 pub key import\n");
    for(i = 0; i < 1; i++)
    {
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
    
        p_ISD->prm.PK_IMPORT.key_group                = testVector_pkImport_zhxk[i].test_param.key_group;
        p_ISD->prm.PK_IMPORT.key_id                   = testVector_pkImport_zhxk[i].test_param.key_id;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.p_signature              = testVector_pkImport_zhxk[i].test_param.p_signature;
        p_ISD->prm.PK_IMPORT.sig_size_in_bits         = testVector_pkImport_zhxk[i].test_param.sig_size_in_bits;
        p_ISD->prm.PK_IMPORT.curve_id                 = testVector_pkImport_zhxk[i].test_param.curve_id;
        p_ISD->prm.PK_IMPORT.p_priv_scalar            = testVector_pkImport_zhxk[i].test_param.p_priv_scalar;
        p_ISD->prm.PK_IMPORT.p_pub_point_x            = testVector_pkImport_zhxk[i].test_param.p_pub_point_x;
        p_ISD->prm.PK_IMPORT.p_pub_point_y            = testVector_pkImport_zhxk[i].test_param.p_pub_point_y;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_x_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_y_size_in_bits;
        p_ISD->prm.PK_IMPORT.sig_key_group            = testVector_pkImport_zhxk[i].test_param.sig_key_group;
        p_ISD->prm.PK_IMPORT.sig_key_id               = testVector_pkImport_zhxk[i].test_param.sig_key_id;
        p_ISD->prm.PK_IMPORT.key_format               = testVector_pkImport_zhxk[i].test_param.key_format;
        p_ISD->prm.PK_IMPORT.hash_primitive           = testVector_pkImport_zhxk[i].test_param.hash_primitive;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = testVector_pkImport_zhxk[i].test_param.flags.disable_on_debug;
        p_ISD->prm.PK_IMPORT.skip_verify              = testVector_pkImport_zhxk[i].test_param.skip_verify;
        
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            err_info = p_ISD->service_result;
            printf("zhxk: error value is : %d\n", err_info);
        }else
        {
            printf("zhxk: Success Test_No . %d\n", i);
        }
    }

    for(i = 5; i < 6; i++)
    {
        if(i == 5){
            printf("zhxk: Import ECC-P192 private key import\n");
        }
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
    
        p_ISD->prm.PK_IMPORT.key_group                = testVector_pkImport_zhxk[i].test_param.key_group;
        p_ISD->prm.PK_IMPORT.key_id                   = testVector_pkImport_zhxk[i].test_param.key_id;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.p_signature              = testVector_pkImport_zhxk[i].test_param.p_signature;
        p_ISD->prm.PK_IMPORT.sig_size_in_bits         = testVector_pkImport_zhxk[i].test_param.sig_size_in_bits;
        p_ISD->prm.PK_IMPORT.curve_id                 = testVector_pkImport_zhxk[i].test_param.curve_id;
        p_ISD->prm.PK_IMPORT.p_priv_scalar            = testVector_pkImport_zhxk[i].test_param.p_priv_scalar;
        p_ISD->prm.PK_IMPORT.p_pub_point_x            = testVector_pkImport_zhxk[i].test_param.p_pub_point_x;
        p_ISD->prm.PK_IMPORT.p_pub_point_y            = testVector_pkImport_zhxk[i].test_param.p_pub_point_y;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_x_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_y_size_in_bits;
        p_ISD->prm.PK_IMPORT.sig_key_group            = testVector_pkImport_zhxk[i].test_param.sig_key_group;
        p_ISD->prm.PK_IMPORT.sig_key_id               = testVector_pkImport_zhxk[i].test_param.sig_key_id;
        p_ISD->prm.PK_IMPORT.key_format               = testVector_pkImport_zhxk[i].test_param.key_format;
        p_ISD->prm.PK_IMPORT.hash_primitive           = testVector_pkImport_zhxk[i].test_param.hash_primitive;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = testVector_pkImport_zhxk[i].test_param.flags.disable_on_debug;
        p_ISD->prm.PK_IMPORT.skip_verify              = testVector_pkImport_zhxk[i].test_param.skip_verify;
        
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            err_info = p_ISD->service_result;
            printf("zhxk: error value is : %d\n", err_info);
            printf("zhxk: Failed Test_No . %d\n", i);
        }else
        {
            printf("zhxk: Success Test_No . %d\n", i);
        }
    }

    printf("zhxk: Import ECC-P224 pub key import\n");
    for(i = 1; i < 2; i++)
    {
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
    
        p_ISD->prm.PK_IMPORT.key_group                = testVector_pkImport_zhxk[i].test_param.key_group;
        p_ISD->prm.PK_IMPORT.key_id                   = testVector_pkImport_zhxk[i].test_param.key_id;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.p_signature              = testVector_pkImport_zhxk[i].test_param.p_signature;
        p_ISD->prm.PK_IMPORT.sig_size_in_bits         = testVector_pkImport_zhxk[i].test_param.sig_size_in_bits;
        p_ISD->prm.PK_IMPORT.curve_id                 = testVector_pkImport_zhxk[i].test_param.curve_id;
        p_ISD->prm.PK_IMPORT.p_priv_scalar            = testVector_pkImport_zhxk[i].test_param.p_priv_scalar;
        p_ISD->prm.PK_IMPORT.p_pub_point_x            = testVector_pkImport_zhxk[i].test_param.p_pub_point_x;
        p_ISD->prm.PK_IMPORT.p_pub_point_y            = testVector_pkImport_zhxk[i].test_param.p_pub_point_y;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_x_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_y_size_in_bits;
        p_ISD->prm.PK_IMPORT.sig_key_group            = testVector_pkImport_zhxk[i].test_param.sig_key_group;
        p_ISD->prm.PK_IMPORT.sig_key_id               = testVector_pkImport_zhxk[i].test_param.sig_key_id;
        p_ISD->prm.PK_IMPORT.key_format               = testVector_pkImport_zhxk[i].test_param.key_format;
        p_ISD->prm.PK_IMPORT.hash_primitive           = testVector_pkImport_zhxk[i].test_param.hash_primitive;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = testVector_pkImport_zhxk[i].test_param.flags.disable_on_debug;
        p_ISD->prm.PK_IMPORT.skip_verify              = testVector_pkImport_zhxk[i].test_param.skip_verify;
        
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            err_info = p_ISD->service_result;
            printf("zhxk: error value is : %d\n", err_info);
        }else
        {
            printf("zhxk: Success Test_No . %d\n", i);
        }
    }

    for(i = 6; i < 7; i++)
    {
        if(i == 5){
            printf("zhxk: Import ECC-P224 private key import\n");
        }
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
    
        p_ISD->prm.PK_IMPORT.key_group                = testVector_pkImport_zhxk[i].test_param.key_group;
        p_ISD->prm.PK_IMPORT.key_id                   = testVector_pkImport_zhxk[i].test_param.key_id;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.p_signature              = testVector_pkImport_zhxk[i].test_param.p_signature;
        p_ISD->prm.PK_IMPORT.sig_size_in_bits         = testVector_pkImport_zhxk[i].test_param.sig_size_in_bits;
        p_ISD->prm.PK_IMPORT.curve_id                 = testVector_pkImport_zhxk[i].test_param.curve_id;
        p_ISD->prm.PK_IMPORT.p_priv_scalar            = testVector_pkImport_zhxk[i].test_param.p_priv_scalar;
        p_ISD->prm.PK_IMPORT.p_pub_point_x            = testVector_pkImport_zhxk[i].test_param.p_pub_point_x;
        p_ISD->prm.PK_IMPORT.p_pub_point_y            = testVector_pkImport_zhxk[i].test_param.p_pub_point_y;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_x_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_y_size_in_bits;
        p_ISD->prm.PK_IMPORT.sig_key_group            = testVector_pkImport_zhxk[i].test_param.sig_key_group;
        p_ISD->prm.PK_IMPORT.sig_key_id               = testVector_pkImport_zhxk[i].test_param.sig_key_id;
        p_ISD->prm.PK_IMPORT.key_format               = testVector_pkImport_zhxk[i].test_param.key_format;
        p_ISD->prm.PK_IMPORT.hash_primitive           = testVector_pkImport_zhxk[i].test_param.hash_primitive;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = testVector_pkImport_zhxk[i].test_param.flags.disable_on_debug;
        p_ISD->prm.PK_IMPORT.skip_verify              = testVector_pkImport_zhxk[i].test_param.skip_verify;
        
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            err_info = p_ISD->service_result;
            printf("zhxk: error value is : %d\n", err_info);
            printf("zhxk: Failed Test_No . %d\n", i);
        }else
        {
            printf("zhxk: Success Test_No . %d\n", i);
        }
    }


    printf("zhxk: Import ECC-B256 pub key import\n");
    for(i = 3; i < 4; i++)
    {
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
    
        p_ISD->prm.PK_IMPORT.key_group                = testVector_pkImport_zhxk[i].test_param.key_group;
        p_ISD->prm.PK_IMPORT.key_id                   = testVector_pkImport_zhxk[i].test_param.key_id;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.p_signature              = testVector_pkImport_zhxk[i].test_param.p_signature;
        p_ISD->prm.PK_IMPORT.sig_size_in_bits         = testVector_pkImport_zhxk[i].test_param.sig_size_in_bits;
        p_ISD->prm.PK_IMPORT.curve_id                 = testVector_pkImport_zhxk[i].test_param.curve_id;
        p_ISD->prm.PK_IMPORT.p_priv_scalar            = testVector_pkImport_zhxk[i].test_param.p_priv_scalar;
        p_ISD->prm.PK_IMPORT.p_pub_point_x            = testVector_pkImport_zhxk[i].test_param.p_pub_point_x;
        p_ISD->prm.PK_IMPORT.p_pub_point_y            = testVector_pkImport_zhxk[i].test_param.p_pub_point_y;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_x_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_y_size_in_bits;
        p_ISD->prm.PK_IMPORT.sig_key_group            = testVector_pkImport_zhxk[i].test_param.sig_key_group;
        p_ISD->prm.PK_IMPORT.sig_key_id               = testVector_pkImport_zhxk[i].test_param.sig_key_id;
        p_ISD->prm.PK_IMPORT.key_format               = testVector_pkImport_zhxk[i].test_param.key_format;
        p_ISD->prm.PK_IMPORT.hash_primitive           = testVector_pkImport_zhxk[i].test_param.hash_primitive;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = testVector_pkImport_zhxk[i].test_param.flags.disable_on_debug;
        p_ISD->prm.PK_IMPORT.skip_verify              = testVector_pkImport_zhxk[i].test_param.skip_verify;
        
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            err_info = p_ISD->service_result;
            printf("zhxk: error value is : %d\n", err_info);
        }else
        {
            printf("zhxk: Success Test_No . %d\n", i);
        }
    }

    for(i = 8; i < 9; i++)
    {
        if(i == 5){
            printf("zhxk: Import ECC-B256 private key import\n");
        }
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
    
        p_ISD->prm.PK_IMPORT.key_group                = testVector_pkImport_zhxk[i].test_param.key_group;
        p_ISD->prm.PK_IMPORT.key_id                   = testVector_pkImport_zhxk[i].test_param.key_id;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.p_signature              = testVector_pkImport_zhxk[i].test_param.p_signature;
        p_ISD->prm.PK_IMPORT.sig_size_in_bits         = testVector_pkImport_zhxk[i].test_param.sig_size_in_bits;
        p_ISD->prm.PK_IMPORT.curve_id                 = testVector_pkImport_zhxk[i].test_param.curve_id;
        p_ISD->prm.PK_IMPORT.p_priv_scalar            = testVector_pkImport_zhxk[i].test_param.p_priv_scalar;
        p_ISD->prm.PK_IMPORT.p_pub_point_x            = testVector_pkImport_zhxk[i].test_param.p_pub_point_x;
        p_ISD->prm.PK_IMPORT.p_pub_point_y            = testVector_pkImport_zhxk[i].test_param.p_pub_point_y;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_x_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_y_size_in_bits;
        p_ISD->prm.PK_IMPORT.sig_key_group            = testVector_pkImport_zhxk[i].test_param.sig_key_group;
        p_ISD->prm.PK_IMPORT.sig_key_id               = testVector_pkImport_zhxk[i].test_param.sig_key_id;
        p_ISD->prm.PK_IMPORT.key_format               = testVector_pkImport_zhxk[i].test_param.key_format;
        p_ISD->prm.PK_IMPORT.hash_primitive           = testVector_pkImport_zhxk[i].test_param.hash_primitive;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = testVector_pkImport_zhxk[i].test_param.flags.disable_on_debug;
        p_ISD->prm.PK_IMPORT.skip_verify              = testVector_pkImport_zhxk[i].test_param.skip_verify;
        
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            err_info = p_ISD->service_result;
            printf("zhxk: error value is : %d\n", err_info);
            printf("zhxk: Failed Test_No . %d\n", i);
        }else
        {
            printf("zhxk: Success Test_No . %d\n", i);
        }
    }

    printf("zhxk: Import ECC-25519 pub key import\n");
    for(i = 4; i < 5; i++)
    {
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
    
        p_ISD->prm.PK_IMPORT.key_group                = testVector_pkImport_zhxk[i].test_param.key_group;
        p_ISD->prm.PK_IMPORT.key_id                   = testVector_pkImport_zhxk[i].test_param.key_id;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.p_signature              = testVector_pkImport_zhxk[i].test_param.p_signature;
        p_ISD->prm.PK_IMPORT.sig_size_in_bits         = testVector_pkImport_zhxk[i].test_param.sig_size_in_bits;
        p_ISD->prm.PK_IMPORT.curve_id                 = testVector_pkImport_zhxk[i].test_param.curve_id;
        p_ISD->prm.PK_IMPORT.p_priv_scalar            = testVector_pkImport_zhxk[i].test_param.p_priv_scalar;
        p_ISD->prm.PK_IMPORT.p_pub_point_x            = testVector_pkImport_zhxk[i].test_param.p_pub_point_x;
        p_ISD->prm.PK_IMPORT.p_pub_point_y            = testVector_pkImport_zhxk[i].test_param.p_pub_point_y;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_x_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_y_size_in_bits;
        p_ISD->prm.PK_IMPORT.sig_key_group            = testVector_pkImport_zhxk[i].test_param.sig_key_group;
        p_ISD->prm.PK_IMPORT.sig_key_id               = testVector_pkImport_zhxk[i].test_param.sig_key_id;
        p_ISD->prm.PK_IMPORT.key_format               = testVector_pkImport_zhxk[i].test_param.key_format;
        p_ISD->prm.PK_IMPORT.hash_primitive           = testVector_pkImport_zhxk[i].test_param.hash_primitive;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = testVector_pkImport_zhxk[i].test_param.flags.disable_on_debug;
        p_ISD->prm.PK_IMPORT.skip_verify              = testVector_pkImport_zhxk[i].test_param.skip_verify;
        
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            err_info = p_ISD->service_result;
            printf("zhxk: error value is : %d\n", err_info);
        }else
        {
            printf("zhxk: Success Test_No . %d\n", i);
        }
    }

    for(i = 9; i < 10; i++)
    {
        if(i == 5){
            printf("zhxk: Import ECC-25519 private key import\n");
        }
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
    
        p_ISD->prm.PK_IMPORT.key_group                = testVector_pkImport_zhxk[i].test_param.key_group;
        p_ISD->prm.PK_IMPORT.key_id                   = testVector_pkImport_zhxk[i].test_param.key_id;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.p_signature              = testVector_pkImport_zhxk[i].test_param.p_signature;
        p_ISD->prm.PK_IMPORT.sig_size_in_bits         = testVector_pkImport_zhxk[i].test_param.sig_size_in_bits;
        p_ISD->prm.PK_IMPORT.curve_id                 = testVector_pkImport_zhxk[i].test_param.curve_id;
        p_ISD->prm.PK_IMPORT.p_priv_scalar            = testVector_pkImport_zhxk[i].test_param.p_priv_scalar;
        p_ISD->prm.PK_IMPORT.p_pub_point_x            = testVector_pkImport_zhxk[i].test_param.p_pub_point_x;
        p_ISD->prm.PK_IMPORT.p_pub_point_y            = testVector_pkImport_zhxk[i].test_param.p_pub_point_y;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_x_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_y_size_in_bits;
        p_ISD->prm.PK_IMPORT.sig_key_group            = testVector_pkImport_zhxk[i].test_param.sig_key_group;
        p_ISD->prm.PK_IMPORT.sig_key_id               = testVector_pkImport_zhxk[i].test_param.sig_key_id;
        p_ISD->prm.PK_IMPORT.key_format               = testVector_pkImport_zhxk[i].test_param.key_format;
        p_ISD->prm.PK_IMPORT.hash_primitive           = testVector_pkImport_zhxk[i].test_param.hash_primitive;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = testVector_pkImport_zhxk[i].test_param.flags.disable_on_debug;
        p_ISD->prm.PK_IMPORT.skip_verify              = testVector_pkImport_zhxk[i].test_param.skip_verify;
        
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            err_info = p_ISD->service_result;
            printf("zhxk: error value is : %d\n", err_info);
            printf("zhxk: Failed Test_No . %d\n", i);
        }else
        {
            printf("zhxk: Success Test_No . %d\n", i);
        }
    }

    printf("zhxk: Import ECC-P256 pub key import\n");
    for(i = 2; i < 3; i++)
    {
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
    
        p_ISD->prm.PK_IMPORT.key_group                = testVector_pkImport_zhxk[i].test_param.key_group;
        p_ISD->prm.PK_IMPORT.key_id                   = testVector_pkImport_zhxk[i].test_param.key_id;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.p_signature              = testVector_pkImport_zhxk[i].test_param.p_signature;
        p_ISD->prm.PK_IMPORT.sig_size_in_bits         = testVector_pkImport_zhxk[i].test_param.sig_size_in_bits;
        p_ISD->prm.PK_IMPORT.curve_id                 = testVector_pkImport_zhxk[i].test_param.curve_id;
        p_ISD->prm.PK_IMPORT.p_priv_scalar            = testVector_pkImport_zhxk[i].test_param.p_priv_scalar;
        p_ISD->prm.PK_IMPORT.p_pub_point_x            = testVector_pkImport_zhxk[i].test_param.p_pub_point_x;
        p_ISD->prm.PK_IMPORT.p_pub_point_y            = testVector_pkImport_zhxk[i].test_param.p_pub_point_y;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_x_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_y_size_in_bits;
        p_ISD->prm.PK_IMPORT.sig_key_group            = testVector_pkImport_zhxk[i].test_param.sig_key_group;
        p_ISD->prm.PK_IMPORT.sig_key_id               = testVector_pkImport_zhxk[i].test_param.sig_key_id;
        p_ISD->prm.PK_IMPORT.key_format               = testVector_pkImport_zhxk[i].test_param.key_format;
        p_ISD->prm.PK_IMPORT.hash_primitive           = testVector_pkImport_zhxk[i].test_param.hash_primitive;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = testVector_pkImport_zhxk[i].test_param.flags.disable_on_debug;
        p_ISD->prm.PK_IMPORT.skip_verify              = testVector_pkImport_zhxk[i].test_param.skip_verify;
        
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            err_info = p_ISD->service_result;
            printf("zhxk: error value is : %d\n", err_info);
        }else
        {
            printf("zhxk: Success Test_No . %d\n", i);
        }
    }

    for(i = 7; i < 8; i++)
    {
        if(i == 5){
            printf("zhxk: Import ECC-P256 private key import\n");
        }
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
    
        p_ISD->prm.PK_IMPORT.key_group                = testVector_pkImport_zhxk[i].test_param.key_group;
        p_ISD->prm.PK_IMPORT.key_id                   = testVector_pkImport_zhxk[i].test_param.key_id;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.p_signature              = testVector_pkImport_zhxk[i].test_param.p_signature;
        p_ISD->prm.PK_IMPORT.sig_size_in_bits         = testVector_pkImport_zhxk[i].test_param.sig_size_in_bits;
        p_ISD->prm.PK_IMPORT.curve_id                 = testVector_pkImport_zhxk[i].test_param.curve_id;
        p_ISD->prm.PK_IMPORT.p_priv_scalar            = testVector_pkImport_zhxk[i].test_param.p_priv_scalar;
        p_ISD->prm.PK_IMPORT.p_pub_point_x            = testVector_pkImport_zhxk[i].test_param.p_pub_point_x;
        p_ISD->prm.PK_IMPORT.p_pub_point_y            = testVector_pkImport_zhxk[i].test_param.p_pub_point_y;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport_zhxk[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_x_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = testVector_pkImport_zhxk[i].test_param.point_y_size_in_bits;
        p_ISD->prm.PK_IMPORT.sig_key_group            = testVector_pkImport_zhxk[i].test_param.sig_key_group;
        p_ISD->prm.PK_IMPORT.sig_key_id               = testVector_pkImport_zhxk[i].test_param.sig_key_id;
        p_ISD->prm.PK_IMPORT.key_format               = testVector_pkImport_zhxk[i].test_param.key_format;
        p_ISD->prm.PK_IMPORT.hash_primitive           = testVector_pkImport_zhxk[i].test_param.hash_primitive;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = testVector_pkImport_zhxk[i].test_param.flags.disable_on_debug;
        p_ISD->prm.PK_IMPORT.skip_verify              = testVector_pkImport_zhxk[i].test_param.skip_verify;
        
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK)
        {
            err_info = p_ISD->service_result;
            printf("zhxk: error value is : %d\n", err_info);
            printf("zhxk: Failed Test_No . %d\n", i);
        }else
        {
            printf("zhxk: Success Test_No . %d\n", i);
        }
    }
    
    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);

    for(i = 2; i < 7; i++)
    {
        printf("zhxk: Test_No . %d\n", testVectorForEcdh_zhxk[i].test_no);
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        expected = testVectorForEcdh_zhxk[i].expected_service_result;

        p_ISD->service_id     = SERVICE_01_ECDH;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.ECDH.ecc_priv_key_group      = testVectorForEcdh_zhxk[i].test_param.ecc_priv_key_group;
        p_ISD->prm.ECDH.ecc_priv_key_id         = testVectorForEcdh_zhxk[i].test_param.ecc_priv_key_id;
        p_ISD->prm.ECDH.ecc_pub_key_group       = testVectorForEcdh_zhxk[i].test_param.ecc_pub_key_group;
        p_ISD->prm.ECDH.ecc_pub_key_id          = testVectorForEcdh_zhxk[i].test_param.ecc_pub_key_id;
        p_ISD->prm.ECDH.shared_key_group        = testVectorForEcdh_zhxk[i].test_param.shared_key_group;
        p_ISD->prm.ECDH.shared_key_id           = testVectorForEcdh_zhxk[i].test_param.shared_key_id;
        p_ISD->prm.ECDH.kdf_algo                = testVectorForEcdh_zhxk[i].test_param.kdf_algo;
        p_ISD->prm.ECDH.hash_primitive          = testVectorForEcdh_zhxk[i].test_param.hash_primitive;
        p_ISD->prm.ECDH.p_info                  = testVectorForEcdh_zhxk[i].test_param.p_info;
        p_ISD->prm.ECDH.info_size_in_bytes      = testVectorForEcdh_zhxk[i].test_param.info_size_in_bytes;
        p_ISD->prm.ECDH.counter                 = testVectorForEcdh_zhxk[i].test_param.counter;
        p_ISD->prm.ECDH.key_size_in_bytes       = testVectorForEcdh_zhxk[i].test_param.key_size_in_bytes;
        if(testVectorForEcdh[i].test_no == 10){
            p_ISD->prm.ECDH.job_slice               = testVectorForEcdh[i].test_param.job_slice;
        }else{
            p_ISD->prm.ECDH.job_slice               = job_slice[JOB_ECC];
        }
        EXEC_SERVICE_WITH_TIMESTAMP(p_ISD);
        printf("----------------------------------------------------------------------------------\n");
        printf("service result : ");
        //cnt_total++;
        if(p_ISD->service_result == expected)
        {
            //cnt_pass++ ;
            if(p_ISD->service_result == SERV_OK)
            {
                printf( "Expected. (PASS!)\n");
            }
            else
            {
                printf( "Expected.\n");
            }
        }
        else
        {
            //cnt_fail++ ;
            printf( "NOT EXPECTED !! %d (%s)\n", p_ISD->service_result, R_ICUMIF_GetErrorCodeName(p_ISD->service_result) );
        }
        printf("----------------------------------------------------------------------------------\n\n");
    }


    #if IMPORT_ECC_KEY_TEST
    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
    
    printf("Generate ECC RAM key  NIST_P192\n");
    p_ISD->service_id     = SERVICE_03_ECC_KEY_GENERATE;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC_RAM;
    p_ISD->prm.ECC_KEY_GENERATE.key_id                  = 0;
    
    p_ISD->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P192;
    p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;
    
    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result != SERV_OK) goto end;

    //zhxk
    printf("Generate ECC RAM key  NIST_P192 Done\n");
    
    printf("Import ECC PUB key import\n");
    for(i = 0; i < 18; i++)
    {
        if(i == 8){
            printf("Import ECC key import\n");
        }
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
    
        p_ISD->prm.PK_IMPORT.key_group                = testVector_pkImport[i].test_param.key_group;
        p_ISD->prm.PK_IMPORT.key_id                   = testVector_pkImport[i].test_param.key_id;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.p_signature              = testVector_pkImport[i].test_param.p_signature;
        p_ISD->prm.PK_IMPORT.sig_size_in_bits         = testVector_pkImport[i].test_param.sig_size_in_bits;
        p_ISD->prm.PK_IMPORT.curve_id                 = testVector_pkImport[i].test_param.curve_id;
        p_ISD->prm.PK_IMPORT.p_priv_scalar            = testVector_pkImport[i].test_param.p_priv_scalar;
        p_ISD->prm.PK_IMPORT.p_pub_point_x            = testVector_pkImport[i].test_param.p_pub_point_x;
        p_ISD->prm.PK_IMPORT.p_pub_point_y            = testVector_pkImport[i].test_param.p_pub_point_y;
        p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = testVector_pkImport[i].test_param.priv_scalar_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = testVector_pkImport[i].test_param.point_x_size_in_bits;
        p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = testVector_pkImport[i].test_param.point_y_size_in_bits;
        p_ISD->prm.PK_IMPORT.sig_key_group            = testVector_pkImport[i].test_param.sig_key_group;
        p_ISD->prm.PK_IMPORT.sig_key_id               = testVector_pkImport[i].test_param.sig_key_id;
        p_ISD->prm.PK_IMPORT.key_format               = testVector_pkImport[i].test_param.key_format;
        p_ISD->prm.PK_IMPORT.hash_primitive           = testVector_pkImport[i].test_param.hash_primitive;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = testVector_pkImport[i].test_param.flags.disable_on_debug;
        p_ISD->prm.PK_IMPORT.skip_verify              = testVector_pkImport[i].test_param.skip_verify;
        
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK) goto end;
    }
    
    
    printf("Import ECC_ROM_PUB_EXTkey import\n");
    
    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
    #endif
    
    end:
    return;
}


//test 2 16 24~32 34~38  40 53~69 :expected

void icum_ecdh_test()
{
    int cnt_pass = 0;
    int cnt_fail = 0;
    int cnt_total = 0;

    int i;
    r_icumif_service_result_t expected = SERV_OK;
    r_icumif_isd_t  *p_ISD  = (r_icumif_isd_t *)ISD_BUFFER;
    
    r_icumif_isd_t *p_ISD2;
    p_ISD2  = p_ISD + 1;


    for(i = 0; i < 16; i++)
    {
        printf("Test_No . %d\n", testVectorForEcdh[i].test_no);
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        expected = testVectorForEcdh[i].expected_service_result;

        p_ISD->service_id     = SERVICE_01_ECDH;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.ECDH.ecc_priv_key_group      = testVectorForEcdh[i].test_param.ecc_priv_key_group;
        p_ISD->prm.ECDH.ecc_priv_key_id         = testVectorForEcdh[i].test_param.ecc_priv_key_id;
        p_ISD->prm.ECDH.ecc_pub_key_group       = testVectorForEcdh[i].test_param.ecc_pub_key_group;
        p_ISD->prm.ECDH.ecc_pub_key_id          = testVectorForEcdh[i].test_param.ecc_pub_key_id;
        p_ISD->prm.ECDH.shared_key_group        = testVectorForEcdh[i].test_param.shared_key_group;
        p_ISD->prm.ECDH.shared_key_id           = testVectorForEcdh[i].test_param.shared_key_id;
        p_ISD->prm.ECDH.kdf_algo                = testVectorForEcdh[i].test_param.kdf_algo;
        p_ISD->prm.ECDH.hash_primitive          = testVectorForEcdh[i].test_param.hash_primitive;
        p_ISD->prm.ECDH.p_info                  = testVectorForEcdh[i].test_param.p_info;
        p_ISD->prm.ECDH.info_size_in_bytes      = testVectorForEcdh[i].test_param.info_size_in_bytes;
        p_ISD->prm.ECDH.counter                 = testVectorForEcdh[i].test_param.counter;
        p_ISD->prm.ECDH.key_size_in_bytes       = testVectorForEcdh[i].test_param.key_size_in_bytes;
        if(testVectorForEcdh[i].test_no == 10){
            p_ISD->prm.ECDH.job_slice               = testVectorForEcdh[i].test_param.job_slice;
        }else{
            p_ISD->prm.ECDH.job_slice               = job_slice[JOB_ECC];
        }
        EXEC_SERVICE_WITH_TIMESTAMP(p_ISD);
        printf("----------------------------------------------------------------------------------\n");
        printf("service result : ");
        cnt_total++;
        if(p_ISD->service_result == expected)
        {
            cnt_pass++ ;
            if(p_ISD->service_result == SERV_OK)
            {
                printf( "Expected. (PASS!)\n");
            }
            else
            {
                printf( "Expected.\n");
            }
        }
        else
        {
            cnt_fail++ ;
            printf( "NOT EXPECTED !! %d (%s)\n", p_ISD->service_result, R_ICUMIF_GetErrorCodeName(p_ISD->service_result) );
        }
        printf("----------------------------------------------------------------------------------\n\n");
    }
    //Double activation of service(Test No.17)
    printf("Test_No . %d\n", testVectorForEcdh[16].test_no);
    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
    expected = testVectorForEcdh[16].expected_service_result;
    
    p_ISD->service_id     = SERVICE_01_ECDH;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.ECDH.ecc_priv_key_group          = testVectorForEcdh[16].test_param.ecc_priv_key_group;
    p_ISD->prm.ECDH.ecc_priv_key_id             = testVectorForEcdh[16].test_param.ecc_priv_key_id;
    p_ISD->prm.ECDH.ecc_pub_key_group           = testVectorForEcdh[16].test_param.ecc_pub_key_group;
    p_ISD->prm.ECDH.ecc_pub_key_id              = testVectorForEcdh[16].test_param.ecc_pub_key_id;
    p_ISD->prm.ECDH.shared_key_group            = testVectorForEcdh[16].test_param.shared_key_group;
    p_ISD->prm.ECDH.shared_key_id               = testVectorForEcdh[16].test_param.shared_key_id;
    
    p_ISD->prm.ECDH.hash_primitive              = testVectorForEcdh[16].test_param.hash_primitive;
    p_ISD->prm.ECDH.kdf_algo                    = testVectorForEcdh[16].test_param.kdf_algo;  
    p_ISD->prm.ECDH.p_info                      = testVectorForEcdh[16].test_param.p_info;
    p_ISD->prm.ECDH.info_size_in_bytes          = testVectorForEcdh[16].test_param.info_size_in_bytes;
    p_ISD->prm.ECDH.counter                     = testVectorForEcdh[16].test_param.counter;
    p_ISD->prm.ECDH.key_size_in_bytes           = testVectorForEcdh[16].test_param.key_size_in_bytes;
    p_ISD->prm.ECDH.job_slice                   = job_slice[JOB_ECC];
    
    p_ISD2->service_id     = SERVICE_01_ECDH;
    p_ISD2->p_callbackfunc = 0;
    p_ISD2->job_id         = 0;
    p_ISD2->prm.ECDH.ecc_priv_key_group          = KEY_GRP_ECC_RAM;
    p_ISD2->prm.ECDH.ecc_priv_key_id             = 0;
    p_ISD2->prm.ECDH.ecc_pub_key_group           = KEY_GRP_ECC_PUB_EXT;
    p_ISD2->prm.ECDH.ecc_pub_key_id              = 0;
    p_ISD2->prm.ECDH.shared_key_group            = KEY_GRP_AES_RAM;
    p_ISD2->prm.ECDH.shared_key_id               = 1;
    p_ISD2->prm.ECDH.hash_primitive              = HASH_PRIMITIVE_RIPEMD_128;
    p_ISD2->prm.ECDH.kdf_algo                    = KDF_ALGO_KDF2;
    p_ISD2->prm.ECDH.p_info                      = 0;
    p_ISD2->prm.ECDH.info_size_in_bytes          = 0;
    p_ISD2->prm.ECDH.counter                     = 4;
    p_ISD2->prm.ECDH.key_size_in_bytes           = 16;
    p_ISD2->prm.ECDH.job_slice                   = job_slice[JOB_ECC];
    
    vrun_chrono_start();
    trigger_2service_and_wait(p_ISD, p_ISD2);
    vrun_chrono_stop();

    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); NL(); FLUSH();
    printf("----------------------------------------------------------------------------------\n");
    
    printf("service result : ");
    cnt_total++;
    if(p_ISD->service_result == expected && p_ISD2->service_result == expected )
    {
        cnt_pass++ ;
        if(p_ISD->service_result == SERV_OK && p_ISD2->service_result == SERV_OK )
        {
            printf( "Expected. (PASS!)\n");
        }
        else
        {
            printf( "Expected.\n" );
        }
    }
    else
    {
        cnt_fail++ ;
        printf( "NOT EXPECTED(p_ISD) !! %d (%s)\n", p_ISD->service_result, R_ICUMIF_GetErrorCodeName(p_ISD->service_result) );
        printf( "NOT EXPECTED(p_ISD2) !! %d (%s)\n", p_ISD2->service_result, R_ICUMIF_GetErrorCodeName(p_ISD2->service_result) );
    }
    printf("----------------------------------------------------------------------------------\n\n");
    
    for(i = 17; i < 72; i++)
    {
        printf("Test_No . %d\n", testVectorForEcdh[i].test_no);
        memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);
        expected = testVectorForEcdh[i].expected_service_result;

        p_ISD->service_id     = SERVICE_01_ECDH;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.ECDH.ecc_priv_key_group      = testVectorForEcdh[i].test_param.ecc_priv_key_group;
        p_ISD->prm.ECDH.ecc_priv_key_id         = testVectorForEcdh[i].test_param.ecc_priv_key_id;
        p_ISD->prm.ECDH.ecc_pub_key_group       = testVectorForEcdh[i].test_param.ecc_pub_key_group;
        p_ISD->prm.ECDH.ecc_pub_key_id          = testVectorForEcdh[i].test_param.ecc_pub_key_id;
        p_ISD->prm.ECDH.shared_key_group        = testVectorForEcdh[i].test_param.shared_key_group;
        p_ISD->prm.ECDH.shared_key_id           = testVectorForEcdh[i].test_param.shared_key_id;
        p_ISD->prm.ECDH.kdf_algo                = testVectorForEcdh[i].test_param.kdf_algo;
        p_ISD->prm.ECDH.hash_primitive          = testVectorForEcdh[i].test_param.hash_primitive;
        p_ISD->prm.ECDH.p_info                  = testVectorForEcdh[i].test_param.p_info;
        p_ISD->prm.ECDH.info_size_in_bytes      = testVectorForEcdh[i].test_param.info_size_in_bytes;
        p_ISD->prm.ECDH.counter                 = testVectorForEcdh[i].test_param.counter;
        p_ISD->prm.ECDH.key_size_in_bytes       = testVectorForEcdh[i].test_param.key_size_in_bytes;
        p_ISD->prm.ECDH.job_slice               = job_slice[JOB_ECC];

        EXEC_SERVICE_WITH_TIMESTAMP(p_ISD);
        printf("----------------------------------------------------------------------------------\n");
        printf("service result : ");
        cnt_total++;
        if(p_ISD->service_result == expected)
        {
            cnt_pass++ ;
            if(p_ISD->service_result == SERV_OK)
            {
                printf( "Expected. (PASS!)\n");
            }
            else
            {
                printf( "Expected.\n");
            }
        }
        else
        {
            cnt_fail++ ;
            printf( "NOT EXPECTED !! %d (%s)\n", p_ISD->service_result, R_ICUMIF_GetErrorCodeName(p_ISD->service_result) );
        }
        printf("----------------------------------------------------------------------------------\n\n");
    }

    printf("\nDone!\n");
    printf( "\nTest Result : Executed %d, Passed = %d, Failed = %d\n", cnt_total, cnt_pass, cnt_fail );

    restart_life_signal();
}
