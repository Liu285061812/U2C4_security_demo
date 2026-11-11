/******************************************************************************/
/* Component Name   COMM driver header file                                   */
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
#include "renesas_types.h"

#if !defined(ICUM_D_COMM_REG_H)
#define ICUM_D_COMM_REG_H

/**********************************************************************************
* Define
***********************************************************************************/
typedef struct {
    uint32_t ICUID;          /* [R]   ICUM ID register                                   <ICUM_CMDREG_base>+0000H */
    uint32_t ICUVER;         /* [R]   ICUM version register                              <ICUM_CMDREG_base>+0004H */
    uint8_t  dummy1[8];
    uint32_t ICU2PES0;       /* [R/W] ICUMHB internal status information register 0      <ICUM_CMDREG_base>+0010H */
    uint32_t PE2ICUS0;       /* [R]   CPU (PE) internal status information register 0    <ICUM_CMDREG_base>+0014H */
    uint32_t ACTFLAG0;       /* [R]   ICUMHB activating flag register                    <ICUM_CMDREG_base>+0018H */
    uint8_t  dummy2[36];
    uint32_t PE2ICUF0;       /* [R]   CPU (PE) interrupt request flag register 0         <ICUM_CMDREG_base>+0040H */
    uint32_t PE2ICUFS0;      /* [-]   ICUM_PE2ICUF0 set register                         <ICUM_CMDREG_base>+0044H */
    uint32_t PE2ICUFC0;      /* [W]   ICUM_PE2ICUF0 clear register                       <ICUM_CMDREG_base>+0048H */
    uint32_t PE2ICUIE0;      /* [R/W] ICUM_PE2ICUF0 enable register                      <ICUM_CMDREG_base>+004CH */
    uint32_t ICU2PEF0;       /* [R]   ICUP interrupt request flag register 0             <ICUM_CMDREG_base>+0050H */
    uint32_t ICU2PEFS0;      /* [W]   ICUM_ICU2PEF0 set register                         <ICUM_CMDREG_base>+0054H */
    uint32_t ICU2PEFC0;      /* [-]   ICUM_ICU2PEF0 clear register                       <ICUM_CMDREG_base>+0058H */
    uint32_t ICU2PEIE0;      /* [R]   ICUM_ICU2PEF0 enable register                      <ICUM_CMDREG_base>+005CH */
    uint8_t  dummy3[4];
    uint32_t PE2ICUFSA0;     /* [R/W] ICUM_PE2ICUFS0 authority control setting register  <ICUM_CMDREG_base>+0064H */
    uint8_t  dummy4[8];
    uint32_t PE2ICUSF;       /* [R]   CPU(PE) interrupt single request flag register     <ICUM_CMDREG_base>+0070H */
    uint32_t PE2ICUSFS;      /* [-]   ICUM_PE2ICUSF set register                         <ICUM_CMDREG_base>+0074H */
    uint32_t PE2ICUSFC;      /* [W]   ICUM_PE2ICUSF clear register                       <ICUM_CMDREG_base>+0078H */
    uint32_t PE2ICUSIE;      /* [R/W] ICUM_PE2ICUSF enable register                      <ICUM_CMDREG_base>+007CH */
    uint32_t ICU2PESF;       /* [R]   ICUP interrupt single request flag register        <ICUM_CMDREG_base>+0080H */
    uint32_t ICU2PESFS;      /* [W]   ICUM_ICU2PESF set register                         <ICUM_CMDREG_base>+0084H */
    uint32_t ICU2PESFC;      /* [-]   ICUM_ICU2PESF clear register                       <ICUM_CMDREG_base>+0088H */
    uint32_t ICU2PESIE;      /* [R]   ICUM_ICU2PESF enable register                      <ICUM_CMDREG_base>+008CH */
    uint32_t ICU2PESIS;      /* [R]   Interrupt of ICUM_ICU2PESF dest selection register <ICUM_CMDREG_base>+0090H */
    uint32_t PE2ICUSFSA;     /* [R]   ICUM_PE2ICUSFS authority control setting register  <ICUM_CMDREG_base>+0094H */
    uint8_t  dummy5[8];
    uint32_t SEMAPE0;        /* [R/W] CPU0 (PE0) Semaphore register                      <ICUM_CMDREG_base>+00A0H */
    uint32_t SEMAPE1;        /* [R/W] CPU1 (PE1) Semaphore register                      <ICUM_CMDREG_base>+00A4H */
    uint32_t SEMAPE2;        /* [R/W] CPU2 (PE2) Semaphore register                      <ICUM_CMDREG_base>+00A8H */
    uint32_t SEMAPE3;        /* [R/W] CPU3 (PE3) Semaphore register                      <ICUM_CMDREG_base>+00ACH */
    uint32_t SEMAPE4;        /* [R/W] CPU4 (PE4) Semaphore register                      <ICUM_CMDREG_base>+00B0H */
    uint32_t SEMAPE5;        /* [R/W] CPU5 (PE5) Semaphore register                      <ICUM_CMDREG_base>+00B4H */
    uint8_t  dummy6[72];
    uint32_t PE2ICUF1;       /* [R]   CPU (PE) interrupt request flag register 1         <ICUM_CMDREG_base>+0100H */
    uint32_t PE2ICUFS1;      /* [-]   ICUM_PE2ICUF1 set register                         <ICUM_CMDREG_base>+0104H */
    uint32_t PE2ICUFC1;      /* [W]   ICUM_PE2ICUF1 clear register                       <ICUM_CMDREG_base>+0108H */
    uint32_t PE2ICUIE1;      /* [R/W] ICUM_PE2ICUF1 enable register                      <ICUM_CMDREG_base>+010CH */
    uint32_t ICU2PEF1;       /* [R]   ICUP interrupt request flag register 1             <ICUM_CMDREG_base>+0110H */
    uint32_t ICU2PEFS1;      /* [W]   ICUM_ICU2PEF1 set register                         <ICUM_CMDREG_base>+0114H */
    uint32_t ICU2PEFC1;      /* [-]   ICUM_ICU2PEF1 clear register                       <ICUM_CMDREG_base>+0118H */
    uint32_t ICU2PEIE1;      /* [R]   ICUM_ICU2PEF1 enable register                      <ICUM_CMDREG_base>+011CH */
    uint8_t  dummy7[4];
    uint32_t PE2ICUFSA1;     /* [R/W] ICUM_PE2ICUFS1 authority control setting register  <ICUM_CMDREG_base>+0124H */
    uint32_t ICU2PES1;       /* [R/W] ICUMHB internal status information register 1      <ICUM_CMDREG_base>+0128H */
    uint32_t PE2ICUS1;       /* [R]   CPU (PE) internal status information register 1    <ICUM_CMDREG_base>+012CH */
    uint32_t PE2ICUF2;       /* [R]   CPU (PE) interrupt request flag register 2         <ICUM_CMDREG_base>+0130H */
    uint32_t PE2ICUFS2;      /* [-]   ICUM_PE2ICUF2 set register                         <ICUM_CMDREG_base>+0134H */
    uint32_t PE2ICUFC2;      /* [W]   ICUM_PE2ICUF2 clear register                       <ICUM_CMDREG_base>+0138H */
    uint32_t PE2ICUIE2;      /* [R/W] ICUM_PE2ICUF2 enable register                      <ICUM_CMDREG_base>+013CH */
    uint32_t ICU2PEF2;       /* [R]   ICUP interrupt request flag register 2             <ICUM_CMDREG_base>+0140H */
    uint32_t ICU2PEFS2;      /* [W]   ICUM_ICU2PEF2 set register                         <ICUM_CMDREG_base>+0144H */
    uint32_t ICU2PEFC2;      /* [-]   ICUM_ICU2PEF2 clear register                       <ICUM_CMDREG_base>+0148H */
    uint32_t ICU2PEIE2;      /* [R]   ICUM_ICU2PEF2 enable register                      <ICUM_CMDREG_base>+014CH */
    uint8_t  dummy8[4];
    uint32_t PE2ICUFSA2;     /* [R/W] ICUM_PE2ICUFS2 authority control setting register  <ICUM_CMDREG_base>+0154H */
    uint32_t ICU2PES2;       /* [R/W] ICUMHB internal status information register 2      <ICUM_CMDREG_base>+0158H */
    uint32_t PE2ICUS2;       /* [R]   CPU (PE) internal status information register 2    <ICUM_CMDREG_base>+015CH */
    uint32_t PE2ICUF3;       /* [R]   CPU (PE) interrupt request flag register 3         <ICUM_CMDREG_base>+0160H */
    uint32_t PE2ICUFS3;      /* [-]   ICUM_PE2ICUF3 set register                         <ICUM_CMDREG_base>+0164H */
    uint32_t PE2ICUFC3;      /* [W]   ICUM_PE2ICUF3 clear register                       <ICUM_CMDREG_base>+0168H */
    uint32_t PE2ICUIE3;      /* [R/W] ICUM_PE2ICUF3 enable register                      <ICUM_CMDREG_base>+016CH */
    uint32_t ICU2PEF3;       /* [R]   ICUP interrupt request flag register 3             <ICUM_CMDREG_base>+0170H */
    uint32_t ICU2PEFS3;      /* [W]   ICUM_ICU2PEF3 set register                         <ICUM_CMDREG_base>+0174H */
    uint32_t ICU2PEFC3;      /* [-]   ICUM_ICU2PEF3 clear register                       <ICUM_CMDREG_base>+0178H */
    uint32_t ICU2PEIE3;      /* [R]   ICUM_ICU2PEF3 enable register                      <ICUM_CMDREG_base>+017CH */
    uint8_t  dummy9[4];
    uint32_t PE2ICUFSA3;     /* [R/W] ICUM_PE2ICUFS3 authority control setting register  <ICUM_CMDREG_base>+0184H */
    uint32_t ICU2PES3;       /* [R/W] ICUMHA internal status information register 3      <ICUM_CMDREG_base>+0188H */
    uint32_t PE2ICUS3;       /* [R]   CPU (PE) internal status information register 3    <ICUM_CMDREG_base>+018CH */
    uint32_t PE2ICUF4;       /* [R]   CPU (PE) interrupt request flag register 4         <ICUM_CMDREG_base>+0190H */
    uint32_t PE2ICUFS4;      /* [-]   ICUM_PE2ICUF4 set register                         <ICUM_CMDREG_base>+0194H */
    uint32_t PE2ICUFC4;      /* [W]   ICUM_PE2ICUF4 clear register                       <ICUM_CMDREG_base>+0198H */
    uint32_t PE2ICUIE4;      /* [R/W] ICUM_PE2ICUF4 enable register                      <ICUM_CMDREG_base>+019CH */
    uint32_t ICU2PEF4;       /* [R]   ICUP interrupt request flag register 4             <ICUM_CMDREG_base>+01A0H */
    uint32_t ICU2PEFS4;      /* [W]   ICUM_ICU2PEF4 set register                         <ICUM_CMDREG_base>+01A4H */
    uint32_t ICU2PEFC4;      /* [-]   ICUM_ICU2PEF4 clear register                       <ICUM_CMDREG_base>+01A8H */
    uint32_t ICU2PEIE4;      /* [R]   ICUM_ICU2PEF4 enable register                      <ICUM_CMDREG_base>+01ACH */
    uint8_t  dummy10[4];
    uint32_t PE2ICUFSA4;     /* [R/W] ICUM_PE2ICUFS4 authority control setting register  <ICUM_CMDREG_base>+01B4H */
    uint32_t ICU2PES4;       /* [R/W] ICUMHA internal status information register 4      <ICUM_CMDREG_base>+01B8H */
    uint32_t PE2ICUS4;       /* [R]   CPU (PE) internal status information register 4    <ICUM_CMDREG_base>+01BCH */
    uint32_t PE2ICUF5;       /* [R]   CPU (PE) interrupt request flag register 5         <ICUM_CMDREG_base>+01C0H */
    uint32_t PE2ICUFS5;      /* [-]   ICUM_PE2ICUF5 set register                         <ICUM_CMDREG_base>+01C4H */
    uint32_t PE2ICUFC5;      /* [W]   ICUM_PE2ICUF5 clear register                       <ICUM_CMDREG_base>+01C8H */
    uint32_t PE2ICUIE5;      /* [R/W] ICUM_PE2ICUF5 enable register                      <ICUM_CMDREG_base>+01CCH */
    uint32_t ICU2PEF5;       /* [R]   ICUP interrupt request flag register 5             <ICUM_CMDREG_base>+01D0H */
    uint32_t ICU2PEFS5;      /* [W]   ICUM_ICU2PEF5 set register                         <ICUM_CMDREG_base>+01D4H */
    uint32_t ICU2PEFC5;      /* [-]   ICUM_ICU2PEF5 clear register                       <ICUM_CMDREG_base>+01D8H */
    uint32_t ICU2PEIE5;      /* [R]   ICUM_ICU2PEF5 enable register                      <ICUM_CMDREG_base>+01DCH */
    uint8_t  dummy11[4];
    uint32_t PE2ICUFSA5;     /* [R/W] ICUM_PE2ICUFS5 authority control setting register  <ICUM_CMDREG_base>+01E4H */
    uint32_t ICU2PES5;       /* [R/W] ICUMHA internal status information register 5      <ICUM_CMDREG_base>+01E8H */
    uint32_t PE2ICUS5;       /* [R]   CPU (PE) internal status information register 5    <ICUM_CMDREG_base>+01ECH */
    uint32_t PE2ICUF6;       /* [R]   CPU (PE) interrupt request flag register 6         <ICUM_CMDREG_base>+01F0H */
    uint32_t PE2ICUFS6;      /* [-]   ICUM_PE2ICUF6 set register                         <ICUM_CMDREG_base>+01F4H */
    uint32_t PE2ICUFC6;      /* [W]   ICUM_PE2ICUF6 clear register                       <ICUM_CMDREG_base>+01F8H */
    uint32_t PE2ICUIE6;      /* [R/W] ICUM_PE2ICUF6 enable register                      <ICUM_CMDREG_base>+01FCH */
    uint32_t ICU2PEF6;       /* [R]   ICUP interrupt request flag register 6             <ICUM_CMDREG_base>+0200H */
    uint32_t ICU2PEFS6;      /* [W]   ICUM_ICU2PEF6 set register                         <ICUM_CMDREG_base>+0204H */
    uint32_t ICU2PEFC6;      /* [-]   ICUM_ICU2PEF6 clear register                       <ICUM_CMDREG_base>+0208H */
    uint32_t ICU2PEIE6;      /* [R]   ICUM_ICU2PEF6 enable register                      <ICUM_CMDREG_base>+020CH */
    uint8_t  dummy12[4];
    uint32_t PE2ICUFSA6;     /* [R/W] ICUM_PE2ICUFS6 authority control setting register  <ICUM_CMDREG_base>+0214H */
    uint32_t ICU2PES6;       /* [R/W] ICUMHA internal status information register 6      <ICUM_CMDREG_base>+0218H */
    uint32_t PE2ICUS6;       /* [R]   CPU (PE) internal status information register 6    <ICUM_CMDREG_base>+021CH */
    uint32_t PE2ICUF7;       /* [R]   CPU (PE) interrupt request flag register 7         <ICUM_CMDREG_base>+0220H */
    uint32_t PE2ICUFS7;      /* [-]   ICUM_PE2ICUF7 set register                         <ICUM_CMDREG_base>+0224H */
    uint32_t PE2ICUFC7;      /* [W]   ICUM_PE2ICUF7 clear register                       <ICUM_CMDREG_base>+0228H */
    uint32_t PE2ICUIE7;      /* [R/W] ICUM_PE2ICUF7 enable register                      <ICUM_CMDREG_base>+022CH */
    uint32_t ICU2PEF7;       /* [R]   ICUP interrupt request flag register 7             <ICUM_CMDREG_base>+0230H */
    uint32_t ICU2PEFS7;      /* [W]   ICUM_ICU2PEF7 set register                         <ICUM_CMDREG_base>+0234H */
    uint32_t ICU2PEFC7;      /* [-]   ICUM_ICU2PEF7 clear register                       <ICUM_CMDREG_base>+0238H */
    uint32_t ICU2PEIE7;      /* [R]   ICUM_ICU2PEF7 enable register                      <ICUM_CMDREG_base>+023CH */
    uint8_t  dummy13[4];
    uint32_t PE2ICUFSA7;     /* [R/W] ICUM_PE2ICUFS7 authority control setting register  <ICUM_CMDREG_base>+0244H */
    uint32_t ICU2PES7;       /* [R/W] ICUMHA internal status information register 7      <ICUM_CMDREG_base>+0248H */
    uint32_t PE2ICUS7;       /* [R]   CPU (PE) internal status information register 7    <ICUM_CMDREG_base>+024CH */
    uint8_t  dummy14[176];
    uint32_t PE2ICUSA0;      /* [R/W] ICUM_PE2ICUS0 authority control setting register   <ICUM_CMDREG_base>+0300H */
    uint32_t PE2ICUSA1;      /* [R/W] ICUM_PE2ICUS1 authority control setting register   <ICUM_CMDREG_base>+0304H */
    uint32_t PE2ICUSA2;      /* [R/W] ICUM_PE2ICUS2 authority control setting register   <ICUM_CMDREG_base>+0308H */
    uint32_t PE2ICUSA3;      /* [R/W] ICUM_PE2ICUS3 authority control setting register   <ICUM_CMDREG_base>+030CH */
    uint32_t PE2ICUSA4;      /* [R/W] ICUM_PE2ICUS4 authority control setting register   <ICUM_CMDREG_base>+0310H */
    uint32_t PE2ICUSA5;      /* [R/W] ICUM_PE2ICUS5 authority control setting register   <ICUM_CMDREG_base>+0314H */
    uint32_t PE2ICUSA6;      /* [R/W] ICUM_PE2ICUS6 authority control setting register   <ICUM_CMDREG_base>+0318H */
    uint32_t PE2ICUSA7;      /* [R/W] ICUM_PE2ICUS7 authority control setting register   <ICUM_CMDREG_base>+031CH */
    uint8_t  dummy15[16];
    uint32_t RACSCC0;        /* [R/W] Code Flash secure region start address configuration change register 0
                                                                                         <ICUM_CMDREG_base>+0330H */
    uint32_t RACECC0;        /* [R/W] Code Flash secure region end address configuration change register 0
                                                                                         <ICUM_CMDREG_base>+0334H */
    uint32_t RACSCC1;        /* [R/W] Code Flash secure region start address configuration change register 1
                                                                                         <ICUM_CMDREG_base>+0338H */
    uint32_t RACECC1;        /* [R/W] Code Flash secure region end address configuration change register 1
                                                                                         <ICUM_CMDREG_base>+033CH */
    uint32_t CCPROT;         /* [R/W] Configuration change protection register           <ICUM_CMDREG_base>+0340H */
} REG_ICUM_CMDREG_t;

typedef struct {
    uint32_t BOOTCTRL;       /* [R/W] Boot Control Register                              <BOOT_base> + 2000 H     */
} REG_BOOT_t;

#ifndef WIN32_SIM

#define REG_ICUM_CMDREG_BASE (0xFF1F0000uL)
#define REG_ICUM_CMDREG (*(volatile REG_ICUM_CMDREG_t *)REG_ICUM_CMDREG_BASE)
#define REG_BOOT_BASE (0xFFFB2000uL)
#define REG_BOOT (*(volatile REG_BOOT_t *)REG_BOOT_BASE)

#else
volatile REG_ICUM_CMDREG_t REG_ICUM_CMDREG;
volatile REG_BOOT_t REG_BOOT;

#endif

#endif /* #if !defined(ICUM_D_COMM_REG_H) */

