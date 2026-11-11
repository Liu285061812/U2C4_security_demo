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

/* PRQA S 0750 ++ # D003 Use union to efficiently use RAM area. */
/* PRQA S 0776 ++ # D005 Exception according to function nomenclature */
/* PRQA S 1434 ++ # D006 Do not build in a 16 bit environment */
/* PRQA S 5002 ++ # D013 Exception according to file nomenclature */

#ifndef R_ICUM_HASH_API_H
/* Multiple inclusion protection macro */
#define R_ICUM_HASH_API_H

#include "renesas_types.h"

/*!***********************************************************
 * \addtogroup  ICUMHASH         HASH module
 * \{
 * \addtogroup  HASHCommon       HASH Common module
 * \addtogroup  HASHHash         HASH Hash module
 * \addtogroup  HASHHMAC         HASH HMAC module
 * \}*********************************************************/

/*=====================================================================================================================
 Public macro definitions
=====================================================================================================================*/
/*!****************************************************
 * \addtogroup  HASHCommonDefines   Common Definitions
 * \{**************************************************/
#define HASH_LEFTOVER_MAX           (168U)          /**< Leftover message buffer max size */
#define HASH_HMAC_KEY_SIZE_MAX      (64U)           /**< Key buffer size for HMAC */
#define HASH_IM_DATA_WORD_SIZE      (50U)          /**< Word size of intermediate data */
#define HASH_IM_LEN_WORD_SIZE       (4U)            /**< Word size of intermediate length */
#define HASH_DMA_PROC_RESULT_SIZE   (64U)           /**< DMA processing result size */

#define HASH_USE_ICUP_TRANSFER      (0U)            /**< Use ICUP transfer */
#define HASH_USE_DMA_TRANSFER       (1U)            /**< Use DMA transfer */
#define HASH_USE_INPUT_KEY          (0U)            /**< Use Input Key */
#define HASH_USE_AES_KEY            (1U)            /**< Use AES Key */
#define HASH_DMA_CH_UNUSED          (0xFFFFFFFFU)   /**< Channel unused */
/*\}*/

/*=====================================================================================================================
 Public type definitions
=====================================================================================================================*/
/*!****************************************************
 * \addtogroup  HASHCommonTypes     Common Types
 * \{**************************************************/
/** HASH return value enumeration */
typedef enum
{
    HASH_RET_OK                 = 0x00000000U,   /**< API succeeded */
    HASH_RET_DMA_RUNNING,                        /**< Processing in DMA transfer mode */
    /* DMA transfer is not possible */
    HASH_RET_DMA_NOTAPPLICABLE,
    HASH_RET_PARAMERR           = 0x00080001U,   /**< Argument parameter error */
    HASH_RET_LENGTHERR,                          /**< Argument length error */
    HASH_RET_TIMEOUTERR,                         /**< HASH Operation timeout */
    HASH_RET_DMAERR             = 0x00081001U,   /**< DMA transfer execution error */
    HASH_RET_DMA_DUPLICATE,                      /**< Attempted to execute DMA transfer during DMA transfer */
    HASH_RET_DMA_NOTRUNNING,                     /**< Suspend was executed while DMA processing was not executed */
    HASH_RET_DMA_NOTSUSPENDED,                   /**< Resume was performed when DMA processing was not suspended */
    HASH_RET_UNSUPPORTED_PARAM  = 0x0008F001U    /**< Currently not supported return value */
} e_r_hash_ret_t;

/* Todo : Delete HASH_RET_UNSUPPORTED_PARAM when supporting SHAB0. */

/** HASH Type enumeration */
typedef enum
{
    HASH_TYPE_SHA1,         /**< Calculate with SHA1 */
    HASH_TYPE_SHA224,       /**< Calculate with SHA224 */
    HASH_TYPE_SHA256,       /**< Calculate with SHA256 */
    HASH_TYPE_SHA384,       /**< Calculate with SHA384 */
    HASH_TYPE_SHA512,       /**< Calculate with SHA512 */
    HASH_TYPE_SHA512_224,   /**< Calculate with SHA512/224 */
    HASH_TYPE_SHA512_256,   /**< Calculate with SHA512/256 */
    HASH_TYPE_SHA3_256 = 8, /**< Calculate with SHA3_256 */
    HASH_TYPE_SHA3_384,     /**< Calculate with SHA3_384 */
    HASH_TYPE_SHA3_512      /**< Calculate with SHA3_512 */
} e_r_hash_type_t;

/** HMAC AES Key length type enumeration */
typedef enum
{
    HASH_AESKEY_LEN_TYPE_128,    /**< AES Key length 128 */
    HASH_AESKEY_LEN_TYPE_256,    /**< AES Key length 256 */
    HASH_AESKEY_LEN_TYPE_384,    /**< AES Key length 384 */
    HASH_AESKEY_LEN_TYPE_512,    /**< AES Key length 512 */
    HASH_AESKEY_LEN_TYPE_UNUSED  /**< AES Key unused */
} e_r_hash_aeskey_len_type_t;
/*\}*/

/*!****************************************************
 * \addtogroup  HASHHashTypes     Hash Types
 * \{**************************************************/
/** HASH context structure */
typedef struct
{
    e_r_hash_type_t  type;                                       /**< HASH type */
    uint32_t         hash_len;                                   /**< Length of hash in bytes */
    /* Total length of message in bytes */
    uint32_t         msg_total_len;
    /* Control state of HASH driver */
    uint32_t         driver_control_state;
    uint32_t         intermediate_len[HASH_IM_LEN_WORD_SIZE];    /**< Length of intermediate data */
    uint32_t         intermediate_data[HASH_IM_DATA_WORD_SIZE];  /**< Intermediate data */
    uint32_t         use_dma_transfer;                           /**< Use of DMA transfer */
    union
    {
        struct
        {
            uint32_t leftover_msg_len;                            /**< Unprocessed message size */
            uint8_t  leftover_msg[HASH_LEFTOVER_MAX];             /**< Unprocessed message buffer */
        } icup;                                                   /**< ICUP transfer parameters */
        struct
        {
            uint8_t  output[HASH_DMA_PROC_RESULT_SIZE];           /**< DMA processing result output buffer */
        } dma;                                                    /**< DMA transfer parameters */
    } transfer_prm;                                               /**< Parameters for each transfer type */
} st_r_hash_ctx_t;

/** HASH option structure */
typedef struct
{
    uint32_t use_dma_transfer;                      /**< Use of DMA transfer */
} st_r_hash_opt_t;
/*\}*/

/* HASH operation parameters */
typedef struct
{
    uint32_t total_msg_size_in_bytes;
} st_r_ope_prm_t;

/*!****************************************************
 * \addtogroup  HASHHMACTypes     HMAC Types
 * \{**************************************************/
/** HMAC context structure */
typedef struct
{
    st_r_hash_ctx_t hash_ctx;                       /**< HASH context */
    uint32_t        key_len;                        /**< HMAC key size */
    uint8_t         key[HASH_HMAC_KEY_SIZE_MAX];    /**< HMAC key buffer */
    uint32_t        use_aeskey;                     /**< Generate key from AES */
    uint32_t        aeskey_len_type;                /**< Size of key generated by AES */
} st_r_hmac_ctx_t;

/** HMAC option structure */
typedef struct
{
    st_r_hash_opt_t            hash_opt;            /**< HASH Option */
    uint32_t                   use_aeskey;          /**< Generate key from AES */
    e_r_hash_aeskey_len_type_t aeskey_len_type;     /**< Size of key generated by AES */
} st_r_hmac_opt_t;
/*\}*/

/*=====================================================================================================================
 Public global functions
=====================================================================================================================*/
/* Hash APIs */
extern e_r_hash_ret_t D_HASH_HashBegin(st_r_hash_ctx_t* const p_ctx, const e_r_hash_type_t type,
                                        const st_r_hash_opt_t* const p_opt);
extern e_r_hash_ret_t D_HASH_HashUpdate(st_r_hash_ctx_t* const p_ctx,
                                        const uint8_t* const p_msg, const uint32_t msg_len);
extern e_r_hash_ret_t D_HASH_HashEnd(st_r_hash_ctx_t* const p_ctx, uint8_t* const p_digest);
extern e_r_hash_ret_t D_HASH_SetOpePrm(st_r_hash_ctx_t* const p_ctx, st_r_ope_prm_t* const p_prm);
extern e_r_hash_ret_t D_HASH_ChkDMAApplicable(const uint8_t* p_input_area,  uint32_t input_size_in_bytes, 
                                                    uint8_t* p_output_area, uint32_t output_size_in_bytes);

/* HMAC APIs */
/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0009]:[ICUMFW_UD_S4_D_HASH_0009]
* Function Name : D_HASH_HMACBegin
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHHMACAPIs
 * \brief       HMAC initial settings and inner key settings.
 * \param       [out]  p_ctx     An address of HMAC context
 * \param       [in]   type      HASH type
 * \param       [in]   p_opt     An address of HMAC option
 * \param       [in]   p_key     An address of key data
 * \param       [in]   key_len   Byte length of key data
 * \return
 *  - #HASH_RET_OK:                   API succeeded
 *  - #HASH_RET_PARAMERR:             Argument parameter error
 *  - #HASH_RET_LENGTHERR:            Key length error
 *  - #HASH_RET_UNSUPPORTED_PARAM:    Unsupported value of hash option parameter
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #ICUM_SHACNT, #ICUM_SHASTS, #ICUM_SHADCNT, #ICUM_SHAIDAT, #ICUM_SHAFIFOCNT, #ICUM_SHADOD0, #ICUM_SHADOD1, <br>
 *    #ICUM_SHADOD2, #ICUM_SHADOD3, #ICUM_SHADOD4, #ICUM_SHADOD5, #ICUM_SHADOD6, #ICUM_SHADOD7, #ICUM_SHADOL0, <br>
 *    #ICUM_SHADOL1, #ICUM_SHASTSC (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - HMAC initial settings and inner key settings.
 *      - Processing using the argument p_opt is only performed argument checking until SHAB0 is supported.
 * 
 * Pseudo-code for the function can be found below. 
 * -# Check argument with #r_hash_hmac_chk_begin_arg
 * -# Set function result to ret
 * -# If ret == HASH_RET_OK
 *   -# HMACBegin process is executed with #r_hash_hmac_begin
 *   -# Set function result to ret
 * -# Else
 *   -# Do nothing
 * -# Return ret
 * 
 * \callgraph
 *********************************************************************************************************************/
extern e_r_hash_ret_t D_HASH_HMACBegin(st_r_hmac_ctx_t* const p_ctx, const e_r_hash_type_t type,
                                        const st_r_hmac_opt_t* const p_opt,
                                        const uint8_t* const p_key, const uint32_t key_len);

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0010]:[ICUMFW_UD_S4_D_HASH_0010]
* Function Name : D_HASH_HMACUpdate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHHMACAPIs
 * \brief       Hash block processing of message data.
 * \param       [in,out]  p_ctx     An address of HMAC context
 * \param       [in]      p_msg     An address of message
 * \param       [in]      msg_len   Byte length of message
 * \return
 *  - #HASH_RET_OK:              API succeeded
 *  - #HASH_RET_PARAMERR:        Argument parameter error
 *  - #HASH_RET_TIMEOUTERR:      Operation timeout
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #ICUM_SHAIDAT, #ICUM_SHAFIFOCNT, #ICUM_SHASTS, #ICUM_SHASTSC, #ICUM_SHADID0, #ICUM_SHADID1, #ICUM_SHADID2, <br>
 *    #ICUM_SHADID3, #ICUM_SHADID4, #ICUM_SHADID5, #ICUM_SHADID6, #ICUM_SHADID7, #ICUM_SHADIL0, #ICUM_SHADIL1, <br>
 *    #ICUM_SHADOD0, #ICUM_SHADOD1, #ICUM_SHADOD2, #ICUM_SHADOD3, #ICUM_SHADOD4, #ICUM_SHADOD5, #ICUM_SHADOD6, <br>
 *    #ICUM_SHADOD7, #ICUM_SHADOL0, #ICUM_SHADOL1, #ICUM_SHADDCNT (Used by sub functions)
 * \par Precondition
 *  - #D_HASH_HMACBegin to be called in advance.
 * \par Detailed description
 *      - Hash block processing of message data.
 *      - If you want to split the message and enter it, call this function in multiple.
 * 
 * Pseudo-code for the function can be found below. 
 * -# If p_ctx != NULL
 *   -# Check argument with #r_hash_hmac_chk_update_arg
 *   -# Set function result to ret
 *   -# If ret == HASH_RET_OK
 *     -# HMACUpdate process is executed with #r_hash_cmn_update
 *     -# Set function result to ret
 *   -# Else
 *     -# Do nothing
 * -# Else
 *   -# Set #HASH_RET_PARAMERR to ret
 * -# Return ret
 * 
 * \callgraph
 *********************************************************************************************************************/
extern e_r_hash_ret_t D_HASH_HMACUpdate(st_r_hmac_ctx_t* const p_ctx,
                                        const uint8_t* const p_msg, const uint32_t msg_len);

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0011]:[ICUMFW_UD_S4_D_HASH_0011]
* Function Name : D_HASH_HMACEnd
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHHMACAPIs
 * \brief       Perform final processing of HMAC to acquire tags.
 * \param       [in]  p_ctx       An address of HMAC context
 * \param       [out] p_tag       An address of tag
 * \return
 *  - #HASH_RET_OK:              API succeeded
 *  - #HASH_RET_PARAMERR:        Argument parameter error
 *  - #HASH_RET_TIMEOUTERR:      Operation timeout
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #ICUM_SHACNT, #ICUM_SHASTS, #ICUM_SHADID0, #ICUM_SHADID1, #ICUM_SHADID2, #ICUM_SHADID3, #ICUM_SHADID4, <br>
 *    #ICUM_SHADID5, #ICUM_SHADID6, #ICUM_SHADID7, #ICUM_SHADIL0, #ICUM_SHADIL1, #ICUM_SHADDCNT, #ICUM_SHADCNT, <br>
 *    #ICUM_SHAIDAT, #ICUM_SHAFIFOCNT, #ICUM_SHAODAT, #ICUM_SHASTSC (Used by sub functions)
 * \par Precondition
 *  - #D_HASH_HMACUpdate to be called in advance.
 * \par Detailed description
 *      - Perform the final processing of the hash block and set the OuterKey, and get the Tag.
 * 
 * Pseudo-code for the function can be found below. 
 * -# Check argument with #r_hash_hmac_chk_end_arg
 * -# Set function result to ret
 * -# If ret == HASH_RET_OK
 *   -# HashEnd process is executed with #r_hash_hmac_end
 *   -# Set function result to ret
 * -# Else
 *   -# Do nothing
 * -# Return ret
 * 
 * \callgraph
 *********************************************************************************************************************/
extern e_r_hash_ret_t D_HASH_HMACEnd(st_r_hmac_ctx_t* const p_ctx, uint8_t* const p_tag);

/* Common APIs */
/*!****************************************************************************************************************//**
 * \ingroup     HASHCommonAPIs
 * \brief       Check the DMA execution status
 * \param       [in]   type      HASH type
 * \return
 *  - #HASH_RET_OK:                   API succeeded (Not Processing in DMA transfer mode)
 *  - #HASH_RET_DMA_RUNNING:          Processing in DMA transfer mode
 *  - #HASH_RET_PARAMERR:             Argument parameter error
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Check the execution status of DMA transfer.
 * 
 * \callgraph
 *********************************************************************************************************************/
extern e_r_hash_ret_t D_HASH_ChkDMABusy(const e_r_hash_type_t type);

#endif /* R_ICUM_HASH_H */
/*=====================================================================================================================
 End of File
=====================================================================================================================*/

