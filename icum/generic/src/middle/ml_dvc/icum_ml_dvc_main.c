/******************************************************************************/
/* Component Name  DVC Manager                                                */
/******************************************************************************/
/* Product      : ICU-M Low Level Driver (Demo Part)                          */
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
/*******************************************************************************
* Header files
********************************************************************************/

#include    "renesas_types.h"
#include    "icum_common_pub.h"
#include    "r_icumif_api.h"
#include    "icum_l_cry_pk_pub.h"
#include    "icum_ml_dvc_pub.h"
#include    "icum_d_info_pub.h"
#include    "icum_d_cry_aes_pub.h"
#include    "icum_l_toolbox_pub.h"

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/

/* 8 bits in 1 byte */
#define BITS_IN_BYTE    ((uint32_t)8)
#define ROUND_UP        ((uint32_t)1)

#define ZERO            (0x00u)
#define NUM0            ((uint32_t)0uL)          /* value : 0 */
#define NUM1            ((uint32_t)1uL)          /* value : 1 */
#define DATA_ZERO       ((uint8_t)0u)            /* value : 0 used for initialization */

#define AES_STATE_INIT  ((uint32_t)0xFFFFFFFFu)  /* initial state */

/*******************************************************************************
* Local variables
********************************************************************************/
/* UCID Cache */
static uint32_t g_ucid_buffer[4u];
extern uint32_t g_ContextAddress;

/*******************************************************************************
* Functions
********************************************************************************/


/*==============================================================================
  [PUBLIC]
  =============================================================================*/
/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_DVCMNG_0001]:[ICUMFW_UD_ML_DVCMNG_0001]
* Function name: ML_DVC_Init
* Description  : Initialize DVC MNG
* Arguments    : none
* Return Value : ER_OK
*                (D_INFO_GetInfo() returns errors other than ER_OK due to specifications,
*                 but since the parameters are fixed, in fact it is not returned except ER_OK.)
***************************************************************************************/
uint32_t ML_DVC_Init(void)
{
    uint32_t err;

    g_ucid_buffer[0u] = 0xFFFFFFFFUL;
    g_ucid_buffer[1u] = 0xFFFFFFFFUL;
    g_ucid_buffer[2u] = 0xFFFFFFFFUL;
    g_ucid_buffer[3u] = 0xFFFFFFFFUL;

    /* Get UCID  & Cache in local buffer */
    err = D_INFO_GetInfo(INFO_UCID,&g_ucid_buffer[0u]);

    switch (err)
    {
        case ER_OK:
            /* DVC manager initialize complete */
            /* do nothing */
            break;
        case ER_PARAM:
            /* never occur */
            break;
        case ER_D_INFO_INVALID_STATE:
            /* never occur */
            break;
        default:
            /* never occur */
            break;
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_DVCMNG_0002]:[ICUMFW_UD_ML_DVCMNG_0002]
* Function name: ML_DVC_Calculate
* Description  : Calculate the DVC value
* Arguments    : *p_par : Pointer to input parameter structure.
                          const uint8_t *p_input  : Data stream address of DVC target area
                          uint16_t      data_size : Data stream size of DVC target area
                          dvc_t         *p_output : DVC value output destination buffer address
*                calc_type : Selection for DVC value calculation type.
* Return Value : ER_OK
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_ML_DVC_ALIGN       (Does not occur with ML_DVC_CALC_TYPE_HASH)
*                ER_ML_DVC_AES_TIMEOUT (Does not occur with ML_DVC_CALC_TYPE_HASH)
*                ER_PARAM
***************************************************************************************/
uint32_t ML_DVC_Calculate(dvc_calculate_t *p_par, uint32_t calc_type)
{
    uint32_t err;
    uint32_t msg_size_in_bits;
    uint16_t last_block;
    aes_all_at_once_t at_once_param;

    /* Initialize parameters */
    err = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&at_once_param, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_t));

#if defined(FSW_DVC_HASH)
    if (ML_DVC_CALC_TYPE_HASH == calc_type)
    {
        err = L_HASH_CalculateDVC(
                (const uint8_t*)&g_ucid_buffer[0u],
                (const uint8_t*)&(p_par->p_input[0u]),
                ((uint32_t)p_par->data_size),
                (uint8_t*)&(p_par->p_output->dvc[0u]));

        if (ER_OK != err)
        {
            err = ER_PARAM;
        }
        else
        {
            /* successful completion */
            /* Do nothing */
        }
    } /* end of if (ML_DVC_CALC_TYPE_HASH == calc_type) */
    else
#endif  /* defined(FSW_DVC_HASH) */
    {
        /* AES CMAC */
        g_ContextAddress          = AES_STATE_INIT;
        msg_size_in_bits          = (uint32_t)p_par->data_size * BITS_IN_BYTE;
        at_once_param.cipher_mode = D_AES_CIPHER_MODE_CMAC;
        at_once_param.direction   = D_AES_DIR_ENCRYPTION;
        at_once_param.number      = ((msg_size_in_bits
                                      + ((uint32_t)AES_BLOCK_SIZE_IN_BITS - NUM1)) / (uint32_t)AES_BLOCK_SIZE_IN_BITS);
        at_once_param.key_type    = D_AES_KEYTYPE_PLAIN;
        last_block = (uint16_t)(msg_size_in_bits % (uint32_t)AES_BLOCK_SIZE_IN_BITS);
        if (NUM0 == (uint32_t)last_block)
        {
            if (NUM0 != msg_size_in_bits)
            {
                at_once_param.last_block_in_bits = (uint16_t)AES_BLOCK_SIZE_IN_BITS;
            }
            else
            {
                at_once_param.last_block_in_bits = (uint16_t)NUM0;
            }
        }
        else
        {
            at_once_param.last_block_in_bits = last_block;
        }
        at_once_param.p_aes_key  = &g_ucid_buffer[0u];
        at_once_param.key_length = D_AES_KEY_SIZE_IN_BYTES_128BIT;
        at_once_param.p_aes_iv   = NULL;
        at_once_param.p_aes_src  = (uint32_t*)&(p_par->p_input[0u]);
        at_once_param.p_aes_dest = (uint32_t*)&(p_par->p_output->dvc[0u]);
        at_once_param.async      = D_AES_TRANS_SYNC;
        err = D_AES_ExecutionAtOnce(&at_once_param);

        switch (err)
        {
            case ER_OK:
                /* successful completion */
                err = ER_OK;
                break;
            case ER_PENDING:
                /* Pending execution */
                err = ER_PENDING;
                break;
            case ER_IN_PROCESS:
                /* In process. */
                err = ER_IN_PROCESS;
                break;
            case ER_PARAM:
                err = ER_PARAM;
                break;
            case ER_D_AES_ALIGN_ERROR:
                err = ER_ML_DVC_ALIGN;
                break;
            case ER_D_AES_TIMEOUT_ERROR:
                err = ER_ML_DVC_AES_TIMEOUT;
                break;
            default:
                err = ER_PARAM;
                break;
        }
    } /* end of else */

    return (err);
}

/* ----- end of source code file ----- */
