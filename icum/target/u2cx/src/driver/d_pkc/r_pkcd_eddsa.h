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

#ifndef R_PKCD_EDDSA_H
/* Multiple inclusion protection macro */
#define R_PKCD_EDDSA_H

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDRSADefinesInternal
 * \{******************************************************************/
#define PKCD_EDDSA_CURVE_EDWARDS25519_LEN       (32U)      /**< Curve length of edward25519 */
#define PKCD_EDDSA_CURVE_EDWARDS448_LEN         (56U)      /**< Curve length of edward448 */

#if ((PKCD_CFG_USE_ED25519_PRECALC_TABLE_16 == 1U) || (PKCD_CFG_USE_ED448_PRECALC_TABLE_16 == 1U))
/** Precomputed table size used by Scalar Multiplication */
    #define PKCD_EDDSA_TBL_SM_PRECALC_TABLE_16          (16U)
#endif

#if ((PKCD_CFG_USE_ED25519_PRECALC_TABLE_256 == 1U) || (PKCD_CFG_USE_ED448_PRECALC_TABLE_256 == 1U))
/** Precomputed table size used by Scalar Multiplication */
    #define PKCD_EDDSA_TBL_SM_PRECALC_TABLE_256         (256U)
#endif
/*! \}*/

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDEDDSATypesInternal
 * \{******************************************************************/
/** ed25519 pre-calculation table for comb method,
  * which contains results of B x (a0*2^192 + a1*2^128 + a2*2^64 + a3*2^0)
  * The terms a0~a3 is 0 or 1, and there are 16 combinations whith those values.
  * Each coordinate value represented as bid endian array of multiple 64bit little endian data.
  */
typedef struct
{
    uint64_t xr[PKCD_EDDSA_CURVE_EDWARDS25519_LEN/sizeof(uint64_t)]; /**< An X-coordinate value in montgomery form */
    uint64_t yr[PKCD_EDDSA_CURVE_EDWARDS25519_LEN/sizeof(uint64_t)]; /**< A Y-coordinate value in montgomery form */
    uint64_t zr[PKCD_EDDSA_CURVE_EDWARDS25519_LEN/sizeof(uint64_t)]; /**< A Z-coordinate value in montgomery form */
} st_ed25519_base_comb_tbl_t;

typedef struct
{
    uint64_t xr[PKCD_EDDSA_CURVE_EDWARDS448_LEN/sizeof(uint64_t)];   /**< An X-coordinate value in montgomery form */
    uint64_t yr[PKCD_EDDSA_CURVE_EDWARDS448_LEN/sizeof(uint64_t)];   /**< A Y-coordinate value in montgomery form */
    uint64_t zr[PKCD_EDDSA_CURVE_EDWARDS448_LEN/sizeof(uint64_t)];   /**< A Z-coordinate value in montgomery form */
} st_ed448_base_comb_tbl_t;

/*! \}*/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
#if (PKCD_CFG_USE_ED25519_PRECALC_TABLE_16 == 1U)
/** ed25519 pre-calculation table for comb method */
extern const st_ed25519_base_comb_tbl_t g_ed25519_comb_table_16[PKCD_EDDSA_TBL_SM_PRECALC_TABLE_16];
#elif (PKCD_CFG_USE_ED25519_PRECALC_TABLE_256 == 1U)
/** ed25519 pre-calculation table for comb method */
extern const st_ed25519_base_comb_tbl_t g_ed25519_comb_table_256[PKCD_EDDSA_TBL_SM_PRECALC_TABLE_256];
#endif

#if (PKCD_CFG_USE_ED448_PRECALC_TABLE_16 == 1U)
/** ed448 pre-calculation table for comb method */
extern const st_ed448_base_comb_tbl_t g_ed448_comb_table_16[PKCD_EDDSA_TBL_SM_PRECALC_TABLE_16];
#elif (PKCD_CFG_USE_ED448_PRECALC_TABLE_256 == 1U)
/** ed448 pre-calculation table for comb method */
extern const st_ed448_base_comb_tbl_t g_ed448_comb_table_256[PKCD_EDDSA_TBL_SM_PRECALC_TABLE_256];
#endif

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#endif /* R_PKCD_EDDSA_H */
