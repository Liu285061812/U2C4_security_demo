/******************************************************************************/
/* Component Name  System / Life Cycle Manager (Public Header)                */
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

#if !defined(ICUM_MH_SYSINFO_PUB_H)
#define ICUM_MH_SYSINFO_PUB_H

/**************************************************************************************/
/* Definitions                                                                        */
/**************************************************************************************/
/* Return Value Definitions */

/* Flash Error */
#define ER_MH_SYSINFO_NVM_SYS                 (ER_MH_SYSINFO_BASE + (0x00000000uL))
#define ER_MH_SYSINFO_NVM_SYS_CRITICAL        (ER_MH_SYSINFO_BASE + (0x00000001uL))
#define ER_MH_SYSINFO_NVM_PROG                (ER_MH_SYSINFO_BASE + (0x00000002uL))
#define ER_MH_SYSINFO_NVM_DATA_INVALID        (ER_MH_SYSINFO_BASE + (0x00000003uL))
#define ER_MH_SYSINFO_NVM_UNAVAILABLE         (ER_MH_SYSINFO_BASE + (0x00000004uL))
#define ER_MH_SYSINFO_SYS_ERROR               (ER_MH_SYSINFO_BASE + (0x00000005uL))
#define ER_MH_SYSINFO_UNKNOWN                 (ER_MH_SYSINFO_BASE + (0x00000006uL))

/* Tick Count ID */
#define SYSINFO_TICK_COUNT_ID_00              (0x00u) /* for secure boot measuring */
#define SYSINFO_TICK_COUNT_ID_01              (0x01u) /* for secure boot measuring */
#define SYSINFO_TICK_COUNT_ID_02              (0x02u) /* not use */
#define SYSINFO_TICK_COUNT_ID_03              (0x03u) /* not use */
#define SYSINFO_TICK_COUNT_ID_04              (0x04u) /* not use */
#define SYSINFO_TICK_COUNT_ID_NONE            (0xFFu)

/* Firmware State */
#define SYSINFO_STATE_OFF                     (uint8_t)(0x00u)
#define SYSINFO_STATE_ON                      (uint8_t)(0x01u)

/* Repro Mode State */
#define SYSINFO_REPRO_STATE_ICUP              (uint8_t)(0x00u)
#define SYSINFO_REPRO_STATE_HOST              (uint8_t)(0x01u)

/* Data Flash Initialized State */
#define SYSINFO_DF_NOT_INITIALIZED            (uint8_t)(0x00u)
#define SYSINFO_DF_INITIALIZED                (uint8_t)(0x01u)

/* Customer Code ID */
#define SYSINFO_CUSTOMER_NUMBER               (uint8_t)(0x00u)

/* Customer Number */
#define SYSINFO_CUSTOMER_NUMBER_REL           (uint8_t)(0x00u)

/********************************************************************************
 Exported global variables (extern).
********************************************************************************/
extern uint32_t g_lc2s_state_bk;

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
/* Init */
uint32_t MH_SYSINFO_Init(void);

/* Update Flash */
uint32_t MH_SYSINFO_UpdateSystemConfig(r_icumif_job_id_t job_id);

/* System Information */
uint32_t MH_SYSINFO_GetFirmwareName(uint8_t *p_firmware_name);
uint32_t MH_SYSINFO_GetFirmwareVersion(uint8_t *p_firmware_version);
uint32_t MH_SYSINFO_GetFirmwareBuildType(uint8_t *p_build_type);
uint32_t MH_SYSINFO_GetChipId(uint8_t *p_chip_id, uint8_t *p_size);
uint32_t MH_SYSINFO_GetReducedChipId(uint8_t *p_reduced_chip_id);
uint32_t MH_SYSINFO_GetCustomerCodeInfo(uint8_t info_id, uint8_t *p_info);

/* Secure Tick Timer */
uint32_t MH_SYSINFO_GetTickCount(secure_tick_info_t *p_info);
uint32_t MH_SYSINFO_SaveTickCount(uint8_t id);
uint32_t MH_SYSINFO_GetSavedTickCount(uint8_t id, uint32_t *p_count);
uint32_t MH_SYSINFO_ClearSavedTickCount(uint8_t id);
uint32_t MH_SYSINFO_SetTickInterval(uint16_t interval);
uint32_t MH_SYSINFO_GetTickFreq(uint32_t *p_freq);

/* Firmware State */
uint32_t MH_SYSINFO_SetCISMState(uint8_t state);
uint32_t MH_SYSINFO_GetCISMState(uint8_t *p_state);
uint32_t MH_SYSINFO_SetRamExeState(uint8_t state);
uint32_t MH_SYSINFO_GetRamExeState(uint8_t *p_state);
uint32_t MH_SYSINFO_SetInitState(uint8_t state);
uint32_t MH_SYSINFO_GetInitState(uint8_t *p_state);
uint32_t MH_SYSINFO_SetReproModeState(uint8_t update_target,uint8_t state);
uint32_t MH_SYSINFO_GetReproModeState(uint8_t update_target,uint8_t *p_state);

/* System Interrupt */
uint32_t MH_SYSINFO_SetSysInterrupt(sys_cfg_sys_int_t *p_sys_interrupt);
uint32_t MH_SYSINFO_InterruptCallBack(uint32_t factor);
uint32_t MH_SYSINFO_InterruptNMICallBack(uint32_t factor);
uint32_t MH_SYSINFO_InterruptMPUCallBack(uint32_t factor);
uint32_t MH_SYSINFO_InterruptCLMACallBack(void);

/* Stop */
void MH_SYSINFO_Stop(void);
#endif /* #if !defined(ICUM_MH_SYSINFO_PUB_H) */
