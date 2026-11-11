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

#include "r_rfd_constant.h"
#include "r_rfd_common.h"
#include "r_rfd_df.h"
/********************************************************************************
 QAC Suppression message list
*********************************************************************************/

/********************************************************************************
 Memory allocation section specification
*********************************************************************************/
CHANGE_SEC(text, ".icum_driver.text")
CHANGE_SEC(rodata, ".icum_driver.rodata")
CHANGE_SEC(data, ".icum_driver.data")
CHANGE_SEC(bss, ".icum_driver.bss")

/********************************************************************************
 Macro definitions
*********************************************************************************/

/*******************************************************************************
 Typedef definitions
*********************************************************************************/
typedef uint32_t (*Func_t)(void);

/********************************************************************************
 Exported global variables (to be accessed by other files)
*********************************************************************************/
/* FACI State machine status variables for FACI exclusive control. */
uint8_t g_d_cf_pe_notify  = CF_PE_INIT;

/********************************************************************************
 Private function prototypes
*********************************************************************************/
/* ===================================================
    FACI Exclusive control state machine
======================================================*/

/*-----------------------------------------------
    state machine common handler
-------------------------------------------------*/
static uint32_t d_FLS_StateNone(void);
static uint32_t d_FLS_StateUnavailableError(void);
static uint32_t d_FLS_StateInternalError(void);
static uint32_t d_FLS_StatePending(void);

/*-----------------------------------------------
 E01 : CF0/1 Programming Request (CF_P/E) interrupt
-------------------------------------------------*/
static uint32_t d_FLS_If0StHostIdle_evCf0Prog(void);
static uint32_t d_FLS_If0StHostCf_evCf0Prog(void);
static uint32_t d_FLS_If0StHostDf_evCf0Prog(void);

/*-----------------------------------------------
 E02 : CF0/1 Programming Request (Read) interrupt
-------------------------------------------------*/
static uint32_t d_FLS_If0StHostCf_evCf0Read(void);

/*-----------------------------------------------
 E03 : DF0/1 Programming Request (DF_P/E) interrupt
-------------------------------------------------*/
static uint32_t d_FLS_If0StHostCf_evDf0Prog(void);

/*-----------------------------------------------
 E04 : DF0/1 Programming Request (Read) interrupt
-------------------------------------------------*/
static uint32_t d_FLS_If0StHostDf_evDf0Read(void);

/*-----------------------------------------------
 E05 : CF0/1 ACK Send Request
-------------------------------------------------*/
static uint32_t d_FLS_If0StCFAcknowledge(void);
static uint32_t d_FLS_If0StDFAcknowledge(void);

/*-----------------------------------------------
 E06 : Get Resources (Code flash)
-------------------------------------------------*/
static uint32_t d_FLS_If0StIdle_evIcumCf0Get(void);
static uint32_t d_FLS_If0StIcumDf_evIcumCf0Get(void);

/*-----------------------------------------------
 E07 : Free Resources (Code flash)
-------------------------------------------------*/
static uint32_t d_FLS_If0StIcumCf_evIcumCf0Rel(void);
static uint32_t d_FLS_If0StIcumAll_evIcumCf0Rel(void);

/*-----------------------------------------------
 E08 : Get Resources (User data flash)
-------------------------------------------------*/
static uint32_t d_FLS_If0StIdle_evIcumDf0Get(void);
static uint32_t d_FLS_If0StIcumCf_evIcumDf0Get(void);

/*-----------------------------------------------
 E09 : Free Resources (User data flash)
-------------------------------------------------*/
static uint32_t d_FLS_If0StIcumDf_evIcumDf0Rel(void);
static uint32_t d_FLS_If0StIcumAll_evIcumDf0Rel(void);

/*-----------------------------------------------
 E10 : Get Resources (Config flash)
-------------------------------------------------*/
static uint32_t d_FLS_If0StIdle_evConfigGet(void);

/*-----------------------------------------------
 E11 : Free Resources (Config flash)
-------------------------------------------------*/
static uint32_t d_FLS_If0StConfig_evConfigRel(void);

/*-----------------------------------------------
 E12 : Get Resources (Secure data flash)
-------------------------------------------------*/
static uint32_t d_FLS_If2StIdle_evIcumDf2Get(void);

/*-----------------------------------------------
 E13 : Free Resources (Secure data flash)
-------------------------------------------------*/
static uint32_t d_FLS_If2StHostDf_evIcumDf2Rel(void);

/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
static Func_t g_machine_tbl_0[D_FLS_IF0_EVT_TERM][D_FLS_IF0_STA_TERM] =
{
    { d_FLS_If0StHostIdle_evCf0Prog,  d_FLS_If0StHostCf_evCf0Prog,    d_FLS_If0StHostDf_evCf0Prog,     
      d_FLS_StateNone,                d_FLS_StateNone,                d_FLS_StateNone,                 
      d_FLS_StateNone               },
    { d_FLS_StateNone,                d_FLS_If0StHostCf_evCf0Read,    d_FLS_StateNone,                 
      d_FLS_StateNone,                d_FLS_StateNone,                d_FLS_StateNone,                 
      d_FLS_StateNone               },
    { d_FLS_If0StDFAcknowledge,       d_FLS_If0StHostCf_evDf0Prog,    d_FLS_If0StDFAcknowledge,        
      d_FLS_StateNone,                d_FLS_StateNone,                d_FLS_StateNone,                 
      d_FLS_StateNone               },
    { d_FLS_StateNone,                d_FLS_StateNone,                d_FLS_If0StHostDf_evDf0Read,     
      d_FLS_StateNone,                d_FLS_StateNone,                d_FLS_StateNone,                 
      d_FLS_StateNone               },
    { d_FLS_If0StCFAcknowledge,       d_FLS_If0StCFAcknowledge,       d_FLS_StateInternalError,        
      d_FLS_StateInternalError,       d_FLS_StateInternalError,       d_FLS_StateInternalError,        
      d_FLS_StateInternalError      },
    { d_FLS_If0StIdle_evIcumCf0Get,   d_FLS_StateUnavailableError,    d_FLS_StateUnavailableError,     
      d_FLS_StateNone,                d_FLS_If0StIcumDf_evIcumCf0Get, d_FLS_StateUnavailableError,     
      d_FLS_StateUnavailableError   },
    { d_FLS_StateInternalError,       d_FLS_StateInternalError,       d_FLS_StateInternalError,        
      d_FLS_If0StIcumCf_evIcumCf0Rel, d_FLS_StateInternalError,       d_FLS_If0StIcumAll_evIcumCf0Rel, 
      d_FLS_StateInternalError      },
    { d_FLS_If0StIdle_evIcumDf0Get,   d_FLS_StateUnavailableError,    d_FLS_StateUnavailableError,     
      d_FLS_If0StIcumCf_evIcumDf0Get, d_FLS_StateNone,                d_FLS_StateUnavailableError,     
      d_FLS_StateUnavailableError   },
    { d_FLS_StateInternalError,       d_FLS_StateInternalError,       d_FLS_StateInternalError,        
      d_FLS_StateInternalError,       d_FLS_If0StIcumDf_evIcumDf0Rel, d_FLS_If0StIcumAll_evIcumDf0Rel, 
      d_FLS_StateInternalError      },
    { d_FLS_If0StIdle_evConfigGet,    d_FLS_StateUnavailableError,    d_FLS_StateUnavailableError,     
      d_FLS_StatePending,             d_FLS_StatePending,             d_FLS_StatePending,              
      d_FLS_StatePending            },
    { d_FLS_StateInternalError,       d_FLS_StateInternalError,       d_FLS_StateInternalError,        
      d_FLS_StateInternalError,       d_FLS_StateInternalError,       d_FLS_StateInternalError,        
      d_FLS_If0StConfig_evConfigRel }
};
static Func_t g_machine_tbl_2[D_FLS_IF2_EVT_TERM][D_FLS_IF2_STA_TERM] =
{
    { d_FLS_If2StIdle_evIcumDf2Get, d_FLS_StatePending             },
    { d_FLS_StateInternalError,     d_FLS_If2StHostDf_evIcumDf2Rel }
};

/*******************************************************************************
* Functions
********************************************************************************/
/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_driver_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")

/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0100]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_StateMachineIf0
* Description   : FACI Exclusive control state machine
* Arguments     : none
* Return Value  : ER_OK
*                 ER_PENDING
*                 ER_D_FLS_INTERNAL
*                 ER_D_FLS_UNAVAILABLE
*********************************************************************************/
uint32_t d_FLS_StateMachineIf0 (const uint8_t event_code)
{
    uint32_t err;

    if ((uint32_t)event_code < (uint32_t)D_FLS_IF0_EVT_TERM)
    {
        err = g_machine_tbl_0[event_code][g_fls_ctrl[R_RFD_FACI0].status]();
        if ((uint32_t)D_FLS_IF0_IDLE == (uint32_t)g_fls_ctrl[R_RFD_FACI0].status)
        {
            g_usr_df_prog_req = AG_D_FLS_DF_PE_CLR;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        err = ER_D_FLS_INTERNAL;
    }
    return (err);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_driver_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0102]:[ICUMFW_UD_U2B_D_FLS_0102]
* Function Name : d_FLS_StateMachineIf2
* Description   : FACI Exclusive control state machine
* Arguments     : none
* Return Value  : ER_OK
*                 ER_PENDING
*                 ER_D_FLS_INTERNAL
*********************************************************************************/
uint32_t d_FLS_StateMachineIf2(const uint8_t event_code)
{
    uint32_t err;

    if ((uint32_t)event_code < (uint32_t)D_FLS_IF2_EVT_TERM)
    {
        err = g_machine_tbl_2[event_code][g_fls_ctrl[R_RFD_FACI2].status]();
    }
    else
    {
        err = ER_D_FLS_INTERNAL;
    }
    return (err);
}


/* Code executed on ROM             */
CHANGE_SEC(text, ".icum_handler.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0112]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StHostIdle_evCf0Prog
* Description   : Event Handler E01
*               : CF0 Programing Reqest Handler  (CF_P/E)
* Arguments     : none
* Return Value  : uint32_t : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StHostIdle_evCf0Prog(void)
{
    /* "CF P/E START " Notify to ISSD */
    g_d_cf_pe_notify = CF_PE_START;
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_IDLE;
    return (ER_OK);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0113]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StHostCf_evCf0Prog
* Description   : Event Handler E01
*               : CF0 Programing Reqest Handler  (CF_P/E)
* Arguments     : none
* Return Value  : uint32_t : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StHostCf_evCf0Prog(void)
{
    /* "CF P/E START " Notify to ISSD    */
    g_d_cf_pe_notify = CF_PE_START;
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_HOSTCF;
    return (ER_OK);
}

/* Code executed on ROM             */
CHANGE_SEC(text, ".icum_handler.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0114]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StHostDf_evCf0Prog
* Description   : Event Handler E01
*               : CF0 Programing Reqest Handler  (CF_P/E)
* Arguments     : none
* Return Value  : uint32_t : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StHostDf_evCf0Prog(void)
{
    /* "CF P/E START " Notify to ISSD */
    g_d_cf_pe_notify = CF_PE_START;
    /* DF0: 0 -> 1 */
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_IDLE;
    return (ER_OK);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0115]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StHostCf_evCf0Read
* Description   : Event Handler E02
*               : CF0 Programing Reqest Handler  (CF_Read)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StHostCf_evCf0Read(void)
{
    /* "CF P/E END " Notify to ISSD    */
    g_d_cf_pe_notify = CF_PE_END;
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_IDLE;
    return (ER_OK);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0116]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StHostCf_evDf0Prog
* Description   : Event Handler E03
*               : DF0 Programing Reqest Handler  (DF_P/E)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StHostCf_evDf0Prog(void)
{
    /* CF0 Free */
    g_d_cf_pe_notify = CF_PE_END;

    /* Respons DF0 ACK to Host */
    DFLS_ICUM_FLPRG0ACK = (DFLS_ICUM_FLPRG0ACK | ICUM_FLPRG0ACK_ACSACK0);
    g_usr_df_prog_req = AG_D_FLS_DF_PE_SET;
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_HOSTDF;
    return (ER_OK);
}

/* Code executed on ROM             */
CHANGE_SEC(text, ".icum_handler.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0117]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StHostDf_evDf0Read
* Description   : Event Handler E04
*               : DF0 Programing Reqest Handler  (DF_Read)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StHostDf_evDf0Read(void)
{
    /* DF0 = 0 -> 1 */
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_IDLE;
    return (ER_OK);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0118]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StCFAcknowledge
* Description   : Event Handler E05
*               : Acknowledge response to "CF0 P/E Mode Entry" Request
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StCFAcknowledge(void)
{
    DFLS_ICUM_FLPRG0ACK = (DFLS_ICUM_FLPRG0ACK | ICUM_FLPRG0ACK_ACSACK0);
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_HOSTCF;
    return (ER_OK);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0119]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StDFAcknowledge
* Description   : Event Handler
*               : DF0 Programing Reqest Handler (DF_P/E)
*                 (The state is the same)
*                   s03_e03, s04_e03, s05_e03, s06_e03
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StDFAcknowledge(void)
{
    /* Respons DF0 ACK to Host */
    DFLS_ICUM_FLPRG0ACK = (DFLS_ICUM_FLPRG0ACK | ICUM_FLPRG0ACK_ACSACK0);
    g_usr_df_prog_req = AG_D_FLS_DF_PE_SET;
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_HOSTDF;
    return (ER_OK);
}

/* Code executed on ROM             */
CHANGE_SEC(text, ".icum_handler.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0120]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StIdle_evIcumCf0Get
* Description   : Event Handler E06
*               : Get Resouce (FirmWareUpdate)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StIdle_evIcumCf0Get(void)
{
    /* CF0:1 -> 0 */
    /* DF0:1 -> 0 */
    /* DF2:1 -> 0 */
    d_FLS_SetBusGuardFromPE_ram();
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_ICUMCF;
    return (ER_OK);
}

/* Code executed on ROM             */
CHANGE_SEC(text, ".icum_handler.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0121]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StIcumDf_evIcumCf0Get
* Description   : Event Handler E06
*               : Get Resouce (FirmWareUpdate)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StIcumDf_evIcumCf0Get(void)
{
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_ICUMALL;
    return (ER_OK);
}

/* Code executed on ROM             */
CHANGE_SEC(text, ".icum_handler.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0122]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StIcumCf_evIcumCf0Rel
*               : Event Handler E07
*               : Free Resouce (FirmWareUpdate)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StIcumCf_evIcumCf0Rel(void)
{
    if ((uint32_t)D_FLS_ZERO_U32 == (uint32_t)g_fls_ctrl_bak[R_RFD_FACI0].context.id)
    {
        /* CF0:0 -> 1 */
        /* DF0:0 -> 1 */
        /* DF2:0 -> 1 */
        d_FLS_ClrBusGuardFromPE_ram();
        g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_IDLE;
    }
    else
    {
        /* Do nothing */
    }
    return (ER_OK);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0123]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StIcumAll_evIcumCf0Rel
*               : Event Handler E07
*               : Free Resouce (FirmWareUpdate)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StIcumAll_evIcumCf0Rel(void)
{
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_ICUMDF;
    return (ER_OK);
}

/* Code executed on ROM             */
CHANGE_SEC(text, ".icum_handler.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0124]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StIdle_evIcumDf0Get
*               : Event Handler E08
*               : Get Resouce (UserArea DF)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StIdle_evIcumDf0Get(void)
{
    d_FLS_SetBusGuardFromPE_ram();
    /* The MM_FLS side now controls the FACIACC flag */
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_ICUMDF;
    return (ER_OK);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0125]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StIcumCf_evIcumDf0Get
*               : Event Handler E08
*               : Get Resouce (UserArea DF)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StIcumCf_evIcumDf0Get(void)
{
    /* The MM_FLS side now controls the FACIACC flag */
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_ICUMALL;
    return (ER_OK);
}

/* Code executed on ROM             */
CHANGE_SEC(text, ".icum_handler.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0126]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StIcumDf_evIcumDf0Rel
*               : Event Handler E09
*               : Free Resouce (UserArea DF)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StIcumDf_evIcumDf0Rel(void)
{
    if (D_FLS_ZERO_U32 == (uint32_t)g_fls_ctrl_bak[R_RFD_FACI0].context.id)
    {
        d_FLS_ClrBusGuardFromPE_ram();
        /* The MM_FLS side now controls the FACIACC flag */
        g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_IDLE;
    }
    else
    {
        /* Do nothing */
    }
    return (ER_OK);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0127]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StIcumAll_evIcumDf0Rel
*               : Event Handler E09
*               : Free Resouce (UserArea DF)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StIcumAll_evIcumDf0Rel(void)
{
    /* The MM_FLS side now controls the FACIACC flag */
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_ICUMCF;
    return (ER_OK);
}

/* Code executed on ROM             */
CHANGE_SEC(text, ".icum_handler.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0128]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StIdle_evConfigGet
*               : Event Handler E10
*               : Get Resouce (Config)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StIdle_evConfigGet(void)
{
    d_FLS_SetBusGuardFromPE_ram();
    /* The MM_FLS side now controls the FACIACC flag */
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_CONFIG;
    return (ER_OK);
}

/* Code executed on ROM             */
CHANGE_SEC(text, ".icum_handler.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_driver.rodata")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0129]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_If0StConfig_evConfigRel
*               : Event Handler E11
*               : Free Resouce (Config)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If0StConfig_evConfigRel(void)
{
    d_FLS_ClrBusGuardFromPE_ram();
    /* The MM_FLS side now controls the FACIACC flag */
    g_fls_ctrl[R_RFD_FACI0].status = D_FLS_IF0_IDLE;
    return (ER_OK);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0130]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_StateNone
* Description   : Event Handler Any
*               : The state is the same and the return value is ER_OK
*                   s04_e01, s05_e01, s06_e01
*                   s08_e01, s09_e01, s01_e02
*                   s03_e02, s04_e02, s05_e02, s06_e02, s07_e02, s08_e02, s09_e02
*                   s07_e03, s08_e03, s09_e03
*                   s01_e04, s02_e04, s04_e04, s05_e04, s06_e04, s07_e04, s08_e04, s09_e04
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_StateNone(void)
{
    /* Do nothing */
    /* No response to HOST when requested from HOST */
    return (ER_OK);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0131]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_StateUnavailableError
* Description   : Event Handler - Unavailable Error
* Arguments     : none
* Return Value  : ER_D_FLS_UNAVAILABLE
*********************************************************************************/
static uint32_t d_FLS_StateUnavailableError(void)
{
    /* Do nothing */
    return (ER_D_FLS_UNAVAILABLE);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0132]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_StateInternalError
*               : Event Handler - Internal Error
* Arguments     : none
* Return Value  : ER_D_FLS_INTERNAL
*********************************************************************************/
static uint32_t d_FLS_StateInternalError(void)
{
    /* Impossible to Soft Design */
    return (ER_D_FLS_INTERNAL);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0133]:[ICUMFW_UD_U2B_D_FLS_0100]
* Function Name : d_FLS_StatePending
*               : Event Handler - Pending
* Arguments     : none
* Return Value  : ER_PENDING
*********************************************************************************/
static uint32_t d_FLS_StatePending(void)
{
    /* Pending */
    return (ER_PENDING);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0152]:[ICUMFW_UD_U2B_D_FLS_0102]
* Function Name : d_FLS_If2StIdle_evIcumDf2Get
*               : Event Handler E12
*               : Get Resouce (Secure Data Flash Access)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If2StIdle_evIcumDf2Get(void)
{
    /* DF2: 1 -> 0 */
    g_fls_ctrl[R_RFD_FACI2].status = D_FLS_IF2_ICUMDF;
    return (ER_OK);
}

/* Code executed on RAM             */
CHANGE_SEC(text, ".icum_handler_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_driver_const_ram")
/********************************************************************************
* Function ID   : [ICUMFW_CD_U2B_D_FLS_0153]:[ICUMFW_UD_U2B_D_FLS_0102]
* Function Name : d_FLS_If2StHostDf_evIcumDf2Rel
*               : Event Handler E13
*               : Free Resouce (Secure Data Flash Access)
* Arguments     : none
* Return Value  : ER_OK
*********************************************************************************/
static uint32_t d_FLS_If2StHostDf_evIcumDf2Rel(void)
{
    /* DF2: 0 -> 1 */
    g_fls_ctrl[R_RFD_FACI2].status = D_FLS_IF2_IDLE;
    return (ER_OK);
}

/* Code executed on RAM             */
CHANGE_SEC(text, default)
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, default)
