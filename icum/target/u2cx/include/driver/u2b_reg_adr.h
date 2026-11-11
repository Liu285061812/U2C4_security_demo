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
/* #include "u2b_reg_adr.h" */

#define ICUM_REG_FHVE_BASE              (0xFF987000UL)                                              /* <ICUM_FHVE_base>           0xFF987000  */
//#define ICUM_FHVE3CNTFP1                (*(volatile uint32_t *)(ICUM_REG_FHVE_BASE + 0x10U))        /* ICUM_FHVE3CNTFP1     */
//#define ICUM_FHVE15CNTFP1               (*(volatile uint32_t *)(ICUM_REG_FHVE_BASE + 0x14U))        /* ICUM_FHVE15CNTFP1    */

#define REG_PORT_BASE                   (0xFFD90000UL)                                              /* <ICUM_PROT0_base>          0xFFFEE090  */
//#define PORT0P6                         (*(volatile uint16_t *)(REG_PORT_BASE + 0x180U))            /* PORT0P6              */
//#define PORTn_15_MSK                    (0x8000U)                                                   /* PORT register bit15 mask value         */
//#define PORTn_15_RSF                    (15U)                                                       /* PORT register bit15 right shift value  */
//#define PORT0P6_15                      ((PORT0P6 & PORTn_15_MSK) >> PORTn_15_RSF)                  /* PORT0P6_15           */

//#define SYSCTRLSTBC0PSC                 (*(volatile uint32_t *)0xFF988C00UL)                        /* STBC0PSC             */

#define ICUM_REG_PROT0_BASE             (0xFFFEE090UL)                                              /* <ICUM_PROT0_base>          0xFFFEE090  */
//#define ICU_GEN0ICUM_PROT0PCMD          (*(volatile uint32_t *)ICUM_REG_PROT0_BASE)                 /* ICUM_PROT0PCMD       */
//#define ICU_GEN0ICUM_PROT0PS            (*(volatile uint32_t *)(ICUM_REG_PROT0_BASE + 0x04U))       /* ICUM_PROT0PS         */

#define ICUM_REG_TRNGC0_BASE            (0xFFFEE0A0UL)                                              /* <ICUM_TRNGC0_base>         0xFFFEE0A0  */
//#define ICUM_TRNGC0SR                   (*(volatile uint32_t *)ICUM_REG_TRNGC0_BASE)                /* ICUM_TRNGC0SR        */
//#define ICUM_TRNGC0SCLR                 (*(volatile uint32_t *)(ICUM_REG_TRNGC0_BASE + 0x04U))      /* ICUM_TRNGC0SCLR      */
//#define ICUM_TRNGC0CR                   (*(volatile uint32_t *)(ICUM_REG_TRNGC0_BASE + 0x08U))      /* ICUM_TRNGC0CR        */
//#define ICUM_TRNGC0RNRR                 (*(volatile uint32_t *)(ICUM_REG_TRNGC0_BASE + 0x0CU))      /* ICUM_TRNGC0RNRR      */
//#define ICUM_TRNGC0MCR                  (*(volatile uint32_t *)(ICUM_REG_TRNGC0_BASE + 0x18U))      /* ICUM_TRNGC0MCR      */

#define ICUM_REG_CTLREG_BASE            (0xFFFEE200UL)                                              /* <ICUM_CTLREG_base>         0xFFFEE200  */
#define ICUM_REG_CTLREG2_BASE           (0xFF1FC880UL)                                              /* <ICUM_CTLREG2_base>        0xFF1FC880  */
#define ICUM_REG_CTLREG_SICREMAP        (*(volatile uint32_t *)ICUM_REG_CTLREG_BASE)  
#define ICUM_CTLREGICUM_SICREMAP2M0     (*(volatile uint32_t *)ICUM_REG_CTLREG2_BASE)               /* ICUM_SICREMAP2M0     */
#define ICUM_CTLREGICUM_SICREMAP2M1     (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x04U))     /* ICUM_SICREMAP2M1     */
#define ICUM_CTLREGICUM_SICREMAP2M2     (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x08U))     /* ICUM_SICREMAP2M2     */
#define ICUM_CTLREGICUM_SICREMAP2M3     (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x0CU))     /* ICUM_SICREMAP2M3     */
#define ICUM_CTLREGICUM_SICREMAP2M4     (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x10U))     /* ICUM_SICREMAP2M4     */
#define ICUM_CTLREGICUM_SICREMAP2M5     (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x14U))     /* ICUM_SICREMAP2M5     */
#define ICUM_CTLREGICUM_SICREMAP2M6     (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x18U))     /* ICUM_SICREMAP2M6     */
#define ICUM_CTLREGICUM_SICREMAP2M7     (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x1CU))     /* ICUM_SICREMAP2M7     */
#define ICUM_CTLREGICUM_SICREMAP2M8     (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x20U))     /* ICUM_SICREMAP2M8     */
#define ICUM_CTLREGICUM_SICREMAP2M9     (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x24U))     /* ICUM_SICREMAP2M9     */
#define ICUM_CTLREGICUM_SICREMAP2M10    (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x28U))     /* ICUM_SICREMAP2M10    */
#define ICUM_CTLREGICUM_SICREMAP2M11    (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x2CU))     /* ICUM_SICREMAP2M11    */
#define ICUM_CTLREGICUM_SICREMAP2M12    (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x30U))     /* ICUM_SICREMAP2M12    */
#define ICUM_CTLREGICUM_SICREMAP2M13    (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x34U))     /* ICUM_SICREMAP2M13    */
#define ICUM_CTLREGICUM_SICREMAP2M14    (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x38U))     /* ICUM_SICREMAP2M14    */
#define ICUM_CTLREGICUM_SICREMAP2M15    (*(volatile uint32_t *)(ICUM_REG_CTLREG2_BASE + 0x3CU))     /* ICUM_SICREMAP2M15    */

//#define ICUM_FENMIICUM_FEINTF           (*(volatile uint32_t *)0xFFFEECD0UL)                        /* ICUM_FEINTF          */
//#define ICUM_FENMIICUM_FEINTFMSK        (*(volatile uint32_t *)0xFFFEECD4UL)                        /* ICUM_FEINTFMSK       */
//#define ICUM_FENMIICUM_FEINTFC          (*(volatile uint32_t *)0xFFFEECD8UL)                        /* ICUM_FEINTFC         */
//#define ICUM_INTC2ICUM_ICRHSIFILA       (*(volatile uint32_t *)0xFFFEF040UL)                        /* ICUM_ICRHSIFILA      */

#define REG_SPIDCTL_BASE                (0xFF0A8000UL)                                              /* <SPIDCTL_base>             0xFF0A8000  */
//#define SPIDCTLBM03SPIDMSK              (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x10CU))         /* BM03SPIDMSK          */
//#define SPIDCTLBM19SPIDMSK              (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x14CU))         /* BM19SPIDMSK          */
//#define SPIDCTLBM22SPIDMSK              (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x158U))         /* BM22SPIDMSK          */
//#define SPIDCTLBM23SPIDMSK              (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x15CU))         /* BM23SPIDMSK          */
//#define SPIDCTLBM24SPIDMSK              (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x160U))         /* BM24SPIDMSK          */
//#define SPIDCTLBM25SPIDMSK              (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x164U))         /* BM25SPIDMSK          */
//#define SPIDCTLBM26SPIDMSK              (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x168U))         /* BM26SPIDMSK          */
//#define SPIDCTLBM27SPIDMSK              (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x16CU))         /* BM27SPIDMSK          */
//#define SPIDCTLBM28SPIDMSK              (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x170U))         /* BM28SPIDMSK          */
//#define SPIDCTLBM29SPIDMSK              (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x174U))         /* BM29SPIDMSK          */
//#define SPIDCTLBM31SPIDMSK              (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x17CU))         /* BM31SPIDMSK          */
//#define SPIDCTLBM03SPIDMSKLOCK          (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x20CU))         /* BM03SPIDMSKLOCK      */
//#define SPIDCTLBM19SPIDMSKLOCK          (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x24CU))         /* BM19SPIDMSKLOCK      */
//#define SPIDCTLBM22SPIDMSKLOCK          (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x258U))         /* BM22SPIDMSKLOCK      */
//#define SPIDCTLBM23SPIDMSKLOCK          (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x25CU))         /* BM23SPIDMSKLOCK      */
//#define SPIDCTLBM24SPIDMSKLOCK          (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x260U))         /* BM24SPIDMSKLOCK      */
//#define SPIDCTLBM25SPIDMSKLOCK          (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x264U))         /* BM25SPIDMSKLOCK      */
//#define SPIDCTLBM26SPIDMSKLOCK          (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x268U))         /* BM26SPIDMSKLOCK      */
//#define SPIDCTLBM27SPIDMSKLOCK          (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x26CU))         /* BM27SPIDMSKLOCK      */
//#define SPIDCTLBM28SPIDMSKLOCK          (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x270U))         /* BM28SPIDMSKLOCK      */
//#define SPIDCTLBM29SPIDMSKLOCK          (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x274U))         /* BM29SPIDMSKLOCK      */
//#define SPIDCTLBM31SPIDMSKLOCK          (*(volatile uint32_t *)(REG_SPIDCTL_BASE + 0x27CU))         /* BM31SPIDMSKLOCK      */

#define REG_GUARD_CRAMCRG2_BASE         (0xFFC6D400UL)                                              /* <GUARD_CRAMCRG2_base>      0xFFC6D400  */
//#define GUARD_CRAMCRG2CRGKCPROT         (*(volatile uint32_t *)REG_GUARD_CRAMCRG2_BASE)             /* CRAMCRG2CRGKCPROT    */
