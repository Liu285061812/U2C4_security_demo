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
#include ICUM_DEVICE_FILE

#include "r_icumif_api.h"
#include "renesas_types.h"

#include "u2b_reg_adr.h"

#include "icum_l_toolbox_pub.h"

#include "icum_d_flash_pub.h"
#include "icum_d_flash.h"

#include "icum_d_misc_pub.h"
#include "icum_d_ost_pub.h"
#include "icum_d_comm_pub.h"

#include "r_rfd_constant.h"
#include "r_rfd_common.h"
#include "r_rfd_df.h"

/********************************************************************************
 QAC Suppression message list
*********************************************************************************/

/********************************************************************************
 Memory allocation section specification
*********************************************************************************/
/* self copy icum_driver_rom.text to icum_driver.data   */
CHANGE_SEC(data, ".icum_driver.data")
CHANGE_SEC(bss, ".icum_driver.bss")

/********************************************************************************
 Macro definitions
*********************************************************************************/

/*******************************************************************************
 Typedef definitions
*********************************************************************************/

/********************************************************************************
 Exported global variables (to be accessed by other files)
*********************************************************************************/

/* FACI base addresses for ICUM */

/* selected fpsys FACI0 or FACI2 */
uint32_t g_fls_selected_fpsys;

/* FLS Control parameter */
fls_ctrl_t g_fls_ctrl[NB_FPSYS];
                /* [0]:FACI0, [2]:FACI2 */
fls_ctrl_t g_fls_ctrl_bak[NB_FPSYS];
                /* Backup is "FACI0" only */

/* Flash driver status */
uint32_t g_fls_status = D_FLS_STS_UNINITIALIZE;

/* Code flash PE mode start / end notification target FACI */
uint32_t g_d_cf_pe_notify_faci = R_RFD_FACI0;

/* DF_PE mode was set on PE side flag */
uint8_t g_usr_df_prog_req = AG_D_FLS_DF_PE_CLR;

/* Contents of DF mode switching response interrupt */
uint8_t g_sec_df_prog_res;

/* issd callback function setting */
p_d_fls_cfint_addr *p_d_fls_cbfunc_addr;

uint16_t g_fls_cpu_frequency_MHz;

extern uint32_t g_security_property_backup[D_FLS_MANDATORY_WRITE_COUNT][D_FLS_WRITE_COUNT];

extern uint16_t g_sec_area_write_offset[D_FLS_MANDATORY_WRITE_COUNT];
/********************************************************************************
 Private global variables and functions
*********************************************************************************/

/********************************************************************************
 Private function prototypes
*********************************************************************************/
static uint32_t d_FLS_ForcedStopCommand_ram(void);
static uint32_t d_FLS_SetOperatingMode_ram(const uint16_t operating_mode, const uint8_t comm_code);
static uint32_t d_FLS_Err2Status_ram(const uint32_t error_code);
static void d_FLS_SetReadMode_ram(void);
static uint32_t d_FLS_PreProcessingCancel_ram(void);
static uint32_t d_FLS_GetDummyWriteSize_ram(UNUSED uint32_t flash_addr, uint32_t *p_size);
static uint32_t d_FLS_IssueCommandCtrl_ram(void);
static uint32_t d_FLS_CheckProcessingCompletion_ram(void);
static void d_FLS_WriteProtectOn_ram(void);
static void d_FLS_WriteProtectOff_ram(void);
static void d_FLS_WriteProtectOffFaci_ram(const uint32_t target_faci);

static uint32_t d_FLS_CheckCondition_ram(const uint16_t context_id, const uint8_t priority,
                                         const uint32_t target_area, const uint32_t bank);
static uint32_t d_FLS_GetInterface_ram(const uint32_t target_area, const uint32_t bank, uint32_t *p_interface);
static uint32_t d_FLS_IssueSuspendCommand(const uint32_t iface);
static uint32_t d_FLS_SuspensionCommand(const uint32_t iface);
static uint32_t d_FLS_ResumptionCommand(const uint32_t iface);
static uint32_t d_FLS_ExecSuspension(const uint32_t iface);
static void d_FLS_ExecResumption(const uint32_t iface);

static uint8_t d_FLS_IsUserBootAddr(const uint32_t addr);
static uint8_t d_FLS_IsRemapAddr(const uint32_t addr);

/********************************************************************************
 API Function definition
*********************************************************************************/
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0001]:[ICUMFW_UD_U2B_D_FLS_0001]
* Function Name : D_FLS_InitFirst
* Description   : Initialized Fpsys
* Arguments     : p_init_prm_pstr
*                  Specify the operation parameters of FLS.
* Return Value  : ER_OK
*                 ER_D_FLS_CONFIGURATION
*                 ER_D_FLS_SETOPRDYTOUT
*                 ER_D_FLS_REJECTED
*                 ER_D_FLS_SETOPMODTOUT
*                 ER_D_FLS_FSTOPTOUT
*********************************************************************************/
/* Code executed on ROM             */
CHANGE_SEC(text, ".icum_driver.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")

uint32_t D_FLS_InitFirst(const icum_fls_init_prm_t *p_init_prm_pstr)
{
    uint32_t lul_fls_status;
    fls_ctrl_t clr_fls_ctrl;
    ICUM_COMM_CB_REGIST_t comm_cb_regist;

    lul_fls_status = ER_OK;
    L_Toolbox_MemSet((uint8_t *)&clr_fls_ctrl, D_FLS_ZERO_U8, sizeof(clr_fls_ctrl));
    L_Toolbox_MemSet((uint8_t *)&comm_cb_regist, D_FLS_ZERO_U8, sizeof(comm_cb_regist));

    /* parameter check */
    if (NULL == p_init_prm_pstr)
    {
        /* parameter address NULL */
        lul_fls_status = ER_D_FLS_CONFIGURATION;
        /* FLS status error */
    }
    else
    {
        /* Normal */
    }

    if (ER_OK == lul_fls_status)
    {
        /* save parameter */
        g_fls_cpu_frequency_MHz = p_init_prm_pstr->cpu_frequency_MHz;
        /* clock range OK ? */
        if (((uint32_t)CPU_CLOCK_MIN <= (uint32_t)g_fls_cpu_frequency_MHz)
            && ((uint32_t)g_fls_cpu_frequency_MHz <= (uint32_t)CPU_CLOCK_MAX))
        {
            /* Normal */
        }
        else
        {
            /* Configuration Error */
            lul_fls_status = ER_D_FLS_CONFIGURATION;
        }
    }
    else
    {
        /* Do nothing */
    }

    /* Initializes the RFD28F */
    lul_fls_status = (uint32_t)R_RFD_Init(0xFFFF);
    
    if (ER_OK == lul_fls_status)
    {
        /* Enables write access of protected registers */
        ICUM_DFECC.ICUM_DFKCPROT.UINT32 = KCE_EN;
        /* ECC Error enable */
        ICUM_DFECC.CTL.UINT32  &= ~ICUM_ECC_ERROR_DIS;
        /* Data Flash ECC Error Notification(EI level:INTECCDECERRDF) Disable */
        ICUM_DFECC.ICUM_DFERRINT.UINT32  &= ~ICUM_DFERRINT_EN;
        /* Disables write access of protected registers */
        ICUM_DFECC.ICUM_DFKCPROT.UINT32 = KCE_DIS;

        /* Initialize flash control table */
        g_fls_ctrl[R_RFD_FACI0] = clr_fls_ctrl;
        g_fls_ctrl[R_RFD_FACI2] = clr_fls_ctrl;
        g_fls_ctrl_bak[R_RFD_FACI0] = clr_fls_ctrl;
        g_fls_ctrl_bak[R_RFD_FACI2] = clr_fls_ctrl;
        g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_IDLE;
        g_fls_ctrl[R_RFD_FACI2].status = D_FLS_IF2_IDLE;
        g_fls_ctrl_bak[R_RFD_FACI0].status = D_FLS_IF0_IDLE;
        g_fls_ctrl_bak[R_RFD_FACI2].status = D_FLS_IF2_IDLE;

        /* Register the handler for the DF mode switching response interrupt */
        comm_cb_regist.channel = ICUM_COMM_ICH_FM_DF_PRG_NOTIFY | ICUM_COMM_ICH_FM_DF_PRG_ERROR;
        comm_cb_regist.p_cbfunc = (p_func_addr *)D_FLS_DFModeSwitchResInt_ram;
        lul_fls_status = D_COMM_SetIntCallBackFunc(&comm_cb_regist);

        /* Set "Driver initialized" to the flash status */
        g_fls_status |= D_FLS_STS_DRV_INITIALIZED;
    } /* end of if (ER_OK == lul_fls_status) */
    else
    {
        /* Do nothing */
    }

    g_fls_selected_fpsys = R_RFD_FACI0;

    return (lul_fls_status);
}

/*******************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0154]:[ICUMFW_UD_U2B_D_FLS_0110]
* Function Name : D_FLS_InitSecond
* Description   : Callback registration at startup.
* Arguments     : p_d_fls_cbfunc
*                  Address of callback function to register.
* Return Value  : ER_OK
********************************************************************************/
uint32_t D_FLS_InitSecond(p_d_fls_cfint_addr *p_d_fls_cbfunc)
{
    /* Callback registration. */
    D_FLS_SetCfIntCallBackFunc_ram((p_d_fls_cfint_addr *)p_d_fls_cbfunc);

    return ER_OK;
}

/* self copy icum_driver_rom.text to icum_driver_ram   */
CHANGE_SEC(text, ".icum_driver_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")

/*******************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0002]:[ICUMFW_UD_U2B_D_FLS_0002]
* Function Name : d_FLS_SelectFpsys_ram
* Description   : Switch the FCU to access the specified flash area address.
* Arguments     : flash_addr
*                  flash area address.
* Return Value  : ER_OK
********************************************************************************/
uint32_t d_FLS_SelectFpsys_ram(const uint32_t flash_addr)
{
    if ((START_ADDR_DF_FPSYS2 <= flash_addr) && (START_ADDR_DF_FPSYS3 > flash_addr))
    {
        g_fls_selected_fpsys = R_RFD_FACI2;
    }
    else
    {
        g_fls_selected_fpsys = R_RFD_FACI0;
    }

    d_FLS_WriteProtectOffFaci_ram(g_fls_selected_fpsys);

    return (ER_OK);
}

/*************************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0003]:[ICUMFW_UD_U2B_D_FLS_0003]
* Function Name : d_FLS_Handler_ram
* Description   : Proceed with this FACI command operation by FLS and confirm the completion.
* Arguments     : fpsys
*                  FACI number
* Return Value  : ER_OK
*                 ER_IN_PROCESS
*                 ER_D_FLS_NON_BLANK
*                 ER_D_FLS_PROTECTION
*                 ER_D_FLS_WRITE
*                 ER_D_FLS_ERASE
*                 ER_D_FLS_INTERNAL
*                 ER_D_FLS_SETOPRDYTOUT
*                 ER_D_FLS_REJECTED
*                 ER_D_FLS_SETOPMODTOUT
*                 ER_D_FLS_FSTOPTOUT
*                 ER_D_FLS_FACITOUT
**************************************************************************************************/
uint32_t d_FLS_Handler_ram(const uint32_t fpsys)
{
    uint32_t lstag_fls_err;
    uint32_t lul_fls_status;
    uint32_t lul_write_size;
    uint32_t lul_comp_result;
    uint32_t lul_continuously_flag;

    lul_fls_status = g_fls_ctrl[fpsys].prm.d_status;

    if (D_FLS_STS_AVAILABLE != g_fls_status)
    {
        lul_fls_status = ER_D_FLS_INTERNAL;
    }
    else if (ER_D_FLS_REQUEST == lul_fls_status)
    {
        /* Performs FACI command issue processing */
        lul_fls_status = d_FLS_HandlerCommandReq_ram(fpsys);
    }
    else if (ER_IN_PROCESS == lul_fls_status)
    {
        g_fls_ctrl[fpsys].prm.p_write_size_in_bytes((g_fls_ctrl[fpsys].prm.flash_addr + g_fls_ctrl[fpsys].prm.p_num),
                                                    &lul_write_size);
        /* accepted the cancel API ? */
        if ((uint32_t)D_FLS_CMDREQ_ON == (uint32_t)g_fls_ctrl[fpsys].prm.commad_req)
        {
            /* Force-stop */
            g_fls_ctrl[fpsys].prm.commad_req = D_FLS_CMDREQ_OFF;
            lstag_fls_err = g_fls_ctrl[fpsys].prm.p_faci_commad_set();
            if (FLS_ERR_NONE != lstag_fls_err)
            {
                lul_fls_status = d_FLS_Err2Status_ram(lstag_fls_err);
                d_FLS_SetReadMode_ram();
                g_fls_ctrl[fpsys].prm.p_set_ctrl_prm = NULL;
            }
            else
            {
                lul_fls_status = ER_IN_PROCESS;
            }
        }
        else
        {
            lul_comp_result = d_FLS_CheckProcessingCompletion_ram();
            if ((uint32_t)ER_OK == lul_comp_result)
            {
                lstag_fls_err = g_fls_ctrl[fpsys].prm.p_error_check();
                if (FLS_ERR_NONE != lstag_fls_err)
                {
                    d_FLS_SetReadMode_ram();
                    lul_fls_status = d_FLS_Err2Status_ram(lstag_fls_err);
                    g_fls_ctrl[fpsys].prm.p_set_ctrl_prm = NULL;
                }
                else
                {
                    /* In case of cancellation, g_fls_ctrl[fpsys].prm.p_num & Lul_write_size is 0 */
                    g_fls_ctrl[fpsys].prm.p_num += lul_write_size;

                    if (g_fls_ctrl[fpsys].prm.p_num < g_fls_ctrl[fpsys].prm.data_size_in_bytes)
                    {   /* In case of cancellation, This condition is not satisfied */
                        /* Next block */
                        lstag_fls_err = g_fls_ctrl[fpsys].prm.p_faci_commad_set();
                        if (FLS_ERR_NONE != lstag_fls_err)
                        {
                            d_FLS_SetReadMode_ram();
                            lul_fls_status = d_FLS_Err2Status_ram(lstag_fls_err);
                            g_fls_ctrl[fpsys].prm.p_set_ctrl_prm = NULL;
                        }
                        else
                        {
                            /* Succeeded in set command, go to normal end */
                        }
                    }
                    else
                    {
                        /* All Block Complete !! */
                        lul_fls_status = d_FLS_SetOperatingMode_ram((uint16_t)R_RFD_MODE_READ, D_FACI_CMD_DUMMY_CMD);
                        if ((uint32_t)ER_OK != lul_fls_status)
                        {
                            d_FLS_SetReadMode_ram();
                            g_fls_ctrl[fpsys].prm.p_set_ctrl_prm = NULL;
                        }
                        else
                        {
                            lul_fls_status = g_fls_ctrl[fpsys].prm.success_resp;
                        }
                        /* Issue the FACI command that continuously requests */
                        if ((ER_OK == lul_fls_status) && (NULL != g_fls_ctrl[fpsys].prm.p_set_ctrl_prm))
                        {
                            /* Performs command parameter setting processing */
                            lul_continuously_flag = D_FLS_SET1_U32;
                            lul_fls_status = g_fls_ctrl[fpsys].prm.p_set_ctrl_prm();
                        }
                        else
                        {
                            lul_continuously_flag = D_FLS_ZERO_U32;
                        }
                        if ((ER_OK == lul_fls_status) && (D_FLS_SET1_U32 == lul_continuously_flag))
                        {
                            /* Succeeded in set command parameters    */
                            /* Performs FACI command issue processing */
                            lul_fls_status = d_FLS_HandlerCommandReq_ram(fpsys);
                        }
                        else
                        {
                            g_fls_ctrl[fpsys].prm.p_set_ctrl_prm = NULL;
                        }
                    }
                } /* end of else */
            } /* end of if ((uint32_t)ER_OK == lul_comp_result) */
            else if (ER_D_FLS_FACITOUT == lul_comp_result)
            {
                /* In case of FACI timeout, set read mode */
                d_FLS_SetReadMode_ram();
                lul_fls_status = lul_comp_result;
            }
            else
            {
                /* Nothing to do ER_IN_PROCESS == lul_fls_status */
            }   /* end of ER_OK == d_FLS_CheckProcessingCompletion_ram() */
        } /* end of else */
    } /* end of else if (ER_IN_PROCESS == lul_fls_status)  */
    else
    {
        g_fls_ctrl[fpsys].prm.p_set_ctrl_prm = NULL;
    }

    g_fls_ctrl[fpsys].prm.d_status = lul_fls_status;
    return (g_fls_ctrl[fpsys].prm.d_status);
}

/*******************************************************************************************
* Function ID   : (not supported)
* Function Name : D_FLS_CancelReq_ram(void)
* Description   : Request for forced stop of the flash operation process being executed.
* Arguments     : none
* Return Value  : ER_OK
*                 ER_IN_PROCESS
*                 ER_D_FLS_CANCEL
*                 ER_D_FLS_INTERNAL
*                 ER_D_FLS_SETOPRDYTOUT
*                 ER_D_FLS_REJECTED
*                 ER_D_FLS_SETOPMODTOUT
*                 ER_D_FLS_FSTOPTOUT
*                 ER_D_FLS_NON_BLANK
*                 ER_D_FLS_PROTECTION
*                 ER_D_FLS_WRITE
*                 ER_D_FLS_ERASE
*                 ER_D_FLS_FACITOUT
********************************************************************************************/
uint32_t D_FLS_CancelReq_ram(const uint32_t target_area, const uint32_t bank)
{
    uint32_t lul_fls_status;
    uint32_t iface;

    iface = R_RFD_FACI0;

    lul_fls_status = d_FLS_GetInterface_ram(target_area, bank, &iface);
    if (ER_OK == lul_fls_status)
    {
        g_fls_ctrl[iface].prm.ope_mode            = (uint16_t)R_RFD_MODE_DFPE;
        g_fls_ctrl[iface].prm.commad_req          = D_FLS_CMDREQ_ON;
        g_fls_ctrl[iface].prm.command_code        = D_FACI_CMD_FORCED_STOP;
        g_fls_ctrl[iface].prm.flash_addr          = D_FLS_ZERO_U32;
        g_fls_ctrl[iface].prm.p_data              = NULL;
        g_fls_ctrl[iface].prm.data_size_in_bytes  = D_FLS_ZERO_U32;
        g_fls_ctrl[iface].prm.p_pre_processing    = &d_FLS_PreProcessingCancel_ram;
        g_fls_ctrl[iface].prm.p_faci_commad_set   = &d_FLS_IssueCommandCtrl_ram;
        g_fls_ctrl[iface].prm.p_write_size_in_bytes = &d_FLS_GetDummyWriteSize_ram;
        g_fls_ctrl[iface].prm.p_error_check       = &d_FLS_CheckFaciErrors_ram;
        g_fls_ctrl[iface].prm.success_resp        = ER_OK;
        g_fls_ctrl[iface].prm.p_num               = D_FLS_ZERO_U32;

        lul_fls_status = d_FLS_Handler_ram(iface);
    } /* end of if (ER_OK == lul_fls_status) */
    else
    {
        /* Do nothing */
    }

    return (lul_fls_status);
}

/*************************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0035]:[ICUMFW_UD_U2B_D_FLS_0035]
* Function Name : D_FLS_IsFinished_ram
* Description   : Proceed with this FACI command operation(REQ) by FLS and confirm the completion.
* Arguments     : context_id
*                  flash context id
*                 target_area
*                  target flash area
*                 bank
*                  target bank
* Return Value  : ER_OK
*                 ER_IN_PROCESS 
*                 ER_PARAM
*                 ER_D_FLS_PROTECTION
*                 ER_D_FLS_INTERNAL
*                 ER_D_FLS_NON_BLANK
*                 ER_D_FLS_WRITE
*                 ER_D_FLS_ERASE
*                 ER_D_FLS_SETOPRDYTOUT
*                 ER_D_FLS_REJECTED
*                 ER_D_FLS_SETOPMODTOUT
*                 ER_D_FLS_FSTOPTOUT
*                 ER_D_FLS_FACITOUT
**************************************************************************************************/
uint32_t D_FLS_IsFinished_ram(const uint16_t context_id, const uint32_t target_area, const uint32_t bank)
{
    uint8_t luc_fls_gard_flg;
    uint32_t lul_fls_status;
    uint32_t iface;
    uint32_t err;

    luc_fls_gard_flg = 0U;
    lul_fls_status = ER_PARAM;
    iface = R_RFD_FACI0;

    err = d_FLS_GetInterface_ram(target_area, bank, &iface);
    if (ER_OK == err)
    {
        /* result address NULL */
        if ((D_FLS_ZERO_U32 == (uint32_t)context_id)
            || (((uint32_t)context_id != (uint32_t)g_fls_ctrl[iface].context.id) 
                &&  ((uint32_t)context_id != (uint32_t)g_fls_ctrl_bak[iface].context.id)))
        {
            /* Do nothing */
        }
        else if ((uint32_t)context_id == (uint32_t)g_fls_ctrl_bak[iface].context.id)
        {
            lul_fls_status = ER_IN_PROCESS;
        }
        else
        {
            /* Flash Req executed ? */
            if (((uint32_t)D_FACI_CMD_BLOCK_ERASE == (uint32_t)g_fls_ctrl[iface].prm.command_code) ||
                ((uint32_t)D_FACI_CMD_PROGRAM == (uint32_t)g_fls_ctrl[iface].prm.command_code) ||
                ((uint32_t)D_FACI_CMD_MULTI_PROGRAM == (uint32_t)g_fls_ctrl[iface].prm.command_code) ||
                ((uint32_t)D_FACI_CMD_BLANK_CHECK == (uint32_t)g_fls_ctrl[iface].prm.command_code) ||
                ((uint32_t)D_FACI_CMD_SECURITY_ERASE == (uint32_t)g_fls_ctrl[iface].prm.command_code) ||
                ((uint32_t)D_FACI_CMD_SECURITY_PROGRAM == (uint32_t)g_fls_ctrl[iface].prm.command_code) ||
                ((uint32_t)D_FACI_CMD_SWITCH_ERASE == (uint32_t)g_fls_ctrl[iface].prm.command_code) ||
                ((uint32_t)D_FACI_CMD_SWITCH_PROGRAM == (uint32_t)g_fls_ctrl[iface].prm.command_code) ||
                ((uint32_t)D_FACI_CMD_TAG_ERASE == (uint32_t)g_fls_ctrl[iface].prm.command_code) ||
                ((uint32_t)D_FACI_CMD_TAG_UPDATE == (uint32_t)g_fls_ctrl[iface].prm.command_code))
            {
                luc_fls_gard_flg = 1U;
            }
            else
            {
                /* do nothing */
            }

            g_fls_selected_fpsys = iface;
            lul_fls_status = d_FLS_Handler_ram(iface);
            if (lul_fls_status != ER_IN_PROCESS)
            {
                /* Req end & Gard ON Req =>Bus guard set & Write protect set */
                if ((uint32_t)luc_fls_gard_flg == 1UL)
                {
                    D_FLS_PrgDisposeOperation_ram();
                }
                else
                {
                    /* Do nothing */
                }
                /* Initialize request command code */
                g_fls_ctrl[iface].prm.d_status = ER_OK;
                g_fls_ctrl[iface].prm.command_code = D_FACI_CMD_NONE;
                g_fls_ctrl[iface].prm.commad_req = D_FLS_CMDREQ_OFF;
            } /* end of if (lul_fls_status != ER_IN_PROCESS) */
            else
            {
                /* Do nothing */
            }
        } /* end of else */
    } /* end of if (ER_OK == err) */
    else
    {
        lul_fls_status = err;
    }

    return (lul_fls_status);
}

/*******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0038]:[ICUMFW_UD_U2B_D_FLS_0038]
* Function Name : d_FLS_CheckCondition_ram
* Description   : Check Flash Condition
* Arguments     : context_id
*                  flash context id
*                 priority
*                  request priority
*                 target_area
*                  target flash area
*                 bank
*                  target bank
* Return Value  : ER_OK
*                 ER_PARAM
*                 ER_PENDING
*                 ER_D_FLS_UNAVAILABLE
********************************************************************************************/
static uint32_t d_FLS_CheckCondition_ram(const uint16_t context_id, const uint8_t priority,
                                         const uint32_t target_area, const uint32_t bank)
{
    uint32_t err;
    uint32_t iface;
    fls_ctx_t *p_current;
    uint32_t  firmware_bank;

    iface = R_RFD_FACI0;

    err = d_FLS_GetInterface_ram(target_area, bank, &iface);
    if (ER_OK == err)
    {
        p_current = &g_fls_ctrl[iface].context;
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else if ((D_FLS_ZERO_U32 != (uint32_t)p_current->id) && ((uint32_t)p_current->id == (uint32_t)context_id))
    {
        /* Cannot get multiplexed resources */
        err = ER_D_FLS_UNAVAILABLE;
    }
    else if ((R_RFD_FACI0 == iface) &&
             (D_FLS_ZERO_U32 != (uint32_t)g_fls_ctrl_bak[iface].context.id))
    {
        if ((uint32_t)context_id == (uint32_t)g_fls_ctrl_bak[iface].context.id)
        {
            /* Suspended context */
            err = ER_D_FLS_UNAVAILABLE;
        }
        else
        {
            err = ER_PENDING;
        }
    }
    else if ((D_FLS_ZERO_U32 != (uint32_t)p_current->id)
                && ((p_current->target_area == target_area) && (p_current->target_bank == bank)))
    {
        err = ER_PENDING;
    }
    else if (R_RFD_FACI0 == iface)
    {
        err = ER_PENDING;
    }
    else if ((D_FLS_ZERO_U32 != (uint32_t)p_current->id) && ((uint32_t)p_current->priority <= (uint32_t)priority))
    {
        err = ER_PENDING;
    }
    else if ((D_FLS_ZERO_U32 != (uint32_t)p_current->id) && ((uint32_t)p_current->priority > (uint32_t)priority))
    {
        /* Checking the RAM execution mode of the preceding job */
        if (AG_D_FLS_CODE == p_current->target_area)
        {
            err = d_FLS_GetCFBankNumber(D_FLS_ICUM_FIRMWARE_START_ADDRESS, &firmware_bank);
            if (ER_OK == err)
            {
                if (firmware_bank == p_current->target_bank)
                {
                    err = ER_PENDING;
                }
                else
                {
                    err = ER_OK;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            err = ER_OK;
        }
    }
    else
    {
        err = ER_OK;
    }

    return (err);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_driver.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/*******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0039]:[ICUMFW_UD_U2B_D_FLS_0039]
* Function Name : d_FLS_ExecSuspension
* Description   : Perform pause.
* Arguments     : iface
*                  FACI I/F
* Return Value  : ER_OK
*                 ER_D_FLS_SUSPEND
********************************************************************************************/
static uint32_t d_FLS_ExecSuspension(const uint32_t iface)
{
    uint32_t err;

    L_Toolbox_MemCpy((uint8_t *)&g_fls_ctrl_bak[iface], (const uint8_t *)&g_fls_ctrl[iface], (int16_t)sizeof(fls_ctrl_t));
    err = d_FLS_SuspensionCommand(iface);

    return (err);
}

/*******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0040]:[ICUMFW_UD_U2B_D_FLS_0040]
* Function Name : d_FLS_ExecResumption
* Description   : Perform Pause Resume.
* Arguments     : iface
*                  FACI I/F
* Return Value  : N/A
********************************************************************************************/
static void d_FLS_ExecResumption(const uint32_t iface)
{
    uint8_t  finish_status;

    (void)d_FLS_ResumptionCommand(iface);
    /* Backup a status, because it is necessary to update the changed state to the restored information */
    finish_status = g_fls_ctrl[iface].status;
    L_Toolbox_MemCpy((uint8_t *)&g_fls_ctrl[iface], (const uint8_t *)&g_fls_ctrl_bak[iface], (int16_t)sizeof(fls_ctrl_t));
    g_fls_ctrl[iface].status = finish_status;

    g_fls_ctrl_bak[iface].context.id = 0U;
    g_fls_ctrl_bak[iface].status = D_FLS_IF0_IDLE;
}

/* self copy icum_driver_rom.text to icum_driver_ram   */
CHANGE_SEC(text, ".icum_driver_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/*******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0005]:[ICUMFW_UD_U2B_D_FLS_0005]
* Function Name : D_FLS_GetFlashResource_ram
* Description   : Refer to the available flag and set unavailable if available.
* Arguments     : context_id
*                  flash context id
*                 priority
*                  request priority
*                 target_area
*                  target flash area
*                 bank
*                  target bank
* Return Value  : ER_OK
*                 ER_PARAM
*                 ER_PENDING
*                 ER_D_FLS_INTERNAL
*                 ER_D_FLS_UNAVAILABLE
*                 ER_D_FLS_SUSPEND
********************************************************************************************/
uint32_t D_FLS_GetFlashResource_ram(const uint16_t context_id, const uint8_t priority, const uint32_t target_area,
                                    const uint32_t bank)
{
    uint32_t err;
    uint32_t iface;
    uint32_t saved_psw;

    iface = R_RFD_FACI0;

    err = d_FLS_GetInterface_ram(target_area, bank, &iface);
    if (ER_OK == err)
    {
        err = d_FLS_CheckCondition_ram(context_id, priority, target_area, bank);
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            /* Disable EI Level Maskable Interrupt */
            DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

            if (AG_D_FLS_DATA == target_area)
            {
                if (AG_D_FLS_BANK_ICUM == bank)
                {
                    err = d_FLS_StateMachineIf2(D_FLS_IF2_ICUM_DF_ACQ);
                }
                else
                {
                    err = d_FLS_StateMachineIf0(D_FLS_IF0_ICUM_DF_ACQ);
                }
            }
            else if (AG_D_FLS_CODE == target_area)
            {
                if ((AG_D_FLS_BANKA == bank) || (AG_D_FLS_BANKB == bank))
                {
                    err = d_FLS_StateMachineIf0(D_FLS_IF0_ICUM_CF_ACQ);
                }
            }
            else
            {
                /* In config and security setting */
                err = d_FLS_StateMachineIf0(D_FLS_IF0_ICUM_EXT_ACQ);
            }

            /* Enable EI Level Maskable Interrupt */
            ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

            if (ER_OK == err)
            {
                if (D_FLS_ZERO_U32 != (uint32_t)g_fls_ctrl[iface].context.id)
                {
                    err = d_FLS_ExecSuspension(iface);
                }
                else
                {
                    /* Do nothing */
                }
                g_fls_ctrl[iface].context.id = context_id;
                g_fls_ctrl[iface].context.priority = priority;
                g_fls_ctrl[iface].context.target_area = target_area;
                g_fls_ctrl[iface].context.target_bank = bank;
            }
            else
            {
                /* Do nothing */
            }
        } /* end of else */
    }
    else
    {
        /* Do nothing */
    }

    return (err);
}

/*******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0006]:[ICUMFW_UD_U2B_D_FLS_0006]
* Function Name : D_FLS_FreeFlashResource_ram
* Description   : Set available.
* Arguments     : target_area
*                  target flash area
*                 bank
*                  target bank
* Return Value  : ER_OK
*                 ER_PENDING
*                 ER_D_FLS_INTERNAL
*                 ER_D_FLS_UNAVAILABLE
********************************************************************************************/
uint32_t D_FLS_FreeFlashResource_ram(const uint32_t target_area, const uint32_t bank)
{
    uint32_t err;
    uint32_t iface;
    uint32_t saved_psw;

    iface = R_RFD_FACI0;

    err = d_FLS_GetInterface_ram(target_area, bank, &iface);
    if (ER_OK == err)
    {
        /* Disable EI Level Maskable Interrupt */
        DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

        if (AG_D_FLS_DATA == target_area)
        {
            if (AG_D_FLS_BANK_ICUM == bank)
            {
                err = d_FLS_StateMachineIf2(D_FLS_IF2_ICUM_DF_REL);
            }
            else
            {
                err = d_FLS_StateMachineIf0(D_FLS_IF0_ICUM_DF_REL);
            }
        }
        else if (AG_D_FLS_CODE == target_area)
        {
            if ((AG_D_FLS_BANKA == bank) || (AG_D_FLS_BANKB == bank))
            {
                err = d_FLS_StateMachineIf0(D_FLS_IF0_ICUM_CF_REL);
            }
        }
        else
        {
            /* Free up resources for configuration and security settings */
            err = d_FLS_StateMachineIf0(D_FLS_IF0_ICUM_EXT_REL);
        }

        if (ER_OK == err)
        {
            if ((R_RFD_FACI0 == iface) &&
                (D_FLS_ZERO_U32 != (uint32_t)g_fls_ctrl_bak[iface].context.id))
            {
                (void)d_FLS_ExecResumption(iface);
            }
            else
            {
                g_fls_ctrl[iface].context.id = 0U;
                g_fls_ctrl[iface].context.priority = 0U;
                g_fls_ctrl[iface].context.target_area = D_FLS_ZERO_U32;
                g_fls_ctrl[iface].context.target_bank = D_FLS_ZERO_U32;
            }
        }
        else
        {
            /* Do nothing */
        }

        /* Enable EI Level Maskable Interrupt */
        ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    return (err);
}

/*******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0007]:[ICUMFW_UD_U2B_D_FLS_0007]
* Function Name : D_FLS_SetDfProgReq_ram
* Description   : Set the flag to detect if the PE side was in DF_PE mode.
* Arguments     : usr_df_prog_req
*                  setting value
* Return Value  : ER_OK
*                 ER_PARAM
********************************************************************************************/
uint32_t D_FLS_SetDfProgReq_ram(const uint8_t d_usr_df_prog_req)
{
    uint32_t err;

    switch ((uint32_t)d_usr_df_prog_req)
    {
        case AG_D_FLS_DF_PE_SET:
            /* Normal */
        case AG_D_FLS_DF_PE_CLR:
            /* Normal */
            err = ER_OK;
            g_usr_df_prog_req = d_usr_df_prog_req;
            break;
        default:
            err = ER_PARAM;
            break;
    }

    return (err);
}

/*******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0008]:[ICUMFW_UD_U2B_D_FLS_0008]
* Function Name : D_FLS_GetDfProgReq_ram
* Description   : Get the flag to detect if the PE side was in DF_PE mode.
* Arguments     : *usr_df_prog_req
*                  getting value
* Return Value  : ER_OK
*                 ER_PARAM
********************************************************************************************/
uint32_t D_FLS_GetDfProgReq_ram(uint8_t *p_d_usr_df_prog_req)
{
    uint32_t err;

    /* parameter check */
    if (NULL == p_d_usr_df_prog_req)
    {
        err = ER_PARAM;
    }
    else
    {
        err = ER_OK;
        *p_d_usr_df_prog_req = g_usr_df_prog_req;
    }

    return (err);
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0009]:[ICUMFW_UD_U2B_D_FLS_0009]
* Function Name : D_FLS_PrgPrepareOperation_ram
* Description   : Perform preprocessing during flash programming.
* Arguments     : none
* Return Value  : none
*******************************************************************************************/
void D_FLS_PrgPrepareOperation_ram(void)
{
     /* Write protect cancellation */
    d_FLS_WriteProtectOff_ram();

    return;
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0011]:[ICUMFW_UD_U2B_D_FLS_0011]
* Function Name : D_FLS_PrgDisposeOperation_ram
* Description   : Perform postprocessing during flash programming.
* Arguments     : none
* Return Value  : none
*******************************************************************************************/
void D_FLS_PrgDisposeOperation_ram(void)
{
    /* Write Protect setting */
    if ((ER_IN_PROCESS != g_fls_ctrl[R_RFD_FACI0].prm.d_status)
        && (ER_IN_PROCESS != g_fls_ctrl[R_RFD_FACI2].prm.d_status)
        && (ER_IN_PROCESS != g_fls_ctrl_bak[R_RFD_FACI0].prm.d_status)
        && (ER_IN_PROCESS != g_fls_ctrl_bak[R_RFD_FACI2].prm.d_status))
    {
        d_FLS_WriteProtectOn_ram();
    }
    else
    {
        /* do nothing */
    }

    return;
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0010]:[ICUMFW_UD_U2B_D_FLS_0010]
* Function Name : D_FLS_CheckECC_ram
* Description   : Check DataFlash ECC Error & Clear.
* Arguments     : clearflg
*                  ECC Error Clear flag
* Return Value  : ER_OK
*                 ER_D_FLS_ECC1EDF1
*                 ER_D_FLS_ECC2EDF1
*******************************************************************************************/
uint32_t D_FLS_CheckECC_ram(const uint8_t clearflg)
{
    uint32_t lul_fls_status;
    uint8_t  luc_clrflg;

    luc_clrflg = clearflg;

    /* Check ECC 2bit Error */
    if (ICUM_DFECCDEDF != 0UL)
    {
        lul_fls_status = ER_D_FLS_ECC2EDF1;
    }
    /* Check ECC 1bit Error */
    else if ((ICUM_DFECCICUM_DFSERSTR & 0xFUL) != 0UL)
    {
        lul_fls_status = ER_D_FLS_ECC1EDF1;
    }
    else
    {
        lul_fls_status = ER_OK;
        luc_clrflg = AG_D_FLS_NOTCLR;
    }

    /* ECC error clear */
    if ((uint32_t)AG_D_FLS_NOTCLR != (uint32_t)luc_clrflg)
    {
        ICUM_DFECCICUM_DFSERSTC = 0xFUL;
        ICUM_DFECCICUM_DFDERSTC = 1UL;
    }
    else
    {
        /* No ECC error clear */
    }

    return (lul_fls_status);
}

/****************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0012]:[ICUMFW_UD_U2B_D_FLS_0012]
* Function name : D_FLS_SetCfIntCallBackFunc_ram
* Description   : Call back registration.
* Arguments     : p_d_fls_cbfunc
*                  Address of callback function to register
* Return Value  : none
****************************************************************************************/
void D_FLS_SetCfIntCallBackFunc_ram(p_d_fls_cfint_addr *p_d_fls_cbfunc)
{
    p_d_fls_cbfunc_addr = p_d_fls_cbfunc;

    return ;
}
/****************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0013]:[ICUMFW_UD_U2B_D_FLS_0013]
* Function name : D_FLS_SetCFPEAck_ram
* Description   : ACK transmission to INTFSACSREQ after switching to RAM execution mode.
* Arguments     : none
* Return Value  : none
****************************************************************************************/
void D_FLS_SetCFPEAck_ram(void)
{
    if (R_RFD_FACI0 == g_d_cf_pe_notify_faci)
    {
        /* set Cf0 flash mode change Ack */
        (void)d_FLS_StateMachineIf0(D_FLS_IF0_CF_PE_ACK);
    }
}

/********************************************************************************
 FLS global Function definition
*********************************************************************************/
/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0036]:[ICUMFW_UD_U2B_D_FLS_0036]
* Function Name : D_FLS_CheckStatus_ram
* Description   : flash status checking.
*                 retvalue
*                  API return value.
* Return Value  : ER_OK
*                 ER_D_FLS_UNAVAILABLE
*******************************************************************************************/
uint32_t D_FLS_CheckStatus_ram(void)
{
    uint32_t lul_fls_status;

    /* status check */
    if (D_FLS_STS_UNINITIALIZE == g_fls_status)
    {
        /* FLS non-initialize */
        lul_fls_status = ER_D_FLS_UNAVAILABLE;
    }
    else
    {
        lul_fls_status = ER_OK;
    }

    return (lul_fls_status);

}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0015]:[ICUMFW_UD_U2B_D_FLS_0015]
* Function Name : d_FLS_CheckFaciErrors_ram
* Description   : Check FACI error
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*                 FLS_ERR_CMD
*                 FLS_ERR_ACCESS
*                 FLS_ERR_PROG
*                 FLS_ERR_FCU
*                 FLS_ERR_FCU_CRITICAL
*                 FLS_ERR_DBFULL_TIMEOUT
*                 FLS_ERR_PROTECTION
*                 FLS_ERR_FENTRYR
*                 FLS_ERR_SECURITY
*                 FLS_ERR_OTHER
*********************************************************************************/
uint32_t d_FLS_CheckFaciErrors_ram(void)
{
    T_u4_RFDReturn l_bu4_returnValue;
    uint32_t lstag_fls_err;

    lstag_fls_err = FLS_ERR_NONE;
    
    /* Acquires the state of the target FACI */
    l_bu4_returnValue = R_RFD_GetFaciStatus((T_u2_FACI)g_fls_selected_fpsys);
    
    if (R_RFD_OK == l_bu4_returnValue)
    {
        /* no error */
    }
    else
    {
        switch (l_bu4_returnValue)
        {
            case R_RFD_ERR_FHVE_PROTECT:
                lstag_fls_err = FLS_ERR_PROTECTION;
                break;

            case R_RFD_ERR_INTERNAL_DATA:
            case R_RFD_ERR_BLOCKPROTECTION0_DATA:
            case R_RFD_ERR_BLOCKPROTECTION1_DATA:
            case R_RFD_ERR_CONFIGURATION_DATA:
            case R_RFD_ERR_SECURITY_DATA:
                lstag_fls_err = FLS_ERR_FCU;
                break;
            
            case R_RFD_ERR_ERASE:
            case R_RFD_ERR_WRITE:
                lstag_fls_err = FLS_ERR_PROG;
                break;
            
            case R_RFD_ERR_INTERNAL_HW:
                lstag_fls_err = FLS_ERR_FENTRYR;
                break;
            
            case R_RFD_ERR_ACCESS_ENV:
                lstag_fls_err = FLS_ERR_ACCESS;
                break;
            
            default:
                lstag_fls_err = FLS_ERR_FENTRYR;
                break;
        }
    }
    
    return (lstag_fls_err);
}

/**********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0016]:[ICUMFW_UD_U2B_D_FLS_0016]
* Function Name : d_FLS_IssueCommand_ram
* Description   : FACI command issue (Program, Erase, Config Update)
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*                 FLS_ERR_DBFULL_TIMEOUT
***********************************************************************************/
uint32_t d_FLS_IssueCommand_ram(void)
{
    uint32_t lstag_fls_err;
    uint16_t lus_write_count;
    uint16_t lus_num_of_write;
    uint32_t lul_fsaddr;
    uint8_t *p_lp_data;
    uint32_t lul_p_data_index;
    uint32_t lul_data_size_in_bytes;
    uint32_t lus_write_size_in_bytes;
    uint32_t lul_wcmddata ;
    uint32_t lul_res;
    T_u4 l_pu4_Data[DF_MULTIW128_SIZE] ;
    
    lstag_fls_err = FLS_ERR_NONE;
    p_lp_data = (uint8_t *)(g_fls_ctrl[g_fls_selected_fpsys].prm.p_data);
    lul_p_data_index = g_fls_ctrl[g_fls_selected_fpsys].prm.p_num;
    lul_data_size_in_bytes = g_fls_ctrl[g_fls_selected_fpsys].prm.data_size_in_bytes;
    lul_res = FLS_ERR_NONE;

    lul_fsaddr = g_fls_ctrl[g_fls_selected_fpsys].prm.flash_addr + g_fls_ctrl[g_fls_selected_fpsys].prm.p_num;

    /* get write size */
    g_fls_ctrl[g_fls_selected_fpsys].prm.p_write_size_in_bytes(lul_fsaddr, &lus_write_size_in_bytes);

    if (FLS_ERR_NONE == lul_res)
    {
        if (D_FACI_CMD_BLOCK_ERASE == g_fls_ctrl[g_fls_selected_fpsys].prm.command_code)
        {
            /* Erases the contents of the data flash memory */
            R_RFD_EraseDFRequest(((T_u4_RfdAddress)lul_fsaddr), ((T_u4_RfdAddress)((lul_fsaddr + lus_write_size_in_bytes) - 1UL)));
        }
        else if ((D_FACI_CMD_PROGRAM == g_fls_ctrl[g_fls_selected_fpsys].prm.command_code)
        ||      (D_FACI_CMD_MULTI_PROGRAM == g_fls_ctrl[g_fls_selected_fpsys].prm.command_code))
        {
            if (NULL != p_lp_data)
            {
                /* Write data set */
                lus_num_of_write =   (uint16_t)(lus_write_size_in_bytes / sizeof(uint32_t));

                lus_write_count = 0U;

                /* Writes data for one block */
                for ( ; lul_p_data_index < lul_data_size_in_bytes; lul_p_data_index += sizeof(lul_wcmddata))
                {
                    lul_wcmddata  = ((uint32_t)p_lp_data[lul_p_data_index] & D_FLS_LSB_MASK);
                    lul_wcmddata |= (((uint32_t)p_lp_data[(lul_p_data_index + D_FLS_INDEX_1)] & D_FLS_LSB_MASK)
                                                                                                    << D_FLS_SHIFT_8BIT);
                    lul_wcmddata |= (((uint32_t)p_lp_data[(lul_p_data_index + D_FLS_INDEX_2)] & D_FLS_LSB_MASK)
                                                                                                    << D_FLS_SHIFT_16BIT);
                    lul_wcmddata |= (((uint32_t)p_lp_data[(lul_p_data_index + D_FLS_INDEX_3)] & D_FLS_LSB_MASK)
                                                                                                    << D_FLS_SHIFT_24BIT);

                    l_pu4_Data[lus_write_count] = ((T_u4)lul_wcmddata);

                    lus_write_count = (uint16_t)((uint32_t)lus_write_count + 1UL);
                    if ((uint32_t)lus_write_count >= (uint32_t)lus_num_of_write)
                    {
                        break;
                    }
                    else
                    {
                        /* go next Writes data */
                    }
                } /* end of for ( ; lul_p_data_index < lul_data_size_in_bytes; lul_p_data_index += sizeof(lul_wcmddata)) */

                /* If the size of the last block is less than the block size, fill it with 0xFF */
                for ( ; (uint32_t)lus_write_count < (uint32_t)lus_num_of_write ;
                        lus_write_count = (uint16_t)((uint32_t)lus_write_count + 1UL))
                {
                    /* 4byte(data) */
                    l_pu4_Data[lus_write_count] = (T_u4)D_FLS_FILL_DATA;
                }

            }   /* end of (NULL != p_lp_data) */
            else
            {
                /* No Write data */
            }
            
            /* Writes the data to the data flash memory */
            R_RFD_WriteDFRequest(((T_u4_RfdAddress)lul_fsaddr), ((T_u2)lus_write_size_in_bytes), ((T_pu4_RfdBuffer)&l_pu4_Data[0]));
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* do nothing */
    }

    /* Read Counter when Start of FACI command */
    D_OST_GetHardCounter(&g_fls_ctrl[g_fls_selected_fpsys].prm.faci_start_time);

    /* Clear Number of checks FACI command Completion */
    g_fls_ctrl[g_fls_selected_fpsys].prm.faci_check_counter = D_FLS_ZERO_U32;

    return (lstag_fls_err);
}

/********************************************************************************
 private Function definition
*********************************************************************************/
/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0017]:[ICUMFW_UD_U2B_D_FLS_0017]
* Function Name : d_FLS_SetBusGuardFromPE_ram
* Description   : Bus guard setting. (FACI0/1 Register and FACI0/1 Command Area)
* Arguments     : none
* Return Value  : none
*******************************************************************************************/
void d_FLS_SetBusGuardFromPE_ram(void)
{
    /* Enables write access of protected registers */
    PBGERRSLV61.PBGKCPROT.UINT32 = KCE_EN;

    /* Enable Bus Guard */
    if (R_RFD_FACI0 == g_fls_selected_fpsys)
    {
        /* Flash Sequencer Register Area and Command Access Area for CPU(PE)(FACI0) */
        /* => Set Access Guard for PE */
        PBG61.PBGPROT0_1.UINT32 |= ICUM_PBGPROT_EN;
    }

    /* Disables write access of protected registers */
    PBGERRSLV61.PBGKCPROT.UINT32 = KCE_DIS;

    return;
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0018]:[ICUMFW_UD_U2B_D_FLS_0018]
* Function Name : d_FLS_ClrBusGuardFromPE_ram
* Description   : Bus guard cancellation. (FACI0/1 Register and FACI0/1 Command area)
* Arguments     : none
* Return Value  : none
*******************************************************************************************/
void d_FLS_ClrBusGuardFromPE_ram(void)
{
    /* Enables write access of protected registers */
    PBGERRSLV61.PBGKCPROT.UINT32 = KCE_EN;

    /* Bus Guard Disabled */
    if (R_RFD_FACI0 == g_fls_selected_fpsys)
    {
        /* Flash Sequencer Register Area and Command Access Area for CPU(PE)(FACI0) */
        /* => Cancel Access Guard from PE */
        PBG61.PBGPROT0_1.UINT32 &= ~ICUM_PBGPROT_EN;
    }

    /* Disables write access of protected registers */
    PBGERRSLV61.PBGKCPROT.UINT32 = KCE_DIS;

    return;
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0019]:[ICUMFW_UD_U2B_D_FLS_0019]
* Function Name : d_FLS_WriteProtectOn_ram
* Description   : Write protect setting.
* Arguments     : none
* Return Value  : none
*******************************************************************************************/
static void d_FLS_WriteProtectOn_ram(void)
{
    /* Programming/erasure/DataFlashBlankChecking disabled */
    /* 
     * Set write protect for all FACIs, 
     * as write protection may be canceled for multiple FACIs.
    */
    (void)R_RFD_SetFHVE(R_RFD_FACI0, R_RFD_FHVE_PROTECT_ON);
    (void)R_RFD_SetFHVE(R_RFD_FACI2, R_RFD_FHVE_PROTECT_ON);
    
    return;
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0020]:[ICUMFW_UD_U2B_D_FLS_0020]
* Function Name : d_FLS_WriteProtectOff_ram
* Description   : Write Protect cancellation.
* Arguments     : none
* Return Value  : none
*******************************************************************************************/
static void d_FLS_WriteProtectOff_ram(void)
{
    /* 
     * Since write protection is canceled for each FACI, 
     * it is performed by the d_FLS_WriteProtectOffFaci_ram function 
     * called from the d_FLS_SelectFpsys_ram function.
     * This function does nothing.
    */

    return;
}

/******************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0155]:[ICUMFW_UD_U2B_D_FLS_0111]
* Function Name : d_FLS_WriteProtectOffFaci_ram
* Description   : Write Protect cancellation of each FACI.
* Arguments     : target_faci
*                   FACI number of operation target
* Return Value  : none
*******************************************************************************************/
static void d_FLS_WriteProtectOffFaci_ram(const uint32_t target_faci)
{
    /* Programming/erasure/DataFlashBlankChecking enabled */
    if (R_RFD_FACI2 == target_faci)
    {
        (void)R_RFD_SetFHVE(R_RFD_FACI2, R_RFD_FHVE_PROTECT_OFF);
    }
    else if (R_RFD_FACI0 == target_faci)
    {
        (void)R_RFD_SetFHVE(R_RFD_FACI0, R_RFD_FHVE_PROTECT_OFF);
    }
    else
    {
        return;
    }

    return;
}

/***************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0022]:[ICUMFW_UD_U2B_D_FLS_0022]
* Function Name : d_FLS_ForcedStopCommand_ram
* Description   : Issue a Forced Stop to cancel the FACI command-lock state
* Arguments     : none
* Return Value  : FLS_ERR_NONE
*                 ER_D_FLS_FSTOPTOUT
****************************************************************************************/
static uint32_t d_FLS_ForcedStopCommand_ram(void)
{
    uint32_t lul_res;
    T_u4_RFDReturn  l_bu4_returnValue ;
    
    /* Forcibly stops the operation on the target memory in the target FACI */
    l_bu4_returnValue = R_RFD_ForcedStopAndErrorClear((T_u2_FACI) g_fls_selected_fpsys);
    
    /* Check timeout */
    if (R_RFD_OK != l_bu4_returnValue)
    {
        lul_res = ER_D_FLS_FSTOPTOUT;
    }
    else
    {
        lul_res = FLS_ERR_NONE;
    }
    
    if ((R_RFD_FACI0 == g_fls_selected_fpsys)
        && (D_FLS_ZERO_U32 != (uint32_t)g_fls_ctrl_bak[g_fls_selected_fpsys].context.id))
    {
        g_fls_ctrl_bak[g_fls_selected_fpsys].prm.d_status = ER_D_FLS_RESUME;
    }
    else
    {
        /* Do nothing */
    }

    return (lul_res);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0023]:[ICUMFW_UD_U2B_D_FLS_0023]
* Function Name : d_FLS_SetOperatingMode_ram
* Description   : FCU operation mode set
* Arguments     : operating_mode
*                  R_RFD_MODE_DFPE
*                  R_RFD_MODE_CFPE
*                  R_RFD_MODE_READ
*               : comm_code
* Return Value  : ER_OK
*                 ER_D_FLS_SETOPRDYTOUT
*                 ER_D_FLS_SETOPMODTOUT
*                 ER_D_FLS_REJECTED
*********************************************************************************/
static uint32_t d_FLS_SetOperatingMode_ram(const uint16_t operating_mode, const uint8_t comm_code)
{
    uint32_t lul_res;
    uint16_t lus_fentryr;
    volatile uint32_t lul_dummy;
    uint32_t lul_timeout_cnt;
    T_u4_RFDReturn  l_bu4_returnValue ;
    
    l_bu4_returnValue = R_RFD_STS_BUSY;
    lul_timeout_cnt = D_FLS_TIMEOUT_CHKREG;
    do
    {
        l_bu4_returnValue = R_RFD_GetFaciSequenceReady((T_u2_FACI) g_fls_selected_fpsys);

        lul_timeout_cnt--;                /* !CERT-C rule INT30 (QAC message 3383) */
        
        /* Check timeout */
        if (0UL == lul_timeout_cnt)
        {
            lul_res = ER_D_FLS_SETOPRDYTOUT;
        }
        else
        {
            /* go to next step when FACI is ready */
        }
    } while (R_RFD_STS_BUSY == l_bu4_returnValue);

    if(R_RFD_STS_READY == l_bu4_returnValue)
    {
        lul_res = ER_OK;
    }
    
    if (ER_OK == lul_res)
    {
        l_bu4_returnValue = R_RFD_CheckPEMode((T_u2_FACI)g_fls_selected_fpsys, R_RFD_MODE_CFPE);
        if (R_RFD_OK == l_bu4_returnValue)
        {
            lus_fentryr = (uint16_t)R_RFD_MODE_CFPE;
        }
        else
        {
            lus_fentryr = (uint16_t)R_RFD_MODE_READ;
        }
        
        if ((uint32_t)R_RFD_MODE_READ != (uint32_t)operating_mode)
        {
            l_bu4_returnValue = R_RFD_CheckPEMode((T_u2_FACI)g_fls_selected_fpsys, R_RFD_MODE_READ);
            
            if (R_RFD_OK != l_bu4_returnValue)
            {
               /* Case Cancel command is return */
                if ((uint32_t)D_FACI_CMD_FORCED_STOP == (uint32_t)comm_code)
                {
                    lul_res = ER_OK;
                }
                else
                {
                    /* Another processor is in use */
                    lul_res = ER_D_FLS_REJECTED;
                }
            }
            else
            {
                /* go to next step when already PE mode */
            }
        }
        else
        {
            /* go to next step when operating_mode is R_RFD_MODE_READ */
        }
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK == lul_res)
    {
        __SYNCP(); /* Synchronize pipeline */

         /* Shifts the mode of the target memory of the target FACI to the P/E mode */
        (void)R_RFD_ShiftToPEMode((T_u2_FACI)g_fls_selected_fpsys, (T_en_FACIMode)operating_mode);
        
        /* Waiting for operation mode change  */
        /* Other than Read mode */
        if ((uint32_t)R_RFD_MODE_READ != (uint32_t)operating_mode)
        {
            lul_timeout_cnt = D_FLS_TIMEOUT_CHKREG;
            do
            {
                l_bu4_returnValue = R_RFD_CheckPEMode((T_u2_FACI)g_fls_selected_fpsys, (T_en_FACIMode)operating_mode);
                
                lul_timeout_cnt--;
                
                /* Check timeout */
                if (0UL == lul_timeout_cnt)
                {
                    lul_res = ER_D_FLS_SETOPRDYTOUT;
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            } while (R_RFD_STS_BUSY == l_bu4_returnValue);
        }
        else
        {
            /* go to next step when operating_mode is R_RFD_MODE_READ */
        }
    }
    else
    {
        /* Do nothing */
    }

    if ((ER_OK == lul_res)
        && ((uint32_t)R_RFD_MODE_CFPE == (uint32_t)lus_fentryr)
        && ((uint32_t)R_RFD_MODE_READ == (uint32_t)operating_mode))
    {
        /* When switching from R_RFD_MODE_CFPE to R_RFD_MODE_READ  */
        lul_dummy = ICUM_FCCTL.ICUM_FCBWCTL.UINT32;
        ICUM_FCCTL.ICUM_FCBWCTL.UINT32 = D_FLS_ZERO_U32;
        ICUM_FCCTL.ICUM_FCBWCTL.UINT32 |= FCBWCTL_ICHCLR;
        ICUM_FCCTL.ICUM_FCBWCTL.UINT32 |= lul_dummy;
        lul_dummy = ICUM_FCCTL.ICUM_FCBWCTL.UINT32;
        /* Synchronize memory */
        __SYNCM();
        /* Synchronize pipeline */
        __SYNCP();
        /* Re-prefetch after clearing the prefetched instructions and data. */
        __SYNCI();
    }
    else
    {
        /* Do not Clear I-Cache RAM and buffer */
    }

    return (lul_res);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0024]:[ICUMFW_UD_U2B_D_FLS_0024]
* Function Name : d_FLS_Err2Status_ram
* Description   : convert uint32_t to uint32_t
* Arguments     : error_code
*                   FLS private error code
* Return Value  : ER_OK
*                 ER_D_FLS_PROTECTIO
*                 ER_D_FLS_WRITE
*                 ER_D_FLS_ERASE
*                 ER_D_FLS_CANCEL
*                 ER_D_FLS_INTERNAL
*********************************************************************************/
static uint32_t d_FLS_Err2Status_ram(const uint32_t error_code)
{
    uint32_t lul_fls_status;

    if (FLS_ERR_NONE == error_code)
    {
        lul_fls_status = ER_OK;
    }
    else if (FLS_ERR_CANCEL_END == error_code)
    {   /* cancel normal end */
        lul_fls_status = ER_OK;
    }
    else if (D_FLS_ZERO_U32 != (error_code & FLS_ERR_PROTECTION))
    {
        lul_fls_status = ER_D_FLS_PROTECTION;
    }
    else if (D_FLS_ZERO_U32 != (error_code & FLS_ERR_PROG))
    {
        switch ((uint32_t)g_fls_ctrl[g_fls_selected_fpsys].prm.command_code)
        {
            case D_FACI_CMD_PROGRAM:
            case D_FACI_CMD_MULTI_PROGRAM:
                lul_fls_status = ER_D_FLS_WRITE;
                break;

            case D_FACI_CMD_BLOCK_ERASE:
                lul_fls_status = ER_D_FLS_ERASE;
                break;

            case D_FACI_CMD_FORCED_STOP:
                lul_fls_status = ER_D_FLS_CANCEL;
                break;

            case D_FACI_CMD_SECURITY_ERASE:
            case D_FACI_CMD_SECURITY_PROGRAM:
            case D_FACI_CMD_SWITCH_ERASE:
            case D_FACI_CMD_SWITCH_PROGRAM:
            case D_FACI_CMD_TAG_ERASE:
            case D_FACI_CMD_TAG_UPDATE:
                lul_fls_status = ER_D_FLS_WRITE;
                break;

            case D_FACI_CMD_STATUS_CLEAR:
                            /* Impossible !!     */
            case D_FACI_CMD_BLANK_CHECK:
                            /* Impossible !!     */
            case D_FACI_CMD_PE_RESUME:
                            /* Impossible !!     */
            default:
                lul_fls_status = ER_D_FLS_INTERNAL;
                break;
        } /* end of switch ((uint32_t)g_fls_ctrl[g_fls_selected_fpsys].prm.command_code) */
    }   /* end of else if (0U != (error_code & FLS_ERR_PROG)) */
    else
    {
        /* other */
        lul_fls_status = ER_D_FLS_INTERNAL;
    }
    return (lul_fls_status);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0025]:[ICUMFW_UD_U2B_D_FLS_0025]
* Function Name : d_FLS_SetReadMode_ram
* Description   : Set FENTRYR to read mode
* Arguments     : none
* Return Value  : none
*********************************************************************************/
static void d_FLS_SetReadMode_ram(void)
{
    volatile uint32_t lul_dummy;
    uint16_t lus_fentryr;
	T_u4_RFDReturn  l_bu4_returnValue ;
	
    l_bu4_returnValue = R_RFD_CheckPEMode((T_u2_FACI)g_fls_selected_fpsys, R_RFD_MODE_CFPE);
    
    if (R_RFD_OK == l_bu4_returnValue)
    {
        lus_fentryr = (uint16_t)R_RFD_MODE_CFPE;
    }
    else
    {
        lus_fentryr = (uint16_t)R_RFD_MODE_DFPE;
    }
    
    /* Shifts the mode of the flash memory of the target FACI to the read mode */
    (void)R_RFD_ShiftToReadMode((T_u2_FACI)g_fls_selected_fpsys);
    
    if ((uint32_t)R_RFD_MODE_CFPE == (uint32_t)lus_fentryr)
    {
        lul_dummy = ICUM_FCCTL.ICUM_FCBWCTL.UINT32;
        ICUM_FCCTL.ICUM_FCBWCTL.UINT32 = D_FLS_ZERO_U32;
        ICUM_FCCTL.ICUM_FCBWCTL.UINT32 |= FCBWCTL_ICHCLR;
        ICUM_FCCTL.ICUM_FCBWCTL.UINT32 |= lul_dummy;
        /* Dummy Read ( ICUM_FCCTLICUM_FCBWCTL = 0 Bus transaction execution) */
        lul_dummy = ICUM_FCCTLICUM_FCBWCTL;
        /* Synchronize memory */
        __SYNCM();
        /* Synchronize pipeline */
        __SYNCP();
        /* Re-prefetch after clearing the prefetched instructions and data. */
        __SYNCI();
    }
    else
    {
        /* Nothing to do for read mode/DF_PE mode */
    }

    return;
}

/********************************************************************************
* Function ID   : (not supported)
* Function Name : d_FLS_PreProcessingCancel_ram
* Description   : Preprocessing of PE operation (For cancel operation)
* Arguments     : none
* Return Value  : FLS_ERR_NONE : No error detected only
*********************************************************************************/
static uint32_t d_FLS_PreProcessingCancel_ram(void)
{
    return (FLS_ERR_NONE);
}

/********************************************************************************
* Function ID   : (not supported)
* Function Name : d_FLS_GetDummyWriteSize_ram
* Description   : Return dummy write size.(=0)
* Arguments     : flash_addr
*                  code flash address (Not use)
*               : *p_size
*                  write size
* Return Value  : FLS_ERR_NONE : No error detected only
*********************************************************************************/
static uint32_t d_FLS_GetDummyWriteSize_ram(UNUSED uint32_t flash_addr, uint32_t *p_size)
{
    *p_size = D_FLS_ZERO_U32;

    return (FLS_ERR_NONE);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0028]:[ICUMFW_UD_U2B_D_FLS_0028]
* Function Name : d_FLS_IssueCommandCtrl_ram
* Description   : FACI command issue (For cancel operation)
* Arguments     : none
* Return Value  : FLS_ERR_CANCEL_END
*                 ER_D_FLS_FSTOPTOUT
*********************************************************************************/
static uint32_t d_FLS_IssueCommandCtrl_ram(void)
{
    uint32_t     lstag_fls_err;
    uint8_t      retry;

    retry = (uint8_t)false;
    /* Set FACI Command code to FACICMD(8bit) */
    lstag_fls_err = d_FLS_ForcedStopCommand_ram();
    if (lstag_fls_err == ER_D_FLS_FSTOPTOUT)
    {
        /* retry */
        retry = (uint8_t)true;
    }
    else
    {
        /* Err Check */
        lstag_fls_err = g_fls_ctrl[g_fls_selected_fpsys].prm.p_error_check();
        if (lstag_fls_err == FLS_ERR_NONE)
        {
            /* Cancel Normal end */
            lstag_fls_err = FLS_ERR_CANCEL_END;
        }
        else
        {
            /* Err Check NG */
            /* retry */
            retry = (uint8_t)true;
        }
    }

    if ((uint32_t)true == (uint32_t)retry)
    {
        /* Retry */

        /* Set FACI Command code to FACICMD(8bit) */
        lstag_fls_err = d_FLS_ForcedStopCommand_ram();
        if (lstag_fls_err == ER_D_FLS_FSTOPTOUT)
        {
            /* Do nothing */
        }
        else
        {
            /* Error Check */
            lstag_fls_err = g_fls_ctrl[g_fls_selected_fpsys].prm.p_error_check();
            if (lstag_fls_err == FLS_ERR_NONE)
            {
                /* Cancel Normal end */
                lstag_fls_err = FLS_ERR_CANCEL_END;
            }
            else
            {
                /* Retry error */
            }
        }
    }
    else
    {
        /* Do nothing */
    }

    /* Error Check NG */
    return (lstag_fls_err);

}

/**************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0029]:[ICUMFW_UD_U2B_D_FLS_0029]
* Function Name : d_FLS_CheckProcessingCompletion_ram
* Description   : Check whether the command is completed
* Arguments     : none
* Return Value  : ER_OK  : FSTAT.FRDY = 1
*                        None of the below is in progress.
*                 ER_IN_PROCESS : FSTAT.FRDY = 0
*                        Processing of the command "Program", "Block Erase",
*                         "Program/Erasure Suspend", "Program/Erasure Resume",
*                         "Forced Stop", "Blank Check", "Config Program", 
*                         "Lock Bit Program", "Lock Bit Read", or "OTP Set" is in
*                         progress.
*                 ER_D_FLS_FACITOUT : FACI timeout
***************************************************************************************/
static uint32_t d_FLS_CheckProcessingCompletion_ram(void)
{
    uint32_t res;
    uint32_t lul_now;
    uint32_t lul_frdy;
    uint32_t saved_psw;
    T_u4_RFDReturn  l_bu4_returnValue ;
    
    res = ER_IN_PROCESS;
    /* Disable EI Level Maskable Interrupt */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    l_bu4_returnValue = R_RFD_GetFaciSequenceReady((T_u2_FACI)g_fls_selected_fpsys);
    
    if (R_RFD_STS_READY == l_bu4_returnValue)
    {
        lul_frdy = R_RFD_STS_READY;
    }
    else
    {
        lul_frdy = D_FLS_ZERO_U32;
    }
    
    /* Check FACI command time out */
     D_OST_GetHardCounter(&lul_now);

    /* Enable EI Level Maskable Interrupt */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    if (D_FLS_ZERO_U32 != lul_frdy)
    {
        res = ER_OK;
    }
    else
    {
        if (g_fls_ctrl[g_fls_selected_fpsys].prm.max_time
            < (uint32_t)(lul_now - g_fls_ctrl[g_fls_selected_fpsys].prm.faci_start_time))
        {
            /* Issue Forced Stop Command */
            (void)d_FLS_IssueCommandCtrl_ram();
            res = ER_D_FLS_FACITOUT;
        }
        else
        {
            g_fls_ctrl[g_fls_selected_fpsys].prm.faci_check_counter++;

            /* Max times of check FRDY bit */
            if (LIMIT_CHECK_FRDY < g_fls_ctrl[g_fls_selected_fpsys].prm.faci_check_counter)
            {
                /* Issue Forced Stop Command */
                (void)d_FLS_IssueCommandCtrl_ram();
                res = ER_D_FLS_FACITOUT;
            }
            else
            {
                 /* waiting FSTATR_FRDY */
                 /* do nothing yet */
            }
        }
    }
    return (res);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_driver.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/*******************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0014]:[ICUMFW_UD_U2B_D_FLS_0014]
* Function Name : D_FLS_CheckBlankFlag
* Description   : Check Blank Flags.
*                  If Blank Flag was 0, set non blank area adddress
*                  to global variable.
* Arguments     : target_addr
*                  Start address of objective Blank Check Area.
*                 data_size_in_bytes
*                  check size.(in 4-byte units)
*                 blkc_mode
*                  incremental mode of blank checking.
*                 p_result_status
*                  result of blank cheking.
* Return Value  : ER_OK
********************************************************************************/
uint32_t D_FLS_CheckBlankFlag(const uint32_t target_addr, const uint32_t data_size_in_bytes, const uint8_t blkc_mode,
                              uint32_t *p_result_status)
{
    uint32_t lul_res;
    uint32_t lul_timeout_cnt;
    T_u2_FACI       lus_faci_num;
    T_u4_RfdAddress lul_start_addr;
    T_u4_RfdAddress lul_end_addr;
    T_u4_RfdAddress lul_dummy_read;
    T_u4_RFDReturn  l_bu4_returnValue;

    *p_result_status = ST_D_FLS_BLANK;
    
    l_bu4_returnValue = R_RFD_STS_BUSY;
    lul_timeout_cnt = D_FLS_TIMEOUT_CHKREG;
    lul_start_addr = (T_u4_RfdAddress)target_addr;
    lul_end_addr = (T_u4_RfdAddress)(target_addr + data_size_in_bytes - 1);
    
    lus_faci_num = R_RFD_DFAddressToFaciNumber(lul_start_addr);
    d_FLS_WriteProtectOffFaci_ram((uint32_t)lus_faci_num);
    (void)R_RFD_ShiftToPEMode(lus_faci_num, R_RFD_MODE_DFPE);
    do
    {
        l_bu4_returnValue = R_RFD_CheckPEMode(lus_faci_num, R_RFD_MODE_DFPE);
        
        lul_timeout_cnt--;
        
        /* Check timeout */
        if (0UL == lul_timeout_cnt)
        {
            lul_res = ER_D_FLS_SETOPRDYTOUT;
            break;
        }
        else
        {
            /* Do nothing */
        }
    } while (R_RFD_OK != l_bu4_returnValue);
    
    if (R_RFD_OK != l_bu4_returnValue)
    {
        /* Do nothing */
    }
    else
    {
        R_RFD_BlankCheckDFRequest(lul_start_addr, lul_end_addr);
        
        lul_timeout_cnt = D_FLS_TIMEOUT_CHKREG;
        do
        {
            l_bu4_returnValue = R_RFD_GetFaciSequenceReady(lus_faci_num);
            
            lul_timeout_cnt--;
            
            /* Check timeout */
            if (0UL == lul_timeout_cnt)
            {
                lul_res = ER_D_FLS_SETOPRDYTOUT;
                break;
            }
            else
            {
                /* Do nothing */
            }
        } while (R_RFD_STS_BUSY == l_bu4_returnValue);

        if(R_RFD_STS_READY == l_bu4_returnValue)
        {
            lul_res = ER_OK;
        }
        
        /* Check the FACI status */
        if (lul_res == ER_OK)
        {
            l_bu4_returnValue = R_RFD_GetBlankCheckResult(lus_faci_num, &lul_dummy_read);
            
            /* Check if there are some space */
            if (R_RFD_STS_NOTBLANK == l_bu4_returnValue)
            {
                *p_result_status = ST_D_FLS_NON_BLANK;
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            lul_res = ER_D_FLS_INTERNAL;
        }
        
        (void)R_RFD_ShiftToReadMode(lus_faci_num);
    }
    d_FLS_WriteProtectOn_ram();
    
    return (lul_res);
}

/* self copy icum_driver_rom.text to icum_driver_ram   */
CHANGE_SEC(text, ".icum_driver_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0037]:[ICUMFW_UD_U2B_D_FLS_0037]
* Function Name : D_FLS_CheckContext_ram
* Description   : Check a context with the specified id.
* Arguments     : context_id
*                  Flash context id
*                 target_area
*                  Target area type
* Return Value  : ER_OK
*                 ER_IN_PROCESS
*                 ER_PENDING
*                 ER_PARAM
*********************************************************************************/
uint32_t D_FLS_CheckContext_ram(const uint16_t context_id, const uint32_t target_area, const uint32_t bank)
{
    uint32_t err;
    uint32_t interface;

    err = d_FLS_GetInterface_ram(target_area, bank, &interface);
    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        if (D_FLS_ZERO_U32 == (uint32_t)context_id)
        {
            /* If context id = 0, check whether state is in idle or busy. */
            if (D_FLS_ZERO_U32 != (uint32_t)g_fls_ctrl[interface].context.id)
            {
                err = ER_PENDING;
            }
            else
            {
                err = ER_OK;
            }
        }
        else if (((uint32_t)context_id == (uint32_t)g_fls_ctrl[interface].context.id)
            || ((uint32_t)context_id == (uint32_t)g_fls_ctrl_bak[interface].context.id))
        {
            /* This context is executing. */
            err = ER_IN_PROCESS;
        }
        else if ((R_RFD_FACI0 == interface)
                && (D_FLS_ZERO_U32 != (uint32_t)g_fls_ctrl[interface].context.id)
                && ((uint32_t)context_id != (uint32_t)g_fls_ctrl[interface].context.id)
                && (D_FLS_ZERO_U32 != (uint32_t)g_fls_ctrl_bak[interface].context.id))
        {
            /* Cannot accept new requests. */
            err = ER_PENDING;
        }
        else if ((R_RFD_FACI2 == interface)
                && (D_FLS_ZERO_U32 != (uint32_t)g_fls_ctrl[interface].context.id)
                && ((uint32_t)context_id != (uint32_t)g_fls_ctrl[interface].context.id))
        {
            /* Cannot accept new requests. */
            err = ER_PENDING;
        }
        else if ((AG_D_FLS_SECURITY == target_area)
                && (D_FLS_ZERO_U32 != (uint32_t)g_fls_ctrl[interface].context.id)
                && ((uint32_t)context_id != (uint32_t)g_fls_ctrl[interface].context.id))
        {
            /* Allow in idle only. */
            err = ER_PENDING;
        }
        else
        {
            /* Current state is idle or executing other context */
            err = ER_OK;
        }
    } /* end of else */

    return (err);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0041]:[ICUMFW_UD_U2B_D_FLS_0041]
* Function Name : d_FLS_GetInterface_ram
* Description   : Get Flash interface
* Arguments     : target_area
*                  Target area type
*                 bank
*                  Target bank
*                 p_interface
*                  Pointer to store interface id
* Return Value  : ER_OK
*                 ER_PARAM
*********************************************************************************/
static uint32_t d_FLS_GetInterface_ram(const uint32_t target_area, const uint32_t bank, uint32_t *p_interface)
{
    uint32_t err;
    uint32_t check_addr;

    /* bank argument check */
    check_addr = D_FLS_ZERO_U32;
    err = D_FLS_CheckAddr_ram(target_area, bank, D_FLS_ZERO_U32, sizeof(uint32_t), &check_addr);
    if (ER_OK == err)
    {
        if (NULL == p_interface)
        {
            err = ER_PARAM;
        }
        else
        {
            switch (target_area)
            {
                case AG_D_FLS_CODE:
                    if (AG_D_FLS_BANKB >= bank)
                    {
                        *p_interface = R_RFD_FACI0;
                    }
                    break;
                case AG_D_FLS_SECURITY:
                    *p_interface = R_RFD_FACI0;
                    break;
                case AG_D_FLS_DATA:
                    if (AG_D_FLS_BANK_ICUM == bank)
                    {
                        *p_interface = R_RFD_FACI2;
                    }
                    else if (AG_D_FLS_BANK_DATA0 == bank)
                    {
                        *p_interface = R_RFD_FACI0;
                    }
                    break;
                default:
                    err = ER_PARAM;
                    break;
            }
        }
    } /* end of if (ER_OK == err) */
    else
    {
        /* Do nothing */
    }

    return (err);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_driver.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0042]:[ICUMFW_UD_U2B_D_FLS_0042]
* Function Name : d_FLS_IssueSuspendCommand
* Description   : Check SUSRDY bit
* Arguments     : iface
*                  FACI I/F
* Return Value  : ER_OK
*                 ER_D_FLS_SUSPEND
*********************************************************************************/
static uint32_t d_FLS_IssueSuspendCommand(const uint32_t iface)
{
    uint32_t lul_res;
    T_u4_RFDReturn  l_bu4_returnValue ;
    
    l_bu4_returnValue = R_RFD_STS_BUSY;
    lul_res = ER_OK;
    
    l_bu4_returnValue = R_RFD_GetFaciSequenceReady((T_u2_FACI)iface);
    
    if (R_RFD_STS_READY == l_bu4_returnValue)
    {
        /* Do nothing */
    }
    else
    {
        (void)R_RFD_SuspendPERequest((T_u2_FACI)iface);
        lul_res = g_fls_ctrl_bak[iface].prm.p_error_check();
        if (FLS_ERR_NONE != lul_res)
        {
            lul_res = ER_D_FLS_SUSPEND;
        }
        else
        {
            l_bu4_returnValue = R_RFD_GetFaciSequenceReady((T_u2_FACI)iface);
            
            if (R_RFD_STS_READY == l_bu4_returnValue)
            {
                (void)d_FLS_ForcedStopCommand_ram();
                lul_res = ER_D_FLS_SUSPEND;
            }
            else
            {
                l_bu4_returnValue = R_RFD_GetFaciStatus((T_u2_FACI)iface);
                if ((l_bu4_returnValue == R_RFD_STS_ERASE_SUSPENDED) || (l_bu4_returnValue == R_RFD_STS_WRITE_SUSPENDED))
                {
                    /* Suspend success */
                    lul_res = ER_OK;
                }
                else
                {
                    /* Suspend failure */
                    lul_res = ER_D_FLS_SUSPEND;
                }
            }
        }
    }
    
    return (lul_res);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0043]:[ICUMFW_UD_U2B_D_FLS_0043]
* Function Name : d_FLS_SuspensionCommand
* Description   : Programming/Erasure Suspension Command
* Arguments     : iface
*                  FACI I/F
* Return Value  : ER_OK
*                 ER_D_FLS_SUSPEND
*********************************************************************************/
static uint32_t d_FLS_SuspensionCommand(const uint32_t iface)
{
    uint32_t lul_res;
    uint32_t          save_iface;
    T_u4_RFDReturn  l_bu4_returnValue ;
    
    save_iface = g_fls_selected_fpsys;
    g_fls_selected_fpsys = iface;
    lul_res = g_fls_ctrl_bak[iface].prm.p_error_check();
    if (FLS_ERR_NONE != lul_res)
    {
        lul_res = ER_D_FLS_SUSPEND;
    }
    else
    {
        l_bu4_returnValue = R_RFD_GetFaciStatus((T_u2_FACI)iface);
        
        if ((l_bu4_returnValue == R_RFD_STS_ERASE_SUSPENDED) || (l_bu4_returnValue == R_RFD_STS_WRITE_SUSPENDED))
        {
            /* Already P/E is completed or suspended. */
            lul_res = ER_OK;
        }
        else
        {
            lul_res = d_FLS_IssueSuspendCommand(iface);
        }
    } /* end of else */

    if (ER_OK == lul_res)
    {
        /* Mode reset */
        d_FLS_SetReadMode_ram();
    }
    else
    {
        /* Do nothing */
    }

    g_fls_selected_fpsys = save_iface;

    return (lul_res);
}

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0044]:[ICUMFW_UD_U2B_D_FLS_0044]
* Function Name : d_FLS_ResumptionCommand
* Description   : Programming/Erasure Resumption Command
* Arguments     : iface
*                  FACI I/F
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_ResumptionCommand(const uint32_t iface)
{
    volatile uint16_t   u16_dummy;
    uint32_t            lul_res;
    uint32_t            save_iface;
    T_u4_RFDReturn  l_bu4_returnValue ;
    
    save_iface = g_fls_selected_fpsys;
    g_fls_selected_fpsys = iface;
    if (D_FLS_ZERO_U32 != (uint32_t)g_fls_ctrl_bak[iface].context.id)
    {
        l_bu4_returnValue = R_RFD_CheckPEMode((T_u2_FACI)save_iface, (T_en_FACIMode)g_fls_ctrl_bak[iface].prm.ope_mode);
        
        if (l_bu4_returnValue != R_RFD_OK)
        {
            (void)R_RFD_ShiftToPEMode((T_u2_FACI)iface, (T_en_FACIMode)g_fls_ctrl_bak[iface].prm.ope_mode);
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

    l_bu4_returnValue = R_RFD_GetFaciStatus((T_u2_FACI)iface);
    if ((l_bu4_returnValue != R_RFD_STS_ERASE_SUSPENDED) && (l_bu4_returnValue != R_RFD_STS_WRITE_SUSPENDED))
    {
        lul_res = ER_OK;
    }
    else
    {
        /* FACI Command Programming/Erasure Resumption(8bit) */
        (void)R_RFD_ResumePERequest((T_u2_FACI)iface);
        D_OST_GetHardCounter(&g_fls_ctrl_bak[iface].prm.faci_start_time);

        lul_res = ER_OK;
    }
    g_fls_selected_fpsys = save_iface;
    
    return (lul_res);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_driver.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")

/********************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0034]:[ICUMFW_UD_U2B_D_FLS_0034]
* Function name: D_FLS_SetUPGPProtect_ram
* Description  : Enable/Disable the UPGP1 and UPGP2 protection
* Arguments    : flag1  : Enable(D_FLS_UPGP_ON) / Disable(D_FLS_UPGP_OFF)
*                flag2  : Enable(D_FLS_UPGP_ON) / Disable(D_FLS_UPGP_OFF)
* Private value: none
* Return Value : ER_OK
*********************************************************************************/
uint32_t D_FLS_SetUPGPProtect_ram(uint32_t flag1, uint32_t flag2)
{
    /* rewrite value */
    uint32_t value;
    /* retry count */
    uint32_t count;

    /* variable initialization */
    value = D_FLS_UPGP1_PROTECT | D_FLS_UPGP2_PROTECT;
    count = D_FLS_UPGP_RETRY;

    if (D_FLS_UPGP_OFF == flag1)
    {
        /* UPGP1 and UPGP2 are Disable */
        if (D_FLS_UPGP_OFF == flag2)
        {
            value &= ~(D_FLS_UPGP1_PROTECT | D_FLS_UPGP2_PROTECT);
        }
        /* UPGP1 is Disable, UPGP2 is Enable */
        else
        {
            value = D_FLS_UPGP2_PROTECT;
        }
    }
    else
    {
        /* UPGP1 is Enable, UPGP2 is Disable */
        if (D_FLS_UPGP_OFF == flag2)
        {
            value = D_FLS_UPGP1_PROTECT;
        }
        /* UPGP1 and UPGP2 are Enable */
        else
        {
            value = D_FLS_UPGP1_PROTECT | D_FLS_UPGP2_PROTECT;
        }
   }
    /* rewrite UP1EN and UP2EN */
    WRITE_PROT_REG(ICUM_CTLREG.ICUM_UPGPS.UINT32, value, count);

    return ER_OK;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0045]:[ICUMFW_UD_U2B_D_FLS_0045]
* Function name: D_FLS_RemapStart
* Description  : Remap area development
* Arguments    : (IN) p_uiTargetAddr : Remap target data address
*                (OUT) p_uiRemapAddr : Remapped address
* Return Value : ER_OK or ER_D_FLS_REMAP
****************************************************************************************/
uint32_t D_FLS_RemapStart(const uint32_t p_uiTargetAddr ,uint32_t **p_uiRemapAddr)
{
    uint32_t        count;
    uint32_t        err;
    uint8_t         result;

    err = ER_OK;
    /* Check UserBoot Area */
    result = d_FLS_IsUserBootAddr(p_uiTargetAddr);
    if ((uint32_t)false == (uint32_t)result)
    {
        /* Not UserBootArea */
        *p_uiRemapAddr = (uint32_t*)p_uiTargetAddr;
    }
    else
    {
        /* UserBootArea */
        /* Set Remap Area */
        count = MISC_PROT_RETRY;
        WRITE_PROT_REG(ICUM_REG_CTLREG_SICREMAP, ADDR_LOWER_USER_BOOT0, count);

        if (D_FLS_ZERO_U32 != count)
        {
            /* TIMEOUT not occured */
            *p_uiRemapAddr = (uint32_t *)(SICREMAP_DEFAULT_ADDRESS
                                          + ((uint32_t)p_uiTargetAddr - (uint32_t)ADDR_LOWER_USER_BOOT0));
        }
        else
        {
            /* TIMEOUT err occured */
            err = ER_D_FLS_REMAP;
        }
    }
    return err;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0046]:[ICUMFW_UD_U2B_D_FLS_0046]
* Function name: D_FLS_RemapEnd
* Description  : Initialization Remap Area
* Arguments    : None
* Return Value : None
****************************************************************************************/
void D_FLS_RemapEnd(void)
{
    uint32_t        count;

    /* Initialization Remap Area */
    count = MISC_PROT_RETRY;
    WRITE_PROT_REG(ICUM_REG_CTLREG_SICREMAP, SICREMAP_DEFAULT_ADDRESS, count);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_driver.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0047]:[ICUMFW_UD_U2B_D_FLS_0047]
* Function name: D_FLS_IsUserBootArea
* Description  : Check of memory address for UserBoot or not
* Arguments    : (in) start_addr : address to be checked
*                (in) size       : data size
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_FLS_IsUserBootArea(const uint32_t start_addr, const uint32_t size)
{
    uint32_t err;
    uint32_t end_addr;

    err = ER_PARAM;
    /* over address check */
    if ((D_FLS_MAX_ADDRESS - start_addr) < size)
    {
        /* end address over 0xFFFFFFFF */
    }
    else
    {
        /* Check UserBoot Area */
        end_addr = start_addr + size;
        if (D_FLS_ZERO_U32 < size)
        {
            end_addr--;
        }
        else
        {
            /* do nothing */
        }

        /* Two UserBootAreas exist */
        if (((ADDR_LOWER_USER_BOOT0 <= start_addr) && (ADDR_UPPER_USER_BOOT0 >= end_addr)) 
            || ((ADDR_LOWER_USER_BOOT1 <= start_addr) && (ADDR_UPPER_USER_BOOT1 >= end_addr)))
        {
            /* Within UserBootArea */
            err = ER_OK;
        }
        else
        {
            /* Out of UserBootArea */
            /* do nothing */
        }
    }
    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0048]:[ICUMFW_UD_U2B_D_FLS_0048]
* Function name: d_FLS_IsUserBootAddr
* Description  : Check of memory address for UserBoot or not
* Arguments    : (in) addr : address to be checked
* Return Value : true
*                false
***************************************************************************************/
static uint8_t d_FLS_IsUserBootAddr(const uint32_t addr)
{
    uint8_t ret;

    ret = (uint8_t)false;
    /* Two UserBootAreas exist */
    if (((ADDR_LOWER_USER_BOOT0 <= addr) && (ADDR_UPPER_USER_BOOT0 >= addr)) 
        || ((ADDR_LOWER_USER_BOOT1 <= addr) && (ADDR_UPPER_USER_BOOT1 >= addr)))
    {
        /* Within UserBootArea */
        ret = (uint8_t)true;
    }
    else
    {
        /* Out of UserBootArea */
        /* do nothing */
    }

    return ret;
}

/* self copy icum_driver_rom.text to icum_driver_ram   */
CHANGE_SEC(text, ".icum_driver_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0049]:[ICUMFW_UD_U2B_D_FLS_0049]
* Function Name : D_FLS_DFModeSwitchResInt_ram
* Description   : DF mode switching response interrupt handler
* Arguments     : int_channel
*                  Channel number of interrupt
*                    ICUM_COMM_ICH_FM_DF_PRG_NOTIFY
*                    ICUM_COMM_ICH_FM_DF_PRG_ERROR
*                 p_isd
*                  ISD address value ( Not used in this function )
* Return Value  : N/A
*********************************************************************************/
void D_FLS_DFModeSwitchResInt_ram(icum_comm_int_from_channel_t int_channel, r_icumif_isd_t *p_isd)
{
    switch (int_channel)
    {
        case ICUM_COMM_ICH_FM_DF_PRG_NOTIFY:
            /* Set acknowledgement response */
            g_sec_df_prog_res = FLS_MODE_SW_RESP_ACK;
            break;
        case ICUM_COMM_ICH_FM_DF_PRG_ERROR:
            /* Set negative-acknowledgement response */
            g_sec_df_prog_res = FLS_MODE_SW_RESP_NACK;
            break;
        default:
            /* Do nothing */
            break;
    } /* end of switch (int_channel) */
}

/*************************************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0108]:[ICUMFW_UD_U2B_D_FLS_0108]
* Function Name : d_FLS_HandlerCommandReq_ram
* Description   : Performs the issuing process of the FACI command.
* Arguments     : fpsys
*                  FACI number
* Return Value  : ER_OK
*                 ER_IN_PROCESS
*                 ER_D_FLS_NON_BLANK
*                 ER_D_FLS_PROTECTION
*                 ER_D_FLS_WRITE
*                 ER_D_FLS_ERASE
*                 ER_D_FLS_INTERNAL
*                 ER_D_FLS_SETOPRDYTOUT
*                 ER_D_FLS_REJECTED
*                 ER_D_FLS_SETOPMODTOUT
*                 ER_D_FLS_FSTOPTOUT
*                 ER_D_FLS_FACITOUT
**************************************************************************************************/
uint32_t d_FLS_HandlerCommandReq_ram(uint32_t fpsys)
{
    uint32_t lstag_fls_err;
    uint32_t lul_fls_status;

    lul_fls_status = g_fls_ctrl[fpsys].prm.d_status;

    /* request check */
    /* accepted the request API or cancel API ? */
    if ((uint32_t)D_FLS_CMDREQ_ON == (uint32_t)g_fls_ctrl[fpsys].prm.commad_req)
    {
        g_fls_ctrl[fpsys].prm.commad_req = D_FLS_CMDREQ_OFF;

        /* pre processing */
        lstag_fls_err = g_fls_ctrl[fpsys].prm.p_pre_processing();
        if (FLS_ERR_NONE != lstag_fls_err)
        {
            lul_fls_status = d_FLS_Err2Status_ram(lstag_fls_err);
        }
        else
        {
            /* P/E mode */
            lul_fls_status = d_FLS_SetOperatingMode_ram(g_fls_ctrl[fpsys].prm.ope_mode,
                                                        g_fls_ctrl[fpsys].prm.command_code);
            if ((uint32_t)ER_OK != lul_fls_status)
            {
                d_FLS_SetReadMode_ram();
            }
            else
            {
                lstag_fls_err = g_fls_ctrl[fpsys].prm.p_faci_commad_set();
                if (FLS_ERR_NONE != lstag_fls_err)
                {
                    d_FLS_SetReadMode_ram();
                    lul_fls_status = d_FLS_Err2Status_ram(lstag_fls_err);
                }
                else
                {
                    lul_fls_status = ER_IN_PROCESS;
                }
            }
        } /* end of else */
    } /* end of if ((uint32_t)D_FLS_CMDREQ_ON == (uint32_t)g_fls_ctrl[fpsys].prm.commad_req) */
    else
    {
        /* Non request, Nothing to do. */
    }

    return (lul_fls_status);
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/****************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0156]:[ICUMFW_UD_U2B_D_FLS_0156]
* Function name: D_FLS_ChangeNoRemapAddr(
* Description  : Restore the remapped address
* Arguments    : (IN) p_uiTargetAddr : target data address
*                (OUT) p_uiNoRemapAddr : No Remaped address
* Return Value : ER_OK
****************************************************************************************/
uint32_t D_FLS_ChangeNoRemapAddr(const uint32_t p_uiTargetAddr ,uint32_t **p_uiNoRemapAddr)
{
    uint8_t         result;

    /* Check Remap Area */
    result = d_FLS_IsRemapAddr(p_uiTargetAddr);
    if ((uint32_t)false == (uint32_t)result)
    {
        /* Not Remap Area */
        *p_uiNoRemapAddr = (uint32_t*)p_uiTargetAddr;
    }
    else
    {
        *p_uiNoRemapAddr = (uint32_t *)(ADDR_LOWER_USER_BOOT0
                                      + ((uint32_t)p_uiTargetAddr - (uint32_t)SICREMAP_DEFAULT_ADDRESS));
    }
    return ER_OK;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_FLS_0157]:[ICUMFW_UD_U2B_D_FLS_0157]
* Function name: d_FLS_IsRemapAddr
* Description  : Check of memory address for Remap window area or not
* Arguments    : (in) addr : address to be checked
* Return Value : true
*                false
***************************************************************************************/
static uint8_t d_FLS_IsRemapAddr(const uint32_t addr)
{
    uint8_t ret;

    ret = (uint8_t)false;
    /* Check RemapWindowArea */
    if ((SICREMAP_DEFAULT_ADDRESS <= addr) && (SICREMAP_UPPER_ADDRESS >= addr))
    {
        /* Within RemapWindowArea */
        ret = (uint8_t)true;
    }
    else
    {
        /* Out of RemapWindowArea */
        /* do nothing */
    }

    return ret;
}
