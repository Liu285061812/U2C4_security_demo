/******************************************************************************/
/* Component Name   ICUM CLK Driver                                           */
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

#if !defined(ICUM_D_CLK_PUB_H)
#define ICUM_D_CLK_PUB_H

/**************************************************************************************/
/* Return Value Definition                                                            */
/**************************************************************************************/
#define ER_D_CLK_WSTABLECLK_TOUT        (ER_D_CLK_BASE + 4UL)
#define ER_D_CLK_WSTABLEPLL_TOUT        (ER_D_CLK_BASE + 5UL)
#define ER_D_CLK_WSTABLEMULT_TOUT       (ER_D_CLK_BASE + 6UL)

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* CLK_HBUS Frequency (MOSC Clock Frequency = 400MHz) */
#define CLK_CLKFREQUENCY_100M           (100U)         /* HBUS_CLK Frequency Value(100MHz) */
#define CLK_CLKFREQUENCY_80M            (80U)          /* HBUS_CLK Frequency Value(80MHz)  */
#define CLK_ERRFREQUENCY_33M            (33U)          /* ERROR at CLK Initialization(33.3MHz) */
#define CLK_ERRFREQUENCY_25M            (25U)          /* ERROR at CLK Initialization(25MHz) */

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* Call Back Routine Definition */
typedef void (icum_clma_interrupt_type_t) (void);

typedef struct icum_clma_cb_regist
{
    /* A pointer to hold the callback function's address */
    icum_clma_interrupt_type_t        *p_cbfunc;
} icum_clma_cb_regist_t;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
uint32_t D_CLK_Init(void);
uint32_t D_CLK_GetClkFreq(uint16_t *p_clkfreq);
uint32_t D_CLK_StartCLMA(icum_clma_cb_regist_t *p_regist_info);
uint32_t D_CLK_StopCLMA(void);

#endif /* #if !defined(ICUM_D_CLK_PUB_H) */
