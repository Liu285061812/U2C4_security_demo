/******************************************************************************/
/* Component Name  DVC Manager                                                */
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

#if !defined(ICUM_ML_DVC_PUB_H)
#define ICUM_ML_DVC_PUB_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Return Value Definition */
#define ER_ML_DVC_ALIGN                 (ER_ML_DVC_BASE + (0x00000000uL))
                                                  /* alignment error of parameter    */
#define ER_ML_DVC_AES_TIMEOUT           (ER_ML_DVC_BASE + (0x00000001uL))
                                                  /* timeout error from AES function */

/* Size of dvc_t.dvc  */
#define DVC_T_SIZE  ((uint32_t)16)

/* Calculation Type */
#define ML_DVC_CALC_TYPE_HASH      (0uL)   /* HASH */
#define ML_DVC_CALC_TYPE_CMAC      (1uL)   /* CMAC */

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Type definition                                                                    */
/**************************************************************************************/



/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/

/* Flash location : combination of block index and word index for secure data flash only */
/* when specify the code flash and the user data flash, set to direct_addr */

typedef struct
{
#pragma alignvar(4)
    uint8_t dvc[DVC_T_SIZE];
} dvc_t;

typedef struct
{
    const uint8_t       *p_input;
    uint16_t            data_size;
    dvc_t               *p_output;
} dvc_calculate_t;


/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
uint32_t ML_DVC_Init( void );
uint32_t ML_DVC_Calculate(dvc_calculate_t *p_par, uint32_t calc_type);

#endif
/* #if !defined(ICUM_ML_DVC_PUB_H) */

