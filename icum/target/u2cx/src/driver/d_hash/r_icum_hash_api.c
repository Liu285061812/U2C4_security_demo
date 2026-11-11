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

/* PRQA S 5002 ++ # D013 Exception according to file nomenclature */

/**************************************************************************************/
/* Header files                                                                       */
/**************************************************************************************/
#include "r_icum_hash_api.h"
#include "r_icum_hash_cmn.h"
#include "r_icum_hash_hash.h"
#include "r_icum_hash_hmac.h"
#include "icum_d_hash_hash_sha3.h"

/**************************************************************************************/
/* Public function definitions                                                        */
/**************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0001]:[ICUMFW_UD_S4_D_HASH_0001]
* Function name: D_HASH_HashBegin
* Description  : Initialize the HASH context
* Arguments    : (IN)     st_r_hash_ctx_t* const p_ctx,
*                         const st_r_hash_opt_t* const p_opt
*                (IN-OUT) st_r_hash_ctx_t* const p_ctx
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
***************************************************************************************/
e_r_hash_ret_t D_HASH_HashBegin(st_r_hash_ctx_t* const p_ctx, const e_r_hash_type_t type,
                                const st_r_hash_opt_t* const p_opt)
{
    e_r_hash_ret_t ret;

    ret = HASH_RET_OK;

    switch (type)
    {
        case HASH_TYPE_SHA3_256:
        case HASH_TYPE_SHA3_384:
        case HASH_TYPE_SHA3_512:

            /* check arguments */
            ret = d_hash_hash_sha3_chk_begin_arg(p_ctx, p_opt);

            if (HASH_RET_OK == ret)
            {
                /* HASHBegin process */
                ret = d_hash_hash_sha3_begin(p_ctx, type, p_opt);
            }
            else
            {
                /* do nothing */
            }
            break;

        default:

            /* check arguments */
            ret = r_hash_hash_chk_begin_arg(p_ctx, type, p_opt);

            if (HASH_RET_OK == ret)
            {
                /* HASHBegin process */
                ret = r_hash_hash_begin(p_ctx, type, p_opt);
            }
            else
            {
                /* do nothing */
            }
            break;
    } /* end of switch (type) */ 

    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0002]:[ICUMFW_UD_S4_D_HASH_0002]
* Function name: D_HASH_HashUpdate
* Description  : Perform HASH operation
* Arguments    : (IN)     const uint8_t* const p_msg,
*                         const uint32_t msg_len
*                (IN-OUT) st_r_hash_ctx_t* const p_ctx
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_TIMEOUTERR
*                HASH_RET_LENGTHERR
***************************************************************************************/
e_r_hash_ret_t D_HASH_HashUpdate(st_r_hash_ctx_t* const p_ctx, const uint8_t* const p_msg, const uint32_t msg_len)
{
    e_r_hash_ret_t ret;

    ret = HASH_RET_OK;

    if (NULL != p_ctx)
    {
        switch (p_ctx->type)
        {
            case HASH_TYPE_SHA3_256:
            case HASH_TYPE_SHA3_384:
            case HASH_TYPE_SHA3_512:

                /* check arguments */
                ret = d_hash_hash_sha3_chk_update_arg(p_ctx);

                if (HASH_RET_OK == ret)
                {
                    /* HashUpdate process */
                    ret = d_hash_hash_sha3_update(p_ctx, p_msg, msg_len);
                }
                else
                {
                    /* do nothing */
                }
                break;

            default:

                /* check arguments */
                ret = r_hash_hash_chk_update_arg(p_ctx, p_msg, msg_len);

                if (HASH_RET_OK == ret)
                {
                    /* HashUpdate process */
                    ret = r_hash_cmn_update(p_ctx, p_msg, msg_len);
                }
                else
                {
                    /* do nothing */
                }
                break;
        } /* end of switch (p_ctx->type) */

        if (HASH_RET_OK == ret)
        {
            /* do nothing */
        }
        else
        {
            /* Initialize DMA management table */
            d_hash_cmn_init_sts_dma(p_ctx);
        }
    } /* end of if (NULL != p_ctx) */ 
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0003]:[ICUMFW_UD_S4_D_HASH_0003]
* Function name: D_HASH_HashEnd
* Description  : Get the output result
* Arguments    : (IN-OUT) st_r_hash_ctx_t* const p_ctx
*                (OUT)    uint8_t* const p_digest
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_TIMEOUTERR
*                HASH_RET_LENGTHERR
*                HASH_RET_DMAERR
***************************************************************************************/
e_r_hash_ret_t D_HASH_HashEnd(st_r_hash_ctx_t* const p_ctx, uint8_t* const p_digest)
{
    e_r_hash_ret_t ret;

    ret = HASH_RET_OK;

    if (NULL != p_ctx)
    {
        switch (p_ctx->type)
        {
            case HASH_TYPE_SHA3_256:
            case HASH_TYPE_SHA3_384:
            case HASH_TYPE_SHA3_512:

                /* check arguments */
                ret = d_hash_hash_sha3_chk_end_arg(p_ctx, p_digest);

                if (HASH_RET_OK == ret)
                {
                    /* HashEnd process */
                    ret = d_hash_hash_sha3_end(p_ctx, p_digest);
                }
                else
                {
                    /* Initialize DMA management table */
                    d_hash_cmn_init_sts_dma(p_ctx);
                }
                break;

            default:

                /* check arguments */
                ret = r_hash_hash_chk_end_arg(p_ctx, p_digest);

                if (HASH_RET_OK == ret)
                {
                    /* HashEnd process */
                    ret = r_hash_hash_end(p_ctx, p_digest);
                }
                else
                {
                    /* Initialize DMA management table */
                    d_hash_cmn_init_sts_dma(p_ctx);
                }
                break;
        } /* end of switch (p_ctx->type) */
    } /* end of if (NULL != p_ctx) */ 
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return (ret);
}

/**********************************************************************************************************************
* Function Name : D_HASH_HMACBegin
**********************************************************************************************************************/
e_r_hash_ret_t D_HASH_HMACBegin(st_r_hmac_ctx_t* const p_ctx, const e_r_hash_type_t type,
                                const st_r_hmac_opt_t* const p_opt, const uint8_t* const p_key, const uint32_t key_len)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t ret;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* check arguments */
    ret = r_hash_hmac_chk_begin_arg(p_ctx, type, p_opt, p_key, key_len);

    if(ret == HASH_RET_OK)
    {
        /* HMACBegin process */
        ret = r_hash_hmac_begin(p_ctx, type, p_key, key_len);
    }
    else
    {
        /* do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function D_HASH_HMACBegin()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : D_HASH_HMACUpdate
**********************************************************************************************************************/
e_r_hash_ret_t D_HASH_HMACUpdate(st_r_hmac_ctx_t* const p_ctx, const uint8_t* const p_msg, const uint32_t msg_len)
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
        /* check arguments */
        ret = r_hash_hmac_chk_update_arg(p_ctx, p_msg, msg_len);

        if(ret == HASH_RET_OK)
        {
            /* HMACUpdate process */
            ret = r_hash_cmn_update(&p_ctx->hash_ctx, p_msg, msg_len);
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
* End of function D_HASH_HMACUpdate()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : D_HASH_HMACEnd
**********************************************************************************************************************/
e_r_hash_ret_t D_HASH_HMACEnd(st_r_hmac_ctx_t* const p_ctx, uint8_t* const p_tag)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t ret;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* check arguments */
    ret = r_hash_hmac_chk_end_arg(p_ctx, p_tag);

    if(ret == HASH_RET_OK)
    {
        /* HMACEnd process */
        ret = r_hash_hmac_end(p_ctx, p_tag);
    }
    else
    {
        /* do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function D_HASH_HMACEnd()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0068]:[ICUMFW_UD_S4_D_HASH_0068]
* Function Name: D_HASH_ChkDMABusy
* Description  : Setting the size of all message data
* Arguments    : (IN)  type : HASH type
* Return Value : HASH_RET_OK
*                HASH_RET_DMA_RUNNING
*                HASH_RET_PARAMERR
**********************************************************************************************************************/
e_r_hash_ret_t D_HASH_ChkDMABusy(const e_r_hash_type_t type)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t  ret;
    uint32_t        dma_status;

    ret = HASH_RET_PARAMERR;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* Check the execution status of Hash module */
    dma_status = d_hash_cmn_get_dma_status(type);

    switch (dma_status)
    {
        case DMA_STATUS_NON_EXEC:
            ret = HASH_RET_OK;
            break;

        case DMA_STATUS_RUNNING:
        case DMA_STATUS_NORMAL_END:
        case DMA_STATUS_ADDR_ERR:
        case DMA_STATUS_TIMEOUT_ERR:
            ret = HASH_RET_DMA_RUNNING;
            break;

        default:
            ret = HASH_RET_PARAMERR;
            break;
    }

    return (ret);
}
/**********************************************************************************************************************
* End of function D_HASH_ChkDMABusy()
**********************************************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0064]:[ICUMFW_UD_S4_D_HASH_0064]
* Function name: D_HASH_SetOpePrm
* Description  : Setting the size of all message data
* Arguments    : (IN-OUT) st_r_hash_ctx_t* const p_ctx
*                (IN)     st_r_ope_prm_t* const p_prm
* Return Value : HASH_RET_OK
***************************************************************************************/
e_r_hash_ret_t D_HASH_SetOpePrm(st_r_hash_ctx_t* const p_ctx, st_r_ope_prm_t* const p_prm)
{
    e_r_hash_ret_t ret;

    ret = HASH_RET_OK;

    p_ctx->msg_total_len = p_prm->total_msg_size_in_bytes;

    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0067]:[ICUMFW_UD_S4_D_HASH_0067]
* Function name: D_HASH_ChkDMAApplicable
* Description  : Check if DMA transfer is applicable
* Arguments    : (IN)  p_input_area         : The address of the input area
*                (IN)  input_size_in_bytes  : The size of the input area
*                (IN)  p_output_area        : The address of the output area
*                (IN)  output_size_in_bytes : The size of the output area
* Return Value : HASH_RET_OK
*                HASH_RET_DMA_NOTAPPLICABLE
***************************************************************************************/
e_r_hash_ret_t D_HASH_ChkDMAApplicable(const uint8_t* p_input_area,  uint32_t input_size_in_bytes, 
                                             uint8_t* p_output_area, uint32_t output_size_in_bytes)
{
    e_r_hash_ret_t ret;

    ret = HASH_RET_OK;

    ret = d_hash_cmn_chk_alignment(ALIGNMENT_64_BYTE, p_input_area);

    if (HASH_RET_OK == ret)
    {
        ret = d_hash_cmn_chk_size(MULTIPLE_OF_64, input_size_in_bytes);
    }
    else
    {
        /* do nothing */
    }

    if (HASH_RET_OK == ret)
    {
        ret = d_hash_cmn_chk_alignment(ALIGNMENT_64_BYTE, p_output_area);
    }
    else
    {
        /* do nothing */
    }

    if (HASH_RET_OK == ret)
    {
        ret = d_hash_cmn_chk_size(MULTIPLE_OF_64, output_size_in_bytes);
    }
    else
    {
        /* do nothing */
    }

    if (HASH_RET_OK != ret)
    {
        ret = HASH_RET_DMA_NOTAPPLICABLE;
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

