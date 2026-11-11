/******************************************************************************/
/* Component Name   Cryptographic Library HASH                                */
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
*  Headers
**********************************************************************************/
/* For use common definition */
#include "icum_common_pub.h"

#include "icum_l_cry_pk_pub.h"
#include "icum_l_hash.h"

#include "../cry_common/icum_l_my_toolbox.h"

#include "r_icum_hash_api.h"

/*********************************************************************************
*  Local constants
*********************************************************************************/

/* note: should follow the order given in hash_type_t */
static const oid_hash_t oid_hash[] =
{
    /* SHA-2 224  */
    {9, { 2, 16,   840,      1, 101,  3, 4, 2, 4, 0, 0, 0, 0, 0, 0, 0 } },
    /* SHA-2 256  */
    {9, { 2, 16,   840,      1, 101,  3, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0 } },
    /* SHA-2 384  */
    {9, { 2, 16,   840,      1, 101,  3, 4, 2, 2, 0, 0, 0, 0, 0, 0, 0 } },
    /* SHA-2 512  */
    {9, { 2, 16,   840,      1, 101,  3, 4, 2, 3, 0, 0, 0, 0, 0, 0, 0 } }
};

static st_r_hash_opt_t  opt = { 0 };

/**********************************************************************************
* Local defines
**********************************************************************************/
#define DVC_LENGTH         ((uint32_t)16u)      /* value : 16 */
#define HASH256_BYTE_SIZE  ((uint32_t)32u)      /* value : 32 */
#define ER_EXTRERNAL_OK    (0x55555555)         /* Normal termination of external hash function*/
#define ER_EXTRERNAL_ERROR (0xAAAAAAAA)         /* Abnormal termination of external hash function*/

/* unsigned data size */
#define NUM0                           (0u)

/* ON/OFF */
#define OFF                              (0u)
#define ON                               (1u)

/**********************************************************************************
*  Functions
**********************************************************************************/

/*===========================================================================
  Register HASH function
  [PUBLIC]
  ===========================================================================*/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0007]:[ICUMFW_UD_L_CRY_HASH_0007]
* Function name: l_HASH_Register()
* Description  : # Register HASH function based on HASH type.
* Arguments    : (IN)  hash_type    : HASH type
*                (IN-OUT)  p_job    : Job control parameter
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t l_HASH_Register(hash_type_t hash_type, hash_descriptor_t *p_hash_desc)
{
    uint32_t err;

    err = ER_OK;

    /* SHA-3, SHA512_224, SHA512_256 is operated by the HASH driver, */
    /* but the same settings as SHA-2 are set to avoid null access of the pointer. */
    switch(hash_type)
    {
        case E_SHA2_224:
            p_hash_desc->p_hash_init      = &l_HASH_Sha224Init;
            p_hash_desc->p_hash_process   = &l_HASH_Sha256Process;
            p_hash_desc->p_hash_terminate = &l_HASH_Sha224Done;
            p_hash_desc->hash_bitlen      = HASH_BITLEN_224;
            p_hash_desc->blocksize        = BLOCKSIZE_SHA2_224;
            p_hash_desc->p_oid            = &oid_hash[E_SHA2_224];
            break;
        case E_SHA2_256:
            p_hash_desc->p_hash_init      = &l_HASH_Sha256Init;
            p_hash_desc->p_hash_process   = &l_HASH_Sha256Process;
            p_hash_desc->p_hash_terminate = &l_HASH_Sha256Done;
            p_hash_desc->hash_bitlen      = HASH_BITLEN_256;
            p_hash_desc->blocksize        = BLOCKSIZE_SHA2_256;
            p_hash_desc->p_oid            = &oid_hash[E_SHA2_256];
            break;
        case E_SHA2_256_SLOW_RAM:
            p_hash_desc->p_hash_init      = &l_HASH_Sha256Init;
            p_hash_desc->p_hash_process   = &l_HASH_Sha256ProcessSlow;
            p_hash_desc->p_hash_terminate = &l_HASH_Sha256Done;
            p_hash_desc->hash_bitlen      = HASH_BITLEN_256;
            p_hash_desc->blocksize        = BLOCKSIZE_SHA2_256;
            p_hash_desc->p_oid            = &oid_hash[E_SHA2_256];
            break;
        case E_SHA2_384:
            p_hash_desc->p_hash_init      = &l_HASH_Sha384Init;
            p_hash_desc->p_hash_process   = &l_HASH_Sha512Process;
            p_hash_desc->p_hash_terminate = &l_HASH_Sha384Done;
            p_hash_desc->hash_bitlen      = HASH_BITLEN_384;
            p_hash_desc->blocksize        = BLOCKSIZE_SHA2_384;
            p_hash_desc->p_oid            = &oid_hash[E_SHA2_384];
            break;
        case E_SHA2_512:
            p_hash_desc->p_hash_init      = &l_HASH_Sha512Init;
            p_hash_desc->p_hash_process   = &l_HASH_Sha512Process;
            p_hash_desc->p_hash_terminate = &l_HASH_Sha512Done;
            p_hash_desc->hash_bitlen      = HASH_BITLEN_512;
            p_hash_desc->blocksize        = BLOCKSIZE_SHA2_512;
            p_hash_desc->p_oid            = &oid_hash[E_SHA2_512];
            break;
        case E_SHA2_512_224:
            p_hash_desc->p_hash_init      = &l_HASH_Sha512Init;
            p_hash_desc->p_hash_process   = &l_HASH_Sha512Process;
            p_hash_desc->p_hash_terminate = &l_HASH_Sha512Done;
            p_hash_desc->hash_bitlen      = HASH_BITLEN_224;
            p_hash_desc->blocksize        = BLOCKSIZE_SHA2_512_224;
            p_hash_desc->p_oid            = &oid_hash[E_SHA2_512];
            break;
        case E_SHA2_512_256:
            p_hash_desc->p_hash_init      = &l_HASH_Sha512Init;
            p_hash_desc->p_hash_process   = &l_HASH_Sha512Process;
            p_hash_desc->p_hash_terminate = &l_HASH_Sha512Done;
            p_hash_desc->hash_bitlen      = HASH_BITLEN_256;
            p_hash_desc->blocksize        = BLOCKSIZE_SHA2_512_256;
            p_hash_desc->p_oid            = &oid_hash[E_SHA2_512];
            break;
        case E_SHA3_256:
            p_hash_desc->p_hash_init      = &l_HASH_Sha256Init;
            p_hash_desc->p_hash_process   = &l_HASH_Sha256Process;
            p_hash_desc->p_hash_terminate = &l_HASH_Sha256Done;
            p_hash_desc->hash_bitlen      = HASH_BITLEN_256;
            p_hash_desc->blocksize        = BLOCKSIZE_SHA3_256;
            p_hash_desc->p_oid            = &oid_hash[E_SHA2_256];
            break;
        case E_SHA3_384:
            p_hash_desc->p_hash_init      = &l_HASH_Sha384Init;
            p_hash_desc->p_hash_process   = &l_HASH_Sha512Process;
            p_hash_desc->p_hash_terminate = &l_HASH_Sha384Done;
            p_hash_desc->hash_bitlen      = HASH_BITLEN_384;
            p_hash_desc->blocksize        = BLOCKSIZE_SHA3_384;
            p_hash_desc->p_oid            = &oid_hash[E_SHA2_384];
            break;
        case E_SHA3_512:
            p_hash_desc->p_hash_init      = &l_HASH_Sha512Init;
            p_hash_desc->p_hash_process   = &l_HASH_Sha512Process;
            p_hash_desc->p_hash_terminate = &l_HASH_Sha512Done;
            p_hash_desc->hash_bitlen      = HASH_BITLEN_512;
            p_hash_desc->blocksize        = BLOCKSIZE_SHA3_512;
            p_hash_desc->p_oid            = &oid_hash[E_SHA2_512];
            break;
        default:
            err = ER_L_HASH_INVALID_ARG;
            break;
    }

    return (err);
}

/*===========================================================================
  Initialize the hash process
  [PUBLIC]
  ===========================================================================*/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0001]:[ICUMFW_UD_L_CRY_HASH_0001]
* Function name: L_HASH_Init()
* Description  : # Register HASH function and perform HASH initialization processing.
* Arguments    : (IN-OUT)  p_hash_ctx    : HASH context
*                (IN)  hash_type         : HASH type
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t L_HASH_Init(hash_ctx_t *p_hash_ctx, hash_type_t hash_type)
{
    uint32_t             err;
    hash_state_t        *p_hash_state;
    hash_descriptor_t   *p_hash_desc;

    e_r_hash_ret_t      ret;
    e_r_hash_type_t     e_hash_type;

    err = l_hash_convert_type(hash_type, &e_hash_type);

    if (ER_OK == err)
    {
        (void)l_HASH_Register(hash_type, &(p_hash_ctx->hash_desc));
        l_hash_convert_type(hash_type, &e_hash_type);
        /* Processing by coprocessor */
        ret = D_HASH_HashBegin((st_r_hash_ctx_t*)&p_hash_ctx->hash_state,
                               e_hash_type,
                               &opt);
        if (HASH_RET_OK == ret)
        {
            err = ER_OK;
        }
        else
        {
            err = ER_L_HASH_INVALID_ARG;
        }
    }
    else
    {

    p_hash_state = &(p_hash_ctx->hash_state);
    p_hash_desc = &(p_hash_ctx->hash_desc);

    err = l_HASH_Register(hash_type, p_hash_desc);

    if (err == ER_OK)
    {
        /* initialize the hash */
        err = (p_hash_ctx->hash_desc.p_hash_init)((hash_inner_state_t *)p_hash_state);
    }
    else
    {
        /* Do nothing */
    }

    }

    /* Initialize the options of D_HASH for the next request */
    l_CMN_MemSet((uint8_t *)&opt, (uint8_t)NUM0, sizeof(st_r_hash_opt_t));

    p_hash_ctx->hash_type = hash_type;
    return(err);
}


/*===========================================================================
  HASH intermediate processing.
  [PUBLIC]
  ===========================================================================*/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0002]:[ICUMFW_UD_L_CRY_HASH_0002]
* Function name: L_HASH_Update()
* Description  : # HASH intermediate processing.
* Arguments    : (IN-OUT)  p_hash_ctx    : HASH context
*                (IN-OUT)  p_data        : message data
*                (IN-OUT)  size_in_bytes : message size(byte)
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t L_HASH_Update(hash_ctx_t *p_hash_ctx, const uint8_t *p_data, uint32_t size_in_bytes)
{

    hash_state_t *p_hash_state;
    uint32_t      err;

    e_r_hash_ret_t      ret;
    e_r_hash_type_t     e_hash_type;

    err = l_hash_convert_type(p_hash_ctx->hash_type, &e_hash_type);

    if (ER_OK == err)
    {
        /* Processing by coprocessor */
        ret = D_HASH_HashUpdate((st_r_hash_ctx_t*)&p_hash_ctx->hash_state,
                                (const uint8_t*)p_data, size_in_bytes);
        if (HASH_RET_OK == ret)
        {
            err = ER_OK;
        }
        else
        {
            err = ER_L_HASH_INVALID_ARG;
        }
    }
    else
    {


    p_hash_state = &(p_hash_ctx->hash_state);
    err = (p_hash_ctx->hash_desc.p_hash_process)((hash_inner_state_t *)p_hash_state, p_data, size_in_bytes);

    }

    return (err);
}


/*===========================================================================
  Finalize the hash process
  [PUBLIC]
  ===========================================================================*/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0003]:[ICUMFW_UD_L_CRY_HASH_0003]
* Function name: L_HASH_Finish()
* Description  : # Perform HASH termination processing.
* Arguments    : (IN-OUT)  p_hash_ctx    : HASH context
*                (IN-OUT)  p_result      : HASH value
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t L_HASH_Finish(hash_ctx_t *p_hash_ctx, uint8_t *p_result)
{

    hash_state_t *p_hash_state;
    uint32_t      err;

    e_r_hash_ret_t      ret;
    e_r_hash_type_t     e_hash_type;

    err = l_hash_convert_type(p_hash_ctx->hash_type, &e_hash_type);

    if (ER_OK == err)
    {
        /* Processing by coprocessor */
        ret = D_HASH_HashEnd((st_r_hash_ctx_t*)&p_hash_ctx->hash_state, p_result);
        if (HASH_RET_OK == ret)
        {
            err = ER_OK;
        }
        else
        {
            err = ER_L_HASH_INVALID_ARG;
        }
    }
    else
    {


    p_hash_state = &(p_hash_ctx->hash_state);
    err = (p_hash_ctx->hash_desc.p_hash_terminate)((hash_inner_state_t *)p_hash_state, p_result);

    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0040]:[ICUMFW_UD_L_CRY_HASH_0040]
* Function name: l_hash_convert_type
* Description  : Convert to driver hash type
* Arguments    : (IN)  hash type
*                (OUT) hash type for HASH driver
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t l_hash_convert_type(hash_type_t hash_type, e_r_hash_type_t *p_hash_type)
{
    uint32_t      err;

    err = ER_OK;
    switch (hash_type)
    {
        case E_SHA2_224:
            *p_hash_type = HASH_TYPE_SHA224;
            break;
        case E_SHA2_256:
            *p_hash_type = HASH_TYPE_SHA256;
            break;
        case E_SHA2_384:
            *p_hash_type = HASH_TYPE_SHA384;
            break;
        case E_SHA2_512:
            *p_hash_type = HASH_TYPE_SHA512;
            break;
        case E_SHA2_512_224:
            *p_hash_type = HASH_TYPE_SHA512_224;
            break;
        case E_SHA2_512_256:
            *p_hash_type = HASH_TYPE_SHA512_256;
            break;
        case E_SHA3_256:
            *p_hash_type = HASH_TYPE_SHA3_256;
            break;
        case E_SHA3_384:
            *p_hash_type = HASH_TYPE_SHA3_384;
            break;
        case E_SHA3_512:
            *p_hash_type = HASH_TYPE_SHA3_512;
            break;
        default:
            err = ER_L_HASH_INVALID_ARG;
            break;
    }

    return (err);
}

uint32_t L_HASH_InitExternal_Sha2_224(hash_ctx_t *p_hash_ctx)
{
    uint32_t             err;

    err = L_HASH_Init(p_hash_ctx, E_SHA2_224);
    if (ER_OK == err)
    {
        err = ER_EXTRERNAL_OK;
    }
    else
    {
        err = ER_EXTRERNAL_ERROR;
    }
    return(err);
}
uint32_t L_HASH_InitExternal_Sha2_256(hash_ctx_t *p_hash_ctx)
{
    uint32_t             err;

    err = L_HASH_Init(p_hash_ctx, E_SHA2_256);
    if (ER_OK == err)
    {
        err = ER_EXTRERNAL_OK;
    }
    else
    {
        err = ER_EXTRERNAL_ERROR;
    }
    return(err);
}
uint32_t L_HASH_InitExternal_Sha2_384(hash_ctx_t *p_hash_ctx)
{
    uint32_t             err;

    err = L_HASH_Init(p_hash_ctx, E_SHA2_384);
    if (ER_OK == err)
    {
        err = ER_EXTRERNAL_OK;
    }
    else
    {
        err = ER_EXTRERNAL_ERROR;
    }
    return(err);
}
uint32_t L_HASH_InitExternal_Sha2_512(hash_ctx_t *p_hash_ctx)
{
    uint32_t             err;

    err = L_HASH_Init(p_hash_ctx, E_SHA2_512);
    if (ER_OK == err)
    {
        err = ER_EXTRERNAL_OK;
    }
    else
    {
        err = ER_EXTRERNAL_ERROR;
    }
    return(err);
}
uint32_t L_HASH_InitExternal_Sha3_256(hash_ctx_t *p_hash_ctx)
{
    uint32_t             err;

    err = L_HASH_Init(p_hash_ctx, E_SHA3_256);
    if (ER_OK == err)
    {
        err = ER_EXTRERNAL_OK;
    }
    else
    {
        err = ER_EXTRERNAL_ERROR;
    }
    return(err);
}
uint32_t L_HASH_InitExternal_Sha3_384(hash_ctx_t *p_hash_ctx)
{
    uint32_t             err;

    err = L_HASH_Init(p_hash_ctx, E_SHA3_384);
    if (ER_OK == err)
    {
        err = ER_EXTRERNAL_OK;
    }
    else
    {
        err = ER_EXTRERNAL_ERROR;
    }
    return(err);
}
uint32_t L_HASH_InitExternal_Sha3_512(hash_ctx_t *p_hash_ctx)
{
    uint32_t             err;

    err = L_HASH_Init(p_hash_ctx, E_SHA3_512);
    if (ER_OK == err)
    {
        err = ER_EXTRERNAL_OK;
    }
    else
    {
        err = ER_EXTRERNAL_ERROR;
    }
    return(err);
}
uint32_t L_HASH_UpdateExternal(hash_ctx_t *p_hash_ctx, const uint8_t *p_data, uint32_t size_in_bytes)
{
    uint32_t             err;

    err = L_HASH_Update(p_hash_ctx, p_data, size_in_bytes);
    if (ER_OK == err)
    {
        err = ER_EXTRERNAL_OK;
    }
    else
    {
        err = ER_EXTRERNAL_ERROR;
    }
    return(err);
}
uint32_t L_HASH_FinishExternal(hash_ctx_t *p_hash_ctx, uint8_t *p_result, uint32_t *p_hash_len)
{
    uint32_t      err;

    err  = L_HASH_Finish(p_hash_ctx, p_result);
    if (ER_OK == err)
    {
        err = ER_EXTRERNAL_OK;
        *p_hash_len = (p_hash_ctx->hash_desc.hash_bitlen / 8u);
    }
    else
    {
        err = ER_EXTRERNAL_ERROR;
    }
    return (err);
}
#if defined(FSW_DVC_HASH)
/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0041]:[ICUMFW_UD_L_CRY_HASH_0041]
* Function name: L_HASH_Init()
* Description  : # HASH with SHA256 Calculated at once (DVC dedicated processing).
* Arguments    : (IN-OUT)  p_key         : key data(UID)
*                (IN-OUT)  p_data        : message data
*                (IN-OUT)  data_size     : message size(byte)
*                (IN-OUT)  p_result      : HASH value
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t L_HASH_CalculateDVC(const uint8_t *p_key, const uint8_t *p_data, uint32_t data_size, uint8_t *p_result)
{
    hash_state_t hash_state;
    uint32_t     err;
    uint8_t      hash_result[HASH256_BYTE_SIZE];

    err = ER_OK;
    l_CMN_MemSet((uint8_t *)&hash_state, (uint8_t)NUM0, sizeof(hash_state_t));

    /* initialize the hash */
    err = l_HASH_Sha256Init((hash_inner_state_t *)&hash_state);
    if (err == ER_OK)
    {
        err = l_HASH_Sha256Process((hash_inner_state_t *)&hash_state, p_key, DVC_LENGTH);
        if (err == ER_OK)
        {
            err = l_HASH_Sha256Process((hash_inner_state_t *)&hash_state, p_data, data_size);
            if (err == ER_OK)
            {
                err = l_HASH_Sha256Done((hash_inner_state_t *)&hash_state, hash_result);
                if (err == ER_OK)
                {
                    l_CMN_MemCpy(p_result, hash_result, DVC_LENGTH);
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }
    return (err);
}
#endif /* defined(FSW_DVC_HASH) */

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0042]:[ICUMFW_UD_L_CRY_HASH_0042]
* Function name: L_HASH_GetInternalHashType
* Description  : Get hash type and size from hash primitive.
* Arguments    : (in) hash_primitive         : HASH Primitive
*                                               Range: HASH_PRIMITIVE_SHA2_224
*                                                      HASH_PRIMITIVE_SHA2_256
*                                                      HASH_PRIMITIVE_SHA2_384
*                                                      HASH_PRIMITIVE_SHA2_512
*                                                      HASH_PRIMITIVE_SHA2_512_224
*                                                      HASH_PRIMITIVE_SHA2_512_256
*                                                      HASH_PRIMITIVE_SHA3_256
*                                                      HASH_PRIMITIVE_SHA3_384
*                                                      HASH_PRIMITIVE_SHA3_512
*                (out) *p_hash_type           : hash type
*                                               Range: E_SHA2_224
*                                                      E_SHA2_256
*                                                      E_SHA2_384
*                                                      E_SHA2_512
*                                                      E_SHA2_512_224
*                                                      E_SHA2_512_256
*                                                      E_SHA3_256
*                                                      E_SHA3_384
*                                                      E_SHA3_512
*                                                      E_LAST_ITEM_HASH_TYPE
*                (out) *p_hash_size_in_bytes  : HASH Size address
*                                               Range: 0 - 0xFFFF(65535)
* Return Value : ER_OK
***************************************************************************************/
uint32_t L_HASH_GetInternalHashType(r_hash_primitive_t hash_primitive, hash_type_t *p_hash_type,
                                    uint16_t *p_hash_size_in_bytes)
{
    switch (hash_primitive)
    {
        case HASH_PRIMITIVE_SHA2_224:
            *p_hash_type = E_SHA2_224;
            *p_hash_size_in_bytes = (uint16_t)HASH_BYTE_SIZE_224;
            break;
        case HASH_PRIMITIVE_SHA2_256:
            *p_hash_type = E_SHA2_256;
            *p_hash_size_in_bytes = (uint16_t)HASH_BYTE_SIZE_256;
            break;
        case HASH_PRIMITIVE_SHA2_384:
            *p_hash_type = E_SHA2_384;
            *p_hash_size_in_bytes = (uint16_t)HASH_BYTE_SIZE_384;
            break;
        case HASH_PRIMITIVE_SHA2_512:
            *p_hash_type = E_SHA2_512;
            *p_hash_size_in_bytes = (uint16_t)HASH_BYTE_SIZE_512;
            break;
        case HASH_PRIMITIVE_SHA2_512_224:
            *p_hash_type = E_SHA2_512_224;
            *p_hash_size_in_bytes = (uint16_t)HASH_BYTE_SIZE_224;
            break;
        case HASH_PRIMITIVE_SHA2_512_256:
            *p_hash_type = E_SHA2_512_256;
            *p_hash_size_in_bytes = (uint16_t)HASH_BYTE_SIZE_256;
            break;
        case HASH_PRIMITIVE_SHA3_256:
            *p_hash_type = E_SHA3_256;
            *p_hash_size_in_bytes = (uint16_t)HASH_BYTE_SIZE_256;
            break;
        case HASH_PRIMITIVE_SHA3_384:
            *p_hash_type = E_SHA3_384;
            *p_hash_size_in_bytes = (uint16_t)HASH_BYTE_SIZE_384;
            break;
        case HASH_PRIMITIVE_SHA3_512:
            *p_hash_type = E_SHA3_512;
            *p_hash_size_in_bytes = (uint16_t)HASH_BYTE_SIZE_512;
            break;
        default:
            *p_hash_type = E_LAST_ITEM_HASH_TYPE;
            *p_hash_size_in_bytes = (uint16_t)0u;
            break;
    }
    return (ER_OK);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0043]:[ICUMFW_UD_L_CRY_HASH_0043]
* Function name: L_HASH_SetOpePrm()
* Description  : # Setting the size of all message data
* Arguments    : (IN-OUT)  p_hash_ctx    : HASH context
*                (IN)      p_prm         : HASH operating parameters
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t L_HASH_SetOpePrm(hash_ctx_t *p_hash_ctx, hash_ope_prm_t* const p_prm)
{
    uint32_t       err;

    e_r_hash_ret_t ret;
    st_r_ope_prm_t ope_prm;

    ret = HASH_RET_OK;
    err = ER_OK;
    l_CMN_MemSet((uint8_t *)&ope_prm, (uint8_t)NUM0, sizeof(st_r_ope_prm_t));

    if (ON == p_prm->use_dma_transfer)
    {
        opt.use_dma_transfer = HASH_USE_DMA_TRANSFER;
    }
    else
    {
        opt.use_dma_transfer = HASH_USE_ICUP_TRANSFER;
    }

    ope_prm.total_msg_size_in_bytes = p_prm->total_msg_size_in_bytes;

    ret = D_HASH_SetOpePrm((st_r_hash_ctx_t*)&p_hash_ctx->hash_state, &ope_prm);

    if (HASH_RET_OK == ret)
    {
        err = ER_OK;
    }
    else
    {
        err = ER_L_HASH_INVALID_ARG;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0044]:[ICUMFW_UD_L_CRY_HASH_0044]
* Function name: L_HASH_ChkDMAApplicable()
* Description  : # Check if DMA transfer is applicable.
* Arguments    : (IN)  hash_type            : HASH type
*                (IN)  p_input_area         : The address of the input area
*                (IN)  input_size_in_bytes  : The size of the input area
*                (IN)  p_output_area        : The address of the output area
*                (IN)  output_size_in_bytes : The size of the output area
* Return Value : OFF : DMA transfer is not possible.
*                ON  : DMA transfer is possible.
****************************************************************************************/
uint32_t L_HASH_ChkDMAApplicable(hash_type_t hash_type, 
                                 const uint8_t *p_input_area,  uint32_t input_size_in_bytes, 
                                       uint8_t *p_output_area, uint32_t output_size_in_bytes)
{
    uint32_t        dma_transfer;

    uint32_t        err;
    e_r_hash_type_t e_hash_type;
    e_r_hash_ret_t  ret;

    dma_transfer = OFF;
    err          = ER_L_HASH_INVALID_ARG;
    e_hash_type  = HASH_TYPE_SHA1;
    ret          = HASH_RET_DMA_NOTAPPLICABLE;

    err = l_hash_convert_type(hash_type, &e_hash_type);
    if (ER_OK == err)
    {
        ret = D_HASH_ChkDMAApplicable(p_input_area, input_size_in_bytes, p_output_area, output_size_in_bytes);
        if (HASH_RET_OK == ret)
        {
            ret = D_HASH_ChkDMABusy(e_hash_type);
            if (HASH_RET_OK == ret)
            {
                dma_transfer = ON;
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

    return (dma_transfer);
}

