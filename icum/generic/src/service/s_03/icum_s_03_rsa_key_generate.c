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
/* For the used common define */
#include "icum_common_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_k_issd_pub.h"
#include "icum_mh_sysinfo_pub.h"

#include "icum_s_pub.h"
#include "icum_s_03_common.h"

#include "icum_ml_ctx_pub.h"

#include "icum_l_toolbox_pub.h"
#include "icum_l_cry_pk_pub.h"

#include "icum_service_common.h"

/**********************************************************************************
* Local defines
**********************************************************************************/
/* Common */
#define NUM2            ((uint32_t)2)       /* value : 2 */
#define NUM3            ((uint32_t)3u)      /* value : 3 */
#define NUM4            ((uint32_t)4u)      /* value : 4 */
#define NUM7            ((uint32_t)7u)      /* value : 7 */
#define NUM8            ((uint32_t)8u)      /* value : 8 */
#define NUM128          ((uint32_t)128u)    /* value : 128 */
#define NUM256          ((uint32_t)256u)    /* value : 256 */
#define NUM384          ((uint32_t)384u)    /* value : 384 */
#define NUM512          ((uint32_t)512u)    /* value : 512 */

/* the limitation of failure for key generation */
#define MAX_OF_TEST_NUM_1024    (0x0A00u)
#define MAX_OF_TEST_NUM_2048    (0x1400u)
#define MAX_OF_TEST_NUM_3072    (0x1E00u)
#define MAX_OF_TEST_NUM_4096    (0x2800u)


/*********************************************************************************
 * Local types
 *********************************************************************************/

/* process_phase_t */
#define ON_HOLD                 (0x00u)

/*===========================================================================
  [PRIVATE]
  ===========================================================================*/

#define GET_CTX_NONE    (0x00u)
#define GET_CTX_RSA     (0x01u)

/*********************************************************************************
 * Local variables
 *********************************************************************************/
typedef struct
{
    uint8_t n[MODULUS_SIZE_IN_BYTES];
    uint8_t d[PRIV_EXP_SIZE_IN_BYTES];
    uint8_t e[PUB_EXP_SIZE_IN_BYTES];
    uint8_t p[MODULUS_SIZE_IN_BYTES / NUM2];
    uint8_t q[MODULUS_SIZE_IN_BYTES / NUM2];
    uint8_t dp[MODULUS_SIZE_IN_BYTES / NUM2];
    uint8_t dq[MODULUS_SIZE_IN_BYTES / NUM2];
    uint8_t qinv[MODULUS_SIZE_IN_BYTES / NUM2];
} rsa_keygen_work_t;

/* Access only while holding the single RSA/ECC context. */
#pragma alignvar(4)
static rsa_keygen_work_t g_rsa_keygen_work;

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_RSA_KEY_GEN_0002]:[ICUMFW_UD_S_03_RSA_KEY_GEN_0002]
* Function name: s_03_GenerateRsaKeySub
* Description  : Sub routine of generation of RSA key service (service entry point)
* Arguments    : p_job           :[in/out]    : job parameter
*                p_key_param     :[in]        : RSA Key parameter
*                p_rsa_ctx       :[in/out]    : RSA context
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_PENDING
*                ER_PARAM
*                ER_L_RSA_SYS_ERROR
*                ER_L_RSA_ERROR
*                ER_L_RSA_INVALID_ARG
*                ER_L_RSA_LENGTHERR
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
*                ER_MH_KEY_INVALID_KEY_GROUP
*                ER_MH_KEY_INVALID_KEY_INDEX
*                ER_MH_KEY_IN_USE
*                ER_MH_KEY_NOT_USABLE
*                ER_MH_KEY_WRITE_PROTECTED
*                ER_MH_KEY_WRITING
*                ER_MH_KEY_NVM_SYS_CRITICAL
*                ER_MH_KEY_NVM_SYS
*                ER_MH_KEY_NVM_PROG
*                ER_MH_KEY_DATA_INVALID
*                ER_MH_KEY_SYS_ERROR
*                ER_MH_KEY_NVM_UNAVAILABLE
*                ER_MH_KEY_NVM_SIZE
*                ER_MH_KEY_SRC_MEM_ERROR
*                ER_MH_KEY_DEST_MEM_ERROR
*                ER_MH_KEY_AES_TIMEOUT_ERROR
*                ER_MH_KEY_AES_PRM_ERROR
*                SERV_KEY_UPDATE_ERROR
***************************************************************************************/
static uint32_t s_03_GenerateRsaKeySub(icum_job_t *p_job, st_pkcd_rsa_key_bin_t *p_key_param, rsa_ctx_t *p_rsa_ctx)
{
    r_icumif_isd_t             *p_isd;
    ISD_RSA_KEY_GENERATE_t     *p_isd_param;

    uint32_t            err;
    rsa_key_t           key;
    rsa_key_prop_t      key_properties;

    /* PKC Lib Parameter */
    st_pkcd_rsa_key_bin_t pub_key;
    uint32_t key_len;
    uint32_t prime_test_num;
    uint32_t iteration_num;
    uint32_t dummy_pub_exponent[PUB_EXP_SIZE_IN_BYTES / NUM4];

    p_isd       = p_job->p_ISD;
    p_isd_param = &p_isd->prm.RSA_KEY_GENERATE;

    err = ER_OK;

    /* Initialize parameters for PKC API */
    L_Toolbox_MemSet((uint8_t *)&key, DATA_ZERO, (int16_t)sizeof(key));
    L_Toolbox_MemSet((uint8_t *)&key_properties, DATA_ZERO, (int16_t)sizeof(key_properties));
    L_Toolbox_MemSet((uint8_t *)&pub_key, DATA_ZERO, (int16_t)sizeof(st_pkcd_rsa_key_bin_t));
    L_Toolbox_MemSet((uint8_t *)dummy_pub_exponent, DATA_ZERO, (int16_t)sizeof(dummy_pub_exponent));

    /* in process */
    p_isd->service_result = SERV_IN_PROCESS;


    err = L_PKC_RsaContextSetup(p_rsa_ctx);
    key_len = (uint16_t)(((uint32_t)p_isd_param->modulus_size_in_bits + NUM7) / NUM8);
    if (NUM128 == key_len)
    {
        prime_test_num = MAX_OF_TEST_NUM_1024;
        iteration_num = NUM7;
    }
    else if (NUM256 == key_len)
    {
        prime_test_num = MAX_OF_TEST_NUM_2048;
        iteration_num = NUM4;
    }
    else if (NUM384 == key_len)
    {
        prime_test_num = MAX_OF_TEST_NUM_3072;
        iteration_num = NUM3;
    }
    else if (NUM512 == key_len)
    {
        prime_test_num = MAX_OF_TEST_NUM_4096;
        iteration_num = NUM2;
    }
    else
    {
        err = ER_PARAM;
    }

    if (ER_OK == err)
    {
        err = L_PKC_RsaKeyGenInit(&p_rsa_ctx->pkc_ctx, key_len, prime_test_num, iteration_num);
    }
    else
    {
        /* Do nothing */
    }
    if (ER_OK == err)
    {
        do
        {
            err = L_PKC_RsaKeyGenOperate(&p_rsa_ctx->pkc_ctx);
        } while (ER_IN_PROCESS == err);
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK == err)
    {
        pub_key.p_n = p_key_param->p_n;
        pub_key.p_e = (uint8_t *)dummy_pub_exponent;
        err = L_PKC_RsaKeyGenFinal(&p_rsa_ctx->pkc_ctx, (void *)p_key_param, (void *)&pub_key);
    }
    else
    {
        /* Do nothing */
    }
    if (ER_OK == err)
    {
        /* initialize the key parameters */
        key.key_group       = KEY_GRP_RSA;
        key.key_id          = p_isd_param->key_id;
        key.p_properties    = &key_properties;
        key.p_pub_exponent  = p_key_param->p_e;
        key.p_priv_exponent = p_key_param->p_d;
        key.p_modulus       = p_key_param->p_n;

        key_properties.update_counter           = NUM0;
        key_properties.modulus_size_in_bits     = p_isd_param->modulus_size_in_bits;
        key_properties.modulus_size_in_bytes    = (uint16_t)p_key_param->key_length;
        key_properties.priv_exp_size_in_bytes   = (uint16_t)p_key_param->key_length;
        key_properties.pub_exp_size_in_bytes    = (uint16_t)p_key_param->e_length;

        key_properties.flags.disable_on_debug   = p_isd_param->flags.disable_on_debug;
        key_properties.flags.write_protected    = p_isd_param->flags.write_protected;
        key_properties.flags.invalidate         = NUM0;

        /* update the key in memory */
        err = MH_KEY_SetRsaRam(p_job, &key);

    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_RSA_KEY_GEN_0001]:[ICUMFW_UD_S_03_RSA_KEY_GEN_0001]
* Function name: S_03_GenerateRsaKey
* Description  : Generate RSA key service (service entry point)
* Arguments    : p_job :[in/out]    : job parameter
* Return Value : none
***************************************************************************************/
void S_03_GenerateRsaKey(icum_job_t *p_job)
{
    /* retrieve the service handler */
    r_icumif_isd_t             *p_isd;
    ISD_RSA_KEY_GENERATE_t     *p_isd_param;
    job_rsa_t                  *p_job_param;

    st_pkcd_rsa_key_bin_t key_param;

    uint32_t            err;
    rsa_ctx_t          *p_rsa_ctx;
    uint32_t            skip_flg;

    p_isd       = p_job->p_ISD;
    p_isd_param = &p_isd->prm.RSA_KEY_GENERATE;
    p_job_param = &p_job->prm.job_rsa;

    p_rsa_ctx = NULL;
    skip_flg = FLG_OFF;
    p_job->process_phase = ON_HOLD;

    if (FLG_OFF == skip_flg)
    {
        /* first entry */
        p_job_param->resource_ctrl = GET_CTX_NONE;

        /* check modulus size */
        if ((MIN_BIT_LEN_RSA_PRIV_KEY > (uint32_t)p_isd_param->modulus_size_in_bits) ||
            (0u != ((uint32_t)p_isd_param->modulus_size_in_bits % MIN_BIT_LEN_RSA_PRIV_KEY)))
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
        else
        {
            if (MAX_BIT_LEN_RSA_PRIV_KEY < (uint32_t)p_isd_param->modulus_size_in_bits)
            {
                err = ER_PARAM;
                skip_flg = FLG_ON;
            }
            else
            {
                if ((0u != p_isd_param->key_id) || (0u != p_job->vm_id))
                {
                    err = ER_MH_KEY_INVALID_KEY_INDEX;
                    skip_flg = FLG_ON;
                }
                else
                {
                    err = ML_CTX_Get(CTX_TYPE_RSA, (uint32_t **)&p_rsa_ctx);
                }
                if (ER_OK != err)
                {
                    /* error : Error code will be converted by _ConvertErr() function */
                    skip_flg = FLG_ON;
                }
                else
                {
                    p_job_param->resource_ctrl |= GET_CTX_RSA;
                }
            }
            /* end of else */
        }
        /* end of else */
    }
    /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        L_Toolbox_MemSet((uint8_t *)&g_rsa_keygen_work, DATA_ZERO, (int16_t)sizeof(g_rsa_keygen_work));
        L_Toolbox_MemSet((uint8_t *)&key_param, DATA_ZERO, (int16_t)sizeof(key_param));
        key_param.p_d = g_rsa_keygen_work.d;
        key_param.p_n = g_rsa_keygen_work.n;
        key_param.p_p = g_rsa_keygen_work.p;
        key_param.p_q = g_rsa_keygen_work.q;
        key_param.p_dp = g_rsa_keygen_work.dp;
        key_param.p_dq = g_rsa_keygen_work.dq;
        key_param.p_qinv = g_rsa_keygen_work.qinv;
        key_param.p_e = g_rsa_keygen_work.e;
        err = s_03_GenerateRsaKeySub(p_job, &key_param, p_rsa_ctx);
        L_Toolbox_MemSet((uint8_t *)&g_rsa_keygen_work, DATA_ZERO, (int16_t)sizeof(g_rsa_keygen_work));
    }
    else
    {
        /* do nothing */
    }

    if (GET_CTX_RSA == ((uint32_t)p_job_param->resource_ctrl & GET_CTX_RSA))
    {
        (void)ML_CTX_Free(CTX_TYPE_RSA, (uint32_t *)p_rsa_ctx);
        p_job_param->resource_ctrl &= ~GET_CTX_RSA;
    }

    if ((uint32_t)ER_OK != err)
    {
        /* error: terminate the service */
        (void)s_03_ConvertErr(err, &p_isd->service_result);
    }
    /* end of if ((uint32_t)ER_OK != err) */
    else
    {
        if (ON_HOLD == (uint32_t)p_job->process_phase)
        {
            p_isd->service_result = SERV_OK;
        }
        else
        {
            /* do nothing */
        }
    }
}

