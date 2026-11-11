/******************************************************************************/
/* Component Name   ISSD (ICU-M Start-up & Service Dispatcher)                */
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

/* Header files                     */
#include "renesas_types.h"
#include "r_icumif_api.h"
#include "icum_d_comm_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_int_pub.h"
#include "icum_d_wdt_pub.h"
#include "icum_k_issd_pub.h"
#include "icum_k_issd.h"

#pragma ghs section text   = ".icum_kernel_ram"
#pragma ghs section rodata = ".icum_kernel_const_ram"
/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0052]:[ICUMFW_UD_K_ISSD_0052]
* Function name: k_ISSD_RamDispatch
* Description  : Dispatch loop in RAM
* Arguments    : none
* Return Value : none
***************************************************************************************/
void k_ISSD_RamDispatch(void)
{
    icum_job_t *p_job;
    bool wdt_flag;
    bool mode_change;
    uint32_t job_count;
    uint32_t saved_psw = 0UL;

    /* loop until a execution mode change is requested */
    k_ISSD_ModeChangeRequested(&mode_change);
    while((uint32_t)false == (uint32_t)mode_change)
    {
        do
        {
            /* re-enable interrupts (if necessary) */
            ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

            /* get next job in queue */
            k_ISSD_GetNextJob(&p_job);

            /* next job to process? */
            if(NULL != p_job)
            {
                /* check debugger connection and set EXT_DEBUGGER */
                k_ISSD_CheckDebuggerConnection();

                /* run the job */
                /* note: using a function call in order to enjoy */
                k_ISSD_ProcessJob(p_job);

                /* are we done yet? */
                if((SERV_REQUEST > p_job->p_ISD->service_result) ||
                   (SERV_IN_PROCESS < p_job->p_ISD->service_result))
                {
                    /* convert undefined result code to SYSTEM ERROR */
                    if(SERV_IN_PROCESS < p_job->p_ISD->service_result)
                    {
                        p_job->p_ISD->service_result = SERV_SYS_ERROR;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                    /* provide the service result to the caller */
                    (void)k_ISSD_HandleResponse(p_job->p_ISD);
                    /* remove the current job from the queue */
                    k_ISSD_RemoveCurrentJob();
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
               /* Do nothing */
            }
            /* check need restart watchdog timer */
            D_WDT_GetIntRequest(&wdt_flag);
            /* restart watchdog timer */
            D_WDT_Start();

            /* disable interrupts (if not already disabled) */
            DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

            (void)k_ISSD_GetAllJobCount(&job_count);
            k_ISSD_ModeChangeRequested(&mode_change);
        }
        while(((uint32_t)false == (uint32_t)mode_change) && (0uL != job_count));

        /* do not go in HALT mode if a mode change has been requested */
        if((uint32_t)false != (uint32_t)mode_change)
        {
            break;
        }
        else
        {
            /* Do nothing */
        }

        /* update firmware status */
        (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_BUSY, ICU_STATUS_OFF);

        /* go in stand-by, wait for next service request */
        SET_HALT();
        NOPS(5U);

        /* re-enable interrupts (if necessary) */
        ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

        /* update firmware status */
        (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_BUSY, ICU_STATUS_ON);

        /* loop until a execution mode change is requested */
        k_ISSD_ModeChangeRequested(&mode_change);
    }
    /* re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
}
#pragma ghs section text    = default
#pragma ghs section rodata  = default

