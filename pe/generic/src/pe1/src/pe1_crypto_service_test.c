#include <stdio.h>
#include <string.h>

#undef NULL
#include "r_icumif_api.h"
#include "r_icumif_pub.h"
#include "pe1.h"
#include "shared.h"
#include "sys.api.h"

#ifndef PE1_TEST_AES_KEY_SETUP
#define PE1_TEST_AES_KEY_SETUP 0
#endif
#ifndef PE1_TEST_AES_CIPHER
#define PE1_TEST_AES_CIPHER 0
#endif
#ifndef PE1_TEST_AES_AUTH
#define PE1_TEST_AES_AUTH 0
#endif
#ifndef PE1_TEST_AES_CMAC
#define PE1_TEST_AES_CMAC 0
#endif
#ifndef PE1_TEST_RSA_KEY_GENERATE
#define PE1_TEST_RSA_KEY_GENERATE 0
#endif
#ifndef PE1_TEST_RSA_CRYPT
#define PE1_TEST_RSA_CRYPT 0
#endif
#ifndef PE1_TEST_RSA_SIGNATURE
#define PE1_TEST_RSA_SIGNATURE 0
#endif

#if PE1_TEST_AES_KEY_SETUP || PE1_TEST_AES_CIPHER || PE1_TEST_AES_AUTH || PE1_TEST_AES_CMAC
#define TEST_AES_KEY_ID       (0u)
#endif
#if PE1_TEST_AES_KEY_SETUP || PE1_TEST_AES_CIPHER
#define TEST_AES_KEY_ID_2     (1u)
#endif
#if PE1_TEST_RSA_KEY_GENERATE || PE1_TEST_RSA_CRYPT || PE1_TEST_RSA_SIGNATURE
#define TEST_RSA_KEY_ID       (1u)
#define TEST_RSA_KEY_BITS     (1024u)
#endif
#if PE1_TEST_RSA_CRYPT || PE1_TEST_RSA_SIGNATURE
#define TEST_RSA_BUFFER_BITS  (4096u)
#define TEST_RSA_BUFFER_SIZE  (TEST_RSA_BUFFER_BITS / 8u)
#endif

#if PE1_TEST_AES_KEY_SETUP
#pragma alignvar(8)
static const uint8_t s_aes_key_0[16] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};
#pragma alignvar(8)
static const uint8_t s_aes_key_1[16] = {
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81
};
#endif
#if PE1_TEST_AES_CIPHER || PE1_TEST_AES_AUTH || PE1_TEST_AES_CMAC
#pragma alignvar(8)
static const uint8_t s_aes_iv[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
#pragma alignvar(8)
static const uint8_t s_aes_input[32] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51
};
#endif
#if PE1_TEST_AES_AUTH
#pragma alignvar(8)
static const uint8_t s_auth_data[16] = {
    0x50, 0x45, 0x2d, 0x49, 0x43, 0x55, 0x4d, 0x2d,
    0x74, 0x65, 0x73, 0x74, 0x2d, 0x41, 0x41, 0x44
};
#endif
#if PE1_TEST_RSA_CRYPT || PE1_TEST_RSA_SIGNATURE
#pragma alignvar(8)
static const uint8_t s_rsa_message[] = "ICUM RSA service test";
#endif

#if PE1_TEST_AES_CIPHER || PE1_TEST_AES_AUTH
#pragma alignvar(8)
static uint8_t s_ciphertext[32];
#pragma alignvar(8)
static uint8_t s_plaintext[32];
#endif
#if PE1_TEST_AES_AUTH || PE1_TEST_AES_CMAC
#pragma alignvar(8)
static uint8_t s_auth_tag[16];
#endif
#if PE1_TEST_RSA_CRYPT
#pragma alignvar(8)
static uint8_t s_rsa_ciphertext[TEST_RSA_BUFFER_SIZE];
#pragma alignvar(8)
static uint8_t s_rsa_plaintext[TEST_RSA_BUFFER_SIZE];
#endif
#if PE1_TEST_RSA_SIGNATURE
#pragma alignvar(8)
static uint8_t s_rsa_signature[TEST_RSA_BUFFER_SIZE];
#endif

#if PE1_TEST_AES_KEY_SETUP || PE1_TEST_AES_CIPHER || PE1_TEST_AES_AUTH || PE1_TEST_AES_CMAC || \
    PE1_TEST_RSA_KEY_GENERATE || PE1_TEST_RSA_CRYPT || PE1_TEST_RSA_SIGNATURE
static r_icumif_isd_t *test_isd(void)
{
    r_icumif_isd_t *p_isd = (r_icumif_isd_t *)ISD_BUFFER;
    memset(p_isd, 0, sizeof(*p_isd));
    return p_isd;
}
#endif

#if PE1_TEST_AES_KEY_SETUP || PE1_TEST_AES_CIPHER || PE1_TEST_AES_AUTH || PE1_TEST_AES_CMAC || \
    PE1_TEST_RSA_KEY_GENERATE || PE1_TEST_RSA_CRYPT || PE1_TEST_RSA_SIGNATURE
static int run_service(const char *name, r_icumif_isd_t *p_isd)
{
    uint32_t start_count;
    uint32_t elapsed_ticks;
    time_t elapsed_time;

    start_count = (uint32_t)sys_vtimer0_get_pclk_counter();
    trigger_service_and_wait(p_isd);
    elapsed_ticks = (uint32_t)sys_vtimer0_get_pclk_counter() - start_count;
    (void)sys_vtimer0_cnv_pclk2time((unsigned long long)elapsed_ticks, &elapsed_time);

    printf("%-24s %s, %lu.%03lu ms\n", name,
           (p_isd->service_result == SERV_OK) ? "PASS" : "FAIL",
            (unsigned long)(elapsed_time.sec * 1000UL + elapsed_time.msec),
           (unsigned long)elapsed_time.usec);
    if (p_isd->service_result != SERV_OK)
    {
        printf("  service_result: %s\n", R_ICUMIF_GetErrorCodeName(p_isd->service_result));
        return 0;
    }
    return 1;
}
#endif

#if PE1_TEST_AES_KEY_SETUP
static int setup_aes_key(r_key_index_t key_id, const uint8_t *p_key)
{
    r_icumif_isd_t *p_isd = test_isd();

    p_isd->service_id = SERVICE_03_AES_KEY_UPDATE_PLAIN;
    p_isd->prm.KEY_UPDATE_PLAIN.key_group = KEY_GRP_AES_RAM;
    p_isd->prm.KEY_UPDATE_PLAIN.key_id = key_id;
    p_isd->prm.KEY_UPDATE_PLAIN.key_size_in_bytes = sizeof(s_aes_key_0);
    p_isd->prm.KEY_UPDATE_PLAIN.p_key_value = p_key;
    p_isd->prm.KEY_UPDATE_PLAIN.flags.encryption = 1;
    p_isd->prm.KEY_UPDATE_PLAIN.flags.decryption = 1;
    p_isd->prm.KEY_UPDATE_PLAIN.flags.cmac_generation = 1;
    p_isd->prm.KEY_UPDATE_PLAIN.flags.cmac_verification = 1;
    return run_service((key_id == TEST_AES_KEY_ID) ? "AES RAM key 0" : "AES RAM key 1", p_isd);
}
#endif

#if PE1_TEST_AES_CIPHER
static int test_aes_cipher_mode(cipher_modes_t mode, const char *name)
{
    r_icumif_isd_t *p_isd;
    p_isd = test_isd();
    p_isd->service_id = SERVICE_01_AES_CIPHER;
    p_isd->prm.AES_CIPHER.direction = CIPHER_DIR_ENCRYPTION;
    p_isd->prm.AES_CIPHER.cipher_mode = mode;
    p_isd->prm.AES_CIPHER.key_group = KEY_GRP_AES_RAM;
    p_isd->prm.AES_CIPHER.key_id = TEST_AES_KEY_ID;
    p_isd->prm.AES_CIPHER.key_group_2 = KEY_GRP_AES_RAM;
    p_isd->prm.AES_CIPHER.key_id_2 = TEST_AES_KEY_ID_2;
    p_isd->prm.AES_CIPHER.p_iv = (const uint32_t *)s_aes_iv;
    p_isd->prm.AES_CIPHER.p_block_in = (const uint32_t *)s_aes_input;
    p_isd->prm.AES_CIPHER.p_block_out = (uint32_t *)s_ciphertext;
    if (mode == CIPHER_MOD_XTS)
    {
        p_isd->prm.AES_CIPHER.size.data_size_in_bits = sizeof(s_aes_input) * 8u;
    }
    else
    {
        p_isd->prm.AES_CIPHER.size.nb_blocks = 2u;
    }
    p_isd->prm.AES_CIPHER.job_slice = job_slice[JOB_AES];
    p_isd->prm.AES_CIPHER.job_cycle = JOB_ALL_AT_ONCE;
    if (!run_service(name, p_isd))
    {
        return 0;
    }

    p_isd = test_isd();
    p_isd->service_id = SERVICE_01_AES_CIPHER;
    p_isd->prm.AES_CIPHER.direction = CIPHER_DIR_DECRYPTION;
    p_isd->prm.AES_CIPHER.cipher_mode = mode;
    p_isd->prm.AES_CIPHER.key_group = KEY_GRP_AES_RAM;
    p_isd->prm.AES_CIPHER.key_id = TEST_AES_KEY_ID;
    p_isd->prm.AES_CIPHER.key_group_2 = KEY_GRP_AES_RAM;
    p_isd->prm.AES_CIPHER.key_id_2 = TEST_AES_KEY_ID_2;
    p_isd->prm.AES_CIPHER.p_iv = (const uint32_t *)s_aes_iv;
    p_isd->prm.AES_CIPHER.p_block_in = (const uint32_t *)s_ciphertext;
    p_isd->prm.AES_CIPHER.p_block_out = (uint32_t *)s_plaintext;
    if (mode == CIPHER_MOD_XTS)
    {
        p_isd->prm.AES_CIPHER.size.data_size_in_bits = sizeof(s_aes_input) * 8u;
    }
    else
    {
        p_isd->prm.AES_CIPHER.size.nb_blocks = 2u;
    }
    p_isd->prm.AES_CIPHER.job_slice = job_slice[JOB_AES];
    p_isd->prm.AES_CIPHER.job_cycle = JOB_ALL_AT_ONCE;
    if (!run_service(name, p_isd))
    {
        return 0;
    }
    if (memcmp(s_plaintext, s_aes_input, sizeof(s_aes_input)) != 0)
    {
        printf("  plaintext comparison failed\n");
        return 0;
    }
    return 1;
}
#endif

#if PE1_TEST_AES_AUTH
static int test_aes_auth_mode(auth_cipher_modes_t mode, const char *name)
{
    r_icumif_isd_t *p_isd = test_isd();

    memset(s_ciphertext, 0, sizeof(s_ciphertext));
    memset(s_plaintext, 0, sizeof(s_plaintext));
    memset(s_auth_tag, 0, sizeof(s_auth_tag));
    p_isd->service_id = SERVICE_01_AES_AUTH_CIPHER;
    p_isd->prm.AES_AUTH_CIPHER.p_iv = (const uint32_t *)s_aes_iv;
    p_isd->prm.AES_AUTH_CIPHER.iv_size_in_bits = (mode == AUTH_CIPHER_MOD_CCM) ? 96u : 128u;
    p_isd->prm.AES_AUTH_CIPHER.p_auth_data = (const uint32_t *)s_auth_data;
    p_isd->prm.AES_AUTH_CIPHER.auth_data_size_in_bits = sizeof(s_auth_data) * 8u;
    p_isd->prm.AES_AUTH_CIPHER.p_data_in = (const uint32_t *)s_aes_input;
    p_isd->prm.AES_AUTH_CIPHER.p_data_out = (uint32_t *)s_ciphertext;
    p_isd->prm.AES_AUTH_CIPHER.p_auth_tag = (uint32_t *)s_auth_tag;
    p_isd->prm.AES_AUTH_CIPHER.data_size_in_bits = 128u;
    p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits = 128u;
    p_isd->prm.AES_AUTH_CIPHER.key_group = KEY_GRP_AES_RAM;
    p_isd->prm.AES_AUTH_CIPHER.key_id = TEST_AES_KEY_ID;
    p_isd->prm.AES_AUTH_CIPHER.direction = CIPHER_DIR_ENCRYPTION;
    p_isd->prm.AES_AUTH_CIPHER.cipher_mode = mode;
    p_isd->prm.AES_AUTH_CIPHER.job_slice = job_slice[JOB_AES];
    p_isd->prm.AES_AUTH_CIPHER.job_cycle = JOB_ALL_AT_ONCE;
    if (!run_service(name, p_isd))
    {
        return 0;
    }

    p_isd = test_isd();
    p_isd->service_id = SERVICE_01_AES_AUTH_CIPHER;
    p_isd->prm.AES_AUTH_CIPHER.p_iv = (const uint32_t *)s_aes_iv;
    p_isd->prm.AES_AUTH_CIPHER.iv_size_in_bits = (mode == AUTH_CIPHER_MOD_CCM) ? 96u : 128u;
    p_isd->prm.AES_AUTH_CIPHER.p_auth_data = (const uint32_t *)s_auth_data;
    p_isd->prm.AES_AUTH_CIPHER.auth_data_size_in_bits = sizeof(s_auth_data) * 8u;
    p_isd->prm.AES_AUTH_CIPHER.p_data_in = (const uint32_t *)s_ciphertext;
    p_isd->prm.AES_AUTH_CIPHER.p_data_out = (uint32_t *)s_plaintext;
    p_isd->prm.AES_AUTH_CIPHER.p_auth_tag = (uint32_t *)s_auth_tag;
    p_isd->prm.AES_AUTH_CIPHER.data_size_in_bits = 128u;
    p_isd->prm.AES_AUTH_CIPHER.auth_tag_size_in_bits = 128u;
    p_isd->prm.AES_AUTH_CIPHER.key_group = KEY_GRP_AES_RAM;
    p_isd->prm.AES_AUTH_CIPHER.key_id = TEST_AES_KEY_ID;
    p_isd->prm.AES_AUTH_CIPHER.direction = CIPHER_DIR_DECRYPTION;
    p_isd->prm.AES_AUTH_CIPHER.cipher_mode = mode;
    p_isd->prm.AES_AUTH_CIPHER.job_slice = job_slice[JOB_AES];
    p_isd->prm.AES_AUTH_CIPHER.job_cycle = JOB_ALL_AT_ONCE;
    p_isd->prm.AES_AUTH_CIPHER.verification_result = VERIFICATION_FAIL;
    if (!run_service(name, p_isd))
    {
        return 0;
    }
    if ((p_isd->prm.AES_AUTH_CIPHER.verification_result != VERIFICATION_PASS) ||
        (memcmp(s_plaintext, s_aes_input, 16u) != 0))
    {
        printf("  authentication/decryption verification failed\n");
        printf("  authentication/decryption comparison failed\n");
        return 0;
    }
    return 1;
}
#endif

#if PE1_TEST_AES_CMAC
static int test_aes_cmac(void)
{
    r_icumif_isd_t *p_isd = test_isd();

    memset(s_auth_tag, 0, sizeof(s_auth_tag));
    p_isd->service_id = SERVICE_01_AES_CMAC;
    p_isd->prm.AES_CMAC.p_msg = (const uint32_t *)s_aes_input;
    p_isd->prm.AES_CMAC.p_cmac = (uint32_t *)s_auth_tag;
    p_isd->prm.AES_CMAC.msg_size_in_bits = sizeof(s_aes_input) * 8u;
    p_isd->prm.AES_CMAC.key_group = KEY_GRP_AES_RAM;
    p_isd->prm.AES_CMAC.key_id = TEST_AES_KEY_ID;
    p_isd->prm.AES_CMAC.cmac_size_in_bits = 128u;
    p_isd->prm.AES_CMAC.cmac_operation = CMAC_GENERATION;
    p_isd->prm.AES_CMAC.job_slice = job_slice[JOB_AES];
    p_isd->prm.AES_CMAC.job_cycle = JOB_ALL_AT_ONCE;
    if (!run_service("AES-CMAC generation", p_isd))
    {
        return 0;
    }

    p_isd = test_isd();
    p_isd->service_id = SERVICE_01_AES_CMAC;
    p_isd->prm.AES_CMAC.p_msg = (const uint32_t *)s_aes_input;
    p_isd->prm.AES_CMAC.p_cmac = (uint32_t *)s_auth_tag;
    p_isd->prm.AES_CMAC.msg_size_in_bits = sizeof(s_aes_input) * 8u;
    p_isd->prm.AES_CMAC.key_group = KEY_GRP_AES_RAM;
    p_isd->prm.AES_CMAC.key_id = TEST_AES_KEY_ID;
    p_isd->prm.AES_CMAC.cmac_size_in_bits = 128u;
    p_isd->prm.AES_CMAC.cmac_operation = CMAC_VERIFICATION;
    p_isd->prm.AES_CMAC.job_slice = job_slice[JOB_AES];
    p_isd->prm.AES_CMAC.job_cycle = JOB_ALL_AT_ONCE;
    p_isd->prm.AES_CMAC.verification_result = VERIFICATION_FAIL;
    if (!run_service("AES-CMAC verification", p_isd))
    {
        return 0;
    }
    if (p_isd->prm.AES_CMAC.verification_result != VERIFICATION_PASS)
    {
        printf("  AES-CMAC verification failed\n");
        return 0;
    }
    return 1;
}
#endif

#if PE1_TEST_RSA_CRYPT
static int test_rsa_crypt_mode(r_message_encoding_format_t mode, const char *name)
{
    r_icumif_isd_t *p_isd = test_isd();
    uint32_t message_size = sizeof(s_rsa_message) - 1u;
    uint16_t encrypted_size_in_bits;

    memset(s_rsa_ciphertext, 0, sizeof(s_rsa_ciphertext));
    memset(s_rsa_plaintext, 0, sizeof(s_rsa_plaintext));
    p_isd->service_id = SERVICE_01_RSA_ENCRYPT;
    p_isd->prm.RSA_ENCRYPT.p_message = s_rsa_message;
    p_isd->prm.RSA_ENCRYPT.message_size_in_bytes = message_size;
    p_isd->prm.RSA_ENCRYPT.p_encrypted_message = s_rsa_ciphertext;
    p_isd->prm.RSA_ENCRYPT.encrypted_message_size_in_bits = TEST_RSA_KEY_BITS;
    p_isd->prm.RSA_ENCRYPT.p_label = NULL;
    p_isd->prm.RSA_ENCRYPT.label_size_in_bytes = 0u;
    p_isd->prm.RSA_ENCRYPT.hash_primitive = HASH_PRIMITIVE_SHA1;
    p_isd->prm.RSA_ENCRYPT.message_encoding_format = mode;
    p_isd->prm.RSA_ENCRYPT.key_group = KEY_GRP_RSA;
    p_isd->prm.RSA_ENCRYPT.key_id = TEST_RSA_KEY_ID;
    p_isd->prm.RSA_ENCRYPT.encrypted_message_format = FORMAT_BINARY_PLAIN;
    p_isd->prm.RSA_ENCRYPT.job_slice_mod_exp = job_slice[JOB_RSA];
    if (!run_service(name, p_isd))
    {
        return 0;
    }
    encrypted_size_in_bits = p_isd->prm.RSA_ENCRYPT.encrypted_message_size_in_bits;

    p_isd = test_isd();
    p_isd->service_id = SERVICE_01_RSA_DECRYPT;
    p_isd->prm.RSA_DECRYPT.p_encrypted_message = s_rsa_ciphertext;
    p_isd->prm.RSA_DECRYPT.encrypted_message_size_in_bits = encrypted_size_in_bits;
    p_isd->prm.RSA_DECRYPT.p_message = s_rsa_plaintext;
    p_isd->prm.RSA_DECRYPT.message_size_in_bytes = TEST_RSA_BUFFER_SIZE;
    p_isd->prm.RSA_DECRYPT.p_label = NULL;
    p_isd->prm.RSA_DECRYPT.label_size_in_bytes = 0u;
    p_isd->prm.RSA_DECRYPT.hash_primitive = HASH_PRIMITIVE_SHA1;
    p_isd->prm.RSA_DECRYPT.message_encoding_format = mode;
    p_isd->prm.RSA_DECRYPT.key_id = TEST_RSA_KEY_ID;
    p_isd->prm.RSA_DECRYPT.encrypted_message_format = FORMAT_BINARY_PLAIN;
    p_isd->prm.RSA_DECRYPT.job_slice_mod_exp = job_slice[JOB_RSA];
    if (!run_service(name, p_isd))
    {
        return 0;
    }
    if ((p_isd->prm.RSA_DECRYPT.message_size_in_bytes != message_size) ||
        (memcmp(s_rsa_plaintext, s_rsa_message, message_size) != 0))
    {
        printf("  RSA plaintext comparison failed\n");
        return 0;
    }
    return 1;
}
#endif

#if PE1_TEST_RSA_SIGNATURE
static int test_rsa_signature_mode(r_hash_encoding_format_t mode, const char *name)
{
    r_icumif_isd_t *p_isd = test_isd();
    uint16_t salt_size = (mode == ENCODING_FORMAT_EMSA_PSS) ? 20u : 0u;
    uint16_t signature_size_in_bits;

    memset(s_rsa_signature, 0, sizeof(s_rsa_signature));
    p_isd->service_id = SERVICE_01_RSA_SIGN;
    p_isd->prm.RSA_SIGN.p_source = s_rsa_message;
    p_isd->prm.RSA_SIGN.p_signature = s_rsa_signature;
    p_isd->prm.RSA_SIGN.source_size_in_bytes = sizeof(s_rsa_message) - 1u;
    p_isd->prm.RSA_SIGN.hash_primitive = HASH_PRIMITIVE_SHA1;
    p_isd->prm.RSA_SIGN.hash_encoding_format = mode;
    p_isd->prm.RSA_SIGN.signature_format = FORMAT_BINARY_PLAIN;
    p_isd->prm.RSA_SIGN.key_id = TEST_RSA_KEY_ID;
    p_isd->prm.RSA_SIGN.job_slice_hash = job_slice[JOB_H];
    p_isd->prm.RSA_SIGN.job_slice_mod_exp = job_slice[JOB_RSA];
    p_isd->prm.RSA_SIGN.signature_size_in_bits = TEST_RSA_KEY_BITS;
    p_isd->prm.RSA_SIGN.salt_size_in_bytes = salt_size;
    if (!run_service(name, p_isd))
    {
        return 0;
    }
    signature_size_in_bits = p_isd->prm.RSA_SIGN.signature_size_in_bits;

    p_isd = test_isd();
    p_isd->service_id = SERVICE_01_RSA_VERIFY;
    p_isd->prm.RSA_VERIFY.p_source = s_rsa_message;
    p_isd->prm.RSA_VERIFY.p_signature = s_rsa_signature;
    p_isd->prm.RSA_VERIFY.source_size_in_bytes = sizeof(s_rsa_message) - 1u;
    p_isd->prm.RSA_VERIFY.signature_size_in_bits = signature_size_in_bits;
    p_isd->prm.RSA_VERIFY.salt_size_in_bytes = salt_size;
    p_isd->prm.RSA_VERIFY.hash_primitive = HASH_PRIMITIVE_SHA1;
    p_isd->prm.RSA_VERIFY.hash_encoding_format = mode;
    p_isd->prm.RSA_VERIFY.signature_format = FORMAT_BINARY_PLAIN;
    p_isd->prm.RSA_VERIFY.key_group = KEY_GRP_RSA;
    p_isd->prm.RSA_VERIFY.key_id = TEST_RSA_KEY_ID;
    p_isd->prm.RSA_VERIFY.job_slice_hash = job_slice[JOB_H];
    p_isd->prm.RSA_VERIFY.job_slice_mod_exp = job_slice[JOB_RSA];
    p_isd->prm.RSA_VERIFY.verification_result = VERIFICATION_FAIL;
    if (!run_service(name, p_isd))
    {
        return 0;
    }
    if (p_isd->prm.RSA_VERIFY.verification_result != VERIFICATION_PASS)
    {
        printf("  RSA signature verification failed\n");
        return 0;
    }
    return 1;
}
#endif

void icum_crypto_service_test(void)
{
#if !(PE1_TEST_AES_KEY_SETUP || PE1_TEST_AES_CIPHER || PE1_TEST_AES_AUTH || PE1_TEST_AES_CMAC || \
            PE1_TEST_RSA_KEY_GENERATE || PE1_TEST_RSA_CRYPT || PE1_TEST_RSA_SIGNATURE)
        printf("No AES/RSA service tests are enabled.\n");
#else
#if PE1_TEST_RSA_KEY_GENERATE
    r_icumif_isd_t *p_isd;
#endif
    int passed = 1;

    printf("\nICUM AES/RSA service tests (OSTM0 ticks to service return)\n");
#if PE1_TEST_AES_KEY_SETUP
    passed &= setup_aes_key(TEST_AES_KEY_ID, s_aes_key_0);
    passed &= setup_aes_key(TEST_AES_KEY_ID_2, s_aes_key_1);
#endif

#if PE1_TEST_AES_CIPHER
    passed &= test_aes_cipher_mode(CIPHER_MOD_ECB, "AES-ECB enc/dec");
    passed &= test_aes_cipher_mode(CIPHER_MOD_CBC, "AES-CBC enc/dec");
    passed &= test_aes_cipher_mode(CIPHER_MOD_CFB, "AES-CFB enc/dec");
    passed &= test_aes_cipher_mode(CIPHER_MOD_OFB, "AES-OFB enc/dec");
    passed &= test_aes_cipher_mode(CIPHER_MOD_CTR, "AES-CTR enc/dec");
    passed &= test_aes_cipher_mode(CIPHER_MOD_XTS, "AES-XTS enc/dec");
#endif
#if PE1_TEST_AES_AUTH
    passed &= test_aes_auth_mode(AUTH_CIPHER_MOD_GCM, "AES-GCM enc/dec");
    passed &= test_aes_auth_mode(AUTH_CIPHER_MOD_CCM, "AES-CCM enc/dec");
#endif
#if PE1_TEST_AES_CMAC
    passed &= test_aes_cmac();
#endif

#if PE1_TEST_RSA_KEY_GENERATE
    p_isd = test_isd();
    p_isd->service_id = SERVICE_03_RSA_KEY_GENERATE;
    p_isd->prm.RSA_KEY_GENERATE.key_id = TEST_RSA_KEY_ID;
    p_isd->prm.RSA_KEY_GENERATE.modulus_size_in_bits = TEST_RSA_KEY_BITS;
    if (!run_service("RSA 1024-bit keygen", p_isd) &&
        (p_isd->service_result != SERV_INVALID_KEY_INDEX))
    {
        passed = 0;
    }
    else if (p_isd->service_result == SERV_INVALID_KEY_INDEX)
    {
        printf("  existing RSA key reused\n");
    }
#endif
#if PE1_TEST_RSA_CRYPT
    passed &= test_rsa_crypt_mode(ENCODING_FORMAT_EME_OAEP, "RSA-OAEP enc/dec");
    passed &= test_rsa_crypt_mode(ENCODING_FORMAT_EME_PKCS1_V15, "RSA-PKCS1 enc/dec");
#endif
#if PE1_TEST_RSA_SIGNATURE
    passed &= test_rsa_signature_mode(ENCODING_FORMAT_EMSA_PSS, "RSA-PSS sign/verify");
    passed &= test_rsa_signature_mode(ENCODING_FORMAT_EMSA_PKCS1_V15, "RSA-PKCS1 sign/verify");
#endif

    printf("Crypto service test result: %s\n", passed ? "PASS" : "FAIL");
#endif
}
