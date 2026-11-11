/******************************************************************************/
/* Component Name   ICU-M REMAP Driver                                        */
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

#if !defined(ICUM_D_REMAP_H)
#define ICUM_D_REMAP_H

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Definition for Separating Base Address and Offset Address*/
#define SEP_BASE_ADRS         (0xFFE00000U)       /* Base Address */
#define SEP_OFF_ADRS          (0x001FFFFFU)       /* Offset Address */
#define BIAS_2MB              (0x00200000U)       /* 2MB Bias */
#define REMAP_REG_MAX         (16U)               /* Remap MAX */

/* Definition for Register Request Control */
/* Register Status */
#define REG_STS_INIT_STS      (0U)
#define REG_STS_FULREGIST     (0U)                /* No Registering Area */
#define REG_STS_EXTNOTREG     (1U)                /* Exist Not Register Area */
#define REG_STS_EXTUSESAM     (2U)                /* Exist Area is used and had same Physical Address */
#define REG_STS_EXTNOUSAM     (3U)                /* Exist Area not used and same Physical Address */

/* Using Status */
#define USE_STS_NOTEXIST      (0U)                /* Not exist Area not used and different Physical Address */
#define USE_STS_EXIST         (1U)                /* Exist Area not used and different Physical Address */

/* Register Definitions */
#if !defined(TARGET_DEVICE_S4H)
#define ICUMX_CTLREG2_BASE    (0xFF1FC400U)       /* Remap Register Base Address */
#else
#define ICUMX_CTLREG2_BASE    (0xFF1FC880U)       /* Remap Register Base Address */
#endif
#define ICUMX_SICREMAP2M0_REG     (ICUMX_CTLREG2_BASE+0x00U)
#define ICUMX_SICREMAP2M1_REG     (ICUMX_CTLREG2_BASE+0x04U)
#define ICUMX_SICREMAP2M2_REG     (ICUMX_CTLREG2_BASE+0x08U)
#define ICUMX_SICREMAP2M3_REG     (ICUMX_CTLREG2_BASE+0x0CU)
#define ICUMX_SICREMAP2M4_REG     (ICUMX_CTLREG2_BASE+0x10U)
#define ICUMX_SICREMAP2M5_REG     (ICUMX_CTLREG2_BASE+0x14U)
#define ICUMX_SICREMAP2M6_REG     (ICUMX_CTLREG2_BASE+0x18U)
#define ICUMX_SICREMAP2M7_REG     (ICUMX_CTLREG2_BASE+0x1CU)
#define ICUMX_SICREMAP2M8_REG     (ICUMX_CTLREG2_BASE+0x20U)
#define ICUMX_SICREMAP2M9_REG     (ICUMX_CTLREG2_BASE+0x24U)
#define ICUMX_SICREMAP2M10_REG    (ICUMX_CTLREG2_BASE+0x28U)
#define ICUMX_SICREMAP2M11_REG    (ICUMX_CTLREG2_BASE+0x2CU)
#define ICUMX_SICREMAP2M12_REG    (ICUMX_CTLREG2_BASE+0x30U)
#define ICUMX_SICREMAP2M13_REG    (ICUMX_CTLREG2_BASE+0x34U)
#define ICUMX_SICREMAP2M14_REG    (ICUMX_CTLREG2_BASE+0x38U)
#define ICUMX_SICREMAP2M15_REG    (ICUMX_CTLREG2_BASE+0x3CU)

/* for Writing to protected Register */
#define PROTCMD_START         (0x000000A5U)       /* Command Data to release protection */
#define PROTS0ERR             (0x00000001U)       /* Confirming Data to complete writing normally */

/* Remap Table Definitions */
typedef struct {
    uint8_t     reg_flag;                         /* Register Status Flag */
    uint8_t     lru_count;                        /* Counter for LRU */
    uint8_t     reg_count;                        /* Counter of Register */
    uint8_t     size_id;                          /* Area size ID */
    uint32_t    phys_address;                     /* Physical Base Address */
    uint32_t    remap_address;                    /* Access Window Base Address */
    uint32_t    remap_register;                   /* Remap Register Address */
}ICUM_REMAP_REMAP_TABLE_t;

/* Definition for reg_flag (Register Status Flag) */

/* Definition for reg_flag (Register Status Flag) */
#define REGST_NOREG           (0xFFU)             /* Not Registered */
#define REGST_NOUSE           (0xFFU)             /* Not Used */
#define REGST_FIXED           (0x7FU)             /* Fixed Registered */
#define REGST_INH             (0x80U)             /* Inhibit to modify */
/* Fixed REMAP setting : index(12 -- 15) */
#define REG_NUM_12            (12U)               /* MFIS */
#define REG_NUM_13            (13U)               /* RPC (Flash) */
#define REG_NUM_14            (14U)               /* Secure Engine (for Calling MaskROM API) */
#define REG_NUM_15            (15U)               /* RTRAM */

/* Definition for lru_count and reg_count */
#define COUNT_ZERO            (0U)                /* Initial Value */

/* Definition for size_id (Area size ID) */
#define SID_NOA               (0U)                /* Not Assigned */
#define SID_2MB               (2U)                /* Area size is 2MB */
#define SID_4MB               (4U)                /* Area size is 4MB */

/* Definition of size is 0 */
#define SIZE_ZERO             (0U)                /* Size is 0 */

/* Definition of value is 0 */
#define VALUE_ZERO            (0U)                /* Value is 0 */

/* Definition of phys_address (Physical Base Address) */
/*  */
#define PHYS_ADR_NOASGN       (0x00000000U)        /* Not Asigned */
#if !defined(TARGET_DEVICE_S4H)
/* SICREMAP2M12 */
#define PHYS_ADR_MFIS         (0xE6200000U)       /* MFIS */
/* SICREMAP2M13 */
#define PHYS_ADR_RPC          (0xEE200000U)       /* RPC (Flash) */
/* SICREMAP2M14 */
#define PHYS_ADR_SE           (0xE6600000U)       /* Secure Engine (for Calling MaskROM API) */
/* SICREMAP2M15 */
#define PHYS_ADR_RTRAM        (0xEB200000U)       /* RTRAM */
#else
/* SICREMAP2M12 */
#define PHYS_ADR_MFIS         (0xD6200000u)       /* MFIS */
/* SICREMAP2M13 */
#define PHYS_ADR_RPC          (0xDE200000U)       /* RPC (Flash) */
/* SICREMAP2M14 */
#define PHYS_ADR_RST          (0xD6000000U)       /* RST */
/* SICREMAP2M15 */
#define PHYS_ADR_MCUAXI       (0xF8E00000U)       /* MCUAXI */
#endif

#define PHYS_ADRS00_BASE      (PHYS_ADR_NOASGN)   /* for remapping SDRAM area */
#define PHYS_ADRS01_BASE      (PHYS_ADR_NOASGN)   /* for remapping SDRAM area */
#define PHYS_ADRS02_BASE      (PHYS_ADR_NOASGN)   /* for remapping SDRAM area */
#define PHYS_ADRS03_BASE      (PHYS_ADR_NOASGN)   /* for remapping SDRAM area */
#define PHYS_ADRS04_BASE      (PHYS_ADR_NOASGN)   /* for remapping SDRAM area */
#define PHYS_ADRS05_BASE      (PHYS_ADR_NOASGN)   /* for remapping SDRAM area */
#define PHYS_ADRS06_BASE      (PHYS_ADR_NOASGN)   /* for remapping SDRAM area */
#define PHYS_ADRS07_BASE      (PHYS_ADR_NOASGN)   /* for remapping SDRAM area */
#define PHYS_ADRS08_BASE      (PHYS_ADR_NOASGN)   /* for remapping SDRAM area */
#define PHYS_ADRS09_BASE      (PHYS_ADR_NOASGN)   /* for remapping SDRAM area */
#define PHYS_ADRS10_BASE      (PHYS_ADR_NOASGN)   /* for remapping SDRAM area */
#define PHYS_ADRS11_BASE      (PHYS_ADR_NOASGN)   /* for remapping SDRAM area */
#define PHYS_ADRS12_BASE      (PHYS_ADR_MFIS)     /* MFIS */
#define PHYS_ADRS13_BASE      (PHYS_ADR_RPC)      /* RPC (Flash) */
#if !defined(TARGET_DEVICE_S4H)
#define PHYS_ADRS14_BASE      (PHYS_ADR_SE)       /* Secure Engine (for Calling MaskROM API) */
#define PHYS_ADRS15_BASE      (PHYS_ADR_RTRAM)    /* RTRAM */
#else  /* !defined(TARGET_DEVICE_S4H) */
#define PHYS_ADRS14_BASE      (PHYS_ADR_RST)      /* RST */
#define PHYS_ADRS15_BASE      (PHYS_ADR_MCUAXI)   /* MCUAXI */
#endif /* !defined(TARGET_DEVICE_S4H) */

/* Definition of remap_address (Access Window Base Address) */
#define REMAP_ADRS00_BASE     (0xFC000000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M0 Register */
#define REMAP_ADRS01_BASE     (0xFC200000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M1 Register */
#define REMAP_ADRS02_BASE     (0xFC400000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M2 Register */
#define REMAP_ADRS03_BASE     (0xFC600000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M3 Register */
#define REMAP_ADRS04_BASE     (0xFC800000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M4 Register */
#define REMAP_ADRS05_BASE     (0xFCA00000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M5 Register */
#define REMAP_ADRS06_BASE     (0xFCC00000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M6 Register */
#define REMAP_ADRS07_BASE     (0xFCE00000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M7 Register */
#define REMAP_ADRS08_BASE     (0xFD000000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M8 Register */
#define REMAP_ADRS09_BASE     (0xFD200000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M9 Register */
#define REMAP_ADRS10_BASE     (0xFD400000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M10 Register */
#define REMAP_ADRS11_BASE     (0xFD600000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M11 Register */
#define REMAP_ADRS12_BASE     (0xFD800000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M12 Register */
#define REMAP_ADRS13_BASE     (0xFDA00000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M13 Register */
#define REMAP_ADRS14_BASE     (0xFDC00000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M14 Register */
#define REMAP_ADRS15_BASE     (0xFDE00000U)       /* Address of Remap Access Window of ICUMX_SICREMAP2M15 Register */

#define CFREMAP_AREA_START    (0x01200000U)       /* CF Remap Access Window Area Start Address */
#define CFREMAP_AREA_END      (0x013FFFFFU)       /* CF Remap Access Window Area End Address */
#define CFREMAP_ADRS_BASE     (0xEA000000U)       /* CF Remap Access Window Area Base Address */
/* LRU Table Definitions */
typedef struct {
    uint8_t     lru_count;                        /* Register Status Flag */
}ICUM_RMAP_LRU_TABLE_t;

/* Retry Over Definition */
#define REMAP_RETRYOVER       ((uint32_t)1U)
/* Definition for initialization */
#define REMAP_NOTINIT         (0U)
#define REMAP_INIT            (1U)
/* Definition for searching remap table */
#define REMAP_NOTFIND         (0U)
#define REMAP_FIND            (1U)

/* Flag ON/OFF */
#define FLG_OFF (0U)
#define FLG_ON  (1U)

/* User setting Table Definition */
typedef struct {
    uint32_t    addr;
    uint32_t    size;
}ICUM_REMAP_USERSET_TABLE_t;

/* Remap Area */
#define AREA_SDRAM_START      (0x40000000U)       /* Start Address of SDRAM Area */
#define AREA_SDRAM_END        (0xBFFFFFFFU)       /* End Address of SDRAM Area */
#define AREA_SYSTEMRAM_START  (0xE6300000U)       /* Start Address of System RAM Area */
#define AREA_SYSTEMRAM_END    (0xE635FFFFU)       /* End Address of System RAM Area */
#define AREA_VRAM_START       (0xE2000000U)       /* Start Address of VRAM Area */
#define AREA_VRAM_END         (0xE3C00000U)       /* End Address of VRAM Area */

#if defined(TARGET_DEVICE_S4H) || defined(TARGET_DEVICE_U2B) || defined(TARGET_DEVICE_U2C) || defined(TARGET_DEVICE_U2C4)
/* Definition of the base address for Standby control register (logical address) */
#define LOGI_ADDR_STBY_BASE         (0xFDF18200U)

/* Standby control register defines */
#define STBY_CTRL                   (0x0034U)       /*  Bus Standby Control Register */
#define STBY_CTRL_OPBT_RDY          (1U << 0U)
#define LOGI_ADDR_STBY_CTRL         (LOGI_ADDR_STBY_BASE + STBY_CTRL)
#endif /* #if defined(TARGET_DEVICE_S4H) */

/***************************************************************************************/
/* Prototypes of Internal Function                                                     */
/***************************************************************************************/
static uint32_t d_REMAP_SearchPosition(uint32_t phys_base);
#if !defined(TARGET_DEVICE_S4H) && !defined(TARGET_DEVICE_U2B) && !defined(TARGET_DEVICE_U2C) && !defined(TARGET_DEVICE_U2C4)
static uint32_t d_REMAP_SetRemapTbl(uint32_t phys_base);
#endif
static uint32_t d_REMAP_Exec(uint32_t regist_ptr, uint32_t phys_base);
static uint32_t d_REMAP_WriteProcReg(register volatile uint32_t *p_reg, uint32_t phys_base, uint32_t retry);
static uint32_t d_REMAP_DecideRegPosition(uint32_t *p_logical_addr, uint32_t phys_base, uint32_t offset_adrs);
static uint32_t d_REMAP_SetRemapFixedTbl(void);

#endif /* ICUM_D_REMAP_H */
