/******************************************************************************/
/* Component Name   ECDH                                                      */
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

#include "shared.h"
#include "pe1.h"

// ********************************************************************************
// Public constants
// ********************************************************************************

// ********************************************************************************
// Local constants
// ********************************************************************************

const uint8_t priv_key_P192[] = {0x89, 0xac, 0x7d, 0xfa, 0x2a, 0x65, 0x93, 0xc6, 0xf5, 0x59, 0xed, 0xe6, 0x3b, 0xe6, 0x5d, 0x4f, 0x6e, 0xce, 0x22, 0xf7, 0x19, 0x11, 0x56, 0xe0};
const uint8_t point_x_P192[] = {0x32, 0x67, 0xff, 0x8e, 0x7b, 0x3a, 0x72, 0x74, 0xa2, 0x61, 0x71, 0xf4, 0x2b, 0x1f, 0xe4, 0xd1, 0x6b, 0x1b, 0xc1, 0x93, 0xf9, 0x6a, 0x1c, 0xa1};
const uint8_t point_y_P192[] = {0x4c, 0x05, 0x6e, 0xa9, 0xce, 0x5b, 0x65, 0x4a, 0xb1, 0x48, 0xa7, 0x8d, 0xd0, 0xb3, 0x84, 0x53, 0xe1, 0x01, 0x0a, 0xcb, 0x26, 0xa8, 0xb0, 0xde};

const uint8_t priv_key_P224[] = {0xce, 0xf6, 0xbd, 0x33, 0xf8, 0xcc, 0xec, 0xad, 0xcd, 0x6f, 0x03, 0xf3, 0x95, 0xf7, 0x04, 0xeb, 0xfd, 0x6d, 0x34, 0x2d, 0xdf, 0x21, 0x9c, 0x8a, 0x09, 0xbe, 0xad, 0xba};
const uint8_t point_x_P224[] = {0xaf, 0xd0, 0xc3, 0x50, 0x93, 0xb1, 0xa2, 0x75, 0x7b, 0x96, 0x2f, 0x73, 0x4e, 0xf9, 0x12, 0xaa, 0x4b, 0x90, 0xc9, 0x2e, 0x11, 0xa8, 0x5a, 0xc5, 0x4c, 0x75, 0xbe, 0xac};
const uint8_t point_y_P224[] = {0x5b, 0xee, 0x20, 0x66, 0x48, 0xdf, 0x21, 0x35, 0xd1, 0x22, 0x76, 0x6c, 0x85, 0x8c, 0xc3, 0x0d, 0x34, 0x36, 0x3a, 0xc0, 0xb3, 0x9d, 0x33, 0x31, 0xce, 0x97, 0x70, 0x63};

const uint8_t priv_key_P256[] = {0xb9, 0x9b, 0xa2, 0x90, 0x38, 0xa6, 0xd4, 0x8f, 0x8f, 0x1b, 0x3a, 0x50, 0x0f, 0xaf, 0x3c, 0x2e, 0x71, 0x18, 0x8c, 0xb7, 0xa1, 0xc6, 0x11, 0x3a, 0x1b, 0x66, 0x82, 0xc4, 0x33, 0x51, 0xe5, 0x61};
const uint8_t point_x_P256[] = {0xa8, 0x78, 0xb6, 0x40, 0x36, 0xf3, 0x40, 0x69, 0x6a, 0x9c, 0x50, 0x32, 0xa3, 0x0c, 0xab, 0x86, 0xd2, 0xbb, 0xa1, 0xd6, 0xb7, 0x74, 0xfa, 0x75, 0x7a, 0xf8, 0x0f, 0x6c, 0xda, 0xc2, 0x04, 0xd8};
const uint8_t point_y_P256[] = {0xb2, 0xaa, 0x40, 0xae, 0xbb, 0x9b, 0x2e, 0x8b, 0xa6, 0x91, 0x73, 0x7d, 0x17, 0xec, 0x82, 0x12, 0x10, 0xb8, 0xe7, 0x87, 0x6c, 0xff, 0x8b, 0x4a, 0x05, 0xc9, 0x65, 0x4e, 0x0e, 0x8c, 0x7b, 0x87};


const uint8_t priv_key_B192[] = {0x21, 0x1d, 0xc1, 0x44, 0x96, 0x64, 0x30, 0xa0, 0x50, 0x9a, 0xf9, 0x54, 0x99, 0x5a, 0x52, 0x2a, 0x74, 0xc7, 0x36, 0x84, 0xa5, 0x04, 0x86, 0xa9};
const uint8_t point_x_B192[] = {0x4a, 0xfa, 0xaa, 0x0f, 0x7f, 0x4c, 0x4f, 0x03, 0x95, 0xdc, 0x94, 0x66, 0x5d, 0x3c, 0xa7, 0x2e, 0x8c, 0x7f, 0x18, 0x76, 0x87, 0x47, 0xa4, 0xac};
const uint8_t point_y_B192[] = {0x33, 0x73, 0xa8, 0x81, 0x55, 0x9f, 0x16, 0xdc, 0x3f, 0xd3, 0x93, 0xd4, 0x55, 0xac, 0xbf, 0xdc, 0x59, 0xf1, 0x86, 0x03, 0x23, 0xdb, 0x14, 0x47};

const uint8_t priv_key_B224[] = {0x28, 0x96, 0xea, 0x93, 0xeb, 0x1f, 0x02, 0x80, 0x2a, 0x33, 0x6c, 0x52, 0x26, 0x56, 0x18, 0xc0, 0x3e, 0x75, 0x1f, 0x46, 0x6f, 0x9b, 0x95, 0x77, 0x76, 0x4e, 0xdf, 0x64};
const uint8_t point_x_B224[] = {0xb5, 0x2c, 0x28, 0x4c, 0x74, 0xb9, 0x58, 0x28, 0xb2, 0x2e, 0x4b, 0xdf, 0x48, 0x51, 0x93, 0xcf, 0xdb, 0xd3, 0xfa, 0x4e, 0xf5, 0x02, 0xb6, 0x8d, 0xf6, 0x3d, 0x00, 0xe7};
const uint8_t point_y_B224[] = {0x53, 0x89, 0x9d, 0x90, 0x44, 0x79, 0x8c, 0xca, 0x6e, 0x2a, 0x68, 0xe7, 0xea, 0x1c, 0x4d, 0x9c, 0xb3, 0x9a, 0x81, 0xdb, 0x64, 0x70, 0x51, 0x19, 0xd6, 0xc6, 0x18, 0x04};

const uint8_t priv_key_B256[] = {0x2d, 0x3f, 0xc8, 0x95, 0x62, 0x51, 0x2c, 0x98, 0x10, 0x59, 0x0b, 0x31, 0x57, 0x28, 0x16, 0x67, 0x20, 0x1c, 0x11, 0x35, 0x2e, 0x49, 0x06, 0xf8, 0xc8, 0x16, 0x95, 0xb5, 0x32, 0xb0, 0xc8, 0xda};
const uint8_t point_x_B256[] = {0x7b, 0xaf, 0x09, 0x6c, 0x72, 0x2d, 0xd3, 0x8b, 0x92, 0xc2, 0x68, 0x0e, 0xd0, 0xf6, 0x53, 0x8e, 0x9a, 0x06, 0xc9, 0xf7, 0x83, 0x5c, 0xc3, 0x25, 0x2f, 0x17, 0x90, 0x38, 0xa7, 0x4d, 0x83, 0xdd};
const uint8_t point_y_B256[] = {0x9a, 0xe5, 0x38, 0xe3, 0x29, 0xb9, 0x48, 0x69, 0x69, 0x90, 0xec, 0x0b, 0x94, 0x7b, 0xec, 0xcd, 0x92, 0x50, 0x3b, 0x14, 0xd3, 0x66, 0xf2, 0xff, 0x6d, 0x61, 0xd3, 0xc2, 0x5a, 0x37, 0x5e, 0x16};

const uint8_t priv_key_B320[] = {0x3a, 0xf7, 0x0e, 0x66, 0x3b, 0x6b, 0xf2, 0x62, 0xd0, 0xf3, 0xec, 0x68, 0xbc, 0x6a, 0xec, 0x7a, 0x9c, 0x80, 0x00, 0x1c, 0x4d, 0x54, 0xb8, 0x95, 0xd2, 0x94, 0x08, 0xb0, 0x06, 0x61, 0x3a, 0x3d, 0xe7, 0x3f, 0x33, 0x1a, 0x36, 0x69, 0xf9, 0x44};
const uint8_t point_x_B320[] = {0xbd, 0xd4, 0x7a, 0x50, 0xe5, 0x61, 0x80, 0x77, 0x95, 0x8e, 0xcc, 0x71, 0xb6, 0x0d, 0xeb, 0xd7, 0x7c, 0xfe, 0xec, 0xb4, 0xeb, 0xe0, 0xd0, 0x53, 0x93, 0x23, 0x4e, 0xd2, 0x87, 0x21, 0x16, 0x2a, 0xef, 0xbc, 0xc4, 0x13, 0xde, 0x91};
const uint8_t point_y_B320[] = {0x8b, 0x66, 0x54, 0x1c, 0x71, 0x91, 0x06, 0xa8, 0x92, 0x8a, 0x4f, 0xff, 0x25, 0x1b, 0xa3, 0xe7, 0x88, 0x86, 0x43, 0x2e, 0xe1, 0xd8, 0x74, 0xfb, 0xf8, 0x00, 0x55, 0xbe, 0xfa, 0xf6, 0xdd, 0xe7, 0x1e, 0xe4, 0xf2, 0x27, 0x7b, 0x2d};

const uint8_t priv_key_25519[] = {0x77, 0x07, 0x6d, 0x0a, 0x73, 0x18, 0xa5, 0x7d, 0x3c, 0x16, 0xc1, 0x72, 0x51, 0xb2, 0x66, 0x45, 0xdf, 0x4c, 0x2f, 0x87, 0xeb, 0xc0, 0x99, 0x2a, 0xb1, 0x77, 0xfb, 0xa5, 0x1d, 0xb9, 0x2c, 0x2a};
const uint8_t point_x_25519[] = {0x85, 0x20, 0xf0, 0x09, 0x89, 0x30, 0xa7, 0x54, 0x74, 0x8b, 0x7d, 0xdc, 0xb4, 0x3e, 0xf7, 0x5a, 0x0d, 0xbf, 0x3a, 0x0d, 0x26, 0x38, 0x1a, 0xf4, 0xeb, 0xa4, 0xa9, 0x8e, 0xaa, 0x9b, 0x4e, 0x6a};

#pragma alignvar(4)
const uint8_t subjECCprime192v1Cert_md5_rsa512[] = {
    0x30,0x82,0x01,0x2C,0x30,0x81,0xD7,0x02,0x09,0x00,0xC5,0x16,0x40,0xCD,0xCD,0x10,
    0x35,0xD0,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x04,0x05,
    0x00,0x30,0x26,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0A,0x0C,0x0A,0x52,0x48,
    0x38,0x35,0x30,0x49,0x43,0x55,0x4D,0x46,0x31,0x0F,0x30,0x0D,0x06,0x03,0x55,0x04,
    0x03,0x0C,0x06,0x52,0x6F,0x6F,0x74,0x43,0x41,0x30,0x1E,0x17,0x0D,0x31,0x37,0x30,
    0x39,0x31,0x35,0x31,0x31,0x34,0x32,0x31,0x39,0x5A,0x17,0x0D,0x31,0x38,0x30,0x39,
    0x31,0x35,0x31,0x31,0x34,0x32,0x31,0x39,0x5A,0x30,0x28,0x31,0x14,0x30,0x12,0x06,
    0x03,0x55,0x04,0x0A,0x0C,0x0B,0x52,0x48,0x38,0x35,0x30,0x49,0x43,0x55,0x4D,0x46,
    0x57,0x31,0x10,0x30,0x0E,0x06,0x03,0x55,0x04,0x03,0x0C,0x07,0x53,0x75,0x62,0x6A,
    0x65,0x63,0x74,0x30,0x49,0x30,0x13,0x06,0x07,0x2A,0x86,0x48,0xCE,0x3D,0x02,0x01,
    0x06,0x08,0x2A,0x86,0x48,0xCE,0x3D,0x03,0x01,0x01,0x03,0x32,0x00,0x04,0x32,0x67,
    0xFF,0x8E,0x7B,0x3A,0x72,0x74,0xA2,0x61,0x71,0xF4,0x2B,0x1F,0xE4,0xD1,0x6B,0x1B,
    0xC1,0x93,0xF9,0x6A,0x1C,0xA1,0x4C,0x05,0x6E,0xA9,0xCE,0x5B,0x65,0x4A,0xB1,0x48,
    0xA7,0x8D,0xD0,0xB3,0x84,0x53,0xE1,0x01,0x0A,0xCB,0x26,0xA8,0xB0,0xDE,0x30,0x0D,
    0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x04,0x05,0x00,0x03,0x41,0x00,
    0x00,0x8D,0x78,0x09,0x39,0x0B,0xC1,0xC8,0xD5,0x52,0x70,0xF0,0x4A,0xFF,0x98,0x2A,
    0x9B,0xEA,0x75,0x40,0x3F,0xFB,0xA9,0x17,0x63,0xB4,0x12,0x21,0xF1,0x50,0x4A,0x81,
    0xE4,0x59,0x62,0x7D,0x1A,0xB2,0x80,0x68,0xD1,0x0A,0xD9,0x8E,0x9C,0xB2,0x6E,0x74,
    0x8E,0xB9,0x1D,0x1D,0xED,0x06,0x8D,0x8B,0x5D,0x6A,0xC2,0x73,0x30,0x8D,0xC4,0xF3,
};

// ********************************************************************************
// Local constants
// ********************************************************************************

typedef struct testVectorForPkImport {
    int16_t                 test_no;
    ISD_PK_IMPORT_t         test_param;
} testVectorForPkImport_t;

// *******************************************************************************
//  Test Vector
// *******************************************************************************
static const testVectorForPkImport_t  testVector_pkImport[] = {
//   test_no,    p_pub_point_x,  p_pub_point_y, p_priv_scalar,  p_signature,   p_info,   key_format,             sig_format,             key_group,       key_id, sig_key_group,     sig_key_id,   sig_algo,                hash_primitive,            curve_id,                   point_x_size_in_bits,      point_y_size_in_bits,       priv_scalar_size_in_bits,   sig_size_in_bits,   info_size_in_bytes,   p_x509_certificate,     x509_certificate_size_in_bytes   disable_on_debug,   write_protected,  test_skip_verify
//ECC_pub
    {1 ,        {point_x_P192 ,  point_y_P192,  0,              (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_PUB, 0,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P192,        sizeof(point_x_P192)  * 8, sizeof(point_y_P192) * 8,   0,                          0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {2 ,        {point_x_P224 ,  point_y_P224,  0,              (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_PUB, 1,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P224,        sizeof(point_x_P224)  * 8, sizeof(point_y_P224) * 8,   0,                          0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {3 ,        {point_x_P256 ,  point_y_P256,  0,              (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_PUB, 2,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P256,        sizeof(point_x_P256)  * 8, sizeof(point_y_P256) * 8,   0,                          0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {4 ,        {point_x_B192 ,  point_y_B192,  0,              (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_PUB, 3,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_BRAINPOOL_P192,   sizeof(point_x_B192)  * 8, sizeof(point_y_B192) * 8,   0,                          0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {5 ,        {point_x_B224 ,  point_y_B224,  0,              (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_PUB, 4,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_BRAINPOOL_P224,   sizeof(point_x_B224)  * 8, sizeof(point_y_B224) * 8,   0,                          0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {6 ,        {point_x_B256 ,  point_y_B256,  0,              (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_PUB, 5,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_BRAINPOOL_P256,   sizeof(point_x_B256)  * 8, sizeof(point_y_B256) * 8,   0,                          0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {7 ,        {point_x_B320 ,  point_y_B320,  0,              (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_PUB, 6,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_BRAINPOOL_P320,   sizeof(point_x_B320)  * 8, sizeof(point_y_B320) * 8,   0,                          0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {8 ,        {point_x_25519,  0           ,  0,              (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC_PUB, 7,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_CURVE25519,       sizeof(point_x_25519) * 8, 0                       ,   0,                          0,                  0,                    0,                      0,                               {0,                  0 },           1}},
//ECC_priv
    {9 ,        {point_x_P192 ,  point_y_P192,  priv_key_P192,  (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC,     0,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P192     ,   sizeof(point_x_P192)  * 8, sizeof(point_y_P192) * 8,   sizeof(priv_key_P192) * 8,  0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {10,        {point_x_P224 ,  point_y_P224,  priv_key_P224,  (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC,     1,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P224     ,   sizeof(point_x_P224)  * 8, sizeof(point_y_P224) * 8,   sizeof(priv_key_P224) * 8,  0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {11,        {point_x_P256 ,  point_y_P256,  priv_key_P256,  (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC,     2,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P256     ,   sizeof(point_x_P256)  * 8, sizeof(point_y_P256) * 8,   sizeof(priv_key_P256) * 8,  0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {12,        {point_x_B192 ,  point_y_B192,  priv_key_B192,  (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC,     3,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_BRAINPOOL_P192,   sizeof(point_x_B192)  * 8, sizeof(point_y_B192) * 8,   sizeof(priv_key_B192) * 8,  0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {13,        {point_x_B224 ,  point_y_B224,  priv_key_B224,  (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC,     4,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_BRAINPOOL_P224,   sizeof(point_x_B224)  * 8, sizeof(point_y_B224) * 8,   sizeof(priv_key_B224) * 8,  0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {14,        {point_x_B256 ,  point_y_B256,  priv_key_B256,  (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC,     5,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_BRAINPOOL_P256,   sizeof(point_x_B256)  * 8, sizeof(point_y_B256) * 8,   sizeof(priv_key_B256) * 8,  0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {15,        {point_x_B320 ,  point_y_B320,  priv_key_B320,  (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC,     6,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_BRAINPOOL_P320,   sizeof(point_x_B320)  * 8, sizeof(point_y_B320) * 8,   sizeof(priv_key_B320) * 8,  0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {16,        {point_x_25519,  0           ,  priv_key_25519, (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC,     7,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_CURVE25519    ,   sizeof(point_x_25519) * 8, 0                       ,   sizeof(priv_key_25519) * 8, 0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {17,        {point_x_P192 ,  point_y_P192,  priv_key_P192,  (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC,     8,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P192     ,   sizeof(point_x_P192)  * 8, sizeof(point_y_P192) * 8,   sizeof(priv_key_P192) * 8,  0,                  0,                    0,                      0,                               {0,                  0 },           1}},
    {18,        {point_x_P192 ,  point_y_P192,  priv_key_P192,  (uint8_t*)0,   0,        FORMAT_BINARY_PLAIN,    (r_number_format_t)0,   KEY_GRP_ECC,     9,      KEY_GRP_RSA_PUB,   0,            (r_signature_algo_t)0,   HASH_PRIMITIVE_SHA2_256,   ECC_CURVE_NIST_P192     ,   sizeof(point_x_P192)  * 8, sizeof(point_y_P192) * 8,   sizeof(priv_key_P192) * 8,  0,                  0,                    0,                      0,                               {1,                  0 },           1}}
};

typedef struct testVectorForEcdh {
    int16_t                     test_no;
    ISD_ECDH_t                  test_param;
    r_icumif_service_result_t   expected_service_result;
} testVectorForEcdh_t;

// ********************************************************************************
//  Test Vector
// *******************************************************************************
static const testVectorForEcdh_t testVectorForEcdh[] = {
//  test_no,     test_ecc_priv_key_group,   test_ecc_priv_key_id,   test_ecc_pub_key_group,   test_ecc_pub_key_id,   test_shared_key_group,  test_shared_key_id,   test_kdf_algo,   test_hash_primitive,         test_job_slice   test_p_info,          test_counter,   test_info_size_in_bytes,   key_size_in_bytes,   expected_service_result
    {1 ,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_RAM,          0,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_OK},
    {2 ,        {KEY_GRP_ECC,               0,                      KEY_GRP_ECC_RAM,          1,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_KEY_SLOT_EMPTY},

    {3 ,        {KEY_GRP_ECC,               1,                      KEY_GRP_ECC_PUB,          1,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF2,   HASH_PRIMITIVE_SHA2_224,     0,               0,                    4,              0,                         16},                 SERV_OK},
    {4 ,        {KEY_GRP_ECC,               2,                      KEY_GRP_ECC_PUB,          2,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF3,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_OK},
    {5 ,        {KEY_GRP_ECC,               3,                      KEY_GRP_ECC_PUB,          3,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_PKDF1,  HASH_PRIMITIVE_SHA2_384,     0,               LOREM_IPSUM,          4,              16,                        16},                 SERV_OK},
    {6 ,        {KEY_GRP_ECC,               4,                      KEY_GRP_ECC_PUB,          4,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_PKDF2,  HASH_PRIMITIVE_SHA2_512,     0,               0,                    4,              0,                         16},                 SERV_OK},
    {7 ,        {KEY_GRP_ECC,               5,                      KEY_GRP_ECC_PUB,          5,                     KEY_GRP_AES_RAM,        0,                    KDF_ALGO_KDF1,   HASH_PRIMITIVE_SHA2_256,     0,               0,                    4,              0,                         16},                 SERV_OK},
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
