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

#include "renesas_types.h"

/*******************************************************************************
* Prototypes
*******************************************************************************/

int main(void);
void vrun_cmd_get_cpu_speed(void);

/*******************************************************************************
* Local constants
*******************************************************************************/

#pragma alignvar(8)

static const uint8_t TESTVECTOR0[16] =
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

/*******************************************************************************
* Local defines
*******************************************************************************/

/* separator between fields */
/* this is used in the XLS import wizard */
#define SEPARATOR   "-"

/* number of tests depends on the number of loops (see below) */
#define NB_TESTS    (6 * 2)

enum 
{
    CIPHER_MODE,
    DIRECTION,
    INPUT_LOCATION,
    INPUT_ALIGNMENT,
    OUTPUT_LOCATION,
    OUTPUT_ALIGNMENT,
    KEY_GROUP,
    KEY_IN_CACHE,
    JOB_SLICE,
    DATA_SIZE,
    TIMING,
    THROUGHPUT,
    MIN_MAX_TIMING,
    MAX_CONDITIONS
};

/*******************************************************************************
* Local variables
*******************************************************************************/

static r_icumif_isd_t     *p_ISD;
static cipher_direction_t  direction;
static cipher_modes_t      cipher_mode;
static r_job_slice_t       jobslice;
static r_key_group_t       key_group;
static r_key_index_t       key_id;
static uint8_t            *p_data_in;
static uint8_t            *p_data_out;
static uint32_t            data_size;
static char                test_condition[MAX_CONDITIONS][NB_TESTS][8];
static uint32_t            min;
static uint32_t            max;
static int                 i_min;
static int                 i_max;

/*******************************************************************************
* Test functions
*******************************************************************************/

static void get_sys_info(void)
{
    float freq;

    printf("***********************\n");

    vrun_cmd_get_cpu_speed();
    
    freq = 900000 / (float)((vrun_chrono_millisec * 1000) + vrun_chrono_microsec);
    printf("CPU frequency (MHz): " SEPARATOR);
    printf("%f\n", freq);
    printf("ICUM frequency (MHz): " SEPARATOR);
    printf("%f\n", (freq/2));

    printf("***********************\n\n");
    FLUSH();

    return;
}

static void init_ram_key(void)
{
    p_ISD->service_id                             = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_ISD->p_callbackfunc                         = 0;
    p_ISD->job_id                                 = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_group         = KEY_GRP_AES_RAM;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_id            = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.p_key_value       = TESTVECTOR0;
    p_ISD->prm.KEY_UPDATE_PLAIN.key_size_in_bytes = AES_BLOCK_SIZE_IN_BYTES;

    p_ISD->prm.KEY_UPDATE_PLAIN.flags.encryption         = 1;     
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.decryption         = 1;     
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_generation    = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.cmac_verification  = 0;
    p_ISD->prm.KEY_UPDATE_PLAIN.flags.disable_on_debug   = 0;

    trigger_service_and_wait(p_ISD);
}

static void update_conditions(int i)
{
    if(i >= NB_TESTS)
    {
        printf("Not enough columns in table test_condition[].\n\n");
        return;
    }
    
    switch(direction)
    {
        case CIPHER_DIR_ENCRYPTION:
            strcpy(test_condition[DIRECTION][i], SEPARATOR "ENC");
            break;
        case CIPHER_DIR_DECRYPTION:
            strcpy(test_condition[DIRECTION][i], SEPARATOR "DEC");
            break;
        default:
            break;
    }

    switch(cipher_mode)
    {
        case CIPHER_MOD_ECB:
            strcpy(test_condition[CIPHER_MODE][i], SEPARATOR "ECB");
            break;
        case CIPHER_MOD_CBC:
            strcpy(test_condition[CIPHER_MODE][i], SEPARATOR "CBC");
            break;
        case CIPHER_MOD_CTR:
            strcpy(test_condition[CIPHER_MODE][i], SEPARATOR "CTR");
            break;
        case CIPHER_MOD_CFB:
            strcpy(test_condition[CIPHER_MODE][i], SEPARATOR "CFB");
            break;
        case CIPHER_MOD_OFB:
            strcpy(test_condition[CIPHER_MODE][i], SEPARATOR "OFB");
            break;
        case CIPHER_MOD_XTS:
            strcpy(test_condition[CIPHER_MODE][i], SEPARATOR "XTS");
            break;
        default:
            break;
    }
        
    switch(key_group)
    {
        case KEY_GRP_AES_ROM:
            strcpy(test_condition[KEY_GROUP][i], SEPARATOR "ROM");
            break;
        case KEY_GRP_AES:
            strcpy(test_condition[KEY_GROUP][i], SEPARATOR "AES");
            break;
        case KEY_GRP_AES_RAM:
            strcpy(test_condition[KEY_GROUP][i], SEPARATOR "RAM");
            strcpy(test_condition[KEY_IN_CACHE][i], SEPARATOR "n/a");
            break;
        default:
            break;
    }

    if(p_data_in >= &BIG_BUFFER[0][0])
    {
        strcpy(test_condition[INPUT_LOCATION][i],   SEPARATOR "GRAM");
    }
    else
    {
        strcpy(test_condition[INPUT_LOCATION][i],   SEPARATOR "ROM");
    }
    
    strcpy(test_condition[OUTPUT_LOCATION][i],  SEPARATOR "GRAM");

    if(((uint32_t)p_data_in % 4) == 0)
    {
        strcpy(test_condition[INPUT_ALIGNMENT][i],  SEPARATOR "A");
    }
    else
    {
        strcpy(test_condition[INPUT_ALIGNMENT][i],  SEPARATOR "MA");
    }

    if(((uint32_t)p_data_out % 4) == 0)
    {
        strcpy(test_condition[OUTPUT_ALIGNMENT][i], SEPARATOR "A");
    }
    else
    {
        strcpy(test_condition[OUTPUT_ALIGNMENT][i], SEPARATOR "MA");
    }

    sprintf(test_condition[JOB_SLICE][i],       SEPARATOR "%d", jobslice);
    sprintf(test_condition[DATA_SIZE][i],       SEPARATOR "%d", data_size);
}

static void update_timings(int i)
{
    uint32_t    timing;
    float       throughput;

    if(i >= NB_TESTS)
    {
        return;
    }

    timing = vrun_chrono_millisec * 1000 + vrun_chrono_microsec;

    sprintf(test_condition[TIMING][i], SEPARATOR "%0.3f", ((float)(timing) / 1000));
    throughput  = (float)(data_size * 1000) * 1000;
    throughput /= (vrun_chrono_millisec * 1000 + vrun_chrono_microsec);
    sprintf(test_condition[THROUGHPUT][i], SEPARATOR "%0.2f", (throughput / (1024 * 1024)));

    /* check for min / max timings */
    if(timing > max)
    {
        max   = timing;
        i_max = i;
    }
    if(timing < min)
    {
        min   = timing;
        i_min = i;
    }
}

static void start_cipher(void)
{
    if ((CIPHER_MOD_CBC == cipher_mode) && (KEY_GRP_AES_ROM == key_group))
    {
        /* Non support */
        return;
    }
    if ((CIPHER_MOD_ECB == cipher_mode) && (KEY_GRP_AES_ROM == key_group))
    {
        /* Non support */
        return;
    }
    p_ISD->service_id                    = SERVICE_01_AES_CIPHER;
    p_ISD->p_callbackfunc                = 0;
    p_ISD->job_id                        = 0;
    p_ISD->prm.AES_CIPHER.direction      = direction;
    p_ISD->prm.AES_CIPHER.cipher_mode    = cipher_mode;
    p_ISD->prm.AES_CIPHER.key_group      = key_group;
    p_ISD->prm.AES_CIPHER.key_id         = key_id;
    p_ISD->prm.AES_CIPHER.p_iv           = (uint32_t *)TESTVECTOR0;
    p_ISD->prm.AES_CIPHER.p_block_in     = (uint32_t *)p_data_in;
    p_ISD->prm.AES_CIPHER.p_block_out    = (uint32_t *)p_data_out;
    p_ISD->prm.AES_CIPHER.size.nb_blocks = (data_size / AES_BLOCK_SIZE_IN_BYTES);
    p_ISD->prm.AES_CIPHER.job_slice      = jobslice;
    p_ISD->prm.AES_CIPHER.job_cycle      = JOB_ALL_AT_ONCE;
    
    if(cipher_mode == CIPHER_MOD_XTS)
    {
        p_ISD->prm.AES_CIPHER.key_group_2             = key_group;
        p_ISD->prm.AES_CIPHER.key_id_2                = key_id;
        p_ISD->prm.AES_CIPHER.size.data_size_in_bits  = (data_size * 8);
    }

    /* trigger the security service and get timing */
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    vrun_chrono_gettimestamp();
}

static void dump_timings(int n)
{
    int i;
    
    for(i = 0; i < n; i++)
    {
        strcpy(test_condition[MIN_MAX_TIMING][i], SEPARATOR);
    }
    strcpy(test_condition[MIN_MAX_TIMING][i_min], SEPARATOR "MIN");
    strcpy(test_condition[MIN_MAX_TIMING][i_max], SEPARATOR "MAX");

    /* printf("Test output:\n\n"); */
    
    printf("Block cipher mode");       for(i = 0; i < n; i++) printf((const char *)test_condition[CIPHER_MODE][i]);        NL(); FLUSH();
    printf("Direction (ENC/DEC)");     for(i = 0; i < n; i++) printf((const char *)test_condition[DIRECTION][i]);          NL(); FLUSH();
    printf("Data input: location");    for(i = 0; i < n; i++) printf((const char *)test_condition[INPUT_LOCATION][i]);     NL(); FLUSH();
    printf("Data output: location");   for(i = 0; i < n; i++) printf((const char *)test_condition[OUTPUT_LOCATION][i]);    NL(); FLUSH();
    printf("Data input: alignment");   for(i = 0; i < n; i++) printf((const char *)test_condition[INPUT_ALIGNMENT][i]);    NL(); FLUSH();
    printf("Data output: alignment");  for(i = 0; i < n; i++) printf((const char *)test_condition[OUTPUT_ALIGNMENT][i]);   NL(); FLUSH();
    printf("Key type (ROM/RAM/AES)");  for(i = 0; i < n; i++) printf((const char *)test_condition[KEY_GROUP][i]);          NL(); FLUSH();
    printf("Key in cache (X/O)");      for(i = 0; i < n; i++) printf((const char *)test_condition[KEY_IN_CACHE][i]);       NL(); FLUSH();
    printf("Job slice (B)");           for(i = 0; i < n; i++) printf((const char *)test_condition[JOB_SLICE][i]);          NL(); FLUSH();
    printf("Data size(B)");            for(i = 0; i < n; i++) printf((const char *)test_condition[DATA_SIZE][i]);          NL(); FLUSH();
    printf("Timing (ms)");             for(i = 0; i < n; i++) printf((const char *)test_condition[TIMING][i]);             NL(); FLUSH();
    printf("Troughput (MB/s)");        for(i = 0; i < n; i++) printf((const char *)test_condition[THROUGHPUT][i]);         NL(); FLUSH();
    printf("Min / Max timing");        for(i = 0; i < n; i++) printf((const char *)test_condition[MIN_MAX_TIMING][i]);     NL(); FLUSH();
    
    NL(); FLUSH();
}

/*===========================================================================
  ICU-M test function
  [PUBLIC]
  ===========================================================================*/
void fw_timings(void)
{
    int i;
    int n0, n1;
    
    printf("\n");
    printf("Firmware timing evaluations\n");
    
    /* pause life signal management */
    pause_life_signal();
    
    /* initialize the global icum service header */
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    
    get_sys_info();
    
    printf("AES encryption / decryption\n\n");
    
    /* ----------------------------------------------------------------------------------*/
    /* ----------------------------------------------------------------------------------*/

    printf("Measuring timing differences on cipher modes\n");

    i   = 0; i_min  = 0; i_max  = 0;
    min = 0xffffffff; max = 0;

    init_ram_key();
    key_group  = KEY_GRP_AES_RAM;
    key_id     = 0;
    jobslice   = 0;
    data_size  = SIZE_OF_BIG_BUFFER;
    p_data_in  = &BIG_BUFFER[0][0];
    p_data_out = &BIG_BUFFER[1][0];

    /* loop on cipher modes */
    for(n0 = 0; n0 < 2; n0++)
    {
        switch(n0)
        {
            case 0:
                cipher_mode = CIPHER_MOD_ECB;
                break;
            case 1:
                cipher_mode = CIPHER_MOD_CBC;
                break;
            case 2:
                cipher_mode = CIPHER_MOD_CTR;
                break;
            case 3:
                cipher_mode = CIPHER_MOD_CFB;
                break;
            case 4:
                cipher_mode = CIPHER_MOD_OFB;
                break;
            case 5:
                cipher_mode = CIPHER_MOD_XTS;
                break;
        }
        
        /* loop on direction */
        for(n1 = 0; n1 < 2; n1++)
        {
            switch(n1)
            {
                case 0:
                    direction = CIPHER_DIR_ENCRYPTION;
                    break;
                case 1:
                    direction = CIPHER_DIR_DECRYPTION;
                    break;
            }

            update_conditions(i);
            
            start_cipher();
            if(p_ISD->service_result != SERV_OK) goto end;
            update_timings(i);

            i++;
        }
    }

    dump_timings(i);
    
    /* ----------------------------------------------------------------------------------*/
    /* ----------------------------------------------------------------------------------*/

    printf("Measuring impact of data size\n");
    
    i   = 0; i_min  = 0; i_max  = 0;
    min = 0xffffffff; max = 0;

    init_ram_key();
    key_group   = KEY_GRP_AES_RAM;
    key_id      = 0;
    jobslice    = 0;
    data_size   = SIZE_OF_BIG_BUFFER;
    p_data_in   = &BIG_BUFFER[0][0];
    p_data_out  = &BIG_BUFFER[1][0];
    cipher_mode = CIPHER_MOD_CBC;
    direction   = CIPHER_DIR_ENCRYPTION;

    /* loop data size */
    for(n0 = 0, data_size = SIZE_OF_BIG_BUFFER; n0 < 8; n0++, data_size /= 2)
    {
        update_conditions(i);

        start_cipher();
        if(p_ISD->service_result != SERV_OK) goto end;
        update_timings(i);

        i++;
    }

    dump_timings(i);

    /* ----------------------------------------------------------------------------------*/
    /* ----------------------------------------------------------------------------------*/

    printf("Measuring impact of key type & cache\n");
    
    i   = 0; i_min  = 0; i_max  = 0;
    min = 0xffffffff; max = 0;

    init_ram_key();
    key_id      = 0;
    jobslice    = 0;
    data_size   = SIZE_OF_BIG_BUFFER;
    p_data_in   = &BIG_BUFFER[0][0];
    p_data_out  = &BIG_BUFFER[1][0];
    cipher_mode = CIPHER_MOD_CBC;
    direction   = CIPHER_DIR_ENCRYPTION;

    for(n0 = 0; n0 < 1; n0++)
    {
        switch(n0)
        {
            case 0:
                key_group = KEY_GRP_AES_RAM;
                break;
        }

        /* loop on key availability (in DF or in RAM cache) */
        for(n1 = 0; n1 < 2; n1++)
        {
            if(key_group == KEY_GRP_AES_RAM)
            {
                if(n1 != 0)
                {
                    /* for the RAM key, this is the same test twice */
                    /* do it only once */
                    break;
                }
                init_ram_key();
            }
            else if(n1 == 0)
            {
                strcpy(test_condition[KEY_IN_CACHE][i], SEPARATOR "X");
            }
            else
            {
                strcpy(test_condition[KEY_IN_CACHE][i], SEPARATOR "O");
            }

            update_conditions(i);
    
            start_cipher();
            if(p_ISD->service_result != SERV_OK) goto end;
            update_timings(i);
    
            i++;
        }
    }
    
    dump_timings(i);
    
    /* ----------------------------------------------------------------------------------*/
    /* ----------------------------------------------------------------------------------*/

    printf("Measuring impact of job slicing\n");
            
    i   = 0; i_min  = 0; i_max  = 0;
    min = 0xffffffff; max = 0;

    init_ram_key();
    key_group   = KEY_GRP_AES_RAM;
    key_id      = 0;
    jobslice    = 0;
    data_size   = SIZE_OF_BIG_BUFFER;
    p_data_in   = &BIG_BUFFER[0][0];
    p_data_out  = &BIG_BUFFER[1][0];
    cipher_mode = CIPHER_MOD_CBC;
    direction   = CIPHER_DIR_ENCRYPTION;

    /* loop on job slice */
    for(n0 = 0, jobslice = 0; n0 < 8; n0++, jobslice+=64)
    {
        update_conditions(i);

        start_cipher();
        if(p_ISD->service_result != SERV_OK) goto end;
        update_timings(i);

        i++;
    }

    dump_timings(i);

    /* ----------------------------------------------------------------------------------*/
    /* ----------------------------------------------------------------------------------*/

    printf("Measuring impact of input memory location\n");

    i   = 0; i_min  = 0; i_max  = 0;
    min = 0xffffffff; max = 0;

    init_ram_key();
    key_group   = KEY_GRP_AES_RAM;
    key_id      = 0;
    jobslice    = 0;
    data_size   = SIZE_OF_BIG_BUFFER;
    p_data_in   = &BIG_BUFFER[0][0];
    p_data_out  = &BIG_BUFFER[1][0];
    cipher_mode = CIPHER_MOD_CBC;
    direction   = CIPHER_DIR_ENCRYPTION;

    /* loop on data input location */
    for(n0 = 0; n0 < 2; n0++)
    {
        switch(n0)
        {
            case 0:
                p_data_in  = &BIG_BUFFER[0][0];
                break;
            case 1:
                p_data_in  = (uint8_t *)(&main);
                if(((uint32_t)p_data_in % 4) != 0)
                {
                    p_data_in -= ((uint32_t)p_data_in % 4);
                }
                break;
        }
        
        update_conditions(i);

        start_cipher();
        if(p_ISD->service_result != SERV_OK) goto end;
        update_timings(i);

        i++;
    }

    dump_timings(i);

end:    
    /* restart life signal */
    restart_life_signal();

    printf("Done!\n");
}

