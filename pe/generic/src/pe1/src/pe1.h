/******************************************************************************/
/* Component Name   Testing the ICU-M                                         */
/******************************************************************************/
/* Product      : ICU-M Firmware                                              */
/******************************************************************************/
/*******************************************************************************
* Copyright(C) 2021-2022 Renesas Electronics Corporation.                       
*                                                                               
*          RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY                     
*                                                                               
* These instructions, statements, and programs are the confidential information 
* of Renesas Electronics Corporation. They must be used and modified solely for 
* the purpose for which it was furnished by Renesas Electronics Corporation.    
* All part of them must not be reproduced nor disclosed to others in any form,  
* without the prior written permission of Renesas Electronics Corporation.      
*******************************************************************************/

#if !defined(PE1_H)
#define PE1_H

// ********************************************************************************
// Headers
// ********************************************************************************

#include "renesas_types.h"
#include "r_icumif_api.h"
#include "r_icumif_pub.h"

// ********************************************************************************
// Type definition
// ********************************************************************************

/*===========================================================================
  Test key type
  ===========================================================================*/
typedef struct
{
    char key_name[32];
    r_key_group_t key_group;
    r_key_index_t key_id;
} test_key_t;

/*===========================================================================
  Available commands
  ===========================================================================*/
enum eTEST_COMMANDS
{
    TEST_GENERATE_TRUE_RND,
    TEST_GENERATE_PSEUDO_RND,
    TEST_AES_ENCRYPTION,
    TEST_AES_DECRYPTION,
    TEST_GENERATE_CMAC
};

/*===========================================================================
  Job type
  ===========================================================================*/
enum eJOB_TYPE
{
    JOB_AES,
    JOB_RSA,
    JOB_ECC,
    JOB_H,
    NB_JOB_TYPE
};

/*===========================================================================
  Enum Types conversion table
  ===========================================================================*/
typedef struct{
    char            * str;
    int             e_num;
}cvt_enum_tbl;

/*===========================================================================
  Constant data address conversion table
  ===========================================================================*/
typedef struct{
    char            * str;
    unsigned int    addr;
}cvt_buff_tbl;

// ********************************************************************************
// Link to global variables
// ********************************************************************************

extern unsigned short job_slice[NB_JOB_TYPE];
extern int global_key_id;
extern int global_cipher_mode;
extern const test_key_t keys[];
#pragma alignvar (4)
extern const uint8_t LOREM_IPSUM[];
extern const uint8_t LOREM_IPSUM_HASH_SHA256[];
extern const uint8_t LOREM_IPSUM_HASH_SHA512[];
extern unsigned short vrun_chrono_sec;
extern unsigned short vrun_chrono_millisec;
extern unsigned short vrun_chrono_microsec;

extern bool  flg_menu_display;
extern bool  flg_service_err_off;

// ********************************************************************************
// global variables
// ********************************************************************************

// ********************************************************************************
// Prototypes
// ********************************************************************************
void system_init(void);

void pe_flash_setFLMDFPLOCK(uint32_t value);
void pe_flash_setFLMDCNT(uint32_t value);
void pe_flash_setFP0LOCKCfg(uint32_t value);
void pe_flash_set_flash_callbackfunc(void);
void pe_flash_initialize(void);
void print_throughput(int data_size);
char * get_cipher_mode_name(uint8_t cipher_mode);
void trigger_service_and_wait(r_icumif_isd_t *p_ISD);
void trigger_2service_and_wait(r_icumif_isd_t *p_ISD, r_icumif_isd_t *p_ISD2);
void trigger_3service_and_wait(r_icumif_isd_t *p_ISD, r_icumif_isd_t *p_ISD2, r_icumif_isd_t *p_ISD3);

void restart_life_signal(void);
void pause_life_signal(void);

void icum_key_update(void);

void flash_init(void);
void flash_test(void);

void change_nvm_keys(void);

void icum_import_my_rsa_key(r_icumif_vm_id_t vm_id);
void icum_test_expansion_service(void);
void icum_test_expansion_service_01(r_icumif_vm_id_t vm_id);
void icum_test_expansion_service_02(r_icumif_vm_id_t vm_id);
void icum_test_expansion_service_03(r_icumif_vm_id_t vm_id);
void icum_test_expansion_service_08(r_icumif_vm_id_t vm_id);
void icum_test_expansion_service_09(r_icumif_vm_id_t vm_id);
void icum_test_expansion_service_10(r_icumif_vm_id_t vm_id);
void icum_test_expansion_service_11(r_icumif_vm_id_t vm_id);
void icum_test_expansion_service_12(r_icumif_vm_id_t vm_id);
void icum_test_expansion_service_13(r_icumif_vm_id_t vm_id);
void icum_test_expansion_service_14(r_icumif_vm_id_t vm_id);
void icum_test_expansion_service_15(r_icumif_vm_id_t vm_id);

void icum_init_fcu_fw(void);

void icum_rsa_cert_import(uint8_t skip_verify);
void icum_rsa_key_pair_import(uint8_t skip_verify);

void show_submenu_for_pk_import(void);

void show_pk_import_sub_menu(void);

void icum_rsa_verif_pss(void);
void icum_rsa_key_generate(void);
void icum_rsa_sign_pss(void);
void icum_rsa_vefify_pss_2048(void);
void icum_rsa_verify_pss_3072(void);
void icum_rsa_verify_pss_2048_sha512(void);
void icum_rsa_sign_pkcs1_v15(void);
void icum_rsa_verify_pkcs1_v15(void);
void icum_rsa_encrypt_oaep(void);
void icum_rsa_encrypt_pkcs1v15(void);
void icum_rsa_dec_cipher_ex(int menuNo);
void icum_rsa_enc_and_dec(int menuNo);
void icum_rsa_verify_all_pss(void);
void icum_rsa_verify_all_pkcs1_v15(void);
/*void icum_rsa_sig_verif_ex(void);*/
void icum_rsa_verify_signature_ex(int n);
void icum_key_exlcution_controll(int n);
void icum_rsa_sign_and_verify_ex(int menuNo);

void show_submenu_for_pk_import_writeprotect(void);
/* Test Function for PK_IMPORT(ECC Key) */
void show_submenu_for_pk_import_ecc(void);
/* Test Function for PK_IMPORT_EXT */

void do_pk_import_A(int32_t count);
void do_pk_import_B(int32_t count);

void icum_ecc_key_generate(void);
void icum_ecdh(void);
void icum_ecdsa_sign(void);
void show_submenu_for_ecdsa_verify(void);
void icum_ecc_key_pair_import(void);

void icum_test_hash(void);
void icum_hash_time_test(void);

void icum_hmac_import(void);
void icum_hmac_import_test1(void);
void icum_hmac_import_test2(void);
void icum_hmac_import_test3(void);
void icum_hmac_import_test4(void);
void icum_hmac_import_test5(void);
void icum_hmac_import_test6(void);
void icum_hmac_import_test7(void);
void icum_hmac_import_test8(void);
void icum_hmac_import_test9(void);
void icum_hmac_import_test10(void);
void icum_hmac_import_test11(void);
void icum_hmac_import_test12(void);
void icum_hmac_import_test13(void);
void icum_hmac_import_test14(void);
void icum_hmac_import_test15(void);

void icum_hmac(void);

int  icum_self_test(void);
int  icum_self_test_fast(void);
int  icum_self_test_gcm(void);
int  icum_self_test_ccm(void);
int  icum_self_test_xts(void);
int  icum_self_test_xts_256(void);
void icum_test_multi_jobs(void);
int  cf_cmac_fast(void);

void icum_test_prng_init_rng(void);
void icum_test_prng_extend_seed(void);
void icum_test_prng_rnd(void);
void icum_test_prng_all(void);
void icum_test_dbg_act_prepare(void);
void icum_test_aes_cipher(int direction);
void icum_test_aes_cipher_in_cycle(void);
void icum_test_aes_cmac(void);
void icum_crypto_service_test(void);

void fw_timings(void);

void icum_garbage_collection_all(void);
void icum_garbage_collection_1block(void);

int icum_cancel_test(void);

void icum_init_clock_set(void);
void icum_get_clock_set(void);

void icum_init_startup_set(uint8_t sys_cfg_startup);
void icum_get_startup_set(void);

void icum_start_ecc_test(void);
void icum_end_ecc_test(void);
void icum_get_ecc_test_lock_status(void);
void icum_aes_cmac_test(void);
void icum_test_aes_auth_cipher(void);
void icum_key_update_test(void);
void icum_aes_cipher_test(void);
void icum_aes_key_cache(int remove);
void icum_rsa_enc_and_dec(int menuNo);
void icum_rsa_sign_and_verify_ex(int menuNo);
void icum_ecdh_setting(void);
void icum_ecdh_test(void);
void ecdsa_sign_nml(void);

//test for Li-auto
void ecdsa_sign_verify_test(void);
void eddsa_sign_verify_test(void);


void icum_hmac_import_test3(void);
void icum_hmac_import_test5(void);
void icum_hmac_import_test8(void);
void icum_hmac_import_test10(void);
void hash_26_byte(void);
void hmac_generation_test(void);
void hmac_verify_test(void);
void test_pk_export(void);
void test_pk_import_ecc_priv_nvm(void);
void measure_ecc(void);

/* test utilities */
char* getKeyGroupName(r_key_group_t key_group);
char* getVerifResultName(r_verif_result_t verif_result);
char* getNumberFormatName(r_number_format_t number_format);
char* getHashEncodingFormatName(r_hash_encoding_format_t hash_encoding_format);
char* getMessageEncodingFormatName(r_message_encoding_format_t message_encoding_format);
char* getHashPrimitiveName(r_hash_primitive_t hash_primitive);
char* getSigAlgoName(r_signature_algo_t sig_algo);
char* getEccCurveName(r_ecc_curve_t ecc_curve);
char* getKdfAlgoName(r_kdf_algo_t kdf_algo);

void icum_vbtest_call_SERVICE(char *p_str);
void icum_vbtest_call_MULTI_SERVICE(char *p_str);
int icum_vbtest_SetValue(char *p_str);
int icum_vbtest_SetSREG(char *p_str);
int icum_vbtest_SetUID(char *p_str);
int icum_vbtest_GetValue(char *p_str);
int icum_vbtest_GetValueBits(char *p_str);
int icum_vbtest_CompareTime(char *p_str);
int icum_vbtest_CalculateXor(char *p_str);
int icum_vbtest_ZeroFill(char *p_str);
int icum_vbtest_Compare(char *p_str);
int icum_vbtest_GetResult(char *p_str);
int icum_vbtest_GetJobID(char *p_str);
int icum_vbtest_MultiJobWait(char *p_str);
int icum_vbtest_setparam(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_01_AES_CIPHER(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_01_AES_AUTH_CIPHER(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_01_AES_CMAC(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_03_AES_KEY_UPDATE_PLAIN(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_call_ORG_SERVICE_JOB_SLICE_AND_CYCLE(char *p_str);
int icum_vbtest_setparam_SERVICE_01_RSA_VERIFY(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_01_RSA_SIGN(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_01_ECDSA_VERIFY(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_01_ECDSA_SIGN(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_01_EDDSA_VERIFY(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_01_EDDSA_SIGN(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_01_ECDH(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_01_HASH(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_01_HMAC(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_02_RAND_GENERATE(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_02_RAND_GENERATE_1(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_02_RAND_EXTEND_SEED(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_02_RAND_INIT_RNG(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_02_RAND_RND(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_03_PK_IMPORT(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_03_RSA_KEY_GENERATE(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_03_ECC_KEY_GENERATE(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_03_HMAC_IMPORT(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_04_ICUMDF_GC(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_01_RSA_ENCRYPT(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_setparam_SERVICE_01_RSA_DECRYPT(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_call_ORG_SERVICE_SHEVS_T3_4_13_MCT_0_9999(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_call_ORG_SERVICE_SHEVS_T3_5_11_MCT_0_9999(char *p_str, r_icumif_isd_t *p_ISD);
int icum_vbtest_call_ORG_SERVICE_SHEVS_T3_CANCEL_SHE(char *p_str);
int icum_vbtest_Fill_BB(char *p_str);
void prepare_mem_check(void);
int icum_vbtest_call_SERVICE_00_ECC_TEST(char *p_str, r_icumif_isd_t *p_ISD);
void icum_key_update_test(void);
void icum_key_update_test_key(void);
void icum_aes_cipher_test(void);
void icum_aes_cmac_test(void);
void icum_aes_cmac_test_JobCycle(void);
void icum_key_update_she_test(void);
void icum_ram_export_she_test(void);
void icum_key_update_she_test_BootMAC(void);
int icum_vbtest_call_ORG_SERVICE_PRIOLITY_TEST(char *p_str);
int icum_vbtest_call_ORG_SERVICE_RAND_NUM_CHECK(char *p_str);
int icum_vbtest_call_ORG_SERVICE_HEAT_RUN(char *p_str);
void icum_rand_test(int tid);
int icum_vbtest_call_MULTI_JOB_SERVICE_EXECUTE(char *p_str);
/* Flash Suspend/Resume test */
void memxor(uint8_t *dest, const uint8_t *source, uint16_t size);
int icum_vbtest_ReadSREG32(void);

void get_secreg_upgp1(void);
void get_secreg_upgp2(void);
void get_secreg_resv(void);
void get_secreg_rac(void);
void get_secreg_rad(void);
void get_secreg_pedbg(void);
void get_secreg_opcrf(void);
void get_secreg_opgpio(void);
void get_secreg_hsif(void);
void get_secreg_opwdvac(void);
void get_secreg_opwdrun(void);
void get_secreg_opwden(void);
void get_secreg_opwdovf(void);
void get_secreg_lram(void);
void get_secreg_pkcca(void);
void get_secreg_ng(void);
void set_secreg_upgp1_enable(void);
void set_secreg_upgp1_disable(void);
void set_secreg_upgp2_enable(void);
void set_secreg_upgp2_disnable(void);
void set_secreg_pedbg_enable(void);
void set_secreg_pedbg_disable(void);
void set_secreg_opcrf_all_enable(void);
void set_secreg_opcrf_all_skip(void);
void set_secreg_opgpio_enable(void);
void set_secreg_opgpio_disable(void);
void set_secreg_hsif_enable(void);
void set_secreg_hsif_disable(void);
void set_secreg_wdta_enable(void);
void set_secreg_wdta_disable(void);
void set_secreg_wdvac_ON(void);
void set_secreg_wdrun_ON(void);
void set_secreg_wddvf_ON(void);
void set_secreg_lram_ON(void);
void set_secreg_PKCCA_ON(void);
void set_secreg_upgp1_enable_NGvalue(void);
void set_secreg_upgp2_enable_NGvalue(void);
void set_secreg_pedbg_enable_NGvalue(void);
void set_secreg_opcrf_all_enable_NGvalue(void);
void set_secreg_opgpio_enable_NGvalue(void);
void set_secreg_hsif_enable_NGvalue(void);
void set_secreg_wdta_enable_NGvalue(void);
void set_secreg_resv_NGvalue(void);
void set_secreg_rac_NGvalue(void);
void set_secreg_rad_NGvalue(void);

#define EXEC_SERVICE_WITH_TIMESTAMP(p_ISD)  \
    vrun_chrono_start();                    \
    trigger_service_and_wait(p_ISD);        \
    vrun_chrono_stop();                     \
    vrun_chrono_gettimestamp();             \
    printf("Processed in : "); PCHRONO(); NL(); FLUSH()

void R_ICUMIF_Init_Reset(void);
void R_ICUMIF_Init_Test(void);
void R_ICUMIF_ServiceRequest_Test(void);
void R_ICUMIF_ServiceRequest_NoInterrupt_Test(void);
void R_ICUMIF_ServiceRequest_NoInterrupt_Test2(void);
void R_ICUMIF_ServiceCompleted_Error_Test(void);
void R_ICUMIF_ServiceCompleted_SoftError_Test(void);
void R_ICUMIF_ServiceCompleted_Test(void);
void R_ICUMIF_ServiceResponse_Test(void);
void R_ICUMIF_ICUM_Request_timeout_Test(void);
void R_ICUMIF_ServiceRequest_timeout_Test(void);
void R_ICUMIF_ServiceResponse_timeout_Test(void);
void R_ICUMIF_ServiceResponse_CallBack_Test(void);
void R_ICUMIF_ServiceResponse_Error_Test(void);
void R_ICUMIF_SetSystemCallBackFunc_Test(void);
void R_ICUMIF_SetFlashCallBackFunc_Test(void);
void Dummy_Semaphore_Free(void);
void Dummy_Response_Semaphore_Set(void);
void FACIACC_NotAcc_Service_Test(void);

char *R_ICUMIF_GetErrorCodeName(uint8_t error_code);

/* function for usecase test */
void faci_data_write(void);
void faci_block_erase(uint32_t addr,uint32_t size);
void faci_programing_data_flash(uint32_t addr,uint32_t wdata);

void faster_cmac_in_ram_execution(void);
unsigned int service_registration(r_icumif_isd_t *p_ISD[4]);

/* function for fast CMAC */
void fast_cmac_execution(void);
void faster_cmac_for_suspend_check (void);
void service_registration_for_fastCMAC(r_icumif_isd_t *p_ISD);
bool req_queue_exist_check(int check_mode);
void result_output_for_fastCMAC(r_icumif_isd_t *p_ISD,unsigned long vrun_stime, unsigned long vrun_etime, int service_num);
void vrun_iobuff_print_chrono_input_time(unsigned long vrun_stime, unsigned long vrun_etime);
void vrun_iobuff_print_timestamp_input_time(unsigned long vrun_time);
int get_semaphore(void);
void free_semaphore(void);
static void wait_milli_sec(int wait_msec);
static void wait_micro_sec(int wait_usec);

/* aes_parallel_procesing_check */
void aes_parallel_procesing_check (void);

/* Flash exclusive control */
void icum_fw_cf_read_mode( void );
void icum_fw_df_read_mode( void );
void icum_fw_securedf_read_mode( void );
void icum_fw_securedf_pe_mode( void );
void icum_host_cf_read_mode( void );
void icum_host_cf_pe_mode( void );
void icum_host_df_read_mode( void );
void icum_host_df_pe_mode( void );
void icum_notify_flash_interrupt( void );

void test_ecc_test_mode( void );
void test_cf_pe_by_faci( void );
void test_ram_execution( void );

void icum_aes_print_Out(uint32_t *p_uiOut, uint32_t uiSize);

/********************************************************************************
 Exported global functions (to be accessed by other files)
********************************************************************************/
void __SYNCI(void);
void __SYNCP(void);
void __SYNCM(void);

// ********************************************************************************
// ASM macros
// ********************************************************************************

/*===========================================================================
  Macros used for symbol concatenation
  ===========================================================================*/

#define PASTER(x, y)        x ## y
#define EVALUATOR(x, y)     PASTER(x, y)

/*===========================================================================
  Macro to put additional latency
  Inserts "n" NOPs where it is declared
  ===========================================================================*/
asm void R_ASM_NOPS(n) /* PRQA S 3002, 3408 */ /* ADC 1, 2*/
{
%con n
.rept n
    nop
.endr
}
asm void R_ASM_HALT()
{
    #ifndef DEBUG
    halt
    #else
    nop
    #endif
}

#define NOPS(x)     R_ASM_NOPS(x)
#define HALT()      R_ASM_HALT()

/* The Flash Operation completion wait flag is ON. */
#define FLS_WAIT_ON             0x00000001uL
/* The Flash Operation completion wait flag is OFF. */
#define FLS_WAIT_OFF            0x00000000uL

#define BIT_PE_BOOTCTRL_CPU0EN  0x00000001uL
#define BIT_PE_BOOTCTRL_CPU1EN  0x00000002uL
#define BIT_PE_BOOTCTRL_CPU2EN  0x00000004uL
#define BIT_PE_BOOTCTRL_CPU3EN  0x00000008uL
#define BIT_PE_BOOTCTRL_CPU4EN  0x00000010uL
#define BIT_PE_BOOTCTRL_CPU5EN  0x00000020uL

#endif //if !defined(PE1_H)

//ICUMIF_INIT_STATUS
extern uint8_t __ghsbegin_icumif_bss[];
#define ICMIF_INIT_FLG    (*(uint32_t*)__ghsbegin_icumif_bss)
