/******************************************************************************/
/* Component Name   ICUM FLS Library                                          */
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

#if !defined(ICUM_L_FLASH_H)
#define ICUM_L_FLASH_H

/********************************************************************************
 Macro definitions
********************************************************************************/

/* selected API ID definition */
#define AG_L_FLS_API_INIT               (0x00020011UL)
                                                    /* L_FLS_Init */
#define AG_L_FLS_API_ERASE_REQ          (0x00020012UL)
                                                    /* L_FLS_REQ_EraseFlash_ram              */
#define AG_L_FLS_API_WRITE_REQ          (0x00020013UL)
                                                    /* L_FLS_REQ_WriteFlash_ram              */
#define AG_L_FLS_API_UPDATE_CFG_REQ     (0x00020014UL)
                                                    /* L_FLS_REQ_WriteFlash_ram(UpdateConfig) */
#define AG_L_FLS_API_UPDATE_SEC_REQ     (0x00020015UL)
                                                    /* L_FLS_REQ_WriteFlash_ram(UpdateSecurity) */
#define AG_L_FLS_API_CHECK_BLANK_REQ    (0x00020016UL)
                                                    /* L_FLS_REQ_CheckBlank        */
#define AG_L_FLS_API_WRITESECURITY_REQ  (0x00020017UL)
                                                    /* L_FLS_REQ_WriteSecurity_ram        */
#define AG_L_FLS_API_CANCEL_REQ         (0x0002001aUL)
                                                    /* L_FLS_REQ_Cancel_ram             */
#define AG_L_FLS_API_IS_FINISHED        (0x0002001bUL)
                                                    /* L_FLS_IsFinished_ram            */
#define AG_L_FLS_API_OTHER              (0x0002001cUL)
                                                    /* Other API                    */

/* number of uint32_t buffer  */
#define L_FLS_CFGBUFF_NUM               D_FLS_CFGBUFF_NUM

/* OPBT4 related definition */
#define L_FLS_OPBT4_ICUMOPCRF2          D_FLS_OPBT4_ICUMOPCRF2
#define L_FLS_OPBT4_ICUMOPCRF1          D_FLS_OPBT4_ICUMOPCRF1
#define L_FLS_OPBT4_ICUMPEDBG           D_FLS_ICUMPEDBG
#define L_FLS_OPBT4_ICUMOPGPIO          D_FLS_ICUMOPGPIO
#define L_FLS_OPBT4_ICUMHSIF            D_FLS_ICUMHSIF
#define L_FLS_OPBT4_ICUMOPWDEN          D_FLS_ICUMOPWDEN
#define L_FLS_OPBT0_ICUMUPGP1           D_FLS_ICUMUPGP1
#define L_FLS_OPBT0_ICUMUPGP2           D_FLS_ICUMUPGP2

/* Zero definition */
#define L_FLS_ZERO_U32                  ((uint32_t)0)

/* Align check definition */
#define ALIGN_CHK_4                     (0x00000003UL)

/* Align size definition */
#define L_FLS_ALIGN_SIZE                (4uL)

/* Default address value */
#define L_FLS_INIT_ADDR                 (0xFFFFFFFFuL)

/* Security setting offset value */
#define SECURITY_SETTING_OFFSET_ICUMRESV    D_FLS_SEC_SETTING_OFFSET_ICUMRESV
#define SECURITY_SETTING_OFFSET_ICUMRACS0   D_FLS_SEC_SETTING_OFFSET_ICUMRACS0
#define SECURITY_SETTING_OFFSET_ICUMRAD     D_FLS_SEC_SETTING_OFFSET_ICUMRAD
#define SECURITY_SETTING_OFFSET_CHGRESP     D_FLS_SEC_SETTING_OFFSET_CHGRESP
#define SECURITY_SETTING_OFFSET_ICUMRACE0   D_FLS_SEC_SETTING_OFFSET_ICUMRACE0
#define SECURITY_SETTING_OFFSET_ICUMRACS1   D_FLS_SEC_SETTING_OFFSET_ICUMRACS1
#define SECURITY_SETTING_OFFSET_ICUMRACE1   D_FLS_SEC_SETTING_OFFSET_ICUMRACE1

/* 32 bit type flag set/clear */
#define L_FLS_SET_U32                   (1uL)
#define L_FLS_CLR_U32                   (0uL)

/********************************************************************************
 Typedef definitions
********************************************************************************/

/********************************************************************************
 Exported global variables
********************************************************************************/

/********************************************************************************
 Private function prototypes
*********************************************************************************/
static void l_FLS_ReadData_ram(const uint32_t target_addr, const uint32_t data_size_in_bytes, uint8_t *p_data);
static uint32_t l_FLS_SetDfProgReq_ram(const uint8_t usr_df_prog_req);
static uint32_t l_FLS_ChangeErrCode_ram(const uint32_t d_err_code);
static uint32_t l_FLS_GetDfProgReq_ram(uint8_t *p_usr_df_prog_req);
static uint32_t l_FLS_SetWriteSecurityData_ram(const uint32_t target_info, const uint32_t set_data,
                                               uint32_t *p_cfg_data);
#endif /* #if !defined(ICUM_L_FLASH_H) */
