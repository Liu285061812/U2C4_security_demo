/******************************************************************************/
/* Component Name   s_04_Common header file                                   */
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

#ifndef ICUM_S_04_COMMON_H
#define ICUM_S_04_COMMON_H

/**********************************************************************************
* Include
***********************************************************************************/

/**********************************************************************************
* Define
***********************************************************************************/
/* Error code */
/* Service 04 error base value */
#define ER_S_04_BASE            0x04000000uL
/* Destination error */
#define ER_S_04_DEST_MEM        (ER_S_04_BASE + 0x00000001uL)
/* Source error */
#define ER_S_04_SRC_MEM         (ER_S_04_BASE + 0x00000002uL)
/* Sequence error */
#define ER_S_04_SEQUENCE        (ER_S_04_BASE + 0x00000003uL)
/* Unavailable */
#define ER_S_04_UNAVAILABLE     (ER_S_04_BASE + 0x00000004uL)
/* Challenge and response verify fail */
#define ER_S_04_CR_VERIFY_FAIL  (ER_S_04_BASE + 0x00000005uL)
/* System error */
#define ER_S_04_SYS             (ER_S_04_BASE + 0x00000006uL)

/* Data size */
/* No fraction */
#define NO_FRACTION             0x00000000uL
/* Data size zero */
#define DATA_SIZE_ZERO          0x00000000uL

/**********************************************************************************
* Structure
***********************************************************************************/

/**********************************************************************************
* Variable
***********************************************************************************/

/**********************************************************************************
* Function
***********************************************************************************/
uint32_t s_04_ConvertError(volatile r_icumif_service_result_t *p_service_result, uint32_t exe_result);

#endif /* end of #ifndef ICUM_S_04_COMMON_H */
