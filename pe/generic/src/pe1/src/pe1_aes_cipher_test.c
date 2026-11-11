/******************************************************************************/
/* Component Name   Testing the ICUM                                          */
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
#include "renesas_types.h"

#include "r_icumif_api.h"

#include "pe1.h"
#include "shared.h"
#include "vrun.api.h"

#include "user_icumif_api.h"

// ********************************************************************************
// Local constants
// ********************************************************************************

#define ALIGNVAR(n) __attribute__ ((aligned(n)))

typedef struct
{
    uint32_t                ALIGNVAR(256) plain_text[4*4];
    uint32_t                ALIGNVAR(256) cipher_text[4*4];
    uint32_t                iv[4];
    cipher_modes_t          cipher_mode;
    cipher_direction_t      direction;
    r_key_group_t           key_group;
    uint8_t                 key_id;
    uint8_t                 length_plain_block;
    r_job_slice_t           job_slice;
    r_job_cycle_t           job_cycle;
    uint8_t                 padding[28];
} type_test_vector;

static const type_test_vector ALIGNVAR(32) test_vector[] =
{
    /* No.1 */
    // CBC Enc RAMKey 0
    {
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
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CBC,     CIPHER_DIR_ENCRYPTION,      KEY_GRP_AES_RAM,        1,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.2 */
    // CBC Dec AESKey 0
    {
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
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CBC,     CIPHER_DIR_DECRYPTION,      KEY_GRP_AES_RAM,        1,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.3 */
    // ECB Enc ROMKey 0
    {
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
        // iv
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_ECB,     CIPHER_DIR_ENCRYPTION,      KEY_GRP_AES_RAM,        2,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.4 */
    // ECB Dec RAMKey 2
    {
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
        // iv
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_ECB,     CIPHER_DIR_DECRYPTION,      KEY_GRP_AES_RAM,        3,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.5 */
    // CFB Enc AESKey 19
    {
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
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CFB,     CIPHER_DIR_ENCRYPTION,      KEY_GRP_AES_RAM,        4,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.6 */
    // CFB Dec ROMKey 1
    {
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
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CFB,     CIPHER_DIR_DECRYPTION,      KEY_GRP_AES_RAM,        1,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.7 */
    // OFB Enc RAMKey 0
    {
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
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_OFB,     CIPHER_DIR_ENCRYPTION,      KEY_GRP_AES_RAM,        1,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.8 */
    // OFB Dec AESKey 0
    {
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
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_OFB,     CIPHER_DIR_DECRYPTION,      KEY_GRP_AES_RAM,        2,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.9 */
    // CTR EncKEY_GRP_AES 2
    {
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cipher text
        { 0x91614d87, 0x26e320b6, 0x6468ef1b, 0xceb60d99,
          0x6bf60698, 0xfffd7079, 0x7b181786, 0xfffdffb9,
          0x3edfe45a, 0x5ed3d5db, 0x02094f5b, 0xab3eb00d,
          0xda1d031e, 0xd103be2f, 0xa0702179, 0xee9c00f3 },
        // iv
        { 0xf3f2f1f0, 0xf7f6f5f4, 0xfbfaf9f8, 0xfffefdfc },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CTR,     CIPHER_DIR_ENCRYPTION,      KEY_GRP_AES,            2,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.10 */
    // CTR Dec RAMKey 2
    {
        // cipher text
        { 0x91614d87, 0x26e320b6, 0x6468ef1b, 0xceb60d99,
          0x6bf60698, 0xfffd7079, 0x7b181786, 0xfffdffb9,
          0x3edfe45a, 0x5ed3d5db, 0x02094f5b, 0xab3eb00d,
          0xda1d031e, 0xd103be2f, 0xa0702179, 0xee9c00f3 },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // iv
        { 0xf3f2f1f0, 0xf7f6f5f4, 0xfbfaf9f8, 0xfffefdfc },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CTR,     CIPHER_DIR_DECRYPTION,      KEY_GRP_SHE,            SHE_KEY_1,      4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.11 */
    // OFB Enc Key_GRP_SHE(SHE_KEY_1)
    {
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
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_OFB,        CIPHER_DIR_ENCRYPTION,        KEY_GRP_SHE,    SHE_KEY_1,        4,                        0,                JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.12 */
    // CTR Enc KEY_GRP_AES 2
    {
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cipher text
        { 0x91614d87, 0x26e320b6, 0x6468ef1b, 0xceb60d99,
          0x6bf60698, 0xfffd7079, 0x7b181786, 0xfffdffb9,
          0x3edfe45a, 0x5ed3d5db, 0x02094f5b, 0xab3eb00d,
          0xda1d031e, 0xd103be2f, 0xa0702179, 0xee9c00f3 },
        // iv
        { 0xf3f2f1f0, 0xf7f6f5f4, 0xfbfaf9f8, 0xfffefdfc },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CTR,     CIPHER_DIR_ENCRYPTION,      KEY_GRP_AES,            2,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.13 */
    // CTR Dec KEY_GRP_AES 3
    {
        // cipher text
        { 0x91614d87, 0x26e320b6, 0x6468ef1b, 0xceb60d99,
          0x6bf60698, 0xfffd7079, 0x7b181786, 0xfffdffb9,
          0x3edfe45a, 0x5ed3d5db, 0x02094f5b, 0xab3eb00d,
          0xda1d031e, 0xd103be2f, 0xa0702179, 0xee9c00f3 },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // iv
        { 0xf3f2f1f0, 0xf7f6f5f4, 0xfbfaf9f8, 0xfffefdfc },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CTR,     CIPHER_DIR_DECRYPTION,      KEY_GRP_AES,            3,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
};

static const type_test_vector ALIGNVAR(32) test_job_cycle[] =
{
    /* No.1 */
    // Job 1 init CBC Enc
    {
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
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_CBC,        CIPHER_DIR_ENCRYPTION,        KEY_GRP_AES,        2,                4,                        2,                JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.2 */
    // Job 2 CFB Dec
    {
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
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_CFB,        CIPHER_DIR_DECRYPTION,        KEY_GRP_AES,        2,                4,                        0,                JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.3 */
    // Job 3 OFB Enc
    {
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
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_OFB,        CIPHER_DIR_ENCRYPTION,        KEY_GRP_AES,        2,                4,                        0,                JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
};
static const type_test_vector ALIGNVAR(32) test_job_cycle1[] =
{
    /* No.0 */
    // Job 1 init CBC Enc
    {
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
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_CBC,        CIPHER_DIR_ENCRYPTION,        KEY_GRP_AES,        2,                4,                        0,                JOB_INIT,
        // padding
        { 0x00 },
    },
    /* No.1 */
    // Job 2 CBC Dec
    {
        // plain text
        { 0xacab4976, 0x46b21981, 0x9b8ee9ce, 0x7d19e912,
          0x9bcb8650, 0xee197250, 0x3a11db95, 0xb2787691,
          0xb8d6be73, 0x3b74c1e3, 0x9ee61671, 0x16952222,
          0xa1caf13f, 0x09ac1f68, 0x30ca0e12, 0xa7e18675 },
        // cipher text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_CBC,        CIPHER_DIR_DECRYPTION,        KEY_GRP_AES,        2,                4,                        2,                JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.2 */
    // Job 1 update 1st CBC Enc
    {
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45 },
        // cipher text
        { 0xacab4976, 0x46b21981, 0x9b8ee9ce, 0x7d19e912,
          0x9bcb8650, 0xee197250, 0x3a11db95, 0xb2787691 },
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_CBC,        CIPHER_DIR_ENCRYPTION,        KEY_GRP_AES,        2,                2,                        1,                JOB_UPDATE,
        // padding
        { 0x00 },
    },
    /* No.3 */
    // Job 3 CFB Dec 
    {
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
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_CFB,        CIPHER_DIR_DECRYPTION,        KEY_GRP_AES,        2,                4,                        0,                JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.4 */
    // Job 1 update 2nd CBC Enc
    {
        // plain text
        { 0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cipher text
        { 0xb8d6be73, 0x3b74c1e3, 0x9ee61671, 0x16952222,
          0xa1caf13f, 0x09ac1f68, 0x30ca0e12, 0xa7e18675 },
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_CBC,        CIPHER_DIR_ENCRYPTION,        KEY_GRP_AES,        2,                2,                        2,                JOB_UPDATE,
        // padding
        { 0x00 },
    },
    /* No.5 */
    // Job 1 finish CBC Enc
    {
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
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode        // direction                // key_group        // key_id        // length_plain_block    // job_slice    // job_cycle
        CIPHER_MOD_CBC,        CIPHER_DIR_ENCRYPTION,        KEY_GRP_AES,        2,                2,                        1,                JOB_FINISH,
        // padding
        { 0x00 },
    },
};

static const type_test_vector ALIGNVAR(32) test_vector_192[] =
{
    /* No.1 */
    // CBC Enc
    {
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cipher text
        { 0xb21d024f, 0x3d63bc43, 0x3a187871, 0xe871a09f,
          0xa9add9b4, 0xf4ed7dad, 0x7638e7e5, 0x5a14693f,
          0x20241b57, 0xe07afb12, 0xacbaa97f, 0xe002f13d,
          0x79e2b008, 0x81885988, 0xe6a920d9, 0xcd15564f },
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CBC,     CIPHER_DIR_ENCRYPTION,      KEY_GRP_AES_RAM,        5,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.2 */
    // CBC Dec
    {
        // cipher text
        { 0xb21d024f, 0x3d63bc43, 0x3a187871, 0xe871a09f,
          0xa9add9b4, 0xf4ed7dad, 0x7638e7e5, 0x5a14693f,
          0x20241b57, 0xe07afb12, 0xacbaa97f, 0xe002f13d,
          0x79e2b008, 0x81885988, 0xe6a920d9, 0xcd15564f },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CBC,     CIPHER_DIR_DECRYPTION,      KEY_GRP_AES_RAM,        5,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.3 */
    // ECB Enc
    {
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cipher text
        { 0x1d4f33bd, 0x5ff2456e, 0x14a212f7, 0xcca51f57,
          0x84044197, 0xadd30a6d, 0xb3ec3477, 0xef4eeeec,
          0x22fd7aef, 0x0ae6e270, 0x2fbae0dc, 0x4e44e6ac,
          0xba414b9a, 0x726c8d73, 0x166916fb, 0x0e8ec103 },
        // iv
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_ECB,     CIPHER_DIR_ENCRYPTION,      KEY_GRP_AES,            5,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.4 */
    // ECB Dec
    {
        // cipher text
        { 0x1d4f33bd, 0x5ff2456e, 0x14a212f7, 0xcca51f57,
          0x84044197, 0xadd30a6d, 0xb3ec3477, 0xef4eeeec,
          0x22fd7aef, 0x0ae6e270, 0x2fbae0dc, 0x4e44e6ac,
          0xba414b9a, 0x726c8d73, 0x166916fb, 0x0e8ec103 },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // iv
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_ECB,     CIPHER_DIR_DECRYPTION,      KEY_GRP_AES,            5,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.5 */
    // CTR Enc
    {
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cipher text
        { 0x2493bc1a, 0xa21c5217, 0x59042b4f, 0x0b6e7efe,
          0xec390309, 0xeffaa60a, 0xc6c2ccd5, 0x948ecef4,
          0x6bb2361e, 0x70c6ebd1, 0x661dbdd1, 0xf7ab2056,
          0xf6a7784f, 0x580998d2, 0xecda975a, 0x50b0c658 },
        // iv
        { 0xf3f2f1f0, 0xf7f6f5f4, 0xfbfaf9f8, 0xfffefdfc },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CTR,     CIPHER_DIR_ENCRYPTION,      KEY_GRP_AES,            5,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.6 */
    // CTR Dec
    {
        // cipher text
        { 0x2493bc1a, 0xa21c5217, 0x59042b4f, 0x0b6e7efe,
          0xec390309, 0xeffaa60a, 0xc6c2ccd5, 0x948ecef4,
          0x6bb2361e, 0x70c6ebd1, 0x661dbdd1, 0xf7ab2056,
          0xf6a7784f, 0x580998d2, 0xecda975a, 0x50b0c658 },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // iv
        { 0xf3f2f1f0, 0xf7f6f5f4, 0xfbfaf9f8, 0xfffefdfc },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CTR,     CIPHER_DIR_DECRYPTION,      KEY_GRP_AES,            5,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    }
};

static const type_test_vector ALIGNVAR(32) test_vector_256[] =
{
    /* No.1 */
    // CBC Enc
    {
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cipher text
        { 0x044c8cf5, 0xbaf1e5d6, 0xfbab9e77, 0xd6fb7b5f,
          0x964efc9c, 0x8d80db7e, 0x7b779f67, 0x7d2c70c6,
          0x6933f239, 0xcfbad9a9, 0x63e230a5, 0x61142304,
          0xe205ebb2, 0xfce99bc3, 0x07196cda, 0x1b9d6a8c },
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CBC,     CIPHER_DIR_ENCRYPTION,      KEY_GRP_AES_RAM,        5,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.2 */
    // CBC Dec
    {
        // cipher text
        { 0x044c8cf5, 0xbaf1e5d6, 0xfbab9e77, 0xd6fb7b5f,
          0x964efc9c, 0x8d80db7e, 0x7b779f67, 0x7d2c70c6,
          0x6933f239, 0xcfbad9a9, 0x63e230a5, 0x61142304,
          0xe205ebb2, 0xfce99bc3, 0x07196cda, 0x1b9d6a8c },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // iv
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CBC,     CIPHER_DIR_DECRYPTION,      KEY_GRP_AES_RAM,        5,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.3 */
    // ECB Enc
    {
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cipher text
        { 0xbdd1eef3, 0x3ca0d2b5, 0x7e5a4b06, 0xf881b13d,
          0x10cb1c59, 0x26ed10d4, 0x4aa75bdc, 0x70283631,
          0xb921edb6, 0xf9f4a69c, 0xb1e753f1, 0x1dedafbe,
          0x7a4b3023, 0xfff3f939, 0x8f8d7d06, 0xc7ec249e },
        // iv
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_ECB,     CIPHER_DIR_ENCRYPTION,      KEY_GRP_AES,            5,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.4 */
    // ECB Dec
    {
        // cipher text
        { 0xbdd1eef3, 0x3ca0d2b5, 0x7e5a4b06, 0xf881b13d,
          0x10cb1c59, 0x26ed10d4, 0x4aa75bdc, 0x70283631,
          0xb921edb6, 0xf9f4a69c, 0xb1e753f1, 0x1dedafbe,
          0x7a4b3023, 0xfff3f939, 0x8f8d7d06, 0xc7ec249e },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // iv
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_ECB,     CIPHER_DIR_DECRYPTION,      KEY_GRP_AES,            5,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.5 */
    // CTR Enc
    {
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cipher text
        { 0x13c31e60, 0xa5895777, 0x04f5a7b7, 0x28d2f3bb,
          0xcae343f4, 0x9ab5624d, 0x90e984ca, 0xc5f5caca,
          0xda30092b, 0x4ce93da2, 0xba1770e8, 0x8d98842d,
          0x8dc5c9df, 0xa6ad7ab6, 0x08ddc213, 0xa6417945 },
        // iv
        { 0xf3f2f1f0, 0xf7f6f5f4, 0xfbfaf9f8, 0xfffefdfc },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CTR,     CIPHER_DIR_ENCRYPTION,      KEY_GRP_AES,            5,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    /* No.6 */
    // CTR Dec
    {
        // cipher text
        { 0x13c31e60, 0xa5895777, 0x04f5a7b7, 0x28d2f3bb,
          0xcae343f4, 0x9ab5624d, 0x90e984ca, 0xc5f5caca,
          0xda30092b, 0x4ce93da2, 0xba1770e8, 0x8d98842d,
          0x8dc5c9df, 0xa6ad7ab6, 0x08ddc213, 0xa6417945 },
        // plain text
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // iv
        { 0xf3f2f1f0, 0xf7f6f5f4, 0xfbfaf9f8, 0xfffefdfc },
        // cipher_mode      // direction                // key_group            // key_id       // length_plain_block   // job_slice    // job_cycle
        CIPHER_MOD_CTR,     CIPHER_DIR_DECRYPTION,      KEY_GRP_AES,            5,              4,                      0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    }
};

static const uint32_t ALIGNVAR(256)  PLAIN_DMAC[] = 
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 };

static const uint32_t ALIGNVAR(256)  CBC_128_CIPHER_DMAC[] = 
        { 0xacab4976, 0x46b21981, 0x9b8ee9ce, 0x7d19e912,
          0x9bcb8650, 0xee197250, 0x3a11db95, 0xb2787691,
          0xb8d6be73, 0x3b74c1e3, 0x9ee61671, 0x16952222,
          0xa1caf13f, 0x09ac1f68, 0x30ca0e12, 0xa7e18675 };

static const uint32_t ALIGNVAR(256)  CBC_192_CIPHER_DMAC[] = 
        { 0xb21d024f, 0x3d63bc43, 0x3a187871, 0xe871a09f,
          0xa9add9b4, 0xf4ed7dad, 0x7638e7e5, 0x5a14693f,
          0x20241b57, 0xe07afb12, 0xacbaa97f, 0xe002f13d,
          0x79e2b008, 0x81885988, 0xe6a920d9, 0xcd15564f };

static const uint32_t ALIGNVAR(256)  CBC_256_CIPHER_DMAC[] = 
        { 0x044c8cf5, 0xbaf1e5d6, 0xfbab9e77, 0xd6fb7b5f,
          0x964efc9c, 0x8d80db7e, 0x7b779f67, 0x7d2c70c6,
          0x6933f239, 0xcfbad9a9, 0x63e230a5, 0x61142304,
          0xe205ebb2, 0xfce99bc3, 0x07196cda, 0x1b9d6a8c };

/*===========================================================================
  ICUMXA test function Cipher + Job Cycle
  [PUBLIC]
  ===========================================================================*/
void icum_aes_cipher_test_JobCycle(void)
{
    int i;
    uint32_t uiStartAddr;
    uint32_t uiNextAddr;
    r_icumif_job_id_t job_id = 0;

    r_icumif_isd_t *p_ISD[3];
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);    // clear the isd_buffer

    uiStartAddr = (uint32_t)ISD_BUFFER;
    uiNextAddr = ((sizeof(r_icumif_isd_t) / 32) + 1) * 32;

    p_ISD[0]  = (r_icumif_isd_t *)uiStartAddr;
    p_ISD[1]  = (r_icumif_isd_t *)(uiStartAddr + uiNextAddr);
    p_ISD[2]  = (r_icumif_isd_t *)(uiStartAddr + (uiNextAddr * 2));

    printf("\nCipher job cycle + job slice \n"); FLUSH();
    for( i = 0; i < 3; i++ )
    {
        p_ISD[i]->service_id = SERVICE_01_AES_CIPHER;
        p_ISD[i]->p_callbackfunc = 0;
        p_ISD[i]->job_id = 0;
        p_ISD[i]->prm.AES_CIPHER.direction = test_job_cycle[i].direction;
        p_ISD[i]->prm.AES_CIPHER.cipher_mode = test_job_cycle[i].cipher_mode;
        p_ISD[i]->prm.AES_CIPHER.key_group = test_job_cycle[i].key_group;
        p_ISD[i]->prm.AES_CIPHER.key_id = test_job_cycle[i].key_id;
        p_ISD[i]->prm.AES_CIPHER.p_iv = test_job_cycle[i].iv;
        p_ISD[i]->prm.AES_CIPHER.p_block_in = test_job_cycle[i].plain_text;
        p_ISD[i]->prm.AES_CIPHER.p_block_out = (uint32_t *)BIG_BUFFER[i];
        p_ISD[i]->prm.AES_CIPHER.size.nb_blocks = test_job_cycle[i].length_plain_block;
        p_ISD[i]->prm.AES_CIPHER.job_slice = test_job_cycle[i].job_slice;
        p_ISD[i]->prm.AES_CIPHER.job_cycle = test_job_cycle[i].job_cycle;
    }
    for( i = 0 ; i < 3 ; i++ )
    {
        /* Flush of cache area of ISD service parameters and buffer */
        FlushDCache_param_and_buff(p_ISD[i]);

        R_ICUMIF_ServiceRequest(p_ISD[i]);

        do
        {
            /* Invalidate of cache area ( ISD service_result ) */
            USER_ICUMIF_InvalidateDCache((uint32_t)((volatile void *)&p_ISD[i]->service_result), 
                                                    (uint32_t)sizeof(p_ISD[i]->service_result));
        } while(p_ISD[i]->service_result == SERV_REQUEST);
    }
    // wait for completion of last service (should be the last one to complete)
    while(!R_ICUMIF_IsServiceCompleted(p_ISD[0]))
    {
        NOPS(10);
    }
    for( i = 0 ; i < 3 ; i++ )
    {
        /* Invalidate of cache area of ISD service parameters and buffer */
        InvaliDCache_param_and_buff(p_ISD[i]);

        // in case of error, stop here
        if( p_ISD[i]->service_result == SERV_OK 
            && ( 0 == memcmp( BIG_BUFFER[i],test_job_cycle[i].cipher_text,(AES_BLOCK_SIZE_IN_BYTES * test_job_cycle[i].length_plain_block))))
        {
            printf("Cipher Test Result : test %d SERV_OK\n", i + 1 ); FLUSH();

        }
        else
        {
            printf("Cipher Test Result : test %d not SERV_OK\n", i + 1); FLUSH();
        }
    }

    memset(BIG_BUFFER[0], 0, SIZE_OF_BIG_BUFFER);
    memset(BIG_BUFFER[1], 0, SIZE_OF_BIG_BUFFER);
    memset(BIG_BUFFER[2], 0, SIZE_OF_BIG_BUFFER);

/******************************************************************/
/*****************************/
/* test_job_cycle1[0](Init)      */
/*****************************/
    printf("\nCipher job cycle + job slice \n"); FLUSH();
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);    // clear the isd_buffer
    p_ISD[0]->service_id = SERVICE_01_AES_CIPHER;
    p_ISD[0]->p_callbackfunc = 0;
    p_ISD[0]->job_id = 0;
    p_ISD[0]->prm.AES_CIPHER.direction = test_job_cycle1[0].direction;
    p_ISD[0]->prm.AES_CIPHER.cipher_mode = test_job_cycle1[0].cipher_mode;
    p_ISD[0]->prm.AES_CIPHER.key_group = test_job_cycle1[0].key_group;
    p_ISD[0]->prm.AES_CIPHER.key_id = test_job_cycle1[0].key_id;
    p_ISD[0]->prm.AES_CIPHER.p_iv = test_job_cycle1[0].iv;
    p_ISD[0]->prm.AES_CIPHER.p_block_in = test_job_cycle1[0].plain_text;
    p_ISD[0]->prm.AES_CIPHER.p_block_out = (uint32_t *)BIG_BUFFER[0];
    p_ISD[0]->prm.AES_CIPHER.size.nb_blocks = test_job_cycle1[0].length_plain_block;
    p_ISD[0]->prm.AES_CIPHER.job_slice = test_job_cycle1[0].job_slice;
    p_ISD[0]->prm.AES_CIPHER.job_cycle = test_job_cycle1[0].job_cycle;

    trigger_service_and_wait(p_ISD[0]);

    job_id = p_ISD[0]->job_id;

/*****************************/
/* test_job_cycle1[1](AllAtOnce) */
/*****************************/
    p_ISD[1]->service_id = SERVICE_01_AES_CIPHER;
    p_ISD[1]->p_callbackfunc = 0;
    p_ISD[1]->job_id = 0;
    p_ISD[1]->prm.AES_CIPHER.direction = test_job_cycle1[1].direction;
    p_ISD[1]->prm.AES_CIPHER.cipher_mode = test_job_cycle1[1].cipher_mode;
    p_ISD[1]->prm.AES_CIPHER.key_group = test_job_cycle1[1].key_group;
    p_ISD[1]->prm.AES_CIPHER.key_id = test_job_cycle1[1].key_id;
    p_ISD[1]->prm.AES_CIPHER.p_iv = test_job_cycle1[1].iv;
    p_ISD[1]->prm.AES_CIPHER.p_block_in = test_job_cycle1[1].plain_text;
    p_ISD[1]->prm.AES_CIPHER.p_block_out = (uint32_t *)BIG_BUFFER[1];
    p_ISD[1]->prm.AES_CIPHER.size.nb_blocks = test_job_cycle1[1].length_plain_block;
    p_ISD[1]->prm.AES_CIPHER.job_slice = test_job_cycle1[1].job_slice;
    p_ISD[1]->prm.AES_CIPHER.job_cycle = test_job_cycle1[1].job_cycle;

    /* Flush of cache area of ISD service parameters and buffer */
    FlushDCache_param_and_buff(p_ISD[1]);

    R_ICUMIF_ServiceRequest(p_ISD[1]);

    do
    {
        /* Invalidate of cache area ( ISD service_result ) */
        USER_ICUMIF_InvalidateDCache((uint32_t)((volatile void *)&p_ISD[1]->service_result), 
                                                (uint32_t)sizeof(p_ISD[1]->service_result));
    } while(p_ISD[1]->service_result == SERV_REQUEST);

/*****************************/
/* test_job_cycle1[2](Update)    */
/*****************************/
    p_ISD[0]->service_id = SERVICE_01_AES_CIPHER;
    p_ISD[0]->p_callbackfunc = 0;
    p_ISD[0]->job_id = job_id;
    p_ISD[0]->prm.AES_CIPHER.direction = test_job_cycle1[2].direction;
    p_ISD[0]->prm.AES_CIPHER.cipher_mode = test_job_cycle1[2].cipher_mode;
    p_ISD[0]->prm.AES_CIPHER.key_group = test_job_cycle1[2].key_group;
    p_ISD[0]->prm.AES_CIPHER.key_id = test_job_cycle1[2].key_id;
    p_ISD[0]->prm.AES_CIPHER.p_iv = test_job_cycle1[2].iv;
    p_ISD[0]->prm.AES_CIPHER.p_block_in = test_job_cycle1[2].plain_text;
    p_ISD[0]->prm.AES_CIPHER.p_block_out = (uint32_t *)BIG_BUFFER[0];
    p_ISD[0]->prm.AES_CIPHER.size.nb_blocks = test_job_cycle1[2].length_plain_block;
    p_ISD[0]->prm.AES_CIPHER.job_slice = test_job_cycle1[2].job_slice;
    p_ISD[0]->prm.AES_CIPHER.job_cycle = test_job_cycle1[2].job_cycle;

    trigger_service_and_wait(p_ISD[0]);

    job_id = p_ISD[0]->job_id;

/*****************************/
/* test_job_cycle1[3](AllAtOnce) */
/*****************************/
    p_ISD[2]->service_id = SERVICE_01_AES_CIPHER;
    p_ISD[2]->p_callbackfunc = 0;
    p_ISD[2]->job_id = 0;
    p_ISD[2]->prm.AES_CIPHER.direction = test_job_cycle1[3].direction;
    p_ISD[2]->prm.AES_CIPHER.cipher_mode = test_job_cycle1[3].cipher_mode;
    p_ISD[2]->prm.AES_CIPHER.key_group = test_job_cycle1[3].key_group;
    p_ISD[2]->prm.AES_CIPHER.key_id = test_job_cycle1[3].key_id;
    p_ISD[2]->prm.AES_CIPHER.p_iv = test_job_cycle1[3].iv;
    p_ISD[2]->prm.AES_CIPHER.p_block_in = test_job_cycle1[3].plain_text;
    p_ISD[2]->prm.AES_CIPHER.p_block_out = (uint32_t *)BIG_BUFFER[2];
    p_ISD[2]->prm.AES_CIPHER.size.nb_blocks = test_job_cycle1[3].length_plain_block;
    p_ISD[2]->prm.AES_CIPHER.job_slice = test_job_cycle1[3].job_slice;
    p_ISD[2]->prm.AES_CIPHER.job_cycle = test_job_cycle1[3].job_cycle;

    /* Flush of cache area of ISD service parameters and buffer */
    FlushDCache_param_and_buff(p_ISD[2]);

    R_ICUMIF_ServiceRequest(p_ISD[2]);

    do
    {
        /* Invalidate of cache area ( ISD service_result ) */
        USER_ICUMIF_InvalidateDCache((uint32_t)((volatile void *)&p_ISD[2]->service_result), 
                                                (uint32_t)sizeof(p_ISD[2]->service_result));
    } while(p_ISD[2]->service_result == SERV_REQUEST);

    while(!R_ICUMIF_IsServiceCompleted(p_ISD[0]))
    {
        NOPS(10);
    }

/*****************************/
/* test_job_cycle1[4](Update)    */
/*****************************/
    p_ISD[0]->service_id = SERVICE_01_AES_CIPHER;
    p_ISD[0]->p_callbackfunc = 0;
    p_ISD[0]->job_id = job_id;
    p_ISD[0]->prm.AES_CIPHER.direction = test_job_cycle1[4].direction;
    p_ISD[0]->prm.AES_CIPHER.cipher_mode = test_job_cycle1[4].cipher_mode;
    p_ISD[0]->prm.AES_CIPHER.key_group = test_job_cycle1[4].key_group;
    p_ISD[0]->prm.AES_CIPHER.key_id = test_job_cycle1[4].key_id;
    p_ISD[0]->prm.AES_CIPHER.p_iv = test_job_cycle1[4].iv;
    p_ISD[0]->prm.AES_CIPHER.p_block_in = test_job_cycle1[4].plain_text;
    p_ISD[0]->prm.AES_CIPHER.p_block_out = (uint32_t *)( BIG_BUFFER[0] + 16*2);
    p_ISD[0]->prm.AES_CIPHER.size.nb_blocks = test_job_cycle1[4].length_plain_block;
    p_ISD[0]->prm.AES_CIPHER.job_slice = test_job_cycle1[4].job_slice;
    p_ISD[0]->prm.AES_CIPHER.job_cycle = test_job_cycle1[4].job_cycle;

    trigger_service_and_wait(p_ISD[0]);

    job_id = p_ISD[0]->job_id;

/*****************************/
/* test_job_cycle1[5](Finish)   */
/*****************************/
    p_ISD[0]->service_id = SERVICE_01_AES_CIPHER;
    p_ISD[0]->p_callbackfunc = 0;
    p_ISD[0]->job_id = job_id;
    p_ISD[0]->prm.AES_CIPHER.direction = test_job_cycle1[5].direction;
    p_ISD[0]->prm.AES_CIPHER.cipher_mode = test_job_cycle1[5].cipher_mode;
    p_ISD[0]->prm.AES_CIPHER.key_group = test_job_cycle1[5].key_group;
    p_ISD[0]->prm.AES_CIPHER.key_id = test_job_cycle1[5].key_id;
    p_ISD[0]->prm.AES_CIPHER.p_iv = test_job_cycle1[5].iv;
    p_ISD[0]->prm.AES_CIPHER.p_block_in = test_job_cycle1[5].plain_text;
    p_ISD[0]->prm.AES_CIPHER.p_block_out = (uint32_t *)BIG_BUFFER[0];
    p_ISD[0]->prm.AES_CIPHER.size.nb_blocks = test_job_cycle1[5].length_plain_block;
    p_ISD[0]->prm.AES_CIPHER.job_slice = test_job_cycle1[5].job_slice;
    p_ISD[0]->prm.AES_CIPHER.job_cycle = test_job_cycle1[5].job_cycle;

    trigger_service_and_wait(p_ISD[0]);

    // in case of error, stop here
    if( p_ISD[0]->service_result == SERV_OK 
        && ( 0 == memcmp( BIG_BUFFER[0],test_job_cycle1[0].cipher_text,(AES_BLOCK_SIZE_IN_BYTES * 4))))
    {
        printf("Cipher Test Result : test %d SERV_OK\n", 1 ); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test %d not SERV_OK\n", 1); FLUSH();
    }

    /* Invalidate of cache area of ISD common parameters */
    USER_ICUMIF_InvalidateDCache((uint32_t)((volatile void *)p_ISD[1]), 
                                 (uint32_t)(sizeof(r_icumif_isd_t) - sizeof(p_ISD[1]->prm)));
    /* Invalidate of cache area of ISD service parameters and buffer */
    InvaliDCache_param_and_buff(p_ISD[1]);

    if( p_ISD[1]->service_result == SERV_OK 
        && ( 0 == memcmp( BIG_BUFFER[1],test_job_cycle1[1].cipher_text,(AES_BLOCK_SIZE_IN_BYTES * 4))))
    {
        printf("Cipher Test Result : test %d SERV_OK\n", 2 ); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test %d not SERV_OK\n", 2); FLUSH();
    }

    /* Invalidate of cache area of ISD common parameters */
    USER_ICUMIF_InvalidateDCache((uint32_t)((volatile void *)p_ISD[2]), 
                                 (uint32_t)(sizeof(r_icumif_isd_t) - sizeof(p_ISD[2]->prm)));
    /* Invalidate of cache area of ISD service parameters and buffer */
    InvaliDCache_param_and_buff(p_ISD[2]);

    if( p_ISD[2]->service_result == SERV_OK 
        && ( 0 == memcmp( BIG_BUFFER[2],test_job_cycle1[3].cipher_text,(AES_BLOCK_SIZE_IN_BYTES * 4))))
    {
        printf("Cipher Test Result : test %d SERV_OK\n", 3 ); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test %d not SERV_OK\n", 3); FLUSH();
    }
/******************************************************************/
}

/*===========================================================================
  ICUMXA test function
  [PUBLIC]
  ===========================================================================*/
void icum_aes_cipher_test_192(void)
{
    r_icumif_isd_t *p_ISD;

    int n = 6;          /* Number of registrations */
    int i;

    /* initialize the global ICUMXA service header */
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nCipher Test Start...\n\n"); FLUSH();
    for(i = 0; i < n; i++)
    {
        p_ISD->service_id     = SERVICE_01_AES_CIPHER;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        p_ISD->prm.AES_CIPHER.direction      = test_vector_192[i].direction;
        p_ISD->prm.AES_CIPHER.cipher_mode    = test_vector_192[i].cipher_mode;
        p_ISD->prm.AES_CIPHER.key_group      = test_vector_192[i].key_group;
        p_ISD->prm.AES_CIPHER.key_id         = test_vector_192[i].key_id;
        p_ISD->prm.AES_CIPHER.p_iv           = test_vector_192[i].iv;
        p_ISD->prm.AES_CIPHER.p_block_in     = test_vector_192[i].plain_text;
        p_ISD->prm.AES_CIPHER.p_block_out    = (uint32_t *)BIG_BUFFER[1];
        p_ISD->prm.AES_CIPHER.size.nb_blocks = test_vector_192[i].length_plain_block;
        p_ISD->prm.AES_CIPHER.job_slice      = test_vector_192[i].job_slice;
        p_ISD->prm.AES_CIPHER.job_cycle      = test_vector_192[i].job_cycle;

        printf("Test No. %d \n",(i+1));

        /* trigger the security service */
        trigger_service_and_wait(p_ISD);

        /* in case of error, stop here */
        if((p_ISD->service_result == SERV_OK) &&
           (0 == memcmp(BIG_BUFFER[1], test_vector_192[i].cipher_text, (AES_BLOCK_SIZE_IN_BYTES * test_vector_192[i].length_plain_block))))
        {
            printf("Cipher Test Result : test %d SERV_OK\n", i + 1 ); FLUSH();
        }
        else
        {
            printf("Cipher Test Result : test %d not SERV_OK\n", i + 1); FLUSH();
        }
    }
}

/*===========================================================================
  ICUMXA test function
  [PUBLIC]
  ===========================================================================*/
void icum_aes_cipher_test_256(void)
{
    r_icumif_isd_t *p_ISD;

    int n = 6;          /* Number of registrations */
    int i;

    /* initialize the global ICUMXA service header */
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nCipher Test Start...\n\n"); FLUSH();
    for(i = 0; i < n; i++)
    {
        p_ISD->service_id     = SERVICE_01_AES_CIPHER;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;
        p_ISD->prm.AES_CIPHER.direction      = test_vector_256[i].direction;
        p_ISD->prm.AES_CIPHER.cipher_mode    = test_vector_256[i].cipher_mode;
        p_ISD->prm.AES_CIPHER.key_group      = test_vector_256[i].key_group;
        p_ISD->prm.AES_CIPHER.key_id         = test_vector_256[i].key_id;
        p_ISD->prm.AES_CIPHER.p_iv           = test_vector_256[i].iv;
        p_ISD->prm.AES_CIPHER.p_block_in     = test_vector_256[i].plain_text;
        p_ISD->prm.AES_CIPHER.p_block_out    = (uint32_t *)BIG_BUFFER[1];
        p_ISD->prm.AES_CIPHER.size.nb_blocks = test_vector_256[i].length_plain_block;
        p_ISD->prm.AES_CIPHER.job_slice      = test_vector_256[i].job_slice;
        p_ISD->prm.AES_CIPHER.job_cycle      = test_vector_256[i].job_cycle;

        printf("Test No. %d \n",(i+1));

        /* trigger the security service */
        trigger_service_and_wait(p_ISD);

        /* in case of error, stop here */
        if((p_ISD->service_result == SERV_OK) &&
           (0 == memcmp(BIG_BUFFER[1], test_vector_256[i].cipher_text, (AES_BLOCK_SIZE_IN_BYTES * test_vector_256[i].length_plain_block))))
        {
            printf("Cipher Test Result : test %d SERV_OK\n", i + 1 ); FLUSH();
        }
        else
        {
            printf("Cipher Test Result : test %d not SERV_OK\n", i + 1); FLUSH();
        }
    }
}


/*===========================================================================
  Reference DMAC
  ===========================================================================*/
void Check_DMAC_AES(void)
{
    r_icumif_isd_t *p_ISD;

    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    /* 1. CBC_128 Enc */
    p_ISD->service_id = SERVICE_01_AES_CIPHER;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id = 0;
    p_ISD->prm.AES_CIPHER.direction = test_vector[0].direction;
    p_ISD->prm.AES_CIPHER.cipher_mode = test_vector[0].cipher_mode;
    p_ISD->prm.AES_CIPHER.key_group = KEY_GRP_AES;
    p_ISD->prm.AES_CIPHER.key_id = 2;
    p_ISD->prm.AES_CIPHER.p_iv = test_vector[0].iv;
    p_ISD->prm.AES_CIPHER.p_block_in = (uint32_t*)PLAIN_DMAC;
    p_ISD->prm.AES_CIPHER.p_block_out = (uint32_t *)DMAC_BUFFE[0];
    p_ISD->prm.AES_CIPHER.size.nb_blocks = test_vector[0].length_plain_block;
    p_ISD->prm.AES_CIPHER.job_slice = test_vector[0].job_slice;
    p_ISD->prm.AES_CIPHER.job_cycle = test_vector[0].job_cycle;

    printf("1. CBC(Encode) 128bit Key test\n");

    // trigger the security service
    trigger_service_and_wait(p_ISD);

    // in case of error, stop here
    if( p_ISD->service_result == SERV_OK
        && ( 0 == memcmp( DMAC_BUFFE[0], test_vector[0].cipher_text,(AES_BLOCK_SIZE_IN_BYTES * test_vector[0].length_plain_block))))
    {
        printf("Cipher Test Result : test 1 SERV_OK\n"); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test 1 not SERV_OK\n"); FLUSH();
    }

    /* 2. CBC_128 Dec */
    p_ISD->service_id = SERVICE_01_AES_CIPHER;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id = 0;
    p_ISD->prm.AES_CIPHER.direction = test_vector[1].direction;
    p_ISD->prm.AES_CIPHER.cipher_mode = test_vector[1].cipher_mode;
    p_ISD->prm.AES_CIPHER.key_group = KEY_GRP_AES;
    p_ISD->prm.AES_CIPHER.key_id = 2;
    p_ISD->prm.AES_CIPHER.p_iv = test_vector[1].iv;
    p_ISD->prm.AES_CIPHER.p_block_in = (uint32_t*)CBC_128_CIPHER_DMAC;
    p_ISD->prm.AES_CIPHER.p_block_out = (uint32_t *)DMAC_BUFFE[0];
    p_ISD->prm.AES_CIPHER.size.nb_blocks = test_vector[1].length_plain_block;
    p_ISD->prm.AES_CIPHER.job_slice = test_vector[1].job_slice;
    p_ISD->prm.AES_CIPHER.job_cycle = test_vector[1].job_cycle;

    printf("2. CBC(Decode) 128bit Key test\n");

    // trigger the security service
    trigger_service_and_wait(p_ISD);

    // in case of error, stop here
    if( p_ISD->service_result == SERV_OK
        && ( 0 == memcmp( DMAC_BUFFE[0], test_vector[1].cipher_text,(AES_BLOCK_SIZE_IN_BYTES * test_vector[1].length_plain_block))))
    {
        printf("Cipher Test Result : test 2 SERV_OK\n"); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test 2 not SERV_OK\n"); FLUSH();
    }

    /* 3. CBC_192 Enc */
    p_ISD->service_id = SERVICE_01_AES_CIPHER;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id = 0;
    p_ISD->prm.AES_CIPHER.direction = test_vector_192[0].direction;
    p_ISD->prm.AES_CIPHER.cipher_mode = test_vector_192[0].cipher_mode;
    p_ISD->prm.AES_CIPHER.key_group = KEY_GRP_AES;
    p_ISD->prm.AES_CIPHER.key_id = 5;
    p_ISD->prm.AES_CIPHER.p_iv = test_vector_192[0].iv;
    p_ISD->prm.AES_CIPHER.p_block_in = (uint32_t*)PLAIN_DMAC;
    p_ISD->prm.AES_CIPHER.p_block_out = (uint32_t *)DMAC_BUFFE[0];
    p_ISD->prm.AES_CIPHER.size.nb_blocks = test_vector_192[0].length_plain_block;
    p_ISD->prm.AES_CIPHER.job_slice = test_vector_192[0].job_slice;
    p_ISD->prm.AES_CIPHER.job_cycle = test_vector_192[0].job_cycle;

    printf("3. CBC(Encode) 192bit Key test\n");

    // trigger the security service
    trigger_service_and_wait(p_ISD);

    // in case of error, stop here
    if( p_ISD->service_result == SERV_OK
        && ( 0 == memcmp( DMAC_BUFFE[0], test_vector_192[0].cipher_text,(AES_BLOCK_SIZE_IN_BYTES * test_vector_192[0].length_plain_block))))
    {
        printf("Cipher Test Result : test 3 SERV_OK\n"); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test 3 not SERV_OK\n"); FLUSH();
    }

    /* 4. CBC_192 Dec */
    p_ISD->service_id = SERVICE_01_AES_CIPHER;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id = 0;
    p_ISD->prm.AES_CIPHER.direction = test_vector_192[1].direction;
    p_ISD->prm.AES_CIPHER.cipher_mode = test_vector_192[1].cipher_mode;
    p_ISD->prm.AES_CIPHER.key_group = KEY_GRP_AES;
    p_ISD->prm.AES_CIPHER.key_id = 5;
    p_ISD->prm.AES_CIPHER.p_iv = test_vector_192[1].iv;
    p_ISD->prm.AES_CIPHER.p_block_in = (uint32_t*)CBC_192_CIPHER_DMAC;
    p_ISD->prm.AES_CIPHER.p_block_out = (uint32_t *)DMAC_BUFFE[0];
    p_ISD->prm.AES_CIPHER.size.nb_blocks = test_vector_192[1].length_plain_block;
    p_ISD->prm.AES_CIPHER.job_slice = test_vector_192[1].job_slice;
    p_ISD->prm.AES_CIPHER.job_cycle = test_vector_192[1].job_cycle;

    printf("4. CBC(Decode) 192bit Key test\n");

    // trigger the security service
    trigger_service_and_wait(p_ISD);

    // in case of error, stop here
    if( p_ISD->service_result == SERV_OK
        && ( 0 == memcmp( DMAC_BUFFE[0], test_vector_192[1].cipher_text,(AES_BLOCK_SIZE_IN_BYTES * test_vector_192[1].length_plain_block))))
    {
        printf("Cipher Test Result : test 4 SERV_OK\n"); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test 4 not SERV_OK\n"); FLUSH();
    }

    /* 5. CBC_256 Enc */
    p_ISD->service_id = SERVICE_01_AES_CIPHER;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id = 0;
    p_ISD->prm.AES_CIPHER.direction = test_vector_256[0].direction;
    p_ISD->prm.AES_CIPHER.cipher_mode = test_vector_256[0].cipher_mode;
    p_ISD->prm.AES_CIPHER.key_group = KEY_GRP_AES;
    p_ISD->prm.AES_CIPHER.key_id = 5;
    p_ISD->prm.AES_CIPHER.p_iv = test_vector_256[0].iv;
    p_ISD->prm.AES_CIPHER.p_block_in = (uint32_t*)PLAIN_DMAC;
    p_ISD->prm.AES_CIPHER.p_block_out = (uint32_t *)DMAC_BUFFE[0];
    p_ISD->prm.AES_CIPHER.size.nb_blocks = test_vector_256[0].length_plain_block;
    p_ISD->prm.AES_CIPHER.job_slice = test_vector_256[0].job_slice;
    p_ISD->prm.AES_CIPHER.job_cycle = test_vector_256[0].job_cycle;

    printf("5. CBC(Encode) 256bit Key test\n");

    // trigger the security service
    trigger_service_and_wait(p_ISD);

    // in case of error, stop here
    if( p_ISD->service_result == SERV_OK
        && ( 0 == memcmp( DMAC_BUFFE[0], test_vector_256[0].cipher_text,(AES_BLOCK_SIZE_IN_BYTES * test_vector_256[0].length_plain_block))))
    {
        printf("Cipher Test Result : test 5 SERV_OK\n"); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test 5 not SERV_OK\n"); FLUSH();
    }

    /* 6. CBC_256 Dec */
    p_ISD->service_id = SERVICE_01_AES_CIPHER;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id = 0;
    p_ISD->prm.AES_CIPHER.direction = test_vector_256[1].direction;
    p_ISD->prm.AES_CIPHER.cipher_mode = test_vector_256[1].cipher_mode;
    p_ISD->prm.AES_CIPHER.key_group = KEY_GRP_AES;
    p_ISD->prm.AES_CIPHER.key_id = 5;
    p_ISD->prm.AES_CIPHER.p_iv = test_vector_256[1].iv;
    p_ISD->prm.AES_CIPHER.p_block_in  = (uint32_t*)CBC_256_CIPHER_DMAC;
    p_ISD->prm.AES_CIPHER.p_block_out = (uint32_t *)DMAC_BUFFE[0];
    p_ISD->prm.AES_CIPHER.size.nb_blocks = test_vector_256[1].length_plain_block;
    p_ISD->prm.AES_CIPHER.job_slice = test_vector_256[1].job_slice;
    p_ISD->prm.AES_CIPHER.job_cycle = test_vector_256[1].job_cycle;

    printf("6. CBC(Decode) 256bit Key test\n");

    // trigger the security service
    trigger_service_and_wait(p_ISD);

    // in case of error, stop here
    if( p_ISD->service_result == SERV_OK
        && ( 0 == memcmp( DMAC_BUFFE[0], test_vector_256[1].cipher_text,(AES_BLOCK_SIZE_IN_BYTES * test_vector_256[1].length_plain_block))))
    {
        printf("Cipher Test Result : test 6 SERV_OK\n"); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test 6 not SERV_OK\n"); FLUSH();
    }

}
