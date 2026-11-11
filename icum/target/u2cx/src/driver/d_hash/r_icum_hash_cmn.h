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

/* PRQA S 0776 ++ # D005 Exception according to function nomenclature */
/* PRQA S 5002 ++ # D013 Exception according to file nomenclature */

#ifndef R_ICUM_HASH_CMN_H
/* Multiple inclusion protection macro */
#define R_ICUM_HASH_CMN_H

#include "r_icum_hash_api.h"
#include "icum_common_pub.h"

/*=====================================================================================================================
 Public macro definitions
=====================================================================================================================*/
/* Value definition */
#define NUM0                     (0u)
#define NUM1                     (1u)
#define NUM2                     (2u)
#define NUM3                     (3u)
#define NUM4                     (4u)
#define NUM12                    (12u)
#define NUM16                    (16u)
#define NUM28                    (28u)
#define NUM30                    (30u)
#define NUM32                    (32u)
#define NUM52                    (52U)
#define NUM104                   (104U)
#define NUM_ERR_U32              (0xFFFFFFFFU)

/*!****************************************************
 * \addtogroup HASHCommonDefinesInternal
 * \{**************************************************/
#define HASH_VERSION_MAJOR   (1U)       /**< HASH Driver Major version */
#define HASH_VERSION_MINOR   (0U)       /**< HASH Driver Minor version */

/* SHA Register defines */
#define ICUM_SHACNT              (*(volatile uint32_t*)0xFFFEE180U)  /**< SHA Control Register */
#define ICUM_SHASTS              (*(volatile uint32_t*)0xFFFEE184U)  /**< SHA Status Register */
#define ICUM_SHASTSC             (*(volatile uint32_t*)0xFFFEE188U)  /**< SHA Status Clear Register */
#define ICUM_SHAIDAT             (*(volatile uint32_t*)0xFFFEE190U)  /**< SHA Input Data Register */
#define ICUM_SHAODAT             (*(volatile uint32_t*)0xFFFEE194U)  /**< SHA Output Data Register */
#define ICUM_SHADDCNT            (*(volatile uint32_t*)0xFFFEE198U)  /**< SHA I/O Data Control Register */
#define ICUM_SHADID0             (*(volatile uint32_t*)0xFFFEE1A0U)  /**< SHA Direct Input Data Register 0 */
#define ICUM_SHADID1             (*(volatile uint32_t*)0xFFFEE1A4U)  /**< SHA Direct Input Data Register 1 */
#define ICUM_SHADID2             (*(volatile uint32_t*)0xFFFEE1A8U)  /**< SHA Direct Input Data Register 2 */
#define ICUM_SHADID3             (*(volatile uint32_t*)0xFFFEE1ACU)  /**< SHA Direct Input Data Register 3 */
#define ICUM_SHADID4             (*(volatile uint32_t*)0xFFFEE1B0U)  /**< SHA Direct Input Data Register 4 */
#define ICUM_SHADID5             (*(volatile uint32_t*)0xFFFEE1B4U)  /**< SHA Direct Input Data Register 5 */
#define ICUM_SHADID6             (*(volatile uint32_t*)0xFFFEE1B8U)  /**< SHA Direct Input Data Register 6 */
#define ICUM_SHADID7             (*(volatile uint32_t*)0xFFFEE1BCU)  /**< SHA Direct Input Data Register 7 */
#define ICUM_SHADIL0             (*(volatile uint32_t*)0xFFFEE1C0U)  /**< SHA Direct Input Data Length Register 0 */
#define ICUM_SHADIL1             (*(volatile uint32_t*)0xFFFEE1C4U)  /**< SHA Direct Input Data Length Register 1 */
#define ICUM_SHADOD0             (*(volatile uint32_t*)0xFFFEE1C8U)  /**< SHA Direct Output Data Register 0 */
#define ICUM_SHADOD1             (*(volatile uint32_t*)0xFFFEE1CCU)  /**< SHA Direct Output Data Register 1 */
#define ICUM_SHADOD2             (*(volatile uint32_t*)0xFFFEE1D0U)  /**< SHA Direct Output Data Register 2 */
#define ICUM_SHADOD3             (*(volatile uint32_t*)0xFFFEE1D4U)  /**< SHA Direct Output Data Register 3 */
#define ICUM_SHADOD4             (*(volatile uint32_t*)0xFFFEE1D8U)  /**< SHA Direct Output Data Register 4 */
#define ICUM_SHADOD5             (*(volatile uint32_t*)0xFFFEE1DCU)  /**< SHA Direct Output Data Register 5 */
#define ICUM_SHADOD6             (*(volatile uint32_t*)0xFFFEE1E0U)  /**< SHA Direct Output Data Register 6 */
#define ICUM_SHADOD7             (*(volatile uint32_t*)0xFFFEE1E4U)  /**< SHA Direct Output Data Register 7 */
#define ICUM_SHADOL0             (*(volatile uint32_t*)0xFFFEE1E8U)  /**< SHA Direct Output Data Length Register 0 */
#define ICUM_SHADOL1             (*(volatile uint32_t*)0xFFFEE1ECU)  /**< SHA Direct Output Data Length Register 1 */
#define ICUM_SHAFIFOCNT          (*(volatile uint32_t*)0xFFFEE1F0U)  /**< SHA FIFO Control Register */
#define ICUM_SHADCNT             (*(volatile uint32_t*)0xFFFEE1F4U)  /**< SHA Data Control Register 2 */
/* SHA Version Register */
#define ICUM_SHAVER              (*(volatile uint32_t*)0xFFFEE1FCUL)
/* SHA DMA Control Register */
#define ICUM_SHADMACNT           (*(volatile uint32_t*)0xFF1FC810UL)
/* SHA DMA transfer Size Register */
#define ICUM_SHADMATSR           (*(volatile uint32_t*)0xFF1FC814UL)
/* SHA DMA data Control Register */
#define ICUM_SHADMADCNT          (*(volatile uint32_t*)0xFF1FC818UL)
/* SHA DMA Status Register */
#define ICUM_SHADMASTS           (*(volatile uint32_t*)0xFF1FC81CUL)
/* SHA DMA Status Clear Register */
#define ICUM_SHADMASTSC          (*(volatile uint32_t*)0xFF1FC820UL)
/* SHA DMA Start Register */
#define ICUM_SHADMASTT           (*(volatile uint32_t*)0xFF1FC824UL)
/* SHA DMA Suspend Register */
#define ICUM_SHADMASUSP          (*(volatile uint32_t*)0xFF1FC828UL)

typedef struct
{
    /* Global Registers */
    uint8_t     dummy814[NUM16];
    /* DMA address error notification status register   */
    uint32_t    ICUM_DMAESTA;
    uint8_t     dummy815[NUM12];
    /* DMA interrupt status register                    */
    uint32_t    ICUM_DMAISTA;
    uint8_t     dummy816[NUM28];
    /* DMA channel request priority register            */
    uint32_t    channel_priority;
    uint8_t     dummy817[NUM28];
    /* DMA operation register                           */
    uint16_t    ICUM_DMAOR;
    uint8_t     dummy818[NUM30];
    /* DMA channel reset register                       */
    uint32_t    channel_reset;
    uint8_t     dummy819[NUM104];
    /* DMA version register                             */
    uint32_t    version;
} REG_ICUM_DMAC_t;

typedef struct
{
    /* Channel Registers */
    /* DMA source address register      */
    uint32_t    ICUM_DMASAR;
    /* DMA destination address register */
    uint32_t    ICUM_DMADAR;
    /* DMA transfer size register       */
    uint32_t    ICUM_DMATSR;
    /* DMA transfer size register B     */
    uint32_t    ICUM_DMATSRB;
    /* DMA transfer mode register       */
    uint32_t    ICUM_DMATMR;
    /* DMA channel control register     */
    uint16_t    ICUM_DMACHCR;
    /* DMA channel stop register        */
    uint16_t    ICUM_DMACHSTP;
    /* DMA channel status register      */
    uint32_t    ICUM_DMACHSTA;
    /* DMA channel flag clear register  */
    uint32_t    ICUM_DMACHFCR;
    uint8_t     dummy821[NUM32];
    /* DAM resource select register     */
    uint32_t    ICUM_DMARS;
    uint8_t     dummy822[NUM4];
    /* DMA buffer control register      */
    uint32_t    ICUM_DMABUFCR;
    uint8_t     dummy823[NUM52];
} REG_ICUM_DMAC_CH_t;

/* Number of channels */
#define ICUM_DMAC_CH_NUM (4U)

/* DMAC Global Registers Base Address */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
#define REG_ICUM_DMAC_BASE       (0xFF600000U)
#else   /* TARGET_DEVICE_S4H */
#define REG_ICUM_DMAC_BASE       (0xFF1FA000U)
#endif

/* DMAC Channel Registers Base Address */
#define REG_ICUM_DMAC_CH_BASE    (REG_ICUM_DMAC_BASE + 0x2000U)

#define REG_ICUM_DMAC            (*(volatile REG_ICUM_DMAC_t *)REG_ICUM_DMAC_BASE)
#define REG_ICUM_DMAC_CH         (*(volatile REG_ICUM_DMAC_CH_t (*)[ICUM_DMAC_CH_NUM])REG_ICUM_DMAC_CH_BASE)

#define CNT_INIT_POS             (31U)  /**< Bit position of INIT field in SHA Control Register */
#define CNT_ENDIAN_POS           (15U)  /**< Bit position of ENDIAN field in SHA Control Register */
#define CNT_MODE_POS             (4U)   /**< Bit position of MODE field in SHA Control Register */
#define CNT_MODE_VAL_SHA1        (0x0U) /**< Value of SHA1 to set in MODE field */
#define CNT_MODE_VAL_SHA224      (0x4U) /**< Value of SHA224 to set in MODE field */
#define CNT_MODE_VAL_SHA256      (0x5U) /**< Value of SHA256 to set in MODE field */
/* Value of SHA512_224 to set in MODE field */
#define CNT_MODE_VAL_SHA512_224  (0x8U)
/* Value of SHA512_256 to set in MODE field */
#define CNT_MODE_VAL_SHA512_256  (0x9U)
/* Value of SHA384 to set in MODE field */
#define CNT_MODE_VAL_SHA384      (0xAU)
/* Value of SHA512 to set in MODE field */
#define CNT_MODE_VAL_SHA512      (0xBU)


/* Bit position of DMA_INIT field in DMA Control Register */
#define DMACNT_INIT_POS          (31UL)
/* Bit position of DMA_ENDIAN field in DMA Control Register */
#define DMACNT_ENDIAN_POS        (15UL)
/* Bit position of DMA_MODE field in DMA Control Register */
#define DMACNT_MODE_POS          (4UL)
/* Bit position of DMA_START field in DMA Start Control Register */
#define DMASTT_START_POS         (0UL)
/* Bit position of DMA_ILB field in DMA Last Byte Setting Register */
#define DMALB_ILB_POS            (0UL)

#define STS_IDATA_READY_POS      (4U)   /**< Bit position of IDATA_READY field in SHA Status Register */
#define STS_ODATA_VALID_POS      (1U)   /**< Bit position of ODATA_VALID field in SHA Status Register */
#define STS_DIRECTOUT_VALID_POS  (0U)   /**< Bit position of DIRECTOUT_VALID field in SHA Status Register */

#define STSC_ODATA_VALCLR_POS    (1U)   /**< Bit position of ODATA_VALID_CLR field in SHA Status Clear Register */
#define STSC_DIROUT_VALCLR_POS   (0U)   /**< Bit position of DIRECTOUT_VALID_CLR field in SHA Status Clear Register */

#define DDCNT_DIRECTIN_LOAD_POS  (4U)   /**< Bit position of DIRECTIN_LOAD field in SHA I/O Data Control Register */
#define DDCNT_DIRECTOUT_REQ_POS  (1U)   /**< Bit position of DIRECTOUT_REQ field in SHA I/O Data Control Register */
/* Bit position of EX_DID field in SHA I/O Data Control Register */
#define DDCNT_EX_DID_POS         (16U)
/* Bit position of EX_DIL field in SHA I/O Data Control Register */
#define DDCNT_EX_DIL_POS         (17U)
/* Bit position of EX_DOD field in SHA I/O Data Control Register */
#define DDCNT_EX_DOD_POS         (18U)
/* Bit position of EX_DOL field in SHA I/O Data Control Register */
#define DDCNT_EX_DOL_POS         (19U)

#define FIFOCNT_ONOTEMP_POS      (19U)  /**< Bit position of ONOTEMP field in SHA FIFO Control Register */
#define FIFOCNT_INOTEMP_POS      (12U)  /**< Bit position of INOTEMP field in SHA FIFO Control Register */
#define FIFOCNT_IDAT_POS         (0U)   /**< Bit position of IDT field in SHA FIFO Control Register */

#define DCNT_IDATA_NUM_POS       (8U)   /**< Bit position of DATA_NUM field in SHA Data Control Register */
#define DCNT_HASH_GET_POS        (6U)   /**< Bit position of HASH_GET field in SHA Data Control Register */
#define DCNT_IDATA_ZERO_POS      (5U)   /**< Bit position of IDATA_ZERO field in SHA Data Control Register */
#define DCNT_IDATA_LAST_POS      (4U)   /**< Bit position of IDATA_LAST field in SHA Data Control Register */
#define DCNT_IPAD_POS            (3U)   /**< Bit position of IPAD field in SHA Data Control Register */
#define DCNT_OPAD_POS            (2U)   /**< Bit position of OPAD field in SHA Data Control Register */

/* Bit position of PR field in DMA Operation Register */
#define OR_PR_POS_DMA            (8U)
/* Bit position of DME field in DMA Operation Register */
#define OR_DME_POS_DMA           (0U)

/* Bit position of TRS field in DMA Transfer Mode Register */
#define TMR_TRS_POS_DMA          (12UL)
/* Bit position of DM field in DMA Transfer Mode Register */
#define TMR_DM_POS_DMA           (10UL)
/* Bit position of SM field in DMA Transfer Mode Register */
#define TMR_SM_POS_DMA           (8UL)
/* Bit position of DTS field in DMA Transfer Mode Register */
#define TMR_DTS_POS_DMA          (4UL)
/* Bit position of STS field in DMA Transfer Mode Register */
#define TMR_STS_POS_DMA          (0UL)

/* Bit position of TC field in DMA Resource Select Register */
#define RS_TC_POS_DMA            (16UL)
/* Bit position of TC field in DMA Resource Select Register */
#define RS_TL_POS_DMA            (12UL)
/* Bit position of PLE field in DMA Resource Select Register */
#define RS_PLE_POS_DMA           (10UL)
/* Bit position of RS field in DMA Resource Select Register */
#define RS_RS_POS_DMA            (0UL)

/* Bit position of CAEE field in DMA Channel Control Register */
#define CHCR_CAEE_POS_DMA        (4U)
/* Bit position of CAIE field in DMA Channel Control Register */
#define CHCR_CAIE_POS_DMA        (3U)
/* Bit position of IE field in DMA Channel Control Register */
#define CHCR_IE_POS_DMA          (1U)
/* Bit position of DE field in DMA Channel Control Register */
#define CHCR_DE_POS_DMA          (0U)

/* Bit position of OVFC field in DMA Channel Status Flag Clear Register */
#define CHFCR_OVFC_POS_DMA        (13UL)
/* Bit position of DRQC field in DMA Channel Status Flag Clear Register */
#define CHFCR_DRQC_POS_DMA        (12UL)
/* Bit position of CAEC field in DMA Channel Status Flag Clear Register */
#define CHFCR_CAEC_POS_DMA        (3UL)
/* Bit position of TEC field in DMA Channel Status Flag Clear Register */
#define CHFCR_TEC_POS_DMA         (1UL)
/* Bit position of DEC field in DMA Channel Status Flag Clear Register */
#define CHFCR_DEC_POS_DMA         (0UL)

/* Register bit setting 0x0 */
#define REGISTER_SET_0           (0UL)
/* Register bit setting 0x1 */
#define REGISTER_SET_1           (1UL)
/* Register bit setting 0x0110 */
#define REGISTER_SET_6           (6UL)
/* Register bit setting 0x0 (for short) */
#define SHORT_REGISTER_SET_0     (0U)
/* Register bit setting 0x1 (for short) */
#define SHORT_REGISTER_SET_1     (1U)
/* Register bit setting 0x11 (for short) */
#define SHORT_REGISTER_SET_3     (3U)

/* Address value for identify HW (SHA-2) */
#define HW_ADDR_SHA2             (0xFF1FC800U)
/* Address value for identify HW (SHA-3) */
#define HW_ADDR_SHA3             (0xFEE00100U)

/* DMA request source (SHA-2) */
#define DMA_REQ_SRC_SHA2_IN      (0U)
#define DMA_REQ_SRC_SHA2_OUT     (1U)
/* DMA request source (SHA-3) */
#define DMA_REQ_SRC_SHA3_IN      (2U)
#define DMA_REQ_SRC_SHA3_OUT     (3U)

/* Ready to use DMA (input channel) */
#define DMA_INPUT_CHANNEL        (0x0U)
/* Ready to use DMA (output channel) */
#define DMA_OUTPUT_CHANNEL       (0x1U)

/* DMA output value length */
#define DMA_OUTPUT_LEN           (0x40U)

#define SHA1_LEN                 (20U)  /**< Length of SHA1 in bytes */
#define SHA224_LEN               (28U)  /**< Length of SHA224 in bytes */
#define SHA256_LEN               (32U)  /**< Length of SHA256 in bytes */
#define SHA384_LEN               (48U)  /**< Length of SHA384 in bytes */
#define SHA512_LEN               (64U)  /**< Length of SHA512 in bytes */

#define BLOCK_SIZE_64_BYTE       (64U)  /**< Processing block size SHA1~SHA256 */
#define BLOCK_SIZE_128_BYTE      (128U) /**< Processing block size SHA384~SHA512 */

#define HASH_WORD_SIZE           (4U)   /**< Processing word size */

#define BYTE2BIT_VAL             (8U)   /**< Exchange Byte to Bit */

#define UNSUPPORTED_TYPE         (0xFFFFFFFFU) /**< Return value when unsupported type is set */

/* The number of bytes for alignment */
#define ALIGNMENT_64_BYTE        (64U)

/* Reference value for size check */
#define MULTIPLE_OF_64           (64U)

/* Index of management information table for DMA transfer */
#define DMA_MNG_FOR_SHA2         (0U)
#define DMA_MNG_FOR_SHA3         (1U)

/* DMA transfer status */
#define DMA_STATUS_NON_EXEC      (0x00000000U)
#define DMA_STATUS_RUNNING       (0x00000001U)
#define DMA_STATUS_NORMAL_END    (0x00081000U)
#define DMA_STATUS_ADDR_ERR      (0x00081001U)
#define DMA_STATUS_TIMEOUT_ERR   (0x00081002U)

/* DMA status check bit */
/* channel pattern 0,1 */
#define DMA_CHECK_STATUS_CH0_1   (0x00000003U)
/* channel pattern 2,3 */
#define DMA_CHECK_STATUS_CH2_3   (0x0000000CU)

/* Zero clear */
#define ZERO_CLEAR                (0U)
/* Variable initialization */
#define VARIABLE_INIT             (0U)

/* HASH size error */
#define LENGTH_ERR                (0U)
/* Block size error */
#define BLOCK_SIZE_ERR            (0U)

/* 0 check of the remaining data */
#define REMAINING_DATA_CHECK_0    (0U)
/* 0 check for message size */
#define MESSAGE_LENGTH_CHECK_0    (0U)
/* Mask for MODE bit in ICUM_SHACNT register */
#define ICUM_SHACNT_MODE_MASK    (0x000000F0U)

/*\}*/

/**********************************************************************************************************************
 Union for pointer manipulation                                                                                        
**********************************************************************************************************************/
typedef union ptr_manip_hash
{
    const uint8_t   *p_cnst_uint8;
    uint32_t        val;
} ptr_manip_hash_t;

/*=====================================================================================================================
 Public global functions
=====================================================================================================================*/
/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0019]:[ICUMFW_UD_S4_D_HASH_0019]
* Function Name : r_hash_cmn_set_byte
**********************************************************************************************************************/
extern void           r_hash_cmn_set_byte(uint8_t* const p_dest, const uint8_t val, const uint32_t len);

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0020]:[ICUMFW_UD_S4_D_HASH_0020]
* Function Name : r_hash_cmn_cpy_byte
**********************************************************************************************************************/
extern void           r_hash_cmn_cpy_byte(uint8_t* const p_dest, const uint8_t* const p_src, const uint32_t len);

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0021]:[ICUMFW_UD_S4_D_HASH_0021]
* Function Name : r_hash_cmn_roundup_4
**********************************************************************************************************************/
extern uint32_t       r_hash_cmn_roundup_4(const uint32_t val);

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0022]:[ICUMFW_UD_S4_D_HASH_0022]
* Function Name : r_hash_cmn_init_register
**********************************************************************************************************************/
extern e_r_hash_ret_t r_hash_cmn_init_register(const uint32_t mode);

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0023]:[ICUMFW_UD_S4_D_HASH_0023]
* Function Name : r_hash_cmn_preprocess_block
**********************************************************************************************************************/
extern e_r_hash_ret_t r_hash_cmn_preprocess_block(const st_r_hash_ctx_t* const p_ctx);
extern e_r_hash_ret_t r_hash_cmn_chk_type(const e_r_hash_type_t type);
extern e_r_hash_ret_t r_hash_cmn_update(st_r_hash_ctx_t* const p_ctx,
                                        const uint8_t* const p_msg, const uint32_t msg_len);
extern uint32_t       r_hash_cmn_get_mode(const e_r_hash_type_t type);
extern uint32_t       r_hash_cmn_get_length(const e_r_hash_type_t type);
extern uint32_t       r_hash_cmn_get_block_size(const e_r_hash_type_t type);
extern e_r_hash_ret_t r_hash_cmn_get_intermediate(uint32_t* const p_im_data, uint32_t* const p_im_len);

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0030]:[ICUMFW_UD_S4_D_HASH_0030]
* Function Name : r_hash_cmn_set_idat
**********************************************************************************************************************/
extern void           r_hash_cmn_set_idat(const uint8_t* const p_data, const uint32_t len);

extern e_r_hash_ret_t r_hash_cmn_context_chk_arg(const st_r_hash_ctx_t* const p_ctx);

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0032]:[ICUMFW_UD_S4_D_HASH_0032]
* Function Name : r_hash_cmn_wait_for_idata_ready
**********************************************************************************************************************/
extern e_r_hash_ret_t r_hash_cmn_wait_for_idata_ready(void);

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0033]:[ICUMFW_UD_S4_D_HASH_0033]
* Function Name : r_hash_cmn_get_output_data
**********************************************************************************************************************/
extern e_r_hash_ret_t r_hash_cmn_get_output_data(const uint32_t hash_len, uint8_t* const p_out);

extern e_r_hash_ret_t d_hash_cmn_chk_alignment(const uint32_t alignment, const uint8_t* p_area);
extern e_r_hash_ret_t d_hash_cmn_chk_size(const uint32_t refer_size, const uint32_t taget_size);
extern uint32_t       d_hash_cmn_get_dma_status(const e_r_hash_type_t type);
extern e_r_hash_ret_t d_hash_cmn_set_dma_status(const e_r_hash_type_t type, const uint32_t status);
extern e_r_hash_ret_t d_hash_cmn_prepare_dma(const uint8_t* const p_addr, 
                                             const uint32_t transfer_byte_len,
                                             const e_r_hash_type_t type,
                                             const uint32_t setting_channel);
extern uint32_t d_hash_cmn_wait_sts_dma(const e_r_hash_type_t type);
extern void d_hash_cmn_init_reg_dma(const uint32_t mode);
extern void d_hash_cmn_init_sts_dma(const st_r_hash_ctx_t* const p_ctx);

#endif /* R_ICUM_HASH_CMN_H */
/*=====================================================================================================================
 End of File
=====================================================================================================================*/

