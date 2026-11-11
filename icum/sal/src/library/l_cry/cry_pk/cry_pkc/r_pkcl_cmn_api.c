/******************************************************************************/
/* Component Name   Cryptographic Library PKC Library                         */
/******************************************************************************/
/* Product      : ICU-M Low Level Driver                                      */
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

/* PRQA S 0310, 3305 ++ # D001 Processing is done in units of WORD to increase processing speed. */
/* PRQA S 0751, 1251, 2100 ++ # D007 Because it is necessary for the judgment of big endian. */
/* PRQA S 1503 ++ # D010 No problem if it is unused because it is an API called from outside the library  */
/* PRQA S 1532 ++ # D011 Since files are grouped by function, they are called from different files. */
/* PRQA S 3383, 3384 ++ # D012 Have confirmed in advance that the value doesn't wrap around. */
/* PRQA S 2981 ++ # D013 Local variable ret of APIs sets INTERNALERR to the initial value
                        as a countermeasure against attacks that pass through processing. */
/* PRQA S 2984 ++ # D021 Double check in setting the return value as a countermeaseure
                        against attacks that skip a certain process */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "renesas_types.h"
#include "r_pkc_api.h"
#include "r_pkcl_api.h"
#include "r_pkcl_cmn.h"

/*!********************************************************************
 * \addtogroup    PKCLCommon
 * \{
 * \addtogroup    PKCLCommonFlags               Common Flags
 * \addtogroup    PKCLCommonDefines             Common Definitions
 * \addtogroup    PKCLCommonTypes               Common Types
 * \addtogroup    PKCLCommonAPIs                Common APIs
 * \addtogroup    PKCLCommonDefinesInternal     Common Internal Definition
 * \addtogroup    PKCLCommonTypesInternal       Common Internal Types
 * \addtogroup    PKCLCommonVariablesInternal   Common Internal Variables
 * \addtogroup    PKCLCommonAPIsInternal        Common Internal APIs
 * \}******************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDCommonDefinesInternal
 * \{******************************************************************/
/*! \}*/

/*!********************************************************************
 * \addtogroup  PKCLCommonDefinesInternal     Common Internal Definition
 * \{******************************************************************/
#define PKCL_CMN_WORD_BYTE_SIZE              (4U)              /**< A byte size of WORD */

#define PKCL_CMN_MULTIPLE_4                  (3U)              /**< Value used to determine multiples of 4 */

#define PKCL_CMN_CH_BIG_ENDIAN_CALC_1B       (0x000000FFU)     /**< First byte value used for big endian conversion */
#define PKCL_CMN_CH_BIG_ENDIAN_CALC_2B       (0x0000FF00U)     /**< Second byte value used for big endian conversion */
#define PKCL_CMN_CH_BIG_ENDIAN_CALC_3B       (0x00FF0000U)     /**< Third byte value used for big endian conversion */
#define PKCL_CMN_CH_BIG_ENDIAN_CALC_4B       (0xFF000000U)     /**< Fourth byte value used for big endian conversion */
#define PKCL_CMN_CH_BIG_ENDIAN_SHIFT_8_BIT   (8U)              /**< 8-bit shift number **/
#define PKCL_CMN_CH_BIG_ENDIAN_SHIFT_24_BIT  (24U)             /**< 24-bit shift number **/
/*! \}*/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/*!********************************************************************
 * \addtogroup  PKCLCommonVariablesInternal     Common Internal Variables
 * \{******************************************************************/
static e_pkcl_state_t s_pkcl_state;               /**< PKC State */
/*! \}*/

/**********************************************************************************************************************
* Function Name : R_PKCL_Initialize
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_Initialize(void)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_INIT == pkcl_state)
    {
        /* PKC-Driver Initialize */
        drv_ret = R_PKCD_Initialize();

        /* Convert to PKC-Lib return code */
        ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

        if (PKCL_RET_OK == ret)
        {
            /* State transition */
            r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_Initialize()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_GetState
**********************************************************************************************************************/
e_pkcl_state_t R_PKCL_GetState(void)
{
    return s_pkcl_state;
}
/**********************************************************************************************************************
* End of function R_PKCL_GetState()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_ChkPKCCBusy
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_ChkPKCCBusy(void)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;

    /* Check PKCC Busy */
    drv_ret = R_PKCD_ChkPKCCBusy();

    /* Convert to PKC-Lib return code */
    ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_ChkPKCCBusy()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_IsAsyncOpeProgress
**********************************************************************************************************************/
e_pkc_bool_t R_PKCL_IsAsyncOpeProgress(void)
{
    e_pkc_bool_t is_async;
    is_async = R_PKCD_IsAsyncOpeProgress();

    return is_async;
}
/**********************************************************************************************************************
* End of function R_PKCL_IsAsyncOpeProgress()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_ClearState
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_ClearState(void)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcd_ret_t drv_ret;
    e_pkcl_state_t pkcl_state;

    /* Check PKC State */
    pkcl_state = r_pkcl_cmn_get_state();
    if (PKCL_STATE_INIT != pkcl_state)
    {
        /* PKC-Driver  */
        drv_ret = R_PKCD_ClearState();

        /* Convert to PKC-Lib return code */
        ret = r_pkcl_cmn_pkcl_ret_from_pkcd_ret(drv_ret);

        if (PKCL_RET_OK == ret)
        {
            /* State transition */
            r_pkcl_cmn_set_state(PKCL_STATE_NO_EXEC_OPERATION);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        ret = PKCL_RET_STATUSERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_ClearState()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCL_GetVersion
**********************************************************************************************************************/
pkcl_ret_t R_PKCL_GetVersion(uint32_t* const p_major, uint32_t* const p_minor)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;

    /* Check NULL argument */
    if ((NULL != p_major) && (NULL != p_minor))
    {
        /* Get major version */
        *p_major = PKCL_VERSION_MAJOR;

        /* Get minor version */
        *p_minor = PKCL_VERSION_MINOR;

        ret = PKCL_RET_OK;
    }
    else
    {
        ret = PKCL_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCL_GetVersion()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_cmn_pkcl_ret_from_pkcd_ret
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLCommonAPIsInternal
 * \brief       Returns the return code corresponding to the return code of the PKC driver
 * \param        [in]   drv_ret                The PKC Driver return code
 * \return
 *  - #PKCL_RET_OK:                             The PKC driver return code PKCD_RET_OK entered.
 *  - #PKCL_RET_PROCESSING:                     The PKC driver return code PKCD_RET_PROCESSING enterd.
 *  - #PKCL_RET_DRV_INTERNALERR:                The PKC driver return code PKCD_RET_INTERNALERR entered.
 *  - #PKCL_RET_DRV_PARAMERR:                   The PKC driver return code PKCD_RET_PARAMERR entered.
 *  - #PKCL_RET_DRV_LENGTHERR:                  The PKC driver return code PKCD_RET_LENGTHERR entered.
 *  - #PKCL_RET_KEY_PARAMERR:                   The PKC driver return code PKCD_RET_KEY_PARAMERR entered.
 *  - #PKCL_RET_KEY_LENGTHERR:                  The PKC driver return code PKCD_RET_KEY_LENGTHERR entered.
 *  - #PKCL_RET_HASHAPIERR:                     The PKC driver return code PKCL_RET_HASHAPIERR entered.
 *  - #PKCL_RET_PKCCTIMEOUTERR:                 The PKC driver return code PKCD_RET_PKCCTIMEOUTERR entered.
 *  - #PKCL_RET_PKCCERR:                        The PKC driver return code PKCD_RET_PKCCERR entered.
 *  - #PKCL_RET_RSA_NON_COPRIME:                The PKC driver return code PKCD_RET_RSA_NON_COPRIME entered.
 *  - #PKCL_RET_RSA_COMPOSITE:                  The PKC driver return code PKCD_RET_RSA_COMPOSITE entered.
 *  - #PKCL_RET_EC_VERIFAIL:                    The PKC driver return code PKCD_RET_PKCD_RET_EC_VERIFAIL entered.
 *  - #PKCL_RET_EC_ZERO_SIGNATURE:              The PKC driver return code PKCD_RET_EC_ZERO_SIGNATURE entered.
 *  - #PKCL_RET_EDDSA_VERIFAIL:                 The PKC driver return code PKCD_RET_EDDSA_VERIFAIL entered.
 *  - #PKCL_RET_EDDSA_DECOMPERR:                The PKC driver return code PKCD_RET_EDDSA_DECOMPERR entered.
 *  - #PKCL_RET_DRV_TAMPERERR:                  The PKC driver return code PKCD_RET_TAMPERERR entered.
 *  - #PKCL_RET_DRV_FAIL:                       The PKC driver return code other than the above entered.
 *  - #PKCL_RET_TAMPERERR:                      Tampering has been detected.
 *  - #PKCL_RET_INTERNALERR:                    Internal processing failed
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Security components
 *  - no
 * \par Detailed description
 *      - Returns the return code corresponding to the return code of cipher driver
 *      - Implement the following as a glitch countermeasure :
 *        - Set success return value in two steps
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the variable ret with #PKCL_RET_INTERNALERR.
 *  -# If drv_ret == PKCD_RET_OK.
 *    -# Set temp_ret to #PKCL_CMN_RET_OK_HALF_1.
 *  -# Else if drv_ret == PKCD_RET_PROCESSING
 *    -# Set temp_ret to #PKCL_CMN_RET_PROC_HALF_1.
 *  -# Else if drv_ret == PKCD_RET_PRIME_GENERATING
 *    -# Set temp_ret to #PKCL_CMN_RET_PRIMEGEN_HALF_1.
 *  -# Else if drv_ret == PKCD_RET_INTERNALERR
 *    -# Set temp_ret to #PKCL_RET_DRV_INTERNALERR.
 *  -# Else if drv_ret == PKCD_RET_PARAMERR
 *    -# Set temp_ret to #PKCL_RET_DRV_PARAMERR.
 *  -# Else if drv_ret == PKCD_RET_LENGTHERR
 *    -# Set temp_ret to #PKCL_RET_DRV_LENGTHERR.
 *  -# Else if drv_ret == PKCD_RET_KEY_PARAMERR
 *    -# Set temp_ret to #PKCL_RET_KEY_PARAMERR.
 *  -# Else if drv_ret == PKCD_RET_KEY_LENGTHERR
 *    -# Set temp_ret to #PKCL_RET_KEY_LENGTHERR.
 *  -# Else if drv_ret == PKCD_RET_HASHAPIERR
 *    -# Set temp_ret to #PKCL_RET_HASHAPIERR.
 *  -# Else if drv_ret == PKCD_RET_DRBGAPIERR
 *    -# Set temp_ret to #PKCL_RET_DRBGAPIERR.
 *  -# Else if drv_ret == PKCD_RET_PKCCTIMEOUTERR
 *    -# Set temp_ret to #PKCL_RET_PKCCTIMEOUTERR.
 *  -# Else if drv_ret == PKCD_RET_PKCCERR
 *    -# Set temp_ret to #PKCL_RET_PKCCERR.
 *  -# Else if drv_ret == PKCD_RET_RSA_NON_COPRIME
 *    -# Set temp_ret to #PKCL_RET_RSA_NON_COPRIME.
 *  -# Else if drv_ret == PKCD_RET_RSA_COMPOSITE
 *    -# Set temp_ret to #PKCL_RET_RSA_COMPOSITE.
 *  -# Else if drv_ret == PKCD_RET_DH_INVALID_SHAREDSECRET
 *    -# Set temp_ret to #PKCL_RET_DH_INVALID_SHAREDSECRET.
 *  -# Else if drv_ret == PKCD_RET_EC_VERIFAIL
 *    -# Set temp_ret to #PKCL_RET_EC_VERIFAIL.
 *  -# Else if drv_ret == PKCD_RET_EC_ZERO_SIGNATURE
 *    -# Set temp_ret to #PKCL_RET_EC_ZERO_SIGNATURE.
 *  -# Else if drv_ret == PKCD_RET_EDDSA_VERIFAIL
 *    -# Set temp_ret to #PKCL_RET_EDDSA_VERIFAIL.
 *  -# Else if drv_ret == PKCD_RET_EDDSA_DECOMPERR
 *    -# Set temp_ret to #PKCL_RET_EDDSA_DECOMPERR.
 *  -# Else if drv_ret == PKCD_RET_TAMPERERR
 *    -# Set temp_ret to #PKCL_RET_DRV_TAMPERERR.
 *  -# Else
 *    -# Set temp_ret to #PKCL_RET_DRV_FAIL.
 *  -# If drv_ret == PKCD_RET_OK
 *    -# Set temp_ret to (temp_ret + #PKCL_CMN_RET_OK_HALF_2).
 *  -# Else if drv_ret == PKCD_RET_PROCESSING
 *    -# Set temp_ret to (temp_ret + #PKCL_CMN_RET_PROC_HALF_2).
 *  -# Else if drv_ret == PKCD_RET_PRIME_GENERATING
 *    -# Set temp_ret to (temp_ret + #PKCL_CMN_RET_PRIMEGEN_HALF_2).
 *  -# Else
 *    -# Do nothing.
 *  -# Set ret to temp_ret.
 *  -# If the return value in ret is an error code, then
 *    -# If ((#PKCL_RET_OK == ret) || (#PKCL_RET_PROCESSING == ret) || (#PKCL_RET_PRIME_GENERATING == ret)), then
 *      -# Do nohting.
 *    -# Else
 *      -# Set ret to #PKCL_RET_TAMPERERR.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
pkcl_ret_t r_pkcl_cmn_pkcl_ret_from_pkcd_ret(const pkcd_ret_t drv_ret)
{
    pkcl_ret_t ret = PKCL_RET_INTERNALERR;
    pkcl_ret_t temp_ret;
    uint32_t i;

    /* Perfomance function struct */
    typedef struct
    {
        pkcd_ret_t driver_ret;
        pkcl_ret_t lib_ret;
    } drv_ret_tbl_t;

    /* Driver return table */
    const drv_ret_tbl_t drv_ret_tbl[20] =
    {
        {PKCD_RET_OK,                      PKCL_CMN_RET_OK_HALF_1},
        {PKCD_RET_PROCESSING,              PKCL_CMN_RET_PROC_HALF_1},
        {PKCD_RET_PRIME_GENERATING,        PKCL_CMN_RET_PRIMEGEN_HALF_1},
        {PKCD_RET_INTERNALERR,             PKCL_RET_DRV_INTERNALERR},
        {PKCD_RET_PARAMERR,                PKCL_RET_DRV_PARAMERR},
        {PKCD_RET_LENGTHERR,               PKCL_RET_DRV_LENGTHERR},
        {PKCD_RET_KEY_PARAMERR,            PKCL_RET_KEY_PARAMERR},
        {PKCD_RET_KEY_LENGTHERR,           PKCL_RET_KEY_LENGTHERR},
        {PKCD_RET_HASHAPIERR,              PKCL_RET_HASHAPIERR},
        {PKCD_RET_DRBGAPIERR,              PKCL_RET_DRBGAPIERR},
        {PKCD_RET_PKCCTIMEOUTERR,          PKCL_RET_PKCCTIMEOUTERR},
        {PKCD_RET_PKCCERR,                 PKCL_RET_PKCCERR},
        {PKCD_RET_RSA_NON_COPRIME,         PKCL_RET_RSA_NON_COPRIME},
        {PKCD_RET_RSA_COMPOSITE,           PKCL_RET_RSA_COMPOSITE},
        {PKCD_RET_DH_INVALID_SHAREDSECRET, PKCL_RET_DH_INVALID_SHAREDSECRET},
        {PKCD_RET_EC_VERIFAIL,             PKCL_RET_EC_VERIFAIL},
        {PKCD_RET_EC_ZERO_SIGNATURE,       PKCL_RET_EC_ZERO_SIGNATURE},
        {PKCD_RET_EDDSA_VERIFAIL,          PKCL_RET_EDDSA_VERIFAIL},
        {PKCD_RET_EDDSA_DECOMPERR,         PKCL_RET_EDDSA_DECOMPERR},
        {PKCD_RET_TAMPERERR,               PKCL_RET_DRV_TAMPERERR},
    };

    /* Select test target */
    for (i = 0U; i < sizeof(drv_ret_tbl)/sizeof(drv_ret_tbl[0U]); i++)
    {
        if (drv_ret == drv_ret_tbl[i].driver_ret)
        {
            temp_ret = drv_ret_tbl[i].lib_ret;
            break;
        }
        else
        {
            /* do nothing */
        }
    }

    if (i == sizeof(drv_ret_tbl))
    {
        temp_ret = PKCL_RET_DRV_FAIL;
    }
    else
    {
        /* do nothing */
    }

    switch (drv_ret)
    {
        case PKCD_RET_OK:
            temp_ret += PKCL_CMN_RET_OK_HALF_2;
            break;
        case PKCD_RET_PROCESSING:
            temp_ret += PKCL_CMN_RET_PROC_HALF_2;
            break;
        case PKCD_RET_PRIME_GENERATING:
            temp_ret += PKCL_CMN_RET_PRIMEGEN_HALF_2;
            break;
        default: /* In case of error, do nothing because the return value has already been set */
            break;
    }

    ret = temp_ret;

    /* The if statement below assumes that all PKC-Lib error codes is in the form 0xAAAA**** */
    if ((ret & PKCL_RET_INTERNALERR) != (PKCL_RET_INTERNALERR))
    {
        if ((PKCL_RET_OK == ret) || (PKCL_RET_PROCESSING == ret) || (PKCL_RET_PRIME_GENERATING == ret))
        {
            /* Do nothing */
        }
        else
        {
            ret = PKCL_RET_TAMPERERR;
        }
    }
    else
    {
        /* Do nothing */
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_cmn_pkcl_ret_from_pkcd_ret()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_cmn_set_state
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLCommonAPIsInternal
 * \brief       Set PKC State.
 * \param       [in] state                 PKC State enum
 * \return
 *  - None
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Security components
 *  - no
 * \par Detailed description
 *      - Set PKC State.
 *
 * \callgraph
 *********************************************************************************************************************/
void r_pkcl_cmn_set_state(const e_pkcl_state_t state)
{
    s_pkcl_state = state;
}
/**********************************************************************************************************************
* End of function r_pkcl_cmn_set_state()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_cmn_get_state
**********************************************************************************************************************/
/*!*******************************************************************************************************************
 * \ingroup     PKCLCommonAPIsInternal
 * \brief       Get PKC State.
 * \return
 *  - PKCC state
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Security components
 *  - no
 * \par Detailed description
 *      - Get PKC State.
 *
 * \callgraph
 *********************************************************************************************************************/
e_pkcl_state_t r_pkcl_cmn_get_state(void)
{
    return s_pkcl_state;
}
/**********************************************************************************************************************
* End of function r_pkcl_cmn_get_state()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_cmn_is_multiple_4
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCCCommonAPIsInternal
 * \brief       Fold multiplication of 4.
 * \param       [in] val      Value to perform multiple determination.
 * \return
 *  - PKC_TRUE:       Multiples of 4.
 *  - PKC_FALSE:      Not Multiples of 4.
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Fold multiplication of 4.
 *
 * \callgraph
 *********************************************************************************************************************/
e_pkc_bool_t r_pkcl_cmn_is_multiple_4(const uint32_t val)
{
    return ((val & PKCL_CMN_MULTIPLE_4) == 0U) ? PKC_BOOL_TRUE : PKC_BOOL_FALSE;
}
/**********************************************************************************************************************
* End of function r_pkcl_cmn_is_multiple_4()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_cmn_set_byte
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Simple byte-wise value set function.
 * \param       [out] p_dest      Destination address
 * \param       [in] val          Set value
 * \param       [in] len          Set length in byte
 * \return
 *  - None
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Simple byte-wise value set function.
 *
 * \callgraph
 *********************************************************************************************************************/
void r_pkcl_cmn_set_byte(uint8_t* const p_dest, const uint8_t val, const uint32_t len)
{
    uint32_t i;

    for (i = 0U; i < len; i++ )
    {
        p_dest[i] = val;
    }
}
/**********************************************************************************************************************
* End of function r_pkcl_cmn_set_byte()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_cmn_cpy_byte
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Simple byte-wise copy function.
 * \param       [out] p_dest      Destination address
 * \param       [in] p_src        Source address
 * \param       [in] len          Copying length in byte
 * \return
 *  - None
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Simple byte-wise copy function.
 *
 * \callgraph
 *********************************************************************************************************************/
void r_pkcl_cmn_cpy_byte(uint8_t* const p_dest, const uint8_t* const p_src, const uint32_t len)
{
    const uint32_t * const p_src32 =  (const uint32_t*)p_src;  /* No problem with casting to pointer size */
    uint32_t * const p_dest32 = (uint32_t*)p_dest;
    const uint32_t word_len = (len / PKCL_CMN_WORD_BYTE_SIZE) * PKCL_CMN_WORD_BYTE_SIZE;
    const uint32_t rem_len = (len % PKCL_CMN_WORD_BYTE_SIZE);
    uint32_t i;

    if (0U != word_len)
    {
        for (i = 0U; i < (word_len / PKCL_CMN_WORD_BYTE_SIZE); i++ )
        {
            p_dest32[i] = p_src32[i];
        }
    }
    else
    {
        /* Do nothing */
    }

    for (i = 0U; i < rem_len; i++ )
    {
        p_dest[word_len + i] = p_src[word_len + i];
    }
}
/**********************************************************************************************************************
* End of function r_pkcl_cmn_cpy_byte()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_cmn_cmp_byte
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Simple byte-wise compare function.
 * \param       [in] p_data_a      A adress of data to be compared.
 * \param       [in] p_data_b      A adress of data to be compared.
 * \param       [in] len           Byte size of data to be compared.
 * \return
 *  - #PKC_BOOL_TRUE: equal, #PKC_BOOL_FALSE: not equal
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Simple byte-wise compare function.
 *
 * \callgraph
 *********************************************************************************************************************/
e_pkc_bool_t r_pkcl_cmn_cmp_byte(const uint8_t* const p_data_a, const uint8_t* const p_data_b, const uint32_t len)
{
    uint32_t i;
    e_pkc_bool_t ret = PKC_BOOL_FALSE;
    uint32_t unmatch_flg = 0U;

    for (i = 0U; i < len; i++ )
    {
        /* Cast to uint32_t for comparison */
        if ((uint32_t)p_data_a[i] == (uint32_t)p_data_b[i])
        {
            /* Do nothing */
        }
        else
        {
            unmatch_flg = 1U;
        }
    }

    if (0U == unmatch_flg)
    {
        ret = PKC_BOOL_TRUE;
    }
    else
    {
        ret = PKC_BOOL_FALSE;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function r_pkcl_cmn_cmp_byte()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcl_cmn_ch_big_endian32
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLCommonAPIsInternal
 * \brief       Change 32bit value to big endian
 * \param       [in] val      A target 32bit value
 * \return
 *  - A 32bit value to big endian
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Change 32bit value to big endian
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
uint32_t r_pkcl_cmn_ch_big_endian32(const uint32_t val)
{
    const uint32_t x = 1U;
    uint32_t be_val;

    /* change endian? */
    if (0U != (*(const uint8_t*)&x))
    {
        /* little endian to big endian */
        be_val = ((val & PKCL_CMN_CH_BIG_ENDIAN_CALC_1B) << PKCL_CMN_CH_BIG_ENDIAN_SHIFT_24_BIT);
        be_val += ((val & PKCL_CMN_CH_BIG_ENDIAN_CALC_2B) << PKCL_CMN_CH_BIG_ENDIAN_SHIFT_8_BIT);
        be_val += ((val & PKCL_CMN_CH_BIG_ENDIAN_CALC_3B) >> PKCL_CMN_CH_BIG_ENDIAN_SHIFT_8_BIT);
        be_val += ((val & PKCL_CMN_CH_BIG_ENDIAN_CALC_4B) >> PKCL_CMN_CH_BIG_ENDIAN_SHIFT_24_BIT);
    }
    else
    {
        /* big endian to big endian(No convert)*/
        be_val = val;
    }

    return be_val;
}
/**********************************************************************************************************************
* End of function r_pkcl_cmn_ch_big_endian32()
**********************************************************************************************************************/

