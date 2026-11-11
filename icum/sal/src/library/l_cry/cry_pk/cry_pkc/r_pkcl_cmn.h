/******************************************************************************/
/* Component Name   Cryptographic Library PKC Library                         */
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

#ifndef R_PKCL_CMN_H
/* Multiple inclusion protection macro */
#define R_PKCL_CMN_H

/* PRQA S 0776 ++ # D008 Develop according to C99 */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_pkcd_api.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \addtogroup  PKCLCommonDefines     Common Definitions
 * \{******************************************************************/
#define PKCL_VERSION_MAJOR         (0U)            /**< PKC-Lib Major version */
#define PKCL_VERSION_MINOR         (1U)            /**< PKC-Lib Minor version */

#define PKCL_EXTERNAL_API_OK       (0x55555555U)   /**< External API success value */
#define PKCL_EXTERNAL_API_ERR      (0xAAAAAAAAU)   /**< External API failed value */

#define PKCL_PROCESSING_BITS_MAX   (0xFFFFU)       /**< Maximum value of processing bits */

/** One of the values that gives #PKCL_RET_OK when the two are added. */
#define PKCL_CMN_RET_OK_HALF_1          ((pkcl_ret_t)0x21B3A5D3)
/** Another of the values that gives #PKCL_RET_OK when the two are added. */
#define PKCL_CMN_RET_OK_HALF_2          ((pkcl_ret_t)0x33A1AF82)

/** One of the values that gives #PKCL_RET_PROCESSING when the two are added. */
#define PKCL_CMN_RET_PROC_HALF_1        ((pkcl_ret_t)0x250A83D9)
/** Another of the values that gives #PKCL_RET_PROCESSING when the two are added. */
#define PKCL_CMN_RET_PROC_HALF_2        ((pkcl_ret_t)0x2FF5D128)

/** One of the values that gives #PKCL_RET_PRIME_GENERATING when the two are added. */
#define PKCL_CMN_RET_PRIMEGEN_HALF_1    ((pkcl_ret_t)0x23F94D1B)
/** Another of the values that gives #PKCL_RET_PRIME_GENERATING when the two are added. */
#define PKCL_CMN_RET_PRIMEGEN_HALF_2    ((pkcl_ret_t)0x310707E7)
/*! \}*/

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern pkcl_ret_t r_pkcl_cmn_pkcl_ret_from_pkcd_ret (const pkcd_ret_t drv_ret);
extern void r_pkcl_cmn_set_state (const e_pkcl_state_t state);
extern e_pkcl_state_t r_pkcl_cmn_get_state (void);
extern e_pkc_bool_t r_pkcl_cmn_is_multiple_4 (const uint32_t val);
extern void r_pkcl_cmn_set_byte (uint8_t * const p_dest, const uint8_t val, const uint32_t len);
extern void r_pkcl_cmn_cpy_byte (uint8_t * const p_dest, const uint8_t * const p_src, const uint32_t len);
extern e_pkc_bool_t r_pkcl_cmn_cmp_byte (const uint8_t * const p_data_a, const uint8_t * const p_data_b,
                                            const uint32_t len);
extern uint32_t r_pkcl_cmn_ch_big_endian32 (const uint32_t val);

#endif /* R_PKCL_CMN_H */
