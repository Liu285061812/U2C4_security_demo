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
*********************************************************************************/
#include ICUM_DEVICE_FILE

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
*********************************************************************************/
/* self copy icum_driver_rom.text to icum_driver_ram.text   */
CHANGE_SEC(text, ".icum_driver_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")

CHANGE_SEC(data, ".icum_driver.data")
CHANGE_SEC(bss, ".icum_driver.bss")

/********************************************************************************
 Macro definitions
*********************************************************************************/

/********************************************************************************
 Typedef definitions
*********************************************************************************/

/********************************************************************************
 Exported global variables (to be accessed by other files)
*********************************************************************************/

/********************************************************************************
 Private variables
*********************************************************************************/

/********************************************************************************
 Private global variables
*********************************************************************************/

/********************************************************************************
 Private function prototypes
*********************************************************************************/
STATIC uint32_t d_FLS_PreProcessingCf_ram(void);
STATIC uint32_t d_FLS_GetCfEraseWriteSize_ram(const uint32_t flash_addr, uint32_t *p_size);

/********************************************************************************
 API Function definition
*********************************************************************************/
/*************************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0050]:[ICUMFW_UD_U2B_D_FLS_0050]
* Function Name : D_FLS_ProgramCf_ram
* Description   : Request write to the code flash area.
* Arguments     : flash_addr
*                   Start address of the data flash area to be written (256-byte boundary).
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
*                   ER_D_FLS_FACITOUT
*                   ER_D_FLS_NON_BLANK
*                   ER_D_FLS_PROTECTION
*                   ER_D_FLS_WRITE
*                   ER_D_FLS_ERASE
**************************************************************************************************/
uint32_t D_FLS_ProgramCf_ram(const uint32_t flash_addr, const uint8_t *p_data, const uint32_t data_size_in_bytes)
{
    uint32_t    lul_fls_status;
    fls_ctrl_t  *p_fls_ctrl;

    /* FACI number setteing */
    (void)d_FLS_SelectFpsys_ram(flash_addr);
    p_fls_ctrl = &g_fls_ctrl[g_fls_selected_fpsys];

    /* control variable setting */
    p_fls_ctrl->prm.d_status                 = ER_D_FLS_REQUEST;
    p_fls_ctrl->prm.ope_mode                 = (uint16_t)R_RFD_MODE_CFPE;
    p_fls_ctrl->prm.commad_req               = D_FLS_CMDREQ_ON;
    p_fls_ctrl->prm.command_code             = D_FACI_CMD_PROGRAM;
    p_fls_ctrl->prm.flash_addr               = flash_addr;
    p_fls_ctrl->prm.p_data                   = p_data;
    p_fls_ctrl->prm.data_size_in_bytes       = data_size_in_bytes;
    p_fls_ctrl->prm.p_pre_processing         = &d_FLS_PreProcessingCf_ram;
    p_fls_ctrl->prm.p_faci_commad_set        = &d_FLS_IssueCommand_ram;
    p_fls_ctrl->prm.p_write_size_in_bytes    = &D_FLS_GetCfProgramWriteSize_ram;
    p_fls_ctrl->prm.p_error_check            = &d_FLS_CheckFaciErrors_ram;
    p_fls_ctrl->prm.success_resp             = ER_OK;
    p_fls_ctrl->prm.p_num                    = D_FLS_ZERO_U32;

    /* Set Max FACI command time + Margin time */
    p_fls_ctrl->prm.max_time                 = TIMEOUT_PROGRAM_CF;

    lul_fls_status = d_FLS_Handler_ram(g_fls_selected_fpsys);

    return (lul_fls_status);
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0051]:[ICUMFW_UD_U2B_D_FLS_0051]
* Function Name : D_FLS_EraseCf_ram
* Description   : Request erase of code flash area.
* Arguments     : flash_addr
*                   Start address of the code flash area to be erase.
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
*                   ER_D_FLS_FACITOUT
*                   ER_D_FLS_NON_BLANK
*                   ER_D_FLS_PROTECTION
*                   ER_D_FLS_WRITE
*                   ER_D_FLS_ERASE
*******************************************************************************************/
uint32_t D_FLS_EraseCf_ram(const uint32_t flash_addr, const uint32_t data_size_in_bytes)
{
    uint32_t    lul_fls_status;
    fls_ctrl_t  *p_fls_ctrl;

    /* FACI number setteing */
    (void)d_FLS_SelectFpsys_ram(flash_addr);
    p_fls_ctrl = &g_fls_ctrl[g_fls_selected_fpsys];

    /* control variable setting */
    p_fls_ctrl->prm.d_status                 = ER_D_FLS_REQUEST;
    p_fls_ctrl->prm.ope_mode                 = (uint16_t)R_RFD_MODE_CFPE;
    p_fls_ctrl->prm.commad_req               = D_FLS_CMDREQ_ON;
    p_fls_ctrl->prm.command_code             = D_FACI_CMD_BLOCK_ERASE;
    p_fls_ctrl->prm.flash_addr               = flash_addr;
    p_fls_ctrl->prm.p_data                   = NULL;
    p_fls_ctrl->prm.data_size_in_bytes       = data_size_in_bytes;
    p_fls_ctrl->prm.p_pre_processing         = &d_FLS_PreProcessingCf_ram;
    p_fls_ctrl->prm.p_faci_commad_set        = &d_FLS_IssueCommand_ram;
    p_fls_ctrl->prm.p_write_size_in_bytes    = &d_FLS_GetCfEraseWriteSize_ram;
    p_fls_ctrl->prm.p_error_check            = &d_FLS_CheckFaciErrors_ram;
    p_fls_ctrl->prm.success_resp             = ER_OK;
    p_fls_ctrl->prm.p_num                    = D_FLS_ZERO_U32;

    /* Set Max FACI command time + Margin time */
    p_fls_ctrl->prm.max_time                 = TIMEOUT_ERASE_CF;

    lul_fls_status = d_FLS_Handler_ram(g_fls_selected_fpsys);

    return (lul_fls_status);
}

/********************************************************************************
 private Function definition
*********************************************************************************/
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0052]:[ICUMFW_UD_U2B_D_FLS_0052]
* Function Name : d_FLS_PreProcessingCf_ram
* Description   : Preprocessing of PE operation
* Arguments     : none
* Return Value  :   FLS_ERR_NONE
*********************************************************************************/
STATIC uint32_t d_FLS_PreProcessingCf_ram(void)
{

    return (FLS_ERR_NONE);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0053]:[ICUMFW_UD_U2B_D_FLS_0053]
* Function Name : D_FLS_GetCfEraseBlockSize_ram
* Description   : get flash erase block size (byte)
* Arguments     : flash_addr
*                   code flash address
*               : *p_size
*                   erase block size
* Return Value  :   ER_OK : No error detected
*********************************************************************************/
uint32_t D_FLS_GetCfEraseBlockSize_ram(const uint32_t flash_addr, uint32_t *p_size)
{
    /* block 0-7 : 16[Kbyte]  */
    /* block 8-  : 64[Kbyte] */

    if ((CF_BOOT_AREA_SIZE_BANKA_END > flash_addr) ||
        ((CF_BOOT_AREA_SIZE_BANKB_START <= flash_addr) && (CF_BOOT_AREA_SIZE_BANKB_END > flash_addr)))
    {
        *p_size = CF_ERASE_BLOCK_SIZE_BOOT;
    }
    else
    {
        *p_size = CF_ERASE_BLOCK_SIZE;
    }

    return (ER_OK);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0054]:[ICUMFW_UD_U2B_D_FLS_0054]
* Function Name : d_FLS_GetCfEraseWriteSize_ram
* Description   : get flash erase write size (byte)
* Arguments     : flash_addr
*                   code flash address(dummy not use)
*               : *p_size
*                   erase size
* Return Value  :   FLS_ERR_NONE : No error detected
*********************************************************************************/
STATIC uint32_t d_FLS_GetCfEraseWriteSize_ram(const uint32_t flash_addr, uint32_t *p_size)
{
    (void)D_FLS_GetCfEraseBlockSize_ram(flash_addr, p_size);

    return (FLS_ERR_NONE);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0055]:[ICUMFW_UD_U2B_D_FLS_0055]
* Function Name : D_FLS_GetCfProgramWriteSize_ram
* Description   : get flash program write size (byte)
* Arguments     : flash_addr
*                   code flash address(dummy not use)
*               : *p_size
*                   write size
* Return Value  :   ER_OK : No error detected
*********************************************************************************/
uint32_t D_FLS_GetCfProgramWriteSize_ram(UNUSED const uint32_t flash_addr, uint32_t *p_size)
{
    if (D_FLS_CF_BANKA_START_ADDRESS <= flash_addr)
    {
        /* Normal */
    }
    else
    {
        /* Abnormal */
    }

    *p_size = CF_WBUFFER_SIZE;

    return (ER_OK);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0109]:[ICUMFW_UD_U2B_D_FLS_0109]
* Function Name : D_FLS_CfPreLoad
* Description   : Code Flash Preload Function Enable
* Arguments     : status
*                   Flash preload enable/disable
* Return Value  : none
*********************************************************************************/
inline void D_FLS_CfPreLoad(const uint32_t status)
{
    ICUM_FCCTL.ICUM_FLPLCTL.UINT32 = status;
    /* Synchronize memory */
    __SYNCM();

    return;
}

