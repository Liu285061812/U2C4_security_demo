/******************************************************************************/
/* Component Name   ABTR (HW Resource MNG)                                    */
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

/* Header files                     */
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_ost_pub.h"
#include "icum_ml_abtr_api_pub.h"

#include "icum_ml_abtr.h"

/****************************************************************************************
* Global Variable                                                                       *
****************************************************************************************/
/* Prototype declaration */
static void ml_ABTR_Timeover_AES(void);

/* ABTR HW resource management table */
static struct ABTR_RESOURCE_MNG_TBL_t g_abtr_resource_mng[ABTR_RESOURCE_TBL_NUM] =
{
    /* type, state, ID, timer id, ABTR callback func */
    {ABTR_RESOURCE_STS_RELEASED, 0UL, 0U, &ml_ABTR_Timeover_AES}  /* ABTR_RESOURCE_TYPE_AES */
};

/* ABTR Init flag */
static uint32_t g_abtr_init = ABTR_INITIALIZED_NOT_INIT;

/****************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ABTR_0001]:[ICUMFW_UD_ML_ABTR_0001]
* Function name: ML_ABTR_Init
* Description  : Init processing.
* Arguments    : none
* Return Value : ER_OK : Success.
****************************************************************************************/
uint32_t ML_ABTR_Init(void)
{
    /* Loop index */
    uint32_t idx;
    
    /* 1. ABTR initialized. */
    for (idx = 0UL; ABTR_RESOURCE_TBL_NUM > idx; idx++)
    {
        /* 1-1. Resource status initialization */
        g_abtr_resource_mng[idx].release_state = ABTR_RESOURCE_STS_RELEASED;
        /* 1-2. Resource ID clear */
        g_abtr_resource_mng[idx].resource_id   = 0UL;
        /* 1-3. Timer ID clear */
        g_abtr_resource_mng[idx].timer_id      = 0U;
    }
    
    /* 2. ABTR initialized flag set */
    g_abtr_init = ABTR_INITIALIZED_FLAG;
    
    /* 3. Set return value is ER_OK */
    return (ER_OK);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ABTR_0002]:[ICUMFW_UD_ML_ABTR_0002]
* Function name: ML_ABTR_GetResource
* Description  : Get Resource.
* Arguments    : (IN)  resource_type  : Resource Type. (AES)
*                (OUT) *p_resource_id : Resource ID. From 1 to unsigned int max.
* Return Value : ER_OK                      : Success.
*                ER_ML_ABTR_EMPTY           : No empty resources.
*                ER_ML_ABTR_NOT_RELEASED    : Resource not released.
*                ER_ML_ABTR_NOT_INITIALIZED : ABTR not initialized.
*                ER_ML_ABTR_TIMER_SET_FAILURE : OST DRV registration failure.
*                ER_ML_ABTR_SYS_ERROR       : System error.
****************************************************************************************/
#pragma ghs section text    = ".icum_middle_ram"
#pragma ghs section rodata  = ".icum_middle_const_ram"
uint32_t ML_ABTR_GetResource(uint32_t resource_type, uint32_t *p_resource_id)
{
    /* Return value */
    uint32_t rtn;
    /* Resource table index */
    uint32_t idx;
    uint32_t saved_psw;
    /* OST DRV return Timer ID */
    uint8_t  tid;
    /* OST DRV callback i/f */
    s_OST_set_SoftTimer_t callback_info;
    
    /* 1. Check resource type */
    if (ABTR_RESOURCE_TYPE_AES == resource_type)
    {
        /* 1-1. Resource type is AES */
        idx = ABTR_RESOURCE_TBL_AES;
    }
    else
    {
        /* 1-2. Other Resource type */
        idx = ABTR_RESOURCE_TBL_NUM;
    }
    
    /* 2. Check resource index (resource type) */
    if (ABTR_RESOURCE_TBL_NUM > idx)
    {
        /* 3. Check resource ID address pointer of parameter */
        if (NULL != p_resource_id)
        {
            /* 4. Resource ID initial value */
            *p_resource_id = 0UL;
            /* 5. Check ABTR initialized */
            if (ABTR_INITIALIZED_FLAG == g_abtr_init)
            {
                /* 6. Disable interrupts (if not already disabled) */
                DISABLE_INTERRUPT_WITH_CHECK(saved_psw);
                
                /* 7. Check resource status is released */
                if (ABTR_RESOURCE_STS_RELEASED == g_abtr_resource_mng[idx].release_state)
                {
                    /* 7-1. OST DRV API call */
                    callback_info.ovf        = OST_MODE_ONESHOT;
                    callback_info.time_ms    = ABTR_RELEASE_OVER_TIME;
                    callback_info.p_callback = g_abtr_resource_mng[idx].p_abtr_func;
                    /* 7-2. Check OST DRV SoftTimer allocate */
                    if (ER_OK == D_OST_AllocateTimer(&callback_info, &tid))
                    {
                        /* 7-3. Set resource status */
                        g_abtr_resource_mng[idx].release_state = ABTR_RESOURCE_STS_ACQUIRED;
                        /* 7-4. Increment resource ID */
                        g_abtr_resource_mng[idx].resource_id++;
                        /* 7-5. Check Resource ID overflow */
                        if (0UL == g_abtr_resource_mng[idx].resource_id)
                        {
                            /* 7-5-1. Resource ID initialization */
                            g_abtr_resource_mng[idx].resource_id = 1UL;
                        }
                        else
                        {
                            /* Do nothing */
                        }
                        /* 7-6. Set output value is resource ID */
                        *p_resource_id = g_abtr_resource_mng[idx].resource_id;
                        /* 7-7. Set timer id */
                        g_abtr_resource_mng[idx].timer_id = tid;
                        /* 7-8. OST DRV SoftTimer start */
                        D_OST_StartTimer(tid);
                        /* 7-9. Set return value is OK */
                        rtn = ER_OK;
                    }
                    else
                    {
                        /* 7-2-1. Set return value is OST DRV error */
                        rtn = ER_ML_ABTR_TIMER_SET_FAILURE;
                    }
                }
                /* 8. Check resource status is acquired */
                else if (ABTR_RESOURCE_STS_ACQUIRED == g_abtr_resource_mng[idx].release_state)
                {
                    /* 7-1. Set return value is No empty resources */
                    rtn = ER_ML_ABTR_EMPTY;
                }
                /* 9. Check resource status is timeover */
                else
                {
                    /* 9-1. Set return value is resource not released */
                    rtn = ER_ML_ABTR_NOT_RELEASED;
                }
                
                /* 10. Re-enable interrupts (if necessary) */
                ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
            }
            else
            {
                /* 5-1. Set return value is ABTR not initialized */
                rtn = ER_ML_ABTR_NOT_INITIALIZED;
            }
        }
        else
        {
            /* 3-1. Set return value is parameter error */
            /* Resource ID pointer is NULL */
            rtn = ER_ML_ABTR_SYS_ERROR;
        }
    }
    else
    {
        /* 2-1. Set return value is parameter error */
        /* Resource Type not found */
        rtn = ER_ML_ABTR_SYS_ERROR;
    }

    return (rtn);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ABTR_0003]:[ICUMFW_UD_ML_ABTR_0003]
* Function name: ML_ABTR_FreeResource
* Description  : Free Resource.
* Arguments    : (IN) resource_type : Resource Type. (AES)
*                     resource_id   : Resource ID. From 1 to unsigned int max.
* Return Value : ER_OK                      : Success.
*                ER_ML_ABTR_INVALID_ID      : Invalid resource ID.
*                ER_ML_ABTR_NOT_INITIALIZED : ABTR not initialized.
*                ER_ML_ABTR_SYS_ERROR       : System error.
****************************************************************************************/
uint32_t ML_ABTR_FreeResource(uint32_t resource_type, uint32_t resource_id)
{
    /* Return value */
    uint32_t rtn;
    /* Resource table index */
    uint32_t idx;
    uint32_t saved_psw;

    /* 1. Check resource type */
    if (ABTR_RESOURCE_TYPE_AES == resource_type)
    {
        /* 1-1. Resource type is AES */
        idx = ABTR_RESOURCE_TBL_AES;
    }
    else
    {
        /* 1-2. Other Resource type */
        idx = ABTR_RESOURCE_TBL_NUM;
    }
    
    /* 2. Check resource index */
    if (ABTR_RESOURCE_TBL_NUM > idx)
    {
        /* 3. Check ABTR initialized */
        if (ABTR_INITIALIZED_FLAG == g_abtr_init)
        {
            /* 4. Disable interrupts (if not already disabled) */
            DISABLE_INTERRUPT_WITH_CHECK(saved_psw);
            
            /* 5. Check resource ID */
            if (resource_id == g_abtr_resource_mng[idx].resource_id)
            {
                /* 6. Check resource status is released */
                if (ABTR_RESOURCE_STS_RELEASED == g_abtr_resource_mng[idx].release_state)
                {
                    /* 6-1. Set return value is invalid resource ID */
                    /* Resource id already released */
                    rtn = ER_ML_ABTR_INVALID_ID;
                }
                /* 7. Resource status is not released */
                else
                {
                    /* 7-1. OST DRV SoftTimer stop */
                    D_OST_StopTimer(g_abtr_resource_mng[idx].timer_id);
                    /* 7-2. OST DRV SoftTimer release */
                    D_OST_ReleaseTimer(g_abtr_resource_mng[idx].timer_id);
                    /* 7-3. Resource id release */
                    g_abtr_resource_mng[idx].release_state = ABTR_RESOURCE_STS_RELEASED;
                    g_abtr_resource_mng[idx].timer_id      = 0U;
                    /* 7-4. Set return value is OK */
                    rtn = ER_OK;
                }
            }
            else
            {
                /* 5-1. Set return value is invalid resource ID */
                rtn = ER_ML_ABTR_INVALID_ID;
            }
            
            /* 8. Re-enable interrupts (if necessary) */
            ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
        }
        else
        {
            /* 3-1. Set return value is ABTR not initialized */
            rtn = ER_ML_ABTR_NOT_INITIALIZED;
        }
    }
    else
    {
        /* 2-1. Set return value is parameter error */
        /* Resource Type not found */
        rtn = ER_ML_ABTR_SYS_ERROR;
    }
    
    return (rtn);
}
#pragma ghs section text    = default
#pragma ghs section rodata  = default

/****************************************************************************************
* Function ID  : [ICUMFW_CD_ML_ABTR_0004]:[ICUMFW_UD_ML_ABTR_0004]
* Function name: ml_ABTR_Timeover_AES
* Description  : AES resource released timeover
* Arguments    : none
* Return Value : none
****************************************************************************************/
static void ml_ABTR_Timeover_AES(void)
{
    /* 1. Check resource status is acquired */
    if (ABTR_RESOURCE_STS_ACQUIRED == g_abtr_resource_mng[ABTR_RESOURCE_TBL_AES].release_state)
    {
        /* 2. Set resource status */
        g_abtr_resource_mng[ABTR_RESOURCE_TBL_AES].release_state = ABTR_RESOURCE_STS_TIMEOVER;
    }
    else
    {
        /* Do nothing */
    }

    return;
}

