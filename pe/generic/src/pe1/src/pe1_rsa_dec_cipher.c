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

// ********************************************************************************
// Testing RSA Public Key Export
// ********************************************************************************

// ********************************************************************************
// Testing RSA Key Generate
// ********************************************************************************

// ********************************************************************************
// Testing RSA 2048 bit key
// ********************************************************************************


// ********************************************************************************
// Testing RSA 3072 bit key
// ********************************************************************************


// ********************************************************************************
// Testing RSA 2048 bit key with SHA2-256
// ********************************************************************************


// ********************************************************************************
// Testing RSA Decrypt (Keys)
// ********************************************************************************
/* Do not change the order */
#include "pe1_rsa_enc_dec_keys_1024.h"
#include "pe1_rsa_enc_dec_keys_2048.h"
#include "pe1_rsa_enc_dec_keys_512.h"
#include "pe1_rsa_enc_dec_keys_3072.h"

#define ENCMSG_LEN      sizeof(encoded_msg_oaep)
#define ENCMSG_LEN_2    (ENCMSG_LEN*2)
#define ENCMSG_LEN_3    309/*Minimum number of digits when 1024 bits are expressed in decimal*/

static const uint8_t label1_16B[] =
{
0x0f,0x0e,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00
};

// from "oaep-vect.txt" and "pkcs1v15crypt-vectors.txt"


/* Notation difference */
const uint8_t encoded_msg_oaep_hexa[] =
    "354fe67b4a126d5d35fe36c777791a3f"
    "7ba13def484e2d3908aff722fad468fb"
    "21696de95d0be911c2d3174f8afcc201"
    "035f7b6d8e69402de5451618c21a535f"
    "a9d7bfc5b8dd9fc243f8cf927db31322"
    "d6e881eaa91a996170e657a05a266426"
    "d98c88003f8477c1227094a0d9fa1e8c"
    "4024309ce1ecccb5210035d47ac72e8a";

const uint8_t encoded_msg_oaep_deci[] =
"37437037213584986740722749024962366529355252601409"
"52482779202749793293002423646866058478090845564178"
"85012245617359476766987254801069139655661322548605"
"01961128828839654354117560526130427034110885012649"
"44426486862198898252067786104770896617275816551903"
"36292006522198349558890134750394873761736356708364"
"75539082";

/* Tampering: Invert LSB bit */
const uint8_t encoded_msg_oaep_lsb[] =
{
    0x35, 0x4f, 0xe6, 0x7b, 0x4a, 0x12, 0x6d, 0x5d, 0x35, 0xfe, 0x36, 0xc7, 0x77, 0x79, 0x1a, 0x3f,
    0x7b, 0xa1, 0x3d, 0xef, 0x48, 0x4e, 0x2d, 0x39, 0x08, 0xaf, 0xf7, 0x22, 0xfa, 0xd4, 0x68, 0xfb,
    0x21, 0x69, 0x6d, 0xe9, 0x5d, 0x0b, 0xe9, 0x11, 0xc2, 0xd3, 0x17, 0x4f, 0x8a, 0xfc, 0xc2, 0x01,
    0x03, 0x5f, 0x7b, 0x6d, 0x8e, 0x69, 0x40, 0x2d, 0xe5, 0x45, 0x16, 0x18, 0xc2, 0x1a, 0x53, 0x5f,
    0xa9, 0xd7, 0xbf, 0xc5, 0xb8, 0xdd, 0x9f, 0xc2, 0x43, 0xf8, 0xcf, 0x92, 0x7d, 0xb3, 0x13, 0x22,
    0xd6, 0xe8, 0x81, 0xea, 0xa9, 0x1a, 0x99, 0x61, 0x70, 0xe6, 0x57, 0xa0, 0x5a, 0x26, 0x64, 0x26,
    0xd9, 0x8c, 0x88, 0x00, 0x3f, 0x84, 0x77, 0xc1, 0x22, 0x70, 0x94, 0xa0, 0xd9, 0xfa, 0x1e, 0x8c,
    0x40, 0x24, 0x30, 0x9c, 0xe1, 0xec, 0xcc, 0xb5, 0x21, 0x00, 0x35, 0xd4, 0x7a, 0xc7, 0x2e, 0x8b
};

#pragma alignvar(4)
const uint8_t encoded_msg_oaep_odd[] =
{
    0xff, /* dummy 1 byte */
    0x35, 0x4f, 0xe6, 0x7b, 0x4a, 0x12, 0x6d, 0x5d, 0x35, 0xfe, 0x36, 0xc7, 0x77, 0x79, 0x1a, 0x3f,
    0x7b, 0xa1, 0x3d, 0xef, 0x48, 0x4e, 0x2d, 0x39, 0x08, 0xaf, 0xf7, 0x22, 0xfa, 0xd4, 0x68, 0xfb,
    0x21, 0x69, 0x6d, 0xe9, 0x5d, 0x0b, 0xe9, 0x11, 0xc2, 0xd3, 0x17, 0x4f, 0x8a, 0xfc, 0xc2, 0x01,
    0x03, 0x5f, 0x7b, 0x6d, 0x8e, 0x69, 0x40, 0x2d, 0xe5, 0x45, 0x16, 0x18, 0xc2, 0x1a, 0x53, 0x5f,
    0xa9, 0xd7, 0xbf, 0xc5, 0xb8, 0xdd, 0x9f, 0xc2, 0x43, 0xf8, 0xcf, 0x92, 0x7d, 0xb3, 0x13, 0x22,
    0xd6, 0xe8, 0x81, 0xea, 0xa9, 0x1a, 0x99, 0x61, 0x70, 0xe6, 0x57, 0xa0, 0x5a, 0x26, 0x64, 0x26,
    0xd9, 0x8c, 0x88, 0x00, 0x3f, 0x84, 0x77, 0xc1, 0x22, 0x70, 0x94, 0xa0, 0xd9, 0xfa, 0x1e, 0x8c,
    0x40, 0x24, 0x30, 0x9c, 0xe1, 0xec, 0xcc, 0xb5, 0x21, 0x00, 0x35, 0xd4, 0x7a, 0xc7, 0x2e, 0x8a
};


/***********************************************************************************/
/* Tentative definition (replace before the test with the actual number of keys) */
#define NB_RSA_PRIV_KEYS       50
#define NB_RSA_PUB_KEYS        70

#include "pe_target_device.h"


/* Argument: Key group (r_key_group_t) */
/* ROM key and RAM key can also be specified */
/* Return value: The maximum number of slots in the specified key group. */
/* Note that it is not an index. */

typedef enum {
    IMPORT_NORMAL,
    IMPORT_DEBUGGER,
    IMPORT_VRF_SKIP = 0x04
} Import_special_t ;

static void busy_loop(void)
{
    int i;
    uint32_t cnt = 0;
    for(i = 0; i < 100000; i++)
    {
        cnt++;
    }
}

/* Key registration function group */
static void prepare_key(r_key_group_t key_group, r_key_index_t key_id, void *modulus, void *priv_key, int key_bits, int specialFlag )
{
    r_icumif_isd_t     *p_ISD         = (r_icumif_isd_t *)ISD_BUFFER;
    ISD_PK_IMPORT_t    *p_ISD_PARAM   = &p_ISD->prm.PK_IMPORT ;
    printf( " key_group = %d, key_id = %d, modulus = (%08x), priv_key = (%08x), bits = %d, flag = %d\n", key_group, key_id, modulus, priv_key, key_bits, specialFlag )

    // initialize the global icum service header
    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);   // clear the ISD_BUFFER

    /* common part */
    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD_PARAM->key_group                = key_group;
    p_ISD_PARAM->key_id                   = key_id;
    p_ISD_PARAM->p_pub_exp                = E10001;
    p_ISD_PARAM->pub_exp_size_in_bits     = 17;
    p_ISD_PARAM->p_modulus                = modulus;
    p_ISD_PARAM->modulus_size_in_bits     = key_bits;
    p_ISD_PARAM->p_priv_exp               = priv_key;
    p_ISD_PARAM->priv_exp_size_in_bits    = key_bits;
    p_ISD_PARAM->key_format               = FORMAT_BINARY_PLAIN;
    p_ISD_PARAM->sig_algo                 = SIGALGO_NONE;
    p_ISD_PARAM->flags.disable_on_debug   = 0;
    p_ISD_PARAM->skip_verify              = 0;

    if( specialFlag & IMPORT_DEBUGGER )
    {
        printf( " disable_on_debug = 1" );
        p_ISD_PARAM->flags.disable_on_debug   = 1;          /* Prohibited when connecting a debugger */
    }
    if( specialFlag & IMPORT_VRF_SKIP )
    {
        printf( " skip_verify = 1" );
        p_ISD_PARAM->skip_verify              = 1;          /* skip verify */
    }

    // trigger the security service
    trigger_service_and_wait(p_ISD);

    if(p_ISD->service_result == SERV_OK)
    {
        printf("\nDone!\n");
    }
    else
    {
        printf( "\nError ! [%s]\n", R_ICUMIF_GetErrorCodeName(p_ISD->service_result) );
    }

}

struct _test_struct {
    int testNo;             /* Test number */
    int AbnormalFlag;           /* 0: Normal system 1: Abnormal system */
    r_icumif_service_id_t       service_id;         /* Service ID */
    char                       *comment;            /* Comment ("Objective" column) */
    ISD_RSA_DECRYPT_t           TestElement;        /* RSA_DECRYPTN structure */
    volatile uint16_t           message_size_in_bytes;
    r_icumif_service_result_t   expected_code;      /* Expected results */
    void                      (*p_setup_func)();    /*  */
} ;


#define PREPARE_KEY(group, id, size, flag) \
    prepare_key( group, id, (void *)modulus_##size, (void *)priv_exp_##size, size, flag )

#define PREPARE_KEY_PKCS(group, id, size, flag) \
    prepare_key( group, id, (void *)modulus_##size##_pkcs, (void *)priv_exp_##size##_pkcs, size, flag )

static void prepare_keyid0_id1_1024(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_KEY( KEY_GRP_RSA, 0, 1024, IMPORT_VRF_SKIP );
    PREPARE_KEY( KEY_GRP_RSA, 1, 1024, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_512(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_KEY( KEY_GRP_RSA, p_TS->TestElement.key_id,  512, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_1024(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_KEY( KEY_GRP_RSA, p_TS->TestElement.key_id, 1024, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_2048(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_KEY( KEY_GRP_RSA, p_TS->TestElement.key_id, 2048, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_3072(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_KEY( KEY_GRP_RSA, p_TS->TestElement.key_id, 3072, IMPORT_VRF_SKIP );
}

static void prepare_keyid0_512_pkcs(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_KEY( KEY_GRP_RSA, p_TS->TestElement.key_id,  512, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_1024_pkcs(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_KEY( KEY_GRP_RSA, p_TS->TestElement.key_id, 1024, IMPORT_VRF_SKIP );
}

static void prepare_keyid0_2048_pkcs(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_KEY_PKCS( KEY_GRP_RSA, p_TS->TestElement.key_id, 2048, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_3072_pkcs(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_KEY( KEY_GRP_RSA, p_TS->TestElement.key_id, 3072, IMPORT_VRF_SKIP );
}

static void prepare_keyid0_1024_dbg(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_KEY( KEY_GRP_RSA, p_TS->TestElement.key_id, 1024, (IMPORT_DEBUGGER | IMPORT_VRF_SKIP) );
}

#define HASH_PRIMITIVE_MAX_OVER  (r_hash_primitive_t)(HASH_PRIMITIVE_SHA2_512+1)
static const struct _test_struct     TestStruct[] = {
{ 1,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"job slice test, p_encrypted_message is hex string",   { (void *) encoded_msg_oaep_hexa,                                   sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,       0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   1,},sizeof(test_message),     SERV_OK,                prepare_keyid0_id1_1024 },/*label_size 0*/
{ 2,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"job slice test, p_encrypted_message is binary",       { (void *) encoded_msg_oaep,                                        sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,       0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  1,},sizeof(test_message),     SERV_OK,                NULL },
{ 3,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"job slice test, p_encrypted_message is decimal,",     { (void *) encoded_msg_oaep_deci,                                   sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,       0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_DECIMAL,1,},sizeof(test_message),     SERV_OK,                NULL }, /*Decimal string*/
{ 4,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"job slice test, p_encrypted_message is hex string",   { (void *) encoded_msg_oaep_hexa,                                   sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,       0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   0,},sizeof(test_message),     SERV_OK,                NULL },
{ 5,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"job slice test, p_encrypted_message is binary",       { (void *) encoded_msg_oaep,                                        sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,       0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_OK,                NULL },
{ 6,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"job slice test, p_encrypted_message is decimal,",     { (void *) encoded_msg_oaep_deci,                                   sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,       0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_DECIMAL,0,},sizeof(test_message),     SERV_OK,                NULL }, /*Decimal string*/
{ 7,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"key_id, max",                                         { (void *) encoded_msg_oaep,                                        sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,       0,   HASH_PRIMITIVE_SHA1,  ENCODING_FORMAT_EME_OAEP, NB_RSA_PRIV_KEYS-1,FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_KEY_SLOT_EMPTY,    NULL },/*Do not become INVALID_KEY_INDEX*/
{ 8,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"key_id, (max+1):invalid",                             { (void *) encoded_msg_oaep,                                        sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,       0,   HASH_PRIMITIVE_SHA1,  ENCODING_FORMAT_EME_OAEP, NB_RSA_PRIV_KEYS,  FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_INVALID_KEY_INDEX, NULL },
{ 9,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid input address, (null) (BINARY)",              { (void *)0,                                                        sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,       0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{10,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(1) (BINARY)",                   { (void *)4,                                                        sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,       0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },/*Fail on DECRYPT*/
{11,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(2) (BINARY)",                   { /*0x009B0000-128*/ (void *) (LOWER_LIMIT_SCF -ENCMSG_LEN),        sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },/*#define ENCMSG_LEN sizeof(encoded_msg_oaep)*/
{12,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid input address, (secure code flash) (BINARY)", { /*0x009B0000-127*/ (void *) (LOWER_LIMIT_SCF -ENCMSG_LEN+1),      sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{13,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, under the boundary (BINARY)",          { /*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),                 sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{14,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(1) (BINARY)",                   { /*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),                sizeof(encoded_msg_oaep) *8, BIG_BUFFER[1],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },/*Fail on DECRYPT*/
{15,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(2) (BINARY)",                   { /*0xFEFFFFFF-127*/ (void *) (UPPER_LIMIT_GRAM -(ENCMSG_LEN-1)),   sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{16,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, beyond the boundary (BINARY)",         { /*0xFEFFFFFF-126*/ (void *) (UPPER_LIMIT_GRAM-(ENCMSG_LEN-1)+1),  sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{17,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, under the boundary (BINARY)",          { /*0xFF200000-1  */ (void *) (LOWER_LIMIT_DF-1),                   sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{18,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(1) (BINARY)",                   { /*0xFF200000    */ (void *) (LOWER_LIMIT_DF),                     sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{19,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(2) (BINARY)",                   { /*0xFF208000-128*/ (void *) (LOWER_LIMIT_SDF-ENCMSG_LEN),         sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{20,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, beyond the boundary (BINARY)",         { /*0xFF208000-127*/ (void *) (LOWER_LIMIT_SDF-ENCMSG_LEN +1),      sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{21,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid input address, (null) (HEXA)",                { (void *)0,                                                        sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },
{22,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(1) (HEXA)",                     { (void *)4,                                                        sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },/*Fail on DECRYPT*/
{23,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(2) (HEXA)",                     { /*0x009B0000-256*/ (void *) (LOWER_LIMIT_SCF -(ENCMSG_LEN_2+1)),  sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },/*#define ENCMSG_LEN_2 (ENCMSG_LEN*2)*/
{24,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid input address, (secure code flash) (HEXA)",   { /*0x009B0000-255*/ (void *) (LOWER_LIMIT_SCF - ENCMSG_LEN_2),     sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{25,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, under the boundary (HEXA)",            { /*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),                 sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{26,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(1) (HEXA)",                     { /*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),                sizeof(encoded_msg_oaep) *8, BIG_BUFFER[1],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },/*Fail on DECRYPT*/
{27,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(2) (HEXA)",                     { /*0xFEFFFFFF-255*/ (void *) (UPPER_LIMIT_GRAM+1-(ENCMSG_LEN_2+1)),sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },
{28,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, beyond the boundary (HEXA)",           { /*0xFEFFFFFF-254*/ (void *) (UPPER_LIMIT_GRAM+1- ENCMSG_LEN_2),   sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{29,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, under the boundary (HEXA)",            { /*0xFF200000-1  */ (void *) (LOWER_LIMIT_DF-1),                   sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{30,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(1) (HEXA)",                     { /*0xFF200000    */ (void *) (LOWER_LIMIT_DF),                     sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },
{31,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(2) (HEXA)",                     { /*0xFF208000-256*/ (void *) (LOWER_LIMIT_SDF-(ENCMSG_LEN_2+1)),   sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },
{32,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, beyond the boundary (HEXA)",           { /*0xFF208000-255*/ (void *) (LOWER_LIMIT_SDF- ENCMSG_LEN_2),      sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{33,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid input address, (null) (DECIMAL)",             { (void *)0,                                                        sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_DECIMAL,0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },
{34,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(1) (DECIMAL)",                  { (void *)4,                                                        sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_DECIMAL,0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },/*Fail on DECRYPT*/
{35,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(2) (DECIMAL)",                  { /*0x009B0000-309*/ (void *) (LOWER_LIMIT_SCF -(ENCMSG_LEN_3+1)),  sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_DECIMAL,0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },/*#define ENCMSG_LEN_3 309*/
{36,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid input address, (secure code flash) (DECIMAL)",{ /*0x009B0000-308*/ (void *) (LOWER_LIMIT_SCF - ENCMSG_LEN_3),     sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_DECIMAL,0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{37,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, under the boundary (DECIMAL)",         { /*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),                 sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_DECIMAL,0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{38,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(1) (DECIMAL)",                  { /*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),                sizeof(encoded_msg_oaep) *8, BIG_BUFFER[1],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_DECIMAL,0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },/*Fail on DECRYPT*/
{39,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, border(2) (DECIMAL)",                  { /*0xFEFFFFFF-308*/ (void *) (UPPER_LIMIT_GRAM+1-(ENCMSG_LEN_3+1)),sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_DECIMAL,0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },
{40,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, beyond the boundary (DECIMAL)",        { /*0xFEFFFFFF-307*/ (void *) (UPPER_LIMIT_GRAM+1- ENCMSG_LEN_3),   sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_DECIMAL,0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{41,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, under the boundary (DECIMAL)",         { /*0xFF200000-1  */ (void *) (LOWER_LIMIT_DF-1),                   sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_DECIMAL,0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{44,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"input address, beyond the boundary (DECIMAL)",        { /*0xFF208000-308*/ (void *) (LOWER_LIMIT_SDF- ENCMSG_LEN_3 ),     sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_DECIMAL,0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{45,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"unaligned input address,",                            { (void *) (&encoded_msg_oaep_odd[1]),                              sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),    seed_oaep,     0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_OK,                NULL }, /* Place same data at odd address*/
{46,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"output address, under the boundary",                  { (void *) encoded_msg_oaep,sizeof(encoded_msg_oaep) *8,/*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),               sizeof(test_message),seed_oaep,   0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_DEST_MEM_ERROR,    NULL },
{47,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"output address, address border(1)",                   { (void *) encoded_msg_oaep,sizeof(encoded_msg_oaep) *8,/*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),              sizeof(test_message),seed_oaep,   0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_OK,                NULL },
{48,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"output address, address border(2)",                   { (void *) encoded_msg_oaep,sizeof(encoded_msg_oaep) *8,/*0xFEFFFFFF-27 */ (void *) (UPPER_LIMIT_GRAM -(TSTMSG_LEN-1)), sizeof(test_message),seed_oaep,   0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_OK,                NULL },/*#define TSTMSG_LEN sizeof(test_message)*/
{49,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"output address, beyond the boundary",                 { (void *) encoded_msg_oaep,sizeof(encoded_msg_oaep) *8,/*0xFEFFFFFF-26 */ (void *) (UPPER_LIMIT_GRAM-(TSTMSG_LEN-1)+1),sizeof(test_message),seed_oaep,   0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_DEST_MEM_ERROR,    NULL },
{50,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid (fake) encrypted massage,",                   { (void *) encoded_msg_oaep_lsb,                           sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },/*Data obtained by inverting the LSB bit*/
{51,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid encrypted_message size (+1),",                { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8+1,     BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{52,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid encrypted_message size (zero),",              { (void *) encoded_msg_oaep,                                    0,                            BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{53,-1,(r_icumif_service_id_t)0,       "invalid encrypted_message size (too large),",         { (void *) encoded_msg_oaep,                               0xf000,                            BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_OK,                NULL },
{54,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"unmatch hash,",                                       { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA2_256,     ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{55,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid hash,",                                       { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_MAX_OVER,     ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },/*#define HASH_PRIMITIVE_MAX_OVER  (r_hash_primitive_t)(HASH_PRIMITIVE_SHA2_512+1)*/
{56,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"unmatch label size,",                                 { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          1,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{57,-1,(r_icumif_service_id_t)0,       "invalid label size (too large),",                     { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{58,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid encode format (none),",                       { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,    (r_message_encoding_format_t)(-1), 1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),SERV_INVALID_ENCODING_FORMAT,NULL },
{59,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid encode format (undefined),",                  { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,    (r_message_encoding_format_t)2,    1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),SERV_INVALID_ENCODING_FORMAT,NULL },
{60,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid encrypted message format (none),",            { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,  (r_number_format_t)(-1),  0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },
{61,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid encrypted message format (undefined),",       { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,  (r_number_format_t)3,     0,},sizeof(test_message),     SERV_PRM_ERROR,         NULL },
{62,-1,(r_icumif_service_id_t)0,       "salt size is 0,",                                     { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_OK,                prepare_keyid0_1024 /*27:key1024 */},
{63,-1,(r_icumif_service_id_t)0,       "salt size is 1,",                                     { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_OK,                prepare_keyid0_1024 /*28:key1024 */},
{64,-1,(r_icumif_service_id_t)0,       "salt size is max,",                                   { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_OK,                prepare_keyid0_1024 /*29:key1024 */},
{65,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"key size 512, SHA1",                                  { (void *) encoded_msg_oaep_512,                           sizeof(encoded_msg_oaep) *8 /2,    BIG_BUFFER[0],   sizeof(test_message), label1_16B, sizeof(label1_16B),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EME_OAEP,     0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message_512), SERV_OK,                prepare_keyid0_512 /*30:key512 */},
{66,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"key size 2048, SHA1",                                 { (void *) encoded_msg_oaep_2048,                          sizeof(encoded_msg_oaep) *8 *2,    BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message_2048),SERV_OK,                prepare_keyid0_2048 /*31:key2048 */},
{67,-1,(r_icumif_service_id_t)0,       "key size 2048, SHA256",                               { (void *) encoded_msg_oaep_2048,                          sizeof(encoded_msg_oaep) *8 *2,    BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA2_256,     ENCODING_FORMAT_EME_OAEP,     0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message_2048),SERV_OK,                prepare_keyid0_2048 /*32:key2048 */},
{68,-1,(r_icumif_service_id_t)0,       "key size 2048, SHA512",                               { (void *) encoded_msg_oaep_2048,                          sizeof(encoded_msg_oaep) *8 *2,    BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA2_512,     ENCODING_FORMAT_EME_OAEP,     0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message_2048),SERV_OK,                prepare_keyid0_2048 /*33:key2048 */},
{69,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"key size 3072, SHA512",                               { (void *) encoded_msg_oaep_3072,                          3072,                              BIG_BUFFER[0],sizeof(test_message_3072),label1_16B,sizeof(label1_16B),HASH_PRIMITIVE_SHA2_512,ENCODING_FORMAT_EME_OAEP,     0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message_3072),SERV_OK,                prepare_keyid0_3072 /*34:key3072 */},
{70,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"uninitialized key,",                                  { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1, ENCODING_FORMAT_EME_OAEP,(NB_RSA_PRIV_KEYS-1),FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_KEY_SLOT_EMPTY,    NULL },
{72,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"unusable key, Debugger",                              { (void *) encoded_msg_oaep,                               sizeof(encoded_msg_oaep) *8,       BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,          0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_KEY_NOT_USABLE,    prepare_keyid0_1024_dbg /*37:key1024 disable_on_debug */},
{73,-1,(r_icumif_service_id_t)0,       "PKCS1_V15, input size is 0",                          { (void *) encoded_msg_pkcs1v15,                               0,                             BIG_BUFFER[0],    0,                    seed_pkcs1v15,      0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_PKCS1_V15,0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_OK,                prepare_keyid0_1024 /*11:key1024 */},
{74,-1,(r_icumif_service_id_t)0,       "PKCS1_V15, input size is 1",                          { (void *) encoded_msg_pkcs1v15,                               1,                             BIG_BUFFER[0],    1,                    seed_pkcs1v15,      0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_PKCS1_V15,0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_OK,                prepare_keyid0_1024 /*12:key1024 */},
{75,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"PKCS1_V15, key size 1024, SHA1",                      { (void *) encoded_msg_pkcs1v15,                           sizeof(encoded_msg_pkcs1v15) *8,   BIG_BUFFER[0],   sizeof(test_message),  seed_pkcs1v15,      0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_PKCS1_V15,0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_OK,                prepare_keyid0_1024_pkcs /*13:key1024 */},
{76,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"PKCS1_V15, key size 512, SHA1",                       { (void *) encoded_msg_pkcs1v15_512,                       sizeof(encoded_msg_pkcs1v15) *8 /2,BIG_BUFFER[0],   sizeof(test_message),  seed_pkcs1v15,      0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_PKCS1_V15,0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message_pkcs1v15_512), SERV_OK,       prepare_keyid0_512_pkcs /*30:key512 */},
{77,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"PKCS1_V15, key size 2048, SHA1",                      { (void *) encoded_msg_pkcs1v15_2048,                      sizeof(encoded_msg_pkcs1v15) *8 *2,BIG_BUFFER[0],   sizeof(test_message),  seed_pkcs1v15,      0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_PKCS1_V15,0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message_pkcs1v15_2048),SERV_OK,       prepare_keyid0_2048_pkcs /*31:key2048 */},
{78,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"PKCS1_V15, key size 2048, SHA256",                    { (void *) encoded_msg_pkcs1v15_2048,                      sizeof(encoded_msg_pkcs1v15) *8 *2,BIG_BUFFER[0],   sizeof(test_message),  seed_pkcs1v15,      0,   HASH_PRIMITIVE_SHA2_256,     ENCODING_FORMAT_EME_PKCS1_V15,0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message_pkcs1v15_2048),SERV_OK,       prepare_keyid0_2048_pkcs /*32:key2048 */},
{79,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"PKCS1_V15, key size 2048, SHA512",                    { (void *) encoded_msg_pkcs1v15_2048,                      sizeof(encoded_msg_pkcs1v15) *8 *2,BIG_BUFFER[0],   sizeof(test_message),  seed_pkcs1v15,      0,   HASH_PRIMITIVE_SHA2_512,     ENCODING_FORMAT_EME_PKCS1_V15,0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message_pkcs1v15_2048),SERV_OK,       prepare_keyid0_2048_pkcs /*33:key2048 */},
{80,0/*Normal system*/,SERVICE_01_RSA_DECRYPT,"PKCS1_V15, key size 3072, SHA1",                      { (void *) encoded_msg_pkcs1v15_3072,                      3072,                              BIG_BUFFER[0],sizeof(test_message_3072),seed_pkcs1v15,      0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_PKCS1_V15,0,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message_pkcs1v15_3072),SERV_OK,       prepare_keyid0_3072_pkcs /*34:key3072 */},

/* Test added: p_label boundary value test */
{100,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"label address, (null) (BINARY)",                      { (void *) encoded_msg_oaep,    sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0], sizeof(test_message), (void *)0,                                             20,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{101,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"label address, border(1) (BINARY)",                   { (void *) encoded_msg_oaep,    sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0], sizeof(test_message), (void *)4,                                             20,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },/*Fail on DECRYPT*/
{102,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"label address, border(2) (BINARY)",                   { (void *) encoded_msg_oaep,    sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0], sizeof(test_message), /*0x009B0000-20*/ (void *) (LOWER_LIMIT_SCF -20),      20,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{103,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"invalid label address, (secure code flash) (BINARY)", { (void *) encoded_msg_oaep,    sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0], sizeof(test_message), /*0x009B0000-19*/ (void *) (LOWER_LIMIT_SCF -20+1),    20,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{104,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"label address, under the boundary (BINARY)",          { (void *) encoded_msg_oaep,    sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0], sizeof(test_message), /*0xFEEF8000-1 */ (void *) (LOWER_LIMIT_GRAM-1),       20,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{105,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"label address, border(1) (BINARY)",                   { (void *) encoded_msg_oaep,    sizeof(encoded_msg_oaep) *8, BIG_BUFFER[1], sizeof(test_message), /*0xFEEF8020   */ (void *) (LOWER_LIMIT_GRAM+48),      20,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },/*Fail on DECRYPT*/
{106,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"label address, border(2) (BINARY)",                   { (void *) encoded_msg_oaep,    sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0], sizeof(test_message), /*0xFEFFFFFF-19*/ (void *) (UPPER_LIMIT_GRAM+1-20),    20,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{107,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"label address, beyond the boundary (BINARY)",         { (void *) encoded_msg_oaep,    sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0], sizeof(test_message), /*0xFEFFFFFF-18*/ (void *) (UPPER_LIMIT_GRAM+1-(20-1)),20,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{108,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"label address, under the boundary (BINARY)",          { (void *) encoded_msg_oaep,    sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0], sizeof(test_message), /*0xFF200000-1 */ (void *) (LOWER_LIMIT_DF-1),         20,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },
{109,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"label address, border(1) (BINARY)",                   { (void *) encoded_msg_oaep,    sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0], sizeof(test_message), /*0xFF200000   */ (void *) (LOWER_LIMIT_DF),           20,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{110,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"label address, border(2) (BINARY)",                   { (void *) encoded_msg_oaep,    sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0], sizeof(test_message), /*0xFF208000-20*/ (void *) (LOWER_LIMIT_SDF -20),      20,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_CRYPT_ERROR,       NULL },
{111,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"label address, beyond the boundary (BINARY)",         { (void *) encoded_msg_oaep,    sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0], sizeof(test_message), /*0xFF208000-19*/ (void *) (LOWER_LIMIT_SDF -20 +1),   20,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_BINARY_PLAIN,  0,},sizeof(test_message),     SERV_SRC_MEM_ERROR,     NULL },

{999,1/*Abnormal system*/,SERVICE_01_RSA_DECRYPT,"Multi Start-up,",                                    { (void *) encoded_msg_oaep_hexa,                                   sizeof(encoded_msg_oaep) *8, BIG_BUFFER[0],   sizeof(test_message),  seed_oaep,       0,   HASH_PRIMITIVE_SHA1,         ENCODING_FORMAT_EME_OAEP,     1,      FORMAT_STRING_HEXA,   1,},sizeof(test_message),     SERV_OK,                prepare_keyid0_id1_1024 },
{-1}
};
static uint8_t *org_message[2][4][3] = {
    // OAEP
    {
        //  512
        {
            (void *)test_message_512, // SHA1
            (void *)NULL,  // SHA256
            (void *)NULL,  // SHA512
        },
        // 1024
        {
            (void *)test_message, // SHA1
            (void *)NULL,  // SHA256
            (void *)NULL,  // SHA512
        },
        // 2048
        {
            (void *)test_message_2048, // SHA1
            (void *)NULL,  // SHA256
            (void *)NULL,  // SHA512
        },
        // 3072
        {
            (void *)NULL,  // SHA1
            (void *)NULL,  // SHA256
            (void *)test_message_3072,  // SHA512
        },
    },
    // PKCS#1 V1.5
    {
        //  512
        {
            (void *)test_message_512,  // SHA1
            (void *)NULL,  // SHA256
            (void *)NULL,  // SHA512
        },
        // 1024
        {
            (void *)test_message, // SHA1
            (void *)NULL,  // SHA256
            (void *)NULL,  // SHA512
        },
        // 2048
        {
            (void *)test_message_pkcs1v15_2048, // SHA1
            (void *)NULL,  // SHA256
            (void *)NULL,  // SHA512
        },
        // 3072
        {
            (void *)test_message_3072,  // SHA1
            (void *)NULL,  // SHA256
            (void *)NULL,  // SHA512
        },
    }
};

static void *get_org_message(struct _test_struct *p_TS)
{
    int i=0,j=0,k=0;

    switch( p_TS->TestElement.message_encoding_format )
    {
    case ENCODING_FORMAT_EME_OAEP :
        i = 0 ;
        switch( p_TS->TestElement.hash_primitive )
        {
        case HASH_PRIMITIVE_SHA1:
            k = 0 ;                break ;
        case HASH_PRIMITIVE_SHA2_256 :
            k = 1 ;                break ;
        case HASH_PRIMITIVE_SHA2_512:
            k = 2 ;                break ;
        }
        break ;
    case ENCODING_FORMAT_EME_PKCS1_V15 :
        i = 1 ;
        k = 0 ;
        break ;
    }

    switch( p_TS->TestElement.encrypted_message_size_in_bits )
    {
    case 512:
        j = 0 ;                break ;
    case 1024 :
        j = 1 ;                break ;
    case 2048:
        j = 2 ;                break ;
    case 3072:
        j = 3 ;                break ;
    }

    return( org_message[i][j][k] );
}

#define _SET_TEST_PARAMETERS(p_ISD) \
    p_ISD->service_id     = p_TS->service_id;\
    p_ISD->p_callbackfunc = 0;\
    p_ISD->job_id         = 0;\
    p_ISD_PARAM->p_encrypted_message            = p_TS->TestElement.p_encrypted_message           ;\
    p_ISD_PARAM->encrypted_message_size_in_bits = p_TS->TestElement.encrypted_message_size_in_bits;\
    p_ISD_PARAM->p_message                      = p_TS->TestElement.p_message                     ;\
    p_ISD_PARAM->message_size_in_bytes          = p_TS->TestElement.message_size_in_bytes         ;\
    p_ISD_PARAM->p_label                        = p_TS->TestElement.p_label                       ;\
    p_ISD_PARAM->label_size_in_bytes            = p_TS->TestElement.label_size_in_bytes           ;\
    p_ISD_PARAM->hash_primitive                 = p_TS->TestElement.hash_primitive                ;\
    p_ISD_PARAM->message_encoding_format        = p_TS->TestElement.message_encoding_format       ;\
    p_ISD_PARAM->key_id                         = p_TS->TestElement.key_id                        ;\
    p_ISD_PARAM->encrypted_message_format       = p_TS->TestElement.encrypted_message_format      ;\
    p_ISD_PARAM->job_slice_mod_exp              = p_TS->TestElement.job_slice_mod_exp             ;

static int _test_by_struct_info(struct _test_struct *p_TS)
{
    r_icumif_isd_t             *p_ISD         = (r_icumif_isd_t *)ISD_BUFFER;
    ISD_RSA_DECRYPT_t          *p_ISD_PARAM   = &p_ISD->prm.RSA_DECRYPT ;
    r_icumif_service_result_t   expected_code = p_TS->expected_code;        /* Expected results */

    int ret = 0 ;

    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);   // clear the ISD_BUFFER

    if( p_TS->service_id == 0 )
    {
        printf( "\n  skip !!\n\n" );
        return(ret);
    }

    /* set general parameter */
    _SET_TEST_PARAMETERS(p_ISD);

    if( p_TS->AbnormalFlag == 0 )
    {
        printf( "(Normal Test)   " );
    }
    else
    {
        printf( "(ABNORMAL Test) " );
    }
    printf( "Expected code = %d(%s)\n\n", expected_code, R_ICUMIF_GetErrorCodeName(expected_code) );

    printf( " rsa key     : group = %d(%s), id = %d\n", 
                    KEY_GRP_RSA, getKeyGroupName(KEY_GRP_RSA), p_ISD_PARAM->key_id );

    printf( " p_cipher    : (%08x) %d bits, (%s)\n", p_ISD_PARAM->p_encrypted_message, p_ISD_PARAM->encrypted_message_size_in_bits,
                                                     getNumberFormatName(p_ISD_PARAM->encrypted_message_format) );
    printf( " p_message   : (%08x) %d bytes\n",      p_ISD_PARAM->p_message,           p_ISD_PARAM->message_size_in_bytes );

    printf( " p_label     : (%08x) %d bytes\n",
                    p_ISD_PARAM->p_label, p_ISD_PARAM->label_size_in_bytes );
    printf( " hash        : primitive %d(%s), enc_format = %d(%s)\n",
                    p_ISD_PARAM->hash_primitive,          getHashPrimitiveName(        p_ISD_PARAM->hash_primitive),
                    p_ISD_PARAM->message_encoding_format, getMessageEncodingFormatName(p_ISD_PARAM->message_encoding_format) );

    printf( " job slice   : _hash = %d, _mod_exp = %d\n",
                    0,  p_ISD_PARAM->job_slice_mod_exp );

    printf( " expected message size : %d bytes\n", (p_TS->message_size_in_bytes) )


    /* trigger the security service */
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    /* fetch execution time in vrun_chrono_xxx variables */
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();

    printf(" Service result : ");
    if(p_ISD->service_result == expected_code)
    {
        printf( "Expected.\n" );

        if(p_ISD->service_result == SERV_OK)
        {
            uint8_t *plain_message ;
            int expected_bytes = p_TS->message_size_in_bytes ;

            plain_message = get_org_message(p_TS) ;

            if( plain_message == NULL ){
                printf(" Message compare : No reffrence.\n" );
            }
            else
            if( p_ISD_PARAM->message_size_in_bytes == expected_bytes )
            {
                if( memcmp( plain_message, p_ISD_PARAM->p_message, p_ISD_PARAM->message_size_in_bytes) == 0 )
                {
                    printf(" Message compare : Expected.\n" );
                }
                else
                {
                    ret = 1 ;
                    printf(" Message compare : NOT Expected.\n" );
                }
            }
            else
            {
                ret = 1 ;
                printf(" Message size  : NOT Expected. %d bytes\n", p_ISD_PARAM->message_size_in_bytes );
            }
        }
    }
    else
    {
        ret = 1;
        if( p_ISD->service_result == SERV_KEY_SLOT_EMPTY ){     /* Unexpected SERV_KEY_SLOT_EMPTY */
            ret = -1;
        }

        printf( "NOT EXPECTED ! [%s]", R_ICUMIF_GetErrorCodeName(p_ISD->service_result) );
        printf(" Message size  : %d bytes\n", p_ISD_PARAM->message_size_in_bytes );
    }

    return(ret) ;
}

static int _multi_startup(struct _test_struct *p_TS)
{
    r_icumif_isd_t     *p_ISD1      = (r_icumif_isd_t *)ISD_BUFFER;
    r_icumif_isd_t     *p_ISD2      = p_ISD1 + 1;
    ISD_RSA_DECRYPT_t  *p_ISD_PARAM = &p_ISD1->prm.RSA_DECRYPT ;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 2));    // clear the ISD_BUFFER

    /* set general parameter */
    _SET_TEST_PARAMETERS(p_ISD1);

    memcpy(p_ISD2, p_ISD1, sizeof(r_icumif_isd_t) );

    /* Test details display */
    printf( " rsa key     : group = %d(%s), id = %d\n", 
                    KEY_GRP_RSA, getKeyGroupName(KEY_GRP_RSA), p_ISD_PARAM->key_id );

    printf( " p_cipher    : (%08x) %d bits, (%s)\n", p_ISD_PARAM->p_encrypted_message, p_ISD_PARAM->encrypted_message_size_in_bits,
                                                     getNumberFormatName(p_ISD_PARAM->encrypted_message_format) );
    printf( " p_message   : (%08x) %d bytes\n",      p_ISD_PARAM->p_message,           p_ISD_PARAM->message_size_in_bytes );

    printf( " p_label     : (%08x) %d bytes\n",
                    p_ISD_PARAM->p_label, p_ISD_PARAM->label_size_in_bytes );
    printf( " hash        : primitive %d(%s), enc_format = %d(%s)\n",
                    p_ISD_PARAM->hash_primitive,          getHashPrimitiveName(        p_ISD_PARAM->hash_primitive),
                    p_ISD_PARAM->message_encoding_format, getMessageEncodingFormatName(p_ISD_PARAM->message_encoding_format) );

    printf( " job slice   : _hash = %d, _mod_exp = %d\n",
                    0,  p_ISD_PARAM->job_slice_mod_exp );

    printf( " expected message size : %d bytes\n", (p_TS->message_size_in_bytes) )


    // trigger the security service
    vrun_chrono_start();
    trigger_2service_and_wait(p_ISD1, p_ISD2);
    vrun_chrono_stop();

    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();

    PCHRONO();
    NL();
    FLUSH();

    printf("ISD1 Result : %s\n",R_ICUMIF_GetErrorCodeName(p_ISD1->service_result));
    printf("ISD2 Result : %s\n",R_ICUMIF_GetErrorCodeName(p_ISD2->service_result));

    printf(" Service result2: ");
    if( p_ISD2->service_result == SERV_OK )
    {
        printf( "Expected.\n" );
        return(0);
    }
    else
    {
        printf( "NOT EXPECTED. \n" );
        return(1);
    }
}

void icum_rsa_dec_cipher_ex_table(int menuNo, struct _test_struct *p_TS_table, int longrun)
{
    int i;
    int cntPass = 0 ;
    int cntFail = 0 ;
    int cntTotal = 0 ;
    int debugger_connect = (R_ICUMIF_GetStatus())->EXT_DEBUGGER ;
    struct _test_struct tmpTS ;

    printf("\n\nMenu No. %d : RSA_DECRYPT ...\n", menuNo);
    printf( "%s::%s()\n", __FILE__, __FUNCTION__ );

    printf("\n Now debugfer is [%s]\n", debugger_connect ? "Connect" : "Not Connect" );
    printf( " NB_RSA_PRIV_KEYS = %d, NB_RSA_PUB_KEYS  = %d\n", 0, 0 );

    /* Test execution loop*/
    for( i = 0; ; i++, p_TS_table++ )
    {
        int ret ;

        if( p_TS_table->testNo <= 0) /* End of table? */
        {
            break ;
        }

        memcpy( &tmpTS, p_TS_table, sizeof(struct _test_struct) );

        printf( "\nTest RSA_DECRYPT No. %2d. %s ...\n\n", tmpTS.testNo, tmpTS.comment );

        /* Long run test */
        if(longrun)
        {
            /* Perform only normal system */
            if( tmpTS.AbnormalFlag != 0 )
            {
                printf( "\n  skip(long run) !!\n\n" );
                continue;
            }
        }

        if( tmpTS.service_id == 0 )
        {
            printf( "\n  skip !!\n\n" );
            continue;
        }

        /* Table adjustment */      /* Replace the temporarily defined "number of keys" with the real thing */
        switch( tmpTS.TestElement.key_id )
        {
        case (NB_RSA_PRIV_KEYS - 1) :   /* priv_key_id maximum value */
            tmpTS.TestElement.key_id  = 0;
            break ;
        case NB_RSA_PRIV_KEYS :         /* priv_key_id maximum value +1 */
            tmpTS.TestElement.key_id  = 0 ;
            break ;

        case (NB_RSA_PUB_KEYS - 1) :    /* pub_key_id maximum value */
            tmpTS.TestElement.key_id  = 0;
            break ;
        case NB_RSA_PUB_KEYS :          /* pub_key_id maximum value +1 */
            tmpTS.TestElement.key_id  = 0 ;
            break ;
        }

        cntTotal ++ ;

        /* Preparation */
        if(tmpTS.p_setup_func != NULL)
            tmpTS.p_setup_func( &tmpTS );

        busy_loop();
        switch( tmpTS.testNo )
        {
        case 999 :
           ret =  _multi_startup( &tmpTS );
           break ;

        default :
            /* (Special handling) test of unavailable key */
            if( tmpTS.expected_code == SERV_KEY_NOT_USABLE )
            {
                if( strstr( tmpTS.comment, "Debugger" ) != NULL )
                {
    
                    if( debugger_connect == 0 )    /* Debugger not connected */
                    {
                        /* Conducted as a normal system */
                        tmpTS.expected_code = SERV_OK ;
                    }
                }
            }
            ret = _test_by_struct_info( &tmpTS ) ;      /* Test body */
            break ;
        }

        switch(ret)
        {
        case 0 :
            cntPass ++ ;
            break ;
        default :
        case -1 :
            cntFail ++ ;
            printf( " Test is in Fail.\n" );
            break ;
        }
    }
    printf("\nMenu No. %d : RSA_DECRYPT ...", menuNo);
    printf( " Test Result : All %d, Pass = %d, Fail = %d\n", cntTotal, cntPass, cntFail );
}

void icum_rsa_dec_cipher_ex(int menuNo)
{
    icum_rsa_dec_cipher_ex_table(menuNo, (struct _test_struct *)TestStruct, 0);
}
