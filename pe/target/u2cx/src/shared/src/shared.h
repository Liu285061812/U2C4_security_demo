/******************************************************************************/
/* Component Name   Testing the ICU-M                                         */
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

#if !defined(SHARED_H)
#define SHARED_H

// ********************************************************************************
// Shared variables
// ********************************************************************************

#include "renesas_types.h"

#define SIZE_OF_BIG_BUFFER      (64*4*16)
#define SIZE_OF_BIG_BUF2        (1024)
#define SIZE_OF_ISD_BUFFER      (128*25)
#define SIZE_OF_COM_BUFFER      (512)

#if !defined(_IN_SHARED_)

extern uint32_t ICUM_QUEUE[];
extern uint8_t  BIG_BUFFER[][SIZE_OF_BIG_BUFFER];
extern uint8_t  ISD_BUFFER[SIZE_OF_ISD_BUFFER];
extern uint8_t  COM_BUFFER[SIZE_OF_COM_BUFFER];

extern uint8_t   strbin[20096];
extern uint8_t  DMAC_BUFFE[][SIZE_OF_BIG_BUFFER];

#endif

#pragma alignvar (4)
extern const uint8_t LOREM_IPSUM[];
extern const uint8_t LOREM_IPSUM_HASH_SHA256[];
extern const uint8_t LOREM_IPSUM_HASH_SHA512[];

extern uint8_t      __ghsbegin_start_gRAM_MEV[];
#define SYNC_PE     (((volatile uint32_t *)__ghsbegin_start_gRAM_MEV)[10])
#define SYNC_CMAC_FAST    (((volatile uint32_t *)__ghsbegin_start_gRAM_MEV)[11])
#define SYNC_PE_ISD_ADDR  (((volatile uint32_t *)__ghsbegin_start_gRAM_MEV)[12])
#define SYNC_PE_REQ_ERR   (((volatile int32_t *)__ghsbegin_start_gRAM_MEV)[13])

typedef enum {
    SYNC_PE_COMPLETED,
    START_ST_RAND_SERVICE_TO_PE2,
    END_ST_RAND_SERVICE_TO_PE2,
    REQUEST_COUNT_RESULT_TO_PE2,
    RESPONSE_COUNT_RESULT_FROM_PE2,
    RESPONSE_COUNT_RESULT_FROM_PE3,
    RESPONSE_COUNT_RESULT_FROM_PE4,
    RESPONSE_COUNT_RESULT_FROM_PE5,
    RESPONSE_COUNT_RESULT_FROM_PE6,
    SYNC_PE_END,
    SERVICE_REQUEST_FM_PE0_START,
    SERVICE_REQUEST_FM_PE0_END,
} sync_pe_t;

typedef enum {
    CMAC_NOT_EXIST,
    CMAC_EXIST,
} sync_cmac_fast_t;

#endif // #if !defined(SHARED_H)
