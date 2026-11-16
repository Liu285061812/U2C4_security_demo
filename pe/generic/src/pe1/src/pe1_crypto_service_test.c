#include <stdio.h>
#include <string.h>

#undef NULL
#include "r_icumif_api.h"
#include "r_icumif_pub.h"
#include "pe1.h"
#include "shared.h"

#ifndef PE1_TEST_AES_CIPHER
#define PE1_TEST_AES_CIPHER 1
#endif
#ifndef PE1_TEST_AES_AUTH
#define PE1_TEST_AES_AUTH 1
#endif
#ifndef PE1_TEST_AES_CMAC
#define PE1_TEST_AES_CMAC 1
#endif
#ifndef PE1_TEST_RSA_KEY_GENERATE
#define PE1_TEST_RSA_KEY_GENERATE 1
#endif
#ifndef PE1_TEST_RSA_CRYPT
#define PE1_TEST_RSA_CRYPT 1
#endif
#ifndef PE1_TEST_RSA_SIGNATURE
#define PE1_TEST_RSA_SIGNATURE 1
#endif

#if PE1_TEST_AES_CIPHER || PE1_TEST_AES_AUTH || PE1_TEST_AES_CMAC
#define TEST_AES_KEY_ID       (0u)
#endif
#if PE1_TEST_AES_CIPHER
#define TEST_AES_KEY_ID_2     (3u)
#endif
#if PE1_TEST_RSA_KEY_GENERATE || PE1_TEST_RSA_CRYPT || PE1_TEST_RSA_SIGNATURE
#define TEST_RSA_KEY_ID       (0u)
#define TEST_RSA_KEY_BITS     (2048u)
#endif
#if PE1_TEST_RSA_CRYPT || PE1_TEST_RSA_SIGNATURE
#define TEST_RSA_BUFFER_BITS  (4096u)
#define TEST_RSA_BUFFER_SIZE  (TEST_RSA_BUFFER_BITS / 8u)
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
#if PE1_TEST_AES_CIPHER
static const struct {
    cipher_modes_t mode;
    uint8_t ciphertext[32];
} s_aes_cipher_expected[] = {
    { CIPHER_MOD_ECB, {
        0x47, 0xc5, 0x8d, 0x5e, 0x21, 0xca, 0xaf, 0x84, 0x0d, 0x01, 0x5b, 0x7d, 0x9b, 0x91, 0x09, 0x81,
        0x5c, 0x05, 0x1c, 0x31, 0xe4, 0xa7, 0x77, 0x74, 0x7c, 0x38, 0xeb, 0xa4, 0xdc, 0x62, 0xe0, 0x73
    } },
    { CIPHER_MOD_CBC, {
        0xe4, 0xef, 0x93, 0xeb, 0x8e, 0xf9, 0xa7, 0x42, 0x47, 0x09, 0xf8, 0xea, 0xa9, 0x53, 0x45, 0x0e,
        0xe7, 0x47, 0x18, 0xc9, 0xbf, 0x0a, 0x2a, 0x6d, 0xcc, 0xc4, 0x06, 0xd6, 0x86, 0xf4, 0xfc, 0xe8
    } },
    { CIPHER_MOD_CFB, {
        0x61, 0x55, 0xb5, 0x57, 0x6f, 0x2e, 0x6f, 0xd3, 0x18, 0xfe, 0xea, 0x49, 0xb5, 0xc0, 0xfd, 0x70,
        0xbe, 0x25, 0xfd, 0x71, 0x47, 0xa3, 0x87, 0x60, 0x66, 0xe7, 0x95, 0x05, 0x30, 0x56, 0x2a, 0x9b
    } },
    { CIPHER_MOD_OFB, {
        0x61, 0x55, 0xb5, 0x57, 0x6f, 0x2e, 0x6f, 0xd3, 0x18, 0xfe, 0xea, 0x49, 0xb5, 0xc0, 0xfd, 0x70,
        0x00, 0xca, 0x94, 0xf2, 0x5f, 0xd4, 0x02, 0xd7, 0x75, 0xd7, 0xd1, 0x60, 0x1c, 0x90, 0x38, 0x32
    } },
    { CIPHER_MOD_CTR, {
        0x61, 0x55, 0xb5, 0x57, 0x6f, 0x2e, 0x6f, 0xd3, 0x18, 0xfe, 0xea, 0x49, 0xb5, 0xc0, 0xfd, 0x70,
        0xac, 0x4e, 0x66, 0xc3, 0x78, 0x1b, 0xde, 0x0a, 0x04, 0x6d, 0x92, 0xa3, 0x0e, 0x0b, 0x81, 0x8d
    } },
    { CIPHER_MOD_XTS, {
        0xfd, 0x29, 0x6f, 0x99, 0xb8, 0x7f, 0x29, 0x27, 0xed, 0xb5, 0xa7, 0xb3, 0x83, 0x75, 0x1d, 0xd8,
        0x4b, 0xd6, 0x1d, 0x86, 0xda, 0xec, 0x38, 0x1d, 0x9f, 0x67, 0xb1, 0x46, 0x80, 0xd4, 0xd6, 0x7e
    } }
};
#endif
#if PE1_TEST_AES_AUTH
#pragma alignvar(8)
static const uint8_t s_auth_data[16] = {
    0x50, 0x45, 0x2d, 0x49, 0x43, 0x55, 0x4d, 0x2d,
    0x74, 0x65, 0x73, 0x74, 0x2d, 0x41, 0x41, 0x44
};
static const struct {
    auth_cipher_modes_t mode;
    uint8_t ciphertext[16];
    uint8_t tag[16];
} s_aes_auth_expected[] = {
    { AUTH_CIPHER_MOD_GCM,
      { 0xc1, 0x69, 0x2d, 0x7c, 0xb7, 0xc2, 0x56, 0x55, 0x18, 0x85, 0x69, 0x7f, 0xf7, 0xec, 0xbf, 0xd5 },
      { 0x4b, 0xd7, 0x11, 0xe3, 0x1d, 0x01, 0x77, 0x20, 0x07, 0xaf, 0x1f, 0x56, 0x8d, 0x36, 0x81, 0x17 } },
    { AUTH_CIPHER_MOD_CCM,
      { 0x58, 0xd4, 0x4d, 0x85, 0xf2, 0xc0, 0x5b, 0x27, 0x98, 0x2e, 0xb7, 0xf1, 0x71, 0x79, 0x92, 0xaa },
      { 0x52, 0x97, 0x08, 0xe2, 0xa8, 0xc3, 0x82, 0x7d, 0x88, 0xb0, 0x54, 0xf8, 0x01, 0x6b, 0x2a, 0x68 } }
};
#endif
#if PE1_TEST_RSA_CRYPT || PE1_TEST_RSA_SIGNATURE
#pragma alignvar(8)
static const uint8_t s_rsa_message[] = "ICUM RSA service test";
#endif

#define s_ciphertext       BIG_BUFFER[1]
#define s_plaintext        BIG_BUFFER[0]
#define s_auth_tag         (BIG_BUFFER[2] + 32u)

static r_icumif_isd_t *test_isd(void)
{
    r_icumif_isd_t *p_isd = (r_icumif_isd_t *)ISD_BUFFER;
    memset(p_isd, 0, sizeof(*p_isd));
    return p_isd;
}

static int run_service(const char *name, r_icumif_isd_t *p_isd)
{
    trigger_service_and_wait(p_isd);

    printf("%-24s result=%s\n", name, R_ICUMIF_GetErrorCodeName(p_isd->service_result));
    if (p_isd->service_result != SERV_OK)
    {
        return 0;
    }
    printf("  service status: PASS\n");
    return 1;
}

static void print_test_result(const char *name, int result)
{
    printf("%-24s return=%d (%s)\n", name, result, result ? "PASS" : "FAIL");
}

#if PE1_TEST_AES_CIPHER || PE1_TEST_AES_AUTH
static int check_aes_output(const char *name, const char *output_name,
                            const uint8_t *actual, const uint8_t *expected, size_t size)
{
    size_t i;

    for (i = 0u; i < size; i++)
    {
        if (actual[i] != expected[i])
        {
            printf("  %s %s: FAIL at byte %lu (actual=%02X expected=%02X)\n",
                   name, output_name, (unsigned long)i,
                   (unsigned int)actual[i], (unsigned int)expected[i]);
            return 0;
        }
    }
    printf("  %s %s: PASS\n", name, output_name);
    return 1;
}
#endif

#if PE1_TEST_AES_CIPHER
static int test_aes_cipher_mode(cipher_modes_t mode, const char *name)
{
    r_icumif_isd_t *p_isd;
    size_t vector_index;

    for (vector_index = 0u;
         vector_index < sizeof(s_aes_cipher_expected) / sizeof(s_aes_cipher_expected[0]);
         vector_index++)
    {
        if (s_aes_cipher_expected[vector_index].mode == mode)
        {
            break;
        }
    }
    if (vector_index == sizeof(s_aes_cipher_expected) / sizeof(s_aes_cipher_expected[0]))
    {
        printf("  %s: no expected ciphertext for mode %d\n", name, (int)mode);
        return 0;
    }
    memcpy(BIG_BUFFER[0], s_aes_input, sizeof(s_aes_input));
    memcpy(BIG_BUFFER[2], s_aes_iv, sizeof(s_aes_iv));
    p_isd = test_isd();
    p_isd->service_id = SERVICE_01_AES_CIPHER;
    p_isd->prm.AES_CIPHER.direction = CIPHER_DIR_ENCRYPTION;
    p_isd->prm.AES_CIPHER.cipher_mode = mode;
    p_isd->prm.AES_CIPHER.key_group = KEY_GRP_AES_RAM;
    p_isd->prm.AES_CIPHER.key_id = TEST_AES_KEY_ID;
    p_isd->prm.AES_CIPHER.key_group_2 = KEY_GRP_AES_RAM;
    p_isd->prm.AES_CIPHER.key_id_2 = TEST_AES_KEY_ID_2;
    p_isd->prm.AES_CIPHER.p_iv = (const uint32_t *)BIG_BUFFER[2];
    p_isd->prm.AES_CIPHER.p_block_in = (const uint32_t *)BIG_BUFFER[0];
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
    if (!check_aes_output(name, "ciphertext", s_ciphertext,
                          s_aes_cipher_expected[vector_index].ciphertext, sizeof(s_aes_input)))
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
    p_isd->prm.AES_CIPHER.p_iv = (const uint32_t *)BIG_BUFFER[2];
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
    if (memcmp(BIG_BUFFER[0], s_aes_input, sizeof(s_aes_input)) != 0)
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
    r_icumif_isd_t *p_isd;
    size_t vector_index;

    for (vector_index = 0u;
         vector_index < sizeof(s_aes_auth_expected) / sizeof(s_aes_auth_expected[0]);
         vector_index++)
    {
        if (s_aes_auth_expected[vector_index].mode == mode)
        {
            break;
        }
    }
    if (vector_index == sizeof(s_aes_auth_expected) / sizeof(s_aes_auth_expected[0]))
    {
        printf("  %s: no expected ciphertext for mode %d\n", name, (int)mode);
        return 0;
    }
    p_isd = test_isd();
    memset(BIG_BUFFER[0], 0, 32u);
    memset(BIG_BUFFER[1], 0, 32u);
    memset(BIG_BUFFER[2], 0, 48u);
    memcpy(BIG_BUFFER[0], s_aes_input, sizeof(s_aes_input));
    memcpy(BIG_BUFFER[2], s_aes_iv, sizeof(s_aes_iv));
    memcpy(BIG_BUFFER[2] + 16u, s_auth_data, sizeof(s_auth_data));
    p_isd->service_id = SERVICE_01_AES_AUTH_CIPHER;
    p_isd->prm.AES_AUTH_CIPHER.p_iv = (const uint32_t *)BIG_BUFFER[2];
    p_isd->prm.AES_AUTH_CIPHER.iv_size_in_bits = (mode == AUTH_CIPHER_MOD_CCM) ? 96u : 128u;
    p_isd->prm.AES_AUTH_CIPHER.p_auth_data = (const uint32_t *)(BIG_BUFFER[2] + 16u);
    p_isd->prm.AES_AUTH_CIPHER.auth_data_size_in_bits = sizeof(s_auth_data) * 8u;
    p_isd->prm.AES_AUTH_CIPHER.p_data_in = (const uint32_t *)BIG_BUFFER[0];
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
    if (!check_aes_output(name, "ciphertext", s_ciphertext,
                          s_aes_auth_expected[vector_index].ciphertext,
                          sizeof(s_aes_auth_expected[vector_index].ciphertext)))
    {
        return 0;
    }
    if (!check_aes_output(name, "auth tag", s_auth_tag,
                          s_aes_auth_expected[vector_index].tag,
                          sizeof(s_aes_auth_expected[vector_index].tag)))
    {
        return 0;
    }

    p_isd = test_isd();
    p_isd->service_id = SERVICE_01_AES_AUTH_CIPHER;
    p_isd->prm.AES_AUTH_CIPHER.p_iv = (const uint32_t *)BIG_BUFFER[2];
    p_isd->prm.AES_AUTH_CIPHER.iv_size_in_bits = (mode == AUTH_CIPHER_MOD_CCM) ? 96u : 128u;
    p_isd->prm.AES_AUTH_CIPHER.p_auth_data = (const uint32_t *)(BIG_BUFFER[2] + 16u);
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
        (memcmp(BIG_BUFFER[0], s_aes_input, 16u) != 0))
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

    memcpy(BIG_BUFFER[0], s_aes_input, sizeof(s_aes_input));
    memset(BIG_BUFFER[2] + 32u, 0, 16u);
    p_isd->service_id = SERVICE_01_AES_CMAC;
    p_isd->prm.AES_CMAC.p_msg = (const uint32_t *)BIG_BUFFER[0];
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
    p_isd->prm.AES_CMAC.p_msg = (const uint32_t *)BIG_BUFFER[0];
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
static int test_rsa_crypt_mode(r_message_encoding_format_t mode, const char *name,
                               r_icumif_service_id_t selected_service)
{
    r_icumif_isd_t *p_isd = test_isd();
    uint32_t message_size = sizeof(s_rsa_message) - 1u;
    uint16_t encrypted_size_in_bits;

    memset(BIG_BUFFER[1], 0, SIZE_OF_BIG_BUFFER);
    memset(BIG_BUFFER[2], 0, SIZE_OF_BIG_BUFFER);
    memcpy(BIG_BUFFER[0], s_rsa_message, message_size);
    p_isd->service_id = SERVICE_02_RSA_ENCRYPT;
    p_isd->prm.RSA_ENCRYPT.p_message = BIG_BUFFER[0];
    p_isd->prm.RSA_ENCRYPT.message_size_in_bytes = message_size;
    p_isd->prm.RSA_ENCRYPT.p_encrypted_message = BIG_BUFFER[1];
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
    if (selected_service == SERVICE_02_RSA_ENCRYPT)
    {
        return 1;
    }

    p_isd = test_isd();
    p_isd->service_id = SERVICE_02_RSA_DECRYPT;
    p_isd->prm.RSA_DECRYPT.p_encrypted_message = BIG_BUFFER[1];
    p_isd->prm.RSA_DECRYPT.encrypted_message_size_in_bits = encrypted_size_in_bits;
    p_isd->prm.RSA_DECRYPT.p_message = BIG_BUFFER[2];
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
        (memcmp(BIG_BUFFER[2], s_rsa_message, message_size) != 0))
    {
        printf("  RSA plaintext comparison failed\n");
        return 0;
    }
    return 1;
}
#endif

#if PE1_TEST_RSA_SIGNATURE
static int test_rsa_signature_mode(r_hash_encoding_format_t mode, const char *name,
                                   r_icumif_service_id_t selected_service)
{
    r_icumif_isd_t *p_isd = test_isd();
    uint16_t salt_size = (mode == ENCODING_FORMAT_EMSA_PSS) ? 20u : 0u;
    uint16_t signature_size_in_bits;

    memset(BIG_BUFFER[1], 0, SIZE_OF_BIG_BUFFER);
    memcpy(BIG_BUFFER[0], s_rsa_message, sizeof(s_rsa_message) - 1u);
    p_isd->service_id = SERVICE_02_RSA_SIGN;
    p_isd->prm.RSA_SIGN.p_source = BIG_BUFFER[0];
    p_isd->prm.RSA_SIGN.p_signature = BIG_BUFFER[1];
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
    if (selected_service == SERVICE_02_RSA_SIGN)
    {
        return 1;
    }

    p_isd = test_isd();
    p_isd->service_id = SERVICE_02_RSA_VERIFY;
    p_isd->prm.RSA_VERIFY.p_source = BIG_BUFFER[0];
    p_isd->prm.RSA_VERIFY.p_signature = BIG_BUFFER[1];
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

static int run_rng_init(void)
{
    r_icumif_isd_t *p_isd = test_isd();
    p_isd->service_id = SERVICE_03_RAND_INIT_RNG;
    p_isd->prm.RAND_INIT.online_test = RAND_DIAG_P1_TEST;
    return run_service("RNG initialize", p_isd);
}

static int run_rng_extend_seed(void)
{
    static const uint8_t seed[16] = {
        0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
        0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51
    };
    r_icumif_isd_t *p_isd;

    memcpy(BIG_BUFFER[0], seed, sizeof(seed));
    p_isd = test_isd();
    p_isd->service_id = SERVICE_03_RAND_EXTEND_SEED;
    p_isd->prm.RAND_EXTEND_SEED.p_entropy = (uint32_t *)BIG_BUFFER[0];
    return run_service("RNG extend seed", p_isd);
}

static int run_rng_generate(void)
{
    r_icumif_isd_t *p_isd = test_isd();
    p_isd->service_id = SERVICE_03_RAND_RND;
    p_isd->prm.RAND_GENERATE.p_block_out = (uint32_t *)BIG_BUFFER[0];
    p_isd->prm.RAND_GENERATE.nb_blocks = 1u;
    p_isd->prm.RAND_GENERATE.job_slice = 0u;
    return run_service("RNG generate", p_isd);
}

static int prepare_rng(void)
{
    return run_rng_init() && run_rng_extend_seed();
}

static int test_rng_service(r_icumif_service_id_t service_id)
{
    if (service_id != SERVICE_03_RAND_INIT_RNG)
    {
        if (!run_rng_init())
        {
            return 0;
        }
        if ((service_id == SERVICE_03_RAND_RND) && !run_rng_extend_seed())
        {
            return 0;
        }
    }

    switch (service_id)
    {
        case SERVICE_03_RAND_INIT_RNG:
            return run_rng_init();
        case SERVICE_03_RAND_EXTEND_SEED:
            return run_rng_extend_seed();
        case SERVICE_03_RAND_RND:
            return run_rng_generate();
        default:
            return 0;
    }
}

static int test_ecc_signature_service(int eddsa, int verify)
{
    static const uint8_t message[] = "ICUM ECC service test";
    r_icumif_isd_t *p_isd;
    r_icumif_service_id_t sign_service;
    r_icumif_service_id_t verify_service;
    r_hash_primitive_t hash_primitive;
    r_key_index_t key_id;
    const char *algorithm_name;

    sign_service = eddsa ? SERVICE_04_EDDSA_SIGN : SERVICE_04_ECDSA_SIGN;
    verify_service = eddsa ? SERVICE_04_EDDSA_VERIFY : SERVICE_04_ECDSA_VERIFY;
    hash_primitive = eddsa ? HASH_PRIMITIVE_SHA2_512 : HASH_PRIMITIVE_SHA2_256;
    key_id = eddsa ? 1u : 0u;
    algorithm_name = eddsa ? "EdDSA" : "ECDSA";

    if (!prepare_rng())
    {
        return 0;
    }

    memset(BIG_BUFFER[0], 0, 32u);
    memset(BIG_BUFFER[1], 0, 32u);
    memset(BIG_BUFFER[2], 0, 32u);
    memcpy(BIG_BUFFER[0], message, sizeof(message) - 1u);

    p_isd = test_isd();
    p_isd->service_id = sign_service;
    if (eddsa)
    {
        p_isd->prm.EDDSA_SIGN.p_source = BIG_BUFFER[0];
        p_isd->prm.EDDSA_SIGN.signature[0].p_data = BIG_BUFFER[1];
        p_isd->prm.EDDSA_SIGN.signature[1].p_data = BIG_BUFFER[2];
        p_isd->prm.EDDSA_SIGN.source_size_in_bytes = sizeof(message) - 1u;
        p_isd->prm.EDDSA_SIGN.hash_primitive = hash_primitive;
        p_isd->prm.EDDSA_SIGN.signature_format = FORMAT_BINARY_PLAIN;
        p_isd->prm.EDDSA_SIGN.key_group = KEY_GRP_ECC_RAM;
        p_isd->prm.EDDSA_SIGN.key_id = key_id;
        p_isd->prm.EDDSA_SIGN.job_slice_hash = job_slice[JOB_H];
        p_isd->prm.EDDSA_SIGN.job_slice_point_mul = 0u;
        p_isd->prm.EDDSA_SIGN.signature_size_in_bits[0] = 256u;
        p_isd->prm.EDDSA_SIGN.signature_size_in_bits[1] = 256u;
        p_isd->prm.EDDSA_SIGN.is_precalculated_hash = 0u;
    }
    else
    {
        p_isd->prm.ECDSA_SIGN.p_source = BIG_BUFFER[0];
        p_isd->prm.ECDSA_SIGN.signature[0].p_data = BIG_BUFFER[1];
        p_isd->prm.ECDSA_SIGN.signature[1].p_data = BIG_BUFFER[2];
        p_isd->prm.ECDSA_SIGN.source_size_in_bytes = sizeof(message) - 1u;
        p_isd->prm.ECDSA_SIGN.hash_primitive = hash_primitive;
        p_isd->prm.ECDSA_SIGN.signature_format = FORMAT_BINARY_PLAIN;
        p_isd->prm.ECDSA_SIGN.key_group = KEY_GRP_ECC_RAM;
        p_isd->prm.ECDSA_SIGN.key_id = key_id;
        p_isd->prm.ECDSA_SIGN.job_slice_hash = job_slice[JOB_H];
        p_isd->prm.ECDSA_SIGN.job_slice_point_mul = 0u;
        p_isd->prm.ECDSA_SIGN.signature_size_in_bits[0] = 256u;
        p_isd->prm.ECDSA_SIGN.signature_size_in_bits[1] = 256u;
        p_isd->prm.ECDSA_SIGN.is_precalculated_hash = 0u;
    }
    if (!run_service(verify ? "signature setup" : algorithm_name, p_isd))
    {
        return 0;
    }
    if (!verify)
    {
        return 1;
    }

    p_isd = test_isd();
    p_isd->service_id = verify_service;
    if (eddsa)
    {
        p_isd->prm.EDDSA_VERIFY.p_source = BIG_BUFFER[0];
        p_isd->prm.EDDSA_VERIFY.signature[0].p_data = BIG_BUFFER[1];
        p_isd->prm.EDDSA_VERIFY.signature[1].p_data = BIG_BUFFER[2];
        p_isd->prm.EDDSA_VERIFY.source_size_in_bytes = sizeof(message) - 1u;
        p_isd->prm.EDDSA_VERIFY.signature_size_in_bits[0] = 256u;
        p_isd->prm.EDDSA_VERIFY.signature_size_in_bits[1] = 256u;
        p_isd->prm.EDDSA_VERIFY.hash_primitive = hash_primitive;
        p_isd->prm.EDDSA_VERIFY.signature_format = FORMAT_BINARY_PLAIN;
        p_isd->prm.EDDSA_VERIFY.key_group = KEY_GRP_ECC_RAM;
        p_isd->prm.EDDSA_VERIFY.key_id = key_id;
        p_isd->prm.EDDSA_VERIFY.job_slice_hash = job_slice[JOB_H];
        p_isd->prm.EDDSA_VERIFY.job_slice_point_mul = 0u;
        p_isd->prm.EDDSA_VERIFY.is_precalculated_hash = 0u;
        p_isd->prm.EDDSA_VERIFY.verification_result = VERIFICATION_FAIL;
    }
    else
    {
        p_isd->prm.ECDSA_VERIFY.p_source = BIG_BUFFER[0];
        p_isd->prm.ECDSA_VERIFY.signature[0].p_data = BIG_BUFFER[1];
        p_isd->prm.ECDSA_VERIFY.signature[1].p_data = BIG_BUFFER[2];
        p_isd->prm.ECDSA_VERIFY.source_size_in_bytes = sizeof(message) - 1u;
        p_isd->prm.ECDSA_VERIFY.signature_size_in_bits[0] = 256u;
        p_isd->prm.ECDSA_VERIFY.signature_size_in_bits[1] = 256u;
        p_isd->prm.ECDSA_VERIFY.hash_primitive = hash_primitive;
        p_isd->prm.ECDSA_VERIFY.signature_format = FORMAT_BINARY_PLAIN;
        p_isd->prm.ECDSA_VERIFY.key_group = KEY_GRP_ECC_RAM;
        p_isd->prm.ECDSA_VERIFY.key_id = key_id;
        p_isd->prm.ECDSA_VERIFY.job_slice_hash = job_slice[JOB_H];
        p_isd->prm.ECDSA_VERIFY.job_slice_point_mul = 0u;
        p_isd->prm.ECDSA_VERIFY.is_precalculated_hash = 0u;
        p_isd->prm.ECDSA_VERIFY.verification_result = VERIFICATION_FAIL;
    }
    if (!run_service(eddsa ? "EdDSA verify" : "ECDSA verify", p_isd))
    {
        return 0;
    }
    return eddsa ?
        (p_isd->prm.EDDSA_VERIFY.verification_result == VERIFICATION_PASS) :
        (p_isd->prm.ECDSA_VERIFY.verification_result == VERIFICATION_PASS);
}

static int test_ecc_key_generate(void)
{
    r_icumif_isd_t *p_isd;

    if (!prepare_rng())
    {
        return 0;
    }

    p_isd = test_isd();
    p_isd->service_id = SERVICE_04_ECC_KEY_GENERATE;
    p_isd->prm.ECC_KEY_GENERATE.key_group = KEY_GRP_ECC_RAM;
    p_isd->prm.ECC_KEY_GENERATE.key_id = 0u;
    p_isd->prm.ECC_KEY_GENERATE.curve_id = ECC_CURVE_NIST_P256;
    p_isd->prm.ECC_KEY_GENERATE.flags.disable_on_debug = 0u;
    p_isd->prm.ECC_KEY_GENERATE.flags.write_protected = 0u;
    return run_service("ECC P-256 keygen", p_isd);
}

static int test_ecdh_service(void)
{
    r_icumif_isd_t *p_isd = test_isd();
    p_isd->service_id = SERVICE_04_ECDH;
    p_isd->prm.ECDH.ecc_priv_key_group = KEY_GRP_ECC_RAM;
    p_isd->prm.ECDH.ecc_priv_key_id = 0u;
    p_isd->prm.ECDH.ecc_pub_key_group = KEY_GRP_ECC_RAM;
    p_isd->prm.ECDH.ecc_pub_key_id = 0u;
    p_isd->prm.ECDH.shared_key_group = KEY_GRP_AES_RAM;
    p_isd->prm.ECDH.shared_key_id = 4u;
    p_isd->prm.ECDH.kdf_algo = KDF_ALGO_KDF1;
    p_isd->prm.ECDH.hash_primitive = HASH_PRIMITIVE_SHA2_256;
    p_isd->prm.ECDH.job_slice = 0u;
    p_isd->prm.ECDH.p_info = NULL;
    p_isd->prm.ECDH.counter = 4u;
    p_isd->prm.ECDH.info_size_in_bytes = 0u;
    p_isd->prm.ECDH.key_size_in_bytes = 16u;
    return run_service("ECDH", p_isd);
}

static int test_hash_service(void)
{
    static const uint8_t message[] = { 0x61u, 0x62u, 0x63u };
    static const uint8_t expected_hash[32] = {
        0xbau, 0x78u, 0x16u, 0xbfu, 0x8fu, 0x01u, 0xcfu, 0xeau,
        0x41u, 0x41u, 0x40u, 0xdeu, 0x5du, 0xaeu, 0x22u, 0x23u,
        0xb0u, 0x03u, 0x61u, 0xa3u, 0x96u, 0x17u, 0x7au, 0x9cu,
        0xb4u, 0x10u, 0xffu, 0x61u, 0xf2u, 0x00u, 0x15u, 0xadu
    };
    r_icumif_isd_t *p_isd;

    memcpy(BIG_BUFFER[0], message, sizeof(message));
    memset(BIG_BUFFER[1], 0, 32u);
    p_isd = test_isd();
    p_isd->service_id = SERVICE_05_HASH;
    p_isd->prm.HASH.p_message = BIG_BUFFER[0];
    p_isd->prm.HASH.p_hash = BIG_BUFFER[1];
    p_isd->prm.HASH.message_size_in_bytes = sizeof(message);
    p_isd->prm.HASH.hash_primitive = HASH_PRIMITIVE_SHA2_256;
    p_isd->prm.HASH.job_slice = job_slice[JOB_H];
    p_isd->prm.HASH.job_cycle = JOB_ALL_AT_ONCE;
    if (!run_service("SHA-256", p_isd))
    {
        return 0;
    }
    if (memcmp(BIG_BUFFER[1], expected_hash, sizeof(expected_hash)) != 0)
    {
        printf("SHA-256 digest comparison failed\n");
        return 0;
    }
    return 1;
}

static int test_hmac_service(void)
{
    static const uint8_t message[] = "ICUM HMAC service test";
    r_icumif_isd_t *p_isd;

    memcpy(BIG_BUFFER[0], message, sizeof(message) - 1u);
    memset(BIG_BUFFER[1], 0, 32u);
    p_isd = test_isd();
    p_isd->service_id = SERVICE_05_HMAC;
    p_isd->prm.HMAC.key_id = 0u;
    p_isd->prm.HMAC.hmac_operation = HMAC_GENERATION;
    p_isd->prm.HMAC.hash_primitive = HASH_PRIMITIVE_SHA2_256;
    p_isd->prm.HMAC.p_msg = BIG_BUFFER[0];
    p_isd->prm.HMAC.msg_size_in_bytes = sizeof(message) - 1u;
    p_isd->prm.HMAC.p_hmac = BIG_BUFFER[1];
    p_isd->prm.HMAC.hmac_size_in_bytes = 32u;
    p_isd->prm.HMAC.job_slice = job_slice[JOB_H];
    if (!run_service("HMAC generation", p_isd))
    {
        return 0;
    }

    p_isd = test_isd();
    p_isd->service_id = SERVICE_05_HMAC;
    p_isd->prm.HMAC.key_id = 0u;
    p_isd->prm.HMAC.hmac_operation = HMAC_VERIFICATION;
    p_isd->prm.HMAC.hash_primitive = HASH_PRIMITIVE_SHA2_256;
    p_isd->prm.HMAC.p_msg = BIG_BUFFER[0];
    p_isd->prm.HMAC.msg_size_in_bytes = sizeof(message) - 1u;
    p_isd->prm.HMAC.p_hmac = BIG_BUFFER[1];
    p_isd->prm.HMAC.hmac_size_in_bytes = 32u;
    p_isd->prm.HMAC.job_slice = job_slice[JOB_H];
    p_isd->prm.HMAC.verification_result = VERIFICATION_FAIL;
    if (!run_service("HMAC verification", p_isd))
    {
        return 0;
    }
    return p_isd->prm.HMAC.verification_result == VERIFICATION_PASS;
}

void icum_crypto_service_test_service(r_icumif_service_id_t service_id)
{
    switch (service_id)
    {
        case SERVICE_01_AES_CIPHER:
#if PE1_TEST_AES_CIPHER
            print_test_result("AES-ECB enc/dec", test_aes_cipher_mode(CIPHER_MOD_ECB, "AES-ECB enc/dec"));
            print_test_result("AES-CBC enc/dec", test_aes_cipher_mode(CIPHER_MOD_CBC, "AES-CBC enc/dec"));
            print_test_result("AES-CFB enc/dec", test_aes_cipher_mode(CIPHER_MOD_CFB, "AES-CFB enc/dec"));
            print_test_result("AES-OFB enc/dec", test_aes_cipher_mode(CIPHER_MOD_OFB, "AES-OFB enc/dec"));
            print_test_result("AES-CTR enc/dec", test_aes_cipher_mode(CIPHER_MOD_CTR, "AES-CTR enc/dec"));
            print_test_result("AES-XTS enc/dec", test_aes_cipher_mode(CIPHER_MOD_XTS, "AES-XTS enc/dec"));
#else
            printf("AES cipher test is disabled by PE1_TEST_AES_CIPHER.\n");
#endif
            break;
        case SERVICE_01_AES_AUTH_CIPHER:
#if PE1_TEST_AES_AUTH
            print_test_result("AES-GCM enc/dec", test_aes_auth_mode(AUTH_CIPHER_MOD_GCM, "AES-GCM enc/dec"));
            print_test_result("AES-CCM enc/dec", test_aes_auth_mode(AUTH_CIPHER_MOD_CCM, "AES-CCM enc/dec"));
#else
            printf("AES authenticated-cipher test is disabled by PE1_TEST_AES_AUTH.\n");
#endif
            break;
        case SERVICE_01_AES_CMAC:
#if PE1_TEST_AES_CMAC
            print_test_result("AES-CMAC", test_aes_cmac());
#else
            printf("AES-CMAC test is disabled by PE1_TEST_AES_CMAC.\n");
#endif
            break;
        case SERVICE_02_RSA_VERIFY:
#if PE1_TEST_RSA_SIGNATURE
            print_test_result("RSA-PSS sign/verify",
                              test_rsa_signature_mode(ENCODING_FORMAT_EMSA_PSS, "RSA-PSS", SERVICE_02_RSA_VERIFY));
            print_test_result("RSA-PKCS1 sign/verify",
                              test_rsa_signature_mode(ENCODING_FORMAT_EMSA_PKCS1_V15, "RSA-PKCS1",
                                                      SERVICE_02_RSA_VERIFY));
#else
            printf("RSA signature test is disabled by PE1_TEST_RSA_SIGNATURE.\n");
#endif
            break;
        case SERVICE_02_RSA_SIGN:
#if PE1_TEST_RSA_SIGNATURE
            print_test_result("RSA-PSS sign",
                              test_rsa_signature_mode(ENCODING_FORMAT_EMSA_PSS, "RSA-PSS", SERVICE_02_RSA_SIGN));
            print_test_result("RSA-PKCS1 sign",
                              test_rsa_signature_mode(ENCODING_FORMAT_EMSA_PKCS1_V15, "RSA-PKCS1",
                                                      SERVICE_02_RSA_SIGN));
#else
            printf("RSA signature test is disabled by PE1_TEST_RSA_SIGNATURE.\n");
#endif
            break;
        case SERVICE_02_RSA_ENCRYPT:
#if PE1_TEST_RSA_CRYPT
            print_test_result("RSA-OAEP encrypt",
                              test_rsa_crypt_mode(ENCODING_FORMAT_EME_OAEP, "RSA-OAEP encrypt",
                                                  SERVICE_02_RSA_ENCRYPT));
            print_test_result("RSA-PKCS1 encrypt",
                              test_rsa_crypt_mode(ENCODING_FORMAT_EME_PKCS1_V15, "RSA-PKCS1 encrypt",
                                                  SERVICE_02_RSA_ENCRYPT));
#else
            printf("RSA encryption test is disabled by PE1_TEST_RSA_CRYPT.\n");
#endif
            break;
        case SERVICE_02_RSA_DECRYPT:
#if PE1_TEST_RSA_CRYPT
            print_test_result("RSA-OAEP decrypt",
                              test_rsa_crypt_mode(ENCODING_FORMAT_EME_OAEP, "RSA-OAEP decrypt",
                                                  SERVICE_02_RSA_DECRYPT));
            print_test_result("RSA-PKCS1 decrypt",
                              test_rsa_crypt_mode(ENCODING_FORMAT_EME_PKCS1_V15, "RSA-PKCS1 decrypt",
                                                  SERVICE_02_RSA_DECRYPT));
#else
            printf("RSA encryption test is disabled by PE1_TEST_RSA_CRYPT.\n");
#endif
            break;
        case SERVICE_02_RSA_KEY_GENERATE:
#if PE1_TEST_RSA_KEY_GENERATE
        {
            r_icumif_isd_t *p_isd = test_isd();
            p_isd->service_id = SERVICE_02_RSA_KEY_GENERATE;
            p_isd->prm.RSA_KEY_GENERATE.key_id = TEST_RSA_KEY_ID;
            p_isd->prm.RSA_KEY_GENERATE.modulus_size_in_bits = TEST_RSA_KEY_BITS;
            print_test_result("RSA 2048-bit keygen", run_service("RSA 2048-bit keygen", p_isd));
            break;
        }
#else
            printf("RSA key-generation test is disabled by PE1_TEST_RSA_KEY_GENERATE.\n");
            break;
#endif
        case SERVICE_03_RAND_INIT_RNG:
            print_test_result("RNG initialization", test_rng_service(service_id));
            break;
        case SERVICE_03_RAND_EXTEND_SEED:
            print_test_result("RNG seed extension", test_rng_service(service_id));
            break;
        case SERVICE_03_RAND_RND:
            print_test_result("RNG generation", test_rng_service(service_id));
            break;
        case SERVICE_04_ECDSA_VERIFY:
            print_test_result("ECDSA verification", test_ecc_signature_service(0, 1));
            break;
        case SERVICE_04_ECDSA_SIGN:
            print_test_result("ECDSA signing", test_ecc_signature_service(0, 0));
            break;
        case SERVICE_04_ECDH:
            print_test_result("ECDH", test_ecdh_service());
            break;
        case SERVICE_04_EDDSA_VERIFY:
            print_test_result("EdDSA verification", test_ecc_signature_service(1, 1));
            break;
        case SERVICE_04_EDDSA_SIGN:
            print_test_result("EdDSA signing", test_ecc_signature_service(1, 0));
            break;
        case SERVICE_04_ECC_KEY_GENERATE:
            print_test_result("ECC P-256 keygen", test_ecc_key_generate());
            break;
        case SERVICE_05_HASH:
            print_test_result("SHA-256", test_hash_service());
            break;
        case SERVICE_05_HMAC:
            print_test_result("HMAC", test_hmac_service());
            break;
        default:
            printf("Unsupported ICUM service ID: 0x%02x\n", (unsigned int)service_id);
            break;
    }
}

void icum_crypto_service_test(void)
{
#if !(PE1_TEST_AES_CIPHER || PE1_TEST_AES_AUTH || PE1_TEST_AES_CMAC || \
            PE1_TEST_RSA_KEY_GENERATE || PE1_TEST_RSA_CRYPT || PE1_TEST_RSA_SIGNATURE)
        printf("No AES/RSA service tests are enabled.\n");
#else
#if PE1_TEST_RSA_KEY_GENERATE
    r_icumif_isd_t *p_isd;
#endif

    printf("\nICUM AES/RSA service tests (OSTM0 time to service return)\n");
#if PE1_TEST_AES_CIPHER
    print_test_result("AES-ECB enc/dec", test_aes_cipher_mode(CIPHER_MOD_ECB, "AES-ECB enc/dec"));
    print_test_result("AES-CBC enc/dec", test_aes_cipher_mode(CIPHER_MOD_CBC, "AES-CBC enc/dec"));
    print_test_result("AES-CFB enc/dec", test_aes_cipher_mode(CIPHER_MOD_CFB, "AES-CFB enc/dec"));
    print_test_result("AES-OFB enc/dec", test_aes_cipher_mode(CIPHER_MOD_OFB, "AES-OFB enc/dec"));
    print_test_result("AES-CTR enc/dec", test_aes_cipher_mode(CIPHER_MOD_CTR, "AES-CTR enc/dec"));
    print_test_result("AES-XTS enc/dec", test_aes_cipher_mode(CIPHER_MOD_XTS, "AES-XTS enc/dec"));
#endif
#if PE1_TEST_AES_AUTH
    print_test_result("AES-GCM enc/dec", test_aes_auth_mode(AUTH_CIPHER_MOD_GCM, "AES-GCM enc/dec"));
    print_test_result("AES-CCM enc/dec", test_aes_auth_mode(AUTH_CIPHER_MOD_CCM, "AES-CCM enc/dec"));
#endif
#if PE1_TEST_AES_CMAC
    print_test_result("AES-CMAC", test_aes_cmac());
#endif

#if PE1_TEST_RSA_KEY_GENERATE
    p_isd = test_isd();
    p_isd->service_id = SERVICE_02_RSA_KEY_GENERATE;
    p_isd->prm.RSA_KEY_GENERATE.key_id = TEST_RSA_KEY_ID;
    p_isd->prm.RSA_KEY_GENERATE.modulus_size_in_bits = TEST_RSA_KEY_BITS;
    print_test_result("RSA 2048-bit keygen", run_service("RSA 2048-bit keygen", p_isd));
#endif
#if PE1_TEST_RSA_CRYPT
    print_test_result("RSA-OAEP enc/dec",
                      test_rsa_crypt_mode(ENCODING_FORMAT_EME_OAEP, "RSA-OAEP enc/dec", LAST_ITEM_SERV_ID));
    print_test_result("RSA-PKCS1 enc/dec",
                      test_rsa_crypt_mode(ENCODING_FORMAT_EME_PKCS1_V15, "RSA-PKCS1 enc/dec",
                                          LAST_ITEM_SERV_ID));
#endif
#if PE1_TEST_RSA_SIGNATURE
    print_test_result("RSA-PSS sign/verify",
                      test_rsa_signature_mode(ENCODING_FORMAT_EMSA_PSS, "RSA-PSS sign/verify",
                                              LAST_ITEM_SERV_ID));
    print_test_result("RSA-PKCS1 sign/verify",
                      test_rsa_signature_mode(ENCODING_FORMAT_EMSA_PKCS1_V15, "RSA-PKCS1 sign/verify",
                                              LAST_ITEM_SERV_ID));
#endif
#endif
}
