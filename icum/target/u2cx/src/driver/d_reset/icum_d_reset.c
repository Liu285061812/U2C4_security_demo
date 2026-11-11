/******************************************************************************/
/* Component Name   ICUM RESET Driver                                         */
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

/* Header Files                         */
#include ICUM_DEVICE_FILE
#include "renesas_types.h"

#include "u2b_reg_adr.h"

#include "icum_common_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_reset_pub.h"
#include "icum_d_reset.h"
/***************************************************************************************/
/* Internal Definition                                                                 */
/***************************************************************************************/
/* None */

/***************************************************************************************/
/* Static                                                                              */
/***************************************************************************************/
/* None */

/***************************************************************************************/
/* Prototypes (Internal)                                                               */
/***************************************************************************************/
/* None */

/***************************************************************************************
* Functions
***************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_RESET_0001]:[ICUMFW_UD_U2B_D_RESET_0001]
* Function name: D_RESET_Init
* Description  : RESET Driver Initialization
* Arguments    : None
*                
* Return Value : ER_OK
***************************************************************************************/
uint32_t D_RESET_Init(void)
{
    return (ER_OK);
}

CHANGE_SEC(text, ".icum_driver_ram")
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_RESET_0002]:[ICUMFW_UD_U2B_D_RESET_0002]
* Function name: D_RESET_SetSysReset2
* Description  : Set Reset Request of System Reset 2
* Arguments    : None
*                
* Return Value : ER_OK
***************************************************************************************/
uint32_t D_RESET_SetSysReset2(void)
{
    /* SYSCTRL Register */
    volatile  uint32_t   *sysctrl_reg;

    /* 1. Unlock (Enables) write access of protected registers */
    UNLOCK_REG_PROT(ICUM_SYSCTRLICUM_KCPROT0);

    /* 2. Generate System Reset 2 (SWSRESA_ICUM Register) */
    sysctrl_reg = (volatile uint32_t *)&ICUM_SYSCTRL.SWSRESA_ICUM;
    *sysctrl_reg = (uint32_t)RESET_SYSRESET2GEN;

    /* 3. Lock (Disable) write access of protected registers */
    LOCK_REG_PROT(ICUM_SYSCTRLICUM_KCPROT0);

    return (ER_OK);
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/***************************************************************************************
* Function ID  : (not supported)
* Function name: D_RESET_SetAppReset
* Description  : Set Reset Request of Application Reset
* Arguments    : None
*                
* Return Value : ER_OK
***************************************************************************************/
uint32_t D_RESET_SetAppReset(void)
{
    /* SYSCTRL Register */
    volatile  uint32_t   *sysctrl_reg;

    /* 1. Unlock (Enables) write access of protected registers */
    UNLOCK_REG_PROT(ICUM_SYSCTRLICUM_KCPROT0);

    /* 2. Generate Application Reset (SWARESA_ICUM Register) */
    sysctrl_reg = (volatile uint32_t *)&ICUM_SYSCTRL.SWARESA_ICUM;
    *sysctrl_reg = (uint32_t)RESET_APPLIRESETGEN;

    /* 3. Lock (Disable) write access of protected registers */
    LOCK_REG_PROT(ICUM_SYSCTRLICUM_KCPROT0);

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : (not supported)
* Function name: D_RESET_SetModReset
* Description  : Set Reset Request of Module Reset
* Arguments    : None
*                
* Return Value : ER_OK
***************************************************************************************/
uint32_t D_RESET_SetModReset(void)
{
    /* SYSCTRL Register */
    volatile  uint32_t   *sysctrl_reg;

    /* 1. Unlock (Enables) write access of protected registers */
    UNLOCK_REG_PROT(ICUM_SYSCTRLICUM_KCPROT0);

    /* 2. Generate Module Reset (SWMRESA_ICUM Register) */
    sysctrl_reg = (volatile uint32_t *)&ICUM_SYSCTRL.SWMRESA_ICUM;
    *sysctrl_reg = (uint32_t)RESET_MODULERESETGEN;

    /* 3. Lock (Disable) write access of protected registers */
    LOCK_REG_PROT(ICUM_SYSCTRLICUM_KCPROT0);

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_RESET_0005]:[ICUMFW_UD_U2B_D_RESET_0005]
* Function name: D_RESET_SetStopModEnable
* Description  : Enable STOP Mode for Module Reset
* Arguments    : None
*                
* Return Value : ER_OK
***************************************************************************************/
uint32_t D_RESET_SetStopModEnable(void)
{
    uint32_t             count = MISC_PROT_RETRY;

    /* 1,2,3. Enable STOP mode transition */
    WRITE_PROT_REG(ICUM_CTLREGICUM_STOP, RESET_STOPRQENABLE, count);

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_RESET_0006]:[ICUMFW_UD_U2B_D_RESET_0006]
* Function name: D_RESET_GetResetFactor
* Description  : Get Reset Factor
* Arguments    : p_status  : Area Address for setting Reset Factor
*                
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_RESET_GetResetFactor(uint32_t *p_status)
{
    uint32_t             uiRet = ER_OK;
    
    /* 1. Check NULL pointer */
    if (NULL != p_status)
    {
        /* 1-1. Execute Following Procedure */
        /* 2. Read Reset Factor Register */
        *p_status = SYSCTRL.RESF.UINT32;
    }
    else
    {
        /* 1-2. When Pointer is NULL */
        uiRet = ER_PARAM;
    }

    return (uiRet);
}

CHANGE_SEC(text, ".icum_driver_ram")
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_RESET_0007]:[ICUMFW_UD_U2B_D_RESET_0007]
* Function name: D_RESET_TransitionStopMode
* Description  : Transition to StopMode
* Arguments    : None
*                
* Return Value : ER_D_RESET_STOP_OK
*                ER_D_RESET_STOP_NG
***************************************************************************************/
uint32_t D_RESET_TransitionStopMode(void)
{
    uint32_t             uiRet = ER_D_RESET_STOP_NG;

    if((ICUM_CTLREGICUM_STOP & RESET_STOPRQCHECK) == RESET_STOPRQCHECK)
    {
        /* Stand-by request permission */
        (void)D_RESET_SetStopModEnable();

        /* Stand-by mode enter */
        if(SYSCTRL.STBC0PSC.UINT32 == RESET_STBC0DISTRG)
        {
            /* DeepSTOP mode */
            while(SYSCTRL.STBC0PSC.UINT32 == RESET_STBC0DISTRG)
            {
                /* Wait for STBC0DISTRG */
            }
        }
        else
        {
            /* STOP mode */
            while(SYSCTRL.STBC0STPT.UINT32 == RESET_STBC0STPTRG)
            {
                /* Wait for STBC0STPTRG */
            }
        }
        uiRet = ER_D_RESET_STOP_OK;
    }
    else
    {
        /* Do nothing */
    }

    return (uiRet);
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

