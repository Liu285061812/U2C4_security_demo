/******************************************************************************/
/* Component Name   ISSD (ICU-M Start-up & Service Dispatcher)                */
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

#if !defined(ICUM_K_ISSD_H)
#define ICUM_K_ISSD_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* enum definition */
/* secure boot stage */
typedef enum
{
    E_SECURE_BOOT_INIT  = -1,
    E_FIRST_SECURE_BOOT = 0,
    E_SECOND_SECURE_BOOT,
    E_BACKGROUND_VERIFY
} secure_boot_stage_t;

/* Background verify status */
#define BACKGROUND_VERIFY_OFF               0uL
#define BACKGROUND_VERIFY_ON                1uL

/* ISSD execution modes */
#define ISSD_MODE_START                     1uL
#define ISSD_MODE_VERIFY_BOOT_1ST           2uL
#define ISSD_MODE_VERIFY_BOOT_2ND           3uL
#define ISSD_MODE_ROM_EXEC                  4uL
#define ISSD_MODE_ECC_TEST                  6uL

/* Value of ICU status */
#define ICU_STATUS_ON               0x00000001uL
#define ICU_STATUS_OFF              0x00000000uL

/* Value of internal error code */
#define ER_SERV_ID_ERROR                    1uL
#define ER_SERV_UNAVAILABLE                 2uL
#define ER_SERV_SYS_ERROR                   3uL

/* definition of  judge for expansion service */
#define EXPANSION_SERVICE_BIT       0x00000100uL

/* definition  of CISM header length */
#define CISM_HEADER_LENGTH          32uL

/* Maximum registered number of service generate request */
#define MAX_REGIST_SRV_GEN_REQ      16uL

/* Job Queue status */
#define ISSD_QUEUE_EMPTY             0xFFu

/* Command Data to release protection */
#define PROTCMD_START               (0x000000A5uL)
/* Confirming Data to complete writing normally */
#define PROTS0ERR                   (0x00000001uL)

/*  Bus Standby Control Register */
#define STBY_CTRL                   (0x0034uL)
#define STBY_CTRL_OPBT_RDY          (1uL << 0uL)
#define ISSD_LOGI_ADDR_STBY_BASE    (0xFDF18200uL)
#define ISSD_LOGI_ADDR_STBY_CTRL    (ISSD_LOGI_ADDR_STBY_BASE + STBY_CTRL)

/* MCUAXI (Bus Standby Control Register) */
#define PHYS_ADR_MCUAXI             (0xF8E00000uL)
/* MFIS */
#define PHYS_ADR_MFIS               (0xD6200000uL)

/* Boot Status Register (Bit24-31) */
#define ISSD_LOGI_ADDR_MFISBTSTSR   (0xFD860604uL)
#define ICUMH_START_KEY_VAL         (0x5AuL << 24uL)
#define ICUMH_START_KEY_MASK        (0xFFuL << 24uL)

/***************************************************************************************
*  Pointer type to an ICU-M service function
***************************************************************************************/
typedef void (*p_high_priority_func_t)(r_icumif_isd_t *p_ISD);

#define ISSD_PERMIT_NORMAL    0x00000001uL
#define ISSD_PERMIT_RAMEXE    0x00000020uL
#define ISSD_PERMIT_ADMIN     0x00000040uL

typedef struct
{
    p_service_func_t        p_service_func;
    p_service_func_t        p_service_func_ram;
    p_high_priority_func_t  p_high_priority_func;
    uint32_t                permission;
} service_t;

/**********************************************************************************
* Prototypes
**********************************************************************************/
/* Execution mode management functions */
void k_ISSD_InitRunMode(void);
void k_ISSD_ModeChangeRequested(bool *p_change_request);
void k_ISSD_Run(void);
void test_ICUM_only(void); //zhxk
void k_ISSD_IsBootMode(bool *p_boot_mode);
void k_ISSD_GetRunMode(uint32_t *p_run_mode);
void k_ISSD_SetBackgroundVerifyState(uint32_t state);

/* Service dispatcher functions */
void k_ISSD_MainDispatch(void);
void k_ISSD_RamDispatch(void);
uint32_t k_ISSD_HandleResponse(r_icumif_isd_t *p_isd);
void k_ISSD_ProcessJob(icum_job_t *p_job);
void k_ISSD_CheckDebuggerConnection(void);
uint32_t k_ISSD_CheckNVMWarning(r_icumif_isd_t *p_isd);
uint32_t k_ISSD_GetServiceFunc(r_icumif_service_id_t in_service_id, p_service_func_t *p_serv_func);
void k_ISSD_HighestDispatch(r_icumif_isd_t *p_ISD, bool *p_job_exec);

/* Job management functions */
void k_ISSD_InitJobQueue(void);
void k_ISSD_GetQueueType(r_icumif_service_priority_t service_priority, uint8_t *p_queue_type);
void k_ISSD_AddJobQueue(r_icumif_isd_t *p_ISD, icum_job_t *p_ghost_job, r_icumif_job_id_t *p_job_id);
void k_ISSD_ClearJobExecCounter(void);
void k_ISSD_RemoveJob(r_icumif_job_id_t job_id);
void k_ISSD_RemoveCurrentJob(void);
void k_ISSD_JudgePriority(int8_t *p_next_que_type);
void k_ISSD_GetNextJob(icum_job_t **p_next_job);
void k_ISSD_GetJob(r_icumif_job_id_t job_id, icum_job_t **p_specif_job);
uint32_t k_ISSD_GetAllJobCount(uint32_t *p_count);
uint32_t k_ISSD_SetExpansionService(r_icumif_job_id_t job_id, uint8_t set_value);
uint32_t k_ISSD_InitGenService(void);
uint32_t k_ISSD_IsRegistGenService(bool *p_registered);
uint32_t k_ISSD_GenerateServiceExec(void);

#endif /* #if !defined(ICUM_K_ISSD_H) */
