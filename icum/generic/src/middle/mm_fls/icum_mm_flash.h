/******************************************************************************/
/* Component Name  Flash Manager    (Internal)                                */
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

#if !defined(ICUM_MM_FLASH_H)
#define ICUM_MM_FLASH_H

/**************************************************************************************/
/* Headers                                                                            */
/**************************************************************************************/
#include "icum_ml_dvc_pub.h"

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* operation of flash memory */
#define FLS_OP_READ                 ((uint8_t)0)
#define FLS_OP_WRITE                ((uint8_t)1)
#define FLS_OP_ERASE                ((uint8_t)2)
#define FLS_OP_B_CHECK              ((uint8_t)3)
#define FLS_OP_CFG_WRITE            ((uint8_t)4)

/* word size definition */
#define WORD_SIZE                   (4uL)

/* constant zero of each type */
#define FLS_ZERO_U8                 ((uint8_t)0)
#define FLS_ZERO_U16                ((uint16_t)0)
#define FLS_ZERO_U32                ((uint32_t)0)

/* all bit 1 32bit word */
#define FLS_FULL_U32                (0xFFFFFFFFuL)

/* flag on */
#define FLS_FLAG_ON_U8              ((uint8_t)1)
                                        /* flag on */
/* data slot tag code */
#define PROGRAMMING_TAG             (0xA55A5AA5uL)


/* write operation phase number */
#define FLS_PHASE_0                 (0u)
#define FLS_PHASE_1                 (1u)
#define FLS_PHASE_2                 (2u)
#define FLS_PHASE_3                 (3u)
#define FLS_PHASE_4                 (4u)
#define FLS_PHASE_5                 (5u)
#define FLS_PHASE_6                 (6u)
#define FLS_PHASE_7                 (7u)
#define FLS_PHASE_8                 (8u)
#define FLS_PHASE_9                 (9u)

/* data slot select */
#define FLS_SIDE_A                  ((uint16_t)0x0000)
#define FLS_SIDE_B                  ((uint16_t)0x0001)

/* DVC code length  */
#define FLS_DVC_SIZE                (16uL)

/* L_Toolbox_MemCmp return value */
#define FLS_TOOL_CMP_OK             (0uL)
#define FLS_TOOL_CMP_NG             (1uL)

/* g_get_resource_flg value */
#define FLS_VAL_2                   (0x02u)
#define FLS_VAL_1                   (0x01u)
#define FLS_VAL_0                   (0x00u)

/* for NVM Data Warning information */

/* NVM Warn Data buffer write position pointer initialize */
#define NVM_WARN_WRPT_INIT          ((uint8_t)0x0)

/* Flash memory status */
#define FLS_GC_INIT_VAL             (0uL)
#define FLS_GC_NORMAL               (1uL)
#define FLS_GC_DURING_1             (2uL)
#define FLS_GC_DURING_2             (3uL)
#define FLS_GC_FORMAT_ERR           (4uL)
#define FLS_GC_ALL_BLANK            (5uL)

/* Block information flag */
#define FLS_DF_BLOCK_TOP            (0uL)
#define FLS_DF_NBEE                 (0uL)
#define FLS_DF_NBES                 (1uL)
#define FLS_DF_TRANSFER             (2uL)
#define FLS_DF_USE                  (3uL)

/* Number of special physical blocks */
#define FLS_RESERVE_BLOCK_NUM       (1uL)
#define FLS_INDEF_BLOCK_NUM         (1uL)

/* One flag size */
#define FLS_FLAG_SIZE               (4uL)
/* Block information flag write value */
#define FLS_FLAG_VALUE              (0xFFFFFFFFuL)

/* Erased flag after power on */
#define FLS_ERASED                  (uint8_t)(0x00u)
#define FLS_NOT_ERASED              (uint8_t)(0x01u)

/* Number of data map entries */
#define FLS_DATA_MAP_SIZE           (256uL)

/* Number of logical blocks included in one physical block */
#define FLS_LBLOCK_NUM_PER_PBLOCK   (30uL)
/* One program tag size */
#define FLS_TAG_SIZE                (sizeof(program_tag_t))
/* All block information flag size */
#define FLS_TOTAL_BLOCK_INFO_SIZE   (FLS_FLAG_SIZE * 4uL)

/* Check result */
#define FLS_FOUND                   (1uL)
#define FLS_NOT_FOUND               (0uL)

/* invalid value */
#define FLS_INVALID_DS_TYPE         ((uint8_t)0xFFu)
#define FLS_INVALID_SLOT_NUM        ((uint8_t)0xFFu)
#define FLS_INVALID_BLOCK_IDX       ((uint16_t)0xFFFFu)

/* Comparison result */
#define FLS_CMP_SMALL               ((uint8_t)0x01u)
#define FLS_CMP_LARGE               ((uint8_t)0x02u)
#define FLS_CMP_EQUAL               ((uint8_t)0x03u)

/* next process */
#define FLS_NEXT_DATA               ((uint8_t)0x00u)
#define FLS_NEXT_PBLOCK             ((uint8_t)0x01u)
#define FLS_READ_ONLY_NEXT_PBLOCK   ((uint8_t)0x02u)

/* TAG status */
#define FLS_TAG_VALID               ((uint8_t)0x00u)
#define FLS_TAG_INVALID             ((uint8_t)0x01u)
#define FLS_TAG_BLANK               ((uint8_t)0x02u)
#define FLS_TAG_ERROR               ((uint8_t)0x03u)

/* Dummy context id */
#define FLS_DUMMY_CTX_ID            ((r_icumif_job_id_t)0x1000u)
/* Dummy lock_handle */
#define FLS_DUMMY_LOCK_HANDLE       L_FLS_DUMMY_LOCK_HANDLE

/* partition information related definition */
#define FLS_PINFO_SIZE              (L_FLS_ERASE_BLOCK_SIZE / 2uL)
#define FLS_PINFO_OFFSET_A          (L_FLS_ERASE_BLOCK_SIZE)
#define FLS_PINFO_OFFSET_B          (FLS_PINFO_OFFSET_A - FLS_PINFO_SIZE)
#define FLS_PINFO_BLOCK_SIZE        (16uL)
#define FLS_PINFO_ELEMENT_SIZE      (4uL)
#define FLS_PINFO_ELEMENT_NUM       (FLS_PINFO_BLOCK_SIZE / FLS_PINFO_ELEMENT_SIZE)
#define FLS_PINFO_NB_BLOCKS         (FLS_PINFO_SIZE / FLS_PINFO_BLOCK_SIZE)
#define FLS_PINFO_ACTIVE_OFFSET     (4uL)
#define FLS_PINFO_REVOKE_OFFSET     (8uL)
#define FLS_PINFO_INVALID_OFFSET    (4uL)
#define FLS_PINFO_TAG               (0xAA55AA55uL)

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Type definition                                                                    */
/**************************************************************************************/

/* type of Data TAG */
typedef uint32_t program_tag_t;

/* type of garbage collection status */
typedef uint32_t gc_state_t;

/* type of DF flags */
typedef uint32_t df_flag_t;

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* element of data-set */
typedef struct data_set
{
    /* number of slots in data-set */
    uint8_t         nb_slots;
    /* number of blocks per slot   */
    uint16_t        nb_blocks_per_slot;
    /* number of copies on flash  */
    uint16_t        nb_flash_copies;
} data_set_t;

/* status of data-set  */
typedef struct data_status
{
    /* data set type */
    uint8_t     ds_type;

    /* slot number */
    uint8_t     slot_num;

    /* data size */
    uint16_t    data_size;

    /* update counter */
    uint32_t    counter;

    /*  result of dvc verification (FLS_CHK_OK or FLS_CHK_NG) */
    uint8_t     dvc_result;

    /* result of flash operation   (ER_MM_FLS_xxxx)           */
    uint32_t    flash_err;
} data_status_t;

/* property of data-map */
typedef struct fls_data_map
{
    uint8_t     ds_type;
    uint8_t     slot_num;
    uint16_t    block_idx;
    uint32_t    update_counter;
} fls_data_map_t;

/* Data area header */
typedef struct fls_data_area_header
{
    dvc_t       dvc;
    uint8_t     ds_type;
    uint8_t     slot_num;
    uint16_t    data_size;
    uint32_t    update_counter;
} fls_data_area_header_t;

/* Data area header pointer */
typedef union fls_data_area_pointer
{
    fls_data_area_header_t *p_header;
    uint8_t                *p_u8;
} fls_data_area_pointer_t;

/* Block information */
typedef struct fls_block_info
{
    /* Erase end flag */
    uint32_t    nbee;
    /* Erase start flag */
    uint32_t    nbes;
    /* GC flag */
    uint32_t    transfer;
    /* unused */
    uint32_t    use;
} fls_block_info_t;

/* Flag or TAG pointer */
typedef union fls_data_u32_pointer
{
    uint32_t    *p_u32;
    uint8_t     *p_u8;
    uint32_t    u32;
} fls_data_u32_pointer_t;

/* fls configuration pointer */
typedef union fls_cfg_pointer
{
    sys_cfg_fls_t   *p_cfg_fls;
    uint32_t        *p_u32;
} fls_cfg_pointer_t;

/**************************************************************************************/
/* Exported global variables (extern).                                                       */
/**************************************************************************************/
extern uint8_t g_nvm_data_warning;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
/* private function */
void     mm_FLS_ClearMap(void);
void     mm_FLS_get_DataSet_size(const uint8_t ds_type, uint16_t *p_size);
uint32_t mm_FLS_ConvertionError(const uint32_t err);

/* NVM Data Warning */
uint32_t mm_FLS_SetNVMWarnData(const uint8_t ds_type, const uint8_t slot_num);
void     mm_FLS_InitNVMWarnData(void);
void     mm_FLS_CheckDataSetMapping(const uint8_t ds_type, uint8_t *p_result);

#endif /* #if !defined(ICUM_MM_FLASH_H) */
