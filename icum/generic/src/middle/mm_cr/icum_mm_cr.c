/******************************************************************************/
/* Component Name   MM CR Manager file                                        */
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

/**********************************************************************************
* Headers
**********************************************************************************/

#include "r_icumif_api.h"
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_d_cr_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_ml_prng_pub.h"
/* For the get config of VM */
#include "icum_ml_cfg_pub.h"
#include "icum_mm_cr.h"
#include "icum_k_issd_pub.h"
/**********************************************************************************
* Global variables
***********************************************************************************/
#if defined(FSW_USE_CRAM)
#pragma ghs section bss = ".start.icum_cram.bss"
#endif
/* to trigger the C&R service */
static r_icumif_isd_t g_cr_internal_service;
#if defined(FSW_USE_CRAM)
#pragma ghs section bss = default
#endif

/* The state of Challenge & Response */
static uint16_t g_cr_state;
/* Save PE / ICUP request from either */
static uint8_t g_cr_pe_type;

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
#define EXT_DEBUGGER_CONNECT   0x00000001uL /* Confirmation constant of debugger connection state */

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_CrMNG_0005]:[ICUMFW_UD_MM_CrMNG_0005]
* Function name: mm_CR_CheckDebuggerConnection
* Description  : check debugger connection and set EXT_DEBUGGER
* Arguments    : (OUT) *p_result
* Return Value : ER_OK
*              : ER_PARAM
***************************************************************************************/
static uint32_t mm_CR_CheckDebuggerConnection(uint16_t *p_result)
{
    uint32_t debugger_status;
    uint32_t err;

    *p_result = CR_DBG_NOT_CONNECT;
    /* Ext debugger check */
    err = D_CR_GetDCS(&debugger_status);
    if(ER_OK == err)
    {
        if(EXT_DEBUGGER_CONNECT == debugger_status)
        {
            *p_result = CR_DBG_CONNECT;
        }
    }
    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_CrMNG_0006]:[ICUMFW_UD_MM_CrMNG_0006]
* Function name: mm_CR_MakeCrJob
* Description  : Register a memory C&R job
* Arguments    : none
* Return Value : ER_OK
***************************************************************************************/
static uint32_t mm_CR_MakeCrJob(void)
{
    sys_cfg_vm_t vm_cfg;

    /* Initialize local variable */
    L_Toolbox_MemSet((uint8_t *)&vm_cfg, 0x00u, (int16_t)sizeof(sys_cfg_vm_t));
    L_Toolbox_MemSet((uint8_t *)&g_cr_internal_service, 0, (int16_t)sizeof(g_cr_internal_service));

    (void)ML_CFG_GetSys(ML_CFG_SYS_VM, (uint32_t *)&vm_cfg);

    g_cr_internal_service.service_id       = ((r_icumif_service_id_t)((uint32_t)SERVICE_02_RAND_RND + 1));
    g_cr_internal_service.requester_id     = (r_icumif_requester_id_t)(GET_MY_PEID);
    g_cr_internal_service.vm_id            = vm_cfg.privilege_vm_id;
    g_cr_internal_service.p_callbackfunc   = NULL;
    g_cr_internal_service.service_result   = SERV_REQUEST;
    g_cr_internal_service.job_id           = 0u;
    g_cr_internal_service.service_priority = R_ICUMIF_SERV_PRIORITY_NORMAL;

    switch(g_cr_state)
    {
        case CR_INT_CHALLENGE_PE:
            g_cr_internal_service.prm.RAND_GENERATE.p_block_out   = (uint32_t*)CR_INT_CHALLENGE;
            g_cr_internal_service.prm.RAND_GENERATE.nb_blocks     = CR_INT_PE;
            break;
        case CR_INT_CHALLENGE_ICUP:
            g_cr_internal_service.prm.RAND_GENERATE.p_block_out   = (uint32_t*)CR_INT_CHALLENGE;
            g_cr_internal_service.prm.RAND_GENERATE.nb_blocks     = CR_INT_ICUP;
            break;
        case CR_INT_RESPONSE_PE:
            g_cr_internal_service.prm.RAND_GENERATE.p_block_out   = (uint32_t*)CR_INT_RESPONSE;
            g_cr_internal_service.prm.RAND_GENERATE.nb_blocks     = CR_INT_PE;
            break;
        case CR_INT_RESPONSE_ICUP:
            g_cr_internal_service.prm.RAND_GENERATE.p_block_out   = (uint32_t*)CR_INT_RESPONSE;
            g_cr_internal_service.prm.RAND_GENERATE.nb_blocks     = CR_INT_ICUP;
            break;
        default: 
            break;
    }
    g_cr_state = CR_JOB_REGISTERED;
    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_CrMNG_0004]:[ICUMFW_UD_MM_CrMNG_0004]
* Function name: mm_CR_ServiceRegister
* Description  : check PRNG initialize and 
* Arguments    : none
* Return Value : ER_OK
*              : ER_PARAM
***************************************************************************************/
static uint32_t mm_CR_ServiceRegister(void)
{
    uint32_t err;
    uint32_t rand_init;
    uint16_t debugger_status;

    /* Ext debugger check */
    err = mm_CR_CheckDebuggerConnection(&debugger_status);
    if((ER_OK == err) && (CR_DBG_CONNECT == debugger_status))
    {
        switch(g_cr_state)
        {
            case CR_INT_CHALLENGE_PE:
            case CR_INT_CHALLENGE_ICUP:
                /* Get Random Numer for Challenge Data */
                (void)ML_PRNG_IsInitialized(&rand_init);
                if(rand_init == ML_PRNG_INIT_COMPLETE)
                {
                    (void)mm_CR_MakeCrJob();
                    err = K_ISSD_GenerateServiceRequest(&g_cr_internal_service);
                }
                else
                {
                    err = ER_PARAM;
                }
                break;
            case CR_INT_RESPONSE_PE:
            case CR_INT_RESPONSE_ICUP:
                (void)mm_CR_MakeCrJob();
                err = K_ISSD_GenerateServiceRequest(&g_cr_internal_service);
                break;
            default: 
                err = ER_PARAM;
                break;
        }
    }
    else
    {
        err = ER_PARAM;
    }
    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_CrMNG_0001]:[ICUMFW_UD_MM_CrMNG_0001]
* Function name: MM_CR_Init
* Description  : Initialize the intermediate value area.
* Arguments    : none
* Return Value : ER_OK
***************************************************************************************/
uint32_t MM_CR_Init(void)
{
    L_Toolbox_MemSet((uint8_t *)&g_cr_internal_service, 0, (int16_t)sizeof(g_cr_internal_service));
    g_cr_state = CR_NONE;
    g_cr_pe_type = CR_INT_INIT;
    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_CrMNG_0002]:[ICUMFW_UD_MM_CrMNG_0002]
* Function name: MM_CR_ClearState
* Description  : Clear C&R state
*              : Called after s_20 implementation is completed
* Arguments    : none
* Return Value : ER_OK
***************************************************************************************/
uint32_t MM_CR_ClearState(void)
{
    g_cr_state = CR_NONE;
    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_CrMNG_0003]:[ICUMFW_UD_MM_CrMNG_0003]
* Function name: MM_CR_InitCompletionNotice
* Description  : Job registration decision at the completion of pseudorandom number initialization
*              : Called at init process completion on s_02 
* Arguments    : none
* Return Value : ER_OK
*              : ER_PARAM
***************************************************************************************/
uint32_t MM_CR_InitCompletionNotice(void)
{
    uint32_t err = ER_OK;
    
    if(g_cr_state != CR_JOB_REGISTERED)
    {
        if( g_cr_state == CR_INT_CHALLENGE_PE
        ||  g_cr_state == CR_INT_CHALLENGE_ICUP)
        {
            err = mm_CR_ServiceRegister();
        }
    }
    else
    {
        /* Do nothing*/
    }
    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_CrMNG_0007]:[ICUMFW_UD_MM_CrMNG_0007]
* Function name: INTEXTDCU20CDRQ
* Description  : ICUP Debug Authentication Challenge Data Request
* Arguments    : none
* Return Value : none
***************************************************************************************/
void INTEXTDCU20CDRQ(void)
{
    if(g_cr_state != CR_JOB_REGISTERED)
    {
        g_cr_state = CR_INT_CHALLENGE_ICUP;
        g_cr_pe_type = CR_INT_ICUP;
        (void)mm_CR_ServiceRegister();
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_CrMNG_0008]:[ICUMFW_UD_MM_CrMNG_0008]
* Function name: INTEXTDCU20CDRQPE
* Description  : CPU (PE) authentication challenge data request for debugging
* Arguments    : none
* Return Value : none
***************************************************************************************/
void INTEXTDCU20CDRQPE(void)
{
    if(g_cr_state != CR_JOB_REGISTERED)
    {
        g_cr_state = CR_INT_CHALLENGE_PE;
        g_cr_pe_type = CR_INT_PE;
        (void)mm_CR_ServiceRegister();
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_CrMNG_0009]:[ICUMFW_UD_MM_CrMNG_0009]
* Function name: INTEXTDCU20RDRQ
* Description  : ICUP / CPU (PE) Authentication response data comparison request for debugging
* Arguments    : none
* Return Value : none
***************************************************************************************/
void INTEXTDCU20RDRQ(void)
{
    if(g_cr_state != CR_JOB_REGISTERED)
    {
        if( g_cr_pe_type == CR_INT_PE )
        {
            g_cr_state = CR_INT_RESPONSE_PE;
            (void)mm_CR_ServiceRegister();
        }
        else if( g_cr_pe_type == CR_INT_ICUP )
        {
            g_cr_state = CR_INT_RESPONSE_ICUP;
            (void)mm_CR_ServiceRegister();
        }
        else
        {
            /* Impossible condition */
        }
    }
}
