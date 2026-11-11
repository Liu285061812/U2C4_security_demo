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

#if !defined(ICUP_IRQ_DEF_PUB)
#define ICUP_IRQ_DEF_PUB

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
#define INT_HNDL_NOT_REGIST          (0x00000000U)   /* Interrupt handler not registered */

/* FE interrupt (ROM operation) valid definition effective setting DEFINE definition */
/* 1 is set for the define value for which the interrupt is valid                    */
#define SYSERR_ENABLE_ROM            (0x00000001U)   /* [offset(010H):SYSERR      ] FE level SYSERR interrupt(1 fixed)        */
#define FETRAPEX_ENABLE_ROM          (0x00000001U)   /* [offset(030H):FETRAP      ] FE level FETRAP interrupt(1 fixed)        */
#define EITRAP0_ENABLE_ROM           (0x00000001U)   /* [offset(040H):TRAP0       ] FE level TRAP0  interrupt(1 fixed)        */
#define EITRAP1_ENABLE_ROM           (0x00000001U)   /* [offset(050H):TRAP1       ] FE level TRAP1  interrupt(1 fixed)        */
#define RIEX_ENABLE_ROM              (0x00000001U)   /* [offset(060H):RIE         ] FE level RIE    interrupt(1 fixed)        */
#define UCPOP_ENABLE_ROM             (0x00000000U)   /* [offset(080H):UCPOP       ] FE level UCPOP  interrupt(1 fixed)        */
#define MIP_MDP_ENABLE_ROM           (0x00000001U)   /* [offset(090H):MIPMDP      ] FE level MIPMDP interrupt(1 fixed)        */
#define PIEX_ENABLE_ROM              (0x00000001U)   /* [offset(0A0H):PIE         ] FE level PIE    interrupt(1 fixed)        */
#define MAEX_ENABLE_ROM              (0x00000001U)   /* [offset(0C0H):MAE         ] FE level MAE    interrupt(1 fixed)        */
#define FENMI_ENABLE_ROM             (0x00000001U)   /* [offset(0E0H):FENMI       ] FE level FENMI  interrupt(1 fixed)        */
#define FEINT_ENABLE_ROM             (0x00000001U)   /* [offset(0F0H):FEINT       ] FE level FEINT  interrupt                 */
#define EINTPRIO_0_ENABLE_ROM        (0x00000001U)   /* [offset(100H):EI level 0  ] EI level user interrupt(fixed 1)          */
#define EINTPRIO_1_ENABLE_ROM        (0x00000000U)   /* [offset(110H):EI level 1  ] Not Used    */
#define EINTPRIO_2_ENABLE_ROM        (0x00000000U)   /* [offset(120H):EI level 2  ] Not Used    */
#define EINTPRIO_3_ENABLE_ROM        (0x00000000U)   /* [offset(130H):EI level 3  ] Not Used    */
#define EINTPRIO_4_ENABLE_ROM        (0x00000000U)   /* [offset(140H):EI level 4  ] Not Used    */
#define EINTPRIO_5_ENABLE_ROM        (0x00000000U)   /* [offset(150H):EI level 5  ] Not Used    */
#define EINTPRIO_6_ENABLE_ROM        (0x00000000U)   /* [offset(160H):EI level 6  ] Not Used    */
#define EINTPRIO_7_ENABLE_ROM        (0x00000000U)   /* [offset(170H):EI level 7  ] Not Used    */

/* FE interrupt (RAM operation) valid definition effective setting DEFINE definition */
/* 1 is set for the define value for which the interrupt is valid                    */
#define SYSERR_ENABLE_RAM            (0x00000000U)
#define FETRAPEX_ENABLE_RAM          (0x00000001U)
#define EITRAP0_ENABLE_RAM           (0x00000001U)
#define EITRAP1_ENABLE_RAM           (0x00000001U)
#define RIEX_ENABLE_RAM              (0x00000001U)
#define UCPOP_ENABLE_RAM             (0x00000000U)
#define MIP_MDP_ENABLE_RAM           (0x00000001U)
#define PIEX_ENABLE_RAM              (0x00000001U)
#define MAEX_ENABLE_RAM              (0x00000001U)
#define FENMI_ENABLE_RAM             (0x00000001U)
#define FEINT_ENABLE_RAM             (0x00000001U)

#define EINTPRIO_0_ENABLE_RAM        (0x00000001U)
#define EINTPRIO_1_ENABLE_RAM        (0x00000000U)
#define EINTPRIO_2_ENABLE_RAM        (0x00000000U)
#define EINTPRIO_3_ENABLE_RAM        (0x00000000U)
#define EINTPRIO_4_ENABLE_RAM        (0x00000000U)
#define EINTPRIO_5_ENABLE_RAM        (0x00000000U)
#define EINTPRIO_6_ENABLE_RAM        (0x00000000U)
#define EINTPRIO_7_ENABLE_RAM        (0x00000000U)

/* EI interrupt (ROM operation) valid definition effective setting DEFINE definition */
/* 1 is set for the define value for which the interrupt is valid                    */
#define INTPES_ENABLE_ROM            (0x00000001U)   /* [00:ICUM_ICPES            ][FFFE EA00H]  CPU(PE) interrupt request (single PE) ICUM_CMDREG           */
#define INTPE0_ENABLE_ROM            (0x00000001U)   /* [01:ICUM_ICPE0            ][FFFE EA02H]  CPU(PE) interrupt request 0 ICUM_CMDREG  */
#define INTPE1_ENABLE_ROM            (0x00000001U)   /* [02:ICUM_ICPE1            ][FFFE EA04H]  CPU(PE) interrupt request 1 ICUM_CMDREG  */
#define INTPE2_ENABLE_ROM            (0x00000001U)   /* [03:ICUM_ICPE2            ][FFFE EA06H]  CPU(PE) interrupt request 2 ICUM_CMDREG  */
#define INTICUAESE0RD_ENABLE_ROM     (0x00000000U)   /* [04:ICUM_ICICUAESE0RD     ][FFFE EA08H]  Completion of ICUM_AESE0 encryption or decryption with ICUP ICUM_AESE0 */
#define INTICUTRNGHT_ENABLE_ROM      (0x00000001U)   /* [05:ICUM_ICICUTRNGHT      ][FFFE EA0AH]  Completion of ICUM_TRNGC0 health test ICUM_TRNGC0 */
#define INTICUTRNGG0_ENABLE_ROM      (0x00000001U)   /* [06:ICUM_ICICUTRNGC0      ][FFFE EA0CH]  ICUM_TRNGC0 output data ready ICUM_TRNGC0 */
#define INTICUOSTM0_ENABLE_ROM       (0x00000001U)   /* [07:ICUM_ICICUOSTM0       ][FFFE EA0EH]  ICUM_OSTM0 overflow ICUM_OSTM0            */
#define INTICUOSTM1_ENABLE_ROM       (0x00000000U)   /* [08:ICUM_ICICUOSTM1       ][FFFE EA10H]  ICUM_OSTM1 overflow ICUM_OSTM1            */
#define INTICUWDTA0_ENABLE_ROM       (0x00000000U)   /* [09:ICUM_ICICUWDTA0       ][FFFE EA12H]  ICUM_WDTA0 counter 75% ICUM_WDTA0         */
#define INTEXTWDTB4_ENABLE_ROM       (0x00000000U)   /* [10:ICUM_ICEXTWDTB4       ][FFFE EA14H]  WDTBA counter 75% WDTB4                   */
#define INTPKCCA0_ENABLE_ROM         (0x00000000U)   /* [11:ICUM_ICPKCCA0         ][FFFE EA16H]  Completion of ICUM_PKCCA0 operation ICUM_PKCC */
#define INTFACI0ERR_ENABLE_ROM       (0x00000000U)   /* [12:ICUM_ICFACI0ERR       ][FFFE EA18H]  FPSYS0 Flash access error interrupt             */
#define INTFACI0ENDNM_ENABLE_ROM     (0x00000000U)   /* [13:ICUM_ICFACI0ENDNM     ][FFFE EA1AH]  FPSYS0 Flash sequencer processing end interrupt */
#define INTFICUIF0ILA_ENABLE_ROM     (0x00000000U)   /* [14:ICUM_ICFICUIF0ILA     ][FFFE EA1CH]  FPSYS0 Flash illegal access error interrupt     */
#define INTICUTRNGERR_ENABLE_ROM     (0x00000000U)   /* [15:ICUM_ICICUTRNGERR     ][FFFE EA1EH]  Error detection of ICUM_TRNGC0 health test ICUM_TRNGC0 */
#define INTFACI2ERR_ENABLE_ROM       (0x00000000U)   /* [19:ICUM_ICFACI2ERR       ][FFFE EA26H]  FPSYS2 Flash access error interrupt             */
#define INTFACI2ENDNM_ENABLE_ROM     (0x00000000U)   /* [20:ICUM_ICFACI2ENDNM     ][FFFE EA28H]  FPSYS2 Flash sequencer processing end interrupt */
#define INTDCUDEGRAD_ENABLE_ROM      (0x00000001U)   /* [22:ICUM_ICDCUDEGRAD      ][FFFE EA2CH]  Degradation interrupt request                   */
#define INTAPBGERR_ENABLE_ROM        (0x00000000U)   /* [23:ICUM_ICAPBGERR        ][FFFE EA2EH]  Violation of access to ICUM_PBG                 */
#define INTICUSHAA0IREQ_ENABLE_ROM   (0x00000000U)   /* [24:ICUM_ICICUSHAA0IREQ   ][FFFE EA30H]  Input data request for ICUM_SHAA0               */
#define INTICUSHAA0OEND_ENABLE_ROM   (0x00000000U)   /* [25:ICUM_ICICUSHAA0OEND   ][FFFE EA32H]  Completion or suspension of ICUM_SHAA0 data output */
#define INTECM0CFGERR_ENABLE_ROM     (0x00000000U)   /* [26:ICUM_ICECM0CFGERR     ][FFFE EA34H]  Code flash guard error */
#define INTECM0DFGERR_ENABLE_ROM     (0x00000000U)   /* [27:ICUM_ICECM0DFGERR     ][FFFE EA36H]  Data flash guard error */
#define INTEXTDCU20CDRQ_ENABLE_ROM   (0x00000001U)   /* [28:ICUM_ICEXTDCU20CDRQ   ][FFFE EA38H]  Authentication challenge data request for ICUP debug    */
#define INTEXTDCU20RDRQ_ENABLE_ROM   (0x00000001U)   /* [29:ICUM_ICEXTDCU20RDRQ   ][FFFE EA3AH]  Authentication response data compare request for ICUP/CPU(PE) debug */
#define INTEXTDCU20CDRQPE_ENABLE_ROM (0x00000001U)   /* [30:ICUM_ICEXTDCU20CDRQPE ][FFFE EA3CH]  Authentication challenge data request for CPU(PE) debug */

#define INTRHSIFILA_ENABLE_ROM       (0x00000000U)   /* [32:ICUM_ICRHSIFILA       ][FFFE F040H]  RHSIF0 int_hsif_sec interrupt       */
#define INTPKCCA0SUSP_ENABLE_ROM     (0x00000000U)   /* [33:ICUM_ICPKCCA0SUSP     ][FFFE F042H]  PKCCA0 suspend completion           */
#define INTERRCFDA_ENABLE_ROM        (0x00000000U)   /* [34:ICUM_ICERRCFDA        ][FFFE F044H]  Code flash data access error detect */
#define INTERRDFDA_ENABLE_ROM        (0x00000000U)   /* [35:ICUM_ICERRDFDA        ][FFFE F046H]  Data flash or other than the code flash and local RAM data access error detect */
#define INTUNINTDBG_ENABLE_ROM       (0x00000000U)   /* [36:ICUM_ICUNINTDBG       ][FFFE F048H]  Unintended debug enable             */
#define INTACEURD_ENABLE_ROM         (0x00000000U)   /* [37:ICUM_ICACEURD         ][FFFE F04AH]  Completion of ACEU ercryption or decryption  */
#define INTACEUDTFEND_ENABLE_ROM     (0x00000000U)   /* [38:ICUM_ICACEUDTFEND     ][FFFE F04CH]  Interrupt end signal for ACER direct data tgransfer[BCO] */
#define INTICUCLMAS_ENABLE_ROM       (0x00000001U)   /* [39:ICUM_ICICUCLMAS       ][FFFE F04EH]  Clock monitor error for ICUMHB            */
#define INTPE3_ENABLE_ROM            (0x00000000U)   /* [40:ICUM_ICPE3            ][FFFE F050H]  CPU(PE) interrupt request 3 ICUM_CMDREG  */
#define INTPE4_ENABLE_ROM            (0x00000000U)   /* [41:ICUM_ICPE4            ][FFFE F052H]  CPU(PE) interrupt request 4 ICUM_CMDREG  */
#define INTPE5_ENABLE_ROM            (0x00000000U)   /* [42:ICUM_ICPE5            ][FFFE F054H]  CPU(PE) interrupt request 5 ICUM_CMDREG  */
#define INTPE6_ENABLE_ROM            (0x00000000U)   /* [43:ICUM_ICPE6            ][FFFE F056H]  CPU(PE) interrupt request 6 ICUM_CMDREG  */
#define INTPE7_ENABLE_ROM            (0x00000000U)   /* [44:ICUM_ICPE7            ][FFFE F058H]  CPU(PE) interrupt request 7 ICUM_CMDREG  */
#define INTSHAOEND_ENABLE_ROM        (0x00000000U)   /* [47:ICUM_ICSHAOEND        ][FFFE F05EH]  Completion or susupension of ICUM_SHA3A0 data output ICUM_SHA3A0 */
#define INTRCAN_ENABLE_ROM           (0x00000000U)   /* [48:ICUM_ICRCAN           ][FFFE F060H]  CAN interrut RS-CANFD */
#define INTSD_ENABLE_ROM             (0x00000000U)   /* [49:ICUM_ICSD             ][FFFE F062H]  sDMAC interrupt sDMAC */
#define INTDM0_ENABLE_ROM            (0x00000000U)   /* [50:ICUM_ICDM0            ][FFFE F064H]  ICUM_DMAC channel 0 interrupt ICUM_DMAC */
#define INTDM1_ENABLE_ROM            (0x00000000U)   /* [51:ICUM_ICDM0            ][FFFE F066H]  ICUM_DMAC channel 1 interrupt ICUM_DMAC */
#define INTDM2_ENABLE_ROM            (0x00000000U)   /* [52:ICUM_ICDM0            ][FFFE F068H]  ICUM_DMAC channel 2 interrupt ICUM_DMAC */
#define INTDM3_ENABLE_ROM            (0x00000000U)   /* [53:ICUM_ICDM0            ][FFFE F06AH]  ICUM_DMAC channel 3 interrupt ICUM_DMAC */
#define INTDMAE_ENABLE_ROM           (0x00000000U)   /* [54:ICUM_ICDMAE           ][FFFE F06CH]  ICUM_DMAC address error interrupt ICUM_DMAC */
#define INTFSACSREQ0_ENABLE_ROM      (0x00000001U)   /* [55:ICUM_ICFSACSREQ0      ][FFFE F06EH]  Flash sequencer access request for FPSYS0 */
#define INTFSACSREQ1_ENABLE_ROM      (0x00000001U)   /* [56:ICUM_ICFSACSREQ1      ][FFFE F070H]  Flash sequencer access request for FPSYS1 */
#define INTICUAESE0DTFEND_ENABLE_ROM (0x00000001U)   /* [57:ICUM_ICICUAESE0DTFEND ][FFFE F072H]  Complition of ICUM_AESE0 encryption or decryption with DTF transfer */
#define INTICUAESE0DTFERR_ENABLE_ROM (0x00000001U)   /* [58:ICUM_ICICUAESE0DTFERR ][FFFE F074H]  SICAB slave error during DTF transfer */
#define INTICUAESE0CMPERR_ENABLE_ROM (0x00000001U)   /* [59:ICUM_ICICUAESE0CMPERR ][FFFE F076H]  ICUM_AESE0 output comparison error    */
#define INTVDDHLDET_ENABLE_ROM       (0x00000001U)   /* [60:ICUM_ICVDDHLDET       ][FFFE F078H]  Temperature Sensor */
#define INTOTULI_OTABE_ENABLE_ROM    (0x00000001U)   /* [62:ICUM_ICOTULI_OTABE    ][FFFE F07CH]  Temperature Sensor */
#define INTACEUCMPERR_ENABLE_ROM     (0x00000000U)   /* [63:ICUM_ICACEUCMPERR     ][FFFE F07EH]  ACEU output comparison error ACEU0_AESE0/ACEU1_AESE0 */

/* EI interrupt (RAM operation) valid definition effective setting DEFINE definition */
/* 1 is set for the define value for which the interrupt is valid                    */
#define INTPES_ENABLE_RAM            (0x00000001U)
#define INTPE0_ENABLE_RAM            (0x00000001U)
#define INTPE1_ENABLE_RAM            (0x00000001U)
#define INTPE2_ENABLE_RAM            (0x00000001U)
#define INTICUAESE0RD_ENABLE_RAM     (0x00000000U)
#define INTICUTRNGHT_ENABLE_RAM      (0x00000000U)
#define INTICUTRNGG0_ENABLE_RAM      (0x00000000U)
#define INTICUOSTM0_ENABLE_RAM       (0x00000001U)
#define INTICUOSTM1_ENABLE_RAM       (0x00000000U)
#define INTICUWDTA0_ENABLE_RAM       (0x00000000U)
#define INTEXTWDTB4_ENABLE_RAM       (0x00000000U)
#define INTPKCCA0_ENABLE_RAM         (0x00000000U)
#define INTFACI0ERR_ENABLE_RAM       (0x00000000U)
#define INTFACI0ENDNM_ENABLE_RAM     (0x00000000U)
#define INTFICUIF0ILA_ENABLE_RAM     (0x00000000U)
#define INTICUTRNGERR_ENABLE_RAM     (0x00000000U)
#define INTFACI2ERR_ENABLE_RAM       (0x00000000U)
#define INTFACI2ENDNM_ENABLE_RAM     (0x00000000U)
#define INTDCUDEGRAD_ENABLE_RAM      (0x00000000U)
#define INTAPBGERR_ENABLE_RAM        (0x00000000U)
#define INTICUSHAA0IREQ_ENABLE_RAM   (0x00000000U)
#define INTICUSHAA0OEND_ENABLE_RAM   (0x00000000U)
#define INTECM0CFGERR_ENABLE_RAM     (0x00000000U)
#define INTECM0DFGERR_ENABLE_RAM     (0x00000000U)
#define INTEXTDCU20CDRQ_ENABLE_RAM   (0x00000000U)
#define INTEXTDCU20RDRQ_ENABLE_RAM   (0x00000000U)
#define INTEXTDCU20CDRQPE_ENABLE_RAM (0x00000000U)

#define INTRHSIFILA_ENABLE_RAM       (0x00000000U)
#define INTPKCCA0SUSP_ENABLE_RAM     (0x00000000U)
#define INTERRCFDA_ENABLE_RAM        (0x00000000U)
#define INTERRDFDA_ENABLE_RAM        (0x00000000U)
#define INTUNINTDBG_ENABLE_RAM       (0x00000000U)
#define INTACEURD_ENABLE_RAM         (0x00000000U)
#define INTACEUDTFEND_ENABLE_RAM     (0x00000000U)
#define INTICUCLMAS_ENABLE_RAM       (0x00000001U)
#define INTPE3_ENABLE_RAM            (0x00000000U)
#define INTPE4_ENABLE_RAM            (0x00000000U)
#define INTPE5_ENABLE_RAM            (0x00000000U)
#define INTPE6_ENABLE_RAM            (0x00000000U)
#define INTPE7_ENABLE_RAM            (0x00000000U)
#define INTSHAOEND_ENABLE_RAM        (0x00000000U)
#define INTRCAN_ENABLE_RAM           (0x00000000U)
#define INTSD_ENABLE_RAM             (0x00000000U)
#define INTDM0_ENABLE_RAM            (0x00000000U)
#define INTDM1_ENABLE_RAM            (0x00000000U)
#define INTDM2_ENABLE_RAM            (0x00000000U)
#define INTDM3_ENABLE_RAM            (0x00000000U)
#define INTDMAE_ENABLE_RAM           (0x00000000U)
#define INTFSACSREQ0_ENABLE_RAM      (0x00000001U)
#define INTFSACSREQ1_ENABLE_RAM      (0x00000001U)
#define INTICUAESE0DTFEND_ENABLE_RAM (0x00000000U)
#define INTICUAESE0DTFERR_ENABLE_RAM (0x00000000U)
#define INTICUAESE0CMPERR_ENABLE_RAM (0x00000000U)
#define INTVDDHLDET_ENABLE_RAM       (0x00000001U)
#define INTOTULI_OTABE_ENABLE_RAM    (0x00000001U)
#define INTACEUCMPERR_ENABLE_RAM     (0x00000000U)

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* None */

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
/* None */

#endif /* #if !defined(ICUP_IRQ_DEF_PUB) */

