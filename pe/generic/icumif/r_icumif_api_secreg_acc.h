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
#if !defined(R_ICUMIF_API_SECREG_ACC_H)
#define R_ICUMIF_API_SECREG_ACC_H

/**********************************************************************************/
/* Type definition                                                                */
/**********************************************************************************/

/* Direction of register */
typedef enum
{
    REG_DIR_READ  = 0x00000000u,    /* Read Secure Register  */
    REG_DIR_WRITE = 0x00000001u     /* Secure register write */
} r_access_direction_t;

/* Specified register */
typedef enum
{
    REG_CPG_SCPTCSR      = 0u,      /* [00]:Secure Protect Control/Status Register */
    REG_CPG_SCERADR      = 2u,      /* [02]:Secure Error Address Register          */
    REG_RST_STBCHR1,                /* [03]:Standby Flag Register 1                */
    REG_RST_STBCHR2,                /* [04]:Standby Flag Register 2                */
    REG_RST_STBCHR3,                /* [05]:Standby Flag Register 3                */
    REG_RST_STBCHR4,                /* [06]:Standby Flag Register 4                */
    REG_RST_RRSTFR,                 /* [07]:RT Reset Flag Register                 */
    REG_RST_SRSTFR,                 /* [08]:SYS Reset Flag Register                */
    REG_RST_SCERADR      = 10u,     /* [10]:Secure Error Address Register          */
    REG_INTC_IMNTRSESR,             /* [11]:INTC-RT Routing Control Register       */
    REG_INTC_IMNTRESADDR = 13u,     /* [13]:INTC-Monitor Security Error Status Register */
    REG_INTC_IMNTRCCR,              /* [14]:INTC-Monitor Counter Control Register  */
    REG_MFIS_MFISBTSTSR,            /* [15]:Boot Status Register                   */
    REG_MFIS_MFISECSTSR,            /* [16]:Security Error Status Register         */
    REG_MFIS_MFISECSADR  = 18u      /* [18]:Security Error Slave Address Register  */
} r_register_id_t;

/**********************************************************************************/
/* Service parameters                                                             */
/**********************************************************************************/
typedef struct ISD_SECREG_ACCESS
{
   uint16_t  reg_id;    /* [IN]Secure register ID           */
   uint16_t  mode;      /* [IN]Access mode (Read/Write)     */
   uint32_t  reg_val;   /* [IN/OUT]Pointer of setting value */
} ISD_SECREG_ACCESS_t;

#endif /* #if !defined(R_ICUMIF_API_SECREG_ACC_H) */
