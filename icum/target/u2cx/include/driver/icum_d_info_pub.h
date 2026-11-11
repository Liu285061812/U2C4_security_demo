/******************************************************************************/
/* Component Name   ICUM INFO Driver                                          */
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

#if !defined(ICUM_D_INFO_PUB_H)
#define ICUM_D_INFO_PUB_H

/**************************************************************************************/
/* Return Value Definition                                                            */
/**************************************************************************************/
#define ER_D_INFO_INVALID_STATE (ER_D_INFO_BASE + 1UL)


/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Getting Information ID */
#define INFO_UCID                       (1UL)               /* Unique Chip ID */
#define INFO_UPGP1                      (2UL)               /* UpgradeProtection1 */
#define INFO_UPGP2                      (3UL)               /* UpgradeProtection2 */
#define INFO_RESV                       (4UL)               /* Reset Vector Address */
#define INFO_RACS0                      (5UL)               /* Starting Address of Secure Code Flash area region 0 */
#define INFO_RAD                        (6UL)               /* Starting Address of Secure Data Flash area */
#define INFO_ICUMPEDBG                  (7UL)               /* PEDBG value */
#define INFO_ICUMOPCRF1                 (8UL)               /* C&R Skip/Enable Value of PE */
#define INFO_ICUMOPCRF2                 (9UL)               /* C&R Skip/Enable Value of ICUP */
#define INFO_ICUMOPGPIO                 (10UL)              /* GPIO Control Enable/Disable Value */
#define INFO_CRAUTH_NEED                (11UL)              /* C&R Value of RHSIF Link Partner */
#define INFO_ICUMOPWDVAC                (12UL)              /* WDTA VAC Value */
#define INFO_ICUMOPWDRUN                (13UL)              /* WDTA Starting Mode Value */
#define INFO_ICUMOPWDEN                 (14UL)              /* WDTA Enable/Disable Value */
#define INFO_ICUMOPWDOVF                (15UL)              /* WDTA Count-Lock Value */
#define INFO_ICUMLRAM                   (16UL)              /* ICUM LRAM value */
#define INFO_ICUMPKCCA                  (17UL)              /* PLKCCA RAM value */
#define INFO_RACE0                      (18UL)              /* Ending Address of Secure Code Flash area region 0 */
#define INFO_RACS1                      (19UL)              /* Starting Address of Secure Code Flash area region 1 */
#define INFO_RACE1                      (20UL)              /* Ending Address of Secure Code Flash area region 1 */
#define INFO_GET_INFO_MAX_PARAM         INFO_RACE1          /* MAX param */

/* Firmware name */
#define INFO_FIRMWARE_NAME "ICUMHB_FUL"

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
uint32_t D_INFO_Init(void);
uint32_t D_INFO_GetInfo(uint32_t param, uint32_t *p_info);

#endif /* #if !defined(ICUM_D_INFO_PUB_H) */
