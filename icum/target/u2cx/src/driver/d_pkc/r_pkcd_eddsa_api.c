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

/* PRQA S 0303 ++ # D001 Cast for accessing the area of register address */
/* PRQA S 0306 ++ # D002 Cast for accessing the area of PKCCRAM address */
/* PRQA S 0306 ++ # D003 Cast to use address as an integer value */
/* PRQA S 1503 ++ # D013 No problem if it is unused because it is an API called from outside the library */
/* PRQA S 0310,0316,3305 ++ # D005 In order not to disclose the structure used internally, after receiving
                        it with type uint8_t* or void*, cast it to the type of the structure. */
/* PRQA S 0488 ++ # D007 Consider not to overrun the size of the array pointed to by the pointer.*/
/* PRQA S 0750 ++ # D008 Use union to efficiently use RAM area. */
/* PRQA S 0751 ++ # D009 For commonality of functions, confirm that the size is correct and use the cast value. */
/* PRQA S 0751 ++ # D010 Cast required to calculated the value to set in the register. */
/* PRQA S 2052,2053 ++ # D016 No problem because it is a comment explaining the process. */
/* PRQA S 3383,3384 ++ # D018 Have confirmed in advance that the value doesn't wrap around. */
/* PRQA S 2981 ++ # D020 Local variable ret of APIs sets INTERNALERR to the initial value
                        as a countermeasure against attacks that pass through processing. */
/* PRQA S 2984,2985 ++ # D021 Because it is calculated in consideration of future expansion */
/* PRQA S 3206 ++ # D024 Because it may accommodate multiple curve lengths in the future. */
/* PRQA S 3453,3469,3472 ++ # D026 If it is a function, overhead is large, so it corresponds with macro. */
/* PRQA S 3673 ++ # D029 Because future extensions may edit values in buffer. */
/* PRQA S 5002 ++ # D030 There is no problem because it is used in a system that can distinguish
                    file names of 8 characters or more. */
/* PRQA S 2982 ++ # D032 Clear the local variable ret as a countermeasure against result tampering attacks. */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "renesas_types.h"
#include "r_pkc_api.h"
#include "r_pkcd_cfg.h"
#include "r_pkcd_api.h"
#include "r_pkcd_dev_api.h"
#include "r_pkcd_cmn.h"
#include "r_pkcd_eddsa.h"

/*!********************************************************************
 * \addtogroup    PKCDEDDSA
 * \{
 * \addtogroup    PKCDEDDSATypes                EdDSA Types
 * \addtogroup    PKCDEDDSAConstants            EdDSA Constants
 * \addtogroup    PKCDEDDSAAPIs                 EdDSA APIs
 * \addtogroup    PKCDEDDSADefinesInternal      EdDSA Internal Definitions
 * \addtogroup    PKCDEDDSATypesInternal        EdDSA Internal Types
 * \addtogroup    PKCDEDDSAConstantsInternal    EdDSA Internal Constants
 * \addtogroup    PKCDEDDSAVariablesInternal    EdDSA Internal Variables
 * \addtogroup    PKCDEDDSAAPIsInternal         EdDSA Internal APIs
 * \}******************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDEDDSADefinesInternal
 * \{******************************************************************/
#define PKCD_EDDSA_WORKUNITPDT   (0x00000008U)   /**< A size of 8byte PDT area */
#define PKCD_EDDSA_WORKUNIT      (0x00000048U)   /**< A size of 72byte work area */
#define PKCD_EDDSA_WORKUNITPNT   (PKCD_EDDSA_WORKUNIT*3U)    /**< A size of 216byte work area */
#define PKCD_EDDSA_WORKUNITPDW   (PKCD_EDDSA_WORKUNIT*3U)    /**< A size of 216byte work area */
#define PKCD_EDDSA_SHA512BYTES   (0x00000040U)   /**< SHA512 byte size */
#define PKCD_EDDSA_SHAKE256BYTES (0x00000072U)   /**< SHAKE256 byte size used by Ed448 */

#define PKCD_EDDSA_PDT_P        ((uint8_t*)sp_eddsa_pkccram->pdt_p)  /**< A work area for T mod p */
#define PKCD_EDDSA_PDT_L        ((uint8_t*)sp_eddsa_pkccram->pdt_l)  /**< A work area for T mod l */
#define PKCD_EDDSA_PBYTES       (sp_eddsa_pkccram->pbytes)           /**< A byte length of field p */
#define PKCD_EDDSA_OFFSET_VAL   (sp_eddsa_pkccram->offval) /**< A byte length of padding in each work area */
#define PKCD_EDDSA_PWORK        (sp_eddsa_pkccram->pwork)   /**< A 72byte work area which stores p */
#define PKCD_EDDSA_LWORK        (sp_eddsa_pkccram->lwork)   /**< A 72byte work area which stores l */
#define PKCD_EDDSA_ARMODP       (sp_eddsa_pkccram->armodpwork) /**< A 72byte work area which stores aR mod p */
#define PKCD_EDDSA_DRMODP       (sp_eddsa_pkccram->drmodpwork) /**< A 72byte work area which stores dR mod p */
#define PKCD_EDDSA_R2MODP       (sp_eddsa_pkccram->r2modpwork) /**< A 72byte work area which stores R^2 mod p */
#define PKCD_EDDSA_R2MODL       (sp_eddsa_pkccram->r2modlwork) /**< A 72byte work area which stores R^2 mod l */
#define PKCD_EDDSA_WORK1        (sp_eddsa_pkccram->work1)   /**< A 72byte work area */
#define PKCD_EDDSA_WORK2        (sp_eddsa_pkccram->work2)   /**< A 72byte work area */
#define PKCD_EDDSA_WORK3        (sp_eddsa_pkccram->work3)   /**< A 72byte work area */
#define PKCD_EDDSA_WORK4        (sp_eddsa_pkccram->work4)   /**< A 72byte work area */
#define PKCD_EDDSA_WORK5        (sp_eddsa_pkccram->work5)   /**< A 72byte work area */
#define PKCD_EDDSA_WORK6        (sp_eddsa_pkccram->work6)   /**< A 72byte work area */
#define PKCD_EDDSA_WORK7        (sp_eddsa_pkccram->work7)   /**< A 72byte work area */
#define PKCD_EDDSA_PNTWORK1     (&sp_eddsa_pkccram->pntwork1)   /**< A 216byte work area */
#define PKCD_EDDSA_PNTWORK2     (&sp_eddsa_pkccram->pntwork2)   /**< A 216byte work area */
#define PKCD_EDDSA_PNTWORK3     (&sp_eddsa_pkccram->pntwork3)   /**< A 216byte work area */
#define PKCD_EDDSA_PNTWORK4     (&sp_eddsa_pkccram->pntwork4)   /**< A 216byte work area */
#define PKCD_EDDSA_WWORK        ((uint8_t*)sp_eddsa_pkccram->wwork) /**< A 216byte work area */

/** Convert pointer shifted by PDT data area size to PKCC data pointer register format */
#define PKCD_EDDSA_FORMAT_DP_DPT(top)          (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_EDDSA_WORKUNITPDT))
/** Convert pointer shifted by PDW data area size to PKCC data pointer register format */
#define PKCD_EDDSA_FORMAT_DP_DPW(top)          (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_EDDSA_WORKUNITPDW))
/** Convert pointer shifted by unit size of work to PKCC data pointer register format */
#define PKCD_EDDSA_FORMAT_DP_UNIT(top)         (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_EDDSA_WORKUNIT))


#define PKCD_EDDSA_ED25519_DIGEST_LEN         (PKCD_EDDSA_SHA512BYTES)      /**< Maximum length of Ed25519 digest */
#define PKCD_EDDSA_ED448_DIGEST_LEN           (116U)                        /**< Maximum length of Ed448 digest.
                                                                                 This value is SHAKE256BYTES rounded up
                                                                                 to a multiple of 4. */
#define PKCD_EDDSA_ED25519_KEY_LEN            (32U)                         /**< Edwards25519 key length */
#define PKCD_EDDSA_ED448_KEY_LEN              (57U)                         /**< Edwards448 key length */
#define PKCD_EDDSA_ED448_RU4_KEY_LEN          (60U)                         /**< Ed448 key length rounded up to
                                                                                 a multiple of 4 */
#define PKCD_EDDSA_KEY_LEN_MAX                (PKCD_EDDSA_ED448_RU4_KEY_LEN)/**< EdDSA key length max size */
#define PKCD_EDDSA_DIGEST_LEN_MAX             (PKCD_EDDSA_ED448_DIGEST_LEN) /**< EdDSA digest length max size */

#define PKCD_EDDSA_CURVE_PARAM_IDX_FIELD        (0U)   /**< Field p index of concatenated curve parameters */
#define PKCD_EDDSA_CURVE_PARAM_IDX_ORDER        (1U)   /**< Order n index of concatenated curve parameters */
#define PKCD_EDDSA_CURVE_PARAM_IDX_COEFF1       (2U)   /**< Coefficient a index of concatenated curve parameters */
#define PKCD_EDDSA_CURVE_PARAM_IDX_COEFF2       (3U)   /**< Coefficient b index of concatenated curve parameters */
#define PKCD_EDDSA_CURVE_PARAM_IDX_BASE_X       (4U)   /**< Base point x index of concatenated curve parameters */
#define PKCD_EDDSA_CURVE_PARAM_IDX_BASE_Y       (5U)   /**< Base point y of concatenated curve parameters */
#define PKCD_EDDSA_CURVE_PARAM_IDX_ALGO_OPT1    (6U)   /**< Algorithm option 1 parameters(Ed25519:(p+3)/8) */
#define PKCD_EDDSA_CURVE_PARAM_IDX_ALGO_OPT2    (7U)   /**< Algorithm option 2 parameters(Ed25519:2^((p-1)/4)) */
#define PKCD_EDDSA_CURVE_PARAM_NUM              (8U)   /**< Number of Concatenated curve parameters */

#define PKCD_EDDSA_MSG_LEN_MAX                  (0x80000000U)   /**< Maximum length of message */

#define PKCD_EDDSA_MASK_F8                     (0xF8U)    /**< the value used for mask the scalar value */
#define PKCD_EDDSA_MASK_3F                     (0x3FU)    /**< the value used for mask the scalar value */
#define PKCD_EDDSA_MASK_40                     (0x40U)    /**< the value used for mask the scalar value */
#define PKCD_EDDSA_MASK_FC                     (0xFCU)    /**< the value used for mask the scalar value */
#define PKCD_EDDSA_MASK_80                     (0x80U)    /**< the value used for mask the scalar value */

#define PKCD_EDDSA_MASK_07                     (0x07U)    /**< mask value 0x07 */
#define PKCD_EDDSA_MASK_MSB                    (0x80U)    /**< Mask to most significant bit */
#define PKCD_EDDSA_CLEAR_MSB                   (0x7FU)    /**< Clear most significant bit */

#define PKCD_EDDSA_SHIFT_7                     (7U)       /**< Shift 7 value */

#define PKCD_EDDSA_ED448_SECOND_TO_LAST_IDX    (PKCD_EDDSA_ED448_KEY_LEN - 2U) /**< Second to last index */

#define PKCD_EDDSA_ZERO_PADDING_3              (3U)       /**< Set zero padding for 3 octets on Ed448 */
#define PKCD_EDDSA_ZERO_PADDING_4              (4U)       /**< Set zero padding for 4 octets on Ed448 */

#define PKCD_EDDSA_ADDINGLIST_NUM              (2U)       /**< Number of adding list array */

#define PKCD_EDDSA_ED25519_ASCII_SIZE          (32U)      /**< Ed25519 context ASCII string size */
#define PKCD_EDDSA_ED448_ASCII_SIZE            (8U)       /**< Ed448 context ASCII string size */

#define PKCD_EDDSA_ED_CTX_LEN_MIN              (1U)       /**< Minimum Ed context length when Ed context is required */
#define PKCD_EDDSA_ED_CTX_LEN_MAX              (255U)     /**< Maximum length of Ed context */

#define PKCD_EDDSA_DOUBLE_LEN                  (2U)       /**< Definition to calculate twice the length */
#define PKCD_EDDSA_TRIPLE_LEN                  (3U)       /**< Definition to calculate length times three */

/** Number of bits processed at one time by the comb method */
#define PKCD_EDDSA_COMB_4_BITS          (4U)
#define PKCD_EDDSA_COMB_8_BITS          (8U)
#define PKCD_EDDSA_BIT_80               (0x80U) /**< bit value 0x80 */
#define PKCD_EDDSA_BIT_40               (0x40U) /**< bit value 0x40 */
#define PKCD_EDDSA_BIT_20               (0x20U) /**< bit value 0x20 */
#define PKCD_EDDSA_BIT_10               (0x10U) /**< bit value 0x10 */
#define PKCD_EDDSA_BIT_08               (0x08U) /**< bit value 0x08 */
#define PKCD_EDDSA_BIT_04               (0x04U) /**< bit value 0x04 */
#define PKCD_EDDSA_BIT_02               (0x02U) /**< bit value 0x02 */
#define PKCD_EDDSA_BIT_01               (0x01U) /**< bit value 0x01 */
#define PKCD_EDDSA_BITS_POS_04          (0x04U) /**< bits position 0x04 */
#define PKCD_EDDSA_BITS_POS_07          (0x07U) /**< bits position 0x07 */
#define PKCD_EDDSA_BITS_POS_08          (0x08U) /**< bits position 0x08 */
#define PKCD_EDDSA_BITS_POS_0C          (0x0CU) /**< bits position 0x0C */
#define PKCD_EDDSA_BITS_POS_0E          (0x0EU) /**< bits position 0x0E */
#define PKCD_EDDSA_BITS_POS_10          (0x10U) /**< bits position 0x10 */
#define PKCD_EDDSA_BITS_POS_14          (0x14U) /**< bits position 0x14 */
#define PKCD_EDDSA_BITS_POS_15          (0x15U) /**< bits position 0x15 */
#define PKCD_EDDSA_BITS_POS_18          (0x18U) /**< bits position 0x18 */
#define PKCD_EDDSA_BITS_POS_1C          (0x1CU) /**< bits position 0x1C */
#define PKCD_EDDSA_BITS_POS_23          (0x23U) /**< bits position 0x23 */
#define PKCD_EDDSA_BITS_POS_2A          (0x2AU) /**< bits position 0x2A */
#define PKCD_EDDSA_BITS_POS_31          (0x31U) /**< bits position 0x31 */

/*! \}*/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDEDDSATypesInternal
 * \{******************************************************************/
/** Work area structure for EDDSA operation */
typedef union
{
    uint8_t    full[PKCD_EDDSA_WORKUNITPNT];      /**< All of this structure */
    struct
    {
        uint8_t    x[PKCD_EDDSA_WORKUNIT];        /**< A work area for x-coordinate */
        uint8_t    y[PKCD_EDDSA_WORKUNIT];        /**< A work area for y-coordinate */
        uint8_t    z[PKCD_EDDSA_WORKUNIT];        /**< A work area for z-coordinate */
    } point;                            /**< Treat as projective coordinate */
} u_eddsa_work_t;

typedef struct
{
    uint8_t         pdt_p[PKCD_EDDSA_WORKUNITPDT];     /**< A work area for T mod p */
    uint8_t         pdt_l[PKCD_EDDSA_WORKUNITPDT];     /**< A work area for T mod l */
    uint32_t        pbytes;                  /**< A byte length of field p */
    uint32_t        offval;                  /**< A byte length of padding in each work area */
    uint8_t         pwork[PKCD_EDDSA_WORKUNIT];        /**< A 72byte work area which stores p */
    uint8_t         lwork[PKCD_EDDSA_WORKUNIT];        /**< A 72byte work area which stores l */
    uint8_t         armodpwork[PKCD_EDDSA_WORKUNIT];   /**< A 72byte work area which stores aR mod p */
    uint8_t         drmodpwork[PKCD_EDDSA_WORKUNIT];   /**< A 72byte work area which stores dR mod p */
    uint8_t         r2modpwork[PKCD_EDDSA_WORKUNIT];   /**< A 72byte work area which stores R^2 mod p */
    uint8_t         r2modlwork[PKCD_EDDSA_WORKUNIT];   /**< A 72byte work area which stores R^2 mod l */
    uint8_t         work1[PKCD_EDDSA_WORKUNIT];        /**< A 72byte work area */
    uint8_t         work2[PKCD_EDDSA_WORKUNIT];        /**< A 72byte work area */
    uint8_t         work3[PKCD_EDDSA_WORKUNIT];        /**< A 72byte work area */
    uint8_t         work4[PKCD_EDDSA_WORKUNIT];        /**< A 72byte work area */
    uint8_t         work5[PKCD_EDDSA_WORKUNIT];        /**< A 72byte work area */
    uint8_t         work6[PKCD_EDDSA_WORKUNIT];        /**< A 72byte work area */
    uint8_t         work7[PKCD_EDDSA_WORKUNIT];        /**< A 72byte work area */
    u_eddsa_work_t  pntwork1;                /**< A 216byte work area */
    u_eddsa_work_t  pntwork2;                /**< A 216byte work area */
    u_eddsa_work_t  pntwork3;                /**< A 216byte work area */
    u_eddsa_work_t  pntwork4;                /**< A 216byte work area */
    uint8_t         wwork[PKCD_EDDSA_WORKUNITPDW];     /**< A 216byte work area */
} st_eddsa_pkccram_t;

/** Works structure */
typedef struct
{
    uint8_t * p_wk1;    /**< An address of work area 1 */
    uint8_t * p_wk2;    /**< An address of work area 2 */
    uint8_t * p_wk3;    /**< An address of work area 3 */
    uint8_t * p_wk4;    /**< An address of work area 4 */
    uint8_t * p_wk5;    /**< An address of work area 5 */
    uint8_t * p_wk6;    /**< An address of work area 6 */
} st_eddsa_wks_t;

/** Data pointer of works structure */
typedef struct
{
    uint32_t dpwk1;     /**< An address of work area 1 in DP format */
    uint32_t dpwk2;     /**< An address of work area 2 in DP format */
    uint32_t dpwk3;     /**< An address of work area 3 in DP format */
    uint32_t dpwk4;     /**< An address of work area 4 in DP format */
    uint32_t dpwk5;     /**< An address of work area 5 in DP format */
    uint32_t dpwk6;     /**< An address of work area 6 in DP format */
} st_eddsa_dpwks_t;

/** EdDSA domain parameter structure of internal */
typedef struct
{
    uint32_t    len;                /**< Byte length of a curve len(must be a multiple of 4) */
    uint8_t *   p_field;            /**< Field order */
    uint8_t *   p_order;            /**< Base point order */
    uint8_t *   p_coeff1;           /**< Coefficient a */
    uint8_t *   p_coeff2;           /**< Coefficient b */
    uint8_t *   p_base_x;           /**< Base point x */
    uint8_t *   p_base_y;           /**< Base point y */
    uint8_t *   p_algo_opt1;        /**< Algorithm option 1 parameter (Ed25519:(p+3)/8, Ed448:(p+1)/4) */
    uint8_t *   p_algo_opt2;        /**< Algorithm option 2 parameter (Ed25519:2^((p-1)/4), Ed448:None) */
} st_eddsa_curve_t;

/** Scalar value structure of internal */
typedef struct
{
    uint32_t len;                   /**< A byte length of a scalar */
    uint8_t  scalar[PKCD_EDDSA_DIGEST_LEN_MAX];  /**< A buffer of scalar */
} st_eddsa_scalar_t;

/** A parameter structure of scalar multiplication with the base point */
typedef struct
{
    st_eddsa_scalar_t scalar;           /**< A scalar value */
    uint32_t          executed_bits;    /**< Number of bits for which the binary method has been executed */
} st_eddsa_prm_smultbase_t;

/** A parameter structure of scalar multiplication */
typedef struct
{
    st_eddsa_scalar_t scalar;           /**< A scalar value */
    st_eddsa_scalar_t comp_point;       /**< A compressed point to be multiplied */
    uint32_t          executed_bits;    /**< Number of bits for which the binary method has been executed */
} st_eddsa_prm_smult_t;

/** A parameter structure of derive signature s */
typedef struct
{
    st_eddsa_scalar_t scalar_s;         /**< A scalar s */
    st_eddsa_scalar_t scalar_r;         /**< A scalar r */
    st_eddsa_scalar_t hram;             /**< Hash(R, A, M) */
} st_eddsa_prm_derive_s_t;

/*! \}*/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDEDDSAVariablesInternal
 * \{******************************************************************/
/** PKCD_CMN_PKCCRAM address cast in st_eddsa_pkccram_t */
static st_eddsa_pkccram_t * const sp_eddsa_pkccram = ((st_eddsa_pkccram_t*)PKCD_CMN_PKCCRAM);

/** 32 byte ASCII string in Ed25519 context */
static const uint8_t s_ed25519_ascii[PKCD_EDDSA_ED25519_ASCII_SIZE+1U] = "SigEd25519 no Ed25519 collisions";

/** 8 byte ASCII string in Ed448 context */
static const uint8_t s_ed448_ascii[PKCD_EDDSA_ED448_ASCII_SIZE+1U] = "SigEd448";

/*! \}*/

static pkcd_ret_t r_pkcd_eddsa_precalc_params (const st_eddsa_curve_t * const p_crv);
static pkcd_ret_t r_pkcd_eddsa_point_compress (const u_eddsa_work_t * const p_p1, uint8_t * const p_comp,
                                                const uint8_t * const p_wk1, const uint8_t * const p_wk2,
                                                uint8_t * const p_wk3);
static pkcd_ret_t r_pkcd_ed25519_point_decompress (const uint8_t * const p_compressed,
                                                    const uint8_t * const p_algo_opt1,
                                                    const uint8_t * const p_algo_opt2,
                                                    u_eddsa_work_t * const p_p3,
                                                    const st_eddsa_wks_t * const p_works);
static pkcd_ret_t r_pkcd_ed448_point_decompress (const uint8_t * const p_compressed,
                                                    const uint8_t * const p_algo_opt1,
                                                    u_eddsa_work_t * const p_p3,
                                                    const st_eddsa_wks_t * const p_works);
static pkcd_ret_t r_pkcd_eddsa_point_smult (u_eddsa_work_t * const p_p1, u_eddsa_work_t * const p_p3,
                                            u_eddsa_work_t * const p_temp, u_eddsa_work_t * const p_neutral,
                                            const uint8_t * const p_scalar, const st_eddsa_wks_t * const p_works,
                                            const uint32_t processing_bits, uint32_t * const p_executed_bits);
static pkcd_ret_t r_pkcd_ed25519_smult_base (u_eddsa_work_t * const p_p3, u_eddsa_work_t * const p_temp,
                                            u_eddsa_work_t * const p_temp2, const uint8_t * const p_scalar,
                                            const st_eddsa_wks_t * const p_works, const uint32_t processing_bits,
                                            uint32_t * const p_executed_bits);
static pkcd_ret_t r_pkcd_ed448_smult_base (u_eddsa_work_t * const p_p3, u_eddsa_work_t * const p_temp,
                                            u_eddsa_work_t * const p_temp2, const uint8_t * const p_scalar,
                                            const st_eddsa_wks_t * const p_works, const uint32_t processing_bits,
                                            uint32_t * const p_executed_bits);
static pkcd_ret_t r_pkcd_eddsa_point_add_inline (const u_eddsa_work_t * const p_p1, const u_eddsa_work_t * const p_p2,
                                                const u_eddsa_work_t * const p_p3,
                                                const st_eddsa_dpwks_t * const p_dpwks);
static pkcd_ret_t r_pkcd_eddsa_point_dbl_inline (const u_eddsa_work_t * const p_p1, const u_eddsa_work_t * const p_p3,
                                                const st_eddsa_dpwks_t * const p_dpwks);
static pkcd_ret_t r_pkcd_eddsa_parse_curve (st_eddsa_curve_t * const p_curve,
                                            const st_pkc_ec_curve_t * const p_curve_info);
static pkcd_ret_t r_pkcd_eddsa_chk_scalar_mult_base_init_arg (const st_pkc_select_data_t * const p_scalar_or_privkey,
                                                                const st_pkc_eddsa_ctx_t * const p_ctx,
                                                                const uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_eddsa_chk_scalar_mult_init_arg (const st_pkc_select_data_t * const p_scalar_or_privkey,
                                                        const st_pkc_select_data_t * const p_comp_point_or_pubkey,
                                                        const st_pkc_eddsa_ctx_t * const p_ctx,
                                                        const uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_eddsa_chk_derive_s_arg (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                const uint8_t * const p_buf,
                                                const st_pkc_data_t * const p_scalar_s,
                                                const st_pkc_data_t * const p_scalar_r,
                                                const st_pkc_data_t * const p_digest_r_a_m,
                                                const st_pkc_data_t * const p_sign_s);
static pkcd_ret_t r_pkcd_eddsa_chk_compare_sign_arg (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                    const uint8_t * const p_buf,
                                                    const st_pkc_data_t * const p_point_ha,
                                                    const st_pkc_data_t * const p_point_sb,
                                                    const st_pkc_data_t * const p_sign_r);
static pkcd_ret_t r_pkcd_eddsa_chk_make_scalar_s_and_r_arg (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                            const uint8_t * const p_buf,
                                                            const void * const p_priv_key,
                                                            const st_pkc_data_t * const p_msg,
                                                            const st_pkc_data_t * const p_scalar_s,
                                                            const st_pkc_data_t * const p_scalar_r);
static pkcd_ret_t r_pkcd_eddsa_chk_hash_r_a_m_arg (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                const uint8_t * const p_buf,
                                                const st_pkc_data_t * const p_sign_r,
                                                const void * const p_pub_key,
                                                const st_pkc_data_t * const p_msg,
                                                const st_pkc_data_t * const p_digest_r_a_m);
static pkcd_ret_t r_pkcd_eddsa_chk_key_wrap_arg (const st_pkc_eddsa_ctx_t * const p_ctx,
                                            const uint8_t * const p_buf,
                                            const st_pkc_data_t * const p_priv_key_raw,
                                            const st_pkc_data_t * const p_pub_key_raw,
                                            const void * const p_priv_key,
                                            const void * const p_pub_key);
static pkcd_ret_t r_pkcd_eddsa_chk_scalar (const st_pkc_data_t * const p_scalar, const uint32_t curve_len);
static pkcd_ret_t r_pkcd_eddsa_chk_digest (const st_pkc_data_t * const p_digest, const uint32_t curve_len);
static pkcd_ret_t r_pkcd_eddsa_chk_scalar_or_digest (const st_pkc_data_t * const p_scalar_or_digest,
                                                    const uint32_t curve_len);
static pkcd_ret_t r_pkcd_eddsa_chk_msg (const st_pkc_data_t * const p_msg);
static pkcd_ret_t r_pkcd_eddsa_chk_priv_key (const st_pkcd_eddsa_key_bin_t * const p_priv_key_bin,
                                                const uint32_t curve_len);
static pkcd_ret_t r_pkcd_eddsa_chk_pub_key (const st_pkcd_eddsa_key_bin_t * const p_pub_key_bin,
                                                const uint32_t curve_len);
static pkcd_ret_t r_pkcd_eddsa_chk_curve_info (const st_pkc_ec_curve_t * const p_curve_info);
static pkcd_ret_t r_pkcd_eddsa_chk_curve_len (const uint32_t curve_len);
static pkcd_ret_t r_pkcd_eddsa_chk_variant (const e_pkc_eddsa_variant_t eddsa_variant,
                                            const st_pkc_data_t * const p_ed_ctx, const uint32_t curve_len);
static void r_pkcd_eddsa_set_len_info (const uint32_t curve_len);
static void r_pkcd_eddsa_encode_scalar (st_eddsa_scalar_t * const p_scalar);
static pkcd_ret_t r_pkcd_eddsa_make_scalar_s(const st_pkc_eddsa_ctx_t* const p_ctx,
                                            const st_pkcd_eddsa_key_bin_t * p_key_bin,
                                            const uint8_t * const p_ascii_data,
                                            const uint32_t ascii_len,
                                            const uint32_t key_len,
                                            st_eddsa_scalar_t* hashed_priv_key,
                                            st_pkc_data_t* const p_scalar_s);
static pkcd_ret_t r_pkcd_eddsa_make_scalar_r(const st_pkc_eddsa_ctx_t* const p_ctx,
                                            const st_pkc_data_t* const p_msg,
                                            const uint8_t * const p_ascii_data,
                                            const uint32_t ascii_len,
                                            const uint32_t key_len,
                                            const st_eddsa_scalar_t* hashed_priv_key,
                                            st_pkc_data_t* const p_scalar_r);

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaScalarMultBaseInit
**********************************************************************************************************************/
pkcd_ret_t   R_PKCD_EddsaScalarMultBaseInit(const st_pkc_select_data_t* const p_scalar_or_privkey,
                                            const st_pkc_eddsa_ctx_t* const p_ctx,
                                            uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_eddsa_prm_smultbase_t * const p_prm = (st_eddsa_prm_smultbase_t*)p_buf;//No problem with casting to pointer size
    const st_pkcd_eddsa_key_bin_t * p_key_bin;

    /* Check argument */
    ret = r_pkcd_eddsa_chk_scalar_mult_base_init_arg(p_scalar_or_privkey, p_ctx, p_buf);
    if (PKCD_RET_OK == ret)
    {
        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* Clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        /* Clear buffer */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_eddsa_prm_smultbase_t));

        /* clear caluculation bits */
        p_prm->executed_bits = 0U;

        /* Copy parameters */
        if (PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
        {
            /* No problem with casting to pointer size */
            p_key_bin = (st_pkcd_eddsa_key_bin_t*)p_scalar_or_privkey->p_sel_data->p_data;
            r_pkcd_cmn_cpy_word(p_prm->scalar.scalar, p_key_bin->p_private, p_key_bin->key_length);
            p_prm->scalar.len = p_key_bin->key_length;
        }
        else
        {
            r_pkcd_cmn_cpy_word(p_prm->scalar.scalar, p_scalar_or_privkey->p_sel_data->p_data,
                                p_scalar_or_privkey->p_sel_data->len);
            p_prm->scalar.len = p_scalar_or_privkey->p_sel_data->len;
        }
        r_pkcd_eddsa_encode_scalar(&p_prm->scalar);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EddsaScalarMultBaseInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaScalarMultBaseOperate
**********************************************************************************************************************/
pkcd_ret_t  R_PKCD_EddsaScalarMultBaseOperate(const st_pkc_eddsa_ctx_t* const p_ctx,
                                                uint8_t* const p_buf,
                                                const uint32_t processing_bits)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_eddsa_wks_t wks;
    st_eddsa_curve_t curve;
    st_eddsa_prm_smultbase_t * const p_prm = (st_eddsa_prm_smultbase_t*)p_buf;//No problem with casting to pointer size

    /* Check arguments */
    if ((NULL != p_ctx) && (NULL != p_buf))
    {
        if ((PKCD_CMN_PROCESSING_BITS_MIN <= processing_bits) &&
            ((processing_bits <= PKCD_CMN_PROCESSING_BITS_MAX)))
        {
            ret = r_pkcd_eddsa_chk_curve_info(p_ctx->p_curve_info);
            if (PKCD_RET_OK == ret)
            {
                /* Parse curve */
                ret = r_pkcd_eddsa_parse_curve(&curve, p_ctx->p_curve_info);
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCD_RET_LENGTHERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    if (PKCD_RET_OK == ret)
    {
        /* initialize works structure */
        wks.p_wk1 = PKCD_EDDSA_WORK1;
        wks.p_wk2 = PKCD_EDDSA_WORK2;
        wks.p_wk3 = PKCD_EDDSA_WORK3;
        wks.p_wk4 = PKCD_EDDSA_WORK4;
        wks.p_wk5 = PKCD_EDDSA_WORK5;
        wks.p_wk6 = PKCD_EDDSA_WORK6;

        if (0U == p_prm->executed_bits)
        {
            /* initialise params */
            r_pkcd_eddsa_set_len_info(curve.len);
            ret = r_pkcd_eddsa_precalc_params(&curve);

            /* calc scalar mod L */
            PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EDDSA_FORMAT_DP_DPT(PKCD_EDDSA_PDT_L);
            ret |= r_pkcd_cmn_reduce_small_modulus(p_prm->scalar.scalar, PKCD_EDDSA_LWORK, PKCD_EDDSA_R2MODL,
                                                    p_prm->scalar.len, PKCD_EDDSA_PBYTES, PKCD_EDDSA_WORKUNIT,
                                                    PKCD_EDDSA_WORK1, PKCD_EDDSA_WORK2, PKCD_EDDSA_WORK4);
            r_pkcd_cmn_ram_clr(p_prm->scalar.scalar, p_prm->scalar.len);
            r_pkcd_cmn_cpy_by_64bounds(p_prm->scalar.scalar, PKCD_EDDSA_WORK4 + PKCD_EDDSA_OFFSET_VAL,
                                        PKCD_EDDSA_PBYTES);
            p_prm->scalar.len = PKCD_EDDSA_PBYTES;
        }
        else
        {
            /* Do nothing */
        }

        if (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == PKCD_EDDSA_PBYTES)
        {
            ret |= r_pkcd_ed25519_smult_base(PKCD_EDDSA_PNTWORK2, PKCD_EDDSA_PNTWORK3, PKCD_EDDSA_PNTWORK4,
                                                p_prm->scalar.scalar, &wks, processing_bits, &p_prm->executed_bits);
        }
        else
        {
            ret |= r_pkcd_ed448_smult_base(PKCD_EDDSA_PNTWORK2, PKCD_EDDSA_PNTWORK3, PKCD_EDDSA_PNTWORK4,
                                                p_prm->scalar.scalar, &wks, processing_bits, &p_prm->executed_bits);
        }

        /* Extract the return value from the ORed result */
        ret = r_pkcd_cmn_extract_result(ret);

        if (PKCD_RET_PROCESSING != ret)
        {
            /* Clear the value that may be the private key stored in p_prm */
            r_pkcd_cmn_ram_clr(p_prm->scalar.scalar, sizeof(p_prm->scalar.scalar));
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EddsaScalarMultBaseOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaScalarMultBaseFinal
**********************************************************************************************************************/
pkcd_ret_t  R_PKCD_EddsaScalarMultBaseFinal(const st_pkc_eddsa_ctx_t* const p_ctx,
                                            uint8_t* const p_buf,
                                            const e_pkc_bool_t comp_flag,
                                            st_pkc_data_t* const p_comp_point)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t comp_point_len;

    /* Check arguments */
    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_comp_point))
    {
        if (NULL != p_comp_point->p_data)
        {
            ret = r_pkcd_eddsa_chk_curve_info(p_ctx->p_curve_info);
            if (PKCD_RET_OK == ret)
            {
                /* compress check */
                if (PKC_BOOL_TRUE == comp_flag)
                {
                    comp_point_len = (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len) ?
                                        PKCD_EDDSA_ED25519_KEY_LEN : PKCD_EDDSA_ED448_RU4_KEY_LEN;

                    if (p_comp_point->len >= comp_point_len)
                    {
                        /* Compress scalar multiplication result */
                        ret = r_pkcd_eddsa_point_compress(PKCD_EDDSA_PNTWORK2, p_comp_point->p_data,
                                                            PKCD_EDDSA_WORK1, PKCD_EDDSA_WORK2, PKCD_EDDSA_WORK3);
                        p_comp_point->len = comp_point_len;
                    }
                    else
                    {
                        ret = PKCD_RET_LENGTHERR;
                    }
                }
                else if (PKC_BOOL_FALSE == comp_flag)
                {
                    if (p_comp_point->len >= (p_ctx->p_curve_info->curve_len * PKCD_EDDSA_TRIPLE_LEN))
                    {
                        r_pkcd_cmn_cpy_by_64bounds(p_comp_point->p_data,
                                                    PKCD_EDDSA_PNTWORK2->point.x + PKCD_EDDSA_OFFSET_VAL,
                                                    p_ctx->p_curve_info->curve_len);
                        r_pkcd_cmn_cpy_by_64bounds(p_comp_point->p_data + p_ctx->p_curve_info->curve_len,
                                                    PKCD_EDDSA_PNTWORK2->point.y + PKCD_EDDSA_OFFSET_VAL,
                                                    p_ctx->p_curve_info->curve_len);
                        r_pkcd_cmn_cpy_by_64bounds(p_comp_point->p_data +
                                                    (p_ctx->p_curve_info->curve_len * PKCD_EDDSA_DOUBLE_LEN),
                                                    PKCD_EDDSA_PNTWORK2->point.z + PKCD_EDDSA_OFFSET_VAL,
                                                    p_ctx->p_curve_info->curve_len);
                        p_comp_point->len = p_ctx->p_curve_info->curve_len * PKCD_EDDSA_TRIPLE_LEN;
                    }
                    else
                    {
                        ret = PKCD_RET_LENGTHERR;
                    }
                }
                else
                {
                    ret = PKCD_RET_PARAMERR;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EddsaScalarMultBaseFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaScalarMultInit
**********************************************************************************************************************/
pkcd_ret_t   R_PKCD_EddsaScalarMultInit(const st_pkc_select_data_t* const p_scalar_or_privkey,
                                        const st_pkc_select_data_t* const p_comp_point_or_pubkey,
                                        const st_pkc_eddsa_ctx_t* const p_ctx,
                                        uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_eddsa_prm_smult_t * const p_prm = (st_eddsa_prm_smult_t*)p_buf; /* No problem with casting to pointer size */
    const st_pkcd_eddsa_key_bin_t * p_key_bin;

    /* Check argument */
    ret = r_pkcd_eddsa_chk_scalar_mult_init_arg(p_scalar_or_privkey, p_comp_point_or_pubkey, p_ctx, p_buf);
    if (PKCD_RET_OK == ret)
    {
        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* Clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        /* Clear buffer */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_eddsa_prm_smult_t));

        /* clear caluculation bits */
        p_prm->executed_bits = 0U;

        /* Copy parameters */
        if (PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
        {
            /* No problem with casting to pointer size */
            p_key_bin = (st_pkcd_eddsa_key_bin_t*)p_scalar_or_privkey->p_sel_data->p_data;
            r_pkcd_cmn_cpy_word(p_prm->scalar.scalar, p_key_bin->p_private, p_key_bin->key_length);
            p_prm->scalar.len = p_key_bin->key_length;
        }
        else
        {
            r_pkcd_cmn_cpy_word(p_prm->scalar.scalar, p_scalar_or_privkey->p_sel_data->p_data,
                                p_scalar_or_privkey->p_sel_data->len);
            p_prm->scalar.len = p_scalar_or_privkey->p_sel_data->len;
        }
        r_pkcd_eddsa_encode_scalar(&p_prm->scalar);

        if (PKC_DATA_TYPE_KEY_BINARY == p_comp_point_or_pubkey->data_type)
        {
            /* No problem with casting to pointer size */
            p_key_bin = (st_pkcd_eddsa_key_bin_t*)p_comp_point_or_pubkey->p_sel_data->p_data;
            r_pkcd_cmn_cpy_word(p_prm->comp_point.scalar, p_key_bin->p_public,
                                p_key_bin->key_length);
            p_prm->comp_point.len = p_key_bin->key_length;
        }
        else
        {
            r_pkcd_cmn_cpy_word(p_prm->comp_point.scalar, p_comp_point_or_pubkey->p_sel_data->p_data,
                                p_comp_point_or_pubkey->p_sel_data->len);
            p_prm->comp_point.len = p_comp_point_or_pubkey->p_sel_data->len;
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EddsaScalarMultInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaScalarMultOperate
**********************************************************************************************************************/
pkcd_ret_t  R_PKCD_EddsaScalarMultOperate(const st_pkc_eddsa_ctx_t* const p_ctx,
                                            uint8_t* const p_buf,
                                            const uint32_t processing_bits)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_eddsa_wks_t wks;
    st_eddsa_curve_t curve;
    st_eddsa_prm_smult_t * const p_prm = (st_eddsa_prm_smult_t*)p_buf;  /* No problem with casting to pointer size */

    /* Check arguments */
    if ((NULL != p_ctx) && (NULL != p_buf))
    {
        if ((PKCD_CMN_PROCESSING_BITS_MIN <= processing_bits) &&
            ((processing_bits <= PKCD_CMN_PROCESSING_BITS_MAX)))
        {
            ret = r_pkcd_eddsa_chk_curve_info(p_ctx->p_curve_info);
            if (PKCD_RET_OK == ret)
            {
                /* Parse curve */
                ret = r_pkcd_eddsa_parse_curve(&curve, p_ctx->p_curve_info);
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCD_RET_LENGTHERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    if (PKCD_RET_OK == ret)
    {
        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* initialize works structure */
        wks.p_wk1 = PKCD_EDDSA_WORK1;
        wks.p_wk2 = PKCD_EDDSA_WORK2;
        wks.p_wk3 = PKCD_EDDSA_WORK3;
        wks.p_wk4 = PKCD_EDDSA_WORK4;
        wks.p_wk5 = PKCD_EDDSA_WORK5;
        wks.p_wk6 = PKCD_EDDSA_WORK6;

        if (0U == p_prm->executed_bits)
        {
            /* initialise params */
            r_pkcd_eddsa_set_len_info(curve.len);
            ret = r_pkcd_eddsa_precalc_params(&curve);

            /* calc scalar mod L */
            PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EDDSA_FORMAT_DP_DPT(PKCD_EDDSA_PDT_L);
            ret |= r_pkcd_cmn_reduce_small_modulus(p_prm->scalar.scalar, PKCD_EDDSA_LWORK, PKCD_EDDSA_R2MODL,
                                                    p_prm->scalar.len, PKCD_EDDSA_PBYTES, PKCD_EDDSA_WORKUNIT,
                                                    PKCD_EDDSA_WORK1, PKCD_EDDSA_WORK2, PKCD_EDDSA_WORK4);
            r_pkcd_cmn_ram_clr(p_prm->scalar.scalar, sizeof(p_prm->scalar.scalar));
            r_pkcd_cmn_cpy_by_64bounds(p_prm->scalar.scalar, PKCD_EDDSA_WORK4 + PKCD_EDDSA_OFFSET_VAL,
                                        PKCD_EDDSA_PBYTES);
            p_prm->scalar.len = PKCD_EDDSA_PBYTES;

            if (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == PKCD_EDDSA_PBYTES)
            {
                /* point decompress */
                ret |= r_pkcd_ed25519_point_decompress(p_prm->comp_point.scalar, curve.p_algo_opt1,
                                                        curve.p_algo_opt2, PKCD_EDDSA_PNTWORK1, &wks);
            }
            else
            {
                /* point decompress */
                ret |= r_pkcd_ed448_point_decompress(p_prm->comp_point.scalar, curve.p_algo_opt1,
                                                        PKCD_EDDSA_PNTWORK1, &wks);
            }
        }
        else
        {
            /* Do nothing */
        }

        ret |= r_pkcd_eddsa_point_smult(PKCD_EDDSA_PNTWORK1, PKCD_EDDSA_PNTWORK2, PKCD_EDDSA_PNTWORK3,
                                        PKCD_EDDSA_PNTWORK4, p_prm->scalar.scalar, &wks, processing_bits,
                                        &p_prm->executed_bits);

        /* Extract the return value from the ORed result */
        ret = r_pkcd_cmn_extract_result(ret);

        if (PKCD_RET_PROCESSING != ret)
        {
            /* Clear the value that may be the private key stored in p_prm */
            r_pkcd_cmn_ram_clr(p_prm->scalar.scalar, sizeof(p_prm->scalar.scalar));
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EddsaScalarMultOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaScalarMultFinal
**********************************************************************************************************************/
pkcd_ret_t  R_PKCD_EddsaScalarMultFinal(const st_pkc_eddsa_ctx_t* const p_ctx,
                                        uint8_t* const p_buf,
                                        const e_pkc_bool_t comp_flag,
                                        st_pkc_data_t* const p_comp_point)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t comp_point_len;

    /* Check arguments */
    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_comp_point))
    {
        if (NULL != p_comp_point->p_data)
        {
            ret = r_pkcd_eddsa_chk_curve_info(p_ctx->p_curve_info);
            if (PKCD_RET_OK == ret)
            {
                /* compress check */
                if (PKC_BOOL_TRUE == comp_flag)
                {
                    comp_point_len = (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len) ?
                                        PKCD_EDDSA_ED25519_KEY_LEN : PKCD_EDDSA_ED448_RU4_KEY_LEN;
                    if (p_comp_point->len >= comp_point_len)
                    {
                        /* Clear async operation flag */
                        r_pkcd_cmn_clr_async_ope();

                        /* Comppress scalar multiplication result */
                        ret = r_pkcd_eddsa_point_compress(PKCD_EDDSA_PNTWORK2, p_comp_point->p_data,
                                                            PKCD_EDDSA_WORK1, PKCD_EDDSA_WORK2, PKCD_EDDSA_WORK3);
                        p_comp_point->len = comp_point_len;
                    }
                    else
                    {
                        ret = PKCD_RET_LENGTHERR;
                    }
                }
                else if (PKC_BOOL_FALSE == comp_flag)
                {
                    if (p_comp_point->len >= (p_ctx->p_curve_info->curve_len * PKCD_EDDSA_TRIPLE_LEN))
                    {
                        r_pkcd_cmn_cpy_by_64bounds(p_comp_point->p_data,
                                                    PKCD_EDDSA_PNTWORK2->point.x + PKCD_EDDSA_OFFSET_VAL,
                                                    p_ctx->p_curve_info->curve_len);
                        r_pkcd_cmn_cpy_by_64bounds(p_comp_point->p_data + p_ctx->p_curve_info->curve_len,
                                                    PKCD_EDDSA_PNTWORK2->point.y + PKCD_EDDSA_OFFSET_VAL,
                                                    p_ctx->p_curve_info->curve_len);
                        r_pkcd_cmn_cpy_by_64bounds(p_comp_point->p_data +
                                                    (p_ctx->p_curve_info->curve_len * PKCD_EDDSA_DOUBLE_LEN),
                                                    PKCD_EDDSA_PNTWORK2->point.z + PKCD_EDDSA_OFFSET_VAL,
                                                    p_ctx->p_curve_info->curve_len);
                        p_comp_point->len = p_ctx->p_curve_info->curve_len * PKCD_EDDSA_TRIPLE_LEN;
                    }
                    else
                    {
                        ret = PKCD_RET_LENGTHERR;
                    }
                }
                else
                {
                    ret = PKCD_RET_PARAMERR;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EddsaScalarMultFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaDeriveS
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EddsaDeriveS(const st_pkc_eddsa_ctx_t* const p_ctx,
                                uint8_t* const p_buf,
                                const st_pkc_data_t* const p_scalar_s,
                                const st_pkc_data_t* const p_scalar_r,
                                const st_pkc_data_t* const p_digest_r_a_m,
                                st_pkc_data_t* const p_sign_s)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_eddsa_curve_t curve;
    st_eddsa_prm_derive_s_t * const p_prm = (st_eddsa_prm_derive_s_t*)p_buf; // No problem with casting to pointer size
    st_eddsa_scalar_t tmp_sign_s;

    /* Check argument */
    ret = r_pkcd_eddsa_chk_derive_s_arg(p_ctx, p_buf, p_scalar_s, p_scalar_r, p_digest_r_a_m, p_sign_s);
    if (PKCD_RET_OK == ret)
    {
        ret = r_pkcd_eddsa_parse_curve(&curve, p_ctx->p_curve_info);
        if (PKCD_RET_OK == ret)
        {
            /* Clear async operation flag */
            r_pkcd_cmn_clr_async_ope();

            /* Clear PKCC RAM */
            r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

            /* Clear PKCC RAM */
            r_pkcd_cmn_ram_clr(p_buf, sizeof(st_eddsa_prm_derive_s_t));

            /* initialise params */
            r_pkcd_eddsa_set_len_info(curve.len);
            ret = r_pkcd_eddsa_precalc_params(&curve);

            /* Copy and encode parameters */
            r_pkcd_cmn_cpy_word(p_prm->scalar_s.scalar, p_scalar_s->p_data, p_scalar_s->len);
            p_prm->scalar_s.len = p_scalar_s->len;
            r_pkcd_eddsa_encode_scalar(&p_prm->scalar_s);

            r_pkcd_cmn_cpy_word(p_prm->scalar_r.scalar, p_scalar_r->p_data, p_scalar_r->len);
            p_prm->scalar_r.len = p_scalar_r->len;
            r_pkcd_eddsa_encode_scalar(&p_prm->scalar_r);

            r_pkcd_cmn_cpy_word(p_prm->hram.scalar, p_digest_r_a_m->p_data, p_digest_r_a_m->len);
            p_prm->hram.len = p_digest_r_a_m->len;
            r_pkcd_eddsa_encode_scalar(&p_prm->hram);

            /* calc scalar mod L */
            PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EDDSA_FORMAT_DP_DPT(PKCD_EDDSA_PDT_L);
            ret |= r_pkcd_cmn_reduce_small_modulus(p_prm->hram.scalar, PKCD_EDDSA_LWORK, PKCD_EDDSA_R2MODL,
                                                    p_prm->hram.len, PKCD_EDDSA_PBYTES, PKCD_EDDSA_WORKUNIT,
                                                    PKCD_EDDSA_WORK1, PKCD_EDDSA_WORK2, PKCD_EDDSA_WORK4);

            /* calculate H(R, A, M) * s */
            if (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == PKCD_EDDSA_PBYTES)
            {
                r_pkcd_cmn_cpy_by_64bounds(PKCD_EDDSA_WORK7 + PKCD_EDDSA_OFFSET_VAL, p_prm->scalar_s.scalar,
                                            PKCD_EDDSA_PBYTES);
            }
            else
            {
                r_pkcd_cmn_cpy_by_64bounds(PKCD_EDDSA_WORK7 + PKCD_EDDSA_OFFSET_VAL,
                                            &p_prm->scalar_s.scalar[PKCD_EDDSA_ZERO_PADDING_4],
                                            PKCD_EDDSA_PBYTES);
            }
            ret |= r_pkcd_cmn_ABRInvModC(PKCD_EDDSA_R2MODL, PKCD_EDDSA_WORK4, PKCD_EDDSA_LWORK, PKCD_EDDSA_WORK5,
                                            PKCD_EDDSA_WORKUNIT);
            ret |= r_pkcd_cmn_ABRInvModC(PKCD_EDDSA_WORK5, PKCD_EDDSA_WORK7, PKCD_EDDSA_LWORK, PKCD_EDDSA_WORK6,
                                            PKCD_EDDSA_WORKUNIT);

            /* calculate S = r + H(R, A, M) * s */
            r_pkcd_cmn_ram_clr(PKCD_EDDSA_WORK2, PKCD_EDDSA_WORKUNIT);
            r_pkcd_cmn_ram_clr(PKCD_EDDSA_WORK3, PKCD_EDDSA_WORKUNIT);
            ret |= r_pkcd_cmn_reduce_small_modulus(p_prm->scalar_r.scalar, PKCD_EDDSA_LWORK, PKCD_EDDSA_R2MODL,
                                                    p_prm->scalar_r.len, PKCD_EDDSA_PBYTES, PKCD_EDDSA_WORKUNIT,
                                                    PKCD_EDDSA_WORK1, PKCD_EDDSA_WORK2, PKCD_EDDSA_WORK3);
            ret |= r_pkcd_cmn_APlusBModC(PKCD_EDDSA_WORK3, PKCD_EDDSA_WORK6, PKCD_EDDSA_LWORK, PKCD_EDDSA_WORK2,
                                            PKCD_EDDSA_WORKUNIT);

            /* Clear buffer */
            r_pkcd_cmn_ram_clr(tmp_sign_s.scalar, sizeof(tmp_sign_s.scalar));

            if (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == PKCD_EDDSA_PBYTES)
            {
                r_pkcd_cmn_cpy_by_64bounds(tmp_sign_s.scalar, PKCD_EDDSA_WORK2 + PKCD_EDDSA_OFFSET_VAL,
                                            PKCD_EDDSA_PBYTES);
                tmp_sign_s.len = PKCD_EDDSA_ED25519_KEY_LEN;
            }
            else
            {
                r_pkcd_cmn_cpy_by_64bounds(&tmp_sign_s.scalar[PKCD_EDDSA_ZERO_PADDING_4],
                                            PKCD_EDDSA_WORK2 + PKCD_EDDSA_OFFSET_VAL,
                                            PKCD_EDDSA_PBYTES);
                tmp_sign_s.len = PKCD_EDDSA_ED448_RU4_KEY_LEN;
            }

            /* Encode signature s */
            r_pkcd_eddsa_encode_scalar(&tmp_sign_s);
            r_pkcd_cmn_cpy_word(p_sign_s->p_data, tmp_sign_s.scalar, tmp_sign_s.len);
            p_sign_s->len = tmp_sign_s.len;

            /* Extract the return value from the ORed result */
            ret = r_pkcd_cmn_extract_result(ret);

        }
        else
        {
            /* Do nothing */
        }

    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EddsaDeriveS()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaCompareSign
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EddsaCompareSign(const st_pkc_eddsa_ctx_t* const p_ctx,
                                    uint8_t* const p_buf,
                                    const st_pkc_data_t* const p_point_ha,
                                    const st_pkc_data_t* const p_point_sb,
                                    const st_pkc_data_t* const p_sign_r)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_eddsa_curve_t curve;
    st_eddsa_dpwks_t dpwks;
    st_eddsa_scalar_t comp_sb_minus_ha;
    int32_t cmp_ret;

    /* Check argument */
    ret = r_pkcd_eddsa_chk_compare_sign_arg(p_ctx, p_buf, p_point_ha, p_point_sb, p_sign_r);

    if (PKCD_RET_OK == ret)
    {
        ret = r_pkcd_eddsa_parse_curve(&curve, p_ctx->p_curve_info);

        if (PKCD_RET_OK == ret)
        {
            /* Clear async operation flag */
            r_pkcd_cmn_clr_async_ope();

            /* Clear PKCC RAM */
            r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

            /* initialize works structure */
            dpwks.dpwk1 = PKCD_EDDSA_FORMAT_DP_UNIT(PKCD_EDDSA_WORK1);
            dpwks.dpwk2 = PKCD_EDDSA_FORMAT_DP_UNIT(PKCD_EDDSA_WORK2);
            dpwks.dpwk3 = PKCD_EDDSA_FORMAT_DP_UNIT(PKCD_EDDSA_WORK3);
            dpwks.dpwk4 = PKCD_EDDSA_FORMAT_DP_UNIT(PKCD_EDDSA_WORK4);
            dpwks.dpwk5 = PKCD_EDDSA_FORMAT_DP_UNIT(PKCD_EDDSA_WORK5);
            dpwks.dpwk6 = PKCD_EDDSA_FORMAT_DP_UNIT(PKCD_EDDSA_WORK6);

            /* clear PKCC RAM */
            r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

            /* initialise params */
            r_pkcd_eddsa_set_len_info(curve.len);
            ret = r_pkcd_eddsa_precalc_params(&curve);

            /* Copy hA to work address */
            r_pkcd_cmn_cpy_by_64bounds(PKCD_EDDSA_PNTWORK1->point.x + PKCD_EDDSA_OFFSET_VAL,
                                        p_point_ha->p_data, curve.len);
            r_pkcd_cmn_cpy_by_64bounds(PKCD_EDDSA_PNTWORK1->point.y + PKCD_EDDSA_OFFSET_VAL,
                                        p_point_ha->p_data + curve.len, curve.len);
            r_pkcd_cmn_cpy_by_64bounds(PKCD_EDDSA_PNTWORK1->point.z + PKCD_EDDSA_OFFSET_VAL,
                                        p_point_ha->p_data + (curve.len * PKCD_EDDSA_DOUBLE_LEN), curve.len);

            /* Copy SB to work address */
            r_pkcd_cmn_cpy_by_64bounds(PKCD_EDDSA_PNTWORK2->point.x + PKCD_EDDSA_OFFSET_VAL,
                                        p_point_sb->p_data, curve.len);
            r_pkcd_cmn_cpy_by_64bounds(PKCD_EDDSA_PNTWORK2->point.y + PKCD_EDDSA_OFFSET_VAL,
                                        p_point_sb->p_data + curve.len, curve.len);
            r_pkcd_cmn_cpy_by_64bounds(PKCD_EDDSA_PNTWORK2->point.z + PKCD_EDDSA_OFFSET_VAL,
                                        p_point_sb->p_data + (curve.len * PKCD_EDDSA_DOUBLE_LEN), curve.len);

            /* calculate SB - hA */
            ret |= r_pkcd_cmn_ABRInvModC(PKCD_EDDSA_PWORK, PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, PKCD_EDDSA_WORK1,
                                            PKCD_EDDSA_WORKUNIT);
            ret |= r_pkcd_cmn_AMinusBModC(PKCD_EDDSA_WORK1, PKCD_EDDSA_PNTWORK1->point.x, PKCD_EDDSA_PWORK,
                                            PKCD_EDDSA_PNTWORK1->point.x, PKCD_EDDSA_WORKUNIT);
            ret |= r_pkcd_eddsa_point_add_inline(PKCD_EDDSA_PNTWORK1, PKCD_EDDSA_PNTWORK2, PKCD_EDDSA_PNTWORK3, &dpwks);

            /* compare compressed sign r and (SB - hA) */
            r_pkcd_cmn_ram_clr((uint8_t*)&comp_sb_minus_ha, sizeof(st_eddsa_scalar_t));
            ret |= r_pkcd_eddsa_point_compress(PKCD_EDDSA_PNTWORK3, comp_sb_minus_ha.scalar, PKCD_EDDSA_WORK1,
                                                PKCD_EDDSA_WORK2, PKCD_EDDSA_WORK3);

            /* Extract the return value from the ORed result */
            ret = r_pkcd_cmn_extract_result(ret);

            if (PKCD_RET_OK == ret)
            {
                /* Double check on verifications */
                ret = (pkcd_ret_t)0U;
                cmp_ret = r_pkcd_cmn_cmp_word(p_sign_r->p_data, comp_sb_minus_ha.scalar, PKCD_EDDSA_PBYTES);
                if (0 != cmp_ret)
                {
                    ret = PKCD_RET_EDDSA_VERIFAIL;
                }
                else
                {
                    /* Set some value of PKCD_RET_OK */
                    ret = PKCD_CMN_RET_OK_HALF_1;

                    cmp_ret = r_pkcd_cmn_cmp_word(p_sign_r->p_data, comp_sb_minus_ha.scalar, PKCD_EDDSA_PBYTES);

                    if (0 != cmp_ret)
                    {
                        ret = PKCD_RET_EDDSA_VERIFAIL;
                    }
                    else
                    {
                        /* Set another value of PKCD_RET_OK */
                        ret += PKCD_CMN_RET_OK_HALF_2;
                    }
                }
            }
            else
            {
                /* Do nothing */
            }

        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* Set an internal error when an invalid result is set */
    if (((pkcd_ret_t)0U == ret) || (PKCD_CMN_RET_OK_HALF_1 == ret) || (PKCD_CMN_RET_OK_HALF_2 == ret))
    {
        ret = PKCD_RET_TAMPERERR;
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EddsaCompareSign()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaMakeScalarSandR
**********************************************************************************************************************/
pkcd_ret_t  R_PKCD_EddsaMakeScalarSandR(const st_pkc_eddsa_ctx_t* const p_ctx,
                                        uint8_t* const p_buf,
                                        const void* const p_priv_key,
                                        const st_pkc_data_t* const p_msg,
                                        st_pkc_data_t* const p_scalar_s,
                                        st_pkc_data_t* const p_scalar_r)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_eddsa_scalar_t hashed_priv_key;
    const st_pkcd_eddsa_key_bin_t * p_key_bin;
    const uint8_t * p_ascii_data;
    uint32_t ascii_len;
    uint32_t key_len = 0U;

    /* Check argument */
    ret = r_pkcd_eddsa_chk_make_scalar_s_and_r_arg(p_ctx, p_buf, p_priv_key, p_msg, p_scalar_s, p_scalar_r);

    if (PKCD_RET_OK == ret)
    {
        p_key_bin  = (const st_pkcd_eddsa_key_bin_t*)p_priv_key;  /* No problem with casting to pointer size */
        r_pkcd_cmn_ram_clr(p_scalar_r->p_data, p_scalar_r->len);

        if (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len)
        {
            p_ascii_data = s_ed25519_ascii;
            ascii_len    = PKCD_EDDSA_ED25519_ASCII_SIZE;
            key_len      = PKCD_EDDSA_ED25519_KEY_LEN;
        }
        else
        {
            p_ascii_data = s_ed448_ascii;
            ascii_len    = PKCD_EDDSA_ED448_ASCII_SIZE;
            key_len      = PKCD_EDDSA_ED448_KEY_LEN;

            /* ed448 needs to specify the length with hash_final */
            hashed_priv_key.len = PKCD_EDDSA_SHAKE256BYTES;
            p_scalar_r->len     = PKCD_EDDSA_SHAKE256BYTES;
        }

        /* Make scalar s */
        ret = r_pkcd_eddsa_make_scalar_s(p_ctx, p_key_bin, p_ascii_data, ascii_len, key_len,
                                            &hashed_priv_key, p_scalar_s);

        if (PKCD_RET_OK == ret)
        {
            /* Make scalar r */
            ret = r_pkcd_eddsa_make_scalar_r(p_ctx, p_msg, p_ascii_data, ascii_len, key_len,
                                                &hashed_priv_key, p_scalar_r);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothoing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EddsaMakeScalarSandR()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaHashRAM
**********************************************************************************************************************/
pkcd_ret_t  R_PKCD_EddsaHashRAM(const st_pkc_eddsa_ctx_t* const p_ctx,
                                uint8_t* const p_buf,
                                const st_pkc_data_t* const p_sign_r,
                                const void* const p_pub_key,
                                const st_pkc_data_t* const p_msg,
                                st_pkc_data_t* const p_digest_r_a_m)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t hash_ret;
    void * p_hash_ctx;
    const st_pkcd_eddsa_key_bin_t * p_key_bin;
    uint8_t ph_flag;
    uint8_t ctx_len;
    const uint8_t * p_ascii_data;
    uint32_t ascii_len;
    uint32_t sign_r_len = 0U;
    uint32_t pubkey_len = 0U;

    /* Check argument */
    ret = r_pkcd_eddsa_chk_hash_r_a_m_arg(p_ctx, p_buf, p_sign_r, p_pub_key, p_msg, p_digest_r_a_m);
    if (PKCD_RET_OK == ret)
    {
        p_key_bin = (const st_pkcd_eddsa_key_bin_t*)p_pub_key;  /* No problem with casting to pointer size */
        p_hash_ctx = p_ctx->p_hash_info->p_hash_ctx;
        r_pkcd_cmn_ram_clr(p_digest_r_a_m->p_data, p_digest_r_a_m->len);

        if (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len)
        {
            p_ascii_data = s_ed25519_ascii;
            ascii_len = PKCD_EDDSA_ED25519_ASCII_SIZE;
            sign_r_len = PKCD_EDDSA_ED25519_KEY_LEN;
            pubkey_len = PKCD_EDDSA_ED25519_KEY_LEN;
        }
        else
        {
            p_ascii_data = s_ed448_ascii;
            ascii_len = PKCD_EDDSA_ED448_ASCII_SIZE;
            sign_r_len = PKCD_EDDSA_ED448_KEY_LEN;
            pubkey_len = PKCD_EDDSA_ED448_KEY_LEN;

            /* ed448 needs to specify the length with hash_final */
            p_digest_r_a_m->len = PKCD_EDDSA_SHAKE256BYTES;
        }

        /* Make scalar s */
        hash_ret = p_ctx->p_hash_info->p_hash_init_api(p_hash_ctx);
        if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
        {
            /* Additional hashing for Ed25519ctx and Ed25519ph, Hashing for Ed448 and Ed448ph */
            if (((PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len) &&
                (PKC_EDDSA_VARIANT_PURE != p_ctx->eddsa_variant)) ||
                (PKCD_EDDSA_CURVE_EDWARDS448_LEN == p_ctx->p_curve_info->curve_len))
            {
                /* get pre hash flag */
                ph_flag = (PKC_EDDSA_VARIANT_HASH == p_ctx->eddsa_variant) ? 1U : 0U;

                hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_hash_ctx, p_ascii_data, ascii_len);

                if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
                {
                    hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_hash_ctx,
                                                                    &ph_flag,
                                                                    sizeof(ph_flag));
                    if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
                    {
                        /* Since ctx_len is set to a value that fits in uint8_t,
                           there is no problem even if you cast it */
                        ctx_len = (uint8_t)p_ctx->p_ed_ctx->len;
                        hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_hash_ctx,
                                                                        &ctx_len,
                                                                        sizeof(ctx_len));
                        if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
                        {
                            if (0U != p_ctx->p_ed_ctx->len)
                            {
                                hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_hash_ctx,
                                                                                p_ctx->p_ed_ctx->p_data,
                                                                                p_ctx->p_ed_ctx->len);
                                if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
                                {
                                    /* Do nothing */
                                }
                                else
                                {
                                    ret = PKCD_RET_HASHAPIERR;
                                }
                            }
                            else
                            {
                                /* Do nothing */
                            }
                        }
                        else
                        {
                            ret = PKCD_RET_HASHAPIERR;
                        }
                    }
                    else
                    {
                        ret = PKCD_RET_HASHAPIERR;
                    }
                }
                else
                {
                    ret = PKCD_RET_HASHAPIERR;
                }
            }
            else
            {
                /* Do nothing */
            }

            if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
            {
                hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_hash_ctx, p_sign_r->p_data, sign_r_len);
                if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
                {
                    hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_hash_ctx, p_key_bin->p_public, pubkey_len);
                    if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
                    {
                        hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_hash_ctx, p_msg->p_data, p_msg->len);
                        if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
                        {
                            hash_ret = p_ctx->p_hash_info->p_hash_final_api(p_hash_ctx, p_digest_r_a_m->p_data,
                                                                            &p_digest_r_a_m->len);
                            if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
                            {
                                if (PKCD_EDDSA_CURVE_EDWARDS448_LEN == p_ctx->p_curve_info->curve_len)
                                {
                                    p_digest_r_a_m->len = PKCD_EDDSA_ED448_DIGEST_LEN;
                                }
                                else
                                {
                                    /* Do nothing */
                                }
                            }
                            else
                            {
                                ret = PKCD_RET_HASHAPIERR;
                            }
                        }
                        else
                        {
                            ret = PKCD_RET_HASHAPIERR;
                        }
                    }
                    else
                    {
                        ret = PKCD_RET_HASHAPIERR;
                    }
                }
                else
                {
                    ret = PKCD_RET_HASHAPIERR;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCD_RET_HASHAPIERR;
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EddsaHashRAM()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaKeyWrap
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EddsaKeyWrap(const st_pkc_eddsa_ctx_t* const p_ctx,
                                uint8_t* const p_buf,
                                const st_pkc_data_t* const p_priv_key_raw,
                                const st_pkc_data_t* const p_pub_key_raw,
                                void* const p_priv_key,
                                void* const p_pub_key)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_pkcd_eddsa_key_bin_t * p_priv_key_bin;
    st_pkcd_eddsa_key_bin_t * p_pub_key_bin;
    uint32_t out_key_len;

    ret = r_pkcd_eddsa_chk_key_wrap_arg(p_ctx, p_buf, p_priv_key_raw, p_pub_key_raw, p_priv_key, p_pub_key);
    if (PKCD_RET_OK == ret)
    {
        p_priv_key_bin = (st_pkcd_eddsa_key_bin_t*)p_priv_key; /* No problem with casting to pointer size */
        p_pub_key_bin  = (st_pkcd_eddsa_key_bin_t*)p_pub_key;  /* Same cast as above */

        out_key_len = (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len) ?
                        PKCD_EDDSA_ED25519_KEY_LEN : PKCD_EDDSA_ED448_RU4_KEY_LEN;

        /* zero clear */
        r_pkcd_cmn_ram_clr(p_priv_key_bin->p_private, out_key_len);
        r_pkcd_cmn_ram_clr(p_priv_key_bin->p_public, out_key_len);
        r_pkcd_cmn_ram_clr(p_pub_key_bin->p_public, out_key_len);

        /* Wrap private key */
        p_priv_key_bin->key_length = out_key_len;
        r_pkcd_cmn_cpy_word(p_priv_key_bin->p_private, p_priv_key_raw->p_data, out_key_len);
        r_pkcd_cmn_cpy_word(p_priv_key_bin->p_public, p_pub_key_raw->p_data, out_key_len);

        /* Wrap Public key */
        p_pub_key_bin->key_length = out_key_len;
        r_pkcd_cmn_cpy_word(p_pub_key_bin->p_public, p_pub_key_raw->p_data, out_key_len);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EddsaKeyWrap()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_precalc_params
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       Prepare pre-calculation values.
 * \param       [in] p_crv     Curve domain parameters.
 * \return
 *  - #PKCD_RET_OK:                       Successfully done.
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EDDSA_WWORK, #PKCD_EDDSA_PBYTES, #PKCD_EDDSA_OFFSET_VAL, #PKCD_EDDSA_PWORK, #PKCD_EDDSA_LWORK,
 *    #PKCD_EDDSA_WORK1, #PKCD_EDDSA_WORK2, #PKCD_EDDSA_R2MODL, #PKCD_EDDSA_PDT_L, #PKCD_EDDSA_R2MODP,
 *    #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_ARMODP, #PKCD_EDDSA_DRMODP
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPW
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EDDSA_OFFSET_VAL, #PKCD_EDDSA_PBYTES must have been prepared.
 * \par Detailed description
 *      - Prepare pre-calculation values.
 *
 * - Pseudo-code for the function can be found below.
 *   -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *   -# Set #PKCD_EDDSA_WWORK address as the top of PKCC DPW area.
 *   -# Set the size of field p as the PKCC DL and BDL value.
 *   -# Copy the field value p of the given curve into #PKCD_EDDSA_PWORK area.
 *   -# Copy the order value l of the given curve into #PKCD_EDDSA_LWORK area.
 *   -# Copy the coefficient1 value of the given curve into #PKCD_EDDSA_WORK1 area.
 *   -# Copy the coefficient2 value of the given curve into #PKCD_EDDSA_WORK2 area.
 *   -# Calculate {T over l} = -1/l mod 2^64 with #r_pkcd_cmn_MinusCInv.
 *   -# Calculate R^2 mod l with #r_pkcd_cmn_R2ModC.
 *   -# Calculate {T over p} = -1/p mod 2^64 with #r_pkcd_cmn_MinusCInv.
 *   -# Calculate R^2 mod p with #r_pkcd_cmn_R2ModC.
 *   -# Calculate aR mod p with #r_pkcd_cmn_ABRInvModC.
 *   -# Calculate dR mod p with #r_pkcd_cmn_ABRInvModC.
 *   -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_precalc_params(const st_eddsa_curve_t* const p_crv)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPW = PKCD_EDDSA_FORMAT_DP_DPW(PKCD_EDDSA_WWORK);
    r_pkcd_cmn_set_dl(PKCD_EDDSA_PBYTES, PKCD_EDDSA_PBYTES);

    /* copy p, l, a, d */
    r_pkcd_cmn_cpy_by_64bounds((PKCD_EDDSA_PWORK + PKCD_EDDSA_OFFSET_VAL), p_crv->p_field, PKCD_EDDSA_PBYTES);
    r_pkcd_cmn_cpy_by_64bounds((PKCD_EDDSA_LWORK + PKCD_EDDSA_OFFSET_VAL), p_crv->p_order, PKCD_EDDSA_PBYTES);
    r_pkcd_cmn_cpy_by_64bounds((PKCD_EDDSA_WORK1 + PKCD_EDDSA_OFFSET_VAL), p_crv->p_coeff1, PKCD_EDDSA_PBYTES);
    r_pkcd_cmn_cpy_by_64bounds((PKCD_EDDSA_WORK2 + PKCD_EDDSA_OFFSET_VAL), p_crv->p_coeff2, PKCD_EDDSA_PBYTES);

    /* calc T over l */
    ret = r_pkcd_cmn_MinusCInv(PKCD_EDDSA_LWORK, (uint8_t*)PKCD_EDDSA_FORMAT_DP_DPT(PKCD_EDDSA_PDT_L),
                                PKCD_EDDSA_WORKUNIT);

    /* set R^2 mod l */
    ret |= r_pkcd_cmn_R2ModC(PKCD_EDDSA_LWORK, PKCD_EDDSA_R2MODL, PKCD_EDDSA_WORKUNIT);

    /* calc T over p */
    ret |= r_pkcd_cmn_MinusCInv(PKCD_EDDSA_PWORK, (uint8_t*)PKCD_EDDSA_FORMAT_DP_DPT(PKCD_EDDSA_PDT_P),
                                PKCD_EDDSA_WORKUNIT);

    /* calc R^2 mod p */
    ret |= r_pkcd_cmn_R2ModC(PKCD_EDDSA_PWORK, PKCD_EDDSA_R2MODP, PKCD_EDDSA_WORKUNIT);

    /* calc aR mod p */
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EDDSA_WORK1, PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, PKCD_EDDSA_ARMODP,
                                    PKCD_EDDSA_WORKUNIT);

    /* calc dR mod p */
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EDDSA_WORK2, PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, PKCD_EDDSA_DRMODP,
                                    PKCD_EDDSA_WORKUNIT);

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_precalc_params()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_point_compress
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief     Compress a point (XR, YR, ZR) into octet string.
 * \param       [in]  p_p1          The address of a compressing point structure.
 * \param       [out] p_comp        The address where the compressed point is output.
 * \param       [in]  p_wk1         The address of a 64bytes work area in #PKCD_CMN_PKCCRAM.
 * \param       [in]  p_wk2         The address of a 64bytes work area in #PKCD_CMN_PKCCRAM.
 * \param       [in,out] p_wk3      The address of a 64bytes work area in #PKCD_CMN_PKCCRAM.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EDDSA_PWORK, #PKCD_EDDSA_OFFSET_VAL, #PKCD_EDDSA_PBYTES
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EDDSA_OFFSET_VAL, #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_PWORK, #PKCD_EDDSA_PBYTES must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit), #PKCD_CMN_ICUM_PKCCB0DPT and #PKCD_CMN_ICUM_PKCCB0DPW <br>
 *    value must have been set beforehand.
 * \par Detailed description
 *      - Compress a point (XR, YR, ZR) into octet string. <br>
 *        The compression of (x, y) coordinates is done following the method written in section 5.1.2 RFC8032.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Clear buffer data with #r_pkcd_cmn_ram_clr.
 * -# Remove a montgomery constant R from ZR using #r_pkcd_cmn_ARInvModC.
 * -# Calculate 1/Z mod p with the #r_pkcd_cmn_modinv_fermat.
 * -# Derive (x, y) unprojecting (X, Y, Z) coordinates.
 *   -# Calculate x = X/Z multiplying XR and 1/Z with the #r_pkcd_cmn_ABRInvModC.
 *   -# Calculate y = Y/Z multiplying XR and 1/Z with the #r_pkcd_cmn_ABRInvModC.
 * -# If (PKCD_EDDSA_PBYTES == PKCD_EDDSA_CURVE_EDWARDS25519_LEN)
 *   -# Copy the y-coordinate into a buffer.
 *   -# Clear the most significant bit of the y-coordinate in the buffer.
 *   -# Get the least significant bit of the x-coordinate.
 *   -# Or the most significant bit of the y-coordinate in the buffer with the
 *      least significant bit of the x-coordinate.
 *   -# Copy the buffer data into the output area indicated by p_comp converting its endianness into little endian.
 * -# Else
 *   -# Copy the y-coordinate into the buffer with 4byte padding.
 *   -# Get the least significant bit of the x-coordinate.
 *   -# OR the most significant bit of the y coordinate in the buffer with the
 *      least significant bit of the 3rd byte of the x coordinate.
 *   -# Copy the buffer data into the output area indicated by p_comp converting its endianness into little endian.
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_point_compress(const u_eddsa_work_t* const p_p1, uint8_t* const p_comp,
                                                const uint8_t* const p_wk1, const uint8_t* const p_wk2,
                                                uint8_t* const p_wk3)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t buf[PKCD_CMN_ARRAY32_SIZE(PKCD_EDDSA_KEY_LEN_MAX)];
    uint8_t * const p_buf = (uint8_t*)buf;         /* No problem with casting to pointer size */
    uint32_t  x_last[PKCD_CMN_ARRAY32_SIZE(PKCD_CMN_PKCCBOUNDARYSIZE)];
    uint8_t *  const p_x_last = (uint8_t*)x_last;  /* No problem with casting to pointer size */
    uint32_t i;

    /* Clear buffer */
     r_pkcd_cmn_ram_clr(p_buf, sizeof(buf));

    /* remove montgomery coefficient R from z-coordinate */
    ret = r_pkcd_cmn_ARInvModC(p_p1->point.z, PKCD_EDDSA_PWORK, p_wk1, PKCD_EDDSA_WORKUNIT);

    /* calc 1/Z */
    ret |= r_pkcd_cmn_modinv_fermat(p_wk1, PKCD_EDDSA_PWORK, p_wk3, p_wk2, PKCD_EDDSA_WORKUNIT);

    /* calc x = X/Z and y = Y/Z */
    ret |= r_pkcd_cmn_ABRInvModC(p_p1->point.x, p_wk3, PKCD_EDDSA_PWORK, p_wk1, PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_ABRInvModC(p_p1->point.y, p_wk3, PKCD_EDDSA_PWORK, p_wk2, PKCD_EDDSA_WORKUNIT);

    /* compress (x, y) */
    if (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == PKCD_EDDSA_PBYTES)
    {
        r_pkcd_cmn_cpy_by_64bounds(p_buf, p_wk2 + PKCD_EDDSA_OFFSET_VAL, PKCD_EDDSA_PBYTES);

        /* No problem for casting to support usual arithmetic conversion */
        p_buf[0] = (uint8_t)((uint32_t)p_buf[0] & (uint32_t)PKCD_EDDSA_CLEAR_MSB);
        r_pkcd_cmn_cpy_by_64bounds(p_x_last,
                                (p_wk1 + PKCD_EDDSA_WORKUNIT) - PKCD_CMN_PKCCBOUNDARYSIZE,
                                    PKCD_CMN_PKCCBOUNDARYSIZE);
        p_buf[0] = (uint8_t)((uint32_t)p_buf[0] |                               /* Same cast as above */
                            (uint32_t)(((uint32_t)p_x_last[PKCD_CMN_LSB_POS] &  /* Same cast as above */
                            (uint32_t)0x01U) << PKCD_EDDSA_SHIFT_7));           /* Same cast as above */
        for (i = 0U; i < PKCD_EDDSA_PBYTES; i++ )
        {
            p_comp[i] = p_buf[(PKCD_EDDSA_PBYTES - 1U) - i];
        }
    }
    else
    {
        r_pkcd_cmn_cpy_by_64bounds(&p_buf[PKCD_EDDSA_ZERO_PADDING_4],
                                    p_wk2 + PKCD_EDDSA_OFFSET_VAL, PKCD_EDDSA_PBYTES);

        r_pkcd_cmn_cpy_by_64bounds(p_x_last,
                                (p_wk1 + PKCD_EDDSA_WORKUNIT) - PKCD_CMN_PKCCBOUNDARYSIZE,
                                    PKCD_CMN_PKCCBOUNDARYSIZE);

        /* No problem for casting to support usual arithmetic conversion */
        p_buf[PKCD_EDDSA_ZERO_PADDING_3] = (uint8_t)((uint32_t)p_buf[PKCD_EDDSA_ZERO_PADDING_3] |
                                            (uint32_t)(((uint32_t)p_x_last[PKCD_CMN_LSB_POS] & /* Same cast as above */
                                            (uint32_t)0x01U) << PKCD_EDDSA_SHIFT_7));          /* Same cast as above */
        for (i = 0U; i < PKCD_EDDSA_ED448_RU4_KEY_LEN; i++ )
        {
            p_comp[i] = p_buf[(PKCD_EDDSA_ED448_RU4_KEY_LEN - 1U) - i];
        }
    }

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_point_compress()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ed25519_point_decompress
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       Decompress an octet string into a point (XR, YR, ZR).
 * \param       [in]  p_compressed          The address of a decompressing octet string.
 * \param       [in]  p_algo_opt1         The address of a precalculated (p + 3)/8 value.
 * \param       [in]  p_algo_opt2    The address of a precalculated (p - 1)/4 value.
 * \param       [out] p_p3                  The address of where the decompressed point (XR, YR, ZR) is output.
 * \param       [in]  p_works               The address of a work area structure which contains 6 work area addresses.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 *  - #PKCD_RET_EDDSA_DECOMPERR:     Failed in deriving square root of x^2 modulo p.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_OFFSET_VAL, #PKCD_EDDSA_R2MODP, #PKCD_EDDSA_PWORK, #PKCD_EDDSA_DRMODP
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 *
 * \par Precondition
 *  - #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_OFFSET_VAL, #PKCD_EDDSA_R2MODP, #PKCD_EDDSA_PWORK, #PKCD_EDDSA_DRMODP <br>
 *    must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit), #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *      - Decompress an octet string into a point (XR, YR, ZR). <br>
 *        The decompression of an octet string is done following the method written in section 5.1.3 RFC8032.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Initialize the variable ret2 with #PKCD_RET_INTERNALERR.
 * -# Copy the octet string into a buffer converting its endianness into big endian.
 * -# Get the most significant bit of the buffer data. The bit is the least significant bit of x-coordinate.
 * -# Clear the most significant bit of the buffer data. The remained bits are of y-coordinate's.
 * -# From here, derive the x-coordinate value. Where some of intermediate values are saved in montogomery format.
 * -# Calculate u = y^2 - 1 from y and #PKCD_EDDSA_R2MODP with #r_pkcd_cmn_ABRInvModC, #r_pkcd_cmn_ARInvModC,
 *    #r_pkcd_cmn_AMinusBModC.
 * -# Calculate v = dy^2 + 1 from y and #PKCD_EDDSA_DRMODP with #r_pkcd_cmn_ABRInvModC, #r_pkcd_cmn_APlusBModC.
 * -# Calculate u/v from u and v with #r_pkcd_cmn_ARInvModC, #r_pkcd_cmn_modinv_fermat, #r_pkcd_cmn_ABRInvModC.
 * -# Calculate x_candidate = (u/v)^((p+3)/8) from (u/V) and ((p+3)/8) with #r_pkcd_cmn_AExpBModC.
 * -# Calculate x_candidate^2 from (x candidate) and #PKCD_EDDSA_R2MODP with #r_pkcd_cmn_ABRInvModC,
 *    #r_pkcd_cmn_ARInvModC.
 * -# Calculate -(u/v) with #r_pkcd_cmn_AMinusBModC.
 * -# Correct the square root value if needed.<br>If (x candidate)^2 != (u/v) Then,
 *   -# If x_candidate^2 is not equal to -(u/v) Then,
 *     -# Decompression result in fail. And set ret2 = #PKCD_RET_EDDSA_DECOMPERR.
 *   -# Else Calculate x_candidate = x_candidate * 2^((p-1)/4) with #r_pkcd_cmn_ABRInvModC.
 * -# Recover x-coordinate value.
 *   -# If the saved LSBit of x equals to that of the x_candidate,
 *     -# x_candidate is the actual x value.
 *     -# Then caluculate XR with #r_pkcd_cmn_ABRInvModC.
 *   -# Else,
 *     -# (p - x_candidate) is the actual x value, So calculate (p - x_candidate) mod p with #r_pkcd_cmn_AMinusBModC.
 *     -# Then caluculate XR with #r_pkcd_cmn_ABRInvModC.
 * -# Calculate ZR from #PKCD_EDDSA_R2MODP with #r_pkcd_cmn_ARInvModC
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# If (#PKCD_RET_OK == ret) and (#PKCD_RET_EDDSA_DECOMPERR == ret2)
 *   -# Set ret to #PKCD_RET_EDDSA_DECOMPERR
 * -# Else
 *   -# Do nothing
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ed25519_point_decompress(const uint8_t* const p_compressed, const uint8_t* const p_algo_opt1,
                                                    const uint8_t* const p_algo_opt2, u_eddsa_work_t* const p_p3,
                                                    const st_eddsa_wks_t* const p_works)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    pkcd_ret_t ret2 = PKCD_RET_INTERNALERR;    /* for DECOMPERR */
    uint32_t buf[PKCD_CMN_ARRAY32_SIZE(PKCD_EDDSA_CURVE_EDWARDS25519_LEN)];
    uint8_t * const p_buf = (uint8_t*)buf;         /* No problem with casting to pointer size */
    uint32_t  x_last[PKCD_CMN_ARRAY32_SIZE(PKCD_CMN_PKCCBOUNDARYSIZE)];
    uint8_t *  const p_x_last = (uint8_t*)x_last;  /* No problem with casting to pointer size */
    uint32_t   x_bit;
    uint32_t i;
    int32_t cmp_ret;

    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EDDSA_FORMAT_DP_DPT(PKCD_EDDSA_PDT_P);
    r_pkcd_cmn_ram_clr((uint8_t*)p_p3, PKCD_EDDSA_WORKUNITPNT);  /* No problem with casting to pointer size */

    /* recover y-coordinate and calc YR */
    for (i = 0U; i < PKCD_EDDSA_CURVE_EDWARDS25519_LEN; i++ )
    {
        p_buf[i] = p_compressed[(PKCD_EDDSA_CURVE_EDWARDS25519_LEN - 1U) - i];
    }

    /* No problem for casting to support usual arithmetic conversion */
    x_bit = ((uint32_t)p_buf[0] & (uint32_t)PKCD_EDDSA_MASK_MSB) >> PKCD_EDDSA_SHIFT_7;
    p_buf[0] = (uint8_t)((uint32_t)p_buf[0] & (uint32_t)PKCD_EDDSA_CLEAR_MSB);  /* Same cast as above */
    r_pkcd_cmn_cpy_by_64bounds(p_works->p_wk1 + PKCD_EDDSA_OFFSET_VAL, p_buf, PKCD_EDDSA_CURVE_EDWARDS25519_LEN);
    ret = r_pkcd_cmn_ABRInvModC(p_works->p_wk1, PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_p3->point.y,
            PKCD_EDDSA_WORKUNIT);

    /* calculate u(at p_works->p_wk3) = y^2 - 1 */
    ret |= r_pkcd_cmn_ABRInvModC(p_p3->point.y, p_p3->point.y, PKCD_EDDSA_PWORK, p_works->p_wk1, PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_ARInvModC(PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_works->p_wk2, PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_AMinusBModC(p_works->p_wk1, p_works->p_wk2, PKCD_EDDSA_PWORK, p_works->p_wk3,
            PKCD_EDDSA_WORKUNIT);

    /* calculate v(at p_works->p_wk4) = dy^2 + 1 */
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk1, PKCD_EDDSA_DRMODP, PKCD_EDDSA_PWORK, p_works->p_wk5,
            PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_APlusBModC(p_works->p_wk5, p_works->p_wk2, PKCD_EDDSA_PWORK, p_works->p_wk4,
            PKCD_EDDSA_WORKUNIT);

    /* calculate u/v(at p_works->p_wk5) */
    ret |= r_pkcd_cmn_ARInvModC(p_works->p_wk4, PKCD_EDDSA_PWORK, p_works->p_wk1, PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_modinv_fermat(p_works->p_wk1, PKCD_EDDSA_PWORK, p_works->p_wk2, p_works->p_wk6,
            PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk3, p_works->p_wk2, PKCD_EDDSA_PWORK, p_works->p_wk5,
            PKCD_EDDSA_WORKUNIT);

    /* calculate x candidate(at p_works->p_wk6) = (u/v)^((p+3)/8) */
    r_pkcd_cmn_cpy_by_64bounds(p_works->p_wk1 + PKCD_EDDSA_OFFSET_VAL, p_algo_opt1,
                                PKCD_EDDSA_CURVE_EDWARDS25519_LEN);
    ret |= r_pkcd_cmn_AExpBModC(p_works->p_wk5, p_works->p_wk1, PKCD_EDDSA_PWORK, p_works->p_wk6, PKCD_EDDSA_WORKUNIT,
            PKC_SYNC_OPT_SYNC);

    /* calculate (x candidate)^2 (at p_works->p_wk3) */
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk6, PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_works->p_wk3,
            PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk3, p_works->p_wk3, PKCD_EDDSA_PWORK, p_works->p_wk4,
            PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_ARInvModC(p_works->p_wk4, PKCD_EDDSA_PWORK, p_works->p_wk3, PKCD_EDDSA_WORKUNIT);

    /* Check ret |= x candidate)^2 == u/v. If not, multiply 2^((p-1)/4) */
    ret |= r_pkcd_cmn_AMinusBModC(PKCD_EDDSA_PWORK, p_works->p_wk5, PKCD_EDDSA_PWORK, p_works->p_wk1,
            PKCD_EDDSA_WORKUNIT);
    cmp_ret = r_pkcd_cmn_cmp_word(p_works->p_wk3 + PKCD_EDDSA_OFFSET_VAL, p_works->p_wk5 + PKCD_EDDSA_OFFSET_VAL,
                                    PKCD_EDDSA_CURVE_EDWARDS25519_LEN);
    if (0 != cmp_ret)
    {
        cmp_ret = r_pkcd_cmn_cmp_word(p_works->p_wk3 + PKCD_EDDSA_OFFSET_VAL, p_works->p_wk1 + PKCD_EDDSA_OFFSET_VAL,
                    PKCD_EDDSA_CURVE_EDWARDS25519_LEN);
        if (0 == cmp_ret)
        {
            ret2 = PKCD_RET_OK;
        }
        else
        {
            ret2 = PKCD_RET_EDDSA_DECOMPERR;
        }

        ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk6, PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_works->p_wk1,
                                        PKCD_EDDSA_WORKUNIT);
        r_pkcd_cmn_cpy_by_64bounds(p_works->p_wk2 + PKCD_EDDSA_OFFSET_VAL, p_algo_opt2,
                                    PKCD_EDDSA_CURVE_EDWARDS25519_LEN);
        ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk1, p_works->p_wk2, PKCD_EDDSA_PWORK, p_works->p_wk6,
                                        PKCD_EDDSA_WORKUNIT);
    }
    else
    {
        ret2 = PKCD_RET_OK;
    }

    /* judge LSBit(x_candidate) == x_0 bit and recover x-coordinate */
    r_pkcd_cmn_cpy_by_64bounds(p_x_last, (p_works->p_wk6 + PKCD_EDDSA_WORKUNIT) - PKCD_CMN_PKCCBOUNDARYSIZE,
                                PKCD_CMN_PKCCBOUNDARYSIZE);

    /* No problem for casting to support usual arithmetic conversion */
    if (((uint32_t)p_x_last[PKCD_CMN_LSB_POS] & (uint32_t)0x01U) == x_bit)
    {
        /* x = x_candidate */
        ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk6, PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_p3->point.x,
                                        PKCD_EDDSA_WORKUNIT);
    }
    else
    {
        /* x = (p - x_candidate) */
        ret |= r_pkcd_cmn_AMinusBModC(PKCD_EDDSA_PWORK, p_works->p_wk6, PKCD_EDDSA_PWORK, p_works->p_wk1,
                                        PKCD_EDDSA_WORKUNIT);
        ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk1, PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_p3->point.x,
                                        PKCD_EDDSA_WORKUNIT);
    }

    /* calculate ZR */
    ret |= r_pkcd_cmn_ARInvModC(PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_p3->point.z, PKCD_EDDSA_WORKUNIT);

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    if ((PKCD_RET_OK == ret) && (PKCD_RET_EDDSA_DECOMPERR == ret2))
    {
        ret = PKCD_RET_EDDSA_DECOMPERR;
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ed25519_point_decompress()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ed448_point_decompress
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       Decompress an octet string into a point (XR, YR, ZR).
 * \param       [in]  p_compressed          The address of a decompressing octet string.
 * \param       [in]  p_algo_opt1           The address of a precalculated (p + 1)/4 value.
 * \param       [out] p_p3                  The address of where the decompressed point (XR, YR, ZR) is output.
 * \param       [in]  p_works               The address of a work area structure which contains 6 work area addresses.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 *  - #PKCD_RET_EDDSA_DECOMPERR:     Failed in deriving square root of x^2 modulo p.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_OFFSET_VAL, #PKCD_EDDSA_R2MODP, #PKCD_EDDSA_PWORK, #PKCD_EDDSA_DRMODP
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 *
 * \par Precondition
 *  - #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_OFFSET_VAL, #PKCD_EDDSA_R2MODP, #PKCD_EDDSA_PWORK, #PKCD_EDDSA_DRMODP <br>
 *    must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit), #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *      - Decompress an octet string into a point (XR, YR, ZR). <br>
 *        The decompression of an octet string is done following the method written in section 5.1.3 RFC8032.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Initialize the variable ret2 with #PKCD_RET_INTERNALERR.
 * -# Copy the octet string into a buffer converting its endianness into big endian.
 *    The first 3 bytes of data set zero padding.
 * -# Get the most significant bit of the buffer data. The bit is the least significant bit of x-coordinate.
 * -# From here, derive the x-coordinate value. Where some of intermediate values are saved in montogomery format.
 * -# Calculate u = y^2 - 1 from y and #PKCD_EDDSA_R2MODP with #r_pkcd_cmn_ABRInvModC, #r_pkcd_cmn_ARInvModC,
 *    #r_pkcd_cmn_AMinusBModC.
 * -# Calculate v = dy^2 - 1 from y and #PKCD_EDDSA_DRMODP with #r_pkcd_cmn_ABRInvModC, #r_pkcd_cmn_AMinusBModC.
 * -# Calculate u/v from u and v with #r_pkcd_cmn_ARInvModC, #r_pkcd_cmn_modinv_fermat, #r_pkcd_cmn_ABRInvModC.
 * -# Calculate x_candidate = (u/v)^((p+1)/4) from (u/v) and ((p+1)/4) with #r_pkcd_cmn_AExpBModC.
 * -# Calculate x_candidate^2 from (x candidate) and #PKCD_EDDSA_R2MODP with #r_pkcd_cmn_ABRInvModC,
 *    #r_pkcd_cmn_ARInvModC.
 * -# Calculate -(u/v) with #r_pkcd_cmn_AMinusBModC.
 * -# If (x candidate)^2 == (u/v)
 *   -# Set ret2 to #PKCD_RET_OK.
 * -# Else
 *   -# Set ret2 to #PKCD_RET_EDDSA_DECOMPERR.
 * -# Recover x-coordinate value.
 *   -# If the saved LSBit of x equals to that of the x_candidate,
 *     -# x_candidate is the actual x value.
 *     -# Then caluculate XR with #r_pkcd_cmn_ABRInvModC.
 *   -# Else,
 *     -# (p - x_candidate) is the actual x value, So calculate (p - x_candidate) mod p with #r_pkcd_cmn_AMinusBModC.
 *     -# Then caluculate XR with #r_pkcd_cmn_ABRInvModC.
 * -# Calculate ZR from #PKCD_EDDSA_R2MODP with #r_pkcd_cmn_ARInvModC
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# If (#PKCD_RET_OK == ret) and (#PKCD_RET_EDDSA_DECOMPERR == ret2)
 *   -# Set ret to #PKCD_RET_EDDSA_DECOMPERR
 * -# Else
 *   -# Do nothing
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ed448_point_decompress(const uint8_t* const p_compressed, const uint8_t* const p_algo_opt1,
                                                u_eddsa_work_t* const p_p3, const st_eddsa_wks_t* const p_works)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    pkcd_ret_t ret2 = PKCD_RET_INTERNALERR;    /* for DECOMPERR */
    uint32_t buf[PKCD_CMN_ARRAY32_SIZE(PKCD_EDDSA_ED448_RU4_KEY_LEN)];
    uint8_t * const p_buf = (uint8_t*)buf;         /* No problem with casting to pointer size */
    uint32_t  x_last[PKCD_CMN_ARRAY32_SIZE(PKCD_CMN_PKCCBOUNDARYSIZE)];
    uint8_t * const p_x_last = (uint8_t*)x_last;  /* No problem with casting to pointer size */
    uint32_t x_bit;
    uint32_t i;
    int32_t cmp_ret;

    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EDDSA_FORMAT_DP_DPT(PKCD_EDDSA_PDT_P);
    r_pkcd_cmn_ram_clr((uint8_t*)p_p3, PKCD_EDDSA_WORKUNITPNT);  /* No problem with casting to pointer size */

    /* recover y-coordinate and calc YR */
    for (i = 0U; i < PKCD_EDDSA_ED448_RU4_KEY_LEN; i++ )
    {
        p_buf[i] = p_compressed[(PKCD_EDDSA_ED448_RU4_KEY_LEN - 1U) - i];
    }

    /* No problem for casting to support usual arithmetic conversion */
    x_bit = ((uint32_t)p_buf[PKCD_EDDSA_ZERO_PADDING_3] & (uint32_t)PKCD_EDDSA_MASK_MSB) >> PKCD_EDDSA_SHIFT_7;
    r_pkcd_cmn_cpy_by_64bounds(p_works->p_wk1 + PKCD_EDDSA_OFFSET_VAL,
                                &p_buf[PKCD_EDDSA_ZERO_PADDING_4], PKCD_EDDSA_CURVE_EDWARDS448_LEN);
    ret = r_pkcd_cmn_ABRInvModC(p_works->p_wk1, PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_p3->point.y,
            PKCD_EDDSA_WORKUNIT);

    /* calculate u(at p_works->p_wk3) = y^2 - 1 */
    ret |= r_pkcd_cmn_ABRInvModC(p_p3->point.y, p_p3->point.y, PKCD_EDDSA_PWORK, p_works->p_wk1, PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_ARInvModC(PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_works->p_wk2, PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_AMinusBModC(p_works->p_wk1, p_works->p_wk2, PKCD_EDDSA_PWORK, p_works->p_wk3,
            PKCD_EDDSA_WORKUNIT);

    /* calculate v(at p_works->p_wk4) = dy^2 - 1 */
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk1, PKCD_EDDSA_DRMODP, PKCD_EDDSA_PWORK, p_works->p_wk5,
            PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_AMinusBModC(p_works->p_wk5, p_works->p_wk2, PKCD_EDDSA_PWORK, p_works->p_wk4,
            PKCD_EDDSA_WORKUNIT);

    /* calculate u/v(at p_works->p_wk5) */
    ret |= r_pkcd_cmn_ARInvModC(p_works->p_wk4, PKCD_EDDSA_PWORK, p_works->p_wk1, PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_modinv_fermat(p_works->p_wk1, PKCD_EDDSA_PWORK, p_works->p_wk2, p_works->p_wk6,
            PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk3, p_works->p_wk2, PKCD_EDDSA_PWORK, p_works->p_wk5,
            PKCD_EDDSA_WORKUNIT);

    /* calculate x candidate(at p_works->p_wk6) = (u/v)^((p+1)/4) */
    r_pkcd_cmn_cpy_by_64bounds(p_works->p_wk1 + PKCD_EDDSA_OFFSET_VAL, p_algo_opt1,
                                PKCD_EDDSA_CURVE_EDWARDS448_LEN);
    ret |= r_pkcd_cmn_AExpBModC(p_works->p_wk5, p_works->p_wk1, PKCD_EDDSA_PWORK, p_works->p_wk6, PKCD_EDDSA_WORKUNIT,
            PKC_SYNC_OPT_SYNC);

    /* calculate (x candidate)^2 (at p_works->p_wk3) */
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk6, PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_works->p_wk3,
            PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk3, p_works->p_wk3, PKCD_EDDSA_PWORK, p_works->p_wk4,
            PKCD_EDDSA_WORKUNIT);
    ret |= r_pkcd_cmn_ARInvModC(p_works->p_wk4, PKCD_EDDSA_PWORK, p_works->p_wk3, PKCD_EDDSA_WORKUNIT);

    /* Check ret |= x candidate)^2 == u/v. */
    cmp_ret = r_pkcd_cmn_cmp_word(p_works->p_wk3 + PKCD_EDDSA_OFFSET_VAL, p_works->p_wk5 + PKCD_EDDSA_OFFSET_VAL,
                                    PKCD_EDDSA_CURVE_EDWARDS448_LEN);
    if (0 == cmp_ret)
    {
        ret2 = PKCD_RET_OK;
    }
    else
    {
        ret2 = PKCD_RET_EDDSA_DECOMPERR;
    }

    /* judge LSBit(x_candidate) == x_0 bit and recover x-coordinate */
    r_pkcd_cmn_cpy_by_64bounds(p_x_last, (p_works->p_wk6 + PKCD_EDDSA_WORKUNIT) - PKCD_CMN_PKCCBOUNDARYSIZE,
                                PKCD_CMN_PKCCBOUNDARYSIZE);

    /* No problem for casting to support usual arithmetic conversion */
    if (((uint32_t)p_x_last[PKCD_CMN_LSB_POS] & (uint32_t)0x01U) == x_bit)
    {
        /* x = x_candidate */
        ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk6, PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_p3->point.x,
                                        PKCD_EDDSA_WORKUNIT);
    }
    else
    {
        /* x = (p - x_candidate) */
        ret |= r_pkcd_cmn_AMinusBModC(PKCD_EDDSA_PWORK, p_works->p_wk6, PKCD_EDDSA_PWORK, p_works->p_wk1,
                                        PKCD_EDDSA_WORKUNIT);
        ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk1, PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_p3->point.x,
                                        PKCD_EDDSA_WORKUNIT);
    }

    /* calculate ZR */
    ret |= r_pkcd_cmn_ARInvModC(PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_p3->point.z, PKCD_EDDSA_WORKUNIT);

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    if ((PKCD_RET_OK == ret) && (PKCD_RET_EDDSA_DECOMPERR == ret2))
    {
        ret = PKCD_RET_EDDSA_DECOMPERR;
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ed448_point_decompress()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_point_smult
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief     Operates point scalar multiplication on the projective coordinate Twisted Edwards Curve.
 * \param       [in]  p_p1              The address of an multiplying point P1(X1, Y1, Z1).
 * \param       [out] p_p3              The address of an multiplied point P3(X3, Y3, Z3).
 * \param       [in]  p_temp            The address of a 192byte work area for temporal value.
 * \param       [in]  p_neutral         The address of a 192byte work area for neutral element value.
 * \param       [in]  p_scalar          The address of a scalar value.
 * \param       [in]  p_works           The address of a work area structure which contains 6 work area addresses.
 * \param       [in]  processing_bits   The number of bits to be processed.
 * \param       [in,out]  p_executed_bits   The number of bits processed.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EDDSA_PBYTES, #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_R2MODP, #PKCD_EDDSA_PWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EDDSA_PBYTES, #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_R2MODP, #PKCD_EDDSA_PWORK must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit), #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *      - Operates scalar multiplication of the projective coordinate point on Twisted Edwards Curve.
 *      - The scalar multiplication is done using the binary method.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below. Some values are represented in Montgomery format.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If (PKCD_EDDSA_PBYTES == PKCD_EDDSA_CURVE_EDWARDS25519_LEN)
 *   -# Set all_bits to (#PKCD_EDDSA_PBYTES * #PKCD_CMN_BITSOFBYTE).
 * -# Else
 *   -# Set all_bits to (#PKCD_EDDSA_CURVE_EDWARDS448_LEN * #PKCD_CMN_BITSOFBYTE).
 * -# If p_executed_bits == 0 Then prepare neutral element (0, 1, 1) as follows,
 *   -# Set #PKCD_EDDSA_PDT_P as PKCC DPT area.
 *   -# Clear an x-coordinate value with #r_pkcd_cmn_ram_clr.
 *   -# Calculate R for a y-coordinate with #r_pkcd_cmn_ARInvModC.
 *   -# Copy R for a z-coordinate with #r_pkcd_cmn_cpy_by_64bounds.
 *   -# Copy neutral element value into temporary area also.
 * -# Set the neutral element as the adding point for the case of a target bit in binary method == 0.
 * -# Set the P1 as the adding point for the case of a target bit in binary method == 1.
 * -# Use the number at p_executed_bits as the starting index of the bit for the binary method.
 * -# Derive the last index of the bit for the binary method from processing bits.
 * -# For the bits between the starting index and the last index of the given scalar, do following
 *   -# Acquire the bit in the scalar which corresponds to the loop index.
 *   -# Double the point in the temporary area.
 *   -# Add P1 or the neutral element into the temporary value depending on if the bit is 1 or 0 respectively.
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# If the value of the return code variable is #PKCD_RET_OK Then,
 *   -# Update the number at p_executed_bits with resulting loop index.
 *   -# If the updated number is not equal to curve field size,
 *     -# Set #PKCD_RET_PROCESSING into the return code variable.
 *   -# Else,
 *     -# All the bits executed, so copy result into p_p3.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_point_smult(u_eddsa_work_t* const p_p1, u_eddsa_work_t* const p_p3,
                                            u_eddsa_work_t* const p_temp, u_eddsa_work_t* const p_neutral,
                                            const uint8_t* const p_scalar, const st_eddsa_wks_t* const p_works,
                                            const uint32_t processing_bits, uint32_t* const p_executed_bits)
{
    pkcd_ret_t ret = PKCD_RET_OK;
    u_eddsa_work_t * p_adding_list[PKCD_EDDSA_ADDINGLIST_NUM];
    st_eddsa_dpwks_t dpwks;
    uint32_t bits;
    uint32_t bit;
    uint32_t start_bit_pos;
    uint32_t end_bit_pos;
    uint32_t i;
    const uint32_t all_bits = PKCD_EDDSA_PBYTES * PKCD_CMN_BITSOFBYTE;

    dpwks.dpwk1 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk1);
    dpwks.dpwk2 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk2);
    dpwks.dpwk3 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk3);
    dpwks.dpwk4 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk4);
    dpwks.dpwk5 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk5);
    dpwks.dpwk6 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk6);

    if (0U == (*p_executed_bits))
    {
        /* prepare neutral element */
        PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EDDSA_FORMAT_DP_DPT(PKCD_EDDSA_PDT_P);
        r_pkcd_cmn_ram_clr(p_neutral->point.x, PKCD_EDDSA_WORKUNIT);
        ret = r_pkcd_cmn_ARInvModC(PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_neutral->point.y, PKCD_EDDSA_WORKUNIT);
        r_pkcd_cmn_cpy_by_64bounds(p_neutral->point.z, p_neutral->point.y, PKCD_EDDSA_WORKUNIT);

        /* copy neutral to temp */
        r_pkcd_cmn_cpy_by_64bounds((uint8_t*)p_temp, (uint8_t*)p_neutral, sizeof(u_eddsa_work_t));
    }
    else
    {
        /* Do nothing */
    }

    /* operate binary method */
    p_adding_list[0] = p_neutral;
    p_adding_list[1] = p_p1;

    /* Set start & end bit position */
    start_bit_pos = *p_executed_bits;      /* Since the bit position is 0 origin, subtract 1 from the remaining bits */
    end_bit_pos   = (((*p_executed_bits) + processing_bits) < all_bits) ?
                        ((*p_executed_bits) + processing_bits) : all_bits;

    for (i = start_bit_pos; i < end_bit_pos; i++ )
    {
        bits = p_scalar[i >> PKCD_CMN_SHIFT_3];

        /* No problem for casting to support usual arithmetic conversion */
        bit = (bits >> ((uint32_t)(PKCD_CMN_BITSOFBYTE - 1U) -
                (i & (uint32_t)PKCD_EDDSA_MASK_07))) & (uint32_t)0x01U;
        ret |= r_pkcd_eddsa_point_dbl_inline(p_temp, p_p3, &dpwks);

        ret |= r_pkcd_eddsa_point_add_inline(p_p3, p_adding_list[bit], p_temp, &dpwks);
    }

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    if (PKCD_RET_OK == ret)
    {
        /* Update remaining bits */
        *p_executed_bits = end_bit_pos;

        if ((*p_executed_bits) != all_bits)
        {
            ret = PKCD_RET_PROCESSING;
        }
        else
        {
            /* all bits execute */
            r_pkcd_cmn_cpy_by_64bounds((uint8_t*)p_p3, (uint8_t*)p_temp, sizeof(u_eddsa_work_t));
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_point_smult()
**********************************************************************************************************************/

#if (PKCD_CFG_USE_ED25519_PRECALC_TABLE_16 == 1U) || (PKCD_CFG_USE_ED25519_PRECALC_TABLE_256 == 1U)
/**********************************************************************************************************************
* Function Name : r_pkcd_ed25519_smult_base
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       Operates base point scalar multiplication on the projective coordinate Twisted Edwards Curve.
 * \param       [out] p_p3              The address of an multiplied point P3(X3, Y3, Z3).
 * \param       [in]  p_temp            The address of a 192byte work area for temporal value.
 * \param       [in]  p_temp2           The address of a 192byte work area for temporal value.
 * \param       [in]  p_scalar          The address of a scalar value.
 * \param       [in]  p_works           The address of a work area structure which contains 6 work area addresses.
 * \param       [in]  processing_bits   The number of bits to be processed.
 * \param       [in,out]  p_executed_bits   The number of bits processed.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_R2MODP, #PKCD_EDDSA_PWORK <br>
 *    (If #PKCD_CFG_USE_ED25519_PRECALC_TABLE_16 == 1 then)  g_ed25519_comb_table_16 <br>
 *    (If #PKCD_CFG_USE_ED25519_PRECALC_TABLE_256 == 1 then) g_ed25519_comb_table_256
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_R2MODP, #PKCD_EDDSA_PWORK must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit), #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *      - Operates scalar multiplication of the projective coordinate base point on Twisted Edwards Curve.
 *      - The scalar multiplication is done using the binary method.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below. Some values are represented in Montgomery format.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If #PKCD_CFG_USE_ED25519_PRECALC_TABLE_16 is declared, the loop of the window method is done
 *      {curve25519 bit size}/4 times .
 *    And if #PKCD_CFG_USE_ED25519_PRECALC_TABLE_256 is declared, the loop of the window method is done
 *      {curve25519 bit size}/16 times.
 * -# If p_executed_bits == 0 Then prepare neutral element (0, 1, 1) for temporary value as follows,
 *   -# Set #PKCD_EDDSA_PDT_P as PKCC DPT area.
 *   -# Clear an x-coordinate value with #r_pkcd_cmn_ram_clr.
 *   -# Calculate R for a y-coordinate with #r_pkcd_cmn_ARInvModC.
 *   -# Copy R for a z-coordinate with #r_pkcd_cmn_cpy_by_64bounds.
 * -# Set the neutral element as the adding point for the case of a target bit in binary method == 0.
 * -# Set the P1 as the adding point for the case of a target bit in binary method == 1.
 * -# Use the number at p_executed_bits as the starting index of the bit for the binary method.
 * -# Derive the last index of the bit for the binary method from processing bits.
 * -# For the bits between the starting index and the last index of the given scalar, do following
 *   -# Assume the scalar value separated into 4 or 16 parts, and acquire the each bit at the loop index
 *      of the separated parts.
 *      For example if scalar value is separated into 4 parts, bit[192+idx], bit[128+idx], bit[64+idx] and
 *      bit[0+idx] are acquired.
 *      And in this case, 2^(192+idx)+2^(128+idx)+2^(64+idx)+2^(0+idx) is used for the window value.
 *      All the scalar multiplication result with base point and available window values
 *      are prepared as g_ed25519_comb_table_16 and g_ed25519_comb_table_256.
 *   -# Double the point in the temporary area.
 *   -# Add an {base point} x {window value} result to the point in the temporary area.
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# If the value of the return code variable is #PKCD_RET_OK Then,
 *   -# Update the number at p_executed_bits with resulting loop index.
 *   -# If the updated number is not equal to curve field size,
 *     -# Set #PKCD_RET_PROCESSING into the return code variable.
 *   -# Else,
 *     -# All the bits executed, so copy result into p_p3.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ed25519_smult_base(u_eddsa_work_t* const p_p3, u_eddsa_work_t* const p_temp,
                                            u_eddsa_work_t* const p_temp2, const uint8_t* const p_scalar,
                                            const st_eddsa_wks_t* const p_works, const uint32_t processing_bits,
                                            uint32_t* const p_executed_bits)
{
    pkcd_ret_t ret = PKCD_RET_OK;
    st_eddsa_dpwks_t dpwks;
    uint32_t window;
    uint32_t start_bit_pos;
    uint32_t end_bit_pos;
    uint32_t i;
    uint32_t bits;
    uint32_t bit;
#if (PKCD_CFG_USE_ED25519_PRECALC_TABLE_16 == 1U)
    const uint32_t all_bits = (PKCD_EDDSA_CURVE_EDWARDS25519_LEN * PKCD_CMN_BITSOFBYTE) / PKCD_EDDSA_COMB_4_BITS;
    const st_ed25519_base_comb_tbl_t * const p_comb_tbl = g_ed25519_comb_table_16;
#elif (PKCD_CFG_USE_ED25519_PRECALC_TABLE_256 == 1U)
    const uint32_t all_bits = (PKCD_EDDSA_CURVE_EDWARDS25519_LEN * PKCD_CMN_BITSOFBYTE) / PKCD_EDDSA_COMB_8_BITS;
    const st_ed25519_base_comb_tbl_t* const p_comb_tbl = g_ed25519_comb_table_256;
#endif

    dpwks.dpwk1 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk1);
    dpwks.dpwk2 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk2);
    dpwks.dpwk3 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk3);
    dpwks.dpwk4 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk4);
    dpwks.dpwk5 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk5);
    dpwks.dpwk6 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk6);

    if (0U == (*p_executed_bits))
    {
        /* prepare neutral element */
        PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EDDSA_FORMAT_DP_DPT(PKCD_EDDSA_PDT_P);
        r_pkcd_cmn_ram_clr(p_temp->point.x, PKCD_EDDSA_WORKUNIT);
        ret = r_pkcd_cmn_ARInvModC(PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_temp->point.y, PKCD_EDDSA_WORKUNIT);
        r_pkcd_cmn_cpy_by_64bounds(p_temp->point.z, p_temp->point.y, PKCD_EDDSA_WORKUNIT);
    }
    else
    {
        /* Do nothing */
    }

    /* Set start & end bit position */
    start_bit_pos = *p_executed_bits;      /* Since the bit position is 0 origin, subtract 1 from the remaining bits */
    end_bit_pos   = (((*p_executed_bits) + processing_bits) < all_bits) ?
                        ((*p_executed_bits) + processing_bits) : all_bits;

    /* operate comb method */
    for (i = start_bit_pos; i < end_bit_pos; i++ )
    {
        bits = i / PKCD_CMN_BITSOFBYTE;
        bit =  (PKCD_CMN_BITSOFBYTE - (i % PKCD_CMN_BITSOFBYTE)) - 1U;
#if (PKCD_CFG_USE_ED25519_PRECALC_TABLE_16 == 1U)
        /* No problem for casting to support usual arithmetic conversion */
        window =  ((uint32_t)PKCD_EDDSA_BIT_08 * ((uint32_t)0x01U & ((uint32_t)p_scalar[bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_04 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[(uint32_t)PKCD_EDDSA_BITS_POS_08 + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_02 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[(uint32_t)PKCD_EDDSA_BITS_POS_10 + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_01 & ((uint32_t)p_scalar[(uint32_t)PKCD_EDDSA_BITS_POS_18 + bits] >> bit));
#elif (PKCD_CFG_USE_ED25519_PRECALC_TABLE_256 == 1U)
        /* No problem for casting to support usual arithmetic conversion */
        window =  ((uint32_t)PKCD_EDDSA_BIT_80 * ((uint32_t)0x01U & ((uint32_t)p_scalar[bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_40 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_04 + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_20 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_08 + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_10 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_0C + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_08 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_10 + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_04 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_14 + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_02 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_18 + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_01 & ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_1C + bits] >> bit));
#endif
        ret |= r_pkcd_eddsa_point_dbl_inline(p_temp, p_p3, &dpwks);
        r_pkcd_cmn_cpy_word(p_temp2->point.x + PKCD_EDDSA_OFFSET_VAL,
                            /* There is no problem casting with the argument of the copy function */
                            (const uint8_t*)p_comb_tbl[window].xr,
                            PKCD_EDDSA_CURVE_EDWARDS25519_LEN);
        r_pkcd_cmn_cpy_word(p_temp2->point.y + PKCD_EDDSA_OFFSET_VAL,
                            (const uint8_t*)p_comb_tbl[window].yr, /* Same cast as above */
                            PKCD_EDDSA_CURVE_EDWARDS25519_LEN);
        r_pkcd_cmn_cpy_word(p_temp2->point.z + PKCD_EDDSA_OFFSET_VAL,
                            (const uint8_t*)p_comb_tbl[window].zr, /* Same cast as above */
                            PKCD_EDDSA_CURVE_EDWARDS25519_LEN);
        ret |= r_pkcd_eddsa_point_add_inline(p_p3, p_temp2, p_temp, &dpwks);
    }

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    if (PKCD_RET_OK == ret)
    {
        /* Update remaining bits */
        *p_executed_bits = end_bit_pos;

        if ((*p_executed_bits) != all_bits)
        {
            ret = PKCD_RET_PROCESSING;
        }
        else
        {
            /* all bits execute */
            r_pkcd_cmn_cpy_by_64bounds((uint8_t*)p_p3, (uint8_t*)p_temp, sizeof(u_eddsa_work_t));
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ed25519_smult_base()
**********************************************************************************************************************/
#endif /* (PKCD_CFG_USE_ED25519_PRECALC_TABLE_16 == 1) || (PKCD_CFG_USE_ED25519_PRECALC_TABLE_256 == 1) */

#if (PKCD_CFG_USE_ED448_PRECALC_TABLE_16 == 1U) || (PKCD_CFG_USE_ED448_PRECALC_TABLE_256 == 1U)
/**********************************************************************************************************************
* Function Name : r_pkcd_ed448_smult_base
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       Operates base point scalar multiplication on the projective coordinate Twisted Edwards Curve.
 * \param       [out] p_p3              The address of an multiplied point P3(X3, Y3, Z3).
 * \param       [in]  p_temp            The address of a 192byte work area for temporal value.
 * \param       [in]  p_temp2           The address of a 192byte work area for temporal value.
 * \param       [in]  p_scalar          The address of a scalar value.
 * \param       [in]  p_works           The address of a work area structure which contains 6 work area addresses.
 * \param       [in]  processing_bits   The number of bits to be processed.
 * \param       [in,out]  p_executed_bits   The number of bits processed.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_R2MODP, #PKCD_EDDSA_PWORK <br>
 *    (If #PKCD_CFG_USE_ED448_PRECALC_TABLE_16 == 1 then)  g_ed448_comb_table_16 <br>
 *    (If #PKCD_CFG_USE_ED448_PRECALC_TABLE_256 == 1 then) g_ed448_comb_table_256
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_R2MODP, #PKCD_EDDSA_PWORK must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit), #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *      - Operates scalar multiplication of the projective coordinate base point on Twisted Edwards Curve.
 *      - The scalar multiplication is done using the binary method.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below. Some values are represented in Montgomery format.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If #PKCD_CFG_USE_ED448_PRECALC_TABLE_16 is declared, the loop of the window method is done
 *      {curve448 bit size}/4 times .
 *    And if #PKCD_CFG_USE_ED448_PRECALC_TABLE_256 is declared, the loop of the window method is done
 *      {curve448 bit size}/16 times.
 * -# If p_executed_bits == 0 Then prepare neutral element (0, 1, 1) for temporary value as follows,
 *   -# Set #PKCD_EDDSA_PDT_P as PKCC DPT area.
 *   -# Clear an x-coordinate value with #r_pkcd_cmn_ram_clr.
 *   -# Calculate R for a y-coordinate with #r_pkcd_cmn_ARInvModC.
 *   -# Copy R for a z-coordinate with #r_pkcd_cmn_cpy_by_64bounds.
 * -# Set the neutral element as the adding point for the case of a target bit in binary method == 0.
 * -# Set the P1 as the adding point for the case of a target bit in binary method == 1.
 * -# Use the number at p_executed_bits as the starting index of the bit for the binary method.
 * -# Derive the last index of the bit for the binary method from processing bits.
 * -# For the bits between the starting index and the last index of the given scalar, do following
 *   -# Assume the scalar value separated into 4 or 16 parts, and acquire the each bit at the loop index
 *      of the separated parts.
 *      For example if scalar value is separated into 4 parts, bit[192+idx], bit[128+idx], bit[64+idx] and
 *      bit[0+idx] are acquired.
 *      And in this case, 2^(192+idx)+2^(128+idx)+2^(64+idx)+2^(0+idx) is used for the window value.
 *      All the scalar multiplication result with base point and available window values
 *      are prepared as g_ed448_comb_table_16 and g_ed448_comb_table_256.
 *   -# Double the point in the temporary area.
 *   -# Add an {base point} x {window value} result to the point in the temporary area.
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# If the value of the return code variable is #PKCD_RET_OK Then,
 *   -# Update the number at p_executed_bits with resulting loop index.
 *   -# If the updated number is not equal to curve field size,
 *     -# Set #PKCD_RET_PROCESSING into the return code variable.
 *   -# Else,
 *     -# All the bits executed, so copy result into p_p3.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ed448_smult_base(u_eddsa_work_t* const p_p3, u_eddsa_work_t* const p_temp,
                                            u_eddsa_work_t* const p_temp2, const uint8_t* const p_scalar,
                                            const st_eddsa_wks_t* const p_works, const uint32_t processing_bits,
                                            uint32_t* const p_executed_bits)
{
    pkcd_ret_t ret = PKCD_RET_OK;
    st_eddsa_dpwks_t dpwks;
    uint32_t window;
    uint32_t start_bit_pos;
    uint32_t end_bit_pos;
    uint32_t i;
    uint32_t bits;
    uint32_t bit;
    const uint32_t tbl_offset = PKCD_EDDSA_WORKUNIT - PKCD_EDDSA_CURVE_EDWARDS448_LEN;
#if (PKCD_CFG_USE_ED448_PRECALC_TABLE_16 == 1U)
    const uint32_t all_bits = (PKCD_EDDSA_CURVE_EDWARDS448_LEN * PKCD_CMN_BITSOFBYTE) / PKCD_EDDSA_COMB_4_BITS;
    const st_ed448_base_comb_tbl_t * const p_comb_tbl = g_ed448_comb_table_16;
#elif (PKCD_CFG_USE_ED448_PRECALC_TABLE_256 == 1U)
    const uint32_t all_bits = (PKCD_EDDSA_CURVE_EDWARDS448_LEN * PKCD_CMN_BITSOFBYTE) / PKCD_EDDSA_COMB_8_BITS;
    const st_ed448_base_comb_tbl_t* const p_comb_tbl = g_ed448_comb_table_256;
#endif

    dpwks.dpwk1 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk1);
    dpwks.dpwk2 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk2);
    dpwks.dpwk3 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk3);
    dpwks.dpwk4 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk4);
    dpwks.dpwk5 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk5);
    dpwks.dpwk6 = PKCD_EDDSA_FORMAT_DP_UNIT(p_works->p_wk6);

    if (0U == (*p_executed_bits))
    {
        /* prepare neutral element */
        PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EDDSA_FORMAT_DP_DPT(PKCD_EDDSA_PDT_P);
        r_pkcd_cmn_ram_clr(p_temp->point.x, PKCD_EDDSA_WORKUNIT);
        ret = r_pkcd_cmn_ARInvModC(PKCD_EDDSA_R2MODP, PKCD_EDDSA_PWORK, p_temp->point.y, PKCD_EDDSA_WORKUNIT);
        r_pkcd_cmn_cpy_by_64bounds(p_temp->point.z, p_temp->point.y, PKCD_EDDSA_WORKUNIT);
    }
    else
    {
        /* Do nothing */
    }

    /* Set start & end bit position */
    start_bit_pos = *p_executed_bits;      /* Since the bit position is 0 origin, subtract 1 from the remaining bits */
    end_bit_pos   = (((*p_executed_bits) + processing_bits) < all_bits) ?
                        ((*p_executed_bits) + processing_bits) : all_bits;

    /* operate comb method */
    for (i = start_bit_pos; i < end_bit_pos; i++ )
    {
        bits = i / PKCD_CMN_BITSOFBYTE;
        bit =  (PKCD_CMN_BITSOFBYTE - (i % PKCD_CMN_BITSOFBYTE)) - 1U;
#if (PKCD_CFG_USE_ED448_PRECALC_TABLE_16 == 1U)
        /* No problem for casting to support usual arithmetic conversion */
        window =  ((uint32_t)PKCD_EDDSA_BIT_08 * ((uint32_t)0x01U & ((uint32_t)p_scalar[bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_04 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[(uint32_t)PKCD_EDDSA_BITS_POS_0E + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_02 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[(uint32_t)PKCD_EDDSA_BITS_POS_1C + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_01 & ((uint32_t)p_scalar[(uint32_t)PKCD_EDDSA_BITS_POS_2A + bits] >> bit));
#elif (PKCD_CFG_USE_ED448_PRECALC_TABLE_256 == 1U)
        /* No problem for casting to support usual arithmetic conversion */
        window =  ((uint32_t)PKCD_EDDSA_BIT_80 * ((uint32_t)0x01U & ((uint32_t)p_scalar[bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_40 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_07 + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_20 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_0E + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_10 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_15 + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_08 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_1C + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_04 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_23 + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_02 * ((uint32_t)0x01U &
                    ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_2A + bits] >> bit)));
        window += ((uint32_t)PKCD_EDDSA_BIT_01 & ((uint32_t)p_scalar[PKCD_EDDSA_BITS_POS_31 + bits] >> bit));
#endif
        ret |= r_pkcd_eddsa_point_dbl_inline(p_temp, p_p3, &dpwks);
        r_pkcd_cmn_cpy_word(p_temp2->point.x + tbl_offset,
                            /* There is no problem casting with the argument of the copy function */
                            (const uint8_t*)p_comb_tbl[window].xr,
                            PKCD_EDDSA_CURVE_EDWARDS448_LEN);
        r_pkcd_cmn_cpy_word(p_temp2->point.y + tbl_offset,
                            (const uint8_t*)p_comb_tbl[window].yr, /* Same cast as above */
                            PKCD_EDDSA_CURVE_EDWARDS448_LEN);
        r_pkcd_cmn_cpy_word(p_temp2->point.z + tbl_offset,
                            (const uint8_t*)p_comb_tbl[window].zr, /* Same cast as above */
                            PKCD_EDDSA_CURVE_EDWARDS448_LEN);
        ret |= r_pkcd_eddsa_point_add_inline(p_p3, p_temp2, p_temp, &dpwks);
    }

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    if (PKCD_RET_OK == ret)
    {
        /* Update remaining bits */
        *p_executed_bits = end_bit_pos;

        if ((*p_executed_bits) != all_bits)
        {
            ret = PKCD_RET_PROCESSING;
        }
        else
        {
            /* all bits execute */
            r_pkcd_cmn_cpy_by_64bounds((uint8_t*)p_p3, (uint8_t*)p_temp, sizeof(u_eddsa_work_t));
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ed448_smult_base()
**********************************************************************************************************************/
#endif /* (PKCD_CFG_USE_ED448_PRECALC_TABLE_16 == 1) || (PKCD_CFG_USE_ED448_PRECALC_TABLE_256 == 1) */

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_point_add_inline
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       Operates point addition on the projective coordinate Twisted Edwards Curve.
 * \param       [in]  p_p1      The address of an adding point P1(X1, Y1, Z1).
 * \param       [in]  p_p2      The address of an adding point P2(X2, Y2, Z2).
 * \param       [out] p_p3      The address of an added point P3(X3, Y3, Z3).
 * \param       [in]  p_dpwks   The address of a work area structure which contains 6 work area addresses in DP format.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_PWORK, #PKCD_EDDSA_DRMODP, #PKCD_EDDSA_ARMODP
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPC
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_PWORK, #PKCD_EDDSA_DRMODP, #PKCD_EDDSA_ARMODP must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit), #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *      - Operates point addition on the projective coordinate Twisted Edwards Curve.
 *      - The addition is done following the method written in section 6 of  <br>
 *        D.J.Bernstein, P.Birkner, M.Joye, T.Lange and C.Peters "Twisted Edwards Curves".
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set #PKCD_EDDSA_PDT_P and #PKCD_EDDSA_PWORK area as PKCC DPT and DPC area respectively.
 * -# Calculate A = Z1 * Z2 using PKCC S=ABR^(-1) mod C mode.
 * -# Calculate B = A * A using PKCC S=ABR^(-1) mod C mode.
 * -# Calculate C = X1 * X2 using PKCC S=ABR^(-1) mod C mode.
 * -# Calculate D = Y1 * Y2 using PKCC S=ABR^(-1) mod C mode.
 * -# Calculate E = d * C * D using PKCC S=ABR^(-1) mod C mode.
 * -# Calculate F = B - E using PKCC S=(A-B) mod C mode.
 * -# Calculate G = B + E using PKCC S=(A+B) mod C mode.
 * -# Calculate Z3 = F * G using PKCC S=ABR^(-1) mod C mode.
 * -# Calculate Y3 = A * G * (D - aC) using PKCC S=ABR^(-1) mod C and S=(A-B) mod C mode.
 * -# Calculate X3 = A * F * ((X1+Y1) * (X2+Y2) - C - D) using PKCC S=ABR^(-1) mod C, S=(A+B) mod C and
 *  S=(A-B) mod C mode.
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_point_add_inline(const u_eddsa_work_t* const p_p1, const u_eddsa_work_t* const p_p2,
                                                const u_eddsa_work_t* const p_p3, const st_eddsa_dpwks_t* const p_dpwks)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EDDSA_FORMAT_DP_DPT(PKCD_EDDSA_PDT_P);
    PKCD_CMN_ICUM_PKCCB0DPC = PKCD_EDDSA_FORMAT_DP_UNIT(PKCD_EDDSA_PWORK);

    /* calc A(at wk1) = Z1*Z2 */
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(p_p1->point.z);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EDDSA_FORMAT_DP_UNIT(p_p2->point.z);
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk1;
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc B(at p_p3.z) = A^2 */
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk1;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk1;
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.z);
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc C(at wk3) = X1*X2 */
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(p_p1->point.x);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EDDSA_FORMAT_DP_UNIT(p_p2->point.x);
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk3;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc D(at wk4) = Y1*Y2 */
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(p_p1->point.y);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EDDSA_FORMAT_DP_UNIT(p_p2->point.y);
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk4;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc E(at wk2) = dC*D */
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(PKCD_EDDSA_DRMODP);
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk3;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk5;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk5;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk4;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk2;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc F(at wk5) = B-E */
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.z);
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk2;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk5;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_AMINUSBMODC, PKC_SYNC_OPT_SYNC);

    /* calc G(at wk6) = B+E */
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.z);
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk2;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk6;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_APLUSBMODC, PKC_SYNC_OPT_SYNC);

    /* calc Z3(at p_p3.z) = F*G */
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk5;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk6;
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.z);
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc Y3(at p_p3.y) = A*G*(D-aC) */
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk1;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk6;
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.x);
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(PKCD_EDDSA_ARMODP);
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk3;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk2;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk4;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk2;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk6;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_AMINUSBMODC, PKC_SYNC_OPT_SYNC);
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.x);
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk6;
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.y);
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc X3(at p_p3.x) = A*F*((X1+Y1)*(X2+Y2)-C-D) */
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk1;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk5;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk2;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);    //A*F
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(p_p1->point.x);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EDDSA_FORMAT_DP_UNIT(p_p1->point.y);
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk1;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_APLUSBMODC, PKC_SYNC_OPT_SYNC);    //X1+Y1
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(p_p2->point.x);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EDDSA_FORMAT_DP_UNIT(p_p2->point.y);
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk5;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_APLUSBMODC, PKC_SYNC_OPT_SYNC);    //X2+Y2
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk1;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk5;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk6;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);    //(X1+Y1)*(X2+Y2)
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk6;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk3;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk1;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_AMINUSBMODC, PKC_SYNC_OPT_SYNC);    //(X1+Y1)*(X2+Y2)-C
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk1;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk4;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk5;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_AMINUSBMODC, PKC_SYNC_OPT_SYNC);    //(X1+Y1)*(X2+Y2)-C-D
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk2;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk5;
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.x);
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);    //A*F*((X1+Y1)*(X2+Y2)-C-D)

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_point_add_inline()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_point_dbl_inline
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       Operates point doubling on the projective coordinate Twisted Edwards Curve.
 * \param       [in]  p_p1      The address of an doubling point P1(X1, Y1, Z1).
 * \param       [out] p_p3      The address of an doubled point P3(X3, Y3, Z3).
 * \param       [in]  p_dpwks   The address of a work area structure which contains 6 work area addresses in DP format.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_PWORK, #PKCD_EDDSA_DRMODP, #PKCD_EDDSA_ARMODP
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPC
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EDDSA_PDT_P, #PKCD_EDDSA_PWORK, #PKCD_EDDSA_DRMODP, #PKCD_EDDSA_ARMODP must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit), #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *      - Operates point doubling on the projective coordinate Twisted Edwards Curve.
 *      - The doubling is done following the method written in section 6 of  <br>
 *        D.J.Bernstein, P.Birkner, M.Joye, T.Lange and C.Peters "Twisted Edwards Curves".
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set #PKCD_EDDSA_PDT_P and #PKCD_EDDSA_PWORK area as PKCC DPT and DPC area respectively.
 * -# Calculate B = (X1 + Y1)^2 using PKCC S=(A+B) mod C and S=ABR^(-1) mod C mode.
 * -# Calculate C = X1 * X1 using PKCC S=ABR^(-1) mod C mode.
 * -# Calculate D = Y1 * Y1 using PKCC S=ABR^(-1) mod C mode.
 * -# Calculate E = a * C using PKCC S=ABR^(-1) mod C mode.
 * -# Calculate F = E + D using PKCC S=(A+B) mod C mode.
 * -# Calculate H = Z1 * Z1 using PKCC S=ABR^(-1) mod C mode.
 * -# Calculate J = F - 2H using PKCC S=(A-B) mod C mode.
 * -# Calculate Z3 = F * J using PKCC S=ABR^(-1) mod C mode.
 * -# Calculate X3 = (B - C - D) * J using PKCC S=ABR^(-1) mod C and S=(A-B) mod C mode.
 * -# Calculate Y3 = F * (E - D) using PKCC S=ABR^(-1) mod C and S=(A-B) mod C mode.
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_point_dbl_inline(const u_eddsa_work_t* const p_p1, const u_eddsa_work_t* const p_p3,
                                                const st_eddsa_dpwks_t* const p_dpwks)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EDDSA_FORMAT_DP_DPT(PKCD_EDDSA_PDT_P);
    PKCD_CMN_ICUM_PKCCB0DPC = PKCD_EDDSA_FORMAT_DP_UNIT(PKCD_EDDSA_PWORK);

    /* calc B(at wk1) = (X1+Y1)^2 */
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(p_p1->point.x);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EDDSA_FORMAT_DP_UNIT(p_p1->point.y);
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk2;
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_APLUSBMODC, PKC_SYNC_OPT_SYNC);
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk2;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk2;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk1;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc C(at wk2) = X1^2 */
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(p_p1->point.x);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EDDSA_FORMAT_DP_UNIT(p_p1->point.x);
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk2;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc D(at wk3) = Y1^2 */
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(p_p1->point.y);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EDDSA_FORMAT_DP_UNIT(p_p1->point.y);
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk3;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc E(at wk4) = aC */
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(PKCD_EDDSA_ARMODP);
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk2;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk4;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc F(at wk5) = E+D */
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk4;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk3;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk5;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_APLUSBMODC, PKC_SYNC_OPT_SYNC);

    /* calc H(at wk6) = Z1^2 */
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(p_p1->point.z);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EDDSA_FORMAT_DP_UNIT(p_p1->point.z);
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk6;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc J(at p_p3.y) = F-2H */
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk5;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk6;
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.x);
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_AMINUSBMODC, PKC_SYNC_OPT_SYNC);
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.x);
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk6;
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.y);
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_AMINUSBMODC, PKC_SYNC_OPT_SYNC);

    /* calc Z3(at p_p3.z) = F*J */
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk5;
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.y);
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.z);
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc X3(at p_p3.x) = (B-C-D)*J */
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk1;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk2;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk6;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_AMINUSBMODC, PKC_SYNC_OPT_SYNC);
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk6;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk3;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk1;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_AMINUSBMODC, PKC_SYNC_OPT_SYNC);
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk1;
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.y);
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.x);
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* calc Y3(at p_p3.y) = F*(E-D) */
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk4;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk3;
    PKCD_CMN_ICUM_PKCCB0DPS = p_dpwks->dpwk6;
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_AMINUSBMODC, PKC_SYNC_OPT_SYNC);
    PKCD_CMN_ICUM_PKCCB0DPA = p_dpwks->dpwk5;
    PKCD_CMN_ICUM_PKCCB0DPB = p_dpwks->dpwk6;
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EDDSA_FORMAT_DP_UNIT(p_p3->point.y);
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_point_dbl_inline()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_parse_curve
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       Parse curve parameters
 * \param       [out] p_curve           An adress of curve information structure defined inside EdDSA is set.
 * \param       [in]  p_curve_info      An adress of curve information structure defined PKC common is set.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Parse the curve parameters and set them in the curve information structure defined inside EC
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# if(NULL != p_curve || NULL != p_curve_info.
 *   -# Set p_curve_info paramters to member of p_curve
 *   -# Set ret to #PKCD_RET_OK
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_parse_curve(st_eddsa_curve_t* const p_curve, const st_pkc_ec_curve_t* const p_curve_info)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if ((NULL != p_curve) && (NULL != p_curve_info))
    {
        p_curve->len = p_curve_info->curve_len;
        p_curve->p_field  = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EDDSA_CURVE_PARAM_IDX_FIELD];
        p_curve->p_order  = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EDDSA_CURVE_PARAM_IDX_ORDER];
        p_curve->p_coeff1 = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EDDSA_CURVE_PARAM_IDX_COEFF1];
        p_curve->p_coeff2 = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EDDSA_CURVE_PARAM_IDX_COEFF2];
        p_curve->p_base_x = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EDDSA_CURVE_PARAM_IDX_BASE_X];
        p_curve->p_base_y = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EDDSA_CURVE_PARAM_IDX_BASE_Y];
        p_curve->p_algo_opt1 = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EDDSA_CURVE_PARAM_IDX_ALGO_OPT1];
        p_curve->p_algo_opt2 = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EDDSA_CURVE_PARAM_IDX_ALGO_OPT2];

        ret = PKCD_RET_OK;
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcd_eddsa_parse_curve()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_scalar_mult_base_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       #R_PKCD_EddsaScalarMultBaseInit argument check function.
 * \param       [in]   p_scalar_or_privkey      A scalar value or private key.
 * \param       [in]   p_ctx                    A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]   p_buf                    Buffer used for internal processing of PKC-Driver
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the arguments received by the #R_PKCD_EddsaScalarMultBaseInit function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_scalar_or_privkey) and (NULL != p_ctx) and (NULL != p_buf)
 *    -# Check the curve information with #r_pkcd_eddsa_chk_curve_info, and set ret to the result.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# If (#PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
 *        -# If (NULL != p_scalar_or_privkey->p_sel_data)
 *          -# Check if given private key binary is valid with #r_pkcd_eddsa_chk_priv_key,
 *             and set ret to the result.
 *        -# Else
 *          -# Set ret to #PKCD_RET_PARAMERR
 *      -# If (#PKC_DATA_TYPE_BYTE == p_scalar_or_privkey->data_type)
 *        -# Check if given scalar or digest is valid with #r_pkcd_eddsa_chk_scalar_or_digest,
 *           and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCD_RET_PARAMERR
 *    -# Else
 *      -# Do nothing
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_scalar_mult_base_init_arg(const st_pkc_select_data_t* const p_scalar_or_privkey,
                                                                const st_pkc_eddsa_ctx_t* const p_ctx,
                                                                const uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if ((NULL != p_scalar_or_privkey) && (NULL != p_ctx) && (NULL != p_buf))
    {
        ret = r_pkcd_eddsa_chk_curve_info(p_ctx->p_curve_info);
        if (PKCD_RET_OK == ret)
        {
            if (PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
            {
                if (NULL != p_scalar_or_privkey->p_sel_data)
                {
                    ret = r_pkcd_eddsa_chk_priv_key(
                                        /* No problem with casting to pointer size */
                                        (const st_pkcd_eddsa_key_bin_t*)p_scalar_or_privkey->p_sel_data->p_data,
                                        p_ctx->p_curve_info->curve_len);
                }
                else
                {
                    ret = PKCD_RET_PARAMERR;
                }
            }
            else if (PKC_DATA_TYPE_BYTE == p_scalar_or_privkey->data_type)
            {
                ret = r_pkcd_eddsa_chk_scalar_or_digest(p_scalar_or_privkey->p_sel_data,
                                                        p_ctx->p_curve_info->curve_len);
            }
            else
            {
                ret = PKCD_RET_PARAMERR;
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_scalar_mult_base_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_scalar_mult_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       #R_PKCD_EddsaScalarMultInit argument check function.
 * \param       [in]   p_scalar_or_privkey      A scalar value or private key.
 * \param       [in]   p_comp_point_or_pubkey   A compress point or public key.
 * \param       [in]   p_ctx                    A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]   p_buf                    Buffer used for internal processing of PKC-Driver
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the arguments received by the #R_PKCD_EddsaScalarMultInit function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_scalar_or_privkey) and (NULL != p_comp_point_or_pubkey) and (NULL != p_ctx) and (NULL != p_buf)
 *    -# Check the curve information with #r_pkcd_eddsa_chk_curve_info, and set ret to the result.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# If (#PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
 *        -# If (NULL != p_scalar_or_privkey->p_sel_data)
 *          -# Check if given private key binary is valid with #r_pkcd_eddsa_chk_priv_key,
 *             and set ret to the result.
 *        -# Else
 *          -# Set ret to #PKCD_RET_PARAMERR
 *      -# If (#PKC_DATA_TYPE_BYTE == p_scalar_or_privkey->data_type)
 *        -# Check if given scalar or digest is valid with #r_pkcd_eddsa_chk_scalar_or_digest,
 *           and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCD_RET_PARAMERR
 *      -# If (#PKCD_RET_OK == ret)
 *        -# If (#PKC_DATA_TYPE_KEY_BINARY == p_comp_point_or_pubkey->data_type)
 *          -# If (NULL != p_comp_point_or_pubkey->p_sel_data)
 *            -# Check if given public key binary is valid with #r_pkcd_eddsa_chk_pub_key,
 *               and set ret to the result.
 *          -# Else
 *            -# Set ret to #PKCD_RET_PARAMERR
 *        -# If (#PKC_DATA_TYPE_BYTE == p_comp_point_or_pubkey->data_type)
 *          -# Check if given compress point is valid with #r_pkcd_eddsa_chk_scalar,
 *             and set ret to the result.
 *        -# Else
 *          -# Set ret to #PKCD_RET_PARAMERR
 *      -# Else
 *        -# Do nothing
 *    -# Else
 *      -# Do nothing
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_scalar_mult_init_arg(const st_pkc_select_data_t* const p_scalar_or_privkey,
                                                        const st_pkc_select_data_t* const p_comp_point_or_pubkey,
                                                        const st_pkc_eddsa_ctx_t* const p_ctx,
                                                        const uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if ((NULL != p_scalar_or_privkey) && (NULL != p_comp_point_or_pubkey) && (NULL != p_ctx) && (NULL != p_buf))
    {
        ret = r_pkcd_eddsa_chk_curve_info(p_ctx->p_curve_info);
        if (PKCD_RET_OK == ret)
        {
            if (PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
            {
                if (NULL != p_scalar_or_privkey->p_sel_data)
                {
                    ret = r_pkcd_eddsa_chk_priv_key(
                                        /* No problem with casting to pointer size */
                                        (const st_pkcd_eddsa_key_bin_t*)p_scalar_or_privkey->p_sel_data->p_data,
                                        p_ctx->p_curve_info->curve_len);
                }
                else
                {
                    ret = PKCD_RET_PARAMERR;
                }
            }
            else if (PKC_DATA_TYPE_BYTE == p_scalar_or_privkey->data_type)
            {
                ret = r_pkcd_eddsa_chk_scalar_or_digest(p_scalar_or_privkey->p_sel_data,
                                                        p_ctx->p_curve_info->curve_len);
            }
            else
            {
                ret = PKCD_RET_PARAMERR;
            }

            if (PKCD_RET_OK == ret)
            {
                if (PKC_DATA_TYPE_KEY_BINARY == p_comp_point_or_pubkey->data_type)
                {
                    if (NULL != p_comp_point_or_pubkey->p_sel_data)
                    {
                        ret = r_pkcd_eddsa_chk_pub_key(
                                        /* No problem with casting to pointer size */
                                        (const st_pkcd_eddsa_key_bin_t*)p_comp_point_or_pubkey->p_sel_data->p_data,
                                        p_ctx->p_curve_info->curve_len);
                    }
                    else
                    {
                        ret = PKCD_RET_PARAMERR;
                    }
                }
                else if (PKC_DATA_TYPE_BYTE == p_comp_point_or_pubkey->data_type)
                {
                    ret = r_pkcd_eddsa_chk_scalar(p_comp_point_or_pubkey->p_sel_data, p_ctx->p_curve_info->curve_len);
                }
                else
                {
                    ret = PKCD_RET_PARAMERR;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_scalar_mult_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_derive_s_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       #R_PKCD_EddsaDeriveS argument check function.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in] p_buf          Buffer used for internal processing of PKC-Driver
 * \param       [in] p_scalar_s     An integer s derived expanding private key.
 * \param       [in] p_scalar_r     A scalar r derived hashing over integer s and message.
 * \param       [in] p_digest_r_a_m A digest derived hashing over signature.R, public key A and message.
 * \param       [in] p_sign_s       An EdDSA signature s.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the arguments received by the #R_PKCD_EddsaDeriveS function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_sign_s)
 *    -# Check the curve information with #r_pkcd_eddsa_chk_curve_info, and set ret to the result.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# Check the scalar s with #r_pkcd_eddsa_chk_scalar, and set ret to the result.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# Check the scalar r with #r_pkcd_eddsa_chk_digest, and set ret to the result.
 *        -# If (#PKCD_RET_OK == ret)
 *          -# Check the digest R,A,M  with #r_pkcd_eddsa_chk_digest, and set ret to the result.
 *          -# If (#PKCD_RET_OK == ret)
 *            -# If (NULL != p_sign_s->p_data)
 *              -# If (p_sign_s->len >= p_ctx->p_curve_info->curve_len)
 *                -# Do nothing
 *              -# Else
 *                -# Set ret to #PKCD_RET_LENGTHERR
 *            -# Else
 *              -# Set ret to #PKCD_RET_PARAMERR
 *          -# Else
 *            -# Do nothing
 *        -# Else
 *          -# Do nothing
 *      -# Else
 *        -# Do nothing
 *    -# Else
 *      -# Do nothing
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_derive_s_arg(const st_pkc_eddsa_ctx_t* const p_ctx,
                                                const uint8_t* const p_buf,
                                                const st_pkc_data_t* const p_scalar_s,
                                                const st_pkc_data_t* const p_scalar_r,
                                                const st_pkc_data_t* const p_digest_r_a_m,
                                                const st_pkc_data_t* const p_sign_s)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_sign_s))
    {
        ret = r_pkcd_eddsa_chk_curve_info(p_ctx->p_curve_info);
        if (PKCD_RET_OK == ret)
        {
            ret = r_pkcd_eddsa_chk_scalar(p_scalar_s, p_ctx->p_curve_info->curve_len);
            if (PKCD_RET_OK == ret)
            {
                ret = r_pkcd_eddsa_chk_digest(p_scalar_r, p_ctx->p_curve_info->curve_len);
                if (PKCD_RET_OK == ret)
                {
                    ret = r_pkcd_eddsa_chk_digest(p_digest_r_a_m, p_ctx->p_curve_info->curve_len);
                    if (PKCD_RET_OK == ret)
                    {
                        if (NULL != p_sign_s->p_data)
                        {
                            if (p_sign_s->len >= p_ctx->p_curve_info->curve_len)
                            {
                                /* Do nothing */
                            }
                            else
                            {
                                ret = PKCD_RET_LENGTHERR;
                            }
                        }
                        else
                        {
                            ret = PKCD_RET_PARAMERR;
                        }
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_derive_s_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_compare_sign_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       #R_PKCD_EddsaCompareSign argument check function.
 * \param       [in] p_ctx            A pointer to a structure that summarizes the parameters required by EC. <br>
 * \param       [in,out] p_buf        Buffer used for internal processing of PKC-Driver
 * \param       [in] p_point_ha       Coordinates data from {H(R,A,M) * public key A}.
 * \param       [in] p_point_sb       Coordinates data from {signature S * base point B}.
 * \param       [in] p_sign_r         A signature r.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the arguments received by the #R_PKCD_EddsaCompareSign function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_ctx) and (NULL != p_buf)
 *    -# Check the curve information with #r_pkcd_eddsa_chk_curve_info, and set ret to the result.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# Check the point hA with #r_pkcd_eddsa_chk_scalar, and set ret to the result.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# Check the point SB with #r_pkcd_eddsa_chk_scalar, and set ret to the result.
 *        -# If (#PKCD_RET_OK == ret)
 *          -# Check the sign r  with #r_pkcd_eddsa_chk_scalar, and set ret to the result.
 *        -# Else
 *          -# Do nothing
 *      -# Else
 *        -# Do nothing
 *    -# Else
 *      -# Do nothing
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_compare_sign_arg(const st_pkc_eddsa_ctx_t* const p_ctx,
                                                    const uint8_t* const p_buf,
                                                    const st_pkc_data_t* const p_point_ha,
                                                    const st_pkc_data_t* const p_point_sb,
                                                    const st_pkc_data_t* const p_sign_r)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* check if the scalar are NULL */

    if ((NULL != p_ctx) && (NULL != p_buf))
    {
        ret = r_pkcd_eddsa_chk_curve_info(p_ctx->p_curve_info);
        if (PKCD_RET_OK == ret)
        {
            if ((NULL != p_point_ha) && (NULL != p_point_sb))
            {
                if ((NULL != p_point_ha->p_data) && (NULL != p_point_sb->p_data))
                {
                    if ((p_point_ha->len == (p_ctx->p_curve_info->curve_len * PKCD_EDDSA_TRIPLE_LEN)) &&
                        (p_point_sb->len == (p_ctx->p_curve_info->curve_len * PKCD_EDDSA_TRIPLE_LEN)))
                    {
                        ret = r_pkcd_eddsa_chk_scalar(p_sign_r, p_ctx->p_curve_info->curve_len);
                    }
                    else
                    {
                        ret = PKCD_RET_LENGTHERR;
                    }
                }
                else
                {
                    ret = PKCD_RET_PARAMERR;
                }
            }
            else
            {
                ret = PKCD_RET_PARAMERR;
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_compare_sign_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_make_scalar_s_and_r_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       #R_PKCD_EddsaMakeScalarSandR argument check function.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] p_priv_key     EdDSA private key of key binary format.
 * \param       [in] p_msg          Message to be signed.
 * \param       [in] p_scalar_s     An integer s derived expanding private key.
 * \param       [in] p_scalar_r     A scalar r derived hashing over integer s and message.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the arguments received by the #R_PKCD_EddsaMakeScalarSandR function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_scalar_s) and (NULL != p_scalar_r)
 *    -# If (NULL != p_ctx->p_curve_info)
 *      -# Check the curve length with #r_pkcd_eddsa_chk_curve_len, and set ret to the result.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# If (NULL != p_ctx->p_hash_info)
 *          -# If (NULL != p_ctx->p_hash_info->p_hash_init_api) and (NULL != p_ctx->p_hash_info->p_hash_update_api)
 *             and (NULL != p_ctx->p_hash_info->p_hash_final_api)
 *            -# Check if given private key binary is valid with #r_pkcd_eddsa_chk_priv_key,
 *               and set ret to the result.
 *            -# If (#PKCD_RET_OK == ret)
 *              -# Check if given message is valid with #r_pkcd_eddsa_chk_msg,
 *                 and set ret to the result.
 *              -# If (#PKCD_RET_OK == ret)
 *                -# If (#PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len)
 *                  -# Set digest_len to #PKCD_EDDSA_ED25519_DIGEST_LEN.
 *                -# Else
 *                  -# Set digest_len to #PKCD_EDDSA_ED448_DIGEST_LEN.
 *                -# If (p_scalar_s->len >=  p_ctx->p_curve_info->curve_len) and (p_scalar_r->len >= digest_len)
 *                  -# Do nothing
 *                -# Else
 *                  -# Set ret to #PKCD_RET_LENGTHERR
 *              -# Else
 *                -# Do nothing
 *            -# Else
 *              -# Do nothing
 *          -# Else
 *            -# Set ret to #PKCD_RET_PARAMERR
 *        -# Else
 *          -# Set ret to #PKCD_RET_PARAMERR
 *      -# Else
 *        -# Do nothing
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_make_scalar_s_and_r_arg(const st_pkc_eddsa_ctx_t* const p_ctx,
                                                            const uint8_t* const p_buf,
                                                            const void* const p_priv_key,
                                                            const st_pkc_data_t* const p_msg,
                                                            const st_pkc_data_t* const p_scalar_s,
                                                            const st_pkc_data_t* const p_scalar_r)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t digest_len;

    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_scalar_s) && (NULL != p_scalar_r))
    {
        if (NULL != p_ctx->p_curve_info)
        {
            ret = r_pkcd_eddsa_chk_curve_len(p_ctx->p_curve_info->curve_len);
            if (PKCD_RET_OK == ret)
            {
                if (NULL != p_ctx->p_hash_info)
                {
                    if ((NULL != p_ctx->p_hash_info->p_hash_init_api) &&
                        (NULL != p_ctx->p_hash_info->p_hash_update_api) &&
                        (NULL != p_ctx->p_hash_info->p_hash_final_api))
                    {
                        ret = r_pkcd_eddsa_chk_variant(p_ctx->eddsa_variant, p_ctx->p_ed_ctx,
                                                        p_ctx->p_curve_info->curve_len);
                        if (PKCD_RET_OK == ret)
                        {
                            /* No problem with casting to pointer size */
                            ret = r_pkcd_eddsa_chk_priv_key((const st_pkcd_eddsa_key_bin_t*)p_priv_key,
                                    p_ctx->p_curve_info->curve_len);
                            if (PKCD_RET_OK == ret)
                            {
                                ret = r_pkcd_eddsa_chk_msg(p_msg);
                                if (PKCD_RET_OK == ret)
                                {
                                    if ((NULL != p_scalar_s->p_data) && (NULL != p_scalar_r->p_data))
                                    {
                                        if (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len)
                                        {
                                            digest_len = PKCD_EDDSA_ED25519_DIGEST_LEN;
                                        }
                                        else
                                        {
                                            digest_len = PKCD_EDDSA_ED448_DIGEST_LEN;
                                        }

                                        if ((p_scalar_s->len >=  p_ctx->p_curve_info->curve_len) &&
                                            (p_scalar_r->len >= digest_len))
                                        {
                                            /* Do nothing */
                                        }
                                        else
                                        {
                                            ret = PKCD_RET_LENGTHERR;
                                        }
                                    }
                                    else
                                    {
                                        ret = PKCD_RET_PARAMERR;
                                    }
                                }
                                else
                                {
                                    /* Do nothing */
                                }
                            }
                            else
                            {
                                /* Do nothing */
                            }
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    else
                    {
                        ret = PKCD_RET_PARAMERR;
                    }
                }
                else
                {
                    ret = PKCD_RET_PARAMERR;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_make_scalar_s_and_r_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_hash_r_a_m_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       #R_PKCD_EddsaHashRAM argument check function.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] p_sign_r       A signature r.
 * \param       [in] p_pub_key      EdDSA public key of key binary format.
 * \param       [in] p_msg          Message to be signed
 * \param       [in] p_digest_r_a_m Hash of sign r, public key A and message m
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the arguments received by the #R_PKCD_EddsaHashRAM function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_digest_r_a_m)
 *    -# If (NULL != p_ctx->p_curve_info)
 *      -# Check the curve length with #r_pkcd_eddsa_chk_curve_len, and set ret to the result.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# If (NULL != p_ctx->p_hash_info)
 *          -# If (NULL != p_ctx->p_hash_info->p_hash_init_api) and (NULL != p_ctx->p_hash_info->p_hash_update_api)
 *             and (NULL != p_ctx->p_hash_info->p_hash_final_api)
 *            -# Check if given sign r is valid with #r_pkcd_eddsa_chk_scalar, and set ret to the result.
 *            -# If (#PKCD_RET_OK == ret)
 *              -# Check if given public key binary is valid with #r_pkcd_eddsa_chk_pub_key,
 *                 and set ret to the result.
 *              -# If (#PKCD_RET_OK == ret)
 *                -# Check if given message is valid with #r_pkcd_eddsa_chk_msg, and set ret to the result.
 *                -# If (#PKCD_RET_OK == ret)
 *                  -# If (NULL != p_digest_r_a_m->p_data)
 *                    -# Set #PKCD_EDDSA_ED25519_DIGEST_LEN into the uint32_t variable digest_len
 *                    -# If (p_digest_r_a_m->len >=  digest_len)
 *                      -# Do nothing
 *                    -# Else
 *                      -# Set ret to #PKCD_RET_LENGTHERR
 *                  -# Else
 *                    -# Set ret to #PKCD_RET_PARAMERR
 *                -# Else
 *                  -# Do nothing
 *              -# Else
 *                -# Do nothing
 *            -# Else
 *              -# Do nothing
 *          -# Else
 *            -# Set ret to #PKCD_RET_PARAMERR
 *        -# Else
 *          -# Set ret to #PKCD_RET_PARAMERR
 *      -# Else
 *        -# Do nothing
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_hash_r_a_m_arg(const st_pkc_eddsa_ctx_t* const p_ctx,
                                                const uint8_t* const p_buf,
                                                const st_pkc_data_t* const p_sign_r,
                                                const void* const p_pub_key,
                                                const st_pkc_data_t* const p_msg,
                                                const st_pkc_data_t* const p_digest_r_a_m)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t digest_len;

    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_digest_r_a_m))
    {
        if (NULL != p_ctx->p_curve_info)
        {
            ret = r_pkcd_eddsa_chk_curve_len(p_ctx->p_curve_info->curve_len);
            if (PKCD_RET_OK == ret)
            {
                if (NULL != p_ctx->p_hash_info)
                {
                    if ((NULL != p_ctx->p_hash_info->p_hash_init_api) &&
                        (NULL != p_ctx->p_hash_info->p_hash_update_api) &&
                        (NULL != p_ctx->p_hash_info->p_hash_final_api))
                    {
                        ret = r_pkcd_eddsa_chk_variant(p_ctx->eddsa_variant, p_ctx->p_ed_ctx,
                                                        p_ctx->p_curve_info->curve_len);
                        if (PKCD_RET_OK == ret)
                        {
                            ret = r_pkcd_eddsa_chk_scalar(p_sign_r, p_ctx->p_curve_info->curve_len);
                            if (PKCD_RET_OK == ret)
                            {
                                /* No problem with casting to pointer size */
                                ret = r_pkcd_eddsa_chk_pub_key((const st_pkcd_eddsa_key_bin_t*)p_pub_key,
                                                                p_ctx->p_curve_info->curve_len);
                                if (PKCD_RET_OK == ret)
                                {
                                    ret = r_pkcd_eddsa_chk_msg(p_msg);
                                    if (PKCD_RET_OK == ret)
                                    {

                                        if (NULL != p_digest_r_a_m->p_data)
                                        {
                                            if (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len)
                                            {
                                                digest_len = PKCD_EDDSA_ED25519_DIGEST_LEN;
                                            }
                                            else
                                            {
                                                digest_len = PKCD_EDDSA_ED448_DIGEST_LEN;
                                            }

                                            if (p_digest_r_a_m->len >=  digest_len)
                                            {
                                                /* Do nothing */
                                            }
                                            else
                                            {
                                                ret = PKCD_RET_LENGTHERR;
                                            }
                                        }
                                        else
                                        {
                                            ret = PKCD_RET_PARAMERR;
                                        }
                                    }
                                    else
                                    {
                                        /* Do nothing */
                                    }

                                }
                                else
                                {
                                    /* Do nothing */
                                }
                            }
                            else
                            {
                                /* Do nothing */
                            }
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    else
                    {
                        ret = PKCD_RET_PARAMERR;
                    }
                }
                else
                {
                    ret = PKCD_RET_PARAMERR;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_hash_r_a_m_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_key_wrap_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       #R_PKCD_EddsaKeyWrap argument check function.
 * \param       [in] p_ctx              A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in] p_buf              Buffer used for internal processing of PKC-Driver
 * \param       [in] p_priv_key_raw     Byte array of private key.
 * \param       [in] p_pub_key_raw      Byte array of public key.
 * \param       [in] p_priv_key         Output EdDSA private key of key binary format.
 * \param       [in] p_pub_key          Output EdDSA public key of key binary format.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the arguments received by the #R_PKCD_EddsaKeyWrap function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_priv_key) and (NULL != p_pub_key)
 *    -# If (NULL != p_ctx->p_curve_info)
 *      -# Check the curve length with is valid with #r_pkcd_eddsa_chk_curve_len,
 *         and set ret to the return value.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# Check the argument p_priv_key_raw with #r_pkcd_eddsa_chk_scalar, and set ret to the return value.
 *        -# If (#PKCD_RET_OK == ret)
 *          -# Check the argument p_pub_key_raw with #r_pkcd_eddsa_chk_scalar, and set ret to the return value.
 *          -# If (#PKCD_RET_OK == ret)
 *            -# Set the argument p_priv_key into #st_pkcd_eddsa_key_bin_t * argument p_priv_key_bin.
 *            -# If (NULL != p_priv_key_bin->p_private) and (NULL != p_priv_key_bin->p_public)
 *              -# Set the argument p_pub_key into #st_pkcd_eddsa_key_bin_t * argument p_pub_key_bin.
 *              -# If (NULL != p_pub_key_bin->p_public)
 *                -# Do nothing.
 *              -# Else
 *                -# Set ret to #PKCD_RET_KEY_PARAMERR.
 *            -# Else
 *              -# Set ret to #PKCD_RET_KEY_PARAMERR.
 *          -# Else
 *            -# Do nothing.
 *        -# Else
 *          -# Do nothing.
 *      -# Else
 *        -# Do nothing.
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_key_wrap_arg(const st_pkc_eddsa_ctx_t* const p_ctx,
                                                const uint8_t* const p_buf,
                                                const st_pkc_data_t* const p_priv_key_raw,
                                                const st_pkc_data_t* const p_pub_key_raw,
                                                const void* const p_priv_key,
                                                const void* const p_pub_key)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const st_pkcd_eddsa_key_bin_t * p_priv_key_bin;
    const st_pkcd_eddsa_key_bin_t * p_pub_key_bin;

    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_priv_key) && (NULL != p_pub_key))
    {
        if ((NULL != p_ctx->p_curve_info))
        {
            ret = r_pkcd_eddsa_chk_curve_len(p_ctx->p_curve_info->curve_len);
            if (PKCD_RET_OK == ret)
            {
                ret = r_pkcd_eddsa_chk_scalar(p_priv_key_raw, p_ctx->p_curve_info->curve_len);
                if (PKCD_RET_OK == ret)
                {
                    ret = r_pkcd_eddsa_chk_scalar(p_pub_key_raw, p_ctx->p_curve_info->curve_len);
                    if (PKCD_RET_OK == ret)
                    {
                        /* No problem with casting to pointer size */
                        p_priv_key_bin = (const st_pkcd_eddsa_key_bin_t*)p_priv_key;
                        if ((NULL != p_priv_key_bin->p_private) && (NULL != p_priv_key_bin->p_public))
                        {
                            /* No problem with casting to pointer size */
                            p_pub_key_bin = (const st_pkcd_eddsa_key_bin_t*)p_pub_key;
                            if (NULL != p_pub_key_bin->p_public)
                            {
                                /* Do nothing */
                            }
                            else
                            {
                                ret = PKCD_RET_KEY_PARAMERR;
                            }
                        }
                        else
                        {
                            ret = PKCD_RET_KEY_PARAMERR;
                        }
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_key_wrap_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_scalar
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDEDDSAAPIsInternal
 * \brief        Check the scalar parameters
 * \param       [in]   p_scalar       A scalar value.
 * \param       [in]   curve_len      A curve length.
 * \return
 *  - #PKCD_RET_OK:                        Successfully done.
 *  - #PKCD_RET_INTERNALERR:               Internal processing failed
 *  - #PKCD_RET_PARAMERR:                  Some mandatory parameters are NULL.
 *  - #PKCD_RET_LENGTHERR:                 Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Check the private or public key parameters.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If (#PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) then Set expected_scalar_len to #PKCD_EDDSA_ED25519_KEY_LEN,
 *      otherwise Set expected_scalar_len to PKCD_EDDSA_ED448_RU4_KEY_LEN.
 * -# If p_scalar != NULL
 *   -# If p_scalar->p_data != NULL
 *     -# If p_scalar->len == expected_scalar_len
 *       -# Set ret to #PKCD_RET_OK
 *     -# Else
 *       -# Set ret to #PKCD_RET_LENGTHERR
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_scalar(const st_pkc_data_t* const p_scalar, const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const uint32_t expected_scalar_len = (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) ?
                                            PKCD_EDDSA_ED25519_KEY_LEN : PKCD_EDDSA_ED448_RU4_KEY_LEN;

    /* check if the scalar are NULL */
    if (NULL != p_scalar)
    {
        if (NULL != p_scalar->p_data)
        {
            if (p_scalar->len == expected_scalar_len)
            {
                ret = PKCD_RET_OK;
            }
            else
            {
                ret = PKCD_RET_LENGTHERR;
            }
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_scalar()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_digest
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDEDDSAAPIsInternal
 * \brief        Check the digest
 * \param       [in]   p_digest       A message digest.
 * \param       [in]   curve_len      A curve length.
 * \return
 *  - #PKCD_RET_OK:                        Successfully done.
 *  - #PKCD_RET_INTERNALERR:               Internal processing failed
 *  - #PKCD_RET_PARAMERR:                  Some mandatory parameters are NULL.
 *  - #PKCD_RET_LENGTHERR:                 Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Check the digest.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If (#PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) then <br>
 *      Set expected_digest_len to #PKCD_EDDSA_ED25519_DIGEST_LEN, <br>
 *      otherwise Set expected_scalar_len to PKCD_EDDSA_ED448_DIGEST_LEN.
 * -# If p_digest != NULL
 *   -# If p_digest->p_data != NULL
 *     -# If p_digest->len == expected_digest_len
 *       -# Set ret to #PKCD_RET_OK
 *     -# Else
 *       -# Set ret to #PKCD_RET_LENGTHERR
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_digest(const st_pkc_data_t* const p_digest, const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const uint32_t expected_digest_len = (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) ?
                                            PKCD_EDDSA_ED25519_DIGEST_LEN : PKCD_EDDSA_ED448_DIGEST_LEN;

    /* check if the p_digest is NULL */
    if (NULL != p_digest)
    {
        if (NULL != p_digest->p_data)
        {
            if (p_digest->len == expected_digest_len)
            {
                ret = PKCD_RET_OK;
            }
            else
            {
                ret = PKCD_RET_LENGTHERR;
            }
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_digest()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_scalar_or_digest
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDEDDSAAPIsInternal
 * \brief        Check the scalar or digest
 * \param       [in]   p_scalar_or_digest   A scalar or message digest.
 * \param       [in]   curve_len            A curve length.
 * \return
 *  - #PKCD_RET_OK:                        Successfully done.
 *  - #PKCD_RET_INTERNALERR:               Internal processing failed
 *  - #PKCD_RET_PARAMERR:                  Some mandatory parameters are NULL.
 *  - #PKCD_RET_LENGTHERR:                 Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Check the scalar or digest.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If #PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len
 *   -# Set expected_scalar_len to #PKCD_EDDSA_ED25519_KEY_LEN.
 *   -# Set expected_digest_len to #PKCD_EDDSA_ED25519_DIGEST_LEN.
 * -# Else
 *   -# Set expected_scalar_len to #PKCD_EDDSA_ED448_RU4_KEY_LEN.
 *   -# Set expected_digest_len to #PKCD_EDDSA_ED448_DIGEST_LEN.
 * -# If p_scalar_or_digest != NULL
 *   -# If p_scalar_or_digest->p_data != NULL
 *     -# If (p_scalar_or_digest->len == expected_scalar_len) or (p_scalar_or_digest->len == expected_digest_len)
 *       -# Set ret to #PKCD_RET_OK
 *     -# Else
 *       -# Set ret to #PKCD_RET_LENGTHERR
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_scalar_or_digest(const st_pkc_data_t* const p_scalar_or_digest,
                                                    const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t expected_scalar_len;
    uint32_t expected_digest_len;

    if (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len)
    {
        expected_scalar_len = PKCD_EDDSA_ED25519_KEY_LEN;
        expected_digest_len = PKCD_EDDSA_ED25519_DIGEST_LEN;
    }
    else
    {
        expected_scalar_len = PKCD_EDDSA_ED448_RU4_KEY_LEN;
        expected_digest_len = PKCD_EDDSA_ED448_DIGEST_LEN;
    }

    if (NULL != p_scalar_or_digest)
    {
        if (NULL != p_scalar_or_digest->p_data)
        {
            if ((p_scalar_or_digest->len == expected_scalar_len) ||
                (p_scalar_or_digest->len == expected_digest_len))
            {
                ret = PKCD_RET_OK;
            }
            else
            {
                ret = PKCD_RET_LENGTHERR;
            }
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_scalar_or_digest()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_msg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDEDDSAAPIsInternal
 * \brief        Check the message
 * \param       [in]   p_msg       A message digest.
 * \return
 *  - #PKCD_RET_OK:                        Successfully done.
 *  - #PKCD_RET_INTERNALERR:               Internal processing failed.
 *  - #PKCD_RET_PARAMERR:                  Some mandatory parameters are NULL.
 * \par Security components
 *  - No
lobal Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Check the digest.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If p_msg != NULL
 *   -# If p_msg->p_data != NULL
 *     -# If p_msg->len <= #PKCD_EDDSA_MSG_LEN_MAX
 *       -# Set ret to #PKCD_RET_OK
 *     -# Else
 *       -# Set ret to #PKCD_RET_LENGTHERR
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_msg(const st_pkc_data_t* const p_msg)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* check if the msg are NULL */
    if (NULL != p_msg)
    {
        if (NULL != p_msg->p_data)
        {
            if (p_msg->len <= PKCD_EDDSA_MSG_LEN_MAX)
            {
                ret = PKCD_RET_OK;
            }
            else
            {
                ret = PKCD_RET_LENGTHERR;
            }
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_msg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_priv_key
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDEDDSAAPIsInternal
 * \brief        Check the private key parameters
 * \param       [in]   p_priv_key_bin EdDSA private key of key binary format.
 * \param       [in]   curve_len      A curve length.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_PARAMERR:               An invalid argument was entered.
 *  - #PKCD_RET_LENGTHERR:              Invalid length.
 *  - #PKCD_RET_KEY_PARAMERR:           Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:          Invalid length entered in key binary format data
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the private key of binary format parameters.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (#PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) then Set expected_key_len to #PKCD_EDDSA_ED25519_KEY_LEN,
 *       otherwise Set expected_key_len to PKCD_EDDSA_ED448_RU4_KEY_LEN.
 *  -# If p_priv_key_bin != NULL
 *    -# If p_priv_key_bin->key_length is equal to expected_key_len then
 *      -# If p_priv_key_bin->p_private != NULL
 *        -# Set ret to #PKCD_RET_OK.
 *      -# Else
 *        -# Set ret to #PKCD_RET_KEY_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_KEY_LENGTHERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_priv_key(const st_pkcd_eddsa_key_bin_t* const p_priv_key_bin,
                                            const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const uint32_t expected_key_len = (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) ?
                                        PKCD_EDDSA_ED25519_KEY_LEN : PKCD_EDDSA_ED448_RU4_KEY_LEN;

    if (NULL != p_priv_key_bin)
    {
        if (p_priv_key_bin->key_length == expected_key_len)
        {
            if (NULL != p_priv_key_bin->p_private)
            {
                ret = PKCD_RET_OK;
            }
            else
            {
                ret = PKCD_RET_KEY_PARAMERR;
            }
        }
        else
        {
            ret = PKCD_RET_KEY_LENGTHERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_priv_key()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_pub_key
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDEDDSAAPIsInternal
 * \brief        Check the public key parameters
 * \param       [in]   p_pub_key_bin  EdDSA public key of key binary format or <br>
 *                                    EdDSA private key include public key of key binary format
 * \param       [in]   curve_len      A curve length.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_PARAMERR:               An invalid argument was entered.
 *  - #PKCD_RET_LENGTHERR:              Invalid length.
 *  - #PKCD_RET_KEY_PARAMERR:           Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:          Invalid length entered in key binary format data
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the public key of binary format parameters.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (#PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) then Set expected_key_len to #PKCD_EDDSA_ED25519_KEY_LEN,
 *       otherwise Set expected_key_len to PKCD_EDDSA_ED448_RU4_KEY_LEN.
 *  -# If p_pub_key_bin != NULL
 *    -# If p_pub_key_bin->key_length is equal to expected_key_len then
 *      -# If p_pub_key_bin->p_public != NULL
 *        -# Set ret to #PKCD_RET_OK.
 *      -# Else
 *        -# Set ret to #PKCD_RET_KEY_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_KEY_LENGTHERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_pub_key(const st_pkcd_eddsa_key_bin_t* const p_pub_key_bin,
                                            const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const uint32_t expected_key_len = (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) ?
                                        PKCD_EDDSA_ED25519_KEY_LEN : PKCD_EDDSA_ED448_RU4_KEY_LEN;

    if (NULL != p_pub_key_bin)
    {
        if (p_pub_key_bin->key_length == expected_key_len)
        {
            if (NULL != p_pub_key_bin->p_public)
            {
                ret = PKCD_RET_OK;
            }
            else
            {
                ret = PKCD_RET_KEY_PARAMERR;
            }
        }
        else
        {
            ret = PKCD_RET_KEY_LENGTHERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_pub_key()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_curve_info
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDEDDSAAPIsInternal
 * \brief        Check the curve information
 * \param       [in]   p_curve_info   A curve infomation.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed.
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the curve information.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If p_curve_info != NULL then
 *    -# Check the p_curve_info->curve_len with #r_pkcd_eddsa_chk_curve_len, and set ret to the return value.
 *    -# If ret == #PKCD_RET_OK
 *      -# If p_curve_info->curve_params.p_data != NULL then
 *        -# If p_curve_info->curve_params.len == (curve length * the number of curve parameters)
 *          -# Do nothing.
 *        -# Else
 *          -# Set ret to #PKCD_RET_LENGTHERR.
 *      -# Else
 *        -# Set ret to #PKCD_RET_PARAMERR.
 *    -# Else
 *      -# Do nothing
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_curve_info(const st_pkc_ec_curve_t* const p_curve_info)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* Check if the argument is null */
    if (NULL != p_curve_info)
    {
        /* Check curve_len */
        ret = r_pkcd_eddsa_chk_curve_len(p_curve_info->curve_len);
        if (PKCD_RET_OK == ret)
        {
            if (NULL != p_curve_info->curve_params.p_data)
            {
                if ((p_curve_info->curve_len * PKCD_EDDSA_CURVE_PARAM_NUM) == p_curve_info->curve_params.len)
                {
                    /* Do nothing */
                }
                else
                {
                    ret = PKCD_RET_LENGTHERR;
                }
            }
            else
            {
                ret = PKCD_RET_PARAMERR;
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_curve_info()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_curve_len
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDEDDSAAPIsInternal
 * \brief        Check the curve length
 * \param       [in]   curve_len      A curve length.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_LENGTHERR:              Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the curve length.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If curve_len is equal to curve length of #PKCD_EDDSA_CURVE_EDWARDS25519_LEN or #PKCD_EDDSA_CURVE_EDWARDS448_LEN
 *    -# Set ret to #PKCD_RET_OK.
 *  -# Else
 *    -# Set ret to #PKCD_RET_LENGTHERR.
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_curve_len(const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    switch (curve_len)
    {
        case PKCD_EDDSA_CURVE_EDWARDS25519_LEN :
        case PKCD_EDDSA_CURVE_EDWARDS448_LEN :
            ret = PKCD_RET_OK;
            break;
        default:
            ret = PKCD_RET_LENGTHERR;
            break;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_curve_len()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_chk_variant
**********************************************************************************************************************/
/*!****************************************************************************************************************/ /**
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       Check EdDSA variant settings.
 * \param       [in]   eddsa_variant       EdDSA variant
 * \param       [in]   p_ed_ctx            Up to 255 bytes of context set by signers and verifiers in EdDSA
 * \param       [in]   curve_len           EdDSA curve length
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Make sure the correct variants and curves are set.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize ret with #PKCD_RET_INTERNALERR.
 *  -# If ((#PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) and (#PKC_EDDSA_VARIANT_PURE == eddsa_variant))
 *    -# Set ret to #PKCD_RET_OK.
 *  -# Else if ((#PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) and (#PKC_EDDSA_VARIANT_CTX == eddsa_variant))
 *    -# If NULL != p_ed_ctx
 *      -# If ((#PKCD_EDDSA_ED_CTX_LEN_MIN <= p_ed_ctx->len) and (#PKCD_EDDSA_ED_CTX_LEN_MAX >= p_ed_ctx->len))
 *        -# If (NULL != p_ed_ctx->p_data)
 *          -# Set ret to #PKCD_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCD_RET_PARAMERR.
 *      -# Else
 *        -# Set ret to #PKCD_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else if ((#PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) and (#PKC_EDDSA_VARIANT_HASH == eddsa_variant)) or
 *             ((#PKCD_EDDSA_CURVE_EDWARDS448_LEN == curve_len) and (#PKC_EDDSA_VARIANT_PURE == eddsa_variant)) or
 *             ((#PKCD_EDDSA_CURVE_EDWARDS448_LEN == curve_len) and (#PKC_EDDSA_VARIANT_HASH == eddsa_variant))
 *    -# If NULL != p_ed_ctx
 *      -# If (#PKCD_EDDSA_ED_CTX_LEN_MAX >= p_ed_ctx->len)
 *        -# If (0UL == p_ed_ctx->len)
 *          -# Set ret to #PKCD_RET_OK.
 *        -# Else
 *          -# If (NULL != p_ed_ctx->p_data)
 *            -# Set ret to #PKCD_RET_OK.
 *          -# Else
 *            -# Set ret to #PKCD_RET_PARAMERR.
 *      -# Else
 *        -# Set ret to #PKCD_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_chk_variant(const e_pkc_eddsa_variant_t eddsa_variant,
                                            const st_pkc_data_t *const p_ed_ctx, const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* Ed25519 */
    if ((PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) && (PKC_EDDSA_VARIANT_PURE == eddsa_variant))
    {
        ret = PKCD_RET_OK;
    }
    /* Ed25519ctx */
    else if ((PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) && (PKC_EDDSA_VARIANT_CTX == eddsa_variant))
    {
        if (NULL != p_ed_ctx)
        {
            if ((PKCD_EDDSA_ED_CTX_LEN_MIN <= p_ed_ctx->len) && (PKCD_EDDSA_ED_CTX_LEN_MAX >= p_ed_ctx->len))
            {
                if (NULL != p_ed_ctx->p_data)
                {
                    ret = PKCD_RET_OK;
                }
                else
                {
                    ret = PKCD_RET_PARAMERR;
                }
            }
            else
            {
                ret = PKCD_RET_LENGTHERR;
            }
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }
    }
    /* Ed25519ph or Ed448,Ed448ph */
    else if (((PKCD_EDDSA_CURVE_EDWARDS25519_LEN == curve_len) && (PKC_EDDSA_VARIANT_HASH == eddsa_variant)) ||
            ((PKCD_EDDSA_CURVE_EDWARDS448_LEN == curve_len) && (PKC_EDDSA_VARIANT_PURE == eddsa_variant)) ||
            ((PKCD_EDDSA_CURVE_EDWARDS448_LEN == curve_len) && (PKC_EDDSA_VARIANT_HASH == eddsa_variant)))
    {
        if (NULL != p_ed_ctx)
        {
            if (PKCD_EDDSA_ED_CTX_LEN_MAX >= p_ed_ctx->len)
            {
                if (0U == p_ed_ctx->len)
                {
                    ret = PKCD_RET_OK;
                }
                else
                {
                    if (NULL != p_ed_ctx->p_data)
                    {
                        ret = PKCD_RET_OK;
                    }
                    else
                    {
                        ret = PKCD_RET_PARAMERR;
                    }
                }
            }
            else
            {
                ret = PKCD_RET_LENGTHERR;
            }
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcd_eddsa_chk_variant()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_set_len_info
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDEDDSAAPIsInternal
 * \brief       Initialize length informations
 * \param       [in] curve_len            A byte size of curve length.
 * \return
 *  - None
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EDDSA_OFFSET_VAL, #PKCD_EDDSA_PBYTES
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Set length information used in EdDSA operation.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Set #PKCD_EDDSA_PBYTES to curve_len.
 * -# Set the offset from the beginning of the WORK area to the data start position to #PKCD_EDDSA_OFFSET_VAL
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static void r_pkcd_eddsa_set_len_info(const uint32_t curve_len)
{
    PKCD_EDDSA_PBYTES = curve_len;
    PKCD_EDDSA_OFFSET_VAL = PKCD_EDDSA_WORKUNIT - curve_len;
}
/***********************************************************************************************************************
* End of function r_pkcd_eddsa_set_len_info()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_encode_scalar
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDEDDSAAPIsInternal
 * \brief        Encode scalar values
 * \param       [in,out]   p_scalar      A address which contain scalar value.
 * \return
 *  - None
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Encoded to Little-endhian integer.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Clear tmp_scalar buffer with #r_pkcd_cmn_ram_clr.
 * -# Loop while i is p_scalar->len or less increment i for each loop
 *   -# Copy from the value after the p_scalar to the tmp buffer
 * -# If (0U != p_scalar->len)
 *   -# Copy from tmp buffer to p_scalar
 * -# Else
 *   -# Do nothing
 *
 * \callgraph
 *********************************************************************************************************************/
static void r_pkcd_eddsa_encode_scalar(st_eddsa_scalar_t* const p_scalar)
{
    uint32_t i;
    st_eddsa_scalar_t tmp_scalar;

    r_pkcd_cmn_ram_clr(tmp_scalar.scalar, PKCD_EDDSA_DIGEST_LEN_MAX);

    for (i = 0U; i < p_scalar->len; i++ )
    {
        tmp_scalar.scalar[i] = p_scalar->scalar[(p_scalar->len - 1U) - i];
    }

    if (0U != p_scalar->len)
    {
        r_pkcd_cmn_cpy_word(p_scalar->scalar, tmp_scalar.scalar, p_scalar->len);
    }
    else
    {
        /* Do nothing */
    }
}
/**********************************************************************************************************************
* End of function r_pkcd_eddsa_encode_scalar()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_make_scalar_s
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDEDDSAAPIsInternal
 * \brief        Encode scalar values
 * \param       [in,out]   p_scalar      A address which contain scalar value.
 * \return
 *  - None
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Encoded to Little-endhian integer.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Clear tmp_scalar buffer with #r_pkcd_cmn_ram_clr.
 * -# Loop while i is p_scalar->len or less increment i for each loop
 *   -# Copy from the value after the p_scalar to the tmp buffer
 * -# If (0U != p_scalar->len)
 *   -# Copy from tmp buffer to p_scalar
 * -# Else
 *   -# Do nothing
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_make_scalar_s(const st_pkc_eddsa_ctx_t* const p_ctx,
                                            const st_pkcd_eddsa_key_bin_t * p_key_bin,
                                            const uint8_t * const p_ascii_data,
                                            const uint32_t ascii_len,
                                            const uint32_t key_len,
                                            st_eddsa_scalar_t* hashed_priv_key,
                                            st_pkc_data_t* const p_scalar_s)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t hash_ret;

    hash_ret = p_ctx->p_hash_info->p_hash_init_api(p_ctx->p_hash_info->p_hash_ctx);

    if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
    {
        hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_ctx->p_hash_info->p_hash_ctx,
                                                            p_key_bin->p_private, key_len);

        if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
        {
            hash_ret = p_ctx->p_hash_info->p_hash_final_api(p_ctx->p_hash_info->p_hash_ctx, hashed_priv_key->scalar,
                                                            &hashed_priv_key->len);
            if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
            {
                if (PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len)
                {
                    /* No problem for casting to support usual arithmetic conversion */
                    hashed_priv_key->scalar[0] = (uint8_t)((uint32_t)hashed_priv_key->scalar[0] &
                                                    (uint32_t)PKCD_EDDSA_MASK_F8);
                    hashed_priv_key->scalar[p_ctx->p_curve_info->curve_len - 1U] =
                                (uint8_t)((uint32_t)hashed_priv_key->scalar[p_ctx->p_curve_info->curve_len - 1U] &
                                                    (uint32_t)PKCD_EDDSA_MASK_3F);
                    hashed_priv_key->scalar[p_ctx->p_curve_info->curve_len - 1U] =
                                (uint8_t)((uint32_t)hashed_priv_key->scalar[p_ctx->p_curve_info->curve_len - 1U] |
                                                    (uint32_t)PKCD_EDDSA_MASK_40);
                    r_pkcd_cmn_cpy_word(p_scalar_s->p_data, hashed_priv_key->scalar,
                                        p_ctx->p_curve_info->curve_len);
                    p_scalar_s->len = p_ctx->p_curve_info->curve_len;
                }
                else
                {
                    /* No problem for casting to support usual arithmetic conversion */
                    hashed_priv_key->scalar[0] = (uint8_t)((uint32_t)hashed_priv_key->scalar[0] &
                                                    (uint32_t)PKCD_EDDSA_MASK_FC);
                    hashed_priv_key->scalar[PKCD_EDDSA_ED448_KEY_LEN - 1U] =
                                (uint8_t)((uint32_t)hashed_priv_key->scalar[PKCD_EDDSA_ED448_KEY_LEN - 1U] &
                                                    0x00U);
                    hashed_priv_key->scalar[PKCD_EDDSA_ED448_SECOND_TO_LAST_IDX] =
                                (uint8_t)((uint32_t)hashed_priv_key->scalar[PKCD_EDDSA_ED448_SECOND_TO_LAST_IDX] |
                                                    (uint32_t)PKCD_EDDSA_MASK_80);
                    r_pkcd_cmn_ram_clr(p_scalar_s->p_data, p_scalar_s->len);
                    r_pkcd_cmn_cpy_byte(p_scalar_s->p_data, hashed_priv_key->scalar,
                                        PKCD_EDDSA_ED448_KEY_LEN);
                    p_scalar_s->len = PKCD_EDDSA_ED448_RU4_KEY_LEN;
                }

                ret = PKCD_RET_OK;
            }
            else
            {
                ret = PKCD_RET_HASHAPIERR;
            }
        }
        else
        {
            ret = PKCD_RET_HASHAPIERR;
        }
    }
    else
    {
        ret = PKCD_RET_HASHAPIERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcd_eddsa_make_scalar_s()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_eddsa_make_scalar_r
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDEDDSAAPIsInternal
 * \brief        Encode scalar values
 * \param       [in,out]   p_scalar      A address which contain scalar value.
 * \return
 *  - None
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Encoded to Little-endhian integer.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Clear tmp_scalar buffer with #r_pkcd_cmn_ram_clr.
 * -# Loop while i is p_scalar->len or less increment i for each loop
 *   -# Copy from the value after the p_scalar to the tmp buffer
 * -# If (0U != p_scalar->len)
 *   -# Copy from tmp buffer to p_scalar
 * -# Else
 *   -# Do nothing
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_eddsa_make_scalar_r(const st_pkc_eddsa_ctx_t* const p_ctx,
                                            const st_pkc_data_t* const p_msg,
                                            const uint8_t * const p_ascii_data,
                                            const uint32_t ascii_len,
                                            const uint32_t key_len,
                                            const st_eddsa_scalar_t* hashed_priv_key,
                                            st_pkc_data_t* const p_scalar_r)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t hash_ret;
    uint8_t ph_flag;
    uint8_t ctx_len;

    hash_ret = p_ctx->p_hash_info->p_hash_init_api(p_ctx->p_hash_info->p_hash_ctx);
    if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
    {
        /* Additional hashing for Ed25519ctx and Ed25519ph, Hashing for Ed448 and Ed448ph */
        if (((PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len) &&
            (PKC_EDDSA_VARIANT_PURE != p_ctx->eddsa_variant)) ||
            (PKCD_EDDSA_CURVE_EDWARDS448_LEN == p_ctx->p_curve_info->curve_len))
        {
            /* get pre hash flag */
            ph_flag = (PKC_EDDSA_VARIANT_HASH == p_ctx->eddsa_variant) ? 1U : 0U;

            hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_ctx->p_hash_info->p_hash_ctx, p_ascii_data, ascii_len);

            if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
            {
                hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_ctx->p_hash_info->p_hash_ctx,
                                                                &ph_flag,
                                                                sizeof(ph_flag));
                if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
                {
                    /* Since ctx_len is set to a value that fits in uint8_t,
                       there is no problem even if you cast it */
                    ctx_len = (uint8_t)p_ctx->p_ed_ctx->len;
                    hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_ctx->p_hash_info->p_hash_ctx,
                                                                    &ctx_len,
                                                                    sizeof(ctx_len));
                    if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
                    {
                        if (0U != p_ctx->p_ed_ctx->len)
                        {
                            hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_ctx->p_hash_info->p_hash_ctx,
                                                                            p_ctx->p_ed_ctx->p_data,
                                                                            p_ctx->p_ed_ctx->len);
                            if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
                            {
                                /* Do nothing */
                            }
                            else
                            {
                                ret = PKCD_RET_HASHAPIERR;
                            }
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    else
                    {
                        ret = PKCD_RET_HASHAPIERR;
                    }
                }
                else
                {
                    ret = PKCD_RET_HASHAPIERR;
                }
            }
            else
            {
                ret = PKCD_RET_HASHAPIERR;
            }
        }
        else
        {
            /* Do nothing */
        }

        if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
        {
            hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_ctx->p_hash_info->p_hash_ctx,
                                                            &hashed_priv_key->scalar[key_len],
                                                            key_len);
            if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
            {
                hash_ret = p_ctx->p_hash_info->p_hash_update_api(p_ctx->p_hash_info->p_hash_ctx,
                                                                p_msg->p_data,
                                                                p_msg->len);
                if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
                {
                    hash_ret = p_ctx->p_hash_info->p_hash_final_api(p_ctx->p_hash_info->p_hash_ctx, p_scalar_r->p_data,
                                                                    &p_scalar_r->len);
                    if (PKCD_CMN_EXTERNAL_API_OK == hash_ret)
                    {
                        if (PKCD_EDDSA_CURVE_EDWARDS448_LEN == p_ctx->p_curve_info->curve_len)
                        {
                            p_scalar_r->len = PKCD_EDDSA_ED448_DIGEST_LEN;
                        }
                        else
                        {
                            /* Do nothing */
                        }

                        ret = PKCD_RET_OK;
                    }
                    else
                    {
                        ret = PKCD_RET_HASHAPIERR;
                    }
                }
                else
                {
                    ret = PKCD_RET_HASHAPIERR;
                }
            }
            else
            {
                ret = PKCD_RET_HASHAPIERR;
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCD_RET_HASHAPIERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcd_eddsa_make_scalar_r()
**********************************************************************************************************************/
