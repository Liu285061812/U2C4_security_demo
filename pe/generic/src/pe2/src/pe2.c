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
#include "pe_target_device.h"
#include "pe2.h"
#include "heat_run_parallel_pe.h"
#include "pe2.int.h"
#include "pe_reg.h"
#include "shared.h"
#include "vrun.api.h"

extern unsigned long long cnt_other_request_error_pe2;

// ********************************************************************************
// Global variables
// ********************************************************************************
// variable to retrieve SYNC_PE register value
uint16_t notification_from_pe1;

/*===========================================================================*/
/* Error codes in text format                                                */
/*===========================================================================*/
#ifdef UNUSED
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
#endif

// ********************************************************************************
// Exception handlers
// ********************************************************************************

/*===========================================================================
  Exception handler for interrupts coming from PE1
  [PUBLIC]
  ===========================================================================*/
#pragma ghs reference INTHAND_PE1
__interrupt void INTHAND_PE1(void)
{
    r_icumif_isd_t *p_ISD;

    // retrieve the ISD to be used here
    p_ISD = (r_icumif_isd_t *)SYNC_PE;
    
    // trigger the service request
    R_ICUMIF_ServiceRequest(p_ISD);

    // done: synchronize with PE1
    SYNC_PE = 0;
}

/*===========================================================================
  Exception handler for interrupts coming from PE1
  [PUBLIC]
  ===========================================================================*/
#pragma ghs reference EIINT2
__interrupt void EIINT2(void)
{
    volatile uint8_t lul_dummy;

    // retrieve the ST_TYPE to be started here
    notification_from_pe1 = (uint16_t)SYNC_PE;
    
    // done: synchronize with PE1
    SYNC_PE = SYNC_PE_COMPLETED;

    // PE0 -> PE1 Interrupt Request Clear
    REG_PE_IPIRIPI2RCLR0 = 0x01 << (D_PEID - 1);
    /* Dummy Read */
    lul_dummy = REG_PE_IPIRIPI2RCLR0;
    /* Synchronize */
    __SYNCP();
}

/*===========================================================================
  Exception handler for interrupts coming from ICUP
  [PUBLIC]
  ===========================================================================*/
#pragma ghs reference INTHAND_ICUP
__interrupt void INTHAND_ICUP(void)
{
    // handle the response
    R_ICUMIF_ServiceResponse();
}

/*===========================================================================
  Waiting for the ICU-M firmware system to be initialized
  RAM execution required! (FPSYS initialized on ICU-M side)
  [PUBLIC]
  ===========================================================================*/
#pragma ghs section text=".pe_rom"
static void wait_for_icu(void)
{
    r_icumif_sts_t *ptrICU2PES;
    // in debug mode, PEs are released after reset in all conditions
    // wait until the ICU-M system intialization is complete
    do
    {
        ptrICU2PES = R_ICUMIF_GetStatus();
        __asm("snooze");
    } while(ptrICU2PES->STARTPE == 0);
}
#pragma ghs section text=default

/*===========================================================================
  CallBack Function
  [PRIVATE]
  ===========================================================================*/
#define d_INT_PE2CH0       0x10
int *p_secure_tick_update_count_pe2_ch0 = (int *)&cnt_other_request_error_pe2;
void system_callback(void)
{
    uint32_t *status;

    status = (uint32_t *)R_ICUMIF_GetStatus();

    // error check
    if((*status & 0x00008000) == 0x00008000)
    {
        /* system error  */
        *(char *)d_SYSERR_LOG_ADDR |= d_INT_PE2CH0;

    }
    else if((*status & 0x0000F800) == 0)
    {
        /* secure tick timer interval */
        *(char *)d_TIMER_LOG_ADDR |= d_INT_PE2CH0;
        (*p_secure_tick_update_count_pe2_ch0)++;
    }
    else
    {
        /* error ocurred */
    }
    return;
}

/*===========================================================================
  PE2 entry point (reset vector)
  [PUBLIC]
  ===========================================================================*/
int main(void)
{
    int32_t errno;
    R_ICUMIF_CB_REGIST_t cb_regist;

    // *** debug mode only ***
    // wait for the ICU to start-up
    wait_for_icu();

    // wait for the system init in PE1
    while(SYNC_PE != 1)
    {
        __asm("snooze");
    }

    // enable interrupt servicing
    __EI();
    
    // interrupts coming from PE1
    // set priority level to 6, handler referenced on table, unmask interrupt

    // Initialize interface to ICU-M
    errno = R_ICUMIF_Init(ICUM_QUEUE);
    if(errno != SERV_OK)
    {
        // error
    }

    // Set System call back function
    cb_regist.p_func_addr = &system_callback;
    errno = R_ICUMIF_SetSystemCallBackFunc(&cb_regist);
    if(errno != SERV_OK)
    {
        // error
    }

#if   (D_PEID == 2)
    REG_PE_INTC1CPU1EIC2 = 0x46;   // EIINT2(INTIPIR2) Enable
    REG_PE_IPIRIPI2EN1   = 0x01;   // PE0 -> PE1 Interrupt Enable
#elif (D_PEID == 3)
    REG_PE_INTC1CPU2EIC2 = 0x46;   // EIINT2(INTIPIR2) Enable
    REG_PE_IPIRIPI2EN2   = 0x01;   // PE0 -> PE1 Interrupt Enable
#elif (D_PEID == 4)
    REG_PE_INTC1CPU3EIC2 = 0x46;   // EIINT2(INTIPIR2) Enable
    REG_PE_IPIRIPI2EN3   = 0x01;   // PE0 -> PE1 Interrupt Enable
#elif (D_PEID == 5)
    REG_PE_INTC1CPU4EIC2 = 0x46;   // EIINT2(INTIPIR2) Enable
    REG_PE_IPIRIPI2EN4   = 0x01;   // PE0 -> PE1 Interrupt Enable
#elif (D_PEID == 6)
    REG_PE_INTC1CPU5EIC2 = 0x46;   // EIINT2(INTIPIR2) Enable
    REG_PE_IPIRIPI2EN5   = 0x01;   // PE0 -> PE1 Interrupt Enable
#endif
    notification_from_pe1 = 0;

    // infinite loop (waiting for wake-up)
    while(1)
    {
        HALT();
        NOPS(10);

        switch(notification_from_pe1)
        {
            case START_ST_RAND_SERVICE_TO_PE2:
                icum_test_pe2_heat_run_parallel_pe();
                notification_from_pe1 = 0;
                break;
            default:
                break;
        }
    }
}

void vrun_cmd_init(void) {}
void vrun_cmd_usr_1(void) {}
void vrun_cmd_usr_2(void) {}
void vrun_cmd_usr_3(void) {}
void vrun_cmd_usr_4(void) {}
void vrun_cmd_usr_5(void) {}
void vrun_cmd_usr_6(void) {}
void vrun_cmd_usr_7(void) {}
void vrun_cmd_usr_8(void) {}
void vrun_cmd_usr_9(void) {}
void vrun_cmd_usr_10(void) {}
const char CMD_USR_1[];
const char CMD_USR_2[];
const char CMD_USR_3[];
const char CMD_USR_4[];
const char CMD_USR_5[];
const char CMD_USR_6[];
const char CMD_USR_7[];
const char CMD_USR_8[];
const char CMD_USR_9[];
const char CMD_USR_10[];
