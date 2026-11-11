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
    uint8_t test_char[64];
    unsigned char key[16];
    unsigned char IV[128];
    unsigned char aad[128];
    unsigned char plain_text[128];
    unsigned char cipher_text[128];
    unsigned char auth_tag[16];
    uint32_t      length_aad;
    uint32_t      length_plain_text;
    uint32_t      length_iv_text;
    uint8_t       length_auth_tag;
} type_test_vector;

static const type_test_vector test_vector[] =
{

/* from NIST Test Vector gcmEncryptExtIV128.rsp   */

/* [IVlen = 96]                                   */
/* [PTlen = 128]                                  */
/* [AADlen = 0]                                   */
/* [Taglen = 128]                                 */
/* Count = 0                                      */
    {
        {"IV = 96, PT = 128, AAD = 0, Tag = 128"},
        // key
        { 0x7f, 0xdd, 0xb5, 0x74, 0x53, 0xc2, 0x41, 0xd0,
          0x3e, 0xfb, 0xed, 0x3a, 0xc4, 0x4e, 0x37, 0x1c },
        // IV
        { 0xee, 0x28, 0x3a, 0x3f, 0xc7, 0x55, 0x75, 0xe3,
          0x3e, 0xfd, 0x48, 0x87 },
        // AAD
        { 0x00  },
        // plain text
        { 0xd5, 0xde, 0x42, 0xb4, 0x61, 0x64, 0x6c, 0x25,
          0x5c, 0x87, 0xbd, 0x29, 0x62, 0xd3, 0xb9, 0xa2 },
        // cipher text
        { 0x2c, 0xcd, 0xa4, 0xa5, 0x41, 0x5c, 0xb9, 0x1e,
          0x13, 0x5c, 0x2a, 0x0f, 0x78, 0xc9, 0xb2, 0xfd },
        // tag
        { 0xb3, 0x6d, 0x1d, 0xf9, 0xb9, 0xd5, 0xe5, 0x96, 
          0xf8, 0x3e, 0x8b, 0x7f, 0x52, 0x97, 0x1c, 0xb3 },
        // length AAD
        0,
        // length plain text
        128,
        // length iv text
        96,
        // length tag
        128
    },
/* [IVlen = 96]                                   */
/* [PTlen = 408]                                  */
/* [AADlen = 160]                                 */
/* [Taglen = 128]                                 */
/* Count = 0                                      */
    {
        {"IV = 96, PT = 408, AAD = 160, Tag = 128"},
        // key
        { 0xfe, 0x47, 0xfc, 0xce, 0x5f, 0xc3, 0x26, 0x65,
          0xd2, 0xae, 0x39, 0x9e, 0x4e, 0xec, 0x72, 0xba },
        // IV
        { 0x5a, 0xdb, 0x96, 0x09, 0xdb, 0xae, 0xb5, 0x8c,
          0xbd, 0x6e, 0x72, 0x75 },
        // AAD
        { 0x88, 0x31, 0x9d, 0x6e, 0x1d, 0x3f, 0xfa, 0x5f,
          0x98, 0x71, 0x99, 0x16, 0x6c, 0x8a, 0x9b, 0x56,
          0xc2, 0xae, 0xba, 0x5a },
        // plain text
        { 0x7c, 0x0e, 0x88, 0xc8, 0x88, 0x99, 0xa7, 0x79, 
          0x22, 0x84, 0x65, 0x07, 0x47, 0x97, 0xcd, 0x4c,
          0x2e, 0x14, 0x98, 0xd2, 0x59, 0xb5, 0x43, 0x90,
          0xb8, 0x5e, 0x3e, 0xef, 0x1c, 0x02, 0xdf, 0x60,
          0xe7, 0x43, 0xf1, 0xb8, 0x40, 0x38, 0x2c, 0x4b,
          0xcc, 0xaf, 0x3b, 0xaf, 0xb4, 0xca, 0x84, 0x29,
          0xbe, 0xa0, 0x63 },
        // cipher text
        { 0x98, 0xf4, 0x82, 0x6f, 0x05, 0xa2, 0x65, 0xe6,
          0xdd, 0x2b, 0xe8, 0x2d, 0xb2, 0x41, 0xc0, 0xfb,
          0xbb, 0xf9, 0xff, 0xb1, 0xc1, 0x73, 0xaa, 0x83,
          0x96, 0x4b, 0x7c, 0xf5, 0x39, 0x30, 0x43, 0x73,
          0x63, 0x65, 0x25, 0x3d, 0xdb, 0xc5, 0xdb, 0x87,
          0x78, 0x37, 0x14, 0x95, 0xda, 0x76, 0xd2, 0x69,
          0xe5, 0xdb, 0x3e },
        // tag
        { 0x29, 0x1e, 0xf1, 0x98, 0x2e, 0x4d, 0xef, 0xed,
          0xaa, 0x22, 0x49, 0xf8, 0x98, 0x55, 0x6b, 0x47 },
        // length AAD
        160,
        // length plain text
        408,
        // length iv text
        96,
        // length tag
        128
    },

/* [IVlen = 1024]                                 */
/* [PTlen = 408]                                  */
/* [AADlen = 720]                                 */
/* [Taglen = 32 ]                                 */
/* Count = 0                                      */
    {
        {"IV = 1024, PT = 408, AAD = 720, Tag = 32"},
        // key
        { 0x26, 0x33, 0xd1, 0x78, 0x1c, 0xe5, 0x4f, 0x74, 0xac, 0x60, 0x9a, 0x5b, 0x52, 0x09, 0xa0, 0x1f },
        // IV
        { 0x7d, 0x0e, 0x90, 0xb7, 0xe9, 0xf3, 0x6f, 0x76, 0x0d, 0x2d, 0xcb, 0xd6, 0x6f, 0x35, 0x2d, 0xf4,
          0x5f, 0x39, 0x17, 0xaf, 0xdb, 0xe1, 0xd0, 0xa8, 0x9c, 0xc4, 0x4b, 0xe0, 0xbd, 0x85, 0xcf, 0x8b,
          0xf7, 0x5e, 0xdb, 0xdd, 0x33, 0xf1, 0xd1, 0x6d, 0xad, 0x02, 0x82, 0x4d, 0x81, 0x38, 0x92, 0x10,
          0xb0, 0xf1, 0x46, 0xf3, 0xdf, 0x63, 0xf9, 0x23, 0x2d, 0x70, 0x35, 0xeb, 0x9e, 0x82, 0x97, 0xa0,
          0x94, 0x74, 0x98, 0x5b, 0x3e, 0x03, 0x8a, 0x5f, 0xa6, 0x84, 0x01, 0x55, 0xd8, 0x84, 0x8f, 0xc7,
          0xc5, 0x30, 0x61, 0xba, 0x0f, 0x44, 0x2b, 0x84, 0x40, 0x86, 0x60, 0xa9, 0x97, 0x17, 0x6c, 0xa5,
          0xbf, 0x34, 0x73, 0x10, 0x3f, 0xd3, 0xc9, 0xa1, 0xde, 0x25, 0x80, 0xb9, 0xe5, 0x39, 0xaf, 0x87,
          0x22, 0x59, 0xec, 0xae, 0x92, 0x5a, 0x8e, 0xf5, 0x0f, 0x5a, 0x17, 0x6a, 0x06, 0x9b, 0x1f, 0xb8 },
        // AAD
        { 0x58, 0x4c, 0x3c, 0xad, 0x30, 0x35, 0xd1, 0x42, 0x7d, 0x6f, 0x5f, 0x1b, 0x26, 0x1e, 0x97, 0xa5,
          0xea, 0x7d, 0x97, 0xc0, 0xb8, 0x8c, 0xed, 0xf3, 0xb1, 0xaa, 0x5e, 0x21, 0xe5, 0x91, 0x68, 0x05,
          0xa6, 0x39, 0x64, 0xea, 0xb4, 0x44, 0x9d, 0x88, 0x06, 0xe7, 0xaf, 0x60, 0x61, 0x84, 0x65, 0xcf,
          0x39, 0xf8, 0x27, 0x69, 0xb7, 0x52, 0x8b, 0xba, 0x9b, 0xb9, 0xc0, 0x49, 0x92, 0xcd, 0x7b, 0x9e,
          0x26, 0xef, 0xe9, 0xbe, 0x38, 0xe1, 0xbf, 0xee, 0xb4, 0x16, 0x78, 0xc5, 0x2d, 0x5b, 0xa3, 0x50,
          0x8f, 0xd7, 0xa2, 0xb1, 0xe8, 0x47, 0x85, 0x05, 0xbf, 0xde },
        // plain text
        { 0xae, 0x69, 0x58, 0x28, 0x62, 0x5b, 0x26, 0x4e, 0x0b, 0x13, 0xd3, 0xc9, 0xa5, 0x39, 0xf2, 0xcf,
          0x30, 0x6a, 0x75, 0x01, 0xcd, 0xd3, 0x5b, 0x81, 0x7b, 0x69, 0x9b, 0x2d, 0x7c, 0x25, 0xcf, 0x20,
          0xd2, 0xdc, 0xee, 0xc3, 0xfa, 0x88, 0x30, 0x19, 0xdb, 0x80, 0x72, 0x72, 0xfd, 0xdf, 0xdc, 0xa8,
          0xe7, 0xf6, 0x72},
        // cipher text
        { 0xfb, 0xc3, 0x2a, 0x56, 0x88, 0x51, 0x00, 0xa3, 0x6c, 0x27, 0x6f, 0xf3, 0x68, 0xdb, 0x92, 0x36,
          0x90, 0x60, 0x21, 0xa8, 0xcc, 0x75, 0x00, 0xf2, 0xb3, 0xe7, 0x8a, 0x6c, 0xa0, 0x15, 0x46, 0x82,
          0x70, 0x73, 0xff, 0x11, 0x03, 0x14, 0x5f, 0x13, 0x9f, 0x4d, 0x11, 0x6e, 0xb4, 0x7b, 0x84, 0xe3,
          0x3c, 0x71, 0x60},
        // tag
        { 0x49, 0x58, 0x9b, 0x3a },
        // length AAD
        720,
        // length plain text
        408,
        // length iv text
        1024,
        // length tag
        32
    },

/* [IVlen = 8]                                    */
/* [PTlen = 128]                                  */
/* [AADlen = 160]                                 */
/* [Taglen = 128]                                 */
/* Count = 0                                      */
    {
        {"IV = 8, PT = 128, AAD = 160, Tag = 128"},
        // key
        { 0x6d, 0xba, 0x65, 0xdb, 0xa3, 0x31, 0x2e, 0x9b, 0x90, 0x7a, 0x49, 0xbc, 0x46, 0x90, 0x23, 0x17 },
        // IV
        { 0x39 },
        // AAD
        { 0x4e, 0xf6, 0xb5, 0x50, 0xad, 0x86, 0x82, 0x87, 0x3a, 0xd2, 0x9b, 0x53, 0x04, 0xd3, 0x48, 0xd3, 0x0a, 0x36, 0x23, 0xb0 },
        // plain text
        { 0xef, 0x7e, 0x92, 0x1f, 0x46, 0x62, 0x4c, 0x69, 0x3e, 0x0f, 0x8d, 0x90, 0x8b, 0x8b, 0x98, 0x0a },
        // cipher text
        { 0xb7, 0x61, 0x89, 0x56, 0xd4, 0x0f, 0x9a, 0xbc, 0x28, 0xe8, 0x35, 0x04, 0x40, 0x2a, 0x4a, 0x76 },
        // tag
        { 0xc0, 0xa4, 0x5a, 0x7b, 0x39, 0x84, 0xd9, 0xb9, 0xa5, 0x12, 0x09, 0x1a, 0xd7, 0x4d, 0x2d, 0x8a },
        // length AAD
        160,
        // length plain text
        128,
        // length iv text
        8,
        // length tag
        128
    },

/* [IVlen = 96]                                   */
/* [PTlen = 0]                                    */
/* [AADlen = 128]                                 */
/* [Taglen = 128]                                 */
/* Count = 0                                      */
    {
        {"IV = 96, PT = 0, AAD = 128, Tag = 128"},
        // key
        { 0x77, 0xbe, 0x63, 0x70, 0x89, 0x71, 0xc4, 0xe2, 0x40, 0xd1, 0xcb, 0x79, 0xe8, 0xd7, 0x7f, 0xeb },
        // IV
        { 0xe0, 0xe0, 0x0f, 0x19, 0xfe, 0xd7, 0xba, 0x01, 0x36, 0xa7, 0x97, 0xf3 },
        // AAD
        { 0x7a, 0x43, 0xec, 0x1d, 0x9c, 0x0a, 0x5a, 0x78, 0xa0, 0xb1, 0x65, 0x33, 0xa6, 0x21, 0x3c, 0xab },
        // plain text
        { 0 },
        // cipher text
        { 0 },
        // tag
        { 0x20, 0x9f, 0xcc, 0x8d, 0x36, 0x75, 0xed, 0x93, 0x8e, 0x9c, 0x71, 0x66, 0x70, 0x9d, 0xd9, 0x46 },
        // length AAD
        128,
        // length plain text
        0,
        // length iv text
        96,
        // length tag
        128
    },

/* [IVlen = 8]                                    */
/* [PTlen = 0]                                    */
/* [AADlen = 384]                                 */
/* [Taglen = 96 ]                                 */
/* Count = 0                                      */
    {
        {"IV = 8, PT = 0, AAD = 384, Tag = 96"},
        // key
        { 0x9a, 0x87, 0x7c, 0x98, 0xda, 0x09, 0x4a, 0xd1, 0xd4, 0xd2, 0xc4, 0x8f, 0xbd, 0xc2, 0x62, 0xb5 },
        // IV
        { 0xa6 },
        // AAD
        { 0xe5, 0x5e, 0x90, 0x63, 0x83, 0x10, 0x13, 0x35, 0xac, 0xda, 0x0e, 0x59, 0x34, 0x32, 0xf0, 0xe3,
          0xc5, 0x8c, 0x51, 0x15, 0x6b, 0x0c, 0xbf, 0x25, 0x73, 0x93, 0xd6, 0x24, 0x38, 0x08, 0xa8, 0xe8,
          0x9d, 0xe2, 0x26, 0x40, 0x76, 0x6f, 0xb7, 0x14, 0x97, 0x37, 0x50, 0xb1, 0xfe, 0x40, 0x7f, 0x35 },
        // plain text
        { 0 },
        // cipher text
        { 0 },
        // tag
        { 0x10, 0x2c, 0xa0, 0x33, 0xa3, 0x54, 0x84, 0x86, 0x1f, 0x33, 0x09, 0x31 },
        // length AAD
        384,
        // length plain text
        0,
        // length iv text
        8,
        // length tag
        96
    },

/* [IVlen = 96]                                   */
/* [PTlen = 0]                                    */
/* [AADlen = 0]                                   */
/* [Taglen = 128 ]                                */
/* Count = 0                                      */
    {
        {"IV = 96, PT = 0, AAD = 0, Tag = 128"},
        // key
        { 0x11, 0x75, 0x4c, 0xd7, 0x2a, 0xec, 0x30, 0x9b, 0xf5, 0x2f, 0x76, 0x87, 0x21, 0x2e, 0x89, 0x57 },
        // IV
        { 0x3c, 0x81, 0x9d, 0x9a, 0x9b, 0xed, 0x08, 0x76, 0x15, 0x03, 0x0b, 0x65 },
        // AAD
        { 0 },
        // plain text
        { 0 },
        // cipher text
        { 0 },
        // tag
        { 0x25, 0x03, 0x27, 0xc6, 0x74, 0xaa, 0xf4, 0x77, 0xae, 0xf2, 0x67, 0x57, 0x48, 0xcf, 0x69, 0x71 },
        // length AAD
        0,
        // length plain text
        0,
        // length iv text
        96,
        // length tag
        128
    },
/* [IVlen = 1024]                                 */
/* [PTlen = 0]                                    */
/* [AADlen = 0]                                   */
/* [Taglen = 32]                                  */
/* Count = 0                                      */
    {
        {"IV = 1024, PT = 0, AAD = 0, Tag = 32"},
        /* key */
        { 0x94, 0x40, 0xcc, 0xa7, 0xf5, 0xc1, 0x4a, 0xf6, 0x4f, 0xfc, 0x0f, 0xc2, 0xc7, 0x4d, 0xb0, 0xcb },
        /* IV */
        { 0xce, 0x47, 0x72, 0xe8, 0x21, 0x7f, 0x7a, 0x3b, 0x9b, 0xc2, 0x48, 0xc1, 0x00, 0xb1, 0x1b, 0x57,
          0x4a, 0x2a, 0x56, 0x8e, 0xbc, 0x29, 0xba, 0x2b, 0xed, 0x1f, 0x1a, 0x10, 0x3b, 0x7d, 0xa5, 0x3c,
          0x90, 0x6e, 0xad, 0xa4, 0xa1, 0x54, 0x7a, 0x40, 0xc3, 0xf1, 0x92, 0x9d, 0x99, 0xde, 0x74, 0x3e,
          0x62, 0x0b, 0x4e, 0x50, 0x41, 0xb5, 0x4e, 0x7a, 0x80, 0x2f, 0x70, 0x67, 0x0e, 0x33, 0x50, 0x07,
          0x51, 0xe0, 0x30, 0x32, 0x7e, 0x64, 0x0d, 0xf6, 0xf0, 0x6b, 0x62, 0x83, 0x76, 0xfa, 0xa6, 0x2a,
          0xc8, 0x2d, 0x67, 0x55, 0xc1, 0x8d, 0x6e, 0x32, 0x26, 0x03, 0x71, 0x5e, 0x18, 0x10, 0x9e, 0xef,
          0x0f, 0xe7, 0x53, 0x4f, 0xb6, 0xa5, 0x5b, 0x60, 0xe1, 0xad, 0x8d, 0x2a, 0x69, 0x7a, 0x1f, 0xf2,
          0xb8, 0x54, 0x30, 0x0e, 0xd5, 0x89, 0x3f, 0x39, 0x75, 0xa6, 0x05, 0x7d, 0x7a, 0x39, 0x23, 0x39 },
        /* AAD */
        { 0 },
        /* plain text */
        { 0 },
        /* cipher text */
        { 0 },
        /* tag */
        { 0x79, 0xb5, 0x6f, 0x56 },
        /* length AAD */
        0,
        /* length plain text */
        0,
        /* length iv text */
        1024,
        /* length tag */
        32
    },
/* [IVlen = 1024]                                  */
/* [PTlen = 256]                                   */
/* [AADlen = 0]                                    */
/* [Taglen = 32]                                   */
    {
        {"IV = 1024, PT = 256, AAD = 0, Tag = 32"},
        /* key */
        { 0xda, 0x75, 0x92, 0xf9, 0xc9, 0x3f, 0x71, 0xda, 0xb7, 0xea, 0xf4, 0x02, 0xb2, 0x40, 0x76, 0xd1 },
        /* IV */
        { 0xcb, 0x1e, 0x7a, 0x18, 0x43, 0xe5, 0xa7, 0xf8, 0x7a, 0x74, 0x76, 0x7c, 0x0e, 0x10, 0x9f, 0xbf,
          0xd9, 0x4f, 0xbc, 0x35, 0x87, 0x4c, 0xa1, 0x39, 0x54, 0x2d, 0xf3, 0xa1, 0xe3, 0x07, 0x15, 0xff,
          0xb9, 0x88, 0xd3, 0xd0, 0x68, 0x0e, 0x36, 0xc6, 0xe3, 0xa3, 0xa1, 0x8c, 0x7f, 0xf6, 0xb3, 0xd3,
          0x89, 0x53, 0x49, 0x6d, 0x2d, 0x32, 0x27, 0x57, 0xad, 0x56, 0x57, 0xeb, 0x64, 0xa0, 0x67, 0x57,
          0x26, 0xbb, 0xb2, 0x38, 0x27, 0xa2, 0x40, 0x9d, 0x0b, 0x2b, 0x23, 0xd7, 0x21, 0x94, 0x66, 0x79,
          0xa7, 0x23, 0xd4, 0xcb, 0x99, 0x9e, 0x08, 0x33, 0x9b, 0xc9, 0x71, 0xee, 0x85, 0x6e, 0xfe, 0x70,
          0x8a, 0x74, 0x53, 0x77, 0xb2, 0x3c, 0x78, 0xde, 0xc9, 0x17, 0xc5, 0x55, 0x01, 0x08, 0x1f, 0xe2,
          0x0b, 0x0b, 0x33, 0x75, 0xe2, 0xd8, 0xb6, 0x71, 0xfc, 0xdf, 0xc0, 0x46, 0xdf, 0x4e, 0x2d, 0x0a },
        /* AAD */
        { 0 },
        /* plain text */
        { 0xce, 0xa5, 0x36, 0x60, 0x87, 0x90, 0xf2, 0x66, 0x60, 0xc2, 0x6b, 0xe7, 0x95, 0x35, 0x7d, 0x00,
          0xba, 0x03, 0x02, 0x95, 0xfe, 0x5b, 0xa2, 0xbc, 0x88, 0xe3, 0x24, 0x9f, 0xf1, 0x53, 0x72, 0xd3 },
        /* cipher text */
        { 0xb2, 0xb8, 0x11, 0x0e, 0x18, 0x44, 0x7c, 0xf6, 0x77, 0xf0, 0xf2, 0xf7, 0xd6, 0x92, 0x34, 0x83,
          0x92, 0x1c, 0xd1, 0xa6, 0x8e, 0x61, 0x53, 0x90, 0x6f, 0xaf, 0x15, 0x93, 0x91, 0xf0, 0xa0, 0xc8 },
        /* tag */
        { 0x44, 0x0a, 0x00, 0xef },
        /* length AAD */
        0,
        /* length plain text */
        256,
        /* length iv text */
        1024,
        /* length tag */
        32
    },
/* [IVlen = 96]                                    */
/* [PTlen = 408]                                   */
/* [AADlen = 0]                                    */
/* [Taglen = 32]                                   */
    {
        {"IV = 96, PT = 408, AAD = 0, Tag = 32"},
        /* key */
        { 0x6e, 0xd9, 0x71, 0xb8, 0x5e, 0x07, 0xa5, 0xec, 0x47, 0x57, 0xf2, 0xac, 0x31, 0x80, 0x2e, 0x84 },
        /* IV */
        { 0x27, 0x7d, 0x38, 0xcd, 0x09, 0xc4, 0xa5, 0x42, 0x86, 0x2d, 0x02, 0x55 },
        /* AAD */
        { 0 },
        /* plain text */
        { 0xaf, 0x2d, 0x72, 0xee, 0x86, 0x87, 0xb6, 0xc4, 0x97, 0xa5, 0xea, 0x5b, 0x15, 0x99, 0xb4, 0x82,
          0x0a, 0xed, 0xaf, 0xbc, 0xd4, 0x85, 0x67, 0x28, 0x6b, 0xc9, 0x99, 0xe1, 0x37, 0x88, 0xc0, 0x0b,
          0x71, 0x99, 0x51, 0x7f, 0x31, 0x74, 0x02, 0x97, 0x75, 0x25, 0x23, 0xfe, 0x30, 0x15, 0x79, 0xde,
          0xa7, 0x81, 0xc2},
        /* cipher text */
        { 0x62, 0xe0, 0x21, 0x8f, 0xf3, 0xc8, 0x17, 0xf1, 0x18, 0x1e, 0x27, 0xc4, 0x10, 0x2e, 0x8e, 0x0e,
          0x69, 0xb9, 0xac, 0xda, 0xec, 0x47, 0x02, 0x0a, 0xd8, 0x96, 0x8c, 0x97, 0x64, 0x2b, 0x06, 0xea,
          0x28, 0x9e, 0x54, 0xa9, 0xfd, 0xa4, 0x09, 0x42, 0xf4, 0xf0, 0x76, 0x3a, 0xdf, 0x9c, 0xc4, 0xae,
          0xd2, 0xc0, 0x75},
        /* tag */
        { 0x48, 0x12, 0xd1, 0xf8 },
        /* length AAD */
        0,
        /* length plain text */
        408,
        /* length iv text */
        96,
        /* length tag */
        32
    },
};

// ********************************************************************************
// Test functions
// ********************************************************************************

/*===========================================================================
  ICU-M self-test function
  [PUBLIC]
  ===========================================================================*/

int icum_self_test_gcm(void)
{
    r_icumif_isd_t *p_ISD;
    int n = 10;
    int i;
    int err;
    
    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    // clear error counter
    err = 0;
    
    printf("Testing GCM encryption...\n\n"); FLUSH();
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
        p_ISD->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_GCM;
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
        p_ISD->prm.AES_AUTH_CIPHER.job_slice               = job_slice[JOB_AES];

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
        p_ISD->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_GCM;
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
        p_ISD->prm.AES_AUTH_CIPHER.job_slice               = job_slice[JOB_AES];

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

    printf("Note: job slice = 2 block(s)\n\n");
    printf("Length data [%s] \n",test_vector[1].test_char); FLUSH();

    // initialize the RAM key 0
    p_ISD->service_id                  = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD->p_callbackfunc              = 0;
    p_ISD->job_id                      = 0;
    
    p_ISD->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES_RAM;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_id                   = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
    p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value              = test_vector[1].key;
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
    p_ISD->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_GCM;
    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[1].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[1].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[1].plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = test_vector[1].length_plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[1].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[1].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)BIG_BUFFER[1];
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[1].length_auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
    p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 2;

    trigger_service_and_wait(p_ISD);

    if(p_ISD->service_result == SERV_OK)
    {
        if(memcmp(test_vector[1].auth_tag , p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, (p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)) != 0)
        {
            printf(" failed\n");
            err++;
        }
        else
        {
            if(memcmp(test_vector[1].cipher_text, p_ISD->prm.AES_AUTH_CIPHER.p_data_out, (test_vector[1].length_plain_text / 8)) != 0)
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
    p_ISD->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_GCM;
    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[1].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[1].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[1].cipher_text;
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = test_vector[1].length_plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[1].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[1].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t*)test_vector[1].auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[1].length_auth_tag;
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
            if(memcmp(test_vector[1].plain_text, p_ISD->prm.AES_AUTH_CIPHER.p_data_out, (test_vector[1].length_plain_text / 8)) != 0)
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
    printf("Length data [%s] \n",test_vector[1].test_char); FLUSH();
    printf("Testing ciphering & tag generation\n ");
    FLUSH();

    p_ISD->service_id                  = SERVICE_01_AES_AUTH_CIPHER;
    p_ISD->p_callbackfunc              = 0;
    p_ISD->job_id                      = 0;

    p_ISD->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES_RAM;
    p_ISD->prm.AES_AUTH_CIPHER.key_id                  = 0;
    p_ISD->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_ENCRYPTION;
    p_ISD->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_GCM;
    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[1].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[1].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[1].plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = test_vector[1].length_plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[1].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[1].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)BIG_BUFFER[1];
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[1].length_auth_tag;
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

    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[1].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[1].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[1].plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = 256;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[1].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[1].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)BIG_BUFFER[1];
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[1].length_auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_UPDATE;
    p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 1;

    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result == SERV_OK)
    {
        if(memcmp(BIG_BUFFER[0] , (test_vector[1].cipher_text), 32) != 0)
        {
            printf(" JOB_UPDATE : failed\n");
            err++;
        }
        else
        {
            printf(" JOB_UPDATE : pass\n");
        }
    }

    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[1].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[1].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)&test_vector[1].plain_text[32];
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = 152;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[1].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[1].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)BIG_BUFFER[1];
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[1].length_auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_FINISH;
    p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 2;

    trigger_service_and_wait(p_ISD);

    if(p_ISD->service_result == SERV_OK)
    {
        if(memcmp(BIG_BUFFER[0] , (&test_vector[1].cipher_text[32]), 19) != 0)
        {
            printf(" JOB_FINISH : failed\n");
            err++;
        }
        else
        {
            printf(" JOB_FINISH : pass\n");
        }
       if(memcmp(test_vector[1].auth_tag , p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, (p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)) != 0)
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
    p_ISD->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_GCM;
    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[1].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[1].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[1].cipher_text;
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = test_vector[1].length_plain_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[1].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[1].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t*)test_vector[1].auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[1].length_auth_tag;
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

    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[1].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[1].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)test_vector[1].cipher_text;
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = 256;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[1].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[1].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t*)test_vector[1].auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[1].length_auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_UPDATE;
    p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 1;

    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result == SERV_OK)
    {
        if(memcmp(BIG_BUFFER[0] , (test_vector[1].plain_text), 32) != 0)
        {
            printf(" JOB_UPDATE : failed\n");
            err++;
        }
        else
        {
            printf(" JOB_UPDATE : pass\n");
        }
    }

    p_ISD->prm.AES_AUTH_CIPHER.p_iv                    = (uint32_t *)test_vector[1].IV;
    p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits         = test_vector[1].length_iv_text;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_in               = (uint32_t *)&test_vector[1].cipher_text[32];
    p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits       = 152;
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_data             = (uint32_t *)test_vector[1].aad;
    p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = test_vector[1].length_aad;
    p_ISD->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t*)test_vector[1].auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = test_vector[1].length_auth_tag;
    p_ISD->prm.AES_AUTH_CIPHER.job_cycle               = JOB_FINISH;
    p_ISD->prm.AES_AUTH_CIPHER.job_slice               = 2;

    trigger_service_and_wait(p_ISD);

    if(p_ISD->service_result == SERV_OK)
    {
        if(memcmp(BIG_BUFFER[0] , (&test_vector[1].plain_text[32]), 19) != 0)
        {
            printf(" JOB_FINISH : failed\n");
            err++;
        }
        else
        {
            printf(" JOB_FINISH : pass\n");
        }
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

