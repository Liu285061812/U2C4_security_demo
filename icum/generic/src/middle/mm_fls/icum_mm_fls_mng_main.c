/******************************************************************************/
/* Component Name  Flash Manager                                              */
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
/*******************************************************************************
* Header files
********************************************************************************/

#include "renesas_types.h"
#include "icum_common_pub.h"
#include "r_icumif_api.h"
#include "icum_d_misc_pub.h"
#include "icum_ml_dvc_pub.h"
#include "icum_mh_key_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mm_flash.h"
#include "icum_l_toolbox_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include "icum_ml_cfg_pub.h"

/********************************************************************************
 Memory allocation section specification
*********************************************************************************/
/* self copy icum_middle.text to icum_middle_ram   */
CHANGE_SEC(text, ".icum_middle_ram")
/* self copy icum_middle.rodata to icum_middle.rodata */
CHANGE_SEC(rodata, ".icum_middle.rodata")
/* self copy icum_middle_romdata to icum_middle.data */
CHANGE_SEC(data, ".icum_middle.data")

/*******************************************************************************
* Local Prototypes
********************************************************************************/
static uint32_t mm_FLS_GetSlotData(const uint16_t target_side, const flash_location_t *const p_index, uint8_t *p_data,
                                   data_status_t *const p_data_status);
static uint32_t mm_FLS_CheckDSizeTag(const uint16_t target_side, const flash_location_t *const p_index,
                                     const uint16_t check_block_num);
static uint32_t mm_FLS_VerifySlotData(fls_data_area_pointer_t data_buffer_pnt, data_status_t *const p_data_status);
static uint32_t mm_FLS_GetProhibitBlk(uint32_t *const p_prohibited_block);
static uint32_t mm_FLS_UpdateWrite(r_icumif_job_id_t job_id, const uint8_t ds_type, const uint8_t slot_num,
                                   const uint16_t size, const fls_data_area_pointer_t data_buffer_pnt,
                                   const flash_location_t *const p_wk_index, uint32_t *const p_update_counter);
static void mm_FLS_EndProcessWFCfg(uint32_t *const p_err);
static uint32_t mm_FLS_CheckParam(const uint8_t target_flash, const uint8_t target_area, const uint8_t ds_type,
                                  const uint16_t size, const uint8_t operation);
static uint32_t mm_FLS_PreCheck(const uint8_t target_flash, const uint8_t target_area, const uint8_t ds_type,
                                const uint16_t size, const uint8_t operation);
static void mm_FLS_InitProcessVariable(void);
static uint32_t mm_FLS_DSRead(const flash_location_t *const p_index, uint8_t *p_data);
static void mm_FLS_ChangeAddr(const uint16_t target_side, const flash_location_t flash_location,
                              uint32_t *const p_addr);
static uint32_t mm_FLS_ExecReadDF(uint32_t df_type, uint32_t offset, uint32_t size, uint8_t *p_data);
static uint32_t mm_FLS_ExecWriteDF(r_icumif_job_id_t job_id, uint32_t size, uint32_t offset, const uint8_t *p_data);

static uint32_t mm_FLS_CheckBlankCheckArea(uint32_t offset, uint32_t size, uint32_t *p_result_status);

static uint32_t mm_FLS_CheckGCFlag(gc_state_t *const p_gc_state, uint32_t *const p_gc_source_block,
                                   uint32_t *const p_gc_reserve_block);
static uint32_t mm_FLS_CountGCFlag(fls_block_info_t block_info[], uint32_t *const p_counter_ssec,
                                   uint32_t *const p_counter_sses, uint32_t *const p_counter_scec,
                                   uint32_t *const p_counter_transfer, uint32_t *const p_num_ssec,
                                   uint32_t *const p_num_scec, uint32_t *const p_num_transfer);
static uint32_t mm_FLS_GetAfterFBlock(const uint32_t block_num);
static uint32_t mm_FLS_GetBeforeFBlock(const uint32_t block_num);
static uint32_t mm_FLS_DSWrite(r_icumif_job_id_t job_id, const uint16_t size, const flash_location_t *const p_index,
                               uint8_t *const p_data);
static void mm_FLS_ChangeTagAddr(const uint16_t target_side, const flash_location_t flash_location,
                                 uint32_t *const p_addr);
static void mm_FLS_ChangeFlagAddr(const uint32_t block_num, const df_flag_t flag_type, uint32_t *const p_addr);
static uint32_t mm_FLS_ExecEraseDF(const r_icumif_job_id_t job_id, const uint32_t block_num);
static flash_block_idx_t mm_FLS_GetFirstLBlock(const uint32_t block_num);
static flash_block_idx_t mm_FLS_GetLastLBlock(const uint32_t block_num);
static uint32_t mm_FLS_CheckMapData(const uint8_t ds_type, const uint8_t slot_num, uint16_t *const p_match_num);
static void mm_FLS_CheckAllFlash(const uint32_t nocheck_block, uint32_t *const p_err);
static uint32_t mm_FLS_Set1MapData(flash_location_t *const p_flash_location);
static uint32_t mm_FLS_CheckHeader(const flash_location_t *const p_flash_location, uint8_t *const p_next_process,
                                   uint16_t *const p_check_block_num);
static uint32_t mm_FLS_CheckTag(const uint16_t target_side, const flash_location_t *const p_index,
                                uint8_t *const p_result_status);
static uint32_t mm_FLS_WriteMapData(const uint8_t ds_type, const uint8_t slot_num, const flash_block_idx_t block_idx,
                                    const uint32_t update_counter);
static uint32_t mm_FLS_ReadMapData(const uint8_t ds_type, const uint8_t slot_num,
                                   flash_location_t *const p_flash_location, uint32_t *const p_update_counter);
static uint32_t mm_FLS_ReadFblockMapData(const uint32_t fblock, flash_block_idx_t *const p_block_idx,
                                         uint8_t *const p_ds_type, uint8_t *const p_slot_num,
                                         uint32_t *const p_update_counter);
static uint8_t mm_FLS_CompareValue(const uint32_t value1, const uint32_t value2, const uint32_t max_value);
static uint32_t mm_FLS_IncValue(const uint32_t value, const uint32_t max_value);
static gc_warning_t mm_FLS_GetCapacityStatus(void);
static uint32_t mm_FLS_GetFBlock(const flash_block_idx_t block_idx);
static uint32_t mm_FLS_GetGCState(const r_icumif_job_id_t job_id, uint32_t *const p_source_block,
                                  uint32_t *const p_reserve_block);
static uint32_t mm_FLS_SetTransferFlag(r_icumif_job_id_t job_id, const uint32_t source_block,
                                       const uint32_t reserve_block, const uint8_t *p_flag_buffer);
static uint32_t mm_FLS_EraseGCDest(const r_icumif_job_id_t job_id, const uint32_t reserve_block);
static uint32_t mm_FLS_WriteGCDest(r_icumif_job_id_t job_id, const uint32_t reserve_block);
static uint32_t mm_FLS_EraseGCSrc(const r_icumif_job_id_t job_id, const uint32_t source_block);
static uint32_t mm_FLS_CollectGarbageFull(r_icumif_job_id_t job_id);
static uint32_t mm_FLS_GetLastMapData(const flash_block_idx_t limit_index, const uint32_t reserve_block,
                                      uint32_t *const p_last_num);
static uint32_t mm_FLS_EraseFormatFBlkSt(const r_icumif_job_id_t job_id);
static uint32_t mm_FLS_EraseFormatFBlkComp(const r_icumif_job_id_t job_id);
static uint32_t mm_FLS_EraseFormatGBlk(const r_icumif_job_id_t job_id);
static uint32_t mm_FLS_ForwardCopy(r_icumif_job_id_t job_id);
static uint32_t mm_FLS_GetFCReserveBlk(const r_icumif_job_id_t job_id, uint32_t *const p_reserve_block,
                                       flash_block_idx_t *const p_checked_block_idx);
static uint32_t mm_FLS_WriteFCDestBlk(r_icumif_job_id_t job_id, const flash_block_idx_t destination_block_idx,
                                      const fls_data_area_pointer_t data_buffer_pnt);
static uint32_t mm_FLS_CalculateDVC(dvc_calculate_t *p_dvc_param, uint8_t *const p_phase);
static uint32_t mm_FLS_AESDSRead(const flash_location_t *const p_index, uint8_t *p_data);
static void mm_FLS_EndPhaseInternal(uint32_t *const p_err, uint8_t *const p_phase);
static void mm_FLS_EndPhaseGCWarning(uint32_t *const p_err, const uint8_t *const p_phase);
static void mm_FLS_EndPhaseWrite(uint32_t *const p_err, const uint8_t *const p_phase, const uint32_t fblock,
                                 const uint8_t flash_full);
static void mm_FLS_EndPhaseFC(uint32_t *const p_err, const uint8_t *const p_phase);
static uint32_t mm_FLS_GetWritableArea(const uint32_t prohibited_block, const uint16_t write_size,
                                       flash_block_idx_t *const p_result_index);
static uint32_t mm_FLS_CompareBlock(const flash_block_idx_t block1, const flash_block_idx_t block2,
                                    const flash_block_idx_t first_block);
static uint32_t mm_FLS_GetFlashResource(const r_icumif_job_id_t job_id);
static void mm_FLS_FreeFlashResource(void);
static uint32_t mm_FLS_CheckFlashAddr(const uint8_t target_flash, const uint8_t target_area, uint32_t addr,
                                      uint32_t size, uint32_t *p_bank, uint32_t *p_offset);
static uint32_t mm_FLS_CheckWriteCfgParam(const r_icumif_job_id_t job_id, const uint8_t target_func,
                                          const uint32_t set_data, uint32_t *const p_target_info,
                                          uint32_t *const p_value);
static uint32_t mm_FLS_CheckParamUser(const r_icumif_job_id_t job_id, const uint8_t target_flash,
                                      const uint8_t target_area, uint32_t addr, uint32_t size, uint32_t *p_bank,
                                      uint32_t *p_offset);
static uint32_t mm_FLS_GetPartitionInfoBaseOffset(const uint8_t target);
static uint32_t mm_FLS_GetPartitionInfoOffset(const uint8_t target, uint32_t *const p_offset);
static uint32_t mm_FLS_WritePartitionInfo(const r_icumif_job_id_t job_id, uint32_t offset,
                                          const uint32_t *const p_data);
static uint32_t mm_FLS_ReadPartitionInfo(const uint32_t offset, uint32_t *const p_info, uint32_t *const p_data);

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
#define PRIORITY_HIGHEST      (0x00U)

/*******************************************************************************
* Local variables
********************************************************************************/
static uint8_t              g_phase_Write       = FLS_PHASE_0;
static uint8_t              g_phase_ExWrite     = FLS_PHASE_0;

static data_status_t        g_data_status_b;

/* 0: before get resource  1:after get resource */
static uint8_t              g_get_resource_flg   = FLS_VAL_0;

ALIGNVAR(4)
static uint8_t              g_read_buffer[FLS_SLOT_MAX];

static uint8_t              g_warning_update_prohibit = FLS_ZERO_U8;

static uint8_t              g_block_status      = FLS_NOT_ERASED;
static flash_block_idx_t    g_write_block_idx[FLS_NB_BLOCK];

#if defined(FSW_USE_CRAM)
#pragma ghs section bss = ".start.icum_cram.bss"
#endif
static fls_data_map_t       g_data_map[FLS_DATA_MAP_SIZE];
#if defined(FSW_USE_CRAM)
#pragma ghs section bss = default
#endif

static uint8_t              g_phase_GC          = FLS_PHASE_0;
static uint8_t              g_phase_ExErase     = FLS_PHASE_0;
static uint8_t              g_phase_DsWrite     = FLS_PHASE_0;
static uint8_t              g_phase_Format      = FLS_PHASE_0;
static uint8_t              g_phase_GCAll       = FLS_PHASE_0;
static uint8_t              g_phase_fcopy       = FLS_PHASE_0;
static uint32_t             g_format_block      = FLS_ZERO_U32;

static sys_cfg_fls_t        g_fls_cfg  =
{
    FLS_DEFAULT_WARNING1_LEVEL, FLS_DEFAULT_WARNING2_LEVEL
};

/*******************************************************************************
* Functions
********************************************************************************/

/* self copy icum_middle_rom.text icum_middle_ram   */
CHANGE_SEC(text, ".icum_middle_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/***************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0016]:[ICUMFW_UD_MM_FlashMNG_0016]
* Function name : mm_FLS_ConvertionError (PRIVATE)
* Description   : Converting a error from flash library and DVC manager
* Arguments     : uint32_t err : flash library and DVC manager error
* Return Value  : ER_OK
*                 ER_MM_FLS_DATA_INVALID
*                 ER_MM_FLS_SYS
*                 ER_MM_FLS_SYS_CRITICAL
*                 ER_MM_FLS_PROG
*                 ER_MM_FLS_UNAVAILABLE
*                 ER_MM_FLS_AES_PRM_ERROR
*                 ER_MM_FLS_AES_TIME_OUT
***************************************************************************************/
uint32_t mm_FLS_ConvertionError(const uint32_t err)
{
    uint32_t ret_val;

    switch (err)
    {
        /* to hide inside */
        case ER_L_FLS_ECC1EDF1:
            ret_val = ER_OK;
            break;

        case ER_L_FLS_ECC2EDF1:
            ret_val = ER_MM_FLS_DATA_INVALID;
            break;

        /* these 5 cases are intentionally combined */
        case ER_L_FLS_FLOW:
        case ER_L_FLS_REJECTED:
        case ER_L_FLS_INVALID_DATA:
        case ER_L_FLS_PROTECTION:
            ret_val = ER_MM_FLS_SYS;
            break;

        case ER_L_FLS_INTERNAL:
        case ER_L_FLS_CANCEL:
        case ER_L_FLS_FACI_TIME_OUT:
            ret_val = ER_MM_FLS_SYS_CRITICAL;
            break;

        case ER_L_FLS_PROGRAM:
            ret_val = ER_MM_FLS_PROG;
            break;

        case ER_L_FLS_UNAVAILABLE:
            ret_val = ER_MM_FLS_UNAVAILABLE;
            break;

        case ER_ML_DVC_ALIGN:
            ret_val = ER_MM_FLS_AES_PRM_ERROR;
            break;

        case ER_ML_DVC_AES_TIMEOUT:
            ret_val = ER_MM_FLS_AES_TIME_OUT;
            break;

        case ER_L_FLS_REMAP:
            ret_val = ER_MM_FLS_MEM_SOURCE;
            break;

        default:
            /* not change error code */
            /* ER_OK, ER_PARAM, ER_PENDING, ER_IN_PROCESS, ER_MM_FLS_UNAVAILABLE, ER_MM_FLS_SYS_CRITICAL */
            ret_val = err;
            break;
    } /* end of switch (err) */

    return (ret_val);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_middle.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_middle.rodata")
/***************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0017]:[ICUMFW_UD_MM_FlashMNG_0017]
* Function name : mm_FLS_ChangeAddr
* Description   : Converting target side and block index into a data area address
* Arguments     : (in) uint16_t target_side             : target side
*               : (in) flash_location_t flash_location  : data slot index
*               : (out) uint32_t *p_addr                : data area address
* Return Value  : none
***************************************************************************************/
static void mm_FLS_ChangeAddr(const uint16_t target_side, const flash_location_t flash_location, uint32_t *const p_addr)
{
    if ((uint32_t)FLS_SIDE_A == (uint32_t)target_side)
    {
        /* physical block top + (Block info) + (Data TAG A&B) + block index fraction */
        *p_addr = ((((uint32_t)flash_location.block_idx) / FLS_LBLOCK_NUM_PER_PBLOCK) * FLS_ERASE_BLOCK_SIZE)
                  + FLS_TOTAL_BLOCK_INFO_SIZE + (FLS_TAG_SIZE * FLS_LBLOCK_NUM_PER_PBLOCK)
                  + (FLS_TAG_SIZE * FLS_LBLOCK_NUM_PER_PBLOCK)
                  + (((uint32_t)flash_location.block_idx % FLS_LBLOCK_NUM_PER_PBLOCK) * FLS_DF_BLOCK_SIZE)
                  + ((uint32_t)flash_location.word_idx * WORD_SIZE);
    }
    else
    {
        /* physical block top + (Block info) + (Data TAG A&B) + (Data Area A) + block index fraction */
        *p_addr = ((((uint32_t)flash_location.block_idx) / FLS_LBLOCK_NUM_PER_PBLOCK) * FLS_ERASE_BLOCK_SIZE)
                  + FLS_TOTAL_BLOCK_INFO_SIZE + (FLS_TAG_SIZE * FLS_LBLOCK_NUM_PER_PBLOCK)
                  + (FLS_TAG_SIZE * FLS_LBLOCK_NUM_PER_PBLOCK) + (FLS_DF_BLOCK_SIZE * FLS_LBLOCK_NUM_PER_PBLOCK)
                  + (((uint32_t)flash_location.block_idx % FLS_LBLOCK_NUM_PER_PBLOCK) * FLS_DF_BLOCK_SIZE)
                  + ((uint32_t)flash_location.word_idx * WORD_SIZE);
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0018]:[ICUMFW_UD_MM_FlashMNG_0018]
* Function name: mm_FLS_PreCheck
* Description  : Prepare check of flash operation(except read operation)
* Arguments    :  (in) uint8_t  target_flash : target flash memory
*                 (in) uint8_t  target_area  : type of target flash memory
*                 (in) uint8_t  ds_type      : type of data-set
*                 (in) uint16_t size         : sizeof target data-set
*                 (in) uint8_t  operation    : operation of flash memory
* Return Value :  ER_OK
*                 ER_PARAM
*                 ER_MM_FLS_SIZE
***************************************************************************************/
static uint32_t mm_FLS_PreCheck(const uint8_t target_flash, const uint8_t target_area, const uint8_t ds_type,
                                const uint16_t size, const uint8_t operation)
{
    uint32_t err;

    err = ER_OK;
    /* check parameter */
    switch ((uint32_t)operation)
    {
        case FLS_OP_READ:
            /* no error : do nothing */
            break;
        case FLS_OP_WRITE:
            /* no error : do nothing */
            break;
        case FLS_OP_ERASE:
            /* no error : do nothing */
            break;
        case FLS_OP_CFG_WRITE:
            /* no error : do nothing */
            break;
        default:
            err = ER_PARAM;
            break;
    } /* end of switch ((uint32_t)operation) */

    if (ER_OK == err)
    {
        /* --- check of parameter --- */
        err = mm_FLS_CheckParam(target_flash, target_area, ds_type, size, operation);
    }
    else
    {
        /* do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0019]:[ICUMFW_UD_MM_FlashMNG_0019]
* Function name: mm_FLS_CheckParam
* Description  :  Check parameter(address check is done by Lib)
* Arguments    :  (in) uint8_t  target_flash : target flash memory
*                 (in) uint8_t  target_area  : type of target flash memory
*                 (in) uint8_t  ds_type      : type of data-set
*                 (in) uint16_t size         : sizeof target data-set
*                 (in) uint8_t  operation    : operation of flash memory
* Return Value :  ER_OK
*                 ER_PARAM
*                 ER_MM_FLS_SIZE
***************************************************************************************/
static uint32_t mm_FLS_CheckParam(const uint8_t target_flash, const uint8_t target_area, const uint8_t ds_type,
                                  const uint16_t size, const uint8_t operation)
{
    uint32_t err;

    err = ER_OK;
    /* check range of target_flash value */
    if (((uint32_t)FLS_SELECT_DF != (uint32_t)target_flash)
            && ((uint32_t)FLS_SELECT_ICUM_DF != (uint32_t)target_flash))
    {
        err = ER_PARAM;
    }
    else if (((uint32_t)FLS_SELECT_SECURE != (uint32_t)target_area)
            && ((uint32_t)FLS_OP_CFG_WRITE == (uint32_t)operation))
    {
        err = ER_PARAM;
    }
    else if ((uint32_t)FLS_DS_NB_DATA_SET <= (uint32_t)ds_type)
    {
        err = ER_PARAM;
    }
    /* check parameter of size */
    else if (((uint32_t)FLS_ZERO_U16 == (uint32_t)size) || ((uint32_t)FLS_SLOT_MAX < (uint32_t)size))
    {
        err = ER_MM_FLS_SIZE;
    }
    else
    {
        /* DF(Read, Write, Erase or cfg_write) */
        /* in case User DF or Secure _DF */

        /* in case secure DF(erase) */
        if (((uint32_t)FLS_OP_ERASE == (uint32_t)operation))
        {
            if ((uint32_t)FLS_ZERO_U16 != ((uint32_t)size % FLS_DF_BLOCK_SIZE))
            {
                err = ER_PARAM;
            }
            else
            {
                /* no errors */
            }
        }
        /* in case secure DF(Read or Write) */
        else if (((uint32_t)FLS_OP_READ == (uint32_t)operation) || ((uint32_t)FLS_OP_WRITE == (uint32_t)operation))
        {
            if ((uint32_t)FLS_ZERO_U16 != ((uint32_t)size % WORD_SIZE))
            {
                err = ER_PARAM;
            }
            else
            {
                /* no errors */
            }
        }
        /* in case (Write CFG) */
        else
        {
            /* check option byte  target_func */
            if ((uint32_t)FLS_PROG_CR_RES < (uint32_t)target_area)
            {
                err = ER_PARAM;
            }
            else
            {
                /* no errors */
            }
        }
    } /* end of else */

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0020]:[ICUMFW_UD_MM_FlashMNG_0020]
* Function name: mm_FLS_InitProcessVariable
* Description  : Initialize variable of write process
*                 g_phase_Write,g_phase_ExWrite,g_phase_GC,g_phase_DsWrite,
*                 g_phase_ExErase,g_phase_Format,g_phase_fcopy,g_format_block
* Arguments    : none
* Return Value : none
***************************************************************************************/
static void mm_FLS_InitProcessVariable(void)
{
    g_phase_Write       = FLS_PHASE_0;
    g_phase_ExWrite     = FLS_PHASE_0;
    g_phase_GC          = FLS_PHASE_0;
    g_phase_DsWrite     = FLS_PHASE_0;
    g_phase_ExErase     = FLS_PHASE_0;
    g_phase_Format      = FLS_PHASE_0;
    g_phase_fcopy       = FLS_PHASE_0;
    g_format_block      = FLS_ZERO_U32;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0015]:[ICUMFW_UD_MM_FlashMNG_0015]
* Function name: mm_FLS_DSRead
* Description  : Read data of specified flash area.
* Arguments    : (in) flash_location_t *p_index : specified block index of data set
*                (out) uint8_t *p_data          : an address pointer that stores the read data
* Return Value : ER_OK
*                ER_MM_FLS_SYS_CRITICAL
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_DATA_INVALID
*                ER_MM_FLS_UNAVAILABLE
*                ER_MM_FLS_MEM_SOURCE
*                ER_MM_FLS_AES_TIME_OUT
*                ER_MM_FLS_DATA_BLANK
*                ER_MM_FLS_AES_PRM_ERROR
***************************************************************************************/
static uint32_t mm_FLS_DSRead(const flash_location_t *const p_index, uint8_t *p_data)
{
    uint32_t err;
    uint32_t ret_val;
    uint8_t set_data_warn_flag;
    uint8_t ds_type;
    uint8_t slot_num;
ALIGNVAR(4)
    uint8_t read_buffer[FLS_SLOT_MAX];
    data_status_t data_status_a;

    ret_val = ER_OK;
    set_data_warn_flag = FLS_ZERO_U8;

    /* reading and verification for side-A data */
    /* retrieve the side-A data */
    err = mm_FLS_GetSlotData(FLS_SIDE_A, p_index, p_data, &data_status_a);
    if (ER_MM_FLS_SYS_CRITICAL == err)
    {
        /* Hardware failure */
        ret_val = err;
    }
    else
    {
        /* reading and verification for side-B data */

        /* retrieve the side-B data */
        err = mm_FLS_GetSlotData(FLS_SIDE_B, p_index, read_buffer, &g_data_status_b);
        if (ER_MM_FLS_SYS_CRITICAL == err)
        {
            /* Hardware failure */
            ret_val = err;
        }
        else
        {
            /* verification for two-side data */

            /* determine of adopted side */

            if ((ER_MM_FLS_DATA_BLANK == data_status_a.flash_err)
                && (ER_MM_FLS_DATA_BLANK == g_data_status_b.flash_err))
            {
                ret_val = ER_MM_FLS_DATA_BLANK;
            }
            else if (((uint32_t)FLS_CHK_OK != (uint32_t)data_status_a.dvc_result)
                     && ((uint32_t)FLS_CHK_OK != (uint32_t)g_data_status_b.dvc_result))
            {
                ret_val = ER_MM_FLS_DATA_INVALID;
            }
            else if ((ER_OK != data_status_a.flash_err) && (ER_OK != g_data_status_b.flash_err))
            {
                ret_val = g_data_status_b.flash_err;
            }
            else if ((ER_MM_FLS_UNAVAILABLE == data_status_a.flash_err)
                     || (ER_MM_FLS_UNAVAILABLE == g_data_status_b.flash_err))
            {
                ret_val = ER_MM_FLS_UNAVAILABLE;
            }
            else if ((uint32_t)FLS_CHK_OK != (uint32_t)data_status_a.dvc_result)
            {
                /* Side-B data adopted */
                L_Toolbox_MemCpy(p_data, (const uint8_t *)read_buffer, (int16_t)g_data_status_b.data_size);

                /* sets data flash information */
                set_data_warn_flag = FLS_FLAG_ON_U8;
                ds_type = g_data_status_b.ds_type;
                slot_num = g_data_status_b.slot_num;
            }
            else if ((uint32_t)FLS_CHK_OK != (uint32_t)g_data_status_b.dvc_result)
            {
                /* Side-A data adopted */
                /* already stored as read data */

                /* sets data flash information */
                set_data_warn_flag = FLS_FLAG_ON_U8;
                ds_type = data_status_a.ds_type;
                slot_num = data_status_a.slot_num;
            }
            else
            {
                /* could been read data of two-side */

                if (((uint32_t)data_status_a.ds_type == (uint32_t)g_data_status_b.ds_type)
                    && ((uint32_t)data_status_a.slot_num == (uint32_t)g_data_status_b.slot_num)
                    && ((uint32_t)data_status_a.data_size == (uint32_t)g_data_status_b.data_size)
                    && (data_status_a.counter == g_data_status_b.counter))
                {
                    /* Side-A data adopted */
                    /* already stored as read data */

                    /* two sides data normal => not warning */
                }
                else
                {
                    ret_val = ER_MM_FLS_DATA_INVALID;
                }
            }
            /* NVM Data Warning Occurred & not MM_FLS_Write Call */
            if (((uint32_t)set_data_warn_flag == (uint32_t)FLS_FLAG_ON_U8)
                && ((uint32_t)g_warning_update_prohibit == (uint32_t)FLS_ZERO_U8))
            {
                /* Set NVM Data Warning Information */
                ret_val = mm_FLS_SetNVMWarnData(ds_type, slot_num);
            }
            else
            {
                /* Do nothing */
            }
        } /* end of else */
    } /* end of else */

    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0021]:[ICUMFW_UD_MM_FlashMNG_0021]
* Function name: mm_FLS_ExecReadDF
* Description  : Read data of specified flash area. (Direct addressing)
* Arguments    : (in) uint8_t df_type   : bank of data flash to be read
*                (in) uint32_t offset   : byte offset position of the area to be read
*                (in) uint32_t size     : size of data to read
*                (out) uint8_t *p_data  : an address pointer that stores the read data
* Return Value : ER_OK
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_DATA_INVALID (ER_L_FLS_ECC2EDF1)
*                ER_MM_FLS_MEM_SOURCE
***************************************************************************************/
static uint32_t mm_FLS_ExecReadDF(uint32_t df_type, uint32_t offset, uint32_t size, uint8_t *p_data)
{
    uint32_t err;
    uint32_t ret_val;

    ret_val = ER_OK;
    err = L_FLS_ReadFlash_ram(AG_L_FLS_DATA, df_type, offset, size, p_data);
    if (ER_OK != err)
    {
        ret_val = mm_FLS_ConvertionError(err);
        if (ER_PARAM == ret_val)
        {
            ret_val = ER_MM_FLS_MEM_SOURCE;
        }
        else
        {
            /* not change error code */
            /* Do nothing */
        }
    }
    else
    {
        /* reading data successful */
        /* Do nothing */
    }

    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0022]:[ICUMFW_UD_MM_FlashMNG_0022]
* Function name: mm_FLS_ExecWriteDF
* Description  : Perform the slot data write for ICU-M DF (Direct addressing)
* Arguments    : (in) r_icumif_job_id_t job_id  : job id
*                (in) uint32_t size     : size of data to write
*                (in) uint32_t offset   : byte offset position of the area to be write
*                (in) uint8_t *p_data   : address of memory in which data to be written is stored
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
***************************************************************************************/
static uint32_t mm_FLS_ExecWriteDF(r_icumif_job_id_t job_id, uint32_t size, uint32_t offset, const uint8_t *p_data)
{
    uint32_t err;

    err = ER_OK;
    switch ((uint32_t)g_phase_ExWrite)
    {
        /* write operation acceptance phase */
        case FLS_PHASE_0:
            err = L_FLS_REQ_WriteFlash_ram(AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM, offset, size, (const uint8_t *)p_data);
            if (ER_IN_PROCESS == err)
            {
                /* process is progress */
                g_phase_ExWrite = FLS_PHASE_1;
            }
            else
            {
                /* error occurred */
                g_phase_ExWrite = FLS_PHASE_9;
            }
            break;

        /* wait for write operation completion phase */
        case FLS_PHASE_1:
            err = L_FLS_IsFinished_ram((uint16_t)job_id, AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
            if (ER_IN_PROCESS == err)
            {
                /* process is progress */
                /* do nothing          */
            }
            else
            {
                /* operation completion or error occurred */
                g_phase_ExWrite = FLS_PHASE_9;
            }
            break;

        default:
            /* Not occurred */
            break;
    } /* end of switch ((uint32_t)g_phase_ExWrite) */

    /* end process */
    mm_FLS_EndPhaseInternal(&err, &g_phase_ExWrite);
    return (err);
}

/**********************************************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0023]:[ICUMFW_UD_MM_FlashMNG_0023]
* Function name: mm_FLS_GetSlotData
* Description  : Read slot data and verification for ICU-M DF only
* Arguments    : (in) uint16_t target_side          : target side
*                (in) flash_location_t *p_index     : specified block index of data set
*                (out) uint8_t *p_data              : A pointer to the memory that stores the obtained slot data
*                (out) data_status_t *p_data_status : A pointer to the memory that stores the obtained slot data status
* Return Value : ER_OK
*                ER_MM_FLS_DATA_BLANK
*                ER_MM_FLS_AES_TIME_OUT
*                ER_MM_FLS_AES_PRM_ERROR
*                ER_MM_FLS_MEM_SOURCE
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_DATA_INVALID (ER_L_FLS_ECC2EDF1)
***********************************************************************************************************************/
static uint32_t mm_FLS_GetSlotData(const uint16_t target_side, const flash_location_t *const p_index, uint8_t *p_data,
                                   data_status_t *const p_data_status)
{
    uint32_t err;
    uint32_t ret_val;
    uint32_t offset;
    uint16_t check_block_num;
    flash_location_t wk_block_index;
    fls_data_area_pointer_t data_buffer_pnt;
    uint8_t tag_result;

    /* return status initial value */
    p_data_status->dvc_result = FLS_CHK_OK;
    p_data_status->counter    = FLS_ZERO_U32;
    p_data_status->ds_type    = FLS_ZERO_U8;
    p_data_status->slot_num   = FLS_ZERO_U8;
    p_data_status->data_size  = FLS_ZERO_U16;

    wk_block_index = *p_index;
    data_buffer_pnt.p_u8 = p_data;

    /* Check data area header */
    /* Check Data TAG (first logical block only) */
    err = mm_FLS_CheckTag(target_side, &wk_block_index, &tag_result);
    if ((uint32_t)FLS_TAG_VALID == (uint32_t)tag_result)
    {
        /* the TAG area is non-blank and the value is valid */
        /* Data area read (header only) */
        mm_FLS_ChangeAddr(target_side, wk_block_index, &offset);
        err = mm_FLS_ExecReadDF(AG_L_FLS_BANK_ICUM, offset, sizeof(fls_data_area_header_t), data_buffer_pnt.p_u8);
        if (ER_OK != err)
        {
            /* error occurred */
            ret_val = err;
        }
        else
        {
            /* Check data size is valid */
            err = mm_FLS_PreCheck(FLS_SELECT_ICUM_DF,
                                  FLS_SELECT_SECURE,
                                  data_buffer_pnt.p_header->ds_type,
                                  data_buffer_pnt.p_header->data_size,
                                  FLS_OP_READ);
            if (ER_OK == err)
            {
                /* Is the data size the data head and end are placed in the same physical block? */
                check_block_num = (uint16_t)(
                    ((((uint32_t)data_buffer_pnt.p_header->data_size) + FLS_DF_BLOCK_SIZE) - 1uL) / FLS_DF_BLOCK_SIZE);
                if (((uint32_t)FLS_ZERO_U16 != (uint32_t)check_block_num)
                    && (((uint32_t)wk_block_index.block_idx / FLS_LBLOCK_NUM_PER_PBLOCK)
                        == ((((uint32_t)wk_block_index.block_idx + (uint32_t)check_block_num) - 1uL)
                            / FLS_LBLOCK_NUM_PER_PBLOCK)))
                {
                    /* valid data size */
                    ret_val = ER_OK;
                }
                else
                {
                    /* data size error occurred : invalid data size */
                    ret_val = ER_MM_FLS_DATA_INVALID;
                }
            }
            else
            {
                /* data size error or data set type error occurred : invalid data size */
                ret_val = ER_MM_FLS_DATA_INVALID;
            }
        }
    }
    else if ((uint32_t)FLS_TAG_BLANK == (uint32_t)tag_result)
    {
        /* the TAG area is blank */
        ret_val = ER_MM_FLS_DATA_BLANK;
    }
    else if ((uint32_t)FLS_TAG_INVALID == (uint32_t)tag_result)
    {
        /* the TAG area is non-blank and the value is invalid */
        ret_val = ER_MM_FLS_DATA_INVALID;
    }
    else
    {
        /* error occurred */
        ret_val = err;
    }

    /* Read data area */
    if (ER_OK == ret_val)
    {
        /* Check Data TAG for data size */
        ret_val = mm_FLS_CheckDSizeTag(target_side, p_index, check_block_num);
        /* Read data area when all TAGs are valid */
        if (ER_OK == ret_val)
        {
            /* data area read */
            wk_block_index = *p_index;
            mm_FLS_ChangeAddr(target_side, wk_block_index, &offset);
            err = mm_FLS_ExecReadDF(AG_L_FLS_BANK_ICUM, offset, (uint32_t)data_buffer_pnt.p_header->data_size, p_data);
            ret_val = err;
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

    /* verification of read data */
    if (ER_OK == ret_val)
    {
        /* DVC verification of read data */
        ret_val = mm_FLS_VerifySlotData(data_buffer_pnt, p_data_status);
    }
    else
    {
        /* error occurred */
        p_data_status->dvc_result = FLS_CHK_NG;
        if (ER_MM_FLS_DATA_INVALID == ret_val)
        {
            ret_val = ER_OK;
        }
        else
        {
            /* do nothing */
        }
    }

    p_data_status->flash_err = ret_val;

    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0090]:[ICUMFW_UD_MM_FlashMNG_0090]
* Function name: mm_FLS_CheckDSizeTag
* Description  : Check Data TAG for data size
* Arguments    : (in) uint16_t target_side      : target side
*                (in) flash_location_t *p_index : first block index of data set
*                (in) uint16_t check_block_num  : Number of logical blocks in data set
* Return Value : ER_OK
*                ER_PARAM
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_DATA_INVALID (ER_L_FLS_ECC2EDF1)
*                ER_MM_FLS_MEM_SOURCE
*                ER_MM_FLS_DATA_BLANK
***************************************************************************************/
static uint32_t mm_FLS_CheckDSizeTag(const uint16_t target_side, const flash_location_t *const p_index,
                                     const uint16_t check_block_num)
{
    uint32_t err;
    uint32_t ret_val;
    uint32_t counter;
    flash_location_t wk_block_index;
    uint8_t tag_result;

    ret_val = ER_OK;
    wk_block_index = *p_index;

    /* Check Data TAG for data size */
    for (counter = FLS_ZERO_U32; counter < (uint32_t)check_block_num; counter++)
    {
        /* Check Data TAG */
        err = mm_FLS_CheckTag(target_side, &wk_block_index, &tag_result);
        if ((uint32_t)FLS_TAG_VALID == (uint32_t)tag_result)
        {
            /* the TAG area is non-blank and the value is valid */
            wk_block_index.block_idx = (uint16_t)((uint32_t)wk_block_index.block_idx + 1uL);
        }
        else
        {
            if ((uint32_t)FLS_TAG_BLANK == (uint32_t)tag_result)
            {
                /* the TAG area is blank */
                ret_val = ER_MM_FLS_DATA_BLANK;
            }
            else if ((uint32_t)FLS_TAG_INVALID == (uint32_t)tag_result)
            {
                /* the TAG area is non-blank and the value is invalid */
                ret_val = ER_MM_FLS_DATA_INVALID;
            }
            else
            {
                /* error occurred */
                ret_val = err;
            }
            break;
        }
    } /* end of for (counter = FLS_ZERO_U32; counter < (uint32_t)check_block_num; counter++) */
    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0091]:[ICUMFW_UD_MM_FlashMNG_0091]
* Function name: mm_FLS_VerifySlotData
* Description  : DVC verification of slot data
* Arguments    : (in) fls_data_area_pointer_t data_buffer_pnt   : A pointer to the memory that stores the obtained slot data
*                (out) data_status_t *p_data_status             : A pointer to the memory that stores the obtained slot data status
* Return Value : ER_OK
*                ER_PARAM
*                ER_MM_FLS_AES_PRM_ERROR
*                ER_MM_FLS_AES_TIME_OUT
***************************************************************************************/
static uint32_t mm_FLS_VerifySlotData(fls_data_area_pointer_t data_buffer_pnt, data_status_t *const p_data_status)
{
    uint32_t err;
    uint32_t ret_val;
ALIGNVAR(4)
    dvc_calculate_t dvc_param;
ALIGNVAR(4)
    dvc_t dvc_result;

    /* sets parameter for DVC processing */
    dvc_param.p_input   = (const uint8_t *)&(data_buffer_pnt.p_header->ds_type);
    dvc_param.data_size = (uint16_t)(((uint32_t)(data_buffer_pnt.p_header->data_size)) - ((uint32_t)sizeof(dvc_t)));
    dvc_param.p_output  = &dvc_result;

    /* calculate DVC */
#if defined(FSW_DVC_HASH)
    err = ML_DVC_Calculate(&dvc_param, ML_DVC_CALC_TYPE_HASH);
#else
    err = ML_DVC_Calculate(&dvc_param, ML_DVC_CALC_TYPE_CMAC);
#endif
    if (ER_OK == err)
    {
        /* compare calculated DVC and stored DVC */
        ret_val = L_Toolbox_MemCmp((const uint8_t *)dvc_param.p_output,
                                   (const uint8_t *)(&(data_buffer_pnt.p_header->dvc)),
                                   (int16_t)FLS_DVC_SIZE);
        if ((uint32_t)FLS_TOOL_CMP_OK == ret_val)
        {
            /* Both DVC matches */
            p_data_status->dvc_result = FLS_CHK_OK;
            p_data_status->counter    = data_buffer_pnt.p_header->update_counter;
            p_data_status->ds_type    = data_buffer_pnt.p_header->ds_type;
            p_data_status->slot_num   = data_buffer_pnt.p_header->slot_num;
            p_data_status->data_size  = data_buffer_pnt.p_header->data_size;
        }
        else
        {
            /* Both DVC mismatch */
            p_data_status->dvc_result = FLS_CHK_NG;
        }
        ret_val = ER_OK;
    }
#if !defined(FSW_DVC_HASH)
    else if (ER_ML_DVC_AES_TIMEOUT == err)
    {
        /* DVC calculation error (AES timeout) */
        ret_val = ER_MM_FLS_AES_TIME_OUT;
    }
    else if (ER_ML_DVC_ALIGN == err)
    {
        /* DVC calculation error (DVC alignment) */
        ret_val = ER_MM_FLS_AES_PRM_ERROR;
    }
#endif /* !defined(FSW_DVC_HASH) */
    else
    {
        /* never occur */
        ret_val = ER_PARAM;
    }
    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0001]:[ICUMFW_UD_MM_FlashMNG_0001]
* Function name: MM_FLS_Init
* Description  : Initialize Flash Manager
* Arguments    : none
* Return Value : ER_OK
***************************************************************************************/
uint32_t MM_FLS_Init(void)
{
    /* Initialization of process variables */
    mm_FLS_InitProcessVariable();
    g_phase_GCAll = FLS_PHASE_0;

    /* sets 0 data to data set elements */
    mm_FLS_ClearMap();

    /* Init NVM Data Warning */
    mm_FLS_InitNVMWarnData();

    /* retrieve ICUM-RAC and ICUM-RAD */
    L_FLS_RetrieveIcumRacRad();

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0002]:[ICUMFW_UD_MM_FlashMNG_0002]
* Function name: MM_FLS_Read
* Description  : read data slot from flash memory
* Arguments    : (in) uint8_t target_flash  : target of flash memory
*                (in) uint8_t target_area   : target of memory area
*                (in) uint8_t ds_type       : type of data-set
*                (in) uint8_t slot_num      : slot number
*                (in) uint16_t size         : size of read
*                (out) uint8_t *p_data      : an address pointer that stores the read data
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MM_FLS_SIZE
*                ER_MM_FLS_DATA_BLANK
*                ER_MM_FLS_DATA_INVALID
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*                ER_MM_FLS_AES_UNAVAILABLE
*                ER_MM_FLS_AES_TIME_OUT
*                ER_MM_FLS_AES_PRM_ERROR
*                ER_MM_FLS_MEM_SOURCE
*****************************************************************************************/
uint32_t MM_FLS_Read(uint8_t target_flash, uint8_t target_area, uint8_t ds_type, uint8_t slot_num, uint16_t size,
                     uint8_t *p_data)
{
    uint32_t ret_val;
    uint16_t data_set_size;
    flash_location_t wk_index;
    uint32_t update_counter;
    uint32_t fblock;
    gc_warning_t gc_warning;
    uint32_t lock_handle;

    data_set_size = FLS_ZERO_U16;
    lock_handle = FLS_DUMMY_LOCK_HANDLE;
    wk_index.block_idx = FLS_INVALID_BLOCK_IDX;

    /* Argument check */
    if ((uint32_t)FLS_SELECT_CF == (uint32_t)target_flash)
    {
        /* CodeFlash not supported */
        ret_val = ER_PARAM;
    }
    else
    {
        /* check of job id */
        ret_val = L_FLS_CheckContext_ram(FLS_ZERO_U16, AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
    }
    if (ER_OK == ret_val)
    {
        /* retrieve the data set size */
        mm_FLS_get_DataSet_size(ds_type, &data_set_size);
        if ((uint32_t)data_set_size != (uint32_t)size)
        {
            /* size check error */
            ret_val = ER_PARAM;
        }
        else
        {
            /* check of parameter */
            ret_val = mm_FLS_PreCheck(target_flash, target_area, ds_type, data_set_size, FLS_OP_READ);
        }
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK == ret_val)
    {
        /* Read data map */
        ret_val = mm_FLS_ReadMapData(ds_type, slot_num, &wk_index, &update_counter);
        if (ER_OK == ret_val)
        {
            /* Get flash resource */
            ret_val = L_FLS_GetFlashResource_ram(lock_handle, R_ICUMIF_SERV_PRIORITY_HIGHEST,
                                                 AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
            if (ER_OK == ret_val)
            {
                /* data set Read & two side check */
                ret_val = mm_FLS_AESDSRead(&wk_index, p_data);
                /* Free flash resource */
                (void)L_FLS_FreeFlashResource_ram(AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
            }
            else
            {
                /* do nothing */
            }
        }
        else
        {
            /* do nothing : data not found */
            /* only error return */
        }
    }
    else
    {
        /* do nothing */
    }

    if (ER_MM_FLS_DATA_INVALID == ret_val)
    {
        g_nvm_data_warning = FLS_FULL_WARNING2;
        if ((uint32_t)FLS_INVALID_BLOCK_IDX != (uint32_t)wk_index.block_idx)
        {
            fblock = mm_FLS_GetFBlock(wk_index.block_idx);
            g_write_block_idx[fblock] = (uint16_t)((uint32_t)mm_FLS_GetLastLBlock(fblock) + 1uL);
        }
        else
        {
            /* do nothing */
        }
    }
    else if (ER_MM_FLS_DATA_BLANK != ret_val)
    {
        /* Calculate GC necessity from remaining flash capacity */
        gc_warning = mm_FLS_GetCapacityStatus();
        if (((uint32_t)NVM_DATA_WARNING == (uint32_t)g_nvm_data_warning)
            && ((uint32_t)FLS_NO_FULL_WARNING == (uint32_t)gc_warning))
        {
            /* do nothing */
        }
        else
        {
            g_nvm_data_warning = gc_warning;
        }
    }
    else
    {
        /* do nothing */
    }

    ret_val = mm_FLS_ConvertionError(ret_val);
    return (ret_val);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0003]:[ICUMFW_UD_MM_FlashMNG_0003]
* Function name: MM_FLS_Write
* Description  : data slot write to flash memory
* Arguments    : (in) r_icumif_job_id_t job_id  : job id
*                (in) uint8_t target_flash  : target of flash memory
*                (in) uint8_t target_area   : target of memory area
*                (in) uint8_t ds_type       : type of data-set
*                (in) uint8_t slot_num      : slot number
*                (in) uint16_t size         : size of write
*                (in) uint8_t *p_data       : an address pointer that stores the write data
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_AES_UNAVAILABLE
*                ER_MM_FLS_SIZE
*                ER_MM_FLS_AES_TIME_OUT
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_ALL_BLANK
*                ER_MM_FLS_FORMAT_ERR
*                ER_MM_FLS_UNAVAILABLE
*                ER_MM_FLS_AES_PRM_ERROR
*****************************************************************************************/
uint32_t MM_FLS_Write(r_icumif_job_id_t job_id, uint8_t target_flash, uint8_t target_area, uint8_t ds_type,
                      uint8_t slot_num, uint16_t size, uint8_t *p_data)
{
    uint32_t err;
    uint16_t data_set_size;
    static uint32_t update_counter;
    fls_data_area_pointer_t data_buffer_pnt;
    static flash_location_t wk_index;
    uint32_t prohibited_block;
    uint32_t fblock;
    uint8_t flash_full;

    data_set_size = FLS_ZERO_U16;
    flash_full = FLS_ZERO_U8;
    data_buffer_pnt.p_u8 = p_data;

    /* check of job id */
    if (FLS_ZERO_U32 == (uint32_t)job_id)
    {
        /* does not occur after the UT/IT process */
        err = ER_PARAM;
    }
    else if ((uint32_t)FLS_SELECT_CF == (uint32_t)target_flash)
    {
        /* CodeFlash not supported */
        err = ER_PARAM;
    }
    else
    {
        /* Check context */
        err = L_FLS_CheckContext_ram(job_id, AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
    }

    if ((ER_OK == err) || (ER_IN_PROCESS == err))
    {
        /* retrieve the data set size */
        mm_FLS_get_DataSet_size(ds_type, &data_set_size);
        if ((uint32_t)data_set_size != (uint32_t)size)
        {
            /* size check error */
            err = ER_PARAM;
            g_phase_Write = FLS_PHASE_9;
        }
        else
        {
            /* size check OK */
            /* Do nothing */
        }

        switch ((uint32_t)g_phase_Write)
        {
            case FLS_PHASE_0:
                mm_FLS_InitProcessVariable();
                /* check of parameter */
                err = mm_FLS_PreCheck(target_flash, target_area, ds_type, data_set_size, FLS_OP_WRITE);
                if (ER_OK != err)
                {
                    g_phase_Write = FLS_PHASE_9;
                    break;
                }
                else
                {
                    /* normal end : do nothing */
                }
                /* get flash resource & sets FACIACC flag */
                err = mm_FLS_GetFlashResource(job_id);
                if (ER_OK != err)
                {
                    g_phase_Write = FLS_PHASE_9;
                    break;
                }
                else
                {
                    /* normal end : do nothing */
                }

                /* get prohibited block number */
                err = mm_FLS_GetProhibitBlk(&prohibited_block);
                if (ER_OK != err)
                {
                    g_phase_Write = FLS_PHASE_9;
                    break;
                }
                else
                {
                    /* normal end : do nothing */
                }

                /* Search for blocks with writable free area */
                err = mm_FLS_GetWritableArea(prohibited_block, data_set_size, &(wk_index.block_idx));
                wk_index.word_idx = FLS_ZERO_U16;
                if (FLS_FOUND == err)
                {
                    /* When there is writable block : do nothing */
                }
                else
                {
                    /* When there is no writable block */
                    flash_full = FLS_FLAG_ON_U8;
                    g_phase_Write = FLS_PHASE_9;
                    break;
                }

                /* Update counter increment, DVC calculation, write start */
                err = mm_FLS_UpdateWrite(
                    job_id, ds_type, slot_num, data_set_size, data_buffer_pnt, &wk_index, &update_counter);
                break;

            case FLS_PHASE_1:
                /* Wait for write completion */
                err = mm_FLS_DSWrite(job_id, data_set_size, &wk_index, data_buffer_pnt.p_u8);
                if (ER_OK == err)
                {
                    /* write data successful */
                    fblock = mm_FLS_GetFBlock(wk_index.block_idx);
                    g_write_block_idx[fblock] =
                        (uint16_t)((uint32_t)g_write_block_idx[fblock] + ((uint32_t)data_set_size / FLS_DF_BLOCK_SIZE));
                    err = mm_FLS_WriteMapData(ds_type, slot_num, wk_index.block_idx, update_counter);
                    g_phase_Write = FLS_PHASE_9;
                }
                else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
                {
                    /* write process in progress */
                }
                else
                {
                    /* error occurred */
                    g_phase_Write = FLS_PHASE_9;
                }
                break;

            default:
                /* Not occurred */
                break;
        } /* end of switch ((uint32_t)g_phase_Write) */
    } /* end of if ((ER_OK == err) || (ER_IN_PROCESS == err)) */
    else
    {
        /* Do nothing */
    }

    /* Phase of end process */
    fblock = mm_FLS_GetFBlock(wk_index.block_idx);
    mm_FLS_EndPhaseWrite(&err, &g_phase_Write, fblock, flash_full);
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0092]:[ICUMFW_UD_MM_FlashMNG_0092]
* Function name: mm_FLS_GetProhibitBlk
* Description  : get prohibited block number
* Arguments    : (out) uint32_t* p_prohibited_block : prohibited block number
* Return Value : ER_OK
*                ER_PARAM
*                ER_MM_FLS_ALL_BLANK
*                ER_MM_FLS_FORMAT_ERR
***************************************************************************************/
static uint32_t mm_FLS_GetProhibitBlk(uint32_t *const p_prohibited_block)
{
    uint32_t err;
    uint32_t source_block;
    uint32_t reserve_block;
    gc_state_t gc_state;

    source_block = FLS_ZERO_U32;
    reserve_block = FLS_ZERO_U32;
    /* check flash memory status */
    err = mm_FLS_CheckGCFlag(&gc_state, &source_block, &reserve_block);
    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        /* normal end */
        if (FLS_GC_NORMAL == gc_state)
        {
            /* When the power is not turned off during GC */
            *p_prohibited_block = reserve_block;
        }
        else if (FLS_GC_DURING_1 == gc_state)
        {
            /* When the power is turned off during GC (during data transfer) */
            *p_prohibited_block = reserve_block;
        }
        else if (FLS_GC_DURING_2 == gc_state)
        {
            /* When the power is turned off during GC (during erase) */
            *p_prohibited_block = source_block;
        }
        else if (FLS_GC_ALL_BLANK == gc_state)
        {
            /* When the flash memory is not yet formatted */
            err = ER_MM_FLS_ALL_BLANK;
        }
        else
        {
            /* When other flash states */
            err = ER_MM_FLS_FORMAT_ERR;
        }
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0093]:[ICUMFW_UD_MM_FlashMNG_0093]
* Function name: mm_FLS_UpdateWrite
* Description  : Update counter increment, DVC calculation, write start (for MM_FLS_Write)
* Arguments    : (in) r_icumif_job_id_t job_id          : job id
*                (in) uint8_t ds_type                   : type of data-set
*                (in) uint8_t slot_num                  : slot number
*                (in) uint16_t size                     : size of write
*                (in) fls_data_area_pointer_t data_buffer_pnt   : data buffer pointer
*                (in) flash_location_t* p_wk_index      : block index of write
*                (out) uint32_t* p_update_counter       : update counter
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*                ER_MM_FLS_AES_UNAVAILABLE
*                ER_MM_FLS_AES_TIME_OUT
*                ER_MM_FLS_AES_PRM_ERROR
***************************************************************************************/
static uint32_t mm_FLS_UpdateWrite(r_icumif_job_id_t job_id, const uint8_t ds_type, const uint8_t slot_num,
                                   const uint16_t size, const fls_data_area_pointer_t data_buffer_pnt,
                                   const flash_location_t *const p_wk_index, uint32_t *const p_update_counter)
{
    uint32_t err;
    flash_location_t dummy_index;
    dvc_calculate_t dvc_param;

    /* Read update counter from previously written data */
    err = mm_FLS_ReadMapData(ds_type, slot_num, &dummy_index, p_update_counter);
    if (ER_OK == err)
    {
        /* If there is previous data, increment the update counter */
        *p_update_counter =
            mm_FLS_IncValue(*p_update_counter, FLS_FULL_U32);
    }
    else
    {
        /* If there is no previous data, set the update counter to 0 */
        *p_update_counter = FLS_ZERO_U32;
    }

    /* Set header information in write buffer */
    data_buffer_pnt.p_header->ds_type = ds_type;
    data_buffer_pnt.p_header->slot_num = slot_num;
    data_buffer_pnt.p_header->data_size = size;
    data_buffer_pnt.p_header->update_counter = *p_update_counter;
    /* DVC calculation of write buffer data */
    dvc_param.p_input = (const uint8_t *)&(data_buffer_pnt.p_header->ds_type);
    dvc_param.data_size = (uint16_t)((uint32_t)size - (uint32_t)sizeof(dvc_t));
    dvc_param.p_output = &(data_buffer_pnt.p_header->dvc);
    /* DVC calculation with AES resource processing */
    err = mm_FLS_CalculateDVC(&dvc_param, &g_phase_Write);
    if (ER_OK == err)
    {
        /* write data */
        err = mm_FLS_DSWrite(job_id, size, p_wk_index, data_buffer_pnt.p_u8);
        if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
        {
            /* write process in progress */
            g_phase_Write = FLS_PHASE_1;
        }
        else
        {
            /* error occurred */
            g_phase_Write = FLS_PHASE_9;
        }
    }
    else
    {
    }
    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0004]:[ICUMFW_UD_MM_FlashMNG_0004]
* Function name: MM_FLS_Erase
* Description  : erase the data slot in the flash memory.
* Arguments    : (in) r_icumif_job_id_t job_id : job id
*                (in) uint8_t target_flash  : target of flash memory
*                (in) uint8_t target_area   : target of memory area
*                (in) uint8_t ds_type       : type of data-set
*                (in) uint8_t slot_num      : slot number
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL,ER_L_FLS_FACI_TIME_OUT)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*                ER_MM_FLS_DATA_BLANK
*                ER_MM_FLS_DATA_INVALID
*                ER_MM_FLS_MEM_SOURCE
*                ER_MM_FLS_AES_UNAVAILABLE
*                ER_MM_FLS_AES_TIME_OUT
*                ER_MM_FLS_AES_PRM_ERROR
*                ER_MM_FLS_FULL_DATA_MAP
*                ER_MM_FLS_ALL_BLANK
*                ER_MM_FLS_FORMAT_ERR
*****************************************************************************************/
uint32_t MM_FLS_Erase(r_icumif_job_id_t job_id, uint8_t target_flash, uint8_t target_area, uint8_t ds_type,
                      uint8_t slot_num)
{
    uint32_t err;

    /* check of job id */
    if (FLS_ZERO_U32 == (uint32_t)job_id)
    {
        /* does not occur after the UT/IT process */
        err = ER_PARAM;
    }
    else if ((uint32_t)FLS_SELECT_CF == (uint32_t)target_flash)
    {
        /* CodeFlash not supported */
        err = ER_PARAM;
    }
    else if ((uint32_t)FLS_SELECT_SECURE != (uint32_t)target_area)
    {
        /* Supports secure flash only */
        err = ER_PARAM;
    }
    else
    {
        /* Check context */
        err = L_FLS_CheckContext_ram(job_id, AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
    }

    if ((ER_OK == err) || (ER_IN_PROCESS == err))
    {
        /* Deleting entries from the data map */
        (void)MM_FLS_DeleteMapData(ds_type, slot_num);
        /* All block garbage collection */
        err = mm_FLS_CollectGarbageFull(job_id);
    }
    else
    {
        /* Do nothing */
    }
    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0085]:[ICUMFW_UD_MM_FlashMNG_0085]
* Function name: mm_FLS_CheckWriteCfgParam
* Description  : Check write configuration data.
* Arguments    : (in) r_icumif_job_id_t job_id  : job id
*                (in) uint8_t target_func       : target of option byte
*                (in) uint32_t set_data         : option byte data
*                (out) uint32_t *p_target_info  : target info
*                (out) uint32_t *p_value        : set value
* Return Value : ER_OK
*                ER_PARAM
*****************************************************************************************/
static uint32_t mm_FLS_CheckWriteCfgParam(const r_icumif_job_id_t job_id, const uint8_t target_func,
                                          const uint32_t set_data, uint32_t *const p_target_info,
                                          uint32_t *const p_value)
{
    uint32_t err;

    err = ER_OK;

    if (FLS_ZERO_U32 == (uint32_t)job_id)
    {
        err = ER_PARAM;
    }
    else
    {
        /* check of exclusion and parameter */
        switch ((uint32_t)target_func)
        {
            case FLS_PROG_ICUMUPGP:
                *p_target_info = AG_L_FLS_ICUMUPGP;
                *p_value = set_data;
                break;
            case FLS_PROG_ICUMRESV:
                *p_target_info = AG_L_FLS_ICUMRESV;
                *p_value = set_data;
                break;
            case FLS_PROG_ICUMRACS0:
                *p_target_info = AG_L_FLS_ICUMRACS0;
                *p_value = set_data;
                break;
            case FLS_PROG_ICUMRACE0:
                *p_target_info = AG_L_FLS_ICUMRACE0;
                *p_value = set_data;
                break;
            case FLS_PROG_ICUMRACS1:
                *p_target_info = AG_L_FLS_ICUMRACS1;
                *p_value = set_data;
                break;
            case FLS_PROG_ICUMRACE1:
                *p_target_info = AG_L_FLS_ICUMRACE1;
                *p_value = set_data;
                break;
            case FLS_PROG_ICUMRAD:
                *p_target_info = AG_L_FLS_ICUMRAD;
                *p_value = set_data;
                break;
            case FLS_PROG_CR_RES:
                *p_target_info = AG_L_FLS_CHGRESP;
                break;
            case FLS_PROG_DEGRADATION:
                *p_target_info = AG_L_FLS_DEGRADATION;
                *p_value = set_data;
                break;
            default:
                err = ER_PARAM;
                break;
        } /* end of switch ((uint32_t)target_func) */

        if ((ER_OK == err) && (AG_L_FLS_CHGRESP == *p_target_info))
        {
            switch (set_data)
            {
                case FLS_PROG_OPBT_CR_ICUP_SKIP:
                    *p_value = AG_L_FLS_CR_RES_ICUP_SKIPPED;
                    break;
                case FLS_PROG_OPBT_CR_CPU_SKIP:
                    *p_value = AG_L_FLS_CR_RES_CPU_SKIPPED;
                    break;
                case FLS_PROG_OPBT_CR_ALL_SKIP:
                    *p_value = AG_L_FLS_CR_RES_ALL_SKIPPED;
                    break;
                case FLS_PROG_OPBT_CR_ICUP_ENABLED:
                    *p_value = AG_L_FLS_CR_RES_ICUP_ENABLED;
                    break;
                case FLS_PROG_OPBT_CR_CPU_ENABLED:
                    *p_value = AG_L_FLS_CR_RES_CPU_ENABLED;
                    break;
                case FLS_PROG_OPBT_CR_ALL_ENABLE:
                    *p_value = AG_L_FLS_CR_RES_ALL_ENABLED;
                    break;
                case FLS_PROG_OPBT_PEDBG_ENABLE:
                    *p_value = AG_L_FLS_OPBT_PEDBG_ENABLE;
                    break;
                case FLS_PROG_OPBT_PEDBG_DISABLE:
                    *p_value = AG_L_FLS_OPBT_PEDBG_DISABLE;
                    break;
                case FLS_PROG_OPBT_GPIO_DISABLE:
                    *p_value = AG_L_FLS_OPBT_GPIO_DISABLE;
                    break;
                case FLS_PROG_OPBT_GPIO_ENABLE:
                    *p_value = AG_L_FLS_OPBT_GPIO_ENABLE;
                    break;
                case FLS_PROG_OPBT_RHSIF_NOT_NEED:
                    *p_value = AG_L_FLS_OPBT_RHSIF_NOT_NEED;
                    break;
                case FLS_PROG_OPBT_RHSIF_NEED:
                    *p_value = AG_L_FLS_OPBT_RHSIF_NEED;
                    break;
                case FLS_PROG_OPBT_WDTA_DISABLE:
                    *p_value = AG_L_FLS_OPBT_WDTA_DISABLE;
                    break;
                case FLS_PROG_OPBT_WDTA_ENABLE:
                    *p_value = AG_L_FLS_OPBT_WDTA_ENABLE;
                    break;
                default:
                    err = ER_PARAM;
                    break;
            } /* end of switch (set_data) */
        } /* end of if ((ER_OK == err) && (AG_L_FLS_CHGRESP == *p_target_info)) */
        else if ((ER_OK == err) && (AG_L_FLS_ICUMUPGP == *p_target_info))
        {
            switch (set_data)
            {
                case FLS_PROG_OPBT_UPGP1_ENABLE:
                    *p_value = AG_L_FLS_OPBT_UPGP1_ENABLE;
                    break;
                case FLS_PROG_OPBT_UPGP1_DISABLE:
                    *p_value = AG_L_FLS_OPBT_UPGP1_DISABLE;
                    break;
                case FLS_PROG_OPBT_UPGP2_ENABLE:
                    *p_value = AG_L_FLS_OPBT_UPGP2_ENABLE;
                    break;
                case FLS_PROG_OPBT_UPGP2_DISABLE:
                    *p_value = AG_L_FLS_OPBT_UPGP2_DISABLE;
                    break;
                default:
                    err = ER_PARAM;
                    break;
            } /* end of switch (set_data) */
        } /* end of else if ((ER_OK == err) && (AG_L_FLS_ICUMUPGP == *p_target_info)) */
        else
        {
            /* Do nothing */
        }
    } /* end of else */

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0005]:[ICUMFW_UD_MM_FlashMNG_0005]
* Function name: MM_FLS_WriteFlashCfg
* Description  : write data in the configure.
* Arguments    : (in)  r_icumif_job_id_t job_id : job id
*                (in) uint8_t target_func       : target of option byte
*                (in) uint32_t set_data         : option byte data
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL,ER_L_FLS_FACI_TIME_OUT)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*****************************************************************************************/
uint32_t MM_FLS_WriteFlashCfg(r_icumif_job_id_t job_id, uint8_t target_func, uint32_t set_data)
{
    uint32_t err;
    uint32_t ret_val;
    uint32_t target_info;
    uint32_t opbt_info;

    err = mm_FLS_CheckWriteCfgParam(job_id, target_func, set_data, &target_info, &opbt_info);
    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        /* check of job id */
        err = L_FLS_CheckContext_ram(job_id, AG_L_FLS_SECURITY, AG_L_FLS_BANK_EXT);
        if (ER_OK == err)
        {
            /* set FACIACC flag */
            err = L_FLS_SetFACIStatus_ram();
            if (ER_OK == err)
            {
                /* Get flash resources */
                err = L_FLS_GetFlashResource_ram((uint32_t)job_id, FLS_ZERO_U8, AG_L_FLS_SECURITY, AG_L_FLS_BANK_EXT);
                if (ER_OK == err)
                {
                    /* perform the flash erasing */
                    err = L_FLS_REQ_WriteSecurity_ram(target_info, opbt_info);
                    mm_FLS_EndProcessWFCfg(&err);
                }
                else
                {
                    /* reset FACIACC flag */
                    (void)L_FLS_ClrFACIStatus_ram();
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else if (ER_IN_PROCESS == err)
        {
            err = L_FLS_IsFinished_ram(job_id, AG_L_FLS_SECURITY, AG_L_FLS_BANK_EXT);
            mm_FLS_EndProcessWFCfg(&err);
        }
        else
        {
            /* Do nothing */
        }
    } /* end of else */

    ret_val = mm_FLS_ConvertionError(err);
    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0094]:[ICUMFW_UD_MM_FlashMNG_0094]
* Function name: mm_FLS_EndProcessWFCfg
* Description  : end process (for MM_FLS_WriteFlashCfg)
* Arguments    : (in/out) uint32_t* p_err   : Error code
* Return Value : none
***************************************************************************************/
static void mm_FLS_EndProcessWFCfg(uint32_t *const p_err)
{
    if (ER_IN_PROCESS == *p_err)
    {
        /* process is progress */
        *p_err = ER_MM_FLS_IN_PROGRESS;
    }
    else
    {
        /* Release flash resources */
        (void)L_FLS_FreeFlashResource_ram(AG_L_FLS_SECURITY, AG_L_FLS_BANK_EXT);
        if (ER_OK == *p_err)
        {
            L_FLS_RetrieveIcumRacRad();
        }
        else
        {
            /* Do nothing */
        }
        /* reset FACIACC flag */
        (void)L_FLS_ClrFACIStatus_ram();
    }
}

/* self copy icum_middle_rom.text icum_middle_ram   */
CHANGE_SEC(text, ".icum_middle_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0007]:[ICUMFW_UD_MM_FlashMNG_0007]
* Function name: MM_FLS_EraseCF
* Description  : Code Flash Erase
* Arguments    : (in)  r_icumif_job_id_t job_id : job id
*                (in) r_icumif_service_priority_t service_priority : priority
*                (in) uint8_t target_area       : target area
*                (in) uint32_t cf_addr          : Code Flash address
*                (in) uint32_t size             : Erase size
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MM_FLS_MEM_DEST
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*****************************************************************************************/
uint32_t MM_FLS_EraseCF(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority, uint8_t target_area,
                        const uint32_t cf_addr, uint32_t size)
{
    uint32_t err;
    uint32_t ret_val;
    uint32_t bank;
    uint32_t offset;
    uint32_t lock_handle;

    bank = FLS_ZERO_U32;
    offset = FLS_ZERO_U32;
    lock_handle = (uint32_t)job_id;
    /* check of job id */
    if ((FLS_ZERO_U32 == (uint32_t)job_id) || ((uint32_t)FLS_SELECT_SECURE != (uint32_t)target_area))
    {
        err = ER_PARAM;
    }
    else
    {
        err = L_FLS_CheckSCFBoundary((uint32_t)cf_addr, size, &bank, &offset);
        if (ER_OK != err)
        {
            err = ER_MM_FLS_MEM_DEST;
        }
        else
        {
            /* Check context */
            err = L_FLS_CheckContext_ram(job_id, AG_L_FLS_CODE, bank);
            if (ER_OK == err)
            {
                /* set FACIACC flag */
                err = L_FLS_SetFACIStatus_ram();
                if (ER_OK == err)
                {
                    /* Get flash resources */
                    err = L_FLS_GetFlashResource_ram(lock_handle, service_priority, AG_L_FLS_CODE, bank);
                    if (ER_OK == err)
                    {
                        err = L_FLS_REQ_EraseFlash_ram(AG_L_FLS_CODE, bank, offset, size);
                        if (ER_IN_PROCESS == err)
                        {
                            /* process is progress */
                            err = ER_MM_FLS_IN_PROGRESS;
                        }
                        else
                        {
                            /* Release flash resources */
                            (void)L_FLS_FreeFlashResource_ram(AG_L_FLS_CODE, bank);
                            /* reset FACIACC flag */
                            (void)L_FLS_ClrFACIStatus_ram();
                        }
                    }
                    else
                    {
                        /* reset FACIACC flag */
                        (void)L_FLS_ClrFACIStatus_ram();
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
            else if (ER_IN_PROCESS == err)
            {
                err = L_FLS_IsFinished_ram((uint16_t)lock_handle, AG_L_FLS_CODE, bank);
                if (ER_IN_PROCESS == err)
                {
                    /* process is progress */
                    err = ER_MM_FLS_IN_PROGRESS;
                }
                else
                {
                    /* Release flash resources */
                    (void)L_FLS_FreeFlashResource_ram(AG_L_FLS_CODE, bank);
                    /* reset FACIACC flag */
                    (void)L_FLS_ClrFACIStatus_ram();
                }
            }
            else
            {
                /* Do nothing */
            }
        } /* end of else */
    } /* end of else */

    ret_val = mm_FLS_ConvertionError(err);

    return (ret_val);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0006]:[ICUMFW_UD_MM_FlashMNG_0006]
* Function name: MM_FLS_WriteCF
* Description  : Code Flash Write
* Arguments    : (in)  r_icumif_job_id_t job_id : job id
*                (in) r_icumif_service_priority_t service_priority : priority
*                (in) uint8_t target_area       : target area
*                (in) uint32_t cf_addr          : Code Flash address
*                (in) uint32_t size             : Write size
*                (in) const uint32_t *p_data    : data buffer address
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MM_FLS_MEM_DEST
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*****************************************************************************************/
uint32_t MM_FLS_WriteCF(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority, uint8_t target_area,
                        const uint32_t cf_addr, uint32_t size, const uint32_t *p_data)
{
    uint32_t err;
    uint32_t ret_val;
    uint32_t bank;
    uint32_t offset;
    uint32_t lock_handle;

    bank = FLS_ZERO_U32;
    offset = FLS_ZERO_U32;
    lock_handle = (uint32_t)job_id;
    /* check of job id */
    if ((FLS_ZERO_U32 == (uint32_t)job_id) || ((uint32_t)FLS_SELECT_SECURE != (uint32_t)target_area))
    {
        err = ER_PARAM;
    }
    else
    {
        err = L_FLS_CheckSCFBoundary((uint32_t)cf_addr, size, &bank, &offset);
        if (ER_OK != err)
        {
            err = ER_MM_FLS_MEM_DEST;
        }
        else
        {
            /* Check context */
            err = L_FLS_CheckContext_ram(job_id, AG_L_FLS_CODE, bank);
            if (ER_OK == err)
            {
                err = L_FLS_SetFACIStatus_ram();
                if (ER_OK == err)
                {
                    err = L_FLS_GetFlashResource_ram(lock_handle, service_priority, AG_L_FLS_CODE, bank);
                    if (ER_OK == err)
                    {
                        err = L_FLS_REQ_WriteFlash_ram(AG_L_FLS_CODE, bank, offset, size, (const uint8_t *)p_data);
                        if (ER_IN_PROCESS == err)
                        {
                            /* process is progress */
                            err = ER_MM_FLS_IN_PROGRESS;
                        }
                        else
                        {
                            /* Release flash resources */
                            (void)L_FLS_FreeFlashResource_ram(AG_L_FLS_CODE, bank);
                            /* reset FACIACC flag */
                            (void)L_FLS_ClrFACIStatus_ram();
                        }
                    }
                    else
                    {
                        /* reset FACIACC flag */
                        (void)L_FLS_ClrFACIStatus_ram();
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
            else if (ER_IN_PROCESS == err)
            {
                err = L_FLS_IsFinished_ram((uint16_t)lock_handle, AG_L_FLS_CODE, bank);
                if (ER_IN_PROCESS == err)
                {
                    /* process is progress */
                    err = ER_MM_FLS_IN_PROGRESS;
                }
                else
                {
                    /* Release flash resources */
                    (void)L_FLS_FreeFlashResource_ram(AG_L_FLS_CODE, bank);
                    /* reset FACIACC flag */
                    (void)L_FLS_ClrFACIStatus_ram();
                }
            }
            else
            {
                /* Do nothing */
            }
        } /* end of else */
    } /* end of else */

    ret_val = mm_FLS_ConvertionError(err);

    return (ret_val);
}

/* self copy icum_middle_rom.text icum_middle_ram   */
CHANGE_SEC(text, ".icum_middle_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0079]:[ICUMFW_UD_MM_FlashMNG_0079]
* Function name: MM_FLS_CheckRamExecRequired
* Description  : 
* Arguments    : uint8_t target_flash   : target flash area (FLS_SELECT_CF or FLS_SELECT_DF)
*                uint32_t *p_addr       : target address
*                uint8_t *p_result      : RAM execution required = true, unnecessary = false
* Return Value : ER_OK
*                ER_PARAM
*****************************************************************************************/
uint32_t MM_FLS_CheckRamExecRequired(uint8_t target_flash, uint32_t *p_addr, uint8_t *p_result)
{
    uint32_t err;

    if ((uint32_t)FLS_SELECT_CF == (uint32_t)target_flash)
    {
        err = L_FLS_CheckRamExecRequired(AG_L_FLS_CODE, (uint32_t)p_addr, p_result);
    }
    else
    {
        *p_result = (uint8_t)false;
        err = ER_OK;
    }

    return (err);
}

/* self copy icum_middle_rom.text icum_middle_ram   */
CHANGE_SEC(text, ".icum_middle_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0078]:[ICUMFW_UD_MM_FlashMNG_0078]
* Function name: mm_FLS_CheckParamUser
* Description  : User area Erase / Write parameter check and return of base address, 
*                target bank, AG_L_FLS_CODE or AG_L_FLS_CODE.
* Arguments    : r_icumif_job_id_t job_id   : job id
*                uint8_t target_flash       : target flash area (FLS_SELECT_CF or FLS_SELECT_DF)
*                uint8_t target_area        : target area (FLS_SELECT_USER or FLS_SELECT_USER)
*                uint32_t addr              : target address
*                uint32_t size              : target size
*                uint32_t *p_bank           : Buffer to store elected bank ID
*                uint32_t *p_offset         : Buffer to store elected offset address
* Return Value : ER_OK
*                ER_PARAM
*                ER_MM_FLS_MEM_DEST
*****************************************************************************************/
static uint32_t mm_FLS_CheckParamUser(const r_icumif_job_id_t job_id, const uint8_t target_flash,
                                      const uint8_t target_area, uint32_t addr, uint32_t size, uint32_t *p_bank,
                                      uint32_t *p_offset)
{
    uint32_t err;

    if ((FLS_ZERO_U32 == (uint32_t)job_id)
        || ((uint32_t)FLS_SELECT_USER != (uint32_t)target_area)
        || (((uint32_t)FLS_SELECT_DF != (uint32_t)target_flash) && ((uint32_t)FLS_SELECT_CF != (uint32_t)target_flash)))
    {
        err = ER_PARAM;
    }
    else
    {
        if ((uint32_t)FLS_SELECT_DF == (uint32_t)target_flash)
        {
            err = L_FLS_CheckUserDFBoundary(addr, size, p_bank, p_offset);
        }
        else
        {
            err = L_FLS_CheckUserCFBoundary(addr, size, p_bank, p_offset);
        }

        if (ER_OK != err)
        {
            err = ER_MM_FLS_MEM_DEST;
        }
        else
        {
            /* Do nothing */
        }
    }

    return (err);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_middle.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_middle.rodata")
/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0074]:[ICUMFW_UD_MM_FlashMNG_0074]
* Function name: MM_FLS_ReadUser
* Description  : Data flash read
* Arguments    : r_icumif_job_id_t job_id   : job id
*                r_icumif_service_priority_t service_priority : priority
*                uint8_t target_flash       : target flash area (FLS_SELECT_DF)
*                uint8_t target_area        : target area (FLS_SELECT_USER or FLS_SELECT_USER)
*                uint32_t *p_addr           : User Area Flash address
*                uint32_t size              : Read size
*                uint8_t *p_data            : Pointer to store the read data
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MM_FLS_MEM_SOURCE
*                ER_MM_FLS_MEM_DEST
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*****************************************************************************************/
uint32_t MM_FLS_ReadUser(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority, uint8_t target_flash,
                         uint8_t target_area, uint32_t *p_addr, uint32_t size, uint8_t *p_data)
{
    uint32_t err;
    uint32_t ret_val;
    uint32_t bank;
    uint32_t offset;
    uint32_t lock_handle;

    lock_handle = (uint32_t)job_id;
    err = mm_FLS_CheckParamUser(job_id, target_flash, target_area, (uint32_t)p_addr, size, &bank, &offset);
    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        if ((uint32_t)FLS_SELECT_DF == (uint32_t)target_flash)
        {
            /* Check context */
            err = L_FLS_CheckContext_ram(job_id, AG_L_FLS_DATA, bank);
            if (ER_OK == err)
            {
                /* set FACIACC flag */
                err = L_FLS_SetFACIStatus_ram();
                if (ER_OK == err)
                {
                    /* Get flash resources */
                    err = L_FLS_GetFlashResource_ram(lock_handle, service_priority, AG_L_FLS_DATA, bank);
                    if (ER_OK == err)
                    {
                        err = L_FLS_ReadFlash_ram(AG_L_FLS_DATA, bank, offset, size, p_data);
                        if ((ER_OK == err) || (ER_L_FLS_ECC1EDF1 == err))
                        {
                            /* reading data successful */
                            err = ER_OK;
                        }
                        else
                        {
                            /* Another error */
                            /* Do nothing */
                        }
                        /* Release flash resources */
                        (void)L_FLS_FreeFlashResource_ram(AG_L_FLS_DATA, bank);
                        /* reset FACIACC flag */
                        (void)L_FLS_ClrFACIStatus_ram();
                    }
                    else
                    {
                        /* reset FACIACC flag */
                        (void)L_FLS_ClrFACIStatus_ram();
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
            else if (ER_IN_PROCESS == err)
            {
                /* process is progress */
                err = ER_MM_FLS_IN_PROGRESS;
            }
            else
            {
                /* Do nothing */
            }
        } /* end of if ((uint32_t)FLS_SELECT_DF == (uint32_t)target_flash) */
        else
        {
            /* error occurred */
            err = ER_PARAM;
        }
    } /* end of else */

    ret_val = mm_FLS_ConvertionError(err);
    if (ER_PARAM == ret_val)
    {
        ret_val = ER_MM_FLS_MEM_SOURCE;
    }
    else
    {
        /* do nothing */
    }

    return (ret_val);
}

/* self copy icum_middle_rom.text icum_middle_ram   */
CHANGE_SEC(text, ".icum_middle_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0075]:[ICUMFW_UD_MM_FlashMNG_0075]
* Function name: MM_FLS_EraseUser
* Description  : User Area CF / DF Erase
* Arguments    : r_icumif_job_id_t job_id  : job id
*                r_icumif_service_priority_t service_priority : priority
*                uint8_t target_flash      : target flash area (FLS_SELECT_CF or FLS_SELECT_DF)
*                uint8_t target_area       : target area (FLS_SELECT_USER or FLS_SELECT_USER)
*                uint32_t *p_addr          : User Area Flash address
*                uint32_t size             : Erase size
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MM_FLS_MEM_DEST
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*****************************************************************************************/
uint32_t MM_FLS_EraseUser(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority, uint8_t target_flash,
                          uint8_t target_area, uint32_t *p_addr, uint32_t size)
{
    uint32_t err;
    uint32_t ret_val;
    uint32_t area;
    uint32_t bank;
    uint32_t offset;
    uint32_t lock_handle;

    lock_handle = (uint32_t)job_id;
    err = mm_FLS_CheckParamUser(job_id, target_flash, target_area, (uint32_t)p_addr, size, &bank, &offset);
    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        if ((uint32_t)FLS_SELECT_DF == (uint32_t)target_flash)
        {
            area = AG_L_FLS_DATA;
        }
        else
        {
            area = AG_L_FLS_CODE;
        }
        /* Check context */
        err = L_FLS_CheckContext_ram(job_id, area, bank);
        if (ER_OK == err)
        {
            /* set FACIACC flag */
            err = L_FLS_SetFACIStatus_ram();
            if (ER_OK == err)
            {
                /* Get flash resources */
                err = L_FLS_GetFlashResource_ram(lock_handle, service_priority, area, bank);
                if (ER_OK == err)
                {
                    err = L_FLS_REQ_EraseFlash_ram(area, bank, offset, size);
                    if (ER_IN_PROCESS == err)
                    {
                        /* process is progress */
                        err = ER_MM_FLS_IN_PROGRESS;
                    }
                    else
                    {
                        /* Release flash resources */
                        (void)L_FLS_FreeFlashResource_ram(area, bank);
                        /* reset FACIACC flag */
                        (void)L_FLS_ClrFACIStatus_ram();
                    }
                }
                else
                {
                    /* reset FACIACC flag */
                    (void)L_FLS_ClrFACIStatus_ram();
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else if (ER_IN_PROCESS == err)
        {
            err = L_FLS_IsFinished_ram((uint16_t)lock_handle, area, bank);
            if (ER_IN_PROCESS == err)
            {
                /* process is progress */
                err = ER_MM_FLS_IN_PROGRESS;
            }
            else
            {
                /* Release flash resources */
                (void)L_FLS_FreeFlashResource_ram(area, bank);
                /* reset FACIACC flag */
                (void)L_FLS_ClrFACIStatus_ram();
            }
        }
        else
        {
            /* Do nothing */
        }
    } /* end of else */

    ret_val = mm_FLS_ConvertionError(err);

    return (ret_val);
}

/* self copy icum_middle_rom.text icum_middle_ram   */
CHANGE_SEC(text, ".icum_middle_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0076]:[ICUMFW_UD_MM_FlashMNG_0076]
* Function name: MM_FLS_WriteUser
* Description  : User Area CF / DF Write
* Arguments    : r_icumif_job_id_t job_id  : job id
*                r_icumif_service_priority_t service_priority : priority
*                uint8_t target_flash      : target flash area (FLS_SELECT_CF or FLS_SELECT_DF)
*                uint8_t target_area       : target area (FLS_SELECT_USER or FLS_SELECT_USER)
*                uint32_t *p_addr          : User Area Flash address
*                uint32_t size             : Write size
*                const uint32_t *p_data    : Pointer to the data to write
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MM_FLS_MEM_DEST
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*****************************************************************************************/
uint32_t MM_FLS_WriteUser(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority, uint8_t target_flash,
                          uint8_t target_area, uint32_t *p_addr, uint32_t size, const uint32_t *p_data)
{
    uint32_t err;
    uint32_t ret_val;
    uint32_t area;
    uint32_t bank;
    uint32_t offset;
    uint32_t lock_handle;

    lock_handle = (uint32_t)job_id;
    err = mm_FLS_CheckParamUser(job_id, target_flash, target_area, (uint32_t)p_addr, size, &bank, &offset);
    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        if ((uint32_t)FLS_SELECT_DF == (uint32_t)target_flash)
        {
            area = AG_L_FLS_DATA;
        }
        else
        {
            area = AG_L_FLS_CODE;
        }
        /* Check context */
        err = L_FLS_CheckContext_ram(job_id, area, bank);
        if (ER_OK == err)
        {
            /* set FACIACC flag */
            err = L_FLS_SetFACIStatus_ram();
            if (ER_OK == err)
            {
                /* Get flash resources */
                err = L_FLS_GetFlashResource_ram(lock_handle, service_priority, area, bank);
                if (ER_OK == err)
                {
                    err = L_FLS_REQ_WriteFlash_ram(area, bank, offset, size, (const uint8_t *)p_data);
                    if (ER_IN_PROCESS == err)
                    {
                        /* process is progress */
                        err = ER_MM_FLS_IN_PROGRESS;
                    }
                    else
                    {
                        /* Release flash resources */
                        (void)L_FLS_FreeFlashResource_ram(area, bank);
                        /* reset FACIACC flag */
                        (void)L_FLS_ClrFACIStatus_ram();
                    }
                }
                else
                {
                    /* reset FACIACC flag */
                    (void)L_FLS_ClrFACIStatus_ram();
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else if (ER_IN_PROCESS == err)
        {
            err = L_FLS_IsFinished_ram((uint16_t)lock_handle, area, bank);
            if (ER_IN_PROCESS == err)
            {
                /* process is progress */
                err = ER_MM_FLS_IN_PROGRESS;
            }
            else
            {
                /* Release flash resources */
                (void)L_FLS_FreeFlashResource_ram(area, bank);
                /* reset FACIACC flag */
                (void)L_FLS_ClrFACIStatus_ram();
            }
        }
        else
        {
            /* Do nothing */
        }
    } /* end of else */

    ret_val = mm_FLS_ConvertionError(err);

    return (ret_val);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_middle.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_middle.rodata")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0077]:[ICUMFW_UD_MM_FlashMNG_0077]
* Function name: MM_FLS_BlankCheckUser
* Description  : User area data flash blank check.
* Arguments    : r_icumif_job_id_t job_id  : job id
*                r_icumif_service_priority_t service_priority : priority
*                uint8_t target_flash      : target flash area (FLS_SELECT_DF only)
*                uint8_t target_area       : target area (FLS_SELECT_USER only)
*                uint32_t *p_addr          : User Area Flash address
*                uint32_t size             : Erase size
*                uint8_t *p_is_blank       : Whether blanks are allowed
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MM_FLS_MEM_DEST
*                ER_MM_FLS_UNAVAILABLE
*                ER_MM_FLS_SYS_CRITICAL
***************************************************************************************/
uint32_t MM_FLS_BlankCheckUser(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                               uint8_t target_flash, uint8_t target_area, uint32_t *p_addr, uint32_t size,
                               uint8_t *p_is_blank)
{
    uint32_t err;
    uint32_t ret_val;
    uint32_t bank;
    uint32_t offset;
    uint32_t result;
    uint32_t lock_handle;

    result = FLS_ZERO_U32;
    lock_handle = (uint32_t)job_id;
    err = mm_FLS_CheckParamUser(job_id, target_flash, target_area, (uint32_t)p_addr, size, &bank, &offset);
    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        if ((uint32_t)FLS_SELECT_DF == (uint32_t)target_flash)
        {
            /* Check context */
            err = L_FLS_CheckContext_ram(job_id, AG_L_FLS_DATA, bank);
            if (ER_OK == err)
            {
                /* set FACIACC flag */
                err = L_FLS_SetFACIStatus_ram();
                if (ER_OK == err)
                {
                    /* Get flash resources */
                    err = L_FLS_GetFlashResource_ram(lock_handle, service_priority, AG_L_FLS_DATA, bank);
                    if (ER_OK == err)
                    {
                        /* request blank check process(Incremental mode) */
                        err = L_FLS_ReadBlankCheckArea(AG_L_FLS_DATA, bank, offset, 
                                        size, L_FLS_BLKCA_INC, &result);
                        if (ER_OK == err)
                        {
                            /* non blank ? */
                            if (ST_L_FLS_NON_BLANK == result)
                            {
                                *p_is_blank = (uint8_t)false;
                            }
                            else
                            {
                                *p_is_blank = (uint8_t)true;
                            }
                        }
                        else
                        {
                            /* error occurred */
                        }
                        /* Release flash resources */
                        (void)L_FLS_FreeFlashResource_ram(AG_L_FLS_DATA, bank);
                        /* reset FACIACC flag */
                        (void)L_FLS_ClrFACIStatus_ram();
                    }
                    else
                    {
                        /* reset FACIACC flag */
                        (void)L_FLS_ClrFACIStatus_ram();
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
        } /* end of if ((uint32_t)FLS_SELECT_DF == (uint32_t)target_flash) */
        else
        {
            /* error occurred */
            err = ER_PARAM;
        }
    } /* end of else */

    /* error conversion */
    ret_val = mm_FLS_ConvertionError(err);

    return (ret_val);
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0024]:[ICUMFW_UD_MM_FlashMNG_0024]
* Function name: mm_FLS_CheckBlankCheckArea
* Description  : Perform a blank check(Incremental mode) with Blank Check Area
*                on the specified area of the bank for ICUM.
* Arguments    : (in) uint32_t offset  : offset from the beginning of data flash area
*                (in) uint32_t size    : size of blank check
*                (out) uint32_t *p_result_status : status of blank check
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
static uint32_t mm_FLS_CheckBlankCheckArea(uint32_t offset, uint32_t size, uint32_t *p_result_status)
{
    uint32_t err;
    uint32_t ret_val;

    /* request blank check process(Incremental mode) */
    err = L_FLS_ReadBlankCheckArea(AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM, offset,
                                   size, L_FLS_BLKCA_INC, p_result_status);

    /* error conversion */
    ret_val = mm_FLS_ConvertionError(err);

    return (ret_val);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0010]:[ICUMFW_UD_MM_FlashMNG_0010]
* Function name: MM_FLS_CollectGarbage
* Description  : 1 Block Garbage Collection
* Arguments    : (in)  r_icumif_job_id_t job_id : job id
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*                ER_MM_FLS_DATA_BLANK
*                ER_MM_FLS_DATA_INVALID
*                ER_MM_FLS_MEM_SOURCE
*                ER_MM_FLS_AES_UNAVAILABLE
*                ER_MM_FLS_AES_TIME_OUT
*                ER_MM_FLS_AES_PRM_ERROR
*                ER_MM_FLS_FULL_DATA_MAP
*                ER_MM_FLS_ALL_BLANK
*                ER_MM_FLS_FORMAT_ERR
*****************************************************************************************/
uint32_t MM_FLS_CollectGarbage(r_icumif_job_id_t job_id)
{
    uint32_t flag_buffer;
    fls_data_u32_pointer_t flag_buffer_pnt;
    uint32_t err;
    uint32_t ret_val;
    uint32_t offset;
    flash_location_t wk_index;
    uint8_t ds_type;
    uint8_t slot_num;
    uint32_t update_counter;
    static uint32_t source_block;
    static uint32_t reserve_block;

    err = ER_OK;
    flag_buffer = FLS_FLAG_VALUE;
    flag_buffer_pnt.p_u32 = &flag_buffer;

    /* check of job id */
    if (FLS_ZERO_U32 == (uint32_t)job_id)
    {
        /* does not occur after the UT/IT process */
        err = ER_PARAM;
    }
    else
    {
        /* Check context */
        err = L_FLS_CheckContext_ram(job_id, AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
    }

    if ((ER_OK == err) || (ER_IN_PROCESS == err))
    {
        switch ((uint32_t)g_phase_GC)
        {
            case FLS_PHASE_0:
                err = mm_FLS_GetGCState(job_id, &source_block, &reserve_block);
                break;
            case FLS_PHASE_1:
                /* Write Transfer flag of GC source block */
                err = mm_FLS_SetTransferFlag(job_id, source_block, reserve_block, flag_buffer_pnt.p_u8);
                break;
            case FLS_PHASE_2:
                /* GC destination block erase */
                err = mm_FLS_EraseGCDest(job_id, reserve_block);
                break;
            case FLS_PHASE_3:
                /* Search data map. Read if there is valid data in GC source block. */
                ret_val = mm_FLS_ReadFblockMapData(source_block,
                                                   &(wk_index.block_idx),
                                                   &ds_type,
                                                   &slot_num, &update_counter);
                if (FLS_FOUND == ret_val)
                {
                    /* there is valid data in GC source block. */
                    wk_index.word_idx = FLS_ZERO_U16;
                    /* Warnings that occur here will be repaired and will not be recorded */
                    g_warning_update_prohibit = FLS_FLAG_ON_U8;
                    err = mm_FLS_AESDSRead(&wk_index, g_read_buffer);
                    g_warning_update_prohibit = FLS_ZERO_U8;
                    if (ER_OK == err)
                    {
                        /* Map data update (from GC source to GC destination) */
                        err = mm_FLS_WriteMapData(ds_type, slot_num, g_write_block_idx[reserve_block], update_counter);
                        if (ER_OK == err)
                        {
                            g_phase_GC = FLS_PHASE_4;
                        }
                        else
                        {
                            /* error occurred */
                            g_phase_GC = FLS_PHASE_9;
                        }
                    }
                    else
                    {
                        /* read data is invalid or error occurred */
                        g_phase_GC = FLS_PHASE_9;
                    }
                }
                else
                {
                    /* there is not valid data in GC source block. this block GC is over */
                    g_phase_GC = FLS_PHASE_6;
                }
                break;
            case FLS_PHASE_4:
                /* GC destination block write */
                err = mm_FLS_WriteGCDest(job_id, reserve_block);
                break;
            case FLS_PHASE_6:
                /* Write NBES flag in GC destination block */
                mm_FLS_ChangeFlagAddr(reserve_block, FLS_DF_NBES, &offset);
                err = mm_FLS_ExecWriteDF(job_id, FLS_FLAG_SIZE, offset, flag_buffer_pnt.p_u8);
                if (ER_OK == err)
                {
                    /* write finished */
                    g_phase_GC = FLS_PHASE_7;
                }
                else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
                {
                    /* do nothing : write process in progress */
                }
                else
                {
                    /* error occurred */
                    g_phase_GC = FLS_PHASE_9;
                }
                break;
            case FLS_PHASE_7:
                /* GC source block erase */
                err = mm_FLS_EraseGCSrc(job_id, source_block);
                break;
            case FLS_PHASE_8:
                /* Write NBEE flag in GC destination block */
                mm_FLS_ChangeFlagAddr(reserve_block, FLS_DF_NBEE, &offset);
                err = mm_FLS_ExecWriteDF(job_id, FLS_FLAG_SIZE, offset, flag_buffer_pnt.p_u8);
                if (ER_OK == err)
                {
                    /* write finished */
                    g_write_block_idx[source_block] = (uint16_t)((uint32_t)mm_FLS_GetLastLBlock(source_block) + 1uL);
                    g_phase_GC = FLS_PHASE_9;
                }
                else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
                {
                    /* do nothing : write process in progress */
                }
                else
                {
                    /* error occurred */
                    g_phase_GC = FLS_PHASE_9;
                }
                break;
            default:
                /* Do nothing */
                break;
        } /* end of switch ((uint32_t)g_phase_GC) */
    }
    else
    {
        /* Do nothing */
    }

    /* Phase of end process */
    mm_FLS_EndPhaseGCWarning(&err, &g_phase_GC);
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0095]:[ICUMFW_UD_MM_FlashMNG_0095]
* Function name: mm_FLS_GetGCState
* Description  : Get flash GC state (for MM_FLS_CollectGarbage)
* Arguments    : (in)  r_icumif_job_id_t job_id  : job id
*                (out) uint32_t* p_source_block  : GC source block number
*                (out) uint32_t* p_reserve_block : GC reserve block number
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MM_FLS_ALL_BLANK
*                ER_MM_FLS_FORMAT_ERR
*                ER_L_FLS_UNAVAILBLE
*                ER_L_FLS_INTERNAL
***************************************************************************************/
static uint32_t mm_FLS_GetGCState(const r_icumif_job_id_t job_id, uint32_t *const p_source_block,
                                  uint32_t *const p_reserve_block)
{
    uint32_t err;
    gc_state_t gc_state;

    /* get flash resource & sets FACIACC flag */
    err = mm_FLS_GetFlashResource(job_id);
    if (ER_OK == err)
    {
        err = mm_FLS_CheckGCFlag(&gc_state, p_source_block, p_reserve_block);
        if (ER_OK != err)
        {
            g_phase_GC = FLS_PHASE_9;
        }
        else
        {
            if (FLS_GC_NORMAL == gc_state)
            {
                /* When the power is not turned off during GC */
                g_phase_GC = FLS_PHASE_1;
            }
            else if (FLS_GC_DURING_1 == gc_state)
            {
                /* When the power is turned off during GC (during data transfer) */
                g_phase_GC = FLS_PHASE_2;
            }
            else if (FLS_GC_DURING_2 == gc_state)
            {
                /* When the power is turned off during GC (during erase) */
                g_phase_GC = FLS_PHASE_7;
            }
            else if (FLS_GC_ALL_BLANK == gc_state)
            {
                /* When the flash memory is not yet formatted */
                err = ER_MM_FLS_ALL_BLANK;
                g_phase_GC = FLS_PHASE_9;
            }
            else
            {
                /* When other flash states */
                err = ER_MM_FLS_FORMAT_ERR;
                g_phase_GC = FLS_PHASE_9;
            }
        }
    }
    else
    {
        g_phase_GC = FLS_PHASE_9;
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0096]:[ICUMFW_UD_MM_FlashMNG_0096]
* Function name: mm_FLS_SetTransferFlag
* Description  : Write Transfer flag of GC source block (for MM_FLS_CollectGarbage)
* Arguments    : (in) r_icumif_job_id_t job_id  : job id
*                (in) uint32_t source_block     : GC source block number
*                (in) uint32_t reserve_block    : GC reserve block number
*                (in) uint8_t* p_flag_buffer    : flag buffer
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
***************************************************************************************/
static uint32_t mm_FLS_SetTransferFlag(r_icumif_job_id_t job_id, const uint32_t source_block,
                                       const uint32_t reserve_block, const uint8_t *p_flag_buffer)
{
    uint32_t err;
    uint32_t offset;

    /* Write Transfer flag of GC source block */
    mm_FLS_ChangeFlagAddr(source_block, FLS_DF_TRANSFER, &offset);
    err = mm_FLS_ExecWriteDF(job_id, FLS_FLAG_SIZE, offset, p_flag_buffer);
    if (ER_OK == err)
    {
        /* write finished */
        /* Check if erasing has been performed since the power was turned on */
        if ((uint32_t)FLS_NOT_ERASED == (uint32_t)g_block_status)
        {
            /* Since erasing has not been performed since the power was turned on, perform the erasing process. */
            g_phase_GC = FLS_PHASE_2;
        }
        else
        {
            /* Since erasing was performed after turning on the power, the erasing process is skipped. */
            g_write_block_idx[reserve_block] = mm_FLS_GetFirstLBlock(reserve_block);
            g_phase_GC = FLS_PHASE_3;
        }
    }
    else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
    {
        /* do nothing : write process in progress */
    }
    else
    {
        /* error occurred */
        g_phase_GC = FLS_PHASE_9;
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0097]:[ICUMFW_UD_MM_FlashMNG_0097]
* Function name: mm_FLS_EraseGCDest (for MM_FLS_CollectGarbage)
* Description  : Erase GC destination block
* Arguments    : (in) r_icumif_job_id_t job_id  : job id
*                (in) uint32_t reserve_block    : GC reserve block number
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
***************************************************************************************/
static uint32_t mm_FLS_EraseGCDest(const r_icumif_job_id_t job_id, const uint32_t reserve_block)
{
    uint32_t err;

    /* GC destination block erase */
    err = mm_FLS_ExecEraseDF(job_id, reserve_block);
    if (ER_OK == err)
    {
        /* erase finished */
        g_block_status = FLS_ERASED;
        g_write_block_idx[reserve_block] = mm_FLS_GetFirstLBlock(reserve_block);
        g_phase_GC = FLS_PHASE_3;
    }
    else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
    {
        /* do nothing : erase process in progress */
    }
    else
    {
        /* error occurred */
        g_phase_GC = FLS_PHASE_9;
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0098]:[ICUMFW_UD_MM_FlashMNG_0098]
* Function name: mm_FLS_WriteGCDest
* Description  : Write data of GC destination block (for MM_FLS_CollectGarbage)
* Arguments    : (in) r_icumif_job_id_t job_id  : job id
*                (in) uint32_t reserve_block    : GC reserve block number
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
***************************************************************************************/
static uint32_t mm_FLS_WriteGCDest(r_icumif_job_id_t job_id, const uint32_t reserve_block)
{
    uint32_t err;
    flash_location_t wk_index;

    /* GC destination block write */
    wk_index.block_idx = g_write_block_idx[reserve_block];
    wk_index.word_idx  = FLS_ZERO_U16;
    err = mm_FLS_DSWrite(job_id, g_data_status_b.data_size, &wk_index, g_read_buffer);
    if (ER_OK == err)
    {
        /* write finished : Search next data */
        g_write_block_idx[reserve_block] =
            (uint16_t)((uint32_t)g_write_block_idx[reserve_block]
                       + (((((uint32_t)g_data_status_b.data_size) + FLS_DF_BLOCK_SIZE) - 1uL) / FLS_DF_BLOCK_SIZE));

        g_phase_GC = FLS_PHASE_3;
    }
    else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
    {
        /* do nothing : write process in progress */
    }
    else
    {
        /* error occurred */
        g_phase_GC = FLS_PHASE_9;
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0099]:[ICUMFW_UD_MM_FlashMNG_0099]
* Function name: mm_FLS_EraseGCSrc
* Description  : Erase GC source block (for MM_FLS_CollectGarbage)
* Arguments    : (in) r_icumif_job_id_t job_id  : job id
*                (in) uint32_t source_block     : GC source block number
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
***************************************************************************************/
static uint32_t mm_FLS_EraseGCSrc(const r_icumif_job_id_t job_id, const uint32_t source_block)
{
    uint32_t err;

    /* GC source block erase */
    err = mm_FLS_ExecEraseDF(job_id, source_block);
    if (ER_OK == err)
    {
        /* erase finished */
        g_phase_GC = FLS_PHASE_8;
    }
    else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
    {
        /* do nothing : erase process in progress */
    }
    else
    {
        /* error occurred */
        g_phase_GC = FLS_PHASE_9;
    }
    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0045]:[ICUMFW_UD_MM_FlashMNG_0045]
* Function name: mm_FLS_CollectGarbageFull
* Description  : All physical block Garbage Collection
* Arguments    : (in)  r_icumif_job_id_t job_id : job id
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*                ER_MM_FLS_DATA_BLANK
*                ER_MM_FLS_DATA_INVALID
*                ER_MM_FLS_MEM_SOURCE
*                ER_MM_FLS_AES_UNAVAILABLE
*                ER_MM_FLS_AES_TIME_OUT
*                ER_MM_FLS_AES_PRM_ERROR
*                ER_MM_FLS_FULL_DATA_MAP
*                ER_MM_FLS_ALL_BLANK
*                ER_MM_FLS_FORMAT_ERR
*****************************************************************************************/
static uint32_t mm_FLS_CollectGarbageFull(r_icumif_job_id_t job_id)
{
    static uint32_t gc_count = FLS_ZERO_U32;
    uint32_t ret_val;

    ret_val = MM_FLS_CollectGarbage(job_id);
    if (ER_OK == ret_val)
    {
        gc_count++;
        if ((FLS_NB_BLOCK - FLS_RESERVE_BLOCK_NUM) > gc_count)
        {
            /* Other than the last physical block GC finished */
            ret_val = ER_MM_FLS_IN_PROGRESS;
        }
        else
        {
            /* All physical block GC finished */
            gc_count = FLS_ZERO_U32;
        }
    }
    else if ((ER_MM_FLS_IN_PROGRESS == ret_val) || (ER_IN_PROCESS == ret_val) || (ER_PENDING == ret_val))
    {
        /* do nothing : GC process in progress */
    }
    else
    {
        /* error occurred */
        gc_count = FLS_ZERO_U32;
    }

    return (ret_val);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0011]:[ICUMFW_UD_MM_FlashMNG_0011]
* Function name: MM_FLS_CollectGarbageAll
* Description  : All physical block Garbage Collection & Defragmentation
* Arguments    : (in)  r_icumif_job_id_t job_id : job id
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*                ER_MM_FLS_DATA_BLANK
*                ER_MM_FLS_DATA_INVALID
*                ER_MM_FLS_MEM_SOURCE
*                ER_MM_FLS_AES_UNAVAILABLE
*                ER_MM_FLS_AES_TIME_OUT
*                ER_MM_FLS_AES_PRM_ERROR
*                ER_MM_FLS_FULL_DATA_MAP
*                ER_MM_FLS_ALL_BLANK
*                ER_MM_FLS_FORMAT_ERR
*****************************************************************************************/
uint32_t MM_FLS_CollectGarbageAll(r_icumif_job_id_t job_id)
{
    uint32_t err;

    /* check of job id */
    if (FLS_ZERO_U32 == (uint32_t)job_id)
    {
        /* does not occur after the UT/IT process */
        err = ER_PARAM;
    }
    else
    {
        /* Check context */
        err = L_FLS_CheckContext_ram(job_id, AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
    }

    if ((ER_OK == err) || (ER_IN_PROCESS == err))
    {
        switch ((uint32_t)g_phase_GCAll)
        {
            case FLS_PHASE_0:
                /* All physical block Garbage Collection (Free up space for defragmentation) */
                err = mm_FLS_CollectGarbageFull(job_id);
                if (ER_OK == err)
                {
                    /* finished */
                    g_phase_GCAll = FLS_PHASE_1;
                }
                else if ((ER_IN_PROCESS == err) || (ER_MM_FLS_IN_PROGRESS == err) || (ER_PENDING == err))
                {
                    /* Do nothing */
                }
                else
                {
                    /* error occurred */
                    g_phase_GCAll = FLS_PHASE_9;
                }
                break;
            case FLS_PHASE_1:
                /* Copy for defragmentation */
                err = mm_FLS_ForwardCopy(job_id);
                if (ER_OK == err)
                {
                    /* finished */
                    g_phase_GCAll = FLS_PHASE_2;
                }
                else if ((ER_IN_PROCESS == err) || (ER_MM_FLS_IN_PROGRESS == err) || (ER_PENDING == err))
                {
                    /* Do nothing */
                }
                else
                {
                    /* error occurred */
                    g_phase_GCAll = FLS_PHASE_9;
                }
                break;
            case FLS_PHASE_2:
                /* All physical block Garbage Collection (Delete defragmentation source data) */
                err = mm_FLS_CollectGarbageFull(job_id);
                if (ER_OK == err)
                {
                    /* finished */
                    g_phase_GCAll = FLS_PHASE_9;
                }
                else if ((ER_IN_PROCESS == err) || (ER_MM_FLS_IN_PROGRESS == err) || (ER_PENDING == err))
                {
                    /* Do nothing */
                }
                else
                {
                    /* error occurred */
                    g_phase_GCAll = FLS_PHASE_9;
                }
                break;
            default:
                /* Do nothing */
                break;
        } /* end of switch ((uint32_t)g_phase_GCAll) */
    } /* end of if ((ER_OK == err) || (ER_IN_PROCESS == err)) */
    else
    {
        /* Do nothing */
    }

    /* Phase of end process */
    if ((uint32_t)FLS_PHASE_9 == (uint32_t)g_phase_GCAll)
    {
        /* Initialization process variables */
        mm_FLS_InitProcessVariable();
        g_phase_GCAll = FLS_PHASE_0;
    }
    else
    {
        /* Returns the process in progress, since it is not in the end process phase */
        if (ER_OK == err)
        {
            err = ER_MM_FLS_IN_PROGRESS;
        }
        else
        {
            /* Do nothing */
        }
    }

    err = mm_FLS_ConvertionError(err);
    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0025]:[ICUMFW_UD_MM_FlashMNG_0025]
* Function name: mm_FLS_CheckGCFlag
* Description  : Check block information area
* Arguments    : (out) fls_state_t* p_fls_state       : GC status of flash memory
*                (out) uint32_t* p_gc_source_block  : GC source block number
*                (out) uint32_t* p_gc_reserve_block : GC reserve block number
* Return Value : ER_OK
*                ER_PARAM
*****************************************************************************************/
static uint32_t mm_FLS_CheckGCFlag(gc_state_t *const p_gc_state, uint32_t *const p_gc_source_block,
                                   uint32_t *const p_gc_reserve_block)
{
    uint32_t err;
    uint32_t fblock;
    uint32_t loop_counter;
    uint32_t counter_ssec;
    uint32_t counter_sses;
    uint32_t counter_scec;
    uint32_t counter_transfer;
    uint32_t num_ssec;
    uint32_t num_scec;
    uint32_t num_transfer;
    uint8_t status;
    fls_block_info_t block_info[FLS_NB_BLOCK];

    counter_ssec = FLS_ZERO_U32;
    counter_sses = FLS_ZERO_U32;
    counter_scec = FLS_ZERO_U32;
    counter_transfer = FLS_ZERO_U32;
    num_ssec = FLS_ZERO_U32;
    num_scec = FLS_ZERO_U32;
    num_transfer = FLS_ZERO_U32;
    *p_gc_state = FLS_GC_INIT_VAL;

    /* Count GC Flag status */
    err = mm_FLS_CountGCFlag(block_info,
                             &counter_ssec,
                             &counter_sses,
                             &counter_scec,
                             &counter_transfer,
                             &num_ssec,
                             &num_scec,
                             &num_transfer);
    /* Determine the flash status from the blank check result of each flag. */
    if (ER_OK != err)
    {
        /* error occurred */
    }
    else
    {
        /* Check if the flag is set when the power is turned off during GC (during erase) : pattern 1 */
        if (FLS_RESERVE_BLOCK_NUM == counter_ssec)
        {
            fblock = num_ssec;
            for (loop_counter = FLS_ZERO_U32;
                 (FLS_NB_BLOCK - (FLS_RESERVE_BLOCK_NUM + FLS_INDEF_BLOCK_NUM)) > loop_counter;
                 loop_counter++)
            {
                fblock = mm_FLS_GetBeforeFBlock(fblock);
                if ((ST_L_FLS_NON_BLANK == block_info[fblock].nbes) && (ST_L_FLS_NON_BLANK == block_info[fblock].nbee))
                {
                    status = FLS_CHK_OK;
                }
                else
                {
                    status = FLS_CHK_NG;
                    break;
                }
            }
            if ((uint32_t)FLS_CHK_OK == (uint32_t)status)
            {
                /* Judged that the power is turned off during GC (during erase) */
                *p_gc_state = FLS_GC_DURING_2;
                *p_gc_reserve_block = num_ssec;
                *p_gc_source_block = mm_FLS_GetAfterFBlock(num_ssec);
            }
            else
            {
                /* do nothing */
            }
        } /* end of if (FLS_RESERVE_BLOCK_NUM == counter_ssec) */
        /* Check if the flag is set when the power is turned off during GC (during erase) : pattern 2 */
        /* Both reserved block and erasing block (undefined) assume NBES set and NBEE clear */
        else if ((FLS_RESERVE_BLOCK_NUM + FLS_INDEF_BLOCK_NUM) == counter_ssec)
        {
            if ((FLS_NB_BLOCK - (FLS_RESERVE_BLOCK_NUM + FLS_INDEF_BLOCK_NUM)) == counter_sses)
            {
                fblock = mm_FLS_GetBeforeFBlock(num_ssec);
                if ((ST_L_FLS_NON_BLANK == block_info[fblock].nbes) && (ST_L_FLS_BLANK == block_info[fblock].nbee))
                {
                    /* Judged that the power is turned off during GC (during erase) */
                    *p_gc_state = FLS_GC_DURING_2;
                    *p_gc_reserve_block = fblock;
                    *p_gc_source_block = num_ssec;
                }
                else
                {
                    fblock = mm_FLS_GetAfterFBlock(num_ssec);
                    if ((ST_L_FLS_NON_BLANK == block_info[fblock].nbes) && (ST_L_FLS_BLANK == block_info[fblock].nbee))
                    {
                        /* Judged that the power is turned off during GC (during erase) */
                        *p_gc_state = FLS_GC_DURING_2;
                        *p_gc_reserve_block = num_ssec;
                        *p_gc_source_block = fblock;
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
            }
            else
            {
                /* do nothing */
            }
        } /* end of else if ((FLS_RESERVE_BLOCK_NUM+FLS_INDEF_BLOCK_NUM) == counter_ssec) */
        else
        {
            /* do nothing */
        }
        if (FLS_GC_INIT_VAL != *p_gc_state)
        {
            /* do nothing : the status has been determined by the above judgments */
        }
        else
        {
            /* Check if the flag is set when the power is turned off during GC (during data transfer) */
            if ((FLS_RESERVE_BLOCK_NUM == counter_transfer) && ((FLS_NB_BLOCK - FLS_RESERVE_BLOCK_NUM) == counter_sses))
            {
                fblock = mm_FLS_GetBeforeFBlock(num_transfer);
                if ((ST_L_FLS_BLANK == block_info[fblock].nbes) && (ST_L_FLS_BLANK == block_info[fblock].nbee)
                    && (ST_L_FLS_BLANK == block_info[fblock].transfer))
                {
                    /* Judged that the power is turned off during GC (during data transfer) */
                    *p_gc_state = FLS_GC_DURING_1;
                    *p_gc_reserve_block = fblock;
                    *p_gc_source_block = num_transfer;
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
        }
        if (FLS_GC_INIT_VAL != *p_gc_state)
        {
            /* do nothing : the status has been determined by the above judgments */
        }
        else
        {
            /* Check the flag status when the power is not turned off during GC */
            if ((FLS_RESERVE_BLOCK_NUM == counter_scec) && ((FLS_NB_BLOCK - FLS_RESERVE_BLOCK_NUM) == counter_sses))
            {
                /* Judged that the power was turned off except during GC */
                *p_gc_state = FLS_GC_NORMAL;
                *p_gc_reserve_block = num_scec;
                *p_gc_source_block = mm_FLS_GetAfterFBlock(num_scec);
            }
            else if ((FLS_NB_BLOCK == counter_scec) && (FLS_ZERO_U32 == counter_transfer))
            {
                /* Judged as all block blanks */
                *p_gc_state = FLS_GC_ALL_BLANK;
                *p_gc_reserve_block = FLS_ZERO_U32;
                *p_gc_source_block = FLS_ZERO_U32;
            }
            else
            {
                /* Judged as unformatted except for known status */
                *p_gc_state = FLS_GC_FORMAT_ERR;
                *p_gc_reserve_block = FLS_ZERO_U32;
                *p_gc_source_block = FLS_ZERO_U32;
            }
        }
    } /* end of else */
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0100]:[ICUMFW_UD_MM_FlashMNG_0100]
* Function name: mm_FLS_CountGCFlag
* Description  : count GC Flag status
* Arguments    : (out) fls_block_info_t block_info[]    : GC flag status of each physical block
*                (out) uint32_t* p_counter_ssec         : Counter of blocks in NBES set and NBEE clear
*                (out) uint32_t* p_counter_sses         : Counter of blocks in NBES set and NBEE set
*                (out) uint32_t* p_counter_scec         : Counter of blocks in NBES clear and NBEE clear
*                (out) uint32_t* p_counter_transfer     : Counter of blocks in transfer set
*                (out) uint32_t* p_num_ssec             : Physical block number in which NBES is set and NBEE is clear
*                (out) uint32_t* p_num_scec             : Physical block number in which NBES is clear and NBEE is clear
*                (out) uint32_t* p_num_transfer         : Physical block number in which transfer is set
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
static uint32_t mm_FLS_CountGCFlag(fls_block_info_t block_info[], uint32_t *const p_counter_ssec,
                                   uint32_t *const p_counter_sses, uint32_t *const p_counter_scec,
                                   uint32_t *const p_counter_transfer, uint32_t *const p_num_ssec,
                                   uint32_t *const p_num_scec, uint32_t *const p_num_transfer)
{
    uint32_t err;
    uint32_t fblock;
    uint32_t offset;

    /* Check the blank status of the block information area of all physical blocks */
    for (fblock = FLS_ZERO_U32; fblock < FLS_NB_BLOCK; fblock++)
    {
        /* NBEE flag area blank check */
        mm_FLS_ChangeFlagAddr(fblock, FLS_DF_NBEE, &offset);
        err = mm_FLS_CheckBlankCheckArea(offset, FLS_FLAG_SIZE, &(block_info[fblock].nbee));
        if (ER_OK != err)
        {
            /* error occurred */
        }
        else
        {
            /* NBES flag area blank check */
            mm_FLS_ChangeFlagAddr(fblock, FLS_DF_NBES, &offset);
            err = mm_FLS_CheckBlankCheckArea(offset, FLS_FLAG_SIZE, &(block_info[fblock].nbes));
            if (ER_OK != err)
            {
                /* error occurred */
            }
            else
            {
                /* transfer flag area blank check */
                mm_FLS_ChangeFlagAddr(fblock, FLS_DF_TRANSFER, &offset);
                err = mm_FLS_CheckBlankCheckArea(offset, FLS_FLAG_SIZE, &(block_info[fblock].transfer));
                if (ER_OK != err)
                {
                    /* error occurred */
                }
                else
                {
                    /* Save non-blank physical block count and physical block number */
                    if (ST_L_FLS_NON_BLANK == block_info[fblock].nbes)
                    {
                        if (ST_L_FLS_NON_BLANK == block_info[fblock].nbee)
                        {
                            /* Counter of blocks in NBES set and NBEE set */
                            (*p_counter_sses)++;
                        }
                        else
                        {
                            /* Counter of blocks in NBES set and NBEE clear */
                            (*p_counter_ssec)++;
                            *p_num_ssec = fblock;
                        }
                    }
                    else
                    {
                        if (ST_L_FLS_NON_BLANK == block_info[fblock].nbee)
                        {
                            /* do nothing */
                        }
                        else
                        {
                            /* Counter of blocks in NBES clear and NBEE clear */
                            (*p_counter_scec)++;
                            *p_num_scec = fblock;
                        }
                    }
                    if (ST_L_FLS_NON_BLANK == block_info[fblock].transfer)
                    {
                        /* Counter of blocks in transfer set */
                        (*p_counter_transfer)++;
                        *p_num_transfer = fblock;
                    }
                    else
                    {
                        /* do nothing */
                    }
                } /* end of else */
            } /* end of else */
        } /* end of else */
        if (ER_OK != err)
        {
            /* error occurred : loop out */
            break;
        }
        else
        {
            /* do nothing */
        }
    } /* end of for (fblock = FLS_ZERO_U32; fblock < FLS_NB_BLOCK; fblock++) */
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0026]:[ICUMFW_UD_MM_FlashMNG_0026]
* Function name: mm_FLS_GetAfterFBlock
* Description  : Get next physical block number
* Arguments    : (in) uint32_t block_num    : physical block number
* Return Value : uint32_t : next physical block number
***************************************************************************************/
static uint32_t mm_FLS_GetAfterFBlock(const uint32_t block_num)
{
    uint32_t ret_val;

    if ((FLS_NB_BLOCK - 1uL) <= block_num)
    {
        ret_val = FLS_ZERO_U32;
    }
    else
    {
        ret_val = block_num + 1uL;
    }
    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0027]:[ICUMFW_UD_MM_FlashMNG_0027]
* Function name: mm_FLS_GetBeforeFBlock
* Description  : Get the previous physical block number
* Arguments    : (in) uint32_t block_num    : physical block number
* Return Value : uint32_t : previous physical block number
***************************************************************************************/
static uint32_t mm_FLS_GetBeforeFBlock(const uint32_t block_num)
{
    uint32_t ret_val;

    if (FLS_ZERO_U32 < block_num)
    {
        ret_val = block_num - 1uL;
    }
    else
    {
        ret_val = FLS_NB_BLOCK - 1uL;
    }
    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0028]:[ICUMFW_UD_MM_FlashMNG_0028]
* Function name: mm_FLS_DSWrite
* Description  : Data writing and TAG writing on both sides
* Arguments    : (in) r_icumif_job_id_t job_id  : job id
*                (in) uint16_t size             : size of write
*                (in) flash_location_t *p_index : specified block index of data set
*                (in) uint8_t *p_data           : A pointer to the memory that stores the obtained write data
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
***************************************************************************************/
static uint32_t mm_FLS_DSWrite(r_icumif_job_id_t job_id, const uint16_t size, const flash_location_t *const p_index,
                               uint8_t *const p_data)
{
    uint32_t err;
    uint32_t offset;
    uint32_t tag_buffer;
    fls_data_u32_pointer_t tag_buffer_pnt;
    static flash_location_t wk_block_index;
    static fls_data_u32_pointer_t write_buffer_pnt;
    flash_location_t end_block_index;

    err = ER_OK;
    tag_buffer = PROGRAMMING_TAG;
    tag_buffer_pnt.p_u32 = &tag_buffer;

    /* Write end block index calculation */
    end_block_index.block_idx =
        (uint16_t)((uint32_t)p_index->block_idx + (((((uint32_t)size) + FLS_DF_BLOCK_SIZE) - 1uL) / FLS_DF_BLOCK_SIZE));

    switch ((uint32_t)g_phase_DsWrite)
    {
        case FLS_PHASE_0:
            /* Variable initialization */
            wk_block_index = *p_index;
            write_buffer_pnt.p_u8 = p_data;
            err = ER_MM_FLS_IN_PROGRESS;
            g_phase_DsWrite = FLS_PHASE_1;
            break;
        case FLS_PHASE_1:
            /* Write side-B one logical block (64 bytes) & wait for completion */
            mm_FLS_ChangeAddr(FLS_SIDE_B, wk_block_index, &offset);
            err = mm_FLS_ExecWriteDF(job_id, FLS_DF_BLOCK_SIZE, offset, write_buffer_pnt.p_u8);
            if (ER_OK == err)
            {
                /* write finished */
                g_phase_DsWrite = FLS_PHASE_2;
            }
            else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
            {
                /* do nothing : write process in progress */
            }
            else
            {
                /* error occurred */
                g_phase_DsWrite = FLS_PHASE_9;
            }
            break;
        case FLS_PHASE_2:
            /* Write side-B one TAG & wait for completion */
            mm_FLS_ChangeTagAddr(FLS_SIDE_B, wk_block_index, &offset);
            err = mm_FLS_ExecWriteDF(job_id, FLS_TAG_SIZE, offset, tag_buffer_pnt.p_u8);
            if (ER_OK == err)
            {
                /* write finished */
                wk_block_index.block_idx = (uint16_t)((uint32_t)wk_block_index.block_idx + 1uL);
                /* Check if it is the last logical block */
                if ((uint32_t)wk_block_index.block_idx >= (uint32_t)end_block_index.block_idx)
                {
                    g_phase_DsWrite = FLS_PHASE_3;
                    wk_block_index = *p_index;
                    write_buffer_pnt.p_u8 = p_data;
                }
                else
                {
                    g_phase_DsWrite = FLS_PHASE_1;
                    write_buffer_pnt.u32 = write_buffer_pnt.u32 + FLS_DF_BLOCK_SIZE;
                }
            }
            else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
            {
                /* do nothing : write process in progress */
            }
            else
            {
                /* error occurred */
                g_phase_DsWrite = FLS_PHASE_9;
            }
            break;
        case FLS_PHASE_3:
            /* Write side-A one logical block (64 bytes) & wait for completion */
            mm_FLS_ChangeAddr(FLS_SIDE_A, wk_block_index, &offset);
            err = mm_FLS_ExecWriteDF(job_id, FLS_DF_BLOCK_SIZE, offset, write_buffer_pnt.p_u8);
            if (ER_OK == err)
            {
                /* write finished */
                g_phase_DsWrite = FLS_PHASE_4;
            }
            else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
            {
                /* do nothing : write process in progress */
            }
            else
            {
                /* error occurred */
                g_phase_DsWrite = FLS_PHASE_9;
            }
            break;
        case FLS_PHASE_4:
            /* Write side-A one TAG & wait for completion */
            mm_FLS_ChangeTagAddr(FLS_SIDE_A, wk_block_index, &offset);
            err = mm_FLS_ExecWriteDF(job_id, FLS_TAG_SIZE, offset, tag_buffer_pnt.p_u8);
            if (ER_OK == err)
            {
                /* write finished */
                wk_block_index.block_idx = (uint16_t)((uint32_t)wk_block_index.block_idx + 1uL);
                /* Check if it is the last logical block */
                if ((uint32_t)wk_block_index.block_idx >= (uint32_t)end_block_index.block_idx)
                {
                    g_phase_DsWrite = FLS_PHASE_9;
                }
                else
                {
                    g_phase_DsWrite = FLS_PHASE_3;
                    write_buffer_pnt.u32 = write_buffer_pnt.u32 + FLS_DF_BLOCK_SIZE;
                }
            }
            else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
            {
                /* do nothing : write process in progress */
            }
            else
            {
                /* error occurred */
                g_phase_DsWrite = FLS_PHASE_9;
            }
            break;
        default:
            /* Do nothing */
            break;
    } /* end of switch ((uint32_t)g_phase_DsWrite) */

    /* end process */
    mm_FLS_EndPhaseInternal(&err, &g_phase_DsWrite);
    return (err);
}

/***************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0029]:[ICUMFW_UD_MM_FlashMNG_0029]
* Function name : mm_FLS_ChangeTagAddr
* Description   : Converting target side and block index into a TAG address
* Arguments     :  (in) uint16_t target_side            : target side
*               :  (in) flash_location_t flash_location : data slot index
*               :  (out) uint32_t *p_addr               : TAG address
* Return Value  :  none
***************************************************************************************/
static void mm_FLS_ChangeTagAddr(const uint16_t target_side, const flash_location_t flash_location,
                                 uint32_t *const p_addr)
{
    if ((uint32_t)FLS_SIDE_A == (uint32_t)target_side)
    {
        /* Side-A */
        *p_addr = ((((uint32_t)flash_location.block_idx) / FLS_LBLOCK_NUM_PER_PBLOCK) * FLS_ERASE_BLOCK_SIZE)
                  + FLS_TOTAL_BLOCK_INFO_SIZE
                  + (((uint32_t)flash_location.block_idx % FLS_LBLOCK_NUM_PER_PBLOCK) * FLS_TAG_SIZE);
    }
    else
    {
        /* Side-B */
        *p_addr = ((((uint32_t)flash_location.block_idx) / FLS_LBLOCK_NUM_PER_PBLOCK) * FLS_ERASE_BLOCK_SIZE)
                  + FLS_TOTAL_BLOCK_INFO_SIZE + (FLS_TAG_SIZE * FLS_LBLOCK_NUM_PER_PBLOCK)
                  + (((uint32_t)flash_location.block_idx % FLS_LBLOCK_NUM_PER_PBLOCK) * FLS_TAG_SIZE);
    }
}

/***************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0030]:[ICUMFW_UD_MM_FlashMNG_0030]
* Function name : mm_FLS_ChangeFlagAddr
* Description   : Converting physical block number and flag type into a flag address
* Arguments     : (in) uint32_t block_num   : physical block number
*               : (in) df_flag_t flag_type  : flag type
*               : (out) uint32_t *p_addr    : flag address
* Return Value  : none
***************************************************************************************/
static void mm_FLS_ChangeFlagAddr(const uint32_t block_num, const df_flag_t flag_type, uint32_t *const p_addr)
{
    if (FLS_DF_NBEE == flag_type)
    {
        *p_addr = (block_num * FLS_ERASE_BLOCK_SIZE);
    }
    else if (FLS_DF_NBES == flag_type)
    {
        *p_addr = (block_num * FLS_ERASE_BLOCK_SIZE) + (FLS_DF_NBES * FLS_FLAG_SIZE);
    }
    else if (FLS_DF_TRANSFER == flag_type)
    {
        *p_addr = (block_num * FLS_ERASE_BLOCK_SIZE) + (FLS_DF_TRANSFER * FLS_FLAG_SIZE);
    }
    else
    {
        *p_addr = (block_num * FLS_ERASE_BLOCK_SIZE) + (FLS_DF_USE * FLS_FLAG_SIZE);
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0031]:[ICUMFW_UD_MM_FlashMNG_0031]
* Function name: mm_FLS_ExecEraseDF
* Description  : Perform the block erase for ICU-M DF
* Arguments    : (in) r_icumif_job_id_t job_id  : job id
*                (in) uint32_t block_num        : physical block number
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
***************************************************************************************/
static uint32_t mm_FLS_ExecEraseDF(const r_icumif_job_id_t job_id, const uint32_t block_num)
{
    uint32_t err;
    uint32_t lul_offset;
    uint32_t lock_handle;

    err = ER_OK;
    lock_handle = (uint32_t)job_id;
    switch ((uint32_t)g_phase_ExErase)
    {
        /* erase operation acceptance phase */
        case FLS_PHASE_0:
            /* convert index to address */
            mm_FLS_ChangeFlagAddr(block_num, FLS_DF_BLOCK_TOP, &lul_offset);

            /* perform the flash erasing */
            err = L_FLS_REQ_EraseFlash_ram(AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM, lul_offset, FLS_ERASE_BLOCK_SIZE);

            if (ER_IN_PROCESS == err)
            {
                /* change next phase (wait for erase operation completion phase) */
                g_phase_ExErase = FLS_PHASE_1;
            }
            else
            {
                /* error occurred */
                g_phase_ExErase = FLS_PHASE_9;
            }
            break;

        /* wait for erase operation completion phase */
        case FLS_PHASE_1:
            err = L_FLS_IsFinished_ram((uint16_t)lock_handle, AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
            if (ER_OK == err)
            {
                /* Erasing process is completed */
                g_phase_ExErase = FLS_PHASE_9;
            }
            else if (ER_IN_PROCESS == err)
            {
                /* process is progress */
                /* do nothing          */
            }
            else
            {
                /* error occurred */
                g_phase_ExErase = FLS_PHASE_9;
            }
            break;

        default:
            /* Not occurred */
            break;
    }

    /* end process */
    mm_FLS_EndPhaseInternal(&err, &g_phase_ExErase);
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0032]:[ICUMFW_UD_MM_FlashMNG_0032]
* Function name: mm_FLS_GetFirstLBlock
* Description  : Get the first logical block index in a physical block
* Arguments    : (in) uint32_t block_num    : physical block number
* Return Value : flash_block_idx_t : first logical block index
***************************************************************************************/
static flash_block_idx_t mm_FLS_GetFirstLBlock(const uint32_t block_num)
{
    const flash_block_idx_t ret_block_idx = (uint16_t)(block_num * FLS_LBLOCK_NUM_PER_PBLOCK);
    return (ret_block_idx);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0033]:[ICUMFW_UD_MM_FlashMNG_0033]
* Function name: mm_FLS_GetLastLBlock
* Description  : Get the last logical block index in a physical block
* Arguments    : (in) uint32_t block_num    : physical block number
* Return Value : flash_block_idx_t : last logical block index
***************************************************************************************/
static flash_block_idx_t mm_FLS_GetLastLBlock(const uint32_t block_num)
{
    const flash_block_idx_t ret_block_idx = (uint16_t)(((block_num + 1uL) * FLS_LBLOCK_NUM_PER_PBLOCK) - 1uL);
    return (ret_block_idx);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0044]:[ICUMFW_UD_MM_FlashMNG_0044]
* Function name: mm_FLS_GetFBlock
* Description  : Get the physical block number where the logical block is placed
* Arguments    : (in) flash_block_idx_t block_idx   : logical block index
* Return Value : uint32_t : physical block number
***************************************************************************************/
static uint32_t mm_FLS_GetFBlock(const flash_block_idx_t block_idx)
{
    const uint32_t ret_block_num = (uint32_t)block_idx / FLS_LBLOCK_NUM_PER_PBLOCK;
    return (ret_block_num);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0008]:[ICUMFW_UD_MM_FlashMNG_0008]
* Function name: MM_FLS_MapDataSet
* Description  : Secure flash memory mount (data map creation)
* Arguments    : none
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MM_FLS_ALL_BLANK
*                ER_MM_FLS_FORMAT_ERR
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_DATA_INVALID (ER_L_FLS_ECC2EDF1)
*                ER_MM_FLS_MEM_SOURCE
*                ER_MM_FLS_FULL_DATA_MAP
***************************************************************************************/
uint32_t MM_FLS_MapDataSet(void)
{
    uint32_t err;
    uint32_t ret_val;
    uint32_t fblock;
    uint32_t counter;
    gc_state_t gc_state;
    uint32_t source_block;
    uint32_t reserve_block;
    uint32_t nocheck_block;
    gc_warning_t gc_warning;
    uint32_t lock_handle;

    source_block   = FLS_ZERO_U32;
    reserve_block  = FLS_ZERO_U32;
    lock_handle = (uint32_t)FLS_DUMMY_CTX_ID;

    /* Initialize all data map with invalid data */
    for (counter = FLS_ZERO_U32; FLS_DATA_MAP_SIZE > counter; counter++)
    {
        g_data_map[counter].ds_type         = FLS_INVALID_DS_TYPE;
        g_data_map[counter].slot_num        = FLS_INVALID_SLOT_NUM;
        g_data_map[counter].block_idx       = FLS_ZERO_U16;
        g_data_map[counter].update_counter  = FLS_ZERO_U16;
    }

    /* All write pointers are initialized with non-writable values */
    for (fblock = FLS_ZERO_U32; fblock < FLS_NB_BLOCK; fblock++)
    {
        g_write_block_idx[fblock] = (uint16_t)((uint32_t)mm_FLS_GetLastLBlock(fblock) + 1uL);
    }

    /* Get flash resources */
    err = L_FLS_GetFlashResource_ram(lock_handle, PRIORITY_HIGHEST, AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);

    if (ER_OK == err)
    {
        /* Check block information area */
        err = mm_FLS_CheckGCFlag(&gc_state, &source_block, &reserve_block);
        if (ER_OK != err)
        {
            /* do nothing */
        }
        else
        {
            if ((uint32_t)FLS_GC_NORMAL == (uint32_t)gc_state)
            {
                /* When the power is not turned off during GC */
                nocheck_block = reserve_block;
            }
            else if ((uint32_t)FLS_GC_DURING_1 == (uint32_t)gc_state)
            {
                /* When the power is turned off during GC (during data transfer) */
                nocheck_block = reserve_block;
            }
            else if ((uint32_t)FLS_GC_DURING_2 == (uint32_t)gc_state)
            {
                /* When the power is turned off during GC (during erase) */
                nocheck_block = source_block;
            }
            else if ((uint32_t)FLS_GC_ALL_BLANK == (uint32_t)gc_state)
            {
                /* When the flash memory is not yet formatted */
                err = ER_MM_FLS_ALL_BLANK;
            }
            else
            {
                /* When other flash states */
                err = ER_MM_FLS_FORMAT_ERR;
            }
            if (((uint32_t)FLS_GC_NORMAL != (uint32_t)gc_state)
                && ((uint32_t)FLS_GC_DURING_1 != (uint32_t)gc_state)
                && ((uint32_t)FLS_GC_DURING_2 != (uint32_t)gc_state))
            {
                /* Since it is unformatted, no data map is created. */
            }
            else
            {
                /* All block flash memory mount */
                mm_FLS_CheckAllFlash(nocheck_block, &err);
            }
            /* Release flash resources */
            (void)L_FLS_FreeFlashResource_ram(AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
        } /* end of else */
    } /* end of if (ER_OK == err) */
    else
    {
        /* get resource error, do nothing */
    }

    if ((ER_MM_FLS_ALL_BLANK != err) && (ER_MM_FLS_FORMAT_ERR != err))
    {
        /* Calculate GC necessity from remaining flash capacity */
        gc_warning = mm_FLS_GetCapacityStatus();
        if (((uint32_t)NVM_DATA_WARNING == (uint32_t)g_nvm_data_warning)
            && ((uint32_t)FLS_NO_FULL_WARNING == (uint32_t)gc_warning))
        {
            /* do nothing */
        }
        else
        {
            g_nvm_data_warning = gc_warning;
        }
    }
    else
    {
        /* Do nothing */
    }
    ret_val = mm_FLS_ConvertionError(err);
    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0101]:[ICUMFW_UD_MM_FlashMNG_0101]
* Function name: mm_FLS_CheckAllFlash
* Description  : All block flash memory mount
* Arguments    : (in) uint32_t nocheck_block    : Physical block number not checked
*                (in/out) uint32_t* p_err       : Error code
* Return Value : none
***************************************************************************************/
static void mm_FLS_CheckAllFlash(const uint32_t nocheck_block, uint32_t *const p_err)
{
    uint32_t fblock;
    flash_location_t wk_block_index;
    flash_location_t fblock_end_index;

    /* Physical block loop */
    for (fblock = FLS_ZERO_U32; fblock < FLS_NB_BLOCK; fblock++)
    {
        if (fblock == nocheck_block)
        {
            /* nocheck_block is not subject to data map creation : go to next physical block */
        }
        else
        {
            wk_block_index.block_idx = mm_FLS_GetFirstLBlock(fblock);
            wk_block_index.word_idx = FLS_ZERO_U16;
            g_write_block_idx[fblock] = wk_block_index.block_idx;
            fblock_end_index.block_idx = mm_FLS_GetLastLBlock(fblock);
            /* Logical block loop */
            while ((uint32_t)wk_block_index.block_idx <= (uint32_t)fblock_end_index.block_idx)
            {
                *p_err = mm_FLS_Set1MapData(&wk_block_index);
                if (ER_OK != *p_err)
                {
                    break;
                }
                else
                {
                    /* do nothing */
                }
            }
            if (ER_OK != *p_err)
            {
                break;
            }
            else
            {
                /* do nothing */
            }
        } /* end of else */
    } /* end of for (fblock = FLS_ZERO_U32; fblock < FLS_NB_BLOCK; fblock++) */
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0035]:[ICUMFW_UD_MM_FlashMNG_0035]
* Function name: mm_FLS_Set1MapData
* Description  : 1 data processing of flash memory mount
* Arguments    : (in) flash_location_t *p_flash_location    : specified block index of data set
* Return Value : ER_OK
*                ER_PARAM
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_DATA_INVALID (ER_L_FLS_ECC2EDF1)
*                ER_MM_FLS_MEM_SOURCE
*                ER_MM_FLS_FULL_DATA_MAP
***************************************************************************************/
static uint32_t mm_FLS_Set1MapData(flash_location_t *const p_flash_location)
{
    uint32_t err;
    uint32_t fblock;
    uint32_t offset;
    uint32_t result_status;
    uint16_t check_block_num;
    uint8_t next_process;
    uint8_t tag_result;

    result_status = FLS_ZERO_U32;
    /* Get the physical block number where the logical block is placed */
    fblock = mm_FLS_GetFBlock(p_flash_location->block_idx);

    /* Side-A (later writing side) TAG area check */
    err = mm_FLS_CheckTag(FLS_SIDE_A, p_flash_location, &tag_result);
    if ((uint32_t)FLS_TAG_BLANK == (uint32_t)tag_result)
    {
        /* Since side-A TAG area is blank, it is determined as an unwritten area */
        /* Perform a blank check on the side-B (first writing side)  */
        /* Considering power turn off during side-B writing */
        mm_FLS_ChangeAddr(FLS_SIDE_B, *p_flash_location, &offset);
        err = mm_FLS_CheckBlankCheckArea(offset, WORD_SIZE, &result_status);
        if (ER_OK != err)
        {
            /* error occurred */
            next_process = FLS_READ_ONLY_NEXT_PBLOCK;
        }
        else
        {
            if (ST_L_FLS_BLANK == result_status)
            {
                /* In case of blank on the side-B next area, it is judged as blank from here */
                next_process = FLS_NEXT_PBLOCK;
            }
            else
            {
                /* Since side-B is written, set non-writable. : go to next physical block */
                next_process = FLS_READ_ONLY_NEXT_PBLOCK;
            }
        }
    }
    else if ((uint32_t)FLS_TAG_VALID == (uint32_t)tag_result)
    {
        /* When the TAG area is non-blank and the value is valid */
        err = mm_FLS_CheckHeader(p_flash_location, &next_process, &check_block_num);
    }
    else if ((uint32_t)FLS_TAG_INVALID == (uint32_t)tag_result)
    {
        /* When the TAG area is non-blank and the value is invalid */
        next_process = FLS_READ_ONLY_NEXT_PBLOCK;
    }
    else
    {
        /* error occurred */
        next_process = FLS_READ_ONLY_NEXT_PBLOCK;
    }

    /* Change the next processing pointer and write pointer according to the judgment result */
    if ((uint32_t)FLS_READ_ONLY_NEXT_PBLOCK == (uint32_t)next_process)
    {
        /* go to next physical block & set this physical block read-only */
        p_flash_location->block_idx = (uint16_t)((uint32_t)mm_FLS_GetLastLBlock(fblock) + 1uL);
        g_write_block_idx[fblock] = p_flash_location->block_idx;
    }
    else if ((uint32_t)FLS_NEXT_PBLOCK == (uint32_t)next_process)
    {
        /* go to next physical block */
        p_flash_location->block_idx = (uint16_t)((uint32_t)mm_FLS_GetLastLBlock(fblock) + 1uL);
    }
    else
    {
        /* go to next data (FLS_NEXT_DATA) */
        p_flash_location->block_idx = (uint16_t)((uint32_t)p_flash_location->block_idx + (uint32_t)check_block_num);
        g_write_block_idx[fblock] = p_flash_location->block_idx;
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0036]:[ICUMFW_UD_MM_FlashMNG_0036]
* Function name: mm_FLS_CheckHeader
* Description  : Data area header check
* Arguments    : (in) flash_location_t *p_flash_location    : specified block index of data set
*                (out) uint8_t* p_next_process              : result information specifying next processing
*                (out) uint16_t* p_check_block_num          : Number of logical blocks checked
* Return Value : ER_OK
*                ER_PARAM
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_DATA_INVALID (ER_L_FLS_ECC2EDF1)
*                ER_MM_FLS_MEM_SOURCE
*                ER_MM_FLS_FULL_DATA_MAP
***************************************************************************************/
static uint32_t mm_FLS_CheckHeader(const flash_location_t *const p_flash_location, uint8_t *const p_next_process,
                                   uint16_t *const p_check_block_num)
{
    uint32_t err;
    uint32_t ret_val;
    uint32_t offset;
    uint8_t result;
    uint8_t cmp_result;
    fls_data_area_header_t data_buffer;
    fls_data_area_pointer_t data_buffer_pnt;
    uint16_t data_map_num;

    data_map_num = FLS_ZERO_U16;
    data_buffer_pnt.p_header = &data_buffer;

    /* data area read (header only) */
    mm_FLS_ChangeAddr(FLS_SIDE_A, *p_flash_location, &offset);
    err = mm_FLS_ExecReadDF(AG_L_FLS_BANK_ICUM, offset, sizeof(fls_data_area_header_t), data_buffer_pnt.p_u8);
    if (ER_OK != err)
    {
        /* error occurred */
        *p_next_process = FLS_READ_ONLY_NEXT_PBLOCK;
    }
    else
    {
        /* Header information check */
        mm_FLS_CheckDataSetMapping(data_buffer.ds_type, &result);
        if ((uint32_t)FLS_CHK_NG == (uint32_t)result)
        {
            /* Since header check ng, set non-writable. : go to next physical block */
            *p_next_process = FLS_READ_ONLY_NEXT_PBLOCK;
        }
        else
        {
            /* Check data size is appropriate */
            ret_val = mm_FLS_PreCheck(
                FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE, data_buffer.ds_type, data_buffer.data_size, FLS_OP_READ);
            if (ER_OK == ret_val)
            {
                /* Check if data is in physical block */
                *p_check_block_num =
                    (uint16_t)(((((uint32_t)data_buffer.data_size) + FLS_DF_BLOCK_SIZE) - 1uL) / FLS_DF_BLOCK_SIZE);
                if (((uint32_t)p_flash_location->block_idx / FLS_LBLOCK_NUM_PER_PBLOCK)
                    == ((((uint32_t)p_flash_location->block_idx + (uint32_t)(*p_check_block_num)) - 1uL)
                        / FLS_LBLOCK_NUM_PER_PBLOCK))
                {
                    /* Check if it exists in the existing data map */
                    ret_val = mm_FLS_CheckMapData(data_buffer.ds_type, data_buffer.slot_num, &data_map_num);
                    if (FLS_FOUND == ret_val)
                    {
                        /* Existing in existing data map */
                        /* Compare the read update counter with the update counter on the data map */
                        cmp_result = mm_FLS_CompareValue(data_buffer.update_counter,
                                                         g_data_map[data_map_num].update_counter,
                                                         (FLS_FULL_U32 - (FLS_NB_BLOCK * FLS_LBLOCK_NUM_PER_PBLOCK)));
                        if ((uint32_t)FLS_CMP_SMALL != (uint32_t)cmp_result)
                        {
                            /* Since the read update counter is newer than the update counter on the data map, the data map is updated. */
                            g_data_map[data_map_num].block_idx = p_flash_location->block_idx;
                            g_data_map[data_map_num].update_counter = data_buffer.update_counter;
                            *p_next_process = FLS_NEXT_DATA;
                        }
                        else
                        {
                            /* Since the read update counter is older than the update counter on the data map, the data map is not updated. */
                            *p_next_process = FLS_NEXT_DATA;
                        }
                    }
                    else
                    {
                        /* Does not exist in existing data map */
                        /* Overwrite invalid data because it is new data */
                        ret_val = mm_FLS_CheckMapData(FLS_INVALID_DS_TYPE, FLS_INVALID_SLOT_NUM, &data_map_num);
                        if (FLS_FOUND == ret_val)
                        {
                            /* Invalid data found : overwrite data */
                            g_data_map[data_map_num].ds_type = data_buffer.ds_type;
                            g_data_map[data_map_num].slot_num = data_buffer.slot_num;
                            g_data_map[data_map_num].block_idx = p_flash_location->block_idx;
                            g_data_map[data_map_num].update_counter = data_buffer.update_counter;
                            *p_next_process = FLS_NEXT_DATA;
                        }
                        else
                        {
                            /* Invalid data not found : data map is full */
                            err = ER_MM_FLS_FULL_DATA_MAP;
                            *p_next_process = FLS_READ_ONLY_NEXT_PBLOCK;
                        }
                    }
                } /* end of if (((uint32_t)FLS_ZERO_U16 != (uint32_t)(*p_check_block_num)) && 
                               (((uint32_t)p_flash_location->block_idx / FLS_LBLOCK_NUM_PER_PBLOCK) ==
                              ((((uint32_t)p_flash_location->block_idx + (uint32_t)(*p_check_block_num)) - 1uL) / FLS_LBLOCK_NUM_PER_PBLOCK))) */
                else
                {
                    /* size is 0 or data does not fit in physical block : size invalid */
                    *p_next_process = FLS_READ_ONLY_NEXT_PBLOCK;
                }
            } /* end of if (ER_OK == ret_val) */
            else
            {
                /* data size or data set type is invalid */
                *p_next_process = FLS_READ_ONLY_NEXT_PBLOCK;
            }
        } /* end of else */
    } /* end of else */

    return (err);
}

/***************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0037]:[ICUMFW_UD_MM_FlashMNG_0037]
* Function name : mm_FLS_CheckTag
* Description   : TAG area check (1 logical block)
* Arguments     : (in) uint16_t target_side         : target side
*               : (in) flash_location_t *p_index    : specified block index of data set
*               : (out) uint8_t *p_result_status    : TAG status
* Return Value : ER_OK
*                ER_PARAM
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_DATA_INVALID (ER_L_FLS_ECC2EDF1)
*                ER_MM_FLS_MEM_SOURCE
***************************************************************************************/
static uint32_t mm_FLS_CheckTag(const uint16_t target_side, const flash_location_t *const p_index,
                                uint8_t *const p_result_status)
{
    uint32_t err;
    uint32_t offset;
    uint32_t result;
    uint32_t tag_buffer;
    fls_data_u32_pointer_t tag_buffer_pnt;

    result = FLS_ZERO_U32;
    tag_buffer = FLS_ZERO_U32;
    tag_buffer_pnt.p_u32 = &tag_buffer;

    /* Blank check Data TAG */
    mm_FLS_ChangeTagAddr(target_side, *p_index, &offset);
    /* Blank Check processing execution */
    err = mm_FLS_CheckBlankCheckArea(offset, sizeof(program_tag_t), &result);
    if (ER_OK != err)
    {
        /* error occurred */
        *p_result_status = FLS_TAG_ERROR;
    }
    else
    {
        if (ST_L_FLS_BLANK == result)
        {
            /* Data TAG is blank */
            *p_result_status = FLS_TAG_BLANK;
        }
        else
        {
            /* Data TAG is non-blank : Check the contents of Data TAG */
            err = mm_FLS_ExecReadDF(AG_L_FLS_BANK_ICUM, offset, sizeof(program_tag_t), tag_buffer_pnt.p_u8);
            if (ER_OK != err)
            {
                /* error occurred */
                *p_result_status = FLS_TAG_ERROR;
            }
            else if (tag_buffer != PROGRAMMING_TAG)
            {
                /* the Data TAG area is non-blank and the value is invalid */
                *p_result_status = FLS_TAG_INVALID;
            }
            else
            {
                /* the Data TAG area is non-blank and the value is valid */
                *p_result_status = FLS_TAG_VALID;
            }
        }
    }
    return (err);
}

/***************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0034]:[ICUMFW_UD_MM_FlashMNG_0034]
* Function name : mm_FLS_CheckMapData
* Description   : Search in data map
* Arguments     : (in) uint8_t ds_type          : type of data-set
*               : (in) uint8_t slot_num         : slot number
*               : (out) uint16_t* p_match_num   : Data number when there is a match
* Return Value  : FLS_FOUND
*                 FLS_NOT_FOUND
***************************************************************************************/
static uint32_t mm_FLS_CheckMapData(const uint8_t ds_type, const uint8_t slot_num, uint16_t *const p_match_num)
{
    uint32_t loop_counter;
    uint32_t ret_val;

    for (loop_counter = FLS_ZERO_U32; loop_counter < FLS_DATA_MAP_SIZE; loop_counter++)
    {
        /* Check if ds_type and slot_num match */
        if (((uint32_t)ds_type == (uint32_t)g_data_map[loop_counter].ds_type)
            && ((uint32_t)slot_num == (uint32_t)g_data_map[loop_counter].slot_num))
        {
            *p_match_num = (uint16_t)loop_counter;
            break;
        }
        else
        {
            /* do nothing */
        }
    }
    if (loop_counter >= FLS_DATA_MAP_SIZE)
    {
        ret_val = FLS_NOT_FOUND;
    }
    else
    {
        ret_val = FLS_FOUND;
    }

    return (ret_val);
}

/***************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0038]:[ICUMFW_UD_MM_FlashMNG_0038]
* Function name : mm_FLS_WriteMapData
* Description   : Write data to data map
* Arguments     : (in) uint8_t ds_type              : write data (type of data-set)
*               : (in) uint8_t slot_num             : write data (slot number)
*               : (in) flash_block_idx_t block_idx  : write data (logical block index)
*               : (in) uint32_t update_counter      : write data (update counter)
* Return Value  : ER_OK
*                 ER_MM_FLS_FULL_DATA_MAP
***************************************************************************************/
static uint32_t mm_FLS_WriteMapData(const uint8_t ds_type, const uint8_t slot_num, const flash_block_idx_t block_idx,
                                    const uint32_t update_counter)
{
    uint32_t ret_val;
    uint16_t data_map_num;

    data_map_num = FLS_ZERO_U16;
    /* Update data if there is existing data with specified ds_type and slot_num */
    ret_val = mm_FLS_CheckMapData(ds_type, slot_num, &data_map_num);
    if (FLS_FOUND == ret_val)
    {
        g_data_map[data_map_num].ds_type        = ds_type;
        g_data_map[data_map_num].slot_num       = slot_num;
        g_data_map[data_map_num].block_idx      = block_idx;
        g_data_map[data_map_num].update_counter = update_counter;
        ret_val = ER_OK;
    }
    else
    {
        /* If the specified ds_type and slot_num do not exist in the existing data, */
        /* a new one is added (invalid data is overwritten) */
        ret_val = mm_FLS_CheckMapData(FLS_INVALID_DS_TYPE, FLS_INVALID_SLOT_NUM, &data_map_num);
        if (FLS_FOUND == ret_val)
        {
            g_data_map[data_map_num].ds_type        = ds_type;
            g_data_map[data_map_num].slot_num       = slot_num;
            g_data_map[data_map_num].block_idx      = block_idx;
            g_data_map[data_map_num].update_counter = update_counter;
            ret_val = ER_OK;
        }
        else
        {
            /* Writing failed because there was no free space in the data map */
            ret_val = ER_MM_FLS_FULL_DATA_MAP;
        }
    }

    return (ret_val);
}

/***************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0039]:[ICUMFW_UD_MM_FlashMNG_0039]
* Function name : mm_FLS_ReadMapData
* Description   : Read data from data map
* Arguments     : (in) uint8_t ds_type                      : type of data-set
*               : (in) uint8_t slot_num                     : slot number
*               : (out) flash_location_t* p_flash_location  : read data (logical block index)
*               : (out) uint32_t* p_update_counter          : read data (update counter)
* Return Value  : ER_OK
*                 ER_MM_FLS_DATA_BLANK
***************************************************************************************/
static uint32_t mm_FLS_ReadMapData(const uint8_t ds_type, const uint8_t slot_num,
                                   flash_location_t *const p_flash_location, uint32_t *const p_update_counter)
{
    uint32_t ret_val;
    uint16_t data_map_num;

    data_map_num = FLS_ZERO_U16;
    /* If the specified ds_type and slot_num exists in the existing data, the block index and update counter are read. */
    ret_val = mm_FLS_CheckMapData(ds_type, slot_num, &data_map_num);
    if (FLS_FOUND == ret_val)
    {
        p_flash_location->block_idx = g_data_map[data_map_num].block_idx;
        p_flash_location->word_idx  = FLS_ZERO_U16;
        *p_update_counter           = g_data_map[data_map_num].update_counter;
        ret_val = ER_OK;
    }
    else
    {
        /* The specified ds_type and slot_num do not exist in the existing data */
        ret_val = ER_MM_FLS_DATA_BLANK;
    }

    return (ret_val);
}

/***************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0009]:[ICUMFW_UD_MM_FlashMNG_0009]
* Function name : MM_FLS_DeleteMapData
* Description   : Delete data in data map
* Arguments     : (in) uint8_t ds_type      : type of data-set
*               : (in) uint8_t slot_num     : slot number
* Return Value  : ER_OK
***************************************************************************************/
uint32_t MM_FLS_DeleteMapData(uint8_t ds_type, uint8_t slot_num)
{
    uint32_t ret_val;
    uint16_t data_map_num;

    data_map_num = FLS_ZERO_U16;
    /* If the specified ds_type and slot_num exist in existing data, overwritten with invalid data. */
    ret_val = mm_FLS_CheckMapData(ds_type, slot_num, &data_map_num);
    if (FLS_FOUND == ret_val)
    {
        g_data_map[data_map_num].ds_type        = FLS_INVALID_DS_TYPE;
        g_data_map[data_map_num].slot_num       = FLS_INVALID_SLOT_NUM;
        g_data_map[data_map_num].block_idx      = FLS_ZERO_U16;
        g_data_map[data_map_num].update_counter = FLS_ZERO_U16;
    }
    else
    {
        /* The specified ds_type and slot_num do not exist in the existing data */
        /* do nothing */
    }

    return (ER_OK);
}

/***************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0040]:[ICUMFW_UD_MM_FlashMNG_0040]
* Function name : mm_FLS_ReadFblockMapData
* Description   : Read the data stored in the specified physical block in the data map
* Arguments     : (in) uint32_t fblock                  : physical block number
*               : (out) flash_block_idx_t* p_block_idx  : read data (logical block index)
*               : (out) uint8_t* p_ds_type              : read data (type of data-set)
*               : (out) uint8_t* p_slot_num             : read data (slot number)
*               : (out) uint32_t* p_update_counter      : read data (update counter)
* Return Value  : FLS_FOUND
*                 FLS_NOT_FOUND
***************************************************************************************/
static uint32_t mm_FLS_ReadFblockMapData(const uint32_t fblock, flash_block_idx_t *const p_block_idx,
                                         uint8_t *const p_ds_type, uint8_t *const p_slot_num,
                                         uint32_t *const p_update_counter)
{
    uint32_t loop_counter;
    uint32_t ret_val = FLS_NOT_FOUND;
    flash_block_idx_t first_block_idx;
    flash_block_idx_t last_block_idx;

    /* Calculate the first and last logical blocks of the specified physical block */
    first_block_idx = mm_FLS_GetFirstLBlock(fblock);
    last_block_idx = mm_FLS_GetLastLBlock(fblock);
    for (loop_counter = FLS_ZERO_U32; loop_counter < FLS_DATA_MAP_SIZE; loop_counter++)
    {
        /* Check if there is data stored from the first logical block to the last logical block in the data map */
        if (((uint32_t)FLS_INVALID_DS_TYPE != (uint32_t)g_data_map[loop_counter].ds_type)
            && ((uint32_t)FLS_INVALID_SLOT_NUM != (uint32_t)g_data_map[loop_counter].slot_num)
            && ((uint32_t)first_block_idx <= (uint32_t)g_data_map[loop_counter].block_idx)
            && ((uint32_t)last_block_idx >= (uint32_t)g_data_map[loop_counter].block_idx))
        {
            /* Read if data exists */
            *p_block_idx        = g_data_map[loop_counter].block_idx;
            *p_ds_type          = g_data_map[loop_counter].ds_type;
            *p_slot_num         = g_data_map[loop_counter].slot_num;
            *p_update_counter   = g_data_map[loop_counter].update_counter;
            ret_val = FLS_FOUND;
            break;
        }
        else
        {
            /* do nothing */
        }
    }

    return (ret_val);
}

/***************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0046]:[ICUMFW_UD_MM_FlashMNG_0046]
* Function name : mm_FLS_GetLastMapData
* Description   : Find the last data on SDF before the limit index
* Arguments     : (in) flash_block_idx_t limit_index    : limit block index
*               : (in) uint32_t reserve_block           : reserve block number
*               : (out) uint32_t* p_last_num            : last data number (g_data_map)
* Return Value  : FLS_FOUND
*                 FLS_NOT_FOUND
***************************************************************************************/
static uint32_t mm_FLS_GetLastMapData(const flash_block_idx_t limit_index, const uint32_t reserve_block,
                                      uint32_t *const p_last_num)
{
    uint32_t loop_counter;
    uint32_t last_num;
    uint32_t ret_val;
    uint32_t compare_result;
    flash_block_idx_t first_block_idx;
    flash_block_idx_t last_block_idx;
    flash_block_idx_t current_block_idx;
    uint32_t first_fblock;

    last_num        = FLS_ZERO_U32;
    ret_val         = FLS_NOT_FOUND;
    first_fblock    = mm_FLS_GetAfterFBlock(reserve_block);
    first_block_idx = mm_FLS_GetFirstLBlock(first_fblock);
    last_block_idx  = first_block_idx;

    /* Check all the data in the data map */
    for (loop_counter = FLS_ZERO_U32; loop_counter < FLS_DATA_MAP_SIZE; loop_counter++)
    {
        if (((uint32_t)FLS_INVALID_DS_TYPE != (uint32_t)g_data_map[loop_counter].ds_type)
            && ((uint32_t)FLS_INVALID_SLOT_NUM != (uint32_t)g_data_map[loop_counter].slot_num))
        {
            /* for valid data : compare with limit block index */
            current_block_idx = g_data_map[loop_counter].block_idx;
            compare_result = mm_FLS_CompareBlock(current_block_idx, limit_index, first_block_idx);
            if ((uint32_t)FLS_CMP_SMALL == compare_result)
            {
                /* current_block_idx is less than limit block index */
                compare_result = mm_FLS_CompareBlock(current_block_idx, last_block_idx, first_block_idx);
                if ((uint32_t)FLS_CMP_SMALL == compare_result)
                {
                    /* current_block_idx is less previous maximum index : do nothing */
                }
                else
                {
                    /* current_block_idx is greater than or equal to previous maximum index : overwrite */
                    last_block_idx = current_block_idx;
                    last_num = loop_counter;
                    ret_val = FLS_FOUND;
                }
            }
            else
            {
                /* current_block_idx is greater than or equal to limit block index : do nothing */
            }
        }
        else
        {
            /* for invalid data : do nothing */
        }
    }
    *p_last_num = last_num;

    return (ret_val);
}

/***************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0043]:[ICUMFW_UD_MM_FlashMNG_0043]
* Function name : mm_FLS_GetCapacityStatus
* Description   : Calculate the GC warning status from the number of free logical blocks
* Arguments     : none
* Return Value  : FLS_NO_FULL_WARNING
*                 FLS_FULL_WARNING1
*                 FLS_FULL_WARNING2
***************************************************************************************/
static gc_warning_t mm_FLS_GetCapacityStatus(void)
{
    uint32_t fblock;
    uint32_t free_blocks;
    gc_warning_t ret_val;

    free_blocks = FLS_ZERO_U32;
    /* Calculate the total number of free logical blocks */
    for (fblock = FLS_ZERO_U32; fblock < FLS_NB_BLOCK; fblock++)
    {
        free_blocks += (((uint32_t)mm_FLS_GetLastLBlock(fblock) + 1uL) - (uint32_t)g_write_block_idx[fblock]);
    }
    /* Compare the total number of free logical blocks with a threshold */
    if ((uint32_t)g_fls_cfg.warning2 >= free_blocks)
    {
        ret_val = FLS_FULL_WARNING2;
    }
    else if ((uint32_t)g_fls_cfg.warning1 >= free_blocks)
    {
        ret_val = FLS_FULL_WARNING1;
    }
    else
    {
        ret_val = FLS_NO_FULL_WARNING;
    }

    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0041]:[ICUMFW_UD_MM_FlashMNG_0041]
* Function name: mm_FLS_CompareValue
* Description  : FLS Compare Value (with wrap-around measures).
* Arguments    : value1: Compare target value 1
*                value2: Compare target value 2
*                max_value: If the difference between value1 and value2 is greater than this value,
*                           it is judged that wrap-around occurs
* Return Value : FLS_CMP_SMALL : value1 < value2
*                FLS_CMP_EQUAL : value1 = value2
*                FLS_CMP_LARGE : value1 > value2
***************************************************************************************/
static uint8_t mm_FLS_CompareValue(const uint32_t value1, const uint32_t value2, const uint32_t max_value)
{
    uint8_t ret_val;

    if (value1 > value2)
    {
        if ((value1 - value2) > max_value)
        {
            ret_val = FLS_CMP_SMALL;
        }
        else
        {
            ret_val = FLS_CMP_LARGE;
        }
    }
    else if (value1 < value2)
    {
        if ((value2 - value1) > max_value)
        {
            ret_val = FLS_CMP_LARGE;
        }
        else
        {
            ret_val = FLS_CMP_SMALL;
        }
    }
    else
    {
        ret_val = FLS_CMP_EQUAL;
    }
    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0042]:[ICUMFW_UD_MM_FlashMNG_0042]
* Function name: mm_FLS_IncValue
* Description  : Increment value (with wrap-around measures).
* Arguments    : value: Increment target value
*                max_value: If value is greater than or equal to max_value,
*                           value is set 0.
* Return Value : incremented value
***************************************************************************************/
static uint32_t mm_FLS_IncValue(const uint32_t value, const uint32_t max_value)
{
    uint32_t ret_val;

    if (value >= max_value)
    {
        ret_val = 0uL;
    }
    else
    {
        ret_val = value + 1uL;
    }

    return (ret_val);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0012]:[ICUMFW_UD_MM_FlashMNG_0012]
* Function name: MM_FLS_Format
* Description  : set secure DF to the initial state
* Arguments    : (in) r_icumif_job_id_t job_id : job id
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL,ER_L_FLS_FACI_TIME_OUT)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*****************************************************************************************/
uint32_t MM_FLS_Format(r_icumif_job_id_t job_id)
{
    uint32_t err;
    uint32_t offset;
    uint32_t wk_block;
    uint32_t counter;
    uint32_t flag_buffer;
    fls_data_u32_pointer_t flag_buffer_pnt;

    flag_buffer = FLS_FLAG_VALUE;
    flag_buffer_pnt.p_u32 = &flag_buffer;

    /* check of job id */
    if (FLS_ZERO_U32 == (uint32_t)job_id)
    {
        /* does not occur after the UT/IT process */
        err = ER_PARAM;
    }
    else
    {
        /* Check context */
        err = L_FLS_CheckContext_ram(job_id, AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
    }

    if ((ER_OK == err) || (ER_IN_PROCESS == err))
    {
        switch ((uint32_t)g_phase_Format)
        {
            case FLS_PHASE_0:
                /* Get resource & first physical block erase start */
                err = mm_FLS_EraseFormatFBlkSt(job_id);
                break;
            case FLS_PHASE_1:
                /* Waiting for first physical block erase to complete */
                err = mm_FLS_EraseFormatFBlkComp(job_id);
                break;
            case FLS_PHASE_2:
                /* Write the NBES flag of the physical block before the erase block */
                wk_block = mm_FLS_GetBeforeFBlock(g_format_block);
                mm_FLS_ChangeFlagAddr(wk_block, FLS_DF_NBES, &offset);
                err = mm_FLS_ExecWriteDF(job_id, FLS_FLAG_SIZE, offset, flag_buffer_pnt.p_u8);
                if (ER_OK == err)
                {
                    /* write successful */
                    g_phase_Format = FLS_PHASE_3;
                }
                else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
                {
                    /* do nothing : write process in progress */
                }
                else
                {
                    /* error occurred */
                    g_phase_Format = FLS_PHASE_9;
                }
                break;
            case FLS_PHASE_3:
                /* physical block erase */
                err = mm_FLS_EraseFormatGBlk(job_id);
                break;
            case FLS_PHASE_4:
                /* Write the NBEE flag of the physical block after the erase block */
                wk_block = mm_FLS_GetBeforeFBlock(g_format_block);
                mm_FLS_ChangeFlagAddr(wk_block, FLS_DF_NBEE, &offset);
                err = mm_FLS_ExecWriteDF(job_id, FLS_FLAG_SIZE, offset, flag_buffer_pnt.p_u8);
                if (ER_OK == err)
                {
                    /* write successful */
                    if (g_format_block >= (FLS_NB_BLOCK - FLS_RESERVE_BLOCK_NUM))
                    {
                        /* all physical block erased */
                        g_block_status = FLS_ERASED;

                        /* Initialize all data map with invalid data */
                        for (counter = FLS_ZERO_U32; FLS_DATA_MAP_SIZE > counter; counter++)
                        {
                            g_data_map[counter].ds_type         = FLS_INVALID_DS_TYPE;
                            g_data_map[counter].slot_num        = FLS_INVALID_SLOT_NUM;
                            g_data_map[counter].block_idx       = FLS_ZERO_U16;
                            g_data_map[counter].update_counter  = FLS_ZERO_U16;
                        }

                        /* Other than reserve block: initialize the write pointer to the first logical block of the physical block */
                        for (wk_block = FLS_ZERO_U32; (FLS_NB_BLOCK - FLS_RESERVE_BLOCK_NUM) > wk_block; wk_block++)
                        {
                            g_write_block_idx[wk_block] = mm_FLS_GetFirstLBlock(wk_block);
                        }
                        /* reserve block: initialize the write pointer with non-writable values */
                        g_write_block_idx[wk_block] = (uint16_t)((uint32_t)mm_FLS_GetLastLBlock(wk_block) + 1uL);
                        g_phase_Format = FLS_PHASE_9;
                    }
                    else
                    {
                        g_format_block = mm_FLS_GetAfterFBlock(g_format_block);
                        g_phase_Format = FLS_PHASE_2;
                    }
                }
                else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
                {
                    /* do nothing : write process in progress */
                }
                else
                {
                    /* error occurred */
                    g_phase_Format = FLS_PHASE_9;
                }
                break;
            default:
                /* Not occurred */
                break;
        } /* end of switch ((uint32_t)g_phase_Format) */
    } /* end of if ((ER_OK == err) || (ER_IN_PROCESS == err)) */
    else
    {
        /* Do nothing */
    }

    /* Phase of end process */
    mm_FLS_EndPhaseGCWarning(&err, &g_phase_Format);
    return (err);
}

/***************************************************************************************
* Function name: MM_FLS_GET_Format
***************************************************************************************/
uint8_t MM_FLS_GET_FormatPhase(void)
{
    return g_phase_Format;
}
/***************************************************************************************
* Function name: MM_FLS_GET_Format
***************************************************************************************/
uint8_t MM_FLS_GET_WritePhase(void)
{
    return g_phase_Write;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0102]:[ICUMFW_UD_MM_FlashMNG_0102]
* Function name: mm_FLS_EraseFormatFBlkSt
* Description  : Erase start first physical block (for MM_FLS_Format)
* Arguments    : (in) r_icumif_job_id_t job_id : job id
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
*                ER_L_FLS_UNAVAILBLE
*                ER_L_FLS_INTERNAL
***************************************************************************************/
static uint32_t mm_FLS_EraseFormatFBlkSt(const r_icumif_job_id_t job_id)
{
    uint32_t err;

    /* Get resource & first physical block erase start */
    err = mm_FLS_GetFlashResource(job_id);
    if (ER_OK == err)
    {
        err = mm_FLS_ExecEraseDF(job_id, g_format_block);
        if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
        {
            /* erase process in progress */
            g_phase_Format = FLS_PHASE_1;
        }
        else
        {
            /* error occurred */
            g_phase_Format = FLS_PHASE_9;
        }
    }
    else
    {
        g_phase_Format = FLS_PHASE_9;
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0103]:[ICUMFW_UD_MM_FlashMNG_0103]
* Function name: mm_FLS_EraseFormatFBlkComp
* Description  : Erase complete first physical block (for MM_FLS_Format)
* Arguments    : (in) r_icumif_job_id_t job_id : job id
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
***************************************************************************************/
static uint32_t mm_FLS_EraseFormatFBlkComp(const r_icumif_job_id_t job_id)
{
    uint32_t err;

    /* Waiting for first physical block erase to complete */
    err = mm_FLS_ExecEraseDF(job_id, g_format_block);
    if (ER_OK == err)
    {
        /* erase successful */
        g_format_block = mm_FLS_GetAfterFBlock(g_format_block);
        g_phase_Format = FLS_PHASE_2;
    }
    else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
    {
        /* do nothing : erase process in progress */
    }
    else
    {
        /* error occurred */
        g_phase_Format = FLS_PHASE_9;
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0104]:[ICUMFW_UD_MM_FlashMNG_0104]
* Function name: mm_FLS_EraseFormatGBlk
* Description  : Erase general physical block (for MM_FLS_Format)
* Arguments    : (in) r_icumif_job_id_t job_id : job id
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
***************************************************************************************/
static uint32_t mm_FLS_EraseFormatGBlk(const r_icumif_job_id_t job_id)
{
    uint32_t err;

    /* physical block erase */
    err = mm_FLS_ExecEraseDF(job_id, g_format_block);
    if (ER_OK == err)
    {
        /* erase successful */
        g_phase_Format = FLS_PHASE_4;
    }
    else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
    {
        /* do nothing : erase process in progress */
    }
    else
    {
        /* error occurred */
        g_phase_Format = FLS_PHASE_9;
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0013]:[ICUMFW_UD_MM_FlashMNG_0013]
* Function name: MM_FLS_Config
* Description  : Flash manager configuration
* Arguments    : none
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MM_FLS_Config(void)
{
    uint32_t err;
    fls_cfg_pointer_t cfg_pnt;

    cfg_pnt.p_cfg_fls = &g_fls_cfg;
    /* GC warning threshold */
    err = ML_CFG_GetSys(ML_CFG_SYS_FLS, cfg_pnt.p_u32);

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0047]:[ICUMFW_UD_MM_FlashMNG_0047]
* Function name: mm_FLS_ForwardCopy
* Description  : Copy data to the front block as much as possible
* Arguments    : (in) r_icumif_job_id_t job_id : job id
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_IN_PROCESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_AES_UNAVAILABLE
*                ER_MM_FLS_AES_TIME_OUT
*                ER_MM_FLS_DATA_BLANK
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_UNAVAILABLE
*                ER_MM_FLS_AES_PRM_ERROR
*                ER_MM_FLS_DATA_INVALID
*                ER_MM_FLS_MEM_SOURCE
*                ER_MM_FLS_FULL_DATA_MAP
***************************************************************************************/
static uint32_t mm_FLS_ForwardCopy(r_icumif_job_id_t job_id)
{
    static uint32_t reserve_block;
    static flash_block_idx_t checked_block_idx;
    static flash_block_idx_t destination_block_idx;
    fls_data_area_pointer_t data_buffer_pnt;
    uint32_t err = ER_OK;
    uint32_t ret_val;
    uint32_t fblock;
    uint32_t first_fblock;
    flash_block_idx_t first_block_idx;
    flash_location_t wk_index;
    uint32_t source_num;
ALIGNVAR(4)
    dvc_calculate_t dvc_param;

    err = ER_OK;
    data_buffer_pnt.p_u8 = g_read_buffer;

    switch ((uint32_t)g_phase_fcopy)
    {
        case FLS_PHASE_0:
            /* get reserve_block */
            err = mm_FLS_GetFCReserveBlk(job_id, &reserve_block, &checked_block_idx);
            break;
        case FLS_PHASE_1:
            /* Read the copy source to find the copy destination */
            /* Find the last data before the limit index */
            ret_val = mm_FLS_GetLastMapData(checked_block_idx, reserve_block, &source_num);
            if (FLS_NOT_FOUND == ret_val)
            {
                /* not found : successful completion (completion of processing up to the first data) */
                g_phase_fcopy = FLS_PHASE_9;
                break;
            }
            else
            {
                /* found : go next process */
            }

            /* Check if the found data is in the first physical block */
            wk_index.block_idx = g_data_map[source_num].block_idx;
            wk_index.word_idx = FLS_ZERO_U16;
            fblock = mm_FLS_GetFBlock(wk_index.block_idx);
            first_fblock = mm_FLS_GetAfterFBlock(reserve_block);
            if (fblock == first_fblock)
            {
                /* The found data is inside the first physical block : successful completion */
                /* If the last data is in the first physical block, there is no movable data */
                g_phase_fcopy = FLS_PHASE_9;
                break;
            }
            else
            {
                /* The found data is outside the first physical block : go next process */
            }

            /* Read source data */
            checked_block_idx = wk_index.block_idx;

            /* Warnings that occur here will be repaired and will not be recorded */
            g_warning_update_prohibit = FLS_FLAG_ON_U8;
            err = mm_FLS_AESDSRead(&wk_index, data_buffer_pnt.p_u8);
            g_warning_update_prohibit = FLS_ZERO_U8;
            if (ER_OK == err)
            {
                /* In case of successful source data reading : Destination block search */
                ret_val = mm_FLS_GetWritableArea(reserve_block,
                                                 data_buffer_pnt.p_header->data_size,
                                                 &destination_block_idx);
                if (FLS_FOUND == ret_val)
                {
                    /* found destination block */
                    fblock = mm_FLS_GetAfterFBlock(reserve_block);
                    first_block_idx = mm_FLS_GetFirstLBlock(fblock);
                    ret_val = mm_FLS_CompareBlock(wk_index.block_idx, destination_block_idx, first_block_idx);
                    if ((uint32_t)FLS_CMP_LARGE == ret_val)
                    {
                        /* When copying is possible to a block before the copy source */
                        /* increment the update counter */
                        data_buffer_pnt.p_header->update_counter =
                            mm_FLS_IncValue(data_buffer_pnt.p_header->update_counter, FLS_FULL_U32);

                        /* DVC calculation of write buffer data */
                        dvc_param.p_input = (const uint8_t *)&(data_buffer_pnt.p_header->ds_type);
                        dvc_param.data_size =
                            (uint16_t)(((uint32_t)(data_buffer_pnt.p_header->data_size)) - (uint32_t)sizeof(dvc_t));
                        dvc_param.p_output = &(data_buffer_pnt.p_header->dvc);
                        /* DVC calculation with AES resource processing */
                        err = mm_FLS_CalculateDVC(&dvc_param, &g_phase_fcopy);
                        if (ER_OK == err)
                        {
                            /* Map data update (from copy source to copy destination) */
                            err = mm_FLS_WriteMapData(g_data_map[source_num].ds_type,
                                                      g_data_map[source_num].slot_num,
                                                      destination_block_idx,
                                                      data_buffer_pnt.p_header->update_counter);
                            if (ER_OK == err)
                            {
                                g_phase_fcopy = FLS_PHASE_2;
                            }
                            else
                            {
                                /* error occurred */
                                g_phase_fcopy = FLS_PHASE_9;
                            }
                        }
                        else
                        {
                            /* error occurred */
                        }
                    } /* end of if ((uint32_t)FLS_CMP_LARGE == ret_val) */
                    else
                    {
                        /* When copying to a block before the copy source is not possible : */
                        /* Since this data cannot be moved, look for the next data */
                    }
                } /* end of if (FLS_FOUND == ret_val) */
                else
                {
                    /* not found destination block : Since this data cannot be moved, look for the next data */
                }
            } /* end of if (ER_OK == err) */
            else
            {
                /* read data is invalid or error occurred */
                g_phase_fcopy = FLS_PHASE_9;
            }
            break;
        case FLS_PHASE_2:
            /* copy destination block write */
            err = mm_FLS_WriteFCDestBlk(job_id, destination_block_idx, data_buffer_pnt);
            break;
        default:
            /* Do Nothing */
            break;
    } /* end of switch ((uint32_t)g_phase_fcopy) */

    /* Phase of end process */
    mm_FLS_EndPhaseFC(&err, &g_phase_fcopy);
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0105]:[ICUMFW_UD_MM_FlashMNG_0105]
* Function name: mm_FLS_GetFCReserveBlk
* Description  : get reserve_block (for mm_FLS_ForwardCopy)
* Arguments    : (in)  r_icumif_job_id_t job_id     : job id
*                (out) uint32_t* p_reserve_block    : reserve block number
*                (out) flash_block_idx_t* p_checked_block_idx : Checked block index
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_L_FLS_UNAVAILBLE
*                ER_L_FLS_INTERNAL
***************************************************************************************/
static uint32_t mm_FLS_GetFCReserveBlk(const r_icumif_job_id_t job_id, uint32_t *const p_reserve_block,
                                       flash_block_idx_t *const p_checked_block_idx)
{
    uint32_t err;
    uint32_t fblock;
    gc_state_t gc_state;

    /* get flash resource & sets FACIACC flag */
    err = mm_FLS_GetFlashResource(job_id);
    if (ER_OK == err)
    {
        /* get reserve_block */
        err = mm_FLS_CheckGCFlag(&gc_state, &fblock, p_reserve_block);
        if (ER_OK != err)
        {
            g_phase_fcopy = FLS_PHASE_9;
        }
        else
        {
            /* initialize limit index */
            *p_checked_block_idx = FLS_INVALID_BLOCK_IDX;
            g_phase_fcopy = FLS_PHASE_1;
        }
    }
    else
    {
        g_phase_fcopy = FLS_PHASE_9;
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0106]:[ICUMFW_UD_MM_FlashMNG_0106]
* Function name: mm_FLS_WriteFCDestBlk
* Description  : Write destination block (for mm_FLS_ForwardCopy)
* Arguments    : (in) r_icumif_job_id_t job_id                  : job id
*                (in) flash_block_idx_t destination_block_idx   : destination block index
*                (in) fls_data_area_pointer_t data_buffer_pnt   : data buffer pointer
* Return Value : ER_OK
*                ER_PARAM
*                ER_IN_PROCESS
*                ER_MM_FLS_IN_PROGRESS
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_SYS_CRITICAL (ER_L_FLS_INTERNAL)
*                ER_MM_FLS_PROG         (ER_L_FLS_PROGRAM)
*                ER_MM_FLS_UNAVAILABLE  (ER_L_FLS_UNAVAILABLE)
***************************************************************************************/
static uint32_t mm_FLS_WriteFCDestBlk(r_icumif_job_id_t job_id, const flash_block_idx_t destination_block_idx,
                                      const fls_data_area_pointer_t data_buffer_pnt)
{
    uint32_t err;
    uint32_t fblock;
    flash_location_t wk_index;

    /* copy destination block write */
    wk_index.block_idx = destination_block_idx;
    wk_index.word_idx = FLS_ZERO_U16;
    err = mm_FLS_DSWrite(job_id, data_buffer_pnt.p_header->data_size, &wk_index, data_buffer_pnt.p_u8);
    if (ER_OK == err)
    {
        /* write finished : write pointer add */
        fblock = mm_FLS_GetFBlock(wk_index.block_idx);
        g_write_block_idx[fblock] = (uint16_t)(
            (uint32_t)g_write_block_idx[fblock]
            + (((((uint32_t)data_buffer_pnt.p_header->data_size) + FLS_DF_BLOCK_SIZE) - 1uL) / FLS_DF_BLOCK_SIZE));

        g_phase_fcopy = FLS_PHASE_1;
    }
    else if ((ER_MM_FLS_IN_PROGRESS == err) || (ER_IN_PROCESS == err))
    {
        /* do nothing : write process in progress */
    }
    else
    {
        /* error occurred */
        g_phase_fcopy = FLS_PHASE_9;
    }
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0107]:[ICUMFW_UD_MM_FlashMNG_0107]
* Function name: mm_FLS_CalculateDVC
* Description  : DVC calculation with AES resource processing
* Arguments    : (in)  dvc_calculate_t* p_dvc_param : DVC parameters
*                (out) uint8_t* p_phase             : Processing phase
* Return Value  : ER_OK
*                 ER_PARAM
*                 ER_PENDING
*                 ER_MM_FLS_AES_UNAVAILABLE
*                 ER_MM_FLS_AES_TIME_OUT
*                 ER_MM_FLS_AES_PRM_ERROR
***************************************************************************************/
static uint32_t mm_FLS_CalculateDVC(dvc_calculate_t *p_dvc_param, uint8_t *const p_phase)
{
    uint32_t err;

#if defined(FSW_DVC_HASH)
    err = ML_DVC_Calculate(p_dvc_param, ML_DVC_CALC_TYPE_HASH);
    if (ER_OK == err)
    {
        /* do nothing */
    }
    else
    {
        *p_phase = FLS_PHASE_9;
    }
#else
    uint32_t resource_id;
    resource_id = FLS_ZERO_U32;
    /* get AES resource */
    err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
    if ((ER_OK == err) && (FLS_ZERO_U32 != resource_id))
    {
        /* normal end */
        err = ML_DVC_Calculate(p_dvc_param, ML_DVC_CALC_TYPE_CMAC);
        if (ER_OK == err)
        {
        }
        else
        {
            if (ER_ML_DVC_AES_TIMEOUT == err)
            {
                err = ER_MM_FLS_AES_TIME_OUT;
            }
            else if (ER_ML_DVC_ALIGN == err)
            {
                err = ER_MM_FLS_AES_PRM_ERROR;
            }
            else
            {
                /* never occur */
                err = ER_PARAM;
            }
            *p_phase = FLS_PHASE_9;
        }
        /* release AES resource(Do not check for errors) */
        (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
    }
    else
    {
        if (ER_ML_ABTR_EMPTY == err)
        {
            err = ER_PENDING;
        }
        else
        {
            err = ER_MM_FLS_AES_UNAVAILABLE;
        }
        *p_phase = FLS_PHASE_9;
    }
#endif /* defined(FSW_DVC_HASH) */
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0108]:[ICUMFW_UD_MM_FlashMNG_0108]
* Function name: mm_FLS_AESDSRead
* Description  : Read data with AES resource processing
* Arguments    : (in) flash_location_t *p_index : specified block index of data set
*                (out) uint8_t *p_data          : an address pointer that stores the read data
* Return Value : ER_OK
*                ER_PENDING
*                ER_MM_FLS_AES_UNAVAILABLE
*                ER_MM_FLS_SYS_CRITICAL
*                ER_MM_FLS_SYS          (ER_L_FLS_FLOW,ER_L_FLS_REJECTED)
*                ER_MM_FLS_DATA_INVALID
*                ER_MM_FLS_UNAVAILABLE
*                ER_MM_FLS_MEM_SOURCE
*                ER_MM_FLS_AES_TIME_OUT
*                ER_MM_FLS_DATA_BLANK
*                ER_MM_FLS_AES_PRM_ERROR
***************************************************************************************/
static uint32_t mm_FLS_AESDSRead(const flash_location_t *const p_index, uint8_t *p_data)
{
    uint32_t err;

#if defined(FSW_DVC_HASH)
    err = mm_FLS_DSRead(p_index, p_data);
#else
    uint32_t resource_id;
    resource_id = FLS_ZERO_U32;
    /* get AES resource */
    err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &resource_id);
    if ((ER_OK == err) && (resource_id != FLS_ZERO_U32))
    {
        /* normal end */
        err = mm_FLS_DSRead(p_index, p_data);
        /* release AES resource(Do not check for errors) */
        (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, resource_id);
    }
    else
    {
        /* error occurred */
        if (ER_ML_ABTR_EMPTY == err)
        {
            err = ER_PENDING;
        }
        else
        {
            err = ER_MM_FLS_AES_UNAVAILABLE;
        }
    }
#endif /* defined(FSW_DVC_HASH) */
    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0109]:[ICUMFW_UD_MM_FlashMNG_0109]
* Function name: mm_FLS_EndPhaseInternal
* Description  : end process (for internal function)
* Arguments    : (in/out) uint32_t* p_err   : Error code
*                (in/out) uint8_t*  p_phase : Processing phase
* Return Value  : none
***************************************************************************************/
static void mm_FLS_EndPhaseInternal(uint32_t *const p_err, uint8_t *const p_phase)
{
    /* Phase of end process */
    if ((uint32_t)FLS_PHASE_9 == (uint32_t)*p_phase)
    {
        /* operation completion phase */

        /* error conversion */
        *p_err = mm_FLS_ConvertionError(*p_err);

        /* initialize variable */
        *p_phase = FLS_PHASE_0;
    }
    else
    {
        /* Returns the process in progress, since it is not in the end process phase */
        if (ER_OK == *p_err)
        {
            *p_err = ER_MM_FLS_IN_PROGRESS;
        }
        else
        {
            /* do nothing */
        }
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0110]:[ICUMFW_UD_MM_FlashMNG_0110]
* Function name: mm_FLS_EndPhaseGCWarning
* Description  : end process (with GC warning update)
* Arguments    : (in/out) uint32_t* p_err   : Error code
*                (in)     uint8_t*  p_phase : Processing phase
* Return Value  : none
***************************************************************************************/
static void mm_FLS_EndPhaseGCWarning(uint32_t *const p_err, const uint8_t *const p_phase)
{
    gc_warning_t gc_warning;

    /* Phase of end process */
    if ((uint32_t)FLS_PHASE_9 == (uint32_t)*p_phase)
    {
        /* Calculate GC necessity from remaining flash capacity */
        gc_warning = mm_FLS_GetCapacityStatus();
        if (((uint32_t)NVM_DATA_WARNING == (uint32_t)g_nvm_data_warning)
            && ((uint32_t)FLS_NO_FULL_WARNING == (uint32_t)gc_warning))
        {
            /* do nothing */
        }
        else
        {
            g_nvm_data_warning = gc_warning;
        }
        /* Initialization process variables */
        mm_FLS_InitProcessVariable();
        /* free flash resource & clear FACIACC flag */
        mm_FLS_FreeFlashResource();
    }
    else
    {
        /* Returns the process in progress, since it is not in the end process phase */
        if (ER_OK == *p_err)
        {
            *p_err = ER_MM_FLS_IN_PROGRESS;
        }
        else
        {
            /* Do nothing */
        }
    }
    *p_err = mm_FLS_ConvertionError(*p_err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0111]:[ICUMFW_UD_MM_FlashMNG_0111]
* Function name: mm_FLS_EndPhaseWrite
* Description  : end process (for MM_FLS_Write)
* Arguments    : (in/out) uint32_t* p_err       : Error code
*                (in/out) uint8_t*  p_phase     : Processing phase
*                (in)     uint32_t  fblock      : Physical block number
*                (in)     uint8_t   flash_full  : Insufficient flash space flag
* Return Value  : none
***************************************************************************************/
static void mm_FLS_EndPhaseWrite(uint32_t *const p_err, const uint8_t *const p_phase, const uint32_t fblock,
                                 const uint8_t flash_full)
{
    gc_warning_t gc_warning;

    /* Phase of end process */
    if ((uint32_t)FLS_PHASE_9 == (uint32_t)*p_phase)
    {
        if (ER_MM_FLS_PROG == *p_err)
        {
            g_nvm_data_warning = FLS_FULL_WARNING2;
            g_write_block_idx[fblock] = (uint16_t)((uint32_t)mm_FLS_GetLastLBlock(fblock) + 1uL);
        }
        else
        {
            /* If the flash free space is insufficient, set ER_MM_FLS_PROG */
            if ((uint32_t)FLS_FLAG_ON_U8 == (uint32_t)flash_full)
            {
                *p_err = ER_MM_FLS_PROG;
            }
            else
            {
                /* do nothing */
            }
            /* Calculate GC necessity from remaining flash capacity */
            gc_warning = mm_FLS_GetCapacityStatus();
            if (((uint32_t)NVM_DATA_WARNING == (uint32_t)g_nvm_data_warning)
                && ((uint32_t)FLS_NO_FULL_WARNING == (uint32_t)gc_warning))
            {
                /* do nothing */
            }
            else
            {
                g_nvm_data_warning = gc_warning;
            }
        }
        /* Initialization process variables */
        mm_FLS_InitProcessVariable();
        /* free flash resource & clear FACIACC flag */
        mm_FLS_FreeFlashResource();
    }
    else
    {
        /* Returns the process in progress, since it is not in the end process phase */
        if (ER_OK == *p_err)
        {
            *p_err = ER_MM_FLS_IN_PROGRESS;
        }
        else
        {
            /* do nothing */
        }
    }
    *p_err = mm_FLS_ConvertionError(*p_err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0112]:[ICUMFW_UD_MM_FlashMNG_0112]
* Function name: mm_FLS_EndPhaseFC
* Description  : end process (for mm_FLS_ForwardCopy)
* Arguments    : (in/out) uint32_t* p_err   : Error code
*                (in)     uint8_t*  p_phase : Processing phase
* Return Value  : none
***************************************************************************************/
static void mm_FLS_EndPhaseFC(uint32_t *const p_err, const uint8_t *const p_phase)
{
    /* Phase of end process */
    if ((uint32_t)FLS_PHASE_9 == (uint32_t)*p_phase)
    {
        /* Initialization process variables */
        mm_FLS_InitProcessVariable();
        /* free flash resource & clear FACIACC flag */
        mm_FLS_FreeFlashResource();
    }
    else
    {
        /* Returns the process in progress, since it is not in the end process phase */
        if (ER_OK == *p_err)
        {
            *p_err = ER_MM_FLS_IN_PROGRESS;
        }
        else
        {
            /* Do nothing */
        }
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0048]:[ICUMFW_UD_MM_FlashMNG_0048]
* Function name: mm_FLS_GetWritableArea
* Description  : Find a writable free area
* Arguments    : (in) uint32_t prohibited_block             : write prohibited block number
*                (in) uint16_t write_size                   : expected writing size
*                (out) flash_block_idx_t* p_result_index    : writable index (valid only at found)
* Return Value  : FLS_FOUND
*                 FLS_NOT_FOUND
***************************************************************************************/
static uint32_t mm_FLS_GetWritableArea(const uint32_t prohibited_block, const uint16_t write_size,
                                       flash_block_idx_t *const p_result_index)
{
    uint32_t ret_val;
    uint32_t fblock;
    uint16_t writable_size;

    ret_val = FLS_NOT_FOUND;
    fblock = mm_FLS_GetAfterFBlock(prohibited_block);
    /* Physical block loop */
    while (fblock != prohibited_block)
    {
        /* Compare free size and write size */
        writable_size = (uint16_t)(
            (((uint32_t)mm_FLS_GetLastLBlock(fblock) + 1uL) - (uint32_t)g_write_block_idx[fblock]) * FLS_DF_BLOCK_SIZE);
        if ((uint32_t)writable_size >= (uint32_t)write_size)
        {
            /* writable free area found */
            *p_result_index = g_write_block_idx[fblock];
            ret_val = FLS_FOUND;
            break;
        }
        else
        {
            /* do nothing */
        }
        /* Go to the next physical block */
        fblock = mm_FLS_GetAfterFBlock(fblock);
    }

    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0049]:[ICUMFW_UD_MM_FlashMNG_0049]
* Function name: mm_FLS_CompareBlock
* Description  : Comparison between block indexes
* Arguments    : block1: Compare target block 1
*                block2: Compare target block 2
*                first_block: First block that is the basis for comparison
* Return Value : FLS_CMP_SMALL : block1 < block2
*                FLS_CMP_EQUAL : block1 = block2
*                FLS_CMP_LARGE : block1 > block2
***************************************************************************************/
static uint32_t mm_FLS_CompareBlock(const flash_block_idx_t block1, const flash_block_idx_t block2,
                                    const flash_block_idx_t first_block)
{
    uint32_t ret_val;
    flash_block_idx_t wk_block1;
    flash_block_idx_t wk_block2;

    wk_block1 = block1;
    wk_block2 = block2;
    if ((uint32_t)wk_block1 < (uint32_t)first_block)
    {
        wk_block1 = (flash_block_idx_t)((uint32_t)wk_block1 + (FLS_NB_BLOCK * FLS_LBLOCK_NUM_PER_PBLOCK));
    }
    else
    {
        /* do nothing */
    }
    if ((uint32_t)wk_block2 < (uint32_t)first_block)
    {
        wk_block2 = (flash_block_idx_t)((uint32_t)wk_block2 + (FLS_NB_BLOCK * FLS_LBLOCK_NUM_PER_PBLOCK));
    }
    else
    {
        /* do nothing */
    }
    if ((uint32_t)wk_block1 > (uint32_t)wk_block2)
    {
        ret_val = FLS_CMP_LARGE;
    }
    else if ((uint32_t)wk_block1 < (uint32_t)wk_block2)
    {
        ret_val = FLS_CMP_SMALL;
    }
    else
    {
        ret_val = FLS_CMP_EQUAL;
    }

    return (ret_val);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0050]:[ICUMFW_UD_MM_FlashMNG_0050]
* Function name: mm_FLS_GetFlashResource
* Description  : Get flash resource
* Arguments    : const r_icumif_job_id_t job_id : job id
* Return Value : ER_OK
*                ER_PENDING
*                ER_L_FLS_UNAVAILBLE
*                ER_L_FLS_INTERNAL
***************************************************************************************/
static uint32_t mm_FLS_GetFlashResource(const r_icumif_job_id_t job_id)
{
    uint32_t err;
    uint32_t lock_handle;

    lock_handle = (uint32_t)job_id;
    /* Get Flash Resource */
    err = L_FLS_GetFlashResource_ram(lock_handle, R_ICUMIF_SERV_PRIORITY_HIGHEST, AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        /* after get resource */
        g_get_resource_flg = FLS_VAL_2;
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0051]:[ICUMFW_UD_MM_FlashMNG_0051]
* Function name: mm_FLS_FreeFlashResource
* Description  : Free flash resource
* Arguments    : N/A
* Return Value : none
***************************************************************************************/
static void mm_FLS_FreeFlashResource(void)
{
    switch ((uint32_t)g_get_resource_flg)
    {
        case FLS_VAL_2:
            /* Free flash resource */
            (void)L_FLS_FreeFlashResource_ram(AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
            break;

        case FLS_VAL_1:
        default:
            /* in case ER_MM_FLS_UNAVAILABLE error occurred or before exclusive check */
            /* Do nothing */
            break;
    }
    /* clear get resource flag */
    g_get_resource_flg = FLS_VAL_0;
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0014]:[ICUMFW_UD_MM_FlashMNG_0014]
* Function name: MM_FLS_GetFreeCapacity
* Description  : Get number of free logical blocks information
* Arguments    : (out) free_block_t* p_free_block   : number of free logical blocks information pointer
* Return Value : ER_OK
***************************************************************************************/
uint32_t MM_FLS_GetFreeCapacity(free_block_t *p_free_block)
{
    uint32_t fblock;

    p_free_block->total_free_block_num = FLS_ZERO_U16;
    /* Calculate the total number of free logical blocks */
    for (fblock = FLS_ZERO_U32; fblock < FLS_NB_BLOCK; fblock++)
    {
        p_free_block->free_block_num[fblock] =
            (uint16_t)(((uint32_t)mm_FLS_GetLastLBlock(fblock) + 1uL) - (uint32_t)g_write_block_idx[fblock]);
        p_free_block->total_free_block_num =
            (uint16_t)((uint32_t)p_free_block->total_free_block_num + (uint32_t)p_free_block->free_block_num[fblock]);
    }

    return (ER_OK);
}

/* self copy icum_middle_rom.text icum_middle_ram   */
CHANGE_SEC(text, ".icum_middle_ram")
/* Read only data referenced on RAM */
CHANGE_SEC(rodata, ".icum_middle_const_ram")
/*******************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0082]:[ICUMFW_UD_MM_FlashMNG_0082]
* Function Name : mm_FLS_CheckFlashAddr
* Description   : Check flash memory range and get offset and bank number.
* Arguments     : uint8_t target_flash
*                   target flash memory (FLS_SELECT_CF, FLS_SELECT_DF, FLS_SELECT_ICUM_DF)
*               : uint8_t target_area
*                   target flash area (FLS_SELECT_USER, FLS_SELECT_USER)
*                 uint32_t addr
*                   target flash address
*                 uint32_t size
*                   target size
* Return Value  :   ER_OK
*                   ER_PARAM
********************************************************************************************/
static uint32_t mm_FLS_CheckFlashAddr(const uint8_t target_flash, const uint8_t target_area, uint32_t addr,
                                      uint32_t size, uint32_t *p_bank, uint32_t *p_offset)
{
    uint32_t err;

    if ((uint32_t)FLS_SELECT_USER == (uint32_t)target_area)
    {
        if ((uint32_t)FLS_SELECT_DF == (uint32_t)target_flash)
        {
            err = L_FLS_CheckUserDFBoundary(addr, size, p_bank, p_offset);
        }
        else
        {
            err = L_FLS_CheckUserCFBoundary(addr, size, p_bank, p_offset);
        }
    }
    else if ((uint32_t)FLS_SELECT_SECURE == (uint32_t)target_area)
    {
        if ((uint32_t)FLS_SELECT_CF == (uint32_t)target_flash)
        {
            err = L_FLS_CheckSCFBoundary(addr, size, p_bank, p_offset);
        }
        else
        {
            /* FLS_SELECT_ICUM_DF */
            err = ER_PARAM;
        }
    }
    else
    {
        err = ER_PARAM;
    }

    return (err);
}

/*******************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0080]:[ICUMFW_UD_MM_FlashMNG_0080]
* Function Name : MM_FLS_GetFlashResource
* Description   : Get flash resources.
* Arguments     : r_icumif_job_id_t job_id
*                   job id(context_id)
*                 r_icumif_service_priority_t service_priority
*                   service priority
*                 uint8_t target_flash
*                   target flash memory (FLS_SELECT_CF, FLS_SELECT_DF)
*                 uint8_t target_area
*                   target flash area (FLS_SELECT_USER, FLS_SELECT_USER)
*                 uint32_t addr
*                   target flash address
*                 uint32_t size
*                   target size
* Return Value  :   ER_OK
*                   ER_PARAM
*                   ER_PENDING
*                   ER_MM_FLS_SYS_CRITICAL
*                   ER_MM_FLS_UNAVAILABLE
********************************************************************************************/
uint32_t MM_FLS_GetFlashResource(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                                 uint8_t target_flash, uint8_t target_area, uint32_t addr, uint32_t size)
{
    uint32_t ret_val;
    uint32_t err;
    uint32_t bank;
    uint32_t offset;
    uint32_t area;
    uint32_t lock_handle;

    bank = FLS_ZERO_U32;
    lock_handle = (uint32_t)job_id;
    if (FLS_ZERO_U32 == (uint32_t)job_id)
    {
        err = ER_PARAM;
    }
    else
    {
        err = mm_FLS_CheckFlashAddr(target_flash, target_area, addr, size, &bank, &offset);
    }

    if (ER_OK == err)
    {
        /* set FACIACC flags */
        err = L_FLS_SetFACIStatus_ram();
        if (ER_OK == err)
        {
            if ((uint32_t)FLS_SELECT_DF == (uint32_t)target_flash)
            {
                area = AG_L_FLS_DATA;
            }
            else
            {
                area = AG_L_FLS_CODE;
            }
            /* Get Flash Resource */
            err = L_FLS_GetFlashResource_ram(lock_handle, service_priority, area, bank);
            if (ER_OK != err)
            {
                /* reset FACIACC flag */
                (void)L_FLS_ClrFACIStatus_ram();
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
    }
    else
    {
        /* Do nothing */
    }

    ret_val = mm_FLS_ConvertionError(err);

    return (ret_val);
}

/*******************************************************************************************
* Function ID   : [ICUMFW_CD_MM_FlashMNG_0081]:[ICUMFW_UD_MM_FlashMNG_0081]
* Function Name : MM_FLS_FreeFlashResource
* Description   : Release flash resources.
* Arguments     : uint8_t target_flash
*                   target flash memory (FLS_SELECT_CF, FLS_SELECT_DF, FLS_SELECT_ICUM_DF)
                  uint8_t target_area
*                   target flash area (FLS_SELECT_USER, FLS_SELECT_USER)
*                 uint32_t addr
*                   target flash address
*                 uint32_t size
*                   target size
* Return Value  :   ER_OK
*                   ER_PARAM
*                   ER_PENDING
*                   ER_MM_FLS_SYS_CRITICAL
*                   ER_MM_FLS_UNAVAILABLE
********************************************************************************************/
uint32_t MM_FLS_FreeFlashResource(uint8_t target_flash, uint8_t target_area, uint32_t addr, uint32_t size)
{
    uint32_t ret_val;
    uint32_t err;
    uint32_t bank;
    uint32_t offset;
    uint32_t area;

    bank = FLS_ZERO_U32;
    err = mm_FLS_CheckFlashAddr(target_flash, target_area, addr, size, &bank, &offset);
    if (ER_OK == err)
    {
        if ((uint32_t)FLS_SELECT_DF == (uint32_t)target_flash)
        {
            area = AG_L_FLS_DATA;
        }
        else
        {
            area = AG_L_FLS_CODE;
        }
        /* Release flash resources */
        (void)L_FLS_FreeFlashResource_ram(area, bank);
        /* reset FACIACC flag */
        (void)L_FLS_ClrFACIStatus_ram();
    }
    else
    {
        /* Do nothung */
    }

    ret_val = mm_FLS_ConvertionError(err);

    return (ret_val);
}

/* Code executed on ROM           */
CHANGE_SEC(text, ".icum_middle.text")
/* Read only data referenced on ROM */
CHANGE_SEC(rodata, ".icum_middle.rodata")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0086]:[ICUMFW_UD_MM_FlashMNG_0086]
* Function name: mm_FLS_GetPartitionInfoBaseOffset
* Description  : read partition info
* Arguments    : (in) uint8_t target            : partition info A / B
* Return Value : partition info base offset
*****************************************************************************************/
static uint32_t mm_FLS_GetPartitionInfoBaseOffset(const uint8_t target)
{
    uint32_t df_sdf_lower_limit;
    uint32_t df_sdf_upper_limit;
    uint32_t offset;

    /* Get SDF Addr */
    (void)MM_FLS_GetMemBoundary(MM_FLS_LOWER_LIMIT_DF_SDF, &df_sdf_lower_limit);
    (void)MM_FLS_GetMemBoundary(MM_FLS_UPPER_LIMIT_DF_SDF, &df_sdf_upper_limit);
    if ((uint32_t)FLS_PINFO_A == (uint32_t)target)
    {
        df_sdf_upper_limit -= ((uint32_t)FLS_PINFO_OFFSET_A);
    }
    else
    {
        df_sdf_upper_limit -= ((uint32_t)FLS_PINFO_OFFSET_B);
    }

    offset = (df_sdf_upper_limit - df_sdf_lower_limit) + 1uL;

    return (offset);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0087]:[ICUMFW_UD_MM_FlashMNG_0087]
* Function name: mm_FLS_GetPartitionInfoOffset
* Description  : read partition info
* Arguments    : (in) uint8_t target            : partition info A / B
*                (out) uint32_t *p_offset       : pointer to offset
* Return Value : ER_OK
*                ER_PARAM
*****************************************************************************************/
static uint32_t mm_FLS_GetPartitionInfoOffset(const uint8_t target, uint32_t *const p_offset)
{
    uint32_t err;
    uint32_t i;
    uint32_t offset;
    uint32_t base;
    uint32_t blank_check_result;

    /* Get Partition Info base offset */
    base = mm_FLS_GetPartitionInfoBaseOffset(target);
    offset = 0uL;

    /* Blank Check Partition Info */
    for (i = 0uL; i < (uint32_t)FLS_PINFO_NB_BLOCKS; i++)
    {
        err = L_FLS_ReadBlankCheckArea(AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM, (base + offset),
                                       FLS_PINFO_ELEMENT_SIZE, L_FLS_BLKCA_INC, &blank_check_result);
        if ((ER_OK != err) || (ST_L_FLS_BLANK == blank_check_result))
        {
            break;
        }
        else
        {
            /* do nothing */
        }

        offset += FLS_PINFO_BLOCK_SIZE;
    }

    if (ER_OK == err)
    {
        *p_offset = offset;
    }
    else
    {
        /* do nothing */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0088]:[ICUMFW_UD_MM_FlashMNG_0088]
* Function name: mm_FLS_WritePartitionInfo
* Description  : read partition info
* Arguments    : (in) r_icumif_job_id_t job_id  : job id
*                (out) uint32_t offset          : partition info offset
*                (out) uint32_t *p_data         : pointer to write data
* Return Value : ER_OK
*                ER_IN_PROCESS 
*                ER_PARAM
*                ER_L_FLS_PROTECTION
*                ER_L_FLS_REJECTED
*                ER_L_FLS_PROGRAM
*                ER_L_FLS_INTERNAL 
*                ER_L_FLS_UNAVAILABLE
*                ER_L_FLS_FACI_TIME_OUT
*****************************************************************************************/
static uint32_t mm_FLS_WritePartitionInfo(const r_icumif_job_id_t job_id, uint32_t offset, const uint32_t *const p_data)
{
    uint32_t err;
    uint32_t is_blank;
    uint32_t lock_handle;

    lock_handle = (uint32_t)job_id;

    err = L_FLS_ReadBlankCheckArea(AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM, offset,
                                   FLS_PINFO_ELEMENT_SIZE, L_FLS_BLKCA_INC, &is_blank);
    if (ER_OK != err)
    {
        /* do nothing */
    }
    else
    {
        if (ST_L_FLS_BLANK == is_blank)
        {
            err = L_FLS_REQ_WriteFlash_ram(AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM,
                                           offset, FLS_PINFO_ELEMENT_SIZE, (const uint8_t *)p_data);
            if ((ER_OK != err) && (ER_IN_PROCESS != err))
            {
                /* do nothing */
            }
            else
            {
                while (ER_IN_PROCESS == err)
                {
                    err = L_FLS_IsFinished_ram((uint16_t)lock_handle, AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
                }
            }
        }
        else
        {
            /* non blank address. */
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0083]:[ICUMFW_UD_MM_FlashMNG_0083]
* Function name: MM_FLS_WritePartitionInfo
* Description  : write partition info
* Arguments    : (in) r_icumif_job_id_t job_id  : job id
*                (in) r_icumif_service_priority_t  service_priority
*                                               : service priority
*                (in) uint8_t target            : partition info A / B
*                (out) uint32_t info            : partition info type
*                (out) uint8_t *p_data          : pointer to counter of partition info
* Return Value : ER_OK
*                ER_IN_PROCESS
*                ER_PARAM
*                ER_PENDING
*                ER_MM_FLS_SYS 
*                ER_MM_FLS_SYS_CRITICAL
*                ER_MM_FLS_PROG
*                ER_MM_FLS_UNAVAILABLE
*****************************************************************************************/
uint32_t MM_FLS_WritePartitionInfo(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                                   uint8_t target, uint32_t info, uint32_t *p_data)
{
    uint32_t err;
    uint32_t offset;
    uint32_t target_offset;
    uint32_t tag;

    offset = FLS_ZERO_U32;
    tag = FLS_PINFO_TAG;


    if (((uint32_t)R_ICUMIF_SERV_PRIORITY_HIGHEST == (uint32_t)service_priority)
        || ((uint32_t)R_ICUMIF_SERV_PRIORITY_HIGH == (uint32_t)service_priority)
        || ((uint32_t)R_ICUMIF_SERV_PRIORITY_NORMAL == (uint32_t)service_priority)
        || ((uint32_t)R_ICUMIF_SERV_PRIORITY_LOWEST == (uint32_t)service_priority))
    {
        /* Normal */
    }
    else
    {
        /* Abnormal */
    }

    /*  Check Parameters */
    if ((uint32_t)FLS_ZERO_U16 == (uint32_t)job_id)
    {
        err = ER_PARAM;
    }
    else
    {
        if (((uint32_t)FLS_PINFO_A != (uint32_t)target) && ((uint32_t)FLS_PINFO_B != (uint32_t)target))
        {
            err = ER_PARAM;
        }
        else
        {
            err = L_FLS_CheckContext_ram(job_id, AG_D_FLS_DATA, AG_L_FLS_BANK_ICUM);
        }
    }

    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        err = mm_FLS_GetFlashResource(job_id);
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            err = mm_FLS_GetPartitionInfoOffset(target, &offset);
            if (ER_OK != err)
            {
                /* Do nothing */
            }
            else
            {
                target_offset = mm_FLS_GetPartitionInfoBaseOffset(target);
                target_offset += offset;

                if (FLS_PINFO_COUNTER == (info & FLS_PINFO_COUNTER))
                {
                    if ((uint32_t)FLS_PINFO_SIZE <= offset)
                    {
                        err = ER_MM_FLS_PROG;
                    }
                    else
                    {
                        err = mm_FLS_WritePartitionInfo(job_id, target_offset, p_data);
                    }
                }
                else
                {
                    /* Do nothing */
                }

                if ((ER_OK == err) && (FLS_PINFO_ACTIVE == (info & FLS_PINFO_ACTIVE)))
                {
                    if ((uint32_t)FLS_PINFO_SIZE <= offset)
                    {
                        err = ER_MM_FLS_PROG;
                    }
                    else
                    {
                        err = mm_FLS_WritePartitionInfo(job_id, (target_offset + FLS_PINFO_ACTIVE_OFFSET), &tag);
                    }
                }
                else
                {
                    /* Do nothing */
                }

                if ((ER_OK == err) && (FLS_PINFO_REVOKE == (info & FLS_PINFO_REVOKE)) && (0uL < offset))
                {
                    err = mm_FLS_WritePartitionInfo(job_id, (target_offset - FLS_PINFO_REVOKE_OFFSET), &tag);
                }
                else
                {
                    /* Do nothing */
                }

                if ((ER_OK == err) && (FLS_PINFO_INVALID == (info & FLS_PINFO_INVALID)) && (0uL < offset))
                {
                    if ((uint32_t)FLS_PINFO_SIZE <= offset)
                    {
                        err = ER_MM_FLS_PROG;
                    }
                    else
                    {
                        err = mm_FLS_WritePartitionInfo(job_id, (target_offset - FLS_PINFO_INVALID_OFFSET), &tag);
                    }
                }
                else
                {
                    /* Do nothing */
                }
            } /* end of else */

            /* Free Resource */
            mm_FLS_FreeFlashResource();
        } /* end of else */
    } /* end of else */

    err = mm_FLS_ConvertionError(err);

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0089]:[ICUMFW_UD_MM_FlashMNG_0089]
* Function name: mm_FLS_ReadPartitionInfo
* Description  : read partition info
* Arguments    : (in) uint32_t offset           : partition info offset
*                (out) uint32_t *p_info         : pointer to partition info type
*                (out) uint8_t *p_data          : pointer to counter of partition info
* Return Value : ER_OK
*                ER_PARAM
*                ER_L_FLS_FLOW 
*                ER_L_FLS_ECC1EDF1
*                ER_L_FLS_ECC2EDF1 
*****************************************************************************************/
static uint32_t mm_FLS_ReadPartitionInfo(const uint32_t offset, uint32_t *const p_info, uint32_t *const p_data)
{
    uint32_t err;
    uint32_t i;
    uint32_t is_blank;
    uint32_t wk_offset;

    wk_offset = offset;
    for (i = 0uL; i < FLS_PINFO_ELEMENT_NUM; i++)
    {
        wk_offset -= FLS_PINFO_ELEMENT_SIZE;
        err = L_FLS_ReadBlankCheckArea(AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM, wk_offset,
                                       FLS_PINFO_ELEMENT_SIZE, L_FLS_BLKCA_INC, &is_blank);
        if (ER_OK != err)
        {
            break;
        }
        else
        {
            /* Do nothing */
        }

        *p_info <<= 0x01u;
        if (ST_L_FLS_NON_BLANK == is_blank)
        {
            *p_info |= 1uL;
        }
        else
        {
            /* Do nothing */
        }
    }

    /* Read Counter */
    if ((ER_OK == err) && ((uint32_t)FLS_PINFO_COUNTER == (*p_info & (uint32_t)FLS_PINFO_COUNTER)))
    {
        err = L_FLS_ReadFlash_ram(AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM, wk_offset,
                                  FLS_PINFO_ELEMENT_SIZE, (uint8_t *)p_data);
    }
    else
    {
        /* Do nothing  */
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0084]:[ICUMFW_UD_MM_FlashMNG_0084]
* Function name: MM_FLS_ReadPartitionInfo
* Description  : read partition info
* Arguments    : (in) r_icumif_job_id_t job_id  : job id
*                (in) r_icumif_service_priority_t  service_priority
*                                               : service priority
*                (in) uint8_t target            : partition info A / B
*                (out) uint32_t *p_info         : pointer to partition info type
*                (out) uint8_t *p_data          : pointer to counter of partition info
* Return Value : ER_OK
*                ER_PARAM
*                ER_PENDING
*                ER_MM_FLS_SYS
*                ER_MM_FLS_SYS_CRITICAL
*                ER_MM_FLS_DATA_INVALID 
*                ER_MM_FLS_UNAVAILABLE
*****************************************************************************************/
uint32_t MM_FLS_ReadPartitionInfo(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                                  uint8_t target, uint32_t *p_info, uint32_t *p_data)
{
    uint32_t err;
    uint32_t offset;
    uint32_t target_offset;

    offset = FLS_ZERO_U32;

    if(((uint32_t)R_ICUMIF_SERV_PRIORITY_HIGHEST == (uint32_t)service_priority)
        || ((uint32_t)R_ICUMIF_SERV_PRIORITY_HIGH == (uint32_t)service_priority)
        || ((uint32_t)R_ICUMIF_SERV_PRIORITY_NORMAL == (uint32_t)service_priority)
        || ((uint32_t)R_ICUMIF_SERV_PRIORITY_LOWEST == (uint32_t)service_priority))
    {
        /* Normal */
    }
    else
    {
        /* Abnormal */
    }

    /* Check Parameters */
    if ((uint32_t)FLS_ZERO_U16 == (uint32_t)job_id)
    {
        err = ER_PARAM;
    }
    else
    {
        if ((((uint32_t)FLS_PINFO_A != (uint32_t)target) && ((uint32_t)FLS_PINFO_B != (uint32_t)target))
            || (NULL == p_info)
            || (NULL == p_data))
        {
            err = ER_PARAM;
        }
        else
        {
            /* Check Context */
            err = L_FLS_CheckContext_ram(job_id, AG_L_FLS_DATA, AG_L_FLS_BANK_ICUM);
        }
    }

    if (ER_OK != err)
    {
        /* Do nothing */
    }
    else
    {
        /* Get Resource */
        err = mm_FLS_GetFlashResource(job_id);
        if (ER_OK != err)
        {
            /* Do nothing */
        }
        else
        {
            err = mm_FLS_GetPartitionInfoOffset(target, &offset);
            if (ER_OK != err)
            {
                /* Do nothing */
            }
            else
            {
                *p_info = 0uL;
                if (0uL == offset)
                {
                    /* Do nothing */
                }
                else
                {
                    target_offset = mm_FLS_GetPartitionInfoBaseOffset(target);
                    target_offset += offset;
                    err = mm_FLS_ReadPartitionInfo(target_offset, p_info, p_data);
                }
            }

            /* Free Resource */
            mm_FLS_FreeFlashResource();
        }
    }

    err = mm_FLS_ConvertionError(err);

    return err;
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/* ----- end of source code file ----- */
