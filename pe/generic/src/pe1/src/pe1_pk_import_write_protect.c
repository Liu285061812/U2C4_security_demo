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

#include "pe1_pk_import_write_protect.h"

typedef enum
{
    PUBLIC_KEY = 0,
    PRIVATE_KEY
} key_type_t;

static void test_pk_import_without_writeprotect(void);
static void test_pk_import_with_writeprotect(void);
static int do_oneshot_pk_import(const testVectorForPkImport_t *p_TS);
static void pk_import_rsa2048(uint8_t write_protected, key_type_t key_type);
static void pk_import_rsa3072(uint8_t write_protected, key_type_t key_type);
static void pk_import_nist256(uint8_t write_protected, key_type_t key_type);
static void pk_import_nist224(uint8_t write_protected, key_type_t key_type);
// ********************************************************************************
// Local constants
// ********************************************************************************

static int16_t      exec_specified_test_no = 0;
void show_submenu_for_pk_import_writeprotect(void)
{
    int n;
    int exit_loop = 0;

    while(!exit_loop)
    {
        printf("\n\n| ICU-M Hello! PK_IMPORT write_protect tests\n");
        printf("|---------------------------------\n");
        printf("| 1  : Test write_protected = 0\n");
        printf("| 2  : Test write_protected = 1\n");
        printf("| 9  : Set exexute specified test No \n");
        printf("| q  : Quit\n\n");
        printf("  ?> ");
        vrun_console_get();
        if(*(char *)vrun_iobuff_readparam(0) == 'q')
        {
            n = -1;
            exit_loop = 1;
        }
        else
        {
            sscanf(vrun_iobuff_readparam(0), "%i", &n);
        }

        switch(n)
        {
            case 1:
                test_pk_import_without_writeprotect();
                break;
            case 2:
                test_pk_import_with_writeprotect();
                break;
            case 9:
                printf(" Execute Specified Test No (0 equal all, default 0)?> ");
                vrun_console_get();
                sscanf(vrun_iobuff_readparam(0), "%i", &n);
                exec_specified_test_no = n;
                break;
            default:
                break;
        }
    }
}

static void test_pk_import_without_writeprotect(void)
{
    int i;
    int cnt_pass = 0 ;
    int cnt_fail = 0 ;
    int cnt_total = 0 ;
    int ret ;
    
    uint8_t     write_protected = 0;
    key_type_t  key_type;
    
    printf("\nPrepare Test PK_IMPORT(without write_protect) Start ...\n");    
    for(key_type = PUBLIC_KEY; (uint8_t)key_type < (uint8_t)(PRIVATE_KEY + 1); key_type++)
    {
        pk_import_rsa2048(write_protected, key_type);
        pk_import_rsa3072(write_protected, key_type);
        pk_import_nist256(write_protected, key_type);
        pk_import_nist224(write_protected, key_type);
    }
    printf("Prepare Test PK_IMPORT(without write_protect) Finish ...\n");
    
    printf("\n\nTest PK_IMPORT(without write_protect) Start ...\n");
    for( i = 0; ; i++ )
    {
        /* terminate */
        if(testVector_pkImport[i].test_no <= 0) 
            break ;
        
        /* setup */
        if(testVector_pkImport[i].p_setup_func != NULL)
            testVector_pkImport[i].p_setup_func();

        /* execute specified test no ? */
        if(exec_specified_test_no != 0 && exec_specified_test_no != testVector_pkImport[i].test_no)
            continue;   /* skip */
        
        cnt_total ++ ;
        ret = do_oneshot_pk_import( &testVector_pkImport[i] ) ;

        if(ret == 0)
        {
            cnt_pass++ ;
        }
        else
        {
            cnt_fail++ ;
        }
    }
    
    printf( "\nTest Result : Executed %d, Passed = %d, Failed = %d\n", cnt_total, cnt_pass, cnt_fail );
}

static void test_pk_import_with_writeprotect(void)
{
    int i;
    int cnt_pass = 0 ;
    int cnt_fail = 0 ;
    int cnt_total = 0 ;
    int ret ;
    
    uint8_t     write_protected = 1;
    key_type_t  key_type;
    
    printf("\nPrepare Test PK_IMPORT(with write_protect) Start ...\n");    
    for(key_type = PUBLIC_KEY; (uint8_t)key_type < (uint8_t)(PRIVATE_KEY + 1); key_type++)
    {
        pk_import_rsa2048(write_protected, key_type);
        pk_import_rsa3072(write_protected, key_type);
        pk_import_nist256(write_protected, key_type);
        pk_import_nist224(write_protected, key_type);
    }
    printf("Prepare Test PK_IMPORT(with write_protect) Finish ...\n");
    
    printf("\n\nTest PK_IMPORT(with write_protect) Start ...\n");
    for( i = 0; ; i++ )
    {
        /* terminate */
        if(testVector_pkImport_2[i].test_no <= 0) 
            break ;
        
        /* setup */
        if(testVector_pkImport_2[i].p_setup_func != NULL)
            testVector_pkImport_2[i].p_setup_func();

        /* execute specified test no ? */
        if(exec_specified_test_no != 0 && exec_specified_test_no != testVector_pkImport_2[i].test_no)
            continue;   /* skip */
        
        cnt_total ++ ;
        ret = do_oneshot_pk_import( &testVector_pkImport_2[i] ) ;

        if(ret == 0)
        {
            cnt_pass++ ;
        }
        else
        {
            cnt_fail++ ;
        }
    }
    
    printf( "\nTest Result : Executed %d, Passed = %d, Failed = %d\n", cnt_total, cnt_pass, cnt_fail );
}

static int do_oneshot_pk_import(const testVectorForPkImport_t *p_TS)
{
    r_icumif_isd_t             *p_ISD                   = (r_icumif_isd_t *)ISD_BUFFER;
    ISD_PK_IMPORT_t            *p_ISD_PARAM             = &p_ISD->prm.PK_IMPORT ;
    r_icumif_service_result_t   expected_service_result = p_TS->expected_service_result;

    int ret = 0 ;

    printf( "\nTest PK_IMPORT(write_protect) No.%3d ...\n\n", p_TS->test_no );

    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER

    /* set general parameter */
    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD_PARAM->p_modulus                      = p_TS->test_param.p_modulus;   // =p_pub_point_x
    p_ISD_PARAM->p_pub_exp                      = p_TS->test_param.p_pub_exp;   // =p_pub_point_y
    p_ISD_PARAM->p_priv_exp                     = p_TS->test_param.p_priv_exp;  // =p_priv_scalar
    p_ISD_PARAM->p_signature                    = p_TS->test_param.p_signature;
    p_ISD_PARAM->p_info                         = p_TS->test_param.p_info;
    p_ISD_PARAM->key_format                     = p_TS->test_param.key_format;
    p_ISD_PARAM->sig_format                     = p_TS->test_param.sig_format;
    
    p_ISD_PARAM->key_group                      = p_TS->test_param.key_group;
    p_ISD_PARAM->key_id                         = p_TS->test_param.key_id;
    p_ISD_PARAM->sig_key_group                  = p_TS->test_param.sig_key_group;
    p_ISD_PARAM->sig_key_id                     = p_TS->test_param.sig_key_id;
    p_ISD_PARAM->sig_algo                       = p_TS->test_param.sig_algo;
    p_ISD_PARAM->hash_primitive                 = p_TS->test_param.hash_primitive;
    p_ISD_PARAM->curve_id                       = p_TS->test_param.curve_id;
    p_ISD_PARAM->modulus_size_in_bits           = p_TS->test_param.modulus_size_in_bits;    // point_x_size_in_bits
    p_ISD_PARAM->pub_exp_size_in_bits           = p_TS->test_param.pub_exp_size_in_bits;    // point_y_size_in_bits
    p_ISD_PARAM->priv_exp_size_in_bits          = p_TS->test_param.priv_exp_size_in_bits;   // priv_scalar_size_in_bits
    p_ISD_PARAM->sig_size_in_bits               = p_TS->test_param.sig_size_in_bits;
    p_ISD_PARAM->info_size_in_bytes             = p_TS->test_param.info_size_in_bytes;
    p_ISD_PARAM->p_x509_certificate             = p_TS->test_param.p_x509_certificate;
    p_ISD_PARAM->x509_certificate_size_in_bytes = p_TS->test_param.x509_certificate_size_in_bytes;
    p_ISD_PARAM->flags.disable_on_debug         = p_TS->test_param.flags.disable_on_debug;
    p_ISD_PARAM->flags.write_protected          = p_TS->test_param.flags.write_protected;
    p_ISD_PARAM->skip_verify                    = p_TS->test_param.skip_verify;

    printf( "Expected code = %d (%s)\n",        expected_service_result, R_ICUMIF_GetErrorCodeName(expected_service_result) );
    printf( "  key_group = %s\n",               getKeyGroupName(p_ISD_PARAM->key_group));
    printf( "  key_id = %d\n",                  p_ISD_PARAM->key_id);
    printf( "  modulus_bits(point_x) = %d\n",   p_ISD_PARAM->modulus_size_in_bits);
    printf( "  priv_exp_bits(priv_scalar) = %d\n",   p_ISD_PARAM->priv_exp_size_in_bits);
    printf( "  curve_id = %s\n",                getEccCurveName(p_ISD_PARAM->curve_id));
    printf( "  sig_key_group = %s\n",           getKeyGroupName(p_ISD_PARAM->sig_key_group));
    printf( "  sig_key_id = %d\n",              p_ISD_PARAM->sig_key_id );
    printf( "  write_protected = %d\n",         p_ISD_PARAM->flags.write_protected );
    printf( "  skip_verify = %d\n",             p_ISD_PARAM->skip_verify);

    /* trigger the security service */
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    /* fetch execution time in vrun_chrono_xxx variables */
    vrun_chrono_gettimestamp();
    printf("Processed in : "); PCHRONO(); NL(); FLUSH();

    printf("service result : ");
    if(p_ISD->service_result == expected_service_result)
    {
        if(p_ISD->service_result == SERV_OK)
        {
            printf( "Expected. (PASS!)\n");
        }
        else
        {
            printf( "Expected.\n" );
        }
    }
    else
    {
        ret = 1;
        printf( "NOT EXPECTED !! [%s]\n", R_ICUMIF_GetErrorCodeName(p_ISD->service_result) );
    }

    return(ret) ;
}


static void pk_import_rsa2048(uint8_t write_protected, key_type_t key_type)
{
    r_icumif_isd_t *p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER

    switch(key_type)
    {
        case PUBLIC_KEY:
            printf("RSA public key (2048bit) import index #0 write_protected = %d...\n", write_protected);
            p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_RSA_PUB;
            break;
        case PRIVATE_KEY:
            printf("RSA key pair (2048bit) import index #0 write_protected = %d...\n", write_protected);
            p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_RSA;
            p_ISD->prm.PK_IMPORT.p_priv_exp               = rsa2048_priv_exponent;
            p_ISD->prm.PK_IMPORT.priv_exp_size_in_bits    = sizeof(rsa2048_priv_exponent) * 8;
            break;
        default:
            printf("illegal key_type!!!\n");
            return;
    }

    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;
    
    p_ISD->prm.PK_IMPORT.key_id                   = 0;
    p_ISD->prm.PK_IMPORT.p_pub_exp                = rsa2048_pub_exponent;
    p_ISD->prm.PK_IMPORT.p_modulus                = rsa2048_modulus;
    p_ISD->prm.PK_IMPORT.pub_exp_size_in_bits     = sizeof(rsa2048_pub_exponent) * 8;
    p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = sizeof(rsa2048_modulus) * 8;
    p_ISD->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
    p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
    p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;
    
    p_ISD->prm.PK_IMPORT.flags.write_protected    = write_protected;
    p_ISD->prm.PK_IMPORT.skip_verify              = 1;
    
    // trigger the security service
    EXEC_SERVICE_WITH_TIMESTAMP(p_ISD);
    printf("Done!\n");
}


static void pk_import_rsa3072(uint8_t write_protected, key_type_t key_type)
{
    r_icumif_isd_t *p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER

    switch(key_type)
    {
        case PUBLIC_KEY:
            printf("RSA public key (3072bit) import index #1 write_protected = %d...\n", write_protected);
            p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_RSA_PUB;
            p_ISD->prm.PK_IMPORT.p_signature              = sig_key2048_sha256_rsa3072_pkcs;
            p_ISD->prm.PK_IMPORT.sig_size_in_bits         = sizeof(sig_key2048_sha256_rsa3072_pkcs) * 8;
            break;
        case PRIVATE_KEY:
            printf("RSA key pair (3072bit) import index #1 write_protected = %d...\n", write_protected);
            p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_RSA;
            p_ISD->prm.PK_IMPORT.p_priv_exp               = rsa3072_priv_exponent;
            p_ISD->prm.PK_IMPORT.priv_exp_size_in_bits    = sizeof(rsa3072_priv_exponent) * 8;
            p_ISD->prm.PK_IMPORT.p_signature              = sig_key2048_sha256_rsa3072_pair_pkcs;
            p_ISD->prm.PK_IMPORT.sig_size_in_bits         = sizeof(sig_key2048_sha256_rsa3072_pair_pkcs) * 8;
            break;
        default:
            printf("illegal key_type!!!\n");
            return;
    }

    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.PK_IMPORT.key_id                   = 1;
    p_ISD->prm.PK_IMPORT.p_pub_exp                = rsa3072_pub_exponent;
    p_ISD->prm.PK_IMPORT.p_modulus                = rsa3072_modulus;
    p_ISD->prm.PK_IMPORT.pub_exp_size_in_bits     = sizeof(rsa3072_pub_exponent) * 8;
    p_ISD->prm.PK_IMPORT.modulus_size_in_bits     = sizeof(rsa3072_modulus) * 8;
    p_ISD->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
    
    p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_RSA_PKCS1_V15;
    p_ISD->prm.PK_IMPORT.hash_primitive           = HASH_PRIMITIVE_SHA2_256;
    p_ISD->prm.PK_IMPORT.sig_format               = FORMAT_BINARY_PLAIN;
    p_ISD->prm.PK_IMPORT.sig_key_group            = KEY_GRP_RSA_PUB;
    p_ISD->prm.PK_IMPORT.sig_key_id               = 0;
    
    p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;

    p_ISD->prm.PK_IMPORT.flags.write_protected    = write_protected;
    p_ISD->prm.PK_IMPORT.skip_verify              = 1;

    // trigger the security service
    EXEC_SERVICE_WITH_TIMESTAMP(p_ISD);
    printf("Done!\n");
}


void pk_import_nist256(uint8_t write_protected, key_type_t key_type)
{
    r_icumif_isd_t *p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER

    switch(key_type)
    {
        case PUBLIC_KEY:
            printf("ECC public key (NIST_P256) import index #0 write_protected = %d...\n", write_protected);
            p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_ECC_PUB;
            break;
        case PRIVATE_KEY:
            printf("ECC key pair (NIST_P256) import index #0  write_protected = %d...\n", write_protected);
            p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_ECC;
            p_ISD->prm.PK_IMPORT.p_priv_scalar            = priv_scalar_nist256;
            p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = sizeof(priv_scalar_nist256) * 8;
            break;
        default:
            printf("illegal key_type!!!\n");
            return;
    }

    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.PK_IMPORT.key_id                   = 0;

    p_ISD->prm.PK_IMPORT.p_pub_point_x            = pub_point_x_nist256;
    p_ISD->prm.PK_IMPORT.p_pub_point_y            = pub_point_y_nist256;
    p_ISD->prm.PK_IMPORT.curve_id                 = ECC_CURVE_NIST_P256;

    p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = sizeof(pub_point_x_nist256) * 8;
    p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = sizeof(pub_point_y_nist256) * 8;

    p_ISD->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
    p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
    p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;

    p_ISD->prm.PK_IMPORT.flags.write_protected    = write_protected;
    p_ISD->prm.PK_IMPORT.skip_verify              = 1;

    // trigger the security service
    EXEC_SERVICE_WITH_TIMESTAMP(p_ISD);
    printf("Done!\n");
}


void pk_import_nist224(uint8_t write_protected, key_type_t key_type)
{
    r_icumif_isd_t *p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER

    switch(key_type)
    {
        case PUBLIC_KEY:
            printf("ECC public key (NIST_P224) import index #1 write_protected = %d...\n", write_protected);
            p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_ECC_PUB;
            p_ISD->prm.PK_IMPORT.p_signature              = sig_key2048_sha256_secp224r1_pkcs;
            p_ISD->prm.PK_IMPORT.sig_size_in_bits         = sizeof(sig_key2048_sha256_secp224r1_pkcs) * 8;
            break;
        case PRIVATE_KEY:
            printf("ECC key pair (NIST_P224) import index #1 write_protected = %d...\n", write_protected);
            p_ISD->prm.PK_IMPORT.key_group                = KEY_GRP_ECC;
            p_ISD->prm.PK_IMPORT.p_priv_scalar            = priv_scalar_nist224;
            p_ISD->prm.PK_IMPORT.priv_scalar_size_in_bits = sizeof(priv_scalar_nist224) * 8;
            p_ISD->prm.PK_IMPORT.p_signature              = sig_key2048_sha256_secp224r1_pair_pkcs;
            p_ISD->prm.PK_IMPORT.sig_size_in_bits         = sizeof(sig_key2048_sha256_secp224r1_pair_pkcs) * 8;
            break;
        default:
            printf("illegal key_type!!!\n");
            return;
    }

    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD->prm.PK_IMPORT.key_id                   = 1;

    p_ISD->prm.PK_IMPORT.p_pub_point_x            = pub_point_x_nist224;
    p_ISD->prm.PK_IMPORT.p_pub_point_y            = pub_point_y_nist224;
    p_ISD->prm.PK_IMPORT.curve_id                 = ECC_CURVE_NIST_P224;

    p_ISD->prm.PK_IMPORT.point_x_size_in_bits     = sizeof(pub_point_x_nist224) * 8;
    p_ISD->prm.PK_IMPORT.point_y_size_in_bits     = sizeof(pub_point_y_nist224) * 8;

    p_ISD->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
    
    p_ISD->prm.PK_IMPORT.sig_algo                 = SIGALGO_RSA_PKCS1_V15;
    p_ISD->prm.PK_IMPORT.hash_primitive           = HASH_PRIMITIVE_SHA2_256;
    p_ISD->prm.PK_IMPORT.sig_format               = FORMAT_BINARY_PLAIN;
    p_ISD->prm.PK_IMPORT.sig_key_group            = KEY_GRP_RSA_PUB;
    p_ISD->prm.PK_IMPORT.sig_key_id               = 0;

    p_ISD->prm.PK_IMPORT.flags.disable_on_debug   = 0;

    p_ISD->prm.PK_IMPORT.flags.write_protected    = write_protected;
    p_ISD->prm.PK_IMPORT.skip_verify              = 1;

    // trigger the security service
    EXEC_SERVICE_WITH_TIMESTAMP(p_ISD);
    printf("Done!\n");

}
