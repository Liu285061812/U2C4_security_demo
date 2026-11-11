/******************************************************************************/
/* Component Name   Cryptographic Library PKC Common                          */
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
* Include
***********************************************************************************/
#include "r_icumif_api.h"
#include "icum_d_wdt_pub.h"
#include "icum_l_cry_pk_pub.h"
#include "r_pkcl_cmn.h"
#include "icum_l_my_toolbox.h"
#include "icum_l_pkc_cmn.h"
#include "r_pkcd_dev_api.h"

/**********************************************************************************
* Define
***********************************************************************************/
#define NB_RSA_CODE                 (24u)
#define NB_ECC_CODE                 (26u)
#define NB_CURVE_TBL                (17u)

/**********************************************************************************
* Structure
***********************************************************************************/
typedef struct pkc_ret_cnv
{
    uint32_t    pkc_code;
    uint32_t    lib_code;
} pkc_ret_cnv_t;

typedef struct pkc_curve_table
{
    r_ecc_curve_t            curve_id;
    const st_pkc_ec_curve_t  *p_curve_table;
} pkc_curve_table_t;

/**********************************************************************************
* Variable
***********************************************************************************/
/* RSA errors table that convert from PKC library to crypt library */
static const pkc_ret_cnv_t rsa_code_tuples[NB_RSA_CODE] =
{
    { PKCL_RET_OK,                      ER_OK },
    { PKCL_RET_PROCESSING,              ER_IN_PROCESS },
    { PKCL_RET_PRIME_GENERATING,        ER_IN_PROCESS },
    { PKCL_RET_INTERNALERR,             ER_L_RSA_ERROR },
    { PKCL_RET_STATUSERR,               ER_L_RSA_ERROR },
    { PKCL_RET_PARAMERR,                ER_L_RSA_INVALID_ARG },
    { PKCL_RET_LENGTHERR,               ER_L_RSA_LENGTHERR },
    { PKCL_RET_HASHAPIERR,              ER_L_HASH_INVALID_ARG },
    { PKCL_RET_DRBGAPIERR,              ER_L_RSA_ERROR },
    { PKCL_RET_KDFAPIERR,               ER_L_RSA_INVALID_ARG },
    { PKCL_RET_TAMPERERR,               ER_L_RSA_SYS_ERROR },
    { PKCL_RET_DRV_PARAMERR,            ER_L_RSA_INVALID_ARG },
    { PKCL_RET_DRV_LENGTHERR,           ER_L_RSA_LENGTHERR },
    { PKCL_RET_DRV_INTERNALERR,         ER_L_RSA_ERROR },
    { PKCL_RET_PKCCTIMEOUTERR,          ER_L_RSA_ERROR },
    { PKCL_RET_PKCCERR,                 ER_L_RSA_ERROR },
    { PKCL_RET_KEY_PARAMERR,            ER_L_RSA_INVALID_ARG },
    { PKCL_RET_KEY_LENGTHERR,           ER_L_RSA_INVALID_ARG },
    { PKCL_RET_DRV_FAIL,                ER_L_RSA_ERROR },
    { PKCL_RET_DRV_TAMPERERR,           ER_L_RSA_SYS_ERROR },
    { PKCL_RET_RSA_VERIFAIL,            ER_L_RSA_VERIFY_FAILED },
    { PKCL_RET_RSA_SCHEMEERR,           ER_L_RSA_INVALID_ARG },
    { PKCL_RET_RSA_NON_COPRIME,         ER_L_RSA_KEYGEN_RETRYOVER },
    { PKCL_RET_RSA_COMPOSITE,           ER_L_RSA_KEYGEN_RETRYOVER },
};

/* ECC errors table that convert from PKC library to crypt library */
static const pkc_ret_cnv_t ecc_code_tuples[NB_ECC_CODE] =
{
    { PKCL_RET_OK,                      ER_OK },
    { PKCL_RET_PROCESSING,              ER_IN_PROCESS },
    { PKCL_RET_PRIME_GENERATING,        ER_IN_PROCESS },
    { PKCL_RET_INTERNALERR,             ER_L_ECC_ERROR },
    { PKCL_RET_STATUSERR,               ER_L_ECC_ERROR },
    { PKCL_RET_PARAMERR,                ER_L_ECC_INVALID_ARG },
    { PKCL_RET_LENGTHERR,               ER_L_ECC_LENGTHERR },
    { PKCL_RET_HASHAPIERR,              ER_L_HASH_INVALID_ARG },
    { PKCL_RET_DRBGAPIERR,              ER_L_ECC_ERROR },
    { PKCL_RET_KDFAPIERR,               ER_L_ECC_INVALID_ARG },
    { PKCL_RET_TAMPERERR,               ER_L_ECC_SYS_ERROR },
    { PKCL_RET_DRV_PARAMERR,            ER_L_ECC_INVALID_ARG },
    { PKCL_RET_DRV_LENGTHERR,           ER_L_ECC_LENGTHERR },
    { PKCL_RET_DRV_INTERNALERR,         ER_L_ECC_ERROR },
    { PKCL_RET_PKCCTIMEOUTERR,          ER_L_ECC_ERROR },
    { PKCL_RET_PKCCERR,                 ER_L_ECC_ERROR },
    { PKCL_RET_KEY_PARAMERR,            ER_L_ECC_INVALID_ARG },
    { PKCL_RET_KEY_LENGTHERR,           ER_L_ECC_INVALID_ARG },
    { PKCL_RET_DRV_FAIL,                ER_L_ECC_ERROR },
    { PKCL_RET_DRV_TAMPERERR,           ER_L_ECC_SYS_ERROR },
    { PKCL_RET_DH_INVALID_SHAREDSECRET, ER_L_ECC_ERROR },
    { PKCL_RET_EC_VERIFAIL,             ER_L_ECC_VERIFY_FAILED },
    { PKCL_RET_EC_ZERO_SIGNATURE,       ER_L_ECC_ERROR },
    { PKCL_RET_EDDSA_ZERO_SIGNATURE,    ER_L_ECC_ERROR },
    { PKCL_RET_EDDSA_VERIFAIL,          ER_L_ECC_VERIFY_FAILED },
    { PKCL_RET_EDDSA_DECOMPERR,         ER_L_ECC_ERROR }
};

/* ECC Curve table */
static const pkc_curve_table_t curve_table[NB_CURVE_TBL] = 
{
    { ECC_CURVE_NIST_P192,         &g_pkcl_p192param       },
    { ECC_CURVE_NIST_P224,         &g_pkcl_p224param       },
    { ECC_CURVE_NIST_P256,         &g_pkcl_p256param       },
    { ECC_CURVE_NIST_P384,         &g_pkcl_p384param       },
    { ECC_CURVE_NIST_P521,         &g_pkcl_p521param       },
    { ECC_CURVE_BRAINPOOL_P256,    &g_pkcl_bp256r1param    },
    { ECC_CURVE_BRAINPOOL_P320,    &g_pkcl_bp320r1param    },
    { ECC_CURVE_BRAINPOOL_P384,    &g_pkcl_bp384r1param    },
    { ECC_CURVE_BRAINPOOL_P512,    &g_pkcl_bp512r1param    },
    { ECC_CURVE_BRAINPOOL_P256_T,  &g_pkcl_bp256t1param    },
    { ECC_CURVE_BRAINPOOL_P320_T,  &g_pkcl_bp320t1param    },
    { ECC_CURVE_BRAINPOOL_P384_T,  &g_pkcl_bp384t1param    },
    { ECC_CURVE_BRAINPOOL_P512_T,  &g_pkcl_bp512t1param    },
    { ECC_CURVE_CURVE25519,        &g_pkcl_x25519param     },
    { ECC_CURVE_CURVE448,          &g_pkcl_x448param       },
    { ECC_CURVE_ED25519,           &g_pkcl_ed25519param    },
    { ECC_CURVE_ED448,             &g_pkcl_ed448param      }
};

static pkc_prng_func_t s_pkc_prng = { ER_OK, 0, NULL, NULL, NULL };

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t l_PKC_FinishProcess(uint32_t err);
static uint32_t l_PKC_ConvertRsaCode(uint32_t code);
static uint32_t l_PKC_ConvertEccCode(uint32_t code);
static uint32_t l_PKC_GenerateRandom(const uint32_t rand_len, uint8_t * const p_random);
static uint32_t l_PKC_GetErrorGenerateRandom(void);
static uint32_t l_PKC_HashInit(hash_ctx_t *p_hash_ctx);
static uint32_t l_PKC_HashUpdate(hash_ctx_t *p_hash_ctx, const uint8_t * const p_data, const uint32_t data_len);
static uint32_t l_PKC_HashFinal(hash_ctx_t *p_hash_ctx, uint8_t * const p_md, uint32_t * const p_md_len);
static uint32_t l_PKC_Kdf(void* const p_kdf_ctx, const uint8_t* const p_shared_secret,
                          const uint32_t shared_secret_len, const uint32_t out_len, uint8_t* const p_out_key);
static uint32_t l_PKC_RsaCommonSetup(rsa_ctx_t *p_rsa_ctx, e_pkc_rsa_padding_t padding,
                                     hash_ctx_t *p_hash_ctx, r_hash_primitive_t primitive);
static uint32_t l_PKC_RsaKeyMod(const void *p_rsa_key, uint8_t *p_modulus,
                                uint8_t *p_pubexp, uint8_t *p_privexp, uint32_t align_num);
static uint32_t l_PKC_SetCurveParams(r_ecc_curve_t curve_id, const st_pkc_ec_curve_t **p_out_params);


/*******************************************************************************/
/* Common APIs                                                                 */
/*******************************************************************************/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0140]:[ICUMFW_UD_L_CRY_PKC_0140]
* Function name: L_PKC_Init
* Description  : Initialize PKC library
* Arguments    : p_func         : Pointer to external functions used by PKC library
* Return Value : ER_OK
****************************************************************************************/
uint32_t L_PKC_Init(const pkc_prng_func_t * const p_func)
{
    s_pkc_prng.err = ER_OK;
    s_pkc_prng.res_id = p_func->res_id;
    s_pkc_prng.p_get = p_func->p_get;
    s_pkc_prng.p_free = p_func->p_free;
    s_pkc_prng.p_generate = p_func->p_generate;

    (void)R_PKCL_Initialize();

    return (ER_OK);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0142]:[ICUMFW_UD_L_CRY_PKC_0142]
* Function name: l_PKC_KickCallBackWDT
* Description  : Kick call back function for WDT restart.
* Arguments    : None
* Return Value : ER_OK
****************************************************************************************/
uint32_t l_PKC_KickCallBackWDT(void)
{
    D_WDT_StartIfNeeded();

    return (ER_OK);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0143]:[ICUMFW_UD_L_CRY_PKC_0143]
* Function name: L_PKC_RsaChkPkccBusy
* Description  : Check execution state of PKCC
* Arguments    : None
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_L_RSA_ERROR
****************************************************************************************/
uint32_t L_PKC_RsaChkPkccBusy(void)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_ChkPKCCBusy();
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0144]:[ICUMFW_UD_L_CRY_PKC_0144]
* Function name: L_PKC_EccChkPkccBusy
* Description  : Check execution state of PKCC
* Arguments    : None
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_L_ECC_ERROR
****************************************************************************************/
uint32_t L_PKC_EccChkPkccBusy(void)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_ChkPKCCBusy();
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0206]:[ICUMFW_UD_L_CRY_PKC_0206]
* Function name: l_PKC_FinishProcess
* Description  : End process for PKC library
* Arguments    : err           : Error code of PKC library
* Return Value : ER_OK
****************************************************************************************/
static uint32_t l_PKC_FinishProcess(uint32_t err)
{
    /* If the error handling is ok or pending, end process */
    if ((ER_OK == err) ||
        (ER_IN_PROCESS == err) ||
        (ER_PENDING == err) ||
        (ER_PENDING_ARB == err))
    {
        /* Do nothing */
    }
    else
    {
        /* Clear PKC driver state */
        (void)R_PKCL_ClearState();
    }

    /* Kick call back function for WDT restart */
    (void)l_PKC_KickCallBackWDT();

    return (ER_OK);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0133]:[ICUMFW_UD_L_CRY_PKC_0133]
* Function name: l_PKC_ConvertRsaCode
* Description  : Convert error code for RSA
* Arguments    : code           : Error code of PKC library
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_HASH_INVALID_ARG
*                ER_L_RSA_SYS_ERROR
*                ER_L_RSA_VERIFY_FAILED
*                ER_L_RSA_KEYGEN_RETRYOVER
*                ER_PENDING
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
static uint32_t l_PKC_ConvertRsaCode(uint32_t code)
{
    uint32_t    i;
    uint32_t    err;

    err = ER_L_RSA_ERROR;

    /* In the case of DRBG error, convert to errors of callback function */
    if (PKCL_RET_DRBGAPIERR == code)
    {
        err = l_PKC_GetErrorGenerateRandom();
    }
    else
    {
        /* Convert to crypt library errors from pkc library errors */
        for (i = NUM0; i < NB_RSA_CODE; i++)
        {
            if (rsa_code_tuples[i].pkc_code == code)
            {
                err = rsa_code_tuples[i].lib_code;
                break;
            }
            else
            {
                /* Do nothing */
            }
        }
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0134]:[ICUMFW_UD_L_CRY_PKC_0134]
* Function name: l_PKC_ConvertEccCode
* Description  : Convert error code for ECC
* Arguments    : code           : Error code of PKC library
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_L_HASH_INVALID_ARG
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_VERIFY_FAILED
*                ER_PENDING
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
static uint32_t l_PKC_ConvertEccCode(uint32_t code)
{
    uint32_t    i;
    uint32_t    err;

    err = ER_L_ECC_ERROR;

    /* In the case of DRBG error, convert to errors of callback function */
    if (PKCL_RET_DRBGAPIERR == code)
    {
        err = l_PKC_GetErrorGenerateRandom();
    }
    else
    {
        /* Convert to crypt library errors from pkc library errors */
        for (i = NUM0; i < NB_ECC_CODE; i++)
        {
            if (ecc_code_tuples[i].pkc_code == code)
            {
                err = ecc_code_tuples[i].lib_code;
                break;
            }
            else
            {
                /* Do nothing */
            }
        }
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0207]:[ICUMFW_UD_L_CRY_PKC_0207]
* Function name: l_PKC_GetErrorGenerateRandom
* Description  : Get error code for generate random number for PKC library
* Arguments    : None
* Return Value : ER_OK
*              : ER_PENDING
*              : ER_ML_ABTR_EMPTY
*              : ER_ML_ABTR_NOT_RELEASED
*              : ER_ML_ABTR_NOT_INITIALIZED
*              : ER_ML_ABTR_TIMER_SET_FAILURE
*              : ER_ML_ABTR_SYS_ERROR
*              : ER_ML_PRNG_OK
*              : ER_ML_PRNG_TIMEOUT
*              : ER_ML_PRNG_RESEED
*              : ER_ML_PRNG_STATUS
*              : ER_ML_PRNG_PARAMETER
*              : ER_ML_PRNG_AES_PRM
****************************************************************************************/
static uint32_t l_PKC_GetErrorGenerateRandom(void)
{
    return s_pkc_prng.err;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0135]:[ICUMFW_UD_L_CRY_PKC_0135]
* Function name: l_PKC_GenerateRandom
* Description  : Generate Random number for PKC library
* Arguments    : rand_len       : Output length of random number
*                p_random       : Pointer to output buffer for random number
* Return Value : PKCL_EXTERNAL_API_OK
*                PKCL_EXTERNAL_API_ERR
****************************************************************************************/
static uint32_t l_PKC_GenerateRandom(const uint32_t rand_len, uint8_t * const p_random)
{
    uint32_t    p_resource_id;
    /* buffer for random : 128 bits */
    uint32_t    buffer[NUM4];
    uint32_t    blocks;
    uint32_t    offset;
    uint32_t    err;
    uint32_t    pkc_err;

    offset = NUM0;
    blocks = rand_len / PRNG_SIZE_IN_BYTES;
    if (NUM0 < (rand_len % PRNG_SIZE_IN_BYTES))
    {
        blocks++;
    }
    else
    {
        /* Do nothing */
    }

    if (NULL != p_random)
    {
        err = s_pkc_prng.p_get(s_pkc_prng.res_id, &p_resource_id);
        if (ER_OK == err)
        {
            while (NUM0 < blocks)
            {
                /* Get random : 128bits(=16 bytes =4 long word) */
                err = s_pkc_prng.p_generate(buffer);
                if (ER_OK == err)
                {
                    if (NUM1 < blocks)
                    {
                        l_CMN_MemCpy(&p_random[offset], (const uint8_t *)buffer, PRNG_SIZE_IN_BYTES);
                        offset += PRNG_SIZE_IN_BYTES;
                    }
                    else
                    {
                        /* Last block */
                        l_CMN_MemCpy(&p_random[offset], (const uint8_t *)buffer, (rand_len - offset));
                    }
                }
                /* end of if (ER_OK == err) */
                else
                {
                    break;
                }
                blocks--;
                /* Kick call back function for WDT restart */
                (void)l_PKC_KickCallBackWDT();
            }
            /* end of while (NUM0 < blocks) */

            if (ER_OK == err)
            {
                pkc_err = PKCL_EXTERNAL_API_OK;
            }
            else
            {
                s_pkc_prng.err = err;
                pkc_err = PKCL_EXTERNAL_API_ERR;
            }

            (void)s_pkc_prng.p_free(s_pkc_prng.res_id, p_resource_id);
        }
        /* end of if (ER_OK == err) */
        else
        {
            s_pkc_prng.err = err;
            pkc_err = PKCL_EXTERNAL_API_ERR;
        }
    }
    /* end of if (NULL != p_random) */
    else
    {
        s_pkc_prng.err = ER_PARAM;
        pkc_err = PKCL_EXTERNAL_API_ERR;
    }

    return (pkc_err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0136]:[ICUMFW_UD_L_CRY_PKC_0136]
* Function name: l_PKC_HashInit
* Description  : HASH initialization processing for PKC library
* Arguments    : p_hash_ctx     : Pointer to Hash context
* Return Value : PKCL_EXTERNAL_API_OK
*                PKCL_EXTERNAL_API_ERR
****************************************************************************************/
static uint32_t l_PKC_HashInit(hash_ctx_t *p_hash_ctx)
{
    uint32_t             err;

    err = L_HASH_Init(p_hash_ctx, p_hash_ctx->hash_type);
    if (ER_OK == err)
    {
        err = PKCL_EXTERNAL_API_OK;
    }
    else
    {
        err = PKCL_EXTERNAL_API_ERR;
    }

    return(err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0137]:[ICUMFW_UD_L_CRY_PKC_0137]
* Function name: l_PKC_HashUpdate
* Description  : HASH intermediate processing for PKC library
* Arguments    : p_hash_ctx     : Pointer to Hash context
*                p_data         : Pointer to message data
*                data_len       : Message length (byte)
* Return Value : PKCL_EXTERNAL_API_OK
*                PKCL_EXTERNAL_API_ERR
****************************************************************************************/
static uint32_t l_PKC_HashUpdate(hash_ctx_t *p_hash_ctx, const uint8_t * const p_data, const uint32_t data_len)
{
    uint32_t err;
    uint32_t processed_len;
    uint32_t slice_len;

    processed_len = NUM0;
    slice_len = NUM0;
    if (NULL != p_hash_ctx)
    {
        do
        {
            if ((data_len - processed_len) < NUM4096)
            {
                slice_len = data_len - processed_len;
            }
            else
            {
                slice_len = NUM4096;
            }
            err = L_HASH_Update(p_hash_ctx, &p_data[processed_len], slice_len);
            if (ER_OK == err)
            {
                err = PKCL_EXTERNAL_API_OK;
            }
            else
            {
                err = PKCL_EXTERNAL_API_ERR;
                break;
            }
            processed_len += slice_len;

            /* Kick call back function for WDT restart */
            (void)l_PKC_KickCallBackWDT();

        }
        while (processed_len < data_len);
    }
    else
    {
        err = PKCL_EXTERNAL_API_ERR;
    }

    return(err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0138]:[ICUMFW_UD_L_CRY_PKC_0138]
* Function name: l_PKC_HashFinal
* Description  : HASH termination processing for PKC library
* Arguments    : p_hash_ctx     : Pointer to Hash context
*                p_result       : Pointer to message digest
*                p_md_len       : Pointer to message digest length (byte)
* Return Value : PKCL_EXTERNAL_API_OK
*                PKCL_EXTERNAL_API_ERR
****************************************************************************************/
static uint32_t l_PKC_HashFinal(hash_ctx_t *p_hash_ctx, uint8_t * const p_md, uint32_t * const p_md_len)
{
    uint32_t err;

    if ((NULL != p_hash_ctx) && (NULL!= p_md_len))
    {
        err  = L_HASH_Finish(p_hash_ctx, p_md);
        if (ER_OK == err)
        {
            *p_md_len = (p_hash_ctx->hash_desc.hash_bitlen / NUM8);
            err = PKCL_EXTERNAL_API_OK;
        }
        else
        {
            err = PKCL_EXTERNAL_API_ERR;
        }
    }
    else
    {
        err = PKCL_EXTERNAL_API_ERR;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0139]:[ICUMFW_UD_L_CRY_PKC_0139]
* Function name: l_PKC_Kdf
* Description  : Key derivation function for PKC library
* Arguments    : p_kdf_ctx          : Pointer to KDF context
*                p_shared_secret    : Pointer to shared secret
*                shared_secret_len  : Length of shared secret
*                out_len            : Output common key length
*                p_out_key          : Pointer to common key output destination
* Return Value : PKCL_EXTERNAL_API_OK
*                PKCL_EXTERNAL_API_ERR
****************************************************************************************/
static uint32_t l_PKC_Kdf(void* const p_kdf_ctx, const uint8_t* const p_shared_secret,
                          const uint32_t shared_secret_len, const uint32_t out_len, uint8_t* const p_out_key)
{
    uint32_t err;
    kdf_t *p_param;

    if (NULL != p_kdf_ctx)
    {
        p_param = p_kdf_ctx;
        p_param->secret.shared_secret.p_data = (uint8_t *)p_shared_secret;
        p_param->secret.shared_secret.size_in_bytes = shared_secret_len;
        p_param->p_deriv_key = p_out_key;
        p_param->deriv_key_size_in_bytes = (uint8_t)out_len;
        err  = L_ECC_Kdf(p_param);
        if (ER_OK == err)
        {
            err = PKCL_EXTERNAL_API_OK;
        }
        else
        {
            err = PKCL_EXTERNAL_API_ERR;
        }
    }
    else
    {
        err = PKCL_EXTERNAL_API_ERR;
    }

    return (err);
}

/*******************************************************************************/
/* RSA APIs                                                                    */
/*******************************************************************************/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0145]:[ICUMFW_UD_L_CRY_PKC_0145]
* Function name: L_PKC_RsaContextSetup
* Description  : Setup common PKC context
* Arguments    : p_rsa_ctx      : Pointer to RSA context
*                padding        : RSA padding scheme
*                p_hash_ctx     : Pointer to HASH context
*                primitive      : Hash primitive type
* Return Value : ER_OK
*                ER_PARAM
****************************************************************************************/
uint32_t L_PKC_RsaContextSetup(rsa_ctx_t *p_rsa_ctx)
{
    uint32_t err;
    st_pkc_rsa_ctx_t *p_pkc_ctx;

    if (NULL != p_rsa_ctx)
    {
        p_pkc_ctx = &p_rsa_ctx->pkc_ctx;
        p_pkc_ctx->p_buf = &((uint8_t *)p_rsa_ctx)[sizeof(rsa_ctx_t)];
        p_pkc_ctx->is_constant_time_mode = PKC_BOOL_TRUE;
        p_pkc_ctx->is_crt_mode = PKC_BOOL_FALSE;
        p_pkc_ctx->verified_gen_plain = PKC_BOOL_FALSE;
        p_pkc_ctx->verified_gen_sign = PKC_BOOL_FALSE;
        p_pkc_ctx->sec_mod_exp = PKC_BOOL_FALSE;
        s_pkc_prng.err = ER_OK;
        p_pkc_ctx->p_drbg_api = (PKC_DRBG_T)l_PKC_GenerateRandom;
        err = ER_OK;
    }
    else
    {
        err = ER_PARAM;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0146]:[ICUMFW_UD_L_CRY_PKC_0146]
* Function name: l_PKC_RsaCommonSetup
* Description  : Setup common PKC context
* Arguments    : p_rsa_ctx      : Pointer to RSA context
*                padding        : RSA padding scheme
*                p_hash_ctx     : Pointer to HASH context
*                primitive      : Hash primitive type
* Return Value : ER_OK
*                ER_PARAM
****************************************************************************************/
static uint32_t l_PKC_RsaCommonSetup(rsa_ctx_t *p_rsa_ctx, e_pkc_rsa_padding_t padding,
                                     hash_ctx_t *p_hash_ctx, r_hash_primitive_t primitive)
{
    uint32_t err;
    st_pkc_rsa_ctx_t *p_pkc_ctx;

    err  = L_PKC_RsaContextSetup(p_rsa_ctx);
    if (ER_OK == err)
    {
        p_pkc_ctx = &p_rsa_ctx->pkc_ctx;
        p_pkc_ctx->padding_option = padding;

        if (NULL != p_hash_ctx)
        {
            p_pkc_ctx->p_mask_option = &p_rsa_ctx->mask_option;
            p_rsa_ctx->hash_info.p_hash_ctx = p_hash_ctx;
            p_rsa_ctx->hash_info.p_hash_init_api = (PKC_HASH_INIT_T)l_PKC_HashInit;
            p_rsa_ctx->hash_info.p_hash_update_api = (PKC_HASH_UPDATE_T)l_PKC_HashUpdate;
            p_rsa_ctx->hash_info.p_hash_final_api = (PKC_HASH_FINAL_T)l_PKC_HashFinal;
            p_pkc_ctx->p_mask_option->p_hash_info = &p_rsa_ctx->hash_info;

            switch (primitive)
            {
                case HASH_PRIMITIVE_SHA2_224:
                    p_pkc_ctx->p_mask_option->mask_type = PKC_HASH_TYPE_SHA2_224;
                    p_pkc_ctx->msg_digest_type = PKC_HASH_TYPE_SHA2_224;
                    p_hash_ctx->hash_type = E_SHA2_224;
                    break;
                case HASH_PRIMITIVE_SHA2_256:
                    p_pkc_ctx->p_mask_option->mask_type = PKC_HASH_TYPE_SHA2_256;
                    p_pkc_ctx->msg_digest_type = PKC_HASH_TYPE_SHA2_256;
                    p_hash_ctx->hash_type = E_SHA2_256;
                    break;
                case HASH_PRIMITIVE_SHA2_384:
                    p_pkc_ctx->p_mask_option->mask_type = PKC_HASH_TYPE_SHA2_384;
                    p_pkc_ctx->msg_digest_type = PKC_HASH_TYPE_SHA2_384;
                    p_hash_ctx->hash_type = E_SHA2_384;
                    break;
                case HASH_PRIMITIVE_SHA2_512:
                    p_pkc_ctx->p_mask_option->mask_type = PKC_HASH_TYPE_SHA2_512;
                    p_pkc_ctx->msg_digest_type = PKC_HASH_TYPE_SHA2_512;
                    p_hash_ctx->hash_type = E_SHA2_512;
                    break;
                case HASH_PRIMITIVE_SHA2_512_224:
                    p_pkc_ctx->p_mask_option->mask_type = PKC_HASH_TYPE_SHA2_512_224;
                    p_pkc_ctx->msg_digest_type = PKC_HASH_TYPE_SHA2_512_224;
                    p_hash_ctx->hash_type = E_SHA2_512_224;
                    break;
                case HASH_PRIMITIVE_SHA2_512_256:
                    p_pkc_ctx->p_mask_option->mask_type = PKC_HASH_TYPE_SHA2_512_256;
                    p_pkc_ctx->msg_digest_type = PKC_HASH_TYPE_SHA2_512_256;
                    p_hash_ctx->hash_type = E_SHA2_512_256;
                    break;
                case HASH_PRIMITIVE_SHA3_256:
                    p_pkc_ctx->p_mask_option->mask_type = PKC_HASH_TYPE_SHA3_256;
                    p_pkc_ctx->msg_digest_type = PKC_HASH_TYPE_SHA3_256;
                    p_hash_ctx->hash_type = E_SHA3_256;
                    break;
                case HASH_PRIMITIVE_SHA3_384:
                    p_pkc_ctx->p_mask_option->mask_type = PKC_HASH_TYPE_SHA3_384;
                    p_pkc_ctx->msg_digest_type = PKC_HASH_TYPE_SHA3_384;
                    p_hash_ctx->hash_type = E_SHA3_384;
                    break;
                case HASH_PRIMITIVE_SHA3_512:
                    p_pkc_ctx->p_mask_option->mask_type = PKC_HASH_TYPE_SHA3_512;
                    p_pkc_ctx->msg_digest_type = PKC_HASH_TYPE_SHA3_512;
                    p_hash_ctx->hash_type = E_SHA3_512;
                    break;
                default:
                    err = ER_PARAM;
                    break;
            }
        }
        /* end of if (NULL != p_hash_ctx) */
        else
        {
            err = ER_PARAM;
        }
    }
    /* end of if (NULL != p_rsa_ctx) */
    else
    {
        /* Do nothing */
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0147]:[ICUMFW_UD_L_CRY_PKC_0147]
* Function name: L_PKC_RsaCryptSetup
* Description  : Setup PKC context for signature processing
* Arguments    : p_rsa_ctx      : Pointer to RSA context
*                format         : Hash encoding format
*                p_hash_ctx     : Pointer to HASH context
*                primitive      : Hash primitive type
* Return Value : ER_OK
*                ER_PARAM
****************************************************************************************/
uint32_t L_PKC_RsaCryptSetup(rsa_ctx_t *p_rsa_ctx, r_message_encoding_format_t format,
                             hash_ctx_t *p_hash_ctx, r_hash_primitive_t primitive)
{
    uint32_t err;
    e_pkc_rsa_padding_t padding;

    err = ER_OK;

    if (NULL != p_rsa_ctx)
    {
        if (ENCODING_FORMAT_EME_OAEP == format)
        {
            padding = PKC_RSA_PADDING_PKCS1_OAEP;
        }
        else if (ENCODING_FORMAT_EME_PKCS1_V15 == format)
        {
            padding = PKC_RSA_PADDING_PKCS1;
        }
        else
        {
            err = ER_PARAM;
        }

        if (ER_OK == err)
        {
            err = l_PKC_RsaCommonSetup(p_rsa_ctx, padding, p_hash_ctx, primitive);
            if (PKC_RSA_PADDING_PKCS1 == padding)
            {
                p_rsa_ctx->pkc_ctx.p_mask_option = NULL;
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
        err = ER_PARAM;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0148]:[ICUMFW_UD_L_CRY_PKC_0148]
* Function name: L_PKC_RsaSignSetup
* Description  : Setup PKC context for signature processing
* Arguments    : p_rsa_ctx      : Pointer to RSA context
*                format         : Hash encoding format
*                p_hash_ctx     : Pointer to HASH context
*                primitive      : Hash primitive type
* Return Value : ER_OK
*                ER_PARAM
****************************************************************************************/
uint32_t L_PKC_RsaSignSetup(rsa_ctx_t *p_rsa_ctx, r_hash_encoding_format_t format,
                            hash_ctx_t *p_hash_ctx, r_hash_primitive_t primitive)
{
    uint32_t err;
    e_pkc_rsa_padding_t padding;

    err = ER_OK;

    if (NULL != p_rsa_ctx)
    {
        if (ENCODING_FORMAT_EMSA_PSS == format)
        {
            padding = PKC_RSA_PADDING_PKCS1_PSS;
        }
        else if (ENCODING_FORMAT_EMSA_PKCS1_V15 == format)
        {
            padding = PKC_RSA_PADDING_PKCS1;
        }
        else
        {
            err = ER_PARAM;
        }

        if (ER_OK == err)
        {
            err = l_PKC_RsaCommonSetup(p_rsa_ctx, padding, p_hash_ctx, primitive);
            if (PKC_RSA_PADDING_PKCS1 == padding)
            {
                p_rsa_ctx->pkc_ctx.p_mask_option = NULL;
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
        err = ER_PARAM;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0215]:[ICUMFW_UD_L_CRY_PKC_0215]
* Function name: l_PKC_RsaKeyMod
* Description  : Modification of specified RSA key
* Arguments    : p_rsa_key  : Pointer to RSA context
*                p_modulus  : Pointer to Modified RSA key value (modulus)
*                             The size of MODULUS_SIZE_IN_BYTES must be needed.
*                p_pubexp   : Pointer to Modified RSA key value (public exponent)
*                             The size of PUB_EXP_SIZE_IN_BYTES must be needed.
*                p_privexp  : Pointer to Modified RSA key value (private exponent)
*                             The size of PRIV_EXP_SIZE_IN_BYTES must be needed.
*                align_num  : Alignment number
* Return Value : ER_OK
****************************************************************************************/
static uint32_t l_PKC_RsaKeyMod(const void *p_rsa_key, uint8_t *p_modulus,
                                uint8_t *p_pubexp, uint8_t *p_privexp, uint32_t align_num)
{
    st_pkcd_rsa_key_bin_t   *p_key;
    uint32_t                align_mod;
    uint32_t                diffs;

    p_key = (st_pkcd_rsa_key_bin_t *)p_rsa_key;

    align_mod = p_key->key_length % align_num;
    if (0u != align_mod)
    {
        diffs = align_num - align_mod;

        l_CMN_MemSet(p_modulus, 0x00u, MODULUS_SIZE_IN_BYTES);
        l_CMN_MemCpy(&p_modulus[diffs], p_key->p_n, p_key->key_length);
        p_key->p_n = p_modulus;

        if (NULL != p_key->p_d)
        {
            l_CMN_MemSet(p_privexp, 0x00u, PRIV_EXP_SIZE_IN_BYTES);
            l_CMN_MemCpy(&p_privexp[diffs], p_key->p_d, p_key->key_length);
            p_key->p_d = p_privexp;
        }
        else
        {
            /* Do nothing */
        }

        p_key->key_length = p_key->key_length + diffs;
    }
    else
    {
        /* Do nothing */
    }

    align_mod = p_key->e_length % align_num;
    if (0u != align_mod)
    {
        diffs = align_num - align_mod;

        l_CMN_MemSet(p_pubexp, 0x00u, PUB_EXP_SIZE_IN_BYTES);
        l_CMN_MemCpy(&p_pubexp[diffs], p_key->p_e, p_key->e_length);
        p_key->p_e = p_pubexp;

        p_key->e_length = p_key->e_length + diffs;
    }
    else
    {
        /* Do nothing */
    }

    return ER_OK;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0149]:[ICUMFW_UD_L_CRY_PKC_0149]
* Function name: L_PKC_RsaEncryptInit
* Description  : RSA encryption initial processing for PKC library
* Arguments    : p_plain        : Pointer to plain data
*                p_pub_key      : Pointer to RSA public key
*                mod_len        : Modulus length
*                p_ctx          : Pointer to RSA context
* Return Value : ER_OK
*                ER_PENDING
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_RsaEncryptInit(const st_pkc_data_t* const p_plain,const void* const p_pub_key,
                              const uint32_t mod_len, st_pkc_rsa_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;
    uint8_t     modulus[MODULUS_SIZE_IN_BYTES];
    uint8_t     pub_exp[PUB_EXP_SIZE_IN_BYTES];
    uint8_t     priv_exp[PRIV_EXP_SIZE_IN_BYTES];

    (void)l_PKC_RsaKeyMod(p_pub_key, modulus, pub_exp, priv_exp, NUM4);
    pkc_ret = R_PKCL_RsaEncryptInit(p_plain, p_pub_key, mod_len, p_ctx);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}


/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0150]:[ICUMFW_UD_L_CRY_PKC_0150]
* Function name: L_PKC_RsaEncryptOperate
* Description  : RSA encryption processing for PKC library
* Arguments    : p_ctx          : Pointer to RSA context
*                p_operate_opt  : Pointer to operate option
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
****************************************************************************************/
uint32_t L_PKC_RsaEncryptOperate(st_pkc_rsa_ctx_t* const p_ctx, const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_RsaEncryptOperate(p_ctx, p_operate_opt);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0151]:[ICUMFW_UD_L_CRY_PKC_0151]
* Function name: L_PKC_RsaEncryptFinal
* Description  : RSA encryption termination processing for PKC library
* Arguments    : p_ctx          : Pointer to RSA context
*                p_cipher       : Pointer to ciphertext data
* Return Value : ER_OK
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
****************************************************************************************/
uint32_t L_PKC_RsaEncryptFinal(const st_pkc_rsa_ctx_t* const p_ctx, st_pkc_data_t* const p_cipher)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_RsaEncryptFinal(p_ctx, p_cipher);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0152]:[ICUMFW_UD_L_CRY_PKC_0152]
* Function name: L_PKC_RsaDecryptInit
* Description  : RSA decryption initial processing for PKC library
* Arguments    : p_cipher       : Pointer to ciphertext data
*                p_priv_key     : Pointer to RSA private key
*                mod_len        : Modulus length
*                p_ctx          : Pointer to RSA context
* Return Value : ER_OK
*                ER_PENDING
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_RsaDecryptInit(const st_pkc_data_t* const p_cipher, const void* const p_priv_key,
                              const uint32_t mod_len, st_pkc_rsa_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;
    uint8_t     modulus[MODULUS_SIZE_IN_BYTES];
    uint8_t     pub_exp[PUB_EXP_SIZE_IN_BYTES];
    uint8_t     priv_exp[PRIV_EXP_SIZE_IN_BYTES];

    (void)l_PKC_RsaKeyMod(p_priv_key, modulus, pub_exp, priv_exp, NUM4);
    pkc_ret = R_PKCL_RsaDecryptInit(p_cipher, p_priv_key, mod_len, p_ctx);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0153]:[ICUMFW_UD_L_CRY_PKC_0153]
* Function name: L_PKC_RsaDecryptOperate
* Description  : RSA decryption processing for PKC library
* Arguments    : p_ctx          : Pointer to RSA context
*                p_operate_opt  : Pointer to operate option
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
****************************************************************************************/
uint32_t L_PKC_RsaDecryptOperate(st_pkc_rsa_ctx_t* const p_ctx, const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_RsaDecryptOperate(p_ctx, p_operate_opt);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0154]:[ICUMFW_UD_L_CRY_PKC_0154]
* Function name: L_PKC_RsaDecryptFinal
* Description  : RSA decryption termination processing for PKC library
* Arguments    : p_ctx          : Pointer to RSA context
*                p_plain        : Pointer to plain data
* Return Value : ER_OK
*                ER_PENDING
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_RsaDecryptFinal(const st_pkc_rsa_ctx_t* const p_ctx, st_pkc_data_t* const p_plain)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_RsaDecryptFinal(p_ctx, p_plain);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0155]:[ICUMFW_UD_L_CRY_PKC_0155]
* Function name: L_PKC_RsaSignInit
* Description  : RSA signature initial processing for PKC library
* Arguments    : p_digest       : Pointer to message digest
*                p_priv_key     : Pointer to RSA private key
*                mod_len        : Modulus length
*                p_ctx          : Pointer to RSA context
* Return Value : ER_OK
*                ER_PENDING
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_RsaSignInit(const st_pkc_data_t* const p_digest, const void* const p_priv_key,
                           const uint32_t mod_len, st_pkc_rsa_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;
    uint8_t     modulus[MODULUS_SIZE_IN_BYTES];
    uint8_t     pub_exp[PUB_EXP_SIZE_IN_BYTES];
    uint8_t     priv_exp[PRIV_EXP_SIZE_IN_BYTES];

    (void)l_PKC_RsaKeyMod(p_priv_key, modulus, pub_exp, priv_exp, NUM4);
    pkc_ret = R_PKCL_RsaSignInit(p_digest, p_priv_key, mod_len, p_ctx);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0156]:[ICUMFW_UD_L_CRY_PKC_0156]
* Function name: L_PKC_RsaSignOperate
* Description  : RSA signature processing for PKC library
* Arguments    : p_ctx              : Pointer to RSA context
*                p_rsa_operate_opt  : Pointer to operate option
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
****************************************************************************************/
uint32_t L_PKC_RsaSignOperate(st_pkc_rsa_ctx_t* const p_ctx, const st_pkc_operate_opt_t* const p_rsa_operate_opt)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_RsaSignOperate(p_ctx, p_rsa_operate_opt);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0157]:[ICUMFW_UD_L_CRY_PKC_0157]
* Function name: L_PKC_RsaSignFinal
* Description  : RSA signature termination processing for PKC library
* Arguments    : p_ctx          : Pointer to RSA context
*                p_sign         : Pointer to signature data
* Return Value : ER_OK
*                ER_PENDING
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_RsaSignFinal(const st_pkc_rsa_ctx_t* const p_ctx, st_pkc_data_t*  const p_sign)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_RsaSignFinal(p_ctx, p_sign);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0158]:[ICUMFW_UD_L_CRY_PKC_0158]
* Function name: L_PKC_RsaVerifyInit
* Description  : RSA signature verification initial processing for PKC library
* Arguments    : p_digest       : Pointer to message digest
*                p_sign         : Pointer to signature data
*                p_pub_key      : Pointer to RSA public key
*                mod_len        : Modulus length
*                p_ctx          : Pointer to RSA context
* Return Value : ER_OK
*                ER_PENDING
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_RsaVerifyInit(const st_pkc_data_t * const p_digest, const st_pkc_data_t * const p_sign,
                             const void * const p_pub_key, const uint32_t mod_len, st_pkc_rsa_ctx_t * const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;
    uint8_t     modulus[MODULUS_SIZE_IN_BYTES];
    uint8_t     pub_exp[PUB_EXP_SIZE_IN_BYTES];
    uint8_t     priv_exp[PRIV_EXP_SIZE_IN_BYTES];

    (void)l_PKC_RsaKeyMod(p_pub_key, modulus, pub_exp, priv_exp, NUM4);
    pkc_ret = R_PKCL_RsaVerifyInit(p_digest, p_sign, p_pub_key, mod_len, p_ctx);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0159]:[ICUMFW_UD_L_CRY_PKC_0159]
* Function name: L_PKC_RsaVerifyOperate
* Description  : RSA signature verification processing for PKC library
* Arguments    : p_ctx          : Pointer to RSA context
*                p_operate_opt  : Pointer to operate option
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
****************************************************************************************/
uint32_t L_PKC_RsaVerifyOperate(st_pkc_rsa_ctx_t* const p_ctx,
                                    const st_pkc_operate_opt_t* const p_operate_opt)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_RsaVerifyOperate(p_ctx, p_operate_opt);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0160]:[ICUMFW_UD_L_CRY_PKC_0160]
* Function name: L_PKC_RsaVerifyFinal
* Description  : RSA signature verification termination processing for PKC library
* Arguments    : p_ctx          : Pointer to RSA context
* Return Value : ER_OK
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_L_RSA_SYS_ERROR
*                ER_L_RSA_VERIFY_FAILED
****************************************************************************************/
uint32_t L_PKC_RsaVerifyFinal(const st_pkc_rsa_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_RsaVerifyFinal(p_ctx);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0161]:[ICUMFW_UD_L_CRY_PKC_0161]
* Function name: L_PKC_RsaKeyGenInit
* Description  : RSA key generation initial processing for PKC library
* Arguments    : p_ctx          : Pointer to RSA context
*                key_len        : RSA key length
*                prime_test_num : Maximum number of generation failures
*                iteration_num  : number of trials
* Return Value : ER_OK
*                ER_L_RSA_SYS_ERROR
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_RsaKeyGenInit(st_pkc_rsa_ctx_t* const  p_ctx, const uint32_t key_len,
                             const uint32_t prime_test_num, const uint32_t iteration_num)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_RsaKeyGenInit(p_ctx, key_len, prime_test_num, iteration_num);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0162]:[ICUMFW_UD_L_CRY_PKC_0162]
* Function name: L_PKC_RsaKeyGenOperate
* Description  : RSA key generation processing for PKC library
* Arguments    : p_ctx          : Pointer to RSA context
* Return Value : ER_OK
*                ER_L_RSA_SYS_ERROR
*                ER_IN_PROCESS
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
*                ER_PENDING
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_RsaKeyGenOperate(st_pkc_rsa_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_RsaKeyGenOperate(p_ctx);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0163]:[ICUMFW_UD_L_CRY_PKC_0163]
* Function name: L_PKC_RsaKeyGenFinal
* Description  : RSA key generation termination processing for PKC library
* Arguments    : p_ctx          : Pointer to RSA context
*                p_priv_key     : Pointer to RSA private key
*                p_pub_key      : Pointer to RSA public key
* Return Value : ER_OK
*                ER_L_RSA_SYS_ERROR
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_RsaKeyGenFinal(const st_pkc_rsa_ctx_t* const p_ctx, void* const p_priv_key, void* const p_pub_key)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_RsaKeyGenFinal(p_ctx, p_priv_key, p_pub_key);
    err = l_PKC_ConvertRsaCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/*******************************************************************************/
/* ECC common APIs                                                             */
/*******************************************************************************/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0164]:[ICUMFW_UD_L_CRY_PKC_0164]
* Function name: l_PKC_SetCurveParams
* Description  : Set ECC curve information
* Arguments    : curve_id       : Curve ID
*                p_out_params   : Curve information
* Return Value : ER_OK
*                ER_PARAM
****************************************************************************************/
static uint32_t l_PKC_SetCurveParams(r_ecc_curve_t curve_id, const st_pkc_ec_curve_t **p_out_params)
{
    uint32_t err;
    uint32_t i;

    err = ER_PARAM;
    *p_out_params = NULL;

    for (i = NUM0; i < NB_CURVE_TBL; i++)
    {
        if (curve_table[i].curve_id == curve_id)
        {
            *p_out_params = curve_table[i].p_curve_table;
            err = ER_OK;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    return (err);
}

/*******************************************************************************/
/* ECDH APIs                                                                   */
/*******************************************************************************/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0165]:[ICUMFW_UD_L_CRY_PKC_0165]
* Function name: L_PKC_EcdhSetup
* Description  : Setup the base parameter of ECDH for PKC library
* Arguments    : p_ecc_ctx      : Pointer to ECC context
*                p_kdf_info     : KDF information
* Return Value : ER_OK
*                ER_PARAM
****************************************************************************************/
uint32_t L_PKC_EcdhSetup(ecc_ctx_t *p_ecc_ctx, st_pkc_kdf_t *p_kdf_info)
{
    uint32_t err;
    st_pkc_ec_ctx_t *p_pkc_ctx;

    err = ER_OK;

    if (NULL != p_ecc_ctx)
    {
        p_pkc_ctx = &p_ecc_ctx->ctx.ec;
        p_pkc_ctx->p_buf = &((uint8_t *)p_ecc_ctx)[sizeof(ecc_ctx_t)];
        p_pkc_ctx->p_hash_info = NULL;
        p_pkc_ctx->p_drbg_api = NULL;
        p_pkc_ctx->p_kdf_info = NULL;
        p_pkc_ctx->verified_gen_sign = PKC_BOOL_FALSE;
        p_pkc_ctx->sec_scalar_mul = PKC_BOOL_FALSE;
        err = l_PKC_SetCurveParams(p_ecc_ctx->curve_id, (const st_pkc_ec_curve_t **)&p_ecc_ctx->ctx.ec.p_curve_info);

        if ((ER_OK == err) && (NULL != p_kdf_info))
        {
            p_pkc_ctx->p_kdf_info = p_kdf_info;
            p_pkc_ctx->p_kdf_info->p_kdf_api = (PKC_KDF_T)l_PKC_Kdf;
            p_pkc_ctx->p_kdf_info->p_kdf_ctx = (void *)&p_ecc_ctx->kdf_ctx;
            p_pkc_ctx->p_kdf_info->out_kdf_len = p_ecc_ctx->kdf_ctx.deriv_key_size_in_bytes;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        err = ER_PARAM;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0166]:[ICUMFW_UD_L_CRY_PKC_0166]
* Function name: L_PKC_EcdhInit
* Description  : ECDH initial processing for PKC library
* Arguments    : p_priv_key     : Pointer to ECC private key
*                p_pub_key      : Pointer to ECC public key
*                p_ctx          : Pointer to ECC context
* Return Value : ER_OK
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EcdhInit(const void* const p_priv_key, const void* const p_pub_key, st_pkc_ec_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdhInit(p_priv_key, p_pub_key, p_ctx);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0167]:[ICUMFW_UD_L_CRY_PKC_0167]
* Function name: L_PKC_EcdhOperate
* Description  : ECDH processing for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
*                sync_opt       : Synchronous / asynchronous
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EcdhOperate(st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdhOperate(p_ctx, sync_opt);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0168]:[ICUMFW_UD_L_CRY_PKC_0168]
* Function name: L_PKC_EcdhFinal
* Description  : ECDH termination processing for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
*                p_shared       : Pointer to shared key
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EcdhFinal(const st_pkc_ec_ctx_t* const p_ctx, st_pkc_data_t* const p_shared)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdhFinal(p_ctx, p_shared);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0211]:[ICUMFW_UD_L_CRY_PKC_0211]
* Function name: L_PKC_EcdhXSetup
* Description  : Setup the base parameter of ECDHX for PKC library
* Arguments    : p_ecc_ctx      : Pointer to ECC context
*                p_kdf_info     : KDF information
* Return Value : ER_OK
*                ER_PARAM
****************************************************************************************/
uint32_t L_PKC_EcdhXSetup(ecc_ctx_t *p_ecc_ctx, st_pkc_kdf_t *p_kdf_info)
{
    uint32_t err;
    st_pkc_ecdhx_ctx_t *p_pkc_ctx;

    err = ER_OK;

    if (NULL != p_ecc_ctx)
    {
        p_pkc_ctx = &p_ecc_ctx->ctx.ecdhx;
        p_pkc_ctx->p_buf = &((uint8_t *)p_ecc_ctx)[sizeof(ecc_ctx_t)];
        p_pkc_ctx->p_hash_info = NULL;
        p_pkc_ctx->p_kdf_info = NULL;
        s_pkc_prng.err = ER_OK;
        p_pkc_ctx->p_drbg_api = (PKC_DRBG_T)l_PKC_GenerateRandom;
        err = l_PKC_SetCurveParams(p_ecc_ctx->curve_id, (const st_pkc_ec_curve_t **)&p_ecc_ctx->ctx.ecdhx.p_curve_info);

        if ((ER_OK == err) && (NULL != p_kdf_info))
        {
            p_pkc_ctx->p_kdf_info = p_kdf_info;
            p_pkc_ctx->p_kdf_info->p_kdf_api = (PKC_KDF_T)l_PKC_Kdf;
            p_pkc_ctx->p_kdf_info->p_kdf_ctx = (void *)&p_ecc_ctx->kdf_ctx;
            p_pkc_ctx->p_kdf_info->out_kdf_len = p_ecc_ctx->kdf_ctx.deriv_key_size_in_bytes;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        err = ER_PARAM;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0212]:[ICUMFW_UD_L_CRY_PKC_0212]
* Function name: L_PKC_EcdhXInit
* Description  : ECDHX initial processing for PKC library
* Arguments    : p_priv_key     : Pointer to ECC private key
*                p_pub_key      : Pointer to ECC public key
*                p_ctx          : Pointer to ECC context
* Return Value : ER_OK
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EcdhXInit(const void* const p_priv_key, const void* const p_pub_key,
                         st_pkc_ecdhx_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdhXInit(p_priv_key, p_pub_key, p_ctx);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0213]:[ICUMFW_UD_L_CRY_PKC_0213]
* Function name: L_PKC_EcdhXOperate
* Description  : ECDHX processing for PKC library
* Arguments    : p_ctx              : Pointer to ECC context
*                processing_bits    : Number of bits processed at once
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EcdhXOperate(st_pkc_ecdhx_ctx_t* const p_ctx, const uint32_t processing_bits)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdhXOperate(p_ctx, processing_bits);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0214]:[ICUMFW_UD_L_CRY_PKC_0214]
* Function name: L_PKC_EcdhXFinal
* Description  : ECDHX termination processing for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
*                p_shared       : Pointer to shared key
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EcdhXFinal(const st_pkc_ecdhx_ctx_t* const p_ctx, st_pkc_data_t* const p_shared)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdhXFinal(p_ctx, p_shared);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/*******************************************************************************/
/* ECDSA APIs                                                                  */
/*******************************************************************************/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0169]:[ICUMFW_UD_L_CRY_PKC_0169]
* Function name: L_PKC_EccSetup
* Description  : Setup common data of ECC for PKC library
* Arguments    : p_ecc_ctx      : Pointer to ECC context
* Return Value : ER_OK
*                ER_PARAM
****************************************************************************************/
uint32_t L_PKC_EccSetup(ecc_ctx_t *p_ecc_ctx)
{
    uint32_t err;
    st_pkc_ec_ctx_t *p_pkc_ctx;

    if (NULL != p_ecc_ctx)
    {
        p_pkc_ctx = &p_ecc_ctx->ctx.ec;
        p_pkc_ctx->p_buf = &((uint8_t *)p_ecc_ctx)[sizeof(ecc_ctx_t)];
        p_pkc_ctx->p_hash_info = NULL;
        p_pkc_ctx->p_kdf_info = (st_pkc_kdf_t *)NULL;
        p_pkc_ctx->verified_gen_sign = PKC_BOOL_FALSE;
        p_pkc_ctx->sec_scalar_mul = PKC_BOOL_FALSE;
        s_pkc_prng.err = ER_OK;
        p_pkc_ctx->p_drbg_api = (PKC_DRBG_T)l_PKC_GenerateRandom;
        err = l_PKC_SetCurveParams(p_ecc_ctx->curve_id, (const st_pkc_ec_curve_t **)&p_ecc_ctx->ctx.ec.p_curve_info);
    }
    /* end of if (NULL != p_ecc_ctx) */
    else
    {
        err = ER_PARAM;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0170]:[ICUMFW_UD_L_CRY_PKC_0170]
* Function name: L_PKC_EcdsaSignInit
* Description  : ECDSA signature generation initial processing for PKC library
* Arguments    : p_digest       : Pointer to message digest
*                p_priv_key     : Pointer to ECC private key
*                p_ctx          : Pointer to ECC context
* Return Value : ER_OK
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EcdsaSignInit(const st_pkc_data_t* const p_digest, const void* const p_priv_key,
                             st_pkc_ec_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdsaSignInit(p_digest, p_priv_key, p_ctx);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0171]:[ICUMFW_UD_L_CRY_PKC_0171]
* Function name: L_PKC_EcdsaSignOperate
* Description  : ECDSA signature generation processing for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
*                sync_opt       : Synchronous / asynchronous
* Return Value : ER_OK
*                ER_PENDING
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_EcdsaSignOperate(st_pkc_ec_ctx_t* const p_ctx, e_pkc_sync_opt_t sync_opt)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdsaSignOperate(p_ctx, sync_opt);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0172]:[ICUMFW_UD_L_CRY_PKC_0172]
* Function name: L_PKC_EcdsaSignFinal
* Description  : ECDSA signature generation termination processing for PKC library
* Arguments    : p_ctx      : Pointer to ECC context
*                p_sign     : Pointer to signature data
* Return Value : ER_OK
*                ER_PENDING
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_L_ECC_SYS_ERROR
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_EcdsaSignFinal(const st_pkc_ec_ctx_t* const p_ctx, st_pkc_data_t* const p_sign)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdsaSignFinal(p_ctx, p_sign);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0173]:[ICUMFW_UD_L_CRY_PKC_0173]
* Function name: L_PKC_EcdsaVerifyInit
* Description  : ECDSA signature verification initial processing for PKC library
* Arguments    : p_digest       : Pointer to message digest
*                p_sign         : Pointer to signature data
*                p_pub_key      : Pointer to ECC public key
*                p_ctx          : Pointer to ECC context
* Return Value : ER_OK
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EcdsaVerifyInit(const st_pkc_data_t* const p_digest, const st_pkc_data_t* const p_sign,
                               const void* const p_pub_key, st_pkc_ec_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdsaVerifyInit(p_digest, p_sign, p_pub_key, p_ctx);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0174]:[ICUMFW_UD_L_CRY_PKC_0174]
* Function name: L_PKC_EcdsaVerifyOperate
* Description  : ECDSA signature verification processing for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
*                sync_opt       : Synchronous / asynchronous
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_PENDING
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_EcdsaVerifyOperate(st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdsaVerifyOperate(p_ctx, sync_opt);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0175]:[ICUMFW_UD_L_CRY_PKC_0175]
* Function name: L_PKC_EcdsaVerifyFinal
* Description  : ECDSA signature verification termination processing for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_L_ECC_VERIFY_FAILED
****************************************************************************************/
uint32_t L_PKC_EcdsaVerifyFinal(const st_pkc_ec_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdsaVerifyFinal(p_ctx);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0176]:[ICUMFW_UD_L_CRY_PKC_0176]
* Function name: L_PKC_LLEcScalarMultInit
* Description  : Scalar multiplication initial processing for PKC library
* Arguments    : p_scalar_or_key    : byte array of scalar values or EdDSA private key
*                p_point_or_key     : A byte array concatenating X and Y of the public
                                      key coordinates or ECC private/public key
*                p_ctx              : Pointer to ECC context
* Return Value : ER_OK
*                ER_PENDING
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_LLEcScalarMultInit(const st_pkc_select_data_t* const p_scalar_or_key,
                                  const st_pkc_select_data_t* const p_point_or_key,
                                  st_pkc_ec_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_LLEcScalarMultInit(p_scalar_or_key, p_point_or_key, p_ctx);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0177]:[ICUMFW_UD_L_CRY_PKC_0177]
* Function name: L_PKC_LLEcScalarMultOperate
* Description  : Scalar multiplication processing for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
*                sync_opt       : Synchronous / asynchronous
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_LLEcScalarMultOperate(st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_LLEcScalarMultOperate(p_ctx, sync_opt);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0178]:[ICUMFW_UD_L_CRY_PKC_0178]
* Function name: L_PKC_LLEcScalarMultFinal
* Description  : Scalar multiplication termination processing for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
*                p_point_out    : Coordinate data multiplied by scalar
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_LLEcScalarMultFinal(const st_pkc_ec_ctx_t* const p_ctx, st_pkc_data_t* const p_point_out)

{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_LLEcScalarMultFinal(p_ctx, p_point_out);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/*******************************************************************************/
/* EdDSA APIs                                                                  */
/*******************************************************************************/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0179]:[ICUMFW_UD_L_CRY_PKC_0179]
* Function name: L_PKC_EddsaSetup
* Description  : Setup the base parameter of EdDSA for PKC library
* Arguments    : p_ecc_ctx      : Pointer to ECC context
*                p_hash_ctx     : Pointer to HASH context
*                hash_primitive : Hash primitive type
* Return Value : ER_OK
*                ER_PARAM
****************************************************************************************/
uint32_t L_PKC_EddsaSetup(ecc_ctx_t *p_ecc_ctx, hash_ctx_t *p_hash_ctx, r_hash_primitive_t hash_primitive)
{
    uint32_t err;
    st_pkc_eddsa_ctx_t *p_pkc_ctx;

    if (NULL != p_ecc_ctx)
    {
        p_pkc_ctx = &p_ecc_ctx->ctx.eddsa;
        p_pkc_ctx->p_buf = &((uint8_t *)p_ecc_ctx)[sizeof(ecc_ctx_t)];
        p_pkc_ctx->p_hash_info = NULL;
        p_pkc_ctx->p_kdf_info = (st_pkc_kdf_t *)NULL;
        p_pkc_ctx->verifed_gen_sign = PKC_BOOL_FALSE;
        s_pkc_prng.err = ER_OK;
        p_pkc_ctx->p_drbg_api = (PKC_DRBG_T)l_PKC_GenerateRandom;
        err = l_PKC_SetCurveParams(p_ecc_ctx->curve_id, (const st_pkc_ec_curve_t **)&p_pkc_ctx->p_curve_info);

        if ((ER_OK == err) && (NULL != p_hash_ctx))
        {
            p_ecc_ctx->hash_info.p_hash_ctx = p_hash_ctx;
            p_ecc_ctx->hash_info.p_hash_init_api = (PKC_HASH_INIT_T)l_PKC_HashInit;
            p_ecc_ctx->hash_info.p_hash_update_api = (PKC_HASH_UPDATE_T)l_PKC_HashUpdate;
            p_ecc_ctx->hash_info.p_hash_final_api = (PKC_HASH_FINAL_T)l_PKC_HashFinal;
            p_pkc_ctx->p_hash_info = &p_ecc_ctx->hash_info;

            switch (hash_primitive)
            {
                case HASH_PRIMITIVE_SHA2_224:
                    p_hash_ctx->hash_type = E_SHA2_224;
                    break;
                case HASH_PRIMITIVE_SHA2_256:
                    p_hash_ctx->hash_type = E_SHA2_256;
                    break;
                case HASH_PRIMITIVE_SHA2_384:
                    p_hash_ctx->hash_type = E_SHA2_384;
                    break;
                case HASH_PRIMITIVE_SHA2_512:
                    p_hash_ctx->hash_type = E_SHA2_512;
                    break;
                case HASH_PRIMITIVE_SHA2_512_224:
                    p_hash_ctx->hash_type = E_SHA2_512_224;
                    break;
                case HASH_PRIMITIVE_SHA2_512_256:
                    p_hash_ctx->hash_type = E_SHA2_512_256;
                    break;
                case HASH_PRIMITIVE_SHA3_256:
                    p_hash_ctx->hash_type = E_SHA3_256;
                    break;
                case HASH_PRIMITIVE_SHA3_384:
                    p_hash_ctx->hash_type = E_SHA3_384;
                    break;
                case HASH_PRIMITIVE_SHA3_512:
                    p_hash_ctx->hash_type = E_SHA3_512;
                    break;
                default:
                    err = ER_PARAM;
                    break;
            }
        }
        /* end of if (ER_OK == err && NULL != p_hash_ctx) */
        else
        {
            /* Do nothing */
        }
    }
    /* end of if (NULL != p_ecc_ctx) */
    else
    {
        err = ER_PARAM;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0180]:[ICUMFW_UD_L_CRY_PKC_0180]
* Function name: L_PKC_EddsaSignInit
* Description  : EdDSA signature generation initial processing for PKC library
* Arguments    : p_msg          : Pointer to message data
*                p_priv_key     : Pointer to ECC private key
*                p_pub_key      : Pointer to ECC public key
*                p_ctx          : Pointer to ECC context
* Return Value : ER_OK
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EddsaSignInit(const st_pkc_data_t* const p_msg, const void* const p_priv_key,
                             const void* const p_pub_key, st_pkc_eddsa_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EddsaSignInit(p_msg, p_priv_key, p_pub_key, p_ctx);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0181]:[ICUMFW_UD_L_CRY_PKC_0181]
* Function name: L_PKC_EddsaSignOperate
* Description  : EdDSA signature generation processing for PKC library
* Arguments    : p_ctx              : Pointer to ECC context
*                processing_bits    : Number of bits processed at once
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EddsaSignOperate(st_pkc_eddsa_ctx_t* const p_ctx, const uint32_t processing_bits)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EddsaSignOperate(p_ctx, processing_bits);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0182]:[ICUMFW_UD_L_CRY_PKC_0182]
* Function name: L_PKC_EddsaSignFinal
* Description  : EdDSA signature generation termination processing for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
*                p_sign         : Pointer to signature data
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EddsaSignFinal(const st_pkc_eddsa_ctx_t* const p_ctx, st_pkc_data_t* const p_sign)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EddsaSignFinal(p_ctx, p_sign);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0183]:[ICUMFW_UD_L_CRY_PKC_0183]
* Function name: L_PKC_EddsaVerifyInit
* Description  : EdDSA signature verification initial processing for PKC library
* Arguments    : p_msg          : Pointer to message data
*                p_sign         : Pointer to signature data
*                p_pub_key      : Pointer to ECC public key
*                p_ctx          : Pointer to ECC context
* Return Value : ER_OK
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EddsaVerifyInit(const st_pkc_data_t* const p_msg, const st_pkc_data_t* const p_sign,
                               const void* const p_pub_key, st_pkc_eddsa_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EddsaVerifyInit(p_msg, p_sign, p_pub_key, p_ctx);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0184]:[ICUMFW_UD_L_CRY_PKC_0184]
* Function name: L_PKC_EddsaVerifyOperate
* Description  : EdDSA signature verification processing for PKC library
* Arguments    : p_ctx              : Pointer to ECC context
*                processing_bits    : Number of bits processed at once
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EddsaVerifyOperate(st_pkc_eddsa_ctx_t* const p_ctx, const uint32_t processing_bits)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EddsaVerifyOperate(p_ctx, processing_bits);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0185]:[ICUMFW_UD_L_CRY_PKC_0185]
* Function name: L_PKC_EddsaVerifyFinal
* Description  : EdDSA signature verification termination processing for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_L_ECC_VERIFY_FAILED
****************************************************************************************/
uint32_t L_PKC_EddsaVerifyFinal(const st_pkc_eddsa_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EddsaVerifyFinal(p_ctx);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0186]:[ICUMFW_UD_L_CRY_PKC_0186]
* Function name: L_PKC_EddsaKeyGenInit
* Description  : EdDSA private/public key pair generation initial processing
                 for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
* Return Value : ER_OK
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR 
*                ER_PENDING
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_EddsaKeyGenInit(st_pkc_eddsa_ctx_t* const p_ctx)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EddsaKeyGenInit(p_ctx);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0187]:[ICUMFW_UD_L_CRY_PKC_0187]
* Function name: L_PKC_EddsaKeyGenOperate
* Description  : EdDSA private/public key pair generation processing for PKC library
* Arguments    : p_ctx              : Pointer to ECC context
*                processing_bits    : Number of bits processed at once
* Return Value : ER_OK
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_PENDING
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_EddsaKeyGenOperate(st_pkc_eddsa_ctx_t* const p_ctx, const uint32_t processing_bits)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EddsaKeyGenOperate(p_ctx, processing_bits);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0188]:[ICUMFW_UD_L_CRY_PKC_0188]
* Function name: L_PKC_EddsaKeyGenFinal
* Description  : EdDSA private/public key pair generation termination processing
                 for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
*                p_priv_key     : Pointer to ECC private key
*                p_pub_key      : Pointer to ECC public key
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EddsaKeyGenFinal(const st_pkc_eddsa_ctx_t* const p_ctx,
                                void* const p_priv_key, void* const p_pub_key)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EddsaKeyGenFinal(p_ctx, p_priv_key, p_pub_key);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0189]:[ICUMFW_UD_L_CRY_PKC_0189]
* Function name: L_PKC_EcKeyGenInit
* Description  : ECC private/public key pair generation initial processing
                 for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_PENDING
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_EcKeyGenInit(st_pkc_ec_ctx_t* const p_ctx)
{
    uint32_t err;
    pkcl_ret_t pkc_ret;

    pkc_ret = R_PKCL_EcKeyGenInit(p_ctx);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0190]:[ICUMFW_UD_L_CRY_PKC_0190]
* Function name: L_PKC_EcKeyGenOperate
* Description  : ECC private/public key pair generation processing for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
*                sync_opt       : Synchronous / asynchronous
* Return Value : ER_OK
*                ER_PENDING
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_EcKeyGenOperate(st_pkc_ec_ctx_t* const p_ctx, const e_pkc_sync_opt_t sync_opt)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcKeyGenOperate(p_ctx, sync_opt);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0191]:[ICUMFW_UD_L_CRY_PKC_0191]
* Function name: L_PKC_EcKeyGenFinal
* Description  : ECC private/public key pair generation termination processing
                 for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
*                p_priv_key     : Pointer to ECC private key
*                p_pub_key      : Pointer to ECC public key
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EcKeyGenFinal(const st_pkc_ec_ctx_t* const p_ctx,
                            void* const p_priv_key,
                            void* const p_pub_key)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcKeyGenFinal(p_ctx, p_priv_key, p_pub_key);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0208]:[ICUMFW_UD_L_CRY_PKC_0208]
* Function name: L_PKC_EcdhXKeyGenInit
* Description  : EcdhX private/public key pair generation initial processing
                 for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
* Return Value : ER_OK
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
*                ER_PENDING
*                ER_ML_ABTR_EMPTY
*                ER_ML_ABTR_NOT_RELEASED
*                ER_ML_ABTR_NOT_INITIALIZED
*                ER_ML_ABTR_TIMER_SET_FAILURE
*                ER_ML_ABTR_SYS_ERROR
*                ER_ML_PRNG_OK
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
uint32_t L_PKC_EcdhXKeyGenInit(st_pkc_ecdhx_ctx_t* const p_ctx)
{
    uint32_t err;
    pkcl_ret_t pkc_ret;

    pkc_ret = R_PKCL_EcdhXKeyGenInit(p_ctx);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0209]:[ICUMFW_UD_L_CRY_PKC_0209]
* Function name: L_PKC_EcdhXKeyGenOperate
* Description  : EcdhX private/public key pair generation processing for PKC library
* Arguments    : p_ctx              : Pointer to ECC context
*                processing_bits    : Number of bits processed at once
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_IN_PROCESS
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EcdhXKeyGenOperate(st_pkc_ecdhx_ctx_t* const p_ctx, const uint32_t processing_bits)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdhXKeyGenOperate(p_ctx, processing_bits);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_PKC_0210]:[ICUMFW_UD_L_CRY_PKC_0210]
* Function name: L_PKC_EcdhXKeyGenFinal
* Description  : EcdhX private/public key pair generation termination processing
                 for PKC library
* Arguments    : p_ctx          : Pointer to ECC context
*                p_priv_key     : Pointer to ECC private key
*                p_pub_key      : Pointer to ECC public key
* Return Value : ER_OK
*                ER_L_ECC_SYS_ERROR
*                ER_L_ECC_ERROR
*                ER_L_ECC_INVALID_ARG
*                ER_L_ECC_LENGTHERR
****************************************************************************************/
uint32_t L_PKC_EcdhXKeyGenFinal(const st_pkc_ecdhx_ctx_t* const p_ctx,
                                void* const p_priv_key,
                                void* const p_pub_key)
{
    pkcl_ret_t pkc_ret;
    uint32_t err;

    pkc_ret = R_PKCL_EcdhXKeyGenFinal(p_ctx, p_priv_key, p_pub_key);
    err = l_PKC_ConvertEccCode(pkc_ret);
    (void)l_PKC_FinishProcess(err);

    return (err);
}

/* End of file */
