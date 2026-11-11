/******************************************************************************/
/* Component Name   heat_run_parallel                                         */
/******************************************************************************/
/* Product      : ICU-M Low Level Driver (Demo Part)                          */
/******************************************************************************/
/***********************************************************************************************************************
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
* 
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* www.renesas.com/disclaimer
*
* Copyright (C) 2021-2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

#if !defined(PARALLEL_PE_HEAT_RUN_H)
#define PARALLEL_PE_HEAT_RUN_H

// ********************************************************************************
// Headers
// ********************************************************************************
#include "renesas_types.h"
#include "r_icumif_api.h"
#include "r_icumif_pub.h"

// ********************************************************************************
// Resister Definition
// ********************************************************************************

// ********************************************************************************
// Define
// ********************************************************************************
/* #define AES_CONTEXT_CHECK */ 

// ********************************************************************************
// Test Data
// ********************************************************************************
static const uint8_t test_msg_PSS_11[] = {
    0xcd, 0xc8, 0x7d, 0xa2, 0x23, 0xd7, 0x86, 0xdf, 0x3b, 0x45, 0xe0, 0xbb, 0xbc, 0x72, 0x13, 0x26,
    0xd1, 0xee, 0x2a, 0xf8, 0x06, 0xcc, 0x31, 0x54, 0x75, 0xcc, 0x6f, 0x0d, 0x9c, 0x66, 0xe1, 0xb6,
    0x23, 0x71, 0xd4, 0x5c, 0xe2, 0x39, 0x2e, 0x1a, 0xc9, 0x28, 0x44, 0xc3, 0x10, 0x10, 0x2f, 0x15,
    0x6a, 0x0d, 0x8d, 0x52, 0xc1, 0xf4, 0xc4, 0x0b, 0xa3, 0xaa, 0x65, 0x09, 0x57, 0x86, 0xcb, 0x76,
    0x97, 0x57, 0xa6, 0x56, 0x3b, 0xa9, 0x58, 0xfe, 0xd0, 0xbc, 0xc9, 0x84, 0xe8, 0xb5, 0x17, 0xa3,
    0xd5, 0xf5, 0x15, 0xb2, 0x3b, 0x8a, 0x41, 0xe7, 0x4a, 0xa8, 0x67, 0x69, 0x3f, 0x90, 0xdf, 0xb0,
    0x61, 0xa6, 0xe8, 0x6d, 0xfa, 0xae, 0xe6, 0x44, 0x72, 0xc0, 0x0e, 0x5f, 0x20, 0x94, 0x57, 0x29,
    0xcb, 0xeb, 0xe7, 0x7f, 0x06, 0xce, 0x78, 0xe0, 0x8f, 0x40, 0x98, 0xfb, 0xa4, 0x1f, 0x9d, 0x61,
    0x93, 0xc0, 0x31, 0x7e, 0x8b, 0x60, 0xd4, 0xb6, 0x08, 0x4a, 0xcb, 0x42, 0xd2, 0x9e, 0x38, 0x08,
    0xa3, 0xbc, 0x37, 0x2d, 0x85, 0xe3, 0x31, 0x17, 0x0f, 0xcb, 0xf7, 0xcc, 0x72, 0xd0, 0xb7, 0x1c,
    0x29, 0x66, 0x48, 0xb3, 0xa4, 0xd1, 0x0f, 0x41, 0x62, 0x95, 0xd0, 0x80, 0x7a, 0xa6, 0x25, 0xca,
    0xb2, 0x74, 0x4f, 0xd9, 0xea, 0x8f, 0xd2, 0x23, 0xc4, 0x25, 0x37, 0x02, 0x98, 0x28, 0xbd, 0x16,
    0xbe, 0x02, 0x54, 0x6f, 0x13, 0x0f, 0xd2, 0xe3, 0x3b, 0x93, 0x6d, 0x26, 0x76, 0xe0, 0x8a, 0xed,
    0x1b, 0x73, 0x31, 0x8b, 0x75, 0x0a, 0x01, 0x67, 0xd0
};
#pragma alignvar (4)
static const uint8_t test_sig_PSS_11[] = {
    "9074308FB598E9701B2294388E52F971FAAC2B60A5145AF185DF5287B5ED2887"
    "E57CE7FD44DC8634E407C8E0E4360BC226F3EC227F9D9E54638E8D31F5051215"
    "DF6EBB9C2F9579AA77598A38F914B5B9C1BD83C4E2F9F382A0D0AA3542FFEE65"
    "984A601BC69EB28DEB27DCA12C82C2D4C3F66CD500F1FF2B994D8A4E30CBB33C"
};
#pragma alignvar (4)
static const uint8_t test_sig_PSS_11_plain[] = {
    0x90, 0x74, 0x30, 0x8f, 0xb5, 0x98, 0xe9, 0x70, 0x1b, 0x22, 0x94, 0x38, 0x8e, 0x52, 0xf9, 0x71,
    0xfa, 0xac, 0x2b, 0x60, 0xa5, 0x14, 0x5a, 0xf1, 0x85, 0xdf, 0x52, 0x87, 0xb5, 0xed, 0x28, 0x87,
    0xe5, 0x7c, 0xe7, 0xfd, 0x44, 0xdc, 0x86, 0x34, 0xe4, 0x07, 0xc8, 0xe0, 0xe4, 0x36, 0x0b, 0xc2,
    0x26, 0xf3, 0xec, 0x22, 0x7f, 0x9d, 0x9e, 0x54, 0x63, 0x8e, 0x8d, 0x31, 0xf5, 0x05, 0x12, 0x15,
    0xdf, 0x6e, 0xbb, 0x9c, 0x2f, 0x95, 0x79, 0xaa, 0x77, 0x59, 0x8a, 0x38, 0xf9, 0x14, 0xb5, 0xb9,
    0xc1, 0xbd, 0x83, 0xc4, 0xe2, 0xf9, 0xf3, 0x82, 0xa0, 0xd0, 0xaa, 0x35, 0x42, 0xff, 0xee, 0x65,
    0x98, 0x4a, 0x60, 0x1b, 0xc6, 0x9e, 0xb2, 0x8d, 0xeb, 0x27, 0xdc, 0xa1, 0x2c, 0x82, 0xc2, 0xd4,
    0xc3, 0xf6, 0x6c, 0xd5, 0x00, 0xf1, 0xff, 0x2b, 0x99, 0x4d, 0x8a, 0x4e, 0x30, 0xcb, 0xb3, 0x3c
};

#pragma alignvar (4)
static const uint8_t test_cmac_msg_512bits[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, 
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51, 
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef, 
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};
#pragma alignvar (4)
static const uint8_t test_cmac_value_128bits[] = {
    0x51, 0xf0, 0xbe, 0xbf, 0x7e, 0x3b, 0x9d, 0x92, 0xfc, 0x49, 0x74, 0x17, 0x79, 0x36, 0x3c, 0xfe
};

#pragma alignvar (4)
static const uint8_t test_cmac_value_LOREM_IPSUM[] = {
    0x1e, 0x58, 0x08, 0xbd, 0x04, 0xc5, 0x7d, 0xe6, 0xc6, 0xc2, 0x96, 0x76, 0x78, 0x6d, 0x63, 0x01
};

#pragma alignvar (4)
static const uint8_t test_cmac_value_LOREM_IPSUM_1279bit[] = {
    0x8D, 0x09, 0xF9, 0xC8, 0x75, 0x36, 0x49, 0x1F, 0xA3, 0x36, 0x30, 0xB2, 0x46, 0x51, 0xBC, 0xE3
};

#pragma alignvar(8)
static const uint8_t IT_INPUT_DATA[] = {
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
};

#pragma alignvar(8)
static const uint8_t test_rsa_signature_128byte[] = {
    0x5D, 0x5D, 0xA2, 0xFE, 0x2F, 0x73, 0xF4, 0x53, 0x59, 0xE3, 0x2B, 0x3A, 0xE9, 0x8F, 0xA5, 0x5F, 
    0x3F, 0xCF, 0x44, 0xE5, 0x7A, 0xFC, 0xA8, 0x23, 0x96, 0xE4, 0xD2, 0x15, 0x4A, 0x4A, 0x22, 0x02, 
    0x81, 0xA7, 0xE0, 0xB5, 0xF1, 0x56, 0xD1, 0x14, 0xD2, 0x6D, 0x42, 0x66, 0x2E, 0x68, 0x84, 0xDA, 
    0xC7, 0xA6, 0xE1, 0x64, 0x2C, 0x9D, 0x88, 0xA8, 0x17, 0x96, 0xA6, 0xE1, 0x64, 0x46, 0x32, 0xD0, 
    0xC2, 0xDB, 0x30, 0x39, 0xCB, 0x71, 0x14, 0xDE, 0x70, 0xA5, 0xFF, 0x79, 0x53, 0x42, 0x17, 0x7C, 
    0x21, 0x88, 0x0C, 0x08, 0x7B, 0x6D, 0x42, 0xFC, 0xA3, 0x43, 0xC3, 0xCB, 0xA1, 0xA6, 0x6D, 0xB3, 
    0x2B, 0x44, 0xA2, 0xA0, 0x14, 0x64, 0x5D, 0xB2, 0xE4, 0xB4, 0xAA, 0x88, 0x48, 0xBE, 0x4F, 0x73, 
    0x89, 0xD8, 0x6A, 0x4C, 0x6C, 0xFE, 0xE2, 0x8B, 0x11, 0xB2, 0xC1, 0xED, 0x8B, 0x2C, 0xF2, 0xFB
};

#pragma alignvar(4)
static const uint8_t test_firmware_name[] = {
    "ICUMD_FUL"
};

static const uint8_t test_def_uid[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#pragma alignvar(4)
static const uint8_t test_hash_message[] = {
    0xcd, 0x67, 0xbd, 0x40, 0x54, 0xaa, 0xa3, 0xba, 0xa0, 0xdb, 0x17, 0x8c, 0xe2, 0x32, 0xfd, 0x5a
};

#pragma alignvar(4)
static const uint8_t test_hash_data[] = {
    0x0d, 0x85, 0x21, 0xf8, 0xf2, 0xf3, 0x90, 0x03, 0x32, 0xd1, 0xa1, 0xa5, 0x5c, 0x60, 0xba, 0x81, 
    0xd0, 0x4d, 0x28, 0xdf, 0xe8, 0xc5, 0x04, 0xb6, 0x32, 0x8a, 0xe7, 0x87, 0x92, 0x5f, 0xe0, 0x18, 
    0x8f, 0x2b, 0xa9, 0x1c, 0x3a, 0x9f, 0x0c, 0x16, 0x53, 0xc4, 0xbf, 0x0a, 0xda, 0x35, 0x64, 0x55, 
    0xea, 0x36, 0xfd, 0x31, 0xf8, 0xe7, 0x3e, 0x39, 0x51, 0xca, 0xd4, 0xeb, 0xba, 0x8c, 0x6e, 0x04
};

#pragma alignvar(4)
static const uint8_t KEY[] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

#pragma alignvar(4)
static const uint8_t KEY_192[24] = {
    0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5, 0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
};

#pragma alignvar(4)
static const uint8_t KEY_256[32] = {
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};

#pragma alignvar(4)
static const uint8_t test_p_iv[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#pragma alignvar(4)
static const uint8_t test_p_iv_chipher[] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
};

#pragma alignvar(4)
static const uint8_t test_p_blick_in[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};

#pragma alignvar(4)
static const uint8_t test_cipher_data[] = {
    0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
    0xf5, 0xd3, 0xd5, 0x85, 0x03, 0xb9, 0x69, 0x9d, 0xe7, 0x85, 0x89, 0x5a, 0x96, 0xfd, 0xba, 0xaf,
    0x43, 0xb1, 0xcd, 0x7f, 0x59, 0x8e, 0xce, 0x23, 0x88, 0x1b, 0x00, 0xe3, 0xed, 0x03, 0x06, 0x88,
    0x7b, 0x0c, 0x78, 0x5e, 0x27, 0xe8, 0xad, 0x3f, 0x82, 0x23, 0x20, 0x71, 0x04, 0x72, 0x5d, 0xd4
};

#pragma alignvar(4)
static const uint8_t test_cipher_data_ctr[] = {
    0xE6, 0x35, 0x57, 0x48, 0xEB, 0x87, 0xC8, 0xAC, 0xCE, 0xE5, 0xAE, 0x44, 0xA5, 0x77, 0xC1, 0x61,
    0x6A, 0x99, 0xFB, 0x74, 0xF9, 0x3D, 0x01, 0x66, 0x6B, 0xBF, 0x63, 0xCE, 0x81, 0xA2, 0xF0, 0x20,
    0x42, 0xAC, 0x5A, 0x6E, 0xE0, 0x7C, 0xA6, 0x5F, 0x78, 0x06, 0x22, 0xAC, 0x5B, 0x67, 0xB3, 0x54,
    0x6C, 0xDC, 0xF3, 0xEF, 0x54, 0x47, 0xE9, 0x52, 0x82, 0x49, 0x9E, 0xD3, 0x6B, 0xD9, 0x2B, 0xBE,
};

#pragma alignvar(4)
static const uint8_t test_cipher_data_cbc[] = {
    0xE0, 0x48, 0x4D, 0x92, 0x5B, 0x2D, 0x98, 0x2F, 0x54, 0x05, 0x93, 0x95, 0x92, 0x62, 0xD1, 0x22,
    0x38, 0xD0, 0x88, 0x8E, 0x2D, 0x32, 0x79, 0x04, 0x78, 0xB0, 0x35, 0x8A, 0xAA, 0x48, 0x66, 0xB8,
    0xC4, 0xBF, 0x82, 0x6B, 0x97, 0x2F, 0x40, 0xF2, 0x84, 0xD9, 0x8B, 0xFD, 0x04, 0xAD, 0x1E, 0xD8,
    0x4F, 0xF6, 0x52, 0xF7, 0xA2, 0x1A, 0x1C, 0xA1, 0x87, 0x8D, 0x62, 0xE1, 0x82, 0xDD, 0xEA, 0xB8,
};

#pragma alignvar(4)
static const uint8_t test_p_iv_xts[] = {
    0x53, 0x19, 0xfb, 0x90, 0xe1, 0x36, 0xc7, 0xd0, 0x25, 0xe3, 0x76, 0x54, 0x32, 0xe6, 0xd4, 0x13
};

#pragma alignvar(4)
static const uint8_t test_p_blick_in_xts[] = {
    0x78, 0x96, 0xce, 0x48, 0x49, 0xa6, 0x03, 0xdc, 0xf0, 0x1e, 0x62, 0x2d, 0x68, 0x20, 0xf4, 0x25,
    0xe7, 0xde, 0x24, 0x85, 0xd2, 0x47, 0x21, 0x05, 0xbb, 0xf5, 0xf1, 0x1c, 0x5d, 0xc5, 0x8e, 0xe1
};

#pragma alignvar(4)
static const uint8_t test_cipher_data_xts[] = {
    0x19, 0x43, 0x90, 0x1f, 0xff, 0x18, 0x17, 0x0a, 0xc5, 0x9c, 0x0b, 0xbb, 0xd6, 0xb7, 0x23, 0x62,
    0xdb, 0xb5, 0xc6, 0x50, 0x57, 0xbf, 0x05, 0x53, 0x89, 0xf5, 0x20, 0x75, 0x7a, 0xc2, 0x9d, 0xaf
};

#pragma alignvar(4)
static const uint8_t test_p_iv_auth_gcm[] = {
    0xbe, 0xb4, 0x5b, 0x75, 0xea, 0x5b, 0x77, 0x50, 0xe6, 0x9a, 0x40, 0x9d
};

#pragma alignvar(4)
static const uint8_t test_p_auth_data_gcm[] = {
    0x8b, 0xad, 0xe1, 0x88, 0x07, 0xac, 0x5f, 0xe5, 0x9c, 0xfd, 0x34, 0xd7, 0xee, 0xd8, 0xae, 0x33,
    0xdf, 0xee, 0xb9, 0x60, 0xd2, 0xa1, 0x56, 0x93, 0x83, 0x7f, 0x22, 0xdd, 0x98, 0xf6, 0x87, 0x15,
    0x07, 0x9d, 0x35, 0x8a, 0x49, 0xa0, 0x4a, 0x7b, 0x23, 0x1d, 0x48, 0x6a, 0xe4, 0x5f, 0x4c, 0xd6,
    0xc0, 0xfe, 0x3c, 0xde, 0x6c, 0x0d, 0x96, 0x7c, 0xe8, 0xd8, 0x4b, 0xbf, 0x2e, 0x4b, 0xe2, 0xdb,
    0x79, 0x3a, 0xfb, 0x72, 0xea, 0x83, 0xc7, 0xb2, 0x1a, 0x73, 0xf6, 0x57, 0x42, 0xc2, 0x14, 0x76,
    0x8f, 0x7e, 0x61, 0x33, 0xe3, 0x81, 0x40, 0x59, 0xef, 0xb6
};

#pragma alignvar(4)
static const uint8_t test_p_data_in_gcm[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#pragma alignvar(4)
static const uint8_t test_p_auth_tag_gcm[] = {
    0x46, 0xe8, 0x45, 0x7d, 0x8d, 0x40, 0x9f, 0x2a, 0x06, 0x90, 0xe6, 0x50, 0x2f, 0x8b, 0x7a, 0x87
};

#pragma alignvar(4)
static const uint8_t test_pk_e10001[] = {
    "10001"
};

#pragma alignvar(4)
static const uint8_t test_pk_e10001_bin[] = {
    0x01, 0x00, 0x01
};

#pragma alignvar(4)
static const uint8_t test_pk_modulus_sig_1024[] = {
    "1B0E47A1CD2F66468354C3D292F8AB3EF26A94A0A7E80E9B130ECC61F6DE0744"
    "11BA047DA4C23179387F0DD7BDB50EA0FF5DE20D06FF79B35307017801A4BA17"
    "893A363BAC78C46AF0FF4496B02A1332C460925832A5CEE837EF2D6C38924EA5"
    "D5E6D79F9F9014B267726A52EDF2E22D3832F62883EA5AE90FFB2F2B76F56AB9"
};
static const uint8_t test_pk_modulus_sig_1024_bin[] = {
    0x1B, 0x0E, 0x47, 0xA1, 0xCD, 0x2F, 0x66, 0x46, 0x83, 0x54, 0xC3, 0xD2, 0x92, 0xF8, 0xAB, 0x3E,
    0xF2, 0x6A, 0x94, 0xA0, 0xA7, 0xE8, 0x0E, 0x9B, 0x13, 0x0E, 0xCC, 0x61, 0xF6, 0xDE, 0x07, 0x44,
    0x11, 0xBA, 0x04, 0x7D, 0xA4, 0xC2, 0x31, 0x79, 0x38, 0x7F, 0x0D, 0xD7, 0xBD, 0xB5, 0x0E, 0xA0,
    0xFF, 0x5D, 0xE2, 0x0D, 0x06, 0xFF, 0x79, 0xB3, 0x53, 0x07, 0x01, 0x78, 0x01, 0xA4, 0xBA, 0x17,
    0x89, 0x3A, 0x36, 0x3B, 0xAC, 0x78, 0xC4, 0x6A, 0xF0, 0xFF, 0x44, 0x96, 0xB0, 0x2A, 0x13, 0x32,
    0xC4, 0x60, 0x92, 0x58, 0x32, 0xA5, 0xCE, 0xE8, 0x37, 0xEF, 0x2D, 0x6C, 0x38, 0x92, 0x4E, 0xA5,
    0xD5, 0xE6, 0xD7, 0x9F, 0x9F, 0x90, 0x14, 0xB2, 0x67, 0x72, 0x6A, 0x52, 0xED, 0xF2, 0xE2, 0x2D,
    0x38, 0x32, 0xF6, 0x28, 0x83, 0xEA, 0x5A, 0xE9, 0x0F, 0xFB, 0x2F, 0x2B, 0x76, 0xF5, 0x6A, 0xB9,
};

#pragma alignvar(4)
static const uint8_t test_pk_priv_exp_sig_1024[] = {
    "108719a532f608b17c9af481177d47074e0963bffcb25af1caa398a123b0a099"
    "a9868c25c0d74c67b3e691ab405d7191524ea4bfb68dac67aa89b610fc5807c1"
    "dab823549cf9fb1438937aff909308e551ddba642151eceb32b7ae168dee43de"
    "b10f0feace1abe18100b26be389ead7b4f86fd5a6c7e230a2fef5126091cf12d"
};

#pragma alignvar(4)
static const uint8_t test_ecdsa_msg_16byte[] = {
    0x9a,0x27,0x86,0x99,0x2f,0x75,0x86,0xc7,0x89,0xfe,0x6c,0x1d,0x43,0xd6,0x3d,0x49,
};
#pragma alignvar(4)
static const uint8_t test_ecdsa_signature_0[] = {
    0x34, 0x35, 0x38, 0x38, 0x31, 0x32, 0x32, 0x33, 
    0x35, 0x30, 0x36, 0x36, 0x33, 0x38, 0x31, 0x38, 
    0x38, 0x32, 0x34, 0x36, 0x32, 0x34, 0x36, 0x37, 
    0x38, 0x34, 0x34, 0x36, 0x39, 0x30, 0x34, 0x33, 
    0x32, 0x33, 0x37, 0x35, 0x30, 0x38, 0x30, 0x30, 
    0x30, 0x33, 0x36, 0x31, 0x32, 0x38, 0x35, 0x37, 
    0x35, 0x30, 0x30, 0x35, 0x35, 0x38, 0x39, 0x37, 
    0x38, 0x38
};
#pragma alignvar(4)
static const uint8_t test_ecdsa_signature_1[] = {
    0x37, 0x30, 0x31, 0x37, 0x39, 0x33, 0x32, 0x34, 
    0x31, 0x35, 0x35, 0x30, 0x36, 0x37, 0x39, 0x30, 
    0x31, 0x32, 0x33, 0x30, 0x36, 0x36, 0x33, 0x33, 
    0x30, 0x32, 0x38, 0x31, 0x39, 0x38, 0x38, 0x31, 
    0x32, 0x30, 0x37, 0x34, 0x38, 0x32, 0x38, 0x36, 
    0x33, 0x39, 0x33, 0x32, 0x39, 0x32, 0x37, 0x33, 
    0x34, 0x36, 0x39, 0x30, 0x36, 0x36, 0x36, 0x35, 
    0x38
};

/* ECC encrypted new priv key include Initial Vector */
#pragma alignvar(4)
static const uint8_t test_newPrivKeyIncludeIV_0[] = {
    /* IV   */
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* Encrypted ECC Priv Scalar    */
    0xBE,0x2F,0xDF,0xA9,0xBF,0x18,0x5E,0x64,0x4F,0xB3,0x94,0xB4,0x21,0xFC,0x1B,0x92,
    0x02,0x24,0x20,0xC1,0x34,0xE1,0x45,0xDE,0x0C,0x9E,0x19,0xB4,0x5B,0x05,0x72,0x7A
};
/* ECC new pub key */
#pragma alignvar(4)
static const uint8_t test_newPubPointX[] = {
    0xbf,0x26,0x17,0xb6,0x13,0x44,0x19,0xf9,0x40,0x67,0x9e,0x21,0x0c,0xaf,0x9d,0x5c,
    0x2e,0x42,0x8f,0xae,0x6f,0x08,0x22,0x04,0xd7,0x45,0x3d,0xbf,0xf5,0x4a,0xc5,0xb8
};
#pragma alignvar(4)
static const uint8_t test_newPubPointY[] = {
    0xda,0x44,0xe1,0xfa,0x76,0x26,0x91,0xe0,0x05,0x39,0xc1,0xf5,0xf7,0x01,0x35,0x2c,
    0xd2,0xa0,0x12,0xda,0xf5,0x2b,0xde,0x57,0x47,0x25,0x21,0x74,0x9f,0xf9,0xf0,0xdc
};

#pragma alignvar(4)
static const uint8_t test_subjRSA512Cert_sha1_rsa3072[] = {
    0x30,0x82,0x02,0x81,0x30,0x81,0xEA,0x02,0x09,0x00,0xC2,0xBF,0xAC,0x3C,0x20,0xEB,
    0x76,0x27,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x05,0x05,
    0x00,0x30,0x26,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0A,0x0C,0x0A,0x52,0x48,
    0x38,0x35,0x30,0x49,0x43,0x55,0x4D,0x46,0x31,0x0F,0x30,0x0D,0x06,0x03,0x55,0x04,
    0x03,0x0C,0x06,0x52,0x6F,0x6F,0x74,0x43,0x41,0x30,0x1E,0x17,0x0D,0x31,0x37,0x30,
    0x39,0x31,0x36,0x30,0x39,0x30,0x37,0x30,0x31,0x5A,0x17,0x0D,0x31,0x38,0x30,0x39,
    0x31,0x36,0x30,0x39,0x30,0x37,0x30,0x31,0x5A,0x30,0x28,0x31,0x14,0x30,0x12,0x06,
    0x03,0x55,0x04,0x0A,0x0C,0x0B,0x52,0x48,0x38,0x35,0x30,0x49,0x43,0x55,0x4D,0x46,
    0x57,0x31,0x10,0x30,0x0E,0x06,0x03,0x55,0x04,0x03,0x0C,0x07,0x53,0x75,0x62,0x6A,
    0x65,0x63,0x74,0x30,0x5C,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,
    0x01,0x01,0x05,0x00,0x03,0x4B,0x00,0x30,0x48,0x02,0x41,0x00,0xBF,0x89,0x32,0x6F,
    0x68,0xCC,0xA5,0xD4,0xBC,0x56,0xF6,0x48,0x83,0xF3,0x2A,0x75,0x3E,0x2B,0xCB,0xA9,
    0xFF,0x69,0xB5,0x4A,0xC9,0x81,0x86,0x00,0x79,0x88,0x33,0x0B,0xFE,0x22,0x50,0xB1,
    0x1B,0xBA,0x76,0x43,0x46,0x29,0x2D,0x4B,0xC1,0xFD,0x81,0x92,0xE4,0xA6,0x08,0x27,
    0xA2,0xE2,0x93,0x2C,0xE7,0x22,0x0A,0x0C,0x6E,0x2F,0x43,0xCF,0x02,0x03,0x01,0x00,
    0x01,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x05,0x05,0x00,
    0x03,0x82,0x01,0x81,0x00,0x27,0xF8,0x81,0xBB,0xE0,0x6E,0x60,0x6A,0x48,0xFD,0x32,
    0x35,0xB9,0xDC,0x44,0x50,0x6F,0x30,0xFD,0xEA,0xD0,0xA7,0x34,0xED,0xAD,0xA3,0x51,
    0x79,0xA7,0xAD,0xD0,0x4C,0x25,0x42,0x38,0x71,0x67,0x04,0x7E,0x1E,0x6F,0xB2,0x78,
    0xDF,0xB8,0x0A,0x8D,0xED,0x68,0x26,0x51,0x39,0x5C,0xAA,0xF8,0xCA,0xFC,0xB2,0x40,
    0xE7,0x0A,0x14,0x05,0x02,0x75,0xE4,0xBE,0x7C,0xBC,0x63,0x93,0x32,0x5C,0x1C,0x70,
    0x29,0x39,0xC4,0x01,0x14,0x4B,0x51,0xE9,0xD7,0xFA,0xCC,0xAB,0x2A,0x88,0xB2,0xFE,
    0x4C,0x85,0x92,0xF0,0x1E,0x3A,0x32,0xD8,0xC0,0x91,0xD3,0x32,0x43,0x78,0x77,0x5E,
    0x77,0x21,0x7D,0xB7,0x7B,0x8E,0xC8,0x3D,0xFB,0x84,0xD8,0x93,0xDB,0xA7,0xDA,0x94,
    0x03,0x95,0x67,0xA6,0x1B,0x84,0xAC,0xD0,0xFE,0x65,0x59,0x57,0x94,0x04,0xA0,0x23,
    0x41,0x29,0x52,0xCA,0x7C,0xB2,0xE8,0xB1,0xFE,0x0E,0x19,0x85,0x81,0x33,0xA8,0xBF,
    0x7B,0x21,0x55,0x10,0xDD,0xCB,0x93,0xA1,0x2E,0x48,0xB4,0x77,0xE1,0x92,0x1C,0xFD,
    0xC6,0xD1,0xE3,0xC5,0x69,0x65,0x7E,0x92,0x0C,0x7D,0xF0,0xCD,0xEA,0x52,0xB5,0xDA,
    0x70,0xD4,0x5D,0xDB,0x2F,0xA9,0x71,0x85,0xE0,0x19,0x3D,0xB1,0x87,0xEC,0xD0,0xBB,
    0x5D,0xEB,0x3A,0x9B,0x81,0x8A,0x74,0xEE,0x56,0x9C,0x84,0xFF,0x22,0x95,0x90,0x64,
    0x39,0x06,0x9D,0x22,0xB0,0xD8,0x9E,0x4F,0xE9,0xB5,0x49,0xF5,0x7C,0x07,0x0C,0x0C,
    0xB5,0xC0,0xE7,0x18,0xFD,0xA9,0x01,0xC6,0x16,0x12,0x55,0xA4,0xBF,0xFE,0x3D,0x77,
    0xAA,0x03,0xCE,0x1D,0x65,0x8F,0xA8,0x0E,0xEE,0xDD,0xC0,0xFB,0x1C,0x0E,0xF7,0x07,
    0x30,0x8A,0xD0,0x16,0x17,0x7A,0x16,0x33,0x63,0xEA,0x1C,0x51,0x0B,0x31,0xA1,0xB6,
    0xFD,0x3B,0x66,0x26,0x10,0xDE,0x04,0xBE,0x85,0xB5,0x92,0xA9,0xFF,0x38,0x4B,0x27,
    0x91,0xE5,0xDB,0x44,0xF6,0xD3,0x8E,0x9B,0xBD,0xB1,0x48,0x0A,0x10,0x54,0x4B,0xE9,
    0x3F,0x42,0xE7,0x43,0xC7,0x4B,0x48,0x1F,0x7D,0x0B,0x8E,0x28,0x62,0x76,0x65,0xE0,
    0x18,0x69,0xA3,0x87,0x42,0xAF,0xD5,0x24,0xC0,0x60,0xE0,0x1E,0x74,0xC2,0xF3,0x7A,
    0x13,0x6C,0x1B,0x0F,0xAA,0x54,0x38,0xAC,0x87,0xF2,0xD1,0x7C,0x48,0xC7,0xA4,0xD2,
    0x1D,0x77,0x82,0x87,0x85,0x96,0x22,0x8D,0x51,0xB5,0x90,0xD1,0x15,0xF9,0x9D,0x5B,
    0x98,0x92,0x38,0x47,0x80
};

#pragma alignvar(4)
static const uint8_t test_encoded_msg_oaep_hexa[] = {
    "98DBE95D57C33ADD7F716BB7FBA82688A79E9D9CC7F054982F36D0B05AB75C01"
    "8CF50726A9F61431C45B9DD1880920362930472E5057F6C6EBB591871E194745"
    "F3E38615036906A30D94232B71B6E2E050A0E9EC8416AD3D8D5DA7E7A43E7EB0"
    "6F508EB1700D69CF23E2D6E76D71EC94B823DDDA70876C66255972C0FBA99A64"
};
#pragma alignvar(4)
static const uint8_t test_msg_rsa_decrypt[] =
{
    0x66, 0x28, 0x19, 0x4e, 0x12, 0x07, 0x3d, 0xb0, 0x3b, 0xa9, 0x4c, 0xda, 0x9e, 0xf9, 0x53, 0x23,
    0x97, 0xd5, 0x0d, 0xba, 0x79, 0xb9, 0x87, 0x00, 0x4a, 0xfe, 0xfe, 0x34
};

#pragma alignvar(4)
static const uint8_t test_msg_rsa_encrypt[] =
{
    0x66, 0x28, 0x19, 0x4e, 0x12, 0x07, 0x3d, 0xb0, 0x3b, 0xa9, 0x4c, 0xda, 0x9e, 0xf9, 0x53, 0x23,
    0x97, 0xd5, 0x0d, 0xba, 0x79, 0xb9, 0x87, 0x00, 0x4a, 0xfe, 0xfe, 0x34
};

#pragma alignvar(4)
static const uint8_t test_hmac_key[]= {
    0x47, 0x66, 0xe6, 0xfe, 0x5d, 0xff, 0xc9, 0x8a, 0x5c, 0x50
};
#pragma alignvar(4)
static const uint8_t test_def_hmac[]={
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#pragma alignvar(4)
static const uint8_t result_hmac[]={
    0x69, 0x5E, 0xBA, 0xCF, 0x48, 0x51, 0x81, 0xD8, 
    0x86, 0xC2, 0x11, 0xDA, 0x1C, 0xB8, 0x68, 0x6B, 
    0x2A, 0x27, 0xDE, 0xC4, 0x11, 0x71, 0x58, 0xEF, 
    0x21, 0x78, 0xE8, 0x86, 0x8C, 0x86, 0xA5, 0x13
};

#pragma alignvar(4)
static const uint8_t test_msg_hmac[]={
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

#pragma alignvar(4)
static const uint8_t test_hmac[]={
    0x1b, 0xa0, 0xe6, 0x6c, 0xf7, 0x2e, 0xfc, 0x34, 0x92, 0x07
};

#pragma alignvar(4)
static const unsigned char test_msg_cmac_fast_128bits[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

#pragma alignvar(4)
static const unsigned char test_vrf_cmac_fast_128bits[] = {
    0x07, 0x0a, 0x16, 0xb4, 0x6b, 0x4d, 0x41, 0x44, 0xf7, 0x9b, 0xdd, 0x9d, 0xd0, 0x4a, 0x28, 0x7c
};

#pragma alignvar(4)
static const uint8_t IT_SIG_2048_FORMAT_PSS_INPUT_128[] = {
    0x7a, 0xbc, 0x46, 0x54, 0x86, 0xca, 0x87, 0xfd, 0x35, 0x2a, 0xa6, 0xb3, 0x65, 0xd7, 0x82, 0xf9, 
    0xd4, 0x00, 0x7a, 0x54, 0xc3, 0x89, 0x39, 0x3e, 0x9d, 0x9b, 0xa4, 0x1b, 0x98, 0xdd, 0x45, 0xb1, 
    0xe4, 0xa6, 0xb6, 0x32, 0x5e, 0x87, 0xb7, 0xdb, 0x56, 0x20, 0x28, 0xa8, 0x6a, 0xb6, 0xf7, 0x4f, 
    0x21, 0xdc, 0x0c, 0xd9, 0x7b, 0x18, 0x3b, 0x55, 0x52, 0x0d, 0x0c, 0xe7, 0x70, 0x0c, 0xaa, 0xfd, 
    0xa7, 0xc5, 0x7f, 0x74, 0x94, 0xd4, 0x3d, 0xd3, 0x76, 0x2f, 0x02, 0x2d, 0x48, 0xfd, 0x5d, 0x8b, 
    0x9e, 0xa7, 0x0f, 0x2f, 0xb6, 0xdb, 0x9e, 0xe1, 0x35, 0x30, 0x34, 0x64, 0x5b, 0x67, 0x6e, 0xf6, 
    0x17, 0x4f, 0x25, 0x3d, 0x5a, 0xa0, 0x59, 0xc6, 0x53, 0xdd, 0x46, 0x1d, 0x13, 0x74, 0x8e, 0x8e, 
    0x9b, 0x96, 0x15, 0xd0, 0xdf, 0xab, 0x52, 0x48, 0xd2, 0x06, 0xa1, 0x54, 0xe7, 0x6c, 0x54, 0xb8, 
    0x7e, 0xea, 0x35, 0x3b, 0xc9, 0x08, 0x8a, 0x20, 0x4c, 0xe1, 0xd8, 0x72, 0x50, 0xf4, 0xa7, 0x76, 
    0xd7, 0x0a, 0xda, 0x0e, 0xd4, 0x3a, 0x6f, 0x81, 0x16, 0x94, 0x95, 0xb4, 0x74, 0x42, 0x11, 0x34, 
    0x00, 0x7d, 0x5a, 0xc0, 0x29, 0x33, 0xf1, 0x07, 0xc1, 0x4b, 0xec, 0x36, 0x44, 0xe3, 0xcc, 0x3d, 
    0x99, 0xf0, 0x5a, 0x6c, 0x0b, 0xcc, 0xfe, 0x38, 0x74, 0x8e, 0xea, 0xc9, 0x00, 0xcc, 0xe1, 0x00, 
    0x4f, 0x3d, 0x75, 0xd2, 0x84, 0xa3, 0x44, 0x43, 0x06, 0x14, 0x71, 0xfb, 0x1e, 0xe4, 0x99, 0x0c, 
    0x3d, 0xfb, 0x1d, 0xe4, 0x4b, 0xbd, 0x5b, 0x7e, 0x0b, 0x4b, 0x3f, 0xec, 0xea, 0x57, 0x75, 0x36, 
    0xf3, 0xa7, 0xb8, 0xfd, 0xd6, 0x7c, 0x4b, 0xfd, 0x77, 0xbe, 0xe8, 0x6d, 0xbd, 0x0e, 0x2e, 0x7d, 
    0x4a, 0x7a, 0x8e, 0xf0, 0x17, 0xe8, 0x48, 0x2c, 0xb9, 0x87, 0xb6, 0x21, 0x4b, 0x90, 0x94, 0x43
};

#pragma alignvar(4)
static const uint8_t test_p_iv_auth_ccm[16] = { 0x5a, 0x8a, 0xa4, 0x85, 0xc3, 0x16, 0xe9, 0x40, 0x3a, 0xff, 0x85, 0x9f, 0xbb };
#pragma alignvar(4)
static const uint8_t test_p_aad_ccm[128] = {
    0xa1, 0x6a, 0x2e, 0x74, 0x1f, 0x1c, 0xd9, 0x71, 0x72, 0x85, 0xb6, 0xd8, 0x82, 0xc1, 0xfc, 0x53,
    0x65, 0x5e, 0x97, 0x73, 0x76, 0x1a, 0xd6, 0x97, 0xa7, 0xee, 0x64, 0x10, 0x18, 0x4c, 0x79, 0x82
};
#pragma alignvar(4)
static const uint8_t test_p_plain_text_ccm[128] = {
    0x87, 0x39, 0xb4, 0xbe, 0xa1, 0xa0, 0x99, 0xfe, 0x54, 0x74, 0x99, 0xcb, 0xc6, 0xd1, 0xb1, 0x3d,
    0x84, 0x9b, 0x80, 0x84, 0xc9, 0xb6, 0xac, 0xc5
};
#pragma alignvar(4)
static const uint8_t  test_p_cipher_text_ccm[128] = {
    0x50, 0x03, 0x8b, 0x5f, 0xdd, 0x36, 0x4e, 0xe7, 0x47, 0xb7, 0x0d, 0x00, 0xbd, 0x36, 0x84, 0x0e,
    0xce, 0x4e, 0xa1, 0x99, 0x98, 0x12, 0x33, 0x75
};
#pragma alignvar(4)
static const uint8_t test_p_auth_tag_ccm[16] = { 0xc0, 0xa4, 0x58, 0xbf, 0xca, 0xfa, 0x3b, 0x26, 0x09, 0xaf, 0xe0, 0xf8, 0x25, 0xcb, 0xf5, 0x03 };

#pragma alignvar(4)
static const uint8_t test_p_blick_in_192[] = { 0xc5, 0x1f, 0xc2, 0x76, 0x77, 0x4d, 0xad, 0x94, 0xbc, 0xdc, 0x1d, 0x28, 0x91, 0xec, 0x86, 0x68 };

#pragma alignvar(4)
static const uint8_t test_p_iv_chipher_192[] = { 0x53, 0x1c, 0xe7, 0x81, 0x76, 0x40, 0x16, 0x66, 0xaa, 0x30, 0xdb, 0x94, 0xec, 0x4a, 0x30, 0xeb };

#pragma alignvar(4)
static const uint8_t test_cipher_data_192[] = { 0x70, 0xdd, 0x95, 0xa1, 0x4e, 0xe9, 0x75, 0xe2, 0x39, 0xdf, 0x36, 0xff, 0x4a, 0xee, 0x1d, 0x5d };

#pragma alignvar(4)
static const uint8_t test_p_blick_in_256[] = { 0x62, 0x82, 0xb8, 0xc0, 0x5c, 0x5c, 0x15, 0x30, 0xb9, 0x7d, 0x48, 0x16, 0xca, 0x43, 0x47, 0x62 };

#pragma alignvar(4)
static const uint8_t test_p_iv_chipher_256[] = { 0x85, 0x1e, 0x87, 0x64, 0x77, 0x6e, 0x67, 0x96, 0xaa, 0xb7, 0x22, 0xdb, 0xb6, 0x44, 0xac, 0xe8 };

#pragma alignvar(4)
static const uint8_t test_cipher_data_256[] = { 0x6a, 0xcc, 0x04, 0x14, 0x2e, 0x10, 0x0a, 0x65, 0xf5, 0x1b, 0x97, 0xad, 0xf5, 0x17, 0x2c, 0x41 };

#pragma alignvar (4)
static const uint8_t test_p_msg_192[] = {
    0xdf, 0xdd, 0xb7, 0x19, 0xd0, 0x03, 0x98, 0xbf, 0x48, 0xa6, 0xce, 0xfd, 0x27, 0x73, 0x63, 0x89,
    0xe6, 0x54, 0xa9, 0x3b, 0x85, 0x95, 0xcd, 0x5a, 0xc4, 0x46, 0xaf, 0x19, 0x96, 0xe0, 0xf1, 0x61
};

#pragma alignvar (4)
static const uint8_t test_p_msg_256[] = {
    0xac, 0x08, 0x74, 0x20, 0xfe, 0xb1, 0xe1, 0xe8, 0xc2, 0x54, 0x6c, 0x2a, 0x8b, 0x8a, 0x5a, 0xf0,
    0xd0, 0x3b, 0x05, 0xf7, 0xe5, 0xbf, 0x74, 0xfa, 0x29, 0x43, 0x2e, 0x33, 0x82, 0x6f, 0x50, 0x20,
    0x06, 0x46, 0xde, 0x83, 0xfb, 0x4d, 0x57, 0xac, 0x02, 0xef, 0x8c, 0xfa, 0x8f, 0x20, 0xb1, 0xa0
};

#pragma alignvar (4)
static const uint8_t test_cmac_value_192bits[] = {
    0x09, 0xc4, 0xd3, 0x37, 0xdb, 0x14, 0x6f, 0x92, 0x0d, 0x93, 0x7e, 0xd2, 0x83, 0xcc, 0x69, 0x44
};

#pragma alignvar (4)
static const uint8_t test_cmac_value_256bits[] = {
    0x32, 0xc7, 0x7c, 0x48, 0x18, 0x21, 0x18, 0x4a, 0x3e, 0x72
};

#pragma alignvar (4)
static const uint8_t test_p_iv_auth_192[] = {
    0x18, 0x17, 0xa5, 0xca, 0x06, 0xe3, 0x43, 0x2b, 0x2e, 0x01, 0x5d, 0xb6
};

#pragma alignvar (4)
static const uint8_t test_p_auth_data_192[] = {
    0x0b, 0xb4, 0x95, 0x97, 0x7b, 0xa3, 0xea, 0x02, 0xe8, 0x02, 0x02, 0x93, 0x81, 0x7c, 0xe5, 0xc5, 0xea, 0xba, 0x0c, 0x5d
};

#pragma alignvar (4)
static const uint8_t test_p_data_in_192[] = {
    0x2f, 0xa4, 0xc3, 0x83, 0x77, 0xf6, 0x68, 0x42, 0xad, 0x54, 0x6c, 0x04, 0x3c, 0x9e, 0xe8, 0x1b, 0x33, 0xe6, 0xb4, 0xcd,
    0x5a, 0x66, 0x47, 0x52, 0x0f, 0xd5, 0x58, 0x9c, 0xba, 0x20, 0xe3, 0x66
};

#pragma alignvar (4)
static const uint8_t test_p_auth_tag_192[] = {
    0x3f, 0xc4, 0x9f, 0x14, 0xbe, 0xe1, 0x22, 0x32, 0x4a, 0xef, 0x23, 0x65, 0x46, 0xb3, 0x01, 0xd9
};

#pragma alignvar (4)
static const uint8_t test_p_iv_auth_256[] = {
    0xfd, 0x1f, 0xa6, 0xb5, 0xca, 0xb9, 0xaa, 0x8d, 0x56, 0x41, 0x8a, 0xbb
};

#pragma alignvar (4)
static const uint8_t test_p_auth_data_256[] = {
    0x95, 0x3b, 0xcb, 0xd7, 0x31, 0xa1, 0x39, 0xc5, 0xde, 0x3a, 0x2b, 0x75, 0xe9, 0xff, 0xa4, 0xf4, 0x80, 0x18, 0x26, 0x6a
};

#pragma alignvar (4)
static const uint8_t test_p_data_in_256[] = {
    0x43, 0x49, 0x22, 0x1f, 0x66, 0x47, 0xa9, 0x06, 0xa4, 0x7e, 0x64, 0xb5, 0xa7, 0xa1, 0xde, 0xb2, 0xf7, 0xca, 0xf5, 0xc3,
    0xfe, 0xf1, 0x6f, 0x0b, 0x96, 0x8d, 0x62, 0x5b, 0xca, 0x36, 0x3d, 0xca
};

#pragma alignvar (4)
static const uint8_t test_p_auth_tag_256[] = {
    0x20, 0x23, 0x9e, 0x97, 0xe2, 0xdb, 0x49, 0x85, 0xf0, 0x7e, 0x27, 0x1b, 0xa5, 0x45, 0xbb, 0xbf
};

#pragma alignvar (4)
static const uint8_t test_eddsa_msg[] = {
    0xaf, 0x82
};

#pragma alignvar(4)
static const uint8_t test_eddsa_signature_0[] = {
    0x62, 0x91, 0xd6, 0x57, 0xde, 0xec, 0x24, 0x02, 0x48, 0x27, 0xe6, 0x9c, 0x3a, 0xbe, 0x01, 0xa3,
    0x0c, 0xe5, 0x48, 0xa2, 0x84, 0x74, 0x3a, 0x44, 0x5e, 0x36, 0x80, 0xd7, 0xdb, 0x5a, 0xc3, 0xac
};

#pragma alignvar(4)
static const uint8_t test_eddsa_signature_1[] = {
    0x18, 0xff, 0x9b, 0x53, 0x8d, 0x16, 0xf2, 0x90, 0xae, 0x67, 0xf7, 0x60, 0x98, 0x4d, 0xc6, 0x59,
    0x4a, 0x7c, 0x15, 0xe9, 0x71, 0x6e, 0xd2, 0x8d, 0xc0, 0x27, 0xbe, 0xce, 0xea, 0x1e, 0xc4, 0x0a
};

#pragma alignvar(4)
static const unsigned char test_msg_cmac_fast_192bits[] = {
    0xdf, 0xdd, 0xb7, 0x19, 0xd0, 0x03, 0x98, 0xbf, 0x48, 0xa6, 0xce, 0xfd, 0x27, 0x73, 0x63, 0x89,
    0xe6, 0x54, 0xa9, 0x3b, 0x85, 0x95, 0xcd, 0x5a, 0xc4, 0x46, 0xaf, 0x19, 0x96, 0xe0, 0xf1, 0x61
};

#pragma alignvar(4)
static const unsigned char test_vrf_cmac_fast_192bits[] = {
    0x09, 0xc4, 0xd3, 0x37, 0xdb, 0x14, 0x6f, 0x92, 0x0d, 0x93, 0x7e, 0xd2, 0x83, 0xcc, 0x69, 0x44
};


#pragma alignvar(4)
static const unsigned char test_cmac_msg_192bits[] = {
    0x2e, 0xae, 0xf8, 0x6b, 0x0f, 0x60, 0x23, 0x64, 0xf8, 0x65, 0x10, 0xea, 0xbc, 0x58, 0xbc, 0x9a,
    0xd1, 0xe6, 0xf0, 0xa6, 0xf6, 0xdf, 0x0b, 0x83, 0x18, 0x8c, 0x01, 0xe1, 0x77, 0x44, 0xa4, 0xe0,
    0x05, 0x3a, 0x22, 0x81, 0x0e, 0x99, 0xcf, 0x5a, 0x1e, 0xd3, 0x25, 0x8f, 0x20, 0x35, 0x09, 0xfd
};

#pragma alignvar (4)
static const uint8_t test_cmac_ver_value_192bits[] = {
    0x80, 0x6a, 0x7c, 0x69, 0xee, 0xbc, 0x60, 0x41, 0xe6, 0xf9
};

#pragma alignvar (4)
static const uint8_t test_p_iv_chipher_192_ctr[] = {
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

#pragma alignvar (4)
static const uint8_t test_p_blick_in_192_ctr[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

#pragma alignvar (4)
static const uint8_t test_cipher_data_ctr_192[] = {
    0x1a, 0xbc, 0x93, 0x24, 0x17, 0x52, 0x1c, 0xa2, 0x4f, 0x2b, 0x04, 0x59, 0xfe, 0x7e, 0x6e, 0x0b
};

#pragma alignvar (4)
static const uint8_t test_p_iv_chipher_192_cbc[] = {
    0x53, 0x1c, 0xe7, 0x81, 0x76, 0x40, 0x16, 0x66, 0xaa, 0x30, 0xdb, 0x94, 0xec, 0x4a, 0x30, 0xeb
};

#pragma alignvar (4)
static const uint8_t test_p_blick_in_192_cbc[] = {
    0xc5, 0x1f, 0xc2, 0x76, 0x77, 0x4d, 0xad, 0x94, 0xbc, 0xdc, 0x1d, 0x28, 0x91, 0xec, 0x86, 0x68
};

#pragma alignvar (4)
static const uint8_t test_cipher_data_cbc_192[] = {
    0x70, 0xdd, 0x95, 0xa1, 0x4e, 0xe9, 0x75, 0xe2, 0x39, 0xdf, 0x36, 0xff, 0x4a, 0xee, 0x1d, 0x5d
};

#pragma alignvar(4)
static const uint8_t test_p_iv_auth_ccm_192[16] = { 0x80, 0x93, 0x43, 0xe9, 0x86, 0xf6, 0xff, 0x47, 0xf5, 0x4d, 0x4c, 0xac, 0x22 };
#pragma alignvar(4)
static const uint8_t test_p_aad_ccm_192[128] = {
    0xd7, 0x0a, 0xef, 0x35, 0x32, 0xbd, 0xc5, 0x29, 0x3a, 0x3e, 0xbb, 0x11, 0x58, 0x9a, 0xc1, 0xf8,
    0x01, 0xc9, 0xf9, 0x3e, 0xa0, 0xd6, 0x56, 0xe1, 0xd0, 0x40, 0x68, 0xfa, 0xcf, 0x9f, 0x76, 0x8b
};
#pragma alignvar(4)
static const uint8_t test_p_plain_text_ccm_192[128] = {
    0x71, 0x8f, 0x06, 0x1e, 0x8b, 0x97, 0x2a, 0x3a, 0xdc, 0xf4, 0x65, 0xd6, 0x6c, 0x5b, 0x28, 0xe8,
    0x66, 0x1f, 0x08, 0x01, 0x27, 0xf6, 0x72, 0x2f
};
#pragma alignvar(4)
static const uint8_t  test_p_cipher_text_ccm_192[128] = {
    0xba, 0xd3, 0xb0, 0xe6, 0x77, 0x2e, 0x9c, 0x4c, 0x9c, 0x63, 0x1c, 0x09, 0x5e, 0x25, 0x9d, 0x99,
    0x69, 0x22, 0x92, 0x93, 0x2e, 0xfb, 0x72, 0xb8
};
#pragma alignvar(4)
static const uint8_t test_p_auth_tag_ccm_192[16] = { 0x96, 0x6e, 0x91, 0xa1, 0x96, 0x17, 0xbb, 0x74, 0x8f, 0x34, 0x95, 0xaa, 0x43, 0x35, 0x85, 0xbb };

#pragma alignvar(4)
static const unsigned char test_msg_cmac_fast_256bits[] = {
    0xac, 0x08, 0x74, 0x20, 0xfe, 0xb1, 0xe1, 0xe8, 0xc2, 0x54, 0x6c, 0x2a, 0x8b, 0x8a, 0x5a, 0xf0,
    0xd0, 0x3b, 0x05, 0xf7, 0xe5, 0xbf, 0x74, 0xfa, 0x29, 0x43, 0x2e, 0x33, 0x82, 0x6f, 0x50, 0x20,
    0x06, 0x46, 0xde, 0x83, 0xfb, 0x4d, 0x57, 0xac, 0x02, 0xef, 0x8c, 0xfa, 0x8f, 0x20, 0xb1, 0xa0
};

#pragma alignvar(4)
static const unsigned char test_cmac_msg_256bits[] = {
    0xac, 0x08, 0x74, 0x20, 0xfe, 0xb1, 0xe1, 0xe8, 0xc2, 0x54, 0x6c, 0x2a, 0x8b, 0x8a, 0x5a, 0xf0,
    0xd0, 0x3b, 0x05, 0xf7, 0xe5, 0xbf, 0x74, 0xfa, 0x29, 0x43, 0x2e, 0x33, 0x82, 0x6f, 0x50, 0x20,
    0x06, 0x46, 0xde, 0x83, 0xfb, 0x4d, 0x57, 0xac, 0x02, 0xef, 0x8c, 0xfa, 0x8f, 0x20, 0xb1, 0xa0
};

#pragma alignvar(4)
static const unsigned char test_vrf_cmac_fast_256bits[] = {
    0x32, 0xc7, 0x7c, 0x48, 0x18, 0x21, 0x18, 0x4a, 0x3e, 0x72
};

#pragma alignvar (4)
static const uint8_t test_cmac_ver_value_256bits[] = {
    0x32, 0xc7, 0x7c, 0x48, 0x18, 0x21, 0x18, 0x4a, 0x3e, 0x72
};

#pragma alignvar (4)
static const uint8_t test_p_iv_chipher_256_ctr[] = {
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

#pragma alignvar (4)
static const uint8_t test_p_blick_in_256_ctr[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

#pragma alignvar (4)
static const uint8_t test_cipher_data_ctr_256[] = {
    0x60, 0x1e, 0xc3, 0x13, 0x77, 0x57, 0x89, 0xa5, 0xb7, 0xa7, 0xf5, 0x04, 0xbb, 0xf3, 0xd2, 0x28
};

#pragma alignvar (4)
static const uint8_t test_p_iv_chipher_256_cbc[] = {
    0x85, 0x1e, 0x87, 0x64, 0x77, 0x6e, 0x67, 0x96, 0xaa, 0xb7, 0x22, 0xdb, 0xb6, 0x44, 0xac, 0xe8
};

#pragma alignvar (4)
static const uint8_t test_p_blick_in_256_cbc[] = {
    0x62, 0x82, 0xb8, 0xc0, 0x5c, 0x5c, 0x15, 0x30, 0xb9, 0x7d, 0x48, 0x16, 0xca, 0x43, 0x47, 0x62
};

#pragma alignvar (4)
static const uint8_t test_cipher_data_cbc_256[] = {
    0x6a, 0xcc, 0x04, 0x14, 0x2e, 0x10, 0x0a, 0x65, 0xf5, 0x1b, 0x97, 0xad, 0xf5, 0x17, 0x2c, 0x41
};

#pragma alignvar(4)
static const uint8_t test_p_iv_xts_256[] = {
    0xad, 0xf8, 0xd9, 0x26, 0x27, 0x46, 0x4a, 0xd2, 0xf0, 0x42, 0x8e, 0x84, 0xa9, 0xf8, 0x75, 0x64
};

#pragma alignvar(4)
static const uint8_t test_p_blick_in_xts_256[] = {
    0x2e, 0xed, 0xea, 0x52, 0xcd, 0x82, 0x15, 0xe1, 0xac, 0xc6, 0x47, 0xe8, 0x10, 0xbb, 0xc3, 0x64,
    0x2e, 0x87, 0x28, 0x7f, 0x8d, 0x2e, 0x57, 0xe3, 0x6c, 0x0a, 0x24, 0xfb, 0xc1, 0x2a, 0x20, 0x2e
};

#pragma alignvar(4)
static const uint8_t test_cipher_data_xts_256[] = {
    0xcb, 0xaa, 0xd0, 0xe2, 0xf6, 0xce, 0xa3, 0xf5, 0x0b, 0x37, 0xf9, 0x34, 0xd4, 0x6a, 0x9b, 0x13,
    0x0b, 0x9d, 0x54, 0xf0, 0x7e, 0x34, 0xf3, 0x6a, 0xf7, 0x93, 0xe8, 0x6f, 0x73, 0xc6, 0xd7, 0xdb
};

#pragma alignvar(4)
static const uint8_t test_p_iv_auth_ccm_256[16] = { 0xc4, 0x9c, 0xce, 0xf8, 0x69, 0xbb, 0x86, 0xd2, 0x19, 0x32, 0xcb, 0x44, 0x3b };

#pragma alignvar(4)
static const uint8_t test_p_aad_ccm_256[128] = {
    0xd3, 0x7e, 0x35, 0xd7, 0xcd, 0xcc, 0xd9, 0x82, 0x4a, 0x1a, 0xe4, 0xc7, 0x87, 0x81, 0x97, 0x35,
    0xe4, 0xaf, 0x79, 0x8a, 0x3b, 0xeb, 0x49, 0xd4, 0x70, 0x53, 0x36, 0xd6, 0x49, 0x68, 0x53, 0xad
};

#pragma alignvar(4)
static const uint8_t test_p_plain_text_ccm_256[128] = {
    0x77, 0x1a, 0x7b, 0xaa, 0x9c, 0xf8, 0x3a, 0xa2, 0x53, 0x34, 0x9f, 0x64, 0x75, 0xd5, 0xe7, 0x4d,
    0xba, 0x45, 0x25, 0x30, 0x7b, 0x02, 0x2b, 0xa7
};

#pragma alignvar(4)
static const uint8_t  test_p_cipher_text_ccm_256[128] = {
    0xee, 0xba, 0xc2, 0x47, 0x50, 0x04, 0x97, 0x00, 0x71, 0xdf, 0xa2, 0xcf, 0xb8, 0x55, 0xc4, 0xe7,
    0x8b, 0x1a, 0xdd, 0x8d, 0xcb, 0xcc, 0xfc, 0x0b
};

#pragma alignvar(4)
static const uint8_t test_p_auth_tag_ccm_256[16] = { 0xd6, 0xb1, 0x40, 0x27, 0x32, 0x4b, 0x65, 0x7a, 0x56, 0x26, 0x3d, 0xf1, 0x48, 0x66, 0x53, 0x93 };

#pragma alignvar(4)
static const unsigned char test_cmac_msg_dtf_128bits[] = {
    0x87, 0x98, 0x55, 0xff, 0x51, 0x96, 0x86, 0x2c, 0xc7, 0x7e, 0x32, 0x3f, 0x8c, 0x76, 0xb5, 0xb0
};

#pragma alignvar(4)
static const unsigned char test_cmac_msg_dtf_192bits[] = {
    0x86, 0x61, 0x70, 0x12, 0xaf, 0x7d, 0x94, 0x78, 0x41, 0xba, 0x40, 0x8d, 0xf5, 0xff, 0x8b, 0x9a,
    0x44, 0xf2, 0x59, 0x85, 0xcc, 0xc5, 0xdc, 0x44, 0xf5, 0x52, 0x5a, 0x97, 0xd4, 0x6b, 0x30, 0xf7
};

#pragma alignvar(4)
static const unsigned char test_cmac_msg_dtf_256bits[] = {
    0x42, 0xf3, 0x5d, 0x5a, 0xa5, 0x33, 0xa7, 0xa0, 0xa5, 0xf7, 0x4e, 0x14, 0x4f, 0x2a, 0x5f, 0x20
};

#pragma alignvar(4)
static const unsigned char test_cmac_dtf_ver_value_128bits[] = {
    0xd1, 0xc2, 0xa7, 0x0e
};

#pragma alignvar(4)
static const unsigned char test_cmac_dtf_ver_value_192bits[] = {
    0x8e, 0xfc, 0x4d, 0x25, 0xb6, 0x92, 0xc3, 0x4d, 0xdb, 0xbc
};

#pragma alignvar(4)
static const unsigned char test_cmac_dtf_ver_value_256bits[] = {
    0xf1, 0x53, 0x2f, 0x87, 0x32, 0xd9, 0xf5, 0x90, 0x30, 0x07
};

#pragma alignvar(4)
static const unsigned char test_cmac_msg_icup_128bits[] = {
    0xdc, 0x78, 0x7b, 0x23, 0x86, 0xfc, 0xae, 0x8b, 0x8c, 0x6d, 0x0c, 0x9d, 0x0c, 0x09, 0x2a, 0xae,
    0xda, 0xdd, 0x31, 0x50, 0x0d, 0x37, 0x84, 0x6b, 0xbe, 0x5f, 0xa3, 0x90, 0x0c, 0x82, 0x50, 0x89,
    0x63, 0xef, 0xf0, 0x35, 0xff
};

#pragma alignvar(4)
static const unsigned char test_cmac_msg_icup_192bits[] = {
    0x86, 0x61, 0x70, 0x12, 0xaf, 0x7d, 0x94, 0x78, 0x41, 0xba, 0x40, 0x8d, 0xf5, 0xff, 0x8b, 0x9a,
    0x44, 0xf2, 0x59, 0x85, 0xcc, 0xc5, 0xdc, 0x44, 0xf5, 0x52, 0x5a, 0x97, 0xd4, 0x6b, 0x30
};

#pragma alignvar(4)
static const unsigned char test_cmac_msg_icup_256bits[] = {
    0x42, 0xf3, 0x5d, 0x5a, 0xa5, 0x33, 0xa7, 0xa0, 0xa5, 0xf7, 0x4e, 0x14, 0x4f, 0x2a, 0x5f
};

#pragma alignvar(4)
static const unsigned char test_cmac_icup_ver_value_128bits[] = {
    0xd4, 0x3a, 0xd1, 0xe2
};

#pragma alignvar(4)
static const unsigned char test_cmac_icup_ver_value_192bits[] = {
    0x24, 0x27, 0x0a, 0xb2, 0xda, 0x6f, 0x3e, 0xef, 0xe8, 0x7a
};

#pragma alignvar(4)
static const unsigned char test_cmac_icup_ver_value_256bits[] = {
    0x25, 0x92, 0x33, 0x90, 0x8b, 0x66, 0x52, 0x9c, 0xbc, 0xb9
};

#pragma alignvar(4)
static const unsigned char test_p_iv_chipher_128_cbc[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

#pragma alignvar(4)
static const unsigned char test_p_blick_in_128_cbc[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};

#pragma alignvar(4)
static const unsigned char test_cipher_data_cbc_128[] = {
    0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
    0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee, 0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
    0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b, 0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
    0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09, 0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7
};


#pragma alignvar(4)
static const unsigned char test_p_iv_chipher_128_xts[] = {
    0x4f, 0xae, 0xf7, 0x11, 0x7c, 0xda, 0x59, 0xc6, 0x6e, 0x4b, 0x92, 0x01, 0x3e, 0x76, 0x8a, 0xd5
};

#pragma alignvar(4)
static const unsigned char test_p_blick_in_xts_128[] = {
    0xeb, 0xab, 0xce, 0x95, 0xb1, 0x4d, 0x3c, 0x8d, 0x6f, 0xb3, 0x50, 0x39, 0x07, 0x90, 0x31, 0x1c
};

#pragma alignvar(4)
static const unsigned char test_cipher_data_xts_128[] = {
    0x77, 0x8a, 0xe8, 0xb4, 0x3c, 0xb9, 0x8d, 0x5a, 0x82, 0x50, 0x81, 0xd5, 0xbe, 0x47, 0x1c, 0x63
};


#pragma alignvar (4)
static const uint8_t test_p_iv_chipher_192_slice[] = {
    0xea, 0xae, 0xca, 0x2e, 0x07, 0xdd, 0xed, 0xf5, 0x62, 0xf9, 0x4d, 0xf6, 0x3f, 0x0a, 0x65, 0x0f
};

#pragma alignvar (4)
static const uint8_t test_p_blick_in_192_slice[] = {
    0xc5, 0xce, 0x95, 0x86, 0x13, 0xbf, 0x74, 0x17, 0x18, 0xc1, 0x74, 0x44, 0x48, 0x4e, 0xba, 0xf1,
    0x05, 0x0d, 0xdc, 0xac, 0xb5, 0x9b, 0x95, 0x90, 0x17, 0x8c, 0xbe, 0x69, 0xd7, 0xad, 0x79, 0x19,
    0x60, 0x8c, 0xb0, 0x3a, 0xf1, 0x3b, 0xbe, 0x04, 0xf3, 0x50, 0x6b, 0x71, 0x8a, 0x30, 0x1e, 0xa0
};

#pragma alignvar (4)
static const uint8_t test_cipher_data_192_slice[] = {
    0xed, 0x6a, 0x50, 0xe0, 0xc6, 0x92, 0x1d, 0x52, 0xd6, 0x64, 0x7f, 0x75, 0xd6, 0x7b, 0x4f, 0xd5,
    0x6a, 0xce, 0x1f, 0xed, 0xb8, 0xb5, 0xa6, 0xa9, 0x97, 0xb4, 0xd1, 0x31, 0x64, 0x05, 0x47, 0xd2,
    0x2c, 0x5d, 0x88, 0x4a, 0x75, 0xe6, 0x75, 0x2b, 0x58, 0x46, 0xb5, 0xb3, 0x3a, 0x51, 0x81, 0xf4
};

#pragma alignvar (4)
static const uint8_t test_p_iv_chipher_256_slice[] = {
    0xbd, 0x41, 0x6c, 0xb3, 0xb9, 0x89, 0x22, 0x28, 0xd8, 0xf1, 0xdf, 0x57, 0x56, 0x92, 0xe4, 0xd0
};

#pragma alignvar (4)
static const uint8_t test_p_blick_in_256_slice[] = {
    0x8d, 0x3a, 0xa1, 0x96, 0xec, 0x3d, 0x7c, 0x9b, 0x5b, 0xb1, 0x22, 0xe7, 0xfe, 0x77, 0xfb, 0x12,
    0x95, 0xa6, 0xda, 0x75, 0xab, 0xe5, 0xd3, 0xa5, 0x10, 0x19, 0x4d, 0x3a, 0x8a, 0x41, 0x57, 0xd5,
    0xc8, 0x9d, 0x40, 0x61, 0x97, 0x16, 0x61, 0x98, 0x59, 0xda, 0x3e, 0xc9, 0xb2, 0x47, 0xce, 0xd9
};

#pragma alignvar (4)
static const uint8_t test_cipher_data_256_slice[] = {
    0x60, 0x8e, 0x82, 0xc7, 0xab, 0x04, 0x00, 0x7a, 0xdb, 0x22, 0xe3, 0x89, 0xa4, 0x47, 0x97, 0xfe,
    0xd7, 0xde, 0x09, 0x0c, 0x8c, 0x03, 0xca, 0x8a, 0x2c, 0x5a, 0xcd, 0x9e, 0x84, 0xdf, 0x37, 0xfb,
    0xc5, 0x8c, 0xe8, 0xed, 0xb2, 0x93, 0xe9, 0x8f, 0x02, 0xb6, 0x40, 0xd6, 0xd1, 0xd7, 0x24, 0x64
};

typedef struct test_service {
    r_icumif_service_id_t   service_id;
    uint8_t                 test_name[256];
    uint8_t                 enable;
} test_service_t;

typedef enum {
    RANDOM_TEST,
    SWEEP_TEST,
} test_type_t;

typedef enum {
    TEST_NO_AES_CMAC_JOB_SLICE_3,
    TEST_NO_AES_CMAC_JOB_SLICE_0,
    TEST_NO_AES_CMAC_JOB_SLICE_1,
    TEST_NO_AES_CIPHER,
    TEST_NO_AES_CIPHER_XTS,
    TEST_NO_AES_AUTH_CIPHER_GCM,
    TEST_NO_AES_AUTH_CIPHER_CCM_ENC,
    TEST_NO_AES_AUTH_CIPHER_CCM_DEC,
    TEST_NO_AES_KEY_UPDATE_PLAIN,
    TEST_NO_AES_KEY_UPDATE_PLAIN_ANOTHER_KEY,
    TEST_NO_AES_RAM_KEY_EXPORT_SHE,
    TEST_NO_HASH,
    TEST_NO_RAND_INIT_RNG,
    TEST_NO_RAND_EXTEND_SEED,
    TEST_NO_RAND_RND,
    TEST_NO_RAND_UNINSTANTIATE,
    TEST_NO_RAND_INSTANTIATE,
    TEST_NO_RAND_RESEED,
    TEST_NO_RAND_GENERATE,
    TEST_NO_PK_IMPORT_EXT,
    TEST_NO_HMAC_IMPORT,
    TEST_NO_HMAC_GENERATE,
    TEST_NO_HMAC_VERIFY,
    TEST_NO_RSA_KEY_GENERATE,
    TEST_NO_ECC_KEY_GENERATE,
    TEST_NO_PK_IMPORT,
    TEST_NO_PK_EXPORT,
    TEST_NO_RSA_VERIFY_JOB_SLICE_0,
    TEST_NO_RSA_VERIFY_JOB_SLICE_1,
    TEST_NO_RSA_SIGN,
    TEST_NO_RSA_SIGN_2,
    TEST_NO_RSA_DECRYPT,
    TEST_NO_RSA_ENCRYPT,
    TEST_NO_ECDSA_VERIFY,
    TEST_NO_ECDSA_SIGN,
    TEST_NO_ECDH,
    TEST_NO_GENERATE_M1_3,
    TEST_NO_STEP2_GENERATE_M1_3,
    TEST_NO_STEP2_UPDATE_SHE,
    TEST_NO_STEP2_CMAC_JOB_SLICE_1,
    TEST_NO_STEP2_CMAC_FAST_A,
    TEST_NO_STEP2_CMAC_FAST_B,
    TEST_NO_AES_CMAC_VERI,
    TEST_NO_AES_CIPHER_CTR_ENC,
    TEST_NO_AES_CIPHER_CTR_DEC,
    TEST_NO_AES_CIPHER_CBC_ENC,
    TEST_NO_AES_CIPHER_CBC_DEC,
    TEST_NO_AES_CIPHER_192,
    TEST_NO_AES_CIPHER_256,
    TEST_NO_AES_CMAC_192,
    TEST_NO_AES_CMAC_256,
    TEST_NO_AES_AUTH_CIPHER_192,
    TEST_NO_AES_AUTH_CIPHER_256,
    TEST_NO_AES_CIPHER_192_SLICE,
    TEST_NO_AES_CIPHER_256_SLICE,
    TEST_NO_AES_CMAC_192_SLICE,
    TEST_NO_AES_CMAC_256_SLICE,
    TEST_NO_AES_AUTH_CIPHER_192_SLICE,
    TEST_NO_AES_AUTH_CIPHER_256_SLICE,
    TEST_NO_AES_KEY_UPDATE_PLAIN_192,
    TEST_NO_AES_KEY_UPDATE_PLAIN_256,
    TEST_NO_ECDH_192,
    TEST_NO_ECDH_256,
    TEST_NO_EDDSA_SIGN,
    TEST_NO_EDDSA_VERIFY,
    TEST_NO_AES_RAM_KEY_EXP_SHE_EXT,
    TEST_NO_STEP3_CMAC_FAST_A_192,
    TEST_NO_STEP3_CMAC_FAST_B_192,
    TEST_NO_AES_CMAC_VERI_192,
    TEST_NO_AES_CIPHER_CTR_ENC_192,
    TEST_NO_AES_CIPHER_CTR_DEC_192,
    TEST_NO_AES_CIPHER_CBC_ENC_192,
    TEST_NO_AES_CIPHER_CBC_DEC_192,
    TEST_NO_AES_AUTH_CIPHER_CCM_ENC_192,
    TEST_NO_AES_AUTH_CIPHER_CCM_DEC_192,
    TEST_NO_STEP3_CMAC_FAST_A_256,
    TEST_NO_STEP3_CMAC_FAST_B_256,
    TEST_NO_AES_CMAC_VERI_256,
    TEST_NO_AES_CIPHER_CTR_ENC_256,
    TEST_NO_AES_CIPHER_CTR_DEC_256,
    TEST_NO_AES_CIPHER_CBC_ENC_256,
    TEST_NO_AES_CIPHER_CBC_DEC_256,
    TEST_NO_AES_CIPHER_XTS_256,
    TEST_NO_AES_AUTH_CIPHER_CCM_ENC_256,
    TEST_NO_AES_AUTH_CIPHER_CCM_DEC_256,
    TEST_NO_AES_CMAC_VERI_192_SLICE,
    TEST_NO_AES_CIPHER_CTR_ENC_192_SLICE,
    TEST_NO_AES_CIPHER_CTR_DEC_192_SLICE,
    TEST_NO_AES_CIPHER_CBC_ENC_192_SLICE,
    TEST_NO_AES_CIPHER_CBC_DEC_192_SLICE,
    TEST_NO_AES_AUTH_CIPHER_CCM_ENC_192_SLICE,
    TEST_NO_AES_AUTH_CIPHER_CCM_DEC_192_SLICE,
    TEST_NO_AES_CMAC_VERI_256_SLICE,
    TEST_NO_AES_CIPHER_CTR_ENC_256_SLICE,
    TEST_NO_AES_CIPHER_CTR_DEC_256_SLICE,
    TEST_NO_AES_CIPHER_CBC_ENC_256_SLICE,
    TEST_NO_AES_CIPHER_CBC_DEC_256_SLICE,
    TEST_NO_AES_CIPHER_XTS_256_SLICE,
    TEST_NO_AES_AUTH_CIPHER_CCM_ENC_256_SLICE,
    TEST_NO_AES_AUTH_CIPHER_CCM_DEC_256_SLICE,
    TEST_NO_AES_CMAC_DTF_GEN_128,
    TEST_NO_AES_CMAC_DTF_GEN_192,
    TEST_NO_AES_CMAC_DTF_GEN_256,
    TEST_NO_AES_CMAC_DTF_VER_128,
    TEST_NO_AES_CMAC_DTF_VER_192,
    TEST_NO_AES_CMAC_DTF_VER_256,
    TEST_NO_AES_CMAC_ICUP_GEN_128,
    TEST_NO_AES_CMAC_ICUP_GEN_192,
    TEST_NO_AES_CMAC_ICUP_GEN_256,
    TEST_NO_AES_CMAC_ICUP_VER_128,
    TEST_NO_AES_CMAC_ICUP_VER_192,
    TEST_NO_AES_CMAC_ICUP_VER_256,
    TEST_NO_AES_CIPHER_CBC_ENC_DTF_128,
    TEST_NO_AES_CIPHER_CBC_ENC_ICUP_128,
    TEST_NO_AES_CIPHER_CBC_ENC_DTF_192,
    TEST_NO_AES_CIPHER_CBC_ENC_ICUP_192,
    TEST_NO_AES_CIPHER_CBC_ENC_DTF_256,
    TEST_NO_AES_CIPHER_CBC_ENC_ICUP_256,
    TEST_NO_AES_CIPHER_CBC_DEC_DTF_128,
    TEST_NO_AES_CIPHER_CBC_DEC_ICUP_128,
    TEST_NO_AES_CIPHER_CBC_DEC_DTF_192,
    TEST_NO_AES_CIPHER_CBC_DEC_ICUP_192,
    TEST_NO_AES_CIPHER_CBC_DEC_DTF_256,
    TEST_NO_AES_CIPHER_CBC_DEC_ICUP_256,
    TEST_NO_AES_CIPHER_XTS_ENC_DTF_128,
    TEST_NO_AES_CIPHER_XTS_ENC_DTF_256,
    TEST_NO_AES_CIPHER_XTS_DEC_DTF_128,
    TEST_NO_AES_CIPHER_XTS_DEC_DTF_256,
    TEST_NO_AES_CIPHER_XTS_ENC_ICUP_128,
    TEST_NO_AES_CIPHER_XTS_ENC_ICUP_256,
    TEST_NO_AES_CIPHER_XTS_DEC_ICUP_128,
    TEST_NO_AES_CIPHER_XTS_DEC_ICUP_256,
    TEST_NO_AES_AUTH_CIPHER_GCM_ENC_DTF_128,
    TEST_NO_AES_AUTH_CIPHER_GCM_ENC_DTF_192,
    TEST_NO_AES_AUTH_CIPHER_GCM_ENC_DTF_256,
    TEST_NO_AES_AUTH_CIPHER_GCM_DEC_DTF_128,
    TEST_NO_AES_AUTH_CIPHER_GCM_DEC_DTF_192,
    TEST_NO_AES_AUTH_CIPHER_GCM_DEC_DTF_256,
    TEST_NO_AES_AUTH_CIPHER_GCM_ENC_ICUP_128,
    TEST_NO_AES_AUTH_CIPHER_GCM_ENC_ICUP_192,
    TEST_NO_AES_AUTH_CIPHER_GCM_ENC_ICUP_256,
    TEST_NO_AES_AUTH_CIPHER_GCM_DEC_ICUP_128,
    TEST_NO_AES_AUTH_CIPHER_GCM_DEC_ICUP_192,
    TEST_NO_AES_AUTH_CIPHER_GCM_DEC_ICUP_256,
    TEST_NO_AES_AUTH_CIPHER_CCM_ENC_DTF_128,
    TEST_NO_AES_AUTH_CIPHER_CCM_ENC_DTF_192,
    TEST_NO_AES_AUTH_CIPHER_CCM_ENC_DTF_256,
    TEST_NO_AES_AUTH_CIPHER_CCM_DEC_DTF_128,
    TEST_NO_AES_AUTH_CIPHER_CCM_DEC_DTF_192,
    TEST_NO_AES_AUTH_CIPHER_CCM_DEC_DTF_256,
    TEST_NO_AES_AUTH_CIPHER_CCM_ENC_ICUP_128,
    TEST_NO_AES_AUTH_CIPHER_CCM_ENC_ICUP_192,
    TEST_NO_AES_AUTH_CIPHER_CCM_ENC_ICUP_256,
    TEST_NO_AES_AUTH_CIPHER_CCM_DEC_ICUP_128,
    TEST_NO_AES_AUTH_CIPHER_CCM_DEC_ICUP_192,
    TEST_NO_AES_AUTH_CIPHER_CCM_DEC_ICUP_256,
    TEST_NO_AES_CMAC_GEN_SLICE_128,
    TEST_NO_AES_CMAC_GEN_SLICE_192,
    TEST_NO_AES_CMAC_GEN_SLICE_256,
    TEST_NO_AES_CMAC_VER_SLICE_128,
    TEST_NO_AES_CMAC_VER_SLICE_192,
    TEST_NO_AES_CMAC_VER_SLICE_256,
    TEST_NO_AES_CIPHER_CBC_ENC_SLICE_128,
    TEST_NO_AES_CIPHER_CBC_ENC_SLICE_192,
    TEST_NO_AES_CIPHER_CBC_ENC_SLICE_256,
    TEST_NO_AES_CIPHER_CBC_DEC_SLICE_128,
    TEST_NO_AES_CIPHER_CBC_DEC_SLICE_192,
    TEST_NO_AES_CIPHER_CBC_DEC_SLICE_256,
    TEST_NO_AES_CIPHER_XTS_ENC_SLICE_128,
    TEST_NO_AES_CIPHER_XTS_ENC_SLICE_256,
    TEST_NO_AES_CIPHER_XTS_DEC_SLICE_128,
    TEST_NO_AES_CIPHER_XTS_DEC_SLICE_256,
    TEST_NO_AES_AUTH_CIPHER_GCM_ENC_SLICE_128,
    TEST_NO_AES_AUTH_CIPHER_GCM_ENC_SLICE_192,
    TEST_NO_AES_AUTH_CIPHER_GCM_ENC_SLICE_256,
    TEST_NO_AES_AUTH_CIPHER_GCM_DEC_SLICE_128,
    TEST_NO_AES_AUTH_CIPHER_GCM_DEC_SLICE_192,
    TEST_NO_AES_AUTH_CIPHER_GCM_DEC_SLICE_256,
    TEST_NO_SDF_GC_REQ,
    TEST_NO_MAX,
    INVALID_SERVICE = 0x01000000,
    LAST_NO = 0x02000000,
} test_no_t;

#define STEP1_ENABLE       1
#define STEP2_ENABLE       1
#define STEP3_ENABLE       1

#define STEP2_UPDATE_SHE   0

#define STEP2_RSA_PRIV_KEY 1
#define SWEEP_ONLY_ENABLE  2

#if defined(AES_CONTEXT_CHECK)
static const test_service_t test_service[] = {

    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_11, job_slice = 3, msg_size_in_bits = 1279, priority normal)",  STEP1_ENABLE},
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_3, job_slice = 0, priority normal)",  STEP1_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_5, job_slice = 1, priority normal)",  STEP1_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_7)",                                STEP1_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS, AES_14, AES_15)",                  STEP2_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM, DEC, AES_16)",                STEP2_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM, ENC, AES_17)",                STEP2_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM, DEC, AES_17)",                STEP2_ENABLE     },
    {SERVICE_03_AES_KEY_UPDATE_PLAIN,   "SERVICE_03_AES_KEY_UPDATE_PLAIN (AES_9)",                      0                },
    {SERVICE_03_AES_KEY_UPDATE_PLAIN,   "SERVICE_03_AES_KEY_UPDATE_PLAIN (AES_10)",                     0                },
    {SERVICE_03_AES_RAM_KEY_EXP_SHE,    "SERVICE_03_AES_RAM_KEY_EXPORT_SHE (AES_RAM_0)",                0                },
    {SERVICE_01_HASH,                   "SERVICE_01_HASH",                                              0                },
    {SERVICE_02_RAND_INIT_RNG,          "SERVICE_02_RAND_INIT_RNG (on_line_test = 1)",                  0                },
    {SERVICE_02_RAND_EXTEND_SEED,       "SERVICE_02_RAND_EXTEND_SEED",                                  0                },
    {SERVICE_02_RAND_RND,               "SERVICE_02_RAND_RND(job_slice = 1)",                           0                },
    {SERVICE_03_HMAC_IMPORT,            "SERVICE_03_HMAC_IMPORT (HMAC_0)",                              0                },
    {SERVICE_01_HMAC,                   "SERVICE_01_HMAC (GENERATE, HMAC_1, job_slice = 1)",            0                },
    {SERVICE_01_HMAC,                   "SERVICE_01_HMAC (VERIFY, HMAC_1, job_slice = 1)",              0                },
    {SERVICE_03_RSA_KEY_GENERATE,       "SERVICE_03_RSA_KEY_GENERATE (RSA_9)",                          0                },
    {SERVICE_03_ECC_KEY_GENERATE,       "SERVICE_03_ECC_KEY_GENERATE (ECC_RAM_0)",                      0                },
    {SERVICE_03_PK_IMPORT,              "SERVICE_03_PK_IMPORT (RSA_3)",                                 0                },
    {SERVICE_01_RSA_VERIFY,             "SERVICE_01_RSA_VERIFY (RSA_0, job_slice_hash = 0)",            0                },
    {SERVICE_01_RSA_VERIFY,             "SERVICE_01_RSA_VERIFY (RSA_1, job_slice_hash = 1)",            0                },
    {SERVICE_01_RSA_SIGN,               "SERVICE_01_RSA_SIGN (RSA_2, job_slice_hash = 1)",              0                },
    {SERVICE_01_RSA_SIGN,               "SERVICE_01_RSA_SIGN (RSA_6, job_slice_hash = 32)",             0                },
    {SERVICE_01_RSA_DECRYPT,            "SERVICE_01_RSA_DECRYPT (RSA_7)",                               0                },
    {SERVICE_01_RSA_ENCRYPT,            "SERVICE_01_RSA_ENCRYPT (RSA_8)",                               0                },
    {SERVICE_01_ECDSA_VERIFY,           "SERVICE_01_ECDSA_VERIFY (ECC_RAM_1)",                          0                },
    {SERVICE_01_ECDSA_SIGN,             "SERVICE_01_ECDSA_SIGN (ECC_RAM_1)",                            0                },
    {SERVICE_01_ECDH,                   "SERVICE_01_ECDH (ECC_1, ECC_PUB_1, AES_RAM_1)",                0                },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (STEP2 AES_8, job_slice = 1)",             STEP1_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (STEP2 AES_12, priority Highest)",         STEP1_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (STEP2 AES_13, priority Highest)",         STEP1_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_3, job_slice = 0, verification)",     STEP1_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_7, CTR ENC)",                       STEP1_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_7, CTR DEC)",                       STEP1_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_7, CBC ENC)",                       STEP1_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_7, CBC DEC)",                       STEP1_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_18, 192BIT)",                       STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_19, 256BIT)",                       STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_20, 192BIT)",                         STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_21, 256BIT)",                         STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM,AES_22, 192BIT)",              STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM,AES_23, 256BIT)",              STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_18, 192BIT)",                       STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_19, 256BIT)",                       STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_20, 192BIT)",                         STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_21, 256BIT)",                         STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM,AES_22, 192BIT)",              STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM,AES_23, 256BIT)",              STEP3_ENABLE     },
    {SERVICE_03_AES_KEY_UPDATE_PLAIN,   "SERVICE_03_AES_KEY_UPDATE_PLAIN (192bit)",                     0                },
    {SERVICE_03_AES_KEY_UPDATE_PLAIN,   "SERVICE_03_AES_KEY_UPDATE_PLAIN (256bit)",                     0                },
    {SERVICE_01_ECDH,                   "SERVICE_01_ECDH (192bit, AES_RAM_2)",                          0                },
    {SERVICE_01_ECDH,                   "SERVICE_01_ECDH (256bit, AES_RAM_3)",                          0                },
    {SERVICE_01_EDDSA_SIGN,             "SERVICE_01_EDDSA_SIGN",                                        0                },
    {SERVICE_01_EDDSA_VERIFY,           "SERVICE_01_EDDSA_VERIFY",                                      0                },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_26, priority Highest)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_27, priority Highest)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC Verification AES_28)",           STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Enc,AES_29, 192BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Dec,AES_29, 192BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Enc,AES_30, 192BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Dec,AES_30, 192BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Enc,CCM,AES_33, 192BIT)",      STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Dec,CCM,AES_33, 192BIT)",      STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_34, priority Highest)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_35, priority Highest)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC Verification AES_36)",           STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Enc,AES_37, 256BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Dec,AES_37, 256BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Enc,AES_38, 256BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Dec,AES_38, 256BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS,AES_39,AES_40, 256BIT)",            STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Enc,AES_41, 256BIT)",          STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Dec,AES_41, 256BIT)",          STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC Verification AES_28,slice)",     STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Enc,AES_29, 192BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Dec,AES_29, 192BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Enc,AES_30, 192BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Dec,AES_30, 192BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Enc,AES_33, 192BIT,slice)",    0     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Dec,AES_33, 192BIT,slice)",    0     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC Verification AES_36,sllice)",    STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Enc,AES_37, 256BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Dec,AES_37, 256BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Enc,AES_38, 256BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Dec,AES_38, 256BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS,AES_39,AES_40, 256BIT,slice)",      STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Enc,AES_41, 256BIT,slice)",    0     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Dec,AES_41, 256BIT,slice)",    0     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC DTF_GEN_128)",                   SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC DTF_GEN_192)",                   SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC DTF_GEN_256)",                   SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC DTF_VER_128)",                   SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC DTF_VER_192)",                   SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC DTF_VER_256)",                   SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC ICUP_GEN_128)",                  SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC ICUP_GEN_192)",                  SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC ICUP_GEN_256)",                  SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC ICUP_VER_128)",                  SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC ICUP_VER_192)",                  SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC ICUP_VER_256)",                  SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC DTF_ENC_128)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC ICUP_ENC_128)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC DTF_ENC_192)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC ICUP_ENC_192)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC DTF_ENC_256)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC ICUP_ENC_256)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC DTF_DEC_128)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC ICUP_DEC_128)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC DTF_DEC_192)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC ICUP_DEC_192)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC DTF_DEC_256)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC ICUP_DEC_256)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS DTF_ENC_128)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS DTF_ENC_256)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS DTF_DEC_128)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS DTF_DEC_256)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS ICUP_ENC_128)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS ICUP_ENC_256)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS ICUP_DEC_128)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS ICUP_DEC_256)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM DTF_ENC_128)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM DTF_ENC_192)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM DTF_ENC_256)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM DTF_DEC_128)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM DTF_DEC_192)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM DTF_DEC_256)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM ICUP_ENC_128)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM ICUP_ENC_192)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM ICUP_ENC_256)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM ICUP_DEC_128)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM ICUP_DEC_192)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM ICUP_DEC_256)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM DTF_ENC_128)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM DTF_ENC_192)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM DTF_ENC_256)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM DTF_DEC_128)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM DTF_DEC_192)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM DTF_DEC_256)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM ICUP_ENC_128)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM ICUP_ENC_192)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM ICUP_ENC_256)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM ICUP_DEC_128)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM ICUP_DEC_192)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM ICUP_DEC_256)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC slice_GEN_128)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC slice_GEN_192)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC slice_GEN_256)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC slice_VER_128)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC slice_VER_192)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC slice_VER_256)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC slice_ENC_128)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC slice_ENC_192)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC slice_ENC_256)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC slice_DEC_128)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC slice_DEC_192)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC slice_DEC_256)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS slice_ENC_128)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS slice_ENC_256)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS slice_DEC_128)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS slice_DEC_256)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM slice_ENC_128)",               SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM slice_ENC_192)",               SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM slice_ENC_256)",               SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM slice_DEC_128)",               SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM slice_DEC_192)",               SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM slice_DEC_256)",               SWEEP_ONLY_ENABLE     },
    {SERVICE_04_ICUMDF_GC,              "SERVICE_04_ICUMDF_GC (GC all/GC 1block)",                      STEP1_ENABLE     },
};

#else
static const test_service_t test_service[] = {

    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_11, job_slice = 3, msg_size_in_bits = 1279, priority normal)",  STEP1_ENABLE},
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_3, job_slice = 0, priority normal)",  STEP1_ENABLE    },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_5, job_slice = 1, priority normal)",  STEP1_ENABLE    },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_7)",                                STEP1_ENABLE    },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS, AES_14, AES_15)",                  STEP2_ENABLE    },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM, DEC, AES_16)",                STEP2_ENABLE    },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM, ENC, AES_17)",                STEP2_ENABLE    },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM, DEC, AES_17)",                STEP2_ENABLE    },
    {SERVICE_03_AES_KEY_UPDATE_PLAIN,   "SERVICE_03_AES_KEY_UPDATE_PLAIN (AES_9)",                      STEP1_ENABLE    },
    {SERVICE_03_AES_KEY_UPDATE_PLAIN,   "SERVICE_03_AES_KEY_UPDATE_PLAIN (AES_10)",                     STEP1_ENABLE    },
    {SERVICE_01_HASH,                   "SERVICE_01_HASH",                                              STEP2_ENABLE    },
    {SERVICE_02_RAND_INIT_RNG,          "SERVICE_02_RAND_INIT_RNG (on_line_test = 1)",                  STEP1_ENABLE    },
    {SERVICE_02_RAND_EXTEND_SEED,       "SERVICE_02_RAND_EXTEND_SEED",                                  STEP1_ENABLE    },
    {SERVICE_02_RAND_RND,               "SERVICE_02_RAND_RND(job_slice = 1)",                           STEP1_ENABLE    },
    {SERVICE_03_HMAC_IMPORT,            "SERVICE_03_HMAC_IMPORT (HMAC_0)",                              STEP2_ENABLE    },
    {SERVICE_01_HMAC,                   "SERVICE_01_HMAC (GENERATE, HMAC_1, job_slice = 1)",            STEP2_ENABLE    },
    {SERVICE_01_HMAC,                   "SERVICE_01_HMAC (VERIFY, HMAC_1, job_slice = 1)",              STEP2_ENABLE    },
    {SERVICE_03_RSA_KEY_GENERATE,       "SERVICE_03_RSA_KEY_GENERATE (RSA_9)",                          0               },
    {SERVICE_03_ECC_KEY_GENERATE,       "SERVICE_03_ECC_KEY_GENERATE (ECC_RAM_0)",                      STEP2_ENABLE    },
    {SERVICE_03_PK_IMPORT,              "SERVICE_03_PK_IMPORT (RSA_3)",                                 STEP2_RSA_PRIV_KEY    },
    {SERVICE_01_RSA_VERIFY,             "SERVICE_01_RSA_VERIFY (RSA_0, job_slice_hash = 0)",            STEP2_ENABLE    },
    {SERVICE_01_RSA_VERIFY,             "SERVICE_01_RSA_VERIFY (RSA_1, job_slice_hash = 1)",            STEP2_ENABLE    },
    {SERVICE_01_RSA_SIGN,               "SERVICE_01_RSA_SIGN (RSA_2, job_slice_hash = 1)",              STEP2_RSA_PRIV_KEY    },
    {SERVICE_01_RSA_SIGN,               "SERVICE_01_RSA_SIGN (RSA_6, job_slice_hash = 32)",             STEP2_RSA_PRIV_KEY    },
    {SERVICE_01_RSA_DECRYPT,            "SERVICE_01_RSA_DECRYPT (RSA_7)",                               STEP2_RSA_PRIV_KEY    },
    {SERVICE_01_RSA_ENCRYPT,            "SERVICE_01_RSA_ENCRYPT (RSA_8)",                               STEP2_RSA_PRIV_KEY    },
    {SERVICE_01_ECDSA_VERIFY,           "SERVICE_01_ECDSA_VERIFY (ECC_RAM_1)",                          STEP2_ENABLE    },
    {SERVICE_01_ECDSA_SIGN,             "SERVICE_01_ECDSA_SIGN (ECC_RAM_1)",                            STEP2_ENABLE    },
    {SERVICE_01_ECDH,                   "SERVICE_01_ECDH (ECC_1, ECC_PUB_1, AES_RAM_1)",                STEP2_ENABLE    },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (STEP2 AES_8, job_slice = 1)",             STEP1_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (STEP2 AES_12, priority Highest)",         STEP1_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (STEP2 AES_13, priority Highest)",         STEP1_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_3, job_slice = 0, verification)",     STEP1_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_7, CTR ENC)",                       STEP1_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_7, CTR DEC)",                       STEP1_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_7, CBC ENC)",                       STEP1_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_7, CBC DEC)",                       STEP1_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_18, 192BIT)",                       STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_19, 256BIT)",                       STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_20, 192BIT)",                         STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_21, 256BIT)",                         STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM,AES_22, 192BIT)",              STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM,AES_23, 256BIT)",              STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_18, 192BIT)",                       STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (AES_19, 256BIT)",                       STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_20, 192BIT)",                         STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_21, 256BIT)",                         STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM,AES_22, 192BIT)",              STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM,AES_23, 256BIT)",              STEP3_ENABLE     },
    {SERVICE_03_AES_KEY_UPDATE_PLAIN,   "SERVICE_03_AES_KEY_UPDATE_PLAIN (192bit)",                     STEP3_ENABLE     },
    {SERVICE_03_AES_KEY_UPDATE_PLAIN,   "SERVICE_03_AES_KEY_UPDATE_PLAIN (256bit)",                     STEP3_ENABLE     },
    {SERVICE_01_ECDH,                   "SERVICE_01_ECDH (192bit, AES_RAM_2)",                          STEP3_ENABLE     },
    {SERVICE_01_ECDH,                   "SERVICE_01_ECDH (256bit, AES_RAM_3)",                          STEP3_ENABLE     },
    {SERVICE_01_EDDSA_SIGN,             "SERVICE_01_EDDSA_SIGN",                                        STEP3_ENABLE     },
    {SERVICE_01_EDDSA_VERIFY,           "SERVICE_01_EDDSA_VERIFY",                                      STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_26, priority Highest)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_27, priority Highest)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC Verification AES_28)",           STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Enc,AES_29, 192BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Dec,AES_29, 192BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Enc,AES_30, 192BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Dec,AES_30, 192BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Enc,CCM,AES_33, 192BIT)",      STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Dec,CCM,AES_33, 192BIT)",      STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_34, priority Highest)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_35, priority Highest)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC Verification AES_36)",           STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Enc,AES_37, 256BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Dec,AES_37, 256BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Enc,AES_38, 256BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Dec,AES_38, 256BIT)",               STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS,AES_39,AES_40, 256BIT)",            STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Enc,AES_41, 256BIT)",          STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Dec,AES_41, 256BIT)",          STEP3_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC Verification AES_28,slice)",     STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Enc,AES_29, 192BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Dec,AES_29, 192BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Enc,AES_30, 192BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Dec,AES_30, 192BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Enc,AES_33, 192BIT,slice)",    0     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Dec,AES_33, 192BIT,slice)",    0     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC Verification AES_36,slice)",     STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Enc,AES_37, 256BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CTR_Dec,AES_37, 256BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Enc,AES_38, 256BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC_Dec,AES_38, 256BIT,slice)",         STEP3_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS,AES_39,AES_40, 256BIT,slice)",      STEP3_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Enc,AES_41, 256BIT,slice)",    0     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM_Dec,AES_41, 256BIT,slice)",    0     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC DTF_GEN_128)",                   SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC DTF_GEN_192)",                   SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC DTF_GEN_256)",                   SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC DTF_VER_128)",                   SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC DTF_VER_192)",                   SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC DTF_VER_256)",                   SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC ICUP_GEN_128)",                  SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC ICUP_GEN_192)",                  SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC ICUP_GEN_256)",                  SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC ICUP_VER_128)",                  SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC ICUP_VER_192)",                  SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC ICUP_VER_256)",                  SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC DTF_ENC_128)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC ICUP_ENC_128)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC DTF_ENC_192)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC ICUP_ENC_192)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC DTF_ENC_256)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC ICUP_ENC_256)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC DTF_DEC_128)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC ICUP_DEC_128)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC DTF_DEC_192)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC ICUP_DEC_192)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC DTF_DEC_256)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC ICUP_DEC_256)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS DTF_ENC_128)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS DTF_ENC_256)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS DTF_DEC_128)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS DTF_DEC_256)",                      SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS ICUP_ENC_128)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS ICUP_ENC_256)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS ICUP_DEC_128)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS ICUP_DEC_256)",                     SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM DTF_ENC_128)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM DTF_ENC_192)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM DTF_ENC_256)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM DTF_DEC_128)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM DTF_DEC_192)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM DTF_DEC_256)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM ICUP_ENC_128)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM ICUP_ENC_192)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM ICUP_ENC_256)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM ICUP_DEC_128)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM ICUP_DEC_192)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM ICUP_DEC_256)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM DTF_ENC_128)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM DTF_ENC_192)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM DTF_ENC_256)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM DTF_DEC_128)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM DTF_DEC_192)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM DTF_DEC_256)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM ICUP_ENC_128)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM ICUP_ENC_192)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM ICUP_ENC_256)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM ICUP_DEC_128)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM ICUP_DEC_192)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (CCM ICUP_DEC_256)",                SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC slice_GEN_128)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC slice_GEN_192)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC slice_GEN_256)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC slice_VER_128)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC slice_VER_192)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CMAC,               "SERVICE_01_AES_CMAC (AES_CMAC slice_VER_256)",                 SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC slice_ENC_128)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC slice_ENC_192)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC slice_ENC_256)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC slice_DEC_128)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC slice_DEC_192)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (CBC slice_DEC_256)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS slice_ENC_128)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS slice_ENC_256)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS slice_DEC_128)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_CIPHER,             "SERVICE_01_AES_CIPHER (XTS slice_DEC_256)",                    SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM slice_ENC_128)",               SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM slice_ENC_192)",               SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM slice_ENC_256)",               SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM slice_DEC_128)",               SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM slice_DEC_192)",               SWEEP_ONLY_ENABLE     },
    {SERVICE_01_AES_AUTH_CIPHER,        "SERVICE_01_AES_AUTH_CIPHER (GCM slice_DEC_256)",               SWEEP_ONLY_ENABLE     }
};
#endif

extern uint32_t (*cnt_result_pe2)[];
extern unsigned long long cnt_trigger_service_request_pe2;
extern unsigned long long cnt_recieve_service_response_pe2;
extern unsigned long long cnt_recieve_service_response_pe2;
extern unsigned long long cnt_request_queue_full_pe2;
extern unsigned long long cnt_other_request_error_pe2;
extern unsigned long long cnt_if_lib_not_init_pe2;
extern unsigned long long cnt_software_error_pe2;

extern uint32_t (*cnt_result_pe3)[];
extern unsigned long long cnt_trigger_service_request_pe3;
extern unsigned long long cnt_recieve_service_response_pe3;
extern unsigned long long cnt_recieve_service_response_pe3;
extern unsigned long long cnt_request_queue_full_pe3;
extern unsigned long long cnt_other_request_error_pe3;
extern unsigned long long cnt_if_lib_not_init_pe3;
extern unsigned long long cnt_software_error_pe3;

extern uint32_t (*cnt_result_pe4)[];
extern unsigned long long cnt_trigger_service_request_pe4;
extern unsigned long long cnt_recieve_service_response_pe4;
extern unsigned long long cnt_recieve_service_response_pe4;
extern unsigned long long cnt_request_queue_full_pe4;
extern unsigned long long cnt_other_request_error_pe4;
extern unsigned long long cnt_if_lib_not_init_pe4;
extern unsigned long long cnt_software_error_pe4;

extern uint32_t (*cnt_result_pe5)[];
extern unsigned long long cnt_trigger_service_request_pe5;
extern unsigned long long cnt_recieve_service_response_pe5;
extern unsigned long long cnt_recieve_service_response_pe5;
extern unsigned long long cnt_request_queue_full_pe5;
extern unsigned long long cnt_other_request_error_pe5;
extern unsigned long long cnt_if_lib_not_init_pe5;
extern unsigned long long cnt_software_error_pe5;

extern uint32_t (*cnt_result_pe6)[];
extern unsigned long long cnt_trigger_service_request_pe6;
extern unsigned long long cnt_recieve_service_response_pe6;
extern unsigned long long cnt_recieve_service_response_pe6;
extern unsigned long long cnt_request_queue_full_pe6;
extern unsigned long long cnt_other_request_error_pe6;
extern unsigned long long cnt_if_lib_not_init_pe6;
extern unsigned long long cnt_software_error_pe6;

// ********************************************************************************
// Prototypes
// ********************************************************************************
void icum_test_pe1_heat_run_parallel_pe(void);
void icum_test_pe2_heat_run_parallel_pe(void);
void icum_test_sweep(void);

#endif //if !defined(PARALLEL_PE_HEAT_RUN_H)
