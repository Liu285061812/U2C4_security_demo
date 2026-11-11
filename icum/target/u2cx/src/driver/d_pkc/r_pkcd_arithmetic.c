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
/* PRQA S 0310,0316,3305 ++ # D005 In order not to disclose the structure used internally, after receiving
                        it with type uint8_t* or void*, cast it to the type of the structure. */
/* PRQA S 0311 ++ # D006 No problem because the value of the target variable is not changed in the subsequent
                        processing.*/
/* PRQA S 0751 ++ # D009 For commonality of functions, confirm that the size is correct and use the cast value. */
/* PRQA S 0488 ++ # D007 Consider not to overrun the size of the array pointed to by the pointer.*/
/* PRQA S 1532 ++ # D014 Since files are grouped by function, they are called from different files. */
/* PRQA S 2981 ++ # D020 Local variable ret of APIs sets INTERNALERR to the initial value
                        as a countermeasure against attacks that pass through processing. */
/* PRQA S 3120 ++ # D022 Because of the constant value determined by the algorithm. */
/* PRQA S 3383, 3384 ++ # D018 Have confirmed in advance that the value doesn't wrap around. */
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
#include "r_pkcd_rsa.h"

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

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_ABRInvModC
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC with S = ABR^(-1) mod C.
 * \param       [in] p_wk_A          Address of a work area which contains A data.
 * \param       [in] p_wk_B          Address of a work area which contains B data.
 * \param       [in] p_wk_C          Address of a work area which contains C data. If NULL is passed,<br>
 *                                   current DPC value is used.
 * \param       [out] p_wk_S         Address of a work area where S data is output.
 * \param       [in] wk_unit_size    The length of work areas.
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0DPT and DPT data must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC with S = ABR^(-1) mod C mode.
 *      - Each data must be aligned with the bottom of the work area.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_ABRInvModC(const uint8_t* const p_wk_A, const uint8_t* const p_wk_B,
                                    const uint8_t* const p_wk_C, const uint8_t* const p_wk_S,
                                    const uint32_t wk_unit_size)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_CMN_FORMAT_DP(p_wk_A + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_CMN_FORMAT_DP(p_wk_B + wk_unit_size);
    if (NULL != p_wk_C)
    {
        PKCD_CMN_ICUM_PKCCB0DPC = PKCD_CMN_FORMAT_DP(p_wk_C + wk_unit_size);
    }
    else
    {
        /*Do nothing */
    }
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_CMN_FORMAT_DP(p_wk_S + wk_unit_size);
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_ABRINVMODC, PKC_SYNC_OPT_SYNC);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_ABRInvModC()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_ARInvModC
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC with S = AR^(-1).
 * \param       [in] p_wk_A          Address of a work area which contains A data.
 * \param       [in] p_wk_C          Address of a work area which contains C data.<br>
 *                                   If NULL is passed, current DPC value is used.
 * \param       [out] p_wk_S         Address of a work area where S data is output.
 * \param       [in] wk_unit_size    The length of work areas.
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0DPT and DPT data must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC with S = AR^(-1) mod C mode. <br>
 *      - Each data must be aligned with the bottom of the work area.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_ARInvModC(const uint8_t* const p_wk_A, const uint8_t* const p_wk_C, const uint8_t* const p_wk_S,
                                const uint32_t wk_unit_size)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_CMN_FORMAT_DP(p_wk_A + wk_unit_size);
    if (NULL != p_wk_C)
    {
        PKCD_CMN_ICUM_PKCCB0DPC = PKCD_CMN_FORMAT_DP(p_wk_C + wk_unit_size);
    }
    else
    {
        /*Do nothing */
    }
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_CMN_FORMAT_DP(p_wk_S + wk_unit_size);
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_ARINVMODC, PKC_SYNC_OPT_SYNC);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_ARInvModC()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_APlusBModC
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC with S = A + B mod C.
 * \param       [in] p_wk_A          Address of a work area which contains A data.
 * \param       [in] p_wk_B          Address of a work area which contains B data.
 * \param       [in] p_wk_C          Address of a work area which contains C data. <br>
 *                                   If NULL is passed, current DPC value is used.
 * \param       [out] p_wk_S         Address of a work area where S data is output.
 * \param       [in] wk_unit_size    The length of work areas.
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0DPT and DPT data must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC with S = A + B mod C mode.
 *      - Each data must be aligned with the bottom of the work area.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_APlusBModC(const uint8_t* const p_wk_A, const uint8_t* const p_wk_B,
                                    const uint8_t* const p_wk_C, const uint8_t* const p_wk_S,
                                    const uint32_t wk_unit_size)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_CMN_FORMAT_DP(p_wk_A + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_CMN_FORMAT_DP(p_wk_B + wk_unit_size);
    if (NULL != p_wk_C)
    {
        PKCD_CMN_ICUM_PKCCB0DPC = PKCD_CMN_FORMAT_DP(p_wk_C + wk_unit_size);
    }
    else
    {
        /*Do nothing */
    }
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_CMN_FORMAT_DP(p_wk_S + wk_unit_size);
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_APLUSBMODC, PKC_SYNC_OPT_SYNC);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_APlusBModC()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_AMinusBModC
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC with S = A - B mod C.
 * \param       [in] p_wk_A          Address of a work area which contains A data.
 * \param       [in] p_wk_B          Address of a work area which contains B data.
 * \param       [in] p_wk_C          Address of a work area which contains C data.<br>
 *                                   If NULL is passed, current DPC value is used.
 * \param       [out] p_wk_S         Address of a work area where S data is output.
 * \param       [in] wk_unit_size    The length of work areas.
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0DPT and DPT data must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC with S = A - B mod C mode.
 *      - Each data must be aligned with the bottom of the work area.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_AMinusBModC(const uint8_t* const p_wk_A, const uint8_t* const p_wk_B,
                                    const uint8_t* const p_wk_C, const uint8_t* const p_wk_S,
                                    const uint32_t wk_unit_size)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_CMN_FORMAT_DP(p_wk_A + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_CMN_FORMAT_DP(p_wk_B + wk_unit_size);
    if (NULL != p_wk_C)
    {
        PKCD_CMN_ICUM_PKCCB0DPC = PKCD_CMN_FORMAT_DP(p_wk_C + wk_unit_size);
    }
    else
    {
        /*Do nothing */
    }
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_CMN_FORMAT_DP(p_wk_S + wk_unit_size);
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_AMINUSBMODC, PKC_SYNC_OPT_SYNC);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_AMinusBModC()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_APlusB
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC with S = A + B.
 * \param       [in] p_wk_A          Address of a work area which contains A data.
 * \param       [in] p_wk_B          Address of a work area which contains B data.
 * \param       [out] p_wk_S         Address of a work area where S data is output.
 * \param       [in] wk_unit_size    The length of work areas.
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPS
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC with S = A + B.
 *      - Each data must be aligned with the bottom of the work area.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_APlusB(const uint8_t* const p_wk_A, const uint8_t* const p_wk_B,
                                const uint8_t* const p_wk_S, const uint32_t wk_unit_size)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_CMN_FORMAT_DP(p_wk_A + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_CMN_FORMAT_DP(p_wk_B + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_CMN_FORMAT_DP(p_wk_S + wk_unit_size);
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_APLUSB, PKC_SYNC_OPT_SYNC);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_APlusB()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_AMinusB
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC with S = A - B.
 * \param       [in] p_wk_A          Address of a work area which contains A data.
 * \param       [in] p_wk_B          Address of a work area which contains B data.
 * \param       [out] p_wk_S         Address of a work area where S data is output.
 * \param       [in] wk_unit_size    The length of work areas.
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPS
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC with S = A - B.
 *      - Each data must be aligned with the bottom of the work area.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_AMinusB(const uint8_t* const p_wk_A, const uint8_t* const p_wk_B,
                                const uint8_t* const p_wk_S, const uint32_t wk_unit_size)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_CMN_FORMAT_DP(p_wk_A + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_CMN_FORMAT_DP(p_wk_B + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_CMN_FORMAT_DP(p_wk_S + wk_unit_size);
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_AMINUSB, PKC_SYNC_OPT_SYNC);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_AMinusB()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_ADiv2
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC with S = A / 2.
 * \param       [in] p_wk_A          Address of a work area which contains A data.
 * \param       [out] p_wk_S         Address of a work area where S data is output.
 * \param       [in] wk_unit_size    The length of work areas.
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPS
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC with S = A / 2 .
 *      - Each data must be aligned with the bottom of the work area.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_ADiv2(const uint8_t* const p_wk_A, const uint8_t* const p_wk_S, const uint32_t wk_unit_size)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_CMN_FORMAT_DP(p_wk_A + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_CMN_FORMAT_DP(p_wk_S + wk_unit_size);
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_ADIV2, PKC_SYNC_OPT_SYNC);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_ADiv2()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_AB
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC with S = A x B. (n-bit x n-bit mode)
 * \param       [in] p_wk_A          Address of a work area which contains A data.
 * \param       [in] p_wk_B          Address of a work area which contains B data.
 * \param       [out] p_wk_S         Address of a work area where S data is output.
 * \param       [in] wk_unit_size    The length of work areas.
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPS
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC with S = A x B. (n-bit x n-bit mode)
 *      - Each data must be aligned with the bottom of the work area.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_AB(const uint8_t* const p_wk_A, const uint8_t* const p_wk_B,
                            const uint8_t* const p_wk_S, const uint32_t wk_unit_size)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_CMN_FORMAT_DP(p_wk_A + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_CMN_FORMAT_DP(p_wk_B + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_CMN_FORMAT_DP(p_wk_S + (wk_unit_size * PKCD_CMN_MUL_2));
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_AB, PKC_SYNC_OPT_SYNC);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_AB()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_AB64
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC with S = A x B. (64 x n-bit mode)
 * \param       [in] p_wk_A          Address of a work area which contains A data.
 * \param       [in] p_wk_B          Address of a work area which contains B data.
 * \param       [out] p_wk_S         Address of a work area where S data is output.
 * \param       [in] wk_unit_size    The length of work areas.
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPS
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC with S = A x B. (64 x n-bit mode)
 *      - Each data must be aligned with the bottom of the work area.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_AB64(const uint8_t* const p_wk_A, const uint8_t* const p_wk_B,
                            const uint8_t* const p_wk_S, const uint32_t wk_unit_size)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_CMN_FORMAT_DP(p_wk_A + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_CMN_FORMAT_DP(p_wk_B + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_CMN_FORMAT_DP(p_wk_S + wk_unit_size);
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_AB64, PKC_SYNC_OPT_SYNC);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_AB64()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_AExpBModC
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC with S = A^B mod C.
 * \param       [in] p_wk_A             Address of a work area which contains A data.
 * \param       [in] p_wk_B             Address of a work area which contains B data.
 * \param       [in] p_wk_C             Address of a work area which contains C data.<br>
 *                                      If NULL is passed, current DPC value is used.
 * \param       [out] p_wk_S            Address of a work area where S data is output.
 * \param       [in] wk_unit_size       The length of work areas.
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0DPT and DPT data must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC with S = A^B mod C mode.
 *      - Each data must be aligned with the bottom of the work area.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_AExpBModC(const uint8_t* const p_wk_A, const uint8_t* const p_wk_B,
                                const uint8_t* const p_wk_C, const uint8_t* const p_wk_S, const uint32_t wk_unit_size,
                                const e_pkc_sync_opt_t sync_option)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_CMN_FORMAT_DP(p_wk_A + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_CMN_FORMAT_DP(p_wk_B + wk_unit_size);
    if (NULL != p_wk_C)
    {
        PKCD_CMN_ICUM_PKCCB0DPC = PKCD_CMN_FORMAT_DP(p_wk_C + wk_unit_size);
    }
    else
    {
        /*Do nothing */
    }
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_CMN_FORMAT_DP(p_wk_S + wk_unit_size);
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_AEXPBMODC, sync_option);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_AExpBModC()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_AExpBModCCon
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC with S = A^B mod C constant time.
 * \param       [in] p_wk_A             Address of a work area which contains A data.
 * \param       [in] p_wk_B             Address of a work area which contains B data.
 * \param       [in] p_wk_C             Address of a work area which contains C data. <br>
 *                                      If 0 is passed, current DPC value is used.
 * \param       [out] p_wk_S            Address of a work area where S data is output.
 * \param       [in] wk_unit_size       The length of work areas.
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0DPT and DPT data must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC with S = A^B mod C constant time mode.
 *      - Each data must be aligned with the bottom of the work area.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_AExpBModCCon(const uint8_t* const p_wk_A, const uint8_t* const p_wk_B,
                                    const uint8_t* const p_wk_C, const uint8_t* const p_wk_S,
                                    const uint32_t wk_unit_size, const e_pkc_sync_opt_t sync_option)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPA = PKCD_CMN_FORMAT_DP(p_wk_A + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPB = PKCD_CMN_FORMAT_DP(p_wk_B + wk_unit_size);
    if (NULL != p_wk_C)
    {
        PKCD_CMN_ICUM_PKCCB0DPC = PKCD_CMN_FORMAT_DP(p_wk_C + wk_unit_size);
    }
    else
    {
        /*Do nothing */
    }
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_CMN_FORMAT_DP(p_wk_S + wk_unit_size);
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_AEXPBMODC_CON, sync_option);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_AExpBModCCon()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_MinusCInv
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC with T = -C^(-1) mod 2^64.
 * \param       [in] p_wk_C           Address of a work area which contains C data.
 * \param       [out] p_dpT           Address of a work area where S data is output.
 * \param       [in] wk_unit_size     The length of work areas.
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPT
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC with T = -C^(-1) mod 2^64 mode.
 *      - Each data must be aligned with the bottom of the work area.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_MinusCInv(const uint8_t* const p_wk_C, const uint8_t* const p_dpT, const uint32_t wk_unit_size)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPC = PKCD_CMN_FORMAT_DP(p_wk_C + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPT = PKCD_CMN_FORMAT_DP(p_dpT);
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_MINUSCINV, PKC_SYNC_OPT_SYNC);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_MinusCInv()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_R2ModC
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Run PKCC with S = R^2 mod C.
 * \param       [in] p_wk_C          Address of a work area which contains C data.
 * \param       [out] p_wk_S         Address of a work area where S data is output.
 * \param       [in] wk_unit_size    The length of work areas.
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS
 *  - #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0DPT and DPT data must be configured beforehand.
 * \par Detailed description
 *      - Run PKCC with S = R^2 mod C mode.
 *      - Each data must be aligned with the bottom of the work area.
 * \par Remarks
 *  - None
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_R2ModC(const uint8_t* const p_wk_C, const uint8_t* const p_wk_S, const uint32_t wk_unit_size)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;

    PKCD_CMN_ICUM_PKCCB0DPC = PKCD_CMN_FORMAT_DP(p_wk_C + wk_unit_size);
    PKCD_CMN_ICUM_PKCCB0DPS = PKCD_CMN_FORMAT_DP(p_wk_S + wk_unit_size);
    ret = r_pkcd_cmn_run(PKCD_CMN_MODE_R2MODC, PKC_SYNC_OPT_SYNC);
    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_R2ModC()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_modinv_fermat
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Calculates modular inversion with Fermat's little theorem
 * \param       [in] p_wk_A            An address of work area which contains a value <br>
 *                                     which modular inverse is to be calculated. The value must be aligned with bottom.
 * \param       [in] p_wk_mod          An address of work area which contains a modulus. <br>
 *                                     The value must be aligned with bottom.
 * \param       [out] p_wk_out         An address of work area where R^2 mod modulus is set. <br>
 *                                     The data must be aligned with bottom.
 * \param       [in] p_wk1             An address of work area
 * \param       [in] wk_size           Byte length of work area
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0DPT and its T data must be configured beforehand.
 * \par Detailed description
 *      - Calculates modular inversion with Fermat's little theorem.
 *      - The target value and modulus must be co-prime.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Clear p_wk_out
 * -# Set little endian 0x00000002 to temp
 * -# Copy temp to p_wk_out
 * -# Calculate (p - 2) with PKCC (A - B) mod C
 * -# Clear p_wk_out
 * -# Calculate k^(-1) mod p = k^(p - 2) mod p with PKCC A^B mod C
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_modinv_fermat(const uint8_t* const p_wk_A, const uint8_t* const p_wk_mod,
                                    uint8_t* const p_wk_out, const uint8_t* const p_wk1, const uint32_t wk_size)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    uint32_t    temp;

    /* calculate (p - 2) */
    r_pkcd_cmn_ram_clr(p_wk_out, wk_size);
    temp = r_pkcd_cmn_ch_endian32(0x00000002U);
    r_pkcd_cmn_cpy_by_64bounds(p_wk_out + (wk_size - sizeof(uint32_t)),
                                (uint8_t*)&temp, sizeof(uint32_t)); /* No problem with casting to pointer size */
    ret = r_pkcd_cmn_AMinusBModC(p_wk_mod, p_wk_out, p_wk_mod, p_wk1, wk_size);

    /* calculate k^(-1) mod p = k^(p - 2) mod p */
    r_pkcd_cmn_ram_clr(p_wk_out, wk_size);
    ret |= r_pkcd_cmn_AExpBModC(p_wk_A, p_wk1, p_wk_mod, p_wk_out, wk_size, PKC_SYNC_OPT_SYNC);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_modinv_fermat()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_reduce_small_modulus
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Calculates remainder of double size value
 * \param       [in] p_a           An address of divident. Must be aligned to a 4 byte boundary.
 * \param       [in] p_wk_mod      An address of work area where modulus is set. <br>
 *                                 The data must be aligned with bottom.
 * \param       [in] p_wk_r2mod    An address of work area where R^2 mod modulus is set. <br>
 *                                 The data must be aligned with bottom.
 * \param       [in] a_size        Byte length of a
 * \param       [in] mod_size      Byte length of modulus, make it the same value as the DL size set in PKCC.
 * \param       [in] wk_size       Byte length of work area
 * \param       [in] p_wk1         An address of work area
 * \param       [in] p_wk2         An address of work area
 * \param       [out] wk_out       An address of work area to output
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
 *  - #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0OMR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0STR (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0DPT and its T data must have been initialized beforehand.
 * \par Detailed description
 *      - Calculates remainder of double size value.
 *      - PKCC does not support modulo operations which modulus is too smaller than the dividend. <br>
 *        So, this function separates dividend by two parts and calculates each remainder. <br>
 *        The separation is done as follows...
 *        - a[0~N-1] = a[0~DL-1] + (2^DL) * a[DL~N-1]
 *      - And remainders are calculated over above 2 terms separately.
 *      - The remainder length must be less than 2048.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Clear p_wk1
 * -# Clear p_wk2
 * -# Set mod_size /2 to half_dl
 * -# Set little endian 0x00000001 to temp
 * -# Copy temp to the offset (wk_size - half_dl - 4) position of the p_wk1
 * -# Caluculate (DL)*R mod modulus with PKCC S = ABR^(-1) mod C
 * -# Caluculate (2^DL)*R) mod modulus with PKCC S = ABR^(-1) mod C
 * -# Set a_size - mod_size to a0_size
 * -# Set mod_size to a1_size
 * -# Clear p_wk2
 * -# Copy a to the offset (wk_size - a0_size) position of the p_wk2
 * -# Caluculate a0' = a0 * 2^DL mod modulus with PKCC S = ABR^(-1) mod C
 * -# Copy the  offset (a0_size) position of a to the offset (wk_size - a1_size) position of the p_wk1
 * -# Caluculate a1*R mod modulus with PKCC S = ABR^(-1) mod C
 * -# Caluculate a1' = a1 mod modulus with PKCC S = AR^(-1) mod C
 * -# Caluculate (a0' + a1') mod modulus with PKCC S = A + B mod C
 * -# Copy result of (a0' + a1') mod modulus to wkout
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_reduce_small_modulus(const uint8_t* const p_a, const uint8_t* const p_wk_mod,
                                            const uint8_t* const p_wk_r2mod, const uint32_t a_size,
                                            const uint32_t mod_size, const uint32_t wk_size, uint8_t* const p_wk1,
                                            uint8_t* const p_wk2, uint8_t* const wk_out)
{
    pkcd_ret_t ret = PKCD_RET_INTERNALERR;
    const uint32_t half_dl = mod_size / PKCD_CMN_DIV_2;
    uint32_t   temp;

    /* treat a as a0 * 2^DL + a1, where a0 = a[DL~(N-1)] */
    const uint32_t a0_size = (a_size <= mod_size) ? 0U : (a_size - mod_size);

    /* treat a as a0 * 2^DL + a1, where a1 = a[0~(DL-1)] */
    const uint32_t a1_size = (a_size <= mod_size) ? a_size : mod_size;

    /* calc (2^DL)*R mod l */
    r_pkcd_cmn_ram_clr(p_wk1, wk_size);
    r_pkcd_cmn_ram_clr(p_wk2, wk_size);
    temp = r_pkcd_cmn_ch_endian32(0x00000001U);
    r_pkcd_cmn_cpy_by_64bounds(p_wk1 + ((wk_size - half_dl) - sizeof(uint32_t)),
                                (uint8_t*)&temp, sizeof(uint32_t)); /* No problem with casting to pointer size */
    ret = r_pkcd_cmn_ABRInvModC(p_wk1, p_wk_r2mod, p_wk_mod, p_wk1, wk_size);
    ret |= r_pkcd_cmn_ABRInvModC(p_wk1, p_wk1, NULL, p_wk1, wk_size);

    /* calc a0' = a0 * 2^DL mod modulus */
    r_pkcd_cmn_ram_clr(p_wk2, wk_size);
    r_pkcd_cmn_cpy_by_64bounds(p_wk2 + (wk_size - a0_size), p_a, a0_size);
    ret |= r_pkcd_cmn_ABRInvModC(p_wk1, p_wk2, NULL, p_wk2, wk_size);

    /* calc a1' = a1 mod l */
    r_pkcd_cmn_ram_clr(p_wk1, wk_size);
    r_pkcd_cmn_cpy_by_64bounds(p_wk1 + (wk_size - a1_size), p_a + a0_size, a1_size);
    ret |= r_pkcd_cmn_ABRInvModC(p_wk1, p_wk_r2mod, NULL, p_wk1, wk_size);
    ret |= r_pkcd_cmn_ARInvModC(p_wk1, NULL, p_wk1, wk_size);

    /* calc a0' + a1' mod l */
    ret |= r_pkcd_cmn_APlusBModC(p_wk1, p_wk2, NULL, p_wk1, wk_size);
    r_pkcd_cmn_cpy_by_64bounds(wk_out, p_wk1, wk_size);

    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_reduce_small_modulus()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_find_msb
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Find most significant bit(msb) of the inpputing value.
 * \param       [in]   p_val           Value to find msb.
 * \param       [in]   len             Byte length of value.
 * \return
 *  - The ost significant bit of the value.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Find most significant bit(msb) of the inpputing value, and return bit postion.
      Bit position returns with 1 origin.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Set valiable msb to (len * #PKCD_CMN_BITSOFBYTE).
 * -# Loop while i is len or less increment i for each loop
 *   -# If *p_val == 0x00 then
 *     -# break
 *   -# Else
 *     -# Do nothing
 *   -# Calculate msb -= #PKCD_CMN_BITSOFBYTE
 *   -# Increment p_val
 * -# If i != len then
 *   -# Loop while i is #PKCD_CMN_BITSOFBYTE or less increment i for each loop
 *     -# If *p_val & (uint8_t)(#PKCD_RSA_MASK_80 >> i)) != 0 then
 *       -# break
 *     -# Else
 *       -# Do nothing.
 *     -# Decrement msb
 * -# Else
 *   -# If p_val all 0, set msb 0 to
 * -# Return msb
 *
 * \callgraph
 *********************************************************************************************************************/
uint32_t r_pkcd_cmn_find_msb(const uint8_t* const p_val, const uint32_t len)
{
    uint32_t msb = (len * PKCD_CMN_BITSOFBYTE);
    uint32_t i;
    uint8_t  msb_val;

    /* Calculate the number of valid bits */
    for (i = 0U; i < len; i++ )
    {
        if ((uint32_t)p_val[i] != (uint32_t)0x00U)  /* No problem for casting to support usual arithmetic conversion */
        {
            break;
        }
        else
        {
            /* Do nothing */
        }
        msb -= PKCD_CMN_BITSOFBYTE;  /* The upper 8 bits are invalid bit */
    }

    if (i != len)
    {
        msb_val = p_val[i];

        for (i = 0U; i < PKCD_CMN_BITSOFBYTE; i++ )
        {
            /* No problem for casting to support usual arithmetic conversion */
            if ((((uint32_t)msb_val) & ((uint32_t)PKCD_RSA_MASK_80 >> i)) != 0U)
            {
                break;
            }
            else
            {
                /* Do nothing */
            }

            msb -= 1U;    /* The upper 1 bit are invalid bit */
        }
    }
    else
    {
        /* if exp all 0 */
        msb = 0U;
    }

    return msb;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_find_msb()
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name : r_pkcd_cmn_reduce_even_modulus
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIsInternal
 * \brief       Calculates the remainder with an even modulus.
 * \param       [in] p_a           An address of divident.
 * \param       [in] p_mod         An address of modulus.
 * \param       [in] a_size        Byte length of a.
 * \param       [in] mod_size      Byte length of modulus.
 * \param       [in] wk_unit_size  Byte length of work area.
 * \param       [in] p_wk1         An address of work area
 * \param       [in] p_wk2         An address of work area
 * \param       [in] p_wk3         An address of work area
 * \param       [out] p_wk_out     An address of work area to output
 * \return
 *  - #PKCD_RET_OK:                  Successfully done.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB,
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *  - #PKCD_CMN_ICUM_PKCCB0OMR must have been configured beforehand.
 * \par Detailed description
 *      - Calculates the remainder with an even modulus.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCD_RET_OK.
 *  -# Set omr to #PKCD_CMN_ICUM_PKCCB0OMR.
 *  -# Set p_wk_a to p_wk1.
 *  -# Set p_wk_mod to p_wk2.
 *  -# Set p_wk_temp to p_wk3.
 *  -# Get the bit length of a with #r_pkcd_cmn_find_msb.
 *  -# Get the bit length of mod with #r_pkcd_cmn_find_msb.
 *  -# Clear p_wk1 with #r_pkcd_cmn_ram_clr.
 *  -# Clear p_wk2 with #r_pkcd_cmn_ram_clr.
 *  -# Clear p_wk3 with #r_pkcd_cmn_ram_clr.
 *  -# Clear p_wk_out with #r_pkcd_cmn_ram_clr.
 *  -# Copy the data at p_a to p_wk_a with #r_pkcd_cmn_cpy_by_64bounds.
 *  -# Copy the modulus at p_mod to p_wk_mod with #r_pkcd_cmn_cpy_by_64bounds.
 *  -# If a_msb >= mod_msb, then
 *    -# Set msb_diff to (a_msb - mod_msb).
 *    -# Set the PKCC DL with #r_pkcd_cmn_set_dl.
 *    -# Get the little-endian 0x00000002U with #r_pkcd_cmn_ch_endian32, and set temp to the return value.
 *    -# Copy temp to p_wk_temp with r_pkcd_cmn_cpy_by_64bounds.
 *    -# Loop while i < msb_diff
 *      -# Calculate 2 * p_wk_mod with #r_pkcd_cmn_AB64 and add the return value on ret.
 *    -# Set reduce_loop_cnt to 0U.
 *    -# Loop while msb_diff >= reduce_loop_cnt,
 *      -# Compare the values of a and mod with #r_pkcd_cmn_cmp_work_unit.
 *      -# If cmp_ret >= 0, then
 *        -# Calculate (a - mod) with r_pkcd_cmn_AMinusB, set the result to p_wk_a and add the return value on ret.
 *      -# Else
 *        -# Do nothing
 *      -# Calculate p_wk_mod / 2 with #r_pkcd_cmn_ADiv2, and add the return value on ret.
 *      -# reduce_loop_cnt++.
 *    -# Copy p_wk_a to p_wk_out.
 *  -# Else
 *    -# Copy p_wk_a to p_wk_out.
 *  -# Merge the return codes added on ret in the highest-priority one using #r_pkcd_cmn_extract_result.
 *  -# Set #PKCD_CMN_ICUM_PKCCB0OMR to omr.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
pkcd_ret_t r_pkcd_cmn_reduce_even_modulus(const uint8_t* const p_a, const uint8_t* const p_mod,
                                            const uint32_t a_size, const uint32_t mod_size, const uint32_t wk_unit_size,
                                            uint8_t* const p_wk1, uint8_t* const p_wk2, uint8_t* const p_wk3,
                                            uint8_t* const p_wk_out)
{
    uint32_t a_msb;
    pkcd_ret_t ret = PKCD_RET_OK; /* Since nothing may be assigned to ret, set PKCD_RET_OK as the initial value. */
    uint32_t mod_msb;
    uint32_t msb_diff;
    uint32_t reduce_loop_cnt;
    uint32_t i;
    uint8_t * const p_wk_a = p_wk1;
    uint8_t * const p_wk_mod = p_wk2;
    uint8_t * const p_wk_temp = p_wk3;
    int32_t cmp_ret;
    uint32_t temp;
    const uint32_t omr = PKCD_CMN_ICUM_PKCCB0OMR;   /* Backup PKCC OMR register */

    /* Calculate the number of valid bits */
    a_msb = r_pkcd_cmn_find_msb(p_a, a_size);
    mod_msb = r_pkcd_cmn_find_msb(p_mod, mod_size);

    r_pkcd_cmn_ram_clr(p_wk1, wk_unit_size);
    r_pkcd_cmn_ram_clr(p_wk2, wk_unit_size);
    r_pkcd_cmn_ram_clr(p_wk3, wk_unit_size);
    r_pkcd_cmn_ram_clr(p_wk_out, wk_unit_size);

    r_pkcd_cmn_cpy_by_64bounds(p_wk_a + (wk_unit_size - a_size), p_a, a_size);
    r_pkcd_cmn_cpy_by_64bounds(p_wk_mod + (wk_unit_size - mod_size), p_mod, mod_size);

    if (a_msb >= mod_msb)
    {
        /* Calculate the difference between p_random msb position and p_q msb position */
        msb_diff = a_msb - mod_msb;

        /* set DL */
        r_pkcd_cmn_set_dl(a_size, 0U);

        /* Calculate twice p_q msb_diff times. */
        temp = r_pkcd_cmn_ch_endian32(0x00000002U);
        r_pkcd_cmn_cpy_by_64bounds(p_wk_temp + (wk_unit_size - sizeof(uint32_t)),
                                    (uint8_t*)&temp, sizeof(uint32_t)); /* No problem with casting to pointer size */

        for (i = 0U; i < msb_diff; i++ )
        {
            /* Double is calculated by A * 2. */
            ret |= r_pkcd_cmn_AB64(p_wk_temp, p_wk_mod, p_wk_mod, wk_unit_size);
        }

        /* Repeat (r - q') to calculate r mod q.
           q' is q * 2 * (msb_diff), and divide by 2 for each loop.
           Loop msb_diff + 1 times*/
        reduce_loop_cnt = 0U;
        do
        {
            ret |= r_pkcd_cmn_cmp_work_unit(p_wk_a, p_wk_mod, p_wk_temp, a_size, wk_unit_size, &cmp_ret);

            if (cmp_ret >= 0)
            {
                ret |= r_pkcd_cmn_AMinusB(p_wk_a, p_wk_mod, p_wk_a, wk_unit_size);
            }
            else
            {
                /* Do nothing */
            }

            ret |= r_pkcd_cmn_ADiv2(p_wk_mod, p_wk_mod, wk_unit_size);

            reduce_loop_cnt++;
        } while (msb_diff >= reduce_loop_cnt);

        r_pkcd_cmn_cpy_word(p_wk_out, p_wk_a, wk_unit_size);
    }
    else
    {
        /* Since p_random < p_q, so private key p_random mod p_q equale p_random. */
        r_pkcd_cmn_cpy_word(p_wk_out, p_wk_a, wk_unit_size);
    }

    /* Extract the return value from the ORed result */
    ret = r_pkcd_cmn_extract_result(ret);

    /* RestoreBackup PKCC OMA register */
    PKCD_CMN_ICUM_PKCCB0OMR = omr;


    return ret;
}
/***********************************************************************************************************************
* End of function r_pkcd_cmn_reduce_even_modulus()
**********************************************************************************************************************/
