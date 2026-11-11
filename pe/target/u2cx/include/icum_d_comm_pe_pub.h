/******************************************************************************/
/* Component Name   ICU-M COMM(PE) Driver                                     */
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

#if !defined(ICUM_D_COMM_PE_PUB_H)
#define ICUM_D_COMM_PE_PUB_H

/**************************************************************************************/
/* Return Value Definition                                                            */
/**************************************************************************************/
#define ER_OK                           (0x00000000U)
#define ER_PARAM                        (0x00000001U)

#define ER_D_COMM_BASE                  (0x00000400U)

#define ER_D_COMM_QUE_FULL              (ER_D_COMM_BASE+1U)     /* Service request queue FULL        */
#define ER_D_COMM_NO_ISD                (ER_D_COMM_BASE+2U)     /* No ISD in service response queue  */
#define ER_D_COMM_QUE_ALREADY_INIT      (ER_D_COMM_BASE+3U)     /* Service queue already initialized */
#define ER_D_COMM_QUE_NOT_INIT          (ER_D_COMM_BASE+4U)     /* Service queue not initialized     */
#define ER_D_COMM_PARAM_NOT_G_RAM       (ER_D_COMM_BASE+5U)     /* Param not within global RAM area  */
#define ER_D_COMM_QUE_NOT_ACCESS        (ER_D_COMM_BASE+6U)
#define ER_D_COMM_EXIST_ISD             (ER_D_COMM_BASE+7U)     /* ISD is exist in service response queue */
#define ER_D_COMM_MEMCHK_NG             (ER_D_COMM_BASE+8U)     /* address not witnin RAM area */

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/

/* Bit pattern indicate channel No. of interrupt to send ( ICUM_COMM_INT_TO_CHANNEL ) */
#define ICUM_COMM_ICH_TO_SERV_REQUEST       (0x00000002UL)
#define ICUM_COMM_ICH_TO_CF_PRG_NOTIFY      (0x00000004UL)
#define ICUM_COMM_ICH_TO_DF_PRG_NOTIFY      (0x00000008UL)
#define ICUM_COMM_ICH_TO_CF_PRG_ERROR       (0x00000010UL)
#define ICUM_COMM_ICH_TO_DF_PRG_ERROR       (0x00000020UL)
#define ICUM_COMM_ICH_TO_OPERATION_END      (0x00000040UL)

#define ICUM_COMM_ICH_TO_SERV_FROM_PE0      (0x01000000UL) /* Service execution request interrupt from PE0         */
#define ICUM_COMM_ICH_TO_SERV_FROM_PE1      (0x02000000UL) /* Service execution request interrupt from PE1         */
#define ICUM_COMM_ICH_TO_SERV_FROM_PE2      (0x04000000UL) /* Service execution request interrupt from PE2         */

#define ICUM_COMM_ICH_TO_SINGLE_INTERRUPT   (0x00000001UL) /* Service execution request single interrupt           */ 

/* Channel No. of interrupt to receive ( ICUM_COMM_INT_FROM_CHANNEL ) */
#define ICUM_COMM_ICH_FM_SYS_INTERRUPT      (0U)
#define ICUM_COMM_ICH_FM_SERV_RESPONSE      (1U)
#define ICUM_COMM_ICH_FM_DF_PRG_REQ_0       (2U)

/* PE status information ( ICUM_COMM_INFO_PE_STATUS ) */
#define ICUM_COMM_INF_PE_ST_FLS_DF_PRG      (0U)
#define ICUM_COMM_INF_PE_ST_FLS_CF_PRG      (1U)
#define ICUM_COMM_INF_PE_ST_FLS_OPRN        (2U)
#define ICUM_COMM_INF_PE_ST_FACIACCDIS      (3U)
#define ICUM_COMM_INF_PE_ST_PEID            (8U)

#define ICUM_COMM_MEMCHECK_OK                (0x00U)
#define ICUM_COMM_MEMCHECK_ERROR             (0x01U)

/* PE identifier for interrupt handler ( D_COMM_PEID ) */
#define D_COMM_PEID_1                       (0UL)

/* Interrupt priority identifier for interrupt handler ( D_COMM_IRQ_PRIORITY ) */
#define D_COMM_IRQ_PRIORITY_NORMAL          (1UL)
#define D_COMM_IRQ_PRIORITY_MFIS_ERR        (2UL)

/* Total number of PE */
#define TOTAL_PE_NUM                        (2U)

/* Length of service queue ( SERVICE_QUEUE_LENGTH ) */
#define REQUEST_QUEUE_LENGTH                (4U)
#define RESPONSE_QUEUE_LENGTH               (4U)

#define RESPONSE_QUEUE_OFST                 (4U)
#define PE_QUEUE_DUMMY                      (0U)

#define TOTAL_QUEUE_LENGTH                  (REQUEST_QUEUE_LENGTH + RESPONSE_QUEUE_LENGTH)

/**************************************************************************************/
/* Macros                                                                             */
/**************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0026]:[ICUMFW_UD_S4_D_COMM_PE_0026]
* Function name: stsr
* Description  : Assembly macro to manipulate system register
*                Get Data from Basic System Register
* Arguments    : regID     : Register ID
*                selID     : Selection ID
* Return Value : Reading Data
***************************************************************************************/
static asm uint32_t stsr(int8_t regID, int8_t selID)
{
%con regID, selID
    stsr    regID, r10, selID
%error
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0027]:[ICUMFW_UD_S4_D_COMM_PE_0027]
* Function name: GET_MY_PEID
* Description  : Retrieve PEID from thread configurator register
* Arguments    : None
*                
* Return Value : PEID Value
***************************************************************************************/
#define GET_MY_PEID     (stsr(0, 2))

#define TARGET_PE0              (0U)         /* PE0 */
#define TARGET_PE1              (1U)         /* PE1 */
#define TARGET_PE2              (2U)         /* PE2 */

/**************************************************************************************/
/* Type definition                                                                    */
/**************************************************************************************/

/*====================================================================================*/
/* Type definition: Channel number of interrupt to send                               */
/*====================================================================================*/
typedef uint32_t    icum_comm_int_to_channel_t;

/*====================================================================================*/
/* Type definition: PE status information                                             */
/*====================================================================================*/
typedef uint32_t    icum_com_info_pe_status_t;

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/

uint32_t D_COMM_PE_Init(uint32_t *p_request_queue);
uint32_t D_COMM_TriggerInterruptToICUP(icum_comm_int_to_channel_t channel,
                                       r_icumif_isd_t *p_isd);
uint32_t D_COMM_GetResponseISD(r_icumif_isd_t **p_isd);
r_icumif_sts_t *D_COMM_GetICUStatusAddr(void);
uint32_t D_COMM_SetPEStatusInfo(icum_com_info_pe_status_t info, uint8_t set_value);
void D_COMM_SetInterruptEnable(void);
uint32_t D_COMM_SetSystemCallBackFunc(R_ICUMIF_CB_REGIST_t *p_regist_info);
uint32_t D_COMM_SetFlashCallBackFunc(R_ICUMIF_CB_REGIST_t *p_regist_info);
uint32_t D_COMM_GetICUMemCheckErrInfo(uint8_t *p_error);
void D_COMM_CallSystemCallBackFunc(uint32_t factor);
uint32_t D_COMM_PE_CheckMemorySharedRAM(uint32_t start_addr, uint32_t size);

#endif /* #if !defined(ICUM_D_COMM_PE_PUB_H) */

