/******************************************************************************/
/* Component Name   TRNG driver header file                                   */
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

#ifndef ICUM_D_TRNG_PUB_H_
#define ICUM_D_TRNG_PUB_H_

/**********************************************************************************
* Define
***********************************************************************************/
#define ER_D_TRNG_OK            ((uint32_t)ER_OK)
#define ER_D_TRNG_PARAMETER     ((uint32_t)ER_PARAM)
#define ER_D_TRNG_SDT_SUCCESS   ((uint32_t)ER_D_TRNG_BASE)
#define ER_D_TRNG_SDT_FAIL      ((uint32_t)(ER_D_TRNG_BASE + 0x01uL))
#define ER_D_TRNG_BUSY          ((uint32_t)(ER_D_TRNG_BASE + 0x10uL))
#define ER_D_TRNG_STATUS        ((uint32_t)(ER_D_TRNG_BASE + 0x20uL))
#define ER_D_TRNG_SEED          ((uint32_t)(ER_D_TRNG_BASE + 0x30uL))
#define ER_D_TRNG_TRN           ((uint32_t)(ER_D_TRNG_BASE + 0x40uL))

/* TRNG self test */
#define D_TRNG_NO_TEST     (0u)    /* no test */
#define D_TRNG_DIAG_P1     (1u)    /* self-diagnostic test P1 */
#define D_TRNG_DIAG_P2     (2u)    /* self-diagnostic test P2 */
#define D_TRNG_HEALTH_TEST (3u)    /* health test P2 */

/* TRNG Status */
#define D_TRNG_NOT_GENERATING ((uint32_t)0uL)  /* TRNG Not generation */
#define D_TRNG_GENERATING     ((uint32_t)1uL)  /* TRNG Generating */
#define D_TRNG_TEST_ERR       ((uint32_t)2uL)  /* TRNG Test Error */


/* TRNG Number Status */
#define D_TRNG_FULL           ((uint32_t)0uL)  /* random number full */
#define D_TRNG_NOT_FULL       ((uint32_t)1uL)  /* random number not full */

/**********************************************************************************
* Function prototype
***********************************************************************************/
uint32_t D_TRNG_Init(void);
uint32_t D_TRNG_SetTest(uint32_t test_enable_bit);
uint32_t D_TRNG_EnableModule(void);
uint32_t D_TRNG_DisableModule(void);
uint32_t D_TRNG_GetValue(uint32_t *p_trng_Value);
uint32_t D_TRNG_CheckTRN(uint32_t word_number, uint32_t* p_result);
uint32_t D_TRNG_GetTRN(uint32_t *p_trng, uint32_t word_number);
uint32_t D_TRNG_GetState(uint32_t* p_trng_state);
uint32_t D_TRNG_GenerateSeed(uint32_t test_mode, uint32_t word_number);

#endif /* ICUM_D_TRNG_PUB_H_ */
