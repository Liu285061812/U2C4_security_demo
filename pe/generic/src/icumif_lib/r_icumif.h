/******************************************************************************/
/* Component Name   ICU-M Interface Library (ICUMIF)                          */
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

#if !defined(R_ICUMIF_H)
#define R_ICUMIF_H

/**************************************************************************************/
/* Defines                                                                            */
/**************************************************************************************/

/*====================================================================================*/
/* Data pattern of alignment check                                                    */
/*====================================================================================*/
#define ALIGNMENT_4BYTE_CHECK_PATTERN       (0x00000003UL)      /* 4 byte alignment   */
#define ALIGNMENT_2BYTE_CHECK_PATTERN       (0x00000001UL)      /* 2 byte alignment   */

/**************************************************************************************/
/* Macros                                                                             */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Type definition                                                                    */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/

/*====================================================================================*/
/* Union for alignment check of pointer value                                         */
/*====================================================================================*/
typedef union alignment_check
{
    void        (*p_align_check)(void);
    uint32_t    val_align_check;
} alignment_check_t;

#endif /* #if !defined(R_ICUMIF_H) */

