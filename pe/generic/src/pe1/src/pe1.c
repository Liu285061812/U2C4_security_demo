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

/* ********************************************************************************/
/* Headers                                                                        */
/* ********************************************************************************/

#include <stdio.h>
#include <string.h>

#undef NULL
#include "r_icumif_api.h"
#include "r_icumif_pub.h"

#include "pe1.h"
#include "pe1.int.h"
#include "shared.h"

#include "vrun.api.h"
#include "icum_d_comm_pe.h"
#include "pe_target_device.h"
#include "pe_reg.h"

/* ********************************************************************************/
/* Local constants                                                                */
/* ********************************************************************************/

/*===========================================================================
  Block cipher modes supported in text format
  *** IMPORTANT!! ***
  -> should align with type cipher_modes_t in icumif.api.aes_cipher.h
  ===========================================================================*/

static const char str_cipher_mode[][8] = 
{
    "CBC",
    "ECB",
    "CTR",
    "CFB",
    "OFB",
    "XTS",
};

/* ********************************************************************************/
/* Public constants                                                               */
/* ********************************************************************************/

/*===========================================================================
  Test keys
  [PUBLIC]
  ===========================================================================*/
const test_key_t keys[16] = 
{
    {"ROM_KEY_0",           KEY_GRP_AES_ROM, 0},
    {"ROM_KEY_1",           KEY_GRP_AES_ROM, 1},
    {"ROM_KEY_5",           KEY_GRP_AES_ROM, 5},
    {"RAM_KEY_0",           KEY_GRP_AES_RAM, 0},
    {"RAM_KEY_1",           KEY_GRP_AES_RAM, 1},
    {"RAM_KEY_5",           KEY_GRP_AES_RAM, 5},
    {"KEY #0",              KEY_GRP_AES, 0},
    {"KEY #1 - RESTRICTED", KEY_GRP_AES, 1},
    {"SHE_KEY_4",           KEY_GRP_AES, 5},
    {"KEY_25",              KEY_GRP_AES, 25},
    {"",                    KEY_GRP_AES_ROM, 0}
};

/*===========================================================================*/
/* Error codes in text format                                                */
/*===========================================================================*/
static const char str_error[][32] = 
{
    "SERV_OK",
    "SERV_SYS_CFG_NOT_INITIALIZED",
    "SERV_ID_ERROR",
    "SERV_REQUESTER_ID_ERROR",
    "SERV_PRM_ERROR",
    "SERV_KEY_INVALID",
    "SERV_KEY_NOT_USABLE",
    "SERV_INVALID_KEY_GROUP",
    "SERV_INVALID_KEY_INDEX",
    "SERV_KEY_SLOT_EMPTY",
    "SERV_KEY_WRITE_PROTECTED",
    "SERV_KEY_UPDATE_ERROR",
    "SERV_SRC_MEM_ERROR",
    "SERV_DEST_MEM_ERROR",
    "SERV_JOB_QUEUE_FULL",
    "SERV_UNAVAILABLE",
    "SERV_INSTANCE_ALREADY_RUNNING",
    "SERV_SYS_ERROR",
    "SERV_MPU_ERROR",

    "SERV_NVM_PROG_ERROR",
    "SERV_NVM_SYS_ERROR",
    "SERV_NVM_SYS_ERROR_CRITICAL",
    "SERV_NVM_UNAVAILABLE",
    "SERV_DATA_INVALID",
    "SERV_NVM_DATA_WARNING",
    "SERV_NVM_SIZE_ERROR",

    "SERV_CRYPT_ERROR",
    "SERV_CLUSTER_VERIF_STATUS_LOCK",
    "SERV_BKGROUND_CHECK_RUNNING",
    
    "SERV_FW_ACTIVATION_FAILED",
    "SERV_INVALID_ENCODING_FORMAT",
    "SERV_ISD_NOT_ACCESSIBLE",
    "SERV_INVALID_DATA_SIZE",

    "SERV_KEY_GENERATION_ERROR",
    "SERV_KEY_VERIFICATION_FAIL",
    "SERV_INVALID_CURVE_ID",
    "SERV_ECC_POINT_INVALID",

    "SERV_MEMORY_FULL",
    
    "SERV_INIT_DELAY",

    "SERV_MYDATA_SET_EMPTY",
    "SERV_MYDATA_SLOT_EMPTY",
    "SERV_MYDATA_SIZE_ERR",
    "SERV_MYDATA_INVALID_IDX",
    
    "SERV_REQUEST_QUEUE_FULL",
    
    "SERV_INVALID_OPERATION",
    
    "SERV_KEY_CACHE_FULL",
    "SERV_SEQUENCE_ERROR",
    
    "SERV_CR_VERIFY_FAIL",

    "SERV_AES_TIMEOUT_ERROR",
    "SERV_AES_STATUS_ERROR",
    "SERV_AES_PRM_ERROR",

    "SERV_NO_COMP_TRNG",
    "SERV_NO_INIT_RNG",
    "SERV_TRN_ERROR",
    "SERV_NEED_RESEED",

    "SERV_RESTRICTED",

    "SERV_NVM_DATA_FULL_WARNING1",
    "SERV_NVM_DATA_FULL_WARNING2",
};

/* ********************************************************************************/
/* Global variables                                                               */
/* ********************************************************************************/

int global_key_id;
int global_cipher_mode;

int test_df_pe_entry_nak_resp = 0;
int test_cf_pe_entry_nak_resp = 0;

int test_cf_ack_wait = 0;
int test_df_ack_wait = 0;
int test_usec = 0;

// variable to retrieve SYNC_PE register value
uint16_t notification_from_pe2;

/* ********************************************************************************/
/* Fatal exceptions                                                               */
/* ********************************************************************************/

#pragma ghs reference SYSERR
#pragma ghs reference FEINT

__interrupt void SYSERR(void)
{
    while(1);
}

__interrupt void FEINT(void)
{
    while(1);
}


/*===========================================================================
  EI Level Interrupt handler
   Single Interrupt request from ICUP to CPU (PE)
   EIC20    ICU-M   INTICU2PES
  [PUBLIC]
  ===========================================================================*/
#pragma ghs reference EIINT20
__interrupt void EIINT20(void)
{
/*    R_ICUMIF_AckSysInterrupt();*/
}

/*===========================================================================
  Exception handler for interrupts coming from PE2
  [PUBLIC]
  ===========================================================================*/
#pragma ghs reference EIINT3
__interrupt void EIINT3(void)
{
    volatile uint8_t lul_dummy;

    /* retrieve the ST_TYPE to be started here*/
    notification_from_pe2 = (uint16_t)SYNC_PE;
    
    /* done: synchronize with PE1*/
    SYNC_PE = SYNC_PE_COMPLETED;

    /* PE1 -> PE0 Interrupt Request Clear */
    REG_PE_IPIRIPI3RCLR0 = 0x01;
    /* Dummy Read */
    lul_dummy = REG_PE_IPIRIPI3RCLR0;
    /* Synchronize */
    __SYNCP();

}

/* ********************************************************************************/
/* Test functions                                                                 */
/* ********************************************************************************/

#pragma ghs section text=".pe_rom"
/*===========================================================================
  Trigger an ICU-M service and wait for the response
  [PUBLIC]
  ===========================================================================*/
void trigger_service_and_wait(r_icumif_isd_t *p_ISD)
{
    int32_t errno;

    /* trigger the service request*/
    errno = R_ICUMIF_ServiceRequest(p_ISD);
    if(errno != SERV_OK)
    {
        /* error */
    }
    /* go in HALT mode */
    /* wait for interrupt */
    while (1)
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
        HALT();
        NOPS(10);
    }

    /* test for possible error */
    if(p_ISD->service_result != SERV_OK)
    {
        printf("Service error: %d (", p_ISD->service_result);
        printf(") *****\a\n");
    }
}

#pragma ghs section text=default

/*===========================================================================
  Display a throughput
  [PUBLIC]
===========================================================================*/
void print_throughput(int data_size)
{
    float throughput;

    /* calculate throughput*/
    if(vrun_chrono_sec > 0)
    {
        /* calculate in B/s*/
        throughput  = (float)(data_size * 1000);
        throughput /= (vrun_chrono_sec * 1000 + vrun_chrono_millisec);
    }
    else
    {
        /* calculate in B/s */
        throughput  = (float)(data_size * 1000) * 1000;
        throughput /= (vrun_chrono_millisec * 1000 + vrun_chrono_microsec);
    }

    /* output throughput */
    printf("Data size:      %d bytes\n", (int)data_size);
    if(throughput > (1024 * 1024))
    {
        printf("Throughput:     %0.2f MB/s\n", (throughput / (1024 * 1024)));
    }
    else if(throughput > 1024)
    {
        printf("Throughput:     %0.2f KB/s\n", (throughput / 1024));
    }
    else
    {
        printf("Throughput:     %0.2f B/s\n", throughput);
    }
}

/*===========================================================================
  Returns the cipher mode name in text format
  [PUBLIC]
  ===========================================================================*/
char * get_cipher_mode_name(uint8_t cipher_mode)
{
    if(cipher_mode < (sizeof(str_cipher_mode) / sizeof(str_cipher_mode[0])))
    {
        return((char *)&str_cipher_mode[cipher_mode]);
    }
    else
    {
        return("");
    }
}

/*===========================================================================
  CallBack Function
  [PRIVATE]
  ===========================================================================*/
#define d_INT_PE1CH0       0x01
#define d_INT_PE2CH0       0x10

void system_callback(void)
{
    uint32_t *status;

    status = (uint32_t *)R_ICUMIF_GetStatus();

    /* error check */
    if((*status & 0x00008000) == 0x00008000)
    {
        /* system error  */
        *(char *)d_SYSERR_LOG_ADDR |= d_INT_PE1CH0;

    }
    else if((*status & 0x0000F800) == 0)
    {
        /* secure tick timer interval */
        *(char *)d_TIMER_LOG_ADDR |= d_INT_PE1CH0;
    }
    else
    {
        /* error ocurred */
    }
    return;
}

/*===========================================================================
  System initialization
  [PUBLIC]
  ===========================================================================*/
void flash_init(void);
void system_init(void)
{
    uint32_t *status;
    int32_t errno;
    R_ICUMIF_CB_REGIST_t cb_regist;

    memset(COM_BUFFER, 0, SIZE_OF_COM_BUFFER);
    /* clear the ISD_BUFFER */
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);

    /* clear the big_buffers */
    memset(BIG_BUFFER[0], 0, SIZE_OF_BIG_BUFFER);
    memset(BIG_BUFFER[1], 0, SIZE_OF_BIG_BUFFER);
    memset(BIG_BUFFER[2], 0, SIZE_OF_BIG_BUFFER);

    /* clear key ID and cipher mode */
    global_key_id = 0;
    global_cipher_mode = CIPHER_MOD_ECB;

    REG_PE_INTC1CPU0EIC753 = 0x0045UL;
    REG_PE_INTC1CPU0EIC754 = 0x0045UL;
    REG_PE_INTC1CPU0EIC755 = 0x0045UL;

    status = (uint32_t *)R_ICUMIF_GetStatus();

    /* error check */
    if((*status & 0x0000F800) == 0)
    {
        /* Initialize interface to ICU-M */

        errno = R_ICUMIF_Init(ICUM_QUEUE);
        if(errno != SERV_OK)
        {
            /* error */
        }

    }
    
    cb_regist.p_func_addr = &system_callback;

    errno = R_ICUMIF_SetSystemCallBackFunc(&cb_regist);

    notification_from_pe2 = 0;
    REG_PE_INTC1CPU0EIC3 = 0x46;   /* EIINT3(INTIPIR3) Enable */
    REG_PE_IPIRIPI3EN0   = 0x01;   /* PE1 -> PE0 Interrupt Enable */

    pe_flash_initialize();
}

/*===========================================================================*/
/* Returns the error code in text format                                     */
/* [PUBLIC]                                                                  */
/*===========================================================================*/
char * R_ICUMIF_GetErrorCodeName(uint8_t error_code)
{
    if(error_code < (sizeof(str_error) / sizeof(str_error[0])))
    {
        return((char *)&str_error[error_code]);
    }
    else
    {
        return("");
    }
}

