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

#ifndef _PE_TARGET_DEVICE_H_
#define _PE_TARGET_DEVICE_H_

#define PE_FIRMWARE_NAME           ("ICUMHA_FUL")

#define ADDR_RESV                   (0x00FB0000uL)

#define DF_WRITE_SIZE               (4uL)
#define CF_WRITE_SIZE               (512uL)
#define CF_BLOCK_SIZE               (65536uL)

#define ADDR_DATA_CF                0x00001000uL
#define ADDR_LOWER_CF_BANKA         0x00000000uL
#define ADDR_UPPER_CF_BANKA         0x002FFFFFuL
#define ADDR_LOWER_CF_BANKB         0x00300000uL
#define ADDR_UPPER_CF_BANKB         0x005FFFFFuL
#define ADDR_LOWER_CF_BANKC         0x00800000uL
#define ADDR_UPPER_CF_BANKC         0x00BFFFFFuL
#define ADDR_LOWER_CF_BANKD         0x00C00000uL
#define ADDR_UPPER_CF_BANKD         0x00FFFFFFuL
#define ADDR_LOWER_CF_SECB_DEF      0x00FB0000uL

#define ADDR_IRREGULAR_DATA         0x01000000uL

#define ADDR_LOWER_C0_RAM           0xFE000000uL
#define ADDR_UPPER_C0_RAM           0xFE057FFFuL
#define ADDR_LOWER_R0_RAM           0xFE058000uL
#define ADDR_UPPER_R0_RAM           0xFE05FFFFuL

#define ADDR_LOWER_RRAM             ADDR_LOWER_R0_RAM
#define ADDR_LOWER_CRAM             ADDR_LOWER_C0_RAM
#define ADDR_UPPER_CRAM             ADDR_UPPER_R0_RAM
#define ADDR_LOWER_LRAM             0xFDC00000uL
#define ADDR_LOWER_SLRAM            0xFDE00000uL
#define ADDR_UPPER_SLRAM            0xFDE0FFFFuL

#define ADDR_LOWER_DF_BANKA         0xFF200000uL
#define ADDR_UPPER_DF_BANKA         0xFF20FFFFuL
#define ADDR_LOWER_DF_BANKB         0xFF210000uL
#define ADDR_UPPER_DF_BANKB         0xFF21FFFFuL
#define ADDR_LOWER_DF_SDF_DEF       0xFF220000uL
#define ADDR_UPPER_DF_SDF           0xFF22FFFFuL

#define LOWER_LIMIT_SCF             ADDR_LOWER_CF_SECB_DEF
#define LOWER_LIMIT_DF              ADDR_LOWER_DF_BANKA
#define LOWER_LIMIT_SDF             ADDR_LOWER_DF_SDF_DEF

#define LOWER_LIMIT_GRAM            ADDR_LOWER_C0_RAM
#define UPPER_LIMIT_GRAM            ADDR_UPPER_R0_RAM

extern uint8_t  __ghsbegin_start_rRAM[];        /* Start address ( retention RAM ) */

#define d_SYSERR_LOG_ADDR   (__ghsbegin_start_rRAM)
#define d_TIMER_LOG_ADDR    (__ghsbegin_start_rRAM + 1)

#endif /*_PE_TARGET_DEVICE_H_ */
