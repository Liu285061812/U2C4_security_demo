/******************************************************************************/
/* Component Name   ICU-M REMAP Driver                                        */
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

#if !defined(ICUM_D_REMAP_PUB_H)
#define ICUM_D_REMAP_PUB_H

/**************************************************************************************/
/* Return Value Definition                                                            */
/**************************************************************************************/
#define ER_D_REMAP_STATE      (ER_D_REMAP_BASE+1U)     /* Not Initialize */
#define ER_D_REMAP_NOTASIGN   (ER_D_REMAP_BASE+2U)     /* Not Empty for Remap Table */
#define ER_D_REMAP_NOREGIST   (ER_D_REMAP_BASE+3U)     /* Not Registered Physical Address */
#define ER_D_REMAP_REGISTERR  (ER_D_REMAP_BASE+4U)     /* Not Used Specified Remap Address */
#define ER_D_REMAP_RETRYOVER  (ER_D_REMAP_BASE+5U)     /* Retry Over at writing Remap Register */
#define ER_D_REMAP_ILLEGALADR (ER_D_REMAP_BASE+6U)     /* Illegal Address */

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* None */

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
uint32_t D_REMAP_Init(void);
uint32_t D_REMAP_Register(uint32_t phys_addr, uint32_t *p_logical_addr);
uint32_t D_REMAP_Unregister(uint32_t logical_addr);
uint32_t D_REMAP_GetPhysAddrs(uint32_t logical_addr, uint32_t *p_phys_addr);
uint32_t D_REMAP_GetLogicalAddrs(uint32_t phys_addr, uint32_t *p_logical_addr);
#if defined(FSW_REMAP_DRIVER)
uint32_t D_REMAP_CheckLogicalAddrs(uint32_t phys_addr);
uint32_t D_REMAP_CheckGetLogicalAddrs(uint32_t phys_addr, uint32_t *p_logical_addr);
#endif

#endif /* ICUM_D_REMAP_PUB_H */
