/******************************************************************************/
/* Component Name   Testing the ICU-M                                         */
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

/*******************************************************************************
* Headers
*******************************************************************************/

#include <stdio.h>
#include <string.h>

#undef NULL
#include "r_icumif_api.h"
#include "r_icumif_pub.h"

#include "pe1.h"
#include "pe1.int.h"
#include "shared.h"

#include "vrun.api.h"

/*******************************************************************************
* Test functions
*******************************************************************************/

/*===========================================================================
  ICU-M test function
  [PUBLIC]
  ===========================================================================*/
void icum_test_prng_init_rng(void)
{
    r_icumif_isd_t *p_ISD;

    printf("\n");

    /* initialize the global icum service header */
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("Operation:    INIT_RNG PRNG\n");

    p_ISD->service_id                    = SERVICE_02_RAND_INIT_RNG;
    p_ISD->p_callbackfunc                = 0;
    p_ISD->job_id                        = 0;
    p_ISD->prm.RAND_INIT.online_test     = RAND_DIAG_P1_TEST;

    /* pause life signal management */
    pause_life_signal();

    /* start timing measurement */
    vrun_chrono_start();

    /* trigger the security service */
    trigger_service_and_wait(p_ISD);

    /* stop timing measurement */
    vrun_chrono_stop();

    /* in case of error, stop here */
    if(p_ISD->service_result != SERV_OK) goto end;

    /* fetch execution time in vrun_chrono_xxx variables */
    vrun_chrono_gettimestamp();

end:
    /* restart life signal */
    restart_life_signal();

    printf("Done!\n");
}

void icum_test_prng_extend_seed(void)
{
    r_icumif_isd_t *p_ISD;

    printf("\n");

    /* initialize the global icum service header */
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("Operation:    EXTEND_SEED PRNG\n");

    BIG_BUFFER[0][ 0] = 0xae;    BIG_BUFFER[0][ 1] = 0x2d;    BIG_BUFFER[0][ 2] = 0x8a;    BIG_BUFFER[0][ 3] = 0x57;
    BIG_BUFFER[0][ 4] = 0x1e;    BIG_BUFFER[0][ 5] = 0x03;    BIG_BUFFER[0][ 6] = 0xac;    BIG_BUFFER[0][ 7] = 0x9c;
    BIG_BUFFER[0][ 8] = 0x9e;    BIG_BUFFER[0][ 9] = 0xb7;    BIG_BUFFER[0][10] = 0x6f;    BIG_BUFFER[0][11] = 0xac;
    BIG_BUFFER[0][12] = 0x45;    BIG_BUFFER[0][13] = 0xaf;    BIG_BUFFER[0][14] = 0x8e;    BIG_BUFFER[0][15] = 0x51;

    p_ISD->service_id                     = SERVICE_02_RAND_EXTEND_SEED;
    p_ISD->p_callbackfunc                 = 0;
    p_ISD->job_id                         = 0;
    p_ISD->prm.RAND_EXTEND_SEED.p_entropy = (uint32_t*)BIG_BUFFER[0];

    /* pause life signal management */
    pause_life_signal();

    /* start timing measurement */
    vrun_chrono_start();

    /* trigger the security service */
    trigger_service_and_wait(p_ISD);

    /* stop timing measurement */
    vrun_chrono_stop();

    /* in case of error, stop here */
    if(p_ISD->service_result != SERV_OK) goto end;

    /* fetch execution time in vrun_chrono_xxx variables */
    vrun_chrono_gettimestamp();

    /* print throughput */
    print_throughput(SIZE_OF_BIG_BUFFER);

end:
    /* restart life signal */
    restart_life_signal();

    printf("Done!\n");
}

void icum_test_prng_rnd(void)
{
    r_icumif_isd_t *p_ISD;

    printf("\n");

    /* initialize the global icum service header */
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("Operation:    RND PRNG\n");

    p_ISD->service_id                    = SERVICE_02_RAND_RND;
    p_ISD->p_callbackfunc                = 0;
    p_ISD->job_id                        = 0;
    p_ISD->prm.RAND_GENERATE.p_block_out = (uint32_t*)BIG_BUFFER[0];
    p_ISD->prm.RAND_GENERATE.nb_blocks   = 1;
    p_ISD->prm.RAND_GENERATE.job_slice   = 0;

    printf("Job slice:    %d block(s)\n", p_ISD->prm.RAND_GENERATE.job_slice);

    /* pause life signal management */
    pause_life_signal();

    /* start timing measurement */
    vrun_chrono_start();

    /* trigger the security service */
    trigger_service_and_wait(p_ISD);

    /* stop timing measurement */
    vrun_chrono_stop();

    /* in case of error, stop here */
    if(p_ISD->service_result != SERV_OK) goto end;

    /* fetch execution time in vrun_chrono_xxx variables */
    vrun_chrono_gettimestamp();

    /* print throughput */
    print_throughput(SIZE_OF_BIG_BUFFER);

end:
    /* restart life signal */
    restart_life_signal();

    printf("Done!\n");
}

void icum_test_prng_all(void)
{
    r_icumif_isd_t *p_ISD;

    printf("\n");

    /* initialize the global icum service header */
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    /*******************************/
    /* SERVICE_02_RAND_INIT_RNG    */
    /*******************************/
    p_ISD->service_id                    = SERVICE_02_RAND_INIT_RNG;
    p_ISD->p_callbackfunc                = 0;
    p_ISD->job_id                        = 0;
    p_ISD->prm.RAND_INIT.online_test     = RAND_DIAG_P1_TEST;

    /* trigger the security service */
    trigger_service_and_wait(p_ISD);

    /* in case of error, stop here */
    if(p_ISD->service_result != SERV_OK) goto end;

    /*******************************/
    /* SERVICE_02_RAND_EXTEND_SEED */
    /*******************************/
    BIG_BUFFER[1][ 0] = 0x00;    BIG_BUFFER[1][ 1] = 0x00;    BIG_BUFFER[1][ 2] = 0x00;    BIG_BUFFER[1][ 3] = 0x00;
    BIG_BUFFER[1][ 4] = 0x00;    BIG_BUFFER[1][ 5] = 0x00;    BIG_BUFFER[1][ 6] = 0x00;    BIG_BUFFER[1][ 7] = 0x00;
    BIG_BUFFER[1][ 8] = 0x00;    BIG_BUFFER[1][ 9] = 0x00;    BIG_BUFFER[1][10] = 0x00;    BIG_BUFFER[1][11] = 0x00;
    BIG_BUFFER[1][12] = 0x00;    BIG_BUFFER[1][13] = 0x00;    BIG_BUFFER[1][14] = 0x00;    BIG_BUFFER[1][15] = 0x00;

    p_ISD->service_id                    = SERVICE_02_RAND_EXTEND_SEED;
    p_ISD->p_callbackfunc                = 0;
    p_ISD->job_id                        = 1;
    p_ISD->prm.RAND_EXTEND_SEED.p_entropy= (uint32_t*)BIG_BUFFER[1];

    /* trigger the security service */
    trigger_service_and_wait(p_ISD);

    /* in case of error, stop here */
    if(p_ISD->service_result != SERV_OK) goto end;

    /*******************************/
    /* SERVICE_02_RAND_RND         */
    /*******************************/
    p_ISD->service_id                    = SERVICE_02_RAND_RND;
    p_ISD->p_callbackfunc                = 0;
    p_ISD->job_id                        = 2;
    p_ISD->prm.RAND_GENERATE.p_block_out = (uint32_t*)BIG_BUFFER[2];
    p_ISD->prm.RAND_GENERATE.nb_blocks   = 1;
    p_ISD->prm.RAND_GENERATE.job_slice   = 1;

    /* trigger the security service */
    trigger_service_and_wait(p_ISD);

    /* in case of error, stop here */
    if(p_ISD->service_result != SERV_OK) goto end;

    /*******************************/
    /* SERVICE_02_RAND_EXTEND_SEED */
    /*******************************/
    BIG_BUFFER[1][ 0] = 0xae;    BIG_BUFFER[1][ 1] = 0x2d;    BIG_BUFFER[1][ 2] = 0x8a;    BIG_BUFFER[1][ 3] = 0x57;
    BIG_BUFFER[1][ 4] = 0x1e;    BIG_BUFFER[1][ 5] = 0x03;    BIG_BUFFER[1][ 6] = 0xac;    BIG_BUFFER[1][ 7] = 0x9c;
    BIG_BUFFER[1][ 8] = 0x9e;    BIG_BUFFER[1][ 9] = 0xb7;    BIG_BUFFER[1][10] = 0x6f;    BIG_BUFFER[1][11] = 0xac;
    BIG_BUFFER[1][12] = 0x45;    BIG_BUFFER[1][13] = 0xaf;    BIG_BUFFER[1][14] = 0x8e;    BIG_BUFFER[1][15] = 0x51;

    p_ISD->service_id                    = SERVICE_02_RAND_EXTEND_SEED;
    p_ISD->p_callbackfunc                = 0;
    p_ISD->job_id                        = 3;
    p_ISD->prm.RAND_EXTEND_SEED.p_entropy= (uint32_t*)BIG_BUFFER[1];

    /* trigger the security service */
    trigger_service_and_wait(p_ISD);

    /* in case of error, stop here */
    if(p_ISD->service_result != SERV_OK) goto end;

    /*******************************/
    /* SERVICE_02_RAND_KAT         */
    /*******************************/
    BIG_BUFFER[1][ 0] = 0xed;    BIG_BUFFER[1][ 1] = 0x1e;    BIG_BUFFER[1][ 2] = 0x7f;    BIG_BUFFER[1][ 3] = 0x21;
    BIG_BUFFER[1][ 4] = 0xef;    BIG_BUFFER[1][ 5] = 0x66;    BIG_BUFFER[1][ 6] = 0xea;    BIG_BUFFER[1][ 7] = 0x5d;
    BIG_BUFFER[1][ 8] = 0x8e;    BIG_BUFFER[1][ 9] = 0x2a;    BIG_BUFFER[1][10] = 0x85;    BIG_BUFFER[1][11] = 0xb9;
    BIG_BUFFER[1][12] = 0x33;    BIG_BUFFER[1][13] = 0x72;    BIG_BUFFER[1][14] = 0x45;    BIG_BUFFER[1][15] = 0x44;
    BIG_BUFFER[1][16] = 0x5b;    BIG_BUFFER[1][17] = 0x71;    BIG_BUFFER[1][18] = 0xd6;    BIG_BUFFER[1][19] = 0x39;
    BIG_BUFFER[1][20] = 0x3a;    BIG_BUFFER[1][21] = 0x4e;    BIG_BUFFER[1][22] = 0xec;    BIG_BUFFER[1][23] = 0xb0;
    BIG_BUFFER[1][24] = 0xe6;    BIG_BUFFER[1][25] = 0x3c;    BIG_BUFFER[1][26] = 0x19;    BIG_BUFFER[1][27] = 0x3d;
    BIG_BUFFER[1][28] = 0x0f;    BIG_BUFFER[1][29] = 0x72;    BIG_BUFFER[1][30] = 0xf9;    BIG_BUFFER[1][31] = 0xa9;
    BIG_BUFFER[1][32] = 0x30;    BIG_BUFFER[1][33] = 0x3f;    BIG_BUFFER[1][34] = 0xb5;    BIG_BUFFER[1][35] = 0x19;
    BIG_BUFFER[1][36] = 0xf0;    BIG_BUFFER[1][37] = 0xa4;    BIG_BUFFER[1][38] = 0xe1;    BIG_BUFFER[1][39] = 0x7d;
    BIG_BUFFER[1][40] = 0x6d;    BIG_BUFFER[1][41] = 0xf0;    BIG_BUFFER[1][42] = 0xb6;    BIG_BUFFER[1][43] = 0x42;
    BIG_BUFFER[1][44] = 0x6a;    BIG_BUFFER[1][45] = 0xa0;    BIG_BUFFER[1][46] = 0xec;    BIG_BUFFER[1][47] = 0xb2;
    BIG_BUFFER[1][48] = 0xa3;    BIG_BUFFER[1][49] = 0x60;    BIG_BUFFER[1][50] = 0x79;    BIG_BUFFER[1][51] = 0xbd;
    BIG_BUFFER[1][52] = 0x48;    BIG_BUFFER[1][53] = 0xbe;    BIG_BUFFER[1][54] = 0x47;    BIG_BUFFER[1][55] = 0xad;
    BIG_BUFFER[1][56] = 0x2a;    BIG_BUFFER[1][57] = 0x8d;    BIG_BUFFER[1][58] = 0xbf;    BIG_BUFFER[1][59] = 0xe4;
    BIG_BUFFER[1][60] = 0x8d;    BIG_BUFFER[1][61] = 0xa3;    BIG_BUFFER[1][62] = 0xef;    BIG_BUFFER[1][63] = 0xad;

    BIG_BUFFER[2][ 0] = 0xf8;    BIG_BUFFER[2][ 1] = 0x01;    BIG_BUFFER[2][ 2] = 0x11;    BIG_BUFFER[2][ 3] = 0xd0;
    BIG_BUFFER[2][ 4] = 0x8e;    BIG_BUFFER[2][ 5] = 0x87;    BIG_BUFFER[2][ 6] = 0x46;    BIG_BUFFER[2][ 7] = 0x72;
    BIG_BUFFER[2][ 8] = 0xf3;    BIG_BUFFER[2][ 9] = 0x2f;    BIG_BUFFER[2][10] = 0x42;    BIG_BUFFER[2][11] = 0x99;
    BIG_BUFFER[2][12] = 0x71;    BIG_BUFFER[2][13] = 0x33;    BIG_BUFFER[2][14] = 0xa5;    BIG_BUFFER[2][15] = 0x21;
    BIG_BUFFER[2][16] = 0x0f;    BIG_BUFFER[2][17] = 0x7a;    BIG_BUFFER[2][18] = 0x93;    BIG_BUFFER[2][19] = 0x75;
    BIG_BUFFER[2][20] = 0xe2;    BIG_BUFFER[2][21] = 0x2c;    BIG_BUFFER[2][22] = 0xea;    BIG_BUFFER[2][23] = 0x70;
    BIG_BUFFER[2][24] = 0x58;    BIG_BUFFER[2][25] = 0x7f;    BIG_BUFFER[2][26] = 0x9c;    BIG_BUFFER[2][27] = 0xfa;
    BIG_BUFFER[2][28] = 0xfe;    BIG_BUFFER[2][29] = 0xbe;    BIG_BUFFER[2][30] = 0x0f;    BIG_BUFFER[2][31] = 0x6a;
    BIG_BUFFER[2][32] = 0x6a;    BIG_BUFFER[2][33] = 0xa2;    BIG_BUFFER[2][34] = 0xeb;    BIG_BUFFER[2][35] = 0x68;
    BIG_BUFFER[2][36] = 0xe7;    BIG_BUFFER[2][37] = 0xdd;    BIG_BUFFER[2][38] = 0x91;    BIG_BUFFER[2][39] = 0x64;
    BIG_BUFFER[2][40] = 0x53;    BIG_BUFFER[2][41] = 0x6d;    BIG_BUFFER[2][42] = 0x53;    BIG_BUFFER[2][43] = 0xfa;
    BIG_BUFFER[2][44] = 0x02;    BIG_BUFFER[2][45] = 0x0f;    BIG_BUFFER[2][46] = 0xca;    BIG_BUFFER[2][47] = 0xb2;
    BIG_BUFFER[2][48] = 0x0f;    BIG_BUFFER[2][49] = 0x54;    BIG_BUFFER[2][50] = 0xca;    BIG_BUFFER[2][51] = 0xdd;
    BIG_BUFFER[2][52] = 0xfa;    BIG_BUFFER[2][53] = 0xb7;    BIG_BUFFER[2][54] = 0xd6;    BIG_BUFFER[2][55] = 0xd9;
    BIG_BUFFER[2][56] = 0x1e;    BIG_BUFFER[2][57] = 0x5f;    BIG_BUFFER[2][58] = 0xfe;    BIG_BUFFER[2][59] = 0xc1;
    BIG_BUFFER[2][60] = 0xdf;    BIG_BUFFER[2][61] = 0xd8;    BIG_BUFFER[2][62] = 0xde;    BIG_BUFFER[2][63] = 0xaa;

end:
    /* restart life signal */
    restart_life_signal();

    printf("Done!\n");
}

void icum_test_dbg_act_prepare(void)
{
    /******************/
    /* MASTER_ECU_KEY */
    /******************/

    /* KEYAuthID = 00000000000000000000000000000000 */
    /* KEY_UPDATE_ENC_C = 010153484500800000000000000000B0 */
    /* K1 = KDF (KEYAuthID, KEY_UPDATE_ENC_C) */
    /* 1st : 66E94BD4EF8A2C3B884CFA59CA342B2E */
    /* 2nd : 77B130262656831C6F20B950D00A5DDE = K1 */

    /* KEY_UPDATE_MAC_C = 010253484500800000000000000000B0 */
    /* K2 = KDF (KEYAuthID, KEY_UPDATE_MAC_C) */
    /* 1st : 66E94BD4EF8A2C3B884CFA59CA342B2E */
    /* 2nd : 7909B651651A59C5894AEAD12A2F4D1B = K2 */

    r_icumif_isd_t *p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    uint32_t *p_key_value = (uint32_t*)((uint8_t*)p_ISD + sizeof(r_icumif_isd_t) + 4);
    uint32_t n = 0, value;

    /* MASTER_ECU_KEY */
    *p_key_value++ = 0/*0x16157e2b*/;
    *p_key_value++ = 0/*0xa6d2ae28*/;
    *p_key_value++ = 0/*0x8815f7ab*/;
    *p_key_value++ = 0/*0x3c4fcf09*/;

    /* BOOT_MAC_KEY, KEY_n(1-10) */
    for (n = 0; n <= 10; ++n)
    {
        value = (n << 24) | (n << 16) | (n << 8) | n;
        *p_key_value++ = value;
        *p_key_value++ = value;
        *p_key_value++ = value;
        *p_key_value++ = value;
    }
    /* KEY_n(11-20) */
    for (n = 0x11; n <= 0x1a; ++n)
    {
        value = (n << 24) | (n << 16) | (n << 8) | n;
        *p_key_value++ = value;
        *p_key_value++ = value;
        *p_key_value++ = value;
        *p_key_value++ = value;
    }

    /******************/
    /* MASTER_ECU_KEY */
    /******************/

    /* M1 */
    BIG_BUFFER[0][0x00] = 0x4D; BIG_BUFFER[0][0x01] = 0x50; BIG_BUFFER[0][0x02] = 0x39; BIG_BUFFER[0][0x03] = 0x36;
    BIG_BUFFER[0][0x04] = 0x36; BIG_BUFFER[0][0x05] = 0x30; BIG_BUFFER[0][0x06] = 0x20; BIG_BUFFER[0][0x07] = 0x20;
    BIG_BUFFER[0][0x08] = 0x6A; BIG_BUFFER[0][0x09] = 0x00; BIG_BUFFER[0][0x0a] = 0x5C; BIG_BUFFER[0][0x0b] = 0x00;
    BIG_BUFFER[0][0x0c] = 0x20; BIG_BUFFER[0][0x0d] = 0x20; BIG_BUFFER[0][0x0e] = 0x07; BIG_BUFFER[0][0x0f] = 0x11;

    /* M2 */
    /* KEY_CONFIG_1 */
    BIG_BUFFER[0][0x10] = 0xFF; BIG_BUFFER[0][0x11] = 0x8B; BIG_BUFFER[0][0x12] = 0x75; BIG_BUFFER[0][0x13] = 0xF7;
    BIG_BUFFER[0][0x14] = 0x3E; BIG_BUFFER[0][0x15] = 0x6A; BIG_BUFFER[0][0x16] = 0xD5; BIG_BUFFER[0][0x17] = 0xA1;
    BIG_BUFFER[0][0x18] = 0x72; BIG_BUFFER[0][0x19] = 0x94; BIG_BUFFER[0][0x1a] = 0x23; BIG_BUFFER[0][0x1b] = 0xC6;
    BIG_BUFFER[0][0x1c] = 0xE9; BIG_BUFFER[0][0x1d] = 0x31; BIG_BUFFER[0][0x1e] = 0x1F; BIG_BUFFER[0][0x1f] = 0x1A;
    BIG_BUFFER[0][0x20] = 0x34; BIG_BUFFER[0][0x21] = 0xD4; BIG_BUFFER[0][0x22] = 0x00; BIG_BUFFER[0][0x23] = 0x35;
    BIG_BUFFER[0][0x24] = 0x8F; BIG_BUFFER[0][0x25] = 0xD0; BIG_BUFFER[0][0x26] = 0x78; BIG_BUFFER[0][0x27] = 0x17;
    BIG_BUFFER[0][0x28] = 0x67; BIG_BUFFER[0][0x29] = 0x18; BIG_BUFFER[0][0x2a] = 0x0D; BIG_BUFFER[0][0x2b] = 0x9C;
    BIG_BUFFER[0][0x2c] = 0xE0; BIG_BUFFER[0][0x2d] = 0x7C; BIG_BUFFER[0][0x2e] = 0x86; BIG_BUFFER[0][0x2f] = 0xE9;
    /* KEY_CONFIG_2 */
    BIG_BUFFER[0][0x30] = 0xB0; BIG_BUFFER[0][0x31] = 0x84; BIG_BUFFER[0][0x32] = 0x16; BIG_BUFFER[0][0x33] = 0x65;
    BIG_BUFFER[0][0x34] = 0xDD; BIG_BUFFER[0][0x35] = 0x30; BIG_BUFFER[0][0x36] = 0xC8; BIG_BUFFER[0][0x37] = 0xE7;
    BIG_BUFFER[0][0x38] = 0x5C; BIG_BUFFER[0][0x39] = 0x67; BIG_BUFFER[0][0x3a] = 0xC0; BIG_BUFFER[0][0x3b] = 0x1A;
    BIG_BUFFER[0][0x3c] = 0xE1; BIG_BUFFER[0][0x3d] = 0x2E; BIG_BUFFER[0][0x3e] = 0x8A; BIG_BUFFER[0][0x3f] = 0xAC;
    BIG_BUFFER[0][0x40] = 0x21; BIG_BUFFER[0][0x41] = 0xA0; BIG_BUFFER[0][0x42] = 0xE0; BIG_BUFFER[0][0x43] = 0xC8;
    BIG_BUFFER[0][0x44] = 0xCD; BIG_BUFFER[0][0x45] = 0x8B; BIG_BUFFER[0][0x46] = 0x97; BIG_BUFFER[0][0x47] = 0xFD;
    BIG_BUFFER[0][0x48] = 0x1E; BIG_BUFFER[0][0x49] = 0x71; BIG_BUFFER[0][0x4a] = 0x9C; BIG_BUFFER[0][0x4b] = 0x36;
    BIG_BUFFER[0][0x4c] = 0x30; BIG_BUFFER[0][0x4d] = 0x43; BIG_BUFFER[0][0x4e] = 0x99; BIG_BUFFER[0][0x4f] = 0x7C;
    /* KEY_CONFIG_3 */
    BIG_BUFFER[0][0x50] = 0xB3; BIG_BUFFER[0][0x51] = 0x0B; BIG_BUFFER[0][0x52] = 0x93; BIG_BUFFER[0][0x53] = 0x01;
    BIG_BUFFER[0][0x54] = 0x02; BIG_BUFFER[0][0x55] = 0x0A; BIG_BUFFER[0][0x56] = 0x4A; BIG_BUFFER[0][0x57] = 0x91;
    BIG_BUFFER[0][0x58] = 0xC6; BIG_BUFFER[0][0x59] = 0x40; BIG_BUFFER[0][0x5a] = 0xCB; BIG_BUFFER[0][0x5b] = 0x37;
    BIG_BUFFER[0][0x5c] = 0x7C; BIG_BUFFER[0][0x5d] = 0xC5; BIG_BUFFER[0][0x5e] = 0xD1; BIG_BUFFER[0][0x5f] = 0x1E;
    BIG_BUFFER[0][0x60] = 0x06; BIG_BUFFER[0][0x61] = 0xAB; BIG_BUFFER[0][0x62] = 0xE3; BIG_BUFFER[0][0x63] = 0x0F;
    BIG_BUFFER[0][0x64] = 0xE7; BIG_BUFFER[0][0x65] = 0x1D; BIG_BUFFER[0][0x66] = 0x26; BIG_BUFFER[0][0x67] = 0x9D;
    BIG_BUFFER[0][0x68] = 0x8C; BIG_BUFFER[0][0x69] = 0x75; BIG_BUFFER[0][0x6a] = 0x11; BIG_BUFFER[0][0x6b] = 0xFB;
    BIG_BUFFER[0][0x6c] = 0x8C; BIG_BUFFER[0][0x6d] = 0xFC; BIG_BUFFER[0][0x6e] = 0xBF; BIG_BUFFER[0][0x6f] = 0xAF;
    /* KEY_CONFIG_4 */
    BIG_BUFFER[0][0x70] = 0xC6; BIG_BUFFER[0][0x71] = 0x3B; BIG_BUFFER[0][0x72] = 0x10; BIG_BUFFER[0][0x73] = 0x75;
    BIG_BUFFER[0][0x74] = 0x9E; BIG_BUFFER[0][0x75] = 0x02; BIG_BUFFER[0][0x76] = 0x98; BIG_BUFFER[0][0x77] = 0x74;
    BIG_BUFFER[0][0x78] = 0x39; BIG_BUFFER[0][0x79] = 0x43; BIG_BUFFER[0][0x7a] = 0x6D; BIG_BUFFER[0][0x7b] = 0x60;
    BIG_BUFFER[0][0x7c] = 0x5B; BIG_BUFFER[0][0x7d] = 0xCE; BIG_BUFFER[0][0x7e] = 0x25; BIG_BUFFER[0][0x7f] = 0xB7;
    BIG_BUFFER[0][0x80] = 0x56; BIG_BUFFER[0][0x81] = 0x55; BIG_BUFFER[0][0x82] = 0x21; BIG_BUFFER[0][0x83] = 0x30;
    BIG_BUFFER[0][0x84] = 0x4A; BIG_BUFFER[0][0x85] = 0x6D; BIG_BUFFER[0][0x86] = 0xA1; BIG_BUFFER[0][0x87] = 0xF3;
    BIG_BUFFER[0][0x88] = 0x83; BIG_BUFFER[0][0x89] = 0x55; BIG_BUFFER[0][0x8a] = 0xF8; BIG_BUFFER[0][0x8b] = 0x10;
    BIG_BUFFER[0][0x8c] = 0xB6; BIG_BUFFER[0][0x8d] = 0x20; BIG_BUFFER[0][0x8e] = 0x43; BIG_BUFFER[0][0x8f] = 0x19;

    /* M3 */
    /* KEY_CONFIG_1 */
    BIG_BUFFER[0][0x90] = 0x9A; BIG_BUFFER[0][0x91] = 0xCD; BIG_BUFFER[0][0x92] = 0xE4; BIG_BUFFER[0][0x93] = 0x8D;
    BIG_BUFFER[0][0x94] = 0x4A; BIG_BUFFER[0][0x95] = 0x29; BIG_BUFFER[0][0x96] = 0xB0; BIG_BUFFER[0][0x97] = 0x4B;
    BIG_BUFFER[0][0x98] = 0x3A; BIG_BUFFER[0][0x99] = 0x92; BIG_BUFFER[0][0x9a] = 0x3C; BIG_BUFFER[0][0x9b] = 0xAC;
    BIG_BUFFER[0][0x9c] = 0xC5; BIG_BUFFER[0][0x9d] = 0x7E; BIG_BUFFER[0][0x9e] = 0x34; BIG_BUFFER[0][0x9f] = 0xFC;
    /* KEY_CONFIG_2 */
    BIG_BUFFER[0][0xa0] = 0x7B; BIG_BUFFER[0][0xa1] = 0xC1; BIG_BUFFER[0][0xa2] = 0x56; BIG_BUFFER[0][0xa3] = 0x04;
    BIG_BUFFER[0][0xa4] = 0xC5; BIG_BUFFER[0][0xa5] = 0xAF; BIG_BUFFER[0][0xa6] = 0xF7; BIG_BUFFER[0][0xa7] = 0xA3;
    BIG_BUFFER[0][0xa8] = 0x37; BIG_BUFFER[0][0xa9] = 0x59; BIG_BUFFER[0][0xaa] = 0xEA; BIG_BUFFER[0][0xab] = 0xE3;
    BIG_BUFFER[0][0xac] = 0xF1; BIG_BUFFER[0][0xad] = 0x2E; BIG_BUFFER[0][0xae] = 0x0B; BIG_BUFFER[0][0xaf] = 0xB8;
    /* KEY_CONFIG_3 */
    BIG_BUFFER[0][0xb0] = 0xED; BIG_BUFFER[0][0xb1] = 0x33; BIG_BUFFER[0][0xb2] = 0x8E; BIG_BUFFER[0][0xb3] = 0xD1;
    BIG_BUFFER[0][0xb4] = 0x6D; BIG_BUFFER[0][0xb5] = 0xEC; BIG_BUFFER[0][0xb6] = 0x0E; BIG_BUFFER[0][0xb7] = 0x7E;
    BIG_BUFFER[0][0xb8] = 0x55; BIG_BUFFER[0][0xb9] = 0x84; BIG_BUFFER[0][0xba] = 0x28; BIG_BUFFER[0][0xbb] = 0xD2;
    BIG_BUFFER[0][0xbc] = 0x54; BIG_BUFFER[0][0xbd] = 0xF4; BIG_BUFFER[0][0xbe] = 0x66; BIG_BUFFER[0][0xbf] = 0x70;
    /* KEY_CONFIG_4 */
    BIG_BUFFER[0][0xc0] = 0xB0; BIG_BUFFER[0][0xc1] = 0xB1; BIG_BUFFER[0][0xc2] = 0x1C; BIG_BUFFER[0][0xc3] = 0xBA;
    BIG_BUFFER[0][0xc4] = 0xFA; BIG_BUFFER[0][0xc5] = 0xDC; BIG_BUFFER[0][0xc6] = 0x5C; BIG_BUFFER[0][0xc7] = 0x5F;
    BIG_BUFFER[0][0xc8] = 0xE4; BIG_BUFFER[0][0xc9] = 0x76; BIG_BUFFER[0][0xca] = 0x13; BIG_BUFFER[0][0xcb] = 0xFB;
    BIG_BUFFER[0][0xcc] = 0x64; BIG_BUFFER[0][0xcd] = 0x99; BIG_BUFFER[0][0xce] = 0x94; BIG_BUFFER[0][0xcf] = 0x09;

    printf("\r\nKEY_CONFIG(1-4)?");
    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &n);
    printf("\r\nKEY_CONFIG_%d\r\n\r\n", n);

    /*********************/
    /* BOOT_MAC_KEY,KEYn */
    /*********************/

    /* initialize the service structure */
    p_ISD->service_id                             = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD->p_callbackfunc                         = 0;
    p_ISD->job_id                                 = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_group         = KEY_GRP_AES;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes = AES_BLOCK_SIZE_IN_BYTES;
    for (n = 1; n < 22; ++n)
    {
        p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value       = (const uint8_t *)(p_ISD) + sizeof(r_icumif_isd_t) + 4 + 16 * n;
        p_ISD->prm.KEY_UPDATE_PLAIN.key_id            = n;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.write_protected    = 0;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.no_wild_card       = 0;
        if (n == 1)
        {
            printf("BOOT_MAC_KEY\r\n");
        }
        else
        {
            printf("KEY_%d\r\n", n - 1);
        }
        trigger_service_and_wait(p_ISD);
    }

    return;
}
