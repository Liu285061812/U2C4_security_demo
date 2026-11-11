/******************************************************************************/
/* Component Name   ICU-M HASH Driver                                         */
/******************************************************************************/
/* Product      : ICU-M Low Level Driver                                      */
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

/* PRQA S 0776 ++ # D005 Exception according to function nomenclature */
/* PRQA S 5002 ++ # D013 Exception according to file nomenclature */

#ifndef R_ICUM_HASH_HMAC_H
/* Multiple inclusion protection macro */
#define R_ICUM_HASH_HMAC_H

#include "r_icum_hash_api.h"

/*=====================================================================================================================
 Public global functions
=====================================================================================================================*/
/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0012]:[ICUMFW_UD_S4_D_HASH_0012]
* Function Name : r_hash_hmac_chk_begin_arg
**********************************************************************************************************************/
extern e_r_hash_ret_t r_hash_hmac_chk_begin_arg(const st_r_hmac_ctx_t* const p_ctx, const e_r_hash_type_t type,
                                                const st_r_hmac_opt_t* const p_opt,
                                                const uint8_t* const p_key, const uint32_t key_len);

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0013]:[ICUMFW_UD_S4_D_HASH_0013]
* Function Name : r_hash_hmac_begin
**********************************************************************************************************************/
extern e_r_hash_ret_t r_hash_hmac_begin(st_r_hmac_ctx_t* const p_ctx, const e_r_hash_type_t type,
                                        const uint8_t* const p_key, const uint32_t key_len);

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0014]:[ICUMFW_UD_S4_D_HASH_0014]
* Function Name : r_hash_hmac_chk_update_arg
**********************************************************************************************************************/
extern e_r_hash_ret_t r_hash_hmac_chk_update_arg(const st_r_hmac_ctx_t* const p_ctx,
                                                    const uint8_t* const p_msg, const uint32_t msg_len);

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0015]:[ICUMFW_UD_S4_D_HASH_0015]
* Function Name : r_hash_hmac_chk_end_arg
**********************************************************************************************************************/
extern e_r_hash_ret_t r_hash_hmac_chk_end_arg(const st_r_hmac_ctx_t* const p_ctx, const uint8_t* const p_tag);

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0016]:[ICUMFW_UD_S4_D_HASH_0016]
* Function Name : r_hash_hmac_end
**********************************************************************************************************************/
extern e_r_hash_ret_t r_hash_hmac_end(st_r_hmac_ctx_t* const p_ctx, uint8_t* const p_tag);

#endif /* R_ICUM_HASH_HMAC_H */
/*=====================================================================================================================
 End of File
=====================================================================================================================*/
