/******************************************************************************/
/* Component Name   ICU-M Interface Library (ICUMIF)                          */
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
#if !defined(R_ICUMIF_API_SHE_H)
#define R_ICUMIF_API_SHE_H

/**********************************************************************************/
/* SHE specific parameters                                                        */
/**********************************************************************************/

/*===========================================================================*/
/* SHE key index                                                             */
/*===========================================================================*/
typedef enum
{
    SHE_SECRET_KEY,         /* SHE key index : 0 */
    SHE_MASTER_ECU_KEY,     /* SHE key index : 1 */
    SHE_BOOT_MAC_KEY,       /* SHE key index : 2 */
    SHE_BOOT_MAC,           /* SHE key index : 3 */
    SHE_KEY_1,              /* SHE key index : 4 */
    SHE_KEY_2,              /* SHE key index : 5 */
    SHE_KEY_3,              /* SHE key index : 6 */
    SHE_KEY_4,              /* SHE key index : 7 */
    SHE_KEY_5,              /* SHE key index : 8 */
    SHE_KEY_6,              /* SHE key index : 9 */
    SHE_KEY_7,              /* SHE key index : 10 */
    SHE_KEY_8,              /* SHE key index : 11 */
    SHE_KEY_9,              /* SHE key index : 12 */
    SHE_KEY_10,             /* SHE key index : 13 */
    SHE_RAM_KEY,            /* SHE key index : 14 */
    SHE_KEY_11,             /* SHE key index : 15 */
    SHE_KEY_12,             /* SHE key index : 16 */
    SHE_KEY_13,             /* SHE key index : 17 */
    SHE_KEY_14,             /* SHE key index : 18 */
    SHE_KEY_15,             /* SHE key index : 19 */
    SHE_KEY_16,             /* SHE key index : 20 */
    SHE_KEY_17,             /* SHE key index : 21 */
    SHE_KEY_18,             /* SHE key index : 22 */
    SHE_KEY_19,             /* SHE key index : 23 */
    SHE_KEY_20,             /* SHE key index : 24 */

    SHE_KEY_21,             /* SHE key index : 25 */
    SHE_KEY_22,             /* SHE key index : 26 */
    SHE_KEY_23,             /* SHE key index : 27 */
    SHE_KEY_24,             /* SHE key index : 28 */
    SHE_KEY_25,             /* SHE key index : 29 */
    SHE_KEY_26,             /* SHE key index : 30 */
    SHE_KEY_27,             /* SHE key index : 31 */
    SHE_KEY_28,             /* SHE key index : 32 */
    SHE_KEY_29,             /* SHE key index : 33 */
    SHE_KEY_30,             /* SHE key index : 34 */
    SHE_KEY_31,             /* SHE key index : 35 */
    SHE_KEY_32,             /* SHE key index : 36 */
    SHE_KEY_33,             /* SHE key index : 37 */
    SHE_KEY_34,             /* SHE key index : 38 */
    SHE_KEY_35,             /* SHE key index : 39 */
    SHE_KEY_36,             /* SHE key index : 40 */
    SHE_KEY_37,             /* SHE key index : 41 */
    SHE_KEY_38,             /* SHE key index : 42 */
    SHE_KEY_39,             /* SHE key index : 43 */
    SHE_KEY_40,             /* SHE key index : 44 */
    SHE_KEY_41,             /* SHE key index : 45 */
    SHE_KEY_42,             /* SHE key index : 46 */
    SHE_KEY_43,             /* SHE key index : 47 */
    SHE_KEY_44,             /* SHE key index : 48 */
    SHE_KEY_45,             /* SHE key index : 49 */
    SHE_KEY_46,             /* SHE key index : 50 */
    SHE_KEY_47,             /* SHE key index : 51 */
    SHE_KEY_48,             /* SHE key index : 52 */
    SHE_KEY_49,             /* SHE key index : 53 */
    SHE_KEY_50,             /* SHE key index : 54 */
    SHE_KEY_51,             /* SHE key index : 55 */
    SHE_KEY_52,             /* SHE key index : 56 */
    SHE_KEY_53,             /* SHE key index : 57 */
    SHE_KEY_54,             /* SHE key index : 58 */
    SHE_KEY_55,             /* SHE key index : 59 */
    SHE_KEY_56,             /* SHE key index : 60 */
    SHE_KEY_57,             /* SHE key index : 61 */
    SHE_KEY_58,             /* SHE key index : 62 */
    SHE_KEY_59,             /* SHE key index : 63 */
    SHE_KEY_60,             /* SHE key index : 64 */
    SHE_KEY_61,             /* SHE key index : 65 */
    SHE_KEY_62,             /* SHE key index : 66 */
    SHE_KEY_63,             /* SHE key index : 67 */
    SHE_KEY_64,             /* SHE key index : 68 */
    SHE_KEY_65,             /* SHE key index : 69 */
    SHE_KEY_66,             /* SHE key index : 70 */
    SHE_KEY_67,             /* SHE key index : 71 */
    SHE_KEY_68,             /* SHE key index : 72 */
    SHE_KEY_69,             /* SHE key index : 73 */
    SHE_KEY_70,             /* SHE key index : 74 */
    SHE_KEY_71,             /* SHE key index : 75 */
    SHE_KEY_72,             /* SHE key index : 76 */
    SHE_KEY_73,             /* SHE key index : 77 */
    SHE_KEY_74,             /* SHE key index : 78 */
    SHE_KEY_75,             /* SHE key index : 79 */
    SHE_KEY_76,             /* SHE key index : 80 */
    SHE_KEY_77,             /* SHE key index : 81 */
    SHE_KEY_78,             /* SHE key index : 82 */
    SHE_KEY_79,             /* SHE key index : 83 */
    SHE_KEY_80,             /* SHE key index : 84 */
    SHE_KEY_81,             /* SHE key index : 85 */
    SHE_KEY_82,             /* SHE key index : 86 */
    SHE_KEY_83,             /* SHE key index : 87 */
    SHE_KEY_84,             /* SHE key index : 88 */
    SHE_KEY_85,             /* SHE key index : 89 */
    SHE_KEY_86,             /* SHE key index : 90 */
    SHE_KEY_87,             /* SHE key index : 91 */
    SHE_KEY_88,             /* SHE key index : 92 */
    SHE_KEY_89,             /* SHE key index : 93 */
    SHE_KEY_90,             /* SHE key index : 94 */

    LAST_ITEM_SHE_KEY = 0x01000000
} r_she_key_index_t;

#endif /* #if !defined(R_ICUMIF_API_SHE_H) */
