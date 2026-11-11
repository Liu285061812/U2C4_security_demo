/******************************************************************************/
/* Component Name   Testing the ICU-M                                         */
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

void sys_watchdog_stop(void);

unsigned long sys_clock_quartzfreq(void);
void sys_clock_maxfreq(void);

void sys_vtimer0_start(void);
void sys_vtimer1_start(void);

unsigned long sys_vtimer0_get_pclk_counter(void);

typedef struct 
{
    unsigned long  sec;     /* second : 0 - 999999999 [sec]  */
    unsigned short msec;    /* msec   : 0 - 999 [msec] */
    unsigned short usec;    /* usec   : 0 - 999 [usec] */
    unsigned short nsec;    /* nsec   : 0 - 999 [nsec] */
}time_t;

unsigned long sys_vtimer0_cnv_pclk2time(const unsigned long long pclk_counter,time_t *time);
unsigned long sys_vtimer0_cnv_pclk2time_pe2(const unsigned long long pclk_counter,time_t *time);

void sys_vuart_config(void);
unsigned char sys_vuart_getbyte(void);
void sys_vuart_sendbyte(unsigned char byte_to_send);

void sys_vport_config(void);
void sys_vport_set_high(unsigned char n);
void sys_vport_set_low(unsigned char n);

void  sys_vtimer1_set_func(void (*pfunc)(),unsigned long time_ms);
void  sys_vtimer1_start(void);

void BENCH_CPU_SPEED(unsigned long count);

unsigned char GetPEID(void);
