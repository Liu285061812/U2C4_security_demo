/******************************************************************************/
/* Component Name   ICUM OS Timer Driver                                      */
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

#if !defined(ICUM_D_OST_H)
#define ICUM_D_OST_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Enumuration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* Softwaer timer context */
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
    /* Timer Intrrupt counter */
    uint32_t int_counter;
    /* Timer run_flag  false:STOP, true:RUN */
    bool     run_flag;
} s_OST_SoftTimerContext_t;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/

#endif /* #if !defined(ICUM_D_OST_H) */

