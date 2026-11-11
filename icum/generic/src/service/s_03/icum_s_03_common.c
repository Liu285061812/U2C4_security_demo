/******************************************************************************/
/* Component Name   ICU-M Service                                             */
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
* Header files
***********************************************************************************/
#include "renesas_types.h"
#include "r_icumif_api.h"
#include "icum_common_pub.h"
#include "icum_d_comm_pub.h"
#include "icum_d_wdt_pub.h"
#include "icum_d_cr_pub.h"
#include "icum_d_cry_aes_pub.h"
#include "icum_l_toolbox_pub.h"
#include "icum_ml_prng_pub.h"
#include "icum_l_cry_pk_pub.h"
#include "icum_ml_abtr_api_pub.h"
#include "icum_ml_ctx_pub.h"
#include "icum_ml_cfg_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mh_key_pub.h"
#include "icum_k_issd_pub.h"

#include "icum_s_03_common.h"
#include "icum_service_common.h"

/**********************************************************************************
* Variable
***********************************************************************************/
#if defined(FSW_USE_CRAM)
#pragma ghs section bss = ".start.icum_cram.bss"
pkc_data_t g_work_bignum[3];
#pragma ghs section bss = default
#endif

/**********************************************************************************
* Local defines
**********************************************************************************/
#define NUMBER_OF_ERROR_TABLE (58)

/* Service and middle error conversion structure */
typedef struct {
    uint32_t                    internal_code;
    r_icumif_service_result_t   external_code;
} errcode_tuple_t;

/* Service 03 and middle layer error conversion table */
static const errcode_tuple_t s_03_errcode_tuples[NUMBER_OF_ERROR_TABLE] =
{
    /* Common error */
    {ER_OK,                         SERV_OK                 },
    {ER_PARAM,                      SERV_PRM_ERROR          },
    {ER_PENDING_ARB,                SERV_PENDING            },
    {ER_PENDING,                    SERV_PENDING            },
    {ER_IN_PROCESS,                 SERV_IN_PROCESS         },
    /* ER_MH_KEY */
    {ER_MH_KEY_SLOT_EMPTY,          SERV_KEY_SLOT_EMPTY     },
    {ER_MH_KEY_INVALID_KEY_GROUP,   SERV_INVALID_KEY_GROUP  },
    {ER_MH_KEY_INVALID_KEY_INDEX,   SERV_INVALID_KEY_INDEX  },
    {ER_MH_KEY_WRITE_PROTECTED,     SERV_KEY_WRITE_PROTECTED},
    {ER_MH_KEY_IN_USE,              SERV_KEY_NOT_USABLE     },
    {ER_MH_KEY_DATA_NOT_FOUND,      SERV_KEY_NOT_USABLE     },
    {ER_MH_KEY_WRITING,             SERV_PENDING            },
    {ER_MH_KEY_USER_OVER,           SERV_KEY_NOT_USABLE     },
    {ER_MH_KEY_NOT_USABLE,          SERV_KEY_NOT_USABLE     },
    {ER_MH_KEY_AES_TIMEOUT_ERROR,   SERV_AES_TIMEOUT_ERROR  },
    {ER_MH_KEY_DATA_INVALID,        SERV_DATA_INVALID       },
    {ER_MH_KEY_NVM_UNAVAILABLE,     SERV_NVM_UNAVAILABLE    },
    {ER_MH_KEY_NVM_SYS_CRITICAL,    SERV_NVM_SYS_ERROR_CRITICAL},
    {ER_MH_KEY_NVM_SYS,             SERV_NVM_SYS_ERROR      },
    {ER_MH_KEY_NVM_PROG,            SERV_NVM_PROG_ERROR     },
    {ER_MH_KEY_NVM_SIZE,            SERV_NVM_SIZE_ERROR     },
    {ER_MH_KEY_SRC_MEM_ERROR,       SERV_SRC_MEM_ERROR      },
    {ER_MH_KEY_DEST_MEM_ERROR,      SERV_DEST_MEM_ERROR     },
    {ER_MH_KEY_SYS_ERROR,           SERV_SYS_ERROR          },
    {ER_MH_KEY_AES_PRM_ERROR,       SERV_AES_PRM_ERROR      },
    {ER_MH_KEY_INVALID_CURVE_ID,    SERV_INVALID_CURVE_ID   },
    {ER_MH_KEY_UPDATE_ERROR,        SERV_KEY_UPDATE_ERROR   },
    /* ER_MH_CTX */
    {ER_ML_CTX_ALL_USE,             SERV_PENDING            },
    {ER_ML_CTX_NOT_FOUND,           SERV_PRM_ERROR          },
    /* Driver */
    {ER_L_HMAC_INVALID_ARG,         SERV_PRM_ERROR          },
    {ER_D_AES_ALIGN_ERROR,          SERV_AES_PRM_ERROR      },
    {ER_D_AES_TIMEOUT_ERROR,        SERV_CRYPT_ERROR        },
    /* ER_ML_ABTR */
    {ER_ML_ABTR_EMPTY,              SERV_PENDING            },
    {ER_ML_ABTR_NOT_RELEASED,       SERV_SYS_ERROR          },
    {ER_ML_ABTR_INVALID_ID,         SERV_SYS_ERROR          },
    {ER_ML_ABTR_TIMER_SET_FAILURE,  SERV_SYS_ERROR          },
    {ER_ML_ABTR_NOT_INITIALIZED,    SERV_SYS_ERROR          },
    {ER_ML_ABTR_SYS_ERROR,          SERV_SYS_ERROR          },
    /* ER_ML_PRNG */
    {ER_ML_PRNG_STATUS,             SERV_NO_INIT_RNG        },
    {ER_ML_PRNG_RESEED,             SERV_NEED_RESEED        },
    {ER_ML_PRNG_TIMEOUT,            SERV_AES_TIMEOUT_ERROR  },
    {ER_ML_PRNG_PARAMETER,          SERV_PRM_ERROR          },
    {ER_ML_PRNG_AES_PRM,            SERV_PRM_ERROR          },
    /* ER_L_PKC_RSA */
    {ER_L_RSA_SYS_ERROR,            SERV_SYS_ERROR          },
    {ER_L_RSA_ERROR,                SERV_CRYPT_ERROR        },
    {ER_L_RSA_INVALID_ARG,          SERV_PRM_ERROR          },
    {ER_L_RSA_LENGTHERR,            SERV_CRYPT_ERROR        },
    {ER_L_RSA_KEYGEN_RETRYOVER,     SERV_KEY_GENERATION_ERROR },
    {ER_L_RSA_RUNNING,              SERV_IN_PROCESS         },
    {ER_L_RSA_INVALID_PARM,         SERV_PRM_ERROR          },
    /* ER_L_PKC ECC */
    {ER_L_ECC_SYS_ERROR,            SERV_SYS_ERROR          },
    {ER_L_ECC_ERROR,                SERV_CRYPT_ERROR        },
    {ER_L_ECC_INVALID_ARG,          SERV_PRM_ERROR          },
    {ER_L_ECC_LENGTHERR,            SERV_CRYPT_ERROR        },
    {ER_L_ECC_RUNNING,              SERV_IN_PROCESS         },
    /* ER_L_PKC HASH */
    {ER_L_HASH_INVALID_ARG,         SERV_PRM_ERROR          },
    /* L_BIGNUM */
    {ER_L_BIGNUM_BUFFER_OVERFLOW,   SERV_PRM_ERROR          },
    {ER_L_BIGNUM_INVALID_ARG,       SERV_PRM_ERROR          },
};

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0015]:[ICUMFW_UD_S_03_PK_0015]
* Function name: s_03_GetInternalHashType
* Description  : Get hash type and size from hash primitive.
* Arguments    : (in) hash_primitive         : HASH Primitive
*                                               Range: HASH_PRIMITIVE_SHA2_224
*                                                      HASH_PRIMITIVE_SHA2_256
*                                                      HASH_PRIMITIVE_SHA2_384
*                                                      HASH_PRIMITIVE_SHA2_512
*                                                      HASH_PRIMITIVE_SHA2_512_224
*                                                      HASH_PRIMITIVE_SHA2_512_256
*                (out) *p_hash_type           : hash type
*                                               Range: E_SHA2_224
*                                                      E_SHA2_256
*                                                      E_SHA2_384
*                                                      E_SHA2_512
*                                                      E_SHA2_512_224
*                                                      E_SHA2_512_256
*                (out) *p_hash_size_in_bytes  : HASH Size address
*                                               Range: 0 - 0xFFFF(65535)
* Return Value : ER_OK
***************************************************************************************/
uint32_t s_03_GetInternalHashType(r_hash_primitive_t hash_primitive,
                                  hash_type_t *p_hash_type,
                                  uint16_t *p_hash_size_in_bytes)
{

    (void)L_HASH_GetInternalHashType(hash_primitive, p_hash_type, p_hash_size_in_bytes);

    return (ER_OK);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0016]:[ICUMFW_UD_S_03_PK_0016]
* Function name: s_03_GetBitCntFromDecimalStr
* Description  : get bit_length from decimal string.
*                     (decimal figure length) = (bit length) * log(2)
*                in below coding, (30103 / 100000) is used instead of log(2) (= 0.30102999...)
* Arguments    : (IN)       p_DecimalString
*                (OUT)      p_bitsMin
* Return Value : ER_OK
****************************************************************************************/
uint32_t s_03_GetBitCntFromDecimalStr(const char *p_DecimalString, uint16_t *p_bitsMin)
{
    uint32_t len;

    len = L_Toolbox_StrLen(p_DecimalString);
    
    /* (+ 30102) to round up */
    *p_bitsMin = (uint16_t)((((len - 1u) * 100000u) + 30102u) / 30103u);
    /* *bitsMax = (uint16_t)(( len      * 100000u + 30102u) / 30103u);. */
    
    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0019]:[ICUMFW_UD_S_03_PK_0019]
* Function name: s_03_CanUseKeyIfDbgConnected
* Description  : Determine whether the key is usable
* Arguments    : (in) disable_on_debug     : flag for whether key is usable when the debugger is connected
*                                           Range: 0 (key is usable when debugger is connected)
*                                                  1 (key is unusable when debugger is connected)
*                (out) p_is_allowed         : Range: TRUE  (key is usable)
*                                                  FALSE (key is unusable)
* Return Value : ER_OK
***************************************************************************************/
uint32_t s_03_CanUseKeyIfDbgConnected(key_flags_t disable_on_debug, uint8_t *p_is_allowed)
{
    uint32_t    icu_status;

    *p_is_allowed = (uint8_t)TRUE;
    (void)D_COMM_GetICUStatusInfo(&icu_status);

    if ((1u == (uint32_t)disable_on_debug)
            && ((ICUM_COMM_INF_ICU_ST_EXT_DEBUG & icu_status) == ICUM_COMM_INF_ICU_ST_EXT_DEBUG))
    {
        *p_is_allowed = (uint8_t)FALSE;
    }
    else
    {
        /* do nothing */
    }
    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_AESKeyUpdate_0006]:[ICUMFW_UD_S_03_AESKeyUpdate_0006]
* Function name: s_03_CheckMemory
* Description  : CheckMemory
* Arguments    : ex_service
*              : *p_input
*              : size
*              : direction
* Return Value : SERV_OK(ER_OK)
*              : ER_PARAM
*              : SERV_SRC_MEM_ERROR
*              : SERV_DEST_MEM_ERROR
***************************************************************************************/
uint32_t s_03_CheckMemory(uint8_t ex_service, uint8_t* p_input, uint32_t size, uint8_t direction)
{
    uint32_t err;
    uint8_t  mem_check;

    /* make sure the source address range is valid */
    err = MM_FLS_CheckMemory((uint32_t)p_input, size, direction, ex_service, &mem_check);
    if (err == ER_OK)
    {
        if ((uint32_t)FLS_CHK_OK != (uint32_t)mem_check)
        {
            if ((uint32_t)direction == (uint32_t)FLS_CHK_MEM_SOURCE)
            {
                err = (uint32_t)SERV_SRC_MEM_ERROR;
            }
            else
            {
                err = (uint32_t)SERV_DEST_MEM_ERROR;
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do Nothing */
    }
    return(err);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_03_PK_0022]:[ICUMFW_UD_S_03_PK_0022]
* Function name: s_03_ConvertErr
* Description  : Convert the set error code to another error code
* Arguments    : (in) arg                        : Error code to be set
*                                                  Range: ER_OK
*                                                         ER_PARAM
*                                                         ER_PENDING_ARB
*                                                         ER_PENDING
*                                                         ER_IN_PROCESS
*                                                         ER_MH_KEY_SLOT_EMPTY
*                                                         ER_MH_KEY_INVALID_KEY_GROUP
*                                                         ER_MH_KEY_INVALID_KEY_INDEX
*                                                         ER_MH_KEY_WRITE_PROTECTED
*                                                         ER_MH_KEY_IN_USE
*                                                         ER_MH_KEY_DATA_NOT_FOUND
*                                                         ER_MH_KEY_WRITING
*                                                         ER_MH_KEY_USER_OVER
*                                                         ER_MH_KEY_NOT_USABLE
*                                                         ER_MH_KEY_AES_TIMEOUT_ERROR
*                                                         ER_MH_KEY_DATA_INVALID
*                                                         ER_MH_KEY_NVM_UNAVAILABLE
*                                                         ER_MH_KEY_NVM_SYS_CRITICAL
*                                                         ER_MH_KEY_NVM_SYS
*                                                         ER_MH_KEY_NVM_PROG
*                                                         ER_MH_KEY_NVM_SIZE
*                                                         ER_MH_KEY_SRC_MEM_ERROR
*                                                         ER_MH_KEY_DEST_MEM_ERROR
*                                                         ER_MH_KEY_SYS_ERROR
*                                                         ER_MH_KEY_AES_PRM_ERROR
*                                                         ER_ML_CTX_ALL_USE
*                                                         ER_ML_CTX_NOT_FOUND
*                                                         R_SALIF_ER_OK
*                                                         R_SALIF_ER_PARAM
*                                                         R_SALIF_ER_PENDING
*                                                         R_SALIF_ER_IN_PROCESS
*                                                         R_SALIF_ER_CRY_STATUS_ERROR
*                                                         R_SALIF_ER_CRY_PKCC_ERROR
*                                                         R_SALIF_ER_CRY_INVALID_ARG
*                                                         R_SALIF_ER_CRY_ERROR
*                                                         R_SALIF_ER_CRY_ALIGN_ERROR
*                                                         R_SALIF_ER_CRY_TIMEOUT 
*                                                         R_SALIF_ER_CRY_RUNNING
*                (out) p_service_result            : converted error code
*                                                    Range: SERV_OK
*                                                           SERV_PRM_ERROR
*                                                           SERV_PENDING
*                                                           SERV_IN_PROCESS
*                                                           SERV_CRYPT_ERROR
*                                                           SERV_KEY_SLOT_EMPTY
*                                                           SERV_INVALID_KEY_GROUP
*                                                           SERV_INVALID_KEY_INDEX
*                                                           SERV_SEQUENCE_ERROR
*                                                           SERV_KEY_WRITE_PROTECTED
*                                                           SERV_DATA_INVALID
*                                                           SERV_NVM_UNAVAILABLE
*                                                           SERV_NVM_SYS_ERROR_CRITICAL 
*                                                           SERV_NVM_SYS_ERROR
*                                                           SERV_NVM_PROG_ERROR
*                                                           SERV_NVM_SIZE_ERROR
*                                                           SERV_SRC_MEM_ERROR
*                                                           SERV_DEST_MEM_ERROR
*                                                           SERV_SYS_ERROR
*                                                           SERV_AES_PRM_ERROR
* Return Value : ER_OK
***************************************************************************************/
uint32_t s_03_ConvertErr(uint32_t arg, volatile r_icumif_service_result_t *p_service_result)
{
    int32_t i;
    
    *p_service_result = (r_icumif_service_result_t)arg;
    for (i = 0; i < NUMBER_OF_ERROR_TABLE; i++)
    {
        if (arg == s_03_errcode_tuples[i].internal_code)
        {
            *p_service_result = s_03_errcode_tuples[i].external_code;
            break;
        }
        else
        {
            /* do nothing */
        }
    }
    return (ER_OK);
}

