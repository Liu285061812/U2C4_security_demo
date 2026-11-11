/******************************************************************************/
/* Component Name   ICU-M PKC Driver                                          */
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

#ifndef R_PKCD_API_H
/* Multiple inclusion protection macro */
#define R_PKCD_API_H

/* PRQA S 0776 ++ # D012 Develop according to C99. */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

/*!**************************************************************************************
 * \addtogroup  PKCDriver       PKC-Driver module
 * \{
 * \addtogroup  PKCCommon       PKC-Lib/Driver Common definitions
 * \addtogroup  PKCDCommon      PKC-Driver Common module
 * \addtogroup  PKCDRSA         RSA module
 * \addtogroup  PKCDDH          Diffie-Hellman module
 * \addtogroup  PKCDEC          Elliptic curve module
 * \addtogroup  PKCDEDDSA       Edwards curve digital signature algorithm module
 * \addtogroup  PKCDECDHX       Edwards curve Diffie-Hellman module
 * \}***********************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!****************************************************
 * \addtogroup  PKCDCommonDefines   Common Definitions
 * \{**************************************************/
/** PKC-Driver return value definition */
#define PKCD_RET_OK                         ((pkcd_ret_t)0x55000001U)  /**< API succeeded */
#define PKCD_RET_PROCESSING                 ((pkcd_ret_t)0x55000002U)  /**< API succeeded(Need to call the same API <br>
                                                                            repeatedly because some processing <br>
                                                                            is not completed) */
#define PKCD_RET_PRIME_GENERATING           ((pkcd_ret_t)0x55000003U)  /**< Prime number is being generated. <br>
                                                                            The same API needs to be called again <br>
                                                                            because the prime number generation <br>
                                                                            has not been completed. */
#define PKCD_RET_INTERNALERR                ((pkcd_ret_t)0xAA000010U)  /**< A internal failure */
#define PKCD_RET_PKCCTIMEOUTERR             ((pkcd_ret_t)0xAA000020U)  /**< PKCC operation timeout error */
#define PKCD_RET_PKCCERR                    ((pkcd_ret_t)0xAA000040U)  /**< PKCC operation error */
#define PKCD_RET_RSA_NON_COPRIME            ((pkcd_ret_t)0xAA100080U)  /**< A combination that is not relatively <br>
                                                                            prime has occurred. */
#define PKCD_RET_RSA_COMPOSITE              ((pkcd_ret_t)0xAA100100U)  /**< Given number is a composite number */
#define PKCD_RET_DH_INVALID_SHAREDSECRET    ((pkcd_ret_t)0xAA200200U)  /**< An invalid SharedSecret was generated */
#define PKCD_RET_EC_VERIFAIL                ((pkcd_ret_t)0xAA300400U)  /**< ECDSA verification error  */
#define PKCD_RET_EC_ZERO_SIGNATURE          ((pkcd_ret_t)0xAA300800U)  /**< A zero signature is generated. */
#define PKCD_RET_EDDSA_VERIFAIL             ((pkcd_ret_t)0xAA401000U)  /**< EdDSA verification error */
#define PKCD_RET_EDDSA_DECOMPERR            ((pkcd_ret_t)0xAA402000U)  /**< Failed to decompress compressed <br>
                                                                            coordinates */
#define PKCD_RET_PARAMERR                   ((pkcd_ret_t)0xAA800001U)  /**< An invalid argument was entered */
#define PKCD_RET_LENGTHERR                  ((pkcd_ret_t)0xAA800002U)  /**< An invalid length of argument */
#define PKCD_RET_KEY_PARAMERR               ((pkcd_ret_t)0xAA800003U)  /**< An invalid key binary data is entered */
#define PKCD_RET_KEY_LENGTHERR              ((pkcd_ret_t)0xAA800004U)  /**< An invalid length of key binary */
#define PKCD_RET_HASHAPIERR                 ((pkcd_ret_t)0xAA800005U)  /**< An error was returned from the Hash API <br>
                                                                            of the external library */
#define PKCD_RET_DRBGAPIERR                 ((pkcd_ret_t)0xAA800006U)  /**< An error was returned from the DRBG API <br>
                                                                            of the external library */
#define PKCD_RET_RSA_RANGEERR               ((pkcd_ret_t)0xAA800007U)  /**< RSA key parameter candidates are <br>
                                                                            within an invalid range */
#define PKCD_RET_TAMPERERR                  ((pkcd_ret_t)0xAA800008U)  /**< Error due to tampering */
/*! \}*/

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/*!****************************************************
 * \addtogroup  PKCDCommonTypes      Common Types
 * \{**************************************************/
typedef uint32_t pkcd_ret_t;                  /**< PKC-Driver return type */

/** RSA execute type value enumeration */
typedef enum
{
    PKCD_RSA_EXEC_TYPE_ENC = 0,             /**< Call for encryption */
    PKCD_RSA_EXEC_TYPE_DEC,                 /**< Call for decryption */
    PKCD_RSA_EXEC_TYPE_SIGN,                /**< Call for signature */
    PKCD_RSA_EXEC_TYPE_VERIFY               /**< Call for verification */
} e_pkcd_rsa_exec_type_t;

/*! \}*/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
/**********************************************************************************************************************
* Function Name : R_PKCD_Initialize
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIs
 * \brief       Initialize PKC-Driver
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 * \par Security components
 *  - No
 * \par Global Variables
 *   - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Clear all the RAM used by PKC-Driver and async flag
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Set ret to #PKCD_RET_OK
 * -# Clear PKCCRAM with #r_pkcd_cmn_ram_clr
 * -# Clear async flag with #r_pkcd_cmn_clr_async_ope
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_Initialize (void);

/**********************************************************************************************************************
* Function Name : R_PKCD_ChkPKCCBusy
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIs
 * \brief       Check if PKCC is running.
 * \return
 *  - #PKCD_RET_OK:                  PKCC has finished operation w/o any errors.
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed.
 *  - #PKCD_RET_PKCCERR:             Any PKCC errors occurred.
 *  - #PKCD_RET_PROCESSING:          PKCC is still running.
 * \par Security components
 *  - No
 * \par Global Variables
 *   - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0STR
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - After a program returns from the update APIs with async option, the PKCC is likely still running. <br>
 *    This API is called for checking it.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Set ret to #PKCD_RET_PROCESSING
 * -# If PKCC is not running then
 *   -# If only the "PKCC status flag bit" of the status register is set, set ret to #PKCD_RET_OK
 *   -# Else set ret to #PKCD_RET_PKCCERR
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_ChkPKCCBusy (void);

/**********************************************************************************************************************
* Function Name : R_PKCD_IsAsyncOpeProgress
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIs
 * \brief       Check if the last called API is asynchronous execution
 * \return
 *  - PKC_BOOL_TRUE:               The last called PKC-Driver API ran in asynchronous mode
 *  - PKC_BOOL_FALSE:              The last called PKC-Driver API ran in synchronous mode
 * \par Security components
 *  - No
 * \par Global Variables
 *   - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *   - Check whether the last called API was executed synchronously or asynchronously.
 *   - This API is intended for use within PKCC's interrupt service.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Return #s_pkcd_is_async_ope
 *
 * \callgraph
 *********************************************************************************************************************/
extern e_pkc_bool_t R_PKCD_IsAsyncOpeProgress (void);

/**********************************************************************************************************************
* Function Name : R_PKCD_ClearState
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIs
 * \brief       - Clear the state of API call in Init/Operate/Final function format.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 * \par Security components
 *  - No
 * \par Global Variables
 *   - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *   - Clear the state of API call in Init/Operate/Final function.
 *   - This API is executed when the API of Init/Operate/Final function is stopped without calling the last Final.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Set ret to #PKCD_RET_OK
 * -# Clear PKCCRAM with #r_pkcd_cmn_ram_clr
 * -# Clear async flag with #r_pkcd_cmn_clr_async_ope
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_ClearState (void);

/**********************************************************************************************************************
* Function Name : R_PKCD_GetVersion
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDCommonAPIs
 * \brief       Get version information of PKC-Driver.
 * \param       [out] p_major       An address of major version
 * \param       [out] p_minor       An address of minor version
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed.
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Get version information of PKC-Driver.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# If (NULL != p_major) and (NULL != p_minor)
 *   -# Set p_major to #PKCD_CMN_VERSION_MAJOR
 *   -# Set p_minor to #PKCD_CMN_VERSION_MINOR
 *   -# Set ret to #PKCD_RET_OK
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_GetVersion (uint32_t * const p_major, uint32_t * const p_minor);

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaModExpInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIs
 * \brief       Prepare for modular exponentiation.
 * \param       [in] p_msg_in       An inputting message.
 * \param       [in] p_key          RSA private or pulic key of key binary format
 * \param       [in] mod_len        Length of the modulus n
 * \param       [in] exec_type      RSA execution type
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by RSA. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 *                                    - p_buf  :                No need to set.
 *                                    - is_constant_time_mode : Must be set.
 *                                    - msg_digest_type :       No need to set.
 *                                    - p_mask_opt :            No need to set.
 *                                    - p_drbgApi :             No need to set.
 *                                    - is_crt_mode :           No need to set.
 *                                    - padding_opt :           No need to set.
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *   - Satisfy both of the following conditions:
 *     - #R_PKCD_Initialize has been called.
 *     - The PKCC must not be running.
 * \par Detailed description
 *    - Prepare for the modular exponentiation operation of the following :
 *      - Clear #PKCD_CMN_PKCCRAM
 *      - Save input parameters in buffer
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_OK
 *   -# Set (#st_rsa_prm_modexp_t*)p_buf to p_prm
 *   -# Check argument with #r_pkcd_rsa_chk_mod_exp_init_arg, and set ret to the return value.
 *   -# If #PKCD_RET_OK == ret then
 *       -# Clear async operation flag with #r_pkcd_cmn_clr_async_ope
 *       -# Clear PKCC RAM with #r_pkcd_cmn_ram_clr
 *       -# Clear buffer with #r_pkcd_cmn_ram_clr
 *       -# Set (const #st_pkcd_rsa_key_bin_t*)p_key to p_key_bin.
 *       -# Prepare modular exponentiation with #r_pkcd_rsa_modexp_init
 *   -# Else
 *     -# Set ret to #PKCD_RET_KEY_PARAMERR
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_RsaModExpInit (const st_pkc_data_t * const p_msg_in,
                                        const void * const p_key,
                                        const uint32_t mod_len,
                                        const e_pkcd_rsa_exec_type_t exec_type,
                                        const st_pkc_rsa_ctx_t * const p_ctx,
                                        uint8_t * const p_buf);

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaModExpOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIs
 * \brief       Perform modular exponentiation.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by RSA. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in] p_operate_opt  Select synchronous / asynchronous execution or processing bits
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPW  #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_RsaModExpInit has been called.
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Perform modular exponentiation.
 *    - Performs processing in one operation by the modular exponentiation operation of PKCC
 *    - Select synchronous or asynchronous execution by setting the argument p_operate_opt->sync_opt
 *    - Select constant time execution or variable time execution <br>
 *      by setting the argument p_ctx->is_constant_time_mode.
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_RsaModExpInit for the argument p_ctx and the argument p_buf. <br>
 *    If you set a value different from #R_PKCD_RsaModExpInit, the result is not guaranteed.
 *  - If the key length is larger than the bit length that HW can process at one time,
 *    It is necessary to call this API repeatedly until all the bits are processed.
 *    If unprocessed bits remain, #PKCD_RET_PROCESSING is returned, and if all bits are processed, <br>
 *    #PKCD_RET_OK is returned.
 *    If the next Final function is called with unprocessed bits remaining, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_OK
 *   -# Set (#st_rsa_prm_modexp_t *)p_buf to p_prm
 *   -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_operate_opt) then
 *     -# If (#PKC_SYNC_OPT_SYNC == p_operate_opt->sync_opt) or (#PKC_SYNC_OPT_ASYNC == p_operate_opt->sync_opt) then
 *       -# Clear async operation flag with #r_pkcd_cmn_clr_async_ope
 *       -# If #PKC_BOOL_FALSE != p_ctx->sec_mod_exp, then
 *         -# Perform modular exponentiation with #r_pkcd_rsa_modexp_sec, and set ret to the return value.
 *       -# Else
 *         -# Perform modular exponentiation with #r_pkcd_rsa_modexp_operate, and set ret to the return value.
 *     -# Else
 *       -# Set ret to #PKCD_RET_PARAMERR
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_RsaModExpOperate (const st_pkc_rsa_ctx_t * const p_ctx,
                                            uint8_t * const p_buf,
                                            const st_pkc_operate_opt_t * const p_operate_opt);

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaModExpFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIs
 * \brief       Finalize a modular exponentioation.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by RSA. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in,out] p_msg_out  Output the message after the modular exponentioation operation <br>
 *                                  Set p_data buffer size in len when inputting <br>
 *                                  At the time of output, set the data in p_data, set the data size in len and return

 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *    - PKCC processing is completed normally
 * \par Detailed description
 *    - Finalize a modular exponentioation.
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_RsaModExpOperate for the argument p_ctx and the argument p_buf. <br>
 *    If you set a value different from #R_PKCD_RsaModExpOperate, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_OK
 *   -# Set (#st_rsa_prm_modexp_t *)p_buf to p_prm
 *   -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_msg_out) then
 *     -# If NULL != p_msg_out->p_data then
 *       -# If p_msg_out->len >= p_prm->mod_len then
 *         -# Clear async operation flag with #r_pkcd_cmn_clr_async_ope
 *         -# Finalize modular exponentiation with #r_pkcd_rsa_modexp_final
 *       -# Else
 *         -# Set ret to #PKCD_RET_LENGTHERR
 *     -# Else
 *       -# Set ret to #PKCD_RET_PARAMERR
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_RsaModExpFinal (const st_pkc_rsa_ctx_t * const p_ctx,
                                            uint8_t * const p_buf,
                                            st_pkc_data_t * const p_msg_out);

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaModExpCRTInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIs
 * \brief       Prepare for modular exponentiation with the Chinese remainder theorem.
 * \param       [in] p_msg_in       An inputting message.
 * \param       [in] p_priv_key     RSA private key of key binary format
 * \param       [in] mod_len        Length of the modulus n
 * \param       [in] exec_type      RSA execution type
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by RSA. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 *                                  * Currently all members are unused, but this argument is set for extensibility.
 *                                    - p_buf  :                No need to set.
 *                                    - is_constant_time_mode : No need to set.
 *                                    - msg_digest_type :       No need to set.
 *                                    - p_mask_opt :            No need to set.
 *                                    - p_drbgApi :             No need to set.
 *                                    - is_crt_mode :           No need to set.
 *                                    - padding_opt :           No need to set.
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *   - Satisfy both of the following conditions:
 *     - #R_PKCD_Initialize has been called.
 *     - The PKCC must not be running.
 * \par Detailed description
 *    - Prepare for the modular exponentiation operation of the following :
 *      - Clear #PKCD_CMN_PKCCRAM
 *      - Save input parameters in buffer
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *   -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 *   -# Set (#st_rsa_prm_modexp_crt_t *)p_buf to p_prm
 *   -# Check argument with #r_pkcd_rsa_chk_mod_exp_crt_init_arg, and set ret to the return value.
 *   -# If #PKCD_RET_OK == ret then
 *     -# Clear async operation flag with #r_pkcd_cmn_clr_async_ope
 *     -# Clear PKCC RAM with #r_pkcd_cmn_ram_clr
 *     -# Clear buffer with #r_pkcd_cmn_ram_clr
 *     -# Set (const #st_pkcd_rsa_key_bin_t*)p_priv_key to p_key_bin.
 *     -# Prepare modular exponentiation with #r_pkcd_rsa_modexp_crt_init, and set ret to the return value.
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_RsaModExpCRTInit (const st_pkc_data_t * const p_msg_in,
                                            const void * const p_priv_key,
                                            const uint32_t mod_len,
                                            const e_pkcd_rsa_exec_type_t exec_type,
                                            const st_pkc_rsa_ctx_t * const p_ctx,
                                            uint8_t * const p_buf);

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaModExpCRTOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIs
 * \brief       Perform modular exponentiation with the Chinese remainder theorem.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by RSA. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in] sync_opt       Select synchronous / asynchronous execution
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_RsaModExpCRTInit has been called.
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Perform modular exponentiation with the Chinese remainder theorem.
 *    - Calculate m1 on the first call and calculate m2 on the second call
 *    - The number of calls is managed by the driver <br>
 *      When the first call ends normally, #PKCD_RET_PROCESSING is returned, <br>
 *      and when the second call ends normally, #PKCD_RET_OK is returned.
 *    - Select synchronous or asynchronous execution by setting the argument sync_opt
 *    - Constant time execution with argument p_ctx->is_constant_time_mode is not supported
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_RsaModExpCRTInit for the argument p_ctx and the argument p_buf. <br>
 *    If you set a value different from #R_PKCD_RsaModExpCRTInit, the result is not guaranteed.
 *  - This API needs to be called twice.
 *    Only one call, no guarantee of results if the next Final function is called.
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_OK
 *   -# Set (#st_rsa_prm_modexp_crt_t *)p_buf to p_prm
 *   -# If ((NULL != p_ctx) and (NULL != p_buf)) and
 *          ((#PKC_SYNC_OPT_SYNC == sync_opt) or (#PKC_SYNC_OPT_ASYNC == sync_opt)) then
 *     -# Clear async operation flag with #r_pkcd_cmn_clr_async_ope
 *     -# Perform modular exponentiation with #r_pkcd_rsa_modexp_crt_operate
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_RsaModExpCRTOperate (const st_pkc_rsa_ctx_t * const p_ctx,
                                                uint8_t * const p_buf,
                                                const e_pkc_sync_opt_t sync_opt);

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaModExpCRTFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIs
 * \brief       Finalize a modular exponentioation with the Chinese remainder theorem.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by RSA. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in,out] p_msg_out  Output the message after the modular exponentioation operation <br>
 *                                  Set p_data buffer size in len when inputting <br>
 *                                  At the time of output, set the data in p_data, set the data size in len and return
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_RsaModExpCRTOperate has been called.
 *      - When asynchronous execution, PKCC processing is completed normally
 * \par Detailed description
 *    - Finalize a modular exponentioation with the Chinese remainder theorem.
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_RsaModExpCRTOperate for the argument p_ctx and the argument p_buf. <br>
 *    If you set a value different from #R_PKCD_RsaModExpCRTOperate, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_OK
 *   -# Set (#st_rsa_prm_modexp_crt_t *)p_buf to p_prm
 *   -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_msg_out) then
 *     -# If NULL != p_msg_out->p_data then
 *       -# If p_msg_out->len >= p_prm->mod_len then
 *         -# Clear async operation flag with #r_pkcd_cmn_clr_async_ope
 *         -# Finalize modular exponentiation with #r_pkcd_rsa_modexp_crt_final
 *       -# Else
 *         -# Set ret to #PKCD_RET_LENGTHERR
 *     -# Else
 *       -# Set ret to #PKCD_RET_PARAMERR
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_RsaModExpCRTFinal (const st_pkc_rsa_ctx_t * const p_ctx,
                                            uint8_t * const p_buf,
                                            st_pkc_data_t * const p_msg_out);

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaKeyGenInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIs
 * \brief       Generate RSA key pair.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by RSA. <br>
 *                                  * Only the p_drbgApi member is used. Other members are not accessed.
 *                                    - p_drbgApi :     A pointer of a DRBG API used for generating prime candidates.
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] key_len        A length of the generating key in bytes.
 * \param       [in] prime_test_num The respective limit number of the trials for prime p, q generation.
 * \param       [in] iteration_num  The number of the iteration of the Miller-Rabin test.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entere
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None.
 * \par Precondition
 *   - Satisfy both of the following conditions:
 *     - #R_PKCD_Initialize has been called.
 *     - The PKCC must not be running.
 * \par Detailed description
 *  - Prepare the HW resources for the RSA key pair generation.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCD_RET_INTERNALERR.
 *  -# If (NULL != p_ctx) then,
 *    -# If ((NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api)) then,
 *      -# If ((#PKCD_RSA_KEY_LEN_1024 == key_len) || (#PKCD_RSA_KEY_LEN_2048 == key_len) ||
 *             (#PKCD_RSA_KEY_LEN_3072 == key_len) || (#PKCD_RSA_KEY_LEN_4096 == key_len)) then,
 *        -# If ((#PKCD_RSA_PRIME_TEST_NUM_MIN <= prime_test_num) and
 *               (#PKCD_RSA_PRIME_TEST_NUM_MAX >= prime_test_num)) then,
 *          -# If ((#PKCD_RSA_ITERATION_NUM_MIN <= iteration_num) and
 *                 (#PKCD_RSA_ITERATION_NUM_MAX >= iteration_num)) then,
 *            -# Set ret to #PKCD_RET_OK.
 *          -# Else
 *            -# Set ret to #PKCD_RET_LENGTHERR.
 *        -# Else
 *          -# Set ret to #PKCD_RET_LENGTHERR.
 *      -# Else
 *        -# Set ret to #PKCD_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# If #PKCD_RET_OK == ret then,
 *    -# Clear the async operation flag with #r_pkcd_cmn_clr_async_ope.
 *    -# Clear PKCCRAM with #r_pkcd_cmn_ram_clr.
 *    -# Assign a buffer for the driver's RSA key generation as p_keygen from p_ctx->p_buf.
 *    -# Clear p_keygen->p_data with #r_pkcd_cmn_ram_clr.
 *    -# Clear p_keygen->q_data with #r_pkcd_cmn_ram_clr.
 *    -# Clear p_keygen->dp_data with #r_pkcd_cmn_ram_clr.
 *    -# Clear p_keygen->dq_data with #r_pkcd_cmn_ram_clr.
 *    -# Clear p_keygen->qinv_data with #r_pkcd_cmn_ram_clr.
 *    -# Set p_keygen->p_len to 0.
 *    -# Set p_keygen->q_len to 0.
 *    -# Set p_keygen->d_len to 0.
 *    -# Set p_keygen->e_len to 0.
 *    -# Set p_keygen->key_len to key_len.
 *    -# Set p_keygen->prime_test_num to prime_test_num.
 *    -# Set p_keygen->iteration_num to iteration_num.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t  R_PKCD_RsaKeyGenInit (const st_pkc_rsa_ctx_t * const p_ctx,
                                        uint8_t * const p_buf,
                                        const uint32_t key_len,
                                        const uint32_t prime_test_num,
                                        const uint32_t iteration_num);

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaKeyGenOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIs
 * \brief       Generate RSA key pair.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in,out] p_buf      Buffer used for the internal processing of PKC-Driver
 * \return
 *  - #PKCD_RET_OK:                  Successful completion
 *  - #PKCD_RET_INTERNALERR:         Internal processing failed
 *  - #PKCD_RET_PARAMERR:            Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:           Invalid argument length entered
 *  - #PKCD_RET_PRIME_GENERATING:    The number of generation fails overed prime_test_num set in #R_PKCD_RsaKeyGenInit.
 *  - #PKCD_RET_DRBGAPIERR:          The DRBG API passed by user returned error.
 *  - #PKCD_RET_PKCCTIMEOUTERR:      PKCC operation timeouted.
 *  - #PKCD_RET_PKCCERR:             An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPW, #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *   - Satisfy both of the following conditions:
 *     - #R_PKCD_Initialize has been called.
 *     - The PKCC must not be running.
 * \par Detailed description
 *  - Generate an RSA key pair from the input prime numbers p and q.
 *  - Public key e uses 65537 (0x10001).
 *  - The key derivation procedure is performed according to appendix B.3.1 in FIPS186-4.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCD_RET_INTERNALERR.
 *  -# Assign a buffer for the driver's RSA key generationas p_keygen from p_ctx->p_buf.
 *  -# If ((NULL != p_ctx) && (NULL != p_buf)) then,
 *    -# If (NULL != p_ctx->p_drbg_api) then,
 *      -# Set ret to #PKCD_RET_OK.
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# If #PKCD_RET_OK == ret, then
 *    -# Clear the Async operation flag with #r_pkcd_cmn_clr_async_ope.
 *    -# Set the public exponent length p_keygen->e_len to #PKCD_RSA_PUB_EXP_FIXED_LEN.
 *    -# Copy the public exponent value pub_exp to p_keygen->e_data with #r_pkcd_cmn_cpy_byte.
 *    -# Generate private prime numbers p, q, and a private exponent d with #r_pkcd_rsa_generate_p_q_n_d,
 *       and set ret to the return value.
 *    -# If #PKCD_RET_OK == ret, then
 *      -# Set the key binary member length key_bin.key_length to p_keygen->key_len.
 *      -# Set the key binary member n address key_bin.p_n to p_keygen->n_data.
 *      -# Set the key binary member d address key_bin.p_d to p_keygen->d_data.
 *      -# Set the key binary member p address key_bin.p_p to p_keygen->p_data.
 *      -# Set the key binary member q address key_bin.p_q to p_keygen->q_data.
 *      -# Set the key binary member dp address key_bin.p_dp to p_keygen->dp_data.
 *      -# Set the key binary member dq address key_bin.p_dq to p_keygen->dq_data.
 *      -# Set the key binary member qinv address key_bin.p_qinv to p_keygen->qinv_data.
 *      -# Set the key binary member e length key_bin.e_length to p_keygen->e_len.
 *      -# Set the key binary member e address key_bin.p_e to p_keygen->e_data.
 *      -# Set is_mod_p to #PKC_BOOL_TRUE.
 *      -# Calculate n, dp, dq, qinv with #r_pkcd_rsa_generate_crt and set ret to the result.
 *    -# Else
 *      -# Do nothing.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t  R_PKCD_RsaKeyGenOperate (const st_pkc_rsa_ctx_t * const p_ctx,
                                            uint8_t * const p_buf);

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaKeyGenFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIs
 * \brief       Output an RSA key pair in the key binary format.
 * \param       [in] p_ctx             A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in,out] p_buf         Buffer used for internal processing of PKC-Driver
 * \param       [in,out] p_priv_key    The output address of the RSA private key (include public key) in the
 *                                     key binary format.
 * \param       [in,out] p_pub_key     The output address of the RSA public key in the key binary format.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None.
 * \par Precondition
 *   - Satisfy the following conditions:
 *     - #R_PKCD_RsaKeyGenOperate has been called just before this API.
 * \par Detailed description
 *  - Encode the RSA parameters generated in #R_PKCD_RsaKeyGenOperate as the key binary format.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCD_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) && (NULL != p_buf) && (NULL != p_priv_key) && (NULL != p_pub_key)) then,
 *    -# Set the private key binary address p_priv_key_bin to (st_pkcd_rsa_key_bin_t*)p_priv_key.
 *    -# If ((NULL != p_priv_key_bin->p_n) && (NULL != p_priv_key_bin->p_d) && (NULL != p_priv_key_bin->p_p) &&
 *           (NULL != p_priv_key_bin->p_q) && (NULL != p_priv_key_bin->p_dp) && (NULL != p_priv_key_bin->p_dq) &&
 *           (NULL != p_priv_key_bin->p_qinv) && (NULL != p_priv_key_bin->p_e)) then,
 *      -# Set the public key binary address p_pub_key_bin to (st_pkcd_rsa_key_bin_t*)p_pub_key.
 *      -# If ((NULL != p_priv_key_bin->p_n) && (NULL != p_priv_key_bin->p_e)) then,
 *        -# Set ret to #PKCD_RET_OK.
 *      -# Else
 *        -# Set ret to #PKCD_RET_KEY_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_KEY_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# If #PKCD_RET_OK == ret then,
 *    -# Clear the async operation flag with #r_pkcd_cmn_clr_async_ope.
 *    -# Assign a buffer for the driver's RSA key generationas p_keygen from p_ctx->p_buf.
 *    -# Set the private key binary length p_priv_key_bin->key_length to p_keygen->key_len.
 *    -# Set e address in the private key binary p_priv_key_bin->e_length to p_keygen->e_len.
 *    -# Copy the public exponent e into p_priv_key_bin->p_e with #r_pkcd_cmn_cpy_word.
 *    -# Copy the modulus n into p_priv_key_bin->p_n with #r_pkcd_cmn_cpy_word.
 *    -# Copy the prime p into p_priv_key_bin->p_p with #r_pkcd_cmn_cpy_word.
 *    -# Copy the prime q into p_priv_key_bin->p_q with #r_pkcd_cmn_cpy_word.
 *    -# Copy the cofactor dp into p_priv_key_bin->p_dp with #r_pkcd_cmn_cpy_word.
 *    -# Copy the cofactor dq into p_priv_key_bin->p_dq with #r_pkcd_cmn_cpy_word.
 *    -# Copy the cofactor qinv into p_priv_key_bin->p_qinv with #r_pkcd_cmn_cpy_word.
 *    -# Copy the prime q into p_priv_key_bin->p_q with #r_pkcd_cmn_cpy_word.
 *    -# Copy the prime q into p_priv_key_bin->p_q with #r_pkcd_cmn_cpy_word.
 *    -# Set the public key binary length p_pub_key_bin->key_length to p_priv_key_bin->key_length.
 *    -# Copy the modulus n into p_pub_key_bin->p_n with #r_pkcd_cmn_cpy_word.
 *    -# Set e length in the public key binary p_pub_key_bin->e_length to p_priv_key_bin->e_length.
 *    -# Copy the public exponent e into p_pub_key_bin->p_e with #r_pkcd_cmn_cpy_word.
 *    -# Clear the buffer for the key generation parameters with #r_pkcd_cmn_ram_clr.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t  R_PKCD_RsaKeyGenFinal (const st_pkc_rsa_ctx_t * const p_ctx,
                                        uint8_t * const p_buf,
                                        void* const p_priv_key,
                                        void* const p_pub_key);

/**********************************************************************************************************************
* Function Name : R_PKCD_RsaTestPrime
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDRSAAPIs
 * \brief       Determine if the entered value is a prime number.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by RSA. <br>
 *                                  * Currently all members are unused, but this argument is set for extensibility.
 *                                    - p_buf  :                No need to set.
 *                                    - is_constant_time_mode : No need to set.
 *                                    - msg_digest_type :       No need to set.
 *                                    - p_mask_opt :            No need to set.
 *                                    - p_drbgApi :             No need to set.
 *                                    - is_crt_mode :           No need to set.
 *                                    - padding_opt :           No need to set.
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] p_candidate    Prime number candidate data.
 * \param       [in] iteration_num  Iteration number for Miller-Rabin test.
 * \param       [in] p_test_randoms Random value used in Miller-Rabin test.
 *                                  Prepare iteration_num random numbers with the same length as p_candidate->len, <br>
 *                                  Set the start address of each random number in the array of this argument.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 *  - #PKCD_RET_RSA_COMPOSITE:            Given number is a composite number.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPA, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *   - Satisfy both of the following conditions:
 *     - #R_PKCD_Initialize has been called.
 *     - The PKCC must not be running.
 * \par Detailed description
 *   - Primality test is performed on the candidate data by the following two tests.
 *     - PrePrime test (Primality test is performed by division by prime numbers up to 16 bits)
 *     - Miller-Rabin test
 *   - The Miller-Rabin algorithm is a probabilistic algorithm, so it has non-zero probability <br>
 *     of erroneous decision.
 *     To reduce the probability to negligible one, the number of the test iteration should be enough big. <br>
 *     Referance values are shown below. <br>
 *     prime bits | iterations | error probabilities
 *    ------------|------------|---------------------
 *     512        | 7          | 4.48E-32
 *     1024       | 4          | 3.82E-35
 *     1536       | 3          | 1.33E-37
 *     2048       | 2          | 2.14E-35
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCD_RET_INTERNALERR.
 *  -# Check the arguments with #r_pkcd_rsa_chk_test_prime_arg and set ret to its return value.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# Clear an asynchronous operation flag with #r_pkcd_cmn_clr_async_ope.
 *      -# Clear #PKCD_CMN_PKCCRAM with #r_pkcd_cmn_ram_clr.
 *      -# Check if the argument p_candidate is prime or not with #r_pkcd_rsa_test_prime
 *         and set ret to its return value.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_RsaTestPrime (const st_pkc_rsa_ctx_t * const p_ctx,
                                        uint8_t * const p_buf,
                                        const st_pkc_data_t * const p_candidate,
                                        const uint32_t iteration_num,
                                        uint8_t * const p_test_randoms[]);


/**********************************************************************************************************************
* Function Name : R_PKCD_DhModExpInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIs
 * \brief       Prepare for modular exponentiation.
 * \param       [in] p_base       An inputting base data. Set one of the following <br>
 *                                  (1) Byte array of base value <br>
 *                                  (2) DH public key of key binary format <br>
 *                                      DH private key including public key of key binary format
 * \param       [in] p_exp        An inputting exponent data. Set one of the following <br>
 *                                  (1) Byte array of exponent value <br>
 *                                  (2) DH private key of key binary format
 * \param       [in] p_ctx        A pointer to a structure that summarizes the parameters required by DH. <br>
 *                                Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf    Buffer used for internal processing of PKC-Driver
 *                                Specify the same buffer with the Init/Operate/Final function
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - None
 * \par Precondition
 *   - Satisfy both of the following conditions:
 *     - #R_PKCD_Initialize has been called.
 *     - The PKCC must not be running.
 * \par Detailed description
 *    - Prepare for the modular exponentiation operation of the following :
 *      - Clear #PKCD_CMN_PKCCRAM
 *      - Save input parameters in buffer
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_INTERNALERR
 *   -# Set (#st_dh_prm_modexp_t*)p_buf to p_prm
 *   -# Check argument with #r_pkcd_dh_chk_mod_exp_init_arg, and set ret to the return value.
 *   -# If #PKCD_RET_OK == ret then
 *       -# Clear async operation flag with #r_pkcd_cmn_clr_async_ope
 *       -# Clear PKCC RAM with #r_pkcd_cmn_ram_clr
 *       -# Clear buffer with #r_pkcd_cmn_ram_clr
 *       -# If #PKC_DATA_TYPE_BYTE == p_base->data_type
 *         -# Set p_base_data to p_base->p_sel_data->p_data
 *         -# Set base_len to p_base->p_sel_data->len
 *       -# Else
 *         -# Set p_base_data to ((st_pkcd_dh_key_bin_t*)p_base->p_sel_data->p_data)->p_public
 *         -# Set base_len to ((st_pkcd_dh_key_bin_t*)p_base->p_sel_data->p_data)->public_key_length
 *       -# If #PKC_DATA_TYPE_BYTE == p_exp->data_type
 *         -# Set p_exp_data to p_exp->p_sel_data->p_data
 *         -# Set exp_len to p_exp->p_sel_data->len
 *       -# Else
 *         -# Set p_exp_data to ((st_pkcd_dh_key_bin_t*)p_exp->p_sel_data->p_data)->p_private
 *         -# Set exp_len to ((st_pkcd_dh_key_bin_t*)p_exp->p_sel_data->p_data)->private_key_length
 *       -# Set (const #st_pkcd_rsa_key_bin_t*)p_key to p_key_bin.
 *       -# Prepare modular exponentiation with #r_pkcd_dh_modexp_init
 *   -# Else
 *     -# Set ret to #PKCD_RET_KEY_PARAMERR
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t   R_PKCD_DhModExpInit (const st_pkc_select_data_t * const p_base,
                                        const st_pkc_select_data_t * const p_exp,
                                        const st_pkc_dh_ctx_t * const p_ctx,
                                        uint8_t * const p_buf);

/**********************************************************************************************************************
* Function Name : R_PKCD_DhModExpOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIs
 * \brief       Perform modular exponentiation.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by DH. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in] p_operate_opt  Select synchronous / asynchronous execution or processing bits
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPW  #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_DhModExpInit has been called.
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Perform modular exponentiation.
 *    - Performs processing in one operation by the modular exponentiation operation of PKCC
 *    - Select synchronous or asynchronous execution by setting the argument p_operate_opt->sync_opt
 *    - Select constant time execution or variable time execution <br>
 *      by setting the argument p_ctx->is_constant_time_mode.
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_DhModExpInit for the argument p_ctx and the argument p_buf. <br>
 *    If you set a value different from #R_PKCD_DhModExpInit, the result is not guaranteed.
 *  - If the key length is larger than the bit length that HW can process at one time,
 *    It is necessary to call this API repeatedly until all the bits are processed.
 *    If unprocessed bits remain, #PKCD_RET_PROCESSING is returned, and if all bits are processed, <br>
 *    #PKCD_RET_OK is returned.
 *    If the next Final function is called with unprocessed bits remaining, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_OK
 *   -# Set p_prm to (#st_rsa_prm_modexp_t *)p_buf
 *   -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_operate_opt) then
 *     -# Check DH domain parameters with #r_pkcd_dh_chk_dh_param, and set ret to the return value.
 *     -# If #PKCD_RET_OK == ret
 *       -# If (#PKC_SYNC_OPT_SYNC == p_operate_opt->sync_opt) or (#PKC_SYNC_OPT_ASYNC == p_operate_opt->sync_opt) then
 *         -# Clear async operation flag with #r_pkcd_cmn_clr_async_ope
 *         -# Perform modular exponentiation with #r_pkcd_dh_modexp_operate
 *       -# Else
 *         -# Set ret to #PKCD_RET_PARAMERR
 *     -# Else
 *       -# Do nothing
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_DhModExpOperate (const st_pkc_dh_ctx_t * const p_ctx,
                                            uint8_t * const p_buf,
                                            const st_pkc_operate_opt_t * const p_operate_opt);

/**********************************************************************************************************************
* Function Name : R_PKCD_DhModExpFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIs
 * \brief       Finalize a modular exponentioation.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by DH. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in,out] p_out      Output the result after the modular exponentioation operation <br>
 *                                  Set p_data buffer size in len when inputting <br>
 *                                  At the time of output, set the data in p_data, set the data size in len and return

 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_DhModExpOperate has been called.
 *      - All bits are processed by #R_PKCD_DhModExpOperate
 * \par Detailed description
 *    - Finalize a modular exponentioation.
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_DhModExpOperate for the argument p_ctx and the argument p_buf. <br>
 *    If you set a value different from #R_PKCD_DhModExpOperate, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_INTERNALERR
 *   -# Set p_prm to (#st_dh_prm_modexp_t *)p_buf
 *   -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_out) then
 *     -# Check DH domain parameters with #r_pkcd_dh_chk_dh_param, and set ret to the return value.
 *     -# If #PKCD_RET_OK == ret
 *       -# If NULL != p_out->p_data then
 *         -# If p_out->len >= p_ctx->p_dh_param->p_p->len then
 *           -# Clear async operation flag with #r_pkcd_cmn_clr_async_ope
 *           -# Finalize modular exponentiation with #r_pkcd_dh_modexp_final
 *         -# Else
 *           -# Set ret to #PKCD_RET_LENGTHERR
 *       -# Else
 *         -# Set ret to #PKCD_RET_PARAMERR
 *     -# Else
 *       -# Do nothing
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_DhModExpFinal (const st_pkc_dh_ctx_t * const p_ctx,
                                        uint8_t * const p_buf,
                                        st_pkc_data_t * const p_out);

/**********************************************************************************************************************
* Function Name : R_PKCD_DhInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIs
 * \brief       Prepare DH key exchange.
 * \param       [in] p_priv_key     DH private key of key binary format
 * \param       [in] p_pub_key      DH public key of key binary format
 * \param       [in] p_ctx        A pointer to a structure that summarizes the parameters required by DH. <br>
 *                                Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf    Buffer used for internal processing of PKC-Driver
 *                                Specify the same buffer with the Init/Operate/Final function
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - None
 * \par Precondition
 *   - Satisfy both of the following conditions:
 *     - #R_PKCD_Initialize has been called.
 *     - The PKCC must not be running.
 * \par Detailed description
 *    - Prepare for the modular exponentiation operation of the following :
 *      - Clear #PKCD_CMN_PKCCRAM
 *      - Save input parameters in buffer
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_INTERNALERR
 *   -# Set the public key structure type base_sel.data_type to #PKC_DATA_TYPE_KEY_BINARY.
 *   -# Set the public key structure content address base_sel.p_sel_data to &pubkey_data.
 *   -# Set the public key address base_sel.p_sel_data->p_data to (uint8_t*)p_pub_key.
 *   -# Set the private key structure type exp_sel.data_type to #PKC_DATA_TYPE_KEY_BINARY.
 *   -# Set the private key structure content address exp_sel.p_sel_data to &privkey_data.
 *   -# Set thr private key address exp_sel.p_sel_data->p_data to (uint8_t*)p_priv_key.
 *   -# Initialize ModularExponentiation with #R_PKCD_DhModExpInit, and set ret to the return value.
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t   R_PKCD_DhInit (const void * const p_priv_key,
                                    const void * const p_pub_key,
                                    const st_pkc_dh_ctx_t * const p_ctx,
                                    uint8_t * const p_buf);

/**********************************************************************************************************************
* Function Name : R_PKCD_DhOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIs
 * \brief       Perform DH key exchange.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by DH. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in] p_operate_opt  Select synchronous / asynchronous execution or processing bits
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - When the key length is 2048 bits or less
 *    - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPW  #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *      #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC, <br>
 *      #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *      #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 *  - When the key length is lager than 2048 bits
 *    - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR. #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *      #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *      #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS,#PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_DhInit has been called.
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Perform modular exponentiation.
 *    - The operation depends on the key length.
 *      - When the key length is 2048 bits or less
 *        - Performs processing in one operation by the modular exponentiation operation of PKCC
 *        - Select synchronous or asynchronous execution by setting the argument p_operate_opt->sync_opt
 *        - Select constant time execution or variable time execution <br>
 *          by setting the argument p_ctx->is_constant_time_mode.
 *      - When the key length is lager than 2048 bits
 *        - Use the binary method to modular exponentiation operations that should be divided into multiple times.
 *        - Indicates the number of bits to be processed at one time <br>
 *          by setting the argument p_operate_op->processing_bits.
 *        - If a value larger than the number of unprocessed bits is set, processing is performed up to the number <br>
 *          of unprocessed bits and the process ends.
 *        - Constant time execution with argument p_ctx->is_constant_time_mode is not supported
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_DhInit for the argument p_ctx and the argument p_buf. <br>
 *    If you set a value different from #R_PKCD_DhInit, the result is not guaranteed.
 *  - If the key length is larger than the bit length that HW can process at one time,
 *    It is necessary to call this API repeatedly until all the bits are processed.
 *    If unprocessed bits remain, #PKCD_RET_PROCESSING is returned, and if all bits are processed, <br>
 *    #PKCD_RET_OK is returned.
 *    If the next Final function is called with unprocessed bits remaining, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_INTERNALERR
 *   -# Perform modular exponentiation with #R_PKCD_DhModExpOperate, and set ret to the return value.
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_DhOperate (const st_pkc_dh_ctx_t * const p_ctx,
                                    uint8_t * const p_buf,
                                    const st_pkc_operate_opt_t * const p_operate_opt);

/**********************************************************************************************************************
* Function Name : R_PKCD_DhFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIs
 * \brief       The shared secret is output
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by DH. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in,out] p_shared_secret      Output the shared secret <br>
 *                                            Set p_data buffer size in len when inputting <br>
 *                                            At the time of output, set the data in p_data, set the data size <br>
 *                                            in len and return
 *
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 *  - #PKCD_RET_DH_INVALID_SHAREDSECRET:  An invalid SharedSecret was generated
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - When the key length is 2048 bits or less
 *    - None
 *  - When the key length is lager than 2048 bits
 *    - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,<br>
 *      #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *      #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_DhOperate has been called.
 *      - Satisfy any of the following
 *        - When the key length is 2048 bits or less and asynchronous execution
 *          - PKCC processing is completed normally
 *        - When the key length is lager than 2048 bits
 *          - All bits are processed by #R_PKCD_DhOperate
 * \par Detailed description
 *    - Finalize a modular exponentioation.
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_DhOperate for the argument p_ctx and the argument p_buf. <br>
 *    If you set a value different from #R_PKCD_DhOperate, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_INTERNALERR
 *   -# Set p_prm to (#st_dh_prm_modexp_t *)p_buf
 *   -# Set array bottom_word_1 to {0x00U, 0x01U, 0x00U, 0x01U}
 *   -# If #PKCD_RET_OK == ret then
 *     -# Check if SharedSecret is 0 with #r_pkcd_cmn_cmp_zero_word, and set cmp_ret to return value.
 *      -# If 0 != cmp_ret then
 *        -# Check if SharedSecret(excepted bottom 4 bytes) is 0 with #r_pkcd_cmn_cmp_zero_word, and set cmp_ret to <br>
 *           return value.
 *        -# Check if SharedSecret(bottom 4 bytes) is 1 with #r_pkcd_cmn_cmp_word, and set cmp_ret2 to return value.
 *        -# If !((0 == cmp_ret) && (0 == cmp_ret2)) then
 *          -# Copy variable bottom_word_p_minus_1 to bottom 4 byte of modulus p with #r_pkcd_cmn_cpy_word
 *          -# Subtract 1 from bottom_word_p_minus_1
 *          -# Check if SharedSecret(excepted bottom 4 bytes) is modulus p - 1(excepted bottom 4 bytes) <br>
 *             with #r_pkcd_cmn_cmp_word, and set cmp_ret to return value.
 *          -# Check if SharedSecret(bottom 4 bytes) is modulus p - 1(bottom 4 bytes) <br>
 *             with #r_pkcd_cmn_cmp_word, and set cmp_ret2 to return value.
 *          -# If !((0 == cmp_ret) && (0 == cmp_ret2)) then
 *            -# Do nothing
 *          -# Else
 *            -# Set ret to #PKCD_RET_DH_INVALID_SHAREDSECRET.
 *        -# Else
 *          -# Set ret to #PKCD_RET_DH_INVALID_SHAREDSECRET.
 *      -# Else
 *        -# Set ret to #PKCD_RET_DH_INVALID_SHAREDSECRET.
 *   -# Else
 *     -# Do nothing
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_DhFinal (const st_pkc_dh_ctx_t * const p_ctx,
                                    uint8_t * const p_buf,
                                    st_pkc_data_t * const p_shared_secret);

/**********************************************************************************************************************
* Function Name : R_PKCD_DhKeyGenInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIs
 * \brief       Prepare DH key generation.
 * \param       [in] p_ctx        A pointer to a structure that summarizes the parameters required by DH. <br>
 *                                Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf    Buffer used for internal processing of PKC-Driver
 *                                Specify the same buffer with the Init/Operate/Final function
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_DRBGAPIERR:               External DRBG API failed
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - None
 * \par Precondition
 *   - Satisfy both of the following conditions:
 *     - #R_PKCD_Initialize has been called.
 *     - The PKCC must not be running.
 * \par Detailed description
 *    - Prepare for the DH key generation of the following :
 *      - Clear #PKCD_CMN_PKCCRAM
 *      - Save input parameters in buffer
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_INTERNALERR
 *   -# Set p_prm to (#st_dh_prm_keygen_t*)p_buf
 *   -# Check argument with #r_pkcd_dh_chk_key_gen_init_arg, and set ret to the return value.
 *   -# If #PKCD_RET_OK == ret then
 *     -# Clear async operation flag with #r_pkcd_cmn_clr_async_ope
 *     -# Clear PKCC RAM with #r_pkcd_cmn_ram_clr
 *     -# Clear buffer with #r_pkcd_cmn_ram_clr
 *     -# Set modexp called flag to #PKC_BOOL_FALSE
 *     -# Set p_prm->random_len to p_ctx->p_dh_param->p_q->len + #PKCD_DH_EXTRA_BYTE_LEN
 *     -# Generate random bytes p_prm->random with into p_ctx->p_drbg_api, and set drbg_ret to the return value.
 *     -# If #PKCD_CMN_EXTERNAL_API_OK == drbg_ret
 *       -# Do nothing
 *     -# Else
 *       -# Set ret to #PKCD_RET_DRBGAPIERR
 *   -# Else
 *     -# Do nothing
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t   R_PKCD_DhKeyGenInit (const st_pkc_dh_ctx_t * const p_ctx,
                                        uint8_t * const p_buf);

/**********************************************************************************************************************
* Function Name : R_PKCD_DhKeyGenOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIs
 * \brief       Perform DH key generation
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by DH. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in] p_operate_opt  Select synchronous / asynchronous execution or processing bits
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPW  #PKCD_CMN_ICUM_PKCCB0CR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPS (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_DhKeyGenInit has been called.
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Perform modular exponentiation.
 *    - Performs processing in one operation by the modular exponentiation operation of PKCC
 *    - Select synchronous or asynchronous execution by setting the argument p_operate_opt->sync_opt
 *    - Select constant time execution or variable time execution <br>
 *      by setting the argument p_ctx->is_constant_time_mode.
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_DhKeyGenInit for the argument p_ctx and the argument p_buf. <br>
 *    If you set a value different from #R_PKCD_DhKeyGenInit, the result is not guaranteed.
 *  - If the key length is larger than the bit length that HW can process at one time,
 *    It is necessary to call this API repeatedly until all the bits are processed.
 *    If unprocessed bits remain, #PKCD_RET_PROCESSING is returned, and if all bits are processed, <br>
 *    #PKCD_RET_OK is returned.
 *    If the next Final function is called with unprocessed bits remaining, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_INTERNALERR
 *   -# Set p_prm to (#st_dh_prm_keygen_t *)p_buf
 *   -# If (NULL != p_ctx) and (NULL != p_buf) and (NULL != p_operate_opt) then
 *     -# Check DH domain parameters with #r_pkcd_dh_chk_dh_param, and set ret to the return value.
 *     -# If #PKCD_RET_OK == ret
 *       -# If NULL != p_ctx->p_drbg_api
 *         -# If (#PKC_SYNC_OPT_SYNC == p_operate_opt->sync_opt) || (#PKC_SYNC_OPT_ASYNC == p_operate_opt->sync_opt)
 *           -# Perform DH key generation with #r_pkcd_dh_keygen_operate, and set ret to the return value.
 *         -# Else
 *           -# Set ret to #PKCD_RET_PARAMERR
 *       -# Else
 *         -# Set ret to #PKCD_RET_PARAMERR
 *     -# Else
 *       -# Do nothing
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_DhKeyGenOperate (const st_pkc_dh_ctx_t * const p_ctx,
                                            uint8_t * const p_buf,
                                            const st_pkc_operate_opt_t * const p_operate_opt);

/**********************************************************************************************************************
* Function Name : R_PKCD_DhKeyGenFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDDHAPIs
 * \brief       Output the generated key pair.
 * \param       [in] p_ctx           A pointer to a structure that summarizes the parameters required by DH. <br>
 *                                   Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                   Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf       Buffer used for internal processing of PKC-Driver
 *                                   Specify the same buffer with the Init/Operate/Final function
 * \param       [in,out] p_priv_key  A pointer to the memory area allocated by the user SW to store the key binary
 *                                   of the DH private key.
 * \param       [in,out] p_pub_key   A pointer to the memory area allocated by the user SW to store the key binary
 *                                   of the DH public key.
 *
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - When the key length is 2048 bits or less
 *    - None
 *  - When the key length is lager than 2048 bits
 *    - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR,<br>
 *      #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *      #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_DhKeyGenOperate has been called.
 *      - Satisfy any of the following
 *        - When the key length is 2048 bits or less and asynchronous execution
 *          - PKCC processing is completed normally
 *        - When the key length is lager than 2048 bits
 *          - All bits are processed by #R_PKCD_DhKeyGenOperate
 * \par Detailed description
 *    - Finalize a modular exponentioation.
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_DhKeyGenOperate for the argument p_ctx and the argument p_buf. <br>
 *    If you set a value different from #R_PKCD_DhKeyGenOperate, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *   -# Set ret to #PKCD_RET_INTERNALERR
 *   -# Set p_prm to (#st_dh_prm_modexp_t *)p_buf
 *   -# Set p_priv_key_bin to (st_pkcd_dh_key_bin_t*)p_priv_key
 *   -# Set p_pub_key_bin to (st_pkcd_dh_key_bin_t*)p_pub_key
 *   -# If (NULL != p_ctx) && (NULL != p_buf) && (NULL != p_priv_key_bin) && (NULL != p_pub_key_bin) then
 *     -# If (NULL != p_priv_key_bin->p_private) && (NULL != p_priv_key_bin->p_public)
 *           && (NULL != p_pub_key_bin->p_public) then
 *       -# Output public key with #R_PKCD_DhModExpFinal, set ret to the return value
 *       -# If #PKCD_RET_OK == ret then
 *         -# Set p_priv_key_bin->private_key_length to p_ctx->p_dh_param->p_q->len
 *         -# Copy p_prm->priv_key to p_priv_key_bin->p_private
 *         -# Set p_priv_key_bin->public_key_length to p_ctx->p_dh_param->p_p->len
 *         -# Copy p_prm->pub_key to p_priv_key_bin->p_public
 *         -# Set p_pub_key_bin->public_key_length to p_ctx->p_dh_param->p_p->len
 *         -# Copy p_prm->pub_key to p_pub_key_bin->p_private
 *         -# Use #r_pkcd_cmn_ram_clr to clear the private key value stored in p_prm.
 *       -# Else
 *         -# Do nothing
 *     -# Else
 *       -# Set ret to #PKCD_RET_KEY_PARAMERR
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR
 *   -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_DhKeyGenFinal (const st_pkc_dh_ctx_t * const p_ctx,
                                        uint8_t * const               p_buf,
                                        void * const                  p_priv_key,
                                        void * const                  p_pub_key);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcScalarMultInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIs
 * \brief       Prepare for scalar multiplication.
 * \param       [in] p_scalar_or_privkey          Set one of the following <br>
 *                                                  (1) Byte array of scalar value <br>
 *                                                  (2) EC private key of key binary format
 * \param       [in] p_point_or_pubkey            Set one of the following <br>
 *                                                  (1) Byte array of point value <br>
 *                                                  (2) EC public key of key binary format <br>
 *                                                      EC private key including public key of key binary format
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 *  - #PKCD_RET_DRBGAPIERR:               External DRBG API failed
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - None
 * \par Precondition
 *   - Satisfy both of the following conditions:
 *     - #R_PKCD_Initialize has been called.
 *     - The PKCC must not be running.
 * \par Detailed description
 *    - Prepare for the scalar multiplication operation of the following :
 *      - Clear #PKCD_CMN_PKCCRAM
 *      - Save input parameters in buffer
 *    - Select whether to execute anti-tampering scalar multiplication or not by the argument p_ctx->sec_scalar_mul.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Set (#st_ec_prm_smult_t*)p_buf to p_prm.
 *  -# Check the arguments with #r_pkcd_ec_chk_scalar_mult_init_arg and set ret to its return value.
 *  -# If (#PKCD_RET_OK == ret)
 *    -# Clear an asynchronous operation flag with #r_pkcd_cmn_clr_async_ope.
 *    -# Clear #PKCD_CMN_PKCCRAM with #r_pkcd_cmn_ram_clr.
 *    -# Clear the buffer p_buf with #r_pkcd_cmn_ram_clr.
 *    -# If (#PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
 *      -# Set (const #st_pkcd_ec_key_bin_t*)p_scalar_or_privkey->p_sel_data->p_data to p_key_bin.
 *      -# Copy p_key_bin->p_d to the buffer p_prm->scalar.scalar with #r_pkcd_cmn_cpy_word.
 *      -# Set the key length to p_prm->scalar.len.
 *    -# Else
 *      -# Set the input scalar data and its length to p_prm->scalar with #r_pkcd_ec_cpy_scalar.
 *    -# If (#PKC_DATA_TYPE_KEY_BINARY == p_point_or_pubkey->data_type)
 *      -# Set (const #st_pkcd_ec_key_bin_t*)p_point_or_pubkey->p_sel_data->p_data to p_key_bin.
 *      -# Copy p_key_bin->p_qx to the buffer p_prm->point.x with #r_pkcd_cmn_cpy_word.
 *      -# Copy p_key_bin->p_qy to the buffer p_prm->point.y with #r_pkcd_cmn_cpy_word.
 *      -# Set the key length to p_prm->point.len.
 *    -# Else
 *      -# Set the input point data and its length p_prm->point with #r_pkcd_ec_cpy_point.
 *    -# If (#PKC_BOOL_FALSE != p_ctx->sec_scalar_mul)
 *      -# Generate random bytes for scalar multiplication with p_ctx->p_drbg_api,
 *         and set drv_ret to the return value. The resulting random bytes is set in p_prm->random_data.
 *      -# If (#PKCD_CMN_EXTERNAL_API_OK == drbg_ret)
 *        -# Generate random bytes for randomizing projection coordinates with #r_pkcd_ec_gen_rand_z,
 *           and set ret to the return value. The resulting random bytes is set in p_prm->random_z.
 *      -# Else
 *        -# Set ret to #PKCD_RET_DRBGAPIERR.
 *    -# Else
 *      -# Do nothing.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcScalarMultInit (const st_pkc_select_data_t * const p_scalar_or_privkey,
                                            const st_pkc_select_data_t * const p_point_or_pubkey,
                                            const st_pkc_ec_ctx_t * const p_ctx,
                                            uint8_t * const p_buf);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcScalarMultOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIs
 * \brief       Perform scalar multiplication for points on elliptic curves.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in] sync_opt       Select synchronous / asynchronous execution
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_XYZWORK1
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPA, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_EcScalarMultInit has been called.
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Perform scalar multiplication for points on elliptic curves.
 *    - Select synchronous execution or asynchronous execution of scalar multiplication operation by argument sync_opt.
 *      - For synchronous execution, wait for the PKCC scalar multiplication process to complete <br>
 *        before returning this API.
 *      - For asynchronous execution, this API is returned immediately <br>
 *        after the PKCC scalar multiplication operation is started.
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_EcScalarMultInit for the argument p_ctx and the argument p_buf. <br>
 *    If you set a value different from #R_PKCD_EcScalarMultInit, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Set (const #st_ec_prm_smult_t*)p_buf to p_prm.
 *  -# If ((NULL != p_ctx) and (NULL != p_buf))
 *         and ((#PKC_SYNC_OPT_SYNC == sync_opt) or (#PKC_SYNC_OPT_ASYNC == sync_opt))
 *    -# Check curve parameters with #r_pkcd_ec_chk_curve_info and set ret to its return value.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# Get curve parameters from p_ctx->p_curve_info with #r_pkcd_ec_parse_curve.
 *    -# Prepare the operation with #r_pkcd_ec_prepare_operation.
 *    -# If (#PKC_BOOL_FALSE != p_ctx->sec_scalar_mul)
 *      -# Calculate scalar multiplication with #r_pkcd_ec_scalar_mul_sec and add its return value to ret.
 *      -# Clear the random data in p_prm->random_data with #r_pkcd_cmn_ram_clr.
 *      -# Clear the random data in p_prm->random_z with #r_pkcd_cmn_ram_clr.
 *    -# Else
 *      -# Calculate scalar multiplication with #r_pkcd_ec_scalar_mul and add its return value to ret.
 *    -# Extract the return value from ret with #r_pkcd_cmn_extract_result and set ret to it.
 *    -# Use #r_pkcd_cmn_ram_clr to clear the potentially private key value stored in p_prm.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcScalarMultOperate (const st_pkc_ec_ctx_t * const p_ctx,
                                                uint8_t * const p_buf,
                                                const e_pkc_sync_opt_t sync_opt);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcScalarMultFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIs
 * \brief       Finalize an EC scalar multiplication.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in,out] p_point    Output coordinate data <br>
 *                                  Output data that concatenates x and y coordinates <br>
 *                                  Set p_data buffer size in len when inputting <br>
 *                                  At the time of output, set the data in p_data, set the data size in len and return
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EC_XYZWORK1
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STR, #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS,  <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy all of the following conditions:
 *      - R_PKCD_EcScalarMultOperate() has been called.
 *      - The PKCC must not be running.
 *      - (When R_PKCD_EcScalarMultOperate is executed asynchronously) <br>
 *        PKCC processing is completed normally
 * \par Detailed description
 *    - Convert scalar multiplied result calculated with projective coordinates to affine coordinates and output
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_EcScalarMultOperate for the argument p_ctx and the argument p_buf.<br>
 *    If you set a value different from #R_PKCD_EcScalarMultOperate, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_buf) and (NULL != p_point))
 *    -# If (NULL != p_point->p_data)
 *      -# Check the curve information with #r_pkcd_ec_chk_curve_info and set ret to the return value.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# If (p_point->len >= #PKCD_EC_POINT_DATA_NUM * curve length)
 *          -# Clear an asynchronous operation flag with #r_pkcd_cmn_clr_async_ope.
 *          -# Get the result of scalar multiplication with #r_pkcd_ec_projective_to_affine.
 *          -# Set #PKCD_EC_POINT_DATA_NUM * curve length to p_point->len.
 *        -# Else
 *          -# Set ret to #PKCD_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcScalarMultFinal (const st_pkc_ec_ctx_t * const p_ctx,
                                            uint8_t * const p_buf,
                                            st_pkc_data_t * const p_point);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcdsaMakeSign
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIs
 * \brief       Generate ECDSA signature.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] p_digest       A message digest.
 * \param       [in] p_point_q      A point derived from {random scalar k} * G(x, y).
 *                                  Data that concatenates the x and y coordinates of the point k*G.
 * \param       [in] p_random       random scalar k
 * \param       [in] p_priv_key     EC private key of key binary format
 * \param       [in,out] p_sign     Output signature data <br>
 *                                  Output data that concatenates signature r and s. <br>
 *                                  Set p_data buffer size in len when inputting. <br>
 *                                  At the time of output, set the data in p_data, set the data size in len and return.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 *  - #PKCD_RET_EC_ZERO_SIGNATURE:        A zero signature is generated.
 *  - #PKCD_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPA, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    -Satisfy both of the following conditions:
 *      - #R_PKCD_Initialize has been called.
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Generate ECDSA signature from digest, private key and intermediate data point Q(=k*G) and random k .
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Set (#st_ec_prm_make_sign_t*) p_buf to p_prm.
 *  -# Check the arguments with #r_pkcd_ec_chk_make_sign_arg.
 *  -# If #PKCD_RET_OK == ret
 *    -# Get curve parameters from p_ctx->p_curve_info with #r_pkcd_ec_parse_curve
 *    -# Clear an asynchronous operation flag with #r_pkcd_cmn_clr_async_ope.
 *    -# Clear #PKCD_CMN_PKCCRAM with #r_pkcd_cmn_ram_clr.
 *    -# Clear the buffer p_buf with #r_pkcd_cmn_ram_clr.
 *    -# Copy an input digest, point and scalar to p_prm with #r_pkcd_ec_cpy_digest, #r_pkcd_ec_cpy_point,
 *       #r_pkcd_ec_cpy_scalar and #r_pkcd_cmn_cpy_word.
 *    -# Set (const #st_pkcd_ec_key_bin_t*) p_priv_key to p_key_bin.
 *    -# p_prm->priv_key.len = p_key_bin->key_length.
 *    -# Prepare for the operation with #r_pkcd_ec_prepare_operation.
 *    -# Make sign with #r_pkcd_ec_make_sign and add its return value to ret.
 *    -# Extract the return value from ret with #r_pkcd_cmn_extract_result and set ret to it.
 *    -# Use #r_pkcd_cmn_ram_clr to clear the random number value used in signature generation stored in p_prm.
 *    -# Use #r_pkcd_cmn_ram_clr to clear the private key value stored in p_prm.
 *    -# Extract the return value from ret with #r_pkcd_cmn_extract_result and set ret to it.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcdsaMakeSign (const st_pkc_ec_ctx_t * const p_ctx,
                                        uint8_t * const p_buf,
                                        const st_pkc_data_t * const p_digest,
                                        const st_pkc_data_t * const p_point_q,
                                        const st_pkc_data_t * const p_random,
                                        const void * const p_priv_key,
                                        st_pkc_data_t * const p_sign);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcdsaCalcU1U2
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIs
 * \brief       Calculate intermediate data u1, u2.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] p_digest       A message digest.
 * \param       [in] p_sign         A signature data.
 *                                  Data that concatenates the signature r and s.
 * \param       [in,out] p_u1       Output intermediate data u1. <br>
 *                                  Set p_data buffer size in len when inputting. <br>
 *                                  At the time of output, set the data in p_data, set the data size in len and return.
 * \param       [in,out] p_u2       Output intermediate data u2. <br>
 *                                  Set p_data buffer size in len when inputting. <br>
 *                                  At the time of output, set the data in p_data, set the data size in len and return.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPA, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_Initialize has been called.
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Derive u1, u2 value from given digest and signature.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Set (#st_ec_prm_calc_u1u2_t*) p_buf to p_prm.
 *  -# Check arguments with #r_pkcd_ec_chk_calc_u1u2_arg.
 *  -# If (#PKCD_RET_OK == ret)
 *    -# Get curve parameters from p_ctx->p_curve_info with #r_pkcd_ec_parse_curve.
 *    -# Clear an asynchronous operation flag with #r_pkcd_cmn_clr_async_ope.
 *    -# Clear #PKCD_CMN_PKCCRAM with #r_pkcd_cmn_ram_clr.
 *    -# Clear the buffer p_buf with #r_pkcd_cmn_ram_clr.
 *    -# Copy an input digest and signature to p_prm with #r_pkcd_ec_cpy_digest and #r_pkcd_ec_cpy_sign.
 *    -# Prepare for the operation with #r_pkcd_ec_prepare_operation.
 *    -# Calculate wR with #r_pkcd_ec_calc_wR.
 *    -# Clear #PKCD_EC_WORK1 with #r_pkcd_cmn_ram_clr.
 *    -# Calculate u1 with #r_pkcd_ec_set_digest_to_work and #r_pkcd_ec_calc_u, and add their return value
 *       to ret.
 *    -# Clear #PKCD_EC_WORK1 #r_pkcd_cmn_ram_clr.
 *    -# Calculate u2 with #r_pkcd_cmn_cpy_by_64bounds and #r_pkcd_ec_calc_u, and add the return value of
 *       #r_pkcd_ec_calc_u to ret.
 *    -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcdsaCalcU1U2 (const st_pkc_ec_ctx_t * const p_ctx,
                                        uint8_t * const p_buf,
                                        const st_pkc_data_t * const p_digest,
                                        const st_pkc_data_t * const p_sign,
                                        st_pkc_data_t * const p_u1,
                                        st_pkc_data_t * const p_u2);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcdsaCompareSign
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIs
 * \brief       Verify signature from intermediate data u1G, u2Q and signature value
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] p_u1g          A point derived from u1 x G(x, y). *where G is base point.
 *                                  When u1 x G = (0, 0), u1 x G is treated as an infinity point.
 * \param       [in] p_u2q          A point derived from u2 x Q(x, y). *where Q is public key.
 *                                  Data that concatenates the x and y coordinates of the point k*G.
 * \param       [in] p_sign         An ECDSA signature. <br>
 *                                  Data that concatenates the signature r and s.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 *  - #PKCD_RET_EC_VERIFAIL:              ECDSA verification error
 *  - #PKCD_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPA, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_Initialize has been called.
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Verify ECDSA signature using {u1 x G(x, y)} and {u2 x Q(x, y)} values.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 * -# Set (#st_ec_prm_compare_sign_t*) p_buf to p_prm.
 * -# Check arguments with #r_pkcd_ec_chk_compare_sign_arg.
 * -# If (#PKCD_RET_OK == ret)
 *   -# Get curve parameters from p_ctx->p_curve_info with #r_pkcd_ec_parse_curve.
 *   -# Clear an asynchronous operation flag with #r_pkcd_cmn_clr_async_ope.
 *   -# Clear #PKCD_CMN_PKCCRAM with #r_pkcd_cmn_ram_clr.
 *   -# Clear the buffer p_buf with #r_pkcd_cmn_ram_clr.
 *   -# Copy an input u1G, u2Q and sigature to p_prm with #r_pkcd_ec_cpy_point and #r_pkcd_ec_cpy_sign.
 *   -# Check if u1G = (0, 0) with #r_pkcd_cmn_cmp_zero_word, and set cmp_ret to the return value.
 *   -# If (0 != cmp_ret)
 *     -# Prepare for the operation with #r_pkcd_ec_prepare_operation and set ret to its return value.
 *     -# Convert u1G (affine coordinates) to projective coordinate with #r_pkcd_ec_affine_to_projective,
 *        and add its return value to ret.
 *     -# Convert u2Q (affine coordinates) to projective coordinate with #r_pkcd_ec_affine_to_projective.
 *        and add its return value to ret.
 *     -# <B>--- If #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 *     -# If (#PKCD_EC_CURVE_NIST_P521_LEN != curve_len), then
 *       -# Set DL, BDL to curve_len + 64 bits with the #r_pkcd_cmn_set_dl.
 *     -# <B>--- Endif #PKCD_CFG_OVF_WORKAROUND_ENABLE is 1 ---</B>
 *     -# Calculate (XR, YR, ZR) = u1*G + u2*Q (projective coordinates) with #r_pkcd_ec_point_add.
 *     -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 *     -# If (#PKCD_RET_OK == ret)
 *       -# Get Z-coordinate of u1G + u2Q from a PKCC work area with #r_pkcd_cmn_cpy_by_64bounds.
 *       -# Check if Z-coordinate of u1G + u2Q is zero with #r_pkcd_cmn_cmp_zero_word,
 *          and set cmp_ret to the return value.
 *       -# If (0 != cmp_ret)
 *         -# Clear the buffer p_prm->bux_x with #r_pkcd_cmn_ram_clr, and set cmp_ret to the return value.
 *         -# Convert u1G + u2Q (projective coordinates) to affine coordinates with #r_pkcd_ec_projective_to_affine,
 *            set the converted value to p_prm->bux_x, and add the return value of #r_pkcd_ec_projective_to_affine
 *            to ret.
 *       -# Else
 *         -# Set ret to #PKCD_RET_EC_VERIFAIL.
 *     -# Else
 *       -# Do nothing.
 *   -# Else
 *     -# Copy p_prm->u2q.x to p_prm->buf_x.
 *     -# Set ret to #PKCD_RET_OK.
 *   -# If (#PKCD_RET_OK == ret)
 *     -# Set ret to 0U.
 *     -# Compare p_prm->bux_x and p_prm->sign.r with #r_pkcd_cmn_cmp_word, and set cmn_ret to the return value.
 *     -# If (0 != cmn_ret)
 *       -# Set ret to #PKCD_RET_EC_VERIFAIL.
 *     -# Else
 *       -# Set ret to #PKCD_CMN_RET_OK_HALF_1.
 *       -# Compare p_prm->bux_x and p_prm->sign.r with #r_pkcd_cmn_cmp_word, and set cmn_ret to the return value.
 *       -# If (0 != cmn_ret)
 *         -# Set ret to #PKCD_RET_EC_VERIFAIL.
 *       -# Else
 *         -# Add #PKCD_CMN_RET_OK_HALF_2 to ret.
 *   -# Else
 *     -# Do nothing.
 *   -# If ((0U == ret ) || (#PKCD_CMN_RET_OK_HALF_1 == ret) || (#PKCD_CMN_RET_OK_HALF_2 == ret))
 *     -# Set ret to #PKCD_RET_TAMPERERR.
 *   -# Else
 *     -# Do nothing.
 * -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcdsaCompareSign (const st_pkc_ec_ctx_t * const p_ctx,
                                            uint8_t * const p_buf,
                                            const st_pkc_data_t * const p_u1g,
                                            const st_pkc_data_t * const p_u2q,
                                            const st_pkc_data_t * const p_sign);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcPrivateScalarGen
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIs
 * \brief       Generate secret scalar
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] p_random          Random value that is the basis of Private Scalar.
 * \param       [in,out] p_priv_scalar Output Private Scalar.
 *                                     Set p_data buffer size in len when inputting. <br>
 *                                     At the time of output, set the data in p_data, set the data size <br>
 *                                     in len and return.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPA, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_Initialize has been called.
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Generate a scalar scalar from the input random value
 *    - This API generates scalar according to B.4.1 Key pair Generation Using Extra Random Bits in FIPS186-4
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 * -# Assign a buffer for the private scalar generation as p_prm from p_buf.
 * -# Check the arguments with #r_pkcd_ec_chk_private_scalar_gen_arg, and set ret to the return value.
 * -# If (#PKCD_RET_OK == ret)
 *   -# Get curve parameters with #r_pkcd_ec_parse_curve.
 *   -# Clear an asynchronous operation flag with #r_pkcd_cmn_clr_async_ope.
 *   -# Clear the buffer pointed to by p_prm with #r_pkcd_cmn_ram_clr.
 *   -# Clear #PKCD_CMN_PKCCRAM with #r_pkcd_cmn_ram_clr.
 *   -# Prepare the operation with #r_pkcd_ec_prepare_operation and set ret to the return value.
 *   -# If #PKCD_RET_OK == ret
 *     -# Copy the base point order in curve.p_order to p_prm->n_minus1.
 *     -# Calculate p_prm->n_minus1[curve.len - 1U] = (uint8_t)((uint32_t)p_prm->n_minus1[curve.len - 1U] &
 *        (uint32_t)#PKCD_EC_MASK_FE).
 *     -# Calculate (p_random mod p_prm->n_minus1) with #r_pkcd_cmn_reduce_even_modulus,
 *        set the results to #PKCD_EC_WORK4, and set its return value to ret.
 *     -# Clear #PKCD_EC_WORK1 with #r_pkcd_cmn_clr_work_unit, and add its return value on ret.
 *     -# Convert the endianness of 0x00000001U into big endian, and set tmp32 to the result.
 *     -# Copy tmp32 to #PKCD_EC_WORK1.
 *     -# Calculate the private scalar = #PKCD_EC_WORK4 + #PKCD_EC_WORK1 with #r_pkcd_cmn_APlusB.
 *     -# Set curve length to p_priv_scalar->len.
 *     -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 *   -# Else
 *     -# Do nothing.
 * -# Retunr ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcPrivateScalarGen (const st_pkc_ec_ctx_t * const p_ctx,
                                            uint8_t * const p_buf,
                                            const st_pkc_data_t * const p_random,
                                            st_pkc_data_t * const p_priv_scalar);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcKeyGenInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIs
 * \brief       Generate secret scalar
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_DRBGAPIERR:               External DRBG API failed
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - None
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_Initialize has been called.
 *      - PKCC must not be running.
 * \par Detailed description
 *    - Generate an N+64bits random number for the ECC key pair generation.
 * \par Remarks
 *  - This API uses an external DRBG API passed by the caller for the random number generation. \n
 *    HW resources and data used by the API must be secured by the API design.
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 * -# If ((NULL != p_ctx) && (NULL != p_buf)) then,
 *   -# If ((NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info)) then,
 *     -# Check the given curve structure with #r_pkcd_ec_chk_curve_info and set ret to the result.
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR.
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR.
 * -# If #PKCD_RET_OK == ret then,
 *   -# Set curve_len to p_ctx->p_curve_info->curve_len.
 *   -# Assign a buffer for the EC keygen operation as p_ec_keygen from p_ctx->p_buf.
 *   -# Set p_ec_keygen->priv_scalar_len to curve_len.
 *   -# Set p_ec_keygen->random_len to (curve_len + #PKCD_EC_EXTRA_BYTES).
 *   -# Genarete an N+64bits random number with p_ctx->p_drbg_api and set drbg_ret to the result.
 *   -# If (#PKCD_CMN_EXTERNAL_API_OK == drbg_ret) then,
 *     -# Do nothing.
 *   -# Else
 *     -# Set ret to #PKCD_RET_DRBGAPIERR.
 * -# Else
 *   -# Do nothing.
 * -# Retunr ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcKeyGenInit (const st_pkc_ec_ctx_t * const p_ctx,
                                        uint8_t* const p_buf);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcKeyGenOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIs
 * \brief       Generate secret scalar
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] sync_opt       A variable to select synchronous / asynchronous execution
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPA, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_EcKeyGenInit has been called.
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Generate a scalar from the N+64bits random number made by #R_PKCD_EcKeyGenInit API.
 *      And calculate public key from it.
 *    - This API generates a scalar according to B.4.1 Key pair Generation Using Extra Random Bits in FIPS186-4.
 *    - If the synchronized operation is selected in sync_opt, this API waits end of the PKCC calculation,
 *      and then returns. Otherwise this API returns without waiting the calculation end.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 * -# If ((NULL != p_ctx) && (NULL != p_buf)) then,
 *   -# If ((NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info)) then,
 *     -# Check the given curve structure with #r_pkcd_ec_chk_curve_info and set ret to the result.
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR.
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR.
 * -# If #PKCD_RET_OK == ret then,
 *   -# Assign a buffer for the EC keygen operation as p_ec_keygen from p_buf.
 *   -# Assign a buffer for the EC private scalar generation operation as p_priv_scalar_ten from p_buf.
 *   -# Set the random data addres random_data.p_data to p_ec_keygen->random_data.
 *   -# Set the random data length random_data.len to p_ec_keygen->random_len.
 *   -# Set the scalar outputting address priv_scalar_data.p_data to p_ec_keygen->priv_scalar_data.
 *   -# Set the scalar outputting length priv_scalar_data.len to sizeof(p_ec_keygen->priv_scalar_data).
 *   -# Genarete a scalar from the given N+64bits random number with #R_PKCD_EcPrivateScalarGen.
 *      And set ret to the result.
 *   -# If #PKCD_RET_OK == ret then,
 *     -# Set p_ec_keygen->priv_scalar_len to the resulting length priv_scalar_data.len.
 *     -# Set the type of the outputting scalar priv_scalar_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *     -# Set the address of the outputting scalar priv_scalar_sel.p_sel_data to &priv_scalar_data.
 *     -# Copy the base point coordinates from the given curve info to the temporal buffer base_point_val
 *        with #r_pkcd_cmn_cpy_byte.
 *     -# Set the base point address base_point_data.p_data to base_point_val.
 *     -# Set the base point total length base_point_data.len to p_ctx->p_curve_info->curve_len * #PKCD_CMN_MUL_2.
 *     -# Set the type of the base point structure base_point_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *     -# Set the address of the base point structure base_point_sel.p_sel_data to &base_point_data.
 *     -# Set the buffer for the scalar multiplication p_buf_smult to &p_buf[sizeof(st_ec_prm_keygen_t)].
 *     -# Initialize the scalar multiplication operation with #R_PKCD_EcScalarMultInit and set ret to the result.
 *     -# If #PKCD_RET_OK == ret then,
 *       -# Perform the scalar multiplication with #R_PKCD_EcScalarMultOperate and set ret to the result.
 *     -# Else
 *       -# Do nothing.
 *   -# Else
 *     -# Do nothing.
 * -# Else
 *   -# Do nothing.
 * -# Retunr ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcKeyGenOperate (const st_pkc_ec_ctx_t * const p_ctx,
                                        uint8_t * const p_buf,
                                        const e_pkc_sync_opt_t sync_opt);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcKeyGenFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECAPIs
 * \brief       Output EC pair.
 * \param       [in] p_ctx              A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in,out] p_buf          Buffer used for internal processing of PKC-Driver.
 * \param       [in,out] p_priv_key     Output EC private key of key binary format.
 * \param       [in,out] p_pub_key      Output EC public key of key binary format.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPT, #PKCD_CMN_ICUM_PKCCB0DPA, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPB, #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_EcKeyGenOperate has been called.
 *      - PKCC must not be running.
 * \par Detailed description
 *    - Convert the EC key pair made in #R_PKCD_EcKeyGenOperate into the key binary format of this driver.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 * -# If ((NULL != p_ctx) && (NULL != p_buf)) then,
 *   -# If (NULL != p_ctx->p_curve_info) then,
 *     -# Check the given curve structure with #r_pkcd_ec_chk_curve_info and set ret to the result.
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR.
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR.
 * -# If #PKCD_RET_OK == ret then,
 *   -# Set the outputting length of the public key point_data.len to sizeof(point_val).
 *   -# Set the outputting address of the public key point_data.p_data to point_val.
 *   -# Set the buffer for the scalar multiplication p_buf_smult to &p_buf[sizeof(st_ec_prm_keygen_t)].
 *   -# Finalize the scalar multiplication with #R_PKCD_EcScalarMultFinal an set ret to the result.
 *   -# Set the buffer for the EC key generation p_ec_keygen to (st_ec_prm_keygen_t*)p_buf.
 *   -# Set the scalar length priv_scalar_data.len to p_ec_keygen->priv_scalar_len.
 *   -# Set the scalar address priv_scalar_data.p_data to p_ec_keygen->priv_scalar_data.
 *   -# Wrap the EC key pair with #r_pkcd_ec_key_wrap and set ret to the result.
 * -# Else
 *   -# Do nothing.
 * -# Retunr ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcKeyGenFinal (const st_pkc_ec_ctx_t * const p_ctx,
                                        uint8_t * const p_buf,
                                        void * const p_priv_key,
                                        void * const p_pub_key);

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaScalarMultBaseInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIs
 * \brief       Prepare for scalar multiplication with the base point.
 * \param       [in] p_scalar_or_privkey          Set one of the following <br>
 *                                                  (1) Byte array of scalar value <br>
 *                                                  (2) EdDSA private key of key binary format
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - None
 * \par Precondition
 *   - Satisfy both of the following conditions:
 *     - #R_PKCD_Initialize has been called.
 *     - The PKCC must not be running.
 * \par Detailed description
 *    - Prepare for the scalar multiplication operation of the following :
 *      - Clear #PKCD_CMN_PKCCRAM
 *      - Save input parameters in buffer
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Set (#st_eddsa_prm_smultbase_t*) p_buf to p_prm.
 *  -# Check the arguments with #r_pkcd_eddsa_chk_scalar_mult_base_init_arg, and set ret to the return value.
 *  -# If (#PKCD_RET_OK == ret)
 *    -# Clear an asynchronous operation flag with #r_pkcd_cmn_clr_async_ope.
 *    -# Clear #PKCD_CMN_PKCCRAM with #r_pkcd_cmn_ram_clr.
 *    -# Clear the buffer p_buf with #r_pkcd_cmn_ram_clr.
 *    -# Initialize p_prm->executed_bits with 0U.
 *    -# If (#PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
 *      -# Set (#st_pkcd_eddsa_key_bin_t*) p_scalar_or_privkey->p_sel_data->p_data to p_key_bin.
 *      -# Copy p_key_bin->p_private to p_prm->scalar.scalar with #r_pkcd_cmn_cpy_word.
 *      -# Set p_key_bin->key_length to p_prm->scalar.len.
 *    -# Else
 *      -# Copy p_scalar_or_privkey->p_sel_data->p_data to p_prm->scalar.scalar with #r_pkcd_cmn_cpy_word.
 *      -# Set p_scalar_or_privkey->p_sel_data->len to p_prm->scalar.len.
 *    -# Encode p_prm->scalar and set the result to p_prm->scalar with #r_pkcd_eddsa_encode_scalar.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EddsaScalarMultBaseInit (const st_pkc_select_data_t * const p_scalar_or_privkey,
                                                    const st_pkc_eddsa_ctx_t * const p_ctx,
                                                    uint8_t * const p_buf);

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaScalarMultBaseOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIs
 * \brief       Perform scalar multiplication for points on edwards curve.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in] processing_bits  If an operation is hoped to be done in separated calls, <br>
 *                                    set the number of processing bits per each call.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_PROCESSING:               Successful completion. <br>
 *                                        Some bits are remain unprocessed, so subsequent calls of this API are needed.
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EDDSA_PBYTES, #PKCD_EDDSA_PDT_L, #PKCD_EDDSA_LWORK, #PKCD_EDDSA_R2MODL, #PKCD_EDDSA_WORK1, <br>
 *    #PKCD_EDDSA_WORK2, #PKCD_EDDSA_WORK3, #PKCD_EDDSA_WORK4 #PKCD_EDDSA_WORK5, #PKCD_EDDSA_WORK6, <br>
 *    #PKCD_EDDSA_PNTWORK2, #PKCD_EDDSA_PNTWORK3, #PKCD_EDDSA_PNTWORK4
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_EddsaScalarMultBaseInit has been called.
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Perform scalar multiplication for base points on edwards curve.
 *    - Scalar multiple operation is realized using the binary method <br>
 *      This operation can be executed by dividing it into multiple calls by specifying the number of bits  <br>
 *      to be processed in the argument processing_bits. <br>
 *      Returns #PKCD_RET_PROCESSING if unprocessed bits remain.
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_EddsaScalarMultBaseInit for the argument p_ctx and  <br>
 *    the argument p_buf. <br>
 *    If you set a value different from #R_PKCD_EddsaScalarMultBaseInit, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Set (#st_eddsa_prm_smultbase_t*) p_buf to p_prm.
 *  -# If ((NULL != p_ctx) and (NULL != p_buf))
 *    -# If ((#PKCD_CMN_PROCESSING_BITS_MIN <= processing_bits) and
            ((processing_bits <= #PKCD_CMN_PROCESSING_BITS_MAX)))
 *      -# Check curve information with #r_pkcd_eddsa_chk_curve_info and its return value to ret.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# Get curve parameters with #r_pkcd_eddsa_parse_curve.
 *    -# Else
 *      Set ret to #PKCD_RET_LENGTHERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# If (#PKCD_RET_OK == ret)
 *    -# Assign #PKCD_EDDSA_WORK1, #PKCD_EDDSA_WORK2, #PKCD_EDDSA_WORK3, #PKCD_EDDSA_WORK4, #PKCD_EDDSA_WORK5
 *       and #PKCD_EDDSA_WORK6 as work areas for EDDSA scalar multiplication operation.
 *    -# If (0U == p_prm->executed_bits)
 *      -# Set curve length to #PKCD_EDDSA_PBYTES and set (#PKCD_EDDSA_WORKUNIT - curve length) to
 *         #PKCD_EDDSA_OFFSET_VAL with #r_pkcd_eddsa_set_len_info.
 *      -# Prepare resisters and values to be used with #r_pkcd_eddsa_precalc_params, and add the return value to ret.
 *      -# Prepare ICUM_PKCCB0DPT to calculate scalar mod L.
 *      -# Perform modulus calculation with #r_pkcd_cmn_reduce_small_modulus, and set ret to the return value.
 *      -# Clear p_prm->scalar.scalar with #r_pkcd_cmn_ram_clr.
 *      -# Get the output of #r_pkcd_cmn_reduce_small_modulus from a PKCC work area,
 *         and set it to p_prm->scalar.scalar with #r_pkcd_cmn_cpy_by_64bounds.
 *      -# Set p_prm->scalar.len to #PKCD_EDDSA_PBYTES.
 *    -# If #PKCD_EDDSA_CURVE_EDWARDS25519_LEN == #PKCD_EDDSA_PBYTES
 *      -# Perform scalar multiplication with #r_pkcd_ed25519_smult_base, and set ret to the return value.
 *    -# Else
 *      -# Perform scalar multiplication with #r_pkcd_ed448_smult_base, and set ret to the return value.
 *    -# Extract the return value from ret with #r_pkcd_cmn_extract_result, and set ret to it.
 *    -# If #PKCD_RET_PROCESSING != ret
 *      -# Use #r_pkcd_cmn_ram_clr to clear the potentially private key value stored in p_prm.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EddsaScalarMultBaseOperate (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                    uint8_t * const p_buf,
                                                    const uint32_t processing_bits);

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaScalarMultBaseFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIs
 * \brief       Finalize an EdDSA scalar multiplication.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in,out] p_comp_point Compressed coordinate data <br>
 *                                  Set p_data buffer size in len when inputting <br>
 *                                  At the time of output, set the data in p_data, set the data size in len and return
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *   * #PKCD_EDDSA_WORK1, #PKCD_EDDSA_WORK2, #PKCD_EDDSA_WORK3, #PKCD_EDDSA_PNTWORK2
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_EddsaScalarMultBaseOperate is processing all bits
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Compress and output coordinates calculated by #R_PKCD_EddsaScalarMultBaseOperate
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_EddsaScalarMultBaseOperate for the argument p_ctx and <br>
 *    the argument p_buf.<br>
 *    If you set a value different from #R_PKCD_EddsaScalarMultBaseOperate, the result is not guaranteed.
 *
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_buf) and (NULL != p_comp_point))
 *    -# If (NULL != p_comp_point->p_data)
 *      -# Check curve informatoin with #r_pkcd_eddsa_chk_curve_info and set ret to the return value.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# If (#PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len) then
 *             Set comp_point_len to #PKCD_EDDSA_ED25519_KEY_LEN,
 *             otherwise Set comp_point_len to #PKCD_EDDSA_ED448_RU4_KEY_LEN.
 *        -# If (p_comp_point->len >= comp_point_len)
 *          -# Compress the scalar mutilplication result following RFC8032 with #r_pkcd_eddsa_point_compress
 *             and set the compressed value to p_comp_point->p_data, and add the return value to ret.
 *          -# Set p_comp_point->len to comp_point_len.
 *        -# Else
 *          -# Set ret to #PKCD_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EddsaScalarMultBaseFinal (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                    uint8_t * const p_buf,
                                                    const e_pkc_bool_t comp_flag,
                                                    st_pkc_data_t * const p_comp_point);

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaScalarMultInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIs
 * \brief       Prepare for scalar multiplication with the base point.
 * \param       [in] p_scalar_or_privkey          Set one of the following <br>
 *                                                  (1) Byte array of scalar value <br>
 *                                                  (2) EdDSA private key of key binary format
 * \param       [in] p_comp_point_or_pubkey       Set one of the following <br>
 *                                                  (1) Byte array of point value <br>
 *                                                  (2) EdDSA public key of key binary format <br>
 *                                                      EdDSA private key including public key of key binary format
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 * \par Registers Used
 *  - None
 * \par Precondition
 *   - Satisfy both of the following conditions:
 *     - #R_PKCD_Initialize has been called.
 *     - The PKCC must not be running.
 * \par Detailed description
 *    - Prepare for the scalar multiplication operation of the following :
 *      - Clear #PKCD_CMN_PKCCRAM
 *      - Save input parameters in buffer
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Set (#st_eddsa_prm_smult_t*)p_buf to p_prm.
 *  -# Check the arguments with #r_pkcd_eddsa_chk_scalar_mult_init_arg.
 *  -# If (#PKCD_RET_OK == ret)
 *    -# Clear an asynchronous operation flag with #r_pkcd_cmn_clr_async_ope.
 *    -# Clear #PKCD_CMN_PKCCRAM with #r_pkcd_cmn_ram_clr.
 *    -# Clear the buffer p_buf with #r_pkcd_cmn_ram_clr.
 *    -# Initialize p_prm->executed_bits with 0U.
 *    -# If (#PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
 *      -# Set (#st_pkcd_eddsa_key_bin_t*) p_scalar_or_privkey->p_sel_data->p_data to p_key_bin.
 *      -# Copy p_key_bin->p_private to p_prm->scalar.scalar with #r_pkcd_cmn_cpy_word.
 *      -# Set p_key_bin->key_length to p_prm->scalar.len.
 *    -# Else
 *      -# Copy p_scalar_or_privkey->p_sel_data->p_data to p_prm->scalar.scalar with #r_pkcd_cmn_cpy_word.
 *      -# Set p_scalar_or_privkey->p_sel_data->len to p_prm->scalar.len.
 *    -# Encode p_prm->scalar and set the result to p_prm->scalar with #r_pkcd_eddsa_encode_scalar.
 *    -# If (#PKC_DATA_TYPE_KEY_BINARY == p_scalar_or_privkey->data_type)
 *      -# Set (#st_pkcd_eddsa_key_bin_t*)p_comp_point_or_pubkey->p_sel_data->p_data to p_key_bin.
 *      -# Copy p_key_bin->p_public to p_prm->comp_point.scalar with #r_pkcd_cmn_cpy_word.
 *      -# Set p_key_bin->key_length to p_prm->comp_point.len.
 *    -# Else
 *      -# Copy p_comp_point_or_pubkey->p_sel_data->p_data to p_prm->comp_point.scalar with #r_pkcd_cmn_cpy_word.
 *      -# Set p_comp_point_or_pubkey->p_sel_data->len to p_prm->comp_point.len.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EddsaScalarMultInit (const st_pkc_select_data_t * const p_scalar_or_privkey,
                                                    const st_pkc_select_data_t * const p_comp_point_or_pubkey,
                                                    const st_pkc_eddsa_ctx_t * const p_ctx,
                                                    uint8_t * const p_buf);

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaScalarMultOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIs
 * \brief       Perform scalar multiplication for points on edwards curve.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in] processing_bits  If an operation is hoped to be done in separated calls, <br>
 *                                    set the number of processing bits per each call.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_PROCESSING:               Successful completion. <br>
 *                                        Some bits are remain unprocessed, so subsequent calls of this API are needed.
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 *  - #PKCD_RET_EDDSA_DECOMPERR:          Failed to decompress compressed coordinates
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_EDDSA_PBYTES, #PKCD_EDDSA_PDT_L, #PKCD_EDDSA_LWORK, #PKCD_EDDSA_R2MODL, #PKCD_EDDSA_WORK1, <br>
 *    #PKCD_EDDSA_WORK2, #PKCD_EDDSA_WORK3, #PKCD_EDDSA_WORK4, #PKCD_EDDSA_WORK5, #PKCD_EDDSA_WORK6, <br>
 *    #PKCD_EDDSA_PNTWORK1, #PKCD_EDDSA_PNTWORK2, #PKCD_EDDSA_PNTWORK3, #PKCD_EDDSA_PNTWORK4
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_EddsaScalarMultInit has been called.
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Perform scalar multiplication for points on edwards curve.
 *    - Scalar multiple operation is realized using the binary method <br>
 *      This operation can be executed by dividing it into multiple calls by specifying the number of bits  <br>
 *      to be processed in the argument processing_bits. <br>
 *      Returns #PKCD_RET_PROCESSING if unprocessed bits remain.
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_EddsaScalarMultInit for the argument p_ctx and  <br>
 *    the argument p_buf. <br>
 *    If you set a value different from #R_PKCD_EddsaScalarMultInit, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Set (#st_eddsa_prm_smultbase_t*) p_buf to p_prm.
 *  -# If ((NULL != p_ctx) and (NULL != p_buf))
 *    -# If ((#PKCD_CMN_PROCESSING_BITS_MIN <= processing_bits) and
            ((processing_bits <= #PKCD_CMN_PROCESSING_BITS_MAX)))
 *      -# Check curve information with #r_pkcd_eddsa_chk_curve_info and its return value to ret.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# Get curve parameters with #r_pkcd_eddsa_parse_curve.
 *    -# Else
 *      Set ret to #PKCD_RET_LENGTHERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *  -# If (#PKCD_RET_OK == ret)
 *    -# Clear an asynchronous operation flag with #r_pkcd_cmn_clr_async_ope.
 *    -# Assign #PKCD_EDDSA_WORK1, #PKCD_EDDSA_WORK2, #PKCD_EDDSA_WORK3, #PKCD_EDDSA_WORK4, #PKCD_EDDSA_WORK5
 *       and #PKCD_EDDSA_WORK6 as work areas for EDDSA scalar multiplication operation.
 *    -# If (0U == p_prm->executed_bits)
 *      -# Set curve length to #PKCD_EDDSA_PBYTES and set (#PKCD_EDDSA_WORKUNIT - curve length) to
 *         #PKCD_EDDSA_OFFSET_VAL with #r_pkcd_eddsa_set_len_info.
 *      -# Prepare resisters and values to be used with #r_pkcd_eddsa_precalc_params, and add the return value to ret.
 *      -# Prepare ICUM_PKCCB0DPT to calculate scalar mod L.
 *      -# Perform modulus calculation with #r_pkcd_cmn_reduce_small_modulus, and set ret to the return value.
 *      -# Clear p_prm->scalar.scalar with #r_pkcd_cmn_ram_clr.
 *      -# Get the output of #r_pkcd_cmn_reduce_small_modulus from a PKCC work area,
 *         and set it to p_prm->scalar.scalar with #r_pkcd_cmn_cpy_by_64bounds.
 *      -# Set p_prm->scalar.len to #PKCD_EDDSA_PBYTES.
 *      -# If #PKCD_EDDSA_CURVE_EDWARDS25519_LEN == #PKCD_EDDSA_PBYTES
 *        -# Decompress p_prm->comp_point.scalar with #r_pkcd_ed25519_point_decompress and set the result to
 *           the work area #PKCD_EDDSA_PNTWORK1, and add the return value to ret.
 *      -# Else
 *        -# Decompress p_prm->comp_point.scalar with #r_pkcd_ed448_point_decompress and set the result to
 *           the work area #PKCD_EDDSA_PNTWORK1, and add the return value to ret.
 *    -# Perform scalar multiplication with #r_pkcd_eddsa_point_smult, set the output to
 *       the work area #PKCD_EDDSA_PNTWORK2, and add the return value to ret.
 *    -# Extract the return value from ret and set ret to it.
 *    -# If PKCD_RET_PROCESSING != ret
 *      -# Use #r_pkcd_cmn_ram_clr to clear the potentially private key value stored in p_prm.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EddsaScalarMultOperate (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                uint8_t * const p_buf,
                                                const uint32_t processing_bits);

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaScalarMultFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIs
 * \brief       Finalize an EdDSA scalar multiplication.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function <br>
 *                                  Do not change the value of a member variable until the Final function completes
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in,out] p_comp_point Compressed coordinate data <br>
 *                                  Set p_data buffer size in len when inputting <br>
 *                                  At the time of output, set the data in p_data, set the data size in len and return
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *   * #PKCD_EDDSA_WORK1, #PKCD_EDDSA_WORK2, #PKCD_EDDSA_WORK3, #PKCD_EDDSA_PNTWORK2
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *    - Satisfy both of the following conditions:
 *      - #R_PKCD_EddsaScalarMultOperate is processing all bits
 *      - The PKCC must not be running.
 * \par Detailed description
 *    - Compress and output coordinates calculated by #R_PKCD_EddsaScalarMultOperate
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_EddsaScalarMultOperate for the argument p_ctx and <br>
 *    the argument p_buf.<br>
 *    If you set a value different from #R_PKCD_EddsaScalarMultOperate, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# If ((NULL != p_ctx) and (NULL != p_buf) and (NULL != p_comp_point))
 *    -# If (NULL != p_comp_point->p_data)
 *      -# Check curve informatoin with #r_pkcd_eddsa_chk_curve_info and set ret to the return value.
 *      -# If (#PKCD_RET_OK == ret)
 *        -# If (#PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len) then
 *             Set comp_point_len to #PKCD_EDDSA_ED25519_KEY_LEN,
 *             otherwise Set comp_point_len to #PKCD_EDDSA_ED448_RU4_KEY_LEN.
 *        -# If (p_comp_point->len >= comp_point_len)
 *          -# Clear an asynchronous operation flag with #r_pkcd_cmn_clr_async_ope.
 *          -# Compress the scalar mutilplication result following RFC8032 with #r_pkcd_eddsa_point_compress
 *             and set the compressed value to p_comp_point->p_data, and add the return value to ret.
 *          -# Set p_comp_point->len to comp_point_len.
 *        -# Else
 *          -# Set ret to #PKCD_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCD_RET_PARAMERR.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EddsaScalarMultFinal (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                uint8_t * const p_buf,
                                                const e_pkc_bool_t comp_flag,
                                                st_pkc_data_t * const p_comp_point);

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaDeriveS
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIs
 * \brief       Derive an EdDSA signature parameter S from intermediate values.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] p_scalar_s     An integer s derived expanding private key.
 * \param       [in] p_scalar_r     A scalar r derived hashing over integer s and message.
 * \param       [in] p_digest_r_a_m A digest derived hashing over signature.R, public key A and message.
 * \param       [in,out] p_sign_s   An EdDSA signature s. <br>
 *                                  Set p_data buffer size in len when inputting. <br>
 *                                  At the time of output, set the data in p_data, set the data size in len and return.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *   * #PKCD_EDDSA_WORK1, #PKCD_EDDSA_WORK2, #PKCD_EDDSA_WORK3, #PKCD_EDDSA_PNTWORK2
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - Satisfy both of the following conditions:
 *    - #R_PKCD_Initialize has been called.
 *    - The PKCC must not be running.
 * \par Detailed description
 *  - Derive an EdDSA signature parameter S from intermediate values.
 * \par Remarks
 *  - Supports only the twisted curve25519. <br>
 *
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Set (#st_eddsa_prm_derive_s_t*)p_buf to p_prm.
 *  -# Check the arguments with #r_pkcd_eddsa_chk_derive_s_arg and set ret to the return value.
 *  -# If (#PKCD_RET_OK == ret)
 *    -# Get curve paramters from p_ctx->p_curve_info with #r_pkcd_eddsa_parse_curve, and set ret to the return value.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# Clear an asynchronous operation flag with #r_pkcd_cmn_clr_async_ope.
 *      -# Clear #PKCD_CMN_PKCCRAM with #r_pkcd_cmn_ram_clr.
 *      -# Clear the buffer p_buf with #r_pkcd_cmn_ram_clr.
 *      -# Set curve length to #PKCD_EDDSA_PBYTES and set (#PKCD_EDDSA_WORKUNIT - curve length) to
 *         #PKCD_EDDSA_OFFSET_VAL with #r_pkcd_eddsa_set_len_info.
 *      -# Prepare resisters and values to be used with #r_pkcd_eddsa_precalc_params, and add the return value to ret.
 *      -# Copy p_scalar_s->p_data to p_prm->scalar_s.scalar with #r_pkcd_cmn_cpy_word.
 *      -# Set p_scalar_s->len to p_prm->scalar_s.len.
 *      -# Encode p_prm->scalar_s and set the result to p_prm->scalar_s with #r_pkcd_eddsa_encode_scalar.
 *      -# Copy p_scalar_r->p_data to p_prm->scalar_r.scalar with #r_pkcd_cmn_cpy_word.
 *      -# Set p_scalar_r->len to p_prm->scalar_r.len.
 *      -# Encode p_prm->scalar_r and set the result to p_prm->scalar_r with #r_pkcd_eddsa_encode_scalar.
 *      -# Copy p_digest_r_a_m->p_data to p_prm->hram.scalar with #r_pkcd_cmn_cpy_word.
 *      -# Set p_digest_r_a_m->len to p_prm->hram.len.
 *      -# Encode p_prm->hram and set the result to p_prm->hram with #r_pkcd_eddsa_encode_scalar.
 *      -# Prepare ICUM_PKCCB0DPT to calculate scalar mod L.
 *      -# Perform modulus calculation with #r_pkcd_cmn_reduce_small_modulus, set the result to the PKCC work area
 *         #PKCD_EDDSA_WORK4, and set ret to the return value.
 *      -# If #PKCD_EDDSA_CURVE_EDWARDS25519_LEN == #PKCD_EDDSA_PBYTES
 *        -# Copy p_prm->scalar_s.scalar to #PKCD_EDDSA_WORK7 with #r_pkcd_cmn_cpy_by_64bounds.
 *      -# Else
 *        -# Copy p_prm->scalar_s.scalar, minus the 4byte padding,
 *           to #PKCD_EDDSA_WORK7 with #r_pkcd_cmn_cpy_by_64bounds.
 *      -# Calculate Hash(R, A, M) * s mod L with #r_pkcd_cmn_ABRInvModC, set the result to #PKCD_EDDSA_WORK6,
 *         and add the return value to ret.
 *      -# Clear PKCC work areas #PKCD_EDDSA_WORK2 and #PKCD_EDDSA_WORK3
 *      -# Calculate S = r + Hash(R, A, M) * s mod L with #r_pkcd_cmn_reduce_small_modulus and #r_pkcd_cmn_APlusBModC,
 *         set the result S the PKCC work area #PKCD_EDDSA_WORK2, and add their return value to ret.
 *      -# If #PKCD_EDDSA_CURVE_EDWARDS25519_LEN == #PKCD_EDDSA_PBYTES
 *        -# Get S from a PKCC work area, set S to the #st_eddsa_scalar_t type buffer tmp_sign_s
 *        -# Set tmp_sign_s.len to #PKCD_EDDSA_ED25519_KEY_LEN.
 *      -# Else
 *        -# Get S from a PKCC work area, set S to the #st_eddsa_scalar_t type buffer tmp_sign_s with 4byte padding
 *        -# Set tmp_sign_s.len to #PKCD_EDDSA_ED448_RU4_KEY_LEN.
 *      -# Encode tmp_sign_s and set the result to tmp_sign_s with #r_pkcd_eddsa_encode_scalar.
 *      -# Get encoded signature s from tmp_sign_s.scalar, and set it to p_sign_s->p_data with #r_pkcd_cmn_cpy_word.
 *      -# Set p_sign_s->len to tmp_sign_s.len.
 *      -# Extract the return value from ret, and set ret to it with #r_pkcd_cmn_extract_result.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EddsaDeriveS (const st_pkc_eddsa_ctx_t * const p_ctx,
                                        uint8_t * const p_buf,
                                        const st_pkc_data_t * const p_scalar_s,
                                        const st_pkc_data_t * const p_scalar_r,
                                        const st_pkc_data_t * const p_digest_r_a_m,
                                        st_pkc_data_t * const p_sign_s);

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaCompareSign
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIs
 * \brief       Verify signature from intermediate data u1G, u2Q and signature value
 * \param       [in] p_ctx            A pointer to a structure that summarizes the parameters required by EC. <br>
 * \param       [in,out] p_buf        Buffer used for internal processing of PKC-Driver
 * \param       [in] p_comp_point_ha  Compressed coordinates derived from {H(R,A,M) * public key A}.
 * \param       [in] p_comp_point_sb  Compressed coordinates derived from {signature S * base point B}.
 * \param       [in] p_sign_r         A signature r.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 *  - #PKCD_RET_EDDSA_VERIFAIL:           The verification failed
 *  - #PKCD_RET_EDDSA_DECOMPERR:          Failed to decompress compressed coordinates
 *  - #PKCD_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *   - #PKCD_CMN_PKCCRAM, #PKCD_EDDSA_PBYTES, #PKCD_EDDSA_PWORK, #PKCD_EDDSA_R2MODP, #PKCD_EDDSA_WORK1, <br>
 *     #PKCD_EDDSA_WORK2, #PKCD_EDDSA_WORK3, #PKCD_EDDSA_WORK4 #PKCD_EDDSA_WORK5, #PKCD_EDDSA_WORK6, <br>
 *     #PKCD_EDDSA_PNTWORK1, #PKCD_EDDSA_PNTWORK2, #PKCD_EDDSA_PNTWORK3
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPT, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPW (Used by sub functions)
 * \par Precondition
 *  - Satisfy both of the following conditions:
 *    - #R_PKCD_Initialize has been called.
 *    - The PKCC must not be running.
 * \par Detailed description
 *  - Verify EdDSA signature using {h x public key A} and {signature S x B} values.
 * \par Remarks
 *  - Supports only the edwards25519.
 *
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Check the aruguments with #r_pkcd_eddsa_chk_compare_sign_arg.
 *  -# If (#PKCD_RET_OK == ret)
 *    -# Clear an asynchronous operation flag with #r_pkcd_cmn_clr_async_ope.
 *    -# Clear #PKCD_CMN_PKCCRAM with #r_pkcd_cmn_ram_clr.
 *    -# Assign #PKCD_EDDSA_WORK1, #PKCD_EDDSA_WORK2, #PKCD_EDDSA_WORK3, Assign #PKCD_EDDSA_WORK4 #PKCD_EDDSA_WORK5,
 *       and #PKCD_EDDSA_WORK6 as PKCC work areas for EdDSA signature comparison operation.
 *    -# Set PKCC work area addresses in DP format to dpwks.
 *    -# Clear #PKCD_CMN_PKCCRAM with #r_pkcd_cmn_ram_clr.
 *    -# Set curve length to #PKCD_EDDSA_PBYTES and set (#PKCD_EDDSA_WORKUNIT - curve length) to
 *       #PKCD_EDDSA_OFFSET_VAL with #r_pkcd_eddsa_set_len_info.
 *    -# Prepare resisters and values to be used with #r_pkcd_eddsa_precalc_params, and set ret to the return value.
 *    -# If #PKCD_EDDSA_PBYTES == #PKCD_EDDSA_CURVE_EDWARDS25519_LEN
 *      -# Using #r_pkcd_ed25519_point_decompress, decompress hA p_comp_point_ha->p_data according to RFC8032,
 *         set the result to #PKCD_EDDSA_PNTWORK1 and add the return value to ret.
 *      -# Using #r_pkcd_ed25519_point_decompress, decompress SB p_comp_point_sb->p_data according to RFC8032,
 *         set the result to #PKCD_EDDSA_PNTWORK2 and add the return value to ret.
 *    -# Else
 *      -# Using #r_pkcd_ed448_point_decompress, decompress hA p_comp_point_ha->p_data according to RFC8032,
 *         set the result to #PKCD_EDDSA_PNTWORK1 and add the return value to ret.
 *      -# Using #r_pkcd_ed448_point_decompress, decompress SB p_comp_point_sb->p_data according to RFC8032,
 *         set the result to #PKCD_EDDSA_PNTWORK2 and add the return value to ret.
 *    -# Calculate SB - hA and set the result to #PKCD_EDDSA_PNTWORK3 with #r_pkcd_cmn_ABRInvModC,
 *       #r_pkcd_cmn_AMinusBModC and #r_pkcd_eddsa_point_add_inline, and add their return value to ret.
 *    -# Clear the #st_eddsa_scalar_t buffer comp_sb_minus_ha with #r_pkcd_cmn_ram_clr.
 *    -# Compress SB-hA following RFC8032, set the result to comp_sb_minus_ha.scalar
 *       with #r_pkcd_eddsa_point_compress, and add the return value to ret.
 *    -# Extract the return value from ret, and set ret to it.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# Set ret to 0U.
 *      -# Compare p_sign_r and comp_sb_minus_ha with #r_pkcd_cmn_cmp_word, and set the result to cmp_ret.
 *      -# If (0 != cmp_ret)
 *        -# Set ret to #PKCD_RET_EDDSA_VERIFAIL.
 *      -# Else
 *        -# Set ret to #PKCD_CMN_RET_OK_HALF_1.
 *        -# Compare p_sign_r and comp_sb_minus_ha with #r_pkcd_cmn_cmp_word, and set the result to cmp_ret.
 *        -# If (0 != cmp_ret)
 *          -# Set ret to #PKCD_RET_EDDSA_VERIFAIL.
 *        -# Else
 *          -# Add ret to #PKCD_CMN_RET_OK_HALF_2. <br>
 *             Add #PKCD_CMN_RET_OK_HALF_1 and #PKCD_CMN_RET_OK_HALF_2 to get the #PKCD_RET_OK value.
 *  -# (0U == ret ) || (#PKCD_CMN_RET_OK_HALF_1 == ret) || (#PKCD_CMN_RET_OK_HALF_2 == ret)
 *    -# Set ret to #PKCD_RET_TAMPERERR.
 *  -# Else
 *    -# Do nothing
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EddsaCompareSign (const st_pkc_eddsa_ctx_t * const p_ctx,
                                            uint8_t * const p_buf,
                                            const st_pkc_data_t * const p_point_ha,
                                            const st_pkc_data_t * const p_point_sb,
                                            const st_pkc_data_t * const p_sign_r);

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaMakeScalarSandR
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIs
 * \brief       Generate interamediate data scalar s and scalar r
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] p_priv_key     EdDSA private key of key binary format.
 * \param       [in] p_msg          Message to be signed.
 * \param       [in,out] p_scalar_s An integer s derived expanding private key.
 *                                  Set p_data buffer size in len when inputting. <br>
 *                                  At the time of output, set the data in p_data, set the data size in len and return.
 * \param       [in,out] p_scalar_r A scalar r derived hashing over integer s and message.
 *                                  Set p_data buffer size in len when inputting. <br>
 *                                  At the time of output, set the data in p_data, set the data size in len and return.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_HASHAPIERR:               Hash API returned error
 * \par Security components
 *  - No
 * \par Global Variables
 *   - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - Satisfy both of the following conditions:
 *    - #R_PKCD_Initialize has been called.
 * \par Detailed description
 *  - Generate intermediate data integers S and R used for signature generation from private key and message
 *  - For Ed25519ctx and Ed25519ph, the fixed ASCII character string, pre-hash flag,
 *    and context are added to the hash target data when the intermediate data integer R is generated.
 *  - Since the Ed448 private key is padded data that is a multiple of 4,
 *    we are changing the position to pass the data when hashing.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Check the arguments with #r_pkcd_eddsa_chk_make_scalar_s_and_r_arg, and set ret to the return value.
 *  -# If (#PKCD_RET_OK == ret)
 *    -# Set p_key_bin to (const #st_pkcd_eddsa_key_bin_t*)p_priv_key.
 *    -# Set p_hash_ctx to p_ctx->p_hash_info->p_hash_ctx.
 *    -# Clear p_scalar_r->p_data with #r_pkcd_cmn_ram_clr.
 *    -# If #PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len
 *      -# Set p_ascii_data to s_ed25519_ascii.
 *      -# Set ascii_len to #PKCD_EDDSA_ED25519_ASCII_SIZE.
 *      -# Set key_len to #PKCD_EDDSA_ED25519_KEY_LEN.
 *    -# Else
 *      -# Set p_ascii_data to s_ed448_ascii.
 *      -# Set ascii_len to #PKCD_EDDSA_ED448_ASCII_SIZE.
 *      -# Set key_len to #PKCD_EDDSA_ED448_KEY_LEN.
 *      -# Set hashed_priv_key.len to #PKCD_EDDSA_SHAKE256BYTES.
 *      -# Set p_scalar_r->len to #PKCD_EDDSA_SHAKE256BYTES.
 *    -# Execute an external hash init function p_ctx->p_hash_info->p_hash_init_api to initialize a hash context,
 *       and set the return value to hash_ret.
 *    -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *      -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *         to hash a private key, and set the return value to hash_ret.
 *      -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *        -# Execute an external hash final function p_ctx->p_hash_info->p_hash_final_api,
 *           to get a hased private key, and set the hashed private key and its byte size to the #st_eddsa_scalar_t
 *           type variable hashed_priv_key.
 *        -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *          -# If #PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len
 *            -# Mask some bits of hashed_priv_key.scalar according to Ed25519 section of RFC8032 as follows.
 *              -# Set (bit-wise AND of hashed_priv_key.scalar[0] and 0xF8) to hashed_priv_key.scalar[0].
 *              -# Set (bit-wise AND of hashed_priv_key.scalar[31] and 0x3F) to hashed_priv_key.scalar[31].
 *              -# Set (bit-wise OR of hashed_priv_key.scalar[31] and 0x40) to hashed_priv_key.scalar[31].
 *            -# Get the scalar s from the first half of hashed_priv_key.scalar
 *               and set s to p_scalar_s->p_data with #r_pkcd_cmn_cpy_word.
 *          -# Else
 *            -# Mask some bits of hashed_priv_key.scalar according to Ed448 section of RFC8032 as follows.
 *              -# Set (bit-wise AND of hashed_priv_key.scalar[0] and 0xFC) to hashed_priv_key.scalar[0].
 *              -# Set (bit-wise AND of hashed_priv_key.scalar[56] and 0x00) to hashed_priv_key.scalar[56].
 *              -# Set (bit-wise OR of hashed_priv_key.scalar[55] and 0x80) to hashed_priv_key.scalar[55].
 *            -# Get the scalar s from the first half of hashed_priv_key.scalar
 *               and set s to p_scalar_s->p_data with #r_pkcd_cmn_cpy_word.
 *          -# Set p_scalar_s->len to #PKCD_EDDSA_ED448_RU4_KEY_LEN.
 *        -# Else
 *          -# Set ret to #PKCD_RET_HASHAPIERR.
 *      -# Else
 *        -# Set ret to #PKCD_RET_HASHAPIERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_HASHAPIERR.
 *    -# If (#PKCD_RET_OK == ret)
 *      -# Execute an external hash init function p_ctx->p_hash_info->p_hash_init_api to initialize a hash context,
 *         and set the return value to hash_ret.
 *      -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *        -# If ((#PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len) and
 *               (#PKC_EDDSA_VARIANT_PURE != p_ctx->eddsa_variant)) or
 *              (#PKCD_EDDSA_CURVE_EDWARDS448_LEN == p_ctx->p_curve_info->curve_len)
 *          -# If (#PKC_EDDSA_VARIANT_HASH == p_ctx->eddsa_variant) then Set ph_flag to 1U,
 *               otherwise Set ph_flag to 0U.
 *          -# Execute the external hash update function p_ctx->p_hash_info->p_hash_update_api
 *             to hash the fixed ASCII string and set the return value to hash_ret.
 *          -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *            -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *               to hash a pre-hash flag, and set the return value to hash_ret.
 *            -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *              -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *                 to hash a ed_ctx length, and set the return value to hash_ret.
 *              -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *                -# If (0U != p_ctx->p_ed_ctx->len)
 *                  -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *                     to hash a ed_ctx data, and set the return value to hash_ret.
 *              -# Else
 *                -# Set ret to #PKCD_RET_HASHAPIERR.
 *            -# Else
 *              -# Set ret to #PKCD_RET_HASHAPIERR.
 *          -# Else
 *            -# Set ret to #PKCD_RET_HASHAPIERR.
 *        -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *          -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *             to hash the second half hashed_priv_key.scalar, and set the return value to hash_ret.
 *          -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *            -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *               to hash a message, and set the return value to hash_ret.
 *            -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *              -# Execute an external hash final function p_ctx->p_hash_info->p_hash_final_api,
 *                 to get the scalar r, and set r and its byte size to p_scalar_r.
 *              -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *                -# If #PKCD_EDDSA_CURVE_EDWARDS448_LEN == p_ctx->p_curve_info->curve_len
 *                  -# Set p_scalar_r->len to #PKCD_EDDSA_ED448_DIGEST_LEN.
 *              -# Else
 *                -# Set ret to #PKCD_RET_HASHAPIERR.
 *            -# Else
 *              -# Set ret to #PKCD_RET_HASHAPIERR.
 *          -# Else
 *            -# Set ret to #PKCD_RET_HASHAPIERR.
 *        -# Else
 *          -# Set ret to #PKCD_RET_HASHAPIERR.
 *      -# Else
 *        -# Set ret to #PKCD_RET_HASHAPIERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EddsaMakeScalarSandR (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                uint8_t * const p_buf,
                                                const void * const p_priv_key,
                                                const st_pkc_data_t * const p_msg,
                                                st_pkc_data_t * const p_scalar_s,
                                                st_pkc_data_t * const p_scalar_r);

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaHashRAM
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIs
 * \brief       Hashed sign r, public key A, message m
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 * \param       [in] p_sign_r       A signature r.
 * \param       [in] p_pub_key      EdDSA public key of key binary format.
 * \param       [in] p_msg          Message to be signed
 *                                  Set p_data buffer size in len when inputting. <br>
 *                                  At the time of output, set the data in p_data, set the data size in len and return.
 * \param       [in,out] p_digest_r_a_m      Hash of sign r, public key A and message m
 *                                           Set p_data buffer size in len when inputting. <br>
 *                                           At the time of output, set the data in p_data,
 *                                           set the data size in len and return.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_HASHAPIERR:               Hash API returned error
 * \par Security components
 *  - No
 * \par Global Variables
 *   - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  - Satisfy both of the following conditions:
 *    - #R_PKCD_Initialize has been called.
 * \par Detailed description
 *  - Hashed sign r, public key A, message m
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Initialize the value sign_r_position_add with zero.
 *  -# Initialize the value pubkey_position_add with zero.
 *  -# Check the arguments with #r_pkcd_eddsa_chk_hash_r_a_m_arg, and set ret to the return value.
 *  -# If (#PKCD_RET_OK == ret)
 *    -# Set (const #st_pkcd_eddsa_key_bin_t*)p_pub_key to p_key_bin.
 *    -# Clear p_scalar_r->p_data with #r_pkcd_cmn_ram_clr.
 *    -# If #PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len
 *      -# Set p_ascii_data to s_ed25519_ascii.
 *      -# Set ascii_len to #PKCD_EDDSA_ED25519_ASCII_SIZE.
 *      -# Set sign_r_len to #PKCD_EDDSA_ED25519_KEY_LEN.
 *      -# Set pubkey_len to #PKCD_EDDSA_ED25519_KEY_LEN.
 *    -# Else
 *      -# Set p_ascii_data to s_ed448_ascii.
 *      -# Set ascii_len to #PKCD_EDDSA_ED448_ASCII_SIZE.
 *      -# Set sign_r_len to #PKCD_EDDSA_ED448_KEY_LEN.
 *      -# Set pubkey_len to #PKCD_EDDSA_ED448_KEY_LEN.
 *      -# Set p_digest_r_a_m->len to #PKCD_EDDSA_SHAKE256BYTES.
 *    -# Execute an external Hash init function p_ctx->p_hash_info->p_hash_init_api to initialize a hash context,
 *       and set the return value to hash_ret.
 *    -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *      -# If ((#PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len) and
 *             (#PKC_EDDSA_VARIANT_PURE != p_ctx->eddsa_variant)) or
 *            (#PKCD_EDDSA_CURVE_EDWARDS448_LEN == p_ctx->p_curve_info->curve_len)
 *        -# If (#PKC_EDDSA_VARIANT_HASH == p_ctx->eddsa_variant) then Set ph_flag to 1U,
 *             otherwise Set ph_flag to 0U.
 *        -# Execute the external hash update function p_ctx->p_hash_info->p_hash_update_api
 *           to hash the fixed ASCII string and set the return value to hash_ret.
 *        -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *          -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *             to hash a pre-hash flag, and set the return value to hash_ret.
 *          -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *            -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *               to hash a ed_ctx length, and set the return value to hash_ret.
 *            -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *              -# If (0U != p_ctx->p_ed_ctx->len)
 *                -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *                   to hash a ed_ctx data, and set the return value to hash_ret.
 *            -# Else
 *              -# Set ret to #PKCD_RET_HASHAPIERR.
 *          -# Else
 *            -# Set ret to #PKCD_RET_HASHAPIERR.
 *        -# Else
 *          -# Set ret to #PKCD_RET_HASHAPIERR.
 *      -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *        -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *           to hash p_sign_r->p_data, and set the return value to hash_ret.
 *        -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *          -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *             to hash p_key_bin->p_public, and set the return value to hash_ret.
 *          -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *            -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *               to hash a message, and set the return value to hash_ret.
 *            -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *              -# Execute an external hash final function p_ctx->p_hash_info->p_hash_final_api
 *                 to set the hased value and its byte size to p_digest_r_a_m,
 *                 and set the return value to hash_ret.
 *              -# If (#PKCD_CMN_EXTERNAL_API_OK == hash_ret)
 *                -# If (PKCD_EDDSA_CURVE_EDWARDS448_LEN == p_ctx->p_curve_info->curve_len)
 *                  -# Set p_digest_r_a_m->len to #PKCD_EDDSA_ED448_DIGEST_LEN.
 *              -# Else
 *                -# Set ret to #PKCD_RET_HASHAPIERR.
 *            -# Else
 *              -# Set ret to #PKCD_RET_HASHAPIERR.
 *          -# Else
 *            -# Set ret to #PKCD_RET_HASHAPIERR.
 *        -# Else
 *          -# Set ret to #PKCD_RET_HASHAPIERR.
 *      -# Else
 *        -# Set ret to #PKCD_RET_HASHAPIERR.
 *    -# Else
 *      -# Set ret to #PKCD_RET_HASHAPIERR.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EddsaHashRAM (const st_pkc_eddsa_ctx_t * const p_ctx,
                                        uint8_t * const p_buf,
                                        const st_pkc_data_t * const p_sign_r,
                                        const void * const p_pub_key,
                                        const st_pkc_data_t * const p_msg,
                                        st_pkc_data_t * const p_digest_r_a_m);

/**********************************************************************************************************************
* Function Name : R_PKCD_EddsaKeyWrap
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDEDDSAAPIs
 * \brief       Wrap in key binary format
 * \param       [in] p_ctx              A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in,out] p_buf          Buffer used for internal processing of PKC-Driver
 * \param       [in] p_priv_key_raw     Byte array of private key.
 * \param       [in] p_pub_key_raw      Byte array of public key.
 * \param       [in,out] p_priv_key     Output EdDSA private key of key binary format.
 *                                      Set p_data buffer size in len when inputting. <br>
 *                                      At the time of output, set the data in p_data, set the data size <br>
 *                                      in len and return.
 * \param       [in,out] p_pub_key      Output EdDSA public key of key binary format.
 *                                      Set p_data buffer size in len when inputting. <br>
 *                                      At the time of output, set the data in p_data, set the data size <br>
 *                                      in len and return.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *    - #R_PKCD_Initialize has been called.
 * \par Detailed description
 *    - Convert the key data of the input byte array to the key binary format
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the value ret with #PKCD_RET_INTERNALERR.
 *  -# Check the arguments with #r_pkcd_eddsa_chk_hash_r_a_m_arg, and set ret to the return value.
 *  -# If (#PKCD_RET_OK == ret)
 *    -# Set (#st_pkcd_eddsa_key_bin_t*)p_priv_key to p_priv_key_bin.
 *    -# Set (#st_pkcd_eddsa_key_bin_t*)p_pub_key to p_pub_key_bin.
 *    -# If (#PKCD_EDDSA_CURVE_EDWARDS25519_LEN == p_ctx->p_curve_info->curve_len) then
 *         Set out_key_len to #PKCD_EDDSA_ED25519_KEY_LEN, otherwise Set out_key_len to #PKCD_EDDSA_ED448_RU4_KEY_LEN.
 *    -# Clear p_priv_key_bin->p_private with #r_pkcd_cmn_ram_clr.
 *    -# Clear p_priv_key_bin->p_public with #r_pkcd_cmn_ram_clr.
 *    -# Clear p_pub_key_bin->p_public with #r_pkcd_cmn_ram_clr.
 *    -# Set p_priv_key_bin->key_length to out_key_len.
 *    -# Set p_priv_key_raw->p_data to p_priv_key_bin->p_private with #r_pkcd_cmn_cpy_word.
 *    -# Set p_pub_key_raw->p_data to p_priv_key_bin->p_private with #r_pkcd_cmn_cpy_word.
 *    -# Set p_pub_key_bin->key_length to out_key_len.
 *    -# Set p_pub_key_raw->p_data to p_pub_key_bin->p_private with #r_pkcd_cmn_cpy_word.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EddsaKeyWrap (const st_pkc_eddsa_ctx_t * const p_ctx,
                                        uint8_t * const p_buf,
                                        const st_pkc_data_t * const p_priv_key_raw,
                                        const st_pkc_data_t * const p_pub_key_raw,
                                        void * const p_priv_key,
                                        void * const p_pub_key);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcdhXScalarMultInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECDHXAPIs
 * \brief       Prepare for scalar multiplication on curve25519/curve448.
 * \param       [in] p_scalar_or_privkey    A pointer to a scalar or a private key structure.
 *                                          - case p_scalar_or_privkey->type == #PKC_DATA_TYPE_BYTE<br>
 *                                            p_sel_data->p_data member points a scalar byte array. <br>
 *                                            And p_sel_data->len member must be its byte length.
 *                                          - case p_scalar_or_privkey->type == #PKC_DATA_TYPE_KEY_BINARY<br>
 *                                            p_sel_data->p_data member points a driver-dependent key binary. <br>
 *                                            For the RH850/U2B driver, it takes #st_pkcd_ecdhx_key_bin_t format.
 *                                            And no need to set p_sel_data->len member.
 * \param       [in] p_point_or_pubkey      A pointer to a curve point(u-coordinate) or public key structure.
 *                                          - case p_point_or_pubkey->type == #PKC_DATA_TYPE_BYTE<br>
 *                                            p_sel_data->p_data member points a u-coordinate byte array. <br>
 *                                            And p_sel_data->len member must be its byte length.
 *                                          - case p_point_or_pubkey->type == #PKC_DATA_TYPE_KEY_BINARY<br>
 *                                            p_sel_data->p_data member points a driver-dependent key binary. <br>
 *                                            For the RH850/U2B driver, it takes #st_pkcd_ecdhx_key_bin_t format. <br>
 *                                            And no need to set p_sel_data->len member.
 * \param       [in] p_ctx                  A pointer to a structure that summarizes the parameters required <br>
 *                                          by X25519/X448.
 *                                          PKC-Lib directly set the same values input in R_PKCL_EcdhXOperate or <br>
 *                                          R_PKCL_EcdhXKeyGenOperate API.
 *                                          Specify the entity of the same structure <br>
 *                                          with the Init/Operate/Final function. <br>
 *                                          Do not change the value of a member variable until the Final function <br>
 *                                          completes.
 * \param       [in] p_buf                  A buffer used for internal processing of PKC-Driver.
 *                                          Specify the same buffer with the Init/Operate/Final function
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_CMN_PKCCRAM
 *  - #PKCD_CMN_PKCCRAMSIZE
 * \par Registers Used
 *  - None
 * \par Precondition
 *  Satisfy both of the following conditions:
 *    - #R_PKCD_Initialize has been called.
 *    - The PKCC is not running.
 * \par Detailed description
 *    - Prepare for the scalar multiplication operation of the following :
 *      - Clear PKCCRAM
 *      - Save input parameters in the buffer p_buf
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Set ret to #PKCD_RET_INTERNALERR
 * -# Check input parameters with #r_pkcd_ecdhx_chk_scalar_mult_init_arg and set ret to the return value.
 * -# If ret == #PKCD_RET_OK then
 *   -# Clear the async operation flag which indicates ongoing async operation with #r_pkcd_cmn_clr_async_ope.
 *   -# Clear #PKCD_CMN_PKCCRAM with #r_pkcd_cmn_ram_clr.
 *   -# Clear p_buf with #r_pkcd_cmn_ram_clr.
 *   -# Initialize p_prm->executed_bits, which indicates the number of proccesed bits, with 0.
 *   -# If p_point_or_privkey->data_type == #PKC_DATA_TYPE_KEY_BINARY then
 *     -# Get the scalar value and its byte size from the public key.
 *     -# Set the scalar value and its byte size into the buffer p_buf.
 *   -# Else
 *     -# Set the scalar value and its byte size to the buffer p_buf.
 *   -# If PKCD_ECDHX_CURVE25519_LEN == p_ctx->p_curve_info->curve_len
 *     -# Decode the scalar value following RFC7748 like below.
 *       -# Bitwise-and the most significant byte with 0x7F.
 *       -# Bitwise-or the most significant byte with 0x40.
 *       -# Bitwise-and the least significant byte with 0xF8.
 *   -# Else
 *     -# Decode the scalar value following RFC7748 like below.
 *       -# Bitwise-or the most significant byte with 0x80.
 *       -# Bitwise-and the least significant byte with 0xFC.
 *   -# Change the endianness of the scalar value with #r_pkcd_ecdhx_encode_data.
 *   -# If p_point_or_pubkey->data_type == #PKC_DATA_TYPE_KEY_BINARY then
 *     -# Get the u-coordinate value and its byte size from the public key.
 *     -# Set the u-coordinate value and its byte size into the buffer p_buf.
 *   -# Else
 *     -# Set the u-coordinate value and its byte size to the buffer of the u-coordinate value
 *        with #r_pkcd_ecdhx_encode_data.
 *   -# With #r_pkcd_ecdhx_set_curve_params, get the curve information from p_ctx->p_curve_info,
 *      and set the curve information into the buffer p_buf.
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcdhXScalarMultInit (const st_pkc_select_data_t * const p_scalar_or_privkey,
                                                const st_pkc_select_data_t * const p_point_or_pubkey,
                                                const st_pkc_ecdhx_ctx_t * const p_ctx,
                                                uint8_t * const p_buf);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcdhXScalarMultOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECDHXAPIs
 * \brief       Perform scalar multiplication for points on curve25519/curve448.
 * \param       [in] p_ctx            A pointer to a structure that summarizes the parameters required by EC.
 *                                    Specify the entity of the same structure with the Init/Operate/Final functions.
 *                                    Do not change the value of a member variable until the Final function completes.
 * \param       [in,out] p_buf        Buffer used for internal processing of PKC-Driver
 *                                    Specify the same buffer with the Init/Operate/Final function
 * \param       [in] processing_bits  A bit size processed in a single execution of this API.
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCD_RET_KEY_PARAMERR:             Invalid data entered in key binary format data
 *  - #PKCD_RET_KEY_LENGTHERR:            Invalid length entered in key binary format data
 *  - #PKCD_RET_PKCCTIMEOUTERR:           PKCC operation timeouted
 *  - #PKCD_RET_PKCCERR:                  An error occurred while the PKCC operations
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #PKCD_ECDHX_WORK1, #PKCD_ECDHX_WORK2, #PKCD_ECDHX_WORK3, #PKCD_ECDHX_PBYTES, #PKCD_ECDHX_OFFSET_VAL, <br>
 *    #PKCD_ECDHX_WORKUNIT
 * \par Registers Used
 *  - #PKCD_CMN_ICUM_PKCCB0CR, #PKCD_CMN_ICUM_PKCCB0OMR, #PKCD_CMN_ICUM_PKCCB0STR, <br>
 *    #PKCD_CMN_ICUM_PKCCB0STC, #PKCD_CMN_ICUM_PKCCB0DPA, #PKCD_CMN_ICUM_PKCCB0DPB, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPC, #PKCD_CMN_ICUM_PKCCB0DPS, #PKCD_CMN_ICUM_PKCCB0DPW, <br>
 *    #PKCD_CMN_ICUM_PKCCB0DPT (Used by sub functions)
 * \par Precondition
 *  Satisfy both of the following conditions:
 *    - #R_PKCD_EcdhXScalarMultInit has been called.
 *    - The PKCC is not running.
 * \par Detailed description
 *    - Perform scalar multiplication for points on curve25519/curve448.
 *    - Select the bit size processed in a single execution of this API by the argument processing_bits.
 *
 * \par Remarks
 *  - Before calling this function, you must set the argument p_ctx and p_buf using #R_PKCD_EcdhXScalarMultInit. <br>
 *    If you set a value different from #R_PKCD_RsaModExpInit, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# If p_ctx and p_buf are not NULL then
 *   -# Check curve parameters with #r_pkcd_ecdhx_chk_curve_info. And set ret to the result.
 *   -# If #PKCD_RET_OK == ret then
 *     -# If #PKCD_CMN_PROCESSING_BITS_MIN <= processing_bits <= #PKCD_CMN_PROCESSING_BITS_MAX then
 *       -# Clear the async operation flag which indicates ongoing async operation with #r_pkcd_cmn_clr_async_ope.
 *       -# Assign #PKCD_ECDHX_WORK1, #PKCD_ECDHX_WORK2 and #PKCD_ECDHX_WORK3 as work areas <br>
 *          for ECDHX scalar multiplication operation.
 *       -# If processing_bits == 0U then,
 *         -# Set byte length of the curve in p_buf area into #PKCD_ECDHX_PBYTES.
 *         -# Set (#PKCD_ECDHX_WORKUNIT - #PKCD_ECDHX_PBYTES) to #PKCD_ECDHX_OFFSET_VAL.
 *         -# Set ICUM_PKCCB0OMR, DPW, DPT registers and prepare values to be used with
 *            #r_pkcd_ecdhx_precalc_params, and add the return value to ret.
 *         -# Perform scalar multiplication with #r_pkcd_ecdhx_point_smult,
 *            and set the output u-coordinate value and its length to p_buf.
 *         -# Add the return value of #r_pkcd_ecdhx_point_smult to ret.
 *         -# Extract the return value with #r_pkcd_cmn_extract_result.
 *         -# If #PKCD_RET_PROCESSING != ret
 *           -# Use #r_pkcd_cmn_ram_clr to clear the potentially private key value stored in p_prm.
 *     -# Else
 *       -# Set ret to #PKCD_RET_LENGTHERR.
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR.
 * -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcdhXScalarMultOperate (const st_pkc_ecdhx_ctx_t * const p_ctx,
                                                uint8_t * const p_buf,
                                                const uint32_t processing_bits);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcdhXScalarMultFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECDHXAPIs
 * \brief       Finalize scalar multiplication on curve25519/curve448.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC. <br>
 *                                  Specify the entity of the same structure with the Init/Operate/Final function. <br>
 *                                  Do not change the value of a member variable until the Final function completes.
 * \param       [in,out] p_buf      Buffer used for internal processing of PKC-Driver
 *                                  Specify the same buffer with the Init/Operate/Final function
 * \param       [in,out] p_point    Output u-coordinate data. <br>
 *                                  Set p_point->p_data buffer size 32 in p_point->len when inputting. <br>
 *                                  At the time of output, this function sets the output data into p_data,
 *                                  and set its byte size into p_point->len.
 *
 * \return
 *  - #PKCD_RET_OK:                       Successful completion
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed
 *  - #PKCD_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCD_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *  Satisfy all of the following conditions:
 *    - #R_PKCD_EcdhXScalarMultOperate has been called.
 *    - The PKCC is not running.
 * \par Detailed description
 *    - Finalize scalar multiplication on curve25519/curve448.
 *    - Extract the result of scalar multiplication on curve25519/curve448 from the buffer p_buf.
 * \par Remarks
 *  - Be sure to set the values used in #R_PKCD_EcdhXScalarMultOperate for the arguments p_ctx and p_buf. <br>
 *    If you set a value different from #R_PKCD_EcdhXScalarMultOperate, the result is not guaranteed.
 *
 * Pseudo-code for the function can be found below.
 * -# Initialize the variable ret with #PKCD_RET_INTERNALERR.
 * -# Extract the result of #R_PKCD_EcdhXScalarMultOperate, which consists of an u-coordinate value and its byte size
 *    from p_buf.
 * -# If p_ctx, p_buf, and p_point in not NULL then
 *   -# If p_point->p_data is not NULL then
 *     -# Check curve parameters with #r_pkcd_ecdhx_chk_curve_info. And set ret to the result.
 *     -# if ret == #PKCD_RET_OK then,
 *       -# If (p_point->len) >= (p_ctx->p_curve_info->curve_len) then,
 *         -# Copy resulting u-coordinate length from p_buf to p_point->len.
 *         -# Copy resulting u-coordinate value from p_buf to p_point->p_data with #r_pkcd_cmn_cpy_word.
 *       -# Else
 *         -# Set ret to #PKCD_RET_LENGTHERR.
 *   -# Else
 *     -# Set ret to #PKCD_RET_PARAMERR.
 * -# Else
 *   -# Set ret to #PKCD_RET_PARAMERR.
 * -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcdhXScalarMultFinal (const st_pkc_ecdhx_ctx_t * const p_ctx,
                                                uint8_t * const p_buf,
                                                st_pkc_data_t * const p_point);

/**********************************************************************************************************************
* Function Name : R_PKCD_EcdhxKeyWrap
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCDECDHXAPIs
 * \brief       Wrap in key binary format
 * \param       [in] p_ctx              A pointer to a structure that summarizes the parameters required.
 * \param       [in,out] p_buf          A buffer used for internal processing of PKC-Driver.
 * \param       [in] p_priv_key_raw     A byte array of scalar data.
 * \param       [in] p_pub_key_raw      A byte array of u-coordinate data.
 * \param       [in,out] p_priv_key     A pointer to a driver-dependent format of a structure, which consists of
 *                                      key length key_length, a buffer for a private key p_private, and a buffer for
 *                                      a public key p_public.<br>
 *                                      At the time of input, set the address of the buffers p_private and
 *                                      p_public.<br>
 *                                      At the time of output, this function sets the scalar data into
 *                                      the buffer p_private, the u-coordinate data into the buffer p_public
 *                                      and the key length into key_length.
 * \param       [in,out] p_pub_key      A pointer to a driver-dependent format of a structure, which consists of
 *                                      key length key_length, a buffer for a private key p_private, and a buffer for
 *                                      a public key p_public.<br>
 *                                      At the time of input, set the address of the buffers p_public.<br>
 *                                      At the time of output, this function sets the scalar data into
 *                                      the buffer p_private, the u-coordinate data into the buffer p_public
 *                                      and the key length into key_length.
 * \return
 *  - #PKCD_RET_OK:                       Successfully done.
 *  - #PKCD_RET_INTERNALERR:              Internal processing failed.
 *  - #PKCD_RET_PARAMERR:                 Invalid arguments entered.
 *  - #PKCD_RET_LENGTHERR:                Invalid arguments length entered.
 *  - #PKCD_RET_KEY_PARAMERR:             NULL key parameters entered.
 *
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Registers Used
 *  - None
 * \par Precondition
 *    - #R_PKCD_Initialize has been called.
 * \par Detailed description
 *    - Convert the input byte arrays to the driver-dependent key structures.
 *    - Set both input scalar data and input u-coordinate date into the argument p_prive_key,
 *      and an input u-coordinate date into the argument p_pub_key.
 * \par Remarks
 *  - At the time of input, you do not need set the adress of p_pub_key->p_private,
 *    unlike in the case of p_priv_key->p_public.
 *
 * Pseudo-code for the function can be found below.
 *  -# Initialize the valiable ret with #PKCD_RET_INTERNALERR.
 *  -# Check the arguments with #r_pkcd_ecdhx_chk_key_wrap_arg and set ret to the return value.
 *  -# If #PKCD_RET_OK == ret then
 *    -# Set the key length (equal to curve length) into p_priv_key->key_length.
 *    -# Set the scalar value p_priv_key_raw into p_priv_key->p_private.
 *    -# Set the u-coordinate value p_pub_key_raw with p_priv_key->p_public.
 *    -# Set the key length (equal to curve length) into p_pub_key->key_length.
 *    -# Set the u-coordinate value into p_pub_key->p_public.
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcd_ret_t R_PKCD_EcdhXKeyWrap (const st_pkc_ecdhx_ctx_t * const p_ctx,
                                        uint8_t * const p_buf,
                                        const st_pkc_data_t * const p_priv_key_raw,
                                        const st_pkc_data_t * const p_pub_key_raw,
                                        void * const p_priv_key,
                                        void * const p_pub_key);

#endif /* R_PKCD_API_H */
