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
#if !defined(ICUM_D_COMM_LCL_H)
#define ICUM_D_COMM_LCL_H

/**************************************************************************************/
/* Return Value Definition                                                            */
/**************************************************************************************/

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* PE2ICUFSA register setting value */
#define ICUM_COMM_PE2ICUFSA0_SET        (0x00000000UL)      /* PE0 */
#define ICUM_COMM_PE2ICUFSA1_SET        (0x00000001UL)      /* PE1 */
#define ICUM_COMM_PE2ICUFSA2_SET        (0x00000002UL)      /* PE2 */
                                        /*  b20-16 : IDMASK[4:0] = 11111: invalid bit of AUTHID   */
                                        /*  b4-0   : AUTHID[4:0] = SPID                           */

/* PE2ICUSA register setting value */
#define ICUM_COMM_PE2ICUSA0_SET         (0x00000000UL)      /* PE0 */
#define ICUM_COMM_PE2ICUSA1_SET         (0x00000001UL)      /* PE1 */
#define ICUM_COMM_PE2ICUSA2_SET         (0x00000002UL)      /* PE2 */
                                        /*  b20-16 : IDMASK[4:0] = 11111: invalid bit of AUTHID   */
                                        /*  b4-0   : AUTHID[4:0] = SPID                           */

/* PE2ICUSFSA register setting value */
#define ICUM_COMM_PE2ICUSFSA_SET        (0x00000000UL)
                                        /*  b20-16 : IDMASK[4:0] = 11111: invalid bit of AUTHID    */
                                        /*  b4-0   : AUTHID[4:0] = SPID                            */

/* ICUM_PE2ICUIE Register setting value */
#define ICUM_COMM_PE2ICUIE_SET          (0xFF00007CU)     /* ICUM_PE2ICUF Enable Register                   */
                                                     /*  b1 : ICUM_COMM_ICH_FM_SERV_REQUEST  disable   */
                                                     /*  b2 : ICUM_COMM_ICH_FM_CF_PRG_NOTIFY enable    */
                                                     /*  b3 : ICUM_COMM_ICH_FM_DF_PRG_NOTIFY enable    */
                                                     /*  b4 : ICUM_COMM_ICH_FM_CF_PRG_ERROR  enable    */
                                                     /*  b5 : ICUM_COMM_ICH_FM_DF_PRG_ERROR  enable    */
                                                     /*  b6 : ICUM_COMM_ICH_FM_OPERATION_END enable    */

#define ICUM_COMM_SERV_REQUEST_MASK     (0xFF000000UL)  /* Disable or Enable service request from all PE */

#define ICUM_COMM_BC0_ACTIVE            (0x00000001UL)  /* This bit triggers the start-up of the PE1 */
#define ICUM_COMM_BC1_ACTIVE            (0x00000002UL)  /* This bit triggers the start-up of the PE2 */
#define ICUM_COMM_BC2_ACTIVE            (0x00000004UL)  /* This bit triggers the start-up of the PE3 */

#define ICUM_COMM_ALL_BC_ACTIVE         (ICUM_COMM_BC0_ACTIVE | ICUM_COMM_BC1_ACTIVE | \
                                         ICUM_COMM_BC2_ACTIVE)

#define ICUM_COMM_STSREG_SFT            (24U)

/* Size Definition */
#define ICUM_ICU2PEIS_PESEL_BIT_WIDTH   (4UL)         /* bit width of PESEL              */
#define REQUEST_QUEUE_DEPTH             (4UL)
#define RESPONSE_QUEUE_DEPTH            (4UL)
#define REQRES_QUEUE_SIZE                ((4UL * REQUEST_QUEUE_DEPTH) + (4UL * RESPONSE_QUEUE_DEPTH))

#define ICUM_COMM_CHANNELMAX            (32U)        /* Channel Number Max each PE2ICUS */
#define ICUM_COMM_ENTERMAX              (26U)

/* Mask Definition */
#define ICUM_PE2ICUS_PEICUMCOM_MASK     (0x00ffffffuL)  /* Used for Communication between  */

/* mask on the start address of the request queue */
#define MASK_ADDR_QUEUE                 (0xFE000000UL)

/* Msbyte check is Ok */
#define ICUM_COMM_MSBYTE_CHECK_OK        (0xFE000000UL)
/* Msbyte check mask definition*/
#define ICUM_COMM_MSBYTE_MASK            (0xFF000000UL)
/* address check mask definition */
#define ICUM_COMM_ADDR_MASK              (0x00FFFFFFUL)
/* address check shift definition */
#define ICUM_COMM_ADDR_SFT               (18U)
/* start address of cluster RAM for ICUM */
extern uint8_t  __ghsbegin_start_gRAM_BANK_C_ICUM[];
#define ICUM_COMM_CRAM_ICUM_START_ADDR   ((uint32_t)__ghsbegin_start_gRAM_BANK_C_ICUM)
#define ICUM_COMM_ADDR_MAX               (0x00057FFFUL)
/* last block index of cluster RAM2 */
#define ICUM_COMM_IDX_LAST_BLOCK_CRAM2   (23U)


/* PE2ICUS Register Bit Position Offset */
#define ICUM_COMM_ICU_BUSY_OFF          (0UL)         /* Service execution state                      */
#define ICUM_COMM_ICU_SEC_BOOT_OFF      (1UL)         /* Secure boot activation state                 */
#define ICUM_COMM_ICU_BOOT_INIT_OFF     (2UL)         /* Secure Boot Personalized State               */
#define ICUM_COMM_ICU_BOOT_FIN_OFF      (3UL)         /* Secure boot complete status                  */
#define ICUM_COMM_ICU_BOOT_OK_OFF       (4UL)         /* Secure boot success status                   */
#define ICUM_COMM_ICU_RND_INIT_OFF      (5UL)         /* Random number generator initialization state */
#define ICUM_COMM_ICU_EXT_DEBUG_OFF     (6UL)         /* Debugger connection status                   */
#define ICUM_COMM_ICU_INT_DEBUG_OFF     (7UL)         /* ICU-M debugging enable state                 */
#define ICUM_COMM_ICU_CLMAERR_OFF       (10UL)        /* Monitored clock over or below the threshold  */
#define ICUM_COMM_ICU_BUSERR_OFF        (11UL)        /* Bus access error occurrence status           */
#define ICUM_COMM_ICU_ROMERR_OFF        (12UL)        /* Code Flash error occurrence status           */
#define ICUM_COMM_ICU_RAMERR_OFF        (13UL)        /* Local RAM error occurrence status            */
#define ICUM_COMM_ICU_WDTERR_OFF        (14UL)        /* Watchdog timer TO state of occurrence        */
#define ICUM_COMM_ICU_SYSTEMERR_OFF     (15UL)        /* System error occurrence status               */
#define ICUM_COMM_ICU_MAPERR_OFF        (16UL)        /* Data Flash map construction error condition  */
#define ICUM_COMM_ICU_EXTERR_OFF        (17UL)        /* External error condition                     */
#define ICUM_COMM_ICU_FCUERR_OFF        (18UL)        /* FCU error condition                          */
#define ICUM_COMM_ICU_RNDERR_OFF        (19UL)        /* TRNG online check error condition            */
#define ICUM_COMM_ICU_STARTUP_OFF       (22UL)        /* Startup Bank Judge Status                    */
#define ICUM_COMM_ICU_TRNG_OFF          (25UL)        /* TRNG generated Status                        */
#define ICUM_COMM_ICU_FLS_WAIT_OFF      (26UL)        /* Flash operation wait status                  */
#define ICUM_COMM_ICU_FACIACC_OFF       (27UL)        /* Data Flash access status                     */
#define ICUM_COMM_ICU_SERVAVAIL_OFF     (28UL)        /* Service availability status                  */
#define ICUM_COMM_ICU_FCUINIT_OFF       (29UL)        /* FCU initialization state                     */
#define ICUM_COMM_ICU_PLLSET_OFF        (30UL)        /* PLL stable state                             */
#define ICUM_COMM_ICU_STARTPE_OFF       (31UL)        /* PE startup state                             */

#define ICUM_COMM_INTTYPE_BITOFF        (0UL)
#define ICUM_COMM_INTTYPE_SERV_MASK     (0xFF000000UL)

#define ICUM_COMM_ALIGNMENT_4BYTE       (0x00000003UL) /* 4 byte alignment                           */

#define ICUM_COMM_SEMA_RES              (0x01000000UL)
#define ICUM_COMM_SEMA_REQ              (0x02000000UL)
#define ICUM_COMM_SEMA_ERR              (0x20000000UL)
#define ICUM_COMM_SEMA_AUTH_MASK        (0xC0000000UL)
#define ICUM_COMM_SEMA_AUTH_FREE        (0x00000000UL)

#define ICUM_COMM_SEMA_AUTH_GET1ST      (0x00000000UL)
#define ICUM_COMM_SEMA_AUTH_GET2ND      (0xC0000000UL)
#define ICUM_COMM_SEMA_WAIT             (0x0000E800UL)

#define ICUM_COMM_SEMA_ER_SYSTEM        (0x00000001UL)
#define ICUM_COMM_SEMA_ER_TIMEOUT       (0x00000002UL)

#define ICUM_COMM_BOTH_SET                (0x02U)
#define ICUM_COMM_ON                      (0x01U)
#define ICUM_COMM_OFF                     (0x00U)

/* Boot Mode Mask */
#define BOOTMODE_BIT                    (0x00010000uL)
#define BOOTMODE_BITSHIFT_NUM           (16uL)

/* ICUM_UBMDSTS Register mask definition */
                                        /* User Boot Mode */
#define ICUM_COMM_UBMODE_MASK           (0x00000001UL)

/* Definition of initialization */
#define D_COMM_ZERO_U32                 (0UL)

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* typedef                                                                            */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* A table for obtaining the shift amount from the bit position */
typedef struct ICUM_COMM_ICUSTS
{
    uint32_t          uiIcsStsBit;              /* ICUM_PE2ICUS register Bit Position */
    uint32_t          uiIcsStsOff;              /* ICUM_PE2ICUS register Bit Offset   */
} ICUM_COMM_ICUSTS_t;

typedef struct ICUM_COMM_REQUESTER
{
    uint32_t                uiReqBit;
    icum_comm_target_pe_t uiRequester;
} ICUM_COMM_REQUESTER_t;


/*====================================================================================*/
/* Union for pointer manipulation                                                     */
/*====================================================================================*/
typedef union ptr_manip
{
    void        *p_ptr;
    uint32_t    val;
} p_manip_t;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
static void d_COMM_IntpeFunc(icum_comm_target_pe_t requester_pe);
static r_icumif_isd_t *d_COMM_GetServiceque(icum_comm_target_pe_t target_pe, int32_t queue_idx, uint8_t mem_check);
static uint32_t d_COMM_SetServiceque(icum_comm_target_pe_t target_pe, r_icumif_isd_t *p_ISD, uint8_t mem_check);
static uint32_t d_COMM_GetSemaphore(icum_comm_target_pe_t target_pe, uint8_t response);
static uint32_t d_COMM_FreeSemaphore(icum_comm_target_pe_t target_pe, uint8_t request);

#endif /* #if !defined(ICUM_D_COMM_LCL_H) */

