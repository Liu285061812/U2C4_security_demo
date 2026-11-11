/******************************************************************************/
/* Component Name  System / Life Cycle Manager (System Interrupt)             */
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
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_d_int_pub.h"
#include "icum_d_mpu_pub.h"
#include "icum_d_reset_pub.h"
#include "icum_d_wdt_pub.h"
#include "icum_ml_cfg_pub.h"
#include "icum_mh_sysinfo_pub.h"
#include "icum_mh_sysinfo.h"
/* To use macro functions */
#include "icum_d_misc_pub.h"

/**************************************************************************************/
/* Definitions                                                                        */
/**************************************************************************************/
#define SYSINFO_OFF            (0x00000000uL)
#define SYSINFO_ON             (0x00000001uL)
#define SYSINFO_START_PE       (0x00000001uL)
#define SYSINFO_WDT_RESETMODE  (0x00000001uL)

/**************************************************************************************/
/* Global Variables                                                                   */
/**************************************************************************************/

/**************************************************************************************/
/* Functions                                                                          */
/**************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0036]:[ICUMFW_UD_MH_SYSINFO_MNG_0036]
* Function name: mh_SYSINFO_SendSysInterrupt
* Description  : Send a system interrupt to PE.
* Arguments    : N/A
* Return Value : N/A
***************************************************************************************/
#pragma ghs section text    = ".icum_middle_ram"
#pragma ghs section rodata  = ".icum_middle_const_ram"
void mh_SYSINFO_SendSysInterrupt(void)
{
    sys_cfg_sys_int_t sys_interrupt;
    sys_interrupt.peid     = 0u;
    sys_interrupt.channel  = 0u;

    (void)ML_CFG_GetSys_ram(ML_CFG_SYS_INTERRUPT,(uint32_t*)&sys_interrupt);
    /* send a system interrupt to PE */
    (void)D_COMM_TriggerInterruptToPE((icum_comm_target_pe_t)sys_interrupt.peid,
                                      ICUM_COMM_ICH_TO_SYS_INTERRUPT, NULL);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0035]:[ICUMFW_UD_MH_SYSINFO_MNG_0035]
* Function name: mh_SYSINFO_Interrupt
* Description  : Common processing for send a system interrupt to PE.
* Arguments    : N/A
* Return Value : ER_OK
***************************************************************************************/
static uint32_t mh_SYSINFO_Interrupt(uint32_t system_err)
{
    uint32_t status = 0uL;
    uint8_t start_pe = 0u;

    /* enable system error state */
    if(system_err == SYSINFO_ON)
    {
        (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_SYSTEMERR, SYSINFO_ON);
    }
    else
    {
        /* Do nothing */
    }
    
    /* disable service available state */
    (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_SERVAVAIL, SYSINFO_OFF);
    /* get ICU-M status property */
    (void)D_COMM_GetICUStatusInfo(&status);
    /* already started pe, send system interrupt to pe */
    if((status & ICUM_COMM_INF_ICU_ST_STARTPE) == ICUM_COMM_INF_ICU_ST_STARTPE)
    {
        /* send a system interrupt to PE */
        (void)mh_SYSINFO_SendSysInterrupt();
    }
    else
    {
        /* get start PE property */
        (void)ML_CFG_GetSys_ram(ML_CFG_SYS_START_PE, (uint32_t*)&start_pe);
        /* enable start PE state */
        if((uint32_t)start_pe == SYSINFO_START_PE)
        {
            /* Instruct startup PE0 */
            (void)D_COMM_StartupPE();
            /* enable PE startup state */
            (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_STARTPE, SYSINFO_ON);
        }
        else
        {
            /* Do nothing */
        }
    }
    /* infinity loop */
    (void)mh_SYSINFO_LoopInfinity();

    return ER_OK;

}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0030]:[ICUMFW_UD_MH_SYSINFO_MNG_0030]
* Function name: MH_SYSINFO_InterruptCallBack
* Description  : Send a system interrupt to PE when an interrupt occurs.
* Arguments    : (In)  factor : Cause of interrupt
*                                Range: ICUM_INT_SYSERR_LRAM_RMW
*                                       ICUM_INT_SYSERR_LRAM_AREA
*                                       ICUM_INT_SYSERR_BUS_ACC
*                                       ICUM_INT_SYSERR_CODE_FLASH
*                                       ICUM_INT_SYSERR_INST_FETCH
*                                       ICUM_INT_FEINT_SYSTEM_STOP
*                                       ICUM_INT_FEINT_ECCERR_SIC
*                                       ICUM_INT_FEINT_ECCERR_SICAB
*                                       ICUM_INT_FEINT_ECCERR_CRAM
*                                       ICUM_INT_FEINT_ECCERR_LRAM
*                                       ICUM_INT_FETRAPEX_OCC
*                                       ICUM_INT_EITRAP0_OCC
*                                       ICUM_INT_EITRAP1_OCC
*                                       ICUM_INT_RIEX_OCC
*                                       ICUM_INT_UCPOP_OCC
*                                       ICUM_INT_PIEX_OCC
*                                       ICUM_INT_MAEX_OCC
*                                       ICUM_INT_ILLEGAL_INTERRRUPT
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_SYSINFO_InterruptCallBack(uint32_t factor)
{
    uint32_t system_err = SYSINFO_ON;
    uint32_t err = SYSINFO_STOP_NG;
    
    /* check range */
    switch(factor)
    {
        case ICUM_INT_SYSERR_LRAM_RMW:
        case ICUM_INT_SYSERR_LRAM_AREA:
        case ICUM_INT_FEINT_ECCERR_CRAM:
        case ICUM_INT_FEINT_ECCERR_LRAM:
        case ICUM_INT_FEINT_ECCERR_PKRAM:
            /* enable local ram error state */
            (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_RAMERR, SYSINFO_ON);
            break;
        case ICUM_INT_SYSERR_BUS_ACC:
        case ICUM_INT_FEINT_ECCERR_SIC:
        case ICUM_INT_FEINT_ECCERR_SICAB:
        case ICUM_INT_FEINT_ECCERR_AESAXI:
            /* enable bus access error state */
            (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_BUSERR, SYSINFO_ON);
            break;
        case ICUM_INT_SYSERR_CODE_FLASH:
            /* enable code flash error state */
            (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_ROMERR, SYSINFO_ON);
            break;
        case ICUM_INT_FEINT_SYSTEM_STOP:
            err = mh_SYSINFO_TransitionStopMode();
            break;
        case ICUM_INT_EI_VOLTAGE_ERR:
        case ICUM_INT_EI_TEMPERATURE_ERR:
            /* enable code flash error state */
            (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_EXTERR, SYSINFO_ON);
            system_err = SYSINFO_OFF;
            break;
        default:
            /* included ICUM_INT_FEINT_SYSTEM_STOP */
            break;
    }

    if(err != SYSINFO_STOP_OK)
    {
        /* send a system interrupt to PE */
        (void)mh_SYSINFO_Interrupt(system_err);
    }
    else
    {
        /* Do nothing */
    }

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0031]:[ICUMFW_UD_MH_SYSINFO_MNG_0031]
* Function name: MH_SYSINFO_InterruptNMICallBack
* Description  : Send a system interrupt to PE when a FENMI interrupt occurs.
* Arguments    : (In)  factor : Cause of interrupt
*                                Range: ICUM_WDT_FENMI_EXT
*                                       ICUM_WDT_FENMI_ICUMWDTA0
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_SYSINFO_InterruptNMICallBack(uint32_t factor)
{
    sys_cfg_watchdog_t sys_cfg_wdt;

    (void)ML_CFG_GetSys_ram(ML_CFG_SYS_WDT , (uint32_t*)&sys_cfg_wdt);

    if(sys_cfg_wdt.erm == SYSINFO_WDT_RESETMODE)
    {
        (void)D_RESET_SetSysReset2();
    }
    else
    {
        /* Do nothing */
    }

    /* enable watch dog timer time out error state */
    if(factor == ICUM_WDT_FENMI_ICUMWDTA0)
    {
        (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_WDTERR, SYSINFO_ON);
    }
    else
    {
        /* Do nothing */
    }

    /* send a system interrupt to PE */
    (void)mh_SYSINFO_Interrupt(SYSINFO_ON);

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0032]:[ICUMFW_UD_MH_SYSINFO_MNG_0032]
* Function name: MH_SYSINFO_InterruptMPUCallBack
* Description  : Send a system interrupt to PE when a MPU interrupt occurs.
* Arguments    : (In)  factor : Cause of interrupt
*                                Range: MPU_MIP_INTERRUPT
*                                       MPU_MDP_INTERRUPT
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_SYSINFO_InterruptMPUCallBack(uint32_t factor)
{
    /* check range */
    switch(factor)
    {
        case MPU_MIP_INTERRUPT:
            break;
        case MPU_MDP_INTERRUPT:
            break;
        default:
            break;
    }
    /* send a system interrupt to PE */
    (void)mh_SYSINFO_Interrupt(SYSINFO_ON);

    return ER_OK;
}
#pragma ghs section text    = default
#pragma ghs section rodata  = default

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0033]:[ICUMFW_UD_MH_SYSINFO_MNG_0033]
* Function name: MH_SYSINFO_InterruptCLMACallBack()
* Description  : Send a system interrupt to PE when a CLMA interrupt occurs.
* Arguments    : N/A
* Return Value : ER_OK
***************************************************************************************/
/* Store in RAM statement */
CHANGE_SEC(text, ".icum_middle_ram")
CHANGE_SEC(rodata, ".icum_middle_const_ram")

uint32_t MH_SYSINFO_InterruptCLMACallBack(void)
{
    /* Error parameter */
    uint32_t err;
    /* System error parameter */
    uint32_t system_err;
    
    /* Initialize parameters */
    err = ER_OK;
    system_err = SYSINFO_ON;

    /* Set the status of CLMA error */
    err = D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_CLMAERR, system_err);

    if (ER_PARAM != err)
    {
        /* Send a system interrupt to PE */
        (void)mh_SYSINFO_Interrupt(system_err);
    }
    else
    {
        /* Do nothing */
    }
    return ER_OK;
}

/* Store in RAM statement ends*/
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)
