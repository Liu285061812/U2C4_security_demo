/******************************************************************************/
/* Component Name   ICU-M Service                                             */
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
#include "renesas_types.h"
#include "r_icumif_api.h"
#include "icum_common_pub.h"
#include "icum_s_pub.h"
#include "icum_s_03_common.h"
#include "icum_d_comm_pub.h"
#include "icum_k_issd_pub.h"
#include "icum_mh_key_pub.h"
#include "icum_mh_sysinfo_pub.h"
#include "icum_ml_ctx_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_service_common.h"
#include "icum_l_cry_pk_pub.h"

/*********************************************************************************
 * Local types
 *********************************************************************************/
/* Defines the byte size of HASH 512 bit */
#define BYTE_SIZE_HASH_512       (64u)
/* Defines the bit convert */
#define BIT_CONVERSION            (8u)

/* Defines is process phase  */
#define ON_HOLD             (0x00u)
#define PROCESS_DF_WRITE    (0x10u)

/* Defines ecc public key size of 64 byte */
#define PUB_KEY_SIZE             (64u)

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t s_03_ECC_KEYGEN_Init(icum_job_t *p_job, ecc_ctx_t **p_ecc_ctx, hash_ctx_t **p_hash_ctx);
static uint32_t s_03_EccKeygenProcess(ecc_ctx_t *p_ecc_ctx,
                                      pkc_small_data_t *p_priv_scalar,
                                      ecc_point_affine_t *p_pub_point);
static uint32_t s_03_EddsaKeygenProcess(ecc_ctx_t *p_ecc_ctx, hash_ctx_t *p_hash_ctx,
                                        pkc_small_data_t *p_priv_scalar,
                                        ecc_point_affine_t *p_pub_point);
static uint32_t s_03_EcdhXKeygenProcess(ecc_ctx_t *p_ecc_ctx,
                                        pkc_small_data_t *p_priv_scalar,
                                        ecc_point_affine_t *p_pub_point);
/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_ECC_KEY_GEN_0002]:[ICUMFW_UD_S_03_ECC_KEY_GEN_0002]
* Function name: s_03_ECC_KEYGEN_Init()
* Description  : Perform initialize of generate the public key and secret key of the ECC key
* Arguments    : (IN-OUT) p_job
*              : (IN-OUT) p_ecc_ctx
*              : (IN-OUT) p_hash_ctx
* Return Value : ER_OK
*                ER_PARAM
*                ER_ML_CTX_ALL_USE
*                ER_MH_KEY_INVALID_CURVE_ID
*                ER_MH_KEY_INVALID_KEY_GROUP
****************************************************************************************/
static uint32_t s_03_ECC_KEYGEN_Init(icum_job_t *p_job, ecc_ctx_t **p_ecc_ctx, hash_ctx_t **p_hash_ctx)
{
    uint32_t err;
    uint32_t key_size_in_bytes;
    r_icumif_isd_t     *p_ISD;

    /* initialize parameter */
    err = ER_PARAM;
    key_size_in_bytes = NUM0;
    p_ISD = p_job->p_ISD;

    /* Checks the user selected curve */
    err = MH_KEY_GetEccKeySize(p_ISD->prm.ECC_KEY_GENERATE.curve_id, &key_size_in_bytes);
    if (ER_OK != err)
    {
        /* invalid curve: abort */
    }
    /* check the key group */
    else if ((KEY_GRP_ECC != p_ISD->prm.ECC_KEY_GENERATE.key_group) &&
             (KEY_GRP_ECC_RAM != p_ISD->prm.ECC_KEY_GENERATE.key_group))
    {
        /* invalid key group */
        err = (uint32_t)ER_MH_KEY_INVALID_KEY_GROUP;
    }
    else
    {
        /* Get ECC context */
        err = ML_CTX_Get(CTX_TYPE_ECC, (uint32_t **)p_ecc_ctx);
        if (ER_OK == err)
        {
            /* If curve_id is ED25519 and Ed448, get HASH context */
            if ((ECC_CURVE_ED25519 == p_ISD->prm.ECC_KEY_GENERATE.curve_id) ||
                (ECC_CURVE_ED448 == p_ISD->prm.ECC_KEY_GENERATE.curve_id))
            {
                err = ML_CTX_Get(CTX_TYPE_HASH, (uint32_t **)p_hash_ctx);
                if (ER_OK == err)
                {
                    /* do nothing */
                }
                else
                {
                    /* Free ECC context */
                    (void)ML_CTX_Free(CTX_TYPE_ECC, (uint32_t *)*p_ecc_ctx);
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

        if (ER_OK == err)
        {
            /* Set the ecc curve id to ecc context */
            (*p_ecc_ctx)->curve_id = p_ISD->prm.ECC_KEY_GENERATE.curve_id;
        }
        else
        {
            /* do nothing */
        }
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_ECC_KEY_GEN_0003]:[ICUMFW_UD_S_03_ECC_KEY_GEN_0003]
* Function name: s_03_EccKeygenProcess()
* Description  : Perform process of generation of the public key and the secret key of the ECC key.
* Arguments    : (IN-OUT) p_ecc_ctx
*              : (IN) p_priv_scalar
*              : (IN-OUT) p_pub_point
* Return Value : ER_OK
*                ER_PARAM
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
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
static uint32_t s_03_EccKeygenProcess(ecc_ctx_t *p_ecc_ctx,
                                      pkc_small_data_t *p_priv_scalar,
                                      ecc_point_affine_t *p_pub_point)
{
    uint32_t                err;
    st_pkcd_ec_key_bin_t    ec_pri_key;
    st_pkcd_ec_key_bin_t    ec_pub_key;
    pkc_small_data_t        dummy_key_scalar;
    ecc_point_affine_t      dummy_key_point;

    /* Initialize parameters */
    err = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&ec_pri_key, DATA_ZERO, (int16_t)sizeof(ec_pri_key));
    L_Toolbox_MemSet((uint8_t *)&ec_pub_key, DATA_ZERO, (int16_t)sizeof(ec_pub_key));
    L_Toolbox_MemSet((uint8_t *)&dummy_key_scalar, DATA_ZERO, (int16_t)sizeof(dummy_key_scalar));
    L_Toolbox_MemSet((uint8_t *)&dummy_key_point, DATA_ZERO, (int16_t)sizeof(dummy_key_point));

    /* Setup base parameter of ECC for ecc key generate */
    err = L_PKC_EccSetup(p_ecc_ctx);
    if (ER_OK == err)
    {
        /* perform init process for ecc key generate */
        err = L_PKC_EcKeyGenInit(&p_ecc_ctx->ctx.ec);
        if (ER_OK == err)
        {
            /* perform operate process for ecc key generate */
            err = L_PKC_EcKeyGenOperate(&p_ecc_ctx->ctx.ec, PKC_SYNC_OPT_SYNC);
            if (ER_OK == err)
            {
                /* Set parameters of ecc key generate */
                ec_pri_key.p_d  = (uint8_t *)&p_priv_scalar->value;
                ec_pri_key.p_qx = (uint8_t *)&p_pub_point->x.value;
                ec_pri_key.p_qy = (uint8_t *)&p_pub_point->y.value;
                ec_pub_key.p_d  = (uint8_t *)&dummy_key_scalar.value;
                ec_pub_key.p_qx = (uint8_t *)&dummy_key_point.x.value;
                ec_pub_key.p_qy = (uint8_t *)&dummy_key_point.y.value;
                /* perform final process for ecc key generate */
                err = L_PKC_EcKeyGenFinal(&p_ecc_ctx->ctx.ec, (void *)&ec_pri_key, (void *)&ec_pub_key);

                /* Set the key length of after perform ecc key generate */
                p_priv_scalar->size = ec_pri_key.key_length;
                p_pub_point->x.size = ec_pri_key.key_length;
                p_pub_point->y.size = ec_pri_key.key_length;
            }
            else
            {
                /* error case : do nothing */
            }
        }
        else
        {
            /* error case : do nothing */
        }
    }
    else
    {
        /* error case : do nothing */
    }

    /* Free ECC context */
    (void)ML_CTX_Free(CTX_TYPE_ECC, (uint32_t *)p_ecc_ctx);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_ECC_KEY_GEN_0005]:[ICUMFW_UD_S_03_ECC_KEY_GEN_0005]
* Function name: s_03_EcdhXKeygenProcess()
* Description  : Perform process of generation of the public key and the secret key of the ecdhx key.
* Arguments    : (IN-OUT) p_ecc_ctx
*              : (IN) p_priv_scalar
*              : (IN-OUT) p_pub_point
* Return Value : ER_OK
*                ER_PARAM
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
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
****************************************************************************************/
static uint32_t s_03_EcdhXKeygenProcess(ecc_ctx_t *p_ecc_ctx,
                                        pkc_small_data_t *p_priv_scalar,
                                        ecc_point_affine_t *p_pub_point)
{
    uint32_t                err;
    uint8_t                 dummy_pub_key_scalar[PUB_KEY_SIZE];
    uint8_t                 dummy_pub_key_point[PUB_KEY_SIZE];
    st_pkcd_ecdhx_key_bin_t ecdhx_pri_key;
    st_pkcd_ecdhx_key_bin_t ecdhx_pub_key;

    /* Initialize parameters */
    err = ER_OK;
    L_Toolbox_MemSet(&dummy_pub_key_scalar[0], DATA_ZERO, (int16_t)sizeof(dummy_pub_key_scalar));
    L_Toolbox_MemSet(&dummy_pub_key_point[0], DATA_ZERO, (int16_t)sizeof(dummy_pub_key_point));
    L_Toolbox_MemSet((uint8_t *)&ecdhx_pri_key, DATA_ZERO, (int16_t)sizeof(ecdhx_pri_key));
    L_Toolbox_MemSet((uint8_t *)&ecdhx_pub_key, DATA_ZERO, (int16_t)sizeof(ecdhx_pub_key));

    /* Setup base parameter of ECC for ecdhx key generate */
    err = L_PKC_EcdhXSetup(p_ecc_ctx, NULL);
    if (ER_OK == err)
    {
        /* perform initialize process for ecdhx key generate */
        err = L_PKC_EcdhXKeyGenInit(&p_ecc_ctx->ctx.ecdhx);
        if (ER_OK == err)
        {
            /* perform update process for ecdhx key generate */
            do
            {
                err = L_PKC_EcdhXKeyGenOperate(&p_ecc_ctx->ctx.ecdhx, L_PKC_PROCESSING_BITS_MAX);
            } while (ER_IN_PROCESS == err);

            if (ER_OK == err)
            {
                /* Set parameters of ecdhx key generate */
                ecdhx_pri_key.p_private = (uint8_t *)&p_priv_scalar->value[0];
                ecdhx_pri_key.p_public  = (uint8_t *)&p_pub_point->x.value[0];
                ecdhx_pub_key.p_private = &dummy_pub_key_scalar[0];
                ecdhx_pub_key.p_public  = &dummy_pub_key_point[0];
                /* perform final process for ecdhx key generate */
                err = L_PKC_EcdhXKeyGenFinal(&p_ecc_ctx->ctx.ecdhx, (void *)&ecdhx_pri_key, (void *)&ecdhx_pub_key);

                /* Set the key length of after perform ecdhx key generate */
                p_priv_scalar->size = ecdhx_pri_key.key_length;
                p_pub_point->x.size = ecdhx_pri_key.key_length;
            }
            else
            {
                /* error case : do nothing */
            }
        }
        else
        {
            /* error case : do nothing */
        }
    }
    else
    {
        /* error case : do nothing */
    }

    /* Free ECC context */
    (void)ML_CTX_Free(CTX_TYPE_ECC, (uint32_t *)p_ecc_ctx);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_ECC_KEY_GEN_0004]:[ICUMFW_UD_S_03_ECC_KEY_GEN_0004]
* Function name: s_03_EddsaKeygenProcess()
* Description  : Generate the public key and secret key of the edwards key,
*                 and register the key.
* Arguments    : (IN-OUT) p_ecc_ctx
*              : (IN-OUT) p_hash_ctx
*              : (IN-OUT) p_priv_scalar
*              : (IN-OUT) p_pub_point
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
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
*                ER_ML_PRNG_TIMEOUT
*                ER_ML_PRNG_RESEED
*                ER_ML_PRNG_STATUS
*                ER_ML_PRNG_PARAMETER
*                ER_ML_PRNG_AES_PRM
*                ER_L_BIGNUM_BUFFER_OVERFLOW
*                ER_L_BIGNUM_INVALID_ARG
****************************************************************************************/
static uint32_t s_03_EddsaKeygenProcess(ecc_ctx_t *p_ecc_ctx, hash_ctx_t *p_hash_ctx,
                                        pkc_small_data_t *p_priv_scalar,
                                        ecc_point_affine_t *p_pub_point)
{
    uint32_t                err;
    uint8_t                 dummy_pub_key_scalar[PUB_KEY_SIZE];
    uint8_t                 dummy_pub_key_point[PUB_KEY_SIZE];
    st_pkcd_eddsa_key_bin_t ed_priv_key;
    st_pkcd_eddsa_key_bin_t ed_pub_key;

    /* Initialize parameters */
    err = ER_OK;
    L_Toolbox_MemSet(&dummy_pub_key_scalar[0], DATA_ZERO, (int16_t)sizeof(dummy_pub_key_scalar));
    L_Toolbox_MemSet(&dummy_pub_key_point[0], DATA_ZERO, (int16_t)sizeof(dummy_pub_key_point));
    L_Toolbox_MemSet((uint8_t *)&ed_priv_key, DATA_ZERO, (int16_t)sizeof(ed_priv_key));
    L_Toolbox_MemSet((uint8_t *)&ed_pub_key, DATA_ZERO, (int16_t)sizeof(ed_pub_key));

    /* Perform setup process for edwards key generate */
    err = L_PKC_EddsaSetup(p_ecc_ctx, p_hash_ctx, HASH_PRIMITIVE_SHA2_512);
    if (ER_OK == err)
    {
        /* Perform initialize process for edwards key generate */
        err = L_PKC_EddsaKeyGenInit(&p_ecc_ctx->ctx.eddsa);
        if (ER_OK == err)
        {
            do
            {
                /* Perform update process for edwards key generate */
                err = L_PKC_EddsaKeyGenOperate(&p_ecc_ctx->ctx.eddsa, BYTE_SIZE_HASH_512);
            }
            while (ER_IN_PROCESS == err);

            if (ER_OK == err)
            {
                /* Set parameters of edwards key generate */
                ed_priv_key.p_private = (uint8_t *)&p_priv_scalar->value[0];
                ed_priv_key.p_public  = (uint8_t *)&p_pub_point->x.value[0];
                ed_pub_key.p_private  = &dummy_pub_key_scalar[0];
                ed_pub_key.p_public   = &dummy_pub_key_point[0];
                /* Perform final process for edwards key generate */
                err = L_PKC_EddsaKeyGenFinal(&p_ecc_ctx->ctx.eddsa, (void *)&ed_priv_key, (void *)&ed_pub_key);

                /* Set the key length of after perform edwards key generate */
                p_priv_scalar->size = ed_priv_key.key_length;
                p_pub_point->x.size = ed_priv_key.key_length;
            }
            else
            {
                /* error case : do nothing */
            }
        }
        else
        {
            /* error case : do nothing */
        }
    }
    else
    {
        /* error case : do nothing */
    }

    /* Free ECC and hash context */
    (void)ML_CTX_Free(CTX_TYPE_ECC, (uint32_t *)p_ecc_ctx);
    (void)ML_CTX_Free(CTX_TYPE_HASH, (uint32_t *)p_hash_ctx);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_ECC_KEY_GEN_0001]:[ICUMFW_UD_S_03_ECC_KEY_GEN_0001]
* Function name: S_03_GenerateEccKey()
* Description  : # Generate the public key and secret key of the ECC key,
*                  and register the key.
* Arguments    : (IN-OUT)  p_job : Pointer to ECC Key Generate job control parameter.
* Return Value : None
****************************************************************************************/
void S_03_GenerateEccKey(icum_job_t *p_job)
{
    uint32_t            err;
    r_icumif_isd_t     *p_ISD;
    ecc_ctx_t          *p_ecc_ctx;
    hash_ctx_t         *p_hash_ctx;
    ecc_key_t           ecc_mh_key;
    ecc_key_prop_t      key_properties;
    pkc_small_data_t    priv_scalar;
    ecc_point_affine_t  pub_point;

    /* Initialize parameters */
    err = ER_OK;
    p_ISD = p_job->p_ISD;
    p_ecc_ctx = NULL;
    p_hash_ctx = NULL;
    L_Toolbox_MemSet((uint8_t *)&ecc_mh_key, DATA_ZERO, (int16_t)sizeof(ecc_key_t));
    L_Toolbox_MemSet((uint8_t *)&key_properties, DATA_ZERO, (int16_t)sizeof(ecc_key_prop_t));
    L_Toolbox_MemSet((uint8_t *)&priv_scalar, DATA_ZERO, (int16_t)sizeof(pkc_small_data_t));
    L_Toolbox_MemSet((uint8_t *)&pub_point, DATA_ZERO, (int16_t)sizeof(ecc_point_affine_t));

    /* If process_phase is ON_HOLD, do the following */
    if (ON_HOLD == (uint32_t)p_job->process_phase)
    {
        /* Perform key generate initialize (check parameter, get context) */
        err = s_03_ECC_KEYGEN_Init(p_job, &p_ecc_ctx, &p_hash_ctx);
        if (ER_OK == err)
        {
            /* If curve id is ED25519 and Ed448, perform the edwards key generate */
            if ((ECC_CURVE_ED25519 == p_ISD->prm.ECC_KEY_GENERATE.curve_id) ||
                (ECC_CURVE_ED448 == p_ISD->prm.ECC_KEY_GENERATE.curve_id))
            {
                err = s_03_EddsaKeygenProcess(p_ecc_ctx, p_hash_ctx,
                                              &priv_scalar,
                                              &pub_point);
            }
            /* If curve id is CURVE25519 and CURVE448, perform the ecdhx key generate */
            else if ((ECC_CURVE_CURVE25519 == p_ISD->prm.ECC_KEY_GENERATE.curve_id) ||
                     (ECC_CURVE_CURVE448 == p_ISD->prm.ECC_KEY_GENERATE.curve_id))
            {
                err = s_03_EcdhXKeygenProcess(p_ecc_ctx,
                                              &priv_scalar,
                                              &pub_point);
            }
            /* Other than those above, perform the ecc key generate */
            else
            {
                err = s_03_EccKeygenProcess(p_ecc_ctx,
                                            &priv_scalar,
                                            &pub_point);
            }
        }
        else
        {
            /* do nothing */
        }

        if (ER_OK == err)
        {
            /* Set the ecc key parameters */
            ecc_mh_key.availability    = E_AVAILABLE;
            ecc_mh_key.key_group       = p_ISD->prm.ECC_KEY_GENERATE.key_group;
            ecc_mh_key.key_id          = p_ISD->prm.ECC_KEY_GENERATE.key_id;
            ecc_mh_key.p_properties    = &key_properties;
            ecc_mh_key.p_priv_scalar   = &priv_scalar;
            ecc_mh_key.p_pub_point     = &pub_point;

            key_properties.update_counter           = 0U;
            key_properties.ecc_curve                = p_ISD->prm.ECC_KEY_GENERATE.curve_id;
            key_properties.priv_scalar_size_in_bits = (key_size_t)(priv_scalar.size * BIT_CONVERSION);

            key_properties.flags.disable_on_debug   = p_ISD->prm.ECC_KEY_GENERATE.flags.disable_on_debug;
            key_properties.flags.write_protected    = p_ISD->prm.ECC_KEY_GENERATE.flags.write_protected;
            key_properties.flags.invalidate         = 0u;

            /* Update the ecc key */
            err = MH_KEY_SetEccKey(p_job, E_KEY_OPE_FIRST, &ecc_mh_key);
            if (ER_IN_PROCESS == err)
            {
                p_job->process_phase = PROCESS_DF_WRITE;
            }
            else
            {
                /* do nothing */
            }
        }
        /* end of if (ER_OK == err) */
        else
        {
            /* Other than the above, no matter what the value of "err" is, there is no processing. */
            /* The result of "err" is converted later. */
        }
    }
    /* end of if (ON_HOLD == (uint32_t)p_job->process_phase) */
    else if (PROCESS_DF_WRITE == (uint32_t)p_job->process_phase)
    {
        /* update the ecc key in memory */
        ecc_mh_key.key_group    = p_ISD->prm.ECC_KEY_GENERATE.key_group;
        ecc_mh_key.key_id       = p_ISD->prm.ECC_KEY_GENERATE.key_id;
        err = MH_KEY_SetEccKey(p_job, E_KEY_OPE_CONTINUE, &ecc_mh_key);
    }
    else
    {
        err = (uint32_t)SERV_SYS_ERROR;
    }

    (void)s_03_ConvertErr(err, &p_ISD->service_result);
}

