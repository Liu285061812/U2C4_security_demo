/******************************************************************************/
/* Component Name   Testing the ICU-M                                         */
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
#ifndef _PE_REG_H_
#define _PE_REG_H_

#include DEVICE_FILE

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/

#define REG_PE_CMDREG_BASE           (0xFF1F0000UL)
#define REG_PE_ICU2PES0              (*(volatile uint32_t *)(REG_PE_CMDREG_BASE + 0x0010UL))
#define REG_PE_ICU2PES1              (*(volatile uint32_t *)(REG_PE_CMDREG_BASE + 0x0128UL))
#define REG_PE_ICU2PES2              (*(volatile uint32_t *)(REG_PE_CMDREG_BASE + 0x0158UL))
#define REG_PE_PE2ICUS0              (*(volatile uint32_t *)(REG_PE_CMDREG_BASE + 0x0014UL))
#define REG_PE_PE2ICUS1              (*(volatile uint32_t *)(REG_PE_CMDREG_BASE + 0x012CUL))
#define REG_PE_PE2ICUS2              (*(volatile uint32_t *)(REG_PE_CMDREG_BASE + 0x015CUL))

#define REG_PE_IPIR_BASE             (0xFFFB9000UL)
#define REG_PE_IPIRIPI3EN0           (*(volatile uint8_t  *)(REG_PE_IPIR_BASE + 0x0860UL))
#define REG_PE_IPIRIPI3REQ0          (*(volatile uint8_t  *)(REG_PE_IPIR_BASE + 0x0870UL))
#define REG_PE_IPIRIPI3RCLR0         (*(volatile uint8_t  *)(REG_PE_IPIR_BASE + 0x0874UL))
#define REG_PE_INTC1CPU0EIC3         (*(volatile uint16_t *)(0xFFFC4006UL))

#define REG_PE_INTC1CPU0EIC753       (*(volatile uint16_t *)(0xFFF805E2UL))
#define REG_PE_INTC1CPU0EIC754       (*(volatile uint16_t *)(0xFFF805E4UL))
#define REG_PE_INTC1CPU0EIC755       (*(volatile uint16_t *)(0xFFF805E6UL))

#define REG_PE_IPIRIPI2REQ0          (*(volatile uint8_t  *)(REG_PE_IPIR_BASE + 0x0850UL))
#define REG_PE_IPIRIPI2RCLR0         (*(volatile uint8_t  *)(REG_PE_IPIR_BASE + 0x0854UL))
#define REG_PE_IPIRIPI2EN1           (*(volatile uint8_t  *)(REG_PE_IPIR_BASE + 0x0940UL))
#define REG_PE_INTC1CPU1EIC2         (*(volatile uint16_t *)(0xFFFC8004UL))
#define REG_PE_IPIRIPI2EN2           (*(volatile uint8_t  *)(REG_PE_IPIR_BASE + 0x0A40UL))
#define REG_PE_INTC1CPU2EIC2         (*(volatile uint16_t *)(0xFFFCC004UL))
#define REG_PE_IPIRIPI2EN3           (*(volatile uint8_t  *)(REG_PE_IPIR_BASE + 0x0B40UL))
#define REG_PE_INTC1CPU3EIC2         (*(volatile uint16_t *)(0xFFFD0004UL))

#define REG_PE_OSTM0TT               OSTM0TT

#define REG_PE_GKCPROT               (*(volatile uint32_t *)0xFFC6C000UL)
#define REG_PE_GSPID0                (*(volatile uint32_t *)0xFFC6C044UL)
#define REG_PE_GBAD0                 (*(volatile uint32_t *)0xFFC6C048UL)
#define REG_PE_GADV0                 (*(volatile uint32_t *)0xFFC6C04CUL)

#define REG_PE_FLMD_BASE             (0xFFA00000UL)
#define REG_PE_FLMDCNT               (*(volatile uint32_t *)(REG_PE_FLMD_BASE + 0x00UL))
#define REG_PE_FLMDFPLOCK            (*(volatile uint32_t *)(REG_PE_FLMD_BASE + 0x0CUL))

#define REG_PE_PBGERRSLV6L0_BASE     (0xFFC83000UL)
#define REG_PE_PBGKCPROT_PBG6L0      (*(volatile uint32_t *)(REG_PE_PBGERRSLV6L0_BASE + 0x18UL))
#define REG_PE_PBG6L0_BASE           (0xFFC81000UL)
#define REG_PE_PBGOPROT0_6           (*(volatile uint32_t *)(REG_PE_PBG6L0_BASE + 0x30UL)) /* 0x30 = 6 * 0x08 */
#define REG_PE_PBGOPROT1_6           (*(volatile uint32_t *)(REG_PE_PBG6L0_BASE + 0x04UL + 0x30UL)) /* 0x30 = 6 * 0x08 */

/* FACI 0 */
#define REG_PE_FACI_BASE            (0xFFA10000UL)
#define REG_PE_FACIFASTAT           (*(volatile uint8_t  *)(REG_PE_FACI_BASE + 0x10UL))
#define REG_PE_FACIFSADDR           (*(volatile uint32_t *)(REG_PE_FACI_BASE + 0x30UL))
#define REG_PE_FACIFSTATR           (*(volatile uint32_t *)(REG_PE_FACI_BASE + 0x80UL))
#define REG_PE_FACIFENTRYR          (*(volatile uint16_t *)(REG_PE_FACI_BASE + 0x84UL))
#define REG_PE_FACICMD              (*(volatile uint8_t  *)(0xFFA20000UL))
#define REG_PE_FACICMDDT            (*(volatile uint32_t *)(0xFFA20000UL))

#define REG_PE_FCU_SYSCTRL_BASE      (0xFF984800UL)

/* FACI 0 */
#define REG_PE_FACIFHVE3            (*(volatile uint32_t *)(REG_PE_FCU_SYSCTRL_BASE + 0x0000UL))
#define REG_PE_FACIFHVE15           (*(volatile uint32_t *)(REG_PE_FCU_SYSCTRL_BASE + 0x0004UL))

#define REG_PE_SWSRESA               (*(volatile uint32_t *)0xFF98840CUL)
#define REG_PE_RESKCPROT             (*(volatile uint32_t *)0xFF980F00UL)

#define REG_PE_BOOT_BASE             (0xFFFB0000UL)
#define REG_PE_PFSYSTEMBOOTCTRL      (*(volatile uint32_t *)(REG_PE_BOOT_BASE + 0x2000UL))



#define REG_PE_PEFSACS0REQ           (*(volatile uint32_t *)0xFF1F00C0UL)
#define REG_PE_PEFSACS0ACK           (*(volatile uint32_t *)0xFF1F00C4UL)
#define REG_PE_PEFSACS1REQ           (*(volatile uint32_t *)0xFF1F00C8UL)
#define REG_PE_PEFSACS1ACK           (*(volatile uint32_t *)0xFF1F00CCUL)


#endif /*_PE_REG_H_ */
