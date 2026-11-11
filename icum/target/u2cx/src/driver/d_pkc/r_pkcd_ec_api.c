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
/* PRQA S 0310,0316,3305 ++ # D005 In order not to disclose the structure used internally, after receiving
                        it with type uint8_t* or void*, cast it to the type of the structure. */
/* PRQA S 0311 ++ # D006 No problem because the value of the target variable is not changed in the subsequent
                        processing.*/
/* PRQA S 0488 ++ # D007 Consider not to overrun the size of the array pointed to by the pointer.*/
/* PRQA S 0750 ++ # D008 Use union to efficiently use RAM area. */
/* PRQA S 0751 ++ # D009 For commonality of functions, confirm that the size is correct and use the cast value. */
/* PRQA S 0751 ++ # D010 Cast required to calculated the value to set in the register. */
/* PRQA S 1503 ++ # D013 No problem if it is unused because it is an API called from outside the library */
/* PRQA S 1505, 1532 ++ # D014 Since files are grouped by function, they are called from different files. */
/* PRQA S 2053 ++ # D016 No problem because it is a comment explaining the process. */
/* PRQA S 2913,3383,3384 ++ # D018 Have confirmed in advance that the value doesn't wrap around. */
/* PRQA S 2981 ++ # D020 Local variable ret of APIs sets INTERNALERR to the initial value
                        as a countermeasure against attacks that pass through processing. */
/* PRQA S 2984,2985 ++ # D021 Because it is calculated in consideration of future expansion */
/* PRQA S 3120 ++ # D023 Because it is a constant for the bit position and dosen't change in synchronization
                        with other values. */
/* PRQA S 3453,3469,3472 ++ # D026 If it is a function, overhead is large, so it corresponds with macro. */
/* PRQA S 3673 ++ # D029 Because future extensions may edit values in buffer. */
/* PRQA S 5002 ++ # D030 There is no problem because it is used in a system that can distinguish
                    file names of 8 characters or more. */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "renesas_types.h"
#include "r_pkc_api.h"
#include "r_pkcd_cfg.h"
#include "r_pkcd_api.h"
#include "r_pkcd_dev_api.h"
#include "r_pkcd_cmn.h"

/*!********************************************************************
 * \addtogroup    PKCDEC
 * \{
 * \addtogroup    PKCDECDefines            Elliptic curve Definitions
 * \addtogroup    PKCDECTypes              Elliptic curve Types
 * \addtogroup    PKCDECConstants          Elliptic curve Constants
 * \addtogroup    PKCDECAPIs               Elliptic curve APIs
 * \addtogroup    PKCDECDefinesInternal    Elliptic curve Internal Definitions
 * \addtogroup    PKCDECTypesInternal      Elliptic curve Internal Types
 * \addtogroup    PKCDECVariablesInternal  Elliptic curve Internal Variables
 * \addtogroup    PKCDECAPIsInternal       Elliptic curve Internal APIs
 * \}******************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDECDefinesInternal
 * \{******************************************************************/
#define PKCD_EC_WORKUNITPDT     (0x00000008U)  /**< A size of 8byte PDT area */
#define PKCD_EC_WORKUNIT        (0x00000050U)  /**< A size of 80byte work area */
#define PKCD_EC_WORKARP         (PKCD_EC_WORKUNIT*2U)  /**< A size of 160byte work area for aR and p area*/
#define PKCD_EC_WORKXYZ         (PKCD_EC_WORKUNIT*3U)  /**< A size of 240byte work area for xyz area*/
#define PKCD_EC_WORKUNITPDW     (PKCD_EC_WORKUNIT*8U)  /**< A size of 640byte work area */

#define PKCD_EC_DLBYTES         (sp_ec_pkccram->dlbytes)   /**< A byte length of DL */
#define PKCD_EC_DL64BYTES       (sp_ec_pkccram->dl64bytes) /**< A byte length of DL + 64bit */
#define PKCD_EC_OFFSET_VAL      (sp_ec_pkccram->offval)    /**< A size of padding of DL in bytes */
#define PKCD_EC_OFFSET_DL64VAL  (sp_ec_pkccram->offdl64val)    /**< A size of padding of DL + 64bit in bytes*/
#define PKCD_EC_PDT_P           (sp_ec_pkccram->pdt_p)     /**< A work area for T mod p */
#define PKCD_EC_PDT_N           (sp_ec_pkccram->pdt_n)     /**< A work area for T mod n */
#define PKCD_EC_PWORK           (sp_ec_pkccram->pwork)     /**< A 80byte work area which stores p */
#define PKCD_EC_NWORK           (sp_ec_pkccram->nwork)     /**< A 80byte work area which stores n */
#define PKCD_EC_WORK1           (sp_ec_pkccram->work1)     /**< A 80byte work area */
#define PKCD_EC_WORK2           (sp_ec_pkccram->work2)     /**< A 80byte work area */
#define PKCD_EC_WORK3           (sp_ec_pkccram->sharework1.unit.work1) /**< A 80byte work area */
#define PKCD_EC_WORK4           (sp_ec_pkccram->sharework1.unit.work2) /**< A 80byte work area */
#define PKCD_EC_WORK5           (sp_ec_pkccram->sharework1.unit.work3) /**< A 80byte work area */
#define PKCD_EC_WORK6           (sp_ec_pkccram->sharework2.unit.work1) /**< A 80byte work area */
#define PKCD_EC_WORK7           (sp_ec_pkccram->sharework2.unit.work2) /**< A 80byte work area */
#define PKCD_EC_SCALARWORK      (sp_ec_pkccram->scalarwork)    /**< A 80byte work area which stores scalar */
#define PKCD_EC_R2MOD           (sp_ec_pkccram->r2modwork)     /**< A 80byte work area which stores R^2 mod (p or n) */
#define PKCD_EC_XYZWORK1        (&sp_ec_pkccram->xyzwork1)     /**< A 240byte xyz work area */
#define PKCD_EC_XYZWORK2        (&sp_ec_pkccram->xyzwork2)     /**< A 240byte xyz work area */
#define PKCD_EC_WWORK           (sp_ec_pkccram->wwork)         /**< A 640byte work area */
#define PKCD_EC_XYZWORKEXT      (&sp_ec_pkccram->sharework1.xyz)   /**< A 240byte xyz work area, <br>
                                                                        which area overlaps work3 to 5 area */
#define PKCD_EC_ARPWORK         (&sp_ec_pkccram->sharework2.arp)   /**< A 160byte aRp work area, <br>
                                                                        which area overlaps work6 to 7 area */

/** Convert pointer shifted by PDT data area size to PKCC data pointer register format */
#define PKCD_EC_FORMAT_DP_DPT(top)          (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_EC_WORKUNITPDT))
/** Convert pointer shifted by PDW data area size to PKCC data pointer register format */
#define PKCD_EC_FORMAT_DP_DPW(top)          (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_EC_WORKUNITPDW))
/** Convert pointer shifted by unit size of work to PKCC data pointer register format */
#define PKCD_EC_FORMAT_DP_UNIT(top)         (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_EC_WORKUNIT))
/** Convert pointer shifted by aR and p size of work to PKCC data pointer register format */
#define PKCD_EC_FORMAT_DP_ARP(top)          (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_EC_WORKARP))
/** Convert pointer shifted by XYZ size of work to PKCC data pointer register format */
#define PKCD_EC_FORMAT_DP_XYZ(top)          (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_EC_WORKXYZ))

#define PKCD_EC_OFFSET_LEN(len)             (PKCD_EC_WORKUNIT - (len))  /**< Offset to value of len work area */

#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
#define PKCD_EC_CURVE_NIST_P521_LEN         (68U)      /**< The byte length of NIST P-521 */
#endif

#define PKCD_EC_CURVE_LEN_MIN               (24U)      /**< Minimum length of curve length */
#define PKCD_EC_CURVE_LEN_MAX               (68U)      /**< Maximum length of curve length */
#define PKCD_EC_DIGEST_LEN_MIN              (4U)       /**< Minimum length of digest */
#define PKCD_EC_DIGEST_LEN_MAX              (PKCD_EC_CURVE_LEN_MAX)  /**< Maximum length of digest */
#define PKCD_EC_INPUT_DIGEST_LEN_MAX        (0x80000000U)  /**< Maximum length of input digest */

#define PKCD_EC_CURVE_PARAM_IDX_FIELD       (0U)       /**< Field p index of concatenated curve parameters */
#define PKCD_EC_CURVE_PARAM_IDX_ORDER       (1U)       /**< Order n index of concatenated curve parameters */
#define PKCD_EC_CURVE_PARAM_IDX_COEFF1      (2U)       /**< Coefficient a index of concatenated curve parameters */
#define PKCD_EC_CURVE_PARAM_IDX_COEFF2      (3U)       /**< Coefficient b index of concatenated curve parameters */
#define PKCD_EC_CURVE_PARAM_IDX_BASE_X      (4U)       /**< Base point x index of concatenated curve parameters */
#define PKCD_EC_CURVE_PARAM_IDX_BASE_Y      (5U)       /**< Base point y of concatenated curve parameters */
#define PKCD_EC_CURVE_PARAM_NUM             (6U)       /**< Number of Concatenated curve parameters */

#define PKCD_EC_POINT_DATA_IDX_X            (0U)       /**< x coordinate index of concatenated point data */
#define PKCD_EC_POINT_DATA_IDX_Y            (1U)       /**< y coordinate index of concatenated point data */
#define PKCD_EC_POINT_DATA_NUM              (2U)       /**< Number of Concatenated of concatenated point data */

#define PKCD_EC_SIGN_DATA_IDX_R             (0U)       /**< Signature r index of concatenated signature data */
#define PKCD_EC_SIGN_DATA_IDX_S             (1U)       /**< Signature s index of concatenated signature data */
#define PKCD_EC_SIGN_DATA_NUM               (2U)       /**< Number of Concatenated of of concatenated signature data */

#define PKCD_EC_EXTRA_BYTES                 (8U)       /**< Number of bytes to add to random numbers when <br>
                                                              generating Private Scalar */

#define PKCD_EC_MASK_07                     (0x07U)    /**< the value used to take the remainder (mod 7) */
#define PKCD_EC_MASK_FE                     (0xFEU)    /**< the value used for clear LSbit */
/*! \}*/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDECTypesInternal
 * \{******************************************************************/
/** Work area structure for xyz points */
typedef struct
{
    uint8_t xyz[PKCD_EC_WORKXYZ];   /**< A work area for Coordinates (X, Y, Z) */
} st_ec_work_xyz_t;

/** Work area structure for aR and p */
typedef struct
{
    uint8_t arp[PKCD_EC_WORKARP];   /**< A work area for aR and p */
} st_ec_work_arp_t;

/** Areas shared with PKCD_EC_WORKUNIT area and xyz work area */
typedef union
{
    struct
    {
        uint8_t work1[PKCD_EC_WORKUNIT];   /**< 80byte work area */
        uint8_t work2[PKCD_EC_WORKUNIT];   /**< 80byte work area */
        uint8_t work3[PKCD_EC_WORKUNIT];   /**< 80byte work area */
    } unit;                         /**< Three PKCD_EC_WORKUNIT areas */
    st_ec_work_xyz_t xyz;           /**< A 240byte xyz work area */
} u_ec_share_work_xyz_t;

/** Areas shared with PKCD_EC_WORKUNIT area and aRp work area */
typedef union
{
    struct
    {
        uint8_t work1[PKCD_EC_WORKUNIT];   /**< 80byte work area */
        uint8_t work2[PKCD_EC_WORKUNIT];   /**< 80byte work area */
    } unit;                         /**< Two PKCD_EC_WORKUNIT areas */
    st_ec_work_arp_t arp;           /**< A 160byte aR and p work area */
} u_ec_share_work_arp_t;

/** Work area structure of PKCC RAM */
typedef struct
{
    uint32_t                dlbytes;                        /**< A byte length of DL */
    uint32_t                dl64bytes;                      /**< A byte length of DL + 64bit */
    uint32_t                offval;                         /**< A size of padding of DL in bytes */
    uint32_t                offdl64val;                     /**< A size of padding of DL + 64bit in bytes */
    uint8_t                 pdt_p[PKCD_EC_WORKUNITPDT];     /**< A work area for T mod p */
    uint8_t                 pdt_n[PKCD_EC_WORKUNITPDT];     /**< A work area for T mod q */
    uint8_t                 pwork[PKCD_EC_WORKUNIT];        /**< A 80byte work area which stores p */
    uint8_t                 nwork[PKCD_EC_WORKUNIT];        /**< A 80byte work area which stores n */
    uint8_t                 work1[PKCD_EC_WORKUNIT];        /**< A 80byte work area */
    uint8_t                 work2[PKCD_EC_WORKUNIT];        /**< A 80byte work area */
    u_ec_share_work_xyz_t   sharework1;                     /**< A 240byte share work area */
    u_ec_share_work_arp_t   sharework2;                     /**< A 160byte share work area */
    uint8_t                 scalarwork[PKCD_EC_WORKUNIT];   /**< A 80byte work area which stores scalar */
    uint8_t                 r2modwork[PKCD_EC_WORKUNIT];    /**< A 80byte work area which stores R^2 mod (p or n) */
    st_ec_work_xyz_t        xyzwork1;                       /**< A 240byte xyz work area */
    st_ec_work_xyz_t        xyzwork2;                       /**< A 240byte xyz work area */
    uint8_t                 wwork[PKCD_EC_WORKUNITPDW];     /**< A 640byte work area */
} st_ec_pkccram_t;

/** Data pointer structure of each coordinate of xyz */
typedef struct
{
    uint8_t * p_dl64_x;   /**< An X-coordinate value pointer in xyz work form */
    uint8_t * p_dl64_y;   /**< A Y-coordinate value pointer in xyz work form */
    uint8_t * p_dl64_z;   /**< A Z-coordinate value pointer in xyz work form */
} st_ec_xyz_data_t;

/** Data pointer structure of aR and p */
typedef struct
{
    uint8_t * p_dl64_ar;  /**< An aR value pointer in arp work form */
    uint8_t * p_dl64_p;   /**< A p value pointer in arp work form */
} st_ec_arp_data_t;

/** EC domain parameter structure of internal */
typedef struct
{
    uint32_t    len;        /**< Byte length of a curve len(must be a supported length) */
    uint8_t *   p_field;    /**< Field order */
    uint8_t *   p_order;    /**< Base point order */
    uint8_t *   p_coeff1;   /**< Coefficient a */
    uint8_t *   p_coeff2;   /**< Coefficient b */
    uint8_t *   p_base_x;   /**< Base point x */
    uint8_t *   p_base_y;   /**< Base point y */
} st_ec_curve_t;

/** Scalar value structure of internal */
typedef struct
{
    uint32_t len;                           /**< A byte length of a scalar */
    uint8_t  scalar[PKCD_EC_CURVE_LEN_MAX]; /**< A buffer of scalar */
} st_ec_scalar_t;

/** Elliptic curve affine coordinate structure of internal */
typedef struct
{
    uint32_t len;                       /**< A byte length of a point */
    uint8_t  x[PKCD_EC_CURVE_LEN_MAX];  /**< A buffer of x-coordinate data */
    uint8_t  y[PKCD_EC_CURVE_LEN_MAX];  /**< A buffer of y-coordinate data */
} st_ec_point_t;

/** ECC signature structure of internal */
typedef struct
{
    uint32_t len;                       /**< A byte length of signature members */
    uint8_t  r[PKCD_EC_CURVE_LEN_MAX];  /**< A buffer of signature value r */
    uint8_t  s[PKCD_EC_CURVE_LEN_MAX];  /**< A buffer of signature value s */
} st_ec_signature_t;

/** A parameter structure of scalar multiplication */
typedef struct
{
    st_ec_scalar_t scalar;      /**< A scalar value */
    st_ec_point_t  point;       /**< A point to be multiplied */
    uint8_t        random_data[PKCD_CMN_MUL_2 * PKCD_EC_CURVE_LEN_MAX]; /**< A random scalar */
    uint8_t        random_z[PKCD_EC_CURVE_LEN_MAX]; /**< random z coordinate */
} st_ec_prm_smult_t;

/** A parameter structure of make signature */
typedef struct
{
    uint8_t        digest[PKCD_EC_DIGEST_LEN_MAX];  /**< A message digest */
    uint32_t       digest_len;                 /**< A byte length of message digest */
    st_ec_point_t  point_q;                    /**< A point derived from G(x, y) x {random scalar k} */
    st_ec_scalar_t random;                     /**< A random scalar k */
    st_ec_scalar_t priv_key;                   /**< A scalar value */
} st_ec_prm_make_sign_t;

/** A parameter structure of Calculate U1 and U2 */
typedef struct
{
    uint8_t              digest[PKCD_EC_DIGEST_LEN_MAX];    /**< A message digest */
    uint32_t             digest_len;                    /**< A byte length of message digest */
    st_ec_signature_t    sign;                          /**< A ECDSA signature */
} st_ec_prm_calc_u1u2_t;

/** A parameter structure of compare signature */
typedef struct
{
    st_ec_point_t        u1g;      /**< A point derived from u1 x G(x, y). *where G is base point */
    st_ec_point_t        u2q;      /**< A point derived from u2 x Q(x, y). *where Q is public key */
    st_ec_signature_t    sign;     /**< A ECDSA signature */
    uint8_t              bux_x[PKCD_EC_WORKUNIT];   /**< Buffer to store intermediate data x */
} st_ec_prm_compare_sign_t;

/** A parameter structure for EC key generation */
typedef struct
{
    uint32_t random_len;
    uint8_t  random_data[PKCD_EC_CURVE_LEN_MAX + PKCD_EC_EXTRA_BYTES];
    uint32_t priv_scalar_len;
    uint8_t  priv_scalar_data[PKCD_EC_CURVE_LEN_MAX];
} st_ec_prm_keygen_t;

/** A parameter structure for EC private scalar generation */
typedef struct
{
    uint8_t  n_minus1[PKCD_EC_CURVE_LEN_MAX]; /**< (The order of the basepoint) - 1  */
} st_ec_prm_priv_scalar_gen_t;

/*! \}*/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDECVariablesInternal
 * \{******************************************************************/
/** PKCD_CMN_PKCCRAM address cast in st_ec_pkccram_t */
static st_ec_pkccram_t * const sp_ec_pkccram = ((st_ec_pkccram_t*)PKCD_CMN_PKCCRAM);

/*! \}*/

static pkcd_ret_t r_pkcd_ec_prepare_operation (const st_ec_curve_t * const p_curve);
static pkcd_ret_t r_pkcd_ec_inverse (const uint8_t * const p_wk_k, uint8_t * const p_wk_invk,
                                    const uint8_t * const p_wk1, const e_pkc_bool_t is_mod_p);
static pkcd_ret_t r_pkcd_ec_scalar_mul (const uint8_t * const p_x, const uint8_t * const p_y,
                                        const uint8_t * const p_scalar, const uint8_t * const p_coeff1,
                                        const uint32_t curve_len,  const e_pkc_sync_opt_t sync_option,
                                        const st_ec_work_xyz_t * const p_wk_xyz_out);
static pkcd_ret_t r_pkcd_ec_scalar_mul_sec (const st_ec_prm_smult_t * const p_prm, const uint8_t * const p_coeff1,
                                        const uint32_t curve_len, const st_ec_work_xyz_t * const p_wk_xyz_out);
static pkcd_ret_t r_pkcd_ec_point_add (const st_ec_work_xyz_t * const p_wk_xyz_a,
                                        const st_ec_work_xyz_t * const p_wk_xyz_b,
                                        const st_ec_work_xyz_t * const p_wk_xyz_c);
static pkcd_ret_t r_pkcd_ec_make_sign (const uint8_t * const p_digest, const uint32_t digest_len,
                                        const uint8_t * const p_q_x, const uint8_t * const p_random,
                                        const uint8_t * const p_priv_key_scalar, const uint8_t * const p_order,
                                        const uint32_t curve_len, st_pkc_data_t * const p_sign);
static pkcd_ret_t r_pkcd_ec_affine_to_projective (const uint8_t * const p_x, const uint8_t * const p_y,
                                                    const uint32_t curve_len, st_ec_work_xyz_t * const p_wk_xyz_out);
static pkcd_ret_t r_pkcd_ec_affine_to_proj_sec (const uint8_t * const p_x, const uint8_t * const p_y,
                                                const uint32_t curve_len, const uint8_t * const p_random,
                                                st_ec_work_xyz_t * const p_wk_xyz_out);
static pkcd_ret_t r_pkcd_ec_projective_to_affine (const st_ec_work_xyz_t * const p_wk_xyx, const uint32_t curve_len,
                                                    uint8_t * const p_x, uint8_t * const p_y);
static pkcd_ret_t r_pkcd_ec_calc_t (const uint8_t * const p_order, const uint8_t * const p_field,
                                    const uint32_t curve_len);
static pkcd_ret_t r_pkcd_ec_calc_wR (const uint8_t * const p_sign_s, const uint32_t curve_len, uint8_t * const p_wk_wR);
static pkcd_ret_t r_pkcd_ec_calc_u (const uint8_t * const p_wk_zr, const uint8_t * const p_wk_wR,
                                    const uint32_t curve_len, st_pkc_data_t * const p_u);
static pkcd_ret_t r_pkcd_ec_set_digest_to_work (const uint8_t * const p_digest, const uint32_t digest_len,
                                                const uint8_t * const p_order, const uint32_t curve_len,
                                                uint8_t * const p_wk_out);
static pkcd_ret_t r_pkcd_ec_chk_scalar_mult_init_arg (const st_pkc_select_data_t * const p_scalar_or_privkey,
                                                    const st_pkc_select_data_t * const p_point_or_pubkey,
                                                    const st_pkc_ec_ctx_t * const p_ctx,
                                                    const uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_ec_chk_make_sign_arg (const st_pkc_ec_ctx_t * const p_ctx,
                                                const uint8_t * const p_buf,
                                                const st_pkc_data_t * const p_digest,
                                                const st_pkc_data_t * const p_point_q,
                                                const st_pkc_data_t * const p_random,
                                                const void * const p_priv_key,
                                                const st_pkc_data_t * const p_sign);
static pkcd_ret_t r_pkcd_ec_chk_calc_u1u2_arg (const st_pkc_ec_ctx_t * const p_ctx,
                                                const uint8_t * const p_buf,
                                                const st_pkc_data_t * const p_digest,
                                                const st_pkc_data_t * const p_sign,
                                                const st_pkc_data_t * const p_u1,
                                                const st_pkc_data_t * const p_u2);
static pkcd_ret_t r_pkcd_ec_chk_compare_sign_arg (const st_pkc_ec_ctx_t * const p_ctx,
                                                    const uint8_t * const p_buf,
                                                    const st_pkc_data_t * const p_u1g,
                                                    const st_pkc_data_t * const p_u2q,
                                                    const st_pkc_data_t * const p_sign);
static pkcd_ret_t r_pkcd_ec_chk_private_scalar_gen_arg (const st_pkc_ec_ctx_t * const p_ctx,
                                                        const uint8_t * const p_buf,
                                                        const st_pkc_data_t * const p_extra_random,
                                                        const st_pkc_data_t * const p_priv_scalar);
static pkcd_ret_t r_pkcd_ec_chk_key_wrap_arg (const st_pkc_ec_ctx_t * const p_ctx,
                                                const st_pkc_data_t * const p_priv_key_raw,
                                                const st_pkc_data_t * const p_pub_key_raw,
                                                const void * const p_priv_key,
                                                const void * const p_pub_key);
static pkcd_ret_t r_pkcd_ec_chk_scalar (const st_pkc_data_t * const p_scalar, const uint32_t curve_len);
static pkcd_ret_t r_pkcd_ec_chk_priv_scalar (const st_pkc_data_t * const p_priv_scalar, const uint32_t curve_len);
static pkcd_ret_t r_pkcd_ec_chk_point (const st_pkc_data_t * const p_point, const uint32_t curve_len);
static pkcd_ret_t r_pkcd_ec_chk_digest (const st_pkc_data_t * const p_digest);
static pkcd_ret_t r_pkcd_ec_chk_priv_key (const st_pkcd_ec_key_bin_t * const p_priv_key_bin, const uint32_t curve_len);
static pkcd_ret_t r_pkcd_ec_chk_pub_key (const st_pkcd_ec_key_bin_t * const p_pub_key_bin, const uint32_t curve_len);
static pkcd_ret_t r_pkcd_ec_chk_sign (const st_pkc_data_t * const p_sign, const uint32_t curve_len);
static pkcd_ret_t r_pkcd_ec_chk_curve_info (const st_pkc_ec_curve_t * const p_curve_info);
static pkcd_ret_t r_pkcd_ec_chk_curve_len (const uint32_t curve_len);
static void r_pkcd_ec_parse_curve (st_ec_curve_t * const p_curve, const st_pkc_ec_curve_t * const p_curve_info);
static void r_pkcd_ec_set_len_info (const uint32_t curve_len);
static void r_pkcd_ec_cpy_scalar (const st_pkc_data_t * const p_in, st_ec_scalar_t * const p_out,
                                    const uint32_t curve_len);
static void r_pkcd_ec_cpy_point (const st_pkc_data_t * const p_in, st_ec_point_t * const p_out,
                                    const uint32_t curve_len);
static void r_pkcd_ec_cpy_digest (const st_pkc_data_t * const p_in, uint8_t * const p_out, uint32_t * const out_len);
static void r_pkcd_ec_cpy_sign (const st_pkc_data_t * const p_in, st_ec_signature_t * const p_out,
                                const uint32_t curve_len);
static pkcd_ret_t r_pkcd_ec_key_wrap (const st_pkc_ec_ctx_t * const p_ctx,
                            const st_pkc_data_t * const p_priv_key_raw,
                            const st_pkc_data_t * const p_pub_key_raw,
                            void * const p_priv_key,
                            void * const p_pub_key);
static pkcd_ret_t r_pkcd_ec_gen_rand_z (const st_pkc_ec_ctx_t * const p_ctx, uint8_t * const p_out);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcScalarMultInit
**********************************************************************************************************************/
pkcd_ret_t   R_PKCD_EcScalarMultInit(const st_pkc_select_data_t* const p_scalar_or_privkey,
                                        const st_pkc_select_data_t* const p_point_or_pubkey,
                                        const st_pkc_ec_ctx_t* const p_ctx,
                                        uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ec_prm_smult_t * const p_prm = (st_ec_prm_smult_t*)p_buf; /* No problem with casting to pointer size */
    const st_pkcd_ec_key_bin_t * p_key_bin;
    uint32_t drbg_ret;

    /* Check argument */
    ret = r_pkcd_ec_chk_scalar_mult_init_arg(p_scalar_or_privkey, p_point_or_pubkey, p_ctx, p_buf);

    if (PKCD_RET_OK == ret)
    {
        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* Clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        /* Clear buffer */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_ec_prm_smult_t));

        /* Copy parameters */
        if (PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
        {
            /* No problem with casting to pointer size */
            p_key_bin = (const st_pkcd_ec_key_bin_t*)p_scalar_or_privkey->p_sel_data->p_data;
            r_pkcd_cmn_cpy_word(p_prm->scalar.scalar, p_key_bin->p_d, p_key_bin->key_length);
            p_prm->scalar.len = p_key_bin->key_length;
        }
        else
        {
            r_pkcd_ec_cpy_scalar(p_scalar_or_privkey->p_sel_data, &p_prm->scalar, p_ctx->p_curve_info->curve_len);
        }

        if (PKC_DATA_TYPE_KEY_BINARY == p_point_or_pubkey->data_type)
        {
            /* No problem with casting to pointer size */
            p_key_bin = (const st_pkcd_ec_key_bin_t*)p_point_or_pubkey->p_sel_data->p_data;
            r_pkcd_cmn_cpy_word(p_prm->point.x, p_key_bin->p_qx, p_key_bin->key_length);
            r_pkcd_cmn_cpy_word(p_prm->point.y, p_key_bin->p_qy, p_key_bin->key_length);
            p_prm->point.len = p_key_bin->key_length;
        }
        else
        {
            r_pkcd_ec_cpy_point(p_point_or_pubkey->p_sel_data, &p_prm->point, p_ctx->p_curve_info->curve_len);
        }

        if (PKC_BOOL_FALSE != p_ctx->sec_scalar_mul)
        {
            /* Generate random data */
            drbg_ret = p_ctx->p_drbg_api(PKCD_CMN_MUL_2 * p_ctx->p_curve_info->curve_len, p_prm->random_data);

            if (PKCD_CMN_EXTERNAL_API_OK == drbg_ret)
            {
                /* Generate random data */
                ret = r_pkcd_ec_gen_rand_z(p_ctx, p_prm->random_z);
            }
            else
            {
                ret = PKCD_RET_DRBGAPIERR;
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

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EcScalarMultInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EcScalarMultOperate
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EcScalarMultOperate(const st_pkc_ec_ctx_t* const p_ctx,
                                        uint8_t* const p_buf,
                                        const e_pkc_sync_opt_t sync_opt)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ec_curve_t curve;
    st_ec_prm_smult_t * const p_prm = (st_ec_prm_smult_t*)p_buf; /* No problem with casting to pointer size */

    /* Check arguments */
    if (((NULL != p_ctx) && (NULL != p_buf)) && ((PKC_SYNC_OPT_SYNC == sync_opt) || (PKC_SYNC_OPT_ASYNC == sync_opt)))
    {
        ret = r_pkcd_ec_chk_curve_info(p_ctx->p_curve_info);

        if (PKCD_RET_OK == ret)
        {
            /* Parse curve */
            r_pkcd_ec_parse_curve(&curve, p_ctx->p_curve_info);

            /* Clear async operation flag */
            r_pkcd_cmn_clr_async_ope();

            /* prepare operation */
            ret = r_pkcd_ec_prepare_operation(&curve);

            if (PKC_BOOL_FALSE != p_ctx->sec_scalar_mul)
            {
                /* Calculate scalar multiple (anti-tamper) */
                ret |= r_pkcd_ec_scalar_mul_sec(p_prm, curve.p_coeff1, curve.len, PKCD_EC_XYZWORK1);

                /* Clear the random value used in the scalar multiplication */
                r_pkcd_cmn_ram_clr(p_prm->random_data, sizeof(p_prm->random_data));

                /* Clear the random value used to randomize Z */
                r_pkcd_cmn_ram_clr(p_prm->random_z, sizeof(p_prm->random_z));
            }
            else
            {
                /* Calculate Scalar multiple */
                ret |= r_pkcd_ec_scalar_mul(p_prm->point.x, p_prm->point.y, p_prm->scalar.scalar, curve.p_coeff1,
                                        curve.len, sync_opt, PKCD_EC_XYZWORK1);
            }

            /* Extract the return value from the ORed result */
            ret = r_pkcd_cmn_extract_result(ret);

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
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EcScalarMultOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EcScalarMultFinal
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EcScalarMultFinal(const st_pkc_ec_ctx_t* const p_ctx,
                                    uint8_t* const p_buf,
                                    st_pkc_data_t* const p_point)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t curve_len;

    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_point))
    {
        if (NULL != p_point->p_data)
        {
            ret = r_pkcd_ec_chk_curve_info(p_ctx->p_curve_info);
            if (PKCD_RET_OK == ret)
            {
                curve_len = p_ctx->p_curve_info->curve_len;
                if (p_point->len >= (curve_len * PKCD_EC_POINT_DATA_NUM))
                {
                    /* Clear async operation flag */
                    r_pkcd_cmn_clr_async_ope();

                    /* Convert the scalar multiplication result from projective coordinates to affine coordinates */
                    ret = r_pkcd_ec_projective_to_affine(PKCD_EC_XYZWORK1, curve_len,
                                                            &p_point->p_data[curve_len * PKCD_EC_POINT_DATA_IDX_X],
                                                            &p_point->p_data[curve_len * PKCD_EC_POINT_DATA_IDX_Y]);
                    p_point->len = p_ctx->p_curve_info->curve_len * PKCD_EC_POINT_DATA_NUM;
                }
                else
                {
                    ret = PKCD_RET_LENGTHERR;
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
* End of function R_PKCD_EcScalarMultFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EcdsaMakeSign
**********************************************************************************************************************/
pkcd_ret_t   R_PKCD_EcdsaMakeSign(const st_pkc_ec_ctx_t* const p_ctx,
                                    uint8_t* const p_buf,
                                    const st_pkc_data_t* const p_digest,
                                    const st_pkc_data_t* const p_point_q,
                                    const st_pkc_data_t* const p_random,
                                    const void* const p_priv_key,
                                    st_pkc_data_t* const p_sign)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ec_prm_make_sign_t * const p_prm = (st_ec_prm_make_sign_t*)p_buf; /* No problem with casting to pointer size */
    st_ec_curve_t curve;
    const st_pkcd_ec_key_bin_t * p_key_bin;

    /* Check argument */
    ret = r_pkcd_ec_chk_make_sign_arg(p_ctx, p_buf, p_digest, p_point_q, p_random, p_priv_key, p_sign);
    if (PKCD_RET_OK == ret)
    {
        /* Parse curve */
        r_pkcd_ec_parse_curve(&curve, p_ctx->p_curve_info);

        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        /* Clear buffer */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_ec_prm_make_sign_t));

        /* copy parameters */
        r_pkcd_ec_cpy_digest(p_digest, p_prm->digest, &p_prm->digest_len);
        r_pkcd_ec_cpy_point(p_point_q, &p_prm->point_q, curve.len);
        r_pkcd_ec_cpy_scalar(p_random, &p_prm->random, curve.len);
        p_key_bin = (const st_pkcd_ec_key_bin_t*)p_priv_key; /* No problem with casting to pointer size */
        r_pkcd_cmn_cpy_word(p_prm->priv_key.scalar, p_key_bin->p_d, p_key_bin->key_length);
        p_prm->priv_key.len = p_key_bin->key_length;

        /* prepare for operation */
        ret = r_pkcd_ec_prepare_operation(&curve);

        /* make sign */
        ret |= r_pkcd_ec_make_sign(p_prm->digest, p_prm->digest_len, p_prm->point_q.x, p_prm->random.scalar,
                                    p_prm->priv_key.scalar, curve.p_order, curve.len, p_sign);

        /* Clear the random number value used in signature generation stored in p_prm */
        r_pkcd_cmn_ram_clr(p_prm->random.scalar, sizeof(p_prm->random.scalar));

        /* Clear the private key value stored in p_prm */
        r_pkcd_cmn_ram_clr(p_prm->priv_key.scalar, p_prm->priv_key.len);

        /* Extract the return value from the ORed result */
        ret = r_pkcd_cmn_extract_result(ret);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EcdsaMakeSign()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EcdsaCalcU1U2
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EcdsaCalcU1U2(const st_pkc_ec_ctx_t* const p_ctx,
                                uint8_t* const p_buf,
                                const st_pkc_data_t* const p_digest,
                                const st_pkc_data_t* const p_sign,
                                st_pkc_data_t* const p_u1,
                                st_pkc_data_t* const p_u2)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ec_prm_calc_u1u2_t * const p_prm = (st_ec_prm_calc_u1u2_t*)p_buf; /* No problem with casting to pointer size */
    st_ec_curve_t curve;

    /* Check argument */
    ret = r_pkcd_ec_chk_calc_u1u2_arg(p_ctx, p_buf, p_digest, p_sign, p_u1, p_u2);
    if (PKCD_RET_OK == ret)
    {
        /* Parse curve */
        r_pkcd_ec_parse_curve(&curve, p_ctx->p_curve_info);

        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        /* Clear buffer */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_ec_prm_calc_u1u2_t));

        /* copy parameters */
        r_pkcd_ec_cpy_digest(p_digest, p_prm->digest, &p_prm->digest_len);
        r_pkcd_ec_cpy_sign(p_sign, &p_prm->sign, curve.len);

        /* prepare for operation */
        ret = r_pkcd_ec_prepare_operation(&curve);

        /* Calculate wR */
        ret |= r_pkcd_ec_calc_wR(p_prm->sign.s, curve.len, PKCD_EC_WORK2);

        /* calculate u1 = zw mod n with S = ABR^(-1) mod C */
        r_pkcd_cmn_ram_clr(PKCD_EC_WORK1, PKCD_EC_WORKUNIT);
        ret |= r_pkcd_ec_set_digest_to_work(p_prm->digest, p_prm->digest_len, curve.p_order,
                                            curve.len, PKCD_EC_WORK1);
        ret |= r_pkcd_ec_calc_u(PKCD_EC_WORK1, PKCD_EC_WORK2, curve.len, p_u1);

        /* calculate u2 = rw mod n with S = ABR^(-1) mod C */
        r_pkcd_cmn_ram_clr(PKCD_EC_WORK1, PKCD_EC_WORKUNIT);
        r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK1 + PKCD_EC_OFFSET_VAL, p_prm->sign.r, curve.len);
        ret |= r_pkcd_ec_calc_u(PKCD_EC_WORK1, PKCD_EC_WORK2, curve.len, p_u2);

        /* Extract the return value from the ORed result */
        ret = r_pkcd_cmn_extract_result(ret);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EcdsaCalcU1U2()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EcdsaCompareSign
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EcdsaCompareSign(const st_pkc_ec_ctx_t* const p_ctx,
                                    uint8_t* const p_buf,
                                    const st_pkc_data_t* const p_u1g,
                                    const st_pkc_data_t* const p_u2q,
                                    const st_pkc_data_t* const p_sign)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ec_curve_t curve;
    int32_t cmp_ret;
    st_ec_prm_compare_sign_t * const p_prm
        = (st_ec_prm_compare_sign_t*)p_buf; /* No problem with casting to pointer size */

    /* Check argument */
    ret = r_pkcd_ec_chk_compare_sign_arg(p_ctx, p_buf, p_u1g, p_u2q, p_sign);
    if (PKCD_RET_OK == ret)
    {
        /* Parse curve */
        r_pkcd_ec_parse_curve(&curve, p_ctx->p_curve_info);

        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* Clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        /* Clear buffer */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_ec_prm_calc_u1u2_t));

        /* copy parameters */
        r_pkcd_ec_cpy_point(p_u1g, &p_prm->u1g, curve.len);
        r_pkcd_ec_cpy_point(p_u2q, &p_prm->u2q, curve.len);
        r_pkcd_ec_cpy_sign(p_sign, &p_prm->sign, curve.len);

        /* Check if u1G = 0, which means u1G is the point at infinity. If u1G = 0, cmp_ret = 0 */
        cmp_ret = r_pkcd_cmn_cmp_zero_word(p_u1g->p_data, PKCD_CMN_MUL_2 * curve.len);

        if (0 != cmp_ret)
        {
            /* prepare for operation */
            ret = r_pkcd_ec_prepare_operation(&curve);

            /* convert u1G from affine coordinates to projective coordinates */
            ret |= r_pkcd_ec_affine_to_projective(p_prm->u1g.x, p_prm->u1g.y, curve.len, PKCD_EC_XYZWORK1);

            /* convert u2Q from affine coordinates to projective coordinates */
            ret |= r_pkcd_ec_affine_to_projective(p_prm->u2q.x, p_prm->u2q.y, curve.len, PKCD_EC_XYZWORK2);

#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
            if (PKCD_EC_CURVE_NIST_P521_LEN != curve.len)
            {
                /* Set DL to (curve length + 64) for ECC operations, unless NIST P-521 curve is used. */
                r_pkcd_cmn_set_dl(curve.len + PKCD_CMN_BITS64, curve.len + PKCD_CMN_BITS64);
            }
            else
            {
                /* Do nothing */
            }
#endif
            /* calculate C(XR, YR, ZR) = u1*G + u2*Q */
            ret |= r_pkcd_ec_point_add(PKCD_EC_XYZWORK1, PKCD_EC_XYZWORK2, PKCD_EC_XYZWORK1);

            /* Extract the return value from the ORed result */
            ret = r_pkcd_cmn_extract_result(ret);

            if (PKCD_RET_OK == ret)
            {
                /* use p_prm->bux_x as a temporary buffer for the Z-coordinate of u1G + u2Q */
                r_pkcd_cmn_cpy_by_64bounds(p_prm->bux_x, (uint8_t*)((uintptr_t)PKCD_EC_XYZWORK1
                                                        + (PKCD_EC_WORKXYZ - curve.len)), curve.len);

                /* Check if Z-coordinate of u1G + u2Q is zero */
                cmp_ret = r_pkcd_cmn_cmp_zero_word(p_prm->bux_x, curve.len);

                if (0 != cmp_ret)
                {
                    r_pkcd_cmn_ram_clr(p_prm->bux_x, sizeof(p_prm->bux_x));

                    /* convert u1G + u2Q from projective coordinates to affine coordinates */
                    ret = r_pkcd_ec_projective_to_affine(PKCD_EC_XYZWORK1, curve.len, p_prm->bux_x, NULL);
                }
                else
                {
                    ret = PKCD_RET_EC_VERIFAIL;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* If u1G is the point at infinity, then u1G + u2Q = u2Q  */
            r_pkcd_cmn_cpy_word(p_prm->bux_x, p_prm->u2q.x, curve.len);

            ret = PKCD_RET_OK;
        }

        if (PKCD_RET_OK == ret)
        {
            /* Double check on verifications */
            ret     = (pkcd_ret_t)0U;
            cmp_ret = r_pkcd_cmn_cmp_word(p_prm->bux_x, p_prm->sign.r, curve.len);

            if (0 != cmp_ret)
            {
                ret = PKCD_RET_EC_VERIFAIL;
            }
            else
            {
                /* Set some value of PKCD_RET_OK */
                ret     += PKCD_CMN_RET_OK_HALF_1;

                cmp_ret = r_pkcd_cmn_cmp_word(p_prm->bux_x, p_prm->sign.r, curve.len);

                if (0 != cmp_ret)
                {
                    ret = PKCD_RET_EC_VERIFAIL;
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
* End of function R_PKCD_EcdsaCompareSign()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EcKeyGenInit
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EcKeyGenInit(const st_pkc_ec_ctx_t* const p_ctx,
                            uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t curve_len;
    st_ec_prm_keygen_t * p_ec_keygen;
    uint32_t drbg_ret;

    /* Check argument */
    if ((NULL != p_ctx) && (NULL != p_buf))
    {
        /* check context */
        if ((NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info))
        {
            /* check curve info */
            ret = r_pkcd_ec_chk_curve_info(p_ctx->p_curve_info);
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

    if (PKCD_RET_OK == ret)
    {
        curve_len = p_ctx->p_curve_info->curve_len;

        /* No problem with casting to pointer size */
        p_ec_keygen = (st_ec_prm_keygen_t*)p_buf;

        /* Clear the buffer and set data lengths */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_ec_prm_keygen_t));
        p_ec_keygen->priv_scalar_len = curve_len;
        p_ec_keygen->random_len = curve_len + PKCD_EC_EXTRA_BYTES;

        /* Generate random data */
        drbg_ret = p_ctx->p_drbg_api(p_ec_keygen->random_len, p_ec_keygen->random_data);

        if (PKCD_CMN_EXTERNAL_API_OK == drbg_ret)
        {
            /* Do nothing */
        }
        else
        {
            ret = PKCD_RET_DRBGAPIERR;
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EcKeyGenInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EcKeyGenOperate
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EcKeyGenOperate(const st_pkc_ec_ctx_t* const p_ctx,
                            uint8_t* const p_buf,
                            const e_pkc_sync_opt_t sync_opt)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ec_prm_keygen_t * p_ec_keygen;
    uint8_t * p_priv_scalar_gen;
    uint8_t * p_buf_smult;
    st_pkc_data_t random_data;
    st_pkc_data_t priv_scalar_data;
    st_pkc_data_t base_point_data;
    st_pkc_select_data_t priv_scalar_sel;
    st_pkc_select_data_t base_point_sel;
    uint8_t base_point_val[PKCD_EC_CURVE_LEN_MAX * PKCD_CMN_MUL_2];

    /* Check argument */
    if ((NULL != p_ctx) && (NULL != p_buf))
    {
        /* check context */
        if ((NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info))
        {
            /* check curve info */
            ret = r_pkcd_ec_chk_curve_info(p_ctx->p_curve_info);
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

    if (PKCD_RET_OK == ret)
    {
        /* Cast p_buf to EC keygen buffer type */
        p_ec_keygen = (st_ec_prm_keygen_t*)p_buf;
        p_priv_scalar_gen = &p_buf[sizeof(st_ec_prm_keygen_t)];

        random_data.p_data = p_ec_keygen->random_data;
        random_data.len = p_ec_keygen->random_len;

        priv_scalar_data.p_data = p_ec_keygen->priv_scalar_data;
        priv_scalar_data.len = sizeof(p_ec_keygen->priv_scalar_data);

        /* Generate a private key */
        ret = R_PKCD_EcPrivateScalarGen(p_ctx, p_priv_scalar_gen, &random_data, &priv_scalar_data);

        /* Clear random data and EC keygen buffer */
        r_pkcd_cmn_ram_clr(p_priv_scalar_gen, sizeof(st_ec_prm_priv_scalar_gen_t));
        r_pkcd_cmn_ram_clr(random_data.p_data, sizeof(p_ec_keygen->random_len));

        if (PKCD_RET_OK == ret)
        {
            /* Save priv_scalar_len */
            p_ec_keygen->priv_scalar_len = priv_scalar_data.len;

            /* Prepare to pass private scalar to PKC-Driver */
            priv_scalar_sel.data_type = PKC_DATA_TYPE_BYTE;
            priv_scalar_sel.p_sel_data = &priv_scalar_data;

            /* Prepare to pass curve base point to PKC-Driver */
            base_point_data.p_data = base_point_val;
            r_pkcd_cmn_cpy_word(base_point_data.p_data,
                                &p_ctx->p_curve_info->curve_params.p_data[p_ctx->p_curve_info->curve_len *
                                                                            PKCD_EC_CURVE_PARAM_IDX_BASE_X],
                                p_ctx->p_curve_info->curve_len * PKCD_CMN_MUL_2);
            base_point_data.len = p_ctx->p_curve_info->curve_len * PKCD_CMN_MUL_2;
            base_point_sel.data_type = PKC_DATA_TYPE_BYTE;
            base_point_sel.p_sel_data = &base_point_data;

            /* Prepare a buffer for scalar multiplication */
            p_buf_smult = &p_buf[sizeof(st_ec_prm_keygen_t)];

            /* Call PKC-Driver scalar multiplication API */
            ret = R_PKCD_EcScalarMultInit(&priv_scalar_sel, &base_point_sel, p_ctx, p_buf_smult);

            if (PKCD_RET_OK == ret)
            {
                /* Call PKC-Driver API */
                ret = R_PKCD_EcScalarMultOperate(p_ctx, p_buf_smult, sync_opt);
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

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EcKeyGenOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EcKeyGenFinal
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EcKeyGenFinal(const st_pkc_ec_ctx_t * const p_ctx,
                                    uint8_t * const p_buf,
                                    void * const p_priv_key,
                                    void * const p_pub_key)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ec_prm_keygen_t * p_ec_keygen;
    uint8_t * p_buf_smult;
    st_pkc_data_t priv_scalar_data;
    st_pkc_data_t point_data;
    uint8_t point_val[PKCD_EC_CURVE_LEN_MAX * PKCD_CMN_MUL_2];

    /* Check argument */
    if ((NULL != p_ctx) && (NULL != p_buf))
    {
        /* check context */
        if (NULL != p_ctx->p_curve_info)
        {
            /* check curve info */
            ret = r_pkcd_ec_chk_curve_info(p_ctx->p_curve_info);
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

    if (PKCD_RET_OK == ret)
    {
        /* Prepare to pass point_data to PKC-Driver */
        point_data.len = sizeof(point_val);
        point_data.p_data = point_val;

        /* Prepare a buffer for scalar multiplication */
        p_buf_smult = &p_buf[sizeof(st_ec_prm_keygen_t)];

        /* Call PKC-Driver API */
        ret = R_PKCD_EcScalarMultFinal(p_ctx, p_buf_smult, &point_data);

        if (PKCD_RET_OK == ret)
        {
            /* Cast p_buf to EC keygen buffer type */
            p_ec_keygen = (st_ec_prm_keygen_t*)p_buf;

            /* Buffer data retrieval */
            priv_scalar_data.len = p_ec_keygen->priv_scalar_len;
            priv_scalar_data.p_data = p_ec_keygen->priv_scalar_data;

            /* Call a Key wrap function */
            ret = r_pkcd_ec_key_wrap(p_ctx, &priv_scalar_data, &point_data, p_priv_key, p_pub_key);

            /* Clear private scalar data */
            r_pkcd_cmn_ram_clr(p_ec_keygen->priv_scalar_data, p_ec_keygen->priv_scalar_len);
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
* End of function R_PKCD_EcKeyGenFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EcPrivateScalarGen
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EcPrivateScalarGen(const st_pkc_ec_ctx_t* const p_ctx,
                                        uint8_t* const p_buf,
                                        const st_pkc_data_t* const p_random,
                                        st_pkc_data_t* const p_priv_scalar)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ec_curve_t curve;
    st_ec_prm_priv_scalar_gen_t * const p_prm = (st_ec_prm_priv_scalar_gen_t*)p_buf; /* No problem with casting to
                                                                                    pointer size */
    uint32_t tmp32;

    /* Check argument */
    ret = r_pkcd_ec_chk_private_scalar_gen_arg(p_ctx, p_buf, p_random, p_priv_scalar);

    if (PKCD_RET_OK == ret)
    {
        r_pkcd_ec_parse_curve(&curve, p_ctx->p_curve_info);

        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* Clear a buffer */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_ec_prm_priv_scalar_gen_t));

        /* clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        ret = r_pkcd_ec_prepare_operation(&curve);

        if (PKCD_RET_OK == ret)
        {
            /* Calculate n-1 */
            r_pkcd_cmn_cpy_word(p_prm->n_minus1, curve.p_order, curve.len);

            /* No problem for casting to support usual arithmetic conversion */
            p_prm->n_minus1[curve.len - 1U] = (uint8_t)((uint32_t)p_prm->n_minus1[curve.len - 1U]
                                            & (uint32_t)PKCD_EC_MASK_FE);

            /* Calculate (random mod n-1) + 1, and set p_priv_scalar to the result. */
            ret = r_pkcd_cmn_reduce_even_modulus(p_random->p_data, p_prm->n_minus1, p_random->len, curve.len,
                                                (PKCD_EC_WORKUNIT + PKCD_CMN_BITS64), PKCD_EC_WORK1,
                                                PKCD_EC_WORK2 + PKCD_CMN_BITS64,
                                                PKCD_EC_WORK3 + (PKCD_CMN_MUL_2 * PKCD_CMN_BITS64),
                                                PKCD_EC_WORK4 + (PKCD_CMN_MUL_3 * PKCD_CMN_BITS64));
            ret |= r_pkcd_cmn_clr_work_unit(PKCD_EC_WORK1, PKCD_EC_WORKUNIT + PKCD_CMN_BITS64);
            tmp32 = r_pkcd_cmn_ch_endian32(0x00000001U);

            /* No problem for casting to support usual arithmetic conversion */
            r_pkcd_cmn_cpy_by_64bounds((uint8_t*)((uintptr_t)PKCD_EC_WORK1
                                        + ((PKCD_EC_WORKUNIT + PKCD_CMN_BITS64) - sizeof(uint32_t))),
                                        (uint8_t*)&tmp32, sizeof(uint32_t));
            ret |= r_pkcd_cmn_APlusB(PKCD_EC_WORK4 + (PKCD_CMN_MUL_3 * PKCD_CMN_BITS64), PKCD_EC_WORK1,
                                    PKCD_EC_WORK4 + (PKCD_CMN_MUL_3 * PKCD_CMN_BITS64),
                                    PKCD_EC_WORKUNIT + PKCD_CMN_BITS64);
            r_pkcd_cmn_cpy_by_64bounds(p_priv_scalar->p_data,
                                    (PKCD_EC_WORK4 + (PKCD_CMN_MUL_3 * PKCD_CMN_BITS64))
                                    + ((PKCD_EC_WORKUNIT + PKCD_CMN_BITS64) - curve.len),
                                    curve.len);
            p_priv_scalar->len = curve.len;

            /* Clear the random data */
            r_pkcd_cmn_ram_clr(p_random->p_data, p_random->len);

            /* Extract the PKC-Driver return value */
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
* End of function R_PKCD_EcPrivateScalarGen()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_key_wrap
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Wrap in key binary format
 * \param       [in] p_ctx              A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in] p_priv_key_raw     Byte array of private key.
 * \param       [in] p_pub_key_raw      Byte array of public key.
 * \param       [in,out] p_priv_key     Output EC private key of key binary format.
 *                                      Set p_data buffer size in len when inputting. <br>
 *                                      At the time of output, set the data in p_data, set the data size <br>
 *                                      in len and return.
 * \param       [in,out] p_pub_key      Output EC public key of key binary format.
 *                                      Set p_data buffer size in len when inputting. <br>
 *                                      At the time of output, set the data in p_data, set the data size <br>
 *                                      in len and return.
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
 *    - #R_PKCD_Initialize has been called.
 * \par Detailed description
 *    - Convert the key data of the input byte array to the key binary format
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Check the arguments with #r_pkcd_ec_chk_key_wrap_arg, and set the return value into ret.
 *  -# If (#PKCD_RET_OK == ret)
 *    -# Set (#st_pkcd_ec_key_bin_t*) p_priv_key to p_priv_key_bin.
 *    -# Set (#st_pkcd_ec_key_bin_t*) p_pub_key to p_pub_key_bin.
 *    -# Set curve length to p_private_key_bin->key_length.
 *    -# Copy p_priv_key_raw->p_data to p_priv_key_bin->p_d with #r_pkcd_cmn_cpy_word.
 *    -# Set curve length to p_priv_key_bin->key_length.
 *    -# Copy p_pub_key_raw->p_data[curve length * #PKCD_EC_POINT_DATA_IDX_X] to p_priv_key_bin->p_qx
 *       with #r_pkcd_cmn_cpy_word.
 *    -# Copy p_pub_key_raw->p_data[curve length * #PKCD_EC_POINT_DATA_IDX_Y] to p_priv_key_bin->p_qy
 *       with #r_pkcd_cmn_cpy_word.
 *    -# Set curve length to p_pub_key_bin->key_length.
 *    -# Copy p_pub_key_raw->p_data[curve length * #PKCD_EC_POINT_DATA_IDX_X] to p_pub_key_bin->p_qx
 *       with #r_pkcd_cmn_cpy_word.
 *    -# Copy p_pub_key_raw->p_data[curve length * #PKCD_EC_POINT_DATA_IDX_Y] to p_pub_key_bin->p_qy
 *       with #r_pkcd_cmn_cpy_word.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_key_wrap(const st_pkc_ec_ctx_t* const p_ctx,
                            const st_pkc_data_t* const p_priv_key_raw,
                            const st_pkc_data_t* const p_pub_key_raw,
                            void* const p_priv_key,
                            void* const p_pub_key)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_pkcd_ec_key_bin_t * p_priv_key_bin;
    st_pkcd_ec_key_bin_t * p_pub_key_bin;
    uint32_t curve_len;

    ret = r_pkcd_ec_chk_key_wrap_arg(p_ctx, p_priv_key_raw, p_pub_key_raw, p_priv_key, p_pub_key);
    if (PKCD_RET_OK == ret)
    {
        p_priv_key_bin = (st_pkcd_ec_key_bin_t*)p_priv_key; /* No problem with casting to pointer size */
        p_pub_key_bin  = (st_pkcd_ec_key_bin_t*)p_pub_key;  /* No problem with casting to pointer size */
        curve_len = p_ctx->p_curve_info->curve_len;

        /* Wrap private key */
        p_priv_key_bin->key_length = curve_len;
        r_pkcd_cmn_cpy_word(p_priv_key_bin->p_d, p_priv_key_raw->p_data, curve_len);
        r_pkcd_cmn_cpy_word(p_priv_key_bin->p_qx, &p_pub_key_raw->p_data[curve_len * PKCD_EC_POINT_DATA_IDX_X],
                            curve_len);
        r_pkcd_cmn_cpy_word(p_priv_key_bin->p_qy, &p_pub_key_raw->p_data[curve_len * PKCD_EC_POINT_DATA_IDX_Y],
                            curve_len);

        /* Wrap Public key */
        p_pub_key_bin->key_length = curve_len;
        r_pkcd_cmn_cpy_word(p_pub_key_bin->p_qx, &p_pub_key_raw->p_data[curve_len * PKCD_EC_POINT_DATA_IDX_X],
                            curve_len);
        r_pkcd_cmn_cpy_word(p_pub_key_bin->p_qy, &p_pub_key_raw->p_data[curve_len * PKCD_EC_POINT_DATA_IDX_Y],
                            curve_len);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcd_ec_key_wrap()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_prepare_operation
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Prepare calculate operation.
 * \param       [in]  p_curve         Curve domain parameters.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_WWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPW
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0STC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Setting length informations.
 *  - Set the value of #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) and #PKCD_CMN_ICUM_PKCCB0DPW
 *  - Calculate PKCC modulus dependant value T.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Setting length info with the #r_pkcd_ec_set_len_info
 * -# Set DL, BDL with the #r_pkcd_cmn_set_dl
 * -# Set #PKCD_EC_WWORK to #PKCD_CMN_ICUM_PKCCB0DPW
 * -# Calculate T with #r_pkcd_ec_calc_t
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_prepare_operation(const st_ec_curve_t* const p_curve)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* prepare length info */
    r_pkcd_ec_set_len_info(p_curve->len);

    /* set DL */
    r_pkcd_cmn_set_dl(p_curve->len, p_curve->len);

    /* set DPW */
    PKCD_CMN_ICUM_PKCCB0DPW = PKCD_EC_FORMAT_DP_DPW(PKCD_EC_WWORK);

    /* Calculate T */
    ret = r_pkcd_ec_calc_t(p_curve->p_order, p_curve->p_field, p_curve->len);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_prepare_operation()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_inverse
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Calculate the inverse for field p or order n.
 * \param       [in]  p_wk_k        An address of work area where k is set. The data must be aligned with bottom.
 * \param       [out] p_wk_invk     An address of work area where k^(-1) is resulting. The data is set at the bottom.
 * \param       [in]  p_wk1         An address of work area.
 * \param       [in]  is_mod_p      If use field p as modulus, then PKC_BOOL_TRUE otherwise use order n.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_PDT_P, #PKCD_EC_PDT_N, #PKCD_EC_PWORK, #PKCD_EC_NWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EC_PDT_P, #PKCD_EC_PDT_N, #PKCD_EC_PWORK and #PKCD_EC_NWORK must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) and #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *  - Calculate the inverse of field p or order n using the Fermat's small theorem.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If PKC_BOOL_TRUE == is_mod_p then
 *   -# Set #PKCD_EC_PDT_P to #PKCD_CMN_ICUM_PKCCB0DPT.
 *   -# Set #PKCD_EC_PWORK to wk_mod.
 * -# Else
 *   -# Set #PKCD_EC_PDT_N to #PKCD_CMN_ICUM_PKCCB0DPT.
 *   -# Set #PKCD_EC_NWORK to wk_mod.
 * -# Calculate the k inversion with #r_pkcd_cmn_modinv_fermat
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_inverse(const uint8_t* const p_wk_k, uint8_t* const p_wk_invk, const uint8_t* const p_wk1,
                                    const e_pkc_bool_t is_mod_p)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const uint8_t * p_wk_mod;

    /* set PDW/T bottom address */
    PKCD_CMN_ICUM_PKCCB0DPT = (PKC_BOOL_TRUE == is_mod_p) ?
                        PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_P) : PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_N);
    p_wk_mod = (PKC_BOOL_TRUE == is_mod_p) ? PKCD_EC_PWORK : PKCD_EC_NWORK;

    /* operate modular inversion */
    ret = r_pkcd_cmn_modinv_fermat(p_wk_k, p_wk_mod, p_wk_invk, p_wk1, PKCD_EC_WORKUNIT);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_inverse()
**********************************************************************************************************************/
/**********************************************************************************************************************
* Function Name : r_pkcd_ec_scalar_mul
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Perform scalar multiplication for points on elliptic curves.
 * \param       [in]  p_x            Affine cordinate x address
 * \param       [in]  p_y            Affine cordinate y address
 * \param       [in]  p_scalar       Scalar value address
 * \param       [in]  p_coeff1       An address of coefficient a of curve domain
 * \param       [in]  curve_len      A byte size of curve length.
 * \param       [in]  sync_option    Indicate if the function returns while PKCC is running.
 * \param       [out] p_wk_xyz_out   An address of work area where Point (XR, YR, ZR) is resulting. <br>
 *                                   The data is set at the bottom.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_OFFSET_DL64VAL, #PKCD_EC_PDT_P, #PKCD_EC_PWORK, #PKCD_EC_WORK1, #PKCD_EC_WORK2,
 *    #PKCD_EC_WORK7, #PKCD_EC_R2MOD, #PKCD_EC_SCALARWORK, #PKCD_EC_ARPWORK, #PKCD_EC_XYZWORKEXT
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_OFFSET_DL64VAL, #PKCD_EC_PDT_P and #PKCD_EC_PWORK, must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) and #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *  - Perform scalar multiplication for points on elliptic curves.
 *  - Resulting coordinate (XR, YR, ZR) are set in wk_xyx_out.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set data pointer of arp_data.
 * -# Set #PKCD_EC_PDT_P to #PKCD_CMN_ICUM_PKCCB0DPT.
 * -# Converte affine coordinates to projective coordinates with the #r_pkcd_ec_affine_to_projective <br>
 *    R^2 mod p is also calculated within this function.
 * -# Calculate a*R mod p with S = ABR^(-1) mod C
 * -# <B>--- If #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 * -# If (#PKCD_EC_CURVE_NIST_P521_LEN != curve_len), then
 *   -# Set DL, BDL to curve_len + 64 bits with the #r_pkcd_cmn_set_dl.
 * -# <B>--- Endif #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 * -# Calculate scalar * (x*R, y*R, R) with mode #PKCD_CMN_MODE_PNTSCALARMUL of #r_pkcd_cmn_run
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_scalar_mul(const uint8_t* const p_x, const uint8_t* const p_y,
                                        const uint8_t* const p_scalar, const uint8_t* const p_coeff1,
                                        const uint32_t curve_len,  const e_pkc_sync_opt_t sync_option,
                                        const st_ec_work_xyz_t* const p_wk_xyz_out)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ec_arp_data_t arp_data;

    arp_data.p_dl64_ar = ((uint8_t*)PKCD_EC_ARPWORK->arp) + /* No problem with casting to pointer size */
                            (PKCD_EC_WORKARP - (PKCD_EC_DL64BYTES * PKCD_CMN_MUL_2));
    arp_data.p_dl64_p  = arp_data.p_dl64_ar + PKCD_EC_DL64BYTES;

    /* set PDT */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_P);

    /* affine coordinates to projective coordinates (r_pkcd_ec_affine_to_projective clears WORK3~5) */
    /* When PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U, the function below set PKCD_EC_R2MOD to (R^2)*(2^64) mod p */
    ret = r_pkcd_ec_affine_to_projective(p_x, p_y, curve_len, PKCD_EC_XYZWORKEXT);

    /* calculate a*R mod p with S = ABR^(-1) mod C mode */
    /* When PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U, calculate a*R*2^64 mod p */
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK1 + PKCD_EC_OFFSET_VAL, p_coeff1, curve_len);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_R2MOD, PKCD_EC_PWORK, PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(arp_data.p_dl64_ar + PKCD_EC_OFFSET_DL64VAL,
                                PKCD_EC_WORK2 + PKCD_EC_OFFSET_VAL, curve_len);
    r_pkcd_cmn_cpy_by_64bounds(arp_data.p_dl64_p + PKCD_EC_OFFSET_DL64VAL,
                                PKCD_EC_PWORK + PKCD_EC_OFFSET_VAL, curve_len);

#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
    if (PKCD_EC_CURVE_NIST_P521_LEN != curve_len)
    {
        /* Set DL to (curve length + 64) for ECC operations, unless NIST P-521 curve is used. */
        r_pkcd_cmn_set_dl(curve_len + PKCD_CMN_BITS64, curve_len + PKCD_CMN_BITS64);
    }
    else
    {
        /* Do nothing */
    }
#endif

    /* calculate scalar_d * (x*R, y*R, R) */
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_SCALARWORK + PKCD_EC_OFFSET_VAL, p_scalar, curve_len);
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EC_FORMAT_DP_XYZ(PKCD_EC_XYZWORKEXT);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EC_FORMAT_DP_UNIT(PKCD_EC_SCALARWORK);
    PKCD_CMN_ICUM_PKCCB0DPC = PKCD_EC_FORMAT_DP_ARP(PKCD_EC_ARPWORK);
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EC_FORMAT_DP_XYZ(p_wk_xyz_out);/* No problem with casting
                                                                    st_ec_work_xyz_out* to uint8_t* */
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_PNTSCALARMUL, sync_option);

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_scalar_mul()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_scalar_mul_sec
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Perform scalar multiplication for points on elliptic curves with the additive splitting
 *              and randomizing projective coordinates.
 * \param       [in]  p_prm          An address of a structure that contains coordinates of points on the elliptic
 *                                   curve, a scalar, and two random data used in scalar multiplication
 *                                   with the additive splitting and randomizing the projective coordinates.
 * \param       [in]  p_coeff1       An address of coefficient a of curve domain
 * \param       [in]  curve_len      A byte size of curve length.
 * \param       [out] p_wk_xyz_out   An address of work area where the resulting point is palaced. <br>
 *                                   The data is set at the bottom.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_OFFSET_DL64VAL, #PKCD_EC_PDT_P, #PKCD_EC_PDT_N, #PKCD_EC_PWORK, #PKCD_EC_WORK1,
 *    #PKCD_EC_WORK2, #PKCD_EC_WORK3, #PKCD_EC_WORK6, #PKCD_EC_R2MOD, #PKCD_EC_SCALARWORK, #PKCD_EC_ARPWORK,
 *    #PKCD_EC_XYZWORKEXT
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_OFFSET_DL64VAL, #PKCD_EC_PDT_P, #PKCD_EC_PDT_N and #PKCD_EC_PWORK
 *    must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) and #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *  - Perform scalar multiplication for points on elliptic curves.
 *  - Resulting point (XR, YR, ZR) are set in wk_xyx_out.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Initialize the variable mod_size with the curvel length rounded up to a multiple of 8.
 * -# Set data pointer of arp_data.
 * -# Set ##PKCD_CMN_ICUM_PKCCB0DPT to #PKCD_EC_PDT_N.
 * -# Calculate R^2 mod n with #r_pkcd_cmn_R2ModC.
 * -# Calculate p_prm->random_data (hereinafter referred to as rand) mod n with #r_pkcd_cmn_reduce_small_modulus,
 *    and add the return value to ret.
 * -# Calculate -rand mod n with r_pkcd_cmn_AMinusBModC.
 * -# Calculate (rand + scalar) mod n with #r_pkcd_cmn_APlusBModC.
 * -# Set #PKCD_CMN_ICUM_PKCCB0DPT to #PKCD_EC_PDT_P.
 * -# Converte affine coordinates to projective coordinates with the #r_pkcd_ec_affine_to_proj_sec <br>
 *    R^2 mod p is also calculated within this function.
 * -# Calculate a*R mod p with S = ABR^(-1) mod C.
 * -# <B>--- If #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 * -# If (#PKCD_EC_CURVE_NIST_P521_LEN != curve_len), then
 *   -# Set DL, BDL to curve_len + 64 bits with the #r_pkcd_cmn_set_dl.
 * -# <B>--- Endif #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 * -# Calculate (-rand mod n) * (x*Z*R, y*Z*R, Z*R) with mode #PKCD_CMN_MODE_PNTSCALARMUL of #r_pkcd_cmn_run.
 * -# Calculate [(scalar + rand) mod n]  * (x*Z*R, y*Z*R, R) with mode #PKCD_CMN_MODE_PNTSCALARMUL of #r_pkcd_cmn_run.
 * -# Calculate (-rand mod n) * (x*Z*R, y*Z*R, Z*R) + [(scalar + rand) mod n] * (x*Z*R, y*Z*R, Z*R)
 *    with #r_pkcd_ec_point_add.
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_scalar_mul_sec(const st_ec_prm_smult_t* const p_prm, const uint8_t* const p_coeff1,
                                        const uint32_t curve_len, const st_ec_work_xyz_t* const p_wk_xyz_out)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ec_arp_data_t arp_data;
    const uint32_t mod_size = ((curve_len + PKCD_CMN_ADD_7) >> PKCD_CMN_SHIFT_3) << PKCD_CMN_SHIFT_3;

    arp_data.p_dl64_ar = ((uint8_t*)PKCD_EC_ARPWORK->arp) + /* No problem with casting to pointer size */
                            (PKCD_EC_WORKARP - (PKCD_EC_DL64BYTES * PKCD_CMN_MUL_2));
    arp_data.p_dl64_p  = arp_data.p_dl64_ar + PKCD_EC_DL64BYTES;

    /* set PDT */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_N);

    /* calculate R^2 mod n */
    ret = r_pkcd_cmn_R2ModC(PKCD_EC_NWORK, PKCD_EC_R2MOD, PKCD_EC_WORKUNIT);

    /* calculate random mod n */
    ret |= r_pkcd_cmn_reduce_small_modulus(p_prm->random_data, PKCD_EC_NWORK, PKCD_EC_R2MOD,
                                        PKCD_CMN_MUL_2 * curve_len, mod_size, PKCD_EC_WORKUNIT, PKCD_EC_WORK1,
                                        PKCD_EC_WORK2, PKCD_EC_WORK1);

    /* calculate -random mod n, and save it in PKCD_EC_WORK6 */
    ret |= r_pkcd_cmn_clr_work_unit(PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    ret |= r_pkcd_cmn_AMinusBModC(PKCD_EC_WORK2, PKCD_EC_WORK1, PKCD_EC_NWORK, PKCD_EC_WORK6, PKCD_EC_WORKUNIT);

    /* calculate (random + scalar) mod n, and save it in PKCD_EC_SCALARWORK */
    ret |= r_pkcd_cmn_clr_work_unit(PKCD_EC_SCALARWORK, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_SCALARWORK + PKCD_EC_OFFSET_VAL, p_prm->scalar.scalar, curve_len);
    ret |= r_pkcd_cmn_APlusBModC(PKCD_EC_WORK1, PKCD_EC_SCALARWORK, PKCD_EC_NWORK, PKCD_EC_SCALARWORK,
                                PKCD_EC_WORKUNIT);

    /* set PDT */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_P);

    /* affine coordinates to projective coordinates */
    /* When PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U, the function below set PKCD_EC_R2MOD to (R^2)*(2^64) mod p */
    ret |= r_pkcd_ec_affine_to_proj_sec(p_prm->point.x, p_prm->point.y, curve_len, p_prm->random_z, PKCD_EC_XYZWORK1);

    /* Copy -random mod n to PKCD_EC_WORK3  */
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK3 + PKCD_EC_OFFSET_VAL, PKCD_EC_WORK6 + PKCD_EC_OFFSET_VAL, curve_len);

    /* calculate a*R mod p with S = ABR^(-1) mod C mode */
    ret |= r_pkcd_cmn_clr_work_unit(PKCD_EC_WORK1, PKCD_EC_WORKUNIT);
    ret |= r_pkcd_cmn_clr_work_unit(PKCD_EC_WORK6, PKCD_EC_WORKUNIT);
#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
    ret |= r_pkcd_cmn_clr_work_unit(PKCD_EC_WORK7, PKCD_EC_WORKUNIT);
#endif
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK1 + PKCD_EC_OFFSET_VAL, p_coeff1, curve_len);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_R2MOD, PKCD_EC_PWORK, PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(arp_data.p_dl64_ar + PKCD_EC_OFFSET_DL64VAL,
                                PKCD_EC_WORK2 + PKCD_EC_OFFSET_VAL, curve_len);
    r_pkcd_cmn_cpy_by_64bounds(arp_data.p_dl64_p + PKCD_EC_OFFSET_DL64VAL,
                                PKCD_EC_PWORK + PKCD_EC_OFFSET_VAL, curve_len);

#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
    if (PKCD_EC_CURVE_NIST_P521_LEN != curve_len)
    {
        /* Set DL to (curve length + 64) for ECC operations, unless NIST P-521 curve is used. */
        r_pkcd_cmn_set_dl(curve_len + PKCD_CMN_BITS64, curve_len + PKCD_CMN_BITS64);
    }
    else
    {
        /* Do nothing */
    }
#endif

    /* calculate (-random mod n) * (x*R, y*R, R) */
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EC_FORMAT_DP_XYZ(PKCD_EC_XYZWORK1);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EC_FORMAT_DP_UNIT(PKCD_EC_WORK3);
    PKCD_CMN_ICUM_PKCCB0DPC = PKCD_EC_FORMAT_DP_ARP(PKCD_EC_ARPWORK);
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EC_FORMAT_DP_XYZ(PKCD_EC_XYZWORK2);
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_PNTSCALARMUL, PKC_SYNC_OPT_SYNC);

    /* calculate [(k + r) mod n] * (x*R, y*R, R) */
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EC_FORMAT_DP_UNIT(PKCD_EC_SCALARWORK);
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EC_FORMAT_DP_XYZ(PKCD_EC_XYZWORKEXT);
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_PNTSCALARMUL, PKC_SYNC_OPT_SYNC);

    /* calculate (-random mod n) * (x*R, y*R, R) + [(k + r) mod n] * (x*R, y*R, R) */
    ret |= r_pkcd_ec_point_add(PKCD_EC_XYZWORK2, PKCD_EC_XYZWORKEXT, p_wk_xyz_out);

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_scalar_mul_sec()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_point_add
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Add points on an elliptic curve
 * \param       [in]  p_wk_xyz_a          An address of work area where Point A(XR, YR, ZR) is set. <br>
 *                                        The data must be aligned with bottom.
 * \param       [in]  p_wk_xyz_b          An address of work area where Point B(XR, YR, ZR) is set. <br>
 *                                        The data must be aligned with bottom.
 * \param       [out] p_wk_xyz_c          An address of work area where Point C(XR, YR, ZR) is resulting. <br>
 *                                        The data is set at the bottom.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_PDT_P, #PKCD_EC_PWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EC_PDT_P and #PKCD_EC_PWORK must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) and #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *  - Add points on an elliptic curve.
 *  - Resulting coordinate (X'R, Y'R, Z'R) are set in wk_xyz_c.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set #PKCD_EC_PDT_P to #PKCD_CMN_ICUM_PKCCB0DPT.
 * -# Calculate C(XR, YR, ZR) = A(XR, YR, ZR) + B(XR, YR, ZR) with mode #PKCD_CMN_MODE_PNTADD of #r_pkcd_cmn_run
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_point_add(const st_ec_work_xyz_t* const p_wk_xyz_a,
                                        const st_ec_work_xyz_t* const p_wk_xyz_b,
                                        const st_ec_work_xyz_t* const p_wk_xyz_c)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* set PDT */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_P);

    /* calculate C(XR, YR, ZR) = A(XR, YR, ZR) + B(XR, YR, ZR) */
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_EC_FORMAT_DP_XYZ(p_wk_xyz_a); /* Set PDA */
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_EC_FORMAT_DP_XYZ(p_wk_xyz_b); /* Set PDB */
    PKCD_CMN_ICUM_PKCCB0DPC = PKCD_EC_FORMAT_DP_UNIT(PKCD_EC_PWORK); /* Set PDC */
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_EC_FORMAT_DP_XYZ(p_wk_xyz_c); /* Set PDS */

    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_PNTADD, PKC_SYNC_OPT_SYNC);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_point_add()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_make_sign
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       ECDSA signature generation.
 * \param       [in]  p_digest          An Address of the digest.
 * \param       [in]  digest_len        A byte size of the digest.
 * \param       [in]  p_q_x             An address of temporary public key x.
 * \param       [in]  p_random          An address of random scalar k.
 * \param       [in]  p_priv_key_scalar An address of private key scalar d.
 * \param       [in]  p_order           An address of order n.
 * \param       [in]  curve_len         A byte size of curve length.
 * \param       [out] p_sign            An address of structure which contains addresses of the signature (r, s)
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 *  - #PKCD_RET_EC_ZERO_SIGNATURE:   Make zero signature.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_PDT_P, #PKCD_EC_PDT_N, #PKCD_EC_PWORK, #PKCD_EC_NWORK, #PKCD_EC_WORK1,
 *    #PKCD_EC_WORK2, #PKCD_EC_WORK3, #PKCD_EC_WORK4, #PKCD_EC_WORK5, #PKCD_EC_WORK6, #PKCD_EC_WORK7,
 *    #PKCD_EC_R2MOD, #PKCD_EC_SCALARWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_PDT_P, #PKCD_EC_PDT_N, #PKCD_EC_PWORK, #PKCD_EC_NWORK must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) and #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *  - ECDSA signature generation. <br>
 *  - The random scalar uses the value of modulus n.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Initialize the variable ret2 with #PKCD_RET_INTERNALERR.
 * -# Set #PKCD_EC_PDT_N to #PKCD_CMN_ICUM_PKCCB0DPT.
 * -# Calculate R^2 mod n with the #r_pkcd_cmn_R2ModC.
 * -# Calculate x * R mod n with the #r_pkcd_cmn_ABRInvModC.
 * -# Calculate r = x mod n with the #r_pkcd_cmn_ARInvModC.
 * -# Copy r to r area of p_sign with the #r_pkcd_cmn_cpy_by_64bounds
 * -# Check the r is zero with #r_pkcd_cmn_cmp_zero_word and set the result to cmp_ret.
 * -# If 0 != cmp_ret then
 *   -# Set ret2 to #PKCD_RET_OK.
 * -# Else
 *   -# Set ret2 to #PKCD_RET_EC_ZERO_SIGNATURE.
 * -# Calculate r * R mod n with the #r_pkcd_cmn_ABRInvModC.
 * -# Calculate r * d mod n with the #r_pkcd_cmn_ABRInvModC.
 * -# Calculate (z + r * d) mod n with the #r_pkcd_cmn_APlusBModC.
 * -# Calculate the random scalar k inversion with #r_pkcd_ec_inverse
 * -# Calculate R * k^(-1) mod n with the #r_pkcd_cmn_ABRInvModC.
 * -# Set the upper n bits of digest (= z) to work area with the #r_pkcd_ec_set_digest_to_work.
 * -# Calculate (z + r * d) mod n with the #r_pkcd_cmn_APlusBModC.
 * -# Calculate k^(-1) mod n with the #r_pkcd_ec_inverse.
 * -# Calculate R * k^(-1) mod n with the #r_pkcd_cmn_ABRInvModC.
 * -# Calculate s = (z + r * d) * k^(-1) mod n with the #r_pkcd_cmn_ABRInvModC.
 * -# Copy s to s area of p_sign with the #r_pkcd_cmn_cpy_by_64bounds
 * -# Check the s is zero with #r_pkcd_cmn_cmp_zero_word and set the result to cmp_ret.
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# If 0 != cmp_ret then
 *   -# Set ret2 to #PKCD_RET_OK.
 * -# Else
 *   -# Set ret2 to #PKCD_RET_EC_ZERO_SIGNATURE.
 * -# If #PKCD_RET_OK == ret then
 *   -# If #PKCD_RET_EC_ZERO_SIGNATURE != ret2 then
 *     -# Set curve_len to p_sign->len * #PKCD_EC_SIGN_DATA_NUM.
 *   -# Else
 *     -# Set ret to #PKCD_RET_EC_ZERO_SIGNATURE.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_make_sign(const uint8_t* const p_digest, const uint32_t digest_len,
                                        const uint8_t* const p_q_x, const uint8_t* const p_random,
                                        const uint8_t* const p_priv_key_scalar, const uint8_t* const p_order,
                                        const uint32_t curve_len, st_pkc_data_t* const p_sign)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    pkcd_ret_t ret2 = PKCD_RET_INTERNALERR;
    int32_t cmp_ret;
    uint8_t * const p_sign_r = &p_sign->p_data[curve_len * PKCD_EC_SIGN_DATA_IDX_R];
    uint8_t * const p_sign_s = &p_sign->p_data[curve_len * PKCD_EC_SIGN_DATA_IDX_S];

    /* Set DT */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_N);

    /* calculate R^2 mod n */
    r_pkcd_cmn_ram_clr(PKCD_EC_R2MOD, PKCD_EC_WORKUNIT);
    ret = r_pkcd_cmn_R2ModC(PKCD_EC_NWORK, PKCD_EC_R2MOD, PKCD_EC_WORKUNIT);

    /* Calculate r = x mod n */
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK1, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK3 + PKCD_EC_OFFSET_VAL, p_q_x, curve_len);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK3, PKCD_EC_R2MOD, PKCD_EC_NWORK, PKCD_EC_WORK1, PKCD_EC_WORKUNIT);

    r_pkcd_cmn_ram_clr(PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    ret |= r_pkcd_cmn_ARInvModC(PKCD_EC_WORK1, PKCD_EC_NWORK, PKCD_EC_WORK2, PKCD_EC_WORKUNIT);

    /* check zero */
    r_pkcd_cmn_cpy_by_64bounds(p_sign_r, PKCD_EC_WORK2 + PKCD_EC_OFFSET_VAL, curve_len);
    cmp_ret = r_pkcd_cmn_cmp_zero_word(p_sign_r, curve_len);
    if (0 != cmp_ret)
    {
        ret2 = PKCD_RET_OK;
    }
    else
    {
        ret2 = PKCD_RET_EC_ZERO_SIGNATURE;
    }

    /* Calculate rR mod n */
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK3, PKCD_EC_WORKUNIT);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_R2MOD, PKCD_EC_WORK2, PKCD_EC_NWORK, PKCD_EC_WORK3, PKCD_EC_WORKUNIT);

    /* Calculate r * d mod n = rR * d * R^(-1) mod n */
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK4, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_SCALARWORK + PKCD_EC_OFFSET_VAL,  p_priv_key_scalar, curve_len);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK3, PKCD_EC_SCALARWORK, PKCD_EC_NWORK, PKCD_EC_WORK4, PKCD_EC_WORKUNIT);

    /* Calculate (z + r * d) mod n */
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK5, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK6, PKCD_EC_WORKUNIT);
    ret |= r_pkcd_ec_set_digest_to_work(p_digest, digest_len, p_order, curve_len, PKCD_EC_WORK5);
    ret |= r_pkcd_cmn_APlusBModC(PKCD_EC_WORK5, PKCD_EC_WORK4, PKCD_EC_NWORK, PKCD_EC_WORK6, PKCD_EC_WORKUNIT);

    /* Calculate k^(-1) mod n */
    r_pkcd_cmn_ram_clr(PKCD_EC_SCALARWORK, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_SCALARWORK + PKCD_EC_OFFSET_VAL, p_random, curve_len);
    ret |= r_pkcd_ec_inverse(PKCD_EC_SCALARWORK, PKCD_EC_WORK7, PKCD_EC_WORK1, PKC_BOOL_FALSE);

    /* Calculate R * k^(-1) mod n */
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK3, PKCD_EC_WORKUNIT);
    ret |=  r_pkcd_cmn_ABRInvModC(PKCD_EC_R2MOD, PKCD_EC_WORK7, PKCD_EC_NWORK, PKCD_EC_WORK3, PKCD_EC_WORKUNIT);

    /* Calculate s = (z + r * d) * k^(-1) mod n  */
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK4, PKCD_EC_WORKUNIT);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK6, PKCD_EC_WORK3, PKCD_EC_NWORK, PKCD_EC_WORK4, PKCD_EC_WORKUNIT);

    /* check zero */
    r_pkcd_cmn_cpy_by_64bounds(p_sign_s, PKCD_EC_WORK4 + PKCD_EC_OFFSET_VAL, curve_len);
    cmp_ret = r_pkcd_cmn_cmp_zero_word(p_sign_s, curve_len);
    if (0 == cmp_ret)
    {
        ret2 = PKCD_RET_EC_ZERO_SIGNATURE;
    }
    else
    {
        /* Do nothing */
    }

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    if (PKCD_RET_OK == ret)
    {
        if (PKCD_RET_EC_ZERO_SIGNATURE != ret2)
        {
            /* make signature successful */
            p_sign->len = curve_len * PKCD_EC_SIGN_DATA_NUM;
        }
        else
        {
            /* make signature fail */
            ret = PKCD_RET_EC_ZERO_SIGNATURE;
        }
    }
    else
    {
        /*Do nothing */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_make_sign()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_affine_to_projective
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Convert from affine coordinates to projective coordinates.
 * \param       [in]  p_x            An address of affine x-coordinate.
 * \param       [in]  p_y            An address of affine y-coordinate.
 * \param       [in]  curve_len      A byte size of curve length.
 * \param       [out] p_wk_xyz_out   An address of work area where projective coordinates after conversion are set.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_OFFSET_DL64VAL, #PKCD_EC_PDT_P, #PKCD_EC_PWORK, #PKCD_EC_R2MOD, #PKCD_EC_WORK1,
 *    #PKCD_EC_WORK2, #PKCD_EC_XYZWORKEXT
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
* \par Precondition
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_OFFSET_DL64VAL, #PKCD_EC_PDT_P and #PKCD_EC_PWORK must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) and #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *  - Convert the affine coordinates of (x, y) to projective coordinates of (x*R, y*R, R).
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set data pointer of p_wk_xyz_out
 * -# Set #PKCD_EC_PDT_P to #PKCD_CMN_ICUM_PKCCB0DPT.
 * -# Calculate R^2 mod p with the #r_pkcd_cmn_R2ModC.
 * -# <B>--- If #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 * -# If (#PKCD_EC_CURVE_NIST_P521_LEN != curve_len), then
 *   -# Calculate R^3 mod p with #r_pkcd_cmn_ABRInvModC.
 *   -# Set #PKCD_EC_WORK3 to 2^64 with #r_pkcd_cmn_cpy_by_64bounds.
 *   -# Calculate (R^2) * (2^64) mod p with #r_pkcd_cmn_ABRInvModC.
 * -# <B>--- Endif #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 * -# Calculate x*R (or x*R*2^64 if if #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1
 *    and the curve length < #PKCD_EC_CURVE_LEN_MAX)
 *    with the #r_pkcd_cmn_ABRInvModC.
 *    Copy the calculation result to x area of p_wk_xyz_out with #r_pkcd_cmn_cpy_by_64bounds.
 * -# Calculate y*R (or y*R*2^64 if if #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1
 *    and the curve length < #PKCD_EC_CURVE_LEN_MAX)
 *    with the #r_pkcd_cmn_ABRInvModC.
 *    Copy the calculation result to y area of p_wk_xyz_out with #r_pkcd_cmn_cpy_by_64bounds.
 * -# Calculate R (or R*2^64 if if #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1
 *    and the curve length < #PKCD_EC_CURVE_LEN_MAX)
 *    with the #r_pkcd_cmn_ARInvModC.
 *    Copy the calculation result to z area of p_wk_xyz_out with #r_pkcd_cmn_cpy_by_64bounds.
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_affine_to_projective(const uint8_t* const p_x, const uint8_t* const p_y,
                                                    const uint32_t curve_len, st_ec_work_xyz_t* const p_wk_xyz_out)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ec_xyz_data_t xyz_data;
#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
    const uint32_t one = r_pkcd_cmn_ch_endian32(0x00000001U);
#endif

    xyz_data.p_dl64_x = (uint8_t*)p_wk_xyz_out->xyz /* No problem with casting to pointer size */
                        + (PKCD_EC_WORKXYZ - (PKCD_EC_DL64BYTES * PKCD_CMN_MUL_3));
    xyz_data.p_dl64_y = xyz_data.p_dl64_x + PKCD_EC_DL64BYTES;
    xyz_data.p_dl64_z = xyz_data.p_dl64_y + PKCD_EC_DL64BYTES;

    /* set PDT */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_P);

    /* calculate R^2 mod p */
    ret = r_pkcd_cmn_R2ModC(PKCD_EC_PWORK, PKCD_EC_R2MOD, PKCD_EC_WORKUNIT);

#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
    if (PKCD_EC_CURVE_NIST_P521_LEN != curve_len)
    {
        /* calculate R^3 mod p */
        ret |= r_pkcd_cmn_clr_work_unit(PKCD_EC_WORK3, PKCD_EC_WORKUNIT);
        ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_R2MOD, PKCD_EC_R2MOD, PKCD_EC_PWORK, PKCD_EC_R2MOD, PKCD_EC_WORKUNIT);

        /* Set PKCD_EC_WORK3 to 2^64. No problem for casting to support usual arithmetic conversion. */
        r_pkcd_cmn_cpy_by_64bounds((uint8_t*)((uintptr_t)PKCD_EC_WORK3
                                    + (PKCD_EC_WORKUNIT - (PKCD_CMN_BITS64 + sizeof(uint32_t)))),
                                    (uint8_t*)&one, sizeof(uint32_t));

        /* calculate R^2 * 2^64 mod p */
        ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_R2MOD, PKCD_EC_WORK3, PKCD_EC_PWORK, PKCD_EC_R2MOD, PKCD_EC_WORKUNIT);
    }
    else
    {
        /* Do nothing */
    }
#endif

    /* set (x*R, y*R, R) */
    /* When PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U and curve_len < 68, set (x*R*2^64, y*R*2^64, R*2^64) */
    r_pkcd_cmn_ram_clr((uint8_t*)PKCD_EC_XYZWORKEXT, PKCD_EC_WORKXYZ);
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK1 + PKCD_EC_OFFSET_VAL, p_x, curve_len);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_R2MOD, PKCD_EC_PWORK, PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(xyz_data.p_dl64_x + PKCD_EC_OFFSET_DL64VAL,
                                PKCD_EC_WORK2 + PKCD_EC_OFFSET_VAL, curve_len);

    r_pkcd_cmn_ram_clr(PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK1 + PKCD_EC_OFFSET_VAL, p_y, curve_len);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_R2MOD, PKCD_EC_PWORK, PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(xyz_data.p_dl64_y + PKCD_EC_OFFSET_DL64VAL,
                                PKCD_EC_WORK2 + PKCD_EC_OFFSET_VAL, curve_len);

    r_pkcd_cmn_ram_clr(PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    ret |= r_pkcd_cmn_ARInvModC(PKCD_EC_R2MOD, PKCD_EC_PWORK, PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(xyz_data.p_dl64_z + PKCD_EC_OFFSET_DL64VAL,
                                PKCD_EC_WORK2 + PKCD_EC_OFFSET_VAL, curve_len);

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_affine_to_projective()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_affine_to_proj_sec
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Convert from affine coordinates to projective coordinates witn random z coordinate.
 * \param       [in]  p_x            An address of affine x-coordinate.
 * \param       [in]  p_y            An address of affine y-coordinate.
 * \param       [in]  curve_len      A byte size of curve length.
 * \param       [in]  p_random       An address of random data used for radomizing projective coordinates.
 * \param       [out] p_wk_xyz_out   An address of work area where projective coordinates after conversion are set.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_OFFSET_DL64VAL, #PKCD_EC_PDT_P, #PKCD_EC_PWORK, #PKCD_EC_R2MOD, #PKCD_EC_WORK1,
 *    #PKCD_EC_WORK2, #PKCD_EC_WORK5, #PKCD_EC_WORK7, #PKCD_EC_XYZWORKEXT
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
* \par Precondition
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_OFFSET_DL64VAL, #PKCD_EC_PDT_P and #PKCD_EC_PWORK must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) and #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *  - Convert the affine coordinates of (x, y) to projective coordinates of (x*Z*R, y*Z*R, Z*R)
 *    where Z is a random value passed by the argument p_random.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set data pointer of p_wk_xyz_out
 * -# Set #PKCD_CMN_ICUM_PKCCB0DPT to #PKCD_EC_PDT_P.
 * -# Calculate R^2 mod p with the #r_pkcd_cmn_R2ModC, and add the return value to ret.
 * -# <B>--- If #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 * -# If (#PKCD_EC_CURVE_NIST_P521_LEN != curve_len), then
 *   -# Calculate R^3 mod p with #r_pkcd_cmn_ABRInvModC, and add the return value to ret.
 *   -# Set #PKCD_EC_WORK3 to 2^64 with #r_pkcd_cmn_cpy_by_64bounds.
 *   -# Calculate (R^2) * (2^64) mod p with #r_pkcd_cmn_ABRInvModC, and add the return value to ret.
 * -# <B>--- Endif #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 * -# Calculate Z*R (or Z*R*2^64 if if #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1
 *    and the curve length < #PKCD_EC_CURVE_LEN_MAX)
 *    with the #r_pkcd_cmn_ABRInvModC, and add the return value to ret.
 *    (Z is a random value passed by the argument p_random.)
 * -# Calculate x*R with the #r_pkcd_cmn_ABRInvModC, and add the return value to ret.
 * -# Calculate x*Z*R (or x*Z*R*2^64 if if #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1
 *    and curve length < #PKCD_EC_CURVE_LEN_MAX)
 *    with the #r_pkcd_cmn_ABRInvModC, and add the returen value to ret.
 *    Copy the calculation result to x area of p_wk_xyz_out with #r_pkcd_cmn_cpy_by_64bounds.
 * -# Calculate y*R with the #r_pkcd_cmn_ABRInvModC, and add the returen value to ret.
 * -# Calculate y*Z*R (or y*Z*R*2^64 if if #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1
 *    and the curve length < #PKCD_EC_CURVE_LEN_MAX)
 *    with the #r_pkcd_cmn_ABRInvModC, and add the returen value to ret.
 *    Copy the calculation result to y area of p_wk_xyz_out with #r_pkcd_cmn_cpy_by_64bounds.
 * -# Copy Z*R (or Z*R*2^64 if if #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1
 *    and the curve length < #PKCD_EC_CURVE_LEN_MAX)
 *    to z area of p_wk_xyz_out with #r_pkcd_cmn_cpy_by_64bounds.
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_affine_to_proj_sec(const uint8_t * const p_x, const uint8_t * const p_y,
                                                const uint32_t curve_len, const uint8_t * const p_random,
                                                st_ec_work_xyz_t * const p_wk_xyz_out)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ec_xyz_data_t xyz_data;
#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
    const uint32_t one = r_pkcd_cmn_ch_endian32(0x00000001U);
#endif

    xyz_data.p_dl64_x = (uint8_t*)p_wk_xyz_out->xyz /* No problem with casting to pointer size */
                        + (PKCD_EC_WORKXYZ - (PKCD_EC_DL64BYTES * PKCD_CMN_MUL_3));
    xyz_data.p_dl64_y = xyz_data.p_dl64_x + PKCD_EC_DL64BYTES;
    xyz_data.p_dl64_z = xyz_data.p_dl64_y + PKCD_EC_DL64BYTES;

    /* set PDT */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_P);

    /* calculate R^2 mod p */
    ret = r_pkcd_cmn_R2ModC(PKCD_EC_PWORK, PKCD_EC_R2MOD, PKCD_EC_WORKUNIT);

#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK7 + PKCD_EC_OFFSET_VAL, PKCD_EC_R2MOD + PKCD_EC_OFFSET_VAL, curve_len);

    if (PKCD_EC_CURVE_NIST_P521_LEN != curve_len)
    {
        /* calculate R^3 mod p */
        ret |= r_pkcd_cmn_clr_work_unit(PKCD_EC_WORK3, PKCD_EC_WORKUNIT);
        ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_R2MOD, PKCD_EC_R2MOD, PKCD_EC_PWORK, PKCD_EC_R2MOD, PKCD_EC_WORKUNIT);

        /* Set PKCD_EC_WORK3 to 2^64. No problem for casting to support usual arithmetic conversion. */
        r_pkcd_cmn_cpy_by_64bounds((uint8_t*)((uintptr_t)PKCD_EC_WORK3
                                    + (PKCD_EC_WORKUNIT - (PKCD_CMN_BITS64 + sizeof(uint32_t)))),
                                    (uint8_t*)&one, sizeof(uint32_t));

        /* calculate R^2 * 2^64 mod p */
        ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_R2MOD, PKCD_EC_WORK3, PKCD_EC_PWORK, PKCD_EC_R2MOD, PKCD_EC_WORKUNIT);
    }
    else
    {
        /* Do nothing */
    }
#endif

    /* Calculate z*R, and save it in PKCD_EC_WORK2 */
    /* When PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U and curve_len < 68, calculate z*R*2^64 */
    r_pkcd_cmn_ram_clr((uint8_t*)PKCD_EC_XYZWORKEXT, PKCD_EC_WORKXYZ);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK1 + PKCD_EC_OFFSET_VAL, p_random, curve_len);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_R2MOD, PKCD_EC_PWORK, PKCD_EC_WORK2, PKCD_EC_WORKUNIT);

    /* set x*z*R */
    /* When PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U and curve_len < 68, set x*z*R*2^64 */
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK1 + PKCD_EC_OFFSET_VAL, p_x, curve_len);
#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_WORK7, PKCD_EC_PWORK, PKCD_EC_WORK1, PKCD_EC_WORKUNIT);
#else
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_R2MOD, PKCD_EC_PWORK, PKCD_EC_WORK1, PKCD_EC_WORKUNIT);
#endif
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_WORK2, PKCD_EC_PWORK, PKCD_EC_WORK5, PKCD_EC_WORKUNIT);

    r_pkcd_cmn_cpy_by_64bounds(xyz_data.p_dl64_x + PKCD_EC_OFFSET_DL64VAL,
                                PKCD_EC_WORK5 + PKCD_EC_OFFSET_VAL, curve_len);

    /* set y*z*R */
    /* When PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U and curve_len < 68, set y*z*R*2^64 */
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK1 + PKCD_EC_OFFSET_VAL, p_y, curve_len);
#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_WORK7, PKCD_EC_PWORK, PKCD_EC_WORK1, PKCD_EC_WORKUNIT);
#else
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_R2MOD, PKCD_EC_PWORK, PKCD_EC_WORK1, PKCD_EC_WORKUNIT);
#endif
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_WORK2, PKCD_EC_PWORK, PKCD_EC_WORK5, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(xyz_data.p_dl64_y + PKCD_EC_OFFSET_DL64VAL,
                                PKCD_EC_WORK5 + PKCD_EC_OFFSET_VAL, curve_len);

    /* Set z*R */
    /* When PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U and curve_len < 68, set z*R*2^64 */
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK5, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(xyz_data.p_dl64_z + PKCD_EC_OFFSET_DL64VAL,
                                PKCD_EC_WORK2 + PKCD_EC_OFFSET_VAL, curve_len);

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_affine_to_proj_sec()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_projective_to_affine
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Convert from projective coordinates to affine coordinates.
 * \param       [in]   p_wk_xyx       An address of work area where projective coordinates.
 * \param       [in]   curve_len      A byte size of curve length.
 * \param       [out]  p_x            An address of the buffer that sets the affine x coordinate after conversion. <br>
 *                                        If NULL is set, do not calculate the value.
 * \param       [out]  p_y            An address of the buffer that sets the affine y coordinate after conversion. <br>
 *                                        If NULL is set, do not calculate the value.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_OFFSET_DL64VAL, #PKCD_EC_PDT_P, #PKCD_EC_R2MOD, #PKCD_EC_WORK1, #PKCD_EC_WORK2,
 *    #PKCD_EC_XYZWORKEXT
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_OFFSET_DL64VAL and #PKCD_EC_PDT_P must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) and #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 *  - If #PKCD_CFG_OVF_WORKAROUND_ENABLE == 1 and the curve length < #PKCD_EC_CURVE_LEN_MAX,
 *    DL must be set to the curve length + 64bits, otherwise, the curve length must be set.
 * \par Detailed description
 *  - Convert the projective coordinates of (XR, YR, ZR) to affine coordinates of (x, y).
 *  - (x = X/Z, y = Y/Z)
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set data pointer of p_wk_xyx
 * -# Set #PKCD_EC_PDT_P to #PKCD_CMN_ICUM_PKCCB0DPT.
 * -# <B>--- If #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 * -# Set DL, BDL to curve_len with the #r_pkcd_cmn_set_dl.
 * -# <B>--- Endif #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 * -# Remove montgomery coefficient R from ZR with the #r_pkcd_cmn_ARInvModC.
 * -# Calculate Z^(-1) with #r_pkcd_ec_inverse.
 * -# <B>--- If #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 * -# If (#PKCD_EC_CURVE_NIST_P521_LEN != curve_len), then
 *   -# Set DL, BDL to curve_len + 64 bits with the #r_pkcd_cmn_set_dl.
 * -# <B>--- Endif #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 * -# If NULL != p_x then
 *   -# Calculate x = X/Z with the #r_pkcd_cmn_ABRInvModC.
 *   -# Copy x to p_x with the #r_pkcd_cmn_cpy_by_64bounds
 * -# If NULL != p_y then
 *   -# Calculate y = Y/Z with the #r_pkcd_cmn_ABRInvModC.
 *   -# Copy y to p_y with the #r_pkcd_cmn_cpy_by_64bounds
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_projective_to_affine(const st_ec_work_xyz_t* const p_wk_xyx, const uint32_t curve_len,
                                                    uint8_t* const p_x, uint8_t* const p_y)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ec_xyz_data_t xyz_data;

    /* No problem with casting to pointer size */
    xyz_data.p_dl64_x = (uint8_t*)p_wk_xyx->xyz +
                            (PKCD_EC_WORKXYZ - (PKCD_EC_DL64BYTES * PKCD_CMN_MUL_3));
    xyz_data.p_dl64_y = xyz_data.p_dl64_x + PKCD_EC_DL64BYTES;
    xyz_data.p_dl64_z = xyz_data.p_dl64_y + PKCD_EC_DL64BYTES;

    /* set PDT */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_P);

    /* remove montgomery coefficient R from ZR */
    /* When PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U and curve_len < 68, remove R*2^64 from Z*R*2^64 */
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK3, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK1 + PKCD_EC_OFFSET_VAL,
                                xyz_data.p_dl64_z + PKCD_EC_OFFSET_DL64VAL, curve_len);
    ret = r_pkcd_cmn_ARInvModC(PKCD_EC_WORK1, PKCD_EC_PWORK, PKCD_EC_WORK3, PKCD_EC_WORKUNIT);

#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
    /* Set dl to curve_len */
    r_pkcd_cmn_set_dl(curve_len, curve_len);
#endif

    /* Calculate Z^(-1) */
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    ret |= r_pkcd_ec_inverse(PKCD_EC_WORK3, PKCD_EC_WORK2, PKCD_EC_WORK1, PKC_BOOL_TRUE);

#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
    if (PKCD_EC_CURVE_NIST_P521_LEN != curve_len)
    {
        /* Set DL to (curve length + 64) for ECC operations, unless NIST P-521 curve is used. */
        r_pkcd_cmn_set_dl(curve_len + PKCD_CMN_BITS64, curve_len + PKCD_CMN_BITS64);
    }
    else
    {
        /* Do nothing */
    }
#endif

    /* Calculate x = X/Z and y = Y/Z with S = ABR^(-1) mod C */
    if (NULL != p_x)
    {
        r_pkcd_cmn_ram_clr(PKCD_EC_WORK3, PKCD_EC_WORKUNIT);
        r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK1 + PKCD_EC_OFFSET_VAL,
                                    xyz_data.p_dl64_x + PKCD_EC_OFFSET_DL64VAL, curve_len);
        ret |= r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_WORK2, PKCD_EC_PWORK, PKCD_EC_WORK3, PKCD_EC_WORKUNIT);
        r_pkcd_cmn_cpy_by_64bounds(p_x, PKCD_EC_WORK3 + PKCD_EC_OFFSET_VAL, curve_len);
    }
    else
    {
        /*Do nothing */
    }

    if (NULL != p_y)
    {
        r_pkcd_cmn_ram_clr(PKCD_EC_WORK3, PKCD_EC_WORKUNIT);
        r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK1 + PKCD_EC_OFFSET_VAL,
                                    xyz_data.p_dl64_y + PKCD_EC_OFFSET_DL64VAL, curve_len);
        ret |=  r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_WORK2, PKCD_EC_PWORK, PKCD_EC_WORK3, PKCD_EC_WORKUNIT);
        r_pkcd_cmn_cpy_by_64bounds(p_y, PKCD_EC_WORK3 + PKCD_EC_OFFSET_VAL, curve_len);
    }
    else
    {
        /*Do nothing */
    }

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_projective_to_affine()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_calc_t
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Calculate PKCC modulus dependant value T
 * \param       [in]   p_order        An address of order n.
 * \param       [in]   p_field        An address of field p.
 * \param       [in]   curve_len      A byte size of curve length.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_PDT_P, #PKCD_EC_PDT_N, #PKCD_EC_PWORK, #PKCD_EC_NWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPC
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EC_PDT_P, #PKCD_EC_PDT_N, #PKCD_EC_PWORK and #PKCD_EC_NWORK must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) and #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *  - Set #PKCD_EC_NWORK and #PKCD_EC_PWORK.
 *  - Calculate PKCC modulus dependant value T.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set #PKCD_EC_PDT_N to #PKCD_CMN_ICUM_PKCCB0DPT.
 * -# Clear #PKCD_EC_NWORK
 * -# Copy p_order to the offset(PKCD_EC_OFFSET_VAL) position of the PKCD_EC_NWORK with the #r_pkcd_cmn_cpy_by_64bounds
 * -# Set #PKCD_EC_NWORK to #PKCD_CMN_ICUM_PKCCB0DPC.
 * -# Calculate T over N with mode #PKCD_CMN_MODE_MINUSCINV of #r_pkcd_cmn_run
 * -# Set #PKCD_EC_PDT_P to #PKCD_CMN_ICUM_PKCCB0DPT.
 * -# Clear #PKCD_EC_PWORK
 * -# Copy p_field to the offset(PKCD_EC_OFFSET_VAL) position of the PKCD_EC_PWORK with the #r_pkcd_cmn_cpy_by_64bounds
 * -# Set #PKCD_EC_PWORK to #PKCD_CMN_ICUM_PKCCB0DPC.
 * -# Calculate T over P with mode #PKCD_CMN_MODE_MINUSCINV of #r_pkcd_cmn_run
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_calc_t(const uint8_t* const p_order, const uint8_t* const p_field, const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* Calculate T over N */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_N);
    r_pkcd_cmn_ram_clr(PKCD_EC_NWORK, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_NWORK + PKCD_EC_OFFSET_VAL, p_order, curve_len);
    PKCD_CMN_ICUM_PKCCB0DPC = PKCD_EC_FORMAT_DP_UNIT(PKCD_EC_NWORK);
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_MINUSCINV, PKC_SYNC_OPT_SYNC);

    /* Calculate T over P */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_P);
    r_pkcd_cmn_ram_clr(PKCD_EC_PWORK, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_PWORK + PKCD_EC_OFFSET_VAL, p_field, curve_len);
    PKCD_CMN_ICUM_PKCCB0DPC = PKCD_EC_FORMAT_DP_UNIT(PKCD_EC_PWORK);
    ret |= r_pkcd_cmn_run(PKCD_CMN_MODE_MINUSCINV, PKC_SYNC_OPT_SYNC);

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_calc_t()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_calc_wR
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Calculate wR
 * \param       [in]   p_sign_s          An address of  signature s.
 * \param       [in]   curve_len     A byte size of curve length.
 * \param       [out]  p_wk_wR       An address of work area to set the result of wR.  The data is set at the bottom.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_PDT_N, #PKCD_EC_NWORK,
 *    #PKCD_EC_WORK1, #PKCD_EC_WORK2, #PKCD_EC_WORK3, #PKCD_EC_R2MOD, #PKCD_EC_SCALARWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
* \par Precondition
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_PDT_N and #PKCD_EC_NWORK must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) and #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *  - Calculate wR used in the calculation process of ECDSA verify by the following formula.
 *        - wR = s^(-1) * R. s is signature. R is montgomery coefficient.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set #PKCD_EC_PDT_N to #PKCD_CMN_ICUM_PKCCB0DPT.
 * -# Calculate w = s^(-1) mod n  with #r_pkcd_ec_inverse.
 * -# Calculate R^2 mod n with the #r_pkcd_cmn_R2ModC.
 * -# Calculate wR = w * R^2 * R^(-1) with the #r_pkcd_cmn_ABRInvModC.
 * -# Copy wR to p_wk_wR with the #r_pkcd_cmn_cpy_word
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_calc_wR(const uint8_t* const p_sign_s, const uint32_t curve_len, uint8_t* const p_wk_wR)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* set DT */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_N);

    /* Calculate w = s^(-1) mod n */
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK1, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK3, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_EC_WORK2 + PKCD_EC_OFFSET_VAL, p_sign_s, curve_len);
    ret = r_pkcd_ec_inverse(PKCD_EC_WORK2, PKCD_EC_WORK1, PKCD_EC_WORK3, PKC_BOOL_FALSE);

    /* Calculate R^2 mod n */
    ret |= r_pkcd_cmn_R2ModC(PKCD_EC_NWORK, PKCD_EC_R2MOD, PKCD_EC_WORKUNIT);

    /* Calculate wR with S = ABR^(-1) mod C */
    r_pkcd_cmn_ram_clr(PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    ret |=  r_pkcd_cmn_ABRInvModC(PKCD_EC_WORK1, PKCD_EC_R2MOD, PKCD_EC_NWORK, PKCD_EC_WORK2, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_word(p_wk_wR, PKCD_EC_WORK2, PKCD_EC_WORKUNIT);

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;

}
/***********************************************************************************************************************
* End of function r_pkcd_ec_calc_wR()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_calc_u
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Calculate U1 or U2 used in ECDSA verify.
 * \param       [in]   p_wk_zr       An address of work area where z or r. The data must be aligned with bottom. <br>
 *                                        - z is the number of bits of order n from the top of digest.
 *                                        - r is signature.
 * \param       [in]   p_wk_wR       An address of work area where wR. The data must be aligned with bottom.
 *                                        - w is s^(-1) mod n. s is signature.
 *                                        - R is montgomery coefficient.
 * \param       [in]   curve_len     A byte size of curve length.
 * \param       [out]  p_u           An address of the buffer to set the result of U.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_PDT_N, #PKCD_EC_NWORK, #PKCD_EC_WORK1, #PKCD_EC_WORK2, #PKCD_EC_SCALARWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
* \par Precondition
 *  - #PKCD_EC_OFFSET_VAL, #PKCD_EC_PDT_N and #PKCD_EC_NWORK must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) and #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *  - Calculate U by the following calculation formula.
 *        - U1 = zw mod n
 *        - U2 = rw mod n
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set #PKCD_EC_PDT_N to #PKCD_CMN_ICUM_PKCCB0DPT.
 * -# Calculate U = z(or r) * w mod n with the #r_pkcd_cmn_ABRInvModC.
 * -# Copy U to p_u with the #r_pkcd_cmn_cpy_by_64bounds
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_calc_u(const uint8_t* const p_wk_zr, const uint8_t* const p_wk_wR,
                                    const uint32_t curve_len, st_pkc_data_t* const p_u)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* set DT */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_EC_FORMAT_DP_DPT(PKCD_EC_PDT_N);

    r_pkcd_cmn_ram_clr(PKCD_EC_SCALARWORK, PKCD_EC_WORKUNIT);
    ret = r_pkcd_cmn_ABRInvModC(p_wk_wR, p_wk_zr, PKCD_EC_NWORK, PKCD_EC_SCALARWORK, PKCD_EC_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(p_u->p_data, PKCD_EC_SCALARWORK + PKCD_EC_OFFSET_VAL, curve_len);
    p_u->len = curve_len;

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_calc_u()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_set_digest_to_work
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       A valid digest value is set in the work area.
 * \param       [in]  p_digest          An Address of the digest.
 * \param       [in]  digest_len        The digest length in bytes.
 * \param       [in]  p_order           An address of order n.
 * \param       [in]  curve_len         A byte size of curve length.
 * \param       [out] p_wk_out          An address of work area where top n bits of digest.<br>
 *                                      The data is set at the bottom.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_OFFSET_VAL
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - #PKCD_EC_OFFSET_VAL must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) value must have been set beforehand.
 * \par Detailed description
 *  - Digest uses the n bit on the most significant side. n is the bit length of order.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Loop while i is curve_len or less<br>
 *    Increment i for each loop
 *   -# If *(p_order + i) != 0U then
 *     -# Set *(p_order + i) to lsb
 *     -# break
 * -# Set #PKCD_CMN_BITSOFBYTE * i to zero_bit
 * -# Loop while i is #PKCD_CMN_BITSOFBYTE or less<br>
 *    Increment i for each loop
 *   -# If (lsb & (0x80U >> i)) != 0U then
 *     -# break
 *   -# Increment zero_bit
 * -# Set (curve_len * #PKCD_CMN_BITSOFBYTE) - zero_bit to curve_bit.
 * -# Set (digest_len* #PKCD_CMN_BITSOFBYTE) to digest_bit.
 * -# If digest_bit > curve_bit then
 *   -# Loop while i is zero_bit or less<br>
 *      Increment i for each loop
 *     -# Calculate the digest shifted to the right with #r_pkcd_cmn_ADiv2
 * -# Else
 *   -# Copy digest to p_wk_out with the #r_pkcd_cmn_cpy_by_64bounds
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_set_digest_to_work(const uint8_t* const p_digest, const uint32_t digest_len,
                                                const uint8_t* const p_order, const uint32_t curve_len,
                                                uint8_t* const p_wk_out)
{
    pkcd_ret_t ret = PKCD_RET_OK;  /* Since nothing may be assigned to ret, set PKCD_RET_OK as the initial value. */
    uint32_t zero_bit;
    uint32_t curve_bit;
    uint32_t digest_bit;
    uint32_t i;
    uint8_t  lsb = 0U;

    /* check which high order bits of order(n) are followed by zeros */
    for (i = 0U; i < curve_len; i++ )
    {
        /* No problem for casting to support usual arithmetic conversion */
        if ((uint32_t)p_order[i] != 0U)
        {
            lsb = p_order[i];
            break;
        }
        else
        {
            /*Do nothing */
        }
    }
    zero_bit = PKCD_CMN_BITSOFBYTE * i;

    for (i = 0U; i < PKCD_CMN_BITSOFBYTE; i++ )
    {
        /* No problem for casting to support usual arithmetic conversion */
        if (((uint32_t)lsb & ((uint32_t)0x80U >> i)) != 0U)
        {
            break;
        }
        else
        {
            /*Do nothing */
        }
        zero_bit++;
    }

    curve_bit = (curve_len * PKCD_CMN_BITSOFBYTE) - zero_bit;
    digest_bit = digest_len * PKCD_CMN_BITSOFBYTE;

    if (digest_bit > curve_bit)
    {
        /* copy digest */
        r_pkcd_cmn_cpy_by_64bounds(p_wk_out + PKCD_EC_OFFSET_VAL, p_digest, curve_len);

        /* Shifted to the right by the value of zero_bit_u 32 */
        for (i = 0U; i < zero_bit; i++ )
        {
            ret |= r_pkcd_cmn_ADiv2(p_wk_out, p_wk_out, PKCD_EC_WORKUNIT);
        }
    } /* if (digest_len > curve_len) */
    else
    {
        /* copy digest */
        r_pkcd_cmn_cpy_by_64bounds(p_wk_out + PKCD_EC_OFFSET_LEN(digest_len), p_digest, digest_len);
    }

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);


    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_set_digest_to_work()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_scalar_mult_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       #R_PKCD_EcScalarMultInit argument check function.
 * \param       [in]   p_scalar_or_privkey  A scalar value or private key.
 * \param       [in]   p_point_or_pubkey    A point or public key.
 * \param       [in]   p_ctx                A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]   p_buf                Buffer used for internal processing of PKC-Driver
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
 *  - Check the arguments received by the #R_PKCD_EcScalarMultInit function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_scalar_or_privkey) and (NULL != p_point_or_pubkey) and (NULL != p_ctx) and (NULL != p_buf)
 *    -# Check the curve information with #r_pkcd_ec_chk_curve_info, and set ret to the result.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# If (#PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
 *        -# If (NULL != p_scalar_or_privkey->p_sel_data)
 *          -# Check if given private key binary is valid with #r_pkcd_ec_chk_priv_key, and set ret to the result.
 *        -# Else
 *          -# Set ret to #PKCD_RET_PARAMERR
 *      -# If (#PKC_DATA_TYPE_BYTE == p_scalar_or_privkey->data_type)
 *        -# Check if given scalar is valid with #r_pkcd_ec_chk_scalar, and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCD_RET_PARAMERR
 *      -# If (#PKCD_RET_OK == ret)
 *        -# If (#PKC_DATA_TYPE_KEY_BINARY == p_comp_point_or_pubkey->data_type)
 *          -# If (NULL != p_comp_point_or_pubkey->p_sel_data)
 *            -# Check if given public key binary is valid with #r_pkcd_ec_chk_pub_key, and set ret to the result.
 *          -# Else
 *            -# Set ret to #PKCD_RET_PARAMERR
 *        -# If (#PKC_DATA_TYPE_BYTE == p_comp_point_or_pubkey->data_type)
 *          -# Check if given compress point is valid with #r_pkcd_ec_chk_point, and set ret to the result.
 *        -# Else
 *          -# Set ret to #PKCD_RET_PARAMERR
 *      -# Else
 *        -# Do nothing
 *    -# Else
 *      -# Do nothing
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR
 *  -# If (#PKCD_RET_OK == ret), then
 *    -# If ((#PKC_BOOL_FALSE == p_ctx->sec_scalar_mul) || (NULL != p_ctx->p_drbg_api)), then
 *      -# Do nothing.
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_chk_scalar_mult_init_arg(const st_pkc_select_data_t* const p_scalar_or_privkey,
                                                        const st_pkc_select_data_t* const p_point_or_pubkey,
                                                        const st_pkc_ec_ctx_t* const p_ctx,
                                                        const uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if ((NULL != p_scalar_or_privkey) && (NULL != p_point_or_pubkey) && (NULL != p_ctx) && (NULL != p_buf))
    {
        ret = r_pkcd_ec_chk_curve_info(p_ctx->p_curve_info);
        if (PKCD_RET_OK == ret)
        {
            if (PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
            {
                if (NULL != p_scalar_or_privkey->p_sel_data)
                {
                    /* No problem with casting to pointer size */
                    ret = r_pkcd_ec_chk_priv_key((const st_pkcd_ec_key_bin_t*)p_scalar_or_privkey->p_sel_data->p_data,
                                                    p_ctx->p_curve_info->curve_len);
                }
                else
                {
                    ret = PKCD_RET_PARAMERR;
                }
            }
            else if (PKC_DATA_TYPE_BYTE == p_scalar_or_privkey->data_type)
            {
                ret = r_pkcd_ec_chk_scalar(p_scalar_or_privkey->p_sel_data, p_ctx->p_curve_info->curve_len);
            }
            else
            {
                ret = PKCD_RET_PARAMERR;
            }

            if (PKCD_RET_OK == ret)
            {
                if (PKC_DATA_TYPE_KEY_BINARY == p_point_or_pubkey->data_type)
                {
                    if (NULL != p_point_or_pubkey->p_sel_data)
                    {
                        /* No problem with casting to pointer size */
                        ret = r_pkcd_ec_chk_pub_key((const st_pkcd_ec_key_bin_t*)p_point_or_pubkey->p_sel_data->p_data,
                                                        p_ctx->p_curve_info->curve_len);
                    }
                    else
                    {
                        ret = PKCD_RET_PARAMERR;
                    }
                }
                else if (PKC_DATA_TYPE_BYTE == p_point_or_pubkey->data_type)
                {
                    ret = r_pkcd_ec_chk_point(p_point_or_pubkey->p_sel_data, p_ctx->p_curve_info->curve_len);
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

    if (PKCD_RET_OK == ret)
    {
        if ((PKC_BOOL_FALSE == p_ctx->sec_scalar_mul) || (NULL != p_ctx->p_drbg_api))
        {
            /* Do nothing */
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

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_chk_scalar_mult_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_make_sign_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       #R_PKCD_EcdsaMakeSign argument check function.
 * \param       [in]   p_ctx        A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]   p_buf        Buffer used for internal processing of PKC-Driver
 * \param       [in]   p_digest     A message digest.
 * \param       [in]   p_point_q    A point derived from {random scalar k} * G(x, y).
 * \param       [in]   p_random     A random scalar k
 * \param       [in]   p_priv_key   EC private key of key binary format
 * \param       [in]   p_sign       Output signature data
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
 *  - Check the arguments received by the #R_PKCD_EcdsaMakeSign function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_sign)
 *    -# Check the curve information with #r_pkcd_ec_chk_curve_info, and set ret to the result.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# Check the digest with #r_pkcd_ec_chk_digest, and set ret to the result.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# Check the point Q with #r_pkcd_ec_chk_point, and set ret to the result.
 *        -# If (#PKCD_RET_OK == ret)
 *          -# Check the random scalar k  with #r_pkcd_ec_chk_scalar, and set ret to the result.
 *          -# If (#PKCD_RET_OK == ret)
 *            -# Check the private key binary  with #r_pkcd_ec_chk_priv_key, and set ret to the result.
 *            -# If (#PKCD_RET_OK == ret)
 *              -# If (NULL != p_sign->p_data)
 *                -# If (p_sign->len >= p_ctx->p_curve_info->curve_len * #PKCD_EC_SIGN_DATA_NUM)
 *                  -# Do nothing
 *                -# Else
 *                  -# Set ret to #PKCD_RET_LENGTHERR
 *              -# Else
 *                -# Set ret to #PKCD_RET_PARAMERR
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
static pkcd_ret_t r_pkcd_ec_chk_make_sign_arg(const st_pkc_ec_ctx_t* const p_ctx,
                                                const uint8_t* const p_buf,
                                                const st_pkc_data_t* const p_digest,
                                                const st_pkc_data_t* const p_point_q,
                                                const st_pkc_data_t* const p_random,
                                                const void* const p_priv_key,
                                                const st_pkc_data_t* const p_sign)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_sign))
    {
        ret = r_pkcd_ec_chk_curve_info(p_ctx->p_curve_info);
        if (PKCD_RET_OK == ret)
        {
            ret = r_pkcd_ec_chk_digest(p_digest);
            if (PKCD_RET_OK == ret)
            {
                ret = r_pkcd_ec_chk_point(p_point_q, p_ctx->p_curve_info->curve_len);
                if (PKCD_RET_OK == ret)
                {
                    ret = r_pkcd_ec_chk_scalar(p_random, p_ctx->p_curve_info->curve_len);
                    if (PKCD_RET_OK == ret)
                    {
                        /* No problem with casting to pointer size */
                        ret = r_pkcd_ec_chk_priv_key((const st_pkcd_ec_key_bin_t*)p_priv_key,
                                                        p_ctx->p_curve_info->curve_len);
                        if (PKCD_RET_OK == ret)
                        {
                            if (NULL != p_sign->p_data)
                            {
                                if (p_sign->len >= (p_ctx->p_curve_info->curve_len * PKCD_EC_SIGN_DATA_NUM))
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
* End of function r_pkcd_ec_chk_make_sign_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_calc_u1u2_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       #R_PKCD_EcdsaCalcU1U2 argument check function.
 * \param       [in] p_ctx      A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] p_digest   A message digest.
 * \param       [in] p_sign     A signature data.
 * \param       [in] p_u1       Output intermediate data u1.
 * \param       [in] p_u2       Output intermediate data u2.
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
 *  - Check the arguments received by the #R_PKCD_EcdsaCalcU1U2 function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_u1) and (NULL != p_u2)
 *    -# Check the curve information with #r_pkcd_ec_chk_curve_info, and set ret to the result.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# Check the digest with #r_pkcd_ec_chk_digest, and set ret to the result.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# Check the sign with #r_pkcd_ec_chk_sign, and set ret to the result.
 *        -# If (#PKCD_RET_OK == ret)
 *          -# If (NULL != p_u1->p_data) and (NULL != p_u2->p_data)
 *            -# If (p_u1->len->len >= p_ctx->p_curve_info->curve_len) and
 *                  (p_u2->len->len >= p_ctx->p_curve_info->curve_len)
 *              -# Do nothing
 *            -# Else
 *              -# Set ret to #PKCD_RET_LENGTHERR
 *          -# Else
 *            -# Set ret to #PKCD_RET_PARAMERR
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
static pkcd_ret_t r_pkcd_ec_chk_calc_u1u2_arg(const st_pkc_ec_ctx_t* const p_ctx,
                                                const uint8_t* const p_buf,
                                                const st_pkc_data_t* const p_digest,
                                                const st_pkc_data_t* const p_sign,
                                                const st_pkc_data_t* const p_u1,
                                                const st_pkc_data_t* const p_u2)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_u1) && (NULL != p_u2))
    {
        ret = r_pkcd_ec_chk_curve_info(p_ctx->p_curve_info);
        if (PKCD_RET_OK == ret)
        {
            ret = r_pkcd_ec_chk_digest(p_digest);
            if (PKCD_RET_OK == ret)
            {
                ret = r_pkcd_ec_chk_sign(p_sign, p_ctx->p_curve_info->curve_len);
                if (PKCD_RET_OK == ret)
                {
                    if ((NULL != p_u1->p_data) && (NULL != p_u2->p_data))
                    {
                        if ((p_u1->len >= p_ctx->p_curve_info->curve_len) &&
                            (p_u2->len >= p_ctx->p_curve_info->curve_len))
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

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_chk_calc_u1u2_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_compare_sign_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       #R_PKCD_EcdsaCompareSign argument check function.
 * \param       [in] p_ctx      A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] p_u1g      A point derived from u1 x G(x, y). *where G is base point.
 * \param       [in] p_u2q      A point derived from u2 x Q(x, y). *where Q is public key.
 * \param       [in] p_sign     An ECDSA signature.
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
 *  - Check the arguments received by the #R_PKCD_EcdsaCompareSign function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_ctx) and (NULL != p_buf)
 *    -# Check the curve information with #r_pkcd_ec_chk_curve_info, and set ret to the result.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# Check the point u1G with #r_pkcd_ec_chk_point, and set ret to the result.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# Check the point u2Q with #r_pkcd_ec_chk_point, and set ret to the result.
 *        -# If (#PKCD_RET_OK == ret)
 *          -# Check the sign with #r_pkcd_ec_chk_sign, and set ret to the result.
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
static pkcd_ret_t r_pkcd_ec_chk_compare_sign_arg(const st_pkc_ec_ctx_t* const p_ctx,
                                                    const uint8_t* const p_buf,
                                                    const st_pkc_data_t* const p_u1g,
                                                    const st_pkc_data_t* const p_u2q,
                                                    const st_pkc_data_t* const p_sign)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if ((NULL != p_ctx) && (NULL != p_buf))
    {
        ret = r_pkcd_ec_chk_curve_info(p_ctx->p_curve_info);
        if (PKCD_RET_OK == ret)
        {
            ret = r_pkcd_ec_chk_point(p_u1g, p_ctx->p_curve_info->curve_len);
            if (PKCD_RET_OK == ret)
            {
                ret = r_pkcd_ec_chk_point(p_u2q, p_ctx->p_curve_info->curve_len);
                if (PKCD_RET_OK == ret)
                {
                    ret = r_pkcd_ec_chk_sign(p_sign, p_ctx->p_curve_info->curve_len);
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
* End of function r_pkcd_ec_chk_compare_sign_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_private_scalar_gen_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       #R_PKCD_EcPrivateScalarGen argument check function.
 * \param       [in] p_ctx              A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in] p_buf              Buffer used for internal processing of PKC-Driver
 * \param       [in] p_extra_random     Random value that is the basis of Private Scalar.
 * \param       [in] p_priv_scalar      Output Private Scalar.
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
 *  - Check the arguments received by the #R_PKCD_EcPrivateScalarGen function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_extra_random) and (NULL != p_priv_scalar)
 *    -# Check the curve information with #r_pkcd_ec_chk_curve_info, and set ret to the result.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# If (NULL != p_extra_random->p_data)
 *        -# If (p_ctx->p_curve_info->curve_len + #PKCD_EC_EXTRA_BYTES) == p_extra_random->len)
 *          -# If (NULL != p_priv_scalar->p_data)
 *            -# If (p_priv_scalar->len >= p_ctx->p_curve_info->curve_len)
 *              -# Do nothing
 *            -# Else
 *              -# Set ret to #PKCD_RET_LENGTHERR
 *          -# Else
 *            -# Set ret to #PKCD_RET_PARAMERR
 *        -# Else
 *          -# Set ret to #PKCD_RET_LENGTHERR
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
static pkcd_ret_t r_pkcd_ec_chk_private_scalar_gen_arg(const st_pkc_ec_ctx_t* const p_ctx,
                                                        const uint8_t* const p_buf,
                                                        const st_pkc_data_t* const p_extra_random,
                                                        const st_pkc_data_t* const p_priv_scalar)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_extra_random) && (NULL != p_priv_scalar))
    {
        ret = r_pkcd_ec_chk_curve_info(p_ctx->p_curve_info);
        if (PKCD_RET_OK == ret)
        {
            if (NULL != p_extra_random->p_data)
            {
                if ((p_ctx->p_curve_info->curve_len + PKCD_EC_EXTRA_BYTES) == p_extra_random->len)
                {
                    if (NULL != p_priv_scalar->p_data)
                    {
                        if (p_priv_scalar->len >= p_ctx->p_curve_info->curve_len)
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
* End of function r_pkcd_ec_chk_private_scalar_gen_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_key_wrap_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       #r_pkcd_ec_key_wrap argument check function.
 * \param       [in] p_ctx              A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in] p_priv_key_raw     Byte array of private key.
 * \param       [in] p_pub_key_raw      Byte array of public key.
 * \param       [in] p_priv_key         Output EC private key of key binary format.
 * \param       [in] p_pub_key          Output EC public key of key binary format.
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
 *  - Check the arguments received by the #r_pkcd_ec_key_wrap function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_ctx) and (NULL != p_priv_key) and (NULL != p_pub_key)
 *    -# If (NULL != p_ctx->p_curve_info)
 *      -# Check the curve length with is valid with #r_pkcd_ec_chk_curve_len, and set ret to the return value.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# Check the argument p_priv_key_raw with #r_pkcd_ec_chk_priv_scalar, and set ret to the return value.
 *        -# If (#PKCD_RET_OK == ret)
 *          -# Check the argument p_pub_key_raw with #r_pkcd_ec_chk_point, and set ret to the return value.
 *          -# If (#PKCD_RET_OK == ret)
 *            -# Set the argument p_priv_key into #st_pkcd_ec_key_bin_t * argument p_priv_key_bin.
 *            -# If (NULL != p_priv_key_bin->p_d) and (NULL != p_priv_key_bin->p_qx) and (NULL != p_priv_key_bin->p_qy)
 *              -# Set the argument p_pub_key into #st_pkcd_ec_key_bin_t * argument p_pub_key_bin.
 *              -# If (NULL != p_pub_key_bin->p_qx) and (NULL != p_pub_key_bin->p_qy)
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
static pkcd_ret_t r_pkcd_ec_chk_key_wrap_arg(const st_pkc_ec_ctx_t* const p_ctx,
                                                const st_pkc_data_t* const p_priv_key_raw,
                                                const st_pkc_data_t* const p_pub_key_raw,
                                                const void* const p_priv_key,
                                                const void* const p_pub_key)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const st_pkcd_ec_key_bin_t * p_priv_key_bin;
    const st_pkcd_ec_key_bin_t * p_pub_key_bin;

    if ((NULL != p_ctx) && (NULL != p_priv_key) && (NULL != p_pub_key))
    {
        if ((NULL != p_ctx->p_curve_info))
        {
            ret = r_pkcd_ec_chk_curve_len(p_ctx->p_curve_info->curve_len);
            if (PKCD_RET_OK == ret)
            {
                ret = r_pkcd_ec_chk_priv_scalar(p_priv_key_raw, p_ctx->p_curve_info->curve_len);
                if (PKCD_RET_OK == ret)
                {
                    ret = r_pkcd_ec_chk_point(p_pub_key_raw, p_ctx->p_curve_info->curve_len);
                    if (PKCD_RET_OK == ret)
                    {
                        /* No problem with casting to pointer size */
                        p_priv_key_bin = (const st_pkcd_ec_key_bin_t*)p_priv_key;
                        if ((NULL != p_priv_key_bin->p_d) && (NULL != p_priv_key_bin->p_qx) &&
                            (NULL != p_priv_key_bin->p_qy))
                        {
                            /* No problem with casting to pointer size */
                            p_pub_key_bin = (const st_pkcd_ec_key_bin_t*)p_pub_key;
                            if ((NULL != p_pub_key_bin->p_qx) && (NULL != p_pub_key_bin->p_qy))
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
* End of function r_pkcd_ec_chk_key_wrap_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_scalar
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
 * \brief        Check the scalar parameters
 * \param       [in]   p_scalar       A scalar value.
 * \param       [in]   curve_len      A curve length.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_PARAMERR:               An invalid argument was entered.
 *  - #PKCD_RET_LENGTHERR:              Invalid length.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the scalar parameters.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If p_scalar != NULL
 *   -# If (NULL != p_scalar->p_data)
 *     -# If (p_scalar->len <= curve_len)
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
static pkcd_ret_t r_pkcd_ec_chk_scalar(const st_pkc_data_t* const p_scalar, const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if (NULL != p_scalar)
    {
        if (NULL != p_scalar->p_data)
        {
            if (p_scalar->len <= curve_len)
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
* End of function r_pkcd_ec_chk_scalar()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_priv_scalar
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
 * \brief        Check the private scalar parameters
 * \param       [in]   p_priv_scalar  A scalar value.
 * \param       [in]   curve_len      A curve length.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_PARAMERR:               An invalid argument was entered.
 *  - #PKCD_RET_LENGTHERR:              Invalid length.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the private scalar parameters.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If p_priv_scalar != NULL
 *   -# If (NULL != p_priv_scalar->p_data)
 *     -# If (p_priv_scalar->len == curve_len)
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
static pkcd_ret_t r_pkcd_ec_chk_priv_scalar(const st_pkc_data_t* const p_priv_scalar, const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if (NULL != p_priv_scalar)
    {
        if (NULL != p_priv_scalar->p_data)
        {
            if (p_priv_scalar->len == curve_len)
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
* End of function r_pkcd_ec_chk_priv_scalar()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_point
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
 * \brief        Check the point parameters
 * \param       [in]   p_point        A point to be multiplied.
 * \param       [in]   curve_len      A curve length.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_PARAMERR:               An invalid argument was entered.
 *  - #PKCD_RET_LENGTHERR:              Invalid length.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the point parameters.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If NULL != p_point
 *   -# If NULL != p_point->p_data
 *     -# If (p_point->len != (curve_len * #PKCD_EC_POINT_DATA_NUM)
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
static pkcd_ret_t r_pkcd_ec_chk_point(const st_pkc_data_t* const p_point, const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* check if the p_point is NULL */
    if (NULL != p_point)
    {
        if (NULL != p_point->p_data)
        {
            if ((curve_len * PKCD_EC_POINT_DATA_NUM) == p_point->len)
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
* End of function r_pkcd_ec_chk_point()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_digest
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
 * \brief        Check the digest
 * \param       [in]   p_digest       A message digest.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_PARAMERR:               Some mandatory parameters are NULL.
 *  - #PKCD_RET_LENGTHERR:              Invalid digest length are passed.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the digest.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If NULL != p_digest
 *   -# If NULL != p_digest->p_digest
 *     -# If (#PKCD_EC_DIGEST_LEN_MIN <= p_digest->len) and (p_digest->len <= #PKCD_EC_INPUT_DIGEST_LEN_MAX)
 *       -# Check the p_digest length is a multiple of 4 with the #r_pkcd_cmn_is_multiple_4.
 *       -# If p_digest length is multiple of 4.
 *         -# Set ret to #PKCD_RET_OK
 *       -# Else
 *         -# Set ret to #PKCD_RET_LENGTHERR
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
static pkcd_ret_t r_pkcd_ec_chk_digest(const st_pkc_data_t* const p_digest)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    e_pkc_bool_t is_multiple;

    /* check if the p_digest is NULL */
    if (NULL != p_digest)
    {
        if (NULL != p_digest->p_data)
        {
            /* check the digest length range */
            if ((PKCD_EC_DIGEST_LEN_MIN <= p_digest->len) && (p_digest->len <= PKCD_EC_INPUT_DIGEST_LEN_MAX))
            {
                /* check the digest length is a multiple of 4 */
                is_multiple = r_pkcd_cmn_is_multiple_4(p_digest->len);
                if (PKC_BOOL_TRUE == is_multiple)
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
* End of function r_pkcd_ec_chk_digest()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_priv_key
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
 * \brief        Check the private key parameters
 * \param       [in]   p_priv_key_bin EC private key of key binary format.
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
 *  -# If p_priv_key_bin != NULL
 *    -# If p_priv_key_bin->key_length is equal to curve length then
 *      -# If (p_priv_key_bin->p_d != NULL)
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
static pkcd_ret_t r_pkcd_ec_chk_priv_key(const st_pkcd_ec_key_bin_t* const p_priv_key_bin, const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if (NULL != p_priv_key_bin)
    {
        if (curve_len == p_priv_key_bin->key_length)
        {
            if (NULL != p_priv_key_bin->p_d)
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
* End of function r_pkcd_ec_chk_priv_key()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_pub_key
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
 * \brief        Check the private key parameters
 * \param       [in]   p_pub_key_bin  EC public key of key binary format or <br>
 *                                    EC private key include public key of key binary format
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
 *  -# If p_pub_key_bin != NULL
 *    -# If p_pub_key_bin->key_length is equal to curve length then
 *      -# If (p_pub_key_bin->p_qx != NULL) and (p_pub_key_bin->p_qy != NULL)
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
static pkcd_ret_t r_pkcd_ec_chk_pub_key(const st_pkcd_ec_key_bin_t* const p_pub_key_bin, const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if (NULL != p_pub_key_bin)
    {
        if (curve_len == p_pub_key_bin->key_length)
        {
            if ((NULL != p_pub_key_bin->p_qx) && (NULL != p_pub_key_bin->p_qy))
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
* End of function r_pkcd_ec_chk_pub_key()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_sign
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
 * \brief        Check the signature parameters
 * \param       [in]   p_sign         An ECDSA signature.
 * \param       [in]   curve_len      A curve length.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
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
 *  - Check the signature parameters.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If NULL != p_sign
 *   -# If NULL != p_sign->p_data
 *     -# If ((curve_len * #PKCD_EC_SIGN_DATA_NUM) == p_sign->len)
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
static pkcd_ret_t r_pkcd_ec_chk_sign(const st_pkc_data_t* const p_sign, const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* check if the p_sign is NULL */
    if (NULL != p_sign)
    {
        /* check if the signature members are NULL */
        if (NULL != p_sign->p_data)
        {
            /* check if the key length is same curve length */
            if ((curve_len * PKCD_EC_SIGN_DATA_NUM) == p_sign->len)
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
* End of function r_pkcd_ec_chk_sign()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_curve_info
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
 * \brief        Check the curve information
 * \param       [in]   p_curve_info   A curve infomation.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
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
 *    -# Check the p_curve_info->curve_len with #r_pkcd_ec_chk_curve_len, and set ret to the return value.
 *    -# If ret == #PKCD_RET_OK
 *      -# If p_curve_info->curve_params.p_data != NULL then
 *        -# If p_curve_info->curve_params.len == (curve length * the number of curve parameters)
 *          -# Check the curve params field p is zero with #r_pkcd_cmn_cmp_zero_word and set the result to cmp_ret.
 *          -# If 0 != cmp_ret
 *            -# Check the curve params order n is zero with #r_pkcd_cmn_cmp_zero_word and set the result to cmp_ret.
 *            -# If 0 != cmp_ret
 *                -# Do nothing.
 *            -# Else
 *              -# Set ret to #PKCD_RET_PARAMERR.
 *          -# Else
 *            -# Set ret to #PKCD_RET_PARAMERR.
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
static pkcd_ret_t r_pkcd_ec_chk_curve_info(const st_pkc_ec_curve_t* const p_curve_info)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    int32_t cmp_ret;

    /* Check if the argument is null */
    if (NULL != p_curve_info)
    {
        /* Check curve_len */
        ret = r_pkcd_ec_chk_curve_len(p_curve_info->curve_len);
        if (PKCD_RET_OK == ret)
        {
            if (NULL != p_curve_info->curve_params.p_data)
            {
                if ((p_curve_info->curve_len * PKCD_EC_CURVE_PARAM_NUM) == p_curve_info->curve_params.len)
                {
                    /* If field p is 0, PKCD_RET_PARAMERR */
                    cmp_ret = r_pkcd_cmn_cmp_zero_word(&p_curve_info->curve_params.p_data[0],
                                                        p_curve_info->curve_len);
                    if (0 != cmp_ret)
                    {
                        /* If order n is 0, PKCD_RET_PARAMERR */
                        cmp_ret = r_pkcd_cmn_cmp_zero_word(&p_curve_info->curve_params.p_data[p_curve_info->curve_len],
                                                            p_curve_info->curve_len);
                        if (0 != cmp_ret)
                        {
                            /* Do nothing */
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
* End of function r_pkcd_ec_chk_curve_info()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_chk_curve_len
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
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
 *  -# Initialize is_multiple_4 with #PKC_BOOL_FALSE.
 *  -# Check that the length of the curve is a multiple of 4 with #r_pkcd_cmn_is_multiple_4,
 *     and set is_multiple_4 to the return value.
 *  -# If ((#PKCD_EC_CURVE_LEN_MIN <= p_curve->curve_len) && (p_curve->curve_len <= #PKCD_EC_CURVE_LEN_MAX) &&
 *        (is_multiple_4 == #PKC_BOOL_TRUE))
 *    -# Set ret to #PKCD_RET_OK.
 *  -# Else
 *    -# Set ret to #PKCD_RET_LENGTHERR.
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_chk_curve_len(const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    e_pkc_bool_t is_multiple_4 = PKC_BOOL_FALSE;

    is_multiple_4 = r_pkcd_cmn_is_multiple_4(curve_len);
    if ((PKCD_EC_CURVE_LEN_MIN <= curve_len) && (curve_len <= PKCD_EC_CURVE_LEN_MAX) &&
        (PKC_BOOL_TRUE == is_multiple_4))
    {
        ret = PKCD_RET_OK;
    }
    else
    {
        ret = PKCD_RET_LENGTHERR;
    }


    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_chk_curve_len()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_set_len_info
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Initialize length informations
 * \param       [in]   curve_len      A curve length.
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
 *  - Setting #PKCD_EC_DLBYTES, #PKCD_EC_DL64BYTES, #PKCD_EC_OFFSET_VAL, #PKCD_EC_OFFSET_DL64VAL.
 *  - If #PKCD_CFG_OVF_WORKAROUND_ENABLE == 1 and #PKCD_EC_CURVE_NIST_P521_LEN != curve_len,
 *    #PKCD_EC_DLBYTES and #PKCD_EC_DL64BYTES are set to 64 bits more than otherwise.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
static void r_pkcd_ec_set_len_info(const uint32_t curve_len)
{
    PKCD_EC_DLBYTES = ((curve_len + PKCD_CMN_ADD_7) >> PKCD_CMN_SHIFT_3)
                        << PKCD_CMN_SHIFT_3;
    PKCD_EC_DL64BYTES = PKCD_EC_DLBYTES + PKCD_CMN_BITS64;
    PKCD_EC_OFFSET_VAL = PKCD_EC_WORKUNIT - curve_len;
    PKCD_EC_OFFSET_DL64VAL = PKCD_CMN_BITS64 + (PKCD_EC_DLBYTES - curve_len);
#if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 1U)
    if (PKCD_EC_CURVE_NIST_P521_LEN != curve_len)
    {
        /* Add 64 bits to PKCD_EC_DL64BYTES and PKCD_EC_OFFSET_DL64VAL for ECC operations */
        PKCD_EC_DL64BYTES = PKCD_EC_DL64BYTES + PKCD_CMN_BITS64;
        PKCD_EC_OFFSET_DL64VAL = PKCD_EC_OFFSET_DL64VAL + PKCD_CMN_BITS64;
    }
    else
    {
        /* Do nothing */
    }
#endif
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_set_len_info()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_parse_curve
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIsInternal
 * \brief       Parse curve parameters
 * \param       [out] p_curve           An adress of curve information structure defined inside EC is set.
 * \param       [in]  p_curve_info      An adress of curve information structure defined PKC common is set.
 * \return
 *  - void
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
 * \callgraph
 *********************************************************************************************************************/
static void r_pkcd_ec_parse_curve(st_ec_curve_t* const p_curve, const st_pkc_ec_curve_t* const p_curve_info)
{
    p_curve->len = p_curve_info->curve_len;
    p_curve->p_field  = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EC_CURVE_PARAM_IDX_FIELD];
    p_curve->p_order  = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EC_CURVE_PARAM_IDX_ORDER];
    p_curve->p_coeff1 = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EC_CURVE_PARAM_IDX_COEFF1];
    p_curve->p_coeff2 = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EC_CURVE_PARAM_IDX_COEFF2];
    p_curve->p_base_x = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EC_CURVE_PARAM_IDX_BASE_X];
    p_curve->p_base_y = &p_curve_info->curve_params.p_data[p_curve->len * PKCD_EC_CURVE_PARAM_IDX_BASE_Y];
}
/**********************************************************************************************************************
* End of function r_pkcd_ec_parse_curve()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_cpy_scalar
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
 * \brief        Copy scalar values
 * \param       [in]   p_in           A source address which contain scalar value.
 * \param       [out]  p_out          A destination address.
 * \param       [in]   curve_len      A curve length.
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
 *  - Copy scalar values.
 *  - If the data length of the copy source is smaller than curve_len, it is filled with leading 0 and copied.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
static void r_pkcd_ec_cpy_scalar(const st_pkc_data_t* const p_in, st_ec_scalar_t* const p_out, const uint32_t curve_len)
{
    const uint32_t offset = curve_len - p_in->len;

    r_pkcd_cmn_cpy_byte(&p_out->scalar[offset], p_in->p_data, p_in->len);
    p_out->len = p_in->len;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_cpy_scalar()
***********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_cpy_point
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
 * \brief        Copy point to be multiplied
 * \param       [in]   p_in           A source address which data that concatenates the x and y coordinates.
 * \param       [out]  p_out          A destination address.
 * \param       [in]   curve_len      A curve length.
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
 *  - Copy point to be multiplied
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
static void r_pkcd_ec_cpy_point(const st_pkc_data_t* const p_in, st_ec_point_t* const p_out, const uint32_t curve_len)
{
    r_pkcd_cmn_cpy_byte(p_out->x,  &p_in->p_data[curve_len * PKCD_EC_POINT_DATA_IDX_X], curve_len);
    r_pkcd_cmn_cpy_byte(p_out->y,  &p_in->p_data[curve_len * PKCD_EC_POINT_DATA_IDX_Y], curve_len);
    p_out->len = curve_len;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_cpy_point()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_cpy_digest
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
 * \brief        Copy message digest
 * \param       [in]   p_in           A source address with message digest.
 * \param       [out]  p_out          A destination address.
 * \param       [out]  out_len        A copy data length.
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
 *  - Copy message digest.
 *  - If the data length of the copy source is larger than #PKCD_EC_DIGEST_LEN_MAX, <br>
 *    the data length to be copied is the value of #PKCD_EC_DIGEST_LEN_MAX.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
static void r_pkcd_ec_cpy_digest(const st_pkc_data_t* const p_in, uint8_t* const p_out, uint32_t* const out_len)
{
    const uint32_t cpy_len = (p_in->len > PKCD_EC_DIGEST_LEN_MAX)?PKCD_EC_DIGEST_LEN_MAX : p_in->len;

    r_pkcd_cmn_cpy_byte(p_out, p_in->p_data,  cpy_len);
    *out_len = cpy_len;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_cpy_digest()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_cpy_sign
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
 * \brief        Copy ECDSA signature.
 * \param       [in]   p_in           A source address which contain ECDSA signature.
 * \param       [out]  p_out          A destination address.
 * \param       [in]   curve_len      A curve length.
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
 *  - Copy ECDSA signature.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
static void r_pkcd_ec_cpy_sign(const st_pkc_data_t* const p_in, st_ec_signature_t* const p_out,
                                const uint32_t curve_len)
{
    r_pkcd_cmn_cpy_word(p_out->r,  &p_in->p_data[curve_len * PKCD_EC_SIGN_DATA_IDX_R],  curve_len);
    r_pkcd_cmn_cpy_word(p_out->s,  &p_in->p_data[curve_len * PKCD_EC_SIGN_DATA_IDX_S],  curve_len);
    p_out->len = curve_len;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_cpy_sign()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ec_gen_rand_z
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDECAPIsInternal
 * \brief        Generate a random z coordinate.
 * \param       [in]   p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [out]  p_out          A destination address.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_DRBGAPIERR:             The DRBG API passed by user returned error.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Generate a random z coordinate.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# Generate random data whose length is curve_len with a DRBG API passed by a user,
 *     and set p_out to the random data.
 *  -# Set drv_ret to the return value of the DRBG API.
 *  -# If (#PKCD_CMN_EXTERNAL_API_OK == drbg_ret)
 *    -# Set ret to #PKCD_RET_OK.
 *  -# Else
 *    -# Set ret to #PKCD_RET_DRBGAPIERR.
 *  -# Count the number of 0s in the upper bits of the field order p with #r_pkcd_cmn_find_msb,
 *     and set zeros to the result.
 *  -# Set zeros_byte to zeros / 8.
 *  -# Set zeros_bit to zeros mod 8.
 *  -# Loop while i is less than zeros_byte<br>
 *     Increment i for each loop
 *    -# Set p_out[i] to 0.
 *  -# Set p_out[i] to (0xFF >> (zeros_bit + 1U)) & p_out[i].
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ec_gen_rand_z(const st_pkc_ec_ctx_t* const p_ctx, uint8_t* const p_out)
{
    pkcd_ret_t      ret = PKCD_RET_INTERNALERR;
    uint32_t        drbg_ret;
    const uint8_t * p_p;
    const uint32_t  curve_len = p_ctx->p_curve_info->curve_len;
    uint32_t        zeros;
    uint32_t        zeros_byte;
    uint32_t        zeros_bit;
    uint32_t        i;

    drbg_ret = p_ctx->p_drbg_api(p_ctx->p_curve_info->curve_len, p_out);

    if (PKCD_CMN_EXTERNAL_API_OK == drbg_ret)
    {
        ret = PKCD_RET_OK;
    }
    else
    {
        ret = PKCD_RET_DRBGAPIERR;
    }

    p_p         = &p_ctx->p_curve_info->curve_params.p_data[curve_len * PKCD_EC_CURVE_PARAM_IDX_FIELD];
    zeros       = (curve_len * PKCD_CMN_BITSOFBYTE) - r_pkcd_cmn_find_msb(p_p, curve_len);
    zeros_byte  = zeros >> PKCD_CMN_SHIFT_3;
    zeros_bit   = zeros & (uint32_t)PKCD_EC_MASK_07; /* No problem for casting to support
                                                        usual bitwise operation */

    for (i = 0U; i < zeros_byte; i++ )
    {
        p_out[i] = 0U;
    }

    /* No problem for casting to support usual arithmetic conversion */
    p_out[i] = (uint8_t) ((((uint32_t)PKCD_CMN_UINT8_MAX) >> (zeros_bit + 1U))
                & (uint32_t)p_out[i]);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ec_gen_rand_z()
**********************************************************************************************************************/
