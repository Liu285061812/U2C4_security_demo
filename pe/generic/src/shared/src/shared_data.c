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

#include "shared.h"
#include "r_icumif_api.h"
#include "r_icumif_pub.h"
#include "icum_d_comm_pe_pub.h"

uint32_t (*cnt_result_pe2)[];
unsigned long long cnt_trigger_service_request_pe2;
unsigned long long cnt_recieve_service_response_pe2;
unsigned long long cnt_recieve_service_response_pe2;
unsigned long long cnt_request_queue_full_pe2;
unsigned long long cnt_other_request_error_pe2;
unsigned long long cnt_if_lib_not_init_pe2;
unsigned long long cnt_software_error_pe2;

uint32_t (*cnt_result_pe3)[];
unsigned long long cnt_trigger_service_request_pe3;
unsigned long long cnt_recieve_service_response_pe3;
unsigned long long cnt_recieve_service_response_pe3;
unsigned long long cnt_request_queue_full_pe3;
unsigned long long cnt_other_request_error_pe3;
unsigned long long cnt_if_lib_not_init_pe3;
unsigned long long cnt_software_error_pe3;

uint32_t (*cnt_result_pe4)[];
unsigned long long cnt_trigger_service_request_pe4;
unsigned long long cnt_recieve_service_response_pe4;
unsigned long long cnt_recieve_service_response_pe4;
unsigned long long cnt_request_queue_full_pe4;
unsigned long long cnt_other_request_error_pe4;
unsigned long long cnt_if_lib_not_init_pe4;
unsigned long long cnt_software_error_pe4;

uint32_t (*cnt_result_pe5)[];
unsigned long long cnt_trigger_service_request_pe5;
unsigned long long cnt_recieve_service_response_pe5;
unsigned long long cnt_recieve_service_response_pe5;
unsigned long long cnt_request_queue_full_pe5;
unsigned long long cnt_other_request_error_pe5;
unsigned long long cnt_if_lib_not_init_pe5;
unsigned long long cnt_software_error_pe5;

uint32_t (*cnt_result_pe6)[];
unsigned long long cnt_trigger_service_request_pe6;
unsigned long long cnt_recieve_service_response_pe6;
unsigned long long cnt_recieve_service_response_pe6;
unsigned long long cnt_request_queue_full_pe6;
unsigned long long cnt_other_request_error_pe6;
unsigned long long cnt_if_lib_not_init_pe6;
unsigned long long cnt_software_error_pe6;
