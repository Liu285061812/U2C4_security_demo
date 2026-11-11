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
/**************************************************************************************/
/* Header files                                                                       */
/**************************************************************************************/
#include "r_icum_hash_api.h"
#include "r_icum_hash_cmn.h"
#include "icum_d_hash_cmn_sha3.h"
#include "icum_d_hash_hash_sha3.h"

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
static void d_hash_hash_sha3_set_middle(const uint32_t* const p_im_data, const uint32_t* const p_im_len);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0037]:[ICUMFW_UD_S4_D_HASH_0037]
* Function name: d_hash_hash_sha3_chk_begin_arg
* Description  : HASHBegin API argument check
* Arguments    : (IN) const st_r_hash_ctx_t* const p_ctx,
*                     const e_r_hash_type_t type,
*                     const st_r_hash_opt_t* const p_opt
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
***************************************************************************************/
e_r_hash_ret_t d_hash_hash_sha3_chk_begin_arg(const st_r_hash_ctx_t* const p_ctx,
                                              const st_r_hash_opt_t* const p_opt)
{
    e_r_hash_ret_t ret;

    ret = HASH_RET_OK;

    /* NULL check */
    if ((NULL != p_ctx) && (NULL != p_opt))
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
        ret = HASH_RET_PARAMERR;
    }

    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0038]:[ICUMFW_UD_S4_D_HASH_0038]
* Function name: d_hash_hash_sha3_begin
* Description  : HASH Initialize
* Arguments    : (IN)     const e_r_hash_type_t type
*                         const st_r_hash_opt_t* const p_opt
*                (IN-OUT) const st_r_hash_ctx_t* const p_ctx
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_LENGTHERR
***************************************************************************************/
e_r_hash_ret_t d_hash_hash_sha3_begin(st_r_hash_ctx_t* const p_ctx,
                                      const e_r_hash_type_t type,
                                      const st_r_hash_opt_t* const p_opt)
{
    e_r_hash_ret_t ret;
    uint32_t msg_total_len;

    ret = HASH_RET_OK;

    /* NULL check */
    if ((NULL != p_ctx) && (NULL != p_opt))
    {
        msg_total_len = p_ctx->msg_total_len;

        /* Initialize param */
        r_hash_cmn_set_byte((uint8_t*)p_ctx, ZERO_CLEAR, sizeof(st_r_hash_ctx_t));

        /* set HASH total message length */
        p_ctx->msg_total_len = msg_total_len;

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
        p_ctx->hash_len = d_hash_cmn_sha3_get_length(p_ctx->type);

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

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0039]:[ICUMFW_UD_S4_D_HASH_0039]
* Function name: d_hash_hash_sha3_chk_update_arg
* Description  : HASHUpdate API argument check
* Arguments    : (IN) const st_r_hash_ctx_t* const p_ctx,
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
***************************************************************************************/
e_r_hash_ret_t d_hash_hash_sha3_chk_update_arg(const st_r_hash_ctx_t* const p_ctx)
{
    e_r_hash_ret_t ret;

    ret = HASH_RET_OK;

    /* check the context */
    ret = d_hash_cmn_sha3_context_chk_arg(p_ctx);

    return ret;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0040]:[ICUMFW_UD_S4_D_HASH_0040]
* Function name: d_hash_hash_sha3_update
* Description  : HASH UPDATE processing(hash)
* Arguments    : (IN)     const uint8_t* const p_msg,
*                         const uint32_t msg_len
*                (IN-OUT) st_r_hash_ctx_t* const p_ctx
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_LENGTHERR
***************************************************************************************/
e_r_hash_ret_t d_hash_hash_sha3_update(st_r_hash_ctx_t* const p_ctx,
                                       const uint8_t* const p_msg, const uint32_t msg_len)
{
    e_r_hash_ret_t ret;
    uint32_t       hash_block_size;

    ret = HASH_RET_OK;
    hash_block_size = VARIABLE_INIT;

    /* NULL check */
    if (NULL != p_ctx)
    {
        if (HASH_USE_DMA_TRANSFER == p_ctx->use_dma_transfer)
        {
            /* Preparing the DMA input channel for use */
            ret = d_hash_cmn_prepare_dma(p_msg, msg_len, p_ctx->type, DMA_INPUT_CHANNEL);
        }
        else
        {
            /* Buffer overflow prevention range check */
            if (HASH_LEFTOVER_MAX > p_ctx->transfer_prm.icup.leftover_msg_len)
            {
                /* get block size */
                hash_block_size = d_hash_cmn_sha3_get_block_size(p_ctx->type);

                if (BLOCK_SIZE_ERR == hash_block_size)
                {
                    ret = HASH_RET_LENGTHERR;
                }
                /* when the processing data size is smaller than the block size */
                else if ((msg_len <= hash_block_size) &&
                        ((p_ctx->transfer_prm.icup.leftover_msg_len + msg_len) <= hash_block_size))
                {
                    r_hash_cmn_cpy_byte(&p_ctx->transfer_prm.icup.leftover_msg
                                                                        [p_ctx->transfer_prm.icup.leftover_msg_len],
                                        p_msg, msg_len);
                    p_ctx->transfer_prm.icup.leftover_msg_len += msg_len;

                    /* End without input processing */
                }
                else
                {
                    /* block preprocessing */
                    ret = d_hash_hash_sha3_prepare_block(p_ctx, msg_len);

                    if (HASH_RET_OK == ret)
                    {
                        ret = d_hash_cmn_sha3_exec_block(p_ctx, p_msg, msg_len, hash_block_size);
                    }
                    else
                    {
                        /* do nothing */
                    }
                } /* end of else */
            } /* end of if(HASH_LEFTOVER_MAX > p_ctx->transfer_prm.icup.leftover_msg_len) */
            else
            {
                ret = HASH_RET_LENGTHERR;
            }
        } /* end of else */
    } /* end of if (NULL != p_ctx) */
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0041]:[ICUMFW_UD_S4_D_HASH_0041]
* Function name: d_hash_cmn_sha3_prepare_block
* Description  : Intermediate data setting confirmation
* Arguments    : (IN) st_r_hash_ctx_t* const p_ctx,
*                     const uint32_t msg_len
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_TIMEOUTERR
*                HASH_RET_LENGTHERR
***************************************************************************************/
e_r_hash_ret_t d_hash_hash_sha3_prepare_block(st_r_hash_ctx_t* const p_ctx, const uint32_t msg_len)
{
    e_r_hash_ret_t ret;
    uint32_t       cnt_mode;
    uint64_t       set_msg_len;
    uint64_t       check_msg_len;

    ret = HASH_RET_OK;
    cnt_mode = VARIABLE_INIT;
    set_msg_len = VARIABLE_INIT;
    check_msg_len = VARIABLE_INIT;

    /* NULL check */
    if (NULL != p_ctx)
    {
        if (INIT_STATE_EXECUTION != (p_ctx->driver_control_state & INIT_STATE_CONT_MASK))
        {
            /* get mode for Control Register */
            cnt_mode = d_hash_cmn_sha3_get_mode(p_ctx->type);

            if (UNSUPPORTED_TYPE == cnt_mode)
            {
                ret = HASH_RET_PARAMERR;
            }
            else
            {
                /* software initialize */
                ret = d_hash_cmn_sha3_init_register(cnt_mode);

                if (HASH_RET_OK == ret)
                {
                    if (MESSAGE_LENGTH_CHECK_0 != p_ctx->msg_total_len)
                    {
                        set_msg_len = p_ctx->msg_total_len;
                        set_msg_len = (set_msg_len * MESSAGE_SIZE_BIT);

                        /* Set the total length of the message */
                        ICUM_SHA3A0ILEN0 = (set_msg_len & ICUM_ILEN0_MASK);
                        ICUM_SHA3A0ILEN1 = (set_msg_len >> ICUM_ILEN1_SHIFT);
                    }
                    else
                    {
                        ICUM_SHA3AOS = (REGISTER_SET_1 << OS_IDATA_ZERO_POS_SHA3);
                    }
                } /* end of if (HASH_RET_OK == ret) */
                else
                {
                    /* do nothing */
                }
            } /* end of else */
        } /* end of if (INIT_STATE_EXECUTION != (p_ctx->driver_control_state & INIT_STATE_CONT_MASK)) */
        else
        {
            /* do nothing */
        }

        if (HASH_RET_OK == ret)
        {
            check_msg_len = ICUM_SHA3A0ILEN1;
            check_msg_len = (check_msg_len << ICUM_ILEN1_SHIFT);
            check_msg_len = (check_msg_len | ICUM_SHA3A0ILEN0);

            check_msg_len = (check_msg_len / MESSAGE_SIZE_BIT);

            if (check_msg_len >= (uint64_t)msg_len)
            {
                /* do nothing */
            }
            else
            {
                ret = HASH_RET_LENGTHERR;
            }
        } /* end of if (HASH_RET_OK == ret) */
        else
        {
            /* do nothing */
        }

        if (HASH_RET_OK == ret)
        {
            /* set intermediate data in register */
            if ((MIDDLE_DATA_CHECK_0 != p_ctx->intermediate_len[0]) ||
                (MIDDLE_DATA_CHECK_0 != p_ctx->intermediate_len[1]) ||
                (MIDDLE_DATA_CHECK_0 != p_ctx->intermediate_len[2]) ||
                (MIDDLE_DATA_CHECK_0 != p_ctx->intermediate_len[3]))
            {
                d_hash_hash_sha3_set_middle(p_ctx->intermediate_data, p_ctx->intermediate_len);
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
        p_ctx->driver_control_state = (p_ctx->driver_control_state | INIT_STATE_EXECUTION);
    } /* end of if (NULL != p_ctx) */
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0042]:[ICUMFW_UD_S4_D_HASH_0042]
* Function name: d_hash_hash_sha3_chk_end_arg
* Description  : HASHEnd API argument check
* Arguments    : (IN) const st_r_hash_ctx_t* const p_ctx,
*                     const uint8_t* const p_digest
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
***************************************************************************************/
e_r_hash_ret_t d_hash_hash_sha3_chk_end_arg(const st_r_hash_ctx_t* const p_ctx, const uint8_t* const p_digest)
{
    e_r_hash_ret_t ret;

    ret = HASH_RET_OK;

    /* check the context */
    ret = d_hash_cmn_sha3_context_chk_arg(p_ctx);

    if (HASH_RET_OK == ret)
    {
        /* NULL check */
        if (NULL != p_digest)
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

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0043]:[ICUMFW_UD_S4_D_HASH_0043]
* Function name: d_hash_hash_sha3_end
* Description  : Perform hash block final processing and get digest
* Arguments    : (IN)  st_r_hash_ctx_t* const p_ctx
*                (OUT) uint8_t* const p_digest
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_LENGTHERR
*                HASH_RET_TIMEOUTERR
*                HASH_RET_DMAERR
***************************************************************************************/
e_r_hash_ret_t d_hash_hash_sha3_end(st_r_hash_ctx_t* const p_ctx, uint8_t* const p_digest)
{
    e_r_hash_ret_t ret;
    uint64_t       check_msg_len;
    uint32_t       dma_status;

    ret = HASH_RET_OK;
    check_msg_len = VARIABLE_INIT;
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
                ret = d_hash_hash_sha3_transfer_dma(p_ctx);

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
            ret = d_hash_hash_sha3_prepare_block(p_ctx, p_ctx->transfer_prm.icup.leftover_msg_len);

            if (HASH_RET_OK == ret)
            {
                check_msg_len = ICUM_SHA3A0ILEN1;
                check_msg_len = (check_msg_len << ICUM_ILEN1_SHIFT);
                check_msg_len = (check_msg_len | ICUM_SHA3A0ILEN0);

                check_msg_len = (check_msg_len / MESSAGE_SIZE_BIT);

                if (check_msg_len == p_ctx->transfer_prm.icup.leftover_msg_len)
                {
                    /* set input data */
                    if (REMAINING_DATA_CHECK_0 == p_ctx->transfer_prm.icup.leftover_msg_len)
                    {
                       /* do nothing */
                    }
                    else
                    {
                        d_hash_cmn_sha3_set_idat(p_ctx->transfer_prm.icup.leftover_msg,
                                                 p_ctx->transfer_prm.icup.leftover_msg_len);
                    }

                    /* get output data */
                    ret = d_hash_cmn_sha3_get_output_data(p_ctx->hash_len, p_digest);
                } /* end of if (check_msg_len == p_ctx->transfer_prm.icup.leftover_msg_len) */
                else
                {
                    ret = HASH_RET_LENGTHERR;
                }

            } /* end of if (HASH_RET_OK == ret) */
            else
            {
                /* do nothing */
            }
        } /* end of else */

        /* context clear */
        r_hash_cmn_set_byte((uint8_t*)p_ctx, ZERO_CLEAR, sizeof(st_r_hash_ctx_t));
    } /* end of if (NULL != p_ctx) */
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0066]:[ICUMFW_UD_S4_D_HASH_0066]
* Function name: d_hash_hash_sha3_transfer_dma
* Description  : Start of DMA operation
* Arguments    : (IN) st_r_hash_ctx_t* const p_ctx
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
***************************************************************************************/
e_r_hash_ret_t d_hash_hash_sha3_transfer_dma(st_r_hash_ctx_t* const p_ctx)
{
    e_r_hash_ret_t ret;
    uint32_t       cnt_mode;

    ret      = HASH_RET_PARAMERR;
    cnt_mode = VARIABLE_INIT;

    /* NULL check */
    if (NULL != p_ctx)
    {
        /* get mode for Control Register */
        cnt_mode = d_hash_cmn_sha3_get_mode(p_ctx->type);

        if (UNSUPPORTED_TYPE != cnt_mode)
        {
            /* ICUMX_SHA3A0DMA initialize */
            d_hash_cmn_sha3_init_reg_dma(cnt_mode);

            /* ICUMX_SHA3A0DMA transfer size set */
            ICUM_SHA3A0DMAITS = p_ctx->msg_total_len;
            
            /* ICUMX_SHA3A0DMA valid bit length for the last input byte set */
            ICUM_SHA3A0DMALB = (REGISTER_SET_0 << DMALB_ILB_POS_SHA3);

            /* ICUMX_SHA3A0DMA DMA start set */
            ICUM_SHA3A0DMASTT = (REGISTER_SET_1 << DMASTT_START_POS_SHA3);

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

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0044]:[ICUMFW_UD_S4_D_HASH_0044]
* Function name: d_hash_hash_sha3_set_middle
* Description  : Set intermediate data in register
* Arguments    : (IN) const uint32_t* const p_im_data,
*                     const uint32_t* const p_im_len
* Return Value : none
***************************************************************************************/
static void d_hash_hash_sha3_set_middle(const uint32_t* const p_im_data, const uint32_t* const p_im_len)
{
    uint32_t read_register;
    uint32_t i;

    read_register = VARIABLE_INIT;
    i = VARIABLE_INIT;

    /* NULL check */
    if ((NULL != p_im_data) && (NULL != p_im_len))
    {
        read_register = ICUM_SHA3A0CNT0;

        /* ICUMX_SHA3A0 initialize and mode set and endian set */
        ICUM_SHA3A0INT = (REGISTER_SET_1 << INIT_INIT_POS_SHA3);

        ICUM_SHA3A0CNT0 = (REGISTER_SET_1 << CNT0_ISI_EN_POS_SHA3) |
                          read_register;

        ICUM_SHA3A0ILEN0 = p_im_len[0];

        ICUM_SHA3A0ILEN1 = p_im_len[1];

        for (; i < REMAINING_DATA_SET; i++)
        {
            ICUM_SHA3A0ISI = p_im_data[i];
        }
    } /* end of if ((NULL != p_im_data) && (NULL != p_im_len)) */
    else
    {
        /* do nothing */
    }

    return;
}

