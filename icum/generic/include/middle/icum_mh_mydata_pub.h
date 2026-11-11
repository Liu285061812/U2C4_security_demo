/******************************************************************************/
/* Component Name  MyData Manager                                             */
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

#if !defined(ICUM_MH_MYDATA_PUB_H)
#define ICUM_MH_MYDATA_PUB_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Return Value Definition */
#define ER_MH_MYDATA_SYSCFG_FAILURE         (ER_MH_MYDATA_BASE + (0x00000001uL))
#define ER_MH_MYDATA_DECLARE_FAILURE        (ER_MH_MYDATA_BASE + (0x00000002uL))
#define ER_MH_MYDATA_NO_DATA_SET            (ER_MH_MYDATA_BASE + (0x00000003uL))
#define ER_MH_MYDATA_NOT_MAP_DATA_SLOT      (ER_MH_MYDATA_BASE + (0x00000004uL))
#define ER_MH_MYDATA_INVALID                (ER_MH_MYDATA_BASE + (0x00000005uL))
#define ER_MH_MYDATA_INVALID_IDX            (ER_MH_MYDATA_BASE + (0x00000006uL))
#define ER_MH_MYDATA_BLANK                  (ER_MH_MYDATA_BASE + (0x00000007uL))
#define ER_MH_MYDATA_SIZE                   (ER_MH_MYDATA_BASE + (0x00000008uL))
#define ER_MH_MYDATA_UNAVAILABLE            (ER_MH_MYDATA_BASE + (0x00000009uL))
#define ER_MH_MYDATA_MEM_SOURCE             (ER_MH_MYDATA_BASE + (0x0000000AuL))
#define ER_MH_MYDATA_MEM_DEST               (ER_MH_MYDATA_BASE + (0x0000000BuL))
#define ER_MH_MYDATA_SYS                    (ER_MH_MYDATA_BASE + (0x0000000CuL))
#define ER_MH_MYDATA_SYS_CRITICAL           (ER_MH_MYDATA_BASE + (0x0000000DuL))
#define ER_MH_MYDATA_PROG                   (ER_MH_MYDATA_BASE + (0x0000000EuL))
#define ER_MH_MYDATA_AES_UNAVAILABLE        (ER_MH_MYDATA_BASE + (0x0000000FuL))
#define ER_MH_MYDATA_AES_TIME_OUT           (ER_MH_MYDATA_BASE + (0x00000010uL))
#define ER_MH_MYDATA_AES_PRM_ERROR          (ER_MH_MYDATA_BASE + (0x00000011uL))
#define ER_MH_MYDATA_SYS_ERROR              (ER_MH_MYDATA_BASE + (0x00000012uL))

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
uint32_t MH_MYDATA_Init(void);
uint32_t MH_MYDATA_Get(uint8_t mydata_idx, uint8_t *p_mydata, uint16_t data_size);
uint32_t MH_MYDATA_Update(uint8_t mydata_idx, uint8_t *p_mydata, uint16_t data_size,
                                              r_icumif_job_id_t job_id);

#endif /* #if !defined(ICUM_MH_MYDATA_PUB_H) */
