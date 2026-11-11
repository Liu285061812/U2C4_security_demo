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

/**********************************************************************************/
/* Type definition                                                                */
/**********************************************************************************/

typedef enum
{
    /* operation is read */
    E_SECREG_GET,
    /* operation is write */
    E_SECREG_SET,

    LAST_ITEM_OPBT_OPERATION = 0x01000000
} r_opbt_oparation_t;

typedef enum
{
    /* UPGP1 */
    E_SECREG_UPGP1,
    /* UPGP2 */
    E_SECREG_UPGP2,
    /* ICUMRESV */
    E_SECREG_RESV,
    /* ICUMRAC */
    E_SECREG_RAC,
    /* ICUMRAD */
    E_SECREG_RAD,
    /* ICUMPEDBG */
    E_SECREG_PEDBG,
    /* OPCRF */
    E_SECREG_OPCRF,
    /* OPGPIO */
    E_SECREG_OPGPIO,
    /* HSIF_CRAUTH_NEED */
    E_SECREG_HSIF_CRAUTH_NEED,
    /* ICUMOPWDVAC */
    E_SECREG_ICUMOPWDVAC,
    /* ICUMOPWDRUN */
    E_SECREG_ICUMOPWDRUN,
    /* ICUMOPWDEN */
    E_SECREG_ICUMOPWDEN,
    /* ICUMOPWDOVF */
    E_SECREG_ICUMOPWDOVF,
    /* ICUMLRAM */
    E_SECREG_LRAM,
    /* ICUMPKCCARAM */
    E_SECREG_PKCCA,

    LAST_ITEM_OPBT = 0x01000000
} r_info_type_access_secreg_t;


/**********************************************************************************/
/* Service parameters                                                             */
/**********************************************************************************/

typedef struct ISD_ACCESS_SECREG
{
    /* operation read or write*/
    r_opbt_oparation_t            opbt_oparation;
    /* Which OPBT to specify */
    r_info_type_access_secreg_t   opbt_num;
    /* Setting value */
    uint32_t                      opbt_value;
} ISD_ACCESS_SECREG_t;
