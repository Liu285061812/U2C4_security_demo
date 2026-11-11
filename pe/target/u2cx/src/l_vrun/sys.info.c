/******************************************************************************/
/* Component Name   Testing the ICU-M                                         */
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

// ----------------------------------------------------------------------
// Package identification and copyright information
//
// ** CAUTION ** CAUTION ** CAUTION ** CAUTION ** CAUTION ** CAUTION **
// Do not update the symbol __PACKAGE_VERSION__
//
// ----------------------------------------------------------------------
#define __PACKAGE_NAME__    "VRUN SYSTEM (SYS)"
#define __PACKAGE_VERSION__ "V0.03"

static const char PACKAGE_INFO[] = __PACKAGE_NAME__ " - " __PACKAGE_VERSION__
#if !defined(RELEASE_PACKAGE)
                                   " (DEBUG) "
#endif
                                   " - BUILD DATE " __DATE__ " " __TIME__ ;

// ********************************************************************************
// Module identification and copyright information
// ********************************************************************************
const char *sys_get_package_info(void)
{
    return(PACKAGE_INFO);
}

