/******************************************************************************/
/* Component Name   ICU-M PKC Driver for taget device                         */
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

#ifndef R_PKCD_DEV_API_H
/* Multiple inclusion protection macro */
#define R_PKCD_DEV_API_H

/* PRQA S 3630 ++ # D027 For use in upper program. */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/*!****************************************************
 * \addtogroup    PKCDRSATypes  RSA Types
 * \{**************************************************/
/** A RSA key binary format structure */
typedef struct
{
    uint32_t  key_length;   /**< key byte length */
    uint8_t * p_n;          /**< Pointer to a byte array of modulus n */
    uint8_t * p_d;          /**< Pointer to a byte array of private exponent d */
    uint8_t * p_p;          /**< Pointer to a byte array of prime p */
    uint8_t * p_q;          /**< Pointer to a byte array of prime q */
    uint8_t * p_dp;         /**< Pointer to a byte array of private exponent dp */
    uint8_t * p_dq;         /**< Pointer to a byte array of private exponent dq */
    uint8_t * p_qinv;       /**< Pointer to a byte array of coefficient qinv */
    uint32_t  e_length;     /**< e byte length */
    uint8_t * p_e;          /**< Pointer to a byte array of public exponent e  */
} st_pkcd_rsa_key_bin_t;

/*! \}*/

/*!****************************************************
 * \addtogroup    PKCDDHTypes    DH Types
 * \{**************************************************/
/** A DH key binary format structure */
typedef struct
{
    uint32_t  private_key_length;    /**< key byte length of private key*/
    uint8_t * p_private;            /**< Pointer to a byte array of private key */
    uint32_t  public_key_length;    /**< key byte length of public key */
    uint8_t * p_public;             /**< Pointer to a byte array of public key */
} st_pkcd_dh_key_bin_t;

/*! \}*/

/*!****************************************************
 * \addtogroup    PKCDECTypes   Elliptic curve Types
 * \{**************************************************/
/** A EC key binary format structure */
typedef struct
{
    uint32_t  key_length;   /**< key byte length */
    uint8_t * p_d;          /**< Pointer to a byte array of private key d */
    uint8_t * p_qx;         /**< Pointer to a byte array of public key Qx */
    uint8_t * p_qy;         /**< Pointer to a byte array of public key Qx */
} st_pkcd_ec_key_bin_t;

/*! \}*/

/*!****************************************************
 * \addtogroup    PKCDEDDSATypes    EdDSA Types
 * \{**************************************************/
/** A EdDSA key binary format structure */
typedef struct
{
    uint32_t  key_length;   /**< key byte length */
    uint8_t * p_private;    /**< Pointer to a byte array of private key */
    uint8_t * p_public;     /**< Pointer to a byte array of public key */
} st_pkcd_eddsa_key_bin_t;

/*! \}*/

/*!****************************************************
 * \addtogroup    PKCDECDHXTypes    ECDHX Types
 * \{**************************************************/
/** A ECDHX key binary format structure */
typedef struct
{
    uint32_t  key_length;   /**< key byte length */
    uint8_t * p_private;    /**< Pointer to a byte array of private key */
    uint8_t * p_public;     /**< Pointer to a byte array of public key */
} st_pkcd_ecdhx_key_bin_t;

/*! \}*/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#endif /* R_PKCD_DEV_API_H */
