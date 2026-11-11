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

/* PRQA S 0303 ++ # D001 Cast for accessing the area of register address */
/* PRQA S 0751 ++ # D004 For commonality of functions, confirm that the size is correct and use the cast value */
/* PRQA S 1532 ++ # D007 Since files are grouped by function, they are called from different files. */
/* PRQA S 3383 ++ # D010 To affect processing time */
/* PRQA S 5002 ++ # D013 Exception according to file nomenclature */

/*=====================================================================================================================
 Includes <System Includes> , "Project Includes"
=====================================================================================================================*/
#include "r_icum_hash_api.h"
#include "r_icum_hash_cmn.h"
#include "r_icum_hash_hmac.h"

/*!***********************************************************
 * \addtogroup  HASHHMAC
 * \{
 * \addtogroup  HASHHMACTypes           HMAC Types
 * \addtogroup  HASHHMACAPIs            HMAC APIs
 * \addtogroup  HASHHMACAPIsInternal    HMAC Internal APIs
 * \}*********************************************************/

/*=====================================================================================================================
 Private function prototypes
=====================================================================================================================*/
static e_r_hash_ret_t r_hash_hmac_set_innerkey(const st_r_hash_ctx_t* const p_ctx,
                                                const uint8_t* const p_key, const uint32_t key_len);
static e_r_hash_ret_t r_hash_hmac_set_outerkey(const st_r_hash_ctx_t* const p_ctx,
                                                const uint8_t* const p_key, const uint32_t key_len);

/*=====================================================================================================================
 Public function definitions
=====================================================================================================================*/

/**********************************************************************************************************************
* Function Name : r_hash_hmac_chk_begin_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHHMACAPIsInternal
 * \brief       HMACBegin API argument check
 * \param       [out]  p_ctx         An address of HMAC context
 * \param       [in]   type          HASH type
 * \param       [in]   p_opt         An address of HMAC option
 * \param       [in]   p_key         An address of key data
 * \param       [in]   key_len       Byte length of key data
 * \return
 *  - #HASH_RET_OK:                  Successfully done.
 *  - #HASH_RET_PARAMERR:            Argument parameter error
 *  - #HASH_RET_LENGTHERR:           Key length error
 *  - #HASH_RET_UNSUPPORTED_PARAM:   Unsupported value of hash option parameter
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - HMACBegin process argument check
 *
 * Pseudo-code for the function can be found below. 
 * -# If (p_ctx != NULL) && (p_opt != NULL)
 *   -# Check the HASH type with #r_hash_cmn_chk_type
 *   -# Set function result to ret
 *   -# If ret == HASH_RET_OK
 *     -# If p_key != NULL
 *       -# If key_len > HASH_HMAC_KEY_SIZE_MAX
 *         -# Set #HASH_RET_LENGTHERR to ret
 *       -# Else
 *         -# If p_opt->hash_opt.use_dma_transfer == HASH_USE_DMA_TRANSFER
 *           -# Set #HASH_RET_UNSUPPORTED_PARAM to ret
 *         -# Else
 *           -# If p_opt->use_aeskey == HASH_USE_AES_KEY
 *             -# Set #HASH_RET_UNSUPPORTED_PARAM to ret
 *           -# Else
 *             -# Do nothing
 *     -# Else
 *       -# Set #HASH_RET_PARAMERR to ret
 *   -# Else
 *     -# Do nothing
 * -# Else
 *   -# Set #HASH_RET_PARAMERR to ret
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
e_r_hash_ret_t r_hash_hmac_chk_begin_arg(const st_r_hmac_ctx_t* const p_ctx, const e_r_hash_type_t type,
                                            const st_r_hmac_opt_t* const p_opt,
                                            const uint8_t* const p_key, const uint32_t key_len)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t ret;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* NULL check */
    if((p_ctx != NULL) && (p_opt != NULL))
    {
        /* check the type */
        ret = r_hash_cmn_chk_type(type);

        if(ret == HASH_RET_OK)
        {
            /* check key */
            if(p_key != NULL)
            {
                if(key_len > HASH_HMAC_KEY_SIZE_MAX)
                {
                    ret = HASH_RET_LENGTHERR;
                }
                else
                {
                    /* DMA transfer must be unused until it corresponds to SHAB0. */
                    if(p_opt->hash_opt.use_dma_transfer == HASH_USE_DMA_TRANSFER)
                    {
                        ret = HASH_RET_UNSUPPORTED_PARAM;
                    }
                    else
                    {
                        /* AES key must be unused until it corresponds to SHAB0. */
                        if(p_opt->use_aeskey == HASH_USE_AES_KEY)
                        {
                            ret = HASH_RET_UNSUPPORTED_PARAM;
                        }
                        else
                        {
                            /* Create a check function of AESKEY len type when supporting SHAB0. */
                            /* do nothing */
                        }
                    }
                }
            }
            else
            {
                ret = HASH_RET_PARAMERR;
            }
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_hash_hmac_chk_begin_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_hash_hmac_begin
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHHMACAPIsInternal
 * \brief       HMAC context initialization and inner key input
 * \param       [out]  p_ctx     An address of HMAC context
 * \param       [in]   type      HASH type
 * \param       [in]   p_key     An address of key data
 * \param       [in]   key_len   Key data size
 * \return
 *  - #HASH_RET_OK:                  API succeeded
 *  - #HASH_RET_PARAMERR:            Argument is invalid.
 *  - #HASH_RET_TIMEOUTERR:          Operation timeouted.
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #ICUM_SHACNT, #ICUM_SHASTS, #ICUM_SHADCNT, #ICUM_SHAIDAT, #ICUM_SHAFIFOCNT, #ICUM_SHADOD0, #ICUM_SHADOD1, <br>
 *    #ICUM_SHADOD2, #ICUM_SHADOD3, #ICUM_SHADOD4, #ICUM_SHADOD5, #ICUM_SHADOD6, #ICUM_SHADOD7, #ICUM_SHADOL0, <br>
 *    #ICUM_SHADOL1, #ICUM_SHASTSC (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Initialize the HMAC context.
 *      - Input inner key using key data set in context.
 *      - Get intermediate data up to here and save in context.
 * 
 * Pseudo-code for the function can be found below. 
 * -# If p_ctx != NULL
 *   -# Initialize p_ctx to 0 using #r_hash_cmn_set_byte
 *   -# Set the address of p_ctx->hash_ctx to p_hctx
 *   -# Set argument type to p_hctx->type
 *   -# Get the hash length with #r_hash_cmn_get_length and set it to p_hctx->hash_len
 *   -# Get mode for control register with #r_hash_cmn_get_mode and set it to cnt_mode
 *   -# If (cnt_mode == UNSUPPORTED_TYPE) || (p_hctx->hash_len == 0U))
 *     -# Set #HASH_RET_PARAMERR to ret
 *   -# Else
 *     -# Software initialize with #r_hash_cmn_init_register
 *     -# Set function result to ret
 *   -# If ret == HASH_RET_OK
 *     -# Set key data as context using #r_hash_cmn_cpy_byte
 *     -# Set key_len to p_ctx->key_len
 *     -# Input to inner key using #r_hash_hmac_set_innerkey
 *     -# Set function result to ret
 *     -# If ret == HASH_RET_OK
 *       -# Get intermediate data with # to inner key using #r_hash_hmac_set_innerkey
 *       -# Set function result to ret
 *     -# Else
 *       -# Do nothing
 *   -# Else
 *     -# Do nothing
 * -# Else
 *   -# Set #HASH_RET_PARAMERR to ret
 * -# Return ret
 * 
 * \callgraph
 *********************************************************************************************************************/
e_r_hash_ret_t r_hash_hmac_begin(st_r_hmac_ctx_t* const p_ctx, const e_r_hash_type_t type,
                                    const uint8_t* const p_key, const uint32_t key_len)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t   ret;
    st_r_hash_ctx_t* p_hctx;
    uint32_t         cnt_mode;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* NULL check */
    if(p_ctx != NULL)
    {
        /* Initialize param */
        r_hash_cmn_set_byte((uint8_t*)p_ctx, 0U, sizeof(st_r_hmac_ctx_t));

        /* context set */
        p_hctx = &p_ctx->hash_ctx;

        /* set HASH type */
        p_hctx->type = type;

        /* set HASH length */
        p_hctx->hash_len = r_hash_cmn_get_length(p_hctx->type);

        /* get mode for Control Register */
        cnt_mode = r_hash_cmn_get_mode(p_hctx->type);

        if((cnt_mode == UNSUPPORTED_TYPE) || (p_hctx->hash_len == 0U))
        {
            ret = HASH_RET_PARAMERR;
        }
        else
        {
            /* SHA software initialize */
            ret = r_hash_cmn_init_register(cnt_mode);
        }

        if(ret == HASH_RET_OK)
        {
            /* key set */
            r_hash_cmn_cpy_byte(&p_ctx->key[0], p_key, key_len);
            p_ctx->key_len = key_len;

            /* Inner KEY inputs operation */
            ret = r_hash_hmac_set_innerkey(p_hctx, p_ctx->key, p_ctx->key_len);

            if(ret == HASH_RET_OK)
            {
                /* get intermediate data in register */
                ret = r_hash_cmn_get_intermediate(p_hctx->intermediate_data, p_hctx->intermediate_len);
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_hash_hmac_begin()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_hash_hmac_chk_update_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHHMACAPIsInternal
 * \brief       HMACUpdate API argument check
 * \param       [in] p_ctx           An address of HMAC context
 * \param       [in] p_msg           An address of message
 * \param       [in] msg_len         The length of the message in bytes
 * \return
 *  - #HASH_RET_OK:                  Successfully done.
 *  - #HASH_RET_PARAMERR:            Argument parameter error.
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - HMACUpdate process argument check.
 *
 * Pseudo-code for the function can be found below. 
 * -# If p_ctx != NULL
 *   -# Check the context with #r_hash_cmn_context_chk_arg
 *   -# Set function result to ret
 *   -# If ret == HASH_RET_OK
 *     -# If (p_msg == NULL) && (msg_len != 0U)
 *       -# Set #HASH_RET_PARAMERR to ret
 *     -# Else
 *       -# Do nothing
 *   -# Else
 *     -# Do nothing
 * -# Else
 *   -# Set #HASH_RET_PARAMERR to ret
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
e_r_hash_ret_t r_hash_hmac_chk_update_arg(const st_r_hmac_ctx_t* const p_ctx,
                                            const uint8_t* const p_msg, const uint32_t msg_len)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t ret;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    if(p_ctx != NULL)
    {
        /* check the context */
        ret = r_hash_cmn_context_chk_arg(&p_ctx->hash_ctx);

        if(ret == HASH_RET_OK)
        {
            /* check the message */
            /* Even if p_msg is NULL, it is acceptable if msg_len is 0. */
            if((p_msg == NULL) && (msg_len != 0U))
            {
                ret = HASH_RET_PARAMERR;
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_hash_hmac_chk_update_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_hash_hmac_chk_end_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHHMACAPIsInternal
 * \brief       HMACEnd API argument check
 * \param       [in] p_ctx           An address of HMAC context
 * \param       [in] p_tag           An address of tag
 * \return
 *  - #HASH_RET_OK:                  Successfully done
 *  - #HASH_RET_PARAMERR:            Argument parameter error
 *  - #HASH_RET_LENGTHERR:           Key length error
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - HMACEnd process argument check.
 *
 * Pseudo-code for the function can be found below. 
 * -# If p_ctx != NULL
 *   -# Check the context with #r_hash_cmn_context_chk_arg
 *   -# Set function result to ret
 *   -# If ret == HASH_RET_OK
 *     -# If p_ctx->key_len <= HASH_HMAC_KEY_SIZE_MAX
 *       -# If p_tag != NULL
 *         -# Do nothing
 *       -# Else
 *         -# Set #HASH_RET_PARAMERR to ret
 *     -# Else
 *       -# Set #HASH_RET_LENGTHERR to ret
 *   -# Else
 *     -# Do nothing
 * -# Else
 *   -# Set #HASH_RET_PARAMERR to ret
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
e_r_hash_ret_t r_hash_hmac_chk_end_arg(const st_r_hmac_ctx_t* const p_ctx, const uint8_t* const p_tag)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t ret;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* NULL check */
    if(p_ctx != NULL)
    {
        /* check the context */
        ret = r_hash_cmn_context_chk_arg(&p_ctx->hash_ctx);

        if(ret == HASH_RET_OK)
        {
            if(p_ctx->key_len <= HASH_HMAC_KEY_SIZE_MAX)
            {
                /* NULL check */
                if(p_tag != NULL)
                {
                    /* do nothing */
                }
                else
                {
                    ret = HASH_RET_PARAMERR;
                }
            }
            else
            {
                ret = HASH_RET_LENGTHERR;
            }
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_hash_hmac_chk_end_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_hash_hmac_end
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHHMACAPIsInternal
 * \brief       Perform hash block final processing and outer key input and acquire Tag.
 * \param       [in] p_ctx           An address of HASH context
 * \param       [out] p_tag          An address of digest
 * \return
 *  - #HASH_RET_OK:                  Successfully done.
 *  - #HASH_RET_PARAMERR:            Argument parameter NULL.
 *  - #HASH_RET_TIMEOUTERR:          process timeouted.
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #ICUM_SHADCNT, #ICUM_SHAFIFOCNT
 *  - #ICUM_SHAIDAT, #ICUM_SHAODAT, #ICUM_SHACNT, #ICUM_SHASTS, #ICUM_SHASTSC, #ICUM_SHADID0, #ICUM_SHADID1, <br>
 *    #ICUM_SHADID2, #ICUM_SHADID3, #ICUM_SHADID4, #ICUM_SHADID5, #ICUM_SHADID6, #ICUM_SHADID7, #ICUM_SHADIL0, <br>
 *    #ICUM_SHADIL1, #ICUM_SHADDCNT (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Perform hash block final processing and outer key input, then output Tag from register.
 *
 * Pseudo-code for the function can be found below.
 * -# If p_ctx != NULL
 *   -# Set the address of p_ctx->hash_ctx to p_hctx
 *   -# Perform block process preprocessing with #r_hash_cmn_preprocess_block
 *   -# If ret == HASH_RET_OK
 *     -# Get block size with #r_hash_cmn_get_block_size and set it to hash_block_size
 *     -# If p_hctx->transfer_prm.icup.leftover_msg_len == 0U
 *       -# 1 is set to the IDATA_LAST and IDATA_ZERO bit of the register #ICUM_SHADCNT
 *     -# Else
 *       -# Set left_idata_num as the remainder of the leftover_msg_len divided by HASH_WORD_SIZE
 *       -# Convert last_idata_num from byte size to bit size
 *       -# 1 is set to the IDATA_LAST bit of the register #ICUM_SHADCNT
 *       -# last_idata_num is set to  the IDATA_NUM bit of the register #ICUM_SHADCNT
 *       -# Execute block processing for leftover_msg_len size from leftover_msg using #r_hash_cmn_set_idat
 *     -# Round up leftover_msg_len to a multiple of 4 using #r_hash_cmn_roundup_4 and set it to roundup_len.
 *     -# If roundup_len != hash_block_size
 *       -# 1 is set to the IDAT bit of the register #ICUM_SHAFIFOCNT
 *     -# Else
 *       -# Do nothing
 *     -# Wait for IDATA_READY with #r_hash_cmn_wait_for_idata_ready
 *     -# Set function result to ret
 *     -# If ret == HASH_RET_OK
 *       -# Input the outer key with #r_hash_hmac_set_outerkey
 *       -# If ret == HASH_RET_OK
 *         -# Get output data of size hash_len to p_digest with #r_hash_cmn_get_output_data
 *         -# Set function result to ret
 *       -# Else
 *         -# Do nothing
 *     -# Else
 *       -# Do nothing
 *   -# Else
 *     -# Do nothing
 * -# Else
 *   -# Set #HASH_RET_PARAMERR to ret
 * -# Clear p_ctx using #r_hash_cmn_set_byte.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
e_r_hash_ret_t r_hash_hmac_end(st_r_hmac_ctx_t* const p_ctx, uint8_t* const p_tag)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t    ret;
    st_r_hash_ctx_t*  p_hctx;
    uint32_t          last_idata_num;
    uint32_t          roundup_len;
    uint32_t          hash_block_size;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* NULL check */
    if(p_ctx != NULL)
    {
        /* context set */
        p_hctx = &p_ctx->hash_ctx;

        /* preprocessing */
        ret = r_hash_cmn_preprocess_block(p_hctx);

        if(ret == HASH_RET_OK)
        {
            /* get block size */
            hash_block_size = r_hash_cmn_get_block_size(p_hctx->type);

            if(hash_block_size == 0U)
            {
                ret = HASH_RET_LENGTHERR;
            }
            else
            {
                /* set input data(last block) */
                if(p_hctx->transfer_prm.icup.leftover_msg_len == 0U)
                {
                    /* Define macro ICUM_SHADCNT is cast to access a register */
                    ICUM_SHADCNT = (0x1U << DCNT_IDATA_LAST_POS) |
                                    (0x1U << DCNT_IDATA_ZERO_POS);

                    /* It is not set because the setting value is 0, but the subsequent bits must be 0. */
                    /* (0x0U << DCNT_HASH_GET_POS)
                       (0x0U << DCNT_IPAD_POS)  
                       (0x0U << DCNT_OPAD_POS) */
                }
                else
                {
                    last_idata_num = p_hctx->transfer_prm.icup.leftover_msg_len % HASH_WORD_SIZE;
                    last_idata_num *= BYTE2BIT_VAL;

                    /* Define macro ICUM_SHADCNT is cast to access a register */
                    ICUM_SHADCNT = (0x1U << DCNT_IDATA_LAST_POS) |
                                    (last_idata_num << DCNT_IDATA_NUM_POS);

                    /* It is not set because the setting value is 0, but the subsequent bits must be 0. */
                    /* (0x0U << DCNT_HASH_GET_POS)
                       (0x0U << DCNT_IPAD_POS)  
                       (0x0U << DCNT_OPAD_POS) */

                    r_hash_cmn_set_idat(p_hctx->transfer_prm.icup.leftover_msg,
                                        p_hctx->transfer_prm.icup.leftover_msg_len);
                }

                /* When processing data length(round up multiple of 4) is not equal the block size, 
                   transfer FIFO data to SHAA0 core. */
                roundup_len = r_hash_cmn_roundup_4(p_hctx->transfer_prm.icup.leftover_msg_len);

                if(roundup_len != hash_block_size)
                {
                    /* Define macro ICUM_SHAFIFOCNT is cast to access a register */
                    ICUM_SHAFIFOCNT = (0x1U << FIFOCNT_IDAT_POS);
                }
                else
                {
                    /* do nothing */
                }

                /* wait for IDATA_READY */
                ret = r_hash_cmn_wait_for_idata_ready();
            }

            if(ret == HASH_RET_OK)
            {
                /* Outer KEY inputs operation */
                ret = r_hash_hmac_set_outerkey(p_hctx, p_ctx->key, p_ctx->key_len);

                if(ret == HASH_RET_OK)
                {
                    /* get output data */
                    ret = r_hash_cmn_get_output_data(p_hctx->hash_len, p_tag);
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    /* context clear */
    r_hash_cmn_set_byte((uint8_t*)p_ctx, 0U, sizeof(st_r_hmac_ctx_t));

    return ret;
}
/**********************************************************************************************************************
* End of function r_hash_hmac_end()
**********************************************************************************************************************/

/*=====================================================================================================================
 Private function definitions
=====================================================================================================================*/

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0017]:[ICUMFW_UD_S4_D_HASH_0017]
* Function Name : r_hash_hmac_set_innerkey
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHHMACAPIsInternal
 * \brief       Input inner key to register
 * \param       [in] p_ctx           An address of HASH context
 * \param       [in] p_key           An address of key data
 * \param       [in] key_len         Byte length of key data
 * \return
 *  - #HASH_RET_OK:                  Successfully done.
 *  - #HASH_RET_PARAMERR:            Argument parameter NULL.
 *  - #HASH_RET_LENGTHERR:           HASH block size error.
 *  - #HASH_RET_TIMEOUTERR:          process timeouted.
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #ICUM_SHADCNT, #ICUM_SHAFIFOCNT
 *  - #ICUM_SHAIDAT,  #ICUM_SHASTS (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Input inner key to register.
 *
 * Pseudo-code for the function can be found below.
 * -# If p_ctx != NULL
 *   -# Get block size with #r_hash_cmn_get_block_size and set it to hash_block_size
 *   -# If hash_block_size == 0U
 *     -# Set #HASH_RET_LENGTHERR to ret
 *   -# Else
 *     -# If key_len == 0U
 *       -# 1 is set to the IDATA_LAST, IDATA_ZERO and IPAD bit of the register #ICUM_SHADCNT
 *     -# Else
 *       -# Set left_idata_num as the remainder of the key_len divided by HASH_WORD_SIZE
 *       -# Convert last_idata_num from byte size to bit size
 *       -# 1 is set to the IDATA_LAST and IPAD bit of the register #ICUM_SHADCNT
 *       -# last_idata_num is set to  the IDATA_NUM bit of the register #ICUM_SHADCNT
 *       -# Execute block processing for key_len size from p_key using #r_hash_cmn_set_idat
 *     -# Round up leftover_msg_len to a multiple of 4 using #r_hash_cmn_roundup_4 and set it to roundup_len.
 *     -# If roundup_len != hash_block_size
 *       -# 1 is set to the IDAT bit of the register #ICUM_SHAFIFOCNT
 *     -# Else
 *       -# Do nothing
 *     -# Wait for IDATA_READY with #r_hash_cmn_wait_for_idata_ready
 *     -# Set function result to ret
 * -# Else
 *   -# Set #HASH_RET_PARAMERR to ret
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static e_r_hash_ret_t r_hash_hmac_set_innerkey(const st_r_hash_ctx_t* const p_ctx,
                                                const uint8_t* const p_key, const uint32_t key_len)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t ret;
    uint32_t       last_idata_num;
    uint32_t       roundup_len;
    uint32_t       hash_block_size;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* NULL check */
    if(p_ctx != NULL)
    {
        /* get block size */
        hash_block_size = r_hash_cmn_get_block_size(p_ctx->type);

        if(hash_block_size == 0U)
        {
            ret = HASH_RET_LENGTHERR;
        }
        else
        {
            /* set input data(InnerKey) */
            if(key_len == 0U)
            {
                /* Define macro ICUM_SHADCNT is cast to access a register */
                ICUM_SHADCNT = (0x1U << DCNT_IDATA_LAST_POS)  |
                                (0x1U << DCNT_IDATA_ZERO_POS) |
                                (0x1U << DCNT_IPAD_POS);

                /* It is not set because the setting value is 0, but the subsequent bits must be 0. */
                /* (0x0U << DCNT_HASH_GET_POS)
                   (0x0U << DCNT_OPAD_POS) */
            }
            else
            {
                last_idata_num = key_len % HASH_WORD_SIZE;
                last_idata_num *= BYTE2BIT_VAL;

                /* Define macro ICUM_SHADCNT is cast to access a register */
                ICUM_SHADCNT = (0x1U << DCNT_IDATA_LAST_POS) |
                                (last_idata_num << DCNT_IDATA_NUM_POS) |
                                (0x1U << DCNT_IPAD_POS);

                /* It is not set because the setting value is 0, but the subsequent bits must be 0. */
                /* (0x0U << DCNT_HASH_GET_POS)
                   (0x0U << DCNT_OPAD_POS) */

                r_hash_cmn_set_idat(p_key, key_len);
            }

            /* When processing data length(round up multiple of 4) is not equal the block size, 
               transfer FIFO data to SHAA0 core. */
            roundup_len = r_hash_cmn_roundup_4(key_len);

            if(roundup_len != hash_block_size)
            {
                /* Define macro ICUM_SHAFIFOCNT is cast to access a register */
                ICUM_SHAFIFOCNT = (0x1U << FIFOCNT_IDAT_POS);
            }
            else
            {
                /* do nothing */
            }

            /* wait for IDATA_READY */
            ret = r_hash_cmn_wait_for_idata_ready();
        }
    }
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_hash_hmac_set_innerkey()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0018]:[ICUMFW_UD_S4_D_HASH_0018]
* Function Name : r_hash_hmac_set_outerkey
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHHMACAPIsInternal
 * \brief       Input outer key to register
 * \param       [in] p_ctx           An address of HASH context
 * \param       [in] p_key           An address of key data
 * \param       [in] key_len         Byte length of key data
 * \return
 *  - #HASH_RET_OK:                  Successfully done.
 *  - #HASH_RET_PARAMERR:            Argument parameter NULL.
 *  - #HASH_RET_LENGTHERR:           HASH block size error.
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #ICUM_SHADCNT, #ICUM_SHAFIFOCNT
 *  - #ICUM_SHAIDAT (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Input outer key to register.
 *
 * Pseudo-code for the function can be found below.
 * -# Set #HASH_RET_OK to ret
 * -# If p_ctx != NULL
 *   -# Get block size with #r_hash_cmn_get_block_size and set it to hash_block_size
 *   -# If hash_block_size == 0U
 *     -# Set #HASH_RET_LENGTHERR to ret
 *   -# Else
 *     -# If key_len == 0U
 *       -# 1 is set to the IDATA_LAST, HASH_GET, IDATA_ZERO and OPAD bit of the register #ICUM_SHADCNT
 *     -# Else
 *       -# Set left_idata_num as the remainder of the key_len divided by HASH_WORD_SIZE
 *       -# Convert last_idata_num from byte size to bit size
 *       -# 1 is set to the IDATA_LAST, HASH_GET and OPAD bit of the register #ICUM_SHADCNT
 *       -# last_idata_num is set to  the IDATA_NUM bit of the register #ICUM_SHADCNT
 *       -# Execute block processing for key_len size from p_key using #r_hash_cmn_set_idat
 *     -# Round up leftover_msg_len to a multiple of 4 using #r_hash_cmn_roundup_4 and set it to roundup_len.
 *     -# If roundup_len != hash_block_size
 *       -# 1 is set to the IDAT bit of the register #ICUM_SHAFIFOCNT
 *     -# Else
 *       -# Do nothing
 * -# Else
 *   -# Set #HASH_RET_PARAMERR to ret
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static e_r_hash_ret_t r_hash_hmac_set_outerkey(const st_r_hash_ctx_t* const p_ctx,
                                                const uint8_t* const p_key, const uint32_t key_len)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t ret = HASH_RET_OK;
    uint32_t last_idata_num;
    uint32_t roundup_len;
    uint32_t hash_block_size;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* NULL check */
    if(p_ctx != NULL)
    {
        /* get block size */
        hash_block_size = r_hash_cmn_get_block_size(p_ctx->type);

        if(hash_block_size == 0U)
        {
            ret = HASH_RET_LENGTHERR;
        }
        else
        {
            /* set input data(OuterKey) */
            if(key_len == 0U)
            {
                /* Define macro ICUM_SHADCNT is cast to access a register */
                ICUM_SHADCNT = (0x1U << DCNT_IDATA_LAST_POS) |
                                (0x1U << DCNT_HASH_GET_POS)   |
                                (0x1U << DCNT_IDATA_ZERO_POS) |
                                (0x1U << DCNT_OPAD_POS);

                /* It is not set because the setting value is 0, but the subsequent bits must be 0. */
                /* (0x0U << DCNT_IPAD_POS) */
            }
            else
            {
                last_idata_num = key_len % HASH_WORD_SIZE;
                last_idata_num *= BYTE2BIT_VAL;

                /* Define macro ICUM_SHADCNT is cast to access a register */
                ICUM_SHADCNT = (0x1U << DCNT_IDATA_LAST_POS) |
                                (0x1U << DCNT_HASH_GET_POS)  |
                                (last_idata_num << DCNT_IDATA_NUM_POS) |
                                (0x1U << DCNT_OPAD_POS);

                /* It is not set because the setting value is 0, but the subsequent bits must be 0. */
                /* (0x0U << DCNT_IPAD_POS) */

                r_hash_cmn_set_idat(p_key, key_len);
            }

            /* When processing data length(round up multiple of 4) is not equal the block size, 
               transfer FIFO data to SHAA0 core. */
            roundup_len = r_hash_cmn_roundup_4(key_len);

            if(roundup_len != hash_block_size)
            {
                /* Define macro ICUM_SHAFIFOCNT is cast to access a register */
                ICUM_SHAFIFOCNT = (0x1U << FIFOCNT_IDAT_POS);
            }
            else
            {
                /* do nothing */
            }
        }
    }
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_hash_hmac_set_outerkey()
**********************************************************************************************************************/

/*=====================================================================================================================
 End of file
=====================================================================================================================*/

