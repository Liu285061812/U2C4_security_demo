/******************************************************************************/
/* Component Name   ICUM FLS Library                                          */
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
#include "renesas_types.h"
#include "icum_d_comm_pub.h"
#include "icum_d_misc_pub.h"
#include L_FLASH_PUBLIC_H
#include "icum_l_flash.h"

/********************************************************************************
 QAC Suppression message list
*********************************************************************************/

/********************************************************************************
 Memory allocation section specification
*********************************************************************************/
/* self copy icum_driver_rom.text to icum_driver_ram   */
CHANGE_SEC(text, ".icum_library_ram")

/* self copy icum_driver_rom.rodata to icum_driver.rodata */
CHANGE_SEC(rodata, ".icum_library.rodata")

CHANGE_SEC(data, ".icum_library.data")

/********************************************************************************
 Macro definitions
*********************************************************************************/
#define L_FLS_REF_CNT_MAX           (2uL)

/*******************************************************************************
 Typedef definitions
*********************************************************************************/

/********************************************************************************
 Exported global variables (to be accessed by other files)
*********************************************************************************/

/********************************************************************************
 Private global variables and functions
*********************************************************************************/
static uint8_t g_fls_faci_ref_count = 0u;

/********************************************************************************
* const data
********************************************************************************/

/********************************************************************************
 API Function definition
*********************************************************************************/

/********************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0001]:[ICUMFW_UD_L_FLS_0001]
* Function Name : L_FLS_Init
* Description   : Initialize FLS Library
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_library.text")
CHANGE_SEC(rodata, ".icum_library.rodata")

uint32_t L_FLS_Init(void)
{
    /* Set DF Request Mode Flag OFF */
    (void)l_FLS_SetDfProgReq_ram(AG_L_FLS_DF_PE_CLR);

    return (ER_OK);
}

/* self copy icum_library_rom.text to icum_library_ram   */
CHANGE_SEC(text, ".icum_library_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_library_const_ram")

/*******************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0002]:[ICUMFW_UD_L_FLS_0002]
* Function Name : L_FLS_ReadFlash_ram
* Description   : Read to the flash area.
* Arguments     : target_area
*                  target flash area.
*                 bank
*                  target bank.
*                 offset
*                  Byte offset position of the area to be read.
*                   (offset specification is specified from the beginning of the
*                    bank when bank specification is valid)
*                 data_size_in_bytes
*                  read size.(in 4-byte units)
*                 p_data
*                  read data buffer.(4byte boundary)
* Return Value  :   ER_OK
*                   ER_L_FLS_FLOW 
*                   ER_PARAM
*                   ER_L_FLS_ECC1EDF1
*                   ER_L_FLS_ECC2EDF1 
********************************************************************************/
uint32_t L_FLS_ReadFlash_ram(const uint32_t target_area, const uint32_t bank, const uint32_t offset,
                             const uint32_t data_size_in_bytes, uint8_t *p_data)
{
    uint32_t lul_fls_status;
    uint32_t lul_raddr;

    lul_raddr = L_FLS_ZERO_U32;
    /* status check */
    lul_fls_status = D_FLS_CheckStatus_ram();
    if (ER_D_FLS_UNAVAILABLE == lul_fls_status)
    {
        /* FLS non-initialize */
        lul_fls_status = ER_L_FLS_FLOW;
    }
    else if (NULL == p_data)
    {
        /* null pointer */
        lul_fls_status = ER_PARAM;
    }
    /* not 4byte align ? */
    else if (L_FLS_ZERO_U32 != ((uint32_t)p_data % sizeof(uint32_t)))
    {
        /* Not 4 byte boundary */
        lul_fls_status = ER_PARAM;
    }
    /* not 4byte align ? */
    else if (0UL != (offset % sizeof(uint32_t)))
    {
        /* Not 4 byte boundary */
        lul_fls_status = ER_PARAM;
    }
    else if (0UL != (data_size_in_bytes % sizeof(uint32_t)))
    {
        /* Not 4 byte boundary */
        lul_fls_status = ER_PARAM;
    }
    else
    {
        /* Address check */
        lul_fls_status = D_FLS_CheckAddr_ram(target_area, bank, offset, data_size_in_bytes, &lul_raddr);
        if (ER_OK != lul_fls_status)
        {
            /* Address error */
        }
        else
        {
            lul_fls_status = ER_OK;
            /* code flash */
            switch (target_area)
            {
                case AG_L_FLS_CODE:
                    /* do nothing */
                    break;
                case AG_L_FLS_DATA:
                    /* clear ecc error */
                    (void)D_FLS_CheckECC_ram(AG_D_FLS_CLR);
                    break;
                case AG_L_FLS_SECURITY:
                    /* do nothing */
                    break;
                default:
                    /* Parameter error */
                    lul_fls_status = ER_PARAM;
                    break;
            }
        }
    }

    if (ER_OK != lul_fls_status)
    {
        /* do nothing */
    }
    else
    {
        /* clear request of flash mode change */
        (void)l_FLS_SetDfProgReq_ram(AG_L_FLS_DF_PE_CLR);
        /* data read */
        l_FLS_ReadData_ram(lul_raddr, data_size_in_bytes, p_data);
        /* Check data flash ecc error */
        if (AG_L_FLS_DATA == target_area)
        {
            lul_fls_status = D_FLS_CheckECC_ram(AG_D_FLS_CLR);
        }
        else
        {
            /* do nothing */
        }
    }

    /* convert driver error code to library error code. */
    lul_fls_status = l_FLS_ChangeErrCode_ram(lul_fls_status);

    return (lul_fls_status);
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0003]:[ICUMFW_UD_L_FLS_0003]
* Function Name : L_FLS_REQ_EraseFlash_ram
* Description   : Request erase of code/data flash area.
* Arguments     : target_area
*                  target flash area.
*                 bank
*                  target bank.
*                 offset
*                  Byte offset position of erase area.
*                   (specify offset position from the beginning of bank)
*                 data_size_in_bytes
*                   Number of bytes to erase (in 4-byte units).
* Return Value  :   ER_IN_PROCESS 
*                   ER_PARAM
*                   ER_L_FLS_FLOW 
*                   ER_L_FLS_REJECTED
*                   ER_L_FLS_INTERNAL
*                   ER_L_FLS_UNAVAILABLE
*                   ER_L_FLS_PROTECTION
*                   ER_L_FLS_PROGRAM
*                   ER_L_FLS_FACI_TIME_OUT
*******************************************************************************************/
uint32_t L_FLS_REQ_EraseFlash_ram(const uint32_t target_area, const uint32_t bank, const uint32_t offset,
                                  const uint32_t data_size_in_bytes)
{
    uint32_t lul_fls_status;
    uint32_t lul_flash_addr;
    uint32_t lul_block_addr;
    uint32_t lul_block_size;
    uint32_t lul_data_size;

    lul_flash_addr = L_FLS_ZERO_U32;
    /* Bus guard cancel & Write protect cancel */
    D_FLS_PrgPrepareOperation_ram();

    /* status check */
    lul_fls_status = D_FLS_CheckStatus_ram();
    if (ER_D_FLS_UNAVAILABLE == lul_fls_status)
    {
        /* FLS non-initialize */
        lul_fls_status = ER_L_FLS_FLOW;
    }
    else if (L_FLS_ZERO_U32 == data_size_in_bytes)
    {
        /* Invalid data size */
        lul_fls_status = ER_PARAM;
    }
    /* not 4byte align ? */
    else if (0UL != (offset % sizeof(uint32_t)))
    {
        /* Not 4 byte boundary */
        lul_fls_status = ER_PARAM;
    }
    else
    {
        /* Address check */
        lul_fls_status = D_FLS_CheckAddr_ram(target_area, bank, offset, data_size_in_bytes, &lul_flash_addr);
        if (ER_OK != lul_fls_status)
        {
            /* Address error */
        }
        else
        {
            lul_fls_status = ER_OK;
        }
    }

    if (ER_OK != lul_fls_status)
    {
        /* do nothing */
    }
    /* code flash */
    else if (AG_L_FLS_CODE == target_area)
    {
        (void)D_FLS_GetCfEraseBlockSize_ram(lul_flash_addr, &lul_block_size);
        /* erase block address */
        lul_block_addr = lul_flash_addr & ~(uint32_t)(lul_block_size - 1UL);
        /* erase block size */
        lul_data_size = data_size_in_bytes + (lul_flash_addr - lul_block_addr);

        /* FlashDrv call */
        lul_fls_status = D_FLS_EraseCf_ram(lul_block_addr, lul_data_size);
    }
    /* data flash */
    else if (AG_L_FLS_DATA == target_area)
    {
        (void)D_FLS_GetDfEraseBlockSize(lul_flash_addr, &lul_block_size);
        /* Address is not on align ? */
        if ((lul_flash_addr & (uint32_t)(lul_block_size - 1UL)) != 0UL)
        {
            /* align error */
            lul_fls_status = ER_PARAM;
        }
        else
        {
            /* Size is not on align ? */
            if ((data_size_in_bytes & (uint32_t)(lul_block_size - 1UL)) != 0UL)
            {
                /* align error */
                lul_fls_status = ER_PARAM;
            }
            else
            {
                /* FlashDrv call */
                lul_fls_status = D_FLS_EraseDf(lul_flash_addr, data_size_in_bytes);
            }
        }
    }
    else
    {
        lul_fls_status = ER_PARAM;
    }

    if (ER_IN_PROCESS != lul_fls_status)
    {
        /* Bus guard set & Write protect set */
        D_FLS_PrgDisposeOperation_ram();
    }
    else
    {
        /* do nothing */
    }

    /* convert driver error code to library error code. */
    lul_fls_status = l_FLS_ChangeErrCode_ram(lul_fls_status);

    return (lul_fls_status);
}
/******************************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0004]:[ICUMFW_UD_L_FLS_0004]
* Function Name : L_FLS_REQ_WriteFlash_ram
* Description   : Request write of code/data/config/security flash area.
* Arguments     : target_area
*                  target flash area.
*                 bank
*                  target bank.
*                 offset
*                  Byte offset position of erase area.
*                   (specify offset position from the beginning of bank)
*                 data_size_in_bytes
*                  Number of bytes to erase (in 4-byte units).
*                 p_data
*                  data to be written.
* Return Value  :   ER_IN_PROCESS 
*                   ER_PARAM
*                   ER_L_FLS_FLOW 
*                   ER_L_FLS_REJECTED
*                   ER_L_FLS_PROTECTION
*                   ER_L_FLS_PROGRAM
*                   ER_L_FLS_INTERNAL
*                   ER_L_FLS_UNAVAILABLE
*                   ER_L_FLS_FACI_TIME_OUT
*******************************************************************************************/
uint32_t L_FLS_REQ_WriteFlash_ram(const uint32_t target_area, const uint32_t bank, const uint32_t offset,
                                  const uint32_t data_size_in_bytes, const uint8_t *p_data)
{
    uint32_t lul_fls_status;
    uint32_t lul_flash_addr;
    uint32_t lul_write_size;

    lul_flash_addr = L_FLS_ZERO_U32;
    /* Bus guard cancel & Write protect cancel */
    D_FLS_PrgPrepareOperation_ram();

    /* status check */
    lul_fls_status = D_FLS_CheckStatus_ram();
    if (ER_D_FLS_UNAVAILABLE == lul_fls_status)
    {
        /* FLS non-initialize */
        lul_fls_status = ER_L_FLS_FLOW;
    }
    else if (NULL == p_data)
    {
        /* null pointer */
        lul_fls_status = ER_PARAM;
    }
    /* not 4byte align ? */
    else if (0UL != ((uint32_t)p_data % sizeof(uint32_t)))
    {
        /* Not 4 byte boundary */
        lul_fls_status = ER_PARAM;
    }
    /* not 4byte align ? */
    else if (0UL != (offset % sizeof(uint32_t)))
    {
        /* Not 4 byte boundary */
        lul_fls_status = ER_PARAM;
    }
    else if (data_size_in_bytes == 0UL)
    {
        /* Invalid data size */
    }
    /* data size is 4 byte unit ? */
    else if (0UL != (data_size_in_bytes % sizeof(uint32_t)))
    {
        /* Not 4 byte boundary */
        lul_fls_status = ER_PARAM;
    }
    else
    {
        /* Address check */
        lul_fls_status = D_FLS_CheckAddr_ram(target_area, bank, offset, data_size_in_bytes, &lul_flash_addr);
    }

    if (ER_OK != lul_fls_status)
    {
        /* Do nothing */
    }
    /* code flash */
    else if (target_area == AG_L_FLS_CODE)
    {
        (void)D_FLS_GetCfProgramWriteSize_ram(lul_flash_addr, &lul_write_size);
        /* Flash address is 512 byte boundary ? */
        if (0UL != (lul_flash_addr % lul_write_size))
        {
            /* align error */
            lul_fls_status = ER_PARAM;
        }
        else
        {
            /* FlashDrv call */
            lul_fls_status = D_FLS_ProgramCf_ram(lul_flash_addr, p_data, data_size_in_bytes);
        }
    }
    /* data flash */
    else if (target_area == AG_L_FLS_DATA)
    {
        (void)D_FLS_GetDfProgramWriteSize(lul_flash_addr, &lul_write_size);
        /* Flash address is 4 byte boundary ? */
        if (0UL != (lul_flash_addr % lul_write_size))
        {
            /* align error */
            lul_fls_status = ER_PARAM;
        }
        else
        {
            /* FlashDrv call */
            lul_fls_status = D_FLS_ProgramDf(lul_flash_addr, p_data, data_size_in_bytes);
        }
    }
    else
    {
        lul_fls_status = ER_PARAM;
    }

    if (lul_fls_status != ER_IN_PROCESS)
    {
        /* Bus guard set & Write protect set */
        D_FLS_PrgDisposeOperation_ram();
    }
    else
    {
        /* do nothing */
    }

    /* convert driver error code to library error code. */
    lul_fls_status = l_FLS_ChangeErrCode_ram(lul_fls_status);

    return (lul_fls_status);
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0005]:[ICUMFW_UD_L_FLS_0005]
* Function Name : L_FLS_REQ_WriteSecurity_ram
* Description   : Write the specified OPBT information.
* Arguments     :
*                 target_info
*                  target opbt infomation.
*                 set_data
*                  write opbt infomation data(4byte).
* Return Value  :   ER_IN_PROCESS 
*                   ER_PARAM
*                   ER_L_FLS_FLOW 
*                   ER_L_FLS_REJECTED
*                   ER_L_FLS_INTERNAL
*                   ER_L_FLS_UNAVAILABLE
*                   ER_L_FLS_PROTECTION
*                   ER_L_FLS_PROGRAM
*                   ER_L_FLS_FACI_TIME_OUT
*******************************************************************************************/
uint32_t L_FLS_REQ_WriteSecurity_ram(const uint32_t target_info, const uint32_t set_data)
{
    uint32_t lul_fls_status;
    uint32_t lul_flash_addr;
    uint32_t lul_write_size;
    uint8_t  luc_prog_req;
    uint32_t lul_start_block_addr;
    /* UpdateSecurity buffer area 32byte */
    uint32_t lul_cfg_data_area[L_FLS_CFGBUFF_NUM];

    /* Bus guard cancel & Write protect cancel */
    D_FLS_PrgPrepareOperation_ram();

    /* status check */
    lul_fls_status = D_FLS_CheckStatus_ram();
    if (ER_D_FLS_UNAVAILABLE == lul_fls_status)
    {
        /* FLS non-initialize */
        lul_fls_status = ER_L_FLS_FLOW;
    }
    else
    {
        /* Get target address */
        lul_fls_status = D_FLS_GetOPBTAddress(target_info, &lul_flash_addr);
    }

    if (ER_OK != lul_fls_status)
    {
        /* do nothing */
    }
    else
    {
        /* write block calc */
        (void)D_FLS_GetWriteSizeSec(lul_flash_addr, &lul_write_size);
        lul_start_block_addr = lul_flash_addr & ~(lul_write_size - 1UL);

        /* clear request of flash mode change */
        (void)l_FLS_SetDfProgReq_ram(AG_L_FLS_DF_PE_CLR);

        /* data read */
        l_FLS_ReadData_ram(lul_start_block_addr, lul_write_size, (uint8_t *)lul_cfg_data_area);
        /* request of flash mode change for CPU(PE)? */
        (void)l_FLS_GetDfProgReq_ram(&luc_prog_req);
    }

    if (ER_OK != lul_fls_status)
    {
        /* do nothing */
    }
    else if ((uint32_t)AG_L_FLS_DF_PE_SET == (uint32_t)luc_prog_req)
    {
        lul_fls_status = ER_L_FLS_PROGRAM;
    }
    else
    {
        /* write data set */
        lul_fls_status = l_FLS_SetWriteSecurityData_ram(
            target_info, set_data, &lul_cfg_data_area[((lul_flash_addr - lul_start_block_addr) / L_FLS_ALIGN_SIZE)]);
    }

    if (ER_OK == lul_fls_status)
    {
        /* FlashDrv call */
        lul_fls_status = D_FLS_UpdateSecurity(lul_start_block_addr, (const uint8_t *)lul_cfg_data_area, lul_write_size);
    }
    else
    {
        /* do nothing */
    }

    if ((uint32_t)ER_IN_PROCESS != lul_fls_status)
    {
        /* Bus guard set & Write protect set */
        D_FLS_PrgDisposeOperation_ram();
    }
    else
    {
        /* do nothing */
    }

    /* convert driver error code to library error code. */
    lul_fls_status = l_FLS_ChangeErrCode_ram(lul_fls_status);

    return (lul_fls_status);
}

/*************************************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0007]:[ICUMFW_UD_L_FLS_0007]
* Function Name : L_FLS_IsFinished_ram
* Description   : Proceed with this FACI command operation(REQ) by FLS and confirm the completion.
* Arguments     : context_id
*                  flash context id
*                 target_area
*                  target flash area
*                 bank
*                  target bank
* Return Value  :   ER_OK
*                   ER_IN_PROCESS 
*                   ER_PARAM
*                   ER_L_FLS_PROTECTION
*                   ER_L_FLS_REJECTED
*                   ER_L_FLS_PROGRAM
*                   ER_L_FLS_INTERNAL 
*                   ER_L_FLS_UNAVAILABLE
*                   ER_L_FLS_FACI_TIME_OUT
**************************************************************************************************/
uint32_t L_FLS_IsFinished_ram(const uint16_t context_id, const uint32_t target_area, const uint32_t bank)
{
    /* Parmeter Check   */
    uint32_t lul_fls_status;

    lul_fls_status = D_FLS_IsFinished_ram(context_id, target_area, bank);

    /* convert driver error code to library error code. */
    lul_fls_status = l_FLS_ChangeErrCode_ram(lul_fls_status);

    return (lul_fls_status);
}
/*******************************************************************************************
* Function ID   : (not supported)
* Function Name : L_FLS_REQ_Cancel_ram
* Description   : Request for forced stop of the flash operation process being executed.
* Arguments     :  uint32_t target_area
*                    target area
*                  uint32_t bank
*                    target bank
* Return Value  :   ER_OK
*                   ER_IN_PROCESS
*                   ER_L_FLS_FLOW 
*                   ER_L_FLS_CANCEL
*                   ER_L_FLS_INTERNAL
*                   ER_L_FLS_REJECTED
*                   ER_L_FLS_UNAVAILABLE
*                   ER_L_FLS_PROTECTION
*                   ER_L_FLS_PROGRAM
*                   ER_L_FLS_FACI_TIME_OUT
********************************************************************************************/
uint32_t L_FLS_REQ_Cancel_ram(const uint32_t target_area, const uint32_t bank)
{
    uint32_t lul_fls_status;

    /* status check */
    lul_fls_status = D_FLS_CheckStatus_ram();
    if (ER_D_FLS_UNAVAILABLE == lul_fls_status)
    {
        /* FLS non-initialize */
        lul_fls_status = ER_L_FLS_FLOW;
    }
    else
    {
        /* FlashDrv call */
        lul_fls_status = D_FLS_CancelReq_ram(target_area, bank);
    }

    D_FLS_PrgDisposeOperation_ram();

    /* convert driver error code to library error code. */
    lul_fls_status = l_FLS_ChangeErrCode_ram(lul_fls_status);

    return (lul_fls_status);
}

/*******************************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0009]:[ICUMFW_UD_L_FLS_0009]
* Function Name : L_FLS_GetFlashResource_ram
* Description   : Refer to the available flag and set unavailable if available.
* Arguments     : context_id
*                  flash context id
* Return Value  :   ER_OK
*                   ER_PENDING
*                   ER_L_FLS_INTERNAL
*                   ER_L_FLS_UNAVAILABLE
********************************************************************************************/
uint32_t L_FLS_GetFlashResource_ram(const uint32_t context_id, const uint8_t priority, const uint32_t target_area,
                                    const uint32_t bank)
{
    uint32_t err;
    uint16_t ctx_id;

    if (L_FLS_DUMMY_LOCK_HANDLE == context_id)
    {
        ctx_id = (uint16_t)L_FLS_ZERO_U32;
    }
    else
    {
        ctx_id = (uint16_t)context_id;
    }

    err = D_FLS_GetFlashResource_ram(ctx_id, priority, target_area, bank);
    if (ER_D_FLS_SUSPEND == err)
    {
        (void)D_FLS_FreeFlashResource_ram(target_area, bank);
    }
    else
    {
        /* do nothing */
    }

    /* convert driver error code to library error code. */
    err = l_FLS_ChangeErrCode_ram(err);

    return (err);
}

/*******************************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0010]:[ICUMFW_UD_L_FLS_0010]
* Function Name : L_FLS_FreeFlashResource_ram
* Description   : Set available.
* Arguments     : flash_availability_flg
*                   refer and setting value
* Return Value  :   ER_OK
*                   ER_PENDING
*                   ER_L_FLS_INTERNAL
*                   ER_L_FLS_UNAVAILABLE
********************************************************************************************/
uint32_t L_FLS_FreeFlashResource_ram(const uint32_t target_area, const uint32_t bank)
{
    uint32_t err;

    /* flag set */
    err = D_FLS_FreeFlashResource_ram(target_area, bank);
    /* convert driver error code to library error code. */
    err = l_FLS_ChangeErrCode_ram(err);

    return (err);
}

/********************************************************************************
 private Function definition
*********************************************************************************/

/*******************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0033]:[ICUMFW_UD_L_FLS_0033]
* Function Name : l_FLS_SetWriteSecurityData_ram
* Description   : convert driver error code to library error code.
* Arguments     : target_info
*                   target opbt infomation.
*                 set_data
*                   Write opbt infomation data(4byte).
*                 p_cfg_data
*                   Write Config data.
* Return Value  :   ER_OK
*                   ER_PARAM
********************************************************************************/
static uint32_t l_FLS_SetWriteSecurityData_ram(const uint32_t target_info, const uint32_t set_data,
                                               uint32_t *p_cfg_data)
{
    uint32_t lul_l_err_code;

    lul_l_err_code = ER_OK;
    /* write data set */
    if (AG_L_FLS_CHGRESP == target_info)
    {
        switch (set_data)
        {
            case AG_L_FLS_CR_RES_ICUP_SKIPPED:
                *p_cfg_data |= L_FLS_OPBT4_ICUMOPCRF2;
                break;

            case AG_L_FLS_CR_RES_CPU_SKIPPED:
                *p_cfg_data |= L_FLS_OPBT4_ICUMOPCRF1;
                break;

            case AG_L_FLS_CR_RES_ALL_SKIPPED:
                *p_cfg_data |= (L_FLS_OPBT4_ICUMOPCRF2 | L_FLS_OPBT4_ICUMOPCRF1);
                break;

            case AG_L_FLS_CR_RES_ICUP_ENABLED:
                *p_cfg_data &= ~L_FLS_OPBT4_ICUMOPCRF2;
                break;

            case AG_L_FLS_CR_RES_CPU_ENABLED:
                *p_cfg_data &= ~L_FLS_OPBT4_ICUMOPCRF1;
                break;

            case AG_L_FLS_CR_RES_ALL_ENABLED:
                *p_cfg_data &= ~(L_FLS_OPBT4_ICUMOPCRF2 | L_FLS_OPBT4_ICUMOPCRF1);
                break;

            case AG_L_FLS_OPBT_PEDBG_DISABLE:
                *p_cfg_data |= L_FLS_OPBT4_ICUMPEDBG;
                break;

            case AG_L_FLS_OPBT_PEDBG_ENABLE:
                *p_cfg_data &= ~L_FLS_OPBT4_ICUMPEDBG;
                break;

            case AG_L_FLS_OPBT_GPIO_ENABLE:
                *p_cfg_data |= L_FLS_OPBT4_ICUMOPGPIO;
                break;

            case AG_L_FLS_OPBT_GPIO_DISABLE:
                *p_cfg_data &= ~L_FLS_OPBT4_ICUMOPGPIO;
                break;

            case AG_L_FLS_OPBT_RHSIF_NEED:
                *p_cfg_data |= L_FLS_OPBT4_ICUMHSIF;
                break;

            case AG_L_FLS_OPBT_RHSIF_NOT_NEED:
                *p_cfg_data &= ~L_FLS_OPBT4_ICUMHSIF;
                break;

            case AG_L_FLS_OPBT_WDTA_ENABLE:
                *p_cfg_data |= D_FLS_ICUMOPWDEN;
                break;

            case AG_L_FLS_OPBT_WDTA_DISABLE:
                *p_cfg_data &= ~D_FLS_ICUMOPWDEN;
                break;

            default:
                /* parameter error   */
                lul_l_err_code = ER_PARAM;
                break;
        } /* end of switch (set_data) */
    } /* end of if (AG_L_FLS_CHGRESP == target_info) */
    else if (AG_L_FLS_ICUMUPGP == target_info)
    {
        switch (set_data)
        {
            case AG_L_FLS_OPBT_UPGP1_ENABLE:
                *p_cfg_data &= ~D_FLS_ICUMUPGP1;
                break;

            case AG_L_FLS_OPBT_UPGP1_DISABLE:
                *p_cfg_data |= D_FLS_ICUMUPGP1;
                break;

            case AG_L_FLS_OPBT_UPGP2_ENABLE:
                *p_cfg_data &= ~D_FLS_ICUMUPGP2;
                break;

            case AG_L_FLS_OPBT_UPGP2_DISABLE:
                *p_cfg_data |= D_FLS_ICUMUPGP2;
                break;

            default:
                /* parameter error   */
                lul_l_err_code = ER_PARAM;
                break;
        } /* end of switch (set_data) */

    } /* end of else if (AG_L_FLS_ICUMUPGP == target_info) */
    else if (AG_L_FLS_DEGRADATION == target_info)
    {
        *p_cfg_data &= ~D_FLS_DEGRADATION_DCF;
    } /* end of else if (AG_L_FLS_DEGRADATION == target_info) */
    
    else
    {
        *p_cfg_data = set_data;
    }

    return (lul_l_err_code);
}

/*******************************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0029]:[ICUMFW_UD_L_FLS_0029]
* Function Name : l_FLS_GetDfProgReq_ram
* Description   : Get the flag to detect if the PE side was in DF_PE mode.
* Arguments     : p_usr_df_prog_req
*                   get value saving address
* Return Value  :   ER_OK
*                   ER_PARAM
********************************************************************************************/
static uint32_t l_FLS_GetDfProgReq_ram(uint8_t *p_usr_df_prog_req)
{
    uint32_t lul_fls_status;

    /* Parmeter Check   */
    /* data address NULL */
    if (NULL == p_usr_df_prog_req)
    {
        lul_fls_status = ER_PARAM;
    }
    else
    {
        (void)D_FLS_GetDfProgReq_ram(p_usr_df_prog_req);
        lul_fls_status = ER_OK;
    }

    return (lul_fls_status);
}

/*******************************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0030]:[ICUMFW_UD_L_FLS_0030]
* Function Name : l_FLS_SetDfProgReq_ram
* Description   : Set the flag to detect if the PE side was in DF_PE mode.
* Arguments     : usr_df_prog_req
*                   setting value
* Return Value  :   ER_OK
*                   ER_PARAM
********************************************************************************************/
static uint32_t l_FLS_SetDfProgReq_ram(const uint8_t usr_df_prog_req)
{
    uint32_t lul_fls_status;
    uint8_t  luc_usr_df_prog_req;

    lul_fls_status = ER_OK;
    /* Parmeter Check   */
    switch ((uint32_t)usr_df_prog_req)
    {
        case (uint32_t)AG_L_FLS_DF_PE_SET:
            luc_usr_df_prog_req = AG_D_FLS_DF_PE_SET;
            break;
        case (uint32_t)AG_L_FLS_DF_PE_CLR:
            luc_usr_df_prog_req = AG_D_FLS_DF_PE_CLR;
            break;
        default:
            lul_fls_status = ER_PARAM;
            break;
    }

    if (ER_OK == lul_fls_status)
    {
        (void)D_FLS_SetDfProgReq_ram(luc_usr_df_prog_req);
    }
    else
    {
        /* do nothing */
    }

    return (lul_fls_status);
}

/*******************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0031]:[ICUMFW_UD_L_FLS_0031]
* Function Name : l_FLS_ReadData_ram
* Description   : Read to the flash area.
* Arguments     : target_addr
*                  Byte offset position of the area to be read.
*                   (offset specification is specified from the beginning of the
*                    bank when bank specification is valid)
*                 data_size_in_bytes
*                  read size.(in 4-byte units)
*                 p_data
*                  read data buffer.(4byte boundary)
* Return Value  : none
********************************************************************************/
static void l_FLS_ReadData_ram(const uint32_t target_addr, const uint32_t data_size_in_bytes, uint8_t *p_data)
{
    volatile uint32_t *p_lp_raddr;
    volatile uint32_t *p_lp_rdata;
    uint32_t           lul_index;

    p_lp_raddr = (volatile uint32_t *)target_addr;
    p_lp_rdata = (volatile uint32_t *)p_data;
    /* data read */
    for (lul_index = 0UL; lul_index < data_size_in_bytes; lul_index += L_FLS_ALIGN_SIZE)
    {
        *p_lp_rdata = *p_lp_raddr;
        p_lp_rdata++;
        p_lp_raddr++;
    }
    return;
}

/*******************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0032]:[ICUMFW_UD_L_FLS_0032]
* Function Name : l_FLS_ChangeErrCode_ram
* Description   : convert driver error code to library error code.
* Arguments     : d_err_code
*                  driver error code
* Return Value  :   ER_OK
*                   ER_PENDING
*                   ER_PARAM
*                   ER_IN_PROCESS
*                   ER_L_FLS_FLOW
*                   ER_L_FLS_PROTECTION
*                   ER_L_FLS_REJECTED
*                   ER_L_FLS_PROGRAM
*                   ER_L_FLS_CANCEL
*                   ER_L_FLS_INTERNAL
*                   ER_L_FLS_ECC1EDF1
*                   ER_L_FLS_ECC2EDF1
*                   ER_L_FLS_UNAVAILABLE
*                   ER_L_FLS_FACI_TIME_OUT
*                   ER_L_FLS_PROGRAM
********************************************************************************/
static uint32_t l_FLS_ChangeErrCode_ram(const uint32_t d_err_code)
{
    uint32_t lul_l_err_code;

    switch (d_err_code)
    {
        /* Driver Err */
        case ER_D_FLS_CANCEL:
            lul_l_err_code = ER_L_FLS_CANCEL;
            break;
        case ER_D_FLS_PROTECTION:
            lul_l_err_code = ER_L_FLS_PROTECTION;
            break;
        case ER_D_FLS_REJECTED:
            lul_l_err_code = ER_L_FLS_REJECTED;
            break;
        case ER_D_FLS_WRITE:
            lul_l_err_code = ER_L_FLS_PROGRAM;
            break;
        case ER_D_FLS_ERASE:
            lul_l_err_code = ER_L_FLS_PROGRAM;
            break;
        case ER_D_FLS_INTERNAL:
            lul_l_err_code = ER_L_FLS_INTERNAL;
            break;
        case ER_D_FLS_FSTOPTOUT:
            lul_l_err_code = ER_L_FLS_INTERNAL;
            break;
        case ER_D_FLS_SETOPRDYTOUT:
            lul_l_err_code = ER_L_FLS_INTERNAL;
            break;
        case ER_D_FLS_SETOPMODTOUT:
            lul_l_err_code = ER_L_FLS_UNAVAILABLE;
            break;
        case ER_D_FLS_UNAVAILABLE:
            lul_l_err_code = ER_L_FLS_UNAVAILABLE;
            break;
        case ER_D_FLS_ECC1EDF1:
            lul_l_err_code = ER_L_FLS_ECC1EDF1;
            break;
        case ER_D_FLS_ECC2EDF1:
            lul_l_err_code = ER_L_FLS_ECC2EDF1;
            break;
        case ER_D_FLS_FACITOUT:
            lul_l_err_code = ER_L_FLS_FACI_TIME_OUT;
            break;
        case ER_D_FLS_SUSPEND:
            lul_l_err_code = ER_PENDING;
            break;
        case ER_D_FLS_RESUME:
            lul_l_err_code = ER_L_FLS_PROGRAM;
            break;
        case ER_D_FLS_REMAP:
            lul_l_err_code = ER_L_FLS_REMAP;
            break;

        /* If an error code of the following library is input, it is output as it is. */
        /* ER_OK - ER_L_FLS_UNAVAILABLE */
        case ER_OK:
        case ER_PARAM:
        case ER_PENDING:
        case ER_IN_PROCESS:
        case ER_L_FLS_FLOW:
        case ER_L_FLS_PROTECTION:
        case ER_L_FLS_REJECTED:
        case ER_L_FLS_PROGRAM:
        case ER_L_FLS_CANCEL:
        case ER_L_FLS_INTERNAL:
        case ER_L_FLS_ECC1EDF1:
        case ER_L_FLS_ECC2EDF1:
        case ER_L_FLS_UNAVAILABLE:
            lul_l_err_code = d_err_code;
            break;
        /* unknown error */
        default:
            lul_l_err_code = ER_L_FLS_INTERNAL;
            break;
    } /* end of switch (d_err_code) */

    return (lul_l_err_code);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_library.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_library.rodata")
/*******************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0011]:[ICUMFW_UD_L_FLS_0011]
* Function Name : L_FLS_ReadBlankCheckArea
* Description   : Paramater check for reading the blank check area.
* Arguments     : target_area
*                  target flash area.
*                 bank
*                  target bank.
*                 offset
*                  Byte offset position of the area to be read.
*                   (offset specification is specified from the beginning of the
*                    bank when bank specification is valid)
*                 data_size_in_bytes
*                  read size.(in 4-byte units)
*                 blkc_mode
*                  incremental mode of blank checking.
*                 p_result_status
*                  result of blank cheking.
* Return Value  :   ER_OK
                    ER_PARAM
********************************************************************************/
uint32_t L_FLS_ReadBlankCheckArea(const uint32_t target_area, const uint32_t bank, const uint32_t offset,
                                  const uint32_t data_size_in_bytes, const uint8_t blkc_mode, uint32_t *p_result_status)
{
    uint32_t lul_fls_status;
    uint32_t lul_raddr;
    uint8_t  arg_blkc_mode;

    /* Parameter Check   */
    lul_fls_status = ER_PARAM;
    lul_raddr      = L_FLS_ZERO_U32;

    /* not 4byte align ? */
    if (0UL != (offset & ALIGN_CHK_4))
    {
        /* Not 4 byte boundary */
    }
    /* data size is 4 byte unit ? */
    else if (0UL == data_size_in_bytes)
    {
        /* Invalid data size */
    }
    else if (0UL != (data_size_in_bytes & ALIGN_CHK_4))
    {
        /* Not 4 byte boundary */
    }
    /* target_area is data flash only */
    else if (AG_L_FLS_DATA != target_area)
    {
        /* Invalid target area */
    }
    else if ((uint32_t)FLS_DF_BANK_NUM <= bank)
    {
        /* Invalid bank */
    }
    else
    {
        /* Address check */
        lul_fls_status = D_FLS_CheckAddr_ram(target_area, bank, offset, data_size_in_bytes, &lul_raddr);
        if (ER_OK != lul_fls_status)
        {
            /* Address error */
        }
        else
        {
            lul_fls_status = ER_OK;
        }
    }

    if (ER_OK != lul_fls_status)
    {
        /* do nothing */
    }
    /* Incremental mode */
    else if ((uint32_t)L_FLS_BLKCA_INC == (uint32_t)blkc_mode)
    {
        arg_blkc_mode = D_FLS_BLKCA_INC;
    }
    else if ((uint32_t)L_FLS_BLKCA_DEC == (uint32_t)blkc_mode)
    {
        arg_blkc_mode = D_FLS_BLKCA_DEC;
    }
    else
    {
        /* Invalid mode */
        lul_fls_status = ER_PARAM;
    }

    if (ER_OK != lul_fls_status)
    {
        /* do nothing */
    }
    else
    {
        /* check blank flag */
        lul_fls_status = D_FLS_CheckBlankFlag(lul_raddr, data_size_in_bytes, arg_blkc_mode, p_result_status);

        /* convert driver result status to library result status. */
        if (ST_D_FLS_BLANK == *p_result_status)
        {
            *p_result_status = ST_L_FLS_BLANK;
        }
        else
        {
            *p_result_status = ST_L_FLS_NON_BLANK;
        }

        /* convert driver error code to library error code. */
        lul_fls_status = l_FLS_ChangeErrCode_ram(lul_fls_status);
    }

    return (lul_fls_status);
}

/* self copy icum_library_rom.text to icum_library_ram   */
CHANGE_SEC(text, ".icum_library_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_library_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_L_FLS_0026]:[ICUMFW_UD_L_FLS_0026]
* Function Name : L_FLS_CheckContext_ram
* Description   : Check context Id of current task.
* Arguments     : uint32_t target_area
*                  target area
*                 uint32_t bank
*                  target bank
* Return Value  : ER_OK
*                 ER_PENDING
*                 ER_PARAM
*********************************************************************************/
uint32_t L_FLS_CheckContext_ram(const uint16_t context_id, const uint32_t target_area, const uint32_t bank)
{
    uint32_t err;

    err = D_FLS_CheckContext_ram(context_id, target_area, bank);

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0027]:[ICUMFW_UD_L_FLS_0027]
* Function name: L_FLS_CheckUserCFBoundary
* Description  : Check of memory source address for code flash bank
* Arguments    : uint32_t addr
*                  address to be checked
*                uint32_t size
*                  size to be checked
*                uint32_t *p_bank
*                  code flash bank
*                uint32_t *p_offset
*                  differnce between addr and base_addr
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t L_FLS_CheckUserCFBoundary(const uint32_t addr, const uint32_t size, uint32_t *p_bank, uint32_t *p_offset)
{
    uint32_t lul_l_err_code;

    lul_l_err_code = D_FLS_CheckUserCFBoundary(addr, size, p_bank, p_offset);

    return (lul_l_err_code);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_library.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_library.rodata")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0028]:[ICUMFW_UD_L_FLS_0028]
* Function name: L_FLS_CheckUserDFBoundary
* Description  : Check of memory source address for code flash bank
* Arguments    : uint32_t addr
*                 address to be checked
*                uint32_t size
*                  size to be checked
*                uint32_t *p_bank
*                  data flash bank
*                uint32_t *p_offset
*                  differnce between addr and base_addr
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t L_FLS_CheckUserDFBoundary(const uint32_t addr, const uint32_t size, uint32_t *p_bank, uint32_t *p_offset)
{
    uint32_t lul_l_err_code;

    lul_l_err_code = D_FLS_CheckUserDFBoundary(addr, size, p_bank, p_offset);

    return (lul_l_err_code);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0012]:[ICUMFW_UD_L_FLS_0012]
* Function name: L_FLS_RetrieveIcumRacRad
* Description  : retrieve the address boundary of the Secure Code Flash & Secure Data Flash
* Arguments    : none
* Return Value : none
***************************************************************************************/
void L_FLS_RetrieveIcumRacRad(void)
{
    static uint32_t icum_racs0;
    static uint32_t icum_race0;
    static uint32_t icum_racs1;
    static uint32_t icum_race1;
    static uint32_t icum_rad;

    icum_racs0 = L_FLS_INIT_ADDR;
    icum_race0 = L_FLS_INIT_ADDR;
    icum_racs1 = L_FLS_INIT_ADDR;
    icum_race1 = L_FLS_INIT_ADDR;
    icum_rad  = L_FLS_INIT_ADDR;

    /* get icumrac & icumrad */
    (void)L_FLS_ReadFlash_ram(AG_L_FLS_SECURITY,
                              0u,
                              SECURITY_SETTING_OFFSET_ICUMRACS0,
                              sizeof(uint32_t),
                              (uint8_t *)&icum_racs0);
    (void)L_FLS_ReadFlash_ram(AG_L_FLS_SECURITY,
                              0u,
                              SECURITY_SETTING_OFFSET_ICUMRACE0,
                              sizeof(uint32_t),
                              (uint8_t *)&icum_race0);
    (void)L_FLS_ReadFlash_ram(AG_L_FLS_SECURITY,
                              0u,
                              SECURITY_SETTING_OFFSET_ICUMRACS1,
                              sizeof(uint32_t),
                              (uint8_t *)&icum_racs1);
    (void)L_FLS_ReadFlash_ram(AG_L_FLS_SECURITY,
                              0u,
                              SECURITY_SETTING_OFFSET_ICUMRACE1,
                              sizeof(uint32_t),
                              (uint8_t *)&icum_race1);
    (void)L_FLS_ReadFlash_ram(AG_L_FLS_SECURITY,
                              0u,
                              SECURITY_SETTING_OFFSET_ICUMRAD,
                              sizeof(uint32_t),
                              (uint8_t *)&icum_rad);

    /* set icum lower or upper limit address */
    D_FLS_SetIcumRacRad(&icum_racs0, &icum_race0, &icum_racs1, &icum_race1, &icum_rad);
}

/* self copy icum_library_rom.text to icum_library_ram   */
CHANGE_SEC(text, ".icum_library_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_library_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0013]:[ICUMFW_UD_L_FLS_0013]
* Function name: L_FLS_GetMemBoundary
* Description  : retrieve the address boundary of the memory region specified
* Arguments    : boundary
*                 The specified boundary
*                p_addr
*                 Retrieve the boundary address
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t L_FLS_GetMemBoundary(const uint8_t boundary, uint32_t *p_addr)
{
    uint32_t err;

    err = D_FLS_GetMemBoundary(boundary, p_addr);

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0014]:[ICUMFW_UD_L_FLS_0014]
* Function name: L_FLS_CheckMemory
* Description  : Check of memory address
* Arguments    : start_addr
*                 Address to be checked
*                size
*                 Data size
*                direction
*                 Direction to check
*                p_result
*                 Check result
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t L_FLS_CheckMemory(const uint32_t start_addr, const uint32_t size, const uint8_t direction, uint8_t *p_result)
{
    uint32_t err;

    err = D_FLS_CheckMemory(start_addr, size, direction, p_result);

    return (err);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_library.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_library.rodata")

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0018]:[ICUMFW_UD_L_FLS_0018]
* Function name: L_FLS_CheckMemoryUserBoot
* Description  : Check of memory address
* Arguments    : start_addr     : Address to be checked
*                size           : Data size
*                direction      : Direction to check
*                p_result       : Check result
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t L_FLS_CheckMemoryUserBoot(const uint32_t start_addr, const uint32_t size, const uint8_t direction,
                                   uint8_t *p_result)
{
    uint32_t err;

    err = D_FLS_CheckMemoryUserBoot(start_addr, size, direction, p_result);

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0019]:[ICUMFW_UD_L_FLS_0019]
* Function name: L_FLS_CheckMemoryCISMUserBoot
* Description  : Check of memory address
* Arguments    : start_addr     : Address to be checked
*                size           : Data size
*                direction      : Direction to check
*                p_result       : Check result
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t L_FLS_CheckMemoryCISMUserBoot(const uint32_t start_addr, const uint32_t size, const uint8_t direction,
                                       uint8_t *p_result)
{
    uint32_t err;

    err = D_FLS_CheckMemoryCISMUserBoot(start_addr, size, direction, p_result);

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0022]:[ICUMFW_UD_L_FLS_0022]
* Function name: L_FLS_IsUserBootArea
* Description  : Check of memory address for UserBoot or not
* Arguments    : start_addr     : Address to be checked
*                size           : Data size
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t L_FLS_IsUserBootArea(const uint32_t start_addr, const uint32_t size)
{
    uint32_t err;

    err = D_FLS_IsUserBootArea(start_addr, size);

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0015]:[ICUMFW_UD_L_FLS_0015]
* Function name: L_FLS_CheckMemoryCISM
* Description  : Check of memory address for CISM
* Arguments    : start_addr
*                 Address to be checked
*                size
*                 Data size
*                direction
*                 Direction to check
*                p_result
*                 Check result
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t L_FLS_CheckMemoryCISM(const uint32_t start_addr, const uint32_t size, const uint8_t direction,
                               uint8_t *p_result)
{
    uint32_t err;

    err = D_FLS_CheckMemoryCISM(start_addr, size, direction, p_result);

    return (err);
}

/* self copy icum_library_rom.text to icum_library_ram   */
CHANGE_SEC(text, ".icum_library_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_library_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0017]:[ICUMFW_UD_L_FLS_0017]
* Function name: L_FLS_CheckSCFBoundary
* Description  : Check of memory source address for code flash bank
* Arguments    : cf_addr
*                 Target CF Address
*                p_bank
*                 Target bank number (The caller must guarantee not NULL)
*                p_offset
*                 Offset from the base address (The caller must guarantee not NULL)
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t L_FLS_CheckSCFBoundary(const uint32_t cf_addr, const uint32_t size, uint32_t *p_bank, uint32_t *p_offset)
{
    uint32_t err;

    err = D_FLS_CheckSCFBoundary(cf_addr, size, p_bank, p_offset);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0023]:[ICUMFW_UD_L_FLS_0023]
* Function name: L_FLS_CheckRamExecRequired
* Description  : Checking RAM execution (D_FLS::D_FLS_CheckRamExecRequired() wrapper function)
* Arguments    : uint32_t target_area
*                 target flash area (AG_L_FLS_CODE)
*                uint32_t addr
*                 target address
*                uint8_t *p_result
*                 RAM execution required = true, unnecessary = false
* Return Value : ER_OK
*                ER_PARAM
*****************************************************************************************/
uint32_t L_FLS_CheckRamExecRequired(const uint32_t target_area, const uint32_t addr, uint8_t *p_result)
{
    uint32_t err;

    err = D_FLS_CheckRamExecRequired(target_area, addr, p_result);

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0024]:[ICUMFW_UD_L_FLS_0024]
* Function name: L_FLS_SetFACIStatus_ram
* Description  : Set the FACIACC flag
* Arguments    : N/A
* Return Value : ER_OK
*                ER_PARAM
*                ER_L_FLS_UNAVAILBLE
*****************************************************************************************/
uint32_t L_FLS_SetFACIStatus_ram(void)
{
    uint32_t    err;
    uint8_t     faci_status;

    /* Check the status of the FACIACCDIS flag */
    err = D_COMM_GetPEStatusInfo(&faci_status);
    if (ER_OK == err)
    {
        if ((uint32_t)ICUM_COMM_STS_FACIACCDIS != ((uint32_t)faci_status & (uint32_t)ICUM_COMM_STS_FACIACCDIS))
        {
            /* Set FACIACC flag */
            err = D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_FACIACC, L_FLS_SET_U32);
            if (L_FLS_REF_CNT_MAX > (uint32_t)g_fls_faci_ref_count)
            {
                g_fls_faci_ref_count = (uint8_t)((uint32_t)g_fls_faci_ref_count + 1uL);
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            err = ER_L_FLS_UNAVAILABLE;
        }
    }
    else
    {
        /* do nothing */
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0025]:[ICUMFW_UD_L_FLS_0025]
* Function name: L_FLS_ClrFACIStatus_ram
* Description  : Reset the FACIACC flag
* Arguments    : N/A
* Return Value : ER_OK
*****************************************************************************************/
uint32_t L_FLS_ClrFACIStatus_ram(void)
{
    if (0uL < (uint32_t)g_fls_faci_ref_count)
    {
        g_fls_faci_ref_count = (uint8_t)((uint32_t)g_fls_faci_ref_count - 1uL);
    }
    else
    {
        /* do nothing */
    }
    if (0uL == (uint32_t)g_fls_faci_ref_count)
    {
        /* Reset FACIACC flag */
        (void)D_COMM_SetICUStatusInfo(ICUM_COMM_INF_ICU_ST_FACIACC, L_FLS_CLR_U32);
    }
    else
    {
        /* do nothing */
    }

    return (ER_OK);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_library.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_library.rodata")

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0020]:[ICUMFW_UD_L_FLS_0020]
* Function name: L_FLS_RemapStart
* Description  : Remap area development
* Arguments    : (IN) p_uiTargetAddr : Remap target data address
*                (OUT) p_uiRemapAddr : Remapped address
* Return Value : ER_OK
*                ER_L_FLS_REMAP
***************************************************************************************/
uint32_t L_FLS_RemapStart(const uint32_t p_uiTargetAddr, uint32_t **p_uiRemapAddr)
{
    uint32_t err;
    uint32_t ret_val;

    err     = D_FLS_RemapStart(p_uiTargetAddr, p_uiRemapAddr);
    ret_val = l_FLS_ChangeErrCode_ram(err);

    return ret_val;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_FLS_0021]:[ICUMFW_UD_L_FLS_0021]
* Function name: L_FLS_RemapEnd
* Description  : Initialization Remap Area
* Arguments    : None
* Return Value : None
***************************************************************************************/
void L_FLS_RemapEnd(void)
{
    D_FLS_RemapEnd();
}
