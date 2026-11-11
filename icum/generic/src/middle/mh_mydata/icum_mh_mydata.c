/******************************************************************************/
/* Component Name   MyData MNG                                                */
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

/* Header files                     */
#include "renesas_types.h"
#include "icum_common_pub.h"
#include "r_icumif_api.h"
#include "icum_ml_cfg_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mh_mydata_pub.h"

#include "icum_mh_mydata.h"

#pragma ghs reference get_mydata
#pragma ghs reference update_mydata
/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_MyDataMNG_0001]:[ICUMFW_UD_MH_MyDataMNG_0001]
* Function name: MH_MYDATA_Init
* Description  : Init processing.
* Arguments    : none
* Return Value : ER_OK : Success.
*                ER_MH_MYDATA_SYSCFG_FAILURE  : Get MyData config failed
*                ER_MH_MYDATA_DECLARE_FAILURE : Data set declaration failed
****************************************************************************************/
uint32_t MH_MYDATA_Init(void)
{
    sys_cfg_mydata_set_t mydata_set;
    uint32_t err;
    uint32_t rtn = ER_OK;
    uint32_t mydata_size;
    uint32_t mydata_blocks;
    
    mydata_set.nb_items = 0U;
    mydata_set.item_size_in_bytes = 0U;
    
    /* 1. Get MyData data set */
    err = ML_CFG_GetSys(ML_CFG_SYS_MYDATA, (uint32_t *)&mydata_set);
    /* 1-1. Success */
    if (ER_OK == err)
    {
        /* 1-1-1. Both items and size are greater than zero */
        if ((MYDATA_SLOT_ITEMS_MIN < (uint32_t)mydata_set.nb_items) &&
            (MYDATA_SLOT_SIZE_MIN < (uint32_t)mydata_set.item_size_in_bytes))
        {
            /* Calculate number of MyData blocks */
            mydata_size = (uint32_t)(mydata_set.item_size_in_bytes) + FLS_DATA_SLOT_MGMT_SIZE;
            /* 1-1-1-1. Remainder is greater than zero */
            if (MYDATA_BLOCKS_MIN < (mydata_size % FLS_DF_BLOCK_SIZE))
            {
                /* Round up number of blocks */
                mydata_blocks = (mydata_size / FLS_DF_BLOCK_SIZE) + MYDATA_BLOCKS_ROUND_UP;
            }
            /* 1-1-1-2. Remainder is  zero */
            else
            {
                mydata_blocks = mydata_size / FLS_DF_BLOCK_SIZE;
            }
            /* 2. MyData data set declare */
            err = MM_FLS_DeclareDataSet(FLS_DS_MYDATA, mydata_set.nb_items,
                                        (uint16_t)mydata_blocks, FLS_DS_NB_COPIES);
            /* 2-1. Failure */
            if (ER_OK != err)
            {
                rtn = ER_MH_MYDATA_DECLARE_FAILURE;
            }
            else
            {
                /* Success declare data set */
            }
        }
        /* 1-1-2. Other(Either items or size is zero) */
        else
        {
            /* Not abnormal */
        }
    }
    /* 1-2. Failure(Get sysconfig error) */
    else
    {
        rtn = ER_MH_MYDATA_SYSCFG_FAILURE;
    }
   
    return (rtn);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_MyDataMNG_0002]:[ICUMFW_UD_MH_MyDataMNG_0002]
* Function name: MH_MYDATA_Get
* Description  : Get MyData
* Arguments    : (IN)  mydata_idx : Data slot index
*                      data_size  : Data size
*                (OUT) *p_mydata  : Data storage area
* Return Value : ER_OK                          : Success.
*                ER_PARAM                       : Parameter error
*                ER_PENDING                     : Pending
*                ER_MH_MYDATA_NO_DATA_SET       : No data set declared
*                ER_MH_MYDATA_NOT_MAP_DATA_SLOT : No data set declared
*                ER_MH_MYDATA_INVALID_IDX       : Invalid data set index
*                ER_MH_MYDATA_SIZE              : Invalid data size
*                ER_MH_MYDATA_BLANK             : Double sided blank
*                ER_MH_MYDATA_UNAVAILABLE       : Unable to use Secure DF
*                ER_MH_MYDATA_MEM_SOURCE        : Abnormality of data slot address
*                ER_MH_MYDATA_SYS               : Abnormality of flash operation
*                ER_MH_MYDATA_SYS_CRITICAL      : FACI abnormality
*                ER_MH_MYDATA_INVALID           : Damaged both sides of data slot
*                ER_MH_MYDATA_AES_UNAVAILABLE   : Not acquire AES usage rights
*                ER_MH_MYDATA_AES_TIME_OUT      : AES timeout occurred
*                ER_MH_MYDATA_AES_PRM_ERROR     : Abnormality during DVC calculation
*                ER_MH_MYDATA_SYS_ERROR         : Other error
****************************************************************************************/
uint32_t MH_MYDATA_Get(uint8_t mydata_idx, uint8_t *p_mydata, uint16_t data_size)
{
    sys_cfg_mydata_set_t mydata_set;
    uint32_t mydata_size;
    uint32_t mydata_blocks;
    uint32_t rtn = ER_OK;
    uint32_t err;
    uint16_t slot_size;
    uint32_t i;
    
    mydata_set.nb_items = 0U;
    mydata_set.item_size_in_bytes = 0U;
    
    /* 1. Get MyData data set */
    err = ML_CFG_GetSys(ML_CFG_SYS_MYDATA, (uint32_t *)&mydata_set);
    /* 1-1. Success */
    if (ER_OK == err)
    {
        /* 1-1-1. Both items and size are greater than zero */
        if ((MYDATA_SLOT_ITEMS_MIN < (uint32_t)mydata_set.nb_items) &&
            (MYDATA_SLOT_SIZE_MIN < (uint32_t)mydata_set.item_size_in_bytes))
        {
            /* Calculate number of MyData blocks */
            mydata_size = (uint32_t)mydata_set.item_size_in_bytes + FLS_DATA_SLOT_MGMT_SIZE;
            /* 1-1-1-1. Remainder is greater than zero */
            if (MYDATA_BLOCKS_MIN < (mydata_size % FLS_DF_BLOCK_SIZE))
            {
                /* Round up number of blocks */
                mydata_blocks = (mydata_size / FLS_DF_BLOCK_SIZE) + MYDATA_BLOCKS_ROUND_UP;
            }
            /* 1-1-1-2. Remainder is  zero */
            else
            {
                mydata_blocks = mydata_size / FLS_DF_BLOCK_SIZE;
            }
        }
        /* 1-1-2. Other(Either items or size is zero) */
        else
        {
            rtn = ER_MH_MYDATA_NO_DATA_SET;
        }
    }
    /* 1-2. Failure(Get sysconfig error) */
    else
    {
        /* Convert exec result to return value */
        rtn = mh_MYDATA_ConvertErr(err);
    }
    
    if (ER_OK == rtn)
    {
        /* 2. Parameter mydata_idx not less than items */
        if ((uint32_t)mydata_idx >= (uint32_t)mydata_set.nb_items)
        {
            rtn = ER_MH_MYDATA_INVALID_IDX;
        }
        /* Parameter mydata_idx is OK */
        /* 3. Parameter data_size is larger than size or zero */
        else if (((uint32_t)data_size > (uint32_t)mydata_set.item_size_in_bytes) ||
                 (MYDATA_SLOT_SIZE_MIN == (uint32_t)data_size))
        {
            rtn = ER_MH_MYDATA_SIZE;
        }
        else
        {
            /* 4. Calculate slot size in bytes */
            slot_size = (uint16_t)(mydata_blocks * FLS_DF_BLOCK_SIZE);

            /* 5. Read MyData data */
            err = MM_FLS_Read(FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE,
                                FLS_DS_MYDATA, mydata_idx, slot_size, p_mydata);

            /* 6. Clear other than data area */
            for (i = 0uL; i < FLS_DATA_SLOT_MGMT_SIZE; i++)
            {
                p_mydata[i] = MYDATA_DATA_CLR;
            }
            for (i = FLS_DATA_SLOT_MGMT_SIZE + (uint32_t)data_size; i < (uint32_t)slot_size; i++)
            {
                p_mydata[i] = MYDATA_DATA_CLR;
            }

            /* 7. Convert exec result to return value */
            rtn = mh_MYDATA_ConvertErr(err);
        }
    }
    else
    {
        /* Do nothing */
    }

    return (rtn);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_MyDataMNG_0003]:[ICUMFW_UD_MH_MyDataMNG_0003]
* Function name: MH_MYDATA_Update
* Description  : Update MyData
* Arguments    : (IN) mydata_idx : Data slot index
*                     *p_mydata  : Update data storage area
*                     data_size  : Data size
*                     job_id     : Job ID
* Return Value : ER_OK                          : Success.
*                ER_PARAM                       : Parameter error
*                ER_PENDING                     : Pending
*                ER_IN_PROCESS                  : Flash writing is continuing
*                ER_MH_MYDATA_NO_DATA_SET       : No data set declared
*                ER_MH_MYDATA_NOT_MAP_DATA_SLOT : No data set declared
*                ER_MH_MYDATA_INVALID_IDX       : Invalid data set index
*                ER_MH_MYDATA_SIZE              : Invalid data size
*                ER_MH_MYDATA_UNAVAILABLE       : Unable to use Secure DF
*                ER_MH_MYDATA_MEM_DEST          : Error in data storage address
*                ER_MH_MYDATA_SYS               : Abnormality of flash operation
*                ER_MH_MYDATA_SYS_CRITICAL      : FACI abnormality
*                ER_MH_MYDATA_PROG              : Flash write error
*                ER_MH_MYDATA_INVALID           : Damaged both sides of data slot
*                ER_MH_MYDATA_AES_UNAVAILABLE   : Not acquire AES usage rights
*                ER_MH_MYDATA_AES_TIME_OUT      : AES timeout occurred
*                ER_MH_MYDATA_AES_PRM_ERROR     : Abnormality during DVC calculation
*                ER_MH_MYDATA_SYS_ERROR         : Other error
****************************************************************************************/
uint32_t MH_MYDATA_Update(uint8_t mydata_idx, uint8_t *p_mydata, uint16_t data_size,
                                              r_icumif_job_id_t job_id)
{
    sys_cfg_mydata_set_t mydata_set;
    uint32_t mydata_size;
    uint32_t mydata_blocks;
    uint32_t rtn = ER_OK;
    uint32_t err;
    uint16_t slot_size;
    uint32_t i;
    
    mydata_set.nb_items = 0U;
    mydata_set.item_size_in_bytes = 0U;
    
    /* 1. Parameter job_id is zero */
    if (MYDATA_JOB_ID_MIN == (uint32_t)job_id)
    {
        rtn = ER_PARAM;
    }
    else
    {
        /* Parameter job_id is OK */
        /* 2. Get MyData data set */
        err = ML_CFG_GetSys(ML_CFG_SYS_MYDATA, (uint32_t *)&mydata_set);
        /* 2-1. Success */
        if (ER_OK == err)
        {
            /* 2-1-1. Both items and size are greater than zero */
            if ((MYDATA_SLOT_ITEMS_MIN < (uint32_t)mydata_set.nb_items) &&
                (MYDATA_SLOT_SIZE_MIN < (uint32_t)mydata_set.item_size_in_bytes))
            {
                /* Calculate number of MyData blocks */
                mydata_size = (uint32_t)mydata_set.item_size_in_bytes + FLS_DATA_SLOT_MGMT_SIZE;
                /* 2-1-1-1. Remainder is greater than zero */
                if (MYDATA_BLOCKS_MIN < (mydata_size % FLS_DF_BLOCK_SIZE))
                {
                    /* Round up number of blocks */
                    mydata_blocks = (mydata_size / FLS_DF_BLOCK_SIZE) + MYDATA_BLOCKS_ROUND_UP;
                }
                /* 2-1-1-2. Remainder is  zero */
                else
                {
                    mydata_blocks = mydata_size / FLS_DF_BLOCK_SIZE;
                }
            }
            /* 2-1-2. Other(Either items or size is zero) */
            else
            {
                rtn = ER_MH_MYDATA_NO_DATA_SET;
            }
        }
        /* 2-2. Failure(Get sysconfig error) */
        else
        {
            /* Convert exec result to return value */
            rtn = mh_MYDATA_ConvertErr(err);
        }
    }
    
    if (ER_OK == rtn)
    {
        /* 3. Parameter mydata_idx not less than items */
        if ((uint32_t)mydata_idx >= (uint32_t)mydata_set.nb_items)
        {
            rtn = ER_MH_MYDATA_INVALID_IDX;
        }
        else
        {
            /* Parameter mydata_idx is OK */
            /* 4. Parameter data_size is larger than size or zero */
            if (((uint32_t)data_size > (uint32_t)mydata_set.item_size_in_bytes) ||
                (MYDATA_SLOT_SIZE_MIN == (uint32_t)data_size))
            {
                rtn = ER_MH_MYDATA_SIZE;
            }
            else
            {
                /* Parameter data_size is OK */
            }
        }
    }
    else
    {
        /* Do nothing */
    }

    if (ER_OK == rtn)
    {
        /* 5. Calculate slot size in bytes */
        slot_size = (uint16_t)(mydata_blocks * FLS_DF_BLOCK_SIZE);
        
        /* 7. Clear after the data area, before the info area */
        for (i = (FLS_DATA_SLOT_MGMT_SIZE + (uint32_t)data_size); i < (uint32_t)slot_size; i++)
        {
            p_mydata[i] = MYDATA_DATA_CLR;
        }

        /* 8. Write MyData data */
        err = MM_FLS_Write(job_id, FLS_SELECT_ICUM_DF, FLS_SELECT_SECURE,
                            FLS_DS_MYDATA, mydata_idx, slot_size, p_mydata);
        
        /* 9. Update finish check */
        if ((ER_IN_PROCESS != err) &&
            (ER_MM_FLS_IN_PROGRESS != err))
        {
            /* 9-1. Clear after data area */
            for (i = 0uL; i < FLS_DATA_SLOT_MGMT_SIZE; i++)
            {
                p_mydata[i] = MYDATA_DATA_CLR;
            }
        }
        else
        {
            /* ER_IN_PROCESS or ER_MM_FLS_IN_PROGRESS */
        }
        
        /* 10. Convert exec result to return value */
        rtn = mh_MYDATA_ConvertErr(err);
    }
    else
    {
        /* Do nothing */
    }
    
    return (rtn);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_MyDataMNG_0004]:[ICUMFW_UD_MH_MyDataMNG_0004]
* Function name: mh_MYDATA_ConvertErr
* Description  : Convert error to MyData error
* Arguments    : (IN) conv_err : Source error
* Return Value : ER_OK                          : Success.
*                ER_PARAM                       : Parameter error
*                ER_PENDING                     : Pending
*                ER_IN_PROCESS                  : Flash writing is continuing
*                ER_MH_MYDATA_UNAVAILABLE       : Unable to use Secure DF
*                ER_MH_MYDATA_MEM_SOURCE        : Abnormality of data slot address
*                ER_MH_MYDATA_MEM_DEST          : Error in data storage address
*                ER_MH_MYDATA_SYS               : Abnormality of flash operation
*                ER_MH_MYDATA_SYS_CRITICAL      : FACI abnormality
*                ER_MH_MYDATA_PROG              : Flash write error
*                ER_MH_MYDATA_INVALID           : Damaged both sides of data slot
*                ER_MH_MYDATA_BLANK             : Double sided blank
*                ER_MH_MYDATA_SIZE              : Invalid data size
*                ER_MH_MYDATA_AES_UNAVAILABLE   : Not acquire AES usage rights
*                ER_MH_MYDATA_AES_TIME_OUT      : AES timeout occurred
*                ER_MH_MYDATA_AES_PRM_ERROR     : Abnormality during DVC calculation
*                ER_MH_MYDATA_NOT_MAP_DATA_SLOT : No data set declared
*                ER_MH_MYDATA_SYS_ERROR         : Other error
****************************************************************************************/
static uint32_t mh_MYDATA_ConvertErr(uint32_t conv_err)
{
    uint32_t rtn;
    
    switch(conv_err)
    {
        case ER_OK:
            rtn = ER_OK;
            break;
        
        case ER_PARAM:
            rtn = ER_PARAM;
            break;
        
        case ER_PENDING:
            rtn = ER_PENDING;
            break;
        
        case ER_IN_PROCESS:
            rtn = ER_IN_PROCESS;
            break;
        
        case ER_MM_FLS_IN_PROGRESS:
            rtn = ER_IN_PROCESS;
            break;
        
        case ER_MM_FLS_UNAVAILABLE:
            rtn = ER_MH_MYDATA_UNAVAILABLE;
            break;
        
        case ER_MM_FLS_MEM_SOURCE:
            rtn = ER_MH_MYDATA_MEM_SOURCE;
            break;
        
        case ER_MM_FLS_MEM_DEST:
            rtn = ER_MH_MYDATA_MEM_DEST;
            break;
        
        case ER_MM_FLS_SYS:
            rtn = ER_MH_MYDATA_SYS;
            break;
        
        case ER_MM_FLS_SYS_CRITICAL:
            rtn = ER_MH_MYDATA_SYS_CRITICAL;
            break;
        
        case ER_MM_FLS_PROG:
            rtn = ER_MH_MYDATA_PROG;
            break;
        
        case ER_MM_FLS_DATA_INVALID:
            rtn = ER_MH_MYDATA_INVALID;
            break;
        
        case ER_MM_FLS_DATA_BLANK:
            rtn = ER_MH_MYDATA_BLANK;
            break;
        
        case ER_MM_FLS_SIZE:
            rtn = ER_MH_MYDATA_SIZE ;
            break;
        
        case ER_MM_FLS_AES_UNAVAILABLE:
            rtn = ER_MH_MYDATA_AES_UNAVAILABLE;
            break;
        
        case ER_MM_FLS_AES_TIME_OUT:
            rtn = ER_MH_MYDATA_AES_TIME_OUT;
            break;
        
        case ER_MM_FLS_AES_PRM_ERROR:
            rtn = ER_MH_MYDATA_AES_PRM_ERROR;
            break;
        
        default:
            rtn = ER_MH_MYDATA_SYS_ERROR;
            break;
    }
    
    return (rtn);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_MyDataMNG_0005]:[ICUMFW_UD_MH_MyDataMNG_0005]
* Function name: mh_MYDATA_ConvertServ
* Description  : Convert MyData error to service result
* Arguments    : (IN) conv_err : Source error
* Return Value : SERV_OK                : Success.
*                SERV_PRM_ERROR         : Parameter error
*                SERV_PENDING           : Pending
*                SERV_IN_PROCESS        : Flash writing is continuing
*                SERV_MYDATA_SET_EMPTY  : No data set declared
*                SERV_MYDATA_SLOT_EMPTY : Double sided blank
*                SERV_MYDATA_INVALID_IDX: Invalid data set index
*                SERV_MYDATA_SIZE_ERR   : Invalid data size
*                SERV_NVM_UNAVAILABLE   : Unable to use Secure DF
*                SERV_SRC_MEM_ERROR     : Abnormality of data slot address
*                SERV_DEST_MEM_ERROR    : Error in data storage address
*                SERV_NVM_SYS_ERROR     : Abnormality of flash operation
*                SERV_NVM_SYS_ERROR_CRITICAL: FACI abnormality
*                SERV_NVM_PROG_ERROR    : Flash write error
*                SERV_SYS_ERROR         : Not acquire AES usage rights
*                SERV_AES_TIMEOUT_ERROR : AES timeout occurred
*                SERV_AES_PRM_ERROR     : Abnormality during DVC calculation
****************************************************************************************/
static r_icumif_service_result_t mh_MYDATA_ConvertServ(uint32_t conv_err)
{
    r_icumif_service_result_t rtn;
    
    switch(conv_err)
    {
        case ER_OK:
            rtn = SERV_OK;
            break;
        
        case ER_PARAM:
            rtn = SERV_PRM_ERROR;
            break;
        
        case ER_PENDING:
            rtn = SERV_PENDING;
            break;
        
        case ER_IN_PROCESS:
            rtn = SERV_IN_PROCESS;
            break;
        
        case ER_MH_MYDATA_NO_DATA_SET :
            rtn = SERV_MYDATA_SET_EMPTY;
            break;
        
        case ER_MH_MYDATA_NOT_MAP_DATA_SLOT:
            rtn = SERV_MYDATA_SET_EMPTY;
            break;
        
        case ER_MH_MYDATA_INVALID:
            rtn = SERV_MYDATA_SET_EMPTY;
            break;
        
        case ER_MH_MYDATA_BLANK:
            rtn = SERV_MYDATA_SLOT_EMPTY;
            break;
        
        case ER_MH_MYDATA_INVALID_IDX:
            rtn = SERV_MYDATA_INVALID_IDX;
            break;
        
        case ER_MH_MYDATA_SIZE:
            rtn = SERV_MYDATA_SIZE_ERR;
            break;
        
        case ER_MH_MYDATA_UNAVAILABLE:
            rtn = SERV_NVM_UNAVAILABLE;
            break;
        
        case ER_MH_MYDATA_MEM_SOURCE:
            rtn = SERV_SRC_MEM_ERROR;
            break;
        
        case ER_MH_MYDATA_MEM_DEST:
            rtn = SERV_DEST_MEM_ERROR;
            break;
        
        case ER_MH_MYDATA_SYS:
            rtn = SERV_NVM_SYS_ERROR;
            break;
        
        case ER_MH_MYDATA_SYS_CRITICAL:
            rtn = SERV_NVM_SYS_ERROR_CRITICAL;
            break;
        
        case ER_MH_MYDATA_PROG:
            rtn = SERV_NVM_PROG_ERROR;
            break;
        
        case ER_MH_MYDATA_AES_UNAVAILABLE:
            rtn = SERV_SYS_ERROR;
            break;
        
        case ER_MH_MYDATA_AES_TIME_OUT:
            rtn = SERV_AES_TIMEOUT_ERROR;
            break;
        
        case ER_MH_MYDATA_AES_PRM_ERROR:
            rtn = SERV_AES_PRM_ERROR;
            break;
        
        case ER_MH_MYDATA_SYS_ERROR:
            rtn = SERV_SYS_ERROR;
            break;
        
        default:
            rtn = SERV_SYS_ERROR;
            break;
    }
    
    return (rtn);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_MyDataMNG_0006]:[ICUMFW_UD_MH_MyDataMNG_0006]
* Function name: get_mydata
* Description  : Get MyData Wrapper API
* Arguments    : (IN)  mydata_idx : Data slot index
*                      data_size  : Data size
*                (OUT) *p_mydata  : Data storage area
* Return Value : SERV_OK                : Success.
*                SERV_PRM_ERROR         : Parameter error
*                SERV_MYDATA_SET_EMPTY  : No data set declared
*                SERV_MYDATA_INVALID_IDX: Invalid data set index
*                SERV_MYDATA_SIZE_ERR   : Invalid data size
*                SERV_MYDATA_SLOT_EMPTY : Double sided blank
*                SERV_NVM_UNAVAILABLE   : Unable to use Secure DF
*                SERV_SRC_MEM_ERROR     : Abnormality of data slot address
*                SERV_NVM_SYS_ERROR     : Abnormality of flash operation
*                SERV_NVM_SYS_ERROR_CRITICAL: FACI abnormality
*                SERV_SYS_ERROR         : Not acquire AES usage rights
*                SERV_AES_TIMEOUT_ERROR : AES timeout occurred
*                SERV_PENDING           : Pending
*                SERV_AES_PRM_ERROR     : Abnormality during DVC calculation
****************************************************************************************/
r_icumif_service_result_t get_mydata(uint8_t mydata_idx, uint8_t *p_mydata, uint16_t data_size)
{
    r_icumif_service_result_t rtn;
    uint32_t err;

    /* 1. Mydata get exec */
    err = MH_MYDATA_Get(mydata_idx, p_mydata, data_size);

    /* 2. Convert exec result to service result */
    rtn = mh_MYDATA_ConvertServ(err);
    
    return (rtn);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_MH_MyDataMNG_0007]:[ICUMFW_UD_MH_MyDataMNG_0007]
* Function name: update_mydata
* Description  : Update MyData Wrapper API
* Arguments    : (IN) mydata_idx : Data slot index
*                     *p_mydata  : Update data storage area
*                     data_size  : Data size
*                     job_id     : Job ID
* Return Value : SERV_OK                : Success.
*                SERV_PRM_ERROR         : Parameter error
*                SERV_PENDING           : Pending
*                SERV_IN_PROCESS        : Flash writing is continuing
*                SERV_MYDATA_SET_EMPTY  : No data set declared
*                SERV_MYDATA_INVALID_IDX: Invalid data set index
*                SERV_MYDATA_SIZE_ERR   : Invalid data size
*                SERV_NVM_UNAVAILABLE   : Unable to use Secure DF
*                SERV_DEST_MEM_ERROR    : Error in data storage address
*                SERV_NVM_SYS_ERROR     : Abnormality of flash operation
*                SERV_NVM_SYS_ERROR_CRITICAL: FACI abnormality
*                SERV_NVM_PROG_ERROR    : Flash write error
*                SERV_SYS_ERROR         : Not acquire AES usage rights
*                SERV_AES_TIMEOUT_ERROR : AES timeout occurred
*                SERV_AES_PRM_ERROR     : Abnormality during DVC calculation
****************************************************************************************/
r_icumif_service_result_t update_mydata(uint8_t mydata_idx, uint8_t *p_mydata, uint16_t data_size,
                                        r_icumif_job_id_t job_id)
{
    r_icumif_service_result_t rtn;
    uint32_t err;

    /* 1. Mydata update exec */
    err = MH_MYDATA_Update(mydata_idx, p_mydata, data_size, job_id);

    /* 2. Convert exec result to service result */
    rtn = mh_MYDATA_ConvertServ(err);

    return (rtn);
}
