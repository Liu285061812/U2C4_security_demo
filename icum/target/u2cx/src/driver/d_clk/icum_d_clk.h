/******************************************************************************/
/* Component Name   ICUM CLK Driver                                           */
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

#if !defined(ICUM_D_CLK_H)
#define ICUM_D_CLK_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/

/* ***Internal Definition*** */

#define CLK_CLOCKSTABLE_COUNT       (0x3FFFUL)          /* OSC Stabling Time Count */
#define CLK_PLLSTABLE_COUNT         (0x9FFFUL)          /* PLL Confirming Time Count after switching */
#define CLK_DEMULTISTABLE_COUNT     (0x3FFFUL)          /* DEMULTI Confirming Time Count after switching */
#define CLK_STABLE_WAIT_COUNT       (4UL)               /* OSC/PLL Stable Confirmation Count */

#define CLK_MOSCE_START             (0x00000001UL)      /* Main OSC Enable Trigger ( Start ) */
#define CLK_MOSCEN_ENABLE_SET       (0x00000001UL)      /* Main OSC Enable Trigger is set */

#define CLK_PLLE_START              (0x00000001UL)      /* PLL Enable Trigger ( Start ) */
#define CLK_PLLS_ENABLE_SET         (0x00000001UL)      /* PLL Enable Trigger is set */

#define CLK_MOSCSTAB_STABLE         (0x00000002UL)      /* Main OSC Stabling Status */
#define CLK_PLLSTAB_STABLE          (0x00000002UL)      /* PLL Stabling Status */

#define CLK_CKSC_C_PLL              (0x00000000UL)      /* PLL Specification Value of Clock Source Control */
#define CLK_CKSC_C_SSCG             (0x00000000UL)      /* SSCG Specification Value of Clock Source Control */
#define CLK_CKSC_C_SSCG1            (0x00000000UL)      /* SSCG1 Specification Value of Clock Source Control */
#define CLK_CKSC_C_ISO              (0x00000001UL)      /* ISO Specification Value of Clock Source Control */

#define CLK_CKSC_S_PLL              (0x00000000UL)      /* Useing Clock Source PLL */
#define CLK_CKSC_S_SSCG             (0x00000000UL)      /* Useing Clock Source SSCG */
#define CLK_CKSC_S_SSCG1            (0x00000000UL)      /* Useing Clock Source SSCG1 */
#define CLK_CKSC_S_ISO              (0x00000001UL)      /* Useing Clock Source ISO */

#define CLK_CLKDIV_NODIV            (0x00000000UL)      /* No Division of a frequency division ratio */
#define CLK_CLKDIV_DIV3BY8          (0x00000006UL)      /* 3/8 Division of a frequency division ratio */
#define CLK_CLKDIV_DIV4BY8          (0x00000008UL)      /* 4/8 Division of a frequency division ratio */
#define CLK_CLKDIV_DIV5BY8          (0x0000000AUL)      /* 5/8 Division of a frequency division ratio */
#define CLK_CLKDIV_DIV6BY8          (0x0000000CUL)      /* 6/8 Division of a frequency division ratio */
#define CLK_CLKDIV_DIV7BY8          (0x0000000EUL)      /* 7/8 Division of a frequency division ratio */
#define CLK_CLKDIV_CHGCYCL5         (5U)                /* Division ratio change cycle 5th            */
#define CLK_CLKDIV_CHGCYCL4         (4U)                /* Division ratio change cycle 4th            */

/* PLL Clock Divider Status Register */
#define CLK_SYNC_CORRESPOND         (0x00000002UL)      /* Setting Status of a frequency division ratio */
#define CLK_SYNC_NOTCORRESPOND      (0x00000000UL)      /* Not Setting Status of a frequency division ratio */

#define CLK_PLL_VOLTSTABLE_COUNT    (0x00000400UL)
#define CLK_DIV_VOLTSTABLE_COUNT    (0x00000800UL)

/* CLMA threshold to detect the error */
#define CLK_CLMACMPL_U2B_HBUS80          (0x04B9U)      /* Lower threshold when HBUS is 80 MHz  */
#define CLK_CLMACMPL_U2B_HBUS100         (0x05E8U)      /* Lower threshold when HBUS is 100 MHz */
#define CLK_CLMACMPH_U2B_HBUS80          (0x05EBU)      /* Upper threshold when HBUS is 80 MHz  */
#define CLK_CLMACMPH_U2B_HBUS100         (0x073CU)      /* Upper threshold when HBUS is 100 MHz */

/* Enable or disable CLMA */
#define CLK_CLMACTL_ENABLED                 (0x01U)      /* Enable */

/* Values used for self-test process */
#define CLK_CLMATEST_ENABLED        (0x00000008UL)      /* Self-test ON*/
#define CLK_CLMATEST_MSKERR_ON      (0x00000004UL)      /* Mask the error notification */
#define CLK_CLMATEST_RESCLM_SET     (0x00000001UL)      /* Signal to disable CLMAS sent */

/* OPBT11  (CSAk_base:0xFF320800 + 03CC) */
#define CLK_CFGSET_OPBT11           (*(volatile uint32_t *)0xFF320BCCUL)

/* Clock Setting 400MHz Mask */
#define CLK_SYSCLCK_400M            (0xC0000000uL)
/* Clock Setting 400MHz Mask */
#define CLK_SYSCLCK_320M            (0x80000000uL)

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    PLL Enable Register PLLE
*/
/* PLL disable trigger (PLLDISTRG) */
#define _CGC_PLL_STOP                        (0x00000002UL) /* Stops PLL */
/* PLL enable trigger (PLLENTRG) */
#define _CGC_PLL_START                       (0x00000001UL) /* Starts PLL */

/*
    PLL Status Register (PLLS)
*/
/* PLL stable status (PLLCLKSTAB) */
#define _CGC_PLL_ACTIVE                      (0x00000003UL) /* PLL is active */

/* PLL stop request mask (PLLSTPMSK) */
#define _CGC_PLL_REQUEST_STOP                (0x00000000UL) /* PLL stops operation in standby mode */
#define _CGC_PLL_REQUEST_CONTINUE            (0x00000001UL) /* PLL continues operation in standby mode */

/*
    MainOSC Enable Register MOSCE
*/
/* MainOSC disable trigger (MOSCDISTRG) */
#define _CGC_MAINOSC_STOP                    (0x00000002UL) /* Stops MainOSC */
/* MainOSC enable trigger (MOSCENTRG) */
#define _CGC_MAINOSC_START                   (0x00000001UL) /* Starts MainOSC */

/*
    MainOSC Status Register (MOSCS)
*/
/* MainOSC active status (MOSCCLKACT) */
#define _CGC_MAINOSC_ACTIVE                  (0x00000003UL) /* MainOSC is active */

/*
    MainOSC Stop Mask Register (MOSCSTPM)
*/
/* MainOSC stop request mask (MOSCSTPMSK) */
#define _CGC_MAINOSC_REQUEST_STOP            (0x00000000UL) /* MainOSC stops operation in standby mode */
#define _CGC_MAINOSC_REQUEST_CONTINUE        (0x00000001UL) /* MainOSC continues operation in standby mode */

/*
    HS IntOSC Status Register (HSOSCS)
*/
/* HS IntOSC active status (HSOSCSTAB) */
#define _CGC_HSOSC_ACTIVE                    (0x00000002UL) /* HS IntOSC is active */

/*
    HS IntOSC Stop Mask Register (HSOSCSTPM)
*/
/* HS IntOSC Stop Request Mask (HSOSCSTPMSK) */
#define _CGC_HSOSC_REQUEST_STOP              (0x00000000UL) /* HS IntOSC stops operation in standby mode */
#define _CGC_HSOSC_REQUEST_CONTINUE          (0x00000001UL) /* HS IntOSC continues operation in standby mode */

/*
    CLK_CPU Selector Control Register(CKSC_CPUC)
*/
/* Clock source control(CPUCLKSCSID) */
#define _CGC_CPU_CLK_SOURCE_IOSC             (0x00000001UL) /* CLK_IOSC (default) */
#define _CGC_CPU_CLK_SOURCE_PLLO             (0x00000000UL) /* CLK_PLLO */

/*
    CLK_PLLO Divider Control Register(CLKD_PLLC)
*/
/* Clock Divider control(PLLCLKDCSID) */
#define _CGC_CLK_PLLO_DIVIDER_1              (0x00000001UL) /* No division (default) */
#define _CGC_CLK_PLLO_DIVIDER_2              (0x00000002UL) /* Divided by 2 */

/*
    CLK_PLLO Divider Status Register(CLKD_PLLS)
*/
/* Divider clock synchronized(PLLCLKDSYNC) */
#define _CGC_PLLC_DIVIDER_SYNC               (0x00000002UL) /* Clock output corresponds to the actual divisor setting
                                                               in PLLCLKDCSID */

/*
    CLK_RLIN Selector Control Register(CKSC_RLINC)
*/
/* Clock source control(RLINSCSID) */
#define _CGC_RLIN_CLK_SOURCE_HSB             (0x00000001UL) /* HSB */
#define _CGC_RLIN_CLK_SOURCE_MAINOSC_1       (0x00000000UL) /* MainOSC / 1 */
#define _CGC_RLIN_CLK_SOURCE_MAINOSC_4       (0x00000002UL) /* MainOSC / 4 */
#define _CGC_RLIN_CLK_SOURCE_MAINOSC_8       (0x00000003UL) /* MainOSC / 8 */

/*
    CLK_RCANOSC Selector Control Register(CKSC_RCANC)
*/
/* Clock source control(RCANSCSID) */
#define _CGC_RCAN_CLK_SOURCE_MAINOSC_1       (0x00000001UL) /* MainOSC / 1 */
#define _CGC_RCAN_CLK_SOURCE_MAINOSC_2       (0x00000002UL) /* MainOSC / 2 */
#define _CGC_RCAN_CLK_SOURCE_MAINOSC_4       (0x00000003UL) /* MainOSC / 4 */

/*
    CLK_ADC Selector Control Register(CKSC_ADCC)
*/
/* Clock source control(ADCSCSID) */
#define _CGC_ADC_CLK_SOURCE_LSB_1            (0x00000000UL) /* LSB / 1 */
#define _CGC_ADC_CLK_SOURCE_LSB_2            (0x00000001UL) /* LSB / 2 */

/*
    CLK_MSPI Selector Control Register(CKSC_MSPIC)
*/
/* Clock source control(MSPISCSID) */
#define _CGC_MSPIS_CLK_SOURCE_MAINOSC        (0x00000000UL) /* MainOSC */
#define _CGC_MSPIS_CLK_SOURCE_HSB            (0x00000001UL) /* HSB */

/*
    CLKA_WDT Selector Control Register(CKSC_AWDTC)
*/
/* Clock source control(AWDTSCSID) */
#define _CGC_AWDT_CLK_SOURCE_LSOSC_1         (0x00000000UL) /* LSOSC / 1 */
#define _CGC_AWDT_CLK_SOURCE_LSOSC_128       (0x00000001UL) /* LSOSC / 128 */

/*
    CLKA_TAUJ Selector Control Register(CKSC_ATAUJC)
*/
/* Clock source control(ATAUJSCSID) */
#define _CGC_ATAUJ_CLK_SOURCE_LSOSC          (0x00000000UL) /* LSOSC / 1 */
#define _CGC_ATAUJ_CLK_SOURCE_HSOSC_20       (0x00000001UL) /* LSOSC / 20 */
#define _CGC_ATAUJ_CLK_SOURCE_MAINOSC        (0x00000002UL) /* MainOSC */
#define _CGC_ATAUJ_CLK_SOURCE_HSB            (0x00000003UL) /* HSB */

/*
    CLKA_RTCA Selector Control Register(CKSC_ARTCAC)
*/
/* Clock source control(ARTCASCSID) */
#define _CGC_ARTCA_CLK_SOURCE_MAINOSC_16     (0x00000000UL) /* MainOSC / 16 */
#define _CGC_ARTCA_CLK_SOURCE_LSOSC          (0x00000001UL) /* LSOSC */

/*
    CLKA_ADC Selector Control Register(CKSC_AADCC)
*/
/* Clock source control(AADCSCSID) */
#define _CGC_AADC_CLK_SOURCE_MAINOSC         (0x00000000UL) /* MainOSC */
#define _CGC_AADC_CLK_SOURCE_HSOSC_20        (0x00000001UL) /* HSOSC / 20 */
#define _CGC_AADC_CLK_SOURCE_LSB             (0x00000002UL) /* LSB */

/*
    CLKA_ADC Divider Control Register(CLKD_AADCC)
*/
/* Clock Divider control(AADCDCSID) */
#define _CGC_AADC_CLK_DIVIDER_1              (0x00000001UL) /* CKSC_AADCC selection / 1 */
#define _CGC_AADC_CLK_DIVIDER_2              (0x00000000UL) /* CKSC_AADCC selection / 2 */

/*
    FOUT0 Clock Selector Control Register(CKSC_FOUT0C)
*/
/* Source Clock Setting for EXTCLK0O(FOUT0SCSID) */
#define _CGC_FOUT0_CLK_SOURCE_MAINOSC        (0x00000000UL) /* MainOSC */
#define _CGC_FOUT0_CLK_SOURCE_HSB            (0x00000001UL) /* HSB */
#define _CGC_FOUT0_CLK_SOURCE_LSOSC          (0x00000003UL) /* LSOSC */
#define _CGC_FOUT0_CLK_SOURCE_HSOSC_20       (0x00000004UL) /* HSOSC / 20 */

/*
    FOUT0 Clock Divider Status Register(CLKD_FOUT0S)
*/
/* Clock Divider Active(FOUT0CLKACT) */
#define _CGC_FOUT0_OUTPUT_ONGOING            (0x00000002UL) /* Frequency output is ongoing */
/* Clock Divider Synchronized(FOUT0SYNC) */
#define _CGC_FOUT0_DIVIDER_STABLE            (0x00000001UL) /* The clock divider is stable */

/*
    FOUT1 Clock Selector Control Register(CKSC_FOUT1C)
*/
/* Source Clock Setting for EXTCLK1O(FOUT1SCSID) */
#define _CGC_FOUT1_CLK_SOURCE_MAINOSC        (0x00000000UL) /* MainOSC */
#define _CGC_FOUT1_CLK_SOURCE_HSB            (0x00000001UL) /* HSB */
#define _CGC_FOUT1_CLK_SOURCE_LSOSC          (0x00000003UL) /* LSOSC */
#define _CGC_FOUT1_CLK_SOURCE_HSOSC_20       (0x00000004UL) /* HSOSC / 20 */

/*
    FOUT1 Clock Divider Status Register(CLKD_FOUT1S)
*/
/* Clock Divider Active(FOUT1CLKACT) */
#define _CGC_FOUT1_OUTPUT_ONGOING            (0x00000002UL) /* Frequency output is ongoing */
/* Clock Divider Synchronized(FOUT1SYNC) */
#define _CGC_FOUT1_DIVIDER_STABLE            (0x00000001UL) /* The clock divider is stable */

/*
    CLK_WDT Selector Control Register(CKSC_WDTC)
*/
/* Clock source control(WDTSCSID) */
#define _CGC_WDT_CLK_SOURCE_HSOSC_20         (0x00000000UL) /* HSOSC / 20 */
#define _CGC_WDT_CLK_SOURCE_HSOSC_640        (0x00000001UL) /* HSOSC / 640 */
/* Write protected macro definition */
#define _WRITE_PROTECT_ENABLE               (0xA5A5A501UL)  /* Write protected */
#define _WRITE_PROTECT_DISABLE              (0xA5A5A500UL)  /* Write protected */
#define _PORT_WRITE_PROTECT_DISABLE         (0x00000000UL)  /* Write protected */

/**************************************************************************************/
/* Enumuration                                                                        */
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

#endif /* #if !defined(ICUM_D_CLK_H) */
