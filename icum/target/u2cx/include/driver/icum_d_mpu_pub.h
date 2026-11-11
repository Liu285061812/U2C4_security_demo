/******************************************************************************/
/* Component Name   ICUM MPU Driver                                           */
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

#if !defined(ICUM_D_MPU_PUB_H)
#define ICUM_D_MPU_PUB_H

/**************************************************************************************/
/* Return Value Definition                                                            */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Reset Factor */
#define MPU_MIP_INTERRUPT          (0x00000000UL)      /* No Factor */
#define MPU_MDP_INTERRUPT          (0x00000001UL)      /* WDTA Reset */

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* Call Back Routine Definition */
typedef uint32_t    icum_mpu_errfactor_t;

typedef uint32_t    (icum_mpu_interrupt_type_t)(icum_mpu_errfactor_t mpu_fact);
typedef struct icum_mpu_cb_regist
{
    icum_mpu_interrupt_type_t      *p_cbf_addr;       /* Call Back Function Address */
} icum_mpu_cb_regist_t;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
uint32_t D_MPU_Init(icum_mpu_cb_regist_t *p_regist_info);
uint32_t D_MPU_SetCramGuard(void);

#endif /* #if !defined(ICUM_D_MPU_PUB_H) */
