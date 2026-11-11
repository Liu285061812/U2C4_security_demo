/******************************************************************************/
/* Component Name   ICU-M TRNG Driver                                         */
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
/**********************************************************************************
* Include
***********************************************************************************/
#include "renesas_types.h"

#include "u2b_reg_adr.h"

#include "r_icumif_api.h"
#include ICUM_DEVICE_FILE
#include "icum_common_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_d_trng_pub.h"
#include "icum_int_declaration_pub.h"

/**********************************************************************************
* Define
***********************************************************************************/
/* ICUM_TRNG Use Register */
#define ICUM_TRNGSR ICUM_TRNGG0SR
#define ICUM_TRNGSCLR ICUM_TRNGG0SCLR
#define ICUM_TRNGCR ICUM_TRNGG0CR
#define ICUM_TRNGRNRR ICUM_TRNGG0RNRR
#define TRNG_MODE_REG (*(volatile uint32_t *)0xFFFEE0B8UL)

/* Common */
#define NUM0        ((uint32_t)0)             /* value : 0 */
#define NUM1        ((uint32_t)1)             /* value : 1 */
#define NUM2        ((uint32_t)2)             /* value : 2 */
#define NUM9        ((uint32_t)9)             /* value : 9 */
#define NUM16       ((uint32_t)16)            /* value : 16 */
#define ALIGN4      ((uint32_t)0x00000003uL)  /* Check 4 alignment */
#define SELFCHK_OFF ((uint32_t)0u)            /* value : 0 */
#define NO_ALIGNERR ((uint32_t)0uL)           /* value : 0 */

#define OFF         ((uint32_t)0)             /* off(disable) */
#define ON          ((uint32_t)1)             /* on(enable) */

/* TRNG control register bit */
#define CR_DEF      ((uint32_t)0x00000000uL)   /* Default  */
#define GE_OFF      ((uint32_t)0x00000000uL)   /* GE bit:0 */
#define GE_ON       ((uint32_t)0x00000001uL)   /* GE bit:1 */
#define RCHKE_OFF   ((uint32_t)0x00000000uL)   /* RCHKE bit:0 */
#define RCHKE_ON    ((uint32_t)0x00000004uL)   /* RCHKE bit:1 */
#define RCMSEL_OFF  ((uint32_t)0x00000000uL)   /* RCMSEL bit:0 */
#define RCMSEL_ON   ((uint32_t)0x00000008uL)   /* RCMSEL bit:1 */
#define OHSTART_OFF ((uint32_t)0x00000000u)   /* OHSTART bit:0 */
#define OHSTART_ON  ((uint32_t)0x00010000u)   /* OHSTART bit:1 */
#define HTIE_OFF    ((uint32_t)0x00000000u)   /* HTIE bit:0 */
#define HTIE_ON     ((uint32_t)0x00100000u)   /* HTIE bit:1 */

/* TRNG status register bit */
#define RRDY_OFF    ((uint32_t)0x00000000uL)   /* RRDY bit:0 */
#define RRDY_ON     ((uint32_t)0x00000001uL)   /* RRDY bit:1 */
#define RCHKS_OFF   ((uint32_t)0x00000000uL)   /* RCHKS bit:0 */
#define RCHKS_ON    ((uint32_t)0x00000002uL)   /* RCHKS bit:1 */
#define RCHKERR_OFF ((uint32_t)0x00000000uL)   /* RCHKERR bit:0 */
#define RCHKERR_ON  ((uint32_t)0x00000004uL)   /* RCHKERR bit:1 */
#define CTHTERR_OFF ((uint32_t)0x00000000u)   /* CTHTERR bit:0 */
#define CTHTERR_ON  ((uint32_t)0x00010000u)   /* CTHTERR bit:1 */
#define ODHTERR_OFF ((uint32_t)0x00000000u)   /* ODHTERR bit:0 */
#define ODHTERR_ON  ((uint32_t)0x00020000u)   /* ODHTERR bit:1 */
#define ODHFIN_OFF  ((uint32_t)0x00000000u)   /* ODHFIN bit:0 */
#define ODHFIN_ON   ((uint32_t)0x02000000u)   /* ODHFIN bit:1 */

/* TRNG status clear register bit */
#define RRDYCLR_ON  ((uint32_t)0x00000001u)   /* RRDYCLR bit:1 */
#define RCHKCLR_ON  ((uint32_t)0x00000002u)   /* RCHKCLR bit:1 */
#define HTERRCLR_ON ((uint32_t)0x00010000u)   /* HTERRCLR bit:1 */
#define ODHFCLR_ON  ((uint32_t)0x02000000u)   /* ODHFCLR bit:1 */

/* TRNG mode control register bit */
#define HTFV_OFF    ((uint32_t)0x00000000uL)   /* HTFV bit:0 */
#define HTFV_ON     ((uint32_t)0x00000001uL)   /* HTFV bit:1 */

/* Array */
#define ANUM_TRN    ((uint32_t)10)             /* array number of trn[] */

/* TRNG test mode (0-3) */
#define TEST_MODE_RANGE  ((uint32_t)3)

/**********************************************************************************
* Structure
***********************************************************************************/
typedef struct
{
    uint32_t trn[ANUM_TRN]; /* TRN store area(32bit*10=320bit) */
    uint8_t  r_top;         /* readable top index */
    uint8_t  w_cur;         /* writable current index */
    uint8_t  w_cur_enable;  /* writable current index(for hold) */
} d_TRNG_trn_info_t;

/**********************************************************************************
* Variable
***********************************************************************************/
static d_TRNG_trn_info_t g_trn_info;
static uint32_t g_online_test_enable;
static uint32_t g_word_number;
static uint32_t g_trng_count;
static uint32_t g_err_seed;
static uint32_t g_cont_word_num;
static uint32_t g_cont_test_cnt;
static uint32_t g_continueous_test = OFF;

/**********************************************************************************
* Function
***********************************************************************************/
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TRNG_0001]:[ICUMFW_UD_U2B_D_TRNG_0001]
* Function name: D_TRNG_Init
* Description  : TRNG initialization
* Arguments    : N/A
* Return Value : ER_D_TRNG_OK
***************************************************************************************/
uint32_t D_TRNG_Init(void)
{
    uint32_t remaining = 1000000UL;
    while ((ICUM_TRNGSR & 0x01000000UL) == 0UL)
    {
        if (remaining-- == 0UL)
        {
            return ER_D_TRNG_STATUS;
        }
    }
    if ((ICUM_TRNGSR & (CTHTERR_ON | ODHTERR_ON)) != 0UL)
    {
        return ER_D_TRNG_SDT_FAIL;
    }
    ICUM_TRNGCR   = HTIE_ON;
    ICUM_TRNGSCLR = RCHKCLR_ON;

    return ER_D_TRNG_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TRNG_0002]:[ICUMFW_UD_U2B_D_TRNG_0002]
* Function name: D_TRNG_SetTest
* Description  : TRNG Self-diagnostic test settings
* Arguments    : test_enable_bit (TRNG self-diagnostic test enable bit)
* Return Value : ER_D_TRNG_OK
*              : ER_D_TRNG_STATUS
***************************************************************************************/
uint32_t D_TRNG_SetTest(uint32_t test_enable_bit)
{
    uint32_t ret_err = ER_D_TRNG_OK;

    if ((GE_OFF != (GE_ON & ICUM_TRNGCR)) || (OHSTART_OFF != (OHSTART_ON & ICUM_TRNGCR)))
    {
        ret_err = ER_D_TRNG_STATUS;
    }
    else
    {
        if (D_TRNG_DIAG_P1 == g_online_test_enable)
        {
            ICUM_TRNGCR &= ~RCMSEL_ON;
        }
        else if (D_TRNG_DIAG_P2 == g_online_test_enable)
        {
            ICUM_TRNGCR |= RCMSEL_ON;
        }
        else if (D_TRNG_HEALTH_TEST == g_online_test_enable)
        {
            TRNG_MODE_REG |= HTFV_ON;
            ICUM_TRNGCR |= OHSTART_ON;
        }
        else
        {
            /* do nothing */
        }

        if (SELFCHK_OFF == test_enable_bit)
        {
            ICUM_TRNGCR &= ~RCHKE_ON;
        }
        else
        {
            ICUM_TRNGCR |= RCHKE_ON;
        }
    }

    return ret_err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TRNG_0003]:[ICUMFW_UD_U2B_D_TRNG_0003]
* Function name: D_TRNG_EnableModule
* Description  : TRNG enabled
* Arguments    : N/A
* Return Value : ER_D_TRNG_OK
***************************************************************************************/
uint32_t D_TRNG_EnableModule(void)
{
    ICUM_TRNGCR |= GE_ON;

    return ER_D_TRNG_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TRNG_0004]:[ICUMFW_UD_U2B_D_TRNG_0004]
* Function name: D_TRNG_DisableModule
* Description  : TRNG disabled
* Arguments    : N/A
* Return Value : ER_D_TRNG_OK
***************************************************************************************/
PRAGMA(ghs section text = ".icum_driver_ram")
PRAGMA(ghs section rodata = ".icum_driver_const_ram")
uint32_t D_TRNG_DisableModule(void)
{
    ICUM_TRNGCR &= ~GE_ON;

    /* Dummy read & Synchronize pipeline to wait for the completion of */
    /* writing to the ICUM_TRNGCR TRNG Control Register              */
    ICUM_TRNGCR;
    SYNC_PIPELINE();

    return ER_D_TRNG_OK;
}
PRAGMA(ghs section text = default)
PRAGMA(ghs section rodata = default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TRNG_0005]:[ICUMFW_UD_U2B_D_TRNG_0005]
* Function name: D_TRNG_GetValue
* Description  : 32-bit intrinsic random number acquisition
* Arguments    : p_trng_Value (Pointer to store output data)
* Return Value : ER_D_TRNG_OK
*              : ER_D_TRNG_PARAMETER
*              : ER_D_TRNG_SDT_SUCCESS
*              : ER_D_TRNG_SDT_FAIL
*              : ER_D_TRNG_BUSY
*              : ER_D_TRNG_STATUS
***************************************************************************************/
uint32_t D_TRNG_GetValue(uint32_t *p_trng_Value)
{
    const uint32_t read_reg = ICUM_TRNGSR;
    uint32_t ret_err;

    if ((NULL == p_trng_Value) || (NO_ALIGNERR != (ALIGN4 & (uint32_t)p_trng_Value)))
    {
        ret_err = ER_D_TRNG_PARAMETER;
    }
    else if (GE_OFF == (GE_ON & ICUM_TRNGCR))
    {
        ret_err = ER_D_TRNG_STATUS;
    }
    else if (RRDY_OFF == (RRDY_ON & read_reg))
    {
        ret_err = ER_D_TRNG_BUSY;
    }
    else
    {
        if (D_TRNG_HEALTH_TEST != g_online_test_enable)
        {
            if (RCHKS_OFF == (RCHKS_ON & read_reg))
            {
                ret_err = ER_D_TRNG_OK;
            }
            else
            {
                if (RCHKERR_OFF != (RCHKERR_ON & read_reg))
                {
                    ret_err = ER_D_TRNG_SDT_FAIL;
                }
                else
                {
                    ret_err = ER_D_TRNG_SDT_SUCCESS;
                }
                ICUM_TRNGSCLR = RCHKCLR_ON;
            }
        }
        else
        {
            if (ON == g_continueous_test)
            {
                if (CTHTERR_OFF != (CTHTERR_ON & read_reg))
                {
                    ret_err = ER_D_TRNG_SDT_FAIL;
                }
                else
                {
                    ret_err = ER_D_TRNG_SDT_SUCCESS;
                }
                ICUM_TRNGSCLR = HTERRCLR_ON;
            }
            else
            {
                if (ODHFIN_OFF == (ODHFIN_ON & read_reg))
                {
                    ret_err = ER_D_TRNG_OK;
                }
                else
                {
                    if (ODHTERR_OFF != (ODHTERR_ON & read_reg))
                    {
                        ret_err = ER_D_TRNG_SDT_FAIL;
                    }
                    else
                    {
                        ret_err = ER_D_TRNG_SDT_SUCCESS;
                    }
                    ICUM_TRNGSCLR = ODHFCLR_ON;
                }
            }
        }
        *p_trng_Value = ICUM_TRNGRNRR;
    }

    return ret_err;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TRNG_0006]:[ICUMFW_UD_U2B_D_TRNG_0006]
* Function name: D_TRNG_CheckTRN
* Description  : Check if there is enough data for g_trn_info.trn[]
* Arguments    : word_number
*              : p_result
* Return Value : ER_D_TRNG_OK
*              : ER_D_TRNG_PARAMETER(Debug only)
***************************************************************************************/
uint32_t D_TRNG_CheckTRN(uint32_t word_number, uint32_t* p_result)
{
    uint32_t ret_err;
    ret_err = ER_D_TRNG_OK;
    
    if (ANUM_TRN < word_number)
    {
        ret_err = ER_D_TRNG_PARAMETER;
    }
    else if (NULL == p_result)
    {
        ret_err = ER_D_TRNG_PARAMETER;
    }
    else
    {
        /* not enough (non toggle) */
        if ((((uint32_t)g_trn_info.w_cur >= (uint32_t)g_trn_info.r_top)
            && (((uint32_t)g_trn_info.w_cur - (uint32_t)g_trn_info.r_top) < word_number))
        /* or not enough (toggle) */
            || (((uint32_t)g_trn_info.w_cur < (uint32_t)g_trn_info.r_top) 
            && (((uint32_t)g_trn_info.w_cur + ANUM_TRN - (uint32_t)g_trn_info.r_top) < word_number)))
        {
            *p_result = D_TRNG_NOT_FULL;
        }
        else
        {
            *p_result = D_TRNG_FULL;
        }
    }
    return ret_err;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TRNG_0008]:[ICUMFW_UD_U2B_D_TRNG_0008]
* Function name: D_TRNG_GetTRN
* Description  : g_trn_info.trn[] in the toggle format is aligned linearly 
*              : and stored in p_trn
* Arguments    : p_trng (Pointer to area to get of the seed)
*              : word_number (Number of words of the seed to get)
* Return Value : ER_OK
*              : ER_D_TRNG_PARAMETER(Debug only)
***************************************************************************************/
uint32_t D_TRNG_GetTRN(uint32_t *p_trng, uint32_t word_number)
{
    uint32_t ct;    /* Loop counter */
    uint32_t ret_err;
    ret_err = ER_D_TRNG_OK;
    
   if (NULL != p_trng)
    {
        for (ct = NUM0; ct < word_number; ++ct)
        {
            p_trng[ct] = g_trn_info.trn[g_trn_info.r_top];
            ++g_trn_info.r_top;
            if (ANUM_TRN <= (uint32_t)g_trn_info.r_top)
            {
                g_trn_info.r_top = NUM0;
            }
            else
            {
                /* do nothing */
            }
        }
        if (ANUM_TRN <= (uint32_t)g_trn_info.w_cur)
        {
            g_trn_info.w_cur = NUM0;
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        ret_err = ER_D_TRNG_PARAMETER;
    }
    return ret_err;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TRNG_0009]:[ICUMFW_UD_U2B_D_TRNG_0009]
* Function name: D_TRNG_GetState
* Description  : g_trn_info.trn[] in the toggle format is aligned linearly 
*              : and stored in p_trn
* Arguments    : p_prng_TRN (Pointer to area to get of the seed)
*              : word_number (Number of words of the seed to get)
* Return Value : ER_OK
*              : ER_D_TRNG_PARAMETER(Debug only)
***************************************************************************************/
uint32_t D_TRNG_GetState(uint32_t* p_trng_state)
{
    uint32_t ret_err;
    ret_err = ER_D_TRNG_OK;
    
    if (NULL != p_trng_state)
    {
        switch (g_err_seed)
        {
            case ER_D_TRNG_OK:
                *p_trng_state = D_TRNG_NOT_GENERATING;
                break;
            case ER_D_TRNG_SEED:
                *p_trng_state = D_TRNG_GENERATING;
                break;
            case ER_D_TRNG_TRN:
                *p_trng_state = D_TRNG_TEST_ERR;
                break;
            default:
                /* Do nothing */
                break;
        }
    }
    else
    {
        ret_err = ER_D_TRNG_PARAMETER;
    }
    return ret_err;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TRNG_0007]:[ICUMFW_UD_U2B_D_TRNG_0007]
* Function name: D_TRNG_GenerateSeed
* Description  : Initializes TRNG and initiates the seed generation
* Arguments    : test_mode (self-diagnostic class P1 / class P2 / Health test)
*              : seed_pattern (Seed pattern to generate)
* Return Value : ER_D_TRNG_OK
*              : ER_D_TRNG_PARAMETER
***************************************************************************************/
uint32_t D_TRNG_GenerateSeed(uint32_t test_mode, uint32_t word_number)
{
    uint32_t err;
    if ((ANUM_TRN < word_number) || (TEST_MODE_RANGE < test_mode))
    {
        err = ER_D_TRNG_PARAMETER;
    }
    else
    {
        /* TRNG initialization, once disabled */
        (void)D_TRNG_DisableModule();
        g_trng_count = NUM0;
        g_word_number = word_number;
        /* Online test selection */
        if (ANUM_TRN == g_word_number)
        {
            /* Enable online test */
            g_online_test_enable = test_mode;
            g_trn_info.r_top = (uint8_t)NUM0;
            /* Write position : beginning of area */
            g_trn_info.w_cur = (uint8_t)NUM0;
            (void)D_TRNG_SetTest(ON);
        }
        else
        {
            g_online_test_enable = D_TRNG_NO_TEST;
            (void)D_TRNG_SetTest(OFF);
        }
        g_err_seed = ER_D_TRNG_SEED;

        if (D_TRNG_HEALTH_TEST != test_mode)
        {
            /* TRNG enable */
            (void)D_TRNG_EnableModule();
        }
        else
        {
            /* do nothing */
        }
        err = ER_D_TRNG_OK;
    }
    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_TRNG_0010]:[ICUMFW_UD_U2B_D_TRNG_0010]
* Function name: ICICUTRNGG0
* Description  : Process when true random number generation completion interrupt occurs
* Arguments    : N/A
* Return Value : N/A
***************************************************************************************/
void ICICUTRNGG0(void)
{
    static uint32_t h_previous;
    uint32_t value;
    uint32_t ret_err;
    uint32_t complete = OFF;
    uint32_t trng_size_chk;
    
    value = NUM0;
    trng_size_chk = D_TRNG_NOT_FULL;

    /* Get TRN (Exit function if non-normal end) */
    ret_err = D_TRNG_GetValue(&value);
    /* Replace with generated bits order */
    value = (value << NUM16) | (value >> NUM16);

    /* Online test disabled */
    if (D_TRNG_NO_TEST == g_online_test_enable)
    {
        /* Three times the same value check */
        /* Same value check (when the same value generation judgment function is enabled) */
        if ((h_previous == value) && (NUM0 != g_trng_count))
        {
            if (NUM2 <= g_trng_count)
            {
                g_trn_info.w_cur = g_trn_info.w_cur_enable;
                g_err_seed = ER_D_TRNG_TRN;
                complete = ON;
            }
            else
            {
                /* Increment the same number of occurrences */
                ++g_trng_count;
            }
        }
        else
        {
            g_trn_info.w_cur_enable = g_trn_info.w_cur;
            /* The same number of occurrences is 1, and the judgment is enabled */
            g_trng_count = NUM1;
        }

        if (ON == complete)
        {
            /* Do nothing */
        }
        else
        {
            /* Last Value update */
            h_previous = value;
            /* Store TRN */
            g_trn_info.trn[g_trn_info.w_cur] = value;
            ++g_trn_info.w_cur;
            if (ANUM_TRN <= (uint32_t)g_trn_info.w_cur)
            {
                g_trn_info.w_cur = NUM0;
            }
            else
            {
                /* do nothing */
            }
            /* If the TRN is not full */
            (void)D_TRNG_CheckTRN(g_word_number, &trng_size_chk);
            if (D_TRNG_FULL != trng_size_chk)
            {
                /* Do nothing */
            }
            else
            {
                (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_TRNG, NUM1);
                g_err_seed = ER_D_TRNG_OK;
                complete = ON;
            }
        }
    }
    /* online test (countinuous test) */
    else if ((ON == g_continueous_test) && (D_TRNG_HEALTH_TEST == g_online_test_enable))
    {
        /* Online test (320 bits) */
        if (NUM9 <= g_cont_word_num)
        {
            if (ER_D_TRNG_SDT_SUCCESS == ret_err)
            {
                /* Online test success((RNDERR clear)) */
                (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_RNDERR, NUM0);
                (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_TRNG, NUM1);
                g_err_seed = ER_D_TRNG_OK;
                complete = ON;
            }
            else if (ER_D_TRNG_SDT_FAIL == ret_err)
            {
                /* If an online test is retried twice and still NG, the error returns (RNDERR set) */
                if (NUM2 <= g_cont_test_cnt)
                {
                    (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_RNDERR, NUM1);
                    g_err_seed = ER_D_TRNG_TRN;
                    complete = ON;
                }
                else
                {
                    ++g_cont_test_cnt;
                    g_cont_word_num = NUM0;

                    /* Wait 1us or more to restart GE due to hardware restriction */
                    (void)D_TRNG_DisableModule();
                    NOPS(400);
                    (void)D_TRNG_EnableModule();
                }
            }
            else
            {
                /* again */
                /* Wait 1us or more to restart GE due to hardware restriction */
                (void)D_TRNG_DisableModule();
                NOPS(400);
                (void)D_TRNG_EnableModule();
            }
        }
        else
        {
            g_cont_word_num++;
        }
    }
    /* Online test enabled */
    else
    {
        if ((D_TRNG_DIAG_P1 == g_online_test_enable) || (D_TRNG_DIAG_P2 == g_online_test_enable))
        {
            /* Store TRN */
            g_trn_info.trn[g_trn_info.w_cur] = value;
            ++g_trn_info.w_cur;
            if (ANUM_TRN <= (uint32_t)g_trn_info.w_cur)
            {
                g_trn_info.w_cur = NUM0;
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

        if (ER_D_TRNG_OK == ret_err)
        {
            /* Do nothing */
        }
        else
        {
            /* Online test */
            if (ER_D_TRNG_SDT_FAIL == ret_err)
            {
                /* Toggle buffer */
                g_trn_info.w_cur = (uint8_t)NUM0;
                /* If an online test is retried twice and still NG, the error returns (RNDERR set) */
                if (NUM2 <= g_trng_count)
                {
                    (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_RNDERR, NUM1);
                    g_err_seed = ER_D_TRNG_TRN;
                    complete = ON;
                }
                else
                {
                    ++g_trng_count;

                     /* Wait 1us or more to restart GE due to hardware restriction */
                   (void)D_TRNG_DisableModule();
                    NOPS(400);
                    (void)D_TRNG_EnableModule();
                }
            }
            else
            {
                /* Online test success((RNDERR clear)) */
                (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_RNDERR, NUM0);
                (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_TRNG, NUM1);
                g_err_seed = ER_D_TRNG_OK;
                complete = ON;
            }
        }
    }

    if (ON == complete)
    {
        (void)D_TRNG_DisableModule();

        TRNG_MODE_REG &= ~HTFV_ON;

        g_continueous_test = OFF;
    }
    else
    {
        /* Do nothing */
    }

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_TRNG_0012]:[ICUMFW_UD_S4_D_TRNG_0012]
* Function name: ICICUTRNGHT
* Description  : Process the continuous health test
* Arguments    : N/A
* Return Value : N/A
***************************************************************************************/
void ICICUTRNGHT(void)
{
    /* continuous health test */
    if (D_TRNG_HEALTH_TEST == g_online_test_enable)
    {
        g_continueous_test = ON;
        g_cont_word_num = NUM0;
        g_cont_test_cnt = NUM0;

        if ((GE_OFF == (GE_ON & ICUM_TRNGCR)) && (OHSTART_OFF == (OHSTART_ON & ICUM_TRNGCR)))
        {
            ICUM_TRNGSCLR = (RRDYCLR_ON | HTERRCLR_ON);

            TRNG_MODE_REG |= HTFV_ON;

            D_TRNG_EnableModule();
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
