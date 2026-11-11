/******************************************************************************/
/* Component Name   ICU-M Crypto AES Driver                                   */
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

/**************************************************************************************/
/* Headers                                                                            */
/**************************************************************************************/
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_flash_pub.h"
#include "icum_d_cry_aes_pub.h"
#include "icum_d_cry_aes.h"
#include "icum_d_ost_pub.h"
/* Include prototypes of interrupt handlers */
#include "icum_int_declaration_pub.h"

/**************************************************************************************/
/* Global Variables                                                                   */
/**************************************************************************************/
#define NUM1                           (1u)
#define NUM2                           (2u)
#define NUM3                           (3u)
#define NUM4                           (4u)
#define NUM5                           (5u)
#define NUM12                         (12u)
#define NUM13                         (13u)
#define NUM14                         (14u)
#define NUM15                         (15u)

/* align size */
#define ALIGN4          (uint32_t)(0x00000003u)
#define ALIGN8          (uint32_t)(0x00000007u)
#define ALIGN256        (uint32_t)(0x000000FFu)

/* used context#2 */
uint32_t g_ctx2_state;
/* aes driver control parameter */
static aes_drv_ctrl_t g_aes_drv_ctrl = {D_AES_STS_NONE, 0u, 0u};

uint32_t g_aes_dtf_flag = FLG_OFF;

/* Key restriction status */
static aes_key_restriction_t g_key_restriction = {0u, D_AES_KEY_SIZE_IN_BYTES_128BIT, {0u}};

init_callback_t *g_p_aes_callbackFunc = NULL;

static uint32_t g_aes_gen_key_length_id;

uint32_t g_aes_atonce_id_use_mng;

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
/* AES(ECB,CBC,CFB,OFB,CTR,MP,CMAC,XTS) Function */
static        uint32_t d_AES_SetParam(aes_setparam_t* p_param);
/* AES(ECB,CBC,CFB,OFB,CTR,MP,CMAC) Function */
static        uint32_t d_AES_ExecutionAtOnceIcup(aes_all_at_once_t* p_param);
/* AES(ECB,CBC,CFB,OFB,CTR,MP) Function */
static        uint32_t d_AES_ProcessNBlocks(aes_nblocks_t* p_param);
static inline uint32_t d_AES_Cipher1Block(aes_nblocks_t* p_param, const uint32_t nb_rem);
/* AES(XTS) Function */
static        uint32_t d_AES_ExecutionAtOnceXtsIcup(aes_all_at_once_xts_t* p_param);
static        uint32_t d_AES_ProcessNBlocks_XTS(aes_nblocks_t* p_param);
static        uint32_t d_AES_SetTweakData(const uint32_t* p_aes_tweak);
static        uint32_t d_AES_XtsStream(aes_nblocks_t* p_param);
static inline uint32_t d_AES_Xts1Block(aes_nblocks_t* p_param);
static        uint32_t d_AES_XtsLastStream(aes_nblocks_t* p_param);
/* AES(GCM) Function */
static        uint32_t d_AES_ExecutionAtOnceGcmIcup(aes_all_at_once_gcm_t* p_param);
static        uint32_t d_AES_SetParam_GCM(const aes_gcm_setparam_t* p_param);
static        uint32_t d_AES_GetGHash(uint32_t* p_gcm_ghash);
static        uint32_t d_AES_ProcessNBlocks_IV(aes_gcm_iv_t* p_param);
static        uint32_t d_AES_GcmIvStream(aes_gcm_iv_t* p_param);
static inline uint32_t d_AES_GcmIv1Block(aes_gcm_iv_t* p_param);
static        uint32_t d_AES_GcmIvLastStream(aes_gcm_iv_t* p_param);
static        uint32_t d_AES_ProcessNBlocks_AAD(aes_gcm_aad_t* p_param);
static inline uint32_t d_AES_GcmAAD1Block(aes_gcm_aad_t* p_param);
static        uint32_t d_AES_ProcessNBlocks_GCM(aes_gcm_t* p_param);
static        uint32_t d_AES_GcmStream(aes_gcm_t* p_param);
static inline uint32_t d_AES_Gcm1Block(aes_gcm_t* p_param);
static        uint32_t d_AES_GcmLastStream(aes_gcm_t* p_param);
static        uint32_t d_AES_GetAuthTag(aes_gcm_tag_t* p_param);
/* AES(CCM) Function */
static        uint32_t d_AES_ExecutionAtOnceCcmIcup(aes_all_at_once_ccm_t* p_param);
static        uint32_t d_AES_ExecutionAtOnceCcmIcupEnc(aes_all_at_once_ccm_t *p_param,
                                                       aes_update_ccm_t *p_param_update,
                                                       uint32_t nb_blocks_auth, uint16_t block_info, uint8_t rem_auth,
                                                       uint32_t nb_blocks, uint8_t rem);
static        uint32_t d_AES_ExecutionAtOnceCcmIcupDec(aes_all_at_once_ccm_t *p_param,
                                                       aes_update_ccm_t *p_param_update,
                                                       uint32_t nb_blocks_auth, uint16_t block_info, uint8_t rem_auth,
                                                       uint32_t nb_blocks, uint8_t rem);
static        uint32_t d_AES_SetParam_CCM(const aes_ccm_setparam_t* p_param);
static        uint32_t d_AES_SetAssociatedData(aes_ccm_a_t* p_param);
static        uint32_t d_AES_ProcessNBlocks_CCM(aes_ccm_t* p_param);
static        uint32_t d_AES_SetPayloadData(aes_ccm_p_t* p_param);
static        uint32_t d_AES_GetAuthTag_CcmEnc(uint32_t* p_auth_tag);
static        uint32_t d_AES_GetAuthTag_CcmDec(aes_ccm_tag_dec_t* p_param);
static        uint32_t d_AES_Cancel(void);
/* AES(CMAC) Function */
static        uint32_t d_AES_ProcessNBlocks_CMAC(aes_nblocks_t* p_param);
static        uint32_t d_AES_Cmac(aes_nblocks_t* p_param, const uint32_t* p_aes_key, uint16_t key_length);
static        uint32_t d_AES_CmacFirstStream(aes_nblocks_t* p_param);
static        uint32_t d_AES_CmacMidStream(aes_nblocks_t* p_param);
static inline uint32_t d_AES_Cmac1Block(aes_nblocks_t* p_param);
static        uint32_t d_AES_CmacLastStream(aes_nblocks_t* p_param);
/* AES Common Function */
static        uint32_t d_AES_GetIV(uint32_t* p_aes_iv);
static        uint32_t d_AES_GetKeyRestriction(uint16_t key_length, uint32_t* p_key_length_id);
inline        uint32_t d_AES_Cipher(const uint32_t* p_aes_src, uint32_t* p_aes_dest);
static inline uint32_t d_AES_WaitIdleState(const uint32_t reg_item, const uint32_t param);
inline        uint32_t d_AES_CipherDummyRead(const uint32_t* p_aes_src);
inline        uint32_t d_AES_CipherNoRead(const uint32_t* p_aes_src);
#if defined(TARGET_DEVICE_S4H) || defined(TARGET_DEVICE_U2B) || defined(TARGET_DEVICE_U2C) || defined(TARGET_DEVICE_U2C4)
static inline void d_AES_IsrCmpErr(void);
#endif
static        uint32_t d_AES_CheckMemory(const uint32_t start_addr, const uint32_t size, uint8_t *p_result);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0001]:[ICUMFW_UD_S4_D_AES_0001]
* Function name: D_AES_Init
* Description  : Initialization and initial value setting.
* Arguments    : p_callback : address of callback function
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_AES_Init(init_callback_t* p_callback)
{
    uint32_t err;
    uint32_t count_nb;

    err = ER_OK;
    count_nb = D_AES_ZERO_U32;

    if (NULL == p_callback)
    {
        g_p_aes_callbackFunc = NULL;
        err = ER_PARAM;
    }
    else
    {
        /* Set the initial value to callback function. */
        g_p_aes_callbackFunc = p_callback;
        /* Set ICUM_AESCTRL register */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
        UNLOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(RST0)) | (PRM_RST0_INIT);
        D_AES_WAIT_REG_VALUE(PRM_RST0_INIT, ICUM_AESCTRL, RST0);
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(RST1)) | (PRM_RST1_INIT);
        D_AES_WAIT_REG_VALUE(PRM_RST1_INIT, ICUM_AESCTRL, RST1);
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(RST2)) | (PRM_RST2_INIT);
        D_AES_WAIT_REG_VALUE(PRM_RST2_INIT, ICUM_AESCTRL, RST2);
        LOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
        /* Set ICUM_AESDMACLKRATE register */
        ICUM_AESDMACLKRATE = DMA_DATA;
#else
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(RST0 | RST1 | RST2)) | (PRM_RST0_INIT | PRM_RST1_INIT | PRM_RST2_INIT);
#endif
        /* Set the initial value to context#2. */
        g_ctx2_state = NO_USE_CTX2;
        /* Set the initial value to Key restriction status */
        g_key_restriction.key_length = D_AES_KEY_SIZE_IN_BYTES_128BIT;
        g_key_restriction.counter = D_AES_ZERO_U16;
        g_aes_gen_key_length_id = D_AES_ZERO_U32;
        g_aes_atonce_id_use_mng = D_AES_ZERO_U32;
        /* Check key length id */
        for(count_nb = D_AES_ZERO_U32; (uint32_t)D_AES_KEYLEN_ID_USE_MNG > count_nb; count_nb++)
        {
            g_key_restriction.key_length_id[count_nb] = D_AES_ZERO_U32;
        }
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0032]:[ICUMFW_UD_S4_D_AES_0032]
* Function name: d_AES_SetParam
* Description  : Set parameters a specified context.
* Arguments    : cipher_mode : cipher mode
*                              Range: D_AES_CIPHER_MODE_ECB
*                                     D_AES_CIPHER_MODE_CBC
*                                     D_AES_CIPHER_MODE_CTR
*                                     D_AES_CIPHER_MODE_CFB
*                                     D_AES_CIPHER_MODE_OFB
*                                     D_AES_CIPHER_MODE_XTS
*                                     D_AES_CIPHER_MODE_CMAC
*                                     D_AES_CIPHER_MODE_MP
*                direction   : cipher direction
*                              Range: D_AES_DIR_ENCRYPTION
*                                     D_AES_DIR_DECRYPTION
*                key_type    : key type
*                              Range: D_AES_KEYTYPE_PLAIN
*                                     D_AES_KEYTYPE_ROMKEY_0
*                first       : first flag
*                              Range: D_AES_NOT_FIRST_CIPHER
*                                     D_AES_FIRST_CIPHER
*                p_aes_key   : address of key value
*                p_aes_iv    : address of IV value
* Return Value : ER_OK
***************************************************************************************/
static uint32_t d_AES_SetParam(aes_setparam_t* p_param)
{
    /* Context #0 only */
    uint32_t iv_flag;
    uint32_t saved_psw;
    uint32_t keysel;
    uint32_t i;
    uint32_t index;
    volatile uint32_t *p_key_reg[D_AES_KEY_NUM];

    iv_flag = D_AES_ZERO_U32;
    saved_psw = D_AES_ZERO_U32;
    keysel = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    if (D_AES_FIRST_CIPHER == (uint32_t)p_param->first)
    {
        /* Set execute and first status */
        g_aes_drv_ctrl.status = (D_AES_STS_EXECUTE | D_AES_STS_FIRST);
    }
    else
    {
        /* Set execute and suspend status */
        g_aes_drv_ctrl.status = (D_AES_STS_EXECUTE | D_AES_STS_SUSPEND);
    }

    /* Set ICUM_AESKEY register */
    if (D_AES_KEY_SIZE_IN_BYTES_128BIT == (uint32_t)p_param->key_length)
    {   /* 128bit */
        keysel = D_AES_SET_KEYSEL_0;
    }
    else if (D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)p_param->key_length)
    {   /* 192bit */
        keysel = D_AES_SET_KEYSEL_1;
    }
    else
    {   /* 256bit */
        keysel = D_AES_SET_KEYSEL_2;
    }
    ICUM_AESKEY = (ICUM_AESKEY & ~(KEYSEL)) | keysel;

    /* Set ICUM_AESCTRL register */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    UNLOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
#endif
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(RST0)) | PRM_RST0_INIT;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    D_AES_WAIT_REG_VALUE(PRM_RST0_INIT, ICUM_AESCTRL, RST0);
    LOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
#endif

    /* Set ICUM_AESCCTL0 register */
    ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(MOD0 | ENCMODE0 | K0SEL)) |
                      ((uint32_t)p_param->cipher_mode
                    | (ENCMODE0 & ((uint32_t)p_param->direction << SHIFT_ENCMODE0))
                    | (K0SEL & ((uint32_t)p_param->key_type << SHIFT_K0SEL)));
    /* Set ICUM_AESCTRL register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST | CTX | BYTE_ORDER | DATREGSW)) |
                     (PRM_IDATDEST_KEY
                    | PRM_CTX_0
                    | PRM_BYTE_ORDER_APPLY
                    | PRM_DATREGSW_DIDI_DODI);

    if (D_AES_KEYTYPE_PLAIN == (uint32_t)p_param->key_type)
    {
        /* Set ICUM_AESKEY register */
        ICUM_AESKEY = (ICUM_AESKEY & ~(EX_KEY)) | PRM_EX_KEY_SET_127_0;

       /* Start at a random index */
        D_OST_GetHardCounter(&index);
        index %= D_AES_KEY_NUM;

        /* Set registers for key input */
        *(p_key_reg + ADDR_0TH) = (volatile uint32_t *)&ICUM_AESDID0;
        *(p_key_reg + ADDR_1ST) = (volatile uint32_t *)&ICUM_AESDID1;
        *(p_key_reg + ADDR_2ND) = (volatile uint32_t *)&ICUM_AESDID2;
        *(p_key_reg + ADDR_3RD) = (volatile uint32_t *)&ICUM_AESDID3;

        /* Write to ICUM_AESDIDi register *//* XTS does not support 192bit */
        /* key length 128 or 192 or 256[bit] */
        for (i = D_AES_ZERO_U32; i < D_AES_KEY_NUM; i++)
        {
            *(p_key_reg[index]) = p_param->p_aes_key[index];
            index = (index + 1U) % D_AES_KEY_NUM;
        }

        /* key length 192 or 256[bit] */
        if ((D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)p_param->key_length)
         || (D_AES_KEY_SIZE_IN_BYTES_256BIT == (uint32_t)p_param->key_length))
        {
            /* Set ICUM_AESKEY register */
            ICUM_AESKEY = (ICUM_AESKEY & ~(EX_KEY)) | PRM_EX_KEY_SET_256_128;

            /* Write to ICUM_AESDIDi register for key length = 192bit */
            ICUM_AESDID0 = *(p_param->p_aes_key + ADDR_4TH);
            ICUM_AESDID1 = *(p_param->p_aes_key + ADDR_5TH);

            /* key length 256[bit] */
            if (D_AES_KEY_SIZE_IN_BYTES_256BIT == (uint32_t)p_param->key_length)
            {

                /* Write to ICUM_AESDIDi register for key length = 256bit */
                ICUM_AESDID2 = *(p_param->p_aes_key + ADDR_6TH);
                ICUM_AESDID3 = *(p_param->p_aes_key + ADDR_7TH);
            }
            else
            {
                /* Do nothing */
            }
        } /* end of if ((D_AES_KEY_SIZE_IN_BYTES_192,256BIT == p_param->key_length) */
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }
    /* Use iv? */
    switch ((uint32_t)p_param->cipher_mode)
    {
        case D_AES_CIPHER_MODE_ECB:
            iv_flag = NO_USE_IV;
            break;
        case D_AES_CIPHER_MODE_CMAC:
        case D_AES_CIPHER_MODE_XTS:
            if (D_AES_FIRST_CIPHER == (uint32_t)p_param->first)
            {
                iv_flag = NO_USE_IV;
            }
            else
            {
                iv_flag = USE_IV;
            }
            break;
        default:
            iv_flag = USE_IV;
            break;
    } /* end of switch ((uint32_t)p_param->cipher_mode) */

    if (USE_IV == iv_flag)
    {
        /* Set ICUM_AESCTRL register IDATDEST = 10: Initialization vector/counter register */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) | PRM_IDATDEST_IV_CTR;
        /* Write to ICUM_AESDIDi register */
        ICUM_AESDID0 = *(p_param->p_aes_iv + ADDR_0TH);
        ICUM_AESDID1 = *(p_param->p_aes_iv + ADDR_1ST);
        ICUM_AESDID2 = *(p_param->p_aes_iv + ADDR_2ND);
        ICUM_AESDID3 = *(p_param->p_aes_iv + ADDR_3RD);
    }
    else
    {
        /* Do nothing */
    }

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0067]:[ICUMFW_UD_S4_D_AES_0067]
* Function name: d_AES_GetIV
* Description  : Get intermediate value current context.
* Arguments    : p_aes_iv : address of IV value
* Return Value : ER_OK
***************************************************************************************/
static uint32_t d_AES_GetIV(uint32_t* p_aes_iv)
{
    uint32_t saved_psw;

    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Set ICUM_AESCTRL register ODATSRC = 00: Block Cipher Output register (BCO) */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC)) | PRM_ODATSRC_BCO;
    /* Dummy read from ICUM_AESDODi register */
    ICUM_AESDOD0;
    ICUM_AESDOD1;
    ICUM_AESDOD2;
    ICUM_AESDOD3;
    /* Dummy read from ICUM_AESDODi register */
    ICUM_AESDOD0;
    ICUM_AESDOD1;
    ICUM_AESDOD2;
    ICUM_AESDOD3;
    /* Set ICUM_AESCTRL register ODATSRC = 10: Initialization vector/counter register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC)) | PRM_ODATSRC_IV_CTR;
    /* Read from ICUM_AESDODi register */
    *(p_aes_iv + ADDR_0TH) = ICUM_AESDOD0;
    *(p_aes_iv + ADDR_1ST) = ICUM_AESDOD1;
    *(p_aes_iv + ADDR_2ND) = ICUM_AESDOD2;
    *(p_aes_iv + ADDR_3RD) = ICUM_AESDOD3;

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0014]:[ICUMFW_UD_S4_D_AES_0014]
* Function name: d_AES_ProcessNBlocks
* Description  : Perform cipher processing.(Other than GCM, XTS, CCM, CMAC)
* Arguments    : number     : Number of blocks to be encrypted
*                             Range: 0x1 - 0xFFFFFFFF
*                block_info : block information
*                             Range: D_AES_BLOCK_INFO_FIRST
*                p_aes_src  : address of input value
*                p_aes_dest : address of output value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_ProcessNBlocks(aes_nblocks_t* p_param)
{
    uint32_t err;
    uint32_t calc_cnt;

    err = ER_OK;
    calc_cnt = D_AES_ZERO_U32;

    /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);
    ASM_MACRO(".align 4");
    for (calc_cnt = p_param->number; D_AES_NUMBER_MIN <= calc_cnt; calc_cnt--)
    {
        err = d_AES_Cipher1Block(p_param, calc_cnt);
        if (ER_OK == err)
        {
            /* Do nothing */
        }
        else
        {
            break;
        }
    }

    /* Clear control status */
    g_aes_drv_ctrl.status = D_AES_STS_NONE;

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0015]:[ICUMFW_UD_S4_D_AES_0015]
* Function name: d_AES_Cipher1Block
* Description  : Perform cipher processing.(Other than GCM, XTS, CCM, CMAC)
* Arguments    : p_aes_src  : address of input value
*                p_aes_dest : address of output value
*                nb_rem     : number of remaining blocks
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static inline uint32_t d_AES_Cipher1Block(aes_nblocks_t* p_param, const uint32_t nb_rem)
{
    uint32_t err;
    uint32_t saved_psw;

    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* New operation, and first of data process */
    if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status))
    {
        /* Clear first or suspend status */
        g_aes_drv_ctrl.status &= ~(D_AES_STS_FIRST | D_AES_STS_SUSPEND);
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_NORMAL
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_USE);
    }
    /* The resumption operation of a suspended operation */
    else if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
    {
        /* Clear suspend status */
        g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_NORMAL
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_RESUME
                        | PRM_NEW_KEY_USE);
    }
    /* The continuous operation of data process */
    else
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_NORMAL
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_NO_USE);
    }

    if ((PRM_MOD0_MP == (MOD0 & ICUM_AESCCTL0)) && (D_AES_NUMBER_MIN != nb_rem))
    {
        err = d_AES_CipherDummyRead(p_param->p_aes_src);
        p_param->p_aes_src += AES_BLOCK_SIZE;
    }
    else
    {
        err = d_AES_Cipher(p_param->p_aes_src, p_param->p_aes_dest);
        p_param->p_aes_src += AES_BLOCK_SIZE;
        p_param->p_aes_dest += AES_BLOCK_SIZE;
    }

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0022]:[ICUMFW_UD_S4_D_AES_0022]
* Function name: d_AES_SetTweakData
* Description  : Set tweak value.
* Arguments    : p_aes_tweak : address of tweak value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_SetTweakData(const uint32_t* p_aes_tweak)
{
    uint32_t err;
    uint32_t saved_psw;

    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);
    /* Set ICUM_AESDCTL register */
    ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                     (PRM_ATTR_XTS_TWEAK
                    | PRM_NO_LAST_BLOCK
                    | PRM_REM_128BIT
                    | PRM_IVSEL_USE
                    | PRM_CONT_NEW_CONTINUE
                    | PRM_NEW_KEY_USE);
    /* Execute AES cipher */
    err = d_AES_CipherDummyRead(p_aes_tweak);

    /* Clear control status */
    g_aes_drv_ctrl.status = D_AES_STS_NONE;

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0028]:[ICUMFW_UD_S4_D_AES_0028]
* Function name: d_AES_ProcessNBlocks_XTS
* Description  : Perform cipher processing in the XTS mode.
* Arguments    : number     : Number of blocks to be encrypted or decrypted
*                             Range: 0x1 - 0xFFFFFFFF
*                block_info : block information
*                             Range: D_AES_BLOCK_INFO_LAST
*                                    D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_aes_src  : address of input value
*                p_aes_dest : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_ProcessNBlocks_XTS(aes_nblocks_t* p_param)
{
    uint32_t err;

    err = ER_OK;

    /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);
    err = d_AES_XtsStream(p_param);

    if ((ER_OK == err)
    && (D_AES_BLOCK_INFO_LAST ==
        (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->block_info)))
    {
        err = d_AES_XtsLastStream(p_param);
    }
    else
    {
        /* Do nothing */
    }

    /* Clear control status */
    g_aes_drv_ctrl.status = D_AES_STS_NONE;

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0029]:[ICUMFW_UD_S4_D_AES_0029]
* Function name: d_AES_XtsStream
* Description  : Perform encryption processing in the XTS mode. (Not last block)
* Arguments    : number     : Number of blocks to be encrypted or decrypted
*                             Range: 0x1 - 0xFFFFFFFF
*                block_info : block information
*                             Range: D_AES_BLOCK_INFO_LAST
*                p_aes_src  : address of input value
*                p_aes_dest : address of output value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_XtsStream(aes_nblocks_t* p_param)
{
    uint32_t err;
    uint32_t calc_cnt;

    err = ER_OK;
    calc_cnt = D_AES_ZERO_U32;

    /* Calculate the number of repetitions of processing. */
    if (D_AES_BLOCK_INFO_LAST != (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->block_info))
    {
        calc_cnt = p_param->number;
    }
    /* Calculate the more than 2 blocks.(include the last block) */
    else if ((uint32_t)NUM_PRECEDING_BLOCK < (uint32_t)p_param->number)
    {
        calc_cnt = p_param->number - NUM_PRECEDING_BLOCK;
    }
    /* Calculate the last block only. */
    else
    {
        calc_cnt = END_CALC;
    }
    ASM_MACRO(".align 4");
    for ( ; END_CALC != calc_cnt; calc_cnt--)
    {
        /* Execute AES cipher */
        err = d_AES_Xts1Block(p_param);
        if (ER_OK == err)
        {
            p_param->p_aes_src += AES_BLOCK_SIZE;
            p_param->p_aes_dest += AES_BLOCK_SIZE;
        }
        else
        {
            break;
        }
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0030]:[ICUMFW_UD_S4_D_AES_0030]
* Function name: d_AES_Xts1Block
* Description  : Perform encryption processing in the XTS mode. (Not last stream)
* Arguments    : p_aes_src  : address of input value
*                p_aes_dest : address of output value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_Xts1Block(aes_nblocks_t* p_param)
{
    uint32_t err;
    uint32_t saved_psw;

    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* The resumption operation of a suspended operation */
    if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
    {
        /* Clear suspend status */
        g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_XTS_TEXT
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_RESUME
                        | PRM_NEW_KEY_USE);
    }
    /* The continuous operation of data process */
    else
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_XTS_TEXT
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_NO_USE);
    }

    /* Execute AES cipher */
    err = d_AES_Cipher(p_param->p_aes_src, p_param->p_aes_dest);

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0031]:[ICUMFW_UD_S4_D_AES_0031]
* Function name: d_AES_XtsLastStream
* Description  : Perform encryption processing in the XTS mode. (Last stream)
* Arguments    : number     : Number of blocks to be encrypted or decrypted
*                             Range: 0x1 - 0xFFFFFFFF
*                block_info : block information
*                             Range: D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_aes_src  : address of input value
*                p_aes_dest : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_XtsLastStream(aes_nblocks_t* p_param)
{
    uint32_t err;
    uint32_t output[AES_BLOCK_SIZE] = {D_AES_ZERO_U32};
    uint32_t input[AES_BLOCK_SIZE] = {D_AES_ZERO_U32};
    uint16_t last_block_size;
    uint32_t saved_psw;

    err = ER_OK;
    last_block_size = D_AES_ZERO_U16;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    if (NUM_PRECEDING_BLOCK <= p_param->number)
    {
        /* Is the length of the last block less than 128 ? */
        if (D_AES_BLOCK_INFO_SIZE_MAX > (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->block_info))
        {
            if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
            {
                /* Clear suspend status */
                g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                                 (PRM_ATTR_XTS_SHORT_LAST
                                | PRM_NO_LAST_BLOCK
                                | PRM_REM_128BIT
                                | PRM_IVSEL_NO_USE
                                | PRM_CONT_RESUME
                                | PRM_NEW_KEY_USE);
            }
            else
            {
                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                                 (PRM_ATTR_XTS_SHORT_LAST
                                | PRM_NO_LAST_BLOCK
                                | PRM_REM_128BIT
                                | PRM_IVSEL_NO_USE
                                | PRM_CONT_NEW_CONTINUE
                                | PRM_NEW_KEY_NO_USE);
            }
            /* Execute AES cipher */
            err = d_AES_Cipher(p_param->p_aes_src, output);
            if (ER_OK == err)
            {
                p_param->p_aes_src += AES_BLOCK_SIZE;
                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                                 (PRM_ATTR_XTS_TEXT
                                | PRM_LAST_BLOCK
                                | (REM & ((uint32_t)p_param->block_info << SHIFT_REM))
                                | PRM_IVSEL_NO_USE
                                | PRM_CONT_NEW_CONTINUE
                                | PRM_NEW_KEY_NO_USE);
                /* calculate last block size */
                last_block_size = (uint16_t)(D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->block_info);
                (void)d_AES_LastBlock(input, p_param->p_aes_src, last_block_size);
                /* Execute AES cipher */
                err = d_AES_Cipher(input, p_param->p_aes_dest);
                if (ER_OK == err)
                {
                    p_param->p_aes_dest += AES_BLOCK_SIZE;
                    /* Read from temporary array  */
                    *(p_param->p_aes_dest + ADDR_0TH) = *(output + ADDR_0TH);
                    *(p_param->p_aes_dest + ADDR_1ST) = *(output + ADDR_1ST);
                    *(p_param->p_aes_dest + ADDR_2ND) = *(output + ADDR_2ND);
                    *(p_param->p_aes_dest + ADDR_3RD) = *(output + ADDR_3RD);
                }
                else
                {
                    /* Do nothing */
                }
            } /* end of if (ER_OK == err) */
            else
            {
                /* Do nothing */
            }
        } /* end of if (D_AES_BLOCK_INFO_SIZE_MAX > (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->block_info)) */
        else
        {
            if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
            {
                /* Clear suspend status */
                g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                                 (PRM_ATTR_XTS_TEXT
                                | PRM_NO_LAST_BLOCK
                                | PRM_REM_128BIT
                                | PRM_IVSEL_NO_USE
                                | PRM_CONT_RESUME
                                | PRM_NEW_KEY_USE);
            }
            else
            {
                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                                 (PRM_ATTR_XTS_TEXT
                                | PRM_NO_LAST_BLOCK
                                | PRM_REM_128BIT
                                | PRM_IVSEL_NO_USE
                                | PRM_CONT_NEW_CONTINUE
                                | PRM_NEW_KEY_NO_USE);
            }
            /* Execute AES cipher */
            err = d_AES_Cipher(p_param->p_aes_src, p_param->p_aes_dest);
            if (ER_OK == err)
            {
                p_param->p_aes_src += AES_BLOCK_SIZE;
                p_param->p_aes_dest += AES_BLOCK_SIZE;
                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                                 (PRM_ATTR_XTS_TEXT
                                | PRM_LAST_BLOCK
                                | PRM_REM_128BIT
                                | PRM_IVSEL_NO_USE
                                | PRM_CONT_NEW_CONTINUE
                                | PRM_NEW_KEY_NO_USE);
                /* Execute AES cipher */
                err = d_AES_Cipher(p_param->p_aes_src, p_param->p_aes_dest);
            } /* end of if (ER_OK == err) */
            else
            {
                /* Do nothing */
            }
        } /* end of else */
    } /* end of if (NUM_PRECEDING_BLOCK <= p_param->number) */
    else
    {
        if (D_AES_BLOCK_INFO_SIZE_MAX == (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->block_info))
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                             (PRM_ATTR_XTS_TEXT
                            | PRM_LAST_BLOCK
                            | PRM_REM_128BIT
                            | PRM_IVSEL_NO_USE
                            | PRM_CONT_RESUME
                            | PRM_NEW_KEY_USE);
            /* Execute AES cipher */
            err = d_AES_Cipher(p_param->p_aes_src, p_param->p_aes_dest);
        }
        else
        {
            /* Dummy read from ICUM_AESDODi register */
            ICUM_AESDOD0;
            ICUM_AESDOD1;
            ICUM_AESDOD2;
            ICUM_AESDOD3;
            /* Dummy read from ICUM_AESDODi register */
            ICUM_AESDOD0;
            ICUM_AESDOD1;
            ICUM_AESDOD2;
            ICUM_AESDOD3;
            err = ER_PARAM;
        } /* end of else */
    } /* end of else */

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0039]:[ICUMFW_UD_S4_D_AES_0039]
* Function name: d_AES_SetParam_GCM
* Description  : Set parameters a specified context for GCM mode.
* Arguments    : direction   : cipher direction
*                              Range: D_AES_DIR_ENCRYPTION
*                                     D_AES_DIR_DECRYPTION
*                first       : first flag
*                              Range: D_AES_NOT_FIRST_CIPHER
*                                     D_AES_FIRST_CIPHER
*                p_gcm_key   : address of key value
*                p_gcm_ghash : address of GHASH value
* Return Value : ER_OK
***************************************************************************************/
static uint32_t d_AES_SetParam_GCM(const aes_gcm_setparam_t* p_param)
{
    /* Context #0 only */
    uint32_t saved_psw;
    uint32_t keysel;
    uint32_t keytype;
    uint32_t i;
    uint32_t index;
    volatile uint32_t *p_key_reg[D_AES_KEY_NUM];

    saved_psw = D_AES_ZERO_U32;
    keysel = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    if (D_AES_FIRST_CIPHER == (uint32_t)p_param->first)
    {
        /* Set execute and first status */
        g_aes_drv_ctrl.status = (D_AES_STS_EXECUTE | D_AES_STS_FIRST);
    }
    else
    {
        /* Set execute and suspend status */
        g_aes_drv_ctrl.status = (D_AES_STS_EXECUTE | D_AES_STS_SUSPEND);
    }

    /* Set ICUM_AESKEY register */
    if (D_AES_KEY_SIZE_IN_BYTES_128BIT == (uint32_t)p_param->key_length)
    {   /* 128bit */
        keysel = D_AES_SET_KEYSEL_0;
    }
    else if (D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)p_param->key_length)
    {   /* 192bit */
        keysel = D_AES_SET_KEYSEL_1;
    }
    else
    {   /* 256bit */
        keysel = D_AES_SET_KEYSEL_2;
    }
    ICUM_AESKEY = (ICUM_AESKEY & ~(KEYSEL)) | keysel;

    if (NULL != p_param->p_gcm_key)
    {
        keytype = PRM_K0SEL_PLAIN;
    }
    else
    {
        keytype = D_AES_KEYTYPE_ROMKEY_1 << SHIFT_K0SEL;
    }

    /* Set ICUM_AESCTRL register */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    UNLOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
#endif
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(RST0)) | PRM_RST0_INIT;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    D_AES_WAIT_REG_VALUE(PRM_RST0_INIT, ICUM_AESCTRL, RST0);
    LOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
#endif

    /* Set ICUM_AESCCTL0 register */
    ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(MOD0 | ENCMODE0 | K0SEL)) |
                      (D_AES_CIPHER_MODE_GCM
                    | (ENCMODE0 & ((uint32_t)p_param->direction << SHIFT_ENCMODE0))
                    | keytype);
    /* Set ICUM_AESCTRL register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST | CTX | BYTE_ORDER | DATREGSW)) |
                     (PRM_IDATDEST_KEY
                    | PRM_CTX_0
                    | PRM_BYTE_ORDER_APPLY
                    | PRM_DATREGSW_DIDI_DODI);

    if (NULL != p_param->p_gcm_key)
    {
        /* Set ICUM_AESKEY register */
        ICUM_AESKEY = (ICUM_AESKEY & ~(EX_KEY)) | PRM_EX_KEY_SET_127_0;

       /* Start at a random index */
        D_OST_GetHardCounter(&index);
        index %= D_AES_KEY_NUM;

        /* Set registers for key input */
        *(p_key_reg + ADDR_0TH) = (volatile uint32_t *)&ICUM_AESDID0;
        *(p_key_reg + ADDR_1ST) = (volatile uint32_t *)&ICUM_AESDID1;
        *(p_key_reg + ADDR_2ND) = (volatile uint32_t *)&ICUM_AESDID2;
        *(p_key_reg + ADDR_3RD) = (volatile uint32_t *)&ICUM_AESDID3;

        /* Write to ICUM_AESDIDi register */
        /* key length 128 or 192 or 256[bit] */
        for (i = D_AES_ZERO_U32; i < D_AES_KEY_NUM; i++)
        {
            *(p_key_reg[index]) = p_param->p_gcm_key[index];
            index = (index + 1U) % D_AES_KEY_NUM;
        }

        /* key length 192 or 256[bit] */
        if ((D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)p_param->key_length)
         || (D_AES_KEY_SIZE_IN_BYTES_256BIT == (uint32_t)p_param->key_length))
        {
            /* Set ICUM_AESKEY register */
            ICUM_AESKEY = (ICUM_AESKEY & ~(EX_KEY)) | PRM_EX_KEY_SET_256_128;

            /* Write to ICUM_AESDIDi register for key length = 192bit */
            ICUM_AESDID0 = *(p_param->p_gcm_key + ADDR_4TH);
            ICUM_AESDID1 = *(p_param->p_gcm_key + ADDR_5TH);

            /* key length 256[bit] */
            if (D_AES_KEY_SIZE_IN_BYTES_256BIT == (uint32_t)p_param->key_length)
            {
                /* Write to ICUM_AESDIDi register for key length = 256bit */
                ICUM_AESDID2 = *(p_param->p_gcm_key + ADDR_6TH);
                ICUM_AESDID3 = *(p_param->p_gcm_key + ADDR_7TH);
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

    if (D_AES_FIRST_CIPHER != (uint32_t)p_param->first)
    {
        /* Set ICUM_AESCTRL register */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) | PRM_IDATDEST_GHASH;
        /* Write to ICUM_AESDIDi register */
        ICUM_AESDID0 = *(p_param->p_gcm_ghash + ADDR_0TH);
        ICUM_AESDID1 = *(p_param->p_gcm_ghash + ADDR_1ST);
        ICUM_AESDID2 = *(p_param->p_gcm_ghash + ADDR_2ND);
        ICUM_AESDID3 = *(p_param->p_gcm_ghash + ADDR_3RD);
    }
    else
    {
        /* Do nothing */
    }

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0040]:[ICUMFW_UD_S4_D_AES_0040]
* Function name: d_AES_GetGHash
* Description  : Get GHASH value.
*                p_gcm_ghash : address of GHASH value
* Return Value : ER_OK
***************************************************************************************/
static uint32_t d_AES_GetGHash(uint32_t* p_gcm_ghash)
{
    uint32_t saved_psw;

    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Set ICUM_AESCTRL register ODATSRC = 00: Block Cipher Output register (BCO) */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC)) | PRM_ODATSRC_BCO;
    /* Dummy read from ICUM_AESDODi register */
    ICUM_AESDOD0;
    ICUM_AESDOD1;
    ICUM_AESDOD2;
    ICUM_AESDOD3;
    /* Dummy read from ICUM_AESDODi register */
    ICUM_AESDOD0;
    ICUM_AESDOD1;
    ICUM_AESDOD2;
    ICUM_AESDOD3;
    /* Set ICUM_AESCTRL register ODATSRC = 11: GHASH register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC)) | PRM_ODATSRC_GHASH;
    /* Read from ICUM_AESDODi register */
    *(p_gcm_ghash + ADDR_0TH) = ICUM_AESDOD0;
    *(p_gcm_ghash + ADDR_1ST) = ICUM_AESDOD1;
    *(p_gcm_ghash + ADDR_2ND) = ICUM_AESDOD2;
    *(p_gcm_ghash + ADDR_3RD) = ICUM_AESDOD3;

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0048]:[ICUMFW_UD_S4_D_AES_0048]
* Function name: d_AES_ProcessNBlocks_IV
* Description  : Process to generate a counter block in the GCM mode.
* Arguments    : iv_number         : Number of blocks to be encrypted or decrypted
*                                    Range: 0x1 - 0xFFFFFFFF
*                iv_block_info     : block information
*                                    Range: D_AES_BLOCK_INFO_FIRST
*                                           D_AES_BLOCK_INFO_LAST
*                                           D_AES_BLOCK_INFO_GCM_IV96
*                                           D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_iv_size_in_bits : address of IV length
*                p_gcm_iv          : address of IV value
*                p_ctr_j0          : address of counter block J0 value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_ProcessNBlocks_IV(aes_gcm_iv_t* p_param)
{
    uint32_t err;
    uint32_t ctr_j_lsb;
    uint32_t saved_psw;

    err = ER_OK;
    ctr_j_lsb = D_AES_ZERO_U32;
    saved_psw = D_AES_ZERO_U32;

    if (D_AES_BLOCK_INFO_GCM_IV96 ==
        (D_AES_BLOCK_INFO_GCM_IV96 & (uint32_t)p_param->iv_block_info))
    {
        ctr_j_lsb = D_AES_IV_MAKE_SET_1;
        L_Toolbox_MemCpy((uint8_t *)p_param->p_ctr_j0, (const uint8_t *)p_param->p_gcm_iv, (int16_t)D_AES_SET_12BYTE);
        L_Toolbox_MemCpy((uint8_t *)(p_param->p_ctr_j0 + ADDR_3RD), (const uint8_t *)&ctr_j_lsb, (int16_t)D_AES_SET_4BYTE);
        if (D_AES_BLOCK_INFO_LAST == (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->iv_block_info))
        {
            /* Disable EI Level Maskable Interrupt */
            DISABLE_INTERRUPT_WITH_CHECK(saved_psw);
            /* Set ICUM_AESCTRL register IDATDEST = 10: Initialization vector/counter register */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) | PRM_IDATDEST_IV_CTR;
            /* Write to ICUM_AESDIDi register */
            ICUM_AESDID0 = *(p_param->p_ctr_j0 + ADDR_0TH);
            ICUM_AESDID1 = *(p_param->p_ctr_j0 + ADDR_1ST);
            ICUM_AESDID2 = *(p_param->p_ctr_j0 + ADDR_2ND);
            ICUM_AESDID3 = D_AES_IV_MAKE_SET_2;
            /* Enable EI Level Maskable Interrupt */
            ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
        } /* end of if (D_AES_BLOCK_INFO_LAST == (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->iv_block_info)) */
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);
        err = d_AES_GcmIvStream(p_param);
        /* ER_OK && Call functions by a last block flag */
        if ((ER_OK == err)
        && (D_AES_BLOCK_INFO_LAST ==
            (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->iv_block_info)))
        {
            err = d_AES_GcmIvLastStream(p_param);
            if (ER_OK == err)
            {
                /* Disable EI Level Maskable Interrupt */
                DISABLE_INTERRUPT_WITH_CHECK(saved_psw);
                /* Set ICUM_AESCTRL register IDATDEST = 10: Initialization vector/counter register */
                ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) | PRM_IDATDEST_IV_CTR;
                /* Write to ICUM_AESDIDi register */
                ICUM_AESDID0 = *(p_param->p_ctr_j0 + ADDR_0TH);
                ICUM_AESDID1 = *(p_param->p_ctr_j0 + ADDR_1ST);
                ICUM_AESDID2 = *(p_param->p_ctr_j0 + ADDR_2ND);
                ctr_j_lsb = BSW(*(p_param->p_ctr_j0 + ADDR_3RD)) + D_AES_SET1_U32;
                ICUM_AESDID3 = BSW(ctr_j_lsb);
                /* Enable EI Level Maskable Interrupt */
                ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
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
    } /* end of else */

    /* Clear control status */
    g_aes_drv_ctrl.status = D_AES_STS_NONE;

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0049]:[ICUMFW_UD_S4_D_AES_0049]
* Function name: d_AES_GcmIvStream
* Description  : Process to generate a counter block in the GCM mode. (Not last stream)
* Arguments    : iv_number     : Number of blocks to be encrypted or decrypted
*                                Range: 0x2 - 0xFFFFFFFF
*                iv_block_info : block information
*                                Range: D_AES_BLOCK_INFO_LAST
*                p_gcm_iv      : address of IV value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_GcmIvStream(aes_gcm_iv_t* p_param)
{
    uint32_t err;
    uint32_t calc_cnt;

    err = ER_OK;
    calc_cnt = D_AES_ZERO_U32;

    /* Calculate the number of repetitions of processing. */
    if (D_AES_BLOCK_INFO_LAST != (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->iv_block_info))
    {
        calc_cnt = p_param->iv_number;
    }
    /* Calculate the more than 1 block.(include the last block) */
    else if ((uint32_t)NUM_LAST_BLOCK < (uint32_t)p_param->iv_number)
    {
        calc_cnt = p_param->iv_number - NUM_LAST_BLOCK;
    }
    /* Calculate only the last block. */
    else
    {
        calc_cnt = END_CALC;
    }
    ASM_MACRO(".align 4");
    for ( ; END_CALC != calc_cnt; calc_cnt--)
    {
        /* Execute AES cipher */
        err = d_AES_GcmIv1Block(p_param);
        if (ER_OK == err)
        {
            p_param->p_gcm_iv += AES_BLOCK_SIZE;
        }
        else
        {
            break;
        }
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0050]:[ICUMFW_UD_S4_D_AES_0050]
* Function name: d_AES_GcmIv1Block
* Description  : Process to generate a counter block in the GCM mode. (Not last stream)
* Arguments    : p_gcm_iv      : address of IV value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static inline uint32_t d_AES_GcmIv1Block(aes_gcm_iv_t* p_param)
{
    uint32_t err;
    uint32_t saved_psw;

    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* New operation, and first of data process */
    if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status))
    {
        /* Clear first or suspend status */
        g_aes_drv_ctrl.status &= ~(D_AES_STS_FIRST | D_AES_STS_SUSPEND);
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GHASH
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_USE);
    }
    /* The resumption operation of a suspended operation */
    else if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
    {
        /* Clear suspend status */
        g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GHASH
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_RESUME
                        | PRM_NEW_KEY_USE);
    }
    /* The continuous operation of data process */
    else
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GHASH
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_NO_USE);
    }

    /* Execute AES cipher */
    err = d_AES_CipherDummyRead(p_param->p_gcm_iv);

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0051]:[ICUMFW_UD_S4_D_AES_0051]
* Function name: d_AES_GcmIvLastStream
* Description  : Process to generate a counter block in the GCM mode. (Last stream)
* Arguments    : iv_number     : Number of blocks to be encrypted or decrypted
*                                Range: 0x1 - 0xFFFFFFFF
*                iv_block_info : block information
*                                Range: D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_gcm_iv      : address of IV value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_GcmIvLastStream(aes_gcm_iv_t* p_param)
{
    uint32_t err;
    uint32_t input[AES_BLOCK_SIZE] = {D_AES_ZERO_U32};
    uint32_t parameter_iv[AES_BLOCK_SIZE] = {D_AES_ZERO_U32};
    uint16_t last_block_size;
    uint32_t saved_psw;

    err = ER_OK;
    last_block_size = D_AES_ZERO_U16;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* New operation, and first of data process */
    if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status))
    {
        /* Clear status first block or suspend */
        g_aes_drv_ctrl.status &= ~(D_AES_STS_FIRST | D_AES_STS_SUSPEND);
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GHASH
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_USE);
    }
    /* The resumption operation of a suspended operation */
    else if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
    {
        /* Clear suspend status */
        g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GHASH
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_RESUME
                        | PRM_NEW_KEY_USE);
    }
    /* The continuous operation of data process */
    else
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GHASH
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_NO_USE);
    }
    /* Is the length of the last block less than 128 ? */
    if (D_AES_BLOCK_INFO_SIZE_MAX > (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->iv_block_info))
    {
        /* calculate last block size */
        last_block_size = (uint16_t)(D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->iv_block_info);
        (void)d_AES_LastBlock(input, p_param->p_gcm_iv, last_block_size);
        /* Execute AES cipher */
        err = d_AES_CipherDummyRead(input);
    }
    else
    {
        /* Execute AES cipher */
        err = d_AES_CipherDummyRead(p_param->p_gcm_iv);
    }
    if (ER_OK == err)
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GHASH
                        | PRM_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_NO_USE);
        parameter_iv[ADDR_0TH] = ALL_ZERO;
        parameter_iv[ADDR_1ST] = ALL_ZERO;
        parameter_iv[ADDR_2ND] = ALL_ZERO;
        parameter_iv[ADDR_3RD] = BSW(*p_param->p_iv_size_in_bits);
        /* Execute AES cipher */
        err = d_AES_Cipher(parameter_iv, p_param->p_ctr_j0);
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0041]:[ICUMFW_UD_S4_D_AES_0041]
* Function name: d_AES_ProcessNBlocks_AAD
* Description  : Process to generate a Additional Authenticated Data in the GCM mode.
* Arguments    : aad_number     : Number of blocks to be encrypted or decrypted
*                                 Range: 0x1 - 0xFFFFFFFF
*                aad_block_info : block information
*                                 Range: D_AES_BLOCK_INFO_LAST
*                                        D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_gcm_aad      : address of AAD value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_ProcessNBlocks_AAD(aes_gcm_aad_t* p_param)
{
    uint32_t err;
    uint32_t calc_cnt;
    uint32_t input[AES_BLOCK_SIZE] = {D_AES_ZERO_U32};
    uint16_t last_block_size;
    uint32_t saved_psw;

    err = ER_OK;
    calc_cnt = D_AES_ZERO_U32;
    last_block_size = D_AES_ZERO_U16;
    saved_psw = D_AES_ZERO_U32;

    /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);
    /* Calculate the number of repetitions of processing. */
    if (D_AES_BLOCK_INFO_LAST != (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->aad_block_info))
    {
        calc_cnt = p_param->aad_number;
    }
    /* Calculate the last block and larger than last blocks. */
    else if ((uint32_t)NUM_LAST_BLOCK < (uint32_t)p_param->aad_number)
    {
        calc_cnt = p_param->aad_number - NUM_LAST_BLOCK;
    }
    else
    {
        /* Calculate only the last block. */
        calc_cnt = END_CALC;
    }

    ASM_MACRO(".align 4");
    for ( ; END_CALC != calc_cnt; calc_cnt--)
    {
        /* Execute AES cipher */
        err = d_AES_GcmAAD1Block(p_param);
        if (ER_OK == err)
        {
            p_param->p_gcm_aad += AES_BLOCK_SIZE;
        }
        else
        {
            break;
        }
    }

    /* ER_OK && Call functions by a last block flag. */
    if ((ER_OK == err)
     && (D_AES_BLOCK_INFO_LAST == (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->aad_block_info)))
    {
        /* Disable EI Level Maskable Interrupt */
        DISABLE_INTERRUPT_WITH_CHECK(saved_psw);
        /* New operation, and first of data process */
        if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status))
        {
            /* Clear first or suspend status */
            g_aes_drv_ctrl.status &= ~(D_AES_STS_FIRST | D_AES_STS_SUSPEND);
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                            (PRM_ATTR_GCM_GHASH
                            | PRM_NO_LAST_BLOCK
                            | PRM_REM_128BIT
                            | PRM_IVSEL_USE
                            | PRM_CONT_NEW_CONTINUE
                            | PRM_NEW_KEY_USE);
        }
        /* The resumption operation of a suspended operation */
        else if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
        {
            /* Clear suspend status */
            g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                            (PRM_ATTR_GCM_GHASH
                            | PRM_NO_LAST_BLOCK
                            | PRM_REM_128BIT
                            | PRM_IVSEL_NO_USE
                            | PRM_CONT_RESUME
                            | PRM_NEW_KEY_USE);
        }
        /* The continuous operation of data process */
        else
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                            (PRM_ATTR_GCM_GHASH
                            | PRM_NO_LAST_BLOCK
                            | PRM_REM_128BIT
                            | PRM_IVSEL_NO_USE
                            | PRM_CONT_NEW_CONTINUE
                            | PRM_NEW_KEY_NO_USE);
        }

        /* Is the length of the last block less than 128 ? */
        if (D_AES_BLOCK_INFO_SIZE_MAX >
            (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->aad_block_info))
        {
            /* Execute AES cipher */
            last_block_size = (uint16_t)(D_AES_BLOCK_INFO_SIZE &  (uint32_t)p_param->aad_block_info);
            (void)d_AES_LastBlock(input, p_param->p_gcm_aad, last_block_size);
            /* Execute AES cipher */
            err = d_AES_CipherDummyRead(input);
        }
        else
        {
            /* Execute AES cipher */
            err = d_AES_CipherDummyRead(p_param->p_gcm_aad);
        }
        /* Enable EI Level Maskable Interrupt */
        ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
    } /* end of if ((ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    /* Clear control status */
    g_aes_drv_ctrl.status = D_AES_STS_NONE;

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0042]:[ICUMFW_UD_S4_D_AES_0042]
* Function name: d_AES_GcmAAD1Block
* Description  : Process to generate a Additional Authenticated Data in the GCM mode.(Not last stream)
* Arguments    : p_gcm_aad      : address of AAD value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static inline uint32_t d_AES_GcmAAD1Block(aes_gcm_aad_t* p_param)
{
    uint32_t err;
    uint32_t saved_psw;

    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* New operation, and first of data process */
    if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status))
    {
        /* Clear first or suspend status */
        g_aes_drv_ctrl.status &= ~(D_AES_STS_FIRST | D_AES_STS_SUSPEND);
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GHASH
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_USE);
    }
    /* The resumption of a suspended operation, and first of data process */
    else if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
    {
        /* Clear suspend status */
        g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GHASH
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_RESUME
                        | PRM_NEW_KEY_USE);
    }
    /* The continuous operation of data process */
    else
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GHASH
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_NO_USE);
    }
    /* Execute AES cipher */
    err = d_AES_CipherDummyRead(p_param->p_gcm_aad);

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0043]:[ICUMFW_UD_S4_D_AES_0043]
* Function name: d_AES_ProcessNBlocks_GCM
* Description  : Perform cipher processing in the GCM mode.
* Arguments    : gcm_number     : Number of blocks to be encrypted or decrypted
*                                 Range: 0x1 - 0xFFFFFFFF
*                gcm_block_info : block information
*                                 Range: D_AES_BLOCK_INFO_FIRST
*                                        D_AES_BLOCK_INFO_LAST
*                                        D_AES_BLOCK_INFO_GCM_ADD0
*                                        D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_ctr_j        : address of counter block Jt value
*                p_gcm_src      : address of input value
*                p_gcm_dest     : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_ProcessNBlocks_GCM(aes_gcm_t* p_param)
{
    uint32_t err;
    uint32_t saved_psw;

    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);
    /* Set ICUM_AESCTRL register IDATDEST = 10: Initialization vector/counter register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) | PRM_IDATDEST_IV_CTR;
    /* Write to ICUM_AESDIDi register */
    ICUM_AESDID0 = *(p_param->p_ctr_j + ADDR_0TH);
    ICUM_AESDID1 = *(p_param->p_ctr_j + ADDR_1ST);
    ICUM_AESDID2 = *(p_param->p_ctr_j + ADDR_2ND);
    ICUM_AESDID3 = *(p_param->p_ctr_j + ADDR_3RD);
    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
    /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);
    err = d_AES_GcmStream(p_param);
    /* ER_OK && Call functions by a last block flag. */
    if ((ER_OK == err)
    && (D_AES_BLOCK_INFO_LAST ==
        (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->gcm_block_info)))
    {
        err = d_AES_GcmLastStream(p_param);
        if (ER_OK == err)
        {
            /* aes control status is no cleared for authentication TAG generation */
        }
        else
        {
            /* Clear aes control status */
            g_aes_drv_ctrl.status = D_AES_STS_NONE;
        }
    }
    else
    {
        /* Clear aes control status */
        g_aes_drv_ctrl.status = D_AES_STS_NONE;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0044]:[ICUMFW_UD_S4_D_AES_0044]
* Function name: d_AES_GcmStream
* Description  : Perform encryption processing in the GCM mode. (Not Last stream)
* Arguments    : gcm_number     : Number of blocks to be encrypted or decrypted
*                                 Range: 0x1 - 0xFFFFFFFF
*                gcm_block_info : block information
*                                 Range: D_AES_BLOCK_INFO_LAST
*                p_gcm_src      : address of input value
*                p_gcm_dest     : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_GcmStream(aes_gcm_t* p_param)
{
    uint32_t err;
    uint32_t calc_cnt;

    err = ER_OK;
    calc_cnt = D_AES_ZERO_U32;

    /* Calculate the gcm_number of repetitions of processing. */
    if (D_AES_BLOCK_INFO_LAST != (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->gcm_block_info))
    {
        calc_cnt = p_param->gcm_number;
    }
    /* Calculate the last block and larger than last blocks. */
    else if ((uint32_t)NUM_LAST_BLOCK < (uint32_t)p_param->gcm_number)
    {
        calc_cnt = p_param->gcm_number - NUM_LAST_BLOCK;
    }
    else
    {
        /* Calculate only the last block. */
        calc_cnt = END_CALC;
    }
    ASM_MACRO(".align 4");
    for ( ; END_CALC != calc_cnt; calc_cnt--)
    {
        /* Execute AES cipher */
        err = d_AES_Gcm1Block(p_param);
        if (ER_OK == err)
        {
            p_param->p_gcm_src += AES_BLOCK_SIZE;
            p_param->p_gcm_dest += AES_BLOCK_SIZE;
        }
        else
        {
            break;
        }
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0045]:[ICUMFW_UD_S4_D_AES_0045]
* Function name: d_AES_Gcm1Block
* Description  : Perform encryption processing in the GCM mode. (Not last stream)
* Arguments    : gcm_block_info : block information
*                                 Range: D_AES_BLOCK_INFO_GCM_ADD0
*                p_gcm_src      : address of input value
*                p_gcm_dest     : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static inline uint32_t d_AES_Gcm1Block(aes_gcm_t* p_param)
{
    uint32_t err;
    uint32_t saved_psw;

    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* New operation, and first of data process */
    if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status))
    {
        /* Clear first or suspend status */
        g_aes_drv_ctrl.status &= ~(D_AES_STS_FIRST | D_AES_STS_SUSPEND);
        if (D_AES_BLOCK_INFO_GCM_ADD0 ==
            (D_AES_BLOCK_INFO_GCM_ADD0 & (uint32_t)p_param->gcm_block_info))
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                             (PRM_ATTR_GCM_GCTR
                            | PRM_NO_LAST_BLOCK
                            | PRM_REM_128BIT
                            | PRM_IVSEL_USE
                            | PRM_CONT_NEW_CONTINUE
                            | PRM_NEW_KEY_USE);
        }
        else
        {
            err = ER_PARAM;
        }
    } /* end of if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status)) */
    /* The resumption operation of a suspended operation */
    else if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
    {
        /* Clear suspend status */
        g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GCTR
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_RESUME
                        | PRM_NEW_KEY_USE);
    }
    /* The continuous operation of data process */
    else
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GCTR
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_NO_USE);
    }
    if (ER_OK == err)
    {
        /* Execute AES cipher */
        err = d_AES_Cipher(p_param->p_gcm_src, p_param->p_gcm_dest);
    }
    else
    {
        /* Do nothing */
    }

    /* Is the AES execution mode encrypted? */
    if ((ER_OK == err) && (PRM_ENCMODE0_ENC == (ENCMODE0 & ICUM_AESCCTL0)))
    {
        /* Wait ICUM_AES GHSTS is in idle state. */
        err = d_AES_WaitIdleState(GHSTS, PRM_GHSTS_VALID);
    }
    else
    {
        /* Do nothing */
    }

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0046]:[ICUMFW_UD_S4_D_AES_0046]
* Function name: d_AES_GcmLastStream
* Description  : Perform encryption processing in the GCM mode. (Last stream)
* Arguments    : gcm_number     : Number of blocks to be encrypted or decrypted
*                                 Range: 0x1 - 0xFFFFFFFF
*                gcm_block_info : block information
*                                 Range: D_AES_BLOCK_INFO_LAST
*                                        D_AES_BLOCK_INFO_GCM_ADD0
*                                        D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_gcm_src      : address of input value
*                p_gcm_dest     : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_GcmLastStream(aes_gcm_t* p_param)
{
    uint32_t err;
    uint32_t input[AES_BLOCK_SIZE] = {D_AES_ZERO_U32};
    uint16_t last_block_size;
    uint32_t saved_psw;
    uint32_t prm_rem;

    err = ER_OK;
    last_block_size = D_AES_ZERO_U16;
    saved_psw = D_AES_ZERO_U32;
    prm_rem = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    if (D_AES_BLOCK_INFO_SIZE_MAX == (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->gcm_block_info))
    {
        prm_rem = PRM_REM_128BIT;
    }
    else
    {
        prm_rem = REM & ((uint32_t)p_param->gcm_block_info << SHIFT_REM);
    }

    /* New operation, and first of data process */
    if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status))
    {
        /* Clear first or suspend status */
        g_aes_drv_ctrl.status &= ~(D_AES_STS_FIRST | D_AES_STS_SUSPEND);
        if (D_AES_BLOCK_INFO_GCM_ADD0 ==
            (D_AES_BLOCK_INFO_GCM_ADD0 & (uint32_t)p_param->gcm_block_info))
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                             (PRM_ATTR_GCM_GCTR
                            | PRM_LAST_BLOCK
                            | prm_rem
                            | PRM_IVSEL_USE
                            | PRM_CONT_NEW_CONTINUE
                            | PRM_NEW_KEY_USE);
        }
        else
        {
            err = ER_PARAM;
        }
    } /* end of if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status)) */
    else if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
    {
        /* Clear suspend status */
        g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GCTR
                        | PRM_LAST_BLOCK
                        | prm_rem
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_RESUME
                        | PRM_NEW_KEY_USE);
    }
    else
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_GCTR
                        | PRM_LAST_BLOCK
                        | prm_rem
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_NO_USE);
    }

    if (ER_OK == err)
    {
        if (D_AES_BLOCK_INFO_SIZE_MAX ==
            (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->gcm_block_info))
        {
            /* Execute AES cipher */
            err = d_AES_Cipher(p_param->p_gcm_src, p_param->p_gcm_dest);
        }
        else
        {
            /* calculate last block size */
            last_block_size = (uint16_t)(D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->gcm_block_info);
            (void)d_AES_LastBlock(input, p_param->p_gcm_src, last_block_size);
            /* Execute AES cipher */
            err = d_AES_Cipher(input, p_param->p_gcm_dest);
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0047]:[ICUMFW_UD_S4_D_AES_0047]
* Function name: d_AES_GetAuthTag
* Description  : Get Authentication Tag.
* Arguments    : p_ctr_j0             : address of counter block J0 value
*                p_total_size_in_bits : total length of AAD and cipher
*                p_auth_tag           : address of authentication tag value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_GetAuthTag(aes_gcm_tag_t* p_param)
{
    uint32_t err;
    uint32_t saved_psw;

    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Set ICUM_AESCTRL register IDATDEST = 10: Initialization vector/counter register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) | PRM_IDATDEST_IV_CTR;
    /* Write to ICUM_AESDIDi register */
    ICUM_AESDID0 = *(p_param->p_ctr_j0 + ADDR_0TH);
    ICUM_AESDID1 = *(p_param->p_ctr_j0 + ADDR_1ST);
    ICUM_AESDID2 = *(p_param->p_ctr_j0 + ADDR_2ND);
    ICUM_AESDID3 = *(p_param->p_ctr_j0 + ADDR_3RD);
    /* New operation, and first of data process */
    if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status))
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_TAG
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_USE);
    }
    else if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_TAG
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_RESUME
                        | PRM_NEW_KEY_USE);
    }
    else
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_GCM_TAG
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_NO_USE);
    }
    /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);
    /* Execute AES cipher */
    err = d_AES_Cipher(p_param->p_total_size_in_bits, p_param->p_auth_tag);

    /* Clear aes control status */
    g_aes_drv_ctrl.status = D_AES_STS_NONE;

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0064]:[ICUMFW_UD_S4_D_AES_0064]
* Function name: d_AES_SetParam_CCM
* Description  : Set parameters a specified context for CCM mode.
* Arguments    : direction    : cipher direction
*                               Range: D_AES_DIR_ENCRYPTION
*                                      D_AES_DIR_DECRYPTION
*                p_ccm_key    : address of key value
*                p_ccm_b0     : address of B0 data
*                p_ccm_nonce  : address of nonce value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_TIMEOUT_ERROR
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t d_AES_SetParam_CCM(const aes_ccm_setparam_t* p_param)
{
    /* Context #2 only */
    uint32_t err;
    uint32_t saved_psw;
    uint32_t keysel;
    uint32_t i;
    uint32_t index;
    volatile uint32_t *p_key_reg[D_AES_KEY_NUM];

    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;
    keysel = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Set ICUM_AESKEY register */
    if (D_AES_KEY_SIZE_IN_BYTES_128BIT == (uint32_t)p_param->key_length)
    {   /* 128bit */
        keysel = D_AES_SET_KEYSEL_0;
    }
    else if (D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)p_param->key_length)
    {   /* 192bit */
        keysel = D_AES_SET_KEYSEL_1;
    }
    else
    {   /* 256bit */
        keysel = D_AES_SET_KEYSEL_2;
    }
    ICUM_AESKEY = (ICUM_AESKEY & ~(KEYSEL)) | keysel;

    if (NO_USE_CTX2 == g_ctx2_state)
    {
        /* Check reserved bit range of B0. */
        if ((B0_RESERVED != (*(p_param->p_ccm_b0 + ADDR_0TH) & ~B0_ALEN))
         || (B0_RESERVED != (*(p_param->p_ccm_b0 + ADDR_1ST) & ~(B0_NLEN | B0_TLEN | B0_PLEN0)))
         || (B0_RESERVED != (*(p_param->p_ccm_b0 + ADDR_3RD) & ~B0_PLEN2)))
        {
            err = ER_PARAM;
        }
        /* Check prohibited settings of Nlen and Tlen. */
        else if ((B0_NLEN == (B0_NLEN & *(p_param->p_ccm_b0 + ADDR_1ST)))
              || (B0_TLEN == (B0_TLEN & *(p_param->p_ccm_b0 + ADDR_1ST))))
        {
            err = ER_PARAM;
        }
        else
        {
            g_ctx2_state = USE_CTX2;
            /* Set ICUM_AESCTRL register */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
            UNLOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
#endif
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(RST2)) | PRM_RST2_INIT;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
            D_AES_WAIT_REG_VALUE(PRM_RST2_INIT, ICUM_AESCTRL, RST2);
            LOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
#endif
            /* Set ICUM_AESCCTL1 register */
            ICUM_AESCCTL1 = (ICUM_AESCCTL1 & ~(MOD2 | ENCMODE2 | K2SEL)) |
                              (D_AES_CIPHER_MODE_CCM
                            | (ENCMODE2 & ((uint32_t)p_param->direction << SHIFT_ENCMODE2))
                            | PRM_K2SEL_PLAIN);
            /* Set ICUM_AESCTRL register */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST | CTX | BYTE_ORDER | DATREGSW)) |
                             (PRM_IDATDEST_KEY
                            | PRM_CTX_2
                            | PRM_BYTE_ORDER_APPLY
                            | PRM_DATREGSW_DIDI_DODI);
            /* Set ICUM_AESKEY register */
            ICUM_AESKEY = (ICUM_AESKEY & ~(EX_KEY)) | PRM_EX_KEY_SET_127_0;
           /* Start at a random index */
            D_OST_GetHardCounter(&index);
            index %= D_AES_KEY_NUM;
            /* Set registers for key input */
            *(p_key_reg + ADDR_0TH) = (volatile uint32_t *)&ICUM_AESDID0;
            *(p_key_reg + ADDR_1ST) = (volatile uint32_t *)&ICUM_AESDID1;
            *(p_key_reg + ADDR_2ND) = (volatile uint32_t *)&ICUM_AESDID2;
            *(p_key_reg + ADDR_3RD) = (volatile uint32_t *)&ICUM_AESDID3;
            /* Write to ICUM_AESDIDi register *//* XTS does not support 192bit */
            /* key length 128 or 192 or 256[bit] */
            for (i = D_AES_ZERO_U32; i < D_AES_KEY_NUM; i++)
            {
                *(p_key_reg[index]) = p_param->p_ccm_key[index];
                index = (index + 1U) % D_AES_KEY_NUM;
            }
            /* key length 192 or 256[bit] */
            if ((D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)p_param->key_length)
             || (D_AES_KEY_SIZE_IN_BYTES_256BIT == (uint32_t)p_param->key_length))
            {

                /* Set ICUM_AESKEY register */
                ICUM_AESKEY = (ICUM_AESKEY & ~(EX_KEY)) | PRM_EX_KEY_SET_256_128;

                /* Write to ICUM_AESDIDi register for key length = 192bit */
                ICUM_AESDID0 = *(p_param->p_ccm_key + ADDR_4TH);
                ICUM_AESDID1 = *(p_param->p_ccm_key + ADDR_5TH);

                /* key length 256[bit] */
                if (D_AES_KEY_SIZE_IN_BYTES_256BIT == (uint32_t)p_param->key_length)
                {
                    /* Write to ICUM_AESDIDi register for key length = 256bit */
                    ICUM_AESDID2 = *(p_param->p_ccm_key + ADDR_6TH);
                    ICUM_AESDID3 = *(p_param->p_ccm_key + ADDR_7TH);
                }
                else
                {
                    /* Do nothing */
                }
            } /* end of if ((D_AES_KEY_SIZE_IN_BYTES_192BIT == p_param->key_length) */
            else
            {
                /* Do nothing */
            }

            /* Set ICUM_AESCTRL register IDATDEST = 10: Initialization vector/counter register */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) | PRM_IDATDEST_IV_CTR;
            /* Write to ICUM_AESDIDi register */
            ICUM_AESDID0 = *(p_param->p_ccm_nonce + ADDR_0TH);
            ICUM_AESDID1 = *(p_param->p_ccm_nonce + ADDR_1ST);
            ICUM_AESDID2 = *(p_param->p_ccm_nonce + ADDR_2ND);
            ICUM_AESDID3 = *(p_param->p_ccm_nonce + ADDR_3RD);
            /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                             (PRM_ATTR_CCM_B0
                            | PRM_IVSEL_USE
                            | PRM_NEW_KEY_USE);
            if (D_AES_DIR_ENCRYPTION == (uint32_t)p_param->direction)
            {
                err = d_AES_CipherDummyRead(p_param->p_ccm_b0);
            }
            else
            {
                /* Check Alen and Plen of B0. */
                if ((ALL_ZERO != (*(p_param->p_ccm_b0 + ADDR_0TH) & B0_ALEN))
                 || (ALL_ZERO != (*(p_param->p_ccm_b0 + ADDR_1ST) & B0_PLEN0))
                 || (ALL_ZERO != (*(p_param->p_ccm_b0 + ADDR_2ND) & B0_PLEN1))
                 || (ALL_ZERO != (*(p_param->p_ccm_b0 + ADDR_3RD) & B0_PLEN2)))
                {
                    err = d_AES_CipherDummyRead(p_param->p_ccm_b0);
                }
                else
                {
                    err = d_AES_CipherNoRead(p_param->p_ccm_b0);
                }
            } /* end of else */
            if (ER_OK == err)
            {
                /* Do nothing */
            }
            else
            {
                g_ctx2_state = NO_USE_CTX2;
            }
        } /* end of else */
    } /* end of if (NO_USE_CTX2 == g_ctx2_state) */
    else
    {
        err = ER_D_AES_SEQUENCE_ERROR;
    }

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0060]:[ICUMFW_UD_S4_D_AES_0060]
* Function name: d_AES_SetAssociatedData
* Description  : Process to generate a Associated Data in the CCM mode.
* Arguments    : direction    : cipher direction
*                               Range: D_AES_DIR_ENCRYPTION
*                                      D_AES_DIR_DECRYPTION
*                a_block_info : block information
*                               Range: D_AES_BLOCK_INFO_LAST
*                                      D_AES_BLOCK_INFO_CCM_PLEN0
*                a_number     : Number of blocks to be encrypted or decrypted
*                               Range: 0x1 - 0xFFFFFFFF
*                p_associated : address of associated data
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t d_AES_SetAssociatedData(aes_ccm_a_t* p_param)
{
    uint32_t err;
    uint32_t calc_cnt;
    uint32_t input[AES_BLOCK_SIZE] = {D_AES_ZERO_U32};
    uint16_t last_block_size;

    err = ER_OK;
    calc_cnt = D_AES_ZERO_U32;
    last_block_size = D_AES_ZERO_U16;

    if (USE_CTX2 == g_ctx2_state)
    {
        /* Set ICUM_AESCTRL register */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST | CTX | BYTE_ORDER | DATREGSW)) |
                         (PRM_ODATSRC_BCO
                        | PRM_IDATDEST_BCI
                        | PRM_CTX_2
                        | PRM_BYTE_ORDER_APPLY
                        | PRM_DATREGSW_DIDI_DODI);
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                         (PRM_ATTR_CCM_A
                        | PRM_IVSEL_NO_USE
                        | PRM_NEW_KEY_NO_USE);
        ASM_MACRO(".align 4");
        for (calc_cnt = p_param->a_number - NUM_LAST_BLOCK; END_CALC != calc_cnt; calc_cnt--)
        {
            /* Execute AES cipher */
            err = d_AES_CipherDummyRead(p_param->p_associated);
            if (ER_OK == err)
            {
                p_param->p_associated += AES_BLOCK_SIZE;
            }
            else
            {
                break;
            }
        }
        if (ER_OK == err)
        {
            if (D_AES_BLOCK_INFO_LAST != (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->a_block_info))
            {
                /* Execute AES cipher */
                err = d_AES_CipherDummyRead(p_param->p_associated);
            }
            else
            {
                /* calculate last block size */
                last_block_size = (uint16_t)(D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->a_block_info);
                /* Is the length of the last block less than 128 ? */
                if (D_AES_BLOCK_INFO_SIZE_MAX ==
                (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->a_block_info))
                {
                    if ((D_AES_DIR_DECRYPTION == (uint32_t)p_param->direction)
                     && (D_AES_BLOCK_INFO_CCM_PLEN0 ==
                     (D_AES_BLOCK_INFO_CCM_PLEN0 & (uint32_t)p_param->a_block_info)))
                    {
                        /* Execute AES cipher */
                        err = d_AES_CipherNoRead(p_param->p_associated);
                    }
                    else
                    {
                        /* Execute AES cipher */
                        err = d_AES_CipherDummyRead(p_param->p_associated);
                    }
                } /* end of if (D_AES_BLOCK_INFO_SIZE_MAX == */
                else
                {
                    /* Shape the last block */
                    (void)d_AES_LastBlock(input, p_param->p_associated, last_block_size);
                    if ((D_AES_DIR_DECRYPTION == (uint32_t)p_param->direction)
                     && (D_AES_BLOCK_INFO_CCM_PLEN0 ==
                     (D_AES_BLOCK_INFO_CCM_PLEN0 & (uint32_t)p_param->a_block_info)))
                    {
                        /* Execute AES cipher */
                        err = d_AES_CipherNoRead(input);
                    }
                    else
                    {
                        /* Execute AES cipher */
                        err = d_AES_CipherDummyRead(input);
                    }
                } /* end of else */
            } /* end of else */
        } /* end of if (ER_OK == err) */
        else
        {
            /* Do nothing */
        }

        if (ER_OK == err)
        {
            /* Do nothing */
        }
        else
        {
            g_ctx2_state = NO_USE_CTX2;
        }
    } /* end of if (USE_CTX2 == g_ctx2_state) */
    else
    {
        err = ER_D_AES_SEQUENCE_ERROR;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0061]:[ICUMFW_UD_S4_D_AES_0061]
* Function name: d_AES_ProcessNBlocks_CCM
* Description  : Perform cipher processing in the CCM mode.
* Arguments    : direction      : cipher direction
*                                 Range: D_AES_DIR_ENCRYPTION
*                                        D_AES_DIR_DECRYPTION
*                ccm_block_info : block information
*                                 Range: D_AES_BLOCK_INFO_LAST
*                                        D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                ccm_number     : Number of blocks to be encrypted
*                                 Range: 0x1 - 0xFFFFFFFF
*                p_ccm_src      : address of input value
*                p_ccm_dest     : address of output value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t d_AES_ProcessNBlocks_CCM(aes_ccm_t* p_param)
{
    uint32_t err;
    uint32_t calc_cnt;
    uint32_t input[AES_BLOCK_SIZE];
    uint32_t output[AES_BLOCK_SIZE];
    uint16_t last_block_size;

    err = ER_OK;
    calc_cnt = D_AES_ZERO_U32;
    last_block_size = D_AES_ZERO_U16;

    if (USE_CTX2 == g_ctx2_state)
    {
        /* Set ICUM_AESCTRL register */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST | CTX | BYTE_ORDER | DATREGSW)) |
                         (PRM_ODATSRC_BCO
                        | PRM_IDATDEST_BCI
                        | PRM_CTX_2
                        | PRM_BYTE_ORDER_APPLY
                        | PRM_DATREGSW_DIDI_DODI);
        if (D_AES_DIR_ENCRYPTION == (uint32_t)p_param->direction)
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                             (PRM_ATTR_CCM_PLAIN
                            | PRM_IVSEL_NO_USE
                            | PRM_NEW_KEY_NO_USE);
        }
        else
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                             (PRM_ATTR_CCM_DEC_CIPHER
                            | PRM_IVSEL_NO_USE
                            | PRM_NEW_KEY_NO_USE);
        }
        ASM_MACRO(".align 4");
        for (calc_cnt = p_param->ccm_number - NUM_LAST_BLOCK; END_CALC != calc_cnt; calc_cnt--)
        {
            /* Execute AES cipher */
            err = d_AES_Cipher(p_param->p_ccm_src, p_param->p_ccm_dest);
            if (ER_OK == err)
            {
                p_param->p_ccm_src += AES_BLOCK_SIZE;
                p_param->p_ccm_dest += AES_BLOCK_SIZE;
            }
            else
            {
                break;
            }
        }  /* end of for (calc_cnt = p_param->ccm_number - NUM_LAST_BLOCK; END_CALC != calc_cnt; calc_cnt--) */
        if (ER_OK == err)
        {
            /* Is the length of the last block less than 128 ? */
            if ((D_AES_BLOCK_INFO_LAST ==
                (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->ccm_block_info))
             && (D_AES_BLOCK_INFO_SIZE_MAX >
             (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->ccm_block_info)))
            {
                /* calculate last block size */
                last_block_size = (uint16_t)(D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->ccm_block_info);
                /* Shape the last block */
                (void)d_AES_LastBlock(input, p_param->p_ccm_src, last_block_size);
                /* Execute AES cipher */
                err = d_AES_Cipher(input, output);
                /* Shape the last block */
                (void)d_AES_LastBlock(p_param->p_ccm_dest, output, last_block_size);
            }
            else
            {
                /* Execute AES cipher */
                err = d_AES_Cipher(p_param->p_ccm_src, p_param->p_ccm_dest);
            }
        } /* end of if (ER_OK == err) */
        else
        {
            /* Do nothing */
        }

        if (ER_OK == err)
        {
            /* Do nothing */
        }
        else
        {
            g_ctx2_state = NO_USE_CTX2;
        }
    } /* end of if (USE_CTX2 == g_ctx2_state) */
    else
    {
        err = ER_D_AES_SEQUENCE_ERROR;
    }
    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0065]:[ICUMFW_UD_S4_D_AES_0065]
* Function name: d_AES_SetPayloadData
* Description  : Set payload data and generate authentication TAG.
* Arguments    : payload_block_info : block information
*                                     Range: D_AES_BLOCK_INFO_LAST
*                                            D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                payload_number     : Number of blocks to be encrypted
*                                     Range: 0x1 - 0xFFFFFFFF
*                p_payload          : address of payload data
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t d_AES_SetPayloadData(aes_ccm_p_t* p_param)
{
    uint32_t err;
    uint32_t calc_cnt;
    uint32_t input[AES_BLOCK_SIZE] = {D_AES_ZERO_U32};
    uint16_t last_block_size;

    err = ER_OK;
    calc_cnt = D_AES_ZERO_U32;
    last_block_size = D_AES_ZERO_U16;

    if (USE_CTX2 == g_ctx2_state)
    {
        /* Set ICUM_AESCTRL register */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST | CTX | BYTE_ORDER | DATREGSW)) |
                         (PRM_ODATSRC_BCO
                        | PRM_IDATDEST_BCI
                        | PRM_CTX_2
                        | PRM_BYTE_ORDER_APPLY
                        | PRM_DATREGSW_DIDI_DODI);
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                         (PRM_ATTR_CCM_PLAIN
                        | PRM_IVSEL_NO_USE
                        | PRM_NEW_KEY_NO_USE);
        ASM_MACRO(".align 4");
        for (calc_cnt = p_param->payload_number - NUM_LAST_BLOCK; END_CALC != calc_cnt; calc_cnt--)
        {
            /* Execute AES cipher */
            err = d_AES_CipherDummyRead(p_param->p_payload);
            if (ER_OK == err)
            {
                p_param->p_payload += AES_BLOCK_SIZE;
            }
            else
            {
                break;
            }
        }
        if (ER_OK == err)
        {
            /* Is the length of the last block less than 128 ? */
            if ((D_AES_BLOCK_INFO_LAST ==
             (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->payload_block_info))
             && (D_AES_BLOCK_INFO_SIZE_MAX >
             (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->payload_block_info)))
            {
                /* calculate last block size */
                last_block_size = (uint16_t)(D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->payload_block_info);
                /* Shape the last block */
                (void)d_AES_LastBlock(input, p_param->p_payload, last_block_size);
                /* Execute AES cipher */
                err = d_AES_CipherDummyRead(input);
            }
            else
            {
                /* Execute AES cipher */
                err = d_AES_CipherDummyRead(p_param->p_payload);
            }
        } /* end of if (ER_OK == err) */
        else
        {
            /* Do nothing */
        }

        if (ER_OK == err)
        {
            /* Do nothing */
        }
        else
        {
            g_ctx2_state = NO_USE_CTX2;
        }
    } /* end of if (USE_CTX2 == g_ctx2_state) */
    else
    {
        err = ER_D_AES_SEQUENCE_ERROR;
    }
    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0062]:[ICUMFW_UD_S4_D_AES_0062]
* Function name: d_AES_GetAuthTag_CcmEnc
* Description  : Get the authentication tag in the encryption processing in the CCM mode.
* Arguments    : p_auth_tag : address of authentication tag data
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t d_AES_GetAuthTag_CcmEnc(uint32_t* p_auth_tag)
{
    uint32_t err;
    uint32_t dummy_write[AES_BLOCK_SIZE] = {ALL_ZERO, ALL_ZERO, ALL_ZERO, ALL_ZERO};

    err = ER_OK;

    if (USE_CTX2 == g_ctx2_state)
    {
        /* Set ICUM_AESCTRL register */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST | CTX | BYTE_ORDER | DATREGSW)) |
                         (PRM_ODATSRC_BCO
                        | PRM_IDATDEST_BCI
                        | PRM_CTX_2
                        | PRM_BYTE_ORDER_APPLY
                        | PRM_DATREGSW_DIDI_DODI);
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                         (PRM_ATTR_CCM_ENC_DUMMY
                        | PRM_IVSEL_NO_USE
                        | PRM_NEW_KEY_NO_USE);
        /* Execute AES cipher */
        err = d_AES_Cipher(dummy_write, p_auth_tag);
        g_ctx2_state = NO_USE_CTX2;
    } /* end of if (USE_CTX2 == g_ctx2_state) */
    else
    {
        err = ER_D_AES_SEQUENCE_ERROR;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0063]:[ICUMFW_UD_S4_D_AES_0063]
* Function name: d_AES_GetAuthTag_CcmDec
* Description  : Get the authentication tag in the decryption processing in the CCM mode.
* Arguments    : p_tag_part         : tag part data of ciphertext
*                p_auth_yr          : address of authentication tag data(Yr)
*                p_auth_tag         : address of authentication tag data(T)
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t d_AES_GetAuthTag_CcmDec(aes_ccm_tag_dec_t* p_param)
{
    uint32_t err;

    err = ER_OK;

    if (USE_CTX2 == g_ctx2_state)
    {
        /* Set ICUM_AESCTRL register */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST | CTX | BYTE_ORDER | DATREGSW)) |
                         (PRM_ODATSRC_BCO
                        | PRM_IDATDEST_BCI
                        | PRM_CTX_2
                        | PRM_BYTE_ORDER_APPLY
                        | PRM_DATREGSW_DIDI_DODI);
        /* Read from ICUM_AESDODi register */
        *(p_param->p_auth_yr + ADDR_0TH) = ICUM_AESDOD0;
        *(p_param->p_auth_yr + ADDR_1ST) = ICUM_AESDOD1;
        *(p_param->p_auth_yr + ADDR_2ND) = ICUM_AESDOD2;
        *(p_param->p_auth_yr + ADDR_3RD) = ICUM_AESDOD3;
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                         (PRM_ATTR_CCM_DEC_T_X_S0
                        | PRM_IVSEL_NO_USE
                        | PRM_NEW_KEY_NO_USE);
        /* Execute AES cipher */
        err = d_AES_Cipher(p_param->p_tag_part, p_param->p_auth_tag);
        g_ctx2_state = NO_USE_CTX2;
    } /* end of if (USE_CTX2 == g_ctx2_state) */
    else
    {
        err = ER_D_AES_SEQUENCE_ERROR;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0066]:[ICUMFW_UD_S4_D_AES_0066]
* Function name: d_AES_Cancel
* Description  : Cancel cipher processing.
* Arguments    : void
* Return Value : ER_OK
***************************************************************************************/
static uint32_t d_AES_Cancel(void)
{
    /* Set ICUM_AESCTRL register ODATSRC = 00: Block Cipher Output register (BCO) */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC)) | PRM_ODATSRC_BCO;
    /* Dummy read from ICUM_AESDODi register */
    ICUM_AESDOD0;
    ICUM_AESDOD1;
    ICUM_AESDOD2;
    ICUM_AESDOD3;
    /* Dummy read from ICUM_AESDODi register */
    ICUM_AESDOD0;
    ICUM_AESDOD1;
    ICUM_AESDOD2;
    ICUM_AESDOD3;
    g_ctx2_state = NO_USE_CTX2;

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0068]:[ICUMFW_UD_S4_D_AES_0068]
* Function name: d_AES_Cipher
* Description  : Perform cipher operation.
* Arguments    : p_aes_src  : address of input value
*                p_aes_dest : address of output value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
inline uint32_t d_AES_Cipher(const uint32_t* p_aes_src, uint32_t* p_aes_dest)
{
    uint32_t err;

    err = ER_OK;

    /* Write to ICUM_AESDIDi register */
    ICUM_AESDID0 = *(p_aes_src + ADDR_0TH);
    ICUM_AESDID1 = *(p_aes_src + ADDR_1ST);
    ICUM_AESDID2 = *(p_aes_src + ADDR_2ND);
    ICUM_AESDID3 = *(p_aes_src + ADDR_3RD);
    /* Wait ICUM_AES is in idle state */
    err = d_AES_WaitIdleState(STS, PRM_STS_IDLE);
    if (ER_OK == err)
    {
        /* Read from ICUM_AESDODi register */
        *(p_aes_dest + ADDR_0TH) = ICUM_AESDOD0;
        *(p_aes_dest + ADDR_1ST) = ICUM_AESDOD1;
        *(p_aes_dest + ADDR_2ND) = ICUM_AESDOD2;
        *(p_aes_dest + ADDR_3RD) = ICUM_AESDOD3;
    }
    else
    {
        /* Do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0069]:[ICUMFW_UD_S4_D_AES_0069]
* Function name: d_AES_CipherDummyRead
* Description  : Perform cipher operation and dummy read.
* Arguments    : p_aes_src : address of input value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
inline uint32_t d_AES_CipherDummyRead(const uint32_t* p_aes_src)
{
    uint32_t err;

    err = ER_OK;

    /* Write to ICUM_AESDIDi register */
    ICUM_AESDID0 = *(p_aes_src + ADDR_0TH);
    ICUM_AESDID1 = *(p_aes_src + ADDR_1ST);
    ICUM_AESDID2 = *(p_aes_src + ADDR_2ND);
    ICUM_AESDID3 = *(p_aes_src + ADDR_3RD);
    /* Wait ICUM_AES is in idle state */
    err = d_AES_WaitIdleState(STS, PRM_STS_IDLE);
    if (ER_OK == err)
    {
        /* Dummy read from ICUM_AESDODi register */
        ICUM_AESDOD0;
        ICUM_AESDOD1;
        ICUM_AESDOD2;
        ICUM_AESDOD3;
    }
    else
    {
        /* Do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0070]:[ICUMFW_UD_S4_D_AES_0070]
* Function name: d_AES_CipherNoRead
* Description  : Perform cipher operation, do not dummy read.
* Arguments    : p_aes_src : address of input value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
inline uint32_t d_AES_CipherNoRead(const uint32_t* p_aes_src)
{
    uint32_t err;

    err = ER_OK;

    /* Write to ICUM_AESDIDi register */
    ICUM_AESDID0 = *(p_aes_src + ADDR_0TH);
    ICUM_AESDID1 = *(p_aes_src + ADDR_1ST);
    ICUM_AESDID2 = *(p_aes_src + ADDR_2ND);
    ICUM_AESDID3 = *(p_aes_src + ADDR_3RD);
    /* Wait ICUM_AES is in idle state */
    err = d_AES_WaitIdleState(STS, PRM_STS_IDLE);

    return err;
}

PRAGMA(ghs section text = ".icum_driver_ram")
PRAGMA(ghs section rodata = ".icum_driver_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0016]:[ICUMFW_UD_S4_D_AES_0016]
* Function name: d_AES_ProcessNBlocks_CMAC
* Description  : Perform encryption processing.(CMAC)
* Arguments    : number     : Number of blocks to be encrypted
*                             Range: 0x0 - 0xFFFFFFFF
*                block_info : block information
*                             Range: D_AES_BLOCK_INFO_LAST
*                                    D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_aes_src  : address of input value
*                p_aes_dest : address of output value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_ProcessNBlocks_CMAC(aes_nblocks_t* p_param)
{
    uint32_t err;

    err = ER_OK;

    /* Set ICUM_AESCTRL register IDATDEST = 00: Block Cipher Input register (BCI) */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) | PRM_IDATDEST_BCI;
    /* Call functions by a last block flag and a number. */
    if (D_AES_BLOCK_INFO_LAST != (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->block_info))
    {
        (void)d_AES_CmacFirstStream(p_param);
        if (NUM_FIRST_BLOCK < p_param->number)
        {
            err = d_AES_CmacMidStream(p_param);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        if (NUM_CMAC_SPECIAL_BLOCK <= p_param->number)
        {
            (void)d_AES_CmacFirstStream(p_param);
            if (NUM_CMAC_SPECIAL_BLOCK < p_param->number)
            {
                err = d_AES_CmacMidStream(p_param);
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
        if (ER_OK == err)
        {
            err = d_AES_CmacLastStream(p_param);
        }
        else
        {
            /* Do nothing */
        }
    } /* end of else */
    /* Clear control status */
    g_aes_drv_ctrl.status = D_AES_STS_NONE;

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0010]:[ICUMFW_UD_S4_D_AES_0010]
* Function name: d_AES_Cmac
* Description  : Do from setup to execution at once for speed up CMAC calculation.
* Arguments    : number     : Number of blocks to be encrypted
*                             Range: 0x0 - 0xFFFFFFFF
*                block_info : block information
*                             Range: D_AES_BLOCK_INFO_LAST
*                                    D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_aes_src  : address of input value
*                p_aes_dest : address of output value
*                p_aes_key  : address of key value
*                key_length : length of key
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_Cmac(aes_nblocks_t* p_param, const uint32_t* p_aes_key, uint16_t key_length)
{
    /* Context #0 only */
    uint32_t err;
    uint32_t saved_psw;
    uint32_t keysel;
    uint32_t i;
    uint32_t index;
    volatile uint32_t *p_key_reg[D_AES_KEY_NUM];

    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;
    keysel = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Set control status to execute or first */
    g_aes_drv_ctrl.status = (D_AES_STS_EXECUTE | D_AES_STS_FIRST);

    /* Set ICUM_AESKEY register */
    if (D_AES_KEY_SIZE_IN_BYTES_128BIT == (uint32_t)key_length)
    {   /* 128bit */
        keysel = D_AES_SET_KEYSEL_0;
    }
    else if (D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)key_length)
    {   /* 192bit */
        keysel = D_AES_SET_KEYSEL_1;
    }
    else
    {   /* 256bit */
        keysel = D_AES_SET_KEYSEL_2;
    }
    ICUM_AESKEY = (ICUM_AESKEY & ~(KEYSEL)) | keysel;

    /* Set ICUM_AESCTRL register */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    UNLOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
#endif
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(RST0)) | PRM_RST0_INIT;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    D_AES_WAIT_REG_VALUE(PRM_RST0_INIT, ICUM_AESCTRL, RST0);
    LOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
#endif
    /* Set ICUM_AESCCTL0 register */
    ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(MOD0 | ENCMODE0 | K0SEL)) |
                      (PRM_MOD0_CMAC
                     | PRM_ENCMODE0_ENC
                     | PRM_K0SEL_PLAIN);
    /* Set ICUM_AESCTRL register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST | CTX | BYTE_ORDER | DATREGSW)) |
                     (PRM_IDATDEST_KEY
                    | PRM_CTX_0
                    | PRM_BYTE_ORDER_APPLY
                    | PRM_DATREGSW_DIDI_DODI);
    /* Set ICUM_AESKEY register */
    ICUM_AESKEY = (ICUM_AESKEY & ~(EX_KEY)) | PRM_EX_KEY_SET_127_0;
   /* Start at a random index */
    D_OST_GetHardCounter(&index);
    index %= D_AES_KEY_NUM;
    /* Set registers for key input */
    *(p_key_reg + ADDR_0TH) = (volatile uint32_t *)&ICUM_AESDID0;
    *(p_key_reg + ADDR_1ST) = (volatile uint32_t *)&ICUM_AESDID1;
    *(p_key_reg + ADDR_2ND) = (volatile uint32_t *)&ICUM_AESDID2;
    *(p_key_reg + ADDR_3RD) = (volatile uint32_t *)&ICUM_AESDID3;
    /* Write to ICUM_AESDIDi register */
    /* key length 128 or 192 or 256[bit] */
    for (i = D_AES_ZERO_U32; i < D_AES_KEY_NUM; i++)
    {
        *(p_key_reg[index]) = p_aes_key[index];
        index = (index + 1U) % D_AES_KEY_NUM;
    }
    /* key length 192 or 256[bit] */
    if ((D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)key_length) ||
        (D_AES_KEY_SIZE_IN_BYTES_256BIT == (uint32_t)key_length))
    {

        /* Set ICUM_AESKEY register */
        ICUM_AESKEY = (ICUM_AESKEY & ~(EX_KEY)) | PRM_EX_KEY_SET_256_128;

        /* Write to ICUM_AESDIDi register for key length = 192bit */
        ICUM_AESDID0 = *(p_aes_key + ADDR_4TH);
        ICUM_AESDID1 = *(p_aes_key + ADDR_5TH);

        /* key length 256[bit] */
        if (D_AES_KEY_SIZE_IN_BYTES_256BIT == (uint32_t)key_length)
        {

            /* Write to ICUM_AESDIDi register for key length = 256bit */
            ICUM_AESDID2 = *(p_aes_key + ADDR_6TH);
            ICUM_AESDID3 = *(p_aes_key + ADDR_7TH);
        }
        else
        {
            /* Do nothing */
        }
    } /* end of if ((D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)key_length) || */
    else
    {
        /* Do nothing */
    }

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Set ICUM_AESCTRL register IDATDEST = 00: Block Cipher Input register (BCI) */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) | PRM_IDATDEST_BCI;
    /* Call functions by a number. */
    if (NUM_CMAC_SPECIAL_BLOCK <= p_param->number)
    {
        (void)d_AES_CmacFirstStream(p_param);
        if (NUM_CMAC_SPECIAL_BLOCK < p_param->number)
        {
            err = d_AES_CmacMidStream(p_param);
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
    if (ER_OK == err)
    {
        err = d_AES_CmacLastStream(p_param);
    }
    else
    {
        /* Do nothing */
    }
    /* Clear control status */
    g_aes_drv_ctrl.status = D_AES_STS_NONE;

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0017]:[ICUMFW_UD_S4_D_AES_0017]
* Function name: d_AES_CmacFirstStream
* Description  : Perform encryption processing.(CMAC)
* Arguments    : p_aes_src  : address of input value
* Return Value : ER_OK
***************************************************************************************/
static uint32_t d_AES_CmacFirstStream(aes_nblocks_t* p_param)
{
    uint32_t saved_psw;

    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* New operation, and first of data process */
    if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status))
    {
        /* Clear first or suspend status */
        g_aes_drv_ctrl.status &= ~(D_AES_STS_FIRST | D_AES_STS_SUSPEND);
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_CMAC_NO_ZERO
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_USE);
    }
    /* The resumption of a suspended operation, and first of data process */
    else
    {
        /* Clear suspend status */
        g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                         (PRM_ATTR_CMAC_NO_ZERO
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_RESUME
                        | PRM_NEW_KEY_USE);
    }
    /* Write to ICUM_AESDIDi register */
    ICUM_AESDID0 = *(p_param->p_aes_src + ADDR_0TH);
    ICUM_AESDID1 = *(p_param->p_aes_src + ADDR_1ST);
    ICUM_AESDID2 = *(p_param->p_aes_src + ADDR_2ND);
    ICUM_AESDID3 = *(p_param->p_aes_src + ADDR_3RD);
    /* read next data */
    p_param->p_aes_src += AES_BLOCK_SIZE;

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0018]:[ICUMFW_UD_S4_D_AES_0018]
* Function name: d_AES_CmacMidStream
* Description  : Perform encryption processing.(CMAC)
* Arguments    : number     : Number of blocks to be encrypted
*                             Range: 0x2 - 0xFFFFFFFF
*                block_info : block information
*                             Range: D_AES_BLOCK_INFO_LAST
*                p_aes_src  : address of input value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_CmacMidStream(aes_nblocks_t* p_param)
{
    uint32_t err;
    uint32_t calc_cnt;

    err = ER_OK;
    calc_cnt = D_AES_ZERO_U32;

    /* Calculate the number of repetitions of processing. */
    if (D_AES_BLOCK_INFO_LAST != (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->block_info))
    {
        calc_cnt = p_param->number - NUM_FIRST_BLOCK;
    }
    else
    {
        calc_cnt = p_param->number - NUM_CMAC_SPECIAL_BLOCK;
    }
    ASM_MACRO(".align 4");
    for (calc_cnt -= NUM_LAST_BLOCK ; END_CALC != calc_cnt; calc_cnt--)
    {
        (void)d_AES_Cmac1Block(p_param);
    }
    /* Wait ICUM_AES is in idle state */
    err = d_AES_WaitIdleState(STS, PRM_STS_IDLE);
    if (ER_OK == err)
    {
        (void)d_AES_Cmac1Block(p_param);
    }
    else
    {
        /* Do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0019]:[ICUMFW_UD_S4_D_AES_0019]
* Function name: d_AES_Cmac1Block
* Description  : Perform encryption processing for CMAC 1 block. (Not first or last stream).
* Arguments    : p_aes_src  : address of input value
* Return Value : ER_OK
***************************************************************************************/
static inline uint32_t d_AES_Cmac1Block(aes_nblocks_t* p_param)
{
    uint32_t saved_psw;

    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);
    if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
    {
        /* Clear suspend status */
        g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                        (PRM_ATTR_CMAC_NO_ZERO
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_RESUME
                        | PRM_NEW_KEY_USE);
    }
    else
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                        (PRM_ATTR_CMAC_NO_ZERO
                        | PRM_NO_LAST_BLOCK
                        | PRM_REM_128BIT
                        | PRM_IVSEL_NO_USE
                        | PRM_CONT_NEW_CONTINUE
                        | PRM_NEW_KEY_NO_USE);
    }
    /* Write to ICUM_AESDIDi register */
    ICUM_AESDID0 = *(p_param->p_aes_src + ADDR_0TH);
    ICUM_AESDID1 = *(p_param->p_aes_src + ADDR_1ST);
    ICUM_AESDID2 = *(p_param->p_aes_src + ADDR_2ND);
    ICUM_AESDID3 = *(p_param->p_aes_src + ADDR_3RD);
    /* read next data */
    p_param->p_aes_src += AES_BLOCK_SIZE;
    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0020]:[ICUMFW_UD_S4_D_AES_0020]
* Function name: d_AES_CmacLastStream
* Description  : Perform encryption processing.(CMAC)
* Arguments    : number     : Number of blocks to be encrypted
*                             Range: 0x0 - 0xFFFFFFFF
*                block_info : block information
*                             Range: D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_aes_src  : address of input value
*                p_aes_dest : address of output value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_CmacLastStream(aes_nblocks_t* p_param)
{
    uint32_t err;
    uint32_t input[AES_BLOCK_SIZE] = {D_AES_ZERO_U32};
    uint32_t total_block_size;
    uint16_t last_block_size;
    uint32_t saved_psw;

    err = ER_OK;
    total_block_size = BIT_SIZE_128_OR_MORE;
    last_block_size = D_AES_ZERO_U16;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* calculate last block size */
    last_block_size = (uint16_t)(D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->block_info);
    /* the last block less than 128 bits ? */
    if (D_AES_BLOCK_INFO_SIZE_MAX > (uint32_t)last_block_size)
    {
        /* Shape the last block */
        (void)d_AES_LastBlock(input, p_param->p_aes_src, last_block_size);
        /* New operation, and first of data process */
        if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status))
        {
            /* Total input block less than 128 bits */
            total_block_size = BIT_SIZE_127_OR_LESS;
            /* Clear first or suspend status */
            g_aes_drv_ctrl.status &= ~(D_AES_STS_FIRST | D_AES_STS_SUSPEND);
            /* Is the empty string ? */
            if ((uint32_t)D_AES_BLOCK_INFO_SIZE_MIN ==
               (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->block_info))
            {
                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                                 (PRM_ATTR_CMAC_ZERO
                                | PRM_LAST_BLOCK
                                | PRM_REM_128BIT
                                | PRM_IVSEL_USE
                                | PRM_CONT_NEW_CONTINUE
                                | PRM_NEW_KEY_USE);
            }
            else
            {
                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                                 (PRM_ATTR_CMAC_NO_ZERO
                                | PRM_LAST_BLOCK
                                | (REM & ((uint32_t)p_param->block_info << SHIFT_REM))
                                | PRM_IVSEL_USE
                                | PRM_CONT_NEW_CONTINUE
                                | PRM_NEW_KEY_USE);
            }
        } /* end of if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status)) */
        else if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
        {
            /* Clear suspend status */
            g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                             (PRM_ATTR_CMAC_NO_ZERO
                            | PRM_LAST_BLOCK
                            | (REM & ((uint32_t)p_param->block_info << SHIFT_REM))
                            | PRM_IVSEL_NO_USE
                            | PRM_CONT_RESUME
                            | PRM_NEW_KEY_USE);
        }
        else
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                             (PRM_ATTR_CMAC_NO_ZERO
                            | PRM_LAST_BLOCK
                            | (REM & ((uint32_t)p_param->block_info << SHIFT_REM))
                            | PRM_IVSEL_NO_USE
                            | PRM_CONT_NEW_CONTINUE
                            | PRM_NEW_KEY_NO_USE);
        }
        /* Write to ICUM_AESDIDi register */
        ICUM_AESDID0 = *(input + ADDR_0TH);
        ICUM_AESDID1 = *(input + ADDR_1ST);
        ICUM_AESDID2 = *(input + ADDR_2ND);
        ICUM_AESDID3 = *(input + ADDR_3RD);
    } /* end of if (D_AES_BLOCK_INFO_SIZE_MAX > (uint32_t)last_block_size) */
    else
    {
        /* New operation, and first of data process */
        if (D_AES_STS_FIRST == (D_AES_STS_FIRST & g_aes_drv_ctrl.status))
        {
            total_block_size = BIT_SIZE_127_OR_LESS;
            /* Clear first or suspend status */
            g_aes_drv_ctrl.status &= ~(D_AES_STS_FIRST | D_AES_STS_SUSPEND);
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                             (PRM_ATTR_CMAC_NO_ZERO
                            | PRM_LAST_BLOCK
                            | PRM_REM_128BIT
                            | PRM_IVSEL_USE
                            | PRM_CONT_NEW_CONTINUE
                            | PRM_NEW_KEY_USE);
        }
        else if (D_AES_STS_SUSPEND == (D_AES_STS_SUSPEND & g_aes_drv_ctrl.status))
        {
            /* Clear suspend status */
            g_aes_drv_ctrl.status &= ~D_AES_STS_SUSPEND;
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                             (PRM_ATTR_CMAC_NO_ZERO
                            | PRM_LAST_BLOCK
                            | PRM_REM_128BIT
                            | PRM_IVSEL_NO_USE
                            | PRM_CONT_RESUME
                            | PRM_NEW_KEY_USE);
        }
        else
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | CONT | NEW_KEY)) |
                             (PRM_ATTR_CMAC_NO_ZERO
                            | PRM_LAST_BLOCK
                            | PRM_REM_128BIT
                            | PRM_IVSEL_NO_USE
                            | PRM_CONT_NEW_CONTINUE
                            | PRM_NEW_KEY_NO_USE);
        }
        /* Write to ICUM_AESDIDi register */
        ICUM_AESDID0 = *(p_param->p_aes_src + ADDR_0TH);
        ICUM_AESDID1 = *(p_param->p_aes_src + ADDR_1ST);
        ICUM_AESDID2 = *(p_param->p_aes_src + ADDR_2ND);
        ICUM_AESDID3 = *(p_param->p_aes_src + ADDR_3RD);
    } /* end of else */
    /* Set ICUM_AESCTRL register ODATSRC = 00: Block Cipher Output register (BCO) */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC)) | PRM_ODATSRC_BCO;
    /* Wait ICUM_AES is in idle state */
    err = d_AES_WaitIdleState(STS, PRM_STS_IDLE);
    if (ER_OK == err)
    {
        /* Is not the total input block less than 128 bits ? */
        if (BIT_SIZE_128_OR_MORE ==total_block_size)
        {
            /* Dummy read from ICUM_AESDODi register */
            ICUM_AESDOD0;
            ICUM_AESDOD1;
            ICUM_AESDOD2;
            ICUM_AESDOD3;
        }
        else
        {
            /* Do nothing */
        }
        /* Read from ICUM_AESDODi register */
        *(p_param->p_aes_dest + ADDR_0TH) = ICUM_AESDOD0;
        *(p_param->p_aes_dest + ADDR_1ST) = ICUM_AESDOD1;
        *(p_param->p_aes_dest + ADDR_2ND) = ICUM_AESDOD2;
        *(p_param->p_aes_dest + ADDR_3RD) = ICUM_AESDOD3;
    }
    else
    {
        /* Do nothing */
    }
    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0071]:[ICUMFW_UD_S4_D_AES_0071]
* Function name: d_AES_LastBlock
* Description  : Shape the last block of less than 128 bits to 128 bits
* Arguments    : p_in_data : Last block after adjustment
*                p_aes_src : Last block before adjustment
*                last_block_size : Size of last block
* Return Value : ER_OK
***************************************************************************************/
uint32_t d_AES_LastBlock(uint32_t* p_in_data, uint32_t* p_aes_src, uint16_t last_block_size)
{
    uint32_t input_index;
    uint32_t input_count;
    uint32_t remainder_byte_index;
    uint32_t remainder_long_bit_size;
    uint32_t remainder_short_bit_size;
    uint32_t remainder_count;
    uint8_t  input_data[D_AES_BLOCK_SIZE_4] = {D_AES_ZERO_U8};
    uint8_t* p_aes_last_src;

    input_index = D_AES_ZERO_U32;
    input_count = D_AES_ZERO_U32;
    remainder_byte_index = D_AES_ZERO_U32;
    remainder_long_bit_size = D_AES_ZERO_U32;
    remainder_short_bit_size = D_AES_ZERO_U32;
    remainder_count = D_AES_ZERO_U32;
    p_aes_last_src = NULL;

    /* AES direct data input unit */
    input_index = (uint32_t)last_block_size / BLOCK_INFO_INPUT_UNIT;
    /* Remainder of AES direct data input unit */
    remainder_long_bit_size = (uint32_t)last_block_size % BLOCK_INFO_INPUT_UNIT;
    /* Adjust the size when there is no remainder */
    if (D_AES_ZERO_U32 == remainder_long_bit_size)
    {
        input_index--;
    }
    else
    {
        /* Do nothing */
    }
    /* Check all input values */
    for (input_count = D_AES_ZERO_U32; input_count < D_AES_SET_LOOP_4; input_count++)
    {
        /* There is no remainder */
        if ((input_count < input_index) ||
           ((input_count == input_index) && (D_AES_ZERO_U32 == remainder_long_bit_size)))
        {
            /* Copy AES direct data input unit */
            L_Toolbox_MemCpy((uint8_t *)(p_in_data + input_count),
                             (const uint8_t *)(p_aes_src + input_count), (int16_t)D_AES_SET_4BYTE);
        }
        /* There is a remainder */
        else
        {
            /* Set zero to AES direct data input unit */
            L_Toolbox_MemSet((uint8_t *)(p_in_data + input_count), (uint8_t)D_AES_SET_ZERO_16, (int16_t)D_AES_SET_4BYTE);
            /* Unit with a remainder */
            if (input_count == input_index)
            {
                /* Convert to 8 bit unit */
                p_aes_last_src = (uint8_t*)(p_aes_src + input_count);
                /* input_data is cleared to 0 */
                L_Toolbox_MemSet(input_data, (uint8_t)D_AES_SET_ZERO_16, (int16_t)D_AES_SET_4BYTE);
                /* Minimum processing unit */
                remainder_byte_index = remainder_long_bit_size / BLOCK_INFO_ACCESS_UNIT;
                /* Remainder of minimum processing unit */
                remainder_short_bit_size = remainder_long_bit_size % BLOCK_INFO_ACCESS_UNIT;
                /* Adjust the size when there is no remainder */
                if (D_AES_ZERO_U32 == remainder_short_bit_size)
                {
                    remainder_byte_index--;
                }
                else
                {
                    /* Do nothing */
                }
                /* Copy data in 8 bit units */
                for (remainder_count = D_AES_ZERO_U16; remainder_count < remainder_byte_index; remainder_count++)
                {
                    input_data[remainder_count] = *(p_aes_last_src + remainder_count);
                }
                /* Adjust the size when there is a remainder */
                if (D_AES_ZERO_U32 == remainder_short_bit_size)
                {
                    input_data[remainder_count] = *(p_aes_last_src + remainder_count);
                }
                else
                {
                    /* Set zero after the valid bit */
                    input_data[remainder_count] = (uint8_t)((uint32_t)*(p_aes_last_src + remainder_count)
                                                          & (D_AES_SET_VALID_BIT >> remainder_short_bit_size));
                }
                /* Copy AES direct data input unit */
                L_Toolbox_MemCpy((uint8_t *)(p_in_data + input_count), (const uint8_t *)input_data, (int16_t)D_AES_SET_4BYTE);
            } /* end of if (input_count == input_index) */
            else
            {
                /* Do nothing */
            }
        } /* end of else */
    } /* end of for (input_count = D_AES_ZERO_U32; input_count < D_AES_SET_LOOP_4; input_count++) */

    return ER_OK;
}
PRAGMA(ghs section text = default)
PRAGMA(ghs section rodata = default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0072]:[ICUMFW_UD_S4_D_AES_0072]
* Function name: d_AES_WaitIdleState
* Description  : AES driver calculation processing is complete.
                 Wait for AES operation, if not complete.
* Arguments    : reg_item : STS or GHSTS(GCM only)
*                param : PRM_STS_IDLE or PRM_GHSTS_VALID(GCM only)
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static inline uint32_t d_AES_WaitIdleState(const uint32_t reg_item, const uint32_t param)
{
    uint32_t err;
    uint32_t timeout;

    err = ER_D_AES_TIMEOUT_ERROR;

    for (timeout = TIMEOUT_INIT; timeout > TIMEOUT_ELAPSED; timeout--)
    {
        /* ICUM_AESSTR.STS or ICUM_AESSTR.GHSTS = 0: ICUM_AES is in idle state. */
        if (param == (reg_item & ICUM_AESSTR))
        {
            err = ER_OK;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0107]:[ICUMFW_UD_S4_D_AES_0107]
* Function name: D_AES_ReleaseKeyRange
* Description  : Release of key length Restriction.
* Arguments    : key_length         : key length
*                              Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                     D_AES_KEY_SIZE_IN_BYTES_192BIT
*                                     D_AES_KEY_SIZE_IN_BYTES_256BIT
*                key_length_id      : key length usage management id
*                              Range: 0x1 - 0xFFFFFFFF
* Return Value : ER_OK
***************************************************************************************/
uint32_t D_AES_ReleaseKeyRange(aes_release_t* p_param)
{
    uint32_t err;

    err = ER_OK;

    err = d_AES_FreeKeyRestriction(p_param->key_length, p_param->key_length_id);

    return err;

}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0005]:[ICUMFW_UD_S4_D_AES_0005]
* Function name: D_AES_InitJob
* Description  : Set parameters of crypto AES.
* Arguments    : cipher_mode : cipher mode
*                              Range: D_AES_CIPHER_MODE_ECB
*                                     D_AES_CIPHER_MODE_CBC
*                                     D_AES_CIPHER_MODE_CTR
*                                     D_AES_CIPHER_MODE_CFB
*                                     D_AES_CIPHER_MODE_OFB
*                                     D_AES_CIPHER_MODE_CMAC
*                direction   : cipher direction
*                              Range: D_AES_DIR_ENCRYPTION
*                                     D_AES_DIR_DECRYPTION
*                p_aes_key   : address of key value
*                key_length  : key length
*                              Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                     D_AES_KEY_SIZE_IN_BYTES_192BIT
*                                     D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv    : address of IV value
*                p_context   : address of intermediate value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_PENDING
***************************************************************************************/
uint32_t D_AES_InitJob(aes_init_t* p_param)
{
    uint32_t err;
    aes_context_t *p_context;

    err = ER_OK;
    p_context = NULL;

    /* Check if the context address is null. */
    if (NULL == p_param->p_context)
    {
        err = ER_PARAM;
    }
    /* Check key size */
    else if ((D_AES_KEY_SIZE_IN_BYTES_128BIT != (uint32_t)p_param->key_length) &&
             (D_AES_KEY_SIZE_IN_BYTES_192BIT != (uint32_t)p_param->key_length) &&
             (D_AES_KEY_SIZE_IN_BYTES_256BIT != (uint32_t)p_param->key_length))
    {
        err = ER_PARAM;
    }
    else
    {
        /* Set the intermediate value area. */
        p_context = (aes_context_t*)p_param->p_context;
        /* Check the cipher mode. */
        if ((D_AES_CIPHER_MODE_CMAC != (uint32_t)p_param->cipher_mode)
         && (D_AES_CIPHER_MODE_ECB  != (uint32_t)p_param->cipher_mode))
        {
            /* Alignment check of IV address. */
            if (ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_aes_iv))
            {
                /* Store the IV value in the context. */
                L_Toolbox_MemCpy((uint8_t *)p_context->aes_iv,
                                 (const uint8_t *)p_param->p_aes_iv,
                                 AES_BLOCK_SIZE_IN_BYTES);
            }
            else
            {
                err = ER_D_AES_ALIGN_ERROR;
            }
        } /* end of if ((D_AES_CIPHER_MODE_CMAC != (uint32_t)p_param->cipher_mode) */
        else
        {
            /* Do nothing */
        }

        if (ER_OK == err)
        {
            err = d_AES_GetKeyRestriction(p_param->key_length, &((aes_context_t*)p_param->p_context)->key_length_id);
        }
        else
        {
            /* Do nothing */
        }

        if (ER_OK == err)
        {
            p_context->cipher_mode = (uint8_t)p_param->cipher_mode;
            p_context->direction = p_param->direction;
            p_context->key_length = p_param->key_length;
            /* Store the key value in the context. */
            L_Toolbox_MemCpy((uint8_t *)p_context->aes_key,
                             (const uint8_t *)p_param->p_aes_key,
                             (int16_t)p_param->key_length);
        }
        else
        {
            /* Do nothing */
        }
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0006]:[ICUMFW_UD_S4_D_AES_0006]
* Function name: D_AES_UpdateJob
* Description  : Execute encryption of crypto AES.
* Arguments    : number     : Number of blocks to be encrypted
*                             Range: 0x1 - 0xFFFFFFFF
*                first      : first flag
*                             Range: D_AES_NOT_FIRST_CIPHER
*                                    D_AES_FIRST_CIPHER
*                p_aes_src  : address of input value
*                p_context  : address of intermediate value
*                p_aes_dest : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
uint32_t D_AES_UpdateJob(aes_update_t* p_param)
{
    uint32_t err;
    aes_context_t *p_context;
    aes_setparam_t setparam;
    aes_nblocks_t nblocks;

    err = ER_OK;
    p_context = NULL;
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_setparam_t));
    L_Toolbox_MemSet((uint8_t *)&nblocks, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_nblocks_t));

    /* Check if the context address is null. */
    if (NULL == p_param->p_context)
    {
        err = ER_PARAM;
    }
    /* Check the range of number. */
    else if (D_AES_NUMBER_MIN > p_param->number)
    {
        err = ER_PARAM;
    }
    /* Check the range of first. */
    else if (D_AES_FIRST_CIPHER < (uint32_t)p_param->first)
    {
        err = ER_PARAM;
    }
    /* Alignment check of input address and output address. */
    else if ((ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_src))
          || (ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_dest)))
    {
        err = ER_D_AES_ALIGN_ERROR;
    }
    else
    {
        /* Set the intermediate value area. */
        p_context = (aes_context_t*)p_param->p_context;
        /* Create parameter group for batch setting. */
        setparam.cipher_mode = p_context->cipher_mode;
        setparam.direction = p_context->direction;
        setparam.first = (uint8_t)p_param->first;
        setparam.p_aes_key = p_context->aes_key;
        setparam.p_aes_iv = p_context->aes_iv;
        setparam.key_type = (uint8_t)D_AES_KEYTYPE_PLAIN;
        setparam.key_length = p_context->key_length;
        /* Set a parameter group for batch setting. */
        (void)d_AES_SetParam(&setparam);
        /* Create a parameter group for cryptographic processing. */
        nblocks.number = p_param->number;
        nblocks.p_aes_src = p_param->p_aes_src;
        nblocks.p_aes_dest = p_param->p_aes_dest;
        nblocks.block_info = (uint16_t)((uint32_t)p_param->first << SHIFT_AES_FIRST_CIPHER);
        if (D_AES_CIPHER_MODE_CMAC == (uint32_t)p_context->cipher_mode)
        {
            /* Execute cryptographic processing. */
            D_FLS_CfPreLoad(D_FLS_PRELOAD_ON);
            err = d_AES_ProcessNBlocks_CMAC(&nblocks);
            D_FLS_CfPreLoad(D_FLS_PRELOAD_OFF);
        }
        else
        {
            /* Execute cryptographic processing. */
            err = d_AES_ProcessNBlocks(&nblocks);
        }

        if (ER_OK == err)
        {
            /* Save intermediate value in context. */
            (void)d_AES_GetIV(p_context->aes_iv);
        }
        else
        {
            /* Do nothing */
        }
    } /* end of else */

    if ((ER_OK != err) && (NULL != p_param->p_context))
    {
        (void)d_AES_FreeKeyRestriction(((aes_context_t*)p_param->p_context)->key_length,
                                       ((aes_context_t*)p_param->p_context)->key_length_id);
    }
    else
    {
        /* do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0007]:[ICUMFW_UD_S4_D_AES_0007]
* Function name: D_AES_FinishJob
* Description  : Finish execution of crypto AES.
* Arguments    : number             : Number of blocks to be encrypted
*                                     Range: 0x0 - 0xFFFFFFFF
*                last_block_in_bits : last block in bits
*                                     Range: D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                first              : first flag
*                                     Range: D_AES_NOT_FIRST_CIPHER
*                                            D_AES_FIRST_CIPHER
*                p_aes_src          : address of input value
*                p_context          : address of intermediate value
*                p_aes_dest         : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
uint32_t D_AES_FinishJob(aes_finish_t* p_param)
{
    uint32_t err;
    aes_context_t *p_context;
    aes_setparam_t setparam;
    aes_nblocks_t nblocks;

    err = ER_OK;
    p_context = NULL;
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_setparam_t));
    L_Toolbox_MemSet((uint8_t *)&nblocks, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_nblocks_t));

    /* Check if the context address is null. */
    if (NULL == p_param->p_context)
    {
        err = ER_PARAM;
    }
    /* Check range last_block_in_bits, context */
    else if (D_AES_BLOCK_INFO_SIZE_MAX
            < (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->last_block_in_bits))
    {
        err = ER_PARAM;
    }
    /* Check the range of first. */
    else if (D_AES_FIRST_CIPHER < (uint32_t)p_param->first)
    {
        err = ER_PARAM;
    }
    /* Alignment check of input address and output address. */
    else if ((ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_src))
          || (ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_dest)))
    {
        err = ER_D_AES_ALIGN_ERROR;
    }
    else
    {
        /* Set the intermediate value area. */
        p_context = (aes_context_t*)p_param->p_context;
        if (D_AES_CIPHER_MODE_CMAC == (uint32_t)p_context->cipher_mode)
        {
            /* Create parameter group for batch setting. */
            setparam.cipher_mode = p_context->cipher_mode;
            setparam.direction = p_context->direction;
            setparam.first = (uint8_t)p_param->first;
            setparam.p_aes_key = p_context->aes_key;
            setparam.p_aes_iv = p_context->aes_iv;
            setparam.key_type = (uint8_t)D_AES_KEYTYPE_PLAIN;
            setparam.key_length = p_context->key_length;
            /* Set a parameter group for batch setting. */
            (void)d_AES_SetParam(&setparam);
            /* Create a parameter group for cryptographic processing. */
            nblocks.number = p_param->number;
            nblocks.p_aes_src = p_param->p_aes_src;
            nblocks.block_info = (uint16_t)(((uint32_t)p_param->first << SHIFT_AES_FIRST_CIPHER)
                                          | (D_AES_BLOCK_INFO_LAST)
                                          | (D_AES_BLOCK_INFO_SIZE
                                           & (uint32_t)p_param->last_block_in_bits));
            nblocks.p_aes_dest = p_param->p_aes_dest;
            /* Execute cryptographic processing. */
            D_FLS_CfPreLoad(D_FLS_PRELOAD_ON);
            err = d_AES_ProcessNBlocks_CMAC(&nblocks);
            D_FLS_CfPreLoad(D_FLS_PRELOAD_OFF);
        }
        else
        {
            /* do nothing */
        }
    } /* end of else */
    if (NULL != p_param->p_context)
    {
        if (ER_OK == err)
        {
            err = d_AES_FreeKeyRestriction(((aes_context_t*)p_param->p_context)->key_length,
                                           ((aes_context_t*)p_param->p_context)->key_length_id);
        }
        else
        {
            (void)d_AES_FreeKeyRestriction(((aes_context_t*)p_param->p_context)->key_length,
                                           ((aes_context_t*)p_param->p_context)->key_length_id);
        }
    }
    else
    {
        /* do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0008]:[ICUMFW_UD_S4_D_AES_0008]
* Function name: D_AES_ExecutionAtOnce
* Description  : Execute encryption of crypto AES at once.
* Arguments    : cipher_mode        : cipher mode
*                                     Range: D_AES_CIPHER_MODE_ECB
*                                            D_AES_CIPHER_MODE_CBC
*                                            D_AES_CIPHER_MODE_CTR
*                                            D_AES_CIPHER_MODE_CFB
*                                            D_AES_CIPHER_MODE_OFB
*                                            D_AES_CIPHER_MODE_CMAC
*                                            D_AES_CIPHER_MODE_MP
*                direction          : cipher direction
*                                     Range: D_AES_DIR_ENCRYPTION
*                                            D_AES_DIR_DECRYPTION
*                number             : Number of blocks to be encrypted
*                                     Range: 0x0 - 0xFFFFFFFF
*                key_type           : key type
*                                     Range: D_AES_KEYTYPE_PLAIN
*                                            D_AES_KEYTYPE_ROMKEY_0
*                last_block_in_bits : last block in bits
*                                     Range: D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_aes_key          : address of key value
*                key_length         : key length
*                                     Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                            D_AES_KEY_SIZE_IN_BYTES_192BIT
*                                            D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv           : address of IV value
*                p_aes_src          : address of input value
*                async              : sync/async transfer mode
*                                     Range: D_AES_TRANS_SYNC
*                                            D_AES_TRANS_ASYNC
*                p_aes_dest         : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_PENDING
*                ER_D_AES_SEQUENCE_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
uint32_t D_AES_ExecutionAtOnce(aes_all_at_once_t* p_param)
{
    uint32_t err;
    uint8_t mem_check_src;
    uint8_t mem_check_dst;

    err = ER_OK;

    /* Check range last_block_in_bits */
    if (D_AES_BLOCK_INFO_SIZE_MAX <
        (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->last_block_in_bits))
    {
        err = ER_PARAM;
    }
    /* Check key size */
    else if ((D_AES_KEY_SIZE_IN_BYTES_128BIT != (uint32_t)p_param->key_length) &&
             (D_AES_KEY_SIZE_IN_BYTES_192BIT != (uint32_t)p_param->key_length) &&
             (D_AES_KEY_SIZE_IN_BYTES_256BIT != (uint32_t)p_param->key_length))
    {
        err = ER_PARAM;
    }
    /* Check sync/async transfer mode */
    else if ((D_AES_TRANS_SYNC  != (uint32_t)p_param->async) &&
             (D_AES_TRANS_ASYNC != (uint32_t)p_param->async))
    {
        err = ER_PARAM;
    }
    /* Alignment check of input address and output address. */
    else if ((ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_src))
          || (ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_dest)))
    {
        err = ER_D_AES_ALIGN_ERROR;
    }
    /* Check range initialization vector */
    else if ((D_AES_CIPHER_MODE_CMAC != (uint32_t)p_param->cipher_mode)
          && (D_AES_CIPHER_MODE_ECB != (uint32_t)p_param->cipher_mode))
    {
        /* Alignment check of IV address. */
        if (ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_aes_iv))
        {
            /* Do nothing */
        }
        else
        {
            err = ER_D_AES_ALIGN_ERROR;
        }
    }
    else
    {
        /* Do nothing */
    }
    if (ER_OK == err)
    {
        err = d_AES_GetKeyRestriction(p_param->key_length, &g_aes_atonce_id_use_mng);
        if (ER_OK == err)
        {
            /* Check that memory address is outside the range of ICUM Local RAM */
            err = d_AES_CheckMemory((uint32_t)p_param->p_aes_src, p_param->number * D_AES_SET_16BYTE, &mem_check_src);
            if (ER_OK == err)
            {
                if (D_AES_CIPHER_MODE_CMAC == (uint32_t)p_param->cipher_mode)
                {
                    mem_check_dst = D_AES_CHK_OK;
                }
                else
                {
                    /* Check that memory address is outside the range of ICUM Local RAM */
                    err = d_AES_CheckMemory((uint32_t)p_param->p_aes_dest, p_param->number * D_AES_SET_16BYTE, &mem_check_dst);
                }
                if (ER_OK == err)
                {
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
                    if ((ALIGN_OK == (ALIGN256 & (uint32_t)p_param->p_aes_src))
                     && ((ALIGN_OK == (ALIGN256 & (uint32_t)p_param->p_aes_dest))
                     || (D_AES_CIPHER_MODE_CMAC == (uint32_t)p_param->cipher_mode))
#else
                    if ((ALIGN_OK == (ALIGN8 & (uint32_t)p_param->p_aes_dest))
#endif
                     && (D_AES_CIPHER_MODE_MP != (uint32_t)p_param->cipher_mode)
                     && (0u != (uint32_t)p_param->number)
                     && ((uint32_t)D_AES_CHK_OK == (uint32_t)mem_check_src)
                     && ((uint32_t)D_AES_CHK_OK == (uint32_t)mem_check_dst)
                     && (((uint32_t)D_AES_SET128 == (uint32_t)p_param->last_block_in_bits) ||
                        ((uint32_t)D_AES_ZERO_U32 == (uint32_t)p_param->last_block_in_bits)))
                    {
                        err = d_AES_ExecutionAtOnceDtf(p_param);
                    }
                    else
                    {
                        err = d_AES_ExecutionAtOnceIcup(p_param);
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
            if (ER_IN_PROCESS == err)
            {
                /* do nothing */
            }
            else if (ER_OK == err)
            {
                err = d_AES_FreeKeyRestriction(p_param->key_length, g_aes_atonce_id_use_mng);
            }
            else
            {
                (void)d_AES_FreeKeyRestriction(p_param->key_length, g_aes_atonce_id_use_mng);
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

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0009]:[ICUMFW_UD_S4_D_AES_0009]
* Function name: d_AES_ExecutionAtOnceIcup
* Description  : Execute encryption of crypto AES at once. (for ICUP)
* Arguments    : cipher_mode        : cipher mode
*                                     Range: D_AES_CIPHER_MODE_ECB
*                                            D_AES_CIPHER_MODE_CBC
*                                            D_AES_CIPHER_MODE_CTR
*                                            D_AES_CIPHER_MODE_CFB
*                                            D_AES_CIPHER_MODE_OFB
*                                            D_AES_CIPHER_MODE_CMAC
*                                            D_AES_CIPHER_MODE_MP
*                direction          : cipher direction
*                                     Range: D_AES_DIR_ENCRYPTION
*                                            D_AES_DIR_DECRYPTION
*                number             : Number of blocks to be encrypted
*                                     Range: 0x0 - 0xFFFFFFFF
*                key_type           : key type
*                                     Range: D_AES_KEYTYPE_PLAIN
*                                            D_AES_KEYTYPE_ROMKEY_0
*                last_block_in_bits : last block in bits
*                                     Range: D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_aes_key          : address of key value
*                key_length         : key length
*                                     Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                            D_AES_KEY_SIZE_IN_BYTES_192BIT
*                                            D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv           : address of IV value
*                p_aes_src          : address of input value
*                p_aes_dest         : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_PENDING
***************************************************************************************/
static uint32_t d_AES_ExecutionAtOnceIcup(aes_all_at_once_t* p_param)
{
    uint32_t err;
    aes_nblocks_t nblocks;
    aes_setparam_t setparam;

    err = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&nblocks, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_nblocks_t));
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_setparam_t));

    if (D_AES_CIPHER_MODE_CMAC == (uint32_t)p_param->cipher_mode)
    {
        /* Set parameters to cmac */
        nblocks.block_info = (uint16_t)(D_AES_BLOCK_INFO_FIRST
                                      | D_AES_BLOCK_INFO_LAST
                                      | (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->last_block_in_bits));
        nblocks.number = p_param->number;
        nblocks.p_aes_src = p_param->p_aes_src;
        nblocks.p_aes_dest = p_param->p_aes_dest;
        /* Execute encryption processing */
        D_FLS_CfPreLoad(D_FLS_PRELOAD_ON);
        err = d_AES_Cmac(&nblocks, p_param->p_aes_key, p_param->key_length);
        D_FLS_CfPreLoad(D_FLS_PRELOAD_OFF);
    } /* end of if (D_AES_CIPHER_MODE_CMAC == (uint32_t)p_param->cipher_mode) */
    else
    {
        /* Create parameter group for batch setting. */
        setparam.cipher_mode = (uint8_t)p_param->cipher_mode;
        setparam.direction = p_param->direction;
        setparam.key_type = (uint8_t)p_param->key_type;
        setparam.first = D_AES_FIRST_CIPHER;
        setparam.p_aes_key = p_param->p_aes_key;
        setparam.p_aes_iv = p_param->p_aes_iv;
        setparam.key_length = p_param->key_length;
        /* Set a parameter group for batch setting. */
        (void)d_AES_SetParam(&setparam);
        /* Create a parameter group for cryptographic processing. */
        nblocks.number = p_param->number;
        nblocks.block_info = D_AES_BLOCK_INFO_FIRST;
        nblocks.p_aes_src = p_param->p_aes_src;
        nblocks.p_aes_dest = p_param->p_aes_dest;
        /* Execute cryptographic processing. */
        err = d_AES_ProcessNBlocks(&nblocks);
    }/* end of else */

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0021]:[ICUMFW_UD_S4_D_AES_0021]
* Function name: D_AES_InitJobXts
* Description  : Set parameters of crypto AES XTS.
* Arguments    : direction       : cipher direction
*                                  Range: D_AES_DIR_ENCRYPTION
*                                         D_AES_DIR_DECRYPTION
*                p_aes_key       : address of the XTS-AES key value
*                p_aes_key_tweak : address of the tweak key value
*                key_length      : key length
*                                  Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                         D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv        : address of the 128-bit tweak value
*                p_context       : address of intermediate value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_PENDING
***************************************************************************************/
uint32_t D_AES_InitJobXts(aes_init_xts_t* p_param)
{
    uint32_t err;
    aes_context_t *p_context;
    aes_setparam_t setparam;

    err = ER_OK;
    p_context = NULL;
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_setparam_t));

    /* Check if the context address is null. */
    if (NULL == p_param->p_context)
    {
        err = ER_PARAM;
    }
    /* Alignment check of tweak address. */
    else if (ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_iv))
    {
        err = ER_D_AES_ALIGN_ERROR;
    }
    /* Check key size */
    else if ((D_AES_KEY_SIZE_IN_BYTES_128BIT != (uint32_t)p_param->key_length) &&
             (D_AES_KEY_SIZE_IN_BYTES_256BIT != (uint32_t)p_param->key_length))
    {
        err = ER_PARAM;
    }
    else
    {
        err = d_AES_GetKeyRestriction(p_param->key_length, &((aes_context_t*)p_param->p_context)->key_length_id);
        if (ER_OK == err)
        {
            /* Set the intermediate value area. */
            p_context = (aes_context_t*)p_param->p_context;
            p_context->cipher_mode = (uint8_t)D_AES_CIPHER_MODE_XTS;
            p_context->direction = p_param->direction;
            p_context->key_length = p_param->key_length;
            /* Store the key value in the context. */
            L_Toolbox_MemCpy((uint8_t *)p_context->aes_key,
                             (const uint8_t *)p_param->p_aes_key,
                             (int16_t)p_param->key_length);
            /* Store the IV value in the context. */
            L_Toolbox_MemCpy((uint8_t *)p_context->aes_iv,
                             (const uint8_t *)p_param->p_aes_iv,
                             AES_BLOCK_SIZE_IN_BYTES);
            /* Create parameter group for batch setting. */
            setparam.cipher_mode = (uint8_t)D_AES_CIPHER_MODE_XTS;
            setparam.direction = p_context->direction;
            setparam.first = D_AES_FIRST_CIPHER;
            setparam.p_aes_key = p_param->p_aes_key_tweak;
            setparam.p_aes_iv = NULL;
            setparam.key_type = (uint8_t)D_AES_KEYTYPE_PLAIN;
            setparam.key_length = p_param->key_length;
            /* Set a parameter group for batch setting. */
            (void)d_AES_SetParam(&setparam);
            /* Set tweak value. */
            err = d_AES_SetTweakData(p_context->aes_iv);
            if (ER_OK == err)
            {
                /* Save intermediate value in context. */
                (void)d_AES_GetIV(p_context->aes_iv);
            }
            else
            {
                (void)d_AES_FreeKeyRestriction(p_context->key_length,
                                              ((aes_context_t*)p_param->p_context)->key_length_id);
            }
        }
        else
        {
            /* Do nothing */
        }
    } /* end of else */

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0023]:[ICUMFW_UD_S4_D_AES_0023]
* Function name: D_AES_UpdateJobXts
* Description  : Execute encryption of crypto AES XTS.
* Arguments    : number     : Number of blocks to be encrypted or decrypted
*                             Range: 0x1 - 0xFFFFFFFF
*                p_aes_src  : address of input value
*                p_context  : address of intermediate value
*                p_aes_dest : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
uint32_t D_AES_UpdateJobXts(aes_update_t* p_param)
{
    uint32_t err;
    aes_context_t *p_context;
    aes_setparam_t setparam;
    aes_nblocks_t nblocks;

    err = ER_OK;
    p_context = NULL;
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_setparam_t));
    L_Toolbox_MemSet((uint8_t *)&nblocks, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_nblocks_t));

    /* Check if the context address is null. */
    if (NULL == p_param->p_context)
    {
        err = ER_PARAM;
    }
    /* Check the range of number. */
    else if (D_AES_NUMBER_MIN > p_param->number)
    {
        err = ER_PARAM;
    }
    /* Alignment check of input address and output address. */
    else if ((ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_src))
          || (ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_dest)))
    {
        err = ER_D_AES_ALIGN_ERROR;
    }
    else
    {
        /* Set the intermediate value area. */
        p_context = (aes_context_t*)p_param->p_context;
        /* Create parameter group for batch setting. */
        setparam.cipher_mode = (uint8_t)D_AES_CIPHER_MODE_XTS;
        setparam.direction = p_context->direction;
        setparam.first = D_AES_NOT_FIRST_CIPHER;
        setparam.p_aes_key = p_context->aes_key;
        setparam.p_aes_iv = p_context->aes_iv;
        setparam.key_type = (uint8_t)D_AES_KEYTYPE_PLAIN;
        setparam.key_length = p_context->key_length;
        /* Set a parameter group for batch setting. */
        (void)d_AES_SetParam(&setparam);
        /* Create a parameter group for cryptographic processing. */
        nblocks.number = p_param->number;
        nblocks.p_aes_src = p_param->p_aes_src;
        nblocks.block_info = 0x0000u;
        nblocks.p_aes_dest = p_param->p_aes_dest;
        /* Execute cryptographic processing. */
        err = d_AES_ProcessNBlocks_XTS(&nblocks);
        if (ER_OK == err)
        {
            /* Save intermediate value in context. */
            (void)d_AES_GetIV(p_context->aes_iv);
        }
        else
        {
            /* Do nothing */
        }
    } /* end of else */

    if ((ER_OK != err) && (NULL != p_param->p_context))
    {
        (void)d_AES_FreeKeyRestriction(((aes_context_t*)p_param->p_context)->key_length,
                                       ((aes_context_t*)p_param->p_context)->key_length_id);
    }
    else
    {
        /* do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0024]:[ICUMFW_UD_S4_D_AES_0024]
* Function name: D_AES_FinishJobXts
* Description  : Finish execution of crypto AES XTS.
* Arguments    : number             : Number of blocks to be encrypted or decrypted
*                                     Range: 0x1 - 0xFFFFFFFF
*                last_block_in_bits : last block in bits
*                                     Range: 1 - D_AES_BLOCK_INFO_SIZE_MAX
*                p_aes_src          : address of input value
*                p_context          : address of intermediate value
*                p_aes_dest         : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
uint32_t D_AES_FinishJobXts(aes_finish_t* p_param)
{
    uint32_t err;
    aes_context_t *p_context;
    aes_setparam_t setparam;
    aes_nblocks_t nblocks;

    err = ER_OK;
    p_context = NULL;
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_setparam_t));
    L_Toolbox_MemSet((uint8_t *)&nblocks, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_nblocks_t));

    /* Check if the context address is null. */
    if (NULL == p_param->p_context)
    {
        err = ER_PARAM;
    }
    /* Check the range of number. */
    else if (D_AES_NUMBER_MIN > p_param->number)
    {
        err = ER_PARAM;
    }
    /* Check range last_block_in_bits */
    else if (D_AES_BLOCK_INFO_SIZE_MAX
             < (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->last_block_in_bits))
    {
        err = ER_PARAM;
    }
    /* Alignment check of input address and output address. */
    else if ((ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_src))
          || (ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_dest)))
    {
        err = ER_D_AES_ALIGN_ERROR;
    }
    else
    {
        /* Set the intermediate value area. */
        p_context = (aes_context_t*)p_param->p_context;
        /* Create parameter group for batch setting. */
        setparam.cipher_mode = (uint8_t)D_AES_CIPHER_MODE_XTS;
        setparam.direction = p_context->direction;
        setparam.first = D_AES_NOT_FIRST_CIPHER;
        setparam.p_aes_key = p_context->aes_key;
        setparam.p_aes_iv = p_context->aes_iv;
        setparam.key_type = (uint8_t)D_AES_KEYTYPE_PLAIN;
        setparam.key_length = p_context->key_length;
        /* Set a parameter group for batch setting. */
        (void)d_AES_SetParam(&setparam);
        /* Create a parameter group for cryptographic processing. */
        nblocks.number = p_param->number;
        nblocks.p_aes_src = p_param->p_aes_src;
        nblocks.block_info = (uint16_t)(D_AES_BLOCK_INFO_LAST
                                      | (D_AES_BLOCK_INFO_SIZE
                                       & (uint32_t)p_param->last_block_in_bits));
        nblocks.p_aes_dest = p_param->p_aes_dest;
        /* Execute cryptographic processing. */
        err = d_AES_ProcessNBlocks_XTS(&nblocks);
    } /* end of else */
    if (NULL != p_param->p_context)
    {
        if (ER_OK == err)
        {
            err = d_AES_FreeKeyRestriction(((aes_context_t*)p_param->p_context)->key_length,
                                           ((aes_context_t*)p_param->p_context)->key_length_id);
        }
        else
        {
            (void)d_AES_FreeKeyRestriction(((aes_context_t*)p_param->p_context)->key_length,
                                           ((aes_context_t*)p_param->p_context)->key_length_id);
        }
    }
    else
    {
        /* do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0025]:[ICUMFW_UD_S4_D_AES_0025]
* Function name: D_AES_ExecutionAtOnceXts
* Description  : Execute encryption of crypto AES at once. (for XTS)
* Arguments    : direction          : cipher direction
*                                     Range: D_AES_DIR_ENCRYPTION
*                                            D_AES_DIR_DECRYPTION
*                number             : Number of blocks to be encrypted
*                                     Range: 0x0 - 0xFFFFFFFF
*                last_block_in_bits : last block in bits
*                                     Range: 1 - D_AES_BLOCK_INFO_SIZE_MAX
*                p_aes_key          : address of key value
*                p_aes_key_tweak    : address of the tweak key value
*                key_length         : key length
*                                     Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                            D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv           : address of IV value
*                p_aes_src          : address of input value
*                async              : sync/async transfer mode
*                                     Range: D_AES_TRANS_SYNC
*                                            D_AES_TRANS_ASYNC
*                p_aes_dest         : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_PENDING
*                ER_D_AES_SEQUENCE_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
uint32_t D_AES_ExecutionAtOnceXts(aes_all_at_once_xts_t* p_param)
{
    uint32_t err;
    uint32_t skip_flg;
    uint32_t check_size;
    uint8_t mem_check_src;
    uint8_t mem_check_dst;
    uint8_t mem_check_iv;

    err = ER_OK;
    skip_flg = FLG_OFF;

    /* Check range last_block_in_bits */
    if (((uint32_t)D_AES_XTS_LAST_BLOCK_IN_BIT_MIN <= (uint32_t)p_param->last_block_in_bits) &&
        ((uint32_t)D_AES_XTS_LAST_BLOCK_IN_BIT_MAX >= (uint32_t)p_param->last_block_in_bits))
    {
        /* do nothing */
    }
    else
    {
        err = ER_PARAM;
        skip_flg = FLG_ON;
    }

    if(FLG_OFF == skip_flg)
    {
        /* Alignment check of input address and output address. */
        if ((ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_aes_iv))
         && (ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_aes_src))
         && (ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_aes_dest)))
        {
            /* do nothing */
        }
        else
        {
            err = ER_D_AES_ALIGN_ERROR;
            skip_flg = FLG_ON;
        }
    } /* end of if(FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    if(FLG_OFF == skip_flg)
    {
        /* Check key size */
        if ((D_AES_KEY_SIZE_IN_BYTES_128BIT == (uint32_t)p_param->key_length) ||
             (D_AES_KEY_SIZE_IN_BYTES_256BIT == (uint32_t)p_param->key_length))
        {
            /* do nothing */
        }
        else
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
    } /* end of if(FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }
    if(FLG_OFF == skip_flg)
    {
        /* Check sync/async transfer mode */
        if ((D_AES_TRANS_SYNC  == (uint32_t)p_param->async) ||
             (D_AES_TRANS_ASYNC == (uint32_t)p_param->async))
        {
            /* do nothing */
        }
        else
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }
    if(FLG_OFF == skip_flg)
    {
        err = d_AES_GetKeyRestriction(p_param->key_length, &g_aes_atonce_id_use_mng);
        if (ER_OK == err)
        {
            if (D_AES_SET128 == (uint32_t)p_param->last_block_in_bits)
            {
                check_size = p_param->number * D_AES_SET_16BYTE;
            }
            else
            {
                check_size = ((p_param->number - D_AES_SET1_U32) * D_AES_SET_16BYTE)
                           + (((uint32_t)p_param->last_block_in_bits
                           + (D_AES_BYTE_SIZE_IN_BITS - D_AES_SET1_U32)) / D_AES_BYTE_SIZE_IN_BITS);
            }

            /* Check that memory address is outside the range of ICUM Local RAM */
            err = d_AES_CheckMemory((uint32_t)p_param->p_aes_src, check_size, &mem_check_src);
            if (ER_OK == err)
            {
                /* Check that memory address is outside the range of ICUM Local RAM */
                err = d_AES_CheckMemory((uint32_t)p_param->p_aes_dest, check_size, &mem_check_dst);
                if (ER_OK == err)
                {
                    /* Check that memory address is outside the range of ICUM Local RAM */
                    err = d_AES_CheckMemory((uint32_t)p_param->p_aes_iv, check_size, &mem_check_iv);
                    if (ER_OK == err)
                    {
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
                        if ((ALIGN_OK == (ALIGN256 & (uint32_t)p_param->p_aes_src))
                         && (ALIGN_OK == (ALIGN256 & (uint32_t)p_param->p_aes_dest))
#else
                        if ((ALIGN_OK == (ALIGN8 & (uint32_t)p_param->p_aes_dest))
#endif
                         && ((uint32_t)D_AES_CHK_OK == (uint32_t)mem_check_src)
                         && ((uint32_t)D_AES_CHK_OK == (uint32_t)mem_check_dst)
                         && ((uint32_t)D_AES_CHK_OK == (uint32_t)mem_check_iv))
                        {
                            err = d_AES_ExecutionAtOnceXtsDtf(p_param);
                        }
                        else
                        {
                            err = d_AES_ExecutionAtOnceXtsIcup(p_param);
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
            }
            else
            {
                /* do nothing */
            }
            if (ER_IN_PROCESS == err)
            {
                /* do nothing */
            }
            else if (ER_OK == err)
            {
                err = d_AES_FreeKeyRestriction(p_param->key_length, g_aes_atonce_id_use_mng);
            }
            else
            {
                (void)d_AES_FreeKeyRestriction(p_param->key_length, g_aes_atonce_id_use_mng);
            }
        } /* end of if(FLG_OFF == skip_flg) */
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0026]:[ICUMFW_UD_S4_D_AES_0026]
* Function name: d_AES_ExecutionAtOnceXtsIcup
* Description  : Execute encryption of crypto AES at once. (for XTS - ICUP)
* Arguments    : direction          : cipher direction
*                                     Range: D_AES_DIR_ENCRYPTION
*                                            D_AES_DIR_DECRYPTION
*                number             : Number of blocks to be encrypted
*                                     Range: 0x0 - 0xFFFFFFFF
*                last_block_in_bits : last block in bits
*                                     Range: 1 - D_AES_BLOCK_INFO_SIZE_MAX
*                p_aes_key          : address of key value
*                p_aes_key_tweak    : address of the tweak key value
*                key_length         : key length
*                                     Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                            D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv           : address of IV value
*                p_aes_src          : address of input value
*                async              : sync/async transfer mode
*                                     Range: D_AES_TRANS_SYNC
*                                            D_AES_TRANS_ASYNC
*                p_aes_dest         : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_PENDING
***************************************************************************************/
static uint32_t d_AES_ExecutionAtOnceXtsIcup(aes_all_at_once_xts_t* p_param)
{
    uint32_t err;
    aes_init_xts_t param_init;
    aes_update_t param_update;
    aes_finish_t param_finish;
    uint32_t number_finish;
    uint32_t* p_aes_src_finish;
    uint32_t* p_aes_dest_finish;
    uint32_t lastBlock[AES_BLOCK_SIZE];
    uint32_t cipher_data[AES_BLOCK_SIZE * NUM2];

    err = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&param_init, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_init_xts_t));
    L_Toolbox_MemSet((uint8_t *)&param_update, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_update_t));
    L_Toolbox_MemSet((uint8_t *)&param_finish, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_finish_t));
    number_finish = D_AES_ZERO_U32;
    p_aes_src_finish = NULL;
    p_aes_dest_finish = NULL;
    L_Toolbox_MemSet((uint8_t *)&lastBlock, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(lastBlock));
    L_Toolbox_MemSet((uint8_t *)&cipher_data, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(cipher_data));

    param_init.direction = p_param->direction;
    param_init.p_aes_key = p_param->p_aes_key;
    param_init.p_aes_key_tweak = p_param->p_aes_key_tweak;
    param_init.key_length = p_param->key_length;
    param_init.p_aes_iv = p_param->p_aes_iv;
    param_init.p_context = p_param->p_context;
    err = D_AES_InitJobXts(&param_init);

    if (ER_OK == err)
    {
        if (D_AES_XTS_LAST_BLOCK_IN_BIT_MAX != (uint32_t)p_param->last_block_in_bits)
        {
            /* If there is a fraction, adjust the output size of the final block */
            if((uint32_t)D_AES_BLOCK_SIZE_2 < p_param->number)
            {
                /* Calculate with the Update function until the remaining 2 blocks */
                param_update.number = p_param->number - (uint32_t)D_AES_BLOCK_SIZE_2;
                param_update.first = D_AES_NOT_FIRST_CIPHER;
                param_update.p_aes_src = p_param->p_aes_src;
                param_update.p_context = p_param->p_context;
                param_update.p_aes_dest = p_param->p_aes_dest;
                err = D_AES_UpdateJobXts(&param_update);

                number_finish     = p_param->number - param_update.number;
                p_aes_src_finish  = p_param->p_aes_src  + ((uint32_t)D_AES_BLOCK_SIZE_4 * param_update.number);
                p_aes_dest_finish = p_param->p_aes_dest + ((uint32_t)D_AES_BLOCK_SIZE_4 * param_update.number);
            }
            else
            {
                number_finish     = p_param->number;
                p_aes_src_finish  = p_param->p_aes_src;
                p_aes_dest_finish = p_param->p_aes_dest;
            }

            if(ER_OK == err)
            {
                param_finish.number = number_finish;
                param_finish.last_block_in_bits = p_param->last_block_in_bits;
                param_finish.first = D_AES_NOT_FIRST_CIPHER;
                param_finish.p_aes_src = p_aes_src_finish;
                param_finish.p_context = p_param->p_context;
                param_finish.p_aes_dest = (uint32_t*)cipher_data;
                err = D_AES_FinishJobXts(&param_finish);
                if (ER_OK == err)
                {
                    /* Shape the last block */
                    L_Toolbox_MemCpy((uint8_t *)lastBlock, (const uint8_t *)&cipher_data[NUM4], AES_BLOCK_SIZE_IN_BYTES);
                    (void)d_AES_LastBlock(&cipher_data[NUM4], lastBlock, p_param->last_block_in_bits);
                    L_Toolbox_MemCpy((uint8_t *)p_aes_dest_finish,
                                     (const uint8_t *)cipher_data,
                                     (int16_t)(AES_BLOCK_SIZE_IN_BYTES +
                                        (((uint32_t)p_param->last_block_in_bits + (D_AES_BYTE_SIZE_IN_BITS - 1u)) /
                                            D_AES_BYTE_SIZE_IN_BITS)));
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
            param_finish.number = p_param->number;
            param_finish.last_block_in_bits = p_param->last_block_in_bits;
            param_finish.first = D_AES_NOT_FIRST_CIPHER;
            param_finish.p_aes_src = p_param->p_aes_src;
            param_finish.p_context = p_param->p_context;
            param_finish.p_aes_dest = p_param->p_aes_dest;
            err = D_AES_FinishJobXts(&param_finish);
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0033]:[ICUMFW_UD_S4_D_AES_0033]
* Function name: D_AES_InitJobGcm
* Description  : Set parameters of crypto AES GCM.
* Arguments    : number            : Number of blocks to be encrypted or decrypted
*                                    Range: 0x1 - 0xFFFFFFFF
*                block_info        : block information
*                                    Range: D_AES_BLOCK_INFO_FIRST
*                                           D_AES_BLOCK_INFO_LAST
*                                           D_AES_BLOCK_INFO_GCM_IV96
*                                           D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                direction         : cipher direction
*                                    Range: D_AES_DIR_ENCRYPTION
*                                           D_AES_DIR_DECRYPTION
*                p_aes_key         : address of key value
*                key_length        : key length
*                                  Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                         D_AES_KEY_SIZE_IN_BYTES_192BIT
*                                         D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv          : address of IV value
*                p_iv_size_in_bits : address of IV length
*                p_context         : address of intermediate value
*                p_ctr_j0          : address of counter block J0
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_PENDING
***************************************************************************************/
uint32_t D_AES_InitJobGcm(aes_init_gcm_t* p_param)
{
    uint32_t err;
    gcm_context_t *p_context;
    aes_gcm_setparam_t setparam;
    aes_gcm_iv_t iv_param;

    err = ER_OK;
    p_context = NULL;
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_gcm_setparam_t));
    L_Toolbox_MemSet((uint8_t *)&iv_param, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_gcm_iv_t));

    /* Check if the context address is null. */
    if (NULL == p_param->p_context)
    {
        err = ER_PARAM;
    }
    /* Check the range of number. */
    else if (D_AES_NUMBER_MIN > p_param->number)
    {
        err = ER_PARAM;
    }
    /* Check range block_info */
    else if (D_AES_BLOCK_INFO_SIZE_MAX < (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->block_info))
    {
        err = ER_PARAM;
    }
    /* Alignment check of IV address. */
    else if (ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_iv))
    {
        err = ER_D_AES_ALIGN_ERROR;
    }
    /* Check key size */
    else if ((D_AES_KEY_SIZE_IN_BYTES_128BIT != (uint32_t)p_param->key_length) &&
             (D_AES_KEY_SIZE_IN_BYTES_192BIT != (uint32_t)p_param->key_length) &&
             (D_AES_KEY_SIZE_IN_BYTES_256BIT != (uint32_t)p_param->key_length))
    {
        err = ER_PARAM;
    }
    else
    {
        err = d_AES_GetKeyRestriction(p_param->key_length, &((gcm_context_t*)p_param->p_context)->key_length_id);
        if (ER_OK == err)
        {
            /* Set the intermediate value area. */
            p_context = (gcm_context_t*)p_param->p_context;
            p_context->direction = p_param->direction;
            p_context->key_length = p_param->key_length;
            /* Store the key value in the context. */
            if (NULL != p_param->p_aes_key)
            {
                L_Toolbox_MemCpy((uint8_t *)p_context->aes_key,
                                 (const uint8_t *)p_param->p_aes_key,
                                 (int16_t)p_param->key_length);
            }
            else
            {
                /* do nothing */
            }
            *p_context->ctr_j = (uint32_t)p_param->p_aes_iv;
            /* Create parameter group for batch setting. */
            setparam.direction = p_context->direction;
            setparam.first = D_AES_FIRST_CIPHER;
            setparam.p_gcm_key = p_context->aes_key;
            setparam.p_gcm_ghash = p_context->ghash;
            setparam.key_length = p_context->key_length;
            /* Set a parameter group for batch setting. */
            (void)d_AES_SetParam_GCM(&setparam);
            /* Create a parameter group for counter J0 value calculation. */
            iv_param.iv_number = p_param->number;
            iv_param.iv_block_info = p_param->block_info;
            iv_param.p_iv_size_in_bits = p_param->p_iv_size_in_bits;
            iv_param.p_gcm_iv = (uint32_t*)*p_context->ctr_j;
            iv_param.p_ctr_j0 = p_param->p_ctr_j0;
            /* Calculate counter J0 value. */
            err = d_AES_ProcessNBlocks_IV(&iv_param);
            if (ER_OK == err)
            {
                if (D_AES_BLOCK_INFO_LAST ==
                    (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->block_info))
                {
                    /* Save counter J1 value in context. */
                    (void)d_AES_GetIV(p_context->ctr_j);
                }
                else
                {
                    /* Save GHASH value in context. */
                    (void)d_AES_GetGHash(p_context->ghash);
                }
            }
            else
            {
                (void)d_AES_FreeKeyRestriction(p_context->key_length,
                                              ((gcm_context_t*)p_param->p_context)->key_length_id);
            }
        } /* end of if (ER_OK == err) */
        else
        {
            /* Do nothing */
        }
    } /* end of else */

    return err;
}

/***************************************************************************************
* Function ID  :[ICUMFW_CD_S4_D_AES_0034]:[ICUMFW_UD_S4_D_AES_0034]
* Function name: D_AES_UpdateJobGcm
* Description  : Execute encryption of crypto AES GCM.
* Arguments    : number            : Number of blocks to be encrypted or decrypted
*                                    Range: 0x1 - 0xFFFFFFFF
*                block_info        : block information
*                                    Range: D_AES_BLOCK_INFO_FIRST
*                                           D_AES_BLOCK_INFO_LAST
*                                           D_AES_BLOCK_INFO_GCM_IV96
*                                           D_AES_BLOCK_INFO_GCM_ADD0
*                                           D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                step              : Operating procedure
*                                    Range: D_AES_GCM_STEP_IV
*                                           D_AES_GCM_STEP_AAD
*                                           D_AES_GCM_STEP_CIPHER
*                p_iv_size_in_bits : address of IV length
*                p_aes_src         : address of input value
*                p_context         : address of intermediate value
*                p_ctr_j0          : address of counter block J0
*                p_aes_dest        : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
uint32_t D_AES_UpdateJobGcm(aes_update_gcm_t* p_param)
{
    uint32_t err;
    gcm_context_t *p_context;
    aes_gcm_setparam_t setparam;
    aes_gcm_iv_t iv_param;
    aes_gcm_aad_t aad_param;
    aes_gcm_t gcm_param;

    err = ER_OK;
    p_context = NULL;
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_gcm_setparam_t));
    L_Toolbox_MemSet((uint8_t *)&iv_param, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_gcm_iv_t));
    L_Toolbox_MemSet((uint8_t *)&aad_param, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_gcm_aad_t));
    L_Toolbox_MemSet((uint8_t *)&gcm_param, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_gcm_t));

    /* Check if the context address is null. */
    if (NULL == p_param->p_context)
    {
        err = ER_PARAM;
    }
    /* Check the range of number. */
    else if (D_AES_NUMBER_MIN > p_param->number)
    {
        err = ER_PARAM;
    }
    /* Check range block_info */
    else if (D_AES_BLOCK_INFO_SIZE_MAX < (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->block_info))
    {
        err = ER_PARAM;
    }
    else
    {
        /* Set the intermediate value area. */
        p_context = (gcm_context_t*)p_param->p_context;
        /* Create parameter group for batch setting. */
        setparam.direction = p_context->direction;
        setparam.first = (uint8_t)((D_AES_BLOCK_INFO_FIRST & (uint32_t)p_param->block_info)
                                   >> SHIFT_AES_FIRST_CIPHER);
        setparam.p_gcm_key = p_context->aes_key;
        setparam.p_gcm_ghash = p_context->ghash;
        setparam.key_length = p_context->key_length;
        /* Set a parameter group for batch setting. */
        (void)d_AES_SetParam_GCM(&setparam);
        /* Check step */
        switch ((uint32_t)p_param->step)
        {
            case D_AES_GCM_STEP_IV:
                /* Create a parameter group for counter J0 value calculation. */
                iv_param.iv_number = p_param->number;
                iv_param.iv_block_info = p_param->block_info;
                iv_param.p_iv_size_in_bits = p_param->p_iv_size_in_bits;
                iv_param.p_gcm_iv = (uint32_t*)*p_context->ctr_j;
                iv_param.p_ctr_j0 = p_param->p_ctr_j0;
                /* Calculate counter J0 value. */
                err = d_AES_ProcessNBlocks_IV(&iv_param);
                if (ER_OK == err)
                {
                    if (D_AES_BLOCK_INFO_LAST
                        == (D_AES_BLOCK_INFO_LAST & (uint32_t)p_param->block_info))
                    {
                        /* Save counter J1 value in context. */
                        (void)d_AES_GetIV(p_context->ctr_j);
                    }
                    else
                    {
                        /* Save GHASH value in context. */
                        (void)d_AES_GetGHash(p_context->ghash);
                    }
                }
                else
                {
                    /* Do nothing */
                }
                break;
            case D_AES_GCM_STEP_AAD:
                /* Alignment check of input address. */
                if (ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_src))
                {
                    err = ER_D_AES_ALIGN_ERROR;
                }
                else
                {
                    /* Create a parameter group for Additional Authenticated Data calculation. */
                    aad_param.aad_number = p_param->number;
                    aad_param.aad_block_info = p_param->block_info;
                    aad_param.p_gcm_aad = p_param->p_aes_src;
                    /* Calculate Additional Authenticated Data. */
                    err = d_AES_ProcessNBlocks_AAD(&aad_param);
                    if (ER_OK == err)
                    {
                        /* Save GHASH value in context. */
                        (void)d_AES_GetGHash(p_context->ghash);
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                break;
            case D_AES_GCM_STEP_CIPHER:
                /* Alignment check of input address and output address. */
                if ((ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_src))
                 || (ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_dest)))
                {
                    err = ER_D_AES_ALIGN_ERROR;
                }
                else
                {
                    /* Create a parameter group for cryptographic processing. */
                    gcm_param.gcm_number = p_param->number;
                    gcm_param.gcm_block_info = p_param->block_info;
                    gcm_param.p_gcm_src = p_param->p_aes_src;
                    gcm_param.p_gcm_dest = p_param->p_aes_dest;
                    gcm_param.p_ctr_j = p_context->ctr_j;
                    /* Execute cryptographic processing. */
                    err = d_AES_ProcessNBlocks_GCM(&gcm_param);
                    if (ER_OK == err)
                    {
                        /* Save counter Jt value in context. */
                        (void)d_AES_GetIV(p_context->ctr_j);
                        /* Save GHASH value in context. */
                        (void)d_AES_GetGHash(p_context->ghash);
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                break;
            default:
                err = ER_PARAM;
                break;
        }
    } /* end of else */

    if ((ER_OK != err) && (NULL != p_param->p_context))
    {
        (void)d_AES_FreeKeyRestriction(((gcm_context_t*)p_param->p_context)->key_length,
                                       ((gcm_context_t*)p_param->p_context)->key_length_id);
    }
    else
    {
        /* do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0035]:[ICUMFW_UD_S4_D_AES_0035]
* Function name: D_AES_FinishJobGcm
* Description  : Finish execution of crypto AES GCM.
* Arguments    : number               : Number of blocks to be encrypted or decrypted
*                                       Range: 0x0 - 0xFFFFFFFF
*                block_info           : block information
*                                       Range: D_AES_BLOCK_INFO_FIRST
*                                              D_AES_BLOCK_INFO_LAST
*                                              D_AES_BLOCK_INFO_GCM_IV96
*                                              D_AES_BLOCK_INFO_GCM_ADD0
*                                              D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_aes_src            : address of input value
*                p_total_size_in_bits : address of total size value
*                p_context            : address of intermediate value
*                p_ctr_j0             : address of counter block J0
*                p_aes_dest           : address of output value
*                p_auth_tag           : address of authentication tag value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
uint32_t D_AES_FinishJobGcm(aes_finish_gcm_t* p_param)
{
    uint32_t err;
    gcm_context_t *p_context;
    aes_gcm_setparam_t setparam;
    aes_gcm_t gcm_param;
    aes_gcm_tag_t tag_param;

    err = ER_OK;
    p_context = NULL;
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_setparam_t));
    L_Toolbox_MemSet((uint8_t *)&gcm_param, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_gcm_t));
    L_Toolbox_MemSet((uint8_t *)&tag_param, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_gcm_tag_t));

    /* Check if the context address is null. */
    if (NULL == p_param->p_context)
    {
        err = ER_PARAM;
    }
    /* Check range block_info */
    else if (D_AES_BLOCK_INFO_SIZE_MAX < (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->block_info))
    {
        err = ER_PARAM;
    }
    else
    {
        /* Set the intermediate value area. */
        p_context = (gcm_context_t*)p_param->p_context;
        /* Create parameter group for batch setting. */
        setparam.direction = p_context->direction;
        setparam.first = (uint8_t)((D_AES_BLOCK_INFO_FIRST & (uint32_t)p_param->block_info)
                                   >> SHIFT_AES_FIRST_CIPHER);
        setparam.p_gcm_key = p_context->aes_key;
        setparam.p_gcm_ghash = p_context->ghash;
        setparam.key_length = p_context->key_length;
        /* Set a parameter group for batch setting. */
        (void)d_AES_SetParam_GCM(&setparam);
        /* Check the range of number. */
        if (D_AES_NUMBER_MIN <= p_param->number)
        {
            /* Create a parameter group for cryptographic processing. */
            gcm_param.gcm_number = p_param->number;
            gcm_param.gcm_block_info = p_param->block_info;
            gcm_param.p_gcm_src = p_param->p_aes_src;
            gcm_param.p_gcm_dest = p_param->p_aes_dest;
            gcm_param.p_ctr_j = p_context->ctr_j;
            /* Execute cryptographic processing. */
            err = d_AES_ProcessNBlocks_GCM(&gcm_param);
        }
        else
        {
            /* Do nothing */
        }

        if (ER_OK == err)
        {
            /* Create a parameter group for authentication tag calculation. */
            tag_param.p_ctr_j0 = p_param->p_ctr_j0;
            tag_param.p_total_size_in_bits = p_param->p_total_size_in_bits;
            tag_param.p_auth_tag = p_param->p_auth_tag;
            /* Get an authentication tag. */
            (void)d_AES_GetAuthTag(&tag_param);
        }
        else
        {
            /* Do nothing */
        }
    } /* end of else */
    if (NULL != p_param->p_context)
    {
        if (ER_OK == err)
        {
            err = d_AES_FreeKeyRestriction(((gcm_context_t*)p_param->p_context)->key_length,
                                           ((gcm_context_t*)p_param->p_context)->key_length_id);
        }
        else
        {
            (void)d_AES_FreeKeyRestriction(((gcm_context_t*)p_param->p_context)->key_length,
                                           ((gcm_context_t*)p_param->p_context)->key_length_id);
        }
    }
    else
    {
        /* do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0036]:[ICUMFW_UD_S4_D_AES_0036]
* Function name: D_AES_ExecutionAtOnceGcm
* Description  : Execute encryption of crypto AES at once. (for GCM)
* Arguments    : direction              : cipher direction
*                                         Range: D_AES_DIR_ENCRYPTION
*                                                D_AES_DIR_DECRYPTION
*                p_aes_key              : address of key value
*                key_length             : key length
*                                         Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                                D_AES_KEY_SIZE_IN_BYTES_192BIT
*                                                D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv               : address of IV value
*                iv_size_in_bits        : IV size in bits
*                p_auth_data            : address of AAD data
*                auth_data_size_in_bits : AAD data size in bits
*                p_aes_src              : address of input value
*                data_size_in_bits      : input value size in bits
*                auth_tag_size_in_bits  : authentication tag size in bits
*                async                  : sync/async transfer mode
*                                         Range: D_AES_TRANS_SYNC
*                                                D_AES_TRANS_ASYNC
*                p_aes_dest             : address of output value
*                p_auth_tag             : address of authentication tag
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_PENDING
*                ER_D_AES_SEQUENCE_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
uint32_t D_AES_ExecutionAtOnceGcm(aes_all_at_once_gcm_t* p_param)
{
    uint32_t err;
    uint32_t skip_flg;
    uint32_t check_size;
    uint8_t mem_check_src;
    uint8_t mem_check_dst;
    uint8_t mem_check_auth_data;

    err = ER_OK;
    skip_flg = FLG_OFF;

    /* Check range last_block_in_bits */
    if ((D_AES_GCM_IV_SIZE_IN_BITS_MIN <= (uint32_t)p_param->iv_size_in_bits) &&
        (D_AES_GCM_IV_SIZE_IN_BITS_MAX >= (uint32_t)p_param->iv_size_in_bits))
    {
        /* do nothing */
    }
    else
    {
        err = ER_PARAM;
        skip_flg = FLG_ON;
    }
    if(FLG_OFF == skip_flg)
    {
        /* Check range authentication tag size in bits */
        if ((D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_32  == p_param->auth_tag_size_in_bits) ||
            (D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_64  == p_param->auth_tag_size_in_bits) ||
            (D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_96  == p_param->auth_tag_size_in_bits) ||
            (D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_104 == p_param->auth_tag_size_in_bits) ||
            (D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_112 == p_param->auth_tag_size_in_bits) ||
            (D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_120 == p_param->auth_tag_size_in_bits) ||
            (D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_128 == p_param->auth_tag_size_in_bits))
        {
            /* do nothing */
        }
        else
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
    }
    else
    {
        /* do nothing */
    }
    if(FLG_OFF == skip_flg)
    {
        /* Alignment check of input address and output address. */
        if ((ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_aes_iv))
         && (ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_auth_data))
         && (ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_aes_src))
         && (ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_aes_dest))
         && (ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_auth_tag)))
        {
            /* do nothing */
        }
        else
        {
        err = ER_D_AES_ALIGN_ERROR;
            skip_flg = FLG_ON;
        }
    } /* end of if(FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }
    if(FLG_OFF == skip_flg)
    {
        /* Check key size */
        if ((D_AES_KEY_SIZE_IN_BYTES_128BIT == (uint32_t)p_param->key_length) ||
            (D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)p_param->key_length) ||
            (D_AES_KEY_SIZE_IN_BYTES_256BIT == (uint32_t)p_param->key_length))
        {
            /* do nothing */
        }
        else
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
    } /* end of if(FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }
    if(FLG_OFF == skip_flg)
    {
        /* Check sync/async transfer mode */
        if ((D_AES_TRANS_SYNC  == (uint32_t)p_param->async) ||
            (D_AES_TRANS_ASYNC == (uint32_t)p_param->async))
        {
            /* do nothing */
        }
        else
        {
            err = ER_PARAM;
            skip_flg = FLG_ON;
        }
    } /* end of if(FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }
    if(FLG_OFF == skip_flg)
    {
        err = d_AES_GetKeyRestriction(p_param->key_length, &g_aes_atonce_id_use_mng);
        if (ER_OK == err)
        {
            check_size = (p_param->data_size_in_bits + ((uint32_t)D_AES_BYTE_SIZE_IN_BITS - D_AES_SET1_U32)) / (uint32_t)D_AES_BYTE_SIZE_IN_BITS;

            /* Check that memory address is outside the range of ICUM Local RAM */
            err = d_AES_CheckMemory((uint32_t)p_param->p_aes_src, check_size, &mem_check_src);
            if (ER_OK == err)
            {
                /* Check that memory address is outside the range of ICUM Local RAM */
                err = d_AES_CheckMemory((uint32_t)p_param->p_aes_dest, check_size, &mem_check_dst);
                if (ER_OK == err)
                {
                    /* Check that memory address is outside the range of ICUM Local RAM */
                    err = d_AES_CheckMemory((uint32_t)p_param->p_auth_data,
                                            (p_param->auth_data_size_in_bits + ((uint32_t)D_AES_BYTE_SIZE_IN_BITS - D_AES_SET1_U32)) / (uint32_t)D_AES_BYTE_SIZE_IN_BITS,
                                            &mem_check_auth_data);
                    if (ER_OK == err)
                    {
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
                        if ((ALIGN_OK == (ALIGN256 & (uint32_t)p_param->p_aes_src))
                         && (ALIGN_OK == (ALIGN256 & (uint32_t)p_param->p_aes_dest))
                         && (ALIGN_OK == (ALIGN256 & (uint32_t)p_param->p_auth_data))
#else
                        if ((ALIGN_OK == (ALIGN8 & (uint32_t)p_param->p_aes_dest))
#endif
                         && ((uint32_t)D_AES_CHK_OK == (uint32_t)mem_check_src)
                         && ((uint32_t)D_AES_CHK_OK == (uint32_t)mem_check_dst)
                         && ((uint32_t)D_AES_CHK_OK == (uint32_t)mem_check_auth_data)
                         && ((uint32_t)D_AES_ZERO_U32 == (uint32_t)(p_param->data_size_in_bits % D_AES_SET128))
                         && ((uint32_t)D_AES_ZERO_U32 == (uint32_t)(p_param->auth_data_size_in_bits % D_AES_SET128)))
                        {
                            err = d_AES_ExecutionAtOnceGcmDtf(p_param);
                        }
                        else
                        {
                            err = d_AES_ExecutionAtOnceGcmIcup(p_param);
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
            }
            else
            {
                /* do nothing */
            }
            if (ER_IN_PROCESS == err)
            {
                /* do nothing */
            }
            else if (ER_OK == err)
            {
                err = d_AES_FreeKeyRestriction(p_param->key_length, g_aes_atonce_id_use_mng);
            }
            else
            {
                (void)d_AES_FreeKeyRestriction(p_param->key_length, g_aes_atonce_id_use_mng);
            }
        }
        else
        {
            /* do nothing */
        }
    } /* end of if(FLG_OFF == skip_flg) */
    else
    {
        /* do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0037]:[ICUMFW_UD_S4_D_AES_0037]
* Function name: d_AES_ExecutionAtOnceGcmIcup
* Description  : Execute encryption of crypto AES at once. (for GCM - ICUP)
* Arguments    : direction              : cipher direction
*                                         Range: D_AES_DIR_ENCRYPTION
*                                                D_AES_DIR_DECRYPTION
*                p_aes_key              : address of key value
*                key_length             : key length
*                                         Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                                D_AES_KEY_SIZE_IN_BYTES_192BIT
*                                                D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv               : address of IV value
*                iv_size_in_bits        : IV size in bits
*                p_auth_data            : address of AAD data
*                auth_data_size_in_bits : AAD data size in bits
*                p_aes_src              : address of input value
*                data_size_in_bit       : input value size in bits
*                auth_tag_size_in_bit   : authentication tag size in bits
*                p_aes_dest             : address of output value
*                p_auth_tag             : address of authentication tag
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_PENDING
***************************************************************************************/
static uint32_t d_AES_ExecutionAtOnceGcmIcup(aes_all_at_once_gcm_t* p_param)
{
    uint32_t err;
    aes_init_gcm_t param_init;
    aes_update_gcm_t param_update;
    aes_finish_gcm_t param_finish;
    uint32_t nb_blocks;
    uint8_t rem;
    uint32_t nb_blocks_auth;
    uint8_t rem_auth;
    uint32_t nb_blocks_iv;
    uint8_t rem_iv;
    uint16_t block_info;
    uint32_t j0[NUM4] = {D_AES_ZERO_U32};
    uint32_t number_finish;
    uint32_t* p_aes_src_finish;
    uint32_t* p_aes_dest_finish;
    uint32_t total_size[NUM4];
    uint32_t cipher_data[AES_BLOCK_SIZE] = {D_AES_ZERO_U32};

    err = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&param_init, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_init_gcm_t));
    L_Toolbox_MemSet((uint8_t *)&param_update, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_update_gcm_t));
    L_Toolbox_MemSet((uint8_t *)&param_finish, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_finish_gcm_t));
    nb_blocks = D_AES_ZERO_U32;
    rem = D_AES_ZERO_U8;
    nb_blocks_auth = D_AES_ZERO_U32;
    rem_auth = D_AES_ZERO_U8;
    nb_blocks_iv = D_AES_ZERO_U32;
    rem_iv = D_AES_ZERO_U8;
    block_info = D_AES_ZERO_U16;
    number_finish = D_AES_ZERO_U32;
    p_aes_src_finish = NULL;
    p_aes_dest_finish = NULL;

    L_Toolbox_MemSet((uint8_t*)total_size, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(total_size));

    if (D_AES_ZERO_U32 == p_param->data_size_in_bits)
    {
        nb_blocks = D_AES_ZERO_U32;
        rem = AES_BLOCK_SIZE_IN_BITS;
    }
    else
    {
        nb_blocks = ((p_param->data_size_in_bits + (AES_BLOCK_SIZE_IN_BITS - D_AES_SET1_U32)) /
            AES_BLOCK_SIZE_IN_BITS);
        rem = (uint8_t)(p_param->data_size_in_bits % AES_BLOCK_SIZE_IN_BITS);
        if (D_AES_ZERO_U32 == (uint32_t)rem)
        {
            rem = AES_BLOCK_SIZE_IN_BITS;
        }
        else
        {
            /* do nothing */
        }
    } /* end of else */

    if (D_AES_ZERO_U32 == p_param->auth_data_size_in_bits)
    {
        nb_blocks_auth = D_AES_ZERO_U32;
        rem_auth = AES_BLOCK_SIZE_IN_BITS;
    }
    else
    {
        nb_blocks_auth = ((p_param->auth_data_size_in_bits + (AES_BLOCK_SIZE_IN_BITS - D_AES_SET1_U32)) /
            AES_BLOCK_SIZE_IN_BITS);
        rem_auth = (uint8_t)(p_param->auth_data_size_in_bits % AES_BLOCK_SIZE_IN_BITS);
        if (D_AES_ZERO_U32 == (uint32_t)rem_auth)
        {
            rem_auth = AES_BLOCK_SIZE_IN_BITS;
        }
        else
        {
            /* do nothing */
        }
    } /* end of else */

    if (D_AES_ZERO_U32 == p_param->iv_size_in_bits)
    {
        nb_blocks_iv = D_AES_ZERO_U32;
        rem_iv = AES_BLOCK_SIZE_IN_BITS;
    }
    else
    {
        nb_blocks_iv = ((p_param->iv_size_in_bits + (AES_BLOCK_SIZE_IN_BITS - D_AES_SET1_U32)) /
            AES_BLOCK_SIZE_IN_BITS);
        rem_iv = (uint8_t)(p_param->iv_size_in_bits % AES_BLOCK_SIZE_IN_BITS);
        if (D_AES_ZERO_U32 == (uint32_t)rem_iv)
        {
            rem_iv = AES_BLOCK_SIZE_IN_BITS;
        }
        else
        {
            /* do nothing */
        }
    } /* end of else */

    if ((uint32_t)IV_LENGTH_96 == p_param->iv_size_in_bits)
    {
        block_info = (uint16_t)(D_AES_BLOCK_INFO_FIRST | D_AES_BLOCK_INFO_LAST | D_AES_BLOCK_INFO_GCM_IV96 | (uint32_t)rem_iv);
    }
    else
    {
        block_info = (uint16_t)(D_AES_BLOCK_INFO_FIRST | D_AES_BLOCK_INFO_LAST | (uint32_t)rem_iv);
    }

    param_init.number            = nb_blocks_iv;
    param_init.block_info        = block_info;
    param_init.direction         = p_param->direction;
    param_init.p_aes_key         = p_param->p_aes_key;
    param_init.p_aes_iv          = p_param->p_aes_iv;
    param_init.p_iv_size_in_bits = &p_param->iv_size_in_bits;
    param_init.p_context         = p_param->p_context;
    param_init.p_ctr_j0          = (uint32_t*)(&j0);
    param_init.key_length        = p_param->key_length;
    err = D_AES_InitJobGcm(&param_init);
    if (ER_OK == err)
    {
        /* Clear flag information that will not be handed over next */
        block_info &= ~D_AES_BLOCK_INFO_LAST;
        block_info &= ~D_AES_BLOCK_INFO_SIZE;

        if (D_AES_ZERO_U32 == p_param->auth_data_size_in_bits)
        {
            block_info |= D_AES_BLOCK_INFO_GCM_ADD0;
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

    /* AAD operation processing */
    if ((ER_OK == err) && (D_AES_ZERO_U32 != nb_blocks_auth))
    {
        (void)d_AES_AddCounter(&total_size[0], p_param->auth_data_size_in_bits);

        param_update.number            = nb_blocks_auth;
        param_update.block_info        = (uint16_t)((uint32_t)block_info | D_AES_BLOCK_INFO_LAST | (uint32_t)rem_auth);
        param_update.step              = D_AES_GCM_STEP_AAD;
        param_update.p_iv_size_in_bits = NULL;
        param_update.p_aes_src         = p_param->p_auth_data;
        param_update.p_context         = p_param->p_context;
        param_update.p_aes_dest        = NULL;
        param_update.p_ctr_j0          = NULL;
        err = D_AES_UpdateJobGcm(&param_update);
        if (ER_OK == err)
        {
            /* Clear flag information that will not be handed over next */
            block_info &= ~D_AES_BLOCK_INFO_FIRST;
            block_info &= ~D_AES_BLOCK_INFO_LAST;
            block_info &= ~D_AES_BLOCK_INFO_SIZE;
        }
        else
        {
            /* do nothing */
        }
    } /* end of if ((ER_OK == err) && (D_AES_ZERO_U32 != nb_blocks_auth)) */
    else
    {
        /* do nothing */
    }

    /* Cipher operation processing */
    if ((ER_OK == err) && (D_AES_ZERO_U32 != nb_blocks))
    {
        (void)d_AES_AddCounter(&total_size[NUM2], p_param->data_size_in_bits);

        if (D_AES_SET128 != (uint32_t)rem)
        {
            number_finish     = nb_blocks;
            p_aes_src_finish  = p_param->p_aes_src;
            p_aes_dest_finish = p_param->p_aes_dest;

            if (D_AES_SET1_U32 < nb_blocks)
            {
                param_update.number            = nb_blocks - D_AES_SET1_U32;
                param_update.block_info        = block_info;
                param_update.step              = D_AES_GCM_STEP_CIPHER;
                param_update.p_iv_size_in_bits = NULL;
                param_update.p_aes_src         = p_param->p_aes_src;
                param_update.p_context         = p_param->p_context;
                param_update.p_aes_dest        = p_param->p_aes_dest;
                param_update.p_ctr_j0          = NULL;
                err = D_AES_UpdateJobGcm(&param_update);
                if (ER_OK == err)
                {
                    block_info &= ~D_AES_BLOCK_INFO_FIRST;

                    number_finish     = D_AES_SET1_U32;
                    p_aes_src_finish  = p_param->p_aes_src  + (D_AES_SET_4 * param_update.number);
                    p_aes_dest_finish = p_param->p_aes_dest + (D_AES_SET_4 * param_update.number);
                }
                else
                {
                    /* do nothing */
                }
            } /* end of if (D_AES_SET1_U32 < nb_blocks) */
            else
            {
                /* do nothing */
            }

            if (ER_OK == err)
            {
                /* Set finish parameter */
                param_finish.number               = number_finish;
                param_finish.block_info           = (uint16_t)((uint32_t)block_info | D_AES_BLOCK_INFO_LAST | (uint32_t)rem);
                param_finish.p_aes_src            = p_aes_src_finish;
                param_finish.p_total_size_in_bits = total_size;
                param_finish.p_ctr_j0             = (uint32_t*)(&j0);
                param_finish.p_context            = p_param->p_context;
                param_finish.p_aes_dest           = cipher_data;
                param_finish.p_auth_tag           = p_param->p_auth_tag;
                err = D_AES_FinishJobGcm(&param_finish);
                /* Output only valid sizes */
                L_Toolbox_MemCpy((uint8_t *)p_aes_dest_finish,
                                 (const uint8_t *)cipher_data,
                                 (int16_t)(((uint32_t)rem + (D_AES_BYTE_SIZE_IN_BITS - D_AES_SET1_U32)) / D_AES_BYTE_SIZE_IN_BITS));
            }
            else
            {
                /* do nothing */
            }
        } /* end of if (D_AES_SET128 != (uint32_t)rem) */
        else
        {
            /* Set finish parameter */
            param_finish.number               = nb_blocks;
            param_finish.block_info           = (uint16_t)((uint32_t)block_info | D_AES_BLOCK_INFO_LAST | (uint32_t)rem);
            param_finish.p_aes_src            = p_param->p_aes_src;
            param_finish.p_total_size_in_bits = total_size;
            param_finish.p_ctr_j0             = (uint32_t*)(&j0);
            param_finish.p_context            = p_param->p_context;
            param_finish.p_aes_dest           = p_param->p_aes_dest;
            param_finish.p_auth_tag           = p_param->p_auth_tag;
            err = D_AES_FinishJobGcm(&param_finish);
        }
    } /* end of if ((ER_OK == err) && (D_AES_ZERO_U32 != nb_blocks)) */
    /* If auth and cipher are 0 or cipher is 0 is done here */
    else if ((ER_OK == err) && (D_AES_ZERO_U32 == nb_blocks))
    {
        param_finish.number               = D_AES_ZERO_U16;
        param_finish.block_info           = (uint16_t)((uint32_t)block_info | D_AES_BLOCK_INFO_LAST | (uint32_t)rem);
        param_finish.p_aes_src            = p_param->p_aes_src;
        param_finish.p_total_size_in_bits = total_size;
        param_finish.p_ctr_j0             = (uint32_t*)(&j0);
        param_finish.p_context            = p_param->p_context;
        param_finish.p_aes_dest           = p_param->p_aes_dest;
        param_finish.p_auth_tag           = p_param->p_auth_tag;
        err = D_AES_FinishJobGcm(&param_finish);
    }
    else
    {
        /* do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0091]:[ICUMFW_UD_S4_D_AES_0091]
* Function name: d_AES__AddCounter
* Description  : 64bit data(p_counter[0],p_counter[1])
*              : + add_counter
* Arguments    : p_counter (Pointer to the 64-bit value to be added)
*              : add_counter (The 32-bit value to add)
* Return Value : ER_OK
***************************************************************************************/
uint32_t d_AES_AddCounter(uint32_t *p_counter, uint32_t add_counter)
{
    p_counter[1] = L_Toolbox_ByteSwap(p_counter[1]);
    /* If p_counter[1] overflows */
    if ((COUNTER_MAX - p_counter[1]) < add_counter)
    {
        p_counter[0] = L_Toolbox_ByteSwap(p_counter[0]);
        ++p_counter[0];
        p_counter[0] = L_Toolbox_ByteSwap(p_counter[0]);
    }
    else
    {
        /* do nothing */
    }
    p_counter[1] += add_counter;
    p_counter[1] = L_Toolbox_ByteSwap(p_counter[1]);

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0052]:[ICUMFW_UD_S4_D_AES_0052]
* Function name: D_AES_InitJobCcm
* Description  : Set parameters of crypto AES CCM.
* Arguments    : direction       : cipher direction
*                                  Range: D_AES_DIR_ENCRYPTION
*                                         D_AES_DIR_DECRYPTION
*                p_size_data     : address of B0 data
*                p_aes_key       : address of key value
*                key_length      : key length
*                                  Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                         D_AES_KEY_SIZE_IN_BYTES_192BIT
*                                         D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv        : address of IV value
*                p_context       : address of intermediate value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_SEQUENCE_ERROR
*                ER_PENDING
***************************************************************************************/
uint32_t D_AES_InitJobCcm(aes_init_ccm_t* p_param)
{
    uint32_t err;
    ccm_context_t *p_context;
    aes_ccm_setparam_t setparam;

    err = ER_OK;
    p_context = NULL;
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_ccm_setparam_t));

    /* Check if the context address is null. */
    if (NULL == p_param->p_context)
    {
        err = ER_PARAM;
    }
    /* Alignment check of IV address. */
    else if (ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_iv))
    {
        err = ER_D_AES_ALIGN_ERROR;
    }
    /* Check key size */
    else if ((D_AES_KEY_SIZE_IN_BYTES_128BIT != (uint32_t)p_param->key_length) &&
             (D_AES_KEY_SIZE_IN_BYTES_192BIT != (uint32_t)p_param->key_length) &&
             (D_AES_KEY_SIZE_IN_BYTES_256BIT != (uint32_t)p_param->key_length))
    {
        err = ER_PARAM;
    }
    else
    {
        err = d_AES_GetKeyRestriction(p_param->key_length, &((ccm_context_t*)p_param->p_context)->key_length_id);
        if (ER_OK == err)
        {
            /* Set the intermediate value area. */
            p_context = (ccm_context_t*)p_param->p_context;
            p_context->direction = p_param->direction;
            p_context->key_length = p_param->key_length;
            /* Create parameter group for batch setting. */
            setparam.direction = p_context->direction;
            setparam.p_ccm_key = p_param->p_aes_key;
            setparam.p_ccm_b0 = p_param->p_size_data;
            setparam.p_ccm_nonce = p_param->p_aes_iv;
            setparam.key_length = p_param->key_length;
            /* Set parameter group for batch setting. */
            err = d_AES_SetParam_CCM(&setparam);
            if (ER_OK == err)
            {
                /* Do nothing */
            }
            else
            {
                (void)d_AES_FreeKeyRestriction(p_context->key_length,
                                              ((ccm_context_t*)p_param->p_context)->key_length_id);
            }
        }
        else
        {
            /* Do nothing */
        }
    } /* end of else */

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0053]:[ICUMFW_UD_S4_D_AES_0053]
* Function name: D_AES_UpdateJobCcm
* Description  : Execute encryption of crypto AES CCM.
* Arguments    : number     : Number of blocks to be encrypted or decrypted
*                             Range: 0x1 - 0xFFFFFFFF
*                block_info : block information
*                             Range: D_AES_BLOCK_INFO_LAST
*                                    D_AES_BLOCK_INFO_CCM_PLEN0
*                                    D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                step       : Operating procedure
*                             Range: D_AES_CCM_STEP_C
*                                    D_AES_CCM_STEP_A
*                                    D_AES_CCM_STEP_P
*                p_aes_src  : address of input value
*                p_context  : address of intermediate value
*                p_aes_dest : address of output value
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
uint32_t D_AES_UpdateJobCcm(aes_update_ccm_t* p_param)
{
    uint32_t err;
    ccm_context_t *p_context;
    aes_ccm_t ccm_param;
    aes_ccm_a_t a_param;
    aes_ccm_p_t pay_param;

    err = ER_OK;
    p_context = NULL;
    L_Toolbox_MemSet((uint8_t *)&ccm_param, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_ccm_t));
    L_Toolbox_MemSet((uint8_t *)&a_param, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_ccm_a_t));
    L_Toolbox_MemSet((uint8_t *)&pay_param, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_ccm_p_t));

    /* Check if the context address is null. */
    if (NULL == p_param->p_context)
    {
        err = ER_PARAM;
    }
    /* Check the range of number. */
    else if (D_AES_NUMBER_MIN > p_param->number)
    {
        err = ER_PARAM;
    }
    /* Check range block_info */
    else if (D_AES_BLOCK_INFO_SIZE_MAX < (D_AES_BLOCK_INFO_SIZE & (uint32_t)p_param->block_info))
    {
        err = ER_PARAM;
    }
    /* Alignment check of input address. */
    else if (ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_src))
    {
        err = ER_D_AES_ALIGN_ERROR;
    }
    else
    {
        /* Set the intermediate value area. */
        p_context = (ccm_context_t*)p_param->p_context;
        /* Check step */
        if (((D_AES_DIR_ENCRYPTION == (uint32_t)p_context->direction)
          && (D_AES_CCM_STEP_P == (uint32_t)p_param->step))
         || ((D_AES_DIR_DECRYPTION == (uint32_t)p_context->direction)
          && (D_AES_CCM_STEP_C == (uint32_t)p_param->step)))
        {
            /* Alignment check of output address. */
            if (ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_aes_dest))
            {
                /* Create a parameter group for cryptographic processing. */
                ccm_param.direction = p_context->direction;
                ccm_param.ccm_block_info = p_param->block_info;
                ccm_param.ccm_number = p_param->number;
                ccm_param.p_ccm_src = p_param->p_aes_src;
                ccm_param.p_ccm_dest = p_param->p_aes_dest;
                /* Execute cryptographic processing. */
                err = d_AES_ProcessNBlocks_CCM(&ccm_param);
            }
            else
            {
                err = ER_D_AES_ALIGN_ERROR;
            }
        }
        else if (D_AES_CCM_STEP_A == (uint32_t)p_param->step)
        {
            /* Create a parameter group for Associated data calculation. */
            a_param.direction = p_context->direction;
            a_param.a_block_info = p_param->block_info;
            a_param.a_number = p_param->number;
            a_param.p_associated = p_param->p_aes_src;
            /* Calculate Associated data. */
            err = d_AES_SetAssociatedData(&a_param);
        }
        else if ((D_AES_DIR_DECRYPTION == (uint32_t)p_context->direction)
              && (D_AES_CCM_STEP_P == (uint32_t)p_param->step))
        {
            /* Create a parameter group for authentication tag calculation. */
            pay_param.payload_block_info = p_param->block_info;
            pay_param.payload_number = p_param->number;
            pay_param.p_payload = p_param->p_aes_src;
            /* Calculate authentication tag. */
            err = d_AES_SetPayloadData(&pay_param);
        }
        else
        {
            err = ER_PARAM;
        }
    } /* end of else */

    if ((ER_OK != err) && (NULL == p_param->p_context))
    {
        (void)d_AES_FreeKeyRestriction(((ccm_context_t*)p_param->p_context)->key_length,
                                       ((ccm_context_t*)p_param->p_context)->key_length_id);
    }
    else
    {
        /* do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0054]:[ICUMFW_UD_S4_D_AES_0054]
* Function name: D_AES_FinishJobCcm
* Description  : Finish execution of crypto AES CCM.
* Arguments    : operation  : Continue or cancel operation
*                             Range: D_AES_OPE_CONTINUE
*                                    D_AES_OPE_CANCEL
*                p_aes_src  : address of input value
*                p_context  : address of intermediate value
*                p_auth_yr  : address of authentication tag data(Yr)
*                p_auth_tag : address of authentication tag data(T)
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
uint32_t D_AES_FinishJobCcm(aes_finish_ccm_t* p_param)
{
    uint32_t err;
    ccm_context_t *p_context;
    aes_ccm_tag_dec_t tag_param;

    err = ER_OK;
    p_context = NULL;
    L_Toolbox_MemSet((uint8_t *)&tag_param, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_ccm_tag_dec_t));

    /* Continue operation */
    if (D_AES_OPE_CONTINUE == (uint32_t)p_param->operation)
    {
        /* Check if the context address is null. */
        if (NULL == p_param->p_context)
        {
            err = ER_PARAM;
        }
        /* Alignment check of tag address. */
        else if (ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_auth_tag))
        {
            err = ER_D_AES_ALIGN_ERROR;
        }
        else
        {
            /* Set the intermediate value area. */
            p_context = (ccm_context_t*)p_param->p_context;
            if (D_AES_DIR_ENCRYPTION == (uint32_t)p_context->direction)
            {
                /* Get an authentication tag. */
                err = d_AES_GetAuthTag_CcmEnc(p_param->p_auth_tag);
            }
            else
            {
                /* Alignment check of input address and tag address. */
                if ((ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_aes_src))
                 || (ALIGN_OK != (ALIGN4 & (uint32_t)p_param->p_auth_yr)))
                {
                    err = ER_D_AES_ALIGN_ERROR;
                }
                else
                {
                    /* Create a parameter group for authentication tag calculation. */
                    tag_param.p_tag_part = p_param->p_aes_src;
                    tag_param.p_auth_yr = p_param->p_auth_yr;
                    tag_param.p_auth_tag = p_param->p_auth_tag;
                    /* Get authentication tags. */
                    err = d_AES_GetAuthTag_CcmDec(&tag_param);
                }
            }
        } /* end of else */
    } /* end of if (D_AES_OPE_CONTINUE == p_param->operation) */
    else
    {
        /* Cancel cryptographic processing. */
        (void)d_AES_Cancel();
    }
    if (NULL != p_param->p_context)
    {
        if (ER_OK == err)
        {
            err = d_AES_FreeKeyRestriction(((ccm_context_t*)p_param->p_context)->key_length,
                                           ((ccm_context_t*)p_param->p_context)->key_length_id);
        }
        else
        {
            (void)d_AES_FreeKeyRestriction(((ccm_context_t*)p_param->p_context)->key_length,
                                           ((ccm_context_t*)p_param->p_context)->key_length_id);
        }
    }
    else
    {
        /* do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0055]:[ICUMFW_UD_S4_D_AES_0055]
* Function name: D_AES_ExecutionAtOnceCcm
* Description  : Execute encryption of crypto AES at once. (for CCM)
* Arguments    : direction              : cipher direction
*                                         Range: D_AES_DIR_ENCRYPTION
*                                                D_AES_DIR_DECRYPTION
*                p_aes_key              : address of key value
*                key_length             : key length
*                                         Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                                D_AES_KEY_SIZE_IN_BYTES_192BIT
*                                                D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv               : address of IV value
*                iv_size_in_bits        : IV size in bits
*                p_auth_data            : address of AAD data
*                auth_data_size_in_bits : AAD data size in bits
*                p_aes_src              : address of input value
*                data_size_in_bits      : input value size in bits
*                auth_tag_size_in_bits  : authentication tag size in bits
*                async                  : sync/async transfer mode
*                                         Range: D_AES_TRANS_SYNC
*                                                D_AES_TRANS_ASYNC
*                p_aes_dest             : address of output value
*                p_auth_tag             : address of authentication tag
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_PENDING
*                ER_D_AES_SEQUENCE_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
uint32_t D_AES_ExecutionAtOnceCcm(aes_all_at_once_ccm_t* p_param)
{
    uint32_t err;
    uint32_t check_size;
    uint8_t mem_check_src;
    uint8_t mem_check_dst;
    uint8_t mem_check_auth_data;

    err = ER_OK;

    /* Check range IV size in bits */
    if ((D_AES_CCM_IV_SIZE_IN_BITS_56  == p_param->iv_size_in_bits) ||
        (D_AES_CCM_IV_SIZE_IN_BITS_64  == p_param->iv_size_in_bits) ||
        (D_AES_CCM_IV_SIZE_IN_BITS_72  == p_param->iv_size_in_bits) ||
        (D_AES_CCM_IV_SIZE_IN_BITS_80  == p_param->iv_size_in_bits) ||
        (D_AES_CCM_IV_SIZE_IN_BITS_88  == p_param->iv_size_in_bits) ||
        (D_AES_CCM_IV_SIZE_IN_BITS_96  == p_param->iv_size_in_bits) ||
        (D_AES_CCM_IV_SIZE_IN_BITS_104 == p_param->iv_size_in_bits))
    {
       /* Check range authentication data size in bits */
        if (D_AES_CCM_AUTH_DATA_SIZE_IN_BITS_MAX >= p_param->auth_data_size_in_bits)
        {
            /* Do nothing */
        }
        else
        {
            err = ER_PARAM;
        }
    }
    else
    {
        err = ER_PARAM;
    }
    if (ER_OK == err)
    {
        /* Check range authentication tag size in bits */
        if ((D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_32  == p_param->auth_tag_size_in_bits) ||
            (D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_48  == p_param->auth_tag_size_in_bits) ||
            (D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_64  == p_param->auth_tag_size_in_bits) ||
            (D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_80  == p_param->auth_tag_size_in_bits) ||
            (D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_96  == p_param->auth_tag_size_in_bits) ||
            (D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_112 == p_param->auth_tag_size_in_bits) ||
            (D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_128 == p_param->auth_tag_size_in_bits))
        {
            /* do nothing */
        }
        else
        {
            err = ER_PARAM;
        }
    }
    else
    {
        /* do nothing */
    }
    if (ER_OK == err)
    {
        /* Alignment check of input address and output address. */
        if ((ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_aes_iv))
         && (ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_auth_data))
         && (ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_aes_src))
         && (ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_aes_dest))
         && (ALIGN_OK == (ALIGN4 & (uint32_t)p_param->p_auth_tag)))
        {
            /* do nothing */
        }
        else
        {
            err = ER_D_AES_ALIGN_ERROR;
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* do nothing */
    }
    if (ER_OK == err)
    {
        /* Check key size */
        if ((D_AES_KEY_SIZE_IN_BYTES_128BIT == (uint32_t)p_param->key_length) ||
            (D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)p_param->key_length) ||
            (D_AES_KEY_SIZE_IN_BYTES_256BIT == (uint32_t)p_param->key_length))
        {
            /* do nothing */
        }
        else
        {
            err = ER_PARAM;
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* do nothing */
    }
    if (ER_OK == err)
    {
        /* Check sync/async transfer mode */
        if ((D_AES_TRANS_SYNC  == (uint32_t)p_param->async) ||
            (D_AES_TRANS_ASYNC == (uint32_t)p_param->async))
        {
            /* do nothing */
        }
        else
        {
            err = ER_PARAM;
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* do nothing */
    }
    if (ER_OK == err)
    {
        err = d_AES_GetKeyRestriction(p_param->key_length, &g_aes_atonce_id_use_mng);
        if (ER_OK == err)
        {
            check_size = (p_param->data_size_in_bits + ((uint32_t)D_AES_BYTE_SIZE_IN_BITS - D_AES_SET1_U32)) / (uint32_t)D_AES_BYTE_SIZE_IN_BITS;

            /* Check that memory address is outside the range of ICUM Local RAM */
            err = d_AES_CheckMemory((uint32_t)p_param->p_aes_src, check_size, &mem_check_src);
            if (ER_OK == err)
            {
                /* Check that memory address is outside the range of ICUM Local RAM */
                err = d_AES_CheckMemory((uint32_t)p_param->p_aes_dest, check_size, &mem_check_dst);
                if (ER_OK == err)
                {
                    /* Check that memory address is outside the range of ICUM Local RAM */
                    err = d_AES_CheckMemory((uint32_t)p_param->p_auth_data,
                                            (p_param->auth_data_size_in_bits + ((uint32_t)D_AES_BYTE_SIZE_IN_BITS - D_AES_SET1_U32)) / (uint32_t)D_AES_BYTE_SIZE_IN_BITS,
                                            &mem_check_auth_data);
                    if (ER_OK == err)
                    {
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
                        if ((ALIGN_OK == (ALIGN256 & (uint32_t)p_param->p_aes_src))
                         && (ALIGN_OK == (ALIGN256 & (uint32_t)p_param->p_aes_dest))
                         && (ALIGN_OK == (ALIGN256 & (uint32_t)p_param->p_auth_data))
#else
                        if ((ALIGN_OK == (ALIGN8 & (uint32_t)p_param->p_aes_dest))
#endif
                         && ((uint32_t)D_AES_CHK_OK == (uint32_t)mem_check_src)
                             && ((uint32_t)D_AES_CHK_OK == (uint32_t)mem_check_dst)
                             && ((uint32_t)D_AES_CHK_OK == (uint32_t)mem_check_auth_data)
                             && ((uint32_t)D_AES_ZERO_U32 == (uint32_t)(p_param->data_size_in_bits % D_AES_SET128))
                             && ((uint32_t)D_AES_ZERO_U32 == (uint32_t)(p_param->auth_data_size_in_bits % D_AES_SET128)))
                        {
                            err = d_AES_ExecutionAtOnceCcmDtf(p_param);
                        }
                        else
                        {
                            err = d_AES_ExecutionAtOnceCcmIcup(p_param);
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
            }
            else
            {
                /* do nothing */
            }
            if (ER_IN_PROCESS == err)
            {
                /* do nothing */
            }
            else if (ER_OK == err)
            {
                err = d_AES_FreeKeyRestriction(p_param->key_length, g_aes_atonce_id_use_mng);
            }
            else
            {
                (void)d_AES_FreeKeyRestriction(p_param->key_length, g_aes_atonce_id_use_mng);
            }
        }
        else
        {
            /* do nothing */
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0056]:[ICUMFW_UD_S4_D_AES_0056]
* Function name: d_AES_ExecutionAtOnceCcmIcup
* Description  : Execute encryption of crypto AES at once. (for CCM)
* Arguments    : direction              : cipher direction
*                                         Range: D_AES_DIR_ENCRYPTION
*                                                D_AES_DIR_DECRYPTION
*                p_aes_key              : address of key value
*                key_length             : key length
*                                         Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                                D_AES_KEY_SIZE_IN_BYTES_192BIT
*                                                D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv               : address of IV value
*                iv_size_in_bits        : IV size in bits
*                p_auth_data            : address of AAD data
*                auth_data_size_in_bits : AAD data size in bits
*                p_aes_src              : address of input value
*                data_size_in_bits      : input value size in bits
*                auth_tag_size_in_bits  : authentication tag size in bits
*                async                  : sync/async transfer mode
*                                         Range: D_AES_TRANS_SYNC
*                                                D_AES_TRANS_ASYNC
*                p_aes_dest             : address of output value
*                p_auth_tag             : address of authentication tag
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_PENDING
***************************************************************************************/
static uint32_t d_AES_ExecutionAtOnceCcmIcup(aes_all_at_once_ccm_t* p_param)
{
    uint32_t err;
    aes_init_ccm_t param_init;
    aes_update_ccm_t param_update;
    aes_finish_ccm_t param_finish;
    uint8_t B0[AES_BLOCK_SIZE_IN_BYTES] = {D_AES_ZERO_U8};
    uint8_t iv_data[AES_BLOCK_SIZE_IN_BYTES] = {D_AES_ZERO_U8};
    uint16_t block_info;
    uint32_t nb_blocks;
    uint8_t rem;
    uint32_t nb_blocks_auth;
    uint8_t rem_auth;
    uint8_t sift;
    uint32_t auth_tag[NUM4] = {D_AES_ZERO_U32};

    err = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&param_init, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_init_ccm_t));
    L_Toolbox_MemSet((uint8_t *)&param_update, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_update_ccm_t));
    L_Toolbox_MemSet((uint8_t *)&param_finish, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_finish_ccm_t));
    block_info = D_AES_ZERO_U16;
    nb_blocks = D_AES_ZERO_U32;
    rem = D_AES_ZERO_U8;
    nb_blocks_auth = D_AES_ZERO_U32;
    rem_auth = D_AES_ZERO_U8;
    sift = D_AES_ZERO_U8;

    if (D_AES_ZERO_U32 == p_param->data_size_in_bits)
    {
        nb_blocks = D_AES_ZERO_U32;
        rem = AES_BLOCK_SIZE_IN_BITS;
    }
    else
    {
        nb_blocks = ((p_param->data_size_in_bits + (AES_BLOCK_SIZE_IN_BITS - D_AES_SET1_U32)) /
            AES_BLOCK_SIZE_IN_BITS);
        rem = (uint8_t)(p_param->data_size_in_bits % AES_BLOCK_SIZE_IN_BITS);
        if (D_AES_ZERO_U32 == (uint32_t)rem)
        {
            rem = AES_BLOCK_SIZE_IN_BITS;
        }
        else
        {
            /* do nothing */
        }
    } /* end of else */

    if (D_AES_ZERO_U32 == p_param->auth_data_size_in_bits)
    {
        nb_blocks_auth = D_AES_ZERO_U32;
        rem_auth = AES_BLOCK_SIZE_IN_BITS;
    }
    else
    {
        nb_blocks_auth = ((p_param->auth_data_size_in_bits + (AES_BLOCK_SIZE_IN_BITS - D_AES_SET1_U32)) /
            AES_BLOCK_SIZE_IN_BITS);
        rem_auth = (uint8_t)(p_param->auth_data_size_in_bits % AES_BLOCK_SIZE_IN_BITS);
        if (D_AES_ZERO_U32 == (uint32_t)rem_auth)
        {
            rem_auth = AES_BLOCK_SIZE_IN_BITS;
        }
        else
        {
            /* do nothing */
        }
    } /* end of else */

    /* Generate size data (B0) */
    err = d_AES_GenerateB0(B0, p_param->iv_size_in_bits, p_param->auth_tag_size_in_bits,
        p_param->data_size_in_bits, p_param->auth_data_size_in_bits);
    if (ER_OK == err)
    {
        /* Generate IV data */
        L_Toolbox_MemSet((uint8_t*)iv_data, (uint8_t)D_AES_SET_ZERO_16, AES_BLOCK_SIZE_IN_BYTES);
        sift = (uint8_t)(AES_BLOCK_SIZE_IN_BYTES - (p_param->iv_size_in_bits / D_AES_BYTE_SIZE_IN_BITS));
        L_Toolbox_MemCpy(&iv_data[sift],
                        (const uint8_t *)p_param->p_aes_iv,
                        (int16_t)(p_param->iv_size_in_bits / D_AES_BYTE_SIZE_IN_BITS));

        block_info |= D_AES_BLOCK_INFO_FIRST;

        param_init.direction   = p_param->direction;
        param_init.p_size_data = (uint32_t *)B0;
        param_init.p_aes_key   = (uint32_t *)p_param->p_aes_key;
        param_init.p_aes_iv    = (uint32_t *)iv_data;
        param_init.p_context   = p_param->p_context;
        param_init.key_length  = p_param->key_length;
        err = D_AES_InitJobCcm(&param_init);
    }
    else
    {
        /* do nothing */
    }

    if (ER_OK == err)
    {
        if (D_AES_DIR_ENCRYPTION == (uint32_t)p_param->direction)
        {
            err = d_AES_ExecutionAtOnceCcmIcupEnc(p_param, &param_update,
                                                  nb_blocks_auth, block_info, rem_auth, nb_blocks, rem);
        }
        else
        {   /* D_AES_DIR_DECRYPTION */
            err = d_AES_ExecutionAtOnceCcmIcupDec(p_param, &param_update,
                                                  nb_blocks_auth, block_info, rem_auth, nb_blocks, rem);
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* do nothing */
    }

    if (ER_OK == err)
    {
        /* job finish */
        if (D_AES_DIR_DECRYPTION == (uint32_t)p_param->direction)
        {
            param_finish.p_aes_src = p_param->p_auth_tag;
            param_finish.p_auth_yr = p_param->p_auth_yr;
        }
        else
        {
            param_finish.p_aes_src = NULL;
            param_finish.p_auth_yr = NULL;
        }
        param_finish.operation = D_AES_OPE_CONTINUE;
        param_finish.p_auth_tag = auth_tag;
        param_finish.p_context = p_param->p_context;
        err = D_AES_FinishJobCcm(&param_finish);
        if (ER_OK == err)
        {
            L_Toolbox_MemCpy((uint8_t *)p_param->p_auth_tag,
                             (const uint8_t *)auth_tag,
                             (int16_t)(p_param->auth_tag_size_in_bits / D_AES_BYTE_SIZE_IN_BITS));
        }
        else
        {
            /* do nothing */
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* do nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0092]:[ICUMFW_UD_S4_D_AES_0092]
* Function name: d_AES_GenerateB0
* Description  : Generate B0 (for CCM)
* Arguments    : p_B0                   : address of B0
*                iv_size_in_bits        : IV size in bits
*                auth_tag_size_in_bits  : AAD data size in bits
*                data_size_in_bits      : input value size in bits
*                auth_data_size_in_bits : authentication tag size in bits
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t d_AES_GenerateB0(uint8_t *p_B0, uint32_t iv_size_in_bits, uint32_t auth_tag_size_in_bits,
    uint32_t data_size_in_bits, uint32_t auth_data_size_in_bits)
{
    uint32_t err;
    uint32_t local_size;

    err = ER_OK;
    local_size = D_AES_ZERO_U32;

    L_Toolbox_MemSet((uint8_t*)p_B0, (uint8_t)D_AES_SET_ZERO_16, (int16_t)AES_BLOCK_SIZE_IN_BYTES);
    switch (iv_size_in_bits)
    {
        case D_AES_CCM_IV_SIZE_IN_BITS_56:
            p_B0[NUM5] = (uint8_t)(D_AES_NLEN_BIT_56 << D_AES_SET_4);
            break;
        case D_AES_CCM_IV_SIZE_IN_BITS_64:
            p_B0[NUM5] = (uint8_t)(D_AES_NLEN_BIT_64 << D_AES_SET_4);
            break;
        case D_AES_CCM_IV_SIZE_IN_BITS_72:
            p_B0[NUM5] = (uint8_t)(D_AES_NLEN_BIT_72 << D_AES_SET_4);
            break;
        case D_AES_CCM_IV_SIZE_IN_BITS_80:
            p_B0[NUM5] = (uint8_t)(D_AES_NLEN_BIT_80 << D_AES_SET_4);
            break;
        case D_AES_CCM_IV_SIZE_IN_BITS_88:
            p_B0[NUM5] = (uint8_t)(D_AES_NLEN_BIT_88 << D_AES_SET_4);
            break;
        case D_AES_CCM_IV_SIZE_IN_BITS_96:
            p_B0[NUM5] = (uint8_t)(D_AES_NLEN_BIT_96 << D_AES_SET_4);
            break;
        case D_AES_CCM_IV_SIZE_IN_BITS_104:
            p_B0[NUM5] = (uint8_t)(D_AES_NLEN_BIT_104 << D_AES_SET_4);
            break;
        default:
            err = ER_PARAM;
            break;
    } /* end of switch (iv_size_in_bits) */

    if (ER_OK == err)
    {
        switch (auth_tag_size_in_bits)
        {
            case D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_32:
                p_B0[NUM5] |= D_AES_TLEN_BIT_32;
                break;
            case D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_48:
                p_B0[NUM5] |= D_AES_TLEN_BIT_48;
                break;
            case D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_64:
                p_B0[NUM5] |= D_AES_TLEN_BIT_64;
                break;
            case D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_80:
                p_B0[NUM5] |= D_AES_TLEN_BIT_80;
                break;
            case D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_96:
                p_B0[NUM5] |= D_AES_TLEN_BIT_96;
                break;
            case D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_112:
                p_B0[NUM5] |= D_AES_TLEN_BIT_112;
                break;
            case D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_128:
                p_B0[NUM5] |= D_AES_TLEN_BIT_128;
                break;
            default:
                err = ER_PARAM;
                break;
        } /* end of switch (auth_tag_size_in_bits) */
    } /* end of if (ER_OK == err) */
    else
    {
        /* do nothing */
    }

    if (ER_OK == err)
    {
        local_size = data_size_in_bits;
        p_B0[NUM15] = (uint8_t)(local_size & D_AES_SET_FF_16); local_size = local_size >> D_AES_SET_8;
        p_B0[NUM14] = (uint8_t)(local_size & D_AES_SET_FF_16); local_size = local_size >> D_AES_SET_8;
        p_B0[NUM13] = (uint8_t)(local_size & D_AES_SET_FF_16); local_size = local_size >> D_AES_SET_8;
        p_B0[NUM12] = (uint8_t)(local_size & D_AES_SET_FF_16);

        local_size = auth_data_size_in_bits;
        p_B0[NUM3] = (uint8_t)(local_size & D_AES_SET_FF_16); local_size = local_size >> D_AES_SET_8;
        p_B0[NUM2] = (uint8_t)(local_size & D_AES_SET_FF_16); local_size = local_size >> D_AES_SET_8;
        p_B0[NUM1] = (uint8_t)(local_size & D_AES_SET_07_16);
    }
    else
    {
        /* do nothing */
    }

    return err;

}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0073]:[ICUMFW_UD_S4_D_AES_0073]
* Function name: d_AES_GetKeyRestriction
* Description  : Get key restriction
* Arguments    : key_length : key length
*                p_key_length_id : key length usage management id
* Return Value : ER_OK
*                ER_PENDING
***************************************************************************************/
static uint32_t d_AES_GetKeyRestriction(uint16_t key_length, uint32_t* p_key_length_id)
{
    uint32_t err;
    uint32_t count_nb;

    err = ER_OK;
    count_nb = D_AES_ZERO_U32;

    if (D_AES_KEYLEN_MAX_ID != g_aes_gen_key_length_id)
    {
        g_aes_gen_key_length_id++;
    }
    else
    {
        g_aes_gen_key_length_id = D_AES_KEYLEN_ID_RETURN1;
    }

    if ((D_AES_ZERO_U16 == (uint32_t)g_key_restriction.counter) ||
        ((uint32_t)key_length == (uint32_t)g_key_restriction.key_length))
    {
        /* Check key length id */
        for(count_nb = D_AES_ZERO_U32; D_AES_KEYLEN_ID_USE_MNG > count_nb; count_nb++)
        {
            /* Found the free key length id */
            if(D_AES_KEYLEN_ID_FREE == g_key_restriction.key_length_id[count_nb])
            {
                /* Not using AES HW */
                g_key_restriction.key_length = key_length;
                g_key_restriction.counter++;

                *p_key_length_id = g_aes_gen_key_length_id;
                g_key_restriction.key_length_id[count_nb] = g_aes_gen_key_length_id;
                break;
            }
            else
            {
                /* Do nothing */
            }
        }
        /* Not found the free key length id */
        if(D_AES_KEYLEN_ID_USE_MNG == count_nb)
        {
            err = ER_PENDING;
        }
        else
        {
            /* Do nothing */
        }
    }

    else
    {
        /* In use by different key length */
        err = ER_PENDING;
    }

    return err;
}

PRAGMA(ghs section text = ".icum_driver_ram")
PRAGMA(ghs section rodata = ".icum_driver_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0074]:[ICUMFW_UD_S4_D_AES_0074]
* Function name: d_AES_FreeKeyRestriction
* Description  : Free key restriction
* Arguments    : key_length : key length
*                key_length_id : key length usage management id
* Return Value : ER_OK
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
uint32_t d_AES_FreeKeyRestriction(uint16_t key_length, uint32_t key_length_id)
{
    uint32_t err;
    uint32_t count_nb;

    err = ER_OK;
    count_nb = D_AES_ZERO_U32;

    if (D_AES_KEYLEN_ID_FREE == key_length_id)
    {
        /* Do nothing */
    }
    else if ((uint32_t)g_key_restriction.key_length != (uint32_t)key_length)
    {
        /* In use by different key length */
        err = ER_D_AES_SEQUENCE_ERROR;
    }
    else
    {
        /* Check key length id */
        for(count_nb = D_AES_ZERO_U32; D_AES_KEYLEN_ID_USE_MNG > count_nb; count_nb++)
        {
            /* Found the specified key length id */
            if(g_key_restriction.key_length_id[count_nb] == key_length_id)
            {
                /* In use by same key length */
                g_key_restriction.counter--;
                g_key_restriction.key_length_id[count_nb] = D_AES_KEYLEN_ID_FREE;
                break;
            }
            else
            {
                /* Do nothing */
            }
        }
    }

    return err;
}

PRAGMA(ghs section text = default)
PRAGMA(ghs section rodata = default)
#if defined(TARGET_DEVICE_S4H) || defined(TARGET_DEVICE_U2B) || defined(TARGET_DEVICE_U2C) || defined(TARGET_DEVICE_U2C4)
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0088]:[ICUMFW_UD_S4_D_AES_0088]
* Function name: ICICUAESE0CMPERR
* Description  : Interrupt handler for ICICUAESE0CMPERR (ROM mode)
* Arguments    : none
* Return Value : none
***************************************************************************************/
void ICICUAESE0CMPERR(void)
{
    d_AES_IsrCmpErr();

    return;
}
#endif
#if defined(TARGET_DEVICE_S4H) || defined(TARGET_DEVICE_U2B) || defined(TARGET_DEVICE_U2C) || defined(TARGET_DEVICE_U2C4)
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0090]:[ICUMFW_UD_S4_D_AES_0090]
* Function name: d_AES_IsrCmpErr
* Description  : Interrupt handler for ICICUAESE0CMPERR (Common)
* Arguments    : none
* Return Value : none
***************************************************************************************/
static inline void d_AES_IsrCmpErr(void)
{
    if (NULL != g_p_aes_callbackFunc)
    {
        g_p_aes_callbackFunc(ER_D_AES_CMP_ERROR);
    }
    else
    {
        /* Do Nothing */
    }

    return;
}
#endif
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0093]:[ICUMFW_UD_S4_D_AES_0093]
* Function name: d_AES_CheckMemory
* Description  : Check of memory address
* Arguments    : start_addr     : Address to be checked
*                size           : Data size
*                p_result       : Check result
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
static uint32_t d_AES_CheckMemory(const uint32_t start_addr, const uint32_t size, uint8_t *p_result)
{
    uint32_t err;
    uint32_t end_addr;

    if (NULL == p_result)
    {
        err = ER_PARAM;
    }
    else
    {
        /* over address check */
        if ((D_AES_MAX_ADDRESS - start_addr) < size)
        {
            /* end address over 0xFFFFFFFF */
            *p_result = D_AES_CHK_NG;
            err = ER_OK;
        }
        else
        {
            end_addr = start_addr + size;
            if (D_AES_ZERO_U32 < size)
            {
                end_addr--;
            }
            else
            {
                /* Do Nothing */
            }

            if ((D_AES_ADDR_LOWER_ICUM_LOCAL_RAM > end_addr)
             || (D_AES_ADDR_UPPER_ICUM_LOCAL_RAM < start_addr))
            {
                *p_result = D_AES_CHK_OK;
                err = ER_OK;
            }
            else
            {
                *p_result = D_AES_CHK_NG;
                err = ER_OK;
            }
        }
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0111]:[ICUMFW_UD_S4_D_AES_0111]
* Function name: d_AES_ExecutionAtOnceCcmIcupEnc
* Description  : Execute encryption of crypto AES. (direction is ENCRYPTION)
* Arguments    : direction              : cipher direction
*                                         Range: D_AES_DIR_ENCRYPTION
*                                                D_AES_DIR_DECRYPTION
*                p_aes_key              : address of key value
*                key_length             : key length
*                                         Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                                D_AES_KEY_SIZE_IN_BYTES_192BIT
*                                                D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv               : address of IV value
*                iv_size_in_bits        : IV size in bits
*                p_auth_data            : address of AAD data
*                auth_data_size_in_bits : AAD data size in bits
*                p_aes_src              : address of input value
*                data_size_in_bits      : input value size in bits
*                auth_tag_size_in_bits  : authentication tag size in bits
*                async                  : sync/async transfer mode
*                                         Range: D_AES_TRANS_SYNC
*                                                D_AES_TRANS_ASYNC
*                p_aes_dest             : address of output value
*                p_auth_tag             : address of authentication tag
*                number                 : Number of blocks to be encrypted or decrypted
*                                         Range: 0x1 - 0xFFFFFFFF
*                block_info             : block information
*                                         Range: D_AES_BLOCK_INFO_LAST
*                                                D_AES_BLOCK_INFO_CCM_PLEN0
*                                                D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                step                   : Operating procedure
*                                         Range: D_AES_CCM_STEP_C
*                                                D_AES_CCM_STEP_A
*                                                D_AES_CCM_STEP_P
*                p_aes_src              : address of input value
*                p_context              : address of intermediate value
*                p_aes_dest             : address of output value
*              : nb_blocks_auth         : no blocks authentication
*              : block_info             : block information
*              : rem_auth               : rem authentication
*              : nb_blocks              : nb blocks
*              : rem                    : rem
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t d_AES_ExecutionAtOnceCcmIcupEnc(aes_all_at_once_ccm_t *p_param,
                                                aes_update_ccm_t *p_param_update,
                                                uint32_t nb_blocks_auth, uint16_t block_info, uint8_t rem_auth,
                                                uint32_t nb_blocks, uint8_t rem)
{
    uint32_t err;
    uint32_t cipher_data[AES_BLOCK_SIZE] = {D_AES_ZERO_U32};

    err = ER_OK;
    /* AAD operation processing */
    if (D_AES_ZERO_U32 != nb_blocks_auth)
    {
        p_param_update->step       = D_AES_CCM_STEP_A;
        p_param_update->block_info = (uint16_t)((uint32_t)block_info | D_AES_BLOCK_INFO_LAST | (uint32_t)rem_auth);
        p_param_update->number     = nb_blocks_auth;
        p_param_update->p_aes_src  = p_param->p_auth_data;
        p_param_update->p_context  = p_param->p_context;
        p_param_update->p_aes_dest = NULL;
        err = D_AES_UpdateJobCcm(p_param_update);
    }
    else
    {
        /* do nothing */
    }

    /* Cipher operation processing */
    if ((ER_OK == err) && (D_AES_ZERO_U32 != nb_blocks))
    {
        if (D_AES_SET128 == (uint32_t)rem)
        {
            p_param_update->step       = D_AES_CCM_STEP_P;
            p_param_update->block_info = (uint16_t)((uint32_t)block_info | D_AES_BLOCK_INFO_LAST | (uint32_t)rem);
            p_param_update->number     = nb_blocks;
            p_param_update->p_aes_src  = p_param->p_aes_src;
            p_param_update->p_context  = p_param->p_context;
            p_param_update->p_aes_dest = p_param->p_aes_dest;
            err = D_AES_UpdateJobCcm(p_param_update);
        }
        else
        {
            /* If there is a fraction, adjust the output size of the final block */
            if (D_AES_SET1_U32 < nb_blocks)
            {
                p_param_update->step       = D_AES_CCM_STEP_P;
                p_param_update->block_info = (uint16_t)block_info;
                p_param_update->number     = nb_blocks - D_AES_SET1_U32;
                p_param_update->p_aes_src  = p_param->p_aes_src;
                p_param_update->p_context  = p_param->p_context;
                p_param_update->p_aes_dest = p_param->p_aes_dest;
                err = D_AES_UpdateJobCcm(p_param_update);
                if (ER_OK == err)
                {
                    p_param_update->step       = D_AES_CCM_STEP_P;
                    p_param_update->block_info = (uint16_t)((uint32_t)block_info
                                               | D_AES_BLOCK_INFO_LAST
                                               | (uint32_t)rem);
                    p_param_update->number     = D_AES_SET1_U32;
                    p_param_update->p_aes_src  = p_param->p_aes_src + (D_AES_SET_4 * (nb_blocks - D_AES_SET1_U32));
                    p_param_update->p_context  = p_param->p_context;
                    p_param_update->p_aes_dest = cipher_data;
                    err = D_AES_UpdateJobCcm(p_param_update);
                    /* Output only valid sizes */
                    L_Toolbox_MemCpy((uint8_t *)(p_param->p_aes_dest + (D_AES_SET_4 * (nb_blocks - D_AES_SET1_U32))),
                                     (const uint8_t *)cipher_data,
                                     (int16_t)(((uint32_t)rem + (D_AES_BYTE_SIZE_IN_BITS - D_AES_SET1_U32)) /
                                               D_AES_BYTE_SIZE_IN_BITS));
                } /* end of if (err == ER_OK) */
                else
                {
                    /* do nothing */
                }
            } /* end of if (D_AES_SET1_U32 < nb_blocks) */
            else
            {
                p_param_update->step       = D_AES_CCM_STEP_P;
                p_param_update->block_info = (uint16_t)((uint32_t)block_info
                                           | D_AES_BLOCK_INFO_LAST
                                           | (uint32_t)rem);
                p_param_update->number     = D_AES_SET1_U32;
                p_param_update->p_aes_src  = p_param->p_aes_src;
                p_param_update->p_context  = p_param->p_context;
                p_param_update->p_aes_dest = cipher_data;
                err = D_AES_UpdateJobCcm(p_param_update);
                /* Output only valid sizes */
                L_Toolbox_MemCpy((uint8_t *)p_param->p_aes_dest,
                                 (const uint8_t *)cipher_data,
                                 (int16_t)(((uint32_t)rem + (D_AES_BYTE_SIZE_IN_BITS - D_AES_SET1_U32)) /
                                           D_AES_BYTE_SIZE_IN_BITS));
            }
        } /* end of if ((ER_OK == err) && (D_AES_ZERO_U32 != nb_blocks)) */
    }
    else
    {
        /* do nothing */
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0112]:[ICUMFW_UD_S4_D_AES_0112]
* Function name: d_AES_ExecutionAtOnceCcmIcupDec
* Description  : Execute encryption of crypto AES. (direction is DECRYPTION)
* Arguments    : direction              : cipher direction
*                                         Range: D_AES_DIR_ENCRYPTION
*                                                D_AES_DIR_DECRYPTION
*                p_aes_key              : address of key value
*                key_length             : key length
*                                         Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                                D_AES_KEY_SIZE_IN_BYTES_192BIT
*                                                D_AES_KEY_SIZE_IN_BYTES_256BIT
*                p_aes_iv               : address of IV value
*                iv_size_in_bits        : IV size in bits
*                p_auth_data            : address of AAD data
*                auth_data_size_in_bits : AAD data size in bits
*                p_aes_src              : address of input value
*                data_size_in_bits      : input value size in bits
*                auth_tag_size_in_bits  : authentication tag size in bits
*                async                  : sync/async transfer mode
*                                         Range: D_AES_TRANS_SYNC
*                                                D_AES_TRANS_ASYNC
*                p_aes_dest             : address of output value
*                p_auth_tag             : address of authentication tag
*                number                 : Number of blocks to be encrypted or decrypted
*                                         Range: 0x1 - 0xFFFFFFFF
*                block_info             : block information
*                                         Range: D_AES_BLOCK_INFO_LAST
*                                                D_AES_BLOCK_INFO_CCM_PLEN0
*                                                D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                step                   : Operating procedure
*                                         Range: D_AES_CCM_STEP_C
*                                                D_AES_CCM_STEP_A
*                                                D_AES_CCM_STEP_P
*                p_aes_src              : address of input value
*                p_context              : address of intermediate value
*                p_aes_dest             : address of output value
*              : nb_blocks_auth         : no blocks authentication
*              : block_info             : block information
*              : rem_auth               : rem authentication
*              : nb_blocks              : nb blocks
*              : rem                    : rem
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_ALIGN_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_D_AES_SEQUENCE_ERROR
***************************************************************************************/
static uint32_t d_AES_ExecutionAtOnceCcmIcupDec(aes_all_at_once_ccm_t *p_param,
                                                aes_update_ccm_t *p_param_update,
                                                uint32_t nb_blocks_auth, uint16_t block_info, uint8_t rem_auth,
                                                uint32_t nb_blocks, uint8_t rem)
{
    uint32_t err;
    uint32_t cipher_data[AES_BLOCK_SIZE] = {D_AES_ZERO_U32};

    err = ER_OK;
    if (D_AES_ZERO_U32 == p_param->data_size_in_bits)
    {
        block_info |= D_AES_BLOCK_INFO_CCM_PLEN0;
    }
    else
    {
        /* do nothing */
    }

    /* Cipher operation processing */
    if (D_AES_ZERO_U32 != nb_blocks)
    {
        if (D_AES_SET128 == (uint32_t)rem)
        {
            p_param_update->step       = D_AES_CCM_STEP_C;
            p_param_update->block_info = (uint16_t)((uint32_t)block_info | D_AES_BLOCK_INFO_LAST | (uint32_t)rem);
            p_param_update->number     = nb_blocks;
            p_param_update->p_aes_src  = p_param->p_aes_src;
            p_param_update->p_context  = p_param->p_context;
            p_param_update->p_aes_dest = p_param->p_aes_dest;
            err = D_AES_UpdateJobCcm(p_param_update);
        }
        else
        {
            /* If there is a fraction, adjust the output size of the final block */
            if (D_AES_SET1_U32 < nb_blocks)
            {
                p_param_update->step       = D_AES_CCM_STEP_C;
                p_param_update->block_info = (uint16_t)block_info;
                p_param_update->number     = nb_blocks - D_AES_SET1_U32;
                p_param_update->p_aes_src  = p_param->p_aes_src;
                p_param_update->p_context  = p_param->p_context;
                p_param_update->p_aes_dest = p_param->p_aes_dest;
                err = D_AES_UpdateJobCcm(p_param_update);
                if (ER_OK == err)
                {
                    p_param_update->step       = D_AES_CCM_STEP_C;
                    p_param_update->block_info = (uint16_t)((uint32_t)block_info
                                            | D_AES_BLOCK_INFO_LAST
                                            | (uint32_t)rem);
                    p_param_update->number     = D_AES_SET1_U32;
                    p_param_update->p_aes_src  = p_param->p_aes_src + (D_AES_SET_4 * (nb_blocks - D_AES_SET1_U32));
                    p_param_update->p_context  = p_param->p_context;
                    p_param_update->p_aes_dest = cipher_data;
                    err = D_AES_UpdateJobCcm(p_param_update);
                    /* Output only valid sizes */
                    L_Toolbox_MemCpy((uint8_t *)(p_param->p_aes_dest + (D_AES_SET_4 * (nb_blocks - D_AES_SET1_U32))),
                                     (const uint8_t *)cipher_data,
                                     (int16_t)(((uint32_t)rem + (D_AES_BYTE_SIZE_IN_BITS - D_AES_SET1_U32)) /
                                               D_AES_BYTE_SIZE_IN_BITS));
                } /* end of if (ER_OK == err) */
                else
                {
                    /* do nothing */
                }
            } /* end of else */
            else
            {
                p_param_update->step       = D_AES_CCM_STEP_C;
                p_param_update->block_info = (uint16_t)((uint32_t)block_info
                                           | D_AES_BLOCK_INFO_LAST
                                           | (uint32_t)rem);
                p_param_update->number     = D_AES_SET1_U32;
                p_param_update->p_aes_src  = p_param->p_aes_src;
                p_param_update->p_context  = p_param->p_context;
                p_param_update->p_aes_dest = cipher_data;
                err = D_AES_UpdateJobCcm(p_param_update);
                /* Output only valid sizes */
                L_Toolbox_MemCpy((uint8_t *)p_param->p_aes_dest,
                                 (const uint8_t *)cipher_data,
                                 (int16_t)(((uint32_t)rem + (D_AES_BYTE_SIZE_IN_BITS - D_AES_SET1_U32)) /
                                           D_AES_BYTE_SIZE_IN_BITS));
            }
        }
    } /* end of if (D_AES_ZERO_U32 != nb_blocks) */
    else
    {
        /* do nothing */
    }

    /* AAD operation processing */
    if ((ER_OK == err) && (D_AES_ZERO_U32 != nb_blocks_auth))
    {
        p_param_update->step       = D_AES_CCM_STEP_A;
        p_param_update->block_info = (uint16_t)((uint32_t)block_info
                                   | D_AES_BLOCK_INFO_LAST
                                   | (uint32_t)rem_auth);
        p_param_update->number     = nb_blocks_auth;
        p_param_update->p_aes_src  = p_param->p_auth_data;
        p_param_update->p_context  = p_param->p_context;
        p_param_update->p_aes_dest = NULL;
        err = D_AES_UpdateJobCcm(p_param_update);
    }
    else
    {
        /* do nothing */
    }

    if ((ER_OK == err)
        && (D_AES_ZERO_U32 != p_param->data_size_in_bits)
        && (D_AES_ZERO_U32 != (uint32_t)rem))
    {
        p_param_update->step       = D_AES_CCM_STEP_P;
        p_param_update->block_info = (uint16_t)((uint32_t)block_info
                                   | D_AES_BLOCK_INFO_LAST
                                   | (uint32_t)rem);
        p_param_update->number     = nb_blocks;
        p_param_update->p_aes_src  = p_param->p_aes_dest;
        p_param_update->p_context  = p_param->p_context;
        p_param_update->p_aes_dest = NULL;
        err = D_AES_UpdateJobCcm(p_param_update);
    }
    else
    {
        /* do nothing */
    }
    return (err);
}
