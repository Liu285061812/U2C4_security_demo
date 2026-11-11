/******************************************************************************/
/* Component Name   PKC Library application header file                       */
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

#ifndef R_PKCL_API_H
/* Multiple inclusion protection macro */
#define R_PKCL_API_H

/* PRQA S 0776, 0778 ++ # D008 Develop according to C99 */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

/*!**************************************************************************************
 * \addtogroup  PKCLib          PKC-Lib module
 * \{
 * \addtogroup  PKCCommon       PKC-Lib/Driver Common definitions
 * \addtogroup  PKCLCommon      PKC-Lib Common module
 * \addtogroup  PKCLRSA         RSA module
 * \addtogroup  PKCLDH          Diffie-Hellman module
 * \addtogroup  PKCLEC          Elliptic curve module
 * \addtogroup  PKCLEDDSA       Edwards curve digital signature algorithm module
 * \addtogroup  PKCLECDHX       Edwards curve Diffie-Hellman module
 * \addtogroup  PKCLKDF         Key derivation function module
 * \}***********************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \addtogroup  PKCLCommonDefines             Common Definitions
 * \{******************************************************************/
/** PKC-Library return value definition */
#define PKCL_RET_OK                        ((pkcl_ret_t)0x55555555U)  /**< API succeeded */
#define PKCL_RET_PROCESSING                ((pkcl_ret_t)0x55005501U)  /**< API succeeded. All the processing is not
                                                                           completed, so you need to call the same
                                                                           API again */
#define PKCL_RET_PRIME_GENERATING          ((pkcl_ret_t)0x55005502U)  /**< Prime number is being generated. The same
                                                                           API needs to be called again because the
                                                                           prime number generation has not been
                                                                           completed. */
#define PKCL_RET_INTERNALERR               ((pkcl_ret_t)0xAAAA0000U)  /**< Internal error */
#define PKCL_RET_STATUSERR                 ((pkcl_ret_t)0xAAAA0001U)  /**< Executed in PKC State different from
                                                                           expected */
#define PKCL_RET_PARAMERR                  ((pkcl_ret_t)0xAAAA0002U)  /**< Input parameter error */
#define PKCL_RET_LENGTHERR                 ((pkcl_ret_t)0xAAAA0003U)  /**< Data length error */
#define PKCL_RET_HASHAPIERR                ((pkcl_ret_t)0xAAAA0004U)  /**< External HASH API error */
#define PKCL_RET_DRBGAPIERR                ((pkcl_ret_t)0xAAAA0005U)  /**< External HASH DRBG error */
#define PKCL_RET_KDFAPIERR                 ((pkcl_ret_t)0xAAAA0006U)  /**< External HASH KDF error */
#define PKCL_RET_TAMPERERR                 ((pkcl_ret_t)0xAAAA0007U)  /**< Error due to tampering */
#define PKCL_RET_DRV_PARAMERR              ((pkcl_ret_t)0xAAAA0800U)  /**< Input parameter error(PKC-Driver) */
#define PKCL_RET_DRV_LENGTHERR             ((pkcl_ret_t)0xAAAA0801U)  /**< Data length error(PKC-Driver) */
#define PKCL_RET_DRV_INTERNALERR           ((pkcl_ret_t)0xAAAA0802U)  /**< Internal error(PKC-Driver) */
#define PKCL_RET_PKCCTIMEOUTERR            ((pkcl_ret_t)0xAAAA0803U)  /**< PKCC timeout error */
#define PKCL_RET_PKCCERR                   ((pkcl_ret_t)0xAAAA0804U)  /**< PKCC operation error */
#define PKCL_RET_KEY_PARAMERR              ((pkcl_ret_t)0xAAAA0805U)  /**< Data error in key binary format */
#define PKCL_RET_KEY_LENGTHERR             ((pkcl_ret_t)0xAAAA0806U)  /**< Data length error in key binary format */
#define PKCL_RET_DRV_FAIL                  ((pkcl_ret_t)0xAAAA0807U)  /**< Other PKC-Driver errors */
#define PKCL_RET_DRV_TAMPERERR             ((pkcl_ret_t)0xAAAA0808U)  /**< Error due to tampering(PKC-Driver) */
#define PKCL_RET_RSA_VERIFAIL              ((pkcl_ret_t)0xAAAA1000U)  /**< RSA verification failed */
#define PKCL_RET_RSA_SCHEMEERR             ((pkcl_ret_t)0xAAAA1001U)  /**< An error occurred in the RSA scheme */
#define PKCL_RET_RSA_NON_COPRIME           ((pkcl_ret_t)0xAAAA1800U)  /**< A non-prime combination occurred during
                                                                           key generation */
#define PKCL_RET_RSA_COMPOSITE             ((pkcl_ret_t)0xAAAA1801U)  /**< The prime number candidate data was a
                                                                           composite number */
#define PKCL_RET_DH_INVALID_SHAREDSECRET   ((pkcl_ret_t)0xAAAA2800U)  /**< An invalid SharedSecret was generated */
#define PKCL_RET_EC_VERIFAIL               ((pkcl_ret_t)0xAAAA3800U)  /**< ECDSA verification failed */
#define PKCL_RET_EC_ZERO_SIGNATURE         ((pkcl_ret_t)0xAAAA3801U)  /**< Generated an ECDSA siganture with a
                                                                           value of 0 */
#define PKCL_RET_EDDSA_ZERO_SIGNATURE      ((pkcl_ret_t)0xAAAA4000U)  /**< Generated an EdDSA siganture with a
                                                                           value of 0 */
#define PKCL_RET_EDDSA_VERIFAIL            ((pkcl_ret_t)0xAAAA4800U)  /**< EdDSA verification failed */
#define PKCL_RET_EDDSA_DECOMPERR           ((pkcl_ret_t)0xAAAA4801U)  /**< Failed to decompress the coordinates */
/*! \}*/

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/*!********************************************************************
 * \addtogroup  PKCLCommonTypes               Common Types
 * \{******************************************************************/
typedef uint32_t pkcl_ret_t;                        /**< PKC-Library return type */

/** PKC State */
typedef enum
{
    PKCL_STATE_INIT = 0x00000000,                               /**< PKC-Lib initial state */
    PKCL_STATE_NO_EXEC_OPERATION,                               /**< A series of processes of Init function, Operate
                                                                     function, Final function is not in progress */
    PKCL_STATE_RSA_ENC_INIT_DONE = 0x00001000,                  /**< R_PKCL_RsaEncryptInit completed */
    PKCL_STATE_RSA_ENC_OPERATE_PROCESSING,                      /**< R_PKCL_RsaEncryptOperate in progress */
    PKCL_STATE_RSA_ENC_OPERATE_DONE,                            /**< R_PKCL_RsaEncryptOperate completed */
    PKCL_STATE_RSA_DEC_INIT_DONE,                               /**< R_PKCL_RsaDecryptInit completed */
    PKCL_STATE_RSA_DEC_OPERATE_PROCESSING,                      /**< R_PKCL_RsaDecryptOperate in progress */
    PKCL_STATE_RSA_DEC_OPERATE_DONE,                            /**< R_PKCL_RsaDecryptOperate completed */
    PKCL_STATE_RSA_SIGN_INIT_DONE,                              /**< R_PKCL_RsaSignInit completed */
    PKCL_STATE_RSA_SIGN_OPERATE_PROCESSING,                     /**< R_PKCL_RsaSignOperate in progress */
    PKCL_STATE_RSA_SIGN_OPERATE_DONE,                           /**< R_PKCL_RsaSignOperate completed */
    PKCL_STATE_RSA_VERIFY_INIT_DONE,                            /**< R_PKCL_RsaVerifyInit completed */
    PKCL_STATE_RSA_VERIFY_OPERATE_PROCESSING,                   /**< R_PKCL_RsaVerifyOperate in progress */
    PKCL_STATE_RSA_VERIFY_OPERATE_DONE,                         /**< R_PKCL_RsaVerifyOperate completed */
    PKCL_STATE_RSA_KEYGEN_INIT_DONE,                            /**< R_PKCL_RsaKeyGenInit completed */
    PKCL_STATE_RSA_KEYGEN_OPERATE_PROCESSING,                   /**< R_PKCL_RsaKeyGenOperate in progress */
    PKCL_STATE_RSA_KEYGEN_OPERATE_DONE,                         /**< R_PKCL_RsaKeyGenOperate completed */
    PKCL_STATE_DH_INIT_DONE = 0x00002000,                       /**< R_PKCL_DhInit completed */
    PKCL_STATE_DH_OPERATE_PROCESSING,                           /**< R_PKCL_DhOperate in progress */
    PKCL_STATE_DH_OPERATE_DONE,                                 /**< R_PKCL_DhOperate completed */
    PKCL_STATE_DH_KEYGEN_INIT_DONE,                             /**< R_PKCL_DhKeyGenInit completed */
    PKCL_STATE_DH_KEYGEN_OPERATE_PROCESSING,                    /**< R_PKCL_DhKeyGenOperate in progress */
    PKCL_STATE_DH_KEYGEN_OPERATE_DONE,                          /**< R_PKCL_DhKeyGenOperate completed */
    PKCL_STATE_EC_SIGN_INIT_DONE = 0x00003000,                  /**< R_PKCL_EcdsaSignInit completed */
    PKCL_STATE_EC_SIGN_OPERATE_DONE,                            /**< R_PKCL_EcdsaSignOperate completed */
    PKCL_STATE_EC_VERIFY_INIT_DONE,                             /**< R_PKCL_EcdsaVerifyInit completed */
    PKCL_STATE_EC_VERIFY_OPERATE_PROCESSING,                    /**< R_PKCL_EcdsaVerifyOperate in progress */
    PKCL_STATE_EC_VERIFY_OPERATE_DONE,                          /**< R_PKCL_EcdsaVerifyOperate completed */
    PKCL_STATE_EC_ECDH_INIT_DONE,                               /**< R_PKCL_EcdhInit completed */
    PKCL_STATE_EC_ECDH_OPERATE_DONE,                            /**< R_PKCL_EcdhOperate completed */
    PKCL_STATE_EC_KEYGEN_INIT_DONE,                             /**< R_PKCL_EcKeyGenInit completed */
    PKCL_STATE_EC_KEYGEN_OPERATE_DONE,                          /**< R_PKCL_EcKeyGenOperate completed */
    PKCL_STATE_EC_SMULT_INIT_DONE,                              /**< R_PKCL_LLEcScalarMultInit completed */
    PKCL_STATE_EC_SMULT_OPERATE_DONE,                           /**< R_PKCL_LLEcScalarMultOperate completed */
    PKCL_STATE_EDDSA_SIGN_INIT_DONE = 0x00004000,               /**< R_PKCL_EddsaSignInit completed */
    PKCL_STATE_EDDSA_SIGN_OPERATE_PROCESSING,                   /**< R_PKCL_EddsaSignOperate in progress */
    PKCL_STATE_EDDSA_SIGN_OPERATE_DONE,                         /**< R_PKCL_EddsaSignOperate completed */
    PKCL_STATE_EDDSA_VERIFY_INIT_DONE,                          /**< R_PKCL_EddsaVerifyInit completed */
    PKCL_STATE_EDDSA_VERIFY_OPERATE_PROCESSING,                 /**< R_PKCL_EddsaVerifyOperate in progress */
    PKCL_STATE_EDDSA_VERIFY_OPERATE_DONE,                       /**< R_PKCL_EddsaVerifyOperate Completed */
    PKCL_STATE_EDDSA_KEYGEN_INIT_DONE,                          /**< R_PKCL_EddsaKeyGenInit completed */
    PKCL_STATE_EDDSA_KEYGEN_OPERATE_PROCESSING,                 /**< R_PKCL_EddsaKeyGenOperate in progress */
    PKCL_STATE_EDDSA_KEYGEN_OPERATE_DONE,                       /**< R_PKCL_EddsaKeyGenOperate completed */
    PKCL_STATE_EDDSA_SMULTBASE_INIT_DONE,                       /**< R_PKCL_LLEddsaScalarMultBaseInit completed */
    PKCL_STATE_EDDSA_SMULTBASE_OPERATE_PROCESSING,              /**< R_PKCL_LLEddsaScalarMultBaseOperate progress */
    PKCL_STATE_EDDSA_SMULTBASE_OPERATE_DONE,                    /**< R_PKCL_LLEddsaScalarMultBaseOperate completed */
    PKCL_STATE_EDDSA_SMULT_INIT_DONE,                           /**< R_PKCL_LLEddsaScalarMultInit completed */
    PKCL_STATE_EDDSA_SMULT_OPERATE_PROCESSING,                  /**< R_PKCL_LLEddsaScalarMultOperate progress */
    PKCL_STATE_EDDSA_SMULT_OPERATE_DONE,                        /**< R_PKCL_LLEddsaScalarMultOperate completed */
    PKCL_STATE_ECDHX_INIT_DONE = 0x00005000,                    /**< R_PKCL_EcdhXInit completed */
    PKCL_STATE_ECDHX_OPERATE_PROCESSING,                        /**< R_PKCL_EcdhXOperate in progress */
    PKCL_STATE_ECDHX_OPERATE_DONE,                              /**< R_PKCL_EcdhXOperate completed */
    PKCL_STATE_ECDHX_KEYGEN_INIT_DONE,                          /**< R_PKCL_EcdhXKeyGenInit completed */
    PKCL_STATE_ECDHX_KEYGEN_OPERATE_PROCESSING,                 /**< R_PKCL_EcdhXKeyGenOperate in progress */
    PKCL_STATE_ECDHX_KEYGEN_OPERATE_DONE                        /**< R_PKCL_EcdhXKeyGenOperate completed */
} e_pkcl_state_t;

/*! \}*/

/*!********************************************************************
 * \addtogroup  PKCLKDFTypes      KDF Types
 * \{******************************************************************/
/** Context structure used in the #R_PKCL_KdfSp80056c function */
typedef struct
{
    e_pkc_hash_type_t  hash_type;           /**< Hash type of auxiliary function */
    st_pkc_hash_t *    p_hash_info;         /**< Hash information used in auxiliary function */
    uint8_t *          p_fixed_info;        /**< FixedInfo data */
    uint32_t           fixed_info_len;      /**< FixedInfo byte length */
} st_pkcl_kdf_sp800_56c_ctx_t;

/*! \}*/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
extern const st_pkc_dh_param_t g_pkcl_modp2048param;            /**< MODP-2048 DH domain parameters */
extern const st_pkc_dh_param_t g_pkcl_modp3072param;            /**< MODP-3072 DH domain parameters */
extern const st_pkc_dh_param_t g_pkcl_modp4096param;            /**< MODP-4096 DH domain parameters */
extern const st_pkc_dh_param_t g_pkcl_ffdhe2048param;           /**< ffdhe2048 DH domain parameters */
extern const st_pkc_dh_param_t g_pkcl_ffdhe3072param;           /**< ffdhe3072 DH domain parameters */
extern const st_pkc_dh_param_t g_pkcl_ffdhe4096param;           /**< ffdhe4096 DH domain parameters */

extern const st_pkc_ec_curve_t g_pkcl_p192param;                /**< NIST P-192 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_p224param;                /**< NIST P-224 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_p256param;                /**< NIST P-256 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_p384param;                /**< NIST P-384 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_p521param;                /**< NIST P-521 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_bp256r1param;             /**< BrainpoolP256r1 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_bp256t1param;             /**< BrainpoolP256t1 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_bp320r1param;             /**< BrainpoolP320r1 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_bp320t1param;             /**< BrainpoolP320t1 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_bp384r1param;             /**< BrainpoolP384r1 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_bp384t1param;             /**< BrainpoolP384t1 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_bp512r1param;             /**< BrainpoolP512r1 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_bp512t1param;             /**< BrainpoolP512t1 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_ed25519param;             /**< Twisted curve25519 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_ed448param;               /**< Twisted curve448 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_x25519param;              /**< curve25519 domain parameters */
extern const st_pkc_ec_curve_t g_pkcl_x448param;                /**< curve448 domain parameters */

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
/**********************************************************************************************************************
* Function Name : R_PKCL_Initialize
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLCommonAPIs
 * \brief       Initialize PKC-Library
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_STATUSERR:                Executed in the already initialized state
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - R_PKCD_Initialize
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Clear PKC state and call PKC-Driver initilize API
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Get the PKC state with #r_pkcl_cmn_get_state,and input it into pkcl_state.
 *  -# If pkcl_state == #PKCL_STATE_INIT then
 *    -# Initialize the PKC-Driver with R_PKCD_Initialize, and output the result to drv_ret.
 *    -# Extract error code from drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and set into ret.
 *    -# If #PKCL_RET_OK == ret then
 *      -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_Initialize (void);

/**********************************************************************************************************************
* Function Name : R_PKCL_GetState
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLCommonAPIs
 * \brief       Get current PKC State.
 * \return
 *  - PKC state (For parameters see #e_pkcl_state_t) :  Current PKC State is returned.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - Get current PKC State.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# Return #s_pkcl_state
 * \callgraph
 *********************************************************************************************************************/
extern e_pkcl_state_t R_PKCL_GetState (void);

/**********************************************************************************************************************
* Function Name : R_PKCL_ChkPKCCBusy
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLCommonAPIs
 * \brief       Check if PKCC is running.
 * \return
 *  - #PKCL_RET_OK:                  PKCC has finished operation w/o any errors.
 *  - #PKCL_RET_PKCCERR:             Any PKCC errors occurred.
 *  - #PKCL_RET_PROCESSING:          PKCC is still running.
 *  - #PKCL_RET_TAMPERERR:           Tampering has been detected.
 *  - #PKCL_RET_INTERNALERR:         Internal processing failed.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - R_PKCD_ChkPKCCBusy
 * \par Precondition
 *  - None
 * \par Detailed description
 *  - After a program returns from the update APIs with async option, the PKCC is likely still running. <br>
 *    This API is called for checking it.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Check if PKCC is running with R_PKCD_ChkPKCCBusy, and output the result to drv_ret.
 *  -# Convert the result in drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_ChkPKCCBusy (void);

/**********************************************************************************************************************
* Function Name : R_PKCL_IsAsyncOpeProgress
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLCommonAPIs
 * \brief       Check if the last called API is asynchronous execution.
 * \return
 *  - PKC_BOOL_TRUE:               The last called PKC-Driver API ran in asynchronous mode
 *  - PKC_BOOL_FALSE:              The last called PKC-Driver API ran in synchronous mode
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - R_PKCD_IsAsyncOpeProgress
 * \par Precondition
 *  - None
 * \par Detailed description
 *   - Check whether the last called API was executed synchronously or asynchronously.
 *   - This API is intended for use within PKCC's interrupt service.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Check if async operation is processing with R_PKCD_IsAsyncOpeProgress, and input the result into is_async.
 *  -# Return is_async.
 * \callgraph
 *********************************************************************************************************************/
extern e_pkc_bool_t R_PKCL_IsAsyncOpeProgress (void);

/**********************************************************************************************************************
* Function Name : R_PKCL_ClearState
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLCommonAPIs
 * \brief       Clear PKC State.
 * \return
 *  - #PKCL_RET_OK:                  Successful completion
 *  - #PKCL_RET_STATUSERR:           PKC State was called with PKCL_STATE_INIT
 *  - #PKCL_RET_TAMPERERR:           Tampering has been detected.
 *  - #PKCL_RET_DRV_INTERNALERR:     An exceptional internal error occurred in PKC-Driver
 *  - #PKCL_RET_INTERNALERR:         Internal processing failed.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - R_PKCD_ClearState
 * \par Precondition
 *  - PKC State is other than #PKCL_STATE_INIT
 * \par Detailed description
 *  - Clear PKC State.
 * \par Remarks
 *  - It is necessary to call this API when stopping a series of calls of Init/Operate/Final function
 *    in the middle. <br>
 *    #PKCL_RET_STATUSERR is returned when the next API(*) is called without calling this API when a <br>
 *    series of calls is stopped in the middle. (*Excluding Common API)
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Get the PKC state with #r_pkcl_cmn_get_state, and input it into pkc_state.
 *  -# If #PKCL_STATE_INIT != pkcl_state then
 *    -# Clear the ongoing Init/Operate/Final operation eith R_PKCD_ClearState, and output the result to drv_ret.
 *    -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# If #PKCL_RET_OK == ret then
 *      -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_ClearState (void);

/**********************************************************************************************************************
* Function Name : R_PKCL_GetVersion
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLCommonAPIs
 * \brief       Get version information of PKC-Library.
 * \param       [out] p_major       An address of major version
 * \param       [out] p_minor       An address of minor version
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *      - Get version information of PKC-Library.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 * -# If (p_major != NULL) && (p_minor != NULL)
 *   -# Set #PKCL_VERSION_MAJOR to p_major
 *   -# Set #PKCL_VERSION_MINOR to p_minor
 *   -# Set ret to #PKCL_RET_OK
 * -# Else
 *   -# Set ret to #PKCL_RET_PARAMERR
 * -# Return ret
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_GetVersion (uint32_t * const p_major,
                                    uint32_t * const p_minor);



/**********************************************************************************************************************
* Function Name : R_PKCL_RsaEncryptInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Performs the initial processing of RSA encryption.
 * \param       [in]     p_plain       A structure consisting of plaintext byte length len and data pointer p_data.
 * \param       [in]     p_pub_key     A pointer to an RSA private or public key in binary format.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaEncryptInit(When #PKCL_CFG_ENABLE_DRV_RSA_ENC == 1)
 *  - R_PKCD_RsaModExpInit
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Prepare for RSA encryption.
 *      - Encode according to the p_ctx-> padding_option method. \n
 *        If #PKC_RSA_PADDING_NO is selected, no encoding is performed. \n
 *        When #PKC_RSA_PADDING_NO is selected and the size of the message is smaller than the key length,
 *         the upper part of the message is padded with 0 and processing is performed according to the key length.
 *      - If p_ctx-> padding_option is #PKC_RSA_PADDING_PKCS1_OAEP, use external Hash API and DRBG API. \n
 *        When using HW in the API of the external library, it is necessary to secure HW resources on the user side.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_RSA_ENC is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Get the PKC state with #r_pkcl_cmn_get_state, and input the result into pkcl_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# Check the arguments p_plain, p_pub_key, mod_len, p_ctx with #r_pkcl_rsa_chk_enc_init_arg,
 *       and input result into ret.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the RSA encrytion operation as p_rsa_enc from p_ctx->p_buf.
 *      -# Save modulus length mod_len into p_rsa_enc->mod_len.
 *      -# Assign a buffer for the driver's RSA encrytion operation as p_buf_drv from p_ctx->p_buf.
 *      -# Assign a buffer for the em data of the PKCS\#1 RSA scheme. And set its length to mod_len.
 *      -# If #PKC_RSA_PADDING_NO == p_ctx->padding_option then,
 *        -# Encode the em data with #r_pkcl_rsa_padding_no.
 *      -# Else if #PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option then,
 *        -# Encode the em data with #r_pkcl_rsa_enc_scheme_pkcs1, and output the result to ret.
 *      -# Else
 *        -# Encode the em data with #r_pkcl_rsa_enc_scheme_oaep, and input he result into ret.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Encrypt the em data with R_PKCD_RsaModExpInit using the given key,
 *         and output the result to drv_ret.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_RSA_ENC_INIT_DONE with #PKCL_STATE_RSA_ENC_INIT_DONE.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_RSA_ENC is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_RSA_ENC is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaEncryptInit (const st_pkc_data_t * const p_plain,
                                        const void * const p_pub_key,
                                        const uint32_t mod_len,
                                        st_pkc_rsa_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaEncryptOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Perform RSA cryptographic processing.
 * \param       [in,out] p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_operate_opt    Operate option selection.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_PROCESSING:               Successful completion (With unprocessed bits)
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaEncryptOperate(When #PKCL_CFG_ENABLE_DRV_RSA_ENC == 1)
 *  - R_PKCD_RsaModExpOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_RSA_ENC_INIT_DONE or #PKCL_STATE_RSA_ENC_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Perform RSA cryptographic processing.
 *      - The operation depends on the bit length that HW can process at one time. \n
 *        If it is an HW (RH850, etc.) that can support up to 2048 bits, the operation will be as follows: \n
 *        (1) When the key length is 2048 bits or less \n
 *            The power modulo operation of the driver is used once to perform the power remainder. \n
 *            p_operate_opt-> sync_opt to select the synchronization method. \n
 *            #PKC_SYNC_OPT_SYNC: Processing cannot be interrupted during PKCC operation \n
 *            #PKC_SYNC_OPT_ASYNC: Processing can be interrupted during PKCC operation \n
 *        (2) When the key length is larger than 2048bit \n
 *            Use the binary method to perform the power remainder that should be divided into multiple times. \n
 *            p_operate_opt-> processing bit indicates the number of bits to be processed at one time.
 *      - If the bit length that can be processed at one time is 4096 bits for HW (RH850 / U2B, etc.),
 *         process only with (1).
 * \par Remarks
 *  - If the key length is larger than the bit length that can be processed by HW at one time,
 *     it is necessary to call this API repeatedly until all the bits are processed. \n
 *    If unprocessed bits remain, #PKCL_RET_PROCESSING is returned, and if all bits are processed,
 *     #PKCL_RET_OK is returned. \n
 *    #PKCL_RET_STATUSERR is returned when the next Final function is called with unprocessed bits remaining.
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_RSA_ENC is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Get the PKC state with #r_pkcl_cmn_get_state and input it to pkcl_state.
 *  -# If (#PKCL_STATE_RSA_ENC_INIT_DONE == pkcl_state) || (#PKCL_STATE_RSA_ENC_OPERATE_PROCESSING == pkcl_state) then,
 *    -# Check the arguments p_ctx, p_operate_opt with #r_pkcl_rsa_chk_enc_operate_arg, and output the result to ret.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the driver's RSA operation as p_buf_drv from p_ctx->p_buf.
 *      -# Execute the modular exponentiation operation with R_PKCD_RsaModExpOperate, and output the result to drv_ret.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_RSA_ENC_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# Else if #PKCL_RET_PROCESSING == ret then,
 *    -# Set the PKC state to #PKCL_STATE_RSA_ENC_OPERATE_PROCESSING with #r_pkcl_cmn_set_state.
 *  -# Else if (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_RSA_ENC is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_RSA_ENC is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaEncryptOperate (st_pkc_rsa_ctx_t * const p_ctx,
                                            const st_pkc_operate_opt_t * const p_operate_opt);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaEncryptFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Output the encrypted result and its byte length.
 * \param       [in]     p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in,out] p_cipher         Output ciphertext data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaEncryptFinal(When #PKCL_CFG_ENABLE_DRV_RSA_ENC == 1)
 *  - R_PKCD_RsaModExpFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_RSA_ENC_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Call R_PKCD_RsaModExpFinal to output the encrypted result and its byte length.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_RSA_ENC is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Get the PKC state with #r_pkcl_cmn_get_state and input the result into pkcl_state.
 *  -# If #PKCL_STATE_RSA_ENC_OPERATE_DONE == pkcl_state then,
 *    -# Check the arguments p_ctx, p_cipher with #r_pkcl_rsa_chk_enc_final_arg, and output the result to ret.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for driver's RSA operation as p_buf_drv from p_ctx.
 *      -# Finalize the RSA modular exponentiation operation with R_PKCD_RsaModExpFinal, <br>
 *         and output the result to drv_ret.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_RSA_ENC is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_RSA_ENC is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaEncryptFinal (const st_pkc_rsa_ctx_t * const p_ctx,
                                            st_pkc_data_t * const p_cipher);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaDecryptInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Performs the initial processing of RSA decryption.
 * \param       [in]     p_cipher      A structure consisting of the ciphertext byte length len and the data pointer
 *                                      p_data.
 * \param       [in]     p_priv_key    A pointer to an RSA private key in key binary format.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaDecryptInit(When #PKCL_CFG_ENABLE_DRV_RSA_DEC == 1)
 *  - R_PKCD_RsaModExpInit
 *  - R_PKCD_RsaModExpCRTInit
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the parameters required for RSA decryption.
 *      - During RSA operation, the operation of PKC-Lib API depends on the bit length that HW can process at one time.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_RSA_DEC is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Get the PKC state with #r_pkcl_cmn_get_state, and input it to pkcl_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# Check the arguments p_cipher, p_priv_key, mod_len, p_ctx with #r_pkcl_rsa_chk_dec_init_arg.
 *    -# If #PKCL_RET_OK == ret the,
 *      -# Assign a buffer for the RSA operation as p_rsa_dec from p_ctx->p_buf.
 *      -# Save the modulus length mod_len into p_rsa_dec->mod_len.
 *      -# Assign a buffer for the driver's RSA operation as p_buf_drv.
 *      -# If #PKC_BOOL_TRUE == p_ctx->is_crt_mode then,
 *        -# Initialize RSA CRT modular exponetation operation with R_PKCD_RsaModExpCRTInit,
 *           and output the result to drv_ret.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# Else
 *        -# Initialize RSA modular exponentiation operation using R_PKCD_RsaModExpInit,
 *           and output the result to drv_ret.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_RSA_DEC_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_RSA_DEC is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_RSA_DEC is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaDecryptInit (const st_pkc_data_t * const p_cipher,
                                        const void * const p_priv_key,
                                        const uint32_t mod_len,
                                        st_pkc_rsa_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaDecryptOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Perform RSA decryption processing.
 * \param       [in,out] p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_operate_opt    Operate option selection.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_PROCESSING:               Successful completion (Decryption process in progress)
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaDecryptOperate(When #PKCL_CFG_ENABLE_DRV_RSA_DEC == 1)
 *  - R_PKCD_RsaModExpOperate
 *  - R_PKCD_RsaModExpCRTOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_RSA_DEC_INIT_DONE or #PKCL_STATE_RSA_DEC_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Perform RSA cryptographic processing.
 *      - The operation depends on the presence or absence of CRT and the bit length that HW can process at one time.\n
 *        If p_ctx-> is_crt_mode is TRUE, use the CRT algorithm to perform exponentiation operation. \n
 *        When p_ctx-> is_crt_mode is FALSE, if the HW (RH850, etc.) that can support up to 2048 bits,
 *         the operation will be as follows. \n
 *        (1) When the key length is 2048 bits or less \n
 *            The power modulo operation of the driver is used once to perform the power remainder. \n
 *            p_operate_opt-> sync_opt to select the synchronization method. \n
 *            #PKC_SYNC_OPT_SYNC: Processing cannot be interrupted during PKCC operation \n
 *            #PKC_SYNC_OPT_ASYNC: Processing can be interrupted during PKCC operation \n
 *        (2) When the key length is larger than 2048bit \n
 *            Use the binary method to perform the power remainder that should be divided into multiple times. \n
 *            p_operate_opt-> processing bit indicates the number of bits to be processed at one time.
 *      - If the bit length that can be processed at one time is 4096 bits for HW (RH850 / U2B, etc.),
 *         process only with (1).
 * \par Remarks
 *  - &lt; When performing exponentiation calculation using CRT &gt; \n
 *    This API needs to be called twice \n
 *    PKCL_RET_STATUSERR is returned when the next Final function is called with only one call.
 *  - &lt; When performing normal exponentiation calculation &gt; \n
 *    If the key length is larger than the bit length that can be processed by HW at one time,
 *     it is necessary to call this API repeatedly until all the bits are processed. \n
 *    If unprocessed bits remain, #PKCL_RET_PROCESSING is returned, and if all bits are processed,
 *     #PKCL_RET_OK is returned. \n
 *    #PKCL_RET_STATUSERR is returned when the next Final function is called with unprocessed bits remaining.
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_RSA_DEC is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Get current PKC state with #r_pkcl_cmn_get_state and input it into pkcl_state.
 *  -# If (#PKCL_STATE_RSA_DEC_INIT_DONE == pkcl_state) || (#PKCL_STATE_RSA_DEC_OPERATE_PROCESSING == pkcl_state) then,
 *    -# Check the arguments p_ctx, p_operate_opt with #r_pkcl_rsa_chk_dec_operate_arg and output the result to ret.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the driver's RSA operation as p_buf_drv from p_ctx->p_buf.
 *      -# If #PKC_BOOL_TRUE == p_ctx->is_crt_mode then,
 *        -# Execute RSA CRT modular exponentiation with R_PKCD_RsaModExpCRTOperate,
 *           and output the result to drv_ret.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# Else
 *        -# Execute RSA modular exponentiation with R_PKCD_RsaModExpOperate,
 *           and output the result to drv_ret.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *   -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_RSA_DEC is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_RSA_DEC is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaDecryptOperate (st_pkc_rsa_ctx_t * const p_ctx,
                                            const st_pkc_operate_opt_t * const p_operate_opt);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaDecryptFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Outputs the decrypted result and its byte length.
 * \param       [in]     p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in,out] p_plain          Output plaintext data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaDecryptFinal(When #PKCL_CFG_ENABLE_DRV_RSA_DEC == 1)
 *  - R_PKCD_RsaModExpFinal
 *  - R_PKCD_RsaModExpCRTFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_RSA_DEC_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Decode according to the setting of p_ctx-> padding_option. \n
 *        If the set value is PKC_RSA_PADDING_NO, decoding is not performed.
 *      - Output the decrypted result and its byte length.
 *      - If p_ctx-> padding_option is #PKC_RSA_PADDING_PKCS1_OAEP, use the external Hash API. \n
 *        When using HW in the API of the external library, it is necessary to secure HW resources on the user side.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_RSA_DEC is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Input current PKC state into pkcl_ret using #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_RSA_DEC_OPERATE_DONE == pkcl_state then,
 *    -# Check the arguments p_ctx, p_plain with #r_pkcl_rsa_chk_dec_final_arg.
 *    -# Assign a buffer for the RSA operation as p_rsa_dec from p_ctx->p_buf area.
 *    -# Set em_data.p_data to em_val as PKCS\#1 em data.
 *    -# Set em_data.len to p_rsa_dec->mod_len.
 *    -# Assign a buffer for the driver's RSA operation as p_buf_drv from p_ctx->p_buf area.
 *    -# If #PKC_BOOL_TRUE == p_ctx->is_crt_mode then,
 *      -# Finalize RSA CRT modular exponentiation with R_PKCD_RsaModExpCRTFinal.
 *         And output the result to drv_ret.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# Else
 *      -# Finalize RSA modular exponentiation with R_PKCD_RsaModExpFinal, and output the result to drv_ret.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# #PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option then,
 *        -# Decode the decrypted data with #r_pkcl_rsa_dec_scheme_pkcs1, and output the result to ret.
 *           The plain text and its length are output to the temporary buffer plain_temp_data.p_data
 *           and plain_temp_data.len respectively.
 *      -# Else if #PKC_RSA_PADDING_PKCS1_OAEP == p_ctx->padding_option then,
 *        -# Decode the recrypted data with #r_pkcl_rsa_dec_scheme_oaep, and output the result to ret.
 *           The plain text and its length are output to the temporary buffer plain_temp_data.p_data
 *           and plain_temp_data.len respectively.
 *      -# Else
 *        -# Copy the resulting plain text to the temporary buffer p_plain_temp.p_data with #r_pkcl_cmn_cpy_byte.
 *        -# Set p_plain_temp.len to the resulting plain text length.
 *      -# If #PKCL_RET_OK == ret
 *        -# If #PKC_BOOL_FALSE != p_ctx->verified_gen_plain
 *          -# Check if the plain text in p_plain_text.p_data and the cipher with #r_pkcl_rsa_verified_gen_plain
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION using #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_RSA_DEC is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_RSA_DEC is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaDecryptFinal (const st_pkc_rsa_ctx_t * const p_ctx,
                                            st_pkc_data_t* const p_plain);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaSignInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Performs the initial processing of RSA signature generation.
 * \param       [in]     p_digest      A structure consisting of the byte length len of the message digest and
 *                                      the data pointer p_data.
 * \param       [in]     p_priv_key    A pointer to an RSA private key in key binary format.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaSignInit(When #PKCL_CFG_ENABLE_DRV_RSA_SIGN == 1)
 *  - R_PKCD_RsaModExpInit
 *  - R_PKCD_RsaModExpCRTInit
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the parameters required for RSA signature generation.
 *      - Encode according to the p_ctx-> padding_option method. \n
 *        If #PKC_RSA_PADDING_NO is selected, no encoding is performed. \n
 *        When #PKC_RSA_PADDING_NO is selected and the size of the message digest is smaller than the key length,
 *         the upper part of the message digest is padded with 0 and processing is performed according to the
 *         key length.
 *      - If p_ctx-> padding_option is #PKC_RSA_PADDING_PKCS1, use the external Hash API. \n
 *        If p_ctx-> padding_option is #PKC_RSA_PADDING_PKCS1_PSS, use external Hash API and DRBG API. \n
 *        When using HW in the API of the external library, it is necessary to secure HW resources on the user side.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_RSA_SIGN is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Input current PKC state into pkcl_ret using #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# Check the arguments p_digest, p_priv_key, mod_len, p_ctx using #r_pkcl_rsa_chk_sign_init_arg.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the RSA operation as p_rsa_sign from p_ctx->p_buf area.
 *      -# Set p_rsa_sign->mod_len to mod_len.
 *      -# Assign a buffer for the driver's RSA operation as p_buf_drv from p_ctx->p_buf area.
 *      -# Set em_data.p_data to em_val as PKCS\#1 em data.
 *      -# Set em_data.len to p_rsa_dec->mod_len.
 *      -# If #PKC_RSA_PADDING_NO == p_ctx->padding_option then,
 *        -# Pad higher bytes with 0x00s using #r_pkcl_rsa_padding_no.
 *        -# If #PKC_BOOL_TRUE == p_ctx->is_crt_mode then,
 *          -# Initialize RSA CRT modular exponentiation operation with R_PKCD_RsaModExpCRTInit.
 *             And output the result to drv_ret.
 *          -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *        -# Else
 *          -# Initialize RSA modular exponentiation operation with #.R_PKCD_RsaModExpInit.
 *             And output the result to drv_ret.
 *          -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# Else
 *        -# If #PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option then,
 *          -# Encode the message with #r_pkcl_rsa_sign_scheme_pkcs1, and output the result to ret.
 *             The encoded message is output to em_data.
 *        -# Else
 *          -# Encode the message with #r_pkcl_rsa_sign_scheme_pss, and output the result to ret.
 *             The encoded message is output to em_data.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# #PKC_BOOL_TRUE == p_ctx->is_crt_mode then,
 *            -# Initialize RSA CRT modular exponentiation operation with R_PKCD_RsaModExpCRTInit.
 *               And output the result to drv_ret.
 *            -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *          -# Else
 *            -# Initialize RSA modular exponentiation operation with R_PKCD_RsaModExpInit.
 *               And output the result to drv_ret.
 *            -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_RSA_SIGN_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_RSA_SIGN is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_RSA_SIGN is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaSignInit (const st_pkc_data_t * const p_digest,
                                        const void * const p_priv_key,
                                        const uint32_t mod_len,
                                        st_pkc_rsa_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaSignOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Generate RSA signature.
 * \param       [in,out] p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_operate_opt    Operate option selection.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_PROCESSING:               Successful completion (Signature generation process in progress)
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaSignOperate(When #PKCL_CFG_ENABLE_DRV_RSA_SIGN == 1)
 *  - R_PKCD_RsaModExpOperate
 *  - R_PKCD_RsaModExpCRTOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_RSA_SIGN_INIT_DONE or #PKCL_STATE_RSA_SIGN_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Generate RSA signature.
 *      - The operation depends on the presence or absence of CRT and the bit length that HW can process at one time.\n
 *        If p_ctx-> is_crt_mode is TRUE, use the CRT algorithm to perform exponentiation operation. \n
 *        When p_ctx-> is_crt_mode is FALSE, if the HW (RH850, etc.) that can support up to 2048 bits,
 *         the operation will be as follows. \n
 *        (1) When the key length is 2048 bits or less \n
 *            The power modulo operation of the driver is used once to perform the power remainder. \n
 *            p_operate_opt-> sync_opt to select the synchronization method. \n
 *            #PKC_SYNC_OPT_SYNC: Processing cannot be interrupted during PKCC operation \n
 *            #PKC_SYNC_OPT_ASYNC: Processing can be interrupted during PKCC operation \n
 *        (2) When the key length is larger than 2048bit \n
 *            Use the binary method to perform the power remainder that should be divided into multiple times. \n
 *            p_operate_opt-> processing bit indicates the number of bits to be processed at one time.
 *      - If the bit length that can be processed at one time is 4096 bits for HW (RH850 / U2B, etc.),
 *         process only with (1).
 * \par Remarks
 *  - &lt; When performing exponentiation calculation using CRT &gt; \n
 *    This API needs to be called twice \n
 *    PKCL_RET_STATUSERR is returned when the next Final function is called with only one call.
 *  - &lt; When performing normal exponentiation calculation &gt; \n
 *    If the key length is larger than the bit length that can be processed by HW at one time,
 *     it is necessary to call this API repeatedly until all the bits are processed. \n
 *    If unprocessed bits remain, #PKCL_RET_PROCESSING is returned, and if all bits are processed,
 *     #PKCL_RET_OK is returned. \n
 *    #PKCL_RET_STATUSERR is returned when the next Final function is called with unprocessed bits remaining.
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_RSA_SIGN is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Input current PKC state into pkcl_state with #r_pkcl_cmn_get_state.
 *  -# If (#PKCL_STATE_RSA_SIGN_INIT_DONE == pkcl_state) or
 *        (#PKCL_STATE_RSA_SIGN_OPERATE_PROCESSING == pkcl_state) then,
 *    -# Check the arguments p_ctx, p_operate_opt with #r_pkcl_rsa_chk_sign_operate_arg.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the RSA operation as p_buf_drv from p_ctx->p_buf area.
 *      -# If #PKC_BOOL_TRUE == p_ctx->is_crt_mode then,
 *        -# Operate RSA CRT modular exponentiation with R_PKCD_RsaModExpCRTOperate.
 *           And output the result to drv_ret.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# Else
 *        -# Operate RSA modular exponentiation with R_PKCD_RsaModExpOperate.
 *           And output the result to drv_ret.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_RSA_SIGN_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# Else if #PKCL_RET_PROCESSING == ret then,
 *    -# Set the PKC state to #PKCL_STATE_RSA_SIGN_OPERATE_PROCESSING with #r_pkcl_cmn_set_state.
 *  -# Else if (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_RSA_SIGN is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_RSA_SIGN is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaSignOperate (st_pkc_rsa_ctx_t * const p_ctx,
                                        const st_pkc_operate_opt_t * const p_operate_opt);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaSignFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       The result of signature generation and its byte length are output.
 * \param       [in]     p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in,out] p_sign           A structure consisting of the byte length len of the data concatenated
 *                                         with the signature values R and S and the data pointer p_data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaSignFinal(When #PKCL_CFG_ENABLE_DRV_RSA_SIGN == 1)
 *  - R_PKCD_RsaModExpFinal
 *  - R_PKCD_RsaModExpCRTFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_RSA_SIGN_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - The result of signature generation and its byte length are output.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_RSA_SIGN is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Input current PKC state to pkcl_state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_RSA_SIGN_OPERATE_DONE == pkcl_state then,
 *    -# Check the argument p_ctx, p_sign with #r_pkcl_rsa_chk_sign_final_arg, and output the result to ret.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the RSA operation as p_buf_drv from p_ctx->p_buf area.
 *      -# If #PKC_BOOL_TRUE == p_ctx->is_crt_mode then,
 *        -# Finalize RSA CRT modular exponentiation operation with R_PKCD_RsaModExpCRTFinal.
 *           And output the result to drv_ret.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# Else
 *        -# Finalize RSA modular exponentiation operation with R_PKCD_RsaModExpFinal.
 *           And output the result to drv_ret.
 *        -# Set the buffer sign_temp_data to the generated signature and its byte length.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret, then
 *        -# If #PKC_BOOL_FALSE != p_ctx->verified_gen_sign, then
 *          -# Verify the generated signature with #r_pkcl_rsa_verified_gen_sign, and ret to set the result.
 *        -# If #PKCL_RET_OK == ret, then
 *          -# Copy the signature in sign_temp_data.p_data to p_sign->p_data with #r_pkcl_cmn_cpy_byte.
 *          -# Set p_sign->len to sign_temp_data.len.
 *        -# Clear the buffer sign_temp_data.p_data.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_RSA_SIGN is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_RSA_SIGN is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaSignFinal (const st_pkc_rsa_ctx_t * const p_ctx,
                                        st_pkc_data_t * const p_sign);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaVerifyInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Performs the initial processing of RSA signature verification.
 * \param       [in]     p_digest      A structure consisting of the byte length len of the message digest and the
 *                                      data pointer p_data.
 * \param       [in]     p_sign        A structure consisting of the byte length len of the signature to be verified
 *                                      and the data pointer p_data.
 * \param       [in]     p_pub_key     Key A pointer to an RSA private or public key in binary format.
 * \param       [in]     mod_len       Byte length of modulus n.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by RSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaVerifyInit(When #PKCL_CFG_ENABLE_DRV_RSA_VERIFY == 1)
 *  - R_PKCD_RsaModExpInit
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the parameters required for RSA signature verification.
 *      - Encode according to the p_ctx-> padding_option method. \n
 *        If #PKC_RSA_PADDING_NO is selected, no encoding is performed. \n
 *        When #PKC_RSA_PADDING_NO is selected and the size of the message is smaller than the key length,
 *         the upper part of the message is padded with 0 and processing is performed according to the key length.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_RSA_VERIFY is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Input current PKC state to pkcl_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# Check the arguments p_digest, p_sign, p_pub_key, mod_len, p_ctx with #r_pkcl_rsa_chk_verify_init_arg.
 *       And output the result to ret.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the RSA operation as p_rsa_verify from p_ctx->p_buf.
 *      -# Assign a buffer for the driver's RSA operation as p_rsa_verify from p_ctx->p_buf.
 *      -# Set p_rsa_verify->mod_len to mod_len.
 *      -# If #PKC_RSA_PADDING_NO == p_ctx->padding_option then,
 *        -# Set em_data.p_data to em_val as PKSC\#1 em data.
 *        -# Set em_data.len to mod_len.
 *        -# Pad higher bytes in em data with 0x00s using #r_pkcl_rsa_padding_no. And output the result to ret.
 *        -# Set p_rsa_verify->digest_len to em_data.len.
 *        -# Copy em data into p_rsa_verify->digest_data with #r_pkcl_cmn_cpy_byte.
 *      -# Else
 *        -# Set p_rsa_verify->digest_len to p_digest->len.
 *        -# Copy em data into p_rsa_verify->digest_data with #r_pkcl_cmn_cpy_byte.
 *      -# Initialize RSA modular exponentiation operation with R_PKCD_RsaModExpInit.
 *         And output the result into drv_ret.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_RSA_VERIFY_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_RSA_VERIFY is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_RSA_VERIFY is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaVerifyInit (const st_pkc_data_t * const p_digest,
                                        const st_pkc_data_t * const p_sign,
                                        const void * const p_pub_key,
                                        const uint32_t mod_len,
                                        st_pkc_rsa_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaVerifyOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Perform RSA signature verification.
 * \param       [in,out] p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_operate_opt    Operate option selection.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_PROCESSING:               Successful completion (Decryption process in progress)
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaVerifyOperate(When #PKCL_CFG_ENABLE_DRV_RSA_VERIFY == 1)
 *  - R_PKCD_RsaModExpOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_RSA_VERIFY_INIT_DONE or #PKCL_STATE_RSA_VERIFY_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Perform RSA signature verification.
 *      - The operation depends on the bit length that HW can process at one time. \n
 *        If it is an HW (RH850, etc.) that can support up to 2048 bits, the operation will be as follows: \n
 *        (1) When the key length is 2048 bits or less \n
 *            The power modulo operation of the driver is used once to perform the power remainder. \n
 *            p_operate_opt-> sync_opt to select the synchronization method. \n
 *            #PKC_SYNC_OPT_SYNC: Processing cannot be interrupted during PKCC operation \n
 *            #PKC_SYNC_OPT_ASYNC: Processing can be interrupted during PKCC operation \n
 *        (2) When the key length is larger than 2048bit \n
 *            Use the binary method to perform the power remainder that should be divided into multiple times. \n
 *            p_operate_opt-> processing bit indicates the number of bits to be processed at one time.
 *      - If the bit length that can be processed at one time is 4096 bits for HW (RH850 / U2B, etc.),
 *         process only with (1).
 * \par Remarks
 *  - If the key length is larger than the bit length that can be processed by HW at one time,
 *     it is necessary to call this API repeatedly until all the bits are processed. \n
 *    If unprocessed bits remain, #PKCL_RET_PROCESSING is returned, and if all bits are processed,
 *     #PKCL_RET_OK is returned. \n
 *    #PKCL_RET_STATUSERR is returned when the next Final function is called with unprocessed bits remaining.
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_RSA_VERIFY is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Input current PKC state to pkcl_state with #r_pkcl_cmn_get_state.
 *  -# If (#PKCL_STATE_RSA_VERIFY_INIT_DONE == pkcl_state) ||
 *        (#PKCL_STATE_RSA_VERIFY_OPERATE_PROCESSING == pkcl_state) then,
 *    -# Check the arguments with #r_pkcl_rsa_chk_verify_operate_arg, p_operate_opt and output the result into ret.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for driver's RSA modular exponentiation operation as p_buf_drv from p_ctx->p_buf area.
 *      -# Operate RSA modular exponentiation with R_PKCD_RsaModExpOperate and output the result to drv_ret.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_RSA_VERIFY_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# Else if #PKCL_RET_PROCESSING == ret then,
 *    -# Set the PKC state to #PKCL_STATE_RSA_VERIFY_OPERATE_PROCESSING with #r_pkcl_cmn_set_state.
 *  -# Else if (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_RSA_VERIFY is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_RSA_VERIFY is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaVerifyOperate (st_pkc_rsa_ctx_t * const p_ctx,
                                            const st_pkc_operate_opt_t * const p_operate_opt);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaVerifyFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Output the verification result.
 * \param       [in]     p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_RSA_VERIFAIL:             Signature verification failed
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaVerifyFinal(When #PKCL_CFG_ENABLE_DRV_RSA_VERIFY == 1)
 *  - R_PKCD_RsaModExpFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_RSA_VERIFY_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Perform verification processing according to the setting of p_ctx->padding_option.
 *      - Output the verification result.
 *      - If p_ctx->padding_option is #PKC_RSA_PADDING_PKCS1, or #PKC_RSA_PADDING_PKCS1_PSS,
 *         use the external Hash API. \n
 *        When using HW in the API of the external library, it is necessary to secure HW resources on the user side.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_RSA_VERIFY is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Input current PKC state into pkcl_state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_RSA_VERIFY_OPERATE_DONE == pkcl_state then,
 *    -# Check the argument p_ctx with #r_pkcl_rsa_chk_verify_final_arg and output the result to ret.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the driver's RSA operation as p_buf_drv from p_ctx->p_buf area.
 *      -# Set em_data.p_data to em_val as PKSC\#1 em data.
 *      -# Set em_data.len to mod_len.
 *      -# Finalize RSA modular exponentiation operation with R_PKCD_RsaModExpFinal.
 *         And output the result to drv_ret.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Set digest_data.len to p_rsa_verify->digest_len.
 *        -# Set digest_data.p_data = p_rsa_verify->digest_data.
 *        -# If #PKC_RSA_PADDING_PKCS1 == p_ctx->padding_option then,
 *          -# Verify given signature with #r_pkcl_rsa_verify_scheme_pkcs1 and output the result to drv_ret.
 *        -# Else if #PKC_RSA_PADDING_PKCS1_PSS == p_ctx->padding_option then,
 *          -# Verify given signature with #r_pkcl_rsa_verify_scheme_pss and output the result to drv_ret.
 *        -# Else
 *          -# Set ret to 0 for double check.
 *          -# Compare decrypted digest em_data.p_data with digest_data.p_data, and set cmp_ret to the result.
 *          -# If #PKC_BOOL_TRUE == cmp_ret, then
 *            -# Add #PKCL_CMN_RET_OK_HALF_1 to ret.
 *            -# Compare decrypted digest em_data.p_data with digest_data.p_data, and set cmp_ret to the result.
 *            -# If #PKC_BOOL_TRUE == cmp_ret, then
 *              -# Add #PKCL_CMN_RET_OK_HALF_2 to ret.
 *            -# Else
 *              -# Set ret to #PKCL_RET_RSA_VERIFAIL.
 *          -# Else
 *            -# Set ret to #PKCL_RET_RSA_VERIFAIL.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_RSA_VERIFY is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_RSA_VERIFY is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaVerifyFinal (const st_pkc_rsa_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaKeyGenInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Performs the initial processing of RSA key generation.
 * \param       [in,out] p_ctx           A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     key_len         Byte length of the generated RSA key.
 * \param       [in]     prime_test_num  Upper limit of the number of prime number generation failures.
 * \param       [in]     iteration_num   Number of trials of Miller-Rabin test used for primality test.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaKeyGenInit
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *  - Prepare the parameters required for RSA key generation.
 *  - The Miller-Rabin test is used for the primality test performed during the RSA key generation process. \n
 *    Since the Miller-Rabin test is a probabilistic algorithm, incorrect results may be output. \n
 *    To reduce the chance of getting false results, you need to increase the number of trials.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Input current PKC state to pkcl_state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# If NULL != p_ctx then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api) then,
 *        -# If (#PKCL_RSA_KEY_LEN_1024 == key_len) || (#PKCL_RSA_KEY_LEN_2048 == key_len)
 *           || (#PKCL_RSA_KEY_LEN_3072 == key_len) || (#PKCL_RSA_KEY_LEN_4096 == key_len) then,
 *          -# Do nothing.
 *        -# Else
 *          -# Set ret to #PKCL_RET_LENGTHERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# If (#PKCL_RSA_PRIME_TEST_NUM_MIN <= prime_test_num) && (#PKCL_RSA_PRIME_TEST_NUM_MAX >= prime_test_num) then,
 *      -# If (#PKCL_RSA_ITERATION_NUM_MIN <= iteration_num) && (#PKCL_RSA_ITERATION_NUM_MAX >= iteration_num) then,
 *        -# Set ret to #PKCL_RET_OK.
 *      -# Else
 *        -# Set ret to #PKCL_RET_LENGTHERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_LENGTHERR.
 *  -# Else
 *    -# Do nothing.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Assign a buffer for the RSA operation with p_rsa_keygen from p_ctx->p_buf.
 *    -# Prepare RSA key generation with R_PKCD_RsaKeyGenInit and set drv_ret to the result.
 *    -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# Set the PKC state to #PKCL_STATE_RSA_KEYGEN_INIT_DONE with #r_pkcl_cmn_set_state
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaKeyGenInit (st_pkc_rsa_ctx_t * const p_ctx,
                                        const uint32_t key_len,
                                        const uint32_t prime_test_num,
                                        const uint32_t iteration_num);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaKeyGenOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Generate prime pairs p and q.
 * \param       [in,out] p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \return
 *  - #PKCL_RET_OK:                     Successful completion
 *  - #PKCL_RET_PRIME_GENERATING:       Successful completion (Prime number generation in progress)
 *  - #PKCL_RET_INTERNALERR:            Internal processing failed
 *  - #PKCL_RET_STATUSERR:              Called in an invalid state.
 *  - #PKCL_RET_PARAMERR:               Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:              Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:           Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:          Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:        Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:         PKCC timed out
 *  - #PKCL_RET_PKCCERR:                An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:              Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaKeyGenOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_RSA_KEYGEN_INIT_DONE or #PKCL_STATE_RSA_KEYGEN_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Continue generating prime number candidate data with the function specified in the DRBG API pointer of p_ctx
 *        until prime numbers p and q are obtained. And derive an RSA key pair from the prime numbers.
 *      - the public exponent e is fixed to 0x010001(65537).
 *      - The key generation process is performed following FIPS186-4 appendix B.3.1.
 * \par Remarks
 *  - When the DRBG API uses HW resources, the user must take care of its conflict.
 *  - If the key length is larger than the bit length that can be processed by HW at one time,
 *     it is necessary to call this API repeatedly until all the bits are processed. \n
 *    If unprocessed bits remain, #PKCL_RET_PRIME_GENERATING is returned, and if all bits are processed,
 *     #PKCL_RET_OK is returned. \n
 *    #PKCL_RET_STATUSERR is returned when the next Final function is called with unprocessed bits remaining.
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Input current PKC state into pkcl_state with #r_pkcl_cmn_get_state.
 *  -# If (#PKCL_STATE_RSA_KEYGEN_INIT_DONE == pkcl_state) ||
 *        (#PKCL_STATE_RSA_KEYGEN_OPERATE_PROCESSING == pkcl_state)
 *    -# If NULL != p_ctx then,
 *      -# If (NULL != p_ctx->p_buf) then,
 *        -# Set ret to #PKCL_RET_OK.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the driver's key generation operation as p_buf_drv from p_ctx->p_buf area.
 *      -# Generate an RSA key pair with R_PKCD_RsaKeyGenOperate and set the result to drv_ret.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# Else
 *      -# Do nothing.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_RSA_KEYGEN_OPERATE_DONE.
 *  -# Else if #PKCL_RET_PRIME_GENERATING == ret then,
 *    -# Set the PKC state to #PKCL_STATE_RSA_KEYGEN_OPERATE_PROCESSING.
 *  -# Else if (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaKeyGenOperate (st_pkc_rsa_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_RsaKeyGenFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLRSAAPIs
 * \brief       Output the generated RSA key.
 * \param       [in]     p_ctx            A pointer to a structure that summarizes the parameters required by RSA.
 * \param       [in]     p_priv_key       A pointer to a memory area allocated by the user SW to store the key binary
 *                                         of the RSA private key.
 * \param       [in]     p_pub_key        A pointer to a memory area allocated by the user SW to store the key binary
 *                                         of the RSA public key.
 * \return
 *  - #PKCL_RET_OK:                     Successful completion
 *  - #PKCL_RET_INTERNALERR:            Internal processing failed
 *  - #PKCL_RET_STATUSERR:              Called in an invalid PKC state.
 *  - #PKCL_RET_PARAMERR:               Invalid argument entered.
 *  - #PKCL_RET_LENGTHERR:              Invalid argument length entered.
 *  - #PKCL_RET_DRV_PARAMERR:           Invalid argument entered in PKC-Driver.
 *  - #PKCL_RET_DRV_LENGTHERR:          Invalid argument length entered in PKC-Driver.
 *  - #PKCL_RET_DRV_INTERNALERR:        Internal processing failed in PKC-Driver.
 *  - #PKCL_RET_TAMPERERR:              Tampering has been detected.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_RsaKeyGenFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_RSA_KEYGEN_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Call R_PKCD_RsaKeyGenFinal to output the generated RSA key.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Output current PKC state to pkcl_state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_RSA_KEYGEN_OPERATE_DONE == pkcl_state then,
 *    -# If (NULL != p_ctx) && (NULL != p_priv_key) && (NULL != p_pub_key) then,
 *      -# If (NULL != p_ctx->p_buf) then,
 *        -# Set ret to #PKCL_RET_OK.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the driver's key generation as p_buf_drv from p_ctx->p_buf area.
 *      -# Generate a wrapped RSA key parameters from those of raw ones generated with R_PKCD_RsaKeyGenOperate.
 *         And input the result to drv_ret.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# Else
 *      -# Do nothing.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_RsaKeyGenFinal (const st_pkc_rsa_ctx_t * const p_ctx,
                                        void * const p_priv_key,
                                        void * const p_pub_key);

/**********************************************************************************************************************
* Function Name : R_PKCL_DhInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLDHAPIs
 * \brief       Set the required parameters for DH.
 * \param       [in]     p_priv_key    A pointer to an DH private key in binary format.
 * \param       [in]     p_pub_key     A pointer to an DH public key in binary format.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by DH.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_STATUSERR:                Executed in PKC State different from expected
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_DhInit
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the required parameters for DH.
 *      - When performing KDF processing with DH, set the KDF function pointer in p_ctx. \n
 *        If KDF processing is not performed, set NULL in p_ctx-> p_kdf_info.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# Check the arguments with #r_pkcl_dh_chk_dh_init_arg, and set ret to the return value.
 *    -# If #PKCL_RET_OK == ret
 *      -# Assign a buffer for the driver's DH operation as p_buf_drv from p_ctx->p_buf.
 *      -# Initialize the DH operation with R_PKCD_DhInit and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# Else
 *      -# Do nothing
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_DH_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Do nothing
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_DhInit (const void * const p_priv_key,
                                const void * const p_pub_key,
                                st_pkc_dh_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_DhOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLDHAPIs
 * \brief       Perform DH key exchange.
 * \param       [in,out] p_ctx          A pointer to a structure that summarizes the parameters required by DH.
 * \param       [in]     p_operate_opt  Operate option selection.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_PROCESSING:               Successful completion (Decryption process in progress)
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_STATUSERR:                Executed in PKC State different from expected
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_DhOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKC State is #PKCL_STATE_DH_INIT_DONE or #PKCL_STATE_DH_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Generate a Shared Secret using the DH algorithm.
 *      - The operation depends on the bit length that HW can process at one time. \n
 *        If it is an HW (RH850, etc.) that can support up to 2048 bits, the operation will be as follows: \n
 *        (1) When the key length is 2048 bits or less \n
 *            The power modulo operation of the driver is used once to perform the power remainder. \n
 *            p_operate_opt-> sync_opt to select the synchronization method. \n
 *            #PKC_SYNC_OPT_SYNC: Processing cannot be interrupted during PKCC operation \n
 *            #PKC_SYNC_OPT_ASYNC: Processing can be interrupted during PKCC operation \n
 *        (2) When the key length is larger than 2048bit \n
 *            Use the binary method to perform the power remainder that should be divided into multiple times. \n
 *            p_operate_opt-> processing bit indicates the number of bits to be processed at one time.
 *      - If the bit length that can be processed at one time is 4096 bits for HW (RH850 / U2B, etc.),
 *         process only with (1).
 * \par Remarks
 *  - If the key length is larger than the bit length that can be processed by HW at one time,
 *     it is necessary to call this API repeatedly until all the bits are processed. \n
 *    If unprocessed bits remain, #PKCL_RET_PROCESSING is returned, and if all bits are processed,
 *     #PKCL_RET_OK is returned. \n
 *    #PKCL_RET_STATUSERR is returned when the next Final function is called with unprocessed bits remaining.
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Input current PKC state to pkcl_state with #r_pkcl_cmn_get_state.
 *  -# If (#PKCL_STATE_DH_INIT_DONE == pkcl_state) ||
 *        (#PKCL_STATE_DH_OPERATE_PROCESSING == pkcl_state) then,
 *    -# Check the arguments with #r_pkcl_dh_chk_dh_operate_arg, and set ret to the return value.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for driver's DH operation as p_buf_drv from p_ctx->p_buf area.
 *      -# Operate DH operation with R_PKCD_DhOperate and output the result to drv_ret.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# Else
 *      -# Do nothing
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_DH_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# Else if #PKCL_RET_PROCESSING == ret then,
 *    -# Set the PKC state to #PKCL_STATE_DH_OPERATE_PROCESSING with #r_pkcl_cmn_set_state.
 *  -# Else if (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Do nothing
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_DhOperate (st_pkc_dh_ctx_t * const p_ctx,
                                    const st_pkc_operate_opt_t * const p_operate_opt);

/**********************************************************************************************************************
* Function Name : R_PKCL_DhFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLDHAPIs
 * \brief       If KDF processing is not performed, the shared secret is output, and if it is performed,
 *               the common key is output.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by DH.
 * \param       [in,out] p_shared       A structure consisting of a byte length len and a data pointer p_data.
 *                                      The data pointed by p_data is a shared secret or a common key.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_STATUSERR:                Executed in PKC State different from expected
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_DhFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_DH_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Output the result of DH / DHE key sharing
 *      - If KDF processing is not performed, the shared secret is output, and if it is performed,
 *         the common key is output.
 *      - If p_ctx->p_kdf_info is non-null, it may use the external KDF API. \n
 *        When using HW in the API of the external library, it is necessary to secure HW resources on the user side.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_DH_OPERATE_DONE == pkcl_state then,
 *    -# Check the arguments with #r_pkcl_dh_chk_dh_final_arg, and set ret to the return value.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the DH operation as p_dh from p_ctx->p_buf.
 *      -# Set the shared secret address shared_data.p_data to p_dh->shared_data.
 *      -# Set the shared secret length shared_data.len to sizeof(p_dh->shared_data).
 *      -# Assign a buffer for the driver's DH operation as p_buf_drv from p_ctx->p_buf.
 *      -# Finalize the DH operation with R_PKCD_DhFinal and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# If NULL != p_ctx->p_kdf_info then,
 *          -# Derive ta shared key from the shared secret shared_data with p_ctx->p_kdf_info->p_kdf_api.
 *             And set kdf_ret to the result.
 *          -# If #PKCL_EXTERNAL_API_OK == kdf_ret then,
 *            -# Overwrite p_shared->len with the derived key length p_ctx->p_kdf_info->out_kdf_len.
 *          -# Else
 *            -# Set ret to #PKCL_RET_KDFAPIERR.
 *        -# Else
 *          -# Copy the shared secret to p_shared->p_data with #r_pkcl_cmn_cpy_byte.
 *          -# Set p_shared->len to curve_len.
 *        -# Use #r_pkcl_cmn_set_byte to clear shared secret results stored in p_buf.
 *      -# Else
 *        -# Do nothing
 *    -# Else
 *      -# Do nothing
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_DhFinal (const st_pkc_dh_ctx_t * const p_ctx,
                                    st_pkc_data_t * const p_shared);

/**********************************************************************************************************************
* Function Name : R_PKCL_DhKeyGenInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLDHAPIs
 * \brief       Set the parameters required for DH key pair generation.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by DH.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_STATUSERR:                Executed in PKC State different from expected
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_DhKeyGenInit
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the parameters required for DH key pair generation.
 *      - This API uses an external DRBG API. \n
 *        When using HW in the API of the external library, it is necessary to secure HW resources on the user side.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# Check the arguments with #r_pkcl_dh_chk_dh_keygen_init_arg, and set ret to the return value.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Initialize the DH key generation operation with R_PKCD_DhKeyGenInit and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# Else
 *  -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_DH_KEYGEN_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_DhKeyGenInit (st_pkc_dh_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_DhKeyGenOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLDHAPIs
 * \brief       Generate key pair.
 * \param       [in,out] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]     p_operate_opt  Operate option selection.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_PROCESSING:               Successful completion (Decryption process in progress)
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_STATUSERR:                Executed in PKC State different from expected
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_DhKeyGenOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_DH_KEYGEN_INIT_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Generate a random number to be passed to DriverAPI using the function specified in the DRBG function pointer
 *        of p_ctx.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Input current PKC state to pkcl_state with #r_pkcl_cmn_get_state.
 *  -# If (#PKCL_STATE_DH_KEYGEN_INIT_DONE == pkcl_state) ||
 *        (#PKCL_STATE_DH_KEYGEN_OPERATE_PROCESSING == pkcl_state) then,
 *    -# Check the arguments with #r_pkcl_dh_chk_dh_keygen_operate_arg, and set ret to the return value.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Operate DH key generation with R_PKCD_DhKeyGenOperate and output the result to drv_ret.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# Else
 *      -# Do nothing
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_DH_KEYGEN_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# Else if #PKCL_RET_PROCESSING == ret then,
 *    -# Set the PKC state to #PKCL_STATE_DH_KEYGEN_OPERATE_PROCESSING with #r_pkcl_cmn_set_state.
 *  -# Else if (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Do nothing
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_DhKeyGenOperate (st_pkc_dh_ctx_t * const p_ctx,
                                            const st_pkc_operate_opt_t * const p_operate_opt);

/**********************************************************************************************************************
* Function Name : R_PKCL_DhKeyGenFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLDHAPIs
 * \brief       Output the generated key pair.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by DH.
 * \param       [in,out] p_priv_key     A pointer to the memory area allocated by the user SW to store the key binary
 *                                       of the DH private key.
 * \param       [in,out] p_pub_key      A pointer to the memory area allocated by the user SW to store the key binary
 *                                       of the DH public key.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_STATUSERR:                Executed in PKC State different from expected
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_DhKeyGenFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_DH_KEYGEN_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Outputs the generated key pair in key binary format.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Input current PKC state to pkcl_state with #r_pkcl_cmn_get_state.
 *  -# If (#PKCL_STATE_DH_OPERATE_DONE == pkcl_state) then,
 *    -# Check the arguments with #r_pkcl_dh_chk_dh_keygen_final_arg, and set ret to the return value.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Operate DH key generation with R_PKCD_DhKeyGenFinal and output the result to drv_ret.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# Else
 *      -# Do nothing
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Do nothing
 *  -# Return ret.
 *
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_DhKeyGenFinal (const st_pkc_dh_ctx_t * const p_ctx,
                                        void * const                  p_priv_key,
                                        void * const                  p_pub_key);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdsaSignInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Performs the initial processing of ECDSA signature generation.
 * \param       [in]     p_digest      A structure consisting of the byte length len of the message digest and
 *                                      the data pointer p_data.
 * \param       [in]     p_priv_key    A pointer to an EC private key in binary format.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by EC.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdsaSignInit(When #PKCL_CFG_ENABLE_DRV_ECDSA_SIGN == 1)
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the parameters required for ECDSA signature generation.
 *      - Select whether to execute anti-tampering scalar multiplication function or not
 *        in the Operate function by the argument p_ctx->sec_scalar_mul.
 *      - Select whether to verify the resulting signature or not in the Final function
 *        by the argument p_ctx->verified_gen_sign.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_ECDSA_SIGN is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# If (NULL != p_digest) && (NULL != p_priv_key) && (NULL != p_ctx) then,
 *      -# Check if the members of the given digest structure are properly set with #r_pkcl_ec_chk_digest.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info) then,
 *          -# Check if the given curve structure is valid with #r_pkcl_ec_chk_curve.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EC operation as p_ec_sign from p_ctx->p_buf area.
 *      -# If #PKCL_EC_CURVE_LEN_MAX < p_digest->len then,
 *        -# Set p_ec_sign->digest_len to #PKCL_EC_CURVE_LEN_MAX.
 *      -# Else
 *        -# Set p_ec_sign->digest_len to p_digest->len.
 *      -# Copy the given digest to the buffer by the buffer size with #r_pkcl_cmn_cpy_byte.
 *      -# Set p_ec_sign->p_priv_key to p_priv_key.
 *      -# Set the current PKC state to #PKCL_STATE_EC_SIGN_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_ECDSA_SIGN is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_ECDSA_SIGN is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdsaSignInit (const st_pkc_data_t * const p_digest,
                                        const void * const p_priv_key, st_pkc_ec_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdsaSignOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Performs ECDSA signature generation.
 * \param       [in,out] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]     sync_opt       Select synchronous/asynchronous execution
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdsaSignOperate(When #PKCL_CFG_ENABLE_DRV_ECDSA_SIGN == 1)
 *  - R_PKCD_EcScalarMultInit
 *  - R_PKCD_EcScalarMultOperate
 *  - R_PKCD_EcPrivateScalarGen
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EC_SIGN_INIT_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Generate ECDSA signature
 *      - Generate a random number using the function specified in the DRBG function pointer of p_ctx, \n
 *        and generate the value of random k using the random number.
 *      - This API uses an external DRBG API. When using HW in the API of the external library, \n
 *        it is necessary to secure HW resources on the user side.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_ECDSA_SIGN is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EC_SIGN_INIT_DONE == pkcl_state then,
 *    -# If (NULL != p_ctx) && ((#PKC_SYNC_OPT_SYNC == sync_opt) || (#PKC_SYNC_OPT_ASYNC == sync_opt)) then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check if the given curve structure is valid with #r_pkcl_ec_chk_curve. And set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EC operation as p_ec_sign from p_ct->p_buf area.
 *      -# Set random_data.p_data to random_val.
 *      -# Set random_data.len to p_ctx->p_curve_info->curve_len + #PKCL_EC_PRIV_SCALAR_NUM;
 *      -# Generate random bytes for the secret random k with p_ctx->p_drbg_api.
 *         And set drbg_ret to the result.
 *      -# If #PKCL_EXTERNAL_API_OK == drbg_ret then,
 *        -# Set the private scalar address priv_scalar_data.p_data to p_ec_sign->priv_scalar_data.
 *        -# Set the private scalar length priv_scalar_data.len to sizeof(p_ec_sign->priv_scalar_data).
 *        -# Assign a buffer for the driver's EC operation as p_buf_drv from p_ctx->p_buf area.
 *        -# Manipulate the private scalar following NIST FIPS186-4 Appendix B.5.1 with R_PKCD_EcPrivateScalarGen.
 *           And set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *        -# Use #r_pkcl_cmn_set_byte to clear the random value used in signature generation stored on the stack.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Set priv_scalar_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *          -# Set priv_scalar_sel.p_sel_data to &priv_scalar_data.
 *          -# Copy the curve base point value from the given curve structure to base_point_val
 *             with #r_pkcl_cmn_cpy_byte.
 *          -# Set base_point_data.p_data to base_point_val.
 *          -# Set base_point_data.len to p_ctx->p_curve_info->curve_len * #PKCL_EC_DOUBLE_LEN.
 *          -# Set base_point_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *          -# Set base_point_sel.p_sel_data = &base_point_data.
 *          -# Initialize a scalar multiplication of the base point with R_PKCD_EcScalarMultInit.
 *             And set drv_ret to the result.
 *          -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *          -# If #PKCL_RET_OK == ret then,
 *            -# Execute the scalar multiplication of the base point with R_PKCD_EcScalarMultOperate.
 *               And set drv_ret to the result.
 *            -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# Else
 *        -# Set ret to #PKCL_RET_DRBGAPIERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EC_SIGN_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# Else if (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_ECDSA_SIGN is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_ECDSA_SIGN is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdsaSignOperate (st_pkc_ec_ctx_t * const p_ctx,
                                            const e_pkc_sync_opt_t sync_opt);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdsaSignFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Generate ECDSA signature.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in,out] p_sign         A structure consisting of the byte length len of the data concatenated
 *                                       with the signature values R and S and the data pointer p_data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_EC_ZERO_SIGNATURE:        Generated a signature with a value of 0
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 *  - #PKCL_RET_DRV_TAMPERERR:            Tampering has been detected in PKC-Driver
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdsaSignFinal(When #PKCL_CFG_ENABLE_DRV_ECDSA_SIGN == 1)
 *  - R_PKCD_EcScalarMultFinal
 *  - R_PKCD_EcdsaMakeSign
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EC_SIGN_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Call R_PKCD_EcdsaMakeSign to generate ECDSA signature.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_ECDSA_SIGN is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EC_SIGN_OPERATE_DONE == pkcl_state, then
 *    -# If (NULL != p_ctx) && (NULL != p_sign), then
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info), then
 *        -# Check if the given curve structure is valid with #r_pkcl_ec_chk_curve, and set ret to the result.
 *        -# If #PKCL_RET_OK == ret, then
 *          -# Check if the given signature or point buffer is valid with #r_pkcl_ec_chk_sign_or_point.
 *             And set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Se ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret, then
 *      -# Set point_data.len to sizeof(point_val).
 *      -# Set point_data.p_data to point_val.
 *      -# Assign a buffer for the driver's EC operation as p_buf_drv from p_ctx->p_buf area.
 *      -# Finalize the scalar multiplication of the base point with R_PKCD_EcScalarMultFinal.
 *         And set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret, then
 *        -# Assign a buffer for the EC operation as p_buf_drv from p_ctx->p_buf area.
 *        -# Set digest_data.len to p_ec_sign->digest_len.
 *        -# Set digest_data.p_data to p_ec_sign->digest_data.
 *        -# Set priv_scalar_data.len to p_ec_sign->priv_scalar_len.
 *        -# Set priv_scalar_data.p_data to p_ec_sign->priv_scalar_data.
 *        -# Make the signature r, s from the multiplicated point with R_PKCD_EcdsaMakeSign,
 *           and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *        -# Use #r_pkcl_cmn_set_byte to clear the value of (random mod p) used in signature generation stored in p_buf.
 *        -# If (#PKCL_RET_OK == ret), then
 *          -# If (#PKC_BOOL_FALSE != p_ctx->verified_gen_sign), then
 *            -# Verify the signature in sign_temp_data with #r_pkcl_ec_verified_gen_sign, and set the result to ret.
 *          -# Else
 *            -# Do nothing.
 *          -# If (#PKCL_RET_OK == ret), then
 *            -# Copy the signature and its size in sign_temp_data to p_sign.
 *          -# Else
 *            -# Do nothing.
 *          -# Clear the temporal signature data in sign_temp_data.
 *        -# Else
 *          -# Do nothing.
 *      -# Else
 *        -# Do nothing.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) ||
 *        (#PKCL_RET_PKCCERR == ret) || (#PKCL_RET_EC_ZERO_SIGNATURE == ret)), then
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_ECDSA_SIGN is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_ECDSA_SIGN is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdsaSignFinal (const st_pkc_ec_ctx_t * const p_ctx,
                                        st_pkc_data_t * const p_sign);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdsaVerifyInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Performs the initial processing of ECDSA signature verification.
 * \param       [in]     p_digest      A structure consisting of the byte length len of the message digest and
 *                                      the data pointer p_data.
 * \param       [in]     p_sign        A structure consisting of the byte length len of the data concatenated
 *                                      with the signature values R and S and the data pointer p_data.
 * \param       [in]     p_pub_key     A pointer to an EC public key in key binary format.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by EC.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdsaVerifyInit(When #PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY == 1)
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the parameters required for ECDSA signature verification.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# If (NULL != p_digest) && (NULL != p_sign) && (NULL != p_pub_key) && (NULL != p_ctx) then,
 *      -# Check if the given digest structure is valid with #r_pkcl_ec_chk_digest and set ret to the result.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *          -# Check if the given curve structure is valid with #r_pkcl_ec_chk_curve and set ret to the result.
 *          -# If #PKCL_RET_OK == ret then,
 *            -# Check if the given signature structure is valid with #r_pkcl_ec_chk_sign_or_point
 *               and set ret to the result.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EC operation as p_ec_verify from p_ctx->p_buf area.
 *      -# If #PKCL_EC_CURVE_LEN_MAX < p_digest->len then,
 *        -# Set p_ec_verify->digest_len to #PKCL_EC_CURVE_LEN_MAX.
 *      -# Else
 *        -# Set p_ec_verify->digest_len to p_digest->len.
 *      -# Copy the given digest to the buffer by the buffer size with #r_pkcl_cmn_cpy_byte.
 *      -# Set p_ec_verify->sign_len to p_sign->len.
 *      -# Copy the given signature to the buffer by the buffer size with #r_pkcl_cmn_cpy_byte.
 *      -# Set p_ec_verify->p_pub_key to p_pub_key.
 *      -# Set the PKC state to #PKCL_STATE_EC_VERIFY_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdsaVerifyInit (const st_pkc_data_t * const p_digest,
                                            const st_pkc_data_t * const p_sign,
                                            const void * const p_pub_key,
                                            st_pkc_ec_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdsaVerifyOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Calculate parameters for ECDSA signature verification processing.
 * \param       [in,out] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]     sync_opt       Select synchronous/asynchronous execution
 * \return
 *  - #PKCL_RET_OK:                       Successful completion (API 2nd call)
 *  - #PKCL_RET_PROCESSING:               Successful completion (API 1st call)
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdsaVerifyOperate(When #PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY == 1)
 *  - R_PKCD_EcdsaCalcU1U2
 *  - R_PKCD_EcScalarMultInit
 *  - R_PKCD_EcScalarMultOperate
 *  - R_PKCD_EcScalarMultFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EC_VERIFY_INIT_DONE or #PKCL_STATE_EC_VERIFY_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Calculate the parameters u1G and u2Q for ECDSA signature verification processing. \n
 *        (G: Base point, Q: Public key)
 * \par Remarks
 *  - This API needs to be called twice. \n
 *    #PKCL_RET_STATUSERR is returned if the next Final function is called with only one call.
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EC_VERIFY_INIT_DONE == pkcl_state then,
 *    -# Check the arguments p_ctx, sync_opt with #r_pkcl_ec_chk_verify_operate_arg.
 *    -# If #PKCL_RET_OK == ret then,
 *    -# Assign a buffer for the EC operation as p_ec_verify from p_ctx->p_buf area.
 *    -# Set the digest address digest_data.p_data to p_ec_verify->digest_data.
 *    -# Set the diggest length digest_data.len to p_ec_verify->digest_len.
 *    -# Set the signature address sign_data.p_data to p_ec_verify->sign_data.
 *    -# Set the signature length sign_data.len to p_ec_verify->sign_len.
 *    -# Set u1 address u1_data.p_data to the buffer u1_val.
 *    -# Set u1 length u1_data.len to #PKCL_EC_CURVE_LEN_MAX.
 *    -# Set u2 address u2_data.p_data to p_ec_verify->u2_data.
 *    -# Set u2 length u2_data.len to #PKCL_EC_CURVE_LEN_MAX.
 *    -# Assign a buffer for the driver's EC operation as p_buf_drv from p_ctx->p_buf area.
 *    -# Calculate ECDSA U1, U2 value with R_PKCD_EcdsaCalcU1U2 and set drv_ret to the result.
 *    -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Set p_ec_verify->u2_len to u2_data.len.
 *      -# Set u1_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *      -# Set u1_sel.p_sel_data to &u1_data.
 *      -# Copy a base point from the curve structure to a buffer base_point_val with #r_pkcl_cmn_cpy_byte.
 *      -# Set base_point_data.p_data to &base_point_val[0].
 *      -# Set base_point_data.len to p_ctx->p_curve_info->curve_len * #PKCL_EC_DOUBLE_LEN.
 *      -# Set base_point_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *      -# Set base_point_sel.p_sel_data to base_point_data.
 *      -# Initiaize the scalar multiplication of the base point with R_PKCD_EcScalarMultInit.
 *         And set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Execute scalar multiplication of the base point with R_PKCD_EcScalarMultOperate.
 *           And set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else if #PKCL_STATE_EC_VERIFY_OPERATE_PROCESSING == pkcl_state then,
 *    -# Check the arguments p_ctx, sync_opt with #r_pkcl_ec_chk_verify_operate_arg and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the driver's EC operation as p_ec_verify from p_ctx->p_buf.
 *      -# Set u1g_data.len to sizeof(p_ec_verify->u1g_data).
 *      -# Set u1g_data.p_data to p_ec_verify->u1g_data.
 *      -# Finalize the EC scalar multiplication of the base point with R_PKCD_EcScalarMultFinal.
 *         And set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Set u1 length p_ec_verify->u1g_len to u1g_data.len.
 *        -# Set u2 address u2_data.p_data to p_ec_verify->u2_data.
 *        -# Set u2 length u2_data.len to p_ec_verify->u2_len.
 *        -# Set the u2 structure type u2_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *        -# Set the u2 structure data address u2_sel.p_sel_data to &u2_data.
 *        -# Set the public key structure type pubkey_sel.data_type to #PKC_DATA_TYPE_KEY_BINARY.
 *        -# Set the public key structure data address pubkey_sel.p_sel_data to &pubkey_data.
 *        -# Set the public key address pubkey_sel.p_sel_data->p_data to (uint8_t*)p_ec_verify->p_pub_key.
 *        -# Initialize the scalar multiplication of the pblic key with R_PKCD_EcScalarMultInit.
 *           And set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Execute the scalar multiplication of the pblic key with R_PKCD_EcScalarMultOperate.
 *            -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EC_VERIFY_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# Else if #PKCL_RET_PROCESSING == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EC_VERIFY_OPERATE_PROCESSING with #r_pkcl_cmn_set_state.
 *  -# Else if (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdsaVerifyOperate (st_pkc_ec_ctx_t * const p_ctx,
                                            const e_pkc_sync_opt_t sync_opt);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdsaVerifyFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Output the result of ECDSA signature verification.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_EC_VERIFAIL:              Signature verification failed
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 *  - #PKCL_RET_DRV_TAMPERERR:            Tampering has been detected in PKC-Driver
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdsaVerifyFinal(When #PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY == 1)
 *  - R_PKCD_EcScalarMultFinal
 *  - R_PKCD_EcdsaCompareSign
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EC_VERIFY_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Call R_PKCD_EcdsaCompareSign to output the ECDSA signature verification result.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EC_VERIFY_OPERATE_DONE == pkcl_state then,
 *    -# If NULL != p_ctx then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_ec_chk_curve and set ret to the result.
 *      -# Else
 *        -# set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EC operation as p_ec_verify from p_ctx->p_buf.
 *      -# Set the multiplied public key length u2q_data.len to sizeof(u2q_val).
 *      -# Set the multiplied public key address u2q_data.p_data to u2q_val.
 *      -# Assign a buffer for the driver's EC operation as p_buf_drv from p_ctx->p_buf.
 *      -# Finalize the scalar multiplication of the public key with R_PKCD_EcScalarMultFinal.
 *         And set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Set the multiplied base point address u1g_data.p_data to p_ec_verify->u1g_data.
 *        -# Set the multiplied base point length u1g_data.len to p_ec_verify->u1g_len.
 *        -# Set the signature address sign_data.p_data to p_ec_verify->sign_data.
 *        -# Set the signature length sign_data.len to p_ec_verify->sign_len.
 *        -# Verify the signature with R_PKCD_EcdsaCompareSign and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) ||
 *        (#PKCL_RET_PKCCERR == ret) || (#PKCL_RET_EC_VERIFAIL == ret)) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_ECDSA_VERIFY is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdsaVerifyFinal (const st_pkc_ec_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEcScalarMultInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Performs initial processing for ECC scalar multiplication.
 * \param       [in]     p_scalar_or_key    Scalar data structure \n
 *                                          Select and set the EC private key in byte array data or key binary format.
 * \param       [in]     p_point_or_key     Coordinate data structure \n
 *                                          Byte array data or key Binary format EC public key or key including
 *                                           public key Binary format EC private key is selected and set.
 * \param       [in,out] p_ctx              A pointer to a structure that summarizes the parameters required by EC.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcScalarMultInit
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the parameters required for ECC scalar multiplication.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# If (NULL != p_scalar_or_key) && (NULL != p_point_or_key) && (NULL != p_ctx), then
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) &&
 *         ((NULL != p_ctx->p_drbg_api) || (#PKC_BOOL_FALSE == p_ctx->sec_scalar_mul)), then
 *        -# Check the given curve structure with #r_pkcl_ec_chk_curve and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *      -# If #PKCL_RET_OK == ret, then
 *        -# Check the given scalar byte array or binary with #r_pkcl_ec_chk_select_data and set ret to the result.
 *        -# If #PKCL_RET_OK == ret, then
 *          -# Check the given point byte array or binary with #r_pkcl_ec_chk_select_data and set ret to the result.
 *    -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret, then
 *      -# Initialize the multiplication of the point with R_PKCD_EcScalarMultInit and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EC_SMULT_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEcScalarMultInit (const st_pkc_select_data_t * const p_scalar_or_key,
                                            const st_pkc_select_data_t * const p_point_or_key,
                                            st_pkc_ec_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEcScalarMultOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Perform ECC scalar multiplication.
 * \param       [in,out] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]     sync_opt       Select synchronous/asynchronous execution
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcScalarMultOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EC_SMULT_INIT_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Call R_PKCD_EcScalarMultOperate to perform ECC scalar multiplication.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EC_SMULT_INIT_DONE == pkcl_state then,
 *    -# If (NULL != p_ctx) && ((#PKC_SYNC_OPT_SYNC == sync_opt) || (#PKC_SYNC_OPT_ASYNC == sync_opt)) then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_ec_chk_curve and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Operate the EC scalar multiplication with R_PKCD_EcScalarMultOperate and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EC_SMULT_OPERATE_DONE.
 *  -# ElseIf (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEcScalarMultOperate (st_pkc_ec_ctx_t * const p_ctx,
                                                const e_pkc_sync_opt_t sync_opt);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEcScalarMultFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Output the result of scalar multiplication.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in,out] p_point_out    A structure consisting of the byte length len of coordinate data multiplied
 *                                       by scalar and the data pointer p_data. \n
 *                                      Output data that concatenates x and y coordinates.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcScalarMultFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EC_SMULT_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Output the result of scalar multiplication.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EC_SMULT_OPERATE_DONE == pkcl_state then,
 *    -# If (NULL != p_ctx) && (NULL != p_point_out) then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_ec_chk_curve and set ret to the result.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Check if the given point structure or binary is valid with #r_pkcl_ec_chk_sign_or_point.
 *             And set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Finalize the EC scalar multiplication with R_PKCD_EcScalarMultFinal and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEcScalarMultFinal (const st_pkc_ec_ctx_t * const p_ctx,
                                                st_pkc_data_t * const p_point_out);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEcdsaMakeSign
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Generate signature.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]     p_digest       A structure consisting of the byte length len of the message digest and
 *                                       the data pointer p_data.
 * \param       [in]     p_point        A structure consisting of the byte length len of the data that concatenates
 *                                       the x and y coordinates and the data pointer p_data.
 * \param       [in]     p_random       A structure consisting of the byte length len of the random number k and
 *                                       the data pointer p_data.
 * \param       [in]     p_priv_key     Key A pointer to an EC private key in binary format.
 * \param       [in,out] p_sign         A structure consisting of the byte length len of the data concatenated
 *                                       with the signature values R and S and the data pointer p_data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_EC_ZERO_SIGNATURE:        Generated a signature with a value of 0
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 *  - #PKCL_RET_DRV_TAMPERERR:            Tampering has been detected in PKC-Driver
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdsaMakeSign
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Call R_PKCD_EcdsaMakeSign to generate a signature.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state, then
 *    -# Check if the arguments p_ctx, p_digest, p_point, p_random, p_priv_key, p_sign are valid
 *       with #r_pkcl_ec_chk_make_sign_arg. And set ret to the result.
 *    -# If #PKCL_RET_OK == ret, then
 *      -# Generate a signature from the given point, random scalar, digest and private key
 *         with R_PKCD_EcdsaMakeSign, and set drv_ret to the result.
 *      -# Set sign_temp_data the generated signature and its byte length.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret, then
 *        -# If #PKC_BOOL_FALSE != p_ctx->verified_gen_sign, then
 *          -# Verify the generated signature with #r_pkcl_ec_verified_gen_sign, and set ret to the result.
 *        -# If #PKCL_RET_OK == ret, then
 *          -# Copy the signature in sign_temp_data.p_data to p_sign->p_data.
 *          -# Set p_sign->len to sign_temp_data.len.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEcdsaMakeSign (const st_pkc_ec_ctx_t * const p_ctx,
                                            const st_pkc_data_t * const p_digest,
                                            const st_pkc_data_t * const p_point,
                                            const st_pkc_data_t * const p_random,
                                            const void * const p_priv_key,
                                            st_pkc_data_t * const p_sign);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEcdsaCalcU1U2
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Calculate u1 and u2.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]     p_digest       A structure consisting of the byte length len of the message digest and
 *                                       the data pointer p_data.
 * \param       [in]     p_sign         A structure consisting of the byte length len of the data concatenated
 *                                       with the signature values R and S and the data pointer p_data.
 * \param       [in,out] p_u1           Structure consisting of byte length len of intermediate data u1 and
 *                                       data pointer p_data.
 * \param       [in,out] p_u2           Structure consisting of byte length len of intermediate data u2 and
 *                                       data pointer p_data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdsaCalcU1U2
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Call R_PKCD_EcdsaCalcU1U2 to calculate u1 and u2.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# Check if the given arguments p_ctx, p_digest, p_sign, p_u1, p_u2 are valid
 *       with #r_pkcl_ec_chk_calc_u1u2_arg. And setret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Calculate U1, U2 from the given signature and digest with R_PKCD_EcdsaCalcU1U2.
 *         And set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *     -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEcdsaCalcU1U2 (const st_pkc_ec_ctx_t * const p_ctx,
                                            const st_pkc_data_t * const p_digest,
                                            const st_pkc_data_t * const p_sign,
                                            st_pkc_data_t * const p_u1,
                                            st_pkc_data_t * const p_u2);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEcdsaCompareSign
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Perform signature verification.
 * \param       [in] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in] p_u1g          Points derived from u1 * G (x, y) \n
 *                                  A structure consisting of the byte length len of the data that concatenates the x
 *                                   and y coordinates and the data pointer p_data.
 * \param       [in] p_u2q          Points derived from u2 * Q (x, y) \n
 *                                  A structure consisting of the byte length len of the data that concatenates the x
 *                                   and y coordinates and the data pointer p_data.
 * \param       [in] p_sign         A structure consisting of the byte length len of the data concatenated with
 *                                   the signature values R and S and the data pointer p_data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_EC_VERIFAIL:              Signature verification failed
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 *  - #PKCL_RET_DRV_TAMPERERR:            Tampering has been detected in PKC-Driver
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdsaCompareSign
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Call R_PKCD_EcdsaCompareSign to verify the signature.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# Check the given arguments p_ctx, p_u1g, p_u2q, p_sign with #r_pkcl_ec_chk_compare_sign_arg.
 *       And set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Verify the signature using u1*g and u2*q with R_PKCD_EcdsaCompareSign.
 *         And set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEcdsaCompareSign (const st_pkc_ec_ctx_t * const p_ctx,
                                            const st_pkc_data_t * const p_u1g,
                                            const st_pkc_data_t * const p_u2q,
                                            const st_pkc_data_t * const p_sign);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Set the required parameters for ECDH.
 * \param       [in]     p_priv_key    Key A pointer to an EC private key in binary format.
 * \param       [in]     p_pub_key     Key A pointer to an EC public key in binary format.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by EC.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdhInit(When #PKCL_CFG_ENABLE_DRV_ECDH == 1)
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the required parameters for ECDH.
 *      - When performing KDF processing with ECDH, set the KDF function pointer in p_ctx. \n
 *        If KDF processing is not performed, set NULL in p_ctx-> p_kdf_info.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_ECDH is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# If (NULL != p_priv_key) && (NULL != p_pub_key) && (NULL != p_ctx) then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_ec_chk_curve and set ret to the result.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# If NULL != p_ctx->p_kdf_info then,
 *            -# If #PKCL_EC_KDF_OUT_LEN_MAX >= p_ctx->p_kdf_info->out_kdf_len then,
 *              -# If NULL != p_ctx->p_kdf_info->p_kdf_api then
 *                -# Do nothing.
 *              -# Else
 *                -# Set ret to #PKCL_RET_PARAMERR.
 *            -# Else
 *              -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the ECDH operation as p_ec_ecdh from p_ctx->p_buf.
 *      -# Set private key address p_ec_ecdh->p_priv_key to p_priv_key.
 *      -# Set public key address p_ec_ecdh->p_pub_key to p_pub_key.
 *      -# Set the PKC state to #PKCL_STATE_EC_ECDH_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_ECDH is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_ECDH is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdhInit (const void * const p_priv_key,
                                    const void * const p_pub_key,
                                    st_pkc_ec_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Perform ECDH key exchange.
 * \param       [in,out] p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in]     sync_opt       Select synchronous/asynchronous execution
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdhOperate(When #PKCL_CFG_ENABLE_DRV_ECDH == 1)
 *  - R_PKCD_EcScalarMultInit
 *  - R_PKCD_EcScalarMultOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EC_ECDH_INIT_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Perform ECDH key exchange.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_ECDH is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EC_ECDH_INIT_DONE == pkcl_state then,
 *    -# Check the arguments p_ctx, sync_opt with #r_pkcl_ec_chk_ecdh_operate_arg and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the ECDH operation as p_ec_ecdh from p_ctx->p_buf.
 *      -# Set the private key structure type privkey_sel.data_type to #PKC_DATA_TYPE_KEY_BINARY.
 *      -# Set the private key structure content address privkey_sel.p_sel_data to &privkey_data.
 *      -# Set thr private key address privkey_sel.p_sel_data->p_data to (uint8_t*)p_ec_ecdh->p_priv_key.
 *      -# Set the public key structure type pubkey_sel.data_type to #PKC_DATA_TYPE_KEY_BINARY.
 *      -# Set the public key structure content address pubkey_sel.p_sel_data to &pubkey_data.
 *      -# Set the public key address pubkey_sel.p_sel_data->p_data to (uint8_t*)p_ec_ecdh->p_pub_key.
 *      -# Assign a buffer for the driver's ECDH operation as p_buf_drv from p_ctx->p_buf.
 *      -# Initialize the EC scalar multiplication with R_PKCD_EcScalarMultInit and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Operate the EC scalar multiplication with R_PKCD_EcScalarMultOperate and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EC_ECDH_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# ElseIf (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_ECDH is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_ECDH is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdhOperate (st_pkc_ec_ctx_t * const p_ctx,
                                        const e_pkc_sync_opt_t sync_opt);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       If KDF processing is not performed, the shared secret is output, and if it is performed,
 *               the common key is output.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in,out] p_shared       A structure consisting of a shared secret or common key byte length len
 *                                       and a data pointer p_data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdhFinal(When #PKCL_CFG_ENABLE_DRV_ECDH == 1)
 *  - R_PKCD_EcScalarMultFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EC_ECDH_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - If KDF processing is not performed, the shared secret is output, and if it is performed,
 *         the common key is output.
 *      - If p_ctx->p_kdf_info is non-null, it may use the external KDF API. \n
 *        When using HW in the API of the external library, it is necessary to secure HW resources on the user side.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_ECDH is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EC_ECDH_OPERATE_DONE == pkcl_state then,
 *    -# Check if the arguments p_ctx, p_shared are valid with #r_pkcl_ec_chk_ecdh_final_arg and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the ECDH operation as p_ec_ecdh from p_ctx->p_buf.
 *      -# Set the shared secret address shared_data.p_data to p_ec_ecdh->shared_data.
 *      -# Set the shared secret length shared_data.len to sizeof(p_ec_ecdh->shared_data).
 *      -# Assign a buffer for the driver's ECDH operation as p_buf_drv from p_ctx->p_buf.
 *      -# Finalize the EC scalar multiplication with R_PKCD_EcScalarMultFinal and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If NULL != p_ctx->p_kdf_info then,
 *        -# Derive ta shared key from the shared secret shared_data with p_ctx->p_kdf_info->p_kdf_api.
 *           And set kdf_ret to the result.
 *        -# If #PKCL_EXTERNAL_API_OK == kdf_ret then,
 *          -# Overwrite p_shared->len with the derived key length p_ctx->p_kdf_info->out_kdf_len.
 *        -# Else
 *          -# Set ret to #PKCL_RET_KDFAPIERR.
 *      -# Else
 *        -# Copy the shared secret to p_shared->p_data with #r_pkcl_cmn_cpy_byte.
 *        -# Set p_shared->len to curve_len.
 *      -# Use #r_pkcl_cmn_set_byte to clear shared secret results stored in p_buf.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_ECDH is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_ECDH is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdhFinal (const st_pkc_ec_ctx_t * const p_ctx,
                                    st_pkc_data_t * const p_shared);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcKeyGenInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Set the parameters required for the ECC key pair generation.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by the EC APIs.
 * \return
 *  - #PKCL_RET_OK:                         Successful completion
 *  - #PKCL_RET_INTERNALERR:                Internal processing failed
 *  - #PKCL_RET_STATUSERR:                  Called at an invalid PKC state.
 *  - #PKCL_RET_PARAMERR:                   Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                  Invalid argument length entered
 *  - #PKCL_RET_DRBGAPIERR:                 Failed in the DRBG API passed by the caller.
 *  - #PKCL_RET_TAMPERERR:                  Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcKeyGenInit
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *  - Set the parameters for ECC key pair generation, and generate a random number with PKC-Driver API.
 * \par Remarks
 *  - This API uses an external DRBG API passed by the caller for the random number generation. \n
 *    HW resources and data used by the API must be secured by the API design.
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# If NULL != p_ctx then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_ec_chk_curve and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Generate an N+64bit random number with R_PKCD_EcKeyGenInit and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# Else
 *      -# Do nothing.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EC_KEYGEN_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcKeyGenInit (st_pkc_ec_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcKeyGenOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Generate key pair.
 * \param       [in,out] p_ctx          A pointer to a structure that summarizes the parameters required by the EC APIs.
 * \param       [in]     sync_opt       Select synchronous/asynchronous execution.
 * \return
 *  - #PKCL_RET_OK:                     Successful completion.
 *  - #PKCL_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCL_RET_STATUSERR:              Called at an invalid PKC state.
 *  - #PKCL_RET_PARAMERR:               Invalid argument entered.
 *  - #PKCL_RET_LENGTHERR:              Invalid argument length entered.
 *  - #PKCL_RET_DRV_PARAMERR:           Invalid argument entered in PKC-Driver.
 *  - #PKCL_RET_DRV_LENGTHERR:          Invalid argument length entered in PKC-Driver.
 *  - #PKCL_RET_DRV_INTERNALERR:        Internal processing failed in PKC-Driver.
 *  - #PKCL_RET_PKCCTIMEOUTERR:         PKCC timed out.
 *  - #PKCL_RET_PKCCERR:                An error occurred in PKCC.
 *  - #PKCL_RET_TAMPERERR:              Tampering has been detected.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcKeyGenOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EC_KEYGEN_INIT_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Generate an ECC key pair from the random number generated in #R_PKCL_EcKeyGenInit API.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EC_KEYGEN_INIT_DONE == pkcl_state then,
 *    -# If (NULL != p_ctx) && ((#PKC_SYNC_OPT_SYNC == sync_opt) || (#PKC_SYNC_OPT_ASYNC == sync_opt)) then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_ec_chk_curve and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EC key generation as p_ec_keygen from p_ctx->p_buf.
 *      -# Generate the private scalar from the N+64bits random number wih R_PKCD_EcKeyGenOperate.
 *         And set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# Else
 *      -# Do nothing.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EC_KEYGEN_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# ElseIf (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcKeyGenOperate (st_pkc_ec_ctx_t * const p_ctx,
                                            const e_pkc_sync_opt_t sync_opt);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcKeyGenFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECAPIs
 * \brief       Output the generated key pair.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EC.
 * \param       [in,out] p_priv_key     A pointer to the memory area allocated by the user SW to store the key binary
 *                                       of the EC private key.
 * \param       [in,out] p_pub_key      A pointer to the memory area allocated by the user SW to store the key binary
 *                                       of the EC public key.
 * \return
 *  - #PKCL_RET_OK:                     Successful completion
 *  - #PKCL_RET_INTERNALERR:            Internal processing failed.
 *  - #PKCL_RET_STATUSERR:              Called at an invalid PKC state.
 *  - #PKCL_RET_PARAMERR:               Invalid argument entered.
 *  - #PKCL_RET_LENGTHERR:              Invalid argument length entered.
 *  - #PKCL_RET_DRV_PARAMERR:           Invalid argument entered in PKC-Driver.
 *  - #PKCL_RET_DRV_LENGTHERR:          Invalid argument length entered in PKC-Driver.
 *  - #PKCL_RET_DRV_INTERNALERR:        Internal processing failed in PKC-Driver.
 *  - #PKCL_RET_PKCCTIMEOUTERR:         PKCC timed out.
 *  - #PKCL_RET_PKCCERR:                An error occurred in PKCC.
 *  - #PKCL_RET_TAMPERERR:              Tampering has been detected.
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcKeyGenFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EC_KEYGEN_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Convert the raw key pair to the key binary format supported by the PKC-Driver.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EC_KEYGEN_OPERATE_DONE == pkcl_state then,
 *    -# If (NULL != p_ctx) && (NULL != p_priv_key) && (NULL != p_pub_key) then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_ec_chk_curve and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the driver's EC key generation as p_buf_drv from p_ctx->p_buf.
 *      -# Wrap the raw key pair as the correspondent key binary format with R_PKCD_EcKeyGenFinal.
 *         And set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *    -# Else
 *      -# Do nothing.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Do nothing.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcKeyGenFinal (const st_pkc_ec_ctx_t * const p_ctx,
                                        void * const p_priv_key,
                                        void * const p_pub_key);


/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaSignInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Performs the initial processing of EdDSA signature generation.
 * \param       [in]     p_msg         A structure consisting of the byte length len of the message data and
 *                                      the data pointer p_data.
 * \param       [in]     p_priv_key    A pointer to the EdDSA private key in key binary format.
 * \param       [in]     p_pub_key     A pointer to the EdDSA public key in key binary format.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by EdDSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaSignInit(When #PKCL_CFG_ENABLE_DRV_EDDSA_SIGN == 1)
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the parameters required for EdDSA signature generation.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_EDDSA_SIGN is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# Check the arguments p_msg, p_priv_key, p_pub_key, p_ctx with #r_pkcl_eddsa_chk_sign_init_arg.
 *       And set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EdDSA as p_eddsa_sign from p_ctx->p_buf.
 *      -# Clear the buffer p_eddsa_sign->scalar_s_data with #r_pkcl_cmn_set_byte.
 *      -# Clear the buffer p_eddsa_sign->scalar_r_data with #r_pkcl_cmn_set_byte.
 *      -# Set the message length p_eddsa_sign->msg_len to p_msg->len.
 *      -# Set the private key address p_eddsa_sign->p_priv_key to p_priv_key.
 *      -# Set the public key address p_eddsa_sign->p_pub_key to p_pub_key.
 *      -# Set the PKC state to #PKCL_STATE_EDDSA_SIGN_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_EDDSA_SIGN is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_EDDSA_SIGN is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EddsaSignInit (const st_pkc_data_t * const p_msg,
                                        const void * const p_priv_key,
                                        const void * const p_pub_key,
                                        st_pkc_eddsa_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaSignOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Perform the calculation to obtain the signature R.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in] processing_bits   Number of bits processed at one time when the operation is executed
 *                                      in multiple times in PKCC.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaSignOperate(When #PKCL_CFG_ENABLE_DRV_EDDSA_SIGN == 1)
 *  - R_PKCD_EddsaScalarMultBaseInit
 *  - R_PKCD_EddsaScalarMultBaseOperate
 *  - R_PKCD_EddsaMakeScalarSandR
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EDDSA_SIGN_INIT_DONE or #PKCL_STATE_EDDSA_SIGN_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - This API is called multiple times depending on the argument processing_bits.
 *      - (1) If p_ctx-> eddsa_variant is PKC_EDDSA_VARIANT_HASH,
 *             hash the message data with the corresponding hash function. \n
 *            Generate intermediate data integers S and R from R_PKCD_EddsaMakeScalarSandR. \n
 *            If the calculation of R_PKCD_EddsaScalarMultBaseOperate is in the middle, go to (2),
 *             and if it finishes, return normal completion.
 *      - (2) If the calculation of R_PKCD_EddsaScalarMultBaseOperate is in the middle, repeat (2),
 *             and if it finishes, return normal end.
 * \par Remarks
 *  - It is necessary to call this API repeatedly until all the bits are processed. \n
 *    If there are unprocessed bits left, #PKCL_RET_PROCESSING is returned,
 *     and if all the bits are processed, #PKCL_RET_OK is returned. \n
 *    #PKCL_RET_STATUSERR is returned when the next Final function is called with unprocessed bits remaining.
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_EDDSA_SIGN is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EDDSA_SIGN_INIT_DONE == pkcl_state then,
 *    -# Check the arguments p_ctx, processing_bits with #r_pkcl_eddsa_chk_sign_verify_operate_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EdDSA operation as p_eddsa_sign from p_ctx->p_buf.
 *      -# Set the message address msg_data.p_data to (uint8_t*)p_eddsa_sign->p_msg.
 *      -# Set the message length msg_data.len to p_eddsa_sign->msg_len.
 *      -# Set the scalar s address scalar_s_data.p_data to p_eddsa_sign->scalar_s_data.
 *      -# Set the scalar s length scalar_s_data.len = sizeof(p_eddsa_sign->scalar_s_data).
 *      -# Set the scalar r address scalar_r_data.p_data to p_eddsa_sign->scalar_r_data.
 *      -# Set the scalar r length scalar_r_data.len = sizeof(p_eddsa_sign->scalar_r_data).
 *      -# Assign a buffer for the driver's EdDSA operation as p_buf_drv from p_ctx->p_buf.
 *      -# If #PKC_EDDSA_VARIANT_HASH == p_ctx->eddsa_variant
 *        -# If #PKCL_EDDSA_ED448_CURVE_LEN == p_ctx->p_curve_info->curve_len
 *          -# Set hmsg_data.len to #PKCL_EDDSA_SHAKE256BYTES_PH.
 *        -# Execute an external Hash init function p_ctx->p_hash_info->p_hash_init_api to initialize a hash context,
 *           and set the return value to hash_ret.
 *        -# If #PKCL_EXTERNAL_API_OK == hash_ret
 *          -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *             to hash a message, and set the return value to hash_ret.
 *          -# If #PKCL_EXTERNAL_API_OK == hash_ret
 *            -# Execute an external hash final function p_ctx->p_hash_info->p_hash_final_api
 *               to set the hased value and its byte size to hmsg_data.p_data,
 *               and set the return value to hash_ret.
 *            -# If #PKCL_EXTERNAL_API_OK == hash_ret
 *              -# Set the hash message length p_eddsa_sign->hash_msg_len to hmsg_data.len.
 *              -# Calculate the scalar s and r with R_PKCD_EddsaMakeScalarSandR and set drv_ret to the result.
 *              -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *            -# Else
 *              -# Set ret to #PKCL_RET_HASHAPIERR.
 *          -# Else
 *            -# Set ret to #PKCL_RET_HASHAPIERR.
 *        -# Else
 *          -# Set ret to #PKCL_RET_HASHAPIERR.
 *      -# Else if (#PKC_EDDSA_VARIANT_PURE == p_ctx->eddsa_variant) or<br>
 *                 (#PKC_EDDSA_VARIANT_CTX == p_ctx->eddsa_variant)
 *        -# Calculate the scalar s and r with R_PKCD_EddsaMakeScalarSandR and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# Else
 *        -# Set ret to #PKCL_RET_INTERNALERR.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Set p_eddsa_sign->scalar_s_len to scalar_s_data.len.
 *        -# Set p_eddsa_sign->scalar_r_len to scalar_r_data.len.
 *        -# Set the scalar s structure type scalar_r_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *        -# Set the scalar s structure contents scalar_r_sel.p_sel_data to &scalar_r_data.
 *        -# Initialize the multiplication of the base point by s with R_PKCD_EddsaScalarMultBaseInit
 *           and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Operate the multiplication of the case point by s with R_PKCD_EddsaScalarMultBaseOperate
 *             and set drv_ret to the result.
 *          -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else if #PKCL_STATE_EDDSA_SIGN_OPERATE_PROCESSING == pkcl_state then,
 *    -# Check the arguments p_ctx, processing_bits with #r_pkcl_eddsa_chk_sign_verify_operate_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EdDSA operation as p_buf_drv from p_ctx->p_buf.
 *      -# Operate the multiplication of the case point by s with R_PKCD_EddsaScalarMultBaseOperate
 *         and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EDDSA_SIGN_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# Else if #PKCL_RET_PROCESSING == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EDDSA_SIGN_OPERATE_PROCESSING with #r_pkcl_cmn_set_state.
 *  -# Else if ((#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret)) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_EDDSA_SIGN is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_EDDSA_SIGN is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EddsaSignOperate (st_pkc_eddsa_ctx_t * const p_ctx,
                                            const uint32_t processing_bits);

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaSignFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Generate EdDSA signature.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in,out] p_sign        A structure consisting of the byte length len of the data concatenated
 *                                      with the signature values R and S and the data pointer p_data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_EDDSA_ZERO_SIGNATURE:     Generated a signature with a value of 0
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaSignFinal(When #PKCL_CFG_ENABLE_DRV_EDDSA_SIGN == 1)
 *  - R_PKCD_EddsaScalarMultBaseFinal
 *  - R_PKCD_EddsaDeriveS
 *  - R_PKCD_EddsaHashRAM
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EDDSA_SIGN_INIT_DONE or #PKCL_STATE_EDDSA_SIGN_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Calculate H (R, A, M), which is the hash data of signature R, message data, and public key.
 *      - Derivation of signature S from H (R, A, M) and scalar values r, s.
 *      - Output signature value by concatenating signature R and signature S.
 *      - If p_ctx->verifed_gen_sign is not #PKC_BOOL_FALSE,
 *        signature verification is performed after signature generation.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_EDDSA_SIGN is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EDDSA_SIGN_OPERATE_DONE == pkcl_state then,
 *    -# Check the arguments p_ctx, p_sign with #r_pkcl_eddsa_chk_sign_final_arg and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EdDSA operation as p_eddsa_sign from p_ctx->p_buf.
 *      -# Set the signature r address sign_r_data.p_data to sign_r_val.
 *      -# Set the signature r length sign_r_data.len to sizeof(sign_r_val).
 *      -# Assign a buffer for the driver's EdDSA operation as p_buf_drv from p_ctx->p_buf.
 *      -# Finalize the multiplication of the base point by s with R_PKCD_EddsaScalarMultBaseFinal
 *         and set drv_ret to the result. The encoded resulting point is the signature value r.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# If #PKC_EDDSA_VARIANT_HASH == p_ctx->eddsa_variant
 *          -# Set the mesage address msg_data.p_data to p_eddsa_sign->hash_msg_data.
 *          -# Set the mesage length msg_data.len to p_eddsa_sign->hash_msg_len.
 *        -# Else if (#PKC_EDDSA_VARIANT_PURE == p_ctx->eddsa_variant) or<br>
 *                   (#PKC_EDDSA_VARIANT_CTX == p_ctx->eddsa_variant)
 *          -# Set the mesage address msg_data.p_data to (uint8_t*)p_eddsa_sign->p_msg.
 *          -# Set the mesage length msg_data.len to p_eddsa_sign->msg_len.
 *        -# Else
 *          -# Set ret to #PKCL_RET_INTERNALERR.
 *        -# Set the digest H(R,A,M) address hash_ram_data.p_data to hash_ram_val.
 *        -# Set the digest H(R,A,M) length hash_ram_data.len to sizeof(hash_ram_val).
 *        -# Calculate the digest H(R,A,M) from the signature r, the public key A and the message
 *            with R_PKCD_EddsaHashRAM and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Set the scalar s address scalar_s_data.p_data to p_eddsa_sign->scalar_s_data.
 *          -# Set the scalar s length scalar_s_data.len to p_eddsa_sign->scalar_s_len.
 *          -# Set the scalar r address scalar_r_data.p_data to p_eddsa_sign->scalar_r_data.
 *          -# Set the scalar r length scalar_r_data.len to p_eddsa_sign->scalar_r_len.
 *          -# Set the signature s address sign_s_data.p_data to sign_s_val.
 *          -# Set the signature s length sign_s_data.len to sizeof(sign_s_val).
 *          -# Derive the signature s from the signature r, H(R,A,M) and the integer s with R_PKCD_EddsaDeriveS
 *             and set drv_ret to the result.
 *          -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *          -# If #PKCL_RET_OK == ret then,
 *            -# If #PKC_BOOL_FALSE != p_ctx->verifed_gen_sign
 *              -# Verify the signature generated by calling #r_pkcl_eddsa_verified_gen_sign,
 *                 and output the result to ret.
 *            -# If #PKCL_RET_OK == ret then,
 *              -# Copy the resulting signature r to &p_sign->p_data[0] with #r_pkcl_cmn_cpy_byte.
 *              -# Copy the resulting signature s to &p_sign->p_data[sign_r_data.len] with #r_pkcl_cmn_cpy_byte.
 *              -# Set p_sign->len to (sign_r_data.len + sign_s_data.len).
 *            -# Clear the buffer sign_r_val and sign_s_val with #r_pkcl_cmn_set_byte.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_EDDSA_ZERO_SIGNATURE == ret) ||
 *        (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret)) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_EDDSA_SIGN is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_EDDSA_SIGN is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EddsaSignFinal (const st_pkc_eddsa_ctx_t * const p_ctx,
                                        st_pkc_data_t * const p_sign);

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaVerifyInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Performs the initial processing of EdDSA signature verification.
 * \param       [in]     p_msg         A structure consisting of the byte length len of the message data and
 *                                      the data pointer p_data.
 * \param       [in]     p_sign        A structure consisting of the byte length len of the data concatenated
 *                                      with the signature values R and S and the data pointer p_data.
 * \param       [in]     p_pub_key     A pointer to the EdDSA public key in key binary format.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by EdDSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaVerifyInit(When #PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY == 1)
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the parameters required for signature verification.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# Check arguments p_msg, p_sign, p_pub_key, p_ctx with #r_pkcl_eddsa_chk_verify_init_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EdDSA operation as p_eddsa_verify from p_ctx->p_buf.
 *      -# Clear the buffer p_eddsa_verify->sign_data with #r_pkcl_cmn_set_byte.
 *      -# Clear the buffer p_eddsa_verify->ha_data with #r_pkcl_cmn_set_byte.
 *      -# Set the message address p_eddsa_verify->p_msg to p_msg->p_data.
 *      -# Set the message length p_eddsa_verify->msg_len to p_msg->len.
 *      -# Set the signature address p_eddsa_verify->sign_len to p_sign->len.
 *      -# Copy the signature to the buffer &p_eddsa_verify->sign_data[0] with #r_pkcl_cmn_cpy_byte.
 *      -# Set p_eddsa_verify->p_pub_key to p_pub_key.
 *      -# Set the PKC state to #PKCL_STATE_EDDSA_VERIFY_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EddsaVerifyInit (const st_pkc_data_t * const p_msg,
                                            const st_pkc_data_t * const p_sign,
                                            const void * const p_pub_key,
                                            st_pkc_eddsa_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaVerifyOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Execute the calculation and get the intermediate data hA, SB.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in] processing_bits   Number of bits processed at one time when the operation is executed
 *                                      in multiple times in PKCC.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaVerifyOperate(When #PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY == 1)
 *  - R_PKCD_EddsaScalarMultBaseInit
 *  - R_PKCD_EddsaScalarMultBaseOperate
 *  - R_PKCD_EddsaScalarMultBaseFinal
 *  - R_PKCD_EddsaScalarMultInit
 *  - R_PKCD_EddsaScalarMultOperate
 *  - R_PKCD_EddsaHashRAM
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State must be #PKCL_STATE_EDDSA_VERIFY_INIT_DONE or #PKCL_STATE_EDDSA_VERIFY_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - This API is called multiple times depending on the argument processing_bits.
 *      - (1) If p_ctx-> eddsa_variant is PKC_EDDSA_VARIANT_HASH,
 *             hash the message data with the corresponding hash function. \n
 *            Generate intermediate data digest (R, A, M) with R_PKCD_EddsaHashRAM. \n
 *            Calculate hA using intermediate data digest (R, A, M) and public key. \n
 *            R_PKCD_EddsaScalarMultOperate If the calculation is in the middle, go to (2),
 *             if it is finished, go to (3).
 *      - (2) If the calculation of R_PKCD_EddsaScalarMultOperate is in the middle, repeat (2),
 *             if it is finished, go to (3).
 *      - (3) Calculate SB using the signature S and the base point of the curve. \n
 *            If the calculation of R_PKCD_EddsaScalarMultBaseOperate is in the middle, return to (4),
 *             and if it finishes, return normal completion.
 *      - (4) If the calculation of R_PKCD_EddsaScalarMultBaseOperate is in the middle, repeat (4),
 *             and if it finishes, return normal end.
 * \par Remarks
 *  - It is necessary to call this API repeatedly until all the bits are processed. \n
 *    If there are unprocessed bits left, #PKCL_RET_PROCESSING is returned,
 *     and if all the bits are processed, #PKCL_RET_OK is returned. \n
 *    #PKCL_RET_STATUSERR is returned when the next Final function is called with unprocessed bits remaining.
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EDDSA_VERIFY_INIT_DONE == pkcl_state then,
 *    -# Check the arguments p_ctx, processing_bits with #r_pkcl_eddsa_chk_sign_verify_operate_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EdDSA operation as p_eddsa_verify from p_ctx->p_buf.
 *      -# Set the message address msg_data.p_data to (uint8_t*)p_eddsa_verify->p_msg.
 *      -# Set the message length msg_data.len to p_eddsa_verify->msg_len.
 *      -# Set the signature r length sign_r_data.len to (p_eddsa_verify->sign_len / #PKCL_EDDSA_HALF_LEN).
 *      -# Copy the signature r value to the temporary buffer sign_r_val with #r_pkcl_cmn_cpy_byte.
 *      -# Set the signature r address sign_r_data.p_data to sign_r_val.
 *      -# Set H(R,A,M) address hash_ram_data.p_data to hash_ram_val.
 *      -# Set H(R,A,M) length hash_ram_data.len to sizeof(hash_ram_val).
 *      -# Assign a buffer for the driver's EdDSA operation as p_buf_drv from p_ctx->p_buf.
 *      -# If #PKC_EDDSA_VARIANT_HASH == p_ctx->eddsa_variant
 *        -# If #PKCL_EDDSA_ED448_CURVE_LEN == p_ctx->p_curve_info->curve_len
 *          -# Set hmsg_data.len to #PKCL_EDDSA_SHAKE256BYTES_PH.
 *        -# Execute an external Hash init function p_ctx->p_hash_info->p_hash_init_api to initialize a hash context,
 *           and set the return value to hash_ret.
 *        -# If #PKCL_EXTERNAL_API_OK == hash_ret
 *          -# Execute an external hash update function p_ctx->p_hash_info->p_hash_update_api
 *             to hash a message, and set the return value to hash_ret.
 *          -# If #PKCL_EXTERNAL_API_OK == hash_ret
 *            -# Execute an external hash final function p_ctx->p_hash_info->p_hash_final_api
 *               to set the hased value and its byte size to hmsg_data.p_data,
 *               and set the return value to hash_ret.
 *            -# If #PKCL_EXTERNAL_API_OK == hash_ret
 *              -# Calculate H(R,A,M) value from the signature r, the public key and the message with
 *                 R_PKCD_EddsaHashRAM and set drv_ret to the result.
 *              -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *            -# Else
 *              -# Set ret to #PKCL_RET_HASHAPIERR.
 *          -# Else
 *            -# Set ret to #PKCL_RET_HASHAPIERR.
 *        -# Else
 *          -# Set ret to #PKCL_RET_HASHAPIERR.
 *      -# Else if (#PKC_EDDSA_VARIANT_PURE == p_ctx->eddsa_variant) or<br>
 *                 (#PKC_EDDSA_VARIANT_CTX == p_ctx->eddsa_variant)
 *        -# Calculate H(R,A,M) value from the signature r, the public key and the message with R_PKCD_EddsaHashRAM
 *           and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# Else
 *        -# Set ret to #PKCL_RET_INTERNALERR.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Set H(R,A,M) structure type hash_ram_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *        -# Set H(R,A,M) structure content address hash_ram_sel.p_sel_data to &hash_ram_data.
 *        -# Set the public key structure type pubkey_sel.data_type to #PKC_DATA_TYPE_KEY_BINARY.
 *        -# Set the public key structure content address pubkey_sel.p_sel_data to &pubkey_data.
 *        -# Set the public key address pubkey_sel.p_sel_data to &pubkey_data.
 *        -# Initialize the multiplication of the public key by H(R,A,M) with R_PKCD_EddsaScalarMultInit
 *           and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Operate the multiplication of the public key by H(R,A,M) with R_PKCD_EddsaScalarMultOperate
 *             and set drv_ret to the result.
 *          -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *          -# Set the PKC state to #PKCL_STATE_EDDSA_VERIFY_OPERATE_PROCESSING with #r_pkcl_cmn_set_state.
 *          -# If #PKCL_RET_OK == ret then,
 *            -# Set verify_ope_flg to #PKCL_EDDSA_VERIFY_OPE_1ST_DONE.
 *            -# Set ret to #PKCL_RET_PROCESSING.
 *          -# Else if #PKCL_RET_PROCESSING == ret then,
 *            -# Set verify_ope_flg to #PKCL_EDDSA_VERIFY_OPE_1ST_PROCESSING.
 *  -# Else if #PKCL_STATE_EDDSA_VERIFY_OPERATE_PROCESSING == pkcl_state then,
 *    -# If #PKCL_EDDSA_VERIFY_OPE_1ST_PROCESSING == verify_ope_flg,
 *      -# Check the arguments p_ctx, processing_bits with #r_pkcl_eddsa_chk_sign_verify_operate_arg
 *         and set ret to the result.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Assign a buffer for the driver's EdDSA operation as p_buf_drv from p_ctx->p_buf.
 *          -# Operate the multiplication of the public key by H(R,A,M) with R_PKCD_EddsaScalarMultOperate
 *             and set drv_ret to the result.
 *          -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *          -# If #PKCL_RET_OK == ret then,
 *            -# Set verify_ope_flg to #PKCL_EDDSA_VERIFY_OPE_1ST_DONE.
 *            -# Set ret to #PKCL_RET_PROCESSING.
 *    -# Else if #PKCL_EDDSA_VERIFY_OPE_1ST_DONE == verify_ope_flg,
 *      -# Check the arguments p_ctx, processing_bits with #r_pkcl_eddsa_chk_sign_verify_operate_arg
 *         and set ret to the result.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Assign a buffer for the EdDSA operation as p_eddsa_verify from p_ctx->p_buf.
 *        -# Set H(R,A,M)*A address ha_data.p_data to p_eddsa_verify->ha_data.
 *        -# Set H(R,A,M)*A length ha_data.len to sizeof(p_eddsa_verify->ha_data).
 *        -# Assign a buffer for the driver's EdDSA operation as p_buf_drv from p_ctx->p_buf.
 *        -# Finalize the multiplication of the public key by H(R,A,M) with R_PKCD_EddsaScalarMultFinal
 *           and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Set H(R,A,M)*A length p_eddsa_verify->ha_len to ha_data.len.
 *          -# Set the signature s length sign_s_data.len to (p_eddsa_verify->sign_len / #PKCL_EDDSA_HALF_LEN).
 *          -# Copy the signature value from p_eddsa_verify->sign_data[p_eddsa_verify->sign_len / #PKCL_EDDSA_HALF_LEN]
 *             to the temporary buffer sign_s_val with #r_pkcl_cmn_cpy_byte.
 *          -# Set the signature s structure type sign_s_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *          -# Set the signature s structure content address sign_s_sel.p_sel_data to &sign_s_data.
 *          -# Initialize the multiplicaion of the base point by signature s with R_PKCD_EddsaScalarMultBaseInit
 *             and set drv_ret to the result.
 *          -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *            -# If #PKCL_RET_OK == ret then,
 *              -# Operate the multiplication of the base point by signature s with R_PKCD_EddsaScalarMultBaseOperate
 *                 and set drv_ret to the result.
 *              -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *              -# If #PKCL_RET_OK == ret then,
 *                -# Set the PKC state to #PKCL_STATE_EDDSA_VERIFY_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *              -# Else if #PKCL_RET_PROCESSING == ret then,
 *                -# Set verify_ope_flg to #PKCL_EDDSA_VERIFY_OPE_2ND_PROCESSING.
 *    -# Else if #PKCL_EDDSA_VERIFY_OPE_2ND_PROCESSING == verify_ope_flg,
 *      -# Check the arguments p_ctx, processing_bits with #r_pkcl_eddsa_chk_sign_verify_operate_arg
 *         and set ret to the result.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Assign a buffer for the driver's EdDSA operation as p_buf_drv from p_ctx->p_buf.
 *        -# Operate the scalar multiplication of the base point by signature s with R_PKCD_EddsaScalarMultBaseOperate
 *           and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Set the PKC state to #PKCL_STATE_EDDSA_VERIFY_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *    -# Else
 *      -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EddsaVerifyOperate (const st_pkc_eddsa_ctx_t * const p_ctx,
                                            const uint32_t processing_bits);

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaVerifyFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Output the result of EdDSA signature verification.
 * \param       [in]     p_ctx         A pointer to a structure that summarizes the parameters required by EdDSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_EDDSA_VERIFAIL:           Signature verification failed
 *  - #PKCL_RET_EDDSA_DECOMPERR:          Failed to decompress the coordinates
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaVerifyFinal(When #PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY == 1)
 *  - R_PKCD_EddsaScalarMultFinal
 *  - R_PKCD_EddsadsaCompareSign
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EDDSA_VERIFY_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Call R_PKCD_EddsadsaCompareSign to output the result of EdDSA signature verification.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EDDSA_VERIFY_OPERATE_DONE == pkcl_state then,
 *    -# If (NULL != p_ctx) then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_hash_info) && (NULL != p_ctx->p_curve_info) then,
 *        -# If (NULL != p_ctx->p_hash_info->p_hash_init_api) &&
 *              (NULL != p_ctx->p_hash_info->p_hash_update_api) &&
 *              (NULL != p_ctx->p_hash_info->p_hash_final_api)) then,
 *          -# Check the given curve structure with #r_pkcl_eddsa_chk_curve and set ret to the result.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EdDSA operation as p_eddsa_verify from p_ctx->p_buf.
 *      -# Set S*B address sb_data.p_data to sb_val.
 *      -# Set S*B length sb_data.len to sizeof(sb_val).
 *      -# Assign a buffer for the EdDSA operation as p_buf_drv from p_ctx->p_buf.
 *      -# Finalize the multiplication of the base point by the signature s with R_PKCD_EddsaScalarMultBaseFinal
 *         and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Set H(R,A,M)*A address ha_data.p_data to p_eddsa_verify->ha_data.
 *        -# Set H(R,A,M)*A length ha_data.len to p_eddsa_verify->ha_len.
 *        -# Set the signature r length sign_r_data.len = (p_eddsa_verify->sign_len / #PKCL_EDDSA_HALF_LEN).
 *        -# Copy the signature r value from &p_eddsa_verify->sign_data[0] to the temporary buffer sign_r_val
 *           with #r_pkcl_cmn_cpy_byte.
 *        -# Set the signature r address sign_r_data.p_data to sign_r_val.
 *        -# Verify the signature comparing the H(R,A,M)*A, S*B and the signature r with
 *           R_PKCD_EddsaCompareSign and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) ||
 *        (#PKCL_RET_EDDSA_VERIFAIL == ret) || (#PKCL_RET_EDDSA_DECOMPERR == ret),
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_EDDSA_VERIFY is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EddsaVerifyFinal (const st_pkc_eddsa_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaKeyGenInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Set the parameters required for EdDSA key pair generation.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by EdDSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the parameters required for EdDSA key pair generation.
 *      - Use the DRBG function to generate the EdDSA private key.
 *      - Hash the private key generated by the DRBG function using the HASH function.
 *      - This API uses an external Hash API and DRBG API. \n
 *        When using HW in the API of the external library, it is necessary to secure HW resources on the user side.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# If NULL != p_ctx then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_hash_info) &&
 *            (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info) then,
 *        -# If (NULL != p_ctx->p_hash_info->p_hash_init_api) &&
 *              (NULL != p_ctx->p_hash_info->p_hash_update_api) &&
 *              (NULL != p_ctx->p_hash_info->p_hash_final_api)
 *          -# Check the given curve structure with #r_pkcl_eddsa_chk_curve and set ret to the result.
 *        -# Else
 *          -# Set ret to #PKCL_RET_PARAMERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EdDSA as p_eddsa_keygen from p_ctx->p_buf.
 *      -# Clear the buffer p_eddsa_keygen->priv_scalar_data with #r_pkcl_cmn_set_byte.
 *      -# Clear the buffer p_eddsa_keygen->hash_scalar_data with #r_pkcl_cmn_set_byte.
 *      -# If #PKCL_EDDSA_ED25519_CURVE_LEN == p_ctx->p_curve_info->curve_len
 *        -# Set the private scalar length p_eddsa_keygen->priv_scalar_len to #PKCL_EDDSA_ED25519_KEY_LEN.
 *      -# Else
 *        -# Set the private scalar length p_eddsa_keygen->hash_scalar_len to #PKCL_EDDSA_SHAKE256BYTES.
 *        -# Set the private scalar length p_eddsa_keygen->priv_scalar_len to #PKCL_EDDSA_ED448_KEY_LEN.
 *      -# Generate random bytes to the private scalar with p_ctx->p_drbg_api and set external_ret to the result.
 *      -# If #PKCL_EXTERNAL_API_OK == external_ret then,
 *        -# Initialize a hash calculation with p_ctx->p_hash_info->p_hash_init_api and set external_ret to the result.
 *        -# If #PKCL_EXTERNAL_API_OK == external_ret then,
 *          -# Input the private scalar to the hash calculation with p_ctx->p_hash_info->p_hash_update_api.
 *             And set external_ret to the result.
 *          -# If #PKCL_EXTERNAL_API_OK == external_ret then,
 *            -# Finalize the hash calculation with p_ctx->p_hash_info->p_hash_final_api and set external_ret
 *               to the result.
 *            -# If #PKCL_EXTERNAL_API_OK == external_ret then,
 *              -# Set curve_len to p_ctx->p_curve_info->curve_len.
 *              -# Set the hashed scalar address p_hscalar to p_eddsa_keygen->hash_scalar_data.
 *              -# If #PKCL_EDDSA_ED25519_CURVE_LEN == curve_len then,
 *                -# Mask the MSByte of the hashed scalar with 0xF8.
 *                -# Mask the LSByte of the hashed scalar with 0x3F.
 *                -# Or the LSByte of the hashed scalar with 0x40.
 *                -# Set the hashed scalar length p_eddsa_keygen->hash_scalar_len to curve_len.
 *              -# Else
 *                -# Mask the MSByte of the hashed scalar with 0xFC.
 *                -# Mask the LSByte of the hashed scalar with 0x00.
 *                -# Or the second to last byte of the hashed scalar with 0x80.
 *                -# Set 3 bytes zero padding from the 58th byte of the hashed scalar.
 *                -# Set the hashed scalar length p_eddsa_keygen->hash_scalar_len to #PKCL_EDDSA_ED448_RU4_KEY_LEN.
 *            -# Else
 *              -# Set ret to #PKCL_RET_HASHAPIERR.
 *          -# Else
 *            -# Set ret to #PKCL_RET_HASHAPIERR.
 *        -# Else
 *          -# Set ret to #PKCL_RET_HASHAPIERR.
 *      -# Else
 *        -# Set ret to #PKCL_RET_DRBGAPIERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set thePKC state to #PKCL_STATE_EDDSA_KEYGEN_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EddsaKeyGenInit (st_pkc_eddsa_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaKeyGenOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Generate EdDSA key pair.
 * \param       [in,out] p_ctx          A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in] processing_bits    Number of bits processed at one time when the operation is executed
 *                                       in multiple times in PKCC.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaScalarMultBaseInit
 *  - R_PKCD_EddsaScalarMultBaseOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EDDSA_KEYGEN_INIT_DONE or #PKCL_STATE_EDDSA_KEYGEN_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - This API is called multiple times depending on the argument processing_bits.
 *      - (1) Perform EdDSA key generation scheme using the function specified in the HASH function pointer of p_ctx.\n
 *            Call R_PKCD_EddsaScalarMultBaseInit. \n
 *            Call R_PKCD_EddsaScalarMultBaseOperate. \n
 *            R_PKCD_EddsaScalarMultBase Operate returns to (2) if the calculation is in the middle,
 *             and returns normal completion if it finishes.
 *      - (2) Call R_PKCD_EddsaScalarMultBaseOperate. \n
 *            Repeat (2) when the calculation of R_PKCD_EddsaScalarMultBaseOperate is in the middle,
 *             and return normal completion when it finishes.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EDDSA_KEYGEN_INIT_DONE == pkcl_state then,
 *    -# Check the argument p_ctx, processing_bits with #r_pkcl_eddsa_chk_keygen_operate_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the EdDSA key generation operation as p_eddsa_keygen from p_ctx->p_buf.
 *      -# Set the scalar hash address hash_scalar_data.p_data to p_eddsa_keygen->hash_scalar_data.
 *      -# Set the scalar hash length hash_scalar_data.len to p_eddsa_keygen->hash_scalar_len.
 *      -# Set the scalar hash structure type hash_scalar_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *      -# Set the scalar hash structure content address hash_scalar_sel.p_sel_data to &hash_scalar_data.
 *      -# Assign a buffer for the driver's EdDSA key generation operation as p_buf_drv from p_ctx->p_buf.
 *      -# Initialize the multiplication of the base point by the scalar hash with R_PKCD_EddsaScalarMultBaseInit
 *         and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Operate the multiplication of the base point by the scalar hash with R_PKCD_EddsaScalarMultBaseOperate
 *           and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else if #PKCL_STATE_EDDSA_KEYGEN_OPERATE_PROCESSING == pkcl_state then,
 *    -# Set the arguments p_ctx, processing_bits with #r_pkcl_eddsa_chk_keygen_operate_arg and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the driver's EdDSA key generation operation as p_buf_drv from p_ctx->p_buf.
 *      -# Operate the multiplication of the base point by the scalar hash with R_PKCD_EddsaScalarMultBaseOperate
 *         and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EDDSA_KEYGEN_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# Else if #PKCL_RET_PROCESSING == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EDDSA_KEYGEN_OPERATE_PROCESSING. with #r_pkcl_cmn_set_state.
 *  -# Else if (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EddsaKeyGenOperate (st_pkc_eddsa_ctx_t * const p_ctx,
                                            const uint32_t processing_bits);

/**********************************************************************************************************************
* Function Name : R_PKCL_EddsaKeyGenFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Output the generated key pair.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in,out] p_priv_key     A pointer to a memory area allocated by the user SW to store the key binary
 *                                       of the EdDSA private key.
 * \param       [in,out] p_pub_key      A pointer to a memory area allocated by the user SW to store the key binary
 *                                       of the EdDSA public key.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaScalarBaseMultFinal
 *  - R_PKCD_EddsaKeyWrap
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EDDSA_KEYGEN_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Convert the raw key pair generated by calling R_PKCD_EddsaKeyWrap to the key binary format.
 *      - Output the generated key pair.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EDDSA_KEYGEN_OPERATE_DONE == pkcl_state then,
 *    -# Check the arguments p_ctx, p_priv_key, p_pub_key with #r_pkcl_eddsa_chk_keygen_final_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Set the public key byte array address pub_key_raw_data.p_data to pub_key_val.
 *      -# Set the public key byte array length pub_key_raw_data.len to sizeof(pub_key_val).
 *      -# Assign a buffer for the driver's EdDSA key generation operation as p_buf_drv from p_ctx->p_buf.
 *      -# Finalize the multiplication of the base point by the scalar hash with R_PKCD_EddsaScalarMultBaseFinal
 *         and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Assign a buffer for the EdDSA key generation operation as p_eddsa_keygen from p_ctx->p_buf.
 *        -# Set the private key byte array address priv_key_raw_data.p_data to p_eddsa_keygen->priv_scalar_data.
 *        -# If #PKCL_EDDSA_ED25519_CURVE_LEN == p_ctx->p_curve_info->curve_len, is true,
 *           set the private key byte array length priv_key_raw_data.len to #PKCL_EDDSA_ED25519_KEY_LEN,
 *           otherwise set the private key byte array length priv_key_raw_data.len to #PKCL_EDDSA_ED448_RU4_KEY_LEN.
 *        -# Format the key byte arrays as key binary with R_PKCD_EddsaKeyWrap and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *        -# Use #r_pkcl_cmn_set_byte to clear the private key value stored in p_buf.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EddsaKeyGenFinal (const st_pkc_eddsa_ctx_t * const p_ctx,
                                            void * const p_priv_key,
                                            void * const p_pub_key);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaScalarMultBaseInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Performs the initial processing of the EdDSA scalar multiplication, which is calculated using
 *               the base point of the curve.
 * \param       [in]     p_scalar_or_key    Scalar data structure \n
 *                                          Select and set EdDSA private key in byte array data or key binary format.
 * \param       [in,out] p_ctx              A pointer to a structure that summarizes the parameters required by EdDSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaScalarMultBaseInit
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the parameters required for EdDSA scalar multiplication calculation with the base point.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# If (NULL != p_scalar_or_key) && (NULL != p_ctx) then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_eddsa_chk_curve and set ret to the result.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Check if p_scalar_or_key is valid with #r_pkcl_eddsa_chk_select_data and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Initialize the scalar multiplication of the base point with R_PKCD_EddsaScalarMultBaseInit
 *         and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EDDSA_SMULTBASE_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEddsaScalarMultBaseInit (const st_pkc_select_data_t * const p_scalar_or_key,
                                                    st_pkc_eddsa_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaScalarMultBaseOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Perform EdDSA scalar multiplication.
 * \param       [in,out] p_ctx          A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in] processing_bits    Number of bits processed at one time when the operation is executed
 *                                       in multiple times in PKCC.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaScalarMultBaseOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EDDSA_SMULTBASE_INIT_DONE or #PKCL_STATE_EDDSA_SMULTBASE_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Call R_PKCD_EddsaScalarMultBaseOperate to perform EdDSA scalar multiplication with base point.
 * \par Remarks
 *  - It is necessary to call this API repeatedly until all the bits are processed. \n
 *    If there are unprocessed bits left, #PKCL_RET_PROCESSING is returned,
 *     and if all the bits are processed, #PKCL_RET_OK is returned. \n
 *    #PKCL_RET_STATUSERR is returned when the next Final function is called with unprocessed bits remaining.
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If (#PKCL_STATE_EDDSA_SMULTBASE_INIT_DONE == pkcl_state) ||
 *        (#PKCL_STATE_EDDSA_SMULTBASE_OPERATE_PROCESSING == pkcl_state)
 *    -# Check the arguments p_ctx, processing_bits with #r_pkcl_eddsa_chk_scalar_mult_operate_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Operate the scalar multiplication of the base point with R_PKCD_EddsaScalarMultBaseOperate
 *         and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EDDSA_SMULTBASE_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# Else if #PKCL_RET_PROCESSING == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EDDSA_SMULTBASE_OPERATE_PROCESSING with #r_pkcl_cmn_set_state.
 *  -# Else if (#PKCL_RET_PKCCTIMEOUTERR == ret) or (#PKCL_RET_PKCCERR == ret) or
 *             (#PKCL_RET_EDDSA_DECOMPERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEddsaScalarMultBaseOperate (st_pkc_eddsa_ctx_t * const p_ctx,
                                                        const uint32_t processing_bits);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaScalarMultBaseFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Output the result of EdDSA scalar multiplication.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in,out] p_point        A structure consisting of the byte length len of coordinate data multiplied
 *                                       by scalar and the data pointer p_data. \n
 *                                      Output data that concatenates x and y coordinates.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaScalarMultBaseFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EDDSA_SMULTBASE_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Output the result of EdDSA scalar multiplication.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EDDSA_SMULTBASE_OPERATE_DONE == pkcl_state then,
 *    -# If (NULL != p_ctx) && (NULL != p_point) then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_eddsa_chk_curve and set ret to the result.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Check the buffer for the resulting point with #r_pkcl_eddsa_chk_pkc_data and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Finalize the scalar multiplication of the base point with R_PKCD_EddsaScalarMultBaseFinal
 *         and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEddsaScalarMultBaseFinal (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                    const e_pkc_bool_t comp_flag,
                                                    st_pkc_data_t * const p_point);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaScalarMultBaseInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Performs the initial processing of the EdDSA scalar multiplication.
 * \param       [in]     p_scalar_or_key    Scalar data structure \n
 *                                          Select and set EdDSA private key in byte array data or key binary format.
 * \param       [in]     p_point_or_key     Coordinate data structure \n
 *                                          Byte array data or key Binary EdDSA public key or
 *                                           key containing public key Binary EdDSA private key.
 * \param       [in,out] p_ctx              A pointer to a structure that summarizes the parameters required by EdDSA.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaScalarMultInit
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the parameters required for EdDSA scalar multiplication calculation.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# If (NULL != p_scalar_or_key) && (NULL != p_point_or_key) && (NULL != p_ctx) then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_eddsa_chk_curve and set ret to the result.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Check if p_scalar_or_key is valid with #r_pkcl_eddsa_chk_select_data and set ret to the result.
 *          -# If #PKCL_RET_OK == ret then,
 *            -# Check if p_point_or_key is valid with #r_pkcl_eddsa_chk_select_data and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Initialize the scalar multiplication with R_PKCD_EddsaScalarMultInit and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state #PKCL_STATE_EDDSA_SMULT_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEddsaScalarMultInit (const st_pkc_select_data_t * const p_scalar_or_key,
                                                const st_pkc_select_data_t * const p_point_or_key,
                                                st_pkc_eddsa_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaScalarMultOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Perform EdDSA scalar multiplication.
 * \param       [in,out] p_ctx          A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in] processing_bits    Number of bits processed at one time when the operation is executed
 *                                       in multiple times in PKCC.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_EDDSA_DECOMPERR:          Failed to decompress the coordinates
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaScalarMultOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EDDSA_SMULT_INIT_DONE or #PKCL_STATE_EDDSA_SMULT_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Call R_PKCD_EddsaScalarMultBaseOperate to perform EdDSA scalar multiplication with base point.
 * \par Remarks
 *  - It is necessary to call this API repeatedly until all the bits are processed. \n
 *    If there are unprocessed bits left, #PKCL_RET_PROCESSING is returned,
 *     and if all the bits are processed, #PKCL_RET_OK is returned. \n
 *    #PKCL_RET_STATUSERR is returned when the next Final function is called with unprocessed bits remaining.
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If (#PKCL_STATE_EDDSA_SMULT_INIT_DONE == pkcl_state) ||
 *        (#PKCL_STATE_EDDSA_SMULT_OPERATE_PROCESSING == pkcl_state) then,
 *    -# Check the arguments p_ctx, processing_bits with #r_pkcl_eddsa_chk_scalar_mult_operate_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Operate the scalar multiplication with R_PKCD_EddsaScalarMultOperate and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EDDSA_SMULT_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# Else if #PKCL_RET_PROCESSING == ret then,
 *    -# Set the PKC state to #PKCL_STATE_EDDSA_SMULT_OPERATE_PROCESSING with #r_pkcl_cmn_set_state.
 *  -# Else if (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) ||
 *             (#PKCL_RET_EDDSA_DECOMPERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEddsaScalarMultOperate (st_pkc_eddsa_ctx_t * const p_ctx,
                                                    const uint32_t processing_bits);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaScalarMultFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Output the result of EdDSA scalar multiplication.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in,out] p_point        A structure consisting of the byte length len of coordinate data multiplied
 *                                       by scalar and the data pointer p_data. \n
 *                                      Output data that concatenates x and y coordinates.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaScalarMultFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_EDDSA_SMULT_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Output the result of EdDSA scalar multiplication.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_EDDSA_SMULT_OPERATE_DONE == pkcl_state then,
 *    -# If (NULL != p_ctx) && (NULL != p_point) then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_eddsa_chk_curve and set ret to the result.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Check if p_point is valid with #r_pkcl_eddsa_chk_pkc_data and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Finalize the scalar multiplication with R_PKCD_EddsaScalarMultFinal and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEddsaScalarMultFinal (const st_pkc_eddsa_ctx_t * const p_ctx,
                                                const e_pkc_bool_t comp_flag,
                                                st_pkc_data_t * const p_point);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaDeriveS
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Generate EdDSA signature value S from intermediate data.
 * \param       [in]     p_ctx              A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in]     p_scalar_s         A structure consisting of the byte length len of the integer s derived
 *                                           from the private key and the data pointer p_data.
 * \param       [in]     p_scalar_r         A structure consisting of the byte length len of the integer r derived
 *                                           from the integer s and the Hash value of the message,
 *                                           and the data pointer p_data.
 * \param       [in]     p_digest_r_a_m     A structure consisting of the signature value R, the public key A,
 *                                           the byte length len of the digest derived from the message,
 *                                           and the data pointer p_data.
 * \param       [in,out] p_sign_s           A structure consisting of the byte length len with the signature value S
 *                                           and the data pointer p_data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaDeriveS
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Generate EdDSA signature value S from intermediate data.
 *      - Supports edwards25519 and edwards448.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# If (NULL != p_ctx) && (NULL != p_scalar_s) && (NULL != p_scalar_r) &&
 *          (NULL != p_digest_r_a_m) && (NULL != p_sign_s) then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_eddsa_chk_curve and set ret to the result.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Check if p_scalar_s is valid with #r_pkcl_eddsa_chk_pkc_data and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Check if p_scalar_r is valid with #r_pkcl_eddsa_chk_pkc_data and set ret to the result..
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Check if p_digest_r_a_m is valid with #r_pkcl_eddsa_chk_pkc_data and set ret to the result..
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Check if p_sign_s is valid with #r_pkcl_eddsa_chk_pkc_data and set ret to the result.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Derive the signature s from scalar r and integer s and H(R,A,M) with R_PKCD_EddsaDeriveS
 *         and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEddsaDeriveS (const st_pkc_eddsa_ctx_t * const p_ctx,
                                        const st_pkc_data_t * const p_scalar_s,
                                        const st_pkc_data_t * const p_scalar_r,
                                        const st_pkc_data_t * const p_digest_r_a_m,
                                        st_pkc_data_t * const p_sign_s);

/**********************************************************************************************************************
* Function Name : R_PKCL_LLEddsaCompareSign
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLEDDSAAPIs
 * \brief       Verify EdDSA signature from intermediate data and signature value.
 * \param       [in]     p_ctx              A pointer to a structure that summarizes the parameters required by EdDSA.
 * \param       [in]     p_comp_point_ha    Compressed coordinates derived from {h x public key A}. \n
 *                                          * H stands for Hash (signature R || public key A || message)
 * \param       [in]     p_comp_point_sb    Compressed coordinates derived from {signature S x B}. \n
 *                                          * B is the base point
 * \param       [in]     p_sign_r           A structure consisting of the byte length len with the signature value R
 *                                           and the data pointer p_data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_EDDSA_DECOMPERR:          Failed to decompress the coordinates
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EddsaCompareSign
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Verify EdDSA signature from intermediate data and signature value.
 *      - Supports edwards25519 and edwards448.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# If (NULL != p_ctx) && (NULL != p_comp_point_ha) && (NULL != p_comp_point_sb) && (NULL != p_sign_r) then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_eddsa_chk_curve and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Check if p_comp_point_ha is valid with #r_pkcl_eddsa_chk_pkc_data and set ret to the result.
 *        -# If #PKCL_RET_OK == ret then,
 *          -# Check if p_comp_point_sb is valid with #r_pkcl_eddsa_chk_pkc_data and set ret to the result.
 *          -# If #PKCL_RET_OK == ret then,
 *            -# Check if p_sign_r is valid with #r_pkcl_eddsa_chk_pkc_data and set ret to the result.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Verify the signature comparing H(R,A,M)*A, S*B and the signature r with R_PKCD_EddsaCompareSign
 *         and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_LLEddsaCompareSign (const st_pkc_eddsa_ctx_t * const p_ctx,
                                            const st_pkc_data_t * const p_point_ha,
                                            const st_pkc_data_t * const p_point_sb,
                                            const st_pkc_data_t * const p_sign_r);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhXInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECDHXAPIs
 * \brief       Set the required parameters for ECDHX.
 * \param       [in]     p_priv_key    A pointer to the ECDHX private key in key binary format.
 * \param       [in]     p_pub_key     Key A pointer to the ECDHX public key in binary format.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by ECDHX.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdhXInit(When #PKCL_CFG_ENABLE_DRV_ECDHX == 1)
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the required parameters for ECDHX.
 *      - When performing KDF processing with ECDHX, set the KDF function pointer in p_ctx. \n
 *        If KDF processing is not performed, set NULL in p_ctx->p_kdf_info.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_ECDHX is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# Check the arguments p_priv_key, p_pub_key, p_ctx with #r_pkcl_ecdhx_chk_ecdhx_init_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the ECDHX operation as p_ecdhx from p_ctx->p_buf.
 *      -# Set the private key address p_ecdhx->p_priv_key to p_priv_key.
 *      -# Set the public key address p_ecdhx->p_pub_key to p_pub_key.
 *      -# Set the PKC state to #PKCL_STATE_ECDHX_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_ECDHX is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_ECDHX is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdhXInit (const void * const p_priv_key,
                                    const void * const p_pub_key,
                                    st_pkc_ecdhx_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhXOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECDHXAPIs
 * \brief       Perform ECDH key exchange.
 * \param       [in,out] p_ctx          A pointer to a structure that summarizes the parameters required by ECDHX.
 * \param       [in] processing_bits    Number of bits processed at one time when the operation is executed
 *                                      in multiple times in PKCC.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdhXOperate(When #PKCL_CFG_ENABLE_DRV_ECDHX == 1)
 *  - R_PKCD_EcdhXScalarMultInit
 *  - R_PKCD_EcdhXScalarMultOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_ECDHX_INIT_DONE or #PKCL_STATE_ECDHX_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Perform ECDHX key exchange.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_ECDHX is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_ECDHX_INIT_DONE == pkcl_state then,
 *    -# Check the arguments p_ctx, processing_bits with #r_pkcl_ecdhx_chk_ecdhx_operate_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the ECDHX operation as p_ecdhx from p_ctx->p_buf.
 *      -# Set the private key structure type privkey_sel.data_type to #PKC_DATA_TYPE_KEY_BINARY.
 *      -# Set the private key structure content address privkey_sel.p_sel_data to &privkey_data.
 *      -# Set the private key address privkey_sel.p_sel_data->p_data to (uint8_t*)p_ecdhx->p_priv_key.
 *      -# Set the public key structure type pubkey_sel.data_type = #PKC_DATA_TYPE_KEY_BINARY.
 *      -# Set the public key structure content address pubkey_sel.p_sel_data to &pubkey_data.
 *      -# Set the public key address pubkey_sel.p_sel_data->p_data to (uint8_t*)p_ecdhx->p_pub_key.
 *      -# Assign a buffer for the driver's ECDHX operation as p_buf_drv from p_ctx->p_buf.
 *      -# Initialize the scalar multiplication with R_PKCD_EcdhXScalarMultInit and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Operate the scalar multiplication with R_PKCD_EcdhXScalarMultOperate and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else if #PKCL_STATE_ECDHX_OPERATE_PROCESSING == pkcl_state then,
 *    -# Check the arguments p_ctx, processing_bits with #r_pkcl_ecdhx_chk_ecdhx_operate_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the driver's ECDHX operation as p_buf_drv from p_ctx->p_buf.
 *      -# Operate the scalar multiplication with R_PKCD_EcdhXScalarMultOperate and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_ECDHX_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# Else if #PKCL_RET_PROCESSING == ret then,
 *    -# Set the PKC state to #PKCL_STATE_ECDHX_OPERATE_PROCESSING with #r_pkcl_cmn_set_state.
 *  -# Else if (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_ECDHX is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_ECDHX is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdhXOperate (st_pkc_ecdhx_ctx_t * const p_ctx,
                                        const uint32_t processing_bits);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhXFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECDHXAPIs
 * \brief       If KDF processing is not performed, the shared secret is output, and if it is performed,
 *               the common key is output.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by ECDHX.
 * \param       [in,out] p_shared       A structure consisting of a shared secret or common key byte length len
 *                                       and a data pointer p_data.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdhXFinal(When #PKCL_CFG_ENABLE_DRV_ECDHX == 1)
 *  - R_PKCD_EcdhXScalarMultFinal
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_ECDHX_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - If KDF processing is not performed, the shared secret is output, and if it is performed,
 *         the common key is output.
 *      - If p_ctx->p_kdf_info is non-null, it may use the external KDF API. \n
 *        When using HW in the API of the external library, it is necessary to secure HW resources on the user side.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# <B>--- If #PKCL_CFG_ENABLE_DRV_ECDHX is not 1 ---</B>
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_ECDHX_OPERATE_DONE == pkcl_state then,
 *    -# Check the arguments p_ctx, p_shared with #r_pkcl_ecdhx_chk_ecdhx_final_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the driver's ECDHX operation as p_buf_drv from p_ctx->p_buf.
 *      -# Finalze the scalar multiplication with R_PKCD_EcdhXScalarMultFinal and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If (#PKCL_RET_OK == ret) && (NULL != p_ctx->p_kdf_info) then,
 *        -# Execute the key derivation function with p_ctx->p_kdf_info->p_kdf_api and set kdf_ret to the result.
 *        -# If #PKCL_EXTERNAL_API_OK == kdf_ret then,
 *          -# Set the shared secret length p_shared->len to p_ctx->p_kdf_info->out_kdf_len.
 *        -# Else
 *          -# Set ret to #PKCL_RET_KDFAPIERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 *  -# <B>--- Else #PKCL_CFG_ENABLE_DRV_ECDHX is not 1 ---</B>
 *  -# Not supported by  PKC-Lib_V.1.20
 *  -# <B>--- Endif #PKCL_CFG_ENABLE_DRV_ECDHX is not 1 ---</B>
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdhXFinal (const st_pkc_ecdhx_ctx_t * const p_ctx,
                                    st_pkc_data_t * const p_shared);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhXKeyGenInit
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECDHXAPIs
 * \brief       Set the parameters required for EdDSA key pair generation.
 * \param       [in,out] p_ctx         A pointer to a structure that summarizes the parameters required by ECDHX.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_NO_EXEC_OPERATION.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Set the parameters required for ECDHX key pair generation.
 *      - Use the DRBG function to generate the ECDHX private key.
 *      - This API uses an external Hash API and DRBG API. \n
 *        When using HW in the API of the external library, it is necessary to secure HW resources on the user side.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_NO_EXEC_OPERATION == pkcl_state then,
 *    -# If NULL != p_ctx then,
 *      -# If (NULL != p_ctx->p_buf) && (NULL != p_ctx->p_drbg_api) && (NULL != p_ctx->p_curve_info) then,
 *        -# Check the given curve structure with #r_pkcl_ecdhx_chk_curve and set ret to the result.
 *      -# Else
 *        -# Set ret to #PKCL_RET_PARAMERR.
 *    -# Else
 *      -# Set ret to #PKCL_RET_PARAMERR.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the ECDHX keygen operation as p_ecdhx_keygen from p_ctx->p_buf.
 *      -# Clear the buffer for the private scalar with p_ecdhx_keygen->priv_scalar_data.
 *      -# Set the private scalar length p_ecdhx_keygen->priv_scalar_len to p_ctx->p_curve_info->curve_len.
 *      -# Generate private scalar value with p_ctx->p_drbg_api and set external_ret to the result.
 *        -# If #PKCL_EXTERNAL_API_OK == external_ret then,
 *          -# Do nothing.
 *        -# Else
 *          -# Set ret to #PKCL_RET_DRBGAPIERR.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_ECDHX_KEYGEN_INIT_DONE with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdhXKeyGenInit (st_pkc_ecdhx_ctx_t * const p_ctx);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhXKeyGenOperate
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECDHXAPIs
 * \brief       Generate ECDHX key pair.
 * \param       [in,out] p_ctx          A pointer to a structure that summarizes the parameters required by ECDHX.
 * \param       [in] processing_bits    Number of bits processed at one time when the operation is executed
 *                                       in multiple times in PKCC.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_KEY_PARAMERR:             Data error in key binary format
 *  - #PKCL_RET_KEY_LENGTHERR:            Abnormal data length in key binary format
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdhxScalarMultInit
 *  - R_PKCD_EcdhxScalarMultOperate
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_ECDHX_KEYGEN_INIT_DONE or #PKCL_STATE_ECDHX_KEYGEN_OPERATE_PROCESSING.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Generate ECDHX key pair.
 *      - This API is called multiple times depending on the argument processing_bits.
 *      - (1) Call R_PKCD_EcdhXScalarMultInit. \n
 *            Call R_PKCD_EcdhXScalarMultOperate. \n
 *            R_PKCD_EcdhXScalarMultOperate Operate returns to (2) if the calculation is in the middle,
 *             and returns normal completion if it finishes.
 *      - (2) Call R_PKCD_EcdhXScalarMultOperate. \n
 *            Repeat (2) when the calculation of R_PKCD_EcdhXScalarMultOperate is in the middle,
 *             and return normal completion when it finishes.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_ECDHX_KEYGEN_INIT_DONE == pkcl_state then,
 *    -# Check the arguments p_ctx, processing_bits with #r_pkcl_ecdhx_chk_keygen_operate_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the ECDHX keygen operation as p_ecdhx_keygen from p_ctx->p_buf.
 *      -# Set the private scalar address scalar_data.p_data to p_ecdhx_keygen->priv_scalar_data.
 *      -# Set the private scalar length scalar_data.len to p_ecdhx_keygen->priv_scalar_len.
 *      -# Set the private scalar structure type scalar_sel.data_type to #PKC_DATA_TYPE_BYTE.
 *      -# Set the private scalar structure content address scalar_sel.p_sel_data to &scalar_data.
 *      -# Copy &p_ctx->p_curve_info->curve_params.p_data[p_ctx->p_curve_info->curve_len * #PKCL_ECDHX_CURVE_OFFSET_GU]
 *         to p_ecdhx_keygen->comp_point.
 *      -# Set the curve length p_ctx->p_curve_info->curve_len to p_ecdhx_keygen->comp_point_len.
 *      -# Change the endianness of the comp_point value with #r_pkcl_ecdhx_encode_data.
 *      -# Set the base point address comp_point_data.p_data to p_ecdhx_keygen->comp_point.
 *      -# Set the base point length comp_point_data.len to p_ecdhx_keygen->comp_point.
 *      -# Set the base point strucure type comp_point_sel.data_type = #PKC_DATA_TYPE_BYTE.
 *      -# Set the case point strucruee content address comp_point_sel.p_sel_data to &comp_point_data.
 *      -# Assign a buffer for the driver's ECDHX keygen operation as p_buf_drv from p_ctx->p_buf.
 *      -# Initialize the scalar multiplication with R_PKCD_EcdhXScalarMultInit and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Operate the scalar multiplication with R_PKCD_EcdhXScalarMultOperate and set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# Else if #PKCL_STATE_ECDHX_KEYGEN_OPERATE_PROCESSING == pkcl_state then,
 *    -# Check the arguments p_ctx, processing_bits with #r_pkcl_ecdhx_chk_keygen_operate_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Assign a buffer for the driver's ECDHX keygen operation as p_buf_drv from p_ctx->p_buf.
 *      -# Operate the scalar multiplication with R_PKCD_EcdhXScalarMultOperate and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *  -# If #PKCL_RET_OK == ret then,
 *    -# Set the PKC state to #PKCL_STATE_ECDHX_KEYGEN_OPERATE_DONE with #r_pkcl_cmn_set_state.
 *  -# Else if #PKCL_RET_PROCESSING == ret then,
 *    -# Set the PKC state to #PKCL_STATE_ECDHX_KEYGEN_OPERATE_PROCESSING with #r_pkcl_cmn_set_state.
 *  -# Else if (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdhXKeyGenOperate (st_pkc_ecdhx_ctx_t * const p_ctx,
                                            const uint32_t processing_bits);

/**********************************************************************************************************************
* Function Name : R_PKCL_EcdhXKeyGenFinal
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLECDHXAPIs
 * \brief       Output the generated key pair.
 * \param       [in]     p_ctx          A pointer to a structure that summarizes the parameters required by ECDHX.
 * \param       [in,out] p_priv_key     A pointer to a memory area allocated by the user SW to store the key binary
 *                                       of the ECDHX private key.
 * \param       [in,out] p_pub_key      A pointer to a memory area allocated by the user SW to store the key binary
 *                                       of the ECDHX public key.
 * \return
 *  - #PKCL_RET_OK:                       Successful completion
 *  - #PKCL_RET_INTERNALERR:              Internal processing failed
 *  - #PKCL_RET_PARAMERR:                 Invalid argument entered
 *  - #PKCL_RET_LENGTHERR:                Invalid argument length entered
 *  - #PKCL_RET_DRV_PARAMERR:             Invalid argument entered in PKC-Driver
 *  - #PKCL_RET_DRV_LENGTHERR:            Invalid argument length entered in PKC-Driver
 *  - #PKCL_RET_DRV_INTERNALERR:          Internal processing failed in PKC-Driver
 *  - #PKCL_RET_PKCCTIMEOUTERR:           PKCC timed out
 *  - #PKCL_RET_PKCCERR:                  An error occurred in PKCC
 *  - #PKCL_RET_TAMPERERR:                Tampering has been detected
 * \par Security components
 *  - No
 * \par Global Variables
 *  - #s_pkcl_state
 * \par Call PKC-Driver API
 *  - R_PKCD_EcdhXScalarBaseMultFinal
 *  - R_PKCD_EcdhXKeyWrap
 * \par Precondition
 *  - Satisfy both of the following conditions
 *    - PKC State is #PKCL_STATE_ECDHX_KEYGEN_OPERATE_DONE.
 *    - PKCC is not running.
 * \par Detailed description
 *      - Convert the raw key pair generated by calling R_PKCD_EcdhXKeyWrap to the key binary format.
 *      - Output the generated key pair.
 * \par Remarks
 *  - None
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_RET_INTERNALERR.
 *  -# Set pkcl_state to the current PKC state with #r_pkcl_cmn_get_state.
 *  -# If #PKCL_STATE_ECDHX_KEYGEN_OPERATE_DONE == pkcl_state then,
 *    -# Check the arguments p_ctx, p_priv_key, p_pub_key with #r_pkcl_ecdhx_chk_keygen_final_arg
 *       and set ret to the result.
 *    -# If #PKCL_RET_OK == ret then,
 *      -# Set the public key address pubkey_data.p_data to pub_key_val.
 *      -# Set the public key length pubkey_data.len to sizeof(pub_key_val).
 *      -# Assign a buffer for the driver's ECDHX keygen operation as p_buf_drv from p_ctx->p_buf.
 *      -# Finalize the scalar multiplication with R_PKCD_EcdhXScalarMultFinal and set drv_ret to the result.
 *      -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *      -# If #PKCL_RET_OK == ret then,
 *        -# Assign a buffer for the ECDHX keygen operation as p_ecdhx_keygen from p_ctx->p_buf.
 *        -# Set the private scalar address priv_scalar_data.p_data to p_ecdhx_keygen->priv_scalar_data.
 *        -# Set the private scalar length priv_scalar_data.len to p_ecdhx_keygen->priv_scalar_len.
 *        -# Format the key pair as the key binary format dedicated to the driver with R_PKCD_EddsaKeyWrap.
 *           And set drv_ret to the result.
 *        -# Convert drv_ret with #r_pkcl_cmn_pkcl_ret_from_pkcd_ret, and output the result to ret.
 *        -# Use #r_pkcl_cmn_set_byte to clear the private key value stored in p_buf.
 *  -# Else
 *    -# Set ret to #PKCL_RET_STATUSERR.
 *  -# If (#PKCL_RET_OK == ret) || (#PKCL_RET_PKCCTIMEOUTERR == ret) || (#PKCL_RET_PKCCERR == ret) then,
 *    -# Set the PKC state to #PKCL_STATE_NO_EXEC_OPERATION with #r_pkcl_cmn_set_state.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern pkcl_ret_t R_PKCL_EcdhXKeyGenFinal (const st_pkc_ecdhx_ctx_t * const p_ctx,
                                            void * const p_priv_key,
                                            void * const p_pub_key);

/**********************************************************************************************************************
* Function Name : R_PKCL_KdfSp80056c
**********************************************************************************************************************/
/*!****************************************************************************************************************//**
 * \ingroup     PKCLKDFAPIs
 * \brief       Check the argument of type st_pkc_select_data_t.
 * \param       [in]   p_kdf_ctx            A structure that summarizes the parameters required for KDF calculation.<br>
 *                                          Set the information in the variable of #st_pkcl_kdf_sp800_56c_ctx_t type<br>
 *                                          and set the address in this argument.
 *                                            - hash_type : Must be set.
 *                                            - p_hash_info : Must be set. Set the following.
 *                                              - p_hash_init_api   : Must be set.
 *                                              - p_hash_update_api : Must be set.
 *                                              - p_hash_final_api  : Must be set.
 *                                              - p_hash_ctx      : Set a pointer of any value when using with <br>
 *                                                                  HASH API, set NULL when not using with HASH API
 * \param       [in]   p_shared_secret      A shared secret
 * \param       [in]   shared_secret_len    Shared secret byte length
 * \param       [in]   out_len              Output byte length
 * \param       [out]  p_out_key            Address of the buffer that outputs the shared key
 * \return
 *  - 0x55555555:                       Successful completion
 *  - 0xAAAAAAAA:                       Processing failed
 * \par Security components
 *  - No
 * \par Global Variables
 *  - None
 * \par Call PKC-Driver API
 *  - None
 * \par Precondition
 *  - None
 * \par Detailed description
 *   - Derivation of common key from Shared Secret
 *   - In this API, SP800-56C (One-Step Key Derivation) auxiliary function option 1 (Hash function) <br>
 *     Supports SHA2-256 only
 * \par Remarks
 *  - This API is not an API called from User SW. <br>
 *    A function pointer is given to PKC-Lib by User SW, and it is an API called by PKC-Lib from that pointer.
 *
 * Pseudo-code for the function can be found below.
 *  -# Set ret to #PKCL_EXTERNAL_API_ERR.
 *  -# Check the arguments p_ctx, p_shared_secret, shared_secret_len, out_len, p_out_key
 *     with #r_pkcl_kdf_chk_sp800_56c_arg and set ret to the result.
 *  -# If #PKCL_EXTERNAL_API_OK == ret then,
 *    -# Get the digest length of the algorithm specified in p_ctx->hash_type with #r_pkcl_kdf_get_hash_len.
 *       And set hash_len to the length.
 *    -# If 0U != hash_len then,
 *      -# Set the hash information address p_hi to p_ctx->p_hash_info.
 *      -# Set the repetition number reps to out_len / hash_len.
 *      -# If 0U != (out_len % hash_len) then,
 *        -# reps++.
 *      -# Set counter to 0U.
 *      -# For (i = 0U; (i < reps) && (#PKCL_EXTERNAL_API_OK == ret); i++ ) do following,
 *        -# counter++.
 *      -# Change endianess of the counter into big endian with #r_pkcl_cmn_ch_big_endian32.
 *      -# Initialize the hash function with p_hi->p_hash_init_api and set ret to the result.
 *      -# If #PKCL_EXTERNAL_API_OK == ret then,
 *        -# Input the big endian counter value into the hash function with p_hi->p_hash_update_api.
 *           And set ret to the result.
 *        -# If #PKCL_EXTERNAL_API_OK == ret then,
 *          -# Input the shared secret into the hash function with p_hi->p_hash_update_api.
 *             And set ret to the result.
 *          -# If #PKCL_EXTERNAL_API_OK == ret then,
 *            -# Input the fixed info into the hash function with p_hi->p_hash_update_api.
 *               And set ret to the result.
 *               -# If #PKCL_EXTERNAL_API_OK == ret then,
 *                 -# Finalize the hash function with p_hi->p_hash_final_api.
 *      -# If #PKCL_EXTERNAL_API_OK == ret then,
 *        -# If ((reps - 1U) == i) && (0U != (out_len % hash_len)) then,
 *          -# Set cpy_len to out_len % hash_len.
 *        -# Else
 *          -# Set cpy_len to hash_len.
 *        -# Copy the derived key from md to p_out_key with #r_pkcl_cmn_cpy_byte.
 *    -# Set ret to #PKCL_EXTERNAL_API_ERR.
 *  -# Return ret.
 * \callgraph
 *********************************************************************************************************************/
extern uint32_t R_PKCL_KdfSp80056c (void * const p_kdf_ctx,
                                    const uint8_t * const p_shared_secret,
                                    const uint32_t shared_secret_len,
                                    const uint32_t out_len,
                                    uint8_t * const p_out_key);


#endif /* R_PKCL_API_H */
