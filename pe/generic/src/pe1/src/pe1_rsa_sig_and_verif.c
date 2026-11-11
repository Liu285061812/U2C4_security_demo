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

#define NEED_PRIV_KEY       /* declare for "pe1_tsa_keys.h", at need for private key */
#include "pe1_rsa_keys.h"


#pragma alignvar(4)
// from "message.c"
#include "pe1_rsa_test_msg_PSS_11.h"

/* "pe1_rsa_sigs.h" : Signature corresponding to test message test_msg_PSS_11 [] */
#include "pe1_rsa_sigs.h"

/* Place test_msg_PSS_11 [] at odd address : test 9 */
#pragma alignvar(4)
static const uint8_t test_msg_PSS_11_odd[] = {
0xff, /* dummy 1 byte */
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
0x1b, 0x73, 0x31, 0x8b, 0x75, 0x0a, 0x01, 0x67, 0xd0};

// ********************************************************************************
// Testing RSA 2048 bit key
// ********************************************************************************

static const uint8_t t2048_msg[]={
    0x88, 0x31, 0x77, 0xe5, 0x12, 0x6b, 0x9b, 0xe2, 0xd9, 0xa9, 0x68, 0x03, 0x27, 0xd5, 0x37, 0x0c, 
    0x6f, 0x26, 0x86, 0x1f, 0x58, 0x20, 0xc4, 0x3d, 0xa6, 0x7a, 0x3a, 0xd6, 0x09};


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


/***********************************************************************************/
// Tentative definition (replace before the test with the actual number of keys)
#define NB_RSA_PRIV_KEYS        50

#include "pe_target_device.h"


// Argument: Key group (r_key_group_t)
// ROM key and RAM key can also be specified
// Return value: The maximum number of slots in the specified key group.
// Note that it is not an index.
typedef enum {
    IMPORT_NORMAL,
    IMPORT_DEBUGGER,
    IMPORT_VRF_SKIP = 0x04
} Import_special_t ;

/* The key registration function group */
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
    p_ISD_PARAM->p_pub_exp                = strE10001;
    p_ISD_PARAM->pub_exp_size_in_bits     = 17;
    p_ISD_PARAM->p_modulus                = modulus;
    p_ISD_PARAM->modulus_size_in_bits     = key_bits;
    p_ISD_PARAM->p_priv_exp               = priv_key;
    p_ISD_PARAM->priv_exp_size_in_bits    = key_bits;
    p_ISD_PARAM->key_format               = FORMAT_STRING_HEXA;
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
    r_icumif_service_id_t       service_id;         /* ServiceID */
    char                       *comment;            /* Comment ("Objective" column) */
    ISD_RSA_SIGN_t              TestElement;        /* RSA_SIGN structure */
    volatile uint16_t           signature_size_in_bits;
    r_icumif_service_result_t   expected_code;      /* Expected results */
    void                      (*p_setup_func)();    /*  */
} ;

static void prepare_keyid0_id1_1024(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key( KEY_GRP_RSA, 0,
                (void *)N_1024,
                (void *)D_1024,  1024, IMPORT_VRF_SKIP );

    prepare_key( KEY_GRP_RSA, 1,
                (void *)N_1024,
                (void *)D_1024, 1024, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_512(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key( KEY_GRP_RSA, p_TS->TestElement.key_id,
                (void *)N_SIG_VERF_512,
                (void *)D_SIG_VERF_512, 512, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_1024(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key( KEY_GRP_RSA, p_TS->TestElement.key_id,
                (void *)N_SIG_VERF_1024,
                (void *)D_SIG_VERF_1024, 1024, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_2048(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key( KEY_GRP_RSA, p_TS->TestElement.key_id,
                (void *)N_SIG_VERF_2048,
                (void *)D_SIG_VERF_2048, 2048, IMPORT_VRF_SKIP );
}
static void prepare_keyid0_3072(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key( KEY_GRP_RSA, p_TS->TestElement.key_id,
                (void *)N_SIG_VERF_3072,
                (void *)D_SIG_VERF_3072, 3072, IMPORT_VRF_SKIP );
}

static void prepare_keyid0_2048_2(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key( KEY_GRP_RSA, p_TS->TestElement.key_id,
                (void *)t2048_key,
                (void *)0,           2048, IMPORT_VRF_SKIP );
                /* Private key, unavailable due to unknown */
}

static void prepare_keyid0_1024_dbg(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key( KEY_GRP_RSA, p_TS->TestElement.key_id,
                (void *)N_SIG_VERF_1024,
                (void *)D_SIG_VERF_1024, 1024, (IMPORT_DEBUGGER | IMPORT_VRF_SKIP) );
}

#define HASH_PRIMITIVE_MAX_OVER  (r_hash_primitive_t)(HASH_PRIMITIVE_SHA2_512+1)
static const struct _test_struct     TestStruct[] = {
    { 1,0/*Normal system*/,SERVICE_01_RSA_SIGN,"job slice test, p_signature is hex string", { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,               32,1,1024,           20,},1024,  SERV_OK,               prepare_keyid0_id1_1024 },
    { 2,0/*Normal system*/,SERVICE_01_RSA_SIGN,"job slice test, p_signature is binary",     { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  1,               32,1,1024,           20,},1024,  SERV_OK,               NULL },
    { 3,0/*Normal system*/,SERVICE_01_RSA_SIGN,"job slice test, p_signature is decimal,",   { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,1,               32,1,1024,           20,},1024,  SERV_OK,               NULL }, /*Decimal string*/
    { 4,0/*Normal system*/,SERVICE_01_RSA_SIGN,"job slice test, p_signature is hex string", { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },
    { 5,0/*Normal system*/,SERVICE_01_RSA_SIGN,"job slice test, p_signature is binary",     { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },
    { 6,0/*Normal system*/,SERVICE_01_RSA_SIGN,"job slice test, p_signature is decimal,",   { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,1,                0,0,1024,           20,},1024,  SERV_OK,               NULL }, /*Decimal string Column*/
    { 7,-1,(r_icumif_service_id_t)0,    "key_group, invalid",                        { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_INVALID_KEY_GROUP,prepare_keyid0_id1_1024 },
    { 8,-1,(r_icumif_service_id_t)0,    "key_group, not invalid",                    { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_KEY_SLOT_EMPTY,   NULL },/*Do not become INVALID_KEY_GROUP*/
    { 9,-1,(r_icumif_service_id_t)0,    "key_group, not invalid",                    { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0,1024,           20,},1024,  SERV_CRYPT_ERROR,      NULL },/*Do not become INVALID_KEY_GROUP*/
    {10,-1,(r_icumif_service_id_t)0,    "key_group, not invalid",                    { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_KEY_SLOT_EMPTY,   NULL },/*Do not become INVALID_KEY_GROUP*/
    {11,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"key_id, max",                               { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,NB_RSA_PRIV_KEYS-1,  0,0,1024,           20,},1024,  SERV_KEY_SLOT_EMPTY,   NULL },/*Because the key is not registered, it was taken as Abnormal system*/ /*Do not become INVALID_KEY_GROUP*/
    {12,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"key_id, (max+1):invalid",                   { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,NB_RSA_PRIV_KEYS,    0,0,1024,           20,},1024,  SERV_INVALID_KEY_INDEX,NULL },
    {13,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"invalid p_message (null),",                 { (void *)0,                                                                   BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_OK         ,      prepare_keyid0_id1_1024 },/*Address 0 is also SERV_OK*/
    {14,0/*Normal system*/,SERVICE_01_RSA_SIGN,"p_message, address border(1)",              { (void *)4,                                                                   BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },
    {15,0/*Normal system*/,SERVICE_01_RSA_SIGN,"p_message, address border(2)",              { /*0x009B0000-217*/ (void *) (LOWER_LIMIT_SCF -sizeof(test_msg_PSS_11)),      BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },
    {16,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"invalid p_message (secure code flash),",    { /*0x009B0000-216*/ (void *) (LOWER_LIMIT_SCF -sizeof(test_msg_PSS_11)+1),    BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_SRC_MEM_ERROR,    NULL },
    {17,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"p_message, under the boundary",             { /*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),                            BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_SRC_MEM_ERROR,    NULL },
    {18,0/*Normal system*/,SERVICE_01_RSA_SIGN,"p_message, address border(1)",              { /*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),                           BIG_BUFFER[1],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },/*If not at address 0, SERV_OK*/
    {19,0/*Normal system*/,SERVICE_01_RSA_SIGN,"p_message, address border(2)",              { /*0xFEFFFFFF-216*/ (void *) (UPPER_LIMIT_GRAM -(sizeof(test_msg_PSS_11)-1)), BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },
    {20,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"p_message, beyond the boundary",            { /*0xFEFFFFFF-215*/ (void *) (UPPER_LIMIT_GRAM-(sizeof(test_msg_PSS_11)-1)+1),BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_SRC_MEM_ERROR,    NULL },
    {21,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"p_message, under the boundary",             { /*0xFF200000-1  */ (void *) (LOWER_LIMIT_DF-1),                              BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_SRC_MEM_ERROR,    NULL },
    {22,0/*Normal system*/,SERVICE_01_RSA_SIGN,"p_message, address border(1)",              { /*0xFF200000    */ (void *) (LOWER_LIMIT_DF),                                BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },/*If not at address 0, SERV_OK*/
    {23,0/*Normal system*/,SERVICE_01_RSA_SIGN,"p_message, address border(2)",              { /*0xFF208000-217*/ (void *) (LOWER_LIMIT_SDF-sizeof(test_msg_PSS_11)),       BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },/*If not at address 0, SERV_OK*/
    {24,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"p_message, beyond the boundary",            { /*0xFF208000-216*/ (void *) (LOWER_LIMIT_SDF-sizeof(test_msg_PSS_11) +1),    BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_SRC_MEM_ERROR,    NULL },
    {25,0/*Normal system*/,SERVICE_01_RSA_SIGN,"unaligned p_message,",                      { (void *) (&test_msg_PSS_11_odd[1]),                                          BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_OK,               NULL }, /* Place same data at odd address*/
    {26,0/*Normal system*/,SERVICE_01_RSA_SIGN,"message size is 0,",                        { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0], 0,                     HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0,1024,           20,},1024,  SERV_OK,               prepare_keyid0_1024 /*11:key1024 */},
    {27,0/*Normal system*/,SERVICE_01_RSA_SIGN,"message size is 1,",                        { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0], 1,                     HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0,1024,           20,},1024,  SERV_OK,               prepare_keyid0_1024 /*12:key1024 */},
    {28,0/*Normal system*/,SERVICE_01_RSA_SIGN,"message size is 16,",                       { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],16,                     HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0,1024,           20,},1024,  SERV_OK,               prepare_keyid0_1024 /*13:key1024 */},
    {29,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"p_signature, under the boundary  (BINARY)", { (void *)test_msg_PSS_11,/*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),                  sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  1,                0,0,1024,           20,},1024,  SERV_DEST_MEM_ERROR,   NULL },
    {30,0/*Normal system*/,SERVICE_01_RSA_SIGN,"p_signature, address border(1)   (BINARY)", { (void *)test_msg_PSS_11,/*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),                 sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },
    {31,0/*Normal system*/,SERVICE_01_RSA_SIGN,"p_signature, address border(2)   (BINARY)", { (void *)test_msg_PSS_11,/*0xFEFFFFFF-127*/ (void *) (UPPER_LIMIT_GRAM -(128-1)),           sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },
    {32,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"p_signature, beyond the boundary (BINARY)", { (void *)test_msg_PSS_11,/*0xFEFFFFFF-126*/ (void *) (UPPER_LIMIT_GRAM-(128-1)+1),          sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  1,                0,0,1024,           20,},1024,  SERV_DEST_MEM_ERROR,   NULL },
    {33,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"p_signature, under the boundary  (HEXA)",   { (void *)test_msg_PSS_11,/*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),                  sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_DEST_MEM_ERROR,   NULL },
    {34,0/*Normal system*/,SERVICE_01_RSA_SIGN,"p_signature, address border(1)   (HEXA)",   { (void *)test_msg_PSS_11,/*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),                 sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },
    /*Tightened DEST check by 1 byte*/
    {35,0/*Normal system*/,SERVICE_01_RSA_SIGN,"p_signature, address border(2)   (HEXA)",   { (void *)test_msg_PSS_11,/*0xFEFFFFFF-256*/ (void *) (UPPER_LIMIT_GRAM+1-(256+1)),          sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },
    {36,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"p_signature, beyond the boundary (HEXA)",   { (void *)test_msg_PSS_11,/*0xFEFFFFFF-255*/ (void *) (UPPER_LIMIT_GRAM+1-(256+1)+1),        sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_DEST_MEM_ERROR,   NULL },
    {37,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"p_signature, under the boundary  (DECIMAL)",{ (void *)test_msg_PSS_11,/*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),                  sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,1,                0,0,1024,           20,},1024,  SERV_DEST_MEM_ERROR,   NULL },
    {38,0/*Normal system*/,SERVICE_01_RSA_SIGN,"p_signature, address border(1)   (DECIMAL)",{ (void *)test_msg_PSS_11,/*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),                 sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },
    /*With DECIMAL, it is not necessary to tighten the check by 1 byte (due to the change of DecimalStrlenFromBitCnt ())*/
        /*In normal system, 1 byte margin is seen considering output area or maximum case */
        /*Test for the largest output area by changing DecimalStrlenFromBitCnt () */
    {39,0/*Normal system*/,SERVICE_01_RSA_SIGN,"p_signature, address border(2)   (DECIMAL)",{ (void *)test_msg_PSS_11,/*0xFEFFFFFF-309*/ (void *) (UPPER_LIMIT_GRAM+1-(309+1)),          sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },/*309 is the number of digits in decimal notation of 1024 bits (MAX)*/
    {40,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"p_signature, beyond the boundary (DECIMAL)",{ (void *)test_msg_PSS_11,/*0xFEFFFFFF-308*/ (void *) (UPPER_LIMIT_GRAM+1-(309+1)+1),        sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,1,                0,0,1024,           20,},1024,  SERV_DEST_MEM_ERROR,   NULL },/*1023xlog(2) = 308,1024xlog(2) = 309*/
    {41,-1,(r_icumif_service_id_t)0,    "invalid (fake) signature,",                 { (void *)test_msg_PSS_11,0/*Data obtained by inverting the LSB bit*/,                                    sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_CRYPT_ERROR,      NULL },
    {42,0/*Normal system*/,SERVICE_01_RSA_SIGN,"invalid signature size (+1),",              { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024 + 1,       20,},1024,  SERV_OK,               NULL },
    {43,0/*Normal system*/,SERVICE_01_RSA_SIGN,"invalid signature size (zero),",            { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,   0,           20,},1024,  SERV_OK,               NULL },
    {44,0/*Normal system*/,SERVICE_01_RSA_SIGN,"invalid signature size (too large),",       { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  1,                0,0,0xF000,         20,},1024,  SERV_OK,               NULL },
    {45,0/*Normal system*/,SERVICE_01_RSA_SIGN,"unmatch hash,",                             { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA2_256,ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_OK,               NULL },
    {46,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"invalid hash,",                             { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_MAX_OVER,ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_PRM_ERROR,        NULL },/*#define HASH_PRIMITIVE_MAX_OVER  (r_hash_primitive_t)(HASH_PRIMITIVE_SHA2_512+1)*/
    {47,0/*Normal system*/,SERVICE_01_RSA_SIGN,"unmatch salt size,",                        { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,           19,},1024,  SERV_OK,               NULL },
    {48,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"invalid salt size (too large),",            { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   1,                0,0,1024,          108,},1024,  SERV_CRYPT_ERROR,      NULL },
    {49,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"invalid encoding format (none),",           { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    (r_hash_encoding_format_t)(-1),FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_INVALID_ENCODING_FORMAT,NULL },
    {50,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"invalid encoding format (undefined),",      { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    (r_hash_encoding_format_t)2,   FORMAT_STRING_HEXA,   1,                0,0,1024,           20,},1024,  SERV_INVALID_ENCODING_FORMAT,NULL },
    {51,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"invalid signature format (none),",          { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,    (r_number_format_t)(-1),1,                0,0,1024,           20,},1024,  SERV_PRM_ERROR,        NULL },
    {52,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"invalid signature format (undefined),",     { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,    (r_number_format_t)3,   1,                0,0,1024,           20,},1024,  SERV_PRM_ERROR,        NULL },
    {53,0/*Normal system*/,SERVICE_01_RSA_SIGN,"salt size is 0,",                           { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0,1024,            0,},1024,  SERV_OK,               prepare_keyid0_1024 /*27:key1024 */},
    {54,0/*Normal system*/,SERVICE_01_RSA_SIGN,"salt size is 1,",                           { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0,1024,            1,},1024,  SERV_OK,               prepare_keyid0_1024 /*28:key1024 */},
    {55,0/*Normal system*/,SERVICE_01_RSA_SIGN,"salt size is max,",                         { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0,1024,/*Maximum value*/106,},1024,  SERV_OK,               prepare_keyid0_1024 /*29:key1024 */},
    {56,0/*Normal system*/,SERVICE_01_RSA_SIGN,"key size 512, SHA1",                        { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0, 512,           20,}, 512,  SERV_OK,               prepare_keyid0_512 /*30:key512 */},
    {57,-1,(r_icumif_service_id_t)0,    "key size 2048, SHA1",                       { (void *)t2048_msg,                                                           BIG_BUFFER[0],sizeof(t2048_msg),      HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0,2048,           20,},2048,  SERV_OK,               prepare_keyid0_2048_2 /*31:key2048 */},
    {58,0/*Normal system*/,SERVICE_01_RSA_SIGN,"key size 2048, SHA1",                       { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0,2048,           20,},2048,  SERV_OK,               prepare_keyid0_2048 /*31:key2048 */},
    {59,0/*Normal system*/,SERVICE_01_RSA_SIGN,"key size 2048, SHA256",                     { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA2_256,ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0,2048,           20,},2048,  SERV_OK,               prepare_keyid0_2048 /*32:key2048 */},
    {60,0/*Normal system*/,SERVICE_01_RSA_SIGN,"key size 2048, SHA512",                     { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA2_512,ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0,2048,           20,},2048,  SERV_OK,               prepare_keyid0_2048 /*33:key2048 */},
    {61,0/*Normal system*/,SERVICE_01_RSA_SIGN,"key size 3072, SHA1",                       { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0,3072,           20,},3072,  SERV_OK,               prepare_keyid0_3072 /*34:key3072 */},
    {62,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"uninitialized key,",                        { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,(NB_RSA_PRIV_KEYS-1),0,0,1024,           20,},1024,  SERV_KEY_SLOT_EMPTY,   NULL },
    {64,1/*Abnormal system*/,SERVICE_01_RSA_SIGN,"unusable key, Debugger",                    { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   0,                0,0,1024,           20,},1024,  SERV_KEY_NOT_USABLE,   prepare_keyid0_1024_dbg /*37:key1024 disable_on_debug */},
    {65,0/*Normal system*/,SERVICE_01_RSA_SIGN,"PKCS1_V15, message size is 0",              { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0], 0,                     HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   0,                0,0,1024,           20,},1024,  SERV_OK,               prepare_keyid0_1024 /*11:key1024 */},
    {66,0/*Normal system*/,SERVICE_01_RSA_SIGN,"PKCS1_V15, message size is 1",              { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0], 1,                     HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   0,                0,0,1024,           20,},1024,  SERV_OK,               prepare_keyid0_1024 /*12:key1024 */},
    {67,0/*Normal system*/,SERVICE_01_RSA_SIGN,"PKCS1_V15, message size is 16",             { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],16,                     HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   0,                0,0,1024,           20,},1024,  SERV_OK,               prepare_keyid0_1024 /*13:key1024 */},
    {68,0/*Normal system*/,SERVICE_01_RSA_SIGN,"PKCS1_V15, key size 512, SHA1",             { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   0,                0,0, 512,           20,}, 512,  SERV_OK,               prepare_keyid0_512 /*30:key512 */},
    {69,0/*Normal system*/,SERVICE_01_RSA_SIGN,"PKCS1_V15, key size 2048, SHA1",            { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   0,                0,0,2048,           20,},2048,  SERV_OK,               prepare_keyid0_2048 /*31:key2048 */},
    {70,0/*Normal system*/,SERVICE_01_RSA_SIGN,"PKCS1_V15, key size 2048, SHA256",          { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA2_256,ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   0,                0,0,2048,           20,},2048,  SERV_OK,               prepare_keyid0_2048 /*32:key2048 */},
    {71,0/*Normal system*/,SERVICE_01_RSA_SIGN,"PKCS1_V15, key size 2048, SHA512",          { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA2_512,ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   0,                0,0,2048,           20,},2048,  SERV_OK,               prepare_keyid0_2048 /*33:key2048 */},
    {72,0/*Normal system*/,SERVICE_01_RSA_SIGN,"PKCS1_V15, key size 3072, SHA1",            { (void *)test_msg_PSS_11,                                                     BIG_BUFFER[0],sizeof(test_msg_PSS_11),HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   0,                0,0,3072,           20,},3072,  SERV_OK,               prepare_keyid0_3072 /*34:key3072 */},
    {-1}
};

#define _SET_TEST_PARAMETERS(p_ISD) \
    p_ISD->service_id     = p_TS->service_id;\
    p_ISD->p_callbackfunc = 0;\
    p_ISD->job_id         = 0;\
    p_ISD_PARAM->p_source               = p_TS->TestElement.p_source              ;\
    p_ISD_PARAM->p_signature             = p_TS->TestElement.p_signature            ;\
    p_ISD_PARAM->source_size_in_bytes   = p_TS->TestElement.source_size_in_bytes  ;\
    p_ISD_PARAM->signature_size_in_bits  = p_TS->TestElement.signature_size_in_bits ;\
    p_ISD_PARAM->salt_size_in_bytes      = p_TS->TestElement.salt_size_in_bytes;    ;\
    p_ISD_PARAM->hash_primitive          = p_TS->TestElement.hash_primitive;        ;\
    p_ISD_PARAM->hash_encoding_format    = p_TS->TestElement.hash_encoding_format;  ;\
    p_ISD_PARAM->signature_format        = p_TS->TestElement.signature_format;      ;\
    p_ISD_PARAM->key_id                  = p_TS->TestElement.key_id;                ;\
    p_ISD_PARAM->job_slice_hash          = p_TS->TestElement.job_slice_hash;        ;\
    p_ISD_PARAM->job_slice_mod_exp       = p_TS->TestElement.job_slice_mod_exp      ;\
    p_ISD_PARAM->is_precalculated_hash   = p_TS->TestElement.is_precalculated_hash  ;

#define RESERVE_BUF &BIG_BUFFER[2][1024]

static int _test_by_struct_info(struct _test_struct *p_TS)
{
    r_icumif_isd_t             *p_ISD         = (r_icumif_isd_t *)ISD_BUFFER;
    ISD_RSA_SIGN_t             *p_ISD_PARAM   = &p_ISD->prm.RSA_SIGN ;
    r_icumif_service_result_t   expected_code = p_TS->expected_code;        /* Expected results */

    int ret = 0 ;

    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);   // clear the ISD_BUFFER

    /* set general parameter */
    _SET_TEST_PARAMETERS(p_ISD);

    /* Save messages */
    memcpy( RESERVE_BUF, p_TS->TestElement.p_source, p_TS->TestElement.source_size_in_bytes );

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

    printf( " p_message   : (%08x) %d bytes\n",      p_ISD_PARAM->p_source,   p_ISD_PARAM->source_size_in_bytes );
    printf( " p_signature : (%08x) %d bits, (%s)\n", p_ISD_PARAM->p_signature, p_ISD_PARAM->signature_size_in_bits,
                                                     getNumberFormatName(p_ISD_PARAM->signature_format) );

    printf( " salt length : %d bytes\n",
                    p_ISD_PARAM->salt_size_in_bytes );
    printf( " hash        : primitive %d(%s), enc_format = %d(%s)\n",
                    p_ISD_PARAM->hash_primitive,       getHashPrimitiveName(     p_ISD_PARAM->hash_primitive),
                    p_ISD_PARAM->hash_encoding_format, getHashEncodingFormatName(p_ISD_PARAM->hash_encoding_format) );

    printf( " job slice   : _hash = %d, _mod_exp = %d\n",
                    p_ISD_PARAM->job_slice_hash,  p_ISD_PARAM->job_slice_mod_exp );

    printf( " expected sign bits : (%d)\n", p_TS->signature_size_in_bits );

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
            printf(" generate sign bits : (%d)\n", p_ISD_PARAM->signature_size_in_bits );

            if(p_ISD_PARAM->signature_size_in_bits != p_TS->signature_size_in_bits)
            {
                /* When the size of the generated signature is different from the expected value */
                printf("  generate sign bits isn't expected (%d)\n", p_TS->signature_size_in_bits );
                ret = 1;    /* Error */
            }
            else
            {
                /* Verify the signature when the size of the generated signature is equal to the expected value */
                ISD_RSA_VERIFY_t             *p_ISD_PARAM   = &p_ISD->prm.RSA_VERIFY ;
                int compare ;

                printf("\nVerification Trial \n");

                compare = memcmp( RESERVE_BUF, p_TS->TestElement.p_source, p_TS->TestElement.source_size_in_bytes );
                if( compare != 0 )
                {
                    /* You can not VERIFY if the original message has changed. */
                    printf( " p_source has changed !! Can't execute VERIFY (MEMORY Error)\n" );
                    return( ret ) ;
                }

                memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);   // clear the ISD_BUFFER

                /* set general parameter */
                p_ISD->service_id     = SERVICE_01_RSA_VERIFY;
                p_ISD->p_callbackfunc = 0;
                p_ISD->job_id         = 0;

                p_ISD_PARAM->p_source               = (const uint8_t *)p_TS->TestElement.p_source              ;
                if( compare != 0 )
                {
                    printf( " p_message has changed !!\n" );
                    p_ISD_PARAM->p_source = RESERVE_BUF ;
                }
                p_ISD_PARAM->p_signature             = p_TS->TestElement.p_signature            ;
                p_ISD_PARAM->source_size_in_bytes   = p_TS->TestElement.source_size_in_bytes  ;
                p_ISD_PARAM->signature_size_in_bits  = p_TS->signature_size_in_bits     ;/* p_TS->TestElement.signature_size_in_bits */
                p_ISD_PARAM->salt_size_in_bytes      = p_TS->TestElement.salt_size_in_bytes;    ;
                p_ISD_PARAM->hash_primitive          = p_TS->TestElement.hash_primitive;        ;
                p_ISD_PARAM->hash_encoding_format    = p_TS->TestElement.hash_encoding_format;  ;
                p_ISD_PARAM->signature_format        = p_TS->TestElement.signature_format;      ;
                p_ISD_PARAM->key_group               = KEY_GRP_RSA                      ;/* p_TS->TestElement.key_group */
                p_ISD_PARAM->key_id                  = p_TS->TestElement.key_id;                ;
                p_ISD_PARAM->job_slice_hash          = p_TS->TestElement.job_slice_hash;        ;
                p_ISD_PARAM->job_slice_mod_exp       = p_TS->TestElement.job_slice_mod_exp      ;
                p_ISD_PARAM->is_precalculated_hash   = p_TS->TestElement.is_precalculated_hash  ;

                /* trigger the security service */
                vrun_chrono_start();
                trigger_service_and_wait(p_ISD);
                vrun_chrono_stop();
                /* fetch execution time in vrun_chrono_xxx variables */
                vrun_chrono_gettimestamp();
                printf("Processed in : "); PCHRONO(); NL(); FLUSH();

                printf(" Verify service : %s\n", R_ICUMIF_GetErrorCodeName(p_ISD->service_result));

                switch( p_ISD->service_result )
                {
                case SERV_OK :
                    printf(" Verify result  : %s\n", getVerifResultName(p_ISD_PARAM->verification_result) );
                    if( p_ISD_PARAM->verification_result == VERIFICATION_PASS )
                    {
                        break ;
                    }
                    ret = 1;
                    break ;
                case SERV_CRYPT_ERROR :
                   ret = 1;
                    break ;
                default :
                    printf(" non excepted case\n" );
                    ret = 1;
                    break ;
                }
            }
        }
    }
    else
    {
        ret = 1;
        if( p_ISD->service_result == SERV_KEY_SLOT_EMPTY ){     /* Unexpected SERV_KEY_SLOT_EMPTY */
            ret = -1;
        }

        if(p_ISD->service_result == SERV_OK)
        {
            printf( "NOT EXPECTED Pass. \n" );
        }
        else
        {
            printf( "NOT EXPECTED ! [%s]\n", R_ICUMIF_GetErrorCodeName(p_ISD->service_result) );
        }
    }

    return(ret) ;
}

static int _multi_startup(struct _test_struct *p_TS)
{
    r_icumif_isd_t     *p_ISD1      = (r_icumif_isd_t *)ISD_BUFFER;
    r_icumif_isd_t     *p_ISD2      = p_ISD1 + 1;
    ISD_RSA_SIGN_t     *p_ISD_PARAM = &p_ISD1->prm.RSA_SIGN ;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 2));    // clear the ISD_BUFFER

    /* set general parameter */
    _SET_TEST_PARAMETERS(p_ISD1);

    memcpy(p_ISD2, p_ISD1, sizeof(r_icumif_isd_t) );

    /* Test details display */
    printf( " rsa key     : group = %d(%s), id = %d\n", 
                    KEY_GRP_RSA, getKeyGroupName(KEY_GRP_RSA), p_ISD_PARAM->key_id );

    printf( " p_message   : (%08x) %d bytes\n",      p_ISD_PARAM->p_source,   p_ISD_PARAM->source_size_in_bytes );
    printf( " p_signature : (%08x) %d bits, (%s)\n", p_ISD_PARAM->p_signature, p_ISD_PARAM->signature_size_in_bits,
                                                     getNumberFormatName(p_ISD_PARAM->signature_format) );

    printf( " salt length : %d bytes\n",
                    p_ISD_PARAM->salt_size_in_bytes );
    printf( " hash        : primitive %d(%s), enc_format = %d(%s)\n",
                    p_ISD_PARAM->hash_primitive,       getHashPrimitiveName(     p_ISD_PARAM->hash_primitive),
                    p_ISD_PARAM->hash_encoding_format, getHashEncodingFormatName(p_ISD_PARAM->hash_encoding_format) );

    printf( " job slice   : _hash = %d, _mod_exp = %d\n",
                    p_ISD_PARAM->job_slice_hash,  p_ISD_PARAM->job_slice_mod_exp );

    printf( " expected sign bits : (%d)\n", p_TS->signature_size_in_bits );


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

extern int icum_rsa_hash_service(const uint8_t *msg, long msgsize, r_hash_primitive_t hash_primitive, uint8_t *make);

void icum_rsa_sign_and_verify_ex_table( int menuNo, struct _test_struct *p_TS_table, int precalculated_hash, int longrun)
{
    int i;
    int cntPass = 0 ;
    int cntFail = 0 ;
    int cntSum = 0 ;
    int debugger_connect = (R_ICUMIF_GetStatus())->EXT_DEBUGGER ;
    struct _test_struct tmpTS;

    printf("\n\nMenu No. %d : RSA_SIGN (& Verify) ...\n", menuNo);
    printf( "%s::%s() %s\n", __FILE__, __FUNCTION__, (precalculated_hash ? "precalculated_hash" : "") );

    printf("\n Now debugfer is [%s]\n", debugger_connect ? "Connect" : "Not Connect" );
    printf( " NB_RSA_PRIV_KEYS  = %d\n", 0 );

    /* Test execution loop */
    for( i = 0; ; i++, p_TS_table++ )
    {
        int ret ;

        if( p_TS_table->testNo <= 0) /* End of table? */
        {
            break ;
        }

        memcpy( &tmpTS, p_TS_table, sizeof(struct _test_struct) );
        tmpTS.TestElement.is_precalculated_hash = precalculated_hash;

        printf( "\nTest RSA_SIGN No. %2d. %s ...%s\n\n", tmpTS.testNo, tmpTS.comment,
                                                           precalculated_hash ? "(precalculated hash)" : "" );

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
        case (NB_RSA_PRIV_KEYS - 1) :    /* pub_key_id Maximum value */
            tmpTS.TestElement.key_id  = 0;
            break ;
        case NB_RSA_PRIV_KEYS :          /* pub_key_id Maximum value +1 */
            tmpTS.TestElement.key_id  = 0 ;
            break ;
        }

        if( precalculated_hash )
        {
            int h_size ;

            /* Use hash value for p_source */
            if( tmpTS.AbnormalFlag != 0 )                   /* It is meaningless in abnormal system */
            {
                printf( "\n  skip abnormal pattern !!\n\n" );
                continue;
            }

            /* find the hash value to use for p_source */
            h_size = icum_rsa_hash_service(tmpTS.TestElement.p_source,
                                           tmpTS.TestElement.source_size_in_bytes,
                                           tmpTS.TestElement.hash_primitive,
                                           BIG_BUFFER[2]);
            if( h_size == 0 ){
                printf( "\n  hash error !!\n\n" );
                continue ;
            }
            /* Use hash value for p_source */
            tmpTS.TestElement.p_source               = BIG_BUFFER[2] ;
            tmpTS.TestElement.source_size_in_bytes   = h_size ;
            if(precalculated_hash != 1){
                tmpTS.TestElement.source_size_in_bytes++;
                tmpTS.expected_code = SERV_PRM_ERROR;
            }
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
    printf("\nMenu No. %d : RSA_SIGN ...", menuNo);
    printf( " Test Result : All %d, Pass = %d, Fail = %d\n", cntSum, cntPass, cntFail );

    icum_rsa_totalCntPass += cntPass ;
    icum_rsa_totalCntFail += cntFail ;
    icum_rsa_totalCntSum  += cntSum  ;
}

/***********************************************************************************/

void icum_rsa_sign_and_verify_ex(int menuNo)
{
    icum_rsa_totalCntPass = 0 ;
    icum_rsa_totalCntFail = 0 ;
    icum_rsa_totalCntSum  = 0 ;
    icum_rsa_sign_and_verify_ex_table(menuNo, (struct _test_struct *)TestStruct, 0, 0);

    /* precalculated_hash : SERV_OK */
    icum_rsa_sign_and_verify_ex_table(menuNo, (struct _test_struct *)TestStruct, 1, 0);
    /* precalculated_hash : SERV_PRM_ERROR */
    icum_rsa_sign_and_verify_ex_table(menuNo, (struct _test_struct *)TestStruct, 2, 0);
    printf( " Total Result : All %d, Pass = %d, Fail = %d\n", icum_rsa_totalCntSum, icum_rsa_totalCntPass, icum_rsa_totalCntFail );
}
