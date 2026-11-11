/******************************************************************************/
/* Component Name  INIT Manager                                               */
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

#if !defined(ICUM_MH_INIT_PUB_H)
#define ICUM_MH_INIT_PUB_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Return Value Definition */
/* INIT Return value : 0x00000001u to 0x0000000Fu */
/* DRV Error         : 0x00000010u to 0x0000002Fu */
/* LIB Error         : 0x00000030u to 0x0000004Fu */
/* MNG Error         : 0x00000050u to 0x0000006Fu */

#define ER_MH_INIT_D_RESET_INITFAIL      (ER_MH_INIT_BASE + (0x00000011uL))
#define ER_MH_INIT_D_CLK_INITFAIL        (ER_MH_INIT_BASE + (0x00000012uL))
#define ER_MH_INIT_D_FLS_INITFAIL        (ER_MH_INIT_BASE + (0x00000013uL))
#define ER_MH_INIT_D_COMM_INITFAIL       (ER_MH_INIT_BASE + (0x00000014uL))
#define ER_MH_INIT_D_TIMER_INITFAIL      (ER_MH_INIT_BASE + (0x00000015uL))
#define ER_MH_INIT_D_INFO_INITFAIL       (ER_MH_INIT_BASE + (0x00000016uL))
#define ER_MH_INIT_D_MPU_INITFAIL        (ER_MH_INIT_BASE + (0x00000017uL))
#define ER_MH_INIT_D_MISC_INITFAIL       (ER_MH_INIT_BASE + (0x00000019uL))
#define ER_MH_INIT_D_INT_INITFAIL        (ER_MH_INIT_BASE + (0x0000001AuL))
#define ER_MH_INIT_D_CRY_RND_INITFAIL    (ER_MH_INIT_BASE + (0x0000001BuL))
#define ER_MH_INIT_D_CRY_AES_INITFAIL    (ER_MH_INIT_BASE + (0x0000001CuL))

#define ER_MH_INIT_L_CRY_AES_INITFAIL    (ER_MH_INIT_BASE + (0x00000031uL))
#define ER_MH_INIT_L_CRY_RND_INITFAIL    (ER_MH_INIT_BASE + (0x00000032uL))
#define ER_MH_INIT_L_CRY_HASH_INITFAIL   (ER_MH_INIT_BASE + (0x00000033uL))
#define ER_MH_INIT_L_CRY_HMAC_INITFAIL   (ER_MH_INIT_BASE + (0x00000034uL))
#define ER_MH_INIT_L_CRY_RSA_INITFAIL    (ER_MH_INIT_BASE + (0x00000035uL))
#define ER_MH_INIT_L_CRY_ECC_INITFAIL    (ER_MH_INIT_BASE + (0x00000036uL))
#define ER_MH_INIT_L_CRY_BIGNUM_INITFAIL (ER_MH_INIT_BASE + (0x00000037uL))
#define ER_MH_INIT_L_CRY_X_509_INITFAIL  (ER_MH_INIT_BASE + (0x00000038uL))
#define ER_MH_INIT_L_CRY_FLS_INITFAIL    (ER_MH_INIT_BASE + (0x00000039uL))

#define ER_MH_INIT_M_CTX_INITFAIL        (ER_MH_INIT_BASE + (0x00000051uL))
#define ER_MH_INIT_M_ABTR_INITFAIL       (ER_MH_INIT_BASE + (0x00000052uL))
#define ER_MH_INIT_M_CFG_INITFAIL        (ER_MH_INIT_BASE + (0x00000053uL))
#define ER_MH_INIT_M_TEST_INITFAIL       (ER_MH_INIT_BASE + (0x00000054uL))
#define ER_MH_INIT_M_FLS_INITFAIL        (ER_MH_INIT_BASE + (0x00000055uL))
#define ER_MH_INIT_M_DVC_INITFAIL        (ER_MH_INIT_BASE + (0x00000056uL))
#define ER_MH_INIT_M_WDT_INITFAIL        (ER_MH_INIT_BASE + (0x00000057uL))
#define ER_MH_INIT_M_CR_INITFAIL         (ER_MH_INIT_BASE + (0x00000058uL))
#define ER_MH_INIT_M_KEY_INITFAIL        (ER_MH_INIT_BASE + (0x00000059uL))
#define ER_MH_INIT_M_MYDATA_INITFAIL     (ER_MH_INIT_BASE + (0x0000005AuL))
#define ER_MH_INIT_M_SYSINFO_INITFAIL    (ER_MH_INIT_BASE + (0x0000005DuL))

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
uint32_t MH_INIT_Init(void);
uint32_t MH_INIT_StartPE(void);

#endif /* #if !defined(ICUM_MH_INIT_PUB_H) */
