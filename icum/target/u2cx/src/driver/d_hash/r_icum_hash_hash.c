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
#include "r_icum_hash_hash.h"

/*!***********************************************************
 * \addtogroup  HASHHash
 * \{
 * \addtogroup  HASHHashTypes            Hash Types
 * \addtogroup  HASHHashAPIs             Hash APIs
 * \addtogroup  HASHHashAPIsInternal     Hash Internal APIs
 * \}*********************************************************/

/*=====================================================================================================================
 Public function definitions
=====================================================================================================================*/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0004]:[ICUMFW_UD_S4_D_HASH_0004]
* Function name: r_hash_hash_chk_begin_arg
* Description  : HASHBegin API argument check
* Arguments    : (IN) const st_r_hash_ctx_t* const p_ctx,
*                     const e_r_hash_type_t type,
*                     const st_r_hash_opt_t* const p_opt
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
***************************************************************************************/
e_r_hash_ret_t r_hash_hash_chk_begin_arg(const st_r_hash_ctx_t* const p_ctx, const e_r_hash_type_t type,
                                         const st_r_hash_opt_t* const p_opt)
{
    e_r_hash_ret_t ret;

    ret = HASH_RET_OK;

    /* NULL check */
    if ((NULL != p_ctx) && (NULL != p_opt))
    {
        /* check the type */
        ret = r_hash_cmn_chk_type(type);

        if (HASH_RET_OK == ret)
        {
            if ((HASH_USE_ICUP_TRANSFER == p_opt->use_dma_transfer) ||
                (HASH_USE_DMA_TRANSFER == p_opt->use_dma_transfer))
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
            /* do nothing */
        }
    } /* end of if ((NULL != p_ctx) && (NULL != p_opt)) */
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0005]:[ICUMFW_UD_S4_D_HASH_0005]
* Function name: r_hash_hash_begin
* Description  : HASH Initialize
* Arguments    : (IN)     const e_r_hash_type_t type
*                         const st_r_hash_opt_t* const p_opt
*                (IN-OUT) const st_r_hash_ctx_t* const p_ctx
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_LENGTHERR
***************************************************************************************/
e_r_hash_ret_t r_hash_hash_begin(st_r_hash_ctx_t* const p_ctx, const e_r_hash_type_t type,
                                 const st_r_hash_opt_t* const p_opt)
{
    e_r_hash_ret_t ret;

    ret = HASH_RET_OK;

    /* NULL check */
    if ((NULL != p_ctx) && (NULL != p_opt))
    {
        /* Initialize param */
        r_hash_cmn_set_byte((uint8_t*)p_ctx, ZERO_CLEAR, sizeof(st_r_hash_ctx_t));

        /* set HASH type */
        p_ctx->type = type;

        if (HASH_USE_DMA_TRANSFER == p_opt->use_dma_transfer)
        {
            /* DMA transfer status setting */
            ret = d_hash_cmn_set_dma_status(type, DMA_STATUS_RUNNING);

            if (HASH_RET_OK == ret)
            {
                /* set use DMA transfer */
                p_ctx->use_dma_transfer = p_opt->use_dma_transfer;
            }
            else
            {
                /* set use ICUP transfer */
                p_ctx->use_dma_transfer = HASH_USE_ICUP_TRANSFER;

                ret = HASH_RET_OK;
            }
        } /* end if if (HASH_USE_DMA_TRANSFER == p_opt->use_dma_transfer) */
        else
        {
            /* do nothing */
        }

        /* set HASH length */
        p_ctx->hash_len = r_hash_cmn_get_length(p_ctx->type);

        if (LENGTH_ERR == p_ctx->hash_len)
        {
            ret = HASH_RET_LENGTHERR;
        }
        else
        {
            /* do nothing */
        }
    } /* end of if ((NULL != p_ctx) && (NULL != p_opt)) */
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return (ret);
}

/**********************************************************************************************************************
* Function Name : r_hash_hash_chk_update_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHHashAPIsInternal
 * \brief       HASHUpdate API argument check
 * \param       [in] p_ctx           An address of HASH context
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
 *      - HASHUpdate process argument check.
 *
 * Pseudo-code for the function can be found below. 
 * -# Check the context with #r_hash_cmn_context_chk_arg
 * -# Set function result to ret
 * -# If ret == HASH_RET_OK
 *   -# If (p_msg == NULL) && (msg_len != 0U)
 *     -# Set #HASH_RET_PARAMERR to ret
 *   -# Else
 *     -# Do nothing
 * -# Else
 *   -# Do nothing
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
e_r_hash_ret_t r_hash_hash_chk_update_arg(const st_r_hash_ctx_t* const p_ctx,
                                            const uint8_t* const p_msg, const uint32_t msg_len)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t ret;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* check the context */
    ret = r_hash_cmn_context_chk_arg(p_ctx);

    return ret;
}
/**********************************************************************************************************************
* End of function r_hash_hash_chk_update_arg()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_hash_hash_chk_end_arg
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHHashAPIsInternal
 * \brief       HASHEnd API argument check
 * \param       [in] p_ctx           An address of HASH context
 * \param       [in] p_digest        An address of digest
 * \return
 *  - #HASH_RET_OK:                  Successfully done
 *  - #HASH_RET_PARAMERR:            Argument parameter error
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - HASHEnd process argument check.
 *
 * Pseudo-code for the function can be found below. 
 * -# Check the context with #r_hash_cmn_context_chk_arg
 * -# Set function result to ret
 * -# If ret == HASH_RET_OK
 *   -# If p_digest != NULL
 *     -# Do nothing
 *   -# Else
 *     -# Set #HASH_RET_PARAMERR to ret
 * -# Else
 *   -# Do nothing
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
e_r_hash_ret_t r_hash_hash_chk_end_arg(const st_r_hash_ctx_t* const p_ctx, const uint8_t* const p_digest)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t ret;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* check the context */
    ret = r_hash_cmn_context_chk_arg(p_ctx);

    if(ret == HASH_RET_OK)
    {
        /* NULL check */
        if(p_digest != NULL)
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
        /* do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_hash_hash_chk_end_arg()
**********************************************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0008]:[ICUMFW_UD_S4_D_HASH_0008]
* Function name: r_hash_hash_end
* Description  : Perform hash block final processing and get digest
* Arguments    : (IN)  st_r_hash_ctx_t* const p_ctx
*                (OUT) uint8_t* const p_digest
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_LENGTHERR
*                HASH_RET_TIMEOUTERR
*                HASH_RET_DMAERR
***************************************************************************************/
e_r_hash_ret_t r_hash_hash_end(st_r_hash_ctx_t* const p_ctx, uint8_t* const p_digest)
{
    e_r_hash_ret_t ret;
    uint32_t       last_idata_num;
    uint32_t       roundup_len;
    uint32_t       hash_block_size;
    uint32_t       dma_status;

    ret = HASH_RET_OK;
    last_idata_num = VARIABLE_INIT;
    roundup_len = VARIABLE_INIT;
    hash_block_size = VARIABLE_INIT;
    dma_status = VARIABLE_INIT;

    /* NULL check */
    if (NULL != p_ctx)
    {
        if (HASH_USE_DMA_TRANSFER == p_ctx->use_dma_transfer)
        {
            /* Preparing the DMA output channel for use */
            ret = d_hash_cmn_prepare_dma(p_digest, DMA_OUTPUT_LEN, p_ctx->type, DMA_OUTPUT_CHANNEL);

            if (HASH_RET_OK == ret)
            {
                /* DMA execution */
                ret = d_hash_hash_transfer_dma(p_ctx);

                if (HASH_RET_OK == ret)
                {
                    /* Checking the status of DMA operation */
                    dma_status = d_hash_cmn_wait_sts_dma(p_ctx->type);

                    switch (dma_status)
                    {
                        case DMA_STATUS_NORMAL_END:
                            /* do nothing */
                            break;

                        case DMA_STATUS_ADDR_ERR:
                        case DMA_STATUS_TIMEOUT_ERR:
                            ret = HASH_RET_DMAERR;
                            break;

                        default:
                            ret = HASH_RET_PARAMERR;
                            break;
                    } /* end of switch (dma_status) */
                }
                else
                {
                    /* do nothing */
                }
            } /* end of if (HASH_RET_OK == ret) */
            else
            {
                /* do nothing */
            }

            /* Initialize DMA management table */
            /* Hash type is already checked, so return value is not checked */
            (void)d_hash_cmn_set_dma_status(p_ctx->type, DMA_STATUS_NON_EXEC);

        } /* end of if (HASH_USE_DMA_TRANSFER == p_ctx->use_dma_transfer) */
        else
        {
            /* preprocessing */
            ret = r_hash_cmn_preprocess_block(p_ctx);

            if (HASH_RET_OK == ret)
            {
                /* get block size */
                hash_block_size = r_hash_cmn_get_block_size(p_ctx->type);

                if (BLOCK_SIZE_ERR == hash_block_size)
                {
                    ret = HASH_RET_LENGTHERR;
                }
                else
                {
                    /* set input data(last block) */
                    if (REMAINING_DATA_CHECK_0 == p_ctx->transfer_prm.icup.leftover_msg_len)
                    {
                        /* Define macro ICUM_SHADCNT is cast to access a register */
                        ICUM_SHADCNT =  (REGISTER_SET_1 << DCNT_IDATA_LAST_POS) |
                                        (REGISTER_SET_1 << DCNT_HASH_GET_POS)   |
                                        (REGISTER_SET_1 << DCNT_IDATA_ZERO_POS);
                    }
                    else
                    {
                        last_idata_num = p_ctx->transfer_prm.icup.leftover_msg_len % HASH_WORD_SIZE;
                        last_idata_num *= BYTE2BIT_VAL;

                        /* Define macro ICUM_SHADCNT is cast to access a register */
                        ICUM_SHADCNT =  (REGISTER_SET_1 << DCNT_IDATA_LAST_POS) |
                                        (REGISTER_SET_1 << DCNT_HASH_GET_POS)   |
                                        (last_idata_num << DCNT_IDATA_NUM_POS);

                        r_hash_cmn_set_idat(p_ctx->transfer_prm.icup.leftover_msg,
                                            p_ctx->transfer_prm.icup.leftover_msg_len);
                    }

                    /* When processing data length(round up multiple of 4) is not equal the block size, */
                    /* transfer FIFO data to SHAA0 core.                                                */
                    roundup_len = r_hash_cmn_roundup_4(p_ctx->transfer_prm.icup.leftover_msg_len);

                    if (roundup_len != hash_block_size)
                    {
                        /* Define macro ICUM_SHAFIFOCNT is cast to access a register */
                        ICUM_SHAFIFOCNT = (REGISTER_SET_1 << FIFOCNT_IDAT_POS);
                    }
                    else
                    {
                        /* do nothing */
                    }

                    /* get output data */
                    ret = r_hash_cmn_get_output_data(p_ctx->hash_len, p_digest);
                } /* end of else */
            } /* end of if (HASH_RET_OK == ret) */
            else
            {
                /* do nothing */
            }
        } /* end of else */

        /* context clear */
        r_hash_cmn_set_byte((uint8_t*)p_ctx, ZERO_CLEAR, sizeof(st_r_hash_ctx_t));
    } /* end of if(NULL != p_ctx) */
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0065]:[ICUMFW_UD_S4_D_HASH_0065]
* Function name: d_hash_hash_transfer_dma
* Description  : Start of DMA operation
* Arguments    : (IN) st_r_hash_ctx_t* const p_ctx
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
***************************************************************************************/
e_r_hash_ret_t d_hash_hash_transfer_dma(st_r_hash_ctx_t* const p_ctx)
{
    e_r_hash_ret_t ret;
    uint32_t       cnt_mode;

    ret      = HASH_RET_PARAMERR;
    cnt_mode = VARIABLE_INIT;

    /* NULL check */
    if (NULL != p_ctx)
    {
        /* get mode for Control Register */
        cnt_mode = r_hash_cmn_get_mode(p_ctx->type);

        if (UNSUPPORTED_TYPE != cnt_mode)
        {
            /* software initialize */
            d_hash_cmn_init_reg_dma(cnt_mode);

            /* ICUMX_SHAB0DMA transfer size set */
            ICUM_SHADMATSR = p_ctx->msg_total_len;

            /* ICUMX_SHAB0DMA valid bit length for the last input byte set */
            ICUM_SHADMADCNT = (REGISTER_SET_0 << DMALB_ILB_POS);

            /* ICUMX_SHAB0DMA DMA start set */
            ICUM_SHADMASTT = (REGISTER_SET_1 << DMASTT_START_POS);

            ret = HASH_RET_OK;
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

    return (ret);
}
/*=====================================================================================================================
 End of file
=====================================================================================================================*/

