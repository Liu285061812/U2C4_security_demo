/******************************************************************************/
/* Component Name   s_04_Common source file                                   */
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

/**********************************************************************************
* Include
***********************************************************************************/
/* For service result. */
#include "r_icumif_api.h"
/* For variable typedef. */
#include "renesas_types.h"
/* For the common error codes. */
#include "icum_common_pub.h"
/* For the error codes of info driver. */
#include "icum_d_info_pub.h"
/* For change section. */
#include "icum_d_misc_pub.h"
#include "icum_mm_flash_pub.h"
#include "icum_d_cry_aes_pub.h"
/* For the error codes of PRNG manager. */
#include "icum_ml_prng_pub.h"
/* For the error codes of hardware resource manager. */
#include "icum_ml_abtr_api_pub.h"
/* For call functions and error codes of flash manager. */
#include MM_FLASH_PUBLIC_H
/* For the error codes of key manager. */
#include "icum_mh_key_pub.h"
/* For the error codes of system/lifecycle manager. */
#include "icum_mh_sysinfo_pub.h"
/* For the error codes of ISSD. */
#include "icum_k_issd_pub.h"
/* For the error codes of service 04. */
#include "icum_s_04_common.h"

/**********************************************************************************
* Define
***********************************************************************************/
/* Total number of execution result types */
#define S04_NB_ERR 49uL

/**********************************************************************************
* Structure
***********************************************************************************/
/* Structure for result conversion array */
typedef struct
{
    volatile r_icumif_service_result_t service_result;
    uint32_t exe_result;
} convert_result_t;

/**********************************************************************************
* Variable
***********************************************************************************/

/**********************************************************************************
* Const data
***********************************************************************************/

CHANGE_SEC(rodata, ".icum_service_const_ram")

/* Result conversion array */
static const convert_result_t s_04_convert_result[S04_NB_ERR] =
{
    /* Normal operations */
    {SERV_OK,                     ER_OK                       },
    {SERV_IN_PROCESS,             ER_IN_PROCESS               },
    {SERV_IN_PROCESS,             ER_MM_FLS_IN_PROGRESS       },
    {SERV_PENDING,                ER_PENDING                  },
    {SERV_PENDING,                ER_ML_ABTR_EMPTY            },
    /* Input errors */
    {SERV_PRM_ERROR,              ER_PARAM                    },
    {SERV_CR_VERIFY_FAIL,         ER_S_04_CR_VERIFY_FAIL      },
    {SERV_SRC_MEM_ERROR,          ER_MM_FLS_MEM_SOURCE        },
    {SERV_SRC_MEM_ERROR,          ER_S_04_SRC_MEM             },
    {SERV_DEST_MEM_ERROR,         ER_MM_FLS_MEM_DEST          },
    {SERV_DEST_MEM_ERROR,         ER_S_04_DEST_MEM            },
    {SERV_NVM_SIZE_ERROR,         ER_MM_FLS_SIZE              },
    {SERV_NVM_UNAVAILABLE,        ER_MM_FLS_UNAVAILABLE       },
    {SERV_NVM_UNAVAILABLE,        ER_MH_KEY_NVM_UNAVAILABLE   },
    /* Incorrect procedure */
    {SERV_UNAVAILABLE,            ER_K_ISSD_MODE_REJECT       },
    {SERV_UNAVAILABLE,            ER_S_04_UNAVAILABLE         },
    {SERV_SEQUENCE_ERROR,         ER_S_04_SEQUENCE            },
    {SERV_KEY_SLOT_EMPTY,         ER_MH_KEY_SLOT_EMPTY        },
    {SERV_NO_INIT_RNG,            ER_ML_PRNG_STATUS           },
    /* Infrequent errors */
    {SERV_NEED_RESEED,            ER_ML_PRNG_RESEED           },
    {SERV_NVM_PROG_ERROR,         ER_MM_FLS_PROG              },
    /* Hardware errors */
    {SERV_DATA_INVALID,           ER_MH_KEY_DATA_INVALID      },
    {SERV_NVM_SYS_ERROR,          ER_MM_FLS_SYS               },
    {SERV_NVM_SYS_ERROR,          ER_MH_KEY_NVM_SYS           },
    {SERV_NVM_SYS_ERROR_CRITICAL, ER_MM_FLS_SYS_CRITICAL      },
    {SERV_NVM_SYS_ERROR_CRITICAL, ER_MH_KEY_NVM_SYS_CRITICAL  },
    {SERV_AES_TIMEOUT_ERROR,      ER_ML_PRNG_TIMEOUT          },
    {SERV_AES_TIMEOUT_ERROR,      ER_D_AES_TIMEOUT_ERROR      },
    {SERV_AES_TIMEOUT_ERROR,      ER_MH_KEY_AES_TIMEOUT_ERROR },
    /* System errors */
    {SERV_SYS_ERROR,              ER_D_INFO_INVALID_STATE     },
    {SERV_SYS_ERROR,              ER_ML_PRNG_PARAMETER        },
    {SERV_SYS_ERROR,              ER_ML_PRNG_AES_PRM          },
    {SERV_SYS_ERROR,              ER_PARAM                    },
    {SERV_SYS_ERROR,              ER_D_AES_ALIGN_ERROR        },
    {SERV_SYS_ERROR,              ER_ML_ABTR_NOT_RELEASED     },
    {SERV_SYS_ERROR,              ER_ML_ABTR_INVALID_ID       },
    {SERV_SYS_ERROR,              ER_ML_ABTR_NOT_INITIALIZED  },
    {SERV_SYS_ERROR,              ER_ML_ABTR_TIMER_SET_FAILURE},
    {SERV_SYS_ERROR,              ER_ML_ABTR_SYS_ERROR        },
    {SERV_SYS_ERROR,              ER_MH_KEY_IN_USE            },
    {SERV_SYS_ERROR,              ER_MH_KEY_SYS_ERROR         },
    {SERV_SYS_ERROR,              ER_MH_KEY_NVM_SIZE          },
    {SERV_SYS_ERROR,              ER_MH_KEY_SRC_MEM_ERROR     },
    {SERV_SYS_ERROR,              ER_MH_KEY_INVALID_KEY_GROUP },
    {SERV_SYS_ERROR,              ER_MH_KEY_INVALID_KEY_INDEX },
    {SERV_SYS_ERROR,              ER_MH_KEY_NOT_USABLE        },
    {SERV_SYS_ERROR,              ER_MH_KEY_AES_PRM_ERROR     },
    {SERV_SYS_ERROR,              ER_MH_SYSINFO_SYS_ERROR     },
    {SERV_SYS_ERROR,              ER_S_04_SYS                 }
};

/**********************************************************************************
* Function
***********************************************************************************/

CHANGE_SEC(text, ".icum_service_ram")

/***************************************************************************************
* Function ID   : [ICUMFW_CD_S_04_FLASH_PROG_0023]:[ICUMFW_UD_S_04_FLASH_PROG_0023]
* Function Name : s_04_ConvertError
* Description   : Convert execution result to service result.
* Arguments     : p_service_result
*                 exe_result
* Return Value  : ER_OK
***************************************************************************************/
uint32_t s_04_ConvertError(volatile r_icumif_service_result_t *p_service_result, uint32_t exe_result)
{
    /* Index of result conversion array. */
    uint32_t err_index;

    /* Process 1. Search the execution result from the result conversion array. */
    for (err_index = 0uL; S04_NB_ERR > err_index; err_index++)
    {
        if (exe_result != s_04_convert_result[err_index].exe_result)
        {
            /* Process 1-1. The execution result do not match. */
            /* Do nothing */
        }
        else
        {
            /* Process 1-2. Output the service result because the execution results match. */
            *p_service_result = s_04_convert_result[err_index].service_result;
            break;
        }
    }
    /* Process 2. Check the search results. */
    if (S04_NB_ERR > err_index)
    {
        /* Process 2-1. Found a execution result. */
        /* Do nothing */
    }
    else
    {
        /* Process 2-2. Output a system error because the execution result was not found. */
        *p_service_result = SERV_SYS_ERROR;
    }

    return ER_OK;
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)
