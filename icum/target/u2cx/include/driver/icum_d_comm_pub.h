/******************************************************************************/
/* Component Name   ICUM COMM Driver                                          */
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

#if !defined(ICUM_D_COMM_PUB_H)
#define ICUM_D_COMM_PUB_H

/**********************************************************************************
* Include
***********************************************************************************/
/* For ICU-M service descriptor (ISD). */
#include "r_icumif_api.h"

/**************************************************************************************/
/* Return Value Definition                                                            */
/**************************************************************************************/
#define ER_D_COMM_QUE_FULL              (ER_D_COMM_BASE+1UL)    /* Service response queue FULL */
#define ER_D_COMM_PARAM_NOT_CRAM        (ER_D_COMM_BASE+2U)     /* Param not within global RAM area  */
#define ER_D_COMM_QUE_NOT_ACCESS        (ER_D_COMM_BASE+3U)
/* address not witnin RAM area */
#define ER_D_COMM_MEMCHK_NG             (ER_D_COMM_BASE+4UL)

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Bit position definition of status information of PE */
#define ICUM_COMM_STS_FACIACCDIS          (0x08U)      /* FACI access disabled state */
/* Flash operation of PE. */
#define ICUM_COMM_STS_FLS_OPRN            (0x04U)
#define ICUM_COMM_STS_CF_PRG              (0x02U)
                /* PE is accessing Code Flash in write / erase mode */
#define ICUM_COMM_STS_DF_PRG              (0x01U)
                /* PE is accessing Data Flash in write / erase mode */

/* Processing object PE */
#define ICUM_COMM_TARGET_PE0              (0U)         /* PE0 */
#define ICUM_COMM_TARGET_PE1              (1U)         /* PE1 */
#define ICUM_COMM_TARGET_PE2              (2U)         /* PE2 */

#define ICUM_COMM_PEID_MASK               (0x07U) /* Mask for PEID */ 

/* Channel number of the interrupt to be issued */
#define ICUM_COMM_ICH_TO_SYS_INTERRUPT    (1UL)         /* System interrupt */
#define ICUM_COMM_ICH_TO_SERV_RESPONSE    (2UL)         /* Service execution response interrupt */
#define ICUM_COMM_ICH_TO_DF_PRG_REQ_0     (3UL)
                /* Data Flash ( FPSYS0 ) mode switching request interrupt */

/* Maximum number of interrupts issued */
#define ICUM_COMM_ICH_TO_SERV_MAX         ICUM_COMM_ICH_TO_DF_PRG_REQ_0

/* Channel number of received interrupt */
#define ICUM_COMM_ICH_FM_SERV_REQUEST     (0x00000002UL) /* Service execution request interrupt                  */
#define ICUM_COMM_ICH_FM_CF_PRG_NOTIFY    (0x00000004UL) /* Code Flash mode switching ACK notification interrupt */
#define ICUM_COMM_ICH_FM_DF_PRG_NOTIFY    (0x00000008UL) /* Data Flash mode switching ACK notification interrupt */
#define ICUM_COMM_ICH_FM_CF_PRG_ERROR     (0x00000010UL) /* Code Flash mode switching NAK notification interrupt */
#define ICUM_COMM_ICH_FM_DF_PRG_ERROR     (0x00000020UL) /* Data Flash mode switching NAK notification interrupt */
#define ICUM_COMM_ICH_FM_OPERATION_END    (0x00000040UL) /* Flash access completion notification interrupt */
#define ICUM_COMM_ICH_FM_REGIST_ADDJOBFUNC (0x80000000UL) /* Job register function regist */

#define ICUM_COMM_ICH_FM_SERV_FROM_PE0    (0x01000000UL) /* Service execution request interrupt from PE0         */
#define ICUM_COMM_ICH_FM_SERV_FROM_PE1    (0x02000000UL) /* Service execution request interrupt from PE1         */
#define ICUM_COMM_ICH_FM_SERV_FROM_PE2    (0x04000000UL) /* Service execution request interrupt from PE2         */

/* Single interrupt */ 
#define ICUM_COMM_ICH_FM_SINGLE_INTERRUPT (0x00000001UL) /* Service execution request single interrupt           */ 

/* ICU-M firmware status information */
#define ICUM_COMM_INF_ICU_ST_BUSY         (0x00000001UL) /* Service execution state                      */
#define ICUM_COMM_INF_ICU_ST_SEC_BOOT     (0x00000002UL) /* Secure boot activation state                 */
#define ICUM_COMM_INF_ICU_ST_BOOT_INIT    (0x00000004UL) /* Secure Boot Personalized State               */
#define ICUM_COMM_INF_ICU_ST_BOOT_FIN     (0x00000008UL) /* Secure boot complete status                  */
#define ICUM_COMM_INF_ICU_ST_BOOT_OK      (0x00000010UL) /* Secure boot success status                   */
#define ICUM_COMM_INF_ICU_ST_RND_INIT     (0x00000020UL) /* Random number generator initialization state */
#define ICUM_COMM_INF_ICU_ST_EXT_DEBUG    (0x00000040UL) /* Debugger connection status                   */
#define ICUM_COMM_INF_ICU_ST_INT_DEBUG    (0x00000080UL) /* ICU-M debugging enable state                 */
#define ICUM_COMM_INF_ICU_ST_CLMAERR      (0x00000400UL) /* Monitored clock over or below the threshold  */
#define ICUM_COMM_INF_ICU_ST_BUSERR       (0x00000800UL) /* Bus access error occurrence status           */
#define ICUM_COMM_INF_ICU_ST_ROMERR       (0x00001000UL) /* Code Flash error occurrence status           */
#define ICUM_COMM_INF_ICU_ST_RAMERR       (0x00002000UL) /* Local RAM error occurrence status            */
#define ICUM_COMM_INF_ICU_ST_WDTERR       (0x00004000UL) /* Watchdog timer TO state of occurrence        */
#define ICUM_COMM_INF_ICU_ST_SYSTEMERR    (0x00008000UL) /* System error occurrence status               */
#define ICUM_COMM_INF_ICU_ST_MAPERR       (0x00010000UL) /* Reserved                                     */
#define ICUM_COMM_INF_ICU_ST_EXTERR       (0x00020000UL) /* External factor error occurrence status      */
#define ICUM_COMM_INF_ICU_ST_FCUERR       (0x00040000UL) /* FCU error condition                          */
#define ICUM_COMM_INF_ICU_ST_RNDERR       (0x00080000UL) /* TRNG online check error condition            */
#define ICUM_COMM_INF_ICU_ST_STARTUP      (0x00400000UL) /* Startup Bank Judge Status                    */
#define ICUM_COMM_INF_ICU_ST_BOOTERR      (0x00800000UL) /* Boot error status                            */
#define ICUM_COMM_INF_ICU_ST_TRNG         (0x02000000UL) /* TRNG generated Status                        */
#define ICUM_COMM_INF_ICU_ST_FLS_WAIT     (0x04000000UL) /* Flash operation wait status                  */
#define ICUM_COMM_INF_ICU_ST_FACIACC      (0x08000000UL) /* Data Flash access status                     */
#define ICUM_COMM_INF_ICU_ST_SERVAVAIL    (0x10000000UL) /* Service availability status                  */
#define ICUM_COMM_INF_ICU_ST_FCUINIT      (0x20000000UL) /* FCU initialization state                     */
#define ICUM_COMM_INF_ICU_ST_PLLSET       (0x40000000UL) /* PLL stable state                             */
#define ICUM_COMM_INF_ICU_ST_STARTPE      (0x80000000UL) /* PE startup state                             */

/* INTPE Interrupt status */
#define ICUM_COMM_INT_REQ_DISABLE         (0x00U)       /* Interrupt request disable */
#define ICUM_COMM_INT_REQ_ENABLE          (0x01U)       /* Interrupt request enable  */

/* Not checking the highest byte */
#define ICUM_COMM_DO_NOT_CHECK            (0x00U)
/* Checking the highest byte */
#define ICUM_COMM_DO_CHECK                (0x01U)
/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* typedef                                                                            */
/**************************************************************************************/
typedef uint8_t     icum_comm_target_pe_t;             /* PE to be processed  */
typedef uint32_t    icum_comm_int_to_channel_t;        /* Channel number of the interrupt to be issued */
typedef uint32_t    icum_comm_int_from_channel_t;      /* Channel number of received interrupt */
typedef uint32_t    icum_comm_info_icu_status_t;       /* ICU-M firmware status information */
typedef uint8_t     icum_comm_int_req_status_t;        /* INTPE Interrupt status */
typedef uint8_t     icum_comm_check_order_t;

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* Registration information of function to call back when receiving an interrupt */
/* 1st Element : Channel number of received interrupt            */
/* 2nd Element : ISD address value notified from PE              */
/*               It is effective only when the channel number is */
/*               service execution request interrupt             */
/*               If the channel number is other than the above,  */
/*               it is NULL                                      */
typedef void (p_func_addr) (icum_comm_int_from_channel_t channel, r_icumif_isd_t *addr);
typedef struct ICUM_COMM_CB_REGIST
{
    icum_comm_int_from_channel_t channel;
                                    /* A bit pattern indicating the channel number */
    p_func_addr                  *p_cbfunc;
                                    /* Function to call back when interrupt specified by channel is received */
}ICUM_COMM_CB_REGIST_t;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
void D_COMM_Init(void);
uint32_t D_COMM_TriggerInterruptToPE(icum_comm_target_pe_t destination, icum_comm_int_to_channel_t channel, 
                                     r_icumif_isd_t *p_isd);
uint32_t D_COMM_ServiceResponseToPE(icum_comm_target_pe_t destination, r_icumif_isd_t *p_isd);
uint32_t D_COMM_GetPEStatusInfo(uint8_t *p_status);
uint32_t D_COMM_SetICUStatusInfo(icum_comm_info_icu_status_t info, uint32_t set_value);
uint32_t D_COMM_GetICUStatusInfo(uint32_t *p_status);
uint32_t D_COMM_SetIntCallBackFunc(ICUM_COMM_CB_REGIST_t *p_regist_info);
void D_COMM_StartupPE(void);
uint32_t D_COMM_GetStartupPEInfo(uint32_t *p_boot_pe);
uint32_t D_COMM_SetInterruptSwitch(icum_comm_int_from_channel_t channel, icum_comm_int_req_status_t status);
uint32_t D_COMM_CheckMemorySharedRAM(uint32_t start_addr, uint32_t size, icum_comm_check_order_t msbyte_check);
uint32_t D_COMM_GetBootMode(uint32_t *p_status);

#endif /* #if !defined(ICUM_D_COMM_PUB_H) */
