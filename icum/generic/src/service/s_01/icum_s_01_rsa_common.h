/******************************************************************************/
/* Component Name   ICU-M Service                                             */
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

#ifndef ICUM_S_01_RSA_COMMON_H
#define ICUM_S_01_RSA_COMMON_H

/**********************************************************************************
* Include
***********************************************************************************/

/**********************************************************************************
* Define
***********************************************************************************/
/* process_phase */
#define ON_HOLD         (0u)
#define PHASE_H1        (110u)
#define PHASE_H2        (120u)
#define PHASE_H2DIV     (121u)
#define PHASE_H3        (130u)
#define PHASE_1         (210u)
#define PHASE_2         (220u)
#define PHASE_2DIV      (221u)
#define PHASE_3         (230u)
#define PHASE_3DIV      (231u)
#define PHASE_4         (240u)
#define PHASE_4DIV      (241u)

/* resource control */
#define GET_CTX_NONE    (0x00u)
#define GET_CTX_RSA     (0x01u)
#define GET_CTX_HASH    (0x02u)
#define GET_KEY_RSA     (0x04u)

/**********************************************************************************
* Structure
***********************************************************************************/

/**********************************************************************************
* Variable
***********************************************************************************/

/*********************************************************************************
 * Functions
 *********************************************************************************/
uint32_t s_01_RsaKeyConvertErr(uint32_t *p_err);

#endif  /* ICUM_S_01_RSA_COMMON_H */

