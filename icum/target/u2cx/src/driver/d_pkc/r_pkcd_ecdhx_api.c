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
/* PRQA S 0488 ++ # D007 Consider not to overrun the size of the array pointed to by the pointer.*/
/* PRQA S 0751 ++ # D009 For commonality of functions, confirm that the size is correct and use the cast value. */
/* PRQA S 1503 ++ # D013 No problem if it is unused because it is an API called from outside the library */
/* PRQA S 2053 ++ # D016 No problem because it is a comment explaining the process. */
/* PRQA S 3383,3384 ++ # D018 Have confirmed in advance that the value doesn't wrap around. */
/* PRQA S 2981 ++ # D020 Local variable ret of APIs sets INTERNALERR to the initial value
                        as a countermeasure against attacks that pass through processing. */
/* PRQA S 2984,2985, 2986 ++ # D021 Because it is calculated in consideration of future expansion */
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
 * \addtogroup    PKCDECDHX
 * \{
 * \addtogroup    PKCDECDHXTypes              ECDHX Types
 * \addtogroup    PKCDECDHXConstants          ECDHX Constants
 * \addtogroup    PKCDECDHXAPIs               ECDHX APIs
 * \addtogroup    PKCDECDHXDefinesInternal    ECDHX Internal Definitions
 * \addtogroup    PKCDECDHXTypesInternal      ECDHX Internal Types
 * \addtogroup    PKCDECDHXVariablesInternal  ECDHX Internal Variables
 * \addtogroup    PKCDECDHXAPIsInternal       ECDHX Internal APIs
 * \}******************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDECDHXDefinesInternal
 * \{******************************************************************/
#define PKCD_ECDHX_WORKUNITPDT                 (0x00000008U)  /**< A size of 8byte PDT area */
#define PKCD_ECDHX_WORKUNIT                    (0x00000040U)  /**< A size of 64byte work area */


/** A size of 216byte work area */
#define PKCD_ECDHX_WORKUNITPDW                 (PKCD_ECDHX_WORKUNIT*3U)
/** A work area for T mod p */
#define PKCD_ECDHX_PDT_P                       (sp_ecdhx_pkccram->pdt_p)
/** A byte length of field p */
#define PKCD_ECDHX_PBYTES                      (sp_ecdhx_pkccram->pbytes)
/** A 64byte work area which stores p */
#define PKCD_ECDHX_PWORK                       (sp_ecdhx_pkccram->pwork)
/** A size of padding of DL in bytes */
#define PKCD_ECDHX_OFFSET_VAL                  (sp_ecdhx_pkccram->offval)
/** A 64byte work area which stores R^2 mod (p-2) */
#define PKCD_ECDHX_R2WORK                      (sp_ecdhx_pkccram->r2work)
/** A 64byte work area which stores x1 */
#define PKCD_ECDHX_X1RWORK                     (sp_ecdhx_pkccram->x1rwork)
/** A 64byte work area which stores x2 */
#define PKCD_ECDHX_X2RWORK                     (sp_ecdhx_pkccram->x2rwork)
/** A 64byte work area which stores x3 */
#define PKCD_ECDHX_X3RWORK                     (sp_ecdhx_pkccram->x3rwork)
/** A 64byte work area which stores z2 */
#define PKCD_ECDHX_Z2RWORK                     (sp_ecdhx_pkccram->z2rwork)
/** A 64byte work area which stores z3 */
#define PKCD_ECDHX_Z3RWORK                     (sp_ecdhx_pkccram->z3rwork)
/** A 64byte work area which stores a */
#define PKCD_ECDHX_ARWORK                      (sp_ecdhx_pkccram->arwork)
/** A 64byte work area which stores b */
#define PKCD_ECDHX_BRWORK                      (sp_ecdhx_pkccram->brwork)
/** A 64byte work area which stores e */
#define PKCD_ECDHX_ERWORK                      (sp_ecdhx_pkccram->erwork)
/** A 64byte work area which stores a^2 mod p */
#define PKCD_ECDHX_A2RWORK                     (sp_ecdhx_pkccram->a2rwork)
/** A 64byte work area which stores a^2 mod p */
#define PKCD_ECDHX_B2RWORK                     (sp_ecdhx_pkccram->b2rwork)
/** A 64byte work area which stores d*a mod p */
#define PKCD_ECDHX_DARWORK                     (sp_ecdhx_pkccram->darwork)
/** A 64byte work area which stores c*b mod p */
#define PKCD_ECDHX_CBRWORK                     (sp_ecdhx_pkccram->cbrwork)
/** A 64byte work area which stores a24 */
#define PKCD_ECDHX_A24RWORK                    (sp_ecdhx_pkccram->a24rwork)
/** A 64byte work area */
#define PKCD_ECDHX_WORK1                       (sp_ecdhx_pkccram->work1)
/** A 64byte work area */
#define PKCD_ECDHX_WORK2                       (sp_ecdhx_pkccram->work2)
/** A 64byte work area */
#define PKCD_ECDHX_WORK3                       (sp_ecdhx_pkccram->work3)
/** A 192byte work area */
#define PKCD_ECDHX_WWORK                       (sp_ecdhx_pkccram->wwork)
/** A value used in X25519 calculation */
#define PKCD_ECDHX_SWAP                        (sp_ecdhx_pkccram->swap)

/** Convert pointer shifted by PDT data area size to PKCC data pointer register format */
#define PKCD_ECDHX_FORMAT_DP_DPT(top)      (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_ECDHX_WORKUNITPDT))
/** Convert pointer shifted by PDW data area size to PKCC data pointer register format */
#define PKCD_ECDHX_FORMAT_DP_DPW(top)      (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_ECDHX_WORKUNITPDW))

#define PKCD_ECDHX_CURVE25519_LEN          (32U)                        /**< Curve length of curve25519 */
#define PKCD_ECDHX_CURVE448_LEN            (56U)                        /**< Curve length of curve448 */
#define PKCD_ECDHX_CURVE_LEN_MAX           (PKCD_ECDHX_CURVE448_LEN)    /**< Maximum length of curve length */

/** Field p index of concatenated curve parameters */
#define PKCD_ECDHX_CURVE_PARAM_IDX_FIELD       (0U)
/** a24(defined by a24 = (A-2)/4) index of concatenated curve parameters */
#define PKCD_ECDHX_CURVE_PARAM_IDX_A24         (1U)
/** u-coordinate of the base point index of concatenated curve parameters */
#define PKCD_ECDHX_CURVE_PARAM_IDX_BASE_U      (2U)
/** The number of concatenated curve parameters */
#define PKCD_ECDHX_CURVE_PARAM_NUM             (3U)

#define PKCD_ECDHX_MASK_7F                     (0x7FU)    /**< the value used for mask the scalar value */
#define PKCD_ECDHX_MASK_40                     (0x40U)    /**< the value used for mask the scalar value */
#define PKCD_ECDHX_MASK_F8                     (0xF8U)    /**< the value used for mask the u-coordinate value */
#define PKCD_ECDHX_MASK_FC                     (0xFCU)    /**< the value used for mask the scalar value */
#define PKCD_ECDHX_MASK_80                     (0x80U)    /**< the value used for mask the scalar value */

/*! \}*/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDECDHXTypesInternal
 * \{******************************************************************/

/** Work area structure of PKCC RAM */
typedef struct
{
    uint8_t                 pdt_p[PKCD_ECDHX_WORKUNITPDT];        /**< A 8 byte work area for T mod p */
    uint32_t                pbytes;                               /**< A byte length of field p */
    uint32_t                offval;                               /**< A size of padding of DL in bytes */
    uint8_t                 pwork[PKCD_ECDHX_WORKUNIT];           /**< A 64byte work area which stores p */
    uint8_t                 r2work[PKCD_ECDHX_WORKUNIT];          /**< A 64byte work area which stores R^2 mod p */
    uint8_t                 x1rwork[PKCD_ECDHX_WORKUNIT];         /**< A 64byte work area which stores x1*R */
    uint8_t                 x2rwork[PKCD_ECDHX_WORKUNIT];         /**< A 64byte work area which stores x2*R */
    uint8_t                 z2rwork[PKCD_ECDHX_WORKUNIT];         /**< A 64byte work area which stores z2*R  */
    uint8_t                 x3rwork[PKCD_ECDHX_WORKUNIT];         /**< A 64byte work area which stores x3*R  */
    uint8_t                 z3rwork[PKCD_ECDHX_WORKUNIT];         /**< A 64byte work area which stores Z3*R  */
    uint8_t                 arwork[PKCD_ECDHX_WORKUNIT];          /**< A 64byte work area which stores a*R  */
    uint8_t                 brwork[PKCD_ECDHX_WORKUNIT];          /**< A 64byte work area which stores b*R  */
    uint8_t                 erwork[PKCD_ECDHX_WORKUNIT];          /**< A 64byte work area which stores e*R  */
    uint8_t                 a2rwork[PKCD_ECDHX_WORKUNIT];         /**< A 64byte work area which stores a^2*R  */
    uint8_t                 b2rwork[PKCD_ECDHX_WORKUNIT];         /**< A 64byte work area which stores b^2*R  */
    uint8_t                 darwork[PKCD_ECDHX_WORKUNIT];         /**< A 64byte work area which stores da*R  */
    uint8_t                 cbrwork[PKCD_ECDHX_WORKUNIT];         /**< A 64byte work area which stores cb*R  */
    uint8_t                 a24rwork[PKCD_ECDHX_WORKUNIT];        /**< A 64byte work area which stores a24*R */
    uint8_t                 work1[PKCD_ECDHX_WORKUNIT];           /**< A 64byte work area */
    uint8_t                 work2[PKCD_ECDHX_WORKUNIT];           /**< A 64byte work area */
    uint8_t                 work3[PKCD_ECDHX_WORKUNIT];           /**< A 64byte work area */
    uint8_t                 wwork[PKCD_ECDHX_WORKUNITPDW];        /**< A 192byte work area */
    uint32_t                swap;                                 /**< A value used in X25519/X448 calculation */
} st_ecdhx_pkccram_t;


/** X25519 domain parameter structure of internal */
typedef struct
{
    /** Byte length of a curve len (must be a multiple of 4) */
    uint32_t len;

    /** Field order (the order of the finite field GF(p)) */
    uint8_t  field[PKCD_ECDHX_CURVE_LEN_MAX];

    /** (A-2)/4 ("A" is a coefficient of a Montgomery curve: v^2 = u^3 + A*u^2 + u) */
    uint8_t  a24[PKCD_ECDHX_CURVE_LEN_MAX];

    /** Base point u */
    uint8_t  base_u[PKCD_ECDHX_CURVE_LEN_MAX];
} st_ecdhx_curve_t;

/** Scalar value structure of internal */
typedef struct
{
    uint32_t len;                      /**< A byte length of a scalar */
    uint8_t  scalar[PKCD_ECDHX_CURVE_LEN_MAX]; /**< A buffer of scalar */
} st_ecdhx_scalar_t;

/** Elliptic curve affine coordinate structure of internal */
typedef struct
{
    uint32_t len;                   /**< A byte length of a point */
    uint8_t  u[PKCD_ECDHX_CURVE_LEN_MAX];  /**< A buffer of u-coordinate data */
} st_ecdhx_point_t;


/** A parameter structure for scalar multiplication */
typedef struct
{
    st_ecdhx_scalar_t scalar;        /**< A scalar value to be multiplied */
    st_ecdhx_point_t  point;         /**< An u-coordinate value to be multiplied */
    uint32_t          executed_bits; /**< The number of processed bits */
    st_ecdhx_curve_t  curve;         /**< Curve information */
    st_ecdhx_point_t  point_out;     /**< An output u-coordinate value */
} st_ecdhx_smult_buffer_t;

/** A work area strucrure */
typedef struct
{
    uint8_t * p_wk1;     /**< An address of work area 1 */
    uint8_t * p_wk2;     /**< An address of work area 2 */
    uint8_t * p_wk3;     /**< An address of work area 3 */
} st_ecdhx_wks_t;

/*! \}*/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDECDHXVariablesInternal
 * \{******************************************************************/
/** PKCCRAM address cast in st_ecdhx_pkccram_t */
static st_ecdhx_pkccram_t * const sp_ecdhx_pkccram = ((st_ecdhx_pkccram_t*)PKCD_CMN_PKCCRAM);

/*! \}*/

static pkcd_ret_t r_pkcd_ecdhx_chk_scalar_mult_init_arg (const st_pkc_select_data_t * const p_scalar_or_privkey,
                                                        const st_pkc_select_data_t * const p_point_or_pubkey,
                                                        const st_pkc_ecdhx_ctx_t * const p_ctx,
                                                        uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_ecdhx_chk_key_wrap_arg (const st_pkc_ecdhx_ctx_t * const p_ctx,
                                                uint8_t * const p_buf,
                                                const st_pkc_data_t * const p_priv_key_raw,
                                                const st_pkc_data_t * const p_pub_key_raw,
                                                const void * const p_priv_key,
                                                const void * const p_pub_key);
static pkcd_ret_t r_pkcd_ecdhx_chk_curve_info (const st_pkc_ec_curve_t * const p_curve_info);
static pkcd_ret_t r_pkcd_ecdhx_chk_pub_key (const st_pkcd_ecdhx_key_bin_t * const p_pub_key_bin,
                                            const uint32_t curve_len);
static pkcd_ret_t r_pkcd_ecdhx_chk_priv_key (const st_pkcd_ecdhx_key_bin_t * const p_priv_key_bin,
                                                const uint32_t curve_len);
static pkcd_ret_t r_pkcd_ecdhx_chk_priv_scalar (const st_pkc_data_t * const p_priv_scalar, const uint32_t curve_len);
static pkcd_ret_t r_pkcd_ecdhx_chk_point (const st_pkc_data_t * const p_point, const uint32_t curve_len);
static pkcd_ret_t r_pkcd_ecdhx_precalc_params (st_ecdhx_smult_buffer_t * const p_prm);
static pkcd_ret_t r_pkcd_ecdhx_cswap (const uint8_t * const p_wk1, const uint8_t * const p_wk2,
                                        uint8_t * const p_wk1_temp, uint8_t * const p_wk2_temp);
static pkcd_ret_t r_pkcd_ecdhx_point_smult (st_ecdhx_point_t * const p_point_out,
                                            uint8_t * const p_scalar, st_ecdhx_wks_t * const p_works,
                                            const uint32_t processing_bits, uint32_t * const p_executed_bits);
static pkcd_ret_t r_pkcd_ecdhx_point_smult_loop (st_ecdhx_wks_t * const p_works);
static void r_pkcd_ecdhx_set_curve_params (st_ecdhx_curve_t * const p_curve,
                                        const st_pkc_ec_curve_t * const p_curve_info);
static void r_pkcd_ecdhx_encode_data (uint8_t * const p_data, const uint32_t len);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcdhXScalarMultInit
**********************************************************************************************************************/
pkcd_ret_t   R_PKCD_EcdhXScalarMultInit(const st_pkc_select_data_t* const p_scalar_or_privkey,
                                        const st_pkc_select_data_t* const p_point_or_pubkey,
                                        const st_pkc_ecdhx_ctx_t* const p_ctx,
                                        uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ecdhx_smult_buffer_t * const p_prm = (st_ecdhx_smult_buffer_t*)p_buf; // No problem with casting to pointer size
    const st_pkcd_ecdhx_key_bin_t * p_key_bin;

    /* Check argument */
    ret = r_pkcd_ecdhx_chk_scalar_mult_init_arg(p_scalar_or_privkey, p_point_or_pubkey, p_ctx, p_buf);

    if (PKCD_RET_OK == ret)
    {
        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* Clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        /* Clear buffer */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_ecdhx_smult_buffer_t));

        /* clear caluculation bits */
        p_prm->executed_bits = 0U;

        /* Copy patameters */
        if (PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
        {
            /* No problem with casting to pointer size */
            p_key_bin = (st_pkcd_ecdhx_key_bin_t*)p_scalar_or_privkey->p_sel_data->p_data;

            p_prm->scalar.len = p_key_bin->key_length;
            r_pkcd_cmn_cpy_word(p_prm->scalar.scalar, p_key_bin->p_private, p_key_bin->key_length);
        }
        else
        {
            p_prm->scalar.len = p_scalar_or_privkey->p_sel_data->len;
            r_pkcd_cmn_cpy_word(p_prm->scalar.scalar, p_scalar_or_privkey->p_sel_data->p_data,
                                    p_scalar_or_privkey->p_sel_data->len );
        }

        if (PKCD_ECDHX_CURVE25519_LEN == p_ctx->p_curve_info->curve_len)
        {
            /* No problem for casting to support usual arithmetic conversion */
            p_prm->scalar.scalar[0U] = (uint8_t)((uint32_t)p_prm->scalar.scalar[0U] & (uint32_t)PKCD_ECDHX_MASK_F8);
            p_prm->scalar.scalar[p_prm->scalar.len - 1U] =
                            (uint8_t)((uint32_t)p_prm->scalar.scalar[p_prm->scalar.len - 1U] & /* Same cast as above */
                            (uint32_t)PKCD_ECDHX_MASK_7F);                                     /* Same cast as above */
            p_prm->scalar.scalar[p_prm->scalar.len - 1U] =
                            (uint8_t)((uint32_t)p_prm->scalar.scalar[p_prm->scalar.len - 1U] | /* Same cast as above */
                            (uint32_t)PKCD_ECDHX_MASK_40);                                     /* Same cast as above */
        }
        else
        {
            /* No problem for casting to support usual arithmetic conversion */
            p_prm->scalar.scalar[0U] = (uint8_t)((uint32_t)p_prm->scalar.scalar[0U] & (uint32_t)PKCD_ECDHX_MASK_FC);
            p_prm->scalar.scalar[p_prm->scalar.len - 1U] =
                            (uint8_t)((uint32_t)p_prm->scalar.scalar[p_prm->scalar.len - 1U] | /* Same cast as above */
                            (uint32_t)PKCD_ECDHX_MASK_80);                                     /* Same cast as above */
        }
        r_pkcd_ecdhx_encode_data(p_prm->scalar.scalar, p_prm->scalar.len);

        if (PKC_DATA_TYPE_KEY_BINARY == p_point_or_pubkey->data_type)
        {
            /* No problem with casting to pointer size */
            p_key_bin = (st_pkcd_ecdhx_key_bin_t*)p_point_or_pubkey->p_sel_data->p_data;

            p_prm->point.len = p_key_bin->key_length;
            r_pkcd_cmn_cpy_word(p_prm->point.u, p_key_bin->p_public, p_key_bin->key_length);
        }
        else
        {
            p_prm->point.len = p_point_or_pubkey->p_sel_data->len;
            r_pkcd_cmn_cpy_word(p_prm->point.u, p_point_or_pubkey->p_sel_data->p_data,
                                    p_point_or_pubkey->p_sel_data->len );

        }

        if (PKCD_ECDHX_CURVE25519_LEN == p_ctx->p_curve_info->curve_len)
        {
            p_prm->point.u[p_prm->point.len - 1U] =
                        /* No problem for casting to support usual arithmetic conversion */
                        (uint8_t)((uint32_t)p_prm->point.u[p_prm->point.len - 1U] & (uint32_t)PKCD_ECDHX_MASK_7F);
        }
        else
        {
            /* Do nothing */
        }
        r_pkcd_ecdhx_encode_data(p_prm->point.u, p_prm->point.len);

        /* Parse curve */
        r_pkcd_ecdhx_set_curve_params(&p_prm->curve, p_ctx->p_curve_info);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EcdhXScalarMultInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EcdhXScalarMultOperate
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EcdhXScalarMultOperate(const st_pkc_ecdhx_ctx_t* const p_ctx,
                                        uint8_t* const p_buf,
                                        const uint32_t processing_bits)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ecdhx_wks_t wks;
    st_ecdhx_smult_buffer_t * const p_prm = (st_ecdhx_smult_buffer_t*)p_buf; // No problem with casting to pointer size

    /* Check arguments */
    if ((NULL != p_ctx) && (NULL != p_buf))
    {
        ret = r_pkcd_ecdhx_chk_curve_info(p_ctx->p_curve_info);

        if (PKCD_RET_OK == ret)
        {
            if ((PKCD_CMN_PROCESSING_BITS_MIN <= processing_bits)
                && (PKCD_CMN_PROCESSING_BITS_MAX >= processing_bits))
            {
                /* Clear async operation flag */
                r_pkcd_cmn_clr_async_ope();

                /* Initialize works structure */
                wks.p_wk1 = PKCD_ECDHX_WORK1;
                wks.p_wk2 = PKCD_ECDHX_WORK2;
                wks.p_wk3 = PKCD_ECDHX_WORK3;

                if (0U == p_prm->executed_bits)
                {
                    /* initialise params */
                    PKCD_ECDHX_PBYTES = p_prm->curve.len;
                    PKCD_ECDHX_OFFSET_VAL = PKCD_ECDHX_WORKUNIT - p_prm->curve.len;
                    ret |= r_pkcd_ecdhx_precalc_params(p_prm);
                }
                else
                {
                    /* Do nothing */
                }

                ret |= r_pkcd_ecdhx_point_smult(&p_prm->point_out, p_prm->scalar.scalar, &wks,
                                                processing_bits, &p_prm->executed_bits);

                /* Calculate the return value */
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

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EcdhXScalarMultOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_EcdhXScalarMultFinal
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EcdhXScalarMultFinal(const st_pkc_ecdhx_ctx_t* const p_ctx,
                                    uint8_t* const p_buf,
                                    st_pkc_data_t* const p_point)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_ecdhx_smult_buffer_t * const p_prm = (st_ecdhx_smult_buffer_t*)p_buf; // No problem with casting to pointer size

    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_point))
    {
        if (NULL != p_point->p_data)
        {
            ret = r_pkcd_ecdhx_chk_curve_info(p_ctx->p_curve_info);

            if (PKCD_RET_OK == ret)
            {
                if ((p_point->len) >= (p_ctx->p_curve_info->curve_len))
                {
                    r_pkcd_ecdhx_encode_data(p_prm->point_out.u, p_prm->point_out.len);

                    p_point->len = p_prm->point_out.len;
                    r_pkcd_cmn_cpy_word(p_point->p_data, p_prm->point_out.u, p_point->len);
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
* End of function R_PKCD_EcdhXScalarMultFinal()
**********************************************************************************************************************/


/**********************************************************************************************************************
* Function Name : R_PKCD_EcdhXKeyWrap
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_EcdhXKeyWrap(const st_pkc_ecdhx_ctx_t* const p_ctx,
                            uint8_t* const p_buf,
                            const st_pkc_data_t* const p_priv_key_raw,
                            const st_pkc_data_t* const p_pub_key_raw,
                            void* const p_priv_key,
                            void* const p_pub_key)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_pkcd_ecdhx_key_bin_t * p_priv_key_bin;
    st_pkcd_ecdhx_key_bin_t * p_pub_key_bin;

    ret = r_pkcd_ecdhx_chk_key_wrap_arg(p_ctx, p_buf, p_priv_key_raw, p_pub_key_raw, p_priv_key, p_pub_key);

    if (PKCD_RET_OK == ret)
    {
        p_priv_key_bin = (st_pkcd_ecdhx_key_bin_t*)p_priv_key; /* No problem with casting to pointer size */
        p_pub_key_bin = (st_pkcd_ecdhx_key_bin_t*)p_pub_key;   /* Same cast as above */

        /* Wrap the private key */
        p_priv_key_bin->key_length = p_ctx->p_curve_info->curve_len;
        r_pkcd_cmn_cpy_by_64bounds(p_priv_key_bin->p_private, p_priv_key_raw->p_data, p_ctx->p_curve_info->curve_len);
        r_pkcd_cmn_cpy_by_64bounds(p_priv_key_bin->p_public, p_pub_key_raw->p_data, p_ctx->p_curve_info->curve_len);

        /* Wrap the public key */
        p_pub_key_bin->key_length = p_ctx->p_curve_info->curve_len;
        r_pkcd_cmn_cpy_by_64bounds(p_pub_key_bin->p_public, p_pub_key_raw->p_data, p_ctx->p_curve_info->curve_len);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_EcdhXKeyWrap()
**********************************************************************************************************************/


/**********************************************************************************************************************
* Function Name : r_pkcd_ecdhx_chk_scalar_mult_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECDHXAPIsInternal
 * \brief       #R_PKCD_EcScalarMultInit argument check function.
 * \param       [in]   p_scalar_or_privkey      A scalar value or a driver-dependent format of a private key.
 * \param       [in]   p_point_or_pubkey        An u-coordinate value or a driver-dependent format of a public key.
 * \param       [in]   p_ctx                    A pointer to a structure that summarizes parameters required by X25519.
 * \param       [in]   p_buf                    A Buffer used for internal processing of PKC-Driver
 * \return
 *  - #PKCD_RET_OK:                       Successfully done.
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed.
 *  - #PKCD_RET_PARAMERR:                 NULL arguments entered.
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered.
 *  - #PKCD_RET_KEY_PARAMERR:             A driver-dependent format of key data including invalid parameters entered
 *  - #PKCD_RET_KEY_LENGTHERR:            A driver-dependent format of key data including invalid key length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the arguments received by the #R_PKCD_EcdhXScalarMultInit function.
 *  - Call #r_pkcd_ecdhx_chk_curve_info to check the curve information of p_ctx.
 *  - Call #r_pkcd_ecdhx_chk_priv_key to check a given private key in the argument p_scalar_or_privkey.
 *  - Call #r_pkcd_ecdhx_chk_priv_scalar to check a given scalar value in the argument p_scalar_or_privkey.
 *  - Call #r_pkcd_ecdhx_chk_pub_key to check a given public key in the argument p_point_or_pubkey.
 *  - Call #r_pkcd_ecdhx_chk_point to check a given u-coordinate value in the argument p_point_or_pubkey.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_scalar_or_privkey) and (NULL != p_point_or_pubkey)
 *     and (NULL != p_ctx) and (NULL != p_buf)
 *      -# Check the curve information with #r_pkcd_ecdhx_chk_curve_info,
 *         and set ret to the result.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# If (#PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
 *          -# If (NULL != p_scalar_or_privkey->p_sel_data)
 *            -# Check if given private key binary is valid with #r_pkcd_ecdhx_chk_priv_key,
 *               and set ret to the result.
 *          -# Else
 *            -# Set ret to #PKCD_RET_PARAMERR.
 *        -# Else if (#PKC_DATA_TYPE_BYTE == p_scalar_or_privkey->data_type)
 *          -# Check if given private scalar is valid with #r_pkcd_ecdhx_chk_priv_scalar,
 *             and set ret to the result.
 *        -# Else
 *          -# Set ret to #PKCD_RET_PARAMERR.
 *        -# If (#PKCD_RET_OK == ret)
 *          -# If (#PKC_DATA_TYPE_KEY_BINARY == p_point_or_pubkey->data_type)
 *            -# If (NULL != p_point_or_pubkey->p_sel_data)
 *              -# Check if given public key binary is valid with #r_pkcd_ecdhx_chk_pub_key,
 *                 and set ret to the result.
 *            -# Else
 *              -# Set ret to #PKCD_RET_PARAMERR.
 *          -# Else if (#PKC_DATA_TYPE_BYTE == p_point_or_pubkey->data_type)
 *            -# Check if given public point data is valid with #r_pkcd_ecdhx_chk_point,
 *               and set ret to the result.
 *          -# Else
 *            -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ecdhx_chk_scalar_mult_init_arg(const st_pkc_select_data_t* const p_scalar_or_privkey,
                                                        const st_pkc_select_data_t* const p_point_or_pubkey,
                                                        const st_pkc_ecdhx_ctx_t* const p_ctx,
                                                        uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if ((NULL != p_scalar_or_privkey) && (NULL != p_point_or_pubkey) && (NULL != p_ctx) && (NULL != p_buf))
    {
        ret = r_pkcd_ecdhx_chk_curve_info(p_ctx->p_curve_info);

        if (PKCD_RET_OK == ret)
        {
            if (PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
            {
                if (NULL != p_scalar_or_privkey->p_sel_data)
                {
                    ret = r_pkcd_ecdhx_chk_priv_key(
                                            /* No problem with casting to pointer size */
                                            (const st_pkcd_ecdhx_key_bin_t*)p_scalar_or_privkey->p_sel_data->p_data,
                                            p_ctx->p_curve_info->curve_len);
                }
                else
                {
                    ret = PKCD_RET_PARAMERR;
                }
            }
            else if (PKC_DATA_TYPE_BYTE == p_scalar_or_privkey->data_type)
            {
                ret = r_pkcd_ecdhx_chk_priv_scalar(p_scalar_or_privkey->p_sel_data, p_ctx->p_curve_info->curve_len);
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
                        ret = r_pkcd_ecdhx_chk_pub_key(
                                            /* No problem with casting to pointer size */
                                            (const st_pkcd_ecdhx_key_bin_t*)p_point_or_pubkey->p_sel_data->p_data,
                                            p_ctx->p_curve_info->curve_len);
                    }
                    else
                    {
                        ret = PKCD_RET_PARAMERR;
                    }
                }
                else if (PKC_DATA_TYPE_BYTE == p_point_or_pubkey->data_type)
                {
                    ret = r_pkcd_ecdhx_chk_point(p_point_or_pubkey->p_sel_data, p_ctx->p_curve_info->curve_len);
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
* End of function r_pkcd_ecdhx_chk_scalar_mult_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ecdhx_chk_key_wrap_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECDHXAPIsInternal
 * \brief       #R_PKCD_EcdhXKeyWrap argument check function.
 * \param       [in] p_ctx              A pointer to a structure that summarizes the parameters required by X25519.
 * \param       [in] p_buf              A buffer used for internal processing of PKC-Driver.
 * \param       [in] p_priv_key_raw     Byte array of scalar data.
 * \param       [in] p_pub_key_raw      Byte array of u-coordinate data.
 * \param       [in] p_priv_key         A driver-dependent format of a private key structure.
 * \param       [in] p_pub_key          A driver-dependent format of a public key structure.
 * \return
 *  - #PKCD_RET_OK:                       Successfully done.
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed.
 *  - #PKCD_RET_PARAMERR:                 NULL arguments entered.
 *  - #PKCD_RET_LENGTHERR:                Invalid curve length entered.
 *  - #PKCD_RET_KEY_PARAMERR:             A driver-dependent format of key data including invalid parameters entered.
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
 *  - Call #r_pkcd_ecdhx_chk_priv_scalar to check the argument p_priv_key_raw.
 *  - Call #r_pkcd_ecdhx_chk_point to check the argument p_pub_key_raw.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_priv_key) and (NULL != p_pub_key)
 *    -# If (NULL != p_ctx->p_curve_info)
 *      -# If p_ctx->p_curve_info->curve_len is equal to curve length of curve25519
 *        -# Set ret to #PKCD_RET_OK
 *      -# Else
 *        -# Set ret to #PKCD_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# Check the argument p_priv_key_raw with #r_pkcd_ecdhx_chk_priv_scalar, and set ret to the return value.
 *      -# If #PKCD_RET_OK == ret
 *        -# Check the argument p_pub_key_raw with #r_pkcd_ecdhx_chk_point, and set ret to the return value.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# If #PKCD_RET_OK == ret
 *    -# If a pointer to the private key in the argument p_priv_key is not NULL,
 *       and a pointer to the public key in the argument p_priv_key is not NULL
 *      -# If a pointer to the publuc key is not NULL
 *        -# Do nothing.
 *      -# Else
 *        -# Set ret to #PKCD_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ecdhx_chk_key_wrap_arg(const st_pkc_ecdhx_ctx_t* const p_ctx,
                                                uint8_t* const p_buf,
                                                const st_pkc_data_t* const p_priv_key_raw,
                                                const st_pkc_data_t* const p_pub_key_raw,
                                                const void* const p_priv_key,
                                                const void* const p_pub_key)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const st_pkcd_ecdhx_key_bin_t * p_priv_key_bin;
    const st_pkcd_ecdhx_key_bin_t * p_pub_key_bin;

    /* Check if the arguments are NULL */
    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_priv_key) && (NULL != p_pub_key))
    {
        if (NULL != p_ctx->p_curve_info)
        {
            switch (p_ctx->p_curve_info->curve_len)
            {
                case PKCD_ECDHX_CURVE25519_LEN :
                case PKCD_ECDHX_CURVE448_LEN :
                {
                    ret = PKCD_RET_OK;
                    break;
                }

                default:
                {
                    ret = PKCD_RET_LENGTHERR;
                    break;
                }
            }
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }


        if (PKCD_RET_OK == ret)
        {
            ret = r_pkcd_ecdhx_chk_priv_scalar(p_priv_key_raw, p_ctx->p_curve_info->curve_len);

            if (PKCD_RET_OK == ret)
            {
                ret = r_pkcd_ecdhx_chk_point(p_pub_key_raw, p_ctx->p_curve_info->curve_len);
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
        /* No problem with casting to pointer size */
        p_priv_key_bin = (const st_pkcd_ecdhx_key_bin_t*)p_priv_key;

        if ((NULL != p_priv_key_bin->p_private) && (NULL != p_priv_key_bin->p_public))
        {
            /* No problem with casting to pointer size */
            p_pub_key_bin = (const st_pkcd_ecdhx_key_bin_t*)p_pub_key;

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

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ecdhx_chk_key_wrap_arg()
**********************************************************************************************************************/


/**********************************************************************************************************************
* Function Name : r_pkcd_ecdhx_chk_curve_info
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECDHXAPIsInternal
 * \brief       Check input curve information
 * \param       [in]   p_curve_info   A pointer to the strucrture that summarize curve infomation.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_PARAMERR:               NULL arguments entered
 *  - #PKCD_RET_LENGTHERR:              Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_ECDHX_CURVE25519_LEN, #PKCD_ECDHX_CURVE_PARAM_NUM
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
 *    -# If p_curve_info->curve_params.p_data != NULL then
 *      -# If p_curve_info->curve_len == #PKCD_ECDHX_CURVE25519_LEN then
 *        -# Set ret to #PKCD_RET_OK.
 *      -# Else If p_curve_info->curve_len == #PKCD_ECDHX_CURVE448_LEN then
 *        -# Set ret to #PKCD_RET_OK.
 *      -# Else
 *        -# Set ret to #PKCD_RET_LENGTHERR.
 *      -# If ret == #PKCD_RET_OK
 *        -# If p_curve_info->curve_params.len == (curve length * the number of curve parameters)
 *          -# Do nothing.
 *        -# Else
 *          -# Set ret to #PKCD_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ecdhx_chk_curve_info(const st_pkc_ec_curve_t* const p_curve_info)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* Check if the argument is null */
    if (NULL != p_curve_info)
    {
        if (NULL != p_curve_info->curve_params.p_data)
        {
            /* Check curve_len */
            switch (p_curve_info->curve_len)
            {
                case PKCD_ECDHX_CURVE25519_LEN :
                case PKCD_ECDHX_CURVE448_LEN :
                {
                    ret = PKCD_RET_OK;
                    break;
                }

                default:
                {
                    ret = PKCD_RET_LENGTHERR;
                    break;
                }
            }

            if (PKCD_RET_OK == ret)
            {
                if ((p_curve_info->curve_len * PKCD_ECDHX_CURVE_PARAM_NUM) == p_curve_info->curve_params.len)
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
* End of function r_pkcd_ecdhx_chk_curve_info()
**********************************************************************************************************************/


/**********************************************************************************************************************
* Function Name : r_pkcd_ecdhx_chk_pub_key
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECDHXAPIsInternal
 * \brief       Check the public key parameters
 * \param       [in]   p_pub_key_bin  The driver-dependent format of X25519 public key structure.
 * \param       [in]   curve_len      A curve length.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_PARAMERR:               NULL arguments entered.
 *  - #PKCD_RET_KEY_PARAMERR:           A driver-dependent format of key data including invalid parameters entered.
 *  - #PKCD_RET_KEY_LENGTHERR:          A driver-dependent format of key data including invalid key length entered.
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
 *    -# If p_pub_key->p_public != NULL
 *      -# If p_pub_key_bin->key_length is equal to curve length then
 *        -# Set ret to #PKCD_RET_OK.
 *      -# Else
 *        -# Set ret to #PKCD_RET_KEY_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ecdhx_chk_pub_key(const st_pkcd_ecdhx_key_bin_t* const p_pub_key_bin, const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* Check if the argument is null */
    if (NULL != p_pub_key_bin)
    {
        if (NULL != p_pub_key_bin->p_public)
        {
            if (curve_len == p_pub_key_bin->key_length)
            {
                ret = PKCD_RET_OK;
            }
            else
            {
                ret = PKCD_RET_KEY_LENGTHERR;
            }
        }
        else
        {
            ret = PKCD_RET_KEY_PARAMERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ecdhx_chk_pub_key()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ecdhx_chk_priv_key
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECDHXAPIsInternal
 * \brief       Check the private key parameters
 * \param       [in]   p_priv_key_bin   A driver-dependent format of private key data.
 * \param       [in]   curve_len        Curve length.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_PARAMERR:               NULL arguments entered.
 *  - #PKCD_RET_KEY_PARAMERR:           A driver-dependent format of key data including invalid parameters entered.
 *  - #PKCD_RET_KEY_LENGTHERR:          A driver-dependent format of key data including invalid key length entered.
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
 *    -# If p_priv_key->p_private != NULL
 *      -# If p_priv_key_bin->key_length is equal to curve length then
 *        -# Set ret to #PKCD_RET_OK.
 *      -# Else
 *        -# Set ret to #PKCD_RET_KEY_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_KEY_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ecdhx_chk_priv_key(const st_pkcd_ecdhx_key_bin_t* const p_priv_key_bin,
                                            const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* Check if the argument is null */
    if (NULL != p_priv_key_bin)
    {
        if (NULL != p_priv_key_bin->p_private)
        {
            if (curve_len == p_priv_key_bin->key_length)
            {
                ret = PKCD_RET_OK;
            }
            else
            {
                ret = PKCD_RET_KEY_LENGTHERR;
            }
        }
        else
        {
            ret = PKCD_RET_KEY_PARAMERR;
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ecdhx_chk_priv_key()
**********************************************************************************************************************/


/**********************************************************************************************************************
* Function Name : r_pkcd_ecdhx_chk_priv_scalar
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECDHXAPIsInternal
 * \brief       Check the private scalar parameters
 * \param       [in]   p_priv_scalar    A scalar value.
 * \param       [in]   curve_len        A curve length.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_PARAMERR:               Invalid arguments entered.
 *  - #PKCD_RET_LENGTHERR:              Invalid length entered.
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
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If p_priv_scalar != NULL
 *     -# If (NULL != p_priv_scalar->p_data)
 *       -# If (p_priv_scalar->len == curve_len)
 *         -# Set ret to #PKCD_RET_OK
 *       -# Else
 *         -# Set ret to #PKCD_RET_LENGTHERR
 *     -# Else
 *       -# Set ret to #PKCD_RET_PARAMERR
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ecdhx_chk_priv_scalar(const st_pkc_data_t* const p_priv_scalar, const uint32_t curve_len)
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
* End of function r_pkcd_ecdhx_chk_priv_scalar()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ecdhx_chk_point
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECDHXAPIsInternal
 * \brief       Check the point parameters
 * \param       [in]   p_point        A point to be multiplied.
 * \param       [in]   curve_len      Curve length.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_PARAMERR:               Invalid arguments entered.
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
 *    -# If NULL != p_point->p_data
 *      -# If (curve_len != p_point->len)
 *        -# Set ret to #PKCD_RET_OK
 *      -# Else
 *        -# Set ret to #PKCD_RET_LENGTHERR
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ecdhx_chk_point(const st_pkc_data_t* const p_point, const uint32_t curve_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* check if the p_point is NULL */
    if (NULL != p_point)
    {
        if (NULL != p_point->p_data)
        {
            if (p_point->len == curve_len)
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
* End of function r_pkcd_ecdhx_chk_point()
**********************************************************************************************************************/


/**********************************************************************************************************************
* Function Name : r_pkcd_ecdhx_precalc_params
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDECDHXAPIsInternal
 * \brief       Prepare pre-calculation values.
 * \param       [in] p_prm     Parameters to be calculated
 * \return
 *  - #PKCD_RET_OK:                       Successfully done.
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_ECDHX_WWORK, #PKCD_ECDHX_PBYTES, #PKCD_ECDHX_OFFSET_VAL, #PKCD_ECDHX_PWORK,
 *    #PKCD_ECDHX_WORK1, #PKCD_ECDHX_R2WORK, #PKCD_ECDHX_PDT_P, #PKCD_ECDHX_WORKUNIT
 *    #PKCD_ECDHX_A24RWORK, #PKCD_ECDHX_X1RWORK, #PKCD_ECDHX_X2RWORK, #PKCD_ECDHX_X3RWORK,
 *    #PKCD_ECDHX_Z3RWORK, #PKCD_ECDHX_WORK2
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPW
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB,
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT (Used by sub functions)
 * \par Precondition
 *  - #PKCD_ECDHX_OFFSET_VAL, #PKCD_ECDHX_PBYTES must have been prepared.
 * \par Detailed description
 *      - Prepare pre-calculation values.
 *      - Calculate T = -p^(-1), a24*R and initial values of x1*R, x2*R, x3*R, z3*R.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# Set #PKCD_ECDHX_WWORK address as the top of PKCC DPW area.
 *  -# Set the size of field p as the PKCC DL and BDL value.
 *  -# Calculate (p_prm->point.u - p) with #r_pkcd_cmn_AMinusB.
 *  -# If (p_prm->point.u - p) >= 0 then
 *    -# Set (p_prm->point.u - p) into p_prm->point.u.
 *  -# Copy the field of the given curve p to #PKCD_ECDHX_PWORK area.
 *  -# Copy a24 to the work area #PKCD_ECDHX_WORK1.
 *  -# Calculate {T over p} = -1/p mod 2^64 with #r_pkcd_cmn_MinusCInv and set ret to the return value.
 *  -# Calculate R^2 mod p with #r_pkcd_cmn_R2ModC and add the return value to ret.
 *  -# Set R^2 mod p into the work area #PKCD_ECDHX_R2WORK with #r_pkcd_cmn_cpy_by_64bounds.
 *  -# Calculate a24*R mod p with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 *  -# Set a24*R mod p into work area #PKCD_ECDHX_A24RWORK with #r_pkcd_cmn_cpy_by_64bounds.
 *  -# Calculate x1*R mod p with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 *  -# Set x1*R mod p into work area #PKCD_ECDHX_X1RWORK with #r_pkcd_cmn_cpy_by_64bounds.
 *  -# Calculate x2*R mod p with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 *  -# Set x2*R mod p into work area #PKCD_ECDHX_X2RWORK with #r_pkcd_cmn_cpy_by_64bounds.
 *  -# Set x3*R mod p into work area #PKCD_ECDHX_X3RWORK with #r_pkcd_cmn_cpy_word.
 *  -# Set z3*R mod p into work area #PKCD_ECDHX_Z3RWORK with #r_pkcd_cmn_cpy_word.
 *  -# Extract the return value with #r_pkcd_cmn_extract_result Sand set ret to it.
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ecdhx_precalc_params(st_ecdhx_smult_buffer_t* const p_prm)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t is_negative;

    PKCD_CMN_ICUM_PKCCB0DPW = PKCD_ECDHX_FORMAT_DP_DPW(PKCD_ECDHX_WWORK);
    r_pkcd_cmn_set_dl(PKCD_ECDHX_PBYTES, PKCD_ECDHX_PBYTES);

    /* copy the order of GF(p) */
    r_pkcd_cmn_cpy_by_64bounds((PKCD_ECDHX_PWORK + PKCD_ECDHX_OFFSET_VAL), p_prm->curve.field, PKCD_ECDHX_PBYTES);

    /* u-coordinate mod p by the combination of subtract functions */
    r_pkcd_cmn_cpy_by_64bounds((PKCD_ECDHX_WORK1 + PKCD_ECDHX_OFFSET_VAL), p_prm->point.u, PKCD_ECDHX_PBYTES);
    ret = r_pkcd_cmn_AMinusB(PKCD_ECDHX_WORK1, PKCD_ECDHX_PWORK, PKCD_ECDHX_WORK2, PKCD_ECDHX_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds((uint8_t*)&is_negative, /* Casts that do not exceed the size of the type are fine */
                        ((PKCD_ECDHX_WORK2 + PKCD_ECDHX_OFFSET_VAL) - PKCD_CMN_PKCCBOUNDARYSIZE) + sizeof(uint32_t),
                        sizeof(uint32_t));

    /* Clear sign area */
    r_pkcd_cmn_ram_clr(((PKCD_ECDHX_WORK2 + PKCD_ECDHX_OFFSET_VAL) - PKCD_CMN_PKCCBOUNDARYSIZE),
                        PKCD_CMN_PKCCBOUNDARYSIZE);

    if (0U == is_negative)
    {
        r_pkcd_cmn_cpy_by_64bounds(p_prm->point.u, PKCD_ECDHX_WORK2 + PKCD_ECDHX_OFFSET_VAL, PKCD_ECDHX_PBYTES);
    }
    else
    {
        /* Do nothing */
    }

    /* copy the order of a24 */
    r_pkcd_cmn_cpy_by_64bounds((PKCD_ECDHX_WORK1 + PKCD_ECDHX_OFFSET_VAL), p_prm->curve.a24, PKCD_ECDHX_PBYTES);

    /* calc T = -p^-1 mod 2^64 */
    ret |= r_pkcd_cmn_MinusCInv(PKCD_ECDHX_PWORK, (uint8_t*)PKCD_ECDHX_FORMAT_DP_DPT(PKCD_ECDHX_PDT_P),
                                PKCD_ECDHX_WORKUNIT);

    /* calc R^2 mod p */
    ret |= r_pkcd_cmn_R2ModC(PKCD_ECDHX_PWORK, PKCD_ECDHX_R2WORK, PKCD_ECDHX_WORKUNIT);

    /* calc a24R mod p */
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_ECDHX_WORK1, PKCD_ECDHX_R2WORK, PKCD_ECDHX_PWORK,
                                    PKCD_ECDHX_A24RWORK, PKCD_ECDHX_WORKUNIT);

    /* calc x1*R mod p (=u*R) */
    r_pkcd_cmn_cpy_by_64bounds((PKCD_ECDHX_WORK1 + PKCD_ECDHX_OFFSET_VAL), p_prm->point.u , PKCD_ECDHX_PBYTES);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_ECDHX_WORK1, PKCD_ECDHX_R2WORK, PKCD_ECDHX_PWORK,
                                    PKCD_ECDHX_X1RWORK, PKCD_ECDHX_WORKUNIT);

    /* initialize x2*R mod p (=R) */
    ret |= r_pkcd_cmn_ARInvModC(PKCD_ECDHX_R2WORK, PKCD_ECDHX_PWORK, PKCD_ECDHX_X2RWORK, PKCD_ECDHX_WORKUNIT);

    /* initialize x3*R mod p (=u*R) */
    r_pkcd_cmn_cpy_word(PKCD_ECDHX_X3RWORK, PKCD_ECDHX_X1RWORK, PKCD_ECDHX_WORKUNIT);

    /* initialize z3*R mod p (=R) */
    r_pkcd_cmn_cpy_word(PKCD_ECDHX_Z3RWORK, PKCD_ECDHX_X2RWORK, PKCD_ECDHX_WORKUNIT);

    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ecdhx_precalc_params()
**********************************************************************************************************************/


/**********************************************************************************************************************
* Function Name : r_pkcd_ecdhx_cswap
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDECDHXAPIsInternal
 * \brief       Swap two valuables in PKCCRAM
 * \param       [in,out] p_wk1     A work area to store data to be swapped.
 * \param       [in,out] p_wk2     A work area to store data to be swapped.
 * \param       [in] p_wk1_temp    A work area to store temporary data
 * \param       [in] p_wk2_temp    A work area to store temporary data
 * \return
 *  - #PKCD_RET_OK:                       Successfully done.
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_ECDHX_SWAP, #PKCD_ECDHX_PBYTES, #PKCD_ECDHX_PWORK, #PKCD_ECDHX_R2WORK,
 *    #PKCD_ECDHX_WORKUNIT
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB,
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT (Used by sub functions)
 * \par Precondition
 *  - #PKCD_ECDHX_OFFSET_VAL, #PKCD_ECDHX_PBYTES, #PKCD_ECDHX_PWORK and #PKCD_ECDHX_R2WORK must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit), #PKCD_CMN_ICUM_PKCCB0DPW, #PKCD_CMN_ICUM_PKCCB0DPT
 *    and DPT value must have been set beforehand.
 * \par Detailed description
 *      - Swap two valiables in PKCCRAM.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# Set (0 - masked_swap) into the unsigned integer value masked_swap.
 *  -# Calculate p_wk1_temp = p_wk1 * R^-1  with #r_pkcd_cmn_ARInvModC and set ret to the return value.
 *  -# Calculate p_wk2_temp = p_wk2 * R^-1  with #r_pkcd_cmn_ARInvModC and add the return value to ret.
 *  -# Set (masked_swap & (p_wk1 ^ p_wk2)) into dummy
 *  -# Set (p_wk1_temp ^ dummy) into p_wk1_temp
 *  -# Set (p_wk2_temp ^ dummy) into p_wk2_temp
 *  -# Set p_wk1_temp * R into p_wk1 with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 *  -# Set p_wk2_temp * R into p_wk2 with #r_pkcd_cmn_ARInvModC and add the return value to ret.
 *  -# Extract the return value and set ret to it with #r_pkcd_cmn_extract_result.
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ecdhx_cswap(const uint8_t* const p_wk1, const uint8_t* const p_wk2,
                                        uint8_t* const p_wk1_temp, uint8_t* const p_wk2_temp)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* Casts that do not exceed the size of the type are fine */
    uint32_t * const p_x1_top = (uint32_t*)(p_wk1_temp + PKCD_ECDHX_OFFSET_VAL);
    uint32_t * const p_x2_top = (uint32_t*)(p_wk2_temp + PKCD_ECDHX_OFFSET_VAL);  /* Same cast as above */
    uint32_t dummy;
    const uint32_t masked_swap = 0U - PKCD_ECDHX_SWAP;
    uint32_t i;

    ret = r_pkcd_cmn_ARInvModC(p_wk1, PKCD_ECDHX_PWORK, p_wk1_temp, PKCD_ECDHX_WORKUNIT);
    ret |= r_pkcd_cmn_ARInvModC(p_wk2, PKCD_ECDHX_PWORK, p_wk2_temp, PKCD_ECDHX_WORKUNIT);

    for (i = 0U; i < (PKCD_ECDHX_PBYTES / PKCD_CMN_WORD_BYTE_SIZE); i++ )
    {
        dummy = masked_swap & (p_x1_top[i] ^ p_x2_top[i]);
        p_x1_top[i] = p_x1_top[i] ^ dummy;
        p_x2_top[i] = p_x2_top[i] ^ dummy;
    }

    ret |= r_pkcd_cmn_ABRInvModC(p_wk1_temp, PKCD_ECDHX_R2WORK, PKCD_ECDHX_PWORK, p_wk1, PKCD_ECDHX_WORKUNIT);

    ret |= r_pkcd_cmn_ABRInvModC(p_wk2_temp, PKCD_ECDHX_R2WORK,
                                    PKCD_ECDHX_PWORK, p_wk2, PKCD_ECDHX_WORKUNIT);

    /* Extract the return value */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ecdhx_cswap()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ecdhx_point_smult
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDECDHXAPIsInternal
 * \brief       Performs scalar multiplication of the point on curve25519.
 * \param       [in]  p_point_out           The address of an multiplying point P1(X1, Y1, Z1).
 * \param       [in]  p_scalar              The address of a scalar value.
 * \param       [in]  p_works               The address of a work area structure which contains 3 work area addresses.
 * \param       [in]  processing_bits       The number of bits to be processed.
 * \param       [in,out]  p_executed_bits   The number of bits processed.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_ECDHX_PBYTES, #PKCD_ECDHX_PWORK, #PKCD_ECDHX_WORK1, #PKCD_ECDHX_WORK2,#PKCD_ECDHX_WORK3,
 *    #PKCD_ECDHX_X2RWORK, #PKCD_ECDHX_X3RWORK, #PKCD_ECDHX_Z2RWORK, #PKCD_ECDHX_Z3RWORK,
 *    #PKCD_ECDHX_WORKUNIT, #PKCD_ECDHX_OFFSET_VAL
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB,
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - #PKCD_ECDHX_PBYTES, #PKCD_ECDHX_PDT_P, #PKCD_ECDHX_PWORK, #PKCD_ECDHX_OFFSET_VAL must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit), #PKCD_CMN_ICUM_PKCCB0DPW, #PKCD_CMN_ICUM_PKCCB0DPT
 *    and DPT value must have been set beforehand.
 * \par Detailed description
 *      - Performs scalar multiplication of the point on curve25519.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below. Some values are represented in Montgomery format.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If p_executed_bits == 0 ,then
 *   -# Set 0 into #PKCD_ECDHX_SWAP.
 * -# Set p_executed_bits into the start bit index to be processed on this execution.
 * -# If (p_executed_bits + processing_bits) < (the total number of bits to be processed) then
 *   -# Set (p_executed_bits + processing_bits) into the last bit index to be processed on this execution.
 * -# Else
     -# Set (the total number of bits to be processed) into the last bit index to be processed on this execution.
 * -# For the bits between the start bit index and the last bit index of the given scalar, do the following
 *   -# Acquire the bit k_t in p_scalar which corresponds to the loop index and set it into #PKCD_ECDHX_SWAP.
 *   -# Swap x2 and x3 with #r_pkcd_ecdhx_cswap when #PKCD_ECDHX_SWAP == 1, and add the return value to ret.
 *   -# Swap z2 and z3 with #r_pkcd_ecdhx_cswap when #PKCD_ECDHX_SWAP == 1, and add the return value to ret.
 *   -# Set the bit k_t into #PKCD_ECDHX_SWAP.
 *   -# Perform the scalar manipulation.
 * -# Set the last bit index into p_executed_bits
 * -# If #PKCD_RET_OK == ret then
 *   -# If p_executed_bits is not equal to the nuber of total number of bits to be processed then
 *     -# Add #PKCD_RET_PROCESSING to ret
 *   -# Else
 *     -# Swap x2*R and x3*R with #r_pkcd_ecdhx_cswap when #PKCD_ECDHX_SWAP == 1, and add the return value to ret.
 *     -# Swap z2*R and z3*R with #r_pkcd_ecdhx_cswap when #PKCD_ECDHX_SWAP == 1, and add the return value to ret.
 *     -# Clear the work area to be used in the following operation with #r_pkcd_cmn_ram_clr.
 *     -# Calculate p-2 with #r_pkcd_cmn_AMinusBModC and add the return value to ret.
 *     -# Calculate z2 = (z2*R)*(R^(-1)) with #r_pkcd_cmn_ARInvModC and add the return value to ret
 *     -# Calculate z2^(p-2) mod p with #r_pkcd_cmn_AExpBModCCon and add the return value to ret.
 *     -# Calculate x2*(z2^(p-2)) mod p with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 *     -# Calculate with #r_pkcd_cmn_ARInvModC and add the return value to ret.
 * -# Extract the return value with r_pkcd_cmn_extract_result.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ecdhx_point_smult(st_ecdhx_point_t* const p_point_out,
                                            uint8_t* const p_scalar, st_ecdhx_wks_t* const p_works,
                                            const uint32_t processing_bits, uint32_t* const p_executed_bits)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t start_bit_pos;
    uint32_t end_bit_pos;
    uint32_t i;
    const uint32_t all_bits = PKCD_ECDHX_PBYTES * PKCD_CMN_BITSOFBYTE;
    uint32_t t;
    uint32_t k_t;
    const uint8_t value_two[PKCD_CMN_PKCCBOUNDARYSIZE] = {0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x02U};

    if (0U == (*p_executed_bits))
    {
        PKCD_ECDHX_SWAP = 0U;
    }
    else
    {
        /* Do nothing */
    }

    start_bit_pos = *p_executed_bits;
    end_bit_pos   = (((*p_executed_bits) + processing_bits) < all_bits) ?
                        ((*p_executed_bits) + processing_bits) : all_bits;

    for (i = start_bit_pos; i < end_bit_pos; i++ )
    {
        t = ((PKCD_ECDHX_PBYTES * PKCD_CMN_BITSOFBYTE) - i) - 1U;

        /* No problem for casting to support usual arithmetic conversion */
        k_t = ((uint32_t)p_scalar[(PKCD_ECDHX_PBYTES - 1U) - (t / PKCD_CMN_BITSOFBYTE)] >>
                (t % (uint32_t)PKCD_CMN_BITSOFBYTE)) & (uint32_t)1U;   /* Same cast as above */
        PKCD_ECDHX_SWAP ^= k_t;
        ret = r_pkcd_ecdhx_cswap(PKCD_ECDHX_X2RWORK, PKCD_ECDHX_X3RWORK, PKCD_ECDHX_WORK1, PKCD_ECDHX_WORK2);
        ret |= r_pkcd_ecdhx_cswap(PKCD_ECDHX_Z2RWORK, PKCD_ECDHX_Z3RWORK, PKCD_ECDHX_WORK1, PKCD_ECDHX_WORK2);
        PKCD_ECDHX_SWAP = k_t;

        ret |= r_pkcd_ecdhx_point_smult_loop(p_works);
    }

    /* Update remaining bits */
    *p_executed_bits = end_bit_pos;

    if (PKCD_RET_OK == ret)
    {
        if ((*p_executed_bits) != all_bits)
        {
            ret |= PKCD_RET_PROCESSING;
        }
        else
        {
            /* all bits execute */
            ret |= r_pkcd_ecdhx_cswap(PKCD_ECDHX_X2RWORK, PKCD_ECDHX_X3RWORK, PKCD_ECDHX_WORK1, PKCD_ECDHX_WORK2);
            ret |= r_pkcd_ecdhx_cswap(PKCD_ECDHX_Z2RWORK, PKCD_ECDHX_Z3RWORK, PKCD_ECDHX_WORK1, PKCD_ECDHX_WORK2);

            /* Calculate x2 * z2^(p-2) mod p  */
            r_pkcd_cmn_ram_clr(PKCD_ECDHX_WORK1, PKCD_ECDHX_WORKUNIT);
            r_pkcd_cmn_ram_clr(PKCD_ECDHX_WORK2, PKCD_ECDHX_WORKUNIT);
            r_pkcd_cmn_ram_clr(PKCD_ECDHX_WORK3, PKCD_ECDHX_WORKUNIT);
            r_pkcd_cmn_cpy_by_64bounds((PKCD_ECDHX_WORK2 + PKCD_ECDHX_WORKUNIT) - (sizeof(value_two)), value_two,
                                        sizeof(value_two));
            ret |= r_pkcd_cmn_AMinusBModC(PKCD_ECDHX_PWORK, PKCD_ECDHX_WORK2, PKCD_ECDHX_PWORK,
                                            PKCD_ECDHX_WORK1, PKCD_ECDHX_WORKUNIT);
            ret |= r_pkcd_cmn_ARInvModC(PKCD_ECDHX_Z2RWORK, PKCD_ECDHX_PWORK, PKCD_ECDHX_WORK3, PKCD_ECDHX_WORKUNIT);
            ret |= r_pkcd_cmn_AExpBModCCon(PKCD_ECDHX_WORK3, PKCD_ECDHX_WORK1, PKCD_ECDHX_PWORK,
                                            PKCD_ECDHX_WORK2, PKCD_ECDHX_WORKUNIT, PKC_SYNC_OPT_SYNC);
            ret |= r_pkcd_cmn_ABRInvModC(PKCD_ECDHX_X2RWORK, PKCD_ECDHX_WORK2, PKCD_ECDHX_PWORK,
                                            PKCD_ECDHX_WORK3, PKCD_ECDHX_WORKUNIT);
            r_pkcd_cmn_cpy_by_64bounds(p_point_out->u, (PKCD_ECDHX_WORK3 + PKCD_ECDHX_OFFSET_VAL), PKCD_ECDHX_PBYTES);
            p_point_out->len = PKCD_ECDHX_PBYTES;
        }
    }
    else
    {
        /* Do nothing */
    }

    /* Extract the return value */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ecdhx_point_smult()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ecdhx_point_smult_loop
**********************************************************************************************************************/
/*!*********************************************************************************************************************
 * \ingroup     PKCDECDHXAPIsInternal
 * \brief       Performs scalar multiplication of the point on curve25519.<br>
 *              Called at the for-loop in #r_pkcd_ecdhx_point_smult.
 * \param       [in]  p_works           The address of a work area structure which contains 3 work area addresses.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_ECDHX_PWORK, #PKCD_ECDHX_WORKUNIT, #PKCD_ECDHX_ARWORK, #PKCD_ECDHX_A2RWORK,
 *    #PKCD_ECDHX_BRWORK, #PKCD_ECDHX_B2RWORK, #PKCD_ECDHX_ERWORK, #PKCD_ECDHX_DARWORK,
 *    #PKCD_ECDHX_CBRWORK, #PKCD_ECDHX_A24RWORK, #PKCD_ECDHX_X1RWORK, #PKCD_ECDHX_X2RWORK,
 *    #PKCD_ECDHX_X3RWORK, #PKCD_ECDHX_Z2RWORK, S#PKCD_ECDHX_Z3RWORK
 *
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB,
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - #PKCD_ECDHX_PBYTES, #PKCD_ECDHX_PDT_P, #PKCD_ECDHX_R2WORK, #PKCD_ECDHX_PWORK must have been prepared.
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit), #PKCD_CMN_ICUM_PKCCB0DPW, #PKCD_CMN_ICUM_PKCCB0DPT
 *    and DPT value must have been set beforehand.
 * \par Detailed description
 *      - Operates scalar multiplication of the point on curve25519.
 *      - Called at the for-loop in #r_pkcd_ecdhx_point_smult.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below. Some values are represented in Montgomery format.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Calculate A*R = (x2 + z2)*R mod p with #r_pkcd_cmn_APlusBModC and set ret to the return value.
 * -# Calculate AA*R = (A^2)*R mod p with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 * -# Calculate B*R = (x2 - z2)*R mod p with #r_pkcd_cmn_AMinusBModC and add the return value to ret.
 * -# Calculate BB*R = (B^2)*R mod p with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 * -# Calculate E*R = (AA - BB)*R mod p with #r_pkcd_cmn_AMinusBModC and add the return value to ret.
 * -# Calculate C*R = (x3 + z3)*R mod p with #r_pkcd_cmn_APlusBModC and add the return value to ret.
 * -# Calculate D*R = (x3 - z3)*R mod p with #r_pkcd_cmn_AMinusBModC and add the return value to ret.
 * -# Calculate DA*R = D*A*R mod p with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 * -# Calculate CB*R = C*B*R mod p with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 * -# Calculate DA*R + CB*R mod p with #r_pkcd_cmn_APlusBModC and add the return value to ret.
 * -# Calculate x3*R = ((DA + CB)^2)*R mod p with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 * -# Calculate (DA - CB)*R mod p with #r_pkcd_cmn_AMinusBModC and add the return value to ret.
 * -# Calculate ((DA - CB)^2)*R mod p with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 * -# Calculate z3*R = (x1*(DA - CB)^2)*R with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 * -# Calculate x2*R = AA*BB*R mod p with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 * -# Calculate a24*E*R mod p with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 * -# Calculate (AA + a24*E)*R mod p with #r_pkcd_cmn_APlusBModC and add the return value to ret.
 * -# Calculate z2*R = (E*(AA + a24*E))*R mod p with #r_pkcd_cmn_ABRInvModC and add the return value to ret.
 * -# Extract the return value with #r_pkcd_cmn_extract_result.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_ecdhx_point_smult_loop(st_ecdhx_wks_t* const p_works)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* A = x2 + z2 (mod p)*/
    ret = r_pkcd_cmn_APlusBModC(PKCD_ECDHX_X2RWORK, PKCD_ECDHX_Z2RWORK, PKCD_ECDHX_PWORK,
                                    PKCD_ECDHX_ARWORK, PKCD_ECDHX_WORKUNIT);

    /* AA = A^2 (mod p)*/
    r_pkcd_cmn_cpy_word(p_works->p_wk1, PKCD_ECDHX_ARWORK, PKCD_ECDHX_WORKUNIT);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_ECDHX_ARWORK, p_works->p_wk1, PKCD_ECDHX_PWORK,
                                    PKCD_ECDHX_A2RWORK, PKCD_ECDHX_WORKUNIT);

    /* B = x2 - z2 (mod p)*/
    ret |= r_pkcd_cmn_AMinusBModC(PKCD_ECDHX_X2RWORK, PKCD_ECDHX_Z2RWORK, PKCD_ECDHX_PWORK,
                                    PKCD_ECDHX_BRWORK, PKCD_ECDHX_WORKUNIT);

    /* BB = B^2 (mod p)*/
    r_pkcd_cmn_cpy_word(p_works->p_wk1, PKCD_ECDHX_BRWORK, PKCD_ECDHX_WORKUNIT);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_ECDHX_BRWORK, p_works->p_wk1, PKCD_ECDHX_PWORK,
                                    PKCD_ECDHX_B2RWORK, PKCD_ECDHX_WORKUNIT);

    /* E = AA - BB (mod p)*/
    ret |= r_pkcd_cmn_AMinusBModC(PKCD_ECDHX_A2RWORK, PKCD_ECDHX_B2RWORK, PKCD_ECDHX_PWORK,
                                    PKCD_ECDHX_ERWORK, PKCD_ECDHX_WORKUNIT);

    /* C (work1) = x3 + z3 (mod p)*/
    ret |= r_pkcd_cmn_APlusBModC(PKCD_ECDHX_X3RWORK, PKCD_ECDHX_Z3RWORK, PKCD_ECDHX_PWORK,
                                    p_works->p_wk1, PKCD_ECDHX_WORKUNIT);

    /* D (work2) = x3 - z3 (mod p)*/
    ret |= r_pkcd_cmn_AMinusBModC(PKCD_ECDHX_X3RWORK, PKCD_ECDHX_Z3RWORK, PKCD_ECDHX_PWORK,
                                    p_works->p_wk2, PKCD_ECDHX_WORKUNIT);

    /* DA = D * A (mod p)*/
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk2, PKCD_ECDHX_ARWORK, PKCD_ECDHX_PWORK,
                                    PKCD_ECDHX_DARWORK, PKCD_ECDHX_WORKUNIT);

    /* CB = C * B (mod p)*/
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk1, PKCD_ECDHX_BRWORK, PKCD_ECDHX_PWORK,
                                    PKCD_ECDHX_CBRWORK, PKCD_ECDHX_WORKUNIT);

    /* x3 = (DA + CB)^2 (mod p)*/
    ret |= r_pkcd_cmn_APlusBModC(PKCD_ECDHX_DARWORK, PKCD_ECDHX_CBRWORK, PKCD_ECDHX_PWORK,
                                    p_works->p_wk1, PKCD_ECDHX_WORKUNIT);
    r_pkcd_cmn_cpy_word(p_works->p_wk2, p_works->p_wk1, PKCD_ECDHX_WORKUNIT);
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk1, p_works->p_wk2, PKCD_ECDHX_PWORK,
                                    PKCD_ECDHX_X3RWORK, PKCD_ECDHX_WORKUNIT);

    /* z3 = x1 * (DA - CB)^2, work1 = DA-CB, work2 = DA-CB, work3 = (DA-CB)^2 (mod p)*/
    ret |= r_pkcd_cmn_AMinusBModC(PKCD_ECDHX_DARWORK, PKCD_ECDHX_CBRWORK, PKCD_ECDHX_PWORK,
                                    p_works->p_wk1, PKCD_ECDHX_WORKUNIT);
    r_pkcd_cmn_cpy_word(p_works->p_wk2, p_works->p_wk1, PKCD_ECDHX_WORKUNIT);
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk1, p_works->p_wk2, PKCD_ECDHX_PWORK,
                                    p_works->p_wk3, PKCD_ECDHX_WORKUNIT);
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk3, PKCD_ECDHX_X1RWORK, PKCD_ECDHX_PWORK,
                                    PKCD_ECDHX_Z3RWORK, PKCD_ECDHX_WORKUNIT);

    /* x2 = AA * BB (mod p)*/
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_ECDHX_A2RWORK, PKCD_ECDHX_B2RWORK, PKCD_ECDHX_PWORK,
                                    PKCD_ECDHX_X2RWORK, PKCD_ECDHX_WORKUNIT);

    /* z2 = E * (AA + a24 * E) (mod p)*/
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_ECDHX_A24RWORK, PKCD_ECDHX_ERWORK, PKCD_ECDHX_PWORK,
                                    p_works->p_wk1, PKCD_ECDHX_WORKUNIT);
    ret |= r_pkcd_cmn_APlusBModC(PKCD_ECDHX_A2RWORK, p_works->p_wk1, PKCD_ECDHX_PWORK,
                                    p_works->p_wk2, PKCD_ECDHX_WORKUNIT);
    ret |= r_pkcd_cmn_ABRInvModC(p_works->p_wk2, PKCD_ECDHX_ERWORK, PKCD_ECDHX_PWORK,
                                    PKCD_ECDHX_Z2RWORK, PKCD_ECDHX_WORKUNIT);

    /* Calculate the return value */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_ecdhx_point_smult_loop()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ecdhx_set_curve_params
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECDHXAPIsInternal
 * \brief       Set curve parameters
 * \param       [out] p_curve           An address of curve information structure defined inside ECDHX.
 * \param       [in]  p_curve_info      An address of curve information structure defined in PKC common.
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
 *  - Parse the curve parameters and set them in the curve information structure defined inside ECDHX.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Set curve length into p_curve->len.
 * -# Set curve parameters into p_curve->field with #r_pkcd_cmn_cpy_word.
 * -# Set the value a24 into p_curve->a24 with #r_pkcd_cmn_cpy_word.
 * -# Set an u-coordinate of the base point into p_curve->base_u with #r_pkcd_cmn_cpy_word.
 *
 * \callgraph
 *********************************************************************************************************************/
static void r_pkcd_ecdhx_set_curve_params(st_ecdhx_curve_t* const p_curve, const st_pkc_ec_curve_t* const p_curve_info)
{
    p_curve->len = p_curve_info->curve_len;

    r_pkcd_cmn_cpy_word(p_curve->field,
                        &p_curve_info->
                            curve_params.p_data[p_curve_info->curve_len * PKCD_ECDHX_CURVE_PARAM_IDX_FIELD],
                        p_curve_info->curve_len);
    r_pkcd_cmn_cpy_word(p_curve->a24,
                        &p_curve_info->
                        curve_params.p_data[p_curve_info->curve_len * PKCD_ECDHX_CURVE_PARAM_IDX_A24],
                        p_curve_info->curve_len);
    r_pkcd_cmn_cpy_word(p_curve->base_u,
                        &p_curve_info->
                        curve_params.p_data[p_curve_info->curve_len * PKCD_ECDHX_CURVE_PARAM_IDX_BASE_U],
                        p_curve_info->curve_len);

}
/**********************************************************************************************************************
* End of function r_pkcd_ecdhx_set_curve_params()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_ecdhx_encode_data
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECDHXAPIsInternal
 * \brief       Reverse the endianness.
 * \param       [in,out] p_data           A top address of input data to be encoded.
 *                                        At the time of output, encoded data is set.
 * \param       [in]  len                 Byte size of data to be encoded
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
 *  - Reverse the byte order of an input byte array.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Acquire a buffer tmp_data for temporary data.
 *  -# Loop while i is less than len. <br>
 *     Increment i for each loop.
 *    -# Set p_data[(len - 1U) - i] into temp_data[i].
 *  -# If (0U != len)
 *    -# Get encoded data from tmp_data, and set the data and its length to p_data and len respectively
 *       with #r_pkcd_cmn_cpy_word.
 *
 * \callgraph
 *********************************************************************************************************************/
static void r_pkcd_ecdhx_encode_data(uint8_t* const p_data, const uint32_t len)
{
    uint32_t i;
    uint8_t tmp_data[PKCD_ECDHX_CURVE_LEN_MAX];

    for (i = 0U; i < len; i++ )
    {
        tmp_data[i] = p_data[(len - 1U) - i];
    }
    if (0U != len)
    {
        r_pkcd_cmn_cpy_word(p_data, tmp_data, len);
    }
    else
    {
        /* Do nothing */
    }
}
/**********************************************************************************************************************
* End of function r_pkcd_ecdhx_encode_data()
**********************************************************************************************************************/
