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

#include    "renesas_types.h"
#include    "debug_sub.h"
#include    "pe2.h"
#include    "pe_reg.h"
#include    <string.h>
#include    "shared.h"
#include    "vrun.api.h"

/*--------------------------------------*/
void regWrite32(volatile unsigned int  *address, unsigned int  data)
{
    *address = data;
    return;
}/*--------------------------------------*/

#define UNLOCK_DATA (0xA5A5A501)
#define LOCK_DATA   (0xA5A5A500)
void protectRegWrite(volatile unsigned int *pAddress, volatile unsigned int *dAddress, unsigned int data)
{
  /* Enables write access of protected registers */
  regWrite32(pAddress, UNLOCK_DATA);

  /* Generate System Reset 2 */
  regWrite32(dAddress, data);

  /* Disables write access of protected registers */
  regWrite32(pAddress, LOCK_DATA);

  return;
}

/*------------------------------------------------------------------------------*/
/* SoftReset(For H / W control)                                                 */
/*------------------------------------------------------------------------------*/
void proc_SOFTRESET(void)
{
  protectRegWrite(&REG_PE_RESKCPROT, &REG_PE_SWSRESA, 0x00000001);
  return;
}

void console_get(void)
{
    char *p = vrun_iobuff_readparam(0);
    
    vrun_console_get();
    if(strncmp(p, "softreset", 9) == 0) 
    { 
        proc_SOFTRESET(); 
    } 
}

/*------------------------------------------------------------------------------*/
/* Debug control                                                                */
/*------------------------------------------------------------------------------*/
char *DivideCommand(char *commandP, unsigned int unitNo, unsigned int *len)
{
    unsigned int    cnt = 0;
    char            *p  = commandP;
    char            *rc = NULL;
    
    if (unitNo == 0UL) rc = p;
    *len = 0UL;
    
    while (1) {
        if (*p == 0x00) break;
        
        if (*p == ',')
        {
            if (rc != NULL) break;
            
            if (++cnt == unitNo)
            {
                rc = p + 1;
            }
        }
        else if((*p == '\r') && (*(p+1) == '\n'))
        {
            if (rc != NULL) break;
            
            if (++cnt == unitNo)
            {
                rc = p + 1;
            }
        }
        else
        {
            if (rc != NULL) (*len)++;
        }
        p++;
    }
    
    return(rc);
}

unsigned char ConvertCharToDec(char c)
{
    unsigned char   w = 0;

    if      ((c >= '0') && (c <= '9'))
    {
        w = c - '0';
    }

    return(w);
}

void ConvertStrToDec(unsigned int *dst, char *src, unsigned int len)
{
    int i;

    *dst = 0;
    for (i = 0; i < len; i++) {
        *dst *= 10;
        *dst += (unsigned int)ConvertCharToDec(*src);
        src++;
    }
}

void ConvertStrToDec64(unsigned long long *dst, char *src, unsigned int len)
{
    int i;

    *dst = 0;
    for (i = 0; i < len; i++) {
        *dst *= 10;
        *dst += (unsigned long long)ConvertCharToDec(*src);
        src++;
    }
}

static unsigned char ConvertCharToHex(char c)
{
    unsigned char   w = 0;

    if      ((c >= '0') && (c <= '9'))
    {
        w = c - '0';
    }
    else if ((c >= 'A') && (c <= 'F'))
    {
        w = c - 'A' + 10;
    }
    else if ((c >= 'a') && (c <= 'f'))
    {
        w = c - 'a' + 10;
    }

    return(w);
}

unsigned char ConvertStrToHexSub(char *p)
{
    return((ConvertCharToHex(*p)<<4)+ConvertCharToHex(*(p+1)));
}

void ConvertStrToHex(char *dst, char *src, unsigned int len)
{
    unsigned int   i;

    for (i = 0; i < (len/2); i++) {
        *dst = ConvertStrToHexSub(src);
        dst++;
        src++;
        src++;
    }
}

static char ConvertHexToChar(unsigned char d)
{
    unsigned char   w;

    if ((d >= 0x0a) && (d <= 0x0f))
    {
        w = (d - 10) + 'a';
    }
    else
    {
        w =  d       + '0';
    }


    return(w);
}

static unsigned char ConvertHexToStrSub(char *p, int sw)
{
    if (sw == UPPER_NIBBLE)
    {
        return(ConvertHexToChar(((*p) >> 4) & 0x0f));
    }
    else
    {
        return(ConvertHexToChar( (*p)       & 0x0f));
    }
}

void ConvertHexToStr(char *dst, char *src, unsigned int len)
{
    int i;

    for (i = 0; i < len; i++) {
        *dst = ConvertHexToStrSub(src, UPPER_NIBBLE);
        dst++;
        *dst = ConvertHexToStrSub(src, LOWER_NIBBLE);
        dst++;
        src++;
    }
    *dst = 0x00;
}

int ConvertStrToEnum(int *dst, char *src, unsigned int len, const cvt_enum_tbl* cvt_tbl)
{
    int i = 0;
    
    while(1)
    {
        if(strncmp("END", cvt_tbl[i].str, 3) == 0)
        {
            ConvertStrToDec((unsigned int*)dst, src, len);
            break;
        }
        if(strncmp(src, cvt_tbl[i].str, len) == 0)
        {
            if(strlen(cvt_tbl[i].str) == len)
            {
                *dst = cvt_tbl[i].e_num;
                break;
            }
        }
        i++;
    }
    return 0;
}

void ConvertStrToArrayAddr(unsigned int *dst, char *src, unsigned int len) 
{
    unsigned int i,j;
    
    i = 0;
    j = 0;
    *dst = (unsigned int)&BIG_BUFFER[0][0];
    while (len) {
        if (*src == 0x00) break;
        if (*src == '[')  break;
        len--;
        src++;
    }
    if (!len) return;
    len--;
    src++;
    
    while (len) {
        if (*src == 0x00) break;
        if (*src == ']')  break;
        i *= 10;
        i += (unsigned int)ConvertCharToDec(*src);
        len--;
        src++;
    }
    if (!len) return;
    len--;
    src++;
    
    while (len) {
        if (*src == 0x00) break;
        if (*src == '[')  break;
        len--;
        src++;
    }
    if (!len) return;
    len--;
    src++;
    
    while (len) {
        if (*src == 0x00) break;
        if (*src == ']')  break;
        j *= 10;
        j += (unsigned int)ConvertCharToDec(*src);
        len--;
        src++;
    }

    *dst = (unsigned int)&BIG_BUFFER[i][j];
}

unsigned int ConvertStrToValue(unsigned int *dst, char *src, unsigned int len)
{
    int i;
    
    if(strncmp(src, "0x", 2) == 0)
    {
        src+=2;
        len-=2;
    }
    for (*dst = 0, i = 0; i < (len/2); i++) { 
        *dst <<= 8;
        *dst |= ConvertStrToHexSub(src); 
        src+=2;
    }
    return 0;
}

unsigned int ConvertStrToValue64(unsigned long long *dst, char *src, unsigned int len)
{
    int i;
    
    if(strncmp(src, "0x", 2) == 0)
    {
        src+=2;
        len-=2;
    }
    for (*dst = 0, i = 0; i < (len/2); i++) { 
        *dst <<= 8;
        *dst |= ConvertStrToHexSub(src); 
        src+=2;
    }
    return 0;
}

int ConvertStrToInteger(int *dst, char *src, unsigned int len)
{
    if(strncmp(src, "0x", 2) == 0){
        ConvertStrToValue((unsigned int*)dst, src, len);
    }
    else
    {
        ConvertStrToDec((unsigned int*)dst, src, len);
    }
    return 0;
}

int ConvertStrToInteger64(unsigned long long *dst, char *src, unsigned int len)
{
    if(strncmp(src, "0x", 2) == 0){
        ConvertStrToValue64(dst, src, len);
    }
    else
    {
        ConvertStrToDec64(dst, src, len);
    }
    return 0;
}
