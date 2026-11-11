/******************************************************************************/
/* Component Name   ICU-M Interface Library (ICUMIF)                          */
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

/**************************************************************************************/
/* Header files                                                                       */
/**************************************************************************************/
#include "r_icumif_api.h"
#include "r_icumif_pub.h"
#include "icum_d_comm_pe_pub.h"

#include "r_icumif.h"

/**************************************************************************************/
/* Interface functions                                                                */
/**************************************************************************************/

#pragma ghs section text   = ".icumif_rom"
#pragma ghs section rodata = ".icumif.rodata"

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_IF_0001]:[ICUMFW_UD_L_IF_0001]
* Function name: R_ICUMIF_Init
* Description  : Initialize the ICU-M interface library
* Arguments    : uint32_t       *p_request_queue : Pointer to service request queue
* Return Value : int32_t Library initialization result
*                   0               : Successful
*                   Negative values : Parameter error
***************************************************************************************/
int32_t R_ICUMIF_Init(uint32_t *p_request_queue)
{
    int32_t     ret_val = R_ICUMIF_ER_OK;
    uint32_t    call_comm_ret;

    /* 1. address check using Cluster RAM area check function */
    if (0UL != ((uint32_t)((volatile void *)p_request_queue) & ALIGNMENT_4BYTE_CHECK_PATTERN))
    {
            ret_val = R_ICUMIF_ER_PARAM_ALIGNMENT;
    }
    else
    {
        /* 3. Execute COMM(PE) driver initialization to initialize service queue */
        call_comm_ret = D_COMM_PE_Init(p_request_queue);

        /* 4. According to result of COMM (PE) driver initialization process, */
        /*    execute respective corresponding processes                      */
        if (ER_D_COMM_PARAM_NOT_G_RAM == call_comm_ret)
        {
            /* 4-1. Parameters other than shared RAM area, process is terminated */
            ret_val = R_ICUMIF_ER_ADDR_NOT_G_RAM;
        }
        else if (ER_D_COMM_QUE_NOT_ACCESS == call_comm_ret)
        {
            /* 4-2. */
            ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
        }
        else
        {
            /* 5. Execute interrupt enable process to enable interrupts from ICUP */
            D_COMM_SetInterruptEnable();
        }
    }

    /* 7. */
    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_IF_0002]:[ICUMFW_UD_L_IF_0002]
* Function name: R_ICUMIF_ServiceRequest
* Description  : Requests ICU-M to execute the service
* Arguments    : r_icumif_isd_t *p_ISD : Pointer to ISD where request parameter is set
* Return Value : int32_t Service execution request result
*                   0               : Successful
*                   Negative values : Parameter error, Interface library uninitialized
***************************************************************************************/
int32_t R_ICUMIF_ServiceRequest(r_icumif_isd_t *p_ISD)
{
    int32_t     ret_val = R_ICUMIF_ER_OK;
    uint32_t    call_comm_ret;

    /* 1. address check using Cluster RAM area check function */
    if (0UL != ((uint32_t)((volatile void *)p_ISD) & ALIGNMENT_4BYTE_CHECK_PATTERN))
    {
        ret_val = R_ICUMIF_ER_PARAM_ALIGNMENT;
    }
    else
    {
        /* ---- These lines should be left unchanged.   Start ---- */
        /* 3-1. Set ID of service request PE */
        p_ISD->requester_id = (r_icumif_requester_id_t)GET_MY_PEID;

        /* 3-2. Set channel of service execution response interrupt */
        p_ISD->host_int_ch = ICUM_COMM_ICH_FM_SERV_RESPONSE;

        /* 3-3. Set initial state to service processing state */
        p_ISD->service_result = SERV_NEW_REQUEST;
        /* ---- These lines should be left unchanged.   End   ---- */

        /* 4. In order to check whether area indicated by p_ISD exists in RAM, */
        /*    it checks whether service processing state is SERV_NEW_REQUEST   */
        if (SERV_NEW_REQUEST != p_ISD->service_result)
        {
            /* Not SERV_NEW_REQUEST, process is terminated */
            ret_val = R_ICUMIF_ER_ADDR_NOT_G_RAM;
        }
        else
        {
            /* 5. Execute interrupt issue process to issue service execution req interrupt */
            call_comm_ret = D_COMM_TriggerInterruptToICUP(ICUM_COMM_ICH_TO_SERV_REQUEST, p_ISD);

            /* 6. According to result of interrupt issue process, */
            /*    execute respective corresponding processes      */
            switch (call_comm_ret)
            {
                case ER_OK:
                    /* 6-1. Do nothing */
                    break;
                case ER_D_COMM_QUE_FULL:
                    /* ---- These lines should be left unchanged.   Start ---- */
                    /* 6-2. */
                    p_ISD->req_res_status = R_ICUMIF_REQRES_PE_RES_DEQUE;
                    p_ISD->service_result = SERV_REQUEST_QUEUE_FULL;
                    ret_val = R_ICUMIF_ER_REQ_QUEUE_FULL;
                    /* ---- These lines should be left unchanged.   End   ---- */
                    break;
                case ER_D_COMM_QUE_NOT_INIT:
                    /* 6-3. */
                    ret_val = R_ICUMIF_ER_IF_LIB_NOT_INIT;
                    break;
                case ER_PARAM:
                case ER_D_COMM_QUE_NOT_ACCESS:
                default:
                    /* 6-4. */
                    ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                    break;
            }
        }
    }

    /* 8. */
    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_IF_0003]:[ICUMFW_UD_L_IF_0003]
* Function name: R_ICUMIF_ServiceResponse
* Description  : Acquires ISD from service response queue for self PE
* Arguments    : None
* Return Value : int32_t Service response reception process result
*                   0               : Successful
*                   Negative values : Interface library uninitialized
***************************************************************************************/
int32_t R_ICUMIF_ServiceResponse(void)
{
    int32_t         ret_val = R_ICUMIF_ER_OK;
    uint32_t        call_comm_ret;
    uint32_t        exist_isd = 1UL;
    r_icumif_isd_t  *p_isd;

    /* Repeat process until there is no ISD in service response queue for self PE */
    while (1UL == exist_isd)
    {
        /* 1. Acquires ISD from service response queue for self PE */
        call_comm_ret = D_COMM_GetResponseISD(&p_isd);

        /* 2. According to result of acquires ISD,       */
        /*    execute respective corresponding processes */
        switch (call_comm_ret)
        {
            case ER_OK:
            case ER_D_COMM_EXIST_ISD:
                /* 3-1. address check using Cluster RAM area check function */
                /* 3-1-1. For detect that ICU-M could not update ISD,          */
                /*          check value of service process state in acquired ISD */
                if (SERV_NEW_REQUEST == p_isd->service_result)
                {
                    /* 3-1-1-1. ICU-M could not update ISD */
                    p_isd->service_result = SERV_ISD_NOT_ACCESSIBLE;
                }
                /* ---- These lines should be left unchanged.   Start ---- */
                /* 3-1-2. */
                if (NULL != p_isd->p_callbackfunc)
                {
                    /* 3-1-2-1. If there is registration of callback function, */
                    /*            calling callback function                      */
                    p_isd->p_callbackfunc(p_isd);
                }
                /* ---- These lines should be left unchanged.   End   ---- */
                if (ER_OK == call_comm_ret)
                {
                    exist_isd = 0UL;
                }
                else
                {
                    /* do nothing */
                }
                break;
            case ER_D_COMM_NO_ISD:
                /* 3-2. No ISD in service response queue, exit from loop process */
                exist_isd = 0UL;
                break;
            case ER_D_COMM_QUE_NOT_INIT:
                /* 3-3. */
                ret_val = R_ICUMIF_ER_IF_LIB_NOT_INIT;
                break;
            case ER_D_COMM_QUE_NOT_ACCESS:
            default:
                /* 3-4. */
                ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                break;
        }
    }

    /* 4. */
    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_IF_0004]:[ICUMFW_UD_L_IF_0004]
* Function name: R_ICUMIF_IsServiceCompleted
* Description  : Check state of service execution requested to ICU-M
* Arguments    : r_icumif_isd_t *p_ISD : Pointer to ISD that service execution requested
* Return Value : int32_t Service execution state
*                   0            : Service running
*                   Other than 0 : Service execution completion
***************************************************************************************/
int32_t R_ICUMIF_IsServiceCompleted(r_icumif_isd_t *p_ISD)
{
    int32_t ret_val;
    uint8_t mem_check = ICUM_COMM_MEMCHECK_OK;

    /* 1. address check using Cluster RAM area check function */
    if (0UL != ((uint32_t)((volatile void *)p_ISD) & ALIGNMENT_4BYTE_CHECK_PATTERN))
    {
        ret_val = R_ICUMIF_ER_PARAM_ALIGNMENT;
    }
    else
    {
        if ((p_ISD->service_id == SERVICE_01_AES_CMAC) && (p_ISD->service_priority == R_ICUMIF_SERV_PRIORITY_HIGHEST))
        {
            /* Do nothing */
        }
        else
        {
            (void)D_COMM_GetICUMemCheckErrInfo(&mem_check);
        }

        if(mem_check == ICUM_COMM_MEMCHECK_OK)
        {
            /* ---- These lines should be left unchanged.   Start ---- */
            /* 3. Check service request / response queue transition state in ISD */
            ret_val = R_ICUMIF_RTN_SERV_RUNNING;

            /* 4. Service execution completion */
            switch (p_ISD->req_res_status)
            {
                case R_ICUMIF_REQRES_ICUM_RES_ENQUE:
                    if(p_ISD->res_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT)
                    {
                        ret_val = R_ICUMIF_RTN_SERV_COMPLETION;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                    break;
                case R_ICUMIF_REQRES_PE_RES_DEQUE:
                    /* 4-1. Check value of service process state in ISD */
                    if (SERV_REQUEST > p_ISD->service_result)
                    {
                        /* 4-1-1. Service execution completion */
                        ret_val = R_ICUMIF_RTN_SERV_COMPLETION;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                    break;
                case R_ICUMIF_REQRES_PE_REQ_ENQUE:
                    if(p_ISD->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT)
                    {
                        ret_val = R_ICUMIF_RTN_SERV_COMPLETION;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                    break;
                case R_ICUMIF_REQRES_ICUM_REQ_DEQUE:
                    /* 4-2. Service running */
                    /* Initial value */
                    break;
                default:
                    /* 4-3. */
                    ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                    break;
            }
        }
        else
        {
            ret_val = R_ICUMIF_ER_ADDR_NOT_G_RAM;
        }
        /* ---- These lines should be left unchanged.   End   ---- */
    }

    /* 6. */
    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_IF_0005]:[ICUMFW_UD_L_IF_0005]
* Function name: R_ICUMIF_GetStatus
* Description  : Acquire address value of status information of ICU-M Firmware
* Arguments    : None
* Return Value : r_icumif_sts_t * Address value of ICU-M Firmware status information
***************************************************************************************/
r_icumif_sts_t *R_ICUMIF_GetStatus(void)
{
    /* ---- These lines should be left unchanged.   Start ---- */
    r_icumif_sts_t  *p_status;

    /* 1. Acquire address value of status information of ICU-M Firmware */
    p_status = D_COMM_GetICUStatusAddr();

    /* 2. */
    return (p_status);
    /* ---- These lines should be left unchanged.   End   ---- */
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_IF_0006]:[ICUMFW_UD_L_IF_0006]
* Function name: R_ICUMIF_SetFlashAccessStatus
* Description  : Set access status of specified Flash type as PE status information
* Arguments    : r_icumif_flash_type_t    flash_type : Flash type to set access state
*                r_icumif_access_status_t status     : Access status to set
* Return Value : int32_t Access status setting result
*                   0               : Successful
*                   Negative values : Parameter error
***************************************************************************************/
int32_t R_ICUMIF_SetFlashAccessStatus(r_icumif_flash_type_t flash_type,
                                      r_icumif_access_status_t status)
{
    /* ---- These lines should be left unchanged.   Start ---- */
    int32_t     ret_val = R_ICUMIF_ER_OK;
    uint32_t    call_comm_ret;

    /* 1. */
    switch (flash_type)
    {
        case E_ICUMIF_FLASH_TYPE_DF:
            /* 1-1. */
            switch (status)
            {
                case E_ICUMIF_ACCESS_OFF:
                    /* 1-1-1. */
                    /* Set the value corresponding of during non-access   */
                    /* to the Data Flash access state clear disable state */
                    /* of the PE status information                       */
                    call_comm_ret = D_COMM_SetPEStatusInfo(ICUM_COMM_INF_PE_ST_FACIACCDIS, 0U);
                    /* According to result of PE status information set    */
                    /* process, execute respective corresponding processes */
                    if (ER_OK != call_comm_ret)
                    {
                        /* 1-1-1-1. */
                        ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                    }
                    break;
                case E_ICUMIF_ACCESS_ON:
                    /* 1-1-2. */
                    /* Set the value corresponding of during access to  */
                    /* the Data Flash access status clear disable state */
                    /* of the PE status information                     */
                    call_comm_ret = D_COMM_SetPEStatusInfo(ICUM_COMM_INF_PE_ST_FACIACCDIS, 1U);
                    /* According to result of PE status information set    */
                    /* process, execute respective corresponding processes */
                    if (ER_OK != call_comm_ret)
                    {
                        /* 1-1-2-1. */
                        ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                    }
                    break;
                default:
                    /* 1-1-3. */
                    ret_val = R_ICUMIF_ER_PARAM_RANGE;
                    break;
            }
            break;
        case E_ICUMIF_FLASH_TYPE_RAM:
            /* 1-2. */
            switch (status)
            {
                case E_ICUMIF_ACCESS_OFF:
                    /* 1-2-1. */
                    /* Set the value corresponding of during non-access to    */
                    /* the Flash operation state clear disable state          */
                    /* of the PE status information.                          */
                    call_comm_ret = D_COMM_SetPEStatusInfo(ICUM_COMM_INF_PE_ST_FLS_OPRN, 0U);
                    /* According to result of PE status information set       */
                    /* process, execute respective corresponding processes.   */
                    if (ER_OK != call_comm_ret)
                    {
                        /* 1-2-1-1. */
                        ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                    }
                    else
                    {
                        /* 1-2-1-2. */
                        /* Do nothing */
                    }
                    break;
                case E_ICUMIF_ACCESS_ON:
                    /* 1-2-2. */
                    /* Set the value corresponding of during access to        */
                    /* the Flash operation status clear disable state         */
                    /* of the PE status information.                          */
                    call_comm_ret = D_COMM_SetPEStatusInfo(ICUM_COMM_INF_PE_ST_FLS_OPRN, 1U);
                    /* According to result of PE status information set       */
                    /* process, execute respective corresponding processes.   */
                    if (ER_OK != call_comm_ret)
                    {
                        /* 1-2-2-1. */
                        ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                    }
                    else
                    {
                        /* 1-2-2-2. */
                        /* Do nothing */
                    }
                    break;
                default:
                    /* 1-2-3. */
                    ret_val = R_ICUMIF_ER_PARAM_RANGE;
                    break;
            }
            break;
        case E_ICUMIF_FLASH_TYPE_DF_PRG:
            /* 1-3. */
            switch (status)
            {
                case E_ICUMIF_ACCESS_OFF:
                    /* 1-3-1. */
                    /* Set the value corresponding of during non-access to    */
                    /* the Data Flash Programming state clear disable state   */
                    /* of the PE status information.                          */
                    call_comm_ret = D_COMM_SetPEStatusInfo(ICUM_COMM_INF_PE_ST_FLS_DF_PRG, 0U);
                    /* According to result of PE status information set       */
                    /* process, execute respective corresponding processes.   */
                    if (ER_OK != call_comm_ret)
                    {
                        /* 1-3-1-1. */
                        ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                    }
                    else
                    {
                        /* 1-3-1-2. */
                        /* Do nothing */
                    }
                    break;
                case E_ICUMIF_ACCESS_ON:
                    /* 1-3-2. */
                    /* Set the value corresponding of during access to        */
                    /* the Data Flash Programming status clear disable state  */
                    /* of the PE status information.                          */
                    call_comm_ret = D_COMM_SetPEStatusInfo(ICUM_COMM_INF_PE_ST_FLS_DF_PRG, 1U);
                    /* According to result of PE status information set       */
                    /* process, execute respective corresponding processes.   */
                    if (ER_OK != call_comm_ret)
                    {
                        /* 1-3-2-1. */
                        ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                    }
                    else
                    {
                        /* 1-3-2-2. */
                        /* Do nothing */
                    }
                    break;
                default:
                    /* 1-3-3. */
                    ret_val = R_ICUMIF_ER_PARAM_RANGE;
                    break;
            }
            break;
        case E_ICUMIF_FLASH_TYPE_CF_PRG:
            /* 1-4. */
            switch (status)
            {
                case E_ICUMIF_ACCESS_OFF:
                    /* 1-4-1. */
                    /* Set the value corresponding of during non-access to    */
                    /* the Code Flash Programming state clear disable state   */
                    /* of the PE status information.                          */
                    call_comm_ret = D_COMM_SetPEStatusInfo(ICUM_COMM_INF_PE_ST_FLS_CF_PRG, 0U);
                    /* According to result of PE status information set       */
                    /* process, execute respective corresponding processes.   */
                    if (ER_OK != call_comm_ret)
                    {
                        /* 1-3-1-1. */
                        ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                    }
                    else
                    {
                        /* 1-3-1-2. */
                        /* Do nothing */
                    }
                    break;
                case E_ICUMIF_ACCESS_ON:
                    /* 1-4-2. */
                    /* Set the value corresponding of during access to        */
                    /* the Code Flash Programming status clear disable state  */
                    /* of the PE status information.                          */
                    call_comm_ret = D_COMM_SetPEStatusInfo(ICUM_COMM_INF_PE_ST_FLS_CF_PRG, 1U);
                    /* According to result of PE status information set       */
                    /* process, execute respective corresponding processes.   */
                    if (ER_OK != call_comm_ret)
                    {
                        /* 1-4-2-1. */
                        ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                    }
                    else
                    {
                        /* 1-4-2-2. */
                        /* Do nothing */
                    }
                    break;
                default:
                    /* 1-4-3. */
                    ret_val = R_ICUMIF_ER_PARAM_RANGE;
                    break;
            }
            break;
        default:
            /* 1-5. */
            ret_val = R_ICUMIF_ER_PARAM_RANGE;
            break;
    }

    /* 2. */
    return (ret_val);
    /* ---- These lines should be left unchanged.   End   ---- */
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_IF_0007]:[ICUMFW_UD_L_IF_0007]
* Function name: R_ICUMIF_NotifyFlashInterrupt
* Description  : Perform specified Flash-related interrupt notification to ICU-M
* Arguments    : r_icumif_notify_type_t notify_type : Type of interrupt notification
* Return Value : int32_t Interrupt notification result
*                   0               : Successful
*                   Negative values : Parameter error
***************************************************************************************/
int32_t R_ICUMIF_NotifyFlashInterrupt(r_icumif_notify_type_t notify_type)
{
    /* ---- These lines should be left unchanged.   Start ---- */
    int32_t                     ret_val = R_ICUMIF_ER_OK;
    uint32_t                    call_comm_ret;
    icum_comm_int_to_channel_t  channel;

    /* 1. */
    switch (notify_type)
    {
        case E_ICUMIF_NOTIFY_CF_PRG_ACK:
        case E_ICUMIF_NOTIFY_DF_PRG_ACK:
        case E_ICUMIF_NOTIFY_CF_PRG_NACK:
        case E_ICUMIF_NOTIFY_DF_PRG_NACK:
        case E_ICUMIF_NOTIFY_OPERATION_END:
            /* 1-1. */
            /* Execute an interrupt issuing process to                             */
            /* issue interrupt of channel number corresponding to each notify_type */
            channel = (1UL << (uint32_t)notify_type);
            call_comm_ret = D_COMM_TriggerInterruptToICUP(channel, NULL);
            switch (call_comm_ret)
            {
                case ER_OK:
                    /* 1-1-1. Do nothing */
                    break;
                case ER_PARAM:
                default:
                    /* 1-1-2. */
                    ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                    break;
            }
            break;
        default:
            /* 1-2. */
            ret_val = R_ICUMIF_ER_PARAM_RANGE;
            break;
    }

    /* 3. */
    return (ret_val);
    /* ---- These lines should be left unchanged.   End   ---- */
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_IF_0008]:[ICUMFW_UD_L_IF_0008]
* Function name: R_ICUMIF_SetSystemCallBackFunc
* Description  : Register the function to call back when receiving system interrupt
* Arguments    : R_ICUMIF_CB_REGIST_t *p_regist_info : Pointer to registration info
* Return Value : int32_t Callback registration result
*                   0               : Successful
*                   Negative values : Parameter error
***************************************************************************************/
int32_t R_ICUMIF_SetSystemCallBackFunc(R_ICUMIF_CB_REGIST_t *p_regist_info)
{
    int32_t             ret_val = R_ICUMIF_ER_OK;
    uint32_t            call_comm_ret;
    alignment_check_t   prm_ptr;

    /* 1. Parameter alignment check ( p_regist_info ) */
    if (0UL != ((uint32_t)((volatile void *)p_regist_info) & ALIGNMENT_4BYTE_CHECK_PATTERN))
    {
        ret_val = R_ICUMIF_ER_PARAM_ALIGNMENT;
    }
    else
    {
        /* 2. Parameter alignment check ( callback function address ) */
        prm_ptr.p_align_check = p_regist_info->p_func_addr;
        if (0UL != (prm_ptr.val_align_check & ALIGNMENT_2BYTE_CHECK_PATTERN))
        {
            ret_val = R_ICUMIF_ER_PARAM_ALIGNMENT;
        }
        else
        {
            /* 3. Register the function to call back */
            call_comm_ret = D_COMM_SetSystemCallBackFunc(p_regist_info);

            /* 4. According to result of callback function regist process, */
            /*    execute respective corresponding processes               */
            switch (call_comm_ret)
            {
                case ER_OK:
                    /* 4-1. Do nothing */
                    break;
                default:
                    /* 4-2. */
                    ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                    break;
            }
        }
    }

    /* 5. */
    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_IF_0009]:[ICUMFW_UD_L_IF_0009]
* Function name: R_ICUMIF_SetFlashCallBackFunc
* Description  : Register the function to call back when receiving flash interrupt
* Arguments    : R_ICUMIF_CB_REGIST_t *p_regist_info : Pointer to registration info
* Return Value : int32_t Callback registration result
*                   0               : Successful
*                   Negative values : Parameter error
***************************************************************************************/
int32_t R_ICUMIF_SetFlashCallBackFunc(R_ICUMIF_CB_REGIST_t *p_regist_info)
{
    int32_t             ret_val = R_ICUMIF_ER_OK;
    uint32_t            call_comm_ret;
    alignment_check_t   prm_ptr;

    /* 1. Parameter alignment check ( p_regist_info ) */
    if (0UL != ((uint32_t)((volatile void *)p_regist_info) & ALIGNMENT_4BYTE_CHECK_PATTERN))
    {
        ret_val = R_ICUMIF_ER_PARAM_ALIGNMENT;
    }
    else
    {
        /* 2. Parameter alignment check ( callback function address ) */
        prm_ptr.p_align_check = p_regist_info->p_func_addr;
        if (0UL != (prm_ptr.val_align_check & ALIGNMENT_2BYTE_CHECK_PATTERN))
        {
            ret_val = R_ICUMIF_ER_PARAM_ALIGNMENT;
        }
        else
        {
            /* 3. Register the function to call back */
            call_comm_ret = D_COMM_SetFlashCallBackFunc(p_regist_info);

            /* 4. According to result of callback function regist process, */
            /*    execute respective corresponding processes               */
            switch (call_comm_ret)
            {
                case ER_OK:
                    /* 4-1. Do nothing */
                    break;
                default:
                    /* 4-2. */
                    ret_val = R_ICUMIF_ER_SOFTWARE_ERROR;
                    break;
            }
        }
    }

    /* 5. */
    return (ret_val);
}

#pragma ghs section text    = default
#pragma ghs section rodata  = default
