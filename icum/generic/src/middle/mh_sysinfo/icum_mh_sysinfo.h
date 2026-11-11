/******************************************************************************/
/* Component Name  System / Life Cycle Manager (Private Header)               */
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

#if !defined(ICUM_MH_SYSINFO_H)
#define ICUM_MH_SYSINFO_H
/* job_id to call mh_sysinfo_ExecTerminate */
#define MH_SYSINFO_JOB_ID (0x0099u)

/* Standby mode tansition */
#define SYSINFO_STOP_OK   (0x00000000uL)
#define SYSINFO_STOP_NG   (0x00000001uL)

#define NB_CUS_CODE_BYTE      (32u)
#define NB_CUS_NUM_BYTE       (28u)

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
/* System Interrupt */
void mh_SYSINFO_SendSysInterrupt(void);
void mh_SYSINFO_LoopInfinity(void);
uint32_t mh_SYSINFO_TransitionStopMode(void);
uint32_t mh_SYSINFO_FindErrCode(uint32_t in_err, uint32_t *p_out_err);

#endif /* #if !defined(ICUM_MH_SYSINFO_H) */
