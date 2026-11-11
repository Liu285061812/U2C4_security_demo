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
#include "r_icumif_api.h"
#include "renesas_types.h"
#include "icum_d_misc_pub.h"
#include "icum_d_flash_pub.h"
#include "icum_d_flash.h"
#include "icum_d_ost_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_l_toolbox_pub.h"
#include "r_rfd_common_types.h"

/********************************************************************************
 QAC Suppression message list
*********************************************************************************/

/********************************************************************************
 Memory allocation section specification
*********************************************************************************/
/* Code executed on ROM             */
CHANGE_SEC(text, ".icum_driver.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
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
 Private global variables
*********************************************************************************/
/* Save area for D_FLS_UpdateSecurity function parameters */
static s_FLS_Update_Sec_Prm_Type_t g_fls_update_sec_prm;

static uint32_t g_switch_area_valid_data[(uint32_t)SWITCH_AREA_PROGRAM_SIZE/sizeof(uint32_t)];

static uint8_t g_security_area_valid_data[(uint8_t)SECURITY_SETTING_PROGRAM_SIZE/sizeof(uint8_t)];

uint16_t g_sec_area_write_offset[D_FLS_MANDATORY_WRITE_COUNT] = 
{
    0x200U,
    0x220U,
    0x2E0U,
    0x300U,
    0x320U,
    0x340U,
    0x360U,
    0x380U,
    0x3A0U,
    0x3C0U,
    0x3E0U,
    0x400U,
    0x420U,
    0x440U
};

#pragma ghs section bss = ".start.icum_cram_guard"

uint32_t g_security_property_backup[D_FLS_MANDATORY_WRITE_COUNT][D_FLS_WRITE_COUNT];

#pragma ghs section bss = default
/********************************************************************************
 Private function prototypes
*********************************************************************************/
static uint32_t d_FLS_PreProcessingSec(void);
static uint32_t d_FLS_ProgramSecurity(void);
static uint32_t d_FLS_EraseSwitch(void);
static uint32_t d_FLS_ProgramSwitch(void);
static uint32_t d_FLS_EraseTAG(void);
static uint32_t d_FLS_UpdateTAG(void);
static uint32_t d_FLS_NoProcessing(void);
static uint32_t d_FLS_PreProcessingSwitch(void);
static uint32_t d_FLS_PreProcessingTAG(void);
static uint32_t d_FLS_GetEraseWriteSizeSec(UNUSED const uint32_t flash_addr, uint32_t *p_size);
static uint32_t d_FLS_GetEraseWriteSizeSwitch(UNUSED const uint32_t flash_addr, uint32_t *p_size);
static uint32_t d_FLS_GetWriteSizeSwitch(UNUSED const uint32_t flash_addr, uint32_t *p_size);
static uint32_t d_FLS_GetEraseWriteSizeTAG(UNUSED const uint32_t flash_addr, uint32_t *p_size);
static uint32_t d_FLS_GetWriteSizeTAG(UNUSED const uint32_t flash_addr, uint32_t *p_size);
static uint32_t d_FLS_IssueCommandSwProg(void);
static uint32_t d_FLS_CheckDfProgRes(uint32_t time_out_cnt);


/********************************************************************************
 Function definition
*********************************************************************************/

/********************************************************************************
 API Function definition
*********************************************************************************/

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0063]:[ICUMFW_UD_U2B_D_FLS_0063]
* Function Name : D_FLS_UpdateSecurity
* Description   : Update the security setting area.
* Arguments     : p_sec_area
*                  Start address of security buffer.
*                 size_sec_area
*                  Size of security setting area.
*                 p_updated_field
*                  Start address of the security buffer to be updated.
*                 size_field
*                  Size to update (in 4-byte units)
* Return Value  : ER_OK
*                 ER_IN_PROCESS
*                 ER_D_FLS_INTERNAL
*                 ER_D_FLS_SETOPRDYTOUT
*                 ER_D_FLS_SETOPMODTOUT
*                 ER_D_FLS_FSTOPTOUT
*                 ER_D_FLS_NON_BLANK
*                 ER_D_FLS_PROTECTION
*                 ER_D_FLS_WRITE
*                 ER_D_FLS_ERASE
*                 ER_D_FLS_REJECTED
*                 ER_D_FLS_FACITOUT
*********************************************************************************/
uint32_t D_FLS_UpdateSecurity(uint32_t target_addr, const uint8_t *p_updated_field, const uint32_t size_field)
{
    uint32_t    lul_fls_status;
    fls_ctrl_t  *p_fls_ctrl;
    uint32_t    valid_data_read_counter;
    uint32_t    read_count;
    uint32_t    *p_property;

    /* Save function parameters to g_fls_update_sec_prm */
    g_fls_update_sec_prm.target_addr     = target_addr +
                                          (D_FLS_SEC_SETTING_START_BACK_ADDRESS - D_FLS_SEC_SETTING_START_ADDRESS);
    g_fls_update_sec_prm.size_field      = size_field;

    /* Reads valid side information from the Security setting area write data */
    for (valid_data_read_counter = 0UL; (sizeof(g_security_area_valid_data) / sizeof(uint8_t)) > valid_data_read_counter; valid_data_read_counter++)
    {
        g_security_area_valid_data[valid_data_read_counter] = *(p_updated_field + valid_data_read_counter);
    }

    /* Reads valid side information from the Switch area (valid area) */
    for (valid_data_read_counter = 0UL; (sizeof(g_switch_area_valid_data) / sizeof(uint32_t)) > valid_data_read_counter; valid_data_read_counter++)
    {
        g_switch_area_valid_data[valid_data_read_counter] = ((uint32_t *)SWITCH_AREA_VALID_VALID_INFO)[valid_data_read_counter];
    }

    /* Reads backup data from the Security setting area (valid area) */
    for (valid_data_read_counter = 0UL ; valid_data_read_counter < D_FLS_MANDATORY_WRITE_COUNT ; valid_data_read_counter++)
    {
        p_property = (uint32_t *)(SECURITY_SETTING_VALID_ST_ADDR + g_sec_area_write_offset[valid_data_read_counter]);
        for (read_count = 0UL ; read_count < D_FLS_WRITE_COUNT ; read_count++)
        {
            g_security_property_backup[valid_data_read_counter][read_count] = p_property[read_count];
        }
    }

    /* FACI number setteing */
    (void)d_FLS_SelectFpsys_ram(target_addr);
    p_fls_ctrl = &g_fls_ctrl[g_fls_selected_fpsys];

    /* control variable setting */
    p_fls_ctrl->prm.d_status                = ER_D_FLS_REQUEST;
    p_fls_ctrl->prm.ope_mode                = R_RFD_MODE_DFPE;

    p_fls_ctrl->prm.command_code            = D_FACI_CMD_SECURITY_ERASE;

    p_fls_ctrl->prm.flash_addr              = SECURITY_SETTING_INVAL_ST_ADDR;
    p_fls_ctrl->prm.commad_req              = D_FLS_CMDREQ_ON;
    p_fls_ctrl->prm.p_data                  = NULL;
    p_fls_ctrl->prm.data_size_in_bytes      = SECURITY_SETTING_ERASE_SIZE;
    p_fls_ctrl->prm.p_pre_processing        = &d_FLS_PreProcessingSec;
    p_fls_ctrl->prm.p_faci_commad_set       = &d_FLS_IssueCommand_ram;
    p_fls_ctrl->prm.p_write_size_in_bytes   = &d_FLS_GetEraseWriteSizeSec;
    p_fls_ctrl->prm.p_error_check           = &d_FLS_CheckFaciErrors_ram;
    p_fls_ctrl->prm.success_resp            = ER_OK;
    p_fls_ctrl->prm.p_num                   = 0UL;

    /* Set Max FACI command time + Margin time */
    p_fls_ctrl->prm.max_time                = TIMEOUT_ERASE_SEC;

    /* Set the address of the driver control parameter setting processing function */
    /* of the FACI command that is continuously requested                          */
    p_fls_ctrl->prm.p_set_ctrl_prm          = &d_FLS_ProgramSecurity;

    lul_fls_status = d_FLS_Handler_ram(g_fls_selected_fpsys);

    return (lul_fls_status);
}

/********************************************************************************
 private Function definition
*********************************************************************************/

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0064]:[ICUMFW_UD_U2B_D_FLS_0064]
* Function Name : d_FLS_PreProcessingSec
* Description   : Preprocessing of security setting write operation
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*                 ER_D_FLS_DFPRGINTTOUT
*                 ER_D_FLS_ACCPROHIBIT
*                 ER_D_FLS_INTERNAL
*********************************************************************************/
static uint32_t d_FLS_PreProcessingSec(void)
{
    uint32_t    lul_fls_status;
    uint32_t    boot_pe;
    uint32_t    bit_position;

    lul_fls_status = D_FLS_ZERO_U32;
    boot_pe = D_FLS_ZERO_U32;
    bit_position = D_FLS_ZERO_U32;

    /* Get the started PE */
    lul_fls_status = D_COMM_GetStartupPEInfo(&boot_pe);

    /* Perform processing for all started PEs */
    while ((D_FLS_ZERO_U32 != boot_pe) && (ER_OK == lul_fls_status))
    {
        /* judge if the PE has been started */
        if (D_FLS_PE_CHECK == (D_FLS_PE_CHECK & boot_pe))
        {
            /* Clear flash mode switching response */
            g_sec_df_prog_res = FLS_MODE_SW_RESP_CLR;
            /* Issues a Data Flash (FPSYS0) mode switching request interrupt to the PE */
            lul_fls_status = D_COMM_TriggerInterruptToPE((icum_comm_target_pe_t)bit_position,
                                                         ICUM_COMM_ICH_TO_DF_PRG_REQ_0, NULL);
            if (ER_OK == lul_fls_status)
            {
                /* Waiting for a response from the PE (with time monitoring) */
                lul_fls_status = d_FLS_CheckDfProgRes(D_FLS_TIMEOUT_DFPROGRES);
            }
            else
            {
                /* Do nothing */
            }

            if (ER_OK == lul_fls_status)
            {
                /* If the response interrupt is NACK, the processing ends */
                if ((uint8_t)FLS_MODE_SW_RESP_NACK == g_sec_df_prog_res)
                {
                    lul_fls_status = ER_D_FLS_ACCPROHIBIT;
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
        } /* end of if (D_FLS_PE_CHECK == (D_FLS_PE_CHECK & boot_pe)) */
        else
        {
            /* Do nothing */
        }
        /* To the next PE processing */
        boot_pe >>= D_FLS_SHIT_1BIT;
        bit_position++;
    }

    if (ER_OK == lul_fls_status)
    {
        /* Disable UPGP2 protection */
        lul_fls_status = D_FLS_SetUPGPProtect_ram(D_FLS_UPGP_ON, D_FLS_UPGP_OFF);
    }
    else
    {
        /* Do nothing */
    }

    switch (lul_fls_status)
    {
        case ER_OK:
            /* Set successful completion */
            lul_fls_status = FLS_ERR_NONE;
            break;
        case ER_D_FLS_DFPRGINTTOUT:
        case ER_D_FLS_ACCPROHIBIT:
            /* Do nothing */
            break;
        default:
            /* Set unexpected error */
            lul_fls_status = ER_D_FLS_INTERNAL;
            break;
    } /* end of for switch (lul_fls_status) */

    return (lul_fls_status);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0065]:[ICUMFW_UD_U2B_D_FLS_0065]
* Function Name : D_FLS_GetWriteSizeSec
* Description   : get security setting write size (byte)
* Arguments     : flash_addr
*                  security setting area address(dummy not use)
*               : *p_size
*                  write size
* Return Value  : ER_OK : No error detected
*********************************************************************************/
uint32_t D_FLS_GetWriteSizeSec(UNUSED const uint32_t flash_addr, uint32_t *p_size)
{
    *p_size = D_FLS_SEC_SETTING_SIZE;

    return (ER_OK);
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0066]:[ICUMFW_UD_U2B_D_FLS_0066]
* Function Name : d_FLS_ProgramSecurity
* Description   : Set the flash control parameters of security setting area write command
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*******************************************************************************************/
static uint32_t d_FLS_ProgramSecurity(void)
{
    fls_ctrl_t  *p_fls_ctrl;

    p_fls_ctrl = &g_fls_ctrl[g_fls_selected_fpsys];

    /* control variable setting */
    p_fls_ctrl->prm.d_status                = ER_D_FLS_REQUEST;
    p_fls_ctrl->prm.ope_mode                = R_RFD_MODE_DFPE;

    p_fls_ctrl->prm.command_code            = D_FACI_CMD_SECURITY_PROGRAM;

    p_fls_ctrl->prm.flash_addr              = g_fls_update_sec_prm.target_addr;
    p_fls_ctrl->prm.commad_req              = D_FLS_CMDREQ_ON;
    p_fls_ctrl->prm.p_data                  = (const uint8_t *)&g_security_area_valid_data;
    p_fls_ctrl->prm.data_size_in_bytes      = g_fls_update_sec_prm.size_field;
    p_fls_ctrl->prm.p_pre_processing        = &d_FLS_NoProcessing;
    p_fls_ctrl->prm.p_faci_commad_set       = &d_FLS_IssueCommand_ram;
    p_fls_ctrl->prm.p_write_size_in_bytes   = &D_FLS_GetWriteSizeSec;
    p_fls_ctrl->prm.p_error_check           = &d_FLS_CheckFaciErrors_ram;
    p_fls_ctrl->prm.success_resp            = ER_OK;
    p_fls_ctrl->prm.p_num                   = D_FLS_ZERO_U32;

    /* Set Max FACI command time + Margin time */
    p_fls_ctrl->prm.max_time                = TIMEOUT_PROGRAM_SEC;

    /* Set the address of the driver control parameter setting processing function */
    /* of the FACI command that is continuously requested                          */
    p_fls_ctrl->prm.p_set_ctrl_prm          = &d_FLS_EraseSwitch;

    return (FLS_ERR_NONE);
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0067]:[ICUMFW_UD_U2B_D_FLS_0067]
* Function Name : d_FLS_EraseSwitch
* Description   : Set the flash control parameters of switch area erase command
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*******************************************************************************************/
static uint32_t d_FLS_EraseSwitch(void)
{
    fls_ctrl_t  *p_fls_ctrl;

    p_fls_ctrl = &g_fls_ctrl[g_fls_selected_fpsys];

    /* control variable setting */
    p_fls_ctrl->prm.d_status                = ER_D_FLS_REQUEST;
    p_fls_ctrl->prm.ope_mode                = R_RFD_MODE_DFPE;

    p_fls_ctrl->prm.command_code            = D_FACI_CMD_SWITCH_ERASE;

    p_fls_ctrl->prm.flash_addr              = SWITCH_AREA_INVAL_ST_ADDR;
    p_fls_ctrl->prm.commad_req              = D_FLS_CMDREQ_ON;
    p_fls_ctrl->prm.p_data                  = NULL;
    p_fls_ctrl->prm.data_size_in_bytes      = SWITCH_AREA_ERASE_SIZE;
    p_fls_ctrl->prm.p_pre_processing        = &d_FLS_PreProcessingSwitch;
    p_fls_ctrl->prm.p_faci_commad_set       = &d_FLS_IssueCommand_ram;
    p_fls_ctrl->prm.p_write_size_in_bytes   = &d_FLS_GetEraseWriteSizeSwitch;
    p_fls_ctrl->prm.p_error_check           = &d_FLS_CheckFaciErrors_ram;
    p_fls_ctrl->prm.success_resp            = ER_OK;
    p_fls_ctrl->prm.p_num                   = D_FLS_ZERO_U32;

    /* Set Max FACI command time + Margin time */
    p_fls_ctrl->prm.max_time                = TIMEOUT_ERASE_SW;

    /* Set the address of the driver control parameter setting processing function */
    /* of the FACI command that is continuously requested                          */
    p_fls_ctrl->prm.p_set_ctrl_prm          = &d_FLS_ProgramSwitch;

    return (FLS_ERR_NONE);
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0068]:[ICUMFW_UD_U2B_D_FLS_0068]
* Function Name : d_FLS_ProgramSwitch
* Description   : Set the flash control parameters of switch area write command
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*******************************************************************************************/
static uint32_t d_FLS_ProgramSwitch(void)
{
    fls_ctrl_t  *p_fls_ctrl;

    p_fls_ctrl = &g_fls_ctrl[g_fls_selected_fpsys];

    /* control variable setting */
    p_fls_ctrl->prm.d_status                = ER_D_FLS_REQUEST;
    p_fls_ctrl->prm.ope_mode                = R_RFD_MODE_DFPE;

    p_fls_ctrl->prm.command_code            = D_FACI_CMD_SWITCH_PROGRAM;

    p_fls_ctrl->prm.flash_addr              = SWITCH_AREA_INVAL_VALID_INFO;
    p_fls_ctrl->prm.commad_req              = D_FLS_CMDREQ_ON;
    p_fls_ctrl->prm.p_data                  = NULL;
    p_fls_ctrl->prm.data_size_in_bytes      = SWITCH_AREA_PROGRAM_SIZE;
    p_fls_ctrl->prm.p_pre_processing        = &d_FLS_NoProcessing;
    p_fls_ctrl->prm.p_faci_commad_set       = &d_FLS_IssueCommandSwProg;
    p_fls_ctrl->prm.p_write_size_in_bytes   = &d_FLS_GetWriteSizeSwitch;
    p_fls_ctrl->prm.p_error_check           = &d_FLS_CheckFaciErrors_ram;
    p_fls_ctrl->prm.success_resp            = ER_OK;
    p_fls_ctrl->prm.p_num                   = D_FLS_ZERO_U32;

    /* Set Max FACI command time + Margin time */
    p_fls_ctrl->prm.max_time                = TIMEOUT_PROGRAM_SW;

    /* Set the address of the driver control parameter setting processing function */
    /* of the FACI command that is continuously requested                          */
    p_fls_ctrl->prm.p_set_ctrl_prm          = &d_FLS_EraseTAG;

    return (FLS_ERR_NONE);
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0069]:[ICUMFW_UD_U2B_D_FLS_0069]
* Function Name : d_FLS_EraseTAG
* Description   : Set the flash control parameters of TAG area erase command
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*******************************************************************************************/
static uint32_t d_FLS_EraseTAG(void)
{
    fls_ctrl_t  *p_fls_ctrl;

    p_fls_ctrl = &g_fls_ctrl[g_fls_selected_fpsys];

    /* control variable setting */
    p_fls_ctrl->prm.d_status                = ER_D_FLS_REQUEST;
    p_fls_ctrl->prm.ope_mode                = R_RFD_MODE_DFPE;

    p_fls_ctrl->prm.command_code            = D_FACI_CMD_TAG_ERASE;

    p_fls_ctrl->prm.flash_addr              = TAG_AREA_ST_ADDR;
    p_fls_ctrl->prm.commad_req              = D_FLS_CMDREQ_ON;
    p_fls_ctrl->prm.p_data                  = NULL;
    p_fls_ctrl->prm.data_size_in_bytes      = TAG_AREA_ERASE_SIZE;
    p_fls_ctrl->prm.p_pre_processing        = &d_FLS_PreProcessingTAG;
    p_fls_ctrl->prm.p_faci_commad_set       = &d_FLS_IssueCommand_ram;
    p_fls_ctrl->prm.p_write_size_in_bytes   = &d_FLS_GetEraseWriteSizeTAG;
    p_fls_ctrl->prm.p_error_check           = &d_FLS_CheckFaciErrors_ram;
    p_fls_ctrl->prm.success_resp            = ER_OK;
    p_fls_ctrl->prm.p_num                   = D_FLS_ZERO_U32;

    /* Set Max FACI command time + Margin time */
    p_fls_ctrl->prm.max_time                = TIMEOUT_ERASE_TAG;

    /* Set the address of the driver control parameter setting processing function */
    /* of the FACI command that is continuously requested                          */
    p_fls_ctrl->prm.p_set_ctrl_prm          = &d_FLS_UpdateTAG;

    return (FLS_ERR_NONE);
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0070]:[ICUMFW_UD_U2B_D_FLS_0070]
* Function Name : d_FLS_UpdateTAG
* Description   : Set the flash control parameters of TAG area update command
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*******************************************************************************************/
static uint32_t d_FLS_UpdateTAG(void)
{
    fls_ctrl_t  *p_fls_ctrl;

    p_fls_ctrl = &g_fls_ctrl[g_fls_selected_fpsys];

    /* control variable setting */
    p_fls_ctrl->prm.d_status                = ER_D_FLS_REQUEST;
    p_fls_ctrl->prm.ope_mode                = R_RFD_MODE_DFPE;

    p_fls_ctrl->prm.command_code            = D_FACI_CMD_TAG_UPDATE;

    p_fls_ctrl->prm.flash_addr              = TAG_AREA_VAF_ADDR;
    p_fls_ctrl->prm.commad_req              = D_FLS_CMDREQ_ON;
    p_fls_ctrl->prm.p_data                  = NULL;
    p_fls_ctrl->prm.data_size_in_bytes      = TAG_AREA_UPDATE_SIZE;
    p_fls_ctrl->prm.p_pre_processing        = &d_FLS_NoProcessing;
    p_fls_ctrl->prm.p_faci_commad_set       = &d_FLS_IssueCommand_ram;
    p_fls_ctrl->prm.p_write_size_in_bytes   = &d_FLS_GetWriteSizeTAG;
    p_fls_ctrl->prm.p_error_check           = &d_FLS_CheckFaciErrors_ram;
    p_fls_ctrl->prm.success_resp            = ER_OK;
    p_fls_ctrl->prm.p_num                   = D_FLS_ZERO_U32;

    /* Set Max FACI command time + Margin time */
    p_fls_ctrl->prm.max_time                = TIMEOUT_UPDATE_TAG;

    /* Set the address of the driver control parameter setting processing function */
    /* of the FACI command that is continuously requested                          */
    p_fls_ctrl->prm.p_set_ctrl_prm          = &D_FLS_PostProgramSec;

    return (FLS_ERR_NONE);
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0071]:[ICUMFW_UD_U2B_D_FLS_0071]
* Function Name : D_FLS_PostProgramSec
* Description   : Set the flash control parameters to complete of security area update
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*                 ER_D_FLS_INTERNAL
*******************************************************************************************/
uint32_t D_FLS_PostProgramSec(void)
{
    fls_ctrl_t  *p_fls_ctrl;
    uint32_t    lul_fls_status;
    uint32_t    boot_pe;
    uint32_t    bit_position;

    lul_fls_status = D_FLS_ZERO_U32;
    boot_pe = D_FLS_ZERO_U32;
    bit_position = D_FLS_ZERO_U32;

    p_fls_ctrl = &g_fls_ctrl[g_fls_selected_fpsys];

    /* control variable setting */
    p_fls_ctrl->prm.d_status                = ER_OK;
    p_fls_ctrl->prm.commad_req              = D_FLS_CMDREQ_OFF;
    p_fls_ctrl->prm.success_resp            = ER_OK;

    /* Set the address of the driver control parameter setting processing function */
    /* of the FACI command that is continuously requested                          */
    p_fls_ctrl->prm.p_set_ctrl_prm          = NULL;

    /* Enable UPGP2 protection */
    lul_fls_status = D_FLS_SetUPGPProtect_ram(D_FLS_UPGP_ON, D_FLS_UPGP_ON);

    if (ER_OK == lul_fls_status)
    {
        /* Get the started PE */
        lul_fls_status = D_COMM_GetStartupPEInfo(&boot_pe);
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK == lul_fls_status)
    {
        /* Perform processing for all started PEs */
        while (D_FLS_ZERO_U32 != boot_pe)
        {
            /* judge if the PE has been started */
            if (D_FLS_PE_CHECK == (D_FLS_PE_CHECK & boot_pe))
            {
                /* Clear flash mode switching response */
                g_sec_df_prog_res = FLS_MODE_SW_RESP_CLR;
                /* Issues a Data Flash (FPSYS0) mode switching request interrupt to the PE */
                lul_fls_status = D_COMM_TriggerInterruptToPE((icum_comm_target_pe_t)bit_position,
                                                             ICUM_COMM_ICH_TO_DF_PRG_REQ_0, NULL);
                if (ER_OK == lul_fls_status)
                {
                    /* Waiting for a response from the PE (with time monitoring) */
                    lul_fls_status = d_FLS_CheckDfProgRes(D_FLS_TIMEOUT_DFPROGRES);
                }
                else
                {
                    /* Do nothing */
                }
            } /* end of if (D_FLS_PE_CHECK == (D_FLS_PE_CHECK & boot_pe)) */
            else
            {
                /* Do nothing */
            }

            /* To the next PE processing */
            boot_pe >>= D_FLS_SHIT_1BIT;
            bit_position++;
        }
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK == lul_fls_status)
    {
        /* Set successful completion */
        lul_fls_status = FLS_ERR_NONE;
    }
    else
    {
        /* Set unexpected error */
        lul_fls_status = ER_D_FLS_INTERNAL;
    }

    return (lul_fls_status);
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0072]:[ICUMFW_UD_U2B_D_FLS_0072]
* Function Name : d_FLS_NoProcessing
* Description   : Pre-processing for issuing the FACI command of writing security area
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*******************************************************************************************/
static uint32_t d_FLS_NoProcessing(void)
{
    /* No preprocessing */
    return (FLS_ERR_NONE);
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0073]:[ICUMFW_UD_U2B_D_FLS_0073]
* Function Name : d_FLS_PreProcessingSwitch
* Description   : Pre-processing for issuing the FACI command of erasing switch area
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*******************************************************************************************/
static uint32_t d_FLS_PreProcessingSwitch(void)
{
    /* No preprocessing */
    return (FLS_ERR_NONE);
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0074]:[ICUMFW_UD_U2B_D_FLS_0074]
* Function Name : d_FLS_PreProcessingTAG
* Description   : Pre-processing for issuing the FACI command of erasing TAG area
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*******************************************************************************************/
static uint32_t d_FLS_PreProcessingTAG(void)
{
    /* No preprocessing */
    return (FLS_ERR_NONE);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0075]:[ICUMFW_UD_U2B_D_FLS_0075]
* Function Name : d_FLS_GetEraseWriteSizeSec
* Description   : get security setting erase size (byte)
* Arguments     : flash_addr
*                  security setting area address (dummy not use)
*               : *p_size
*                  erase size
* Return Value  : ER_OK : No error detected
*********************************************************************************/
static uint32_t d_FLS_GetEraseWriteSizeSec(UNUSED const uint32_t flash_addr, uint32_t *p_size)
{
    *p_size = SECURITY_SETTING_ERASE_SIZE;

    return (ER_OK);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0076]:[ICUMFW_UD_U2B_D_FLS_0076]
* Function Name : d_FLS_GetEraseWriteSizeSwitch
* Description   : get switch area erase size (byte)
* Arguments     : flash_addr
*                  switch area address (dummy not use)
*               : *p_size
*                  erase size
* Return Value  : ER_OK : No error detected
*********************************************************************************/
static uint32_t d_FLS_GetEraseWriteSizeSwitch(UNUSED const uint32_t flash_addr, uint32_t *p_size)
{
    *p_size = SWITCH_AREA_ERASE_SIZE;

    return (ER_OK);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0077]:[ICUMFW_UD_U2B_D_FLS_0077]
* Function Name : d_FLS_GetWriteSizeSwitch
* Description   : get switch area write size (byte)
* Arguments     : flash_addr
*                  switch area address (dummy not use)
*               : *p_size
*                  write size
* Return Value  : ER_OK : No error detected
*********************************************************************************/
static uint32_t d_FLS_GetWriteSizeSwitch(UNUSED const uint32_t flash_addr, uint32_t *p_size)
{
    *p_size = SWITCH_AREA_PROGRAM_SIZE;

    return (ER_OK);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0078]:[ICUMFW_UD_U2B_D_FLS_0078]
* Function Name : d_FLS_GetEraseWriteSizeTAG
* Description   : get TAG area erase size (byte)
* Arguments     : flash_addr
*                  TAG area address (dummy not use)
*               : *p_size
*                  erase size
* Return Value  : ER_OK : No error detected
*********************************************************************************/
static uint32_t d_FLS_GetEraseWriteSizeTAG(UNUSED const uint32_t flash_addr, uint32_t *p_size)
{
    *p_size = TAG_AREA_ERASE_SIZE;

    return (ER_OK);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0079]:[ICUMFW_UD_U2B_D_FLS_0079]
* Function Name : d_FLS_GetWriteSizeTAG
* Description   : get TAG area update size (byte)
* Arguments     : flash_addr
*                  TAG area address (dummy not use)
*               : *p_size
*                  update size
* Return Value  : ER_OK : No error detected
*********************************************************************************/
static uint32_t d_FLS_GetWriteSizeTAG(UNUSED const uint32_t flash_addr, uint32_t *p_size)
{
    *p_size = TAG_AREA_UPDATE_SIZE;

    return (ER_OK);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0080]:[ICUMFW_UD_U2B_D_FLS_0080]
* Function Name : d_FLS_IssueCommandSwProg
* Description   : FACI command issue (ProgramSwitch)
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*********************************************************************************/
static uint32_t d_FLS_IssueCommandSwProg(void)
{
    uint32_t lstag_fls_err;
    uint16_t lus_write_count;
    uint16_t lus_num_of_write;
    uint32_t lul_fsaddr;
    uint8_t  *p_lp_data;
    uint32_t lul_p_data_index;
    uint32_t lul_data_size_in_bytes;
    uint32_t lus_write_size_in_bytes;
    uint32_t lul_wfsaddr;
    uint32_t lul_wfeaddr;
    uint32_t lul_wcmddata;
    uint32_t lul_switch_data[(uint32_t)SWITCH_AREA_PROGRAM_SIZE/sizeof(uint32_t)];
    uint32_t lul_data_count;

    lstag_fls_err = D_FLS_ZERO_U32;
    lus_write_count = D_FLS_ZERO_U16;
    lus_num_of_write = D_FLS_ZERO_U16;
    lul_fsaddr = D_FLS_ZERO_U32;
    p_lp_data = NULL;
    lul_p_data_index = D_FLS_ZERO_U32;
    lul_data_size_in_bytes = D_FLS_ZERO_U32;
    lus_write_size_in_bytes = D_FLS_ZERO_U32;
    lul_wfsaddr = D_FLS_ZERO_U32;
    lul_wfeaddr = D_FLS_ZERO_U32;
    lul_wcmddata = D_FLS_ZERO_U32;
    lul_data_count = D_FLS_ZERO_U32;
    L_Toolbox_MemSet((uint8_t *)&lul_switch_data, (uint8_t)D_FLS_ZERO_U32, (int16_t)sizeof(lul_switch_data));

    lstag_fls_err = FLS_ERR_NONE;
    lul_p_data_index = g_fls_ctrl[g_fls_selected_fpsys].prm.p_num;
    lul_data_size_in_bytes = g_fls_ctrl[g_fls_selected_fpsys].prm.data_size_in_bytes;

    /* set Incremental mod */
     FBCCNT = (uint8_t)((uint32_t)FBCCNT & ~FBCCNT_DEC_MODE);
    /* get target flash adr */
    lul_fsaddr = g_fls_ctrl[g_fls_selected_fpsys].prm.flash_addr + g_fls_ctrl[g_fls_selected_fpsys].prm.p_num;
    /* get write size */
    g_fls_ctrl[g_fls_selected_fpsys].prm.p_write_size_in_bytes(lul_fsaddr, &lus_write_size_in_bytes);
    /* get start adr & end adr */
    lul_wfsaddr = (lul_fsaddr & FSEADDR_MASK);
    lul_wfeaddr = (((lul_fsaddr + lus_write_size_in_bytes) - D_FLS_SET1_U32) & FSEADDR_MASK);
    /* Set flash memory address to FSADDR(bit31-21 is 0) */
    FSADDR  = lul_wfsaddr;
    /* Set (Flash memory address + lus_write_size_in_bytes - 1) to FEADDR */
    FEADDR  = lul_wfeaddr;
    /* Set FACI Command code to FACICMD(8bit)   */
    FACICMD = (uint8_t)g_fls_ctrl[g_fls_selected_fpsys].prm.command_code;

    /* Reads valid side information from the Switch area (valid area) */
    for (lul_data_count = D_FLS_ZERO_U32; (sizeof(lul_switch_data) / sizeof(uint32_t)) > lul_data_count; lul_data_count++)
    {
        lul_switch_data[lul_data_count] = g_switch_area_valid_data[lul_data_count];
    }
    /* Invert the valid side of the Security setting Valid Area */
    lul_switch_data[SWITCH_AREA_VALID_INDEX_SVA] = ~lul_switch_data[SWITCH_AREA_VALID_INDEX_SVA];
    p_lp_data = (uint8_t *)lul_switch_data;

    /* Write data set */
    lus_num_of_write = (uint16_t)(lus_write_size_in_bytes / sizeof(uint32_t));

    FACICMD = (uint8_t)lus_num_of_write;
    lus_write_count = D_FLS_ZERO_U16;

    /* Writes data for one block */
    for ( ; lul_p_data_index < lul_data_size_in_bytes; lul_p_data_index += sizeof(lul_wcmddata))
    {
        lul_wcmddata  = (uint32_t)p_lp_data[lul_p_data_index];
        lul_wcmddata |= ((uint32_t)p_lp_data[(lul_p_data_index + D_FLS_INDEX_1)] << D_FLS_SHIFT_8BIT);
        lul_wcmddata |= ((uint32_t)p_lp_data[(lul_p_data_index + D_FLS_INDEX_2)] << D_FLS_SHIFT_16BIT);
        lul_wcmddata |= ((uint32_t)p_lp_data[(lul_p_data_index + D_FLS_INDEX_3)] << D_FLS_SHIFT_24BIT);
        /* 4byte(data) */
        FACICMDDT = lul_wcmddata;

        lus_write_count = (uint16_t)((uint32_t)lus_write_count + D_FLS_SET1_U32);
        if ((uint32_t)lus_write_count >= (uint32_t)lus_num_of_write)
        {
            break;
        }
        else
        {
            /* go next Writes data */
        }
    } /* end of for loop ( lul_p_data_index < lul_data_size_in_bytes ) */

    /* If the size of the last block is less than the block size, fill it with 0xFF */
    for ( ; (uint32_t)lus_write_count < (uint32_t)lus_num_of_write ;
            lus_write_count = (uint16_t)((uint32_t)lus_write_count + D_FLS_SET1_U32))
    {
        /* 4byte(data) */
        FACICMDDT = D_FLS_FILL_DATA;
    }

    /* Command execution(8bit) */
    FACICMD = (uint8_t)D_FACI_CMD_PE_RESUME;

    /* Read Counter when Start of FACI command */
    D_OST_GetHardCounter(&g_fls_ctrl[g_fls_selected_fpsys].prm.faci_start_time);

    /* Clear Number of checks FACI command Completion */
    g_fls_ctrl[g_fls_selected_fpsys].prm.faci_check_counter = D_FLS_ZERO_U32;

    return (lstag_fls_err);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0081]:[ICUMFW_UD_U2B_D_FLS_0081]
* Function Name : d_FLS_CheckDfProgRes
* Description   : Waiting for a response from the PE (with time monitoring)
* Arguments     : time_out_cnt
*                  Loop count value until timeout
* Return Value  : ER_OK
*                 ER_D_FLS_DFPRGINTTOUT
*********************************************************************************/
static uint32_t d_FLS_CheckDfProgRes(uint32_t time_out_cnt)
{
    uint32_t    lul_fls_status;
    uint32_t    lul_loop_count;

    lul_fls_status = ER_D_FLS_DFPRGINTTOUT;
    lul_loop_count = D_FLS_ZERO_U32;

    /* Waiting for a response from the PE */
    for (lul_loop_count = time_out_cnt; D_FLS_ZERO_U32 < lul_loop_count; lul_loop_count--)
    {
        if ((uint8_t)FLS_MODE_SW_RESP_CLR != g_sec_df_prog_res)
        {
            lul_fls_status = ER_OK;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    return (lul_fls_status);
}

