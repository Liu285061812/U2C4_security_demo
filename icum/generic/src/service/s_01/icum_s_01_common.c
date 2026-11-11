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

#include "icum_s_pub.h"
#include "icum_s_01_common.h"

#include "icum_ml_abtr_api_pub.h"
#include "icum_ml_cfg_pub.h"
#include "icum_ml_ctx_pub.h"

#include "icum_d_wdt_pub.h"
#include "icum_d_cry_aes_pub.h"

#include "icum_l_toolbox_pub.h"
#include "icum_ml_prng_pub.h"
#include "icum_service_common.h"


/**********************************************************************************
* Local defines
**********************************************************************************/
#define PRNG_SIZE_IN_BYTES          (16u)
#define NUMBER_OF_ERROR_TABLE (47)

/**********************************************************************************
* Structure
***********************************************************************************/
/* Service and middle error conversion structure */
typedef struct {
    uint32_t                    internal_code;
    r_icumif_service_result_t   external_code;
} errcode_tuple_t;

/**********************************************************************************
* Variable
***********************************************************************************/
/* Service 01 and middle layer error conversion table */
static const errcode_tuple_t s_01_errcode_tuples[NUMBER_OF_ERROR_TABLE] =
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
    /* ER_MH_CTX */
    {ER_ML_CTX_ALL_USE,             SERV_PENDING            },
    {ER_ML_CTX_NOT_FOUND,           SERV_PRM_ERROR          },
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
    /* ER_L_PKC ECC */
    {ER_L_ECC_SYS_ERROR,            SERV_SYS_ERROR          },
    {ER_L_ECC_ERROR,                SERV_CRYPT_ERROR        },
    {ER_L_ECC_INVALID_ARG,          SERV_PRM_ERROR          },
    {ER_L_ECC_LENGTHERR,            SERV_CRYPT_ERROR        },
    /* Driver */
    {ER_L_RSA_INVALID_PARM,         SERV_PRM_ERROR          },
    {ER_D_AES_TIMEOUT_ERROR,        SERV_CRYPT_ERROR        },
    /* ER_L_BIGNUM */
    {ER_L_BIGNUM_BUFFER_OVERFLOW,   SERV_CRYPT_ERROR        },
    {ER_L_BIGNUM_INVALID_ARG,       SERV_PRM_ERROR          },
};

/**********************************************************************************
* Function
***********************************************************************************/
static uint32_t s_01_ContainsZero(const uint8_t *p_target_bytes, uint32_t length, bool *p_exists);

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0016]:[ICUMFW_UD_S_01_PK_0016]
* Function name: s_01_GetDecimalStrlenFromBitCnt
* Description  : get decimal figure length from bit_length.
*                     (decimal figure length) = (bit length) * log(2)
*                in below coding, (30103 / 100000) is used instead of log(2) (= 0.30102999...)
* Arguments    : (IN)       bits
*                (OUT)      p_figure10Max
* Return Value : ER_OK
****************************************************************************************/
uint32_t s_01_GetDecimalStrlenFromBitCnt(uint16_t bits, uint16_t *p_figure10Max)
{
    /* if you want to calculate Minimum, show below code */
/*    *figure10Min = (uint16_t)((((uint32_t)(bits-1u) * 30103U) + 99999U) / 100000U);*/
    /* (+ 99999) to round up */

    *p_figure10Max = (uint16_t)((((uint32_t)bits      * 30103U) + 99999U) / 100000U);

    return (ER_OK);
}


/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0017]:[ICUMFW_UD_S_01_PK_0017]
* Function name: s_01_GenerateRandomSequence
* Description  : Generate random sequence
*
* Arguments    : (IN)       size_in_bytes
*                           emePkcsFlag
*                (OUT)      p_udwRandom
* Return Value : SERV_OK
*                SERV_PENDING
*                SERV_UNAVAILABLE
*                SERV_SYS_ERROR
*                SERV_NO_INIT_RNG
*                SERV_NEED_RESEED
*                SERV_AES_TIMEOUT_ERROR
****************************************************************************************/
uint32_t s_01_GenerateRandomSequence(uint32_t *p_udwRandom, uint16_t size_in_bytes, bool emePkcsFlag)
{
    uint32_t    p_resource_id;
    /* buffer for random : 128 bits */
    uint32_t    buffer[4];
    int32_t     remain_bytes;
    uint32_t    i = 0u;
    bool        exists;
    uint32_t    err;
    uint32_t    skip_flg = FLG_OFF;

    err = ML_ABTR_GetResource(ABTR_RESOURCE_TYPE_AES, &p_resource_id);
    switch (err)
    {
        case ER_OK:
            break;
        case ER_ML_ABTR_EMPTY:
            err = (uint32_t)SERV_PENDING;
            break;
        case ER_ML_ABTR_NOT_RELEASED:
            err = (uint32_t)SERV_SYS_ERROR;
            break;
        case ER_ML_ABTR_NOT_INITIALIZED:
            err = (uint32_t)SERV_SYS_ERROR;
            break;
        case ER_ML_ABTR_TIMER_SET_FAILURE:
            err = (uint32_t)SERV_SYS_ERROR;
            break;
        default:
            /* Never Occur !! */
            err = (uint32_t)SERV_SYS_ERROR;
            break;
    }
    if (err != ER_OK)
    {
        skip_flg = FLG_ON;
    }
    else
    {
        /* do nothing */
    }
    if (skip_flg == FLG_OFF)
    {
        remain_bytes = (int32_t)size_in_bytes;
        while ((remain_bytes > 0) && (skip_flg == FLG_OFF))
        {
            /* get random : 128bits(=16 bytes =4 long word) */
            /* 1st parameter, 2 : auto judgement type */
            err = ML_PRNG_Gen(buffer);
            switch (err)
            {
                case ER_OK:
                    break;
                case ER_ML_PRNG_STATUS:
                    err = (uint32_t)SERV_NO_INIT_RNG;
                    break;
                case ER_ML_PRNG_RESEED:
                    err = (uint32_t)SERV_NEED_RESEED;
                    break;
                case ER_ML_PRNG_TIMEOUT:
                    err = (uint32_t)SERV_AES_TIMEOUT_ERROR;
                    break;
                case ER_PENDING:
                    err = (uint32_t)SERV_PENDING;
                    break;
                default:
                    /* Never Occur !! */
                    err = (uint32_t)SERV_SYS_ERROR;
                    break;
            }
            if (err != ER_OK)
            {
                skip_flg = FLG_ON;
            }
            else
            {
                /* do nothing */
            }
            if (skip_flg == FLG_OFF)
            {
                /* register call back function for WDT restart */
                D_WDT_StartIfNeeded();

                if ((int32_t)emePkcsFlag == true)
                {
                    (void)s_01_ContainsZero((uint8_t *)buffer, PRNG_SIZE_IN_BYTES, &exists);
                    if ((int8_t)exists == true)
                    {
                        continue;
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
                if (remain_bytes < (int32_t)PRNG_SIZE_IN_BYTES)
                {
                    L_Toolbox_MemCpy((uint8_t *)&p_udwRandom[i * (PRNG_SIZE_IN_BYTES / sizeof(uint32_t))],
                            (const uint8_t *)buffer, (int16_t)remain_bytes);
                }
                else
                {
                    L_Toolbox_MemCpy((uint8_t *)&p_udwRandom[i * (PRNG_SIZE_IN_BYTES / sizeof(uint32_t))],
                            (const uint8_t *)buffer, (int16_t)PRNG_SIZE_IN_BYTES);
                }
                i++;
                remain_bytes -= (int32_t)PRNG_SIZE_IN_BYTES;
            }
            else
            {
                /* do nothing */
            }
        }

        (void)ML_ABTR_FreeResource(ABTR_RESOURCE_TYPE_AES, p_resource_id);
    }
    else
    {
        /* do nothing */
    }

    return (err);
}


/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0018]:[ICUMFW_UD_S_01_PK_0018]
* Function name: s_01_ContainsZero
* Description  : check for existence of byte zero in the specified byte sequence
*
* Arguments    : (in)       p_target_bytes
*                           length
*                (out)      p_exists       Range: boolean (true/false)
* Return Value : ER_OK
****************************************************************************************/
static uint32_t s_01_ContainsZero(const uint8_t *p_target_bytes, uint32_t length, bool *p_exists)
{
    uint32_t     i;

    *p_exists = false;
    for (i = 0u; i < length; i++)
    {
        if (0x00u == (uint32_t)p_target_bytes[i])
        {
            *p_exists = true;
            break;
        }
        else
        {
            /* do nothing */
        }
    }
    return (ER_OK);
}


/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0023]:[ICUMFW_UD_S_01_PK_0023]
* Function name: s_01_IsDigitString
* Description  : check character in decimal string
*
* Arguments    : (in)       p_target_bytes
*                           length
*                (out)      p_isdigit     Range: uint8_t (TRUE/FALSE)
* Return Value : ER_OK
****************************************************************************************/
uint32_t s_01_IsDigitString(const uint8_t *p_target_bytes, uint32_t length, uint8_t *p_isdigit)
{
    uint32_t     i;

    *p_isdigit = FALSE;
    for (i = 0u; i < length; i++)
    {
        if (0x00u == (uint32_t)p_target_bytes[i])
        {
            /* found terminator(NULL). */
            if(i != 0u)
            {
                /* string length not equals zero */
                *p_isdigit = TRUE;
            }
            else
            {
                /* do nothing */
            }
            break;
        }
        else
        {
            /* do nothing */
        }

        if (i == (length - 1u))
        {
            /* not found terminator(NULL), after all */
            break;
        }
        else
        {
            /* do nothing */
        }

        if (((uint32_t)'0' <= (uint32_t)p_target_bytes[i]) && ((uint32_t)p_target_bytes[i] <= (uint32_t)'9'))
        {
            continue;
        }
        else
        {
            /* do nothing */
        }

        /* illegal character */
        break;
    }
    return (ER_OK);
}

/****************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0024]:[ICUMFW_UD_S_01_PK_0024]
* Function name: s_01_IsHexDigitString
* Description  : check character in hexadecimal string
*
* Arguments    : (in)       p_target_bytes
*                           length
*                (out)      p_isdigit     Range: uint8_t (TRUE/FALSE)
* Return Value : ER_OK
****************************************************************************************/
uint32_t s_01_IsHexDigitString(const uint8_t *p_target_bytes, uint32_t length, uint8_t *p_isdigit)
{
    uint32_t     i;

    *p_isdigit = FALSE;
    for (i = 0u; i < length; i++)
    {
        if (0x00u == (uint32_t)p_target_bytes[i])
        {
            /* found terminator(NULL). */
            if(i != 0u)
            {
                /* string length not equals zero */
                *p_isdigit = TRUE;
            }
            else
            {
                /* do nothing */
            }
            break;
        }
        else
        {
            /* do nothing */
        }

        if (i == (length - 1u))
        {
            /* not found terminator(NULL), after all */
            break;
        }
        else
        {
            /* do nothing */
        }

        if (((uint32_t)'0' <= (uint32_t)p_target_bytes[i]) && ((uint32_t)p_target_bytes[i] <= (uint32_t)'9'))
        {
            continue;
        }
        else
        {
            /* do nothing */
        }
        if (((uint32_t)'A' <= (uint32_t)p_target_bytes[i]) && ((uint32_t)p_target_bytes[i] <= (uint32_t)'F'))
        {
            continue;
        }
        else
        {
            /* do nothing */
        }
        if (((uint32_t)'a' <= (uint32_t)p_target_bytes[i]) && ((uint32_t)p_target_bytes[i] <= (uint32_t)'f'))
        {
            continue;
        }
        else
        {
            /* do nothing */
        }

        /* illegal character */
        break;
    }
    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0019]:[ICUMFW_UD_S_01_PK_0019]
* Function name: s_01_CanUseKeyIfDbgConnected
* Description  : Determine whether the key is usable
* Arguments    : (in) disable_on_debug     : flag for whether key is usable when the debugger is connected
*                                           Range: 0 (key is usable when debugger is connected)
*                                                  1 (key is unusable when debugger is connected)
*                (out) p_is_allowed       : Range: TRUE  (key is usable)
*                                                  FALSE (key is unusable)
* Return Value : ER_OK
***************************************************************************************/
uint32_t s_01_CanUseKeyIfDbgConnected(key_flags_t disable_on_debug, uint8_t *p_is_allowed)
{
    uint32_t    icu_status;

    *p_is_allowed = TRUE;
    (void)D_COMM_GetICUStatusInfo(&icu_status);

    if ((1u == (uint32_t)disable_on_debug) &&
        ((ICUM_COMM_INF_ICU_ST_EXT_DEBUG & icu_status) == ICUM_COMM_INF_ICU_ST_EXT_DEBUG))
    {
        *p_is_allowed = FALSE;
    }
    else
    {
        /* do nothing */
    }
    return (ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S_01_PK_0022]:[ICUMFW_UD_S_01_PK_0022]
* Function name: s_01_ConvertErr
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
*                                                         ER_ML_ABTR_EMPTY
*                                                         ER_ML_ABTR_NOT_RELEASED
*                                                         ER_ML_ABTR_INVALID_ID
*                                                         ER_ML_ABTR_TIMER_SET_FAILURE
*                                                         ER_ML_ABTR_NOT_INITIALIZED
*                                                         R_SALIF_ER_OK
*                                                         R_SALIF_ER_IN_PROCESS
*                                                         R_SALIF_ER_PARAM
*                                                         R_SALIF_ER_CRY_STATUS_ERROR
*                                                         R_SALIF_ER_CRY_PKCC_ERROR
*                                                         R_SALIF_ER_CRY_TIMEOUT
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
uint32_t s_01_ConvertErr(uint32_t arg, volatile r_icumif_service_result_t *p_service_result)
{
    int32_t i;

    *p_service_result = (r_icumif_service_result_t)arg;
    for (i = 0; i < NUMBER_OF_ERROR_TABLE; i++)
    {
        if (arg == s_01_errcode_tuples[i].internal_code)
        {
            *p_service_result = s_01_errcode_tuples[i].external_code;
            break;
        }
        else
        {
            /* do nothing */
        }
    }
    return (ER_OK);
}

