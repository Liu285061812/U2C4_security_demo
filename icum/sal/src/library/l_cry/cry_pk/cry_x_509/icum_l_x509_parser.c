/******************************************************************************/
/* Component Name   Cryptographic Library X509                                */
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

#include "icum_l_cry_pk_pub.h"
#include "icum_l_x509.h"
#include "../cry_common/icum_l_my_toolbox.h"

/* OID list */
/* source RFC 3279 */
/* additional algo identifier in RFC 4055 */
#define OID_ISO                 (1u)

#define OID_MEMBER_BODY         (2u)
#define OID_IDENTIFIED_ORG      (3u)

#define OID_US                  (840u)
#define OID_TELETRUST           (36u)
#define OID_CERTICOM            (132u)

#define OID_RSADSI              (113549u)
#define OID_PKCS                (1u)
#define OID_PKCS_1              (1u)

#define OID_RSA_ENCRYPTION      (1u)

/* OID list */
/* source RFC 5480 */
#define OID_X9_62               (10045u)
#define OID_CURVE               (0u)
#define OID_KEY_TYPE            (2u)
#define OID_CURVES              (3u)
#define OID_PRIME               (1u)

#define OID_PRIME192V1          (1u)       /* secp192r1 */
#define OID_PRIME256V1          (7u)       /* secp256r1 */
#define OID_SECP224R1           (33u)
#define OID_SECP384R1           (34u)

/* OID list */
/* source RFC 5639 */
#define OID_ALGO                (3)
#define OID_SIG_ALGO            (3)
#define OID_ECSIGN              (2)
#define OID_ECSTDCURVES_GENERATION    (8)
#define OID_BRAINPOOL_P256R1    (7u)
#define OID_BRAINPOOL_P320R1    (9u)


static const uint32_t OID_xxxWithRSAEncryption[] = {OID_ISO, OID_MEMBER_BODY, OID_US, OID_RSADSI, OID_PKCS, OID_PKCS_1};
static const uint32_t OID_ecPublicKey[] ={OID_ISO, OID_MEMBER_BODY,    OID_US,     OID_X9_62, OID_KEY_TYPE, 1};
static const uint32_t OID_curveX962[]  = {OID_ISO, OID_MEMBER_BODY,    OID_US,     OID_X9_62, OID_CURVES, OID_PRIME};
static const uint32_t OID_curveSEC2[]  = {OID_ISO, OID_IDENTIFIED_ORG, OID_CERTICOM, OID_CURVE};
static const uint32_t OID_curveBPool[]  =
    {OID_ISO, OID_IDENTIFIED_ORG, OID_TELETRUST, OID_ALGO, OID_SIG_ALGO, OID_ECSIGN, OID_ECSTDCURVES_GENERATION, 1, 1};

static const uint16_t key_size_tab[] =
{
    192u,       /* ECC_CURVE_NIST_P192 */
    224u,       /* ECC_CURVE_NIST_P224 */
    256u,       /* ECC_CURVE_NIST_P256 */
    384u,       /* ECC_CURVE_NIST_P384 */
    544u,       /* ECC_CURVE_NIST_P521 */
    192u,       /* ECC_CURVE_BRAINPOOL_P192 */
    224u,       /* ECC_CURVE_BRAINPOOL_P224 */
    256u,       /* ECC_CURVE_BRAINPOOL_P256 */
    320u,       /* ECC_CURVE_BRAINPOOL_P320 */
    384u,       /* ECC_CURVE_BRAINPOOL_P384 */
    512u,       /* ECC_CURVE_BRAINPOOL_P512 */
    256u,       /* ECC_CURVE_BRAINPOOL_P256_T */
    320u,       /* ECC_CURVE_BRAINPOOL_P320_T */
    384u,       /* ECC_CURVE_BRAINPOOL_P384_T */
    512u,       /* ECC_CURVE_BRAINPOOL_P512_T */
    256u,       /* ECC_CURVE_CURVE25519 */
    448u,       /* ECC_CURVE_CURVE448 */
    256u,       /* ECC_CURVE_ED25519 */
    480u,       /* ECC_CURVE_ED448 */
};

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_X509_0003]:[ICUMFW_UD_L_CRY_X509_0003]
* Function name: l_X509_ParseTop
* Description  : Parse x.509 certificate top hierarchy
*            X.509 certificate v3
*            - on top: 1 member, which is a sequence
*            - in the sequence : 3 members has follows:
*                 - the certificate to be signed
*                 - the signature algorithm
*                 - the signature value
*
*            SEQUENCE {
*                SEQUENCE {
*                    [0] {
*                        INTEGER 2
*                        }
*                    INTEGER
*                     5E EB E4 CB 24 23 90 EF 72 AE 44 79 40 50 DE 2F
*                    SEQUENCE {
*                        OBJECT IDENTIFIER md5withRSAEncryption (1 2 840 113549 1 1 4)
*                        NULL
*                        }
*                    .../...
*
*                SEQUENCE {
*                    OBJECT IDENTIFIER md5withRSAEncryption (1 2 840 113549 1 1 4)
*                    NULL
*                    }
*                BIT STRING 0 unused bits
*                    A8 88 57 C0 6D AD 3B 35 8D 64 00 72 B8 D5 BE 46
*                    .../...
*                }
* Arguments    : p_X509_cert    :[in]       :address of x.509 certificate
*                sizeof_cert    :[in]       :byte size of x.509 certificate
*                p_fields_cert  :[out]      :list of ASN1 DER objects
*                p_nb_items     :[in/out]   :the number of objects found
* Return Value : ER_OK
*                ER_L_X509_INVALID_ARG
***************************************************************************************/
static uint32_t l_X509_ParseTop (const uint8_t  *p_X509_cert,
                                       uint16_t  sizeof_cert,
                                       asn1_t   *p_fields_cert,
                                       uint32_t *p_nb_items)
{
    uint32_t    max_nb_items;
    uint32_t    err;
    asn1_t      top_field[2];
    
    err = ER_L_X509_INVALID_ARG;
    
    max_nb_items = *p_nb_items;
    if (max_nb_items < 3u)
    {
        /* Do nothing */
    }
    else
    {
        /* get the top sequence member */
        *p_nb_items = 2;
        err = l_X509_ParseDerFormat(p_X509_cert, sizeof_cert, top_field, p_nb_items);
        if (err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            /* should have only 1 member */
            if (*p_nb_items != 1u)
            {
                err = ER_L_X509_INVALID_ARG;
            }
            else
            {
                *p_nb_items = max_nb_items;
                err = l_X509_ParseDerFormat(top_field[0].p_data, top_field[0].size, p_fields_cert, p_nb_items);
                if (err != ER_OK)
                {
                    /* Do nothing */
                }
                else
                {
                    /* should have only 3 members */
                    if (*p_nb_items != 3u)
                    {
                        err = ER_L_X509_INVALID_ARG;
                    }
                    else
                    {
                        /* all good */
                        err = ER_OK;
                    }
                }
            }
        }
    }

    return (err);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_X509_0005]:[ICUMFW_UD_L_CRY_X509_0005]
* Function name: l_X509_IdentifyKeyType
* Description  : Identify RSA KEY or ECC KEY
* Arguments    : p_part_cert    [in]       :list of ASN1 DER objects
*                p_key_type     [out]      :Range :E_KEY_TYPE_UNKNOWN
*                                                  E_KEY_TYPE_RSA_PUB_KEY
*                                                  E_KEY_TYPE_ECC_PUB_KEY
* Return Value : E_KEY_TYPE_UNKNOWN
*                E_KEY_TYPE_RSA_PUB_KEY
*                E_KEY_TYPE_ECC_PUB_KEY
***************************************************************************************/
static uint32_t l_X509_IdentifyKeyType(asn1_t *p_part_cert, key_type_t *p_key_type)
{
    asn1_t      fields_temp[2];
    uint8_t     compare;
    uint32_t    OID[16];
    uint32_t    n;
    uint32_t    err;
    
    *p_key_type = E_KEY_TYPE_UNKNOWN;
        
    /* decode the DER in OID fields */
    n = 2;
    err = l_X509_ParseDerFormat(p_part_cert->p_data, p_part_cert->size, fields_temp, &n);
    if (err != ER_OK)
    {
        /* Do nothing */
    }
    else
    {
        n = sizeof(OID) / sizeof(OID[0]);
        err = l_X509_DecodeFromDerFormat(&fields_temp[0], (uint8_t *)OID, &n);
        if (err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            compare = l_CMN_MemCmp((const uint8_t *)OID, (const uint8_t *)OID_xxxWithRSAEncryption, (n - 1u) * 4u);
            if ((uint32_t)compare == 0u)
            {
                /* this is a RSA key */
                /* search for the type of key */
                if (OID[n-1u] == OID_RSA_ENCRYPTION)
                {
                    *p_key_type = E_KEY_TYPE_RSA_PUB_KEY;
                }
                else
                {
                    /* Do nothing */
                }
            }
            else 
            {
                compare = l_CMN_MemCmp((const uint8_t *)OID, (const uint8_t *)OID_ecPublicKey, n * 4u);
                if ((uint32_t)compare == 0u)
                {
                    /* this is an ECC key */
                    *p_key_type = E_KEY_TYPE_ECC_PUB_KEY;
                }
                else
                {
                    /* Do nothing */
                }
            }
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_X509_0004]:[ICUMFW_UD_L_CRY_X509_0004]
* Function name: l_X509_IdentifyCurveId
* Description  : Identify ECC Curve Id
* Arguments    : p_part_cert    :[in]       :list of ASN1 DER objects
*                p_curve_id     :[out]      Range:  LAST_ITEM_ECC_TYPE
*                                                   ECC_CURVE_NIST_P192
*                                                   ECC_CURVE_NIST_P224
*                                                   ECC_CURVE_NIST_P256
*                                                   ECC_CURVE_BRAINPOOL_P256
*                                                   ECC_CURVE_BRAINPOOL_P320
*                                                   ECC_CURVE_BRAINPOOL_P384
* Return Value : ER_OK
*                ER_L_X509_INVALID_ARG
***************************************************************************************/
static uint32_t l_X509_IdentifyCurveId(asn1_t *p_part_cert, r_ecc_curve_t *p_curve_id)
{
    uint8_t         compare;
    uint32_t        OID[16];
    uint32_t        n;
    uint32_t        err;

    *p_curve_id = LAST_ITEM_ECC_TYPE;

    /* decode the DER in OID fields */
    n = sizeof(OID) / sizeof(OID[0]);
    err = l_X509_DecodeFromDerFormat(p_part_cert, (uint8_t *)OID, &n);
    if (err != ER_OK)
    {
        /* Do nothing */
    }
    else
    {
        if (n == 7u)
        {
            compare = l_CMN_MemCmp((const uint8_t *)OID, (const uint8_t *)OID_curveX962, 6 * 4);
            if ((uint32_t)compare == 0u)
            {
                /* this is a X9.62 curve */
                switch (OID[n-1u])
                {
                    case OID_PRIME192V1:
                        *p_curve_id = ECC_CURVE_NIST_P192;
                        break;
                    case OID_PRIME256V1:
                        *p_curve_id = ECC_CURVE_NIST_P256;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else if (n == 5u)
        {
            compare = l_CMN_MemCmp((const uint8_t *)OID, (const uint8_t *)OID_curveSEC2, 4 * 4);
            if ((uint32_t)compare == 0u)
            {
                /* this is a SEC 2 curve */
                switch (OID[n-1u])
                {
                    case OID_SECP224R1:
                        *p_curve_id = ECC_CURVE_NIST_P224;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else if (n == 10u)
        {
            compare = l_CMN_MemCmp((const uint8_t *)OID, (const uint8_t *)OID_curveBPool, 9 * 4);
            if ((uint32_t)compare == 0u)
            {
                /* this is a Brain Pool curve */
                switch (OID[n-1u])
                {
                    case OID_BRAINPOOL_P256R1:
                        *p_curve_id = ECC_CURVE_BRAINPOOL_P256;
                        break;
                    case OID_BRAINPOOL_P320R1:
                        *p_curve_id = ECC_CURVE_BRAINPOOL_P320;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* not support curve */
        }
    }

    return (err);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_X509_0002]:[ICUMFW_UD_L_CRY_X509_0002]
* Function name: l_X509_ExtractPubKey
* Description  : RSA/ECC public key & signature extraction from DER encoded X.509 certificate
* Arguments    : p_part_cert                :[in]       :address of x.509 certificate
*                sizeof_cert                :[in]       :byte size of x.509 certificate
*                key_type                   :[out]      :RSA/ECC Key Type
*                p_signed_data              :[out]      :the result of the signed data in the buffer p_signed_data
*                p_signed_data_size_in_byte :[out]      :number of bytes signed data
*                p_field_data               :[out]      :list of ASN1 DER objects (key, signature)
* Return Value : ER_OK
*                ER_L_X509_KEYTYPE_UNKOWN
***************************************************************************************/
static uint32_t l_X509_ExtractPubKey (const uint8_t         *p_cert, 
                                            uint16_t        sizeof_cert,
                                            /*x509_sig_algo_t *p_sig_algo, */
                                            key_type_t      *p_key_type,
                                            uint8_t         **p_signed_data,
                                            uint16_t        *p_signed_data_size_in_byte,
                                            asn1_t          *p_field_data)
{
    asn1_t      fields_cert[3];
    asn1_t      fields_tbs_cert[32];
    asn1_t      fields_temp[8];

    uint32_t    number_items;
    int32_t     i;
    uint16_t    header_size;
    uint32_t    err;
    
    /* start the certificate parsing */
    number_items = 3u;
    err = l_X509_ParseTop(p_cert, sizeof_cert, fields_cert, &number_items);
    if (err != ER_OK)
    {
        goto end;
    }
    else
    {
        /* Do nothing */
    }
    
    /* at this stage in fields_cert[] */
    /* 1st member in the list (0) has the certificate to be signed */
    /* 2nd member in the list (1) has the signature algorithm */
    /* 3rd member in the list (2) has the signature value */

    /* calc header size */
    if (fields_cert[0].size < 128u)
    {
        header_size = 2u;
    }
    else if (fields_cert[0].size < 256u)
    {
       /* 0x30 0x81 LL */
        header_size = 3u;
    }
    else if (fields_cert[0].size < 65536u)
    {
       /* 0x30 0x82 LL LL */
        header_size = 4u;
    }
    else if (fields_cert[0].size < 16777216u)
    {
       /* 0x30 0x83 LL LL LL */
        header_size = 5u;
    }
    else
    {
       err = ER_L_X509_INVALID_ARG;
       goto end;
    }
    /* signed_data include header */
    *(p_signed_data)              = fields_cert[0].p_data - header_size;
    *(p_signed_data_size_in_byte) = (uint16_t)fields_cert[0].size + header_size;

    l_CMN_MemCpy((uint8_t *)&p_field_data[0], (const uint8_t *)&fields_cert[2], sizeof(asn1_t));
    
    /* parse the certificate to be signed */
    number_items = sizeof(fields_tbs_cert) / sizeof(asn1_t);
    err = l_X509_ParseDerFormat(fields_cert[0].p_data, fields_cert[0].size, fields_tbs_cert, &number_items);
    if (err != ER_OK)
    {
        goto end;
    }
    else
    {
        /* Do nothing */
    }
    
    /* search for the RSA or ECC public key */
    for (i = 0; i < (int32_t)number_items; i++)
    {
        uint32_t p;
        err = ER_L_X509_INVALID_ARG;
        if ((uint32_t)fields_tbs_cert[i].constructed != 0u)
        {
            p = sizeof(fields_temp) / sizeof(asn1_t);
            err = l_X509_ParseDerFormat(fields_tbs_cert[i].p_data, fields_tbs_cert[i].size, fields_temp, &p);
            /**********************************************************************************
            * looking for this (RSA public key):
            *      SEQUENCE {
            *          SEQUENCE {
            *              OBJECT IDENTIFIER rsaEncryption (1 2 840 113549 1 1 1)
            *              NULL
            *          }
            *          BIT STRING 0 unused bits, encapsulates {
            *              SEQUENCE {
            *                  INTEGER <modulus>
            *                  INTEGER <public exponent>
            *              }
            *          }
            *      }
            * looking for this (ECC public key):
            *      SEQUENCE {
            *          SEQUENCE {
            *              OBJECT IDENTIFIER ecPublicKey (1 2 840 10045 2 1)
            *              OBJECT IDENTIFIER sect571r1 (1 3 132 0 39)
            *          }
            *          BIT STRING {
            *              04 ... <ECC point>
            *              }
            *          }
            *      }
            ***********************************************************************************/
            if (err == ER_OK)
            {
                if ((p == 2u) && (fields_temp[0].constructed != 0u) && (fields_temp[1].type == ASN1_BIT_STRING))
                {
                    (void)l_X509_IdentifyKeyType(&fields_temp[0], p_key_type);
                    if (*p_key_type == E_KEY_TYPE_RSA_PUB_KEY)
                    {
                        /* found RSA public key */
                        /* first content byte is number of unused bits */
                        /* second content byte should be the start of the sequence (encapsulated integer) */
                        /* parsing the sequence should return 2 integers */
                        /* - first integer is the modulus */
                        /* - seconde integer is the public exponent */
                        p = sizeof(fields_temp) / sizeof(asn1_t);
                        err = l_X509_ParseDerFormat(fields_temp[1].p_data + 1, (fields_temp[1].size - 2u), fields_temp, &p);
                        if ((err == ER_OK) && (p == 1u))
                        {
                            /* get the pointers to the modulus and public exponent, then exit */
                            p = sizeof(fields_temp) / sizeof(asn1_t);
                            err = l_X509_ParseDerFormat(fields_temp[0].p_data, fields_temp[0].size, fields_temp, &p);
                            if ((p == 2u) && (fields_temp[0].type == ASN1_INTEGER) && (fields_temp[1].type == ASN1_INTEGER))
                            {
                                l_CMN_MemCpy((uint8_t *)&p_field_data[1], (const uint8_t *)&fields_temp[0], sizeof(asn1_t));
                                l_CMN_MemCpy((uint8_t *)&p_field_data[2], (const uint8_t *)&fields_temp[1], sizeof(asn1_t));
                            }
                            else
                            {
                                /* Do nothing */
                            }
                            goto end;
                        }
                        else
                        {
                            /* Do nothing */
                        }
                    }
                    else if (*p_key_type == E_KEY_TYPE_ECC_PUB_KEY)
                    {
                        /* found ECC public key */
                        /* first content byte indicate the key format as follows: */
                        /* - 0x04 : uncompressed (this format MUST be supported) */
                        /* - 0x02 or 0x03 : compressed (this format MAY be supported) */
                        /* in the uncompressed form, the X and Y coordinate are following */
                        /* for the compressed form, not sure... */
                        /* get the pointer to the X & Y coordinates */
                        l_CMN_MemCpy((uint8_t *)&p_field_data[1], (const uint8_t *)&fields_temp[1], sizeof(asn1_t));
                        /* get the pointer to the curve identifier, then exit */
                        p = sizeof(fields_temp) / sizeof(asn1_t);
                        err = l_X509_ParseDerFormat(fields_temp[0].p_data, fields_temp[0].size, fields_temp, &p);
                        l_CMN_MemCpy((uint8_t *)&p_field_data[2], (const uint8_t *)&fields_temp[1], sizeof(asn1_t));
                        goto end;
                    }
                    else
                    {
                        err = ER_L_X509_KEYTYPE_UNKOWN;
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Do nothing */
        }
    }

end:
    return (err);
}

/* MAX ECC KEYSIZE(byte) * 2 + 2 */
#define TEMP_BUFFER_SIZE    (((R_CRYPT_SMALLBIGNUM_BITSIZE + 7u) / 8u) * 2u + 2u)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_X509_0001]:[ICUMFW_UD_L_CRY_X509_0001]
* Function name: L_X509_ParseCertificate
* Description  : RSA/ECC public key & signature extraction from DER encoded X.509 certificate
* Arguments    : p_x509_cert_prm   [in/out]       :address of structure for parsing x.509 certificate
* Return Value : ER_OK
*                ER_L_X509_INVALID_ARG
*                ER_L_X509_BUFFER_OVERFLOW
*                ER_L_X509_INVALID_CURVE
*                ER_L_X509_KEYTYPE_UNKOWN
***************************************************************************************/
uint32_t L_X509_ParseCertificate(x509_cert_prm_t *p_x509_cert_prm)
{
    uint8_t     ecc_key_buffer[TEMP_BUFFER_SIZE];
    asn1_t      field_data[3];
    uint32_t    size;
    uint16_t    key_size;

    uint32_t    err;

    err = l_X509_ExtractPubKey(p_x509_cert_prm->p_cert, 
                               p_x509_cert_prm->cert_size_in_bytes,
                               /*&(x509_cert_prm->sig_algo),*/
                               &(p_x509_cert_prm->key_type),
                               &(p_x509_cert_prm->p_signed_data),
                               &(p_x509_cert_prm->signed_data_size_in_bytes),
                               field_data);
    if (err != ER_OK)
    {
        /* Do nothing */
    }
    else if (p_x509_cert_prm->key_type == E_KEY_TYPE_RSA_PUB_KEY)
    {
        /* dump the signature (0), the modulus (1) and the public exponent (2) */
        err = l_X509_DecodeFromDerFormat(&field_data[0],
                                        p_x509_cert_prm->p_signature,
                                        (uint32_t *)&(p_x509_cert_prm->sign_size_in_bytes) );
        if (err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            err = l_X509_DecodeFromDerFormat(&field_data[1],
                                            p_x509_cert_prm->key.rsa.p_modulus,
                                            (uint32_t *)&(p_x509_cert_prm->key.rsa.modulus_size_in_bytes) );
        }

        if (err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            err = l_X509_DecodeFromDerFormat(&field_data[2],
                                            p_x509_cert_prm->key.rsa.p_pub_exp,
                                            (uint32_t *)&(p_x509_cert_prm->key.rsa.pub_exp_size_in_bytes) );
        }
    }
    else if (p_x509_cert_prm->key_type == E_KEY_TYPE_ECC_PUB_KEY)
    {
        /* dump the signature (0) */
        err = l_X509_DecodeFromDerFormat(&field_data[0], 
                                        p_x509_cert_prm->p_signature,
                                        (uint32_t *)&(p_x509_cert_prm->sign_size_in_bytes) );
        if (err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            /* get the curve type */
            (void)l_X509_IdentifyCurveId(&field_data[2], &p_x509_cert_prm->curve_id);
            if (p_x509_cert_prm->curve_id == LAST_ITEM_ECC_TYPE)
            {
                err = ER_L_X509_INVALID_CURVE;
            }
            else
            {
                /* retrieve the public key point */
                key_size = (key_size_tab[p_x509_cert_prm->curve_id] + 7u) / 8u;
                if ((key_size == 0u) || ((key_size % 2u) != 0u))
                {
                    err = ER_L_X509_INVALID_ARG;
                }
                else
                {
                    /* decode the public key */
                    size = TEMP_BUFFER_SIZE;
                    err = l_X509_DecodeFromDerFormat(&field_data[1], &ecc_key_buffer[0], &size);
                }
            }
        }

        if (err != ER_OK)
        {
            /* Do nothing */
        }
        else
        {
            /* not sure why there is a leading 0; need to check later */
            switch (ecc_key_buffer[0])
            {
                case 0x04u:
                    if ((uint16_t)size == (2u * key_size + 1u))
                    {
                        /* uncompressed form: OK! */
                        /* get X and y coordinate */
                        l_CMN_MemCpy(p_x509_cert_prm->key.ecc.p_pub_point_x, &ecc_key_buffer[1],             key_size);
                        l_CMN_MemCpy(p_x509_cert_prm->key.ecc.p_pub_point_y, &ecc_key_buffer[1u + key_size], key_size);
                        p_x509_cert_prm->key.ecc.point_x_size_in_bytes = key_size;
                        p_x509_cert_prm->key.ecc.point_y_size_in_bytes = key_size;
                    }
                    else
                    {
                        err = ER_L_X509_INVALID_ARG;
                    }
                    break;
                case 0x00u:
                    /* point at infinity */
                    err = ER_L_X509_INVALID_ARG;
                    break;
                case 0x02u:
                case 0x03u:
                    /* compressed form (only x coordinate) not support currently */
                    err = ER_L_X509_INVALID_ARG;
                    break;
                default:
                    /* invalid x.509 certificate */
                    err = ER_L_X509_INVALID_ARG;
                    break;
            }
        }
    }
    else
    {
        /* key type is unknown */
        err = ER_L_X509_KEYTYPE_UNKOWN;
    }

    return (err);
}
