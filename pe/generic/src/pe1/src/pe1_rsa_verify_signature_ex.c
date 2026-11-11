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

#include "OctetStringLib.h"

// ********************************************************************************
// Public constants
// ********************************************************************************

// ********************************************************************************
// Local constants
// ********************************************************************************

// simple test vectors
// source: https://github.com/pyca/cryptography/blob/master/vectors/cryptography_vectors/asymmetric/RSA/pkcs-1v2-1d2-vec/pss-vect.txt

#include "pe1_rsa_keys.h"

#define SIG_LEN     (1024/8)
#define SIG_LEN_2   (SIG_LEN*2)
#define SIG_LEN_3   309/*Number of digits in decimal notation of 1024 bits (MAX)*/

#pragma alignvar(4)
#include "pe1_rsa_test_msg_PSS_11.h"
// from "message.c"

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

    /* test_salt_PSS_11 [] is described only to determine the SALT length.*/
    /* Use the definition SALT_BYTES instead of sizeof (test_salt_PSS_11) */
    #define SALT_BYTES  20

/* Decimal notation of test_sig_PSS_11 [] : test 14 */
static const uint8_t test_sig_PSS_11_dec[] =
"1014389543711978857763427364172527886824314892240508294780766307044970067608885611898798500877360793"
"1712618673745897471655653445982542128128903143532114279014154573753823961329501284393318132243877257"
"4250172388063290732233704739962662802364861752806743649252418028699363941146636478748657134764864739"
"545428796";

/* Invert the LSB of test_sig_PSS_11 []: test 17 */
static const uint8_t test_sig_PSS_11_lsb[] =
    "9074308FB598E9701B2294388E52F971FAAC2B60A5145AF185DF5287B5ED2887"
    "E57CE7FD44DC8634E407C8E0E4360BC226F3EC227F9D9E54638E8D31F5051215"
    "DF6EBB9C2F9579AA77598A38F914B5B9C1BD83C4E2F9F382A0D0AA3542FFEE65"
    "984A601BC69EB28DEB27DCA12C82C2D4C3F66CD500F1FF2B994D8A4E30CBB33D";

// ********************************************************************************
// Testing RSA 2048 bit key
// ********************************************************************************

static const uint8_t t2048_msg[]={
    0x88, 0x31, 0x77, 0xe5, 0x12, 0x6b, 0x9b, 0xe2, 0xd9, 0xa9, 0x68, 0x03, 0x27, 0xd5, 0x37, 0x0c, 
    0x6f, 0x26, 0x86, 0x1f, 0x58, 0x20, 0xc4, 0x3d, 0xa6, 0x7a, 0x3a, 0xd6, 0x09};

static const uint8_t t2048_sig[]=
    "82c2b160093b8aa3c0f7522b19f87354"
    "066c77847abf2a9fce542d0e84e920c5"
    "afb49ffdfdace16560ee94a136960114"
    "8ebad7a0e151cf16331791a5727d05f2"
    "1e74e7eb811440206935d744765a15e7"
    "9f015cb66c532c87a6a05961c8bfad74"
    "1a9a6657022894393e7223739796c02a"
    "77455d0f555b0ec01ddf259b6207fd0f"
    "d57614cef1a5573baaff4ec000699516"
    "59b85f24300a25160ca8522dc6e6727e"
    "57d019d7e63629b8fe5e89e25cc15beb"
    "3a647577559299280b9b28f79b040900"
    "0be25bbd96408ba3b43cc486184dd1c8"
    "e62553fa1af4040f60663de7f5e49c04"
    "388e257f1ce89c95dab48a315d9b66b1"
    "b7628233876ff2385230d070d07e1666";

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

#define    PRESET_RSA_VERIFY_PRM( hash_encoding_form ) \
    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);           \
    p_ISD->service_id     = SERVICE_01_RSA_VERIFY;  \
    p_ISD->p_callbackfunc = 0;                      \
    p_ISD->job_id         = 0;                      \
    job_slice[JOB_H]      = 0;                      \
    job_slice[JOB_RSA]    = 0;                      \
    p_ISD->prm.RSA_VERIFY.key_group               = KEY_GRP_RSA_PUB;                            \
    p_ISD->prm.RSA_VERIFY.key_id                  = 1;                                          \
    p_ISD->prm.RSA_VERIFY.p_source                = (void *)test_msg_PSS_11;                    \
    p_ISD->prm.RSA_VERIFY.source_size_in_bytes    = sizeof(test_msg_PSS_11);                    \
    p_ISD->prm.RSA_VERIFY.p_signature             = (void *)test_sig_PSS_11;                    \
    p_ISD->prm.RSA_VERIFY.signature_size_in_bits  = (strlen((char *)test_sig_PSS_11) / 2) * 8;  \
    p_ISD->prm.RSA_VERIFY.signature_format        = FORMAT_STRING_HEXA;                         \
    p_ISD->prm.RSA_VERIFY.hash_primitive          = HASH_PRIMITIVE_SHA1;                        \
    p_ISD->prm.RSA_VERIFY.hash_encoding_format    = hash_encoding_form;                         \
    p_ISD->prm.RSA_VERIFY.salt_size_in_bytes      = SALT_BYTES;                                 \
    p_ISD->prm.RSA_VERIFY.job_slice_hash          = job_slice[JOB_H];                           \
    p_ISD->prm.RSA_VERIFY.job_slice_mod_exp       = job_slice[JOB_RSA];

#define Exec_Service_Process(p_ISD)     \
    /* trigger the security service */  \
    vrun_chrono_start();                \
    trigger_service_and_wait(p_ISD);    \
    vrun_chrono_stop();                 \
    /* fetch execution time in vrun_chrono_xxx variables */             \
    vrun_chrono_gettimestamp();                                         \
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();                \
    printf("Verification result : ");                                   \
    if(p_ISD->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)  \
    {                                   \
        printf("PASS!\n\n");            \
    }                                   \
    else                                \
    {                                   \
        printf("*FAIL*!\a\n\n");        \
    }

/***************************/
/*  PKCS                   */
/***************************/
#include "rsa_signature_pkcs.h"

/* RSA signature verify (PKCS#1 ver1.5) */

/***********************************************************************************/
// Tentative definition (replace before the test with the actual number of keys)
#define NB_RSA_PUB_KEYS        20

#include "pe_target_device.h"

// Argument: Key group (r_key_group_t)
// ROM key and RAM key can also be specified
// Return value: The maximum number of slots in the specified key group.
// Note that it is not an index.

typedef enum {
    IMPORT_NORMAL,
    IMPORT_DEBUGGER,
    IMPORT_VRF_SKIP = 0x04,
    IMPORT_PUB_3    = 0x08
} Import_special_t ;

static const uint8_t strE0003[] = "03";

/* Key registration function group */
static void prepare_key(r_key_group_t key_group, r_key_index_t key_id, void *keydata, int key_bits, int specialFlag  )
{
    r_icumif_isd_t     *p_ISD         = (r_icumif_isd_t *)ISD_BUFFER;
    ISD_PK_IMPORT_t    *p_ISD_PARAM   = &p_ISD->prm.PK_IMPORT ;

    printf( " key_group = %d, key_id = %d, keydata = (%08x), key_bits = %d, specialFlag = %d\n", key_group, key_id, keydata, key_bits, specialFlag );

    // initialize the global icum service header
    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);    // clear the ISD_BUFFER

    /* common part */
    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD_PARAM->key_group                = key_group;
    p_ISD_PARAM->key_id                   = key_id;
    p_ISD_PARAM->p_pub_exp                = strE10001;
    p_ISD_PARAM->pub_exp_size_in_bits     = 17;
    if( specialFlag & IMPORT_PUB_3 )
    {
        printf( " use pub exponent 3" );
        p_ISD_PARAM->p_pub_exp            = strE0003;
        p_ISD_PARAM->pub_exp_size_in_bits = 8;
    }
    p_ISD_PARAM->p_modulus                = keydata;
    p_ISD_PARAM->modulus_size_in_bits     = key_bits;
    p_ISD_PARAM->priv_exp_size_in_bits    = 0;
    p_ISD_PARAM->key_format               = FORMAT_STRING_HEXA;
    p_ISD_PARAM->sig_algo                 = SIGALGO_NONE;
    p_ISD_PARAM->flags.disable_on_debug   = 0;
    p_ISD_PARAM->skip_verify              = 1;/**/

    if( specialFlag & IMPORT_DEBUGGER )
    {
        printf( " disable_on_debug = 1" );
        p_ISD_PARAM->flags.disable_on_debug   = 1;          /* Prohibited when connecting a debugger */
    }
    if( specialFlag & IMPORT_VRF_SKIP )
    {
        p_ISD_PARAM->skip_verify              = 1;          /* skip verify */
    }

    if( p_ISD_PARAM->skip_verify == 1 )
    {
        printf( " skip_verify = 1" );
    }

    // trigger the security service
    trigger_service_and_wait(p_ISD);

    printf("\nDone!\n");

}

struct _test_struct {
    int testNo;                 /* Test number */
    int AbnormalFlag;           /* 0: Normal system 1: Abnormal system */
    r_icumif_service_id_t       service_id;         /* ServiceID */
    char                       *comment;            /* Comment ("Objective" column) */
    ISD_RSA_VERIFY_t            TestElement;        /* RSA_VERIFY structure */
    r_icumif_service_result_t   expected_code;      /* Expected results */
    void                      (*p_setup_func)();    /* Preparation function */
} ;

static void prepare_keyid0_id1_1024(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key(KEY_GRP_RSA_PUB, 0, (void *)N_1024, 1024, IMPORT_NORMAL );
    prepare_key(KEY_GRP_RSA_PUB, 1, (void *)N_1024, 1024, IMPORT_VRF_SKIP );
}


static void prepare_keyid0_512(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key(p_TS->TestElement.key_group, p_TS->TestElement.key_id,
                (void *)N_SIG_VERF_512, 512, IMPORT_NORMAL );
}
static void prepare_keyid0_1024(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key(p_TS->TestElement.key_group, p_TS->TestElement.key_id,
                (void *)N_SIG_VERF_1024, 1024, IMPORT_NORMAL );
}
static void prepare_keyid0_2048(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key(p_TS->TestElement.key_group, p_TS->TestElement.key_id,
                (void *)N_SIG_VERF_2048, 2048, IMPORT_NORMAL );
}
static void prepare_keyid0_3072(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key(p_TS->TestElement.key_group, p_TS->TestElement.key_id,
                (void *)N_SIG_VERF_3072, 3072, IMPORT_NORMAL );
}
void prepare_keyid0_3072_pub3(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key(p_TS->TestElement.key_group, p_TS->TestElement.key_id,
                (void *)N_SIG_VERF_3072, 3072, IMPORT_NORMAL | IMPORT_PUB_3 );
}

static void prepare_keyid0_2048_2(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key(p_TS->TestElement.key_group, p_TS->TestElement.key_id,
                (void *)t2048_key, 2048, IMPORT_NORMAL );
}

static void prepare_keyid0_1024_dbg(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key(p_TS->TestElement.key_group, p_TS->TestElement.key_id,
                (void *)N_SIG_VERF_1024, 1024, IMPORT_DEBUGGER );
}

int icum_rsa_totalCntPass ;
int icum_rsa_totalCntFail ;
int icum_rsa_totalCntSum ;

int icum_rsa_hash_service(const uint8_t *msg, long msgsize, r_hash_primitive_t hash_primitive, uint8_t *make)
{
    r_icumif_isd_t             *p_ISD         = (r_icumif_isd_t *)ISD_BUFFER;
    ISD_HASH_t                 *p_ISD_PARAM   = &p_ISD->prm.HASH ;
    int h_size ;

    switch(hash_primitive){
    case HASH_PRIMITIVE_SHA1 :          h_size = 20;    break ;
    case HASH_PRIMITIVE_SHA2_224 :      h_size = 28;    break ;
    case HASH_PRIMITIVE_SHA2_256 :      h_size = 32;    break ;
    case HASH_PRIMITIVE_SHA2_384 :      h_size = 48;    break ;
    case HASH_PRIMITIVE_SHA2_512 :      h_size = 64;    break ;
    case HASH_PRIMITIVE_MD5 :           h_size = 16;    break ;
    case HASH_PRIMITIVE_RIPEMD_128 :    h_size = 16;    break ;
    case HASH_PRIMITIVE_RIPEMD_160 :    h_size = 20;    break ;
    case HASH_PRIMITIVE_RIPEMD_256 :    h_size = 32;    break ;
    case HASH_PRIMITIVE_RIPEMD_320 :    h_size = 40;    break ;
    default :                                           return(0);
    }

    p_ISD->service_id                  = SERVICE_01_HASH;
    p_ISD->p_callbackfunc              = 0;
    p_ISD->job_id                      = 0;
    p_ISD_PARAM->p_message             = msg;
    p_ISD_PARAM->message_size_in_bytes = msgsize;
    p_ISD_PARAM->hash_primitive        = hash_primitive;
    p_ISD_PARAM->job_slice             = 0;
    p_ISD_PARAM->job_cycle             = JOB_ALL_AT_ONCE;
    p_ISD_PARAM->p_hash                = make;

    printf("start hash...\n");

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();

    printf("done\n");

    if(p_ISD->service_result != SERV_OK)
        return(0);

    return(h_size);
}

#define HASH_PRIMITIVE_MAX_OVER  (r_hash_primitive_t)(HASH_PRIMITIVE_SHA2_512+1)
static const struct _test_struct     TestStruct[] = {
{ 1,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"job slice test, p_signature is hex string",                { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,              32,1,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_id1_1024 },
{ 2,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"job slice test, p_signature is binary",                    { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11_plain,         sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,              32,1,0,VERIFICATION_PASS },SERV_OK,               NULL },
{ 3,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"job slice test, p_signature is decimal,",                  { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11_dec,           sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,              32,1,0,VERIFICATION_PASS },SERV_OK,               NULL },
{ 4,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"job slice test, p_signature is hex string",                { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_PASS },SERV_OK,               NULL },
{ 5,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"job slice test, p_signature is binary",                    { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11_plain,         sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_PASS },SERV_OK,               NULL },
{ 6,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"job slice test, p_signature is decimal,",                  { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11_dec ,          sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_PASS },SERV_OK,               NULL },
{ 7,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"key_group, invalid",                                       { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_ECC_PUB,    1,               0,0,0,                  },SERV_INVALID_KEY_GROUP,prepare_keyid0_id1_1024 },
{ 8,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"key_group, not invalid",                                   { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA,        2,               0,0,0,                  },SERV_KEY_SLOT_EMPTY,   NULL },/*Do not become INVALID_KEY_GROUP*/
{ 9,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"key_group, not invalid",                                   { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_ROM_PUB,0,               0,0,0,                  },SERV_CRYPT_ERROR,      NULL },/*Do not become INVALID_KEY_GROUP*/
{10,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"key_group, not invalid",                                   { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB_EXT,1,               0,0,0,                  },SERV_KEY_SLOT_EMPTY,   NULL },/*Do not become INVALID_KEY_GROUP*/
{11,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"key_id, max",                                              { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,  NB_RSA_PUB_KEYS-1, 0,0,0,                  },SERV_KEY_SLOT_EMPTY,   NULL },/*Do not become INVALID_KEY_GROUP*/
{12,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"key_id, (max+1):invalid",                                  { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,  NB_RSA_PUB_KEYS,   0,0,0,                  },SERV_INVALID_KEY_INDEX,NULL },
{13,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid p_message (null),",                                { (void *)0,                                                (void *)test_sig_PSS_11,sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_OK,               NULL },/*Address 0 is also SERV_OK*/
{14,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_message, address border(1)",                             { (void *)4,                                                (void *)test_sig_PSS_11,sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_OK,               NULL },/*If not at address 0, SERV_OK*/
{15,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_message, address border(2)",          /*0x009B0000-217*/ { (void *) (LOWER_LIMIT_SCF -sizeof(test_msg_PSS_11)),      (void *)test_sig_PSS_11,sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_OK,               NULL },
{16,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid p_message (secure code flash),",/*0x009B0000-216*/ { (void *) (LOWER_LIMIT_SCF -sizeof(test_msg_PSS_11)+1),    (void *)test_sig_PSS_11,sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{17,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_message, under the boundary",         /*0xFEEF8000-1  */ { (void *) (LOWER_LIMIT_GRAM-1),                            (void *)test_sig_PSS_11,sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{18,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_message, address border(1)",          /*0xFEEF8020    */ { (void *) (LOWER_LIMIT_GRAM+48),                           (void *)test_sig_PSS_11,sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_OK,               NULL },/*If not at address 0, SERV_OK*/
{19,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_message, address border(2)",          /*0xFEFFFFFF-216*/ { (void *) (UPPER_LIMIT_GRAM -(sizeof(test_msg_PSS_11)-1)), (void *)test_sig_PSS_11,sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_OK,               NULL },
{20,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_message, beyond the boundary",        /*0xFEFFFFFF-215*/ { (void *) (UPPER_LIMIT_GRAM-(sizeof(test_msg_PSS_11)-1)+1),(void *)test_sig_PSS_11,sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{21,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_message, under the boundary",         /*0xFF200000-1  */ { (void *) (LOWER_LIMIT_DF-1),                              (void *)test_sig_PSS_11,sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{22,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_message, address border(1)",          /*0xFF200000    */ { (void *) (LOWER_LIMIT_DF),                                (void *)test_sig_PSS_11,sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_OK,               NULL },/*If not at address 0, SERV_OK*/
{23,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_message, address border(2)",          /*0xFF208000-217*/ { (void *) (LOWER_LIMIT_SDF-sizeof(test_msg_PSS_11)),       (void *)test_sig_PSS_11,sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_OK,               NULL },
{24,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_message, beyond the boundary",        /*0xFF208000-216*/ { (void *) (LOWER_LIMIT_SDF-sizeof(test_msg_PSS_11) +1),    (void *)test_sig_PSS_11,sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{25,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"unaligned p_message,",                                     { (void *) (&test_msg_PSS_11_odd[1]),                       (void *)test_sig_PSS_11,sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_PASS },SERV_OK,               NULL }, /* Place same data at odd address*/
{26,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"message size is 0,",                                       { (void *)test_msg_PSS_11,                   (void *) sig_key1024_sha1_msg0,         0,                     1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_1024 /*11:key1024 */},
{27,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"message size is 1,",                                       { (void *)test_msg_PSS_11,                   (void *) sig_key1024_sha1_msg1,         1,                     1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_1024 /*12:key1024 */},
{28,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"message size is 16,",                                      { (void *)test_msg_PSS_11,                   (void *) sig_key1024_sha1_msg16,       16,                     1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_1024 /*13:key1024 */},
{29,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid p_signature (null), (BINARY)",                     { (void *)test_msg_PSS_11,                   (void *)0,                             sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_CRYPT_ERROR,      NULL },
{30,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(1) (BINARY)",                  { (void *)test_msg_PSS_11,                   (void *)4,                             sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_CRYPT_ERROR,      NULL },
{31,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(2) (BINARY)",                  { (void *)test_msg_PSS_11,/*0x009B0000-128*/ (void *) (LOWER_LIMIT_SCF -SIG_LEN),   sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_CRYPT_ERROR,      NULL },
{32,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, beyond the boundary (BINARY)",                { (void *)test_msg_PSS_11,/*0x009B0000-127*/ (void *) (LOWER_LIMIT_SCF -SIG_LEN)+1, sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{33,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, under the boundary (BINARY)",                 { (void *)test_msg_PSS_11,/*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),         sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{34,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(1) (BINARY)",                  { (void *)test_msg_PSS_11,/*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),        sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_CRYPT_ERROR,      NULL },
{35,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(2) (BINARY)",                  { (void *)test_msg_PSS_11,/*0xFEFFFFFF-127*/ (void *)(UPPER_LIMIT_GRAM+1 -SIG_LEN),   sizeof(test_msg_PSS_11),1024,                              20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_CRYPT_ERROR,      NULL },
{36,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, beyond the boundary (BINARY)",                { (void *)test_msg_PSS_11,/*0xFEFFFFFF-126*/ (void *)(UPPER_LIMIT_GRAM+1 -SIG_LEN+1), sizeof(test_msg_PSS_11),1024,                              20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{37,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, under the boundary (BINARY)",                 { (void *)test_msg_PSS_11,/*0xFF200000-1  */ (void *) (LOWER_LIMIT_DF-1),           sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{38,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(1) (BINARY)",                  { (void *)test_msg_PSS_11,/*0xFF200000    */ (void *) (LOWER_LIMIT_DF),             sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_CRYPT_ERROR,      NULL },
{39,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(2) (BINARY)",                  { (void *)test_msg_PSS_11,/*0xFF208000-128*/ (void *) (LOWER_LIMIT_SDF-SIG_LEN),    sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_CRYPT_ERROR,      NULL },
{40,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, beyond the boundary (BINARY)",                { (void *)test_msg_PSS_11,/*0xFF208000-127*/ (void *) (LOWER_LIMIT_SDF-SIG_LEN +1), sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{41,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid p_signature (null), (HEXA)",                       { (void *)test_msg_PSS_11,                   (void *)0,                             sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_PRM_ERROR,        NULL },
{42,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(1) (HEXA)",                    { (void *)test_msg_PSS_11,                   (void *)4,                             sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_PRM_ERROR,        NULL },
{43,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(2) (HEXA)",                    { (void *)test_msg_PSS_11,/*0x009B0000-256*/ (void *)(LOWER_LIMIT_SCF -(SIG_LEN_2+1)),sizeof(test_msg_PSS_11),1024,                              20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_PRM_ERROR,        NULL },
{44,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, beyond the boundary (HEXA)",                  { (void *)test_msg_PSS_11,/*0x009B0000-255*/ (void *)(LOWER_LIMIT_SCF -SIG_LEN_2),  sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{45,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, under the boundary (HEXA)",                   { (void *)test_msg_PSS_11,/*0xFEEF8000-1  */ (void *)(LOWER_LIMIT_GRAM- SIG_LEN_2), sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{46,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(1) (HEXA)",                    { (void *)test_msg_PSS_11,/*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48),        sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_PRM_ERROR,        NULL },
{47,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(2) (HEXA)",                    { (void *)test_msg_PSS_11,/*0xFEFFFFFF-255*/ (void *)(UPPER_LIMIT_GRAM+1-(SIG_LEN_2+1)),sizeof(test_msg_PSS_11),1024,                            20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_PRM_ERROR,        NULL },
{48,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, beyond the boundary (HEXA)",                  { (void *)test_msg_PSS_11,/*0xFEFFFFFF-254*/ (void *)(UPPER_LIMIT_GRAM+1- SIG_LEN_2), sizeof(test_msg_PSS_11),1024,                              20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{49,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, under the boundary (HEXA)",                   { (void *)test_msg_PSS_11,/*0xFF200000-1  */ (void *) (LOWER_LIMIT_DF-1),           sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{50,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(1) (HEXA)",                    { (void *)test_msg_PSS_11,/*0xFF200000    */ (void *) (LOWER_LIMIT_DF),             sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_PRM_ERROR,        NULL },
{51,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(2) (HEXA)",                    { (void *)test_msg_PSS_11,/*0xFF208000-256*/ (void *)(LOWER_LIMIT_SDF-(SIG_LEN_2+1)),sizeof(test_msg_PSS_11),1024,                               20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_PRM_ERROR,        NULL },
{52,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, beyond the boundary (HEXA)",                  { (void *)test_msg_PSS_11,/*0xFF208000-255*/ (void *)(LOWER_LIMIT_SDF- SIG_LEN_2),  sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{53,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid p_signature (null), (DECIMAL)",                    { (void *)test_msg_PSS_11,                   (void *)0,                             sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_PRM_ERROR,        NULL },
{54,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(1) (DECIMAL)",                 { (void *)test_msg_PSS_11,                   (void *)4,                             sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_PRM_ERROR,        NULL },
{55,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(2) (DECIMAL)",                 { (void *)test_msg_PSS_11,/*0x009B0000-309*/ (void *)(LOWER_LIMIT_SCF-(SIG_LEN_3+1)),sizeof(test_msg_PSS_11),1024,                               20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_PRM_ERROR,        NULL },
{56,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, beyond the boundary (DECIMAL)",               { (void *)test_msg_PSS_11,/*0x009B0000-308*/ (void *)(LOWER_LIMIT_SCF -SIG_LEN_3),  sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{57,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, under the boundary (DECIMAL)",                { (void *)test_msg_PSS_11,/*0xFEEF8000-1  */ (void *) (LOWER_LIMIT_GRAM-1),         sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
/* No.58 Since the signature is generated at this position in the Sign / Verify test, this test is shifted by 1 byte. */
{58,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(1) (DECIMAL)",                 { (void *)test_msg_PSS_11,/*0xFEEF8020    */ (void *) (LOWER_LIMIT_GRAM+48+1),      sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_PRM_ERROR,        NULL },
{59,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(2) (DECIMAL)",                 { (void *)test_msg_PSS_11,/*0xFEFFFFFF-308*/ (void *)(UPPER_LIMIT_GRAM+1-(SIG_LEN_3+1)),sizeof(test_msg_PSS_11),1024,                            20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_PRM_ERROR,        NULL },
{60,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, beyond the boundary (DECIMAL)",               { (void *)test_msg_PSS_11,/*0xFEFFFFFF-306*/ (void *)(UPPER_LIMIT_GRAM+1 -SIG_LEN_3),sizeof(test_msg_PSS_11),1024,                               20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{61,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, under the boundary (DECIMAL)",                { (void *)test_msg_PSS_11,/*0xFF200000-1  */ (void *) (LOWER_LIMIT_DF-1),           sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{62,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(1) (DECIMAL)",                 { (void *)test_msg_PSS_11,/*0xFF200000    */ (void *) (LOWER_LIMIT_DF),             sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_PRM_ERROR,        NULL },
{63,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, address border(2) (DECIMAL)",                 { (void *)test_msg_PSS_11,/*0xFF208000-309*/ (void *)(LOWER_LIMIT_SDF-(SIG_LEN_3+1)),sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,               0,0,0,VERIFICATION_FAIL },SERV_PRM_ERROR,        NULL },
{64,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"p_signature, beyond the boundary (DECIMAL)",               { (void *)test_msg_PSS_11,/*0xFF208000-308*/ (void *)(LOWER_LIMIT_SDF -SIG_LEN_3),  sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_DECIMAL,KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_SRC_MEM_ERROR,    NULL },
{65,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid (fake) signature,",                                { (void *)test_msg_PSS_11,/*The data which reversed LSB*/(void *)test_sig_PSS_11_lsb,       sizeof(test_msg_PSS_11),1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_CRYPT_ERROR,      NULL },
{66,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid signature size (+1),",                             { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8 +1,20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_PRM_ERROR,        NULL },
{67,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid signature size (zero),",                           { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),   0,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_PRM_ERROR,        NULL },
{68,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid signature size (too large),",                      { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11_plain,         sizeof(test_msg_PSS_11),0xF000,                              20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_PRM_ERROR,        NULL },
{69,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"unmatch hash,",                                            { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA2_256,ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_CRYPT_ERROR,      NULL },
{70,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid hash,",                                            { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_MAX_OVER,ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_PRM_ERROR,        NULL },/*#define HASH_PRIMITIVE_MAX_OVER  (r_hash_primitive_t)(HASH_PRIMITIVE_SHA2_512+1)*/
{71,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"unmatch salt size,",                                       { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   19,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_CRYPT_ERROR,      NULL },
{72,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid salt size (too large),",                           { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,  108,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_CRYPT_ERROR,      NULL },
{73,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid encoding format (none),",                          { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,   (r_hash_encoding_format_t)(-1), FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_INVALID_ENCODING_FORMAT,NULL },
{74,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid encoding format (undefined),",                     { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,   (r_hash_encoding_format_t)2,    FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_INVALID_ENCODING_FORMAT,NULL },
{75,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid signature format (none),",                         { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,    (r_number_format_t)(-1),KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_PRM_ERROR,        NULL },
{76,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"invalid signature format (undefined),",                    { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,    (r_number_format_t)3,   KEY_GRP_RSA_PUB,    1,               0,0,0,                  },SERV_PRM_ERROR,        NULL },
{77,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"salt size is 0,",                                          { (void *)test_msg_PSS_11,                   (void *) sig_key1024_sha1_salt0,       sizeof(test_msg_PSS_11),1024,                                 0,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_1024 /*27:key1024 */},
{78,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"salt size is 1,",                                          { (void *)test_msg_PSS_11,                   (void *) sig_key1024_sha1_salt1,       sizeof(test_msg_PSS_11),1024,                                 1,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_1024 /*28:key1024 */},
{79,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"salt size is max,",                                        { (void *)test_msg_PSS_11,                   (void *) sig_key1024_sha1_salt106,     sizeof(test_msg_PSS_11),1024, /*I think the maximum value is unknown*/  106,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_1024 /*29:key1024 */},
{80,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"key size 512, SHA1",                                       { (void *)test_msg_PSS_11,                   (void *) sig_key512_sha1_salt20,       sizeof(test_msg_PSS_11), 512,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_512 /*30:key512 */},
{81,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"key size 2048, SHA1",                                      { (void *)t2048_msg,                         (void *)t2048_sig,                     sizeof(t2048_msg),      2048,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_2048_2 /*31:key2048 */},
{82,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"key size 2048, SHA1",                                      { (void *)test_msg_PSS_11,                   (void *) sig_key2048_sha1_salt20,      sizeof(test_msg_PSS_11),2048,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_2048 /*31:key2048 */},
{83,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"key size 2048, SHA256",                                    { (void *)test_msg_PSS_11,                   (void *) sig_key2048_sha256_salt20,    sizeof(test_msg_PSS_11),2048,                                20,HASH_PRIMITIVE_SHA2_256,ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_2048 /*32:key2048 */},
{84,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"key size 2048, SHA512",                                    { (void *)test_msg_PSS_11,                   (void *) sig_key2048_sha512_salt20,    sizeof(test_msg_PSS_11),2048,                                20,HASH_PRIMITIVE_SHA2_512,ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_2048 /*33:key2048 */},
{85,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"key size 3072, SHA1",                                      { (void *)test_msg_PSS_11,                   (void *) sig_key3072_sha1_salt20,      sizeof(test_msg_PSS_11),3072,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_3072 /*34:key3072 */},
{86,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"uninitialized key,",                                       { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,(NB_RSA_PUB_KEYS-1), 0,0,0,                  },SERV_KEY_SLOT_EMPTY,   NULL },
{88,1/*Abnormal system*/,SERVICE_01_RSA_VERIFY,"unusable key, Debugger",                                   { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,                  },SERV_KEY_NOT_USABLE,   prepare_keyid0_1024_dbg /*37:key1024 disable_on_debug */},
{89,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"PKCS1_V15, message size is 0",                             { (void *)test_msg_PSS_11,                   (void *) sig_key1024_sha1_msg0_pkcs,    0,                     1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_1024 /*11:key1024 */},
{90,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"PKCS1_V15, message size is 1",                             { (void *)test_msg_PSS_11,                   (void *) sig_key1024_sha1_msg1_pkcs,    1,                     1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_1024 /*12:key1024 */},
{91,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"PKCS1_V15, message size is 16",                            { (void *)test_msg_PSS_11,                   (void *) sig_key1024_sha1_msg16_pkcs,  16,                     1024,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_1024 /*13:key1024 */},
{92,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"PKCS1_V15, key size 512, SHA1",                            { (void *)test_msg_PSS_11,                   (void *) sig_key512_sha1_pkcs,         sizeof(test_msg_PSS_11), 512,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_512 /*30:key512 */},
{93,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"PKCS1_V15, key size 2048, SHA1",                           { (void *)test_msg_PSS_11,                   (void *) sig_key2048_sha1_pkcs,        sizeof(test_msg_PSS_11),2048,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_2048 /*31:key2048 */},
{94,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"PKCS1_V15, key size 2048, SHA256",                         { (void *)test_msg_PSS_11,                   (void *) sig_key2048_sha256_pkcs,      sizeof(test_msg_PSS_11),2048,                                20,HASH_PRIMITIVE_SHA2_256,ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_2048 /*32:key2048 */},
{95,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"PKCS1_V15, key size 2048, SHA512",                         { (void *)test_msg_PSS_11,                   (void *) sig_key2048_sha512_pkcs,      sizeof(test_msg_PSS_11),2048,                                20,HASH_PRIMITIVE_SHA2_512,ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_2048 /*33:key2048 */},
{96,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"PKCS1_V15, key size 3072, SHA1",                           { (void *)test_msg_PSS_11,                   (void *) sig_key3072_sha1_pkcs,        sizeof(test_msg_PSS_11),3072,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PKCS1_V15,FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_3072 /*34:key3072 */},

{999,1/*Abnormal system*/, SERVICE_01_RSA_VERIFY,"Multi Start-up,",                                        { (void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,              32,1,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_id1_1024 },

{-1}
};

static const uint8_t sig_key2048_sha1_salt20_plain[]=
{
0x64, 0xa8, 0xbd, 0xb9, 0x62, 0xbe, 0xdf, 0xdf, 0xd8, 0xcd, 0xc3, 0xfe, 0x81, 0xb8, 0xf6, 0xb1,
0xa6, 0x79, 0x05, 0x5c, 0x9e, 0x19, 0x64, 0xac, 0x78, 0x01, 0x38, 0x5c, 0xf6, 0x8e, 0x47, 0xa1,
0x84, 0x1d, 0x5e, 0xa5, 0x30, 0x9b, 0xba, 0x59, 0x84, 0x41, 0x20, 0xaa, 0x87, 0x25, 0x19, 0xa9,
0x42, 0xa8, 0x7e, 0xc0, 0x1b, 0x6a, 0xda, 0xe9, 0xdf, 0x79, 0x9d, 0x15, 0xeb, 0xf2, 0x22, 0x3b,
0x89, 0x1d, 0xcd, 0xb4, 0x39, 0xbd, 0xb9, 0x3e, 0xb2, 0xc0, 0xb7, 0x07, 0xe8, 0x61, 0xe4, 0xb0,
0x35, 0x01, 0xfa, 0x33, 0x85, 0xd2, 0xa7, 0xcf, 0xb4, 0x7c, 0xa5, 0x43, 0x11, 0xba, 0x01, 0x55,
0x1a, 0x40, 0xb0, 0x59, 0x08, 0xa9, 0xbc, 0x0f, 0xd7, 0x99, 0xdd, 0xd0, 0xab, 0xfb, 0x85, 0x65,
0x73, 0xae, 0x06, 0xc8, 0x36, 0xcf, 0x5f, 0x26, 0xf5, 0x32, 0xfe, 0x14, 0xd9, 0xf0, 0x1a, 0x7f,
0x0a, 0x45, 0xa3, 0x0e, 0xbf, 0x08, 0x6d, 0x68, 0x94, 0x75, 0xe3, 0x8f, 0x2e, 0x30, 0x33, 0x96,
0x74, 0xb0, 0x63, 0x51, 0xba, 0x7b, 0x97, 0x81, 0x03, 0xd1, 0x1d, 0x58, 0x33, 0xc5, 0xe1, 0x64,
0x95, 0xe5, 0x88, 0x6b, 0x33, 0xe6, 0xfc, 0xd6, 0xd3, 0x80, 0xa0, 0x8f, 0x10, 0xa5, 0x0a, 0xaf,
0x2a, 0x9a, 0x3e, 0x39, 0x71, 0x18, 0x34, 0x12, 0xd7, 0xc4, 0x68, 0x7b, 0x2f, 0xe9, 0x7c, 0x82,
0x7c, 0x5e, 0x7f, 0xb1, 0xa5, 0xfb, 0x5f, 0x3c, 0xc4, 0x18, 0x95, 0xc6, 0x15, 0xbe, 0x35, 0x9f,
0x8b, 0x66, 0xe6, 0xcf, 0x82, 0x90, 0xa1, 0x37, 0xb2, 0xb9, 0xf6, 0x5a, 0x27, 0x5a, 0xa6, 0x19,
0x04, 0x5c, 0xe6, 0x63, 0x96, 0xc9, 0x62, 0x6b, 0xe8, 0x39, 0x77, 0x21, 0x53, 0x98, 0xca, 0xed,
0xb8, 0x7d, 0x62, 0x52, 0x9b, 0x77, 0x63, 0x75, 0x7e, 0x69, 0x12, 0x57, 0x0a, 0xe7, 0x63, 0x7a 
};

static const struct _test_struct     TestStruct2[] = {
{500,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"key size 2048, SHA1, hash_job_slice = 0, exp_job_slice = 1", { (void *)test_msg_PSS_11,                (void *) sig_key2048_sha1_salt20_plain,sizeof(test_msg_PSS_11),2048,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    0,               0,1,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_2048 /*31:key2048 */},
{501,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"key size 2048, SHA1, hash_job_slice = 0, exp_job_slice = 0", { (void *)test_msg_PSS_11,                (void *) sig_key2048_sha1_salt20_plain,sizeof(test_msg_PSS_11),2048,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    0,               0,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_2048 /*31:key2048 */},
{502,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"key size 2048, SHA1, hash_job_slice =32, exp_job_slice = 1", { (void *)test_msg_PSS_11,                (void *) sig_key2048_sha1_salt20_plain,sizeof(test_msg_PSS_11),2048,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    0,              32,1,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_2048 /*31:key2048 */},
{503,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"key size 2048, SHA1, hash_job_slice = 1, exp_job_slice = 0", { (void *)test_msg_PSS_11,                (void *) sig_key2048_sha1_salt20_plain,sizeof(test_msg_PSS_11),2048,                                20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_BINARY_PLAIN,  KEY_GRP_RSA_PUB,    0,               1,0,0,VERIFICATION_PASS },SERV_OK,               prepare_keyid0_2048 /*31:key2048 */},

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
    p_ISD_PARAM->key_group               = p_TS->TestElement.key_group;             ;\
    p_ISD_PARAM->key_id                  = p_TS->TestElement.key_id;                ;\
    p_ISD_PARAM->job_slice_hash          = p_TS->TestElement.job_slice_hash;        ;\
    p_ISD_PARAM->job_slice_mod_exp       = p_TS->TestElement.job_slice_mod_exp      ;\
    p_ISD_PARAM->is_precalculated_hash   = p_TS->TestElement.is_precalculated_hash  ;

static int _test_by_struct_info(struct _test_struct *p_TS)
{
    r_icumif_isd_t             *p_ISD         = (r_icumif_isd_t *)ISD_BUFFER;
    ISD_RSA_VERIFY_t           *p_ISD_PARAM   = &p_ISD->prm.RSA_VERIFY ;
    r_icumif_service_result_t   expected_code = p_TS->expected_code;        /* Expected results */

    int ret = 0 ;

    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);    // clear the ISD_BUFFER

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
                    p_ISD_PARAM->key_group, getKeyGroupName(p_ISD_PARAM->key_group), p_ISD_PARAM->key_id );

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

    printf( " expected verify result : (%s)\n", getVerifResultName(p_TS->TestElement.verification_result) );


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
            printf(" Verify result  : ");
            if( p_ISD_PARAM->verification_result == p_TS->TestElement.verification_result )
            {
                printf( "Expected.\n" );
            }
            else
            {
                printf( "Not Expected. %d[%s]\n", 
                        p_ISD_PARAM->verification_result, getVerifResultName(p_ISD_PARAM->verification_result) );
            }
        }
    }
    else
    {
        ret = 1;
        if( p_ISD->service_result == SERV_KEY_SLOT_EMPTY ){        /* Unexpected SERV_KEY_SLOT_EMPTY */
            ret = -1;
        }

        if(p_ISD->service_result == SERV_OK)
        {
            printf( "NOT EXPECTED. \n" );
            printf("Verify result  : %d[%s]\n", 
                   p_ISD_PARAM->verification_result, getVerifResultName(p_ISD_PARAM->verification_result) );

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
    ISD_RSA_VERIFY_t   *p_ISD_PARAM = &p_ISD1->prm.RSA_VERIFY ;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 2));    // clear the ISD_BUFFER

    /* set general parameter */
    _SET_TEST_PARAMETERS(p_ISD1);

    memcpy(p_ISD2, p_ISD1, sizeof(r_icumif_isd_t) );

    /* Test details display */
    printf( " rsa key     : group = %d(%s), id = %d\n", 
                    p_ISD_PARAM->key_group, getKeyGroupName(p_ISD_PARAM->key_group), p_ISD_PARAM->key_id );

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

    printf( " expected verify result : (%s)\n", getVerifResultName(p_TS->TestElement.verification_result) );


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

void icum_rsa_verify_signature_ex_table(int menuNo, struct _test_struct *p_TS_table, int precalculated_hash)
{
    int i;
    int cntPass = 0 ;
    int cntFail = 0 ;
    int cntSum = 0 ;
    int debugger_connect = (R_ICUMIF_GetStatus())->EXT_DEBUGGER ;
    struct _test_struct tmpTS ;

    printf("\n\nMenu No. %d : RSA_VERIFY ...\n", menuNo);
    printf( "%s::%s() %s\n", __FILE__, __FUNCTION__, (precalculated_hash ? "precalculated_hash" : "") );

    printf("\n Now debugfer is [%s]\n", debugger_connect ? "Connect" : "Not Connect" );
    printf( " NB_RSA_PUB_KEYS  = %d\n", 0 );

    memset( (void *)LOWER_LIMIT_GRAM+0x30, 0, (UPPER_LIMIT_GRAM - LOWER_LIMIT_GRAM-0x30) );     /* GRAM clear */

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

        printf( "\nTest RSA_VERIFY No. %2d. %s ...%s\n\n", tmpTS.testNo, tmpTS.comment,
                                                           precalculated_hash ? "(precalculated hash)" : "" );

        if( tmpTS.service_id == 0 )
        {
            printf( "\n  skip !!\n\n" );
            continue;
        }

        /* Table adjustment */      /* Replace the temporarily defined "number of keys" with the real thing */
        switch( tmpTS.TestElement.key_id )
        {
        case (NB_RSA_PUB_KEYS - 1) :    /* pub_key_id Maximum value */
            tmpTS.TestElement.key_id  = 0;
            break ;
        case NB_RSA_PUB_KEYS :          /* pub_key_id Maximum value +1 */
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
                                           BIG_BUFFER[0]);
            if( h_size == 0 ){
                printf( "\n  hash error !!\n\n" );
                continue ;
            }
            /* Use hash value for p_source */
            tmpTS.TestElement.p_source               = BIG_BUFFER[0] ;
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
            ret = _test_by_struct_info( &tmpTS ) ;        /* Test body */
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
    printf("\nMenu No. %d : RSA_VERIFY ...", menuNo);
    printf( " Test Result : All %d, Pass = %d, Fail = %d\n", cntSum, cntPass, cntFail );

    icum_rsa_totalCntPass += cntPass ;
    icum_rsa_totalCntFail += cntFail ;
    icum_rsa_totalCntSum  += cntSum  ;
}

/***********************************************************************************/
typedef struct
{
    unsigned char nb_words;
    unsigned int words[16];
} oid_hash_t;
static oid_hash_t oid_hash[] =
{
    {6, { 1, 3, 14, 3, 2, 26,  },            }, /* SHA-1 */
};

#define CRYPT_OK                (0)
#define CRYPT_INVALID_ARG       (-1)
#define CRYPT_BUFFER_OVERFLOW   (-2)

unsigned int der_object_identifier_bits(unsigned int x)
{
   unsigned int c;
   x &= 0xFFFFFFFFU;
   c  = 0;
   while (x) {
     ++c;
     x >>= 1U;
   }
   return c;
}

/**
  Gets length of DER encoding of Object Identifier
  \param nwords   The number of OID words 
  \param words    The actual OID words to get the size of
  \param outlen   [out] The length of the DER encoding for the given string
  \return CRYPT_OK if successful
*/
int der_length_object_identifier(unsigned int *words, unsigned int nwords, unsigned int *outlen)
{
   unsigned int y, z, t, wordbuf;

   /* must be >= 2 words */
   if (nwords < 2UL) {
      return CRYPT_INVALID_ARG;
   }

   /* word1 = 0,1,2,3 and word2 0..39 */
   if (words[0] > 3UL || (words[0] < 2UL && words[1] > 39UL)) {
      return CRYPT_INVALID_ARG;
   }

   /* leading word is the first two */
   z = 0;
   wordbuf = words[0] * 40UL + words[1];
   for (y = 1; y < nwords; y++) {
       t = der_object_identifier_bits(wordbuf);
       z += t/7UL + ((t%7UL) ? 1UL : 0UL) + (wordbuf == 0UL ? 1UL : 0UL);
       if (y < nwords - 1UL) {
          /* grab next word */
          wordbuf = words[y+1UL];
       }
   }

   /* now depending on the length our length encoding changes */
   if (z < 128UL) {
      z += 2UL;
   } else if (z < 256UL) {
      z += 3UL;
   } else if (z < 65536UL) {
      z += 4UL;
   } else {
      return CRYPT_INVALID_ARG;
   }

   *outlen = z;
   return CRYPT_OK;
}

/**
  Encode an OID
  \param words   The words to encode  (upto 32-bits each)
  \param nwords  The number of words in the OID
  \param out     [out] Destination of OID data
  \param outlen  [in/out] The max and resulting size of the OID
  \return CRYPT_OK if successful
*/
int der_encode_object_identifier(unsigned int *words, unsigned int  nwords,
                                 unsigned char *out,   unsigned int *outlen)
{
   unsigned int i, x, y, z, t, mask, wordbuf;
   int           err;

   /* check length */
   if ((err = der_length_object_identifier(words, nwords, &x)) != CRYPT_OK) {
      return err;
   }
   if (x > *outlen) {
      *outlen = x;
      return CRYPT_BUFFER_OVERFLOW;
   }

   /* compute length to store OID data */
   z = 0;
   wordbuf = words[0] * 40UL + words[1];
   for (y = 1; y < nwords; y++) {
       t = der_object_identifier_bits(wordbuf);
       z += t/7UL + (unsigned int)(((t%7UL) ? 1 : 0) + (wordbuf == 0UL ? 1 : 0));
       if (y < nwords - 1UL) {
          wordbuf = words[y + 1UL];
       }
   }

   /* store header + length */
   x = 0; 
   out[x++] = 0x06;
   if (z < 128UL) {
      out[x++] = (unsigned char)z;
   } else if (z < 256UL) {
      out[x++] = 0x81;
      out[x++] = (unsigned char)z;
   } else if (z < 65536UL) {
      out[x++] = 0x82;
      out[x++] = (unsigned char)((z>>8UL)&255UL);
      out[x++] = (unsigned char)(z&255UL);
   } else {
      return CRYPT_INVALID_ARG;
   }

   /* store first byte */
    wordbuf = words[0] * 40UL + words[1];
    for (i = 1; i < nwords; i++) {
        /* store 7 bit words in little endian */
        t    = wordbuf & 0xFFFFFFFFU;
        if (t) {
           y    = x;
           mask = 0;
           while (t) {
               out[x++] = (unsigned char)((t & 0x7FUL) | mask);
               t    >>= 7UL;
               mask  |= 0x80UL;  /* upper bit is set on all but the last byte */
           }
           /* now swap bytes y...x-1 */
           z = x - 1UL;
           while (y < z) {
               t = out[y]; out[y] = out[z]; out[z] = (unsigned char)t;
               ++y; 
               --z;
           }
       } else {
          /* zero word */
          out[x++] = 0x00;
       }
       
       if (i < nwords - 1UL) {
          wordbuf = words[i + 1UL];
       }
   }

   *outlen = x;
   return CRYPT_OK;
}


struct _test_struct     TestStruct_Forgery[] = {
{300,0/*Normal system*/,SERVICE_01_RSA_VERIFY,"PKCS1_V15 Forger test, key size 3072, SHA1", { (void *)BIG_BUFFER[0],       (void *) BIG_BUFFER[1],        sizeof(test_msg_PSS_11), 3072,  20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PKCS1_V15, FORMAT_BINARY_PLAIN,     KEY_GRP_RSA_PUB,    0,               0,0,VERIFICATION_FAIL },SERV_CRYPT_ERROR,  prepare_keyid0_3072_pub3},
{-1}
};
void icum_rsa_verify_forgey(int menuNo)
{
unsigned char test_Msg_PKCS_forge[sizeof(test_msg_PSS_11)] ;
unsigned char test_Sig_PKCS_forge[3072/8] ;
unsigned char encoded_oid_sha1[256] ;
static unsigned char D[3072/8] ;
static unsigned char N[3072/8] ;

    int cntPass = 0 ;
    int cntFail = 0 ;
    int cntSum = 0 ;
    r_icumif_isd_t         *p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    struct _test_struct    *p_TS  = TestStruct_Forgery;
    unsigned char hash[160/8] ;     // SHA1 hash
    unsigned int n = sizeof(encoded_oid_sha1);
    int hashlen = 160/8 ;       /* hash size of SHA1 */
    int ret = 0 ;

    printf("\n\nMenu No. %d : RSA_VERIFY ...\n", menuNo);
    printf( "%s::%s\n", __FILE__, __FUNCTION__ );

    printf( "\nTest RSA_VERIFY No. %2d. %s ...\n\n", p_TS->testNo, p_TS->comment );

    /* make encoded oid of SHA1 */
    der_encode_object_identifier(oid_hash->words, oid_hash->nb_words, encoded_oid_sha1, &n );

    /* make ASN.1 ( 00 + ASN.1 ) : use SHA1 */
    memset( D, 0x00, sizeof(D) );
    D[0] = 0x00;

    D[1] = 0x30;
    D[2] = n + 6 + hashlen;
    D[3] = 0x30;
    D[4] = n + 2;
    /* OID of sha1 */
    memcpy( &D[5], encoded_oid_sha1, n);

    n += 4 + 1;
    D[n++] = 0x05;
    D[n++] = 0x00;
    D[n++] = 0x04;
    D[n++] = hashlen;    /* hash size of SHA1 */

    printf(" D         : ");    PMEM(&D[0], (n));
    printf("\n");

    OCTET_STRING_R_shift(D, 3072 - (n + hashlen)*8, 3072/8);      /* Right-justified leaving the area of Hash */

    printf(" ASN.1     : ");    PMEM(&D[3072/8 - (n + hashlen)], (n + hashlen));
    printf("\n");

    memcpy( test_Msg_PKCS_forge, test_msg_PSS_11, sizeof(test_msg_PSS_11));
    for(;;)
    {
        /* make hash */
        p_ISD->service_id                     = SERVICE_01_HASH;
        p_ISD->p_callbackfunc                 = 0;
        p_ISD->job_id                         = 0;
        memcpy(BIG_BUFFER[1], test_Msg_PKCS_forge, sizeof(test_msg_PSS_11) );
        p_ISD->prm.HASH.p_message             = BIG_BUFFER[1];
        p_ISD->prm.HASH.message_size_in_bytes = sizeof(test_Msg_PKCS_forge);
        p_ISD->prm.HASH.hash_primitive        = HASH_PRIMITIVE_SHA1;
        p_ISD->prm.HASH.job_slice             = 0;
        p_ISD->prm.HASH.job_cycle             = JOB_ALL_AT_ONCE;
        p_ISD->prm.HASH.p_hash                = BIG_BUFFER[0];

        printf("hash job slice:  %d\n", p_ISD->prm.HASH.job_slice);
        printf("start hash...\n");
    
        // trigger the security service
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        
        printf("done\n");
        memcpy(hash, BIG_BUFFER[0], hashlen );

        /* make D : ( 00 + ASN.1 + hash ) */
        memcpy( &D[3072/8 - hashlen], hash, hashlen );
        printf(" ASN.1+hash: ");    PMEM(&D[3072/8 - (n + hashlen)-10], (n + hashlen)+10);
        printf("\n");

        /* make N : N = 2^288 - D */
        memset( N, 0x00, sizeof(N) );
        OCTET_STRING_setbit(N, 288, 3072/8);
        printf(" 2^288     : ");    PMEM(&N[3072/8 - 288/8 -10], 288/8+10);
        printf("\n");
        OCTET_STRING_sub(N, D, 3072/8);
        printf(" N         : ");    PMEM(&N[3072/8 - 288/8 -10], 288/8+10);
        printf("\n");

        /* confirm that N is multiple of 3 */
        if((ret = OCTET_STRING_modByDigit(N, 3, 3072/8)) == 0 ){
            break ;
        }
        printf("  remaider of division by 3 = %d\n", ret);

        /* increase message */
        OCTET_STRING_inc(test_Msg_PKCS_forge, sizeof(test_msg_PSS_11));
    }

    /* make forge signature, test_Sig_PKCS_forge[] : 2^1019 - (N * 2^34 / 3) */
    OCTET_STRING_divByDigit(N, 3, 3072/8);
    OCTET_STRING_L_shift(N, 34, 3072/8);
    printf(" N*2^34 / 3: ");    PMEM(&N[3072/8 - 288/8 -10], 288/8+10);
    printf("\n");

    memset( test_Sig_PKCS_forge, 0x00, sizeof(test_Sig_PKCS_forge) );
    OCTET_STRING_setbit(test_Sig_PKCS_forge, 1019, 3072/8);
    printf(" 2^1019    : ");    PMEM(&test_Sig_PKCS_forge[0], 3072/8);
    printf("\n");
    OCTET_STRING_sub(test_Sig_PKCS_forge, N, 3072/8 );
    printf(" Signature : ");    PMEM(&test_Sig_PKCS_forge[0], 3072/8);
    printf("\n");

    memcpy(BIG_BUFFER[0], test_Msg_PKCS_forge, 3072/8 );
    memcpy(BIG_BUFFER[1], test_Sig_PKCS_forge, 3072/8 );

    /* Preparation */
    if(p_TS->p_setup_func != NULL)
        p_TS->p_setup_func( p_TS );

    ret = _test_by_struct_info(p_TS);       /* Test body */

    cntSum ++ ;
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

    printf("\nMenu No. %d : RSA_VERIFY ...", menuNo);
    printf( " Test Result : All %d, Pass = %d, Fail = %d\n", cntSum, cntPass, cntFail );

    icum_rsa_totalCntPass += cntPass ;
    icum_rsa_totalCntFail += cntFail ;
    icum_rsa_totalCntSum  += cntSum  ;
}
/***********************************************************************************/

void icum_rsa_verify_signature_ex(int menuNo)
{

    icum_rsa_totalCntPass = 0 ;
    icum_rsa_totalCntFail = 0 ;
    icum_rsa_totalCntSum  = 0 ;

    icum_rsa_verify_signature_ex_table(menuNo, (struct _test_struct *)TestStruct2, 0);
    icum_rsa_verify_signature_ex_table(menuNo, (struct _test_struct *)TestStruct, 0);

    /* precalculated_hash : SERV_OK */
    icum_rsa_verify_signature_ex_table(menuNo, (struct _test_struct *)TestStruct, 1);
    /* precalculated_hash : SERV_PRM_ERROR */
    icum_rsa_verify_signature_ex_table(menuNo, (struct _test_struct *)TestStruct, 2);
    icum_rsa_verify_forgey(menuNo);
    printf( " Total Result : All %d, Pass = %d, Fail = %d\n", icum_rsa_totalCntSum, icum_rsa_totalCntPass, icum_rsa_totalCntFail );
}
