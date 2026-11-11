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
/*#define ENCMSG_LEN_3    308*/ /*Minimum number of digits when 1024 bits are expressed in decimal*/

static const uint8_t label1_16B[] =
{
0x0f,0x0e,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00
};

#pragma alignvar(4)
static const uint8_t test_message_odd[] =
//# Message to be encrypted:
//66 28 19 4e 12 07 3d b0 3b a9 4c da 9e f9 53 23 
//97 d5 0d ba 79 b9 87 00 4a fe fe 34 
{
    0xff, /* dummy 1 byte */
    0x66, 0x28, 0x19, 0x4e, 0x12, 0x07, 0x3d, 0xb0, 0x3b, 0xa9, 0x4c, 0xda, 0x9e, 0xf9, 0x53, 0x23,
    0x97, 0xd5, 0x0d, 0xba, 0x79, 0xb9, 0x87, 0x00, 0x4a, 0xfe, 0xfe, 0x34
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
    if( key_group == KEY_GRP_RSA )
    {
        p_ISD_PARAM->p_priv_exp               = priv_key;
        p_ISD_PARAM->priv_exp_size_in_bits    = key_bits;
    }
    else
    {
        p_ISD_PARAM->p_priv_exp               = priv_key;
        p_ISD_PARAM->priv_exp_size_in_bits    = 0;
    }
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

    printf("\nDone!\n");

}

struct _test_struct {
    int testNo;             /* Test number */
    int AbnormalFlag;           /* 0: Normal system 1: Abnormal system */
    r_icumif_service_id_t       service_id;         /* serviceID */
    char                       *comment;            /* Comment ("Objective" column)*/
    ISD_RSA_ENCRYPT_t           TestElement;        /* RSA_ENCRYPT structure */
    r_icumif_service_result_t   expected_code;      /* Expected results */
    void                      (*p_setup_func)();    /*  */
} ;

#define PREPARE_KEY(group, id, size, flag) \
    prepare_key( group, id, (void *)modulus_##size, (void *)priv_exp_##size, size, flag )

#define PREPARE_2KEY(group, id, size, flag) \
    PREPARE_KEY(group, id, size, flag);     \
    if( group != KEY_GRP_RSA )              \
        PREPARE_KEY(KEY_GRP_RSA, id, size, flag);


#define PREPARE_KEY_PKCS(group, id, size, flag) \
    prepare_key( group, id, (void *)modulus_##size##_pkcs, (void *)priv_exp_##size##_pkcs, size, flag )

/* Also register to KEY_GRP_RSA */
#define PREPARE_2KEY_PKCS(group, id, size, flag) \
    PREPARE_KEY_PKCS(group, id, size, flag);     \
    if( group != KEY_GRP_RSA )              \
        PREPARE_KEY_PKCS(KEY_GRP_RSA, id, size, flag);

static void prepare_keyid0_id1_1024(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_2KEY(p_TS->TestElement.key_group,     0, 1024, IMPORT_VRF_SKIP );
    PREPARE_2KEY(p_TS->TestElement.key_group,     1, 1024, IMPORT_VRF_SKIP );
}

static void prepare_keyid0_512(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_2KEY(p_TS->TestElement.key_group, p_TS->TestElement.key_id,  512, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_1024(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_2KEY(p_TS->TestElement.key_group, p_TS->TestElement.key_id, 1024, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_2048(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_2KEY(p_TS->TestElement.key_group, p_TS->TestElement.key_id, 2048, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_3072(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_2KEY(p_TS->TestElement.key_group, p_TS->TestElement.key_id, 3072, IMPORT_VRF_SKIP );
}

static void prepare_keyid0_512_pkcs(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_2KEY(p_TS->TestElement.key_group, p_TS->TestElement.key_id,  512, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_1024_pkcs(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_2KEY(p_TS->TestElement.key_group, p_TS->TestElement.key_id, 1024, IMPORT_VRF_SKIP );
}

static void prepare_keyid0_2048_pkcs(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_2KEY_PKCS(p_TS->TestElement.key_group, p_TS->TestElement.key_id, 2048, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_3072_pkcs(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    PREPARE_2KEY(p_TS->TestElement.key_group, p_TS->TestElement.key_id, 3072, IMPORT_VRF_SKIP );
}

static void prepare_keyid0_1024_dbg(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

//    /* Registration with KEY_GRP_RSA is unnecessary because it becomes SERV_KEY_NOT_USABLE */
    PREPARE_2KEY(p_TS->TestElement.key_group, p_TS->TestElement.key_id, 1024, (IMPORT_DEBUGGER | IMPORT_VRF_SKIP) );
    /* When the debugger is not connected, SERV_OK is required, so registration with KEY_GRP_RSA is required. */
}

#define HASH_PRIMITIVE_MAX_OVER  (r_hash_primitive_t)(HASH_PRIMITIVE_SHA2_512+1)
static const struct _test_struct     TestStruct[] = {
{ 1,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"job slice test, p_encrypted_message is hex string",        { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_STRING_HEXA,   1,},SERV_OK,                     prepare_keyid0_id1_1024 },/*label_size is 0*/
{ 2,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"job slice test, p_encrypted_message is binary",            { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  1,},SERV_OK,                     NULL },
{ 3,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"job slice test, p_encrypted_message is decimal,",          { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_STRING_DECIMAL,1,},SERV_OK,                     NULL }, /*Decimal string*/
{ 4,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"job slice test, p_encrypted_message is hex string",        { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_STRING_HEXA,   0,},SERV_OK,                     NULL },
{ 5,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"job slice test, p_encrypted_message is binary",            { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{ 6,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"job slice test, p_encrypted_message is decimal,",          { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_STRING_DECIMAL,0,},SERV_OK,                     NULL }, /*Decimal string*/
{ 7,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"key_group, invalid",                                       { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_ECC_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_INVALID_KEY_GROUP,      NULL },
{ 8,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"key_group, not invalid",                                   { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA,        2,            FORMAT_BINARY_PLAIN,  0,},SERV_KEY_SLOT_EMPTY,         NULL },/*Do not become INVALID_KEY_GROUP*/
{ 9,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"key_group, not invalid",                                   { (void *)0,                    sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_ROM_PUB,0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{10,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"key_group, not invalid",                                   { (void *)4,                    sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB_EXT,1,            FORMAT_BINARY_PLAIN,  0,},SERV_KEY_SLOT_EMPTY,         NULL },/*Do not become INVALID_KEY_GROUP*/
{11,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"key_id, max",                                              { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,NB_RSA_PUB_KEYS-1,FORMAT_BINARY_PLAIN,  0,},SERV_KEY_SLOT_EMPTY,         NULL },/*Do not become INVALID_KEY_GROUP*/
{12,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"key_id, (max+1):invalid",                                  { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,NB_RSA_PUB_KEYS,  FORMAT_BINARY_PLAIN,  0,},SERV_INVALID_KEY_INDEX,      NULL },
{13,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"invalid input address, (null) (BINARY)",                   { (void *)0,                    sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{14,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"input address, border(1) (BINARY)",                        { (void *)4,                    sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{15,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"input address, border(2) (BINARY)",                        { /*0x009B0000- 28*/ (void *) (LOWER_LIMIT_SCF -TSTMSG_LEN),      sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },/*#define ENCMSG_LEN sizeof(encoded_msg_oaep)*/
{16,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"invalid input address, (secure code flash) (BINARY)",      { /*0x009B0000- 27*/ (void *) (LOWER_LIMIT_SCF -TSTMSG_LEN+1),    sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_SRC_MEM_ERROR,          NULL },
{17,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"input address, under the boundary (BINARY)",               { /*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),               sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_SRC_MEM_ERROR,          NULL },
{18,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"input address, border(1) (BINARY)",                        { /*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),              sizeof(test_message),       BIG_BUFFER[1],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{19,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"input address, border(2) (BINARY)",                        { /*0xFEFFFFFF- 27*/ (void *) (UPPER_LIMIT_GRAM -(TSTMSG_LEN-1)), sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{20,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"input address, beyond the boundary (BINARY)",              { /*0xFEFFFFFF- 26*/ (void *) (UPPER_LIMIT_GRAM-(TSTMSG_LEN-1)+1),sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_SRC_MEM_ERROR,          NULL },
{21,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"input address, under the boundary (BINARY)",               { /*0xFF200000-1  */ (void *) (LOWER_LIMIT_DF-1),                 sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_SRC_MEM_ERROR,          NULL },
{22,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"input address, border(1) (BINARY)",                        { /*0xFF200000    */ (void *) (LOWER_LIMIT_DF),                   sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{23,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"input address, border(2) (BINARY)",                        { /*0xFF208000- 28*/ (void *) (LOWER_LIMIT_SDF-TSTMSG_LEN),       sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{24,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"input address, beyond the boundary (BINARY)",              { /*0xFF208000- 27*/ (void *) (LOWER_LIMIT_SDF-TSTMSG_LEN +1),    sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_SRC_MEM_ERROR,          NULL },
{25,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"unaligned input address,",                                 { (void *) &test_message_odd[1],                                  sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL }, /* Place same data at odd address*/
{26,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"output address, under the boundary (BINARY)",              { (void *) test_message, sizeof(test_message),/*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),               sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_DEST_MEM_ERROR,         NULL },
{27,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"output address, address border(1) (BINARY)",               { (void *) test_message, sizeof(test_message),/*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),              sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{28,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"output address, address border(2) (BINARY)",               { (void *) test_message, sizeof(test_message),/*0xFEFFFFFF-127*/ (void *) (UPPER_LIMIT_GRAM -(ENCMSG_LEN-1)), sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },/*#define TSTMSG_LEN sizeof(test_message)*/
{29,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"output address, beyond the boundary (BINARY)",             { (void *) test_message, sizeof(test_message),/*0xFEFFFFFF-126*/ (void *) (UPPER_LIMIT_GRAM-(ENCMSG_LEN-1)+1),sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_DEST_MEM_ERROR,         NULL },
{30,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"output address, under the boundary (HEXA)",                { (void *) test_message, sizeof(test_message),/*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),               sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_STRING_HEXA,   0,},SERV_DEST_MEM_ERROR,         NULL },
{31,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"output address, address border(1) (HEXA)",                 { (void *) test_message, sizeof(test_message),/*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),              sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_STRING_HEXA,   0,},SERV_OK,                     NULL },
{32,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"output address, address border(2) (HEXA)",                 { (void *) test_message, sizeof(test_message),/*0xFEFFFFFF-256*/ (void *) (UPPER_LIMIT_GRAM+1-(ENCMSG_LEN_2+1)), sizeof(encoded_msg_oaep) *8,   seed_oaep,0,               HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_STRING_HEXA,   0,},SERV_OK,                     NULL },/*#define TSTMSG_LEN sizeof(test_message)*/
{33,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"output address, beyond the boundary (HEXA)",               { (void *) test_message, sizeof(test_message),/*0xFEFFFFFF-255*/ (void *) (UPPER_LIMIT_GRAM+1-(ENCMSG_LEN_2+1)+1),sizeof(encoded_msg_oaep) *8,  seed_oaep,0,               HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_STRING_HEXA,   0,},SERV_DEST_MEM_ERROR,         NULL },
{34,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"output address, under the boundary (DECIMAL)",             { (void *) test_message, sizeof(test_message),/*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),               sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_STRING_DECIMAL,0,},SERV_DEST_MEM_ERROR,         NULL },
{35,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"output address, address border(1) (DECIMAL)",              { (void *) test_message, sizeof(test_message),/*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),              sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_STRING_DECIMAL,0,},SERV_OK,                     NULL },
{36,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"output address, address border(2) (DECIMAL)",              { (void *) test_message, sizeof(test_message),/*0xFEFFFFFF-309*/ (void *) (UPPER_LIMIT_GRAM+1-(ENCMSG_LEN_3+1)),  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,              HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_STRING_DECIMAL,0,},SERV_OK,                     NULL },/*#define TSTMSG_LEN sizeof(test_message)*/
{37,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"output address, beyond the boundary (DECIMAL)",            { (void *) test_message, sizeof(test_message),/*0xFEFFFFFF-307*/ (void *) (UPPER_LIMIT_GRAM+1-(ENCMSG_LEN_3+1)+1),sizeof(encoded_msg_oaep) *8,   seed_oaep,0,              HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_STRING_DECIMAL,0,},SERV_DEST_MEM_ERROR,         NULL },
{38,-1,(r_icumif_service_id_t)0,"invalid (fake) encrypted massage,",                               { (void *) NULL,                sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_CRYPT_ERROR,            NULL },/*Data obtained by inverting the LSB bit*//*test_message_lsb*/
{39,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"invalid input message size (+1),",                         { (void *) test_message,        sizeof(test_message)+1,                                       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{40,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"invalid input message size (zero),",                       { (void *) test_message,        0,                                                            BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{41,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"invalid input message size (too large),",                  { (void *) test_message,        sizeof(encoded_msg_oaep)*8 +1,                                BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_CRYPT_ERROR,            NULL },
{42,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"unmatch hash,",                                            { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA2_256, ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{43,-1,(r_icumif_service_id_t)0,"invalid hash,",                                                   { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_MAX_OVER, ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_PRM_ERROR,              NULL },/*#define HASH_PRIMITIVE_MAX_OVER  (r_hash_primitive_t)(HASH_PRIMITIVE_SHA2_512+1)*/
{44,-1,(r_icumif_service_id_t)0,"unmatch label size,",                                             { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,1,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_CRYPT_ERROR,            NULL },
{45,-1,(r_icumif_service_id_t)0,"invalid label size (too large),",                                 { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0xf000,             HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_CRYPT_ERROR,            NULL },
{46,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"invalid encode format (none),",                            { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1, (r_message_encoding_format_t)(-1),KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_INVALID_ENCODING_FORMAT,NULL },
{47,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"invalid encode format (undefined),",                       { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1, (r_message_encoding_format_t)2,   KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_INVALID_ENCODING_FORMAT,NULL },
{48,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"invalid encrypted message format (none),",                 { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,        (r_number_format_t)(-1),  0,},SERV_PRM_ERROR,              NULL },
{49,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"invalid encrypted message format (undefined),",            { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,        (r_number_format_t)3,     0,},SERV_PRM_ERROR,              NULL },
{50,-1,(r_icumif_service_id_t)0,"salt size is 0,",                                                 { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_1024 /*27:key1024 */},
{51,-1,(r_icumif_service_id_t)0,"salt size is 1,",                                                 { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_1024 /*28:key1024 */},
{52,-1,(r_icumif_service_id_t)0,"salt size is max,",                                               { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_1024 /*29:key1024 */},
{53,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"key size 512, SHA1",                                       { (void *) test_message_512,    sizeof(test_message_512),                                     BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8/2, label1_16B,sizeof(label1_16B),HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_512 /*30:key512 */},
{54,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"key size 2048, SHA1",                                      { (void *) test_message_2048,   sizeof(test_message_2048),                                    BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8 *2,seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_2048 /*31:key2048 */},
{55,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"key size 2048, SHA256",                                    { (void *) test_message_2048,   sizeof(test_message_2048),                                    BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8 *2,seed_oaep,0,                  HASH_PRIMITIVE_SHA2_256, ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_2048 /*32:key2048 */},/*Because there is no reference, DECRYPT test is not conducted*/
{56,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"key size 2048, SHA512",                                    { (void *) test_message_2048,   sizeof(test_message_2048),                                    BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8 *2,seed_oaep,0,                  HASH_PRIMITIVE_SHA2_512, ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_2048 /*33:key2048 */},/*Because there is no reference, DECRYPT test is not conducted*/
{57,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"key size 3072, SHA512",                                    { (void *) test_message_3072,   sizeof(test_message_3072),                                    BIG_BUFFER[0],  3072,                          label1_16B,sizeof(label1_16B),HASH_PRIMITIVE_SHA2_512, ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_3072 /*34:key3072 */},
{58,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"uninitialized key,",                                       { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,NB_RSA_PUB_KEYS-1,FORMAT_BINARY_PLAIN,  0,},SERV_KEY_SLOT_EMPTY,         NULL },
{60,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"unusable key, Debugger",                                   { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,0,                  HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_KEY_NOT_USABLE,         prepare_keyid0_1024_dbg /*37:key1024 disable_on_debug */},
{61,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"PKCS1_V15, input size is 0",                               { (void *) test_message,     0,                                                               BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,       seed_pkcs1v15,0,          HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_PKCS1_V15,KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_1024 /*11:key1024 */},
{62,-1,(r_icumif_service_id_t)0,"PKCS1_V15, input size is 1",                                      { (void *) test_message,     1,                                                               BIG_BUFFER[0],  1,                                 seed_pkcs1v15,0,          HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_PKCS1_V15,KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_1024 /*12:key1024 */},
{63,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"PKCS1_V15, key size 1024, SHA1",                           { (void *) test_message,        sizeof(test_message),                                         BIG_BUFFER[0],  sizeof(encoded_msg_pkcs1v15) *8,   seed_pkcs1v15,0,          HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_PKCS1_V15,KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_1024_pkcs /*13:key1024 */},
{64,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"PKCS1_V15, key size 512, SHA1",                            { (void *) test_message_512,    sizeof(test_message_512),                                     BIG_BUFFER[0],  sizeof(encoded_msg_pkcs1v15) *8/2, seed_pkcs1v15,0,          HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_PKCS1_V15,KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_512_pkcs /*30:key512 */},
{65,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"PKCS1_V15, key size 2048, SHA1",                           { (void *) test_message_2048,   sizeof(test_message_2048),                                    BIG_BUFFER[0],  sizeof(encoded_msg_pkcs1v15) *8 *2,seed_pkcs1v15,0,          HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_PKCS1_V15,KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_2048_pkcs /*31:key2048 */},
{66,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"PKCS1_V15, key size 2048, SHA256",                         { (void *) test_message_2048,   sizeof(test_message_2048),                                    BIG_BUFFER[0],  sizeof(encoded_msg_pkcs1v15) *8 *2,seed_pkcs1v15,0,          HASH_PRIMITIVE_SHA2_256, ENCODING_FORMAT_EME_PKCS1_V15,KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_2048_pkcs /*32:key2048 */},
{67,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"PKCS1_V15, key size 2048, SHA512",                         { (void *) test_message_2048,   sizeof(test_message_2048),                                    BIG_BUFFER[0],  sizeof(encoded_msg_pkcs1v15) *8 *2,seed_pkcs1v15,0,          HASH_PRIMITIVE_SHA2_512, ENCODING_FORMAT_EME_PKCS1_V15,KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_2048_pkcs /*33:key2048 */},
{68,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"PKCS1_V15, key size 3072, SHA1",                           { (void *) test_message_3072,   sizeof(test_message_3072),                                    BIG_BUFFER[0],  3072,                              seed_pkcs1v15,0,          HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_PKCS1_V15,KEY_GRP_RSA_PUB,    0,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_3072_pkcs /*34:key3072 */},

/* Test added: p_label boundary value test */
{100,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"label address, (null) (BINARY)",                          { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   (void *)0,                                                20,   HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{101,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"label address, border(1) (BINARY)",                       { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   (void *)4,                                                20,   HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{102,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"label address, border(2) (BINARY)",                       { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   /*0x009B0000- 20*/ (void *) (LOWER_LIMIT_SCF -20),        20,   HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{103,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"invalid label address, (secure code flash) (BINARY)",     { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   /*0x009B0000- 19*/ (void *) (LOWER_LIMIT_SCF -20+1),      20,   HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_SRC_MEM_ERROR,          NULL },
{104,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"label address, under the boundary (BINARY)",              { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   /*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),         20,   HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_SRC_MEM_ERROR,          NULL },
{105,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"label address, border(1) (BINARY)",                       { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[1],  sizeof(encoded_msg_oaep) *8,   /*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),        20,   HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{106,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"label address, border(2) (BINARY)",                       { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   /*0xFEFFFFFF- 20*/ (void *) (UPPER_LIMIT_GRAM+1 -20),     20,   HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{107,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"label address, beyond the boundary (BINARY)",             { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   /*0xFEFFFFFF- 19*/ (void *) (UPPER_LIMIT_GRAM+1 -20+1),   20,   HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_SRC_MEM_ERROR,          NULL },
{108,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"label address, under the boundary (BINARY)",              { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   /*0xFF200000-1  */ (void *) (LOWER_LIMIT_DF-1),           20,   HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_SRC_MEM_ERROR,          NULL },
{109,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"label address, border(1) (BINARY)",                       { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   /*0xFF200000    */ (void *) (LOWER_LIMIT_DF),             20,   HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{110,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"label address, border(2) (BINARY)",                       { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   /*0xFF208000- 20*/ (void *) (LOWER_LIMIT_SDF -20),        20,   HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     NULL },
{111,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"label address, beyond the boundary (BINARY)",             { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   /*0xFF208000- 19*/ (void *) (LOWER_LIMIT_SDF -20+1),      20,   HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_SRC_MEM_ERROR,          NULL },

{999,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"Multi Start-up,",                                         { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   seed_oaep,                                                 0,   HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_STRING_HEXA,   1,},SERV_OK,                     prepare_keyid0_id1_1024 },

{-1}
};

static const struct _test_struct     TestStructLongLabel[] = {

{400,0/*Normal system*/,SERVICE_01_RSA_ENCRYPT,"long label size,",                                        { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   (void *)0,                                 LOWER_LIMIT_SCF,    HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_OK,                     prepare_keyid0_id1_1024 },
{401,1/*Abnormal system*/,SERVICE_01_RSA_ENCRYPT,"long label size,",                                        { (void *) test_message,        sizeof(test_message),       BIG_BUFFER[0],  sizeof(encoded_msg_oaep) *8,   (void *)4,                                 0xffffffff,         HASH_PRIMITIVE_SHA1,     ENCODING_FORMAT_EME_OAEP,     KEY_GRP_RSA_PUB,    1,            FORMAT_BINARY_PLAIN,  0,},SERV_SRC_MEM_ERROR,          NULL },
{-1}
};

static int _test_decrypt(struct _test_struct *p_TS, void *dest )
{
    r_icumif_isd_t             *p_ISD         = (r_icumif_isd_t *)ISD_BUFFER;
    ISD_RSA_DECRYPT_t          *p_ISD_PARAM   = &p_ISD->prm.RSA_DECRYPT ;

    int ret = 0 ;

    printf( "test decrypt. " );

    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);   // clear the ISD_BUFFER

    /* set general parameter */
    p_ISD->service_id     = SERVICE_01_RSA_DECRYPT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD_PARAM->p_encrypted_message            = p_TS->TestElement.p_encrypted_message           ;
    p_ISD_PARAM->encrypted_message_size_in_bits = p_TS->TestElement.encrypted_message_size_in_bits;
    p_ISD_PARAM->p_message                      = dest ;
    p_ISD_PARAM->message_size_in_bytes          = p_TS->TestElement.message_size_in_bytes         ;
    p_ISD_PARAM->p_label                        = p_TS->TestElement.p_label                       ;
    p_ISD_PARAM->label_size_in_bytes            = p_TS->TestElement.label_size_in_bytes           ;
    p_ISD_PARAM->hash_primitive                 = p_TS->TestElement.hash_primitive                ;
    p_ISD_PARAM->message_encoding_format        = p_TS->TestElement.message_encoding_format       ;
    p_ISD_PARAM->key_id                         = p_TS->TestElement.key_id                        ;
    p_ISD_PARAM->encrypted_message_format       = p_TS->TestElement.encrypted_message_format      ;
    p_ISD_PARAM->job_slice_mod_exp              = p_TS->TestElement.job_slice_mod_exp             ;

    /* trigger the security service */
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    /* fetch execution time in vrun_chrono_xxx variables */
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();

    if(p_ISD->service_result != SERV_OK)
    {
        ret = -1 ;
    }

    return( ret );
}

#define _SET_TEST_PARAMETERS(p_ISD) \
    p_ISD->service_id     = p_TS->service_id;\
    p_ISD->p_callbackfunc = 0;\
    p_ISD->job_id         = 0;\
    p_ISD_PARAM->p_message                      = p_TS->TestElement.p_message                     ;\
    p_ISD_PARAM->message_size_in_bytes          = p_TS->TestElement.message_size_in_bytes         ;\
    p_ISD_PARAM->p_encrypted_message            = p_TS->TestElement.p_encrypted_message           ;\
    p_ISD_PARAM->encrypted_message_size_in_bits = p_TS->TestElement.encrypted_message_size_in_bits;\
    p_ISD_PARAM->p_label                        = p_TS->TestElement.p_label                       ;\
    p_ISD_PARAM->label_size_in_bytes            = p_TS->TestElement.label_size_in_bytes           ;\
    p_ISD_PARAM->hash_primitive                 = p_TS->TestElement.hash_primitive                ;\
    p_ISD_PARAM->message_encoding_format        = p_TS->TestElement.message_encoding_format       ;\
    p_ISD_PARAM->key_group                      = p_TS->TestElement.key_group                     ;\
    p_ISD_PARAM->key_id                         = p_TS->TestElement.key_id                        ;\
    p_ISD_PARAM->encrypted_message_format       = p_TS->TestElement.encrypted_message_format      ;\
    p_ISD_PARAM->job_slice_mod_exp              = p_TS->TestElement.job_slice_mod_exp             ;

static int _test_by_struct_info(struct _test_struct *p_TS)
{
    r_icumif_isd_t             *p_ISD         = (r_icumif_isd_t *)ISD_BUFFER;
    ISD_RSA_ENCRYPT_t          *p_ISD_PARAM   = &p_ISD->prm.RSA_ENCRYPT ;
    r_icumif_service_result_t   expected_code = p_TS->expected_code;        /* Expected results */

    int ret = 0 ;
    int compare ;

    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);   // clear the ISD_BUFFER

    if( p_TS->service_id == 0 )
    {
        printf( "\n  skip !!\n\n" );
        return(ret);
    }

    /* set general parameter */
    _SET_TEST_PARAMETERS(p_ISD);

    memcpy( BIG_BUFFER[2], p_TS->TestElement.p_message, p_TS->TestElement.message_size_in_bytes );  /* Remember p_message */

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
                    p_ISD_PARAM->key_group, getKeyGroupName(p_ISD_PARAM->key_group), p_ISD_PARAM->key_id );

    printf( " p_message   : (%08x) %d bytes\n",      p_ISD_PARAM->p_message,           p_ISD_PARAM->message_size_in_bytes );
    printf( " p_cipher    : (%08x) %d bits, (%s)\n", p_ISD_PARAM->p_encrypted_message, p_ISD_PARAM->encrypted_message_size_in_bits,
                                                     getNumberFormatName(p_ISD_PARAM->encrypted_message_format) );

    printf( " p_label     : (%08x) %d bytes\n",
                    p_ISD_PARAM->p_label, p_ISD_PARAM->label_size_in_bytes );
    printf( " hash        : primitive %d(%s), enc_format = %d(%s)\n",
                    p_ISD_PARAM->hash_primitive,          getHashPrimitiveName(        p_ISD_PARAM->hash_primitive),
                    p_ISD_PARAM->message_encoding_format, getMessageEncodingFormatName(p_ISD_PARAM->message_encoding_format) );

    printf( " job slice   : _hash = %d, _mod_exp = %d\n",
                    0,  p_ISD_PARAM->job_slice_mod_exp );

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
            uint8_t *plain_message = BIG_BUFFER[2];

            // If successful, try to decrypt
            switch( p_TS->TestElement.key_group )
            {
            case KEY_GRP_RSA_ROM_PUB :   /* The decryption test is not performed because the content of the secret key for KEY_GRP_RSA_ROM_PUB is unknown */
                break ;

            default :
                /* Verify that p_message has not changed */
                compare = memcmp( BIG_BUFFER[2], p_TS->TestElement.p_message, p_TS->TestElement.message_size_in_bytes );
                if( compare != 0 )
                {
                    /* If the original message has changed, there is no point in decoding test. */
                    printf( " p_message has changed !! Can't execute RECRYPT~Compare (MEMORY Error)\n" );
                    return( ret ) ;
                }

                memset(plain_message, 0, sizeof(BIG_BUFFER[2]) );   // clear BIG_BUFFER[2]

                if( _test_decrypt(p_TS, plain_message ) == 0 )
                {
                    printf( " Compare : (%08x),  (%08x), %d bytes.",
                               plain_message, p_TS->TestElement.p_message, p_TS->TestElement.message_size_in_bytes );
                    if( memcmp( plain_message, p_TS->TestElement.p_message, p_TS->TestElement.message_size_in_bytes) == 0 )
                    {
                        printf(" Plain Message compare : Expected.\n" );
                    }
                    else
                    {
                        ret = 1 ;
                        printf(" Plain  compare : NOT Expected.\n" );
                    }
                }
                else
                {
                    ret = 1 ;
                    printf(" Decrypt in fail  : NOT Expected.\n" );
                }
                break ;
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
    ISD_RSA_ENCRYPT_t  *p_ISD_PARAM = &p_ISD1->prm.RSA_ENCRYPT ;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 2));    // clear the ISD_BUFFER

    /* set general parameter */
    _SET_TEST_PARAMETERS(p_ISD1);

    memcpy(p_ISD2, p_ISD1, sizeof(r_icumif_isd_t) );

    /* Test details display */
    printf( " rsa key     : group = %d(%s), id = %d\n", 
                    p_ISD_PARAM->key_group, getKeyGroupName(p_ISD_PARAM->key_group), p_ISD_PARAM->key_id );

    printf( " p_message   : (%08x) %d bytes\n",      p_ISD_PARAM->p_message,           p_ISD_PARAM->message_size_in_bytes );
    printf( " p_cipher    : (%08x) %d bits, (%s)\n", p_ISD_PARAM->p_encrypted_message, p_ISD_PARAM->encrypted_message_size_in_bits,
                                                     getNumberFormatName(p_ISD_PARAM->encrypted_message_format) );

    printf( " p_label     : (%08x) %d bytes\n",
                    p_ISD_PARAM->p_label, p_ISD_PARAM->label_size_in_bytes );
    printf( " hash        : primitive %d(%s), enc_format = %d(%s)\n",
                    p_ISD_PARAM->hash_primitive,          getHashPrimitiveName(        p_ISD_PARAM->hash_primitive),
                    p_ISD_PARAM->message_encoding_format, getMessageEncodingFormatName(p_ISD_PARAM->message_encoding_format) );

    printf( " job slice   : _hash = %d, _mod_exp = %d\n",
                    0,  p_ISD_PARAM->job_slice_mod_exp );

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

extern int icum_rsa_totalCntPass ;
extern int icum_rsa_totalCntFail ;
extern int icum_rsa_totalCntSum ;

void icum_rsa_enc_and_dec_table(int menuNo, struct _test_struct *p_TS_table, int longrun)
{
    int i;
    int cntPass = 0 ;
    int cntFail = 0 ;
    int cntSum = 0 ;
    int debugger_connect = (R_ICUMIF_GetStatus())->EXT_DEBUGGER ;
    struct _test_struct tmpTS ;

    printf("\n\nMenu No. %d : RSA_ENCRYPT ...\n", menuNo);
    printf( "%s::%s()\n", __FILE__, __FUNCTION__ );


    printf("\n Now debugfer is [%s]\n", debugger_connect ? "Connect" : "Not Connect" );
    printf( " NB_RSA_PRIV_KEYS = %d, NB_RSA_PUB_KEYS  = %d\n", 0, 0 );

    /* Test execution loop */
    for( i = 0; ; i++, p_TS_table++ )
    {
        int ret ;

        if( p_TS_table->testNo <= 0) /* End of table? */
        {
            break ;
        }

        memcpy( &tmpTS, p_TS_table, sizeof(struct _test_struct) );

        printf( "\nTest RSA_ENCRYPT No. %2d. %s ...\n\n", tmpTS.testNo, tmpTS.comment );

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
        case (NB_RSA_PRIV_KEYS - 1) :   /* priv_key_id Maximum value */
            tmpTS.TestElement.key_id  = 0;
            break ;
        case NB_RSA_PRIV_KEYS :         /* priv_key_id Maximum value +1 */
            tmpTS.TestElement.key_id  = 0 ;
            break ;

        case (NB_RSA_PUB_KEYS - 1) :    /* pub_key_id Maximum value */
            tmpTS.TestElement.key_id  = 0;
            break ;
        case NB_RSA_PUB_KEYS :          /* pub_key_id Maximum value +1 */
            tmpTS.TestElement.key_id  = 0 ;
            break ;
        }

        cntSum ++ ;

        /* Preparation */
        if(tmpTS.p_setup_func != NULL)
            tmpTS.p_setup_func( &tmpTS );

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
    printf("\nMenu No. %d : RSA_ENCRYPT ...", menuNo);
    printf( " Test Result : All %d, Pass = %d, Fail = %d\n", cntSum, cntPass, cntFail );

    icum_rsa_totalCntPass += cntPass ;
    icum_rsa_totalCntFail += cntFail ;
    icum_rsa_totalCntSum  += cntSum  ;
}

void icum_rsa_enc_and_dec(int menuNo)
{
    icum_rsa_totalCntPass = 0 ;
    icum_rsa_totalCntFail = 0 ;
    icum_rsa_totalCntSum  = 0 ;
    icum_rsa_enc_and_dec_table(menuNo, (struct _test_struct *)TestStructLongLabel, 0);
    icum_rsa_enc_and_dec_table(menuNo, (struct _test_struct *)TestStruct,          0);
    printf( " Total Result : All %d, Pass = %d, Fail = %d\n", icum_rsa_totalCntSum, icum_rsa_totalCntPass, icum_rsa_totalCntFail );
}
