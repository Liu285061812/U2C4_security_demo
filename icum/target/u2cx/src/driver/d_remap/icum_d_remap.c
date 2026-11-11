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
/**************************************************************************************/
/* Header files                                                                       */
/**************************************************************************************/
#include "renesas_types.h"
#include ICUM_DEVICE_FILE
#include "icum_common_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_remap_pub.h"
#include "icum_d_remap.h"

/**************************************************************************************/
/* Global Variable                                                                    */
/**************************************************************************************/
/* Remap Control Table */
#define REMAP_TBL_MAX    (sizeof(s_remap_tbl)/sizeof(s_remap_tbl[0]))
#define USER_SET_MAX     (4U)
#define NUM2             (2u)
#define NUM4             (4u)

static ICUM_REMAP_REMAP_TABLE_t s_remap_tbl[] = {
    [0] =  { REGST_NOREG, COUNT_ZERO, COUNT_ZERO, SID_4MB, PHYS_ADRS00_BASE, REMAP_ADRS00_BASE, ICUMX_SICREMAP2M0_REG },
    [1] =  { REGST_INH,   COUNT_ZERO, COUNT_ZERO, SID_NOA, PHYS_ADRS01_BASE, REMAP_ADRS01_BASE, ICUMX_SICREMAP2M1_REG },
    [2] =  { REGST_NOREG, COUNT_ZERO, COUNT_ZERO, SID_4MB, PHYS_ADRS02_BASE, REMAP_ADRS02_BASE, ICUMX_SICREMAP2M2_REG },
    [3] =  { REGST_INH,   COUNT_ZERO, COUNT_ZERO, SID_NOA, PHYS_ADRS03_BASE, REMAP_ADRS03_BASE, ICUMX_SICREMAP2M3_REG },
    [4] =  { REGST_NOREG, COUNT_ZERO, COUNT_ZERO, SID_4MB, PHYS_ADRS04_BASE, REMAP_ADRS04_BASE, ICUMX_SICREMAP2M4_REG },
    [5] =  { REGST_INH,   COUNT_ZERO, COUNT_ZERO, SID_NOA, PHYS_ADRS05_BASE, REMAP_ADRS05_BASE, ICUMX_SICREMAP2M5_REG },
    [6] =  { REGST_NOREG, COUNT_ZERO, COUNT_ZERO, SID_4MB, PHYS_ADRS06_BASE, REMAP_ADRS06_BASE, ICUMX_SICREMAP2M6_REG },
    [7] =  { REGST_INH,   COUNT_ZERO, COUNT_ZERO, SID_NOA, PHYS_ADRS07_BASE, REMAP_ADRS07_BASE, ICUMX_SICREMAP2M7_REG },
    [8] =  { REGST_NOREG, COUNT_ZERO, COUNT_ZERO, SID_4MB, PHYS_ADRS08_BASE, REMAP_ADRS08_BASE, ICUMX_SICREMAP2M8_REG },
    [9] =  { REGST_INH,   COUNT_ZERO, COUNT_ZERO, SID_NOA, PHYS_ADRS09_BASE, REMAP_ADRS09_BASE, ICUMX_SICREMAP2M9_REG },
    [10] = { REGST_NOREG, COUNT_ZERO, COUNT_ZERO, SID_4MB, PHYS_ADRS10_BASE, REMAP_ADRS10_BASE, ICUMX_SICREMAP2M10_REG },
    [11] = { REGST_INH,   COUNT_ZERO, COUNT_ZERO, SID_NOA, PHYS_ADRS11_BASE, REMAP_ADRS11_BASE, ICUMX_SICREMAP2M11_REG },
    [12] = { REGST_FIXED, COUNT_ZERO, COUNT_ZERO, SID_2MB, PHYS_ADRS12_BASE, REMAP_ADRS12_BASE, ICUMX_SICREMAP2M12_REG },
    [13] = { REGST_FIXED, COUNT_ZERO, COUNT_ZERO, SID_2MB, PHYS_ADRS13_BASE, REMAP_ADRS13_BASE, ICUMX_SICREMAP2M13_REG },
    [14] = { REGST_FIXED, COUNT_ZERO, COUNT_ZERO, SID_2MB, PHYS_ADRS14_BASE, REMAP_ADRS14_BASE, ICUMX_SICREMAP2M14_REG },
    [15] = { REGST_FIXED, COUNT_ZERO, COUNT_ZERO, SID_2MB, PHYS_ADRS15_BASE, REMAP_ADRS15_BASE, ICUMX_SICREMAP2M15_REG }
};

/* LRU Calculating Table */
static ICUM_RMAP_LRU_TABLE_t s_lru_tbl[] = {
    [0] =  { COUNT_ZERO },
    [1] =  { COUNT_ZERO },
    [2] =  { COUNT_ZERO },
    [3] =  { COUNT_ZERO },
    [4] =  { COUNT_ZERO },
    [5] =  { COUNT_ZERO },
    [6] =  { COUNT_ZERO },
    [7] =  { COUNT_ZERO },
    [8] =  { COUNT_ZERO },
    [9] =  { COUNT_ZERO },
    [10] = { COUNT_ZERO },
    [11] = { COUNT_ZERO },
    [12] = { COUNT_ZERO },
    [13] = { COUNT_ZERO },
    [14] = { COUNT_ZERO },
    [15] = { COUNT_ZERO }
};

#if !defined(TARGET_DEVICE_S4H) && !defined(TARGET_DEVICE_U2B) && !defined(TARGET_DEVICE_U2C) && !defined(TARGET_DEVICE_U2C4)
/* User setting Table */
static ICUM_REMAP_USERSET_TABLE_t s_user_set_tbl[USER_SET_MAX];
#endif

/*  */
static uint32_t    s_reg_sts_flag = REG_STS_FULREGIST;
static uint32_t    s_reg_ptr      = REG_STS_INIT_STS;
static uint32_t    s_no_use_flag  = REG_STS_INIT_STS;
static uint32_t    s_lru_ptr      = REG_STS_INIT_STS;

/* Remap Control Data's */
/* Initialize Status */
static uint32_t s_init_sts = REMAP_NOTINIT;

/* The index number that the area is registered by user */
static uint32_t s_user_set_no = REG_STS_INIT_STS;

PRAGMA(ghs section text = ".icum_driver.text")
PRAGMA(ghs section rodata = ".icum_driver.rodata")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_REMAP_0001]:[ICUMFW_UD_S4_D_REMAP_0001]
* Function name: D_REMAP_Init
* Description  : Init processing.
* Arguments    : none
* Return Value : ER_OK
*                ER_D_REMAP_RETRYOVER : Retry Over at writing Remap Register
*                ER_D_REMAP_ILLEGALADR : The user area is not REMAP area
***************************************************************************************/
uint32_t D_REMAP_Init(void)
{
    uint32_t    uiRet = ER_OK;
#if !defined(TARGET_DEVICE_S4H) && !defined(TARGET_DEVICE_U2B) && !defined(TARGET_DEVICE_U2C) && !defined(TARGET_DEVICE_U2C4)
    uint32_t    counter;
    uint32_t    compare_ptr;
    uint32_t    sort_ptr;
    uint32_t    uiRemap_temporary[USER_SET_MAX];
    uint32_t    uiRemap_base_addr[USER_SET_MAX];
#else   
    uint32_t    reg;
#endif

    /* 1. Set Remap operation(fixed area) */
    uiRet = d_REMAP_SetRemapFixedTbl();
#if !defined(TARGET_DEVICE_S4H) && !defined(TARGET_DEVICE_U2B) && !defined(TARGET_DEVICE_U2C) && !defined(TARGET_DEVICE_U2C4)
    if (ER_OK == uiRet)
    {
        /* 2. Initialize the setting table */
        for (counter = COUNT_ZERO; counter < USER_SET_MAX; counter++)
        {
            uiRemap_temporary[counter] = SIZE_ZERO;
            uiRemap_base_addr[counter] = SIZE_ZERO;
        }
        s_user_set_tbl[0].addr = SERVICEQUEUE_1_ADDR;
        s_user_set_tbl[0].size = SERVICEQUEUE_1_SIZE;
        s_user_set_tbl[1].addr = SHAREDMEM_1_ADDR;
        s_user_set_tbl[1].size = SHAREDMEM_1_SIZE;
        s_user_set_tbl[2].addr = SHAREDMEM_2_ADDR;
        s_user_set_tbl[2].size = SHAREDMEM_2_SIZE;
        s_user_set_tbl[3].addr = SHAREDMEM_3_ADDR;
        s_user_set_tbl[3].size = SHAREDMEM_3_SIZE;

        /* 3. Check the area that is specified by user */
        for (counter = COUNT_ZERO; counter < USER_SET_MAX; counter++)
        {
            /* 3-1. Check whether the size is 0 */
            if (SIZE_ZERO != s_user_set_tbl[counter].size)
            {
                /* 3-2. Check whether the area in the RT-SRAM */
                /* If check area is on the RT-SRAM judgement is false, else is true */
                if (!((PHYS_ADRS15_BASE <= s_user_set_tbl[counter].addr)
                    && (s_user_set_tbl[counter].addr < (PHYS_ADRS15_BASE + BIAS_2MB))))
                {
                    /* 3-3. Check whether the area in the SDRAM or SystemRAM or VRAM */
                    if (((AREA_SDRAM_START <= s_user_set_tbl[counter].addr) && (s_user_set_tbl[counter].addr <= AREA_SDRAM_END)) ||
                        ((AREA_SYSTEMRAM_START <= s_user_set_tbl[counter].addr) && (s_user_set_tbl[counter].addr <= AREA_SYSTEMRAM_END)) ||
                        ((AREA_VRAM_START <= s_user_set_tbl[counter].addr) && (s_user_set_tbl[counter].addr <= AREA_VRAM_END)))
                    {
                        /* 3-4. Set the physical address to temporary array */
                        uiRemap_temporary[counter] = s_user_set_tbl[counter].addr & SEP_BASE_ADRS;
                    }
                    else
                    {
                        uiRet = ER_D_REMAP_ILLEGALADR;
                        break;
                    }
                }
                else
                {
                    /* No Operation */
                }
            }
            else
            {
                /* No Operation */
            }
        }
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK == uiRet)
    {
        /* 4. Sort uiRemap_temporary[] in ascending order */
        for (counter = COUNT_ZERO; counter < USER_SET_MAX; counter++)
        {
            /* 4-1. If uiRemap_temporary[] is 0, then skip */
            if (VALUE_ZERO != uiRemap_temporary[counter])
            {
                for (compare_ptr = COUNT_ZERO; compare_ptr < USER_SET_MAX; compare_ptr++)
                {
                    /* 4-2. If uiRemap_base_addr[] is 0, then set uiRemap_temporary[] to uiRemap_base_addr[] */
                    if (VALUE_ZERO == uiRemap_base_addr[compare_ptr])
                    {
                        /* 4-2-1. If uiRemap_base_addr[] is 0, then set uiRemap_temporary[] to uiRemap_base_addr[] */
                        uiRemap_base_addr[compare_ptr] = uiRemap_temporary[counter];
                        compare_ptr = USER_SET_MAX;
                    }
                    /* 4-3. If uiRemap_base_addr[] equals uiRemap_Temporary[], then skip */
                    else if (uiRemap_base_addr[compare_ptr] == uiRemap_temporary[counter])
                    {
                        compare_ptr = USER_SET_MAX;
                    }
                    /* 4-4. If uiRemap_base_addr[] is grader than uiRemap_temporary[], then sort uiRemap_base_addr[] */
                    else if (uiRemap_base_addr[compare_ptr] > uiRemap_temporary[counter])
                    {
                        for (sort_ptr = (USER_SET_MAX - 1U); sort_ptr > compare_ptr; sort_ptr--)
                        {
                            /* 4-4-1. Sort uiRemap_base_addr[] */
                            uiRemap_base_addr[sort_ptr] = uiRemap_base_addr[sort_ptr-1U];
                        }
                        uiRemap_base_addr[compare_ptr] = uiRemap_temporary[counter];
                        compare_ptr = USER_SET_MAX;
                    }
                    else
                    {
                        /* No Operation */
                    }
                }
            }
            else
            {
                /* No Operation */
            }
        }
    }
    else
    {
        /* No Operation */
    }

    if (ER_OK == uiRet)
    {
        /* 5. Remap operation */
        for (counter = COUNT_ZERO; ((counter < USER_SET_MAX) && (ER_OK == uiRet)); counter++)
        {
            if (VALUE_ZERO != uiRemap_base_addr[counter])
            {
                uiRet = d_REMAP_SetRemapTbl(uiRemap_base_addr[counter]);
            }
            else
            {
                counter = USER_SET_MAX;
            }
        }
    }
    else
    {
        /* Do nothing */
    }
#else /* !defined(TARGET_DEVICE_S4H) */
    reg = (*(volatile uint32_t *)LOGI_ADDR_STBY_CTRL);
    reg |= STBY_CTRL_OPBT_RDY;
    *(volatile uint32_t *)LOGI_ADDR_STBY_CTRL = reg;
#endif /* !defined(TARGET_DEVICE_S4H) */

    if (ER_OK == uiRet)
    {
        /* 6. Set Completion Initialize Status */
        s_init_sts = REMAP_INIT;
    }
    else
    {
        /* Do nothing */
    }

    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_REMAP_0005]:[ICUMFW_UD_S4_D_REMAP_0005]
* Function name: D_REMAP_Register
* Description  : Register Remap Area.
* Arguments    : (IN)phys_addr         Physical Address
*                (OUT)*p_logical_addr  Logical Address
* Return Value : ER_OK
*                ER_PARAM             : Pointer is NULL
*                ER_D_REMAP_STATE     : Not Initialize
*                ER_D_REMAP_NOTASIGN  : Not Empty for Remap Table
*                ER_D_REMAP_RETRYOVER : Retry Over at writing Remap Register
*                ER_D_REMAP_ILLEGALADR: Illegal Address
***************************************************************************************/
uint32_t D_REMAP_Register(uint32_t phys_addr, uint32_t *p_logical_addr)
{
    uint32_t    uiRet = ER_OK;
    uint32_t    phys_base;
    uint32_t    offset_adrs;
    uint32_t    ret_sts;

    /* 1. Check pointer */
    if (NULL == p_logical_addr)
    {
        uiRet = ER_PARAM;
    }
    /* 2. Check Initialize status */
    else if (REMAP_NOTINIT == s_init_sts)
    {
        uiRet = ER_D_REMAP_STATE;
    }
    else
    {
        /* 3. Generate Physical Base Address and Offset Address */
        phys_base = phys_addr & SEP_BASE_ADRS;
        offset_adrs = phys_addr & SEP_OFF_ADRS;

        if (PHYS_ADRS15_BASE == phys_base)
        {
            /* 4. Case of RT-SRAM Remap Area */
            *p_logical_addr = offset_adrs + REMAP_ADRS15_BASE;
        }
        else if (PHYS_ADRS12_BASE  == phys_base)
        {
            /* 5. Case of  MFIS Remap Area */
            *p_logical_addr = offset_adrs + REMAP_ADRS12_BASE;
        }
        else if (PHYS_ADRS13_BASE  == phys_base)
        {
            /* 6. Case of  RPC (Flash) Remap Area */
            *p_logical_addr = offset_adrs + REMAP_ADRS13_BASE;
        }
        else if (PHYS_ADRS14_BASE  == phys_base)
        {
            /* 7. Case of  Secure Engine Remap Area */
            *p_logical_addr = offset_adrs + REMAP_ADRS14_BASE;
        }
        else
        {
            /* 8. Case of Not Fixed Area, Check Specified Address Area */
            ret_sts = D_MISC_CheckArea(phys_addr);
            if (AREA_REMAP != ret_sts)
            {
                /* If Specified Address is not in Remap Area, Return with PARAMETER ERROR */
                uiRet = ER_D_REMAP_ILLEGALADR;
            }
            else
            {
                uiRet = d_REMAP_DecideRegPosition(p_logical_addr, phys_base, offset_adrs);
            }
        }
    }

    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_REMAP_0007]:[ICUMFW_UD_S4_D_REMAP_0007]
* Function name: D_REMAP_Unregister
* Description  : Set not use Remap Area.
* Arguments    : (IN)logical_addr  Logical Address
* Return Value : ER_OK
*                ER_PARAM             : Logical Address is illegal
*                ER_D_REMAP_STATE     : Not Initialize
*                ER_D_REMAP_NOREGIST  : Not Registered Physical Address
*                ER_D_REMAP_REGISTERR : Not Used Specified Remap Address
*                ER_D_REMAP_ILLEGALADR: Illegal Address
***************************************************************************************/
uint32_t D_REMAP_Unregister(uint32_t logical_addr)
{
    uint32_t    uiRet = ER_OK;
    uint32_t    saved_psw;
    uint32_t    remap_base;
    uint32_t    index_no;
    uint32_t    ret_sts;
    uint32_t    find_flg = REMAP_NOTFIND;
    uint32_t    find_no = COUNT_ZERO;

    /* 1. Check Initialize status */
    if (REMAP_NOTINIT == s_init_sts)
    {
        uiRet = ER_D_REMAP_STATE;
    }
    else
    {
        /* 2. Generate Remap Access Window Base Address */
        remap_base = logical_addr & SEP_BASE_ADRS;

        if ((REMAP_ADRS15_BASE == remap_base) || (REMAP_ADRS12_BASE  == remap_base) ||
            (REMAP_ADRS13_BASE == remap_base) || (REMAP_ADRS14_BASE  == remap_base))
        {
            /* 3. Case of Fixed Area (RT-SRAM, MFIS, RPC, Secure Engine Remap Access Window Area */
            /* Normal End */
        }
        else
        {
            /* 4. Case of Not Fixed Area, Check Specified Address Area */
            ret_sts = D_MISC_CheckArea(logical_addr);
            if (AREA_SICWINDOW != ret_sts)
            {
                /* If Specified Address is not in SIC Remap Access Window Area, Return with PARAMETER ERROR */
                uiRet = ER_D_REMAP_ILLEGALADR;
            }
            else
            {
                /* 5. Disable interrupts (save ID bit) */
                DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

                /* 6. Search position same as Remap Access Window Address */
                for (index_no = COUNT_ZERO; (REMAP_REG_MAX - NUM4) > index_no; index_no += NUM2)
                {
                    if (remap_base  == s_remap_tbl[index_no].remap_address)
                    {
                        /* Find same address, expire */
                        find_flg = REMAP_FIND;
                        find_no = index_no;
                        break;
                    }
                    else
                    {
                        /* No Operation */
                    }
                }

                /* 6-1. Not Found same Remap Access Window Address, Parameter Error */
                if (REMAP_NOTFIND == find_flg)
                {
                    uiRet = ER_PARAM;
                    /* re-enable interrupts (Enable Int when saved ID bit is enable) */
                }
                /* 7. When Physical Base Address is NULL(Not Registered), Not Registered Error */
                else if (PHYS_ADR_NOASGN == s_remap_tbl[find_no].phys_address)
                {
                    uiRet = ER_D_REMAP_NOREGIST;
                }
                else
                {
                    /* 7-1. When Register Status is "Not Used" or Register Counter is 0, Register Error */
                    if ((REGST_NOUSE == (uint32_t)s_remap_tbl[find_no].reg_flag) ||
                        (COUNT_ZERO == (uint32_t)s_remap_tbl[find_no].reg_count))
                    {
                        uiRet = ER_D_REMAP_REGISTERR;
                    }
                    else
                    {
                        /* 7-2. Check whether the area is registered by user. */
                        if (s_user_set_no <= find_no)
                        {
                            /* 7-3. Decrement Register Counter */
                            s_remap_tbl[find_no].reg_count--;
                            /* If Register Counter is 0, Set "Not Use" to Register Status  */
                            if (COUNT_ZERO == (uint32_t)s_remap_tbl[find_no].reg_count)
                            {
                                s_remap_tbl[find_no].reg_flag = REGST_NOUSE;
                            }
                            else
                            {
                                /* Case used by other registrations */
                            }
                        }
                        else
                        {
                            /* The case where an area is registered by user */
                        }
                    }
                }

                /* 8. re-enable interrupts (Enable Int when saved ID bit is enable) */
                ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
            }
        }
    }

    return (uiRet);
}

PRAGMA(ghs section text = default)
PRAGMA(ghs section rodata = default)

PRAGMA(ghs section text = ".icum_driver_ram")
PRAGMA(ghs section rodata = ".icum_driver_const_ram")
#if defined(FSW_REMAP_DRIVER)
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_REMAP_0010]:[ICUMFW_UD_S4_D_REMAP_0010]
* Function name: D_REMAP_CheckLogicalAddrs
* Description  : Get Logical Address
* Arguments    : (IN)phys_addr         Physical Address
* Return Value : ER_OK
*                ER_D_REMAP_STATE     : Not Initialize
*                ER_D_REMAP_NOREGIST  : Not Registered Physical Address
*                ER_D_REMAP_ILLEGALADR: Illegal Address
***************************************************************************************/
uint32_t  D_REMAP_CheckLogicalAddrs(uint32_t phys_addr)
{
    uint32_t    uiRet;
    uint32_t    phys_base;
    uint32_t    index_no;
    uint32_t    ret_sts;
    uint32_t    find_flg = REMAP_NOTFIND;

    /* Generate Physical Base Address and Offset Address */
    phys_base = phys_addr & SEP_BASE_ADRS;

    /* Check Initialize status */
    if (REMAP_NOTINIT == s_init_sts)
    {
        uiRet = ER_D_REMAP_STATE;
    }
    else if (PHYS_ADRS15_BASE == phys_base)
    {
        /* Case of RT-SRAM Remap Area */
        uiRet = ER_OK;
    }
    else if (PHYS_ADRS12_BASE == phys_base)
    {
        /* Case of  MFIS Remap Area */
        uiRet = ER_OK;
    }
    else if (PHYS_ADRS13_BASE == phys_base)
    {
        /* Case of  RPC (Flash) Remap Area */
        uiRet = ER_OK;
    }
    else if (PHYS_ADRS14_BASE == phys_base)
    {
        /* Case of  Secure Engine Remap Area */
        uiRet = ER_OK;
    }
    else
    {
        /* Case of Local RAM (Not targeted to remap but accessible from ICU-M) */
        ret_sts = D_MISC_CheckArea(phys_addr);
        if (AREA_LOCALRAM == ret_sts)
        {
            uiRet = ER_OK;
        }
        /* Case of Not Fixed Area, Check Specified Address Area */
        else if (AREA_REMAP == ret_sts)
        {
            /* Search position same as Physical Base Address */
            for (index_no = COUNT_ZERO; (REMAP_REG_MAX - NUM4) > index_no; index_no += NUM2)
            {
                if (phys_base  == s_remap_tbl[index_no].phys_address)
                {
                    find_flg = REMAP_FIND;
                    break;
                }
                else
                {
                    /* No Operation */
                }
            }

            /* Not Found same Physical Base Address, Not Register Error */
            if (REMAP_NOTFIND == find_flg)
            {
                uiRet = ER_D_REMAP_NOREGIST;
            }
            else
            {
                uiRet = ER_OK;
            }
        }
        else
        {
            /* If Specified Address is not in Remap Area, Return with PARAMETER ERROR */
            uiRet = ER_D_REMAP_ILLEGALADR;
        }
    }

    return (uiRet);
}
#endif  /* defined(FSW_REMAP_DRIVER) */

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_REMAP_0008]:[ICUMFW_UD_S4_D_REMAP_0008]
* Function name: D_REMAP_GetPhysAddrs
* Description  : Get Physical Address
* Arguments    : (IN)logical_addr      Logical Address
*                (OUT)*p_phys_addr     Physical Address
* Return Value : ER_OK
*                ER_PARAM             : Logical Address is illegal or Pointer is NULL
*                ER_D_REMAP_STATE     : Not Initialize
*                ER_D_REMAP_NOREGIST  : Not Registered Physical Address
*                ER_D_REMAP_ILLEGALADR: Illegal Address
***************************************************************************************/
uint32_t D_REMAP_GetPhysAddrs(uint32_t logical_addr, uint32_t *p_phys_addr)
{
    uint32_t    uiRet = ER_OK;
#if !defined(FSW_REMAP_DRIVER)
    *p_phys_addr = logical_addr;
#else   /* FSW_REMAP_DRIVER */
    uint32_t    remap_base;
    uint32_t    offset_adrs;
    uint32_t    index_no;
    uint32_t    ret_sts;
    uint32_t    find_flg = REMAP_NOTFIND;
    uint32_t    find_no = COUNT_ZERO;

    /* 1. If pointer is NULL, Parameter Error */
    if (NULL == p_phys_addr)
    {
        uiRet = ER_PARAM;
    }
    /* 2. Check Initialize status */
    else if (REMAP_NOTINIT == s_init_sts)
    {
        uiRet = ER_D_REMAP_STATE;
    }
    else
    {
        /* 3. Generate Remap Access Window Base Address and Offset Address */
        remap_base = logical_addr & SEP_BASE_ADRS;
        offset_adrs = logical_addr & SEP_OFF_ADRS;

        if (REMAP_ADRS15_BASE == remap_base)
        {
            /* 4. Case of RT-SRAM Remap Access Window Area */
            *p_phys_addr = offset_adrs + PHYS_ADRS15_BASE;
        }
        else if (REMAP_ADRS12_BASE  == remap_base)
        {
            /* 5. Case of  MFIS Remap Area */
            *p_phys_addr = offset_adrs + PHYS_ADRS12_BASE;
        }
        else if (REMAP_ADRS13_BASE  == remap_base)
        {
            /* 6. Case of  RPC (Flash) Remap Area */
            *p_phys_addr = offset_adrs + PHYS_ADRS13_BASE;
        }
        else if (REMAP_ADRS14_BASE  == remap_base)
        {
            /* 7. Case of  Secure Engine Remap Area */
            *p_phys_addr = offset_adrs + PHYS_ADRS14_BASE;
        }
        else
        {
            /* 8. Check Specified Address Area */
            ret_sts = D_MISC_CheckArea(logical_addr);
            if (AREA_SICWINDOW != ret_sts)
            {
                /* 8-1. Case of Code Fetch Remap Area */
                if (AREA_CFWINDOW == ret_sts)
                {
                    /* 8-1-1. Case of Code Fetch Remap Area on RT-SRAM */
                    if ((CFREMAP_AREA_START <= logical_addr) && (CFREMAP_AREA_END >= logical_addr))
                    {
                        /* 8-1-1. Set Physical Address of Code Fetch Remap Access on RT-SRAM  */
                        *p_phys_addr = offset_adrs + CFREMAP_ADRS_BASE;
                    }
                    else
                    {
                        /* 8-1-2. If Specified Address is not in CF Remap Area, Return with PARAMETER ERROR */
                        uiRet = ER_D_REMAP_ILLEGALADR;
                    }
                }
                else
                {
                    /* 8-2. If Specified Address is not in SIC Remap Access Window Area, Return with PARAMETER ERROR */
                    uiRet = ER_D_REMAP_ILLEGALADR;
                }
            }
            else
            {
                /* 9. Search position same as Remap Access Window Address */
                for (index_no = COUNT_ZERO; (REMAP_REG_MAX - NUM4) > index_no; index_no += NUM2)
                {
                    if (remap_base == s_remap_tbl[index_no].remap_address)
                    {
                        find_flg = REMAP_FIND;
                        find_no = index_no;
                        break;
                    }
                    else
                    {
                        /* No Operation */
                    }
                }

                /* 9-1. Not Found same Remap Access Window Address, Parameter Error */
                if (REMAP_NOTFIND == find_flg)
                {
                    uiRet = ER_D_REMAP_NOREGIST;
                }
                else
                {
                    /* 10. When Physical Base Address is NULL(Not Registered), Not Registered Error */
                    if (PHYS_ADR_NOASGN == s_remap_tbl[find_no].phys_address)
                    {
                        uiRet = ER_D_REMAP_NOREGIST;
                    }
                    else
                    {
                        /* 11. Generate and Set Physical Address */
                        *p_phys_addr = s_remap_tbl[find_no].phys_address + offset_adrs;
                    }
                }
            }
        }
    }
#endif  /* FSW_REMAP_DRIVER */

    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_REMAP_0009]:[ICUMFW_UD_S4_D_REMAP_0009]
* Function name: D_REMAP_GetLogicalAddrs
* Description  : Get Logical Address (get only, no check)
* Arguments    : (IN)phys_addr           Physical Address
*                (OUT)*p_logical_addr    Logical Address
* Return Value : ER_OK
***************************************************************************************/
uint32_t D_REMAP_GetLogicalAddrs(uint32_t phys_addr, uint32_t *p_logical_addr)
{
#if defined(FSW_REMAP_DRIVER)
    uint32_t    phys_base;
    uint32_t    offset_adrs;
    uint32_t    index_no;
    uint32_t    ret_sts;
    uint32_t    find_flg = REMAP_NOTFIND;
    uint32_t    find_no = COUNT_ZERO;

    /* 1. Check Initialize status */
    if (REMAP_NOTINIT == s_init_sts)
    {
        *p_logical_addr = phys_addr;
    }
    else
    {
        /* 2. Generate Physical Base Address and Offset Address */
        phys_base = phys_addr & SEP_BASE_ADRS;
        offset_adrs = phys_addr & SEP_OFF_ADRS;

        if (PHYS_ADRS15_BASE == phys_base)
        {
            /* 3. Case of RT-SRAM Remap Area */
            *p_logical_addr = offset_adrs + REMAP_ADRS15_BASE;
        }
        else if (PHYS_ADRS12_BASE  == phys_base)
        {
            /* 4. Case of  MFIS Remap Area */
            *p_logical_addr = offset_adrs + REMAP_ADRS12_BASE;
        }
        else if (PHYS_ADRS13_BASE  == phys_base)
        {
            /* 5. Case of  RPC (Flash) Remap Area */
            *p_logical_addr = offset_adrs + REMAP_ADRS13_BASE;
        }
        else if (PHYS_ADRS14_BASE  == phys_base)
        {
            /* 6. Case of  Secure Engine Remap Area */
            *p_logical_addr = offset_adrs + REMAP_ADRS14_BASE;
        }
        else
        {
            /* 7. Case of Not Fixed Area, Check Specified Address Area */
            ret_sts = D_MISC_CheckArea(phys_addr);
            if (AREA_REMAP != ret_sts)
            {
                /* If Specified Address is not in Remap Area, Return with PARAMETER ERROR */
                *p_logical_addr = phys_addr;
            }
            else
            {
                /* 8. Search position same as Physical Base Address */
                for (index_no = COUNT_ZERO; (REMAP_REG_MAX - NUM4) > index_no; index_no += NUM2)
                {
                    if (phys_base  == s_remap_tbl[index_no].phys_address)
                    {
                        find_flg = REMAP_FIND;
                        find_no = index_no;
                        break;
                    }
                    else
                    {
                        /* No Operation */
                    }
                }

                /* 8-1. Not Found same Physical Base Address, Not Register Error */
                if (REMAP_NOTFIND == find_flg)
                {
                    *p_logical_addr = phys_addr;
                }
                else
                {
                    /* 9. Generate and Set Logical Address */
                    *p_logical_addr = s_remap_tbl[find_no].remap_address + offset_adrs;
                }
            }
        }
    }
#else   /* defined(FSW_REMAP_DRIVER) */
    *p_logical_addr = phys_addr;
#endif  /* defined(FSW_REMAP_DRIVER) */

    return ER_OK; 
}

#if defined(FSW_REMAP_DRIVER)
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_REMAP_0011]:[ICUMFW_UD_S4_D_REMAP_0011]
* Function name: D_REMAP_CheckGetLogicalAddrs
* Description  : Check & Get Logical Address
* Arguments    : (IN)phys_addr           Physical Address
*                (OUT)*p_logical_addr    Logical Address
* Return Value : ER_OK
*                ER_PARAM             : Pointer is NULL
*                ER_D_REMAP_STATE     : Not Initialize
*                ER_D_REMAP_NOREGIST  : Not Registered Physical Address
*                ER_D_REMAP_ILLEGALADR: Illegal Address
***************************************************************************************/
uint32_t  D_REMAP_CheckGetLogicalAddrs(uint32_t phys_addr, uint32_t *p_logical_addr)
{
    uint32_t    uiRet = ER_OK;
    uint32_t    phys_base;
    uint32_t    offset_adrs;
    uint32_t    index_no;
    uint32_t    ret_sts;
    uint32_t    find_flg = REMAP_NOTFIND;
    uint32_t    find_no = COUNT_ZERO;

    /* 1. If pointer is NULL, Parameter Error */
    if (NULL == p_logical_addr)
    {
        uiRet = ER_PARAM;
    }
    /* 2. Check Initialize status */
    else if (REMAP_NOTINIT == s_init_sts)
    {
        uiRet = ER_D_REMAP_STATE;
    }
    else
    {
        /* 3. Generate Physical Base Address and Offset Address */
        phys_base = phys_addr & SEP_BASE_ADRS;
        offset_adrs = phys_addr & SEP_OFF_ADRS;

        if (PHYS_ADRS15_BASE == phys_base)
        {
            /* 4. Case of RT-SRAM Remap Area */
            *p_logical_addr = offset_adrs + REMAP_ADRS15_BASE;
        }
        else if (PHYS_ADRS12_BASE  == phys_base)
        {
            /* 5. Case of  MFIS Remap Area */
            *p_logical_addr = offset_adrs + REMAP_ADRS12_BASE;
        }
        else if (PHYS_ADRS13_BASE  == phys_base)
        {
            /* 6. Case of  RPC (Flash) Remap Area */
            *p_logical_addr = offset_adrs + REMAP_ADRS13_BASE;
        }
        else if (PHYS_ADRS14_BASE  == phys_base)
        {
            /* 7. Case of  Secure Engine Remap Area */
            *p_logical_addr = offset_adrs + REMAP_ADRS14_BASE;
        }
        else
        {
            /* 8. Case of Not Fixed Area, Check Specified Address Area */
            ret_sts = D_MISC_CheckArea(phys_addr);
            if (AREA_REMAP != ret_sts)
            {
                /* If Specified Address is not in Remap Area, Return with PARAMETER ERROR */
                uiRet = ER_D_REMAP_ILLEGALADR;
            }
            else
            {
                /* 9. Search position same as Physical Base Address */
                for (index_no = COUNT_ZERO; (REMAP_REG_MAX - NUM4) > index_no; index_no += NUM2)
                {
                    if (phys_base  == s_remap_tbl[index_no].phys_address)
                    {
                        find_flg = REMAP_FIND;
                        find_no = index_no;
                        break;
                    }
                    else
                    {
                        /* No Operation */
                    }
                }

                /* 9-1. Not Found same Physical Base Address, Not Register Error */
                if (REMAP_NOTFIND == find_flg)
                {
                    uiRet = ER_D_REMAP_NOREGIST;
                }
                else
                {
                    /* 10. Generate and Set Logical Address */
                    *p_logical_addr = s_remap_tbl[find_no].remap_address + offset_adrs;
                }
            }
        }
    }

    return (uiRet);
}
#endif  /* defined(FSW_REMAP_DRIVER) */

PRAGMA(ghs section text = default)
PRAGMA(ghs section rodata = default)

PRAGMA(ghs section text = ".icum_driver.text")
PRAGMA(ghs section rodata = ".icum_driver.rodata")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_REMAP_0006]:[ICUMFW_UD_S4_D_REMAP_0006]
* Function name: d_REMAP_SearchPosition
* Description  : Register Remap Area.Search Registering position
* Arguments    : (IN)phys_base                Physical Base Address
*                (OUT)static s_reg_sts_flag   Registering Status Flag
*                     static s_reg_ptr        Registering Position
*                     static s_no_use_flag    Using Status Flag
*                     static s_lru_ptr        Registering Position according to LRU
* Return Value : ER_OK
*                ER_D_REMAP_NOREGIST
***************************************************************************************/
static uint32_t d_REMAP_SearchPosition(uint32_t phys_base)
{
    uint32_t    uiRet = ER_OK;
    uint32_t    index_no;
    uint8_t     lru_max_no = REG_STS_INIT_STS;
    uint32_t    find_flg = REMAP_NOTFIND;

    /* 1. Search Registering position */
    for (index_no = COUNT_ZERO; (REMAP_REG_MAX - NUM4) > index_no; index_no += NUM2)
    {
        /* 2. Area is not registered or not used (0xFF) */
        if (REGST_NOUSE == (uint32_t)s_remap_tbl[index_no].reg_flag)
        {
            /* 2-1. Area is not registered : Physical Address is not registered */
            if (PHYS_ADR_NOASGN == s_remap_tbl[index_no].phys_address)
            {
                /* 2-1-1. Registering Status Flag is Init Status */
                if (REG_STS_INIT_STS == s_reg_sts_flag)
                {
                    /* Set "Not Registered Area exist" status to Registering Status Flag */
                    s_reg_sts_flag = REG_STS_EXTNOTREG;
                    s_reg_ptr = index_no;
                    /* Set Find Flag */
                    find_flg = REMAP_FIND;
                    /* Search next area */
                }
                else
                {
                    /* No Operation */
                }
            }
            /* 2-2. Area is not used : Physical Address was registered */
            else
            {
                /* 2-2-1. Same as Physical Base Address */
                if (phys_base == s_remap_tbl[index_no].phys_address)
                {
                    /* Set "Area that is not used and had same Physical Address exist" status to Registering Status Flag */
                    s_reg_sts_flag = REG_STS_EXTNOUSAM;
                    s_reg_ptr = index_no;
                    /* Set Find Flag */
                    find_flg = REMAP_FIND;
                    break;
                    /* Expire the "for loop" */
                }
                /* 2-2-2. Not same */
                else
                {
                    /* Set "Area that is not used and had different Physical Address exist" status to Using Status Flag */
                    s_no_use_flag = USE_STS_EXIST;
                    /* Increment LRU Counter */
                    s_lru_tbl[index_no].lru_count++;
                    /* 2-2-2-1. When LRU Counter Value is larger than Maximum Value of searched LRU Counter */
                    if ((uint32_t)s_lru_tbl[index_no].lru_count > (uint32_t)lru_max_no)
                    {
                        /* Renew Maximum Value of searched LRU Counter */
                        lru_max_no = s_lru_tbl[index_no].lru_count;
                        /* Save REMAP Table position */
                        s_lru_ptr = index_no;
                    }
                    else
                    {
                        /* No Operation */
                    }
                    /* Set Find Flag */
                    find_flg = REMAP_FIND;
                    /* Search next area */
                }
            }
        }
        /* 3. Area is using (not 0xFF) */
        else
        {
            /* 3-1. Same as Physical Base Address */
            if (phys_base == s_remap_tbl[index_no].phys_address)
            {
                /* Set "Area that is used and had same Physical Address exist" status to Registering Status Flag */
                s_reg_sts_flag = REG_STS_EXTUSESAM;
                s_reg_ptr = index_no;
                /* Set Find Flag */
                find_flg = REMAP_FIND;
                break;
                /* Expire the "for loop" */
            }
            /* 3-2. Not same */
            else
            {
                /* Search next area */
            }
        }
    }

    if (REMAP_NOTFIND == find_flg)
    {
        uiRet = ER_D_REMAP_NOREGIST;
    }
    else
    {
        /* No Operation */
    }
    return (uiRet);
}

#if !defined(TARGET_DEVICE_S4H) && !defined(TARGET_DEVICE_U2B) && !defined(TARGET_DEVICE_U2C) && !defined(TARGET_DEVICE_U2C4)
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_REMAP_0002]:[ICUMFW_UD_S4_D_REMAP_0002]
* Function name: d_REMAP_SetRemapTbl
* Description  : Register user-specified area in the remap table
* Arguments    : (IN)phys_base        : Physical Address
* Return Value : ER_OK
*                ER_D_REMAP_RETRYOVER : Retry Over at writing Remap Register
***************************************************************************************/
static uint32_t d_REMAP_SetRemapTbl(uint32_t phys_base)
{
    uint32_t uiRet = ER_OK;
    uint32_t ret_sts;
    uint32_t loop_cnt;
    uint32_t index_no = REMAP_TBL_MAX - NUM4;

    /* 1. Search for unused or registered */
    for (loop_cnt = COUNT_ZERO; loop_cnt < (REMAP_TBL_MAX - NUM4); loop_cnt += NUM2)
    {
        if (REGST_NOUSE == (uint32_t)s_remap_tbl[loop_cnt].reg_flag)
        {
            /* 1-1. If register flag is REGST_NOUSE, set loop_cnt to index_no and finish the for loop */
            index_no = loop_cnt;
            break;
        }
        else if (phys_base == s_remap_tbl[loop_cnt].phys_address)
        {
            /* 1-2. If register flag is not REGST_NOUSE and phys_base is already remapped, return without doing anything. */
            break;
        }
        else
        {
            /* No Operation */
        }
    }

    /* 2. Check whether current return code is ER_OK and index number is not initial value */
    if ((REMAP_TBL_MAX - NUM4) > index_no)
    {
        /* 2-1. Synchronize memory */
        SYNC_MEMORY();
        /* 2-2. Set physical base address to ICUMX_SCIREMAP2Mn register */
        ret_sts = d_REMAP_Exec(index_no, phys_base);
        /* 2-3. Check whether return code from d_REMAP_Exec function is ER_OK */
        if (ER_OK == ret_sts)
        {
            /* 2-3-1. Synchronize memory */
            SYNC_MEMORY();
            /* 2-3-2. Set to remap table */
            s_remap_tbl[index_no].reg_flag = (uint8_t)index_no;
            s_remap_tbl[index_no].lru_count = 1U;
            s_remap_tbl[index_no].reg_count = 1U;
            s_remap_tbl[index_no].phys_address = phys_base;
            s_remap_tbl[index_no+1U].phys_address = phys_base + BIAS_2MB;
            /* 2-3-3. Set the number of the index that the area is registered by user. */
            s_user_set_no = index_no + 1U;
        }
        else
        {
            /* 2-4. Error writing to remap register */
            uiRet = ER_D_REMAP_RETRYOVER;
        }
    }
    else
    {
        /* No Operation */
    }
    return (uiRet);
}
#endif /* !defined(TARGET_DEVICE_S4H) */

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_REMAP_0003]:[ICUMFW_UD_S4_D_REMAP_0003]
* Function name: d_REMAP_Exec
* Description  : Get Logical Address
* Arguments    : (IN)reg               Remapping Register Address
*                (IN)phys_base         Physical Address
* Return Value : ER_OK
*                ER_D_REMAP_RETRYOVER  : Retry Over
***************************************************************************************/
static uint32_t d_REMAP_Exec(uint32_t regist_ptr, uint32_t phys_base)
{
    uint32_t    uiRet = ER_OK;
    uint32_t    ret_sts;

    /* 1. Remap Physical Base Area */
    ret_sts = d_REMAP_WriteProcReg((volatile uint32_t *)(s_remap_tbl[regist_ptr].remap_register), phys_base, MISC_PROT_RETRY);
    if (ER_OK != ret_sts)
    {
        /* 1-1. Retry Over Occur */
        uiRet = ER_D_REMAP_RETRYOVER;
    }
    /* Success of Writing remap register */
    else
    {
        /* 2. Remap Next Physical Base Area */
        ret_sts = d_REMAP_WriteProcReg((volatile uint32_t *)(s_remap_tbl[regist_ptr + 1U].remap_register), phys_base + BIAS_2MB, MISC_PROT_RETRY);
        if (ER_OK != ret_sts)
        {
            /* 2-1. Retry Over Occur */
            uiRet = ER_D_REMAP_RETRYOVER;
        }
        /* Success of Writing remap register */
        else
        {
            /* No Operation */
        }
    }

    return (uiRet);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_REMAP_0004]:[ICUMFW_UD_S4_D_REMAP_0004]
* Function name: d_REMAP_WriteProcReg
* Description  : Write REMAP Register
* Arguments    : (IN)p_reg             Remapping Register Address
*                (IN)phys_base         Physical Address
*                (IN)retry             Retry Value
* Return Value : ER_OK
*                REMAP_RETRYOVER       : Retry Over
***************************************************************************************/
static uint32_t d_REMAP_WriteProcReg(register volatile uint32_t *p_reg, uint32_t phys_base, uint32_t retry)
{
    uint32_t    uiRet = ER_OK;
    int32_t     loop_counter;

    loop_counter = (int32_t)retry;
    while (1)
    {
        /* 1. Set ICUMX_PROT0CMD register to 0xA5 */
        (ICU_GEN0ICUM_PROT0PCMD) = PROTCMD_START;
        /* 2. Set physical address to SIC remap area register */
        *p_reg = phys_base;
        /* 3. Set the inverse data of physical address to SIC remap area register */
        *p_reg = ~phys_base;
        /* 4. Set physical address to SIC remap area register */
        *p_reg = phys_base;
        /* 5. Check ICUM_PROT0PS register */
        if (PROTS0ERR == (uint32_t)(ICU_GEN0ICUM_PROT0PS & PROTS0ERR))
        {
            /* 5-1. If error is occurred, decrement the retry counter */
            loop_counter--;
            if (0 >= loop_counter)
            {
                /* 5-2. If retry counter is 0, return with REMAP_RETRYOVER */
                uiRet = REMAP_RETRYOVER;
                break;
            }
            else
            {
                /* No Operation */
            }
        }
        else
        {
            break;
        }
    }
    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_REMAP_0013]:[ICUMFW_UD_S4_D_REMAP_0013]
* Function name: d_REMAP_DecideRegPosition
* Description  : Decide Registering Position
* Arguments    : (OUT)p_logical_addr   Logical Address
*              : (IN)phys_base         Physical Address
*              : (IN)offset_adrs       offset Address
* Return Value : ER_OK
*                REMAP_RETRYOVER       : Retry Over
***************************************************************************************/
static uint32_t d_REMAP_DecideRegPosition(uint32_t *p_logical_addr, uint32_t phys_base, uint32_t offset_adrs)
{
    uint32_t    saved_psw;
    uint32_t    uiRet = ER_OK;
    uint32_t    index_no;
    uint32_t    ret_sts;

    /* 1. Disable interrupts (save ID bit) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    s_reg_sts_flag = REG_STS_FULREGIST;
    s_reg_ptr      = REG_STS_INIT_STS;
    s_no_use_flag  = REG_STS_INIT_STS;
    s_lru_ptr      = REG_STS_INIT_STS;

    /* 2. Copy LRU Counter to LRU Table */
    for (index_no = COUNT_ZERO; (REMAP_REG_MAX - NUM4) > index_no; index_no++)
    {
        s_lru_tbl[index_no].lru_count = s_remap_tbl[index_no].lru_count;
    }

    /* 3. Search Registering position */
    ret_sts = d_REMAP_SearchPosition(phys_base);
    if (ER_D_REMAP_NOREGIST == ret_sts)
    {
        uiRet = ER_D_REMAP_NOTASIGN;
    }
    else
    {
        /* No Operation */
    }

    if (ER_OK == uiRet)
    {
        /* 4. Decide Registering Position */
        switch (s_reg_sts_flag)
        {
            /* 4-1. When Area are all Registered */
            case REG_STS_FULREGIST:

                /* 4-1-1. There are some Area that is not used and has different Physical Address */
                if (USE_STS_EXIST == s_no_use_flag)
                {
                    /* 4-1-1-1. Synchronize memory */
                    SYNC_MEMORY();

                    /* 4-1-1-2. Remap Physical Base Area */
                    ret_sts = d_REMAP_Exec(s_lru_ptr, phys_base);
                    if (ER_D_REMAP_RETRYOVER == ret_sts)
                    {
                        /* Retry Over Occur */
                        uiRet = ER_D_REMAP_RETRYOVER;
                        break;
                    }
                    /* Success of Writing remap register */
                    else
                    {
                        /* No Operation */
                    }

                    /* 4-1-1-3. Synchronize memory */
                    SYNC_MEMORY();

                    /* 4-1-1-4. Save Physical Base Address */
                    s_remap_tbl[s_lru_ptr].phys_address = phys_base;
                    s_remap_tbl[s_lru_ptr + 1U].phys_address = phys_base + BIAS_2MB;

                    /* 4-1-1-5. Renew LRU Counter to REMAP Table */
                    for (index_no = COUNT_ZERO; (REMAP_REG_MAX - NUM4) > index_no; index_no++)
                    {
                        s_remap_tbl[index_no].lru_count = s_lru_tbl[index_no].lru_count;
                    }

                    /* 4-1-1-6. Set using status to Register Status */
                    s_remap_tbl[s_lru_ptr].reg_flag = (uint8_t)s_lru_ptr;
                    /* 4-1-1-7. Set 1 to LRU Counter */
                    s_remap_tbl[s_lru_ptr].lru_count = 1U;
                    /* 4-1-1-8. Increment Register Counter */
                    s_remap_tbl[s_lru_ptr].reg_count++;
                    /* 4-1-1-9. Set Logical Address */
                    *p_logical_addr = s_remap_tbl[s_lru_ptr].remap_address + offset_adrs;

                }
                /* 4-1-2. ALL Area are used and have different Physical Address */
                else
                {
                    /* Return with "Not Empty for Remap Table" */
                    uiRet = ER_D_REMAP_NOTASIGN;
                }
                break;

            /* 4-2. There are some area are not registered */
            case REG_STS_EXTNOTREG:

                /* 4-2-1. Synchronize memory */
                SYNC_MEMORY();

                /* 4-2-2. Remap Physical Base Area */
                ret_sts = d_REMAP_Exec(s_reg_ptr, phys_base);
                if (ER_D_REMAP_RETRYOVER == ret_sts)
                {
                    /* Retry Over Occur */
                    uiRet = ER_D_REMAP_RETRYOVER;
                    break;
                }
                /* Success of Writing remap register */
                else
                {
                    /* No Operation */
                }

                /* 4-2-3. Synchronize memory */
                SYNC_MEMORY();

                /* 4-2-4. Save Physical Base Address */
                s_remap_tbl[s_reg_ptr].phys_address = phys_base;
                s_remap_tbl[s_reg_ptr + 1U].phys_address = phys_base + BIAS_2MB;

                /* 4-2-5. Set using status to Register Status */
                s_remap_tbl[s_reg_ptr].reg_flag = (uint8_t)s_reg_ptr;
                /* 4-2-6. Set 1 to LRU Counter */
                s_remap_tbl[s_reg_ptr].lru_count = 1U;
                /* 4-2-7. Increment Register Counter */
                s_remap_tbl[s_reg_ptr].reg_count++;
                /* 4-2-8. Set Logical Address */
                *p_logical_addr = s_remap_tbl[s_reg_ptr].remap_address + offset_adrs;
                break;

            /* 4-3. Area has same Physical Address exist */
            case REG_STS_EXTUSESAM:
            case REG_STS_EXTNOUSAM:

                /* 4-3-1. Check whether the area is registered by user. */
                if (s_user_set_no <= s_reg_ptr)
                {
                    /* 4-3-2. Set using status to Register Status */
                    s_remap_tbl[s_reg_ptr].reg_flag = (uint8_t)s_reg_ptr;
                    /* 4-3-3. Set 1 to LRU Counter */
                    s_remap_tbl[s_reg_ptr].lru_count = 1U;
                    /* 4-3-4. Increment Register Counter */
                    s_remap_tbl[s_reg_ptr].reg_count++;
                }
                else
                {
                    /* No Operation */
                }
                /* 4-3-5. Set Logical Address */
                *p_logical_addr = s_remap_tbl[s_reg_ptr].remap_address + offset_adrs;
                break;

            /* 4-4. Register Status Flag is other status */
            default:
                uiRet = ER_D_REMAP_NOTASIGN;
                break;
        }
    } /* end of if (ER_OK == uiRet) */
    else
    {
        /* Do nothing */
    }

    /* 13. re-enable interrupts (Enable Int when saved ID bit is enable) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_REMAP_0014]:[ICUMFW_UD_S4_D_REMAP_0014]
* Function name: d_REMAP_SetRemapFixedTbl
* Description  : Register fixed area in the remap table
* Arguments    : none
* Return Value : ER_OK
*                ER_D_REMAP_RETRYOVER
***************************************************************************************/
static uint32_t d_REMAP_SetRemapFixedTbl(void)
{
    uint32_t    uiRet = ER_OK;
    uint32_t    ret_sts;

    /* 1. Set SICREMAP2M12 */
    ret_sts = d_REMAP_WriteProcReg((volatile uint32_t *)(s_remap_tbl[REG_NUM_12].remap_register),
                                   s_remap_tbl[REG_NUM_12].phys_address,
                                   MISC_PROT_RETRY);
    if (ER_OK != ret_sts)
    {
        /* Retry Over Occur */
        uiRet = ER_D_REMAP_RETRYOVER;
    }
    /* Success of Writing remap register */
    else
    {
        /* No Operation */
    }

    if (ER_OK == uiRet)
    {
        /* 2. Set SICREMAP2M13 */
        ret_sts = d_REMAP_WriteProcReg((volatile uint32_t *)(s_remap_tbl[REG_NUM_13].remap_register),
                                       s_remap_tbl[REG_NUM_13].phys_address,
                                       MISC_PROT_RETRY);
        if (ER_OK != ret_sts)
        {
            /* Retry Over Occur */
            uiRet = ER_D_REMAP_RETRYOVER;
        }
        /* Success of Writing remap register */
        else
        {
            /* No Operation */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK == uiRet)
    {
        /* 3. Set SICREMAP2M14 */
        ret_sts = d_REMAP_WriteProcReg((volatile uint32_t *)(s_remap_tbl[REG_NUM_14].remap_register),
                                       s_remap_tbl[REG_NUM_14].phys_address,
                                       MISC_PROT_RETRY);
        if (ER_OK != ret_sts)
        {
            /* Retry Over Occur */
            uiRet = ER_D_REMAP_RETRYOVER;
        }
        /* Success of Writing remap register */
        else
        {
            /* No Operation */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK == uiRet)
    {
        /* 4. Set SICREMAP2M15 */
        ret_sts = d_REMAP_WriteProcReg((volatile uint32_t *)(s_remap_tbl[REG_NUM_15].remap_register),
                                       s_remap_tbl[REG_NUM_15].phys_address,
                                       MISC_PROT_RETRY);
        if (ER_OK != ret_sts)
        {
            /* Retry Over Occur */
            uiRet = ER_D_REMAP_RETRYOVER;
        }
        /* Success of Writing remap register */
        else
        {
            /* No Operation */
        }
    }
    else
    {
        /* Do nothing */
    }
    return(uiRet);
}

PRAGMA(ghs section text = default)
PRAGMA(ghs section rodata = default)
