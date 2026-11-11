/******************************************************************************/
/* Component Name   Testing the ICU-M                                         */
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

// ----------------------------------------------------------------------------------------------------
// vrun toolbox
// ----------------------------------------------------------------------------------------------------

// optimized memset() for a 32-bit CPU and for this application
void v850_memset(unsigned char *dest, unsigned char mychar, short size);

// optimized memcpy() for a 32-bit CPU
void v850_memcpy(unsigned char *dest, const unsigned char *source, short size);

// reading far data
// copy far source data in local dest
// add additionall data access latency
void v850_farread(unsigned char *dest, const unsigned char *far_source, short size);

// reading far data
// write far dest data with local source
// add additionall data access latency
void v850_farwrite(unsigned char *far_dest, const unsigned char *source, short size);

// optimized memcmp() for a 32-bit CPU and for this application
unsigned char v850_memcmp(const unsigned char *data1, const unsigned char *data2, short size);

// performs a XOR between source and dest, store the result in dest
void v850_xor(unsigned char *dest, const unsigned char *source, short size);

// performs a AND between source and dest, store the result in dest
void v850_and(unsigned char *dest, const unsigned char *source, short size);

// performs a OR between source and dest, store the result in dest
void v850_or(unsigned char *dest, const unsigned char *source, short size);

// performs an addition (data1 + data2) stores the result in data1
// examples:
// data1 = 00 FF 01, data2 = 00 00 FF, v850_add(L=3, lsb_first = 1) -> 01 00 00
// data1 = 00 FF 01, data2 = 00 01 00, v850_add(L=3, lsb_first = 0) -> 00 00 02
void v850_add(unsigned char *data1, const unsigned char *data2, unsigned short size, unsigned char lsb_first);

// performs a subtraction (data1 - data2) stores the result in data1
void v850_sub(unsigned char *data1, const unsigned char *data2, unsigned short size, unsigned char lsb_first);

// performs a binary shift to the left (from lsb to msb!)
// the shift is performed byte per bytes, no optimization seems to be possible
void v850_shift_left(unsigned char *data1, unsigned short size, unsigned char no_bits);

// performs a binary shift to the right (from msb to lsb!)
// the shift is performed byte per bytes, no optimization seems to be possible
void v850_shift_right(unsigned char *data1, unsigned short size, unsigned char no_bits);

// reading one word from far memory
#define v850_farread_oneword(far_source) \
    *(unsigned long *)(far_source); \
    DATA_ACCESS_LATENCY(1);

// writing one word in far memory
#define v850_farwrite_oneword(far_dest, myword) \
    DATA_ACCESS_LATENCY(1); \
    *(unsigned long *)(far_dest) = myword;

// ----------------------------------------------------------------------------------------------------
// ASM macro to enjoy the bsw macro instead of the original macro defined for LITTLE_ENDIAN
// far more better than the function v850_byte_swap()
// usage: 
//      any_data = V850_REVERSE_U32(any_data)
// example:
//      V850_REVERSE_U32(0x11223344) returns 0x44332211
// see GHS V5 help for more info on ASM macros in C
// ----------------------------------------------------------------------------------------------------
asm int V850_REVERSE_U32(a)
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
#define REV32(x)    V850_REVERSE_U32(x)
#define BSW(x)      V850_REVERSE_U32(x)

// ----------------------------------------------------------------------------------------------------
// the below macro is defined to put additional latency
// ----------------------------------------------------------------------------------------------------
asm void NOPS(n)
{
%con n
.rept n
    nop
.endr
}

#if !defined(LATENCY_CYCLES)
#define LATENCY_CYCLES      0
#endif
#define DATA_ACCESS_LATENCY(number_of_access)   NOPS(number_of_access * LATENCY_CYCLES)

/*===========================================================================
  Assembly macro to manipulate the system registers
  ===========================================================================*/
#if defined(__RH850__)  || defined( __V850E3__)
// set the system ID register regID with the content of the register regValue
asm void LDSR(regValue, regID, selID)
{
%nearmem regValue, %con regID, selID
    ld.w    regValue, r10
    ldsr    r10, regID, selID
%reg regValue, %con regID, selID
    ldsr    regValue, regID, selID
%error
}

// returns the value (in r10, as per the C calling convention) from the system ID register regID
asm unsigned long STSR(regID, selID)
{
%con regID, selID
    stsr    regID, r10, selID
%error
}
#else
// set the system ID register regID with the content of the register regValue
asm void LDSR(regValue, regID)
{
%nearmem regValue, %con regID
    ld.w    regValue, r10
    ldsr    r10, regID
%reg regValue, %con regID
    ldsr    regValue, regID
%error
}

// returns the value (in r10, as per the C calling convention) from the system ID register regID
asm unsigned long STSR(regID)
{
%con regID
    stsr    regID, r10
%error
}
#endif

// ---------------------------------------------------------------------------
//  System register definition to be used with above defined macro LDSR / STSR
// ---------------------------------------------------------------------------
#if defined(__RH850__)  || defined(__V850E3__)
    #define PSW         5, 0
    #define EIPSW       1, 0
    #define INTBP       4, 1
    #define MCTL        5, 1
    #define ICCFG       26, 4
    #define DCCFG       27, 4
#else
    #define FEPC        2
    #define MPM         0
    #define PSW         5
    #define BSEL        31
    #define DPA0L       16
    #define DPA0U       17
    #define DPA1L       18
    #define DPA1U       19
    #define DPA2L       20
    #define DPA2U       21
    #define DPA3L       22
    #define DPA3U       23
#endif

// ---------------------------------------------------------------------------
// Various flag settings for the system registers
// ---------------------------------------------------------------------------

// PSW flags
#define SET_DMP_FLAG    0x00020000
#define SET_UM_FLAG     0x40000000

// MPM flags
#define SET_MPE_FLAG    0x00000001
#define SET_AUE_FLAG    0x00000002
#define SET_SPS_FLAG    0x00000004

// DPAxL / DPAxU flags
#define DISABLE_WR      0x00000000
#define ENABLE_WR       0x00000004
#define DISABLE_RD      0x00000000
#define ENABLE_RD       0x00000002
#define DISABLE_PROT    0x00000000
#define ENABLE_PROT     0x00000001

// ---------------------------------------------------------------------------
// the below macro returns the value of the carry flag
// ---------------------------------------------------------------------------
asm unsigned long GETCARRY()
{
    setf    c, r10
}

// ---------------------------------------------------------------------------
// the below macros sets the device in supervisor or user mode (RH850)
// ---------------------------------------------------------------------------
#if defined(__RH850__)  || defined( __V850E2V3__)
#define SUPERVISOR_MODE() \
    LDSR((STSR(PSW) & (~SET_UM_FLAG)), PSW);

#define USER_MODE() \
    LDSR((STSR(PSW) | SET_UM_FLAG), PSW);
#endif

// ---------------------------------------------------------------------------
// the below macros are used for symbol concatenation
// ---------------------------------------------------------------------------
#define PASTER(x, y)        x ## y
#define EVALUATOR(x, y)     PASTER(x, y)

