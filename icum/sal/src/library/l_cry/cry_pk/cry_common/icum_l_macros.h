/******************************************************************************/
/* Component Name   Cryptographic Library                                     */
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

#ifndef ICUM_L_MACROS_H
#define ICUM_L_MACROS_H


#define CONST64(n) n ## ULL

/* ---- HELPER MACROS ---- */

/**********************************************************************************
* added by F. Poulard / Renesas Electronics
* - specific store / load macro using RH850 assembly
***********************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_HASH_0038]:[ICUMFW_UD_L_CRY_HASH_0038]
* Function name: BSW_CRY
* Description  : Byte Swap
* Arguments    : a    UINT 32 bit Source Data
*                
* Return Value : Byte Swapped Data
***************************************************************************************/
static asm uint32_t BSW_CRY(a)
{
%reg a
    bsw    a, r10
%nearmem a
    ld.w   a, r10
    bsw    r10, r10
%con a
    mov    a, r10
    bsw    r10, r10
%farsprel a
    movhi  hi(%SPOFF(a)), sp, r6
    ld.w   lo(%SPOFF(a))[r6], r10
    bsw    r10, r10
%farmem a
    movhi  hi(a), r0, r6
    ld.w   lo(a)[r6], r10
    bsw    r10, r10
%mem a
    ld.w    a[r0], r10
    bsw     r10, r10
%error
}

#define STORE32H(x, y)  ( *(uint32_t *)(y) = BSW_CRY(x) )
#define LOAD32H(x, y)   ( (x) = BSW_CRY(*(uint32_t *)(y)) )

/**********************************************************************************
* added by F. Poulard / Renesas Electronics
* - specific store / load macro using RH850 assembly
***********************************************************************************/

#define STORE64H(x, y)                                                      \
    do {                                                     \
        *((uint32_t *)&(y)[0]) = BSW_CRY((uint32_t)((x) >> 32u)); \
        *((uint32_t *)&(y)[4]) = BSW_CRY((uint32_t)(x));         \
    } while(0)

#define LOAD64H(x, y)                                                       \
    ( (x) = ( ((uint64_t )(BSW_CRY(*((uint32_t *)&(y)[0])))) << 32 ) | ( ((uint64_t )(BSW_CRY(*((uint32_t *)&(y)[4])))) & 0x0FFFFFFFF ) )



#define STORE32L(x, y)  ( *(uint32_t *)(y) = (x) )
#define LOAD32L(x, y)   ( (x) = *(uint32_t *)(y) )

#define STORE64L(x, y)                                              \
    do {                                                            \
        *((uint32_t *)&(y)[4]) = (uint32_t)((x) >> (uint32_t)32);   \
        *((uint32_t *)&(y)[0]) = (uint32_t)(x);                     \
    } while(0)
#define LOAD64L(x, y)   \
    ( (x) = ((uint64_t)(*((uint32_t *)&(y)[4])) << 32u) | ((*((uint32_t *)&(y)[0])) & 0x0FFFFFFFF) )



/* 32-bit Rotates */
#define ROL(x, y)   \
    ( (((uint32_t)(x)<<((y)&31u)) | (((uint32_t)(x)&0xFFFFFFFFuL)>>((32u-((y)&31u))&31u)))  \
        & 0xFFFFFFFFuL)
#define ROR(x, y)   \
    ( ((((uint32_t)(x)&0xFFFFFFFFuL)>>((y)&31u)) | ((uint32_t)(x)<<((32u-((y)&31u))&31u)))  \
        & 0xFFFFFFFFuL)
#define ROLc(x, y)  \
    ( (((uint32_t)(x)<<((y)&31u)) | (((uint32_t)(x)&0xFFFFFFFFuL)>>((32u-((y)&31u))&31u)))  \
        & 0xFFFFFFFFuL)
#define RORc(x, y)  \
    ( ((((uint32_t)(x)&0xFFFFFFFFuL)>>((y)&31u)) | ((uint32_t)(x)<<((32u-((y)&31u))&31u)))  \
        & 0xFFFFFFFFuL)


/* 64-bit Rotates */
#define ROL64(x, y) \
    ( (((x)<<((uint64_t)(y)&(uint64_t)63)) | (((x)&CONST64(0xFFFFFFFFFFFFFFFF))>>(((uint64_t)64-((y)&63ULL))&63ULL))) \
        & CONST64(0xFFFFFFFFFFFFFFFF))

#define ROR64(x, y) \
    ( ((((x)&CONST64(0xFFFFFFFFFFFFFFFF))>>((uint64_t)(y)&CONST64(63))) | ((x)<<(((uint64_t)64-((y)&63ULL))&63ULL)))  \
        & CONST64(0xFFFFFFFFFFFFFFFF))

#define ROL64c(x, y) \
    ( (((x)<<((uint64_t)(y)&(uint64_t)63)) | (((x)&CONST64(0xFFFFFFFFFFFFFFFF))>>(((uint64_t)64-((y)&63ULL))&63ULL))) \
        & CONST64(0xFFFFFFFFFFFFFFFF))

#define ROR64c(x, y) \
    ( ((((x)&CONST64(0xFFFFFFFFFFFFFFFF))>>((uint64_t)(y)&CONST64(63))) | ((x)<<(((uint64_t)64-((y)&63ULL))&63ULL)))  \
        & CONST64(0xFFFFFFFFFFFFFFFF))

#ifndef MAX
   #define MAX(x, y) ( ((x)>(y))?(x):(y) )
#endif

#ifndef MIN
   #define MIN(x, y) ( ((x)<(y))?(x):(y) )
#endif

#endif /* ICUM_L_MACROS_H */
