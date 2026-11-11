/******************************************************************************/
/* Component Name   ICUM INT Driver                                           */
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

#if !defined(ICUM_D_INT_PUB_H)
#define ICUM_D_INT_PUB_H

/**************************************************************************************/
/* Return Value Definition                                                            */
/**************************************************************************************/
#define ER_D_INT_ERROR      (ER_D_INT_BASE+1UL)  /* State transition error */

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* EI interrupt channel number */
#define ICUM_INT_INTPES                (0U)         /* CPU(PE) interrupt request (single PE) */
#define ICUM_INT_INTPE0                (1U)         /* CPU(PE) interrupt request 0 */
#define ICUM_INT_INTPE1                (2U)         /* CPU(PE) interrupt request 1 */
#define ICUM_INT_INTPE2                (3U)         /* CPU(PE) interrupt request 2 */
#define ICUM_INT_INTICUAESE0RD         (4U)         /* Completion of ICUM_AESE0 encryption or decryption with ICUP */
#define ICUM_INT_ICICUTRNGHT           (5U)         /* [5:ICUM_ICICUTRNGHT      ][FFFE EA0AH]  Completion of ICUM_TRNGG0 health test */
#define ICUM_INT_INTICUTRNGC0          (6U)         /* ICUM_TRNGC0 output data ready */
#define ICUM_INT_INTICUOSTM0           (7U)         /* ICUM_OSTM0 overflow     */
#define ICUM_INT_INTICUOSTM1           (8U)         /* ICUM_OSTM1 overflow     */
#define ICUM_INT_INTICUWDTA0           (9U)         /* ICUM_WDTA0 counter 75%  */
#define ICUM_INT_INTEXTWDTB4           (10U)        /* WDTBA counter 75% WDTBA */
#define ICUM_INT_INTPKCCA0IRQ          (11U)        /* Completion of ICUM_PKCCA0 operation */
#define ICUM_INT_INTFACI0ERR           (12U)        /* FPSYS0 Flash access error interrupt */
#define ICUM_INT_INTFACI0ENDNM         (13U)        /* FPSYS0 Flash sequencer processing end interrupt */
#define ICUM_INT_INTFICUIF0ILA         (14U)        /* FPSYS0 Flash illegal access error interrupt */
#define ICUM_INT_INTICUTRNGERR         (15U)        /* Error Detection of ICUM_TRNGG0 health test */
#define ICUM_INT_RFU_16                (16U)        /* Unavailable channels */
#define ICUM_INT_RFU_17                (17U)        /* Unavailable channels */
#define ICUM_INT_RFU_18                (18U)        /* Unavailable channels */
#define ICUM_INT_INTFACI2ERR           (19U)        /* FPSYS2 Flash access error interrupt */
#define ICUM_INT_INTFACI2ENDNM         (20U)        /* FPSYS2 Flash sequencer processing end interrupt */
#define ICUM_INT_RFU_21                (21U)        /* Unavailable channels */
#define ICUM_INT_INTDCUDEGRAD          (22U)        /* Degradation interrupt request Degradation control */
#define ICUM_INT_INTAPBGERR            (23U)        /* Violation of access to ICUM_PBG */
#define ICUM_INT_INTICUSHAA0IREQ       (24U)        /* Input data request for ICUM_SHAA0 */
#define ICUM_INT_INTICUSHAA0OEND       (25U)        /* Completion or suspension of ICUM_SHAA0 data output */
#define ICUM_INT_INTECM0CFGERR         (26U)        /* Code flash guard error  */
#define ICUM_INT_INTECM0DFGERR         (27U)        /* Data flash guard error  */
#define ICUM_INT_INTEXTDCU20CDRQ       (28U)        /* Authentication challenge data request for ICUP debug */
#define ICUM_INT_INTEXTDCU20RDRQ       (29U)        /* Authentication response data compare request for ICUP/CPU(PE) debug */
#define ICUM_INT_INTEXTDCU20CDRQPE     (30U)        /* Authentication challenge data request for CPU(PE) debug */
#define ICUM_INT_RFU_31                (31U)        /* Unavailable channels */
#define ICUM_INT_INTRHSIFILA           (32U)        /* RHSIF0 int_hsif_sec interrupt */
#define ICUM_INT_INTPKCCA0SUSP         (33U)        /* PKCCA0 suspend completion */
#define ICUM_INT_INTERRCFDA            (34U)        /* Code flash data access error detect */
#define ICUM_INT_INTERRDFDA            (35U)        /* Data flash or other than the code flash and local RAM data access error detect */
#define ICUM_INT_INTUNINTDBG           (36U)        /* Unintended debug enable */
#define ICUM_INT_INTACEURD             (37U)        /* Completion of ACEU encryption or decryotion. Data ready to be read from the ACEU Block cipher register */
#define ICUM_INT_INTACEUDTFEND         (38U)        /* Interrupt end signal for ACEU direct data transfer[BCO] */
#define ICUM_INT_INTICUCLMAS           (39U)        /* Clock monitor error for ICUMHA */
#define ICUM_INT_INTPE3                (40U)        /* CPU(PE) interrupt request 3 */
#define ICUM_INT_INTPE4                (41U)        /* CPU(PE) interrupt request 4 */
#define ICUM_INT_INTPE5                (42U)        /* CPU(PE) interrupt request 5 */
#define ICUM_INT_INTPE6                (43U)        /* CPU(PE) interrupt request 6 */
#define ICUM_INT_INTPE7                (44U)        /* CPU(PE) interrupt request 7 */
#define ICUM_INT_RFU_45                (45U)        /* Unavailable channels */
#define ICUM_INT_RFU_46                (46U)        /* Unavailable channels */
#define ICUM_INT_INTICUSHA3A0END       (47U)        /* Completion or suspension of ICUM_SHA3A0 data output */
#define ICUM_INT_INTRCAN               (48U)        /* CAN interrution */
#define ICUM_INT_INTSDMAC              (49U)        /* sDMAC interruption */
#define ICUM_INT_INTICUDMACH0          (50U)        /* ICUM_DMAC channel 0 interrupt */
#define ICUM_INT_INTICUDMACH1          (51U)        /* ICUM_DMAC channel 1 interrupt */
#define ICUM_INT_INTICUDMACH2          (52U)        /* ICUM_DMAC channel 2 interrupt */
#define ICUM_INT_INTICUDMACH3          (53U)        /* ICUM_DMAC channel 3 interrupt */
#define ICUM_INT_INTICUDMAADDRERR      (54U)        /* ICUM_DMAC address error interrupt */
#define ICUM_INT_INTFSACSREQ0          (55U)        /* Flash sequencer access request for FPSYS0 ICUMHA Edge 1037H +0DCH   */
#define ICUM_INT_INTFSACSREQ1          (56U)        /* Flash sequencer access request for FPSYS1 ICUMHA Edge 1037H +0DCH   */
#define ICUM_INT_INTICUAESE0DTFEND     (57U)        /* Complition of ICUM_AESE0 encryption or decryption with DTF transfer */
#define ICUM_INT_INTICUAESE0DTFERR     (58U)        /*  SICAB slave error during DTF transfer */
#define ICUM_INT_INTICUAESE0CMPERR     (59U)        /* ICUM_AESE0 output comparison error */
#define ICUM_INT_INTVDDHLDET           (60U)        /* Temperature Sensor   */
#define ICUM_INT_RFU_61                (61U)        /* Unavailable channels */
#define ICUM_INT_INTOTULI_OTABE        (62U)        /* Temperature Sensor   */
#define ICUM_INT_INTECCDECERRPKCC      (63U)        /*  PKCC RAM ECC decoder error detect */
#define ICUM_INT_NB_CHANNELS           (64U)        /* End of channel       */

/* Operation mode change instruction */
#define ICUM_INT_ROMMODE               (1UL)         /* ROM operation mode   */
#define ICUM_INT_RAMMODE               (2UL)         /* RAM operation mode   */

/* Interrupt operation indication for a specific interrupt channel */
#define ICUM_INT_CLR_CHANNEL           (0U)         /* Channel clear designation (Interrupt Clear)    */
#define ICUM_INT_SET_CHANNEL           (1U)         /* Channel set designation (Interrupt Generation) */

/* Interrupt generation factor when an interrupt occurs handled by the INT driver */
#define ICUM_INT_SYSERR_LRAM_RMW       (1U)         /* (SYSERR interrupt) Local RAM read / modify write error */
#define ICUM_INT_SYSERR_LRAM_AREA      (2U)         /* (SYSERR interrupt) Local RAM area error */
#define ICUM_INT_SYSERR_BUS_ACC        (3U)         /* (SYSERR interrupt) Bus access error */
#define ICUM_INT_SYSERR_CODE_FLASH     (4U)         /* (SYSERR interrupt) Code flash error */
#define ICUM_INT_SYSERR_INST_FETCH     (17U)        /* (SYSERR interrupt) Instruction fetch */
#define ICUM_INT_FEINT_SYSTEM_STOP     (5U)         /* (FEINT interrupt) System control stop mode */
#define ICUM_INT_FEINT_ECCERR_SIC      (6U)         /* (FEINT interrupt) SIC ECC error */
#define ICUM_INT_FEINT_ECCERR_SICAB    (7U)         /* (FEINT interrupt) SIC AB ECC error */
#define ICUM_INT_FEINT_ECCERR_CRAM     (8U)         /* (FEINT interrupt) Cache RAM ECC error */
#define ICUM_INT_FEINT_ECCERR_LRAM     (9U)         /* (FEINT interrupt) Local RAM ECC error */
#define ICUM_INT_FEINT_ECCERR_PKRAM    (18U)        /* (FEINT interrupt) PKCCRAM ECC error */
#define ICUM_INT_FEINT_ECCERR_AESAXI   (19U)        /* (FEINT interrupt) AESAXI ECC error */
#define ICUM_INT_FETRAPEX_OCC          (10U)        /* (FETRAPEX interrupt)  */
#define ICUM_INT_EITRAP0_OCC           (11U)        /* (EITRAP0 interrupt)  */
#define ICUM_INT_EITRAP1_OCC           (12U)        /* (EITRAP1 interrupt)  */
#define ICUM_INT_RIEX_OCC              (13U)        /* (RIEX interrupt)     */
#define ICUM_INT_UCPOP_OCC             (14U)        /* (UCPOP interrupt)    */
#define ICUM_INT_PIEX_OCC              (15U)        /* (PIEX interrupt)     */
#define ICUM_INT_MAEX_OCC              (16U)        /* (MAEX interrupt)     */
#define ICUM_INT_EI_VOLTAGE_ERR        (20U)        /* (PIEX interrupt)     */
#define ICUM_INT_EI_TEMPERATURE_ERR    (21U)        /* (MAEX interrupt)     */
#define ICUM_INT_ILLEGAL_INTERRRUPT    (22U)        /* Illegal Interrupt    */

/* EI level interrupt source base offset */
#define ICUM_INT_EILVL_BASE            (0x1000UL)    /* EIIC31-0:Exception cause code is stored */

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* Registration information of the function to call back when receiving an interrupt */
/* 1st Element : Set the generated interrupt cause */
typedef void (icum_intc_interrupt_type_t) (uint32_t int_fact);
typedef struct ICUM_INT_CB_REGIST
{
    icum_intc_interrupt_type_t        *p_cbfunc;
} ICUM_INT_CB_REGIST_t;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
uint32_t D_INT_Init(ICUM_INT_CB_REGIST_t *p_regist_info);
uint32_t D_INT_EnableInterrupts(void);
uint32_t D_INT_DisableInterrupts(void);
uint32_t D_INT_GetRequestFlag(uint32_t channel, uint32_t *p_Result);
uint32_t D_INT_ChgExecMode(uint32_t chgmode);

#endif /* #if !defined(ICUM_D_INT_PUB_H) */
