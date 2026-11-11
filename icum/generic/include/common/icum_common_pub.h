/******************************************************************************/
/* Component Name   common header file                                        */
/******************************************************************************/
/* Product      : ICU-M Low Level Driver (Demo Part)                          */
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

#ifndef _ER_ICUM_COMMON_H_
#define _ER_ICUM_COMMON_H_

/**********************************************************************************
* Header files
***********************************************************************************/
#include "renesas_types.h"

/**********************************************************************************
* Common error
***********************************************************************************/
#define ER_OK                     (0x00000000uL)
#define ER_PARAM                  (0x00000001uL)
#define ER_PENDING_ARB            (0x00000002uL)
#define ER_PENDING                (0x00000003uL)
#define ER_IN_PROCESS             (0x00000004uL)

/**********************************************************************************
* Driver Layer Error Base
**********************************************************************************/
#define ER_D_RESET_BASE           (0x00000100uL)
#define ER_D_CLK_BASE             (0x00000200uL)
#define ER_D_FLS_BASE             (0x00000300uL)
#define ER_D_COMM_BASE            (0x00000400uL)
#define ER_D_TIMER_BASE           (0x00000500uL)
#define ER_D_INFO_BASE            (0x00000600uL)
#define ER_D_MPU_BASE             (0x00000700uL)
#define ER_D_ECC_BASE             (0x00000800uL)
#define ER_D_MISC_BASE            (0x00000900uL)
#define ER_D_INT_BASE             (0x00000A00uL)
#define ER_D_TRNG_BASE            (0x00000B00uL)
#define ER_D_CRY_AES_BASE         (0x00000C00uL)
#define ER_D_CR_BASE              (0x00000D00uL)
#define ER_D_REMAP_BASE           (0x00000E00uL)

/**********************************************************************************
* Library Layer Error Base
***********************************************************************************/
#define ER_L_CRY_AES_BASE         (0x00010000uL)
#define ER_L_CRY_PRNG_BASE        (0x00020000uL)
#define ER_L_CRY_HASH_BASE        (0x00030000uL)
#define ER_L_CRY_HMAC_BASE        (0x00040000uL)
#define ER_L_CRY_RSA_BASE         (0x00050000uL)
#define ER_L_CRY_ECC_BASE         (0x00060000uL)
#define ER_L_CRY_BIGNUM_BASE      (0x00070000uL)
#define ER_L_CRY_X_509_BASE       (0x00080000uL)
#define ER_L_FLS_BASE             (0x000C0000uL)
#define ER_L_TOOLBOX_BASE         (0x000E0000uL)

/**********************************************************************************
* Middle Layer Error Base
***********************************************************************************/
#define ER_ML_CTX_BASE            (0x00100000uL)
#define ER_ML_ABTR_BASE           (0x00200000uL)
#define ER_ML_CFG_BASE            (0x00300000uL)
#define ER_ML_TEST_BASE           (0x00400000uL)
#define ER_ML_DVC_BASE            (0x00500000uL)
#define ER_MM_FLS_BASE            (0x00600000uL)
#define ER_MM_WDT_BASE            (0x00700000uL)
#define ER_MH_KEY_BASE            (0x00900000uL)
#define ER_MH_MYDATA_BASE         (0x00A00000uL)
#define ER_MH_MTC_BASE            (0x00C00000uL)
#define ER_MH_SYSINFO_BASE        (0x00D00000uL)
#define ER_MH_INIT_BASE           (0x00E00000uL)
#define ER_ML_PRNG_BASE           (0x00F00000uL)

/**********************************************************************************
* Kernel Layer Error Base
***********************************************************************************/
#define ER_K_ISSD_BASE            (0x10000000uL)

/**********************************************************************************
* Common definition
***********************************************************************************/
/* value of boolean type */
#define FALSE                            (0u)
#define TRUE                             (1u)

/**********************************************************************************
* AES block size
***********************************************************************************/
#define AES_BLOCK_SIZE             4
#define AES_BLOCK_SIZE_IN_BYTES   16
#define AES_BLOCK_SIZE_IN_BITS   128

/**********************************************************************************
* Hash Size
***********************************************************************************/
#define HASH_256_SIZE_IN_BYTES     (NUM256 / NUM8)
#define HASH_512_SIZE_IN_BYTES     (NUM512 / NUM8)

/**********************************************************************************
* RSA Size
***********************************************************************************/
#define PUB_EXP_SIZE_IN_BYTES   (16u)
#define MODULUS_SIZE_IN_BYTES   (R_CRYPT_BIGNUM_BYTESIZE)
#define PRIV_EXP_SIZE_IN_BYTES  (MODULUS_SIZE_IN_BYTES)

/**********************************************************************************
* Structure
***********************************************************************************/
typedef union remap_ptr_manip
{
    uint8_t     *p_ptr;
    uint32_t    val;
} remap_ptr_manip_t;

#endif /* _ER_ICUM_COMMON_H_ */

