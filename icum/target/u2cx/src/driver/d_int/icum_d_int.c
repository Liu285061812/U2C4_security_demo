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
/**************************************************************************************/
/* Header files                                                                       */
/**************************************************************************************/
#include ICUM_DEVICE_FILE
#include "r_icumif_api.h"
#include "renesas_types.h"

#include "u2b_reg_adr.h"

#include "icum_common_pub.h"
#include "icum_irq_define_pub.h"
#include "icum_int_declaration_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_d_flash_pub.h"
#include "icum_d_int_pub.h"
#include "icum_d_int_lcl.h"
#include "icum_s_pub.h"

/**************************************************************************************/
/* Global Variable                                                                    */
/**************************************************************************************/
/* Callback registration at interrupt occurrence */
static ICUM_INT_CB_REGIST_t s_cb_Reg_intc;

static uint32_t s_intcdrv_status;
static uint32_t s_ei_imr0_unmuskSet;
static uint32_t s_ei_imr1_unmuskSet;

static r_icumif_isd_t g_deg_internal_service;

#pragma ghs section rodata=".icum_driver_const_ram"
/**************************************************************************************/
/* Global Variable                                                                    */
/* Table for capturing FE interrupts during RAM operation                             */
/**************************************************************************************/
static ICUM_INT_FE_FPTBL_t s_feit_fptbl_ram[ICUM_INT_FETBL_MAX] =
{
                                                          /* 00[offset(000H):RESET ]  */
    { (uint16_t)0x0000, { (uint8_t)0x0, (uint8_t)0x0 },   NULL, { (uint32_t)0, (uint32_t)0 } },
#if (SYSERR_ENABLE_RAM > INT_HNDL_NOT_REGIST)             /* 01[offset(010H):SYSERR]  */
    { (uint16_t)0x0010, { (uint8_t)0xe0, (uint8_t)0x06 }, NULL, { (uint32_t)0, (uint32_t)0 } },
#else
    { (uint16_t)0x0010, { (uint8_t)0xe0, (uint8_t)0x06 }, NULL, { (uint32_t)0, (uint32_t)0 } },
#endif
                                                          /* 02[offset(020H):HVTRAP]  */
    { (uint16_t)0x0020, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#if (FETRAPEX_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 03[offset(030H):FETRAP]  */
    { (uint16_t)0x0030, { (uint8_t)0xe0, (uint8_t)0x06 }, FETRAPEX_RAM, { (uint32_t)0, (uint32_t)0 } },
#else
    { (uint16_t)0x0030, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#endif
#if (EITRAP0_ENABLE_RAM > INT_HNDL_NOT_REGIST)            /* 04[offset(040H):TRAP0]   */
    { (uint16_t)0x0040, { (uint8_t)0xe0, (uint8_t)0x06 }, EITRAP0_RAM, { (uint32_t)0, (uint32_t)0 } },
#else
    { (uint16_t)0x0040, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#endif
#if (EITRAP1_ENABLE_RAM > INT_HNDL_NOT_REGIST)            /* 05[offset(050H):TRAP1]   */
    { (uint16_t)0x0050, { (uint8_t)0xe0, (uint8_t)0x06 }, EITRAP1_RAM, { (uint32_t)0, (uint32_t)0 } },
#else
    { (uint16_t)0x0050, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#endif
#if (RIEX_ENABLE_RAM > INT_HNDL_NOT_REGIST)               /* 06[offset(060H):RIE]     */
    { (uint16_t)0x0060, { (uint8_t)0xe0, (uint8_t)0x06 }, RIEX_RAM, { (uint32_t)0, (uint32_t)0 } },
#else
    { (uint16_t)0x0060, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#endif
                                                          /* 07[offset(070H):FPP/FPI] */
    { (uint16_t)0x0070, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#if (UCPOP_ENABLE_RAM > INT_HNDL_NOT_REGIST)              /* 08[offset(080H):UCPOP]   */
    { (uint16_t)0x0080, { (uint8_t)0xe0, (uint8_t)0x06 }, UCPOP_RAM, { (uint32_t)0, (uint32_t)0 } },
#else
    { (uint16_t)0x0080, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#endif
#if (MIP_MDP_ENABLE_RAM > INT_HNDL_NOT_REGIST)            /* 09[offset(090H):MIPMDP]  */
    { (uint16_t)0x0090, { (uint8_t)0xe0, (uint8_t)0x06 }, MIP_MDP_RAM, { (uint32_t)0, (uint32_t)0 } },
#else
    { (uint16_t)0x0090, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#endif
#if (PIEX_ENABLE_RAM > INT_HNDL_NOT_REGIST)               /* 10[offset(0A0H):PIE]     */
    { (uint16_t)0x00A0, { (uint8_t)0xe0, (uint8_t)0x06 }, PIEX_RAM, { (uint32_t)0, (uint32_t)0 } },
#else
    { (uint16_t)0x00A0, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#endif
                                                          /* 11[offset(0B0H):DEBUG]   */
    { (uint16_t)0x00B0, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#if (MAEX_ENABLE_RAM > INT_HNDL_NOT_REGIST)               /* 12[offset(0C0H):MAE]     */
    { (uint16_t)0x00C0, { (uint8_t)0xe0, (uint8_t)0x06 }, MAEX_RAM, { (uint32_t)0, (uint32_t)0 } },
#else
    { (uint16_t)0x00C0, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#endif
                                                          /* 13[offset(0D0H):(R.F.U)] */
    { (uint16_t)0x00D0, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#if (FENMI_ENABLE_RAM > INT_HNDL_NOT_REGIST)              /* 14[offset(0E0H):FENMI]   */
    { (uint16_t)0x00E0, { (uint8_t)0xe0, (uint8_t)0x06 }, FENMI_RAM, { (uint32_t)0, (uint32_t)0 } },
#else
    { (uint16_t)0x00E0, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#endif
#if (FEINT_ENABLE_RAM > INT_HNDL_NOT_REGIST)              /* 15[offset(0F0H):FEINT]   */
    { (uint16_t)0x00F0, { (uint8_t)0xe0, (uint8_t)0x06 }, FEINT_RAM, { (uint32_t)0, (uint32_t)0 } },
#else
    { (uint16_t)0x00F0, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } },
#endif
#if (EINTPRIO_0_ENABLE_RAM > INT_HNDL_NOT_REGIST)         /* 16[offset(100H):EIlevel0] */
    { (uint16_t)0x0100, { (uint8_t)0xe0, (uint8_t)0x06 }, EINTPRIO_0_RAM, { (uint32_t)0, (uint32_t)0 } }
#else
    { (uint16_t)0x0100, { (uint8_t)0xe0, (uint8_t)0x06 }, infinite_loop, { (uint32_t)0, (uint32_t)0 } }
#endif
};
#pragma ghs section rodata=default

/**************************************************************************************/
/* Table for capturing EI interrupts during ROM operation                             */
/**************************************************************************************/
static ICUM_INT_EI_FPTBL_t s_eiit_fptbl_rom[ICUM_INT_EITBL_MAX] =
{
#if (INTPES_ENABLE_ROM > INT_HNDL_NOT_REGIST)           /* 00: CPU(PE) interrupt request (single PE) ICUM_CMDREG */
    { ICPES },
#else
    { NULL },
#endif
#if (INTPE0_ENABLE_ROM > INT_HNDL_NOT_REGIST)           /* 01: CPU(PE) interrupt request 0 ICUM_CMDREG */
    { ICPE0 },
#else
    { NULL },
#endif
#if (INTPE1_ENABLE_ROM > INT_HNDL_NOT_REGIST)           /* 02: CPU(PE) interrupt request 2 ICUM_CMDREG */
    { ICPE1 },
#else
    { NULL },
#endif
#if (INTPE2_ENABLE_ROM > INT_HNDL_NOT_REGIST)           /* 03: CPU(PE) interrupt request 3 ICUM_CMDREG */
    { ICPE2 },
#else
    { NULL },
#endif
#if (INTICUAESE0RD_ENABLE_ROM > INT_HNDL_NOT_REGIST)    /* 04: Completion of AES encryption or decryption with ICUP */
    { ICICUAESE0RD },
#else
    { NULL },
#endif
#if (INTICUTRNGHT_ENABLE_ROM > INT_HNDL_NOT_REGIST)     /* 05: Completion of ICUM_TRNGC0 health test ICUM_TRNGC0 */
    { ICICUTRNGHT },
#else
    { NULL },
#endif
#if (INTICUTRNGG0_ENABLE_ROM > INT_HNDL_NOT_REGIST)     /* 06: ICUM_TRNGC0 output data ready ICUM_TRNGC0 */
    { ICICUTRNGG0 },
#else
    { NULL },
#endif
#if (INTICUOSTM0_ENABLE_ROM > INT_HNDL_NOT_REGIST)      /* 07: ICUM_OSTM0 overflow ICUM_OSTM0 */
    { INTICUOSTM0 },
#else
    { NULL },
#endif
#if (INTICUOSTM1_ENABLE_ROM > INT_HNDL_NOT_REGIST)      /* 08: ICUM_OSTM1 overflow ICUM_OSTM1 */
    { ICICUOSTM1 },
#else
    { NULL },
#endif
#if (INTICUWDTA0_ENABLE_ROM > INT_HNDL_NOT_REGIST)      /* 09: ICUM_WDTA0 counter 75% ICUM_WDTA0 */
    { ICICUWDTA0 },
#else
    { NULL },
#endif
#if (INTEXTWDTB4_ENABLE_ROM > INT_HNDL_NOT_REGIST)      /* 10: WDTBA counter 75% WDTBA */
    { ICEXTWDTB4 },
#else
    { NULL },
#endif
#if (INTPKCCA0_ENABLE_ROM > INT_HNDL_NOT_REGIST)        /* 11: Completion of ICUM_PKCCA0 operation ICUM_PKCC */
    { ICPKCCA0 },
#else
    { NULL },
#endif
#if (INTFACI0ERR_ENABLE_ROM > INT_HNDL_NOT_REGIST)      /* 12: FPSYS0 Flash access error interrupt */
    { ICFACI0ERR },
#else
    { NULL },
#endif
#if (INTFACI0ENDNM_ENABLE_ROM > INT_HNDL_NOT_REGIST)    /* 13: FPSYS0 Flash sequencer processing end interrupt */
    { ICFACI0ENDNM },
#else
    { NULL },
#endif
#if (INTFICUIF0ILA_ENABLE_ROM > INT_HNDL_NOT_REGIST)    /* 14: FPSYS0 Flash illegal access error interrupt */
    { ICFICUIF0ILA },
#else
    { NULL },
#endif
#if (INTICUTRNGERR_ENABLE_ROM > INT_HNDL_NOT_REGIST)    /* 15: Error detection of ICUM_TRNGC0 health test ICUM_TRNGC0 */
    { ICICUTRNGERR },
#else
    { NULL },
#endif
    { NULL },     /* 16: Not Used */
    { NULL },     /* 17: Not Used */
    { NULL },     /* 18: Not Used */
#if (INTFACI2ERR_ENABLE_ROM > INT_HNDL_NOT_REGIST)      /* 19: FPSYS2 Flash access error interrupt */
    { ICFACI2ERR },
#else
    { NULL },
#endif
#if (INTFACI2ENDNM_ENABLE_ROM > INT_HNDL_NOT_REGIST)    /* 20: FPSYS2 Flash sequencer processing end interrupt */
    { ICFACI2ENDNM },
#else
    { NULL },
#endif
    { NULL },     /* 21: Not Used */
#if (INTDCUDEGRAD_ENABLE_ROM > INT_HNDL_NOT_REGIST)     /* 22: Degradation interrupt request */
    { ICDCUDEGRAD },
#else
    { NULL },
#endif
#if (INTAPBGERR_ENABLE_ROM > INT_HNDL_NOT_REGIST)       /* 23: Violation of access to ICUM_PBG  */
    { ICAPBGERR },
#else
    { NULL },
#endif
#if (INTICUSHAA0IREQ_ENABLE_ROM > INT_HNDL_NOT_REGIST)  /* 24: Input data request for ICUM_SHAA0 */
    { ICICUSHAA0IREQ },
#else
    { NULL },
#endif
#if (INTICUSHAA0OEND_ENABLE_ROM > INT_HNDL_NOT_REGIST)  /* 25: Completion or suspension of ICUM_SHAA0 data output */
    { ICICUSHAA0OEND },
#else
    { NULL },
#endif
#if (INTECM0CFGERR_ENABLE_ROM > INT_HNDL_NOT_REGIST)    /* 26: Code flash guard error */
    { ICECM0CFGERR },
#else
    { NULL },
#endif
#if (INTECM0DFGERR_ENABLE_ROM > INT_HNDL_NOT_REGIST)    /* 27: Data flash guard error */
    { ICECM0DFGERR },
#else
    { NULL },
#endif
#if (INTEXTDCU20CDRQ_ENABLE_ROM > INT_HNDL_NOT_REGIST)  /* 28: Authentication challenge data request for ICUP debug */
    { INTEXTDCU20CDRQ },
#else
    { NULL },
#endif
#if (INTEXTDCU20RDRQ_ENABLE_ROM > INT_HNDL_NOT_REGIST)  /* 29: Authentication res data comp req for ICUP/PE debug */
    { INTEXTDCU20RDRQ },
#else
    { NULL },
#endif
#if (INTEXTDCU20CDRQPE_ENABLE_ROM > INT_HNDL_NOT_REGIST)/* 30: Authentication challenge data request for PE debug */
    { INTEXTDCU20CDRQPE },
#else
    { NULL },
#endif
    { NULL },     /* 31: Not Used */

#if (INTRHSIFILA_ENABLE_ROM > INT_HNDL_NOT_REGIST)      /* 32: RHSIF0 int_hsif_sec interrupt */
    { ICRHSIFILA },
#else
    { NULL },
#endif
#if (INTPKCCA0SUSP_ENABLE_ROM > INT_HNDL_NOT_REGIST)    /* 33: PKCCA0 suspend completion */
    { ICPKCCA0SUSP },
#else
    { NULL },
#endif
#if (INTERRCFDA_ENABLE_ROM > INT_HNDL_NOT_REGIST)       /* 34: Code flash data access error detect */
    { ICERRCFDA },
#else
    { NULL },
#endif
#if (INTERRDFDA_ENABLE_ROM > INT_HNDL_NOT_REGIST)       /* 35: DF or local RAM data access error detect */
    { ICERRDFDA },
#else
    { NULL },
#endif
#if (INTUNINTDBG_ENABLE_ROM > INT_HNDL_NOT_REGIST)      /* 36: Unintended debug enable */
    { ICUNINTDBG },
#else
    { NULL },
#endif
#if (INTACEURD_ENABLE_ROM > INT_HNDL_NOT_REGIST)        /* 37: Completion of ACEU ercryption or decryption */
    { ICACEURD },
#else
    { NULL },
#endif
#if (INTACEUDTFEND_ENABLE_ROM > INT_HNDL_NOT_REGIST)    /* 38: Interrupt end signal for ACER direct data tgransfer[BCO] */
    { ICACEUDTFEND },
#else
    { NULL },
#endif
#if (INTICUCLMAS_ENABLE_ROM > INT_HNDL_NOT_REGIST)      /* 39: Clock monitor error for ICUMHB */
    { INTICUCLMA },
#else
    { NULL },
#endif
#if (INTPE3_ENABLE_ROM > INT_HNDL_NOT_REGIST)           /* 40: CPU(PE) interrupt request 3 ICUM_CMDREG */
    { ICPE3 },
#else
    { NULL },
#endif
#if (INTPE4_ENABLE_ROM > INT_HNDL_NOT_REGIST)           /* 41: CPU(PE) interrupt request 4 ICUM_CMDREG */
    { ICPE4 },
#else
    { NULL },
#endif
#if (INTPE5_ENABLE_ROM > INT_HNDL_NOT_REGIST)           /* 42: CPU(PE) interrupt request 5 ICUM_CMDREG */
    { ICPE5 },
#else
    { NULL },
#endif
#if (INTPE6_ENABLE_ROM > INT_HNDL_NOT_REGIST)           /* 43: CPU(PE) interrupt request 6 ICUM_CMDREG */
    { ICPE6 },
#else
    { NULL },
#endif
#if (INTPE7_ENABLE_ROM > INT_HNDL_NOT_REGIST)           /* 44: CPU(PE) interrupt request 7 ICUM_CMDREG */
    { ICPE7 },
#else
    { NULL },
#endif
    { NULL },     /* 45: Not Used */
    { NULL },     /* 46: Not Used */
#if (INTSHAOEND_ENABLE_ROM > INT_HNDL_NOT_REGIST)       /* 47: Completion or susupension of ICUM_SHA3A0 data output ICUM_SHA3A0 */
    { ICSHAOEND },
#else
    { NULL },
#endif
#if (INTRCAN_ENABLE_ROM > INT_HNDL_NOT_REGIST)          /* 48: CAN interrut RS-CANFD */
    { ICRCAN },
#else
    { NULL },
#endif
#if (INTSD_ENABLE_ROM > INT_HNDL_NOT_REGIST)            /* 49: sDMAC interrupt sDMAC */
    { ICSD },
#else
    { NULL },
#endif
#if (INTDM0_ENABLE_ROM > INT_HNDL_NOT_REGIST)           /* 50: ICUM_DMAC channel 0 interrupt ICUM_DMAC */
    { ICDM0 },
#else
    { NULL },
#endif
#if (INTDM1_ENABLE_ROM > INT_HNDL_NOT_REGIST)           /* 51: ICUM_DMAC channel 1 interrupt ICUM_DMAC */
    { ICDM1 },
#else
    { NULL },
#endif
#if (INTDM2_ENABLE_ROM > INT_HNDL_NOT_REGIST)           /* 52: ICUM_DMAC channel 2 interrupt ICUM_DMAC */
    { ICDM2 },
#else
    { NULL },
#endif
#if (INTDM3_ENABLE_ROM > INT_HNDL_NOT_REGIST)           /* 53: ICUM_DMAC channel 3 interrupt ICUM_DMAC */
    { ICDM3 },
#else
    { NULL },
#endif
#if (INTDMAE_ENABLE_ROM > INT_HNDL_NOT_REGIST)          /* 54: ICUM_DMAC address error interrupt ICUM_DMAC */
    { ICDMAE },
#else
    { NULL },
#endif
#if (INTFSACSREQ0_ENABLE_ROM > INT_HNDL_NOT_REGIST)     /* 55: Flash sequencer access request for FPSYS0 */
    { INTFSACSREQ0 },
#else
    { NULL },
#endif
    { NULL },     /* 56: Not Used */
#if (INTICUAESE0DTFEND_ENABLE_ROM > INT_HNDL_NOT_REGIST)/* 57: Complition of AES enc or dec with DTF transfer */
    { ICICUAESE0DTFEND },
#else
    { NULL },
#endif
#if (INTICUAESE0DTFERR_ENABLE_ROM > INT_HNDL_NOT_REGIST)/* 58: SICAB slave error during DTF transfer */
    { ICICUAESE0DTFERR },
#else
    { NULL },
#endif
#if (INTICUAESE0CMPERR_ENABLE_ROM > INT_HNDL_NOT_REGIST)/* 59: ICUM_AESE0 output comparison error */
    { ICICUAESE0CMPERR },
#else
    { NULL },
#endif
#if (INTVDDHLDET_ENABLE_ROM > INT_HNDL_NOT_REGIST)      /* 60: Temperature Sensor */
    { ICVDDHLDET },
#else
    { NULL },
#endif
    { NULL },     /* 61: Not Used */
#if (INTOTULI_OTABE_ENABLE_ROM > INT_HNDL_NOT_REGIST)   /* 62: Temperature Sensor */
    { ICOTULI_OTABE },
#else
    { NULL },
#endif
#if (INTACEUCMPERR_ENABLE_ROM > INT_HNDL_NOT_REGIST)    /* 63: ACEU output comparison error ACEU0_AESE0/ACEU1_AESE0 */
    { ICACEUCMPERR }
#else
    { NULL }
#endif
};

#pragma ghs section rodata=".icum_driver_const_ram"
/**************************************************************************************/
/* Table for capturing EI interrupts during RAM operation                             */
/**************************************************************************************/
static ICUM_INT_EI_FPTBL_t s_eiit_fptbl_ram[ICUM_INT_EITBL_MAX] =
{
#if (INTPES_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 00: CPU(PE) interrupt request (single PE) ICUM_CMDREG */
    { ICPES_RAM },
#else
    { NULL },
#endif
#if (INTPE0_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 01: CPU(PE) interrupt request 0 ICUM_CMDREG */
    { ICPE0_RAM },
#else
    { NULL },
#endif
#if (INTPE1_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 02: CPU(PE) interrupt request 2 ICUM_CMDREG */
    { ICPE1_RAM },
#else
    { NULL },
#endif
#if (INTPE2_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 03: CPU(PE) interrupt request 3 ICUM_CMDREG */
    { ICPE2_RAM },
#else
    { NULL },
#endif
#if (INTICUAESE0RD_ENABLE_RAM > INT_HNDL_NOT_REGIST)    /* 04: Completion of AES encryption or decryption with ICUP */
    { ICICUAESE0RD_RAM },
#else
    { NULL },
#endif
#if (INTICUTRNGHT_ENABLE_RAM > INT_HNDL_NOT_REGIST)     /* 05: Completion of ICUM_TRNGC0 health test ICUM_TRNGC0 */
    { ICICUTRNGHT },
#else
    { NULL },
#endif
#if (INTICUTRNGG0_ENABLE_RAM > INT_HNDL_NOT_REGIST)     /* 06: ICUM_TRNGC0 output data ready ICUM_TRNGC0 */
    { ICICUTRNGG0 },
#else
    { NULL },
#endif
#if (INTICUOSTM0_ENABLE_RAM > INT_HNDL_NOT_REGIST)      /* 07: ICUM_OSTM0 overflow ICUM_OSTM0 */
    { INTICUOSTM0_RAM },
#else
    { NULL },
#endif
#if (INTICUOSTM1_ENABLE_RAM > INT_HNDL_NOT_REGIST)      /* 08: ICUM_OSTM1 overflow ICUM_OSTM1 */
    { ICICUOSTM1_RAM },
#else
    { NULL },
#endif
#if (INTICUWDTA0_ENABLE_RAM > INT_HNDL_NOT_REGIST)      /* 09: ICUM_WDTA0 counter 75% ICUM_WDTA0 */
    { ICICUWDTA0_RAM },
#else
    { NULL },
#endif
#if (INTEXTWDTB4_ENABLE_RAM > INT_HNDL_NOT_REGIST)      /* 10: WDTBA counter 75% WDTBA */
    { ICEXTWDTB4_RAM },
#else
    { NULL },
#endif
#if (INTPKCCA0_ENABLE_RAM > INT_HNDL_NOT_REGIST)        /* 11: Completion of ICUM_PKCCA0 operation ICUM_PKCC */
    { ICPKCCA0_RAM },
#else
    { NULL },
#endif
#if (INTFACI0ERR_ENABLE_RAM > INT_HNDL_NOT_REGIST)      /* 12: FPSYS0 Flash access error interrupt */
    { ICFACI0ERR_RAM },
#else
    { NULL },
#endif
#if (INTFACI0ENDNM_ENABLE_RAM > INT_HNDL_NOT_REGIST)    /* 13: FPSYS0 Flash sequencer processing end interrupt */
    { ICFACI0ENDNM_RAM },
#else
    { NULL },
#endif
#if (INTFICUIF0ILA_ENABLE_RAM > INT_HNDL_NOT_REGIST)    /* 14: FPSYS0 Flash illegal access error interrupt */
    { ICFICUIF0ILA_RAM },
#else
    { NULL },
#endif
#if (INTICUTRNGERR_ENABLE_RAM > INT_HNDL_NOT_REGIST)    /* 15: Error detection of ICUM_TRNGC0 health test ICUM_TRNGC0 */
    { ICICUTRNGERR },
#else
    { NULL },
#endif
    { NULL },     /* 16: Not Used */
    { NULL },     /* 17: Not Used */
    { NULL },     /* 18: Not Used */
#if (INTFACI2ERR_ENABLE_RAM > INT_HNDL_NOT_REGIST)      /* 19: FPSYS2 Flash access error interrupt */
    { ICFACI2ERR_RAM },
#else
    { NULL },
#endif
#if (INTFACI2ENDNM_ENABLE_RAM > INT_HNDL_NOT_REGIST)    /* 20: FPSYS2 Flash sequencer processing end interrupt */
    { ICFACI2ENDNM_RAM },
#else
    { NULL },
#endif
    { NULL },     /* 21: Not Used */
#if (INTDCUDEGRAD_ENABLE_RAM > INT_HNDL_NOT_REGIST)     /* 22: Degradation interrupt request */
    { ICDCUDEGRAD_RAM },
#else
    { NULL },
#endif
#if (INTAPBGERR_ENABLE_RAM > INT_HNDL_NOT_REGIST)       /* 23: Violation of access to ICUM_PBG  */
    { ICAPBGERR_RAM },
#else
    { NULL },
#endif
#if (INTICUSHAA0IREQ_ENABLE_RAM > INT_HNDL_NOT_REGIST)  /* 24: Input data request for ICUM_SHAA0 */
    { ICICUSHAA0IREQ_RAM },
#else
    { NULL },
#endif
#if (INTICUSHAA0OEND_ENABLE_RAM > INT_HNDL_NOT_REGIST)  /* 25: Completion or suspension of ICUM_SHAA0 data output */
    { ICICUSHAA0OEND_RAM },
#else
    { NULL },
#endif
#if (INTECM0CFGERR_ENABLE_RAM > INT_HNDL_NOT_REGIST)    /* 26: Code flash guard error */
    { ICECM0CFGERR_RAM },
#else
    { NULL },
#endif
#if (INTECM0DFGERR_ENABLE_RAM > INT_HNDL_NOT_REGIST)    /* 27: Data flash guard error */
    { ICECM0DFGERR_RAM },
#else
    { NULL },
#endif
#if (INTEXTDCU20CDRQ_ENABLE_RAM > INT_HNDL_NOT_REGIST)  /* 28: Authentication challenge data request for ICUP debug */
    { ICEXTDCU20CDRQ_RAM },
#else
    { NULL },
#endif
#if (INTEXTDCU20RDRQ_ENABLE_RAM > INT_HNDL_NOT_REGIST)  /* 29: Authentication res data comp req for ICUP/PE debug */
    { ICEXTDCU20RDRQ_RAM },
#else
    { NULL },
#endif
#if (INTEXTDCU20CDRQPE_ENABLE_RAM > INT_HNDL_NOT_REGIST)/* 30: Authentication challenge data request for PE debug */
    { ICEXTDCU20CDRQPE_RAM },
#else
    { NULL },
#endif
    { NULL },     /* 31: Not Used */
#if (INTRHSIFILA_ENABLE_RAM > INT_HNDL_NOT_REGIST)      /* 32: RHSIF0 int_hsif_sec interrupt */
    { ICRHSIFILA_RAM },
#else
    { NULL },
#endif
#if (INTPKCCA0SUSP_ENABLE_RAM > INT_HNDL_NOT_REGIST)    /* 33: PKCCA0 suspend completion */
    { ICPKCCA0SUSP_RAM },
#else
    { NULL },
#endif
#if (INTERRCFDA_ENABLE_RAM > INT_HNDL_NOT_REGIST)       /* 34: Code flash data access error detect */
    { ICERRCFDA_RAM },
#else
    { NULL },
#endif
#if (INTERRDFDA_ENABLE_RAM > INT_HNDL_NOT_REGIST)       /* 35: DF or local RAM data access error detect */
    { ICERRDFDA_RAM },
#else
    { NULL },
#endif
#if (INTUNINTDBG_ENABLE_RAM > INT_HNDL_NOT_REGIST)      /* 36: Unintended debug enable */
    { ICUNINTDBG_RAM },
#else
    { NULL },
#endif
#if (INTACEURD_ENABLE_RAM > INT_HNDL_NOT_REGIST)        /* 37: Completion of ACEU ercryption or decryption */
    { ICACEURD_RAM },
#else
    { NULL },
#endif
#if (INTACEUDTFEND_ENABLE_RAM > INT_HNDL_NOT_REGIST)    /* 38: Interrupt end signal for ACER direct data tgransfer[BCO] */
    { ICACEUDTFEND_RAM },
#else
    { NULL },
#endif
#if (INTICUCLMAS_ENABLE_RAM > INT_HNDL_NOT_REGIST)      /* 39: Clock monitor error for ICUMHB */
    { INTICUCLMA_RAM },
#else
    { NULL },
#endif
#if (INTPE3_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 40: CPU(PE) interrupt request 3 ICUM_CMDREG */
    { ICPE3_RAM },
#else
    { NULL },
#endif
#if (INTPE4_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 41: CPU(PE) interrupt request 4 ICUM_CMDREG */
    { ICPE4_RAM },
#else
    { NULL },
#endif
#if (INTPE5_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 42: CPU(PE) interrupt request 5 ICUM_CMDREG */
    { ICPE5_RAM },
#else
    { NULL },
#endif
#if (INTPE6_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 43: CPU(PE) interrupt request 6 ICUM_CMDREG */
    { ICPE6_RAM },
#else
    { NULL },
#endif
#if (INTPE7_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 44: CPU(PE) interrupt request 7 ICUM_CMDREG */
    { ICPE7_RAM },
#else
    { NULL },
#endif
    { NULL },     /* 45: Not Used */
    { NULL },     /* 46: Not Used */
#if (INTSHAOEND_ENABLE_RAM > INT_HNDL_NOT_REGIST)       /* 47: Completion or susupension of ICUM_SHA3A0 data output ICUM_SHA3A0 */
    { ICSHAOEND_RAM },
#else
    { NULL },
#endif
#if (INTRCAN_ENABLE_RAM > INT_HNDL_NOT_REGIST)          /* 48: CAN interrut RS-CANFD */
    { ICRCAN_RAM },
#else
    { NULL },
#endif
#if (INTSD_ENABLE_RAM > INT_HNDL_NOT_REGIST)            /* 49: sDMAC interrupt sDMAC */
    { ICSD_RAM },
#else
    { NULL },
#endif
#if (INTDM0_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 50: ICUM_DMAC channel 0 interrupt ICUM_DMAC */
    { ICDM0_RAM },
#else
    { NULL },
#endif
#if (INTDM1_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 51: ICUM_DMAC channel 1 interrupt ICUM_DMAC */
    { ICDM1_RAM },
#else
    { NULL },
#endif
#if (INTDM2_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 52: ICUM_DMAC channel 2 interrupt ICUM_DMAC */
    { ICDM2_RAM },
#else
    { NULL },
#endif
#if (INTDM3_ENABLE_RAM > INT_HNDL_NOT_REGIST)           /* 53: ICUM_DMAC channel 3 interrupt ICUM_DMAC */
    { ICDM3_RAM },
#else
    { NULL },
#endif
#if (INTDMAE_ENABLE_RAM > INT_HNDL_NOT_REGIST)          /* 54: ICUM_DMAC address error interrupt ICUM_DMAC */
    { ICDMAE_RAM },
#else
    { NULL },
#endif
#if (INTFSACSREQ0_ENABLE_RAM > INT_HNDL_NOT_REGIST)     /* 55: Flash sequencer access request for FPSYS0 */
    { INTFSACSREQ0_RAM },
#else
    { NULL },
#endif
    { NULL },     /* 56: Not Used */
#if (INTICUAESE0DTFEND_ENABLE_RAM > INT_HNDL_NOT_REGIST)/* 57: Complition of AES enc or dec with DTF transfer */
    { ICICUAESE0DTFEND_RAM },
#else
    { NULL },
#endif
#if (INTICUAESE0DTFERR_ENABLE_RAM > INT_HNDL_NOT_REGIST)/* 58: SICAB slave error during DTF transfer */
    { ICICUAESE0DTFERR_RAM },
#else
    { NULL },
#endif
#if (INTICUAESE0CMPERR_ENABLE_RAM > INT_HNDL_NOT_REGIST)/* 59: ICUM_AESE0 output comparison error */
    { ICICUAESE0CMPERR_RAM },
#else
    { NULL },
#endif
#if (INTVDDHLDET_ENABLE_RAM > INT_HNDL_NOT_REGIST)      /* 60: Temperature Sensor */
    { ICVDDHLDET_RAM },
#else
    { NULL },
#endif
    { NULL },     /* 61: Not Used */
#if (INTOTULI_OTABE_ENABLE_RAM > INT_HNDL_NOT_REGIST)   /* 62: Temperature Sensor */
    { ICOTULI_OTABE_RAM },
#else
    { NULL },
#endif
#if (INTACEUCMPERR_ENABLE_RAM > INT_HNDL_NOT_REGIST)    /* 63: ACEU output comparison error ACEU0_AESE0/ACEU1_AESE0 */
    { ICACEUCMPERR_RAM }
#else
    { NULL }
#endif
};
#pragma ghs section rodata=default

/**************************************************************************************/
/* API module group                                                                   */
/**************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0001]:[ICUMFW_UD_U2B_D_INT_0001]
* Function name: D_INT_Init
* Description  : Init processing.
* Arguments    : (IN)ICUM_INT_CB_REGIST_t *p_regist_info
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_INT_ERROR
***************************************************************************************/
uint32_t D_INT_Init(ICUM_INT_CB_REGIST_t *p_regist_info)
{
    register volatile uint16_t *pICUM_EIC;
    static uint16_t regData;
    uint32_t uiRet = ER_OK;
    uint32_t iLoop;
    uint32_t iSetBit;
    p_int_manip_t ramvect_addr;
    p_int_calbk_t callback_addr;

    /* 1. Address check of input parameters (p_regist_info) */
    if ((NULL != p_regist_info) && (NULL != p_regist_info->p_cbfunc))
    {
        /* 2. Alignment check of the RAM vector address */
        ramvect_addr.p_ptr = (void *)&p_ram_intvec;
        if ((ramvect_addr.val & ICUM_INT_RAMVECT_ADDRMUSK) == ramvect_addr.val)
        {
            /* 3. Status check of current INT driver */
            if (ICUM_INT_IDLE == s_intcdrv_status)
            {
                /* 4. Initialization of internal use area */
                s_ei_imr0_unmuskSet = ICUM_INT_IMR0_MUSK;
                s_ei_imr1_unmuskSet = ICUM_INT_IMR1_MUSK;

                /* 5. Register the callback function */
                s_cb_Reg_intc.p_cbfunc = p_regist_info->p_cbfunc;

                /* 6. Loop for the number of tables in the EI level */
                /*    module registration table (g_eit_fptbl_rom)   */
                for (iLoop = 0UL; ICUM_INT_EITBL_MAX > iLoop; iLoop++)
                {
                    /* 7. NULL check of callback module area in table */
                    /*    Priority Check Level 7 in table             */
                    if (NULL != s_eiit_fptbl_rom[iLoop].p_callbackfunc)
                    {
                        /* 8. Find the data set target register */
                        if (ICUM_INT_CHANNELMAX > iLoop)
                        {
                            pICUM_EIC = (volatile uint16_t *)&ICUM_INTC1.ICUM_ICPES.UINT16 + iLoop;

                            /* 9. 1 is shifted to the left by the loopcountvalue     */
                            /*    the result is inverted and set in the storage area */
                            iSetBit = (ICUM_INT_SET_BITSFT_1 << iLoop);
                            s_ei_imr0_unmuskSet &= ~iSetBit;
                        }
                        else
                        {
                            pICUM_EIC = (volatile uint16_t *)&ICUM_INTC2ICUM_ICRHSIFILA + 
                                        (iLoop - ICUM_INT_CHANNELMAX);

                            /* 10. 1 is shifted to the left by (loopcountvalue-32) */
                            /*     value and the result is inverted, set in the    */
                            /*     storage area                                    */
                            iSetBit = (ICUM_INT_SET_BITSFT_1 << (iLoop - ICUM_INT_CHANNELMAX));
                            s_ei_imr1_unmuskSet &= ~iSetBit;
                        }

                        /* 11. OR the following data in the data set target */
                        /*     register and write it                        */
                        regData = (ICUM_INT_MASK_ENA << ICUM_INT_MKBIT_SFT) | 
                                  (ICUM_INT_V_DIRECT << ICUM_INT_TBBIT_SFT) |
                                  ICUM_INT_PRI_LVL0;
                        *pICUM_EIC = regData;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }

                /* 12. update priority mask register to disable */
                /*     Intrrupt Priority 1 to 7                 */
                d_INT_SetPriorityMask();

                /* 13. Initialize RAM vector definition area */
                L_Toolbox_MemSet((uint8_t *)ramvect_addr.p_ptr, ICUM_INT_CLEAR_TABLE, (int16_t)ICUM_INT_RAMVCTER_MAX);

                /* 14. Loop of internal tables (s_feit_fptbl_ram) */
                for (iLoop = 0UL; ICUM_RAM_FEINT_MAXCNT > iLoop; iLoop++)
                {
                    /* 15. Callback registration area is not NULL */
                    if (NULL != s_feit_fptbl_ram[iLoop].p_callbackfunc)
                    {
                        /* 15-1. Generate RAM vector */
                        p_ram_intvec[s_feit_fptbl_ram[iLoop].w_offset + ICUM_INT_RAMVECT_POS_0] = 
                                                        s_feit_fptbl_ram[iLoop].b_jumpcode[ICUM_INT_TABLEOFFSET_0];
                        p_ram_intvec[s_feit_fptbl_ram[iLoop].w_offset + ICUM_INT_RAMVECT_POS_1] = 
                                                        s_feit_fptbl_ram[iLoop].b_jumpcode[ICUM_INT_TABLEOFFSET_1];

                        callback_addr.p_ptr = (p_callback_t)s_feit_fptbl_ram[iLoop].p_callbackfunc;
                        L_Toolbox_MemCpy((uint8_t *)&p_ram_intvec[s_feit_fptbl_ram[iLoop].w_offset + 
                                                                  ICUM_INT_RAMVECT_POS_2], 
                                         (const uint8_t*)&callback_addr.val, (int16_t)sizeof(callback_addr.val));
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }

                /* 16. State update of the INT driver */
                s_intcdrv_status = ICUM_INT_ROM_MODE;
            }
            else
            {
                /* 3-1. State mismatch and processing is terminated */
                uiRet = ER_D_INT_ERROR;
            }
        }
        else
        {
            /* 2-1. In the case of invalid alignment and processing is terminated */
            uiRet =  ER_PARAM;
        }
    }
    else
    {
        /* 1-1. In the case of error judgment (NULL) and processing is terminated */
        uiRet = ER_PARAM;
    }

    return (uiRet);
}

#pragma ghs section text=".icum_driver_ram"
#pragma ghs section rodata=".icum_driver_const_ram"
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0003]:[ICUMFW_UD_U2B_D_INT_0003]
* Function name: D_INT_EnableInterrupts
* Description  : Enable Interrupts.
* Arguments    : none
* Return Value : ER_OK
*                ER_D_INT_ERROR
***************************************************************************************/
uint32_t D_INT_EnableInterrupts(void)
{
    register volatile uint32_t *p_reg32;
    register volatile uint16_t *p_reg16;
    register uint32_t reg_ulpsw_u;
    uint32_t ulpsw_u;
    uint32_t uiRet = ER_OK;
    uint32_t uiComRet;
    uint32_t uiStatus;
    uint32_t address;
    p_int_manip_t p_icup_resv_a;

    /* 1. Status check of current INT driver */
    if (ICUM_INT_ROM_MODE == s_intcdrv_status)
    {
        /* 2. Get PE Status Info */
        uiComRet = D_COMM_GetICUStatusInfo(&uiStatus);
        /* 2-1. Get PE Status Info success */
        if (ER_OK == uiComRet)
        {
            /* 3. ICUM_SEGCONT register(SYSERR) UNMASK Setting */
            //p_reg16  = (volatile uint16_t *)&ICUM_SEG.CONT;
            //*p_reg16 = (uint16_t)ICUM_INT_SYSERR_UNMUSK;

            /* 4. ICUM_FEINTFMSK register UNMASK Setting */
            p_reg32  = (volatile uint32_t *)&ICUM_FEINTFMSK;
            *p_reg32 = ICUM_INT_FEINT_UNMUSK;

            /* 5. ICUM_IMR0 register UNMASK Setting */
            p_reg32  = (volatile uint32_t *)&ICUM_INTC1.ICUM_IMR0.UINT32;
            *p_reg32 = s_ei_imr0_unmuskSet;

            /* Dummy read & Synchronize pipeline to wait for the completion of */
            /* writing to the ICUM_IMR0 register                               */
            (void)(*p_reg32);
            SYNC_PIPELINE();

            /* 6. ICUM_IMR1 register UNMASK Setting */
            p_reg32  = (volatile uint32_t *)&ICUM_INTC2.ICUM_IMR1.UINT32;
            *p_reg32 = s_ei_imr1_unmuskSet;

            /* Dummy read & Synchronize pipeline to wait for the completion of */
            /* writing to the ICUM_IMR1 register                               */
            (void)(*p_reg32);
            SYNC_PIPELINE();

            /* 7. Setting the RBASE or EBASE register as the base register */
            ulpsw_u = STSR(PSW);

            /* 8. Get firmware partition address */
            (void)D_FLS_GetMemBoundary(D_FLS_LOWER_LIMIT_FIRMWARE, &address);
            /* 9-1. Set to firmware vector */
            p_icup_resv_a.val = address;
            /* 9-2. EBASE Register RINT BIT ON                */
            /* (Treat all user interrupt (EI) as offset 100H) */
            p_icup_resv_a.val |= ICUM_INT_EXTENDED_INTERRUPTS;
            /* 9-3. Set EBASE = reset vector of Side A */
            LDSR(p_icup_resv_a.val, EBASE);

            /* 9-4. Set PSW.EBV to 1(Used EBASE register) */
            ulpsw_u |= (uint32_t)ICUM_PSW_EBVBIT_U;

            /* 10. Setting PSW Register */
            reg_ulpsw_u = ulpsw_u;
            LDSR(reg_ulpsw_u, PSW);

            /* 11. Enable all interrupts */
            ENABLE_INTERRUPT();
        }
        else
        {
            /* 2-2. Get PE Status Info failure */
            uiRet = ER_D_INT_ERROR;
        }
    }
    else
    {
        /* 12. State mismatch and processing is terminated */
        uiRet = ER_D_INT_ERROR;
    }

    return (uiRet);
}

/***************************************************************************************
* Function ID  : (not supported)
* Function name: D_INT_DisableInterrupts
* Description  : Disable Interrupts.
* Arguments    : none
* Return Value : ER_OK
*                ER_D_INT_ERROR
***************************************************************************************/
uint32_t D_INT_DisableInterrupts(void)
{
    register volatile uint32_t *p_reg;
    register volatile uint16_t *p_reg16;
    uint32_t uiRet = ER_OK;

    /* 1. Status check of current INT driver */
    if (ICUM_INT_ROM_MODE == s_intcdrv_status)
    {
        /* 2. Disable all interrupts */
        DISABLE_INTERRUPT();

        /* 3. ICUM_IMR0 register MASK Setting */
        p_reg  = (volatile uint32_t *)&ICUM_INTC1.ICUM_IMR0.UINT32;
        *p_reg = ICUM_INT_IMR0_MUSK;

        p_reg  = (volatile uint32_t *)&ICUM_INTC2.ICUM_IMR1.UINT32;
        *p_reg = ICUM_INT_IMR1_MUSK;

        /* 4. ICUM_FEINTFMSK register MASK Setting */
        p_reg  = (volatile uint32_t *)&ICUM_FEINTFMSK;
        *p_reg = ICUM_INT_FEINT_MUSK;

        /* 5. ICUM_SEGCONT register(SYSERR) MASK Setting */
        p_reg16  = (volatile uint16_t *)&ICUM_SEGCONT;
        *p_reg16 = (uint16_t)ICUM_INT_SYSERR_MUSK;
    }
    else
    {
        /* 1-1. State mismatch and processing is terminated */
        uiRet = ER_D_INT_ERROR;
    }

    return (uiRet);
}

/***************************************************************************************
* Function ID  : (not supported)
* Function name: D_INT_GetRequestFlag
* Description  : Request Flag Get.
* Arguments    : (IN)uint32_tt channel
*                (OUT)uint32_t *p_Result
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_INT_ERROR
***************************************************************************************/
uint32_t D_INT_GetRequestFlag(uint32_t channel, uint32_t *p_Result)
{
    register volatile uint16_t *pICUM_EIC;
    uint32_t uiRet = ER_OK;
    uint32_t uiEicSave;

    /* 1. Status check of current INT driver */
    if (ICUM_INT_IDLE != s_intcdrv_status)
    {
        /* 2. Check range of input parameters (channel) */
        if (ICUM_INT_INTERRUPTMAX > channel)
        {
            /* 3. The base register (ICUM_INTC1.ICUM_ICPES)+channel is executed */
            /*    to determine the register for data acquisition.               */
            if (ICUM_INT_CHANNELMAX > channel)
            {
                pICUM_EIC = (volatile uint16_t *)&ICUM_INTC1.ICUM_ICPES.UINT16 + channel;
            }
            else
            {
                pICUM_EIC = (volatile uint16_t *)&ICUM_INTC2ICUM_ICPKCCA0SUSP + (channel - ICUM_INT_CHANNELMAX);
            }
            uiEicSave = *pICUM_EIC;

            /* 4. Read data from the determined register and extract only */
            /*    the RF bit                                              */
            uiEicSave = (uiEicSave & ICUM_INT_RFBIT_SETINTRRUP);
            *p_Result = (uiEicSave >> ICUM_INT_RFBIT_RIGHT_SIFT12);
        }
        else
        {
             /* 2-1. Out of range and processing is terminated */
            uiRet = ER_PARAM;
        }
    }
    else
    {
        /* 1-1. State mismatch and processing is terminated */
        uiRet = ER_D_INT_ERROR;
    }

    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0006]:[ICUMFW_UD_U2B_D_INT_0006]
* Function name: D_INT_ChgExecMode
* Description  : Mode Change(ROM<->RAM).
* Arguments    : (IN)uint32_t chgmode
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_INT_ERROR
***************************************************************************************/
uint32_t D_INT_ChgExecMode(uint32_t chgmode)
{
    uint32_t uiRet = ER_OK;

    /* 1. Status check of current INT driver */
    if (ICUM_INT_IDLE != s_intcdrv_status)
    {
        /* 2-1. Current ROM mode */
        if (ICUM_INT_ROM_MODE == s_intcdrv_status)
        {
            /*  2-1-1. input parameter (chgmode) is ICUM_INT_RAM_MODE */
            if (ICUM_INT_RAMMODE == chgmode)
            {
                uiRet = d_INT_ChgRomToRam();
            }
            else
            {
                /* do noting */
            }
        }
        /* 2-2. Current RAM mode */
        else
        {
            /*  2-2-1. input parameter (chgmode) is ICUM_INT_ROM_MODE */
            if (ICUM_INT_ROMMODE == chgmode)
            {
                uiRet = d_INT_ChgRamToRom();
            }
            else
            {
                /* do noting */
            }
        }
    }
    else
    {
        /* 1-1. State mismatch and processing is terminated */
        uiRet = ER_D_INT_ERROR;
    }

    return (uiRet);
}
#pragma ghs section text=default
#pragma ghs section rodata=default

/**************************************************************************************/
/* Local Function                                                                     */
/**************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0002]:[ICUMFW_UD_U2B_D_INT_0002]
* Function name: d_INT_SetPriorityMask
* Description  : 
* Arguments    : 
* Return Value : none
***************************************************************************************/
static void d_INT_SetPriorityMask(void)
{
    register uint32_t reg_musk_pmr;
    uint32_t musk_pmr;
    uint32_t iLoop;

    /* 1. Interrupt priority 1 to 7 Loop */
    for (iLoop = 1UL; (uint32_t)ICUM_INT_PRI_LVLCNT > iLoop; iLoop++)
    {
        /* 2. Read PMR register  */
        musk_pmr = STSR(PMR);

        /* 3. disable priority masking */
        musk_pmr |= (ICUM_INT_SET_BITSFT_1 << iLoop);

        /* 4. Set in the PMR register */
        reg_musk_pmr = musk_pmr;
        LDSR(reg_musk_pmr, PMR);
    }
    return;
}

#pragma ghs section text=".icum_driver_ram"
#pragma ghs section rodata=".icum_driver_const_ram"
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0007]:[ICUMFW_UD_U2B_D_INT_0007]
* Function name: d_INT_ChgRomToRam
* Description  : Mode Change(ROM->RAM)
* Arguments    : none
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
static uint32_t d_INT_ChgRomToRam(void)
{
    register volatile uint32_t *pICUM_IMR;
    register volatile uint32_t ulpsw_u;
    p_int_manip_t ramvect_addr;

    ramvect_addr.p_ptr = (void *)&p_ram_intvec;

    pICUM_IMR = (volatile uint32_t *)&ICUM_INTC1.ICUM_IMR0.UINT32;

    /* 1. Disable all interrupts */
    DISABLE_INTERRUPT();

    /* 2. Random number generation completion interrupt disable */
    *pICUM_IMR = (*pICUM_IMR | ICUM_INT_IMR0_ICUTRNGC0);

    /* Dummy read & Synchronize pipeline to wait for the completion of */
    /* writing to the ICUM_IMR0 register                               */
    (void)(*pICUM_IMR);
    SYNC_PIPELINE();

    /* 3. EBASE register RINT bit ON */
    ramvect_addr.val |= ICUM_INT_EXTENDED_INTERRUPTS;
    LDSR(ramvect_addr.val, EBASE);

    /* 4. Setting the EBASE register as the base register */
    /*    EBV bit to 1                                    */
    ulpsw_u = STSR(PSW);

    ulpsw_u |= ICUM_PSW_EBVBIT_U;
    LDSR(ulpsw_u, PSW);

    /* 5. Update the state of the INT driver */
    s_intcdrv_status = ICUM_INT_RAM_MODE;

    /* 6. Enable all interrupts */
    ENABLE_INTERRUPT();

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0008]:[ICUMFW_UD_U2B_D_INT_0008]
* Function name: d_INT_ChgRamToRom
* Description  : Mode Change(RAM->ROM)
* Arguments    : none
* Return Value : ER_OK
***************************************************************************************/
static uint32_t d_INT_ChgRamToRom(void)
{
    register volatile uint32_t *pICUM_IMR;
    register uint32_t ulpsw_u;
    uint32_t uiComRet;
    uint32_t uiStatus;
    uint32_t address;
    p_int_manip_t p_icup_resv_a;
    uint32_t uiRet = ER_OK;

    /* 1. Disable all interrupts */
    DISABLE_INTERRUPT();

    /* 2. Get PE Status Info */
    uiComRet = D_COMM_GetICUStatusInfo(&uiStatus);
    /* 2-1. Get PE Status Info success */
    if (ER_OK == uiComRet)
    {
        /* 3. Setting the RBASE or EBASE register as the base register */
        ulpsw_u = STSR(PSW);

        /* 4. Get firmware partition address */
        (void)D_FLS_GetMemBoundary(D_FLS_LOWER_LIMIT_FIRMWARE, &address);
        /* 5-1. Set to firmware vector */
        p_icup_resv_a.val = address;

        /* 5-2. EBASE Register RINT BIT ON                */
        /* (Treat all user interrupt (EI) as offset 100H) */
        p_icup_resv_a.val |= ICUM_INT_EXTENDED_INTERRUPTS;
        /* 5-3. Set EBASE = reset vector of Side A */
        LDSR(p_icup_resv_a.val, EBASE);
        /* 5-4. Set PSW.EBV to 1(Used EBASE register) */
        ulpsw_u |= (uint32_t)ICUM_PSW_EBVBIT_U;

        /* 6. Setting PSW Register */
        LDSR(ulpsw_u, PSW);

        pICUM_IMR = (volatile uint32_t *)&ICUM_INTC1.ICUM_IMR0.UINT32;

        /* 7. Random number generation completion interrupt enable */
        *pICUM_IMR = (*pICUM_IMR & ~ICUM_INT_IMR0_ICUTRNGC0);

        /* 8. Update the state of the INT driver */
        s_intcdrv_status = ICUM_INT_ROM_MODE;
    }
    else
    {
        /* 2-2. Get PE Status Info failure */
        uiRet = ER_D_INT_ERROR;
    }

    /* 9. Enable all interrupts */
    ENABLE_INTERRUPT();

    return (uiRet);
}
#pragma ghs section text=default
#pragma ghs section rodata=default

/**************************************************************************************/
/* Interrupt Module group                                                             */
/**************************************************************************************/

/**************************************************************************************/
/* ROM SECTION group                                                                  */
/**************************************************************************************/

#pragma ghs section text=".icum_handler_ram"
#pragma ghs section rodata=".icum_handler_const_ram"
/**************************************************************************************/
/* Note : SYSERR handler is conceptually those allocate to ROM section.               */
/*        However, since SYSERR handler is activated even in RAM mode, it is assign   */
/*        to RAM section.                                                             */
/**************************************************************************************/
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0009]:[ICUMFW_UD_U2B_D_INT_0009]
* Function name: SYSERR
* Description  : System error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (SYSERR_ENABLE_ROM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER_FE(SYSERR)
{
    uint32_t uiFact = ICUM_INT_SYSERR_FACT;

    /* 1. Acquire the error type of the generated system error */
    /*    from ICUM_SEGFLAG and branch processing              */

    /* 1-1. Local RAM read-modify-write error flag check */
    if (((uint32_t)ICUM_SEGFLAG & (uint32_t)ICUM_INT_SEGFLAG_RMWF) == (uint32_t)ICUM_INT_SEGFLAG_RMWF)
    {
        /* 1-1-1. SEG_FLAGRMWF bit clear */
        ICUM_SEGFLAG &= ~ICUM_INT_SEGFLAG_RMWF;
        uiFact = ICUM_INT_SYSERR_LRAM_RMW;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-1-1. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(uiFact);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 1-2. Local RAM area error flag check */
    if (((uint32_t)ICUM_SEGFLAG & (uint32_t)ICUM_INT_SEGFLAG_RAMF) == (uint32_t)ICUM_INT_SEGFLAG_RAMF)
    {
        /* 1-2-1. SEG_FLAGRAMF bit clear */
        ICUM_SEGFLAG &= ~ICUM_INT_SEGFLAG_RAMF;
        uiFact = ICUM_INT_SYSERR_LRAM_AREA;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-2-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(uiFact);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 1-3. BUS error flag check */
    if (((uint32_t)ICUM_SEGFLAG & (uint32_t)ICUM_INT_SEGFLAG_EXTF) == (uint32_t)ICUM_INT_SEGFLAG_EXTF)
    {
        /* 1-3-1. SEG_FLAGRAMF bit clear */
        ICUM_SEGFLAG &= ~ICUM_INT_SEGFLAG_EXTF;
        uiFact = ICUM_INT_SYSERR_BUS_ACC;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-3-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(uiFact);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 1-4. Code flash error flag */
    if (((uint32_t)ICUM_SEGFLAG & (uint32_t)ICUM_INT_SEGFLAG_FCHF) == (uint32_t)ICUM_INT_SEGFLAG_FCHF)
    {
        /* 1-4-1. SEG_FLAGFCHF bit clear */
        ICUM_SEGFLAG &= ~ICUM_INT_SEGFLAG_FCHF;
        uiFact = ICUM_INT_SYSERR_CODE_FLASH;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-4-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(uiFact);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 2. When none of the above error types are applicable */
    if (0UL == uiFact)
    {
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 2-1. Callback module exec */
            uiFact = ICUM_INT_SYSERR_INST_FETCH;
            (*s_cb_Reg_intc.p_cbfunc)(uiFact);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }
}
#endif
#pragma ghs section text=default
#pragma ghs section rodata=default

#pragma ghs section text=".icum_handler.text"
#pragma ghs section rodata=".icum_handler.rodata"
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0010]:[ICUMFW_UD_U2B_D_INT_0010]
* Function name: FEINT
* Description  : FE level maskable interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (FEINT_ENABLE_ROM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER_FE(FEINT)
{
    /* 1. Acquire the error type of the generated system error */
    /*    from ICUM_FEINTF and branch processing               */

    /* 1-1. INTSYSSTPRQ interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_STPRQFEIFC) == ICUM_INT_STPRQFEIFC)
    {
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-1-1. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_SYSTEM_STOP);
        }
        else
        {
            /* Do nothing */
        }
        /* 1-1-2. INTSYSSTPRQ bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_STPRQFEIFC;
    }
    else
    {
        /* Do nothing */
    }

    /* 1-2. INTECCSIC interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_ECCSICFEIFC) == ICUM_INT_ECCSICFEIFC)
    {
        /* 1-2-1. INTICUECCSICAB bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_ECCSICFEIFC;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-2-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_ECCERR_SIC);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 1-3. INTECCSICAB interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_ECCSICABFEIF) == ICUM_INT_ECCSICABFEIF)
    {
        /* 1-3-1. INTICUECCSICAB bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_ECCSICABFEIF;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-3-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_ECCERR_SICAB);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 1-4. INTECCCRAM interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_ECCCRAMFEIF) == ICUM_INT_ECCCRAMFEIF)
    {
        /* 1-4-1. INTICUECCCRAM bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_ECCCRAMFEIF;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-4-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_ECCERR_CRAM);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 1-5. INTECCLRAM interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_ECCLRAMFEIF) == ICUM_INT_ECCLRAMFEIF)
    {
        /* 1-5-1. INTICUECCLRAM bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_ECCLRAMFEIF;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-5-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_ECCERR_LRAM);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 1-6. INTECCPKCCRAM interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_ECCPKCCRAMFEIF) == ICUM_INT_ECCPKCCRAMFEIF)
    {
        /* 1-6-1. INTICUECCPKCCRAM bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_ECCPKCCRAMFEIF;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-6-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_ECCERR_PKRAM);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 1-7. INTECCAESAXI interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_ECCAESAXIFEIF) == ICUM_INT_ECCAESAXIFEIF)
    {
        /* 1-7-1. INTICUECCAESAXI bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_ECCAESAXIFEIF;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-7-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_ECCERR_AESAXI);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0011]:[ICUMFW_UD_U2B_D_INT_0011]
* Function name: FETRAPEX
* Description  : FE level trap interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (FETRAPEX_ENABLE_ROM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER_FE(FETRAPEX)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1.Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FETRAPEX_OCC);
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0012]:[ICUMFW_UD_U2B_D_INT_0012]
* Function name: EITRAP0
* Description  : EI level trap 0 interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (EITRAP0_ENABLE_ROM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER(EITRAP0)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_EITRAP0_OCC);
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0013]:[ICUMFW_UD_U2B_D_INT_0013]
* Function name: EITRAP1
* Description  : EI level trap 1 interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (EITRAP1_ENABLE_ROM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER(EITRAP1)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_EITRAP1_OCC);
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0014]:[ICUMFW_UD_U2B_D_INT_0014]
* Function name: RIEX
* Description  : Reserved instruction exception interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (RIEX_ENABLE_ROM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER_FE(RIEX)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_RIEX_OCC);
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : (not supported)
* Function name: UCPOP
* Description  : Coprocessor unusable exception interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (UCPOP_ENABLE_ROM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER_FE(UCPOP)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_UCPOP_OCC);
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0016]:[ICUMFW_UD_U2B_D_INT_0016]
* Function name: PIEX
* Description  : Privileged instruction exception interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (PIEX_ENABLE_ROM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER_FE(PIEX)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_PIEX_OCC);
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0017]:[ICUMFW_UD_U2B_D_INT_0017]
* Function name: MAEX
* Description  : Misalign exception interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (MAEX_ENABLE_ROM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER_FE(MAEX)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_MAEX_OCC);
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0018]:[ICUMFW_UD_U2B_D_INT_0018]
* Function name: EINTPRIO_0
* Description  : EI Level interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (EINTPRIO_0_ENABLE_ROM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER(EINTPRIO_0)
{
    register uint32_t eiic;
    uint32_t          tbloff;

    /* 1. Obtain interrupt cause from register (EIIC)    */
    /*    or acquiring exception cause at EI level       */
    /*   (The values of 1000H to 1030H will be returned) */
    eiic = STSR(EIIC);

    /* EI interrupt factor Effective range check */
    if ((ICUM_INT_EI_INTOCC_LOW <= eiic) && (ICUM_INT_EI_INTOCC_HIGH >= eiic))
    {
        tbloff = (eiic - ICUM_INT_EILVL_BASE);
        if (ICUM_INT_EITBL_MAX > tbloff)
        {
            /* 2. Check whether the callback is registered in p_callbackfunc */
            /*    from table with the acquired interrupt cause as an offset  */
            if (NULL != s_eiit_fptbl_rom[tbloff].p_callbackfunc)
            {
                /* 3. Call registered callback module */
                s_eiit_fptbl_rom[tbloff].p_callbackfunc();

                /* 4. Synchronize Pipeline Macro */
                SYNC_PIPELINE();
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }
}
#endif
#pragma ghs section text=default
#pragma ghs section rodata=default

/**************************************************************************************/
/* RAM SECTION group                                                                  */
/**************************************************************************************/
#pragma ghs section text=".icum_handler_ram"
#pragma ghs section rodata = ".icum_handler_const_ram"
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_XXXX]:[ICUMFW_UD_U2B_D_INT_XXXX]
* Function name: FEINT_RAM
* Description  : FE level maskable interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (FEINT_ENABLE_RAM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER_FE(FEINT_RAM)
{
    /* 1. Acquire the error type of the generated system error */
    /*    from ICUM_FEINTF and branch processing               */

    /* 1-1. INTSYSSTPRQ interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_STPRQFEIFC) == ICUM_INT_STPRQFEIFC)
    {
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-1-1. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_SYSTEM_STOP);
        }
        else
        {
            /* Do nothing */
        }
        /* 1-1-2. INTSYSSTPRQ bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_STPRQFEIFC;
    }
    else
    {
        /* Do nothing */
    }

    /* 1-2. INTECCSIC interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_ECCSICFEIFC) == ICUM_INT_ECCSICFEIFC)
    {
        /* 1-2-1. INTICUECCSICAB bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_ECCSICFEIFC;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-2-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_ECCERR_SIC);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 1-3. INTECCSICAB interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_ECCSICABFEIF) == ICUM_INT_ECCSICABFEIF)
    {
        /* 1-3-1. INTICUECCSICAB bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_ECCSICABFEIF;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-3-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_ECCERR_SICAB);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 1-4. INTECCCRAM interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_ECCCRAMFEIF) == ICUM_INT_ECCCRAMFEIF)
    {
        /* 1-4-1. INTICUECCCRAM bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_ECCCRAMFEIF;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-4-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_ECCERR_CRAM);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 1-5. INTECCLRAM interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_ECCLRAMFEIF) == ICUM_INT_ECCLRAMFEIF)
    {
        /* 1-5-1. INTICUECCLRAM bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_ECCLRAMFEIF;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-5-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_ECCERR_LRAM);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 1-6. INTECCPKCCRAM interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_ECCPKCCRAMFEIF) == ICUM_INT_ECCPKCCRAMFEIF)
    {
        /* 1-6-1. INTICUECCPKCCRAM bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_ECCPKCCRAMFEIF;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-6-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_ECCERR_PKRAM);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 1-7. INTECCAESAXI interrupt occurrence check */
    if ((ICUM_FENMIICUM_FEINTF & ICUM_INT_ECCAESAXIFEIF) == ICUM_INT_ECCAESAXIFEIF)
    {
        /* 1-7-1. INTICUECCAESAXI bit set to clear */
        ICUM_FENMIICUM_FEINTFC = ICUM_INT_ECCAESAXIFEIF;
        if (NULL != s_cb_Reg_intc.p_cbfunc)
        {
            /* 1-7-2. Callback module exec */
            (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FEINT_ECCERR_AESAXI);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }

}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_XXXX]:[ICUMFW_UD_U2B_D_INT_XXXX]
* Function name: FETRAPEX_RAM
* Description  : FE level trap interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (FETRAPEX_ENABLE_RAM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER_FE(FETRAPEX_RAM)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_FETRAPEX_OCC);
    }
     else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_XXXX]:[ICUMFW_UD_U2B_D_INT_XXXX]
* Function name: EITRAP0_RAM
* Description  : EI level trap 0 interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (EITRAP0_ENABLE_RAM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER(EITRAP0_RAM)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_EITRAP0_OCC);
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_XXXX]:[ICUMFW_UD_U2B_D_INT_XXXX]
* Function name: EITRAP1_RAM
* Description  : EI level trap 1 interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (EITRAP1_ENABLE_RAM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER(EITRAP1_RAM)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_EITRAP1_OCC);
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_XXXX]:[ICUMFW_UD_U2B_D_INT_XXXX]
* Function name: RIEX_RAM
* Description  : Reserved instruction exception interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (RIEX_ENABLE_RAM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER_FE(RIEX_RAM)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_RIEX_OCC);
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : (not supported)
* Function name: UCPOP_RAM
* Description  : Coprocessor unusable exception interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (UCPOP_ENABLE_RAM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER_FE(UCPOP_RAM)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_UCPOP_OCC);
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_XXXX]:[ICUMFW_UD_U2B_D_INT_XXXX]
* Function name: PIEX_RAM
* Description  : Privileged instruction exception interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (PIEX_ENABLE_RAM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER_FE(PIEX_RAM)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_PIEX_OCC);
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_XXXX]:[ICUMFW_UD_U2B_D_INT_XXXX]
* Function name: MAEX_RAM
* Description  : Misalign exception interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (MAEX_ENABLE_RAM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER_FE(MAEX_RAM)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_MAEX_OCC);
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0019]:[ICUMFW_UD_U2B_D_INT_0019]
* Function name: EINTPRIO_0_RAM
* Description  : EI Level interrupt error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
#if (EINTPRIO_0_ENABLE_RAM > INT_HNDL_NOT_REGIST)
IRQ_HANDLER(EINTPRIO_0_RAM)
{
    register uint32_t   eiic;
    uint32_t            tbloff;

    /* 1. Obtain interrupt cause from register (EIIC)     */
    /*    for acquiring exception cause at EI level       */
    /*    (The values of 1000H to 1030H will be returned) */
    eiic = STSR(EIIC);

    /* EI interrupt factor Effective range check */
    if ((ICUM_INT_EI_INTOCC_LOW <= eiic) && (ICUM_INT_EI_INTOCC_HIGH >= eiic))
    {
        tbloff = (eiic - ICUM_INT_EILVL_BASE);

        /* Check the calculated table offset range */
        if (ICUM_INT_EITBL_MAX > tbloff)
        {
            /* 2. Check whether the callback is registered in p_callbackfunc */
            /*           from table with the acquired interrupt cause as an offset  */
            if (NULL != s_eiit_fptbl_ram[tbloff].p_callbackfunc)
            {
                /* 3. Call registered callback module */
                s_eiit_fptbl_ram[tbloff].p_callbackfunc();

                /* 4. Synchronize Pipeline Macro */
                SYNC_PIPELINE();
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }
}
#endif

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0020]:[ICUMFW_UD_U2B_D_INT_0020]
* Function name: infinite_loop
* Description  : RAM mode infinite loop
* Arguments    : none
* Return Value : none
***************************************************************************************/
IRQ_HANDLER_FE(infinite_loop)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_ILLEGAL_INTERRRUPT);
    }
    else
    {
        /* Do nothing */
    }
}

#pragma ghs section text=default

#pragma ghs section text=".icum_handler.text"
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0021]:[ICUMFW_UD_U2B_D_INT_0021]
* Function name: ICVDDHLDET
* Description  : EI Level interrupt(ICVDDHLDET) error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
void ICVDDHLDET(void)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_EI_VOLTAGE_ERR);
    }
    else
    {
        /* Do nothing */
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0023]:[ICUMFW_UD_U2B_D_INT_0023]
* Function name: ICOTULI_OTABE
* Description  : EI Level interrupt(ICOTULI_OTABE) error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
void ICOTULI_OTABE(void)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_EI_TEMPERATURE_ERR);
    }
    else
    {
        /* Do nothing */
    }
}
#pragma ghs section text=default

#pragma ghs section text=".icum_handler_ram"
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0022]:[ICUMFW_UD_U2B_D_INT_0022]
* Function name: ICVDDHLDET_RAM
* Description  : EI Level interrupt(ICVDDHLDET) error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
void ICVDDHLDET_RAM(void)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_EI_VOLTAGE_ERR);
    }
    else
    {
        /* Do nothing */
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0024]:[ICUMFW_UD_U2B_D_INT_0024]
* Function name: ICOTULI_OTABE_RAM
* Description  : EI Level interrupt(ICOTULI_OTABE) error occurred.
* Arguments    : none
* Return Value : none
***************************************************************************************/
void ICOTULI_OTABE_RAM(void)
{
    if (NULL != s_cb_Reg_intc.p_cbfunc)
    {
        /* 1. Callback module exec */
        (*s_cb_Reg_intc.p_cbfunc)(ICUM_INT_EI_TEMPERATURE_ERR);
    }
    else
    {
        /* Do nothing */
    }
}

#pragma ghs section text=default

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_INT_0025]:[ICUMFW_UD_U2B_D_INT_0025]
* Function name: ICDCUDEGRAD
* Description  : EI Level interrupt(ICDCUDEGRAD)
* Arguments    : none
* Return Value : none
***************************************************************************************/
void ICDCUDEGRAD(void)
{
    (void)ICUM_DEGEMU;
}
