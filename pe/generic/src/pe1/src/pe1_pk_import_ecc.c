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

#include "vrun.api.h"
#include "pe1_pk_import_ecc.h"

// ********************************************************************************
// prototype
// ********************************************************************************
static void replace_key_id(testVectorForPkImportEcc_t *p_testVector);
static void import_sign_pub_key(void);

static void test_pk_import_ecc_pub_nvm(void);
static void test_pk_import_ecc_pub_ram(void);
void test_pk_import_ecc_priv_nvm(void);
static void test_pk_import_ecc_priv_ram(void);
static void test_pk_import_ecc_err(void);
#ifdef UNUSED
static void test_pk_import_ecc_for_special(void);
static void test_ecdsa_verify_using_pub_key(void);
#endif


static int  do_oneshot_pk_import(testVectorForPkImportEcc_t *p_TS);
#ifdef UNUSED
static int  do_oneshot_ecdsa_verify(testVectorForEcdsaVerify_t *p_TS);
#endif

static int16_t      exec_specified_test_no = 0;

void show_submenu_for_pk_import_ecc()
{
    int n;
    int exit_loop = 0;

    while(!exit_loop)
    {
        printf("\n\n| ICU-M Hello! PK_IMPORT ecc key tests\n");
        printf("|---------------------------------\n");
        printf("| 1  : Test public key import ecc pub nvm key\n");
        printf("| 2  : Test public key import ecc pub ram key\n");
        printf("| 3  : Test public key import ecc priv nvm key\n");
        printf("| 4  : Test public key import ecc priv ram key\n");
        printf("| 5  : Test public key import ecc err test\n");
        printf("| 7  : Test public key import ecc key(disable_on_debug)\n");
        printf("| 8  : Test ecdsa verify using ecc pub key\n");
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
                test_pk_import_ecc_pub_nvm();
                break;
            case 2:
                test_pk_import_ecc_pub_ram();
                break;
            case 3:
                test_pk_import_ecc_priv_nvm();
                break;
            case 4:
                test_pk_import_ecc_priv_ram();
                break;
            case 5:
                test_pk_import_ecc_err();
                break;
            case 7:
                /* do nothing */
                break;
            case 8:
                /* do nothing */
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

static void test_pk_import_ecc_pub_nvm(void)
{
    int i;
    int cnt_pass = 0 ;
    int cnt_fail = 0 ;
    int cnt_total = 0 ;
    int ret = 0 ;
    
    printf("\nPrepare Test PK_IMPORT ecc pub_nvm_key Start ...\n");
    import_sign_pub_key();
    if(ret != 0)
        goto end;
        
    printf("\nPrepare Test PK_IMPORT ecc pub_nvm_key Finish ...\n");
    
    printf("\n\nTest PK_IMPORT ecc pub_nvm_key Start ...\n");
    for( i = 0; ; i++ )
    {
        testVectorForPkImportEcc_t testVector;
        
        memcpy(&testVector, &testVector_pkImportEcc[i], sizeof(testVectorForPkImportEcc_t));
        replace_key_id(&testVector);
        
        /* terminate */
        if(testVector.test_no <= 0)
            break ;
        
        /* setup */
        if(testVector.p_setup_func != NULL)
            testVector.p_setup_func(0);   /* 0 is dummy */

        /* execute specified test no ? */
        if(exec_specified_test_no != 0 && exec_specified_test_no != testVector.test_no)
            continue;   /* skip */
        
        cnt_total ++ ;
        ret = do_oneshot_pk_import( &testVector ) ;

        if(ret == 0)
        {
            cnt_pass++ ;
        }
        else
        {
            cnt_fail++ ;
        }
    }
end:
    printf( "\nTest Result : Executed %d, Passed = %d, Failed = %d\n", cnt_total, cnt_pass, cnt_fail );
}

static void test_pk_import_ecc_pub_ram(void)
{
    int i;
    int cnt_pass = 0 ;
    int cnt_fail = 0 ;
    int cnt_total = 0 ;
    int ret = 0 ;
    
    printf("\nPrepare Test PK_IMPORT ecc pub_ram_key Start ...\n");
    import_sign_pub_key();
    if(ret != 0)
        goto end;
        
    printf("\nPrepare Test PK_IMPORT ecc pub_ram_key Finish ...\n");
    
    printf("\n\nTest PK_IMPORT ecc pub_ram_key Start ...\n");
    for( i = 0; ; i++ )
    {
        testVectorForPkImportEcc_t testVector;
        
        memcpy(&testVector, &testVector_pkImportEcc_2[i], sizeof(testVectorForPkImportEcc_t));
        replace_key_id(&testVector);
        
        /* terminate */
        if(testVector.test_no <= 0) 
            break ;
        
        /* setup */
        if(testVector.p_setup_func != NULL)
            testVector.p_setup_func(0);

        /* execute specified test no ? */
        if( (exec_specified_test_no != 0) && (exec_specified_test_no != testVector.test_no) )
            continue;   /* skip */
        
        cnt_total ++ ;
        ret = do_oneshot_pk_import( &testVector ) ;

        if(ret == 0)
        {
            cnt_pass++ ;
        }
        else
        {
            cnt_fail++ ;
        }
    }
end:
    printf( "\nTest Result : Executed %d, Passed = %d, Failed = %d\n", cnt_total, cnt_pass, cnt_fail );
}

void test_pk_import_ecc_priv_nvm(void)
{
    int i;
    int cnt_pass = 0 ;
    int cnt_fail = 0 ;
    int cnt_total = 0 ;
    int ret = 0 ;
    
    printf("\nPrepare Test PK_IMPORT ecc priv_nvm_key Start ...\n");
    import_sign_pub_key();
    if(ret != 0)
        goto end;
        
    printf("\nPrepare Test PK_IMPORT ecc priv_nvm_key Finish ...\n");
    
    printf("\n\nTest PK_IMPORT ecc priv_nvm_key Start ...\n");
    for( i = 0; ; i++ )
    {
        testVectorForPkImportEcc_t testVector;
        
        memcpy(&testVector, &testVector_pkImportEcc_3[i], sizeof(testVectorForPkImportEcc_t));
        replace_key_id(&testVector);
        
        /* terminate */
        if(testVector.test_no <= 0) 
            break ;
        
        /* setup */
        if(testVector.p_setup_func != NULL)
            testVector.p_setup_func(0);

        /* execute specified test no ? */
        if( (exec_specified_test_no != 0) && (exec_specified_test_no != testVector.test_no) )
            continue;   /* skip */
        
        cnt_total ++ ;
        ret = do_oneshot_pk_import( &testVector ) ;

        if(ret == 0)
        {
            cnt_pass++ ;
        }
        else
        {
            cnt_fail++ ;
        }
    }
end:
    printf( "\nTest Result : Executed %d, Passed = %d, Failed = %d\n", cnt_total, cnt_pass, cnt_fail );
}


static void test_pk_import_ecc_priv_ram(void)
{
    int i;
    int cnt_pass = 0 ;
    int cnt_fail = 0 ;
    int cnt_total = 0 ;
    int ret = 0 ;
    
    printf("\nPrepare Test PK_IMPORT ecc priv_ram_key Start ...\n");
    import_sign_pub_key();
    if(ret != 0)
        goto end;
        
    printf("\nPrepare Test PK_IMPORT ecc priv_ram_key Finish ...\n");
    
    printf("\n\nTest PK_IMPORT ecc priv_ram_key Start ...\n");
    for( i = 0; ; i++ )
    {
        testVectorForPkImportEcc_t testVector;
        
        memcpy(&testVector, &testVector_pkImportEcc_4[i], sizeof(testVectorForPkImportEcc_t));
        replace_key_id(&testVector);
        
        /* terminate */
        if(testVector.test_no <= 0) 
            break ;
        
        /* setup */
        if(testVector.p_setup_func != NULL)
            testVector.p_setup_func(0);

        /* execute specified test no ? */
        if( (exec_specified_test_no != 0) && (exec_specified_test_no != testVector.test_no) )
            continue;   /* skip */
        
        cnt_total ++ ;
        ret = do_oneshot_pk_import( &testVector ) ;

        if(ret == 0)
        {
            cnt_pass++ ;
        }
        else
        {
            cnt_fail++ ;
        }
    }
end:
    printf( "\nTest Result : Executed %d, Passed = %d, Failed = %d\n", cnt_total, cnt_pass, cnt_fail );
}

static void test_pk_import_ecc_err(void)
{
    int i;
    int cnt_pass = 0 ;
    int cnt_fail = 0 ;
    int cnt_total = 0 ;
    int ret = 0 ;
    
    printf("\nPrepare Test PK_IMPORT ecc err Start ...\n");
    import_sign_pub_key();
    if(ret != 0)
        goto end;
        
    printf("\nPrepare Test PK_IMPORT ecc err Finish ...\n");
    
    printf("\n\nTest PK_IMPORT ecc err Start ...\n");
    for( i = 0; ; i++ )
    {
        testVectorForPkImportEcc_t testVector;
        
        memcpy(&testVector, &testVector_pkImportEcc_5[i], sizeof(testVectorForPkImportEcc_t));
        replace_key_id(&testVector);
        
        /* terminate */
        if(testVector.test_no <= 0) 
            break ;
        
        /* setup */
        if(testVector.p_setup_func != NULL)
            testVector.p_setup_func(0);

        /* execute specified test no ? */
        if( (exec_specified_test_no != 0) && (exec_specified_test_no != testVector.test_no) )
            continue;   /* skip */
        
        cnt_total ++ ;
        ret = do_oneshot_pk_import( &testVector ) ;

        if(ret == 0)
        {
            cnt_pass++ ;
        }
        else
        {
            cnt_fail++ ;
        }
    }
end:
    printf( "\nTest Result : Executed %d, Passed = %d, Failed = %d\n", cnt_total, cnt_pass, cnt_fail );
}

static int do_oneshot_pk_import(testVectorForPkImportEcc_t *p_TS)
{
    r_icumif_isd_t             *p_ISD                   = (r_icumif_isd_t *)ISD_BUFFER;
    ISD_PK_IMPORT_t            *p_ISD_PARAM             = &p_ISD->prm.PK_IMPORT ;
    r_icumif_service_result_t   expected_service_result = p_TS->expected_service_result;

    int ret = 0 ;

    printf( "\nTest PK_IMPORT(ECC) No.%3d ...\n\n", p_TS->test_no );

    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER

    /* set general parameter */
    p_ISD->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD_PARAM->p_modulus                      = p_TS->test_param.p_modulus;   // =p_pub_point_x
    p_ISD_PARAM->modulus_size_in_bits           = p_TS->test_param.modulus_size_in_bits;    // point_x_size_in_bits
    p_ISD_PARAM->p_pub_exp                      = p_TS->test_param.p_pub_exp;   // =p_pub_point_y
    p_ISD_PARAM->pub_exp_size_in_bits           = p_TS->test_param.pub_exp_size_in_bits;    // point_y_size_in_bits
    p_ISD_PARAM->p_priv_exp                     = p_TS->test_param.p_priv_exp;  // =p_priv_scalar
    p_ISD_PARAM->priv_exp_size_in_bits          = p_TS->test_param.priv_exp_size_in_bits;   // priv_scalar_size_in_bits
    
    p_ISD_PARAM->key_group                      = p_TS->test_param.key_group;
    p_ISD_PARAM->key_id                         = p_TS->test_param.key_id;
    p_ISD_PARAM->key_format                     = p_TS->test_param.key_format;
    
    p_ISD_PARAM->sig_key_group                  = p_TS->test_param.sig_key_group;
    p_ISD_PARAM->sig_key_id                     = p_TS->test_param.sig_key_id;
    p_ISD_PARAM->sig_algo                       = p_TS->test_param.sig_algo;
    p_ISD_PARAM->hash_primitive                 = p_TS->test_param.hash_primitive;
    p_ISD_PARAM->sig_format                     = p_TS->test_param.sig_format;
    p_ISD_PARAM->p_signature                    = p_TS->test_param.p_signature;
    p_ISD_PARAM->sig_size_in_bits               = p_TS->test_param.sig_size_in_bits;
    
    p_ISD_PARAM->curve_id                       = p_TS->test_param.curve_id;
    
    p_ISD_PARAM->p_info                         = p_TS->test_param.p_info;
    p_ISD_PARAM->info_size_in_bytes             = p_TS->test_param.info_size_in_bytes;
    
    p_ISD_PARAM->p_x509_certificate             = p_TS->test_param.p_x509_certificate;
    p_ISD_PARAM->x509_certificate_size_in_bytes = p_TS->test_param.x509_certificate_size_in_bytes;
    
    p_ISD_PARAM->flags.disable_on_debug         = p_TS->test_param.flags.disable_on_debug;
    p_ISD_PARAM->flags.write_protected          = p_TS->test_param.flags.write_protected;
    p_ISD_PARAM->skip_verify                    = p_TS->test_param.skip_verify;

    printf( "Expected code      = %d (%s)\n",   expected_service_result, R_ICUMIF_GetErrorCodeName(expected_service_result) );
    printf( "  key_group        = %s\n",        getKeyGroupName(p_ISD_PARAM->key_group));
    printf( "  key_id           = %d\n",        p_ISD_PARAM->key_id);
    printf( "  key_format       = %s\n",        getNumberFormatName(p_ISD_PARAM->key_format));
    printf( "  point_x_bits     = %d\n",        p_ISD_PARAM->modulus_size_in_bits);
    printf( "  point_y_bits     = %d\n",        p_ISD_PARAM->pub_exp_size_in_bits);
    printf( "  priv_scalar_bits = %d\n",        p_ISD_PARAM->priv_exp_size_in_bits);
    printf( "  curve_id         = %s\n",        getEccCurveName(p_ISD_PARAM->curve_id));
    printf( "  sig_key_group    = %s\n",        getKeyGroupName(p_ISD_PARAM->sig_key_group));
    printf( "  sig_key_id       = %d\n",        p_ISD_PARAM->sig_key_id);
    printf( "  sig_algo         = %s\n",        getSigAlgoName(p_ISD_PARAM->sig_algo));
    printf( "  hash_primitive   = %s\n",        getHashPrimitiveName(p_ISD_PARAM->hash_primitive));
    printf( "  sig_format       = %s\n",        getNumberFormatName(p_ISD_PARAM->sig_format));
    printf( "  sig_bits         = %d\n",        p_ISD_PARAM->sig_size_in_bits);
    printf( "  info_bytes       = %d\n",        p_ISD_PARAM->info_size_in_bytes);
    printf( "  x509_bytes       = %d\n",        p_ISD_PARAM->x509_certificate_size_in_bytes);
    printf( "  write_protected  = %d\n",        p_ISD_PARAM->flags.write_protected );
    printf( "  skip_verify      = %d\n",        p_ISD_PARAM->skip_verify);

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

/* Replace the provisionally defined "number of keys" with the actual one */
static void replace_key_id(testVectorForPkImportEcc_t *p_testVector)
{
    /* replace key_id for import */
    switch (p_testVector->test_param.key_id)
    {
        case NB_ECC_PUB_KEYS - 1:
            p_testVector->test_param.key_id = 0;  break;  /* 0 fixed */
        case NB_ECC_PUB_KEYS:
            p_testVector->test_param.key_id = 0;      break;  /* 0 fixed */
        case NB_ECC_KEYS - 1:
            p_testVector->test_param.key_id = 0;      break;  /* 0 fixed */
        case NB_ECC_KEYS:
            p_testVector->test_param.key_id = 0;          break;  /* 0 fixed */
        default:
            break;
    }
    /* replace key_id for verification signature */
    switch (p_testVector->test_param.sig_key_id)
    {
        case NB_RSA_PUB_KEYS - 1:
            p_testVector->test_param.sig_key_id = 0;    break;  /* 0 fixed */
        case NB_RSA_PUB_KEYS:
            p_testVector->test_param.sig_key_id = 0;        break;  /* 0 fixed */
        default:
            break;
    }
}

static const uint8_t pub_exp[] = {
    0x01,0x00,0x01
};

static const uint8_t rsa3072_modulus[] = {
    0xea,0xa6,0xac,0xb7,0xe2,0xc2,0xaa,0xa9,0x15,0x27,0xe5,0x5b,0x1a,0xb5,0x34,0xcd,
    0x9e,0xe8,0x9c,0x6a,0xc8,0x3b,0xfb,0x72,0x66,0xce,0x4d,0xf5,0xd5,0x35,0x7b,0xe0,
    0x7e,0x88,0x5a,0xaa,0x23,0x4c,0x73,0x0e,0x62,0x83,0x66,0x7e,0x33,0x19,0x04,0xb5,
    0x34,0x40,0x64,0x56,0x3f,0x0d,0x49,0x0f,0x57,0x5e,0xa6,0x02,0x92,0xa5,0x7c,0x44,
    0x1f,0x61,0xf1,0x74,0x2b,0xe4,0x27,0x93,0x23,0xd9,0x59,0xb5,0x81,0x34,0x88,0xca,
    0x93,0xe4,0x74,0x7a,0x4b,0x49,0x7c,0x6f,0x4e,0x9b,0xf2,0x4a,0xb2,0x63,0xd7,0x2a,
    0xef,0x8b,0xae,0xf3,0xae,0x20,0x94,0xa3,0x91,0x97,0xfb,0xc3,0xb0,0x45,0x12,0x4d,
    0x9b,0xd2,0xd5,0x26,0x91,0x72,0x9f,0xa7,0x4e,0x6d,0x58,0x70,0x86,0xb7,0x82,0xdd,
    0x98,0x20,0x6b,0xd8,0x4f,0x76,0x60,0x2f,0xce,0x8d,0x06,0x8c,0xcf,0x84,0x1a,0x65,
    0x9f,0x5d,0xb1,0x2b,0x23,0xce,0xe1,0x8e,0xf3,0xad,0x3f,0x13,0xa4,0xd1,0xfd,0xff,
    0xbf,0x74,0xd8,0x48,0xb0,0x6e,0xd2,0x4e,0x7a,0x1a,0xdb,0x88,0xbb,0x72,0x67,0xab,
    0x45,0x98,0x04,0x46,0xc0,0x43,0xad,0x26,0x92,0x4d,0x21,0xc1,0x30,0x14,0x84,0xcb,
    0x4d,0x99,0xbf,0x78,0x32,0xf0,0xdf,0x84,0x9c,0x95,0x8a,0xfc,0xda,0x24,0x69,0x86,
    0x9e,0xb9,0x4c,0x28,0xe3,0x4a,0xe6,0xc3,0x23,0x4e,0x2f,0xbd,0xdd,0x21,0xea,0x54,
    0x51,0xc3,0x35,0xad,0x06,0x54,0x44,0x61,0x05,0x77,0x72,0xa7,0xc7,0xd0,0x09,0x70,
    0x77,0xb6,0x6c,0x18,0xf0,0x5c,0xe1,0x8a,0x67,0x1a,0xce,0xa1,0xf7,0xf1,0x45,0xaa,
    0x63,0xcd,0x4b,0x1f,0x50,0x94,0x1c,0x60,0x9f,0xbe,0x19,0xb0,0xed,0x68,0xe1,0x15,
    0x99,0x38,0x9f,0xcd,0x4a,0x2f,0xc5,0x86,0x9c,0xdc,0xd5,0x1d,0x7f,0xff,0x51,0xea,
    0x31,0x68,0x87,0x0e,0x58,0x40,0x01,0x77,0x2a,0x3f,0xd7,0x7d,0xc3,0x45,0x3f,0xd2,
    0x82,0xe3,0xe3,0x3a,0xa1,0x02,0x2d,0x54,0xee,0x8e,0xc4,0xb4,0x0e,0x1f,0x51,0xf3,
    0x29,0x66,0xb4,0x58,0x47,0xce,0xd5,0xcc,0x8b,0xf5,0x0c,0xb8,0x50,0xe0,0x0f,0x16,
    0x14,0xa6,0x1b,0x4d,0x25,0x75,0x16,0xb0,0x27,0xa2,0x83,0xef,0x8f,0xd3,0xc1,0x62,
    0xc4,0xd7,0xa1,0x1d,0xaa,0xfb,0xb6,0xcc,0xae,0x78,0x17,0x42,0x8b,0x86,0xae,0x55,
    0x7c,0xf3,0x9f,0x63,0xeb,0xac,0xda,0x84,0x4a,0xa2,0x1a,0x4e,0x8b,0xd4,0x11,0xa9,
};
static const uint8_t rsa3072_modulus_for_x509[] = {
    0xe0,0x7a,0x40,0x1c,0x05,0xec,0x66,0xc9,0xb1,0x6b,0x72,0x3d,0x78,0xad,0x6b,0xe2,
    0xfd,0xf1,0x28,0xef,0x8e,0x30,0xdc,0xd3,0xb7,0x02,0x42,0x01,0xe9,0xde,0x65,0x39,
    0x6c,0x78,0x0c,0xb1,0x38,0xdc,0x81,0xbc,0x33,0xa1,0xf6,0x9b,0x05,0x3a,0x5f,0x12,
    0x63,0x96,0xa7,0x1e,0xc8,0xbf,0xab,0xef,0xc5,0x63,0x65,0x9f,0x05,0x6e,0xd3,0x15,
    0x9b,0xe1,0xd6,0x31,0x48,0x3d,0xe9,0xf3,0x7f,0x09,0x30,0xe8,0x4a,0x5c,0xea,0x4b,
    0xfb,0x7b,0x66,0x91,0x8d,0xa4,0xcf,0x11,0xce,0xe1,0x61,0x27,0x79,0x9b,0x0a,0x33,
    0x9d,0x61,0x06,0x90,0xbe,0xd9,0xc5,0x8c,0x01,0x98,0x79,0x0d,0xdf,0x9b,0x23,0x78,
    0xc7,0x9b,0xbc,0x3e,0xe5,0xba,0xcb,0x4a,0x53,0x1e,0xfa,0x32,0x1c,0xd7,0x06,0xe8,
    0xee,0xe2,0x85,0xeb,0xd3,0xca,0xb1,0x79,0xd0,0xc0,0x6e,0x7f,0x4c,0x42,0xa3,0xd6,
    0xdb,0xf8,0x4c,0xef,0xb1,0xb7,0x63,0x9e,0x2f,0x59,0xc7,0x25,0xe9,0x74,0x9a,0xaa,
    0x05,0xd6,0x41,0x67,0x5d,0x5a,0x75,0xd2,0x13,0x46,0xcc,0xe9,0x10,0x72,0x94,0x1c,
    0x85,0x2f,0x34,0xf5,0x39,0x17,0x67,0x45,0xa2,0x37,0xce,0xe5,0x82,0x65,0xaa,0x46,
    0x0f,0xb9,0xdb,0x6a,0xbb,0xa2,0x9a,0x9c,0x5e,0x7d,0xd6,0xc6,0xd4,0xa6,0x62,0x5f,
    0x02,0x38,0xca,0x9e,0x1c,0x55,0x94,0xb4,0x4e,0x1f,0xb9,0x7f,0x3a,0x3b,0xdf,0xc9,
    0x7f,0xb3,0x59,0xda,0x50,0xd3,0x6f,0x90,0x93,0x21,0x98,0x07,0x5e,0x07,0x0c,0xed,
    0x9f,0x13,0xfd,0x75,0x1a,0xe9,0xdf,0x07,0x8f,0xf5,0x8d,0xb6,0x1d,0x1f,0xf7,0x7b,
    0xf5,0xf2,0xfe,0xe3,0x2e,0x74,0xd3,0x02,0x9a,0x81,0x51,0xa5,0xbf,0x76,0x7c,0x8f,
    0xd7,0x9a,0xa0,0x7a,0x1b,0xf8,0x29,0xe2,0x7b,0xe1,0x0e,0x54,0x49,0x0c,0x99,0xd8,
    0x4c,0x4c,0x78,0x64,0xa7,0x6c,0x87,0x17,0x2e,0xcd,0x4c,0xba,0x26,0xda,0x7a,0xc8,
    0xc8,0x7f,0x46,0x12,0x78,0x65,0xb3,0x07,0x47,0x72,0x7f,0xd6,0x18,0x20,0x50,0x85,
    0xe2,0x7a,0x53,0xf2,0x41,0x70,0xe4,0xf9,0xea,0xce,0x5a,0x59,0x2f,0xb9,0x7e,0x3c,
    0x23,0x59,0xf5,0x38,0x14,0x52,0xbb,0xe5,0x4e,0x2a,0xef,0x64,0x4d,0x1c,0x6b,0x15,
    0xf7,0x6c,0x83,0x20,0x88,0xfc,0x26,0x37,0xba,0xca,0xe9,0x74,0xb1,0x78,0x7d,0x14,
    0x7e,0x36,0x46,0xf9,0xc1,0x85,0x20,0xeb,0x9b,0xd5,0x45,0xea,0x2b,0x03,0x91,0xc5
};
static void import_sign_pub_key(void)
{
    int i;
    r_icumif_isd_t *p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\n import sign public key...\n");

    for(i = 0; i < 2; i++)
    {
        // initialize the global icum service header
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER

        p_ISD->service_id     = SERVICE_03_PK_IMPORT;
        p_ISD->p_callbackfunc = 0;
        p_ISD->job_id         = 0;

        p_ISD->prm.PK_IMPORT.p_pub_exp              = &pub_exp[0];
        p_ISD->prm.PK_IMPORT.pub_exp_size_in_bits   = 8 * sizeof(pub_exp);
        p_ISD->prm.PK_IMPORT.priv_exp_size_in_bits  = 0;
        p_ISD->prm.PK_IMPORT.key_format             = FORMAT_BINARY_PLAIN;
        p_ISD->prm.PK_IMPORT.flags.disable_on_debug = 0;
        p_ISD->prm.PK_IMPORT.skip_verify            = 1;

        switch (i)
        {
        case 0:
            p_ISD->prm.PK_IMPORT.p_modulus              = &rsa3072_modulus[0];
            p_ISD->prm.PK_IMPORT.modulus_size_in_bits   = 8 * sizeof(rsa3072_modulus);
            p_ISD->prm.PK_IMPORT.key_group              = KEY_GRP_RSA_PUB;
            p_ISD->prm.PK_IMPORT.key_id                 = 0;
            break;
        case 1:
            p_ISD->prm.PK_IMPORT.p_modulus              = &rsa3072_modulus_for_x509[0];
            p_ISD->prm.PK_IMPORT.modulus_size_in_bits   = 8 * sizeof(rsa3072_modulus_for_x509);
            p_ISD->prm.PK_IMPORT.key_group              = KEY_GRP_RSA_PUB;
            p_ISD->prm.PK_IMPORT.key_id                 = 1;
            break;
        }
        // trigger the security service
        EXEC_SERVICE_WITH_TIMESTAMP(p_ISD);
    }
    printf("\nDone!\n");
}
