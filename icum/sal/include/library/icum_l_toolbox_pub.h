/******************************************************************************/
/* Component Name   Library Toolbox public header file                        */
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

#if !defined(ICUM_L_TOOLBOX_PUB_H)
#define ICUM_L_TOOLBOX_PUB_H

/**********************************************************************************
* Headers
***********************************************************************************/

#include "renesas_types.h"
#include "icum_common_pub.h"

/********************************************************************************
 Macro definitions
*******************************************************************************/
#define ER_L_TOOLBOX_SYS_ERROR      (ER_L_TOOLBOX_BASE | (0x00000001uL))

/**********************************************************************************
* Function prototypes
***********************************************************************************/

uint32_t L_Toolbox_ByteSwap(uint32_t myword);

void     L_Toolbox_MemClr64(uint64_t *dest64, int32_t size64);

void     L_Toolbox_MemSet(uint8_t *dest, uint8_t mychar, int16_t size);
void     L_Toolbox_MemCpy(uint8_t *dest, const uint8_t *source, int16_t size);
uint32_t L_Toolbox_MemCpy_Sec(uint8_t *dest, const uint8_t *source, int16_t size);
uint8_t  L_Toolbox_MemCmp(const uint8_t *data1, const uint8_t *data2, int16_t size);

uint32_t L_Toolbox_StrLen(const char *s);
void     L_Toolbox_Strcpy(char *s1, const char *s2);
int32_t  L_Toolbox_Strcmp(const char *s1, const char *s2);

void    R_RH850_MemSet(uint8_t *dest, uint8_t mychar, int16_t size);
void    R_RH850_MemCpy(uint8_t *dest, const uint8_t *source, int16_t size);
uint8_t R_RH850_MemCmp(const uint8_t *data1, const uint8_t *data2, int16_t size);

#endif /* #if !defined(ICUM_L_TOOLBOX_H) */
