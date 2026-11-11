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

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
static e_r_hash_ret_t d_hash_cmn_sha3_wait_sts(volatile const uint32_t* const p_reg,
                                               const uint32_t pos, const uint32_t break_sts);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0054]:[ICUMFW_UD_S4_D_HASH_0054]
* Function name: d_hash_cmn_sha3_get_length
* Description  : Get hash length from HASH type
* Arguments    : (IN) const e_r_hash_type_t type
* Return Value : none
***************************************************************************************/
uint32_t d_hash_cmn_sha3_get_length(const e_r_hash_type_t type)
{
    uint32_t hash_len;

    hash_len = LENGTH_ERR;

    /* set HASH length */
    switch (type)
    {
        case HASH_TYPE_SHA3_256:
            hash_len = SHA3_256_LEN;
            break;

        case HASH_TYPE_SHA3_384:
            hash_len = SHA3_384_LEN;
            break;

        case HASH_TYPE_SHA3_512:
            hash_len = SHA3_512_LEN;
            break;

        default:
            break;
    } /* end of switch(type) */

    return hash_len;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0055]:[ICUMFW_UD_S4_D_HASH_0055]
* Function name: d_hash_cmn_sha3_context_chk_arg
* Description  : Context member check
* Arguments    : (IN) const st_r_hash_ctx_t* const p_ctx
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_LENGTHERR
***************************************************************************************/
e_r_hash_ret_t d_hash_cmn_sha3_context_chk_arg(const st_r_hash_ctx_t* const p_ctx)
{
    e_r_hash_ret_t ret;
    uint32_t       hash_block_size;

    ret = HASH_RET_OK;
    hash_block_size = VARIABLE_INIT;

    /* NULL check */
    if (NULL != p_ctx)
    {
        /* check HASH type */
        switch (p_ctx->type)
        {
            case HASH_TYPE_SHA3_256:
                if (SHA3_256_LEN != p_ctx->hash_len)
                {
                    ret = HASH_RET_PARAMERR;
                }
                else
                {
                    /* do nothing */
                }
                break;

            case HASH_TYPE_SHA3_384:
                if (SHA3_384_LEN != p_ctx->hash_len)
                {
                    ret = HASH_RET_PARAMERR;
                }
                else
                {
                    /* do nothing */
                }
                break;

            case HASH_TYPE_SHA3_512:
                if (SHA3_512_LEN != p_ctx->hash_len)
                {
                    ret = HASH_RET_PARAMERR;
                }
                else
                {
                    /* do nothing */
                }
                break;

            default:
                ret = HASH_RET_PARAMERR;
                break;
        } /* end of switch(p_ctx->type) */

        if (HASH_RET_OK == ret)
        {
            /* get block size */
            hash_block_size = d_hash_cmn_sha3_get_block_size(p_ctx->type);

            if ((BLOCK_SIZE_ERR == hash_block_size) ||
                (p_ctx->transfer_prm.icup.leftover_msg_len > hash_block_size))
            {
                ret = HASH_RET_LENGTHERR;
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
    } /* end of if (NULL != p_ctx) */
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0056]:[ICUMFW_UD_S4_D_HASH_0056]
* Function name: d_hash_cmn_sha3_get_block_size
* Description  : Get hash block size from HASH type
* Arguments    : (IN) const e_r_hash_type_t type
* Return Value : none
***************************************************************************************/
uint32_t d_hash_cmn_sha3_get_block_size(const e_r_hash_type_t type)
{
    uint32_t block_size;

    block_size = BLOCK_SIZE_ERR;

    switch (type)
    {
        case HASH_TYPE_SHA3_256:
            block_size = BLOCK_SIZE_136_BYTE;
            break;

        case HASH_TYPE_SHA3_384:
            block_size = BLOCK_SIZE_104_BYTE;
            break;

        case HASH_TYPE_SHA3_512:
            block_size = BLOCK_SIZE_72_BYTE;
            break;

        default:
            break;
    } /* end of switch(type) */

    return block_size;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0057]:[ICUMFW_UD_S4_D_HASH_0057]
* Function name: d_hash_cmn_sha3_get_mode
* Description  : Get operating mode from HASH type
* Arguments    : (IN) const e_r_hash_type_t type
* Return Value : none
***************************************************************************************/
uint32_t d_hash_cmn_sha3_get_mode(const e_r_hash_type_t type)
{
    uint32_t mode;

    mode = VARIABLE_INIT;

    switch(type)
    {
        case HASH_TYPE_SHA3_256:
            mode = CNT_MODE_VAL_SHA3_256;
            break;

        case HASH_TYPE_SHA3_384:
            mode = CNT_MODE_VAL_SHA3_384;
            break;

        case HASH_TYPE_SHA3_512:
            mode = CNT_MODE_VAL_SHA3_512;
            break;

        default:
            mode = UNSUPPORTED_TYPE;
            break;
    } /* end of switch(type) */

    return mode;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0058]:[ICUMFW_UD_S4_D_HASH_0058]
* Function name: d_hash_cmn_sha3_init_register
* Description  : SHA3A0 software initialize
* Arguments    : (IN) const uint32_t mode
* Return Value : HASH_RET_OK
*                HASH_RET_TIMEOUTERR
***************************************************************************************/
e_r_hash_ret_t d_hash_cmn_sha3_init_register(const uint32_t mode)
{
    e_r_hash_ret_t ret;

    ret = HASH_RET_OK;

    /* ICUMX_SHA3A0 initialize and mode set and endian set */
    ICUM_SHA3A0INT = (REGISTER_SET_1 << INIT_INIT_POS_SHA3);

    ICUM_SHA3A0CNT0 = (REGISTER_SET_0 << CNT0_ENDIAN_POS_SHA3) |
                      (mode << CNT0_MODE_POS_SHA3);

    /* wait for the state to change */
    ret = d_hash_cmn_sha3_wait_sts(&ICUM_SHA3A0STS, STS_IDATA_READY_POS_SHA3, REGISTER_SET_1);

    return ret;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0059]:[ICUMFW_UD_S4_D_HASH_0059]
* Function name: d_hash_cmn_sha3_get_middle
* Description  : Get intermediate data
* Arguments    : (IN) uint32_t* const p_im_data
*                     uint32_t* const p_im_len
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_TIMEOUTERR
***************************************************************************************/
e_r_hash_ret_t d_hash_cmn_sha3_get_middle(uint32_t* const p_im_data, uint32_t* const p_im_len)
{
    e_r_hash_ret_t ret;
    uint32_t i;

    ret = HASH_RET_OK;
    i = VARIABLE_INIT;

    /* NULL check */
    if ((NULL != p_im_data) && (NULL != p_im_len))
    {
        /* Define macro ICUM_SHA3A0STS is cast to access a register */
        ret = d_hash_cmn_sha3_wait_sts(&ICUM_SHA3A0STS, STS_BUSY_POS_SHA3, REGISTER_SET_0);

        if (HASH_RET_OK == ret)
        {
            for (; i < REMAINING_DATA_SET; i++)
            {
                p_im_data[i] = ICUM_SHA3A0ISO;
            }

            p_im_len[0] = ICUM_SHA3A0ILEN0;

            p_im_len[1] = ICUM_SHA3A0ILEN1;
        }
        else
        {
            /* do nothing */
        }
    } /* if ((NULL != p_im_data) && (NULL != p_im_len)) */
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0060]:[ICUMFW_UD_S4_D_HASH_0060]
* Function name: d_hash_cmn_sha3_exec_block
* Description  : Perform HASH block processing
* Arguments    : (IN)     const uint8_t* const p_msg,
*                         const uint32_t msg_len,
*                         const uint32_t hash_block_size
*                (IN-OUT) st_r_hash_ctx_t* const p_ctx
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_LENGTHERR
*                HASH_RET_TIMEOUTERR
***************************************************************************************/
e_r_hash_ret_t d_hash_cmn_sha3_exec_block(st_r_hash_ctx_t* const p_ctx, const uint8_t* const p_msg,
                                          const uint32_t msg_len, const uint32_t hash_block_size)
{
    e_r_hash_ret_t ret;
    uint32_t       rem_size;
    uint32_t       msg_offset;

    ret = HASH_RET_OK;
    rem_size = msg_len;
    msg_offset = VARIABLE_INIT;

    /* NULL check */
    if (NULL != p_ctx)
    {
        /* Buffer overflow prevention range check */
        if (HASH_LEFTOVER_MAX > p_ctx->transfer_prm.icup.leftover_msg_len)
        {
            /* Process the last unprocessed data */
            if (REMAINING_DATA_CHECK_0 != p_ctx->transfer_prm.icup.leftover_msg_len)
            {
                /* remaining data + msg data create */
                r_hash_cmn_cpy_byte(&p_ctx->transfer_prm.icup.leftover_msg
                                                       [p_ctx->transfer_prm.icup.leftover_msg_len],
                                    &p_msg[msg_offset],
                                    (hash_block_size - p_ctx->transfer_prm.icup.leftover_msg_len));
                d_hash_cmn_sha3_set_idat(p_ctx->transfer_prm.icup.leftover_msg, hash_block_size);
                msg_offset = (hash_block_size - p_ctx->transfer_prm.icup.leftover_msg_len);
                rem_size -= (hash_block_size - p_ctx->transfer_prm.icup.leftover_msg_len);

                r_hash_cmn_set_byte(p_ctx->transfer_prm.icup.leftover_msg, ZERO_CLEAR, HASH_LEFTOVER_MAX);
                p_ctx->transfer_prm.icup.leftover_msg_len = REMAINING_DATA_ZERO;

                /* wait for IDATA_READY */
                ret = d_hash_cmn_sha3_wait_sts(&ICUM_SHA3A0STS, STS_IDATA_READY_POS_SHA3, REGISTER_SET_1);
            } /* end of if (REMAINING_DATA_CHECK_0 != p_ctx->transfer_prm.icup.leftover_msg_len) */
            else
            {
                /* do nothing */
            }

            if (HASH_RET_OK == ret)
            {
                /* Process the msg data */
                while (rem_size >= hash_block_size)
                {
                    d_hash_cmn_sha3_set_idat(&p_msg[msg_offset], hash_block_size);
                    msg_offset += hash_block_size;
                    rem_size -= hash_block_size;

                    /* wait for IDATA_READY */
                    ret = d_hash_cmn_sha3_wait_sts(&ICUM_SHA3A0STS, STS_IDATA_READY_POS_SHA3, REGISTER_SET_1);

                    if (HASH_RET_OK != ret)
                    {
                        break;
                    }
                    else
                    {
                        /* do nothing */
                    }
                } /* end of while (rem_size >= hash_block_size) */

                if (HASH_RET_OK == ret)
                {
                    /* save remaining data */
                    if (SAVE_DATA_CHECK_0 != rem_size)
                    {
                        r_hash_cmn_cpy_byte(p_ctx->transfer_prm.icup.leftover_msg, &p_msg[msg_offset], rem_size);
                        p_ctx->transfer_prm.icup.leftover_msg_len = rem_size;
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
            } /* end of if (HASH_RET_OK = ret) */
            else
            {
                /* do nothing */
            }
            if (HASH_RET_OK == ret)
            {
                /* get intermediate data in register */
                ret = d_hash_cmn_sha3_get_middle(p_ctx->intermediate_data, p_ctx->intermediate_len);
            }
            else
            {
                /* do nothing */
            }
        } /* end of if (HASH_LEFTOVER_MAX > p_ctx->transfer_prm.icup.leftover_msg_len) */
        else
        {
            ret = HASH_RET_LENGTHERR;
        }
    } /* end of if (NULL != p_ctx) */
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0061]:[ICUMFW_UD_S4_D_HASH_0061]
* Function name: d_hash_cmn_sha3_set_idat
* Description  : Set data in register
* Arguments    : (IN) const uint8_t* const p_data,
*                     const uint32_t len,
* Return Value : none
***************************************************************************************/
void d_hash_cmn_sha3_set_idat(const uint8_t* const p_data, const uint32_t len)
{
    uint32_t         loop_max;
    uint32_t         i;
    ptr_manip_sha3_t ptr_addr;

    loop_max = (r_hash_cmn_roundup_4(len) / HASH_WORD_SIZE);
    i = VARIABLE_INIT;
    ptr_addr.p_ptr_8 = p_data;

    /* set the data to IDAT register */
    for(; i < loop_max; i++)
    {
        /* Define macro ICUM_SHA3A0IDAT is cast to access a register */
        ICUM_SHA3A0IDAT = ptr_addr.p_temp[i];
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0062]:[ICUMFW_UD_S4_D_HASH_0062]
* Function name: d_hash_cmn_sha3_get_output_data
* Description  : Get output data
* Arguments    : (IN) const uint32_t hash_len,
*                     uint8_t* const p_out
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_TIMEOUTERR
***************************************************************************************/
e_r_hash_ret_t d_hash_cmn_sha3_get_output_data(const uint32_t hash_len, uint8_t* const p_out)
{
    e_r_hash_ret_t   ret;
    uint32_t         i;
    ptr_manip_sha3_t ptr_addr;

    ret = HASH_RET_OK;
    i = VARIABLE_INIT;
    ptr_addr.p_ptr_8 = p_out;

    /* NULL check */
    if (NULL != p_out)
    {
        /* wait for the state to changed the state to ODATA_VALID */
        /* Define macro ICUM_SHA3A0STS is cast to access a register */
        ret = d_hash_cmn_sha3_wait_sts(&ICUM_SHA3A0STS, STS_ODATA_VALID_POS_SHA3, REGISTER_SET_1);

        if (HASH_RET_OK == ret)
        {
            /* get output data */
            for (; i < (hash_len / HASH_WORD_SIZE); i++)
            {
                ptr_addr.p_out_buf[i] = ICUM_SHA3A0ODAT;
            }
        }
        else
        {
            /* do nothing */
        }
    } /* end of if (NULL != p_out) */
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0079]:[ICUMFW_UD_S4_D_HASH_0079]
* Function name: d_hash_cmn_sha3_init_reg_dma
* Description  : SHA3A0DMA initialize
* Arguments    : (IN) const uint32_t mode
* Return Value : none
***************************************************************************************/
void d_hash_cmn_sha3_init_reg_dma(const uint32_t mode)
{

    /* ICUMX_SHA3A0 initialize set */
    ICUM_SHA3A0INT = (REGISTER_SET_1 << INIT_INIT_POS_SHA3);

    /* ICUMX_SHA3A0DMA initialize set */
    ICUM_SHA3A0DMAINIT = (REGISTER_SET_1 << DMAINIT_INIT_POS_SHA3);

    /* ICUMX_SHA3A0DMA mode and endian set */
    ICUM_SHA3A0DMACNT0 = (REGISTER_SET_0 << DMACNT0_ENDIAN_POS_SHA3) |
                         (mode << DMACNT0_MODE_POS_SHA3);

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0063]:[ICUMFW_UD_S4_D_HASH_0063]
* Function name: d_hash_cmn_sha3_wait_sts
* Description  : Get output data
* Arguments    : (IN) volatile const uint32_t* const p_reg,
*                     const uint32_t pos,
*                     const uint32_t break_sts
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_TIMEOUTERR
***************************************************************************************/
static e_r_hash_ret_t d_hash_cmn_sha3_wait_sts(volatile const uint32_t* const p_reg,
                                               const uint32_t pos, const uint32_t break_sts)
{
    volatile uint32_t temp;
    uint32_t          timeout;
    e_r_hash_ret_t    ret;

    temp = VARIABLE_INIT;
    timeout = TIMEOUT_INIT_SHA3;
    ret = HASH_RET_OK;

    /* NULL check */
    if (NULL != p_reg)
    {
        /* Wait for change to break_sts until timeout */
        while (TIMEOUT_CHECK_0 != timeout)
        {
            temp = *p_reg;

            if ((break_sts << pos) == (temp & (REGISTER_SET_1 << pos)))
            {
                break;
            }
            else
            {
                /* do nothing */
            }

            timeout--;
        } /* end of while (TIMEOUT_CHECK_0 != timeout) */

        if (TIMEOUT_CHECK_0 == timeout)
        {
            ret = HASH_RET_TIMEOUTERR;
        }
        else
        {
            /* do nothing */
        }
    } /* end of if (NULL != p_reg) */
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}

