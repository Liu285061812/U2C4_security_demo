/******************************************************************************/
/* Component Name   ICU-M COMM(PE) Driver                                     */
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
#include "icum_d_comm_pe.h"

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* #define FSW_FASTCMAC_FIXEDPE */

/**************************************************************************************/
/* Local variables                                                                    */
/**************************************************************************************/
/* Initialization state information */
static uint32_t icumif_initialized;

static ICUM_COMM_PE_INFO_t pe_info[TOTAL_PE_NUM] =
{
    { &INTC1CPU0EIC0, INTICUP_PESEL_PE0_MASK, INTICUP_PESEL_TO_PE0, ICUM_COMM_ICH_TO_SERV_FROM_PE0,
      PE0_QUE_INIT, PE0_INTC_INIT, PE0_ICU2PEI_INIT, INTICUP_PE0_POSITION}
   ,{ &INTC1CPU1EIC0, INTICUP_PESEL_PE1_MASK, INTICUP_PESEL_TO_PE1, ICUM_COMM_ICH_TO_SERV_FROM_PE1,
      PE1_QUE_INIT, PE1_INTC_INIT, PE1_ICU2PEI_INIT, INTICUP_PE1_POSITION}
#if TOTAL_PE_NUM > 2
    ,{ &INTC1CPU2EIC0, INTICUP_PESEL_PE2_MASK, INTICUP_PESEL_TO_PE2, ICUM_COMM_ICH_TO_SERV_FROM_PE2,
      PE2_QUE_INIT, PE2_INTC_INIT, PE2_ICU2PEI_INIT, INTICUP_PE2_POSITION}
#endif
};
/* Callback registration at interrupt occurrence */
static R_ICUMIF_CB_REGIST_t cb_regist_info = { NULL };
static R_ICUMIF_CB_REGIST_t cb_flash_regist_info = { NULL };

/**************************************************************************************/
/* Interface functions                                                                */
/**************************************************************************************/

#pragma ghs section text   = ".icumif_rom"
#pragma ghs section rodata = ".icumif.rodata"

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0003]:[ICUMFW_UD_S4_D_COMM_PE_0003]
* Function name: d_COMM_Enqueue_Request
* Description  : Enqueue specified ISD to service request queue
* Arguments    : r_icumif_isd_t *p_isd  : Address value of ISD to be passed to ICUP
*              : uint8_t *req_index     : Index of the request queue entry
* Return Value : uint32_t Enqueue processing result
*                   ER_OK              : Successful
*                   ER_D_COMM_QUE_FULL : Service request queue FULL
***************************************************************************************/
static uint32_t d_COMM_Enqueue_Request(r_icumif_isd_t *p_isd, uint8_t *req_index)
{
    /* ---- These lines should be left unchanged.   Start ---- */
    uint32_t            ret_val = ER_OK;
    volatile uint32_t   *p_reg;
    ptr_manip_t         queptr;
    uint32_t            *p_queptr;
    int32_t             next_idx;
    uint32_t            saved_psw;
    volatile uint32_t   lul_dummy;

    /* 2. For interrupt control between PEs, */
    /* Interrupt disable check */
    D_COMM_PE_DIS_INT_CHECK(saved_psw);

    /* 3. Lower 24 bits of ICUM_PE2ICUS register are acquire and upper 8 bits are */
    /*    complemented to generate top address of service request queue           */
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;
    p_reg += p_isd->requester_id;

    queptr.val = (*p_reg & QUE_ADDR_LOWER_24BITS) | QUE_ADDR_UPPER_8BITS_VALUE;
    p_queptr = (uint32_t *)queptr.p_ptr;

    /* Get the index of the next request queue entry */
    /* 4. Check entry after one of entry index of the service request queue */

    ret_val = ER_D_COMM_QUE_FULL;

    for (next_idx = 0; next_idx < REQUEST_QUEUE_LENGTH; next_idx++)
    {
        if (0U == (*(p_queptr + next_idx)))
        {
            /* 4-2. */
            /* Regist ISD to service request queue */
            *(p_queptr + next_idx) = (uint32_t)((volatile void *)p_isd);

            /* Set service request / response queue transition state in */
            /* ISD indicated by parameter p_isd                         */
            p_isd->req_res_status = R_ICUMIF_REQRES_PE_REQ_ENQUE;
            /* Dummy Read */
            lul_dummy = p_isd->req_res_status;
            /* Synchronize */
            D_COMM_SYNC_PE();

            /* prevent to become full the request queue. */
            if ((REQUEST_QUEUE_LENGTH - 1) == next_idx)
            {
                p_isd->req_nointerrupt = R_ICUMIF_REQRES_INTERRPUT;
                /* Dummy Read */
                lul_dummy = p_isd->req_nointerrupt;
                /* Synchronize */
                D_COMM_SYNC_PE();
            }
            else
            {
                /* Do nothing */
            }

            *req_index = next_idx;
            ret_val = ER_OK;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    /* 5. Interrupt enable check */
    D_COMM_PE_EN_INT_CHECK(saved_psw);

    /* 6. */
    return (ret_val);
    /* ---- These lines should be left unchanged.   End   ---- */
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0004]:[ICUMFW_UD_S4_D_COMM_PE_0004]
* Function name: d_COMM_PE_GetSemaphore
* Description  : Get the Semaphore register
* Arguments    : (IN)icum_comm_target_pe_t target_pe
*                    uint8_t request
* Return Value : ER_OK
*                PE_SEMAPHORE_ER_SYSTEM;
*                PE_SEMAPHORE_ER_TIMEOUT
***************************************************************************************/
static uint32_t d_COMM_PE_GetSemaphore(uint8_t target_pe, uint8_t request)
{
    register volatile uint32_t *p_reg;
    uint32_t uiAuth;
    uint32_t uiIcusem;
    uint32_t err;
    uint32_t uiCount;

    /* Get the Semaphore register address */
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;
    p_reg += target_pe;

    err = PE_SEMAPHORE_ER_TIMEOUT;
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

            if (PE_SEMAPHORE_AUTH_GET2ND != uiAuth)
            {
                /* Error case. retry to get the Semaphore */
                /* Do nothing */
            }
            else
            {
                /* Got the Semaphore */
                err = ER_OK;
                break;
            }
        }
        else if (PE_SEMAPHORE_AUTH_GET2ND == uiAuth)
        {
            /* Got the Semaphore */
            err = ER_OK;
            break;
        }
        else
        {
            /* Do nothing */
        }

        uiCount++;
    } while (uiCount < PE_SEMAPHORE_WAIT);

    /* Set Request bit in the Semaphore register */
    if ((ER_OK == err) && (PE_SEMAPHORE_ON == request))
    {
        *p_reg |= PE_SEMAPHORE_REQ;
    }
    else
    {
        /* Do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0005]:[ICUMFW_UD_S4_D_COMM_PE_0005]
* Function name: d_COMM_PE_FreeSemaphore
* Description  : Free the Semaphore register
* Arguments    : (IN)icum_comm_target_pe_t target_pe
*                    uint8_t response
* Return Value : ER_OK
***************************************************************************************/
static uint32_t d_COMM_PE_FreeSemaphore(uint8_t target_pe, uint8_t response)
{
    register volatile uint32_t *p_reg;
    uint32_t uiAuth;

    /* Get the Semaphore register address */
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;
    p_reg += target_pe;

    uiAuth   = (*p_reg & PE_SEMAPHORE_AUTH_MASK);

    /* Check the Semaphore register status */
    if (PE_SEMAPHORE_AUTH_GET2ND == uiAuth)
    {
        if (PE_SEMAPHORE_ON == response)
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

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0001]:[ICUMFW_UD_S4_D_COMM_PE_0001]
* Function name: D_COMM_PE_Init
* Description  : Initialize the COMM(PE) driver
* Arguments    : uint32_t *p_request_queue : Pointer to service request queue
* Return Value : uint32_t Driver initialization result
*                   ER_OK                      : Successful
*                   ER_D_COMM_PARAM_NOT_G_RAM  : Parameter is not shared RAM area
*                   ER_D_COMM_QUE_ALREADY_INIT : Service queue already initialization
*                   ER_D_COMM_QUE_NOT_ACCESS   : Can not get the Semaphore
*                   ER_D_COMM_MEMCHK_NG        : Invalid memory info
***************************************************************************************/
uint32_t D_COMM_PE_Init(uint32_t *p_request_queue)
{
    uint32_t            ret_val = ER_OK;
    volatile uint32_t   *p_reg;
    uint32_t            count;
    uint32_t            bit_position;
    ptr_manip_t         wkptr;
    uint32_t            my_peid;
    uint32_t            semaphore;
    uint32_t            saved_psw;

    /* 1. Memory information check ( Service queue size )    */
    /*    In the case of check NG, the process is terminated */
    my_peid = GET_MY_PEID & PEID_MASK;
    bit_position = pe_info[my_peid].pe_que_init;
    /* ---- These lines should be left unchanged.   Start ---- */
    /* 2. For interrupt control between PEs,                  */
    /*    Interrupt disable initialization state information */
    D_COMM_PE_DIS_INT_CHECK(saved_psw);
    /* 3. Check the "service queue initialization" state */
    /*    of the initialization state information        */
    if (INIT_UNPROCESSED == (0x00000001UL & (icumif_initialized >> bit_position)))
    {
        /* 3-1. Get the Semaphore to clear err bit in the Semaphore register */
        semaphore = d_COMM_PE_GetSemaphore((uint8_t)my_peid, PE_SEMAPHORE_OFF);

        if(ER_OK == semaphore)
        {
            /* 3-1-1. Get top address of service queue area from memory information */
            wkptr.p_uint32_ptr = p_request_queue;
            *(wkptr.p_vlt_ptr) = VALUE_FOR_RAM_AREA_CHECK;
            if (VALUE_FOR_RAM_AREA_CHECK == *(wkptr.p_vlt_ptr))
            {
                /* 3-1-1-1. */
                /* Set lower 24 bits of p_request_queue      */
                /* to lower 24 bits of ICUM_PE2ICUS register */
                p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;
                p_reg += my_peid;
                *p_reg = (*p_reg & ~QUE_ADDR_LOWER_24BITS) | 
                        ((uint32_t)((volatile void *)p_request_queue) & QUE_ADDR_LOWER_24BITS);

                /* Clear Error bit in the Semaphore register */
                *p_reg &= ~PE_SEMAPHORE_ERR;

                /* Clear queue area from address specified by p_request_queue */
                for (count = 0UL; count < TOTAL_QUEUE_LENGTH; count++)
                {
                    p_request_queue[count] = 0UL;
                }
                cb_regist_info.p_func_addr = NULL;
                /* The "service queue initialization" state is set to "processed" */
                icumif_initialized |= (uint32_t)(INIT_PROCESSED << bit_position);
                /* Synchronize Pipeline Macro */
                D_COMM_SYNC_PE();
            }
            else
            {
                ret_val = ER_D_COMM_PARAM_NOT_G_RAM;
            }

            /* Free the Semaphore */
            (void)d_COMM_PE_FreeSemaphore((uint8_t)my_peid, PE_SEMAPHORE_OFF);
#ifdef FSW_FASTCMAC_FIXEDPE
            if (PEID_PE0 == my_peid)
            {
                /* Set PEID status for INTPES interrupt */
                (void)D_COMM_SetPEStatusInfo(ICUM_COMM_INF_PE_ST_PEID, (uint8_t)my_peid);
            }
            else
            {
                /* do nothing */
            }
#endif /* FSW_FASTCMAC_FIXEDPE */
        }
        else
        {
            ret_val = ER_D_COMM_QUE_NOT_ACCESS;
        }
    }
    else
    {
        /* 3-2. */
        ret_val = ER_D_COMM_QUE_ALREADY_INIT;
    }

    /* 4. Interrupt enable initialization state information */
    /* 5. Unlock initialization state information */
    D_COMM_PE_EN_INT_CHECK(saved_psw);

    /* 5. */
    return (ret_val);
    /* ---- These lines should be left unchanged.   End   ---- */
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0002]:[ICUMFW_UD_S4_D_COMM_PE_0002]
* Function name: D_COMM_TriggerInterruptToICUP
* Description  : Issue an interrupt with channel number specified for ICUP
* Arguments    : icum_comm_int_to_channel_t channel : Bit pattern indicating ch number
*                r_icumif_isd_t             *p_isd  : Address value of ISD to ICUP
* Return Value : uint32_t Interrupt issue result
*                   ER_OK                  : Successful
*                   ER_PARAM               : Parameter error
*                   ER_D_COMM_QUE_NOT_INIT : Service queue initialization not yet
*                   ER_D_COMM_QUE_FULL     : Service request queue FULL
***************************************************************************************/
uint32_t D_COMM_TriggerInterruptToICUP(icum_comm_int_to_channel_t channel,
                                       r_icumif_isd_t *p_isd)
{
    /* ---- These lines should be left unchanged.   Start ---- */
    uint32_t            ret_val = ER_OK;
    uint32_t            call_func_ret;
    volatile uint32_t  *p_reg;
    volatile uint32_t  *p_single_reg;
    uint32_t            semaphore;
    uint32_t            bit_position;
    uint32_t            my_peid;
    uint8_t             index;

    p_single_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_PE2ICUSFS;

    my_peid = GET_MY_PEID & PEID_MASK;

    if (my_peid == TARGET_PE0)
    {
        p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_PE2ICUFS0;
    }
    else if (my_peid == TARGET_PE1)
    {
        p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_PE2ICUFS1;
    }
    else
    {
        p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_PE2ICUFS2;
    }
    index = QUEUE_INDEX_MAX;

    /* 2. */
    switch (channel)
    {
        case ICUM_COMM_ICH_TO_SERV_REQUEST:
            /* 2-1. Check the "service queue initialization" state of */
            /*      the initialization state information              */
            bit_position = pe_info[my_peid].pe_que_init;
            if (INIT_UNPROCESSED == (0x00000001UL & (icumif_initialized >> bit_position)))
            {
                /* 2-1-1. Service queue initialization not yet */
                ret_val = ER_D_COMM_QUE_NOT_INIT;
            }
            else
            {
                /* Get the Semaphore register to access the request queue*/
                semaphore = d_COMM_PE_GetSemaphore((uint8_t)my_peid, PE_SEMAPHORE_ON);

                if (ER_OK == semaphore)
                {
                    /* 2-1-2. */
                    /* Enqueue ISD specified by parameter p_isd to service request queue */
                    call_func_ret = d_COMM_Enqueue_Request(p_isd, &index);
                    if (ER_OK == call_func_ret)
                    {
                        if (R_ICUMIF_REQRES_INTERRPUT == p_isd->req_nointerrupt)
                        {
                            if ((QUEUE_INDEX_FIRST == index) &&
                                (SERVICE_01_AES_CMAC == p_isd->service_id) && 
                                ((r_icumif_service_priority_t)R_ICUMIF_SERV_PRIORITY_HIGHEST ==
                                p_isd->service_priority))
                            {
#if !defined(FSW_FASTCMAC_FIXEDPE)
                                /* Set PEID status for INTPES interrupt */
                                (void)D_COMM_SetPEStatusInfo(ICUM_COMM_INF_PE_ST_PEID, (uint8_t)my_peid);
#endif /* FSW_FASTCMAC_FIXEDPE */
                                /* Issue service execution request single interrupt */
                                *p_single_reg = ICUM_COMM_ICH_TO_SINGLE_INTERRUPT;
                            }
                            else
                            {
                                /* 2-1-2-1. Issue service execution request interrupt */
                                *p_reg = pe_info[my_peid].inticup_reqflg;
                            }
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    else
                    {
                    /* 2-1-2-2. Service request queue FULL */
                        ret_val = call_func_ret;
                    }
                    /* Free the Semaphore */
                    (void)d_COMM_PE_FreeSemaphore((uint8_t)my_peid, PE_SEMAPHORE_OFF);
                }
                else
                {
                    ret_val = ER_D_COMM_QUE_NOT_ACCESS;
                }
            }
            break;
        case ICUM_COMM_ICH_TO_CF_PRG_NOTIFY:
        case ICUM_COMM_ICH_TO_DF_PRG_NOTIFY:
        case ICUM_COMM_ICH_TO_CF_PRG_ERROR:
        case ICUM_COMM_ICH_TO_DF_PRG_ERROR:
        case ICUM_COMM_ICH_TO_OPERATION_END:
            /* 2-2. Issue an interrupt with specified channel number */
            *p_reg = channel;
            break;
        default:
            /* 2-3. */
            ret_val = ER_PARAM;
            break;
    }

    /* 3. */
    return (ret_val);
    /* ---- These lines should be left unchanged.   End   ---- */
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0006]:[ICUMFW_UD_S4_D_COMM_PE_0006]
* Function name: D_COMM_GetResponseISD
* Description  : Dequeue ISD from head of the service response queue for its self PE
* Arguments    : r_icumif_isd_t **p_isd : The store address of ISD
* Return Value : uint32_t ISD acquire result
*                   ER_OK                  : Successful
*                   ER_D_COMM_QUE_NOT_INIT : Service queue initialization not yet
*                   ER_D_COMM_NO_ISD       : There is no ISD in service response queue
***************************************************************************************/
uint32_t D_COMM_GetResponseISD(r_icumif_isd_t **p_isd)
{
    /* ---- These lines should be left unchanged.   Start ---- */
    uint32_t            ret_val = ER_OK;
    volatile uint32_t   *p_reg;
    ptr_manip_t         queptr;
    uint32_t            *p_queptr;
    int32_t             next_idx;
    ptr_manip_t         isdptr;
    uint32_t            my_peid;
    uint32_t             semaphore;
    uint32_t            bit_position;
    uint32_t            saved_psw;
    volatile uint32_t   lul_dummy;

    /* 2. Check the "service queue initialization" state of */
    /*    the initialization state information              */
    my_peid = GET_MY_PEID & PEID_MASK;
    bit_position = pe_info[my_peid].pe_que_init;
    if (INIT_UNPROCESSED == (0x00000001UL & (icumif_initialized >> bit_position)))
    {
        /* 2-1. Service queue initialization not yet */
        ret_val = ER_D_COMM_QUE_NOT_INIT;
    }
    else
    {
        /* 2-2. */
        /* For interrupt control between PEs,       */
        /* Interrupt disable of service response queue */
        D_COMM_PE_DIS_INT_CHECK(saved_psw);

        /* Get the Semaphore register to access the respnse queue */
        semaphore = d_COMM_PE_GetSemaphore((uint8_t)my_peid, PE_SEMAPHORE_OFF);

        if (ER_OK == semaphore)
        {
            p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;
            p_reg += my_peid;

            /* Lower 24 bits of Semaphore register are acquire and upper 8 bits */
            /* are complemented to generate top address of service request queue   */
            queptr.val = (*p_reg & QUE_ADDR_LOWER_24BITS) | QUE_ADDR_UPPER_8BITS_VALUE;
            p_queptr = (uint32_t *)queptr.p_ptr;

            ret_val = ER_D_COMM_NO_ISD;
            p_queptr += REQUEST_QUEUE_LENGTH;

            for (next_idx = 0; next_idx < RESPONSE_QUEUE_LENGTH; next_idx++)
            {
                /* Check entry after one of entry index of the service request queue */
                if (0UL != (*(p_queptr + next_idx)))
                {
                    if (ret_val != ER_OK)
                    {
                        /* 2-2-2. */
                        /* Acquire the address of ISD */
                        isdptr.val = *(p_queptr + next_idx);
                        *p_isd = (r_icumif_isd_t *)isdptr.p_ptr;
                        /* free the entry in service response queue */
                        *(p_queptr + next_idx) = 0UL;
                        /* Set service request / response queue transition state in acquired ISD */
                        (*p_isd)->req_res_status = R_ICUMIF_REQRES_PE_RES_DEQUE;
                        /* Dummy Read */
                        lul_dummy = (*p_isd)->req_res_status;
                        /* Synchronize */
                        D_COMM_SYNC_PE();
                        ret_val = ER_OK;
                    }
                    else
                    {
                        ret_val = ER_D_COMM_EXIST_ISD;
                        break;
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }

            /* when the error bit of the semaphore register is be set */
            if (PE_SEMAPHORE_ERR == (*p_reg & PE_SEMAPHORE_ERR))
            {
                /* error occurred */
                ret_val = ER_D_COMM_QUE_NOT_ACCESS;
                *p_reg &= ~PE_SEMAPHORE_ERR;
            }
            else
            {
                /* Do nothing */
            }

            /* Free the Semaphore */
            (void)d_COMM_PE_FreeSemaphore((uint8_t)my_peid, PE_SEMAPHORE_OFF);
        }
        else
        {
            ret_val = ER_D_COMM_QUE_NOT_ACCESS;
        }

        /* 2-3. Interrupt enable of service response queue */
        D_COMM_PE_EN_INT_CHECK(saved_psw);
    }

    /* 3. */
    return (ret_val);
    /* ---- These lines should be left unchanged.   End   ---- */
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0007]:[ICUMFW_UD_S4_D_COMM_PE_0007]
* Function name: D_COMM_GetICUStatusAddr
* Description  : Acquire address value of status information of ICU-M Firmware
* Arguments    : None
* Return Value : r_icumif_sts_t * Address value of ICU-M Firmware status information
***************************************************************************************/
r_icumif_sts_t *D_COMM_GetICUStatusAddr(void)
{
    /* ---- These lines should be left unchanged.   Start ---- */
    r_icumif_sts_t  *p_reg;
    ptr_manip_t     regptr;

    /* 1. Return the address value of ICUM_ICU2PES register as function return value */
    regptr.p_vlt_ptr = &ICUM_CMDREGICUM_ICU2PES0;
    p_reg = (r_icumif_sts_t *)regptr.p_ptr;

    return (p_reg);
    /* ---- These lines should be left unchanged.   End   ---- */
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0008]:[ICUMFW_UD_S4_D_COMM_PE_0008]
* Function name: D_COMM_SetPEStatusInfo
* Description  : Set status information of the specified PE
* Arguments    : icum_com_info_pe_status_t info      : Status information of PE to set
*                uint8_t                   set_value : Value of status information
* Return Value : uint32_t Status information setting result
*                   ER_OK    : Successful
*                   ER_PARAM : Parameter error
***************************************************************************************/
uint32_t D_COMM_SetPEStatusInfo(icum_com_info_pe_status_t info, uint8_t set_value)
{
    /* ---- These lines should be left unchanged.   Start ---- */
    uint32_t            ret_val = ER_OK;
    volatile uint32_t   *p_reg;
    uint32_t            bit_pattern;
    uint32_t            mask_pattern;
    uint32_t            saved_psw;
    volatile uint32_t   lul_dummy;
    uint32_t            my_peid;

    /* 2. For interrupt control between PEs,      */
    /*    Interrupt disable of PE state information */
    D_COMM_PE_DIS_INT_CHECK(saved_psw);

    my_peid = GET_MY_PEID & PEID_MASK;

    if (my_peid == TARGET_PE0)
    {
        p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_PE2ICUS0;
    }
    else if (my_peid == TARGET_PE1)
    {
        p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_PE2ICUS1;
    }
    else
    {
        p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_PE2ICUS2;
    }

    /* 3. */
    switch (info)
    {
        case ICUM_COMM_INF_PE_ST_FACIACCDIS:
            /* 3-1. */
            /* Generate bit pattern so that value of least significant bit of  */
            /* parameter set_value can be set as state of                      */
            /* inhibiting the data flash access state of PE status information */
            bit_pattern = ((INFO_PE_ST_FACIACCDIS_BIT & (uint32_t)set_value) << info) << INFO_PE_ST_POSITION;

            /* Set generate bit pattern to upper 8 bits of ICUM_PE2ICUS register */
            mask_pattern = ~(((uint32_t)(INFO_PE_ST_FACIACCDIS_BIT << info)) << INFO_PE_ST_POSITION);
            *p_reg = (*p_reg & mask_pattern) | bit_pattern;
            /* Dummy Read */
            lul_dummy = *p_reg;
            /* Synchronize */
            D_COMM_SYNC_PE();
            break;
        case ICUM_COMM_INF_PE_ST_FLS_OPRN:
            /* 3-2. */
            /* Generate bit pattern so that value of least significant bit of  */
            /* parameter set_value can be set as state of                      */
            /* inhibiting the data flash access state of PE status information */
            bit_pattern = ((INFO_PE_ST_FLS_OPRN_BIT & (uint32_t)set_value) << info) << INFO_PE_ST_POSITION;

            /* Set generate bit pattern to upper 8 bits of ICUM_PE2ICUS register */
            mask_pattern = ~(((uint32_t)(INFO_PE_ST_FLS_OPRN_BIT << info)) << INFO_PE_ST_POSITION);
            *p_reg = (*p_reg & mask_pattern) | bit_pattern;
            /* Dummy Read */
            lul_dummy = *p_reg;
            /* Synchronize */
            D_COMM_SYNC_PE();
            break;
        case ICUM_COMM_INF_PE_ST_PEID:
            /* 3-3. */
            /* Generate bit pattern so that value of least significant bit of  */
            /* parameter set_value can be set PEID information for INTPES interrupt */
            bit_pattern = (PEID_MASK & (uint32_t)set_value);

            /* Set generate bit pattern to upper 8 bits of ICUM_PE2ICUS register */
            mask_pattern = ~(PEID_MASK);
            *p_reg = (*p_reg & mask_pattern) | bit_pattern;
            /* Dummy Read */
            lul_dummy = *p_reg;
            /* Synchronize */
            D_COMM_SYNC_PE();
            break;
        case ICUM_COMM_INF_PE_ST_FLS_DF_PRG:
            /* 3-4. */
            /* Generate bit pattern so that value of least significant bit of  */
            /* parameter set_value can be set as state of data flash prograimming */
            bit_pattern = ((INFO_PE_ST_FLS_DF_PRG_BIT & (uint32_t)set_value) << info) << INFO_PE_ST_POSITION;

            /* Set generate bit pattern to upper 8 bits of ICUM_PE2ICUS register */
            mask_pattern = ~(((uint32_t)(INFO_PE_ST_FLS_DF_PRG_BIT << info)) << INFO_PE_ST_POSITION);
            *p_reg = (*p_reg & mask_pattern) | bit_pattern;
            /* Dummy Read */
            lul_dummy = *p_reg;
            /* Synchronize */
            D_COMM_SYNC_PE();
            break;
        case ICUM_COMM_INF_PE_ST_FLS_CF_PRG:
            /* 3-5. */
            /* Generate bit pattern so that value of least significant bit of  */
            /* parameter set_value can be set as state of code flash prograimming */
            bit_pattern = ((INFO_PE_ST_FLS_CF_PRG_BIT & (uint32_t)set_value) << info) << INFO_PE_ST_POSITION;

            /* Set generate bit pattern to upper 8 bits of ICUM_PE2ICUS register */
            mask_pattern = ~(((uint32_t)(INFO_PE_ST_FLS_CF_PRG_BIT << info)) << INFO_PE_ST_POSITION);
            *p_reg = (*p_reg & mask_pattern) | bit_pattern;
            /* Dummy Read */
            lul_dummy = *p_reg;
            /* Synchronize */
            D_COMM_SYNC_PE();
            break;
        default:
            /* 3-4. */
            ret_val = ER_PARAM;
            break;
    }

    /* 4. Interrupt enable of PE state information */
    D_COMM_PE_EN_INT_CHECK(saved_psw);

    /* 5. */
    return (ret_val);
    /* ---- These lines should be left unchanged.   End   ---- */
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0009]:[ICUMFW_UD_S4_D_COMM_PE_0009]
* Function name: D_COMM_SetInterruptEnable
* Description  : Enable interrupt of ICUP --> PE (INTICUP)
* Arguments    : None
* Return Value : None
***************************************************************************************/
void D_COMM_SetInterruptEnable(void)
{
    uint32_t            bit_position;
    volatile uint32_t   *p_reg;
    volatile uint16_t   *p_eic_reg;
    uint32_t            pesel_mask;
    uint32_t            ie_position;
    uint32_t            ie_value;
    uint32_t            my_peid;
    ICUM_COMM_PE_INFO_t *p_pe_info;
    uint32_t            saved_psw;

    /* ---- These lines should be left unchanged.   Start ---- */
    /* 1. For interrupt control between PEs,                  */
    /*    Interrupt disable of initialization state information */
    D_COMM_PE_DIS_INT_CHECK(saved_psw);

    /* 2. Check the state of "INTC initialization" of self PE */
    /*    in initialization state information                 */
    my_peid = GET_MY_PEID & PEID_MASK;
    p_pe_info = &pe_info[my_peid];
    bit_position = (uint32_t)p_pe_info->pe_intc_init;
    p_eic_reg = (volatile uint16_t *)p_pe_info->intc1cpueic0;
    if (INIT_UNPROCESSED == (0x00000001UL & (icumif_initialized >> bit_position)))
    {
    /* ---- These lines should be left unchanged.   End   ---- */
        /* 2-1. */
        /* Set EIC21 (for INTICU2PE) register of INTC of its self PE */
        p_eic_reg += INTC_EIC_CH_INTICU2PE;
        *p_eic_reg = INTC_EIC_CT_LEVEL | INTC_EIC_RF_NO_INT | INTC_EIC_MK_INT_ENABLE | 
                     INTC_EIC_VM_TABLE | INTC_EIC_INT_PRI_5;

    /* ---- These lines should be left unchanged.   Start ---- */
        /* The "INTC initialization" state is set to "processed" */
        icumif_initialized |= (uint32_t)(INIT_PROCESSED << bit_position);
        /* Synchronize Pipeline Macro */
        D_COMM_SYNC_PE();
    }

    /* 3. Check the state of "ICU2PEI initialization" of self PE */
    /*    in initialization state information                    */
    bit_position = p_pe_info->pe_icu2pei_init;
    pesel_mask   = p_pe_info->inticup_pesel_pe_mask;
    ie_position  = p_pe_info->inticup_pe_position;
    if (INIT_UNPROCESSED == (0x00000001UL & (icumif_initialized >> bit_position)))
    {
        /* 3-1. */
        /* Set Interrupt enable */
        if (my_peid == TARGET_PE0)
        {
            p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_ICU2PEIE0;
        }
        else if (my_peid == TARGET_PE1)
        {
            p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_ICU2PEIE1;
        }
        else
        {
            p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_ICU2PEIE2;
        }
        ie_value = (1UL << ICUM_COMM_ICH_FM_SYS_INTERRUPT) | (1UL << ICUM_COMM_ICH_FM_SERV_RESPONSE) | 
                   (1UL << ICUM_COMM_ICH_FM_DF_PRG_REQ_0);
        *p_reg = (*p_reg & pesel_mask) | (ie_value << ie_position);

        /* The "ICU2PEI initialization" state is set to "processed" */
        icumif_initialized |= (uint32_t)(INIT_PROCESSED << bit_position);
        /* Synchronize Pipeline Macro */
        D_COMM_SYNC_PE();
    }
    /* 4. Interrupt enable of initialization state information */

    D_COMM_PE_EN_INT_CHECK(saved_psw);
    /* ---- These lines should be left unchanged.   End   ---- */
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0010]:[ICUMFW_UD_S4_D_COMM_PE_0010]
* Function name: D_COMM_SetSystemCallBackFunc
* Description  : Register the function to call back when receiving system interrupt
* Arguments    : R_ICUMIF_CB_REGIST_t *p_regist_info : Pointer to registration info
* Return Value : uint32_t Callback registration result
*                   ER_OK : Successful
***************************************************************************************/
uint32_t D_COMM_SetSystemCallBackFunc(R_ICUMIF_CB_REGIST_t *p_regist_info)
{
    /* 1. Save CallBack Function Address */
    cb_regist_info.p_func_addr = p_regist_info->p_func_addr;

    /* 2. */
    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0011]:[ICUMFW_UD_S4_D_COMM_PE_0011]
* Function name: D_COMM_SetFlashCallBackFunc
* Description  : Register the function to call back when receiving flash interrupt
* Arguments    : R_ICUMIF_CB_REGIST_t *p_regist_info : Pointer to registration info
* Return Value : uint32_t Callback registration result
*                   ER_OK : Successful
***************************************************************************************/
uint32_t D_COMM_SetFlashCallBackFunc(R_ICUMIF_CB_REGIST_t *p_regist_info)
{
    /* 1. Save CallBack Function Address */
    cb_flash_regist_info.p_func_addr = p_regist_info->p_func_addr;

    /* 2. */
    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0012]:[ICUMFW_UD_S4_D_COMM_PE_0012]
* Function name: D_COMM_GetICUMemCheckErrInfo
* Description  :
* Arguments    :
* Return Value :
***************************************************************************************/
uint32_t D_COMM_GetICUMemCheckErrInfo(uint8_t *p_error)
{
    volatile uint32_t   *p_reg;
    uint32_t            my_peid;
    uint32_t            ret_val = ER_OK;

    if(p_error != NULL)
    {
        /* get PE ID */
        my_peid = GET_MY_PEID & PEID_MASK;

        (void)d_COMM_PE_GetSemaphore((uint8_t)my_peid, PE_SEMAPHORE_OFF);

        /*  get semaphore register according to the PEID of the self PE */
        p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;
        p_reg += my_peid;

        /* when the error bit of the semaphore register is be set*/
        if (PE_SEMAPHORE_ERR == (*p_reg & PE_SEMAPHORE_ERR))
        {
            /* memory check error to be set in *p_error */
            *p_error = ICUM_COMM_MEMCHECK_ERROR;
            *p_reg &= ~PE_SEMAPHORE_ERR;
        }
        else
        {
            /* memory check OK to be set in *p_error */
            *p_error = ICUM_COMM_MEMCHECK_OK;
        }

        (void)d_COMM_PE_FreeSemaphore((uint8_t)my_peid, PE_SEMAPHORE_OFF);
    }
    else
    {
        ret_val = ER_PARAM;
    }

    return (ret_val);
}

/**************************************************************************************/
/* Interrupt handler functions                                                        */
/**************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0014]:[ICUMFW_UD_S4_D_COMM_PE_0014]
* Function name: d_COMM_Sys_Interrupt
* Description  : Execute process of system interrupt from ICU-M
* Arguments    : None
* Return Value : None
***************************************************************************************/
static void d_COMM_Sys_Interrupt(void)
{
    /* 1. Check registration state of function to call back */
    /*    when system interrupt is received                 */
    if(NULL != cb_regist_info.p_func_addr)
    {
        /* 1-1. Call registered callback function */
        (*cb_regist_info.p_func_addr)();
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0015]:[ICUMFW_UD_S4_D_COMM_PE_0015]
* Function name: d_COMM_Flash_Interrupt
* Description  : Execute process of flash-related interrupt from ICU-M
* Arguments    : None
* Return Value : None
***************************************************************************************/
static void d_COMM_Flash_Interrupt(void)
{
    volatile uint32_t   *p_reg;

    /* 1. Check registration state of function to call back */
    /*    when flash-related interrupt is received          */
    if(NULL != cb_flash_regist_info.p_func_addr)
    {
        /* 1-1. Call registered callback function */
        (*cb_flash_regist_info.p_func_addr)();
    }

    /* 2. Issues an interrupt according to the value of FACI0 LOCK */
    p_reg = (volatile uint32_t *)&FLMDFPLOCK;
    if (FLMD_FACI0_LOCK != (*p_reg & FLMD_FACI0_LOCK))
    {
        (void)D_COMM_TriggerInterruptToICUP(ICUM_COMM_ICH_TO_DF_PRG_NOTIFY, NULL);
    }
    else
    {
        (void)D_COMM_TriggerInterruptToICUP(ICUM_COMM_ICH_TO_DF_PRG_ERROR, NULL);
    }
}




/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0019]:[ICUMFW_UD_S4_D_COMM_PE_0019]
* Function name: D_COMM_IRQ_Handler
* Description  : Execute interrupt process of ICUP --> PE (INTICUP)
* Arguments    : uint32_t peid     : Identifier of the PE that accepted the interrupt
*                uint32_t priority : Priority identifier of the generated interrupt
* Return Value : None
***************************************************************************************/
#pragma ghs reference EIINT21
#pragma ghs interrupt(nonreentrant)
__interrupt void EIINT21(void)
{
    volatile uint32_t   *p_reg;
    uint32_t            work_icu2pef;
    uint32_t            i;
    uint32_t            my_peid;
    volatile uint32_t   lul_dummy;
    uint32_t            ie_position;


    /* 1. Dispatched processing with parameter priority              */
    /*    (It also serves as a check on whether it is a valid value) */
    my_peid = GET_MY_PEID & PEID_MASK;

    if (my_peid == TARGET_PE0)
    {
        p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_ICU2PEF0;
        work_icu2pef = *p_reg;
        p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_ICU2PEFC0;
    }
    else if (my_peid == TARGET_PE1)
    {
        p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_ICU2PEF1;
        work_icu2pef = *p_reg;
        p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_ICU2PEFC1;
    }
    else
    {
        p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_ICU2PEF2;
        work_icu2pef = *p_reg;
        p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_ICU2PEFC2;
    }

    /* Clear interrupt factor of the non set PESELs */
    if (0x00000000UL != (work_icu2pef & INTICUP_PESEL_NO_PE_MASK))
    {
        *p_reg = (work_icu2pef & INTICUP_PESEL_NO_PE_MASK);
        work_icu2pef &= ~INTICUP_PESEL_NO_PE_MASK;
    }

    /* Acquire necessary bit of ICUM_ICU2PEF register according to PEID */
    ie_position = pe_info[my_peid].inticup_pe_position;
    work_icu2pef >>= ie_position;
    /* 2. Check value of bits 0 to 15 of work_icu2pef bit by bit in order */

    /*    to determine occurred interrupt                                 */
    for (i = 0U; ((0U != work_icu2pef) && (INTICUP_EACH_PE_BIT_WIDTH > i)); i++)
    {
        /* 2-1. Check the value of least significant bit of work_icu2pef */
        if (0x00000001UL == (0x00000001UL & work_icu2pef))
        {
            /* 2-1-1. */
            /* Clear interrupt factor */
            *p_reg = ((1UL << i) << ie_position);
            lul_dummy = *p_reg;
            /* Synchronize */
            D_COMM_SYNC_PE();

            switch (i)
            {
                case ICUM_COMM_ICH_FM_SYS_INTERRUPT:
                    /* 2-1-1-1. Execute system interrupt handling process */
                    d_COMM_Sys_Interrupt();
                    break;
                case ICUM_COMM_ICH_FM_SERV_RESPONSE:
                    /* 2-1-1-2. Execute service execution response interrupt */
                    /*          handling process                             */
                    (void)R_ICUMIF_ServiceResponse();
                    break;
                case ICUM_COMM_ICH_FM_DF_PRG_REQ_0:
                    /* 2-1-1-3. Execute flash-related interrupt handling process */
                    d_COMM_Flash_Interrupt();
                    break;
                default:
                    /* 2-1-1-4. Do nothing */
                    break;
            }
        }

        /* 2-2. Update work_icu2pef right shifted by 1 bit */
        work_icu2pef >>= 1U;
    }
}

#pragma ghs section text    = default
#pragma ghs section rodata  = default
