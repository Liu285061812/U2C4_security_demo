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

#include "icum_l_hash.h"
#include "icum_l_my_toolbox.h"

#include "r_icum_hash_api.h"

/* #define HMAC_BLOCKSIZE hash_descriptor[hash].blocksize */
#define HMAC_BLOCKSIZE p_hash_ctx->hash_desc.blocksize
#define SHA224_LEN              (28U)
#define SHA256_LEN              (32U)

#define NUM3                (3u)

/** 
  Hash a block of memory and store the digest. 
  \param hash           The index of the hash you wish to use 
  \param p_data         The data you wish to hash 
  \param size_in_bytes  The length of the data to hash (octets) 
  \param p_hash         [out] Where to store the digest 
  \param p_outlen       [in/out] Max size and resulting size of the digest 
  \return CRYPT_OK if successful 
*/ 

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0039]:[ICUMFW_UD_L_CRY_HASH_0039]
* Function name: l_HMAC_Determinekey()
* Description  : # Secret key confirmation processing when the size of
*                  the specified secret key is larger than the block size
*                  determined for each HASH type.
* Arguments    : (IN-OUT)  p_hash_desc      : HASH descriptor
*                (IN-OUT)  p_data           : Input message
*                (IN-OUT)  size_in_bytes    : Input message length
*                (IN-OUT)  p_hash           : HASH value (secret key) output destination
*                (IN-OUT)  p_outlen         : HASH length
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
*                ER_L_HMAC_BUFFER_OVERFLOW
****************************************************************************************/
static uint32_t l_HMAC_Determinekey(hash_descriptor_t *p_hash_desc,
                              const uint8_t *p_data,
                              uint32_t size_in_bytes,
                              uint8_t *p_hash,
                              uint32_t *p_outlen)
{ 
    hash_inner_state_t  md; 
    uint32_t            err; 

    if (p_data == NULL)
    {
        err = ER_L_HMAC_INVALID_ARG;
    }
    else if (p_hash == NULL)
    {
        err = ER_L_HMAC_INVALID_ARG;
    }
    else if (p_outlen == NULL)
    {
        err = ER_L_HMAC_INVALID_ARG;
    }
    else if (*p_outlen < (p_hash_desc->hash_bitlen >> 3))
    {
        *p_outlen = p_hash_desc->hash_bitlen >> 3;
        err = ER_L_HMAC_BUFFER_OVERFLOW; 
    }
    else
    {
        err = p_hash_desc->p_hash_init(&md);
        if (err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            err = p_hash_desc->p_hash_process(&md, p_data, size_in_bytes);
            if (err != ER_OK)
            {
                /* Do nothing */
            }
            else
            {
                err = p_hash_desc->p_hash_terminate(&md, p_hash); 
                *p_outlen = p_hash_desc->hash_bitlen >> 3;
            }
        }
    }

    return (err); 
}


/** 
   Initialize an HMAC context. 
   \param hmac     The HMAC state  
   \param hash     The index of the hash you want to use  
   \param key      The secret key 
   \param keylen   The length of the secret key (octets) 
   \return CRYPT_OK if successful 
*/ 

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0004]:[ICUMFW_UD_L_CRY_HASH_0004]
* Function name: L_HMAC_Init()
* Description  : # Perform initialization of HMAC.
* Arguments    : (IN-OUT)  hash_ctx : HASH context
*                (IN-OUT)  key      : secret key
*                (IN-OUT)  keylen   : key length
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
*                ER_L_HMAC_INVALID_KEYSIZE
****************************************************************************************/
uint32_t L_HMAC_Init(hash_ctx_t *p_hash_ctx, const uint8_t *p_key, uint32_t keylen)
{
    uint8_t            buf[128];
    uint32_t           hashsize;
    uint32_t           keylen_conf;
    uint32_t           au_loop;
    uint32_t           hash_byte_size;
    uint32_t           err;
    hmac_state_t      *p_hmac_state;
    hash_descriptor_t *p_hash_desc;
    hash_state_t      *p_hash_state;

    if (p_hash_ctx == NULL)
    {
        err = ER_L_HMAC_INVALID_ARG;
    }
    else if (p_key == NULL)
    {
        err = ER_L_HMAC_INVALID_ARG;
    }
    /* valid key length? */ 
    else if (keylen == 0u)
    {
        err = ER_L_HMAC_INVALID_KEYSIZE;
    }
    else
    {
        p_hash_ctx->hash_type = p_hash_ctx->hmac_state.hash_type;
        p_hmac_state = &(p_hash_ctx->hmac_state);
        p_hash_desc  = &(p_hash_ctx->hash_desc);
 
        err = l_HASH_Register(p_hmac_state->hash_type, p_hash_desc);

        e_r_hash_ret_t      ret;
        st_r_hmac_opt_t     opt = {0};
        e_r_hash_type_t     e_hash_type;

        if ((E_SHA2_224     == p_hash_ctx->hmac_state.hash_type) ||
            (E_SHA2_256     == p_hash_ctx->hmac_state.hash_type) ||
            (E_SHA2_384     == p_hash_ctx->hmac_state.hash_type) ||
            (E_SHA2_512     == p_hash_ctx->hmac_state.hash_type) ||
            (E_SHA2_512_224 == p_hash_ctx->hmac_state.hash_type) ||
            (E_SHA2_512_256 == p_hash_ctx->hmac_state.hash_type))
        {
            if (ER_OK == err)
            {
                /* Processing by coprocessor */
                l_hash_convert_type(p_hash_ctx->hmac_state.hash_type, &e_hash_type);
                ret = D_HASH_HMACBegin((st_r_hmac_ctx_t*)&p_hash_ctx->hash_state,
                                       e_hash_type,
                                       &opt,
                                       p_key,
                                       keylen);
                if (HASH_RET_OK == ret)
                {
                    err = ER_OK;
                }
                else
                {
                    err = ER_L_HASH_INVALID_ARG;
                }
            } /* end of if (ER_OK == err) */
            else
            {
                /* Do nothing */
            }
        }
        else
        {

        p_hmac_state = &(p_hash_ctx->hmac_state);
        p_hash_desc  = &(p_hash_ctx->hash_desc);
        p_hash_state = &(p_hash_ctx->hash_state);

        err = l_HASH_Register(p_hmac_state->hash_type, p_hash_desc);
        if (err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            hashsize = p_hash_desc->hash_bitlen >> 3;

            l_CMN_MemSet((uint8_t *)&p_hmac_state->key, 0x00, sizeof(p_hmac_state->key));

            /* (1) make sure we have a large enough key */ 
            if (keylen > HMAC_BLOCKSIZE)
            { 
                hash_byte_size = HMAC_BLOCKSIZE; 
                err = l_HMAC_Determinekey(p_hash_desc, p_key, keylen, p_hmac_state->key, &hash_byte_size);
                if (err != ER_OK)
                {
                    /* Do nothing */
                }
                else
                {
                    keylen_conf = hashsize;
                }
            }
            else
            {
                keylen_conf = keylen; 
                l_CMN_MemCpy(p_hmac_state->key, p_key, (uint32_t)keylen_conf);
            }

            if (err != ER_OK)
            {
                /* Do nothing */
            }
            else
            {
                if (keylen_conf < HMAC_BLOCKSIZE)
                {
                    l_CMN_MemSet(((p_hmac_state->key) + keylen_conf), (uint8_t)'\0', (uint32_t)(HMAC_BLOCKSIZE - keylen_conf));
                }
                else
                {
                    /* Do nothing */
                }

                /* Create the initial vector for step (3) */ 
                l_CMN_MemSet(buf, (uint8_t)'\0', sizeof(buf));
                for (au_loop = 0; au_loop < HMAC_BLOCKSIZE; au_loop++)
                {
                    buf[au_loop] = p_hmac_state->key[au_loop] ^ (uint8_t)0x36;
                }

                /* Pre-pend that to the hash data */ 
                err = p_hash_desc->p_hash_init((hash_inner_state_t *)p_hash_state);
                if (err != ER_OK)
                {
                    /* Do nothing */
                }
                else
                {
                    err = p_hash_desc->p_hash_process((hash_inner_state_t *)p_hash_state, buf, HMAC_BLOCKSIZE);

                }
            }
        }

        }
    }
    return (err);
}


/**  
  Process data through HMAC 
  \param hmac            The hmac state 
  \param p_data          The data to send through HMAC 
  \param size_in_bytes   The length of the data to HMAC (octets) 
  \return CRYPT_OK if successful 
*/ 

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0005]:[ICUMFW_UD_L_CRY_HASH_0005]
* Function name: L_HMAC_Update()
* Description  : # Intermediate processing of HMAC. Make the message HASH.
* Arguments    : (IN-OUT)  hash_ctx      : HASH context
*                (IN-OUT)  p_data        : Input message
*                (IN-OUT)  size_in_bytes : Input message length
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t L_HMAC_Update(hash_ctx_t *p_hash_ctx, const uint8_t *p_data, uint32_t size_in_bytes)
{
    hash_descriptor_t *p_hash_desc;
    hash_state_t      *p_hash_state;
    uint32_t          err;

    if (p_data == NULL)
    {
        err = ER_L_HMAC_INVALID_ARG;
    }
    else
    {

        e_r_hash_ret_t      ret;

        if ((E_SHA2_224     == p_hash_ctx->hash_type) ||
            (E_SHA2_256     == p_hash_ctx->hash_type) ||
            (E_SHA2_384     == p_hash_ctx->hash_type) ||
            (E_SHA2_512     == p_hash_ctx->hash_type) ||
            (E_SHA2_512_224 == p_hash_ctx->hash_type) ||
            (E_SHA2_512_256 == p_hash_ctx->hash_type))
        {
            /* Processing by coprocessor */
            ret = D_HASH_HMACUpdate((st_r_hmac_ctx_t*)&p_hash_ctx->hash_state,
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
        p_hash_desc  = &(p_hash_ctx->hash_desc);
        err = p_hash_desc->p_hash_process((hash_inner_state_t *)p_hash_state, p_data, size_in_bytes);

        }
    }

    return (err);
}


/** 
   Terminate an HMAC session 
   \param hash_ctx    HASH context
   \param p_hash      [out] The destination of the HMAC authentication tag 
   \param p_outlen    [in/out]  The max size and resulting size of the HMAC authentication tag 
   \return CRYPT_OK if successful 
   hmac->keylen <= 2048
*/ 

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0006]:[ICUMFW_UD_L_CRY_HASH_0006]
* Function name: L_HMAC_Finish()
* Description  : # Intermediate processing of HMAC. Make the message HASH.
* Arguments    : (IN-OUT)  hash_ctx : HASH context
*                (IN-OUT)  out      : HMAC value output destination
*                (IN-OUT)  outlen   : HMAC length
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t L_HMAC_Finish(hash_ctx_t *p_hash_ctx, uint8_t *p_hmac, uint32_t *p_outlen) 
{ 
    uint8_t            buf[128], isha[64];
    uint32_t           hashsize, i;
    uint32_t           err; 
    hmac_state_t      *p_hmac_state;
    hash_descriptor_t *p_hash_desc;
    hash_state_t      *p_hash_state;

    if (p_hash_ctx == NULL)
    {
        err = ER_L_HMAC_INVALID_ARG;
    }
    else if (p_hmac == NULL)
    {
        err = ER_L_HMAC_INVALID_ARG;
    }
    else if (p_outlen == NULL)
    {
        err = ER_L_HMAC_INVALID_ARG;
    }
    else
    {
        p_hash_desc  = &(p_hash_ctx->hash_desc);
        
        e_r_hash_ret_t      ret;

        if ((E_SHA2_224     == p_hash_ctx->hash_type) ||
            (E_SHA2_256     == p_hash_ctx->hash_type) ||
            (E_SHA2_384     == p_hash_ctx->hash_type) ||
            (E_SHA2_512     == p_hash_ctx->hash_type) ||
            (E_SHA2_512_224 == p_hash_ctx->hash_type) ||
            (E_SHA2_512_256 == p_hash_ctx->hash_type))
        {
            /* Processing by coprocessor */
            ret = D_HASH_HMACEnd((st_r_hmac_ctx_t*)&p_hash_ctx->hash_state, p_hmac);
            if (HASH_RET_OK == ret)
            {
                *p_outlen = p_hash_desc->hash_bitlen >> NUM3;
                err = ER_OK;
            }
            else
            {
                err = ER_L_HASH_INVALID_ARG;
            }
        }
        else
        {

        p_hmac_state = &(p_hash_ctx->hmac_state);
        p_hash_desc  = &(p_hash_ctx->hash_desc);
        p_hash_state = &(p_hash_ctx->hash_state);

        /* get the hash message digest size */
        hashsize = p_hash_desc->hash_bitlen >> 3;


        /* Get the hash of the first HMAC vector plus the data */
        l_CMN_MemSet(isha, (uint8_t)'\0', sizeof(isha));
        err = p_hash_desc->p_hash_terminate((hash_inner_state_t *)p_hash_state, isha);

        if (err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            /* Create the second HMAC vector vector for step (3) */
            l_CMN_MemSet(buf, (uint8_t)'\0', sizeof(buf));
            for (i=0; i < HMAC_BLOCKSIZE; i++)
            {
                buf[i] = p_hmac_state->key[i] ^ (uint8_t)0x5C;
            }

            /* Now calculate the "outer" hash for step (5), (6), and (7) */
            err = p_hash_desc->p_hash_init((hash_inner_state_t *)p_hash_state);
            if (err != ER_OK)
            {
                /* Do nothing */
            }
            else
            {
                err = p_hash_desc->p_hash_process((hash_inner_state_t *)p_hash_state, buf, HMAC_BLOCKSIZE);
            }
            if (err != ER_OK)
            {
                /* Do nothing */
            }
            else
            {
                err = p_hash_desc->p_hash_process((hash_inner_state_t *)p_hash_state, isha, hashsize);
            }
            if (err != ER_OK)
            {
                /* Do nothing */
            }
            else
            {
                err = p_hash_desc->p_hash_terminate((hash_inner_state_t *)p_hash_state, buf);
            }
            if (err != ER_OK)
            {
                /* Do nothing */
            }
            else
            {
                /* copy to output  */
                for (i = 0; ((i < hashsize) && (i < *p_outlen)); i++)
                {
                    p_hmac[i] = buf[i];
                }
                *p_outlen = i;

                err = ER_OK;
            }
        }

        }
    }

    return (err);
}
