/******************************************************************************/
/* Component Name  System / Life Cycle Manager (System Configuration)         */
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
#include "r_icumif_api.h"
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "icum_d_info_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_wdt_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_ml_cfg_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mh_sysinfo_pub.h"
#include "icum_mh_sysinfo.h"
#include "icum_d_trng_pub.h"
#include "icum_d_reset_pub.h"
#include "icum_d_cry_aes_pub.h"
#include "icum_d_clk_pub.h"
/**************************************************************************************/
/* Definitions                                                                        */
/**************************************************************************************/
#define SYSINFO_CONFIG_NB_SLOT    (0x0001u)
#define SYSINFO_CONFIG_FIRST_IDX  (0x0000u)
#define SYSINFO_UCID_SIZE         (0x0Fu)
#define SYSINFO_NB_ERR            (0x05uL)

/**************************************************************************************/
/* Structures                                                                         */
/**************************************************************************************/
typedef struct
{
    uint8_t cism_state;
    uint8_t ram_execute;
    uint8_t init_dataflash;
    uint8_t icup_update_state;
    uint8_t host_update_state;
} fw_cfg_t;

/**************************************************************************************/
/* Global Variables                                                                   */
/**************************************************************************************/
/* Firmware Configuration */
static fw_cfg_t g_fw_cfg;
/* Firmware name */
static const uint8_t g_firmware_name[] = INFO_FIRMWARE_NAME;
/*redmine #252062*/
/* Firmware version */
static const uint8_t g_firmware_version[] = FW_VERSION;
/* Firmware build type */
static const uint8_t g_build_type[] =
#if defined(_RELEASE_MP_)
    "MP";
#elif defined(_RELEASE_EVAL_)
    "EV";
#else
    "DBG";
#endif

/**************************************************************************************/
/* Global Array                                                                       */
/**************************************************************************************/
static const uint32_t g_sysinfo_err_correspond[SYSINFO_NB_ERR][2uL] =
{
    { ER_MM_FLS_SYS,           ER_MH_SYSINFO_NVM_SYS             },
    { ER_MM_FLS_SYS_CRITICAL,  ER_MH_SYSINFO_NVM_SYS_CRITICAL    },
    { ER_MM_FLS_PROG,          ER_MH_SYSINFO_NVM_PROG            },
    { ER_MM_FLS_DATA_INVALID,  ER_MH_SYSINFO_NVM_DATA_INVALID    },
    { ER_MM_FLS_UNAVAILABLE,   ER_MH_SYSINFO_NVM_UNAVAILABLE     }
};

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/

/**************************************************************************************/
/* Functions                                                                          */
/**************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0001]:[ICUMFW_UD_MH_SYSINFO_MNG_0001]
* Function name: MH_SYSINFO_Init
* Description  : Set initial value of system configuration and internal state of firmware.
* Arguments    : N/A
* Return Value : ER_OK
*                ER_ML_CFG_RAM_SIZE_ERROR
*                ER_ML_CFG_NVM_SIZE_ERROR
*                ER_MH_SYSINFO_NVM_UNAVAILABLE
***************************************************************************************/
uint32_t MH_SYSINFO_Init(void)
{
    uint32_t err;
    uint32_t *p_all_config;
    uint16_t size;

    /* initialize firmware internal state */
    g_fw_cfg.cism_state     = SYSINFO_STATE_OFF;
    g_fw_cfg.ram_execute    = SYSINFO_STATE_OFF;
    g_fw_cfg.host_update_state = SYSINFO_STATE_OFF;
    g_fw_cfg.icup_update_state = SYSINFO_STATE_OFF;
    g_fw_cfg.init_dataflash = SYSINFO_DF_NOT_INITIALIZED;

    /* dataset for system configuration */
    (void)MM_FLS_DeclareDataSet(FLS_DS_SYS_CFG, SYSINFO_CONFIG_NB_SLOT, ML_CFG_DS_NB_BLOCKS, FLS_DS_NB_COPIES);
    /* get system configuration pointer in secure RAM */
    (void)ML_CFG_GetSysAllConfig(&p_all_config, &size);
    /* read system configuration from secure data flash */
    err = MM_FLS_Read(FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE,
                     FLS_DS_SYS_CFG, (uint8_t)0u, size, (uint8_t *)p_all_config);
    /* check return value */
    if(err != ER_OK)
    {
        /* not initialized secure data flash */
        (void)MH_SYSINFO_SetInitState(SYSINFO_DF_NOT_INITIALIZED);
    }
    else
    {
        /* already initialized secure data flash */
        (void)MH_SYSINFO_SetInitState(SYSINFO_DF_INITIALIZED);
    }

    /* set return value */
    if((err == ER_OK) || (err == ER_MM_FLS_DATA_BLANK))
    {
        /* Update VM index */
        err = ML_CFG_SetIndexMappingVM();
    }
    else
    {
        err = ER_MH_SYSINFO_NVM_UNAVAILABLE;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0009]:[ICUMFW_UD_MH_SYSINFO_MNG_0009]
* Function name: MH_SYSINFO_GetFirmwareName
* Description  : Get firmware name.
* Arguments    : (Out) p_firmware_name : Firmware name
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_SYSINFO_GetFirmwareName(uint8_t *p_firmware_name)
{
    /* get firmware name */
    L_Toolbox_MemCpy(p_firmware_name, g_firmware_name, (int16_t)sizeof(g_firmware_name));
    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0010]:[ICUMFW_UD_MH_SYSINFO_MNG_0010]
* Function name: MH_SYSINFO_GetFirmwareVersion
* Description  : Get firmware version.
* Arguments    : (Out) p_version : version
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_SYSINFO_GetFirmwareVersion(uint8_t *p_firmware_version)
{
    /* get firmware version */
    L_Toolbox_MemCpy(p_firmware_version, g_firmware_version, (int16_t)sizeof(g_firmware_version));
    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0011]:[ICUMFW_UD_MH_SYSINFO_MNG_0011]
* Function name: MH_SYSINFO_GetFirmwareBuildType
* Description  : Get firmware build type.
* Arguments    : (Out) p_build_type : Build type
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_SYSINFO_GetFirmwareBuildType(uint8_t *p_build_type)
{
    /* get firmware build type */
    L_Toolbox_MemCpy(p_build_type, g_build_type, (int16_t)sizeof(g_build_type));
    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0012]:[ICUMFW_UD_MH_SYSINFO_MNG_0012]
* Function name: MH_SYSINFO_GetChipId
* Description  : Get UCID.
* Arguments    : (Out) p_chip_id : UCID
*                (Out) p_size    : Size of UCID
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_SYSINFO_GetChipId(uint8_t *p_chip_id, uint8_t *p_size)
{
    uint8_t chip[16];

    /* clear the area of the local variable */
    L_Toolbox_MemSet(chip, 0x00u, (int16_t)sizeof(chip));
    /* get UCID */
    (void)D_INFO_GetInfo(INFO_UCID, (uint32_t *)chip);
    /* replace UCID order */
    L_Toolbox_MemCpy(p_chip_id, (const uint8_t *)&chip[0], 8);
    L_Toolbox_MemCpy((p_chip_id + 8uL), (const uint8_t *)&chip[12], 4);
    L_Toolbox_MemCpy((p_chip_id + 12uL), (const uint8_t *)&chip[8], 3);

    /* get UCID size */
    *p_size = SYSINFO_UCID_SIZE;

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0013]:[ICUMFW_UD_MH_SYSINFO_MNG_0013]
* Function name: MH_SYSINFO_GetReducedChipId
* Description  : Get part of UCID.
* Arguments    : (Out) p_reduced_chip_id : Part of UCID
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_SYSINFO_GetReducedChipId(uint8_t *p_reduced_chip_id)
{
    uint8_t chip[16];

    /* clear the area of the local variable */
    L_Toolbox_MemSet(chip, 0x00u, (int16_t)sizeof(chip));
    /* get UCID */
    (void)D_INFO_GetInfo(INFO_UCID, (uint32_t *)chip);
    /* replace UCID order */
    L_Toolbox_MemCpy(p_reduced_chip_id, (const uint8_t *)&chip[0], (int16_t)0x0004u);

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0014]:[ICUMFW_UD_MH_SYSINFO_MNG_0014]
* Function name: MH_SYSINFO_GetCustomerCodeInfo
* Description  : Get customer code information.
* Arguments    : (Out) p_info : customer code information
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MH_SYSINFO_GetCustomerCodeInfo(uint8_t info_id, uint8_t *p_info)
{
    uint32_t err = ER_OK;
    uint8_t  used_ac[NB_CUS_CODE_BYTE];

    /* clear the area of the local variable */
    L_Toolbox_MemSet(used_ac, 0x00u, (int16_t)sizeof(used_ac));

    /* check parameter info_id */
    if ((uint32_t)SYSINFO_CUSTOMER_NUMBER == (uint32_t)info_id)
    {
        /* get customer code from system configuration */
        (void)ML_CFG_GetSys(ML_CFG_SYS_USED_AC, (uint32_t *)&used_ac);
        /* get customer number information */
        *p_info = used_ac[NB_CUS_NUM_BYTE];
    }
    else
    {
        err = ER_PARAM;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0021]:[ICUMFW_UD_MH_SYSINFO_MNG_0021]
* Function name: MH_SYSINFO_SetCISMState
* Description  : Set CISM state.
* Arguments    : (In)  state : CISM state
*                                Range: SYSINFO_STATE_OFF
*                                       SYSINFO_STATE_ON
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MH_SYSINFO_SetCISMState(uint8_t state)
{
    uint32_t err = ER_OK;

    /* check range */
    if(((uint32_t)SYSINFO_STATE_OFF == (uint32_t)state) || ((uint32_t)SYSINFO_STATE_ON == (uint32_t)state))
    {
        /* set CISM state */
        g_fw_cfg.cism_state = state;
    }
    else
    {
        err = ER_PARAM;
    }

     return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0022]:[ICUMFW_UD_MH_SYSINFO_MNG_0022]
* Function name: MH_SYSINFO_GetCISMState
* Description  : Get CISM state.
* Arguments    : (Out) p_state : CISM state
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_SYSINFO_GetCISMState(uint8_t *p_state)
{
    /* get CISM state */
    *p_state = g_fw_cfg.cism_state;
    return ER_OK;
}

CHANGE_SEC(text, ".icum_middle_ram")
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0023]:[ICUMFW_UD_MH_SYSINFO_MNG_0023]
* Function name: MH_SYSINFO_SetRamExeState
* Description  : Set RAM execution state.
* Arguments    : (In)  state : RAM execution state
*                                Range: SYSINFO_STATE_OFF
*                                       SYSINFO_STATE_ON
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MH_SYSINFO_SetRamExeState(uint8_t state)
{
    uint32_t err = ER_OK;

    /* check range */
    if(((uint32_t)SYSINFO_STATE_OFF == (uint32_t)state) || ((uint32_t)SYSINFO_STATE_ON == (uint32_t)state))
    {
        /* set RAM execution state */
        g_fw_cfg.ram_execute = state;
    }
    else
    {
        err = ER_PARAM;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0024]:[ICUMFW_UD_MH_SYSINFO_MNG_0024]
* Function name: MH_SYSINFO_GetRamExeState
* Description  : Get RAM execution state.
* Arguments    : (Out) p_state : RAM execution state
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_SYSINFO_GetRamExeState(uint8_t *p_state)
{
    /* get RAM execution state */
    *p_state = g_fw_cfg.ram_execute;
    return ER_OK;
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0027]:[ICUMFW_UD_MH_SYSINFO_MNG_0027]
* Function name: MH_SYSINFO_SetInitState
* Description  : Set initialization state of secure data flash.
* Arguments    : (In)  state : Initialization state of secure data flash
*                                Range: SYSINFO_DF_NOT_INITIALIZED
*                                       SYSINFO_DF_INITIALIZED
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MH_SYSINFO_SetInitState(uint8_t state)
{
    uint32_t err = ER_OK;

    /* check range */
    if(((uint32_t)SYSINFO_DF_NOT_INITIALIZED == (uint32_t)state) || ((uint32_t)SYSINFO_DF_INITIALIZED == (uint32_t)state))
    {
        /* set initialization state of secure data flash */
        g_fw_cfg.init_dataflash = state;
    }
    else
    {
        err = ER_PARAM;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0028]:[ICUMFW_UD_MH_SYSINFO_MNG_0028]
* Function name: MH_SYSINFO_GetInitState
* Description  : Get initialization state of secure data flash.
* Arguments    : (Out) p_state : Initialization state of secure data flash
* Return Value : ER_OK
***************************************************************************************/
uint32_t MH_SYSINFO_GetInitState(uint8_t *p_state)
{
    /* get initialization state of secure data flash */
    *p_state = g_fw_cfg.init_dataflash;
    return ER_OK;
}

CHANGE_SEC(text, ".icum_middle_ram")
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0025]:[ICUMFW_UD_MH_SYSINFO_MNG_0025]
* Function name: MH_SYSINFO_SetReproModeState
* Description  : Set RAM execution state.
* Arguments    :(In)  update_target : Host update or ICUP update
*               (Out) state : Repro mode state
* Return Value : ER_OK
*                ER_MH_SYSINFO_SYS_ERROR
***************************************************************************************/
uint32_t MH_SYSINFO_SetReproModeState(uint8_t update_target, uint8_t state)
{
    /* return value */
    uint32_t err;

    /* variable initialization */
    err = ER_OK;

    /* check range */
    if (((uint32_t)SYSINFO_STATE_OFF == (uint32_t)state) ||
        ((uint32_t)SYSINFO_STATE_ON == (uint32_t)state))
    {
        /* Determine the mode to change */
        if ((uint32_t)SYSINFO_REPRO_STATE_ICUP == (uint32_t)update_target)
        {
            /* Store values in global variables that manage icup repro state */
            g_fw_cfg.icup_update_state = state;
        }
        else if ((uint32_t)SYSINFO_REPRO_STATE_HOST == (uint32_t)update_target)
        {
            /* Store values in global variables that manage host repro state */
            g_fw_cfg.host_update_state = state;
        }
        else
        {
            err = ER_MH_SYSINFO_SYS_ERROR;
        }
    }
    else
    {
        err = ER_MH_SYSINFO_SYS_ERROR;
    }

    return err;
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0026]:[ICUMFW_UD_MH_SYSINFO_MNG_0026]
* Function name: MH_SYSINFO_GetReproModeState
* Description  : Get Repro mode state.
* Arguments    :(In)  update_target : Host update or ICUP update
*               (Out) p_state : Repro mode state
* Return Value : ER_OK
*                ER_MH_SYSINFO_SYS_ERROR
***************************************************************************************/
uint32_t MH_SYSINFO_GetReproModeState(uint8_t update_target, uint8_t *p_state)
{
    /* return value */
    uint32_t err;

    /* variable initialization */
    err = ER_OK;

    /* get Repro mode state */
    if ((uint32_t)SYSINFO_REPRO_STATE_ICUP == (uint32_t)update_target)
    {
        /* store ICUP update state */
        *p_state = g_fw_cfg.icup_update_state;
    }
    else if ((uint32_t)SYSINFO_REPRO_STATE_HOST == (uint32_t)update_target)
    {
        /* store HOST update mode */
        *p_state = g_fw_cfg.host_update_state;
    }
    else
    {
        err = ER_MH_SYSINFO_SYS_ERROR;
    }

    return err;
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0008]:[ICUMFW_UD_MH_SYSINFO_MNG_0008]
* Function name: MH_SYSINFO_UpdateSystemConfig
* Description  : Update system configuration in secure data flash.
* Arguments    : (In)  job_id : Job ID
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MH_SYSINFO_NVM_SYS
*                ER_MH_SYSINFO_NVM_SYS_CRITICAL
*                ER_MH_SYSINFO_NVM_PROG
*                ER_MH_SYSINFO_NVM_DATA_INVALID
*                ER_MH_SYSINFO_NVM_UNAVAILABLE
*                ER_MH_SYSINFO_UNKNOWN
***************************************************************************************/
uint32_t MH_SYSINFO_UpdateSystemConfig(r_icumif_job_id_t job_id)
{
    uint32_t err;
    uint32_t errcode;
    uint32_t *p_all_config;
    uint16_t size;

    /* get system configuration pointer in RAM */
    (void)ML_CFG_GetSysAllConfig(&p_all_config, &size);
    /* write system configuration to secure data flash */
    err = MM_FLS_Write(job_id, FLS_SELECT_ICUM_DF,
                      FLS_SELECT_SECURE, FLS_DS_SYS_CFG, (uint8_t)0u,
                      size, (uint8_t *)p_all_config);

    /* check return value */
    if(ER_IN_PROCESS >= err)
    {
        /* do nothing */
    }
    else if(ER_MM_FLS_IN_PROGRESS == err)
    {
        /* change error code */
        err = ER_IN_PROCESS;
    }
    else
    {
        /* change error code */
        (void)mh_SYSINFO_FindErrCode(err, &errcode);
        err = errcode;
    }

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0034]:[ICUMFW_UD_MH_SYSINFO_MNG_0034]
* Function name: MH_SYSINFO_Stop
* Description  : Stop firmware.
* Arguments    : N/A
* Return Value : N/A
***************************************************************************************/
void MH_SYSINFO_Stop(void)
{
    bool wdt_flag;

    /* disable EI level maskable interrupt */
    DISABLE_INTERRUPT();

    /* until reset */
    while(1)
    {
        D_WDT_GetIntRequest(&wdt_flag);

        if((uint32_t)true == (uint32_t)wdt_flag)
        {
            /* restart watchdog timer */
            D_WDT_Start();
        }
        else
        {
            /* Do nothing */
        }

        /* Holt instruction */
        SET_HALT();
    }
}

CHANGE_SEC(text, ".icum_middle_ram")
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0037]:[ICUMFW_UD_MH_SYSINFO_MNG_0037]
* Function name: mh_SYSINFO_LoopInfinity
* Description  : Maintains the halt state of the processor.
* Arguments    : N/A
* Return Value : N/A
***************************************************************************************/
void mh_SYSINFO_LoopInfinity(void)
{
    /* until reset */
    while(1)
    {
        /* Holt instruction */
        SET_HALT();
    }
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0038]:[ICUMFW_UD_MH_SYSINFO_MNG_0038]
* Function name: mh_SYSINFO_FindErrCode
* Description  : Converts the error of the lower layer to the error of the system / life cycle manager.
* Arguments    : (In)  in_err    : Error before conversion
*                                Range: ER_MM_FLS_SYS
*                                       ER_MM_FLS_SYS_CRITICAL
*                                       ER_MM_FLS_PROG
*                                       ER_MM_FLS_DATA_INVALID
*                                       ER_MM_FLS_UNAVAILABLE
*                (Out) p_out_err : Error after conversion
* Return Value : ER_OK
***************************************************************************************/
uint32_t mh_SYSINFO_FindErrCode(uint32_t in_err, uint32_t *p_out_err)
{
    uint32_t index;

    /* not supported error by system / life cycle manager */
    *p_out_err = ER_MH_SYSINFO_UNKNOWN;

    /* error search */
    for(index = 0uL; index < SYSINFO_NB_ERR; index++)
    {
         /* found error code */
         if(g_sysinfo_err_correspond[index][0uL] == in_err)
         {
              /* change error code */
              *p_out_err = g_sysinfo_err_correspond[index][1uL];
              break;
         }
         else
         {
             /* Do nothing */
         }
    }

    return ER_OK;
}

CHANGE_SEC(text, ".icum_middle_ram")
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MH_SYSINFO_MNG_0039]:[ICUMFW_UD_MH_SYSINFO_MNG_0039]
* Function name: mh_SYSINFO_TransitionStopMode
* Description  : Transition to StopMode.
* Arguments    : N/A
* Return Value : ER_MH_SYSINFO_STOP_OK
*                ER_MH_SYSINFO_STOP_NG
***************************************************************************************/
uint32_t mh_SYSINFO_TransitionStopMode(void)
{
    uint32_t err;
    icum_clma_cb_regist_t regist_info_clk;

    /* Not change callback function */
    regist_info_clk.p_cbfunc = NULL;

    /* ICUM_TRNGC0 disable */
    (void)D_TRNG_DisableModule();

#if 0
    /* Stop CLMA */
    (void)D_CLK_StopCLMA();
#endif
    /* Transition to StopMode/DeepStopMode */
    err = D_RESET_TransitionStopMode();

    /* Success of transition to StopMode/DeepStopMode */
    if (err == ER_D_RESET_STOP_OK)
    {
        /* Restart CLMA */
        (void)D_CLK_StartCLMA(&regist_info_clk);

        err = SYSINFO_STOP_OK;
    }
    else
    {
        err = SYSINFO_STOP_NG;
    }
    

    return err;
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

