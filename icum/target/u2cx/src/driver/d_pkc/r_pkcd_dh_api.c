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
/* PRQA S 0751 ++ # D009 For commonality of functions, confirm that the size is correct and use the cast value. */
/* PRQA S 0776 ++ # D012 Develop according to C99 */
/* PRQA S 1503, 1505 ++ # D013 No problem if it is unused because it is an API called from outside the library */
/* PRQA S 3383 ++ # D018 Have confirmed in advance that the value doesn't wrap around. */
/* PRQA S 2981 ++ # D020 Local variable ret of APIs sets INTERNALERR to the initial value
                        as a countermeasure against attacks that pass through processing. */
/* PRQA S 3453,3469,3472 ++ # D026 If it is a function, overhead is large, so it corresponds with macro. */
/* PRQA S 3630 ++ # D028 Header file used only by the library */
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
 * \addtogroup    PKCDDH
 * \{
 * \addtogroup    PKCDDHTypes              DH Types
 * \addtogroup    PKCDDHAPIs               DH APIs
 * \addtogroup    PKCDDHDefinesInternal    DH Internal Definitions
 * \addtogroup    PKCDDHTypesInternal      DH Internal Types
 * \addtogroup    PKCDDHConstantsInternal  DH Internal Constants
 * \addtogroup    PKCDDHVariablesInternal  DH Internal Variables
 * \addtogroup    PKCDDHAPIsInternal       DH Internal APIs
 * \}******************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDDHDefinesInternal
 * \{******************************************************************/
#define PKCD_DH_WORKUNITPDT                 (0x00000008U) /**< A size of 8byte PDT area */
#define PKCD_DH_WORKUNIT                    (0x00000220U) /**< A size of 544byte work area */
#define PKCD_DH_WORKUNITPDW                 (PKCD_DH_WORKUNIT*3U) /**< A size of 1632byte PDW area */

/** Convert pointer shifted by 8bytes to PKCC data pointer register format */
#define PKCD_DH_FORMAT_DP_DPT(top)     (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_DH_WORKUNITPDT))
/** Convert pointer shifted by 816bytes to PKCC data pointer register format */
#define PKCD_DH_FORMAT_DP_DPW(top)     (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_DH_WORKUNITPDW))

/** Offset to value of unitsize work area */
#define PKCD_DH_OFFSET_VAL(val_size)        (PKCD_DH_WORKUNIT     - (val_size))

#define PKCD_DH_MOD_LEN_MAX                (512U)    /**< Maximum length of modulus */

#define PKCD_DH_EXTRA_BYTE_LEN             (8U)    /**< Extra byte length when generating random numbers */
/** Maximum length of extra random */
#define PKCD_DH_EXTRA_RANDOM_LEN_MAX       (PKCD_DH_MOD_LEN_MAX + PKCD_DH_EXTRA_BYTE_LEN)

#define PKCD_DH_PDT_1                          (sp_dh_pkccram->pdt_1)    /**< A work area for T mod p */
#define PKCD_DH_WORK1                          (sp_dh_pkccram->work1)    /**< A work area for 1 */
#define PKCD_DH_WORK2                          (sp_dh_pkccram->work2)    /**< A work area for 2 */
#define PKCD_DH_WORK3                          (sp_dh_pkccram->work3)    /**< A work area for 3 */
#define PKCD_DH_WORK4                          (sp_dh_pkccram->work4)    /**< A work area for 4 */
#define PKCD_DH_WWORK                          (sp_dh_pkccram->wwork)    /**< A work area for DPW */

#define PKCD_DH_KEY_LEN_1024                   (128U)           /**< DH1024 key byte length */
#define PKCD_DH_KEY_LEN_2048                   (256U)           /**< DH2048 key byte length */
#define PKCD_DH_KEY_LEN_3072                   (384U)           /**< DH3072 key byte length */
#define PKCD_DH_KEY_LEN_4096                   (512U)           /**< DH4096 key byte length */
#define PKCD_DH_Q_LEN_1024_MIN                 (20U)            /**< DH parameters q minimum byte length when DH1024 */
#define PKCD_DH_Q_LEN_2048_MIN                 (28U)            /**< DH parameters q minimum byte length when DH2048 */
#define PKCD_DH_Q_LEN_3072_MIN                 (32U)            /**< DH parameters q minimum byte length when DH3072 */
#define PKCD_DH_Q_LEN_4096_MIN                 (40U)            /**< DH parameters q minimum byte length when DH4096 */
#define PKCD_DH_G_LEN_MIN                      (4U)             /**< DH parameters g minimum byte length */

/** The #r_pkcd_dh_chk_dh_param does not check the DH parameters Q and G. */
#define PKCD_DH_CHK_DH_PARAM_NO_CHECK_QG       (0x00000000U)
/** The #r_pkcd_dh_chk_dh_param check the DH parameters Q */
#define PKCD_DH_CHK_DH_PARAM_CHECK_Q           (0x00000001U)
/** The #r_pkcd_dh_chk_dh_param check the DH parameters G */
#define PKCD_DH_CHK_DH_PARAM_CHECK_G           (0x00000002U)
/** The #r_pkcd_dh_chk_dh_param check the DH parameters Q and G */
#define PKCD_DH_CHK_DH_PARAM_CHECK_QG          (PKCD_DH_CHK_DH_PARAM_CHECK_Q | PKCD_DH_CHK_DH_PARAM_CHECK_G)

/*! \}*/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDDHTypesInternal
 * \{******************************************************************/
/** Work area structure of PKCC RAM */
typedef struct
{
    uint8_t      pdt_1[PKCD_DH_WORKUNITPDT];    /**< A work area for T mod p */
    uint8_t      work1[PKCD_DH_WORKUNIT];       /**< A work area for 1 */
    uint8_t      work2[PKCD_DH_WORKUNIT];       /**< A work area for 2 */
    uint8_t      work3[PKCD_DH_WORKUNIT];       /**< A work area for 3 */
    uint8_t      work4[PKCD_DH_WORKUNIT];       /**< A work area for 4 */
    uint8_t      wwork[PKCD_DH_WORKUNITPDW];    /**< A work area for DPW */
} st_dh_pkccram_t;

/** A parameter structure of Modular exponentiation */
typedef struct
{
    uint8_t         base[PKCD_DH_MOD_LEN_MAX];  /**< A base */
    uint32_t        base_len;                   /**< A byte length of base */
    uint8_t         exp[PKCD_DH_MOD_LEN_MAX];   /**< An exponent */
    uint32_t        exp_len;                    /**< A byte length of exponent */
    e_pkc_bool_t    is_constant;                /**< Constant time mode in exponentiation (use if 2048 or less) */
    uint8_t *       p_buf;                      /**< A buffer pointer for operation (use if larger than 2048) */
    uint32_t        exp_bits;                   /**< Number of exponent bits */
    uint32_t        remaining_bits;             /**< Number of bits for which the binary method has NOT been executed */
} st_dh_prm_modexp_t;

/** A parameter structure of key generation */
typedef struct
{
    e_pkc_bool_t        modexp_called;          /**< A flag that indicates that calling DhModExp API.
                                                    Used to determine Multi-time calling in R_PKCD_DhKeyGenOperate */
    uint8_t             random[PKCD_DH_EXTRA_RANDOM_LEN_MAX];   /**< An extra random */
    uint32_t            random_len;                             /**< A byte length of extra random */
    uint8_t             priv_key[PKCD_DH_MOD_LEN_MAX];          /**< A private key */
    uint8_t             pub_key[PKCD_DH_MOD_LEN_MAX];           /**< A public key */
    st_dh_prm_modexp_t  prm_modexp;                             /**< DH ModExp parameter structure */
} st_dh_prm_keygen_t;

/*! \}*/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup    PKCDDHVariablesInternal
 * \{******************************************************************/
/** Set PKCC RAM address to pkccram of structure st_dh_pkccram_t */
static st_dh_pkccram_t * const sp_dh_pkccram = ((st_dh_pkccram_t*)PKCD_CMN_PKCCRAM);

/*! \}*/

static pkcd_ret_t r_pkcd_dh_modexp_init (st_dh_prm_modexp_t * const p_prm, const uint8_t * const p_base,
                                            const uint32_t base_len, const uint8_t * const p_exp,
                                            const uint32_t exp_len, const e_pkc_bool_t is_constant);
static pkcd_ret_t r_pkcd_dh_modexp_operate (st_dh_prm_modexp_t * const p_prm, const uint8_t * const p_mod,
                                            const uint32_t mod_len, const e_pkc_sync_opt_t sync_option);
static pkcd_ret_t r_pkcd_dh_modexp_final (const uint32_t mod_len, uint8_t * const p_out, uint32_t * const p_out_len);
static pkcd_ret_t r_pkcd_dh_keygen_operate (const st_pkc_dh_ctx_t * const p_ctx,
                                            st_dh_prm_keygen_t * const p_prm,
                                            const st_pkc_operate_opt_t * const p_operate_opt);
static pkcd_ret_t r_pkcd_dh_get_priv_key (const uint8_t * const p_extra_random,
                                            const uint32_t extra_random_len,
                                            const uint8_t * const p_q,
                                            const uint32_t q_len,
                                            uint8_t * const p_buf,
                                            uint8_t * const p_priv_key);
static pkcd_ret_t r_pkcd_dh_chk_mod_exp_init_arg (const st_pkc_select_data_t * const p_base,
                                                    const st_pkc_select_data_t * const p_exp,
                                                    const st_pkc_dh_ctx_t * const p_ctx,
                                                    const uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_dh_chk_key_gen_init_arg (const st_pkc_dh_ctx_t * const p_ctx,
                                                    const uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_dh_chk_dh_param (const st_pkc_dh_param_t * const p_dh_param, const uint32_t chk_qg);
static pkcd_ret_t r_pkcd_dh_chk_dh_param_qg (const st_pkc_data_t * const p_q_or_g, const uint32_t len_min,
                                            const uint32_t p_len);
static pkcd_ret_t r_pkcd_dh_chk_base (const st_pkc_select_data_t * const p_base, const uint32_t p_len);
static pkcd_ret_t r_pkcd_dh_chk_exp (const st_pkc_select_data_t * const p_exp, const uint32_t p_len);
static pkcd_ret_t r_pkcd_dh_chk_base_exp_byte_data (const st_pkc_data_t * const p_base_or_exp, const uint32_t p_len);
static pkcd_ret_t r_pkcd_dh_chk_priv_key (const st_pkcd_dh_key_bin_t * const p_priv_key_bin, const uint32_t p_len);
static pkcd_ret_t r_pkcd_dh_chk_pub_key (const st_pkcd_dh_key_bin_t * const p_pub_key_bin, const uint32_t p_len);

/**********************************************************************************************************************
* Function Name : R_PKCD_DhModExpInit
**********************************************************************************************************************/
pkcd_ret_t   R_PKCD_DhModExpInit(const st_pkc_select_data_t* const p_base,
                                    const st_pkc_select_data_t* const p_exp,
                                    const st_pkc_dh_ctx_t* const p_ctx,
                                    uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_dh_prm_modexp_t * const p_prm = (st_dh_prm_modexp_t*)p_buf; /* No problem with casting to pointer size */
    const uint8_t * p_base_data;
    uint32_t base_len;
    const uint8_t * p_exp_data;
    uint32_t exp_len;

    /* check argument */
    ret = r_pkcd_dh_chk_mod_exp_init_arg(p_base, p_exp, p_ctx, p_buf);
    if (PKCD_RET_OK == ret)
    {
        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* Clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        /* Clear buffer */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_dh_prm_modexp_t));

        /* Select base data */
        if (PKC_DATA_TYPE_BYTE == p_base->data_type)
        {
            p_base_data = p_base->p_sel_data->p_data;
            base_len = p_base->p_sel_data->len;
        }
        else
        {
            /* No problem with casting to pointer size */
            p_base_data = ((st_pkcd_dh_key_bin_t*)p_base->p_sel_data->p_data)->p_public;
            base_len = ((st_pkcd_dh_key_bin_t*)p_base->p_sel_data->p_data)->public_key_length; /* Same cast as above */
        }

        /* Select base data */
        if (PKC_DATA_TYPE_BYTE == p_exp->data_type)
        {
            p_exp_data = p_exp->p_sel_data->p_data;
            exp_len = p_exp->p_sel_data->len;
        }
        else
        {
            /* No problem with casting to pointer size */
            p_exp_data = ((st_pkcd_dh_key_bin_t*)p_exp->p_sel_data->p_data)->p_private;
            exp_len = ((st_pkcd_dh_key_bin_t*)p_exp->p_sel_data->p_data)->private_key_length; /* Same cast as above */
        }

        ret = r_pkcd_dh_modexp_init(p_prm, p_base_data, base_len, p_exp_data, exp_len,
                                        p_ctx->is_constant_time_mode);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_DhModExpInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_DhModExpOperate
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_DhModExpOperate(const st_pkc_dh_ctx_t* const p_ctx,
                                    uint8_t* const p_buf,
                                    const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_dh_prm_modexp_t * const p_prm = (st_dh_prm_modexp_t*)p_buf; /* No problem with casting to pointer size */

    /* check argument */
    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_operate_opt))
    {
        ret = r_pkcd_dh_chk_dh_param(p_ctx->p_dh_param, PKCD_DH_CHK_DH_PARAM_NO_CHECK_QG);
        if (PKCD_RET_OK == ret)
        {
            if ((PKC_SYNC_OPT_SYNC == p_operate_opt->sync_opt) || (PKC_SYNC_OPT_ASYNC == p_operate_opt->sync_opt))
            {
                /* Clear async operation flag */
                r_pkcd_cmn_clr_async_ope();

                ret = r_pkcd_dh_modexp_operate(p_prm, p_ctx->p_dh_param->p_p->p_data, p_ctx->p_dh_param->p_p->len,
                                                p_operate_opt->sync_opt);

            }
            else
            {
                ret = PKCD_RET_PARAMERR;
            }
        }
        else
        {
            /* DH parameter error. Do nothing */
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_DhModExpOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_DhModExpFinal
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_DhModExpFinal(const st_pkc_dh_ctx_t* const p_ctx,
                                    uint8_t* const p_buf,
                                    st_pkc_data_t* const p_out)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* check argument */
    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_out))
    {
        ret = r_pkcd_dh_chk_dh_param(p_ctx->p_dh_param, PKCD_DH_CHK_DH_PARAM_NO_CHECK_QG);
        if (PKCD_RET_OK == ret)
        {
            if (NULL != p_out->p_data)
            {
                if (p_out->len >= p_ctx->p_dh_param->p_p->len)
                {
                    /* Clear async operation flag */
                    r_pkcd_cmn_clr_async_ope();

                    ret = r_pkcd_dh_modexp_final(p_ctx->p_dh_param->p_p->len, p_out->p_data, &p_out->len);
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
            /* DH parameter error. Do nothing */
        }
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_DhModExpFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_DhInit
**********************************************************************************************************************/
pkcd_ret_t   R_PKCD_DhInit(const void* const p_priv_key,
                            const void* const p_pub_key,
                            const st_pkc_dh_ctx_t* const p_ctx,
                            uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_pkc_select_data_t base_sel;
    st_pkc_select_data_t exp_sel;
    st_pkc_data_t base_data;
    st_pkc_data_t exp_data;

    base_sel.data_type = PKC_DATA_TYPE_KEY_BINARY;
    base_sel.p_sel_data = &base_data;
    base_sel.p_sel_data->p_data = (uint8_t*)p_pub_key;  /* No problem with casting to pointer size */

    exp_sel.data_type = PKC_DATA_TYPE_KEY_BINARY;
    exp_sel.p_sel_data = &exp_data;
    exp_sel.p_sel_data->p_data = (uint8_t*)p_priv_key;  /* No problem with casting to pointer size */

    ret = R_PKCD_DhModExpInit(&base_sel, &exp_sel, p_ctx, p_buf);

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_DhInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_DhOperate
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_DhOperate(const st_pkc_dh_ctx_t* const p_ctx,
                                uint8_t* const p_buf,
                                const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    ret = R_PKCD_DhModExpOperate(p_ctx, p_buf, p_operate_opt);

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_DhOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_DhFinal
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_DhFinal(const st_pkc_dh_ctx_t* const p_ctx,
                            uint8_t* const p_buf,
                            st_pkc_data_t* const p_shared_secret)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    int32_t cmp_ret;
    int32_t cmp_ret2;
    const uint8_t bottom_word_1[PKCD_CMN_WORD_BYTE_SIZE] = {0x00U, 0x00U, 0x00U, 0x01U};
    uint8_t bottom_word_p_minus_1[PKCD_CMN_WORD_BYTE_SIZE];
    const uint8_t * p_p;
    uint32_t p_len;

    /* Output SharedSecret */
    ret = R_PKCD_DhModExpFinal(p_ctx, p_buf, p_shared_secret);

    if (PKCD_RET_OK == ret)
    {
        /* Check if SharedSecret is a valid value  */

        /* Invalid case 1 : SharedSecret == 0 */
        cmp_ret = r_pkcd_cmn_cmp_zero_word(p_shared_secret->p_data, p_shared_secret->len);
        if (0 != cmp_ret)
        {
            /* Invalid case 2 : SharedSecret == 1 */
            cmp_ret = r_pkcd_cmn_cmp_zero_word(p_shared_secret->p_data,
                                                (p_shared_secret->len - PKCD_CMN_WORD_BYTE_SIZE));
            cmp_ret2 = r_pkcd_cmn_cmp_word(&p_shared_secret->p_data[(p_shared_secret->len - PKCD_CMN_WORD_BYTE_SIZE)],
                                            bottom_word_1,
                                            PKCD_CMN_WORD_BYTE_SIZE);
            if (!((0 == cmp_ret) && (0 == cmp_ret2)))
            {
                /* Invalid case 3 : SharedSecret == p - 1 */
                p_p = p_ctx->p_dh_param->p_p->p_data;
                p_len = p_ctx->p_dh_param->p_p->len;
                r_pkcd_cmn_cpy_word(bottom_word_p_minus_1, &p_p[p_len - PKCD_CMN_WORD_BYTE_SIZE],
                                    PKCD_CMN_WORD_BYTE_SIZE);

                /* Since modulus p is an odd prime number, no underflow occurs. */
                bottom_word_p_minus_1[PKCD_CMN_WORD_BYTE_SIZE - 1U] =
                    /* No problem for casting to support usual arithmetic conversion */
                    (uint8_t)((uint32_t)bottom_word_p_minus_1[PKCD_CMN_WORD_BYTE_SIZE - 1U] - (uint32_t)1U);
                cmp_ret = r_pkcd_cmn_cmp_word(p_shared_secret->p_data, p_ctx->p_dh_param->p_p->p_data,
                                                (p_shared_secret->len - PKCD_CMN_WORD_BYTE_SIZE));
                cmp_ret2 = r_pkcd_cmn_cmp_word(&p_shared_secret->p_data[(p_shared_secret->len -
                                                                            PKCD_CMN_WORD_BYTE_SIZE)],
                                                bottom_word_p_minus_1,
                                                PKCD_CMN_WORD_BYTE_SIZE);
                if (!((0 == cmp_ret) && (0 == cmp_ret2)))
                {
                    /* A valid Shared Secret was generated. Do nothing */
                }
                else
                {
                    /* An invalid SharedSecret was generated */
                    ret = PKCD_RET_DH_INVALID_SHAREDSECRET;
                }
            }
            else
            {
                /* An invalid SharedSecret was generated */
                ret = PKCD_RET_DH_INVALID_SHAREDSECRET;
            }
        }
        else
        {
            /* An invalid SharedSecret was generated */
            ret = PKCD_RET_DH_INVALID_SHAREDSECRET;
        }

    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_DhFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_DhKeyGenInit
**********************************************************************************************************************/
pkcd_ret_t   R_PKCD_DhKeyGenInit(const st_pkc_dh_ctx_t* const p_ctx,
                                    uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t drbg_ret;
    st_dh_prm_keygen_t * const p_prm = (st_dh_prm_keygen_t*)p_buf;  /* No problem with casting to pointer size */

    ret = r_pkcd_dh_chk_key_gen_init_arg(p_ctx, p_buf);
    if (PKCD_RET_OK == ret)
    {
        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* Clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        /* Clear buffer */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_dh_prm_keygen_t));

        /* Set modexp called flag to PKC_BOOL_FALSE */
        p_prm->modexp_called = PKC_BOOL_FALSE;

        /* Generate a random number 64 bits longer than order q to generate the private key */
        /* The private key is generated according to process 6 of key pair generation using the
           additional random bits of 5.6.11.3 NIST SP800-56A revision 3.
           Therefore, a random number of q byte size + 8 bytes is generated. */
        p_prm->random_len = p_ctx->p_dh_param->p_q->len + PKCD_DH_EXTRA_BYTE_LEN;

        drbg_ret = p_ctx->p_drbg_api(p_prm->random_len, p_prm->random);
        if (PKCD_CMN_EXTERNAL_API_OK == drbg_ret)
        {
            /* Random number generation was successful. Do nothing. */
        }
        else
        {
            /* DRBG API error */
            ret = PKCD_RET_DRBGAPIERR;
        }
    }
    else
    {
        /* Argument check error. Do nothing. */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_DhKeyGenInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_DhKeyGenOperate
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_DhKeyGenOperate(const st_pkc_dh_ctx_t* const p_ctx,
                                    uint8_t* const p_buf,
                                    const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_dh_prm_keygen_t * const p_prm = (st_dh_prm_keygen_t*)p_buf;  /* No problem with casting to pointer size */

    /* check argument */
    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_operate_opt))
    {
        /* Check the DH domain parameters */
        ret = r_pkcd_dh_chk_dh_param(p_ctx->p_dh_param, PKCD_DH_CHK_DH_PARAM_CHECK_QG);
        if (PKCD_RET_OK == ret)
        {
            if (NULL != p_ctx->p_drbg_api)
            {
                if ((PKC_SYNC_OPT_SYNC == p_operate_opt->sync_opt) ||
                    (PKC_SYNC_OPT_ASYNC == p_operate_opt->sync_opt))
                {
                    ret = r_pkcd_dh_keygen_operate(p_ctx, p_prm, p_operate_opt);
                }
                else
                {
                    ret = PKCD_RET_PARAMERR;
                }
            }
            else
            {
                /* An invalid value was set in DRBG function pointer */
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
/**********************************************************************************************************************
* End of function R_PKCD_DhKeyGenOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_DhKeyGenFinal
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_DhKeyGenFinal(const st_pkc_dh_ctx_t* const p_ctx,
                                uint8_t* const              p_buf,
                                void* const                 p_priv_key,
                                void* const                 p_pub_key)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_dh_prm_keygen_t * const p_prm = (st_dh_prm_keygen_t*)p_buf;  /* No problem with casting to pointer size */
    st_pkcd_dh_key_bin_t * const p_priv_key_bin = (st_pkcd_dh_key_bin_t*)p_priv_key; /* Same cast as above */
    st_pkcd_dh_key_bin_t * const p_pub_key_bin = (st_pkcd_dh_key_bin_t*)p_pub_key;   /* Same cast as above */
    st_pkc_data_t out;

    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_priv_key_bin) && (NULL != p_pub_key_bin))
    {
        if ((NULL != p_priv_key_bin->p_private) && (NULL != p_priv_key_bin->p_public)
            && (NULL != p_pub_key_bin->p_public))
        {
            /* Output public key */
            out.p_data = p_prm->pub_key;
            out.len = sizeof(p_prm->pub_key);

            /* No problem with casting to pointer size */
            ret = R_PKCD_DhModExpFinal(p_ctx, (uint8_t*)&p_prm->prm_modexp, &out);

            if (PKCD_RET_OK == ret)
            {
                /* Wrap private key */
                p_priv_key_bin->private_key_length = p_ctx->p_dh_param->p_q->len;
                r_pkcd_cmn_cpy_word(p_priv_key_bin->p_private, p_prm->priv_key, p_priv_key_bin->private_key_length);
                p_priv_key_bin->public_key_length = p_ctx->p_dh_param->p_p->len;
                r_pkcd_cmn_cpy_word(p_priv_key_bin->p_public, p_prm->pub_key, p_priv_key_bin->public_key_length);

                /* Wrap Public key */
                p_pub_key_bin->public_key_length = p_ctx->p_dh_param->p_p->len;
                r_pkcd_cmn_cpy_word(p_pub_key_bin->p_public, p_prm->pub_key, p_pub_key_bin->public_key_length);

                /* Clear the private key value stored in p_prm */
                r_pkcd_cmn_ram_clr(p_prm->priv_key, p_priv_key_bin->private_key_length);
            }
            else
            {
                /* Do nothing */
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
/**********************************************************************************************************************
* End of function R_PKCD_DhKeyGenFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_modexp_init
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIsInternal
 * \brief       Prepare parameters for modular exponentiation.
 * \param       [in,out]  p_prm         Parameters to be calculated
 * \param       [in]  p_base            An inputting base data.
 * \param       [in]  base_len          The length of the base in bytes.
 * \param       [in]  p_exp             An inputting exp data.
 * \param       [in]  exp_len           The length of the exp in bytes.
 * \param       [in]  is_constant       Indicate the use of constant time mode in exponentiation.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Prepare the following for the modular exponentiation operation.
 *        - Copy parameters to context.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Copy parameters to p_prm
 * -# Return #PKCD_RET_OK
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_dh_modexp_init(st_dh_prm_modexp_t* const p_prm, const uint8_t* const p_base,
                                            const uint32_t base_len, const uint8_t* const p_exp,
                                            const uint32_t exp_len, const e_pkc_bool_t is_constant)
{
    /* copy parameters */
    r_pkcd_cmn_cpy_word(p_prm->base, p_base, base_len);
    p_prm->base_len = base_len;
    r_pkcd_cmn_cpy_word(p_prm->exp, p_exp, exp_len);
    p_prm->exp_len = exp_len;
    p_prm->is_constant = is_constant;
    p_prm->p_buf = NULL;    /* Not used */

    return PKCD_RET_OK;
}
/***********************************************************************************************************************
* End of function r_pkcd_dh_modexp_init()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_modexp_operate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIsInternal
 * \brief       Perform a modular exponentioation.
 * \param       [in,out]  p_prm         Parameters to be calculated
 * \param       [in]  p_mod             modulus p in DH parameters.
 * \param       [in]  mod_len           The length of the modulus p in bytes.
 * \param       [in]  sync_option       Indicate if the function returns while PKCC is running.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_DH_PDT_1, #PKCD_DH_WORK1, #PKCD_DH_WORK2, #PKCD_DH_WORK3, #PKCD_DH_WORK4, #PKCD_DH_WWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPW
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 *
 * \par Precondition
 *  - The DH Context value must have been set beforehand.
 * \par Detailed description
 *      - Perform modular exponentiation operation using PKCC Operation.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Set #PKCD_DH_WWORK to #PKCD_CMN_ICUM_PKCCB0DPW
 * -# Set #PKCD_DH_PDT_1 to #PKCD_CMN_ICUM_PKCCB0DPT
 * -# Set modulo length to DL bit and exponent length to BDL bit of #PKCD_CMN_ICUM_PKCCB0OMR with <br>
 *    the #r_pkcd_cmn_set_dl
 * -# Copy msg, exp and mod to works.
 * -# Use #r_pkcd_cmn_ram_clr to clear the private key value stored in p_prm.
 * -# If Constant flag == PKC_BOOL_TRUE then
 *   -# Claclulate A^B mod C at constant time with the #r_pkcd_cmn_AExpBModCCon
 * -# Else
 *   -# Claclulate A^B mod C at variable time with the #r_pkcd_cmn_AExpBModC
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_dh_modexp_operate(st_dh_prm_modexp_t* const p_prm, const uint8_t* const p_mod,
                                            const uint32_t mod_len, const e_pkc_sync_opt_t sync_option)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* prepare operation */
    PKCD_CMN_ICUM_PKCCB0DPW = PKCD_DH_FORMAT_DP_DPW(PKCD_DH_WWORK);
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_DH_FORMAT_DP_DPT(PKCD_DH_PDT_1);
    r_pkcd_cmn_set_dl(mod_len, p_prm->exp_len);

    /* Copy data */
    r_pkcd_cmn_cpy_by_64bounds(PKCD_DH_WORK1 + PKCD_DH_OFFSET_VAL(p_prm->base_len),
                                p_prm->base, p_prm->base_len);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_DH_WORK2 + PKCD_DH_OFFSET_VAL(p_prm->exp_len),
                                p_prm->exp, p_prm->exp_len);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_DH_WORK3 + PKCD_DH_OFFSET_VAL(mod_len),
                                p_mod, mod_len);

    /* Clear the private key value stored in p_prm */
    r_pkcd_cmn_ram_clr(p_prm->exp, p_prm->exp_len);

    /* Calc S = A^B mod C */
    if (PKC_BOOL_TRUE == p_prm->is_constant)
    {
        ret = r_pkcd_cmn_AExpBModCCon(PKCD_DH_WORK1, PKCD_DH_WORK2, PKCD_DH_WORK3,
                                        PKCD_DH_WORK4, PKCD_DH_WORKUNIT, sync_option);
    }
    else
    {
        ret = r_pkcd_cmn_AExpBModC(PKCD_DH_WORK1, PKCD_DH_WORK2, PKCD_DH_WORK3,
                                    PKCD_DH_WORK4, PKCD_DH_WORKUNIT, sync_option);
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_dh_modexp_operate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_modexp_final
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIsInternal
 * \brief       Finalize a modular exponentioation.
 * \param       [in]   mod_len         The length of the modulus p in bytes.
 * \param       [out]  p_out           An address where the modular exponentiation result is output. <br>
 *                                     The length of output is equal to that of the modulus.
 * \param       [out]  p_out_len       An address where the modular exponentiation result length is output.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_DH_WORK4
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - The modular exponentiation is calculated by #r_pkcd_dh_modexp_operate.
 * \par Detailed description
 *      - Output the result of modular exponentiation operation by #r_pkcd_dh_modexp_operate.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_OK.
 * -# Copy the result calculated by #r_pkcd_dh_modexp_operate to p_out.
 * -# Set mod_len to *p_out_len
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_dh_modexp_final(const uint32_t mod_len, uint8_t* const p_out, uint32_t* const p_out_len)
{
    const pkcd_ret_t ret = PKCD_RET_OK;

    /* copy result */
    r_pkcd_cmn_cpy_by_64bounds(p_out, PKCD_DH_WORK4 + PKCD_DH_OFFSET_VAL(mod_len), mod_len);

    *p_out_len = mod_len;

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_dh_modexp_final()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_keygen_operate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIsInternal
 * \brief       Perform a key generation.
 * \param       [in]      p_ctx            A pointer to a structure that summarizes the parameters required by DH.
 * \param       [in,out]  p_prm            Parameters to be calculated
 * \param       [in]      p_operate_opt    Indicate if the function returns while PKCC is running.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPW
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 *
 * \par Precondition
 *  - The DH Context value must have been set beforehand.
 * \par Detailed description
 *      - Generate private key x. <br>
 *        If the order q in DH parameters is 2048 bits or less, calculate with based on the method of
 *        5.6.1.1.3 of NIST SP800-56A. <br>
 *        If q is larger than 2048bit, calculate with the following formula. <br>
 *          x = c mod q
 *          (c is a random number equal byte length of q , and q is order q in DH parameters)
 *      - Generate public key with y = g^x mod p.
 *        g is generator in DH parameters.
 *        p is modulus in DH parameters.
 *      - A modular exponentioation use R_PKCD_DhModExpInit/Operate/Final
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Initialize the variable p_buf with p_prm->pub_key.
 *    Since the buffer after p_prm->pub_key is not used in the r_pkcd_dh(2)_get_priv_key function,
 *    it is used as a temporary buffer.
 * -# Generate private key with #r_pkcd_dh_get_priv_key.
 * -# Use #r_pkcd_cmn_ram_clr to clear the value of the random number that is the private key stored in p_prm.
 * -# If #PKCD_RET_OK == ret then
 *   -# Set data type of base to #PKC_DATA_TYPE_BYTE
 *   -# Set data pointer of base to p_ctx->p_dh_param->p_g->p_data;
 *   -# Set data length of base to p_ctx->p_dh_param->p_g->len;
 *   -# Set data type of exponent to #PKC_DATA_TYPE_BYTE
 *   -# Set data pointer of exponent to p_prm->priv_key;
 *   -# Set data length of exponent to p_ctx->p_dh_param->p_q->len;
 *   -# Prepare modular exponentiation with #R_PKCD_DhModExpInit
 *   -# If #PKCD_RET_OK == ret then
 *     -# Perform modular exponentiation with #R_PKCD_DhModExpOperate
 *     -# Set p_prm->modexp_called to #PKC_BOOL_TRUE
 *   -# Else
 *     -# Do nothing.
 * -# Else
 *   -# Do nothing.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_dh_keygen_operate(const st_pkc_dh_ctx_t* const p_ctx,
                                            st_dh_prm_keygen_t* const p_prm,
                                            const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_pkc_select_data_t base_sel;
    st_pkc_select_data_t exp_sel;
    st_pkc_data_t base_data;
    st_pkc_data_t exp_data;
    uint8_t * p_buf;

    /* Since the buffer after p_prm->pub_key is not used in the r_pkcd_dh(2)_get_priv_key function,
        it is used as a temporary buffer. */
    p_buf = p_prm->pub_key;

    /* Generate private key */
    ret = r_pkcd_dh_get_priv_key(p_prm->random, p_prm->random_len, p_ctx->p_dh_param->p_q->p_data,
                                    p_ctx->p_dh_param->p_q->len, p_buf, p_prm->priv_key);

    /* Clear the value of the random number that is the private key stored in p_prm */
    r_pkcd_cmn_ram_clr(p_prm->random, p_prm->random_len);

    if (PKCD_RET_OK == ret)
    {
        /* Generate public key with g ^ priv_Key mod p */
        base_sel.data_type = PKC_DATA_TYPE_BYTE;
        base_sel.p_sel_data = &base_data;
        base_sel.p_sel_data->p_data = p_ctx->p_dh_param->p_g->p_data;
        base_sel.p_sel_data->len = p_ctx->p_dh_param->p_g->len;

        exp_sel.data_type = PKC_DATA_TYPE_BYTE;
        exp_sel.p_sel_data = &exp_data;
        exp_sel.p_sel_data->p_data = p_prm->priv_key;
        exp_sel.p_sel_data->len = p_ctx->p_dh_param->p_q->len;

        /* No problem with casting to pointer size */
        ret = R_PKCD_DhModExpInit(&base_sel, &exp_sel, p_ctx, (uint8_t*)&p_prm->prm_modexp);
        if (PKCD_RET_OK == ret)
        {
            ret = R_PKCD_DhModExpOperate(p_ctx, (uint8_t*)&p_prm->prm_modexp, p_operate_opt);  /* Same cast as above */

            /* Set modexp called flag to TRUE for Multi-time calling */
            p_prm->modexp_called = PKC_BOOL_TRUE;
        }
        else
        {
            /* R_PKCD_DhModExpInit returned error. Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_dh_keygen_operate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_get_priv_key
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIsInternal
 * \brief       Get private key
 * \param       [in]  p_extra_random   An adress of random value whose size is order q length + 8 bytes is set.
 * \param       [in]  extra_random_len The length of the extra random value in bytes.
 * \param       [in]  p_q              order q in DH parameters.
 * \param       [in]  q_len            The length of the order q value in bytes.
 * \param       [in]  p_buf            Buffer to store the value of q-1.
 *                                     Prepare in the same byte size as q.
 * \param       [out] p_priv_key       An address where the private key is output. <br>
 *                                     The length of output is equal to that of the order q.

 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPW
 *    #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 *
 * \par Precondition
 *  - None.
 * \par Detailed description
 *  - Generate a private key according to process 6 in 5.6.1.1.3 Key-Pair Generation Using Extra Random Bits of <br>
 *    NIST SP800-56A Revision 3. <br>
 *    In PKC-Driver, N used in the above process is the byte length of q, so q is always chosen for M.
 *
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Copy p_q to p_buf.
 * -# Subtract 1 from q set in p_buf.
 * -# Calculate random value mod (q-1) with #r_pkcd_cmn_reduce_even_modulus.
 *    The result is output to p_priv_key
 * -# If #PKCD_RET_OK == ret then
 *   -# Loop while i is q_len or less increment i for each loop
 *     -# If p_priv_key[q_len - i] equal #PKCD_CMN_UINT8_MAX
 *       -# Set p_priv_key[q_len - i] to 0 and check the next byte.
 *     -# Else
 *       -# Add 1 to p_priv_key[q_len - i] and end the loop.
 * -# Else
 *   -# Do nothing.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_dh_get_priv_key(const uint8_t* const p_extra_random,
                                            const uint32_t extra_random_len,
                                            const uint8_t* const p_q,
                                            const uint32_t q_len,
                                            uint8_t* const p_buf,
                                            uint8_t* const p_priv_key)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t i;

    /* Calculate q-1 */
    r_pkcd_cmn_cpy_word(p_buf, p_q, q_len);
    p_buf[q_len - 1U] = (uint8_t)((uint32_t)p_buf[q_len - 1U] - (uint32_t)1U); /* No problem for casting to support
                                                                                        usual arithmetic conversion */

    /* Calculate random mod (q-1) */
    ret = r_pkcd_cmn_reduce_even_modulus(p_extra_random, p_buf, extra_random_len, q_len, PKCD_DH_WORKUNIT,
                                            PKCD_DH_WORK1, PKCD_DH_WORK2, PKCD_DH_WORK3, PKCD_DH_WORK4);

    if (PKCD_RET_OK == ret)
    {
        /* Copy result of (random mod (q-1)) to buffer */
        r_pkcd_cmn_cpy_by_64bounds(p_priv_key, PKCD_DH_WORK4 + PKCD_DH_OFFSET_VAL(q_len), q_len);

        /* Calculate random mod (q-1) + 1 */
        for (i = 1U; i <= q_len; i++ )
        {
            /* Casts that do not exceed the size of the type are fine */
            if ((uint32_t)p_priv_key[q_len - i] == PKCD_CMN_UINT8_MAX)
            {
                /* Set 0 and check next byte */
                p_priv_key[q_len - i] = 0U;
            }
            else
            {
                /* Add 1 and end loop*/
                p_priv_key[q_len - i] = (uint8_t)((uint32_t)p_priv_key[q_len - i] + (uint32_t)1U);
                break;
            }
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_dh_get_priv_key()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_chk_mod_exp_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDDHAPIsInternal
 * \brief        #R_PKCD_DhModExpInit argument check function.
 * \param       [in] p_base         An inputting base value.
 * \param       [in] p_exp          An inputting exponent value.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by DH.
 * \param       [in] p_buf          Buffer used for internal processing of PKC-Driver
 * \return
 *  - #PKCD_RET_OK:                       Successfully done.
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed.
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
 *  - Check the arguments received by the #R_PKCD_DhModExpInit function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If ((NULL != p_ctx) && (NULL != p_buf))
 *   -# Check DH domain parameters with #r_pkcd_dh_chk_dh_param, and set ret to the return value.
 *   -# If #PKCD_RET_OK == ret
 *     -# If ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
 *       -# Check base value with #r_pkcd_dh_chk_base, and set ret to the return value.
 *       -# If #PKCD_RET_OK == ret
 *         -# Check exponent value with #r_pkcd_dh_chk_exp, and set ret to the return value.
 *       -# Else
 *         -# Do nothing.
 *     -# Else
 *       -# Set ret to #PKCD_RET_PARAMERR
 *   -# Else
 *     -# Do nothing.
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR
 * -# return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_dh_chk_mod_exp_init_arg(const st_pkc_select_data_t* const p_base,
                                                    const st_pkc_select_data_t* const p_exp,
                                                    const st_pkc_dh_ctx_t* const p_ctx,
                                                    const uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* check if the arguments are NULL */
    if ((NULL != p_ctx) && (NULL != p_buf))
    {
        /* Check the DH domain parameters */
        ret = r_pkcd_dh_chk_dh_param(p_ctx->p_dh_param, PKCD_DH_CHK_DH_PARAM_NO_CHECK_QG);
        if (PKCD_RET_OK == ret)
        {
            /* Check constant time mode */
            if ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
            {
                /* Check base */
                ret = r_pkcd_dh_chk_base(p_base, p_ctx->p_dh_param->p_p->len);
                if (PKCD_RET_OK == ret)
                {
                    ret = r_pkcd_dh_chk_exp(p_exp, p_ctx->p_dh_param->p_p->len);
                }
                else
                {
                    /* An invalid value was set in the argument p_base. Do nothing */
                }
            }
            else
            {
                /* An invalid  was set in the constant time mode */
                ret = PKCD_RET_PARAMERR;
            }
        }
        else
        {
            /* An invalid value were set in the DH domain parameters. Do nothing */
        }
    }
    else
    {
        ret =  PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_dh_chk_mod_exp_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_chk_key_gen_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDDHAPIsInternal
 * \brief        #R_PKCD_DhKeyGenInit argument check function.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by DH.
 * \param       [in] p_buf          Buffer used for internal processing of PKC-Driver
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed
 *  - #PKCD_RET_PARAMERR:               Invalid argument entered
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
 *  - Check the arguments received by the #R_PKCD_DhKeyGenInit function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If ((NULL != p_ctx) && (NULL != p_buf))
 *   -# Check DH domain parameters with #r_pkcd_dh_chk_dh_param, and set ret to the return value.
 *   -# If #PKCD_RET_OK == ret
 *     -# If ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
 *       -# If NULL != p_ctx->p_drbg_api
 *         -# Do nothing.
 *       -# Else
 *         -# Set ret to #PKCD_RET_PARAMERR
 *     -# Else
 *       -# Set ret to #PKCD_RET_PARAMERR
 *   -# Else
 *     -# Do nothing.
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR
 * -# return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_dh_chk_key_gen_init_arg(const st_pkc_dh_ctx_t* const p_ctx,
                                                    const uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* check if the arguments are NULL */
    if ((NULL != p_ctx) && (NULL != p_buf))
    {
        /* Check the DH domain parameters */
        ret = r_pkcd_dh_chk_dh_param(p_ctx->p_dh_param, PKCD_DH_CHK_DH_PARAM_CHECK_QG);
        if (PKCD_RET_OK == ret)
        {
            if ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
            {
                if (NULL != p_ctx->p_drbg_api)
                {
                    /* Do nothing */
                }
                else
                {
                    /* An invalid value was set in DRBG function pointer */
                    ret = PKCD_RET_PARAMERR;
                }
            }
            else
            {
                /* An invalid value was set in the constant time mode */
                ret = PKCD_RET_PARAMERR;
            }
        }
        else
        {
            /* An invalid value were set in the DH domain parameters. Do nothing */
        }
    }
    else
    {
        ret =  PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_dh_chk_key_gen_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_chk_dh_param
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDDHAPIsInternal
 * \brief        Check the DH domain parameters
 * \param       [in]   p_dh_param        DH domain parameters.
 * \param       [in]   chk_qg            Flags to set when checking DH parameters q and g.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed
 *  - #PKCD_RET_PARAMERR:               Invalid argument entered
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
 *  - Check if the data and length of the members of the DH parameter structure are set correctly.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If NULL != p_dh_param
 *   -# If NULL != p_dh_param->p_p
 *     -# If NULL != p_dh_param->p_p->p_data
 *       -# switch p_dh_param->p_p->len
 *         -# p_dh_param->p_p->len == #PKCD_DH_KEY_LEN_1024
 *           -# Set q_len_min to #PKCD_DH_Q_LEN_1024_MIN
 *           -# Set ret to #PKCD_RET_OK
 *         -# p_dh_param->p_p->len == #PKCD_DH_KEY_LEN_2048
 *           -# Set q_len_min to #PKCD_DH_Q_LEN_2048_MIN
 *           -# Set ret to #PKCD_RET_OK
 *         -# p_dh_param->p_p->len == #PKCD_DH_KEY_LEN_3072
 *           -# Set q_len_min to #PKCD_DH_Q_LEN_3072_MIN
 *           -# Set ret to #PKCD_RET_OK
 *         -# p_dh_param->p_p->len == #PKCD_DH_KEY_LEN_4096
 *           -# Set q_len_min to #PKCD_DH_Q_LEN_4096_MIN
 *           -# Set ret to #PKCD_RET_OK
 *         -# default
 *           -# Set ret to #PKCD_RET_LENGTHERR
 *     -# Else
 *       -# Set ret to #PKCD_RET_PARAMERR
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 *   -# If #PKCD_RET_OK == ret
 *     -# If((chk_qg & #PKCD_DH_CHK_DH_PARAM_CHECK_Q) == #PKCD_DH_CHK_DH_PARAM_CHECK_Q)
 *       -# Check order q in DH parameters with #r_pkcd_dh_chk_dh_param_qg, and set ret to the return value.
 *     -# Else
 *       -# Do nothing
 *     -# If #PKCD_RET_OK == ret
 *       -# If((chk_qg & #PKCD_DH_CHK_DH_PARAM_CHECK_G) == #PKCD_DH_CHK_DH_PARAM_CHECK_G)
 *         -# Check order g in DH parameters with #r_pkcd_dh_chk_dh_param_qg, and set ret to the return value.
 *       -# Else
 *         -# Do nothing
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_dh_chk_dh_param(const st_pkc_dh_param_t* const p_dh_param, const uint32_t chk_qg)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t q_len_min;

    /* Check the key length range */
    if (NULL != p_dh_param)
    {
        /* Check the modulus p (p is always checkd) */
        if (NULL != p_dh_param->p_p)
        {
            if (NULL != p_dh_param->p_p->p_data)
            {
                /* Check modulus p length */
                switch (p_dh_param->p_p->len)
                {
                    case PKCD_DH_KEY_LEN_1024:
                        q_len_min = PKCD_DH_Q_LEN_1024_MIN;
                        ret = PKCD_RET_OK;
                        break;
                    case PKCD_DH_KEY_LEN_2048:
                        q_len_min = PKCD_DH_Q_LEN_2048_MIN;
                        ret = PKCD_RET_OK;
                        break;
                    case PKCD_DH_KEY_LEN_3072:
                        q_len_min = PKCD_DH_Q_LEN_3072_MIN;
                        ret = PKCD_RET_OK;
                        break;
                    case PKCD_DH_KEY_LEN_4096:
                        q_len_min = PKCD_DH_Q_LEN_4096_MIN;
                        ret = PKCD_RET_OK;
                        break;
                    default:
                        ret = PKCD_RET_LENGTHERR;
                        break;
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

        /* Check the order q */
        if (PKCD_RET_OK == ret)
        {
            if ((chk_qg & PKCD_DH_CHK_DH_PARAM_CHECK_Q) == PKCD_DH_CHK_DH_PARAM_CHECK_Q)
            {
                ret = r_pkcd_dh_chk_dh_param_qg(p_dh_param->p_q, q_len_min, p_dh_param->p_p->len);
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

        /* Check the generator g */
        if (PKCD_RET_OK == ret)
        {
            if ((chk_qg & PKCD_DH_CHK_DH_PARAM_CHECK_G) == PKCD_DH_CHK_DH_PARAM_CHECK_G)
            {
                ret = r_pkcd_dh_chk_dh_param_qg(p_dh_param->p_g, PKCD_DH_G_LEN_MIN, p_dh_param->p_p->len);
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
* End of function r_pkcd_dh_chk_dh_param()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_chk_dh_param_qg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDDHAPIsInternal
 * \brief        Check the DH domain parameters q or g
 * \param       [in]   p_q_or_g        A q or g data in DH parameters.
 * \param       [in]   len_min         Minimaum length of q or g.
 * \param       [in]   p_len           A length of modulus p in DH parameters.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed
 *  - #PKCD_RET_PARAMERR:               Invalid argument entered
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
 *  - Check if the data and length of the DH parameter q or g are set correctly.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If NULL != p_q_or_g
 *   -# If NULL != p_q_or_g->p_data
 *     -# Check if the length is a multiple of 4 with #r_pkcd_cmn_is_multiple_4,
 *        and set is_multiple_4 to the return value.
 *     -If (len_min <= p_q_or_g->len) && (p_q_or_g->len <= p_len) && (#PKC_BOOL_TRUE == is_multiple_4)
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
static pkcd_ret_t r_pkcd_dh_chk_dh_param_qg(const st_pkc_data_t* const p_q_or_g, const uint32_t len_min,
                                            const uint32_t p_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    e_pkc_bool_t is_multiple_4;

    if (NULL != p_q_or_g)
    {
        if ((NULL != p_q_or_g->p_data))
        {
            /* Check length */
            is_multiple_4 = r_pkcd_cmn_is_multiple_4(p_q_or_g->len);
            if ((len_min <= p_q_or_g->len) && (p_q_or_g->len <= p_len) &&
                (PKC_BOOL_TRUE == is_multiple_4))
            {
                ret = PKCD_RET_OK;
            }
            else
            {
                /* Invalid range */
                ret = PKCD_RET_LENGTHERR;
            }

        }
        else
        {
            /* p_q_or_g->p_data is NULL */
            ret = PKCD_RET_PARAMERR;
        }
    }
    else
    {
        /* p_q_or_g is NULL */
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_dh_chk_dh_param_qg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_chk_base
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDDHAPIsInternal
 * \brief        Check the base data
 * \param       [in]   p_base         An inputting base data.
 * \param       [in]   p_len          A length of modulus p in DH parameters.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed
 *  - #PKCD_RET_PARAMERR:               Invalid argument entered
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
 *  - Check if the data and length of an inputting base data are set correctly.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If NULL != p_base
 *   -# If #PKC_DATA_TYPE_KEY_BINARY == p_base->data_type
 *     -# If NULL != p_base->p_sel_data
 *       -# Check if given public key binary is valid with #r_pkcd_dh_chk_pub_key, and set ret to the result.
 *     -# Else
 *       -# Set ret to #PKCD_RET_PARAMERR
 *   -# Else if #PKC_DATA_TYPE_BYTE == p_base->data_type
 *     -# Check the given #st_pkc_data_t type base data with #r_pkcd_dh_chk_base_exp_byte_data,
 *        and set ret to the result.
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_dh_chk_base(const st_pkc_select_data_t* const p_base, const uint32_t p_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* check p_base is NULL */
    if (NULL != p_base)
    {
        if (PKC_DATA_TYPE_KEY_BINARY == p_base->data_type)
        {
            if (NULL != p_base->p_sel_data)
            {
                /* No problem with casting to pointer size */
                ret = r_pkcd_dh_chk_pub_key((const st_pkcd_dh_key_bin_t*)p_base->p_sel_data->p_data, p_len);
            }
            else
            {
                ret = PKCD_RET_PARAMERR;
            }
        }
        else if (PKC_DATA_TYPE_BYTE == p_base->data_type)
        {
            ret = r_pkcd_dh_chk_base_exp_byte_data(p_base->p_sel_data, p_len);
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }
    }
    else
    {
        ret =  PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_dh_chk_base()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_chk_exp
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDDHAPIsInternal
 * \brief        Check the exponent data
 * \param       [in]   p_exp         An inputting exponent data.
 * \param       [in]   p_len         A length of modulus p in DH parameters.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed
 *  - #PKCD_RET_PARAMERR:               Invalid argument entered
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
 *  - Check if the data and length of an inputting exponent data are set correctly.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If NULL != p_exp
 *   -# If #PKC_DATA_TYPE_KEY_BINARY == p_exp->data_type
 *     -# If NULL != p_exp->p_sel_data
 *       -# Check if given private key binary is valid with #r_pkcd_dh_chk_priv_key, and set ret to the result.
 *     -# Else
 *       -# Set ret to #PKCD_RET_PARAMERR
 *   -# Else if #PKC_DATA_TYPE_BYTE == p_exp->data_type
 *     -# Check the given #st_pkc_data_t type exponent data with #r_pkcd_dh_chk_base_exp_byte_data,
 *        and set ret to the result.
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_dh_chk_exp(const st_pkc_select_data_t* const p_exp, const uint32_t p_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* check p_exp is NULL */
    if (NULL != p_exp)
    {
        if (PKC_DATA_TYPE_KEY_BINARY == p_exp->data_type)
        {
            if (NULL != p_exp->p_sel_data)
            {
                /* No problem with casting to pointer size */
                ret = r_pkcd_dh_chk_priv_key((const st_pkcd_dh_key_bin_t*)p_exp->p_sel_data->p_data, p_len);
            }
            else
            {
                ret = PKCD_RET_PARAMERR;
            }
        }
        else if (PKC_DATA_TYPE_BYTE == p_exp->data_type)
        {
            ret = r_pkcd_dh_chk_base_exp_byte_data(p_exp->p_sel_data, p_len);
        }
        else
        {
            ret = PKCD_RET_PARAMERR;
        }
    }
    else
    {
        ret =  PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_dh_chk_exp()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_chk_base_exp_byte_data
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDDHAPIsInternal
 * \brief        Check the given #st_pkc_data_t type base or exponent data
 * \param       [in]   p_base_or_exp    An inputting base or exponent data.
 * \param       [in]   p_len            A length of modulus p in DH parameters.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed
 *  - #PKCD_RET_PARAMERR:               Mandatory parameters are NULL.
 *  - #PKCD_RET_LENGTHERR:              Invalid message lengths are passed.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check if the data and length of the  #st_pkc_data_t type base or exponent are set correctly.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *  -# If NULL != p_base_or_exp
 *    -# If NULL != p_base_or_exp->p_data
 *      -# Check if the length is a multiple of 4 with #r_pkcd_cmn_is_multiple_4,
 *         and set is_multiple_4 to the return value.
 *      -# If (p_base_or_exp->len <= p_len) && (PKC_BOOL_TRUE == is_multiple)
 *        -# Set ret to #PKCD_RET_OK
 *      -# Else
 *        -# Set ret to #PKCD_RET_LENGTHERR
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_dh_chk_base_exp_byte_data(const st_pkc_data_t* const p_base_or_exp, const uint32_t p_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    e_pkc_bool_t is_multiple;

    /* check p_base_or_exp is NULL */
    if (NULL != p_base_or_exp)
    {
        if (NULL != p_base_or_exp->p_data)
        {
            /* check the base or exp length range */
            is_multiple = r_pkcd_cmn_is_multiple_4(p_base_or_exp->len);
            if ((p_base_or_exp->len <= p_len) && (PKC_BOOL_TRUE == is_multiple))
            {
                /* Do nothing */
                ret =  PKCD_RET_OK;
            }
            else
            {
                ret =  PKCD_RET_LENGTHERR;
            }
        }
        else
        {
            ret =  PKCD_RET_PARAMERR;
        }
    }
    else
    {
        ret =  PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_dh_chk_base_exp_byte_data()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_chk_priv_key
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDDHAPIsInternal
 * \brief        Check the private key parameters
 * \param       [in]   p_priv_key_bin DH private key of key binary format.
 * \param       [in]   p_len          A length of modulus p.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed
 *  - #PKCD_RET_PARAMERR:               An invalid argument was entered.
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
 *    -# If NULL != p_priv_key_bin->p_private
 *      -# Check if the length is a multiple of 4 with #r_pkcd_cmn_is_multiple_4,
 *         and set is_multiple_4 to the return value.
 *      -# If (p_priv_key_bin->private_key_length <= p_len) && (PKC_BOOL_TRUE == is_multiple)
 *        -# Set ret to #PKCD_RET_OK
 *      -# Else
 *        -# Set ret to #PKCD_RET_LENGTHERR
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_dh_chk_priv_key(const st_pkcd_dh_key_bin_t* const p_priv_key_bin, const uint32_t p_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    e_pkc_bool_t is_multiple;

    /* check p_base_or_exp is NULL */
    if (NULL != p_priv_key_bin)
    {
        if (NULL != p_priv_key_bin->p_private)
        {
            /* check the private key length range */
            is_multiple = r_pkcd_cmn_is_multiple_4(p_priv_key_bin->private_key_length);
            if ((p_priv_key_bin->private_key_length <= p_len) && (PKC_BOOL_TRUE == is_multiple))
            {
                /* Do nothing */
                ret =  PKCD_RET_OK;
            }
            else
            {
                ret =  PKCD_RET_KEY_LENGTHERR;
            }
        }
        else
        {
            ret =  PKCD_RET_KEY_PARAMERR;
        }
    }
    else
    {
        ret =  PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_dh_chk_priv_key()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_dh_chk_pub_key
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDDHAPIsInternal
 * \brief        Check the private key parameters
 * \param       [in]   p_pub_key_bin  DH public key of key binary format.
 * \param       [in]   p_len          A length of modulus p.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed
 *  - #PKCD_RET_PARAMERR:               An invalid argument was entered.
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
 *    -# If NULL != p_pub_key_bin->p_private
 *      -# Check if the length is a multiple of 4 with #r_pkcd_cmn_is_multiple_4,
 *         and set is_multiple_4 to the return value.
 *      -# If (p_pub_key_bin->public_key_length <= p_len) && (#PKC_BOOL_TRUE == is_multiple)
 *        -# Set ret to #PKCD_RET_OK
 *      -# Else
 *        -# Set ret to #PKCD_RET_LENGTHERR
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_dh_chk_pub_key(const st_pkcd_dh_key_bin_t* const p_pub_key_bin, const uint32_t p_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    e_pkc_bool_t is_multiple;

    /* check p_pub_key_bin is NULL */
    if (NULL != p_pub_key_bin)
    {
        if (NULL != p_pub_key_bin->p_public)
        {
            /* check the base or exp length range */
            is_multiple = r_pkcd_cmn_is_multiple_4(p_pub_key_bin->public_key_length);
            if ((p_pub_key_bin->public_key_length <= p_len) && (PKC_BOOL_TRUE == is_multiple))
            {
                /* Do nothing */
                ret =  PKCD_RET_OK;
            }
            else
            {
                ret =  PKCD_RET_KEY_LENGTHERR;
            }
        }
        else
        {
            ret =  PKCD_RET_KEY_PARAMERR;
        }
    }
    else
    {
        ret =  PKCD_RET_PARAMERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_dh_chk_pub_key()
**********************************************************************************************************************/
