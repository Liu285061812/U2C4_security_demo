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
#include "pe_target_device.h"

#include "shared.h"

#include "vrun.api.h"

#include "renesas_types.h"

/**********************************************************************************
* Global external references
***********************************************************************************/
extern const uint8_t sha512_large_msg[];


// ********************************************************************************
// Test functions
// ********************************************************************************

void hmac_generation_test(void);

static void hmac_generation_test1(void);
static void hmac_generation_test2(void);
static void hmac_generation_test3(void);
static void hmac_generation_test4(void);
static void hmac_generation_test5(void);
static void hmac_generation_test6(void);
static void hmac_generation_test7(void);
static void hmac_generation_test8(void);
static void hmac_generation_test9(void);
static void hmac_generation_test10(void);
static void hmac_generation_error_test11(void);
static void hmac_generation_error_test12(void);
static void hmac_generation_error_test13(void);
static void hmac_generation_error_test14(void);
static void hmac_generation_error_test15(void);
static void hmac_generation_error_test16(void);
static void hmac_generation_error_test17(void);
static void hmac_generation_test18(void);
static void hmac_generation_test19(void);


void hmac_verify_test(void);

static void hmac_verify_test1(void);
static void hmac_verify_test2(void);
static void hmac_verify_test3(void);
static void hmac_verify_test4(void);
static void hmac_verify_test5(void);
static void hmac_verify_test6(void);
static void hmac_verify_test7(void);
static void hmac_verify_test8(void);
static void hmac_verify_test9(void);
static void hmac_verify_test10(void);
static void hmac_verify_error_test11(void);
static void hmac_verify_error_test12(void);
static void hmac_verify_error_test13(void);
static void hmac_verify_error_test14(void);
static void hmac_verify_error_test15(void);
static void hmac_verify_error_test16(void);
static void hmac_verify_error_test17(void);

static void hmac_key_null_test(void);

static unsigned int test_no;

/*===========================================================================
  ICU-M test function
  [PUBLIC]
  ===========================================================================*/

static const uint8_t p_msg1[]={
    0xfc, 0xd6, 0xd9, 0x8b, 0xef, 0x45, 0xed, 0x68,
    0x50, 0x80, 0x6e, 0x96, 0xf2, 0x55, 0xfa, 0x0c,
    0x81, 0x14, 0xb7, 0x28, 0x73, 0xab, 0xe8, 0xf4,
    0x3c, 0x10, 0xbe, 0xa7, 0xc1, 0xdf, 0x70, 0x6f,
    0x10, 0x45, 0x8e, 0x6d, 0x4e, 0x1c, 0x92, 0x01,
    0xf0, 0x57, 0xb8, 0x49, 0x2f, 0xa1, 0x0f, 0xe4,
    0xb5, 0x41, 0xd0, 0xfc, 0x9d, 0x41, 0xef, 0x83,
    0x9a, 0xcf, 0xf1, 0xbc, 0x76, 0xe3, 0xfd, 0xfe,
    0xbf, 0x22, 0x35, 0xb5, 0xbd, 0x03, 0x47, 0xa9,
    0xa6, 0x30, 0x3e, 0x83, 0x15, 0x2f, 0x9f, 0x8d,
    0xb9, 0x41, 0xb1, 0xb9, 0x4a, 0x8a, 0x1c, 0xe5,
    0xc2, 0x73, 0xb5, 0x5d, 0xc9, 0x4d, 0x99, 0xa1,
    0x71, 0x37, 0x79, 0x69, 0x23, 0x41, 0x34, 0xe7,
    0xda, 0xd1, 0xab, 0x4c, 0x8e, 0x46, 0xd1, 0x8d,
    0xf4, 0xdc, 0x01, 0x67, 0x64, 0xcf, 0x95, 0xa1,
    0x1a, 0xc4, 0xb4, 0x91, 0xa2, 0x64, 0x6b, 0xe1
};

static const uint8_t p_msg2[]={
    0xd6, 0x8b, 0x82, 0x8a, 0x15, 0x3f, 0x51, 0x98,
    0xc0, 0x05, 0xee, 0x36, 0xc0, 0xaf, 0x2f, 0xf9,
    0x2e, 0x84, 0x90, 0x75, 0x17, 0xf0, 0x1d, 0x9b,
    0x7c, 0x79, 0x93, 0x46, 0x9d, 0xf5, 0xc2, 0x10,
    0x78, 0xfa, 0x35, 0x6a, 0x8c, 0x97, 0x15, 0xec,
    0xe2, 0x41, 0x4b, 0xe9, 0x4e, 0x10, 0xe5, 0x47,
    0xf3, 0x2c, 0xbb, 0x8d, 0x05, 0x82, 0x52, 0x3e,
    0xd3, 0xbb, 0x00, 0x66, 0x04, 0x6e, 0x51, 0x72,
    0x20, 0x94, 0xaa, 0x44, 0x53, 0x3d, 0x2c, 0x87,
    0x6e, 0x82, 0xdb, 0x40, 0x2f, 0xbb, 0x00, 0xa6,
    0xc2, 0xf2, 0xcc, 0x34, 0x87, 0x97, 0x3d, 0xfc,
    0x16, 0x74, 0x46, 0x3e, 0x81, 0xe4, 0x2a, 0x39,
    0xd9, 0x40, 0x29, 0x41, 0xf3, 0x9b, 0x5e, 0x12,
    0x6b, 0xaf, 0xe8, 0x64, 0xea, 0x16, 0x48, 0xc0,
    0xa5, 0xbe, 0x0a, 0x91, 0x26, 0x97, 0xa8, 0x7e,
    0x4f, 0x8e, 0xab, 0xf7, 0x9c, 0xbf, 0x13, 0x0e
};

static const uint8_t p_msg3[]={
    0x41, 0x18, 0x43, 0xa2, 0x13, 0x87, 0x84, 0x6f,
    0x3b, 0x9e, 0xd5, 0xfc, 0x54, 0x5a, 0xca, 0xdf,
    0xa5, 0xb7, 0x03, 0x86, 0xf6, 0x2d, 0xa4, 0xd9,
    0xa2, 0x7b, 0x04, 0x1b, 0xee, 0xa3, 0xaa, 0x11,
    0x99, 0x36, 0x75, 0x67, 0xb4, 0xd1, 0x1a, 0x4f,
    0xb4, 0xe8, 0xd4, 0x6b, 0xc6, 0xc2, 0x56, 0xed,
    0x62, 0xc5, 0x05, 0xfd, 0x23, 0xf4, 0x64, 0x5b,
    0xd6, 0xb6, 0xcf, 0x45, 0xd1, 0xd9, 0x6d, 0x9b,
    0x86, 0xd6, 0x60, 0x41, 0x57, 0x57, 0x3e, 0xc5,
    0xac, 0xf6, 0xc5, 0x41, 0x43, 0x48, 0xca, 0x83,
    0xc8, 0x1a, 0x73, 0x6c, 0xa6, 0xfa, 0xa6, 0x96,
    0x1c, 0xfa, 0xc1, 0x39, 0x93, 0xb0, 0x8c, 0x50,
    0x2f, 0x81, 0x6c, 0xf7, 0xa4, 0x20, 0xd9, 0x18,
    0x4b, 0x51, 0x11, 0x46, 0x75, 0xf3, 0x0e, 0xe9,
    0xff, 0x3d, 0xb6, 0x9c, 0x26, 0x48, 0x53, 0xd3,
    0x9d, 0xcd, 0x42, 0xc1, 0xdd, 0x31, 0xef, 0x79
};

static const uint8_t p_msg4[]={
    0x41, 0x0a, 0xc8, 0x70, 0x3f, 0x31, 0x2f, 0xe4,
    0x26, 0xf0, 0xd1, 0xa6, 0x2d, 0x36, 0x2d, 0x44,
    0x78, 0x25, 0x2f, 0x11, 0xdf, 0xc7, 0x0b, 0x78,
    0xf0, 0xfc, 0x6c, 0x91, 0x37, 0xb7, 0xce, 0xf2,
    0xbd, 0x6a, 0x28, 0xc8, 0xce, 0xbf, 0xcf, 0x26,
    0xff, 0x89, 0x97, 0x9c, 0x70, 0xfa, 0x3b, 0x0c,
    0x4f, 0x16, 0xff, 0xb9, 0x9d, 0x67, 0x93, 0xa1,
    0x35, 0xf3, 0x3b, 0x0d, 0x0a, 0x6b, 0x2d, 0x66,
    0xb0, 0xa5, 0x3a, 0x4a, 0x1e, 0x4c, 0xb1, 0xdf,
    0xb2, 0xd7, 0x81, 0x67, 0x75, 0xb7, 0x9d, 0x15,
    0xa1, 0xd5, 0xa5, 0x1f, 0x60, 0x3b, 0xc2, 0x15,
    0xd7, 0x11, 0xd2, 0x71, 0x63, 0xcc, 0xc5, 0x6f,
    0x22, 0xa2, 0x46, 0x1f, 0xb6, 0x41, 0x9e, 0xb8,
    0x45, 0xc0, 0xd6, 0x2f, 0xc4, 0xd6, 0x1c, 0x08,
    0x76, 0x4a, 0x69, 0x6d, 0xaf, 0xaa, 0x60, 0x7d,
    0xde, 0x40, 0x78, 0xe6, 0xca, 0x42, 0x5d, 0xb6
};

static const uint8_t p_msg5[]={
    0x75, 0x2c, 0xff, 0x52, 0xe4, 0xb9, 0x07, 0x68,
    0x55, 0x8e, 0x53, 0x69, 0xe7, 0x5d, 0x97, 0xc6,
    0x96, 0x43, 0x50, 0x9a, 0x5e, 0x59, 0x04, 0xe0,
    0xa3, 0x86, 0xcb, 0xe4, 0xd0, 0x97, 0x0e, 0xf7,
    0x3f, 0x91, 0x8f, 0x67, 0x59, 0x45, 0xa9, 0xae,
    0xfe, 0x26, 0xda, 0xea, 0x27, 0x58, 0x7e, 0x8d,
    0xc9, 0x09, 0xdd, 0x56, 0xfd, 0x04, 0x68, 0x80,
    0x5f, 0x83, 0x40, 0x39, 0xb3, 0x45, 0xf8, 0x55,
    0xcf, 0xe1, 0x9c, 0x44, 0xb5, 0x5a, 0xf2, 0x41,
    0xff, 0xf3, 0xff, 0xcd, 0x80, 0x45, 0xcd, 0x5c,
    0x28, 0x8e, 0x6c, 0x4e, 0x28, 0x4c, 0x37, 0x20,
    0x57, 0x0b, 0x58, 0xe4, 0xd4, 0x7b, 0x8f, 0xee,
    0xed, 0xc5, 0x2f, 0xd1, 0x40, 0x1f, 0x69, 0x8a,
    0x20, 0x9f, 0xcc, 0xfa, 0x3b, 0x4c, 0x0d, 0x9a,
    0x79, 0x7b, 0x04, 0x6a, 0x27, 0x59, 0xf8, 0x2a,
    0x54, 0xc4, 0x1c, 0xcd, 0x7b, 0x5f, 0x59, 0x2b
};

static const uint8_t p_msg6[]={
    0xe0, 0xef, 0xf0, 0x0f, 0x3c, 0x46, 0xe9, 0x6c,
    0x8d, 0x5b, 0xd1, 0x81, 0x28, 0x3e, 0x46, 0x05,
    0x34, 0x8e, 0x3f, 0xa1, 0x0b, 0x47, 0x94, 0x5d,
    0xe3, 0xdc, 0xc1, 0x59, 0xae, 0x86, 0xe7, 0xbd,
    0x3f, 0xdb, 0x13, 0xf2, 0xad, 0xa2, 0xc3, 0x13,
    0xfc, 0xe6, 0xa6, 0x9e, 0xfa, 0x49, 0xa4, 0x70,
    0x68, 0x9b, 0x1e, 0xf0, 0x5a, 0xab, 0x77, 0x8a,
    0xe1, 0x5d, 0xd3, 0x5f, 0xe6, 0xfd, 0x1e, 0x3a,
    0x59, 0xd3, 0x51, 0xc6, 0x8c, 0xf8, 0xf0, 0xff,
    0xd9, 0x68, 0xd7, 0xe7, 0x8b, 0x57, 0x37, 0x7a,
    0xfc, 0xc9, 0xdc, 0xe3, 0xfa, 0x5d, 0xb1, 0xf0,
    0x6f, 0x69, 0x85, 0xc4, 0x41, 0x4c, 0x0f, 0xcc,
    0x78, 0x00, 0x30, 0xf4, 0x9f, 0xef, 0x79, 0x1a,
    0x6c, 0x08, 0xed, 0xc2, 0xa3, 0x11, 0x08, 0x0c,
    0x37, 0x3f, 0x00, 0xe4, 0xb2, 0x04, 0x4a, 0x79,
    0xd8, 0x28, 0x60, 0xf0, 0x87, 0x1b, 0xc2, 0x59
};

static const uint8_t p_msg7[]={
    0x98, 0x70, 0x00, 0x76, 0x54, 0xeb, 0xc3, 0xd2,
    0x8f, 0x88, 0x3b, 0xb8, 0x32, 0xe0, 0xb3, 0x17,
    0x00, 0xf9, 0x23, 0xd9, 0xc9, 0xb1, 0x01, 0x68,
    0xe0, 0x60, 0x59, 0x71, 0xcf, 0xb9, 0x20, 0xe8,
    0x48, 0xf1, 0xc6, 0x4c, 0x5f, 0x24, 0x0a, 0x2c,
    0xf7, 0xf4, 0x12, 0xea, 0x7a, 0x73, 0xbb, 0xbf,
    0xce, 0x43, 0x2e, 0xff, 0x84, 0xfb, 0xb4, 0x9e,
    0x52, 0xcd, 0xcb, 0xf4, 0xc3, 0x66, 0x79, 0xbd,
    0x2d, 0x16, 0xe0, 0x64, 0xe4, 0x31, 0x13, 0x81,
    0xad, 0xb5, 0x28, 0xa0, 0x75, 0x2c, 0x8e, 0x44,
    0x43, 0xd4, 0xa1, 0x2b, 0x6c, 0xfe, 0x7c, 0xd4,
    0x06, 0xb4, 0x0e, 0x3f, 0x9e, 0x9e, 0x71, 0xf4,
    0x2e, 0x27, 0x76, 0x46, 0x49, 0xdb, 0x85, 0xd9,
    0x99, 0x13, 0xa4, 0x62, 0x8b, 0xd5, 0xd5, 0xae,
    0x49, 0xf6, 0xa5, 0xe6, 0xe9, 0x81, 0x02, 0x11,
    0xe3, 0x5d, 0x4d, 0xda, 0xc9, 0x29, 0xb0, 0x93
};

static const uint8_t p_msg8[]={
    0x4f, 0x56, 0x9d, 0x60, 0x40, 0x56, 0x63, 0xff,
    0xd4, 0x89, 0x37, 0x77, 0xcb, 0xc3, 0x71, 0x55,
    0xd4, 0x03, 0xe2, 0xb0, 0xf5, 0x48, 0x5d, 0xa4,
    0x2c, 0xa6, 0x75, 0x03, 0x57, 0x98, 0x89, 0x46,
    0x51, 0x98, 0xfe, 0xca, 0x5e, 0xed, 0xcc, 0x39,
    0xc9, 0xc5, 0x3c, 0x45, 0xcb, 0x83, 0xf0, 0x9d,
    0xaf, 0x5a, 0x23, 0x19, 0x34, 0x1b, 0x32, 0x38,
    0x33, 0x4b, 0x5b, 0xcd, 0x81, 0x79, 0xc5, 0xf5,
    0x17, 0xce, 0xc1, 0x4c, 0x70, 0xe6, 0x50, 0x61,
    0x33, 0xde, 0xe5, 0x67, 0x12, 0xaf, 0x6c, 0x2d,
    0xf2, 0xba, 0x8a, 0x50, 0x4c, 0xa4, 0x27, 0xaf,
    0xd3, 0x63, 0x2a, 0x1f, 0x57, 0x99, 0x83, 0x60,
    0xe9, 0x21, 0x6f, 0x50, 0x40, 0xe8, 0xf7, 0x5f,
    0x5b, 0xff, 0xba, 0x43, 0x68, 0xee, 0xed, 0xed,
    0xe5, 0x4a, 0xa0, 0xbb, 0x05, 0x8a, 0x43, 0xef,
    0x55, 0x16, 0x68, 0x60, 0x9f, 0xa1, 0xcb, 0x6f
};

static const uint8_t p_msg9[]={
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd
};

static const uint8_t p_msg10[]={
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd
};



static const uint8_t p_hmac1[]={
    0x1b, 0xa0, 0xe6, 0x6c, 0xf7, 0x2e, 0xfc, 0x34, 0x92, 0x07
};

static const uint8_t p_hmac2[]={
    0x00, 0x7e, 0x45, 0x04, 0x04, 0x1a, 0x12, 0xf9, 0xe3, 0x45
};

static const uint8_t p_hmac3[]={
    0x33, 0xf1, 0x7a, 0xc8, 0xa5, 0xc6, 0xb5,
    0x25, 0xdb, 0x8b, 0x86, 0x44, 0xb6, 0xab
};

static const uint8_t p_hmac4[]={
    0x90, 0x08, 0x42, 0xbb, 0x91, 0x6f, 0xb0,
    0xdf, 0xf7, 0xab, 0xe3, 0x7e, 0xd8, 0xf6
};

static const uint8_t p_hmac5[]={
    0x05, 0xd1, 0x24, 0x3e, 0x64, 0x65, 0xed, 0x96,
    0x20, 0xc9, 0xae, 0xc1, 0xc3, 0x51, 0xa1, 0x86
};

static const uint8_t p_hmac6[]={
    0xc4, 0x06, 0x14, 0x27, 0x76, 0x4f, 0x97, 0x94,
    0x68, 0xac, 0x42, 0x28, 0x91, 0xde, 0xa9, 0xca
};

static const uint8_t p_hmac7[]={
    0x79, 0xe2, 0x4a, 0x20, 0x3b, 0xf4, 0x20, 0x74,
    0xe7, 0x2c, 0x8b, 0x4a, 0x02, 0x22, 0xaf, 0xac,
    0xe3, 0xe8, 0xce, 0x7b, 0x40, 0x04, 0xce, 0xc2
};

static const uint8_t p_hmac8[]={
    0x24, 0x7e, 0xb5, 0x1a, 0x39, 0x7b, 0xa3, 0x69,
    0xec, 0xba, 0x43, 0xb9, 0x5a, 0x46, 0xa9, 0x33,
    0xcf, 0xf0, 0xb1, 0x00, 0x57, 0x14, 0xf0, 0xe5
};

static const uint8_t p_hmac9[]={
    0xfa, 0x73, 0xb0, 0x08, 0x9d, 0x56, 0xa2, 0x84,
    0xef, 0xb0, 0xf0, 0x75, 0x6c, 0x89, 0x0b, 0xe9,
    0xb1, 0xb5, 0xdb, 0xdd, 0x8e, 0xe8, 0x1a, 0x36,
    0x55, 0xf8, 0x3e, 0x33, 0xb2, 0x27, 0x9d, 0x39,
    0xbf, 0x3e, 0x84, 0x82, 0x79, 0xa7, 0x22, 0xc8,
    0x06, 0xb4, 0x85, 0xa4, 0x7e, 0x67, 0xc8, 0x07,
    0xb9, 0x46, 0xa3, 0x37, 0xbe, 0xe8, 0x94, 0x26,
    0x74, 0x27, 0x88, 0x59, 0xe1, 0x32, 0x92, 0xfb
};


static const uint8_t p_hmac10[]={
    0xb0, 0xba, 0x46, 0x56, 0x37, 0x45, 0x8c, 0x69,
    0x90, 0xe5, 0xa8, 0xc5, 0xf6, 0x1d, 0x4a, 0xf7,
    0xe5, 0x76, 0xd9, 0x7f, 0xf9, 0x4b, 0x87, 0x2d,
    0xe7, 0x6f, 0x80, 0x50, 0x36, 0x1e, 0xe3, 0xdb,
    0xa9, 0x1c, 0xa5, 0xc1, 0x1a, 0xa2, 0x5e, 0xb4,
    0xd6, 0x79, 0x27, 0x5c, 0xc5, 0x78, 0x80, 0x63,
    0xa5, 0xf1, 0x97, 0x41, 0x12, 0x0c, 0x4f, 0x2d,
    0xe2, 0xad, 0xeb, 0xeb, 0x10, 0xa2, 0x98, 0xdd
};

#ifdef UNUSED
static const unsigned char sha1_26byte_msg[26] = 
{0x44,0xd9,0x1d,0x3d,0x46,0x5a,0x41,0x11,0x46,0x2b,0xa0,0xc7,0xec,0x22,0x3d,0xa6,0x73,0x5f,0x4f,0x52,0x00,0x45,0x3c,0xf1,0x32,0xc3};

static const unsigned char sha256_26byte_msg[26] =
{0x47,0xc7,0x70,0xeb,0x45,0x49,0xb6,0xef,0xf6,0x38,0x1d,0x62,0xe9,0xbe,0xb4,0x64,0xcd,0x98,0xd3,0x41,0xcc,0x1c,0x09,0x98,0x1a,0x7a};
#endif

extern const uint8_t sha512_large_msg[];


void icum_hmac()
{
    int n;
    
    while(1)
    {
        printf("\n| select hmac test menu\n");
        printf("|------------------------------------------------|\n");
        printf("|1          : hmac_generation_test\n");
        printf("|2          : hmac_verify_test\n");
        printf("|3          : hmac_key_null_test\n");
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
            case 1: hmac_generation_test();            break;
            case 2: hmac_verify_test();                break;
            case 3: hmac_key_null_test();              break;
            
            default:
                goto end;
        }
        // restart life signal
        restart_life_signal();
end:
        ;
    }
}

static void hmac_generation_test1(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1, ai_loop2;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};
    
    printf("-----------------------------------------------------------------\n");
    printf("No1.hmac_generation_test1 \n");
    
    icum_hmac_import_test1();

    test_no = 1;
    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 0;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
        p_ISD->prm.HMAC.p_msg                 = p_msg1;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_generation_test1\n[Count : %d]", (ai_loop1 + 1));

        for (ai_loop2 = 0; ai_loop2 < p_ISD->prm.HMAC.hmac_size_in_bytes; ai_loop2++)
        {
            if (BIG_BUFFER[0][ai_loop2] != p_hmac1[ai_loop2])
            {
                printf("verify fault !\n");
                break;
            }
        }
        
        printf("calculated value:");
        PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
        
        printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

    }
}

static void hmac_generation_test2(){
    
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1, ai_loop2;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};
    
    printf("-----------------------------------------------------------------\n");
    printf("No2.hmac_generation_test \n");
    
    icum_hmac_import_test2();
        
    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 1;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
        p_ISD->prm.HMAC.p_msg                 = p_msg2;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_generation_test2\n[Count : %d]", (ai_loop1 + 1));

        for (ai_loop2 = 0; ai_loop2 < p_ISD->prm.HMAC.hmac_size_in_bytes; ai_loop2++)
        {
            if (BIG_BUFFER[0][ai_loop2] != p_hmac2[ai_loop2])
            {
                printf("verify fault !\n");
                break;
            }
        }

        printf("calculated value:");
        PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
        
        printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}

static void hmac_generation_test3(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1, ai_loop2;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};
    
    printf("-----------------------------------------------------------------\n");
    printf("No3.hmac_generation_test \n");
    
    icum_hmac_import_test3();
        
    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 2;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_224;
        p_ISD->prm.HMAC.p_msg                 = p_msg3;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 28;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_generation_test3\n[Count : %d]", (ai_loop1 + 1));

        for (ai_loop2 = 0; ai_loop2 < p_ISD->prm.HMAC.hmac_size_in_bytes; ai_loop2++)
        {
            if (BIG_BUFFER[0][ai_loop2] != p_hmac3[ai_loop2])
            {
                printf("verify fault !\n");
                break;
            }
        }

        printf("calculated value:");
        PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
        
        printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}

static void hmac_generation_test4(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1, ai_loop2;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No4.hmac_generation_test \n");

    icum_hmac_import_test4();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 3;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_224;
        p_ISD->prm.HMAC.p_msg                 = p_msg4;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 28;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_generation_test4\n[Count : %d]", (ai_loop1 + 1));

        for (ai_loop2 = 0; ai_loop2 < p_ISD->prm.HMAC.hmac_size_in_bytes; ai_loop2++)
        {
            if (BIG_BUFFER[0][ai_loop2] != p_hmac4[ai_loop2])
            {
                printf("verify fault !\n");
                break;
            }
        }

        printf("calculated value:");
        PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
        
        printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}

static void hmac_generation_test5(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1, ai_loop2;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No5.hmac_generation_test \n");
    icum_hmac_import_test5();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 0;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_256;
        p_ISD->prm.HMAC.p_msg                 = p_msg5;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 32;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_generation_test5\n[Count : %d]", (ai_loop1 + 1));

        for (ai_loop2 = 0; ai_loop2 < p_ISD->prm.HMAC.hmac_size_in_bytes; ai_loop2++)
        {
            if (BIG_BUFFER[0][ai_loop2] != p_hmac5[ai_loop2])
            {
                printf("verify fault !\n");
                break;
            }
        }

        printf("calculated value:");
        PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
        
        printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}

static void hmac_generation_test6(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1, ai_loop2;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No6.hmac_generation_test \n");
    icum_hmac_import_test6();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 1;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_256;
        p_ISD->prm.HMAC.p_msg                 = p_msg6;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 32;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_generation_test6\n[Count : %d]", (ai_loop1 + 1));

        for (ai_loop2 = 0; ai_loop2 < p_ISD->prm.HMAC.hmac_size_in_bytes; ai_loop2++)
        {
            if (BIG_BUFFER[0][ai_loop2] != p_hmac6[ai_loop2])
            {
                printf("verify fault !\n");
                break;
            }
        }

        printf("calculated value:");
        PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
        
        printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}

static void hmac_generation_test7(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1, ai_loop2;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No7.hmac_generation_test \n");
    icum_hmac_import_test7();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 2;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_384;
        p_ISD->prm.HMAC.p_msg                 = p_msg7;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 24;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_generation_test7\n[Count : %d]", (ai_loop1 + 1));

        for (ai_loop2 = 0; ai_loop2 < p_ISD->prm.HMAC.hmac_size_in_bytes; ai_loop2++)
        {
            if (BIG_BUFFER[0][ai_loop2] != p_hmac7[ai_loop2])
            {
                printf("verify fault !\n");
                break;
            }
        }

        printf("calculated value:");
        PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
        
        printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}

static void hmac_generation_test8(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1, ai_loop2;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No8.hmac_generation_test \n");
    icum_hmac_import_test8();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 3;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_384;
        p_ISD->prm.HMAC.p_msg                 = p_msg8;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 24;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_generation_test8\n[Count : %d]", (ai_loop1 + 1));

        for (ai_loop2 = 0; ai_loop2 < p_ISD->prm.HMAC.hmac_size_in_bytes; ai_loop2++)
        {
            if (BIG_BUFFER[0][ai_loop2] != p_hmac8[ai_loop2])
            {
                printf("verify fault !\n");
                break;
            }
        }

        printf("calculated value:");
        PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
        
        printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}

static void hmac_generation_test9(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1, ai_loop2;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No9.hmac_generation_test \n");

    icum_hmac_import_test9();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 0;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_512;
        p_ISD->prm.HMAC.p_msg                 = p_msg9;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 50;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 64;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_generation_test9\n[Count : %d]", (ai_loop1 + 1));

        for (ai_loop2 = 0; ai_loop2 < p_ISD->prm.HMAC.hmac_size_in_bytes; ai_loop2++)
        {
            if (BIG_BUFFER[0][ai_loop2] != p_hmac9[ai_loop2])
            {
                printf("verify fault !\n");
                break;
            }
        }

        printf("calculated value:");
        PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
        
        printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}

static void hmac_generation_test10(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1, ai_loop2;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No10.hmac_generation_test \n");

    icum_hmac_import_test10();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 1;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_512;
        p_ISD->prm.HMAC.p_msg                 = p_msg10;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 50;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 64;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_generation_test10\n[Count : %d]", (ai_loop1 + 1));

        for (ai_loop2 = 0; ai_loop2 < p_ISD->prm.HMAC.hmac_size_in_bytes; ai_loop2++)
        {
            if (BIG_BUFFER[0][ai_loop2] != p_hmac10[ai_loop2])
            {
                printf("verify fault !\n");
                break;
            }
        }

        printf("calculated value:");
        PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
        
        printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}

static void hmac_generation_error_test11(){
    r_icumif_isd_t     *p_ISD;
    
    printf("\n*------ Test No. %02d\n", test_no); test_no++;
    printf("hmac_generation_error_test11 \n");
    
    icum_hmac_import_test1();
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER,      0x00, SIZE_OF_ISD_BUFFER);
    memset(BIG_BUFFER[0],   0x00, SIZE_OF_BIG_BUFFER);
    
    p_ISD->service_id     = SERVICE_01_HMAC;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.HMAC.key_id                = 4;
    p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
    p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.HMAC.p_msg                 = p_msg1;
    p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD->prm.HMAC.job_slice             = job_slice[JOB_H];
    p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("calculated value:");
    PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
    
    printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

static void hmac_generation_error_test12(){
    r_icumif_isd_t     *p_ISD;
    
    printf("\n*------ Test No. 42\n");
    printf("hmac_generation_error_test12 \n");
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER,      0x00, SIZE_OF_ISD_BUFFER);
    memset(BIG_BUFFER[0],   0x00, SIZE_OF_BIG_BUFFER);
    
    p_ISD->service_id     = SERVICE_01_HMAC;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.HMAC.key_id                = 3;
    p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
    p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.HMAC.p_msg                 = p_msg1;
    p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD->prm.HMAC.job_slice             = 0;
    
    p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

static void hmac_generation_error_test13(){
    r_icumif_isd_t     *p_ISD;
    
    test_no = 43;
    printf("\n*------ Test No. %02d\n", test_no); test_no++;
    printf("hmac_generation_error_test13 \n");
    
    icum_hmac_import_test1();
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER,      0x00, SIZE_OF_ISD_BUFFER);
    memset(BIG_BUFFER[0],   0x00, SIZE_OF_BIG_BUFFER);
    
    p_ISD->service_id     = SERVICE_01_HMAC;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.HMAC.key_id                = 0;
    p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
    p_ISD->prm.HMAC.hash_primitive        = (r_hash_primitive_t)100;
    p_ISD->prm.HMAC.p_msg                 = p_msg1;
    p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD->prm.HMAC.job_slice             = job_slice[JOB_H];
    p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("calculated value:");
    PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
    
    printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

static void hmac_generation_error_test14(){
    r_icumif_isd_t     *p_ISD;
    
    uint8_t p_error_msg[]={
    0xfc, 0xd6, 0xd9, 0x8b, 0xef, 0x45, 0xed, 0x68,
    0x50, 0x80, 0x6e, 0x96, 0xf2, 0x55, 0xfa, 0x0c,
    0x81, 0x14, 0xb7, 0x28, 0x73, 0xab, 0xe8, 0xf4,
    0x3c, 0x10, 0xbe, 0xa7, 0xc1, 0xdf, 0x70, 0x6f,
    0x10, 0x45, 0x8e, 0x6d, 0x4e, 0x1c, 0x92, 0x01,
    0xf0, 0x57, 0xb8, 0x49, 0x2f, 0xa1, 0x0f, 0xe4,
    0xb5, 0x41, 0xd0, 0xfc, 0x9d, 0x41, 0xef, 0x83,
    0x9a, 0xcf, 0xf1, 0xbc, 0x76, 0xe3, 0xfd, 0xfe,
    0xbf, 0x22, 0x35, 0xb5, 0xbd, 0x03, 0x47, 0xa9,
    0xa6, 0x30, 0x3e, 0x83, 0x15, 0x2f, 0x9f, 0x8d,
    0xb9, 0x41, 0xb1, 0xb9, 0x4a, 0x8a, 0x1c, 0xe5,
    0xc2, 0x73, 0xb5, 0x5d, 0xc9, 0x4d, 0x99, 0xa1,
    0x71, 0x37, 0x79, 0x69, 0x23, 0x41, 0x34, 0xe7,
    0xda, 0xd1, 0xab, 0x4c, 0x8e, 0x46, 0xd1, 0x8d,
    0xf4, 0xdc, 0x01, 0x67, 0x64, 0xcf, 0x95, 0xa1,
    0x1a, 0xc4, 0xb4, 0x91, 0xa2, 0x64, 0x6b, 0xe1
};
    
    printf("\n*------ Test No. %02d\n", test_no); test_no++;
    printf("hmac_generation_error_test14 \n");
    
    icum_hmac_import_test1();
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER,      0x00, SIZE_OF_ISD_BUFFER);
    memset(BIG_BUFFER[0],   0x00, SIZE_OF_BIG_BUFFER);
    
    p_ISD->service_id     = SERVICE_01_HMAC;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.HMAC.key_id                = 0;
    p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
    p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.HMAC.p_msg                 = p_error_msg;
    p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD->prm.HMAC.job_slice             = 0;
    p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("calculated value:");
    PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
    
    printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

static void hmac_generation_error_test15(){
    r_icumif_isd_t     *p_ISD;
    
    uint8_t p_error_mac[64] = {0};
    
    printf("\n*------ Test No. %02d\n", test_no); test_no++;
    printf("hmac_generation_error_test15 \n");
    
    icum_hmac_import_test1();
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER,      0x00, SIZE_OF_ISD_BUFFER);
    memset(BIG_BUFFER[0],   0x00, SIZE_OF_BIG_BUFFER);
    
    p_ISD->service_id     = SERVICE_01_HMAC;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.HMAC.key_id                = 0;
    p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
    p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.HMAC.p_msg                 = p_msg1;
    p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD->prm.HMAC.job_slice             = 0;
    p_ISD->prm.HMAC.p_hmac                = p_error_mac;
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("calculated value:");
    PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
    
    printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

static void hmac_generation_error_test16(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop;

    printf("error_No16.hmac_generation_test \n");

    icum_hmac_import_test1();

    for (ai_loop = 0; ai_loop < 3; ai_loop++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER,      0x00, SIZE_OF_ISD_BUFFER);
        memset(BIG_BUFFER[0],   0x00, SIZE_OF_BIG_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 0;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
        p_ISD->prm.HMAC.p_msg                 = p_msg1;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_generation_error_test16 \n[Count : %d]\n", (ai_loop + 1));

        if (ai_loop == 0)
        {
           p_ISD->prm.HMAC.msg_size_in_bytes     = 0;
           p_ISD->prm.HMAC.p_msg                 = 0;
           printf("msg_size = 0\n");
        }
        else
        {
            p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
            if (ai_loop == 1)
            {
                p_ISD->prm.HMAC.hmac_size_in_bytes    = 0;
                printf("hmac_size = 0\n");
            }
            else
            {
                p_ISD->prm.HMAC.hmac_size_in_bytes    = 129;
                printf("hmac_size = 129\n");
            }
        }
        p_ISD->prm.HMAC.job_slice             = 0;
        p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        
        printf("calculated value:");
        PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
        
        printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}

static void hmac_generation_error_test17(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop;
    unsigned int        err_adrs[] = {ADDR_UPPER_CRAM-127, ADDR_UPPER_CRAM-126, ADDR_UPPER_CRAM-9, ADDR_UPPER_CRAM-8};

    printf("error_No17.hmac_generation_test \n");

    icum_hmac_import_test1();

    for (ai_loop = 0; ai_loop < 4; ai_loop++)
    {

        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER,      0x00, SIZE_OF_ISD_BUFFER);
        memset(BIG_BUFFER[0],   0x00, SIZE_OF_BIG_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;

        p_ISD->prm.HMAC.key_id                = 0;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
        if (ai_loop > 1)
        {
            p_ISD->prm.HMAC.p_msg             = p_msg1;
            p_ISD->prm.HMAC.p_hmac            = (void *)err_adrs[ai_loop];
        }
        else
        {
            p_ISD->prm.HMAC.p_msg             = (void *)err_adrs[ai_loop];
            p_ISD->prm.HMAC.p_hmac            = BIG_BUFFER[0];
        }
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
        p_ISD->prm.HMAC.job_slice             = 0;

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_generation_error_test17 \n[Count : %d]\n", (ai_loop + 1));

        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("calculated value:");
        PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
        
        printf("\nresult: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}

static void hmac_generation_test18(){
    r_icumif_isd_t     *p_ISD;
    r_icumif_isd_t     *p_ISD2;

    printf("No18.hmac_generation_test \n");

    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD2  = p_ISD + 1;
    memset(ISD_BUFFER,      0x00, SIZE_OF_ISD_BUFFER * 2);
    memset(BIG_BUFFER[0],   0x00, SIZE_OF_BIG_BUFFER);

    p_ISD->service_id     = SERVICE_01_HMAC;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    p_ISD->prm.HMAC.key_id                = 0;
    p_ISD->prm.HMAC.hmac_operation        = HMAC_GENERATION;
    p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.HMAC.p_msg                 = p_msg1;
    p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD->prm.HMAC.job_slice             = 0;
    p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];

    p_ISD2->service_id     = SERVICE_01_HMAC;
    p_ISD2->p_callbackfunc = 0;
    p_ISD2->job_id         = 0;
    p_ISD2->prm.HMAC.key_id                = 1;
    p_ISD2->prm.HMAC.hmac_operation        = HMAC_GENERATION;
    p_ISD2->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
    p_ISD2->prm.HMAC.p_msg                 = p_msg1;
    p_ISD2->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD2->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD2->prm.HMAC.job_slice             = 0;
    p_ISD2->prm.HMAC.p_hmac                = BIG_BUFFER[0];

    printf("\n*------ Test No. %02d\n", test_no); test_no++;
    printf("hmac_generation_test18\n");

    vrun_chrono_start();
    trigger_2service_and_wait(p_ISD, p_ISD2);
    vrun_chrono_stop();

    printf("calculated value1:");
    PMEM(p_ISD->prm.HMAC.p_hmac, p_ISD->prm.HMAC.hmac_size_in_bytes);
    printf("\ncalculated value2:");
    PMEM(p_ISD2->prm.HMAC.p_hmac, p_ISD2->prm.HMAC.hmac_size_in_bytes);

    printf("\nresult1: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    printf("result2: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD2->service_result));
}


static void hmac_generation_test19(){
    r_icumif_isd_t     *p_ISD;
    r_icumif_isd_t     *p_ISD2;
    r_icumif_isd_t     *p_ISD3;

    printf("No19.hmac_generation_test \n");

    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD2  = p_ISD + 1;
    p_ISD3  = p_ISD2 + 1;
    memset(ISD_BUFFER,      0x00, SIZE_OF_ISD_BUFFER);
    memset(BIG_BUFFER[0],   0x00, SIZE_OF_BIG_BUFFER);

    p_ISD->service_id                      = SERVICE_01_HASH;
    p_ISD->p_callbackfunc                  = 0;
    p_ISD->job_id                          = 0;
    p_ISD->prm.HASH.p_message              = sha512_large_msg;
    p_ISD->prm.HASH.message_size_in_bytes  = 12800;
    p_ISD->prm.HASH.hash_primitive         = HASH_PRIMITIVE_SHA2_512;
    p_ISD->prm.HASH.job_slice              = 10;
    p_ISD->prm.HASH.job_cycle              = JOB_ALL_AT_ONCE;
    p_ISD->prm.HASH.p_hash                 = BIG_BUFFER[0];

    p_ISD2->service_id                     = SERVICE_01_HASH;
    p_ISD2->p_callbackfunc                 = 0;
    p_ISD2->job_id                         = 0;
    p_ISD2->prm.HASH.p_message             = sha512_large_msg;
    p_ISD2->prm.HASH.message_size_in_bytes = 12800;
    p_ISD2->prm.HASH.hash_primitive        = HASH_PRIMITIVE_SHA2_512;
    p_ISD2->prm.HASH.job_slice             = 10;
    p_ISD2->prm.HASH.job_cycle             = JOB_ALL_AT_ONCE;
    p_ISD2->prm.HASH.p_hash                = BIG_BUFFER[1];

    p_ISD3->service_id                     = SERVICE_01_HMAC;
    p_ISD3->p_callbackfunc                 = 0;
    p_ISD3->job_id                         = 0;
    p_ISD3->prm.HMAC.key_id                = 1;
    p_ISD3->prm.HMAC.hmac_operation        = HMAC_GENERATION;
    p_ISD3->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
    p_ISD3->prm.HMAC.p_msg                 = p_msg1;
    p_ISD3->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD3->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD3->prm.HMAC.job_slice             = 0;
    p_ISD3->prm.HMAC.p_hmac                = BIG_BUFFER[0];

    printf("\n*------ Test No. %02d\n", test_no); test_no++;
    printf("hmac_generation_test19\n");

    vrun_chrono_start();
    trigger_3service_and_wait(p_ISD, p_ISD2, p_ISD3);
    vrun_chrono_stop();

    if(p_ISD->service_result  != SERV_OK) {printf("HASH 1 ERROR \n"); goto end;}
    if(p_ISD2->service_result != SERV_OK) {printf("HASH 2 ERROR \n"); goto end;}

    printf("calculated value:");
    PMEM(p_ISD3->prm.HMAC.p_hmac, p_ISD3->prm.HMAC.hmac_size_in_bytes);

    printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD3->service_result));
end:
    ;
}


static void hmac_generation_error_test20(){
    r_icumif_isd_t     *p_ISD;

    printf("No18.hmac_generation_test \n");


    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER,      0x00, SIZE_OF_ISD_BUFFER * 2);

    p_ISD->service_id     = SERVICE_01_HMAC;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    p_ISD->prm.HMAC.key_id                = 0;
    p_ISD->prm.HMAC.hmac_operation        = (hmac_operation_t)(HMAC_VERIFICATION + 1);
    p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.HMAC.p_msg                 = p_msg1;
    p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD->prm.HMAC.job_slice             = 0;
    p_ISD->prm.HMAC.p_hmac                = BIG_BUFFER[0];

    printf("\n*------ Test No. %02d\n", test_no); test_no++;
    printf("hmac_generation_error_test20\n");

    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();

    printf("\nresult1: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}


static void hmac_verify_test1(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("No1.hmac_verify_test\n");
    
    icum_hmac_import_test1();

    test_no = 1;
    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 0;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
        p_ISD->prm.HMAC.p_msg                 = p_msg1;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac1;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_verify_test1\n[Count : %d]", (ai_loop1 + 1));

        printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        
        if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
        {
            printf("VERIFICATION_PASS\n\n");
        }
        else{
            printf("VERIFICATION_FAIL\n\n");
        }
    }
}

static void hmac_verify_test2(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No2.hmac_verify_test\n");
    
    icum_hmac_import_test2();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 1;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
        p_ISD->prm.HMAC.p_msg                 = p_msg2;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac2;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_verify_test2\n[Count : %d]", (ai_loop1 + 1));

        printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        
        if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
        {
            printf("VERIFICATION_PASS\n\n");
        }
        else{
            printf("VERIFICATION_FAIL\n\n");
        }
    }
}

static void hmac_verify_test3(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No3.hmac_verify_test\n");
    
    icum_hmac_import_test3();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 2;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_224;
        p_ISD->prm.HMAC.p_msg                 = p_msg3;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac3;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 28;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_verify_test3\n[Count : %d]", (ai_loop1 + 1));

        printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        
        if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
        {
            printf("VERIFICATION_PASS\n\n");
        }
        else{
            printf("VERIFICATION_FAIL\n\n");
        }
    }
}

static void hmac_verify_test4(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No4.hmac_verify_test\n");
    
    icum_hmac_import_test4();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 3;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_224;
        p_ISD->prm.HMAC.p_msg                 = p_msg4;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac4;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 28;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_verify_test4\n[Count : %d]", (ai_loop1 + 1));

        printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        
        if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
        {
            printf("VERIFICATION_PASS\n\n");
        }
        else{
            printf("VERIFICATION_FAIL\n\n");
        }
    }
}

static void hmac_verify_test5(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No5.hmac_verify_test\n");
    
    icum_hmac_import_test5();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 0;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_256;
        p_ISD->prm.HMAC.p_msg                 = p_msg5;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac5;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 32;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_verify_test5\n[Count : %d]", (ai_loop1 + 1));

        printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        
        if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
        {
            printf("VERIFICATION_PASS\n\n");
        }
        else{
            printf("VERIFICATION_FAIL\n\n");
        }
    }
}

static void hmac_verify_test6(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No6.hmac_verify_test\n");
    
    icum_hmac_import_test6();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 1;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_256;
        p_ISD->prm.HMAC.p_msg                 = p_msg6;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac6;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 32;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_verify_test6\n[Count : %d]", (ai_loop1 + 1));

        printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        
        if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
        {
            printf("VERIFICATION_PASS\n\n");
        }
        else{
            printf("VERIFICATION_FAIL\n\n");
        }
    }
}

static void hmac_verify_test7(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No7.hmac_verify_test\n");
    
    icum_hmac_import_test7();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 2;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_384;
        p_ISD->prm.HMAC.p_msg                 = p_msg7;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac7;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 24;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_verify_test7\n[Count : %d]", (ai_loop1 + 1));

        printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        
        if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
        {
            printf("VERIFICATION_PASS\n\n");
        }
        else{
            printf("VERIFICATION_FAIL\n\n");
        }
    }
}

static void hmac_verify_test8(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No8.hmac_verify_test\n");
    
    icum_hmac_import_test8();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 3;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_384;
        p_ISD->prm.HMAC.p_msg                 = p_msg8;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac8;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 24;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_verify_test8\n[Count : %d]", (ai_loop1 + 1));

        printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        
        if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
        {
            printf("VERIFICATION_PASS\n\n");
        }
        else{
            printf("VERIFICATION_FAIL\n\n");
        }
    }
}

static void hmac_verify_test9(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No9.hmac_verify_test\n");
    
    icum_hmac_import_test9();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 0;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_512;
        p_ISD->prm.HMAC.p_msg                 = p_msg9;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 50;
        p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac9;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 64;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_verify_test9\n[Count : %d]", (ai_loop1 + 1));

        printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        
        if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
        {
            printf("VERIFICATION_PASS\n\n");
        }
        else{
            printf("VERIFICATION_FAIL\n\n");
        }
    }
}

static void hmac_verify_test10(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop1;
    r_job_slice_t       ajob_slice[] = {0, 1, 64, 256};

    printf("-----------------------------------------------------------------\n");
    printf("No10.hmac_verify_test\n");
    
    icum_hmac_import_test10();

    for (ai_loop1 = 0; ai_loop1 < 4; ai_loop1++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 1;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_512;
        p_ISD->prm.HMAC.p_msg                 = p_msg10;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 50;
        p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac10;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 64;
        p_ISD->prm.HMAC.job_slice             = ajob_slice[(ai_loop1 % 4)];
        p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_verify_test10\n[Count : %d]", (ai_loop1 + 1));
    
        printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        
        if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
        {
            printf("VERIFICATION_PASS\n\n");
        }
        else{
            printf("VERIFICATION_FAIL\n\n");
        }
    }
}

static void hmac_verify_error_test11(){
    r_icumif_isd_t     *p_ISD;
    
    printf("-----------------------------------------------------------------\n");
    printf("hmac_verification_error_test11\n");
    
    icum_hmac_import_test1();
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id     = SERVICE_01_HMAC;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.HMAC.key_id                = 4;
    p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
    p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.HMAC.p_msg                 = p_msg1;
    p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac1;
    p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD->prm.HMAC.job_slice             = 0;
    p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("\n*------ Test No. %02d\n", test_no); test_no++;
    printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    
    if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
    {
        printf("VERIFICATION_PASS\n\n");
    }
    else{
        printf("VERIFICATION_FAIL\n\n");
    }
}

static void hmac_verify_error_test12(){
    r_icumif_isd_t     *p_ISD;
    
    printf("-----------------------------------------------------------------\n");
    printf("hmac_verification_error_test12\n");
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id     = SERVICE_01_HMAC;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.HMAC.key_id                = 3;
    p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
    p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.HMAC.p_msg                 = p_msg1;
    p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac1;
    p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD->prm.HMAC.job_slice             = 0;
    p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("\n*------ Test No. 42\n");
    printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    
    if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
    {
        printf("VERIFICATION_PASS\n\n");
    }
    else{
        printf("VERIFICATION_FAIL\n\n");
    }
}

static void hmac_verify_error_test13(){
    r_icumif_isd_t     *p_ISD;
    
    printf("-----------------------------------------------------------------\n");
    printf("hmac_verification_error_test13\n");
    
    icum_hmac_import_test1();
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id     = SERVICE_01_HMAC;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.HMAC.key_id                = 0;
    p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
    p_ISD->prm.HMAC.hash_primitive        = (r_hash_primitive_t)100;
    p_ISD->prm.HMAC.p_msg                 = p_msg1;
    p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac1;
    p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD->prm.HMAC.job_slice             = 0;
    p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    test_no = 43;
    printf("\n*------ Test No. %02d\n", test_no); test_no++;
    printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    
    if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
    {
        printf("VERIFICATION_PASS\n\n");
    }
    else{
        printf("VERIFICATION_FAIL\n\n");
    }
}

static void hmac_verify_error_test14(){
    r_icumif_isd_t     *p_ISD;
    uint8_t err_p_msg1[]={
        0xfc, 0xd6, 0xd9, 0x8b, 0xef, 0x45, 0xed, 0x68,
        0x50, 0x80, 0x6e, 0x96, 0xf2, 0x55, 0xfa, 0x0c,
        0x81, 0x14, 0xb7, 0x28, 0x73, 0xab, 0xe8, 0xf4,
        0x3c, 0x10, 0xbe, 0xa7, 0xc1, 0xdf, 0x70, 0x6f,
        0x10, 0x45, 0x8e, 0x6d, 0x4e, 0x1c, 0x92, 0x01,
        0xf0, 0x57, 0xb8, 0x49, 0x2f, 0xa1, 0x0f, 0xe4,
        0xb5, 0x41, 0xd0, 0xfc, 0x9d, 0x41, 0xef, 0x83,
        0x9a, 0xcf, 0xf1, 0xbc, 0x76, 0xe3, 0xfd, 0xfe,
        0xbf, 0x22, 0x35, 0xb5, 0xbd, 0x03, 0x47, 0xa9,
        0xa6, 0x30, 0x3e, 0x83, 0x15, 0x2f, 0x9f, 0x8d,
        0xb9, 0x41, 0xb1, 0xb9, 0x4a, 0x8a, 0x1c, 0xe5,
        0xc2, 0x73, 0xb5, 0x5d, 0xc9, 0x4d, 0x99, 0xa1,
        0x71, 0x37, 0x79, 0x69, 0x23, 0x41, 0x34, 0xe7,
        0xda, 0xd1, 0xab, 0x4c, 0x8e, 0x46, 0xd1, 0x8d,
        0xf4, 0xdc, 0x01, 0x67, 0x64, 0xcf, 0x95, 0xa1,
        0x1a, 0xc4, 0xb4, 0x91, 0xa2, 0x64, 0x6b, 0xe1
    };

    printf("\n*------ Test No. %02d\n", test_no); test_no++;
    printf("hmac_verification_error_test14\n");
    
    icum_hmac_import_test1();
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id     = SERVICE_01_HMAC;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.HMAC.key_id                = 0;
    p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
    p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.HMAC.p_msg                 = err_p_msg1;
    p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac1;
    p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD->prm.HMAC.job_slice             = 0;
    p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    
    if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
    {
        printf("VERIFICATION_PASS\n\n");
    }
    else{
        printf("VERIFICATION_FAIL\n\n");
    }
}

static void hmac_verify_error_test15(){
    r_icumif_isd_t     *p_ISD;
    
    uint8_t err_p_hmac1[]={
        0x1b, 0xa0, 0xe6, 0x6c, 0xf7, 0x2e, 0xfc, 0x34, 0x92, 0x07
    };
    
    printf("\n*------ Test No. %02d\n", test_no); test_no++;
    printf("hmac_verification_error_test15\n");
    
    icum_hmac_import_test1();
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id     = SERVICE_01_HMAC;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.HMAC.key_id                = 0;
    p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
    p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.HMAC.p_msg                 = p_msg1;
    p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
    p_ISD->prm.HMAC.p_hmac                = (uint8_t *)err_p_hmac1;
    p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
    p_ISD->prm.HMAC.job_slice             = 0;
    p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    
    if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
    {
        printf("VERIFICATION_PASS\n\n");
    }
    else{
        printf("VERIFICATION_FAIL\n\n");
    }
}


static void hmac_verify_error_test16(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop;

    printf("-----------------------------------------------------------------\n");
    printf("error_No16.hmac_verify_test\n");
    
    icum_hmac_import_test1();

    for (ai_loop = 0; ai_loop < 3; ai_loop++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 0;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA1;
        p_ISD->prm.HMAC.p_msg                 = p_msg1;
        p_ISD->prm.HMAC.msg_size_in_bytes     = 10;
        p_ISD->prm.HMAC.p_hmac                = (uint8_t *)p_hmac1;
        p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_verification_error_test16 \n[Count : %d]\n", (ai_loop + 1));

        if (ai_loop == 0)
        {
            p_ISD->prm.HMAC.msg_size_in_bytes     = 0;
            p_ISD->prm.HMAC.hmac_size_in_bytes    = 10;
            printf("msg_size = 0\n");
        }
        else
        {
            p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
            if (ai_loop == 1)
            {
                p_ISD->prm.HMAC.hmac_size_in_bytes    = 0;
                printf("hmac_size = 0\n");
            }
            else
            {
                p_ISD->prm.HMAC.hmac_size_in_bytes    = 129;
                printf("hmac_size = 129\n");
            }
        }
        p_ISD->prm.HMAC.job_slice             = 0;
        
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        
        printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        
        if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
        {
            printf("VERIFICATION_PASS\n\n");
        }
        else{
            printf("VERIFICATION_FAIL\n\n");
        }
    }
}

static void hmac_verify_error_test17(){
    r_icumif_isd_t     *p_ISD;
    unsigned int        ai_loop;
    unsigned int        err_adrs[] = {ADDR_UPPER_CRAM-127, ADDR_UPPER_CRAM-126, ADDR_UPPER_CRAM-23, ADDR_UPPER_CRAM-22};

    printf("-----------------------------------------------------------------\n");
    printf("error_No17.hmac_verify_test\n");
    
    icum_hmac_import_test1();

    for (ai_loop = 0; ai_loop < 4; ai_loop++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
        
        p_ISD->service_id     = SERVICE_01_HMAC;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        
        p_ISD->prm.HMAC.key_id                = 0;
        p_ISD->prm.HMAC.hmac_operation        = HMAC_VERIFICATION;
        p_ISD->prm.HMAC.hash_primitive        = HASH_PRIMITIVE_SHA2_512;
        if (ai_loop > 1)
        {
            p_ISD->prm.HMAC.p_msg             = p_msg1;
            p_ISD->prm.HMAC.p_hmac            = (void *)err_adrs[ai_loop];
        }
        else
        {
            p_ISD->prm.HMAC.p_msg             = (void *)err_adrs[ai_loop];
            p_ISD->prm.HMAC.p_hmac            = BIG_BUFFER[0];
        }
        p_ISD->prm.HMAC.msg_size_in_bytes     = 128;
        p_ISD->prm.HMAC.hmac_size_in_bytes    = 24;
        p_ISD->prm.HMAC.job_slice             = 0;
        p_ISD->prm.HMAC.verification_result   = VERIFICATION_FAIL;

        printf("\n*------ Test No. %02d\n", test_no); test_no++;
        printf("hmac_verification_error_test17 \n[Count : %d]\n", (ai_loop + 1));

       vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        
        printf("\nresult: %s\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        
        if(p_ISD->prm.HMAC.verification_result == VERIFICATION_PASS)
        {
            printf("VERIFICATION_PASS\n\n");
        }
        else{
            printf("VERIFICATION_FAIL\n\n");
        }
    }
}

void hmac_generation_test(){
    hmac_generation_test1();
    hmac_generation_test2();
    hmac_generation_test3();
    hmac_generation_test4();
    hmac_generation_test5();
    hmac_generation_test6();
    hmac_generation_test7();
    hmac_generation_test8();
    hmac_generation_test9();
    hmac_generation_test10();
    hmac_generation_error_test11();
    hmac_generation_error_test13();
    hmac_generation_error_test14();
    hmac_generation_error_test15();
    hmac_generation_error_test16();
    hmac_generation_error_test17();
    hmac_generation_test18();
    hmac_generation_test19();
    hmac_generation_error_test20();
}

void hmac_verify_test(){
    hmac_verify_test1();
    hmac_verify_test2();
    hmac_verify_test3();
    hmac_verify_test4();
    hmac_verify_test5();
    hmac_verify_test6();
    hmac_verify_test7();
    hmac_verify_test8();
    hmac_verify_test9();
    hmac_verify_test10();
    hmac_verify_error_test11();
    hmac_verify_error_test13();
    hmac_verify_error_test14();
    hmac_verify_error_test15();
    hmac_verify_error_test16();
    hmac_verify_error_test17();
}

static void hmac_key_null_test(){
    hmac_generation_error_test12();
    hmac_verify_error_test12();
}
