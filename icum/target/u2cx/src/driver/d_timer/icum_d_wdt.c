/******************************************************************************/
/* Component Name   ICUM Timer Driver                                         */
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
/* Includes <System Includes> , ProjectIncludes                                       */
/**************************************************************************************/
#include ICUM_DEVICE_FILE
#include "renesas_types.h"
#include "icum_int_declaration_pub.h"
#include "icum_d_wdt_pub.h"


/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/

/* Initial value of EI Level Interrupt Control Registers at Watchdog Timer 0 interrupt (at 75%) */
#define INTC1ICUM_ICICUWDTA0_INIT ((uint16_t)0x0007)

/* Writing the fixed activation code (ACH) generates the ICUM_WDTA trigger
 * and starts/restarts the ICUM_WDTAn counter. */
#define WDT_ACH                   ((uint8_t)0xACU)

/* EI Level Interrupt Control Register  */
#define EIRF                      ((uint32_t)(1UL << 12UL))

/* ICUWDTA0NMI has occurred flag (Read only) */
#define ICUM_WDTA0NMIF            ((uint32_t)(1UL << 8UL))
#define EXT_NMIF                  ((uint32_t)(1UL)

/**************************************************************************************/
/* Exported global variables (to be accessed by other files)                          */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Private global variables and functions                                             */
/**************************************************************************************/

/* Pointer of callback function */
static p_wdt_func_addr_t *g_callback = NULL;

/**************************************************************************************/
/* Private function prototypes                                                        */
/**************************************************************************************/
/* None */

/********************************************************************************
 API Function definition
*********************************************************************************/
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0001]:[ICUMFW_UD_U2B_D_TIMER_0001]
* Function name: D_WDT_Init
* Description  : Initialize D_WDT.
* Arguments    : p_init_prm_pstr:
*                Specify the operation parameters of WDT.
* Return Value : none
***************************************************************************************/
void D_WDT_Init (const s_WDT_init_prm_t *p_init_prm_pstr)
{
    uint32_t wk_reg_wdta0md;

    /* Set WDTA0OVF */
    wk_reg_wdta0md  = (((uint32_t)(p_init_prm_pstr->ovf) & 0x7UL) << 4UL);

    /* WDTA0WIE = 1 (Enables/disables the 75% interrupt request) */
    wk_reg_wdta0md |= (1UL << 3UL);

    /* WDTA0ERM =0 (WDT_ERM_NMI   : NMI request mode) */
    /*          =1 (WDT_ERM_RESET : NMI Reset mode) */
    wk_reg_wdta0md |= ((uint32_t)(p_init_prm_pstr->mode) << 2UL);

    /* WDTA0WS[1:0] = B'11 (window-open period is 100%) */
    wk_reg_wdta0md |= (0x03UL << 0UL);

    ICUM_WDTA0MD = (uint8_t)wk_reg_wdta0md;

    /* callback pointer latch */
    g_callback = p_init_prm_pstr->p_callback;

    /* ICUM_ICWDTA0 = 7: EI Level Interrupt Control Registers
        bit15 CTxxx This bit indicates an interrupt channel type. This bit is read only.
            * 0: Detection in synchronization with an edge is currently selected.
              1: Detection of the high level is currently selected.
        bit12 RFxxx
            * 0: No interrupt request is made.
              1: Interrupt request is made.
        bit7 MKxxx This is an interrupt request mask bit.
            * 0: Enables interrupt processing
              1: Disables interrupt processing
        bit6 TBxxx This bit is used to select the way to determine the interrupt vector
            * 0: Direct jumping to an address determined from the level of priority
              1: Referencing a table
        bit2 to 0 = 7 These bits specify the interrupt priority as one of 8 levels,
        with 0 as the highest and 7 as the lowest. */
    ICUM_INTC1ICUM_ICICUWDTA0 = INTC1ICUM_ICICUWDTA0_INIT;
}


/**************************************************************************************/
/* Memory allocation section specification                                            */
/*  D_WDT_Start                                                                       */
/*  D_WDT_GetIntRequest                                                               */
/**************************************************************************************/
#pragma ghs section text = ".icum_driver_ram"
#pragma ghs section rodata = ".icum_driver_const_ram"

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0002]:[ICUMFW_UD_U2B_D_TIMER_0002]
* Function name: D_WDT_Start
* Description  : Start or restart WDT OVF Counter
* Arguments    : none
* Return Value : none
***************************************************************************************/
void D_WDT_Start (void)
{
    /* ICUM_WDTA trigger and starts or restarts the ICUM_WDTA counter. */
    ICUM_WDTA0WDTE = WDT_ACH;
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_TIMER_0018]:[ICUMFW_UD_S4_D_TIMER_0018]
* Function name: D_WDT_StartIfNeeded
* Description  : WDT OVF Counter start or restart if needed
* Arguments    : none
* Return Value : none
***************************************************************************************/
void D_WDT_StartIfNeeded(void)
{
    bool wdt_flag;

    wdt_flag = false;

    /* Check the WDT 75% interrupt request flag value */
    D_WDT_GetIntRequest(&wdt_flag);
    if ((uint32_t)true == (uint32_t)wdt_flag)
    {
        /* restart watchdog timer */
        D_WDT_Start();
    }
    else
    {
        /* do nothing */
    }

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0003]:[ICUMFW_UD_U2B_D_TIMER_0003]
* Function name: D_WDT_GetIntRequest
* Description  : Check the Watchdog Timer 0 interrupt (at 75%)
* Arguments    : p_flag : true: 75% Interrupt Request flag set
* Return Value : none
***************************************************************************************/
void D_WDT_GetIntRequest (bool * p_flag)
{
    uint32_t wk_reg_eiic;

    wk_reg_eiic = ICUM_INTC1ICUM_ICICUWDTA0;

    if (0UL != (wk_reg_eiic & EIRF))
    {
        /* 75% Request Active */
        *p_flag = (uint8_t)true;

        /* Request Clear */
        ICUM_INTC1ICUM_ICICUWDTA0 = (uint16_t)(wk_reg_eiic ^ EIRF);
    }
    else
    {
        *p_flag = (uint8_t)false;
    }
}

#pragma ghs section text = default
#pragma ghs section rodata = default

/**************************************************************************************/
/* Memory allocation section specification                                            */
/*  FENMI_RAM                                                                         */
/*  FENMI                                                                             */
/**************************************************************************************/
#pragma ghs section text = ".icum_handler_ram"
#pragma ghs section rodata = ".icum_handler_const_ram"

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0004]:[ICUMFW_UD_U2B_D_TIMER_0004]
* Function name: FENMI_RAM (RAM mode)
* Description  : NMI Handler (WDT counter over flow)
* Arguments    : none
* Return Value : none
***************************************************************************************/
IRQ_HANDLER_FE (FENMI_RAM)
{
    uint32_t factor;
    
    /* confirm interrupt factor */
    if(ICUM_WDTA0NMIF == (ICUM_FENMIF & ICUM_WDTA0NMIF))
    {
        factor = ICUM_WDT_FENMI_ICUMWDTA0;
    }
    else
    {
        factor = ICUM_WDT_FENMI_EXT;
    }

    /* check Callback function and ICUWDTA0NMI has occurred */
    if(NULL != g_callback)
    {
        g_callback(factor);
    }
    else
    {
        /* nothing to do */
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0005]:[ICUMFW_UD_U2B_D_TIMER_0005]
* Function name: FENMI (ROM mode)
* Description  : NMI Handler (WDT counter over flow)
* Arguments    : none
* Return Value : none
***************************************************************************************/
IRQ_HANDLER_FE (FENMI)
{
    uint32_t factor;

    /* confirm interrupt factor */
    if(ICUM_WDTA0NMIF == (ICUM_FENMIF & ICUM_WDTA0NMIF))
    {
        factor = ICUM_WDT_FENMI_ICUMWDTA0;
    }
    else
    {
        factor = ICUM_WDT_FENMI_EXT;
    }

    /* check Callback function and ICUWDTA0NMI has occurred */
    if(NULL != g_callback)
    {
        g_callback(factor);
    }
    else
    {
        /* nothing to do */
    }
}

#pragma ghs section text = default
#pragma ghs section rodata = default

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0006]:[ICUMFW_UD_U2B_D_TIMER_0006]
* Function name: watchdog_restart
* Description  : Restart WDT OVF Counter
* Arguments    : none
* Return Value : none
***************************************************************************************/
#pragma ghs reference watchdog_restart
void watchdog_restart (void)
{
    /* ICUM_WDTA trigger and starts or restarts the ICUM_WDTA counter. */
    D_WDT_Start();
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0007]:[ICUMFW_UD_U2B_D_TIMER_0007]
* Function name: watchdog_get_restart_req
* Description  : Check the Watchdog Timer 0 interrupt (at 75%)
* Arguments    : none
* Return Value : WATCHDOG_ON: 75% Interrupt Request flag set
***************************************************************************************/
#pragma ghs reference watchdog_get_restart_req
watchdog_param_t watchdog_get_restart_req (void)
{
    bool flag;
    watchdog_param_t param;

    /* Get flag of 75% Interrupt Request */
    D_WDT_GetIntRequest(&flag);

    /* Check flag of 75% Interrupt Request */
    if((uint32_t)true == (uint32_t)flag)
    {
        /* INTICUWDTA0 occurred */
        param = WATCHDOG_ON;
    }
    else
    {
        /* No INTICUWDTA0 occurred */
        param = WATCHDOG_OFF;
    }

    return(param);
}

