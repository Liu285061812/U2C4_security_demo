/******************************************************************************/
/* Component Name   ICUM ECC Driver                                           */
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

#if !defined(ICUM_D_ECC_PUB_H)
#define ICUM_D_ECC_PUB_H

/**************************************************************************************/
/* Return Value Definition                                                            */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Bit pattern indicating type of ECC test control register */
#define ICUM_ECC_TEST_TYPE_COLLECTIVE   (0x00000001UL)  /* Collective designation of ECC test control register */

/* Protection status of ECC test control register */
#define ICUM_ECC_TEST_PROTECT_OFF       (0x00U)         /* Protect OFF */
#define ICUM_ECC_TEST_PROTECT_ON        (0x01U)         /* Protect ON  */

/**************************************************************************************/
/* Typedef                                                                            */
/**************************************************************************************/
typedef uint32_t    icum_ecc_test_ctrl_reg_type_t;      /* Bit pattern indicating type of ECC test control register */
typedef uint8_t     icum_ecc_test_protect_status_t;     /* Protection status of ECC test control register           */
typedef uint32_t    icum_ecc_int_from_channel_t;        /* Channel number of received interrupt */
typedef uint32_t    icum_ecc_int_error_fact;            /* Detail Error Factor */

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* Registration information of function to call back when receiving an interrupt */
/* 1st Element : Channel number of received interrupt            */
/* 2nd Element : Detail Error Factor                             */
typedef void (p_eccfunc_addr) (icum_ecc_int_from_channel_t channel, icum_ecc_int_error_fact ecc_fact);
typedef struct ICUM_ECC_CB_REGIST
{
    p_eccfunc_addr            *p_cbfunc;        /* Function to call back when an interrupt specified by channel is received */
}ICUM_ECC_CB_REGIST_t;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
uint32_t D_ECC_Init(ICUM_ECC_CB_REGIST_t *p_regist_info);
uint32_t D_ECC_SetECCTestProtection(icum_ecc_test_ctrl_reg_type_t type, icum_ecc_test_protect_status_t status);
uint32_t D_ECC_GetECCTestProtection(icum_ecc_test_ctrl_reg_type_t type, icum_ecc_test_protect_status_t *p_status);

#endif /* #if !defined(ICUM_D_ECC_PUB_H) */
