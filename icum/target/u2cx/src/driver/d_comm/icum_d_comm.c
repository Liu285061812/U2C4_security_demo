/******************************************************************************/
/* Component Name   ICUM COMM Driver                                          */
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
#include "icum_d_comm_reg.h"
#include "renesas_types.h"
#include "r_icumif_api.h"
#include "icum_common_pub.h"
#include "icum_int_declaration_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_d_comm_lcl.h"

/**************************************************************************************/
/* Global Variable                                                                    */
/**************************************************************************************/
CHANGE_SEC(rodata, ".icum_driver_const_ram")
static ICUM_COMM_ICUSTS_t s_Pe2icu_BitToOff[ICUM_COMM_ENTERMAX] =
{
    { ICUM_COMM_INF_ICU_ST_BUSY, ICUM_COMM_ICU_BUSY_OFF },
    { ICUM_COMM_INF_ICU_ST_SEC_BOOT, ICUM_COMM_ICU_SEC_BOOT_OFF },
    { ICUM_COMM_INF_ICU_ST_BOOT_INIT, ICUM_COMM_ICU_BOOT_INIT_OFF },
    { ICUM_COMM_INF_ICU_ST_BOOT_FIN, ICUM_COMM_ICU_BOOT_FIN_OFF },
    { ICUM_COMM_INF_ICU_ST_BOOT_OK, ICUM_COMM_ICU_BOOT_OK_OFF },
    { ICUM_COMM_INF_ICU_ST_RND_INIT, ICUM_COMM_ICU_RND_INIT_OFF },
    { ICUM_COMM_INF_ICU_ST_EXT_DEBUG, ICUM_COMM_ICU_EXT_DEBUG_OFF },
    { ICUM_COMM_INF_ICU_ST_INT_DEBUG, ICUM_COMM_ICU_INT_DEBUG_OFF },
    { ICUM_COMM_INF_ICU_ST_CLMAERR, ICUM_COMM_ICU_CLMAERR_OFF },
    { ICUM_COMM_INF_ICU_ST_BUSERR, ICUM_COMM_ICU_BUSERR_OFF },
    { ICUM_COMM_INF_ICU_ST_ROMERR, ICUM_COMM_ICU_ROMERR_OFF },
    { ICUM_COMM_INF_ICU_ST_RAMERR, ICUM_COMM_ICU_RAMERR_OFF },
    { ICUM_COMM_INF_ICU_ST_WDTERR, ICUM_COMM_ICU_WDTERR_OFF },
    { ICUM_COMM_INF_ICU_ST_SYSTEMERR, ICUM_COMM_ICU_SYSTEMERR_OFF },
    { ICUM_COMM_INF_ICU_ST_MAPERR, ICUM_COMM_ICU_MAPERR_OFF },
    { ICUM_COMM_INF_ICU_ST_EXTERR, ICUM_COMM_ICU_EXTERR_OFF },
    { ICUM_COMM_INF_ICU_ST_FCUERR, ICUM_COMM_ICU_FCUERR_OFF },
    { ICUM_COMM_INF_ICU_ST_RNDERR, ICUM_COMM_ICU_RNDERR_OFF },
    { ICUM_COMM_INF_ICU_ST_STARTUP, ICUM_COMM_ICU_STARTUP_OFF },
    { ICUM_COMM_INF_ICU_ST_TRNG, ICUM_COMM_ICU_TRNG_OFF },
    { ICUM_COMM_INF_ICU_ST_FLS_WAIT, ICUM_COMM_ICU_FLS_WAIT_OFF },
    { ICUM_COMM_INF_ICU_ST_FACIACC, ICUM_COMM_ICU_FACIACC_OFF },
    { ICUM_COMM_INF_ICU_ST_SERVAVAIL, ICUM_COMM_ICU_SERVAVAIL_OFF },
    { ICUM_COMM_INF_ICU_ST_FCUINIT, ICUM_COMM_ICU_FCUINIT_OFF },
    { ICUM_COMM_INF_ICU_ST_PLLSET, ICUM_COMM_ICU_PLLSET_OFF },
    { ICUM_COMM_INF_ICU_ST_STARTPE, ICUM_COMM_ICU_STARTPE_OFF }
};

/* Requester PE serch table */
static ICUM_COMM_REQUESTER_t s_ReqBittoRequester[TOTAL_PE_NUM] =
{
    {ICUM_COMM_ICH_FM_SERV_FROM_PE0, ICUM_COMM_TARGET_PE0},
    {ICUM_COMM_ICH_FM_SERV_FROM_PE1, ICUM_COMM_TARGET_PE1},
#if TOTAL_PE_NUM > 2
    {ICUM_COMM_ICH_FM_SERV_FROM_PE2, ICUM_COMM_TARGET_PE2},
#endif
};

/* Queue Index for memory check 
static uint8_t Mem_Check_Table[] = 
    {2U, 1U, 0U, 0U, 2U, 1U, 0U, 0U,
     0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
     8U, 7U, 6U, 5U, 4U, 3U, 2U, 1U,
     0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
     1U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
     0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
     0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
     0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U
    };*/
CHANGE_SEC(rodata, default)

/* Service Execution Request Interrupt Save callback */
static ICUM_COMM_CB_REGIST_t s_cb_isd_ServReq_t;
/* Code Flash Mode Switching ACK Notification Interrupt Save callback */
static ICUM_COMM_CB_REGIST_t s_cb_isd_CF_PrgNtfy_t;
/* Data Flash Mode Switching ACK Notification Interrupt Save callback */
static ICUM_COMM_CB_REGIST_t s_cb_isd_DF_PrgNtfy_t;
/* Code Flash mode switching NAK notification interrupt Save callback */
static ICUM_COMM_CB_REGIST_t s_cb_isd_CF_PrgErr_t;
/* Data Flash mode switching NAK notification interrupt Save callback */
static ICUM_COMM_CB_REGIST_t s_cb_isd_DF_PrgErr_t;
/* Flash access completion notification interrupt Save callback */
static ICUM_COMM_CB_REGIST_t s_cb_isd_OpeEnd_t;
/* Single Interrupt FastCMAC add job queue Save callback */
static ICUM_COMM_CB_REGIST_t s_cb_isd_AddJobQue_t;

/**************************************************************************************/
/* API module group                                                                   */
/**************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0001]:[ICUMFW_UD_U2B_D_COMM_0001]
* Function name: D_COMM_Init
* Description  : Init processing.
* Arguments    : none
* Return Value : none
***************************************************************************************/
void D_COMM_Init(void)
{

    /* Service Execution Request Interrupt Save callback Area Clear */
    s_cb_isd_ServReq_t.channel = 0UL;
    s_cb_isd_ServReq_t.p_cbfunc = NULL;

    /* Code Flash Mode Switching ACK Notification Interrupt Save callback Area Clear  */
    s_cb_isd_CF_PrgNtfy_t.channel = 0UL;
    s_cb_isd_CF_PrgNtfy_t.p_cbfunc = NULL;

    /* Data Flash Mode Switching ACK Notification Interrupt Save callback Area Clear  */
    s_cb_isd_DF_PrgNtfy_t.channel = 0UL;
    s_cb_isd_DF_PrgNtfy_t.p_cbfunc = NULL;

    /* Code Flash mode switching NAK notification interrupt Save callback Area Clear  */
    s_cb_isd_CF_PrgErr_t.channel = 0UL;
    s_cb_isd_CF_PrgErr_t.p_cbfunc = NULL;

    /* Data Flash mode switching NAK notification interrupt Save callback Area Clear  */
    s_cb_isd_DF_PrgErr_t.channel = 0UL;
    s_cb_isd_DF_PrgErr_t.p_cbfunc = NULL;

    /* Flash access completion notification interrupt Save callback */
    s_cb_isd_OpeEnd_t.channel = 0UL;
    s_cb_isd_OpeEnd_t.p_cbfunc = NULL;
    
    /* Single Interrupt FastCMAC add job queue Save callback */
    s_cb_isd_AddJobQue_t.channel = 0UL;
    s_cb_isd_AddJobQue_t.p_cbfunc = NULL;

    /* 2. Set the ICUM_PE 2 ICUIE register and        */
    /* activate the interrupt of PE -> ICUP (INTPE)   */
    /*  b1 : ICUM_COMM_ICH_FM_SERV_REQUEST  enable    */
    /*  b2 : ICUM_COMM_ICH_FM_CF_PRG_NOTIFY enable    */
    /*  b3 : ICUM_COMM_ICH_FM_DF_PRG_NOTIFY enable    */
    /*  b4 : ICUM_COMM_ICH_FM_CF_PRG_ERROR  enable    */
    /*  b5 : ICUM_COMM_ICH_FM_DF_PRG_ERROR  enable    */
    /*  b6 : ICUM_COMM_ICH_FM_OPERATION_END enable    */
    /*  ( Set disable (0) except the above Bit )      */

    /* Set the PE2ICUFSA register            */
    REG_ICUM_CMDREG.PE2ICUFSA0 = ICUM_COMM_PE2ICUFSA0_SET;
    REG_ICUM_CMDREG.PE2ICUFSA1 = ICUM_COMM_PE2ICUFSA1_SET;
    /* Set the PE2ICUSA register            */
    REG_ICUM_CMDREG.PE2ICUSA0  = ICUM_COMM_PE2ICUSA0_SET;
    REG_ICUM_CMDREG.PE2ICUSA1  = ICUM_COMM_PE2ICUSA1_SET;
    /* Set the PE2ICUIE register            */
    REG_ICUM_CMDREG.PE2ICUIE0  = ICUM_COMM_PE2ICUIE_SET;
    REG_ICUM_CMDREG.PE2ICUIE1  = ICUM_COMM_PE2ICUIE_SET;

    /* 3. Set the ICUM_PE 2 ICUSIE register and        */
    /* Set the PE2ICUSFSA register            */
    REG_ICUM_CMDREG.PE2ICUSFSA = ICUM_COMM_PE2ICUSFSA_SET;
    /* Set the PE2ICUSIE register             */
    REG_ICUM_CMDREG.PE2ICUSIE  = ICUM_COMM_ICH_FM_SINGLE_INTERRUPT;

    return;
}

CHANGE_SEC(text, ".icum_driver_ram")
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0002]:[ICUMFW_UD_U2B_D_COMM_0002]
* Function name: D_COMM_TriggerInterruptToPE
* Description  : Trigger Interrupt To PE.
* Arguments    : (IN)icum_comm_target_pe_t destination, 
*                    icum_comm_int_to_channel_t channel, 
*                    r_icumif_isd_t *p_isd
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_COMM_QUE_FULL
***************************************************************************************/
uint32_t D_COMM_TriggerInterruptToPE(icum_comm_target_pe_t destination, icum_comm_int_to_channel_t channel, 
                                     r_icumif_isd_t *p_isd)
{
    volatile uint32_t *p_reg;
    uint32_t uiRet = ER_OK;
    uint32_t uiSelPe;
    uint32_t uiSetbit = (uint32_t)1U;
    uint32_t saved_psw;
    uint8_t  res_interrput = ICUM_COMM_ON;

    /* 1. Check range of input parameters (destination) */
    if (((int32_t)MISC_PEID_PE0 <= (int32_t)destination) &&
        ((int32_t)TOTAL_PE_NUM > (int32_t)destination))
    {
        /* 2. Check the range of the input parameter (channel) */
        if ((ICUM_COMM_ICH_TO_SYS_INTERRUPT <= channel) && (ICUM_COMM_ICH_TO_SERV_MAX >= channel))
        {
            /* 3. When the input parameter is a service execution response */
            if (ICUM_COMM_ICH_TO_SERV_RESPONSE == channel)
            {
                /* 3-1. Address check of input parameter (p_isd) Null & Alignment */
                if ((NULL != p_isd) && (0UL == ((uint32_t)((volatile void *)p_isd) & ICUM_COMM_ALIGNMENT_4BYTE)))
                {
                    /* 3-2. Interrupt status check is performed         */
                    /*      and interrupt is enabled Interrupt disabled */
                    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

                    /* Get the Semaphore to access the response queue */
                    (void)d_COMM_GetSemaphore(destination, ICUM_COMM_ON);

                    /* 3-3. Set the service request in the response queue */
                    (void)d_COMM_SetServiceque(destination, p_isd, ICUM_COMM_ON);

                    /* Free the Semaphore */
                    (void)d_COMM_FreeSemaphore(destination, ICUM_COMM_OFF);
                    /* 3-4. Based on the acquired interrupt state,         */
                    /*      interrupt is enabled when interrupt is enabled */
                    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
                    
                    if (p_isd->res_nointerrupt == R_ICUMIF_REQRES_INTERRPUT)
                    {
                         /* Do nothing */
                    }
                    else
                    {
                         res_interrput = ICUM_COMM_OFF;
                    }
                }
                else
                {
                    /* 3-1-1. Specified address invalid */
                    uiRet = (uint32_t)ER_PARAM;
                }
            }
            else
            {
                /* Do nothing */
            }

            /* 3-5. Determination of execution result of d_COMM_SetServiceque */
            if ((uint32_t)ER_OK == uiRet)
            {
                /* 4. Write the input parameter to the ICUM_ICU2PEFS register */
                if (destination == ICUM_COMM_TARGET_PE0)
                {
                    p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.ICU2PEFS0;
                }
                else if (destination == ICUM_COMM_TARGET_PE1)
                {
                    p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.ICU2PEFS1;
                }
                else
                {
                    p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.ICU2PEFS2;
                }

                uiSelPe = (ICUM_ICU2PEIS_PESEL_BIT_WIDTH * (uint32_t)destination) + (uint32_t)channel - 1UL;
                uiSetbit = (uiSetbit << uiSelPe);

                /* Issue the interrupt according to the isd */
                if (res_interrput == ICUM_COMM_ON)
                {
                    /* set register data */
                    *p_reg = uiSetbit;
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                /* No Prossesing */
            }
        }
        else
        {
            /* 2-1. Out of parameter range */
            uiRet = ER_PARAM;
        }
    }
    else
    {
        /* 1-1. Out of parameter range */
        uiRet = ER_PARAM;
    }

    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0003]:[ICUMFW_UD_U2B_D_COMM_0003]
* Function name: D_COMM_ServiceResponseToPE
* Description  : Trigger Interrupt To PE.
* Arguments    : (IN)icum_comm_target_pe_t destination, 
*                    r_icumif_isd_t *p_isd
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_COMM_QUE_FULL
***************************************************************************************/
uint32_t D_COMM_ServiceResponseToPE(icum_comm_target_pe_t destination, r_icumif_isd_t *p_isd)
{
    volatile uint32_t *p_reg;
    uint32_t uiSelPe;
    uint32_t uiSetbit = (uint32_t)1U;

    /* 1. Set the service request in the response queue */
    (void)d_COMM_SetServiceque(destination, p_isd, ICUM_COMM_OFF);

    /* 2. Determination of execution result of d_COMM_SetServiceque */
    /* 2-1. Write the input parameter to the ICUM_ICU2PEFS register */
    if (destination == ICUM_COMM_TARGET_PE0)
    {
        p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.ICU2PEFS0;
    }
    else if (destination == ICUM_COMM_TARGET_PE1)
    {
        p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.ICU2PEFS1;
    }
    else
    {
        p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.ICU2PEFS2;
    }

    uiSelPe = (ICUM_ICU2PEIS_PESEL_BIT_WIDTH * (uint32_t)destination) + (uint32_t)ICUM_COMM_ICH_TO_SERV_RESPONSE - 1UL;
    uiSetbit = (uiSetbit << uiSelPe);

    /* Issue the interrupt according to the isd */
    if (p_isd->res_nointerrupt == R_ICUMIF_REQRES_INTERRPUT)
    {
        /* set register data */
        *p_reg = uiSetbit;
    }
    else
    {
        /* Do nothing */
    }

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0020]:[ICUMFW_UD_U2B_D_COMM_0020]
* Function name: d_COMM_GetSemaphore
* Description  : Get the semaphore register
* Arguments    : (IN)icum_comm_target_pe_t target_pe
*                    uint8_t response
* Return Value : ER_OK
*                ICUM_COMM_SEMA_ER_SYSTEM
*                ICUM_COMM_SEMA_ER_TIMEOUT
***************************************************************************************/
static uint32_t d_COMM_GetSemaphore(icum_comm_target_pe_t target_pe, uint8_t response)
{
    register volatile uint32_t *p_reg;
    uint32_t uiAuth;
    uint32_t uiIcusem;
    uint32_t err;
    uint32_t uiCount;

    /* Get the Semaphore register address */
    p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.SEMAPE0;
    p_reg += target_pe;

    err = ICUM_COMM_SEMA_ER_TIMEOUT;
    uiCount = 0UL;

    /* Wait for the Semaphore */
    do {
        /* Try to get the Semaphore as step1 */
        uiIcusem = *p_reg;
        uiAuth   = uiIcusem & ICUM_COMM_SEMA_AUTH_MASK;

        if (ICUM_COMM_SEMA_AUTH_GET1ST == uiAuth)
        {
            /* Try to get the Semaphore as step2 */
            uiIcusem = *p_reg;
            uiAuth   = (uiIcusem & ICUM_COMM_SEMA_AUTH_MASK);

            if (ICUM_COMM_SEMA_AUTH_GET2ND != uiAuth)
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
        else if (ICUM_COMM_SEMA_AUTH_GET2ND == uiAuth)
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
    } while (uiCount < ICUM_COMM_SEMA_WAIT);

    /* Set the Response bit in the Semaphore register */
    if ((ER_OK == err) && (ICUM_COMM_ON == response))
    {
        *p_reg |= ICUM_COMM_SEMA_RES;
    }
    else
    {
        /* Do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0021]:[ICUMFW_UD_U2B_D_COMM_0021]
* Function name: d_COMM_FreeSemaphore
* Description  : Free the semaphore register
* Arguments    : (IN)icum_comm_target_pe_t target_pe
*                    uint8_t request
* Return Value : ER_OK
***************************************************************************************/
static uint32_t d_COMM_FreeSemaphore(icum_comm_target_pe_t target_pe, uint8_t request)
{
    register volatile uint32_t *p_reg;
    uint32_t uiAuth;

    /* Get the Semaphore register address */
    p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.SEMAPE0;
    p_reg += target_pe;

    uiAuth   = (*p_reg & ICUM_COMM_SEMA_AUTH_MASK);

    /* Check the Semaphore register status */
    if (ICUM_COMM_SEMA_AUTH_GET2ND == uiAuth)
    {
        if (ICUM_COMM_BOTH_SET == request)
        {
            /* Clear Request bit and set response bit in the Semaphore register */
            *p_reg &= ~ICUM_COMM_SEMA_REQ;
            *p_reg |= ICUM_COMM_SEMA_RES;
        }
        else if (ICUM_COMM_ON == request)
        {
            /* Clear Request bit in the Semaphore register */
            *p_reg &= ~ICUM_COMM_SEMA_REQ;
        }
        else
        {
            /* Do nothing */
        }

        /* Free the Semaphore */
        *p_reg &= ~ICUM_COMM_SEMA_AUTH_MASK;
    }
    else
    {
        /* Do nothing */
    }

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0005]:[ICUMFW_UD_U2B_D_COMM_0005]
* Function name: D_COMM_GetPEStatusInfo
* Description  : PE Status Infomation Get.
* Arguments    : (OUT)uint8_t *p_status
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_COMM_GetPEStatusInfo(uint8_t *p_status)
{
    uint32_t uiRet = ER_OK;
    volatile uint32_t *p_reg;

    /* 1. Address check of output parameter */
    if (NULL != p_status)
    {
        /* 2. Acquire the ICUM_PE 2 ICUS register value     */
        /*            (upper 1 byte portion) and set it to p_status */

        p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.PE2ICUS0;

        *p_status = (uint8_t)(*p_reg >> ICUM_COMM_STSREG_SFT);
    }

    /* 1-1. Out of parameter range */
    else
    {
        uiRet = ER_PARAM;
    }
    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0006]:[ICUMFW_UD_U2B_D_COMM_0006]
* Function name: D_COMM_SetICUStatusInfo
* Description  : PE Status Infomation Set.
* Arguments    : (IN)icum_comm_info_icu_status_t info
*                    uint32_t                    set_value
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_COMM_SetICUStatusInfo(icum_comm_info_icu_status_t info, uint32_t set_value)
{
    uint32_t uiRet = ER_OK;
    uint32_t uiSetbit;
    volatile uint32_t *p_reg;
    uint32_t uiInfoOff = (uint32_t)0U;
    uint32_t uiSavCnt = 0xFFFFFFFFU;
    int32_t  i;
    uint32_t saved_psw;

    /* 1. Check the range of input parameters */
    if ((ICUM_COMM_INF_ICU_ST_BUSY <= info) && (ICUM_COMM_INF_ICU_ST_STARTPE >= info))
    {
        /* 2. Obtain offset value from input parameter (info) */
        for (i = 0; (int32_t)ICUM_COMM_ENTERMAX > i; i++)
        {
            /* Obtain the offset position using the conversion table */
            /* from the bit position (info) to the offset position   */
            if (s_Pe2icu_BitToOff[i].uiIcsStsBit == info)
            {
                uiInfoOff = s_Pe2icu_BitToOff[i].uiIcsStsOff;
                uiSavCnt = (uint32_t)i;
                break;
            }
            else
            {
                /* Do nothing */
            }
        }

        /* 2-1. When the input parameter (info) matches the bit position (info) of the */
        /*     conversion table (s_Pe2icu_BitToOff) to the offset position            */
        if (0xFFFFFFFFUL != uiSavCnt)
        {
            /* 3. Interrupt status check is performed         */
            /*      and interrupt is enabled Interrupt disabled */
            DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

            if (1UL == set_value)
            {
                uiSetbit = 1U;
            }
            else
            {
                uiSetbit = 0U;
            }

            /* 4. Write the specified value (set_value)            */
            /*    for the specified status information bit (info)  */
            /*    to the ICUM_ICU2PES register.                    */

            for (i = 0; i < (int32_t)TOTAL_PE_NUM; i++)
            {
                if (i == ICUM_COMM_TARGET_PE0)
                {
                    p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.ICU2PES0;
                }
                else if (i == ICUM_COMM_TARGET_PE1)
                {
                    p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.ICU2PES1;
                }
                else
                {
                    p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.ICU2PES2;
                }

                uiSetbit = (uiSetbit << uiInfoOff);
                *p_reg   = ((*p_reg & ~s_Pe2icu_BitToOff[uiSavCnt].uiIcsStsBit) | uiSetbit);
            }

            /* 5. Based on the acquired interrupt state,         */
            /*      interrupt is enabled when interrupt is enabled */
            ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
        }
        else
        {
            /* 2-1-2. Out of parameter range */
            uiRet = ER_PARAM;
        }
    }
    else
    {
        /* 1-1. Out of parameter range */
        uiRet = ER_PARAM;
    }
    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0007]:[ICUMFW_UD_U2B_D_COMM_0007]
* Function name: D_COMM_GetICUStatusInfo
* Description  : PE Status Infomation Get.
* Arguments    : (OUT)uint32_t *p_status
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_COMM_GetICUStatusInfo(uint32_t *p_status)
{
    volatile uint32_t *p_reg;
    uint32_t uiRet = ER_OK;

    /* 1. Address check of output parameter */
    if (NULL != p_status)
    {

        p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.ICU2PES0;

        *p_status = *p_reg;
    }
    else
    {
        /* 1-1. Out of parameter range */
        uiRet = ER_PARAM;
    }
    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0019]:[ICUMFW_UD_U2B_D_COMM_0019]
* Function name: D_COMM_GetBOOTMODE
* Description  : Get BootMode information.
* Arguments    : (OUT)uint32_t *p_status
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_COMM_GetBootMode(uint32_t *p_status)
{
    volatile uint32_t *p_reg;
    uint32_t uiRet;

    uiRet = ER_OK;
    /* 1. Address check of output parameter */
    if (NULL != p_status)
    {
        /* 2. Get BootMode information and stores it at the specified address */
        //p_reg = (volatile uint32_t *)&ICUM_UBMDSTS;
        //*p_status = (*p_reg & ICUM_COMM_UBMODE_MASK);
        *p_status = 0x1UL;
    }
    else
    {
        /* 1-1. Out of parameter range */
        uiRet = ER_PARAM;
    }
    return (uiRet);
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0008]:[ICUMFW_UD_U2B_D_COMM_0008]
* Function name: D_COMM_SetIntCallBackFunc
* Description  : Call back registration.
* Arguments    : (IN)ICUM_COMM_CB_REGIST_t *p_regist_info 
* Return Value : ER_OK
***************************************************************************************/
uint32_t D_COMM_SetIntCallBackFunc(ICUM_COMM_CB_REGIST_t *p_regist_info)
{
    /* 1. input parameter channel:Service execution request interrupt */
    if (ICUM_COMM_ICH_FM_SERV_REQUEST == (p_regist_info->channel & ICUM_COMM_ICH_FM_SERV_REQUEST))
    {
        /* Callback Function Set */
        s_cb_isd_ServReq_t.channel = ICUM_COMM_ICH_FM_SERV_REQUEST;
        s_cb_isd_ServReq_t.p_cbfunc = p_regist_info->p_cbfunc;
    }
    else
    {
        /* do nothing */
    }

    /* 2. input parameter channel:CF mode switching ACK notification interrupt */
    if (ICUM_COMM_ICH_FM_CF_PRG_NOTIFY == (p_regist_info->channel & ICUM_COMM_ICH_FM_CF_PRG_NOTIFY))
    {
        /* Callback Function Set */
        s_cb_isd_CF_PrgNtfy_t.channel = ICUM_COMM_ICH_FM_CF_PRG_NOTIFY;
        s_cb_isd_CF_PrgNtfy_t.p_cbfunc = p_regist_info->p_cbfunc;
    }
    else
    {
        /* do nothing */
    }

    /* 3. input parameter channel:DF mode switching ACK notification interrupt */
    if (ICUM_COMM_ICH_FM_DF_PRG_NOTIFY == (p_regist_info->channel & ICUM_COMM_ICH_FM_DF_PRG_NOTIFY))
    {
        /* Callback Function Set */
        s_cb_isd_DF_PrgNtfy_t.channel = ICUM_COMM_ICH_FM_DF_PRG_NOTIFY;
        s_cb_isd_DF_PrgNtfy_t.p_cbfunc = p_regist_info->p_cbfunc;
    }
    else
    {
        /* do nothing */
    }

    /* 4. input parameter channel:CF mode switching NAK notification interrupt */
    if (ICUM_COMM_ICH_FM_CF_PRG_ERROR == (p_regist_info->channel & ICUM_COMM_ICH_FM_CF_PRG_ERROR))
    {
        /* Callback Function Set */
        s_cb_isd_CF_PrgErr_t.channel = ICUM_COMM_ICH_FM_CF_PRG_ERROR;
        s_cb_isd_CF_PrgErr_t.p_cbfunc = p_regist_info->p_cbfunc;
    }
    else
    {
        /* do nothing */
    }

    /* 5. input parameter channel:DF mode switching NAK notification interrupt */
    if (ICUM_COMM_ICH_FM_DF_PRG_ERROR == (p_regist_info->channel & ICUM_COMM_ICH_FM_DF_PRG_ERROR))
    {
        /* Callback Function Set */
        s_cb_isd_DF_PrgErr_t.channel = ICUM_COMM_ICH_FM_DF_PRG_ERROR;
        s_cb_isd_DF_PrgErr_t.p_cbfunc = p_regist_info->p_cbfunc;
    }
    else
    {
        /* do nothing */
    }

    /* 6. input parameter channel:Flash access completion notification interrupt */
    if (ICUM_COMM_ICH_FM_OPERATION_END == (p_regist_info->channel & ICUM_COMM_ICH_FM_OPERATION_END))
    {
        /* Callback Function Set */
        s_cb_isd_OpeEnd_t.channel = ICUM_COMM_ICH_FM_OPERATION_END;
        s_cb_isd_OpeEnd_t.p_cbfunc = p_regist_info->p_cbfunc;
    }
    else
    {
        /* do nothing */
    }

    /* 7. input parameter channel:add job function for single interrupt  */
    if (ICUM_COMM_ICH_FM_REGIST_ADDJOBFUNC == (p_regist_info->channel & ICUM_COMM_ICH_FM_REGIST_ADDJOBFUNC))
    {
        /* Callback Function Set */
        s_cb_isd_AddJobQue_t.channel = ICUM_COMM_ICH_FM_REGIST_ADDJOBFUNC;
        s_cb_isd_AddJobQue_t.p_cbfunc = p_regist_info->p_cbfunc;
    }
    else
    {
        /* do nothing */
    }

    return ER_OK;
}

CHANGE_SEC(text, ".icum_driver_ram")
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0009]:[ICUMFW_UD_U2B_D_COMM_0009]
* Function name: D_COMM_StartupPE
* Description  : PE Startup.
* Arguments    : none
* Return Value : none
***************************************************************************************/
void D_COMM_StartupPE(void)
{
    volatile uint32_t *p_bootctrl;

    /* 1. Set the BC0Bit in the BOOTCTRL register to 1 */
    p_bootctrl = (volatile uint32_t *)&REG_BOOT.BOOTCTRL;
    #ifndef DEBUG
    *p_bootctrl = ICUM_COMM_BC0_ACTIVE;
    #else
    *p_bootctrl = 0xFFFFFFFFUL;
    #endif

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_D_COMM_016]:[ICUMFW_UD_U2B_D_COMM_0010]
* Function name: D_COMM_GetStartupPEInfo
* Description  : Get the PE information that issued the startup instruction.
* Arguments    : uint32_t *p_boot_pe : Store address of the startup instruction info
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_COMM_GetStartupPEInfo(uint32_t *p_boot_pe)
{
    uint32_t            uiRet;
    volatile uint32_t   *p_bootctrl;
    uint32_t            start_pe;

    uiRet = ER_OK;
    p_bootctrl = NULL;
    start_pe = D_COMM_ZERO_U32;

    /* Address check of output parameter */
    if (NULL != p_boot_pe)
    {
        /* 1. Read the value of the BOOTCTRL register */
        p_bootctrl = (volatile uint32_t *)&REG_BOOT.BOOTCTRL;
        start_pe = *p_bootctrl;

        /* 2. The bit of PE0 is fixed to 1 */
        start_pe |= ICUM_COMM_BC0_ACTIVE;

        /* 3. Store startup instruction info */
        *p_boot_pe = start_pe;
    }
    else
    {
        /* Out of parameter range */
        uiRet = ER_PARAM;
    }

    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0011]:[ICUMFW_UD_U2B_D_COMM_0011]
* Function name: D_COMM_SetInterruptSwitch
* Description  : Set interrupt of specified channel of INTPE to enable or disable.
* Arguments    : icum_comm_int_from_channel_t channel : INTPE interrupt channel
*                icum_comm_int_req_status_t   status  : Interrupt status to be set
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_COMM_SetInterruptSwitch(icum_comm_int_from_channel_t channel, icum_comm_int_req_status_t status)
{
    uint32_t          uiRet = ER_OK;
    uint32_t          saved_psw;
    icum_comm_int_from_channel_t set_channel;

    /* Change service request setting value */
    if (ICUM_COMM_ICH_FM_SERV_REQUEST == channel)
    {
        set_channel = ICUM_COMM_SERV_REQUEST_MASK;
    }
    else
    {
        set_channel = channel;
    }

    /* 1. Processing by parameter channel */
    switch (channel)
    {
        case ICUM_COMM_ICH_FM_SERV_REQUEST:
        case ICUM_COMM_ICH_FM_CF_PRG_NOTIFY:
        case ICUM_COMM_ICH_FM_DF_PRG_NOTIFY:
        case ICUM_COMM_ICH_FM_CF_PRG_ERROR:
        case ICUM_COMM_ICH_FM_DF_PRG_ERROR:
        case ICUM_COMM_ICH_FM_OPERATION_END:
            /* 1-1. When channel is valid     */
            /* Processing by parameter status */
            switch ((uint32_t)status)
            {
                case ICUM_COMM_INT_REQ_DISABLE:
                    /* 1-1-1. When status is DISABLE               */
                    /* Interrupt status check is performed         */
                    /* and interrupt is enabled Interrupt disabled */
                    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

                    /* Set interrupt of specified channel of INTPE to disable */
                    REG_ICUM_CMDREG.PE2ICUIE0 = (REG_ICUM_CMDREG.PE2ICUIE0 & ~set_channel);
                    REG_ICUM_CMDREG.PE2ICUIE1 = (REG_ICUM_CMDREG.PE2ICUIE1 & ~set_channel);
                    REG_ICUM_CMDREG.PE2ICUIE2 = (REG_ICUM_CMDREG.PE2ICUIE2 & ~set_channel);

                    /* Dummy read & Synchronize pipeline to wait for the completion */
                    /* of writing to the ICUM_PE2ICUF enable register               */
                    (void)REG_ICUM_CMDREG.PE2ICUIE0;
                    SYNC_PIPELINE();

                    /* Based on the acquired interrupt state,         */
                    /* interrupt is enabled when interrupt is enabled */
                    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
                    break;
                case ICUM_COMM_INT_REQ_ENABLE:
                    /* 1-1-2. When status is ENABLE                */
                    /* Interrupt status check is performed         */
                    /* and interrupt is enabled Interrupt disabled */
                    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

                    /* Set interrupt of specified channel of INTPE to enable */
                    REG_ICUM_CMDREG.PE2ICUIE0 = (REG_ICUM_CMDREG.PE2ICUIE0 | set_channel);
                    REG_ICUM_CMDREG.PE2ICUIE1 = (REG_ICUM_CMDREG.PE2ICUIE1 | set_channel);
                    REG_ICUM_CMDREG.PE2ICUIE2 = (REG_ICUM_CMDREG.PE2ICUIE2 | set_channel);

                    /* Dummy read & Synchronize pipeline to wait for the completion */
                    /* of writing to the ICUM_PE2ICUF enable register               */
                    (void)REG_ICUM_CMDREG.PE2ICUIE0;
                    SYNC_PIPELINE();

                    /* Based on the acquired interrupt state,         */
                    /* interrupt is enabled when interrupt is enabled */
                    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
                    break;
                default:
                    /* 1-1-3. When status is invalid */
                    uiRet = ER_PARAM;
                    break;
            }
            break;
        default:
            /* 1-2. When channel is invalid */
            uiRet = ER_PARAM;
            break;
    }

    /* 2. Return interrupt enable / disable result */
    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0012]:[ICUMFW_UD_U2B_D_COMM_0012]
* Function name: D_COMM_CheckMemorySharedRAM
* Description  : Check the memory 
* Arguments    :(IN)uint32_t start_addr: start address
*               (IN)uint32_t size: check memory size
*               (IN)icum_comm_check_order_t msbyte_check: Msbyte check flag
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_COMM_MEMCHK_NG
***************************************************************************************/
uint32_t D_COMM_CheckMemorySharedRAM(uint32_t start_addr, uint32_t size, icum_comm_check_order_t msbyte_check)
{
    uint32_t          uiRet = ER_OK;

    /* Determine input parameters msbyte_check */
    /* When msbyte_check is doing check */
    if ((uint32_t)ICUM_COMM_DO_CHECK == (uint32_t)msbyte_check)
    {
        /* When the highest byte of address is check OK */
        if (ICUM_COMM_MSBYTE_CHECK_OK == (start_addr & ICUM_COMM_MSBYTE_MASK))
        {
            /* Do nothing */
        }
        else
        {
            /* memory check faild */
            uiRet = ER_D_COMM_MEMCHK_NG;
        }
    }
    /* When msbyte_check is doing check */
    else if ((uint32_t)ICUM_COMM_DO_NOT_CHECK == (uint32_t)msbyte_check)
    {
        /* Do nothing */
    }
    else
    {
        /* Out of parameter range */
        uiRet = ER_PARAM;
    }

    if (ER_OK == uiRet)
    {
        /* When the address is out of range */
        if (((start_addr & (uint32_t)ICUM_COMM_ADDR_MASK) > (uint32_t)ICUM_COMM_ADDR_MAX) ||
           (((start_addr + size - 1UL) & (uint32_t)ICUM_COMM_ADDR_MASK) > (uint32_t)ICUM_COMM_ADDR_MAX))
        {
            /* memory check faild */
            uiRet = ER_D_COMM_MEMCHK_NG;
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

    return (uiRet);
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

CHANGE_SEC(text, ".icum_handler.text")
CHANGE_SEC(rodata, ".icum_handler.rodata")
/**************************************************************************************/
/* Interrupt Function                                                                 */
/**************************************************************************************/

/**************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0013]:[ICUMFW_UD_U2B_D_COMM_0013]
* Function name: ICPE0 Interrupt handler
* Description  : ICPE0 Interrupt handler.
* Arguments    : none
* Return Value : none
**************************************************************************************/
void ICPE0(void)
{
    /* 1. Call the interrupt type identification callback function */
    d_COMM_IntpeFunc(ICUM_COMM_TARGET_PE0);
}

/**************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0013]:[ICUMFW_UD_U2B_D_COMM_0013]
* Function name: ICPE1 Interrupt handler
* Description  : ICPE1 Interrupt handler.
* Arguments    : none
* Return Value : none
**************************************************************************************/
void ICPE1(void)
{
    /* 1. Call the interrupt type identification callback function */
    d_COMM_IntpeFunc(ICUM_COMM_TARGET_PE1);
}

/**************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0013]:[ICUMFW_UD_U2B_D_COMM_0013]
* Function name: ICPE2 Interrupt handler
* Description  : ICPE2 Interrupt handler.
* Arguments    : none
* Return Value : none
**************************************************************************************/
void ICPE2(void)
{
    /* 1. Call the interrupt type identification callback function */
    d_COMM_IntpeFunc(ICUM_COMM_TARGET_PE2);
}

/**************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0017]:[ICUMFW_UD_U2B_D_COMM_0017]
* Function name: ICPES Interrupt handler
* Description  : ICPES Interrupt handler.
* Arguments    : none
* Return Value : none
**************************************************************************************/
void ICPES(void)
{
    volatile uint32_t *p_Pe2icusf;
    volatile uint32_t *p_Pe2icusfc;
    volatile uint32_t *p_Pe2icus;
    r_icumif_isd_t *p_isd;
    uint32_t err;
    icum_comm_target_pe_t requester_pe;
    uint32_t semaphore;
    
    err = ER_OK;
    semaphore = ICUM_COMM_SEMA_ER_TIMEOUT;
    
    /* 
    * 1. Acquire the interrupt factor from the ICUM_PE 2 ICUF register.
    * ICUM_CMDREG.ICUM_PE2ICUF - CPU (PE) Interrupt Request Flag Register 
    */
    p_Pe2icusf = (volatile uint32_t*)&REG_ICUM_CMDREG.PE2ICUSF;
    
    /* ICUM_CMDREG.ICUM_PE2ICUFC - ICUM_PE2ICUF Clear Register */
    p_Pe2icusfc = (volatile uint32_t*)&REG_ICUM_CMDREG.PE2ICUSFC;
    
    /* 
    * 2. Acquire the ICUM_PE 2 ICUS register value.
    * (lower 1 byte portion) and set it to p_status 
    */
    p_Pe2icus = (volatile uint32_t *)&REG_ICUM_CMDREG.PE2ICUS0;
    requester_pe = (uint8_t)(*p_Pe2icus);
    requester_pe = requester_pe & (uint8_t)ICUM_COMM_PEID_MASK;
    
    /* Clear interrupt factor */
    *p_Pe2icusfc = ICUM_COMM_ICH_FM_SINGLE_INTERRUPT;
    
    (void)(*p_Pe2icusf);
    SYNC_PIPELINE();
    
    /* is the requester ID of register ICUM_PE2ICUS valid? */
    if ((int32_t)TOTAL_PE_NUM > (int32_t)requester_pe)
    {
        semaphore = d_COMM_GetSemaphore(requester_pe, ICUM_COMM_OFF);
        if (semaphore == ER_OK)
        {
            /* Success get semaphore. Do nothing */
        }
        else
        {
            /* Fail to get semaphore. Not response and end process. */
            err = ER_PARAM;
        }
    }
    else
    {
        /* the requester ID of register ICUM_PE2ICUS is invalid. Not response and end process. */
        err = ER_PARAM;
    }
    
    /* Success get semaphore */
    if (ER_OK == err)
    {
        /* Dequeue the ISD from the head of the service request queue */
        p_isd = d_COMM_GetServiceque(requester_pe, 0L, ICUM_COMM_OFF);
        
        /* ISD parameter check */
        if (NULL != p_isd)
        {
            /* is the requester ID of ISD valid? */
            if ((int32_t)TOTAL_PE_NUM > (int32_t)p_isd->requester_id)
            {
                /* Check service request is FastCMAC  */
                if (((uint32_t)R_ICUMIF_SERV_PRIORITY_HIGHEST == (uint32_t)p_isd->service_priority) && 
                    ((uint32_t)SERVICE_01_AES_CMAC == (uint32_t)p_isd->service_id))
                {
                    /* Do nothing */
                }
                else
                {
                    /* Service is pending. Add job in queue. */ 
                    if (NULL != s_cb_isd_AddJobQue_t.p_cbfunc)
                    {
                        (*s_cb_isd_AddJobQue_t.p_cbfunc)(s_cb_isd_AddJobQue_t.channel, (r_icumif_isd_t *)p_isd);
                    }
                    else
                    {
                        /* Callback function is not registed. */
                        p_isd->service_result = SERV_SYS_ERROR;
                    }
                    err = ER_IN_PROCESS;
                }
            }
            else
            {
                p_isd->service_result = SERV_REQUESTER_ID_ERROR;
                err = ER_PARAM;
                /* choose an arbitrary target for the error response */
                p_isd->requester_id = MISC_PEID_PE0;
            }
        }
        else
        {
            /* Fail to get the ISD. Not response and end process. */
            err = ER_PARAM;
        }
        
        if (NULL != p_isd)
        {
            if (SERV_REQUEST > p_isd->service_result)
            {
                /* 
                * provide the error back to the caller
                * note: in this case, no job created (see above) 
                */
                if ((uint32_t)0xFFu != (uint32_t)p_isd->host_int_ch)
                {
                    /* response in queue: trigger host interrupt */
                    (void)D_COMM_ServiceResponseToPE((icum_comm_target_pe_t)p_isd->requester_id, p_isd);
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
            /* Do nothing */
        }
        
    }
    else
    {
        /* Do nothing */
    }
    
    if (ER_OK == semaphore)
    {
        /* Free semaphore */
        (void)d_COMM_FreeSemaphore(requester_pe, ICUM_COMM_BOTH_SET);
    }
    else
    {
        /* Do nothing */
    }
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

CHANGE_SEC(text, ".icum_handler_ram")
CHANGE_SEC(rodata, ".icum_handler_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0014]:[ICUMFW_UD_U2B_D_COMM_0014]
* Function name: ICPE0 Interrupt handler
* Description  : ICPE0 Interrupt handler.
* Arguments    : none
* Return Value : none
***************************************************************************************/
void ICPE0_RAM(void)
{
    /* 1. Call the interrupt type identification callback function */
    d_COMM_IntpeFunc(ICUM_COMM_TARGET_PE0);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0014]:[ICUMFW_UD_U2B_D_COMM_0014]
* Function name: ICPE1 Interrupt handler
* Description  : ICPE1 Interrupt handler.
* Arguments    : none
* Return Value : none
***************************************************************************************/
void ICPE1_RAM(void)
{
    /* 1. Call the interrupt type identification callback function */
    d_COMM_IntpeFunc(ICUM_COMM_TARGET_PE1);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0014]:[ICUMFW_UD_U2B_D_COMM_0014]
* Function name: ICPE2 Interrupt handler
* Description  : ICPE2 Interrupt handler.
* Arguments    : none
* Return Value : none
***************************************************************************************/
void ICPE2_RAM(void)
{
    /* 1. Call the interrupt type identification callback function */
    d_COMM_IntpeFunc(ICUM_COMM_TARGET_PE2);
}

/**************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0018]:[ICUMFW_UD_U2B_D_COMM_0018]
* Function name: ICPES Interrupt handler
* Description  : ICPES Interrupt handler.
* Arguments    : none
* Return Value : none
**************************************************************************************/
void ICPES_RAM(void)
{
    volatile uint32_t *p_Pe2icusf;
    volatile uint32_t *p_Pe2icusfc;
    volatile uint32_t *p_Pe2icus;
    r_icumif_isd_t *p_isd;
    uint32_t err;
    icum_comm_target_pe_t requester_pe;
    uint32_t semaphore;
    
    err = ER_OK;
    semaphore = ICUM_COMM_SEMA_ER_TIMEOUT;
    
    /* 
    * 1. Acquire the interrupt factor from the ICUM_PE 2 ICUF register.
    * ICUM_CMDREG.ICUM_PE2ICUF - CPU (PE) Interrupt Request Flag Register 
    */
    p_Pe2icusf = (volatile uint32_t*)&REG_ICUM_CMDREG.PE2ICUSF;
    
    /* ICUM_CMDREG.ICUM_PE2ICUFC - ICUM_PE2ICUF Clear Register */
    p_Pe2icusfc = (volatile uint32_t*)&REG_ICUM_CMDREG.PE2ICUSFC;
    
    /* 
    * 2. Acquire the ICUM_PE 2 ICUS register value.
    * (lower 1 byte portion) and set it to p_status 
    */
    
    p_Pe2icus = (volatile uint32_t *)&REG_ICUM_CMDREG.PE2ICUS0;
    requester_pe = (uint8_t)(*p_Pe2icus);
    requester_pe = requester_pe & (uint8_t)ICUM_COMM_PEID_MASK;
    
    /* Clear interrupt factor */
    *p_Pe2icusfc = ICUM_COMM_ICH_FM_SINGLE_INTERRUPT;
    
    (void)(*p_Pe2icusf);
    SYNC_PIPELINE();
    
    /* is the requester ID of register ICUM_PE2ICUS valid? */
    if ((int32_t)TOTAL_PE_NUM > (int32_t)requester_pe)
    {
        semaphore = d_COMM_GetSemaphore(requester_pe, ICUM_COMM_OFF);
        if (semaphore == ER_OK)
        {
            /* Success get semaphore. Do nothing */
        }
        else
        {
            /* Fail to get semaphore. Not response and end process. */
            err = ER_PARAM;
        }
    }
    else
    {
        /* the requester ID of register ICUM_PE2ICUS is invalid. Not response and end process. */
        err = ER_PARAM;
    }
    
    /* Success get semaphore */
    if (ER_OK == err)
    {
        /* Dequeue the ISD from the head of the service request queue */
        p_isd = d_COMM_GetServiceque(requester_pe, 0L, ICUM_COMM_OFF);
        
        /* ISD parameter check */
        if (NULL != p_isd)
        {
            /* is the requester ID of ISD valid? */
            if ((int32_t)TOTAL_PE_NUM > (int32_t)p_isd->requester_id)
            {
                /* Check service request is FastCMAC  */
                if (((uint32_t)R_ICUMIF_SERV_PRIORITY_HIGHEST == (uint32_t)p_isd->service_priority) && 
                    ((uint32_t)SERVICE_01_AES_CMAC == (uint32_t)p_isd->service_id))
                {
                    /* Do nothing */
                }
                else
                {
                    /* Service is pending. Add job in queue. */ 
                    if (NULL != s_cb_isd_AddJobQue_t.p_cbfunc)
                    {
                        (*s_cb_isd_AddJobQue_t.p_cbfunc)(s_cb_isd_AddJobQue_t.channel, (r_icumif_isd_t *)p_isd);
                    }
                    else
                    {
                        /* Callback function is not registed. */
                        p_isd->service_result = SERV_SYS_ERROR;
                    }
                    err = ER_IN_PROCESS;
                }
            }
            else
            {
                p_isd->service_result = SERV_REQUESTER_ID_ERROR;
                err = ER_PARAM;
                /* choose an arbitrary target for the error response */
                p_isd->requester_id = MISC_PEID_PE0;
            }
        }
        else
        {
            /* Fail to get the ISD. Not response and end process. */
            err = ER_PARAM;
        }
        
        if (NULL != p_isd)
        {
            if (SERV_REQUEST > p_isd->service_result)
            {
                /* 
                * provide the error back to the caller
                * note: in this case, no job created (see above) 
                */
                if ((uint32_t)0xFFu != (uint32_t)p_isd->host_int_ch)
                {
                    /* response in queue: trigger host interrupt */
                    (void)D_COMM_ServiceResponseToPE((icum_comm_target_pe_t)p_isd->requester_id, p_isd);
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
            /* Do nothing */
        }
        
    }
    else
    {
        /* Do nothing */
    }
    
    if (ER_OK == semaphore)
    {
        /* Free semaphore */
        (void)d_COMM_FreeSemaphore(requester_pe, ICUM_COMM_BOTH_SET);
    }
    else
    {
        /* Do nothing */
    }
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

CHANGE_SEC(text, ".icum_driver_ram")
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/**************************************************************************************/
/* Local Function                                                                     */
/**************************************************************************************/
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0004]:[ICUMFW_UD_U2B_D_COMM_0004]
* Function name: d_COMM_SetServiceque
* Description  : Set the service request in the response queue.
* Arguments    : (IN)r_icumif_isd_t *p_isd
*              : (IN)ICUM_COMM_CB_REGIST_t *p_regist_info 
* Return Value : ER_OK
*                ER_D_COMM_QUE_FULL
***************************************************************************************/
static uint32_t d_COMM_SetServiceque(icum_comm_target_pe_t target_pe, r_icumif_isd_t *p_ISD, uint8_t mem_check)
{
    register volatile uint32_t *p_reg;
    uint32_t *p_qptr;
    uint32_t err = ER_OK;
    int32_t  i_idx;
    p_manip_t queptr;
    uint32_t mem_err = ER_OK;

    /* 1. Acquire value from ICUM_CMDREGICUM_PE 2 ICUS register */
    p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.SEMAPE0;
    p_reg += target_pe;

    /* 2. mask the upper 1 byte of input parameter (p_ISD) data */
    /* 3. Set (OR) the head address of the service request queue */
    queptr.val = ((*p_reg & ICUM_PE2ICUS_PEICUMCOM_MASK) | MASK_ADDR_QUEUE);
    p_qptr  = (uint32_t *)queptr.p_ptr;

    /* Check the respose queue address */
    if (ICUM_COMM_ON == mem_check)
    {
        mem_err = D_COMM_CheckMemorySharedRAM((uint32_t)p_qptr, REQRES_QUEUE_SIZE, ICUM_COMM_DO_CHECK);
    }
    else
    {
        /* Do nothing */
    }

    /* Check the ISD parameter address */
    if (ER_OK == mem_err)
    {
        if (ICUM_COMM_ON == mem_check)
        {
            mem_err = D_COMM_CheckMemorySharedRAM((uint32_t)p_ISD, sizeof(r_icumif_isd_t), ICUM_COMM_DO_CHECK);
        }
        else
        {
            /* Do nothing */
        }

        if(ER_OK == mem_err)
        {
            err = ER_D_COMM_QUE_FULL;
            p_qptr += REQUEST_QUEUE_DEPTH;

            /* 6. When the internal area value is less than 0 */
            for (i_idx = 0; i_idx < RESPONSE_QUEUE_DEPTH; i_idx++)
            {
                if (0UL == (*(p_qptr + i_idx)))
                {
                    /* R_ICUMIF_REQRES_ICUM_RES_ENQUE is set to p_ISD(req_res_status) */
                    /* ICU-M enqueued ISD to service response queue Set */
                    p_ISD->req_res_status = R_ICUMIF_REQRES_ICUM_RES_ENQUE;

                    /* mark the entry as occupied and set the p_ISD */
                    *(p_qptr + i_idx) = (uint32_t)((volatile void *)p_ISD);

                    /* Dummy read & Synchronize pipeline to wait for the completion of */
                    /* writing to the shared memory                                    */
                    (void)(p_ISD->req_res_status);
                    SYNC_PIPELINE();

                    err = ER_OK;
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
        }
        else
        {
            *p_reg |= ICUM_COMM_SEMA_ERR;
            err = ER_D_COMM_PARAM_NOT_CRAM;
        }
    }
    else
    {
        *p_reg |= ICUM_COMM_SEMA_ERR;
        err = ER_D_COMM_QUE_NOT_ACCESS;
    }
    return (err);
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

CHANGE_SEC(text, ".icum_handler_ram")
CHANGE_SEC(rodata, ".icum_handler_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0015]:[ICUMFW_UD_U2B_D_COMM_0015]
* Function name: d_COMM_IntpeFunc
* Description  : When an interrupt is received from the PE side, 
*                the interrupt type generated from the ICUM_PE 2 ICUIE register 
*                is specified.
* Arguments    : (IN)icum_comm_target_pe_t requester_pe
* Return Value : none
***************************************************************************************/
static void d_COMM_IntpeFunc(icum_comm_target_pe_t requester_pe)
{
    volatile uint32_t *p_Pe2icuf;
    volatile uint32_t *p_Pe2icufc;
    uint32_t uiPe2icuf;
    uint32_t uiexistPisd;
    r_icumif_isd_t *p_isd;
    int32_t idx;
    uint32_t semaphore;
    uint8_t sem_set_bit;

    /* Initial value */
    sem_set_bit = ICUM_COMM_ON;

    /* 1. Acquire the interrupt factor from the ICUM_PE 2 ICUF register. */
    if (requester_pe == ICUM_COMM_TARGET_PE0)
    {
        /* ICUM_CMDREG.ICUM_PE2ICUF0 - CPU (PE) Interrupt Request Flag Register 0 */
        p_Pe2icuf = (volatile uint32_t *)&REG_ICUM_CMDREG.PE2ICUF0;

        /* ICUM_CMDREG.ICUM_PE2ICUFC0 - ICUM_PE2ICUF Clear Register 0 */
        p_Pe2icufc = (volatile uint32_t *)&REG_ICUM_CMDREG.PE2ICUFC0;
     }
     else if (requester_pe == ICUM_COMM_TARGET_PE1)
     {
        /* ICUM_CMDREG.ICUM_PE2ICUF0 - CPU (PE) Interrupt Request Flag Register 1 */
        p_Pe2icuf = (volatile uint32_t *)&REG_ICUM_CMDREG.PE2ICUF1;

        /* ICUM_CMDREG.ICUM_PE2ICUFC0 - ICUM_PE2ICUF Clear Register 1 */
        p_Pe2icufc = (volatile uint32_t *)&REG_ICUM_CMDREG.PE2ICUFC1;
     }
     else
     {
        /* ICUM_CMDREG.ICUM_PE2ICUF0 - CPU (PE) Interrupt Request Flag Register 2 */
        p_Pe2icuf = (volatile uint32_t *)&REG_ICUM_CMDREG.PE2ICUF2;

        /* ICUM_CMDREG.ICUM_PE2ICUFC0 - ICUM_PE2ICUF Clear Register 2 */
        p_Pe2icufc = (volatile uint32_t *)&REG_ICUM_CMDREG.PE2ICUFC2;
     }

    /* 2. Saving the acquired interrupt factor in the internal area */
    uiPe2icuf = *p_Pe2icuf;

    /* 9. Looping while interrupt factors remain */
    while (0UL != uiPe2icuf)
    {
        /* 3. interrupt cause:"service execution request interrupt" */
        if (ICUM_COMM_INTTYPE_BITOFF != (uiPe2icuf & ICUM_COMM_INTTYPE_SERV_MASK))
        {
            /* Serch the PE */
            for (idx = 0; idx < TOTAL_PE_NUM; idx++)
            {
                if (s_ReqBittoRequester[idx].uiReqBit & uiPe2icuf)
                {
                    *p_Pe2icufc =  s_ReqBittoRequester[idx].uiReqBit;
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
            /* Dummy read & Synchronize pipeline to wait for the completion of */
            /* writing to the ICUM_PE2ICUF Clear Register                      */
            (void)(*p_Pe2icuf);
            SYNC_PIPELINE();

            /* The ICUM_COMM_ICH_FM_SERV_REQUEST bit is cleared */
            /* from the interrupt factor save area              */
            uiPe2icuf   = (uiPe2icuf & ~s_ReqBittoRequester[idx].uiReqBit);

            /* Get the Semaphore to access the request queue */
            semaphore = d_COMM_GetSemaphore(requester_pe, ICUM_COMM_OFF);

            if (ER_OK == semaphore)
            {
                for (uiexistPisd = 0UL; REQUEST_QUEUE_DEPTH > uiexistPisd; uiexistPisd++)
                {
                    /* 3-2. Dequeue the ISD from the head of the */
                    /* service request queue                            */
                    p_isd = d_COMM_GetServiceque(requester_pe, uiexistPisd, ICUM_COMM_ON);
                    if (NULL != p_isd)
                    {
                        /* 3-3. Callback Function Execute */
                        if (NULL != s_cb_isd_ServReq_t.p_cbfunc)
                        {
                            (*s_cb_isd_ServReq_t.p_cbfunc)(s_cb_isd_ServReq_t.channel, (r_icumif_isd_t *)p_isd);
                            
                            /* When response to caller, set response bit in semaphore register */
                            if ((SERV_REQUEST > p_isd->service_result) && 
                                ((uint32_t)0xFFu != (uint32_t)p_isd->host_int_ch))
                            {
                                sem_set_bit = ICUM_COMM_BOTH_SET;
                            }
                            else
                            {
                                /* Do nothing */
                            }
                        }
                        else
                        {
                            /* No Prossesing */
                        }
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                (void)d_COMM_FreeSemaphore(requester_pe, sem_set_bit);
            }
            else
            {
                /* Do nothing */
            }
        }
        /* 4. interrupt cause:"CF mode switching ACK notification" */
        else if (ICUM_COMM_INTTYPE_BITOFF != (uiPe2icuf & ICUM_COMM_ICH_FM_CF_PRG_NOTIFY))
        {
            /* 4-1. Clear the ICUM_COMM_ICH_FM_CF_PRG_NOTIFY bit */
            *p_Pe2icufc = ICUM_COMM_ICH_FM_CF_PRG_NOTIFY;

            /* Dummy read & Synchronize pipeline to wait for the completion of */
            /* writing to the ICUM_PE2ICUF Clear Register                      */
            (void)(*p_Pe2icuf);
            SYNC_PIPELINE();

            /* The ICUM_COMM_ICH_FM_CF_PRG_NOTIFY bit is cleared */
            /* from the interrupt factor save area               */
            uiPe2icuf   = (uiPe2icuf & ~ICUM_COMM_ICH_FM_CF_PRG_NOTIFY);

            /* 4-2. Callback Function Execute */
            if (NULL != s_cb_isd_CF_PrgNtfy_t.p_cbfunc)
            {
                (*s_cb_isd_CF_PrgNtfy_t.p_cbfunc)(s_cb_isd_CF_PrgNtfy_t.channel, NULL);
            }
            else
            {
                /* No Prossesing */
            }
        }
        /* 5. interrupt cause:"DF mode switching ACK notification" */
        else if (ICUM_COMM_INTTYPE_BITOFF != (uiPe2icuf & ICUM_COMM_ICH_FM_DF_PRG_NOTIFY))
        {
            /* 5-1. Clear the ICUM_COMM_ICH_FM_DF_PRG_NOTIFY bit */
            *p_Pe2icufc = ICUM_COMM_ICH_FM_DF_PRG_NOTIFY;

            /* Dummy read & Synchronize pipeline to wait for the completion of */
            /* writing to the ICUM_PE2ICUF Clear Register                      */
            (void)(*p_Pe2icuf);
            SYNC_PIPELINE();

            /* The ICUM_COMM_ICH_FM_DF_PRG_NOTIFY bit is cleared */
            /* from the interrupt factor save area               */
            uiPe2icuf   = (uiPe2icuf & ~ICUM_COMM_ICH_FM_DF_PRG_NOTIFY);

            /* 5-2. Callback Function Execute */
            if (NULL != s_cb_isd_DF_PrgNtfy_t.p_cbfunc)
            {
                (*s_cb_isd_DF_PrgNtfy_t.p_cbfunc)(s_cb_isd_DF_PrgNtfy_t.channel, NULL);
            }
            else
            {
                /* No Prossesing */
            }
        }
        /* 6. interrupt cause:"CF mode switching NACK notification" */
        else if (ICUM_COMM_INTTYPE_BITOFF != (uiPe2icuf & ICUM_COMM_ICH_FM_CF_PRG_ERROR))
        {
            /* 6-1. Clear the ICUM_COMM_ICH_FM_CF_PRG_ERROR bit */
            *p_Pe2icufc = ICUM_COMM_ICH_FM_CF_PRG_ERROR;

            /* Dummy read & Synchronize pipeline to wait for the completion of */
            /* writing to the ICUM_PE2ICUF Clear Register                      */
            (void)(*p_Pe2icuf);
            SYNC_PIPELINE();

            /* The ICUM_COMM_ICH_FM_CF_PRG_ERROR bit is cleared */
            /* from the interrupt factor save area              */
            uiPe2icuf   = (uiPe2icuf & ~ICUM_COMM_ICH_FM_CF_PRG_ERROR);

            /* 6-2. Callback Function Execute */
            if (NULL != s_cb_isd_CF_PrgErr_t.p_cbfunc)
            {
                (*s_cb_isd_CF_PrgErr_t.p_cbfunc)(s_cb_isd_CF_PrgErr_t.channel, NULL);
            }
            else
            {
                /* No Prossesing */
            }
        }
        /* 7. interrupt cause:"DF mode switching NACK notification" */
        else if (ICUM_COMM_INTTYPE_BITOFF != (uiPe2icuf & ICUM_COMM_ICH_FM_DF_PRG_ERROR))
        {
            /* 7-1. Clear the ICUM_COMM_ICH_FM_DF_PRG_ERROR bit */
            *p_Pe2icufc = ICUM_COMM_ICH_FM_DF_PRG_ERROR;

            /* Dummy read & Synchronize pipeline to wait for the completion of */
            /* writing to the ICUM_PE2ICUF Clear Register                      */
            (void)(*p_Pe2icuf);
            SYNC_PIPELINE();

            /* The ICUM_COMM_ICH_FM_DF_PRG_ERROR bit is cleared */
            /* from the interrupt factor save area              */
            uiPe2icuf   = (uiPe2icuf & ~ICUM_COMM_ICH_FM_DF_PRG_ERROR);

            /* 7-2. Callback Function Execute */
            if (NULL != s_cb_isd_DF_PrgErr_t.p_cbfunc)
            {
                (*s_cb_isd_DF_PrgErr_t.p_cbfunc)(s_cb_isd_DF_PrgErr_t.channel, NULL);
            }
            else
            {
                /* No Prossesing */
            }
        }
        /* 8. interrupt cause:"Flash access completion notification" */
        else if (ICUM_COMM_INTTYPE_BITOFF != (uiPe2icuf & ICUM_COMM_ICH_FM_OPERATION_END))
        {
            /* 8-1. Clear the ICUM_COMM_ICH_FM_OPERATION_END bit */
            *p_Pe2icufc = ICUM_COMM_ICH_FM_OPERATION_END;

            /* Dummy read & Synchronize pipeline to wait for the completion of */
            /* writing to the ICUM_PE2ICUF Clear Register                      */
            (void)(*p_Pe2icuf);
            SYNC_PIPELINE();

            /* The ICUM_COMM_ICH_FM_OPERATION_END bit is cleared */
            /* from the interrupt factor save area               */
            uiPe2icuf   = (uiPe2icuf & ~ICUM_COMM_ICH_FM_OPERATION_END);

            /* 8-2. Callback Function Execute */
            if (NULL != s_cb_isd_OpeEnd_t.p_cbfunc)
            {
                (*s_cb_isd_OpeEnd_t.p_cbfunc)(s_cb_isd_OpeEnd_t.channel, NULL);
            }
            else
            {
                /* No Prossesing */
            }
        }
        else
        {
            break;
        }
    }
    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_COMM_0016]:[ICUMFW_UD_U2B_D_COMM_0016]
* Function name: d_COMM_GetServiceque
* Description  : Acquire the ISD from the beginning of the service request queue.
* Arguments    : (IN)icum_comm_target_pe_t target_pe
*              : (IN)int32_t queue_idx
* Return Value : static r_icumif_isd_t *
***************************************************************************************/
static r_icumif_isd_t *d_COMM_GetServiceque(icum_comm_target_pe_t target_pe, int32_t queue_idx, uint8_t mem_check)
{
    register volatile uint32_t *p_reg;
    r_icumif_isd_t *p_ISD = NULL;
    uint32_t  *p_qptr;
    p_manip_t queptr;
    p_manip_t isdptr;
    uint32_t mem_err= ER_OK;

    p_reg = (volatile uint32_t *)&REG_ICUM_CMDREG.SEMAPE0;
    p_reg += target_pe;

    queptr.val = ((*p_reg & ICUM_PE2ICUS_PEICUMCOM_MASK) | MASK_ADDR_QUEUE);
    p_qptr = (uint32_t *)queptr.p_ptr;

    /* Check the request queue address */
    if (ICUM_COMM_ON == mem_check)
    {
        mem_err = D_COMM_CheckMemorySharedRAM((uint32_t)p_qptr, REQRES_QUEUE_SIZE, ICUM_COMM_DO_CHECK);
    }
    else
    {
        /* Do nothiong */
    }

    if (ER_OK == mem_err)
    {
        if (0UL != (*(p_qptr + queue_idx)))
        {
            /* 6-1. Set contents added in the area saved in the p_ISD */
            isdptr.val = *(p_qptr + queue_idx);
            p_ISD = (r_icumif_isd_t *)isdptr.p_ptr;

            /* Check the ISD parameter address */
            if (ICUM_COMM_ON == mem_check)
            {
                mem_err = D_COMM_CheckMemorySharedRAM((uint32_t)p_ISD, sizeof(r_icumif_isd_t), ICUM_COMM_DO_CHECK);
            }
            else
            {
                /* Do nothiong */
            }

            if (ER_OK == mem_err)
            {
                /* 6-2. R_ICUMIF_REQRES_ICUM_REQ_DEQUE is set to p_ISD(req_res_status) */
                p_ISD->req_res_status = R_ICUMIF_REQRES_ICUM_REQ_DEQUE;
            }
            else
            {
                /* Do nothing */
            }
            /* 6-3. free the entry in the request queue */
            *(p_qptr + queue_idx) = 0UL;
        }
        else
        {
            /* 7. In cases other than the above ISD is set to 0 */
            /* Do nothing */
        }
    }
    else
    {
        *p_reg |= ICUM_COMM_SEMA_ERR;
    }

    return (p_ISD);
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

