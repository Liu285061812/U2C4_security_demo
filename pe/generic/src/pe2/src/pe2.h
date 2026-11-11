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

#if !defined(PE2_H)
#define PE2_H

// ********************************************************************************
// Headers
// ********************************************************************************

#include "renesas_types.h"
#include "r_icumif_api.h"
#include "r_icumif_pub.h"
// ********************************************************************************
// Type definition
// ********************************************************************************

/*===========================================================================
  Enum Types conversion table
  ===========================================================================*/
typedef struct{
    char            * str;
    int             e_num;
}cvt_enum_tbl;

/*===========================================================================
  Constant data address conversion table
  ===========================================================================*/
typedef struct{
    char            * str;
    unsigned int    addr;
}cvt_buff_tbl;

// ********************************************************************************
// Link to global variables
// ********************************************************************************

#pragma alignvar (4)
extern const uint8_t LOREM_IPSUM[];
extern unsigned short vrun_chrono_sec;
extern unsigned short vrun_chrono_millisec;
extern unsigned short vrun_chrono_microsec;

// ********************************************************************************
// global variables
// ********************************************************************************

// ********************************************************************************
// Prototypes
// ********************************************************************************

char *R_ICUMIF_GetErrorCodeName(uint8_t error_code);

/********************************************************************************
 Exported global functions (to be accessed by other files)
********************************************************************************/
void __SYNCI(void);
void __SYNCP(void);
void __SYNCM(void);

// ********************************************************************************
// ASM macros
// ********************************************************************************

/*===========================================================================
  Macros used for symbol concatenation
  ===========================================================================*/

#endif //if !defined(PE2_H)
