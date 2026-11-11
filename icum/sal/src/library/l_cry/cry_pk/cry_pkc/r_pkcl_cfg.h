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

#ifndef R_PKCL_CFG_H
/* Multiple inclusion protection macro */
#define R_PKCL_CFG_H

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!****************************************************
 * \addtogroup  PKCLCommonFlags   Common Flags
 * \{**************************************************/
#define PKCL_CFG_ENABLE_DRV_RSA_ENC         (0U)     /**< RSA encryption configuration */
#define PKCL_CFG_ENABLE_DRV_RSA_DEC         (0U)     /**< RSA decryption configuration */
#define PKCL_CFG_ENABLE_DRV_RSA_SIGN        (0U)     /**< RSA signature generation configuration */
#define PKCL_CFG_ENABLE_DRV_RSA_VERIFY      (0U)     /**< RSA signature verification configuration */
#define PKCL_CFG_ENABLE_DRV_ECDSA_SIGN      (0U)     /**< ECDSA signature generation configuration */
#define PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY    (0U)     /**< ECDSA signature verification configuration */
#define PKCL_CFG_ENABLE_DRV_ECDH            (0U)     /**< ECDH/ECDHE key exchange configuration */
#define PKCL_CFG_ENABLE_DRV_EDDSA_SIGN      (0U)     /**< EdDSA signature generation configuration */
#define PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY    (0U)     /**< EdDSA signature verification configuration */
#define PKCL_CFG_ENABLE_DRV_EDDSA_KEYGEN    (0U)     /**< EdDSA key pair generation configuration */
#define PKCL_CFG_ENABLE_DRV_ECDHX           (0U)     /**< X25519/X448 key sharing configuration */
#define PKCL_CFG_ENABLE_DRV_ECDHX_KEYGEN    (0U)     /**< X25519/X448 key pair generation configuration */

/**< Configuration check flag */
#define PKCL_CFG_PARAM_CHECKING_ENABLE         (1U)
#if (PKCL_CFG_PARAM_CHECKING_ENABLE == 1U)
    /* Check if the config is defined */
    #ifndef PKCL_CFG_ENABLE_DRV_RSA_ENC
        #error "PKCL_CFG_ENABLE_DRV_RSA_ENC config not defined."
    #endif
    #ifndef PKCL_CFG_ENABLE_DRV_RSA_DEC
        #error "PKCL_CFG_ENABLE_DRV_RSA_DEC config not defined."
    #endif
    #ifndef PKCL_CFG_ENABLE_DRV_RSA_SIGN
        #error "PKCL_CFG_ENABLE_DRV_RSA_SIGN config not defined."
    #endif
    #ifndef PKCL_CFG_ENABLE_DRV_RSA_VERIFY
        #error "PKCL_CFG_ENABLE_DRV_RSA_VERIFY config not defined."
    #endif
    #ifndef PKCL_CFG_ENABLE_DRV_ECDSA_SIGN
        #error "PKCL_CFG_ENABLE_DRV_ECDSA_SIGN config not defined."
    #endif
    #ifndef PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY
        #error "PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY config not defined."
    #endif
    #ifndef PKCL_CFG_ENABLE_DRV_ECDH
        #error "PKCL_CFG_ENABLE_DRV_ECDH config not defined."
    #endif
    #ifndef PKCL_CFG_ENABLE_DRV_EDDSA_SIGN
        #error "PKCL_CFG_ENABLE_DRV_EDDSA_SIGN config not defined."
    #endif
    #ifndef PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY
        #error "PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY config not defined."
    #endif
    #ifndef PKCL_CFG_ENABLE_DRV_EDDSA_KEYGEN
        #error "PKCL_CFG_ENABLE_DRV_EDDSA_KEYGEN config not defined."
    #endif
    #ifndef PKCL_CFG_ENABLE_DRV_ECDHX
        #error "PKCL_CFG_ENABLE_DRV_ECDHX config not defined."
    #endif
    #ifndef PKCL_CFG_ENABLE_DRV_ECDHX_KEYGEN
        #error "PKCL_CFG_ENABLE_DRV_ECDHX_KEYGEN config not defined."
    #endif
#endif /* PKCL_CFG_PARAM_CHECKING_ENABLE == 1U */

/*! \}*/

#endif /* R_PKCL_CFG_H */
