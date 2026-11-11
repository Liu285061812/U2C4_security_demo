/******************************************************************************/
/* Component Name   pe2_heat_run_parallel                                     */
/******************************************************************************/
/* Product      : ICU-M Low Level Driver (Demo Part)                          */
/******************************************************************************/
/***********************************************************************************************************************
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
* 
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* www.renesas.com/disclaimer
*
* Copyright (C) 2021-2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

// ********************************************************************************
// Headers
// ********************************************************************************

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#undef NULL
#include "r_icumif_api.h"
#include "r_icumif_pub.h"

#include "pe_reg.h"
#include "pe_target_device.h"

#include "pe2.h"
#include "pe2.int.h"
#include "shared.h"
#include "vrun.api.h"

#include "heat_run_parallel_pe.h"
#include "pe_target_device.h"

// ********************************************************************************
// Define
// ********************************************************************************
/* #define FSW_FASTCMAC_FIXEDPE */

// ********************************************************************************
// Local constants
// ********************************************************************************

#if   (D_PEID == 2)
#pragma ghs section bss=".start.gRAM_PE2"
#elif (D_PEID == 3)
#pragma ghs section bss=".start.gRAM_PE3"
#elif (D_PEID == 4)
#pragma ghs section bss=".start.gRAM_PE4"
#elif (D_PEID == 5)
#pragma ghs section bss=".start.gRAM_PE5"
#elif (D_PEID == 6)
#pragma ghs section bss=".start.gRAM_PE6"
#endif

#define PASTER(x, y)                        x ## y
#define EVALUATOR(x, y)                     PASTER(x, y)
#define CNT_RESULT_PE(n)                    EVALUATOR(cnt_result_pe, n)
#define CNT_TRIGGER_SERVICE_REQUEST_PE(n)   EVALUATOR(cnt_trigger_service_request_pe, n)
#define CNT_RECIEVE_SERVICE_RESPONSE_PE(n)  EVALUATOR(cnt_recieve_service_response_pe, n)
#define CNT_REQUEST_QUEUE_FULL_PE(n)        EVALUATOR(cnt_request_queue_full_pe, n)
#define CNT_IT_LIB_NOT_INIT_PE(n)           EVALUATOR(cnt_if_lib_not_init_pe, n)
#define CNT_SOFTWARE_ERROR_PE(n)            EVALUATOR(cnt_software_error_pe, n)

static unsigned long long cnt_recieve_service_response;
static unsigned long long cnt_serv_job_queue_full;
static unsigned long long cnt_trigger_service_request;
static unsigned long long cnt_request_queue_full;
static unsigned long long cnt_if_lib_not_init;
static unsigned long long cnt_software_error;
static uint32_t  cnt_result[TEST_NO_MAX][79];
unsigned int       sw_update_she;
unsigned int       sw_key_mng;
unsigned int       sw_err_detect;
uint8_t            sw_gc_warning;

extern uint16_t notification_from_pe1;

extern void ConvertStrToArrayAddr(unsigned int *dst, char *src, unsigned int len);
extern void ConvertStrToHex(char *dst, char *src, unsigned int len);
extern void ConvertStrToDec(unsigned int *dst, char *src, unsigned int len);
extern int  ConvertStrToInteger(int *dst, char *src, unsigned int len);
extern int  ConvertStrToEnum(int *dst, char *src, unsigned int len, const cvt_enum_tbl* cvt_tbl);

// ********************************************************************************
// Define
// ********************************************************************************
#define JOB_QUE_FULL_COUNT (25)      // P_ISD[] full count
#define JOB_QUE_WAIT_COUNT (60*1000) /* ms */

// ********************************************************************************
// Select service priority
// ********************************************************************************
void select_service_pri(test_type_t test_type, r_icumif_isd_t *p_ISD)
{
    uint8_t priority_cnt = 4;
    uint8_t priority = 0;

    if (RANDOM_TEST == test_type)
    {
        priority = rand() % priority_cnt;

        switch (priority)
        {
        case 0:
            p_ISD->service_priority  = R_ICUMIF_SERV_PRIORITY_HIGHEST;
            break;
        case 1:
            p_ISD->service_priority  = R_ICUMIF_SERV_PRIORITY_HIGH;
            break;
        case 2:
            p_ISD->service_priority  = R_ICUMIF_SERV_PRIORITY_NORMAL;
            break;
        case 3:
            p_ISD->service_priority  = R_ICUMIF_SERV_PRIORITY_LOWEST;
            break;
        default:
            p_ISD->service_priority  = R_ICUMIF_SERV_PRIORITY_NORMAL;
            break;
        }
    }
    else
    {
        p_ISD->service_priority  = R_ICUMIF_SERV_PRIORITY_NORMAL;
    }
}

// ********************************************************************************
// Test functions
// ********************************************************************************
/* Start gc */
void start_gc_wait(r_icumif_isd_t *p_isd)
{
    int32_t errno;

    if (sw_gc_warning > 0u)
    {
        memset(p_isd, 0x00, sizeof(r_icumif_isd_t));

        p_isd->service_id               = SERVICE_04_ICUMDF_GC;
        p_isd->p_callbackfunc           = 0;
        p_isd->job_id                   = 0;
        p_isd->service_priority         = R_ICUMIF_SERV_PRIORITY_HIGH;
        p_isd->prm.SECURE_FLASH_MGMT.gc_operation = GC_ALL_BLOCK;
        /* gc all block service request */
        errno = R_ICUMIF_ServiceRequest(p_isd);
        /* if service request is ok, wait until response */
        if (R_ICUMIF_ER_OK == errno)
        {
            /* wait service responce */
            while (1)
            {
                if ((SERV_REQUEST > p_isd->service_result)
                 && (R_ICUMIF_REQRES_PE_RES_DEQUE == p_isd->req_res_status))
                {
                    /* loop end */
                    break;
                }
            }
            /* if service responce is ok, gc request flag clear */
            if (SERV_OK == p_isd->service_result)
            {
                sw_gc_warning = 0u;
            }
        }
    }
}

// Result Check
int ResultCheck( r_icumif_isd_t  *p_isd )
{
    int ret = 0;

    switch (p_isd->service_result){
    case SERV_OK :
    case SERV_INSTANCE_ALREADY_RUNNING :
        break;
    case SERV_JOB_QUEUE_FULL :
        cnt_serv_job_queue_full++;
        break;
    case SERV_NVM_DATA_FULL_WARNING1:
    case SERV_NVM_DATA_FULL_WARNING2:
        sw_gc_warning = 1;
        break;
    default :
        sw_err_detect = 1;
        ret = -1;
        break;
    }
    return( ret );
}
// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (AES_11, job_slice = 3, msg_size_in_bits = 1279, priority normal)
// ********************************************************************************
static void isd_call_back_cmac_job_slice_3(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CMAC_JOB_SLICE_3;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CMAC.p_cmac, test_cmac_value_LOREM_IPSUM_1279bit, sizeof(test_cmac_value_LOREM_IPSUM_1279bit)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}
// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (no job_slice)
// ********************************************************************************
static void isd_call_back_cmac_job_slice_0(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CMAC_JOB_SLICE_0;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CMAC.p_cmac, test_cmac_value_128bits, sizeof(test_cmac_value_128bits)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (job_slice = 1)
// ********************************************************************************
static void isd_call_back_cmac_job_slice_1(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CMAC_JOB_SLICE_1;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CMAC.p_cmac, test_cmac_value_128bits, sizeof(test_cmac_value_128bits)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_HASH
// ********************************************************************************
static void isd_call_back_hash(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_HASH;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.HASH.p_hash, test_hash_data, p_isd->prm.HASH.hash_size_in_bytes))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_03_AES_KEY_UPDATE_PLAIN (AES_9)
// ********************************************************************************
static void isd_call_back_aes_update_plain(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_KEY_UPDATE_PLAIN;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_03_AES_KEY_UPDATE_PLAIN (AES_10)
// ********************************************************************************
static void isd_call_back_aes_update_plain_another_key(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_KEY_UPDATE_PLAIN_ANOTHER_KEY;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_01_RSA_VERIFY (no job_slice)
// ********************************************************************************
static void isd_call_back_rsa_verify_job_slice_hash_0(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_RSA_VERIFY_JOB_SLICE_0;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (p_isd->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
        {
            // OK
            cnt_result[i][77] += 1;
            cnt_result[TEST_NO_RSA_SIGN][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            cnt_result[TEST_NO_RSA_SIGN][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_RSA_VERIFY (job_slice_hash = 1)
// ********************************************************************************
static void isd_call_back_rsa_verify_job_slice_hash_1(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_RSA_VERIFY_JOB_SLICE_1;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (p_isd->prm.RSA_VERIFY.verification_result == VERIFICATION_PASS)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER
// ********************************************************************************
static void isd_call_back_aes_cipher(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_CIPHER (XTS, AES_14, AES_15)
// ********************************************************************************
static void isd_call_back_aes_cipher_xts(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_XTS;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_xts, (p_isd->prm.AES_CIPHER.size.data_size_in_bits / 8)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (GCM, DEC, AES_16)
// ********************************************************************************
static void isd_call_back_aes_auth_cipher(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_GCM;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (p_isd->prm.AES_AUTH_CIPHER.verification_result == VERIFICATION_PASS)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (CCM,ENC,AES_17)
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_ccm_enc(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_CCM_ENC;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_auth_tag_ccm , p_isd->prm.AES_AUTH_CIPHER.p_auth_tag, (p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (CCM,DEC,AES_17)
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_ccm_dec(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_CCM_DEC;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (VERIFICATION_PASS == p_isd->prm.AES_AUTH_CIPHER.verification_result)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_RSA_SIGN (RSA_2)
// ********************************************************************************
static void isd_call_back_rsa_sign(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_RSA_SIGN;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_01_RSA_SIGN (RSA_6)
// ********************************************************************************
static void isd_call_back_rsa_sign_2(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_RSA_SIGN_2;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

/**********************************************************************************
* Call Back Func SERVICE_02_RAND_INIT_RNG
***********************************************************************************/
static void isd_call_back_rand_init_rng(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_RAND_INIT_RNG;

    cnt_recieve_service_response++;

    if (SERV_NVM_REJECTED != p_isd->service_result)
    {
        cnt_result[i][p_isd->service_result] += 1;
    }
    else
    {
        cnt_result[i][76] += 1;
    }

    /* Result Check */
    ResultCheck( p_isd );
}

/**********************************************************************************
* Call Back Func SERVICE_02_RAND_EXTEND_SEED
***********************************************************************************/
static void isd_call_back_rand_extend_seed(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_RAND_EXTEND_SEED;

    cnt_recieve_service_response++;

    if (SERV_NVM_REJECTED != p_isd->service_result)
    {
        cnt_result[i][p_isd->service_result] += 1;
    }
    else
    {
        cnt_result[i][76] += 1;
    }

    /* Result Check */
    ResultCheck( p_isd );
}

/**********************************************************************************
* Call Back Func SERVICE_02_RAND_RND
***********************************************************************************/
static void isd_call_back_rand_rnd(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_RAND_RND;

    cnt_recieve_service_response++;

    if (SERV_NVM_REJECTED != p_isd->service_result)
    {
        cnt_result[i][p_isd->service_result] += 1;
    }
    else
    {
        cnt_result[i][76] += 1;
    }

    /* Result Check */
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_03_PK_IMPORT
// ********************************************************************************
static void isd_call_back_pk_import(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_PK_IMPORT;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_03_ECC_KEY_GENERATE
// ********************************************************************************
static void isd_call_back_ecc_key_generate(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_ECC_KEY_GENERATE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_01_ECDSA_SIGN (ECC_RAM_0)
// ********************************************************************************
static void isd_call_back_ecdsa_sign(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_ECDSA_SIGN;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_01_ECDSA_VERIFY (ECC_RAM_1)
// ********************************************************************************
static void isd_call_back_ecdsa_verify(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_ECDSA_VERIFY;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (p_isd->prm.ECDSA_VERIFY.verification_result == VERIFICATION_PASS)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_RSA_DECRYPT (RSA_7)
// ********************************************************************************
static void isd_call_back_rsa_decrypt(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_RSA_DECRYPT;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.RSA_DECRYPT.p_message, test_msg_rsa_decrypt, sizeof(test_msg_rsa_decrypt)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_03_GENERATE_M1_3 (AES_8)
// ********************************************************************************
#if (STEP2_UPDATE_SHE > 0)
static void isd_call_back_generate_m1_3(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_GENERATE_M1_3;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

static void isd_call_back_step2_generate_m1_3(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_GENERATE_M1_3;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if( p_isd->service_result == SERV_OK )sw_update_she = 2;
    else                                  sw_update_she = 0;
}
#endif /* (STEP2_UPDATE_SHE > 0) */

// ********************************************************************************
// Call Back Func SERVICE_01_RSA_ENCRYPT (RSA_8)
// ********************************************************************************
static void isd_call_back_rsa_encrypt(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_RSA_ENCRYPT;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_03_HMAC_IMPORT (HMAC_0)
// ********************************************************************************
static void isd_call_back_hmac_import(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_HMAC_IMPORT;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_01_HMAC (HMAC_1, job_slice = 1)
// ********************************************************************************
static void isd_call_back_hmac_generate(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_HMAC_GENERATE;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;

    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.HMAC.p_hmac, result_hmac, sizeof(result_hmac)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}
static void isd_call_back_hmac_verify(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_HMAC_VERIFY;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;

    if (p_isd->service_result == SERV_OK)
    {
        if (p_isd->prm.HMAC.verification_result == VERIFICATION_PASS)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_ECDH (ECC_1, ECC_PUB_1, AES_RAM_1)
// ********************************************************************************
static void isd_call_back_ecdh(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_ECDH;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (AES_10)
// ********************************************************************************
static void isd_call_back_step2_cmac_job_slice_1(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_STEP2_CMAC_JOB_SLICE_1;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        //if (0 == memcmp(p_isd->prm.AES_CMAC.p_cmac, test_cmac_value_LOREM_IPSUM, sizeof(test_cmac_value_LOREM_IPSUM)))
        if (0 == memcmp(p_isd->prm.AES_CMAC.p_cmac, test_cmac_value_128bits, sizeof(test_cmac_value_128bits)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
    sw_key_mng = 0;
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (STEP2 AES_12, priority Highest)
// ********************************************************************************
static void isd_call_back_step2_cmac_fast_A(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_STEP2_CMAC_FAST_A;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CMAC.p_cmac, test_vrf_cmac_fast_128bits, sizeof(test_vrf_cmac_fast_128bits)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
    SYNC_CMAC_FAST = CMAC_NOT_EXIST;
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (STEP2 AES_13, priority Highest)
// ********************************************************************************
static void isd_call_back_step2_cmac_fast_B(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_STEP2_CMAC_FAST_B;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CMAC.p_cmac, test_vrf_cmac_fast_128bits, sizeof(test_vrf_cmac_fast_128bits)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
    SYNC_CMAC_FAST = CMAC_NOT_EXIST;
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (veri)
// ********************************************************************************
static void isd_call_back_cmac_veri(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CMAC_VERI;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (VERIFICATION_PASS == p_isd->prm.AES_CMAC.verification_result)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(AES_7, CTR, ENC)
// ********************************************************************************
static void isd_call_back_aes_cipher_ctr_enc(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CTR_ENC;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_ctr, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(AES_7, CTR, DEC)
// ********************************************************************************
static void isd_call_back_aes_cipher_ctr_dec(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CTR_DEC;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_blick_in, p_isd->prm.AES_CIPHER.p_block_out, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(AES_7, CBC, ENC)
// ********************************************************************************
static void isd_call_back_aes_cipher_cbc_enc(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CBC_ENC;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_cbc, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(AES_7, CBC, DEC)
// ********************************************************************************
static void isd_call_back_aes_cipher_cbc_dec(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CBC_DEC;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_blick_in, p_isd->prm.AES_CIPHER.p_block_out, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_04_ICUMDF_GC (GC all/GC 1block)
// ********************************************************************************
static void isd_call_back_start_gc(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_SDF_GC_REQ;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    if (-1 == ResultCheck(p_isd)) return;

    if (SERV_OK == p_isd->service_result)
    {
        sw_gc_warning = 0;
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(AES_18, CBC, 192bit)
// ********************************************************************************
static void isd_call_back_aes_cipher_192(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_192;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_192, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(AES_19, CBC, 256bit)
// ********************************************************************************
static void isd_call_back_aes_cipher_256(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_256;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_256, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (192bit)
// ********************************************************************************
static void isd_call_back_cmac_192(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CMAC_192;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CMAC.p_cmac, test_cmac_value_192bits, sizeof(test_cmac_value_128bits)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (256bit)
// ********************************************************************************
static void isd_call_back_cmac_256(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CMAC_256;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CMAC.p_cmac, test_cmac_value_256bits, sizeof(test_cmac_value_256bits)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (GCM,192bit))
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_192(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_192;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_auth_tag_192 , p_isd->prm.AES_AUTH_CIPHER.p_auth_tag, (p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (GCM,256bit)
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_256(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_256;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_auth_tag_256 , p_isd->prm.AES_AUTH_CIPHER.p_auth_tag, (p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_03_AES_KEY_UPDATE_PLAIN (AES_24,192bit)
// ********************************************************************************
static void isd_call_back_aes_update_plain_192(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_KEY_UPDATE_PLAIN_192;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_03_AES_KEY_UPDATE_PLAIN (AES_25,256bit)
// ********************************************************************************
static void isd_call_back_aes_update_plain_256(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_KEY_UPDATE_PLAIN_256;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(AES_18, CBC, 192bit)
// ********************************************************************************
static void isd_call_back_aes_cipher_192_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_192_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_192, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(AES_19, CBC, 256bit)
// ********************************************************************************
static void isd_call_back_aes_cipher_256_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_256_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_256, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (192bit)
// ********************************************************************************
static void isd_call_back_cmac_192_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CMAC_192_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CMAC.p_cmac, test_cmac_value_192bits, sizeof(test_cmac_value_192bits)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (256bit)
// ********************************************************************************
static void isd_call_back_cmac_256_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CMAC_256_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CMAC.p_cmac, test_cmac_value_256bits, sizeof(test_cmac_value_256bits)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (GCM,192bit))
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_192_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_192_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_auth_tag_192 , p_isd->prm.AES_AUTH_CIPHER.p_auth_tag, (p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (GCM,256bit)
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_256_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_256_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_auth_tag_256 , p_isd->prm.AES_AUTH_CIPHER.p_auth_tag, (p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_ECDH (192bit,AES_RAM_2)
// ********************************************************************************
static void isd_call_back_ecdh_192(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_ECDH_192;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_01_ECDH (256bit,AES_RAM_3)
// ********************************************************************************
static void isd_call_back_ecdh_256(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_ECDH_256;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}


// ********************************************************************************
// Call Back Func SERVICE_01_EDDSA_SIGN
// ********************************************************************************
static void isd_call_back_eddsa_sign(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_EDDSA_SIGN;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    ResultCheck( p_isd );
}

// ********************************************************************************
// Call Back Func SERVICE_01_EDDSA_VERIFY
// ********************************************************************************
static void isd_call_back_eddsa_verify(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_EDDSA_VERIFY;

    cnt_recieve_service_response++;
    

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (p_isd->prm.EDDSA_VERIFY.verification_result == VERIFICATION_PASS)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (veri)
// ********************************************************************************
static void isd_call_back_cmac_veri_192(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CMAC_VERI_192;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (VERIFICATION_PASS == p_isd->prm.AES_CMAC.verification_result)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CTR, ENC)
// ********************************************************************************
static void isd_call_back_aes_cipher_ctr_enc_192(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CTR_ENC_192;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_ctr_192, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CTR, DEC)
// ********************************************************************************
static void isd_call_back_aes_cipher_ctr_dec_192(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CTR_DEC_192;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_blick_in_192_ctr, p_isd->prm.AES_CIPHER.p_block_out, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CBC, ENC)
// ********************************************************************************
static void isd_call_back_aes_cipher_cbc_enc_192(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CBC_ENC_192;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_cbc_192, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CBC, DEC)
// ********************************************************************************
static void isd_call_back_aes_cipher_cbc_dec_192(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CBC_DEC_192;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_blick_in_192_cbc, p_isd->prm.AES_CIPHER.p_block_out, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (CCM,ENC)
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_ccm_enc_192(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_CCM_ENC_192;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_auth_tag_ccm_192 , p_isd->prm.AES_AUTH_CIPHER.p_auth_tag, (p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (CCM,DEC)
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_ccm_dec_192(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_CCM_DEC_192;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (VERIFICATION_PASS == p_isd->prm.AES_AUTH_CIPHER.verification_result)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (veri)
// ********************************************************************************
static void isd_call_back_cmac_veri_256(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CMAC_VERI_256;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (VERIFICATION_PASS == p_isd->prm.AES_CMAC.verification_result)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CTR, ENC)
// ********************************************************************************
static void isd_call_back_aes_cipher_ctr_enc_256(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CTR_ENC_256;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_ctr_256, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CTR, DEC)
// ********************************************************************************
static void isd_call_back_aes_cipher_ctr_dec_256(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CTR_DEC_256;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_blick_in_256_ctr, p_isd->prm.AES_CIPHER.p_block_out, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CBC, ENC)
// ********************************************************************************
static void isd_call_back_aes_cipher_cbc_enc_256(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CBC_ENC_256;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_cbc_256, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CBC, DEC)
// ********************************************************************************
static void isd_call_back_aes_cipher_cbc_dec_256(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CBC_DEC_256;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_blick_in_256_cbc, p_isd->prm.AES_CIPHER.p_block_out, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_CIPHER (XTS)
// ********************************************************************************
static void isd_call_back_aes_cipher_xts_256(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_XTS_256;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_xts_256, (p_isd->prm.AES_CIPHER.size.data_size_in_bits / 8)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (CCM,ENC)
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_ccm_enc_256(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_CCM_ENC_256;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_auth_tag_ccm_256 , p_isd->prm.AES_AUTH_CIPHER.p_auth_tag, (p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (CCM,DEC)
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_ccm_dec_256(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_CCM_DEC_256;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (VERIFICATION_PASS == p_isd->prm.AES_AUTH_CIPHER.verification_result)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (veri)
// ********************************************************************************
static void isd_call_back_cmac_veri_192_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CMAC_VERI_192_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (VERIFICATION_PASS == p_isd->prm.AES_CMAC.verification_result)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CTR, ENC)
// ********************************************************************************
static void isd_call_back_aes_cipher_ctr_enc_192_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CTR_ENC_192_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_ctr_192, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CTR, DEC)
// ********************************************************************************
static void isd_call_back_aes_cipher_ctr_dec_192_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CTR_DEC_192_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_blick_in_192_ctr, p_isd->prm.AES_CIPHER.p_block_out, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CBC, ENC)
// ********************************************************************************
static void isd_call_back_aes_cipher_cbc_enc_192_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CBC_ENC_192_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_cbc_192, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CBC, DEC)
// ********************************************************************************
static void isd_call_back_aes_cipher_cbc_dec_192_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CBC_DEC_192_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_blick_in_192_cbc, p_isd->prm.AES_CIPHER.p_block_out, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}


// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (CCM,ENC)
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_ccm_enc_192_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_CCM_ENC_192_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_auth_tag_ccm_192 , p_isd->prm.AES_AUTH_CIPHER.p_auth_tag, (p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (CCM,DEC)
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_ccm_dec_192_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_CCM_DEC_192_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_auth_tag_ccm_192 , p_isd->prm.AES_AUTH_CIPHER.p_auth_tag, (p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CMAC (veri)
// ********************************************************************************
static void isd_call_back_cmac_veri_256_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CMAC_VERI_256_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (VERIFICATION_PASS == p_isd->prm.AES_CMAC.verification_result)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CTR, ENC)
// ********************************************************************************
static void isd_call_back_aes_cipher_ctr_enc_256_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CTR_ENC_256_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_ctr_256, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CTR, DEC)
// ********************************************************************************
static void isd_call_back_aes_cipher_ctr_dec_256_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CTR_DEC_256_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_blick_in_256_ctr, p_isd->prm.AES_CIPHER.p_block_out, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CBC, ENC)
// ********************************************************************************
static void isd_call_back_aes_cipher_cbc_enc_256_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CBC_ENC_256_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_cbc_256, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// Call Back Func SERVICE_01_AES_CIPHER(CBC, DEC)
// ********************************************************************************
static void isd_call_back_aes_cipher_cbc_dec_256_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_CBC_DEC_256_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_blick_in_256_cbc, p_isd->prm.AES_CIPHER.p_block_out, (p_isd->prm.AES_CIPHER.size.nb_blocks * 16)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}


// ********************************************************************************
// SERVICE_01_AES_CIPHER (XTS)
// ********************************************************************************
static void isd_call_back_aes_cipher_xts_256_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_CIPHER_XTS_256_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if( -1 == ResultCheck( p_isd ))return;
    // Result Check
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(p_isd->prm.AES_CIPHER.p_block_out, test_cipher_data_xts_256, (p_isd->prm.AES_CIPHER.size.data_size_in_bits / 8)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}


// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (CCM,ENC)
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_ccm_enc_256_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_CCM_ENC_256_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (0 == memcmp(test_p_auth_tag_ccm_256 , p_isd->prm.AES_AUTH_CIPHER.p_auth_tag, (p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits / 8)))
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}

// ********************************************************************************
// SERVICE_01_AES_AUTH_CIPHER (CCM,DEC)
// ********************************************************************************
static void isd_call_back_aes_auth_cipher_ccm_dec_256_slice(r_icumif_isd_t  *p_isd)
{
    int i = TEST_NO_AES_AUTH_CIPHER_CCM_DEC_256_SLICE;

    cnt_recieve_service_response++;

    cnt_result[i][p_isd->service_result] += 1;

    // Result Check
    if (-1 == ResultCheck(p_isd)) return;
    if (p_isd->service_result == SERV_OK)
    {
        if (VERIFICATION_PASS == p_isd->prm.AES_AUTH_CIPHER.verification_result)
        {
            // OK
            cnt_result[i][77] += 1;
        }
        else
        {
            // NG
            cnt_result[i][78] += 1;
            sw_err_detect = 1;
        }
    }
}


// ********************************************************************************
// Select service to request
// ********************************************************************************
int select_service(test_type_t test_type, r_icumif_isd_t *p_ISD[], int isd_index, int select_index)
{
    int ret = 0;
#if (STEP2_UPDATE_SHE > 0)
    static unsigned long cid_update_she = 1;
#endif

    if( test_service[select_index].enable == 0 ){
        return(-1);
    }

    /* random test no test */
    if ((RANDOM_TEST == test_type) &&
        (SWEEP_ONLY_ENABLE == test_service[select_index].enable))
    {
        return(-1);
    }
    

#ifdef FSW_FASTCMAC_FIXEDPE
    if ((TEST_NO_STEP2_CMAC_FAST_A == select_index) || (TEST_NO_STEP2_CMAC_FAST_B == select_index) || (TEST_NO_STEP3_CMAC_FAST_A_192 == select_index)
    || (TEST_NO_STEP3_CMAC_FAST_B_192 == select_index) || (TEST_NO_STEP3_CMAC_FAST_A_256 == select_index) || (TEST_NO_STEP3_CMAC_FAST_B_256 == select_index))
    {
        return(-1);
    }
#else
    if (((TEST_NO_STEP2_CMAC_FAST_A == select_index) || (TEST_NO_STEP2_CMAC_FAST_B == select_index) || (TEST_NO_STEP3_CMAC_FAST_A_192 == select_index)
    || (TEST_NO_STEP3_CMAC_FAST_B_192 == select_index) || (TEST_NO_STEP3_CMAC_FAST_A_256 == select_index) || (TEST_NO_STEP3_CMAC_FAST_B_256 == select_index))
        && (CMAC_EXIST == SYNC_CMAC_FAST))
    {
        return(-1);
    }
#endif /* FSW_FASTCMAC_FIXEDPE */

    // initialize p_ISD
    memset(p_ISD[isd_index], 0x00, sizeof(r_icumif_isd_t));

    // service priority at random
    select_service_pri(test_type, p_ISD[isd_index]);

    switch (select_index)
    {
        case TEST_NO_AES_CMAC_JOB_SLICE_3:
            // SERVICE_01_AES_CMAC (AES_11, job_slice = 3, msg_size_in_bits = 1279, priority normal)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_NORMAL;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_cmac_job_slice_3;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 11;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&LOREM_IPSUM;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = 1279;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&BIG_BUFFER[2][1024];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 3;
            break;

        case TEST_NO_AES_CMAC_JOB_SLICE_0:
            // SERVICE_01_AES_CMAC Generate (no job_slice, priority normal)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_NORMAL;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_cmac_job_slice_0;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 3;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_cmac_msg_512bits;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = sizeof(test_cmac_msg_512bits) * 8;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&BIG_BUFFER[0][0];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 0;
            break;

        case TEST_NO_AES_CMAC_JOB_SLICE_1:
            // SERVICE_01_AES_CMAC Generate (job_slice = 1, priority normal)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_NORMAL;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_cmac_job_slice_1;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 5;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_cmac_msg_512bits;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = sizeof(test_cmac_msg_512bits) * 8;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&BIG_BUFFER[0][0];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 2;
            break;

        case TEST_NO_HASH:
            // SERVICE_01_HASH
            p_ISD[isd_index]->service_id                               = SERVICE_01_HASH;
            p_ISD[isd_index]->job_id                                   = 0;
            p_ISD[isd_index]->p_callbackfunc                           = (p_func_t)&isd_call_back_hash;
            p_ISD[isd_index]->prm.HASH.p_message                       = (const uint8_t *)&test_hash_message;
            p_ISD[isd_index]->prm.HASH.p_hash                          = &BIG_BUFFER[0][40];
            p_ISD[isd_index]->prm.HASH.message_size_in_bytes           = 16;
            p_ISD[isd_index]->prm.HASH.hash_size_in_bytes              = 0;
            p_ISD[isd_index]->prm.HASH.hash_primitive                  = HASH_PRIMITIVE_SHA2_512;
            p_ISD[isd_index]->prm.HASH.job_slice                       = 1;

            break;

        case TEST_NO_AES_KEY_UPDATE_PLAIN:
            // SERVICE_03_AES_KEY_UPDATE_PLAIN (AES_9)
            p_ISD[isd_index]->service_id                                   = SERVICE_03_AES_KEY_UPDATE_PLAIN;
            p_ISD[isd_index]->job_id                                       = 0;
            p_ISD[isd_index]->p_callbackfunc                               = (p_func_t)&isd_call_back_aes_update_plain;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.p_key_value             = (const uint8_t *)&KEY;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.key_id                  = 9;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes       = 16;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.encryption        = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.decryption        = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation   = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug  = 0;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.write_protected   = 0;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.no_wild_card      = 0;
            break;

        case TEST_NO_AES_KEY_UPDATE_PLAIN_ANOTHER_KEY:
            // SERVICE_03_AES_KEY_UPDATE_PLAIN (AES_10)
            p_ISD[isd_index]->service_id                                   = SERVICE_03_AES_KEY_UPDATE_PLAIN;
            p_ISD[isd_index]->job_id                                       = 0;
            p_ISD[isd_index]->p_callbackfunc                               = (p_func_t)&isd_call_back_aes_update_plain_another_key;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.p_key_value             = (const uint8_t *)&KEY;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.key_id                  = 10;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes       = 16;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.encryption        = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.decryption        = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation   = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug  = 0;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.write_protected   = 0;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.no_wild_card      = 0;
            break;

        case TEST_NO_RSA_VERIFY_JOB_SLICE_0:
            // SERVICE_01_RSA_VERIFY (RSA_0, job_slice_hash = 0)
            p_ISD[isd_index]->service_id                               = SERVICE_01_RSA_VERIFY;
            p_ISD[isd_index]->job_id                                   = 0;
            p_ISD[isd_index]->p_callbackfunc                           = (p_func_t)&isd_call_back_rsa_verify_job_slice_hash_0;
            p_ISD[isd_index]->prm.RSA_VERIFY.p_source                  = (const uint8_t *)&IT_INPUT_DATA;
            p_ISD[isd_index]->prm.RSA_VERIFY.p_signature               = &BIG_BUFFER[1][1360];//&test_rsa_signature_128byte;
            p_ISD[isd_index]->prm.RSA_VERIFY.source_size_in_bytes      = 16;
            p_ISD[isd_index]->prm.RSA_VERIFY.signature_size_in_bits    = 1024;
            p_ISD[isd_index]->prm.RSA_VERIFY.salt_size_in_bytes        = 20;
            p_ISD[isd_index]->prm.RSA_VERIFY.hash_primitive            = HASH_PRIMITIVE_SHA2_256;
            p_ISD[isd_index]->prm.RSA_VERIFY.hash_encoding_format      = ENCODING_FORMAT_EMSA_PSS;
            p_ISD[isd_index]->prm.RSA_VERIFY.signature_format          = FORMAT_BINARY_PLAIN;
            p_ISD[isd_index]->prm.RSA_VERIFY.key_group                 = KEY_GRP_RSA;
            p_ISD[isd_index]->prm.RSA_VERIFY.key_id                    = 0;
            p_ISD[isd_index]->prm.RSA_VERIFY.job_slice_hash            = 0;
            p_ISD[isd_index]->prm.RSA_VERIFY.job_slice_mod_exp         = 0;

            break;

        case TEST_NO_RSA_VERIFY_JOB_SLICE_1:
            // SERVICE_01_RSA_VERIFY (RSA_1, job_slice_hash = 1)
            p_ISD[isd_index]->service_id                               = SERVICE_01_RSA_VERIFY;
            p_ISD[isd_index]->job_id                                   = 0;
            p_ISD[isd_index]->p_callbackfunc                           = (p_func_t)&isd_call_back_rsa_verify_job_slice_hash_1;
            p_ISD[isd_index]->prm.RSA_VERIFY.p_source                  = (const uint8_t *)&IT_INPUT_DATA;
            p_ISD[isd_index]->prm.RSA_VERIFY.p_signature               = (const uint8_t *)&test_rsa_signature_128byte;
            p_ISD[isd_index]->prm.RSA_VERIFY.source_size_in_bytes      = 16;
            p_ISD[isd_index]->prm.RSA_VERIFY.signature_size_in_bits    = 1024;
            p_ISD[isd_index]->prm.RSA_VERIFY.salt_size_in_bytes        = 20;
            p_ISD[isd_index]->prm.RSA_VERIFY.hash_primitive            = HASH_PRIMITIVE_SHA2_256;
            p_ISD[isd_index]->prm.RSA_VERIFY.hash_encoding_format      = ENCODING_FORMAT_EMSA_PSS;
            p_ISD[isd_index]->prm.RSA_VERIFY.signature_format          = FORMAT_BINARY_PLAIN;
            p_ISD[isd_index]->prm.RSA_VERIFY.key_group                 = KEY_GRP_RSA;
            p_ISD[isd_index]->prm.RSA_VERIFY.key_id                    = 1;
            p_ISD[isd_index]->prm.RSA_VERIFY.job_slice_hash            = 1;
            p_ISD[isd_index]->prm.RSA_VERIFY.job_slice_mod_exp         = 0;

            break;

        case TEST_NO_AES_CIPHER:
            // SERVICE_01_AES_CIPHER (AES_7)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[0][56];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 4;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 7;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_ECB;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = (r_job_cycle_t)0;
            break;

        case TEST_NO_AES_CIPHER_XTS:
            // SERVICE_01_AES_CIPHER (XTS, AES_14, AES_15)
            p_ISD[isd_index]->service_id                            = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                                = 0;
            p_ISD[isd_index]->p_callbackfunc                        = (p_func_t)&isd_call_back_aes_cipher_xts;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv                   = (const uint32_t *)&test_p_iv_xts;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in             = (const uint32_t *)&test_p_blick_in_xts;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out            = (uint32_t *)&BIG_BUFFER[1][2000];
            p_ISD[isd_index]->prm.AES_CIPHER.size.data_size_in_bits = 256;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group              = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id                 = 14;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2            = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2               = 15;
            p_ISD[isd_index]->prm.AES_CIPHER.direction              = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode            = CIPHER_MOD_XTS;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice              = 2;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle              = (r_job_cycle_t)0;
            break;

        case TEST_NO_AES_AUTH_CIPHER_GCM:
            // SERVICE_01_AES_AUTH_CIPHER (GCM, DEC, AES_16)
            p_ISD[isd_index]->service_id                                    = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                        = 0;
            p_ISD[isd_index]->p_callbackfunc                                = (p_func_t)&isd_call_back_aes_auth_cipher;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                      = (const uint32_t *)&test_p_iv_auth_gcm;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data               = (const uint32_t *)&test_p_auth_data_gcm;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in                 = (const uint32_t *)&test_p_data_in_gcm;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out                = (uint32_t *)&BIG_BUFFER[1][2032];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag                = (uint32_t *)&test_p_auth_tag_gcm;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits         = 0;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits    = 720;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits           = 96;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits     = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group                 = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                    = 16;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction                 = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode               = AUTH_CIPHER_MOD_GCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice                 = 3;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle                 = (r_job_cycle_t)0;
            break;

        case TEST_NO_AES_AUTH_CIPHER_CCM_ENC:
            // SERVICE_01_AES_AUTH_CIPHER (ENC CCM, AES_17)
            /* NIST DVPT128.rsp [Alen = 32, Plen = 24, Nlen = 13, Tlen = 16] Count = 225 */
            p_ISD[isd_index]->service_id                                  = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                      = 0;
            p_ISD[isd_index]->p_callbackfunc                              = (p_func_t)&isd_call_back_aes_auth_cipher_ccm_enc;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                  = 17;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                    = (const uint32_t *)test_p_iv_auth_ccm;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits         = 104;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in               = (const uint32_t *)test_p_plain_text_ccm;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits       = 192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data             = (const uint32_t *)test_p_aad_ccm;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)&BIG_BUFFER[2][3088];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)&BIG_BUFFER[2][3072];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice               = 0;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_AUTH_CIPHER_CCM_DEC:
            // SERVICE_01_AES_AUTH_CIPHER (DEC CCM, AES_17)
            /* NIST DVPT128.rsp [Alen = 32, Plen = 24, Nlen = 13, Tlen = 16] Count = 225 */
            p_ISD[isd_index]->service_id                                  = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                      = 0;
            p_ISD[isd_index]->p_callbackfunc                              = (p_func_t)&isd_call_back_aes_auth_cipher_ccm_dec;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                  = 17;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                    = (const uint32_t *)test_p_iv_auth_ccm;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits         = 104;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in               = (const uint32_t *)test_p_cipher_text_ccm;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits       = 192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data             = (const uint32_t *)test_p_aad_ccm;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)&BIG_BUFFER[2][3088];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)&test_p_auth_tag_ccm;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice               = 0;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_RSA_SIGN:
            // SERVICE_01_RSA_SIGN (RSA_2)
            p_ISD[isd_index]->service_id                             = SERVICE_01_RSA_SIGN;
            p_ISD[isd_index]->job_id                                 = 0;
            p_ISD[isd_index]->p_callbackfunc                         = (p_func_t)&isd_call_back_rsa_sign;
            p_ISD[isd_index]->prm.RSA_SIGN.p_source                  = (const uint8_t *)&IT_INPUT_DATA;
            p_ISD[isd_index]->prm.RSA_SIGN.p_signature               = (uint8_t *)&BIG_BUFFER[1][1360];//&test_rsa_signature_128byte;
            p_ISD[isd_index]->prm.RSA_SIGN.source_size_in_bytes      = 16;
            p_ISD[isd_index]->prm.RSA_SIGN.signature_size_in_bits    = 1024;
            p_ISD[isd_index]->prm.RSA_SIGN.salt_size_in_bytes        = 20;
            p_ISD[isd_index]->prm.RSA_SIGN.hash_primitive            = HASH_PRIMITIVE_SHA2_256;
            p_ISD[isd_index]->prm.RSA_SIGN.hash_encoding_format      = ENCODING_FORMAT_EMSA_PSS;
            p_ISD[isd_index]->prm.RSA_SIGN.signature_format          = FORMAT_BINARY_PLAIN;
            p_ISD[isd_index]->prm.RSA_SIGN.key_id                    = 2;
            p_ISD[isd_index]->prm.RSA_SIGN.job_slice_hash            = 1;
            p_ISD[isd_index]->prm.RSA_SIGN.job_slice_mod_exp         = 0;
            p_ISD[isd_index]->prm.RSA_SIGN.is_precalculated_hash     = 0;
            break;

        case TEST_NO_RSA_SIGN_2:
            // SERVICE_01_RSA_SIGN (RSA_6)
            memset(p_ISD[isd_index], 0x00, sizeof(r_icumif_isd_t));
            p_ISD[isd_index]->service_id                             = SERVICE_01_RSA_SIGN;
            p_ISD[isd_index]->job_id                                 = 0;
            p_ISD[isd_index]->p_callbackfunc                         = (p_func_t)&isd_call_back_rsa_sign_2;
            p_ISD[isd_index]->prm.RSA_SIGN.p_source                  = (void *)test_msg_PSS_11;
            p_ISD[isd_index]->prm.RSA_SIGN.p_signature               = &BIG_BUFFER[1][1536];
            p_ISD[isd_index]->prm.RSA_SIGN.source_size_in_bytes      = sizeof(test_msg_PSS_11);
            p_ISD[isd_index]->prm.RSA_SIGN.signature_size_in_bits    = 1024;
            p_ISD[isd_index]->prm.RSA_SIGN.salt_size_in_bytes        = 20;
            p_ISD[isd_index]->prm.RSA_SIGN.hash_primitive            = HASH_PRIMITIVE_SHA2_512;
            p_ISD[isd_index]->prm.RSA_SIGN.hash_encoding_format      = ENCODING_FORMAT_EMSA_PSS;
            p_ISD[isd_index]->prm.RSA_SIGN.signature_format          = FORMAT_BINARY_PLAIN;
            p_ISD[isd_index]->prm.RSA_SIGN.key_id                    = 6;
            p_ISD[isd_index]->prm.RSA_SIGN.job_slice_hash            = 32;
            p_ISD[isd_index]->prm.RSA_SIGN.job_slice_mod_exp         = 1;
            p_ISD[isd_index]->prm.RSA_SIGN.is_precalculated_hash     = 0;
            break;

        case TEST_NO_RAND_INIT_RNG:
            // SERVICE_02_RAND_INIT_RNG (on_line_test = 1)
            p_ISD[isd_index]->service_id                             = SERVICE_02_RAND_INIT_RNG;
            p_ISD[isd_index]->job_id                                 = 0;
            p_ISD[isd_index]->p_callbackfunc                         = (p_func_t)&isd_call_back_rand_init_rng;
            p_ISD[isd_index]->prm.RAND_INIT.online_test              = RAND_DIAG_P1_TEST;
            break;

        case TEST_NO_RAND_EXTEND_SEED:
            // SERVICE_02_RAND_EXTEND_SEED
            p_ISD[isd_index]->service_id                             = SERVICE_02_RAND_EXTEND_SEED;
            p_ISD[isd_index]->job_id                                 = 0;
            p_ISD[isd_index]->p_callbackfunc                         = (p_func_t)&isd_call_back_rand_extend_seed;
            p_ISD[isd_index]->prm.RAND_EXTEND_SEED.p_entropy         = (const uint32_t *)&BIG_BUFFER[0][136];
            break;

        case TEST_NO_RAND_RND:
            // SERVICE_02_RAND_RND
            p_ISD[isd_index]->service_id                             = SERVICE_02_RAND_RND;
            p_ISD[isd_index]->job_id                                 = 0;
            p_ISD[isd_index]->p_callbackfunc                         = (p_func_t)&isd_call_back_rand_rnd;
            p_ISD[isd_index]->prm.RAND_GENERATE.p_block_out          = (uint32_t *)&BIG_BUFFER[0][152];
            p_ISD[isd_index]->prm.RAND_GENERATE.nb_blocks            = 1;
            p_ISD[isd_index]->prm.RAND_GENERATE.job_slice            = 1;
            break;

        case TEST_NO_PK_IMPORT:
            // SERVICE_03_PK_IMPORT (RSA_3)
            p_ISD[isd_index]->service_id                                   = SERVICE_03_PK_IMPORT;
            p_ISD[isd_index]->job_id                                       = 0;
            p_ISD[isd_index]->p_callbackfunc                               = (p_func_t)&isd_call_back_pk_import;
            p_ISD[isd_index]->prm.PK_IMPORT.p_modulus                      = (const uint8_t *)&test_pk_modulus_sig_1024;
            p_ISD[isd_index]->prm.PK_IMPORT.p_pub_exp                      = (const uint8_t *)&test_pk_e10001;
            p_ISD[isd_index]->prm.PK_IMPORT.p_priv_exp                     = (const uint8_t *)&test_pk_priv_exp_sig_1024;
            p_ISD[isd_index]->prm.PK_IMPORT.p_signature                    = 0;
            p_ISD[isd_index]->prm.PK_IMPORT.p_info                         = 0;
            p_ISD[isd_index]->prm.PK_IMPORT.key_format                     = FORMAT_STRING_HEXA;
            p_ISD[isd_index]->prm.PK_IMPORT.sig_format                     = (r_number_format_t)0;
            p_ISD[isd_index]->prm.PK_IMPORT.key_group                      = KEY_GRP_RSA;
            p_ISD[isd_index]->prm.PK_IMPORT.key_id                         = 3;
            p_ISD[isd_index]->prm.PK_IMPORT.sig_key_group                  = (r_key_group_t)0;
            p_ISD[isd_index]->prm.PK_IMPORT.sig_key_id                     = 0;
            p_ISD[isd_index]->prm.PK_IMPORT.sig_algo                       = SIGALGO_NONE;
            p_ISD[isd_index]->prm.PK_IMPORT.hash_primitive                 = (r_hash_primitive_t)0;
            p_ISD[isd_index]->prm.PK_IMPORT.curve_id                       = (r_ecc_curve_t)0;
            p_ISD[isd_index]->prm.PK_IMPORT.point_x_size_in_bits           = 1024;
            p_ISD[isd_index]->prm.PK_IMPORT.point_y_size_in_bits           = 17;
            p_ISD[isd_index]->prm.PK_IMPORT.priv_scalar_size_in_bits       = 1024;
            p_ISD[isd_index]->prm.PK_IMPORT.sig_size_in_bits               = 0;
            p_ISD[isd_index]->prm.PK_IMPORT.info_size_in_bytes             = 0;
            p_ISD[isd_index]->prm.PK_IMPORT.flags.disable_on_debug         = 0;
            p_ISD[isd_index]->prm.PK_IMPORT.flags.write_protected          = 0;
            p_ISD[isd_index]->prm.PK_IMPORT.skip_verify                    = 1;

            break;

        case TEST_NO_RSA_KEY_GENERATE:
            /* Service Request delete from PE2 because simultaneous request for RSA key generate. */
            ret = -1;
            break;

        case TEST_NO_ECC_KEY_GENERATE:
            // SERVICE_03_ECC_KEY_GENERATE (ECC_1)
            p_ISD[isd_index]->service_id                                   = SERVICE_03_ECC_KEY_GENERATE;
            p_ISD[isd_index]->job_id                                       = 0;
            p_ISD[isd_index]->p_callbackfunc                               = (p_func_t)&isd_call_back_ecc_key_generate;
            p_ISD[isd_index]->prm.ECC_KEY_GENERATE.key_group               = KEY_GRP_ECC_RAM;
            p_ISD[isd_index]->prm.ECC_KEY_GENERATE.key_id                  = 0;
            p_ISD[isd_index]->prm.ECC_KEY_GENERATE.curve_id                = ECC_CURVE_NIST_P192;
            p_ISD[isd_index]->prm.ECC_KEY_GENERATE.flags.disable_on_debug  = 0;
            break;

        case TEST_NO_ECDSA_SIGN:
            // SERVICE_01_ECDSA_SIGN (ECC_RAM_0)
            p_ISD[isd_index]->service_id                                   = SERVICE_01_ECDSA_SIGN;
            p_ISD[isd_index]->job_id                                       = 0;
            p_ISD[isd_index]->p_callbackfunc                               = (p_func_t)&isd_call_back_ecdsa_sign;
            p_ISD[isd_index]->prm.ECDSA_SIGN.p_source                      = (const uint8_t *)&test_ecdsa_msg_16byte;
            p_ISD[isd_index]->prm.ECDSA_SIGN.signature[0].p_data           = &BIG_BUFFER[0][2048];
            p_ISD[isd_index]->prm.ECDSA_SIGN.signature[1].p_data           = &BIG_BUFFER[0][2108];
            p_ISD[isd_index]->prm.ECDSA_SIGN.source_size_in_bytes          = 16;
            p_ISD[isd_index]->prm.ECDSA_SIGN.hash_primitive                = HASH_PRIMITIVE_SHA2_224;
            p_ISD[isd_index]->prm.ECDSA_SIGN.signature_format              = FORMAT_STRING_DECIMAL;
            p_ISD[isd_index]->prm.ECDSA_SIGN.key_group                     = KEY_GRP_ECC_RAM;
            p_ISD[isd_index]->prm.ECDSA_SIGN.key_id                        = 1;
            p_ISD[isd_index]->prm.ECDSA_SIGN.job_slice_hash                = 1;
            p_ISD[isd_index]->prm.ECDSA_SIGN.job_slice_point_mul           = 1;
            p_ISD[isd_index]->prm.ECDSA_SIGN.signature_size_in_bits[0]     = 0;
            p_ISD[isd_index]->prm.ECDSA_SIGN.signature_size_in_bits[1]     = 0;
            p_ISD[isd_index]->prm.ECDSA_SIGN.is_precalculated_hash         = 0;

            break;

        case TEST_NO_ECDSA_VERIFY:
            // SERVICE_01_ECDSA_VERIFY (ECC_RAM_1)
            p_ISD[isd_index]->service_id                                   = SERVICE_01_ECDSA_VERIFY;
            p_ISD[isd_index]->job_id                                       = 0;
            p_ISD[isd_index]->p_callbackfunc                               = (p_func_t)&isd_call_back_ecdsa_verify;
            p_ISD[isd_index]->prm.ECDSA_VERIFY.p_source                    = (const uint8_t *)&test_ecdsa_msg_16byte;
            p_ISD[isd_index]->prm.ECDSA_VERIFY.signature[0].p_data         = (const uint8_t *)&test_ecdsa_signature_0;
            p_ISD[isd_index]->prm.ECDSA_VERIFY.signature[1].p_data         = (const uint8_t *)&test_ecdsa_signature_1;
            p_ISD[isd_index]->prm.ECDSA_VERIFY.source_size_in_bytes        = 16;
            p_ISD[isd_index]->prm.ECDSA_VERIFY.signature_size_in_bits[0]   = 192;
            p_ISD[isd_index]->prm.ECDSA_VERIFY.signature_size_in_bits[1]   = 192;
            p_ISD[isd_index]->prm.ECDSA_VERIFY.hash_primitive              = HASH_PRIMITIVE_SHA2_512;
            p_ISD[isd_index]->prm.ECDSA_VERIFY.signature_format            = FORMAT_STRING_DECIMAL;
            p_ISD[isd_index]->prm.ECDSA_VERIFY.key_group                   = KEY_GRP_ECC_RAM;
            p_ISD[isd_index]->prm.ECDSA_VERIFY.key_id                      = 1;
            p_ISD[isd_index]->prm.ECDSA_VERIFY.job_slice_hash              = 1;
            p_ISD[isd_index]->prm.ECDSA_VERIFY.job_slice_point_mul         = 1;
            p_ISD[isd_index]->prm.ECDSA_VERIFY.is_precalculated_hash       = 0;

            break;

        case TEST_NO_RSA_DECRYPT:
            // SERVICE_01_RSA_DECRYPT (RSA_7)
            p_ISD[isd_index]->service_id                                       = SERVICE_01_RSA_DECRYPT;
            p_ISD[isd_index]->job_id                                           = 0;
            p_ISD[isd_index]->p_callbackfunc                                   = (p_func_t)&isd_call_back_rsa_decrypt;
            p_ISD[isd_index]->prm.RSA_DECRYPT.p_encrypted_message              = (const uint8_t *)&test_encoded_msg_oaep_hexa;
            p_ISD[isd_index]->prm.RSA_DECRYPT.encrypted_message_size_in_bits   = 1024;
            p_ISD[isd_index]->prm.RSA_DECRYPT.p_message                        = &BIG_BUFFER[1][0];
            p_ISD[isd_index]->prm.RSA_DECRYPT.message_size_in_bytes            = 28;
            p_ISD[isd_index]->prm.RSA_DECRYPT.p_label                          = &BIG_BUFFER[1][32];
            p_ISD[isd_index]->prm.RSA_DECRYPT.label_size_in_bytes              = 0;
            p_ISD[isd_index]->prm.RSA_DECRYPT.hash_primitive                   = HASH_PRIMITIVE_SHA2_224;
            p_ISD[isd_index]->prm.RSA_DECRYPT.message_encoding_format          = ENCODING_FORMAT_EME_OAEP;
            p_ISD[isd_index]->prm.RSA_DECRYPT.key_id                           = 7;
            p_ISD[isd_index]->prm.RSA_DECRYPT.encrypted_message_format         = FORMAT_STRING_HEXA;
            p_ISD[isd_index]->prm.RSA_DECRYPT.job_slice_mod_exp                = 1;

            break;

        case TEST_NO_GENERATE_M1_3:
            /* do nothing */
            break;
        case TEST_NO_STEP2_GENERATE_M1_3:
        case TEST_NO_STEP2_UPDATE_SHE:
            /* do nothing */
            break;

        case TEST_NO_RSA_ENCRYPT:
            // SERVICE_01_RSA_ENCRYPT (RSA_8)
            p_ISD[isd_index]->service_id                                       = SERVICE_01_RSA_ENCRYPT;
            p_ISD[isd_index]->job_id                                           = 0;
            p_ISD[isd_index]->p_callbackfunc                                   = (p_func_t)&isd_call_back_rsa_encrypt;
            p_ISD[isd_index]->prm.RSA_ENCRYPT.p_message                        = (const uint8_t *)&test_msg_rsa_encrypt;
            p_ISD[isd_index]->prm.RSA_ENCRYPT.message_size_in_bytes            = 28;
            p_ISD[isd_index]->prm.RSA_ENCRYPT.p_encrypted_message              = &BIG_BUFFER[1][48];
            p_ISD[isd_index]->prm.RSA_ENCRYPT.encrypted_message_size_in_bits   = 1024;
            p_ISD[isd_index]->prm.RSA_ENCRYPT.p_label                          = &BIG_BUFFER[1][256];
            p_ISD[isd_index]->prm.RSA_ENCRYPT.label_size_in_bytes              = 20;
            p_ISD[isd_index]->prm.RSA_ENCRYPT.hash_primitive                   = HASH_PRIMITIVE_SHA2_384;
            p_ISD[isd_index]->prm.RSA_ENCRYPT.message_encoding_format          = ENCODING_FORMAT_EME_OAEP;
            p_ISD[isd_index]->prm.RSA_ENCRYPT.key_group                        = KEY_GRP_RSA;
            p_ISD[isd_index]->prm.RSA_ENCRYPT.key_id                           = 8;
            p_ISD[isd_index]->prm.RSA_ENCRYPT.encrypted_message_format         = FORMAT_STRING_HEXA;
            p_ISD[isd_index]->prm.RSA_ENCRYPT.job_slice_mod_exp                = 1;

            break;

        case TEST_NO_HMAC_IMPORT:
            // SERVICE_03_HMAC_IMPORT (HMAC_0)
            p_ISD[isd_index]->service_id                                       = SERVICE_03_HMAC_IMPORT;
            p_ISD[isd_index]->job_id                                           = 0;
            p_ISD[isd_index]->p_callbackfunc                                   = (p_func_t)&isd_call_back_hmac_import;
            p_ISD[isd_index]->prm.HMAC_IMPORT.key_id                           = 0;
            p_ISD[isd_index]->prm.HMAC_IMPORT.p_key                            = (const uint8_t *)&test_hmac_key;
            p_ISD[isd_index]->prm.HMAC_IMPORT.key_size_in_bytes                = 10;
            break;

        case TEST_NO_HMAC_GENERATE:
            // SERVICE_01_HMAC (HMAC_1, job_slice = 1)
            p_ISD[isd_index]->service_id                                       = SERVICE_01_HMAC;
            p_ISD[isd_index]->job_id                                           = 0;
            p_ISD[isd_index]->p_callbackfunc                                   = (p_func_t)&isd_call_back_hmac_generate;
            p_ISD[isd_index]->prm.HMAC.key_id                                  = 1;
            p_ISD[isd_index]->prm.HMAC.hmac_operation                          = HMAC_GENERATION;
            p_ISD[isd_index]->prm.HMAC.hash_primitive                          = HASH_PRIMITIVE_SHA2_256;
            p_ISD[isd_index]->prm.HMAC.p_msg                                   = (const uint8_t *)&test_msg_hmac;
            p_ISD[isd_index]->prm.HMAC.msg_size_in_bytes                       = 128;
            p_ISD[isd_index]->prm.HMAC.hmac_size_in_bytes                      = 32;
            p_ISD[isd_index]->prm.HMAC.job_slice                               = 1;
            p_ISD[isd_index]->prm.HMAC.p_hmac                                  = &BIG_BUFFER[1][384];

            break;

        case TEST_NO_HMAC_VERIFY:
            // SERVICE_01_HMAC (HMAC_1, job_slice = 1)
            p_ISD[isd_index]->service_id                                       = SERVICE_01_HMAC;
            p_ISD[isd_index]->job_id                                           = 0;
            p_ISD[isd_index]->p_callbackfunc                                   = (p_func_t)&isd_call_back_hmac_verify;
            p_ISD[isd_index]->prm.HMAC.key_id                                  = 1;
            p_ISD[isd_index]->prm.HMAC.hmac_operation                          = HMAC_VERIFICATION;
            p_ISD[isd_index]->prm.HMAC.hash_primitive                          = HASH_PRIMITIVE_SHA2_256;
            p_ISD[isd_index]->prm.HMAC.p_msg                                   = (const uint8_t *)&test_msg_hmac;
            p_ISD[isd_index]->prm.HMAC.msg_size_in_bytes                       = 128;
            p_ISD[isd_index]->prm.HMAC.hmac_size_in_bytes                      = 32;
            p_ISD[isd_index]->prm.HMAC.job_slice                               = 1;
            p_ISD[isd_index]->prm.HMAC.p_hmac                                  = (uint8_t *)&result_hmac;

            break;

        case TEST_NO_ECDH:
            // SERVICE_01_ECDH (ECC_1, ECC_PUB_1, AES_RAM_1)
            p_ISD[isd_index]->service_id                                       = SERVICE_01_ECDH;
            p_ISD[isd_index]->job_id                                           = 0;
            p_ISD[isd_index]->p_callbackfunc                                   = (p_func_t)&isd_call_back_ecdh;
            p_ISD[isd_index]->prm.ECDH.ecc_priv_key_group                      = KEY_GRP_ECC;
            p_ISD[isd_index]->prm.ECDH.ecc_priv_key_id                         = 1;
            p_ISD[isd_index]->prm.ECDH.ecc_pub_key_group                       = KEY_GRP_ECC_PUB;
            p_ISD[isd_index]->prm.ECDH.ecc_pub_key_id                          = 1;
            p_ISD[isd_index]->prm.ECDH.shared_key_group                        = KEY_GRP_AES_RAM;
            p_ISD[isd_index]->prm.ECDH.shared_key_id                           = 1;
            p_ISD[isd_index]->prm.ECDH.kdf_algo                                = KDF_ALGO_KDF2;
            p_ISD[isd_index]->prm.ECDH.hash_primitive                          = HASH_PRIMITIVE_SHA2_384;
            p_ISD[isd_index]->prm.ECDH.job_slice                               = 1;
            p_ISD[isd_index]->prm.ECDH.p_info                                  = 0;
            p_ISD[isd_index]->prm.ECDH.counter                                 = 4;
            p_ISD[isd_index]->prm.ECDH.info_size_in_bytes                      = 0;
            p_ISD[isd_index]->prm.ECDH.key_size_in_bytes                       = 16;
            break;

        case TEST_NO_STEP2_CMAC_JOB_SLICE_1:
            // SERVICE_01_AES_CMAC Generate (AES_8, job_slice = 1)
            if( sw_key_mng ){
                return(-1);
            }
            if (RANDOM_TEST == test_type) sw_key_mng = 1;
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_step2_cmac_job_slice_1;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 8;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_cmac_msg_512bits;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = sizeof(test_cmac_msg_512bits) * 8;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&BIG_BUFFER[1][1340];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 2;
            break;

        case TEST_NO_STEP2_CMAC_FAST_A:
            // SERVICE_01_AES_CMAC (STEP2 AES_12, priority Highest)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_step2_cmac_fast_A;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_HIGHEST;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 12;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_msg_cmac_fast_128bits;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = sizeof(test_msg_cmac_fast_128bits) * 8;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&BIG_BUFFER[2][1588];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 0;
            SYNC_CMAC_FAST = CMAC_EXIST;
            break;

        case TEST_NO_STEP2_CMAC_FAST_B:
            // SERVICE_01_AES_CMAC (STEP2 AES_13, priority Highest)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_step2_cmac_fast_B;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_HIGHEST;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 13;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_msg_cmac_fast_128bits;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = sizeof(test_msg_cmac_fast_128bits) * 8;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&BIG_BUFFER[2][1572];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 0;
            SYNC_CMAC_FAST = CMAC_EXIST;
            break;

        case TEST_NO_AES_CMAC_VERI:
            // SERVICE_01_AES_CMAC Verification (no job_slice, priority normal)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_NORMAL;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_cmac_veri;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 3;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_cmac_msg_512bits;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = sizeof(test_cmac_msg_512bits) * 8;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&test_cmac_value_128bits[0];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 1;
            p_ISD[isd_index]->prm.AES_CMAC.verification_result = VERIFICATION_FAIL;
            break;


        case TEST_NO_AES_CIPHER_CTR_ENC:
            // SERVICE_01_AES_CIPHER (AES_7, CTR, ENC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_ctr_enc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][1260];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 4;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 7;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CTR;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CTR_DEC:
            // SERVICE_01_AES_CIPHER (AES_7, CTR, DEC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_ctr_dec;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_cipher_data_ctr;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][1340];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 4;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 7;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CTR;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CBC_ENC:
            // SERVICE_01_AES_CIPHER (AES_7, CBC, ENC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_cbc_enc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][1420];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 4;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 7;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CBC_DEC:
            // SERVICE_01_AES_CIPHER (AES_7, CBC, DEC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_cbc_dec;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_cipher_data_cbc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][1500];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 4;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 7;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;


        case TEST_NO_AES_CIPHER_192:
            // SERVICE_01_AES_CIPHER (AES_18,192bit)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_192;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_192;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in_192;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][1540];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 18;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_256:
            // SERVICE_01_AES_CIPHER (AES_19,256bit)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][1580];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 19;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CMAC_192:
            // SERVICE_01_AES_CMAC (AES_20,192bit)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_NORMAL;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_cmac_192;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 20;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_p_msg_192;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = 256;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&BIG_BUFFER[2][1620];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 0;
            break;

        case TEST_NO_AES_CMAC_256:
            // SERVICE_01_AES_CMAC (AES_21,256bit)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_NORMAL;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_cmac_256;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 21;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_p_msg_256;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = 384;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&BIG_BUFFER[2][1660];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 10 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 0;
            break;

        case TEST_NO_AES_AUTH_CIPHER_192:
            // SERVICE_01_AES_AUTH_CIPHER (AES_22,192bit)
            p_ISD[isd_index]->service_id                                    = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                        = 0;
            p_ISD[isd_index]->p_callbackfunc                                = (p_func_t)&isd_call_back_aes_auth_cipher_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                      = (const uint32_t *)&test_p_iv_auth_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data               = (const uint32_t *)&test_p_auth_data_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in                 = (const uint32_t *)&test_p_data_in_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out                = (uint32_t *)&BIG_BUFFER[2][1700];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag                = (uint32_t *)&BIG_BUFFER[2][1740];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits         = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits    = 160;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits           = 96;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits     = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group                 = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                    = 22;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction                 = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode               = AUTH_CIPHER_MOD_GCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice                 = 0;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle                 = (r_job_cycle_t)0;
            break;

        case TEST_NO_AES_AUTH_CIPHER_256:
            // SERVICE_01_AES_AUTH_CIPHER (AES_23,256bit)
            p_ISD[isd_index]->service_id                                    = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                        = 0;
            p_ISD[isd_index]->p_callbackfunc                                = (p_func_t)&isd_call_back_aes_auth_cipher_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                      = (const uint32_t *)&test_p_iv_auth_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data               = (const uint32_t *)&test_p_auth_data_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in                 = (const uint32_t *)&test_p_data_in_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out                = (uint32_t *)&BIG_BUFFER[2][1780];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag                = (uint32_t *)&BIG_BUFFER[2][1820];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits         = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits    = 160;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits           = 96;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits     = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group                 = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                    = 23;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction                 = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode               = AUTH_CIPHER_MOD_GCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice                 = 0;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle                 = (r_job_cycle_t)0;
            break;

        case TEST_NO_AES_CIPHER_192_SLICE:
            // SERVICE_01_AES_CIPHER_SLICE (AES_18,192bit)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_192_slice;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_192;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in_192;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][1540];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 18;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 2;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_256_SLICE:
            // SERVICE_01_AES_CIPHER_SLICE (AES_19,256bit)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_256_slice;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][1580];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 19;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 2;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CMAC_192_SLICE:
            // SERVICE_01_AES_CMAC_SLICE (AES_20,192bit)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_NORMAL;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_cmac_192_slice;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 20;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_p_msg_192;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = 256;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&BIG_BUFFER[2][1620];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 2;
            break;

        case TEST_NO_AES_CMAC_256_SLICE:
            // SERVICE_01_AES_CMAC_SLICE (AES_21,256bit)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_NORMAL;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_cmac_256_slice;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 21;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_p_msg_256;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = 384;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&BIG_BUFFER[2][1660];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 10 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 2;
            break;

        case TEST_NO_AES_AUTH_CIPHER_192_SLICE:
            // SERVICE_01_AES_AUTH_CIPHER_SLICE (AES_22,192bit)
            p_ISD[isd_index]->service_id                                    = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                        = 0;
            p_ISD[isd_index]->p_callbackfunc                                = (p_func_t)&isd_call_back_aes_auth_cipher_192_slice;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                      = (const uint32_t *)&test_p_iv_auth_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data               = (const uint32_t *)&test_p_auth_data_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in                 = (const uint32_t *)&test_p_data_in_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out                = (uint32_t *)&BIG_BUFFER[2][1700];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag                = (uint32_t *)&BIG_BUFFER[2][1740];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits         = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits    = 160;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits           = 96;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits     = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group                 = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                    = 22;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction                 = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode               = AUTH_CIPHER_MOD_GCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice                 = 2;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle                 = (r_job_cycle_t)0;
            break;

        case TEST_NO_AES_AUTH_CIPHER_256_SLICE:
            // SERVICE_01_AES_AUTH_CIPHER_SLICE (AES_23,256bit)
            p_ISD[isd_index]->service_id                                    = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                        = 0;
            p_ISD[isd_index]->p_callbackfunc                                = (p_func_t)&isd_call_back_aes_auth_cipher_256_slice;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                      = (const uint32_t *)&test_p_iv_auth_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data               = (const uint32_t *)&test_p_auth_data_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in                 = (const uint32_t *)&test_p_data_in_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out                = (uint32_t *)&BIG_BUFFER[2][1780];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag                = (uint32_t *)&BIG_BUFFER[2][1820];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits         = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits    = 160;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits           = 96;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits     = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group                 = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                    = 23;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction                 = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode               = AUTH_CIPHER_MOD_GCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice                 = 2;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle                 = (r_job_cycle_t)0;
            break;

        case TEST_NO_AES_KEY_UPDATE_PLAIN_192:
            // SERVICE_03_AES_KEY_UPDATE_PLAIN (192bit)
            p_ISD[isd_index]->service_id                                   = SERVICE_03_AES_KEY_UPDATE_PLAIN;
            p_ISD[isd_index]->job_id                                       = 0;
            p_ISD[isd_index]->p_callbackfunc                               = (p_func_t)&isd_call_back_aes_update_plain_192;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.p_key_value             = (const uint8_t *)&KEY_192;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.key_id                  = 24;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes       = 24;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.encryption        = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.decryption        = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation   = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug  = 0;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.write_protected   = 0;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.no_wild_card      = 0;
            break;

        case TEST_NO_AES_KEY_UPDATE_PLAIN_256:
            // SERVICE_03_AES_KEY_UPDATE_PLAIN (256bit)
            p_ISD[isd_index]->service_id                                   = SERVICE_03_AES_KEY_UPDATE_PLAIN;
            p_ISD[isd_index]->job_id                                       = 0;
            p_ISD[isd_index]->p_callbackfunc                               = (p_func_t)&isd_call_back_aes_update_plain_256;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.p_key_value             = (const uint8_t *)&KEY_256;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.key_id                  = 25;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes       = 32;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.encryption        = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.decryption        = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation   = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification = 1;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug  = 0;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.write_protected   = 0;
            p_ISD[isd_index]->prm.KEY_UPDATE_PLAIN.flags.no_wild_card      = 0;
            break;

        case TEST_NO_ECDH_192:
            // SERVICE_01_ECDH (192bit, AES_RAM_2)
            p_ISD[isd_index]->service_id                                       = SERVICE_01_ECDH;
            p_ISD[isd_index]->job_id                                           = 0;
            p_ISD[isd_index]->p_callbackfunc                                   = (p_func_t)&isd_call_back_ecdh_192;
            p_ISD[isd_index]->prm.ECDH.ecc_priv_key_group                      = KEY_GRP_ECC;
            p_ISD[isd_index]->prm.ECDH.ecc_priv_key_id                         = 1;
            p_ISD[isd_index]->prm.ECDH.ecc_pub_key_group                       = KEY_GRP_ECC_PUB;
            p_ISD[isd_index]->prm.ECDH.ecc_pub_key_id                          = 1;
            p_ISD[isd_index]->prm.ECDH.shared_key_group                        = KEY_GRP_AES_RAM;
            p_ISD[isd_index]->prm.ECDH.shared_key_id                           = 2;
            p_ISD[isd_index]->prm.ECDH.kdf_algo                                = KDF_ALGO_KDF2;
            p_ISD[isd_index]->prm.ECDH.hash_primitive                          = HASH_PRIMITIVE_SHA2_384;
            p_ISD[isd_index]->prm.ECDH.job_slice                               = 1;
            p_ISD[isd_index]->prm.ECDH.p_info                                  = 0;
            p_ISD[isd_index]->prm.ECDH.counter                                 = 4;
            p_ISD[isd_index]->prm.ECDH.info_size_in_bytes                      = 0;
            p_ISD[isd_index]->prm.ECDH.key_size_in_bytes                       = 24;
            break;

        case TEST_NO_ECDH_256:
            // SERVICE_01_ECDH (256bit, AES_RAM_3)
            p_ISD[isd_index]->service_id                                       = SERVICE_01_ECDH;
            p_ISD[isd_index]->job_id                                           = 0;
            p_ISD[isd_index]->p_callbackfunc                                   = (p_func_t)&isd_call_back_ecdh_256;
            p_ISD[isd_index]->prm.ECDH.ecc_priv_key_group                      = KEY_GRP_ECC;
            p_ISD[isd_index]->prm.ECDH.ecc_priv_key_id                         = 1;
            p_ISD[isd_index]->prm.ECDH.ecc_pub_key_group                       = KEY_GRP_ECC_PUB;
            p_ISD[isd_index]->prm.ECDH.ecc_pub_key_id                          = 1;
            p_ISD[isd_index]->prm.ECDH.shared_key_group                        = KEY_GRP_AES_RAM;
            p_ISD[isd_index]->prm.ECDH.shared_key_id                           = 3;
            p_ISD[isd_index]->prm.ECDH.kdf_algo                                = KDF_ALGO_KDF2;
            p_ISD[isd_index]->prm.ECDH.hash_primitive                          = HASH_PRIMITIVE_SHA2_384;
            p_ISD[isd_index]->prm.ECDH.job_slice                               = 1;
            p_ISD[isd_index]->prm.ECDH.p_info                                  = 0;
            p_ISD[isd_index]->prm.ECDH.counter                                 = 4;
            p_ISD[isd_index]->prm.ECDH.info_size_in_bytes                      = 0;
            p_ISD[isd_index]->prm.ECDH.key_size_in_bytes                       = 32;
            break;

        case TEST_NO_EDDSA_SIGN:
            // SERVICE_01_EDDSA_SIGN
            p_ISD[isd_index]->service_id                                   = SERVICE_01_EDDSA_SIGN;
            p_ISD[isd_index]->job_id                                       = 0;
            p_ISD[isd_index]->p_callbackfunc                               = (p_func_t)&isd_call_back_eddsa_sign;
            p_ISD[isd_index]->prm.EDDSA_SIGN.p_source                      = (const uint8_t *)&test_eddsa_msg;
            p_ISD[isd_index]->prm.EDDSA_SIGN.signature[0].p_data           = &BIG_BUFFER[0][2048];
            p_ISD[isd_index]->prm.EDDSA_SIGN.signature[1].p_data           = &BIG_BUFFER[0][2108];
            p_ISD[isd_index]->prm.EDDSA_SIGN.source_size_in_bytes          = 2;
            p_ISD[isd_index]->prm.EDDSA_SIGN.hash_primitive                = HASH_PRIMITIVE_SHA2_512;
            p_ISD[isd_index]->prm.EDDSA_SIGN.signature_format              = FORMAT_BINARY_PLAIN;
            p_ISD[isd_index]->prm.EDDSA_SIGN.key_group                     = KEY_GRP_ECC;
            p_ISD[isd_index]->prm.EDDSA_SIGN.key_id                        = 4;
            p_ISD[isd_index]->prm.EDDSA_SIGN.job_slice_hash                = 0;
            p_ISD[isd_index]->prm.EDDSA_SIGN.job_slice_point_mul           = 0;
            p_ISD[isd_index]->prm.EDDSA_SIGN.signature_size_in_bits[0]     = 256;
            p_ISD[isd_index]->prm.EDDSA_SIGN.signature_size_in_bits[1]     = 256;
            p_ISD[isd_index]->prm.EDDSA_SIGN.is_precalculated_hash         = 0;

            break;

        case TEST_NO_EDDSA_VERIFY:
            // SERVICE_01_EDDSA_VERIFY
            p_ISD[isd_index]->service_id                                   = SERVICE_01_EDDSA_VERIFY;
            p_ISD[isd_index]->job_id                                       = 0;
            p_ISD[isd_index]->p_callbackfunc                               = (p_func_t)&isd_call_back_eddsa_verify;
            p_ISD[isd_index]->prm.EDDSA_VERIFY.p_source                    = (const uint8_t *)&test_eddsa_msg;
            p_ISD[isd_index]->prm.EDDSA_VERIFY.signature[0].p_data         = (const uint8_t *)&test_eddsa_signature_0;
            p_ISD[isd_index]->prm.EDDSA_VERIFY.signature[1].p_data         = (const uint8_t *)&test_eddsa_signature_1;
            p_ISD[isd_index]->prm.EDDSA_VERIFY.source_size_in_bytes        = 2;
            p_ISD[isd_index]->prm.EDDSA_VERIFY.signature_size_in_bits[0]   = 256;
            p_ISD[isd_index]->prm.EDDSA_VERIFY.signature_size_in_bits[1]   = 256;
            p_ISD[isd_index]->prm.EDDSA_VERIFY.hash_primitive              = HASH_PRIMITIVE_SHA2_512;
            p_ISD[isd_index]->prm.EDDSA_VERIFY.signature_format            = FORMAT_BINARY_PLAIN;
            p_ISD[isd_index]->prm.EDDSA_VERIFY.key_group                   = KEY_GRP_ECC;
            p_ISD[isd_index]->prm.EDDSA_VERIFY.key_id                      = 4;
            p_ISD[isd_index]->prm.EDDSA_VERIFY.job_slice_hash              = 0;
            p_ISD[isd_index]->prm.EDDSA_VERIFY.job_slice_point_mul         = 0;
            p_ISD[isd_index]->prm.EDDSA_VERIFY.is_precalculated_hash       = 0;

            break;

        case TEST_NO_AES_CMAC_VERI_192:
            // SERVICE_01_AES_CMAC
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_NORMAL;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_cmac_veri_192;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 28;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_cmac_msg_192bits;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = sizeof(test_cmac_msg_192bits) * 8;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&test_cmac_ver_value_192bits[0];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 10 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 0;
            p_ISD[isd_index]->prm.AES_CMAC.verification_result = VERIFICATION_FAIL;
            break;


        case TEST_NO_AES_CIPHER_CTR_ENC_192:
            // SERVICE_01_AES_CIPHER (AES_7, CTR, ENC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_ctr_enc_192;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_192_ctr;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in_192_ctr;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][3280];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 29;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CTR;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CTR_DEC_192:
            // SERVICE_01_AES_CIPHER (AES_7, CTR, DEC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_ctr_dec_192;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_192_ctr;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_cipher_data_ctr_192;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][3320];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 29;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CTR;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CBC_ENC_192:
            // SERVICE_01_AES_CIPHER (AES_7, CBC, ENC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_cbc_enc_192;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_192_cbc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in_192_cbc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][3360];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 30;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CBC_DEC_192:
            // SERVICE_01_AES_CIPHER (AES_7, CBC, DEC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_cbc_dec_192;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_192_cbc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_cipher_data_cbc_192;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][3400];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 30;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_AUTH_CIPHER_CCM_ENC_192:
            // SERVICE_01_AES_AUTH_CIPHER (ENC CCM, AES_17)
            p_ISD[isd_index]->service_id                                  = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                      = 0;
            p_ISD[isd_index]->p_callbackfunc                              = (p_func_t)&isd_call_back_aes_auth_cipher_ccm_enc_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                  = 33;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                    = (const uint32_t *)test_p_iv_auth_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits         = 104;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in               = (const uint32_t *)test_p_plain_text_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits       = 192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data             = (const uint32_t *)test_p_aad_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)&BIG_BUFFER[2][3480];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)&BIG_BUFFER[2][3520];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice               = 0;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_AUTH_CIPHER_CCM_DEC_192:
            // SERVICE_01_AES_AUTH_CIPHER (DEC CCM, AES_17)
            p_ISD[isd_index]->service_id                                  = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                      = 0;
            p_ISD[isd_index]->p_callbackfunc                              = (p_func_t)&isd_call_back_aes_auth_cipher_ccm_dec_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                  = 33;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                    = (const uint32_t *)test_p_iv_auth_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits         = 104;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in               = (const uint32_t *)test_p_cipher_text_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits       = 192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data             = (const uint32_t *)test_p_aad_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)&BIG_BUFFER[2][3560];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)&test_p_auth_tag_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice               = 0;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CMAC_VERI_256:
            // SERVICE_01_AES_CMAC Verification AES_28
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_NORMAL;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_cmac_veri_256;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 36;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_cmac_msg_256bits;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = sizeof(test_cmac_msg_256bits) * 8;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&test_cmac_ver_value_256bits[0];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 10 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 0;
            p_ISD[isd_index]->prm.AES_CMAC.verification_result = VERIFICATION_FAIL;
            break;


        case TEST_NO_AES_CIPHER_CTR_ENC_256:
            // SERVICE_01_AES_CIPHER (AES_7, CTR, ENC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_ctr_enc_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_256_ctr;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in_256_ctr;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][3680];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 37;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CTR;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CTR_DEC_256:
            // SERVICE_01_AES_CIPHER (AES_7, CTR, DEC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_ctr_dec_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_256_ctr;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_cipher_data_ctr_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][3720];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 37;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CTR;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CBC_ENC_256:
            // SERVICE_01_AES_CIPHER (AES_7, CBC, ENC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_cbc_enc_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_256_cbc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in_256_cbc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][3760];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 38;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CBC_DEC_256:
            // SERVICE_01_AES_CIPHER (AES_7, CBC, DEC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_cbc_dec_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_256_cbc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_cipher_data_cbc_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[2][3800];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 38;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_XTS_256:
            // SERVICE_01_AES_CIPHER (XTS, AES_14, AES_15)
            p_ISD[isd_index]->service_id                            = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                                = 0;
            p_ISD[isd_index]->p_callbackfunc                        = (p_func_t)&isd_call_back_aes_cipher_xts_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv                   = (const uint32_t *)&test_p_iv_xts_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in             = (const uint32_t *)&test_p_blick_in_xts_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out            = (uint32_t *)&BIG_BUFFER[2][3840];
            p_ISD[isd_index]->prm.AES_CIPHER.size.data_size_in_bits = 256;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group              = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id                 = 39;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2            = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2               = 40;
            p_ISD[isd_index]->prm.AES_CIPHER.direction              = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode            = CIPHER_MOD_XTS;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice              = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle              = (r_job_cycle_t)0;
            break;

        case TEST_NO_AES_AUTH_CIPHER_CCM_ENC_256:
            // SERVICE_01_AES_AUTH_CIPHER (ENC CCM, AES_17)
            p_ISD[isd_index]->service_id                                  = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                      = 0;
            p_ISD[isd_index]->p_callbackfunc                              = (p_func_t)&isd_call_back_aes_auth_cipher_ccm_enc_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                  = 41;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                    = (const uint32_t *)test_p_iv_auth_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits         = 104;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in               = (const uint32_t *)test_p_plain_text_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits       = 192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data             = (const uint32_t *)test_p_aad_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)&BIG_BUFFER[2][3880];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)&BIG_BUFFER[2][3920];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice               = 0;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_AUTH_CIPHER_CCM_DEC_256:
            // SERVICE_01_AES_AUTH_CIPHER (DEC CCM, AES_17)
            p_ISD[isd_index]->service_id                                  = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                      = 0;
            p_ISD[isd_index]->p_callbackfunc                              = (p_func_t)&isd_call_back_aes_auth_cipher_ccm_dec_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                  = 41;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                    = (const uint32_t *)test_p_iv_auth_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits         = 104;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in               = (const uint32_t *)test_p_cipher_text_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits       = 192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data             = (const uint32_t *)test_p_aad_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)&BIG_BUFFER[2][3960];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)&test_p_auth_tag_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice               = 0;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CMAC_VERI_192_SLICE:
            // SERVICE_01_AES_CMAC Verification AES_28
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_NORMAL;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_cmac_veri_192_slice;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 28;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_cmac_msg_192bits;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = sizeof(test_cmac_msg_192bits) * 8;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&test_cmac_ver_value_192bits[0];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 10 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 2;
            p_ISD[isd_index]->prm.AES_CMAC.verification_result = VERIFICATION_FAIL;
            break;


        case TEST_NO_AES_CIPHER_CTR_ENC_192_SLICE:
            // SERVICE_01_AES_CIPHER (AES_7, CTR, ENC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_ctr_enc_192_slice;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_192_ctr;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in_192_ctr;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[1][3280];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 29;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CTR;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 2;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CTR_DEC_192_SLICE:
            // SERVICE_01_AES_CIPHER (AES_7, CTR, DEC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_ctr_dec_192_slice;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_192_ctr;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_cipher_data_ctr_192;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[1][3320];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 29;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CTR;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 2;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CBC_ENC_192_SLICE:
            // SERVICE_01_AES_CIPHER (AES_7, CBC, ENC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_cbc_enc_192_slice;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_192_cbc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in_192_cbc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[1][3360];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 30;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 2;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CBC_DEC_192_SLICE:
            // SERVICE_01_AES_CIPHER (AES_7, CBC, DEC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_cbc_dec_192_slice;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_192_cbc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_cipher_data_cbc_192;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[1][3400];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 30;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 2;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_AUTH_CIPHER_CCM_ENC_192_SLICE:
            // SERVICE_01_AES_AUTH_CIPHER (ENC CCM, AES_17)
            p_ISD[isd_index]->service_id                                  = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                      = 0;
            p_ISD[isd_index]->p_callbackfunc                              = (p_func_t)&isd_call_back_aes_auth_cipher_ccm_enc_192_slice;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                  = 33;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                    = (const uint32_t *)test_p_iv_auth_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits         = 104;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in               = (const uint32_t *)test_p_plain_text_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits       = 192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data             = (const uint32_t *)test_p_aad_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)&BIG_BUFFER[1][3440];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)&BIG_BUFFER[1][3480];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice               = 2;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_AUTH_CIPHER_CCM_DEC_192_SLICE:
            // SERVICE_01_AES_AUTH_CIPHER (DEC CCM, AES_17)
            p_ISD[isd_index]->service_id                                  = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                      = 0;
            p_ISD[isd_index]->p_callbackfunc                              = (p_func_t)&isd_call_back_aes_auth_cipher_ccm_dec_192_slice;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                  = 33;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                    = (const uint32_t *)test_p_iv_auth_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits         = 104;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in               = (const uint32_t *)test_p_cipher_text_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits       = 192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data             = (const uint32_t *)test_p_aad_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)&BIG_BUFFER[1][3520];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)&test_p_auth_tag_ccm_192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice               = 2;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CMAC_VERI_256_SLICE:
            // SERVICE_01_AES_CMAC Verification AES_28
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CMAC;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->service_priority                 = (r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_NORMAL;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_cmac_veri_256_slice;
            p_ISD[isd_index]->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CMAC.key_id              = 36;
            p_ISD[isd_index]->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
            p_ISD[isd_index]->prm.AES_CMAC.p_msg               = (const uint32_t *)&test_cmac_msg_256bits;
            p_ISD[isd_index]->prm.AES_CMAC.msg_size_in_bits    = sizeof(test_cmac_msg_256bits) * 8;
            p_ISD[isd_index]->prm.AES_CMAC.p_cmac              = (uint32_t *)&test_cmac_ver_value_256bits[0];
            p_ISD[isd_index]->prm.AES_CMAC.cmac_size_in_bits   = 10 * 8;
            p_ISD[isd_index]->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;
            p_ISD[isd_index]->prm.AES_CMAC.job_slice           = 2;
            p_ISD[isd_index]->prm.AES_CMAC.verification_result = VERIFICATION_FAIL;
            break;


        case TEST_NO_AES_CIPHER_CTR_ENC_256_SLICE:
            // SERVICE_01_AES_CIPHER (AES_7, CTR, ENC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_ctr_enc_256_slice;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_256_ctr;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in_256_ctr;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[1][3680];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 37;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CTR;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 2;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CTR_DEC_256_SLICE:
            // SERVICE_01_AES_CIPHER (AES_7, CTR, DEC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_ctr_dec_256_slice;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_256_ctr;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_cipher_data_ctr_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[1][3720];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 37;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CTR;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 2;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CBC_ENC_256_SLICE:
            // SERVICE_01_AES_CIPHER (AES_7, CBC, ENC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_cbc_enc_256_slice;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_256_cbc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_p_blick_in_256_cbc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[1][3760];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 38;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 2;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_CBC_DEC_256_SLICE:
            // SERVICE_01_AES_CIPHER (AES_7, CBC, DEC)
            p_ISD[isd_index]->service_id                       = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                           = 0;
            p_ISD[isd_index]->p_callbackfunc                   = (p_func_t)&isd_call_back_aes_cipher_cbc_dec_256_slice;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv              = (const uint32_t *)&test_p_iv_chipher_256_cbc;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in        = (const uint32_t *)&test_cipher_data_cbc_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out       = (uint32_t *)&BIG_BUFFER[1][3800];
            p_ISD[isd_index]->prm.AES_CIPHER.size.nb_blocks    = 1;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group         = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id            = 38;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2       = (r_key_group_t)0;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2          = 0;
            p_ISD[isd_index]->prm.AES_CIPHER.direction         = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode       = CIPHER_MOD_CBC;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice         = 2;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle         = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_CIPHER_XTS_256_SLICE:
            // SERVICE_01_AES_CIPHER (XTS, AES_14, AES_15)
            p_ISD[isd_index]->service_id                            = SERVICE_01_AES_CIPHER;
            p_ISD[isd_index]->job_id                                = 0;
            p_ISD[isd_index]->p_callbackfunc                        = (p_func_t)&isd_call_back_aes_cipher_xts_256_slice;
            p_ISD[isd_index]->prm.AES_CIPHER.p_iv                   = (const uint32_t *)&test_p_iv_xts_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_in             = (const uint32_t *)&test_p_blick_in_xts_256;
            p_ISD[isd_index]->prm.AES_CIPHER.p_block_out            = (uint32_t *)&BIG_BUFFER[1][3840];
            p_ISD[isd_index]->prm.AES_CIPHER.size.data_size_in_bits = 256;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group              = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id                 = 39;
            p_ISD[isd_index]->prm.AES_CIPHER.key_group_2            = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_CIPHER.key_id_2               = 40;
            p_ISD[isd_index]->prm.AES_CIPHER.direction              = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_CIPHER.cipher_mode            = CIPHER_MOD_XTS;
            p_ISD[isd_index]->prm.AES_CIPHER.job_slice              = 2;
            p_ISD[isd_index]->prm.AES_CIPHER.job_cycle              = (r_job_cycle_t)0;
            break;

        case TEST_NO_AES_AUTH_CIPHER_CCM_ENC_256_SLICE:
            // SERVICE_01_AES_AUTH_CIPHER (ENC CCM, AES_17)
            p_ISD[isd_index]->service_id                                  = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                      = 0;
            p_ISD[isd_index]->p_callbackfunc                              = (p_func_t)&isd_call_back_aes_auth_cipher_ccm_enc_256_slice;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                  = 41;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_ENCRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                    = (const uint32_t *)test_p_iv_auth_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits         = 104;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in               = (const uint32_t *)test_p_plain_text_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits       = 192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data             = (const uint32_t *)test_p_aad_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)&BIG_BUFFER[1][3880];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)&BIG_BUFFER[1][3920];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice               = 2;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
            break;

        case TEST_NO_AES_AUTH_CIPHER_CCM_DEC_256_SLICE:
            // SERVICE_01_AES_AUTH_CIPHER (DEC CCM, AES_17)
            p_ISD[isd_index]->service_id                                  = SERVICE_01_AES_AUTH_CIPHER;
            p_ISD[isd_index]->job_id                                      = 0;
            p_ISD[isd_index]->p_callbackfunc                              = (p_func_t)&isd_call_back_aes_auth_cipher_ccm_dec_256_slice;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_group               = KEY_GRP_AES;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.key_id                  = 41;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.direction               = CIPHER_DIR_DECRYPTION;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.cipher_mode             = AUTH_CIPHER_MOD_CCM;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_iv                    = (const uint32_t *)test_p_iv_auth_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.iv_size_in_bits         = 104;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_in               = (const uint32_t *)test_p_cipher_text_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.data_size_in_bits       = 192;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_data             = (const uint32_t *)test_p_aad_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_data_size_in_bits  = 256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_data_out              = (uint32_t *)&BIG_BUFFER[1][3960];
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.p_auth_tag              = (uint32_t *)&test_p_auth_tag_ccm_256;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits   = 128;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_slice               = 2;
            p_ISD[isd_index]->prm.AES_AUTH_CIPHER.job_cycle               = JOB_ALL_AT_ONCE;
            break;


        case TEST_NO_SDF_GC_REQ:
        {
            /* SERVICE_04_ICUMDF_GC (GC all/GC 1block) */
            gc_operation_t gc_type = (gc_operation_t)(rand() % 2);
            p_ISD[isd_index]->service_id               = SERVICE_04_ICUMDF_GC;
            p_ISD[isd_index]->p_callbackfunc           = (p_func_t)&isd_call_back_start_gc;
            p_ISD[isd_index]->job_id                   = 0;
            p_ISD[isd_index]->prm.SECURE_FLASH_MGMT.gc_operation = gc_type;
            break;
        }

        default:
            ret = -1;
            break;
    }
    return(ret);
}

/*******************************************************************************
* Wait milli msec
*******************************************************************************/
static void wait_milli_sec(int wait_msec) {
    int timestamp_msec = 0;
    timer_id_t timer_id = VRUN_TIMER_ID_PE1;
    
    /* select timer id */
#if   (D_PEID == 2)
    timer_id = VRUN_TIMER_ID_PE2;
#elif (D_PEID == 3)
    timer_id = VRUN_TIMER_ID_PE3;
#elif (D_PEID == 4)
    timer_id = VRUN_TIMER_ID_PE4;
#elif (D_PEID == 5)
    timer_id = VRUN_TIMER_ID_PE5;
#elif (D_PEID == 6)
    timer_id = VRUN_TIMER_ID_PE6;
#endif

    vrun_chrono_start_id(timer_id);
    while(1) {
        vrun_chrono_stop_id(timer_id);
        vrun_chrono_gettimestamp_id(timer_id);
        timestamp_msec = vrun_chrono_getmsec_id(timer_id);
        if( timestamp_msec < wait_msec ) {
            __asm("nop");
        } else {
            break;
        }
    }
}

// ********************************************************************************
// Testing HeatRun Parallel PE Trigger Random Service
// ********************************************************************************
void icum_test_pe2_heat_run_parallel_pe(void)
{
    r_icumif_isd_t *p_ISD[JOB_QUE_FULL_COUNT];
    int isd_length              = JOB_QUE_FULL_COUNT;
    int isd_index               = 0;
    int i                       = 0;
    int select_index            = 0;
    int result_trigger          = 0;
    int service_no              = 0;
    int interval_msec           = 0;
    int min_interval_msec       = 25;
    int max_interval_msec       = 50;
    int ret;

    sw_update_she = 0;
    sw_key_mng = 0;
    sw_err_detect = 0;

    // Initialize p_ISD
    p_ISD[0]  = (r_icumif_isd_t *)ISD_BUFFER;
    memset(p_ISD[0], 0x00, sizeof(r_icumif_isd_t));
    for (i = 1; i < isd_length; i++) {
        p_ISD[i]  = p_ISD[i - 1] + 1;
        memset(p_ISD[i], 0x00, sizeof(r_icumif_isd_t));
    }

    /* Initialize rsa signature for TEST_NO_RSA_VERIFY_JOB_SLICE_0 and TEST_NO_RSA_SIGN */
    memcpy(&BIG_BUFFER[1][1360],test_rsa_signature_128byte,sizeof(test_rsa_signature_128byte));

    // Initialize rand seed for service interval time
    srand((unsigned)time(NULL));

    service_no = sizeof(test_service) / sizeof(test_service_t);

    // Heat run start
    cnt_trigger_service_request  = 0;
    cnt_recieve_service_response = 0;
    sw_err_detect                = 0;

    while (notification_from_pe1 != END_ST_RAND_SERVICE_TO_PE2) {

        /* update ISD index */
        if (isd_index < isd_length - 1) {
            isd_index++;
        } else {
            isd_index = 0;
        }

        /* check previous service of this ISD is completed or not */
        if( (R_ICUMIF_IsServiceCompleted(p_ISD[isd_index]) == R_ICUMIF_RTN_SERV_COMPLETION) ||
            (0 == p_ISD[isd_index]->req_res_status) ) {

            /* previous service of this ISD is arleady completed or software error (semaphore timeout) */
            /* this ISD is able to execute new srevice */

        } else {

            /* previous service of this ISD is still NOT completed */
            /* continue to next loop and update isd_index */
            continue;
        }

        // Select service at random
        while(1){
            select_index = rand() % service_no;
            if (sw_gc_warning > 0)
            {
                start_gc_wait(p_ISD[isd_index]);
            }
            ret = select_service(RANDOM_TEST, p_ISD, isd_index, select_index);
            if( ret != -1 ){
                break;
            }
        }

        // Trigger the service request
        result_trigger = R_ICUMIF_ServiceRequest(p_ISD[isd_index]);
        cnt_trigger_service_request++;

        // Check service request error
        if (result_trigger < 0) {

            switch (result_trigger)
            {
            case R_ICUMIF_ER_REQ_QUEUE_FULL:
                cnt_request_queue_full++;
                wait_milli_sec(interval_msec);
                break;
            
            case R_ICUMIF_ER_IF_LIB_NOT_INIT:
                cnt_if_lib_not_init++;
                break;

            case R_ICUMIF_ER_SOFTWARE_ERROR:
                cnt_software_error++;
                break;

            default:
                break;
            }

            if (TEST_NO_STEP2_CMAC_JOB_SLICE_1 == select_index)
            {
                sw_key_mng = 0;
            }

            if ((TEST_NO_STEP2_CMAC_FAST_A == select_index)
                   || (TEST_NO_STEP2_CMAC_FAST_B == select_index)
                   || (TEST_NO_STEP3_CMAC_FAST_A_192 == select_index)
                   || (TEST_NO_STEP3_CMAC_FAST_B_192 == select_index)
                   || (TEST_NO_STEP3_CMAC_FAST_A_256 == select_index)
                   || (TEST_NO_STEP3_CMAC_FAST_B_256 == select_index))
            {
                SYNC_CMAC_FAST = CMAC_NOT_EXIST;
            }
        }

        /* Change service interval at random */
        interval_msec = rand() % (max_interval_msec - min_interval_msec) + min_interval_msec;

        /* Wait service inerval time */
        wait_milli_sec(interval_msec);
    }

    // done: synchronize with PE1
    SYNC_PE = SYNC_PE_END;

    // wait notify from PE1
    while (notification_from_pe1 != REQUEST_COUNT_RESULT_TO_PE2) {};

    // Wait job queue empty
    for (i = 0; i < JOB_QUE_WAIT_COUNT; i++) {
        if ((cnt_trigger_service_request -
                (cnt_recieve_service_response + cnt_request_queue_full + cnt_if_lib_not_init + cnt_software_error)) == 0)
        {
            break;
        }

        /* Wait 1msec */
        wait_milli_sec(1);
    }

    /* set count result to global variables */
    CNT_RESULT_PE(D_PEID) = cnt_result;
    CNT_TRIGGER_SERVICE_REQUEST_PE(D_PEID) = cnt_trigger_service_request;
    CNT_RECIEVE_SERVICE_RESPONSE_PE(D_PEID) = cnt_recieve_service_response;
    CNT_REQUEST_QUEUE_FULL_PE(D_PEID) = cnt_request_queue_full;
    CNT_IT_LIB_NOT_INIT_PE(D_PEID) = cnt_if_lib_not_init;
    CNT_SOFTWARE_ERROR_PE(D_PEID) = cnt_software_error;

    SYNC_PE = RESPONSE_COUNT_RESULT_FROM_PE2 + (D_PEID - 2); // response count result to PE1
    REG_PE_IPIRIPI3REQ0 = 0x01;             // PE1 -> PE0 Interrupt Request
    while (SYNC_PE != SYNC_PE_COMPLETED);   // wait notify from PE1

    // wait notify from PE1
    while (notification_from_pe1 != END_ST_RAND_SERVICE_TO_PE2) {};
}

#pragma ghs section bss=default
