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

/*******************************************************************************
* Headers
*******************************************************************************/

#include <stdio.h>
#include <string.h>

#undef NULL
#include "r_icumif_api.h"
#include "r_icumif_pub.h"

#include "pe1.h"
#include "shared.h"

#include "vrun.api.h"

/* #include "rh850.toolbox.h" */

/*******************************************************************************
* Public constants
*******************************************************************************/

/*******************************************************************************
* Local constants
*******************************************************************************/

/* simple test vectors */
/* source: https://github.com/pyca/cryptography/blob/master/vectors/cryptography_vectors/asymmetric/RSA/pkcs-1v2-1d2-vec/pss-vect.txt*/

#include "pe1_rsa_keys.h"


#pragma alignvar(4)
#include "pe1_rsa_test_msg_PSS_11.h"
/* from "message.c" */
/*static const uint8_t test_msg_PSS_11[] */
/*static const uint8_t test_sig_PSS_11[] */
/*static const uint8_t test_sig_PSS_11_plain[] */

/* "pe1_rsa_sigs.h" : Signature corresponding to test message test_msg_PSS_11 [] */
#include "pe1_rsa_sigs.h"


/***********************************************************************************/
/* Tentative definition (replace before the test with the actual number of keys)) */
#define NB_RSA_PUB_KEYS        20

#include "pe_target_device.h"


/*Argument: Key group(r_key_group_t) */
/*You can also specify ROM key and RAM key */
/*Return value: The maximum number of slots in the specified key group. */
/*Note that it is not an index. */
typedef enum {
    IMPORT_NORMAL,
    IMPORT_DEBUGGER,
    IMPORT_VRF_SKIP = 0x04
} Import_special_t ;

/* Key registration function group */
static void prepare_key(r_key_group_t key_group, r_key_index_t key_id, void *keydata, int key_bits, int specialFlag  )
{
    r_icumif_isd_t     *p_ISD         = (r_icumif_isd_t *)ISD_BUFFER;
    ISD_PK_IMPORT_t    *p_ISD_PARAM   = &p_ISD->prm.PK_IMPORT ;
    printf( " key_group = %d, key_id = %d, keydata = (%08x), key_bits = %d, specialFlag = %d\n", key_group, key_id, keydata, key_bits, specialFlag );

    /* initialize the global icum service header */
    memset(p_ISD, 0, SIZE_OF_ISD_BUFFER);    /* clear the ISD_BUFFER */

    /* common part */
    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD_PARAM->key_group                = key_group;
    p_ISD_PARAM->key_id                   = key_id;
    p_ISD_PARAM->p_pub_exp                = strE10001;
    p_ISD_PARAM->pub_exp_size_in_bits     = 17;
    p_ISD_PARAM->p_modulus                = keydata;
    p_ISD_PARAM->modulus_size_in_bits     = key_bits;
    p_ISD_PARAM->priv_exp_size_in_bits    = 0;
    p_ISD_PARAM->key_format               = FORMAT_STRING_HEXA;
    p_ISD_PARAM->sig_algo                 = SIGALGO_NONE;
    p_ISD_PARAM->flags.disable_on_debug   = 0;
/*    p_ISD_PARAM->skip_verify              = 0; */
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
        printf( " skip_verify = 1" );

    /* trigger the security service */
    trigger_service_and_wait(p_ISD);

    if( p_ISD->service_result == SERV_OK )
    {
        printf("\nDone!\n");
    }
    else
    {
        printf("\nError [%s]\n", R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}

struct _test_struct {
    int testNo;                 /* Test number*/
    int AbnormalFlag;           /* 0: Normal system 1: Abnormal system */
    r_icumif_service_id_t       service_id;         /* Service ID */
    char                       *comment;            /* Comment (purpose column) */
    ISD_RSA_VERIFY_t            TestElement;        /* RSA_VERIFY structure */
    r_icumif_service_result_t   expected_code1;      /* Expected results */
    r_icumif_service_result_t   expected_code2;      /* Expected results */
    void                      (*p_setup_func)();    /*  */
} ;

static void prepare_keyid0_id1_1024(struct _test_struct *p_TS)
{
    printf( "\n %s()\n", __FUNCTION__ );

    prepare_key(KEY_GRP_RSA_PUB, 0, (void *)N_1024, 1024, IMPORT_NORMAL );
    prepare_key(KEY_GRP_RSA_PUB, 1, (void *)N_1024, 1024, IMPORT_VRF_SKIP );
}

#define HASH_PRIMITIVE_MAX_OVER  (r_hash_primitive_t)(HASH_PRIMITIVE_SHA2_512+1)
static struct _test_struct     TestStruct[] = {
{900,0/*Normal system*/, SERVICE_01_RSA_VERIFY,"Multi Start-up,",                                         {(void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,              32,1,0,VERIFICATION_PASS },SERV_OK,   SERV_OK,             prepare_keyid0_id1_1024 },
{901,0/*Normal system*/, SERVICE_01_RSA_VERIFY,"Multi Start-up, update(PK_IMPORT) and getkey(RSA Vefiry)",{(void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,              32,1,0,VERIFICATION_PASS },SERV_OK,   SERV_OK,             prepare_keyid0_id1_1024 },
{902,0/*Normal system*/, SERVICE_01_RSA_VERIFY,"Multi Start-up, update(PK_IMPORT) and getkey(RSA Vefiry)",{(void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,              32,1,0,VERIFICATION_PASS },SERV_OK,   SERV_OK,             prepare_keyid0_id1_1024 },
{903,0/*Normal system*/, SERVICE_01_RSA_VERIFY,"Multi Start-up, getkey(RSA Vefiry) and update(PK_IMPORT)",{(void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,              32,1,0,VERIFICATION_PASS },SERV_OK,   SERV_OK,             prepare_keyid0_id1_1024 },
{904,1/*Normal system*/, SERVICE_01_RSA_VERIFY,"Multi Start-up, getkey(RSA Vefiry) and update(PK_IMPORT)",{(void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,              32,1,0,VERIFICATION_PASS },SERV_OK,   SERV_KEY_NOT_USABLE, prepare_keyid0_id1_1024 },/*SERV_SEQUENCE_ERROR*/
{905,1/*Normal system*/, SERVICE_01_RSA_VERIFY,"Multi Start-up, updata RSA and update AES",               {(void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,              32,1,0,VERIFICATION_PASS },SERV_KEY_NOT_USABLE,   SERV_OK, NULL },
{906,1/*Normal system*/, SERVICE_01_RSA_VERIFY,"Multi Start-up, updata RSA and update AES",               {(void *)test_msg_PSS_11,                   (void *)test_sig_PSS_11,               sizeof(test_msg_PSS_11),sizeof(test_sig_PSS_11_plain) * 8,   20,HASH_PRIMITIVE_SHA1,    ENCODING_FORMAT_EMSA_PSS,      FORMAT_STRING_HEXA,   KEY_GRP_RSA_PUB,    1,              32,1,0,VERIFICATION_PASS },SERV_OK,   SERV_KEY_NOT_USABLE, NULL },/*SERV_SEQUENCE_ERROR*/

{-1}
};

#define _SET_TEST_PARAMETERS(p_ISD, p_ISD_PARAM) \
    p_ISD->service_id     = p_TS->service_id;\
    p_ISD->p_callbackfunc = 0;\
    p_ISD->job_id         = 0;\
    p_ISD_PARAM->p_source                = p_TS->TestElement.p_source              ;\
    p_ISD_PARAM->p_signature             = p_TS->TestElement.p_signature            ;\
    p_ISD_PARAM->source_size_in_bytes    = p_TS->TestElement.source_size_in_bytes  ;\
    p_ISD_PARAM->signature_size_in_bits  = p_TS->TestElement.signature_size_in_bits ;\
    p_ISD_PARAM->salt_size_in_bytes      = p_TS->TestElement.salt_size_in_bytes;    ;\
    p_ISD_PARAM->hash_primitive          = p_TS->TestElement.hash_primitive;        ;\
    p_ISD_PARAM->hash_encoding_format    = p_TS->TestElement.hash_encoding_format;  ;\
    p_ISD_PARAM->signature_format        = p_TS->TestElement.signature_format;      ;\
    p_ISD_PARAM->key_group               = p_TS->TestElement.key_group;             ;\
    p_ISD_PARAM->key_id                  = p_TS->TestElement.key_id;                ;\
    p_ISD_PARAM->job_slice_hash          = p_TS->TestElement.job_slice_hash;        ;\
    p_ISD_PARAM->job_slice_mod_exp       = p_TS->TestElement.job_slice_mod_exp      ;


#define _SET_PARAMETERS_PK_IMPORT(p_ISD, p_ISD_PARAM) \
    p_ISD->service_id     = SERVICE_03_PK_IMPORT;\
    p_ISD->p_callbackfunc = 0;\
    p_ISD->job_id         = 0;\
    p_ISD_PARAM->key_group                = p_TS->TestElement.key_group;             \
    p_ISD_PARAM->key_id                   = p_TS->TestElement.key_id;                \
    p_ISD_PARAM->p_pub_exp                = strE10001;                               \
    p_ISD_PARAM->pub_exp_size_in_bits     = 17;                                      \
    p_ISD_PARAM->p_modulus                = (void *)N_1024;                          \
    p_ISD_PARAM->modulus_size_in_bits     = 1024;                                    \
    p_ISD_PARAM->priv_exp_size_in_bits    = 0;                                       \
    p_ISD_PARAM->key_format               = FORMAT_STRING_HEXA;                      \
    p_ISD_PARAM->sig_algo                 = SIGALGO_NONE;                            \
    p_ISD_PARAM->flags.disable_on_debug   = 0;                                       \
    p_ISD_PARAM->skip_verify              = 1;

static void printf_verify_detail(ISD_RSA_VERIFY_t *p_ISD_PARAM)
{
    /* Test details display */
    printf( " RSA Verify\n" ); 
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
}

static void printf_import_detail(ISD_PK_IMPORT_t *p_ISD_PARAM)
{
    /* Test details display */
    printf( " PK Import\n" ); 
    printf( " rsa key     : group = %d(%s), id = %d\n", 
                    p_ISD_PARAM->key_group, getKeyGroupName(p_ISD_PARAM->key_group), p_ISD_PARAM->key_id );

    printf( " public key  : (%08x) %d bytes\n",      p_ISD_PARAM->p_pub_exp,   p_ISD_PARAM->pub_exp_size_in_bits );
    printf( " modulus     : (%08x) %d bytes\n",      p_ISD_PARAM->p_modulus,   p_ISD_PARAM->modulus_size_in_bits );
    printf( " public key  : (%08x) %d bytes\n",      p_ISD_PARAM->p_priv_exp,  p_ISD_PARAM->priv_exp_size_in_bits );

    printf( " key format  : %d(%s)\n", p_ISD_PARAM->key_format,
                   getNumberFormatName(p_ISD_PARAM->key_format) );

    printf( " sig algo    : %d(%s)\n", p_ISD_PARAM->sig_algo,
                   getSigAlgoName(     p_ISD_PARAM->sig_algo) );
}

static int _multi_startup(struct _test_struct *p_TS)
{
    r_icumif_isd_t     *p_ISD1      = (r_icumif_isd_t *)ISD_BUFFER;
    r_icumif_isd_t     *p_ISD2      = p_ISD1 + 1;
    ISD_RSA_VERIFY_t   *p_ISD_PARAM = &p_ISD1->prm.RSA_VERIFY ;
    r_icumif_service_result_t   expected_code1 = p_TS->expected_code1;        /* Expected results */
    r_icumif_service_result_t   expected_code2 = p_TS->expected_code2;        /* Expected results */

    printf( "%s::%s()\n", __FILE__, __FUNCTION__ );

    /* initialize the global icum service header */
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 2));    /* clear the ISD_BUFFER */
    /* set general parameter */
    _SET_TEST_PARAMETERS(p_ISD1, p_ISD_PARAM);

    memcpy(p_ISD2, p_ISD1, sizeof(r_icumif_isd_t) );

    if( p_TS->AbnormalFlag == 0 )
    {
    }
    else
    {
        printf( "(ABNORMAL Test) " );
    }
    printf( "Expected code = ISD1 : %d(%s), IDS2 : %d(%s)\n\n", expected_code1, R_ICUMIF_GetErrorCodeName(expected_code1)
                                                              , expected_code2, R_ICUMIF_GetErrorCodeName(expected_code2) );

    printf_verify_detail(p_ISD_PARAM);

    printf( " expected verify result : (%s)\n", getVerifResultName(p_TS->TestElement.verification_result) );

    /* trigger the security service */
    vrun_chrono_start();
    trigger_2service_and_wait(p_ISD1, p_ISD2);
    vrun_chrono_stop();

    /* fetch execution time in vrun_chrono_xxx variables */
    vrun_chrono_gettimestamp();

    PCHRONO();
    NL();
    FLUSH();

    printf("ISD1 Result : %s\n",R_ICUMIF_GetErrorCodeName(p_ISD1->service_result));
    printf("ISD2 Result : %s\n",R_ICUMIF_GetErrorCodeName(p_ISD2->service_result));

    printf(" Service result: ");
    if( ( p_ISD1->service_result == expected_code1 )
     && ( p_ISD2->service_result == expected_code2 ) )
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


/* int order    -1 : verify after import(), 1: verify before import() */
static int _multi_startup_with_import(struct _test_struct *p_TS, int order, bool same_id )
{
    r_icumif_isd_t     *p_ISD1       = (r_icumif_isd_t *)ISD_BUFFER;
    r_icumif_isd_t     *p_ISD2       = p_ISD1 + 1;
    ISD_RSA_VERIFY_t   *p_ISD_PARAM1 = &p_ISD1->prm.RSA_VERIFY ;
    ISD_PK_IMPORT_t    *p_ISD_PARAM2 = &p_ISD2->prm.PK_IMPORT ;
    r_icumif_service_result_t   expected_code1 = p_TS->expected_code1;        /* Expected results */
    r_icumif_service_result_t   expected_code2 = p_TS->expected_code2;        /* Expected results */

    printf( "%s::%s()\n", __FILE__, __FUNCTION__ );
    switch(order)
    {
    case -1 :   printf( "\n1st step : import() - ISD2, 2nd step : verify() - ISD1\n");
        break ;
    case  1 :   printf( "\n1st step : verify() - ISD1, 2nd step : import() - ISD2\n");
        break ;
    }

    /* initialize the global icum service header */
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 2));    /* clear the ISD_BUFFER */

    /* set general parameter */
    _SET_TEST_PARAMETERS(p_ISD1, p_ISD_PARAM1);         /* Set the parameters of RSA_VERIFY */

    _SET_PARAMETERS_PK_IMPORT(p_ISD2, p_ISD_PARAM2);    /* Set the parameters of PK_IMPORT */

    if( p_TS->AbnormalFlag == 0 )
    {
        /* Use different ID in normal system */
        /*p_ISD_PARAM2->key_id = ( p_ISD_PARAM2->key_id == 0 ) ? 1 : 0 ; */
    }
    else
    {
        printf( "(ABNORMAL Test) " );
    }
    if( same_id == false )
    {
        /* Make different ID */
        p_ISD_PARAM2->key_id = ( p_ISD_PARAM1->key_id == 0 ) ? 1 : 0 ;
    }
    printf( "Expected code = ISD1 : %d(%s), IDS2 : %d(%s)\n\n", expected_code1, R_ICUMIF_GetErrorCodeName(expected_code1)
                                                              , expected_code2, R_ICUMIF_GetErrorCodeName(expected_code2) );

    printf_verify_detail(p_ISD_PARAM1);
    printf_import_detail(p_ISD_PARAM2);

    /* trigger the security service */
    vrun_chrono_start();
    switch(order)
    {
    case -1 :   /* verify after import() */
        trigger_2service_and_wait(p_ISD2, p_ISD1);
        break ;
    case 1 :    /* verify before import() */
        trigger_2service_and_wait(p_ISD1, p_ISD2);
        break ;
    }
    vrun_chrono_stop();

    /* fetch execution time in vrun_chrono_xxx variables */
    vrun_chrono_gettimestamp();

    PCHRONO();
    NL();
    FLUSH();

    printf("RSA_VERIFY:ISD1 Result : %s\n",R_ICUMIF_GetErrorCodeName(p_ISD1->service_result));
    printf("PK_IMPORT :ISD2 Result : %s\n",R_ICUMIF_GetErrorCodeName(p_ISD2->service_result));

    printf(" Service result: ");
    if( ( p_ISD1->service_result == expected_code1 )
     && ( p_ISD2->service_result == expected_code2 ) )
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

static int _multi_startup_rsa_aes(struct _test_struct *p_TS, int order)
{
    r_icumif_isd_t         *p_ISD1       = (r_icumif_isd_t *)ISD_BUFFER;
    r_icumif_isd_t         *p_ISD2       = p_ISD1 + 1;
    ISD_KEY_UPDATE_PLAIN_t *p_ISD_PARAM1 = &p_ISD1->prm.KEY_UPDATE_PLAIN ;
    ISD_PK_IMPORT_t        *p_ISD_PARAM2 = &p_ISD2->prm.PK_IMPORT ;

    r_icumif_service_result_t   expected_code1 = p_TS->expected_code1;        /* Expected results */
    r_icumif_service_result_t   expected_code2 = p_TS->expected_code2;        /* Expected results */

    printf( "%s::%s()\n", __FILE__, __FUNCTION__ );
    switch(order)
    {
    case -1 :   printf( "\n1st step : import() - ISD2, 2nd step : aes() - ISD1\n");
        break ;
    case  1 :   printf( "\n1st step : aes() - ISD1,    2nd step : import() - ISD2\n");
        break ;
    }

/*SERVICE_03_AES_KEY_UPDATE_PLAIN */
    p_ISD1->service_id              = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD1->p_callbackfunc          = 0;
    p_ISD1->job_id                  = 0;

    p_ISD_PARAM1->key_group         = KEY_GRP_AES;
    p_ISD_PARAM1->key_id            = 0;
/*    p_ISD_PARAM1->p_key_value       = KEY; */
    p_ISD_PARAM1->p_key_value       = (void *)N_1024;                   /* Whatever */
    p_ISD_PARAM1->key_size_in_bytes = 16;

    _SET_PARAMETERS_PK_IMPORT(p_ISD2, p_ISD_PARAM2);    /* Set the parameters of PK_IMPORT */

    printf( "Expected code = ISD1 : %d(%s), IDS2 : %d(%s)\n\n", expected_code1, R_ICUMIF_GetErrorCodeName(expected_code1)
                                                              , expected_code2, R_ICUMIF_GetErrorCodeName(expected_code2) );

    /* trigger the security service */
    vrun_chrono_start();

    switch(order)
    {
    case -1 :   /* after import() */
        trigger_2service_and_wait(p_ISD2, p_ISD1);
        break ;
    case 1 :    /* before import() */
        trigger_2service_and_wait(p_ISD1, p_ISD2);
        break ;
    }
    vrun_chrono_stop();

    /* fetch execution time in vrun_chrono_xxx variables */
    vrun_chrono_gettimestamp();

    PCHRONO();
    NL();
    FLUSH();

    printf("AES_UPDATE:ISD1 Result : %s\n",R_ICUMIF_GetErrorCodeName(p_ISD1->service_result));
    printf("PK_IMPORT :ISD2 Result : %s\n",R_ICUMIF_GetErrorCodeName(p_ISD2->service_result));

    printf(" Service result: ");
    if( ( p_ISD1->service_result == expected_code1 )
     && ( p_ISD2->service_result == expected_code2 ) )
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

static void icum_key_exclusion_test_byTable(int menuNo, struct _test_struct *p_TS_param)
{
    int i;
    int cntPass = 0 ;
    int cntFail = 0 ;
    int cntTotal = 0 ;
    int debugger_connect = (R_ICUMIF_GetStatus())->EXT_DEBUGGER ;
/*    r_icumif_service_result_t restore_expected_code1 ; */     /* Error code (expected value) rewrite */
    struct _test_struct *p_TS ;

    printf("\n\nMenu No. %d : EXCLUSION_CONTROLL ...\n", menuNo);
    printf( "%s::%s()\n", __FILE__, __FUNCTION__ );

    printf("\n Now debugfer is [%s]\n", debugger_connect ? "Connect" : "Not Connect" );
    printf( " NB_RSA_PUB_KEYS  = %d\n", 0 );

    memset( (void *)(ADDR_LOWER_CRAM+0x20), 0, (ADDR_UPPER_CRAM - (ADDR_LOWER_CRAM+0x20)) );     /* GRAM clear */

    p_TS = p_TS_param ;
    /* Test execution loop */
    for( i = 0; ; i++, p_TS++ )
    {
        int ret ;

/*        restore_expected_code1 = SERV_OK ; */

        if( p_TS->testNo <= 0) /* Table end? */
        {
            break ;
        }

        printf( "\nTest EXCLUSION_CONTROLL No. %2d. %s ...\n\n", p_TS->testNo, p_TS->comment );

        cntTotal ++ ;

        /* Preparation */
        if(p_TS->p_setup_func != NULL)
            p_TS->p_setup_func( p_TS );

        switch( p_TS->testNo )
        {
        case 900 :
           ret =  _multi_startup( p_TS );
           break ;
        case 901 :
        case 902 :
           /*ret =  _multi_startup_after_import( p_TS );*/
           ret = _multi_startup_with_import(p_TS, -1, (p_TS->testNo==902) );
           break ;
        case 903 :
        case 904 :
           /*ret =  _multi_startup_before_import( p_TS );*/
           ret = _multi_startup_with_import(p_TS, 1, (p_TS->testNo==904) );
           break ;
        case 905 :
           ret =  _multi_startup_rsa_aes( p_TS, -1 );     /* after import() */
           break ;
        case 906 :
           ret =  _multi_startup_rsa_aes( p_TS, 1 );     /* before import() */
           break ;

        default :
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
    printf("\nMenu No. %d : EXCLUSION_CONTROLL ...", menuNo);
    printf( " Test Result : All %d, Pass = %d, Fail = %d\n", cntTotal, cntPass, cntFail );
}

/***********************************************************************************/

void icum_key_exlcution_controll(int menuNo)
{
    printf( "%s::%s()\n", __FILE__, __FUNCTION__ );

    icum_key_exclusion_test_byTable(menuNo, TestStruct);
}
