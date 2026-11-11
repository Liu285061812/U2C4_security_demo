/******************************************************************************/
/* Component Name   ICU-M PKC Driver                                         */
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

#ifndef R_PKCD_CFG_H
/* Multiple inclusion protection macro */
#define R_PKCD_CFG_H

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!****************************************************
 * \addtogroup  PKCDCommonFlags   Common Flags
 * \{**************************************************/
/** Use Ed25519 precalc table 16. <br>
 * (Select PKCD_CFG_USE_ED25519_PRECALC_TABLE_16 or PKCD_CFG_USE_ED25519_PRECALC_TABLE_256) */
#define PKCD_CFG_USE_ED25519_PRECALC_TABLE_16      (1U)
/** Use Ed25519 precalc table 256.<br>
 * (Select PKCD_CFG_USE_ED25519_PRECALC_TABLE_16 or PKCD_CFG_USE_ED25519_PRECALC_TABLE_256) */
#define PKCD_CFG_USE_ED25519_PRECALC_TABLE_256     (0U)

/** Use Ed448 precalc table 16. <br>
 * (Select PKCD_CFG_USE_ED448_PRECALC_TABLE_16 or PKCD_CFG_USE_ED448_PRECALC_TABLE_256) */
#define PKCD_CFG_USE_ED448_PRECALC_TABLE_16      (1U)
/** Use Ed448 precalc table 256.<br>
 * (Select PKCD_CFG_USE_ED448_PRECALC_TABLE_16 or PKCD_CFG_USE_ED448_PRECALC_TABLE_256) */
#define PKCD_CFG_USE_ED448_PRECALC_TABLE_256     (0U)

/** For ECC, extend DL by 64 bits, and for other algorithms, set BEME to 1 */
#define PKCD_CFG_OVF_WORKAROUND_ENABLE         (1U)

/** Configuration check flag */
#define PKCD_CFG_PARAM_CHECKING_ENABLE         (1U)
#if (PKCD_CFG_PARAM_CHECKING_ENABLE == 1U)
    /* Check Ed25519 precalc Flags */
    #if (PKCD_CFG_USE_ED25519_PRECALC_TABLE_16 != 1U) && (PKCD_CFG_USE_ED25519_PRECALC_TABLE_256 != 1U)
        #error "Not selected Ed25519 precalc table\
                (shutd be select ED25519_PRECALC_TABLE_16 or ED25519_PRECALC_TABLE_256)"
    #endif
    #if (PKCD_CFG_USE_ED25519_PRECALC_TABLE_16 == 1U) && (PKCD_CFG_USE_ED25519_PRECALC_TABLE_256 == 1U)
        #error "Select either ED25519_PRECALC_TABLE_16 or ED25519_PRECALC_TABLE_256"
    #endif
    /* Check Ed448 precalc Flags */
    #if (PKCD_CFG_USE_ED448_PRECALC_TABLE_16 != 1U) && (PKCD_CFG_USE_ED448_PRECALC_TABLE_256 != 1U)
        #error "Not selected Ed448 precalc table\
                (shutd be select ED448_PRECALC_TABLE_16 or ED448_PRECALC_TABLE_256)"
    #endif
    #if (PKCD_CFG_USE_ED448_PRECALC_TABLE_16 == 1U) && (PKCD_CFG_USE_ED448_PRECALC_TABLE_256 == 1U)
        #error "Select either ED448_PRECALC_TABLE_16 or ED448_PRECALC_TABLE_256"
    #endif
    /* Check if the config is defined */
    #ifndef PKCD_CFG_OVF_WORKAROUND_ENABLE
        #error "PKCD_CFG_OVF_WORKAROUND_ENABLE config not defined."
    #endif
#endif
/*! \}*/

#endif /* R_PKCD_CFG_H */
