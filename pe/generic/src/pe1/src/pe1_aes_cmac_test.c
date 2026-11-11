/******************************************************************************/
/* Component Name   Testing the ICUM                                          */
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
#include "renesas_types.h"

#include "r_icumif_api.h"

#include "pe1.h"
#include "shared.h"
#include "vrun.api.h"

// ********************************************************************************
// Local constants
// ********************************************************************************

#define ALIGNVAR(n) __attribute__ ((aligned(n)))

typedef struct
{
    uint32_t             ALIGNVAR(256) message[16];
    uint32_t             ALIGNVAR(256) cmac[4];
    r_key_group_t        key_group;
    uint8_t              key_id;
    uint32_t             mes_size;
    uint8_t              cmac_size;
    r_job_slice_t        job_slice;
    r_job_cycle_t        job_cycle;
    uint8_t              padding[24];
} type_test_vector;

typedef struct
{
    uint8_t              ALIGNVAR(256) message[64];
    uint8_t              ALIGNVAR(256) cmac[16];
    r_key_group_t        key_group;
    uint8_t              key_id;
    uint32_t             mes_size;
    uint8_t              cmac_size;
    r_job_slice_t        job_slice;
    r_job_cycle_t        job_cycle;
    uint8_t              padding[24];
} type_test_vector_8bit;


static const type_test_vector ALIGNVAR(32) test_cycle[] =
{
    // 512bit message, 128bit cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cmac
        { 0xbfbef051, 0x929d3b7e, 0x177449fc, 0xfe3c3679 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               512,               128,            2,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    // 320bit message, 128bit cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3 },
        // cmac
        { 0x4767a6df, 0x30e69ade, 0x6132ca30, 0x27c89714 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               320,               128,            0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    // 128bit message, 128bit cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373 },
        // cmac
        { 0xb4160a07, 0x44414d6b, 0x9ddd9bf7, 0x7c284ad0 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               128,               128,            2,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
};
static const type_test_vector ALIGNVAR(32) test_cycle1[] =
{
    // 512bit message, 128bit cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cmac
        { 0xbfbef051, 0x929d3b7e, 0x177449fc, 0xfe3c3679 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               0,                 128,            0,              JOB_INIT,
        // padding
        { 0x00 },
    },
    // 320bit message, 128bit cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3 },
        // cmac
        { 0x4767a6df, 0x30e69ade, 0x6132ca30, 0x27c89714 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               320,               128,            2,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    // 512bit message, 128bit cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45 },
        // cmac
        { 0xbfbef051, 0x929d3b7e, 0x177449fc, 0xfe3c3679 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               256,               128,            1,              JOB_UPDATE,
        // padding
        { 0x00 },
    },
    // 128bit message, 128bit cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373 },
        // cmac
        { 0xb4160a07, 0x44414d6b, 0x9ddd9bf7, 0x7c284ad0 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               128,               128,            0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    // 512bit message, 128bit cmac
    {
        // message
        { 0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a },
        // cmac
        { 0xbfbef051, 0x929d3b7e, 0x177449fc, 0xfe3c3679 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               128,               128,            0,              JOB_UPDATE,
        // padding
        { 0x00 },
    },
    // 512bit message, 128bit cmac
    {
        // message
        { 0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cmac
        { 0xbfbef051, 0x929d3b7e, 0x177449fc, 0xfe3c3679 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               128,               128,            2,              JOB_FINISH,
        // padding
        { 0x00 },
    },
};
static const type_test_vector ALIGNVAR(32) test_cycle2[] =
{
    // 512bit message, 128bit cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cmac
        { 0xbfbef051, 0x929d3b7e, 0x177449fc, 0xfe3c3679 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               0,                 128,            0,              JOB_INIT,
        // padding
        { 0x00 },
    },
    // 320bit message, 128bit cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3 },
        // cmac
        { 0x4767a6df, 0x30e69ade, 0x6132ca30, 0x27c89714 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               320,               128,            2,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    // 512bit message, 128bit cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45 },
        // cmac
        { 0xbfbef051, 0x929d3b7e, 0x177449fc, 0xfe3c3679 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               256,               128,            0,              JOB_UPDATE,
        // padding
        { 0x00 },
    },
    // 128bit message, 128bit cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373 },
        // cmac
        { 0xb4160a07, 0x44414d6b, 0x9ddd9bf7, 0x7c284ad0 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               128,               128,            0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    // 512bit message, 128bit cmac
    {
        // message
        { 0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a },
        // cmac
        { 0xbfbef051, 0x929d3b7e, 0x177449fc, 0xfe3c3679 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               128,               128,            0,              JOB_UPDATE,
        // padding
        { 0x00 },
    },
    // 512bit message, 128bit cmac
    {
        // message
        { 0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cmac
        { 0xbfbef051, 0x929d3b7e, 0x177449fc, 0xfe3c3679 },
        // key_group        // key_id        // mes_size        // cmac_size    // job slice    // job cycle
        KEY_GRP_AES_RAM,        2,               128,               128,            2,              JOB_FINISH,
        // padding
        { 0x00 },
    },
};


static const type_test_vector ALIGNVAR(32) test_vector_simplt[] =
{
    // 64byte message, 16byte cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3, 0x19c1fbe5, 0xef520a1a,
          0x45249ff6, 0x179b4fdf, 0x7b412bad, 0x10376ce6 },
        // cmac
        { 0xbfbef051, 0x929d3b7e, 0x177449fc, 0xfe3c3679 },
        // key_group        // key_id       // mes_size     // cmac_size        // job slice        // job cycle
        KEY_GRP_AES_RAM,    1,              512,            128,                0,                  JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    // 40byte message, 16byte cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373,
          0x578a2dae, 0x9cac031e, 0xac6fb79e, 0x518eaf45,
          0x461cc830, 0x11e45ca3 },
        // cmac
        { 0x4767a6df, 0x30e69ade, 0x6132ca30, 0x27c89714 },
        // key_group        // key_id       // mes_size     // cmac_size        // job slice        // job cycle
        KEY_GRP_AES_RAM,    1,              320,            128,                0,                  JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    // 16byte message, 16byte cmac
    {
        // message
        { 0xe2bec16b, 0x969f402e, 0x117e3de9, 0x2a179373 },
        // cmac
        { 0xb4160a07, 0x44414d6b, 0x9ddd9bf7, 0x7c284ad0 },
        // key_group        // key_id       // mes_size     // cmac_size        // job slice        // job cycle
        KEY_GRP_AES_RAM,    2,              128,            128,                0,                  JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    // 0byte message, 16byte cmac
    {
        // message
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        // cmac
        { 0x29691dbb, 0x283759e9, 0x127da37f, 0x4667759b },
        // key_group        // key_id       // mes_size     // cmac_size        // job slice        // job cycle
        KEY_GRP_AES_RAM,    3,              0,              128,                0,                  JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
};

static const type_test_vector_8bit ALIGNVAR(32) test_vector_simplt_8bit[] =
{    // 64byte message, 16byte cmac
    {
        // message
        { 0x6b,0xc1,0xbe,0xe2, 0x2e,0x40,0x9f,0x96, 0xe9,0x3d,0x7e,0x11, 0x73,0x93,0x17,0x2a,
          0xae,0x2d,0x8a,0x57, 0x1e,0x03,0xac,0x9c, 0x9e,0xb7,0x6f,0xac, 0x45,0xaf,0x8e,0x51,
          0x30,0xc8,0x1c,0x46, 0xa3,0x5c,0xe4,0x11, 0xe5,0xfb,0xc1,0x19, 0x1a,0x0a,0x52,0xef,
          0xf6,0x9f,0x24,0x45, 0xdf,0x4f,0x9b,0x17, 0xad,0x2b,0x41,0x7b, 0xe6,0x6c,0x37,0x10 },
        // cmac
        { 0x51,0xf0,0xbe,0xbf, 0x7e,0x3b,0x9d,0x92, 0xfc,0x49,0x74,0x17, 0x79,0x36,0x3c,0xfe },
        // key_group        // key_id        // mes_size        // cmac_size        // job slice    // job cycle
        KEY_GRP_AES,        3,               512,               128,                0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    // 40byte message, 16byte cmac
    {
        // message
        { 0x6b,0xc1,0xbe,0xe2, 0x2e,0x40,0x9f,0x96, 0xe9,0x3d,0x7e,0x11, 0x73,0x93,0x17,0x2a,
          0xae,0x2d,0x8a,0x57, 0x1e,0x03,0xac,0x9c, 0x9e,0xb7,0x6f,0xac, 0x45,0xaf,0x8e,0x51,
          0x30,0xc8,0x1c,0x46, 0xa3,0x5c,0xe4,0x11 },
        // cmac
        { 0xdf,0xa6,0x67,0x47, 0xde,0x9a,0xe6,0x30, 0x30,0xca,0x32,0x61, 0x14,0x97,0xc8,0x27 },
        // key_group        // key_id        // mes_size        // cmac_size        // job slice    // job cycle
        KEY_GRP_AES,        3,               320,               128,                0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    // 16byte message, 16byte cmac
    {
        // message
        { 0x6b,0xc1,0xbe,0xe2, 0x2e,0x40,0x9f,0x96, 0xe9,0x3d,0x7e,0x11, 0x73,0x93,0x17,0x2a },
        // cmac
        { 0x07,0x0a,0x16,0xb4, 0x6b,0x4d,0x41,0x44, 0xf7,0x9b,0xdd,0x9d, 0xd0,0x4a,0x28,0x7c },
        // key_group        // key_id        // mes_size        // cmac_size        // job slice    // job cycle
        KEY_GRP_AES,        3,               128,               128,                0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
    // 0byte message, 16byte cmac
    {
        // message
        { 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00 },
        // cmac
        { 0xbb,0x1d,0x69,0x29, 0xe9,0x59,0x37,0x28, 0x7f,0xa3,0x7d,0x12, 0x9b,0x75,0x67,0x46 },
        // key_group        // key_id        // mes_size        // cmac_size        // job slice    // job cycle
        KEY_GRP_AES,        3,               0,                 128,                0,              JOB_ALL_AT_ONCE,
        // padding
        { 0x00 },
    },
};


/*--------------------------------------------------------------------------*/
/* Include Files                                                            */
/*--------------------------------------------------------------------------*/
void icum_aes_print_CMAC(uint32_t *p_uiCmacOut);

/*===========================================================================
  ICUMXA test function
  [PUBLIC]
  ===========================================================================*/
void icum_aes_cmac_test(void)
{
    r_icumif_isd_t *p_ISD;

    int n = 4;          /* Number of registrations */
    int i;
    int size;

    /* initialize the global ICUMXA service header */
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;

    printf("\nCMAC Generation Start\n\n"); FLUSH();
    for(i = 0; i < n; i++)
    {
        memset(p_ISD, 0, sizeof(r_icumif_isd_t));
        p_ISD->service_id                       = SERVICE_01_AES_CMAC;
        p_ISD->p_callbackfunc                   = 0;
        p_ISD->job_id                           = 0;
        p_ISD->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
        p_ISD->prm.AES_CMAC.key_group           = test_vector_simplt[i].key_group;
        p_ISD->prm.AES_CMAC.key_id              = test_vector_simplt[i].key_id;
        p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
        p_ISD->prm.AES_CMAC.p_msg               = test_vector_simplt[i].message;
        p_ISD->prm.AES_CMAC.msg_size_in_bits    = test_vector_simplt[i].mes_size;
        p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
        p_ISD->prm.AES_CMAC.cmac_size_in_bits   = test_vector_simplt[i].cmac_size;
        p_ISD->prm.AES_CMAC.job_slice           = test_vector_simplt[i].job_slice;
        p_ISD->prm.AES_CMAC.job_cycle           = test_vector_simplt[i].job_cycle;

        printf("Test No. %d \n",(i+1));
        /* trigger the security service */
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        /* fetch execution time in vrun_chrono_xxx variables */
        vrun_chrono_gettimestamp();
        printf("\rtiming : %3dus", vrun_chrono_microsec);
        if(vrun_chrono_microsec >= 150) printf(" (!!)");
        NL();

        size = test_vector_simplt[i].cmac_size / 8;
        if( test_vector_simplt[i].cmac_size % 8 != 0 )
        {
            size++;
        }
        /* in case of error, stop here */
        if((p_ISD->service_result == SERV_OK) &&
           (0 == memcmp(BIG_BUFFER[0], test_vector_simplt[i].cmac, size)))
        {
            printf("Cipher Test Result : test %d SERV_OK\n", i + 1 ); FLUSH();
        }
        else
        {
            printf("Cipher Test Result : test %d not SERV_OK\n", i + 1); FLUSH();
        }
    }
    for(i = 0; i < n; i++)
    {
        memset(p_ISD, 0, sizeof(r_icumif_isd_t));
        p_ISD->service_id                       = SERVICE_01_AES_CMAC;
        p_ISD->p_callbackfunc                   = 0;
        p_ISD->job_id                           = 0;
        p_ISD->service_priority                 = R_ICUMIF_SERV_PRIORITY_HIGHEST;
        p_ISD->prm.AES_CMAC.key_group           = test_vector_simplt[i].key_group;
        p_ISD->prm.AES_CMAC.key_id              = test_vector_simplt[i].key_id;
        p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
        p_ISD->prm.AES_CMAC.p_msg               = test_vector_simplt[i].message;
        p_ISD->prm.AES_CMAC.msg_size_in_bits    = test_vector_simplt[i].mes_size;
        p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
        p_ISD->prm.AES_CMAC.cmac_size_in_bits   = test_vector_simplt[i].cmac_size;
        p_ISD->prm.AES_CMAC.job_slice           = test_vector_simplt[i].job_slice;
        p_ISD->prm.AES_CMAC.job_cycle           = test_vector_simplt[i].job_cycle;

        printf("Test No. %d \n",(i+1));
        /* trigger the security service */
        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        /* fetch execution time in vrun_chrono_xxx variables */
        vrun_chrono_gettimestamp();
        printf("\rtiming : %3dus", vrun_chrono_microsec);
        if(vrun_chrono_microsec >= 150) printf(" (!!)");
        NL();

        size = test_vector_simplt[i].cmac_size / 8;
        if( test_vector_simplt[i].cmac_size % 8 != 0 )
        {
            size++;
        }
        /* in case of error, stop here */
        if((p_ISD->service_result == SERV_OK) &&
           (0 == memcmp(BIG_BUFFER[0], test_vector_simplt[i].cmac, size)))
        {
            printf("Cipher Test Result : test %d SERV_OK\n", i + 1 ); FLUSH();
        }
        else
        {
            printf("Cipher Test Result : test %d not SERV_OK\n", i + 1); FLUSH();
        }
    }
    for(i = 0; i < n; i++)
    {
        memset(p_ISD, 0, sizeof(r_icumif_isd_t));
        p_ISD->service_id                       = SERVICE_01_AES_CMAC;
        p_ISD->p_callbackfunc                   = 0;
        p_ISD->job_id                           = 0;
        p_ISD->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
        p_ISD->prm.AES_CMAC.key_group           = test_vector_simplt_8bit[i].key_group;
        p_ISD->prm.AES_CMAC.key_id              = test_vector_simplt_8bit[i].key_id;
        p_ISD->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
        p_ISD->prm.AES_CMAC.p_msg               = (uint32_t *)test_vector_simplt_8bit[i].message;
        p_ISD->prm.AES_CMAC.msg_size_in_bits    = test_vector_simplt_8bit[i].mes_size;
        p_ISD->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
        p_ISD->prm.AES_CMAC.cmac_size_in_bits   = test_vector_simplt_8bit[i].cmac_size;
        p_ISD->prm.AES_CMAC.job_slice           = test_vector_simplt_8bit[i].job_slice;
        p_ISD->prm.AES_CMAC.job_cycle           = test_vector_simplt_8bit[i].job_cycle;

        printf("Test No. %d \n",(i+1));
        // trigger the security service
        vrun_chrono_start();
        // trigger the security service
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();
        // fetch execution time in vrun_chrono_xxx variables
        vrun_chrono_gettimestamp();
        printf("\rtiming : %3dus", vrun_chrono_microsec);
        if(vrun_chrono_microsec >= 150) printf(" (!!)");
        NL();

        size = test_vector_simplt_8bit[i].cmac_size / 8;
        if( test_vector_simplt_8bit[i].cmac_size % 8 != 0 )
        {
            size++;
        }
        // in case of error, stop here
        if( p_ISD->service_result == SERV_OK 
            && ( 0 == memcmp( BIG_BUFFER[0],test_vector_simplt_8bit[i].cmac,  size )))
        {
            printf("Cipher Test Result : test %d SERV_OK\n", i + 1 ); FLUSH();

        }
        else
        {
            printf("Cipher Test Result : test %d not SERV_OK\n", i + 1); FLUSH();
        }
    }
}

/*===========================================================================
  ICUMXA test function CMAC + Job Cycle
  [PUBLIC]
  ===========================================================================*/
void icum_aes_cmac_test_JobCycle(void)
{
    int i;
    r_icumif_isd_t *p_ISD[3];
    r_icumif_job_id_t job_id = 0;
    uint32_t uiStartAddr;
    uint32_t uiNextAddr;

    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the isd_buffer

    uiStartAddr = (uint32_t)ISD_BUFFER;
    uiNextAddr = ((sizeof(r_icumif_isd_t) / 32) + 1) * 32;

    p_ISD[0]  = (r_icumif_isd_t *)uiStartAddr;
    p_ISD[1]  = (r_icumif_isd_t *)(uiStartAddr + uiNextAddr);
    p_ISD[2]  = (r_icumif_isd_t *)(uiStartAddr + (uiNextAddr * 2));

    printf("\nCMAC job slice \n"); FLUSH();
    for( i = 0; i < 3; i++ )
    {
        p_ISD[i]->service_id                       = SERVICE_01_AES_CMAC;
        p_ISD[i]->p_callbackfunc                   = 0;
        p_ISD[i]->job_id                           = 0;
        p_ISD[i]->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
        p_ISD[i]->prm.AES_CMAC.key_group           = test_cycle[i].key_group;
        p_ISD[i]->prm.AES_CMAC.key_id              = test_cycle[i].key_id;
#ifdef TEST_VERIFICATION
        p_ISD[i]->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
        p_ISD[i]->prm.AES_CMAC.p_cmac              = (uint32_t *)test_cycle[i].cmac;
#else
        p_ISD[i]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
        p_ISD[i]->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[i];
#endif
        p_ISD[i]->prm.AES_CMAC.p_msg               = test_cycle[i].message;
        p_ISD[i]->prm.AES_CMAC.msg_size_in_bits    = test_cycle[i].mes_size;
        p_ISD[i]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle[i].cmac_size;
        p_ISD[i]->prm.AES_CMAC.job_slice           = test_cycle[i].job_slice;

        /* Flush of cache area of ISD service parameters and buffer */
        FlushDCache_param_and_buff(p_ISD[i]);

        R_ICUMIF_ServiceRequest(p_ISD[i]);
    }

    for( i = 0 ; i < 3 ; i++ )
    {
        // wait for completion of last service (should be the last one to complete)
        while(!R_ICUMIF_IsServiceCompleted(p_ISD[i]))
        {
            NOPS(10);
        }

        /* Invalidate of cache area of ISD service parameters and buffer */
#ifdef TEST_VERIFICATION
        if (VERIFICATION_PASS == p_ISD[i]->prm.AES_CMAC.verification_result)
#else
        InvaliDCache_param_and_buff(p_ISD[i]);

        // in case of error, stop here
        if( p_ISD[i]->service_result == SERV_OK 
            && ( 0 == memcmp( BIG_BUFFER[i], test_cycle[i].cmac,  16 )))
#endif
        {
            printf("Cipher Test Result : test %d SERV_OK\n", i + 1 ); FLUSH();

        }
        else
        {
            printf("Cipher Test Result : test %d not SERV_OK\n", i + 1); FLUSH();
            printf("service_result = %x\n", p_ISD[i]->service_result); FLUSH();
        }
    }

/******************************************************************/
/*****************************/
/* test_cycle1[0](Init)      */
/*****************************/

    printf("\nCMAC job cycle + job slice \n"); FLUSH();
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the isd_buffer
    p_ISD[0]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[0]->p_callbackfunc                   = 0;
    p_ISD[0]->job_id                           = 0;
    p_ISD[0]->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
    p_ISD[0]->prm.AES_CMAC.key_group           = test_cycle1[0].key_group;
    p_ISD[0]->prm.AES_CMAC.key_id              = test_cycle1[0].key_id;
#ifdef TEST_VERIFICATION
    p_ISD[0]->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = (uint32_t *)test_cycle[0].cmac;
#else
    p_ISD[0]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
#endif
    p_ISD[0]->prm.AES_CMAC.p_msg               = test_cycle1[0].message;
    p_ISD[0]->prm.AES_CMAC.msg_size_in_bits    = test_cycle1[0].mes_size;
    p_ISD[0]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle1[0].cmac_size;
    p_ISD[0]->prm.AES_CMAC.job_slice           = test_cycle1[0].job_slice;
    p_ISD[0]->prm.AES_CMAC.job_cycle           = test_cycle1[0].job_cycle;

    trigger_service_and_wait(p_ISD[0]);

    job_id = p_ISD[0]->job_id;

/*****************************/
/* test_cycle1[1](AllAtOnce) */
/*****************************/
    p_ISD[1]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[1]->p_callbackfunc                   = 0;
    p_ISD[1]->job_id                           = 0;
    p_ISD[1]->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
    p_ISD[1]->prm.AES_CMAC.key_group           = test_cycle1[1].key_group;
    p_ISD[1]->prm.AES_CMAC.key_id              = test_cycle1[1].key_id;
#ifdef TEST_VERIFICATION
    p_ISD[1]->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
    p_ISD[1]->prm.AES_CMAC.p_cmac              = (uint32_t *)test_cycle[1].cmac;
#else
    p_ISD[1]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD[1]->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[1];
#endif
    p_ISD[1]->prm.AES_CMAC.p_msg               = test_cycle1[1].message;
    p_ISD[1]->prm.AES_CMAC.msg_size_in_bits    = test_cycle1[1].mes_size;
    p_ISD[1]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle1[1].cmac_size;
    p_ISD[1]->prm.AES_CMAC.job_slice           = test_cycle1[1].job_slice;
    p_ISD[1]->prm.AES_CMAC.job_cycle           = test_cycle1[1].job_cycle;

    trigger_service_and_wait(p_ISD[1]);

/*****************************/
/* test_cycle1[2](Update)    */
/*****************************/


    p_ISD[0]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[0]->p_callbackfunc                   = 0;
    p_ISD[0]->job_id                           = job_id;
    p_ISD[0]->prm.AES_CMAC.key_group           = test_cycle1[2].key_group;
    p_ISD[0]->prm.AES_CMAC.key_id              = test_cycle1[2].key_id;
    p_ISD[0]->prm.AES_CMAC.p_msg               = test_cycle1[2].message;
    p_ISD[0]->prm.AES_CMAC.msg_size_in_bits    = test_cycle1[2].mes_size;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
    p_ISD[0]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle1[2].cmac_size;
    p_ISD[0]->prm.AES_CMAC.job_slice           = test_cycle1[2].job_slice;
    p_ISD[0]->prm.AES_CMAC.job_cycle           = test_cycle1[2].job_cycle;

    trigger_service_and_wait(p_ISD[0]);

    job_id = p_ISD[0]->job_id;

/*****************************/
/* test_cycle1[3](AllAtOnce) */
/*****************************/
    p_ISD[2]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[2]->p_callbackfunc                   = 0;
    p_ISD[2]->job_id                           = 0;
    p_ISD[2]->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
    p_ISD[2]->prm.AES_CMAC.key_group           = test_cycle1[3].key_group;
    p_ISD[2]->prm.AES_CMAC.key_id              = test_cycle1[3].key_id;
#ifdef TEST_VERIFICATION
    p_ISD[2]->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
    p_ISD[2]->prm.AES_CMAC.p_cmac              = (uint32_t *)test_cycle[2].cmac;
#else
    p_ISD[2]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD[2]->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[2];
#endif
    p_ISD[2]->prm.AES_CMAC.p_msg               = test_cycle1[3].message;
    p_ISD[2]->prm.AES_CMAC.msg_size_in_bits    = test_cycle1[3].mes_size;
    p_ISD[2]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle1[3].cmac_size;
    p_ISD[2]->prm.AES_CMAC.job_slice           = test_cycle1[3].job_slice;
    p_ISD[2]->prm.AES_CMAC.job_cycle           = test_cycle1[3].job_cycle;

    trigger_service_and_wait(p_ISD[2]);

/*****************************/
/* test_cycle1[4](Update)    */
/*****************************/

    p_ISD[0]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[0]->p_callbackfunc                   = 0;
    p_ISD[0]->job_id                           = job_id;
    p_ISD[0]->prm.AES_CMAC.key_group           = test_cycle1[4].key_group;
    p_ISD[0]->prm.AES_CMAC.key_id              = test_cycle1[4].key_id;
    p_ISD[0]->prm.AES_CMAC.p_msg               = test_cycle1[4].message;
    p_ISD[0]->prm.AES_CMAC.msg_size_in_bits    = test_cycle1[4].mes_size;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
    p_ISD[0]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle1[4].cmac_size;
    p_ISD[0]->prm.AES_CMAC.job_slice           = test_cycle1[4].job_slice;
    p_ISD[0]->prm.AES_CMAC.job_cycle           = test_cycle1[4].job_cycle;

    trigger_service_and_wait(p_ISD[0]);

    job_id = p_ISD[0]->job_id;

/*****************************/
/* test_cycle1[5](Finish)   */
/*****************************/

    p_ISD[0]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[0]->p_callbackfunc                   = 0;
    p_ISD[0]->job_id                           = job_id;
    p_ISD[0]->prm.AES_CMAC.key_group           = test_cycle1[5].key_group;
    p_ISD[0]->prm.AES_CMAC.key_id              = test_cycle1[5].key_id;
    p_ISD[0]->prm.AES_CMAC.p_msg               = test_cycle1[5].message;
    p_ISD[0]->prm.AES_CMAC.msg_size_in_bits    = test_cycle1[5].mes_size;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
    p_ISD[0]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle1[5].cmac_size;
    p_ISD[0]->prm.AES_CMAC.job_slice           = test_cycle1[5].job_slice;
    p_ISD[0]->prm.AES_CMAC.job_cycle           = test_cycle1[5].job_cycle;

    trigger_service_and_wait(p_ISD[0]);

    // in case of error, stop here
#ifdef TEST_VERIFICATION
    if (VERIFICATION_PASS == p_ISD[0]->prm.AES_CMAC.verification_result)
#else

    if( p_ISD[0]->service_result == SERV_OK 
        && ( 0 == memcmp( BIG_BUFFER[0], test_cycle1[5].cmac,  16 )))
#endif
    {
        printf("Cipher Test Result : test1 SERV_OK\n"); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test1 not SERV_OK\n"); FLUSH();
        printf("service_result = %x\n", p_ISD[0]->service_result); FLUSH();
    }

    // in case of error, stop here
#ifdef TEST_VERIFICATION
    if (VERIFICATION_PASS == p_ISD[1]->prm.AES_CMAC.verification_result)
#else

    if( p_ISD[1]->service_result == SERV_OK 
        && ( 0 == memcmp( BIG_BUFFER[1],test_cycle1[1].cmac,  16 )))
#endif
    {
        printf("Cipher Test Result : test2 SERV_OK\n"); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test2 not SERV_OK\n"); FLUSH();
        printf("service_result = %x\n", p_ISD[1]->service_result); FLUSH();
    }

    // in case of error, stop here
#ifdef TEST_VERIFICATION
    if (VERIFICATION_PASS == p_ISD[2]->prm.AES_CMAC.verification_result)
#else

    if( p_ISD[2]->service_result == SERV_OK 
        && ( 0 == memcmp( BIG_BUFFER[2],test_cycle1[3].cmac,  16 )))
#endif
    {
        printf("Cipher Test Result : test3 SERV_OK\n"); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test3 not SERV_OK\n"); FLUSH();
        printf("service_result = %x\n", p_ISD[2]->service_result); FLUSH();
    }


/******************************************************************/
/*****************************/
/* test_cycle2[0](Init)      */
/*****************************/
    printf("\nCMAC Update + No_slice \n"); FLUSH();
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);  // clear the isd_buffer
    p_ISD[0]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[0]->p_callbackfunc                   = 0;
    p_ISD[0]->job_id                           = 0;
    p_ISD[0]->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
    p_ISD[0]->prm.AES_CMAC.key_group           = test_cycle2[0].key_group;
    p_ISD[0]->prm.AES_CMAC.key_id              = test_cycle2[0].key_id;
#ifdef TEST_VERIFICATION
    p_ISD[0]->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = (uint32_t *)test_cycle[0].cmac;
#else
    p_ISD[0]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
#endif
    p_ISD[0]->prm.AES_CMAC.p_msg               = test_cycle2[0].message;
    p_ISD[0]->prm.AES_CMAC.msg_size_in_bits    = test_cycle2[0].mes_size;
    p_ISD[0]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle2[0].cmac_size;
    p_ISD[0]->prm.AES_CMAC.job_slice           = test_cycle2[0].job_slice;
    p_ISD[0]->prm.AES_CMAC.job_cycle           = test_cycle2[0].job_cycle;

    trigger_service_and_wait(p_ISD[0]);

    job_id = p_ISD[0]->job_id;

/*****************************/
/* test_cycle2[1](AllAtOnce) */
/*****************************/
    p_ISD[1]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[1]->p_callbackfunc                   = 0;
    p_ISD[1]->job_id                           = 0;
    p_ISD[1]->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
    p_ISD[1]->prm.AES_CMAC.key_group           = test_cycle2[1].key_group;
    p_ISD[1]->prm.AES_CMAC.key_id              = test_cycle2[1].key_id;
#ifdef TEST_VERIFICATION
    p_ISD[1]->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
    p_ISD[1]->prm.AES_CMAC.p_cmac              = (uint32_t *)test_cycle[1].cmac;
#else
    p_ISD[1]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD[1]->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[1];
#endif
    p_ISD[1]->prm.AES_CMAC.p_msg               = test_cycle2[1].message;
    p_ISD[1]->prm.AES_CMAC.msg_size_in_bits    = test_cycle2[1].mes_size;
    p_ISD[1]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle2[1].cmac_size;
    p_ISD[1]->prm.AES_CMAC.job_slice           = test_cycle2[1].job_slice;
    p_ISD[1]->prm.AES_CMAC.job_cycle           = test_cycle2[1].job_cycle;

    trigger_service_and_wait(p_ISD[1]);

/*****************************/
/* test_cycle2[2](Update)    */
/*****************************/
    p_ISD[0]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[0]->p_callbackfunc                   = 0;
    p_ISD[0]->job_id                           = job_id;
    p_ISD[0]->prm.AES_CMAC.key_group           = test_cycle2[2].key_group;
    p_ISD[0]->prm.AES_CMAC.key_id              = test_cycle2[2].key_id;
    p_ISD[0]->prm.AES_CMAC.p_msg               = test_cycle2[2].message;
    p_ISD[0]->prm.AES_CMAC.msg_size_in_bits    = test_cycle2[2].mes_size;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
    p_ISD[0]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle2[2].cmac_size;
    p_ISD[0]->prm.AES_CMAC.job_slice           = test_cycle2[2].job_slice;
    p_ISD[0]->prm.AES_CMAC.job_cycle           = test_cycle2[2].job_cycle;

    trigger_service_and_wait(p_ISD[0]);

    job_id = p_ISD[0]->job_id;

/*****************************/
/* test_cycle2[3](AllAtOnce) */
/*****************************/
    p_ISD[2]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[2]->p_callbackfunc                   = 0;
    p_ISD[2]->job_id                           = 0;
    p_ISD[2]->service_priority                 = R_ICUMIF_SERV_PRIORITY_NORMAL;
    p_ISD[2]->prm.AES_CMAC.key_group           = test_cycle2[3].key_group;
    p_ISD[2]->prm.AES_CMAC.key_id              = test_cycle2[3].key_id;
#ifdef TEST_VERIFICATION
    p_ISD[2]->prm.AES_CMAC.cmac_operation      = CMAC_VERIFICATION;
    p_ISD[2]->prm.AES_CMAC.p_cmac              = (uint32_t *)test_cycle[2].cmac;
#else
    p_ISD[2]->prm.AES_CMAC.cmac_operation      = CMAC_GENERATION;
    p_ISD[2]->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[2];
#endif
    p_ISD[2]->prm.AES_CMAC.p_msg               = test_cycle2[3].message;
    p_ISD[2]->prm.AES_CMAC.msg_size_in_bits    = test_cycle2[3].mes_size;
    p_ISD[2]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle2[3].cmac_size;
    p_ISD[2]->prm.AES_CMAC.job_slice           = test_cycle2[3].job_slice;
    p_ISD[2]->prm.AES_CMAC.job_cycle           = test_cycle2[3].job_cycle;

    trigger_service_and_wait(p_ISD[2]);

/*****************************/
/* test_cycle2[4](Update)    */
/*****************************/
    p_ISD[0]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[0]->p_callbackfunc                   = 0;
    p_ISD[0]->job_id                           = job_id;
    p_ISD[0]->prm.AES_CMAC.key_group           = test_cycle2[4].key_group;
    p_ISD[0]->prm.AES_CMAC.key_id              = test_cycle2[4].key_id;
    p_ISD[0]->prm.AES_CMAC.p_msg               = test_cycle2[4].message;
    p_ISD[0]->prm.AES_CMAC.msg_size_in_bits    = test_cycle2[4].mes_size;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
    p_ISD[0]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle2[4].cmac_size;
    p_ISD[0]->prm.AES_CMAC.job_slice           = test_cycle2[4].job_slice;
    p_ISD[0]->prm.AES_CMAC.job_cycle           = test_cycle2[4].job_cycle;

    trigger_service_and_wait(p_ISD[0]);

    job_id = p_ISD[0]->job_id;

/*****************************/
/* test_cycle2[5](Finish)   */
/*****************************/

    p_ISD[0]->service_id                       = SERVICE_01_AES_CMAC;
    p_ISD[0]->p_callbackfunc                   = 0;
    p_ISD[0]->job_id                           = job_id;
    p_ISD[0]->prm.AES_CMAC.key_group           = test_cycle2[5].key_group;
    p_ISD[0]->prm.AES_CMAC.key_id              = test_cycle2[5].key_id;
    p_ISD[0]->prm.AES_CMAC.p_msg               = test_cycle2[5].message;
    p_ISD[0]->prm.AES_CMAC.msg_size_in_bits    = test_cycle2[5].mes_size;
    p_ISD[0]->prm.AES_CMAC.p_cmac              = (uint32_t *)BIG_BUFFER[0];
    p_ISD[0]->prm.AES_CMAC.cmac_size_in_bits   = test_cycle2[5].cmac_size;
    p_ISD[0]->prm.AES_CMAC.job_slice           = test_cycle2[5].job_slice;
    p_ISD[0]->prm.AES_CMAC.job_cycle           = test_cycle2[5].job_cycle;

    trigger_service_and_wait(p_ISD[0]);

    // in case of error, stop here
#ifdef TEST_VERIFICATION
    if (VERIFICATION_PASS == p_ISD[0]->prm.AES_CMAC.verification_result)
#else

    if( p_ISD[0]->service_result == SERV_OK 
        && ( 0 == memcmp( BIG_BUFFER[0],test_cycle2[5].cmac,  16 )))
#endif
    {
        printf("Cipher Test Result : test1 SERV_OK\n" ); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test1 not SERV_OK\n"); FLUSH();
        printf("service_result = %x\n", p_ISD[0]->service_result); FLUSH();
    }

    // in case of error, stop here
#ifdef TEST_VERIFICATION
    if (VERIFICATION_PASS == p_ISD[1]->prm.AES_CMAC.verification_result)
#else

    if( p_ISD[1]->service_result == SERV_OK 
        && ( 0 == memcmp( BIG_BUFFER[1],test_cycle2[1].cmac,  16 )))
#endif
    {
        printf("Cipher Test Result : test2 SERV_OK\n"); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test2 not SERV_OK\n"); FLUSH();
        printf("service_result = %x\n", p_ISD[1]->service_result); FLUSH();
    }

    // in case of error, stop here
#ifdef TEST_VERIFICATION
    if (VERIFICATION_PASS == p_ISD[2]->prm.AES_CMAC.verification_result)
#else

    if( p_ISD[2]->service_result == SERV_OK 
        && ( 0 == memcmp( BIG_BUFFER[2],test_cycle2[3].cmac,  16 )))
#endif
    {
        printf("Cipher Test Result : test3 SERV_OK\n"); FLUSH();

    }
    else
    {
        printf("Cipher Test Result : test3 not SERV_OK\n"); FLUSH();
        printf("service_result = %x\n", p_ISD[2]->service_result); FLUSH();
    }
}

/*===========================================================================
  ICUMXA test function
  [PUBLIC]
  ===========================================================================*/
void icum_aes_print_CMAC(uint32_t *p_uiCmacOut)
{
    uint8_t *p_uiCmac;
    uint8_t uiLoop = 0;

        p_uiCmac = (uint8_t *)p_uiCmacOut;
        /* OUTPUT Data */
        for(uiLoop= 0; uiLoop<16; uiLoop++ )
        {
            printf("%02x", *p_uiCmac);
            p_uiCmac =  p_uiCmac + 1;
        }

        printf("\n");
}

