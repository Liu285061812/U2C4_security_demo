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
Includes <System Includes> , Project Includes
********************************************************************************/
#include "r_icumif_api.h"
#include "renesas_types.h"
#include "icum_d_misc_pub.h"
#include "icum_d_flash_pub.h"
#include "icum_d_flash.h"
#include "icum_d_ost_pub.h"

#include "r_rfd_constant.h"
#include "r_rfd_common.h"
#include "r_rfd_df.h"
/********************************************************************************
 QAC Suppression message list
*********************************************************************************/

/********************************************************************************
 Memory allocation section specification
********************************************************************************/
/* Code executed on ROM             */
CHANGE_SEC(text, ".icum_driver.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
CHANGE_SEC(data, ".icum_driver.data")
CHANGE_SEC(bss, ".icum_driver.bss")


/********************************************************************************
 Macro definitions
********************************************************************************/

/********************************************************************************
 Typedef definitions
********************************************************************************/

/********************************************************************************
 Exported global variables (to be accessed by other files)
********************************************************************************/

/********************************************************************************
 Private global variables
********************************************************************************/

/********************************************************************************
 Private function prototypes
********************************************************************************/
STATIC uint32_t d_FLS_PreProcessingDf(void);
STATIC uint32_t d_FLS_GetDfEraseWriteSize(UNUSED const uint32_t flash_addr, uint32_t *p_size);
STATIC uint32_t d_FLS_GetDfMultiProgramSize(UNUSED const uint32_t flash_addr, uint32_t *p_size);

/********************************************************************************
 API Function definition
*********************************************************************************/
/*************************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0056]:[ICUMFW_UD_U2B_D_FLS_0056]
* Function Name : D_FLS_ProgramDf
* Description   : Request write to the data flash area.
* Arguments     : flash_addr
*                   Start address of the data flash area to be written (4-byte boundary).
*                   It is specified by a relative address with the head address of the block 0
*                   of the flash set to 0x0000_0000.
*                 p_data
*                   data to be written.
*                 data_size_in_bytes
*                   Number of bytes to write (in 4-byte units).
* Return Value  :   ER_OK
*                   ER_IN_PROCESS
*                   ER_D_FLS_INTERNAL
*                   ER_D_FLS_SETOPRDYTOUT
*                   ER_D_FLS_REJECTED
*                   ER_D_FLS_SETOPMODTOUT
*                   ER_D_FLS_FSTOPTOUT
*                   ER_D_FLS_NON_BLANK
*                   ER_D_FLS_PROTECTION
*                   ER_D_FLS_WRITE
*                   ER_D_FLS_ERASE
*                   ER_D_FLS_FACITOUT
**************************************************************************************************/
uint32_t D_FLS_ProgramDf(const uint32_t flash_addr, const uint8_t *p_data, const uint32_t data_size_in_bytes)
{
    uint32_t    lul_fls_status;
    fls_ctrl_t  *p_fls_ctrl;

    /* FACI number setteing */
    (void)d_FLS_SelectFpsys_ram(flash_addr);
    p_fls_ctrl = &g_fls_ctrl[g_fls_selected_fpsys];

    /* control variable setting */
    p_fls_ctrl->prm.d_status               = ER_D_FLS_REQUEST;
    p_fls_ctrl->prm.ope_mode               = (uint16_t)R_RFD_MODE_DFPE;
    p_fls_ctrl->prm.commad_req             = D_FLS_CMDREQ_ON;
    if (DF_MULTIW8_SIZE <= (data_size_in_bytes - DF_WBUFFER_SIZE))
    {
        p_fls_ctrl->prm.command_code           = D_FACI_CMD_MULTI_PROGRAM;
        p_fls_ctrl->prm.p_write_size_in_bytes  = &d_FLS_GetDfMultiProgramSize;
    }
    else
    {
        p_fls_ctrl->prm.command_code           = D_FACI_CMD_PROGRAM;
        p_fls_ctrl->prm.p_write_size_in_bytes  = &D_FLS_GetDfProgramWriteSize;
    }
    p_fls_ctrl->prm.flash_addr             = flash_addr;
    p_fls_ctrl->prm.p_data                 = p_data;
    p_fls_ctrl->prm.data_size_in_bytes     = data_size_in_bytes;
    p_fls_ctrl->prm.p_pre_processing       = &d_FLS_PreProcessingDf;
    p_fls_ctrl->prm.p_faci_commad_set      = &d_FLS_IssueCommand_ram;
    p_fls_ctrl->prm.p_error_check          = &d_FLS_CheckFaciErrors_ram;
    p_fls_ctrl->prm.success_resp           = ER_OK;
    p_fls_ctrl->prm.p_num                  = D_FLS_ZERO_U32;

    /* Set Max FACI command time + Margin time */
    p_fls_ctrl->prm.max_time               = TIMEOUT_PROGRAM_DF;

    lul_fls_status = d_FLS_Handler_ram(g_fls_selected_fpsys);

    return (lul_fls_status);
}

/*********************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0057]:[ICUMFW_UD_U2B_D_FLS_0057]
* Function Name : D_FLS_EraseDf
* Description   : Request erase of data flash area.
* Arguments     : flash_addr
*                   Start address of the data flash area to be erase.
*                   Designated by physical address.
*                 data_size_in_bytes
*                   Number of bytes to erase (in 4-byte units).
* Return Value  :   ER_OK
*                   ER_IN_PROCESS
*                   ER_D_FLS_INTERNAL
*                   ER_D_FLS_SETOPRDYTOUT
*                   ER_D_FLS_REJECTED
*                   ER_D_FLS_SETOPMODTOUT
*                   ER_D_FLS_FSTOPTOUT
*                   ER_D_FLS_NON_BLANK
*                   ER_D_FLS_PROTECTION
*                   ER_D_FLS_WRITE
*                   ER_D_FLS_ERASE
*                   ER_D_FLS_FACITOUT
**********************************************************************************************/
uint32_t D_FLS_EraseDf(const uint32_t flash_addr, const uint32_t data_size_in_bytes)
{
    uint32_t    lul_fls_status;
    fls_ctrl_t  *p_fls_ctrl;

    /* FACI number setteing */
    (void)d_FLS_SelectFpsys_ram(flash_addr);
    p_fls_ctrl = &g_fls_ctrl[g_fls_selected_fpsys];

    /* control variable setting */
    p_fls_ctrl->prm.d_status               = ER_D_FLS_REQUEST;
    p_fls_ctrl->prm.ope_mode               = (uint16_t)R_RFD_MODE_DFPE;
    p_fls_ctrl->prm.commad_req             = D_FLS_CMDREQ_ON;
    p_fls_ctrl->prm.command_code           = D_FACI_CMD_BLOCK_ERASE;
    p_fls_ctrl->prm.flash_addr             = flash_addr;
    p_fls_ctrl->prm.p_data                 = NULL;
    p_fls_ctrl->prm.data_size_in_bytes     = data_size_in_bytes;
    p_fls_ctrl->prm.p_pre_processing       = &d_FLS_PreProcessingDf;
    p_fls_ctrl->prm.p_faci_commad_set      = &d_FLS_IssueCommand_ram;
    p_fls_ctrl->prm.p_write_size_in_bytes  = &d_FLS_GetDfEraseWriteSize;
    p_fls_ctrl->prm.p_error_check          = &d_FLS_CheckFaciErrors_ram;
    p_fls_ctrl->prm.success_resp           = ER_OK;
    p_fls_ctrl->prm.p_num                  = D_FLS_ZERO_U32;

    /* Set Max FACI command time + Margin time */
    p_fls_ctrl->prm.max_time               = TIMEOUT_ERASE_DF;

    lul_fls_status = d_FLS_Handler_ram(g_fls_selected_fpsys);

    return (lul_fls_status);
}

/********************************************************************************
 private Function definition
*********************************************************************************/
/******************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0058]:[ICUMFW_UD_U2B_D_FLS_0058]
* Function Name : d_FLS_PreProcessingDf
* Description   : Preprocessing of PE operation
* Arguments     : none
* Return Value  :   FLS_ERR_NONE
******************************************************************************/
STATIC uint32_t d_FLS_PreProcessingDf(void)
{
    /* No preprocessing */
    return (FLS_ERR_NONE);
}

/******************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0059]:[ICUMFW_UD_U2B_D_FLS_0059]
* Function Name : D_FLS_GetDfEraseBlockSize
* Description   : get flash erase block size (byte)
* Arguments     : flash_addr
*                   data flash address(dummy not use)
*               : *p_size
*                   erase size
* Return Value  :   ER_OK : No error detected
******************************************************************************/
uint32_t D_FLS_GetDfEraseBlockSize(UNUSED const uint32_t flash_addr, uint32_t *p_size)
{
    if (D_FLS_DF_BANKA_START_ADDRESS <= flash_addr)
    {
        /* Normal */
    }
    else
    {
        /* Abnormal */
    }

    *p_size = DF_ERASE_BLOCK_SIZE;

    return (ER_OK);
}

/******************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0061]:[ICUMFW_UD_U2B_D_FLS_0061]
* Function Name : D_FLS_GetDfProgramWriteSize
* Description   : get flash program write size (byte)
* Arguments     : flash_addr
*                   data flash address(dummy not use)
*               : *p_size
*                   write size
* Return Value  : uint32_t
*                   ER_OK : No error detected
******************************************************************************/
uint32_t D_FLS_GetDfProgramWriteSize(UNUSED const uint32_t flash_addr, uint32_t *p_size)
{
    if (D_FLS_DF_BANKA_START_ADDRESS <= flash_addr)
    {
        /* Normal */
    }
    else
    {
        /* Abnormal */
    }

    *p_size = DF_WBUFFER_SIZE;

    return (ER_OK);
}

/******************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0062]:[ICUMFW_UD_U2B_D_FLS_0062]
* Function Name : D_FLS_GetDfMultiProgramSize
* Description   : get flash multi program write size (byte)
* Arguments     : flash_addr
*                   data flash address(dummy not use)
*               : *p_size
*                   write size
* Return Value  : uint32_t
*                   ER_OK : No error detected
******************************************************************************/
STATIC uint32_t d_FLS_GetDfMultiProgramSize(UNUSED const uint32_t flash_addr, uint32_t *p_size)
{
    uint32_t lus_leftover_size;

    lus_leftover_size =
        g_fls_ctrl[g_fls_selected_fpsys].prm.data_size_in_bytes - g_fls_ctrl[g_fls_selected_fpsys].prm.p_num;
    lus_leftover_size -= DF_WBUFFER_SIZE;
    g_fls_ctrl[g_fls_selected_fpsys].prm.command_code = D_FACI_CMD_MULTI_PROGRAM;

    if ((DF_MULTIW128_SIZE <= lus_leftover_size) && ( ALIGN_CHK_OK == (flash_addr & ALIGN_CHK_128)))
    {
        *p_size = DF_MULTIW128_SIZE;
    }
    else if ((DF_MULTIW64_SIZE <= lus_leftover_size) && (ALIGN_CHK_OK == (flash_addr & ALIGN_CHK_64)))
    {
        *p_size = DF_MULTIW64_SIZE;
    }
    else if ((DF_MULTIW32_SIZE <= lus_leftover_size) && (ALIGN_CHK_OK == (flash_addr & ALIGN_CHK_32)))
    {
        *p_size = DF_MULTIW32_SIZE;
    }
    else if ((DF_MULTIW16_SIZE <= lus_leftover_size) && (ALIGN_CHK_OK == (flash_addr & ALIGN_CHK_16)))
    {
        *p_size = DF_MULTIW16_SIZE;
    }
    else if ((DF_MULTIW8_SIZE <= lus_leftover_size) && (ALIGN_CHK_OK == (flash_addr & ALIGN_CHK_8)))
    {
        *p_size = DF_MULTIW8_SIZE;
    }
    else
    {
        g_fls_ctrl[g_fls_selected_fpsys].prm.command_code = D_FACI_CMD_PROGRAM;
        *p_size = DF_WBUFFER_SIZE;
    }

    return (ER_OK);
}

/******************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0060]:[ICUMFW_UD_U2B_D_FLS_0060]
* Function Name : d_FLS_GetDfEraseWriteSize
* Description   : get flash erase write size (byte) dummy function
* Arguments     : flash_addr
*                   data flash address(dummy not use)
*               : *p_size
*                   erase size
* Return Value  :   FLS_ERR_NONE : No error detected
******************************************************************************/
STATIC uint32_t d_FLS_GetDfEraseWriteSize(UNUSED const uint32_t flash_addr, uint32_t *p_size)
{
    if (D_FLS_DF_BANKA_START_ADDRESS <= flash_addr)
    {
        /* Normal */
    }
    else
    {
        /* Abnormal */
    }

    *p_size = DF_ERASE_BLOCK_SIZE;

    return (FLS_ERR_NONE);
}

