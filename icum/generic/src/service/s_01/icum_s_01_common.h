/******************************************************************************/
/* Component Name  Service 01                                                 */
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

#ifndef ICUM_S_01_COMMON_H
#define ICUM_S_01_COMMON_H

#include "renesas_types.h"
#include "r_icumif_api.h"
#include "icum_l_cry_pk_pub.h"
#include "icum_mh_key_pub.h"

/* data size of shared secret key */
#define NIST_P384_DATA_SIZE         (48uL)
#define BRAINPOOL_P384_DATA_SIZE    (48uL)

uint32_t s_01_GetDecimalStrlenFromBitCnt(uint16_t bits,uint16_t *figure10Max);
uint32_t s_01_GenerateRandomSequence(uint32_t *udwRandom, uint16_t size_in_bytes, bool emePkcsFlag);
uint32_t s_01_IsDigitString(const uint8_t *target_bytes, uint32_t length, uint8_t *isdigit);
uint32_t s_01_IsHexDigitString(const uint8_t *target_bytes, uint32_t length, uint8_t *isdigit);
uint32_t s_01_CanUseKeyIfDbgConnected(key_flags_t disable_on_debug, uint8_t *is_allowed);
uint32_t s_01_ConvertErr(uint32_t arg, volatile r_icumif_service_result_t *p_service_result);

#endif /* ICUM_S_01_COMMON_H */

