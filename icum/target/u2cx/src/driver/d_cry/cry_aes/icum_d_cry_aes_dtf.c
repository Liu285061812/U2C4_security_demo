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
#include "icum_d_remap_pub.h"
#include "icum_d_flash_pub.h"
#include "icum_d_cry_aes_pub.h"
#include "icum_d_cry_aes.h"
#include "icum_d_ost_pub.h"
/* Include prototypes of interrupt handlers */
#include "icum_int_declaration_pub.h"


/**************************************************************************************/
/* Global Variables                                                                   */
/**************************************************************************************/
#define NUM0                           (0u)
#define NUM2                           (2u)
#define NUM3                           (3u)
#define NUM4                           (4u)

/* Interrupt parameter */
static aes_dtf_info_t g_aes_dtf_info = {0};

/* gcm at once  */
static uint32_t g_aes_dtf_gcm_j0[D_AES_BLOCK_SIZE_4] = {0};
static uint32_t g_aes_dtf_gcm_j1[D_AES_BLOCK_SIZE_4] = {0};

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
static uint32_t d_AES_ConvertAddress(
    const uint32_t insrc, const uint32_t indest, const uint32_t inauth,
    uint32_t *p_outsrc, uint32_t *p_outdest, uint32_t *p_outauth,
    const uint16_t mode);
#endif
static uint32_t d_AES_SetParamDtf(aes_setparam_t *p_param);
static void d_AES_SetParamOtherCcm(uint8_t cipher_mode, uint8_t direction);
static void d_AES_SetParamCcm(uint8_t direction);
static uint32_t d_AES_TransferDtf(aes_transfer_dtf_t *p_param);
static uint32_t d_AES_CmacDtf(aes_nblocks_t* p_param, uint8_t async);
static uint32_t d_AES_ExecutionAtOnceCcmDtfSubEnc(aes_all_at_once_ccm_t *p_param);
static uint32_t d_AES_ExecutionAtOnceCcmDtfSubDec(aes_all_at_once_ccm_t *p_param);
static uint32_t d_AES_ProcessNBlocksDtf(aes_nblocks_t* p_param, uint8_t cipher_mode, uint32_t* p_aes_iv, uint8_t async);
static uint32_t d_AES_AtOnceXtsDtf(aes_all_at_once_xts_t* p_param);
static uint32_t d_AES_GetTagXts(aes_all_at_once_xts_t* p_param);
static void d_AES_KeySetXts_Dtf(uint32_t* p_aes_key, uint16_t key_length);
static void d_AES_GcmSetIV_NoIcup(uint32_t* p_iv);
static uint32_t d_AES_GcmSetIV_Icup(aes_all_at_once_gcm_t* p_param);
static void d_AES_GetYr(void);
static void d_AES_GetIv_Dtf(uint32_t* p_iv, uint32_t iv_size_in_bits);
static uint32_t d_AES_SetB0_Dtf(aes_all_at_once_ccm_t* p_param, uint32_t* p_B0);
static uint32_t d_AES_GetTagCcm_Dtf(uint32_t* p_auth_tag, uint32_t auth_tag_size_in_bits);
static inline void d_AES_IsrDtfEnd(void);
static inline void d_AES_IsrDtfEndSubCmac(void);
static inline void d_AES_IsrDtfEndSubXts(void);
static inline void d_AES_IsrDtfEndSubGcm(void);
static inline void d_AES_IsrDtfEndSubCcmEnc(void);
static inline void d_AES_IsrDtfEndSubCcmDec(void);
#if defined(TARGET_DEVICE_S4H) || defined(TARGET_DEVICE_U2B) || defined(TARGET_DEVICE_U2C) || defined(TARGET_DEVICE_U2C4)
static inline void d_AES_IsrDtfErr(void);
#endif

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0108]:[ICUMFW_UD_S4_D_AES_0108]
* Function name: d_AES_ConvertAddress
* Description  : Convert logical address to physical address.
* Arguments    : (IN)const uint32_t insrc
*                (IN)const uint32_t indest
*                (IN)const uint32_t inauth
*                (OUT)uint32_t *p_outsrc
*                (OUT)uint32_t *p_outdest
*                (OUT)uint32_t *p_outauth
*                (IN)const uint16_t mode
* Return Value : ER_OK
***************************************************************************************/
static uint32_t d_AES_ConvertAddress(
    const uint32_t insrc, const uint32_t indest, const uint32_t inauth,
    uint32_t *p_outsrc, uint32_t *p_outdest, uint32_t *p_outauth,
    const uint16_t mode)
{

    switch ((uint32_t)mode)
    {
        case D_AES_CIPHER_MODE_CMAC:
            (void)D_REMAP_GetPhysAddrs(insrc, p_outsrc);
            *p_outdest = indest;
            break;

        case D_AES_CIPHER_MODE_GCM:
        case D_AES_CIPHER_MODE_CCM:
            (void)D_REMAP_GetPhysAddrs(insrc, p_outsrc);
            if (D_AES_ZERO_U32 != indest)
            {
                (void)D_REMAP_GetPhysAddrs(indest, p_outdest);
            }
            else
            {
                /* do nothing */
            }

            if (D_AES_ZERO_U32 != inauth)
            {
                (void)D_REMAP_GetPhysAddrs(inauth, p_outauth);
            }
            else
            {
                /* do nothing */
            }
            break;

        default:
            (void)D_REMAP_GetPhysAddrs(insrc, p_outsrc);
            (void)D_REMAP_GetPhysAddrs(indest, p_outdest);
            break;
    }

    return ER_OK;
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0096]:[ICUMFW_UD_S4_D_AES_0096]
* Function name: d_AES_SetParamCcm
* Description  : Parameter setting for DTF (for CCM)
* Arguments    : cipher_mode          : Encryption mode
*                                              D_AES_CIPHER_MODE_CCM
*                direction            : cipher direction
*                                       Range: D_AES_DIR_ENCRYPTION
*                                              D_AES_DIR_DECRYPTION
* Return Value : none
***************************************************************************************/
static void d_AES_SetParamCcm(uint8_t direction)
{
    g_ctx2_state = USE_CTX2;
    /* Set ICUM_AESCTRL register */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    UNLOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
#endif
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(RST2)) |
                     /* Set 1 : Initialize KEY_2, IV_CTR_2, GHASH_2, SubKEY_2 */
                      (PRM_RST2_INIT);
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    D_AES_WAIT_REG_VALUE(PRM_RST2_INIT, ICUM_AESCTRL, RST2);
    LOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
#endif

    /* Set ICUM_AESCCTL1 register */
    ICUM_AESCCTL1 = (ICUM_AESCCTL1 & ~(K2SEL | MOD2)) |
                     /* Set 000 : AES0.DIDi [i = 0 to 3] is set */
                      (PRM_K2SEL_PLAIN
                     /* context #2. CCM mode */
                     | PRM_MOD2_CCM);

    if (D_AES_DIR_ENCRYPTION == (uint32_t)direction)
    {
        ICUM_AESCCTL1 = (ICUM_AESCCTL1 & ~(ENCMODE2)) |
                         /* Set 0 : ENCRYPTION MODE */
                          (PRM_ENCMODE2_ENC);
    }
    else
    {
        ICUM_AESCCTL1 = (ICUM_AESCCTL1 & ~(ENCMODE2)) |
                         /* Set 1 : DECRYPTION MODE */
                          (PRM_ENCMODE2_DEC);
    }

    /* Set ICUM_AESCTRL register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(CTX | BYTE_ORDER | DATREGSW)) |
                     /* Set 010 : context #2 */
                      (PRM_CTX_2
                     /* Set 0 : Convert byte order */
                     | PRM_BYTE_ORDER_APPLY
                     /* Set 1 : Use AES0DIDi/AES0DODi */
                     | PRM_DATREGSW_DIDI_DODI);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0095]:[ICUMFW_UD_S4_D_AES_0095]
* Function name: d_AES_SetParamOtherCcm
* Description  : Parameter setting for DTF (for other CCM)
* Arguments    : cipher_mode          : Encryption mode
*                                       Range: D_AES_CIPHER_MODE_ECB
*                                              D_AES_CIPHER_MODE_CBC
*                                              D_AES_CIPHER_MODE_CTR
*                                              D_AES_CIPHER_MODE_CFB
*                                              D_AES_CIPHER_MODE_OFB
*                                              D_AES_CIPHER_MODE_CMAC
*                                              D_AES_CIPHER_MODE_GCM
*                direction            : cipher direction
*                                       Range: D_AES_DIR_ENCRYPTION
*                                              D_AES_DIR_DECRYPTION
* Return Value : none
***************************************************************************************/
static void d_AES_SetParamOtherCcm(uint8_t cipher_mode, uint8_t direction)
{
    /* Set ICUM_AESCTRL register */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    UNLOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
#endif
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(RST0)) |
                      /* Set 1 : Initialize KEY_0, IV_CTR_0, GHASH_0, SubKEY_0 */
                      (PRM_RST0_INIT);
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    D_AES_WAIT_REG_VALUE(PRM_RST0_INIT, ICUM_AESCTRL, RST0);
    LOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
#endif

    /* Set ICUM_AESCCTL0 register */
    ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(K0SEL)) |
                      /* Set 000 : AES0.DIDi [i = 0 to 3] is set */
                      (PRM_K0SEL_PLAIN);

    if (D_AES_DIR_ENCRYPTION == (uint32_t)direction)
    {
        ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(ENCMODE0)) |
                          /* Set 0 : ENCRYPTION MODE */
                          (PRM_ENCMODE0_ENC);
    }
    else
    {
        ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(ENCMODE0)) |
                          /* Set 1 : DECRYPTION MODE */
                          (PRM_ENCMODE0_DEC);
    }
    switch ((uint32_t)cipher_mode)
    {
        case D_AES_CIPHER_MODE_ECB:
            ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(MOD0)) |
                              /* context #0. ECB mode */
                              (PRM_MOD0_ECB);
            break;
        case D_AES_CIPHER_MODE_CBC:
            ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(MOD0)) |
                              /* context #0. CBC mode */
                              (PRM_MOD0_CBC);
            break;
        case D_AES_CIPHER_MODE_CTR:
            ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(MOD0)) |
                              /* context #0. CTR mode */
                              (PRM_MOD0_CTR);
            break;
        case D_AES_CIPHER_MODE_CFB:
            ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(MOD0)) |
                              /* context #0. CBF mode */
                              (PRM_MOD0_CBF_128);
            break;
        case D_AES_CIPHER_MODE_OFB:
            ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(MOD0)) |
                              /* context #0. OFB mode */
                              (PRM_MOD0_OFB);
            break;
        case D_AES_CIPHER_MODE_CMAC:
            ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(MOD0)) |
                              /* context #0. CMAC mode */
                              (PRM_MOD0_CMAC);
            break;
        case D_AES_CIPHER_MODE_GCM:
            ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(MOD0)) |
                              /* context #0. GCM mode */
                              (PRM_MOD0_GCM);
            break;
        case D_AES_CIPHER_MODE_XTS:
            ICUM_AESCCTL0 = (ICUM_AESCCTL0 & ~(MOD0)) |
                              /* context #0. XTS mode */
                              (PRM_MOD0_XTS);
            break;
        default:
            /* Do Nothing */
            break;
    } /* end of switch (p_param->cipher_mode) */

    /* Settings for key value input */
    /* Set ICUM_AESCTRL register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(CTX | BYTE_ORDER | DATREGSW)) |
                      /* Set 000 : context #0 */
                      (PRM_CTX_0
                      /* Set 0 : Convert byte order */
                     | PRM_BYTE_ORDER_APPLY
                      /* Set 1 : Use AES0DIDi/AES0DODi */
                     | PRM_DATREGSW_DIDI_DODI);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0076]:[ICUMFW_UD_S4_D_AES_0076]
* Function name: d_AES_SetParamDtf
* Description  : Parameter setting for DTF
* Arguments    : cipher_mode          : Encryption mode
*                                       Range: D_AES_CIPHER_MODE_ECB
*                                              D_AES_CIPHER_MODE_CBC
*                                              D_AES_CIPHER_MODE_CTR
*                                              D_AES_CIPHER_MODE_CFB
*                                              D_AES_CIPHER_MODE_OFB
*                                              D_AES_CIPHER_MODE_CMAC
*                                              D_AES_CIPHER_MODE_GCM
*                                              D_AES_CIPHER_MODE_CCM
*                direction            : cipher direction
*                                       Range: D_AES_DIR_ENCRYPTION
*                                              D_AES_DIR_DECRYPTION
*                key_type             : key_type
*                                       Range: D_AES_KEYTYPE_PLAIN
*                                              D_AES_KEYTYPE_ROMKEY_0
*                p_aes_key            : address of key value
*                key_length           : key length
*                                       Range: D_AES_KEY_SIZE_IN_BYTES_128BIT
*                                              D_AES_KEY_SIZE_IN_BYTES_192BIT
*                                              D_AES_KEY_SIZE_IN_BYTES_256BIT
* Return Value : ER_OK
***************************************************************************************/
static uint32_t d_AES_SetParamDtf(aes_setparam_t *p_param)
{
    uint32_t keysel;
    uint32_t saved_psw;
    uint32_t i;
    uint32_t index;
    volatile uint32_t *p_key_reg[D_AES_KEY_NUM];

    keysel = D_AES_ZERO_U32;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    g_aes_dtf_flag = FLG_ON;

    /* Set ICUM_AESCTRL register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(DISTDTF));

    /* Key length setting */
    if (D_AES_KEY_SIZE_IN_BYTES_128BIT == (uint32_t)p_param->key_length)
    {
        /* 128bit */
        keysel = D_AES_SET_KEYSEL_0;
    }
    else if (D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)p_param->key_length)
    {
        /* 192bit */
        keysel = D_AES_SET_KEYSEL_1;
    }
    else
    {
        /* 256bit */
        keysel = D_AES_SET_KEYSEL_2;
    }

    ICUM_AESKEY = (ICUM_AESKEY & ~(KEYSEL)) |
                    /* Set 00 or 01 or 10 : key length */
                     keysel;

    /* Set processing according to Context */
    /* For CCM */
    if (D_AES_CIPHER_MODE_CCM == (uint32_t)p_param->cipher_mode)
    {
        (void)d_AES_SetParamCcm(p_param->direction);
    }
    /* Other than CCM */
    else
    {
        (void)d_AES_SetParamOtherCcm(p_param->cipher_mode, p_param->direction);
    } /* end of else */

    /* Branch by key_type */
    if (D_AES_KEYTYPE_ROMKEY_0 != (uint32_t)p_param->key_type)
    {
        /* Enter key value */
        /* Set ICUM_AESCTRL register */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) |
                        /* Set 01 : Key register KEY 1 */
                        (PRM_IDATDEST_KEY);

        /* EX_KEY 0: Set key bit [127:0] */
        ICUM_AESKEY = (ICUM_AESKEY & ~(EX_KEY)) |
                        /* Set 0 : Set key bit [127:0] */
                        (PRM_EX_KEY_SET_127_0);

       /* Start at a random index */
        D_OST_GetHardCounter(&index);
        index %= D_AES_KEY_NUM;

        /* Set registers for key input */
        *(p_key_reg + ADDR_0TH) = (volatile uint32_t *)&ICUM_AESDID0;
        *(p_key_reg + ADDR_1ST) = (volatile uint32_t *)&ICUM_AESDID1;
        *(p_key_reg + ADDR_2ND) = (volatile uint32_t *)&ICUM_AESDID2;
        *(p_key_reg + ADDR_3RD) = (volatile uint32_t *)&ICUM_AESDID3;

        /* Set key from a random index */
        for (i = D_AES_ZERO_U32; i < D_AES_KEY_NUM; i++)
        {
            *(p_key_reg[index]) = p_param->p_aes_key[index];
            index = (index + 1U) % D_AES_KEY_NUM;
        }

        if ((D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)p_param->key_length) ||
            (D_AES_KEY_SIZE_IN_BYTES_256BIT == (uint32_t)p_param->key_length))
        {
            /* EX_KEY 1: Set key bit [191:128] or [256:128] */
            ICUM_AESKEY = (ICUM_AESKEY & ~(EX_KEY)) |
                            /* Set 1 : Set key bit [191:128] or [256:128] */
                            (PRM_EX_KEY_SET_256_128);
            
            if (D_AES_KEY_SIZE_IN_BYTES_192BIT == (uint32_t)p_param->key_length)
            {
                ICUM_AESDID0 = *(p_param->p_aes_key + ADDR_4TH);
                ICUM_AESDID1 = *(p_param->p_aes_key + ADDR_5TH);
            }
            else
            {
                ICUM_AESDID0 = *(p_param->p_aes_key + ADDR_4TH);
                ICUM_AESDID1 = *(p_param->p_aes_key + ADDR_5TH);
                ICUM_AESDID2 = *(p_param->p_aes_key + ADDR_6TH);
                ICUM_AESDID3 = *(p_param->p_aes_key + ADDR_7TH);
            }
        } /* end of if ((D_AES_KEY_SIZE_IN_BYTES_192BIT == p_param->key_length) || */
        else
        {
            /* Do Nothing */
        }
    } /* end of if (D_AES_KEYTYPE_ROMKEY_0 != p_param->key_type) */
    else
    {
        /* Do Nothing */
    }

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return ER_OK;
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0077]:[ICUMFW_UD_S4_D_AES_0077]
* Function name: d_AES_TransferDtf
* Description  : DTF execution
* Arguments    : cipher_mode          : Encryption mode
*                                       Range: D_AES_CIPHER_MODE_ECB
*                                              D_AES_CIPHER_MODE_CBC
*                                              D_AES_CIPHER_MODE_CTR
*                                              D_AES_CIPHER_MODE_CFB
*                                              D_AES_CIPHER_MODE_OFB
*                                              D_AES_CIPHER_MODE_CMAC
*                                              D_AES_CIPHER_MODE_GCM
*                                              D_AES_CIPHER_MODE_CCM
*                direction            : cipher direction
*                                       Range: D_AES_DIR_ENCRYPTION
*                                              D_AES_DIR_DECRYPTION
*                number               : Number of blocks to be encrypted
*                                       Range: 0x0 - 0xFFFFFFFF
*                p_aes_src            : address of input value
*                p_aes_dest           : address of output value
*                async                : sync/async transfer mode
*                                       Range: D_AES_TRANS_SYNC
*                                              D_AES_TRANS_ASYNC
*                subroutine           : subroutine
*                                       Range: D_AES_DTF_XTS_ECDC1
*                                              D_AES_DTF_XTS_ECDC2
*                                              D_AES_DTF_XTS_ECDC3
*                                              D_AES_DTF_XTS_DEC
*                                              D_AES_DTF_GCM_IV
*               last_block_in_bits   : last block in bits
*                                      Range: D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
* Return Value : ER_OK
*                ER_D_AES_SEQUENCE_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
static uint32_t d_AES_TransferDtf(aes_transfer_dtf_t *p_param)
{
    uint32_t err;
    uint32_t skip_flg;

    err = ER_OK;
    skip_flg = FLG_OFF;

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    ICUM_AESDMASRCTRBLK = p_param->number - 1U;
    if ((D_AES_CIPHER_MODE_CMAC == (uint32_t)g_aes_dtf_info.cipher_mode)
        || ((D_AES_CIPHER_MODE_GCM == (uint32_t)g_aes_dtf_info.cipher_mode) && (NULL == p_param->p_aes_dest))
        || ((D_AES_CIPHER_MODE_CCM == (uint32_t)g_aes_dtf_info.cipher_mode) && (NULL == p_param->p_aes_dest)))
    {
        ICUM_AESDMADSTTRBLK = 0U;
    }
    else
    {
        ICUM_AESDMADSTTRBLK = p_param->number - 1U;
    }
    /* Initial address to transfer RAM data to AES via DTF transfer */
    /* SET : source address */
    ICUM_AESDMASRCAD = (DMASRCAD_MASK & (uint32_t)(p_param->p_aes_src));

    /* Initial address to transfer the calculation result from AES to RAM via DTF transfer */
    /* SET : destination address */
    ICUM_AESDMADSTAD = (DMADSTAD_MASK & (uint32_t)(p_param->p_aes_dest));

    ICUM_AESDMASRCCNT |= DMA_DME;
    ICUM_AESDMADSTCNT |= DMA_DME;
#else
    /* Synchronous reset of DTF control and DTF transfer I/F */
    /* Set ICUM_AESCTRL register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(RSTDTF)) |
                     /* Set 1 : DTF RSTDTF soft reset bit */
                     (RSTDTF_RESET);
#endif

    /* Settings for AES DTF transfer */
    /* A number of blocks to perform AES operation using the DTF transfer */
    /* SET : cipher block count */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    ICUM_AESDTFTRB = p_param->number;

    if ((D_AES_CIPHER_MODE_CCM == (uint32_t)g_aes_dtf_info.cipher_mode)
        || ((D_AES_CIPHER_MODE_GCM == (uint32_t)g_aes_dtf_info.cipher_mode)
            && ((uint32_t)D_AES_DTF_GCM_PC != (uint32_t)g_aes_dtf_info.subroutine))
        || ((D_AES_CIPHER_MODE_XTS == (uint32_t)g_aes_dtf_info.cipher_mode)
            && ((uint32_t)D_AES_DTF_XTS_ECDC3 != (uint32_t)g_aes_dtf_info.subroutine)))
    {
        ICUM_AESDMAMOD = (ICUM_AESDMAMOD & (~DMA_BO)) | DMA_LASTDIS;
    }
    else
    {
        ICUM_AESDMAMOD = (ICUM_AESDMAMOD & (~(DMA_BO | DMA_LASTDIS)));
    }

    /* Synchronous reset of DTF control and DTF transfer I/F */
    /* Set ICUM_AESCTRL register */
    UNLOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(RSTDTF)) |
                     /* Set 1 : DTF RSTDTF soft reset bit */
                     (RSTDTF_RESET);
    D_AES_WAIT_REG_VALUE(RSTDTF, ICUM_AESCTRL, RSTDTF);
    LOCK_REG_PROT(ICUMX_CTLREGICUMX_PERIREGPROT);
#else
    ICUM_AESDTFTRB = ( ICUM_AESDTFTRB & ~(DTF_TRBLK_RESET)) |
                        (uint32_t)(p_param->number);

    /* Initial address to transfer RAM data to AES via DTF transfer */
    /* SET : source address */
    ICUM_AESDTFIAD = ( ICUM_AESDTFIAD & ~(DTFIAD_RESET)) |
                        (uint32_t)(p_param->p_aes_src);

    /* Initial address to transfer the calculation result from AES to RAM via DTF transfer */
    /* SET : destination address */
    ICUM_AESDTFOAD = ( ICUM_AESDTFOAD & ~(DTFOAD_RESET)) |
                        (uint32_t)(p_param->p_aes_dest);
#endif

    /* Check the status of AES DTF */
    /* DTF run bit == 0? */
    if (D_AES_ZERO_U32 == (ICUM_AESSTR & PRM_DTF_RUN))
    {
        /* Do Nothing */
    }
    else
    {
        err = ER_D_AES_SEQUENCE_ERROR;
        skip_flg = FLG_ON;
    }

    if (FLG_OFF == skip_flg)
    {
        /* Start AES DTF transfer */
        /* 1: Start DTF transfer */
        ICUM_AESDTFSTT = PRM_DTF_START;

        /* For synchronization */
        if (D_AES_TRANS_SYNC == (uint32_t)p_param->async)
        {
            do
            {
                /* DTF run bit != 0? */
                if (D_AES_ZERO_U32 != (ICUM_AESSTR & PRM_DTF_RUN))
                {
                     break;
                }
                else
                {
                    /* Do Nothing */
                }
            } while (1);

            do
            {
                /* DTF run bit == 0? */
                if (D_AES_ZERO_U32 == (ICUM_AESSTR & PRM_DTF_RUN))
                {
                     break;
                }
                else
                {
                    /* Do Nothing */
                }
            } while (1);
        } /* end of if (D_AES_TRANS_SYNC == p_param->async) */
        /* For Asynchronous */
        else
        {
            err = ER_IN_PROCESS;
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do Nothing */
    }
    return err;
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0011]:[ICUMFW_UD_S4_D_AES_0011]
* Function name: d_AES_ExecutionAtOnceDtf
* Description  : Execute encryption of crypto AES at once. (for DTF)
* Arguments    : cipher_mode        : cipher mode
*                                     Range: D_AES_CIPHER_MODE_ECB
*                                            D_AES_CIPHER_MODE_CBC
*                                            D_AES_CIPHER_MODE_CTR
*                                            D_AES_CIPHER_MODE_CFB
*                                            D_AES_CIPHER_MODE_OFB
*                                            D_AES_CIPHER_MODE_CMAC
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
*                ER_D_AES_SEQUENCE_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
uint32_t d_AES_ExecutionAtOnceDtf(aes_all_at_once_t* p_param)
{
    uint32_t err;
    aes_setparam_t setparam;
    aes_nblocks_t nblocks;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    uint32_t aes_phys_src;
    uint32_t aes_phys_dest;
#endif
    /* Set local variable and 0 initialization */
    err = ER_OK;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    aes_phys_src = D_AES_ZERO_U32;
    aes_phys_dest = D_AES_ZERO_U32;
#endif
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_setparam_t));
    L_Toolbox_MemSet((uint8_t *)&nblocks, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_nblocks_t));

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    /* Set convert physical address */
    (void)d_AES_ConvertAddress((uint32_t)p_param->p_aes_src,
                               (uint32_t)p_param->p_aes_dest,
                               D_AES_ZERO_U32,
                               (uint32_t *)&aes_phys_src,
                               (uint32_t *)&aes_phys_dest,
                               NULL,
                               (uint16_t)p_param->cipher_mode);
#endif
    /* Set parameters (g_aes_dtf_info) */
    g_aes_dtf_info.cipher_mode            = p_param->cipher_mode;
    g_aes_dtf_info.direction              = p_param->direction;
    g_aes_dtf_info.number                 = p_param->number;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    g_aes_dtf_info.p_aes_src              = (uint32_t *)aes_phys_src;
    g_aes_dtf_info.p_aes_dest             = (uint32_t *)aes_phys_dest;
#else
    g_aes_dtf_info.p_aes_src              = p_param->p_aes_src;
    g_aes_dtf_info.p_aes_dest             = p_param->p_aes_dest;
#endif
    g_aes_dtf_info.async                  = p_param->async;
    g_aes_dtf_info.last_block_in_bits     = p_param->last_block_in_bits;
    g_aes_dtf_info.key_length             = p_param->key_length;

    /* Set parameters (setparam) */
    setparam.cipher_mode = p_param->cipher_mode;
    setparam.direction   = p_param->direction;
    setparam.key_type    = p_param->key_type;
    setparam.p_aes_key   = p_param->p_aes_key;
    setparam.key_length  = p_param->key_length;

    /* Create a parameter group for cryptographic processing. */
    nblocks.number = p_param->number;
    nblocks.block_info = p_param->last_block_in_bits;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    nblocks.p_aes_src = (uint32_t *)aes_phys_src;
    nblocks.p_aes_dest = (uint32_t *)aes_phys_dest;
#else
    nblocks.p_aes_src = p_param->p_aes_src;
    nblocks.p_aes_dest = p_param->p_aes_dest;
#endif

    /* Initialization of encryption and key operation */
    (void)d_AES_SetParamDtf(&setparam);

    /* CMAC mode? */
    if (D_AES_CIPHER_MODE_CMAC == (uint32_t)p_param->cipher_mode)
    {
        err = d_AES_CmacDtf(&nblocks, p_param->async);
    }
    else
    {
        err = d_AES_ProcessNBlocksDtf(&nblocks, p_param->cipher_mode, p_param->p_aes_iv, p_param->async);
    }
    if (ER_IN_PROCESS == err)
    {
        /* do nothing */
    }
    else
    {
        g_aes_dtf_flag = FLG_OFF;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0012]:[ICUMFW_UD_S4_D_AES_0012]
* Function name: d_AES_CmacDtf
* Description  : Performs AES-CMAC encryption processing collectively. (for DTF)
* Arguments    : number     : Number of blocks to be encrypted
*                             Range: 0x0 - 0xFFFFFFFF
*                block_info : block information
*                             Range: D_AES_BLOCK_INFO_FIRST
*                                    D_AES_BLOCK_INFO_LAST
*                                    D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_aes_src  : address of input value
*                p_aes_dest : address of output value
*                async      : sync/async transfer mode
*                             Range: D_AES_TRANS_SYNC
*                                    D_AES_TRANS_ASYNC
* Return Value : ER_OK
*                ER_D_AES_SEQUENCE_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
static uint32_t d_AES_CmacDtf(aes_nblocks_t* p_param, uint8_t async)
{
    uint32_t err;
    uint32_t saved_psw;
    aes_transfer_dtf_t transfer_dtf;
    /* Set local variable and 0 initialization */
    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;
    L_Toolbox_MemSet((uint8_t *)&transfer_dtf, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_transfer_dtf_t));

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Set ICUM_AESCTRL register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST | DISTDTF)) |
                     /* Set 00: Block Cipher Output register (BCO) */
                     ( PRM_IDATDEST_BCI
                     /* Set 1: Do not transfer the tag data to RAM */
                     | PRM_DISTDTF );

    /* Register setting */
    /* Set ICUM_AESDCTL register */
    ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | REM | IVSEL | NEW_KEY)) |
                     /* Set 0000 : The number of valid bits in the input data is not 0 */
                     ( PRM_ATTR_CMAC_NO_ZERO
                     /* Set : block_size */
                     | (REM & ((uint32_t)p_param->block_info << SHIFT_REM))
                     /* Set 1 : Use the value set in IV_CTR_i */
                     | PRM_IVSEL_USE
                     /* Set 1 : Use the data set in the key register (KEY_i) */
                     | PRM_NEW_KEY_USE );

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Set parameters (transfer_dtf) */
#if defined(TARGET_DEVICE_U2B)
    (void)D_FLS_ChangeNoRemapAddr((uint32_t)p_param->p_aes_src, (uint32_t**)&transfer_dtf.p_aes_src);
#else
    transfer_dtf.p_aes_src = p_param->p_aes_src;
#endif /* defined(TARGET_DEVICE_U2B) */
    transfer_dtf.number     = p_param->number;
    transfer_dtf.p_aes_dest = p_param->p_aes_dest;
    transfer_dtf.async      = async;

    /* Execute AES (DTF transfer) */
    err = d_AES_TransferDtf(&transfer_dtf);


    if ((ER_OK == err) && (D_AES_TRANS_SYNC == (uint32_t)async))
    {
        /* Disable EI Level Maskable Interrupt */
        DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

        /* Output data source selection bit */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC)) |
                         /* Set 01 : Block cipher output register (BCO_DTF) */
                          (PRM_ODATSRC_BCO_DTF);

        /* Read from ICUM_AESDODi register */
        *(p_param->p_aes_dest + ADDR_0TH) = ICUM_AESDOD0;
        *(p_param->p_aes_dest + ADDR_1ST) = ICUM_AESDOD1;
        *(p_param->p_aes_dest + ADDR_2ND) = ICUM_AESDOD2;
        *(p_param->p_aes_dest + ADDR_3RD) = ICUM_AESDOD3;

        /* Enable EI Level Maskable Interrupt */
        ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    }
    else
    {
        /* do nothing */
    }
    return err;
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0013]:[ICUMFW_UD_S4_D_AES_0013]
* Function name: d_AES_ProcessNBlocksDtf
* Description  : Perform AES operation in each mode of ECB, CBC, OFB, CFB, CTR (for DTF)
* Arguments    : number      : Number of blocks to be encrypted
*                              Range: 0x0 - 0xFFFFFFFF
*                block_info  : block information
*                              Range: D_AES_BLOCK_INFO_FIRST
*                                     D_AES_BLOCK_INFO_LAST
*                                     D_AES_BLOCK_INFO_SIZE_MIN - D_AES_BLOCK_INFO_SIZE_MAX
*                p_aes_src   : address of input value
*                p_aes_dest  : address of output value
*                cipher_mode : Encryption mode
*                              Range: D_AES_CIPHER_MODE_ECB
*                                     D_AES_CIPHER_MODE_CBC
*                                     D_AES_CIPHER_MODE_CTR
*                                     D_AES_CIPHER_MODE_CFB
*                                     D_AES_CIPHER_MODE_OFB
*                p_aes_iv    : address of IV value
*                async       : sync/async transfer mode
*                              Range: D_AES_TRANS_SYNC
*                                     D_AES_TRANS_ASYNC
* Return Value : ER_OK
*                ER_D_AES_SEQUENCE_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
static uint32_t d_AES_ProcessNBlocksDtf(aes_nblocks_t* p_param, uint8_t cipher_mode, uint32_t* p_aes_iv, uint8_t async)
{
    uint32_t err;
    uint32_t saved_psw;
    aes_transfer_dtf_t transfer_dtf;
    /* Set local variable and 0 initialization */
    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;
    L_Toolbox_MemSet((uint8_t *)&transfer_dtf, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_transfer_dtf_t));

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Other ECB mode? */
    if (D_AES_CIPHER_MODE_ECB != (uint32_t)cipher_mode)
    {
        /* Set ICUM_AESCTRL register */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) |
                         /* Set 10 : Initial vector */
                         (PRM_IDATDEST_IV_CTR);

        /* Set initialization vector to AES data input register */
        ICUM_AESDID0 = *(p_aes_iv + ADDR_0TH);
        ICUM_AESDID1 = *(p_aes_iv + ADDR_1ST);
        ICUM_AESDID2 = *(p_aes_iv + ADDR_2ND);
        ICUM_AESDID3 = *(p_aes_iv + ADDR_3RD);
    }
    else
    {
        /* Do Nothing */
    }

    /* Register setting */
    /* Set ICUM_AESDCTL register */
    ICUM_AESDCTL = (ICUM_AESDCTL & ~(IVSEL | NEW_KEY)) |
                     /* Set 1 : Use the value set in IV_CTR_i */
                     ( PRM_IVSEL_USE
                     /* Set 1 : Use the data set in the key register (KEY_i) */
                     | PRM_NEW_KEY_USE);

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Set parameters (transfer_dtf) */
    transfer_dtf.number     = p_param->number;
    transfer_dtf.p_aes_src  = p_param->p_aes_src;
    transfer_dtf.p_aes_dest = p_param->p_aes_dest;
    transfer_dtf.async      = async;

    /* AES DTF processing */
    /* Execute AES (DTF transfer) */
    err = d_AES_TransferDtf(&transfer_dtf);

    return err;
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0098]:[ICUMFW_UD_S4_D_AES_0098]
* Function name: d_AES_GetTagXts
* Description  : XTS mode tag value output
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
*                p_aes_dest         : address of output value
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_GetTagXts(aes_all_at_once_xts_t* p_param)
{
    uint32_t err;
    uint32_t saved_psw;
    uint32_t output[AES_BLOCK_SIZE];
    uint32_t input[AES_BLOCK_SIZE];

    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;
    L_Toolbox_MemSet((uint8_t *)output, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(output));
    L_Toolbox_MemSet((uint8_t *)input, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(input));

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    g_aes_dtf_info.subroutine = D_AES_DTF_XTS_ECDC3;

    /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) |
                    (PRM_ODATSRC_BCO
                   | PRM_IDATDEST_BCI);

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_param->p_aes_src, (uint32_t *)&p_param->p_aes_src);
    (void)D_REMAP_GetLogicalAddrs((uint32_t)p_param->p_aes_dest, (uint32_t *)&p_param->p_aes_dest);
#endif

    if ((uint32_t)D_AES_SET128 == (uint32_t)p_param->last_block_in_bits)
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM )) |
                        /* Set 0000 : Plain text */
                          (PRM_ATTR_XTS_TEXT
                        /* Set 1 : Last block */
                         | PRM_LAST_BLOCK
                        /* Set : Number of bits in final block */
                         | PRM_REM_ZERO);

        err = d_AES_Cipher(p_param->p_aes_src, (uint32_t*)&output);
    }
    else
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM )) |
                        /* Set 0000 : Plain text */
                          (PRM_ATTR_XTS_TEXT
                        /* Set 1 : Last block */
                         | PRM_LAST_BLOCK
                        /* Set : Number of bits in final block */
                         | (REM & ((uint32_t)&p_param->last_block_in_bits << SHIFT_REM)));

        /* calculate last block size */
        (void)d_AES_LastBlock(input, p_param->p_aes_src, p_param->last_block_in_bits);

        err = d_AES_Cipher((const uint32_t*)input, (uint32_t*)output);
    }

    if ((uint32_t)D_AES_SET128 == (uint32_t)p_param->last_block_in_bits)
    {
        (void)L_Toolbox_MemCpy((uint8_t *)p_param->p_aes_dest,
                               (const uint8_t *)output,
                               (int16_t)AES_BLOCK_SIZE_IN_BYTES);
    }
    else
    {
        (void)d_AES_LastBlock(p_param->p_aes_dest,
                              output,
                              p_param->last_block_in_bits);
    }

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0097]:[ICUMFW_UD_S4_D_AES_0097]
* Function name: d_AES_AtOnceXtsDtf
* Description  : Execute encryption of crypto AES at once. (for XTS - DTF)
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
*                p_aes_dest         : address of output value
* Return Value : ER_OK
*                ER_D_AES_SEQUENCE_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
static uint32_t d_AES_AtOnceXtsDtf(aes_all_at_once_xts_t* p_param)
{
    uint32_t err;
    uint32_t skip_flg;
    uint32_t output[AES_BLOCK_SIZE];
    uint32_t input[AES_BLOCK_SIZE];
    aes_transfer_dtf_t transfer_dtf;

    err = ER_OK;
    skip_flg = FLG_OFF;
    L_Toolbox_MemSet((uint8_t *)&transfer_dtf, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_transfer_dtf_t));
    L_Toolbox_MemSet((uint8_t *)output, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(output));
    L_Toolbox_MemSet((uint8_t *)input, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(input));

    /* AES DTF processing of XTS_ECDC1 */
    /* number check */
    if ((uint32_t)D_AES_BLOCK_SIZE_3 <= p_param->number)
    {
        /* Set ICUM_AESDCTL register */
        /* XTS_ECDC1 Type:Enc ABTR:0000 DTF_TRBLK:blkcnt(C)-2 */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR)) |
                        /* Set 0000 : Plain text */
                         PRM_ATTR_XTS_TEXT;

        g_aes_dtf_info.subroutine = D_AES_DTF_XTS_ECDC1;

        transfer_dtf.number     = p_param->number - (uint32_t)D_AES_BLOCK_SIZE_2;
        transfer_dtf.p_aes_src  = p_param->p_aes_src;
        transfer_dtf.p_aes_dest = p_param->p_aes_dest;
        transfer_dtf.async      = p_param->async;

        /* Execute AES (DTF transfer) XTS_ECDC1 */
        err = d_AES_TransferDtf(&transfer_dtf);

        if ((D_AES_TRANS_ASYNC == (uint32_t)transfer_dtf.async) ||
            ((D_AES_TRANS_SYNC == (uint32_t)transfer_dtf.async) && (ER_OK != err)))
        {
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do Nothing */
        }
    } /* end of if ((uint32_t)D_AES_BLOCK_SIZE_3 <= p_param->number) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* AES DTF processing of XTS_ECDC2 */
        if ((((uint32_t)D_AES_BLOCK_SIZE_2 <= p_param->number) &&
                 ((uint32_t)D_AES_SET128 == (uint32_t)p_param->last_block_in_bits)) ||
                (((uint32_t)D_AES_BLOCK_SIZE_2 <= p_param->number) &&
                 ((uint32_t)D_AES_SET128 != (uint32_t)p_param->last_block_in_bits) &&
                 (D_AES_DIR_ENCRYPTION == (uint32_t)p_param->direction)))
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR)) |
                            /* Set 0000 : Plain text */
                             PRM_ATTR_XTS_TEXT;

            g_aes_dtf_info.subroutine = D_AES_DTF_XTS_ECDC2;

            transfer_dtf.number     = (uint32_t)D_AES_BLOCK_SIZE_1;
            transfer_dtf.p_aes_src  = p_param->p_aes_src;
            transfer_dtf.p_aes_dest = p_param->p_aes_dest;
            transfer_dtf.async      = p_param->async;

            /* Execute AES (DTF transfer) XTS_ECDC2 */
            err = d_AES_TransferDtf(&transfer_dtf);

            if ((D_AES_TRANS_ASYNC == (uint32_t)transfer_dtf.async) ||
                ((D_AES_TRANS_SYNC == (uint32_t)transfer_dtf.async) && (ER_OK != err)))
            {
               skip_flg = FLG_ON;
            }
            else
            {
                /* Do Nothing */
            }
        } /* end of if ((uint32_t)D_AES_BLOCK_SIZE_2 <= p_param->number)  */
        else
        {
            /* Do Nothing */
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* AES DTF processing of XTS_DEC */
        if (((uint32_t)D_AES_BLOCK_SIZE_2 <= p_param->number) &&
                ((uint32_t)D_AES_SET128 !=  (uint32_t)p_param->last_block_in_bits) &&
                (D_AES_DIR_DECRYPTION == (uint32_t)p_param->direction))
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~( ATTR | REM )) |
                            /* Set 10 : Next to the last block                                              *
                             * if a decryption operation is in progress and the last block is less than 127 */
                             PRM_ATTR_XTS_SHORT_LAST;

            g_aes_dtf_info.subroutine = D_AES_DTF_XTS_DEC;

            transfer_dtf.number     = (uint32_t)D_AES_BLOCK_SIZE_1;
            transfer_dtf.p_aes_src  = p_param->p_aes_src;
            transfer_dtf.p_aes_dest = p_param->p_aes_dest;
            transfer_dtf.async      = p_param->async;

            /* Execute AES (DTF transfer) XTS_DEC */
            err = d_AES_TransferDtf(&transfer_dtf);

            if ((D_AES_TRANS_ASYNC == (uint32_t)transfer_dtf.async) ||
                ((D_AES_TRANS_SYNC == (uint32_t)transfer_dtf.async) && (ER_OK != err)))
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do Nothing */
            }
        } /* end of if ((uint32_t)D_AES_BLOCK_SIZE_2 <= p_param->number) */
        else
        {
            /* Do Nothing */
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        err = d_AES_GetTagXts(p_param);
    }
    else
    {
        /* Do Nothing */
    }
    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0106]:[ICUMFW_UD_S4_D_AES_0106]
* Function name: d_AES_KeySetXts_Dtf
* Description  : Key setting. (for XTS - DTF)
* Arguments    : p_aes_key          : address of key value
*                key_length         : key length
* Return Value : none
***************************************************************************************/
static void d_AES_KeySetXts_Dtf(uint32_t* p_aes_key, uint16_t key_length)
{
    uint32_t i;
    uint32_t index;
    volatile uint32_t *p_key_reg[D_AES_KEY_NUM];

    /* Register setting */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) |
                      /* Set 01 : Key register KEY 1 */
                      (PRM_IDATDEST_KEY);

    /* Key setting */
    /* EX_KEY 0: Set key bit [127:0] */
    ICUM_AESKEY = (ICUM_AESKEY & ~(EX_KEY)) |
                     /* Set 0: key bit [127:0] */
                     (PRM_EX_KEY_SET_127_0);

   /* Start at a random index */
    D_OST_GetHardCounter(&index);
    index %= D_AES_KEY_NUM;

    /* Set registers for key input */
    *(p_key_reg + ADDR_0TH) = (volatile uint32_t *)&ICUM_AESDID0;
    *(p_key_reg + ADDR_1ST) = (volatile uint32_t *)&ICUM_AESDID1;
    *(p_key_reg + ADDR_2ND) = (volatile uint32_t *)&ICUM_AESDID2;
    *(p_key_reg + ADDR_3RD) = (volatile uint32_t *)&ICUM_AESDID3;

    /* Set key from a random index */
    for (i = D_AES_ZERO_U32; i < D_AES_KEY_NUM; i++)
    {
        *(p_key_reg[index]) = p_aes_key[index];
        index = (index + 1U) % D_AES_KEY_NUM;
    }

    if (D_AES_KEY_SIZE_IN_BYTES_256BIT == (uint32_t)key_length)
    {
        ICUM_AESKEY = (ICUM_AESKEY & ~(EX_KEY)) |
                         /* Set 0: key bit [256:128] */
                         (PRM_EX_KEY_SET_256_128);

        ICUM_AESDID0 = *(p_aes_key + ADDR_4TH);
        ICUM_AESDID1 = *(p_aes_key + ADDR_5TH);
        ICUM_AESDID2 = *(p_aes_key + ADDR_6TH);
        ICUM_AESDID3 = *(p_aes_key + ADDR_7TH);
    }
    else
    {
        /* Do Nothing */
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0027]:[ICUMFW_UD_S4_D_AES_0027]
* Function name: d_AES_ExecutionAtOnceXtsDtf
* Description  : Execute encryption of crypto AES at once. (for XTS - DTF)
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
*                p_aes_dest         : address of output value
* Return Value : ER_OK
*                ER_D_AES_SEQUENCE_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
uint32_t d_AES_ExecutionAtOnceXtsDtf(aes_all_at_once_xts_t* p_param)
{
    uint32_t err;
    uint32_t saved_psw;
    uint32_t output[AES_BLOCK_SIZE];
    uint32_t input[AES_BLOCK_SIZE];
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    uint32_t aes_phys_src;
    uint32_t aes_phys_dest;
#endif
    aes_setparam_t setparam;
    aes_all_at_once_xts_t xtsparam;

    /* Set local variable and 0 initialization */
    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    aes_phys_src = D_AES_ZERO_U32;
    aes_phys_dest = D_AES_ZERO_U32;
#endif
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_setparam_t));
    L_Toolbox_MemSet((uint8_t *)&xtsparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_all_at_once_xts_t));
    L_Toolbox_MemSet((uint8_t *)output, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(output));
    L_Toolbox_MemSet((uint8_t *)input, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(input));

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    /* convert physical address */
    (void)d_AES_ConvertAddress((uint32_t)p_param->p_aes_src,
                               (uint32_t)p_param->p_aes_dest,
                               D_AES_ZERO_U32,
                               (uint32_t *)&aes_phys_src,
                               (uint32_t *)&aes_phys_dest,
                               NULL,
                               (uint16_t)D_AES_CIPHER_MODE_XTS);
#endif
    /* Set parameters (g_aes_dtf_info) */
    g_aes_dtf_info.cipher_mode            = (uint8_t)D_AES_CIPHER_MODE_XTS;
    g_aes_dtf_info.direction              = p_param->direction;
    g_aes_dtf_info.number                 = p_param->number;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    g_aes_dtf_info.p_aes_src              = (uint32_t *)aes_phys_src;
    g_aes_dtf_info.p_aes_dest             = (uint32_t *)aes_phys_dest;
#else
    g_aes_dtf_info.p_aes_src              = p_param->p_aes_src;
    g_aes_dtf_info.p_aes_dest             = p_param->p_aes_dest;
#endif
    g_aes_dtf_info.async                  = p_param->async;
    g_aes_dtf_info.last_block_in_bits     = p_param->last_block_in_bits;
    g_aes_dtf_info.key_length             = p_param->key_length;

    /* Set parameters (setparam) */
    setparam.cipher_mode = (uint8_t)D_AES_CIPHER_MODE_XTS;
    setparam.direction   = p_param->direction;
    setparam.key_type    = (uint8_t)D_AES_KEYTYPE_PLAIN;
    setparam.p_aes_key   = p_param->p_aes_key_tweak;
    setparam.key_length  = p_param->key_length;

    /* Initialization of encryption and key operation */
    (void)d_AES_SetParamDtf(&setparam);

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Register setting */
    /* Set ICUM_AESDCTL register */
    ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | IVSEL | NEW_KEY)) |
                    /* Set 0001: tweak value */
                     (PRM_ATTR_XTS_TWEAK
                    /* Set 0 : Reset last block register */
                    | PRM_NO_LAST_BLOCK
                    /* Set 1 : Use the value set in IV_CTR_i */
                    | PRM_IVSEL_USE
                    /* Set 1 : Use the data set in the key register (KEY_i) */
                    | PRM_NEW_KEY_USE);

    /* AES DTF processing of XTS_TW  */
    g_aes_dtf_info.subroutine = D_AES_DTF_XTS_TW;

    /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);

    err = d_AES_CipherDummyRead(p_param->p_aes_iv);

    if (ER_OK == err)
    {
        (void)d_AES_KeySetXts_Dtf(p_param->p_aes_key, p_param->key_length);

        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(REM | IVSEL | NEW_KEY)) |
                         /* Set 0000000 : 128bit */
                        ( PRM_REM_ZERO
                        /* Set 0 : Do not use the value set in IV_CTR_i */
                        | PRM_IVSEL_NO_USE
                        /* Set 1 : Use the data set in the key register (KEY_i) */
                        | PRM_NEW_KEY_USE);

        /* Enable EI Level Maskable Interrupt */
        ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

        /* Set parameters (xtsparam) */
        xtsparam = *p_param;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
        xtsparam.p_aes_src   = (uint32_t *)aes_phys_src;
        xtsparam.p_aes_dest  = (uint32_t *)aes_phys_dest;
#endif
        err = d_AES_AtOnceXtsDtf(&xtsparam);

    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Enable EI Level Maskable Interrupt */
        ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
    }
    if (ER_IN_PROCESS == err)
    {
        /* do nothing */
    }
    else
    {
        g_aes_dtf_flag = FLG_OFF;
    }

    return err;
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0100]:[ICUMFW_UD_S4_D_AES_0100]
* Function name: d_AES_GcmSetIV_
* Description  : J1 output at IV value 96
* Arguments    : p_aes_iv           : address of IV value
* Return Value : none
***************************************************************************************/
static void d_AES_GcmSetIV_NoIcup(uint32_t* p_iv)
{
    uint32_t ctr_j_lsb;
    uint32_t saved_psw;

    ctr_j_lsb = D_AES_ZERO_U32;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* g_aes_dtf_gcm_j0 = IV || 031 || 1B */
    ctr_j_lsb = D_AES_IV_MAKE_SET_1;
    L_Toolbox_MemCpy((uint8_t *)g_aes_dtf_gcm_j0, (const uint8_t *)p_iv, (int16_t)D_AES_SET_12BYTE);
    L_Toolbox_MemCpy((uint8_t *)(g_aes_dtf_gcm_j0 + ADDR_3RD), (const uint8_t *)&ctr_j_lsb, (int16_t)D_AES_SET_4BYTE);

    /* g_aes_dtf_gcm_j1 = g_aes_dtf_gcm_j0 + 1B */
    ctr_j_lsb = D_AES_IV_MAKE_SET_2;
    L_Toolbox_MemCpy((uint8_t *)g_aes_dtf_gcm_j1, (const uint8_t *)g_aes_dtf_gcm_j0, (int16_t)D_AES_SET_12BYTE);
    L_Toolbox_MemCpy((uint8_t *)(g_aes_dtf_gcm_j1 + ADDR_3RD), (const uint8_t *)&ctr_j_lsb, (int16_t)D_AES_SET_4BYTE);

    /* Set ICUM_AESCTRL register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) |
                     /* Set 10 : Initial vector */
                     (PRM_IDATDEST_IV_CTR);

    /* Set g_aes_dtf_gcm_j1 to AES data input register */
    ICUM_AESDID0 = *(g_aes_dtf_gcm_j1 + ADDR_0TH);
    ICUM_AESDID1 = *(g_aes_dtf_gcm_j1 + ADDR_1ST);
    ICUM_AESDID2 = *(g_aes_dtf_gcm_j1 + ADDR_2ND);
    ICUM_AESDID3 = *(g_aes_dtf_gcm_j1 + ADDR_3RD);

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0101]:[ICUMFW_UD_S4_D_AES_0101]
* Function name: d_AES_GcmSetIV_Icup
* Description  : J1 output other than IV value 96
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
*                async                  : sync/async transfer mode
*                                         Range: D_AES_TRANS_SYNC
*                                                D_AES_TRANS_ASYNC
*                p_aes_dest             : address of output value
*                p_auth_tag             : address of authentication tag
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_GcmSetIV_Icup(aes_all_at_once_gcm_t* p_param)
{
    uint32_t err;
    uint32_t skip_flg;
    uint32_t calc_cnt;
    uint32_t loop_count;
    uint32_t saved_psw;
    uint32_t parameter_iv[AES_BLOCK_SIZE];

    err = ER_OK;
    skip_flg = FLG_OFF;
    calc_cnt = D_AES_ZERO_U32;
    loop_count = D_AES_ZERO_U8;
    saved_psw = D_AES_ZERO_U32;
    L_Toolbox_MemSet((uint8_t *)parameter_iv, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(parameter_iv));

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Set ICUM_AESDCTL register */
    ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | NEW_KEY)) |
                     /* Set 0000 : GHASH function input data */
                      (PRM_ATTR_GCM_GHASH
                     /* Set 0 : No last block */
                     | PRM_NO_LAST_BLOCK
                     /* Set 0000000 : 128bit */
                     | PRM_REM_128BIT
                     /* Set 1 : Use the value set in IV_CTR_i */
                     | PRM_IVSEL_USE
                     /* Set 1 : Use the data set in the key register (KEY_i) */
                     | PRM_NEW_KEY_USE);

    calc_cnt = (((uint32_t)p_param->iv_size_in_bits + D_AES_SET127) / D_AES_SET128);

    for ( ; END_CALC != calc_cnt ; calc_cnt--)
    {
        if (D_AES_ZERO_U32 != loop_count)
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | NEW_KEY)) |
                             /* Set 0000 : GHASH function input data */
                              (PRM_ATTR_GCM_GHASH
                             /* Set 0 : No last block */
                             | PRM_NO_LAST_BLOCK
                             /* Set 0000000 : 128bit */
                             | PRM_REM_128BIT
                             /* Set 0 : Use the value set in IV_CTR_i */
                             | PRM_IVSEL_NO_USE
                             /* Set 0 : Use the data set in the key register (KEY_i) */
                             | PRM_NEW_KEY_NO_USE);
        }
        else
        {
            /* Do nothing */
        }

        /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);

        err = d_AES_CipherDummyRead(p_param->p_aes_iv);
        
        if (ER_OK == err)
        {
            p_param->p_aes_iv += AES_BLOCK_SIZE;
            loop_count++;
        }
        else
        {
            skip_flg = FLG_ON;
            break;
        }
    } /* end of for ( ; END_CALC != calc_cnt ; calc_cnt--) */

    if (FLG_OFF == skip_flg)
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | NEW_KEY)) |
                         /* Set 0000 : GHASH function input data */
                          (PRM_ATTR_GCM_GHASH
                         /* Set 0 : No last block */
                         | PRM_NO_LAST_BLOCK
                         /* Set 0000000 : 128bit */
                         | PRM_REM_128BIT
                         /* Set 0 : Do not use the value set in IV_CTR_i */
                         | PRM_IVSEL_NO_USE
                         /* Set 0 : Use the key used in the previous operation */
                         | PRM_NEW_KEY_NO_USE);

        /* Make iv */
        parameter_iv[ADDR_0TH] = ALL_ZERO;
        parameter_iv[ADDR_1ST] = ALL_ZERO;
        parameter_iv[ADDR_2ND] = ALL_ZERO;
        parameter_iv[ADDR_3RD] = BSW(p_param->iv_size_in_bits);

        /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);

        err = d_AES_Cipher((const uint32_t*)parameter_iv, (uint32_t*)g_aes_dtf_gcm_j0);

        if (ER_OK == err)
        {
            L_Toolbox_MemCpy((uint8_t *)g_aes_dtf_gcm_j1, (const uint8_t *)g_aes_dtf_gcm_j0, (int16_t)D_AES_SET_16BYTE);
            g_aes_dtf_gcm_j1[NUM3] = g_aes_dtf_gcm_j1[NUM3] + D_AES_IV_MAKE_SET_1;

            /* Set ICUM_AESCTRL register */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) |
                             /* Set 10 : Initial vector */
                             (PRM_IDATDEST_IV_CTR);

            /* Set g_aes_dtf_gcm_j1 to AES data input register */
            ICUM_AESDID0 = *(g_aes_dtf_gcm_j1 + ADDR_0TH);
            ICUM_AESDID1 = *(g_aes_dtf_gcm_j1 + ADDR_1ST);
            ICUM_AESDID2 = *(g_aes_dtf_gcm_j1 + ADDR_2ND);
            ICUM_AESDID3 = *(g_aes_dtf_gcm_j1 + ADDR_3RD);

        } /* end of if (FLG_ON == skip_flg) */
        else
        {
            /* Do Nothing */
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do Nothing */
    }

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0099]:[ICUMFW_UD_S4_D_AES_0099]
* Function name: d_AES_AtOnceGcmDtf
* Description  : Execute encryption of crypto AES at once. (for GCM - DTF)
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
*                async                  : sync/async transfer mode
*                                         Range: D_AES_TRANS_SYNC
*                                                D_AES_TRANS_ASYNC
*                p_aes_dest             : address of output value
*                p_auth_tag             : address of authentication tag
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
static uint32_t d_AES_AtOnceGcmDtf(aes_all_at_once_gcm_t* p_param)
{
    uint32_t err;
    uint32_t skip_flg;
    uint32_t saved_psw;
    uint32_t total_size[D_AES_SET128];
    uint32_t auth_tag[D_AES_SET_4];
    aes_transfer_dtf_t transfer_dtf;

    /* Set local variable and 0 initialization */
    err = ER_OK;
    skip_flg = FLG_OFF;
    saved_psw = D_AES_ZERO_U32;
    L_Toolbox_MemSet((uint8_t *)&transfer_dtf, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_transfer_dtf_t));
    L_Toolbox_MemSet((uint8_t*)total_size, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(total_size));
    L_Toolbox_MemSet((uint8_t *)auth_tag, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(auth_tag));

    /* Perform AES DTF processing of additional authenticated data (AAD) (GCM_AAD) */
    /* auth_data_size_in_bit != 0 */
    if (D_AES_ZERO_U32 != p_param->auth_data_size_in_bits)
    {
        g_aes_dtf_info.subroutine = D_AES_DTF_GCM_AAD;

        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | NEW_KEY)) |
                         /* Set 0000 : GHASH function input data */
                          (PRM_ATTR_GCM_GHASH
                         /* Set 0 : No last block */
                         | PRM_NO_LAST_BLOCK
                         /* Set 0000000 : 128bit */
                         | PRM_REM_128BIT
                         /* Set 1 : Use the value set in IV_CTR_i */
                         | PRM_IVSEL_USE
                         /* Set 1 : Use the data set in the key register (KEY_i) */
                         | PRM_NEW_KEY_USE);

        /* Set parameters (transfer_dtf) */
        transfer_dtf.number     = p_param->auth_data_size_in_bits / D_AES_SET128;
        transfer_dtf.p_aes_src  = p_param->p_auth_data;
        transfer_dtf.p_aes_dest = NULL;
        transfer_dtf.async      = p_param->async;

        /* Execute AES (DTF transfer) */
        err = d_AES_TransferDtf(&transfer_dtf);

        /* Judgment of execution result */
        if ((D_AES_TRANS_ASYNC == (uint32_t)transfer_dtf.async) ||
            ((D_AES_TRANS_SYNC == (uint32_t)transfer_dtf.async) && (ER_OK != err)))
        {
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do Nothing */
        }
    } /* end of if (D_AES_ZERO_U32 != p_param->auth_data_size_in_bits) */
    /* auth_data_size_in_bit = 0 */
    else
    {
        /* Do Nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* Perform AES DTF processing of plain text (GCM_PC) */
        /* data_size_in_bit != 0 */
        if (D_AES_ZERO_U32 != p_param->data_size_in_bits)
        {
            g_aes_dtf_info.subroutine = D_AES_DTF_GCM_PC;

            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | NEW_KEY)) |
                             /* Set 0000 : GHASH function input data */
                              (PRM_ATTR_GCM_GCTR
                             /* Set 0 : No last block */
                             | PRM_NO_LAST_BLOCK
                             /* Set 0000000 : 128bit */
                             | (REM & ((p_param->data_size_in_bits % D_AES_SET128) << SHIFT_REM))
                             /* Set 1 : Use the value set in IV_CTR_i */
                             | PRM_IVSEL_USE
                             /* Set 1 : Use the data set in the key register (KEY_i) */
                             | PRM_NEW_KEY_USE);

            /* Set parameters (transfer_dtf) */
            transfer_dtf.number     = p_param->data_size_in_bits / D_AES_SET128;
            transfer_dtf.p_aes_src  = p_param->p_aes_src;
            transfer_dtf.p_aes_dest = p_param->p_aes_dest;
            transfer_dtf.async      = p_param->async;

            /* Execute AES (DTF transfer) */
            err = d_AES_TransferDtf(&transfer_dtf);

            /* Judgment of execution result */
            if ((D_AES_TRANS_ASYNC == (uint32_t)transfer_dtf.async) ||
               ((D_AES_TRANS_SYNC == (uint32_t)transfer_dtf.async) && (ER_OK != err)))
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do Nothing */
            }
        } /* end of if (D_AES_ZERO_U32 != p_param->data_size_in_bits) */
        /* data_size_in_bit = 0 */
        else
        {
            /* Disable EI Level Maskable Interrupt */
            DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

            /* Set ICUM_AESCTRL register */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) |
                             /* Set 10 : Initial vector */
                             (PRM_IDATDEST_IV_CTR);

            /* Set g_aes_dtf_gcm_j1 to AES data input register */
            ICUM_AESDID0 = *(g_aes_dtf_gcm_j0 + ADDR_0TH);
            ICUM_AESDID1 = *(g_aes_dtf_gcm_j0 + ADDR_1ST);
            ICUM_AESDID2 = *(g_aes_dtf_gcm_j0 + ADDR_2ND);
            ICUM_AESDID3 = *(g_aes_dtf_gcm_j0 + ADDR_3RD);

            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | IVSEL | NEW_KEY)) |
                            /* Set 0000 : Plain text */
                              (PRM_ATTR_GCM_TAG
                             /* Set 0 : No last block */
                             | PRM_NO_LAST_BLOCK
                            /* Set : Number of bits in final block */
                             | PRM_REM_128BIT
                            /* Set 1 : Use the value set in IV_CTR_i */
                             | PRM_IVSEL_USE
                            /* Set 0 : Use the key used in the previous operation */
                             | PRM_NEW_KEY_USE);
        }

        if (FLG_OFF == skip_flg)
        {
            /* Perform AES DTF processing of TAG (GCM_TA) */
            /* Ask for tags */
            (void)d_AES_AddCounter(&total_size[NUM0], p_param->auth_data_size_in_bits);
            (void)d_AES_AddCounter(&total_size[NUM2], p_param->data_size_in_bits);

            /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);

            err = d_AES_Cipher((const uint32_t*)total_size, (uint32_t*)auth_tag);
            if (ER_OK == err)
            {
                (void)d_AES_LastBlock(g_aes_dtf_info.p_auth_tag, (uint32_t*)auth_tag, g_aes_dtf_info.auth_tag_size_in_bit);
            }
            else
            {
                /* do nothing */
            }
        } /* end of if (FLG_OFF == skip_flg) */
        else
        {
            /* Do Nothing */
        }
        /* Enable EI Level Maskable Interrupt */
        ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do Nothing */
    }
    return err;
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0038]:[ICUMFW_UD_S4_D_AES_0038]
* Function name: d_AES_ExecutionAtOnceGcmDtf
* Description  : Execute encryption of crypto AES at once. (for GCM - DTF)
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
*                async                  : sync/async transfer mode
*                                         Range: D_AES_TRANS_SYNC
*                                                D_AES_TRANS_ASYNC
*                p_aes_dest             : address of output value
*                p_auth_tag             : address of authentication tag
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_AES_TIMEOUT_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
uint32_t d_AES_ExecutionAtOnceGcmDtf(aes_all_at_once_gcm_t* p_param)
{
    uint32_t err;
    uint32_t skip_flg;
    uint32_t set_iv[AES_BLOCK_SIZE];
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    uint32_t aes_phys_src;
    uint32_t aes_phys_dest;
    uint32_t aes_phys_auth;
#endif
    aes_setparam_t setparam;
    aes_all_at_once_gcm_t gcmparam;

    /* Set local variable and 0 initialization */
    err = ER_OK;
    skip_flg = FLG_OFF;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    aes_phys_src = D_AES_ZERO_U32;
    aes_phys_dest = D_AES_ZERO_U32;
    aes_phys_auth = D_AES_ZERO_U32;
#endif
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_setparam_t));
    L_Toolbox_MemSet((uint8_t *)&gcmparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_all_at_once_gcm_t));
    L_Toolbox_MemSet((uint8_t *)set_iv, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(set_iv));

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    /* convert physical address */
    (void)d_AES_ConvertAddress((uint32_t)p_param->p_aes_src,
                               (uint32_t)p_param->p_aes_dest,
                               (uint32_t)p_param->p_auth_data,
                               (uint32_t *)&aes_phys_src,
                               (uint32_t *)&aes_phys_dest,
                               (uint32_t *)&aes_phys_auth,
                               (uint16_t)D_AES_CIPHER_MODE_GCM);
#endif

    /* Set parameters (g_aes_dtf_info) */
    g_aes_dtf_info.cipher_mode            = (uint8_t)D_AES_CIPHER_MODE_GCM;
    g_aes_dtf_info.direction              = p_param->direction;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    g_aes_dtf_info.p_aes_src              = (uint32_t *)aes_phys_src;
    g_aes_dtf_info.p_aes_dest             = (uint32_t *)aes_phys_dest;
#else
    g_aes_dtf_info.p_aes_src              = p_param->p_aes_src;
    g_aes_dtf_info.p_aes_dest             = p_param->p_aes_dest;
#endif
    g_aes_dtf_info.async                  = p_param->async;
    g_aes_dtf_info.iv_size_in_bits        = p_param->iv_size_in_bits;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    g_aes_dtf_info.p_auth_data            = (uint32_t *)aes_phys_auth;
#else
    g_aes_dtf_info.p_auth_data            = p_param->p_auth_data;
#endif
    g_aes_dtf_info.auth_data_size_in_bits = p_param->auth_data_size_in_bits;
    g_aes_dtf_info.data_size_in_bit       = p_param->data_size_in_bits;
    g_aes_dtf_info.auth_tag_size_in_bit   = p_param->auth_tag_size_in_bits;
    g_aes_dtf_info.p_auth_tag             = p_param->p_auth_tag;
    g_aes_dtf_info.key_length             = p_param->key_length;

    /* Set parameters (setparam) */
    setparam.cipher_mode = (uint8_t)D_AES_CIPHER_MODE_GCM;
    setparam.direction   = p_param->direction;
    setparam.key_type    = (uint8_t)D_AES_KEYTYPE_PLAIN;
    setparam.p_aes_key   = p_param->p_aes_key;
    setparam.key_length  = p_param->key_length;

    /* Initialization of encryption and key operation */
    (void)d_AES_SetParamDtf(&setparam);

    g_aes_dtf_info.subroutine = D_AES_DTF_GCM_IV;

    /* Calculation g_aes_dtf_gcm_j0, g_aes_dtf_gcm_j1 */
    /* IV = 96 ? */
    if (D_AES_SET96 == p_param->iv_size_in_bits)
    {
        d_AES_GcmSetIV_NoIcup(p_param->p_aes_iv);

    } /* end of if (D_AES_SET96 == p_param->iv_size_in_bits) */
    else
    {
        err = d_AES_GcmSetIV_Icup(p_param);
        if (ER_OK == err)
        {
            /* Do Nothing */
        }
        else
        {
            skip_flg = FLG_ON;
        }
    } /* end of else */

    if (FLG_OFF == skip_flg)
    {
        /* Set GCM parameters (gcmparam) */
        gcmparam = *p_param;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
        gcmparam.p_aes_src = (uint32_t *)aes_phys_src;
        gcmparam.p_aes_dest = (uint32_t *)aes_phys_dest;
        gcmparam.p_auth_data = (uint32_t *)aes_phys_auth;
#endif
        err = d_AES_AtOnceGcmDtf(&gcmparam);
    }
    else
    {
        /* Do Nothing */
    }
    if (ER_IN_PROCESS == err)
    {
        /* do nothing */
    }
    else
    {
        g_aes_dtf_flag = FLG_OFF;
    }

    return(err);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0102]:[ICUMFW_UD_S4_D_AES_0102]
* Function name: d_AES_GetYr
* Description  : Output of yr value (for CCM)
* Arguments    : none
* Return Value : none
***************************************************************************************/
static void d_AES_GetYr(void)
{
    uint32_t auth_yr[D_AES_BLOCK_SIZE_4];

    L_Toolbox_MemSet((uint8_t *)auth_yr, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(auth_yr));

    /* Read from ICUM_AESB0DODi register */
    *(auth_yr + ADDR_0TH) = ICUM_AESDOD0;
    *(auth_yr + ADDR_1ST) = ICUM_AESDOD1;
    *(auth_yr + ADDR_2ND) = ICUM_AESDOD2;
    *(auth_yr + ADDR_3RD) = ICUM_AESDOD3;

    (void)d_AES_LastBlock(g_aes_dtf_info.p_auth_yr,
                          (uint32_t*)auth_yr,
                          (uint16_t)g_aes_dtf_info.auth_tag_size_in_bit);

}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0103]:[ICUMFW_UD_S4_D_AES_0103]
* Function name: d_AES_GetIv_Dtf
* Description  : Input of iv value (for CCM)
* Arguments    : none
* Return Value : none
***************************************************************************************/
static void d_AES_GetIv_Dtf(uint32_t* p_iv, uint32_t iv_size_in_bits)
{
    uint8_t iv_data[AES_BLOCK_SIZE_IN_BYTES];
    uint8_t sift;
    uint32_t saved_psw;

    L_Toolbox_MemSet((uint8_t *)iv_data, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(iv_data));
    sift = D_AES_ZERO_U8;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Register setting */
    /* Set ICUM_AESCTRL register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) |
                   /* Set 10 : Initial vector */
                   PRM_IDATDEST_IV_CTR;

    /* Generate IV data */
    sift = (uint8_t)(AES_BLOCK_SIZE_IN_BYTES - (iv_size_in_bits /
           D_AES_BYTE_SIZE_IN_BITS));

    (void)L_Toolbox_MemCpy(&iv_data[sift],
                    (const uint8_t *)p_iv,
                    (int16_t)(iv_size_in_bits / D_AES_BYTE_SIZE_IN_BITS));
    
    /* Set input data N */
    /* Set j1 to AES data input register */
    ICUM_AESDID0 = *((uint32_t *)iv_data + ADDR_0TH);
    ICUM_AESDID1 = *((uint32_t *)iv_data + ADDR_1ST);
    ICUM_AESDID2 = *((uint32_t *)iv_data + ADDR_2ND);
    ICUM_AESDID3 = *((uint32_t *)iv_data + ADDR_3RD);

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0104]:[ICUMFW_UD_S4_D_AES_0104]
* Function name: d_AES_SetB0_Dtf
* Description  : Output of B0 (for CCM)
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
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_SetB0_Dtf(aes_all_at_once_ccm_t* p_param, uint32_t* p_B0)
{
    uint32_t err;
    uint32_t saved_psw;

    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Check reserved bit range of B0. */
    if ((B0_RESERVED != (*(p_B0 + ADDR_0TH) & ~B0_ALEN))
     || (B0_RESERVED != (*(p_B0 + ADDR_1ST) & ~(B0_NLEN | B0_TLEN | B0_PLEN0)))
     || (B0_RESERVED != (*(p_B0 + ADDR_3RD) & ~B0_PLEN2)))
    {
        err = ER_PARAM;
    }
    /* Check prohibited settings of Nlen and Tlen. */
    else if ((B0_NLEN == (B0_NLEN & *(p_B0 + ADDR_1ST)))
          || (B0_TLEN == (B0_TLEN & *(p_B0 + ADDR_1ST))))
    {
        err = ER_PARAM;
    }
    else
    {
        /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);

        if (D_AES_DIR_ENCRYPTION == (uint32_t)p_param->direction)
        {
            err = d_AES_CipherDummyRead((const uint32_t*)p_B0);
        }
        else
        {
            /* Check Alen and Plen of B0. */
            if ((D_AES_ZERO_U32 != p_param->auth_data_size_in_bits) ||
                (D_AES_ZERO_U32 != p_param->data_size_in_bits))
            {
                err = d_AES_CipherDummyRead((const uint32_t*)p_B0);
            }
            else
            {
                err = d_AES_CipherNoRead((const uint32_t*)p_B0);
            }
        }
    }
    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0105]:[ICUMFW_UD_S4_D_AES_0105]
* Function name: d_AES_GetTagCcm_Dtf
* Description  : Output of tag (for CCM)
* Arguments    : auth_tag_size_in_bits  : authentication tag size in bits
*                p_auth_tag             : address of authentication tag
* Return Value : ER_OK
*                ER_D_AES_TIMEOUT_ERROR
***************************************************************************************/
static uint32_t d_AES_GetTagCcm_Dtf(uint32_t* p_auth_tag, uint32_t auth_tag_size_in_bits)
{
    uint32_t err;
    uint32_t saved_psw;
    uint32_t auth_tag[AES_BLOCK_SIZE];

    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;
    L_Toolbox_MemSet((uint8_t *)auth_tag, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(auth_tag));

    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* Set ICUM_AESCTRL register */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) |
                    PRM_IDATDEST_BCI;

    /* Yr value output */
    (void)d_AES_GetYr();

    /* Register setting */
    /* Set ICUM_AESDCTL register */
    ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                     /* Set 1101 : Input data T XOR MSBTlen (S0) */
                      (PRM_ATTR_CCM_DEC_T_X_S0
                     /* Set 0 : Do not use the value set in IV_CTR_i */
                     | PRM_IVSEL_NO_USE
                     /* Set 0 : Use the key used in the previous calculation */
                     | PRM_NEW_KEY_NO_USE);

    /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
    ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);

    err = d_AES_Cipher(p_auth_tag, (uint32_t*)auth_tag);

    if (ER_OK == err)
    {
        (void)d_AES_LastBlock(p_auth_tag,
                             (uint32_t*)auth_tag,
                             (uint16_t)auth_tag_size_in_bits);
    }
    else
    {
        /* Do Nothing */
    }
    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0057]:[ICUMFW_UD_S4_D_AES_0057]
* Function name: d_AES_ExecutionAtOnceCcmDtf
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
*                ER_D_AES_SEQUENCE_ERROR
*                ER_D_AES_TIMEOUT_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
uint32_t d_AES_ExecutionAtOnceCcmDtf(aes_all_at_once_ccm_t* p_param)
{
    uint32_t err;
    uint8_t B0[AES_BLOCK_SIZE_IN_BYTES];
    uint32_t saved_psw;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    uint32_t aes_phys_src;
    uint32_t aes_phys_dest;
    uint32_t aes_phys_auth;
#endif
    aes_setparam_t setparam;
    aes_all_at_once_ccm_t ccmparam;

    /* Set local variable and 0 initialization */
    err = ER_OK;
    saved_psw = D_AES_ZERO_U32;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    aes_phys_src = D_AES_ZERO_U32;
    aes_phys_dest = D_AES_ZERO_U32;
    aes_phys_auth = D_AES_ZERO_U32;
#endif
    L_Toolbox_MemSet((uint8_t *)B0, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(B0));
    L_Toolbox_MemSet((uint8_t *)&setparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_setparam_t));
    L_Toolbox_MemSet((uint8_t *)&ccmparam, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_all_at_once_ccm_t));

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    /* convert physical address */
    (void)d_AES_ConvertAddress((uint32_t)p_param->p_aes_src,
                               (uint32_t)p_param->p_aes_dest,
                               (uint32_t)p_param->p_auth_data,
                               (uint32_t *)&aes_phys_src,
                               (uint32_t *)&aes_phys_dest,
                               (uint32_t *)&aes_phys_auth,
                               (uint16_t)D_AES_CIPHER_MODE_CCM);
#endif
    
    if (USE_CTX2 == g_ctx2_state)
    {
        err = ER_D_AES_SEQUENCE_ERROR;
    }
    else
    {
        /* Set parameters (g_aes_dtf_info) */
        g_aes_dtf_info.cipher_mode            = (uint8_t)D_AES_CIPHER_MODE_CCM;
        g_aes_dtf_info.direction              = p_param->direction;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
        g_aes_dtf_info.p_aes_src              = (uint32_t *)aes_phys_src;
        g_aes_dtf_info.p_aes_dest             = (uint32_t *)aes_phys_dest;
#else
        g_aes_dtf_info.p_aes_src              = p_param->p_aes_src;
        g_aes_dtf_info.p_aes_dest             = p_param->p_aes_dest;
#endif
        g_aes_dtf_info.async                  = p_param->async;
        g_aes_dtf_info.iv_size_in_bits        = p_param->iv_size_in_bits;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
        g_aes_dtf_info.p_auth_data            = (uint32_t *)aes_phys_auth;
#else
        g_aes_dtf_info.p_auth_data            = p_param->p_auth_data;
#endif
        g_aes_dtf_info.auth_data_size_in_bits = p_param->auth_data_size_in_bits;
        g_aes_dtf_info.data_size_in_bit       = p_param->data_size_in_bits;
        g_aes_dtf_info.auth_tag_size_in_bit   = p_param->auth_tag_size_in_bits;
        g_aes_dtf_info.p_auth_tag             = p_param->p_auth_tag;
        g_aes_dtf_info.key_length             = p_param->key_length;
        g_aes_dtf_info.p_auth_yr              = p_param->p_auth_yr;

        /* Set parameters (setparam) */
        setparam.cipher_mode = (uint8_t)D_AES_CIPHER_MODE_CCM;
        setparam.direction   = p_param->direction;
        setparam.key_type    = (uint8_t)D_AES_KEYTYPE_PLAIN;
        setparam.p_aes_key   = p_param->p_aes_key;
        setparam.key_length  = p_param->key_length;

        /* Initialization of encryption and key operation */
        (void)d_AES_SetParamDtf(&setparam);

        (void)d_AES_GetIv_Dtf(p_param->p_aes_iv, p_param->iv_size_in_bits);

        /* Disable EI Level Maskable Interrupt */
        DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

        /* Register setting */
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                         /* Set 0001 : Input data of parameter B0 used as format function */
                          (PRM_ATTR_CCM_B0
                         /* Set 1 : Use the value set in IV_CTR_i */
                         | PRM_IVSEL_USE
                         /* Set 1 : Use the data set in the key register (KEY_i) */
                         | PRM_NEW_KEY_USE);

        err = d_AES_GenerateB0((uint8_t*)B0, p_param->iv_size_in_bits, p_param->auth_tag_size_in_bits,
                                p_param->data_size_in_bits, p_param->auth_data_size_in_bits);

        /* Enable EI Level Maskable Interrupt */
        ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

        if (ER_OK == err)
        {
            err = d_AES_SetB0_Dtf(p_param, (uint32_t *)B0);
            if (ER_OK == err)
            {
                /* Set parameters (ccmparam) */
                ccmparam = *p_param;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
                ccmparam.p_aes_src   = (uint32_t *)aes_phys_src;
                ccmparam.p_aes_dest  = (uint32_t *)aes_phys_dest;
                ccmparam.p_auth_data = (uint32_t *)aes_phys_auth;
#endif

                /* Cryptographic mode : ENCRYPTION */
                if (D_AES_DIR_ENCRYPTION == (uint32_t)p_param->direction)
                {
                    err = d_AES_ExecutionAtOnceCcmDtfSubEnc(&ccmparam);
                }
                /* Cryptographic mode : DECRYPTION */
                else
                {
                    err = d_AES_ExecutionAtOnceCcmDtfSubDec(&ccmparam);
                }
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
        if (ER_IN_PROCESS == err)
        {
            /* do nothing */
        }
        else
        {
            g_aes_dtf_flag = FLG_OFF;
            g_ctx2_state = NO_USE_CTX2;
        }
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0058]:[ICUMFW_UD_S4_D_AES_0058]
* Function name: d_AES_ExecutionAtOnceCcmDtfSubEnc
* Description  : Performs AES-CCM encryption processing collectively (for DTF)
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
*                ER_D_AES_SEQUENCE_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
static uint32_t d_AES_ExecutionAtOnceCcmDtfSubEnc(aes_all_at_once_ccm_t *p_param)
{
    uint32_t err;
    uint32_t skip_flg;
    uint32_t saved_psw;
    aes_transfer_dtf_t transfer_dtf;
    uint32_t auth_tag[AES_BLOCK_SIZE];
    uint32_t auth_tag_zero[AES_BLOCK_SIZE];

    /* Set local variable and 0 initialization */
    err = ER_OK;
    skip_flg = FLG_OFF;
    saved_psw = D_AES_ZERO_U32;
    L_Toolbox_MemSet((uint8_t *)&transfer_dtf, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_transfer_dtf_t));
    L_Toolbox_MemSet((uint8_t *)auth_tag, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(auth_tag));
    L_Toolbox_MemSet((uint8_t *)auth_tag_zero, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(auth_tag_zero));

    /* CCM_ENC1 DTF processing */
    if (D_AES_ZERO_U32 != p_param->auth_data_size_in_bits)
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                         /* Set 0011 : Input data of related data A used as format function */
                          (PRM_ATTR_CCM_A
                         /* Set 0 : Do not use the value set in IV_CTR_i */
                         | PRM_IVSEL_NO_USE
                         /* Set 0 : Use the key used in the previous calculation */
                         | PRM_NEW_KEY_NO_USE);

        g_aes_dtf_info.subroutine = D_AES_DTF_CCM_ENC1;

        transfer_dtf.number     = p_param->auth_data_size_in_bits / D_AES_SET128;
        transfer_dtf.p_aes_src  = p_param->p_auth_data;
        transfer_dtf.p_aes_dest = NULL;
        transfer_dtf.async      = p_param->async;

        /* Execute AES (DTF transfer) CCM_ENC1 */
        err = d_AES_TransferDtf(&transfer_dtf);

        /* Judgment of execution result */
        if ((D_AES_TRANS_ASYNC == (uint32_t)transfer_dtf.async) ||
            ((D_AES_TRANS_SYNC == (uint32_t)transfer_dtf.async) && (ER_OK != err)))
        {
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do Nothing */
        }

    } /* end of if (D_AES_ZERO_U32 != p_param->auth_data_size_in_bits) */
    else
    {
        /* Do Nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* CCM_ENC2 DTF processing */
        if (D_AES_ZERO_U32 != g_aes_dtf_info.data_size_in_bit)
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                             /* Set 0100 : Plaintext */
                              (PRM_ATTR_CCM_PLAIN
                             /* Set 0 : Do not use the value set in IV_CTR_i */
                             | PRM_IVSEL_NO_USE
                             /* Set 0 : Use the key used in the previous calculation */
                             | PRM_NEW_KEY_NO_USE);

            g_aes_dtf_info.subroutine = D_AES_DTF_CCM_ENC2;

            transfer_dtf.number     = p_param->data_size_in_bits / D_AES_SET128;
            transfer_dtf.p_aes_src  = p_param->p_aes_src;
            transfer_dtf.p_aes_dest = p_param->p_aes_dest;
            transfer_dtf.async      = p_param->async;

            /* Execute AES (DTF transfer) CCM_ENC2 */
            err = d_AES_TransferDtf(&transfer_dtf);

            /* Judgment of execution result */
            if ((D_AES_TRANS_ASYNC == (uint32_t)transfer_dtf.async) ||
                ((D_AES_TRANS_SYNC == (uint32_t)transfer_dtf.async) && (ER_OK != err)))
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do Nothing */
            }
        } /* end of if (D_AES_ZERO_U32 != g_aes_dtf_info.data_size_in_bit) */
        else
        {
            /* Do Nothing */
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do Nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* Disable EI Level Maskable Interrupt */
        DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

        /* Set ICUM_AESCTRL register */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC) |
                         /* Set 01 : Block cipher output register (BCO_DTF) */
                          PRM_ODATSRC_BCO_DTF);

        /* CCM_T DTF processing */
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                         /* Set 1000 : Input data of dummy T XOR MSB Tlen (S0) */
                          (PRM_ATTR_CCM_ENC_DUMMY
                         /* Set 0 : Do not use the value set in IV_CTR_i */
                         | PRM_IVSEL_NO_USE
                         /* Set 0 : Use the key used in the previous calculation */
                         | PRM_NEW_KEY_NO_USE);

        /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);

        /* Set initialization vector to AES data input register */
        err = d_AES_Cipher((uint32_t*)auth_tag_zero, (uint32_t*)auth_tag);

        if (ER_OK == err)
        {
            (void)d_AES_LastBlock(g_aes_dtf_info.p_auth_tag,
                                 (uint32_t*)auth_tag,
                                 (uint16_t)g_aes_dtf_info.auth_tag_size_in_bit);
        }
        else
        {
            /* Do Nothing */
        }
        /* Enable EI Level Maskable Interrupt */
        ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do Nothing */
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0059]:[ICUMFW_UD_S4_D_AES_0059]
* Function name: d_AES_ExecutionAtOnceCcmDtfSubDec
* Description  : Performs AES-CCM encryption processing collectively (for DTF)
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
*                ER_D_AES_SEQUENCE_ERROR
*                ER_IN_PROCESS
***************************************************************************************/
static uint32_t d_AES_ExecutionAtOnceCcmDtfSubDec(aes_all_at_once_ccm_t *p_param)
{
    uint32_t err;
    uint32_t skip_flg;
    aes_transfer_dtf_t transfer_dtf;
    /* Set local variable and 0 initialization */
    err = ER_OK;
    skip_flg = FLG_OFF;
    L_Toolbox_MemSet((uint8_t *)&transfer_dtf, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_transfer_dtf_t));

    /* CCM_DEC1 DTF processing */
    if (D_AES_ZERO_U32 != p_param->data_size_in_bits)
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                          /* Set 0010 : Ciphertext */
                           (PRM_ATTR_CCM_DEC_CIPHER
                          /* Set 0 : Do not use the value set in IV_CTR_i */
                          | PRM_IVSEL_NO_USE
                          /* Set 0 : Use the key used in the previous calculation */
                          | PRM_NEW_KEY_NO_USE);

        g_aes_dtf_info.subroutine = D_AES_DTF_CCM_DEC1;

        transfer_dtf.number     = p_param->data_size_in_bits / D_AES_SET128;
        transfer_dtf.p_aes_src  = p_param->p_aes_src;
        transfer_dtf.p_aes_dest = p_param->p_aes_dest;
        transfer_dtf.async      = p_param->async;

        /* Execute AES (DTF transfer) CCM_DEC1 */
        err = d_AES_TransferDtf(&transfer_dtf);

        /* Judgment of execution result */
        if ((D_AES_TRANS_ASYNC == (uint32_t)transfer_dtf.async) ||
            ((D_AES_TRANS_SYNC == (uint32_t)transfer_dtf.async) && (ER_OK != err)))
        {
            skip_flg = FLG_ON;
        }
        else
        {
            /* Do Nothing */
        }
    } /* end of if ((D_AES_ZERO_U32 != p_param->auth_data_size_in_bits) || */
    else
    {
        /* Do Nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* CCM_DEC2 DTF processing */
        if (D_AES_ZERO_U32 != p_param->auth_data_size_in_bits)
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                             /* Set 0011 : Input data of related data A used as format function */
                              (PRM_ATTR_CCM_A
                             /* Set 0 : Do not use the value set in IV_CTR_i */
                             | PRM_IVSEL_NO_USE
                             /* Set 0 : Use the key used in the previous calculation */
                             | PRM_NEW_KEY_NO_USE);

            g_aes_dtf_info.subroutine = D_AES_DTF_CCM_DEC2;

            transfer_dtf.number     = p_param->auth_data_size_in_bits / D_AES_SET128;
            transfer_dtf.p_aes_src  = p_param->p_auth_data;
            transfer_dtf.p_aes_dest = NULL;
            transfer_dtf.async      = p_param->async;

            /* Execute AES (DTF transfer) CCM_DEC2 */
            err = d_AES_TransferDtf(&transfer_dtf);

            /* Judgment of execution result */
            if ((D_AES_TRANS_ASYNC == (uint32_t)transfer_dtf.async) ||
                ((D_AES_TRANS_SYNC == (uint32_t)transfer_dtf.async) && (ER_OK != err)))
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do Nothing */
            }

            } /* end of if (D_AES_SET1_U32 != p_param->auth_data_size_in_bits) */
        else
        {
            /* Do Nothing */
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do Nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* CCM_DEC3 DTF processing */
        if (D_AES_ZERO_U32 != p_param->data_size_in_bits)
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                             /* Set 0100 : Plaintext */
                              (PRM_ATTR_CCM_PLAIN
                             /* Set 0 : Do not use the value set in IV_CTR_i */
                             | PRM_IVSEL_NO_USE
                             /* Set 0 : Use the key used in the previous calculation */
                             | PRM_NEW_KEY_NO_USE);

            g_aes_dtf_info.subroutine = D_AES_DTF_CCM_DEC3;

            transfer_dtf.number     = p_param->data_size_in_bits / D_AES_SET128;
            transfer_dtf.p_aes_src  = p_param->p_aes_dest;
            transfer_dtf.p_aes_dest = NULL;
            transfer_dtf.async      = p_param->async;

            /* Execute AES (DTF transfer) CCM_DEC3 */
            err = d_AES_TransferDtf(&transfer_dtf);

            /* Judgment of execution result */
            if ((D_AES_TRANS_ASYNC == (uint32_t)transfer_dtf.async) ||
                ((D_AES_TRANS_SYNC == (uint32_t)transfer_dtf.async) && (ER_OK != err)))
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* Do Nothing */
            }
        } /* end of if ((D_AES_ZERO_U32 != (p_param->data_size_in_bits - p_param->auth_tag_size_in_bits))) */
        else
        {
            /* Do Nothing */
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do Nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        err = d_AES_GetTagCcm_Dtf(p_param->p_auth_tag, p_param->auth_tag_size_in_bits);
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do Nothing */
    }
    return err;
}

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0078]:[ICUMFW_UD_S4_D_AES_0078]
* Function name: ICICUDMACA0
* Description  : Interrupt handler for ICICUDMACA0 (ROM Mode)
* Arguments    : none
* Return Value : none
***************************************************************************************/
void ICICUDMACA0(void)
{
    if ((DMA_DST_END == (ICUM_AESDMAINTF & DMA_DST_END))
        || ((DISTDTF == (ICUM_AESCTRL & DISTDTF)) && (DMA_SRC_END == (ICUM_AESDMAINTF & DMA_SRC_END))))
    {
        ICUM_AESDMAINTF = (ICUM_AESDMAINTF & (~DMA_DST_END));
        ICUM_AESDMAINTF = (ICUM_AESDMAINTF & (~DMA_SRC_END));
        ICUM_AESDMAINTF;
        SYNC_PIPELINE();

        ICUM_AESDMASRCCNT &= ~DMA_DME;
        ICUM_AESDMADSTCNT &= ~DMA_DME;
        ICUM_AESDMADSTCNT;
        SYNC_PIPELINE();

        if (D_AES_TRANS_ASYNC == (uint32_t)g_aes_dtf_info.async)
        {
            d_AES_IsrDtfEnd();
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

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0113]:[ICUMFW_UD_S4_D_AES_0113]
* Function name: ICICUDMACA0_RAM
* Description  : Interrupt handler for ICICUDMACA0 (RAM Mode)
* Arguments    : none
* Return Value : none
***************************************************************************************/
PRAGMA(ghs section text = ".icum_driver_ram")
PRAGMA(ghs section rodata = ".icum_driver_const_ram")
void ICICUDMACA0_RAM(void)
{
    if (NULL != g_p_aes_callbackFunc)
    {
        g_p_aes_callbackFunc(ER_D_AES_STATUS_ERROR);
        (void)d_AES_FreeKeyRestriction(g_aes_dtf_info.key_length, g_aes_atonce_id_use_mng);
    }
    else
    {
        /* Do Nothing */
    }

    return;
}
PRAGMA(ghs section text = default)
PRAGMA(ghs section rodata = default)
#else
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0109]:[ICUMFW_UD_S4_D_AES_0109]
* Function name: ICICUAESE0DTFEND
* Description  : Interrupt handler for ICICUAESE0DTFEND (ROM mode)
* Arguments    : none
* Return Value : none
***************************************************************************************/
void ICICUAESE0DTFEND(void)
{
    if (D_AES_TRANS_ASYNC == g_aes_dtf_info.async)
    {
        d_AES_IsrDtfEnd();
    }
    else
    {
        /* do nothing */
    }

    return;
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0080]:[ICUMFW_UD_S4_D_AES_0080]
* Function name: d_AES_IsrDtfEnd
* Description  : Interrupt handler for ICICUAESE0DTFEND (Common)
* Arguments    : none
* Return Value : none
***************************************************************************************/
static inline void d_AES_IsrDtfEnd(void)
{
    /* Part 1 : Parameter check */
    if (NULL != g_p_aes_callbackFunc)
    {
        /* 2-1 */
        switch ((uint32_t)g_aes_dtf_info.cipher_mode)
        {
            case D_AES_CIPHER_MODE_XTS:
                (void)d_AES_IsrDtfEndSubXts();
                break;
            case D_AES_CIPHER_MODE_GCM:
                (void)d_AES_IsrDtfEndSubGcm();
                break;
            case D_AES_CIPHER_MODE_CCM:
                if (D_AES_DIR_ENCRYPTION == (uint32_t)g_aes_dtf_info.direction)
                {
                    (void)d_AES_IsrDtfEndSubCcmEnc();
                }
                else if (D_AES_DIR_DECRYPTION == (uint32_t)g_aes_dtf_info.direction)
                {
                    (void)d_AES_IsrDtfEndSubCcmDec();
                }
                else
                {
                    /* Do Nothing */
                }
                break;
            case D_AES_CIPHER_MODE_CMAC:
                (void)d_AES_IsrDtfEndSubCmac();
                break;
            default:
                g_aes_dtf_flag = FLG_OFF;
                /* Part 3 : Call back result */
                (void)g_p_aes_callbackFunc(ER_OK);
                (void)d_AES_FreeKeyRestriction(g_aes_dtf_info.key_length, g_aes_atonce_id_use_mng);
                break;
        } /* end of switch (g_aes_dtf_info.cipher_mode) */
    } /* end of if (NULL != g_p_aes_callbackFunc) */
    else
    {
        /* Do Nothing */
    }
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0094]:[ICUMFW_UD_S4_D_AES_0094]
* Function name: d_AES_IsrDtfEndSubCmac
* Description  : Interrupt handler for ICICUAESE0DTFEND (for CMAC)
* Arguments    : none
* Return Value : none
***************************************************************************************/
static inline void d_AES_IsrDtfEndSubCmac(void)
{
    /* Parameter check */
    if (NULL != g_p_aes_callbackFunc)
    {
        /* Output data source selection bit */
        ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC)) |
                         /* Set 01 : Block cipher output register (BCO_DTF) */
                          (PRM_ODATSRC_BCO_DTF);

        /* Read from ICUM_AESDODi register */
        *(g_aes_dtf_info.p_aes_dest + ADDR_0TH) = ICUM_AESDOD0;
        *(g_aes_dtf_info.p_aes_dest + ADDR_1ST) = ICUM_AESDOD1;
        *(g_aes_dtf_info.p_aes_dest + ADDR_2ND) = ICUM_AESDOD2;
        *(g_aes_dtf_info.p_aes_dest + ADDR_3RD) = ICUM_AESDOD3;

        g_aes_dtf_flag = FLG_OFF;
        /* Call back result */
        (void)g_p_aes_callbackFunc(ER_OK);
        (void)d_AES_FreeKeyRestriction(g_aes_dtf_info.key_length, g_aes_atonce_id_use_mng);
    }
    else
    {
        /* Do Nothing */
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0110]:[ICUMFW_UD_S4_D_AES_0110]
* Function name: d_AES_IsrDtfEndSubXtsLast
* Description  : Interrupt handler for XTS last block
* Arguments    : none
* Return Value : none
***************************************************************************************/
static inline void d_AES_IsrDtfEndSubXtsLast(void)
{
    uint32_t err;
    uint32_t output[AES_BLOCK_SIZE];
    uint32_t last_output[AES_BLOCK_SIZE];
    uint32_t input[AES_BLOCK_SIZE];
    aes_transfer_dtf_t transfer_dtf;

    err = ER_OK;
    L_Toolbox_MemSet((uint8_t *)output, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(output));
    L_Toolbox_MemSet((uint8_t *)last_output, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(last_output));
    L_Toolbox_MemSet((uint8_t *)input, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(input));

    transfer_dtf.number     = (uint32_t)D_AES_BLOCK_SIZE_1;
    transfer_dtf.p_aes_src  = (uint32_t*)((uint32_t)g_aes_dtf_info.p_aes_src +
                                ((g_aes_dtf_info.number - (uint32_t)D_AES_BLOCK_SIZE_1) * D_AES_BYTE_CONVERSION));
    transfer_dtf.p_aes_dest = (uint32_t*)((uint32_t)g_aes_dtf_info.p_aes_dest +
                                ((g_aes_dtf_info.number - (uint32_t)D_AES_BLOCK_SIZE_1) * D_AES_BYTE_CONVERSION));
    transfer_dtf.async      = (uint8_t)D_AES_TRANS_ASYNC;

    if (D_AES_SET128 == (uint32_t)g_aes_dtf_info.last_block_in_bits)
    {
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM )) |
                        /* Set 0000 : Plain text */
                            (PRM_ATTR_XTS_TEXT
                        /* Set 1 : Last block */
                            | PRM_LAST_BLOCK
                        /* Set : block_size */
                            | PRM_REM_ZERO);

        err = d_AES_Cipher(transfer_dtf.p_aes_src, transfer_dtf.p_aes_dest);
    }
    else
    {
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM )) |
                        /* Set 0000 : Plain text */
                            (PRM_ATTR_XTS_TEXT
                        /* Set 1 : Last block */
                            | PRM_LAST_BLOCK
                        /* Set : block_size */
                            | (REM & ((uint32_t)g_aes_dtf_info.last_block_in_bits << SHIFT_REM)));

        (void)d_AES_LastBlock(output,
                                (g_aes_dtf_info.p_aes_dest +
                                ((g_aes_dtf_info.number - (uint32_t)D_AES_BLOCK_SIZE_2) * NUM4)),
                                g_aes_dtf_info.last_block_in_bits);

        /* calculate last block size */
        (void)d_AES_LastBlock(input, transfer_dtf.p_aes_src, g_aes_dtf_info.last_block_in_bits);

        err = d_AES_Cipher((const uint32_t*)input, (uint32_t*)last_output);

        (void)L_Toolbox_MemCpy((uint8_t *)(g_aes_dtf_info.p_aes_dest +
                                ((g_aes_dtf_info.number - (uint32_t)D_AES_BLOCK_SIZE_2) * NUM4)),
                                (const uint8_t *)last_output,
                                (int16_t)AES_BLOCK_SIZE_IN_BYTES);

        (void)d_AES_LastBlock(transfer_dtf.p_aes_dest,
                                output,
                                g_aes_dtf_info.last_block_in_bits);
    }

    g_aes_dtf_flag = FLG_OFF;
    (void)g_p_aes_callbackFunc(err);
    (void)d_AES_FreeKeyRestriction(g_aes_dtf_info.key_length, g_aes_atonce_id_use_mng);

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0081]:[ICUMFW_UD_S4_D_AES_0081]
* Function name: d_AES_IsrDtfEndSubXts
* Description  : Interrupt handler for ICICUAESE0DTFEND (for XTS)
* Arguments    : none
* Return Value : none
***************************************************************************************/
static inline void d_AES_IsrDtfEndSubXts(void)
{
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    uint32_t err;
#endif
    uint32_t skip_flg;
    aes_transfer_dtf_t transfer_dtf;
    /* Set local variable and 0 initialization */
    skip_flg = FLG_OFF;
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
    err = ER_OK;
#endif
    L_Toolbox_MemSet((uint8_t *)&transfer_dtf, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_transfer_dtf_t));

    /* Parameter check */
    if (NULL != g_p_aes_callbackFunc)
    {
        /* Do nothing */
    }
    else
    {
        skip_flg = FLG_ON;
    }

    if (FLG_OFF == skip_flg)
    {
        /* Set ICUM_AESDCTL register */
        ICUM_AESDCTL = (ICUM_AESDCTL & ~(IVSEL | NEW_KEY)) |
                        /* Set 0 : Do not use the value set in IV_CTR_i */
                        ( PRM_IVSEL_NO_USE
                        /* Set 0 : Use the key used in the previous operation */
                        | PRM_NEW_KEY_NO_USE);

        /* AES DTF processing of XTS_ECDC2 */
        if ((uint32_t)D_AES_DTF_XTS_ECDC1 == (uint32_t)g_aes_dtf_info.subroutine)
        {
            if (((uint32_t)D_AES_SET128 != (uint32_t)g_aes_dtf_info.last_block_in_bits) &&
                (D_AES_DIR_DECRYPTION == (uint32_t)g_aes_dtf_info.direction))
            {
                g_aes_dtf_info.subroutine = (uint8_t)D_AES_DTF_XTS_DEC;

                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~( ATTR | REM)) |
                                /* Set 10 : Next to the last block *
                                 * if a decryption operation is in progress and the last block is less than 127 */
                                 (PRM_ATTR_XTS_SHORT_LAST
                                /* Set 0: Not the last block */
                                | PRM_REM_ZERO);
            }
            else
            {
                g_aes_dtf_info.subroutine = (uint8_t)D_AES_DTF_XTS_ECDC2;

                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~( ATTR | REM)) |
                                /* Set 0000 : Plain text */
                                 (PRM_ATTR_XTS_TEXT
                                /* Set 0: Not the last block */
                                | PRM_REM_ZERO);
            }

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
            (void)D_REMAP_GetLogicalAddrs((uint32_t)g_aes_dtf_info.p_aes_src, (uint32_t *)&g_aes_dtf_info.p_aes_src);
            (void)D_REMAP_GetLogicalAddrs((uint32_t)g_aes_dtf_info.p_aes_dest, (uint32_t *)&g_aes_dtf_info.p_aes_dest);
#endif

            transfer_dtf.number     = (uint32_t)D_AES_BLOCK_SIZE_1;
            transfer_dtf.p_aes_src  = (uint32_t*)((uint32_t)g_aes_dtf_info.p_aes_src +
                                     ((g_aes_dtf_info.number - (uint32_t)D_AES_BLOCK_SIZE_2) * D_AES_BYTE_CONVERSION));
            transfer_dtf.p_aes_dest = (uint32_t*)((uint32_t)g_aes_dtf_info.p_aes_dest +
                                     ((g_aes_dtf_info.number - (uint32_t)D_AES_BLOCK_SIZE_2) * D_AES_BYTE_CONVERSION));
            transfer_dtf.async      = (uint8_t)D_AES_TRANS_ASYNC;

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
            /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) |
                                (PRM_ODATSRC_BCO
                               | PRM_IDATDEST_BCI);
            err = d_AES_Cipher(transfer_dtf.p_aes_src, transfer_dtf.p_aes_dest);
            if (ER_OK == err)
            {
                d_AES_IsrDtfEndSubXtsLast();
            }
            else
            {
                g_aes_dtf_flag = FLG_OFF;
                (void)g_p_aes_callbackFunc(err);
                (void)d_AES_FreeKeyRestriction(g_aes_dtf_info.key_length, g_aes_atonce_id_use_mng);
            }
#else
            /* Execute AES (DTF transfer) */
            (void)d_AES_TransferDtf(&transfer_dtf);
#endif
        } /* end of if (D_AES_DTF_XTS_ECDC1 == g_aes_dtf_info.subroutine) */

        /* AES DTF processing of XTS_ECDC3 */
        else if (((uint32_t)D_AES_DTF_XTS_ECDC2 == (uint32_t)g_aes_dtf_info.subroutine) ||
                ((uint32_t)D_AES_DTF_XTS_DEC == (uint32_t)g_aes_dtf_info.subroutine))
        {
            g_aes_dtf_info.subroutine = (uint8_t)D_AES_DTF_XTS_ECDC3;

            /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | 
                                (PRM_ODATSRC_BCO
                               | PRM_IDATDEST_BCI);

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
            (void)D_REMAP_GetLogicalAddrs((uint32_t)g_aes_dtf_info.p_aes_src, (uint32_t *)&g_aes_dtf_info.p_aes_src);
            (void)D_REMAP_GetLogicalAddrs((uint32_t)g_aes_dtf_info.p_aes_dest, (uint32_t *)&g_aes_dtf_info.p_aes_dest);
#endif
            d_AES_IsrDtfEndSubXtsLast();
        } /* end of else if ((D_AES_DTF_XTS_ECDC2 == g_aes_dtf_info.subroutine) || */
        /* Call back result */
        else
        {
            g_aes_dtf_flag = FLG_OFF;
            (void)g_p_aes_callbackFunc(ER_D_AES_STATUS_ERROR);
            (void)d_AES_FreeKeyRestriction(g_aes_dtf_info.key_length, g_aes_atonce_id_use_mng);
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        g_aes_dtf_flag = FLG_OFF;
    }
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0082]:[ICUMFW_UD_S4_D_AES_0082]
* Function name: d_AES_IsrDtfEndSubGcm
* Description  : Interrupt handler for ICICUAESE0DTFEND (for GCM)
* Arguments    : none
* Return Value : none
***************************************************************************************/
static inline void d_AES_IsrDtfEndSubGcm(void)
{
    uint32_t skip_flg;
    uint32_t total_size[AES_BLOCK_SIZE];
    aes_transfer_dtf_t transfer_dtf;
    uint32_t auth_tag[D_AES_SET_4];
    uint32_t err;

    /* Set local variable and 0 initialization */
    skip_flg = FLG_OFF;
    L_Toolbox_MemSet((uint8_t *)&transfer_dtf, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_transfer_dtf_t));
    L_Toolbox_MemSet((uint8_t*)total_size, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(total_size));
    L_Toolbox_MemSet((uint8_t*)auth_tag, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(auth_tag));

    /* Parameter check */
    if (NULL != g_p_aes_callbackFunc)
    {
        /* Do Nothing */
    }
    else
    {
        skip_flg = FLG_ON;
    }

    if (FLG_OFF == skip_flg)
    {
        /* For D_AES_DTF_GCM_AAD */
        if ((uint32_t)D_AES_DTF_GCM_AAD == (uint32_t)g_aes_dtf_info.subroutine)
        {
            g_aes_dtf_info.subroutine = D_AES_DTF_GCM_PC;

            if (D_AES_ZERO_U32 != g_aes_dtf_info.data_size_in_bit)
            {
                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR)) |
                                 /* Set 0001 : Input data of GCTR function */
                                  (PRM_ATTR_GCM_GCTR);

                /* Set IVSEL */
                if (D_AES_ZERO_U32 == g_aes_dtf_info.auth_data_size_in_bits)
                {
                    /* Set ICUM_AESDCTL register */
                    ICUM_AESDCTL = (ICUM_AESDCTL & ~(IVSEL)) |
                                     /* Set 1 : Use the value set in IV_CTR_i */
                                      (PRM_IVSEL_USE);
                }
                else
                {
                    /* Set ICUM_AESDCTL register */
                    ICUM_AESDCTL = (ICUM_AESDCTL & ~(IVSEL)) |
                                     /* Set 0 : Do not use the value set in IV_CTR_i */
                                      (PRM_IVSEL_NO_USE);
                }

                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~(NEW_KEY)) |
                                 /* Set 0 : Use the key used in the previous operation */
                                  (PRM_NEW_KEY_NO_USE);

                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~(REM)) |
                                 /* Set last block size : Set the number of bits in the last block */
                                 (REM & ((g_aes_dtf_info.data_size_in_bit % D_AES_SET128) << SHIFT_REM));

                g_aes_dtf_info.subroutine = (uint8_t)D_AES_DTF_GCM_PC;

                transfer_dtf.number     = g_aes_dtf_info.data_size_in_bit / D_AES_SET128;
                transfer_dtf.p_aes_src  = g_aes_dtf_info.p_aes_src;
                transfer_dtf.p_aes_dest = g_aes_dtf_info.p_aes_dest;
                transfer_dtf.async      = (uint8_t)D_AES_TRANS_ASYNC;

                (void)d_AES_TransferDtf(&transfer_dtf);

                skip_flg = FLG_ON;
            } /* end of if (D_AES_ZERO_U32 != g_aes_dtf_info.data_size_in_bit) */
            else
            {
                /* Do Nothing */
            }
        } /* end of if (D_AES_DTF_GCM_AAD == g_aes_dtf_info.subroutine) */
        else
        {
            /* Do Nothing */
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        /* Do Nothing */
    }

    if (FLG_OFF == skip_flg)
    {
        /* For D_AES_DTF_GCM_PC */
        if ((uint32_t)D_AES_DTF_GCM_PC == (uint32_t)g_aes_dtf_info.subroutine)
        {
            /* Set ICUM_AESCTRL register */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(IDATDEST)) |
                             /* Set 10 : Initial vector */
                             (PRM_IDATDEST_IV_CTR);

            /* Set g_aes_dtf_gcm_j1 to AES data input register */
            ICUM_AESDID0 = *(g_aes_dtf_gcm_j0 + ADDR_0TH);
            ICUM_AESDID1 = *(g_aes_dtf_gcm_j0 + ADDR_1ST);
            ICUM_AESDID2 = *(g_aes_dtf_gcm_j0 + ADDR_2ND);
            ICUM_AESDID3 = *(g_aes_dtf_gcm_j0 + ADDR_3RD);

            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | LAST | REM | NEW_KEY)) |
                            /* Set 0000 : Plain text */
                              (PRM_ATTR_GCM_TAG
                            /* Set 0 : No last block */
                             | PRM_NO_LAST_BLOCK
                            /* Set : Number of bits in final block */
                             | PRM_REM_128BIT
                            /* Set 0 : Use the key used in the previous operation */
                             | PRM_NEW_KEY_NO_USE);

            if ((D_AES_ZERO_U32 == g_aes_dtf_info.auth_data_size_in_bits) &&
                (D_AES_ZERO_U32 == g_aes_dtf_info.data_size_in_bit))
            {
                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~(IVSEL)) |
                                /* Set 1 : Do use the value set in IV_CTR_i */
                                 PRM_IVSEL_USE;
            }
            else
            {
                /* Set ICUM_AESDCTL register */
                ICUM_AESDCTL = (ICUM_AESDCTL & ~(IVSEL)) |
                                /* Set 0 : Do not use the value set in IV_CTR_i */
                                 PRM_IVSEL_NO_USE;
            }

            (void)d_AES_AddCounter(&total_size[NUM0], g_aes_dtf_info.auth_data_size_in_bits);
            (void)d_AES_AddCounter(&total_size[NUM2], g_aes_dtf_info.data_size_in_bit);

            /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);

            err = d_AES_Cipher((const uint32_t*)total_size, (uint32_t*)auth_tag);
            if (ER_OK == err)
            {
                (void)d_AES_LastBlock(g_aes_dtf_info.p_auth_tag,
                                      (uint32_t*)auth_tag,
                                      (uint16_t)g_aes_dtf_info.auth_tag_size_in_bit);
            }
            else
            {
                /* do nothing */
            }
            g_aes_dtf_flag = FLG_OFF;
            (void)g_p_aes_callbackFunc(err);
            (void)d_AES_FreeKeyRestriction(g_aes_dtf_info.key_length, g_aes_atonce_id_use_mng);

        } /* end of else if (D_AES_DTF_GCM_PC == g_aes_dtf_info.subroutine) */
        else
        {
            g_aes_dtf_flag = FLG_OFF;
            (void)g_p_aes_callbackFunc(ER_D_AES_STATUS_ERROR);
            (void)d_AES_FreeKeyRestriction(g_aes_dtf_info.key_length, g_aes_atonce_id_use_mng);
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        g_aes_dtf_flag = FLG_OFF;
    }
    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0083]:[ICUMFW_UD_S4_D_AES_0083]
* Function name: d_AES_IsrDtfEndSubCcmEnc
* Description  : Interrupt handler for ICICUAESE0DTFEND (for CCM encode)
* Arguments    : none
* Return Value : none
***************************************************************************************/
static inline void d_AES_IsrDtfEndSubCcmEnc(void)
{
    uint32_t skip_flg;
    uint32_t err;
    aes_transfer_dtf_t transfer_dtf;
    uint32_t auth_tag[AES_BLOCK_SIZE];
    uint32_t auth_tag_zero[AES_BLOCK_SIZE];
    /* Set local variable and 0 initialization */
    skip_flg = FLG_OFF;
    err = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&transfer_dtf, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_transfer_dtf_t));
    L_Toolbox_MemSet((uint8_t *)auth_tag, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(auth_tag));
    L_Toolbox_MemSet((uint8_t *)auth_tag_zero, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(auth_tag_zero));

    /* Parameter check */
    if (NULL != g_p_aes_callbackFunc)
    {
        /* Do Nothing */
    }
    else
    {
        skip_flg = FLG_ON;
    }

    if (FLG_OFF == skip_flg)
    {
        /* D_AES_DTF_CCM_ENC2 DTF processing */
        if (((uint32_t)D_AES_DTF_CCM_ENC1 == (uint32_t)g_aes_dtf_info.subroutine) &&
            (D_AES_ZERO_U32 != g_aes_dtf_info.data_size_in_bit))
        {
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                             /* Set 0100 : Plaintext */
                              (PRM_ATTR_CCM_PLAIN
                             /* Set 0 : Do not use the value set in IV_CTR_i */
                             | PRM_IVSEL_NO_USE
                             /* Set 0 : Use the key used in the previous calculation */
                             | PRM_NEW_KEY_NO_USE);

            g_aes_dtf_info.subroutine = (uint8_t)D_AES_DTF_CCM_ENC2;

            transfer_dtf.number     = g_aes_dtf_info.data_size_in_bit / D_AES_SET128;
            transfer_dtf.p_aes_src  = g_aes_dtf_info.p_aes_src;
            transfer_dtf.p_aes_dest = g_aes_dtf_info.p_aes_dest;
            transfer_dtf.async      = (uint8_t)D_AES_TRANS_ASYNC;

            /* Execute AES (DTF transfer) */
            (void)d_AES_TransferDtf(&transfer_dtf);

        } /* end of else if ((uint8_t)D_AES_DTF_CCM_ENC1 == g_aes_dtf_info.subroutine) */

        /* Tag processing */
        else if (((uint32_t)D_AES_DTF_CCM_ENC1 == (uint32_t)g_aes_dtf_info.subroutine) ||
                ((uint32_t)D_AES_DTF_CCM_ENC2 == (uint32_t)g_aes_dtf_info.subroutine))
        {

            /* Set ICUM_AESCTRL register */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC) |
                             /* Set 01 : Block cipher output register (BCO_DTF) */
                              PRM_ODATSRC_BCO_DTF);

            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                             /* Set 1000 : Input data of dummy T XOR MSB Tlen (S0) */
                              (PRM_ATTR_CCM_ENC_DUMMY
                             /* Set 0 : Do not use the value set in IV_CTR_i */
                             | PRM_IVSEL_NO_USE
                             /* Set 0 : Use the key used in the previous calculation */
                             | PRM_NEW_KEY_NO_USE);

            /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);

            /* Set initialization vector to AES data input register */
            err = d_AES_Cipher((uint32_t*)auth_tag_zero, (uint32_t*)auth_tag);

            if (ER_OK == err)
            {
                (void)d_AES_LastBlock(g_aes_dtf_info.p_auth_tag,
                                     (uint32_t*)auth_tag,
                                     (uint16_t)g_aes_dtf_info.auth_tag_size_in_bit);
            }
            else
            {
                /* Do Nothing */
            }
            g_aes_dtf_flag = FLG_OFF;
            g_ctx2_state = NO_USE_CTX2;
            (void)g_p_aes_callbackFunc(err);
            (void)d_AES_FreeKeyRestriction(g_aes_dtf_info.key_length, g_aes_atonce_id_use_mng);
        } /* end of else if ((uint8_t)D_AES_DTF_CCM_ENC1 == g_aes_dtf_info.subroutine) */
        else
        {
            g_aes_dtf_flag = FLG_OFF;
            g_ctx2_state = NO_USE_CTX2;
            (void)g_p_aes_callbackFunc(ER_D_AES_STATUS_ERROR);
            (void)d_AES_FreeKeyRestriction(g_aes_dtf_info.key_length, g_aes_atonce_id_use_mng);
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        g_aes_dtf_flag = FLG_OFF;
        g_ctx2_state = NO_USE_CTX2;
    }
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0084]:[ICUMFW_UD_S4_D_AES_0084]
* Function name: d_AES_IsrDtfEndSubCcmDec
* Description  : Interrupt handler for ICICUAESE0DTFEND (for CCM Decode)
* Arguments    : none
* Return Value : none
***************************************************************************************/
static inline void d_AES_IsrDtfEndSubCcmDec(void)
{
    uint32_t skip_flg;
    uint32_t err;
    aes_transfer_dtf_t transfer_dtf;
    uint32_t auth_tag[AES_BLOCK_SIZE];
    /* Set local variable and 0 initialization */
    skip_flg = FLG_OFF;
    err = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&transfer_dtf, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(aes_transfer_dtf_t));
    L_Toolbox_MemSet((uint8_t *)auth_tag, (uint8_t)D_AES_SET_ZERO_16, (int16_t)sizeof(auth_tag));

    /* Parameter check */
    if (NULL != g_p_aes_callbackFunc)
    {
        /* Do Nothing */
    }
    else
    {
        skip_flg = FLG_ON;
    }

    if (FLG_OFF == skip_flg)
    {
        /* AES DTF processing of CCM_DEC2 */
        if ((D_AES_DTF_CCM_DEC1 == (uint32_t)g_aes_dtf_info.subroutine) &&
            (D_AES_ZERO_U32 != g_aes_dtf_info.auth_data_size_in_bits))
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                             /* Set 0011 : Input data of related data A used as format function */
                              (PRM_ATTR_CCM_A
                             /* Set 0 : Do not use the value set in IV_CTR_i */
                             | PRM_IVSEL_NO_USE
                             /* Set 0 : Use the key used in the previous calculation */
                             | PRM_NEW_KEY_NO_USE);

            g_aes_dtf_info.subroutine = D_AES_DTF_CCM_DEC2;

            transfer_dtf.number     = g_aes_dtf_info.auth_data_size_in_bits / D_AES_SET128;
            transfer_dtf.p_aes_src  = g_aes_dtf_info.p_auth_data;
            transfer_dtf.p_aes_dest = NULL;
            transfer_dtf.async      = (uint8_t)D_AES_TRANS_ASYNC;

            /* Execute AES (DTF transfer) */
            (void)d_AES_TransferDtf(&transfer_dtf);
        } /* end of if (D_AES_DTF_CCM_DEC1 == (uint32_t)g_aes_dtf_info.subroutine) */

        /* AES DTF processing of CCM_DEC3 */
        else if (((D_AES_DTF_CCM_DEC1 == (uint32_t)g_aes_dtf_info.subroutine) &&
                  (D_AES_ZERO_U32 == g_aes_dtf_info.auth_data_size_in_bits)) ||
                 ((D_AES_DTF_CCM_DEC2 == (uint32_t)g_aes_dtf_info.subroutine) &&
                  (D_AES_ZERO_U32 != g_aes_dtf_info.data_size_in_bit)))
        {
            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                             /* Set 0100 : Plaintext */
                              (PRM_ATTR_CCM_PLAIN
                             /* Set 0 : Do not use the value set in IV_CTR_i */
                             | PRM_IVSEL_NO_USE
                             /* Set 0 : Use the key used in the previous calculation */
                             | PRM_NEW_KEY_NO_USE);

            g_aes_dtf_info.subroutine = D_AES_DTF_CCM_DEC3;

            transfer_dtf.number     = g_aes_dtf_info.data_size_in_bit / D_AES_SET128;
            transfer_dtf.p_aes_src  = g_aes_dtf_info.p_aes_dest;
            transfer_dtf.p_aes_dest = NULL;
            transfer_dtf.async      = (uint8_t)D_AES_TRANS_ASYNC;

            /* Execute AES (DTF transfer) */
            (void)d_AES_TransferDtf(&transfer_dtf);
        } /* end of else if (D_AES_DTF_CCM_DEC1 == (uint32_t)g_aes_dtf_info.subroutine) */

        /* AES DTF processing of CCM_C */
        else if  ((D_AES_DTF_CCM_DEC1 == (uint32_t)g_aes_dtf_info.subroutine) ||
                  (D_AES_DTF_CCM_DEC2 == (uint32_t)g_aes_dtf_info.subroutine) ||
                  (D_AES_DTF_CCM_DEC3 == (uint32_t)g_aes_dtf_info.subroutine))
        {
            /* Set ICUM_AESCTRL register */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC)) |
                             PRM_ODATSRC_BCO_DTF;

            /* Yr value output */
            d_AES_GetYr();

            /* Set ICUM_AESDCTL register */
            ICUM_AESDCTL = (ICUM_AESDCTL & ~(ATTR | IVSEL | NEW_KEY)) |
                             /* Set 1101 : Input data T XOR MSBTlen (S0) */
                              (PRM_ATTR_CCM_DEC_T_X_S0
                             /* Set 0 : Do not use the value set in IV_CTR_i */
                             | PRM_IVSEL_NO_USE
                             /* Set 0 : Use the key used in the previous calculation */
                             | PRM_NEW_KEY_NO_USE);

            /* Set ICUM_AESCTRL register ODATSRC = BCO, IDATDEST = BCI */
            ICUM_AESCTRL = (ICUM_AESCTRL & ~(ODATSRC | IDATDEST)) | (PRM_ODATSRC_BCO | PRM_IDATDEST_BCI);

            /* Set initialization vector to AES data input register */
            err = d_AES_Cipher(g_aes_dtf_info.p_auth_tag, (uint32_t*)auth_tag);

            if (ER_OK == err)
            {
                (void)d_AES_LastBlock(g_aes_dtf_info.p_auth_tag,
                                     (uint32_t*)auth_tag,
                                     (uint16_t)g_aes_dtf_info.auth_tag_size_in_bit);
            }
            else
            {
                /* Do Nothing */
            }

            g_aes_dtf_flag = FLG_OFF;
            g_ctx2_state = NO_USE_CTX2;
            (void)g_p_aes_callbackFunc(err);
            (void)d_AES_FreeKeyRestriction(g_aes_dtf_info.key_length, g_aes_atonce_id_use_mng);
        } /* end of else if (D_AES_DTF_CCM_DEC1 == (uint32_t)g_aes_dtf_info.subroutine) */
        else
        {
            g_aes_dtf_flag = FLG_OFF;
            g_ctx2_state = NO_USE_CTX2;
            (void)g_p_aes_callbackFunc(ER_D_AES_STATUS_ERROR);
            (void)d_AES_FreeKeyRestriction(g_aes_dtf_info.key_length, g_aes_atonce_id_use_mng);
        }
    } /* end of if (FLG_OFF == skip_flg) */
    else
    {
        g_aes_dtf_flag = FLG_OFF;
        g_ctx2_state = NO_USE_CTX2;
    }
}

#if defined(TARGET_DEVICE_S4H) || defined(TARGET_DEVICE_U2B) || defined(TARGET_DEVICE_U2C) || defined(TARGET_DEVICE_U2C4)
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0085]:[ICUMFW_UD_S4_D_AES_0085]
* Function name: ICICUAESE0DTFERR
* Description  : Interrupt handler for ICICUAESE0DTFERR (ROM mode)
* Arguments    : none
* Return Value : none
***************************************************************************************/
void ICICUAESE0DTFERR(void)
{
    d_AES_IsrDtfErr();

    return;
}
#endif

#if defined(TARGET_DEVICE_S4H) || defined(TARGET_DEVICE_U2B) || defined(TARGET_DEVICE_U2C) || defined(TARGET_DEVICE_U2C4)
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_AES_0087]:[ICUMFW_UD_S4_D_AES_0087]
* Function name: d_AES_IsrDtfErr
* Description  : Interrupt handler for ICICUAESE0DTFERR (Common)
* Arguments    : none
* Return Value : none
***************************************************************************************/
static inline void d_AES_IsrDtfErr(void)
{
    if (NULL != g_p_aes_callbackFunc)
    {
        g_p_aes_callbackFunc(ER_D_AES_DTF_ERROR);
    }
    else
    {
        /* Do Nothing */
    }
    
    return;
}
#endif
