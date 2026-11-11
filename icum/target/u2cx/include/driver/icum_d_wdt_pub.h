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

#if !defined(ICUM_D_WDT_PUB_H)
#define ICUM_D_WDT_PUB_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* NMI request mode */
#define WDT_ERM_NMI   ((uint8_t)0)

/* Reset Mode */
#define WDT_ERM_RESET ((uint8_t)1)

/* 64msec */
#define WDT_OVF_64MS  ((uint8_t)5)

/* 128msec */
#define WDT_OVF_128MS ((uint8_t)6)

/* 256msec */
#define WDT_OVF_256MS ((uint8_t)7)

/* Interrupt factor */
#define ICUM_WDT_FENMI_EXT         (1UL)
#define ICUM_WDT_FENMI_ICUMWDTA0   (2UL)

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
typedef enum
{
    WATCHDOG_OFF,
    WATCHDOG_ON
} watchdog_param_t;

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/

/*  WDT initialize parameter */
typedef void (p_wdt_func_addr_t) (uint32_t int_fact);

typedef struct
{
    /* Mode is WDT_ERM_NMI or WDT_ERM_RESET */
    uint8_t mode;

    /* Over flow time is WDT_OVF_2MS to WDT_OVF_256MS */
    uint8_t ovf;

    /* Pointer of callback function (NMI Handler) */
    p_wdt_func_addr_t * p_callback;
} s_WDT_init_prm_t;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
void D_WDT_Init(const s_WDT_init_prm_t *p_init_prm_pstr);
void D_WDT_Start(void);
void D_WDT_StartIfNeeded(void);
void D_WDT_GetIntRequest(bool *p_flag);

#endif /* #if !defined(ICUM_D_WDT_PUB_H) */
