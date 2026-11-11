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

// ********************************************************************************
// Headers
// ********************************************************************************

#include <stdio.h>
#include <string.h>

#undef NULL
#include "r_icumif_api.h"
#include "r_icumif_pub.h"

#include "pe1.h"
#include "pe1.int.h"
#include "shared.h"

#include "vrun.api.h"

#include "renesas_types.h"

#define NB_ROM_AES_KEYS             2
#define NB_RAM_AES_KEYS             6
#define NB_CR_KEYS                  4
#define NB_RSA_ROM_PUB_KEYS         2
#define NB_ECC_ROM_PUB_KEYS         2
#define NB_RAM_ECC_KEYS             2
#define NB_RAM_HMAC_KEYS            4
#define NB_MASTER_SECRET            1

#define STRINGIFY(s) { #s, s }

typedef struct {
    char  *name;
    int   value;
} constant_tuple;


static const constant_tuple key_group_tuples[] = {
    STRINGIFY(KEY_GRP_AES_ROM),
    STRINGIFY(KEY_GRP_AES),
    STRINGIFY(KEY_GRP_AES_RAM),
    STRINGIFY(KEY_GRP_SHE),
    STRINGIFY(KEY_GRP_RSA),
    STRINGIFY(KEY_GRP_RSA_PUB),
    STRINGIFY(KEY_GRP_RSA_ROM_PUB),
    STRINGIFY(KEY_GRP_ECC),
    STRINGIFY(KEY_GRP_ECC_RAM),
    STRINGIFY(KEY_GRP_ECC_PUB),
    STRINGIFY(KEY_GRP_ECC_ROM_PUB),
    STRINGIFY(KEY_GRP_AES_CR),
    STRINGIFY(KEY_GRP_HMAC),
    STRINGIFY(KEY_GRP_TLS_MASTER),
    STRINGIFY(KEY_GRP_RSA_PUB_EXT),
    STRINGIFY(KEY_GRP_ECC_PUB_EXT),
};

static const constant_tuple verif_result_tuples[] = {
    STRINGIFY(VERIFICATION_FAIL),
    STRINGIFY(VERIFICATION_PASS),
};
static const constant_tuple number_format_tuples[] = {
    STRINGIFY(FORMAT_STRING_DECIMAL),
    STRINGIFY(FORMAT_STRING_HEXA),
    STRINGIFY(FORMAT_BINARY_PLAIN),
};
static const constant_tuple hash_encoding_format_tuples[] = {
    STRINGIFY(ENCODING_FORMAT_EMSA_PSS),
    STRINGIFY(ENCODING_FORMAT_EMSA_PKCS1_V15),
};
static const constant_tuple message_encoding_format_tuples[] = {
    STRINGIFY(ENCODING_FORMAT_EME_OAEP),
    STRINGIFY(ENCODING_FORMAT_EME_PKCS1_V15),
};
static const constant_tuple hash_primitive_tuples[] = {
    STRINGIFY(HASH_PRIMITIVE_MD5),
    STRINGIFY(HASH_PRIMITIVE_RIPEMD_128),
    STRINGIFY(HASH_PRIMITIVE_RIPEMD_160),
    STRINGIFY(HASH_PRIMITIVE_RIPEMD_256),
    STRINGIFY(HASH_PRIMITIVE_RIPEMD_320),
    STRINGIFY(HASH_PRIMITIVE_SHA1),
    STRINGIFY(HASH_PRIMITIVE_SHA2_224),
    STRINGIFY(HASH_PRIMITIVE_SHA2_256),
    STRINGIFY(HASH_PRIMITIVE_SHA2_384),
    STRINGIFY(HASH_PRIMITIVE_SHA2_512),
};
static const constant_tuple sig_algo_tuples[] = {
    STRINGIFY(SIGALGO_NONE),
    STRINGIFY(SIGALGO_RSA_PSS),
    STRINGIFY(SIGALGO_RSA_PKCS1_V15),
};
static const constant_tuple ecc_curve_tuples[] = {
    STRINGIFY(ECC_CURVE_NIST_P192),
    STRINGIFY(ECC_CURVE_NIST_P224),
    STRINGIFY(ECC_CURVE_NIST_P256),
    STRINGIFY(ECC_CURVE_BRAINPOOL_P192),
    STRINGIFY(ECC_CURVE_BRAINPOOL_P224),
    STRINGIFY(ECC_CURVE_BRAINPOOL_P256),
    STRINGIFY(ECC_CURVE_BRAINPOOL_P320),
    STRINGIFY(ECC_CURVE_CURVE25519),
};
static const constant_tuple kdf_algo_tuples[] = {
    STRINGIFY(KDF_ALGO_KDF1),
    STRINGIFY(KDF_ALGO_KDF2),
    STRINGIFY(KDF_ALGO_KDF3),
    STRINGIFY(KDF_ALGO_PKDF1),
    STRINGIFY(KDF_ALGO_PKDF2),
};

/* convert from type 'r_key_group_t' to 'string' */
char* getKeyGroupName(r_key_group_t value_in)
{
    int i;
    int table_len = sizeof(key_group_tuples) / sizeof(key_group_tuples[0]);
    
    for (i=0; i<table_len; i++)
    {
        if (key_group_tuples[i].value == value_in)
        {
            return key_group_tuples[i].name;
        }
    }
    return "";
}

/* convert from type 'r_verif_result_t' to 'string' */
char* getVerifResultName(r_verif_result_t value_in)
{
    int i;
    int table_len = sizeof(verif_result_tuples) / sizeof(verif_result_tuples[0]);
    
    for (i=0; i<table_len; i++)
    {
        if (verif_result_tuples[i].value == value_in)
        {
            return verif_result_tuples[i].name;
        }
    }
    return "";
}

/* convert from type 'r_number_format_t' to 'string' */
char* getNumberFormatName(r_number_format_t value_in)
{
    int i;
    int table_len = sizeof(number_format_tuples) / sizeof(number_format_tuples[0]);
    
    for (i=0; i<table_len; i++)
    {
        if (number_format_tuples[i].value == value_in)
        {
            return number_format_tuples[i].name;
        }
    }
    return "";
}

/* convert from type 'r_hash_encoding_format_t' to 'string' */
char* getHashEncodingFormatName(r_hash_encoding_format_t value_in)
{
    int i;
    int table_len = sizeof(hash_encoding_format_tuples) / sizeof(hash_encoding_format_tuples[0]);
    
    for (i=0; i<table_len; i++)
    {
        if (hash_encoding_format_tuples[i].value == value_in)
        {
            return hash_encoding_format_tuples[i].name;
        }
    }
    return "";
}

/* convert from type 'r_message_encoding_format_t' to 'string' */
char* getMessageEncodingFormatName(r_message_encoding_format_t value_in)
{
    int i;
    int table_len = sizeof(message_encoding_format_tuples) / sizeof(message_encoding_format_tuples[0]);
    
    for (i=0; i<table_len; i++)
    {
        if (message_encoding_format_tuples[i].value == value_in)
        {
            return message_encoding_format_tuples[i].name;
        }
    }
    return "";
}

/* convert from type 'r_hash_primitive_t' to 'string' */
char* getHashPrimitiveName(r_hash_primitive_t value_in)
{
    int i;
    int table_len = sizeof(hash_primitive_tuples) / sizeof(hash_primitive_tuples[0]);
    
    for (i=0; i<table_len; i++)
    {
        if (hash_primitive_tuples[i].value == value_in)
        {
            return hash_primitive_tuples[i].name;
        }
    }
    return "";
}

/* convert from type 'r_signature_algo_t' to 'string' */
char* getSigAlgoName(r_signature_algo_t value_in)
{
    int i;
    int table_len = sizeof(sig_algo_tuples) / sizeof(sig_algo_tuples[0]);
    
    for (i=0; i<table_len; i++)
    {
        if (sig_algo_tuples[i].value == value_in)
        {
            return sig_algo_tuples[i].name;
        }
    }
    return "";
}

/* convert from type 'r_ecc_curve_t' to 'string' */
char* getEccCurveName(r_ecc_curve_t value_in)
{
    int i;
    int table_len = sizeof(ecc_curve_tuples) / sizeof(ecc_curve_tuples[0]);
    
    for (i=0; i<table_len; i++)
    {
        if (ecc_curve_tuples[i].value == value_in)
        {
            return ecc_curve_tuples[i].name;
        }
    }
    return "";
}

/* convert from type 'r_kdf_algo_t' to 'string' */
char* getKdfAlgoName(r_kdf_algo_t value_in)
{
    int i;
    int table_len = sizeof(kdf_algo_tuples) / sizeof(kdf_algo_tuples[0]);
    
    for (i=0; i<table_len; i++)
    {
        if (kdf_algo_tuples[i].value == value_in)
        {
            return kdf_algo_tuples[i].name;
        }
    }
    return "";
}

/*===========================================================================
  Trigger an ICU-M service and wait for the response
  [PUBLIC]
  ===========================================================================*/
void trigger_2service_and_wait(r_icumif_isd_t *p_ISD, r_icumif_isd_t *p_ISD2)
{
    // trigger the service request
    R_ICUMIF_ServiceRequest(p_ISD);
    R_ICUMIF_ServiceRequest(p_ISD2);

    // go in HALT mode
    // wait for interrupt
    while((!R_ICUMIF_IsServiceCompleted(p_ISD)) || (!R_ICUMIF_IsServiceCompleted(p_ISD2)))
    {
        HALT();
        NOPS(10);
    }

    // test for possible error
    if(p_ISD->service_result != SERV_OK)
    {
        printf("Service error: %d (", p_ISD->service_result);
        printf(R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        printf(") *****\a\n");
    }

    if(p_ISD2->service_result != SERV_OK)
    {
        printf("Service2 error: %d (", p_ISD2->service_result);
        printf(R_ICUMIF_GetErrorCodeName(p_ISD2->service_result));
        printf(") *****\a\n");
    }
}

/*===========================================================================
  Trigger an ICU-M service and wait for the response
  [PUBLIC]
  ===========================================================================*/
void trigger_3service_and_wait(r_icumif_isd_t *p_ISD, r_icumif_isd_t *p_ISD2, r_icumif_isd_t *p_ISD3)
{
    // trigger the service request
    R_ICUMIF_ServiceRequest(p_ISD);
    R_ICUMIF_ServiceRequest(p_ISD2);
    R_ICUMIF_ServiceRequest(p_ISD3);

    // go in HALT mode
    // wait for interrupt
    while((!R_ICUMIF_IsServiceCompleted(p_ISD)) || (!R_ICUMIF_IsServiceCompleted(p_ISD2)) || (!R_ICUMIF_IsServiceCompleted(p_ISD3)))
    {
        HALT();
        NOPS(10);
    }

    // test for possible error
    if(p_ISD->service_result != SERV_OK)
    {
        printf("Service error: %d (", p_ISD->service_result);
        printf(R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
        printf(") *****\a\n");
    }

    if(p_ISD2->service_result != SERV_OK)
    {
        printf("Service2 error: %d (", p_ISD2->service_result);
        printf(R_ICUMIF_GetErrorCodeName(p_ISD2->service_result));
        printf(") *****\a\n");
    }

    if(p_ISD3->service_result != SERV_OK)
    {
        printf("Service3 error: %d (", p_ISD3->service_result);
        printf(R_ICUMIF_GetErrorCodeName(p_ISD3->service_result));
        printf(") *****\a\n");
    }
}
