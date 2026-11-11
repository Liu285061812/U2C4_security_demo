/******************************************************************************/
/* Component Name   C&R driver header file                                    */
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

#ifndef ICUM_D_CR_PUB_H_
#define ICUM_D_CR_PUB_H_

/**********************************************************************************
* Define
***********************************************************************************/
#define D_CR_EC_SUCCESS         ((uint32_t)ER_OK)
#define D_CR_EC_INVALID_PARAM   ((uint32_t)ER_PARAM)

#define D_CR_MAIN   ((uint32_t)0x00000000) /* for Main CPU */
#define D_CR_ICUM   ((uint32_t)0x00000001) /* for ICUM */

/**********************************************************************************
* Function prototype
***********************************************************************************/
uint32_t D_CR_Init(void);
uint32_t D_CR_GetDCS(uint32_t *p_cr_Status);
uint32_t D_CR_SetChallenge(uint32_t AuthenticateFor, const uint32_t* p_cr_ChallengeData);
uint32_t D_CR_GetResponse(uint32_t *p_cr_ResponseData);
uint32_t D_CR_OpenDebugger(uint32_t AccessFor);

#endif /* ICUM_D_CR_PUB_H_ */
