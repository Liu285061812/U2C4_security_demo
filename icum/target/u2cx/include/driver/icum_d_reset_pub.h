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

#if !defined(ICUM_D_RESET_PUB_H)
#define ICUM_D_RESET_PUB_H

/**************************************************************************************/
/* Return Value Definition                                                            */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
#define ER_D_RESET_STOP_OK         (ER_D_RESET_BASE + 0uL)
#define ER_D_RESET_STOP_NG         (ER_D_RESET_BASE + 1uL)

/* Reset Factor */
#define RESET_NOFACTOR             (0x00000000UL)      /* No Factor */
#define RESET_PRESF0               (0x00000001UL)      /* Power Up Reset */
#define RESET_SRES1F0              (0x00000002UL)      /* Stand-by Reset */
#define RESET_SRES1F1              (0x00000004UL)      /* External Reset */
#define RESET_SRES1F2              (0x00000008UL)      /* VMON Reset */
#define RESET_SRES2F0              (0x00000010UL)      /* Software System Reset */
#define RESET_SRES2F1              (0x00000020UL)      /* SWDT System Reset */
#define RESET_SRES2F2              (0x00000040UL)      /* ECM System Reset */
#define RESET_SRES2F3              (0x00000080UL)      /* WDTBA Reset */
#define RESET_ARESF0               (0x00000100UL)      /* Software Application Reset */
#define RESET_ARESF1               (0x00000200UL)      /* SWDT Application Reset */
#define RESET_ARESF2               (0x00000400UL)      /* ECM Application Reset */
#define RESET_HWBISTF0             (0x00001000UL)      /* HW BIST(Field BIST0) */
#define RESET_HWBISTF1             (0x00002000UL)      /* HW BIST(Field BIST1) */
#define RESET_HWBISTF2             (0x00004000UL)      /* HW BIST(Field BIST2) */
#define RESET_DRESF0               (0x00008000UL)      /* DeepStop Reset */
#define RESET_ICUMSRESF0           (0x00001000UL)      /* ICUM Software System Reset */
#define RESET_ICUMARESF0           (0x00002000UL)      /* ICUM Software Application Reset */

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* None */

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
uint32_t D_RESET_Init(void);
uint32_t D_RESET_SetSysReset2(void);
uint32_t D_RESET_SetAppReset(void);
uint32_t D_RESET_SetModReset(void);
uint32_t D_RESET_SetStopModEnable(void);
uint32_t D_RESET_GetResetFactor(uint32_t *p_status);
uint32_t D_RESET_TransitionStopMode(void);

#endif /* #if !defined(ICUM_D_RESET_PUB_H) */
