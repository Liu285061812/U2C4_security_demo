/******************************************************************************/
/* Component Name   Crypto library(PRNG) source file                          */
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
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "r_icumif_api.h"
#include "icum_d_comm_pub.h"
#include "icum_d_trng_pub.h"
#include "icum_d_cry_aes_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_ml_prng_pub.h"

/**********************************************************************************
* Define
***********************************************************************************/
/* Common */
#define NUM0            ((uint32_t)0)       /* value : 0 */
#define NUM1            ((uint32_t)1)       /* value : 1 */
#define NUM2            ((uint32_t)2)       /* value : 2 */
#define NUM3            ((uint32_t)3)       /* value : 3 */
#define NUM8            ((uint32_t)8)       /* value : 8 */
#define NUM4096         ((uint32_t)4096)    /* value : 4096 */

#define ALIGN4          ((uint32_t)0x00000003)  /* 4byte alignment check */

#define OFF             ((uint32_t)0)   /* off(disable) */
#define ON              ((uint32_t)1)   /* on(enable) */
#define MAXUL           ((uint32_t)0xFFFFFFFFuL)  /* unsigned long max number */
#define DATA_ZERO       ((uint8_t)0u)         /* value : 0 used for initialization */

/* Array */
#define ANUM_PER_BLK    ((uint32_t)4)   /* 32bit array number per block(128bit) */
#define ANUM_TRN        ((uint32_t)10)  /* array number of trn[] */
#define ANUM_MP_NOPAD   ((uint32_t)2)   /* array number of AES-MP(non padding) */
#define ANUM_MP_PAD     ((uint32_t)3)   /* array number of AES-MP(padding) */

/* reseed_interval */
#define RESEED_INTERVAL ((uint32_t)0x00010000)  /* Upper 32bit of 2^48 */
#define AES_STATE_INIT  ((uint32_t)0xFFFFFFFFu)  /* initial state */

/* number of error conversion table */
#define TEST_MODE_RANGE (3u)        /* eligible test mode range (0-3) */

/**********************************************************************************
* Structure
***********************************************************************************/
typedef struct
{
    uint32_t prng_key[ANUM_PER_BLK];
    uint32_t prng_state[ANUM_PER_BLK];
    uint32_t cmplt_extend_seed;
} ml_PRNG_state_s_t;

typedef struct
{
    uint32_t key[ANUM_PER_BLK];
    uint32_t v[ANUM_PER_BLK];
    uint32_t reseed_counter[NUM2];
    uint32_t cmplt_instantiate;
} ml_PRNG_state_cd_t;

/**********************************************************************************
* Variable
***********************************************************************************/
extern uint32_t g_ContextAddress;

#pragma ghs section data=".SECRET_DATA_SECTION"
static ml_PRNG_state_s_t g_state_s = 
{
    NUM0
};
static ml_PRNG_state_cd_t g_state_cd = 
{
    NUM0
};
#pragma ghs section data=default

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t ml_PRNG_CheckParameter(const uint32_t *p_EntropySecret_ptr);
static uint32_t ml_PRNG_OperateAESMP(const uint32_t *p_prng_InputMP[],
                                         uint32_t *p_prng_OutputMP, uint32_t num_block);
static uint32_t ml_PRNG_GenerateRN(uint32_t *p_block_out);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_PRNG_0003]:[ICUMFW_UD_ML_PRNG_0003]
* Function name: ml_PRNG_CheckParameter
* Description  : Check out of range of the L_CRY_PRNG_t
* Arguments    : p_EntropySecret_ptr
*              : (Pseudo random number generator initialization structure pointer)
* Return Value : ER_ML_PRNG_OK
*              : ER_ML_PRNG_PARAMETER
***************************************************************************************/
static uint32_t ml_PRNG_CheckParameter(const uint32_t *p_EntropySecret_ptr)
{
    uint32_t ret_err;

    if ((NULL == p_EntropySecret_ptr)
        || (NUM0 != (ALIGN4 & (uint32_t)p_EntropySecret_ptr)))
    {
        ret_err = ER_ML_PRNG_PARAMETER;
    }
    else
    {
        ret_err = ER_ML_PRNG_OK;
    }

    return ret_err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_PRNG_0004]:[ICUMFW_UD_ML_PRNG_0004]
* Function name: ml_PRNG_OperateAESMP
* Description  : AES-MP arithmetic
* Arguments    : p_prng_InputMP (Pointer to data to be calculated)
*              : p_prng_OutputMP (Pointer to calculation result area)
*              : num_block (Number of blocks of data to be calculated)
* Return Value : ER_ML_PRNG_OK
*              : ER_ML_PRNG_TIMEOUT
*              : ER_ML_PRNG_AES_PRM(Debug only)
*              : ER_PENDING
***************************************************************************************/
static uint32_t ml_PRNG_OperateAESMP(const uint32_t *p_prng_InputMP[],
                                         uint32_t *p_prng_OutputMP, uint32_t num_block)
{
    uint32_t aes_zero[ANUM_PER_BLK];
    uint32_t input_mp[ANUM_MP_PAD][ANUM_PER_BLK];
    uint32_t ret_err;
    uint32_t ct;    /* Loop counter */
    aes_all_at_once_t at_once_param;

    L_Toolbox_MemSet((uint8_t *)&at_once_param, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_t));
    ret_err = ER_ML_PRNG_OK;
    ct = NUM0;

    if (NULL == p_prng_InputMP)
    {
        ret_err = ER_ML_PRNG_PARAMETER;
    }
    else
    {
        for (ct = NUM0; ct < num_block; ++ct)
        {
            L_Toolbox_MemCpy((uint8_t*)input_mp[ct], (const uint8_t*)p_prng_InputMP[ct],
                              (int16_t)((uint32_t)ANUM_PER_BLK * (uint32_t)sizeof(uint32_t)));
        }
        g_ContextAddress          = AES_STATE_INIT;
        at_once_param.cipher_mode = D_AES_CIPHER_MODE_MP;
        at_once_param.direction   = (uint8_t)D_AES_DIR_ENCRYPTION;
        at_once_param.number      = (uint32_t)((num_block * (uint32_t)AES_BLOCK_SIZE_IN_BYTES)
                                         / (uint32_t)AES_BLOCK_SIZE_IN_BYTES);
        at_once_param.key_type    = D_AES_KEYTYPE_PLAIN;
        at_once_param.p_aes_key   = (uint32_t *)aes_zero;
        at_once_param.key_length  = D_AES_KEY_SIZE_IN_BYTES_128BIT;
        at_once_param.p_aes_iv    = (uint32_t *)aes_zero;
        at_once_param.p_aes_src   = (uint32_t*)&input_mp[NUM0][NUM0];
        at_once_param.p_aes_dest  = (uint32_t*)p_prng_OutputMP;
        at_once_param.last_block_in_bits = (uint16_t)NUM0;
        at_once_param.async = D_AES_TRANS_SYNC;

        ret_err = D_AES_ExecutionAtOnce(&at_once_param);
        /* Timeout */
        if (ER_D_AES_TIMEOUT_ERROR == ret_err)  /* Timeout */
        {
            ret_err = ER_ML_PRNG_TIMEOUT;
        }
        else if ( ER_D_AES_ALIGN_ERROR == ret_err )
        {
            ret_err = ER_ML_PRNG_AES_PRM;
        }
        else if (ER_PENDING == ret_err)
        {
            ret_err = ER_PENDING;
        }
        else
        {
            /* Do nothing */
        }
    }

    return ret_err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_PRNG_0012]:[ICUMFW_UD_ML_PRNG_0012]
* Function name: ml_PRNG_GenerateRN
* Description  : Generate pseudo random number at type of SHE and updates the PRNG_STATE
* Arguments    : p_block_out (Pointer to pseudo random number output destination)
* Return Value : ER_ML_PRNG_OK
*              : ER_ML_PRNG_TIMEOUT
*              : ER_ML_PRNG_AES_PRM(Debug only)
*              : ER_PENDING
***************************************************************************************/
static uint32_t ml_PRNG_GenerateRN(uint32_t *p_block_out)
{
    uint32_t ret_err;
    aes_all_at_once_t at_once_param;

    L_Toolbox_MemSet((uint8_t *)&at_once_param, DATA_ZERO, (int16_t)sizeof(aes_all_at_once_t));
    ret_err = ER_ML_PRNG_OK;

    /*  IF(SREGRND_INIT == 1) */

    /*  PRNG_STATEi = ENCECB,KEY_PRNG_KEY(PRNG_STATEi-1) */
    g_ContextAddress          = AES_STATE_INIT;
    at_once_param.cipher_mode = D_AES_CIPHER_MODE_ECB;
    at_once_param.direction   = (uint8_t)D_AES_DIR_ENCRYPTION;
    at_once_param.number      = (uint32_t)((NUM1 * (uint32_t)AES_BLOCK_SIZE_IN_BYTES)
                                    / (uint32_t)AES_BLOCK_SIZE_IN_BYTES);
    at_once_param.key_type    = D_AES_KEYTYPE_PLAIN;
    at_once_param.p_aes_key   = (uint32_t *)g_state_s.prng_key;
    at_once_param.key_length  = D_AES_KEY_SIZE_IN_BYTES_128BIT;
    at_once_param.p_aes_iv    = (uint32_t *)NULL;
    at_once_param.p_aes_src   = (uint32_t*)g_state_s.prng_state;
    at_once_param.p_aes_dest  = (uint32_t*)g_state_s.prng_state;
    at_once_param.last_block_in_bits = (uint16_t)NUM0;
    at_once_param.async = D_AES_TRANS_SYNC;

    ret_err = D_AES_ExecutionAtOnce(&at_once_param);
    /* Timeout */
    if (ER_D_AES_TIMEOUT_ERROR == ret_err)  /* Timeout */
    {
        ret_err = ER_ML_PRNG_TIMEOUT;
    }
    else if ( ER_D_AES_ALIGN_ERROR == ret_err )
    {
        ret_err = ER_ML_PRNG_AES_PRM;
    }
    else if (ER_PENDING == ret_err)
    {
        ret_err = ER_PENDING;
    }
    else
    {
        /*  RND = PRNG_STATEi */
        L_Toolbox_MemCpy((uint8_t*)p_block_out, (uint8_t*)g_state_s.prng_state, (int16_t)sizeof(g_state_s.prng_state));
    }
    /*  END IF */
    return ret_err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_PRNG_0001]:[ICUMFW_UD_ML_PRNG_0001]
* Function name: ML_PRNG_GenTrng
* Description  : Pseudo random number generator initialization
* Arguments    : TestMode (TRNG self-diagnostic test selection mode)
*              : OperationMode (Pseudo random number generator initialization type)
*              : p_prng_Init 
*              : (Pseudo random number generator initialization structure pointer)
* Return Value : ER_ML_PRNG_OK
*              : ER_ML_PRNG_SEED
*              : ER_ML_PRNG_TIMEOUT
*              : ER_ML_PRNG_TRN
*              : ER_ML_PRNG_STATUS
*              : ER_ML_PRNG_PARAMETER
*              : ER_ML_PRNG_AES_PRM(Debug only)
***************************************************************************************/
uint32_t ML_PRNG_GenTrng(uint32_t test_mode, uint32_t trng_size)
{
    uint32_t trng_state;
    uint32_t ret_err;
    uint32_t chk_result;
    
    chk_result = D_TRNG_NOT_FULL;
    ret_err = ER_ML_PRNG_OK;
    
    /* TRN error */
    (void)D_TRNG_GetState(&trng_state);
    if (D_TRNG_TEST_ERR == trng_state)
    {
        ret_err = ER_ML_PRNG_TRN;
    }
    /* Out of Range(TestMode) */
    else if (TEST_MODE_RANGE < test_mode)
    {
        ret_err = ER_ML_PRNG_PARAMETER;
    }
    /* Out of Range(OperationMode) */
    else if (ANUM_TRN < trng_size)
    {
        ret_err = ER_ML_PRNG_PARAMETER;
    }
    /* During seed generation */
    else if (D_TRNG_GENERATING == trng_state)
    {
        ret_err = ER_ML_PRNG_STATUS;
    }
    /* Status Check OK */
    else
    {
        (void)D_TRNG_CheckTRN(trng_size, &chk_result);
        if (D_TRNG_FULL != chk_result)
        {
            ret_err = D_TRNG_GenerateSeed(test_mode, trng_size);
            if (ER_D_TRNG_OK == ret_err)
            {
                ret_err = ER_IN_PROCESS;
            }
            else
            {
                ret_err = ER_ML_PRNG_PARAMETER;
            }
        }
        else
        {
            ret_err = ER_ML_PRNG_OK;
        }
    }

    return ret_err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_PRNG_0002]:[ICUMFW_UD_ML_PRNG_0002]
* Function name: ML_PRNG_InitSHE
* Description  : Pseudo random number generator initialization
* Arguments    : OperationMode (Pseudo random number generator initialization type)
*              : p_prng_Callback (Callback function pointer at end of seed generation)
*              : p_prng_Init 
*              : (Pseudo random number generator initialization structure pointer)
* Return Value : ER_ML_PRNG_OK
*              : ER_ML_PRNG_SEED
*              : ER_ML_PRNG_TIMEOUT
*              : ER_ML_PRNG_TRN
*              : ER_ML_PRNG_STATUS
*              : ER_ML_PRNG_PARAMETER
*              : ER_ML_PRNG_AES_PRM(Debug only)
*              : ER_PENDING
***************************************************************************************/
uint32_t ML_PRNG_InitSHE(const uint32_t* p_EntropySecret_ptr)
{
    const uint32_t c_prng_key[ANUM_PER_BLK] =
    {
        0x48530401uL, 0x00800045uL, 0x00000000uL, 0xB0000000uL
    };
    const uint32_t *p_prng_InputMP[ANUM_MP_NOPAD];
    uint32_t trng_state;
    uint32_t ret_err;

    ret_err = ER_ML_PRNG_OK;

    /* TRN error */
    (void)D_TRNG_GetState(&trng_state);
    if (D_TRNG_TEST_ERR == trng_state)
    {
        ret_err = ER_ML_PRNG_TRN;
    }
    /* During seed generation */
    else if (D_TRNG_GENERATING == trng_state)
    {
        ret_err = ER_ML_PRNG_STATUS;
    }
    /* Status Check OK */
    else
    {
        /* Crypto operation */
        ret_err = ml_PRNG_CheckParameter(p_EntropySecret_ptr);
        if (ER_ML_PRNG_PARAMETER == ret_err)
        {
            /* Do nothing */
        }
        else
        {
            /* EXTEND_SEED not done */
            g_state_s.cmplt_extend_seed = OFF;
            (void)D_TRNG_GetTRN(g_state_s.prng_state, NUM1 * ANUM_PER_BLK);
            /* PRNG_KEY = KDF(SECRET_KEY, PRNG_KEY_C) */
            p_prng_InputMP[NUM0] = p_EntropySecret_ptr;
            p_prng_InputMP[NUM1] = c_prng_key;
            ret_err = ml_PRNG_OperateAESMP(p_prng_InputMP, g_state_s.prng_key, ANUM_MP_NOPAD);
            if (ER_ML_PRNG_OK == ret_err)
            {
                (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_RND_INIT, NUM1);
            }
            else
            {
                /* Do nothing */
            }
        }
    }

    return ret_err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_PRNG_0010]:[ICUMFW_UD_ML_PRNG_0010]
* Function name: ML_PRNG_ExtendSHE
* Description  : Extend seed of pseudo random number
* Arguments    : p_prng_Entropy (128-bit entropy input source pointer)
* Return Value : ER_ML_PRNG_OK
*              : ER_ML_PRNG_TIMEOUT
*              : ER_ML_PRNG_STATUS
*              : ER_ML_PRNG_PARAMETER
*              : ER_ML_PRNG_AES_PRM(Debug only)
*              : ER_PENDING
***************************************************************************************/
uint32_t ML_PRNG_ExtendSHE(const uint32_t *p_prng_Entropy)
{
    const uint32_t c_prng_extension[ANUM_PER_BLK] =
    {
        0x00000080uL, 0x00000000uL, 0x00000000uL, 0x00010000uL
    };
    const uint32_t *p_prng_InputMP[ANUM_MP_PAD];
    uint32_t ret_err;
    uint32_t get_status;
    uint32_t trng_state;

    ret_err = ER_ML_PRNG_OK;

    (void)D_COMM_GetICUStatusInfo(&get_status);
    (void)D_TRNG_GetState(&trng_state);
    /* Out of Range(p_prng_Entropy) */
    if (NUM0 != (ALIGN4 & (uint32_t)p_prng_Entropy))
    {
        ret_err = ER_ML_PRNG_PARAMETER;
    }
    /* During seed generation */
    else if ((D_TRNG_GENERATING == trng_state)
    /* Uninitialized */
    || (NUM0 == (ICUM_COMM_INF_ICU_ST_RND_INIT & get_status)))
    {
        ret_err = ER_ML_PRNG_STATUS;
    }
    else
    {
        /* PRNG_STATE = AES-MP(PRNG_STATE|ENTROPY) */
        p_prng_InputMP[NUM0] = g_state_s.prng_state;
        p_prng_InputMP[NUM1] = p_prng_Entropy;
        p_prng_InputMP[NUM2] = c_prng_extension;
        ret_err = ml_PRNG_OperateAESMP(p_prng_InputMP, g_state_s.prng_state, ANUM_MP_PAD);
        if (ER_ML_PRNG_OK == ret_err)
        {
            /* EXTEND_SEED done */
            g_state_s.cmplt_extend_seed = ON;
        }
        else
        {
            /* do nothing */
        }
        /* PRNG_SEED = AES-MP(PRNG_SEED|ENTROPY) -> None(PRNG_SEED is not used) */
    }
    return ret_err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_PRNG_0011]:[ICUMFW_UD_ML_PRNG_0011]
* Function name: ML_PRNG_GenSHE
* Description  : Pseudo random number generation
* Arguments    : p_prng_DataOut (Pointer to pseudo random number or 
*              : pseudo random number generation ready status output destination)
* Return Value : ER_ML_PRNG_OK
*              : ER_ML_PRNG_TIMEOUT
*              : ER_ML_PRNG_RESEED
*              : ER_ML_PRNG_STATUS
*              : ER_ML_PRNG_PARAMETER
*              : ER_ML_PRNG_AES_PRM(Debug only)
*              : ER_PENDING
***************************************************************************************/
uint32_t ML_PRNG_GenSHE(uint32_t *p_prng_DataOut)
{
    uint32_t ret_err;
    uint32_t get_status;
    uint32_t trng_state;

    ret_err = ER_ML_PRNG_OK;

    (void)D_COMM_GetICUStatusInfo(&get_status);
    (void)D_TRNG_GetState(&trng_state);
    /* Out of Range(p_prng_DataOut) */
    if ((NULL == p_prng_DataOut) 
        || (NUM0 != (ALIGN4 & (uint32_t)p_prng_DataOut)))
    {
        ret_err = ER_ML_PRNG_PARAMETER;
    }
    else if ((D_TRNG_GENERATING == trng_state)
    /* Uninitialized */
        || (NUM0 == (ICUM_COMM_INF_ICU_ST_RND_INIT & get_status)))
    {
        ret_err = ER_ML_PRNG_STATUS;
    }
    /* Initialized(SHE type) */
    else
    {
        ret_err = ml_PRNG_GenerateRN(p_prng_DataOut);
    }
    return ret_err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_PRNG_0015]:[ICUMFW_UD_ML_PRNG_0015]
* Function name: ML_PRNG_Gen
* Description  : Pseudo random number generation
* Arguments    : p_prng_DataOut (Pointer to pseudo random number or 
*                pseudo random number generation ready status output destination)
* Return Value : ER_ML_PRNG_OK
*              : ER_ML_PRNG_TIMEOUT
*              : ER_ML_PRNG_RESEED
*              : ER_ML_PRNG_STATUS
*              : ER_ML_PRNG_PARAMETER
*              : ER_ML_PRNG_AES_PRM(Debug only)
*              : ER_PENDING
***************************************************************************************/
uint32_t ML_PRNG_Gen(uint32_t *p_prng_DataOut)
{
    uint32_t ret_err;
    uint32_t get_status;
    uint32_t trng_state;

    ret_err = ER_ML_PRNG_OK;

    (void)D_COMM_GetICUStatusInfo(&get_status);
    (void)D_TRNG_GetState(&trng_state);
    /* Out of Range(p_prng_DataOut) */
    if ((NULL == p_prng_DataOut) 
        || (NUM0 != (ALIGN4 & (uint32_t)p_prng_DataOut)))
    {
        ret_err = ER_ML_PRNG_PARAMETER;
    }
    else if (D_TRNG_GENERATING == trng_state)
    {
        ret_err = ER_ML_PRNG_STATUS;
    }
    /* Initialized(SHE type) */
    else if ((NUM0 != (ICUM_COMM_INF_ICU_ST_RND_INIT & get_status)) 
            && (OFF != g_state_s.cmplt_extend_seed))
    {
        ret_err = ml_PRNG_GenerateRN(p_prng_DataOut);
    }
        else if (OFF != g_state_cd.cmplt_instantiate)
    {
        /* Do nothing */
    }
    else
    {
        ret_err = ER_ML_PRNG_STATUS;
    }
    return ret_err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_PRNG_0016]:[ICUMFW_UD_ML_PRNG_0016]
* Function name: ML_PRNG_IsInitialized
* Description  : Pseudo random number generation
* Arguments    : OperationMode (Pseudo random number generation type)
*              : DataOutSize (Number of pseudo random number output blocks)
*              : p_prng_DataOut (Pointer to pseudo random number or 
*              : pseudo random number generation ready status output destination)
* Return Value : ER_ML_PRNG_OK
*              : ER_ML_PRNG_PARAMETER
***************************************************************************************/
uint32_t ML_PRNG_IsInitialized(uint32_t *p_prng_state)
{
    uint32_t ret_err;
    uint32_t get_status;

    ret_err = ER_ML_PRNG_OK;

    (void)D_COMM_GetICUStatusInfo(&get_status);
    /* Out of Range(p_prng_DataOut) */
    if ((NULL == p_prng_state) 
        || (NUM0 != (ALIGN4 & (uint32_t)p_prng_state)))
    {
        ret_err = ER_ML_PRNG_PARAMETER;
    }
    else
    {
        if (((NUM0 != (ICUM_COMM_INF_ICU_ST_RND_INIT & get_status)) 
            && (OFF != g_state_s.cmplt_extend_seed)) 
            || (OFF != g_state_cd.cmplt_instantiate))
        {
            *p_prng_state = ML_PRNG_INIT_COMPLETE;
        }
        else
        {
            *p_prng_state = ML_PRNG_INIT_NOT_COMPLETE;
        }
        ret_err = ER_ML_PRNG_OK;
    }
    return ret_err;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_ML_PRNG_0017]:[ICUMFW_UD_ML_PRNG_0017]
* Function name: ML_PRNG_GetTrngState
* Description  : Get TRNG status
* Arguments    : p_trng_state 
* Return Value : ER_ML_PRNG_OK
***************************************************************************************/
uint32_t ML_PRNG_GetTrngState(uint32_t* p_trng_state)
{
    uint32_t trng_state;
    (void)D_TRNG_GetState(&trng_state);
    if (NULL != p_trng_state)
    {
        if (D_TRNG_TEST_ERR == trng_state)
        {
            *p_trng_state = ML_PRNG_TRN_TEST_ERR;
        }
        else if (D_TRNG_GENERATING == trng_state)
        {
            *p_trng_state = ML_PRNG_TRN_GENERATING;
        }
        else
        {
            *p_trng_state = ML_PRNG_TRN_NOT_GENERATING;
        }
    }
    else
    {
        /* do nothing */
    }
    return ER_ML_PRNG_OK;
}

