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
#include "pe1.int.h"
#include "shared.h"

#include "vrun.api.h"

/*******************************************************************************
* Test functions
*******************************************************************************/
void icum_test_aes_cmac(void)
{
    r_icumif_isd_t *p_ISD;
    
    printf("\n");
    
    /* initialize the global icum service header */
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    
    printf("Operation:    CMAC GENERATION\n");
    
    printf("Key used:     ");
    printf(keys[global_key_id].key_name); NL(); FLUSH();

    /* initialize the service structure */
    p_ISD->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD->p_callbackfunc                   = 0;
    p_ISD->job_id                           = 0;
    p_ISD->prm.AES_CMAC.key_group           = keys[global_key_id].key_group;
    p_ISD->prm.AES_CMAC.key_id              = keys[global_key_id].key_id;
    p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD->prm.AES_CMAC.p_msg               = (uint32_t*)BIG_BUFFER[0];
    p_ISD->prm.AES_CMAC.msg_size_in_bits    = SIZE_OF_BIG_BUFFER * 8;
    p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t*)BIG_BUFFER[1];
    p_ISD->prm.AES_CMAC.cmac_size_in_bits   = 16 * 8;
    p_ISD->prm.AES_CMAC.job_slice           = job_slice[JOB_AES];
    
    printf("Job slice:    %d block(s)\n", p_ISD->prm.AES_CMAC.job_slice);

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
    
    printf("CMAC:         "); PMEM((const unsigned char *)p_ISD->prm.AES_CMAC.p_cmac, 16); NL(); FLUSH();

end:    
    /* restart life signal */
    restart_life_signal();

    printf("Done!\n");
}

typedef struct
{
    uint8_t              output_text[32];
    r_key_group_t        key_group;
    uint8_t              key_id;
    uint32_t             mes_size;
    uint8_t              cmac_size;
    r_job_slice_t        job_slice;
    r_job_cycle_t        job_cycle;
    uint32_t             message[16];
    uint32_t             cmac[4];
} type_test_vector;

static const type_test_vector test_vector[] =
{    /* 64byte message, 16byte cmac */
    {
        {"512bit"},
        /* key_group        key_id           mes_size           cmac_size           job slice       job cycle */
        KEY_GRP_AES,        5,              512,               128,                0,              JOB_ALL_AT_ONCE,
        /* message */
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        /* cmac */
        { 0xbfbef051, 0x929d3b7e, 0x177449fc, 0xfe3c3679 },
    },
    /* 40byte message, 16byte cmac */
    {
        {"320bit"},
        /* key_group        key_id           mes_size           cmac_size           job slice       job cycle */
        KEY_GRP_AES_RAM,    0,               320,               128,                0,              JOB_ALL_AT_ONCE,
        /* message */
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3 },
        /* cmac */
        { 0x4767a6df, 0x30e69ade, 0x6132ca30, 0x27c89714 },
    },
    /* 16byte message, 16byte cmac */
    {
        {"128bit"},
        /* key_group        key_id           mes_size           cmac_size           job slice       job cycle */
        KEY_GRP_AES,        5,              128,               128,                0,              JOB_ALL_AT_ONCE,
        /* message */
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373 },
        /* cmac */
        { 0xb4160a07, 0x44414d6b, 0x9ddd9bf7, 0x7c284ad0 },
    },
    /* 0byte message, 16byte cmac */
    {
        {"  0bit"},
        /* key_group        key_id           mes_size           cmac_size           job slice       job cycle */
        KEY_GRP_AES_RAM,    0,               0,                 128,                0,              JOB_ALL_AT_ONCE,
        /* message */
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        /* cmac */
        { 0x29691dbb, 0x283759e9, 0x127da37f, 0x4667759b },
    },
};
static const uint8_t key_data[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };

/*===========================================================================
  ICU-M test function
  [PUBLIC]
  ===========================================================================*/
void icum_aes_cmac_test(void)
{
    r_icumif_isd_t *p_ISD;

    int n = 4;            /* Registration number */
    int i;
    int size;

    /* initialize the global icum service header */
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nSet AES 9 Key\n"); FLUSH();
    p_ISD->service_id                             = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD->p_callbackfunc                         = 0;
    p_ISD->job_id                                 = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_group         = KEY_GRP_AES;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_id            = 5;
    p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value       = key_data;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes = 16;

    p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 1;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.write_protected    = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.no_wild_card       = 0;

    trigger_service_and_wait(p_ISD);
    /* in case of error, stop here */
    if(p_ISD->service_result == SERV_OK)
    {
        printf("key Registration : OK\n");
    }
    printf("\nSet AES_RAM 0 Key\n"); FLUSH();
    p_ISD->prm.KEY_UPDATE_PLAIN.key_group         = KEY_GRP_AES_RAM;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_id            = 0;

    trigger_service_and_wait(p_ISD);
    /* in case of error, stop here */
    if(p_ISD->service_result == SERV_OK)
    {
        printf("key Registration : OK\n");
    }

    printf("\nCMAC Priority_Nomal Generation\n\n"); FLUSH();
    for(i = 0; i < n; i++)
    {
        memset(p_ISD, 0, sizeof(r_icumif_isd_t));
        p_ISD->service_id                       = SERVICE_01_AES_CMAC;
        p_ISD->p_callbackfunc                   = 0;
        p_ISD->job_id                           = 0;
        p_ISD->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
        p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
        p_ISD->prm.AES_CMAC.key_group           = test_vector[i].key_group;
        p_ISD->prm.AES_CMAC.key_id              = test_vector[i].key_id;
        p_ISD->prm.AES_CMAC.p_msg               = test_vector[i].message;
        p_ISD->prm.AES_CMAC.msg_size_in_bits    = test_vector[i].mes_size;
        p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
        p_ISD->prm.AES_CMAC.cmac_size_in_bits   = test_vector[i].cmac_size;
        p_ISD->prm.AES_CMAC.job_slice           = job_slice[JOB_AES];
        p_ISD->prm.AES_CMAC.job_cycle           = test_vector[i].job_cycle;

        /* trigger the security service */
        vrun_chrono_start();
        /* trigger the security service */
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        /* fetch execution time in vrun_chrono_xxx variables */
        vrun_chrono_gettimestamp();
        printf("\rtiming : %3dus", vrun_chrono_microsec);
        if(vrun_chrono_microsec >= 150) printf(" (!!)");
        NL();

        size = test_vector[i].cmac_size / 8;
        if( test_vector[i].cmac_size % 8 != 0 )
        {
            size++;
        }
        /* in case of error, stop here */
        if( p_ISD->service_result == SERV_OK 
            && ( 0 == memcmp( BIG_BUFFER[0],test_vector[i].cmac,  size )))
        {
            printf("CMAC %s Test Result : OK\n",test_vector[i].output_text); FLUSH();
        }
        else
        {
            printf("CMAC %s Test Result : NG\n",test_vector[i].output_text); FLUSH();
        }
    }
    printf("\nCMAC Priority_Nomal Verification \n\n"); FLUSH();
    for(i = 0; i < n; i++)
    {
        memset(p_ISD, 0, sizeof(r_icumif_isd_t));
        p_ISD->service_id                       = SERVICE_01_AES_CMAC;
        p_ISD->p_callbackfunc                   = 0;
        p_ISD->job_id                           = 0;
        p_ISD->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
        p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
        p_ISD->prm.AES_CMAC.key_group           = test_vector[i].key_group;
        p_ISD->prm.AES_CMAC.key_id              = test_vector[i].key_id;
        p_ISD->prm.AES_CMAC.p_msg               = test_vector[i].message;
        p_ISD->prm.AES_CMAC.msg_size_in_bits    = test_vector[i].mes_size;
        p_ISD->prm.AES_CMAC.cmac_size_in_bits   = test_vector[i].cmac_size;
        p_ISD->prm.AES_CMAC.job_slice           = job_slice[JOB_AES];
        p_ISD->prm.AES_CMAC.job_cycle           = test_vector[i].job_cycle;
        p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t *)&test_vector[i].cmac[0];

        printf("Test No. %d \n",(i+1));
        /* trigger the security service */
        vrun_chrono_start();
        /* trigger the security service */
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        /* fetch execution time in vrun_chrono_xxx variables */
        vrun_chrono_gettimestamp();
        printf("\rtiming : %3dus", vrun_chrono_microsec);
        if(vrun_chrono_microsec >= 150) printf(" (!!)");
        NL();

        size = test_vector[i].cmac_size / 8;
        if( test_vector[i].cmac_size % 8 != 0 )
        {
            size++;
        }
        /* in case of error, stop here */
        if( p_ISD->service_result == SERV_OK)
        {
            printf("CMAC %s Test Result : OK\n",test_vector[i].output_text); FLUSH();
            if( p_ISD->prm.AES_CMAC.verification_result == VERIFICATION_PASS )
            {
                printf("Verify Result : PASS \n"); FLUSH();
            }
            else
            {
                printf("Verify Result : Fail \n"); FLUSH();
            }
        }
        else
        {
            printf("CMAC %s Test Result : NG\n",test_vector[i].output_text); FLUSH();
        }
    }
}

static const type_test_vector test_cycle[] =
{
    // 512bit messageÅA128bit cmac
    {
        {"512bit"},
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        0,               512,               128,            0,              JOB_ALL_AT_ONCE,
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cmac
        { 0xbfbef051, 0x929d3b7e, 0x177449fc, 0xfe3c3679 },
    },
    // 320bit messageÅA128bit cmac
    {
        {"320bit"},
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        0,               320,               128,            0,              JOB_ALL_AT_ONCE,
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3 },
        // cmac
        { 0x4767a6df, 0x30e69ade, 0x6132ca30, 0x27c89714 },
    },
    // 128bit messageÅA128bit cmac
    {
        {"128bit"},
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        0,               128,               128,            0,              JOB_ALL_AT_ONCE,
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373 },
        // cmac
        { 0xb4160a07, 0x44414d6b, 0x9ddd9bf7, 0x7c284ad0 },
    },
};
/*===========================================================================
  ICU-M test function CMAC + Job Cycle
  [PUBLIC]
  ===========================================================================*/
void icum_aes_cmac_test_JobCycle(void)
{
    int i;
    uint8_t size;
    r_icumif_isd_t *p_ISD[3];
    uint32_t *p_cmac[3];
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);

    p_ISD[0]  = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD[1]  = p_ISD[0] + 1;
    p_ISD[2]  = p_ISD[1] + 1;
    p_cmac[0] = (uint32_t*)(p_ISD[2] + 1);
    p_cmac[1] = p_cmac[0] + 4;
    p_cmac[2] = p_cmac[1] + 4;

    printf("\nCMAC job slice = 1\n"); FLUSH();
    for( i = 0; i < 3; i++ )
    {
        p_ISD[i]->service_id                       = SERVICE_01_AES_CMAC;
        p_ISD[i]->p_callbackfunc                   = 0;
        p_ISD[i]->job_id                           = 0;
        p_ISD[i]->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
        p_ISD[i]->prm.AES_CMAC.key_group           = test_cycle[i].key_group;
        p_ISD[i]->prm.AES_CMAC.key_id              = test_cycle[i].key_id;
        p_ISD[i]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
        p_ISD[i]->prm.AES_CMAC.p_msg               = test_cycle[i].message;
        p_ISD[i]->prm.AES_CMAC.msg_size_in_bits    = test_cycle[i].mes_size;
        p_ISD[i]->prm.AES_CMAC.p_cmac              = p_cmac[i];
        p_ISD[i]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle[i].cmac_size;
        p_ISD[i]->prm.AES_CMAC.job_slice           = 1;
    }
    for( i = 0 ; i < 3 ; i++ )
    {
        R_ICUMIF_ServiceRequest(p_ISD[i]);
        while(p_ISD[i]->service_result == SERV_REQUEST);
    }
    // wait for completion of last service (should be the last one to complete)
    while(!R_ICUMIF_IsServiceCompleted(p_ISD[0]))
    {
        HALT();
        NOPS(10);
    }
    while(!R_ICUMIF_IsServiceCompleted(p_ISD[1]))
    {
        HALT();
        NOPS(10);
    }
    while(!R_ICUMIF_IsServiceCompleted(p_ISD[2]))
    {
        HALT();
        NOPS(10);
    }

    for( i = 0 ; i < 3 ; i++ )
    {
        size = test_cycle[i].cmac_size / 8;
        if( test_cycle[i].cmac_size % 8 != 0 )
        {
            size++;
        }
        // in case of error, stop here
        if( p_ISD[i]->service_result == SERV_OK 
            && ( 0 == memcmp( p_cmac[i],test_cycle[i].cmac,  size )))
        {
            printf("Cipher Test Result : test %d SERV_OK\n", i + 1 ); FLUSH();

        }
        else
        {
            printf("Cipher Test Result : test %d not SERV_OK\n", i + 1); FLUSH();
        }
    }

    printf("\njob_cycle + job_slice \n"); FLUSH();
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    p_ISD[0]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[0]->p_callbackfunc                   = 0;
    p_ISD[0]->job_id                           = 0;
    p_ISD[0]->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
    p_ISD[0]->prm.AES_CMAC.key_group           = test_cycle[0].key_group;
    p_ISD[0]->prm.AES_CMAC.key_id              = test_cycle[0].key_id;
    p_ISD[0]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD[0]->prm.AES_CMAC.p_msg               = test_cycle[0].message;
    p_ISD[0]->prm.AES_CMAC.msg_size_in_bits    = test_cycle[0].mes_size;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = p_cmac[0];
    p_ISD[0]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle[0].cmac_size;
    p_ISD[0]->prm.AES_CMAC.job_slice           = 0;
    p_ISD[0]->prm.AES_CMAC.job_cycle           = JOB_INIT;
    R_ICUMIF_ServiceRequest(p_ISD[0]);
    while(p_ISD[0]->service_result == SERV_REQUEST);
    while(!R_ICUMIF_IsServiceCompleted(p_ISD[0]))
    {
        HALT();
        NOPS(10);
    }
    
    p_ISD[1]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[1]->p_callbackfunc                   = 0;
    p_ISD[1]->job_id                           = 0;
    p_ISD[1]->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
    p_ISD[1]->prm.AES_CMAC.key_group           = test_cycle[1].key_group;
    p_ISD[1]->prm.AES_CMAC.key_id              = test_cycle[1].key_id;
    p_ISD[1]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD[1]->prm.AES_CMAC.p_msg               = test_cycle[1].message;
    p_ISD[1]->prm.AES_CMAC.msg_size_in_bits    = test_cycle[1].mes_size;
    p_ISD[1]->prm.AES_CMAC.p_cmac              = p_cmac[1];
    p_ISD[1]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle[1].cmac_size;
    p_ISD[1]->prm.AES_CMAC.job_slice           = 2;
    p_ISD[1]->prm.AES_CMAC.job_cycle           = test_cycle[1].job_cycle;
    R_ICUMIF_ServiceRequest(p_ISD[1]);
    while(p_ISD[1]->service_result == SERV_REQUEST);
    
    p_ISD[0]->prm.AES_CMAC.key_group           = test_cycle[0].key_group;
    p_ISD[0]->prm.AES_CMAC.key_id              = test_cycle[0].key_id;
    p_ISD[0]->prm.AES_CMAC.p_msg               = test_cycle[0].message;
    p_ISD[0]->prm.AES_CMAC.msg_size_in_bits    = 256;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = p_cmac[0];
    p_ISD[0]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle[0].cmac_size;
    p_ISD[0]->prm.AES_CMAC.job_slice           = 1;
    p_ISD[0]->prm.AES_CMAC.job_cycle           = JOB_UPDATE;
    R_ICUMIF_ServiceRequest(p_ISD[0]);
    while(p_ISD[0]->service_result == SERV_REQUEST);
    
    p_ISD[2]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[2]->p_callbackfunc                   = 0;
    p_ISD[2]->job_id                           = 0;
    p_ISD[2]->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
    p_ISD[2]->prm.AES_CMAC.key_group           = test_cycle[2].key_group;
    p_ISD[2]->prm.AES_CMAC.key_id              = test_cycle[2].key_id;
    p_ISD[2]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD[2]->prm.AES_CMAC.p_msg               = test_cycle[2].message;
    p_ISD[2]->prm.AES_CMAC.msg_size_in_bits    = test_cycle[2].mes_size;
    p_ISD[2]->prm.AES_CMAC.p_cmac              = p_cmac[2];
    p_ISD[2]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle[2].cmac_size;
    p_ISD[2]->prm.AES_CMAC.job_slice           = 2;
    p_ISD[2]->prm.AES_CMAC.job_cycle           = test_cycle[2].job_cycle;
    R_ICUMIF_ServiceRequest(p_ISD[2]);
    while(p_ISD[2]->service_result == SERV_REQUEST);
    while(!R_ICUMIF_IsServiceCompleted(p_ISD[0]))
    {
        HALT();
        NOPS(10);
    }
    
    p_ISD[0]->prm.AES_CMAC.key_group           = test_cycle[0].key_group;
    p_ISD[0]->prm.AES_CMAC.key_id              = test_cycle[0].key_id;
    p_ISD[0]->prm.AES_CMAC.p_msg               = &test_cycle[0].message[8];
    p_ISD[0]->prm.AES_CMAC.msg_size_in_bits    = 128;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = p_cmac[0];
    p_ISD[0]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle[0].cmac_size;
    p_ISD[0]->prm.AES_CMAC.job_slice           = test_cycle[0].job_slice;
    p_ISD[0]->prm.AES_CMAC.job_cycle           = JOB_UPDATE;
    R_ICUMIF_ServiceRequest(p_ISD[0]);
    while(p_ISD[0]->service_result == SERV_REQUEST);
    while(!R_ICUMIF_IsServiceCompleted(p_ISD[0]))
    {
        HALT();
        NOPS(10);
    }
    
    p_ISD[0]->prm.AES_CMAC.key_group           = test_cycle[0].key_group;
    p_ISD[0]->prm.AES_CMAC.key_id              = test_cycle[0].key_id;
    p_ISD[0]->prm.AES_CMAC.p_msg               = &test_cycle[0].message[12];
    p_ISD[0]->prm.AES_CMAC.msg_size_in_bits    = 128;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = p_cmac[0];
    p_ISD[0]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle[0].cmac_size;
    p_ISD[0]->prm.AES_CMAC.job_slice           = 2;
    p_ISD[0]->prm.AES_CMAC.job_cycle           = JOB_FINISH;
    R_ICUMIF_ServiceRequest(p_ISD[0]);
    while(p_ISD[0]->service_result == SERV_REQUEST);
    // wait for completion of last service (should be the last one to complete)
    while(!R_ICUMIF_IsServiceCompleted(p_ISD[0]))
    {
        HALT();
        NOPS(10);
    }
    while(!R_ICUMIF_IsServiceCompleted(p_ISD[1]))
    {
        HALT();
        NOPS(10);
    }
    while(!R_ICUMIF_IsServiceCompleted(p_ISD[2]))
    {
        HALT();
        NOPS(10);
    }

    // in case of error, stop here
    if( p_ISD[0]->service_result == SERV_OK 
        && ( 0 == memcmp( p_cmac[0],test_cycle[0].cmac,  16 )))
    {
        printf("Cipher Test Result : test1 %d SERV_OK\n", i + 1 ); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test1 %d not SERV_OK\n", i + 1); FLUSH();
    }
    // in case of error, stop here
    if( p_ISD[1]->service_result == SERV_OK 
        && ( 0 == memcmp( p_cmac[1],test_cycle[1].cmac,  16 )))
    {
        printf("Cipher Test Result : test2 %d SERV_OK\n", i + 1 ); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test2 %d not SERV_OK\n", i + 1); FLUSH();
    }
    // in case of error, stop here
    if( p_ISD[2]->service_result == SERV_OK 
        && ( 0 == memcmp( p_cmac[2],test_cycle[2].cmac,  16 )))
    {
        printf("Cipher Test Result : test3 %d SERV_OK\n", i + 1 ); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test3 %d not SERV_OK\n", i + 1); FLUSH();
    }

}
