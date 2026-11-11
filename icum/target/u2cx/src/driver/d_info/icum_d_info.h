/******************************************************************************/
/* Component Name   ICUM INFO Driver                                          */
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

#if !defined(ICUM_D_INFO_H)
#define ICUM_D_INFO_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* address used by SIC remap function */
#define INFO_UCID_ADDRESS_FOR_SICREMAP  (0x08000000uL)
#define INFO_SICREMAP_DEFAULT_ADDRESS   (0xFC000000uL)

/* values for multiple call detection flg */
#define INFO_INIT_NOT_CALLED_YET        (0uL)
#define INFO_INIT_ALREADY_CALLED        (1uL)

/* Definition used by INFO driver */
#define INFO_BIT_CALC_GET_00_TO_03      (0x0000000FuL)
#define INFO_BIT_CALC_GET_04_TO_07      (0x000000F0uL)
#define INFO_BIT_CALC_GET_08_TO_11      (0x00000F00uL)
#define INFO_BIT_CALC_GET_12_TO_15      (0x0000F000uL)
#define INFO_BIT_CALC_GET_16_TO_19      (0x000F0000uL)
#define INFO_BIT_CALC_GET_20_TO_23      (0x00F00000uL)
#define INFO_BIT_CALC_GET_24_TO_27      (0x0F000000uL)
#define INFO_BIT_CALC_GET_28_TO_31      (0xF0000000uL)

#define INFO_BIT_CALC_GET_BIT00         (0x00000001uL)
#define INFO_BIT_CALC_GET_BIT01         (0x00000002uL)
#define INFO_BIT_CALC_GET_BIT02         (0x00000004uL)
#define INFO_BIT_CALC_GET_BIT03         (0x00000008uL)
#define INFO_BIT_CALC_GET_BIT04         (0x00000010uL)
#define INFO_BIT_CALC_GET_BIT05         (0x00000020uL)
#define INFO_BIT_CALC_GET_BIT06         (0x00000040uL)
#define INFO_BIT_CALC_GET_BIT07         (0x00000080uL)
#define INFO_BIT_CALC_GET_BIT08         (0x00000100uL)
#define INFO_BIT_CALC_GET_BIT09         (0x00000200uL)
#define INFO_BIT_CALC_GET_BIT10         (0x00000400uL)
#define INFO_BIT_CALC_GET_BIT11         (0x00000800uL)
#define INFO_BIT_CALC_GET_BIT12         (0x00001000uL)
#define INFO_BIT_CALC_GET_BIT13         (0x00002000uL)
#define INFO_BIT_CALC_GET_BIT14         (0x00004000uL)
#define INFO_BIT_CALC_GET_BIT15         (0x00008000uL)
#define INFO_BIT_CALC_GET_BIT16         (0x00010000uL)
#define INFO_BIT_CALC_GET_BIT17         (0x00020000uL)
#define INFO_BIT_CALC_GET_BIT18         (0x00040000uL)
#define INFO_BIT_CALC_GET_BIT19         (0x00080000uL)
#define INFO_BIT_CALC_GET_BIT20         (0x00100000uL)
#define INFO_BIT_CALC_GET_BIT21         (0x00200000uL)
#define INFO_BIT_CALC_GET_BIT22         (0x00400000uL)
#define INFO_BIT_CALC_GET_BIT23         (0x00800000uL)
#define INFO_BIT_CALC_GET_BIT24         (0x01000000uL)
#define INFO_BIT_CALC_GET_BIT25         (0x02000000uL)
#define INFO_BIT_CALC_GET_BIT26         (0x04000000uL)
#define INFO_BIT_CALC_GET_BIT27         (0x08000000uL)
#define INFO_BIT_CALC_GET_BIT28         (0x10000000uL)
#define INFO_BIT_CALC_GET_BIT29         (0x20000000uL)
#define INFO_BIT_CALC_GET_BIT30         (0x40000000uL)
#define INFO_BIT_CALC_GET_BIT31         (0x80000000uL)

#define INFO_BIT_CALC_GET_00_TO_02      (0x00000007uL)
#define INFO_BIT_CALC_GET_08_TO_09      (0x00000300uL)
#define INFO_BIT_CALC_GET_10_TO_11      (0x00000C00uL)

#define INFO_BIT_SHIFT_03                (3u)
#define INFO_BIT_SHIFT_04                (4u)
#define INFO_BIT_SHIFT_05                (5u)
#define INFO_BIT_SHIFT_08                (8u)
#define INFO_BIT_SHIFT_10               (10u)
#define INFO_BIT_SHIFT_12               (12u)
#define INFO_BIT_SHIFT_15               (15u)
#define INFO_BIT_SHIFT_17               (17u)
#define INFO_BIT_SHIFT_20               (20u)
#define INFO_BIT_SHIFT_24               (24u)
#define INFO_BIT_SHIFT_25               (25u)
#define INFO_BIT_SHIFT_28               (28u)

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
typedef struct
{
    uint32_t ucid_reg0;
    uint32_t ucid_reg1;
    uint32_t ucid_reg2;
    uint32_t ucid_reg3;
} ucid_reg_t;

#define REG_UCID_BASE (0xFC031000uL)
#define REG_UCID (*(volatile ucid_reg_t *)REG_UCID_BASE)

/***************************************************************************************/
/* Local macro                                                                         */
/***************************************************************************************/

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/* None */

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
/* None */

#endif /* #if !defined(ICUM_D_INFO_H) */
