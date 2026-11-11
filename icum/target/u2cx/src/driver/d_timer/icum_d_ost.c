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
/* Headers                                                                            */
/**************************************************************************************/
#include ICUM_DEVICE_FILE
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "icum_int_declaration_pub.h"
#include "icum_d_ost_pub.h"
#include "icum_d_clk_pub.h"

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Number of resources for soft timer */
#define NUMBER_OF_SOFT_TIMER    ((uint32_t)8U)

/* Released the resource of soft timer */
#define RELEASED_SOFT_TIMER     ((uint32_t)0UL)

/* H - Bus clock Slowest value (25MHz) */
#define CLK_HBUS                ((uint32_t)(25UL*1000UL*1000UL))

/* ICUM_OSTMn Unit Clock Name */
#define PCLK                    (CLK_HBUS)

/* 10m secs is 100Hz */
#define F_100HZ                 ((uint32_t)(100UL))

/* Number of counts for 10m sec */
#define COUNT_10M_SEC_FOR_ERR   (PCLK/F_100HZ)

#define INTRRUPT_INTERVAL_M_SEC ((uint32_t)10UL)

/* Value when counter is Stopped  */
#define OVER_COUNTER            ((uint32_t)0xffffffffUL)

/* Stops the counter and clears the REG_ICUM_OSTM0.TE.OSTMnTE bit */
#define STOP_TRIGGER            ((uint8_t)0x01U)

/* Starts the counter and sets REG_ICUM_OSTM0.TE.OSTMnTE = 1 */
#define START_TRIGGER           ((uint8_t)0x01U)

/* OSTM0MD1 = 1:Freerun mode */
#define FREERUN_MODE            ((uint8_t)0x02U)

/* Invalid soft timer id (0) */
#define INVALID_ID              (0UL)

/* First id of soft timer */
#define FIRST_ID                (1UL)

/*  */
#define OST_ZERO                (0UL)

/* Indicate soft timer is running. */
#define SOFT_TIMER_RUN          ((uint8_t)true)

/* Indicate soft timer is stopped. */
#define SOFT_TIMER_STOP         ((uint8_t)false)

/**************************************************************************************/
/* Exported global variables (to be accessed by other files)                          */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* Software timer context */
typedef struct
{
    /* Selects the timer mode */
    uint32_t mode;
    /* Timer setting time value[msec], 0:means Not Allocated resource */
    uint32_t time_ms;
    /* Pointer of function to callback */
    void (*p_callback)(void);
    /* Timer counter */
    uint32_t counter;
    /* Timer Interrupt counter */
    uint32_t int_counter;
    /* Timer run_flag  false:STOP, true:RUN */
    bool     run_flag;

} s_OST_SoftTimerContext_t;

/**************************************************************************************/
/* Private global variables and functions                                             */
/**************************************************************************************/
/* Table of soft timers */
static volatile s_OST_SoftTimerContext_t g_osttbl[NUMBER_OF_SOFT_TIMER];

/* Counts of interrupt from D_OST_ContinueCount() */
static uint32_t g_intrrupt_count;

/* Number of counts for 10m sec */
static uint64_t g_ostimer_count;

/**************************************************************************************/
/* Private function prototypes                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* API Function definition                                                            */
/**************************************************************************************/
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0008]:[ICUMFW_UD_U2B_D_TIMER_0008]
* Function name: D_OST_Init
* Description  : Initialize OS Timer
*                Set mode of OS Timer, Initialize global variables for soft timer
* Arguments    : none
* Return Value : none
***************************************************************************************/
void D_OST_Init (void)
{
    uint32_t i;
    uint32_t err;
    uint16_t mhz_freq;

    err = ER_OK;
    mhz_freq = 0U;

    /* g_osttbl[NUMBER_OF_SOFT_TIMER */
    for (i = OST_ZERO; NUMBER_OF_SOFT_TIMER > i; i++)
    {
        g_osttbl[i].time_ms = RELEASED_SOFT_TIMER;
        g_osttbl[i].run_flag = SOFT_TIMER_STOP;
        g_osttbl[i].p_callback = NULL;
    }
    g_intrrupt_count = OST_ZERO;

    /* Stops the counter and clears the REG_ICUM_OSTM0.TE.OSTMnTE bit. */
    ICUM_OSTM0TT = STOP_TRIGGER;

    /* OSTM0MD1 = 1:Freerun mode */
    ICUM_OSTM0CTL = FREERUN_MODE;

    /* get clock frequency(MHz) */
    err = D_CLK_GetClkFreq(&mhz_freq);
    if (ER_OK == err)
    {
        g_ostimer_count = ((uint64_t)(mhz_freq * 1000UL * 1000UL) / F_100HZ);
    }
    else
    {
        g_ostimer_count = COUNT_10M_SEC_FOR_ERR;
    }

    /* Set Next Compare */
    ICUM_OSTM0CMP = g_ostimer_count;
}

/**************************************************************************************/
/* Memory allocation section specification                                            */
/*  D_OST_GetRunTime                                                                  */
/*  D_OST_GetHardCounter                                                              */
/*  D_OST_ContinueCount                                                               */
/*  D_OST_AllocateTimer                                                               */
/*  D_OST_ReleaseTimer                                                                */
/*  D_OST_StartTimer                                                                  */
/*  D_OST_StopTimer                                                                   */
/**************************************************************************************/
#pragma ghs section text = ".icum_driver_ram"
#pragma ghs section rodata = ".icum_driver_const_ram"

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0009]:[ICUMFW_UD_U2B_D_TIMER_0009]
* Function name: D_OST_GetRunTime
* Description  : Elapsed time since the base timer started
* Arguments    : p_time : time(msec) from D_OST_ContinueCount()
* Return Value : none
***************************************************************************************/
void D_OST_GetRunTime (uint64_t *p_time)
{
    /* Check the  g_intrrupt_count is Over (0xFFFF FFFF/10) msec */
    if ((OVER_COUNTER / INTRRUPT_INTERVAL_M_SEC) >= g_intrrupt_count)
    {
        /* It's count of interrupt x 10 */
        *p_time = g_intrrupt_count * INTRRUPT_INTERVAL_M_SEC;
    }
    else
    {
        /* Counter Stop */
        *p_time = OVER_COUNTER;
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0010]:[ICUMFW_UD_U2B_D_TIMER_0010]
* Function name: D_OST_GetHardCounter
* Description  : Get Timer counts from a OST hardware
                 Read counter register, Set counts to *p_counts
* Arguments    : p_counts : Counter value(10 nano sec/count) of hardware
* Return Value : none
***************************************************************************************/
void D_OST_GetHardCounter (uint32_t *p_counts)
{
    /* Read OSTM0 Counter Register */
    *p_counts = ICUM_OSTM0CNT;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0011]:[ICUMFW_UD_U2B_D_TIMER_0011]
* Function name: D_OST_ContinueCount
* Description  : Start increments a OST hardware counter
*                Set Start trigger to Start register
* Arguments    : none
* Return Value : none
***************************************************************************************/
void D_OST_ContinueCount (void)
{
    /* Starts the counter and sets REG_ICUM_OSTM0.TE.OSTMnTE = 1. */
    /* ICUM_OSTM0CNT will be clear to 0 */
    ICUM_OSTM0TS = START_TRIGGER;

}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0012]:[ICUMFW_UD_U2B_D_TIMER_0012]
* Function name: D_OST_AllocateTimer
* Description  : Get a resource of soft timer with specified setting
* Arguments    : p_prm : Setting value of a soft timer
                 p_id  : Pointer of timer ID
* Return Value : Error code
***************************************************************************************/
uint32_t D_OST_AllocateTimer (const s_OST_set_SoftTimer_t *p_prm, uint8_t *p_id)
{
    uint32_t ret;
    uint32_t i;

    ret = ER_D_TIMER_RESOURCE_EMPTY;

    /* Check parameters */
    if ((RELEASED_SOFT_TIMER == p_prm->time_ms) ||
        ((OST_MODE_ONESHOT != p_prm->ovf) && (OST_MODE_REPEAT != p_prm->ovf)) )
    {
        ret = (uint32_t)ER_PARAM;
    }
    else
    {
        /* g_osttbl[NUMBER_OF_SOFT_TIMER] */
        for (i = OST_ZERO; NUMBER_OF_SOFT_TIMER > i; i++)
        {
            if (RELEASED_SOFT_TIMER == g_osttbl[i].time_ms)
            {
                /* Copy OST initialize parameter to Software timer context */
                g_osttbl[i].mode = p_prm->ovf;
                g_osttbl[i].time_ms = p_prm->time_ms;
                g_osttbl[i].p_callback = p_prm->p_callback;

                /* Initialize context */
                g_osttbl[i].run_flag = SOFT_TIMER_STOP;
                g_osttbl[i].counter = OST_ZERO;
                g_osttbl[i].int_counter = OST_ZERO;

                /* Get timer_id from index */
                *p_id = (uint8_t)(i + FIRST_ID);
                ret = ER_OK;
                break;
            }
            else
            {
                /* go to next element */
            }
        }
    }
    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0013]:[ICUMFW_UD_U2B_D_TIMER_0013]
* Function name: D_OST_ReleaseTimer
* Description  : Release the resource of soft timer
* Arguments    : timer_id : Timer ID
* Return Value : none
***************************************************************************************/
void D_OST_ReleaseTimer (uint8_t timer_id)
{
    uint32_t i;

    /* Check the parameter */
    if (NUMBER_OF_SOFT_TIMER < (uint32_t)timer_id)
    {
        /* Parameter error */
    }
    else if (INVALID_ID == (uint32_t)timer_id)
    {
        /* Parameter error */
    }
    else
    {
        /* Release specified  soft timer */
        i = (uint32_t)timer_id - FIRST_ID;
        g_osttbl[i].run_flag = SOFT_TIMER_STOP;
        g_osttbl[i].time_ms = RELEASED_SOFT_TIMER;
    }
    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0014]:[ICUMFW_UD_U2B_D_TIMER_0014]
* Function name: D_OST_StartTimer
* Description  : Start or restart specified soft timer
* Arguments    : timer_id : Timer ID
* Return Value : none
***************************************************************************************/
void D_OST_StartTimer (uint8_t timer_id)
{
    uint32_t i;

    /* Check the parameter */
    if (NUMBER_OF_SOFT_TIMER < (uint32_t)timer_id)
    {
        /* Parameter error */
    }
    else if (INVALID_ID == (uint32_t)timer_id)
    {
        /* Parameter error */
    }
    else
    {
        /* Start specified soft timer */
        i = (uint32_t)timer_id - FIRST_ID;

        if ((RELEASED_SOFT_TIMER != g_osttbl[i].time_ms) &&
                ((uint32_t)SOFT_TIMER_STOP == (uint32_t)g_osttbl[i].run_flag))
        {
            g_osttbl[i].counter = g_osttbl[i].time_ms / INTRRUPT_INTERVAL_M_SEC;
            if (OST_ZERO != (g_osttbl[i].time_ms % INTRRUPT_INTERVAL_M_SEC))
            {
                /* Round up */
                g_osttbl[i].counter++;
            }
            else
            {
                /* NOT Round up */
            }
            g_osttbl[i].run_flag = SOFT_TIMER_RUN;
        }
        else
        {
            /* Inactive timer */
        }
    }
    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0015]:[ICUMFW_UD_U2B_D_TIMER_0015]
* Function name: D_OST_StopTimer
* Description  : Stop specified soft timer
* Arguments    : timer_id : Timer ID
* Return Value : none
***************************************************************************************/
void D_OST_StopTimer (uint8_t timer_id)
{
    uint32_t i;

    /* Check the parameter */
    if (NUMBER_OF_SOFT_TIMER < (uint32_t)timer_id)
    {
        /* Parameter error */
    }
    else if (INVALID_ID == (uint32_t)timer_id)
    {
        /* Parameter error */
    }
    else
    {
        /* specified a resource */
        i = (uint32_t)timer_id - FIRST_ID;
        if ((uint32_t)SOFT_TIMER_RUN == (uint32_t)g_osttbl[i].run_flag)
        {
            /* Stop soft timer counter */
            g_osttbl[i].run_flag = SOFT_TIMER_STOP;
        }
        else
        {
            /* Stopped timer id */
        }
    }
    return;
}

#pragma ghs section text = default
#pragma ghs section rodata = default

/**************************************************************************************/
/* Memory allocation section specification                                            */
/*  INTICUOSTM0_RAM                                                                   */
/**************************************************************************************/
#pragma ghs section text = ".icum_handler_ram"
#pragma ghs section rodata = ".icum_handler_const_ram"

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0016]:[ICUMFW_UD_U2B_D_TIMER_0016]
* Function name: INTICUOSTM0_RAM
* Description  : interrupt handler for INTICUOSTM0 (RAM mode)
* Arguments    : none
* Return Value : none
***************************************************************************************/
void INTICUOSTM0_RAM (void)
{
    uint32_t i;

    /* Set Next */
    ICUM_OSTM0CMP += g_ostimer_count;

    /* When the g_intrrupt_count is not stopped */
    /* (g_intrrupt_count * INTRRUPT_INTERVAL_M_SEC is less than OVER_COUNTER) */
    if ((OVER_COUNTER / INTRRUPT_INTERVAL_M_SEC) >= g_intrrupt_count)
    {
        g_intrrupt_count++;
    }
    else
    {
        /* Stopped g_intrrupt_count, nothing to do. */
    }

    /* Scan All record */
    for (i = OST_ZERO; NUMBER_OF_SOFT_TIMER > i; i++)
    {
        /* Check active record */
        if ((uint32_t)SOFT_TIMER_RUN == (uint32_t)g_osttbl[i].run_flag)
        {
            g_osttbl[i].counter--;
            g_osttbl[i].int_counter++;
            if (OST_ZERO == g_osttbl[i].counter)
            {
                if (OST_MODE_ONESHOT == g_osttbl[i].mode)
                {
                    /* Stop soft timer counter */
                    g_osttbl[i].run_flag = SOFT_TIMER_STOP;
                }
                else
                {
                    g_osttbl[i].counter = g_osttbl[i].time_ms / INTRRUPT_INTERVAL_M_SEC;
                    if (OST_ZERO != (g_osttbl[i].time_ms % INTRRUPT_INTERVAL_M_SEC))
                    {
                        /* Round up */
                        g_osttbl[i].counter++;
                    }
                    else
                    {
                        /* Not Round up  */
                    }
                }

                /* Check callback function */
                if (NULL != g_osttbl[i].p_callback)
                {
                    g_osttbl[i].p_callback();
                }
                else
                {
                    /* Unregistered callback function, nothing to do. */
                }
            }
            else
            {
                /* Stopped this soft timer, nothing to do. */
            }
        }
        else
        {
            /* Stopped this soft timer, nothing to do. */
        }
    }
}
#pragma ghs section text = default
#pragma ghs section rodata = default
/**************************************************************************************/
/* Memory allocation section specification                                            */
/*  INTICUOSTM0_RAM                                                                   */
/**************************************************************************************/
#pragma ghs section text    = ".icum_handler.text"
#pragma ghs section rodata  = ".icum_handler.rodata"

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TIMER_0017]:[ICUMFW_UD_U2B_D_TIMER_0017]
* Function name: INTICUOSTM0
* Description  : interrupt handler for INTICUOSTM0 (ROM mode)
* Arguments    : none
* Return Value : none
***************************************************************************************/
void INTICUOSTM0 (void)
{
    uint32_t i;

    /* Set Next */
    ICUM_OSTM0CMP += g_ostimer_count;

    /* When the g_intrrupt_count is not stopped */
    /* (g_intrrupt_count * INTRRUPT_INTERVAL_M_SEC is less than OVER_COUNTER) */
    if ((OVER_COUNTER / INTRRUPT_INTERVAL_M_SEC) >= g_intrrupt_count)
    {
        g_intrrupt_count++;
    }
    else
    {
        /* Stopped g_intrrupt_count, nothing to do. */
    }

    /* Scan All record */
    for (i = OST_ZERO; NUMBER_OF_SOFT_TIMER > i; i++)
    {
        /* Check active record */
        if ((uint32_t)SOFT_TIMER_RUN == (uint32_t)g_osttbl[i].run_flag)
        {
            g_osttbl[i].counter--;
            g_osttbl[i].int_counter++;
            if (OST_ZERO == g_osttbl[i].counter)
            {
                if (OST_MODE_ONESHOT == g_osttbl[i].mode)
                {
                    /* Stop soft timer counter */
                    g_osttbl[i].run_flag = SOFT_TIMER_STOP;
                }
                else
                {
                    g_osttbl[i].counter = g_osttbl[i].time_ms / INTRRUPT_INTERVAL_M_SEC;
                    if (OST_ZERO != (g_osttbl[i].time_ms % INTRRUPT_INTERVAL_M_SEC))
                    {
                        /* Round up */
                        g_osttbl[i].counter++;
                    }
                    else
                    {
                        /* Not Round up  */
                    }
                }

                /* Check callback function */
                if (NULL != g_osttbl[i].p_callback)
                {
                    g_osttbl[i].p_callback();
                }
                else
                {
                    /* Unregistered callback function, nothing to do. */
                }
            }
            else
            {
                /* Stopped this soft timer, nothing to do. */
            }
        }
        else
        {
            /* Stopped this soft timer, nothing to do. */
        }
    }
}
#pragma ghs section text = default
#pragma ghs section rodata = default
