/******************************************************************************/
/* Component Name   Service 01 AES CMAC private header file                   */
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

#if !defined(ICUM_S_01_AES_COMMON_H)
#define ICUM_S_01_AES_COMMON_H

#include "renesas_types.h"
#include "r_icumif_api.h"
#include "icum_mh_key_pub.h"

#include "icum_common_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_d_cry_aes_pub.h"
#include "icum_d_misc_pub.h"

#include "icum_s_pub.h"
#include "icum_service_common.h"

#include "icum_k_issd_pub.h"
#include "icum_l_toolbox_pub.h"

#include "icum_ml_ctx_pub.h"

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
#define JOB_NOT_COMPLETE         (0x00u)       /* Represents not job completion processing */
#define JOB_COMPLETE             (0x01u)       /* Represents job completion processing     */
#define RESOURCES_NOT_HOLD       (0x00u)       /* Holds resources(key)                     */
#define RESOURCES_HOLD           (0x01u)       /* Not hold resources(key)                  */
#define AES_BYTE_SIZE_IN_BITS    (0x08u)       /* 1 byte is 8 bits                         */
#define NO_SLICE                 (0x00u)       /* No slice                                 */
/**************************************************************************************/
/* Definition CMAC                                                                    */
/**************************************************************************************/
#define CMAC_SIZE_MAX               (0x80u)         /* Maximum size of CMAC               */
#define CMAC_SIZE_HALF              (0x40u)         /* Half size of CMAC                  */
#define CMAC_SIZE_MIN_VERIFICATION  (0x00u)         /* Minimum size of CMAC verification  */

#define ALIGN4                      (0x00000003uL)  /* 4byte alignment check */
#define PADDING_MASK                (0xFF00uL)      /* Mask for zero padding */

/**************************************************************************************/
/* Definition Convert ERROR                                                           */
/**************************************************************************************/
#define AES_CIPHER_NB_ERR       (30u)
#define AES_CMAC_NB_ERR         (29u)

#define ERR_IDX_BASE            (0u)
#define ERR_IDX_SERV            (1u)
#define ERR_IDX_MAX             (2u)

#define ADDR_1ST                (uint32_t)(1uL)

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
typedef struct s_01_com_key_param
{
    r_key_group_t   key_group;         /* Key Group */
    r_key_index_t   key_id;            /* Key ID */
    uint8_t         NotUseFlag;        /* Set to 1 if there is no need for release */
    uint8_t         direction;         /* CIPHER(Enc:0/Dec:1) */
    aes_key_t       **p_aeskey;        /* Output Area of Key pointer */
} s_01_com_key_param_t;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
uint32_t s_01_CIPHER_ConvertErr(uint32_t base_err);
uint32_t s_01_CIPHER_GetAESKey(icum_job_t *p_job, s_01_com_key_param_t *p_KeyParam);
uint32_t s_01_CIPHER_Sizing(uint8_t *lastb, uint8_t length);

#endif /* ICUM_S_01_AES_COMMON_H */
