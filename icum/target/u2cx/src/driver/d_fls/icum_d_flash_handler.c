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
#include "icum_common_pub.h"
#include "icum_int_declaration_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_d_flash_pub.h"
#include "icum_d_flash.h"

#include "r_rfd_constant.h"
#include "r_rfd_common.h"
#include "r_rfd_df.h"
/********************************************************************************
 QAC Suppression message list
*********************************************************************************/

/********************************************************************************
 Memory allocation section specification
*********************************************************************************/
CHANGE_SEC(text, ".icum_handler.text")
CHANGE_SEC(rodata, ".icum_handler.rodata")
CHANGE_SEC(data, ".icum_handler.data")
CHANGE_SEC(bss, ".icum_handler.bss")

/********************************************************************************
 Macro definitions
*********************************************************************************/

/*******************************************************************************
 Typedef definitions
*********************************************************************************/

/********************************************************************************
 Exported global variables (to be accessed by other files)
*********************************************************************************/

/*******************************************************************************
* Functions
********************************************************************************/

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0105]:[ICUMFW_UD_U2B_D_FLS_0105]
* Function Name : INTFSACSREQ0
* Description   : FACI0 programming request interrupt handler from PE
* Arguments     : none
* Return Value  : none
*********************************************************************************/
void INTFSACSREQ0(void)
{
    uint32_t    err;
    uint8_t     pe_status;

    err = ER_OK;
    pe_status = D_FLS_ZERO_U8;

    /* Judge the flash area accessed by PE and the start / end of access */
    err = D_COMM_GetPEStatusInfo(&pe_status);
    if (ER_OK == err)
    {
        if ((uint8_t)ICUM_COMM_STS_DF_PRG == (pe_status & (uint8_t)ICUM_COMM_STS_DF_PRG))
        {
            if (PEFSACS0REQ_ACSREQ == (DFLS_PEFSACS0REQ & PEFSACS0REQ_ACSREQ))
            {
                /* DF0 P/E mode Entry */
                err = d_FLS_StateMachineIf0(D_FLS_IF0_HOST_DF_ACQ);
            }
            else
            {
                /* DF0 Read Mode Entry */
                err = d_FLS_StateMachineIf0(D_FLS_IF0_HOST_DF_REL);
            }
        }
        else if ((uint8_t)ICUM_COMM_STS_CF_PRG == (pe_status & (uint8_t)ICUM_COMM_STS_CF_PRG))
        {
            if (PEFSACS0REQ_ACSREQ == (DFLS_PEFSACS0REQ & PEFSACS0REQ_ACSREQ))
            {
                /* CF0 P/E mode Entry */
                err = d_FLS_StateMachineIf0(D_FLS_IF0_HOST_CF_ACQ);
            }
            else
            {
                /* CF0 Read Mode Entry */
                err = d_FLS_StateMachineIf0(D_FLS_IF0_HOST_CF_REL);
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

    /* Check StateMachine */
    if (ER_OK != err)
    {
        /* Sataus Error */
        ASSERT(0);
    }
    else
    {
        if ((uint32_t)g_d_cf_pe_notify != (uint32_t)CF_PE_INIT)
        {
            /* Call back to notify CF_P/E Mode to start or end */
            g_d_cf_pe_notify_faci = R_RFD_FACI0;
            p_d_fls_cbfunc_addr((uint32_t)g_d_cf_pe_notify);
            g_d_cf_pe_notify = CF_PE_INIT;
        }
        else
        {
            /* No notification */
        }
    }

    return;
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_handler_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0106]:[ICUMFW_UD_U2B_D_FLS_0105]
* Function Name : INTFSACSREQ0_RAM
* Description   : FACI0 programming request interrupt handler from PE
* Arguments     : none
* Return Value  : none
*********************************************************************************/
void INTFSACSREQ0_RAM(void)
{
    uint32_t    err;
    uint8_t     pe_status;

    err = ER_OK;
    pe_status = D_FLS_ZERO_U8;

    /* Judge the flash area accessed by PE and the start / end of access */
    err = D_COMM_GetPEStatusInfo(&pe_status);
    if (ER_OK == err)
    {
        if ((uint8_t)ICUM_COMM_STS_DF_PRG == (pe_status & (uint8_t)ICUM_COMM_STS_DF_PRG))
        {
            if (PEFSACS0REQ_ACSREQ == (DFLS_PEFSACS0REQ & PEFSACS0REQ_ACSREQ))
            {
                /* DF0 P/E mode Entry */
                err = d_FLS_StateMachineIf0(D_FLS_IF0_HOST_DF_ACQ);
            }
            else
            {
                /* DF0 Read Mode Entry */
                err = d_FLS_StateMachineIf0(D_FLS_IF0_HOST_DF_REL);
            }
        }
        else if ((uint8_t)ICUM_COMM_STS_CF_PRG == (pe_status & (uint8_t)ICUM_COMM_STS_CF_PRG))
        {
            if (PEFSACS0REQ_ACSREQ == (DFLS_PEFSACS0REQ & PEFSACS0REQ_ACSREQ))
            {
                /* CF0 P/E mode Entry */
                err = d_FLS_StateMachineIf0(D_FLS_IF0_HOST_CF_ACQ);
            }
            else
            {
                /* CF0 Read Mode Entry */
                err = d_FLS_StateMachineIf0(D_FLS_IF0_HOST_CF_REL);
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

    /* Check StateMachine */
    if (ER_OK != err)
    {
        /* Sataus Error */
        ASSERT(0);
    }
    else
    {
        if ((uint32_t)g_d_cf_pe_notify != (uint32_t)CF_PE_INIT)
        {
            /* Call back to notify CF_P/E Mode to start or end */
            p_d_fls_cbfunc_addr((uint32_t)g_d_cf_pe_notify);
            g_d_cf_pe_notify = CF_PE_INIT;
        }
        else
        {
            /* No notification */
        }
    }

    return;
}


/* Code executed on RAM             */
CHANGE_SEC(text, default)
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, default)

