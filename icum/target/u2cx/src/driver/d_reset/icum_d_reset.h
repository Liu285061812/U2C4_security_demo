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

#if !defined(ICUM_D_RESET_H)
#define ICUM_D_RESET_H


/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/

/* ***Internal Definition*** */
/* Value of Setting to Register */
#define RESET_SYSRESET2GEN              (0x00000001UL)      /* Generate System Reset 2 (SWSRESA_ICUM Register) */
#define RESET_APPLIRESETGEN             (0x00000001UL)      /* Generate Application Reset (SWARESA_ICUM Register) */
#define RESET_MODULERESETGEN            (0x00000001UL)      /* Generate Module Reset (SWMRESA_ICUM Register) */
#define RESET_STOPRQCHECK               (0x00000002UL)      /* Check whether STOP is requested  (ICUM_STOP Register) */
#define RESET_STOPRQENABLE              (0x00000001UL)      /* Requests for STOP mode are enabled  (ICUM_STOP Register) */

/* Reset Factor Check Value */
#define RESET_CHK_ICUMMRESF0            (0x00000001UL)      /* WDTA Reset of RESF_ICUM Register */
#define RESET_CHK_ICUMMRESF1            (0x00000002UL)      /* ICUM Software Module Reset of RESF_ICUM Register */

#define RESET_CHK_PRESF0                (0x00000001UL)      /* Power Up Reset of RESF Register */
#define RESET_CHK_SRES1F0               (0x00000002UL)      /* Stand-by Reset of RESF Register */
#define RESET_CHK_SRES1F1               (0x00000004UL)      /* External Reset of RESF Register */
#define RESET_CHK_SRES1F2               (0x00000008UL)      /* VMON Reset of RESF Register */
#define RESET_CHK_SRES2F0               (0x00000010UL)      /* Software System Reset of RESF Register */
#define RESET_CHK_SRES2F1               (0x00000020UL)      /* SWDT System Reset of RESF Register */
#define RESET_CHK_SRES2F2               (0x00000040UL)      /* ECM System Reset of RESF Register */
#define RESET_CHK_ARESF0                (0x00000100UL)      /* Software Application Reset of RESF Register */
#define RESET_CHK_ARESF1                (0x00000200UL)      /* SWDT Application Reset of RESF Register */
#define RESET_CHK_ARESF2                (0x00000400UL)      /* ECM Application Reset of RESF Register */
#define RESET_CHK_ICUMSRESF0            (0x00010000UL)      /* ICUM Software System Reset of RESF Register */
#define RESET_CHK_ICUMARESF0            (0x00020000UL)      /* ICUM Software Application Reset of RESF Register */

#define RESET_NOSTATUS                  (0x00000000UL)      /* Not set correspondent bit */

#define RESET_STBC0DISTRG               (0x00000002UL)      /* Transition to DeepSTOP mode (STBC0PSC  Register) */
#define RESET_STBC0STPTRG               (0x00000001UL)      /* Transition to STOP mode     (STBC0STPT Register) */
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

#endif /* #if !defined(ICUM_D_RESET_H) */
