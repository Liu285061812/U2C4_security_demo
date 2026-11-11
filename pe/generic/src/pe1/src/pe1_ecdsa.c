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
#include "pe1_ecdsa.h"

// ********************************************************************************
// Public constants
// ********************************************************************************

static int16_t      exec_specified_test_no = 0;

static int do_oneshot_ecdsa_verify(testVectorForEcdsaVerify_t *p_TS);
static int do_oneshot_pk_import(testVectorForPkImport_t *p_TS);
static void ecdsa_verify(void);

static void ecdsa_verify(void)
{
    int i;
    int cnt_pass = 0;
    int cnt_fail = 0;
    int cnt_total = 0;
    int ret = 0;
    
    printf("\nTest ECDSA verify normal case Start ...\n\n");
    for( i = 0; ; i++ )
    {
        testVectorForEcdsaVerify_t testVector;
        
        memcpy(&testVector, &testVector_ecdsaVerify[i], sizeof(testVectorForEcdsaVerify_t));
        
        /* terminate */
        if(testVector.test_no <= 0)
            break ;
        
        /* setup */
        if(testVector.p_setup_func != NULL)
        {
            ret = testVector.p_setup_func(&testVector_pkImport[i]);
            if(ret != 0)
            {
                printf("\nSetup import key FAILED !! ...\n");
                break;
            }
        }
        /* execute specified test no ? */
        if(exec_specified_test_no != 0 && exec_specified_test_no != testVector.test_no)
            continue;   /* skip */
        
        cnt_total++;
        ret = do_oneshot_ecdsa_verify(&testVector);

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

static void ecdsa_veri_dbg(void)
{
    int i;
    int cnt_pass = 0;
    int cnt_fail = 0;
    int cnt_total = 0;
    int ret = 0;
    
    int debugger_connect = (R_ICUMIF_GetStatus())->EXT_DEBUGGER;
    
    printf("\nTest ECDSA verify disable_on_debug Start ...\n\n");
    
    printf("\n Now debugfer is [%s]\n", (debugger_connect == 1) ? "Connect" : "Not Connect" );
    for( i = 0; ; i++ )
    {
        testVectorForEcdsaVerify_t testVector;
        
        memcpy(&testVector, &testVector_ecdsaVerify_dbg[i], sizeof(testVectorForEcdsaVerify_t));
        
        /* terminate */
        if(testVector.test_no <= 0)
            break ;
        
        /* setup */
        if(testVector.p_setup_func != NULL)
        {
            ret = testVector.p_setup_func(&testVector_pkImport_dbg[i]);
            if(ret != 0)
            {
                printf("\nSetup import key FAILED !! ...\n");
                break;
            }
        }
        /* execute specified test no ? */
        if(exec_specified_test_no != 0 && exec_specified_test_no != testVector.test_no)
            continue;   /* skip */
        
        cnt_total++;
        if(debugger_connect == 0)
        {
            /* If debubber is not connected, rewrite testNo and expected_service_result */
            testVector.test_no = 50;
            testVector.expected_service_result = SERV_OK;
        }
        ret = do_oneshot_ecdsa_verify(&testVector);

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


static int do_oneshot_ecdsa_verify(testVectorForEcdsaVerify_t *p_TS)
{
    r_icumif_isd_t             *p_ISD                   = (r_icumif_isd_t *)ISD_BUFFER;
    ISD_ECDSA_VERIFY_t         *p_ISD_PARAM             = &p_ISD->prm.ECDSA_VERIFY;
    r_icumif_service_result_t   expected_service_result = p_TS->expected_service_result;
    r_verif_result_t            expected_verif_result   = p_TS->expected_verif_result;

    int ret = 0 ;

    printf( "\nTest ECDSA_VERIFY No.%3d ...\n\n", p_TS->test_no );

    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the ISD_BUFFER

    /* set general parameter */
    p_ISD->service_id     = SERVICE_01_ECDSA_VERIFY;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id         = 0;

    p_ISD_PARAM->p_source                       = p_TS->test_param.p_source;
    p_ISD_PARAM->signature[0].p_data            = p_TS->test_param.signature[0].p_data;
    p_ISD_PARAM->signature[1].p_data            = p_TS->test_param.signature[1].p_data;
    p_ISD_PARAM->source_size_in_bytes           = p_TS->test_param.source_size_in_bytes;
    p_ISD_PARAM->signature_size_in_bits[0]      = p_TS->test_param.signature_size_in_bits[0];
    p_ISD_PARAM->signature_size_in_bits[1]      = p_TS->test_param.signature_size_in_bits[1];
    p_ISD_PARAM->hash_primitive                 = p_TS->test_param.hash_primitive;
    p_ISD_PARAM->signature_format               = p_TS->test_param.signature_format;
    p_ISD_PARAM->key_group                      = p_TS->test_param.key_group;
    p_ISD_PARAM->key_id                         = p_TS->test_param.key_id;
    p_ISD_PARAM->job_slice_hash                 = p_TS->test_param.job_slice_hash;
    p_ISD_PARAM->job_slice_point_mul            = p_TS->test_param.job_slice_point_mul;
    p_ISD_PARAM->is_precalculated_hash          = p_TS->test_param.is_precalculated_hash;

    printf( "Expected code          = %d (%s)\n",   expected_service_result, R_ICUMIF_GetErrorCodeName(expected_service_result));
    
    printf( "  p_source             = (%08x)\n",    p_ISD_PARAM->p_source);
    printf( "  source_bytes         = %d\n",        p_ISD_PARAM->source_size_in_bytes );
    printf( "  p_signature[r]       = (%08x)\n",    p_ISD_PARAM->signature[0].p_data);
    printf( "  p_signature[s]       = (%08x)\n",    p_ISD_PARAM->signature[1].p_data);
    printf( "  key_group            = %s\n",        getKeyGroupName(p_ISD_PARAM->key_group));
    printf( "  key_id               = %d\n",        p_ISD_PARAM->key_id);
    printf( "  sig_format           = %s\n",        getNumberFormatName(p_ISD_PARAM->signature_format));
    printf( "  sig_size_in_bits[r]  = %d\n",        p_ISD_PARAM->signature_size_in_bits[0]);
    printf( "  sig_size_in_bits[s]  = %d\n",        p_ISD_PARAM->signature_size_in_bits[1]);
    printf( "  hash_primitive       = %s\n",        getHashPrimitiveName(p_ISD_PARAM->hash_primitive));
    printf( "  job_slice_hash       = %d\n",        p_ISD_PARAM->job_slice_hash);
    printf( "  job_slice_point_mul  = %d\n",        p_ISD_PARAM->job_slice_point_mul);
    printf( "  is_precalculated_hash= %d\n",        p_ISD_PARAM->is_precalculated_hash);

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
            printf("verification result : ");
            if (p_ISD_PARAM->verification_result == expected_verif_result)
            {
                printf("Expected. (PASS)! [%s]\n", getVerifResultName(p_ISD_PARAM->verification_result));
            }
            else
            {
                ret = 1;
                printf( "NOT EXPECTED !! [%s]\n", getVerifResultName(p_ISD_PARAM->verification_result));
            }
        }
        else
        {
            printf( "Expected. [%s]\n", R_ICUMIF_GetErrorCodeName(expected_service_result));
        }
    }
    else
    {
        ret = 1;
        printf( "NOT EXPECTED !! [%s]\n", R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }

    return (ret);
}

static int setup_pk_import(const testVectorForPkImport_t *p_TestVector)
{
    testVectorForPkImport_t testVector;
    
    memcpy(&testVector, p_TestVector, sizeof(testVectorForPkImport_t));
    return do_oneshot_pk_import(&testVector);
}


static int do_oneshot_pk_import(testVectorForPkImport_t *p_TS)
{
    r_icumif_isd_t             *p_ISD                   = (r_icumif_isd_t *)ISD_BUFFER;
    ISD_PK_IMPORT_t            *p_ISD_PARAM             = &p_ISD->prm.PK_IMPORT ;

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

    /* trigger the security service */
    trigger_service_and_wait(p_ISD);

    if(p_ISD->service_result != SERV_OK)
    {
        printf("service result : %s\n", R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        return 1;
    }
    else
    {
        return 0;
    }
}

static void ecdsa_veri_cluster(void)
{
    r_icumif_isd_t *p_ISD0;
    r_icumif_isd_t *p_ISD;
    int n = 0;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 2));    // clear the ISD_BUFFER

    p_ISD0 = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD = p_ISD0 + 1;

    printf("\n*------ Test No. 51\n");
    printf("\necdsa_veri_cluster...\n\n");


    p_ISD0->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD0->p_callbackfunc = 0;
    p_ISD0->job_id         = 0;

    p_ISD0->prm.PK_IMPORT.key_group                = KEY_GRP_ECC_PUB;
    p_ISD0->prm.PK_IMPORT.key_id                   = 0;

    p_ISD0->prm.PK_IMPORT.p_pub_point_x            = point_x_sha1_P256;
    p_ISD0->prm.PK_IMPORT.p_pub_point_y            = point_y_sha1_P256;
    p_ISD0->prm.PK_IMPORT.hash_primitive           = HASH_PRIMITIVE_SHA1;

    p_ISD0->prm.PK_IMPORT.point_x_size_in_bits     = 256;
    p_ISD0->prm.PK_IMPORT.point_y_size_in_bits     = 256;
    p_ISD0->prm.PK_IMPORT.priv_scalar_size_in_bits = 0;

    p_ISD0->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
    p_ISD0->prm.PK_IMPORT.curve_id                 = ECC_CURVE_NIST_P256;
    p_ISD0->prm.PK_IMPORT.sig_key_id               = 0;

    p_ISD0->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
    p_ISD0->prm.PK_IMPORT.skip_verify              = 1;    /* without valification */
    p_ISD0->prm.PK_IMPORT.flags.disable_on_debug   = 0;
    p_ISD0->prm.PK_IMPORT.flags.write_protected    = 0;

    // trigger the security service
    trigger_service_and_wait(p_ISD0);

    printf("\nECC key pair import Done!\n");


    p_ISD->service_id = SERVICE_01_ECDSA_VERIFY;
    p_ISD->p_callbackfunc = 0;
    p_ISD->job_id = 0;

    p_ISD->prm.ECDSA_VERIFY.key_group = KEY_GRP_ECC_PUB;
    p_ISD->prm.ECDSA_VERIFY.key_id = n;
    p_ISD->prm.ECDSA_VERIFY.p_source  = (void *)ecdsa_msg_sha1;                       // just a test message!
    p_ISD->prm.ECDSA_VERIFY.source_size_in_bytes  = 128;
    p_ISD->prm.ECDSA_VERIFY.hash_primitive = HASH_PRIMITIVE_SHA1;
    ;
    p_ISD->prm.ECDSA_VERIFY.job_slice_hash = 0;
    p_ISD->prm.ECDSA_VERIFY.job_slice_point_mul = 1;
    p_ISD->prm.ECDSA_VERIFY.signature[0].p_data = (void *)ecdsa_hex_sig0_sha1;
    p_ISD->prm.ECDSA_VERIFY.signature[1].p_data = (void *)ecdsa_hex_sig1_sha1;
    p_ISD->prm.ECDSA_VERIFY.signature_size_in_bits[0] = 256;
    p_ISD->prm.ECDSA_VERIFY.signature_size_in_bits[1] = 256;
    p_ISD->prm.ECDSA_VERIFY.signature_format = FORMAT_STRING_HEXA;

    printf("\nStart verification cluster check...\n\n");

    // trigger the security service
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();

    // fetch execution time in vrun_chrono_xxx variables
    vrun_chrono_gettimestamp();

    printf("Processed in : ");
    PCHRONO();
    NL();
    FLUSH();
    
    if (p_ISD->service_result == SERV_OK)
    {
        printf("SERV OK\n\n");
    }
    else
    {
        printf("SERV NG\n\n");
        goto end;
    }

    printf("Verification result : ");
    if (p_ISD->prm.ECDSA_VERIFY.verification_result == VERIFICATION_PASS)
    {
        printf("PASS!\n\n");
    }
    else
    {
        printf("*FAIL*!\a\n\n");
    }
end:
    ;

}


static void ecdsa_veri_multi(void)
{
    r_icumif_isd_t *p_ISD0;
    r_icumif_isd_t *p_ISD1;
    r_icumif_isd_t *p_ISD2;
    int n = 0;

    // initialize the global icum service header
    memset(ISD_BUFFER, 0, (SIZE_OF_ISD_BUFFER * 2));    // clear the ISD_BUFFER

    p_ISD0 = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD1 = p_ISD0 + 1;
    p_ISD2 = p_ISD1 + 1;

    printf("\n*------ Test No. 65\n");
    printf("\nTesting ECDSA verify [Multi Start-up...\n\n");

    p_ISD0->service_id     = SERVICE_03_PK_IMPORT;
    p_ISD0->p_callbackfunc = 0;
    p_ISD0->job_id         = 0;

    p_ISD0->prm.PK_IMPORT.key_group                = KEY_GRP_ECC_PUB;
    p_ISD0->prm.PK_IMPORT.key_id                   = 0;

    p_ISD0->prm.PK_IMPORT.p_pub_point_x            = point_x_sha1_P256;
    p_ISD0->prm.PK_IMPORT.p_pub_point_y            = point_y_sha1_P256;
    p_ISD0->prm.PK_IMPORT.hash_primitive           = HASH_PRIMITIVE_SHA1;

    p_ISD0->prm.PK_IMPORT.point_x_size_in_bits     = 256;
    p_ISD0->prm.PK_IMPORT.point_y_size_in_bits     = 256;
    p_ISD0->prm.PK_IMPORT.priv_scalar_size_in_bits = 0;

    p_ISD0->prm.PK_IMPORT.key_format               = FORMAT_BINARY_PLAIN;
    p_ISD0->prm.PK_IMPORT.curve_id                 = ECC_CURVE_NIST_P256;
    p_ISD0->prm.PK_IMPORT.sig_key_id               = 0;

    p_ISD0->prm.PK_IMPORT.sig_algo                 = SIGALGO_NONE;
    p_ISD0->prm.PK_IMPORT.skip_verify              = 1;    /* without valification */
    p_ISD0->prm.PK_IMPORT.flags.disable_on_debug   = 0;
    p_ISD0->prm.PK_IMPORT.flags.write_protected    = 0;

    // trigger the security service
    trigger_service_and_wait(p_ISD0);
    
    printf("\nECC key pair import Done!\n");

    p_ISD1->service_id = SERVICE_01_ECDSA_VERIFY;
    p_ISD1->p_callbackfunc = 0;
    p_ISD1->job_id = 0;
    p_ISD1->prm.ECDSA_VERIFY.key_group = KEY_GRP_ECC_PUB;
    p_ISD1->prm.ECDSA_VERIFY.key_id = n;
    p_ISD1->prm.ECDSA_VERIFY.p_source  = (void *)ecdsa_msg_sha1;                       // just a test message!
    p_ISD1->prm.ECDSA_VERIFY.source_size_in_bytes  = 20;
    p_ISD1->prm.ECDSA_VERIFY.hash_primitive = HASH_PRIMITIVE_SHA1;
    p_ISD1->prm.ECDSA_VERIFY.job_slice_hash = 0;
    p_ISD1->prm.ECDSA_VERIFY.job_slice_point_mul = 1;
    p_ISD1->prm.ECDSA_VERIFY.signature[0].p_data = (void *)ecdsa_hex_sig0_sha1;
    p_ISD1->prm.ECDSA_VERIFY.signature[1].p_data = (void *)ecdsa_hex_sig1_sha1;
    p_ISD1->prm.ECDSA_VERIFY.signature_size_in_bits[0] = 256;
    p_ISD1->prm.ECDSA_VERIFY.signature_size_in_bits[1] = 256;
    p_ISD1->prm.ECDSA_VERIFY.signature_format =FORMAT_STRING_HEXA;

    p_ISD2->service_id = SERVICE_01_ECDSA_VERIFY;
    p_ISD2->p_callbackfunc = 0;
    p_ISD2->job_id = 0;
    p_ISD2->prm.ECDSA_VERIFY.key_group = KEY_GRP_ECC_PUB;
    p_ISD2->prm.ECDSA_VERIFY.key_id = n;
    p_ISD2->prm.ECDSA_VERIFY.p_source  = (void *)ecdsa_msg_sha1;                       // just a test message!
    p_ISD2->prm.ECDSA_VERIFY.source_size_in_bytes  = 20;
    p_ISD2->prm.ECDSA_VERIFY.hash_primitive = HASH_PRIMITIVE_SHA1;
    p_ISD2->prm.ECDSA_VERIFY.job_slice_hash = 0;
    p_ISD2->prm.ECDSA_VERIFY.job_slice_point_mul = 1;
    p_ISD2->prm.ECDSA_VERIFY.signature[0].p_data = (void *)ecdsa_hex_sig0_sha1;
    p_ISD2->prm.ECDSA_VERIFY.signature[1].p_data = (void *)ecdsa_hex_sig1_sha1;
    p_ISD2->prm.ECDSA_VERIFY.signature_size_in_bits[0] = 256;
    p_ISD2->prm.ECDSA_VERIFY.signature_size_in_bits[1] = 256;
    p_ISD2->prm.ECDSA_VERIFY.signature_format =FORMAT_STRING_HEXA;

    printf("\nStart verification...\n[Multi Start-up]\n\n");

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

}

void show_submenu_for_ecdsa_verify(void)
{
    int n;
    
    while (1)
    {
        printf("\n\n| select ECDSA Verify Test\n");
        printf("|---------------------------------\n");
        printf("| 0  : verify normal\n");
        printf("| 1  : debugger check\n");
        printf("| 2  : cluster check\n");
        printf("| 3  : Multi start-up\n");
        printf("| 9  : Set exexute specified test No \n");
        printf("| q  : Quit\n\n");
        printf("  ?> ");
        
        vrun_console_get();

        if (*(char *)vrun_iobuff_readparam(0) == 'q')
        {
            break;
        }
        else
        {
            sscanf(vrun_iobuff_readparam(0), "%i", &n);
        }
        
        switch (n)
        {
            case 0:
                ecdsa_verify();
                break;
            case 1:
                ecdsa_veri_dbg();
                break;
            case 2:
                ecdsa_veri_cluster();
                break;
            case 3:
                ecdsa_veri_multi();
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
