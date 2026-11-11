/******************************************************************************/
/* Component Name   Library Crypto ECC Component                              */
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
* Header files
***********************************************************************************/

#include "icum_l_cry_pk_pub.h"
#include "../cry_common/icum_l_my_toolbox.h"
#include "../cry_bignum/icum_l_bignum.h"
#include "icum_l_toolbox_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_remap_pub.h"

/**********************************************************************************
* Local define
***********************************************************************************/

#define MAX_HASH_SIZE_IN_BYTES  (512u / 8u)
/* assuming 8 x maximum hash size */
#define SIZEOF_T_IN_BYTES       (8u * MAX_HASH_SIZE_IN_BYTES)
#define SIZEOF_C_IN_BYTES       (4u)

/* Value definition */
#define NUM0                     (0u)

/**********************************************************************************
* Functions
***********************************************************************************/

static const uint8_t CONST_ZERO[16] =
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
/*===========================================================================
  Process a KDF according to E_KDF1/KDF2/KDF3 algorithm
  E_KDF1/KDF2/KFD3 are defined in ISO 18033-2
  E_KDF3 is defined in NIST SP800-56A
  - p_kdf: pointer to KDF parameters
  - returns 0 if OK
    - in this case p_kdf->p_derived_key points to the derived key
  - retruns a value different from 0 if NG
  [PRIVATE]
  ===========================================================================*/
/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_ECC_0039]:[ICUMFW_UD_L_CRY_ECC_0039]
* Function name: l_ECC_Kdfn
* Description  : Process a KDF according to E_KDF1/KDF2/KDF3 algorithm
*                E_KDF1/KDF2/KFD3 are defined in ISO 18033-2
*                E_KDF3 is defined in NIST SP800-56A
*                - p_kdf: pointer to KDF parameters
*                - returns 0 if OK
*                - in this case p_kdf->p_derived_key points to the derived key
*                - retruns a value different from 0 if NG
* Arguments    : p_kdf  :[in/out]     :pointer to KDF parameters
* Return Value : ER_OK
*                ER_L_ECC_INVALID_ARG
*                ER_L_HASH_INVALID_ARG
***************************************************************************************/
static uint32_t l_ECC_Kdfn(const kdf_t *p_kdf)
{
    hash_ctx_t      hash_ctx;
    uint8_t         T[SIZEOF_T_IN_BYTES];
    uint16_t        idx;
    uint16_t        hlen;
    uint8_t         ceil_cnt;
    uint8_t         counter;
    uint8_t         counter_tab[SIZEOF_C_IN_BYTES];
    uint8_t         count_start;
    uint8_t         count_stop;
    int32_t         i;
    uint32_t        err;
    
    l_CMN_MemSet(T, 0x00, SIZEOF_T_IN_BYTES);

    /* register the hash */
    err = L_HASH_Init(&hash_ctx, p_kdf->hash_type);
    
    /* sanity check */
    if ((ER_OK == err) && (p_kdf->kdf_algo == E_KDF3) && ((uint32_t)p_kdf->count.pAmt < (uint32_t)4u))
    {
        err = ER_L_ECC_INVALID_ARG;
    }
    else
    {
        /* get the size of the hash (in bytes) */
        hlen = (uint16_t)(hash_ctx.hash_desc.hash_bitlen / 8u);

        /* calculate ceil_cnt = ceil_counting(key_len/hash_len) */
        /* calculation method with integers: ceil_cnt = (key_len + hash_len - 1) / hash_len */
        ceil_cnt = (uint8_t)(p_kdf->deriv_key_size_in_bytes + hlen - 1);
        ceil_cnt = (uint8_t)(ceil_cnt / hlen);
        
        /* init processing variables */
        *(uint32_t *)counter_tab = 0;
        count_start = 0;
        count_stop  = ceil_cnt;

        /* tweak on counter start/stop values in E_KDF2 */
        if((E_KDF2 == p_kdf->kdf_algo) ||
            (E_KDF3 == p_kdf->kdf_algo) ||
            (E_CONCAT_KDF == p_kdf->kdf_algo))
        {
            count_start++;
            count_stop++;
        }
        else
        {
            /* Do nothing */
        }
    }
    
    /* core of the KDFn */
    for(idx = 0u, counter = count_start;
        (ER_OK == err) && (counter < count_stop);
        idx += hlen, counter++)
    {
        /* sanity check */
        /* assert(idx <= (SIZEOF_T_IN_BYTES - hlen)); */
        /* counter cannot be above 256 */
        counter_tab[SIZEOF_C_IN_BYTES - 1u] = counter;
        /* hash */
        err = L_HASH_Init(&hash_ctx, p_kdf->hash_type);

        if ((ER_OK == err) &&
            ((p_kdf->kdf_algo == E_KDF3) || (p_kdf->kdf_algo == E_CONCAT_KDF)))
        {
            if (E_KDF3 == p_kdf->kdf_algo)
            {
                for(i = 0; i < (int32_t)(p_kdf->count.pAmt - (uint16_t)4); i++)
                {
                    err = L_HASH_Update(&hash_ctx, CONST_ZERO, 1);
                    if(ER_OK == err)
                    {
                        /* Do nothing */
                    }
                    else
                    {
                        break;
                    }
                    (void)l_PKC_KickCallBackWDT();
                }
            }
            else
            {
                /* Do nothing */
            }
            if (ER_OK == err)
            {
                err = L_HASH_Update(&hash_ctx, counter_tab, SIZEOF_C_IN_BYTES);
            }
            else
            {
                /* Do nothing */
            }
            if ((ER_OK == err) && (p_kdf->secret.shared_secret.size_in_bytes > 0u))
            {
                err = L_HASH_Update(&hash_ctx,
                                    p_kdf->secret.shared_secret.p_data,
                                    p_kdf->secret.shared_secret.size_in_bytes);
            }
            else
            {
                /* Do nothing */
            }
            if ((ER_OK == err) && (p_kdf->addition.info.size_in_bytes > 0u))
            {
                err = L_HASH_Update(&hash_ctx,
                                    p_kdf->addition.info.p_data,
                                    p_kdf->addition.info.size_in_bytes);
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            if ((ER_OK == err) && (p_kdf->secret.shared_secret.size_in_bytes > 0u))
            {
                err = L_HASH_Update(&hash_ctx,
                                    p_kdf->secret.shared_secret.p_data,
                                    p_kdf->secret.shared_secret.size_in_bytes);
            }
            else
            {
                /* Do nothing */
            }
            if (ER_OK == err)
            {
                err = L_HASH_Update(&hash_ctx, counter_tab, SIZEOF_C_IN_BYTES);
            }
            else
            {
                /* Do nothing */
            }
            if ((ER_OK == err) && (p_kdf->addition.info.size_in_bytes > 0u))
            {
                err = L_HASH_Update(&hash_ctx,
                                    p_kdf->addition.info.p_data,
                                    p_kdf->addition.info.size_in_bytes);
            }
            else
            {
                /* Do nothing */
            }
        }

        if(ER_OK == err)
        {
            err = L_HASH_Finish(&hash_ctx, &T[idx]);
        }
        else
        {
            /* Do nothing */
        }
    }

    if (ER_OK == err)
    {
        /* return the derived key */
        l_CMN_MemCpy(p_kdf->p_deriv_key, T, p_kdf->deriv_key_size_in_bytes);
    }
    else
    {
        /* Do nothing */
    }

    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_ECC_0040]:[ICUMFW_UD_L_CRY_ECC_0040]
* Function name: l_ECC_Pbkdf1
* Description  : Process a KDF according to PBKDF1
*                PBKDF1 is defined in PKCS5-v1
*                - p_kdf: pointer to KDF parameters
*                - returns 0 if OK
*                - in this case p_kdf->p_derived_key points to the derived key
*                - retruns a value different from 0 if NG
* Arguments    : p_kdf  :[in/out]     :pointer to KDF parameters   
* Return Value : ER_OK
*                ER_L_ECC_INVALID_ARG
*                ER_L_HASH_INVALID_ARG
***************************************************************************************/
static uint32_t l_ECC_Pbkdf1(const kdf_t *p_kdf)
{
    hash_ctx_t      hash_ctx;
    uint8_t         T[MAX_HASH_SIZE_IN_BYTES];
    uint16_t        hlen;
    int32_t         i;
    uint32_t        err;

    err = L_HASH_Init(&hash_ctx, p_kdf->hash_type);

    if (ER_OK == err)
    {
        /* get the size of the hash (in bytes) */
        hlen = (uint16_t)(hash_ctx.hash_desc.hash_bitlen / 8u);
    }
    else
    {
        /* Do nothing */
    }
    
    /* 20170306 */
    if ((ER_OK == err) && ((uint32_t)p_kdf->deriv_key_size_in_bytes > (uint32_t)hlen))
    {
        /* key too long: stop */
        err = ER_L_ECC_INVALID_ARG;
    }
    else
    {
        /* Do nothing */
    }
    
    if (ER_OK == err)
    {
        /* initial hash */
        err = L_HASH_Init(&hash_ctx, p_kdf->hash_type);
    }
    else
    {
        /* Do nothing */
    }
    
    if ((ER_OK == err) && (p_kdf->secret.pwd.size_in_bytes > 0u))
    {
        err = L_HASH_Update(&hash_ctx, p_kdf->secret.pwd.p_data, p_kdf->secret.pwd.size_in_bytes);
    }
    else
    {
        /* Do nothing */
    }
    
    if ((ER_OK == err) && (p_kdf->addition.salt.size_in_bytes > 0u))
    {
        err = L_HASH_Update(&hash_ctx, p_kdf->addition.salt.p_data, p_kdf->addition.salt.size_in_bytes);
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK == err)
    {
        err = L_HASH_Finish(&hash_ctx, T);
    }
    else
    {
        /* Do nothing */
    }
    
    /* hash loop */
    for (i = 2; (ER_OK == err) && (i <= (int32_t)p_kdf->count.counter); i++)
    {
        err = L_HASH_Init(&hash_ctx, p_kdf->hash_type);
        if (ER_OK == err)
        {
            err = L_HASH_Update(&hash_ctx, T, (uint32_t)hlen);
        }
        else
        {
            /* Do nothing */
        }
        if (ER_OK == err)
        {
            err = L_HASH_Finish(&hash_ctx, T);
        }
        else
        {
            /* Do nothing */
        }
        (void)l_PKC_KickCallBackWDT();
    }

    if (ER_OK == err)
    {
        /* return the derived key */
        l_CMN_MemCpy(p_kdf->p_deriv_key, T, (uint32_t)p_kdf->deriv_key_size_in_bytes);
    }
    else
    {
        /* Do nothing */
    }

    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_ECC_0041]:[ICUMFW_UD_L_CRY_ECC_0041]
* Function name: l_ECC_Hmac
* Description  : Initialization processing of HASH generation
* Arguments    : p_hash_ctx             :[in]   :hash context
*                hash_type              :[in]   :hash type
*                p_key                  :[in]   :Pointer to secret key
*                key_size_in_bytes      :[out]  :Secret key size
*                p_text                 :[in]   :Pointer size
*                text_size_in_bytes     :[in]   :Message size
*                p_hmac                 :[out]  :Pointer to MAC value to output
                 p_hmac_size_in_bytes   :[out]  :Pointer to MAC size to output
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
***************************************************************************************/
static uint32_t l_ECC_Hmac(  hash_ctx_t *p_hash_ctx,
                             hash_type_t hash_type,
                             const uint8_t *p_key,
                             uint32_t key_size_in_bytes,
                             const uint8_t *p_text,
                             uint32_t text_size_in_bytes,
                             uint8_t *p_hmac,
                             uint32_t *p_hmac_size_in_bytes)
{
    uint32_t o_key_pad[]={0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,
                          0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,
                          0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,
                          0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c,0x5c5c5c5c};
    uint32_t i_key_pad[]={0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,
                          0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,
                          0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,
                          0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,0x36363636,0x36363636};
    
    /* hash_state_t        hash_state; */
    uint16_t    block_size;
    uint32_t    err;
    
    /* init the hash */
    /* err = p_hash_desc->p_hash_init(&hash_state); */
    err = L_HASH_Init(p_hash_ctx, hash_type);
    if(err != ER_OK)
    {
        /* Do nothing */
    }
    else
    {
        /* calculate the expected hmac size */
        *p_hmac_size_in_bytes = p_hash_ctx->hash_desc.hash_bitlen / 8u;

        switch(p_hash_ctx->hash_desc.hash_bitlen)
        {
            case 384u:
            case 512u:
                /* 1024 bits */
                block_size = 128u;
                break;
            default:
                /* 512 bits */
                block_size = 64u;
                break;
        }

        /* sanity check */
        /* note: in the hmac algorithm, the key is shortened with an addition hash operation not done here */
        /* assert(key_size_in_bytes <= block_size); */

        /* init o_key_pad */
        l_CMN_MemXor((uint8_t *)o_key_pad, p_key, (uint16_t)key_size_in_bytes);

        /* init i_key_pad */
        l_CMN_MemXor((uint8_t *)i_key_pad, p_key, (uint16_t)key_size_in_bytes);

        /* first hash on i_key_pad || message */
        err = L_HASH_Init(p_hash_ctx, hash_type);
        if(err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            err = L_HASH_Update(p_hash_ctx, (const uint8_t *)i_key_pad, block_size);
        }

        if(err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            err = L_HASH_Update(p_hash_ctx, p_text, text_size_in_bytes);
        }

        if(err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            err = L_HASH_Finish(p_hash_ctx, p_hmac);
        }

        if(err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            /* second hash on o_key_pad || prev. hash */
            err = L_HASH_Init(p_hash_ctx, hash_type);
        }

        if(err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            err = L_HASH_Update(p_hash_ctx, (const uint8_t *)o_key_pad, block_size);
        }

        if(err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            err = L_HASH_Update(p_hash_ctx, p_hmac, *p_hmac_size_in_bytes);
        }

        if(err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            err = L_HASH_Finish(p_hash_ctx, p_hmac);
        }

        if(err != ER_OK)
        {
            *p_hmac_size_in_bytes = 0;
        }
        else
        {
            /* Do nothing */
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_ECC_0042]:[ICUMFW_UD_L_CRY_ECC_0042]
* Function name: l_ECC_Pbkdf2
* Description  : Process a KDF according to PBKDF12
*                PBKDF2 is defined in PKCS5-v2
*                - p_kdf: pointer to KDF parameters
*                - returns 0 if OK
*                - in this case p_kdf->p_derived_key points to the derived key
*                - retruns a value different from 0 if NG
* Arguments    : p_kdf  :[in/out]     :pointer to KDF parameters
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
***************************************************************************************/
static uint32_t l_ECC_Pbkdf2(const kdf_t *p_kdf)
{
    hash_ctx_t  hash_ctx;
    uint8_t     T[SIZEOF_T_IN_BYTES];
    uint8_t     U[2][MAX_HASH_SIZE_IN_BYTES];
    uint8_t     hlen;
    uint8_t     ceil_cnt;
    uint16_t    idx;
    uint8_t     counter;
    uint8_t     counter_tab[SIZEOF_C_IN_BYTES];
    int32_t     i;
    int32_t     n;
    uint32_t    p = 0;
    uint32_t    err;
    
    /* register the hash */
    /* err = l_HASH_Register(p_kdf->hash_type, &hash_desc); */
    err = L_HASH_Init(&hash_ctx, p_kdf->hash_type);
    if(err != ER_OK)
    {
        /* error: stop here */
    }
    else
    {
        l_CMN_MemSet(counter_tab, 0x00, SIZEOF_C_IN_BYTES);
        l_CMN_MemSet(T, 0x00, SIZEOF_T_IN_BYTES);

        /* get the size of the hash (in bytes) */
        hlen = (uint8_t)(hash_ctx.hash_desc.hash_bitlen / 8u);

        /* calculate ceil_cnt = ceiling(key_len/hash_len) */
        /* calculation method with integers: ceil_cnt = (key_len + hash_len - 1) / hash_len */
        ceil_cnt = p_kdf->deriv_key_size_in_bytes + hlen - 1;
        ceil_cnt = (uint8_t)(ceil_cnt / hlen);

        /* sanity check */
        /* assert(ceil_cnt <= 255); */

        n = p_kdf->addition.salt.size_in_bytes + SIZEOF_C_IN_BYTES;

        for(idx = 0, counter = 1; counter <= ceil_cnt; counter++)
        {
            /* counter cannot be above 256 */
            counter_tab[SIZEOF_C_IN_BYTES - 1u] = counter;

            /* initial HMAC */
            l_CMN_MemCpy(&T[idx], p_kdf->addition.salt.p_data, p_kdf->addition.salt.size_in_bytes);
            l_CMN_MemCpy(&T[idx + p_kdf->addition.salt.size_in_bytes], counter_tab, SIZEOF_C_IN_BYTES);
            (void)l_ECC_Hmac(&hash_ctx, p_kdf->hash_type, 
                            p_kdf->secret.pwd.p_data, p_kdf->secret.pwd.size_in_bytes,
                            &T[idx], (uint32_t)n, U[0],
                            &p);
            l_CMN_MemCpy(&T[idx], (const uint8_t *)U[0], p);

            /* hmac loop */
            for(i = 0; i < (int32_t)p_kdf->count.counter - 1; i++)
            {
                uint8_t u_idx;
                u_idx = (uint8_t)((uint32_t)i & 1u);
                (void)l_ECC_Hmac(&hash_ctx, p_kdf->hash_type,
                                p_kdf->secret.pwd.p_data, p_kdf->secret.pwd.size_in_bytes,
                                U[u_idx], (uint32_t)p, U[ u_idx^1u ],
                                &p);
                l_CMN_MemXor(&T[idx], (const uint8_t *)U[ u_idx^1u ], (uint16_t)p);
                (void)l_PKC_KickCallBackWDT();
            }

            idx += (uint16_t)p;
        }

        /* return the derived key */
        l_CMN_MemCpy(p_kdf->p_deriv_key, T, p_kdf->deriv_key_size_in_bytes);
    }

    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_ECC_0043]:[ICUMFW_UD_L_CRY_ECC_0043]
* Function name: L_ECC_Kdf
* Description  : Process a KDF according to one of the selected algorithms
*                - returns 0 if OK
*                - in this case p_kdf->p_derived_key points to the derived key
*                - retruns a value different from 0 if NG
* Arguments    : p_kdf  :[in/out]     :pointer to KDF parameters
* Return Value : ER_OK
*                ER_L_ECC_INVALID_ARG
***************************************************************************************/
uint32_t L_ECC_Kdf(kdf_t *p_kdf)
{
    uint32_t err;
    
    switch(p_kdf->kdf_algo)
    {
        case E_KDF1:
        case E_KDF2:
        case E_KDF3:
        case E_CONCAT_KDF:
            err = l_ECC_Kdfn(p_kdf);
            break;
        case E_PKDF1:
            err = l_ECC_Pbkdf1(p_kdf);
            break;
        case E_PKDF2:
            err = l_ECC_Pbkdf2(p_kdf);
            break;
        default:
            err = ER_L_ECC_INVALID_ARG;
            break;
    }
    
    if(err == ER_L_HASH_INVALID_ARG)
    {
        err = ER_L_ECC_INVALID_ARG;
    }
    else
    {
        /* Do nothing */
    }
    
    return(err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_ECC_0061]:[ICUMFW_UD_L_CRY_ECC_0061]
* Function name: CISM_DerivKeyValue()
* Description  : L_ECC_Kdf() wrapper API
* Arguments    : p_kdf
* Return Value : SERV_OK
*              : SERV_PRM_ERROR
***************************************************************************************/
PRAGMA(ghs reference CISM_DerivKeyValue)
r_icumif_service_result_t CISM_DerivKeyValue(r_kdf_t *p_kdf)
{
    uint32_t                    err;
    r_icumif_service_result_t   rtn;
    kdf_t                       kdf_prm;
    kdf_type_t                  kdf_type;
    hash_type_t                 hash_type;
    uint16_t                    hash_size;
    r_kdf_t                     *p_kdf_logical;
    uint8_t                     *p_secret_key_value_logical;
    uint8_t                     *p_info_logical;
    uint8_t                     *p_derived_key_value_logical;

    err = ER_OK;

    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_kdf, (uint32_t *)&p_kdf_logical);
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_kdf_logical->p_secret_key_value,
                                  (uint32_t *)&p_secret_key_value_logical);
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_kdf_logical->p_info, (uint32_t *)&p_info_logical);
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_kdf_logical->p_deriv_key_value,
                                  (uint32_t *)&p_derived_key_value_logical);

    L_Toolbox_MemSet((uint8_t *)&kdf_prm, 0x00, sizeof(kdf_t));

    /* param check */
    (void)L_ECC_ToInternalKdfType(p_kdf_logical->kdf_algo, &kdf_type);
    if (E_LAST_ITEM_KDF_TYPE == kdf_type)
    {
        err = ER_PARAM;
    }
    else
    {
        (void)L_HASH_GetInternalHashType(p_kdf_logical->hash_primitive, &hash_type, &hash_size);
        if (NUM0 == (uint32_t)hash_size)
        {
            err = ER_PARAM;
        }
        else
        {
            /* Do nothing */
        }
    }
    /* end of else */

    /* KDF */
    if (ER_OK == err)
    {
        kdf_prm.kdf_algo                           = kdf_type;
        kdf_prm.hash_type                          = hash_type;
        kdf_prm.secret.shared_secret.p_data        = p_secret_key_value_logical;
        kdf_prm.secret.shared_secret.size_in_bytes = p_kdf_logical->secret_key_size_in_bytes;
        kdf_prm.addition.info.p_data               = p_info_logical;
        kdf_prm.addition.info.size_in_bytes        = p_kdf_logical->info_size_in_bytes;
        kdf_prm.count.counter                      = p_kdf_logical->counter;
        kdf_prm.p_deriv_key                        = p_derived_key_value_logical;
        kdf_prm.deriv_key_size_in_bytes            = p_kdf_logical->deriv_key_size_in_bytes;

        err = L_ECC_Kdf(&kdf_prm);
    }
    /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    /* return value */
    if (ER_OK == err)
    {
        rtn = SERV_OK;
    }
    else if ((ER_PARAM == err) || (ER_L_ECC_INVALID_ARG == err))
    {
        rtn = SERV_PRM_ERROR;
    }
    else
    {
        rtn = SERV_SYS_ERROR;
    }

    return rtn;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_ECC_0062]:[ICUMFW_UD_L_CRY_ECC_0062]
* Function name: L_ECC_ToInternalKdfType
* Description  : Set the value to kdf_type.
* Arguments    : kdf_algo   :[in]
*                kdf_type   :[out]
* Return Value : ER_OK
****************************************************************************************/
uint32_t L_ECC_ToInternalKdfType(r_kdf_algo_t kdf_algo, kdf_type_t *kdf_type)
{

    switch (kdf_algo)
    {
        case KDF_ALGO_KDF1:
            *kdf_type = E_KDF1;
            break;
        case KDF_ALGO_KDF2:
            *kdf_type = E_KDF2;
            break;
        case KDF_ALGO_KDF3:
            *kdf_type = E_KDF3;
            break;
        case KDF_ALGO_PKDF1:
            *kdf_type = E_PKDF1;
            break;
        case KDF_ALGO_PKDF2:
            *kdf_type = E_PKDF2;
            break;
        default:
            *kdf_type = E_LAST_ITEM_KDF_TYPE;
            break;
    }
    return (ER_OK);
}

