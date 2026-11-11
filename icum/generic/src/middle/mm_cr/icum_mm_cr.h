/******************************************************************************/
/* Component Name   ML CR Manager header file                                 */
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

#if !defined(ICUM_MM_CR_H)
#define ICUM_MM_CR_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Interrupt and job registration status */
#define CR_NONE                (0u) /* Not action */
#define CR_JOB_REGISTERED      (1u) /* Job registration for C&R service */
#define CR_INT_CHALLENGE_PE    (2u) /* PE Challenge interrupt acceptance state */
#define CR_INT_CHALLENGE_ICUP  (3u) /* ICUP Challenge interrupt acceptance state */
#define CR_INT_RESPONSE_PE     (4u) /* PE Response interrupt acceptance state */
#define CR_INT_RESPONSE_ICUP   (5u) /* ICUP Response interrupt acceptance state */

/* Save PE / ICUP request from either */
#define CR_INT_PE              (0u) /* Request from PE */
#define CR_INT_ICUP            (1u) /* Request from ICUP */
#define CR_INT_INIT            (2u) /* init Value */

/* Save Challenge / Response  request from either */
#define CR_INT_CHALLENGE       (0u) /* challenge request*/
#define CR_INT_RESPONSE        (1u) /* response request */

/* Debugger connect state */
#define CR_DBG_NOT_CONNECT     (0u) /* Debugger not connect */
#define CR_DBG_CONNECT         (1u) /* Debugger connect */

#endif /* #if !defined(ICUM_ML_CTX_H) */
