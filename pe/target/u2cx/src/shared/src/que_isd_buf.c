/******************************************************************************/
/* Component Name   Testing the ICU-M                                         */
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

#define _IN_SHARED_

#include "que_isd_buf.h"
#include "r_icumif_api.h"
#include "r_icumif_pub.h"
#include "icum_d_comm_pe_pub.h"

#pragma ghs section bss=".que_isd_buf.bss"

// ********************************************************************************
// Below shared variables are moved in GRAM via the linker directive file
// ********************************************************************************

// ********************************************************************************
// ISD request & response queue
// ********************************************************************************
uint32_t ICUM_QUEUE[TOTAL_QUEUE_LENGTH];

// ********************************************************************************
// Big buffers for tests
// ********************************************************************************
uint8_t BIG_BUFFER[3][SIZE_OF_BIG_BUFFER]  __attribute__((aligned(4)));

// ********************************************************************************
// Communication buffer between host and ICU-M
// ********************************************************************************
uint8_t ISD_BUFFER[SIZE_OF_ISD_BUFFER]    __attribute__((aligned(4)));

// ********************************************************************************
// Output buffer
// ********************************************************************************
uint8_t COM_BUFFER[SIZE_OF_COM_BUFFER]    __attribute__((aligned(4)));

// ********************************************************************************
// Output buffer
// ********************************************************************************
uint8_t strbin[20096]       __attribute__((aligned(4)));
