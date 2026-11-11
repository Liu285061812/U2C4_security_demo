/******************************************************************************/
/* Component Name   Cryptographic Library HASH                                */
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

#include "icum_l_hash.h"
#include "../cry_common/icum_l_macros.h"
#include "../cry_common/icum_l_my_toolbox.h"

/* the K array */
static const uint64_t K[80] = {
CONST64(0x428a2f98d728ae22), CONST64(0x7137449123ef65cd), 
CONST64(0xb5c0fbcfec4d3b2f), CONST64(0xe9b5dba58189dbbc),
CONST64(0x3956c25bf348b538), CONST64(0x59f111f1b605d019), 
CONST64(0x923f82a4af194f9b), CONST64(0xab1c5ed5da6d8118),
CONST64(0xd807aa98a3030242), CONST64(0x12835b0145706fbe), 
CONST64(0x243185be4ee4b28c), CONST64(0x550c7dc3d5ffb4e2),
CONST64(0x72be5d74f27b896f), CONST64(0x80deb1fe3b1696b1), 
CONST64(0x9bdc06a725c71235), CONST64(0xc19bf174cf692694),
CONST64(0xe49b69c19ef14ad2), CONST64(0xefbe4786384f25e3), 
CONST64(0x0fc19dc68b8cd5b5), CONST64(0x240ca1cc77ac9c65),
CONST64(0x2de92c6f592b0275), CONST64(0x4a7484aa6ea6e483), 
CONST64(0x5cb0a9dcbd41fbd4), CONST64(0x76f988da831153b5),
CONST64(0x983e5152ee66dfab), CONST64(0xa831c66d2db43210), 
CONST64(0xb00327c898fb213f), CONST64(0xbf597fc7beef0ee4),
CONST64(0xc6e00bf33da88fc2), CONST64(0xd5a79147930aa725), 
CONST64(0x06ca6351e003826f), CONST64(0x142929670a0e6e70),
CONST64(0x27b70a8546d22ffc), CONST64(0x2e1b21385c26c926), 
CONST64(0x4d2c6dfc5ac42aed), CONST64(0x53380d139d95b3df),
CONST64(0x650a73548baf63de), CONST64(0x766a0abb3c77b2a8), 
CONST64(0x81c2c92e47edaee6), CONST64(0x92722c851482353b),
CONST64(0xa2bfe8a14cf10364), CONST64(0xa81a664bbc423001),
CONST64(0xc24b8b70d0f89791), CONST64(0xc76c51a30654be30),
CONST64(0xd192e819d6ef5218), CONST64(0xd69906245565a910), 
CONST64(0xf40e35855771202a), CONST64(0x106aa07032bbd1b8),
CONST64(0x19a4c116b8d2d0c8), CONST64(0x1e376c085141ab53), 
CONST64(0x2748774cdf8eeb99), CONST64(0x34b0bcb5e19b48a8),
CONST64(0x391c0cb3c5c95a63), CONST64(0x4ed8aa4ae3418acb), 
CONST64(0x5b9cca4f7763e373), CONST64(0x682e6ff3d6b2b8a3),
CONST64(0x748f82ee5defb2fc), CONST64(0x78a5636f43172f60), 
CONST64(0x84c87814a1f0ab72), CONST64(0x8cc702081a6439ec),
CONST64(0x90befffa23631e28), CONST64(0xa4506cebde82bde9), 
CONST64(0xbef9a3f7b2c67915), CONST64(0xc67178f2e372532b),
CONST64(0xca273eceea26619c), CONST64(0xd186b8c721c0c207), 
CONST64(0xeada7dd6cde0eb1e), CONST64(0xf57d4f7fee6ed178),
CONST64(0x06f067aa72176fba), CONST64(0x0a637dc5a2c898a6), 
CONST64(0x113f9804bef90dae), CONST64(0x1b710b35131c471b),
CONST64(0x28db77f523047d84), CONST64(0x32caab7b40c72493), 
CONST64(0x3c9ebe0a15c9bebc), CONST64(0x431d67c49c100d4c),
CONST64(0x4cc5d4becb3e42b6), CONST64(0x597f299cfc657e2a), 
CONST64(0x5fcb6fab3ad6faec), CONST64(0x6c44198c4a475817)
};

/* Various logical functions */
#define Ch(x,y,z)       ((z) ^ ((x) & ((y) ^ (z))))
#define Maj(x,y,z)      ((((x) | (y)) & (z)) | ((x) & (y))) 
#define S(x, n)         ROR64c(x, n)
#define R(x, n)         (((x)&CONST64(0xFFFFFFFFFFFFFFFF))>>((uint64_t)(n)))
#define Sigma0(x)       (S(x, (uint64_t)28) ^ S(x, (uint64_t)34) ^ S(x, (uint64_t)39))
#define Sigma1(x)       (S(x, (uint64_t)14) ^ S(x, (uint64_t)18) ^ S(x, (uint64_t)41))
#define Gamma0(x)       (S(x, (uint64_t)1)  ^ S(x, (uint64_t)8)  ^ R(x, (uint64_t)7))
#define Gamma1(x)       (S(x, (uint64_t)19) ^ S(x, (uint64_t)61) ^ R(x, (uint64_t)6))

/* compress 1024-bits */
/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0035]:[ICUMFW_UD_L_CRY_HASH_0035]
* Function name: l_HASH_Sha512Compress()
* Description  : # HASH the 128-byte data.
* Arguments    : (IN-OUT)  md     : HASH state
*                (IN)      buf   : work buffer
* Return Value : 
****************************************************************************************/

static uint32_t l_HASH_Sha512Compress(hash_inner_state_t * md, const uint8_t *buf)
{
    uint64_t S[8], W[80], t0, t1;
    uint32_t  i;

    /* copy state into S */
    for (i = 0U; i < 8U; i++)
    {
        S[i] = md->sha512.state[i];
    }

    /* copy the state into 1024-bits into W[0..15]   "W [i] = 0ULL" for countermeasures against CERT-c.  */
    for (i = 0U; i < 16U; i++)
    {
        W[i] = 0ULL;
        LOAD64H(W[i], buf + (8U*i));
    }

    /* fill W[16..79]   "W [i] = 0ULL" for countermeasures against CERT-c.  */
    for (i = 16U; i < 80U; i++)
    {
        W[i] = 0ULL;
        W[i] = Gamma1(W[i - 2U]) + W[i - 7U] + Gamma0(W[i - 15U]) + W[i - 16U];
    }

    /* Compress */
#define RND(a,b,c,d,e,f,g,h,i)                          \
    do {                                                  \
        t0 = (h) + Sigma1(e) + Ch(e, f, g) + K[i] + W[i]; \
        t1 = Sigma0(a) + Maj(a, b, c);                    \
        (d) += t0;                                        \
        (h)  = t0 + t1;                                   \
    } while(0)

    for (i = 0U; i < 80U; i += 8U)
    {
        RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],(uint64_t)(i+0U));
        RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],(uint64_t)(i+1U));
        RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],(uint64_t)(i+2U));
        RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],(uint64_t)(i+3U));
        RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],(uint64_t)(i+4U));
        RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],(uint64_t)(i+5U));
        RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],(uint64_t)(i+6U));
        RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],(uint64_t)(i+7U));
    }

    /* feedback */
    for (i = 0U; i < 8U; i++)
    {
        md->sha512.state[i] = md->sha512.state[i] + S[i];
    }

    return(ER_OK);
}


/**
   Initialize the hash state
   \param md   The hash state you wish to initialize
   \return ER_OK if successful
*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0036]:[ICUMFW_UD_L_CRY_HASH_0036]
* Function name: l_HASH_Sha512Init()
* Description  : # Initialize the working area of HASH with specified values.
* Arguments    : (IN-OUT)  md     : HASH state
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t l_HASH_Sha512Init(hash_inner_state_t * md)
{
    uint32_t    err;

    if (md == NULL)
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else
    {
        md->sha512.curlen = 0U;
        md->sha512.length = 0U;
        md->sha512.state[0] = CONST64(0x6a09e667f3bcc908);
        md->sha512.state[1] = CONST64(0xbb67ae8584caa73b);
        md->sha512.state[2] = CONST64(0x3c6ef372fe94f82b);
        md->sha512.state[3] = CONST64(0xa54ff53a5f1d36f1);
        md->sha512.state[4] = CONST64(0x510e527fade682d1);
        md->sha512.state[5] = CONST64(0x9b05688c2b3e6c1f);
        md->sha512.state[6] = CONST64(0x1f83d9abfb41bd6b);
        md->sha512.state[7] = CONST64(0x5be0cd19137e2179);

        err = ER_OK;
    }

    return (err);
}

/**
   Process a block of memory though the hash
   \param md     The hash state
   \param in     The data to hash
   \param inlen  The length of the data (octets)
   \return ER_OK if successful
*/

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0034]:[ICUMFW_UD_L_CRY_HASH_0034]
* Function name: l_HASH_Sha512Process()
* Description  : # HASH the data for the length indicated by the message length.
* Arguments    : (IN-OUT)  md     : HASH state
*                (IN-OUT)  in    : Input message
*                (IN-OUT)  inlen  : Input message length
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/

uint32_t l_HASH_Sha512Process(hash_inner_state_t * md, const uint8_t *in, uint32_t inlen)
{
    uint32_t    n;
    uint32_t    err;
    
    if (md == NULL)
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else if (md->sha512.curlen > sizeof(md->sha512.buf))
    {
       err = ER_L_HASH_INVALID_ARG;
    }
    else
    {
        while (inlen > 0U)
        {
            if ((md->sha512.curlen == 0u) && (inlen >= 128u))
            {
                (void)l_HASH_Sha512Compress(md, in);
                md->sha512.length   += ((uint64_t)128 * (uint64_t)8);
                in                  += 128U;
                inlen               -= 128U;
            }
            else
            {
                n = MIN(inlen, (128U - md->sha512.curlen));
                l_CMN_MemCpy(md->sha512.buf + md->sha512.curlen, in, n);
                md->sha512.curlen   += n;
                in                  += n;
                inlen               -= n;
                if (md->sha512.curlen == 128U)
                {
                    (void)l_HASH_Sha512Compress(md, md->sha512.buf);

                    md->sha512.length += (uint64_t)(8 * 128);
                    md->sha512.curlen = 0;
                }
            }
        }
        err = ER_OK;
    }

    return (err);
}


/**s
   Terminate the hash to get the digest
   \param md  The hash state
   \param out [out] The destination of the hash (64 bytes)
   \return ER_OK if successful
*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0037]:[ICUMFW_UD_L_CRY_HASH_0037]
* Function name: l_HASH_Sha512Done()
* Description  : # Output the HASH value.
* Arguments    : (IN-OUT)  md     : HASH state
*                (IN-OUT)  out    : HASH value output destination
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t l_HASH_Sha512Done(hash_inner_state_t * md, uint8_t *out)
{
    uint32_t ai_loop;
    uint32_t err;

    if (md == NULL)
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else if (md->sha512.curlen >= sizeof(md->sha512.buf))
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else
    {
        /* increase the length of the message */
        md->sha512.length += md->sha512.curlen * CONST64(8);

        /* append the '1' bit */
        md->sha512.buf[md->sha512.curlen++] = (uint8_t)0x80;

        /* if the length is currently above 112 bytes we append zeros
        * then compress.  Then we can fall back to padding zeros and length
        * encoding like normal.
        */
        if (md->sha512.curlen > 112u)
        {
            while (md->sha512.curlen < 128u)
            {
                md->sha512.buf[md->sha512.curlen++] = (uint8_t)0;
            }
            (void)l_HASH_Sha512Compress(md, md->sha512.buf);
            md->sha512.curlen = 0U;
        }
        else
        {
            /* Do nothing */
        }

        /* pad upto 120 bytes of zeroes 
        * note: that from 112 to 120 is the 64 MSB of the length.  We assume that you won't hash
        * > 2^64 bits of data... :-)
        */
        while (md->sha512.curlen < 120U)
        {
            md->sha512.buf[md->sha512.curlen++] = (uint8_t)0;
        }

        /* store length */
        STORE64H(md->sha512.length, md->sha512.buf+120U);
        (void)l_HASH_Sha512Compress(md, md->sha512.buf);

        /* copy output */
        for (ai_loop = 0U; ai_loop < 8U; ai_loop++)
        {
            STORE64H(md->sha512.state[ai_loop], out + (8U * ai_loop));
        }
        err = ER_OK;
    }

    return (err);
}

/**
   Initialize the hash state
   \param md   The hash state you wish to initialize
   \return ER_OK if successful
*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0032]:[ICUMFW_UD_L_CRY_HASH_0032]
* Function name: l_HASH_Sha384Init()
* Description  : # Initialize the working area of HASH with specified values.
* Arguments    : (IN-OUT)  md     : HASH state
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t l_HASH_Sha384Init(hash_inner_state_t * md)
{
    uint32_t err;

    if (md == NULL)
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else
    {
        md->sha512.curlen = 0U;
        md->sha512.length = 0ULL;
        md->sha512.state[0] = CONST64(0xcbbb9d5dc1059ed8);
        md->sha512.state[1] = CONST64(0x629a292a367cd507);
        md->sha512.state[2] = CONST64(0x9159015a3070dd17);
        md->sha512.state[3] = CONST64(0x152fecd8f70e5939);
        md->sha512.state[4] = CONST64(0x67332667ffc00b31);
        md->sha512.state[5] = CONST64(0x8eb44a8768581511);
        md->sha512.state[6] = CONST64(0xdb0c2e0d64f98fa7);
        md->sha512.state[7] = CONST64(0x47b5481dbefa4fa4);

        err = ER_OK;
    }

    return (err);
}

/**
   Terminate the hash to get the digest
   \param md  The hash state
   \param out [out] The destination of the hash (48 bytes)
   \return ER_OK if successful
*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0033]:[ICUMFW_UD_L_CRY_HASH_0033]
* Function name: l_HASH_Sha384Done()
* Description  : # Output the HASH value.
* Arguments    : (IN-OUT)  md     : HASH state
*                (IN-OUT)  out    : HASH value output destination
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t l_HASH_Sha384Done(hash_inner_state_t * md, uint8_t *out)
{
    uint8_t buf[64];
    uint32_t    err;

    if (md == NULL)
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else
    {
        err = l_HASH_Sha512Done(md, buf);
        l_CMN_MemCpy(out, buf, (uint32_t)48);
    }

    return (err);
}

