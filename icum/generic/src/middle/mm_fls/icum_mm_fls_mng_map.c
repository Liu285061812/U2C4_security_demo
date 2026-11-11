/******************************************************************************/
/* Component Name  Flash Manager  mapping related                             */
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
#include "icum_d_misc_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mm_flash.h"
#include "icum_mh_key_pub.h"

/*******************************************************************************
* Local defines
********************************************************************************/

/*******************************************************************************
* Type definition
********************************************************************************/

/*******************************************************************************
* Local variables
********************************************************************************/
/* table of data sets */
static data_set_t g_data_set[FLS_DS_NB_DATA_SET];

/* NVM Data Warning occurrence flag */
uint8_t g_nvm_data_warning;
/* NVM Warning Data ring buffer write position pointer */
static uint8_t g_wr_pt;
/* NVM Warning Data ring buffer */
static nvm_warning_buff_t g_nvm_warn_buff;

/***************************************************************************************/
/* Local Prototypes                                                                    */
/***************************************************************************************/
static void mm_FLS_ClrNVMWarnBuff(void);

/*******************************************************************************
* Functions
********************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0058]:[ICUMFW_UD_MM_FlashMNG_0058]
* Function name: mm_FLS_CheckDataSetMapping
* Description  : Check dataset of mapping
* Arguments    : (in)  uint8_t ds_type  : type of data-set
*                (out) uint8_t *p_result : return the checked result
* Return Value : none
***************************************************************************************/
void mm_FLS_CheckDataSetMapping(const uint8_t ds_type, uint8_t *p_result)
{
    uint8_t res_val;

    if ((uint32_t)FLS_DS_NB_DATA_SET > (uint32_t)ds_type)
    {
        /* all check OK */
        res_val = FLS_CHK_OK;
    }
    else
    {
        /* check NG */
        res_val = FLS_CHK_NG;
    }

    *p_result = res_val;
    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0059]:[ICUMFW_UD_MM_FlashMNG_0059]
* Function name: mm_FLS_get_DataSet_size
* Description  : get data-set size
* Arguments    : (in)  uint8_t ds_type  : type of data-set
*                (out) uint16_t *p_size : return data-set size
* Return Value : none
***************************************************************************************/
void mm_FLS_get_DataSet_size(const uint8_t ds_type, uint16_t *p_size)
{
    if ((uint32_t)FLS_DS_NB_DATA_SET > (uint32_t)ds_type)
    {
        *p_size = (uint16_t)((uint32_t)g_data_set[ds_type].nb_blocks_per_slot * (uint32_t)FLS_DF_BLOCK_SIZE);
    }
    else
    {
        /* Do nothing */
    }

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0060]:[ICUMFW_UD_MM_FlashMNG_0060]
* Function name: mm_FLS_ClearMap
* Description  : data-set Map initialization
* Arguments    : none
* Return Value : none
***************************************************************************************/
void mm_FLS_ClearMap(void)
{
    /* Initialize data                */
    /* nb_slots            = 0x0000u; */
    /* nb_blocks_per_slot  = 0x0000u; */
    /* nb_flash_copies     = 0x0000u; */
    const data_set_t init_data = {0u, 0u, 0u};

    uint32_t i;

    /* g_data_set */
    for (i = FLS_ZERO_U32; (uint32_t)FLS_DS_NB_DATA_SET > i; i++)

    {
        g_data_set[i] = init_data;
    }

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0052]:[ICUMFW_UD_MM_FlashMNG_0052]
* Function name: MM_FLS_DeclareDataSet
* Description  : declare of data set
* Arguments    : (in) uint8_t ds_type   : type of data-set
*                (in) uint8_t nb_slots  : number of slot
*                (in) nb_blocks_per_slot: number of blocks per slot
*                (in)  number of copies : number of copies
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MM_FLS_DeclareDataSet(uint8_t ds_type ,uint8_t nb_slots,
                               uint16_t nb_blocks_per_slot, uint16_t nb_flash_copies)
{
    uint32_t err;

    /* parameter check */
    if ((uint32_t)FLS_DS_NB_DATA_SET <= (uint32_t)ds_type)
    {
        /* Does not occurred */
        err = ER_PARAM;
    }
    else if ((uint32_t)FLS_DS_NB_COPIES != (uint32_t)nb_flash_copies)
    {
        /* Does not occurred */
        err = ER_PARAM;
    }
    else
    {
        g_data_set[ds_type].nb_slots           = nb_slots;
        g_data_set[ds_type].nb_blocks_per_slot = nb_blocks_per_slot;
        g_data_set[ds_type].nb_flash_copies    = nb_flash_copies;
        err = ER_OK;
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0053]:[ICUMFW_UD_MM_FlashMNG_0053]
* Function name: MM_FLS_CheckDataSetSize
* Description  : confirms that the data set size fits within the secure DF size
*                data set declaration must have been done prior to calling this function
* Arguments    : (out) uint8_t *result : return the checked result
* Return Value : ER_OK
***************************************************************************************/
uint32_t MM_FLS_CheckDataSetSize(uint8_t *p_result)
{
    uint32_t    total_blocks;
    uint32_t    total_slots;
    data_set_t  wk;
    uint32_t    i;
    total_blocks = FLS_ZERO_U32;
    total_slots  = FLS_ZERO_U32;
    *p_result    = FLS_CHK_OK;
    /* all data set size Calculation */
    for (i = FLS_ZERO_U32; (uint32_t)FLS_DS_NB_DATA_SET > i; i++)
    {
        wk = g_data_set[i];
        if (((uint32_t)FLS_SLOT_MAX / FLS_DF_BLOCK_SIZE) < (uint32_t)wk.nb_blocks_per_slot)
        {
            *p_result = FLS_CHK_NG;
            break;
        }
        else
        {
            total_blocks += ((uint32_t)wk.nb_slots * (uint32_t)wk.nb_blocks_per_slot);
            total_slots += (uint32_t)wk.nb_slots;
        }
    }
    if (((uint32_t)FLS_CHK_OK == (uint32_t)(*p_result))
        && ((FLS_LBLOCK_NUM_PER_PBLOCK * (FLS_NB_BLOCK - FLS_RESERVE_BLOCK_NUM)) >= total_blocks)
        && (FLS_DATA_MAP_SIZE >= total_slots))
    {
        *p_result = FLS_CHK_OK;
    }
    else
    {
        *p_result = FLS_CHK_NG;
    }
    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0062]:[ICUMFW_UD_MM_FlashMNG_0062]
* Function name: mm_FLS_InitNVMWarnData
* Description  : init NVM data warning
* Arguments    : none
* Return Value : none
***************************************************************************************/
void mm_FLS_InitNVMWarnData(void)
{
    /* Clear NVM Data Warning occurrence flag */
    g_nvm_data_warning = NVM_DATA_NO_WARNING;
    /* Clear NVM Data Warning info buffer */
    mm_FLS_ClrNVMWarnBuff();
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0063]:[ICUMFW_UD_MM_FlashMNG_0063]
* Function name: mm_FLS_SetNVMWarnData
* Description  : set element to NVM data warning info buffer
* Arguments    : (in) uint8_t ds_type   : type of data-set
*                (in) uint8_t slot_num  : slot number
* Return Value : ER_OK
***************************************************************************************/
uint32_t mm_FLS_SetNVMWarnData(const uint8_t ds_type, const uint8_t slot_num)
{
    uint32_t wr_pt;

    /* Check pointer */
    wr_pt = (uint32_t)g_wr_pt;
    if (NVM_WARN_DATA_NUM <= wr_pt)
    {
        /* pointer wrap around */
        wr_pt = (uint32_t)NVM_WARN_WRPT_INIT;
    }
    else
    {
        /* pointer is Within range */
    }

    /* Put ring buffer */
    g_nvm_warn_buff.data[wr_pt].ds_type    = ds_type;
    g_nvm_warn_buff.data[wr_pt].slot_index = slot_num;
    /* pointer update */
    wr_pt++;
    g_wr_pt = (uint8_t)wr_pt;
    /* Set NVM Data Warning occurrence flag */
    g_nvm_data_warning = NVM_DATA_WARNING;

    return (ER_OK);
}

/* self copy icum_middle_rom.text icum_middle_ram   */
CHANGE_SEC(text, ".icum_middle_ram")
CHANGE_SEC(rodata, ".icum_middle_const_ram")

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0061]:[ICUMFW_UD_MM_FlashMNG_0061]
* Function name: mm_FLS_ClrNVMWarnBuff
* Description  : Clear NVM Data Warning info buffer
* Arguments    : none
* Return Value : none
***************************************************************************************/
static void mm_FLS_ClrNVMWarnBuff(void)
{
    uint32_t i;

    /* Clear Warning Data */
    for (i = FLS_ZERO_U32; NVM_WARN_DATA_NUM > i; i++)
    {
        g_nvm_warn_buff.data[i].ds_type    = NVM_WARN_DSTYPE_INIT;
        g_nvm_warn_buff.data[i].slot_index = NVM_WARN_SLOTNUM_INIT;
    }
    /* Clear Warning Data buffer write position pointer */
    g_wr_pt = NVM_WARN_WRPT_INIT;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0054]:[ICUMFW_UD_MM_FlashMNG_0054]
* Function name: MM_FLS_GetNVMWarnFlg
* Description  : Get NVM Data Warning occurrence flag
* Arguments    : uint8_t *p_nvm_warn_flg : nvm_warn_flg storage address pointer
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MM_FLS_GetNVMWarnFlg(uint8_t *p_nvm_warn_flg)
{
    uint32_t err;

    if (NULL == p_nvm_warn_flg)
    {
        /* Does not occurred */
        err = ER_PARAM;
    }
    else
    {
        *p_nvm_warn_flg = g_nvm_data_warning;
        err = ER_OK;
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0055]:[ICUMFW_UD_MM_FlashMNG_0055]
* Function name: MM_FLS_ClrNVMWarnFlg
* Description  : Clr NVM Data Warning occurrence flag
* Arguments    : none
* Return Value : ER_OK
***************************************************************************************/
uint32_t MM_FLS_ClrNVMWarnFlg(void)
{
    g_nvm_data_warning = NVM_DATA_NO_WARNING;

    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0056]:[ICUMFW_UD_MM_FlashMNG_0056]
* Function name: MM_FLS_GetNVMWarnData
* Description  : Get NVM Warning Data & Clear NVM Data Warning info buffer
* Arguments    : uint8_t *p_nvm_warn_flg : nvm_warn_buff storage address pointer
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MM_FLS_GetNVMWarnData(nvm_warning_buff_t *p_nvm_warn_buff)
{
    uint32_t err;

    if (NULL == p_nvm_warn_buff)
    {
        /* data address is NULL  */
        err = ER_PARAM;
    }
    else
    {
        *p_nvm_warn_buff = g_nvm_warn_buff;
        /*   Clear NVM Data Warning info buffer */
        mm_FLS_ClrNVMWarnBuff();
        err = ER_OK;
    }

    return (err);
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0057]:[ICUMFW_UD_MM_FlashMNG_0057]
* Function name: MM_FLS_GetMaxSlotNum
* Description  : Get maximum number of slots
* Arguments    : (in) uint8_t  ds_type      : type of data-set
                 (out) uint8_t* p_nb_slots  : Maximum number of slots
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t MM_FLS_GetMaxSlotNum(const uint8_t ds_type, uint8_t *p_nb_slots)
{
    uint32_t err;

    /* parameter check */
    if ((uint32_t)FLS_DS_NB_DATA_SET <= (uint32_t)ds_type)
    {
        /* Does not occurred */
        err = ER_PARAM;
    }
    else
    {
        *p_nb_slots = g_data_set[ds_type].nb_slots;
        err = ER_OK;
    }

    return (err);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0069]:[ICUMFW_UD_MM_FlashMNG_0069]
* Function name: MM_FLS_RemapStart()
* Description  : Remap area development
* Arguments    : (IN) p_uiTargetAddr : Remap target data address
*                (OUT) p_uiRemapAddr : Remapped address
* Return Value : ER_OK
*                ER_MM_FLS_MEM_SOURCE
****************************************************************************************/
uint32_t MM_FLS_RemapStart(const uint32_t p_uiTargetAddr, uint32_t **p_uiRemapAddr)
{
    uint32_t err;
    uint32_t ret_val;

    err = L_FLS_RemapStart(p_uiTargetAddr , p_uiRemapAddr);
    ret_val = mm_FLS_ConvertionError(err);
    return ret_val;
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MM_FlashMNG_0070]:[ICUMFW_UD_MM_FlashMNG_0070]
* Function name: MM_FLS_RemapEnd()
* Description  : Initialization Remap Area
* Arguments    : None
* Return Value : None
****************************************************************************************/
void MM_FLS_RemapEnd(void)
{
    L_FLS_RemapEnd();
}

/* ----- end of source code file ----- */
