/******************************************************************************/
/* Component Name   ABTR (HW Resource MNG)                                    */
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

#if !defined(ICUM_ML_ABTR_API_PUB_H)
#define ICUM_ML_ABTR_API_PUB_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Return Value Definition                                                            */
#define ER_ML_ABTR_EMPTY                (ER_ML_ABTR_BASE + (1UL))  /* No empty resources */
#define ER_ML_ABTR_NOT_RELEASED         (ER_ML_ABTR_BASE + (2UL))  /* Resource not released */
#define ER_ML_ABTR_INVALID_ID           (ER_ML_ABTR_BASE + (3UL))  /* Invalid resource ID */
#define ER_ML_ABTR_NOT_INITIALIZED      (ER_ML_ABTR_BASE + (4UL))  /* Not initialized */
#define ER_ML_ABTR_TIMER_SET_FAILURE    (ER_ML_ABTR_BASE + (5UL))  /* TimerDRV failure */
#define ER_ML_ABTR_SYS_ERROR            (ER_ML_ABTR_BASE + (6UL))  /* System error */
#define ER_ML_ABTR_OTHER                (ER_ML_ABTR_BASE + (99UL)) /* Other error */

/* Resource Type */
#define ABTR_RESOURCE_TYPE_AES          (1UL)                    /* AES */

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
uint32_t ML_ABTR_Init(void);
uint32_t ML_ABTR_GetResource(uint32_t resource_type, uint32_t *p_resource_id);
uint32_t ML_ABTR_FreeResource(uint32_t resource_type, uint32_t resource_id);

#endif /* #if !defined(ICUM_ML_ABTR_API_PUB_H) */
