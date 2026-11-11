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

#if !defined(ICUM_D_OST_PUB_H)
#define ICUM_D_OST_PUB_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Error of Allocated All Software timer table elements  */
#define ER_D_TIMER_RESOURCE_EMPTY  (ER_D_TIMER_BASE)

/* One shot mode */
#define OST_MODE_ONESHOT ((uint32_t)0U)

/* Free run mode */
#define OST_MODE_REPEAT  ((uint32_t)1U)

/* Counts of D_OST_GetHardCounter() counts par 1 usec */
#define OST_COUNTS_OF_USEC ((uint32_t)100U)

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* OST initialize parameter */
typedef struct
{
    /* OST_MODE_ONESHOT or OST_MODE_REPEAT */
    uint32_t ovf;
    
    /* Timer setting time value [msec] */
    uint32_t time_ms;
    
    /* Pointer of function to callback */
    void(*p_callback)(void);
    
} s_OST_set_SoftTimer_t;


/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
void D_OST_Init(void);
void D_OST_GetRunTime(uint64_t *p_time);
void D_OST_GetHardCounter(uint32_t *p_counts);
void D_OST_ContinueCount(void);
uint32_t D_OST_AllocateTimer(const s_OST_set_SoftTimer_t *p_prm, uint8_t *p_id);
void D_OST_ReleaseTimer(uint8_t timer_id);
void D_OST_StartTimer(uint8_t timer_id);
void D_OST_StopTimer(uint8_t timer_id);

#endif /* #if !defined(ICUM_D_OST_PUB_H) */
