/******************************************************************************/
/* Component Name   ICU-M Interface Library User Flash                        */
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
#if !defined(R_ICUMIF_API_USER_FLASH_H)
#define R_ICUMIF_API_USER_FLASH_H

/**********************************************************************************
* Include
***********************************************************************************/

/**********************************************************************************
* Define
***********************************************************************************/
/* Flash memory operation types. */
typedef enum
{
    E_FLASH_READ,
    E_FLASH_ERASE,
    E_FLASH_WRITE,
    E_FLASH_BLANK_CHECK,
    E_FLASH_INIT_CR_NONE,
    E_FLASH_INIT_CR_CHALLENGE,
    E_FLASH_INIT_CR_RESPONSE,
    E_FLASH_END,
    E_FLASH_SHIFT_TO_RAM,

    LAST_ITEM_FLASH_OP = 0x01000000
} r_flash_op_t;

/* Service parameters of user flash management */
typedef struct ISD_USER_FLASH_MGMT
{
    /* Flash memory operation types. */
    r_flash_op_t        flash_op;
    /* Start address of flash memory to be processed. */
    union
    {
        uint8_t        *p_flash;
        uint64_t        padding_sys64_1;
    };
    /* Data storage address. */
    union
    {
        uint8_t        *p_buffer;
        uint64_t        padding_sys64_2;
    };
    /* Number of bytes of data size to be processed. */
    uint32_t            data_size_in_bytes;
    /* Result of Blank check. */
    volatile uint8_t    is_blank;
} ISD_USER_FLASH_MGMT_t;

/**********************************************************************************
* Structure
***********************************************************************************/

/**********************************************************************************
* Variable
***********************************************************************************/

/**********************************************************************************
* Function
***********************************************************************************/

#endif /* #if !defined(R_ICUMIF_API_USER_FLASH_H) */
