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

#ifndef R_PKCD_RSA_H
/* Multiple inclusion protection macro */
#define R_PKCD_RSA_H

/* PRQA S 0776 ++ # D012 Develop according to C99 */
/* PRQA S 3453,3472 ++ # D026 If it is a function, overhead is large, so it corresponds with macro. */
/* PRQA S 3630 ++ # D028 Header file used only by the library */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDRSADefinesInternal
 * \{******************************************************************/
#define PKCD_RSA_WORKUNITPDT                 (0x00000008U) /**< A size of 8byte PDT area */
#define PKCD_RSA_WORKUNIT                    (0x00000220U) /**< A size of 544byte work area */
#define PKCD_RSA_WORKUNITHALF                (0x00000110U) /**< A size of 272byte work area */
#define PKCD_RSA_WORKUNITPDW                 (0x00000660U) /**< A size of 1632byte PDW area */

/** Convert pointer shifted by 8bytes to PKCC data pointer register format */
#define PKCD_RSA_FORMAT_DP_DPT(top)     (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_RSA_WORKUNITPDT))
/** Convert pointer shifted by 272bytes to PKCC data pointer register format */
#define PKCD_RSA_FORMAT_DP_UNIT(top)    (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_RSA_WORKUNIT))
/** Convert pointer shifted by 816bytes to PKCC data pointer register format */
#define PKCD_RSA_FORMAT_DP_DPW(top)     (PKCD_CMN_FORMAT_DP((const uint8_t*)(top) + PKCD_RSA_WORKUNITPDW))

/** Offset to value of unitsize work area */
#define PKCD_RSA_OFFSET_VAL(val_size)        (PKCD_RSA_WORKUNIT     - (val_size))
/** Offset to value of half unitsize work area */
#define PKCD_RSA_OFFSET_VALHALF(val_size)    (PKCD_RSA_WORKUNITHALF - (val_size))

#define PKCD_RSA_MOD_LEN_MAX                (512U)    /**< Maximum length of modulus */
#define PKCD_RSA_MSG_LEN_MAX                (PKCD_RSA_MOD_LEN_MAX) /**< Maximum length of message */
#define PKCD_RSA_EXP_LEN_MAX                (PKCD_RSA_MOD_LEN_MAX) /**< Maximum length of exponent */

/** Maximum length of prime */
#define PKCD_RSA_PQ_LEN_MAX                 (PKCD_RSA_MOD_LEN_MAX/PKCD_CMN_DIV_2)

#define PKCD_RSA_PRIME_TBL_SIZE             (1024U)   /**< Prime table size */

#define PKCD_RSA_CRT_OPERATE_NOT_CALLED     (0U)   /**< ModExp CRT Operate API not called */
#define PKCD_RSA_CRT_OPERATE_CALLED_ONCE    (1U)   /**< ModExp CRT Operate API called once */
#define PKCD_RSA_CRT_OPERATE_CALLED_TWICE   (2U)   /**< ModExp CRT Operate API called twice */

#define PKCD_RSA_MASK_FE                    (0xFEU)    /**< the value used for clear LSbit */

#define PKCD_RSA_MASK_80                    (0x80U)    /**< the value used for bit mask */
#define PKCD_RSA_MASK_01                    (0x01U)    /**< the value used for setting LSbit */
#define PKCD_RSA_MASK_10                    (0x10U)    /**< the value used for setting the 4th bit from the top */

#define PKCD_RSA_MODMINUS100_BYTE_POS       (0x0CU)    /**< the value used for caluculating 2^(mod size - 100) */
/*! \}*/

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDRSATypesInternal
 * \{******************************************************************/
/** montgomery reduction table member structure */
typedef struct
{
    uint32_t h;             /**< Data of higher 4 byte */
    uint32_t l;             /**< Data of lower 4 byte */
} st_rsa_mont_reduction_table_t;

/** A parameter structure of Modular exponentiation */
typedef struct
{
    uint8_t         msg[PKCD_RSA_MSG_LEN_MAX];      /**< A message */
    uint32_t        msg_len;                    /**< A byte length of message */
    uint8_t         exp[PKCD_RSA_EXP_LEN_MAX];      /**< Exponent of secret key or public key */
    uint32_t        exp_len;                    /**< A byte length of exponent */
    uint8_t         mod[PKCD_RSA_MOD_LEN_MAX];      /**< A modulus */
    uint32_t        mod_len;                    /**< A byte length of modulus */
    e_pkc_bool_t    is_constant;                /**< Constant time mode in exponentiation (use if 2048 or less) */
    uint8_t *       p_buf;                      /**< A buffer pointer for operation (use if larger than 2048) */
    uint32_t        exp_bits;                   /**< Number of exponent bits */
    uint32_t        remaining_bits;             /**< Number of bits for which the binary method has NOT been executed */
    uint8_t         p1[PKCD_RSA_PQ_LEN_MAX];    /**< A prime p (used when the sec_mod_exp flag is TRUE) */
    uint8_t         q1[PKCD_RSA_PQ_LEN_MAX];    /**< A prime q (used when the sec_mod_exp flag is TRUE) */
    uint32_t        pq_len;                     /**< A byte size of prime (used when the sec_mod_exp flag is TRUE */
    uint8_t         random[PKCD_CMN_BITS64];    /**< 64-bit random data to be used in modular exponentiation
                                                        (used when the sec_mod_exp flag is TRUE)*/
} st_rsa_prm_modexp_t;

/** A parameter structure of Modular exponentiation with the Chinese remainder theorem */
typedef struct
{
    uint8_t         msg[PKCD_RSA_MSG_LEN_MAX];      /**< A message */
    uint32_t        msg_len;                    /**< A byte length of message */
    uint32_t        mod_len;                    /**< A byte length of modulus */
    uint8_t         p[PKCD_RSA_PQ_LEN_MAX];         /**< A prime p */
    uint8_t         q[PKCD_RSA_PQ_LEN_MAX];         /**< A prime q */
    uint8_t         dp[PKCD_RSA_PQ_LEN_MAX];        /**< An exponent dp */
    uint8_t         dq[PKCD_RSA_PQ_LEN_MAX];        /**< An exponent dq */
    uint8_t         qinv[PKCD_RSA_PQ_LEN_MAX];      /**< A cofactor qinv */
    uint32_t        pq_len;                    /**< A byte length of prime */
    uint32_t        operate_call_cnt;          /**< Operate API call count */
    uint8_t         random_p[PKCD_CMN_BITS64]; /**< 64-bit random data to be used in modular exponentiation */
    uint8_t         random_q[PKCD_CMN_BITS64]; /**< 64-bit random data to be used in modular exponentiation */
    uint8_t *       p_buf;                     /**< A buffer pointer for operation (use if larger than 2048) */
} st_rsa_prm_modexp_crt_t;

/** Work area structure for RSA operation */
typedef union
{
    uint8_t         full[PKCD_RSA_WORKUNIT];        /**< All of this structure */
    struct
    {
        uint8_t     h[PKCD_RSA_WORKUNITHALF];       /**< The 1st half of this structure */
        uint8_t     l[PKCD_RSA_WORKUNITHALF];       /**< The 2nd half of this structure */
    } half;                                             /**< Access this structure by half size */
} u_rsa_work_t;

/** Work area structure of PKCC RAM */
typedef struct
{
    uint8_t      pdt_1[PKCD_RSA_WORKUNITPDT];       /**< A work area for T(mod p) */
    uint8_t      pdt_2[PKCD_RSA_WORKUNITPDT];       /**< A work area for T mod q */
    u_rsa_work_t work1;                                 /**< A work area for 1 */
    u_rsa_work_t work2;                                 /**< A work area for 2 */
    u_rsa_work_t work3;                                 /**< A work area for 3 */
    u_rsa_work_t work4;                                 /**< A work area for 4 */
    uint8_t      wwork[PKCD_RSA_WORKUNITPDW];       /**< A work area for DPW */
} st_rsa_pkccram_t;

/*! \}*/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
extern const uint32_t g_rsa_prime_table[PKCD_RSA_PRIME_TBL_SIZE];   /**< array of 16bit small primes */

/** value T of each small prime C */
extern const st_rsa_mont_reduction_table_t g_rsa_mont_reduction_table[PKCD_RSA_PRIME_TBL_SIZE];
extern const uint8_t g_rsa_one[PKCD_CMN_PKCCBOUNDARYSIZE];   /**< Data of 1 in PKCC Boundary size(8byte) */

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern pkcd_ret_t r_pkcd_rsa_gcd (const uint8_t * const p_x, const uint32_t xlen, const uint8_t * const p_y,
                                    const uint32_t ylen, uint8_t * const p_v, uint8_t * const p_D);
extern pkcd_ret_t r_pkcd_rsa_inverse (const uint8_t * const p_a, const uint32_t alen, const uint8_t * const p_m,
                                        const uint32_t mlen, uint8_t * const p_ainv, uint8_t * const p_buf);

#endif /* R_PKCD_RSA_H */
