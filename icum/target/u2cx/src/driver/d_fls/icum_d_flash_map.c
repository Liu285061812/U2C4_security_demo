/******************************************************************************/
/* Component Name   ICUM Flash Driver                                         */
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

/********************************************************************************
* Includes <System Includes> , ProjectIncludes
*********************************************************************************/
#include "r_icumif_api.h"
#include "icum_common_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_flash_pub.h"
#include "icum_d_flash.h"
#include "icum_d_comm_pub.h"

/********************************************************************************
 QAC Suppression message list
*********************************************************************************/

/********************************************************************************
 Memory allocation section specification
*********************************************************************************/
/* self copy icum_driver_rom.text to icum_driver_ram   */
CHANGE_SEC(text, ".icum_driver_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")

CHANGE_SEC(data, ".icum_driver.data")
CHANGE_SEC(bss, ".icum_driver.bss")

/********************************************************************************
 Macro definitions
*********************************************************************************/

/*******************************************************************************
 Typedef definitions
*********************************************************************************/

/********************************************************************************
 Private global variables
*********************************************************************************/
static  uint32_t    g_icum_rac_double;
static  uint32_t    g_icum_rac_single;
static  uint32_t    g_icum_rad = D_FLS_INVALID;
static  uint32_t    g_icum_lower_limit_cf_side_a;
static  uint32_t    g_icum_upper_limit_cf_side_a;
static  uint32_t    g_icum_lower_limit_cf_side_b;
static  uint32_t    g_icum_upper_limit_cf_side_b;

static d_flash_info_t g_code_flash_area_list[D_FLS_MAX_CF_BANK_NUM] =
{
    {(uint32_t)D_FLS_CF_BANKA_START_ADDRESS,    (uint32_t)D_FLS_CF_BANKA_END_ADDRESS },     /* Bank A */
    {(uint32_t)D_FLS_CF_BANKB_START_ADDRESS,    (uint32_t)D_FLS_CF_BANKB_END_ADDRESS }      /* Bank B */
};

static d_flash_info_t g_data_flash_area_list[FLS_DF_BANK_NUM] =
{
    {(uint32_t)D_FLS_DF_BANKA_START_ADDRESS,    (uint32_t)D_FLS_DF_BANKA_END_ADDRESS    },  /* BankA */
    {(uint32_t)D_FLS_DF_BANKB_START_ADDRESS,    (uint32_t)D_FLS_DF_BANKB_END_ADDRESS    },  /* BankB */
    {(uint32_t)D_FLS_DF_BANKICUM_START_ADDRESS, (uint32_t)D_FLS_DF_BANKICUM_END_ADDRESS }   /* Bank for ICUM */
};

static const d_flash_info_t security_area_list[1U] =
{
    {D_FLS_SEC_SETTING_START_ADDRESS,           (D_FLS_SEC_SETTING_END_ADDRESS + 1UL)   }   /* non-BANK */
};

static uint32_t g_fls_boundary_list[D_FLS_BOUNDARY_INDEX_MAX] =
{
    ADDR_LOWER_SCF_SIDEA,                   /* D_FLS_LOWER_LIMIT_CF_SIDEA */
    0uL,                                    /* D_FLS_LOWER_LIMIT_SCF_DOUBLE */
    ADDR_UPPER_SCF_SIDEA,                   /* D_FLS_UPPER_LIMIT_CF_SIDEA */
    ADDR_LOWER_SCF_SIDEB,                   /* D_FLS_LOWER_LIMIT_CF_SIDEB */
    0uL,                                    /* D_FLS_LOWER_LIMIT_SCF_SING */
    ADDR_UPPER_SCF_SIDEB,                   /* D_FLS_UPPER_LIMIT_CF_SIDEB */
    ADDR_LOWER_CRAM,                        /* D_FLS_LOWER_LIMIT_CRAM     */
    ADDR_UPPER_CRAM,                        /* D_FLS_UPPER_LIMIT_CRAM     */
    ADDR_LOWER_SLRAM,                       /* D_FLS_LOWER_LIMIT_SLRAM    */
    ADDR_UPPER_SLRAM,                       /* D_FLS_UPPER_LIMIT_SLRAM    */
    ADDR_LOWER_DF_BANKA,                    /* D_FLS_LOWER_LIMIT_DF_BANKA */
    ADDR_UPPER_DF_BANKA,                    /* D_FLS_UPPER_LIMIT_DF_BANKA */
    ADDR_LOWER_DF_BANKB,                    /* D_FLS_LOWER_LIMIT_DF_BANKB */
    ADDR_UPPER_DF_BANKB,                    /* D_FLS_UPPER_LIMIT_DF_BANKB */
    0uL,                                    /* D_FLS_LOWER_LIMIT_DF_SDF   */
    ADDR_UPPER_DF_SDF,                      /* D_FLS_UPPER_LIMIT_DF_SDF   */
    0uL,                                    /* D_FLS_LOWER_LIMIT_SCF_SIDE */
    0uL,                                    /* D_FLS_UPPER_LIMIT_SCF_SIDE */
    0uL,                                    /* D_FLS_LOWER_LIMIT_SCF_SIDE */
    0uL,                                    /* D_FLS_UPPER_LIMIT_SCF_SIDE */
    ADDR_LOWER_FIRMWARE,                    /* D_FLS_LOWER_LIMIT_FIRMWARE */
    ADDR_UPPER_FIRMWARE,                    /* D_FLS_UPPER_LIMIT_FIRMWARE */
    ADDR_LOWER_FIRMWARE_A,                  /* D_FLS_LOWER_LIMIT_FIRMWARE_A */
    ADDR_UPPER_FIRMWARE_A,                  /* D_FLS_UPPER_LIMIT_FIRMWARE_A */
    ADDR_LOWER_FIRMWARE_B,                  /* D_FLS_LOWER_LIMIT_FIRMWARE_B */
    ADDR_UPPER_FIRMWARE_B,                  /* D_FLS_UPPER_LIMIT_FIRMWARE_B */
    ADDR_LOWER_SECRET_DATA_SECTION,         /* D_FLS_LOWER_LIMIT_SECRET_DATA_SECTION */
    ADDR_UPPER_SECRET_DATA_SECTION,         /* D_FLS_UPPER_LIMIT_SECRET_DATA_SECTION */
    ADDR_LOWER_CF_BANKA,                    /* D_FLS_LOWER_LIMIT_CF_BANKA */
    ADDR_UPPER_CF_BANKA,                    /* D_FLS_UPPER_LIMIT_CF_BANKA */
    ADDR_LOWER_CF_BANKB,                    /* D_FLS_LOWER_LIMIT_CF_BANKB */
    ADDR_UPPER_CF_BANKB,                    /* D_FLS_UPPER_LIMIT_CF_BANKB */
    0uL,                                    /* D_FLS_LOWER_LIMIT_CF_BANKC */
    0uL,                                    /* D_FLS_UPPER_LIMIT_CF_BANKC */
    0uL,                                    /* D_FLS_LOWER_LIMIT_CF_BANKD */
    0uL,                                    /* D_FLS_UPPER_LIMIT_CF_BANKD */
    0uL,                                    /* D_FLS_LOWER_LIMIT_CF_BANKE */
    0uL,                                    /* D_FLS_UPPER_LIMIT_CF_BANKE */
    0uL,                                    /* D_FLS_LOWER_LIMIT_CF_BANKF */
    0uL,                                    /* D_FLS_UPPER_LIMIT_CF_BANKF */
};

/********************************************************************************
 Private function prototypes
*********************************************************************************/
static void d_FLS_CheckSDFAddress(const uint32_t addr, const uint32_t size, uint8_t *p_ret_val);
static void d_FLS_CheckSCFAddress(const uint32_t addr, const uint32_t size, uint8_t *p_ret_val);
static uint32_t d_FLS_GetDFBankNumber(const uint32_t df_addr, uint32_t *p_bank);
static uint32_t d_FLS_CheckMemory(const uint32_t start_addr, const uint32_t end_addr, const uint8_t direction,
                                  uint8_t *p_result);

/*******************************************************************************
* Functions
********************************************************************************/

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_driver.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0082]:[ICUMFW_UD_U2B_D_FLS_0082]
* Function name: D_FLS_SetIcumRacRad
* Description  : set the address boundary of the Secure Code Flash & Secure Data Flash
* Arguments    : icum_racs0     : Start address of secure code flash region 0 (Case of U2B)
*                icum_race0     : End address of secure code flash region 0 (Case of U2B)
*                icum_racs1     : Start address of secure code flash region 1 (Case of U2B)
*                icum_race1     : End address of secure code flash region 1 (Case of U2B)
*                icum_resv      : ICUP reset vector address (Other than U2B)
*                icum_rac       : Start address of secure code flash (Other than U2B)
*                icum_rad       : Start address of secure data flash
* Return Value : none
***************************************************************************************/
void D_FLS_SetIcumRacRad(const uint32_t *icum_racs0, const uint32_t *icum_race0, const uint32_t *icum_racs1, const uint32_t *icum_race1, const uint32_t *icum_rad)
{
    uint32_t boundary_a;
    uint32_t boundary_b;

    g_icum_rad  = *icum_rad;

    (void)D_FLS_GetMemBoundary(D_FLS_LOWER_LIMIT_CF_SIDEA, &boundary_a);
    (void)D_FLS_GetMemBoundary(D_FLS_LOWER_LIMIT_CF_SIDEB, &boundary_b);
    /* Single map mode */
    /* Single map mode */
    if ((boundary_a == boundary_b) || (ADDR_LOWER_SCF_SIDEB <= *icum_racs0))
    {
        /* Single Map/Single Region - Front region */
        g_icum_rac_double = D_FLS_INVALID;
        g_icum_lower_limit_cf_side_a = D_FLS_INVALID;
        g_icum_upper_limit_cf_side_a = D_FLS_INVALID;

        /* Single Map/Single Region - Back region */
        g_icum_rac_single = *icum_racs0;
        g_icum_lower_limit_cf_side_b = *icum_racs0;
        g_icum_upper_limit_cf_side_b = *icum_race0;
    }
    else
    {
        /* Single Map/Double Region - Front region */
        g_icum_rac_double = *icum_racs0;
        g_icum_lower_limit_cf_side_a = *icum_racs0;
        g_icum_upper_limit_cf_side_a = *icum_race0;

        /* Single Map/Double Region - Back region */
        g_icum_rac_single =  *icum_racs1;
        g_icum_lower_limit_cf_side_b = *icum_racs1;
        g_icum_upper_limit_cf_side_b = *icum_race1;
    } /* end of else */

    g_fls_status |= D_FLS_STS_MAP_INITIALIZED;
}

/* self copy icum_driver_rom.text to icum_driver_ram   */
CHANGE_SEC(text, ".icum_driver_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0083]:[ICUMFW_UD_U2B_D_FLS_0083]
* Function name: D_FLS_GetMemBoundary
* Description  : retrieve the address boundary of the memory region specified
* Arguments    : boundary       : The specified boundary
*                p_addr         : Retrieve the boundary address
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_FLS_GetMemBoundary(const uint8_t boundary, uint32_t *p_addr)
{
    uint32_t    err;

    err = ER_OK;
    if (NULL == p_addr)
    {
        err = ER_PARAM;
    }
    else if ((uint32_t)D_FLS_BOUNDARY_INDEX_MAX <= (uint32_t)boundary)
    {
        *p_addr = D_FLS_INVALID;
        err = ER_PARAM;
    }
    else
    {
        switch ((uint32_t)boundary)
        {
            case D_FLS_LOWER_LIMIT_SCF_DOUBLE:
                *p_addr = g_icum_rac_double;
                break;

            case D_FLS_LOWER_LIMIT_SCF_SINGLE:
                *p_addr = g_icum_rac_single;
                break;
            case D_FLS_LOWER_LIMIT_DF_SDF:
                *p_addr = g_icum_rad;
                break;

            case D_FLS_LOWER_LIMIT_SCF_SIDEA:
                *p_addr = g_icum_lower_limit_cf_side_a;
                break;

            case D_FLS_UPPER_LIMIT_SCF_SIDEA:
                *p_addr = g_icum_upper_limit_cf_side_a;
                break;

            case D_FLS_LOWER_LIMIT_SCF_SIDEB:
                *p_addr = g_icum_lower_limit_cf_side_b;
                break;

            case D_FLS_UPPER_LIMIT_SCF_SIDEB:
                *p_addr = g_icum_upper_limit_cf_side_b;
                break;

            default:
                *p_addr = g_fls_boundary_list[boundary];
                break;
        }
    } /* end of else */

    return (err);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_driver.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0087]:[ICUMFW_UD_U2B_D_FLS_0087]
* Function name: d_FLS_CheckSDFAddress
* Description  : Check of address. Is the address SDF or not.
* Arguments    : addr           : Address to be checked
*                size           : Data size
*                p_ret_val      : Check result (FLS_CHK_OK or FLS_CHK_NG)
* Return Value : none
***************************************************************************************/
static void d_FLS_CheckSDFAddress(const uint32_t addr, const uint32_t size, uint8_t *p_ret_val)
{
    /* size check */
    if (addr > (addr + size))
    {
        /* end address over 0xFFFFFFFF */
        *p_ret_val = D_FLS_CHK_NG;
    }
    else
    {
        if ((addr >= g_icum_rad) && (ADDR_UPPER_DF_SDF >= ((addr + size) - 1uL)))
        {
            *p_ret_val = D_FLS_CHK_OK;
        }
        else
        {
            *p_ret_val = D_FLS_CHK_NG;
        }
    }

    return;
}

/* self copy icum_driver_rom.text to icum_driver_ram   */
CHANGE_SEC(text, ".icum_driver_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0099]:[ICUMFW_UD_U2B_D_FLS_0099]
* Function name: d_FLS_CheckSCFAddress
* Description  : Check of address. Is the address SCF or not.
* Arguments    : addr         : Address to be checked
*                size           : Data size
*                p_ret_val      : Check result (FLS_CHK_OK or FLS_CHK_NG)
* Return Value : none
***************************************************************************************/
static void d_FLS_CheckSCFAddress(const uint32_t addr, const uint32_t size, uint8_t *p_ret_val)
{
    /* size check */
    if (addr > (addr + size))
    {
        /* end address over 0xFFFFFFFF */
        *p_ret_val = D_FLS_CHK_NG;
    }
    else
    {
        if (D_FLS_INVALID == g_icum_rac_double)
        {
            if ((addr >= g_icum_rac_single) && (ADDR_UPPER_SCF_SIDEB >= ((addr + size) - 1uL)))
            {
                *p_ret_val = D_FLS_CHK_OK;
            }
            else
            {
                *p_ret_val = D_FLS_CHK_NG;
            }
        }
        else
        {
            if (((addr >= g_icum_rac_double) && (ADDR_UPPER_SCF_SIDEA >= ((addr + size) - 1uL)))
                || ((addr >= g_icum_rac_single) && (ADDR_UPPER_SCF_SIDEB >= ((addr + size) - 1uL))))
            {
                *p_ret_val = D_FLS_CHK_OK;
            }
            else
            {
                *p_ret_val = D_FLS_CHK_NG;
            }

        }
    }

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0088]:[ICUMFW_UD_U2B_D_FLS_0088]
* Function name: D_FLS_CheckAddr_ram
* Description  : (1) Check whether the specified offset value is in the target area.
*                (2) Convert offset values to physical addresses.
* Arguments    : target_area            : Target flash area.
*                bank                   : Target bank number.
*                offset                 : Byte offset position of the area.
*                size                   : Size.(in 4-byte units)
*                p_addr                 : Target physical address (The caller must guarantee not NULL)
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_FLS_CheckAddr_ram(const uint32_t target_area, const uint32_t bank, const uint32_t offset,
                             const uint32_t size, uint32_t *p_addr)
{
    uint32_t phy_addr_bgn;
    uint32_t phy_addr_end;
    const d_flash_info_t *p_info_tbl;
    uint32_t bank_num;
    uint32_t lul_fls_status;

    lul_fls_status = ER_PARAM;

    switch (target_area)
    {
        case AG_D_FLS_CODE:
            p_info_tbl = g_code_flash_area_list;
            bank_num = D_FLS_CF_BANK_NUM;
            break;
        case AG_D_FLS_DATA:
            p_info_tbl = g_data_flash_area_list;
            bank_num   = sizeof (g_data_flash_area_list)/sizeof(d_flash_info_t);
            break;
        case AG_D_FLS_SECURITY:
            p_info_tbl = security_area_list;
            bank_num   = sizeof (security_area_list)/sizeof(d_flash_info_t);
            break;
        default:
            p_info_tbl = NULL;
            break;
    }

     /* target area check */
    if (p_info_tbl == NULL)
    {
        /* Invalid target area. Do nothing */
    }
    else
    {
        /* bank number check */
        if (bank_num < (bank + 1UL))
        {
            /* Invalid bank number. Do nothing */
        }
        else
        {
            phy_addr_bgn = offset + p_info_tbl[bank].area_top_addr;
            if ((D_FLS_MAX_ADDRESS - phy_addr_bgn) < size)
            {
                /* end address over 0xFFFFFFFF. Do nothing */
            }
            else
            {
                phy_addr_end = phy_addr_bgn + size;
                /* offset check & size check*/
                if (phy_addr_end > p_info_tbl[bank].area_end_addr)
                {
                    /* Range over. Do nothing */
                }
                else
                {
                    /* check OK */
                    *p_addr = phy_addr_bgn;
                    lul_fls_status = ER_OK;
                }
            }
        }
    }

    return (lul_fls_status);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0091]:[ICUMFW_UD_U2B_D_FLS_0091]
* Function name: d_FLS_CheckMemory
* Description  : Check of memory address
* Arguments    : start_addr     : Address to be checked
*                end_addr       : Data size
*                direction      : Direction to check
*                p_result       : Check result (The caller must guarantee not NULL)
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
static uint32_t d_FLS_CheckMemory(const uint32_t start_addr, const uint32_t end_addr, const uint8_t direction,
                                  uint8_t *p_result)
{
    uint32_t err;
    uint32_t size;

    switch ((uint32_t)direction)
    {
        /* in case indicate source address */
        case D_FLS_CHK_MEM_SOURCE:
            if (ADDR_UPPER_SCF_SIDEB >= start_addr)
            /* in case code flash region   */
            {
                if (D_FLS_INVALID == g_icum_rac_double)
                /* in case Single ICUMD region                        */
                /* g_icum_rac_single : Code Flash region Last Address */
                /*    = Secure Flash Start Address                    */
                {
                    if (g_icum_rac_single <= start_addr)
                    {
                        *p_result = D_FLS_CHK_NG;
                    }
                    else if (g_icum_rac_single <= end_addr)
                    {
                        *p_result = D_FLS_CHK_NG;
                    }
                    else
                    {
                        *p_result = D_FLS_CHK_OK;
                    }
                }
                else
                {
                    /* in case Double ICUMD region                   */
                    /*  g_icum_rac_double:Side A region Last address */
                     /*     = Side B Start address                   */
                    /*  g_icum_rac_single:Side B region Last address */
                    /*      = Secure Code Flash region Start address */

                    if (ADDR_LOWER_SCF_SIDEA <= start_addr)
                    /* SIDE B area */
                    {
                        if (g_icum_rac_single <= start_addr)
                        {
                            *p_result = D_FLS_CHK_NG;
                        }
                        else if (g_icum_rac_single <= end_addr)
                        {
                            *p_result = D_FLS_CHK_NG;
                        }
                        else
                        {
                            *p_result = D_FLS_CHK_OK;
                        }
                    }
                    else if (g_icum_rac_double <= start_addr)
                    /* SIDE A area */
                    {
                        *p_result = D_FLS_CHK_NG;
                    }
                    else if (g_icum_rac_double <= end_addr)
                    {
                        *p_result = D_FLS_CHK_NG;
                    }
                    else
                    {
                        *p_result = D_FLS_CHK_OK;
                    }
                }
            }
            else if ((ADDR_LOWER_CRAM <= start_addr) && (ADDR_UPPER_CRAM >= start_addr))
            /* in case Cluster RAM */
            {
                /* Calculate size */
                size = (end_addr - start_addr) + 1uL;
                /* address check using Cluster RAM area check function */
                err = D_COMM_CheckMemorySharedRAM(start_addr, size, ICUM_COMM_DO_CHECK);
                if (ER_OK == err)
                {
                    /* Set p_result to D_FLS_CHK_OK */
                    *p_result = D_FLS_CHK_OK;
                }
                else
                {
                    /* Set p_result to D_FLS_CHK_NG */
                    *p_result = D_FLS_CHK_NG;
                }
            }
            else if (ADDR_LOWER_DF_BANKA <= start_addr)
            /* in case data flash (User) region           */
            /* g_icum_rad:secure Data Flash Start Address */
            {
                if (g_icum_rad <= start_addr)
                {
                    *p_result = D_FLS_CHK_NG;
                }
                else if (g_icum_rad <= end_addr)
                {
                    *p_result = D_FLS_CHK_NG;
                }
                else
                {
                    *p_result = D_FLS_CHK_OK;
                }
            }
            else
            /* in case Illegal address */
            {
                *p_result = D_FLS_CHK_NG;
            }

            err = ER_OK;
            break;

        /* in case indicate destination address */
        case D_FLS_CHK_MEM_DESTINATION:
            if ((ADDR_LOWER_CRAM <= start_addr) && (ADDR_UPPER_CRAM >= start_addr))
            /* in case Cluster RAM */
            {

                 /* Calculate size */
                size = (end_addr - start_addr) + 1uL;
                /* address check using Cluster RAM area check function */
                err = D_COMM_CheckMemorySharedRAM(start_addr, size, ICUM_COMM_DO_CHECK);
                if (ER_OK == err)
                {
                    /* Set p_result to D_FLS_CHK_OK */
                    *p_result = D_FLS_CHK_OK;
                }
                else
                {
                    /* Set p_result to D_FLS_CHK_NG */
                    *p_result = D_FLS_CHK_NG;
                }
            }
            else
            {
                *p_result = D_FLS_CHK_NG;
            }

            err = ER_OK;
            break;

        default:
            /* Does not occur after the UT/IT process */
            *p_result = D_FLS_CHK_NG;
            err = ER_PARAM;
            break;
    } /* end of switch ((uint32_t)direction) */

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0084]:[ICUMFW_UD_U2B_D_FLS_0084]
* Function name: D_FLS_CheckMemory
* Description  : Check of memory address
* Arguments    : start_addr     : Address to be checked
*                size           : Data size
*                direction      : Direction to check
*                p_result       : Check result
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_FLS_CheckMemory(const uint32_t start_addr, const uint32_t size, const uint8_t direction, uint8_t *p_result)
{
    uint32_t err;
    uint32_t end_addr;

    if (NULL == p_result)
    {
        err = ER_PARAM;
    }
    else
    {
        /* over address check */
        if ((D_FLS_MAX_ADDRESS - start_addr) < size)
        {
            /* end address over 0xFFFFFFFF */
            *p_result = D_FLS_CHK_NG;
            err = ER_OK;
        }
        else
        {
            end_addr = start_addr + size;
            if (D_FLS_ZERO_U32 < size)
            {
                end_addr--;
            }
            else
            {
                /* Do Nothing */
            }
            err = d_FLS_CheckMemory(start_addr, end_addr, direction, p_result);
        }
    }

    return (err);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_driver.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0093]:[ICUMFW_UD_U2B_D_FLS_0093]
* Function name: d_FLS_GetDFBankNumber
* Description  : Check of memory source address for data flash bank
* Arguments    : df_addr        : Target DF Address
*                p_bank         : Target bank number (The caller must guarantee not NULL)
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
static uint32_t d_FLS_GetDFBankNumber(const uint32_t df_addr, uint32_t *p_bank)
{
    uint32_t i;
    uint32_t err;

    err = ER_PARAM;
    *p_bank = D_FLS_ZERO_U32;

    for (i = 0uL; i < (uint32_t)FLS_DF_BANK_NUM; i++)
    {
        if ((g_data_flash_area_list[i].area_top_addr <= df_addr)
            && (g_data_flash_area_list[i].area_end_addr > df_addr))
        {
            *p_bank = i;
            err = ER_OK;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0097]:[ICUMFW_UD_U2B_D_FLS_0097]
* Function name: D_FLS_CheckMemoryUserBoot
* Description  : Check of memory address
* Arguments    : start_addr     : Address to be checked
*                size           : Data size
*                direction      : Direction to check
*                p_result       : Check result
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_FLS_CheckMemoryUserBoot(const uint32_t start_addr, const uint32_t size, const uint8_t direction,
                                   uint8_t *p_result)
{
    uint32_t err;
    uint32_t end_addr;

    if (NULL == p_result)
    {
        err = ER_PARAM;
    }
    else
    {
        /* over address check */
        if ((D_FLS_MAX_ADDRESS - start_addr) < size)
        {
            /* end address over 0xFFFFFFFF */
            *p_result = D_FLS_CHK_NG;
            err = ER_OK;
        }
        else
        {
            end_addr = start_addr + size;
            if (D_FLS_ZERO_U32 < size)
            {
                end_addr--;
            }
            else
            {
                /* Do Nothing */
            }

            if (((uint32_t)D_FLS_CHK_MEM_SOURCE == (uint32_t)direction)
               && (((ADDR_LOWER_USER_BOOT0 <= start_addr) && (ADDR_UPPER_USER_BOOT0 >= end_addr)) 
               ||  ((ADDR_LOWER_USER_BOOT1 <= start_addr) && (ADDR_UPPER_USER_BOOT1 >= end_addr))))
            {
                *p_result = D_FLS_CHK_OK;
                err = ER_OK;
            }
            else
            {
                err = d_FLS_CheckMemory(start_addr, end_addr, direction, p_result);
            }
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0098]:[ICUMFW_UD_U2B_D_FLS_0098]
* Function name: D_FLS_CheckMemoryCISMUserBoot
* Description  : Check of memory address
* Arguments    : start_addr     : Address to be checked
*                size           : Data size
*                direction      : Direction to check
*                p_result       : Check result
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_FLS_CheckMemoryCISMUserBoot(const uint32_t start_addr, const uint32_t size, const uint8_t direction,
                                       uint8_t *p_result)
{
    uint32_t err;
    uint32_t end_addr;

    if (NULL == p_result)
    {
        err = ER_PARAM;
    }
    else
    {
        /* over address check */
        if ((D_FLS_MAX_ADDRESS - start_addr) < size)
        {
            /* end address over 0xFFFFFFFF */
            *p_result = D_FLS_CHK_NG;
            err = ER_OK;
        }
        else
        {
            end_addr = start_addr + size;
            if (D_FLS_ZERO_U32 < size)
            {
                end_addr--;
            }
            else
            {
                /* Do Nothing */
            }

            if ((ADDR_LOWER_CISM_BSS <= start_addr) && (ADDR_UPPER_CISM_BSS >= end_addr))
            {
                *p_result = D_FLS_CHK_OK;
                err = ER_OK;
            }
            else if (((uint32_t)D_FLS_CHK_MEM_SOURCE == (uint32_t)direction)
               && (((ADDR_LOWER_USER_BOOT0 <= start_addr) && (ADDR_UPPER_USER_BOOT0 >= end_addr)) 
               ||  ((ADDR_LOWER_USER_BOOT1 <= start_addr) && (ADDR_UPPER_USER_BOOT1 >= end_addr))))
            {
                *p_result = D_FLS_CHK_OK;
                err = ER_OK;
            }
            else
            {
                err = d_FLS_CheckMemory(start_addr, end_addr, direction, p_result);
            }
        }
    }

    return (err);
}

/* self copy icum_driver_rom.text to icum_driver_ram   */
CHANGE_SEC(text, ".icum_driver_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0092]:[ICUMFW_UD_U2B_D_FLS_0092]
* Function name: d_FLS_GetCFBankNumber
* Description  : Check of memory source address for code flash bank
* Arguments    : cf_addr        : Target CF Address
*                p_bank         : Target bank number (The caller must guarantee not NULL)
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t d_FLS_GetCFBankNumber(const uint32_t cf_addr, uint32_t *p_bank)
{
    uint32_t i;
    uint32_t err;

    err = ER_PARAM;
    *p_bank = D_FLS_ZERO_U32;

    for (i = 0uL; i < D_FLS_CF_BANK_NUM; i++)
    {
        if ((g_code_flash_area_list[i].area_top_addr <= cf_addr)
            && (g_code_flash_area_list[i].area_end_addr > cf_addr))
        {
            *p_bank = i;
            err = ER_OK;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0089]:[ICUMFW_UD_U2B_D_FLS_0089]
* Function name: D_FLS_CheckSCFBoundary
* Description  : Check of memory source address for code flash bank
* Arguments    : cf_addr        : Target CF Address
*                size           : Target size
*                p_bank         : Target bank number (The caller must guarantee not NULL)
*                p_offset       : Offset from the base address (The caller must guarantee not NULL)
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_FLS_CheckSCFBoundary(const uint32_t cf_addr, const uint32_t size, uint32_t *p_bank, uint32_t *p_offset)
{
    uint32_t err;
    uint32_t bank;
    uint32_t lower_addr;
    uint32_t upper_addr;
    uint32_t base_addr;
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t lower_fw_addr;
    uint32_t upper_fw_addr;

    if ((D_FLS_MAX_ADDRESS - cf_addr) < size)
    {
        err = ER_PARAM;
    }
    else
    {
        if ((g_icum_lower_limit_cf_side_b <= cf_addr) && (cf_addr <= g_icum_upper_limit_cf_side_b))
        {
            lower_addr = g_icum_lower_limit_cf_side_b;
            upper_addr = g_icum_upper_limit_cf_side_b;
        }
        else
        {
            lower_addr = g_icum_lower_limit_cf_side_a;
            upper_addr = g_icum_upper_limit_cf_side_a;
        }

        if ((ADDR_LOWER_FIRMWARE_B <= cf_addr) && (cf_addr <= ADDR_UPPER_FIRMWARE_B))
        {
            lower_fw_addr = ADDR_LOWER_FIRMWARE_B;
            upper_fw_addr = ADDR_UPPER_FIRMWARE_B;
        }
        else
        {
            lower_fw_addr = ADDR_LOWER_FIRMWARE_A;
            upper_fw_addr = ADDR_UPPER_FIRMWARE_A;
        }

        start_addr = cf_addr;
        end_addr   = start_addr + size;
        if (D_FLS_ZERO_U32 < size)
        {
            end_addr--;
        }
        else
        {
            /* Do nothing */
        }

        if (((lower_addr <= start_addr) && (upper_addr >= end_addr))
            || ((lower_fw_addr <= start_addr) && (upper_fw_addr >= end_addr)))
        {
            err = d_FLS_GetCFBankNumber(cf_addr, &bank);
            if (ER_OK != err)
            {
                /* Do nothing */
            }
            else
            {
                base_addr = g_code_flash_area_list[bank].area_top_addr;
                *p_bank = bank;
                if (start_addr < base_addr)
                {
                    err = ER_PARAM;
                }
                else
                {
                    *p_offset = (uint32_t)(start_addr - base_addr); 
                }
            }
        }
        else
        {
            err = ER_PARAM;
        }
    } /* end of else */

    return err;
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_driver.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0090]:[ICUMFW_UD_U2B_D_FLS_0090]
* Function name: D_FLS_GetOPBTAddress
* Description  : Get the address according to the target_info
* Arguments    : target_info            : target info
*                p_addr                 : the OPBT address (The caller must guarantee not NULL)
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_FLS_GetOPBTAddress(const uint32_t target_info, uint32_t *p_addr)
{
    uint32_t lul_fls_status;

    lul_fls_status = ER_OK;
    /* target_info */
    switch (target_info)
    {
        case AG_D_FLS_ICUMUPGP:
            *p_addr = D_FLS_ADR_ICUMUPGP;
            break;
        case AG_D_FLS_ICUMRESV:
            *p_addr = D_FLS_ADR_ICUMRESV;
            break;
        case AG_D_FLS_ICUMRACS0:
            *p_addr = D_FLS_ADR_ICUMRACS0;
            break;
        case AG_D_FLS_ICUMRAD:
            *p_addr = D_FLS_ADR_ICUMRAD;
            break;
        case AG_D_FLS_CHGRESP:
            *p_addr = D_FLS_ADR_CHGRESP;
            break;
        case AG_D_FLS_ICUMRACE0:
            *p_addr = D_FLS_ADR_ICUMRACE0;
            break;
        case AG_D_FLS_ICUMRACS1:
            *p_addr = D_FLS_ADR_ICUMRACS1;
            break;
        case AG_D_FLS_ICUMRACE1:
            *p_addr = D_FLS_ADR_ICUMRACE1;
            break;
        case AG_D_FLS_DEGRADATION:
            *p_addr = D_FLS_ADR_DEGRADATION;
            break;
        default:
            /* parameter error   */
            lul_fls_status = ER_PARAM;
            break;
    } /* end of switch (target_info) */

    return lul_fls_status;
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_driver.text")
CHANGE_SEC(rodata, ".icum_driver.rodata")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0086]:[ICUMFW_UD_U2B_D_FLS_0086]
* Function name: D_FLS_CheckMemoryCISM
* Description  : Check of memory source address for CISM
* Arguments    : start_addr     : address to be checked
*                size           : data size
*                direction      : direction to check
*                p_result       : check result
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_FLS_CheckMemoryCISM(const uint32_t start_addr, const uint32_t size, const uint8_t direction,
                               uint8_t *p_result)
{
    uint32_t err;
    uint32_t end_addr;

    if (NULL == p_result)
    {
        err = ER_PARAM;
    }
    else
    {
        /* over address check */
        if ((D_FLS_MAX_ADDRESS - start_addr) < size)
        {
            /* end address over 0xFFFFFFFF */
            *p_result = D_FLS_CHK_NG;
            err = ER_OK;
        }
        else
        {
            end_addr = start_addr + size;
            if (D_FLS_ZERO_U32 < size)
            {
                end_addr--;
            }
            else
            {
                /* Do Nothing */
            }

            if ((ADDR_LOWER_CISM_BSS <= start_addr) && (ADDR_UPPER_CISM_BSS >= end_addr))
            {
                *p_result = D_FLS_CHK_OK;
                err = ER_OK;
            }
            else
            {
                err = d_FLS_CheckMemory(start_addr, end_addr, direction, p_result);
            }
        }
    }

    return (err);
}

/* self copy icum_driver_rom.text to icum_driver_ram.text   */
CHANGE_SEC(text, ".icum_driver_ram")
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0094]:[ICUMFW_UD_U2B_D_FLS_0094]
* Function name: D_FLS_CheckUserCFBoundary
* Description  : Check of memory source address for code flash bank
* Arguments    : addr           : address to be checked
*                size           : size to be checked
*                p_bank         : pointer to store BANK ID
*                p_offset       : pointer to store offset address
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_FLS_CheckUserCFBoundary(const uint32_t addr, const uint32_t size, uint32_t *p_bank, uint32_t *p_offset)
{
    uint32_t err;
    uint8_t ret_val;
    uint32_t lower_addr;
    uint32_t upper_addr;
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t wk32;

    if ((D_FLS_MAX_ADDRESS - addr) < size)
    {
        err = ER_PARAM;
    }
    else
    {
        err = d_FLS_GetCFBankNumber(addr, &wk32);
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            /* Check if Secure Code Flash */
            d_FLS_CheckSCFAddress(((addr + size) - 1UL), 1UL, &ret_val);
            if ((uint32_t)D_FLS_CHK_OK == (uint32_t)ret_val)
            {
                err = ER_PARAM;
            }
            else
            {
                /* Check if Secure Code Flash */
                d_FLS_CheckSCFAddress(addr, size, &ret_val);
                if ((uint32_t)D_FLS_CHK_OK == (uint32_t)ret_val)
                {
                    err = ER_PARAM;
                }
                else
                {
                    err = ER_OK;
                }
            }

            if (ER_OK == err)
            {
                lower_addr = g_code_flash_area_list[wk32].area_top_addr;
                upper_addr = g_code_flash_area_list[wk32].area_end_addr;

                start_addr = addr;
                end_addr   = (start_addr + size) - 1UL;

                if ((lower_addr <= start_addr) && (upper_addr >= end_addr))
                {
                    *p_bank = wk32;
                    *p_offset = (uint32_t)(addr - lower_addr);
                    err = ER_OK;
                }
                else
                {
                    err = ER_PARAM;
                }
            }
        } /* end of else */
    } /* end of else */

    return err;
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_driver.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0095]:[ICUMFW_UD_U2B_D_FLS_0095]
* Function name: D_FLS_CheckUserDFBoundary
* Description  : Check of memory source address for data flash bank
* Arguments    : addr           : address to be checked
*                size           : size to be checked
*                p_bank         : pointer to store BANK ID
*                p_offset       : pointer to store offset address
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_FLS_CheckUserDFBoundary(const uint32_t addr, const uint32_t size, uint32_t *p_bank, uint32_t *p_offset)
{
    uint32_t err;
    uint8_t ret_val;
    uint32_t lower_addr;
    uint32_t upper_addr;
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t wk32;

    if ((D_FLS_MAX_ADDRESS - addr) < size)
    {
        err = ER_PARAM;
    }
    else
    {
        err = d_FLS_GetDFBankNumber(addr, &wk32);
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            /* Check if Secure Data Flash */
            d_FLS_CheckSDFAddress(((addr + size) - 1UL), 1UL, &ret_val);
            if ((uint32_t)D_FLS_CHK_OK == (uint32_t)ret_val)
            {
                err = ER_PARAM;
            }
            else
            {
                d_FLS_CheckSDFAddress(addr, size, &ret_val);
                if ((uint32_t)D_FLS_CHK_OK == (uint32_t)ret_val)
                {
                    err = ER_PARAM;
                }
                else
                {
                    err = ER_OK;
                }
            }

            if (ER_OK == err)
            {
                lower_addr = g_data_flash_area_list[wk32].area_top_addr;
                upper_addr = g_data_flash_area_list[wk32].area_end_addr;

                start_addr = addr;
                end_addr   = (start_addr + size) - 1UL;

                if ((lower_addr <= start_addr) && (upper_addr >= end_addr))
                {
                    *p_bank = wk32;
                    *p_offset = (uint32_t)(addr - lower_addr);
                    err = ER_OK;
                }
                else
                {
                    err = ER_PARAM;
                }
            }
        } /* end of else */
    } /* end of else */

    return err;
}

/* self copy icum_driver_rom.text to icum_driver_ram   */
CHANGE_SEC(text, ".icum_driver_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/****************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0096]:[ICUMFW_UD_U2B_D_FLS_0096]
* Function name: D_FLS_CheckRamExecRequired
* Description  : 
* Arguments    : uint32_t target_area   : target flash area (FLS_SELECT_CF or FLS_SELECT_DF)
*                uint32_t addr          : target address
*                uint8_t *p_result      : RAM execution required = true, unnecessary = false
* Return Value : ER_OK
*                ER_PARAM
*****************************************************************************************/
uint32_t D_FLS_CheckRamExecRequired(const uint32_t target_area, const uint32_t addr, uint8_t *p_result)
{
    uint32_t err;
    uint32_t req_bank;
    uint32_t firmware_bank;

    *p_result = (uint8_t)false;

    err = d_FLS_GetCFBankNumber(addr, &req_bank);
    if (ER_OK == err)
    {
        err = d_FLS_GetCFBankNumber(D_FLS_ICUM_FIRMWARE_START_ADDRESS, &firmware_bank);
        if (ER_OK == err)
        {
            if ((AG_D_FLS_CODE == target_area) && (firmware_bank == req_bank))
            {
                *p_result = (uint8_t)true;
            }
            else
            {
                *p_result = (uint8_t)false;
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

    return (err);
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

