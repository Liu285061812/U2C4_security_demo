/******************************************************************************/
/* Component Name   ICU-M HASH Driver                                         */
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

#ifndef ICUM_HASH_CMN_SHA3_H
/* Multiple inclusion protection macro */
#define ICUM_HASH_CMN_SHA3_H

#include "r_icum_hash_api.h"

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* SHA-3 Register defines */
/* SHA-3 Initialization register */
#define ICUM_SHA3A0INT            (*(volatile uint32_t*)0xFEE00000UL)
/* SHA-3 Control register 0 */
#define ICUM_SHA3A0CNT0           (*(volatile uint32_t*)0xFEE00004UL)
/* SHA-3 Control register 1 */
#define ICUM_SHA3A0CNT1           (*(volatile uint32_t*)0xFEE00008UL)
/* SHA-3 Operation start instruction register */
#define ICUM_SHA3AOS              (*(volatile uint32_t*)0xFEE0000CUL)
/* SHA-3 Input message bit length control register 0 */
#define ICUM_SHA3A0ILEN0          (*(volatile uint32_t*)0xFEE00010UL)
/* SHA-3 Input message bit length control register 1 */
#define ICUM_SHA3A0ILEN1          (*(volatile uint32_t*)0xFEE00014UL)
/* SHA-3 SHAKE output message length control register 0 */
#define ICUM_SHA3A0OLEN0          (*(volatile uint32_t*)0xFEE00018UL)
/* SHA-3 SHAKE output message length control register 1 */
#define ICUM_SHA3A0OLEN1          (*(volatile uint32_t*)0xFEE0001CUL)
/* SHA-3 FIFO type input register */
#define ICUM_SHA3A0IDAT           (*(volatile uint32_t*)0xFEE00030UL)
/* SHA-3 FIFO type output register */
#define ICUM_SHA3A0ODAT           (*(volatile uint32_t*)0xFEE00034UL)
/* SHA-3 Internal state input bit register */
#define ICUM_SHA3A0ISI            (*(volatile uint32_t*)0xFEE00038UL)
/* SHA-3 Internal state output bit register */
#define ICUM_SHA3A0ISO            (*(volatile uint32_t*)0xFEE0003CUL)
/* SHA-3 Status register */
#define ICUM_SHA3A0STS            (*(volatile uint32_t*)0xFEE00040UL)
/* SHA-3 Status clear register */
#define ICUM_SHA3A0STSC           (*(volatile uint32_t*)0xFEE00044UL)
/* SHA-3 DMA initialization register */
#define ICUM_SHA3A0DMAINIT        (*(volatile uint32_t*)0xFEE00050UL)
/* SHA-3 DMA control register 0 */
#define ICUM_SHA3A0DMACNT0        (*(volatile uint32_t*)0xFEE00054UL)
/* SHA-3 DMA control register 1 */
#define ICUM_SHA3A0DMACNT1        (*(volatile uint32_t*)0xFEE00058UL)
/* SHA-3 DMA input transfer size control register */
#define ICUM_SHA3A0DMAITS         (*(volatile uint32_t*)0xFEE00060UL)
/* SHA-3 DMA output transfer size control register */
#define ICUM_SHA3A0DMAOTS         (*(volatile uint32_t*)0xFEE00064UL)
/* SHA-3 DMA transfer data control register */
#define ICUM_SHA3A0DMALB          (*(volatile uint32_t*)0xFEE00068UL)
/* SHA-3 DMA transfer execution register */
#define ICUM_SHA3A0DMASTT         (*(volatile uint32_t*)0xFEE00080UL)
/* SHA-3 DMA pause request register */
#define ICUM_SHA3A0DMASUSP        (*(volatile uint32_t*)0xFEE00084UL)
/* SHA-3 DMA transfer status register */
#define ICUM_SHA3A0DMASTS         (*(volatile uint32_t*)0xFEE00088UL)
/* SHA-3 DMA transfer status clear register */
#define ICUM_SHA3A0DMASTSC        (*(volatile uint32_t*)0xFEE0008CUL)
/* SHA-3 Unit version register */
#define ICUM_SHA3A0VER            (*(volatile uint32_t*)0xFEE000FCUL)

/* Length of SHA3_256 in bytes */
#define SHA3_256_LEN              (32U)
/* Length of SHA3_384 in bytes */
#define SHA3_384_LEN              (48U)
/* Length of SHA3_512 in bytes */
#define SHA3_512_LEN              (64U)

/* Processing block size SHA3_256 */
#define BLOCK_SIZE_136_BYTE       (136U)
/* Processing block size SHA3_384 */
#define BLOCK_SIZE_104_BYTE       (104U)
/* Processing block size SHA3_512 */
#define BLOCK_SIZE_72_BYTE        (72U)

/* Value of SHA3_256 to set in MODE field */
#define CNT_MODE_VAL_SHA3_256     (0x1U)
/* Value of SHA3_384 to set in MODE field */
#define CNT_MODE_VAL_SHA3_384     (0x2U)
/* Value of SHA3_512 to set in MODE field */
#define CNT_MODE_VAL_SHA3_512     (0x3U)

/* Bit position of INIT field in SHA-3 Control Register */
#define INIT_INIT_POS_SHA3        (0UL)
/* Bit position of ISI_EN field in SHA-3 Control Register */
#define CNT0_ISI_EN_POS_SHA3      (12UL)
/* Bit position of ENDIAN field in SHA-3 Control Register */
#define CNT0_ENDIAN_POS_SHA3      (0UL)
/* Bit position of MODE field in SHA-3 Control Register */
#define CNT0_MODE_POS_SHA3        (4UL)
/* Bit position of HMK_CPU field in SHA-3 Status Register */
#define CNT1_HMK_CPU_POS_SHA3     (1UL)
/* Bit position of HMAC field in SHA-3 Status Register */
#define CNT1_HMAC_POS_SHA3        (0UL)
/* Bit position of HMAC_START field in SHA-3 Status Register */
#define CNT1_HMAC_START_POS_SHA3  (0UL)
/* Bit position of IDATA_READY field in SHA-3 Status Register */
#define STS_IDATA_READY_POS_SHA3  (0UL)
/* Bit position of BUSY field in SHA-3 Status Register */
#define STS_BUSY_POS_SHA3         (8UL)
/* Bit position of ODATA_VALID field in SHA-3 Status Register */
#define STS_ODATA_VALID_POS_SHA3  (4UL)
/* Bit position of IDATA_ZERO field in SHA-3 Data Control Register */
#define OS_IDATA_ZERO_POS_SHA3    (8UL)

/* Bit position of DMA_INIT field in SHA-3 DMA INIT Register */
#define DMAINIT_INIT_POS_SHA3     (0UL)
/* Bit position of DMA_ENDIAN field in SHA-3 DMA Control Register */
#define DMACNT0_ENDIAN_POS_SHA3   (0UL)
/* Bit position of DMA_MODE field in SHA-3 DMA Control Register */
#define DMACNT0_MODE_POS_SHA3     (4UL)
/* Bit position of DMA_ILB field in SHA-3 DMA Last Byte Setting Register */
#define DMALB_ILB_POS_SHA3        (0UL)
/* Bit position of DMA_START field in SHA-3 DMA Start Control Register */
#define DMASTT_START_POS_SHA3     (0UL)

/* 0 check of intermediate data */
#define MIDDLE_DATA_CHECK_0       (0U)
/* Message size bit conversion */
#define MESSAGE_SIZE_BIT          (8ULL)
/* 0 setting for the remaining data */
#define REMAINING_DATA_ZERO       (0U)
/* Number of register settings for remaining data */
#define REMAINING_DATA_SET        (50U)
/* 0 check of data to be saved */
#define SAVE_DATA_CHECK_0         (0U)

/* 0 check for timeout */
#define TIMEOUT_CHECK_0           (0U)
/* Timeout count of hash operation */
#define TIMEOUT_INIT_SHA3         (0x00000020U)

/* Lower 32-bit mask */
#define ICUM_ILEN0_MASK           (0xFFFFFFFFULL)
/* Upper 32-bit acquisition shift */
#define ICUM_ILEN1_SHIFT          (32U)

/* Init processing management mask */
#define INIT_STATE_CONT_MASK      (0x00000001U)
/* Execution check of Init process */
#define INIT_STATE_EXECUTION      (0x00000001U)

/**********************************************************************************************************************
 Union for pointer manipulation                                                                                        
**********************************************************************************************************************/
typedef union ptr_manip_sha3
{
    const uint8_t*  p_ptr_8;
    const uint32_t* p_temp;
    uint32_t*       p_out_buf;
    uint32_t        val;
} ptr_manip_sha3_t;

/**********************************************************************************************************************
 Public global functions
**********************************************************************************************************************/
extern uint32_t d_hash_cmn_sha3_get_length(const e_r_hash_type_t type);
extern e_r_hash_ret_t d_hash_cmn_sha3_context_chk_arg(const st_r_hash_ctx_t* const p_ctx);
extern uint32_t d_hash_cmn_sha3_get_block_size(const e_r_hash_type_t type);
extern uint32_t d_hash_cmn_sha3_get_mode(const e_r_hash_type_t type);
extern e_r_hash_ret_t d_hash_cmn_sha3_init_register(const uint32_t mode);
extern e_r_hash_ret_t d_hash_cmn_sha3_get_middle(uint32_t* const p_im_data, uint32_t* const p_im_len);
extern e_r_hash_ret_t d_hash_cmn_sha3_exec_block(st_r_hash_ctx_t* const p_ctx, const uint8_t* const p_msg,
                                                 const uint32_t msg_len, const uint32_t hash_block_size);
extern void d_hash_cmn_sha3_set_idat(const uint8_t* const p_data, const uint32_t len);
extern e_r_hash_ret_t d_hash_cmn_sha3_get_output_data(const uint32_t hash_len, uint8_t* const p_out);
extern void d_hash_cmn_sha3_init_reg_dma(const uint32_t mode);

#endif /* ICUM_HASH_CMN_SHA3_H */
