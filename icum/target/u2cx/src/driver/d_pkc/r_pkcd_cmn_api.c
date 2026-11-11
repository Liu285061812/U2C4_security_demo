/******************************************************************************/
/* Component Name   ICU-M PKC Driver                                         */
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

/* PRQA S 0303 ++ # D001 Cast for accessing the area of register address */
/* PRQA S 0306 ++ # D002 Cast for accessing the area of PKCCRAM address */
/* PRQA S 0306 ++ # D003 Cast to use address as an integer value */
/* PRQA S 0310, 3305 ++ # D004 Processing is done in units of WORD to increase processing speed. */
/* PRQA S 0488 ++ # D007 Consider not to overrun the size of the array pointed to by the pointer.*/
/* PRQA S 0751 ++ # D011 In order to allocate the buffer at the 4-byte boundary,
                    the buffer defined by uint32_t type is cast to uint8_t type.*/
/* PRQA S 1503 ++ # D013 No problem if it is unused because it is an API called from outside the library */
/* PRQA S 1505, 1532 ++ # D014 Since files are grouped by function, they are called from different files. */
/* PRQA S 2843 ++ # D017 No problem because it does not go through the path that refers to the invalid pointer */
/* PRQA S 2953 ++ # D019 A calculation required to swap and set the value in PKCCRAM,
                        and there is no problem because it is intentionally  subtracted. */
/* PRQA S 2981 ++ # D020 Local variable ret of APIs sets INTERNALERR to the initial value
                        as a countermeasure against attacks that pass through processing. */
/* PRQA S 3383, 3384 ++ # D018 Have confirmed in advance that the value doesn't wrap around. */
/* PRQA S 3416 ++ # D025 Because it is volatile to prevent the timeout count from being optimized. */
/* PRQA S 3469 ++ # D026 If it is a function, overhead is large, so it corresponds with macro. */
/* PRQA S 5002 ++ # D030 There is no problem because it is used in a system that can distinguish
                    file names of 8 characters or more. */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "renesas_types.h"
#include "r_pkc_api.h"
#include "r_pkcd_cfg.h"
#include "r_pkcd_api.h"
#include "r_pkcd_dev_api.h"
#include "r_pkcd_cmn.h"

/*!********************************************************************
 * \addtogroup    PKCDCommon
 * \{
 * \addtogroup    PKCDCommonFlags               Common Flags
 * \addtogroup    PKCDCommonDefines             Common Definitions
 * \addtogroup    PKCDCommonTypes               Common Types
 * \addtogroup    PKCDCommonAPIs                Common APIs
 * \addtogroup    PKCDCommonDefinesInternal     Common Internal Definition
 * \addtogroup    PKCDCommonVariablesInternal   Common Internal Variables
 * \addtogroup    PKCDCommonAPIsInternal        Common Internal APIs
 * \}******************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \ingroup PKCDCommonDefinesInternal
 * \{******************************************************************/
#define PKCD_CMN_CLR_STATUS              (0x00050001U)      /**< Clear PKCC status register */

#define PKCD_CMN_OMR_MD_MASK             (0x0000001FU)      /**< Mask of MD in PKCC Operation Mode Register */

#define PKCD_CMN_OMR_DL_MASK             (0x007F0000U)      /**< Mask of DL in PKCC Operation Mode Register */
#define PKCD_CMN_OMR_BDL_MASK            (0x7F000000U)      /**< Mask of BDL in PKCC Operation Mode Register */

#define PKCD_CMN_CH_ENDIAN_CALC_1B       (0x000000FFU)      /**< First byte value used for endian conversion */
#define PKCD_CMN_CH_ENDIAN_CALC_2B       (0x0000FF00U)      /**< Second byte value used for endian conversion */
#define PKCD_CMN_CH_ENDIAN_CALC_3B       (0x00FF0000U)      /**< Third byte value used for endian conversion */
#define PKCD_CMN_CH_ENDIAN_CALC_4B       (0xFF000000U)      /**< Fourth byte value used for endian conversion */
#define PKCD_CMN_CH_ENDIAN_SHIFT_8_BIT   (8U)               /**< 8-bit shift number **/
#define PKCD_CMN_CH_ENDIAN_SHIFT_24_BIT  (24U)              /**< 24-bit shift number **/

/*! \}*/
/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static e_pkc_bool_t s_pkcd_is_async_ope;      /**< Async operetion progress flag */

/**********************************************************************************************************************
* Function Name : R_PKCD_Initialize
**********************************************************************************************************************/
pkcd_ret_t   R_PKCD_Initialize(void)
{
    const pkcd_ret_t ret = PKCD_RET_OK;

    /* clear PKCCRAM */
    r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

    /* Clear async flag */
    r_pkcd_cmn_clr_async_ope();

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_Initialize()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_ChkPKCCBusy
**********************************************************************************************************************/
pkcd_ret_t   R_PKCD_ChkPKCCBusy(void)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if ((PKCD_CMN_ICUM_PKCCB0CR & PKCD_CMN_PKCC_START) != PKCD_CMN_PKCC_START)
    {
        ret = (PKCD_CMN_PKCC_OK == PKCD_CMN_ICUM_PKCCB0STR) ? PKCD_RET_OK : PKCD_RET_PKCCERR;
    }
    else
    {
        ret = PKCD_RET_PROCESSING;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_ChkPKCCBusy()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_IsAsyncOpeProgress
**********************************************************************************************************************/
e_pkc_bool_t    R_PKCD_IsAsyncOpeProgress(void)
{
    return s_pkcd_is_async_ope;
}
/**********************************************************************************************************************
* End of function R_PKCD_IsAsyncOpeProgress()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_ClearState
**********************************************************************************************************************/
pkcd_ret_t   R_PKCD_ClearState(void)
{
    const pkcd_ret_t ret = PKCD_RET_OK;

    /* clear PKCCRAM */
    r_pkcd_cmn_ram_clr((uint8_t*)PKCD_CMN_PKCCRAM, PKCD_CMN_PKCCRAMSIZE);

    /* Clear async flag */
    r_pkcd_cmn_clr_async_ope();

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_ClearState()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : R_PKCD_GetVersion
**********************************************************************************************************************/
pkcd_ret_t R_PKCD_GetVersion(uint32_t* const p_major, uint32_t* const p_minor)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    /* Check NULL argument */
    if ((NULL != p_major) && (NULL != p_minor))
    {
        /* Get major version */
        *p_major = PKCD_CMN_VERSION_MAJOR;

        /* Get minor version */
        *p_minor = PKCD_CMN_VERSION_MINOR;

        ret = PKCD_RET_OK;
    }
    else
    {
        ret = PKCD_RET_PARAMERR;
    }

    return ret;
}
/**********************************************************************************************************************
* End of function R_PKCD_GetVersion()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_cpy_by_64bounds
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Copy data from/to #PKCD_CMN_PKCCRAM to/from external memory.
 * \param       [out] p_dest      A destination address which points 32bits alignment
 * \param       [in]  p_src       A source address which points 32bit alignment
 * \param       [in]  len         Copying length in bytes, which must be multiple of 4.
 * \return
 *  - None
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Copy data from/to #PKCD_CMN_PKCCRAM to/from external memory, <br>
 *    so that it can follow the #PKCD_CMN_PKCCRAM data arragement spec.
 *  - A data on external RAM is considered as a big endian binary, <br>
 *    and a data on #PKCD_CMN_PKCCRAM is considered as <br>
 *    a bigendian sequence of 64bit units which endianness is little.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# if src is #PKCD_CMN_PKCCRAM and dest is #PKCD_CMN_PKCCRAM then
 *   -# Set src's swap of 4-byte units value
 *   -# Set dest's swap of 4-byte units value
 *   -# Loop while i is less than len<br>
 *      Add 4 to i for each loop
 *     -# Copy src to dest in word unit considering swap
 *     -# Increment src and dest to next word
 *     -# Reverse swap sign
 * -# else if src is #PKCD_CMN_PKCCRAM and dest is external RAM
 *   -# Set src's swap of 4-byte units value
 *   -# Loop while i is less than len<br>
 *      Add 4 to i for each loop
 *     -# Copy src which converted endianness considering swap to word dest in word units
 *     -# Increment src and dest to next word
 *     -# Reverse swap sign
 * -# else if src is external RAM and dest is #PKCD_CMN_PKCCRAM
 *   -# Set dest's swap of 4-byte units value
 *   -# Loop while i is less than len<br>
 *      Add 4 to i for each loop
 *     -# Copy src which converted endianness considering swap to word dest in word units
 *     -# Increment src and dest to next word
 *     -# Reverse swap sign
 * -# else
 *   -# Copy with #r_pkcd_cmn_cpy_word()
 * -# Return
 *
 * \callgraph
 *********************************************************************************************************************/
void r_pkcd_cmn_cpy_by_64bounds(uint8_t* const p_dest, const uint8_t* const p_src, const uint32_t len)
{
    uint32_t i;
    int32_t  pkcc_swap_src;
    int32_t  pkcc_swap_dest;
    const uint32_t * p_src32 = (const uint32_t*)p_src; /* No problem with casting to pointer size */
    uint32_t * p_dest32 = (uint32_t*)p_dest; /* No problem with casting to pointer size */

    /* Casts that do not exceed the size of the type are fine */
    if (((PKCD_CMN_PKCCRAM & (uintptr_t)p_src) == PKCD_CMN_PKCCRAM) &&
        ((PKCD_CMN_PKCCRAM & (uintptr_t)p_dest) == PKCD_CMN_PKCCRAM))  /* Same cast as above */
    {
        /* Source(PKCD_CMN_PKCCRAM) - Destination(PKCD_CMN_PKCCRAM) */
        pkcc_swap_src  = (((uintptr_t)p_src32 % PKCD_CMN_PKCCBOUNDARYSIZE) != 0U) ? (-1) : 1;  /* Same cast as above */
        pkcc_swap_dest = (((uintptr_t)p_dest32 % PKCD_CMN_PKCCBOUNDARYSIZE) != 0U) ? (-1) : 1; /* Same cast as above */
        for (i = 0U; i < len; i += PKCD_CMN_WORD_BYTE_SIZE)
        {
            *(p_dest32 + pkcc_swap_dest) = *(p_src32 + pkcc_swap_src);
            p_src32++;
            p_dest32++;
            pkcc_swap_src = (1 == pkcc_swap_src) ? (-1) : 1;
            pkcc_swap_dest = (1 == pkcc_swap_dest) ? (-1) : 1;
        }
    }
    /* Casts that do not exceed the size of the type are fine */
    else if ((PKCD_CMN_PKCCRAM & (uintptr_t)p_src32) == PKCD_CMN_PKCCRAM)
    {
        /* Source(PKCD_CMN_PKCCRAM) - Destination(External RAM) */
        pkcc_swap_src  = (((uintptr_t)p_src32 % PKCD_CMN_PKCCBOUNDARYSIZE) != 0U) ? (-1) : 1;
        for (i = 0U; i < len; i += PKCD_CMN_WORD_BYTE_SIZE)
        {
            *p_dest32 = r_pkcd_cmn_ch_endian32(*(p_src32 + pkcc_swap_src));
            p_src32++;
            p_dest32++;
            pkcc_swap_src = (1 == pkcc_swap_src) ? (-1) : 1;
        }
    }
    /* Casts that do not exceed the size of the type are fine */
    else if ((PKCD_CMN_PKCCRAM & (uintptr_t)p_dest32) == PKCD_CMN_PKCCRAM)
    {
        /* Source(External RAM) - Destination(PKCD_CMN_PKCCRAM) */
        pkcc_swap_dest = (((uintptr_t)p_dest32 % PKCD_CMN_PKCCBOUNDARYSIZE) != 0U) ? (-1) : 1;
        for (i = 0U; i < len; i += PKCD_CMN_WORD_BYTE_SIZE)
        {
            *(p_dest32 + pkcc_swap_dest) = r_pkcd_cmn_ch_endian32(*p_src32);
            p_src32++;
            p_dest32++;
            pkcc_swap_dest = (1 == pkcc_swap_dest) ? (-1) : 1;
        }
    }
    else
    {
        r_pkcd_cmn_cpy_word(p_dest, p_src, len);
    }
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_cpy_by_64bounds()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_cpy_word
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Simple word-wise copy function.
 * \param       [out] p_dest      Destination address
 * \param       [in] p_src        Source address
 * \param       [in] len          Copying length in byte
 * \return
 *  - None
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Simple word-wise copy function.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
void r_pkcd_cmn_cpy_word(uint8_t* const p_dest, const uint8_t* const p_src, const uint32_t len)
{
    const uint32_t *   p_src32 =  (const uint32_t*)p_src; /* No problem with casting to pointer size */
    uint32_t *   p_dest32 = (uint32_t*)p_dest;
    uint32_t    i;

    for (i = 0U; i < len; i += PKCD_CMN_WORD_BYTE_SIZE)
    {
        *p_dest32 = *p_src32;
        p_dest32++;
        p_src32++;
    }
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_cpy_word()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_cpy_byte
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Simple byte-wise copy function.
 * \param       [out] p_dest      Destination address
 * \param       [in] p_src        Source address
 * \param       [in] len          Copying length in byte
 * \return
 *  - None
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Simple byte-wise copy function.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
void r_pkcd_cmn_cpy_byte(uint8_t* const p_dest, const uint8_t* const p_src, const uint32_t len)
{
    const uint32_t    word_len = (len / PKCD_CMN_WORD_BYTE_SIZE)*PKCD_CMN_WORD_BYTE_SIZE;
    const uint32_t    rem_len = (len % PKCD_CMN_WORD_BYTE_SIZE);
    uint32_t    i;

    if (0U != word_len)
    {
        r_pkcd_cmn_cpy_word (p_dest, p_src, word_len);
    }
    else
    {
        /*Do nothing */
    }

    for (i = 0U; i < rem_len; i++ )
    {
        p_dest[word_len + i] = p_src[word_len + i];
    }
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_cpy_byte()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_cmp_word
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Simple word-wise compare function.
 * \param       [in] p_a      Comparison area address a
 * \param       [in] p_b      Comparison area address b
 * \param       [in] len      Comparing length in byte
 * \return
 *  - 0: equal, 1: not equal
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Simple word-wise compare function.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
int32_t r_pkcd_cmn_cmp_word(const uint8_t* const p_a, const uint8_t* const p_b, const uint32_t len)
{
    const uint32_t *   p_a32 = (const uint32_t*)p_a; /* No problem with casting to pointer size */
    const uint32_t *   p_b32 = (const uint32_t*)p_b; /* No problem with casting to pointer size */
    uint32_t    i;
    int32_t     cmp_ret = 1;
    uint32_t    unmatch_flg = 0U;

    for (i = 0U; i < len; i += PKCD_CMN_WORD_BYTE_SIZE)
    {
        if ((*p_a32) == (*p_b32))
        {
            /*Do nothing */
        }
        else
        {
            unmatch_flg = 1U;
        }
        p_a32++;
        p_b32++;
    }

    if (0U == unmatch_flg)
    {
        cmp_ret = 0;
    }
    else
    {
        cmp_ret = 1;
    }

    return cmp_ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_cmp_word()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_cmp_zero_word
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Function to compare with zero.
 * \param       [in] p_a      Comparison area address a
 * \param       [in] len      Comparing length in byte
 * \return
 *  - 0: equal, 1: not equal
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Function to compare with zero.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
int32_t r_pkcd_cmn_cmp_zero_word(const uint8_t* const p_a, const uint32_t len)
{
    const uint32_t *   p_a32 = (const uint32_t*)p_a; /* No problem with casting to pointer size */
    uint32_t    i;
    int32_t     cmp_ret = 1;
    uint32_t    unmatch_flg = 0U;

    for (i = 0U; i < len; i += PKCD_CMN_WORD_BYTE_SIZE)
    {
        if (0U == (*p_a32))
        {
            /*Do nothing */
        }
        else
        {
            unmatch_flg = 1U;
        }
        p_a32++;
    }

    if (0U == unmatch_flg)
    {
        cmp_ret = 0;
    }
    else
    {
        cmp_ret = 1;
    }

    return cmp_ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_cmp_zero_word()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_cmp_work_unit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Comparison function between WORKUNIT.
 * \param       [in] p_wk_a          Address of a work area which contains A data.
 * \param       [in] p_wk_b          Address of a work area which contains B data.
 * \param       [in] p_wk_1          Address of a work area 1.
 * \param       [in] data_len        The length of data.
 * \param       [in] wk_unit_size    The length of work areas.
 * \param       [out] p_cmp_ret      Comparison result.
 *                                         - 0: equal
 *                                         - 1: not equal
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPS, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR must be configured beforehand.
 * \par Detailed description
 *      - Comparison function between WORKUNIT.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_cmp_work_unit(const uint8_t* const p_wk_a, const uint8_t* const p_wk_b, uint8_t* const p_wk_1,
                                    const uint32_t data_len, const uint32_t wk_unit_size, int32_t* const p_cmp_ret)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint8_t sign_trial[PKCD_CMN_PKCCBOUNDARYSIZE];
    uint8_t sign_trial2[PKCD_CMN_PKCCBOUNDARYSIZE];

    /* a - b */
    r_pkcd_cmn_ram_clr(p_wk_1, wk_unit_size);
    ret = r_pkcd_cmn_AMinusB(p_wk_a, p_wk_b, p_wk_1, wk_unit_size);
    r_pkcd_cmn_cpy_by_64bounds(sign_trial, p_wk_1 + ((wk_unit_size - data_len) - PKCD_CMN_PKCCBOUNDARYSIZE),
                                PKCD_CMN_PKCCBOUNDARYSIZE);

    /* b - a */
    r_pkcd_cmn_ram_clr(p_wk_1, wk_unit_size);
    ret |= r_pkcd_cmn_AMinusB(p_wk_b, p_wk_a, p_wk_1, wk_unit_size);
    r_pkcd_cmn_cpy_by_64bounds(sign_trial2, p_wk_1 + ((wk_unit_size - data_len) - PKCD_CMN_PKCCBOUNDARYSIZE),
                                PKCD_CMN_PKCCBOUNDARYSIZE);

    /* Casts that do not exceed the size of the type are fine */
    if (((uint32_t)sign_trial[PKCD_CMN_LSB_POS] == 0U) && ((uint32_t)sign_trial2[PKCD_CMN_LSB_POS] != 0U))
    {
        *p_cmp_ret = 1;
    }
    /* Casts that do not exceed the size of the type are fine */
    else if (((uint32_t)sign_trial[PKCD_CMN_LSB_POS] != 0U) && ((uint32_t)sign_trial2[PKCD_CMN_LSB_POS] == 0U))
    {
        *p_cmp_ret = -1;
    }
    else
    {
        *p_cmp_ret = 0;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_cmp_work_unit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_ram_clr
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Clear data in PKCC RAM.
 * \param       [out] p_dest      Destination address
 * \param       [in] len          Clearing length in byte
 * \return
 *  - None
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Clear data in PKCC RAM.
 *
 * Pseudo-code for the function can be found below.
 * -# Calculate words size of len
 * -# Loop while i is less than words size<br>
 *    Increment i for each loop
 *   -# Set 0x00000000 to dest
 *   -# Add 4 to dest address
 * -# if len % 4 != 0 then
 *   -# Set the value of dest to temp
 *   -# Calculate shift value from (32 - (4 - (len % 4)) * 8)
 *   -# Shift temp to the right by shift value
 *   -# Shift temp to the left by shift value
 *   -# Set the value of temp to dest
 * -# Return
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
void r_pkcd_cmn_ram_clr(uint8_t* const p_dest, const uint32_t len)
{
    uint32_t i;
    const uint32_t words = len / PKCD_CMN_WORD_BYTE_SIZE;
    uint32_t shift;
    uint32_t * p_dest32 = (uint32_t*)p_dest;  /* No problem with casting to pointer size */
    volatile uint32_t    temp;

    for (i = 0U; i < words; i++ )
    {
        *p_dest32 = 0x00000000U;
        p_dest32++;
    }
    if ((len % PKCD_CMN_WORD_BYTE_SIZE) != 0U)
    {
        temp = *p_dest32;
        shift = (sizeof(uint32_t) * PKCD_CMN_BITSOFBYTE)
                - ((PKCD_CMN_WORD_BYTE_SIZE - (len % PKCD_CMN_WORD_BYTE_SIZE)) * PKCD_CMN_BITSOFBYTE);
        temp >>= shift;
        temp <<= shift;
        *p_dest32 = temp;
    }
    else
    {
        /*Do nothing */
    }
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_ram_clr()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_ch_endian32
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Change endianness of 32bit value
 * \param       [in] val      A target 32bit value
 * \return
 *  - A 32bit value which endianness is converted
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Change endianness of 32bit value.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
uint32_t r_pkcd_cmn_ch_endian32(const uint32_t val)
{
    uint32_t temp;

    temp = (val & PKCD_CMN_CH_ENDIAN_CALC_1B) << PKCD_CMN_CH_ENDIAN_SHIFT_24_BIT;
    temp += ((val & PKCD_CMN_CH_ENDIAN_CALC_2B) << PKCD_CMN_CH_ENDIAN_SHIFT_8_BIT);
    temp += ((val & PKCD_CMN_CH_ENDIAN_CALC_3B) >> PKCD_CMN_CH_ENDIAN_SHIFT_8_BIT);
    temp += ((val & PKCD_CMN_CH_ENDIAN_CALC_4B) >> PKCD_CMN_CH_ENDIAN_SHIFT_24_BIT);
    return temp;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_ch_endian32()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_run
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC
 * \param       [in] mode               Operation mode.
 * \param       [in] sync_option        Indicate if the function returns while PKCC is running. <br>
 *                                          Following value can be selected.
 *                                            - PKC_SYNC_OPT_SYNC  : Don't return while PKCC is running
 *                                            - PKC_SYNC_OPT_ASYNC : Returns while PKCC is running
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR
 * \par Precondition
 *  - PKCC arithmetic data must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC.
 *
 * Pseudo-code for the function can be found below.
 * -# 1 is set to the ERROVFCLR, ERRCLR and PKCST_C bit of the register #PKCD_CMN_ICUM_PKCCB0STC
 * -# Set the mode to register #PKCD_CMN_ICUM_PKCCB0OMR
 * -# If SYNC option == #PKC_SYNC_OPT_SYNC then
 *   -# timeout_cnt is set to 0
 *   -# The BEME bit of the register #PKCD_CMN_ICUM_PKCCB0CR is set to 1
 *   -# The PKCSTRT bit of the register #PKCD_CMN_ICUM_PKCCB0CR is set to 1
 *   -# While #PKCD_CMN_PKCD_OPE_TIMEOUT is greater than timeout_cnt and PKCSTRT bit of the register <br>
 *      #PKCD_CMN_ICUM_PKCCB0CR is 1
 *     -# Increment timeout_cnt
 *   -# If #PKCD_CMN_PKCD_OPE_TIMEOUT > timeout_cnt
 *     -# If #PKCD_CMN_ICUM_PKCCB0STR == 1 is true, set ret to #PKCD_RET_OK, otherwise set ret to #PKCD_RET_PKCCERR
 *   -# Else
 *     -# Set ret to #PKCD_RET_PKCCTIMEOUTERR
 * -# Else
 *   -# Set async operation flag with the #r_pkcd_cmn_set_async_ope
 *   -# The BEME bit of the register #PKCD_CMN_ICUM_PKCCB0CR is set to 1
 *   -# The PKCSTRT bit of the register #PKCD_CMN_ICUM_PKCCB0CR is set to 1
 *   -# Set ret to #PKCD_RET_OK
 * -# Return ret
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_run(const uint32_t mode, const e_pkc_sync_opt_t sync_option)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    volatile uint32_t timeout_cnt;

    PKCD_CMN_ICUM_PKCCB0STC = PKCD_CMN_CLR_STATUS;
    PKCD_CMN_ICUM_PKCCB0OMR = (PKCD_CMN_ICUM_PKCCB0OMR & (~PKCD_CMN_OMR_MD_MASK)) | mode;

    if (PKC_SYNC_OPT_SYNC == sync_option)
    {
        /* SYNC mode */
        timeout_cnt = 0U;

    #if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 0x1U)
        switch (mode)
        {
            case PKCD_CMN_MODE_APLUSBMODC:
            case PKCD_CMN_MODE_AMINUSBMODC:
            case PKCD_CMN_MODE_AB:
            case PKCD_CMN_MODE_AB64:
            case PKCD_CMN_MODE_APLUSB:
            case PKCD_CMN_MODE_AMINUSB:
            case PKCD_CMN_MODE_ADIV2:
            case PKCD_CMN_MODE_MINUSCINV:
                /* The operation does NOT support BEME == 1, so set BEME to 0. */
                PKCD_CMN_ICUM_PKCCB0CR =  PKCD_CMN_PKCC_BEME_0;
                PKCD_CMN_ICUM_PKCCB0CR |= PKCD_CMN_PKCC_START;
                break;
            default:
                /* BEME == 1 */
                PKCD_CMN_ICUM_PKCCB0CR =  PKCD_CMN_PKCC_BEME;
                PKCD_CMN_ICUM_PKCCB0CR |= PKCD_CMN_PKCC_START;
                break;
        }
    #else
        /* BEME == 0 */
        PKCD_CMN_ICUM_PKCCB0CR =  PKCD_CMN_PKCC_BEME;
        PKCD_CMN_ICUM_PKCCB0CR |= PKCD_CMN_PKCC_START;
    #endif

        while ((PKCD_CMN_PKCD_OPE_TIMEOUT > timeout_cnt) &&
                ((PKCD_CMN_ICUM_PKCCB0CR & PKCD_CMN_PKCC_START) == PKCD_CMN_PKCC_START))
        {
            timeout_cnt += 1U;
        }

        if (PKCD_CMN_PKCD_OPE_TIMEOUT > timeout_cnt)
        {
            ret = (PKCD_CMN_PKCC_OK == PKCD_CMN_ICUM_PKCCB0STR) ? PKCD_RET_OK : PKCD_RET_PKCCERR;
        }
        else
        {
            ret = PKCD_RET_PKCCTIMEOUTERR;
        }
    }
    else
    {
        /* ASYNC mode */
        r_pkcd_cmn_set_async_ope();

    #if (PKCD_CFG_OVF_WORKAROUND_ENABLE == 0x1U)
        switch (mode)
        {
            case PKCD_CMN_MODE_APLUSBMODC:
            case PKCD_CMN_MODE_AMINUSBMODC:
            case PKCD_CMN_MODE_AB:
            case PKCD_CMN_MODE_AB64:
            case PKCD_CMN_MODE_APLUSB:
            case PKCD_CMN_MODE_AMINUSB:
            case PKCD_CMN_MODE_ADIV2:
            case PKCD_CMN_MODE_MINUSCINV:
                /* The operation does NOT support BEME == 1, so set BEME to 0. */
                PKCD_CMN_ICUM_PKCCB0CR =  PKCD_CMN_PKCC_BEME_0;
                PKCD_CMN_ICUM_PKCCB0CR |= PKCD_CMN_PKCC_START;
                break;
            default:
                /* BEME == 1 */
                PKCD_CMN_ICUM_PKCCB0CR =  PKCD_CMN_PKCC_BEME;
                PKCD_CMN_ICUM_PKCCB0CR |= PKCD_CMN_PKCC_START;
                break;
        }
    #else
        /* BEME == 0 */
        PKCD_CMN_ICUM_PKCCB0CR =  PKCD_CMN_PKCC_BEME;
        PKCD_CMN_ICUM_PKCCB0CR |= PKCD_CMN_PKCC_START;
    #endif

        ret = PKCD_RET_OK;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_run()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_clr_work_unit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Clear data in PKCC RAM using S = A - B.
 * \param       [in,out] p_wk         Address of a work area which is to be cleared.
 * \param       [in] wk_unit_size     The length of work area.
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPS
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR must be configured beforehand.
 * \par Detailed description
 *      - Clear any area in #PKCD_CMN_PKCCRAM using S = A - B.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_clr_work_unit(const uint8_t* const p_wk, const uint32_t wk_unit_size)
{
    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_CMN_FORMAT_DP(p_wk + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_CMN_FORMAT_DP(p_wk + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_CMN_FORMAT_DP(p_wk + wk_unit_size);
    return (r_pkcd_cmn_AMinusB(p_wk, p_wk, p_wk, wk_unit_size));
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_clr_work_unit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_get_lsb_work_unit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Get least significant bit in workunit.
 * \param       [in] p_wk             Address of a work area.
 * \param       [in] wk_unit_size     The length of work area.
 * \return
 *  - Get lsb.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Get least significant bit in workunit.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
uint8_t r_pkcd_cmn_get_lsb_work_unit(const uint8_t* const p_wk, const uint32_t wk_unit_size)
{
    uint32_t  buf[PKCD_CMN_ARRAY32_SIZE(PKCD_CMN_PKCCBOUNDARYSIZE)];
    uint8_t * const p_buf = (uint8_t*)buf;  /* No problem with casting to pointer size */

    r_pkcd_cmn_cpy_by_64bounds(p_buf, p_wk + (wk_unit_size - PKCD_CMN_PKCCBOUNDARYSIZE),
                                PKCD_CMN_PKCCBOUNDARYSIZE);

    return p_buf[PKCD_CMN_LSB_POS];
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_get_lsb_work_unit()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_set_async_ope
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Set async operation flag.
 * \return
 *  - None
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcd_is_async_ope
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Set async operation flag.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
void r_pkcd_cmn_set_async_ope(void)
{
    s_pkcd_is_async_ope = PKC_BOOL_TRUE;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_set_async_ope()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_clr_async_ope
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Clear async operation flag.
 * \return
 *  - None
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcd_is_async_ope
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Clear async operation flag.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
void r_pkcd_cmn_clr_async_ope(void)
{
    s_pkcd_is_async_ope = PKC_BOOL_FALSE;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_clr_async_ope()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_set_dl
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Set PKCC data length, DL and BDL.
 * \return
 *  - None
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcd_is_async_ope
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0OMR
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Set PKCC data length, DL and BDL.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
void r_pkcd_cmn_set_dl(const uint32_t dl, const uint32_t bdl)
{
    PKCD_CMN_ICUM_PKCCB0OMR &= (~(PKCD_CMN_OMR_DL_MASK | PKCD_CMN_OMR_BDL_MASK));
    PKCD_CMN_ICUM_PKCCB0OMR |= ((((bdl + PKCD_CMN_ADD_7) >> PKCD_CMN_SHIFT_3)
                                    << PKCD_CMN_OMR_BDL_SHIFT) +
                                    (((dl  + PKCD_CMN_ADD_7) >> PKCD_CMN_SHIFT_3)
                                    << PKCD_CMN_OMR_DL_SHIFT));
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_set_dl()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_is_multiple_4
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Fold multiplication of 4.
 * \param       [in] val      Value to perform multiple determination.
 * \return
 *  - PKC_BOOL_TRUE:       Multiples of 4.
 *  - PKC_BOOL_FALSE:      Not Multiples of 4.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Fold multiplication of 4.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
e_pkc_bool_t r_pkcd_cmn_is_multiple_4(const uint32_t val)
{
    return ((val & PKCD_CMN_REMAINDER_DIV4) == 0U) ? PKC_BOOL_TRUE : PKC_BOOL_FALSE;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_is_multiple_4()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_extract_result
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Extract return value
 * \param       [in] ored_ret      ORed result
 * \return
 *  - Return value extracted from ORed result
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Extract the return value from the ORed result.
 *  - For the following return values that may be ORed.
 *    - #PKCD_RET_INTERNALERR, #PKCD_RET_PKCCTIMEOUTERR, #PKCD_RET_PKCCERR, #PKCD_RET_RSA_NON_COPRIME,
 *      #PKCD_RET_RSA_COMPOSITE, #PKCD_RET_EC_VERIFAIL, #PKCD_RET_EC_ZERO_SIGNATURE, #PKCD_RET_EDDSA_VERIFAIL,
 *      #PKCD_RET_EDDSA_DECOMPERR
 *  - If the result to be extracted is not found, the input result is returned.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_extract_result(const pkcd_ret_t ored_ret)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    if ((ored_ret & PKCD_RET_INTERNALERR) == PKCD_RET_INTERNALERR)
    {
        ret = PKCD_RET_INTERNALERR;
    }
    else if ((ored_ret & PKCD_RET_PKCCTIMEOUTERR) == PKCD_RET_PKCCTIMEOUTERR)
    {
        ret = PKCD_RET_PKCCTIMEOUTERR;
    }
    else if ((ored_ret & PKCD_RET_PKCCERR) == PKCD_RET_PKCCERR)
    {
        ret = PKCD_RET_PKCCERR;
    }
    else if ((ored_ret & PKCD_RET_RSA_NON_COPRIME) == PKCD_RET_RSA_NON_COPRIME)
    {
        ret = PKCD_RET_RSA_NON_COPRIME;
    }
    else if ((ored_ret & PKCD_RET_RSA_COMPOSITE) == PKCD_RET_RSA_COMPOSITE)
    {
        ret = PKCD_RET_RSA_COMPOSITE;
    }
    else if ((ored_ret & PKCD_RET_DH_INVALID_SHAREDSECRET) == PKCD_RET_DH_INVALID_SHAREDSECRET)
    {
        ret = PKCD_RET_DH_INVALID_SHAREDSECRET;
    }
    else if ((ored_ret & PKCD_RET_EC_VERIFAIL) == PKCD_RET_EC_VERIFAIL)
    {
        ret = PKCD_RET_EC_VERIFAIL;
    }
    else if ((ored_ret & PKCD_RET_EC_ZERO_SIGNATURE) == PKCD_RET_EC_ZERO_SIGNATURE)
    {
        ret = PKCD_RET_EC_ZERO_SIGNATURE;
    }
    else if ((ored_ret & PKCD_RET_EDDSA_VERIFAIL) == PKCD_RET_EDDSA_VERIFAIL)
    {
        ret = PKCD_RET_EDDSA_VERIFAIL;
    }
    else if ((ored_ret & PKCD_RET_EDDSA_DECOMPERR) == PKCD_RET_EDDSA_DECOMPERR)
    {
        ret = PKCD_RET_EDDSA_DECOMPERR;
    }
    else if ((ored_ret & PKCD_RET_PROCESSING) == PKCD_RET_PROCESSING)
    {
        ret = PKCD_RET_PROCESSING;
    }
    else if ((ored_ret & PKCD_RET_OK) == PKCD_RET_OK)
    {
        ret = PKCD_RET_OK;
    }
    else
    {
        /* Illegal results entered */
        ret = PKCD_RET_INTERNALERR;
    }

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_extract_result()
**********************************************************************************************************************/
