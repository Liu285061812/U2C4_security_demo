/******************************************************************************/
/* Component Name   MyData MNG                                                */
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

#if !defined(ICUM_MH_MYDATA_H)
#define ICUM_MH_MYDATA_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
#define MYDATA_SLOT_ITEMS_MIN               (0uL)
#define MYDATA_SLOT_SIZE_MIN                (0uL)
#define MYDATA_BLOCKS_MIN                   (0uL)
#define MYDATA_JOB_ID_MIN                   (0uL)

#define MYDATA_BLOCKS_ROUND_UP              (1uL)

#define MYDATA_DATA_CLR                     (0u)

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
static uint32_t mh_MYDATA_ConvertErr(uint32_t conv_err);
static r_icumif_service_result_t mh_MYDATA_ConvertServ(uint32_t conv_err);

#endif /* #if !defined(ICUM_MH_MYDATA_H) */
