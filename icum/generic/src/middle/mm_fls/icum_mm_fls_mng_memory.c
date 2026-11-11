/******************************************************************************/
/* Component Name  Flash Manager  memory related                              */
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
/*******************************************************************************
* Header files
********************************************************************************/
#include "renesas_types.h"
#include "icum_d_misc_pub.h"
#include MM_FLASH_PUBLIC_H
/* For used defines of issd kernel. */
#include    "icum_k_issd_pub.h"

/*******************************************************************************
* Local defines
********************************************************************************/

/*******************************************************************************
* Type definition
********************************************************************************/

/*******************************************************************************
* Local variables
********************************************************************************/

/*******************************************************************************
* Functions
********************************************************************************/

CHANGE_SEC(text, ".icum_middle.text")
CHANGE_SEC(rodata, ".icum_middle.rodata")

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0067]:[ICUMFW_UD_MM_FlashMNG_0067]
* Function name: MM_FLS_IsChangeCFPE
* Description  : Check to see if switching to CF_PE MODE
* Arguments    : (out) p_result   : check result
*                                    = FLS_CHK_OK : can switch to CF_P/E mode.
*                                    = FLS_CHK_NG : cannot switch to CF_P/E mode.
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MM_FLS_IsChangeCFPE(uint8_t *p_result)
{
    /* This function is unnecessary and deleted processing contents.*/
    *p_result = FLS_CHK_OK;
    return (ER_OK);
}

CHANGE_SEC(text, ".icum_middle_ram")
CHANGE_SEC(rodata, ".icum_middle_const_ram")

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0068]:[ICUMFW_UD_MM_FlashMNG_0068]
* Function name: MM_FLS_GetMemBoundary
* Description  : retrieve the address boundary of the memory region specified
* Arguments    : boundary       : The specified boundary
*                p_addr         : Retrieve the boundary address
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MM_FLS_GetMemBoundary(const uint8_t boundary, uint32_t *p_addr)
{
    uint32_t err;

    err = L_FLS_GetMemBoundary(boundary, p_addr);

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0064]:[ICUMFW_UD_MM_FlashMNG_0064]
* Function name: MM_FLS_CheckMemory
* Description  : Check of memory address
* Arguments    : start_addr     : Address to be checked
*                size           : Data size
*                direction      : Direction to check
*                p_result       : Check result
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MM_FLS_CheckMemory(const uint32_t start_addr, const uint32_t size, const uint8_t direction,
                            uint8_t ex_service, uint8_t *p_result)
{
    uint32_t err;

    if ((uint32_t)FROM_NATIVE_SERVICE == (uint32_t)ex_service)
    {
        err = L_FLS_CheckMemory(start_addr, size, direction, p_result);
    }
    else
    {
        err = L_FLS_CheckMemoryCISM(start_addr, size, direction, p_result);
    }

    return (err);
}

CHANGE_SEC(text, ".icum_middle.text")
CHANGE_SEC(rodata, ".icum_middle.rodata")

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0071]:[ICUMFW_UD_MM_FlashMNG_0071]
* Function name: MM_FLS_CheckMemoryUserBoot
* Description  : Check of memory address
* Arguments    : start_addr     : Address to be checked
*                size           : Data size
*                direction      : Direction to check
*                p_result       : Check result
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MM_FLS_CheckMemoryUserBoot(uint32_t start_addr, uint32_t size, uint8_t direction, uint8_t *p_result)
{
    uint32_t err;

    err = L_FLS_CheckMemoryUserBoot(start_addr, size, direction, p_result);

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0072]:[ICUMFW_UD_MM_FlashMNG_0072]
* Function name: MM_FLS_CheckMemoryCISMUserBoot
* Description  : Check of memory address
* Arguments    : start_addr     : Address to be checked
*                size           : Data size
*                direction      : Direction to check
*                p_result       : Check result
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MM_FLS_CheckMemoryCISMUserBoot(uint32_t start_addr, uint32_t size, uint8_t direction, uint8_t *p_result)
{
    uint32_t err;

    err = L_FLS_CheckMemoryCISMUserBoot(start_addr, size, direction, p_result);

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0073]:[ICUMFW_UD_MM_FlashMNG_0073]
* Function name: MM_FLS_IsUserBootArea
* Description  : Check of memory address for UserBoot or not
* Arguments    : start_addr     : Address to be checked
*                size           : Data size
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MM_FLS_IsUserBootArea(const uint32_t start_addr, const uint32_t size)
{
    uint32_t err;

    err = L_FLS_IsUserBootArea(start_addr, size);

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0066]:[ICUMFW_UD_MM_FlashMNG_0066]
* Function name: L_FLS_CheckMemoryCISM
* Description  : Check of memory source address for CISM
* Arguments    : start_addr     : address to be checked
*                size           : data size
*                direction      : direction to check
*                p_result       : check result
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MM_FLS_CheckMemoryCISM(const uint32_t start_addr, const uint32_t size, const uint8_t direction,
                                uint8_t *p_result)
{
    uint32_t err;

    err = L_FLS_CheckMemoryCISM(start_addr, size, direction, p_result);

    return (err);
}

/* ----- end of source code file ----- */
