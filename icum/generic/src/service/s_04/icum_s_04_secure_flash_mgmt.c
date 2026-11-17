/******************************************************************************/
/* Component Name   ICU-M Service 04 garbage collection of flash              */
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
/* For ICU-M service descriptor (ISD). */
#include "r_icumif_api.h"
/* For variable typedef. */
#include "renesas_types.h"
/* For the common error codes. */
#include "icum_common_pub.h"
/* For call functions of flash manager. */
#include MM_FLASH_PUBLIC_H
/* For service function declaration. */
#include "icum_s_pub.h"
/* For call common error code conversion function. */
#include "../s_00/icum_s_00_common.h"

/**********************************************************************************
* Structure
***********************************************************************************/

/**********************************************************************************
* Variable
***********************************************************************************/

/**********************************************************************************
* Function
***********************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_04_FLASH_PROG_0022]:[ICUMFW_UD_S_04_FLASH_PROG_0022]
* Function name: S_04_CollectGarbage
* Description  :
* Arguments    : p_job
* Return Value : N/A
***************************************************************************************/
void S_04_CollectGarbage(icum_job_t *p_job)
{
    if ((NULL != p_job) && (NULL != p_job->p_ISD))
    {
        p_job->p_ISD->service_result = (r_icumif_service_result_t)ER_UNAVAILABLE;
    }
}

