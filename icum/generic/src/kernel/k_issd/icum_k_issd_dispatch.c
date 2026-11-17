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
#include "icum_common_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_d_int_pub.h"
#include "icum_d_wdt_pub.h"
#include "icum_d_cr_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_k_issd_pub.h"
#include "icum_k_issd.h"

/***************************************************************************************
* Local Definition                                                                     *
***************************************************************************************/
#define EXT_DEBUGGER_CONNECT   0x00000001uL

#pragma ghs section text   = ".icum_kernel_ram"
#pragma ghs section rodata = ".icum_kernel_const_ram"
/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0025]:[ICUMFW_UD_K_ISSD_0025]
* Function name: k_ISSD_CheckNVMWarning
* Description  : Check the NVM Warning
* Arguments    : (IN) : *p_isd
* Return Value : ER_OK
***************************************************************************************/
uint32_t k_ISSD_CheckNVMWarning(r_icumif_isd_t *p_isd)
{
    uint32_t result = ER_OK;
    uint8_t nvm_warn_state;

    if(SERV_OK == p_isd->service_result)
    {
        /* get of NVM Warning state */
        nvm_warn_state = NVM_DATA_NO_WARNING;
        (void)MM_FLS_GetNVMWarnFlg(&nvm_warn_state);
        /* set to service result, if NVM Warning */
        if((uint32_t)NVM_DATA_WARNING == (uint32_t)nvm_warn_state)
        {
            p_isd->service_result = SERV_NVM_DATA_WARNING;
            /* clear the NVM Warning */
            (void)MM_FLS_ClrNVMWarnFlg();
        }
        else if((uint32_t)FLS_FULL_WARNING1 == (uint32_t)nvm_warn_state)
        {
            p_isd->service_result = SERV_NVM_DATA_FULL_WARNING1;
            /* clear the NVM Warning */
            (void)MM_FLS_ClrNVMWarnFlg();
        }
        else if((uint32_t)FLS_FULL_WARNING2 == (uint32_t)nvm_warn_state)
        {
            p_isd->service_result = SERV_NVM_DATA_FULL_WARNING2;
            /* clear the NVM Warning */
            (void)MM_FLS_ClrNVMWarnFlg();
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
    return(result);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0021]:[ICUMFW_UD_K_ISSD_0021]
* Function name: k_ISSD_HandleResponse
* Description  : Trigger response to host
* Arguments    : (IN) : *p_isd
* Return Value : ER_OK : if the response has been set in the response queue
*                        and p_ISD->host_int_ch is different from 0xff
*                ER_PARAM :
***************************************************************************************/
uint32_t k_ISSD_HandleResponse(r_icumif_isd_t *p_isd)
{
    uint32_t rtn = ER_OK;

    /* 1. NULL check */
    if (NULL != p_isd)
    {
        /* 2. request from PE */
        if(((int32_t)p_isd->requester_id >= (int32_t)MISC_PEID_PE0) &&
           ((int32_t)p_isd->requester_id < (int32_t)TOTAL_PE_NUM))
        {
            if((uint32_t)0xFFu != (uint32_t)p_isd->host_int_ch)
            {
                /* Check the NVM Warning */
                //zxhk (void)k_ISSD_CheckNVMWarning(p_isd);
                /* response in queue: trigger host interrupt */
                rtn = D_COMM_TriggerInterruptToPE((uint32_t)p_isd->requester_id, ICUM_COMM_ICH_TO_SERV_RESPONSE, p_isd);
            }
            else
            {
                /* Do nothing */
            }
        }
        /* 3. request from ICUP */
        else if((uint32_t)p_isd->requester_id == (uint32_t)MISC_PEID_ICUP)
        {
            /* internal service request: no response to trigger */
        }
        /* 4. illegal request */
        else
        {
            rtn = ER_PARAM;
            /* should never occur */
            ASSERT(0);
        }
    }
    else
    {
        rtn = ER_PARAM;
    }

    return(rtn);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0026]:[ICUMFW_UD_K_ISSD_0026]
* Function name: k_ISSD_CheckDebuggerConnection
* Description  : check debugger connection and set EXT_DEBUGGER
* Arguments    : none
* Return Value : none
***************************************************************************************/
void k_ISSD_CheckDebuggerConnection(void)
{
    uint32_t debugger_status;
    uint32_t rtn;

    /* Ext debugger check */
    rtn = D_CR_GetDCS(&debugger_status);
    if(ER_OK == rtn)
    {
        if(EXT_DEBUGGER_CONNECT == debugger_status)
        {
            /* Set EXT_DEBUGGER bit - ICU2PES register */
            (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_EXT_DEBUG , ICU_STATUS_ON);
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
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0024]:[ICUMFW_UD_K_ISSD_0024]
* Function name: K_ISSD_HandleRequest
* Description  : Service dispatcher
*                This function is triggered via the inter-PE exception
* Arguments    : (IN) : channel
*                       *p_isd
* Return Value : ER_OK :
*                SERV_REQUESTER_ID_ERROR : wrong requester ID
*                SERV_UNAVAILABLE : service unavailable
*                SERV_JOB_QUEUE_FULL : cannot accept another service request
*                SERV_REQUEST :
***************************************************************************************/
uint32_t K_ISSD_HandleRequest(icum_comm_int_from_channel_t channel, r_icumif_isd_t *p_isd)
{
    icum_job_t         *p_ghost_job;
    p_service_func_t   p_service_func;
    r_icumif_job_id_t  new_job_id = 0u;
    r_icumif_service_result_t err = SERV_OK;
    uint32_t rtn = ER_OK;
    bool highest_job_executed = (bool)false;
    uint32_t result;

    /* Check channel number */
    if (ICUM_COMM_ICH_FM_SERV_REQUEST == channel)
    {
        /* NULL check */
        if (NULL != p_isd)
        {
            /* is the requester ID valid? */
            if(((int32_t)MISC_PEID_PE0 > (int32_t)p_isd->requester_id) ||
               ((int32_t)TOTAL_PE_NUM <= (int32_t)p_isd->requester_id))
            {
                err = SERV_REQUESTER_ID_ERROR;
                /* choose an arbitrary target for the error response */
                p_isd->requester_id = MISC_PEID_PE0;
            }
            /* requester ID is valid */
            else
            {
                if ((uint32_t)R_ICUMIF_SERV_PRIORITY_HIGHEST == (uint32_t)p_isd->service_priority)
                {
                    /* Dispatcher of highest priority service request */
                    k_ISSD_HighestDispatch(p_isd, &highest_job_executed);
                }
                else
                {
                    /* Do nothing */
                }

                if((uint32_t)false == (uint32_t)highest_job_executed)
                {
                    /* retrieve the pointer to the service function */
                    result = k_ISSD_GetServiceFunc(p_isd->service_id, &p_service_func);

                    if(ER_SERV_ID_ERROR == result)
                    {
                        /* service ID provided is invalid */
                        err = SERV_ID_ERROR;
                    }
                    else if(ER_SERV_UNAVAILABLE == result)
                    {
                        /* Conditions for service execution not satisfied */
                        err = SERV_UNAVAILABLE;
                    }
                    else if(ER_SERV_SYS_ERROR == result)
                    {
                        err = SERV_SYS_ERROR;
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
            /* No error occurred */
            if((SERV_OK == err) &&
               ((uint32_t)false == (uint32_t)highest_job_executed))
            {
                /* is this the continuation of a previous job? */
                p_ghost_job = NULL;

                if((uint32_t)0u != (uint32_t)p_isd->job_id)
                {
                    switch(p_isd->service_id)
                    {
                        case SERVICE_01_AES_CIPHER:
                        case SERVICE_01_AES_AUTH_CIPHER:
                        case SERVICE_01_AES_CMAC:
                        case SERVICE_01_HASH:
                            /* retrieve job ghost */
                            k_ISSD_GetJob(p_isd->job_id, &p_ghost_job);
                            break;
                        default:
                            /* job continuation not supported */
                            break;
                    }
                }
                else
                {
                    /* Do nothing */
                }
                /* add job in queue */
                k_ISSD_AddJobQueue(p_isd, p_ghost_job, &new_job_id);
                if((uint32_t)0u == (uint32_t)new_job_id)
                {
                    /* indicate impossibility to add to job queue */
                    err = SERV_JOB_QUEUE_FULL;
                }
                else
                {
                    /* request acknowledged */
                    p_isd->job_id = new_job_id;
                    err = SERV_REQUEST;
                }
            }
            else
            {
                /* Do nothing */
            }

            if ((uint32_t)false == (uint32_t)highest_job_executed)
            {
                p_isd->service_result = err;
            }
            else
            {
                /* Do nothing */
            }
            /* check service result */
            if(SERV_REQUEST > p_isd->service_result)
            {
                /* provide the error back to the caller */
                /* note: in this case, no job created (see above) */
                if((uint32_t)0xFFu != (uint32_t)p_isd->host_int_ch)
                {
                    /* response in queue: trigger host interrupt */
                    rtn = D_COMM_ServiceResponseToPE((uint32_t)p_isd->requester_id, p_isd);
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
        }
        else
        {
            rtn = ER_PARAM;
        }
    }
    else
    {
        /* Do nothing */
    }

    return(rtn);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0022]:[ICUMFW_UD_K_ISSD_0022]
* Function name: k_ISSD_ProcessJob
* Description  : Calling the service function for the job given in parameter
* Arguments    : (IN) : *p_job
* Return Value : none
***************************************************************************************/
void k_ISSD_ProcessJob(icum_job_t *p_job)
{
    p_service_func_t p_service_func;
    uint32_t result;

    /* 1. retrieve the pointer to the service function */
    result = k_ISSD_GetServiceFunc(p_job->p_ISD->service_id, &p_service_func);

    /* 2. jump to corresponding service handler */
    if(ER_OK == result)
    {
        /* process the function associated to the current job */
        p_service_func(p_job);
    }
    else if(ER_SERV_ID_ERROR == result)
    {
        p_job->p_ISD->service_result = SERV_ID_ERROR;
    }
    else if (ER_SERV_SYS_ERROR == result)
    {
        p_job->p_ISD->service_result = SERV_SYS_ERROR;
    }
    else
    {
        /* 3. this situation is possible when the service request */
        /* is triggered in between 2 mode changes */
        /* in this case, a job has been created referring to a */
        /* service that is not available anymore (change in */
        /* dispatch table) */
        /* action: simply return that the service is unavailable */
        p_job->p_ISD->service_result = SERV_UNAVAILABLE;
    }
}

#pragma ghs section text    = default
#pragma ghs section rodata  = default

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0023]:[ICUMFW_UD_K_ISSD_0023]
* Function name: k_ISSD_MainDispatch
* Description  : Main dispatch loop
* Arguments    : none
* Return Value : none
***************************************************************************************/
void k_ISSD_MainDispatch(void)
{
    icum_job_t *p_job;
    bool wdt_flag;
    bool mode_change;
    bool boot_mode;
    bool gen_request;
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

            /* get job counter */
            (void)k_ISSD_GetAllJobCount(&job_count);
            /* get request of mode change */
            k_ISSD_ModeChangeRequested(&mode_change);
            /* get request of service generate */
            (void)k_ISSD_IsRegistGenService(&gen_request);
        }
        while(((uint32_t)false == (uint32_t)mode_change) && ((uint32_t)false == (uint32_t)gen_request) && (0uL != job_count));

        /* secure boot/background verify executing */
        k_ISSD_IsBootMode(&boot_mode);

        /* do not go in HALT mode if a mode change has been requested or 
           requested generate service or auto verify mode */
        if(((uint32_t)false != (uint32_t)mode_change) || ((uint32_t)false != (uint32_t)gen_request) || ((uint32_t)false != (uint32_t)boot_mode))
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


