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

#ifndef R_PKCD_CMN_H
/* Multiple inclusion protection macro */
#define R_PKCD_CMN_H

/* PRQA S 0776 ++ # D012 Develop according to C99 */
/* PRQA S 3453,3472 ++ # D026 If it is a function, overhead is large, so it corresponds with macro. */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDCommonDefinesInternal
 * \{******************************************************************/
#define PKCD_CMN_VERSION_MAJOR   (0U)            /**< PKC-Driver Major version */
#define PKCD_CMN_VERSION_MINOR   (1U)            /**< PKC-Driver Library Minor version */

#define PKCD_CMN_ICUM_PKCCB0CR       (*(volatile uint32_t*)0xFFFEE2C0U)   /**< PKCC control register */
#define PKCD_CMN_ICUM_PKCCB0OMR      (*(volatile uint32_t*)0xFFFEE2C4U)   /**< PKCC operation mode register */
#define PKCD_CMN_ICUM_PKCCB0STR      (*(volatile uint32_t*)0xFFFEE2C8U)   /**< PKCC status register */
#define PKCD_CMN_ICUM_PKCCB0STC      (*(volatile uint32_t*)0xFFFEE2CCU)   /**< PKCC status clear register */
#define PKCD_CMN_ICUM_PKCCB0DPT      (*(volatile uint32_t*)0xFFFEE2D0U)   /**< PKCC data pointer T register */
#define PKCD_CMN_ICUM_PKCCB0DPA      (*(volatile uint32_t*)0xFFFEE2D4U)   /**< PKCC data pointer A register */
#define PKCD_CMN_ICUM_PKCCB0DPB      (*(volatile uint32_t*)0xFFFEE2D8U)   /**< PKCC data pointer B register */
#define PKCD_CMN_ICUM_PKCCB0DPC      (*(volatile uint32_t*)0xFFFEE2DCU)   /**< PKCC data pointer C register */
#define PKCD_CMN_ICUM_PKCCB0DPS      (*(volatile uint32_t*)0xFFFEE2E0U)   /**< PKCC data pointer S register */
#define PKCD_CMN_ICUM_PKCCB0DPW      (*(volatile uint32_t*)0xFFFEE2E4U)   /**< PKCC data pointer W register */

#define PKCD_CMN_PKCCRAMSIZE         (4096U)       /**< PKCD_CMN_PKCCRAM size */
#define PKCD_CMN_PKCCRAM             (0xFF1F8000U) /**< PKCD_CMN_PKCCRAM top address */
#define PKCD_CMN_PKCCBOUNDARYSIZE    (0x08U)       /**< PKCD_CMN_PKCCRAM data boundary is 8-byte */

#define PKCD_CMN_PKCC_START          (0x00000001U) /**< When set in the PKCC Control Register, start PKCC */

#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 0x1U)
#define PKCD_CMN_PKCC_BEME_0         (0x00000000U) /**< BEME = 0. */
#define PKCD_CMN_PKCC_BEME           (0x00000200U) /**< BEME = 1. */
#else
#define PKCD_CMN_PKCC_BEME           (0x00000000U) /**< Always BEME = 0. */
#endif

#define PKCD_CMN_PKCC_OK             (0x00000001U) /**< PKCC has run normally */

#define PKCD_CMN_MODE_AEXPBMODC_CON  (0x00U)       /**< Operate S = A^B mod C (constant time) */
#define PKCD_CMN_MODE_ABRINVMODC     (0x02U)       /**< Operate S = ABR^(-1) mod C */
#define PKCD_CMN_MODE_R2MODC         (0x03U)       /**< Operate S = R^2 mod C */
#define PKCD_CMN_MODE_ARINVMODC      (0x04U)       /**< Operate S = AR^(-1) mod C */
#define PKCD_CMN_MODE_APLUSBMODC     (0x05U)       /**< Operate S = A + B mod C */
#define PKCD_CMN_MODE_AMINUSBMODC    (0x06U)       /**< Operate S = A - B mod C */
#define PKCD_CMN_MODE_AB             (0x07U)       /**< Operate S = A x B */
#define PKCD_CMN_MODE_AB64           (0x08U)       /**< Operate S = A x B (A is 64bit) */
#define PKCD_CMN_MODE_APLUSB         (0x09U)       /**< Operate S = A + B */
#define PKCD_CMN_MODE_AMINUSB        (0x0AU)       /**< Operate S = A - B */
#define PKCD_CMN_MODE_ADIV2          (0x0CU)       /**< Operate S = A / 2 */
#define PKCD_CMN_MODE_AEXPBMODC      (0x0FU)       /**< Operate S = A^B mod C */
#define PKCD_CMN_MODE_MINUSCINV      (0x10U)       /**< Operate T = -C^(-1) mod 2^64 */
#define PKCD_CMN_MODE_PNTADD         (0x13U)       /**< Operate (S1,S2,S3) = (A1,A2,A3) + (B1,B2,B3) */
#define PKCD_CMN_MODE_PNTSCALARMUL   (0x15U)       /**< Operate (S1,S2,S3) = B x (A1,A2,A3) */

#define PKCD_CMN_DPMASK              (0x00000FF8U) /**< Used for mask 0 bits in PKCC data pointer register value */

#define PKCD_CMN_OMR_DL_SHIFT        (16U)         /**< Bit position of DL in PKCC Operation Mode Register */
#define PKCD_CMN_OMR_BDL_SHIFT       (24U)         /**< Bit position of BDL in PKCC Operation Mode Register */

#define PKCD_CMN_BITS64              (0x00000008U) /**< A size of 8byte 64-bits area */

#define PKCD_CMN_BITSOFBYTE          (8U)          /**< Number of bits in a byte */

#define PKCD_CMN_PKCD_OPE_TIMEOUT    (75000000U)   /**< Number of cycles for operation timeout */


#define PKCD_CMN_EXTERNAL_API_OK     (0x55555555U) /**< External API OK return value */

#define PKCD_CMN_PROCESSING_BITS_MIN (0x1U)        /**< Minimum of processing bits */
#define PKCD_CMN_PROCESSING_BITS_MAX (0xFFFFU)     /**< Maximum  of processing bits */

#define PKCD_CMN_MUL_2               (2U)          /**< Double value */
#define PKCD_CMN_MUL_3               (3U)          /**< Triple  value */
#define PKCD_CMN_DIV_2               (2U)          /**< Half value */
#define PKCD_CMN_ADD_7               (7U)          /**< Add 7 value */
#define PKCD_CMN_SHIFT_3             (3U)          /**< Shift 3 value */
#define PKCD_CMN_REMAINDER_DIV4      (3U)          /**< Remainder of divided by 4 */

#define PKCD_CMN_WORD_BYTE_SIZE      (4U)          /**< A byte size of WORD */

#define PKCD_CMN_LSB_POS             (7U)          /**< LSByte postion in PKCCRAM Boundary*/

#define PKCD_CMN_UINT8_MAX           (0xFFU)       /**< Maximum value of type uint8_t */

/** One of the values that gives #PKCD_RET_OK when the two are added. Use for double check on verification. */
#define PKCD_CMN_RET_OK_HALF_1       ((pkcd_ret_t)0x21B7ACB0)
/** Another of the values that gives #PKCD_RET_OK when the two are added. Use for double check on verification. */
#define PKCD_CMN_RET_OK_HALF_2       ((pkcd_ret_t)0x33485351)


/*!****************************************************************************************************************//**
 * \brief       Convert pointer to PKCC data pointer register format
 * \param       [in] ptr    A pointer
 * \return
 *  -   A converted ptr value which can be set to PKCC data pointer register
 * \par Global Variables
 *  -   None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      Convert pointer to PKCC data pointer register format.
 *      This macro casts internally.
 *********************************************************************************************************************/
#define PKCD_CMN_FORMAT_DP(ptr)      (((uintptr_t)(ptr)) & PKCD_CMN_DPMASK)


/*!****************************************************************************************************************//**
 * \details     Return 32-bit array size
 * \param       [in] size    Byte size of buffer
 * \return      Array Size
 * \par Global Variables
 *  -   None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      Returns the number of elements of the 32-bit array required for the specified byte size.
 *********************************************************************************************************************/
#define PKCD_CMN_ARRAY32_SIZE(size)  (((size) + 3U) / 4U)
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
extern void r_pkcd_cmn_cpy_by_64bounds (uint8_t * const p_dest, const uint8_t * const p_src, const uint32_t len);
extern void r_pkcd_cmn_cpy_word (uint8_t * const p_dest, const uint8_t * const p_src, const uint32_t len);
extern void r_pkcd_cmn_cpy_byte (uint8_t * const p_dest, const uint8_t * const p_src, const uint32_t len);
extern int32_t r_pkcd_cmn_cmp_word (const uint8_t * const p_a, const uint8_t * const p_b, const uint32_t len);
extern int32_t r_pkcd_cmn_cmp_zero_word (const uint8_t * const p_a, const uint32_t len);
extern pkcd_ret_t r_pkcd_cmn_cmp_work_unit (const uint8_t * const p_wk_a, const uint8_t * const p_wk_b,
                                    uint8_t * const p_wk_1, const uint32_t data_len, const uint32_t wk_unit_size,
                                    int32_t * const p_cmp_ret);
extern void r_pkcd_cmn_ram_clr (uint8_t * const p_dest, const uint32_t len);
extern uint32_t r_pkcd_cmn_ch_endian32 (const uint32_t val);
extern pkcd_ret_t r_pkcd_cmn_run (const uint32_t mode, const e_pkc_sync_opt_t sync_option);
extern pkcd_ret_t r_pkcd_cmn_clr_work_unit (const uint8_t * const p_wk, const uint32_t wk_unit_size);
extern uint8_t r_pkcd_cmn_get_lsb_work_unit (const uint8_t * const p_wk, const uint32_t wk_unit_size);
extern void r_pkcd_cmn_set_async_ope (void);
extern void r_pkcd_cmn_clr_async_ope (void);
extern void r_pkcd_cmn_set_dl (const uint32_t dl, const uint32_t bdl);
extern e_pkc_bool_t r_pkcd_cmn_is_multiple_4 (const uint32_t val);
extern pkcd_ret_t r_pkcd_cmn_ABRInvModC (const uint8_t * const p_wk_A, const uint8_t * const p_wk_B,
                                    const uint8_t * const p_wk_C, const uint8_t * const p_wk_S,
                                    const uint32_t wk_unit_size);
extern pkcd_ret_t r_pkcd_cmn_ARInvModC (const uint8_t * const p_wk_A, const uint8_t * const p_wk_C,
                                const uint8_t * const p_wk_S, const uint32_t wk_unit_size);
extern pkcd_ret_t r_pkcd_cmn_APlusBModC (const uint8_t * const p_wk_A, const uint8_t * const p_wk_B,
                                    const uint8_t * const p_wk_C, const uint8_t * const p_wk_S,
                                    const uint32_t wk_unit_size);
extern pkcd_ret_t r_pkcd_cmn_AMinusBModC (const uint8_t * const p_wk_A, const uint8_t * const p_wk_B,
                                    const uint8_t * const p_wk_C, const uint8_t * const p_wk_S,
                                    const uint32_t wk_unit_size);
extern pkcd_ret_t r_pkcd_cmn_APlusB (const uint8_t * const p_wk_A, const uint8_t * const p_wk_B,
                                const uint8_t * const p_wk_S, const uint32_t wk_unit_size);
extern pkcd_ret_t r_pkcd_cmn_AMinusB (const uint8_t * const p_wk_A, const uint8_t * const p_wk_B,
                                const uint8_t * const p_wk_S, const uint32_t wk_unit_size);
extern pkcd_ret_t r_pkcd_cmn_ADiv2 (const uint8_t * const p_wk_A, const uint8_t * const p_wk_S,
                                    const uint32_t wk_unit_size);
extern pkcd_ret_t r_pkcd_cmn_CPlusAB (const uint8_t * const p_wk_A, const uint8_t * const p_wk_B,
                                const uint8_t * const p_wk_C, const uint8_t * const p_wk_S,
                                const uint32_t wk_unit_size);
extern pkcd_ret_t r_pkcd_cmn_AB (const uint8_t * const p_wk_A, const uint8_t * const p_wk_B,
                            const uint8_t * const p_wk_S, const uint32_t wk_unit_size);
extern pkcd_ret_t r_pkcd_cmn_AB64 (const uint8_t * const p_wk_A, const uint8_t * const p_wk_B,
                            const uint8_t * const p_wk_S, const uint32_t wk_unit_size);
extern pkcd_ret_t r_pkcd_cmn_AExpBModC (const uint8_t * const p_wk_A, const uint8_t * const p_wk_B,
                                const uint8_t * const p_wk_C, const uint8_t * const p_wk_S, const uint32_t wk_unit_size,
                                const e_pkc_sync_opt_t sync_option);
extern pkcd_ret_t r_pkcd_cmn_AExpBModCCon (const uint8_t * const p_wk_A, const uint8_t * const p_wk_B,
                                    const uint8_t * const p_wk_C, const uint8_t * const p_wk_S,
                                    const uint32_t wk_unit_size, const e_pkc_sync_opt_t sync_option);
extern pkcd_ret_t r_pkcd_cmn_MinusCInv (const uint8_t * const p_wk_C, const uint8_t * const p_dpT,
                                    const uint32_t wk_unit_size);
extern pkcd_ret_t r_pkcd_cmn_R2ModC (const uint8_t * const p_wk_C, const uint8_t * const p_wk_S,
                                    const uint32_t wk_unit_size);
extern pkcd_ret_t r_pkcd_cmn_modinv_fermat (const uint8_t * const p_wk_A, const uint8_t * const p_wk_mod,
                                    uint8_t * const p_wk_out, const uint8_t * const p_wk1, const uint32_t wk_size);
extern pkcd_ret_t r_pkcd_cmn_reduce_small_modulus (const uint8_t * const p_a, const uint8_t * const p_wk_mod,
                                            const uint8_t * const p_wk_r2mod, const uint32_t a_size,
                                            const uint32_t mod_size, const uint32_t wk_size, uint8_t * const p_wk1,
                                            uint8_t * const p_wk2, uint8_t * const wk_out);
extern uint32_t r_pkcd_cmn_find_msb (const uint8_t * const p_val, const uint32_t len);
extern pkcd_ret_t r_pkcd_cmn_reduce_even_modulus (const uint8_t * const p_a, const uint8_t * const p_mod,
                                            const uint32_t a_size, const uint32_t mod_size,
                                            const uint32_t wk_unit_size, uint8_t * const p_wk1,
                                            uint8_t * const p_wk2, uint8_t * const p_wk3, uint8_t * const p_wk_out);
extern pkcd_ret_t r_pkcd_cmn_extract_result (const pkcd_ret_t ored_ret);
#endif /* R_PKCD_CMN_H */
