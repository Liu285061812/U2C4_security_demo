/******************************************************************************/
/* Component Name   ICU-M Service                                             */
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

#ifndef ICUM_S_00_COMMON_H
#define ICUM_S_00_COMMON_H

/*******************************************************************************/
/* Defines                                                                     */
/*******************************************************************************/
/* Service 00 Error definition */
#define ER_NVM_SIZE_ERROR          (0x00000005uL)
#define ER_SYS_CFG_NOT_INITIALIZED (0x00000006uL)
#define ER_UNAVAILABLE             (0x00000007uL)
#define ER_DEST_MEM_ERROR          (0x00000008uL)
#define ER_SRC_MEM_ERROR           (0x00000009uL)
#define ER_FW_ACTIVATION_FAILED    (0x0000000AuL)
#define ER_CR_VERIFY_FAIL          (0x0000000BuL)
#define ER_S_00_SYS                (0x0000000CuL)
#define ER_INVALID_KEY_GROUP       (0x0000000DuL)
#define ER_CLUSTER_INSTALL_FAIL    (0x0000000FuL)
#define ER_KEY_NOT_USABLE          (0x00000010uL)
#define ER_RESTRICTED              (0x00000011uL)
#define ER_INVALID_KEY_INDEX       (0x00000012uL)
#define ER_SEQUENCE                (0x00000013uL)

#define ON_HOLD   (0u)
#define PHASE_1   (1u)
#define PHASE_2   (2u)
#define PHASE_3   (3u)
#define PHASE_4   (4u)
#define PHASE_5   (5u)
#define PHASE_6   (6u)
#define PHASE_7   (7u)
#define PHASE_8   (8u)

#define KEY_ID_0  (0u)
#define KEY_ID_1  (1u)
#define KEY_ID_2  (2u)

#define CHIP_SIZE      (4)
#define FW_NAME_SIZE   (16)
#define CUSTOMER_SIZE  (32)

#define LOOP_OFF       (0u)
#define LOOP_ON        (1u)

/* system configuration items is for VM */
#define CFG_FW         (0u)
#define CFG_VM         (1u)

#define UID_SIZE       (0x000Fu)

#define LICENSE_ID_OTA (0x04u)
#define LICENSE_ID_IDX (30u)

#define INDEX_NONE     (0xFFFFu)

/*******************************************************************************/
/* Prototypes                                                                  */
/*******************************************************************************/
uint32_t s_00_ConvertErr(uint32_t err);

#endif /* ICUM_S_00_COMMON_H */
