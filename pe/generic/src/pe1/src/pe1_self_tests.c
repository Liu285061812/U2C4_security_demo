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
#include "pe1.int.h"
#include "shared.h"

#include "vrun.api.h"
#include "pe_target_device.h"
#include "pe_reg.h"
#include "icum_d_comm_pe.h"
#include "icum_d_comm_pe_pub.h"

#include "renesas_types.h"


// ********************************************************************************
// Local constants
// ********************************************************************************

#pragma alignvar(8)

static const uint8_t TESTVECTOR0[16] =
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

// self-test: CMAC based on test vectors from the CMAC standard
// source: https://tools.ietf.org/html/rfc4493#page-11

const uint8_t KEY[16] =
    {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

static const uint8_t EXPECTED_CMAC_0[16] = 
    {0xbb, 0x1d, 0x69, 0x29, 0xe9, 0x59, 0x37, 0x28, 0x7f, 0xa3, 0x7d, 0x12, 0x9b, 0x75, 0x67, 0x46};

//64bit
static const uint8_t MESSAGE_64[8] =
    {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96};

static const uint8_t EXPECTED_CMAC_64[16] =
    {0xDC, 0x87, 0xCD, 0xCF, 0x77, 0xA2, 0xF1, 0x82, 0x9E, 0x01, 0x2C, 0x4D, 0x31, 0xAF, 0x2F, 0x8B};

//128bit
static const uint8_t MESSAGE_128[16] =
    {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};

static const uint8_t EXPECTED_CMAC_128[16] =
    {0x07, 0x0a, 0x16, 0xb4, 0x6b, 0x4d, 0x41, 0x44, 0xf7, 0x9b, 0xdd, 0x9d, 0xd0, 0x4a, 0x28, 0x7c};


//320bit
static const uint8_t MESSAGE_320[40] =
    {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
     0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
     0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11};

static const uint8_t EXPECTED_CMAC_320[16] =
    {0xDF, 0xA6, 0x67, 0x47, 0xDE, 0x9A, 0xE6, 0x30, 0x30, 0xCA, 0x32, 0x61, 0x14, 0x97, 0xC8, 0x27};


//512bit
static const uint8_t MESSAGE_512[64] =
    {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
     0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
     0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
     0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};

static const uint8_t EXPECTED_CMAC_512[16] =
    {0x51, 0xf0, 0xbe, 0xbf, 0x7e, 0x3b, 0x9d, 0x92, 0xfc, 0x49, 0x74, 0x17, 0x79, 0x36, 0x3c, 0xfe};

typedef enum eFLASH_ACT_TYPE
{
    CF_N_READ_ACT = 0,
    CF_N_PE_ACT,
    CF_M_READ_ACT,
    CF_M_PE_ACT,
    DF_A_READ_ACT,
    DF_A_PE_ACT,
    DF_B_READ_ACT,
    DF_B_PE_ACT,
    SDF_READ_ACT,
    SDF_PE_ACT
} flash_act_type_t;

typedef struct
{
    
    r_icumif_service_id_t service_id;
    // CF_read
    uint32_t             *p_msg;
    uint32_t              msg_size_in_bits;
    r_icumif_service_priority_t priority;
    // CF_PE or DF_PE or DF_Read
    r_flash_op_t          flash_op;
    uint8_t              *p_flash;
    uint32_t              data_size_in_bytes;
    flash_act_type_t      type;
} exc_ctl_fw_act_t;

typedef struct
{
    uint16_t              rpe_action;
    uint16_t              target_flash;
    uint32_t              address;
    flash_act_type_t      type;
} exc_ctl_host_act_t;

// FW Action in Host CF Read mode
const exc_ctl_fw_act_t FW_ACT_HOST_CF_READ[] = {
    SERVICE_01_AES_CMAC,             (uint32_t *)(ADDR_LOWER_CF_BANKA + 0x00100000), 64 * 1024, R_ICUMIF_SERV_PRIORITY_NORMAL, LAST_ITEM_FLASH_OP,                                             0,         0, CF_N_READ_ACT,
    SERVICE_01_AES_CMAC,             (uint32_t *)(ADDR_LOWER_CF_BANKB + 0x00100000), 64 * 1024, R_ICUMIF_SERV_PRIORITY_NORMAL, LAST_ITEM_FLASH_OP,                                             0,         0, CF_M_READ_ACT,
    SERVICE_01_AES_CMAC,                                (uint32_t *)ADDR_LOWER_CRAM, 64 * 1024, R_ICUMIF_SERV_PRIORITY_NORMAL, LAST_ITEM_FLASH_OP,                                             0,         0,  SDF_READ_ACT,
    SERVICE_03_AES_KEY_UPDATE_PLAIN,                                              0,         0, R_ICUMIF_SERV_PRIORITY_NORMAL, LAST_ITEM_FLASH_OP,                                             0,         0,    SDF_PE_ACT
};

// FW Action in Host CF Program/Erace mode
const exc_ctl_fw_act_t FW_ACT_HOST_CF_PE[] = {
    SERVICE_01_AES_CMAC,            (uint32_t *)(ADDR_UPPER_CF_BANKB - 0x000FFFFF), 64 * 1024, R_ICUMIF_SERV_PRIORITY_HIGHEST, LAST_ITEM_FLASH_OP,                                             0,   0, CF_N_READ_ACT,
    SERVICE_01_AES_CMAC,            (uint32_t *)(ADDR_LOWER_CF_BANKC + 0x00100000), 64 * 1024, R_ICUMIF_SERV_PRIORITY_HIGHEST, LAST_ITEM_FLASH_OP,                                             0,   0, CF_M_READ_ACT,
    SERVICE_01_AES_CMAC,            (uint32_t *)(ADDR_LOWER_DF_BANKA + 0x00010000), 64 * 1024, R_ICUMIF_SERV_PRIORITY_HIGHEST, LAST_ITEM_FLASH_OP,                                             0,   0, DF_A_READ_ACT,
    SERVICE_01_AES_CMAC,                               (uint32_t *)ADDR_LOWER_CRAM, 64 * 1024, R_ICUMIF_SERV_PRIORITY_HIGHEST, LAST_ITEM_FLASH_OP,                                             0,   0,  SDF_READ_ACT,
    SERVICE_03_AES_KEY_UPDATE_PLAIN,                                             0,         0,  R_ICUMIF_SERV_PRIORITY_NORMAL, LAST_ITEM_FLASH_OP,                                             0,   0,    SDF_PE_ACT
};

// FW Action in Host DF Read/Program/Erace mode
const exc_ctl_fw_act_t FW_ACT_HOST_DF_RPE[] = {
    SERVICE_01_AES_CMAC,           (uint32_t *)(ADDR_LOWER_CF_BANKB + 0x00100000), 64 * 1024, R_ICUMIF_SERV_PRIORITY_NORMAL, LAST_ITEM_FLASH_OP,                                             0,         0, CF_N_READ_ACT,
    SERVICE_01_AES_CMAC,           (uint32_t *)(ADDR_LOWER_DF_BANKB + 0x00010000), 64 * 1024, R_ICUMIF_SERV_PRIORITY_NORMAL, LAST_ITEM_FLASH_OP,                                             0,         0, DF_B_READ_ACT,
    SERVICE_01_AES_CMAC,                              (uint32_t *)ADDR_LOWER_CRAM, 64 * 1024, R_ICUMIF_SERV_PRIORITY_NORMAL, LAST_ITEM_FLASH_OP,                                             0,         0,  SDF_READ_ACT,
    SERVICE_03_AES_KEY_UPDATE_PLAIN,                                            0,         0, R_ICUMIF_SERV_PRIORITY_NORMAL, LAST_ITEM_FLASH_OP,                                             0,         0,    SDF_PE_ACT
};

// Host Action in FW CF Read mode
const exc_ctl_host_act_t HOST_ACT_FW_CF_READ_MODE[] = {
    0x00, 0xAA01, ADDR_LOWER_CF_BANKB + 0x00100000, CF_N_READ_ACT,
    0xE8, 0xAA01,              ADDR_LOWER_CF_BANKC,   CF_N_PE_ACT,
    0x00, 0xAA01, ADDR_LOWER_CF_BANKC + 0x00100000, CF_M_READ_ACT,
    0xE8, 0xAA01,              ADDR_LOWER_CF_BANKB,   CF_M_PE_ACT,
    0x00, 0xAA80, ADDR_LOWER_DF_BANKA + 0x00010000, DF_A_READ_ACT,
    0xE8, 0xAA80,              ADDR_LOWER_DF_BANKA,   DF_A_PE_ACT
};

// Host Action in FW CF Program/Erace mode
const exc_ctl_host_act_t HOST_ACT_FW_CF_PE_MODE[] = {
    0x00, 0xAA01, ADDR_LOWER_CF_BANKB + 0x00100000, CF_N_READ_ACT,
    0xE8, 0xAA01,              ADDR_LOWER_CF_BANKB,   CF_N_PE_ACT,
    0x00, 0xAA01, ADDR_LOWER_CF_BANKC + 0x00100000, CF_M_READ_ACT,
    0xE8, 0xAA01,              ADDR_LOWER_CF_BANKC,   CF_M_PE_ACT,
    0x00, 0xAA80, ADDR_LOWER_DF_BANKA + 0x00010000, DF_A_READ_ACT,
    0xE8, 0xAA80,              ADDR_LOWER_DF_BANKA,   DF_A_PE_ACT
};


// Host Action in FW DF Read/Program/Erace mode
const exc_ctl_host_act_t HOST_ACT_FW_DF_RPE_MODE[] = {
    0x00, 0xAA01, ADDR_LOWER_CF_BANKB + 0x00100000, CF_N_READ_ACT,
    0xE8, 0xAA01,              ADDR_LOWER_CF_BANKB,   CF_N_PE_ACT,
    0x00, 0xAA80, ADDR_LOWER_DF_BANKA + 0x00010000, DF_A_READ_ACT,
    0xE8, 0xAA80,              ADDR_LOWER_DF_BANKA,   DF_A_PE_ACT,
    0x00, 0xAA80, ADDR_LOWER_DF_BANKB + 0x00010000, DF_B_READ_ACT,
    0xE8, 0xAA80,              ADDR_LOWER_DF_BANKA,   DF_B_PE_ACT
};

// Host Action in FW SecureDF Read/Program/Erace mode
const exc_ctl_host_act_t HOST_ACT_FW_SECURE_DF_RPE_MODE[] = {
    0x00, 0xAA01, ADDR_LOWER_CF_BANKB + 0x00100000, CF_N_READ_ACT,
    0xE8, 0xAA01,              ADDR_LOWER_CF_BANKB,   CF_N_PE_ACT,
    0x00, 0xAA80, ADDR_LOWER_DF_BANKA + 0x00010000, DF_A_READ_ACT,
    0xE8, 0xAA80,              ADDR_LOWER_DF_BANKA,   DF_A_PE_ACT,
};

#define REG_PE_PEFSACS0REQ           (*(volatile uint32_t *)0xFF1F00C0UL)
#define REG_PE_PEFSACS0ACK           (*(volatile uint32_t *)0xFF1F00C4UL)
#define REG_PE_PEFSACS1REQ           (*(volatile uint32_t *)0xFF1F00C8UL)
#define REG_PE_PEFSACS1ACK           (*(volatile uint32_t *)0xFF1F00CCUL)

int nop_cnt = 0;

static int isd_call_back_flag = 0;

extern int test_cf_ack_wait;
extern int test_df_ack_wait;
extern int test_usec;

// ********************************************************************************
// Test functions
// ********************************************************************************

/*===========================================================================
  ICU-M self-test function
  [PUBLIC]
  ===========================================================================*/

int icum_self_test(void)
{
    r_icumif_isd_t *p_ISD;
    int err;
    uint32_t *p_cmac;
    uint32_t i = 0;
    int32_t n = 0;
    uint32_t m = 0;
    uint32_t exit_loop = 0;
    uint8_t expected_cmac[16];

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    while(!exit_loop){

        // Get Input Parameter
        printf("\n| Please Select Key Group\n");
        printf("|---------------------------------\n");
        printf("| 1  : KEY_GRP_AES\n");
        printf("| 2  : KEY_GRP_AES_RAM\n");
        printf("| 3  : KEY_GRP_SHE\n");
        printf("| 4  : KEY_GRP_SHE(RAM)\n");
        printf("| 5  : KEY_GRP_AES(Use already registered key)\n");
        printf("| 6  : KEY_GRP_AES_RAM(Use already registered key)\n");
        printf("  ?> ");

        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &m);

        if( (m >= 1) && (m <= 6) )
        {
            exit_loop = 1;
        }
        else
        {
            printf("Parameter Error!!\n\n");
        }
    }

    switch(m)
    {
        case 1:
            // KEY_GRP_AES
            p_ISD->prm.KEY_UPDATE_PLAIN.key_group   = KEY_GRP_AES;
            p_ISD->prm.KEY_UPDATE_PLAIN.key_id      = 2;
          break;

        case 2:
            // KEY_GRP_AES_RAM
            p_ISD->prm.KEY_UPDATE_PLAIN.key_group   = KEY_GRP_AES_RAM;
            p_ISD->prm.KEY_UPDATE_PLAIN.key_id      = 0;
          break;

        case 3:
            // KEY_GRP_AES
            p_ISD->prm.KEY_UPDATE_PLAIN.key_group   = KEY_GRP_AES;
            p_ISD->prm.KEY_UPDATE_PLAIN.key_id      = 2;
            break;

        case 4:
            // KEY_GRP_SHE
            p_ISD->prm.KEY_UPDATE_PLAIN.key_group   = KEY_GRP_SHE;
            p_ISD->prm.KEY_UPDATE_PLAIN.key_id      = 14;
            break;

        default:
            break;
    }

    if( (m >= 1) && (m <= 4) )
    {
        /********************************************/
        /* testing Key Registration                 */
        /********************************************/
        p_ISD->service_id                  = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD->p_callbackfunc              = 0;
        p_ISD->job_id                      = 0;

        p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;

        printf("\n=================================\n");
        printf("Key registration Start...\n"); FLUSH();
        trigger_service_and_wait(p_ISD);
        printf("Key registration End!\n"); FLUSH();
        if(p_ISD->service_result != SERV_OK) goto end;
        NL(); FLUSH();
    }

    exit_loop = 0;

    while(!exit_loop){

        // Get Input Parameter
        printf("\n\n| Please Select Message Size\n");
        printf("|---------------------------------\n");
        printf("| 0  :   0 bit\n");
        printf("| 1  :  64 bit\n");
        printf("| 2  : 128 bit\n");
        printf("| 3  : 320 bit\n");
        printf("| 4  : 512 bit\n");
        printf("  ?> ");

        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &n);

        if( (n >= 0) && (n <= 4))
        {
            exit_loop = 1;
        }
        else
        {
            printf("Parameter Error!!\n\n");
        }
    }

    switch(n)
    {
        case 0:
            // 0 bit
            p_ISD->prm.AES_CMAC.p_msg               = NULL;
            p_ISD->prm.AES_CMAC.msg_size_in_bits    = 0;
            memcpy(&expected_cmac[0], &EXPECTED_CMAC_0[0], 16);
            break;

        case 1:
            // 64 bit
            p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)MESSAGE_64;
            p_ISD->prm.AES_CMAC.msg_size_in_bits    = 8 * 8;
            memcpy(&expected_cmac[0], &EXPECTED_CMAC_64[0], 16);
            break;

        case 2:
            // 128 bit
            p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)MESSAGE_128;
            p_ISD->prm.AES_CMAC.msg_size_in_bits    = 16 * 8;
            memcpy(&expected_cmac[0], &EXPECTED_CMAC_128[0], 16);
            break;

        case 3:
            // 320 bit
            p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)MESSAGE_320;
            p_ISD->prm.AES_CMAC.msg_size_in_bits    = 40 * 8;
            memcpy(&expected_cmac[0], &EXPECTED_CMAC_320[0], 16);
            break;

        case 4:
            // 512 bit
            p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)MESSAGE_512;
            p_ISD->prm.AES_CMAC.msg_size_in_bits    = 64 * 8;
            memcpy(&expected_cmac[0], &EXPECTED_CMAC_512[0], 16);
            break;

        default:
            break;
    }

    switch(m)
    {
        case 1:
        case 5:
            // KEY_GRP_AES
            p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD->prm.AES_CMAC.key_id              = 2;
            break;

        case 2:
        case 6:
            // KEY_GRP_AES_RAM
            p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_AES_RAM;
            p_ISD->prm.AES_CMAC.key_id              = 0;
            break;

        case 3:
            // KEY_GRP_SHE
            p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_SHE;
            p_ISD->prm.AES_CMAC.key_id              = 4;
            break;

        case 4:
            // KEY_GRP_SHE
            p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_SHE;
            p_ISD->prm.AES_CMAC.key_id              = 14;
            break;

        default:
            break;
    }

    printf("\n=================================\n");
    printf("Testing CMAC GENERATION...\n"); FLUSH();

    // clear error counter
    err = 0;

    /********************************************/
    /* testing CMAC generation                 */
    /********************************************/
    p_ISD->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD->p_callbackfunc                   = 0;
    p_ISD->job_id                           = 0;
    p_ISD->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
    p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t*)(((((uint32_t)ISD_BUFFER + (((uint32_t)sizeof(r_icumif_isd_t) / 4UL) + 1UL) * 4UL) + 7UL) / 8UL) * 8UL);   // 8-byte alignment
    p_ISD->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
    p_ISD->prm.AES_CMAC.job_slice           = job_slice[JOB_AES];
    p_ISD->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;

    printf("Messge Size   : %d \n", p_ISD->prm.AES_CMAC.msg_size_in_bits);
    printf("CMAC Size     : %d \n", p_ISD->prm.AES_CMAC.cmac_size_in_bits);
    printf("EXPECTED_CMAC : ");
    for(i= 0; i<16; i++ )
    {
        printf("%02x", expected_cmac[i]);
    }
    printf("\n");

    // start timing measurement
    vrun_chrono_start();

    trigger_service_and_wait(p_ISD);

    // stop timing measurement
    vrun_chrono_stop();

    if(p_ISD->service_result != SERV_OK) goto end;

    // fetch execution time in vrun_chrono_xxx variables
    vrun_iobuff_print_chrono();

    NL(); FLUSH();


    p_cmac = p_ISD->prm.AES_CMAC.p_cmac;
    //OUTPUT Data
    printf("OUTPUT CMAC   : ");
    for(i= 0; i<16; i++ )
    {
        printf("%02x", *p_cmac);
        p_cmac =  p_cmac + 1;
    }
    printf("\n\n");

    if(memcmp(&expected_cmac[0], p_ISD->prm.AES_CMAC.p_cmac, 16) != 0)
    {
        printf("GENERATION_FAIL!!\n\n");
        err++;
    }
    else
    {
        printf("GENERATION_SUCCESS!!\n\n");
    }

    /********************************************/
    /* testing CMAC verifcation                 */
    /********************************************/
    printf("\n=================================\n");
    printf("\nTesting CMAC VERIFICATION...\n"); FLUSH();
    p_ISD->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD->p_callbackfunc                   = 0;
    p_ISD->job_id                           = 0;
    p_ISD->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
    p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
    memcpy(p_ISD->prm.AES_CMAC.p_cmac, &expected_cmac[0], 16);
    p_ISD->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
    p_ISD->prm.AES_CMAC.job_slice           = job_slice[JOB_AES];
    p_ISD->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;

    printf("Messge Size : %d \n", p_ISD->prm.AES_CMAC.msg_size_in_bits);
    printf("CMAC Size   : %d \n", p_ISD->prm.AES_CMAC.cmac_size_in_bits);

    // start timing measurement
    vrun_chrono_start();

    trigger_service_and_wait(p_ISD);

    // stop timing measurement
    vrun_chrono_stop();

    // fetch execution time in vrun_chrono_xxx variables
    vrun_iobuff_print_chrono();

    NL(); FLUSH();


    if(p_ISD->prm.AES_CMAC.verification_result == VERIFICATION_PASS)
    {
        printf("VERIFICATION_PASS!!\n\n");
    }
    else
    {
        printf("VERIFICATION_FAIL!!\n\n");
    }


    if(p_ISD->service_result != SERV_OK) goto end;
    if(p_ISD->prm.AES_CMAC.verification_result != VERIFICATION_PASS) err++;

end:
    // return the error counter
    return(err);
}

/*===========================================================================
  ICU-M self-test function
  [PUBLIC]
  ===========================================================================*/

int icum_self_test_fast(void)
{
    r_icumif_isd_t *p_ISD;
    int err;
    uint32_t *p_cmac;
    uint32_t i = 0;
    int32_t n = 0;
    uint32_t m = 0;
    uint32_t exit_loop = 0;
    uint8_t expected_cmac[16];
    int32_t ret;

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    memset(p_ISD, 0, sizeof(r_icumif_isd_t));

    while(!exit_loop){

        // Get Input Parameter
        printf("\n| Please Select Key Group\n");
        printf("|---------------------------------\n");
        printf("| 1  : KEY_GRP_AES\n");
        printf("| 2  : KEY_GRP_AES_RAM\n");
        printf("| 3  : KEY_GRP_SHE\n");
        printf("| 4  : KEY_GRP_SHE(RAM)\n");
        printf("| 5  : KEY_GRP_AES(Use already registered key)\n");
        printf("| 6  : KEY_GRP_AES_RAM(Use already registered key)\n");
        printf("  ?> ");

        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &m);

        if( (m >= 1) && (m <= 6) )
        {
            exit_loop = 1;
        }
        else
        {
            printf("Parameter Error!!\n\n");
        }
    }

    switch(m)
    {
        case 1:
            // KEY_GRP_AES
            p_ISD->prm.KEY_UPDATE_PLAIN.key_group   = KEY_GRP_AES;
            p_ISD->prm.KEY_UPDATE_PLAIN.key_id      = 2;
          break;

        case 2:
            // KEY_GRP_AES_RAM
            p_ISD->prm.KEY_UPDATE_PLAIN.key_group   = KEY_GRP_AES_RAM;
            p_ISD->prm.KEY_UPDATE_PLAIN.key_id      = 0;
          break;

        case 3:
            // KEY_GRP_AES
            p_ISD->prm.KEY_UPDATE_PLAIN.key_group   = KEY_GRP_AES;
            p_ISD->prm.KEY_UPDATE_PLAIN.key_id      = 2;
            break;

        case 4:
            // KEY_GRP_SHE
            p_ISD->prm.KEY_UPDATE_PLAIN.key_group   = KEY_GRP_SHE;
            p_ISD->prm.KEY_UPDATE_PLAIN.key_id      = 14;
            break;

        default:
            break;
    }

    if( (m >= 1) && (m <= 4) )
    {
        /********************************************/
        /* testing Key Registration                 */
        /********************************************/
        p_ISD->service_id                  = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD->p_callbackfunc              = 0;
        p_ISD->job_id                      = 0;

        p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;

        printf("\n=================================\n");
        printf("Key registration Start...\n"); FLUSH();
        trigger_service_and_wait(p_ISD);
        printf("Key registration End!\n"); FLUSH();
        if(p_ISD->service_result != SERV_OK) goto end;
        NL(); FLUSH();
    }

    exit_loop = 0;

    while(!exit_loop){

        // Get Input Parameter
        printf("\n\n| Please Select Message Size\n");
        printf("|---------------------------------\n");
        printf("| 0  :   0 bit\n");
        printf("| 1  :  64 bit\n");
        printf("| 2  : 128 bit\n");
        printf("| 3  : 320 bit\n");
        printf("| 4  : 512 bit\n");
        printf("  ?> ");

        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &n);

        if( (n >= 0) && (n <= 4))
        {
            exit_loop = 1;
        }
        else
        {
            printf("Parameter Error!!\n\n");
        }
    }

    switch(n)
    {
        case 0:
            // 0 bit
            p_ISD->prm.AES_CMAC.p_msg               = NULL;
            p_ISD->prm.AES_CMAC.msg_size_in_bits    = 0;
            memcpy(&expected_cmac[0], &EXPECTED_CMAC_0[0], 16);
            break;

        case 1:
            // 64 bit
            p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)MESSAGE_64;
            p_ISD->prm.AES_CMAC.msg_size_in_bits    = 8 * 8;
            memcpy(&expected_cmac[0], &EXPECTED_CMAC_64[0], 16);
            break;

        case 2:
            // 128 bit
            p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)MESSAGE_128;
            p_ISD->prm.AES_CMAC.msg_size_in_bits    = 16 * 8;
            memcpy(&expected_cmac[0], &EXPECTED_CMAC_128[0], 16);
            break;

        case 3:
            // 320 bit
            p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)MESSAGE_320;
            p_ISD->prm.AES_CMAC.msg_size_in_bits    = 40 * 8;
            memcpy(&expected_cmac[0], &EXPECTED_CMAC_320[0], 16);
            break;

        case 4:
            // 512 bit
            p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)MESSAGE_512;
            p_ISD->prm.AES_CMAC.msg_size_in_bits    = 64 * 8;
            memcpy(&expected_cmac[0], &EXPECTED_CMAC_512[0], 16);
            break;

        default:
            break;
    }

    switch(m)
    {
        case 1:
        case 5:
            // KEY_GRP_AES
            p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_AES;
            p_ISD->prm.AES_CMAC.key_id              = 2;
            break;

        case 2:
        case 6:
            // KEY_GRP_AES_RAM
            p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_AES_RAM;
            p_ISD->prm.AES_CMAC.key_id              = 0;
            break;

        case 3:
            // KEY_GRP_SHE
            p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_SHE;
            p_ISD->prm.AES_CMAC.key_id              = 4;
            break;

        case 4:
            // KEY_GRP_SHE
            p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_SHE;
            p_ISD->prm.AES_CMAC.key_id              = 14;
            break;

        default:
            break;
    }

    printf("\n=================================\n");
    printf("Testing CMAC GENERATION...\n"); FLUSH();

    // clear error counter
    err = 0;

    /********************************************/
    /* testing CMAC generation                 */
    /********************************************/
    p_ISD->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD->p_callbackfunc                   = 0;
    p_ISD->job_id                           = 0;
    p_ISD->service_priority                 = R_ICUMIF_SERV_PRIORITY_HIGHEST;
    p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t *)(ISD_BUFFER + ((sizeof(r_icumif_isd_t) / 4) + 1) * 4);
    p_ISD->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
    p_ISD->prm.AES_CMAC.job_slice           = 0;
    p_ISD->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;

    ret = R_ICUMIF_RTN_SERV_RUNNING;
    p_ISD->res_nointerrupt = R_ICUMIF_REQRES_NOINTERRPUT;

    printf("Messge Size   : %d \n", p_ISD->prm.AES_CMAC.msg_size_in_bits);
    printf("CMAC Size     : %d \n", p_ISD->prm.AES_CMAC.cmac_size_in_bits);
    printf("EXPECTED_CMAC : ");
    for(i= 0; i<16; i++ )
    {
        printf("%02x", expected_cmac[i]);
    }
    printf("\n");

    // start timing measurement
    vrun_chrono_start();

    (void)R_ICUMIF_ServiceRequest(p_ISD);
    /* trigger_service_and_wait(p_ISD); */

    while (ret == R_ICUMIF_RTN_SERV_RUNNING)
    {
        ret = R_ICUMIF_IsServiceCompleted(p_ISD);
    }

    // stop timing measurement
    vrun_chrono_stop();

    (void)R_ICUMIF_ServiceResponse();

    if(p_ISD->service_result != SERV_OK) goto end;

    // fetch execution time in vrun_chrono_xxx variables
    vrun_iobuff_print_chrono();

    NL(); FLUSH();

    p_cmac = p_ISD->prm.AES_CMAC.p_cmac;
    //OUTPUT Data
    printf("OUTPUT CMAC   : ");
    for(i= 0; i<16; i++ )
    {
        printf("%02x", *p_cmac);
        p_cmac =  p_cmac + 1;
    }
    printf("\n\n");

    if(memcmp(&expected_cmac[0], p_ISD->prm.AES_CMAC.p_cmac, 16) != 0)
    {
        printf("GENERATION_FAIL!!\n\n");
        err++;
    }
    else
    {
        printf("GENERATION_SUCCESS!!\n\n");
    }

    /********************************************/
    /* testing CMAC verifcation                 */
    /********************************************/
    printf("\n=================================\n");
    printf("\nTesting CMAC VERIFICATION...\n"); FLUSH();
    p_ISD->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD->p_callbackfunc                   = 0;
    p_ISD->job_id                           = 0;
    p_ISD->service_priority                 = R_ICUMIF_SERV_PRIORITY_HIGHEST;
    p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
    memcpy(p_ISD->prm.AES_CMAC.p_cmac, &expected_cmac[0], 16);
    p_ISD->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
    p_ISD->prm.AES_CMAC.job_slice           = 0;
    p_ISD->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;

    printf("Messge Size : %d \n", p_ISD->prm.AES_CMAC.msg_size_in_bits);
    printf("CMAC Size   : %d \n", p_ISD->prm.AES_CMAC.cmac_size_in_bits);

    ret = R_ICUMIF_RTN_SERV_RUNNING;
    p_ISD->res_nointerrupt = R_ICUMIF_REQRES_NOINTERRPUT;

    // start timing measurement
    vrun_chrono_start();

    (void)R_ICUMIF_ServiceRequest(p_ISD);
    /* trigger_service_and_wait(p_ISD); */

    while (ret == R_ICUMIF_RTN_SERV_RUNNING)
    {
        ret = R_ICUMIF_IsServiceCompleted(p_ISD);
    }

    // stop timing measurement
    vrun_chrono_stop();

    (void)R_ICUMIF_ServiceResponse();

    // fetch execution time in vrun_chrono_xxx variables
    vrun_iobuff_print_chrono();

    NL(); FLUSH();


    if(p_ISD->prm.AES_CMAC.verification_result == VERIFICATION_PASS)
    {
        printf("VERIFICATION_PASS!!\n\n");
    }
    else
    {
        printf("VERIFICATION_FAIL!!\n\n");
    }


    if(p_ISD->service_result != SERV_OK) goto end;
    if(p_ISD->prm.AES_CMAC.verification_result != VERIFICATION_PASS) err++;

end:
    // return the error counter
    return(err);
}
/*===========================================================================
  ICU-M test function - multi jobs
  [PUBLIC]
  ===========================================================================*/
void icum_test_multi_jobs(void)
{
    r_icumif_isd_t *p_ISD[6];
    uint8_t *p_cmac[2];
    int ret = 0;
    
    printf("\n");

    // initialize the global icum service header
    p_ISD[0]  = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD[1]  = p_ISD[0] + sizeof(r_icumif_isd_t);
    p_ISD[2]  = p_ISD[1] + sizeof(r_icumif_isd_t);
    p_ISD[3]  = p_ISD[2] + sizeof(r_icumif_isd_t);
    p_ISD[4]  = p_ISD[3] + sizeof(r_icumif_isd_t);
    p_ISD[5]  = p_ISD[4] + sizeof(r_icumif_isd_t);
    p_cmac[0] = (uint8_t *)p_ISD[5] + sizeof(r_icumif_isd_t);
    p_cmac[1] = p_cmac[0] + 16;

    memset(BIG_BUFFER[0], 0,    SIZE_OF_BIG_BUFFER);
    memset(BIG_BUFFER[0], 0xaa, 64);

    memset(BIG_BUFFER[1], 0,    SIZE_OF_BIG_BUFFER);

    memset(BIG_BUFFER[2], 0,    SIZE_OF_BIG_BUFFER);
    memset(BIG_BUFFER[2], 0xaa, 64);

    // pause life signal management
    pause_life_signal();

    printf("Testing multi-job calls...\n\n");

    // initialize the RAM key 0
    p_ISD[0]->service_id                  = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD[0]->p_callbackfunc              = 0;
    p_ISD[0]->job_id                      = 0;

    p_ISD[0]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES_RAM;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.key_id                   = 0;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
    p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;

    printf("Updating RAM key #0...\n"); FLUSH();
    trigger_service_and_wait(p_ISD[0]);
    if(p_ISD[0]->service_result != SERV_OK) goto end;
    NL(); FLUSH();

//ENCRYPTION
    p_ISD[0]->service_id                    = SERVICE_01_AES_CIPHER;
    p_ISD[0]->p_callbackfunc                = 0;
    p_ISD[0]->job_id                        = 0;
    p_ISD[0]->prm.AES_CIPHER.direction      = CIPHER_DIR_ENCRYPTION;
    p_ISD[0]->prm.AES_CIPHER.cipher_mode    = CIPHER_MOD_CBC;
    p_ISD[0]->prm.AES_CIPHER.key_group      = KEY_GRP_AES_RAM;
    p_ISD[0]->prm.AES_CIPHER.key_id         = 0;
    p_ISD[0]->prm.AES_CIPHER.p_iv           = (uint32_t *)TESTVECTOR0;
    p_ISD[0]->prm.AES_CIPHER.p_block_in     = (uint32_t *)BIG_BUFFER[0];
    p_ISD[0]->prm.AES_CIPHER.p_block_out    = (uint32_t *)BIG_BUFFER[1];
    p_ISD[0]->prm.AES_CIPHER.size.nb_blocks = 4;
    p_ISD[0]->prm.AES_CIPHER.job_cycle      = JOB_ALL_AT_ONCE;


//DECRYPTION
    p_ISD[1]->service_id                    = SERVICE_01_AES_CIPHER;
    p_ISD[1]->p_callbackfunc                = 0;
    p_ISD[1]->job_id                        = 0;
    p_ISD[1]->prm.AES_CIPHER.direction      = CIPHER_DIR_DECRYPTION;
    p_ISD[1]->prm.AES_CIPHER.cipher_mode    = CIPHER_MOD_CBC;
    p_ISD[1]->prm.AES_CIPHER.key_group      = KEY_GRP_AES_RAM;
    p_ISD[1]->prm.AES_CIPHER.key_id         = 0;
    p_ISD[1]->prm.AES_CIPHER.p_iv           = (uint32_t *)TESTVECTOR0;
    p_ISD[1]->prm.AES_CIPHER.p_block_in     = (uint32_t *)BIG_BUFFER[1];
    p_ISD[1]->prm.AES_CIPHER.p_block_out    = (uint32_t *)BIG_BUFFER[0];
    p_ISD[1]->prm.AES_CIPHER.size.nb_blocks = 4;
    p_ISD[1]->prm.AES_CIPHER.job_cycle      = JOB_ALL_AT_ONCE;

//ENCRYPTION
    p_ISD[2]->service_id                    = SERVICE_01_AES_CIPHER;
    p_ISD[2]->p_callbackfunc                = 0;
    p_ISD[2]->job_id                        = 0;
    p_ISD[2]->prm.AES_CIPHER.direction      = CIPHER_DIR_ENCRYPTION;
    p_ISD[2]->prm.AES_CIPHER.cipher_mode    = CIPHER_MOD_ECB;
    p_ISD[2]->prm.AES_CIPHER.key_group      = KEY_GRP_AES_RAM;
    p_ISD[2]->prm.AES_CIPHER.key_id         = 0;
    p_ISD[2]->prm.AES_CIPHER.p_iv           = 0;
    p_ISD[2]->prm.AES_CIPHER.p_block_in     = (uint32_t*)BIG_BUFFER[0];
    p_ISD[2]->prm.AES_CIPHER.p_block_out    = (uint32_t*)BIG_BUFFER[1];
    p_ISD[2]->prm.AES_CIPHER.size.nb_blocks = 4;
    p_ISD[2]->prm.AES_CIPHER.job_cycle      = JOB_ALL_AT_ONCE;

//DECRYPTION
    p_ISD[3]->service_id                    = SERVICE_01_AES_CIPHER;
    p_ISD[3]->p_callbackfunc                = 0;
    p_ISD[3]->job_id                        = 0;
    p_ISD[3]->prm.AES_CIPHER.direction      = CIPHER_DIR_DECRYPTION;
    p_ISD[3]->prm.AES_CIPHER.cipher_mode    = CIPHER_MOD_ECB;
    p_ISD[3]->prm.AES_CIPHER.key_group      = KEY_GRP_AES_RAM;
    p_ISD[3]->prm.AES_CIPHER.key_id         = 0;
    p_ISD[3]->prm.AES_CIPHER.p_iv           = 0;
    p_ISD[3]->prm.AES_CIPHER.p_block_in     = (uint32_t*)BIG_BUFFER[1];
    p_ISD[3]->prm.AES_CIPHER.p_block_out    = (uint32_t*)BIG_BUFFER[0];
    p_ISD[3]->prm.AES_CIPHER.size.nb_blocks = 4;
    p_ISD[3]->prm.AES_CIPHER.job_cycle      = JOB_ALL_AT_ONCE;

    // start timing measurement
    vrun_chrono_start();

    // trigger the security services in sequence
    trigger_service_and_wait(p_ISD[0]);
    trigger_service_and_wait(p_ISD[1]);

    ret = memcmp(BIG_BUFFER[0], BIG_BUFFER[2], 64);
    if(ret == 0)
    {
        printf("AES Success\n\n");
    }
    else
    {
        printf("AES Failed\n\n");
    }

    memset(BIG_BUFFER[0], 0,    SIZE_OF_BIG_BUFFER);
    memset(BIG_BUFFER[0], 0xaa, 64);

    memset(BIG_BUFFER[1], 0,    SIZE_OF_BIG_BUFFER);

    memset(BIG_BUFFER[2], 0,    SIZE_OF_BIG_BUFFER);
    memset(BIG_BUFFER[2], 0xaa, 64);

    trigger_service_and_wait(p_ISD[2]);
    trigger_service_and_wait(p_ISD[3]);

    ret = memcmp(BIG_BUFFER[0], BIG_BUFFER[2], 64);
    if(ret == 0)
    {
        printf("AES Success\n\n");
    }
    else
    {
        printf("AES Failed\n\n");
    }

    // stop timing measurement
    vrun_chrono_stop();

end:
    // restart life signal
    restart_life_signal();

    printf("Done!\n");
}

/*===========================================================================
  ICU-M self-test function for LLWEB-10058998-03
  [PUBLIC]
  ===========================================================================*/

static void block_erase_code_flash_test(uint32_t select_addr,uint32_t erase_area)
{
    uint32_t addr;

    addr = select_addr+erase_area*CF_BLOCK_SIZE;
    // issue CF block erase command
    REG_PE_FACIFSADDR  = addr;
    REG_PE_FACICMD = 0x20;
    REG_PE_FACICMD = 0xD0;

    // wait for e/o blank check operation
    while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};

}

static void init_code_flash_user_area_test(uint32_t select_addr,uint32_t write_area)
{
    int i;
    uint32_t addr;
    uint32_t wdata = 0x10000000;

    // issue CF program command
    addr = select_addr+write_area*CF_WRITE_SIZE;
    REG_PE_FACIFSADDR  = addr;
    REG_PE_FACICMD = 0xE8;
    REG_PE_FACICMD = 0x80;
    for( i=0; i<128; i++){
        REG_PE_FACICMDDT = wdata++;
    }
    REG_PE_FACICMD = 0xD0;
    
    // wait for e/o blank check operation
    while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};

}

int cf_cmac_fast(void)
{
    r_icumif_isd_t *p_ISD;
    int err;
    uint32_t *p_cmac;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t exit_loop = 0;
    uint32_t select_addr = 0;
    uint32_t erase_area = 0;
    uint32_t write_area = 0;
    uint32_t erase_size = 0;

    // clear error counter
    err = 0;

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    while(!exit_loop){
    
        // Select Write Area
        printf("\n| Please Select Write Start Area\n");
        printf("|---------------------------------\n");
        printf("| Code Flash Bank B : 0x00300000-0x005FFFFF\n");
        printf("| Code Flash Bank C : 0x00600000-0x008FFFFF\n");
        printf("| Code Flash Bank D : 0x00900000-0x00BFFFFF\n");
        printf("| Code Flash Bank E : 0x00C00000-0x00EFFFFF\n");
        printf("  ?> ");

        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &select_addr);

        if( (select_addr >= 0x00300000) && (select_addr <= 0x00F00000) )
        {
            exit_loop = 1;
        }
        else
        {
            printf("Select Area Error!!\n\n");
        }
    }

    exit_loop = 0;

    while(!exit_loop){

        // Erase Block Size
        printf("\n\n| Please Select Erase Block Size\n");
        printf("|---------------------------------\n");
        printf("|Erase some block?(Code Flash Bank = 48 Block) \n");
        printf("|Range:1-192 \n");
        printf("  ?> ");
    
        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &erase_size);

        if( (erase_size >= 1) && (erase_size <= 192))
        {
            exit_loop = 1;
        }
        else
        {
            printf("Parameter Error!!\n\n");
        }
    }

    // AES_KEY_UPDATE_PLAIN
    p_ISD->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD->p_callbackfunc                                = 0;
    p_ISD->job_id                                        = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_id                   = 2;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
    p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;

    printf("Key registration Start...\n");
    trigger_service_and_wait(p_ISD);
    printf("Key registration End!\n");
    printf("\n=================================\n");
    if(p_ISD->service_result != SERV_OK) goto end;

    //AES_CMAC FAST
    p_ISD->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD->p_callbackfunc                   = 0;
    p_ISD->job_id                           = 0;
    p_ISD->service_priority                 = R_ICUMIF_SERV_PRIORITY_HIGHEST;
    p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)MESSAGE_64;
    p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t *)(ISD_BUFFER + ((sizeof(r_icumif_isd_t) / 4) + 1) * 4);
    p_ISD->prm.AES_CMAC.msg_size_in_bits    = 8 * 8;
    p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_SHE;
    p_ISD->prm.AES_CMAC.key_id              = 4;
    p_ISD->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
    p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD->prm.AES_CMAC.job_slice           = 0;
    p_ISD->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;

    trigger_service_and_wait(p_ISD);
    if(p_ISD->service_result != SERV_OK) goto end;

    // Change to code flash programming/erasure mode
    REG_PE_FACIFENTRYR = 0xAA01;
    while( REG_PE_FACIFENTRYR != 0x0001 ){};
    while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};

    // Programming/erasure/CodeFlashBlankChecking enabled
    REG_PE_FACIFHVE3  = 0x01;
    REG_PE_FACIFHVE15 = 0x01;

    for(i=1;i<=erase_size;i++){
        write_area = erase_area*128;
        block_erase_code_flash_test(select_addr,erase_area);
        init_code_flash_user_area_test(select_addr,write_area);
        trigger_service_and_wait(p_ISD);
        if(p_ISD->service_result != SERV_OK){
            printf("GENERATION_FAIL!! NUMBER : %d\n",i);
            err++;
        }
        else{
            printf("GENERATION_SUCCESS!! NUMBER : %d\n",i);
        }
        // check for errors
        if( (REG_PE_FACIFASTAT&0x10/*CMDLK*/) || (REG_PE_FACIFSTATR&(0x00004000/*ILGLERR*/|0x00002000/*ERSERR*/|0x00001000/*PRGERR*/)) ){
            printf("!!  Error caused in issuing CF program command  !!\n");
            goto end;
        }
        p_cmac = p_ISD->prm.AES_CMAC.p_cmac;
        printf("OUTPUT_CMAC : ");
        for(j= 0; j<16; j++ )
        {
            printf("%02x", *p_cmac);
            p_cmac =  p_cmac + 1;
        }
        erase_area++;
        printf("\n");
    }

end:
    // Programming/erasure/DataFlashBlankChecking disabled
    REG_PE_FACIFHVE3  = 0x00;
    REG_PE_FACIFHVE15 = 0x00;

    // Change to Code flash read mode
    REG_PE_FACIFENTRYR = 0xAA00;
    while( REG_PE_FACIFENTRYR!= 0x0000 ){};
    // return the error counter
    return(err);
}

/*===========================================================================
  R_ICUMIF_Init_Reset
  ===========================================================================*/
void R_ICUMIF_Init_Reset()
{
    ICMIF_INIT_FLG = 0x00000000UL;
}

/*===========================================================================
  R_ICUMIF_Init_Test
  ===========================================================================*/
 
void R_ICUMIF_Init_Test()
{
    uint32_t *test_p_request_queue;
    int32_t errno;

    
        // Select Area
        printf("\n| Please Select Area\n");
        printf("|---------------------------------\n");
        printf("  ?> ");

        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &test_p_request_queue);

    errno = R_ICUMIF_Init(test_p_request_queue);
    printf("ERR No = %d\n",errno);
    /* Parameter alignment error */
    if(errno == R_ICUMIF_ER_PARAM_ALIGNMENT)
    {
        printf("R_ICUMIF_ER_PARAM_ALIGNMENT !! \n");
    }
    /* Address not within global RAM area */
    else if(errno == R_ICUMIF_ER_ADDR_NOT_G_RAM)
    {
        printf("R_ICUMIF_ER_ADDR_NOT_G_RAM !!\n");
    }
    /* Software error */
    else if(errno == R_ICUMIF_ER_SOFTWARE_ERROR)
    {
        printf("R_ICUMIF_ER_SOFTWARE_ERROR !!\n");
    }
}

/*===========================================================================
  R_ICUMIF_ServiceRequest_Test
  ===========================================================================*/
 
void R_ICUMIF_ServiceRequest_Test()
{
    r_icumif_isd_t *p_ISD;
    int32_t errno;
    volatile uint32_t   *p_reg;
    ptr_manip_t         queptr;
    uint32_t            *p_queptr;
    int32_t             next_idx;
    int32_t             use_queue = 0;


    // ICUM_CMDREGICUM_SEMAPHORE
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;

    // QUE_ADDR_LOWER_24BITS,QUE_ADDR_UPPER_8BITS_VALUE
    queptr.val = (*p_reg & QUE_ADDR_LOWER_24BITS) | QUE_ADDR_UPPER_8BITS_VALUE;
    p_queptr = (uint32_t *)queptr.p_ptr;

     // p_ISD header
    printf("\n| p_ISD header \n");
    printf("|---------------------------------\n");
    printf("  ?> ");

    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &p_ISD);

    
    // ALIGNMENT_4BYTE_CHECK_PATTERN
    if (0UL == ((uint32_t)((volatile void *)p_ISD) & 0x00000003UL))
    {
    // AES_KEY_UPDATE_PLAIN
    memset(p_ISD, 0, sizeof(r_icumif_isd_t));
    p_ISD->req_nointerrupt                               = R_ICUMIF_REQRES_INTERRPUT;
    p_ISD->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD->p_callbackfunc                                = 0;
    p_ISD->job_id                                        = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_id                   = 3;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
    p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
    }

    errno = R_ICUMIF_ServiceRequest(p_ISD);
    printf("ERR No = %d\n",errno);
    /* Parameter alignment error */
    if(errno == R_ICUMIF_ER_PARAM_ALIGNMENT)
    {
        printf("R_ICUMIF_ER_PARAM_ALIGNMENT !! \n");
    }
    /* Address not within global RAM area */
    if(errno == R_ICUMIF_ER_ADDR_NOT_G_RAM)
    {
        printf("R_ICUMIF_ER_ADDR_NOT_G_RAM !! \n");
    }
    /* NOT_INIT */
    if(errno == R_ICUMIF_ER_IF_LIB_NOT_INIT)
    {
        printf("R_ICUMIF_ER_IF_LIB_NOT_INIT !! \n");
    }
    /* QUEUE_FULL */
    if(errno == R_ICUMIF_ER_REQ_QUEUE_FULL)
    {
        printf("R_ICUMIF_ER_REQ_QUEUE_FULL !! \n");
    }

    for (next_idx = 0; next_idx < REQUEST_QUEUE_LENGTH; next_idx++)
    {
        if (0U == (*(p_queptr + next_idx)))
        {
            break;
        }
        else
        {
            use_queue++;
        }
    }
    printf("REQUEST QUEUE Use Area : %d\n",use_queue);
}

/*===========================================================================
  R_ICUMIF_ServiceRequest_NoInterrupt_Test
  ===========================================================================*/
 
void R_ICUMIF_ServiceRequest_NoInterrupt_Test()
{
    r_icumif_isd_t *p_ISD[5];
    int32_t nointerrupt;
    volatile uint32_t   *p_reg;
    ptr_manip_t         queptr;
    uint32_t            *p_queptr;
    int32_t             next_idx;
    int32_t             used_queue = 0;
    int32_t             use_queue = 0;
    int32_t             i;


    // ICUM_CMDREGICUM_SEMAPHORE
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;

    // QUE_ADDR_LOWER_24BITS,QUE_ADDR_UPPER_8BITS_VALUE
    queptr.val = (*p_reg & QUE_ADDR_LOWER_24BITS) | QUE_ADDR_UPPER_8BITS_VALUE;
    p_queptr = (uint32_t *)queptr.p_ptr;

    // initialize the global icum service header
    p_ISD[0]  = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD[1]  = p_ISD[0] + sizeof(r_icumif_isd_t);
    p_ISD[2]  = p_ISD[1] + sizeof(r_icumif_isd_t);
    p_ISD[3]  = p_ISD[2] + sizeof(r_icumif_isd_t);
    p_ISD[4]  = p_ISD[3] + sizeof(r_icumif_isd_t);

    // INTERRPUT
    printf("\n| INTERRPUT SET \n");
    printf("|0:REQRES_INTERRPUT \n");
    printf("|1:REQRES_NOINTERRPUT \n");
    printf("|---------------------------------\n");
    printf("  ?> ");

    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &nointerrupt);

    for (next_idx = 0; next_idx < REQUEST_QUEUE_LENGTH; next_idx++)
    {
        if (0U == (*(p_queptr + next_idx)))
        {
            break;
        }
        else
        {
            used_queue++;
        }
    }

    if(used_queue ==0){
    // AES_KEY_UPDATE_PLAIN
        if(nointerrupt == 1){
            p_ISD[0]->req_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
        }
        else
        {
            p_ISD[0]->req_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
        }
        p_ISD[0]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[0]->p_callbackfunc                                = 0;
        p_ISD[0]->job_id                                        = 0;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.key_id                   = 3;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;

        trigger_service_and_wait(p_ISD[0]);
    }else if(used_queue ==1){
        // AES_CMAC
        if(nointerrupt == 1){
            p_ISD[1]->req_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
        }
        else
        {
            p_ISD[1]->req_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
        }
        p_ISD[1]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[1]->p_callbackfunc                                = 0;
        p_ISD[1]->job_id                                        = 0;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.key_id                   = 4;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
        
        trigger_service_and_wait(p_ISD[1]);
   }else if(used_queue ==2){
        // AES_CMAC
        if(nointerrupt == 1){
            p_ISD[2]->req_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
        }
        else
        {
            p_ISD[2]->req_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
        }
        p_ISD[2]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[2]->p_callbackfunc                                = 0;
        p_ISD[2]->job_id                                        = 0;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.key_id                   = 5;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
        
        trigger_service_and_wait(p_ISD[2]);
    }else if(used_queue ==3){
        // AES_CMAC
        if(nointerrupt == 1){
            p_ISD[3]->req_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
        }
        else
        {
            p_ISD[3]->req_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
        }
        p_ISD[3]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[3]->p_callbackfunc                                = 0;
        p_ISD[3]->job_id                                        = 0;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.key_id                   = 6;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
        
        trigger_service_and_wait(p_ISD[3]);
   }else{
        // AES_CMAC
        if(nointerrupt == 1){
            p_ISD[4]->req_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
        }
        else
        {
            p_ISD[4]->req_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
        }
        p_ISD[4]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[4]->p_callbackfunc                                = 0;
        p_ISD[4]->job_id                                        = 0;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.key_id                   = 7;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
        
        trigger_service_and_wait(p_ISD[4]);
    }

    for (next_idx = 0; next_idx < REQUEST_QUEUE_LENGTH; next_idx++)
    {
        if (0U == (*(p_queptr + next_idx)))
        {
            break;
        }
        else
        {
            use_queue++;
        }
    }
    printf("REQUEST QUEUE Use Area : %d\n",use_queue);

    for(i = 0; i < REQUEST_QUEUE_LENGTH; i++){
        if(p_ISD[i]->service_result == SERV_OK && ((i < (used_queue+ 1U))))
        {
        printf("SERV_OK \n");
        }
        else if(p_ISD[i]->service_result == SERV_NEW_REQUEST && ((i < (used_queue + 1U))))
        {
        printf("SERV_NOINTERRUPT \n");
        }
        else
        {
        }
    }
}

/*===========================================================================
  R_ICUMIF_ServiceRequest_NoInterrupt_Test
  ===========================================================================*/
 
void R_ICUMIF_ServiceRequest_NoInterrupt_Test2()
{
    r_icumif_isd_t *p_ISD[20];
    int32_t nointerrupt;
    volatile uint32_t   *p_reg;
    ptr_manip_t         queptr;
    uint32_t            *p_queptr;
    int32_t             next_idx;
    int32_t             use_queue = 0;
    int32_t errno;


    // ICUM_CMDREGICUM_SEMAPHORE
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;

    // QUE_ADDR_LOWER_24BITS,QUE_ADDR_UPPER_8BITS_VALUE
    queptr.val = (*p_reg & QUE_ADDR_LOWER_24BITS) | QUE_ADDR_UPPER_8BITS_VALUE;
    p_queptr = (uint32_t *)queptr.p_ptr;

    // initialize the global icum service header
    p_ISD[0]  = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD[1]  = p_ISD[0] + sizeof(r_icumif_isd_t);
    p_ISD[2]  = p_ISD[1] + sizeof(r_icumif_isd_t);
    p_ISD[3]  = p_ISD[2] + sizeof(r_icumif_isd_t);
    p_ISD[4]  = p_ISD[3] + sizeof(r_icumif_isd_t);
    p_ISD[5]  = p_ISD[4] + sizeof(r_icumif_isd_t);
    p_ISD[6]  = p_ISD[5] + sizeof(r_icumif_isd_t);
    p_ISD[7]  = p_ISD[6] + sizeof(r_icumif_isd_t);
    p_ISD[8]  = p_ISD[7] + sizeof(r_icumif_isd_t);
    p_ISD[9]  = p_ISD[8] + sizeof(r_icumif_isd_t);
    p_ISD[10]  = p_ISD[9] + sizeof(r_icumif_isd_t);
    p_ISD[11]  = p_ISD[10] + sizeof(r_icumif_isd_t);
    p_ISD[12]  = p_ISD[11] + sizeof(r_icumif_isd_t);
    p_ISD[13]  = p_ISD[12] + sizeof(r_icumif_isd_t);
    p_ISD[14]  = p_ISD[13] + sizeof(r_icumif_isd_t);
    p_ISD[15]  = p_ISD[14] + sizeof(r_icumif_isd_t);
    p_ISD[16]  = p_ISD[15] + sizeof(r_icumif_isd_t);
    p_ISD[17]  = p_ISD[16] + sizeof(r_icumif_isd_t);
    p_ISD[18]  = p_ISD[17] + sizeof(r_icumif_isd_t);
    p_ISD[19]  = p_ISD[18] + sizeof(r_icumif_isd_t);

    
    // INTERRPUT
    printf("\n| INTERRPUT SET \n");
    printf("|0:REQRES_INTERRPUT \n");
    printf("|1:REQRES_NOINTERRPUT \n");
    printf("|---------------------------------\n");
    printf("  ?> ");

    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &nointerrupt);

    for (next_idx = 0; next_idx < 20; next_idx++)
    {
    // AES_KEY_UPDATE_PLAIN
        if(nointerrupt == 1){
            p_ISD[next_idx]->req_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
        }
        else
        {
            p_ISD[next_idx]->req_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
        }
        p_ISD[next_idx]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[next_idx]->p_callbackfunc                                = 0;
        p_ISD[next_idx]->job_id                                        = 0;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.key_id                   = next_idx + 3;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;

        errno = R_ICUMIF_ServiceRequest(p_ISD[next_idx]);
        printf("ERR No = %d\n", errno);
    }

    for (next_idx = 0; next_idx < 20; next_idx++)
    {
    // AES_KEY_UPDATE_PLAIN
        if(nointerrupt == 1){
            p_ISD[next_idx]->req_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
        }
        else
        {
            p_ISD[next_idx]->req_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
        }
        p_ISD[next_idx]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[next_idx]->p_callbackfunc                                = 0;
        p_ISD[next_idx]->job_id                                        = 0;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.key_id                   = next_idx + 3;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;

        errno = R_ICUMIF_ServiceRequest(p_ISD[next_idx]);
        printf("ERR No = %d\n", errno);
    }

    for (next_idx = 0; next_idx < 20; next_idx++)
    {
    // AES_KEY_UPDATE_PLAIN
        if(nointerrupt == 1){
            p_ISD[next_idx]->req_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
        }
        else
        {
            p_ISD[next_idx]->req_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
        }
        p_ISD[next_idx]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[next_idx]->p_callbackfunc                                = 0;
        p_ISD[next_idx]->job_id                                        = 0;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.key_id                   = next_idx + 3;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[next_idx]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;

        errno = R_ICUMIF_ServiceRequest(p_ISD[next_idx]);
        printf("ERR No = %d\n", errno);
    }

    for (next_idx = 0; next_idx < REQUEST_QUEUE_LENGTH; next_idx++)
    {
        if (0U == (*(p_queptr + next_idx)))
        {
            break;
        }
        else
        {
            use_queue++;
        }
    }
    printf("REQUEST QUEUE Use Area : %d\n",use_queue);

}

/*===========================================================================
  R_ICUMIF_ServiceCompleted_Error_Test
  ===========================================================================*/
 
void R_ICUMIF_ServiceCompleted_Error_Test()
{
    r_icumif_isd_t *p_ISD;
    int32_t errno;
    volatile uint32_t   *p_reg;
    ptr_manip_t         queptr;
    uint32_t            *p_queptr;
    int32_t             next_idx;
    int32_t             use_queue = 0;

    // ICUM_CMDREGICUM_SEMAPHORE
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;

    // QUE_ADDR_LOWER_24BITS,QUE_ADDR_UPPER_8BITS_VALUE
    queptr.val = (*p_reg & QUE_ADDR_LOWER_24BITS) | QUE_ADDR_UPPER_8BITS_VALUE;
    p_queptr = (uint32_t *)queptr.p_ptr;
    p_queptr += REQUEST_QUEUE_LENGTH;

     // p_ISD header
    printf("\n| p_ISD header \n");
    printf("|---------------------------------\n");
    printf("  ?> ");

    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &p_ISD);

    // ALIGNMENT_4BYTE_CHECK_PATTERN
    if (0UL == ((uint32_t)((volatile void *)p_ISD) & 0x00000003UL))
    {
    p_ISD->res_nointerrupt = R_ICUMIF_REQRES_INTERRPUT;
    p_ISD->req_res_status = R_ICUMIF_REQRES_ICUM_RES_ENQUE;
    }

    errno = R_ICUMIF_IsServiceCompleted(p_ISD);
    printf("ERR No = %d\n",errno);
    /* Parameter alignment error */
    if(errno == R_ICUMIF_ER_PARAM_ALIGNMENT)
    {
        printf("R_ICUMIF_ER_PARAM_ALIGNMENT !! \n");
    }
    /* Address not within global RAM area */
    if(errno == R_ICUMIF_ER_ADDR_NOT_G_RAM)
    {
        printf("R_ICUMIF_ER_ADDR_NOT_G_RAM !! \n");
    }

    for (next_idx = 0; next_idx < RESPONSE_QUEUE_LENGTH; next_idx++)
    {
        if (0U == (*(p_queptr + next_idx)))
        {
            break;
        }
        else
        {
            use_queue++;
        }
    }
    printf("RESPONSE QUEUE Use Area : %d\n",use_queue);
}

/*===========================================================================
  R_ICUMIF_ServiceCompleted_SoftError_Test
  ===========================================================================*/
 
void R_ICUMIF_ServiceCompleted_SoftError_Test()
{
    r_icumif_isd_t *p_ISD;
    int32_t         errno;
    int32_t         req_res_status;

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    // p_ISD header
    printf("\n| req_res_status \n");
    printf("|---------------------------------\n");
    printf("  ?> ");

    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &req_res_status);

    p_ISD->req_res_status = req_res_status;
    
    errno = R_ICUMIF_IsServiceCompleted(p_ISD);
    printf("ERR No = %d\n",errno);
    /* Parameter alignment error */
    if(errno == R_ICUMIF_ER_PARAM_ALIGNMENT)
    {
        printf("R_ICUMIF_ER_PARAM_ALIGNMENT !! \n");
    }
    /* Address not within global RAM area */
    if(errno == R_ICUMIF_ER_ADDR_NOT_G_RAM)
    {
        printf("R_ICUMIF_ER_ADDR_NOT_G_RAM !! \n");
    }

    if(errno == R_ICUMIF_ER_SOFTWARE_ERROR)
    {
        printf("R_ICUMIF_ER_SOFTWARE_ERROR !! \n");
    }

}

/*===========================================================================
  R_ICUMIF_ServiceCompleted_Test
  ===========================================================================*/
 
void R_ICUMIF_ServiceCompleted_Test()
{
    r_icumif_isd_t *p_ISD[6];
    int32_t errno;
    int32_t nointerrupt;
    int32_t running_check;
    volatile uint32_t   *p_reg;
    ptr_manip_t         queptr;
    uint32_t            *p_queptr;
    int32_t             next_idx;
    int32_t             used_queue = 0;
    int32_t             use_queue = 0;

    // ICUM_CMDREGICUM_SEMAPHORE
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;

    // QUE_ADDR_LOWER_24BITS,QUE_ADDR_UPPER_8BITS_VALUE
    queptr.val = (*p_reg & QUE_ADDR_LOWER_24BITS) | QUE_ADDR_UPPER_8BITS_VALUE;
    p_queptr = (uint32_t *)queptr.p_ptr;
    p_queptr += REQUEST_QUEUE_LENGTH;

    // initialize the global icum service header
    p_ISD[0]  = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD[1]  = p_ISD[0] + sizeof(r_icumif_isd_t);
    p_ISD[2]  = p_ISD[1] + sizeof(r_icumif_isd_t);
    p_ISD[3]  = p_ISD[2] + sizeof(r_icumif_isd_t);
    p_ISD[4]  = p_ISD[3] + sizeof(r_icumif_isd_t);

    // INTERRPUT
    printf("\n| INTERRPUT SET \n");
    printf("|0:REQRES_INTERRPUT \n");
    printf("|1:REQRES_NOINTERRPUT \n");
    printf("|---------------------------------\n");
    printf("  ?> ");

    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &nointerrupt);
    
    if(nointerrupt == 0){
    // RUNNING_CHECK
    printf("\n| RUNNING_CHECK? \n");
    printf("|0:SERV_COMPLETION \n");
    printf("|1:SERV_RUNNING \n");
    printf("|---------------------------------\n");
    printf("  ?> ");

    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &running_check);
    }
    else
    {
    running_check = 1;
    }


    for (next_idx = 0; next_idx < RESPONSE_QUEUE_LENGTH; next_idx++)
    {
        if (0U == (*(p_queptr + next_idx)))
        {
            break;
        }
        else
        {
            used_queue++;
        }
    }

    // AES_KEY_UPDATE_PLAIN
    if(used_queue ==0){
    // AES_KEY_UPDATE_PLAIN
    if(nointerrupt == 1){
            p_ISD[0]->res_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
    }
    else
    {
            p_ISD[0]->res_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
    }
        p_ISD[0]->req_nointerrupt                               = R_ICUMIF_REQRES_INTERRPUT;
        p_ISD[0]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[0]->p_callbackfunc                                = 0;
        p_ISD[0]->job_id                                        = 0;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.key_id                   = 3;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[0]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;

    if(running_check ==1)
    {
        R_ICUMIF_ServiceRequest(p_ISD[0]);
    }
    else
    {
        trigger_service_and_wait(p_ISD[0]);
        errno = R_ICUMIF_IsServiceCompleted(p_ISD[0]);
        }
   }else if(used_queue ==1){
        if(nointerrupt == 1){
            p_ISD[1]->res_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
        }
        else
        {
            p_ISD[1]->res_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
        }
        p_ISD[1]->req_nointerrupt                               = R_ICUMIF_REQRES_INTERRPUT;
        p_ISD[1]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[1]->p_callbackfunc                                = 0;
        p_ISD[1]->job_id                                        = 0;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.key_id                   = 4;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[1]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
        
        if(running_check ==1)
        {
        R_ICUMIF_ServiceRequest(p_ISD[1]);
        }
        else
        {
        trigger_service_and_wait(p_ISD[1]);
        errno = R_ICUMIF_IsServiceCompleted(p_ISD[1]);
        }
   }else if(used_queue ==2){
        if(nointerrupt == 1){
            p_ISD[2]->res_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
        }
        else
        {
            p_ISD[2]->res_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
        }
        p_ISD[2]->req_nointerrupt                               = R_ICUMIF_REQRES_INTERRPUT;
        p_ISD[2]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[2]->p_callbackfunc                                = 0;
        p_ISD[2]->job_id                                        = 0;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.key_id                   = 5;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[2]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
        
        if(running_check ==1)
        {
        R_ICUMIF_ServiceRequest(p_ISD[2]);
        }
        else
        {
        trigger_service_and_wait(p_ISD[2]);
        errno = R_ICUMIF_IsServiceCompleted(p_ISD[2]);
        }
   }else if(used_queue ==3){
        if(nointerrupt == 1){
            p_ISD[3]->res_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
        }
        else
        {
            p_ISD[3]->res_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
        }
        p_ISD[3]->req_nointerrupt                               = R_ICUMIF_REQRES_INTERRPUT;
        p_ISD[3]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[3]->p_callbackfunc                                = 0;
        p_ISD[3]->job_id                                        = 0;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.key_id                   = 6;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[3]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
        
        if(running_check ==1)
        {
        R_ICUMIF_ServiceRequest(p_ISD[3]);
        }
        else
        {
        trigger_service_and_wait(p_ISD[3]);
        errno = R_ICUMIF_IsServiceCompleted(p_ISD[3]);
        }
   }else if((used_queue ==4)&&(0U == (*(p_queptr + 3U)))){
        if(nointerrupt == 1){
            p_ISD[4]->res_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
        }
        else
        {
            p_ISD[4]->res_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
        }
        p_ISD[4]->req_nointerrupt                               = R_ICUMIF_REQRES_INTERRPUT;
        p_ISD[4]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[4]->p_callbackfunc                                = 0;
        p_ISD[4]->job_id                                        = 0;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.key_id                   = 7;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[4]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
        
        if(running_check ==1)
        {
        R_ICUMIF_ServiceRequest(p_ISD[4]);
        }
        else
        {
        trigger_service_and_wait(p_ISD[4]);
        errno = R_ICUMIF_IsServiceCompleted(p_ISD[4]);
        }
   }else{
        if(nointerrupt == 1){
            p_ISD[5]->res_nointerrupt                           = R_ICUMIF_REQRES_NOINTERRPUT;
        }
        else
        {
            p_ISD[5]->res_nointerrupt                           = R_ICUMIF_REQRES_INTERRPUT;
        }
        p_ISD[5]->req_nointerrupt                               = R_ICUMIF_REQRES_INTERRPUT;
        p_ISD[5]->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
        p_ISD[5]->p_callbackfunc                                = 0;
        p_ISD[5]->job_id                                        = 0;
        p_ISD[5]->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
        p_ISD[5]->prm.KEY_UPDATE_PLAIN.key_id                   = 8;
        p_ISD[5]->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
        p_ISD[5]->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
        p_ISD[5]->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
        p_ISD[5]->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
        p_ISD[5]->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
        p_ISD[5]->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
        p_ISD[5]->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
        
        if(running_check ==1)
        {
        R_ICUMIF_ServiceRequest(p_ISD[5]);
        }
        else
        {
        trigger_service_and_wait(p_ISD[5]);
        errno = R_ICUMIF_IsServiceCompleted(p_ISD[5]);
        }
    }

    /* R_ICUMIF_RTN_SERV_RUNNING */
    if(errno == R_ICUMIF_RTN_SERV_RUNNING)
    {
        printf("R_ICUMIF_RTN_SERV_RUNNING !! \n");
    }
    /* R_ICUMIF_RTN_SERV_COMPLETION */
    if(errno == R_ICUMIF_RTN_SERV_COMPLETION)
    {
        printf("R_ICUMIF_RTN_SERV_COMPLETION !! \n");
    }

    if((nointerrupt == 1)&&(0U == (*(p_queptr + 3U))))
    {
       while(0U == (*(p_queptr + used_queue)));
    }

    for (next_idx = 0; next_idx < RESPONSE_QUEUE_LENGTH; next_idx++)
    {
        if (0U == (*(p_queptr + next_idx)))
        {
            break;
        }
        else
        {
            use_queue++;
        }
    }
    printf("RESPONSE QUEUE Use Area : %d\n",use_queue);
}

/*===========================================================================
  R_ICUMIF_ServiceResponse_Test
  ===========================================================================*/
 
void R_ICUMIF_ServiceResponse_Test()
{
    r_icumif_isd_t *p_ISD[6];
    volatile uint32_t   *p_reg;
    ptr_manip_t         queptr;
    uint32_t            *p_queptr;
    int32_t             next_idx;
    int32_t             use_queue = 0;


    // ICUM_CMDREGICUM_SEMAPHORE
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;

    // QUE_ADDR_LOWER_24BITS,QUE_ADDR_UPPER_8BITS_VALUE
    queptr.val = (*p_reg & QUE_ADDR_LOWER_24BITS) | QUE_ADDR_UPPER_8BITS_VALUE;
    p_queptr = (uint32_t *)queptr.p_ptr;
    p_queptr += REQUEST_QUEUE_LENGTH;

    // initialize the global icum service header
    p_ISD[0]  = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD[1]  = p_ISD[0] + sizeof(r_icumif_isd_t);
    p_ISD[2]  = p_ISD[1] + sizeof(r_icumif_isd_t);
    p_ISD[3]  = p_ISD[2] + sizeof(r_icumif_isd_t);
    p_ISD[4]  = p_ISD[3] + sizeof(r_icumif_isd_t);
    
    R_ICUMIF_IsServiceCompleted(p_ISD[0]);

    for (next_idx = 0; next_idx < RESPONSE_QUEUE_LENGTH; next_idx++)
    {
        if (0U == (*(p_queptr + next_idx)))
        {
            break;
        }
        else
        {
            use_queue++;
        }
    }
    printf("RESPONSE QUEUE Use Area : %d\n",use_queue);
}

/*===========================================================================
  Dummy_Semaphore_Register
  ===========================================================================*/

void Dummy_Semaphore_Set()
{
    register volatile uint32_t *p_reg;
    uint32_t uiAuth;
    uint32_t uiIcusem;
    uint32_t uiCount;

    /* Get the Semaphore register address */
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;
    uiCount = 0UL;

    /* Wait for the Semaphore */
    do {
        /* Try to get the Semaphore as step1 */
        uiIcusem = *p_reg;
        uiAuth   = uiIcusem & PE_SEMAPHORE_AUTH_MASK;

        if (PE_SEMAPHORE_AUTH_GET1ST == uiAuth)
        {
            /* Try to get the Semaphore as step2 */
            uiIcusem = *p_reg;
            uiAuth   = (uiIcusem & PE_SEMAPHORE_AUTH_MASK);

            /*PE_SEMAPHORE_AUTH_GET2ND */
            if (PE_SEMAPHORE_AUTH_GET2ND != uiAuth)
            {
                /* Error */
                printf("SEMAPHORE ACCSESS ERROR!!\n");
            }
            else
            {
                /* Got the Semaphore */
                printf("SEMAPHORE ACCSESS GET!!\n");
                *p_reg |= PE_SEMAPHORE_REQ;
            }
            break;
        }
        else if (PE_SEMAPHORE_AUTH_GET2ND == uiAuth)
        {
            /* Got the Semaphore */
            printf("SEMAPHORE ACCSESS GET!!\n");
            *p_reg |= PE_SEMAPHORE_REQ;
            break;
        }
        else
        {
            /* Do nothing */
        }
        uiCount++;
    } while (uiCount < PE_SEMAPHORE_WAIT);
}

/*===========================================================================
  Dummy_Response_Semaphore_Set
  ===========================================================================*/

void Dummy_Response_Semaphore_Set()
{
    register volatile uint32_t *p_reg;
    uint32_t uiAuth;
    uint32_t uiIcusem;
    uint32_t uiCount;

    /* Get the Semaphore register address */
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;
    uiCount = 0UL;

    /* Wait for the Semaphore */
    do {
        /* Try to get the Semaphore as step1 */
        uiIcusem = *p_reg;
        uiAuth   = uiIcusem & PE_SEMAPHORE_AUTH_MASK;

        if (PE_SEMAPHORE_AUTH_GET1ST == uiAuth)
        {
            /* Try to get the Semaphore as step2 */
            uiIcusem = *p_reg;
            uiAuth   = (uiIcusem & PE_SEMAPHORE_AUTH_MASK);

            /*PE_SEMAPHORE_AUTH_GET2ND */
            if (PE_SEMAPHORE_AUTH_GET2ND != uiAuth)
            {
                /* Error */
                printf("SEMAPHORE ACCSESS ERROR!!\n");
            }
            else
            {
                /* Got the Semaphore */
                printf("SEMAPHORE ACCSESS GET!!\n");
            }
            break;
        }
        else if (PE_SEMAPHORE_AUTH_GET2ND == uiAuth)
        {
            /* Got the Semaphore */
            printf("SEMAPHORE ACCSESS GET!!\n");
            break;
        }
        else
        {
            /* Do nothing */
        }
        uiCount++;
    } while (uiCount < PE_SEMAPHORE_WAIT);
}

/*===========================================================================
  Dummy_Semaphore_Free
  ===========================================================================*/

void Dummy_Semaphore_Free()
{
    register volatile uint32_t *p_reg;
    uint32_t uiAuth;

    /* Get the Semaphore register address */
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;
    uiAuth   = (*p_reg & PE_SEMAPHORE_AUTH_MASK);

    /* Check the Semaphore register status */
    if (PE_SEMAPHORE_AUTH_GET2ND == uiAuth)
    {
        /* Clear Request bit in the Semaphore register */
        *p_reg &= ~PE_SEMAPHORE_RES;
    }
    else
    {
        /* Do nothing */
    }
    /* Free the Semaphore */
    *p_reg &= ~PE_SEMAPHORE_AUTH_MASK;
}

/*===========================================================================
  R_ICUMIF_ICUM_Request_timeout_Test
  ===========================================================================*/
 
void R_ICUMIF_ICUM_Request_timeout_Test()
{
    r_icumif_isd_t *p_ISD;
    volatile uint32_t   *p_reg;
    ptr_manip_t         queptr;
    uint32_t            *p_queptr;
    int32_t             next_idx;
    int32_t             use_queue = 0;

    // ICUM_CMDREGICUM_SEMAPHORE
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;

    // QUE_ADDR_LOWER_24BITS,QUE_ADDR_UPPER_8BITS_VALUE
    queptr.val = (*p_reg & QUE_ADDR_LOWER_24BITS) | QUE_ADDR_UPPER_8BITS_VALUE;
    p_queptr = (uint32_t *)queptr.p_ptr;

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    
    // AES_KEY_UPDATE_PLAIN
    memset(p_ISD, 0, sizeof(r_icumif_isd_t));
    p_ISD->req_nointerrupt                               = R_ICUMIF_REQRES_INTERRPUT;
    p_ISD->res_nointerrupt                               = R_ICUMIF_REQRES_INTERRPUT;
    p_ISD->service_id                                    = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD->p_callbackfunc                                = 0;
    p_ISD->job_id                                        = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_group                = KEY_GRP_AES;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_id                   = 3;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes        = 128/8;
    p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value              = KEY;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
    
    R_ICUMIF_ServiceRequest(p_ISD);
    Dummy_Semaphore_Set();

    while(1)
    {
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD->req_res_status )
        {
            if( SERV_REQUEST > p_ISD->service_result )
            {
                break;
            }
        }
        HALT();
         NOPS(10);
    }

        if(p_ISD->service_result != SERV_OK)
        {
            printf("Service error: %d (", p_ISD->service_result);
/*        printf(R_ICUMIF_GetErrorCodeName(p_ISD->service_result));*/
            printf(") *****\a\n");
        }

    for (next_idx = 0; next_idx < REQUEST_QUEUE_LENGTH; next_idx++)
    {
        if (0U == (*(p_queptr + next_idx)))
        {
            break;
        }
        else
        {
            use_queue++;
        }
    }
    printf("REQUEST QUEUE Use Area : %d\n",use_queue);
}

/*===========================================================================
  R_ICUMIF_ServiceRequest_timeout_Test
  ===========================================================================*/

void R_ICUMIF_ServiceRequest_timeout_Test()
{
    r_icumif_isd_t *p_ISD;
    volatile uint32_t   *p_reg;
    ptr_manip_t         queptr;
    uint32_t            *p_queptr;
    int32_t             next_idx;
    int32_t             use_queue = 0;
    int32_t             errno;

    // ICUM_CMDREGICUM_SEMAPHORE
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;

    // QUE_ADDR_LOWER_24BITS,QUE_ADDR_UPPER_8BITS_VALUE
    queptr.val = (*p_reg & QUE_ADDR_LOWER_24BITS) | QUE_ADDR_UPPER_8BITS_VALUE;
    p_queptr = (uint32_t *)queptr.p_ptr;

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    // AES_CMAC
    memset(p_ISD, 0, sizeof(r_icumif_isd_t));
    p_ISD->req_nointerrupt                  = R_ICUMIF_REQRES_INTERRPUT;
    p_ISD->res_nointerrupt                  = R_ICUMIF_REQRES_INTERRPUT;
    p_ISD->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD->p_callbackfunc                   = 0;
    p_ISD->job_id                           = 0;
    p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)BIG_BUFFER[1];
    p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_CMAC.msg_size_in_bits    = 4194304;
    p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_AES;
    p_ISD->prm.AES_CMAC.key_id              = 3;
    p_ISD->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
    p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD->prm.AES_CMAC.job_slice           = 1;
    p_ISD->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;

    errno = R_ICUMIF_ServiceRequest(p_ISD);

    /* REQUEST TIMEOUT */
    if(errno == R_ICUMIF_ER_SOFTWARE_ERROR)
    {
        printf("R_ICUMIF_ER_SOFTWARE_ERROR !! \n");
    }

    for (next_idx = 0; next_idx < REQUEST_QUEUE_LENGTH; next_idx++)
    {
        if (0U == (*(p_queptr + next_idx)))
        {
            break;
        }
        else
        {
            use_queue++;
        }
    }
    printf("REQUEST QUEUE Use Area : %d\n",use_queue);
}


/*===========================================================================
  R_ICUMIF_ServiceResponse_timeout_Test
  ===========================================================================*/
 
void R_ICUMIF_ServiceResponse_timeout_Test()
{
    r_icumif_isd_t *p_ISD;
    volatile uint32_t   *p_reg;
    ptr_manip_t         queptr;
    uint32_t            *p_queptr;
    int32_t             next_idx;
    int32_t             use_queue = 0;

    // ICUM_CMDREGICUM_SEMAPHORE
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;

    // QUE_ADDR_LOWER_24BITS,QUE_ADDR_UPPER_8BITS_VALUE
    queptr.val = (*p_reg & QUE_ADDR_LOWER_24BITS) | QUE_ADDR_UPPER_8BITS_VALUE;
    p_queptr = (uint32_t *)queptr.p_ptr;
    p_queptr += REQUEST_QUEUE_LENGTH;

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    // AES_CMAC
    memset(p_ISD, 0, sizeof(r_icumif_isd_t));
    p_ISD->req_nointerrupt                  = R_ICUMIF_REQRES_INTERRPUT;
    p_ISD->res_nointerrupt                  = R_ICUMIF_REQRES_INTERRPUT;
    p_ISD->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD->p_callbackfunc                   = 0;
    p_ISD->job_id                           = 0;
    p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)BIG_BUFFER[1];
    p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_CMAC.msg_size_in_bits    = 4194304;
    p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_AES;
    p_ISD->prm.AES_CMAC.key_id              = 3;
    p_ISD->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
    p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD->prm.AES_CMAC.job_slice           = 1;
    p_ISD->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;

    R_ICUMIF_ServiceRequest(p_ISD);
    Dummy_Response_Semaphore_Set();

    while(1)
    {
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD->req_res_status )
        {
            if( SERV_REQUEST > p_ISD->service_result )
            {
                break;
            }
        }
        HALT();
         NOPS(10);
    }

        if(p_ISD->service_result != SERV_OK)
        {
            printf("Service error: %d (", p_ISD->service_result);
/*        printf(R_ICUMIF_GetErrorCodeName(p_ISD->service_result));*/
            printf(") *****\a\n");
        }

    for (next_idx = 0; next_idx < RESPONSE_QUEUE_LENGTH; next_idx++)
    {
        if (0U == (*(p_queptr + next_idx)))
        {
            break;
        }
        else
        {
            use_queue++;
        }
    }
    printf("RESPONSE QUEUE Use Area : %d\n",use_queue);
}

static void isd_call_back_test(r_icumif_isd_t  *p_isd)
{
    isd_call_back_flag = 1;
    return;
}

/*===========================================================================
  R_ICUMIF_ServiceResponse_CallBack_Test
  ===========================================================================*/
 
void R_ICUMIF_ServiceResponse_CallBack_Test()
{
    r_icumif_isd_t *p_ISD;
    int32_t callbackfunc;
    volatile uint32_t   *p_reg;
    ptr_manip_t         queptr;
    uint32_t            *p_queptr;
    int32_t             next_idx;
    int32_t             use_queue = 0;

    isd_call_back_flag = 0;
    // ICUM_CMDREGICUM_SEMAPHORE
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;

    // QUE_ADDR_LOWER_24BITS,QUE_ADDR_UPPER_8BITS_VALUE
    queptr.val = (*p_reg & QUE_ADDR_LOWER_24BITS) | QUE_ADDR_UPPER_8BITS_VALUE;
    p_queptr = (uint32_t *)queptr.p_ptr;
    p_queptr += REQUEST_QUEUE_LENGTH;

    // p_callbackfunc
    printf("\n| CallBack FUNC SET \n");
    printf("|0:SET NULL \n");
    printf("|1:SET FUNC \n");
    printf("|---------------------------------\n");
    printf("  ?> ");

    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &callbackfunc);

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    // AES_CMAC
    memset(p_ISD, 0, sizeof(r_icumif_isd_t));
    p_ISD->req_nointerrupt                  = R_ICUMIF_REQRES_INTERRPUT;
    p_ISD->res_nointerrupt                  = R_ICUMIF_REQRES_INTERRPUT;
    p_ISD->service_id                       = SERVICE_01_AES_CMAC;
    if (0 == callbackfunc)
    {
        p_ISD->p_callbackfunc                   = 0;
    }
    else
    {
        p_ISD->p_callbackfunc                   = (p_func_t)&isd_call_back_test;
    }
    p_ISD->job_id                           = 0;
    p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)BIG_BUFFER[1];
    p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_CMAC.msg_size_in_bits    = 32768;
    p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_AES;
    p_ISD->prm.AES_CMAC.key_id              = 3;
    p_ISD->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
    p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD->prm.AES_CMAC.job_slice           = 1;
    p_ISD->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;

    R_ICUMIF_ServiceRequest(p_ISD);

    while(1)
    {
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD->req_res_status )
        {
            if( SERV_REQUEST > p_ISD->service_result )
            {
                break;
            }
        }
        HALT();
         NOPS(10);
    }

    if(p_ISD->service_result != SERV_OK)
    {
        printf("Service error: %d (", p_ISD->service_result);
        printf(") *****\a\n");
    }

    for (next_idx = 0; next_idx < RESPONSE_QUEUE_LENGTH; next_idx++)
    {
        if (0U == (*(p_queptr + next_idx)))
        {
            break;
        }
        else
        {
            use_queue++;
        }
    }
    printf("RESPONSE QUEUE Use Area : %d\n",use_queue);
    printf("ISD CallBack FUNC : %d\n",isd_call_back_flag)
}

/*===========================================================================
  R_ICUMIF_ServiceResponse_Error_Test
  ===========================================================================*/
 
void R_ICUMIF_ServiceResponse_Error_Test()
{
    int32_t             errno;

    errno = R_ICUMIF_ServiceResponse();
    printf("ERR No = %d\n",errno);
    /* NOT_INIT */
    if(errno == R_ICUMIF_ER_IF_LIB_NOT_INIT)
    {
        printf("R_ICUMIF_ER_IF_LIB_NOT_INIT !! \n");
    }
    /* Software error */
    else if(errno == R_ICUMIF_ER_SOFTWARE_ERROR)
    {
        printf("R_ICUMIF_ER_SOFTWARE_ERROR !!\n");
    }

}

/*===========================================================================
  R_ICUMIF_SetSystemCallBackFunc_Test
  ===========================================================================*/

void R_ICUMIF_SetSystemCallBackFunc_Test(void)
{
    int32_t rtn;
    uint32_t regist_info_address;

    printf("\nInput p_regist_info address[HEX]\n");
    printf("|---------------------------------\n");
    printf("  ?> ");
    
    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &regist_info_address);

    rtn = R_ICUMIF_SetSystemCallBackFunc((R_ICUMIF_CB_REGIST_t *)regist_info_address);
    printf("ERR No = %d\n", rtn);
}

/*===========================================================================
  R_ICUMIF_SetFlashCallBackFunc_Test
  ===========================================================================*/

void R_ICUMIF_SetFlashCallBackFunc_Test(void)
{
    int32_t rtn;
    uint32_t regist_info_address;

    printf("\nInput p_regist_info address[HEX]\n");
    printf("|---------------------------------\n");
    printf("  ?> ");

    vrun_console_get();
    sscanf(vrun_iobuff_readparam(0), "%i", &regist_info_address);

    rtn = R_ICUMIF_SetFlashCallBackFunc((R_ICUMIF_CB_REGIST_t *)regist_info_address);
    printf("ERR No = %d\n", rtn);
}


/*===========================================================================
  KEY_MNGT_TEST
  ===========================================================================*/
 
void FACIACC_NotAcc_Service_Test()
{
    int32_t        DF_ACC = 0;
    r_icumif_isd_t *p_ISD;
    r_icumif_sts_t *ptrICU2PES;

    // initialize the global icum service header
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    // AES_KEY_UPDATE_PLAIN
    memset(p_ISD, 0, sizeof(r_icumif_isd_t));
    p_ISD->req_nointerrupt                  = R_ICUMIF_REQRES_INTERRPUT;
    p_ISD->res_nointerrupt                  = R_ICUMIF_REQRES_INTERRPUT;
    p_ISD->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD->p_callbackfunc                   = 0;
    p_ISD->job_id                           = 0;
    p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)BIG_BUFFER[1];
    p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
    p_ISD->prm.AES_CMAC.msg_size_in_bits    = 128;
    p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_AES;
    p_ISD->prm.AES_CMAC.key_id              = 2;
    p_ISD->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
    p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD->prm.AES_CMAC.job_slice           = 1;
    p_ISD->prm.AES_CMAC.job_cycle           = JOB_ALL_AT_ONCE;

    R_ICUMIF_ServiceRequest(p_ISD);

    while(1)
    {
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD->req_res_status )
        {
            if( SERV_REQUEST > p_ISD->service_result )
            {
                break;
            }
        }
        ptrICU2PES = R_ICUMIF_GetStatus();
        if(0x00 != ptrICU2PES->FACIACC )
        {
            DF_ACC = 1;
        }
        HALT();
         NOPS(10);
    }

    if(p_ISD->service_result != SERV_OK)
    {
        printf("Service error: %d (", p_ISD->service_result);
/*    printf(R_ICUMIF_GetErrorCodeName(p_ISD->service_result));*/
        printf(") *****\a\n");
    }
    else{
        printf("SERV_OK \n");
    }
    
    if(0 == DF_ACC)
    {
    printf("OK:DataFlash Not Accsess \n");
    }
    else{
    printf("NG:DataFlash Accsess!! \n");
    }
}

/*===========================================================================
  For VT UseCase Test
  ===========================================================================*/
void faci_command_error_check( void ) {
    // check for errors
    if( (REG_PE_FACIFASTAT&0x10/*CMDLK*/) || (REG_PE_FACIFSTATR&(0x00004000/*ILGLERR*/|0x00002000/*ERSERR*/|0x00001000/*PRGERR*/)) ){
        if (REG_PE_FACIFASTAT&0x10/*CMDLK*/) {
            printf("CMDLK_error_occured\n");
        }
        else if (REG_PE_FACIFSTATR&0x00004000/*ILGLERR*/) {
            printf("ILGLERR_error_occured\n");
        }
        else if (REG_PE_FACIFSTATR&0x00002000/*ERSERR*/) {
            printf("ERSERR_error_occured\n");
        }
        else if (REG_PE_FACIFSTATR&0x00001000/*PRGERR*/) {
            printf("PRGERR_error_occured\n");
        }
        else {
            printf("OTHER_error_occured\n");
        }
    }
    else {
        printf("error_not_occured\n");
    }
}

void make_ecc_test_ISD( r_icumif_isd_t *p_ISD, ecc_test_operation_t op ) {
    p_ISD->service_id                        = SERVICE_00_ECC_TEST;
    p_ISD->p_callbackfunc                    = 0;
    p_ISD->job_id                            = 0;
    p_ISD->prm.ECC_TEST.ecc_test_operation   = op;
}


void make_cmac_ISD( r_icumif_isd_t *p_ISD, uint32_t *p_msag, uint32_t *p_cmac, r_icumif_service_priority_t priority ) {
    p_ISD->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD->p_callbackfunc                   = 0;
    p_ISD->job_id                           = 0;
    p_ISD->prm.AES_CMAC.p_msg               = p_msag;
    p_ISD->prm.AES_CMAC.p_cmac              = p_cmac;
    p_ISD->prm.AES_CMAC.msg_size_in_bits    = 64 * 1024;
    p_ISD->prm.AES_CMAC.key_group           = KEY_GRP_AES;
    p_ISD->prm.AES_CMAC.key_id              = 2;
    p_ISD->prm.AES_CMAC.cmac_size_in_bits   = 128;
    p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD->prm.AES_CMAC.job_slice           = (r_job_slice_t)0;
    p_ISD->prm.AES_CMAC.job_cycle           = (r_job_cycle_t)0;
    p_ISD->prm.AES_CMAC.verification_result = (r_verif_result_t)0;
    p_ISD->service_priority                 = priority;
}

void make_key_update_plain_ISD( r_icumif_isd_t *p_ISD ) {
    p_ISD->service_id                                   = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD->p_callbackfunc                               = 0;
    p_ISD->job_id                                       = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value             = &BIG_BUFFER[0][0];
    p_ISD->prm.KEY_UPDATE_PLAIN.key_group               = KEY_GRP_AES;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_id                  = 3;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes       = 128 / 8;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption        = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption        = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation   = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug  = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.write_protected   = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.no_wild_card      = 0;
}


/*===========================================================================
  RAM execution mode Test
  ===========================================================================*/
void ram_execute_check( r_icumif_isd_t *p_ISD ) {
    

    // not wait
    R_ICUMIF_ServiceRequest(p_ISD);
    char flag = 0;
    
    while(1)
    {
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD->req_res_status )
        {
            if( SERV_REQUEST > p_ISD->service_result )
            {
                break;
            }
        }
        else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD->req_res_status)
                && (p_ISD->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
        {
            break;
        }
        
        else {
                if ( 1 != flag ) {
                    printf("RAM_execute\n");
                }
                flag = 1;
            }
    }
    if ( 0 == flag ) {
        printf("not_RAM_execute\n");
    }
    else {
        // ram_execute
    }
}

void cmac_check( r_icumif_isd_t *p_ISD ) {
    
    char flag = 0;

    r_icumif_isd_t     *p_ISD2;
    p_ISD2  = p_ISD + 1;
    r_icumif_isd_t     *p_ISD3;
    p_ISD3  = p_ISD2 + 1;

    R_ICUMIF_ServiceRequest(p_ISD);
    
    while(1)
    {
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD->req_res_status )
        {
            if( SERV_REQUEST > p_ISD->service_result )
            {
                break;
            }
        }
        else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD->req_res_status)
                && (p_ISD->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
        {
            break;
        }
        
        else {
                make_cmac_ISD( p_ISD3, (uint32_t *)ADDR_LOWER_CRAM, (uint32_t *)(ADDR_LOWER_CRAM + 64), R_ICUMIF_SERV_PRIORITY_HIGHEST );
                trigger_service_and_wait(p_ISD3);
                
                // check cmac result
                if ( SERV_OK == p_ISD3->service_result ) {
                    if ( 1 != flag ) {
                        printf("OK, CMAC in RAM execute\n");
                    }
                }
                else {
                    printf("NG, CMAC in RAM execute\n");
                }
                flag = 1;
            }
    }
    if ( 0 == flag ) {
        printf("not_RAM_execute\n");
    }
    else {
        // ram_execute
    }
}

void test_ram_execution( void ) {
    
    int n;
    int exit_loop = 0;

    r_icumif_isd_t *p_ISD;
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    while(!exit_loop)
    {
            printf("\n\n| test for RAM execution\n");
            printf("|-------------------------------------\n");
            printf("| 910  : RAM execute in ECC_TEST Test\n");
            printf("| 911  : CMAC in ECC_TEST Test\n");
            printf("| 912  : CMAC(not fast) in ECC_TEST Test\n");
            printf("| q    : Quit\n\n");
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
            // RAM execute in ECC_TEST Test
            case 910:
                make_ecc_test_ISD( p_ISD, ECC_TEST_START );
                trigger_service_and_wait( p_ISD );
                make_ecc_test_ISD( p_ISD, ECC_TEST_END );
            
                trigger_service_and_wait( p_ISD );
                break;

            // CMAC in ECC_TEST Test
            case 911:
                make_ecc_test_ISD( p_ISD, ECC_TEST_START );
                trigger_service_and_wait(p_ISD);
                make_cmac_ISD( p_ISD, (uint32_t *)ADDR_LOWER_CRAM, (uint32_t *)(ADDR_LOWER_CRAM + 64), R_ICUMIF_SERV_PRIORITY_HIGHEST );
                trigger_service_and_wait(p_ISD);
                
                if ( SERV_OK == p_ISD->service_result ) {
                    printf("OK, CMAC in RAM execute\n");
                }
                else {
                    printf("NG, CMAC in RAM execute\n");
                }
                make_ecc_test_ISD( p_ISD, ECC_TEST_END );
            
                trigger_service_and_wait( p_ISD );
                break;
            
            // CMAC(not fast) in ECC_TEST Test
            case 912:
                make_ecc_test_ISD( p_ISD, ECC_TEST_START );
                trigger_service_and_wait(p_ISD);
                make_cmac_ISD( p_ISD, (uint32_t *)ADDR_LOWER_CRAM, (uint32_t *)(ADDR_LOWER_CRAM + 64), R_ICUMIF_SERV_PRIORITY_NORMAL );
                trigger_service_and_wait(p_ISD);
                
                if ( SERV_OK == p_ISD->service_result ) {
                    printf("OK, CMAC in RAM execute\n");
                }
                else {
                    printf("NG, CMAC in RAM execute\n");
                }
                make_ecc_test_ISD( p_ISD, ECC_TEST_END );
            
                trigger_service_and_wait( p_ISD );
                break;
            
            default:
                break;
        }
    }    
}

/*===========================================================================
  Locking/Unlocking ECC test Test
  ===========================================================================*/
void test_ecc_test_mode( void ) {
    
    int n;
    int exit_loop = 0;

    r_icumif_isd_t *p_ISD;
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    while(!exit_loop)
    {
            printf("\n\n| test for ECC test mode\n");
            printf("|-------------------------------------\n");
            printf("| 851  : Start ECC test \n");
            printf("| 852  : Check lock -> unlock\n");
            printf("| 853  : End ECC test\n");
            printf("| 854  : Check unlock -> lock\n");
            printf("| q    : Quit\n\n");
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
            // Start ECC test
            case 851:
                make_ecc_test_ISD( p_ISD, ECC_TEST_START );
                R_ICUMIF_ServiceRequest(p_ISD);
                
                while(1)
                {
                    if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD->req_res_status )
                    {
                        if( SERV_REQUEST > p_ISD->service_result )
                        {
                            break;
                        }
                    }
                    else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD->req_res_status)
                            && (p_ISD->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
                    {
                        break;
                    }
                }
                if ( SERV_OK == p_ISD->service_result ) {
                    printf("SERV_OK, ECC_TEST_START request\n");
                }
                else {
                    printf("SERV_ERROR, ECC_TEST_START request\n");
                }
                break;
            
            // Check lock -> unlock
            case 852:
                while(1) {
                    make_ecc_test_ISD( p_ISD, ECC_TEST_STATUS );
                    R_ICUMIF_ServiceRequest(p_ISD);
                    
                    while(1)
                    {
                        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD->req_res_status )
                        {
                            if( SERV_REQUEST > p_ISD->service_result )
                            {
                                break;
                            }
                        }
                        else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD->req_res_status)
                                && (p_ISD->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
                        {
                            break;
                        }
                    }
                    if ( SERV_OK == p_ISD->service_result ) {
                        if ( ECC_TEST_LOCKED == p_ISD->prm.ECC_TEST.ecc_test_lock_status ) {
                        }
                        else {
                            printf("ECC_TEST_UNLOCKED, ECC test status\n");
                            break;
                        }
                    }
                    else {
                        printf("SERV_ERROR, ECC_TEST_STATUS request\n");
                    }
                }
                break;
            
            // End ECC test
            case 853:
                make_ecc_test_ISD( p_ISD, ECC_TEST_END );
                R_ICUMIF_ServiceRequest(p_ISD);
                
                while(1)
                {
                    if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD->req_res_status )
                    {
                        if( SERV_REQUEST > p_ISD->service_result )
                        {
                            break;
                        }
                    }
                    else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD->req_res_status)
                            && (p_ISD->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
                    {
                        break;
                    }
                }
                if ( SERV_OK == p_ISD->service_result ) {
                    printf("SERV_OK, ECC_TEST_END request\n");
                }
                else {
                    printf("SERV_ERROR, ECC_TEST_END request\n");
                }
                break;
            
            // Check unlock -> lock
            case 854:
                while(1) {
                    make_ecc_test_ISD( p_ISD, ECC_TEST_STATUS );
                    R_ICUMIF_ServiceRequest(p_ISD);
                    
                    while(1)
                    {
                        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD->req_res_status )
                        {
                            if( SERV_REQUEST > p_ISD->service_result )
                            {
                                break;
                            }
                        }
                        else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD->req_res_status)
                                && (p_ISD->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
                        {
                            break;
                        }
                    }
                    if ( SERV_OK == p_ISD->service_result ) {
                        if ( ECC_TEST_LOCKED == p_ISD->prm.ECC_TEST.ecc_test_lock_status ) {
                            printf("ECC_TEST_LOCKED, ECC test status\n");
                            break;
                        }
                        else {
                        }
                    }
                    else {
                        printf("SERV_ERROR, ECC_TEST_STATUS request\n");
                    }
                }
                break;
            
            default:
                break;
        }
    }    
}
