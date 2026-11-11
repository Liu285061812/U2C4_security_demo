/******************************************************************************/
/* Component Name  System / Life Cycle Manager (Secure Tick Timer)            */
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
#include "icum_d_clk_pub.h"
#include "icum_d_ost_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_ml_cfg_pub.h"
#include "icum_mh_sysinfo_pub.h"
#include "icum_mh_sysinfo.h"

/**************************************************************************************/
/* Definitions                                                                        */
/**************************************************************************************/
#define SYSINFO_TICK_TID_DEFAULT  (0xFFu)
#define SYSINFO_TICK_MIN          (10uL)
#define SYSINFO_TICK_DEFAULT      (10000uL)
#define SYSINFO_TICK_KHZ          (1000uL)
#define SYSINFO_TICK_COUNT_NUM    (5uL)
#define SYSINFO_TICK_EMPTY        (0uL)
#define SYSINFO_TICK_EMPTY_LONG   (0uLL)

/**************************************************************************************/
/* Global Variables                                                                   */
/**************************************************************************************/
/* Software Timer ID */
static uint8_t  g_sysinfo_tid = SYSINFO_TICK_TID_DEFAULT;
/* Tick Counter save area */
static uint32_t g_sysinfo_save_count[SYSINFO_TICK_COUNT_NUM];

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
static uint32_t mh_SYSINFO_StartTickTimer(void);

/**************************************************************************************/
/* Functions                                                                          */
/**************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0015]:[ICUMFW_UD_MH_SYSINFO_MNG_0015]
* Function name: MH_SYSINFO_GetTickCount
* Description  : Get elapsed time since start of base timer from Timer Driver.
* Arguments    : (Out) p_info : Secure tick property
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_SYSINFO_GetTickCount(secure_tick_info_t *p_info)
{
    uint64_t tick_count;

    tick_count = SYSINFO_TICK_EMPTY_LONG;

    /* This API can not get 8 byte's value */
    (void)D_OST_GetRunTime(&tick_count);
    /* get tick counter */
    p_info->counter = tick_count;

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0016]:[ICUMFW_UD_MH_SYSINFO_MNG_0016]
* Function name: MH_SYSINFO_SaveTickCount
* Description  : Save the current tick counter in the area specified by tick counter ID.
* Arguments    : (In)  id : Tick counter ID
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MH_SYSINFO_SaveTickCount(uint8_t id)
{
    uint32_t err = ER_OK;
    uint32_t tick_count = 0uL;
    uint16_t mhz_freq = 0u;

    /* check range */
    if((uint32_t)id < SYSINFO_TICK_COUNT_NUM)
    {
        /* get hardware timer counter */
        (void)D_OST_GetHardCounter(&tick_count);
        /* get clock frequency(MHz) */
        (void)D_CLK_GetClkFreq(&mhz_freq);
        /* check frequency */
        if((uint32_t)mhz_freq != 0uL)
        {
            /* save tick counter(us) */
            g_sysinfo_save_count[id] = tick_count / (uint32_t)mhz_freq;
        }
        else
        {
            /* set 0ms */
            g_sysinfo_save_count[id] = SYSINFO_TICK_EMPTY;
        }
    }
    else
    {
        err = ER_PARAM;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0017]:[ICUMFW_UD_MH_SYSINFO_MNG_0017]
* Function name: MH_SYSINFO_GetSavedTickCount
* Description  : Get tick counter from the area specified by tick counter ID.
* Arguments    : (In)  id      : Tick counter ID
*                (Out) p_count : Tick counter
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MH_SYSINFO_GetSavedTickCount(uint8_t id, uint32_t *p_count)
{
    uint32_t err = ER_OK;

    /* check range */
    if((uint32_t)id < SYSINFO_TICK_COUNT_NUM)
    {
        /* get tick counter(ms) */
        *p_count = g_sysinfo_save_count[id];
    }
    else
    {
        err = ER_PARAM;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0018]:[ICUMFW_UD_MH_SYSINFO_MNG_0018]
* Function name: MH_SYSINFO_SetTickInterval
* Description  : Set the software timer interrupt interval.
* Arguments    : (In)  interval : software timer interrupt interval
* Return Value : ER_OK
*                ER_PENDING
***************************************************************************************/
uint32_t MH_SYSINFO_SetTickInterval(uint16_t interval)
{
    uint32_t err;
    uint32_t set_interval;

    /* already started software timer ? */
    if((uint32_t)g_sysinfo_tid != (uint32_t)SYSINFO_TICK_TID_DEFAULT)
    {
        /* stop software timer */
        (void)D_OST_StopTimer(g_sysinfo_tid);
        /* release software timer resource */
        (void)D_OST_ReleaseTimer(g_sysinfo_tid);
        /* initialize software timer */
        g_sysinfo_tid = SYSINFO_TICK_TID_DEFAULT;
    }
    else
    {
        /* Do nothing */
    }
    /* check interval value */
    if((uint32_t)interval < SYSINFO_TICK_MIN)
    {
        /* set default value(10000ms) */
        set_interval = SYSINFO_TICK_DEFAULT;
    }
    else
    {
        /* set input value */
        set_interval = interval;
    }
    /* get secure tick interval */
    (void)ML_CFG_SetSys(ML_CFG_SYS_SECURE_TICK, &set_interval);
    /* start secure tick */
    err = mh_SYSINFO_StartTickTimer();

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0020]:[ICUMFW_UD_MH_SYSINFO_MNG_0020]
* Function name: MH_SYSINFO_GetTickFreq
* Description  : Get the hardware clock frequency.
* Arguments    : (Out) p_freq : Hardware clock frequency
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_SYSINFO_GetTickFreq(uint32_t *p_freq)
{
   uint16_t mhz_freq = 0u;

   /* get clock frequency(MHz) */
   (void)D_CLK_GetClkFreq(&mhz_freq);
   /* convert the unit of frequency from MHz to kHz */
   *p_freq = (uint32_t)mhz_freq * SYSINFO_TICK_KHZ;

   return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0019]:[ICUMFW_UD_MH_SYSINFO_MNG_0019]
* Function name: mh_SYSINFO_StartTickTimer
* Description  : Restart the software timer.
* Arguments    : N/A
* Return Value : ER_OK
*                ER_PENDING
***************************************************************************************/
static uint32_t mh_SYSINFO_StartTickTimer(void)
{
    uint32_t err;
    uint32_t interval = 0uL;
    s_OST_set_SoftTimer_t callback_info;

    /* get secure tick interval */
    (void)ML_CFG_GetSys(ML_CFG_SYS_SECURE_TICK, &interval);
    /* set software timer setting */
    callback_info.ovf        = OST_MODE_REPEAT;
    callback_info.time_ms    = interval;
    callback_info.p_callback = &mh_SYSINFO_SendSysInterrupt;
    /* register callback function */
    err = D_OST_AllocateTimer(&callback_info, &g_sysinfo_tid);
    /* check return value */
    if(ER_OK == err)
    {
        /* restart software timer */
        (void)D_OST_StartTimer(g_sysinfo_tid);
    }
    else
    {
        /* need to retry */
        err = ER_PENDING;
    }

    return err;
}
