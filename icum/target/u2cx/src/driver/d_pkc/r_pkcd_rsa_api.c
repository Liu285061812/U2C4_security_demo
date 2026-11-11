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
/* PRQA S 3383,3384 ++ # D018 Have confirmed in advance that the value doesn't wrap around. */
/* PRQA S 2981 ++ # D020 Local variable ret of APIs sets INTERNALERR to the initial value
                        as a countermeasure against attacks that pass through processing. */
/* PRQA S 3416 ++ # D025 Because it is volatile to prevent the timeout count from being optimized. */
/* PRQA S 3469 ++ # D026 If it is a function, overhead is large, so it corresponds with macro. */
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
#include "r_pkcd_rsa.h"

/*!********************************************************************
 * \addtogroup    PKCDRSA
 * \{
 * \addtogroup    PKCDRSATypes              RSA Types
 * \addtogroup    PKCDRSAAPIs               RSA APIs
 * \addtogroup    PKCDRSADefinesInternal    RSA Internal Definitions
 * \addtogroup    PKCDRSATypesInternal      RSA Internal Types
 * \addtogroup    PKCDRSAConstantsInternal  RSA Internal Constants
 * \addtogroup    PKCDRSAVariablesInternal  RSA Internal Variables
 * \addtogroup    PKCDRSAAPIsInternal       RSA Internal APIs
 * \}******************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDRSADefinesInternal
 * \{******************************************************************/
#define PKCD_RSA_PDT_1          (sp_rsa_pkccram->pdt_1)    /**< A work area for T (mod p) */
#define PKCD_RSA_PDT_2          (sp_rsa_pkccram->pdt_2)    /**< A work area for T mod q */
#define PKCD_RSA_WORK1          (&sp_rsa_pkccram->work1)   /**< A work area for 1 */
#define PKCD_RSA_WORK2          (&sp_rsa_pkccram->work2)   /**< A work area for 2 */
#define PKCD_RSA_WORK3          (&sp_rsa_pkccram->work3)   /**< A work area for 3 */
#define PKCD_RSA_WORK4          (&sp_rsa_pkccram->work4)   /**< A work area for 4 */
/** A 272byte spare work area, which area overlaps DPW area */
#define PKCD_RSA_SPAREWORK1     ((u_rsa_work_t*)sp_rsa_pkccram->wwork)
/** A 272byte spare work area, which area overlaps DPW area */
#define PKCD_RSA_SPAREWORK2     ((u_rsa_work_t*)(sp_rsa_pkccram->wwork + PKCD_RSA_WORKUNIT))
/** A 272byte spare work area, which area overlaps DPW area */
#define PKCD_RSA_SPAREWORK3     ((u_rsa_work_t*)(sp_rsa_pkccram->wwork +\
                                (PKCD_RSA_WORKUNIT*PKCD_CMN_MUL_2)))
#define PKCD_RSA_WWORK          (sp_rsa_pkccram->wwork)    /**< A work area for DPW */

#define PKCD_RSA_PKCC_CARRY_LEN                 (8U)       /**< Carry size calculated by PKCC */

#define PKCD_RSA_REDUCE_LOOP_TIMEOUT            (1000U)    /**< Timeout count of reduce loop */
#define PKCD_RSA_MILLER_RABIN_DIV2_LOOP_TIMEOUT (4096U)    /**< Timeout count of Miller-Rabin Test divide 2 loop */
#define PKCD_RSA_EX_GCD_LOOP_TIMEOUT            (4096U)    /**< Timeout count of Binary extended GCD algorithm loop */

#define PKCD_RSA_KEY_LEN_1024                   (128U)     /**< RSA1024 key byte length */
#define PKCD_RSA_KEY_LEN_2048                   (256U)     /**< RSA2048 key byte length */
#define PKCD_RSA_KEY_LEN_3072                   (384U)     /**< RSA3072 key byte length */
#define PKCD_RSA_KEY_LEN_4096                   (512U)     /**< RSA4096 key byte length */

#define PKCD_RSA_KEY_LEN_MAX                    (512U)     /**< Maximum byte size of RSA key */

#define PKCD_RSA_GEN_D_NUM_MAX                  (2U)       /**< Max number of trials to generate d. */
#define PKCD_RSA_PRIME_TEST_NUM_MIN             (0x1U)     /**< Min number of trials to generate a prime number. */
#define PKCD_RSA_PRIME_TEST_NUM_MAX             (0xF4240U) /**< Max number of trials to generate a prime number. */

/** Fixed Public exponent length when generating a key with PKC-Driver */
#define PKCD_RSA_PUB_EXP_FIXED_LEN              (4U)
#define PKCD_RSA_PUB_EXP_LEN_MIN                (4U)    /**< Minimum of public exponent length */

#define PKCD_RSA_ITERATION_NUM_MIN              (1U)    /**< Minimum of iteration number */
#define PKCD_RSA_ITERATION_NUM_MAX              (20U)   /**< Maximum of iteration number */

#define PKCD_RSA_MASK_0F                        (0x0FU)    /**< the value used for miller rabin test */

#define PKCD_RSA_EXP2_VAL                       (0x02U)    /**< the value uValue used in the calculation of exp2 */

#define PKCD_RSA_MODMINUS100_BYTE_POS           (0x0CU)    /**< the value used for caluculating 2^(mod size - 100) */
/*! \}*/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDRSATypesInternal
 * \{******************************************************************/
/** Large buffer area structure for RSA key generate operation */
typedef struct
{
    uint8_t mod[PKCD_RSA_MOD_LEN_MAX];          /**< A 512byte buffer */
    uint8_t p1[PKCD_RSA_PQ_LEN_MAX];            /**< A 256byte buffer */
    uint8_t q1[PKCD_RSA_PQ_LEN_MAX];            /**< A 256byte buffer */
    uint8_t p1q1[PKCD_RSA_MOD_LEN_MAX];         /**< A 512byte buffer */
    uint8_t gcd[PKCD_RSA_MOD_LEN_MAX];          /**< A 512byte buffer */
    uint8_t invgcd[PKCD_RSA_MOD_LEN_MAX];       /**< A 512byte buffer */
} st_rsa_keygen_buffer_t;

/** Works area structure for r_pkcd_rsa_ex_gcd_calc_even() */
typedef struct
{
    uint8_t *   p_wk_uv;       /**< An address of u or v work area */
    uint8_t *   p_wk_AC;       /**< An address of A or C work area */
    uint32_t *  p_signAC;     /**< An address of signed flag of A or C */
    uint8_t *   p_wk_BD;       /**< An address of B or D work area */
    uint32_t *  p_signBD;     /**< An address of signed flag of B or D */
    uint8_t *   p_wk_xy;       /**< An address of x or y work area */
} st_rsa_ex_gcd_calc_even_wks_t;

/** Data area structure for r_pkcd_rsa_ex_gcd_calc_even() */
typedef struct
{
    uint8_t *   p_x;          /**< An address of x */
    uint32_t    xlen;          /**< A byte length of x */
    uint8_t *   p_y;          /**< An address of y */
    uint32_t    ylen;          /**< A byte length of y */
} st_rsa_ex_gcd_xy_t;

/** A parameter structure of Modular exponentiation */
typedef struct
{
    uint8_t wk_n_minus1[PKCD_RSA_WORKUNIT];
    uint8_t wk_fact2_in_n_minus1[PKCD_RSA_WORKUNIT];
} st_rsa_prm_test_prime_t;

/** A structure of values that needs to be buffered in RSA key generation */
typedef struct
{
    uint32_t  key_len;                   /**< A byte length of the key */
    uint32_t  prime_test_num;            /**< The respective limit number of the trials for prime p, q generation */
    uint32_t  iteration_num;             /**< The number of the iteration of the Miller-Rabin test */
    uint32_t  e_len;                     /**< A byte length of a public exponent e */
    uint8_t   e_data[PKCD_RSA_PUB_EXP_FIXED_LEN];                    /**< A 4byte buffer */
    uint32_t  p_len;                                                 /**< A byte length of the prime p */
    uint8_t   p_data[PKCD_RSA_KEY_LEN_MAX / PKCD_CMN_DIV_2];         /**< A 256byte buffer */
    uint32_t  q_len;                                                 /**< A byte length of the prime q */
    uint8_t   q_data[PKCD_RSA_KEY_LEN_MAX / PKCD_CMN_DIV_2];         /**< A 256byte buffer */
    uint32_t  d_len;                                                 /**< A byte length of d */
    uint8_t   d_data[PKCD_RSA_KEY_LEN_MAX];                          /**< A 512byte buffer */
    uint8_t   n_data[PKCD_RSA_KEY_LEN_MAX];                          /**< A 512byte buffer */
    uint8_t   dp_data[PKCD_RSA_KEY_LEN_MAX / PKCD_CMN_DIV_2];        /**< A 256byte buffer */
    uint8_t   dq_data[PKCD_RSA_KEY_LEN_MAX / PKCD_CMN_DIV_2];        /**< A 256byte buffer */
    uint8_t   qinv_data[PKCD_RSA_KEY_LEN_MAX / PKCD_CMN_DIV_2];      /**< A 256byte buffer */
    uint8_t   candidate[PKCD_RSA_KEY_LEN_MAX / PKCD_CMN_DIV_2];      /**< A 256byte buffer */
    uint32_t  ck_offset;                 /**< A byte length of a buffer for the function checking p, q */
    uint8_t * p_test_random_address[PKCD_RSA_ITERATION_NUM_MAX];    /**< An address to random values used in
                                                                        Miller-Rabin test. */
} st_rsa_prm_keygen_t;

/** A structure of values used when checking private parameter p, q candidates */
typedef struct
{
    uint8_t pq1[PKCD_RSA_MOD_LEN_MAX / PKCD_CMN_DIV_2];            /**< A 256byte buffer */
    uint8_t for_check[PKCD_RSA_MOD_LEN_MAX];     /**< A 512byte buffer */
} st_rsa_check_pq_t;

/*! \}*/


/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup    PKCDRSAVariablesInternal
 * \{******************************************************************/
/** Set PKCC RAM address to pkccram of structure st_rsa_pkccram_t */
static st_rsa_pkccram_t * const sp_rsa_pkccram = ((st_rsa_pkccram_t*)PKCD_CMN_PKCCRAM);

/** Set WDT clear api */
static PKC_CALLBACK_T sp_rsa_wdtclr_api = NULL;
/*! \}*/

static pkcd_ret_t r_pkcd_rsa_modexp_init (st_rsa_prm_modexp_t * const p_prm, const uint8_t * const p_msg_in,
                                            const uint32_t msg_len, const st_pkcd_rsa_key_bin_t * const p_key_bin,
                                            const st_pkc_rsa_ctx_t * const p_ctx,
                                            const e_pkcd_rsa_exec_type_t exec_type);
static pkcd_ret_t r_pkcd_rsa_modexp_operate (st_rsa_prm_modexp_t * const p_prm,
                                                const e_pkc_sync_opt_t sync_option);
static pkcd_ret_t r_pkcd_rsa_modexp_sec (st_rsa_prm_modexp_t * const p_prm,
                                            const e_pkc_sync_opt_t sync_option);
static pkcd_ret_t r_pkcd_rsa_modexp_final (const st_rsa_prm_modexp_t * const p_prm, uint8_t * const p_msg_out,
                                            uint32_t * const p_msg_out_len);
static pkcd_ret_t r_pkcd_rsa_modexp_crt_init (st_rsa_prm_modexp_crt_t * const p_prm, const uint8_t * const p_msg_in,
                                                const uint32_t msg_len, const st_pkcd_rsa_key_bin_t * const p_key_bin,
                                                const st_pkc_rsa_ctx_t * const p_ctx);
static pkcd_ret_t r_pkcd_rsa_modexp_crt_operate (st_rsa_prm_modexp_crt_t * const p_prm,
                                                const st_pkc_rsa_ctx_t * const p_ctx,
                                                const e_pkc_sync_opt_t sync_option);
static pkcd_ret_t r_pkcd_rsa_modexp_crt_final (st_rsa_prm_modexp_crt_t * const p_prm, uint8_t * const p_msg_out,
                                                uint32_t * const p_msg_out_len);
static pkcd_ret_t r_pkcd_rsa_generate_crt (const st_pkcd_rsa_key_bin_t * const p_key_bin, uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_rsa_test_prime (st_rsa_prm_test_prime_t * const p_prm,
                                        const uint8_t * const p_candidate, const uint32_t prime_len,
                                        const uint32_t iteration_num, uint8_t * const p_test_randoms[]);
static pkcd_ret_t r_pkcd_rsa_calc_m (const uint8_t * const p_msg, const uint32_t msg_len, const uint8_t * const p_pq,
                                    const uint8_t * const p_dpq, const uint32_t pq_len,
                                    const e_pkc_sync_opt_t sync_option, const e_pkc_bool_t is_mod_p);
static pkcd_ret_t r_pkcd_rsa_calc_m_sec (const uint8_t * const p_msg, const uint32_t msg_len,
                                    const uint8_t * const p_pq, const uint8_t * const p_dpq, const uint32_t pq_len,
                                    const uint8_t * const p_random, const e_pkc_sync_opt_t sync_option,
                                    const e_pkc_bool_t is_mod_p);
static pkcd_ret_t r_pkcd_rsa_pre_prime_test (const uint8_t * const p_d, const uint32_t dlen);
static pkcd_ret_t r_pkcd_rsa_miller_rabin_test (st_rsa_prm_test_prime_t * const p_prm,
                                                const uint8_t * const p_n, const uint32_t nlen,
                                                const uint32_t iteration_num, uint8_t * const p_test_randoms[]);
static pkcd_ret_t r_pkcd_rsa_calc_key_d (const st_pkcd_rsa_key_bin_t * const p_key_bin,
                                        st_rsa_keygen_buffer_t * const p_kegen_buf);
static pkcd_ret_t r_pkcd_rsa_ex_gcd_calc_even (const st_rsa_ex_gcd_calc_even_wks_t * const p_wks,
                                                const st_rsa_ex_gcd_xy_t * const p_xy, const uint32_t len,
                                                uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_rsa_ex_gcd_APlusB (const uint8_t * const p_wk_a, uint32_t * const p_sign_a,
                                            const uint8_t * const p_wk_b, const uint32_t sign_b, const uint32_t len,
                                            uint8_t * const p_wk_tmp);
static pkcd_ret_t r_pkcd_rsa_ex_gcd_AMinusB (const uint8_t * const p_wk_a, uint32_t * const p_sign_a,
                                            const uint8_t * const p_wk_b, const uint32_t sign_b,
                                            const uint32_t len, uint8_t * const p_wk_tmp);
static pkcd_ret_t r_pkcd_rsa_ex_gcd_format_D (const uint8_t * const p_wk_D, const uint32_t signD,
                                                const uint8_t * const p_wk_m, uint8_t * const p_wk_temp,
                                                const uint32_t len);
static pkcd_ret_t r_pkcd_rsa_chk_mod_exp_init_arg (const st_pkc_data_t * const p_msg_in,
                                                    const void * const p_key,
                                                    const uint32_t mod_len,
                                                    const e_pkcd_rsa_exec_type_t exec_type,
                                                    const st_pkc_rsa_ctx_t * const p_ctx,
                                                    const uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_rsa_chk_mod_exp_crt_init_arg (const st_pkc_data_t * const p_msg_in,
                                                        const void * const p_priv_key,
                                                        const uint32_t mod_len,
                                                        const e_pkcd_rsa_exec_type_t exec_type,
                                                        const st_pkc_rsa_ctx_t * const p_ctx,
                                                        const uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_rsa_chk_test_prime_arg (const st_pkc_rsa_ctx_t * const p_ctx,
                                                const uint8_t * const p_buf,
                                                const st_pkc_data_t * const p_candidate,
                                                const uint32_t iteration_num,
                                                uint8_t * const p_test_randoms[]);
static pkcd_ret_t r_pkcd_rsa_chk_msg (const st_pkc_data_t * const p_msg, const st_pkcd_rsa_key_bin_t* const p_key_bin);
static pkcd_ret_t r_pkcd_rsa_chk_key_len (const uint32_t key_len);
static pkcd_ret_t r_pkcd_rsa_generate_p (st_rsa_prm_keygen_t * const p_keygen, const st_pkc_rsa_ctx_t * const p_ctx,
                                            uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_rsa_generate_q (st_rsa_prm_keygen_t * const p_keygen, const st_pkc_rsa_ctx_t * const p_ctx,
                                            uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_rsa_test_random_prime (st_rsa_prm_keygen_t * const p_keygen,
                                            const st_pkc_rsa_ctx_t * const p_ctx, uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_rsa_generate_p_q_n_d (st_rsa_prm_keygen_t * const p_keygen,
                                            const st_pkc_rsa_ctx_t * const p_ctx, uint8_t * const p_buf,
                                            uint32_t * const p_offset_for_gen_crt);
static pkcd_ret_t r_pkcd_rsa_check_p_q (uint8_t * const p_pq, const uint8_t * const p_p, const uint32_t pqlen,
                                        const uint8_t * const p_e, const uint32_t e_len, uint8_t * const p_buf,
                                        const e_pkc_bool_t is_mod_p);
static pkcd_ret_t r_pkcd_rsa_check_q (uint8_t * const p_wk_q, const uint8_t * const p_p, const uint32_t pqlen,
                                    u_rsa_work_t * const p_wk1, u_rsa_work_t * const p_wk2, uint8_t * const p_buf);
static pkcd_ret_t r_pkcd_rsa_generate_n_d(st_pkcd_rsa_key_bin_t* const p_key_bin,
                                        uint8_t* const p_buf);
static inline void r_pkcd_rsa_call_wdtclr(void);

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaModExpInit
**********************************************************************************************************************/
pkcd_ret_t   R_PKCD_RsaModExpInit(const st_pkc_data_t* const p_msg_in,
                                    const void* const p_key,
                                    const uint32_t mod_len,
                                    const e_pkcd_rsa_exec_type_t exec_type,
                                    const st_pkc_rsa_ctx_t* const p_ctx,
                                    uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_rsa_prm_modexp_t * const p_prm = (st_rsa_prm_modexp_t*)p_buf; /* No problem with casting to pointer size */
    const st_pkcd_rsa_key_bin_t * p_key_bin;

    /* check argument */
    ret = r_pkcd_rsa_chk_mod_exp_init_arg(p_msg_in, p_key, mod_len, exec_type, p_ctx, p_buf);
    if (PKCD_RET_OK == ret)
    {
        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* Clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        /* Clear buffer */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_rsa_prm_modexp_t));

        /* Cast void* to the driver-specified key binary pointer type. */
        p_key_bin = (const st_pkcd_rsa_key_bin_t*)p_key;

        ret = r_pkcd_rsa_modexp_init(p_prm, p_msg_in->p_data, p_msg_in->len, p_key_bin, p_ctx,
                                    exec_type);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_RsaModExpInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaModExpOperate
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_RsaModExpOperate(const st_pkc_rsa_ctx_t* const p_ctx,
                                    uint8_t* const p_buf,
                                    const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_rsa_prm_modexp_t * const p_prm = (st_rsa_prm_modexp_t*)p_buf; /* No problem with casting to pointer size */

    /* check argument */
    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_operate_opt))
    {

        if ((PKC_SYNC_OPT_SYNC == p_operate_opt->sync_opt) || (PKC_SYNC_OPT_ASYNC == p_operate_opt->sync_opt))
        {
            /* Clear async operation flag */
            r_pkcd_cmn_clr_async_ope();

            if (PKC_BOOL_FALSE != p_ctx->sec_mod_exp)
            {
                ret = r_pkcd_rsa_modexp_sec(p_prm, p_operate_opt->sync_opt);
            }
            else
            {
                ret = r_pkcd_rsa_modexp_operate(p_prm, p_operate_opt->sync_opt);
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
* End of function R_PKCD_RsaModExpOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaModExpFinal
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_RsaModExpFinal(const st_pkc_rsa_ctx_t* const p_ctx,
                                    uint8_t* const p_buf,
                                    st_pkc_data_t* const p_msg_out)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const st_rsa_prm_modexp_t * const p_prm = (st_rsa_prm_modexp_t*)p_buf; /* No problem with casting to
                                                                            pointer size */

    /* check argument */
    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_msg_out))
    {
        if (NULL != p_msg_out->p_data)
        {
            if (p_msg_out->len >= p_prm->mod_len)
            {
                /* Clear async operation flag */
                r_pkcd_cmn_clr_async_ope();

                ret = r_pkcd_rsa_modexp_final(p_prm, p_msg_out->p_data, &p_msg_out->len);
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
* End of function R_PKCD_RsaModExpFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaModExpCRTInit
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_RsaModExpCRTInit(const st_pkc_data_t* const p_msg_in,
                                    const void* const p_priv_key,
                                    const uint32_t mod_len,
                                    const e_pkcd_rsa_exec_type_t exec_type,
                                    const st_pkc_rsa_ctx_t* const p_ctx,
                                    uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_rsa_prm_modexp_crt_t * const p_prm = (st_rsa_prm_modexp_crt_t*)p_buf; /* No problem with casting
                                                                                to pointer size */
    const st_pkcd_rsa_key_bin_t * p_key_bin;

    /* check argument */
    ret = r_pkcd_rsa_chk_mod_exp_crt_init_arg(p_msg_in, p_priv_key, mod_len, exec_type, p_ctx, p_buf);
    if (PKCD_RET_OK == ret)
    {
        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* Clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        /* Clear buffer */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_rsa_prm_modexp_crt_t));

        /* Cast void* to the driver-specified key binary pointer type. */
        p_key_bin = (const st_pkcd_rsa_key_bin_t*)p_priv_key;

        ret = r_pkcd_rsa_modexp_crt_init(p_prm, p_msg_in->p_data, p_msg_in->len, p_key_bin, p_ctx);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_RsaModExpCRTInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaModExpCRTOperate
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_RsaModExpCRTOperate(const st_pkc_rsa_ctx_t* const p_ctx,
                                        uint8_t* const p_buf,
                                        const e_pkc_sync_opt_t sync_opt)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_rsa_prm_modexp_crt_t * const p_prm = (st_rsa_prm_modexp_crt_t*)p_buf; /* No problem with casting
                                                                                to pointer size */

    /* check argument */
    if (((NULL != p_ctx) && (NULL != p_buf)) &&
        ((PKC_SYNC_OPT_SYNC == sync_opt) || (PKC_SYNC_OPT_ASYNC == sync_opt)))
    {
        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        ret = r_pkcd_rsa_modexp_crt_operate(p_prm, p_ctx, sync_opt);
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_RsaModExpCRTOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaModExpCRTFinal
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_RsaModExpCRTFinal(const st_pkc_rsa_ctx_t* const p_ctx,
                                    uint8_t* const p_buf,
                                    st_pkc_data_t* const p_msg_out)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_rsa_prm_modexp_crt_t * const p_prm = (st_rsa_prm_modexp_crt_t*)p_buf; /* No problem with casting
                                                                                to pointer size */

    /* check argument */
    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_msg_out))
    {
        if (NULL != p_msg_out->p_data)
        {
            if (p_msg_out->len >= p_prm->mod_len)
            {
                /* Clear async operation flag */
                r_pkcd_cmn_clr_async_ope();

                ret = r_pkcd_rsa_modexp_crt_final(p_prm, p_msg_out->p_data, &p_msg_out->len);
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
* End of function R_PKCD_RsaModExpCRTFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaTestPrime
**********************************************************************************************************************/
pkcd_ret_t  R_PKCD_RsaTestPrime(const st_pkc_rsa_ctx_t* const p_ctx,
                                uint8_t* const p_buf,
                                const st_pkc_data_t* const p_candidate,
                                const uint32_t iteration_num,
                                uint8_t* const p_test_randoms[])
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_rsa_prm_test_prime_t * const p_prm = (st_rsa_prm_test_prime_t*)p_buf; /* No problem with casting
                                                                                to pointer size */
    PKC_CALLBACK_T wdtclr_api_bkp;

    /* check argument */
    ret = r_pkcd_rsa_chk_test_prime_arg(p_ctx, p_buf, p_candidate, iteration_num, p_test_randoms);
    if (PKCD_RET_OK == ret)
    {
        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* Clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        /* Set the API passed as an argument to a variable in the WDT clear API
           When called from r_pkcd_rsa_test_random_prime, sp_rsa_wdtclr_api is backed up because it is necessary to 
           return to the function pointer being called.*/
        wdtclr_api_bkp = sp_rsa_wdtclr_api;
        sp_rsa_wdtclr_api = p_ctx->p_callback_api;

        ret = r_pkcd_rsa_test_prime(p_prm, p_candidate->p_data, p_candidate->len, iteration_num, p_test_randoms);

        /* Set the backed up value to sp_rsa_wdtclr_api */
        sp_rsa_wdtclr_api = wdtclr_api_bkp;
    }
    else
    {
        /* Do nothing */
    }


    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_RsaTestPrime()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaKeyGenInit
**********************************************************************************************************************/
pkcd_ret_t  R_PKCD_RsaKeyGenInit(const st_pkc_rsa_ctx_t* const p_ctx,
                                uint8_t* const p_buf,
                                const uint32_t key_len,
                                const uint32_t prime_test_num,
                                const uint32_t iteration_num)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_rsa_prm_keygen_t * p_keygen;

    /* Check argument */
    if (NULL != p_ctx)
    {
        if ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api))
        {
            /* check key_len */
            if ((PKCD_RSA_KEY_LEN_1024 == key_len) || (PKCD_RSA_KEY_LEN_2048 == key_len) ||
                (PKCD_RSA_KEY_LEN_3072 == key_len) || (PKCD_RSA_KEY_LEN_4096 == key_len))
            {
                /* check prime_test_num */
                if ((PKCD_RSA_PRIME_TEST_NUM_MIN <= prime_test_num) && (PKCD_RSA_PRIME_TEST_NUM_MAX >= prime_test_num))
                {
                    /* check iteration_num */
                    if ((PKCD_RSA_ITERATION_NUM_MIN <= iteration_num) && (PKCD_RSA_ITERATION_NUM_MAX >= iteration_num))
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
    if (PKCD_RET_OK == ret)
    {
        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* Clear PKCC RAM */
        r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

        /* Cast p_buf to RSA KeyGen buffer type */
        p_keygen = (st_rsa_prm_keygen_t*)p_buf;

        /* buffer clear */
        r_pkcd_cmn_ram_clr(p_keygen->p_data, sizeof(p_keygen->p_data));
        r_pkcd_cmn_ram_clr(p_keygen->q_data, sizeof(p_keygen->q_data));
        r_pkcd_cmn_ram_clr(p_keygen->n_data, sizeof(p_keygen->n_data));
        r_pkcd_cmn_ram_clr(p_keygen->dp_data, sizeof(p_keygen->dp_data));
        r_pkcd_cmn_ram_clr(p_keygen->dq_data, sizeof(p_keygen->dq_data));
        r_pkcd_cmn_ram_clr(p_keygen->qinv_data, sizeof(p_keygen->qinv_data));
        p_keygen->p_len = 0U;
        p_keygen->q_len = 0U;
        p_keygen->d_len = 0U;
        p_keygen->e_len = 0U;

        /* Save arguments */
        p_keygen->key_len = key_len;
        p_keygen->prime_test_num = prime_test_num;
        p_keygen->iteration_num = iteration_num;
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_RsaKeyGenInit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaKeyGenOperate
**********************************************************************************************************************/
pkcd_ret_t  R_PKCD_RsaKeyGenOperate(const st_pkc_rsa_ctx_t* const p_ctx, uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const uint8_t pub_exp[PKCD_RSA_PUB_EXP_FIXED_LEN] = {0x00U, 0x01U, 0x00U, 0x01U};
    st_rsa_prm_keygen_t * const p_keygen = (st_rsa_prm_keygen_t*)p_buf; /* No problem with casting to pointer size */
    const uint32_t keygen_prm_offset = sizeof(st_rsa_prm_keygen_t);
    st_pkcd_rsa_key_bin_t key_bin;
    uint32_t offset_for_gen_crt = 0U;

    /* Check argument */
    if ((NULL != p_ctx) && (NULL != p_buf))
    {
        if (NULL != p_ctx->p_drbg_api)
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
        ret = PKCD_RET_PARAMERR;
    }

    if (PKCD_RET_OK == ret)
    {
        /* Clear async operation flag */
        r_pkcd_cmn_clr_async_ope();

        /* Set e */
        p_keygen->e_len = PKCD_RSA_PUB_EXP_FIXED_LEN;
        r_pkcd_cmn_cpy_byte(p_keygen->e_data, pub_exp, p_keygen->e_len);

        /* Set the API passed as an argument to a variable in the WDT clear API */
        sp_rsa_wdtclr_api = p_ctx->p_callback_api;

        /* Generate RSA key parameters p, q, d. */
        ret = r_pkcd_rsa_generate_p_q_n_d(p_keygen, p_ctx, &p_buf[keygen_prm_offset], &offset_for_gen_crt);

        if (PKCD_RET_OK == ret)
        {
            /* Set key binary */
            key_bin.key_length = p_keygen->key_len;
            key_bin.p_n = p_keygen->n_data;
            key_bin.p_d = p_keygen->d_data;
            key_bin.p_p = p_keygen->p_data;
            key_bin.p_q = p_keygen->q_data;
            key_bin.p_dp = p_keygen->dp_data;
            key_bin.p_dq = p_keygen->dq_data;
            key_bin.p_qinv = p_keygen->qinv_data;
            key_bin.e_length = p_keygen->e_len;
            key_bin.p_e = p_keygen->e_data;

            /* Generate CRT parameters (dp, dq, qinv) */
            ret = r_pkcd_rsa_generate_crt(&key_bin, &p_buf[keygen_prm_offset + offset_for_gen_crt]);
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

    /* Clear variables in WDT clear API */
    sp_rsa_wdtclr_api = NULL;

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_RsaKeyGenOperate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaKeyGenFinal
**********************************************************************************************************************/
pkcd_ret_t  R_PKCD_RsaKeyGenFinal(const st_pkc_rsa_ctx_t* const p_ctx,
                                uint8_t* const p_buf,
                                void* const p_priv_key,
                                void* const p_pub_key)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_pkcd_rsa_key_bin_t * p_priv_key_bin;
    st_pkcd_rsa_key_bin_t * p_pub_key_bin;
    const st_rsa_prm_keygen_t * const p_keygen = (st_rsa_prm_keygen_t*)p_buf; /* No problem with casting
                                                                                to pointer size */

    /* Check argument */
    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_priv_key) && (NULL != p_pub_key))
    {
        /* Cast void* to the driver-specified key binary pointer type. */
        p_priv_key_bin = (st_pkcd_rsa_key_bin_t*)p_priv_key;
        if ((NULL != p_priv_key_bin->p_n) && (NULL != p_priv_key_bin->p_d) && (NULL != p_priv_key_bin->p_p) &&
            (NULL != p_priv_key_bin->p_q) && (NULL != p_priv_key_bin->p_dp) && (NULL != p_priv_key_bin->p_dq) &&
            (NULL != p_priv_key_bin->p_qinv) && (NULL != p_priv_key_bin->p_e))
        {
            /* Cast void* to the driver-specified key binary pointer type. */
            p_pub_key_bin = (st_pkcd_rsa_key_bin_t*)p_pub_key;
            if ((NULL != p_pub_key_bin->p_n) && (NULL != p_pub_key_bin->p_e))
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
            ret = PKCD_RET_KEY_PARAMERR;
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


        /* Set key length */
        p_priv_key_bin->key_length = p_keygen->key_len;

        /* Set public parameters e and n */
        p_priv_key_bin->e_length = p_keygen->e_len;
        r_pkcd_cmn_cpy_word(p_priv_key_bin->p_e, p_keygen->e_data, p_priv_key_bin->e_length);
        r_pkcd_cmn_cpy_word(p_priv_key_bin->p_n, p_keygen->n_data, p_keygen->key_len);

        /* Set private exponents p and q */
        r_pkcd_cmn_cpy_word(p_priv_key_bin->p_p, p_keygen->p_data, p_keygen->p_len);
        r_pkcd_cmn_cpy_word(p_priv_key_bin->p_q, p_keygen->q_data, p_keygen->q_len);
        r_pkcd_cmn_cpy_word(p_priv_key_bin->p_d, p_keygen->d_data, p_keygen->d_len);

        /* Set private parameters for CRT mode */
        r_pkcd_cmn_cpy_word(p_priv_key_bin->p_dp, p_keygen->dp_data,
                            p_keygen->key_len / PKCD_CMN_DIV_2);
        r_pkcd_cmn_cpy_word(p_priv_key_bin->p_dq, p_keygen->dq_data,
                            p_keygen->key_len / PKCD_CMN_DIV_2);
        r_pkcd_cmn_cpy_word(p_priv_key_bin->p_qinv, p_keygen->qinv_data,
                            p_keygen->key_len / PKCD_CMN_DIV_2);

        /* Wrap public key */
        p_pub_key_bin->key_length = p_priv_key_bin->key_length;
        r_pkcd_cmn_cpy_word(p_pub_key_bin->p_n, p_priv_key_bin->p_n, p_priv_key_bin->key_length);
        p_pub_key_bin->e_length = p_priv_key_bin->e_length;
        r_pkcd_cmn_cpy_word(p_pub_key_bin->p_e, p_priv_key_bin->p_e, p_priv_key_bin->e_length);

        /* Clear the buffer */
        r_pkcd_cmn_ram_clr(p_buf, sizeof(st_rsa_prm_keygen_t));
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_RsaKeyGenFinal()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_modexp_init
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Prepare parameters for modular exponentiation.
 * \param       [in,out]  p_prm         Parameters to be calculated
 * \param       [in]  p_msg_in          An inputting message.
 * \param       [in]  msg_len           The length of the message in bytes.
 * \param       [in]  p_key_bin         RSA key of key binary format.
 * \param       [in]  p_ctx             A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]  exec_type         RSA execution type
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Prepare the following for the modular exponentiation operation.
 *        - #PKCD_CMN_PKCCRAM Clear.
 *        - Copy parameters to context.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If (#PKCD_RSA_EXEC_TYPE_DEC == exec_type) or (#PKCD_RSA_EXEC_TYPE_SIGN == exec_type)
 *   -# Set the argument p_key_bin->p_d into p_exp
 *   -# Set the argument p_key_bin->key_length into exp_len
 * -# Else
 *   -# Set the argument p_key_bin->p_e into p_exp
 *   -# Set the argument p_key_bin->e_length into exp_len
 * -# Copy parameters to p_prm->msg, p_prm->exp and p_prm->mod.
 * -# Set p_prm->msg_len to msg_len, p_prm->exp_len to exp_len, p_prm->mod_len to mod_len and
 *    p_prm->is_constant to p_ctx->is_constant_time_mode.
 * -# If #PKC_BOOL_FALSE != p_ctx->sec_mod_exp, then
 *   -# Generate 8-byte random value with p_ctx->p_drbg_api, and set drbg_ret to the return value.
 *   -# If #PKCD_CMN_EXTERNAL_API_OK == drbg_ret, then
 *     -# Set p_prm->pq_len to half the byte size of mod.
 *     -# Set p_prm->p1 to p-1 and p_prm->q1 to q-1.
 *     -# Set ret to #PKCD_RET_OK
 * -# Else
 *   -# Set ret to #PKCD_RET_OK
 * -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_modexp_init(st_rsa_prm_modexp_t* const p_prm, const uint8_t* const p_msg_in,
                                            const uint32_t msg_len, const st_pkcd_rsa_key_bin_t* const p_key_bin,
                                            const st_pkc_rsa_ctx_t * const p_ctx,
                                            const e_pkcd_rsa_exec_type_t exec_type)
{
    const uint8_t * p_exp;
    uint32_t exp_len;
    const uint8_t * const p_mod   = p_key_bin->p_n;
    const uint32_t mod_len = p_key_bin->key_length;
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t drbg_ret;

    /* Set exponent */
    if ((PKCD_RSA_EXEC_TYPE_DEC == exec_type) || (PKCD_RSA_EXEC_TYPE_SIGN == exec_type))
    {
        p_exp = p_key_bin->p_d;
        exp_len = p_key_bin->key_length;
    }
    else
    {
        p_exp = p_key_bin->p_e;
        exp_len = p_key_bin->e_length;
    }

    /* copy parameters */
    r_pkcd_cmn_cpy_word(p_prm->msg, p_msg_in, msg_len);
    p_prm->msg_len = msg_len;
    r_pkcd_cmn_cpy_word(p_prm->exp, p_exp, exp_len);
    p_prm->exp_len = exp_len;
    r_pkcd_cmn_cpy_word(p_prm->mod, p_mod, mod_len);
    p_prm->mod_len = mod_len;
    p_prm->is_constant = p_ctx->is_constant_time_mode;
    p_prm->p_buf = NULL;    /* Not used */

    if (PKC_BOOL_FALSE != p_ctx->sec_mod_exp)
    {
        /* Generate 64-bit random data for anti-tampering mod exp */
        drbg_ret = p_ctx->p_drbg_api(sizeof(p_prm->random), p_prm->random);

        if (PKCD_CMN_EXTERNAL_API_OK == drbg_ret)
        {
            p_prm->pq_len = p_key_bin->key_length / PKCD_CMN_DIV_2;

            /* make p-1, q-1 */
            r_pkcd_cmn_cpy_word(p_prm->p1, p_key_bin->p_p, p_prm->pq_len);
            p_prm->p1[p_prm->pq_len - 1U] =
                /* No problem for casting to support usual arithmetic conversion */
                (uint8_t)((uint32_t)p_prm->p1[p_prm->pq_len - 1U] & (uint32_t)PKCD_RSA_MASK_FE);

            r_pkcd_cmn_cpy_word(p_prm->q1, p_key_bin->p_q, p_prm->pq_len);
            p_prm->q1[p_prm->pq_len - 1U] =
                /* No problem for casting to support usual arithmetic conversion */
                (uint8_t)((uint32_t)p_prm->q1[p_prm->pq_len - 1U] & (uint32_t)PKCD_RSA_MASK_FE);

            ret = PKCD_RET_OK;
        }
        else
        {
            ret = PKCD_RET_DRBGAPIERR;
        }
    }
    else
    {
        ret = PKCD_RET_OK;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_modexp_init()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_modexp_operate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Perform a modular exponentioation.
 * \param       [in,out]  p_prm         Parameters to be calculated
 * \param       [in]  sync_option       Indicate if the function returns while PKCC is running.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_PDT_1, #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WORK4, #PKCD_RSA_WWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPW
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 *
 * \par Precondition
 *  - The RSA Context value must have been set beforehand.
 * \par Detailed description
 *      - Perform modular exponentiation operation using PKCC Operation.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set #PKCD_RSA_WWORK to #PKCD_CMN_ICUM_PKCCB0DPW
 * -# Set #PKCD_RSA_PDT_1 to #PKCD_CMN_ICUM_PKCCB0DPT
 * -# Set modulo length to DL bit and exponent length to BDL bit of #PKCD_CMN_ICUM_PKCCB0OMR with <br>
 *    the #r_pkcd_cmn_set_dl
 * -# Copy msg, exp and mod to works.
 * -# Use #r_pkcd_cmn_ram_clr to clear the potentially private key value stored in p_prm.
 * -# If Constant flag == PKC_BOOL_TRUE then
 *   -# Claclulate A^B mod C at constant time with the #r_pkcd_cmn_AExpBModCCon
 * -# Else
 *   -# Claclulate A^B mod C at variable time with the #r_pkcd_cmn_AExpBModC
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_modexp_operate(st_rsa_prm_modexp_t* const p_prm, const e_pkc_sync_opt_t sync_option)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* prepare operation */
    PKCD_CMN_ICUM_PKCCB0DPW = PKCD_RSA_FORMAT_DP_DPW(PKCD_RSA_WWORK);
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_RSA_FORMAT_DP_DPT(PKCD_RSA_PDT_1);
    r_pkcd_cmn_set_dl(p_prm->mod_len, p_prm->exp_len);

    /* Copy data */
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK1->full + PKCD_RSA_OFFSET_VAL(p_prm->msg_len),
                                p_prm->msg, p_prm->msg_len);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK2->full + PKCD_RSA_OFFSET_VAL(p_prm->exp_len),
                                p_prm->exp, p_prm->exp_len);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK3->full + PKCD_RSA_OFFSET_VAL(p_prm->mod_len),
                                p_prm->mod, p_prm->mod_len);

    /* Clear the value that may be the private key stored in p_prm */
    r_pkcd_cmn_ram_clr(p_prm->exp, p_prm->exp_len);

    /* Calc S = A^B mod C */
    if (PKC_BOOL_TRUE == p_prm->is_constant)
    {
        ret = r_pkcd_cmn_AExpBModCCon(PKCD_RSA_WORK1->full, PKCD_RSA_WORK2->full, PKCD_RSA_WORK3->full,
                                        PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNIT, sync_option);
    }
    else
    {
        ret = r_pkcd_cmn_AExpBModC(PKCD_RSA_WORK1->full, PKCD_RSA_WORK2->full, PKCD_RSA_WORK3->full,
                                    PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNIT, sync_option);
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_modexp_operate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_modexp_sec
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Perform a modular exponentioation with randomizing the exponent.
 * \param       [in,out]  p_prm         Parameters to be calculated
 * \param       [in]  sync_option       Indicate if the function returns while PKCC is running.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_PDT_1, #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WORK4, #PKCD_RSA_WWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPW
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 *
 * \par Precondition
 *  - The RSA Context value must have been set beforehand.
 * \par Detailed description
 *      - Perform modular exponentiation operation using PKCC Operation.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set #PKCD_RSA_WWORK to #PKCD_CMN_ICUM_PKCCB0DPW
 * -# Set #PKCD_RSA_PDT_1 to #PKCD_CMN_ICUM_PKCCB0DPT
 * -# Copy the modulo to a work area.
 * -# Calculate T of the modulo with #r_pkcd_cmn_MinusCInv.
 * -# Copy p-1, q-1, and random to works.
 * -# Use #r_pkcd_cmn_ram_clr to clear the potentially private key value stored in p_prm.
 * -# Set DL bit and BDL bit of #PKCD_CMN_ICUM_PKCCB0OMR to exponent length with #r_pkcd_cmn_set_dl.
 * -# Calculate (p-1)(q-1) with #r_pkcd_cmn_AB.
 * -# Set DL bit and BDL bit of #PKCD_CMN_ICUM_PKCCB0OMR to modulo length with #r_pkcd_cmn_set_dl.
 * -# Calculate random * (p-1)(q-1) with #r_pkcd_cmn_AB64.
 * -# Set DL bit and BDL bit of #PKCD_CMN_ICUM_PKCCB0OMR to modulo length + 8bytes with #r_pkcd_cmn_set_dl.
 * -# Copy exp to a work area.
 * -# Calculate exp + random * (p-1)(q-1) with #r_pkcd_cmn_APlusB.
 * -# Set DL bit of #PKCD_CMN_ICUM_PKCCB0OMR to the modulo length, and BDL bit to the modulo length + 1byte.
 * -# If Constant flag == #PKC_BOOL_TRUE then
 *   -# Calclulate A^B mod C at constant time with the #r_pkcd_cmn_AExpBModCCon
 * -# Else
 *   -# Calclulate A^B mod C at variable time with the #r_pkcd_cmn_AExpBModC
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_modexp_sec(st_rsa_prm_modexp_t* const p_prm, const e_pkc_sync_opt_t sync_option)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* prepare operation */
    PKCD_CMN_ICUM_PKCCB0DPW = PKCD_RSA_FORMAT_DP_DPW(PKCD_RSA_WWORK);
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_RSA_FORMAT_DP_DPT(PKCD_RSA_PDT_1);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK2->full + PKCD_RSA_OFFSET_VAL(p_prm->mod_len), p_prm->mod, p_prm->mod_len);
    ret = r_pkcd_cmn_MinusCInv(PKCD_RSA_WORK2->full,
                                /* No problem with casting to pointer size */
                                (uint8_t*)PKCD_RSA_FORMAT_DP_DPT(PKCD_RSA_PDT_1), PKCD_RSA_WORKUNIT);

    /* Copy data */
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK1->half.h + PKCD_RSA_OFFSET_VALHALF(p_prm->pq_len),
                                p_prm->p1, p_prm->pq_len);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK1->half.l + PKCD_RSA_OFFSET_VALHALF(p_prm->pq_len),
                                p_prm->q1, p_prm->pq_len);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK3->full + PKCD_RSA_OFFSET_VAL(sizeof(p_prm->random)),
                                p_prm->random, sizeof(p_prm->random));

    /* Clear the value that may be the private key stored in p_prm */
    r_pkcd_cmn_ram_clr(p_prm->p1, p_prm->pq_len);
    r_pkcd_cmn_ram_clr(p_prm->q1, p_prm->pq_len);
    r_pkcd_cmn_ram_clr(p_prm->random, sizeof(p_prm->random));

    /* Calculate random(64-bit) * (p-1)(q-1), and set PKCD_RSA_WORK1->full to the result */
    r_pkcd_cmn_set_dl(p_prm->pq_len, p_prm->pq_len);
    ret |= r_pkcd_cmn_AB(PKCD_RSA_WORK1->half.h, PKCD_RSA_WORK1->half.l, PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNITHALF);
    r_pkcd_cmn_set_dl(p_prm->mod_len, p_prm->mod_len);
    ret |= r_pkcd_cmn_AB64(PKCD_RSA_WORK3->full, PKCD_RSA_WORK4->full, PKCD_RSA_WORK1->full, PKCD_RSA_WORKUNIT);

    /* Calculate exp + random(64-bit) * (p-1)(q-1), and set PKCD_RSA_WORK3->full to the result */
    r_pkcd_cmn_set_dl(p_prm->mod_len + (sizeof(p_prm->random)), p_prm->mod_len + (sizeof(p_prm->random)));
    ret |= r_pkcd_cmn_clr_work_unit(PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK4->full + PKCD_RSA_OFFSET_VAL(p_prm->exp_len), p_prm->exp, p_prm->exp_len);
    ret |= r_pkcd_cmn_APlusB(PKCD_RSA_WORK4->full, PKCD_RSA_WORK1->full, PKCD_RSA_WORK3->full, PKCD_RSA_WORKUNIT);

    /* Calculate m^(exp + random(64-bit)*(p-1)(q-1)) mod n */
    r_pkcd_cmn_set_dl(p_prm->mod_len, p_prm->mod_len + ((sizeof(p_prm->random)) + 1U));
    ret |= r_pkcd_cmn_clr_work_unit(PKCD_RSA_WORK1->full, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK1->full + PKCD_RSA_OFFSET_VAL(p_prm->msg_len),
                                p_prm->msg, p_prm->msg_len);

    if (PKC_BOOL_TRUE == p_prm->is_constant)
    {
        ret |= r_pkcd_cmn_AExpBModCCon(PKCD_RSA_WORK1->full, PKCD_RSA_WORK3->full, PKCD_RSA_WORK2->full,
                                        PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNIT, sync_option);
    }
    else
    {
        ret |= r_pkcd_cmn_AExpBModC(PKCD_RSA_WORK1->full, PKCD_RSA_WORK3->full, PKCD_RSA_WORK2->full,
                                    PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNIT, sync_option);
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_modexp_sec()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_modexp_final
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Finalize a modular exponentioation.
 * \param       [in]   p_prm           Parameters to be calculated
 * \param       [out]  p_msg_out       An address where the modular exponentiation result is output. <br>
 *                                     The length of output is equal to that of the modulus.
 * \param       [out] p_msg_out_len    An address where the modular exponentiation result length is output.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_WORK4
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - The RSA Context value must have been set beforehand.
 *  - The modular exponentiation is calculated by #r_pkcd_rsa_modexp_operate.
 * \par Detailed description
 *      - Output the result of modular exponentiation operation by #r_pkcd_rsa_modexp_operate.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_OK.
 * -# Initialize the variable mod_len with p_prm->mod_len.
 * -# Copy the result calculated by #r_pkcd_rsa_modexp_operate to p_msg_out.
 * -# Set mod_len into *p_msg_out_len
 * -# Return #PKCD_RET_OK
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_modexp_final(const st_rsa_prm_modexp_t* const p_prm, uint8_t* const p_msg_out,
                                            uint32_t* const p_msg_out_len)
{
    const uint32_t mod_len = p_prm->mod_len;

    /* copy result */
    r_pkcd_cmn_cpy_by_64bounds(p_msg_out, PKCD_RSA_WORK4->full + PKCD_RSA_OFFSET_VAL(mod_len), mod_len);

    *p_msg_out_len = mod_len;

    return PKCD_RET_OK;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_modexp_final()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_modexp_crt_init
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Prepare parameters for modular exponentiation with the Chinese remainder theorem.
 * \param       [in,out]  p_prm         Parameters to be calculated
 * \param       [in]  p_msg_in          An inputting message.
 * \param       [in]  msg_len           The length of the message in bytes.
 * \param       [in]  p_key_bin         RSA private key of key binary format.
 * \param       [in]  p_ctx             A pointer to a structure that summarizes the parameters required by RSA.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Prepare the following for the modular exponentiation operation.
 *        - #PKCD_CMN_PKCCRAM Clear.
 *        - Copy parameters to context.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Copy parameters to p_prm
 * -# If #PKC_BOOL_FALSE != p_ctx->sec_mod_exp, then
 *   -# Generate 8-byte rundom number with p_ctx->p_drbg_api with p_ctx->p_drbg_api,
 *      and set drbg_ret to the return value.
 *   -# If #PKCD_CMN_EXTERNAL_API_OK == drbg_ret, then
 *     -# Generate 8-byte rundom number with p_ctx->p_drbg_api with p_ctx->p_drbg_api,
 *        and set drbg_ret to the return value.
 *     -# If #PKCD_CMN_EXTERNAL_API_OK == drbg_ret, then
 *       -# Set ret to #PKCD_RET_OK.
 *     -# Else
 *       -# Set ret to #PKCD_RET_DRBGAPIERR.
 *   -# Else
 *     -# Set ret to #PKCD_RET_DRBGAPIERR.
 * -# Else
 *   -# Set ret to #PKCD_RET_OK.
 * -# Clear p_prm->operate_call_cnt with #PKCD_RSA_CRT_OPERATE_NOT_CALLED
 * -# Return #PKCD_RET_OK
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_modexp_crt_init(st_rsa_prm_modexp_crt_t* const p_prm, const uint8_t* const p_msg_in,
                                                const uint32_t msg_len, const st_pkcd_rsa_key_bin_t* const p_key_bin,
                                                const st_pkc_rsa_ctx_t* const p_ctx)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const uint32_t mod_len = p_key_bin->key_length;
    const uint32_t pq_len  = (p_key_bin->key_length / PKCD_CMN_DIV_2);
    const uint8_t * const p_p     = p_key_bin->p_p;
    const uint8_t * const p_q     = p_key_bin->p_q;
    const uint8_t * const p_dp    = p_key_bin->p_dp;
    const uint8_t * const p_dq    = p_key_bin->p_dq;
    const uint8_t * const p_qinv  = p_key_bin->p_qinv;
    uint32_t drbg_ret;

    /* copy parameters */
    r_pkcd_cmn_cpy_word(&p_prm->msg[mod_len - msg_len], p_msg_in, msg_len);
    p_prm->msg_len = mod_len;
    p_prm->mod_len = mod_len;
    r_pkcd_cmn_cpy_word(p_prm->p,    p_p,    pq_len);
    r_pkcd_cmn_cpy_word(p_prm->q,    p_q,    pq_len);
    r_pkcd_cmn_cpy_word(p_prm->dp,   p_dp,   pq_len);
    r_pkcd_cmn_cpy_word(p_prm->dq,   p_dq,   pq_len);
    r_pkcd_cmn_cpy_word(p_prm->qinv, p_qinv, pq_len);
    p_prm->pq_len = pq_len;
    p_prm->p_buf = NULL;    /* Not used */

    /* Generate a random number to be used in anti-leakage mod exp */
    if (PKC_BOOL_FALSE != p_ctx->sec_mod_exp)
    {
        drbg_ret = p_ctx->p_drbg_api(sizeof(p_prm->random_p), p_prm->random_p);

        if (PKCD_CMN_EXTERNAL_API_OK == drbg_ret)
        {
            drbg_ret = p_ctx->p_drbg_api(sizeof(p_prm->random_q), p_prm->random_q);

            if (PKCD_CMN_EXTERNAL_API_OK == drbg_ret)
            {
                ret = PKCD_RET_OK;
            }
            else
            {
                ret = PKCD_RET_DRBGAPIERR;
            }
        }
        else
        {
            ret = PKCD_RET_DRBGAPIERR;
        }
    }
    else
    {
        ret = PKCD_RET_OK;
    }

    /* clear Operate count */
    p_prm->operate_call_cnt = PKCD_RSA_CRT_OPERATE_NOT_CALLED;

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_modexp_crt_init()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_modexp_crt_operate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Perform a modular exponentioation with the Chinese remainder theorem.
 * \param       [in,out]  p_prm         Parameters to be calculated
 * \param       [in]  p_ctx             A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]  sync_option       Indicate if the function returns while PKCC is running.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_PROCESSING:          Successfully done.
 *                                   Next half of the operation unprocessed.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT, (Used by sub functions)
 *    #PKCD_CMN_ICUM_PKCCB0DPW
 * \par Precondition
 *  - The RSA Context value must have been set beforehand.
 * \par Detailed description
 *      - Calculate m used in CRT mode.
 *      - When calling with #PKCD_RSA_CRT_OPERATE_NOT_CALLED == p_prm->operate_call_cnt, calculate m1 and <br>
 *        when calling with #PKCD_RSA_CRT_OPERATE_CALLED_ONCE == p_prm->operate_call_cnt, calculate m2.
 *        when calling with #PKCD_RSA_CRT_OPERATE_CALLED_TWICE == p_prm->operate_call_cnt, #PKCD_RET_INTERNALERR <br>
 *        returned.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If #PKCD_RSA_CRT_OPERATE_NOT_CALLED == p_prm->operate_call_cnt then
 *   -# If #PKC_BOOL_FALSE != p_ctx->sec_mod_exp, then
 *     -# Calculate m1 with the #r_pkcd_rsa_calc_m_sec.
 *   -# Else
 *     -# Calculate m1 with the #r_pkcd_rsa_calc_m.
 *   -# Set p_prm->operate_call_cnt to #PKCD_RSA_CRT_OPERATE_CALLED_ONCE.
 *   -# If #PKCD_RET_OK == ret then
 *     -# Set ret to #PKCD_RET_PROCESSING.
 *   -# Use #r_pkcd_cmn_ram_clr to clear the potentially private key value stored in p_prm.
 * -# Else if #PKCD_RSA_CRT_OPERATE_CALLED_ONCE == p_prm->operate_call_cnt then
 *   -# If #PKC_BOOL_FALSE != p_ctx->sec_mod_exp, then
 *     -# Calculate m2 with the #r_pkcd_rsa_calc_m_sec.
 *   -# Else
 *     -# Calculate m2 with the #r_pkcd_rsa_calc_m.
 *   -# Set #PKCD_RSA_CRT_OPERATE_CALLED_TWICE to p_prm->operate_call_cnt
 *   -# Use #r_pkcd_cmn_ram_clr to clear the potentially private key value stored in p_prm.
 * -# Else
 *   -# Do nothing
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_modexp_crt_operate(st_rsa_prm_modexp_crt_t* const p_prm,
                                                const st_pkc_rsa_ctx_t * const p_ctx,
                                                const e_pkc_sync_opt_t sync_option)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* Check call count */
    if (PKCD_RSA_CRT_OPERATE_NOT_CALLED == p_prm->operate_call_cnt)
    {
        if (PKC_BOOL_FALSE != p_ctx->sec_mod_exp)
        {
            ret = r_pkcd_rsa_calc_m_sec(p_prm->msg, p_prm->msg_len, p_prm->p, p_prm->dp, p_prm->pq_len,
                                    p_prm->random_p, sync_option, PKC_BOOL_TRUE);
        }
        else
        {
            ret = r_pkcd_rsa_calc_m(p_prm->msg, p_prm->msg_len, p_prm->p, p_prm->dp, p_prm->pq_len, sync_option,
                                    PKC_BOOL_TRUE);
        }

        p_prm->operate_call_cnt = PKCD_RSA_CRT_OPERATE_CALLED_ONCE;
        ret = (PKCD_RET_OK == ret) ? PKCD_RET_PROCESSING : ret;

        /* Clear the private key value stored in p_prm */
        r_pkcd_cmn_ram_clr(p_prm->p, sizeof(p_prm->p));
        r_pkcd_cmn_ram_clr(p_prm->dp, sizeof(p_prm->dp));
        r_pkcd_cmn_ram_clr(p_prm->random_p, sizeof(p_prm->random_p));
    }
    else if (PKCD_RSA_CRT_OPERATE_CALLED_ONCE == p_prm->operate_call_cnt)
    {
        if (PKC_BOOL_FALSE != p_ctx->sec_mod_exp)
        {
            ret = r_pkcd_rsa_calc_m_sec(p_prm->msg, p_prm->msg_len, p_prm->q, p_prm->dq, p_prm->pq_len,
                                    p_prm->random_q, sync_option, PKC_BOOL_FALSE);
        }
        else
        {
            ret = r_pkcd_rsa_calc_m(p_prm->msg, p_prm->msg_len, p_prm->q, p_prm->dq, p_prm->pq_len, sync_option,
                                    PKC_BOOL_FALSE);
        }

        p_prm->operate_call_cnt = PKCD_RSA_CRT_OPERATE_CALLED_TWICE;

        /* Clear the private key value stored in p_prm */
        r_pkcd_cmn_ram_clr(p_prm->q, sizeof(p_prm->q));
        r_pkcd_cmn_ram_clr(p_prm->dq, sizeof(p_prm->dq));
        r_pkcd_cmn_ram_clr(p_prm->random_q, sizeof(p_prm->random_q));
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_modexp_crt_operate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_modexp_crt_final
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Finalize a modular exponentioation with the Chinese remainder theorem.
 * \param       [in]  p_prm              Parameters to be calculated
 * \param       [out] p_msg_out           An address where the modular exponentiation  result is output. <br>
 *                                        The length of output is equal to that of the modulus.
 * \param       [out] p_msg_out_len       An address where the modular exponentiation result length is output.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_PDT_1, #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WORK4, <br>
 *    #PKCD_RSA_SPAREWORK1, #PKCD_RSA_SPAREWORK2,
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - The RSA Context value must have been set beforehand.
 *  - The m1 and m2 must be calculated.
 * \par Detailed description
 *      - Perform the remaining calculation in the CRT mode and output the results.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set #PKCD_RSA_PDT_1 to #PKCD_CMN_ICUM_PKCCB0DPT
 * -# Set pq_len to DL and BDL bit of #PKCD_CMN_ICUM_PKCCB0OMR with the #r_pkcd_cmn_set_dl
 * -# Calculate m2 - p to check m2 > p.
 * -# Loop while in the following case
 *   - The timeout counter has not exceeded the #PKCD_RSA_REDUCE_LOOP_TIMEOUT
 *   - m2 - p > 0
 *   -# Calculate m2 = m2 - p with the #r_pkcd_cmn_AMinusB
 *   -# Increment timeout count
 * -# If timeout count >= #PKCD_RSA_REDUCE_LOOP_TIMEOUT then
 *   -# Set ret to #PKCD_RET_PKCCTIMEOUTERR
 * -# Calculate (m1 - m2) mod p with the #r_pkcd_cmn_AMinusBModC
 * -# Calculate h = qinv * ((m1 - m2) mod p) with the #r_pkcd_cmn_ABRInvModC
 * -# Use #r_pkcd_cmn_ram_clr to clear the potentially private key value stored in p_prm.
 * -# Calculate h * q with the #r_pkcd_cmn_AB
 * -# Calculate msg = m2 + (h * q) with the #r_pkcd_cmn_APlusB
 * -# Copy msg to  p_msg_out with the #r_pkcd_cmn_cpy_by_64bounds
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_modexp_crt_final(st_rsa_prm_modexp_crt_t* const p_prm, uint8_t* const p_msg_out,
                                                uint32_t* const p_msg_out_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const uint32_t pq_len = p_prm->pq_len;
    const uint32_t mod_len = p_prm->mod_len;
    const uint8_t * const p_qinv = p_prm->qinv;
    uint32_t sign_trial;
    volatile uint32_t timeout_cnt;
    const uint8_t * p_wk_m2;
    const uint8_t * p_wk_dmy = NULL;

    /* prepare operation */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_RSA_FORMAT_DP_DPT(PKCD_RSA_PDT_1);
    r_pkcd_cmn_set_dl(pq_len, pq_len);

    /* if m2 > p, calc m2 - p */
    p_wk_m2 = PKCD_RSA_WORK4->half.l;
    r_pkcd_cmn_ram_clr(PKCD_RSA_SPAREWORK1->half.h, PKCD_RSA_WORKUNITHALF);
    r_pkcd_cmn_ram_clr(PKCD_RSA_SPAREWORK1->half.l, PKCD_RSA_WORKUNITHALF);
    ret = r_pkcd_cmn_AMinusB(p_wk_m2, PKCD_RSA_WORK1->half.h, PKCD_RSA_SPAREWORK1->half.h, PKCD_RSA_WORKUNITHALF);
    r_pkcd_cmn_cpy_by_64bounds((uint8_t*)&sign_trial,  /* No problem with casting to pointer size */
                                PKCD_RSA_SPAREWORK1->half.h + PKCD_RSA_OFFSET_VALHALF(pq_len + sizeof(uint32_t)),
                                sizeof(uint32_t));
    timeout_cnt = 0U;
    while ((PKCD_RSA_REDUCE_LOOP_TIMEOUT > timeout_cnt) && (0U == sign_trial))
    {
        p_wk_m2  = (p_wk_m2  != PKCD_RSA_SPAREWORK1->half.h) ?
                    PKCD_RSA_SPAREWORK1->half.h : PKCD_RSA_SPAREWORK1->half.l;
        p_wk_dmy = (p_wk_dmy != PKCD_RSA_SPAREWORK1->half.l) ?
                    PKCD_RSA_SPAREWORK1->half.l : PKCD_RSA_SPAREWORK1->half.h;
        ret |= r_pkcd_cmn_AMinusB(p_wk_m2, PKCD_RSA_WORK1->half.h, p_wk_dmy, PKCD_RSA_WORKUNITHALF);
        r_pkcd_cmn_cpy_by_64bounds((uint8_t*)&sign_trial, /* No problem with casting to pointer size */
                                    p_wk_dmy + PKCD_RSA_OFFSET_VALHALF(pq_len + sizeof(uint32_t)),
                                    sizeof(uint32_t));
        timeout_cnt += 1U;
    }

    /* If loop is timeout, return error */
    if (timeout_cnt >= PKCD_RSA_REDUCE_LOOP_TIMEOUT)
    {
        ret |= PKCD_RET_PKCCTIMEOUTERR;
    }
    else
    {
        /* Do nothing */
    }

    /* Calc (m1 - m2) mod p */
    ret |= r_pkcd_cmn_AMinusBModC(PKCD_RSA_WORK4->half.h, p_wk_m2, PKCD_RSA_WORK1->half.h, PKCD_RSA_WORK3->half.h,
                                    PKCD_RSA_WORKUNITHALF);

    /* Calc h = qInv * (m1 - m2) mod p */
    r_pkcd_cmn_ram_clr(PKCD_RSA_WORK2->half.l, PKCD_RSA_WORKUNITHALF);
    r_pkcd_cmn_ram_clr(PKCD_RSA_SPAREWORK1->half.h, PKCD_RSA_WORKUNITHALF);
    r_pkcd_cmn_ram_clr(PKCD_RSA_WORK3->half.l, PKCD_RSA_WORKUNITHALF);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK3->half.l + PKCD_RSA_OFFSET_VALHALF(pq_len), p_qinv, pq_len);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_RSA_WORK3->half.l, PKCD_RSA_WORK3->half.h, PKCD_RSA_WORK1->half.h,
                                    PKCD_RSA_WORK2->half.l, PKCD_RSA_WORKUNITHALF);
    ret |= r_pkcd_cmn_ABRInvModC(PKCD_RSA_WORK2->half.l, PKCD_RSA_WORK2->half.h, PKCD_RSA_WORK1->half.h,
                                    PKCD_RSA_SPAREWORK1->half.h, PKCD_RSA_WORKUNITHALF);

    /* Clear the private key value stored in p_prm */
    r_pkcd_cmn_ram_clr(p_prm->qinv, sizeof(p_prm->qinv));

    /* Calc (h * q) */
    ret |= r_pkcd_cmn_AB(PKCD_RSA_SPAREWORK1->half.h, PKCD_RSA_WORK1->half.l, PKCD_RSA_WORK3->full,
                        PKCD_RSA_WORKUNITHALF);

    /* Calc M = m2 + (h * q) */
    r_pkcd_cmn_set_dl(mod_len, pq_len);
    r_pkcd_cmn_ram_clr(PKCD_RSA_WORK4->half.h, PKCD_RSA_WORKUNITHALF);
    ret |= r_pkcd_cmn_APlusB(PKCD_RSA_WORK4->full, PKCD_RSA_WORK3->full, PKCD_RSA_WORK1->full, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(p_msg_out, PKCD_RSA_WORK1->full + PKCD_RSA_OFFSET_VAL(mod_len), mod_len);

    *p_msg_out_len = mod_len;

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_modexp_crt_final()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_generate_crt
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Calculate RSA key parameters dp, dq, qinv from P and Q.
 * \param       [in,out]  p_key_bin     RSA key of key binary format.
 * \param       [in]      p_buf         Set 1,300byte buffer area.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 *  - #PKCD_RET_RSA_NON_COPRIME:     (e and p-1), (e and q-1) or (p and q) are not co-prime.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM, #PKCD_RSA_PDT_2, #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WORK4,
 *    #PKCD_RSA_WWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPW, #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - key_len, pq_len, e_len, p_e, p_p and p_q of p_key and p-1, q-1 of p_kegen_buf must have been set beforehand.
 * \par Detailed description
 *      - Calculate RSA key parameters n, dp, dq, qinv from P and Q.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCD_RET_INTERNALERR.
 *  -# Assign a buffer for the key generation as p_kegen_buf from p_buf.
 *  -# Set pq_length to p_key_bin->key_length/#PKCD_CMN_DIV_2.
 *  -# Set the PKCC DL to pq_length with #r_pkcd_cmn_set_dl.
 *  -# Set #PKCD_CMN_ICUM_PKCCB0DPW to #PKCD_RSA_WWORK.
 *  -# Clear p_kegen_buf->gcd with #r_pkcd_cmn_ram_clr.
 *  -# Generate dp = e^(-1) mod (p-1) on p_key_bin->p_dp with #r_pkcd_rsa_inverse and set ret to the result.
 *  -# If #PKCD_RET_OK == ret then
 *    -# Clear p_kegen_buf->gcd with #r_pkcd_cmn_ram_clr.
 *    -# Generate dq = e^(-1) mod (q-1) on p_key_bin->p_dq with #r_pkcd_rsa_inverse and set ret to the result.
 *    -# If #PKCD_RET_OK == ret then,
 *      -# Set the PKCC DL and BDL to pq_length with #r_pkcd_cmn_set_dl.
 *      -# Clear #PKCD_RSA_WORK1 with #r_pkcd_cmn_ram_clr.
 *      -# Clear #PKCD_RSA_WORK2 with #r_pkcd_cmn_ram_clr.
 *      -# Clear #PKCD_RSA_WORK3 with #r_pkcd_cmn_ram_clr.
 *      -# Clear #PKCD_RSA_WORK4 with #r_pkcd_cmn_ram_clr.
 *      -# Copy q at p_key_bin->p_q to #PKCD_RSA_WORK1 with #r_pkcd_cmn_cpy_by_64bounds.
 *      -# Copy p at p_key_bin->p_p to #PKCD_RSA_WORK2 with #r_pkcd_cmn_cpy_by_64bounds.
 *      -# Set #PKCD_CMN_ICUM_PKCCB0DPT to the address of #PKCD_RSA_PDT_1.
 *      -# Generate qinv = q^(-1) mod p on #PKCD_RSA_WORK3 with #r_pkcd_cmn_modinv_fermat
 *         and set ret to the result.
 *      -# Copy qinv in #PKCD_RSA_WORK3 to p_key_bin->p_qinv with #r_pkcd_cmn_cpy_by_64bounds.
 *    -# Else
 *      -# Do nothing.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_generate_crt(const st_pkcd_rsa_key_bin_t* const p_key_bin, uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    st_rsa_keygen_buffer_t * const p_kegen_buf = (st_rsa_keygen_buffer_t*)p_buf; /* No problem with casting
                                                                                    to pointer size */
    const uint32_t pq_length = p_key_bin->key_length / PKCD_CMN_DIV_2;

    /* set DL */
    r_pkcd_cmn_set_dl(p_key_bin->key_length, 0U);

    /* set WORK */
    PKCD_CMN_ICUM_PKCCB0DPW = PKCD_RSA_FORMAT_DP_DPW(PKCD_RSA_WWORK);

    /* calc dp = E^(-1) mod (p-1) */
    r_pkcd_cmn_ram_clr(p_kegen_buf->gcd, PKCD_RSA_PQ_LEN_MAX);
    ret = r_pkcd_rsa_inverse(p_key_bin->p_e, p_key_bin->e_length, p_kegen_buf->p1, pq_length,
                                p_key_bin->p_dp, p_kegen_buf->gcd);

    if (PKCD_RET_OK == ret)
    {
        /* calc dq = E^(-1) mod (q-1) */
        r_pkcd_cmn_ram_clr(p_kegen_buf->gcd, PKCD_RSA_PQ_LEN_MAX);
        ret = r_pkcd_rsa_inverse(p_key_bin->p_e, p_key_bin->e_length, p_kegen_buf->q1, pq_length,
                                    p_key_bin->p_dq, p_kegen_buf->gcd);
        if (PKCD_RET_OK == ret)
        {
            /* calc qinv = q^(-1) mod p */
            r_pkcd_cmn_set_dl(pq_length, pq_length);
            r_pkcd_cmn_ram_clr(PKCD_RSA_WORK1->full, PKCD_RSA_WORKUNIT);
            r_pkcd_cmn_ram_clr(PKCD_RSA_WORK2->full, PKCD_RSA_WORKUNIT);
            r_pkcd_cmn_ram_clr(PKCD_RSA_WORK3->full, PKCD_RSA_WORKUNIT);
            r_pkcd_cmn_ram_clr(PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNIT);
            r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK1->full + PKCD_RSA_OFFSET_VAL(pq_length),
                                        p_key_bin->p_q, pq_length);
            r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK2->full + PKCD_RSA_OFFSET_VAL(pq_length),
                                        p_key_bin->p_p, pq_length);
            PKCD_CMN_ICUM_PKCCB0DPT = PKCD_RSA_FORMAT_DP_DPT(PKCD_RSA_PDT_1);
            ret = r_pkcd_cmn_modinv_fermat(PKCD_RSA_WORK1->full, PKCD_RSA_WORK2->full, PKCD_RSA_WORK3->full,
                                            PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNIT);
            r_pkcd_cmn_cpy_by_64bounds(p_key_bin->p_qinv, PKCD_RSA_WORK3->full +
                                            PKCD_RSA_OFFSET_VAL(pq_length), pq_length);
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
/***********************************************************************************************************************
* End of function r_pkcd_rsa_generate_crt()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_test_prime
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Test if given value is a prime number.
 * \param       [in,out]  p_prm         Parameters to be calculated
 * \param       [in] p_candidate        A candidate for a prime number. <br>
 * \param       [in] prime_len          The length of the candidate in bytes. <br>
 *                                          Its value must be within 64~256 and must be multiple of 32.
 * \param       [in] iteration_num      The number of iterations.
 * \param       [in] p_test_randoms    A random number used for the prime test. <br>
 *                                          Its size must be {prime_len} x {iteration_num}.
 * \return
 *  - #PKCD_RET_OK:                  The API succeeded.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 *  - #PKCD_RET_RSA_COMPOSITE:       Given number is a composite number.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Test if given value is a prime number in the following.
 *        - pre-prime Test
 *        - Miller-Rabin Test
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Clear #PKCD_CMN_PKCCRAM
 * -# Execute pre-prime Test with the #r_pkcd_rsa_pre_prime_test
 * -# If #PKCD_RET_OK == ret then
 *   -# Execute Miller-Rabin Test with the #r_pkcd_rsa_miller_rabin_test
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_test_prime(st_rsa_prm_test_prime_t* const p_prm,
                                        const uint8_t* const p_candidate, const uint32_t prime_len,
                                        const uint32_t iteration_num, uint8_t* const p_test_randoms[])
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* execute  pre-prime test */
    ret = r_pkcd_rsa_pre_prime_test(p_candidate, prime_len);

    if (PKCD_RET_OK == ret)
    {
        /* execute  Miller-Rabin test */
        ret = r_pkcd_rsa_miller_rabin_test(p_prm, p_candidate, prime_len, iteration_num, p_test_randoms);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_test_prime()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_gcd
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Calculate the GCD(greatest common divisor) of x and y
 * \param       [in]  p_x       An address of x.
 * \param       [in]  xlen      A byte size of x.
 * \param       [in]  p_y       An address of y.
 * \param       [in]  ylen      A byte size of y.
 * \param       [out] p_v       An address of the buffer that outputs the value of v after calculation.
 * \param       [out] p_D       An address of the buffer that outputs the value of D after calculation.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WORK4, <br>
 *    #PKCD_RSA_SPAREWORK1, #PKCD_RSA_SPAREWORK2, #PKCD_RSA_SPAREWORK3
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Calculate the GCD(greatest common divisor) of x and y.
 *      - The algorithm is described in chapter 14.61 of HANDBOOK of APPLIED CRYPTOGRAPHY.
 *      - Since it is necessary for the #r_pkcd_rsa_inverse, it outputs D together with GCD v.
 *      - The signs of A,B,C and D are managed separately from the value.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set the larger of xlen and ylen to len.
 * -# Add 8 bytes to len_u 32 in consideration of carry.
 * -# Set data lengt to DL bit of #PKCD_CMN_ICUM_PKCCB0OMR with the #r_pkcd_cmn_set_dl
 * -# Set 1 in PKCC format to p_pkccram_one.
 * -# Copy x in work area of u with the #r_pkcd_cmn_cpy_by_64bounds
 * -# Copy y in work area of v with the #r_pkcd_cmn_cpy_by_64bounds
 * -# Set 0 to B and C work area.
 * -# Set 1 to A and D work area.
 * -# Clear sign flag.
 * -# Set 0 to div2_cnt.
 * -# Loop while in the following case
 *   - The timeout counter has not exceeded the #PKCD_RSA_EX_GCD_LOOP_TIMEOUT
 *   - The x and y are even
 *   - There was no error in PKCC operation
 *   -# Calculate u = u/2 with the #r_pkcd_cmn_ADiv2
 *   -# Calculate v = v/2 with the #r_pkcd_cmn_ADiv2
 *   -# Increment div2_cnt
 *   -# Increment timeout count
 * -# If timeout count >= #PKCD_RSA_EX_GCD_LOOP_TIMEOUT then
 *   -# Set ret to #PKCD_RET_PKCCTIMEOUTERR
 * -# Set x and y value to xy.
 * -# Set u, A, and B to wks_u.
 * -# Set v, C, and D to wks_v.
 * -# Check if u is 0 and set the result to cmp_ret
 * -# Check if v is 0 and set the result to cmp_ret2
 * -# Loop while in the following case
 *   - The timeout counter has not exceeded the #PKCD_RSA_EX_GCD_LOOP_TIMEOUT
 *   - There was no error in PKCC operation
 *   - The cmp_ret and cmp_ret is not 0 (u and v is not 0)
 *   -# If u is an even number, processing is done with the #r_pkcd_rsa_ex_gcd_calc_even
 *   -# If v is an even number, processing is done with the #r_pkcd_rsa_ex_gcd_calc_even
 *   -# If u >= v then
 *     -# Calculate u = u - v with the #r_pkcd_cmn_AMinusB
 *     -# Calculate A = A - C with the #r_pkcd_rsa_ex_gcd_AMinusB
 *     -# Calculate B = B - D with the #r_pkcd_rsa_ex_gcd_AMinusB
 *   -# Else
 *     -# Calculate v = v - u with the #r_pkcd_cmn_AMinusB
 *     -# Calculate C = C - A with the #r_pkcd_rsa_ex_gcd_AMinusB
 *     -# Calculate D = D - B with the #r_pkcd_rsa_ex_gcd_AMinusB
 *   -# Check if u is 0 and set the result to cmp_ret
 *   -# Check if v is 0 and set the result to cmp_ret2
 *   -# Increment timeout counter
 * -# If timeout count >= #PKCD_RSA_EX_GCD_LOOP_TIMEOUT then
 *   -# Set ret to #PKCD_RET_PKCCTIMEOUTERR
 * -# If #PKCD_RET_OK == ret then
 *   -# Loop while in the following case
 *     - div2_cnt > 0
 *     -# Calculate v = v * 2 with the #r_pkcd_cmn_AB64
 *     -# Decrement div2_cnt
 *   -# Copy D to p_v
 *   -# Format D with the #r_pkcd_rsa_ex_gcd_format_D
 *   -# Copy D to p_D
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_rsa_gcd(const uint8_t* const p_x, const uint32_t xlen, const uint8_t* const p_y, const uint32_t ylen,
                            uint8_t* const p_v, uint8_t* const p_D)
{
    pkcd_ret_t ret = PKCD_RET_OK;
    st_rsa_ex_gcd_calc_even_wks_t wks_u;
    st_rsa_ex_gcd_calc_even_wks_t wks_v;
    st_rsa_ex_gcd_xy_t xy;
    const uint32_t len = (xlen > ylen)?xlen : ylen;
    const uint32_t len64 = len + PKCD_RSA_PKCC_CARRY_LEN; /* add 8 bytes to length to prevent carry generation */
    uint32_t div2_cnt;
    uint32_t temp;
    volatile uint32_t timeout_cnt;
    int32_t cmp_ret;
    int32_t cmp_ret2;
    uint32_t signA;
    uint32_t signB;
    uint32_t signC;
    uint32_t signD;
    uint32_t lsb_u;
    uint32_t lsb_v;
    uint8_t * const p_wk_u  = PKCD_RSA_WORK1->full;
    uint8_t * const p_wk_v  = PKCD_RSA_WORK2->full;
    uint8_t * const p_wk_A  = PKCD_RSA_WORK3->full;
    uint8_t * const p_wk_B  = PKCD_RSA_WORK4->full;
    uint8_t * const p_wk_C  = PKCD_RSA_SPAREWORK1->full;
    uint8_t * const p_wk_D  = PKCD_RSA_SPAREWORK2->full;
    uint8_t * const p_wk_xy = PKCD_RSA_SPAREWORK3->full;
    uint8_t * const p_div2_x  = p_v;   /* use p_v as temporary variable */
    uint8_t * const p_div2_y  = p_D;   /* use p_D as temporary variable */

    /* Defined uint32_t for 4-byte boundary */
    uint32_t  pkccram_one_array[PKCD_CMN_ARRAY32_SIZE(PKCD_CMN_PKCCBOUNDARYSIZE)];
    uint8_t *  const p_pkccram_one = (uint8_t*)pkccram_one_array; /* No problem with casting to pointer size */
    uint32_t  buf[PKCD_CMN_ARRAY32_SIZE(PKCD_RSA_WORKUNIT)];
    uint8_t * const p_buf = (uint8_t*)buf; /* No problem with casting to pointer size */

    /* set DL */
    r_pkcd_cmn_set_dl(len64, 0U);

    /* set pkccram one */
    r_pkcd_cmn_ram_clr(PKCD_RSA_WORK1->full, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK1->full + PKCD_RSA_OFFSET_VAL(sizeof(g_rsa_one)), g_rsa_one,
                                sizeof(g_rsa_one));
    r_pkcd_cmn_cpy_word(p_pkccram_one, PKCD_RSA_WORK1->full + PKCD_RSA_OFFSET_VAL(sizeof(g_rsa_one)),
                        sizeof(g_rsa_one));

    /* set u */
    r_pkcd_cmn_ram_clr(p_wk_u, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(p_wk_u + PKCD_RSA_OFFSET_VAL(xlen), p_x, xlen);

    /* set v */
    r_pkcd_cmn_ram_clr(p_wk_v, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(p_wk_v + PKCD_RSA_OFFSET_VAL(ylen), p_y, ylen);

    /* set B and C = 0 */
    r_pkcd_cmn_ram_clr(p_wk_B, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_ram_clr(p_wk_C, PKCD_RSA_WORKUNIT);

    /* set A and D = 1 */
    r_pkcd_cmn_ram_clr(p_wk_A, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_ram_clr(p_wk_D, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(p_wk_A + PKCD_RSA_OFFSET_VAL(sizeof(g_rsa_one)), g_rsa_one, sizeof(g_rsa_one));
    r_pkcd_cmn_cpy_by_64bounds(p_wk_D + PKCD_RSA_OFFSET_VAL(sizeof(g_rsa_one)), g_rsa_one, sizeof(g_rsa_one));

    /* set sign(0:positive, 1:negative) */
    signA = 0U;
    signB = 0U;
    signC = 0U;
    signD = 0U;

    /* clear work */
    r_pkcd_cmn_ram_clr(p_wk_xy, PKCD_RSA_WORKUNIT);

    /* While x and y are both even, do the following: x<-x/2, y<-y/2, g_cnt++ */
    div2_cnt = 0U;
    lsb_u = r_pkcd_cmn_get_lsb_work_unit(p_wk_u, PKCD_RSA_WORKUNIT);
    lsb_v = r_pkcd_cmn_get_lsb_work_unit(p_wk_v, PKCD_RSA_WORKUNIT);
    timeout_cnt = 0U;
    while ((PKCD_RSA_EX_GCD_LOOP_TIMEOUT > timeout_cnt) && (((lsb_u & 0x01U) == 0x00U) && ((lsb_v & 0x01U) == 0x00U)) &&
            (PKCD_RET_OK == ret))
    {
        /* Call WDT callback */
        r_pkcd_rsa_call_wdtclr();

        ret = r_pkcd_cmn_ADiv2(p_wk_u, p_wk_u, PKCD_RSA_WORKUNIT);
        ret |= r_pkcd_cmn_ADiv2(p_wk_v, p_wk_v, PKCD_RSA_WORKUNIT);
        div2_cnt++;

        lsb_u = r_pkcd_cmn_get_lsb_work_unit(p_wk_u, PKCD_RSA_WORKUNIT);
        lsb_v = r_pkcd_cmn_get_lsb_work_unit(p_wk_v, PKCD_RSA_WORKUNIT);
        timeout_cnt += 1U;
    }

    /* If loop is timeout, return error */
    if (timeout_cnt >= PKCD_RSA_EX_GCD_LOOP_TIMEOUT)
    {
        ret |= PKCD_RET_PKCCTIMEOUTERR;
    }
    else
    {
        /* Do nothing */
    }

    r_pkcd_cmn_ram_clr(p_div2_x, len);
    r_pkcd_cmn_ram_clr(p_div2_y, len);
    r_pkcd_cmn_cpy_by_64bounds(p_div2_x, p_wk_u + PKCD_RSA_OFFSET_VAL(len), len);
    r_pkcd_cmn_cpy_by_64bounds(p_div2_y, p_wk_v + PKCD_RSA_OFFSET_VAL(len), len);

    /* set structure */
    xy.p_x = p_div2_x;
    xy.xlen = len;
    xy.p_y = p_div2_y;
    xy.ylen = len;

    wks_u.p_wk_uv  = p_wk_u;
    wks_u.p_wk_AC  = p_wk_A;
    wks_u.p_signAC = &signA;
    wks_u.p_wk_BD  = p_wk_B;
    wks_u.p_signBD = &signB;
    wks_u.p_wk_xy  = p_wk_xy;

    wks_v.p_wk_uv  = p_wk_v;
    wks_v.p_wk_AC  = p_wk_C;
    wks_v.p_signAC = &signC;
    wks_v.p_wk_BD  = p_wk_D;
    wks_v.p_signBD = &signD;
    wks_v.p_wk_xy  = p_wk_xy;

    timeout_cnt = 0U;
    cmp_ret = r_pkcd_cmn_cmp_zero_word(p_wk_u + PKCD_RSA_OFFSET_VAL(len64), len64);
    cmp_ret2 = r_pkcd_cmn_cmp_zero_word(p_wk_u + PKCD_RSA_OFFSET_VAL(len64), len64);
    while ((PKCD_RSA_EX_GCD_LOOP_TIMEOUT > timeout_cnt) && (PKCD_RET_OK == ret) &&
            (0 != cmp_ret) && (0 != cmp_ret2))
    {
        /* Call WDT callback */
        r_pkcd_rsa_call_wdtclr();

        /* While u is even do the following */
        ret = r_pkcd_rsa_ex_gcd_calc_even(&wks_u, &xy, len64, p_buf);

        /* While v is even do the following */
        ret |= r_pkcd_rsa_ex_gcd_calc_even(&wks_v, &xy, len64, p_buf);

        /* is u >= v ?(wk_xy use as a temp work) */
        ret |= r_pkcd_cmn_cmp_work_unit(p_wk_u, p_wk_v, p_wk_xy, len64, PKCD_RSA_WORKUNIT, &cmp_ret);
        if (cmp_ret >= 0)
        {
            /* u = u - v */
            ret |= r_pkcd_cmn_AMinusB(p_wk_u, p_wk_v, p_wk_u, PKCD_RSA_WORKUNIT);

            /* A = A - C */
            ret |= r_pkcd_rsa_ex_gcd_AMinusB(p_wk_A, &signA, p_wk_C, signC, len64, p_wk_xy);

            /* B = B - D */
            ret |= r_pkcd_rsa_ex_gcd_AMinusB(p_wk_B, &signB, p_wk_D, signD, len64, p_wk_xy);
        }
        else
        {
            /* v = v - u */
            ret |= r_pkcd_cmn_AMinusB(p_wk_v, p_wk_u, p_wk_v, PKCD_RSA_WORKUNIT);

            /* C = C - A */
            ret |= r_pkcd_rsa_ex_gcd_AMinusB(p_wk_C, &signC, p_wk_A, signA, len64, p_wk_xy);

            /* D = D - B */
            ret |= r_pkcd_rsa_ex_gcd_AMinusB(p_wk_D, &signD, p_wk_B, signB, len64, p_wk_xy);
        }

        cmp_ret = r_pkcd_cmn_cmp_zero_word(p_wk_u + PKCD_RSA_OFFSET_VAL(len64), len64);
        cmp_ret2 = r_pkcd_cmn_cmp_zero_word(p_wk_v + PKCD_RSA_OFFSET_VAL(len64), len64);

        /* timeout count increment */
        timeout_cnt += 1U;
    }

    /* If loop is timeout, return error */
    if (timeout_cnt >= PKCD_RSA_EX_GCD_LOOP_TIMEOUT)
    {
        ret |= PKCD_RET_PKCCTIMEOUTERR;
    }
    else
    {
        /* Do nothing */
    }

    if (PKCD_RET_OK == ret)
    {
        temp = r_pkcd_cmn_ch_endian32(PKCD_CMN_DIV_2);
        r_pkcd_cmn_ram_clr(p_wk_u, PKCD_RSA_WORKUNIT);

        /* use u s temporary variable */
        r_pkcd_cmn_cpy_by_64bounds(p_wk_u + PKCD_RSA_OFFSET_VAL(sizeof(uint32_t)), (uint8_t*)&temp,
                                    sizeof(uint32_t));
        while (div2_cnt > 0U)
        {
            /* Call WDT callback */
            r_pkcd_rsa_call_wdtclr();

            ret |= r_pkcd_cmn_AB64(p_wk_u, p_wk_v, p_wk_v, PKCD_RSA_WORKUNIT);
            div2_cnt--;
        }
        r_pkcd_cmn_cpy_by_64bounds(p_v, p_wk_v + PKCD_RSA_OFFSET_VAL(len), len);

        /* Format D */
        r_pkcd_cmn_ram_clr(p_wk_xy, PKCD_RSA_WORKUNIT);
        r_pkcd_cmn_cpy_by_64bounds(p_wk_xy + PKCD_RSA_OFFSET_VAL(xlen), p_x, xlen);
        ret = r_pkcd_rsa_ex_gcd_format_D(p_wk_D, signD, p_wk_xy, p_wk_A, len64);

        /* copy D to out buffer */
        r_pkcd_cmn_cpy_by_64bounds(p_D, p_wk_D + PKCD_RSA_OFFSET_VAL(len), len);
    }
    else
    {
        /* Do nothing */
    }

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;

}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_gcd()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_inverse
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Calculate the modular inversion acording to Binary extended gcd algorithm
 * \param       [in]  p_a       An address of operand.
 * \param       [in]  alen      A byte size of operand.
 * \param       [in]  p_m       An address of modulo.
 * \param       [in]  mlen      A byte size of m.
 * \param       [out] p_ainv    An address of the buffer that outputs the inverse of a after calculation.
 * \param       [in]  p_buf     An address of the temporary buffer.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 *  - #PKCD_RET_RSA_NON_COPRIME:     a and m are not co-prime
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Calculate the modular inversion acording to Binary extended gcd altorithm.
 *      - The algorithm is described in chapter 14.61 of HANDBOOK of APPLIED CRYPTOGRAPHY.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Calculate GCD v and ainv with the #r_pkcd_rsa_gcd
 * -# Check if v is 1.
 * -# If v is not 1.
 *   -# Set ret to #PKCD_RET_RSA_NON_COPRIME
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_rsa_inverse(const uint8_t* const p_a, const uint32_t alen, const uint8_t* const p_m,
                                const uint32_t mlen, uint8_t* const p_ainv, uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    int32_t cmp_ret;
    int32_t cmp_ret2;
    uint8_t * const p_v = p_buf;

    r_pkcd_cmn_ram_clr(p_v, mlen);
    ret = r_pkcd_rsa_gcd(p_m, mlen, p_a, alen, p_v, p_ainv);

    cmp_ret = r_pkcd_cmn_cmp_zero_word(p_v, mlen - PKCD_CMN_PKCCBOUNDARYSIZE);
    cmp_ret2 = r_pkcd_cmn_cmp_word(p_v + (mlen - (sizeof(g_rsa_one))), g_rsa_one, sizeof(g_rsa_one));
    if ((0 != cmp_ret) || (0 != cmp_ret2))
    {
        ret = PKCD_RET_RSA_NON_COPRIME;
    }
    else
    {
        /* Do nothing */
    }

    return ret;

}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_inverse()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_calc_m
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Calculate m1, m2 to be used in CRT mode
 * \param       [in] p_msg          An address of message
 * \param       [in] msg_len        A byte size of message.
 * \param       [in] p_pq           An address of p or q.
 * \param       [in] p_dpq          An address of dp or dq.
 * \param       [in] pq_len         A byte size of prime.
 * \param       [in] sync_option    Indicate if the function returns while PKCC is running.
 * \param       [in] is_mod_p       A flag indicating that the calculation target is mod p.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_PDT_1, #PKCD_RSA_PDT_2, #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WORK4, <br>
 *    #PKCD_RSA_SPAREWORK1, #PKCD_RSA_WWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPW
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Calculate m1, m2 to be used in CRT mode.
 *      - The m1, m2 is calculated according to the following calculation formula. (C is msg)
 *        - m1 = (C mod p) ^ dp mod p
 *        - m2 = (C mod q) ^ dq mod q
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If is_mod_p == 1 then
 *   -# Set #PKCD_RSA_WORK1->half.h to p_wk_pq.
 *   -# Set #PKCD_RSA_WORK2->half.h to p_wk_r2mod.
 *   -# Set #PKCD_RSA_SPAREWORK1->half.h to p_wk_cmodpq.
 *   -# Set #PKCD_RSA_WORK3->half.h to p_wk_dpq.
 *   -# Set #PKCD_RSA_WORK4->half.h to p_wk_m.
 *   -# Set #PKCD_RSA_PDT_1 to p_dpt.
 * -# Else
 *   -# Set #PKCD_RSA_WORK1->half.l to p_wk_pq.
 *   -# Set #PKCD_RSA_WORK2->half.l to p_wk_r2mod.
 *   -# Set #PKCD_RSA_SPAREWORK1->half.l to p_wk_cmodpq.
 *   -# Set #PKCD_RSA_WORK3->half.l to p_wk_dpq.
 *   -# Set #PKCD_RSA_WORK4->half.l to p_wk_m.
 *   -# Set #PKCD_RSA_PDT_2 to p_dpt.
 * -# Set #PKCD_RSA_WWORK to #PKCD_CMN_ICUM_PKCCB0DPW.
 * -# Set p_dpt to #PKCD_CMN_ICUM_PKCCB0DPT.
 * -# Set pq_len to DL and BDL bit of #PKCD_CMN_ICUM_PKCCB0OMR with the #r_pkcd_cmn_set_dl
 * -# Calculate T of p_wk_pq with the #r_pkcd_cmn_MinusCInv
 * -# Calculate R^2 mod p_wk_pq with the #r_pkcd_cmn_R2ModC
 * -# Calculate C mod p_wk_pq with the #r_pkcd_cmn_reduce_small_modulus
 * -# Calculate m = (C mod p_wk_pq)^p_wk_dpq mod p_wk_pq with the #r_pkcd_cmn_AExpBModCCon
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_calc_m(const uint8_t* const p_msg, const uint32_t msg_len, const uint8_t* const p_pq,
                                    const uint8_t* const p_dpq, const uint32_t pq_len,
                                    const e_pkc_sync_opt_t sync_option, const e_pkc_bool_t is_mod_p)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint8_t * p_wk_pq;
    const uint8_t * p_wk_r2mod;
    uint8_t * p_wk_cmodpq;
    uint8_t * p_wk_dpq;
    const uint8_t * p_wk_m;
    const uint8_t * p_dpt;

    /* set work */
    if (PKC_BOOL_TRUE == is_mod_p)
    {
        p_wk_pq     = PKCD_RSA_WORK1->half.h;
        p_wk_r2mod  = PKCD_RSA_WORK2->half.h;
        p_wk_cmodpq = PKCD_RSA_SPAREWORK1->half.h;
        p_wk_dpq    = PKCD_RSA_WORK3->half.h;
        p_wk_m      = PKCD_RSA_WORK4->half.h;
        p_dpt       = PKCD_RSA_PDT_1;
    }
    else
    {
        p_wk_pq     = PKCD_RSA_WORK1->half.l;
        p_wk_r2mod  = PKCD_RSA_WORK2->half.l;
        p_wk_cmodpq = PKCD_RSA_SPAREWORK1->half.l;
        p_wk_dpq    = PKCD_RSA_WORK3->half.l;
        p_wk_m      = PKCD_RSA_WORK4->half.l;
        p_dpt       = PKCD_RSA_PDT_2;
    }

    /* prepare operation */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_RSA_FORMAT_DP_DPT(p_dpt);
    PKCD_CMN_ICUM_PKCCB0DPW = PKCD_RSA_FORMAT_DP_DPW(PKCD_RSA_WWORK);
    r_pkcd_cmn_set_dl(pq_len, pq_len);

    /* Prepare C mod p (or q) */
    r_pkcd_cmn_cpy_by_64bounds(p_wk_pq + PKCD_RSA_OFFSET_VALHALF(pq_len), p_pq, pq_len);
    ret = r_pkcd_cmn_MinusCInv(p_wk_pq,
                                (uint8_t*)PKCD_RSA_FORMAT_DP_DPT(p_dpt), /* No problem with casting to pointer size */
                                PKCD_RSA_WORKUNITHALF);
    ret |= r_pkcd_cmn_R2ModC(p_wk_pq, p_wk_r2mod, PKCD_RSA_WORKUNITHALF);
    ret |= r_pkcd_cmn_reduce_small_modulus(p_msg, p_wk_pq, p_wk_r2mod, msg_len, pq_len, PKCD_RSA_WORKUNITHALF,
                                            PKCD_RSA_WORK3->half.h, PKCD_RSA_WORK3->half.l, p_wk_cmodpq);

    /* Calc m1 = (C mod p)^dp mod p or m2 = (C mod q)^dq mod q */
    r_pkcd_cmn_cpy_by_64bounds(p_wk_dpq + PKCD_RSA_OFFSET_VALHALF(pq_len), p_dpq, pq_len);
    ret |= r_pkcd_cmn_AExpBModCCon(p_wk_cmodpq, p_wk_dpq, p_wk_pq, p_wk_m, PKCD_RSA_WORKUNITHALF, sync_option);

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_calc_m()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_calc_m_sec
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Calculate m1, m2 to be used in CRT mode
 * \param       [in] p_msg          An address of message
 * \param       [in] msg_len        A byte size of message.
 * \param       [in] p_pq           An address of p or q.
 * \param       [in] p_dpq          An address of dp or dq.
 * \param       [in] pq_len         A byte size of prime.
 * \param       [in] p_random       A random number used to randomize the exponent.
 * \param       [in] sync_option    Indicate if the function returns while PKCC is running.
 * \param       [in] is_mod_p       A flag indicating that the calculation target is mod p.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_PDT_1, #PKCD_RSA_PDT_2, #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WORK4, <br>
 *    #PKCD_RSA_SPAREWORK1, #PKCD_RSA_WWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPW
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Calculate m1, m2 to be used in CRT mode.
 *      - The m1, m2 is calculated as follows. (C is msg. r_p and r_q are 8-byte random values.)
 *        - m1 = (C mod p) ^ (dp + r_p * (p - 1)) mod p
 *        - m2 = (C mod q) ^ (dq + r_q * (q - 1)) mod q
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If is_mod_p == 1 then
 *   -# Set #PKCD_RSA_WORK1->half.h to p_wk_pq.
 *   -# Set #PKCD_RSA_WORK2->half.h to p_wk_r2mod.
 *   -# Set #PKCD_RSA_SPAREWORK1->half.h to p_wk_cmodpq.
 *   -# Set #PKCD_RSA_WORK3->half.h to p_wk_dpq.
 *   -# Set #PKCD_RSA_WORK4->half.h to p_wk_m.
 *   -# Set #PKCD_RSA_PDT_1 to p_dpt.
 * -# Else
 *   -# Set #PKCD_RSA_WORK1->half.l to p_wk_pq.
 *   -# Set #PKCD_RSA_WORK2->half.l to p_wk_r2mod.
 *   -# Set #PKCD_RSA_SPAREWORK1->half.l to p_wk_cmodpq.
 *   -# Set #PKCD_RSA_WORK3->half.l to p_wk_dpq.
 *   -# Set #PKCD_RSA_WORK4->half.l to p_wk_m.
 *   -# Set #PKCD_RSA_PDT_2 to p_dpt.
 * -# Set #PKCD_RSA_WWORK to #PKCD_CMN_ICUM_PKCCB0DPW.
 * -# Set p_dpt to #PKCD_CMN_ICUM_PKCCB0DPT.
 * -# Set DL and BDL bit of #PKCD_CMN_ICUM_PKCCB0OMR to pq_len with the #r_pkcd_cmn_set_dl.
 * -# Calculate p_wk_pq - 1 with #r_pkcd_cmn_AMinusB.
 * -# Calculate random * (p_wk_pq - 1) with #r_pkcd_cmn_AB64.
 * -# Set DL bit of #PKCD_CMN_ICUM_PKCCB0OMR to pq_len + #PKCD_CMN_BITS64.
 * -# Calculate random * (p_wk_pq - 1) + p_dpq with #r_pkcd_cmn_APlusB.
 * -# Set p_wk_exp to random * (p_wk_pq - 1) + p_dpq.
 * -# Set DL bit of #PKCD_CMN_ICUM_PKCCB0OMR to pq_len.
 * -# Calculate T of p_wk_pq with the #r_pkcd_cmn_MinusCInv.
 * -# Calculate R^2 mod p_wk_pq with the #r_pkcd_cmn_R2ModC.
 * -# Calculate C mod p_wk_pq with the #r_pkcd_cmn_reduce_small_modulus.
 * -# Set BDL bit of #PKCD_CMN_ICUM_PKCCB0OMR to pq_len + 2 * #PKCD_CMN_BITS64 with the #r_pkcd_cmn_set_dl.
 * -# Calculate m = (C mod p_wk_pq)^p_wk_exp mod p_wk_pq with the #r_pkcd_cmn_AExpBModCCon
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_calc_m_sec(const uint8_t* const p_msg, const uint32_t msg_len, const uint8_t* const p_pq,
                                    const uint8_t* const p_dpq, const uint32_t pq_len, const uint8_t* const p_random,
                                    const e_pkc_sync_opt_t sync_option, const e_pkc_bool_t is_mod_p)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint8_t * p_wk_pq;
    uint8_t * p_wk_r2mod;
    uint8_t * p_wk_cmodpq;
    const uint8_t * p_wk_exp;
    uint8_t * p_wk_m;
    const uint8_t * p_dpt;

    /* set work */
    if (PKC_BOOL_TRUE == is_mod_p)
    {
        p_wk_pq     = PKCD_RSA_WORK1->half.h;
        p_wk_r2mod  = PKCD_RSA_WORK2->half.h;
        p_wk_cmodpq = PKCD_RSA_SPAREWORK1->half.h;
        p_wk_exp    = PKCD_RSA_WORK3->half.h;
        p_wk_m      = PKCD_RSA_WORK4->half.h;
        p_dpt       = PKCD_RSA_PDT_1;
    }
    else
    {
        p_wk_pq     = PKCD_RSA_WORK1->half.l;
        p_wk_r2mod  = PKCD_RSA_WORK2->half.l;
        p_wk_cmodpq = PKCD_RSA_SPAREWORK1->half.l;
        p_wk_exp    = PKCD_RSA_WORK3->half.l;
        p_wk_m      = PKCD_RSA_WORK4->half.l;
        p_dpt       = PKCD_RSA_PDT_2;
    }

    /* prepare operation */
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_RSA_FORMAT_DP_DPT(p_dpt);
    PKCD_CMN_ICUM_PKCCB0DPW = PKCD_RSA_FORMAT_DP_DPW(PKCD_RSA_WWORK);

    /* Calculate p-1 (or q-1) */
    /* Use p_wk_r2mod as a buffer for 0x00000001, and use p_wk_m as a buffer for p-1 (or q-1) */
    r_pkcd_cmn_set_dl(pq_len, pq_len);
    r_pkcd_cmn_cpy_by_64bounds(p_wk_pq + PKCD_RSA_OFFSET_VALHALF(pq_len), p_pq, pq_len);
    r_pkcd_cmn_cpy_by_64bounds(p_wk_r2mod + PKCD_RSA_OFFSET_VALHALF(sizeof(g_rsa_one)), g_rsa_one, sizeof(g_rsa_one));
    ret = r_pkcd_cmn_AMinusB(p_wk_pq, p_wk_r2mod, p_wk_m, PKCD_RSA_WORKUNITHALF);

    /* Clear PKCD_RSA_WORK3 */
    r_pkcd_cmn_ram_clr(PKCD_RSA_WORK3->full, PKCD_RSA_WORKUNIT);

    /* Calculate random*(p-1) (or random*(q-1)) */
    /* Use p_wk_r2mod as a buffer for random number */
    r_pkcd_cmn_cpy_by_64bounds(p_wk_r2mod + PKCD_RSA_OFFSET_VALHALF(PKCD_CMN_BITS64), p_random, PKCD_CMN_BITS64);
    ret |= r_pkcd_cmn_AB64(p_wk_r2mod, p_wk_m, PKCD_RSA_WORK3->half.h, PKCD_RSA_WORKUNITHALF);

    /* Calculate random*(p-1) + dp or random*(q-1) + dq  */
    r_pkcd_cmn_set_dl(pq_len + PKCD_CMN_BITS64, pq_len);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK3->half.l + PKCD_RSA_OFFSET_VALHALF(pq_len), p_dpq, pq_len);
    ret |= r_pkcd_cmn_APlusB(PKCD_RSA_WORK3->half.h, PKCD_RSA_WORK3->half.l, p_wk_exp, PKCD_RSA_WORKUNITHALF);

    /* Prepare C mod p (or q) */
    r_pkcd_cmn_set_dl(pq_len, pq_len);
    ret |= r_pkcd_cmn_MinusCInv(p_wk_pq,
                                (uint8_t*)PKCD_RSA_FORMAT_DP_DPT(p_dpt),/* No problem with casting to pointer size */
                                PKCD_RSA_WORKUNITHALF);
    ret |= r_pkcd_cmn_R2ModC(p_wk_pq, p_wk_r2mod, PKCD_RSA_WORKUNITHALF);
    ret |= r_pkcd_cmn_reduce_small_modulus(p_msg, p_wk_pq, p_wk_r2mod, msg_len, pq_len, PKCD_RSA_WORKUNITHALF,
                                            p_wk_cmodpq, p_wk_m, p_wk_cmodpq);

    /* Calc m1 = (C mod p)^dp mod p or m2 = (C mod q)^dq mod q */
    r_pkcd_cmn_set_dl(pq_len, pq_len + (PKCD_CMN_MUL_2 * PKCD_CMN_BITS64));
    ret |= r_pkcd_cmn_AExpBModCCon(p_wk_cmodpq, p_wk_exp, p_wk_pq, p_wk_m, PKCD_RSA_WORKUNITHALF, sync_option);

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_calc_m_sec()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_pre_prime_test
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Perform a pre-prime test
 * \param       [in]  p_d               An address of candidate for a prime number d.
 * \param       [in]  dlen              A byte size of a prime number.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 *  - #PKCD_RET_RSA_COMPOSITE:       Given number is a composite number.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_PDT_1, #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPW
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Try to divide with 16bit small primes
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set dlen to DL bit of #PKCD_CMN_ICUM_PKCCB0OMR with the #r_pkcd_cmn_set_dl
 * -# Set WORK to ICUM_PKCCB0DPX
 * -# Set Prime number d to #PKCD_RSA_WORK1
 * -# Get the least significant byte of d.
 * -# If LSB is not 0 then
 *   -# Loop while i is #PKCD_RSA_PRIME_TBL_SIZE or less increment i for each loop
 *     -# Copy divisor in #g_rsa_prime_table to WORK area.
 *     -# Copy Montgomery Reduction in #g_rsa_mont_reduction_table to WORK area.
 *     -# Calculate dR^(-1) mod divisor with mode #PKCD_CMN_MODE_ARINVMODC of #r_pkcd_cmn_run.
 *     -# Check the remainder and set ret to #PKCD_RET_RSA_COMPOSITE if it is 0
 *     -# If #PKCD_RET_OK != ret then, break.
 * -# Else
 *   -# Set ret to #PKCD_RET_RSA_COMPOSITE
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_pre_prime_test(const uint8_t* const p_d, const uint32_t dlen)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t i;
    uint32_t tmp;
    uint32_t lsb;

    /* set DL */
    r_pkcd_cmn_set_dl(dlen, 0U);

    /* set DPT and work areas */
    r_pkcd_cmn_ram_clr(PKCD_RSA_PDT_1, PKCD_RSA_WORKUNITPDT);
    r_pkcd_cmn_ram_clr(PKCD_RSA_WORK1->full, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_ram_clr(PKCD_RSA_WORK2->full, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_ram_clr(PKCD_RSA_WORK3->full, PKCD_RSA_WORKUNIT);
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_RSA_FORMAT_DP_DPT(PKCD_RSA_PDT_1);
    PKCD_CMN_ICUM_PKCCB0DPW = PKCD_RSA_FORMAT_DP_DPW(PKCD_RSA_WWORK);
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_RSA_FORMAT_DP_UNIT(PKCD_RSA_WORK1->full);
    PKCD_CMN_ICUM_PKCCB0DPC = PKCD_RSA_FORMAT_DP_UNIT(PKCD_RSA_WORK2->full);
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_RSA_FORMAT_DP_UNIT(PKCD_RSA_WORK3->full);

    /* set prime data */
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK1->full + PKCD_RSA_OFFSET_VAL(dlen), p_d, dlen);

    lsb = r_pkcd_cmn_get_lsb_work_unit(PKCD_RSA_WORK1->full, PKCD_RSA_WORKUNIT);
    if ((lsb & 0x01U) != 0x00U)
    {
        for (i = 0U; i < PKCD_RSA_PRIME_TBL_SIZE; i++ )
        {
            /* Call WDT callback */
            r_pkcd_rsa_call_wdtclr();

            /* set divisor */
            tmp = r_pkcd_cmn_ch_endian32(g_rsa_prime_table[i]);
            r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK2->full + PKCD_RSA_OFFSET_VAL(sizeof(uint32_t)),
                                        (uint8_t*)&tmp, sizeof(uint32_t)); /* No problem with casting pointer size */

            /* set montgomery_T of Prime C */
            tmp = r_pkcd_cmn_ch_endian32(g_rsa_mont_reduction_table[i].h);
            r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_PDT_1, (uint8_t*)&tmp, sizeof(uint32_t)); /* No problem with casting to
                                                                                             pointer size */
            tmp = r_pkcd_cmn_ch_endian32(g_rsa_mont_reduction_table[i].l);
            r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_PDT_1 + sizeof(uint32_t),
                                        (uint8_t*)&tmp, sizeof(uint32_t)); /* Same type of cast as above */

            /* calc */
            ret = r_pkcd_cmn_run(PKCD_CMN_MODE_ARINVMODC, PKC_SYNC_OPT_SYNC);

            /* check remainder */
            r_pkcd_cmn_cpy_by_64bounds((uint8_t*)&tmp, PKCD_RSA_WORK3->full + PKCD_RSA_OFFSET_VAL(sizeof(uint32_t)),
                                        sizeof(uint32_t));
            if (0x00000000U == tmp)
            {
                /* d is composit */
                if (PKCD_RET_OK == ret)
                {
                    ret = PKCD_RET_RSA_COMPOSITE; /* not prime */
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

            if (PKCD_RET_OK != ret)
            {
                break;
            }
            else
            {
                /* Do nothing */
            }
        } /* for (i = 0; i < PKCD_RSA_PRIME_TBL_SIZE; i++ ) */
    }
    else
    {
        ret = PKCD_RET_RSA_COMPOSITE; /* not prime */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_pre_prime_test()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_miller_rabin_test
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Perform the Miller-Rabin test
 * \param       [in,out]  p_prm             Parameters to be calculated
 * \param       [in]      p_n               An address of candidate for a prime number N.
 * \param       [in]      nlen              A byte size of a prime number.
 * \param       [in]      iteration_num     The number of iterations.
 * \param       [in]      p_test_randoms    A random number used for the prime test. <br>
 *                                          Its size must be {prime_len} x {iteration_num}.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 *  - #PKCD_RET_RSA_COMPOSITE:       Given number is a composite number.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_PDT_1, #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WORK4, <br>
 *    #PKCD_RSA_SPAREWORK1, #PKCD_RSA_SPAREWORK2, #PKCD_RSA_WWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPW
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Test if given value is a prime number using the Miller-Rabin algorithm.
 *      - See the #R_PKCD_RsaTestPrime function for the validity of iteration number for each bit number.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Set pq_len to DL and BDL bit of #PKCD_CMN_ICUM_PKCCB0OMR with the #r_pkcd_cmn_set_dl
 * -# Set #PKCD_RSA_WWORK to #PKCD_CMN_ICUM_PKCCB0DPW
 * -# Copy N to wk_n_minus1 of keygenerate buffer
 * -# Calculate the least significant byte of wk_n_minus1 AND 0xFE, so make N-1.
 * -# Copy n to wk_n_minus1 of keygenerate buffer.
 * -# Convert wk_n_minus1 to PKCC format and copy it to wk_n_minus1.
 * -# Get the least significant byte of N - 1.
 * -# Loop while in the following case
 *   - The timeout counter has not exceeded the #PKCD_RSA_MILLER_RABIN_DIV2_LOOP_TIMEOUT
 *   - The lsb is even
 *   - There was no error in PKCC operation
 *   -# Increment exp2_cnt.
 *   -# Calculate (n-1) = (n-1) / 2 with the #r_pkcd_cmn_ADiv2.
 *   -# Get the least significant byte of N - 1.
 *   -# Increment timeout count.
 * -# If timeout count >= #PKCD_RSA_MILLER_RABIN_DIV2_LOOP_TIMEOUT then
 *   -# Set ret to #PKCD_RET_PKCCTIMEOUTERR
 * -# Else
 *   -# Loop while i is iteration_num or less increment i for each loop
 *     -# Set p_test_randoms[i] to p_A.
 *     -# Clear most significant 4-bits so that A < N
 *     -# Set #PKCD_RSA_PDT_1 to #PKCD_CMN_ICUM_PKCCB0DPT.
 *     -# Calculate A^d(mod N) with the #r_pkcd_cmn_AExpBModCCon
 *     -# If A^d(mod N) is equal 1, continue
 *     -# if A^d(mod N) is equal n - 1, continue
 *     -# Loop while j is exp2_cnt or less increment i for each loop
 *       -# Calculate square (A^(d*2^x)) with the #r_pkcd_cmn_AExpBModCCon
 *       -# If A^(d*2^x) is equal n - 1, break
 *     -# If j == exp2_cnt then
 *       -# If #PKCD_RET_OK == ret then
 *         -# Set ret to #PKCD_RET_RSA_COMPOSITE
 *     -# If #PKCD_RET_OK != ret, break
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_miller_rabin_test(st_rsa_prm_test_prime_t* const p_prm,
                                                const uint8_t* const p_n, const uint32_t nlen,
                                                const uint32_t iteration_num, uint8_t* const p_test_randoms[])
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t tmp32;
    uint32_t i;
    uint32_t j;
    uint32_t exp2_cnt;
    volatile uint32_t timeout_cnt;
    int32_t  cmp_ret;
    uint8_t * p_A;
    uint32_t lsb;

    uint8_t * const p_wk_n_minus1 = p_prm->wk_n_minus1;
    uint8_t * const p_wk_fact2_in_n_minus1 = p_prm->wk_fact2_in_n_minus1;

    /* set DL */
    r_pkcd_cmn_set_dl(nlen, nlen);

    /* set WORK */
    PKCD_CMN_ICUM_PKCCB0DPW = PKCD_RSA_FORMAT_DP_DPW(PKCD_RSA_WWORK);

    /* Copy n - 1 to buf */
    r_pkcd_cmn_ram_clr(p_wk_n_minus1, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_ram_clr(PKCD_RSA_WORK2->full, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_cpy_word(p_wk_n_minus1, p_n, nlen);
    p_wk_n_minus1[nlen - 1U] =
    /* No problem for casting to support usual arithmetic conversion */
    (uint8_t)((uint32_t)p_wk_n_minus1[nlen - 1U] & (uint32_t)PKCD_RSA_MASK_FE);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK2->full + PKCD_RSA_OFFSET_VAL(nlen), p_wk_n_minus1, nlen);
    r_pkcd_cmn_cpy_word(p_wk_n_minus1, PKCD_RSA_WORK2->full, PKCD_RSA_WORKUNIT);

    /* find the number of factor "2" in "N - 1" ( N-1 = 2^r * d ) */
    exp2_cnt = 0U;
    timeout_cnt = 0U;
    lsb = r_pkcd_cmn_get_lsb_work_unit(PKCD_RSA_WORK2->full, PKCD_RSA_WORKUNIT);
    while ((PKCD_RSA_MILLER_RABIN_DIV2_LOOP_TIMEOUT > timeout_cnt) && ((lsb & 0x01U) == 0x00U))
    {
        /* Call WDT callback */
        r_pkcd_rsa_call_wdtclr();

        /* while even */
        exp2_cnt += 1U;
        ret = r_pkcd_cmn_ADiv2(PKCD_RSA_WORK2->full, PKCD_RSA_WORK2->full, PKCD_RSA_WORKUNIT);
        lsb = r_pkcd_cmn_get_lsb_work_unit(PKCD_RSA_WORK2->full, PKCD_RSA_WORKUNIT);
        timeout_cnt += 1U;

        if (PKCD_RET_OK == ret)
        {
            /* Do nothing */
        }
        else
        {
            break;
        }
    }

    /* If loop is timeout, return error */
    if (timeout_cnt >= PKCD_RSA_MILLER_RABIN_DIV2_LOOP_TIMEOUT)
    {
        ret |= PKCD_RET_PKCCTIMEOUTERR;
    }
    else
    {
        /* Copy factor "2" in "N - 1" to buf */
        r_pkcd_cmn_ram_clr(p_wk_fact2_in_n_minus1, PKCD_RSA_WORKUNIT);
        r_pkcd_cmn_cpy_word(p_wk_fact2_in_n_minus1, PKCD_RSA_WORK2->full, PKCD_RSA_WORKUNIT);

        for (i = 0U; i < iteration_num; i++ )
        {
            /* Call WDT callback */
            r_pkcd_rsa_call_wdtclr();

            /* set A */
            p_A = p_test_randoms[i];

            /* change MSByte so that "A < N" is true */
            p_A[0] = (uint8_t)((uint32_t)p_A[0] & (uint32_t)PKCD_RSA_MASK_0F);

            /* calculate A^d(mod N) */
            r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK1->full + PKCD_RSA_OFFSET_VAL(nlen), p_A, nlen);
            r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK3->full + PKCD_RSA_OFFSET_VAL(nlen), p_n, nlen);
            r_pkcd_cmn_cpy_word(PKCD_RSA_WORK2->full, p_wk_fact2_in_n_minus1, PKCD_RSA_WORKUNIT);

            r_pkcd_cmn_ram_clr(PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNIT);
            PKCD_CMN_ICUM_PKCCB0DPT = PKCD_RSA_FORMAT_DP_DPT(PKCD_RSA_PDT_1);
            ret |= r_pkcd_cmn_AExpBModCCon(PKCD_RSA_WORK1->full, PKCD_RSA_WORK2->full, PKCD_RSA_WORK3->full,
                                            PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNIT, PKC_SYNC_OPT_SYNC);

            /* check if A^d(mod N) is 0x000000......0001 */
            r_pkcd_cmn_ram_clr(PKCD_RSA_SPAREWORK1->full, PKCD_RSA_WORKUNIT);
            r_pkcd_cmn_ram_clr(PKCD_RSA_SPAREWORK2->full, PKCD_RSA_WORKUNIT);
            tmp32 = r_pkcd_cmn_ch_endian32(0x00000001U);
            r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_SPAREWORK1->full + PKCD_RSA_OFFSET_VAL(sizeof(uint32_t)),
                                        (uint8_t*)&tmp32, sizeof(uint32_t)); /* No problem with casting to
                                                                                pointer size */
            ret |= r_pkcd_cmn_AMinusB(PKCD_RSA_WORK4->full, PKCD_RSA_SPAREWORK1->full, PKCD_RSA_SPAREWORK2->full,
                                        PKCD_RSA_WORKUNIT);
            cmp_ret = r_pkcd_cmn_cmp_zero_word(PKCD_RSA_SPAREWORK2->full + PKCD_RSA_OFFSET_VAL(nlen), nlen);
            if (0 == cmp_ret)
            {
                continue;   /* A^(d*2^x) != -1(mod N) = N-1. A is strong liar!(goto next loop) */
            }
            else
            {
                /* Do nothing */
            }

            /* check if A^d(mod N) is n - 1 */
            r_pkcd_cmn_ram_clr((uint8_t*)PKCD_RSA_SPAREWORK1->full, PKCD_RSA_WORKUNIT); /* No problem with casting to
                                                                                            pointer size */
            r_pkcd_cmn_ram_clr((uint8_t*)PKCD_RSA_SPAREWORK2->full, PKCD_RSA_WORKUNIT); /* Same type of cast
                                                                                            as above */

            /* copy p_wk_n_minus1 to PKCD_RSA_SPAREWORK1 */
            r_pkcd_cmn_cpy_word(PKCD_RSA_SPAREWORK1->full, p_wk_n_minus1, PKCD_RSA_WORKUNIT);
            ret |= r_pkcd_cmn_AMinusB(PKCD_RSA_WORK4->full, PKCD_RSA_SPAREWORK1->full, PKCD_RSA_SPAREWORK2->full,
                                        PKCD_RSA_WORKUNIT);
            cmp_ret = r_pkcd_cmn_cmp_zero_word(PKCD_RSA_SPAREWORK2->full + PKCD_RSA_OFFSET_VAL(nlen), nlen);
            if (0 == cmp_ret)
            {
                continue;   /* if true, goto next loop */
            }
            else
            {
                /* Do nothing */
            }

            /* check if "A^Exp(mod N) != -1" is true */
            /* where d*2^0 < Exp < d*2^r = N-1 */
            r_pkcd_cmn_ram_clr(PKCD_RSA_WORK2->full, PKCD_RSA_WORKUNIT);
            tmp32 = r_pkcd_cmn_ch_endian32(PKCD_RSA_EXP2_VAL);

            /* Cast &temp32 to pass the PKCC work area */
            r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK2->full + PKCD_RSA_OFFSET_VAL(sizeof(uint32_t)),
                                        (uint8_t*)&tmp32, sizeof(uint32_t));

            for (j = 0U; j < exp2_cnt; j++ )
            {
                /* Call WDT callback */
                r_pkcd_rsa_call_wdtclr();

                /* calculate square [A^(d*2^x) => A^{d*2^(x+1)}] */
                /* copy result of last loop[= A^(d*2^x)] */
                r_pkcd_cmn_cpy_word(PKCD_RSA_WORK1->full, PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNIT);
                r_pkcd_cmn_ram_clr(PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNIT);
                ret |= r_pkcd_cmn_AExpBModCCon(PKCD_RSA_WORK1->full, PKCD_RSA_WORK2->full, PKCD_RSA_WORK3->full,
                                                PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNIT, PKC_SYNC_OPT_SYNC);

                /* check A^(d*2^x) == -1(mod N) = N-1 */
                r_pkcd_cmn_ram_clr(PKCD_RSA_SPAREWORK2->full, PKCD_RSA_WORKUNIT);
                r_pkcd_cmn_cpy_word(PKCD_RSA_SPAREWORK1->full, p_wk_n_minus1, PKCD_RSA_WORKUNIT);
                ret |= r_pkcd_cmn_AMinusB(PKCD_RSA_WORK4->full, PKCD_RSA_SPAREWORK1->full, PKCD_RSA_SPAREWORK2->full,
                                            PKCD_RSA_WORKUNIT);
                cmp_ret = r_pkcd_cmn_cmp_zero_word(PKCD_RSA_SPAREWORK2->full + PKCD_RSA_OFFSET_VAL(nlen), nlen);
                if (0 == cmp_ret)
                {
                    break;  /* if true, goto next loop */
                }
                else
                {
                    /* Do nothing */
                }
            }

            if (j == exp2_cnt)
            {
                if (PKCD_RET_OK == ret)
                {
                    /* n is composite. end loop(iteration_num) */
                    ret = PKCD_RET_RSA_COMPOSITE;
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

            if (PKCD_RET_OK != ret)
            {
                /* PKCC error. end loop(iteration_num) */
                break;
            }
            else
            {
                /* Do nothing */
            }

        }   /* for (i=0; i < iteration_num; i++ ) */
    }   /* if (timeout_cnt >= PKCD_RSA_MILLER_RABIN_DIV2_LOOP_TIMEOUT)'s else */

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_miller_rabin_test()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_calc_key_d
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Calculate RSA private key d
 * \param       [in,out]  p_key_bin     RSA key of key binary format.
 * \param       [in]      p_kegen_buf   An address of key generate buffer.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 *  - #PKCD_RET_RSA_NON_COPRIME:     a and m are not co-prime
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_PDT_2, #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WORK4
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) and #PKCD_CMN_ICUM_PKCCB0DPW value must have been set beforehand.
 * \par Detailed description
 *      - Calculate private key d of RSA based on the method of Appendix B.3 of FIPS 186-4
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Calculate (p-1)(q-1) with the #r_pkcd_cmn_AB
 * -# If #PKCD_RET_OK == ret then
 *   -# Calculate GCD((p-1), (q-1)) with the #r_pkcd_rsa_gcd
 *   -# If #PKCD_RET_OK == ret then
 *     -# Calculate Inverse GCD((p-1), (q-1)) = GCD((p-1), (q-1)) ^ (-1) mod N with the #r_pkcd_rsa_inverse
 *       -# If #PKCD_RET_OK == ret then
 *         -# Calculate T of N with the #r_pkcd_cmn_MinusCInv
 *         -# Calculate R^2 mod N with the #r_pkcd_cmn_R2ModC
 *         -# Calculate (p-1)(q-1) * Inverse GCD((p-1), (q-1)) * R^(-1) mod nwith the #r_pkcd_cmn_ABRInvModC
 *         -# Calculate LCM((p-1), (q-1)) = (p-1)(q-1) * Inverse GCD((p-1), (q-1)) mod n with the #r_pkcd_cmn_ABRInvModC
 *         -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 *         -# If #PKCD_RET_OK == ret then
 *           -# Calculate d = E^(-1) mod LCM((p-1), (q-1)) with the #r_pkcd_rsa_inverse
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_calc_key_d (const st_pkcd_rsa_key_bin_t * const p_key_bin,
                                        st_rsa_keygen_buffer_t * const p_kegen_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const uint32_t pq_length = p_key_bin->key_length / PKCD_CMN_DIV_2;

    /* calc (p-1)(q-1) */
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK1->half.h + PKCD_RSA_OFFSET_VALHALF(pq_length), p_kegen_buf->p1,
                                pq_length);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK2->half.h + PKCD_RSA_OFFSET_VALHALF(pq_length), p_kegen_buf->q1,
                                pq_length);
    r_pkcd_cmn_ram_clr(PKCD_RSA_WORK3->full, PKCD_RSA_WORKUNIT);
    ret = r_pkcd_cmn_AB(PKCD_RSA_WORK1->half.h, PKCD_RSA_WORK2->half.h, PKCD_RSA_WORK3->full,
                        PKCD_RSA_WORKUNITHALF);
    if (PKCD_RET_OK == ret)
    {
        r_pkcd_cmn_cpy_by_64bounds(p_kegen_buf->p1q1,
                                    PKCD_RSA_WORK3->full + PKCD_RSA_OFFSET_VAL(p_key_bin->key_length),
                                    p_key_bin->key_length);

        /* calc GCD((p-1), (q-1)) */
        ret = r_pkcd_rsa_gcd(p_kegen_buf->p1, pq_length, p_kegen_buf->q1, pq_length,
                                p_kegen_buf->gcd, p_kegen_buf->mod);
        if (PKCD_RET_OK == ret)
        {
            /* calc InvGCD = GCD((p-1), (q-1)) ^ (-1) mod N */
            ret = r_pkcd_rsa_inverse(p_kegen_buf->gcd, pq_length, p_key_bin->p_n, p_key_bin->key_length,
                                        p_kegen_buf->invgcd, p_kegen_buf->mod);
            if (PKCD_RET_OK == ret)
            {
                /* calc LCM((p-1), (q-1)) = ((p-1) * (q-1) * InvGCD) mod N */
                r_pkcd_cmn_set_dl(p_key_bin->key_length, p_key_bin->key_length);
                r_pkcd_cmn_ram_clr(PKCD_RSA_WORK3->full, PKCD_RSA_WORKUNIT);
                r_pkcd_cmn_ram_clr(PKCD_RSA_WORK4->full, PKCD_RSA_WORKUNIT);
                r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK4->full + PKCD_RSA_OFFSET_VAL(p_key_bin->key_length),
                                            p_key_bin->p_n, p_key_bin->key_length);
                ret = r_pkcd_cmn_MinusCInv(PKCD_RSA_WORK4->full,
                                            /* No problem with casting to pointer size */
                                            (uint8_t*)PKCD_RSA_FORMAT_DP_DPT(PKCD_RSA_PDT_2), PKCD_RSA_WORKUNIT);
                ret |= r_pkcd_cmn_R2ModC(PKCD_RSA_WORK4->full, PKCD_RSA_WORK3->full, PKCD_RSA_WORKUNIT);

                ret |= r_pkcd_cmn_clr_work_unit(PKCD_RSA_SPAREWORK1->full, PKCD_RSA_WORKUNIT);
                ret |= r_pkcd_cmn_clr_work_unit(PKCD_RSA_SPAREWORK2->full, PKCD_RSA_WORKUNIT);
                r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_SPAREWORK1->full + PKCD_RSA_OFFSET_VAL(p_key_bin->key_length),
                                            p_kegen_buf->p1q1, p_key_bin->key_length);
                r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_SPAREWORK2->full + PKCD_RSA_OFFSET_VAL(p_key_bin->key_length),
                                            p_kegen_buf->invgcd, p_key_bin->key_length);
                ret |= r_pkcd_cmn_ABRInvModC(PKCD_RSA_SPAREWORK1->full, PKCD_RSA_SPAREWORK2->full,
                                                PKCD_RSA_WORK4->full, PKCD_RSA_SPAREWORK1->full, PKCD_RSA_WORKUNIT);
                ret |= r_pkcd_cmn_ABRInvModC(PKCD_RSA_SPAREWORK1->full, PKCD_RSA_WORK3->full, PKCD_RSA_WORK4->full,
                                                PKCD_RSA_WORK3->full, PKCD_RSA_WORKUNIT);

                /* Extract the return value from the ORed result */
                ret = r_pkcd_cmn_extract_result(ret);

                if (PKCD_RET_OK == ret)
                {
                    /* calc d = E^(-1) mod LCM((p-1), (q-1))  */
                    r_pkcd_cmn_cpy_by_64bounds(p_kegen_buf->mod,
                                                PKCD_RSA_WORK3->full + PKCD_RSA_OFFSET_VAL(p_key_bin->key_length),
                                                p_key_bin->key_length);
                    ret = r_pkcd_rsa_inverse(p_key_bin->p_e, p_key_bin->e_length, p_kegen_buf->mod,
                                                p_key_bin->key_length, p_key_bin->p_d, p_kegen_buf->gcd);
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

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_calc_key_d()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_ex_gcd_calc_even
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Perform processing when u or v is even number for binary extended gbd algorithm
 * \param       [in,out]  p_wks     An address of structure having works area for binary extended gbd algorithm.
 * \param       [in]      p_xy      An address of structure having x and y data buffer pointer <br>
 *                                  for binary extended gbd algorithm.
 * \param       [in]      len       A byte size of data length.
 * \param       [in]      p_buf     An address of data temporary save buffer.
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
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) value must have been set beforehand.
 * \par Detailed description
 *      - When the value of u or v is even, calculate according to binary extended gbd algorithm.
 *      - If the processing loop exceeds the specified number, an error is returned.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Get the least significant byte of u or v(hereinafter called the "uv").
 * -# Clear timeout count
 * -# Loop while in the following case
 *   - The timeout counter has not exceeded the #PKCD_RSA_EX_GCD_LOOP_TIMEOUT
 *   - The lsb is even
 *   - There was no error in PKCC operation
 *   -# Calculate uv = uv / 2 with the #r_pkcd_cmn_ADiv2
 *   -# Get the least significant byte of A or C(hereinafter called the "AC").
 *   -# Get the least significant byte of B or D(hereinafter called the "BD").
 *   -# If (AC is even) and (BD is even) then
 *     -# Calculate AC = AC / 2 with the #r_pkcd_cmn_ADiv2
 *     -# Calculate BD = BD / 2 with the #r_pkcd_cmn_ADiv2
 *   -# Else
 *     -# Backup BD to p_buf.
 *     -# Calculate AC = AC + y with the #r_pkcd_rsa_ex_gcd_APlusB
 *     -# Calculate AC = AC / 2 with the #r_pkcd_cmn_ADiv2
 *     -# Restore p_buf to BD.
 *     -# Backup AC to p_buf.
 *     -# Calculate BD = BD - x with the #r_pkcd_rsa_ex_gcd_AMinusB
 *     -# Calculate BD = BD / 2 with the #r_pkcd_cmn_ADiv2
 *     -# Restore p_buf to AC.
 *   -# Get the least significant byte of uv.
 *   -# Increment timeout count.
 * -# If timeout count >= #PKCD_RSA_EX_GCD_LOOP_TIMEOUT then
 *   -# Set ret to #PKCD_RET_PKCCTIMEOUTERR
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_ex_gcd_calc_even(const st_rsa_ex_gcd_calc_even_wks_t* const p_wks,
                                                const st_rsa_ex_gcd_xy_t* const p_xy, const uint32_t len,
                                                uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_OK;
    uint32_t lsb;
    uint32_t lsb2;
    volatile uint32_t timeout_cnt;

    lsb = r_pkcd_cmn_get_lsb_work_unit(p_wks->p_wk_uv, PKCD_RSA_WORKUNIT);
    timeout_cnt = 0U;
    while ((PKCD_RSA_EX_GCD_LOOP_TIMEOUT > timeout_cnt) && ((lsb & 0x01U) == 0x00U) && (PKCD_RET_OK == ret))
    {
        /* Perform processing when u or v is even */
        /* uv = uv /2 */
        ret = r_pkcd_cmn_ADiv2(p_wks->p_wk_uv, p_wks->p_wk_uv, PKCD_RSA_WORKUNIT);

        /* check AC & BD even */
        lsb = r_pkcd_cmn_get_lsb_work_unit(p_wks->p_wk_AC, PKCD_RSA_WORKUNIT);
        lsb2 = r_pkcd_cmn_get_lsb_work_unit(p_wks->p_wk_BD, PKCD_RSA_WORKUNIT);
        if (((lsb & 0x01U) == 0x00U) && ((lsb2 & 0x01U) == 0x00U))
        {
            /* AC = AC/2 */
            ret |= r_pkcd_cmn_ADiv2(p_wks->p_wk_AC, p_wks->p_wk_AC, PKCD_RSA_WORKUNIT);

            /* BD = BD/2*/
            ret |= r_pkcd_cmn_ADiv2(p_wks->p_wk_BD, p_wks->p_wk_BD, PKCD_RSA_WORKUNIT);
        }
        else
        {
            /* AC = (AC + y)/2 (y is p_xy->p_y) */
            r_pkcd_cmn_ram_clr(p_wks->p_wk_xy, PKCD_RSA_WORKUNIT);
            r_pkcd_cmn_cpy_by_64bounds(p_wks->p_wk_xy + PKCD_RSA_OFFSET_VAL(p_xy->ylen), p_xy->p_y, p_xy->ylen);
            r_pkcd_cmn_cpy_word(p_buf, p_wks->p_wk_BD, PKCD_RSA_WORKUNIT);   /* wk_BD backup */

            /* wk_BD use as a temp work */
            ret |= r_pkcd_rsa_ex_gcd_APlusB(p_wks->p_wk_AC, p_wks->p_signAC, p_wks->p_wk_xy, 0U, len, p_wks->p_wk_BD);
            ret |= r_pkcd_cmn_ADiv2(p_wks->p_wk_AC, p_wks->p_wk_AC, PKCD_RSA_WORKUNIT);
            r_pkcd_cmn_cpy_word(p_wks->p_wk_BD, p_buf, PKCD_RSA_WORKUNIT);   /* wk_BD restore */

            /* BD = (BD - x)/2  (x is p_xy->p_x)*/
            r_pkcd_cmn_ram_clr(p_wks->p_wk_xy, PKCD_RSA_WORKUNIT);
            r_pkcd_cmn_cpy_by_64bounds(p_wks->p_wk_xy + PKCD_RSA_OFFSET_VAL(p_xy->xlen), p_xy->p_x, p_xy->xlen);
            r_pkcd_cmn_cpy_word(p_buf, p_wks->p_wk_AC, PKCD_RSA_WORKUNIT);   /* wk_AC backup */

            /* wk_BD use as a temp work */
            ret |= r_pkcd_rsa_ex_gcd_AMinusB(p_wks->p_wk_BD, p_wks->p_signBD, p_wks->p_wk_xy, 0U, len, p_wks->p_wk_AC);
            ret |= r_pkcd_cmn_ADiv2(p_wks->p_wk_BD, p_wks->p_wk_BD, PKCD_RSA_WORKUNIT);
            r_pkcd_cmn_cpy_word(p_wks->p_wk_AC, p_buf, PKCD_RSA_WORKUNIT);   /* wk_AC restore */
        }

        lsb = r_pkcd_cmn_get_lsb_work_unit(p_wks->p_wk_uv, PKCD_RSA_WORKUNIT);
        timeout_cnt += 1U;
    }   /* while ((lsb & 0x01) == 0x00) */

    /* If loop is timeout, return error */
    if (timeout_cnt >= PKCD_RSA_EX_GCD_LOOP_TIMEOUT)
    {
        ret |= PKCD_RET_PKCCTIMEOUTERR;
    }
    else
    {
        /* Do nothing */
    }

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_ex_gcd_calc_even()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_ex_gcd_APlusB
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Calculate the addition for binary extended gbd algorithm
 * \param       [in,out]  p_wk_a       An address of work area where either A to D is set. <br>
 *                                     The data must be aligned with bottom.
 * \param       [in,out]  p_sign_a     A signed flag of wk_a.
 * \param       [in]      p_wk_b       An address of work area where either A to D is set. <br>
*                                      The data must be aligned with bottom.
 * \param       [in]      sign_b       A signed flag of wk_b.
 * \param       [in]      len          A byte size of data length.
 * \param       [in]      p_wk_tmp     An adress of work area where temporary.
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
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) value must have been set beforehand.
 * \par Detailed description
 *      - Calculate the addition of either A to D of binary extended gbd algorithm parameters.
 *      - The signs is calculated separately from the value.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Compare a and b with #r_pkcd_cmn_cmp_work_unit and set compare result to cmp_ret.
 * -# If cmp_ret >= 0 (a >= b) then
 *   -# if sign a == sign b then
 *     -# Caluclate a = a + b with #r_pkcd_cmn_APlusB
 *   -# else
 *     -# Caluclate a = a - b with #r_pkcd_cmn_AMinusB
 * -# else
 *   -# if sign a == sign b then
 *     -# Caluclate a = a + b with #r_pkcd_cmn_APlusB
 *   -# else
 *     -# Caluclate a = b - b with #r_pkcd_cmn_AMinusB
 *     -# turn over sign a
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_ex_gcd_APlusB(const uint8_t* const p_wk_a, uint32_t* const p_sign_a,
                                            const uint8_t* const p_wk_b, const uint32_t sign_b, const uint32_t len,
                                            uint8_t* const p_wk_tmp)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    int32_t cmp_ret;

    ret = r_pkcd_cmn_cmp_work_unit(p_wk_a, p_wk_b, p_wk_tmp, len, PKCD_RSA_WORKUNIT, &cmp_ret);
    if (cmp_ret >= 0)
    {
        if ((*p_sign_a) == sign_b)
        {
            ret |= r_pkcd_cmn_APlusB(p_wk_a, p_wk_b, p_wk_a, PKCD_RSA_WORKUNIT);
        }
        else
        {
            ret |= r_pkcd_cmn_AMinusB(p_wk_a, p_wk_b, p_wk_a, PKCD_RSA_WORKUNIT);
        }
    }
    else
    {
        if ((*p_sign_a) == sign_b)
        {
            ret |= r_pkcd_cmn_APlusB(p_wk_a, p_wk_b, p_wk_a, PKCD_RSA_WORKUNIT);
        }
        else
        {
            ret |= r_pkcd_cmn_AMinusB(p_wk_b, p_wk_a, p_wk_a, PKCD_RSA_WORKUNIT);
            *p_sign_a = (0U == (*p_sign_a)) ? 1U : 0U; /* change sign */
        }
    }

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_ex_gcd_APlusB()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_ex_gcd_AMinusB
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Calculate the subtraction for binary extended gbd algorithm
 * \param       [in,out]  p_wk_a       An address of work area where either A to D is set. <br>
 *                                     The data must be aligned with bottom.
 * \param       [in,out]  p_sign_a     A signed flag of wk_a.
 * \param       [in]      p_wk_b       An address of work area where either A to D is set. <br>
 *                                     The data must be aligned with bottom.
 * \param       [in]      sign_b       A signed flag of wk_b.
 * \param       [in]      len          A byte size of data length.
 * \param       [in]      p_wk_tmp     An adress of work area where temporary.
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
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) value must have been set beforehand.
 * \par Detailed description
 *      - Calculate the subtraction of either A to D of binary extended gbd algorithm parameters.
 *      - The signs is calculated separately from the value.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Compare a and b with #r_pkcd_cmn_cmp_work_unit and set compare result to cmp_ret.
 * -# If cmp_ret >= 0 (a >= b) then
 *   -# if sign a == sign b then
 *     -# Caluclate a = a - b with #r_pkcd_cmn_AMinusB
 *   -# else
 *     -# Caluclate a = a + b with #r_pkcd_cmn_APlusB
 * -# else
 *   -# if sign a == sign b then
 *     -# Caluclate a = b - a with #r_pkcd_cmn_AMinusB
 *     -# turn over sign a
 *   -# else
 *     -# Caluclate a = a + b with #r_pkcd_cmn_APlusB
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_ex_gcd_AMinusB(const uint8_t* const p_wk_a, uint32_t* const p_sign_a,
                                            const uint8_t* const p_wk_b, const uint32_t sign_b,
                                            const uint32_t len, uint8_t* const p_wk_tmp)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    int32_t cmp_ret;

    ret = r_pkcd_cmn_cmp_work_unit(p_wk_a, p_wk_b, p_wk_tmp, len, PKCD_RSA_WORKUNIT, &cmp_ret);
    if (cmp_ret >= 0)
    {
        if ((*p_sign_a) == sign_b)
        {
            ret |= r_pkcd_cmn_AMinusB(p_wk_a, p_wk_b, p_wk_a, PKCD_RSA_WORKUNIT);
        }
        else
        {
            ret |= r_pkcd_cmn_APlusB(p_wk_a, p_wk_b, p_wk_a, PKCD_RSA_WORKUNIT);
        }
    }
    else
    {
        if ((*p_sign_a) == sign_b)
        {
            ret |= r_pkcd_cmn_AMinusB(p_wk_b, p_wk_a, p_wk_a, PKCD_RSA_WORKUNIT);
            *p_sign_a = (0U == (*p_sign_a)) ? 1U : 0U;  /* change sign */
        }
        else
        {
            ret |= r_pkcd_cmn_APlusB(p_wk_a, p_wk_b, p_wk_a, PKCD_RSA_WORKUNIT);
        }
    }

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_ex_gcd_AMinusB()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_ex_gcd_format_D
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Format the calculation result D of the binary extended gbd algorithm
 * \param       [in,out]  p_wk_D       An adress of work area where D. The data must be aligned with bottom.
 * \param       [in]      signD        A signed flag of D.
 * \param       [in]      p_wk_m       An adress of work area where modulus. The data must be aligned with bottom.
 * \param       [in]      p_wk_temp    An adress of work area where temporary.
 * \param       [in]      len          A byte size of data length.
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
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) value must have been set beforehand.
 * \par Detailed description
 *      - Format the calculation result D of the binary extended gbd algorithm in the following case.
 *        -  If |D| is lager than modulus m, calculate |D| mod m.
 *        -  If D is negative, subtruct D from moduls m.
 *      - If the processing loop exceeds the specified number, an error is returned.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Clear timeout counter.
 * -# Compare |D| and m with #r_pkcd_cmn_cmp_work_unit and set compare result to cmp_ret.
 * -# Loop while in the following case
 *   - The timeout counter has not exceeded the #PKCD_RSA_REDUCE_LOOP_TIMEOUT
 *   - D is greater than m
 *   - There was no error in PKCC operation
 *   -# Calculate |D'| = |D| - m with #r_pkcd_cmn_AMinusB
 *   -# Compare |D'| and m with #r_pkcd_cmn_cmp_work_unit and set compare result to cmp_ret.
 *   -# Increment timout count
 * -# If timeout count >= #PKCD_RSA_REDUCE_LOOP_TIMEOUT then
 *   -# Set ret to #PKCD_RET_PKCCTIMEOUTERR
 * -# If signD != 0 then
 *   -# Calculate m - D' with #r_pkcd_cmn_AMinusB
 * -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_ex_gcd_format_D(const uint8_t* const p_wk_D, const uint32_t signD,
                                                const uint8_t* const p_wk_m, uint8_t* const p_wk_temp,
                                                const uint32_t len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    volatile uint32_t timeout_cnt;
    int32_t cmp_ret;

    /* If |D| is larger than m, calculate |D| - m */
    timeout_cnt = 0U;
    ret = r_pkcd_cmn_cmp_work_unit(p_wk_D, p_wk_m, p_wk_temp, len, PKCD_RSA_WORKUNIT, &cmp_ret);
    while ((PKCD_RSA_REDUCE_LOOP_TIMEOUT > timeout_cnt) && (cmp_ret > 0) && (PKCD_RET_OK == ret))
    {
        ret = r_pkcd_cmn_AMinusB(p_wk_D, p_wk_m, p_wk_D, PKCD_RSA_WORKUNIT);
        ret |= r_pkcd_cmn_cmp_work_unit(p_wk_D, p_wk_m, p_wk_temp, len, PKCD_RSA_WORKUNIT, &cmp_ret);
        timeout_cnt += 1U;
    }

    /* If loop is timeout, return error */
    if (timeout_cnt >= PKCD_RSA_REDUCE_LOOP_TIMEOUT)
    {
        ret |= PKCD_RET_PKCCTIMEOUTERR;
    }
    else
    {
        /* Do nothing */
    }

    /* If D is negative, subtruct D from modulus m  */
    if (0U != signD)
    {
        ret |= r_pkcd_cmn_AMinusB(p_wk_m, p_wk_D, p_wk_D, PKCD_RSA_WORKUNIT);
    }
    else
    {
        /* Do nothing */
    }

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_ex_gcd_format_D()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_chk_mod_exp_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDRSAAPIsInternal
 * \brief        #R_PKCD_RsaModExpInit argument check function.
 * \param       [in] p_msg_in       An inputting message.
 * \param       [in] p_key          RSA private or pulic key of key binary format
 * \param       [in] mod_len        Length of the modulus n
 * \param       [in] exec_type      RSA execution type
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in] p_buf          Buffer used for internal processing of PKC-Driver
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the arguments received by the #R_PKCD_RsaModExpInit function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If ((NULL != p_key) && (NULL != p_ctx) && (NULL != p_buf))
 *   -# If ((#PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) || (#PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
 *     -# Set (#st_pkcd_rsa_key_bin_t*)p_key to p_key_bin
 *     -# Check key length with #r_pkcd_rsa_chk_key_len, and set ret to the return value.
 *     -# If #PKCD_RET_OK == ret
 *       -# If mod_len == p_key_bin->key_length
 *         -# If ((#PKCD_RSA_EXEC_TYPE_DEC == exec_type) || (#PKCD_RSA_EXEC_TYPE_SIGN == exec_type))
 *           -# If ((NULL != p_key_bin->p_n) && (NULL != p_key_bin->p_d))
 *           -# Else
 *             -# Set ret to #PKCD_RET_KEY_PARAMERR.
 *         -# Else If ((#PKCD_RSA_EXEC_TYPE_ENC == exec_type) || (#PKCD_RSA_EXEC_TYPE_VERIFY == exec_type))
 *           -# Check with #r_pkcd_cmn_is_multiple_4 to see if the length of e is a multiple of 4,
 *              and set ret to the return value.
 *           -# If (((#PKCD_RSA_PUB_EXP_LEN_MIN <= p_key_bin->e_length) &&
 *                   (p_key_bin->e_length <= p_key_bin->key_length)) && (PKC_BOOL_TRUE == is_multiple))
 *             -# if ((NULL != p_key_bin->p_n) && (NULL != p_key_bin->p_e))
 *             -# Else
 *               -# Set ret to #PKCD_RET_KEY_PARAMERR.
 *           -# Else
 *             -# Set ret to #PKCD_RET_KEY_LENGTHERR.
 *         -# Else
 *           -# Set ret to #PKCD_RET_PARAMERR.
 *         -# If #PKCD_RET_OK == ret
 *           -# Check the message with #r_pkcd_rsa_chk_msg, and set ret to the return value.
 *       -# Else
 *         -# Set ret to #PKCD_RET_LENGTHERR.
 *     -# Else
 *       -# Set ret to #PKCD_RET_KEY_LENGTHERR.
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR.
 *   -# If (#PKC_BOOL_FALSE == p_ctx->sec_mod_exp) || (NULL != p_ctx->p_drbg_api), then
 *     -# Do nothing.
 *   -# Else
 *     Set ret to #PKCD_RET_PARAMERR.
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_chk_mod_exp_init_arg(const st_pkc_data_t* const p_msg_in,
                                                    const void* const p_key,
                                                    const uint32_t mod_len,
                                                    const e_pkcd_rsa_exec_type_t exec_type,
                                                    const st_pkc_rsa_ctx_t* const p_ctx,
                                                    const uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const st_pkcd_rsa_key_bin_t * p_key_bin;
    e_pkc_bool_t is_multiple;

    /* check if the arguments are NULL */
    if ((NULL != p_key) && (NULL != p_ctx) && (NULL != p_buf))
    {
        if ((PKC_BOOL_TRUE == p_ctx->is_constant_time_mode) ||
            (PKC_BOOL_FALSE == p_ctx->is_constant_time_mode))
        {
            /* Cast void* to the driver-specified key binary pointer type. */
            p_key_bin = (const st_pkcd_rsa_key_bin_t*)p_key;

            /* Check key length */
            ret = r_pkcd_rsa_chk_key_len(p_key_bin->key_length);
            if (PKCD_RET_OK == ret)
            {
                /* Compare key length and modulus length */
                if (mod_len == p_key_bin->key_length)
                {
                    if ((PKCD_RSA_EXEC_TYPE_DEC == exec_type) || (PKCD_RSA_EXEC_TYPE_SIGN == exec_type))
                    {
                        if ((NULL != p_key_bin->p_n) && (NULL != p_key_bin->p_d))
                        {
                            /* All check OK. Do nothing. */
                        }
                        else
                        {
                            ret = PKCD_RET_KEY_PARAMERR;
                        }
                    }
                    else if ((PKCD_RSA_EXEC_TYPE_ENC == exec_type) || (PKCD_RSA_EXEC_TYPE_VERIFY == exec_type))
                    {
                        is_multiple = r_pkcd_cmn_is_multiple_4(p_key_bin->e_length);
                        if (((PKCD_RSA_PUB_EXP_LEN_MIN <= p_key_bin->e_length) &&
                            (p_key_bin->e_length <= p_key_bin->key_length)) && (PKC_BOOL_TRUE == is_multiple))
                        {
                            if ((NULL != p_key_bin->p_n) && (NULL != p_key_bin->p_e))
                            {
                                /* Do nothing. Go to the message check below. */
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
                        /* Invalid exec type */
                        ret = PKCD_RET_PARAMERR;
                    }

                    if (PKCD_RET_OK == ret)
                    {
                        /* Check message */
                        ret = r_pkcd_rsa_chk_msg(p_msg_in, p_key_bin);
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    /* Key length and modulus length do not match */
                    ret = PKCD_RET_LENGTHERR;
                }
            }
            else
            {
                /* Invalid Key length in key binary */
                ret = PKCD_RET_KEY_LENGTHERR;
            }
        }
        else
        {
            /* An invalid value was set for the constant time mode */
            ret = PKCD_RET_PARAMERR;
        }

        if ((PKC_BOOL_FALSE == p_ctx->sec_mod_exp) || (NULL != p_ctx->p_drbg_api))
        {
            /* Do nothing */
        }
        else
        {
            /* The pointer to DRBG API is NULL even though the blinding flag is TRUE. */
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
* End of function r_pkcd_rsa_chk_mod_exp_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_chk_mod_exp_crt_init_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDRSAAPIsInternal
 * \brief        #R_PKCD_RsaModExpCRTInit argument check function.
 * \param       [in] p_msg_in       An inputting message.
 * \param       [in] p_priv_key     RSA private key of key binary format
 * \param       [in] mod_len        Length of the modulus n
 * \param       [in] exec_type      RSA execution type
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in] p_buf          Buffer used for internal processing of PKC-Driver
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Check the arguments received by the #R_PKCD_RsaModExpCRTInit function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If ((NULL != p_priv_key) && (NULL != p_ctx) && (NULL != p_buf))
 *   -# Set (st_pkcd_rsa_key_bin_t*)p_priv_key to p_key_bin
 *   -# Check key length with #r_pkcd_rsa_chk_key_len, and set ret to the return value.
 *   -# If #PKCD_RET_OK == ret
 *     -# If mod_len == p_key_bin->key_length
 *       -# Check message with #r_pkcd_rsa_chk_msg, and set ret to the return value.
 *       -# If #PKCD_RET_OK == ret
 *         -# If ((#PKCD_RSA_EXEC_TYPE_DEC == exec_type) || (#PKCD_RSA_EXEC_TYPE_SIGN == exec_type))
 *           -# If ((NULL != p_key_bin->p_p) && (NULL != p_key_bin->p_q) && (NULL != p_key_bin->p_dp) &&
 *                 (NULL != p_key_bin->p_dq) && (NULL != p_key_bin->p_qinv) && (NULL != p_key_bin->p_n))
 *           -# Else
 *             -# Set ret to #PKCD_RET_KEY_PARAMERR.
 *         -# Else
 *           -# Set ret to #PKCD_RET_PARAMERR.
 *         -# If #PKCD_RET_OK == ret
 *           -# Check the message with #r_pkcd_rsa_chk_msg, and set ret to the return value.
 *     -# Else
 *       -# Set ret to #PKCD_RET_LENGTHERR.
 *   -# Else
 *     -# Set ret to #PKCD_RET_KEY_LENGTHERR.
 *   -# If (#PKC_BOOL_FALSE == p_ctx->sec_mod_exp) || (NULL != p_ctx->p_drbg_api), then
 *     -# Do nothing.
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR.
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_chk_mod_exp_crt_init_arg(const st_pkc_data_t * const p_msg_in,
                                                        const void * const p_priv_key,
                                                        const uint32_t mod_len,
                                                        const e_pkcd_rsa_exec_type_t exec_type,
                                                        const st_pkc_rsa_ctx_t * const p_ctx,
                                                        const uint8_t * const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const st_pkcd_rsa_key_bin_t * p_key_bin;

    /* check if the arguments are NULL */
    if ((NULL != p_priv_key) && (NULL != p_ctx) && (NULL != p_buf))
    {
        /* Cast void* to the driver-specified key binary pointer type. */
        p_key_bin = (const st_pkcd_rsa_key_bin_t*)p_priv_key;

        /* Check key length */
        ret = r_pkcd_rsa_chk_key_len(p_key_bin->key_length);
        if (PKCD_RET_OK == ret)
        {
            /* Compare key length and modulus length */
            if (mod_len == p_key_bin->key_length)
            {
                if ((PKCD_RSA_EXEC_TYPE_DEC == exec_type) || (PKCD_RSA_EXEC_TYPE_SIGN == exec_type))
                {
                    if ((NULL != p_key_bin->p_p) && (NULL != p_key_bin->p_q) &&
                        (NULL != p_key_bin->p_dp) && (NULL != p_key_bin->p_dq) &&
                        (NULL != p_key_bin->p_qinv) && (NULL != p_key_bin->p_n))
                    {
                        /* Do nothing. Go to message check below */
                    }
                    else
                    {
                        ret = PKCD_RET_KEY_PARAMERR;
                    }
                }
                else
                {
                    /* Invalid exec type */
                    ret = PKCD_RET_PARAMERR;
                }

                if (PKCD_RET_OK == ret)
                {
                    /* Check message */
                    ret = r_pkcd_rsa_chk_msg(p_msg_in, p_key_bin);
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                /* Key length and modulus length do not match */
                ret = PKCD_RET_LENGTHERR;
            }
        }
        else
        {
            /* Invalid Key length in key binary */
            ret = PKCD_RET_KEY_LENGTHERR;
        }

        if ((PKC_BOOL_FALSE == p_ctx->sec_mod_exp) || (NULL != p_ctx->p_drbg_api))
        {
            /* Do nothing */
        }
        else
        {
            /* The pointer to DRBG API is NULL even though the anti-tampering flag is TRUE. */
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
* End of function r_pkcd_rsa_chk_mod_exp_crt_init_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_chk_test_prime_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDRSAAPIsInternal
 * \brief        #R_PKCD_RsaTestPrime argument check function.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by RSA. <br>
 * \param       [in] p_buf          Buffer used for internal processing of PKC-Driver
 * \param       [in] p_candidate    Prime number candidate data.
 * \param       [in] iteration_num  Iteration number for Miller-Rabin test.
 * \param       [in] p_test_randoms Random value used in Miller-Rabin test.
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
 *  - Check the arguments received by the #R_PKCD_RsaTestPrime function.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_candidate))
 *   -# If (NULL != p_candidate->p_data)
 *     -# If ((#PKCD_RSA_ITERATION_NUM_MIN <= iteration_num) && (#PKCD_RSA_ITERATION_NUM_MAX >= iteration_num))
 *       -# If (p_candidate->len <= #PKCD_RSA_PQ_LEN_MAX)
 *         -# Check prime length with #r_pkcd_rsa_chk_key_len, and set ret to the return value.
 *         -# If #PKCD_RET_OK == ret
 *           -# Loop while i is iteration_num or less<br>
 *              Increment i for each loop
 *             -# If NULL == p_test_randoms[i]
 *               -# Set ret to #PKCD_RET_PARAMERR.
 *       -# Else
 *         -# Set ret to #PKCD_RET_LENGTHERR.
 *     -# Else
 *       -# Set ret to #PKCD_RET_LENGTHERR.
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR.
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_chk_test_prime_arg(const st_pkc_rsa_ctx_t* const p_ctx,
                                                const uint8_t* const p_buf,
                                                const st_pkc_data_t* const p_candidate,
                                                const uint32_t iteration_num,
                                                uint8_t* const p_test_randoms[])
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t i;

    /* check if the arguments are NULL */
    if ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_candidate))
    {
        /* check candidate */
        if (NULL != p_candidate->p_data)
        {
            /* check iteration number */
            if ((PKCD_RSA_ITERATION_NUM_MIN <= iteration_num) && (PKCD_RSA_ITERATION_NUM_MAX >= iteration_num))
            {
                if (p_candidate->len <= PKCD_RSA_PQ_LEN_MAX)
                {
                    /* check prime len*/
                    ret = r_pkcd_rsa_chk_key_len(p_candidate->len * PKCD_CMN_MUL_2);
                    if (PKCD_RET_OK == ret)
                    {
                        /* check test randomus are NULL */
                        for (i = 0U; i < iteration_num; i++ )
                        {
                            if (NULL == p_test_randoms[i])
                            {
                                ret = PKCD_RET_PARAMERR;
                                break;
                            }
                            else
                            {
                                /* Do nothing */
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
* End of function r_pkcd_rsa_chk_test_prime_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_chk_msg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDRSAAPIsInternal
 * \brief        Check the message
 * \param       [in]   p_msg          A message.
 * \param       [in]   p_key_bin      Key binary data.
 * \return
 *  - #PKCD_RET_OK:                     Successfully done.
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCD_RET_PARAMERR:               Mandatory parameters are NULL.
 *  - #PKCD_RET_LENGTHERR:              Invalid message lengths are passed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:         PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:                An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Check the message.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If NULL != p_msg
 *   -# If NULL != p_msg
 *     -# If message length <= key length
 *       -# Check the message length is a multiple of 4 with the #r_pkcd_cmn_is_multiple_4
 *       -# If PKC_BOOL_TRUE == is_multiple
 *         -# Set ret to #PKCD_RET_OK
 *       -# Else
 *         -# Set ret to #PKCD_RET_LENGTHERR
 *     -# Else
 *       -# Set ret to #PKCD_RET_LENGTHERR
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR
 * -# If #PKCD_RET_OK == ret, then
 *   -# Set the PKCC DL to the key length with #r_pkcd_cmn_set_dl.
 *   -# Compare message and n with #r_pkcd_cmn_cmp_work_unit, and set cmp_ret the return value.
 *   -# Clear work areas used in above comparison, and add the return value to ret.
 *   -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 *   -# If #PKCD_RET_OK == ret, then
 *     -# If cmp_ret <= 0, then
 *       -# Set ret to #PKCD_RET_PARAMERR.
 *     -# Else
 *       -# Do nothing.
 *   -# Else
 *     -# Do nothing.
 * -# Else
 *   -# Do nothing.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_chk_msg(const st_pkc_data_t* const p_msg, const st_pkcd_rsa_key_bin_t* const p_key_bin)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    e_pkc_bool_t is_multiple;
    int32_t cmp_ret;

    /* check p_msg is NULL */
    if (NULL != p_msg)
    {
        if (NULL != p_msg->p_data)
        {
            /* check the message length range */
            if (p_msg->len <= p_key_bin->key_length)
            {
                /* check the message length is a multiple of 4 */
                is_multiple = r_pkcd_cmn_is_multiple_4(p_msg->len);

                if (PKC_BOOL_TRUE == is_multiple)
                {
                    ret = PKCD_RET_OK;
                }
                else
                {
                    ret =  PKCD_RET_LENGTHERR;
                }
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

    /* Check if msg < n */
    if (PKCD_RET_OK == ret)
    {
        /* Prepare operation */
        r_pkcd_cmn_set_dl(p_key_bin->key_length, p_key_bin->key_length);
        r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK1->full + PKCD_RSA_OFFSET_VAL(p_msg->len),
                                    p_msg->p_data, p_msg->len);
        r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK2->full + PKCD_RSA_OFFSET_VAL(p_key_bin->key_length),
                                    p_key_bin->p_n, p_key_bin->key_length);

        /* Compare n (in WORK2) and msg (in WORK1) */
        ret = r_pkcd_cmn_cmp_work_unit(PKCD_RSA_WORK2->full, PKCD_RSA_WORK1->full, PKCD_RSA_WORK3->full,
                                        p_key_bin->key_length, PKCD_RSA_WORKUNIT, &cmp_ret);

        /* Clear work areas */
        ret |= r_pkcd_cmn_clr_work_unit(PKCD_RSA_WORK1->full, PKCD_RSA_WORKUNIT);
        ret |= r_pkcd_cmn_clr_work_unit(PKCD_RSA_WORK2->full, PKCD_RSA_WORKUNIT);
        ret |= r_pkcd_cmn_clr_work_unit(PKCD_RSA_WORK3->full, PKCD_RSA_WORKUNIT);

        ret = r_pkcd_cmn_extract_result(ret);

        if (PKCD_RET_OK == ret)
        {
            if (cmp_ret <= 0)
            {
                ret = PKCD_RET_PARAMERR;
            }
            else
            {
                /* Do nothing. */
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
/***********************************************************************************************************************
* End of function r_pkcd_rsa_chk_msg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_chk_key_len
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup      PKCDRSAAPIsInternal
 * \brief        Check the key length
 * \param       [in]   key_len        A key length.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
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
 *  - Check if the key length fits in the supported length.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# switch key_len
 *   -# key_len == #PKCD_RSA_KEY_LEN_1024
 *      key_len == #PKCD_RSA_KEY_LEN_2048
 *      key_len == #PKCD_RSA_KEY_LEN_3072
 *      key_len == #PKCD_RSA_KEY_LEN_4096
 *     -# set ret to #PKCD_RET_OK.
 *   -# default
 *     -# set ret to #PKCD_RET_KEY_LENGTHERR.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_chk_key_len(const uint32_t key_len)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* check the key length range */
    switch (key_len)
    {
        case PKCD_RSA_KEY_LEN_1024:
        case PKCD_RSA_KEY_LEN_2048:
        case PKCD_RSA_KEY_LEN_3072:
        case PKCD_RSA_KEY_LEN_4096:
            ret = PKCD_RET_OK;
            break;
        default:
            ret = PKCD_RET_LENGTHERR;
            break;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_chk_key_len()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_generate_p_q_n_d
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Generate RSA key parameters p, q, d, and modulus n.
 * \param       [in] p_keygen       A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in,out] p_ctx          An address of the context for the RSA key generation.
 * \param       [in] p_buf              An address of a buffer used for the internal processing of PKC-Driver.
 * \param       [out] p_offset_for_gen_crt    A pointer to an offset value for #r_pkcd_rsa_generate_crt.
 * \return
 *  - #PKCD_RET_OK:                     Successful completion
 *  - #PKCD_RET_INTERNALERR:            Internal processing failed
 *  - #PKCD_RET_PARAMERR:               Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:              Invalid argument length entered
 *  - #PKCD_RET_PRIME_GENERATING:       The number of generation failures exceeds prime_test_num
 *                                      set in #R_PKCD_RsaKeyGenInit.
 *  - #PKCD_RET_DRBGAPIERR:             The DRBG API passed by user returned error.
 *  - #PKCD_RET_PKCCTIMEOUTERR:         PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:                An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPW, #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *   - Satisfy both of the following conditions:
 *     - #R_PKCD_Initialize has been called.
 *     - The PKCC must not be running.
 * \par Detailed description
 *  - Generate an RSA key pair from the input prime numbers p and q.
 *  - Public key e uses 65537 (0x10001).
 *  - The key derivation procedure is performed according to appendix B.3.1 in FIPS186-4.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCD_RET_INTERNALERR.
 *  -# Set cnt_d_loop to 0U.
 *  -# While ((0U == p_keygen->d_len) && (#PKCD_RET_OK == ret)), do the following:
 *    -# Generate a private prime p with #r_pkcd_rsa_generate_p. And set ret to the result.
 *    -# If #PKCD_RET_OK == ret, then
 *      -# Generate a private prime q with #r_pkcd_rsa_generate_q. And set ret to the result.
 *    -# Else
 *      -# Do nothing.
 *    -# If #PKCD_RET_OK == ret, then
 *      -# Set the key binary member length key_bin.key_length to p_keygen->key_len.
 *      -# Set the key binary member n address key_bin.p_n to p_keygen->n_data.
 *      -# Set the key binary member d address key_bin.p_d to p_keygen->d_data.
 *      -# Set the key binary member p address key_bin.p_p to p_keygen->p_data.
 *      -# Set the key binary member q address key_bin.p_q to p_keygen->q_data.
 *      -# Set the key binary member dp address key_bin.p_dp to p_keygen->dp_data.
 *      -# Set the key binary member dq address key_bin.p_dq to p_keygen->dq_data.
 *      -# Set the key binary member qinv address key_bin.p_qinv to p_keygen->qinv_data.
 *      -# Set the key binary member e length key_bin.e_length to p_keygen->e_len.
 *      -# Set the key binary member e address key_bin.p_e to p_keygen->e_data.
 *      -# Set the buffer address for the parameter d generation gen_pq_offset to
 *         p_keygen->iteration_num * (key length / 2) + p_keygen->ck_offset + sizeof(#st_rsa_prm_test_prime_t).
 *      -# Generate a private exponent d from p, q with #r_pkcd_rsa_generate_n_d and set ret to the result.
 *      -# If #PKCD_RET_OK == ret then,
 *        -# Set the key binary member d length p_keygen->d_len to key_bin.key_length.
 *        -# Set p_offset_for_gen_crt to gen_pq_offset.
 *      -# Else if (#PKCD_RET_RSA_RANGEERR == ret)
 *        -# If (cnt_d_loop < #PKCD_RSA_GEN_D_NUM_MAX)
 *          -# Set ret to #PKCD_RET_OK.
 *        -# Else
 *          -# Set ret to #PKCD_RET_INTERNALERR
 *        -# cnt_d_loop++
 *      -# Else
 *        -# Do nothing.
 *    -# Else
 *      -# Do nothing.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_generate_p_q_n_d(st_rsa_prm_keygen_t * const p_keygen,
                                            const st_pkc_rsa_ctx_t * const p_ctx, uint8_t * const p_buf,
                                            uint32_t * const p_offset_for_gen_crt)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t gen_pq_offset;
    st_pkcd_rsa_key_bin_t key_bin;
    uint32_t cnt_d_loop = 0U;

    do
    {
        /* Generate p */
        ret = r_pkcd_rsa_generate_p(p_keygen, p_ctx, p_buf);

        if (PKCD_RET_OK == ret)
        {
            /* Generate q */
            ret = r_pkcd_rsa_generate_q(p_keygen, p_ctx, p_buf);
        }
        else
        {
            /* Do nothing */
        }

        if (PKCD_RET_OK == ret)
        {
            /* Set key binary */
            key_bin.key_length = p_keygen->key_len;
            key_bin.p_n = p_keygen->n_data;
            key_bin.p_d = p_keygen->d_data;
            key_bin.p_p = p_keygen->p_data;
            key_bin.p_q = p_keygen->q_data;
            key_bin.p_dp = p_keygen->dp_data;
            key_bin.p_dq = p_keygen->dq_data;
            key_bin.p_qinv = p_keygen->qinv_data;
            key_bin.e_length = p_keygen->e_len;
            key_bin.p_e = p_keygen->e_data;

            gen_pq_offset = ((p_keygen->iteration_num * (p_keygen->key_len / PKCD_CMN_DIV_2))
                            + (p_keygen->ck_offset + sizeof(st_rsa_prm_test_prime_t)));

            ret = r_pkcd_rsa_generate_n_d(&key_bin, &p_buf[gen_pq_offset]);

            if (PKCD_RET_OK == ret)
            {
                /* Set the byte size of d */
                p_keygen->d_len = key_bin.key_length;

                *p_offset_for_gen_crt = gen_pq_offset;
            }
            else if (PKCD_RET_RSA_RANGEERR == ret)
            {
                if (cnt_d_loop < PKCD_RSA_GEN_D_NUM_MAX)
                {
                    /* Clear p and q length for r_pkcd_rsa_generate_p and r_pkcd_rsa_generate_q*/
                    p_keygen->p_len = 0U;
                    p_keygen->q_len = 0U;

                    ret = PKCD_RET_OK;
                }
                else
                {
                    ret = PKCD_RET_INTERNALERR;
                }

                cnt_d_loop++;
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
    while ((0U == p_keygen->d_len) && (PKCD_RET_OK == ret));

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_generate_p_q_n_d()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_generate_p
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Continue generating random numbers until a prime number is obtained.
 *              This process follows FIPS186-4 Appendix B.3.1.
 * \param       [in] p_keygen           An address of the buffer for the RSA key generation.
 * \param       [in] p_ctx              An address of the context for the RSA key generation.
 * \param       [in] p_buf              An address of the buffer for this driver's RSA key generation.
 * \return
 *  - #PKCD_RET_OK:                 Successful completion
 *  - #PKCD_RET_DRBGAPIERR:         External DRBG API returned error.
 *  - #PKCD_RET_INTERNALERR:        Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:     PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:            An error occurred while the PKCC operations.
 *  - #PKCD_RET_PRIME_GENERATING:   Prime generation did not finished in the specified times.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WORK4, #PKCD_RSA_WWORK, #PKCD_RSA_PDT_1
 *    (Used by sub functions)
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPW, #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR,
 *    #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPS
 *    #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_RSA_PDT_1 must be configured beforehand.
 * \par Detailed description
 *  - Continue generating random numbers until a prime number is obtained.
 *  - If p_keygen->p_len != 0 (which means that the prime p has already been generated),
 *    this function returns #PKCD_RET_OK.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCD_RET_INTERNALERR.
 *  -# Set test_loop to 0.
 *  -# Set is_continued to #PKC_BOOL_TRUE.
 *  -# Set is_mod_p to #PKC_BOOL_TRUE.
 *  -# Set candidate_len to p_keygen->key_len/#PKCD_CMN_DIV_2.
 *  -# Set p_keygen->ck_offset to the byte size of the structure #st_rsa_check_pq_t.
 *  -# If (0U != p_keygen->p_len)
 *    -# Set ret to #PKCD_RET_OK.
 *  -# Else
 *    -# Do nothing.
 *  -# While ((0U == p_keygen->p_len) && (test_loop < p_keygen->prime_test_num) && (#PKC_BOOL_TRUE == is_continued))
 *     do the following:
 *    -# Generate a random number for a prime candidate at p_candidate->p_data with p_ctx->p_drbg_api
 *       and set external_ret to the result.
 *    -# If (#PKCD_CMN_EXTERNAL_API_OK == external_ret), then
 *      -# Check if the prime candidate satisfies the condition specified in FIPS186-4 Appendix B.3.1
 *         with #r_pkcd_rsa_check_p_q and set ret to the result.
 *      -# Set p_keygen->ck_offset to the byte size of the structure #st_rsa_check_pq_t.
 *      -# If #PKCD_RET_OK == ret, then
 *        -# Check if the prime candidate is a prime number with #r_pkcd_rsa_test_random_prime
 *           and set ret to the result.
 *        -# If #PKCD_RET_OK == ret, then
 *          -# Set p_keygen->p_len to candidate_len.
 *          -# Copy the candidate to p_keygen->p_data with #r_pkcd_cmn_cpy_word.
 *        -# Else if (#PKCD_RET_RSA_COMPOSITE == ret) then,
 *          -# test_loop++.
 *        -# Else
 *          -# Set is_continued to #PKC_BOOL_FALSE.
 *      -# Else if ((#PKCD_RET_RSA_RANGEERR == ret) || (#PKCD_RET_RSA_NON_COPRIME == ret)) then,
 *        -# test_loop++
 *      -# Else
 *        -# Do nothing.
 *    -# Else
 *      -# Set ret to #PKCD_RET_DRBGAPIERR.
 *      -# Set is_continued to #PKC_BOOL_FALSE.
 *  -# Clear the buffer for the prime number candidate with #r_pkcd_cmn_ram_clr.
 *  -# If ((0U == p_keygen->p_len) && (test_loop >= p_keygen->prime_test_num)
 *     && (#PKCD_RET_OK == ret)), then
 *    -# Set ret to #PKCD_RET_PRIME_GENERATING.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_generate_p(st_rsa_prm_keygen_t * const p_keygen, const st_pkc_rsa_ctx_t * const p_ctx,
                                            uint8_t * const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t test_loop = 0U;
    e_pkc_bool_t is_continued = PKC_BOOL_TRUE;
    uint32_t external_ret;
    const e_pkc_bool_t is_mod_p = PKC_BOOL_TRUE;
    const uint32_t candidate_len = p_keygen->key_len / PKCD_CMN_DIV_2;

    p_keygen->ck_offset = sizeof(st_rsa_check_pq_t);

    if (0U != p_keygen->p_len)
    {
        ret = PKCD_RET_OK;
    }
    else
    {
        /* Do nothing */
    }

    while ((0U == p_keygen->p_len) && (test_loop < p_keygen->prime_test_num) && (PKC_BOOL_TRUE == is_continued))
    {
        /* Call WDT callback */
        r_pkcd_rsa_call_wdtclr();

        /* Generate a prime candidate */
        external_ret = p_ctx->p_drbg_api(candidate_len, p_keygen->candidate);

        if (PKCD_CMN_EXTERNAL_API_OK == external_ret)
        {
                ret = r_pkcd_rsa_check_p_q(p_keygen->candidate, NULL, candidate_len,
                                        p_keygen->e_data, p_keygen->e_len, p_buf, is_mod_p);
            if (PKCD_RET_OK == ret)
            {
                /* Check if p_candidate is a prime number with Miller-Rabin test */
                ret = r_pkcd_rsa_test_random_prime(p_keygen, p_ctx, &p_buf[p_keygen->ck_offset]);

                if (PKCD_RET_OK == ret)
                {
                    r_pkcd_cmn_cpy_word(p_keygen->p_data, p_keygen->candidate, candidate_len);
                    p_keygen->p_len = candidate_len;
                }
                else if (PKCD_RET_RSA_COMPOSITE == ret)
                {
                    /* In the case of composite number, proceed to the next loop without setting values for p or q. */
                    ret = PKCD_RET_OK;
                    test_loop++;
                }
                else
                {
                    is_continued = PKC_BOOL_FALSE;
                }
            }
            else if ((PKCD_RET_RSA_RANGEERR == ret) || (PKCD_RET_RSA_NON_COPRIME == ret))
            {
                /* If p_cadidate is not within the proper range or p_cadidate and e are not relatively prime,
                proceed to the next loop without setting values for p and q. */
                ret = PKCD_RET_OK;
                test_loop++;
            }
            else
            {
                is_continued = PKC_BOOL_FALSE;
            }
        }
        else
        {
            ret = PKCD_RET_DRBGAPIERR;
            is_continued = PKC_BOOL_FALSE;
        }

    }

    /* Clear the prime number candidate and the buffer for r_pkcd_rsa_check_p_q. */
    r_pkcd_cmn_ram_clr(p_keygen->candidate, candidate_len);
    r_pkcd_cmn_ram_clr(p_buf, p_keygen->ck_offset);

    /* When the generation of q is incomplete and the number of trials is exceeded */
    if ((0U == p_keygen->p_len) && (test_loop >= p_keygen->prime_test_num) && (PKCD_RET_OK == ret))
    {
        ret = PKCD_RET_PRIME_GENERATING;
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_generate_p()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_generate_q
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Continue generating random numbers until a prime number is obtained.
 *              This process follows FIPS186-4 Appendix B.3.1.
 * \param       [in] p_keygen           An address of the buffer for the RSA key generation.
 * \param       [in] p_ctx              An address of the context for the RSA key generation.
 * \param       [in] p_buf              An address of the buffer for this driver's RSA key generation.
 * \return
 *  - #PKCD_RET_OK:                 Successful completion
 *  - #PKCD_RET_DRBGAPIERR:         External DRBG API returned error.
 *  - #PKCD_RET_INTERNALERR:        Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:     PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:            An error occurred while the PKCC operations.
 *  - #PKCD_RET_PRIME_GENERATING:   Prime generation did not finished in the specified times.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WORK4, #PKCD_RSA_WWORK, #PKCD_RSA_PDT_1
 *    (Used by sub functions)
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPW, #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR,
 *    #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC,#PKCD_CMN_ICUM_PKCCB0DPS
 *    #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_RSA_PDT_1 must be configured beforehand.
 * \par Detailed description
 *  - Continue generating random numbers until a prime number is obtained.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCD_RET_INTERNALERR.
 *  -# Set test_loop to 0.
 *  -# Set is_continued to #PKC_BOOL_TRUE.
 *  -# Set is_mod_p to #PKC_BOOL_FALSE.
 *  -# Set candidate_len to p_keygen->key_len/#PKCD_CMN_DIV_2.
 *  -# Set p_keygen->ck_offset to the byte size of the structure #st_rsa_check_pq_t.
 *  -# If (#PKC_BOOL_TRUE == is_mod_p), then
 *    -# Do nothing.
 *  -# Else
 *    -# Set p_pqlen to &p_keygen->q_len.
 *    -# Set p_pqdata to p_keygen->q_data.
 *  -# While ((0U == p_keygen->q_len) && (test_loop < p_keygen->prime_test_num) && (#PKC_BOOL_TRUE == is_continued))
 *     do the following:
 *    -# Generate a random number for a prime candidate at p_candidate->p_data with p_ctx->p_drbg_api
 *       and set external_ret to the result.
 *    -# If (#PKCD_CMN_EXTERNAL_API_OK == external_ret), then
 *      -# Check if the prime candidate satisfies the condition specified in FIPS186-4 Appendix B.3.1
 *         with #r_pkcd_rsa_check_p_q and set ret to the result.
 *      -# Set p_keygen->ck_offset to the byte size of the structure #st_rsa_check_pq_t.
 *      -# If #PKCD_RET_OK == ret, then
 *        -# Check if the prime candidate is a prime number with #r_pkcd_rsa_test_random_prime
 *           and set ret to the result.
 *        -# If #PKCD_RET_OK == ret, then
 *          -# Set p_keygen->q_len to candidate_len.
 *          -# Copy the candidate to p_keygen->q_data with #r_pkcd_cmn_cpy_word.
 *        -# Else if (#PKCD_RET_RSA_COMPOSITE == ret) then,
 *          -# test_loop++.
 *        -# Else
 *          -# Set is_continued to #PKC_BOOL_FALSE.
 *      -# Else if ((#PKCD_RET_RSA_RANGEERR == ret) || (#PKCD_RET_RSA_NON_COPRIME == ret)) then,
 *        -# test_loop++
 *      -# Else
 *        -# Do nothing.
 *    -# Else
 *      -# Set ret to #PKCD_RET_DRBGAPIERR.
 *      -# Set is_continued to #PKC_BOOL_FALSE.
 *  -# Clear the buffer for the prime number candidate with #r_pkcd_cmn_ram_clr.
 *  -# If ((0U == p_keygen->q_len) && (test_loop >= p_keygen->prime_test_num)
 *     && (#PKCD_RET_OK == ret)), then
 *    -# Set ret to #PKCD_RET_PRIME_GENERATING.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_generate_q(st_rsa_prm_keygen_t * const p_keygen, const st_pkc_rsa_ctx_t * const p_ctx,
                                            uint8_t * const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t test_loop = 0U;
    e_pkc_bool_t is_continued = PKC_BOOL_TRUE;
    uint32_t external_ret;
    const e_pkc_bool_t is_mod_p = PKC_BOOL_FALSE;
    const uint32_t candidate_len = p_keygen->key_len / PKCD_CMN_DIV_2;

    p_keygen->ck_offset = sizeof(st_rsa_check_pq_t);

    while ((0U == p_keygen->q_len) && (test_loop < p_keygen->prime_test_num) && (PKC_BOOL_TRUE == is_continued))
    {
        /* Call WDT callback */
        r_pkcd_rsa_call_wdtclr();

        /* Generate a prime candidate */
        external_ret = p_ctx->p_drbg_api(candidate_len, p_keygen->candidate);

        if (PKCD_CMN_EXTERNAL_API_OK == external_ret)
        {
            ret = r_pkcd_rsa_check_p_q(p_keygen->candidate, p_keygen->p_data, candidate_len,
                                        p_keygen->e_data, p_keygen->e_len, p_buf, is_mod_p);
            if (PKCD_RET_OK == ret)
            {
                /* Check if p_candidate is a prime number with Miller-Rabin test */
                ret = r_pkcd_rsa_test_random_prime(p_keygen, p_ctx, &p_buf[p_keygen->ck_offset]);

                if (PKCD_RET_OK == ret)
                {
                    r_pkcd_cmn_cpy_word(p_keygen->q_data, p_keygen->candidate, candidate_len);
                    p_keygen->q_len = candidate_len;
                }
                else if (PKCD_RET_RSA_COMPOSITE == ret)
                {
                    /* In the case of composite number, proceed to the next loop without setting values for p or q. */
                    ret = PKCD_RET_OK;
                    test_loop++;
                }
                else
                {
                    is_continued = PKC_BOOL_FALSE;
                }
            }
            else if ((PKCD_RET_RSA_RANGEERR == ret) || (PKCD_RET_RSA_NON_COPRIME == ret))
            {
                /* If p_cadidate is not within the proper range or p_cadidate and e are not relatively prime,
                proceed to the next loop without setting values for p and q. */
                ret = PKCD_RET_OK;
                test_loop++;
            }
            else
            {
                is_continued = PKC_BOOL_FALSE;
            }
        }
        else
        {
            ret = PKCD_RET_DRBGAPIERR;
            is_continued = PKC_BOOL_FALSE;
        }

    }

    /* Clear the prime number candidate and the buffer for r_pkcd_rsa_check_p_q. */
    r_pkcd_cmn_ram_clr(p_keygen->candidate, candidate_len);
    r_pkcd_cmn_ram_clr(p_buf, p_keygen->ck_offset);

    /* When the generation of q is incomplete and the number of trials is exceeded */
    if ((0U == p_keygen->q_len) && (test_loop >= p_keygen->prime_test_num) && (PKCD_RET_OK == ret))
    {
        ret = PKCD_RET_PRIME_GENERATING;
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_generate_q()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_test_random_prime
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Continue generating random numbers until a prime number is obtained.
 *              This process follows FIPS186-4 Appendix B.3.1.
 * \param       [in] p_keygen           An address of the buffer for the RSA key generation.
 * \param       [in] p_ctx              An address of the context for the RSA key generation.
 * \param       [in] p_buf              An address of the buffer for this driver's RSA key generation.
 * \return
 *  - #PKCD_RET_OK:                 Successful completion
 *  - #PKCD_RET_DRBGAPIERR:         External DRBG API returned error.
 *  - #PKCD_RET_INTERNALERR:        Internal processing failed.
 *  - #PKCD_RET_PARAMERR:           Invalid arguments entered.
 *  - #PKCD_RET_LENGTHERR:          Invalid argument lengths entered.
 *  - #PKCD_RET_PKCCTIMEOUTERR:     PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:            An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WORK4, #PKCD_RSA_WWORK, #PKCD_RSA_PDT_1
 *    (Used by sub functions)
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPW, #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR,
 *    #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC,#PKCD_CMN_ICUM_PKCCB0DPS
 *    #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Continue generating random numbers until a prime number is obtained.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCD_RET_INTERNALERR.
 *  -# Set rand_loop to 0.
 *  -# Set candidate_data.len to p_keygen->key_len/#PKCD_CMN_DIV_2.
 *  -# Set candidate_data.p_data to the prime candidate address p_keygen->cnadidate.
 *  -# While (rand_loop < p_keygen->iteration_num) do following,
 *    -# Set the test randoms address p_buf_test_randoms to &p_buf[rand_loop * p_candidate->len].
 *    -# Generate random numbers at p_buf_test_randoms with p_ctx->p_drbg_api and set external_ret to the result.
 *    -# If (#PKCD_CMN_EXTERNAL_API_OK == external_ret) then,
 *      -# Set p_keygen->p_test_random_address[rand_loop] to p_buf_test_randoms;
 *    -# Else
 *      -# Set ret to #PKCD_RET_DRBGAPIERR.
 *    -# rand_loop++
 *  -# If #PKCD_RET_OK == ret, then
 *    -# Check if the prime candidate is a prime number with #R_PKCD_RsaTestPrime.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_test_random_prime(st_rsa_prm_keygen_t * const p_keygen,
                                            const st_pkc_rsa_ctx_t * const p_ctx,
                                            uint8_t * const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t rand_loop = 0U;
    uint32_t external_ret;
    uint8_t * p_buf_test_randoms;
    const st_pkc_data_t candidate_data = {p_keygen->key_len / PKCD_CMN_DIV_2, p_keygen->candidate};

    /* Generate test_randoms */
    while (rand_loop < p_keygen->iteration_num)
    {
        p_buf_test_randoms = &p_buf[(rand_loop * candidate_data.len)];
        external_ret = p_ctx->p_drbg_api(candidate_data.len, p_buf_test_randoms);

        if (PKCD_CMN_EXTERNAL_API_OK == external_ret)
        {
            p_keygen->p_test_random_address[rand_loop] = p_buf_test_randoms;
            ret = PKCD_RET_OK;
        }
        else
        {
            ret = PKCD_RET_DRBGAPIERR;
            break;
        }

        rand_loop++;
    }

    if (PKCD_RET_OK == ret)
    {
        ret = R_PKCD_RsaTestPrime(p_ctx, &p_buf[(p_keygen->iteration_num * candidate_data.len)], &candidate_data,
                            p_keygen->iteration_num, p_keygen->p_test_random_address);
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_test_random_prime()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_check_p_q
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Check if a prime candidate given with p_pq satisfies the conditions
 *              defined in FIPS186-4 appendix B.3.1.
 * \param       [in]    p_pq        An address of a prime candidate p or q.
 * \param       [in]    p_p         An address of a prime (candidate) p.
 * \param       [in]    pqlen       The length in bytes of the prime candidate.
 * \param       [in]    p_e         An address of a public exponent e.
 * \param       [in]    e_len       The length in bytes of the public exponent e.
 * \param       [in]    p_buf       An address of the buffer for intermediate data.
 * \param       [in]    is_mod_p    Indicates if the value given with p_pq is a prime candidate p.
 * \return
 *  - #PKCD_RET_OK:                 Successful completion
 *  - #PKCD_RET_INTERNALERR:        Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:     PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:            An error occurred while the PKCC operations.
 *  - #PKCD_RET_RSA_RANGEERR:       The candidate did not satisfy some of the conditions.
 *                                  defined in FIPS186-4 appendix B.3.1.
 *  - #PKCD_RET_RSA_NON_COPRIME:    The candidate is not coprime to the public exponent e.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WORK3, #PKCD_RSA_WORK4, #PKCD_RSA_WWORK, #PKCD_RSA_PDT_1
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPW, #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC,
 *    #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS
 *    (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR must be configured beforehand.
 *  - #PKCD_RSA_PDT_1 must be configured beforehand.
 * \par Detailed description
 *  - Check if a prime candidate given with p_pq satisfies the conditions defined in FIPS186-4 appendix B.3.1.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCD_RET_INTERNALERR.
 *  -# Set nlen to #PKCD_CMN_MUL_2 * pqlen.
 *  -# Set the PKCC DL size to that of the key length with #r_pkcd_cmn_set_dl.
 *  -# Clear p_wk_pq(#PKCD_RSA_WORK1->half.l) with #r_pkcd_cmn_clr_work_unit, and set ret to the result.
 *  -# Clear p_wk_pq2(#PKCD_RSA_WORK2) with #r_pkcd_cmn_clr_work_unit, and add the result on ret.
 *  -# Copy the the prime candidate to p_wk_pq(#PKCD_RSA_WORK1->half.l) with #r_pkcd_cmn_cpy_by_64bounds.
 *  -# Set #PKCD_CMN_ICUM_PKCCB0DPW to #PKCD_RSA_FORMAT_DP_DPW(#PKCD_RSA_WWORK).
 *  -# Set #PKCD_CMN_ICUM_PKCCB0DPT to #PKCD_RSA_FORMAT_DP_DPT(#PKCD_RSA_PDT_1).
 *  -# Or the prime candidate at p_pq with 0x01.
 *  -# Merge the return codes added on ret in the highest-priority one using #r_pkcd_cmn_extract_result.
 *  -# If ((#PKC_BOOL_FALSE == is_mod_p) && (#PKCD_RET_OK == ret)), then
 *    -# Check q with #r_pkcd_rsa_check_q and set ret to the result.
 *  -# Else
 *    -# Do nothing.
 *  -# If #PKCD_RET_OK == ret, then
 *    -# Clear p_buf_ck_pq->for_check with #r_pkcd_cmn_ram_clr.
 *    -# Clear #PKCD_RSA_WORK3 with #r_pkcd_cmn_clr_work_unit.
 *    -# Set p_buf_ck_pq->for_check[0] = (uint8_t)((uint32_t)for_check2[0] | 0x80), that is setting the bit (nlen-1).
 *    -# Copy 2^(nlen-1) in for_check to #PKCD_RSA_WORK3 with #r_pkcd_cmn_cpy_by_64bounds.
 *    -# Calculate the square of the prime candidate with #r_pkcd_cmn_AB and add the result on ret.
 *    -# Set the PKCC DL to nlen and BDL pqlen with #r_pkcd_cmn_set_dl.
 *    -# Check if p^2 or q^2 >= 2^(nlen-1) with #r_pkcd_cmn_cmp_work_unit and add the result on ret.
 *       The comparison result is set to cmp_ret.
 *    -# Merge the return codes added on ret in the highest-priority one using #r_pkcd_cmn_extract_result.
 *    -# If #PKCD_RET_OK == ret, then
 *      -# If cmp_ret < 0U then,
 *        -# Set ret to #PKCD_RET_RSA_RANGEERR.
 *      -# Else
 *        -# Set the PKCC DL and BDL to pqlen with #r_pkcd_cmn_set_dl.
 *        -# Copy the prime candidate p or q to p_buf_ck_pq->pq1 with #r_pkcd_cmn_cpy_byte.
 *        -# Mask p_buf_ck_pq->pq1[pqlen - 1U] with 0xFE and make (p - 1) or (q - 1).
 *        -# Clear #PKCD_RSA_PDT_1 with #r_pkcd_cmn_clr_work_unit and set ret to the result.
 *        -# Clear #PKCD_RSA_WORK3->half.h with #r_pkcd_cmn_clr_work_unit and add the result on ret.
 *        -# Copy (p - 1) or (q - 1) at p_buf_ck_pq->pq1 to #PKCD_RSA_WORK3->half.h with #r_pkcd_cmn_cpy_by_64bounds.
 *        -# Clear #PKCD_RSA_WORK3->half.l with #r_pkcd_cmn_clr_work_unit and add the result on ret.
 *        -# Copy the pulic exponent e at p_e to #PKCD_RSA_WORK3->half.l with #r_pkcd_cmn_cpy_by_64bounds.
 *        -# Clear #PKCD_RSA_WORK4->half.h with #r_pkcd_cmn_clr_work_unit and add the result on ret.
 *        -# Copy the pre-calculated -e^(-1) mod 2^64 in minus_einv to #PKCD_RSA_PDT_1
 *           with #r_pkcd_cmn_cpy_by_64bounds.
 *        -# Calculate the Montogomery reduction of (p-1) or (q-1) by e with #r_pkcd_cmn_ARInvModC
 *           and add the result on ret.
 *        -# Check if the Montogomary reduction is 0 with #r_pkcd_cmn_cmp_zero_word and set cmp_ret to the result.
 *        -# Merge the return codes added on ret in the highest-priority one using #r_pkcd_cmn_extract_result.
 *        -# If ((0 == cmp_ret) && (#PKCD_RET_OK == ret)) then,
 *          -# Set ret to #PKCD_RET_RSA_NON_COPRIME.
 *        -# Else
 *          -# Do nothing.
 *    -# Else
 *      -# Do nothing.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_check_p_q(uint8_t * const p_pq, const uint8_t * const p_p, const uint32_t pqlen,
                                        const uint8_t * const p_e, const uint32_t e_len, uint8_t * const p_buf,
                                        const e_pkc_bool_t is_mod_p)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint8_t * const p_wk_pq        = PKCD_RSA_WORK1->half.l;
    const uint8_t * const p_wk_pq2       = PKCD_RSA_WORK2->full;
    const uint32_t nlen = PKCD_CMN_MUL_2 * pqlen;
    int32_t cmp_ret;
    const uint8_t minus_einv[PKCD_RSA_WORKUNITPDT] = {0x00U, 0x00U, 0xFFU, 0xFFU, 0x00U, 0x00U, 0xFFU, 0xFFU};
    st_rsa_check_pq_t * const p_buf_ck_pq = (st_rsa_check_pq_t*)p_buf; /* No problem with casting to pointer size */

    /* Prepare the operation */
    r_pkcd_cmn_set_dl(nlen, 0U);
    ret = r_pkcd_cmn_clr_work_unit(PKCD_RSA_WORK1->full, PKCD_RSA_WORKUNIT);
    ret |= r_pkcd_cmn_clr_work_unit(p_wk_pq2, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(p_wk_pq + PKCD_RSA_OFFSET_VALHALF(pqlen), p_pq, pqlen);
    PKCD_CMN_ICUM_PKCCB0DPW = PKCD_RSA_FORMAT_DP_DPW(PKCD_RSA_WWORK);
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_RSA_FORMAT_DP_DPT(PKCD_RSA_PDT_1);

    /* If p_pq is an even number, then p_pq = p_pq + 1 */
    p_pq[pqlen - 1U] = (uint8_t)((uint32_t)p_pq[pqlen - 1U] | (uint32_t)PKCD_RSA_MASK_01);
    ret = r_pkcd_cmn_extract_result(ret);

    /* When generating q, check if |p_pq - p_p| > 2^(nlen/2-100) */
    if ((PKC_BOOL_FALSE == is_mod_p) && (PKCD_RET_OK == ret))
    {
        ret = r_pkcd_rsa_check_q(p_wk_pq, p_p, pqlen, PKCD_RSA_WORK3, PKCD_RSA_WORK4, p_buf_ck_pq->for_check);
    }
    else
    {
        /* Do nothing */
    }

    if (PKCD_RET_OK == ret)
    {
        /* Set WORK3 to 2^(nlen-1) */
        r_pkcd_cmn_set_dl(pqlen, 0U);
        r_pkcd_cmn_ram_clr(p_buf_ck_pq->for_check, nlen);
        ret = r_pkcd_cmn_clr_work_unit(PKCD_RSA_WORK3->half.h, PKCD_RSA_WORKUNITHALF);
        ret |= r_pkcd_cmn_clr_work_unit(PKCD_RSA_WORK3->half.l, PKCD_RSA_WORKUNITHALF);
        p_buf_ck_pq->for_check[0U] =
            /* No problem for casting to support usual arithmetic conversion */
            (uint8_t)((uint32_t)p_buf_ck_pq->for_check[0U] | (uint32_t)PKCD_RSA_MASK_80);
        r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK3->full + PKCD_RSA_OFFSET_VAL(nlen), p_buf_ck_pq->for_check, nlen);

        /* Check if p^2(q^2) >= 2^(nlen-1) */
        ret |= r_pkcd_cmn_AB(p_wk_pq, p_wk_pq, p_wk_pq2, PKCD_RSA_WORKUNITHALF);
        r_pkcd_cmn_set_dl(nlen, 0U);
        ret |= r_pkcd_cmn_cmp_work_unit(p_wk_pq2, PKCD_RSA_WORK3->full,
                                        PKCD_RSA_WORK4->full, nlen, PKCD_RSA_WORKUNIT, &cmp_ret);
        ret = r_pkcd_cmn_extract_result(ret);

        if (PKCD_RET_OK == ret)
        {
            if (cmp_ret < 0)
            {
                ret = PKCD_RET_RSA_RANGEERR;
            }
            else
            {
                /* Check if p-1 and e are relatively prime */
                r_pkcd_cmn_set_dl(pqlen, pqlen);
                r_pkcd_cmn_cpy_word(p_buf_ck_pq->pq1, p_pq, pqlen);
                p_buf_ck_pq->pq1[pqlen - 1U] =
                /* No problem for casting to support usual arithmetic conversion */
                (uint8_t)((uint32_t)p_buf_ck_pq->pq1[pqlen - 1U] & (uint32_t)PKCD_RSA_MASK_FE);
                ret = r_pkcd_cmn_clr_work_unit(PKCD_RSA_WORK3->half.h, PKCD_RSA_WORKUNITHALF);
                r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK3->half.h + PKCD_RSA_OFFSET_VALHALF(pqlen),
                                            p_buf_ck_pq->pq1, pqlen);
                ret |= r_pkcd_cmn_clr_work_unit(PKCD_RSA_WORK3->half.l, PKCD_RSA_WORKUNITHALF);
                r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK3->half.l + PKCD_RSA_OFFSET_VALHALF(e_len), p_e, e_len);
                ret |= r_pkcd_cmn_clr_work_unit(PKCD_RSA_WORK4->half.h, PKCD_RSA_WORKUNITHALF);
                r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_PDT_1, minus_einv, PKCD_RSA_WORKUNITPDT);
                ret |= r_pkcd_cmn_ARInvModC(PKCD_RSA_WORK3->half.h, PKCD_RSA_WORK3->half.l,
                                            PKCD_RSA_WORK4->half.h, PKCD_RSA_WORKUNITHALF);
                cmp_ret = r_pkcd_cmn_cmp_zero_word(PKCD_RSA_WORK4->half.h
                                                    + PKCD_RSA_OFFSET_VALHALF(PKCD_CMN_PKCCBOUNDARYSIZE),
                                                    PKCD_CMN_PKCCBOUNDARYSIZE);
                ret = r_pkcd_cmn_extract_result(ret);

                if ((0 == cmp_ret) && (PKCD_RET_OK == ret))
                {
                    ret = PKCD_RET_RSA_NON_COPRIME;
                }
                else
                {
                    /* Do nothing */
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

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_check_p_q()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_check_q
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Check if a prime candidate given with p_pq is a prime number
 *              and satisfies the conditions defined in FIPS186-4 appendix B.3.1.
 * \param       [in]    p_wk_q      An address of a work area for a prime q.
 * \param       [in]    p_p         An address of a prime (candidate) p.
 * \param       [in]    pqlen       The length in bytes of the prime candidate.
 * \param       [in]    p_wk1       An address of a work area.
 * \param       [in]    p_wk2       An address of a work area.
 * \param       [in]    p_buf       An address of a buffer used for the internal processing of PKC-Driver.
 * \return
 *  - #PKCD_RET_OK:                 Successful completion
 *  - #PKCD_RET_INTERNALERR:        Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:     PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:            An error occurred while the PKCC operations.
 *  - #PKCD_RET_RSA_RANGEERR:       The candidate did not satisfy some of the conditions.
 *                                  defined in FIPS186-4 appendix B.3.1.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC,
 *    #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS
 *    (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR (DL bit) value must have been set beforehand.
 * \par Detailed description
 *  - Check if a prime candidate given with p_pq is a prime number
 *    and satisfies the conditions defined in FIPS186-4 appendix B.3.1.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCD_RET_INTERNALERR.
 *  -# Clear p_wk1 with #r_pkcd_cmn_clr_work_unit.
 *  -# Clear p_wk2 with #r_pkcd_cmn_clr_work_unit.
 *  -# Copy the prime candidate at p_p to p_wk2->half.l with #r_pkcd_cmn_cpy_by_64bounds.
 *  -# Calculate p_wk1->half.l = p - q with #r_pkcd_cmn_AMinusB and set ret to the result.
 *  -# Copy the output sign bits in p_wk1->half.l to sign_trial with #r_pkcd_cmn_cpy_by_64bounds.
 *  -# Calculate p_wk1->half.h = q - p with #r_pkcd_cmn_AMinusB and accumlate the result on ret.
 *  -# Copy the output sign bits in p_wk1->half.h to sign_trial2 with #r_pkcd_cmn_cpy_by_64bounds.
 *  -# If (sign_trial[#PKCD_CMN_LSB_POS] == 0) and (sign_trial2[#PKCD_CMN_LSB_POS] != 0), that is p > q, then
 *    -# Copy (p - q) in p_wk1->half.l to p_wk2->half.h with #r_pkcd_cmn_cpy_word.
 *  -# Else if (sign_trial[#PKCD_CMN_LSB_POS] != 0) and (sign_trial2[#PKCD_CMN_LSB_POS] == 0), that is p < q, then
 *    -# Copy (q - p) in p_wk1->half.h to p_wk2->half.h with #r_pkcd_cmn_cpy_word.
 *  -# Else
 *    -# Do nothing.
 *  -# Set the PKCC DL and BDL to pqlen with #r_pkcd_cmn_set_dl.
 *  -# Clear p_wk2->half.l with #r_pkcd_cmn_clr_work_unit.
 *  -# Clear p_buf with #r_pkcd_cmn_ram_clr.
 *  -# Set p_buf[12U] to 0x10, that is setting the bit (nlen/2 - 100).
 *  -# Copy 2^(nlen/2-100) in p_buf to p_wk2->half.l with #r_pkcd_cmn_cpy_by_64bounds.
 *  -# Check if |p-q| > 2^(nlen/2 - 100) with #r_pkcd_cmn_cmp_work_unit and set ret to the result.
 *     The comparison result is set to cmp_ret.
 *  -# Merge the return codes added on ret in the highest-priority one using #r_pkcd_cmn_extract_result.
 *  -# If #PKCD_RET_OK == ret then,
 *    -# If (cmp_ret <= 0) then,
 *      -# Set ret to #PKCD_RET_RSA_RANGEERR.
 *    -# Else
 *      -# Do nothing.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_check_q(uint8_t * const p_wk_q, const uint8_t * const p_p, const uint32_t pqlen,
                                    u_rsa_work_t * const p_wk1, u_rsa_work_t * const p_wk2, uint8_t * const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint8_t sign_trial[PKCD_CMN_PKCCBOUNDARYSIZE];
    uint8_t sign_trial2[PKCD_CMN_PKCCBOUNDARYSIZE];
    int32_t cmp_ret;

    /* Clear the buffer */
    r_pkcd_cmn_set_dl(pqlen * PKCD_CMN_MUL_2, 0U);
    ret = r_pkcd_cmn_clr_work_unit(p_wk1->full, PKCD_RSA_WORKUNIT);
    ret |= r_pkcd_cmn_clr_work_unit(p_wk2->full, PKCD_RSA_WORKUNIT);

    /* Set p_wk2->half.l to p_p */
    r_pkcd_cmn_cpy_by_64bounds(p_wk2->half.l + PKCD_RSA_OFFSET_VALHALF(pqlen), p_p, pqlen);

    /* Set p_wk1->half.l to p_p - p_pq */
    r_pkcd_cmn_set_dl(pqlen, 0U);
    ret |= r_pkcd_cmn_AMinusB(p_wk2->half.l, p_wk_q, p_wk1->half.l, PKCD_RSA_WORKUNITHALF);
    r_pkcd_cmn_cpy_by_64bounds(sign_trial, p_wk1->half.l + ((PKCD_RSA_WORKUNITHALF - pqlen)
                                - PKCD_CMN_PKCCBOUNDARYSIZE), PKCD_CMN_PKCCBOUNDARYSIZE);

    /* Set p_wk1->half.h to p_pq - p_p */
    ret |= r_pkcd_cmn_AMinusB(p_wk_q, p_wk2->half.l, p_wk1->half.h, PKCD_RSA_WORKUNITHALF);
    r_pkcd_cmn_cpy_by_64bounds(sign_trial2, p_wk1->half.h + ((PKCD_RSA_WORKUNITHALF - pqlen)
                                - PKCD_CMN_PKCCBOUNDARYSIZE), PKCD_CMN_PKCCBOUNDARYSIZE);

    /* Set p_wk2->half.h to |p_pq - p_p| */
    if (((uint32_t)sign_trial[PKCD_CMN_LSB_POS] == 0U) && ((uint32_t)sign_trial2[PKCD_CMN_LSB_POS] != 0U))
    {
        r_pkcd_cmn_cpy_word(p_wk2->half.h, p_wk1->half.l, PKCD_RSA_WORKUNITHALF);
    }
    /* No problem for casting to support usual arithmetic conversion */
    else if (((uint32_t)sign_trial[PKCD_CMN_LSB_POS] != 0U) && ((uint32_t)sign_trial2[PKCD_CMN_LSB_POS] == 0U))
    {
        r_pkcd_cmn_cpy_word(p_wk2->half.h, p_wk1->half.h, PKCD_RSA_WORKUNITHALF);
    }
    else
    {
        /* Do nothing */
    }

    /* Set p_wk2->half.l to 2^(nlen/2-100) */
    r_pkcd_cmn_set_dl(pqlen, 0U);
    ret |= r_pkcd_cmn_clr_work_unit(p_wk2->half.l, PKCD_RSA_WORKUNITHALF);
    r_pkcd_cmn_ram_clr(p_buf, pqlen);
    p_buf[PKCD_RSA_MODMINUS100_BYTE_POS] = (uint8_t)PKCD_RSA_MASK_10; /* No problem for casting to support
                                                                        usual arithmetic conversion */
    r_pkcd_cmn_cpy_by_64bounds(p_wk2->half.l + PKCD_RSA_OFFSET_VALHALF(pqlen),
                                p_buf, pqlen);

    /* Check if |p_pq - p_p| > 2^(nlen/2-100) */
    ret |= r_pkcd_cmn_cmp_work_unit(p_wk2->half.h, p_wk2->half.l,
                                    p_wk1->half.l, pqlen, PKCD_RSA_WORKUNITHALF, &cmp_ret);
    ret = r_pkcd_cmn_extract_result(ret);

    if (PKCD_RET_OK == ret)
    {
        if (cmp_ret <= 0)
        {
            ret = PKCD_RET_RSA_RANGEERR;
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
/***********************************************************************************************************************
* End of function r_pkcd_rsa_check_q()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_generate_n_d
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Calculate modulus n and private key d of RSA based on the method of Appendix B.3 of FIPS 186-4.
 * \param       [in,out]  p_key_bin     An RSA key in the key binary format.
 * \param       [in]      p_buf         An address of key generate buffer.
 * \return
 *  - #PKCD_RET_OK:                 Successfully done.
 *  - #PKCD_RET_PKCCTIMEOUTERR:     PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:            An error occurred while the PKCC operations.
 *  - #PKCD_RET_RSA_NON_COPRIME:    a and m are not co-prime
 *  - #PKCD_RET_RSA_RANGEERR:       d did not satisfied the condition specified in FIPS186-4 Appendix B3.1.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_RSA_WORK1, #PKCD_RSA_WORK2, #PKCD_RSA_WWORK
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPW, #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR,
 *    #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPS
 *    #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Calculate private key d of RSA based on the method of Appendix B.3 of FIPS 186-4.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCD_RET_INTERNALERR.
 *  -# Set p_d1 to p_key_bin->p_d.
 *  -# Set p_d2 = &p_key_bin->p_d[p_key_bin->key_length/#PKCD_CMN_DIV_2].
 *  -# Set d_halflen to p_key_bin->key_length/#PKCD_CMN_DIV_2
 *  -# Assign the buffer for the driver's key generation as p_keygen_buf from p_buf.
 *  -# Copy the prime p at p_key_bin->p_p to p_keygen_buf->p1 with #r_pkcd_cmn_cpy_word.
 *  -# Mask p_keygen_buf->p1[d_halflen - 1U] with 0xFE to get p - 1.
 *  -# Copy the prime q at p_key_bin->p_q to p_keygen_buf->q1 with #r_pkcd_cmn_cpy_word.
 *  -# Mask p_keygen_buf->q1[d_halflen - 1U] with 0xFE to get q - 1.
 *  -# Set the PKCC DL to the key length with #r_pkcd_cmn_set_dl.
 *  -# Clear #PKCD_RSA_WORK1 with #r_pkcd_cmn_clr_work_unit, and set ret to the result.
 *  -# Clear #PKCD_RSA_WORK2 with #r_pkcd_cmn_clr_work_unit, and add the result on ret.
 *  -# Copy p at p_key_bin->p_p to #PKCD_RSA_WORK1->half.h with #r_pkcd_cmn_cpy_by_64bounds.
 *  -# Copy q at p_key_bin->p_q to #PKCD_RSA_WORK2->half.h with #r_pkcd_cmn_cpy_by_64bounds.
 *  -# Set the PKCC DL to d_halflen with #r_pkcd_cmn_set_dl.
 *  -# Calculate modulus n = p * q with the #r_pkcd_cmn_AB and set ret to the result.
 *  -# Merge the return codes added on ret in the highest-priority one using #r_pkcd_cmn_extract_result.
 *  -# If #PKCD_RET_OK == ret, then
 *    -# Copy the calculated n to p_key_bin->p_n with #r_pkcd_cmn_cpy_by_64bounds.
 *    -# Calculate a private exponent d from the prime p, q with #r_pkcd_rsa_calc_key_d and set ret to the result.
 *  -# Else
 *    -# Do nothing.
 *  -# If #PKCD_RET_OK == ret, then
 *    -# Check if the upper half of d at p_d1 is 0 with  #r_pkcd_cmn_cmp_zero_word. And set cmp_ret to the result.
 *    -# Check if the lower half of d at p_d2 is 0 with  #r_pkcd_cmn_cmp_zero_word. And set cmp_ret2 to the result.
 *    -# Check if the upper half of d out of its LSByte at p_d1 is 0
 *       with  #r_pkcd_cmn_cmp_zero_word. And set cmp_ret3 to the result.
 *    -# Check if the LSByte of the upper half of d is 0x01
 *       with #r_pkcd_cmn_cmp_word. And set cmp_ret4 to the result.
 *    -# If ((0 == cmp_ret) || ((0 == cmp_ret2) && (0 == cmp_ret3) && (0 == cmp_ret4))) that is
 *       d > 2^(key length/2) then,
 *      -# Set ret to #PKCD_RET_RSA_RANGEERR.
 *    -# Else
 *      -# Do nothing.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
static pkcd_ret_t r_pkcd_rsa_generate_n_d(st_pkcd_rsa_key_bin_t* const p_key_bin,
                                        uint8_t* const p_buf)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const uint8_t * const p_d1 = p_key_bin->p_d;
    const uint8_t * const p_d2 = &p_key_bin->p_d[p_key_bin->key_length / PKCD_CMN_DIV_2];
    int32_t cmp_ret;
    int32_t cmp_ret2;
    int32_t cmp_ret3;
    int32_t cmp_ret4;
    const uint32_t d_halflen = p_key_bin->key_length / PKCD_CMN_DIV_2;
    st_rsa_keygen_buffer_t * const p_keygen_buf = (st_rsa_keygen_buffer_t*)p_buf; /* No problem with casting
                                                                                    to pointer size */

    /* make p-1 */
    r_pkcd_cmn_cpy_word(p_keygen_buf->p1, p_key_bin->p_p, d_halflen);
    p_keygen_buf->p1[d_halflen - 1U] =
        /* No problem for casting to support usual arithmetic conversion */
        (uint8_t)((uint32_t)p_keygen_buf->p1[d_halflen - 1U] & (uint32_t)PKCD_RSA_MASK_FE);

    /* make q-1 */
    r_pkcd_cmn_cpy_word(p_keygen_buf->q1, p_key_bin->p_q, d_halflen);
    p_keygen_buf->q1[d_halflen - 1U] =
        /* No problem for casting to support usual arithmetic conversion */
        (uint8_t)((uint32_t)p_keygen_buf->q1[d_halflen - 1U] & (uint32_t)PKCD_RSA_MASK_FE);

    /* set DPW */
    PKCD_CMN_ICUM_PKCCB0DPW = PKCD_RSA_FORMAT_DP_DPW(PKCD_RSA_WWORK);

    /* calc modulus n */
    r_pkcd_cmn_set_dl(p_key_bin->key_length, 0U);
    ret  = r_pkcd_cmn_clr_work_unit(PKCD_RSA_WORK1->full, PKCD_RSA_WORKUNIT);
    ret |= r_pkcd_cmn_clr_work_unit(PKCD_RSA_WORK2->full, PKCD_RSA_WORKUNIT);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK1->half.h + PKCD_RSA_OFFSET_VALHALF(d_halflen),
                                p_key_bin->p_p, d_halflen);
    r_pkcd_cmn_cpy_by_64bounds(PKCD_RSA_WORK1->half.l + PKCD_RSA_OFFSET_VALHALF(d_halflen),
                                p_key_bin->p_q, d_halflen);
    r_pkcd_cmn_set_dl(d_halflen, 0U);
    ret |= r_pkcd_cmn_AB(PKCD_RSA_WORK1->half.h, PKCD_RSA_WORK1->half.l, PKCD_RSA_WORK2->full,
                        PKCD_RSA_WORKUNITHALF);
    ret = r_pkcd_cmn_extract_result(ret);

    if (PKCD_RET_OK == ret)
    {
        r_pkcd_cmn_cpy_by_64bounds(p_key_bin->p_n, PKCD_RSA_WORK2->full + PKCD_RSA_OFFSET_VAL(p_key_bin->key_length),
                                    p_key_bin->key_length);
        ret = r_pkcd_rsa_calc_key_d(p_key_bin, p_keygen_buf);
    }
    else
    {
        /* Do nothing */
    }

    if (PKCD_RET_OK == ret)
    {
        /* Check if d > 2^(key length/2) */
        cmp_ret  = r_pkcd_cmn_cmp_zero_word(p_d1, d_halflen);
        cmp_ret2 = r_pkcd_cmn_cmp_zero_word(p_d2, d_halflen);
        cmp_ret3 = r_pkcd_cmn_cmp_zero_word(p_d1, d_halflen - (sizeof(g_rsa_one)));
        cmp_ret4 = r_pkcd_cmn_cmp_word(p_d1 + (d_halflen - (sizeof(g_rsa_one))), g_rsa_one, sizeof(g_rsa_one));

        /* When p_d1==0 or (p_d1==1 and p_d2==0), set ret to PKCD_RET_RSA_RANGEERR */
        if ((0 == cmp_ret) || ((0 == cmp_ret2) && (0 == cmp_ret3) && (0 == cmp_ret4)))
        {
            ret = PKCD_RET_RSA_RANGEERR;
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
/***********************************************************************************************************************
* End of function r_pkcd_rsa_generate_n_d()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_rsa_call_wdtclr
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIsInternal
 * \brief       Call WDT clear api.
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
 *  - Call WDT clear api.
 * \par Remarks
 *  - None
 *********************************************************************************************************************/
static inline void r_pkcd_rsa_call_wdtclr(void)
{
    if (NULL != sp_rsa_wdtclr_api)
    {
        sp_rsa_wdtclr_api();
    }
    else
    {
        /* Do nothing */
    }
}
/***********************************************************************************************************************
* End of function r_pkcd_rsa_call_wdtclr()
**********************************************************************************************************************/
