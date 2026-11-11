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
/* For the used GHS macro functions */
#include "icum_d_misc_pub.h"


/**********************************************************************************
* K array defined without condition + force alignment on 32-bit boundary
***********************************************************************************/
ALIGNVAR(4)
/* the K array */
static const uint32_t K[64] = {
    0x428a2f98U, 0x71374491U, 0xb5c0fbcfU, 0xe9b5dba5U, 0x3956c25bU,
    0x59f111f1U, 0x923f82a4U, 0xab1c5ed5U, 0xd807aa98U, 0x12835b01U,
    0x243185beU, 0x550c7dc3U, 0x72be5d74U, 0x80deb1feU, 0x9bdc06a7U,
    0xc19bf174U, 0xe49b69c1U, 0xefbe4786U, 0x0fc19dc6U, 0x240ca1ccU,
    0x2de92c6fU, 0x4a7484aaU, 0x5cb0a9dcU, 0x76f988daU, 0x983e5152U,
    0xa831c66dU, 0xb00327c8U, 0xbf597fc7U, 0xc6e00bf3U, 0xd5a79147U,
    0x06ca6351U, 0x14292967U, 0x27b70a85U, 0x2e1b2138U, 0x4d2c6dfcU,
    0x53380d13U, 0x650a7354U, 0x766a0abbU, 0x81c2c92eU, 0x92722c85U,
    0xa2bfe8a1U, 0xa81a664bU, 0xc24b8b70U, 0xc76c51a3U, 0xd192e819U,
    0xd6990624U, 0xf40e3585U, 0x106aa070U, 0x19a4c116U, 0x1e376c08U,
    0x2748774cU, 0x34b0bcb5U, 0x391c0cb3U, 0x4ed8aa4aU, 0x5b9cca4fU,
    0x682e6ff3U, 0x748f82eeU, 0x78a5636fU, 0x84c87814U, 0x8cc70208U,
    0x90befffaU, 0xa4506cebU, 0xbef9a3f7U, 0xc67178f2U
};
/* #endif */

/* Various logical functions */
#define Ch(x,y,z)       ((z) ^ ((x) & ((y) ^ (z))))
#define Maj(x,y,z)      ((((x) | (y)) & (z)) | ((x) & (y))) 
#define S(x, n)         RORc((x),(n))
#define R(x, n)         (((x)&0xFFFFFFFFU)>>(n))
#define Sigma0(x)       (S(x, 2U) ^ S(x, 13U) ^ S(x, 22U))
#define Sigma1(x)       (S(x, 6U) ^ S(x, 11U) ^ S(x, 25U))
#define Gamma0(x)       (S(x, 7U) ^ S(x, 18U) ^ R(x, 3U))
#define Gamma1(x)       (S(x, 17U) ^ S(x, 19U) ^ R(x, 10U))


#undef RND

/* #pragma ghs section text    = ".icum_library_ram" */
/* compress 512-bits */

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0028]:[ICUMFW_UD_L_CRY_HASH_0028]
* Function name: l_HASH_Sha256Compress()
* Description  : # HASH the 64-byte data.
* Arguments    : (IN-OUT)  md     : HASH state
*                (IN)      buf   : work buffer
* Return Value : 
****************************************************************************************/
static uint32_t l_HASH_Sha256Compress(hash_inner_state_t * md, const uint8_t *buf)
{
    uint32_t S[8], W[64], t0, t1;
    uint32_t  i;
    uint32_t  times;

    /* copy state into S */
    for (i = 0u; i < 8U; i++)
    {
        S[i] = md->sha256.state[i];
    }

    /* copy the state into 512-bits into W[0..15]   "W [i] = 0U" for countermeasures against CERT-c. */
    for (i = 0U; i < 16U; i++)
    {
        W[i] = 0U;
        LOAD32H(W[i], buf + (4U*i));
    }

    /* fill W[16..63]  "W [i] = 0U" for countermeasures against CERT-c. */
    for (i = 16U; i < 64U; i++)
    {
        W[i] = 0U;
        W[i] = Gamma1(W[i - 2U]) + W[i - 7U] + Gamma0(W[i - 15U]) + W[i - 16U];
    }        

    /* Compress */
#define RND(a,b,c,d,e,f,g,h,i,ki)                     \
    do {                                                  \
        t0 = (h) + Sigma1(e) + Ch(e, f, g) + (ki) + W[i]; \
        t1 = Sigma0(a) + Maj(a, b, c);                    \
        (d) += t0;                                        \
        (h)  = t0 + t1;                                   \
    } while(0)

    for (i = 0U; i < 8U; i++)
    {
        times = i * 8U;
        RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],0U + times, K[0U + times]);
        RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],1U + times, K[1U + times]);
        RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],2U + times, K[2U + times]);
        RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],3U + times, K[3U + times]);
        RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],4U + times, K[4U + times]);
        RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],5U + times, K[5U + times]);
        RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],6U + times, K[6U + times]);
        RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],7U + times, K[7U + times]);
    }

#undef RND

    /* feedback */
    for (i = 0U; i < 8U; i++)
    {
        md->sha256.state[i] = md->sha256.state[i] + S[i];
    }

    return(ER_OK);
}

/* #pragma ghs section text    = default */

/**********************************************************************************
* code moved to specific section
***********************************************************************************/
/* #pragma ghs section text=".r_crypt.text.rel" */

/**
   Initialize the hash state
   \param md   The hash state you wish to initialize
   \return ER_OK if successful
*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0030]:[ICUMFW_UD_L_CRY_HASH_0030]
* Function name: l_HASH_Sha256Init()
* Description  : # Initialize the working area of HASH with specified values.
* Arguments    : (IN-OUT)  md     : HASH state
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t l_HASH_Sha256Init(hash_inner_state_t * md)
{
    uint32_t    err;

    if (md == NULL)
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else
    {
        md->sha256.curlen = 0U;
        md->sha256.length = 0ULL;
        md->sha256.state[0] = 0x6A09E667U;
        md->sha256.state[1] = 0xBB67AE85U;
        md->sha256.state[2] = 0x3C6EF372U;
        md->sha256.state[3] = 0xA54FF53AU;
        md->sha256.state[4] = 0x510E527FU;
        md->sha256.state[5] = 0x9B05688CU;
        md->sha256.state[6] = 0x1F83D9ABU;
        md->sha256.state[7] = 0x5BE0CD19U;
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
/**********************************************************************************
* code moved to default text section
***********************************************************************************/
/* #pragma ghs section text=default */

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0026]:[ICUMFW_UD_L_CRY_HASH_0026]
* Function name: l_HASH_Sha256Process()
* Description  : # HASH the data for the length indicated by the message length.
* Arguments    : (IN-OUT)  md     : HASH state
*                (IN-OUT)  in    : Input message
*                (IN-OUT)  inlen  : Input message length
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/

uint32_t l_HASH_Sha256Process(hash_inner_state_t * md, const uint8_t *in, uint32_t inlen)
{
    uint32_t    n;
    uint32_t    err;
    
    if (md == NULL)
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else if (md->sha256.curlen > sizeof(md->sha256.buf))
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else
    {
        while (inlen > 0u)
        {
            if ((md->sha256.curlen == 0u) && (inlen >= 64u))
            {
                (void)l_HASH_Sha256Compress(md, in);
                md->sha256.length   += (uint64_t)(64U * 8U);
                in                  += 64u;
                inlen               -= 64u;
            }
            else
            {
                n = MIN(inlen, (64U - md->sha256.curlen));
                l_CMN_MemCpy(md->sha256.buf + md->sha256.curlen, in, n);
                md->sha256.curlen += n;
                in                += n;
                inlen             -= n;
                if (md->sha256.curlen == 64u)
                {
                    (void)l_HASH_Sha256Compress(md, md->sha256.buf);
                    md->sha256.length += (uint64_t)(8U * 64U);
                    md->sha256.curlen = 0U;
                }
                else
                {
                    /* Do nothing */
                }
            }
        }
        err = ER_OK;
    }

    return (err);
}


/**********************************************************************************
* assigning ROM code and constants to specific sections
***********************************************************************************/
/* #pragma ghs section text=".r_crypt.text.rel"      */
/* #pragma ghs section rodata=".r_crypt.rodata.rel"  */

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0029]:[ICUMFW_UD_L_CRY_HASH_0029]
* Function name: l_HASH_Sha256CompressSlow()
* Description  : # HASH the 64-byte data.
* Arguments    : (IN-OUT)  md     : HASH state
*                (IN)      buf   : work buffer
* Return Value : 
****************************************************************************************/
static uint32_t l_HASH_Sha256CompressSlow(hash_inner_state_t * md, const uint8_t *buf)
{
    uint32_t S[8], W[64], t0, t1;
    uint32_t t;
    uint32_t  i;

    /* copy state into S */
    for (i = 0U; i < 8U; i++)
    {
        S[i] = md->sha256.state[i];
    }

    /* copy the state into 512-bits into W[0..15]   "W [i] = 0U" for countermeasures against CERT-c. */
    for (i = 0U; i < 16U; i++)
    {
        W[i] = 0U;
        LOAD32H(W[i], buf + (4U*i));
    }

    /* fill W[16..63]   "W [i] = 0U" for countermeasures against CERT-c.  */
    for (i = 16U; i < 64U; i++)
    {
        W[i] = 0U;
        W[i] = Gamma1(W[i - 2U]) + W[i - 7U] + Gamma0(W[i - 15U]) + W[i - 16U];
    }

    /* Compress */
#define RND(a,b,c,d,e,f,g,h,i)                           \
    do {                                                  \
        t0 = (h) + Sigma1(e) + Ch(e, f, g) + K[i] + W[i]; \
        t1 = Sigma0(a) + Maj(a, b, c);                    \
        (d) += t0;                                        \
        (h)  = t0 + t1;                                   \
    } while(0)

    for (i = 0U; i < 64U; ++i)
    {
         RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],i);
         t    = S[7];
         S[7] = S[6];
         S[6] = S[5];
         S[5] = S[4];
         S[4] = S[3];
         S[3] = S[2];
         S[2] = S[1];
         S[1] = S[0];
         S[0] = t;
    }


    /* feedback */
    for (i = 0U; i < 8U; i++)
    {
        md->sha256.state[i] = md->sha256.state[i] + S[i];
    }

    return(ER_OK);
}

/**********************************************************************************
* _sha256_compress function forced with LTC_SMALL_CODE undefined
* code moved to default text section
***********************************************************************************/
/* #pragma ghs section text=default   */
/* #pragma ghs section rodata=default */

/* #pragma ghs section text=".r_crypt.text.rel" */

/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0027]:[ICUMFW_UD_L_CRY_HASH_0027]
* Function name: l_HASH_Sha256ProcessSlow()
* Description  : # HASH the data for the length indicated by the message length.
* Arguments    : (IN-OUT)  md     : HASH state
*                (IN-OUT)  in    : Input message
*                (IN-OUT)  inlen  : Input message length
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t l_HASH_Sha256ProcessSlow(hash_inner_state_t * md, const uint8_t *in, uint32_t inlen)
{
    uint32_t    n;
    uint32_t    err;
    
    if (md == NULL)
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else if (md->sha256.curlen > sizeof(md->sha256.buf))
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else
    {
        while (inlen > 0u)
        {
            if ((md->sha256.curlen == 0U) && (inlen >= 64u))
            {
                (void)l_HASH_Sha256CompressSlow(md, in);
                md->sha256.length += ((uint64_t)64 * (uint64_t)8);
                in             += 64U;
                inlen          -= 64U;
            }
            else
            {
                n = MIN(inlen, (64U - md->sha256.curlen));
                l_CMN_MemCpy(md->sha256.buf + md-> sha256.curlen, in, n);
                md->sha256.curlen += n;
                in             += n;
                inlen          -= n;
                if (md->sha256.curlen == 64U)
                {
                    (void)l_HASH_Sha256CompressSlow(md, md->sha256.buf);
                    md->sha256.length += (uint64_t)(8 * 64);
                    md->sha256.curlen = 0;
                }
                else
                {
                    /* Do nothing */
                }
            }
        }
        err = ER_OK;
    }

    return (err);
}


/**
   Terminate the hash to get the digest
   \param md  The hash state
   \param out [out] The destination of the hash (32 bytes)
   \return ER_OK if successful
*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0031]:[ICUMFW_UD_L_CRY_HASH_0031]
* Function name: l_HASH_Sha256Done()
* Description  : # Output the HASH value.
* Arguments    : (IN-OUT)  md     : HASH state
*                (IN-OUT)  out    : HASH value output destination
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t l_HASH_Sha256Done(hash_inner_state_t * md, uint8_t *out)
{
    uint32_t i;
    uint32_t err;

    if (md == NULL)
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else if (md->sha256.curlen >= sizeof(md->sha256.buf))
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else
    {
        /* increase the length of the message */
        md->sha256.length += (uint64_t)(md->sha256.curlen * 8U);

        /* append the '1' bit */
        md->sha256.buf[md->sha256.curlen++] = (uint8_t)0x80;

        /* if the length is currently above 56 bytes we append zeros
        * then compress.  Then we can fall back to padding zeros and length
        * encoding like normal.
        */
        if (md->sha256.curlen > 56u)
        {
            while (md->sha256.curlen < 64u)
            {
                md->sha256.buf[md->sha256.curlen++] = (uint8_t)0;
            }
            (void)l_HASH_Sha256Compress(md, md->sha256.buf);
            md->sha256.curlen = 0u;
        }
        else
        {
            /* Do nothing */
        }

        /* pad upto 56 bytes of zeroes */
        while (md->sha256.curlen < 56u)
        {
            md->sha256.buf[md->sha256.curlen++] = (uint8_t)0;
        }

        /* store length */
        STORE64H(md->sha256.length, md->sha256.buf+56U);
        (void)l_HASH_Sha256Compress(md, md->sha256.buf);

        /* copy output */
        for (i = 0U; i < 8U; i++)
        {
            STORE32H(md->sha256.state[i], out+(4U*i));
        }
        err = ER_OK;
    }

    return (err);
}

/* #pragma ghs section text=default */


/* init the sha256 er... sha224 state ;-) */
/**
   Initialize the hash state
   \param md   The hash state you wish to initialize
   \return ER_OK if successful
*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0024]:[ICUMFW_UD_L_CRY_HASH_0024]
* Function name: l_HASH_Sha224Init()
* Description  : # Initialize the working area of HASH with specified values.
* Arguments    : (IN-OUT)  md     : HASH state
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t l_HASH_Sha224Init(hash_inner_state_t * md)
{
    uint32_t    err;

    if (md == NULL)
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else
    {
        md->sha256.curlen = 0U;
        md->sha256.length = 0ULL;
        md->sha256.state[0] = 0xc1059ed8U;
        md->sha256.state[1] = 0x367cd507U;
        md->sha256.state[2] = 0x3070dd17U;
        md->sha256.state[3] = 0xf70e5939U;
        md->sha256.state[4] = 0xffc00b31U;
        md->sha256.state[5] = 0x68581511U;
        md->sha256.state[6] = 0x64f98fa7U;
        md->sha256.state[7] = 0xbefa4fa4U;
        err = ER_OK;
    }

    return (err);
}

/**
   Terminate the hash to get the digest
   \param md  The hash state
   \param out [out] The destination of the hash (28 bytes)
   \return ER_OK if successful
*/
/****************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0025]:[ICUMFW_UD_L_CRY_HASH_0025]
* Function name: l_HASH_Sha224Done()
* Description  : # Output the HASH value.
* Arguments    : (IN-OUT)  md     : HASH state
*                (IN-OUT)  out    : HASH value output destination
* Return Value : ER_OK
*                ER_L_HASH_INVALID_ARG
****************************************************************************************/
uint32_t l_HASH_Sha224Done(hash_inner_state_t * md, uint8_t *out)
{
    uint8_t     buf[32];
    uint32_t    err;

    if (md == NULL)
    {
        err = ER_L_HASH_INVALID_ARG;
    }
    else
    {
        err = l_HASH_Sha256Done(md, buf);
        l_CMN_MemCpy(out, buf, (uint32_t)28);
    }

    return (err);
}



