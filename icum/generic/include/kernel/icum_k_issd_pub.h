/******************************************************************************/
/* Component Name   ISSD (ICU-M Start-up & Service Dispatcher)                */
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
#if !defined(ICUM_K_ISSD_PUB_H)
#define ICUM_K_ISSD_PUB_H

/**************************************************************************************/
/* Headers                                                                            */
/**************************************************************************************/
#include "r_icumif_api.h"
/* For a bit pattern indicating the channel number. */
#include "icum_d_comm_pub.h"

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* Return value definition */
#define ER_K_ISSD_JOB_NOT_FOUND         (ER_K_ISSD_BASE + (0x00000001uL))
#define ER_K_ISSD_REGIST_FULL           (ER_K_ISSD_BASE + (0x00000002uL))
#define ER_K_ISSD_MODE_REJECT           (ER_K_ISSD_BASE + (0x00000003uL))

/* ISSD max number of jobs */
#define ISSD_MAX_NB_OF_JOBS             (16)

/* Cancel request state definition */
#define JOB_CANCEL_NO_REQUEST           (0u)
#define JOB_CANCEL_REQUESTED            (1u)

/* Sanction ADMIN state definition */
#define SANCTION_ADMIN_OFF              (0u)
#define SANCTION_ADMIN_ON               (1u)

/* ECC Test state definition */
#define ISSD_ECC_TEST_OFF               (0u)
#define ISSD_ECC_TEST_ON                (1u)

/* Expansion service state definition */
#define FROM_NATIVE_SERVICE             (0u)
#define FROM_EXPANSION_SERVICE          (1u)

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* Type definition */
/***************************************************************************************
*  AES specific job parameters
*    - nb_blocks: number of remaining blocks to process
*    - nb_blocks_per_round: number of blocks processed per job round
*    - p_source: pointer to next block to process
*    - p_dest: pointer to next destination block
*    - aes_context: the HW AES context in use for that job
*    FOR XTS MODE:
*    - rem: number of bits to process in the last data block
***************************************************************************************/
typedef struct job_aes
{
    const uint32_t      *p_source;
    uint32_t            *p_dest;
    uint32_t            nb_blocks;
    uint16_t            nb_slice;
    uint8_t             rem;
    uint32_t            *get_context;
    uint8_t             get_key;
    uint8_t             warning;
    uint32_t            block_info;
} job_aes_t;

/***************************************************************************************
*  AES authentication cipher specific job parameters
*    - src_size_bits: number of remaining blocks to process
*    - auth_size_bits: number of remaining authentication blocks to process
*    - nb_blocks_per_round: number of blocks processed per job round
*    - p_auth_data: pointer to the next block of authentication data to process
*    - p_source: pointer to next block to process
*    - p_dest: pointer to next destination block
*    - aes_context: the HW AES context in use for that job
***************************************************************************************/
typedef struct job_aes_auth
{
    const uint32_t      *p_source;
    const uint32_t      *p_auth_data;
    uint32_t            *p_dest;
    uint32_t            src_size_bits;
    uint32_t            auth_size_bits;
    uint32_t            *get_context;
    uint32_t            block_info;
    uint32_t            j0[4];
    uint32_t            total_size[4];
    uint32_t            nb_ccm_p_blocks;
    uint32_t            *p_ccm_p_add;
    uint16_t            nb_slice;
    uint8_t             get_key;
    uint8_t             not_reverse;
    uint8_t             direction;
    uint8_t             ccm_cancel;
} job_aes_auth_t;

/***************************************************************************************
*  CMAC specific job parameters
*    - nb_blocks: number of blocks processed
*    - p_source: pointer to next block to process
*    - nb_slice: number of blocks in job slice
*    - n: number of blocks to process overall
*    - last_bit: number of bits to process in the last block
*    - aes_context: the HW AES context in use for that job
***************************************************************************************/
typedef struct job_cmac
{
    uint32_t            nb_blocks;
    const uint32_t      *p_source;
    uint16_t            nb_slice;
    uint8_t             last_bit;
    uint32_t            *get_context;
    uint8_t             get_key;
    uint16_t            block_info;
} job_cmac_t;

/**************************************************************************************
*  HASH specific job parameters
*    - nb_bytes: number of remaining bytes to process
*    - nb_bytes_per_round: number of bytes processed per job round
*    - p_source: pointer to next block to process
*    - n: number of bytes to process overall
*    - hash_context: the HASH context in use for that job
***************************************************************************************/
typedef struct job_hash
{
    volatile uint32_t   nb_bytes;
    const uint8_t       *p_source;
    uint16_t            nb_bytes_per_round;
    uint32_t            *p_hash_ctx;
    uint32_t            total_msg_size_in_bytes;
    uint32_t            processed_data_size;
} job_hash_t;

/***************************************************************************************
*  HMAC specific job parameters
*    - nb_bytes: number of remaining bytes to process
*    - nb_bytes_per_round: number of bytes processed per job round
*    - p_source: pointer to next block to process
*    - n: number of bytes to process overall
*    - hash_context: the HASH context in use for that job
***************************************************************************************/
typedef struct job_hmac
{
    volatile uint32_t   nb_bytes;
    const uint8_t       *p_source;
    uint32_t            *p_hash_ctx;
    uint16_t            nb_bytes_per_round;
} job_hmac_t;

/***************************************************************************************
*  RSA signature generation / verification job parameters
*    - p_key: a pointer to the key to use
*    - hash_context: the Hast context in use to calculate the hash over the msg
*    - p_hash_plain: the pointer to the hash of the message to verify
*    - hash_pre_calculated: set to 1 if the hash is already calculated
***************************************************************************************/
typedef struct job_rsa
{
    uint8_t             *p_key;
    uint32_t            *p_rsa_ctx;
    uint32_t            *p_hash_ctx;
    uint32_t            resource_ctrl;  /* bit0:RSA context, bit1:HASH context, bit2:rsa key */
    uint8_t             proc_type;      /* processing type */
    r_job_slice_t       job_slice;
    uint32_t            hash_source_index;
    uint32_t            hash_remain_bytes;
} job_rsa_t;

/**************************************************************************************
*  ECC job parameters
*    - p_priv_key: a pointer to the private key to use
*    - p_shared_key: a pointer to the shared key to use
*    - hash_type: hash algorithm to be used
***************************************************************************************/
typedef struct job_ecc
{
    uint8_t             *p_priv_key;
    uint8_t             *p_pub_key;
    uint32_t            *p_ecc_ctx;
    uint32_t            *p_hash_ctx;
    uint32_t            resource_ctrl;
    r_job_slice_t       job_slice;
    uint32_t            hash_source_index;
    uint32_t            hash_remain_bytes;
} job_ecc_t;

/***************************************************************************************
*  Memory cluster automatic verification parameters
*  (secure boot, background check)
*    - verif_context: the context in which the verification takes place
*                     (secure boot stage 1 or 2, background check)
***************************************************************************************/
typedef struct job_sys_config
{
    union job_configs
    {
            sys_cfg_key_mapping_t   key_mapping;
    } config_prm;
} job_sys_config_t;

/***************************************************************************************
*  AES update parameters
*    - key_group:
*    - key_id:
***************************************************************************************/
typedef struct job_aes_update
{
    r_key_group_t     key_group;
    r_key_index_t     key_id;
} job_aes_update_t;

/***************************************************************************************
***************************************************************************************/
typedef struct job_user_flash {
    uint8_t challenge_data[16];
} job_user_flash_t;

/***************************************************************************************
*  Generic job parameters
*    - p_ISD: a pointer to a valid service descriptor
*    - prm: specific service parameters
*    - job_id: the job ID in the job queue
*    - next_job_idx: index of next job in the table job_table[]
*    - prev_job_idx: index of previous job in the table job_table[]
***************************************************************************************/
typedef struct icum_job
{
    r_icumif_isd_t           *p_ISD;
    r_icumif_requester_id_t  requester_id;
    r_icumif_vm_id_t         vm_id;
    r_icumif_job_id_t        job_id;
    uint8_t                  nvm_warning;
    uint8_t                  priority;
    uint8_t                  process_phase;
    uint8_t                  cancel_request;
    uint8_t                  expansion_service;
    struct
    {
        uint8_t job_ghost:1;       /* set to 1 to keep the job image in the queue */
        uint8_t job_cycle:1;       /* set to 1 to keep the job image in the queue */
    } keep;
    union job_parameters
    {
        job_aes_t           job_aes;
        job_aes_auth_t      job_aes_auth;
        job_cmac_t          job_cmac;
        job_hash_t          job_hash;
        job_hmac_t          job_hmac;
        job_rsa_t           job_rsa;
        job_ecc_t           job_ecc;
        job_sys_config_t    job_sys_config;
        job_user_flash_t    job_user_flash;
        job_aes_update_t    job_aes_update;
    } prm;
} icum_job_t;

typedef struct icum_job_queue
{
    uint8_t                  next_job_idx;
    uint8_t                  prev_job_idx;
    uint8_t                  table_idx;
} icum_job_queue_t;



/***************************************************************************************
*  Pointer type to an ICU-M service function
***************************************************************************************/
typedef void (*p_service_func_t)(icum_job_t *p_job);

/***************************************************************************************
*  Patch service table
*    - service_id: service ID of patch service
*    - p_service_func: pointer of function for patch service
***************************************************************************************/
typedef struct
{
    r_icumif_service_id_t   service_id;
    p_service_func_t       *p_service_func;
} p_service_t;

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* ISSD execution mode change events */
#define ISSD_EVT_SYS_INIT_COMPLETED         1UL
#define ISSD_EVT_SYS_INIT_FAILED            2UL
#define ISSD_EVT_START_PE_COMPLETED         3UL
#define ISSD_EVT_START_PE_FAILED            4UL
#define ISSD_EVT_TRANSITION_TO_ROM          5UL
#define ISSD_EVT_ECC_TEST_START             10UL
#define ISSD_EVT_ECC_TEST_END               11UL
#define ISSD_EVT_MAX                        99UL

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
uint32_t K_ISSD_HandleRequest(icum_comm_int_from_channel_t channel, r_icumif_isd_t *p_isd);
uint32_t K_ISSD_GenerateServiceRequest(r_icumif_isd_t *p_isd);
uint32_t K_ISSD_AddService(r_icumif_isd_t *p_isd, r_icumif_job_id_t *p_job_id);
uint32_t K_ISSD_SendModeEvent(uint32_t event);
uint32_t K_ISSD_GetJobCount(uint32_t *p_count);
uint32_t K_ISSD_GetServiceID(r_icumif_job_id_t job_id, r_icumif_service_id_t *p_service_id);
uint32_t K_ISSD_SetCancelRequest(r_icumif_job_id_t cancel_job_id);
uint32_t K_ISSD_GetSanctionADMIN(uint8_t *p_state);
uint32_t K_ISSD_GetECCTestState(uint8_t *p_state);

void K_ISSD_StartVerifyAuto(void);

r_icumif_service_result_t start_internal_service(r_icumif_service_id_t service_id, r_icumif_isd_t *p_service, r_icumif_service_id_t from_service_id);
void K_ISSD_FastestCmacAddJobQue(icum_comm_int_from_channel_t channel, r_icumif_isd_t * p_isd);

#endif /* #if !defined(ICUM_K_ISSD_PUB_H) */
