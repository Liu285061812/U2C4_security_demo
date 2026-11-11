/******************************************************************************/
/* Component Name   ICU-M Firmware System Primitives                          */
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

/**********************************************************************************
* Header files
***********************************************************************************/
#include "r_icumif_api.h"

/**********************************************************************************
* Enumerates
***********************************************************************************/
typedef enum
{
    WATCHDOG_OFF,
    WATCHDOG_ON
} watchdog_param_t;

/**********************************************************************************
* Redefining the standard system functions
***********************************************************************************/
#define memcpy(...) R_RH850_MemCpy(__VA_ARGS__)
#define memset(...) R_RH850_MemSet(__VA_ARGS__)
#define memcmp(...) R_RH850_MemCmp(__VA_ARGS__)
/**********************************************************************************
* Types
***********************************************************************************/

typedef struct icum_job
{
    r_icumif_isd_t      *p_ISD;
    r_icumif_job_id_t   job_id;
} icum_job_t;

/* KDF */
typedef struct
{
    r_kdf_algo_t        kdf_algo;
    r_hash_primitive_t  hash_primitive;
    uint8_t             *p_secret_key_value;
    uint32_t            secret_key_size_in_bytes;
    uint8_t             *p_info;
    uint32_t            info_size_in_bytes;
    uint16_t            counter;
    uint8_t             *p_deriv_key_value;
    uint8_t             deriv_key_size_in_bytes;
} r_kdf_t;

/**********************************************************************************
* Prototypes
***********************************************************************************/
void    R_RH850_MemSet(uint8_t *dest, uint8_t mychar, int16_t size);
void    R_RH850_MemCpy(uint8_t *dest, const uint8_t *source, int16_t size);
uint8_t R_RH850_MemCmp(const uint8_t *data1, const uint8_t *data2, int16_t size);

r_icumif_service_result_t start_internal_service(r_icumif_service_id_t service_id, r_icumif_isd_t *p_service, r_icumif_service_id_t from_service_id);

r_icumif_service_result_t get_mydata(uint8_t mydata_idx, uint8_t *p_mydata, uint16_t data_size);
r_icumif_service_result_t update_mydata(uint8_t mydata_idx, uint8_t *p_mydata, uint16_t data_size,
                                        r_icumif_job_id_t job_id);

watchdog_param_t watchdog_get_restart_req(void);
void watchdog_restart(void);
