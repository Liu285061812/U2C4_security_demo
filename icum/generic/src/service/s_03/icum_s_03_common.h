/******************************************************************************/
/* Component Name  Service 03                                                 */
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

#ifndef ICUM_S_03_COMMON_H
#define ICUM_S_03_COMMON_H

#include "renesas_types.h"
#include "r_icumif_api.h"
#include "icum_l_cry_pk_pub.h"
#include "icum_mh_key_pub.h"

/**************************************************************************************/
/* Exported global variables (extern).                                                */
/**************************************************************************************/
#if defined(FSW_USE_CRAM)
extern pkc_data_t g_work_bignum[];
#endif

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
#define S03_KEY_SIZE_NOT_EXP MH_KEY_SIZE_NOT_EXP
#define S03_KEY_SIZE_EXP MH_KEY_SIZE_EXP

/* data size of shared secret key */
#define NIST_P384_DATA_SIZE         (48uL)
#define BRAINPOOL_P384_DATA_SIZE    (48uL)

uint32_t s_03_GetInternalHashType(r_hash_primitive_t hash_primitive, hash_type_t *p_hash_type, uint16_t *p_hash_size_in_bytes);
uint32_t s_03_GetBitCntFromDecimalStr(const char *DecimalString, uint16_t *bitsMin);
uint32_t s_03_CanUseKeyIfDbgConnected(key_flags_t disable_on_debug, uint8_t *is_allowed);
uint32_t s_03_CheckMemory(uint8_t ex_service, uint8_t* p_input, uint32_t size, uint8_t direction);
uint32_t s_03_ConvertErr(uint32_t arg, volatile r_icumif_service_result_t *p_service_result);

#endif /* ICUM_S_03_COMMON_H */

