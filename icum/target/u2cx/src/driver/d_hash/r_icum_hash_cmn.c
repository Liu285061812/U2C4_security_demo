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
/* PRQA S 0310, 3305 ++ # D002 Cast for input and output to 32-bit registers */
/* PRQA S 1532 ++ # D007 Since files are grouped by function, they are called from different files. */
/* PRQA S 2912, 3383, 3384 ++ # D008 Checking for each operation affects the processing speed */
/* PRQA S 3120 ++ # D009 It is easy to understand if you do not use a constant */
/* PRQA S 5002 ++ # D013 Exception according to file nomenclature */

/*=====================================================================================================================
 Includes <System Includes> , "Project Includes"
=====================================================================================================================*/
#include "r_icum_hash_api.h"
#include "r_icum_hash_cmn.h"
#include "icum_d_wdt_pub.h"
#include "icum_d_remap_pub.h"
#include "icum_common_pub.h"

/*!***********************************************************
 * \addtogroup  HASHCommon
 * \{
 * \addtogroup  HASHCommonDefines         Common Definitions
 * \addtogroup  HASHCommonTypes           Common Types
 * \addtogroup  HASHCommonAPIs            Common APIs
 * \addtogroup  HASHCommonDefinesInternal Common Internal Definitions
 * \addtogroup  HASHCommonAPIsInternal    Common Internal APIs
 * \}*********************************************************/

/*=====================================================================================================================
 Private macro definitions
=====================================================================================================================*/
/*!****************************************************
 * \addtogroup HASHCommonDefinesInternal
 * \{**************************************************/
#define TIMEOUT_INIT             (0x0000019BU)  /**< Timeout count of hash operation */

/* DMA operation timeout */
#define DMA_OPE_TIMEOUT          (0x0000CA7FU)

/* The number of elements in the management information table for DMA transfers */
#define MNG_ELEMENT_NUM          (2U)
/*\}*/

/*=====================================================================================================================
 Private global variables
=====================================================================================================================*/
/* Management information table for DMA transfers */
typedef struct st_d_hash_dma_mng
{
    uint32_t         dma_transfer_status;
    uint32_t         dma_src_channel;
    uint32_t         dma_dst_channel;
} st_d_hash_dma_mng_t;

/* [0]: For SHA2  [1]: For SHA3 */
static st_d_hash_dma_mng_t dma_mng[MNG_ELEMENT_NUM] = {
    { DMA_STATUS_NON_EXEC, HASH_DMA_CH_UNUSED, HASH_DMA_CH_UNUSED },
    { DMA_STATUS_NON_EXEC, HASH_DMA_CH_UNUSED, HASH_DMA_CH_UNUSED },
};

/*=====================================================================================================================
 Private function prototypes
=====================================================================================================================*/
static e_r_hash_ret_t r_hash_cmn_exec_block(st_r_hash_ctx_t* const p_ctx, const uint8_t* const p_msg,
                                            const uint32_t msg_len, const uint32_t hash_block_size);
static void           r_hash_cmn_set_intermediate(const uint32_t* const p_im_data, const uint32_t* const p_im_len);
static e_r_hash_ret_t r_hash_cmn_wait_sts(volatile const uint32_t* const p_reg,
                                            const uint32_t pos, const uint32_t break_sts);
static uint32_t       d_hash_cmn_get_dma_mng_index(const e_r_hash_type_t type);
static void           d_hash_cmn_alloc_dma_channel(const uint32_t index);
static void           d_hash_cmn_start_wdt_if_need(void);

/*=====================================================================================================================
 Public function definitions
=====================================================================================================================*/

/**********************************************************************************************************************
* Function Name : r_hash_cmn_set_byte
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHCommonAPIsInternal
 * \brief       Simple byte-wise value set function.
 * \param       [out] p_dest       Destination address
 * \param       [in]  val          value
 * \param       [in]  len          set length in byte
 * \return
 *  - None
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Simple byte-wise value set function.
 *
 * \callgraph
 *********************************************************************************************************************/
void r_hash_cmn_set_byte(uint8_t* const p_dest, const uint8_t val, const uint32_t len)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    uint32_t i;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* NULL check */
    if(p_dest != NULL)
    {
        for(i = 0U; i < len; i++)
        {
            p_dest[i] = val;
        }
    }
    else
    {
        /* It has been confirmed that there is no problem */
        /* do nothing */
    }
}
/**********************************************************************************************************************
* End of function r_hash_cmn_set_byte()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_hash_cmn_cpy_byte
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHCommonAPIsInternal
 * \brief       Simple byte-wise copy function.
 * \param       [out] p_dest       Destination address
 * \param       [in]  p_src        Source address
 * \param       [in]  len          Copying length in byte
 * \return
 *  - None
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Simple byte-wise copy function.
 *
 * \callgraph
 *********************************************************************************************************************/
void r_hash_cmn_cpy_byte(uint8_t* const p_dest, const uint8_t* const p_src, const uint32_t len)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    uint32_t i;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* NULL check */
    if(p_dest != NULL)
    {
        for(i = 0U; i < len; i++)
        {
            p_dest[i] = p_src[i];
        }
    }
    else
    {
        /* It has been confirmed that there is no problem */
        /* do nothing */
    }
}
/**********************************************************************************************************************
* End of function r_hash_cmn_cpy_byte()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_hash_cmn_roundup_4
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHCommonAPIsInternal
 * \brief       Round up the value to a multiple of 4.
 * \param       [in] val     target value
 * \return
 *  - Round up value
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Round up the value to a multiple of 4.
 *
 * \callgraph
 *********************************************************************************************************************/
uint32_t r_hash_cmn_roundup_4(const uint32_t val)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    const uint32_t ret_val = (((val + 3U) >> 2U) << 2U);

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    return ret_val;
}
/**********************************************************************************************************************
* End of function r_hash_cmn_roundup_4()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_hash_cmn_init_register
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHCommonAPIsInternal
 * \brief       SHAA software initialize
 * \param       [in] mode      operating mode
 * \return
 *  - #HASH_RET_OK:                  Successfully done.
 *  - #HASH_RET_TIMEOUTERR:          Operation timeouted.
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #ICUM_SHACNT, #ICUM_SHASTS
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - SHA software, Operating mode, and Byte order conversion resetting 
 *
 * Pseudo-code for the function can be found below. 
 * -# 1 is set to the INIT bit of the register #ICUM_SHACNT
 * -# mode is set to the MODE bit of the register #ICUM_SHACNT
 * -# Wait IDATA_READY is 1 with #r_hash_cmn_wait_sts
 * -# If ret != HASH_RET_OK
 *   -# 1 is set to the ENDIAN bit of the register #ICUM_SHACNT
 * -# Else
 *   -# Do nothing
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
e_r_hash_ret_t r_hash_cmn_init_register(const uint32_t mode)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t ret;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* ICUMX_SHAA initialize and mode set and endian set */
    ICUM_SHACNT =   (0x1U << CNT_INIT_POS) |
                    (mode << CNT_MODE_POS) |
                    (0x1U << CNT_ENDIAN_POS); // set endian to 0 from 1 zhxk

    /* wait for the state to change */
    ret = r_hash_cmn_wait_sts(&ICUM_SHASTS, STS_IDATA_READY_POS, 0x1U);

    return ret;
}
/**********************************************************************************************************************
* End of function r_hash_cmn_init_register()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_hash_cmn_preprocess_block
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHCommonAPIsInternal
 * \brief       Intermediate data setting confirmation
 * \param       [in] p_ctx      An address of hash context
 * \return
 *  - #HASH_RET_OK:                  Successfully done.
 *  - #HASH_RET_PARAMERR:            Non-compatible HASH type.
 *  - #HASH_RET_TIMEOUTERR:          Operation timeouted.
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #ICUM_SHADID0, #ICUM_SHADID1, #ICUM_SHADID2, #ICUM_SHADID3, <br>
 *    #ICUM_SHADID4, #ICUM_SHADID5, #ICUM_SHADID6, #ICUM_SHADID7, <br>
 *    #ICUM_SHADIL0, #ICUM_SHADIL1, #ICUM_SHADDCNT (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Check if it is necessary to set intermediate data in the register
 *
 * Pseudo-code for the function can be found below. 
 * -# If p_ctx != NULL
 *   -# Get mode for control register with #r_hash_cmn_get_mode and set it to cnt_mode
 *   -# If cnt_mode == UNSUPPORTED_TYPE
 *     -# Set #HASH_RET_PARAMERR to ret
 *   -# Else
 *     -# Software initialize with #r_hash_cmn_init_register
 *     -# Set function result to ret 
 *   -# If ret == HASH_RET_OK
 *     -# If (p_ctx->intermediate_len[0] != 0U) || (p_ctx->intermediate_len[1] != 0U) || <br>
 *           (p_ctx->intermediate_len[2] != 0U) || (p_ctx->intermediate_len[3] != 0U)
 *       -# Set intermediate data with #r_hash_cmn_set_intermediate
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
e_r_hash_ret_t r_hash_cmn_preprocess_block(const st_r_hash_ctx_t* const p_ctx)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t ret;
    uint32_t       cnt_mode;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* NULL check */
    if(p_ctx != NULL)
    {
        /* get mode for Control Register */
        cnt_mode = r_hash_cmn_get_mode(p_ctx->type);

        if(cnt_mode == UNSUPPORTED_TYPE)
        {
            ret = HASH_RET_PARAMERR;
        }
        else
        {
            /* software initialize */
            ret = r_hash_cmn_init_register(cnt_mode);
        }

        if(ret == HASH_RET_OK)
        {
            /* set intermediate data in register */
            if( (p_ctx->intermediate_len[0] != 0U) || (p_ctx->intermediate_len[1] != 0U) ||
                (p_ctx->intermediate_len[2] != 0U) || (p_ctx->intermediate_len[3] != 0U))
            {
                r_hash_cmn_set_intermediate(p_ctx->intermediate_data, p_ctx->intermediate_len);
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
* End of function r_hash_cmn_preprocess_block()
**********************************************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0024]:[ICUMFW_UD_S4_D_HASH_0024]
* Function name: r_hash_cmn_chk_type
* Description  : HASH type check
* Arguments    : (IN) const e_r_hash_type_t type
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
***************************************************************************************/
e_r_hash_ret_t r_hash_cmn_chk_type(const e_r_hash_type_t type)
{
    e_r_hash_ret_t ret;

    ret = HASH_RET_PARAMERR;

    if (HASH_TYPE_SHA512_256 >= type)
    {
        ret = HASH_RET_OK;
    }
    else
    {
        /* do nothing */
    }

    return ret;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0025]:[ICUMFW_UD_S4_D_HASH_0025]
* Function name: r_hash_cmn_update
* Description  : HASH UPDATE processing
* Arguments    : (IN)     const uint8_t* const p_msg,
*                         const uint32_t msg_len
*                (IN-OUT) st_r_hash_ctx_t* const p_ctx
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_LENGTHERR
***************************************************************************************/
e_r_hash_ret_t r_hash_cmn_update(st_r_hash_ctx_t* const p_ctx, const uint8_t* const p_msg, const uint32_t msg_len)
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
            ret = d_hash_cmn_prepare_dma(p_msg, msg_len, p_ctx->type, DMA_INPUT_CHANNEL);
            p_ctx->msg_total_len = msg_len;
        }
        else
        {
            /* Buffer overflow prevention range check */
            if (p_ctx->transfer_prm.icup.leftover_msg_len < HASH_LEFTOVER_MAX)
            {
                /* get block size */
                hash_block_size = r_hash_cmn_get_block_size(p_ctx->type);

                if (BLOCK_SIZE_ERR == hash_block_size)
                {
                    ret = HASH_RET_LENGTHERR;
                }
                /* when the processing data size is smaller than the block size */
                else if (((msg_len < hash_block_size) &&
                        ((p_ctx->transfer_prm.icup.leftover_msg_len + msg_len) < hash_block_size)))
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
                    ret = r_hash_cmn_preprocess_block(p_ctx);

                    if (HASH_RET_OK == ret)
                    {
                        ret = r_hash_cmn_exec_block(p_ctx, p_msg, msg_len, hash_block_size);
                    }
                    else
                    {
                        /* do nothing */
                    }
                } /* end of else */
            } /* end of if(p_ctx->transfer_prm.icup.leftover_msg_len < HASH_LEFTOVER_MAX) */
            else
            {
                ret = HASH_RET_LENGTHERR;
            }
        } /* end of else */
    } /* end of if(NULL != p_ctx) */
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0026]:[ICUMFW_UD_S4_D_HASH_0026]
* Function name: r_hash_cmn_get_mode
* Description  : Get operation mode
* Arguments    : (IN) const e_r_hash_type_t type
* Return Value : none
***************************************************************************************/
uint32_t r_hash_cmn_get_mode(const e_r_hash_type_t type)
{
    uint32_t mode;

    mode = VARIABLE_INIT;

    switch(type)
    {
        case HASH_TYPE_SHA1:
            mode = CNT_MODE_VAL_SHA1;
            break;

        case HASH_TYPE_SHA224:
            mode = CNT_MODE_VAL_SHA224;
            break;

        case HASH_TYPE_SHA256:
            mode = CNT_MODE_VAL_SHA256;
            break;

        case HASH_TYPE_SHA512_224:
            mode = CNT_MODE_VAL_SHA512_224;
            break;

        case HASH_TYPE_SHA512_256:
            mode = CNT_MODE_VAL_SHA512_256;
            break;

        case HASH_TYPE_SHA384:
            mode = CNT_MODE_VAL_SHA384;
            break;

        case HASH_TYPE_SHA512:
            mode = CNT_MODE_VAL_SHA512;
            break;

        default:
            mode = UNSUPPORTED_TYPE;
            break;
    } /* end of switch (type) */

    return mode;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0027]:[ICUMFW_UD_S4_D_HASH_0027]
* Function name: r_hash_cmn_get_length
* Description  : Get hash length
* Arguments    : (IN) const e_r_hash_type_t type
* Return Value : none
***************************************************************************************/
uint32_t r_hash_cmn_get_length(const e_r_hash_type_t type)
{
    uint32_t hash_len;

    hash_len = LENGTH_ERR;

    /* set HASH length */
    switch(type)
    {
        case HASH_TYPE_SHA1:
            hash_len = SHA1_LEN;
            break;

        case HASH_TYPE_SHA224:
        case HASH_TYPE_SHA512_224:
            hash_len = SHA224_LEN;
            break;

        case HASH_TYPE_SHA256:
        case HASH_TYPE_SHA512_256:
            hash_len = SHA256_LEN;
            break;

        case HASH_TYPE_SHA384:
            hash_len = SHA384_LEN;
            break;

        case HASH_TYPE_SHA512:
            hash_len = SHA512_LEN;
            break;

        default:
            break;
    } /* end of switch (type) */

    return hash_len;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0028]:[ICUMFW_UD_S4_D_HASH_0028]
* Function name: r_hash_cmn_get_block_size
* Description  : Get hash block size
* Arguments    : (IN) const e_r_hash_type_t type
* Return Value : none
***************************************************************************************/
uint32_t r_hash_cmn_get_block_size(const e_r_hash_type_t type)
{
    uint32_t block_size;

    block_size = BLOCK_SIZE_ERR;

    switch(type)
    {
        /* These 3 cases are intentionally combined. */
        case HASH_TYPE_SHA1:
        case HASH_TYPE_SHA224:
        case HASH_TYPE_SHA256:
            block_size = BLOCK_SIZE_64_BYTE;
            break;

        case HASH_TYPE_SHA512_224:
        case HASH_TYPE_SHA512_256:
        case HASH_TYPE_SHA384:
        case HASH_TYPE_SHA512:
            block_size = BLOCK_SIZE_128_BYTE;
            break;

        default:
            break;
    } /* end of switch (type) */

    return block_size;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0029]:[ICUMFW_UD_S4_D_HASH_0029]
* Function name: r_hash_cmn_get_intermediate
* Description  : Get intermediate data
* Arguments    : (IN) uint32_t* const p_im_data
*                     uint32_t* const p_im_len
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_TIMEOUTERR
***************************************************************************************/
e_r_hash_ret_t r_hash_cmn_get_intermediate(uint32_t* const p_im_data, uint32_t* const p_im_len)
{
    e_r_hash_ret_t ret;
    uint32_t       read_register;

    ret = HASH_RET_OK;
    read_register = VARIABLE_INIT;

    /* NULL check */
    if ((NULL !=p_im_data) && (NULL != p_im_len))
    {
        /* Define macro ICUM_SHAFIFOCNT is cast to access a register */
        ret = r_hash_cmn_wait_sts(&ICUM_SHAFIFOCNT, FIFOCNT_INOTEMP_POS, REGISTER_SET_0);

        if (HASH_RET_OK == ret)
        {
            /* Define macro ICUM_SHADDCNT is cast to access a register */
            ICUM_SHADDCNT = (REGISTER_SET_1 << DDCNT_DIRECTOUT_REQ_POS);

            /* Define macro ICUM_SHASTS is cast to access a register */
            ret = r_hash_cmn_wait_sts(&ICUM_SHASTS, STS_DIRECTOUT_VALID_POS, REGISTER_SET_1);

            if (HASH_RET_OK == ret)
            {
                /* intermediate data output */
                /* Define macro ICUM_SHADOD0 is cast to access a register */
                p_im_data[0] = ICUM_SHADOD0;

                /* Define macro ICUM_SHADOD1 is cast to access a register */
                p_im_data[1] = ICUM_SHADOD1;

                /* Define macro ICUM_SHADOD2 is cast to access a register */
                p_im_data[2] = ICUM_SHADOD2;

                /* Define macro ICUM_SHADOD3 is cast to access a register */
                p_im_data[3] = ICUM_SHADOD3;

                /* Define macro ICUM_SHADOD4 is cast to access a register */
                p_im_data[4] = ICUM_SHADOD4;

                /* Define macro ICUM_SHADOD5 is cast to access a register */
                p_im_data[5] = ICUM_SHADOD5;

                /* Define macro ICUM_SHADOD6 is cast to access a register */
                p_im_data[6] = ICUM_SHADOD6;

                /* Define macro ICUM_SHADOD7 is cast to access a register */
                p_im_data[7] = ICUM_SHADOD7;

                /* intermediate data length output */
                /* Define macro ICUM_SHADOL0 is cast to access a register */
                p_im_len[0] = ICUM_SHADOL0;

                /* Define macro ICUM_SHADOL1 is cast to access a register */
                p_im_len[1] = ICUM_SHADOL1;

                read_register = ICUM_SHACNT;

                /* Get MODE information and set to lower 4 bits */
                read_register = (read_register & ICUM_SHACNT_MODE_MASK) >> CNT_MODE_POS;

                /* When hash type is SHA512_224, SHA512_256, SHA384, SHA512 */
                if ((CNT_MODE_VAL_SHA512_224 <= read_register) && (CNT_MODE_VAL_SHA512 >= read_register))
                {
                    ICUM_SHADDCNT = (REGISTER_SET_1 << DDCNT_EX_DOD_POS) |
                                    (REGISTER_SET_1 << DDCNT_EX_DOL_POS);

                    /* intermediate data output (expansion area) */
                    p_im_data[8]  = ICUM_SHADOD0;
                    p_im_data[9]  = ICUM_SHADOD1;
                    p_im_data[10] = ICUM_SHADOD2;
                    p_im_data[11] = ICUM_SHADOD3;
                    p_im_data[12] = ICUM_SHADOD4;
                    p_im_data[13] = ICUM_SHADOD5;
                    p_im_data[14] = ICUM_SHADOD6;
                    p_im_data[15] = ICUM_SHADOD7;

                    /* intermediate data length output (expansion area) */
                    p_im_len[2] = ICUM_SHADOL0;
                    p_im_len[3] = ICUM_SHADOL1;

                } /* end of if ((CNT_MODE_VAL_SHA512_224 <= read_register) && */
                  /*            (CNT_MODE_VAL_SHA512 >= read_register))       */
                else
                {
                    /* do nothing */
                }

                /* clear odata valid */
                /* Define macro ICUM_SHASTSC is cast to access a register */
                ICUM_SHASTSC = (REGISTER_SET_1 << STSC_DIROUT_VALCLR_POS);
            } /* end of if(HASH_RET_OK == ret) */
            else
            {
                /* do nothing */
            }
        } /* end of if (HASH_RET_OK == ret) */
        else
        {
            /* do nothing */
        }
    } /* end of if ((NULL !=p_im_data) && (NULL != p_im_len)) */
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}

/**********************************************************************************************************************
* Function Name : r_hash_cmn_set_idat
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHCommonAPIsInternal
 * \brief       Set data in register
 * \param       [in]  p_data       An inputting data.
 * \param       [in]  len          The length of the data in bytes.
 * \return
 *  - None
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #ICUM_SHAIDAT
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Set data in the #ICUM_SHAIDAT register.
 * 
 * Pseudo-code for the function can be found below. 
 * -# Set the argument p_data to p_temp
 * -# Set calculate of (r_hash_cmn_roundup_4(len) / HASH_WORD_SIZE) value to loop_max
 * -# If p_data != NULL
 *   -# Loop while i is loop_max or less<br>
 *      Increment i for each loop
 *     -# Set p_temp[i] to #ICUM_SHAIDAT
 * -# Else
 *   -# Do nothing
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
void r_hash_cmn_set_idat(const uint8_t* const p_data, const uint32_t len)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    const uint32_t* const p_temp = (const uint32_t*)p_data;
    const uint32_t        loop_max = (r_hash_cmn_roundup_4(len) / HASH_WORD_SIZE);
    uint32_t              i;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* set the data to IDAT register */
    for(i = 0U; i < loop_max; i++)
    {
        /* Define macro ICUM_SHAIDAT is cast to access a register */
        ICUM_SHAIDAT = p_temp[i];
    }
}
/**********************************************************************************************************************
* End of function r_hash_cmn_set_idat()
**********************************************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0031]:[ICUMFW_UD_S4_D_HASH_0031]
* Function name: r_hash_cmn_context_chk_arg
* Description  : Context member check
* Arguments    : (IN) const st_r_hash_ctx_t* const p_ctx
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
*                HASH_RET_TIMEOUTERR
***************************************************************************************/
e_r_hash_ret_t r_hash_cmn_context_chk_arg(const st_r_hash_ctx_t* const p_ctx)
{
    e_r_hash_ret_t ret;
    uint32_t       hash_block_size;

    ret = HASH_RET_OK;
    hash_block_size = VARIABLE_INIT;

    /* NULL check */
    if(p_ctx != NULL)
    {
        /* check HASH type */
        switch (p_ctx->type)
        {
            case HASH_TYPE_SHA1:
                if (SHA1_LEN != p_ctx->hash_len)
                {
                    ret = HASH_RET_PARAMERR;
                }
                else
                {
                    /* do nothing */
                }
                break;

            case HASH_TYPE_SHA224:
            case HASH_TYPE_SHA512_224:
                if (SHA224_LEN != p_ctx->hash_len)
                {
                    ret = HASH_RET_PARAMERR;
                }
                else
                {
                    /* do nothing */
                }
                break;

            case HASH_TYPE_SHA256:
            case HASH_TYPE_SHA512_256:
                if (SHA256_LEN != p_ctx->hash_len)
                {
                    ret = HASH_RET_PARAMERR;
                }
                else
                {
                    /* do nothing */
                }
                break;

            case HASH_TYPE_SHA384:
                if (SHA384_LEN != p_ctx->hash_len)
                {
                    ret = HASH_RET_PARAMERR;
                }
                else
                {
                    /* do nothing */
                }
                break;

            case HASH_TYPE_SHA512:
                if (SHA512_LEN != p_ctx->hash_len)
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
        } /* end of switch (p_ctx->type) */

        if (HASH_RET_OK == ret)
        {
            /* get block size */
            hash_block_size = r_hash_cmn_get_block_size(p_ctx->type);

            if ((BLOCK_SIZE_ERR == hash_block_size) ||
                (p_ctx->transfer_prm.icup.leftover_msg_len >= hash_block_size))
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
    } /* end of if (NULL != p_ctx)*/
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}

/**********************************************************************************************************************
* Function Name : r_hash_cmn_wait_for_idata_ready
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHCommonAPIsInternal
 * \brief       Wait for IDATA_READY
 * \return
 *  - #HASH_RET_OK:                  Successfully done.
 *  - #HASH_RET_TIMEOUTERR:          Operation timeouted.
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #ICUM_SHAFIFOCNT, #ICUM_SHASTS
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - The "wait for IDATA_READY" process is called from the hash operation or HMAC operation.
 *
 * Pseudo-code for the function can be found below. 
 * -# Wait INotEmp is 0 with #r_hash_cmn_wait_sts
 * -# If ret == HASH_RET_OK
 *   -# Wait IDATA_READY is 1 with #r_hash_cmn_wait_sts
 * -# Else
 *   -# Do nothing
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
e_r_hash_ret_t r_hash_cmn_wait_for_idata_ready(void)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t ret;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* wait for the state to changed the state to IFIFO_EMPTY */
    /* Define macro ICUM_SHAFIFOCNT is cast to access a register */
    ret = r_hash_cmn_wait_sts(&ICUM_SHAFIFOCNT, FIFOCNT_INOTEMP_POS, 0x0U);

    if(ret == HASH_RET_OK)
    {
        /* wait for the state to changed the state to IDATA_READY with timeout */
        /* Define macro ICUM_SHASTS is cast to access a register */
        ret = r_hash_cmn_wait_sts(&ICUM_SHASTS, STS_IDATA_READY_POS, 0x1U);
    }
    else
    {
        /* do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_hash_cmn_wait_for_idata_ready()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_hash_cmn_get_output_data
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHCommonAPIsInternal
 * \brief       Get output data.
 * \param       [in] hash_len      HASH length
 * \param       [out] p_out        Acquisition data storage destination address
 * \return
 *  - #HASH_RET_OK:                  Successfully done.
 *  - #HASH_RET_PARAMERR:            Unexpected data in context structure member.
 *  - #HASH_RET_TIMEOUTERR:          process timeouted.
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #ICUM_SHASTS, #ICUM_SHAFIFOCNT, #ICUM_SHAODAT, #ICUM_SHASTSC
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Acquires the data that has been subjected to HASH processing from the register. <br>
 *        This process is called from a hash operation or an HMAC operation.
 * 
 * Pseudo-code for the function can be found below. 
 * -# Set the argument p_out to p_out_buf
 * -# If p_out != NULL
 *   -# Wait ODATA_VALID is 1 with #r_hash_cmn_wait_sts
 *   -# If ret == HASH_RET_OK
 *     -# Wait ONOTEMP is 1 with #r_hash_cmn_wait_sts
 *     -# If ret == HASH_RET_OK
 *       -# Loop while i is (hash_len/HASH_WORD_SIZE) or less<br>
 *        Increment i for each loop
 *         -# Set p_out_buf[i] to #ICUM_SHAODAT
 *       -# 1 is set to the ODATA_VALCLR bit of the register #ICUM_SHASTSC
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
e_r_hash_ret_t r_hash_cmn_get_output_data(const uint32_t hash_len, uint8_t* const p_out)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t  ret;
    uint32_t        i;

    /* This variable is cast to receive a value from a register */
    uint32_t* const p_out_buf = (uint32_t*)p_out;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* NULL check */
    if(p_out != NULL)
    {
        /* wait for the state to changed the state to ODATA_VALID */
        /* Define macro ICUM_SHASTS is cast to access a register */
        ret = r_hash_cmn_wait_sts(&ICUM_SHASTS, STS_ODATA_VALID_POS, 0x1U);

        if(ret == HASH_RET_OK)
        {
            /* wait for the state to changed the state to OFIFO_EMPTY */
            /* Define macro ICUM_SHAFIFOCNT is cast to access a register */
            ret = r_hash_cmn_wait_sts(&ICUM_SHAFIFOCNT, FIFOCNT_ONOTEMP_POS, 0x1U);

            if(ret == HASH_RET_OK)
            {
                /* get output data */
                for(i=0U; i<(hash_len/HASH_WORD_SIZE); i++)
                {
                    /* Define macro ICUM_SHAODAT is cast to access a register */
                    p_out_buf[i] = ICUM_SHAODAT;
                }

                /* clear odata valid */
                /* Define macro ICUM_SHASTSC is cast to access a register */
                ICUM_SHASTSC = (0x1U << STSC_ODATA_VALCLR_POS);
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
* End of function r_hash_cmn_get_output_data()
**********************************************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0069]:[ICUMFW_UD_S4_D_HASH_0069]
* Function name: d_hash_cmn_chk_alignment
* Description  : Check if the alignment is as specified
* Arguments    : (IN)  alignment : The number of bytes for alignment
*                (IN)  p_area    : The address to be checked
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
***************************************************************************************/
e_r_hash_ret_t d_hash_cmn_chk_alignment(const uint32_t alignment, const uint8_t* p_area)
{
    e_r_hash_ret_t      ret;
    ptr_manip_hash_t    ptr_addr;

    ret                   = HASH_RET_PARAMERR;
    ptr_addr.p_cnst_uint8 = p_area;

    if (NUM0 != alignment)
    {
        if (NUM0 == (ptr_addr.val % alignment))
        {
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
* Function ID  : [ICUMFW_CD_S4_D_HASH_0070]:[ICUMFW_UD_S4_D_HASH_0070]
* Function name: d_hash_cmn_chk_size
* Description  : Check if the size is as specified
* Arguments    : (IN)  refer_size : Reference value for size check
*                (IN)  taget_size : The size to be checked
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
***************************************************************************************/
e_r_hash_ret_t d_hash_cmn_chk_size(const uint32_t refer_size, const uint32_t taget_size)
{
    e_r_hash_ret_t  ret;

    ret = HASH_RET_PARAMERR;

    if ((NUM0 != taget_size) && (NUM0 != refer_size))
    {
        if (NUM0 == (taget_size % refer_size))
        {
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
* Function ID  : [ICUMFW_CD_S4_D_HASH_0071]:[ICUMFW_UD_S4_D_HASH_0071]
* Function name: d_hash_cmn_get_dma_status
* Description  : Get DMA transfer status
* Arguments    : (IN)  type : HASH type
* Return Value : DMA_STATUS_NON_EXEC
*                DMA_STATUS_RUNNING
*                DMA_STATUS_ADDR_ERR
*                NUM_ERR_U32
***************************************************************************************/
uint32_t d_hash_cmn_get_dma_status(const e_r_hash_type_t type)
{
    uint32_t    dma_status;
    uint32_t    index;

    dma_status = NUM_ERR_U32;

    index = d_hash_cmn_get_dma_mng_index(type);
    if (NUM_ERR_U32 != index)
    {
        dma_status = dma_mng[index].dma_transfer_status;
    }
    else
    {
        /* do nothing */
    }

    return (dma_status);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0072]:[ICUMFW_UD_S4_D_HASH_0072]
* Function name: d_hash_cmn_set_dma_status
* Description  : Set DMA transfer status
* Arguments    : (IN)  type   : HASH type
*                (IN)  status : DMA transfer status
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
***************************************************************************************/
e_r_hash_ret_t d_hash_cmn_set_dma_status(const e_r_hash_type_t type, const uint32_t status)
{
    e_r_hash_ret_t  ret;
    uint32_t        index;

    ret = HASH_RET_PARAMERR;

    index = d_hash_cmn_get_dma_mng_index(type);
    if (NUM_ERR_U32 != index)
    {
        switch (status)
        {
            case DMA_STATUS_RUNNING:
                if (DMA_STATUS_NON_EXEC == dma_mng[index].dma_transfer_status)
                {
                    dma_mng[index].dma_transfer_status = status;
                    d_hash_cmn_alloc_dma_channel(index);
                    ret = HASH_RET_OK;
                }
                else
                {
                    /* do nothing */
                }
                break;

            case DMA_STATUS_NORMAL_END:
            case DMA_STATUS_ADDR_ERR:
            case DMA_STATUS_TIMEOUT_ERR:
                if (DMA_STATUS_RUNNING == dma_mng[index].dma_transfer_status)
                {
                    dma_mng[index].dma_transfer_status = status;
                    ret = HASH_RET_OK;
                }
                else
                {
                    /* do nothing */
                }
                break;

            case DMA_STATUS_NON_EXEC:
                dma_mng[index].dma_transfer_status = status;
                dma_mng[index].dma_src_channel = HASH_DMA_CH_UNUSED;
                dma_mng[index].dma_dst_channel = HASH_DMA_CH_UNUSED;
                ret = HASH_RET_OK;
                break;

            default:
                break;
        }
    }
    else
    {
        /* do nothing */
    }

    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0073]:[ICUMFW_UD_S4_D_HASH_0073]
* Function name: d_hash_cmn_prepare_dma
* Description  : DMA preparation
* Arguments    : (IN) const uint8_t* const p_addr,
*                     const uint32_t transfer_byte_len,
*                     const e_r_hash_type_t type,
*                     const uint32_t setting_channel
* Return Value : HASH_RET_OK
*                HASH_RET_PARAMERR
***************************************************************************************/
e_r_hash_ret_t d_hash_cmn_prepare_dma(const uint8_t* const p_addr, 
                                      const uint32_t transfer_byte_len,
                                      const e_r_hash_type_t type,
                                      const uint32_t setting_channel)
{
    e_r_hash_ret_t   ret;
    uint32_t         uiRet;
    uint32_t         index;
    uint32_t         channel;
    uint32_t         sha_hw_addr;
    uint32_t         req_source;
    uint32_t         setting_addr;
    ptr_manip_hash_t ptr_addr;

    ret                   = HASH_RET_OK;
    uiRet                 = ER_OK;
    index                 = VARIABLE_INIT;
    channel               = VARIABLE_INIT;
    sha_hw_addr           = VARIABLE_INIT;
    req_source            = VARIABLE_INIT;
    setting_addr          = VARIABLE_INIT;
    ptr_addr.p_cnst_uint8 = p_addr;

    if (NULL != p_addr)
    {
        /* Address alignment check */
        ret = d_hash_cmn_chk_alignment(ALIGNMENT_64_BYTE, p_addr);
    }
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    if (HASH_RET_OK == ret)
    {
        /* Data length size check */
        ret = d_hash_cmn_chk_size(MULTIPLE_OF_64, transfer_byte_len);
    }
    else
    {
        /* do nothing */
    }

    if (HASH_RET_OK == ret)
    {
        /* Get Physical Address */
        uiRet = D_REMAP_GetPhysAddrs(ptr_addr.val, &setting_addr);

        if (ER_OK == uiRet)
        {
            /* do nothing */
        }
        else
        {
            ret = HASH_RET_PARAMERR;
        }
    } /* end of if (HASH_RET_OK == ret) */
    else
    {
        /* do nothing */
    }

    if (HASH_RET_OK == ret)
    {
        index = d_hash_cmn_get_dma_mng_index(type);

        if (NUM_ERR_U32 != index)
        {
            if (DMA_INPUT_CHANNEL == setting_channel)
            {
                /* Input channel settings */
                channel = dma_mng[index].dma_src_channel;

                if (DMA_MNG_FOR_SHA2 == index)
                {
                    sha_hw_addr = HW_ADDR_SHA2;
                    req_source  = DMA_REQ_SRC_SHA2_IN;
                }
                else
                {
                    sha_hw_addr = HW_ADDR_SHA3;
                    req_source  = DMA_REQ_SRC_SHA3_IN;
                }

                /* Set priority mode and DMA master enable */
                REG_ICUM_DMAC.ICUM_DMAOR = (SHORT_REGISTER_SET_3 << OR_PR_POS_DMA) |
                                           (SHORT_REGISTER_SET_1 << OR_DME_POS_DMA);

                /* Set all register clear */
                REG_ICUM_DMAC.channel_reset = (REGISTER_SET_1 << channel);

                /* Set transfer request, dest addr mode, src addr mode, dest size and src size */
                REG_ICUM_DMAC_CH[channel].ICUM_DMATMR = (REGISTER_SET_1 << TMR_TRS_POS_DMA) |
                                                        (REGISTER_SET_0 << TMR_DM_POS_DMA)  |
                                                        (REGISTER_SET_1 << TMR_SM_POS_DMA)  |
                                                        (REGISTER_SET_6 << TMR_DTS_POS_DMA) |
                                                        (REGISTER_SET_6 << TMR_STS_POS_DMA);

                /* Set source address */
                REG_ICUM_DMAC_CH[channel].ICUM_DMASAR = setting_addr;

                /* Set destination address */
                REG_ICUM_DMAC_CH[channel].ICUM_DMADAR = sha_hw_addr;

                /* Set total length of message in bytes */
                REG_ICUM_DMAC_CH[channel].ICUM_DMATSR = transfer_byte_len;

                /* Set transfer count, transfer limit, pre-load enable and DMA request source */
                REG_ICUM_DMAC_CH[channel].ICUM_DMARS = (REGISTER_SET_1 << RS_TC_POS_DMA)  |
                                                       (REGISTER_SET_1 << RS_TL_POS_DMA)  |
                                                       (REGISTER_SET_1 << RS_PLE_POS_DMA) |
                                                       (req_source     << RS_RS_POS_DMA);

                /* Set addres error notification, addres error interrupt, transfer end interrupt and DMA enable */
                REG_ICUM_DMAC_CH[channel].ICUM_DMACHCR = (SHORT_REGISTER_SET_1 << CHCR_CAEE_POS_DMA) |
                                                         (SHORT_REGISTER_SET_1 << CHCR_CAIE_POS_DMA) |
                                                         (SHORT_REGISTER_SET_0 << CHCR_IE_POS_DMA)   |
                                                         (SHORT_REGISTER_SET_1 << CHCR_DE_POS_DMA);
            } /* end of if (DMA_INPUT_CHANNEL == setting_channel)*/
            else if (DMA_OUTPUT_CHANNEL == setting_channel)
            {
                /* Output channel settings */
                channel = dma_mng[index].dma_dst_channel;

                if (DMA_MNG_FOR_SHA2 == index)
                {
                    sha_hw_addr = HW_ADDR_SHA2;
                    req_source  = DMA_REQ_SRC_SHA2_OUT;
                }
                else
                {
                    sha_hw_addr = HW_ADDR_SHA3;
                    req_source  = DMA_REQ_SRC_SHA3_OUT;
                }

                /* Set all register clear */
                REG_ICUM_DMAC.channel_reset = (REGISTER_SET_1 << channel);

                /* Set transfer request, dest addr mode, src addr mode, dest size and src size */
                REG_ICUM_DMAC_CH[channel].ICUM_DMATMR = (REGISTER_SET_1 << TMR_TRS_POS_DMA) |
                                                        (REGISTER_SET_1 << TMR_DM_POS_DMA)  |
                                                        (REGISTER_SET_0 << TMR_SM_POS_DMA)  |
                                                        (REGISTER_SET_6 << TMR_DTS_POS_DMA) |
                                                        (REGISTER_SET_6 << TMR_STS_POS_DMA);

                /* Set source address */
                REG_ICUM_DMAC_CH[channel].ICUM_DMASAR = sha_hw_addr;

                /* Set destination address */
                REG_ICUM_DMAC_CH[channel].ICUM_DMADAR = setting_addr;

                /* Set total length of message in bytes */
                REG_ICUM_DMAC_CH[channel].ICUM_DMATSR = transfer_byte_len;

                /* Set transfer count, transfer limit, pre-load enable and DMA request source */
                REG_ICUM_DMAC_CH[channel].ICUM_DMARS = (REGISTER_SET_1 << RS_TC_POS_DMA)  |
                                                       (REGISTER_SET_0 << RS_TL_POS_DMA)  |
                                                       (REGISTER_SET_0 << RS_PLE_POS_DMA) |
                                                       (req_source     << RS_RS_POS_DMA);

                /* Set addres error notification, addres error interrupt, transfer end interrupt and DMA enable */
                REG_ICUM_DMAC_CH[channel].ICUM_DMACHCR = (SHORT_REGISTER_SET_1 << CHCR_CAEE_POS_DMA) |
                                                         (SHORT_REGISTER_SET_1 << CHCR_CAIE_POS_DMA) |
                                                         (SHORT_REGISTER_SET_1 << CHCR_IE_POS_DMA)   |
                                                         (SHORT_REGISTER_SET_1 << CHCR_DE_POS_DMA);
            } /* end of else if (DMA_OUTPUT_CHANNEL == setting_channel) */
            else
            {
                ret = HASH_RET_PARAMERR;
            }
        } /* end of if (NUM_ERR_U32 != index) */
        else
        {
            ret = HASH_RET_PARAMERR;
        }
    } /* end of if (HASH_RET_OK == ret) */
    else
    {
        /* do nothing */
    }

    return (ret);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0074]:[ICUMFW_UD_S4_D_HASH_0074]
* Function name: d_hash_cmn_wait_sts_dma
* Description  : Execution of DMA transfer
* Arguments    : (IN) const e_r_hash_type_t type
* Return Value : DMA_STATUS_NORMAL_END
*                DMA_STATUS_ADDR_ERR
*                DMA_STATUS_TIMEOUT_ERR
*                NUM_ERR_U32
***************************************************************************************/
uint32_t d_hash_cmn_wait_sts_dma(const e_r_hash_type_t type)
{
    uint32_t    index;
    uint32_t    state_mask;
    uint32_t    dma_status;
    uint32_t    timeout;
    uint32_t    input_channel;
    uint32_t    output_channel;

    index          = VARIABLE_INIT;
    state_mask     = VARIABLE_INIT;
    dma_status     = NUM_ERR_U32;
    timeout        = DMA_OPE_TIMEOUT;
    input_channel  = VARIABLE_INIT;
    output_channel = VARIABLE_INIT;

    index = d_hash_cmn_get_dma_mng_index(type);

    if (NUM_ERR_U32 != index)
    {
        /* Determine the channel for status confirmation */
        if (NUM0 == dma_mng[index].dma_src_channel)
        {
            state_mask = DMA_CHECK_STATUS_CH0_1;
        }
        else
        {
            state_mask = DMA_CHECK_STATUS_CH2_3;
        }

        do
        {
            /* Channel address error notification status */
            if (NUM0 != (REG_ICUM_DMAC.ICUM_DMAESTA & state_mask))
            {
                /* Address error occurred */
                dma_status = DMA_STATUS_ADDR_ERR;

                /* Hash type is already checked, so return value is not checked */
                (void)d_hash_cmn_set_dma_status(type, dma_status);
                break;
            }
            else
            {
                /* do nothing */
            }

            /* Channel interrupt status */
            if (NUM0 != (REG_ICUM_DMAC.ICUM_DMAISTA & state_mask))
            {
                /* Normal termination */
                dma_status = DMA_STATUS_NORMAL_END;

                /* Hash type is already checked, so return value is not checked */
                (void)d_hash_cmn_set_dma_status(type, dma_status);
                break;
            }
            else
            {
                /* do nothing */
            }

            /* Watchdog timer reset */
            d_hash_cmn_start_wdt_if_need();

            timeout--;

        } while (NUM0 != timeout);

        if (NUM0 != timeout)
        {
            /* do nothing */
        }
        else
        {
            /* DMA operation timeout */
            dma_status = DMA_STATUS_TIMEOUT_ERR;

            /* Hash type is already checked, so return value is not checked */
            (void)d_hash_cmn_set_dma_status(type, dma_status);
        }

        /* Clear DMA channel status flag */
        input_channel = dma_mng[index].dma_src_channel;
        output_channel = dma_mng[index].dma_dst_channel;

        REG_ICUM_DMAC_CH[input_channel].ICUM_DMACHFCR = (REGISTER_SET_1 << CHFCR_OVFC_POS_DMA) |
                                                        (REGISTER_SET_1 << CHFCR_DRQC_POS_DMA) |
                                                        (REGISTER_SET_1 << CHFCR_CAEC_POS_DMA) |
                                                        (REGISTER_SET_1 << CHFCR_TEC_POS_DMA)  |
                                                        (REGISTER_SET_1 << CHFCR_DEC_POS_DMA);

        REG_ICUM_DMAC_CH[output_channel].ICUM_DMACHFCR = (REGISTER_SET_1 << CHFCR_OVFC_POS_DMA) |
                                                         (REGISTER_SET_1 << CHFCR_DRQC_POS_DMA) |
                                                         (REGISTER_SET_1 << CHFCR_CAEC_POS_DMA) |
                                                         (REGISTER_SET_1 << CHFCR_TEC_POS_DMA)  |
                                                         (REGISTER_SET_1 << CHFCR_DEC_POS_DMA);
    } /* end of if (NUM_ERR_U32 != index) */
    else
    {
        /* do nothing */
    }

    return (dma_status);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0075]:[ICUMFW_UD_S4_D_HASH_0075]
* Function name: d_hash_cmn_init_reg_dma
* Description  : SHAB0DMA initialize
* Arguments    : (IN) const uint32_t mode
* Return Value : none
***************************************************************************************/
void d_hash_cmn_init_reg_dma(const uint32_t mode)
{

    /* ICUMX_SHAB0 initialize set */
    ICUM_SHACNT = (REGISTER_SET_1 << CNT_INIT_POS);

    /* ICUMX_SHAB0DMA initialize and mode set and endian set */
    ICUM_SHADMACNT = (REGISTER_SET_1 << DMACNT_INIT_POS) |
                     (REGISTER_SET_1 << DMACNT_ENDIAN_POS) |
                     (mode << DMACNT_MODE_POS);

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0076]:[ICUMFW_UD_S4_D_HASH_0076]
* Function name: d_hash_cmn_init_sts_dma
* Description  : Initialize DMA status
* Arguments    : (IN) const st_r_hash_ctx_t* const p_ctx
* Return Value : none
***************************************************************************************/
void d_hash_cmn_init_sts_dma(const st_r_hash_ctx_t* const p_ctx)
{

    if (HASH_USE_DMA_TRANSFER == p_ctx->use_dma_transfer)
    {
        /* Initialize DMA management table */
        /* Hash type is already checked, so return value is not checked */
        (void)d_hash_cmn_set_dma_status(p_ctx->type, DMA_STATUS_NON_EXEC);
    }
    else
    {
        /* do nothing */
    }

    return;
}

/*=====================================================================================================================
 Private function definitions
=====================================================================================================================*/

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0034]:[ICUMFW_UD_S4_D_HASH_0034]
* Function Name : r_hash_cmn_exec_block
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHCommonAPIsInternal
 * \brief       Perform HASH block processing.
 * \param       [in,out] p_ctx          An address of hash context
 * \param       [in] p_msg              An inputting message
 * \param       [in] msg_len            The length of the message in bytes
 * \param       [in] hash_block_size    HASH block size
 * \return
 *  - #HASH_RET_OK:                  Successfully done.
 *  - #HASH_RET_PARAMERR:            There is an error in the parameter.
 *  - #HASH_RET_LENGTHERR:           Buffer overflow.
 *  - #HASH_RET_TIMEOUTERR:          Operation timeouted.
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #ICUM_SHASTS
 *  - #ICUM_SHAIDAT, #ICUM_SHAFIFOCNT (Used by sub functions)
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Perform HASH block processing. <br>
 *        This process is called from a hash operation or an HMAC operation.
 *
 * Pseudo-code for the function can be found below. 
 * -# Set #HASH_RET_OK to ret
 * -# Set the argument msg_len to rem_size
 * -# Set 0 to msg_offset
 * -# If (p_ctx != NULL) && (p_msg != NULL)
 *   -# If p_ctx->transfer_prm.icup.leftover_msg_len < #HASH_LEFTOVER_MAX
 *     -# If p_ctx->transfer_prm.icup.leftover_msg_len != 0
 *       -# Copy msg data after the remaining data until the buffer is filled with #r_hash_cmn_cpy_byte
 *       -# Execute block processing for block size from leftover_msg using #r_hash_cmn_set_idat
 *       -# Set msg_offset to hash_block_size - leftover_msg_len
 *       -# Subtract the value of (hash_block_size - leftover_msg_len) from rem_size
 *       -# Set #HASH_LEFTOVER_MAX size 0 to p_ctx->transfer_prm.icup.leftover_msg with #r_hash_cmn_set_byte
 *       -# Set 0 to p_ctx->transfer_prm.icup.leftover_msg_len
 *       -# Wait IDATA_READY is 1 with #r_hash_cmn_wait_sts
 *     -# Else
 *       -# Do nothing
 *     -# If ret == #HASH_RET_OK
 *       -# Loop while rem_size is more than hash_block_size
 *         -# Execute block processing for block size from p_msg[msg_offset] using #r_hash_cmn_set_idat
 *         -# Add hash_block_size to msg_offset
 *         -# Subtract hash_block_size from rem_size
 *         -# Wait for IDATA_READY with #r_hash_cmn_wait_for_idata_ready
 *         -# Set function result to ret
 *         -# If ret == #HASH_RET_OK
 *           -# Break
 *         -# Else
 *           -# Do nothing
 *       -# If ret == #HASH_RET_OK
 *         -# If rem_size != 0
 *           -# Copy the leftover messages into the buffer with #r_hash_cmn_cpy_byte
 *           -# Set rem_size to p_ctx->transfer_prm.icup.leftover_msg_len
 *         -# Else
 *           -# Do nothing
 *         -# Get intermediate data with #r_hash_cmn_get_intermediate
 *       -# Else
 *         -# Do nothing
 *     -# Else
 *       -# Do nothing
 *   -# Else
 *     -# Set #HASH_RET_LENGTHERR to ret
 * -# Else
 *   -# Set #HASH_RET_PARAMERR to ret
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static e_r_hash_ret_t r_hash_cmn_exec_block(st_r_hash_ctx_t* const p_ctx, const uint8_t* const p_msg,
                                            const uint32_t msg_len, const uint32_t hash_block_size)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    e_r_hash_ret_t ret = HASH_RET_OK;
    uint32_t       rem_size = msg_len;
    uint32_t       msg_offset = 0U;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* NULL check */
    if(p_ctx != NULL)
    {
        /* Buffer overflow prevention range check */
        if(p_ctx->transfer_prm.icup.leftover_msg_len < HASH_LEFTOVER_MAX)
        {
            /* Process the last unprocessed data */
            if(p_ctx->transfer_prm.icup.leftover_msg_len != 0U) /* remaining data */
            {
                /* remaining data + msg data create */
                r_hash_cmn_cpy_byte(&p_ctx->transfer_prm.icup.leftover_msg[p_ctx->transfer_prm.icup.leftover_msg_len],
                                    &p_msg[msg_offset], (hash_block_size - p_ctx->transfer_prm.icup.leftover_msg_len));
                r_hash_cmn_set_idat(p_ctx->transfer_prm.icup.leftover_msg, hash_block_size);
                msg_offset = (hash_block_size - p_ctx->transfer_prm.icup.leftover_msg_len);
                rem_size -= (hash_block_size - p_ctx->transfer_prm.icup.leftover_msg_len);

                r_hash_cmn_set_byte(p_ctx->transfer_prm.icup.leftover_msg, 0U, HASH_LEFTOVER_MAX);
                p_ctx->transfer_prm.icup.leftover_msg_len = 0U;

                /* wait for IDATA_READY */
                ret = r_hash_cmn_wait_for_idata_ready();
            }
            else
            {
                /* do nothing */
            }

            if(ret == HASH_RET_OK)
            {
                /* Process the msg data */
                while(rem_size >= hash_block_size)
                {
                    r_hash_cmn_set_idat(&p_msg[msg_offset], hash_block_size);
                    msg_offset += hash_block_size;
                    rem_size -= hash_block_size;

                    /* wait for IDATA_READY */
                    ret = r_hash_cmn_wait_for_idata_ready();

                    if(ret != HASH_RET_OK)
                    {
                        break;
                    }
                    else
                    {
                        /* do nothing */
                    }
                }

                if(ret == HASH_RET_OK)
                {
                    /* save remaining data */
                    if(rem_size != 0U)
                    {
                        r_hash_cmn_cpy_byte(p_ctx->transfer_prm.icup.leftover_msg, &p_msg[msg_offset], rem_size);
                        p_ctx->transfer_prm.icup.leftover_msg_len = rem_size;
                    }
                    else
                    {
                        /* do nothing */
                    }

                    /* get intermediate data in register */
                    ret = r_hash_cmn_get_intermediate(p_ctx->intermediate_data, p_ctx->intermediate_len);
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
            ret = HASH_RET_LENGTHERR;
        }
    }
    else
    {
        ret = HASH_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_hash_cmn_exec_block()
**********************************************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0035]:[ICUMFW_UD_S4_D_HASH_0035]
* Function name: r_hash_cmn_set_intermediate
* Description  : Set intermediate data in register
* Arguments    : (IN) const uint32_t* const p_im_data,
*                     const uint32_t* const p_im_len
* Return Value : none
***************************************************************************************/
static void r_hash_cmn_set_intermediate(const uint32_t* const p_im_data, const uint32_t* const p_im_len)
{
    uint32_t       read_register;

    read_register = VARIABLE_INIT;

    /* NULL check */
    if ((NULL != p_im_data) && (NULL != p_im_len))
    {
        /* intermediate data input */
        /* Define macro ICUM_SHADID0 is cast to access a register */
        ICUM_SHADID0 = p_im_data[0];

        /* Define macro ICUM_SHADID1 is cast to access a register */
        ICUM_SHADID1 = p_im_data[1];

        /* Define macro ICUM_SHADID2 is cast to access a register */
        ICUM_SHADID2 = p_im_data[2];

        /* Define macro ICUM_SHADID3 is cast to access a register */
        ICUM_SHADID3 = p_im_data[3];

        /* Define macro ICUM_SHADID4 is cast to access a register */
        ICUM_SHADID4 = p_im_data[4];

        /* Define macro ICUM_SHADID5 is cast to access a register */
        ICUM_SHADID5 = p_im_data[5];

        /* Define macro ICUM_SHADID6 is cast to access a register */
        ICUM_SHADID6 = p_im_data[6];

        /* Define macro ICUM_SHADID7 is cast to access a register */
        ICUM_SHADID7 = p_im_data[7];

        /* intermediate data length input */
        /* Define macro ICUM_SHADIL0 is cast to access a register */
        ICUM_SHADIL0 = p_im_len[0];

        /* Define macro ICUM_SHADIL1 is cast to access a register */
        ICUM_SHADIL1 = p_im_len[1];

        read_register = ICUM_SHACNT;

        /* Get MODE information and set to lower 4 bits */
        read_register = (read_register & ICUM_SHACNT_MODE_MASK) >> CNT_MODE_POS;

        /* When hash type is SHA512_224, SHA512_256, SHA384, SHA512 */
        if ((CNT_MODE_VAL_SHA512_224 <= read_register) && (CNT_MODE_VAL_SHA512 >= read_register))
        {
            ICUM_SHADDCNT = (REGISTER_SET_1 << DDCNT_EX_DID_POS) |
                            (REGISTER_SET_1 << DDCNT_EX_DIL_POS);

            /* intermediate data input (expansion area) */
            ICUM_SHADID0 = p_im_data[8];
            ICUM_SHADID1 = p_im_data[9];
            ICUM_SHADID2 = p_im_data[10];
            ICUM_SHADID3 = p_im_data[11];
            ICUM_SHADID4 = p_im_data[12];
            ICUM_SHADID5 = p_im_data[13];
            ICUM_SHADID6 = p_im_data[14];
            ICUM_SHADID7 = p_im_data[15];

            /* intermediate data length input (expansion area) */
            ICUM_SHADIL0 = p_im_len[2];
            ICUM_SHADIL1 = p_im_len[3];

            ICUM_SHADDCNT = (REGISTER_SET_1 << DDCNT_EX_DID_POS) |
                            (REGISTER_SET_1 << DDCNT_EX_DIL_POS) |
                            (REGISTER_SET_1 << DDCNT_DIRECTIN_LOAD_POS);
            
        } /* end of if ((CNT_MODE_VAL_SHA512_224 <= read_register) && (CNT_MODE_VAL_SHA512 >= read_register)) */
        else
        {
            /* Load intermediate data */
            /* Define macro ICUM_SHADDCNT is cast to access a register */
            ICUM_SHADDCNT = (REGISTER_SET_0 << DDCNT_EX_DID_POS) |
                            (REGISTER_SET_0 << DDCNT_EX_DIL_POS) |
                            (REGISTER_SET_1 << DDCNT_DIRECTIN_LOAD_POS);
        }
    } /* end of if ((NULL != p_im_data) && (NULL != p_im_len)) */
    else
    {
        /* It has been confirmed that there is no problem */
        /* do nothing */
    }

    return;
}

/**********************************************************************************************************************
* Function ID   : [ICUMFW_CD_S4_D_HASH_0036]:[ICUMFW_UD_S4_D_HASH_0036]
* Function Name : r_hash_cmn_wait_sts
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     HASHCommonAPIsInternal
 * \brief       
 * \param       [in] p_reg            Target register
 * \param       [in] pos              Bit position of wait status field in target register
 * \param       [in] break_sts        Break status
 * \return
 *  - #HASH_RET_OK:                  Successfully done.
 *  - #HASH_RET_PARAMERR:            Argument NULL.
 *  - #HASH_RET_TIMEOUTERR:          process timeouted.
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Wait until the specified state.
 *
 * Pseudo-code for the function can be found below. 
 * -# Set #TIMEOUT_INIT to timeout
 * -# Set #HASH_RET_OK to ret
 * -# If p_reg != NULL
 *   -# Loop while timeout is not 0
 *     -# Set *p_reg to temp
 *     -# If (break_sts << pos) == (temp & (0x1U << pos)) then, break.
 *     -# Else
 *       -# do nothing
 *     -# Decrement i
 *   -# If timeout == 0
 *     -# Set #HASH_RET_TIMEOUTERR to ret
 *   -# Else
 *     -# Do nothing
 * -# Else
 *   -# Set #HASH_RET_PARAMERR to ret
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
static e_r_hash_ret_t r_hash_cmn_wait_sts(volatile const uint32_t* const p_reg,
                                            const uint32_t pos, const uint32_t break_sts)
{
    /*-----------------------------------------------------------------------------------------------------------------
     Local variables
    -----------------------------------------------------------------------------------------------------------------*/
    volatile uint32_t temp;
    uint32_t          timeout = TIMEOUT_INIT;
    e_r_hash_ret_t    ret = HASH_RET_OK;

    /*-----------------------------------------------------------------------------------------------------------------
     Function body
    -----------------------------------------------------------------------------------------------------------------*/
    /* NULL check */
    if(p_reg != NULL)
    {
        /* Wait for change to break_sts until timeout */
        while(timeout != 0U)
        {
            temp = *p_reg;

            if((break_sts << pos) == (temp & (0x1U << pos)))
            {
                break;
            }
            else
            {
                /* do nothing */
            }

            timeout--;
        }

        if(timeout == 0U)
        {
            ret = HASH_RET_TIMEOUTERR;
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
* End of function r_hash_cmn_wait_sts()
**********************************************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0077]:[ICUMFW_UD_S4_D_HASH_0077]
* Function name: d_hash_cmn_get_dma_mng_index
* Description  : Get Index of management information table for DMA transfer
* Arguments    : (IN)  type : HASH type
* Return Value : DMA_MNG_FOR_SHA2
*                DMA_MNG_FOR_SHA3
*                NUM_ERR_U32
***************************************************************************************/
static uint32_t d_hash_cmn_get_dma_mng_index(const e_r_hash_type_t type)
{
    uint32_t    index;

    index = NUM_ERR_U32;

    if (HASH_TYPE_SHA512_256 >= type)
    {
        index = DMA_MNG_FOR_SHA2;
    }
    else if (HASH_TYPE_SHA3_512 >= type)
    {
        index = DMA_MNG_FOR_SHA3;
    }
    else
    {
        /* do nothing */
    }

    return (index);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0078]:[ICUMFW_UD_S4_D_HASH_0078]
* Function name: d_hash_cmn_alloc_dma_channel
* Description  : Allocate the DMA channel
* Arguments    : (IN)  index : Index of management information table for DMA transfer
* Return Value : None
***************************************************************************************/
static void d_hash_cmn_alloc_dma_channel(const uint32_t index)
{
    uint32_t    other_index;

    other_index = (index + NUM1) % MNG_ELEMENT_NUM;

    switch (dma_mng[other_index].dma_src_channel)
    {
        case HASH_DMA_CH_UNUSED:
        case NUM2:
            dma_mng[index].dma_src_channel = NUM0;
            dma_mng[index].dma_dst_channel = NUM1;
            break;

        case NUM0:
            dma_mng[index].dma_src_channel = NUM2;
            dma_mng[index].dma_dst_channel = NUM3;
            break;

        default:
            dma_mng[index].dma_src_channel = HASH_DMA_CH_UNUSED;
            dma_mng[index].dma_dst_channel = HASH_DMA_CH_UNUSED;
            break;
    }

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_HASH_0080]:[ICUMFW_UD_S4_D_HASH_0080]
* Function name: d_hash_cmn_start_wdt_if_need
* Description  : Restarting the watchdog timer
* Arguments    : none
* Return Value : none
***************************************************************************************/
static void d_hash_cmn_start_wdt_if_need(void)
{
    bool wdt_flag;

    wdt_flag = false;

    /* check need restart watchdog timer */
    D_WDT_GetIntRequest(&wdt_flag);
    if (true == wdt_flag)
    {
        /* restart watchdog timer */
        D_WDT_Start();
    }
    else
    {
        /* do nothing */
    }

    return;
}

/*=====================================================================================================================
 End of file
=====================================================================================================================*/

