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

#if !defined(ICUM_K_ML_ABTR_H)
#define ICUM_K_ML_ABTR_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
#define ABTR_RESOURCE_TBL_NUM       (1UL)        /* Number of ABTR HW resource management tables */
#define ABTR_RESOURCE_TBL_AES       (0UL)        /* Table index of AES Resource */
/* Resource status */
#define ABTR_RESOURCE_STS_RELEASED  (0UL)        /* Resource free status */
#define ABTR_RESOURCE_STS_ACQUIRED  (1UL)        /* Resource get status */
#define ABTR_RESOURCE_STS_TIMEOVER  (2UL)        /* Resource released timeover */

#define ABTR_INITIALIZED_NOT_INIT   (0UL)        /* ABTR not initialized */
#define ABTR_INITIALIZED_FLAG       (1UL)        /* ABTR initialized */

#define ABTR_RELEASE_OVER_TIME      (1000UL)     /* Over RunTime */

struct ABTR_RESOURCE_MNG_TBL_t
{
    uint32_t release_state;         /* Resource state : get / free / timeover */
    uint32_t resource_id;           /* Resource ID   : Sequential Number */
    uint8_t timer_id;               /* TimerDRV return timer id */
    void(*p_abtr_func)(void);       /* ABTR callback function */
};

#endif /* #if !defined(ICUM_K_ML_ABTR_H) */
