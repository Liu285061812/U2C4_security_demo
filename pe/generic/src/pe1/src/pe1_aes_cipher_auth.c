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
#include "renesas_types.h"

#include "r_icumif_api.h"

#include "pe1.h"
#include "shared.h"
#include "vrun.api.h"

// ********************************************************************************
// Local constants
// ********************************************************************************
#define ALIGNVAR(n) __attribute__ ((aligned(n)))

#define DEF_VECTOR_NUM      28
#define ENCRIPT             1
#define DECRIPT             2
#define ALL_AT_ONCE         3

#define TAG_SIZE_32             (32uL)
#define TAG_SIZE_48             (48uL)
#define TAG_SIZE_64             (64uL)
#define TAG_SIZE_80             (80uL)
#define TAG_SIZE_96             (96uL)
#define TAG_SIZE_112            (112uL)
#define TAG_SIZE_128            (128uL)

#define CCM_NONCESIZE_56        (56uL)
#define CCM_NONCESIZE_64        (64uL)
#define CCM_NONCESIZE_72        (72uL)
#define CCM_NONCESIZE_80        (80uL)
#define CCM_NONCESIZE_88        (88uL)
#define CCM_NONCESIZE_96        (96uL)
#define CCM_NONCESIZE_104       (104uL)

static const uint8_t ALIGNVAR(32) TEST_KEY_2[16] =
    { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

static const uint32_t ALIGNVAR(32) TEST_IV[4] =
    { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c };

static const uint32_t ALIGNVAR(32) TEST_PLAIN_TEXT[16] =
    { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
      0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
      0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
      0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 };

static const uint32_t ALIGNVAR(32) TEST_CIPHER_TEXT[16] =
    { 0x0d82bed2, 0x8a5f7a7b, 0x4f49c305, 0x83c8b00e, 
      0xf45f5f85, 0x172286ac, 0xea523941, 0x0bf33812, 
      0x898d62f8, 0x16f1a6cc, 0xf7bb35c2, 0x205b7bbc, 
      0xd3383169, 0x2f1baacf, 0xf3de1d82, 0x4c31cd76 };

static const uint32_t ALIGNVAR(32) TEST_OUTPUT_TAG[4] = 
    { 0xBD1971FA, 0x4AE1DF0E, 0x70BF401D, 0xD59EFC75 };

/********************************************************************************************/

typedef struct VECTOR_AES_AUTH_CIPHER
{
    uint32_t                    Key[8];
    uint32_t                    iv[4];
    uint32_t                    iv_size_in_bits;
    uint32_t                    auth_data[8];
    uint32_t                    auth_data_size_in_bits;
    uint32_t                    data_in[8];
    uint32_t                    data_out[8];
    uint32_t                    data_size_in_bits;
    uint32_t                    auth_tag[4];
    uint32_t                    auth_tag_size_in_bits;
    r_key_group_t               key_group;
    r_key_index_t               key_id;
    uint32_t                    key_size;
    cipher_direction_t          direction;
    auth_cipher_modes_t         cipher_mode;
    r_job_slice_t               job_slice;
    r_job_cycle_t               job_cycle;
//    volatile r_verif_result_t   verification_result;
} VECTOR_AES_AUTH_CIPHER_t;

static const VECTOR_AES_AUTH_CIPHER_t ALIGNVAR(8) test_vector_auth[] =
{
/***************************/
/* GCM                     */
/***************************/
/* KeySize 128 */
    /* No.1 ([Keylen = 128] [IVlen = 96] [PTlen = 0] [AADlen = 0] [Taglen = 128]) <gcmDecrypt128.rsp Line:13> */
    {
        // Key
        { 0x343a06cf, 0x6ca7a9d4, 0x7d78862c, 0x71db963f, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // iv
        { 0x85973b11, 0xc8641897, 0x87c7013b, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        0,
        // data_in(Cipher Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Plain Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        0,
        // auth_tag
        { 0x9384ac72, 0x8b22a5e3, 0x690a135d, 0x420e51d2 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.2 ([Keylen = 128] [IVlen = 96] [PTlen = 0] [AADlen = 128] [Taglen = 128]) gcmDecrypt128.rsp Line:904 */
    {
        // Key
        { 0x20e37023, 0x084234d4, 0x8356ffe0, 0x13b243f2, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // iv
        { 0x2fb8db04, 0x83304d04, 0x2812441c, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0x508e3ad4, 0xd0a0ee89, 0x853ac026, 0xda278b17, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        128,
        // data_in(Cipher Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Plain Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        0,
        // auth_tag
        { 0x049c042a, 0x95aa259d, 0x1d459b96, 0x6e1cc393 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.3 [Keylen = 128] [IVlen = 96] [PTlen = 128] [AADlen = 0] [Taglen = 128] gcmDecrypt128.rsp Line: 4423 */
    {
        // Key
        { 0xa9728be9, 0xca841a88, 0xf4e0766b, 0x7a64683e, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // iv
        { 0x9f29238b, 0x534017de, 0xba52d6f3, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        0,
        // data_in(Cipher Text)
        { 0xf11c3c5a, 0x8bbb5d98, 0x368081ed, 0x42abd5fd, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Plain Text)
        { 0x326a2828, 0x3c259312, 0x70a20a3e, 0x3280274a, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        128,
        // auth_tag
        { 0x0fabc723, 0x91702b95, 0x354832cd, 0xb55e3b04 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.4 [Keylen = 128] [IVlen = 96] [PTlen = 128] [AADlen = 128] [Taglen = 128] gcmDecrypt128.rsp Line: 5305 */
    {
        // Key
        { 0x07396e81, 0x21cf1004, 0xa04d9084, 0x5a07a53e, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // iv
        { 0xa367c332, 0xb2132636, 0x7ee6c37f, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0x2807a3f2, 0xe04e87ed, 0x94c28329, 0x163c5d43, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        128,
        // data_in(Cipher Text)
        { 0x01be2e55, 0x90cf7b2e, 0x2f71effc, 0xf1e84483, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Plain Text)
        { 0x6ce9afec, 0x6764a167, 0x91c8f144, 0x2794e6f5, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        128,
        // auth_tag
        { 0xfce9aaec, 0x456a2768, 0xcba30cab, 0x9f53d99d },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.5 [Keylen = 128] [IVlen = 96] [PTlen = 256] [AADlen = 128] [Taglen = 128] gcmDecrypt128.rsp Line : 14141 */
    {
        // Key
        { 0x70debc95, 0x4ef094c0, 0x9c25d83d, 0xe88cd8af, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // iv
        { 0x7a09cf12, 0x438023d2, 0x5c0af42f, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0xcca083c7, 0x9f8d0aa1, 0x697dd2b8, 0xf2639465, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        128,
        // data_in(Cipher Text)
        { 0xa43b028a, 0x09b8f577, 0x8fdadcbd, 0x6490e055, 0xae8ad8d6, 0x141e9cc9, 0x5bea1212, 0x60365008 },
        // data_out(Plain Text)
        { 0x831ef532, 0x8348977a, 0x6d062589, 0x8071e869, 0x37644af3, 0xe596b3e6, 0xcb343b64, 0xb1f7e42e },
        // data_size_in_bits
        256,
        // auth_tag
        { 0x0d502f56, 0x605d63ae, 0x66b469a7, 0x1ecd5ae1 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

/* KeySize 192 */
    /* No.6 ([Keylen = 192] [IVlen = 96] [PTlen = 0] [AADlen = 0] [Taglen = 128]) <gcmDecrypt192.rsp Line:13> */
    {
        // Key
        { 0x686e5d0e, 0xbe322ff8, 0x94b6f0a3, 0x1ea3c198, 0xcd55d9f6, 0xa8a2273d, 0x00000000, 0x00000000 },
        // iv
        { 0xe12ef7ca, 0x01102ee6, 0x63bccfe8, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        0,
        // data_in(Cipher Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Plain Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        0,
        // auth_tag
        { 0xff741adb, 0x26def7b5, 0x092e74f5, 0xcbb9b142 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM ,   0,          24,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.7 ([Keylen = 192] [IVlen = 96] [PTlen = 0] [AADlen = 128] [Taglen = 128]) gcmDecrypt192.rsp Line:895 */
    {
        // Key
        { 0xb51c92aa, 0x08ab43a2, 0x89321f91, 0xda396b26, 0xc4f533b1, 0xcdc5a620, 0x00000000, 0x00000000 },
        // iv
        { 0x68db738f, 0x2dedeeda, 0xa0b15f15, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0xc7437448, 0x74e44cc1, 0x1f293dcb, 0xa2707025, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        128,
        // data_in(Cipher Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Plain Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        0,
        // auth_tag
        { 0xfb7426b1, 0x9a88c6ea, 0x278f9424, 0x500ae392 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          24,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.8 [Keylen = 192] [IVlen = 96] [PTlen = 128] [AADlen = 0] [Taglen = 128] gcmDecrypt192.rsp Line: 4423 */
    {
        // Key
        { 0x6a5b7c7a, 0xacb59a8a, 0xf6a934ae, 0xa9191fe4, 0x30c3f971, 0x0c0f3c02, 0x00000000, 0x00000000 },
        // iv
        { 0xbf384caa, 0xf9947f58, 0xd577ee9f, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        0,
        // data_in(Cipher Text)
        { 0x5be92a13, 0x4ac459d3, 0x4863faae, 0xbdaf2c63, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Plain Text)
        { 0x476fae99, 0x3504309b, 0xd88cf14f, 0xfb6e0b6c, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        128,
        // auth_tag
        { 0xd5c7d719, 0x64d69a80, 0x2ff21081, 0x727d2e27 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          24,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.9 [Keylen = 192] [IVlen = 96] [PTlen = 128] [AADlen = 128] [Taglen = 128] gcmDecrypt192.rsp Line: 5329 */
    {
        // Key
        { 0xc9d6440c, 0x2c11ee28, 0x54fe65e6, 0x7238bd7e, 0xb454a998, 0xd895f662, 0x00000000, 0x00000000 },
        // iv
        { 0x20f3b818, 0x8caef4fe, 0x52f9e8cb, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0x3fd44173, 0x8238cf98, 0x41091821, 0xe8760397, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        128,
        // data_in(Cipher Text)
        { 0x810764a6, 0xb95e4033, 0xf7364d09, 0x1f1970e0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Plain Text)
        { 0xf907ad96, 0x52b628b6, 0x73cb86cf, 0x516f8817, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        128,
        // auth_tag
        { 0x17c3f9e8, 0xf3e37c84, 0xa49439c2, 0x8165f002 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          24,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.10 [Keylen = 192] [IVlen = 96] [PTlen = 256] [AADlen = 128] [Taglen = 128] gcmDecrypt192.rsp Line : 14133 */
    {
        // Key
        { 0xb6cbf4b8, 0xbd99b60a, 0xcd91efa8, 0x704bbb49, 0x2e97c24a, 0x5be88a67, 0x00000000, 0x00000000 },
        // iv
        { 0x897e44bb, 0x9e382232, 0x0b896e9f, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0xf97f3c5a, 0x8c75a0f5, 0x53e5a960, 0xb758de5f, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        128,
        // data_in(Cipher Text)
        { 0x334972d3, 0xe8d4fe7c, 0x4a6c30a9, 0xdd0b5d20, 0x2a130000, 0x6d83f68a, 0x40be9dea, 0xd3448e8b },
        // data_out(Plain Text)
        { 0x608d08d6, 0xbfb3ea91, 0x1269ddb7, 0x62fcd1eb, 0x7625aa3c, 0xd7c2b601, 0x900bdf31, 0x45b5bdde },
        // data_size_in_bits
        256,
        // auth_tag
        { 0xd1bdc9d5, 0x7c107db3, 0x2ada22c1, 0x6f138481 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          24,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

/* KeySize 256 */
    /* No.11 ([Keylen = 256] [IVlen = 96] [PTlen = 0] [AADlen = 0] [Taglen = 128]) <gcmDecrypt256.rsp Line:13> */
    {
        // Key
        { 0x7cb2a2f5, 0x72583574, 0xc5f63eeb, 0x74aaaffe, 0x90e96a0e, 0x3b8cd4d9, 0x3582bbd9, 0x10f089e5 },
        // iv
        { 0x0f24d258, 0xc1310a58, 0xe94849d2, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        0,
        // data_in(Cipher Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Plain Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        0,
        // auth_tag
        { 0xa551e015, 0xdaf5a5e4, 0xe292ea6c, 0xac5beeeb },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          32,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.12 ([Keylen = 256] [IVlen = 96] [PTlen = 0] [AADlen = 128] [Taglen = 128]) gcmDecrypt256.rsp Line:895 */
    {
        // Key
        { 0xd6affd6d, 0x5c283c70, 0xd14ff101, 0x8612600a, 0x50f92a2b, 0xbb3a73d4, 0x742e3b40, 0x5d94265b },
        // iv
        { 0xb3d04937, 0x71cbbad5, 0xe6ad06be, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0x8749d2c0, 0x03e79219, 0x8100ae02, 0x9fe8d193, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        128,
        // data_in(Cipher Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Plain Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        0,
        // auth_tag
        { 0x69cca44a, 0xace64ef8, 0xb4bfd916, 0x00e55de0 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          32,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.13 [Keylen = 256] [IVlen = 96] [PTlen = 128] [AADlen = 0] [Taglen = 128] gcmDecrypt256.rsp Line: 4423 */
    {
        // Key
        { 0xe1bf8e4c, 0xb2c14e44, 0x98c603d5, 0x2caf5966, 0x94fefa94, 0xe8c1725f, 0xcf5a6a48, 0xf8a0b8ed },
        // iv
        { 0xe0603347, 0x998824ad, 0x95898559, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        0,
        // data_in(Cipher Text)
        { 0x1081c7d2, 0x108f7eac, 0x57f00d7c, 0x0cc9d70b, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Plain Text)
        { 0x1cb48977, 0x8854eeb3, 0x380bca14, 0x43b3108c, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        128,
        // auth_tag
        { 0x9b376ac2, 0x28ef986d, 0xced8ea52, 0xa733a883 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          32,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.14 [Keylen = 256] [IVlen = 96] [PTlen = 128] [AADlen = 128] [Taglen = 128] gcmDecrypt256.rsp Line: 5305 */
    {
        // Key
        { 0xea52e354, 0xe6bf841d, 0x0911104a, 0xe7fb1161, 0x20d28a66, 0x012a903d, 0xbd3b8c45, 0x14cebf85 },
        // iv
        { 0xca3b7cdf, 0x0c6d3900, 0xd9958401, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0x718d967e, 0x111f0cb5, 0x3f1f00fd, 0x45d049ef, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        128,
        // data_in(Cipher Text)
        { 0xfc0e6e42, 0xe17b3b69, 0xb78d01f3, 0x4d7ebbdd, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Plain Text)
        { 0xfa3dfc85, 0xd3a8b5d9, 0xc44f8e25, 0x3bbd7145, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        128,
        // auth_tag
        { 0x795782ee, 0x16a1e65b, 0x2d1d7e4d, 0xa777ac6c },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          32,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.15 [Keylen = 256] [IVlen = 96] [PTlen = 256] [AADlen = 128] [Taglen = 128] gcmDecrypt256.rsp Line : 14133 */
    {
        // Key
        { 0xb4f7eaf0, 0x50f4421b, 0x05bc3506, 0x11deced9, 0x593d36a5, 0x708828a6, 0xffbc27f5, 0x046e4deb },
        // iv
        { 0x7816f318, 0x95a57710, 0x074c2dc7, 0x00000000 },
        // iv_size_in_bits
        96,
        // auth_data(AAD)
        { 0x3adeca42, 0x7d4b2019, 0x8c624348, 0x75235c42, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        128,
        // data_in(Cipher Text)
        { 0x00611b7a, 0x7c6bce9d, 0x02ead1d4, 0xf179b103, 0xced59d21, 0x2ee10774, 0x6cc5a4a0, 0x1b79bb71 },
        // data_out(Plain Text)
        { 0xefb50f40, 0x3a3b0832, 0xc457a9be, 0xadab68f0, 0x6bd8c850, 0xfa5193bf, 0x51dae772, 0xf938df71 },
        // data_size_in_bits
        256,
        // auth_tag
        { 0x0b181944, 0x723b960b, 0x3ffaa489, 0xa335c545 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          32,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_GCM,    0,              JOB_ALL_AT_ONCE
    },


/***************************/
/* CCM                     */
/***************************/
    /* No.16 ([Keylen = 128] [Alen = 0] [Plen = 0] [Nlen = 56] [Tlen = 32]) <DVPT128.txt Line:11 Count = 0> */
    {
        // Key(K)
        { 0x1001e74a, 0xcade633c, 0x39395a5b, 0x9259d0d7, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // Noce(N)
        { 0x85a48a5a, 0x00e916c3, 0x00000000, 0x00000000 },
        // iv_size_in_bits
        CCM_NONCESIZE_56,
        // auth_data(A)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        0,
        // data_in(Cipher Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Payload)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        0,
        // auth_tag(T)
        { 0x559f2002, 0x00000000, 0x00000000, 0x00000000 },
        // auth_tag_size_in_bits
        TAG_SIZE_32,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_CCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.17 ([Keylen = 128] [Alen = 256] [Plen = 192] [Nlen = 104] [Tlen = 128]) <DVPT128.txt Line:1496 Count = 225> */
    {
        // Key(K)
        { 0xb51f5126, 0x5ca7cf1f, 0xa74db4b4, 0x0e5a6e5a, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // Noce(N)
        {0x85a48a5a, 0x40e916c3, 0x9f85ff3a, 0x000000bb },
        // iv_size_in_bits
        CCM_NONCESIZE_104,
        // auth_data(A)
        { 0x742e6aa1, 0x71d91c1f, 0xd8b68572, 0x53fcc182, 0x73975e65, 0x97d61a76, 0x1064eea7, 0x82794c18 },
        // auth_data_size_in_bits
        256,
        // data_in(Cipher Text)
        { 0x5f8b0350, 0xe74e36dd, 0x000db747, 0x0e8436bd, 0x99a14ece, 0x75331298, 0x00000000, 0x00000000 },
        // data_out(Payload)
        { 0xbeb43987, 0xfe99a0a1, 0xcb997454, 0x3db1d1c6, 0x84809b84, 0xc5acb6c9, 0x00000000, 0x00000000 },
        // data_size_in_bits
        192,
        // auth_tag(T)
        { 0xbf58a4c0, 0x263bfaca, 0xf8e0af09, 0x03f5cb25 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_CCM,    0,              JOB_ALL_AT_ONCE
    },
    /* No.18 ([Keylen = 192] [Alen = 0] [Plen = 0] [Nlen = 56] [Tlen = 32]) <DVPT192.txt Line:11 Count = 0> */
    {
        // Key(K)
        { 0xf3d78ac9, 0x977e2c8b, 0x5e969b0c, 0x20087ac8, 0x8f718483, 0x6c6c2078, 0x00000000, 0x00000000 },
        // Noce(N)
        { 0x85a48a5a, 0x00e916c3, 0x00000000, 0x00000000 },
        // iv_size_in_bits
        CCM_NONCESIZE_56,
        // auth_data(A)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        0,
        // data_in(Cipher Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Payload)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        0,
        // auth_tag(T)
        { 0x3b7f4b9d, 0x00000000, 0x00000000, 0x00000000 },
        // auth_tag_size_in_bits
        TAG_SIZE_32,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          24,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_CCM,    0,              JOB_ALL_AT_ONCE
    },
    /* No.19 ([Keylen = 192] [[Alen = 256]] [Plen = 192] [Nlen = 104] [Tlen = 128]) <DVPT192.txt Line:1496 Count = 225> */
    {
        // Key(K)
        { 0xb51f5126, 0x5ca7cf1f, 0xa74db4b4, 0x0e5a6e5a, 0xf3c8d9b8, 0x86f806b9, 0x00000000, 0x00000000 },
        // Noce(N)
        { 0x85a48a5a, 0x40e916c3, 0x9f85ff3a, 0x000000bb },
        // iv_size_in_bits
        CCM_NONCESIZE_104,
        // auth_data(A)
        { 0x742e6aa1, 0x71d91c1f, 0xd8b68572, 0x53fcc182, 0x73975e65, 0x97d61a76, 0x1064eea7, 0x82794c18 },
        // auth_data_size_in_bits
        256,
        // data_in(Cipher Text)
        { 0xefb2b0c5, 0x558c4917, 0x5d33eb70, 0x328058f4, 0xd6a38b95, 0x17f3f69b, 0x00000000, 0x00000000 },
        // data_out(Payload)
        { 0xbeb43987, 0xfe99a0a1, 0xcb997454, 0x3db1d1c6, 0x84809b84, 0xc5acb6c9, 0x00000000, 0x00000000 },
        // data_size_in_bits
        192,
        // auth_tag(T)
        { 0xf7a66484, 0x74762bfa, 0x958d8e4e, 0xb8ec1c69 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          24,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_CCM,    0,              JOB_ALL_AT_ONCE
    },
    /* No.20 ([Keylen = 256] [Alen = 0] [Plen = 0] [Nlen = 56] [Tlen = 32]) <DVPT256.txt Line:11 Count = 0> */
    {
        // Key(K)
        { 0x752fa3ed, 0x31e35614, 0x99f4f195, 0xc9c72dcf, 0xb627a17e, 0x11f288d4, 0x6f12c5cc, 0xa6af24bb },
        // Noce(N)
        { 0x8d2144a5, 0x00c1d3ad, 0x00000000, 0x00000000 },
        // iv_size_in_bits
        CCM_NONCESIZE_56,
        // auth_data(A)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        0,
        // data_in(Cipher Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Payload)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        0,
        // auth_tag(T)
        { 0xbb909c46, 0x00000000, 0x00000000, 0x00000000 },
        // auth_tag_size_in_bits
        TAG_SIZE_32,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          32,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_CCM,    0,              JOB_ALL_AT_ONCE
    },
    /* No.21 ([Keylen = 256] [[Alen = 256]] [Plen = 192] [Nlen = 104] [Tlen = 128]) <DVPT256.txt Line:1496 Count = 225> */
    {
        // Key(K)
        { 0x2f204a31, 0x259f6f83, 0xc1d8227e, 0x2a835717, 0x351d13e5, 0x88df727a, 0xfff0eff3, 0x4edae0ce },
        // Noce(N)
        { 0x8d2144a5, 0x05c1d3ad, 0xcf49db83, 0x00000039 },
        // iv_size_in_bits
        CCM_NONCESIZE_104,
        // auth_data(A)
        { 0x15280e3c, 0x4f847dd3, 0xba40c27a, 0x0b3a6e9d, 0x06f7862a, 0x9e9585e8, 0x5e00a109, 0x07694f02 },
        // auth_data_size_in_bits
        256,
        // data_in(Cipher Text)
        { 0xcacd348d, 0xbe77ce37, 0xaf5bf668, 0x1ee38233, 0x633e69fa, 0x81a714f9, 0x00000000, 0x00000000 },
        // data_out(Payload)
        { 0x0f97dee8, 0x0ee8e86e, 0x813593de, 0xd8f4bcb5, 0x2bb7e237, 0xc3008baa, 0x00000000, 0x00000000 },
        // data_size_in_bits
        192,
        // auth_tag(T)
        { 0xf2307f36, 0x068cadea, 0x9507a53c, 0x0302d9ac },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          32,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_CCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.22 ([Keylen = 128] [N(IV)len = 56] [P(PT)len = 32] [A(AAD)len = 64] [Tlen = 32]) <SP_800-38C_CCM.PDF Example1> */
    {
        // Key(K)
        { 0x43424140, 0x47464544, 0x4b4a4948, 0x4f4e4d4c, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // Noce(N)
        { 0x13121110, 0x00161514, 0x00000000, 0x00000000 },
        // iv_size_in_bits
        CCM_NONCESIZE_56,
        // auth_data(A)
        { 0x03020100, 0x07060504, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        64,
        // data_in(Cipher Text)
        { 0x5b016271, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Payload)
        { 0x23222120, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        32,
        // auth_tag(T)
        { 0x5d25ac4d, 0x00000000, 0x00000000, 0x00000000 },
        // auth_tag_size_in_bits
        TAG_SIZE_32,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_CCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.23 ([Keylen = 128] [N(IV)len = 64] [P(PT)len = 128] [A(AAD)len = 128] [Tlen = 48]) <SP_800-38C_CCM.PDF Example2> */
    {
        // Key(K)
        { 0x43424140, 0x47464544, 0x4b4a4948, 0x4f4e4d4c, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // Noce(N)
        { 0x13121110, 0x17161514, 0x00000000, 0x00000000 },
        // iv_size_in_bits
        CCM_NONCESIZE_64,
        // auth_data(A)
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        128,
        // data_in(Cipher Text)
        { 0xe0f0a1d2, 0x625fea51, 0x92771a08, 0x3d593d07, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Payload)
        { 0x23222120, 0x27262524, 0x2b2a2928, 0x2f2e2d2c, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        128,
        // auth_tag(T)
        { 0xbf4fc61f, 0x0000cdac, 0x00000000, 0x00000000 },
        // auth_tag_size_in_bits
        TAG_SIZE_48,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_CCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.24 ([Keylen = 128] [N(IV)len = 96] [P(PT)len = 192] [A(AAD)len = 160] [Tlen = 64]) <SP_800-38C_CCM.PDF Example3> */
    {
        // Key(K)
        { 0x43424140, 0x47464544, 0x4b4a4948, 0x4f4e4d4c, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // Noce(N)
        { 0x13121110, 0x17161514, 0x1b1a1918, 0x00000000 },
        // iv_size_in_bits
        CCM_NONCESIZE_96,
        // auth_data(A)
        { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, 0x13121110, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        160,
        // data_in(Cipher Text)
        { 0xa901b2e3, 0x7a1ab7f5, 0xecea1c9b, 0x0be797cd, 0xd9aa7661, 0xa58a42a4, 0x00000000, 0x00000000 },
        // data_out(Payload)
        { 0x23222120, 0x27262524, 0x2b2a2928, 0x2f2e2d2c, 0x33323130, 0x37363534, 0x00000000, 0x00000000 },
        // data_size_in_bits
        192,
        // auth_tag(T)
        { 0xfb924348, 0x5199b0c1, 0x00000000, 0x00000000 },
        // auth_tag_size_in_bits
        TAG_SIZE_64,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_CCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.25 ([Alen = 0, Plen = 24, Nlen = 7, Tlen = 4]) <DVPT128.txt Line:407 Count = 60> */
    {
        // Key(K)
        { 0xe2fdeb19, 0xa08b46d5, 0xde1b03a3, 0xfd119b62, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // Noce(N)
        { 0x85a48a5a, 0x00e916c3, 0x00000000, 0x00000000 },
        // iv_size_in_bits
        CCM_NONCESIZE_56,
        // auth_data(A)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        0,
        // data_in(Cipher Text)
        { 0xa48e0ea9, 0xd7ce8540, 0xb7fdb291, 0xcfb544fd, 0x77d2d70b, 0xb79b0218, 0x00000000, 0x00000000 },
        // data_out(Payload)
        { 0x51cf9637, 0x526672b8, 0x334720a4, 0x47b0fbb8, 0x91fb00cf, 0x227e83a9, 0x00000000, 0x00000000 },
        // data_size_in_bits
        192,
        // auth_tag(T)
        { 0x6bfae103, 0x00000000, 0x00000000, 0x00000000 },
        // auth_tag_size_in_bits
        TAG_SIZE_32,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_CCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.26 ([Alen = 32, Plen = 0, Nlen = 7, Tlen = 4]) <DVPT128.txt Line:799 Count = 120> */
    {
        // Key(K)
        { 0x4b9a9290, 0x355bc60a, 0x1659d10a, 0x2948fe11, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // Noce(N)
        { 0x85a48a5a, 0x00e916c3, 0x00000000, 0x00000000 },
        // iv_size_in_bits
        CCM_NONCESIZE_56,
        // auth_data(A)
        { 0x51cf9637, 0x526672b8, 0x334720a4, 0x47b0fbb8, 0x91fb00cf, 0x227e83a9, 0xa2b122ec, 0x2c8ef868 },
        // auth_data_size_in_bits
        256,
        // data_in(Cipher Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Payload)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        0,
        // auth_tag(T)
        { 0x18432e78, 0x00000000, 0x00000000, 0x00000000 },
        // auth_tag_size_in_bits
        TAG_SIZE_32,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_CCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.27 ([Alen = 0, Plen = 0, Nlen = 7, Tlen = 16]) <DVPT128.txt Line:110 Count = 15> */
    {
        // Key(K)
        { 0xa4c4b34b, 0x8cad93f8, 0x3c83dc9b, 0xb3625d32, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // Noce(N)
        { 0x85a48a5a, 0x00e916c3, 0x00000000, 0x00000000 },
        // iv_size_in_bits
        CCM_NONCESIZE_56,
        // auth_data(A)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        0,
        // data_in(Cipher Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Payload)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        0,
        // auth_tag(T)
        { 0xdb82d575, 0x139bce43, 0x7f6f4bab, 0x30133414 },
        // auth_tag_size_in_bits
        TAG_SIZE_128,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_CCM,    0,              JOB_ALL_AT_ONCE
    },

    /* No.28 ([Alen = 0, Plen = 0, Nlen = 13, Tlen = 4]) <DVPT128.txt Line:209 Count = 30> */
    {
        // Key(K)
        { 0xa4c4b34b, 0x8cad93f8, 0x3c83dc9b, 0xb3625d32, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // Noce(N)
        { 0x85a48a5a, 0x40e916c3, 0x9f85ff3a, 0x000000bb },
        // iv_size_in_bits
        CCM_NONCESIZE_104,
        // auth_data(A)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // auth_data_size_in_bits
        0,
        // data_in(Cipher Text)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_out(Payload)
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // data_size_in_bits
        0,
        // auth_tag(T)
        { 0x3f6f1590, 0x00000000, 0x00000000, 0x00000000 },
        // auth_tag_size_in_bits
        TAG_SIZE_32,
        // key_group        // key_id   // key_size // direction            // cipher_mode          // job_slice    // job_cycle   
        KEY_GRP_AES_RAM,    0,          16,         CIPHER_DIR_DECRYPTION,  AUTH_CIPHER_MOD_CCM,    0,              JOB_ALL_AT_ONCE
    }

};

static uint8_t g_uiTest100_EncOKCounter;
static uint8_t g_uiTest100_DecOKCounter;

void TEST_AuthCipher(ISD_AES_AUTH_CIPHER_t *p_stAES_AUTH_CIPHER, uint8_t uiMode, uint16_t *uiJobID);

// ********************************************************************************
// Test functions
// ********************************************************************************

/*===========================================================================
  ICUMXA test function
  [PUBLIC]
  ===========================================================================*/
void icum_test_aes_auth_cipher(void)
{
    r_icumif_isd_t          *p_ISD;
    ISD_AES_AUTH_CIPHER_t   stAES_AUTH_CIPHER;
    uint16_t                uiJobID;
    uint8_t                 uiEncCounter;
    uint8_t                 uiDecCounter;

    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(BIG_BUFFER[2], 0, SIZE_OF_BIG_BUFFER);
    g_uiTest100_EncOKCounter = 0;
    g_uiTest100_DecOKCounter = 0;
    uiEncCounter = 0;
    uiDecCounter = 0;

    /* Start Key registration */
    printf("\nStart Key registration(NVM key)...\n\n");

    p_ISD->service_id                             = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD->p_callbackfunc                         = 0;
    p_ISD->job_id                                 = 0;

    p_ISD->prm.KEY_UPDATE_PLAIN.key_group         = KEY_GRP_AES_RAM;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_id            = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value       = TEST_KEY_2;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes = 16;
    
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;

    printf("AES key #%d\n", p_ISD->prm.KEY_UPDATE_PLAIN.key_id);

    trigger_service_and_wait(p_ISD);

    printf("\nDone!\n");

    /********************************************************************/
    printf("\n=========================================================\n");
    printf("TEST 1(CCM)");
    printf("\n=========================================================\n\n");
    printf("--------------------------------------\n");
    uiJobID = 0;
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = CCM_NONCESIZE_104;

    stAES_AUTH_CIPHER.p_auth_data              = TEST_IV;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 128u;

    stAES_AUTH_CIPHER.p_data_in                = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = NULL;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_CCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_ALL_AT_ONCE;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ALL_AT_ONCE, &uiJobID);

    uiEncCounter++;
    uiDecCounter++;

    /********************************************************************/
    printf("\n=========================================================\n");
    printf("TEST 2(GCM)");
    printf("\n=========================================================\n\n");
    printf("--------------------------------------\n");
    uiJobID = 0;
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_ALL_AT_ONCE;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ALL_AT_ONCE, &uiJobID);

    uiEncCounter++;
    uiDecCounter++;

    /********************************************************************/
    printf("\n=========================================================\n");
    printf("TEST 3(GCM) :Job Slice");
    printf("\n=========================================================\n\n");

    printf("Test3-1 (SliceSize:2 TotalBlock:4 Rem:124) \n");
    printf("--------------------------------------\n");
    uiJobID = 0;
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8 - 4;

    stAES_AUTH_CIPHER.p_auth_tag               = NULL;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 2;
    stAES_AUTH_CIPHER.job_cycle                = JOB_ALL_AT_ONCE;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ALL_AT_ONCE, &uiJobID);

    uiEncCounter++;
    uiDecCounter++;

    /******************/
    printf("Test3-2 (SliceSize:1 TotalBlock:4) \n");
    printf("--------------------------------------\n");
    uiJobID = 0;
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = NULL;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 1;
    stAES_AUTH_CIPHER.job_cycle                = JOB_ALL_AT_ONCE;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ALL_AT_ONCE, &uiJobID);

    uiEncCounter++;
    uiDecCounter++;

    /******************/
    printf("Test3-3 Only AAD(SliceSize:0 TotalBlock:0) \n");
    printf("--------------------------------------\n");
    uiJobID = 0;
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 32 * 8;

    stAES_AUTH_CIPHER.p_data_in                = NULL;
    stAES_AUTH_CIPHER.data_size_in_bits        = 0;

    stAES_AUTH_CIPHER.p_auth_tag               = NULL;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_ALL_AT_ONCE;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ALL_AT_ONCE, &uiJobID);

    uiEncCounter++;
    uiDecCounter++;

    /********************************************************************/
    printf("\n=========================================================\n");
    printf("TEST 4(GCM) :Job Cycle");
    printf("\n=========================================================\n\n");
    uiJobID = 0;
    /******************************************************************************************************************/
    printf("Test4-1 (SliceSize:0 TotalBlock:4 Rem:128) \n");
    printf("--------------------------------------\n");
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    /* Encription */
    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_INIT;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ENCRIPT, &uiJobID);

    /******************/
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 32 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_UPDATE;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ENCRIPT, &uiJobID);

    /******************/
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_PLAIN_TEXT + (32 / 4);
    stAES_AUTH_CIPHER.data_size_in_bits        = 32 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_FINISH;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ENCRIPT, &uiJobID);

    /////////////////////////////////////////////////////////////////////
    /* Decription */
    uiJobID = 0;
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_CIPHER_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_INIT;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, DECRIPT, &uiJobID);

    /******************/
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_CIPHER_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 32 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_UPDATE;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, DECRIPT, &uiJobID);

    /******************/
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_CIPHER_TEXT + (32 / 4);
    stAES_AUTH_CIPHER.data_size_in_bits        = 32 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_FINISH;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, DECRIPT, &uiJobID);

    uiEncCounter++;
    uiDecCounter++;

    /******************************************************************************************************************/
    printf("Test4-2 Init -> Finish (SliceSize:2 TotalBlock:4) \n");
    printf("--------------------------------------\n");
    uiJobID = 0;
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    /* Encription */
    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_INIT;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ENCRIPT, &uiJobID);

    /******************/
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 2;
    stAES_AUTH_CIPHER.job_cycle                = JOB_FINISH;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ENCRIPT, &uiJobID);


    /////////////////////////////////////////////////////////////////////
    /* Decription */
    uiJobID = 0;
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_CIPHER_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_INIT;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, DECRIPT, &uiJobID);

    /******************/
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_CIPHER_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 2;
    stAES_AUTH_CIPHER.job_cycle                = JOB_FINISH;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, DECRIPT, &uiJobID);

    uiEncCounter++;
    uiDecCounter++;

    /******************************************************************************************************************/
    printf("Test4-3 Init -> Finish (SliceSize:0 TotalBlock:4) \n");
    printf("--------------------------------------\n");
    uiJobID = 0;
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    /* Encription */
    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_INIT;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ENCRIPT, &uiJobID);

    /******************/
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_FINISH;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ENCRIPT, &uiJobID);


    /////////////////////////////////////////////////////////////////////
    /* Decription */
    uiJobID = 0;
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_CIPHER_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_INIT;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, DECRIPT, &uiJobID);

    /******************/
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = NULL;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 0;

    stAES_AUTH_CIPHER.p_data_in                = TEST_CIPHER_TEXT;
    stAES_AUTH_CIPHER.data_size_in_bits        = 64 * 8;

    stAES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)TEST_OUTPUT_TAG;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_FINISH;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, DECRIPT, &uiJobID);

    uiEncCounter++;
    uiDecCounter++;

    /******************************************************************************************************************/
    printf("Test4-4 AAD Only(SliceSize:0 TotalBlock:0) \n");
    printf("--------------------------------------\n");
    uiJobID = 0;
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    /* Encription */
    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 64 * 8;

    stAES_AUTH_CIPHER.p_data_in                = NULL;
    stAES_AUTH_CIPHER.data_size_in_bits        = 0;

    stAES_AUTH_CIPHER.p_auth_tag               = NULL;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_INIT;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ENCRIPT, &uiJobID);

    /******************/
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 64 * 8;

    stAES_AUTH_CIPHER.p_data_in                = NULL;
    stAES_AUTH_CIPHER.data_size_in_bits        = 0;

    stAES_AUTH_CIPHER.p_auth_tag               = NULL;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_FINISH;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, ENCRIPT, &uiJobID);

    /////////////////////////////////////////////////////////////////////
    /* Decription */
    uiJobID = 0;
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 64 * 8;

    stAES_AUTH_CIPHER.p_data_in                = NULL;
    stAES_AUTH_CIPHER.data_size_in_bits        = 0;

    stAES_AUTH_CIPHER.p_auth_tag               = NULL;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_INIT;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, DECRIPT, &uiJobID);

    /******************/
    memset(&stAES_AUTH_CIPHER, 0x00, sizeof(stAES_AUTH_CIPHER));

    stAES_AUTH_CIPHER.p_iv                     = TEST_IV;
    stAES_AUTH_CIPHER.iv_size_in_bits          = 128u;

    stAES_AUTH_CIPHER.p_auth_data              = TEST_PLAIN_TEXT;
    stAES_AUTH_CIPHER.auth_data_size_in_bits   = 64 * 8;

    stAES_AUTH_CIPHER.p_data_in                = NULL;
    stAES_AUTH_CIPHER.data_size_in_bits        = 0;

    stAES_AUTH_CIPHER.p_auth_tag               = NULL;
    stAES_AUTH_CIPHER.auth_tag_size_in_bits    = TAG_SIZE_128;

    stAES_AUTH_CIPHER.key_group                = KEY_GRP_AES_RAM;
    stAES_AUTH_CIPHER.key_id                   = 0;

    stAES_AUTH_CIPHER.cipher_mode              = AUTH_CIPHER_MOD_GCM;
    stAES_AUTH_CIPHER.job_slice                = 0;
    stAES_AUTH_CIPHER.job_cycle                = JOB_FINISH;

    TEST_AuthCipher(&stAES_AUTH_CIPHER, DECRIPT, &uiJobID);

    uiEncCounter++;
    uiDecCounter++;

    printf("\n===================================\n"); 
    printf(" Total Count = %d\n", (uiEncCounter + uiDecCounter)); 
    printf(" Encode OK   = %d\n", g_uiTest100_EncOKCounter); 
    printf(" Encode NG   = %d\n", uiEncCounter - g_uiTest100_EncOKCounter); 
    printf(" Decode OK   = %d\n", g_uiTest100_DecOKCounter); 
    printf(" Decode NG   = %d\n", uiDecCounter - g_uiTest100_DecOKCounter); 
    printf("===================================\n"); 

    /******************************************************************************************************************/

    return;
}

void TEST_AuthCipher(ISD_AES_AUTH_CIPHER_t *p_stAES_AUTH_CIPHER, uint8_t uiMode, uint16_t *uiJobID)
{
    uint8_t        *p_uiAuthTAG;
    uint8_t        *p_Data;
    r_icumif_isd_t *p_ISD;
    int32_t         iRet;

    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    memset(BIG_BUFFER[0], 0, SIZE_OF_BIG_BUFFER);
    memset(BIG_BUFFER[1], 0, SIZE_OF_BIG_BUFFER);

    if((ALL_AT_ONCE == uiMode) || (ENCRIPT == uiMode))
    {
        /* Case: Encription */
        printf("ENCRIPTION...\n");
        /********************************************************************************************/
        p_ISD->service_id                                   = SERVICE_01_AES_AUTH_CIPHER;
        p_ISD->p_callbackfunc                               = 0;
        p_ISD->job_id                                       = *uiJobID;
        p_ISD->service_priority                             = R_ICUMIF_SERV_PRIORITY_NORMAL;

        p_ISD->prm.AES_AUTH_CIPHER.p_iv                     = p_stAES_AUTH_CIPHER->p_iv;
        p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits          = p_stAES_AUTH_CIPHER->iv_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.p_auth_data              = p_stAES_AUTH_CIPHER->p_auth_data;
        p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits   = p_stAES_AUTH_CIPHER->auth_data_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.p_data_in                = p_stAES_AUTH_CIPHER->p_data_in;
        p_ISD->prm.AES_AUTH_CIPHER.p_data_out               = (uint32_t *)BIG_BUFFER[0];
        p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits        = p_stAES_AUTH_CIPHER->data_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)BIG_BUFFER[2];
        p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits    = p_stAES_AUTH_CIPHER->auth_tag_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.key_group                = p_stAES_AUTH_CIPHER->key_group;
        p_ISD->prm.AES_AUTH_CIPHER.key_id                   = p_stAES_AUTH_CIPHER->key_id;
        p_ISD->prm.AES_AUTH_CIPHER.direction                = CIPHER_DIR_ENCRYPTION;
        p_ISD->prm.AES_AUTH_CIPHER.cipher_mode              = p_stAES_AUTH_CIPHER->cipher_mode;
        p_ISD->prm.AES_AUTH_CIPHER.job_slice                = p_stAES_AUTH_CIPHER->job_slice;
        p_ISD->prm.AES_AUTH_CIPHER.job_cycle                = p_stAES_AUTH_CIPHER->job_cycle;
        /********************************************************************************************/

        trigger_service_and_wait(p_ISD);

        if(p_ISD->service_result == SERV_OK)
        {
            p_Data = (uint8_t *)p_ISD->prm.AES_AUTH_CIPHER.p_data_out;

            if ((JOB_ALL_AT_ONCE == p_stAES_AUTH_CIPHER->job_cycle) ||
                (JOB_FINISH == p_stAES_AUTH_CIPHER->job_cycle))
            {
                p_uiAuthTAG = (uint8_t *)p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag;

                if (p_stAES_AUTH_CIPHER->p_auth_tag != NULL)
                {
                    iRet = memcmp( p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag, 
                                   p_stAES_AUTH_CIPHER->p_auth_tag, 16);
                    if (0 == iRet)
                    {
                        /* OK */
                        g_uiTest100_EncOKCounter++;
                        printf("TAG  :Match!\n");
                    }
                    else
                    {
                        printf("TAG  :Mismatch!!!\n");

                        /* OUTPUT Data */
                        printf("OUTPUT Data   : ");
                        icum_aes_print_Out((uint32_t *)p_Data, 64);

                        printf("\n");

                        /* OUTPUT Tag */
                        printf("OUTPUT TAG   : ");
                        icum_aes_print_Out((uint32_t *)p_uiAuthTAG, 16);
                    }
                }
                else
                {
                    /* OK */
                    g_uiTest100_EncOKCounter++;
                }
                printf("--------------------------------------\n");
            }
        }
        else
        {
            printf("\n\nERROR! \n\n");
        }
    }

    if((ALL_AT_ONCE == uiMode) || (DECRIPT == uiMode))
    {
        /* Case: Decription */
        printf("DECRIPTION...\n");
        /********************************************************************************************/
        p_ISD->service_id                                   = SERVICE_01_AES_AUTH_CIPHER;
        p_ISD->p_callbackfunc                               = 0;
        p_ISD->job_id                                       = *uiJobID;
        p_ISD->service_priority                             = R_ICUMIF_SERV_PRIORITY_NORMAL;

        p_ISD->prm.AES_AUTH_CIPHER.p_iv                     = p_stAES_AUTH_CIPHER->p_iv;
        p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits          = p_stAES_AUTH_CIPHER->iv_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.p_auth_data              = p_stAES_AUTH_CIPHER->p_auth_data;
        p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits   = p_stAES_AUTH_CIPHER->auth_data_size_in_bits;

        if (ALL_AT_ONCE == uiMode)
        {
            /* In AtOnce case, input the cipher data encoded  */
            p_ISD->prm.AES_AUTH_CIPHER.p_data_in            = (uint32_t *)BIG_BUFFER[0];
        }
        else
        {
            /* In Non AtOnce case, input the cipher data expected (for dividing) */
            p_ISD->prm.AES_AUTH_CIPHER.p_data_in            = (uint32_t *)p_stAES_AUTH_CIPHER->p_data_in;
        }
        p_ISD->prm.AES_AUTH_CIPHER.p_data_out               = (uint32_t *)BIG_BUFFER[1];
        p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits        = p_stAES_AUTH_CIPHER->data_size_in_bits;

        if (NULL == p_stAES_AUTH_CIPHER->p_auth_tag)
        {
            /* If Tag is NULL, compare with the TAG of the encoding result. */
            p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag           = (uint32_t *)BIG_BUFFER[2];
        }
        else
        {
            /* If Tag is not NULL, compare with expected value. */
            p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag           = p_stAES_AUTH_CIPHER->p_auth_tag;
        }
        p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits    = p_stAES_AUTH_CIPHER->auth_tag_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.key_group                = p_stAES_AUTH_CIPHER->key_group;
        p_ISD->prm.AES_AUTH_CIPHER.key_id                   = p_stAES_AUTH_CIPHER->key_id;
        p_ISD->prm.AES_AUTH_CIPHER.direction                = CIPHER_DIR_DECRYPTION;
        p_ISD->prm.AES_AUTH_CIPHER.cipher_mode              = p_stAES_AUTH_CIPHER->cipher_mode;
        p_ISD->prm.AES_AUTH_CIPHER.job_slice                = p_stAES_AUTH_CIPHER->job_slice;
        p_ISD->prm.AES_AUTH_CIPHER.job_cycle                = p_stAES_AUTH_CIPHER->job_cycle;
        p_ISD->prm.AES_AUTH_CIPHER.verification_result      = VERIFICATION_PASS;
        /********************************************************************************************/

        trigger_service_and_wait(p_ISD);

        if(p_ISD->service_result == SERV_OK)
        {
            p_Data = (uint8_t *)p_ISD->prm.AES_AUTH_CIPHER.p_data_out;

            if ((JOB_ALL_AT_ONCE == p_stAES_AUTH_CIPHER->job_cycle) ||
                (JOB_FINISH == p_stAES_AUTH_CIPHER->job_cycle))
            {
                p_uiAuthTAG = (uint8_t *)p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag;
                printf("--------------------------------------\n");
                printf("@@@Result:   ");
                if (VERIFICATION_PASS == p_ISD->prm.AES_AUTH_CIPHER.verification_result)
                {
                    printf("VERIFICATION_PASS!     @@@\n");

                    /* OK */
                    g_uiTest100_DecOKCounter++;
                }
                else
                {
                    printf("VERIFICATION_FAIL!     @@@\n");

                    /* OUTPUT Data */
                    printf("OUTPUT Data   : ");
                    icum_aes_print_Out((uint32_t *)p_Data, 64);
                    printf("\n");

                    /* OUTPUT Data */
                    printf("OUTPUT TAG   : ");
                    icum_aes_print_Out((uint32_t *)p_uiAuthTAG, 16);
                    printf("\n");
                }
                printf("--------------------------------------\n\n");
                memset(BIG_BUFFER[2], 0, SIZE_OF_BIG_BUFFER);
            }
        }
        else
        {
            printf("\n\nERROR! \n\n");
        }
    }

    /* Set JobID */
    *uiJobID = p_ISD->job_id;
}

/*===========================================================================
  ICUMXA test function
  [PUBLIC]
  ===========================================================================*/
void icum_aes_auth_vector(void)
{
    r_icumif_isd_t *p_ISD;
    uint32_t        uiLoop;
    int32_t         iRet;
    uint32_t        uiStart     = 0;
    uint32_t        uiEnd       = 0;
    uint32_t        uiSelect    = 0;
    uint8_t         uiExitLoop  = 0;
    uint32_t        uiSize;
    uint32_t        uiEncOKCount = 0;
    uint32_t        uiDecOKCount = 0;

    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nStart AES Auth Cipher Vector \n"); 

    while(!uiExitLoop)
    {
        /* Get Input Parameter */
        printf("\n| Please Select Vector\n");
        printf("|---------------------------------\n");
        printf("| 0    : All Vector\n");
        printf("| 1-%d : Select Vector\n", DEF_VECTOR_NUM);
        printf("  ?>");

        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%du", &uiSelect);

        if(((int32_t)uiSelect >= 0u) && (uiSelect <= DEF_VECTOR_NUM))
        {
            uiExitLoop = 1;
        }
        else
        {
            printf("Parameter Error!!\n\n");
        }
    }

    if (0u == uiSelect)
    {
        /* All Vector */
        uiStart = 0;
        uiEnd   = DEF_VECTOR_NUM;
    }
    else
    {
        /* Select Vector */
        uiStart = uiSelect - 1u;
        uiEnd   = uiSelect;
    }

    for (uiLoop = uiStart; uiLoop < uiEnd; uiLoop++)
    {
        /********************************************************************************************/
        /* Set Key */
        printf("\n===================================\n"); 
        printf(" +++++ Start Test Vector %d +++++\n", uiLoop + 1); 
        printf("===================================\n"); 
        printf("<<Data Infomation>>\n"); 

        if (AUTH_CIPHER_MOD_GCM == test_vector_auth[uiLoop].cipher_mode)
        {
            printf("[mode = GCM], ");
        }
        else
        {
            printf("[mode = CCM], ");
        }

        printf("[KeySize = %d], [job_slice = %d]\n[IV = %d], [AAD = %d], [Data = %d], [Tag = %d]\n", 
                test_vector_auth[uiLoop].key_size * 8,
                test_vector_auth[uiLoop].job_slice,
                test_vector_auth[uiLoop].iv_size_in_bits, 
                test_vector_auth[uiLoop].auth_data_size_in_bits, 
                test_vector_auth[uiLoop].data_size_in_bits, 
                test_vector_auth[uiLoop].auth_tag_size_in_bits);

        p_ISD->service_id                             = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD->p_callbackfunc                         = 0;
        p_ISD->job_id                                 = 0;
        p_ISD->service_priority                       = R_ICUMIF_SERV_PRIORITY_NORMAL;
        p_ISD->prm.KEY_UPDATE_PLAIN.key_group         = test_vector_auth[uiLoop].key_group;
        p_ISD->prm.KEY_UPDATE_PLAIN.key_id            = test_vector_auth[uiLoop].key_id;
        p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value       = (const uint8_t*)test_vector_auth[uiLoop].Key;
        p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes = test_vector_auth[uiLoop].key_size;
        
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        
        printf("\n<<Start AES Key Generate>>\n"); 
        trigger_service_and_wait(p_ISD);
        printf("Done!\n");

        /********************************************************************************************/
        /* Caluclate(Encode)_Provisional */
        memset(BIG_BUFFER[0], 0, SIZE_OF_BIG_BUFFER);
        memset(BIG_BUFFER[1], 0, SIZE_OF_BIG_BUFFER);

        printf("\n<<Encode Start>>\n");

        p_ISD->service_id                                   = SERVICE_01_AES_AUTH_CIPHER;
        p_ISD->p_callbackfunc                               = 0;
        p_ISD->job_id                                       = 0;
        p_ISD->service_priority                             = R_ICUMIF_SERV_PRIORITY_NORMAL;

        p_ISD->prm.AES_AUTH_CIPHER.p_iv                     = test_vector_auth[uiLoop].iv;
        p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits          = test_vector_auth[uiLoop].iv_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.p_auth_data              = test_vector_auth[uiLoop].auth_data;
        p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits   = test_vector_auth[uiLoop].auth_data_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.p_data_in                = test_vector_auth[uiLoop].data_out;
        p_ISD->prm.AES_AUTH_CIPHER.p_data_out               = (uint32_t *)BIG_BUFFER[0];
        p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits        = test_vector_auth[uiLoop].data_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)BIG_BUFFER[1];
        p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits    = test_vector_auth[uiLoop].auth_tag_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.key_group                = test_vector_auth[uiLoop].key_group;
        p_ISD->prm.AES_AUTH_CIPHER.key_id                   = test_vector_auth[uiLoop].key_id;
        p_ISD->prm.AES_AUTH_CIPHER.direction                = CIPHER_DIR_ENCRYPTION;
        p_ISD->prm.AES_AUTH_CIPHER.cipher_mode              = test_vector_auth[uiLoop].cipher_mode;
        p_ISD->prm.AES_AUTH_CIPHER.job_slice                = test_vector_auth[uiLoop].job_slice;
        p_ISD->prm.AES_AUTH_CIPHER.job_cycle                = test_vector_auth[uiLoop].job_cycle;

        trigger_service_and_wait(p_ISD);

        /********************************************************************************************/
        /* Check_Provisional */
        if(p_ISD->service_result == SERV_OK)
        {
            printf("SERV_OK!\n");
            printf("--- Check Result ---\n");
            iRet = memcmp( BIG_BUFFER[0], test_vector_auth[uiLoop].data_in, (test_vector_auth[uiLoop].data_size_in_bits / 8));
            if (0 == iRet)
            {
                printf("Cipher Text         :Match!\n");
            }
            else
            {
                printf("Cipher Text         :Mismatch!!!\n");

                // Output Data Start
                if (0 == (test_vector_auth[uiLoop].data_size_in_bits % 8))
                {
                    uiSize = p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits / 8;
                }
                else
                {
                    uiSize = (p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits / 8) + (test_vector_auth[uiLoop].data_size_in_bits % 8);
                }
                printf("ENC expected value  :");
                icum_aes_print_Out((uint32_t *)test_vector_auth[uiLoop].data_in, uiSize);
                printf("ENC output value    :");
                icum_aes_print_Out((uint32_t *)BIG_BUFFER[0], uiSize);
                // Output Data End
            }

            iRet = memcmp( BIG_BUFFER[1], test_vector_auth[uiLoop].auth_tag, (test_vector_auth[uiLoop].auth_tag_size_in_bits / 8));
            if (0 == iRet)
            {
                printf("Tag Output          :Match!\n");
                uiEncOKCount++;
            }
            else
            {
                printf("Tag Output          :Mismatch!!!\n");

                // Output Data Start
                if (0 == (test_vector_auth[uiLoop].auth_tag_size_in_bits % 8))
                {
                    uiSize = p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8;
                }
                else
                {
                    uiSize = (p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8) + (test_vector_auth[uiLoop].auth_tag_size_in_bits % 8);
                }
                printf("TAG expected value  :");
                icum_aes_print_Out((uint32_t *)test_vector_auth[uiLoop].auth_tag, uiSize);
                printf("TAG output value    :");
                icum_aes_print_Out((uint32_t *)BIG_BUFFER[1], uiSize);
                // Output Data End
            }
        }
        else
        {
            printf("Error Occured!!!\n");
        }

        /********************************************************************************************/
        /* Caluclate (Decode) */
        memset(BIG_BUFFER[2], 0, SIZE_OF_BIG_BUFFER);

        printf("\n<<Decode Start>>\n");

        p_ISD->service_id                                   = SERVICE_01_AES_AUTH_CIPHER;
        p_ISD->p_callbackfunc                               = 0;
        p_ISD->job_id                                       = 0;
        p_ISD->service_priority                             = R_ICUMIF_SERV_PRIORITY_NORMAL;

        p_ISD->prm.AES_AUTH_CIPHER.p_iv                     = test_vector_auth[uiLoop].iv;
        p_ISD->prm.AES_AUTH_CIPHER.iv_size_in_bits          = test_vector_auth[uiLoop].iv_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.p_auth_data              = test_vector_auth[uiLoop].auth_data;
        p_ISD->prm.AES_AUTH_CIPHER.auth_data_size_in_bits   = test_vector_auth[uiLoop].auth_data_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.p_data_in                = test_vector_auth[uiLoop].data_in;
        p_ISD->prm.AES_AUTH_CIPHER.p_data_out               = (uint32_t *)BIG_BUFFER[2];
        p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits        = test_vector_auth[uiLoop].data_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.p_auth_tag               = (uint32_t *)test_vector_auth[uiLoop].auth_tag;
        p_ISD->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits    = test_vector_auth[uiLoop].auth_tag_size_in_bits;

        p_ISD->prm.AES_AUTH_CIPHER.key_group                = test_vector_auth[uiLoop].key_group;
        p_ISD->prm.AES_AUTH_CIPHER.key_id                   = test_vector_auth[uiLoop].key_id;
        p_ISD->prm.AES_AUTH_CIPHER.direction                = test_vector_auth[uiLoop].direction;
        p_ISD->prm.AES_AUTH_CIPHER.cipher_mode              = test_vector_auth[uiLoop].cipher_mode;
        p_ISD->prm.AES_AUTH_CIPHER.job_slice                = test_vector_auth[uiLoop].job_slice;
        p_ISD->prm.AES_AUTH_CIPHER.job_cycle                = test_vector_auth[uiLoop].job_cycle;

        trigger_service_and_wait(p_ISD);

        /********************************************************************************************/
        /* Check */
        if(p_ISD->service_result == SERV_OK)
        {
            printf("SERV_OK!\n");
            printf("--- Check Result ---\n");
            if (VERIFICATION_PASS == p_ISD->prm.AES_AUTH_CIPHER.verification_result)
            {
                printf("verification_result: VERIFICATION_PASS!\n");
                iRet = memcmp( BIG_BUFFER[2], test_vector_auth[uiLoop].data_out, (test_vector_auth[uiLoop].data_size_in_bits / 8));
                if (0 == iRet)
                {
                    printf("Plain Text          :Match!\n");
                    uiDecOKCount++;
                }
                else
                {
                    printf("Plain Text          :Mismatch!!!\n");

                    // Output Data Start
                    if (0 == (test_vector_auth[uiLoop].data_size_in_bits % 8))
                    {
                        uiSize = p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits / 8;
                    }
                    else
                    {
                        uiSize = (p_ISD->prm.AES_AUTH_CIPHER.data_size_in_bits / 8) + (test_vector_auth[uiLoop].data_size_in_bits % 8);
                    }

                    printf("DEC expected value  :");
                    icum_aes_print_Out((uint32_t *)test_vector_auth[uiLoop].data_out, uiSize);
                    printf("DEC output value    :");
                    icum_aes_print_Out((uint32_t *)BIG_BUFFER[2], uiSize);
                    // Output Data End
                }
            }
            else
            {
                printf("verification_result :VERIFICATION_FAIL!!!\n");
            }
        }
        else
        {
            printf("Error Occured!!!\n");
        }
        /********************************************************************************************/
    }

    printf("\n===================================\n"); 
    printf(" Total Count = %d\n", uiEnd); 
    printf(" Encode OK   = %d\n", uiEncOKCount); 
    printf(" Encode NG   = %d\n", uiEnd - uiEncOKCount); 
    printf(" Decode OK   = %d\n", uiDecOKCount); 
    printf(" Decode NG   = %d\n", uiEnd - uiDecOKCount); 
    printf("===================================\n"); 

}

/*===========================================================================
  ICUMXA test function
  [PUBLIC]
  ===========================================================================*/
void icum_aes_print_GCMCCM(uint32_t *p_uiCmacOut)
{
    uint8_t *p_uiCmac;
    uint8_t uiLoop = 0;

        p_uiCmac = (uint8_t *)p_uiCmacOut;
        /* OUTPUT Data */
        for(uiLoop= 0; uiLoop<16; uiLoop++ )
        {
            printf("%02x", *p_uiCmac);
            p_uiCmac =  p_uiCmac + 1;
        }

        printf("\n");
}
