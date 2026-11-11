/******************************************************************************/
/* Component Name   ML Context Manager file                                   */
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

/**********************************************************************************
* Headers
**********************************************************************************/

#include "renesas_types.h"
#include "icum_common_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_ml_ctx_pub.h"
#include "icum_ml_ctx.h"

/**********************************************************************************
* Global variables
***********************************************************************************/
/* Intermediate value area */
#if defined(FSW_USE_CRAM)
#pragma ghs section bss = ".start.icum_cram.bss"
#endif
static ctx_aes_t g_aes_ctx[CTX_NB_AES];
static ctx_rsa_ecc_t g_rsa_ecc_ctx[CTX_NB_RSA_ECC];
static ctx_hash_t g_hash_ctx[CTX_NB_HASH];
#if defined(FSW_USE_CRAM)
#pragma ghs section bss = default
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ContextMNG_0001]:[ICUMFW_UD_ML_ContextMNG_0001]
* Function name: ML_CTX_Init
* Description  : Initialize the intermediate value area.
* Arguments    : N/A
* Return Value : ER_OK
***************************************************************************************/
uint32_t ML_CTX_Init(void)
{
    uint32_t count_nb;

    /* Initialize the use state of the AES intermediate value area */
    for(count_nb = 0uL; CTX_NB_AES > count_nb; count_nb++)
    {
        g_aes_ctx[count_nb].ctx_addr = CTX_FREE;
    }
    /* Initialize the use state of the RSA and ECC intermediate value area */
    for(count_nb = 0uL; CTX_NB_RSA_ECC > count_nb; count_nb++)
    {
        g_rsa_ecc_ctx[count_nb].ctx_addr = CTX_FREE;
    }
    /* Initialize the use state of the HASH intermediate value area */
    for(count_nb = 0uL; CTX_NB_HASH > count_nb; count_nb++)
    {
        g_hash_ctx[count_nb].ctx_addr = CTX_FREE;
    }
#if defined(FSW_USE_CRAM)
    /* Clear of g_rsa_ecc_ctx in CRAM*/
    L_Toolbox_MemSet((uint8_t *)&g_rsa_ecc_ctx[0], (uint8_t)0, (int16_t)sizeof(g_rsa_ecc_ctx));
#endif

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ContextMNG_0002]:[ICUMFW_UD_ML_ContextMNG_0002]
* Function name: ML_CTX_Get
* Description  : Get the address of the specified intermediate value area.
* Arguments    : type      : Type of context
*                            Range: CTX_TYPE_AES
*                                   CTX_TYPE_RSA
*                                   CTX_TYPE_ECC
*                                   CTX_TYPE_HASH
*                                   CTX_TYPE_HMAC
*                p_context : Start address of acquired intermediate value area
* Return Value : ER_OK
*                ER_PARAM
*                ER_ML_CTX_ALL_USE
***************************************************************************************/
uint32_t ML_CTX_Get(uint16_t type, uint32_t **p_context)
{
    uint32_t err = ER_OK;
    uint32_t count_nb;

    switch((uint32_t)type)
    {
        case CTX_TYPE_AES:
            /* Check context address */
            for(count_nb = 0uL; CTX_NB_AES > count_nb; count_nb++)
            {
                /* Found the free context address */
                if(CTX_FREE == g_aes_ctx[count_nb].ctx_addr)
                {
                    /* Set the use state */
                    g_aes_ctx[count_nb].ctx_addr = (uint32_t)g_aes_ctx[count_nb].context;
                    /* Get the address of the specified intermediate value area. */
                    *p_context = g_aes_ctx[count_nb].context;
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
            /* Not found the free context address */
            if(CTX_NB_AES == count_nb)
            {
                err = ER_ML_CTX_ALL_USE;
            }
            else
            {
                /* Do nothing */
            }
            break;
        case CTX_TYPE_RSA:
        case CTX_TYPE_ECC:
            /* Check context address */
            for(count_nb = 0uL; CTX_NB_RSA_ECC > count_nb; count_nb++)
            {
                /* Found the free context address */
                if(CTX_FREE == g_rsa_ecc_ctx[count_nb].ctx_addr)
                {
                    /* Set the use state */
                    g_rsa_ecc_ctx[count_nb].ctx_addr = (uint32_t)g_rsa_ecc_ctx[count_nb].context;
                    /* Get the address of the specified intermediate value area. */
                    *p_context = g_rsa_ecc_ctx[count_nb].context;
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
            /* Not found the free context address */
            if(CTX_NB_RSA_ECC == count_nb)
            {
                err = ER_ML_CTX_ALL_USE;
            }
            else
            {
                /* Do nothing */
            }
            break;
        case CTX_TYPE_HASH:
        case CTX_TYPE_HMAC:
            /* Check context address */
            for(count_nb = 0uL; CTX_NB_HASH > count_nb; count_nb++)
            {
                /* Found the free context address */
                if(CTX_FREE == g_hash_ctx[count_nb].ctx_addr)
                {
                    /* Set the use state */
                    g_hash_ctx[count_nb].ctx_addr = (uint32_t)g_hash_ctx[count_nb].context;
                    /* Get the address of the specified intermediate value area. */
                    *p_context = g_hash_ctx[count_nb].context;
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
            /* Not found the free context address */
            if(CTX_NB_HASH == count_nb)
            {
                err = ER_ML_CTX_ALL_USE;
            }
            else
            {
                /* Do nothing */
            }
            break;
        default:
            err = ER_PARAM;
            break;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ContextMNG_0003]:[ICUMFW_UD_ML_ContextMNG_0003]
* Function name: ML_CTX_Free
* Description  : Free the specified intermediate value area.
* Arguments    : type      : Type of context
*                            Range: CTX_TYPE_AES
*                                   CTX_TYPE_RSA
*                                   CTX_TYPE_ECC
*                                   CTX_TYPE_HASH
*                                   CTX_TYPE_HMAC
*                p_context : Start address of acquired intermediate value area
* Return Value : ER_OK
*                ER_PARAM
*                ER_ML_CTX_NOT_FOUND
***************************************************************************************/
uint32_t ML_CTX_Free(uint16_t type, uint32_t *p_context)
{
    uint32_t err = ER_OK;
    uint32_t count_nb;

    switch((uint32_t)type)
    {
        case CTX_TYPE_AES:
            /* Check context address */
            for(count_nb = 0uL; CTX_NB_AES > count_nb; count_nb++)
            {
                /* Found the specified context address */
                if(g_aes_ctx[count_nb].context == p_context)
                {
                    /* Set zero to the specified intermediate value area. */
                    L_Toolbox_MemClr64((uint64_t *)&g_aes_ctx[count_nb], (int32_t)sizeof(ctx_aes_t));
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
            /* Not found the specified context address */
            if(CTX_NB_AES == count_nb)
            {
                err = ER_ML_CTX_NOT_FOUND;
            }
            else
            {
                /* Do nothing */
            }
            break;
        case CTX_TYPE_RSA:
        case CTX_TYPE_ECC:
            /* Check context address */
            for(count_nb = 0uL; CTX_NB_RSA_ECC > count_nb; count_nb++)
            {
                /* Found the specified context address */
                if(g_rsa_ecc_ctx[count_nb].context == p_context)
                {
                    /* Set zero to the specified intermediate value area. */
                    L_Toolbox_MemClr64((uint64_t *)&g_rsa_ecc_ctx[count_nb], (int32_t)sizeof(ctx_rsa_ecc_t));
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
            /* Not found the specified context address */
            if(CTX_NB_RSA_ECC == count_nb)
            {
                err = ER_ML_CTX_NOT_FOUND;
            }
            else
            {
                /* Do nothing */
            }
            break;
        case CTX_TYPE_HASH:
        case CTX_TYPE_HMAC:
            /* Check context address */
            for(count_nb = 0uL; CTX_NB_HASH > count_nb; count_nb++)
            {
                /* Found the specified context address */
                if(g_hash_ctx[count_nb].context == p_context)
                {
                    /* Set zero to the specified intermediate value area. */
                    L_Toolbox_MemClr64((uint64_t *)&g_hash_ctx[count_nb], (int32_t)sizeof(ctx_hash_t));
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
            /* Not found the specified context address */
            if(CTX_NB_HASH == count_nb)
            {
                err = ER_ML_CTX_NOT_FOUND;
            }
            else
            {
                /* Do nothing */
            }
            break;
        default:
            err = ER_PARAM;
            break;
    }

    return err;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ContextMNG_0004]:[ICUMFW_UD_ML_ContextMNG_0004]
* Function name: ML_CTX_InUseCheckAes
* Description  : Check existence of context in use.(AES Only)
* Arguments    : p_judge_use
* Return Value : ER_OK
***************************************************************************************/
uint32_t ML_CTX_InUseCheckAes(uint8_t *p_judge_use)
{
    uint32_t count_nb;
    uint32_t  judge_use = CTX_FREE;

    /* Check context address */
    for(count_nb = 0uL; CTX_NB_AES > count_nb; count_nb++)
    {
        if(CTX_FREE != g_aes_ctx[count_nb].ctx_addr)
        {
            judge_use = CTX_IN_USE;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }
    *p_judge_use = (uint8_t)judge_use;

    return ER_OK;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ContextMNG_0005]:[ICUMFW_UD_ML_ContextMNG_0005]
* Function name: ML_CTX_InUseCheckNotAes
* Description  : Check existence of context in use.(except AES)
* Arguments    : p_judge_use
* Return Value : ER_OK
***************************************************************************************/
uint32_t ML_CTX_InUseCheckNotAes(uint8_t *p_judge_use)
{
    uint32_t count_nb;
    uint32_t  judge_use = CTX_FREE;

    /* Check context address */
    for(count_nb = 0uL; CTX_NB_RSA_ECC > count_nb; count_nb++)
    {
        if(CTX_FREE != g_rsa_ecc_ctx[count_nb].ctx_addr)
        {
            judge_use = CTX_IN_USE;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }
    if(CTX_FREE == judge_use)
    {
        /* Check context address */
        for(count_nb = 0uL; CTX_NB_HASH > count_nb; count_nb++)
        {
            if(CTX_FREE != g_hash_ctx[count_nb].ctx_addr)
            {
                judge_use = CTX_IN_USE;
                break;
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    else
    {
        /* Do nothing */
    }
    *p_judge_use = (uint8_t)judge_use;

    return ER_OK;
}
