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
#include "icum_mh_key_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_s_03_common.h"
#include "icum_s_pub.h"
#include "icum_service_common.h"
/* For call functions of remap driver. */
#include "icum_d_remap_pub.h"

/**********************************************************************************
* Functions
**********************************************************************************/

/**********************************************************************************
* ICU-M service
**********************************************************************************/

/*===========================================================================
  Import HMAC
  Command parameters extracted from the ICU-M service descriptor (ISD)
  [PUBLIC]
  ===========================================================================*/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_HMAC_IMPORT_0001]:[ICUMFW_UD_S_03_HMAC_IMPORT_0001]
* Function name: S_03_ImportHmacKey()
* Description  : # Acquire the HMAC key with the specified Key_id.
*                  If it is successfully acquired, if the key slot is empty,
*                  register with the given information.
* Arguments    : (IN-OUT)  p_job : Pointer to HMAC Import job control parameter.
* Return Value : None
****************************************************************************************/

void S_03_ImportHmacKey(icum_job_t *p_job)
{

    r_icumif_isd_t     *p_ISD;
    hmac_key_t          p_new_key;
    uint32_t            err;
    uint8_t             result;
    uint32_t            skip_flg = FLG_OFF;
    remap_ptr_manip_t   key_logical;

    /* retrieve the service handler */
    p_ISD = p_job->p_ISD;

    /* in process */
    p_ISD->service_result = SERV_OK;
    if ((p_ISD->prm.HMAC_IMPORT.key_size_in_bytes == (uint8_t)0)
     || (p_ISD->prm.HMAC_IMPORT.key_size_in_bytes > MH_KEY_HMAC_KEY_SIZE_IN_BYTES))
    {
        p_ISD->service_result = SERV_PRM_ERROR;
        skip_flg = FLG_ON;
    }
    else
    {
        /* do nothing */
    }

    if(skip_flg == FLG_OFF)
    {
        if (p_job->expansion_service == FROM_NATIVE_SERVICE)
        {
            /* make sure the source address of the HMAC KEY is valid */
            err = MM_FLS_CheckMemory((uint32_t)p_ISD->prm.HMAC_IMPORT.p_key,
                                    p_ISD->prm.HMAC_IMPORT.key_size_in_bytes,
                                    FLS_CHK_MEM_SOURCE,
                                    p_job->expansion_service,
                                    &result);
        }
        else
        {
            /* make sure the source address of the HMAC KEY is valid */
            err = MM_FLS_CheckMemoryCISM((uint32_t)p_ISD->prm.HMAC_IMPORT.p_key,
                                        p_ISD->prm.HMAC_IMPORT.key_size_in_bytes,
                                        FLS_CHK_MEM_SOURCE,
                                        &result);
        }

        if (err != ER_OK)
        {
            p_ISD->service_result = SERV_PRM_ERROR;
            skip_flg = FLG_ON;
        }
        else
        {
            if (result != FLS_CHK_OK)
            {
                p_ISD->service_result = SERV_SRC_MEM_ERROR;
                skip_flg = FLG_ON;
            }
            else
            {
                /* do nothing */
            }
        }

        if(skip_flg == FLG_OFF)
        {
            /* Get Remap(Convert Physical address -> Logical Address) */
            (void)D_REMAP_GetLogicalAddrs((uint32_t)p_ISD->prm.HMAC_IMPORT.p_key, &key_logical.val);

            p_new_key.key_id                   = p_ISD->prm.HMAC_IMPORT.key_id;
            p_new_key.key_size_in_byte         = p_ISD->prm.HMAC_IMPORT.key_size_in_bytes;
            p_new_key.p_key                    = (void *)key_logical.p_ptr;

            err = MH_KEY_SetHmacKey(p_job, &p_new_key);

            if (err != ER_OK)
            {
                (void)s_03_ConvertErr(err, (r_icumif_service_result_t *)&p_ISD->service_result);
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
}
