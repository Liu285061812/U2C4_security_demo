/******************************************************************************/
/* Component Name   Library Toolbox public source file                        */
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

/**********************************************************************************
* Headers
***********************************************************************************/
#include "icum_l_toolbox_pub.h"
#include MM_FLASH_PUBLIC_H
#include "icum_d_misc_pub.h"

#define ALIGN4    (0x00000003uL)
#define MAX_ADDR  (0xFFFFFFFFuL)

CHANGE_SEC(text, ".icum_library_ram")
CHANGE_SEC(rodata, ".icum_library_const_ram")

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_ToolBox_0001]:[ICUMFW_UD_L_ToolBox_0001]
* Function name: L_Toolbox_ByteSwap()
* Description  : swaps the byte order of a word (0xAABBCCDD -> 0xDDCCBBAA)
* Arguments    : myword : Target data for swap
* Return Value : swapped myword
***************************************************************************************/
uint32_t L_Toolbox_ByteSwap(uint32_t myword)
{
    uint32_t swapped;
    swapped = BSW(myword);
    return(swapped);
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_ToolBox_0008]:[ICUMFW_UD_L_ToolBox_0008]
* Function name: L_Toolbox_MemClr64()
* Description  : Clears data in a block of memory by 8 byte at a time.
* Arguments    : dest64 : Block of memory to be cleared
*                size64 : Size of data to be cleared
* Return Value : void
***************************************************************************************/
void L_Toolbox_MemClr64(uint64_t *dest64, int32_t size64)
{
    uint64_t *dest = dest64;
    int32_t size;

    for(size = size64; size >= 8L; size -= 8L)
    {
        *dest = 0uLL;
        dest++;
    }
}

CHANGE_SEC(text, ".icum_library_ram")
CHANGE_SEC(rodata, ".icum_library_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_ToolBox_0002]:[ICUMFW_UD_L_ToolBox_0002]
* Function name: L_Toolbox_MemSet()
* Description  : Same as C Standard API memset()
* Arguments    : dest   : Memory of destination
*                mychar : Set this data to dest
*                size   : Size of data to be set
* Return Value : void
***************************************************************************************/
void L_Toolbox_MemSet(uint8_t *dest, uint8_t mychar, int16_t size)
{
    uint32_t  temp;
    uint32_t  align;

    /* copy 4bytes mychar to temp */
    temp  = (uint32_t)mychar | ((uint32_t)mychar << 8uL);
    temp |= (temp << 16uL);

    /* check 4bytes align */
    align = (uint32_t)dest & ALIGN4;

    if(align != 0uL)
    {
        while((align < 4uL) && ((int32_t)size > 0L))
        {
            /* when dest address is not 4bytes align, set 1byte at the time */
            *dest = mychar;
            dest++;
            size = (int16_t)((int32_t)size - 1L);
            align++;
        }
    }
    else
    {
        /* Do nothing */
    }

    while((int32_t)size >= 4L)
    {
        /* when size is over 4bytes, set 4bytes at the time */
        *(uint32_t *)dest = temp;
        dest += 4uL;
        size = (int16_t)((int32_t)size - 4L);
    }
    while((int32_t)size > 0L)
    {
        /* when size is less than 4bytes, set 1byte at the time */
        *dest = mychar;
        dest++;
        size = (int16_t)((int32_t)size - 1L);
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_ToolBox_0003]:[ICUMFW_UD_L_ToolBox_0003]
* Function name: L_Toolbox_MemCpy()
* Description  : Same as C Standard API memcpy()
* Arguments    : dest   : Memory of destination
*                source : Copy this data to dest
*                size   : Size of data to be copied
* Return Value : void
***************************************************************************************/
void L_Toolbox_MemCpy(uint8_t *dest, const uint8_t *source, int16_t size)
{
    uint32_t  align1;
    uint32_t  align2;

    /* check 4bytes align */
    align1 = (uint32_t)dest & ALIGN4;
    align2 = (uint32_t)source & ALIGN4;

    if((align1 == 0uL) && (align2 == 0uL))
    {
        while((int32_t)size >= 4L)
        {
            /* when size is over 4bytes, copy 4bytes at the time */
            *(uint32_t *)dest = *(const uint32_t *)source;
            dest += 4uL;
            source += 4uL;
            size = (int16_t)((int32_t)size - 4L);
        }
    }
    else if(align1 == align2)
    {
        while((align1 < 4uL) && ((int32_t)size > 0L))
        {
            /* when dest address is not 4bytes align, copy 1byte at the time */
            *dest = *source;
            dest++;
            source++;
            size = (int16_t)((int32_t)size - 1L);
            align1++;
        }
        while((int32_t)size >= 4L)
        {
            /* when size is over 4bytes, copy 4bytes at the time */
            *(uint32_t *)dest = *(const uint32_t *)source;
            dest += 4uL;
            source += 4uL;
            size = (int16_t)((int32_t)size - 4L);
        }
    }
    else
    {
        /* nothing */
    }

    while((int32_t)size > 0L)
    {
        /* copy 1byte at the time */
        *dest = *source;
        dest++;
        source++;
        size = (int16_t)((int32_t)size - 1L);
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_ToolBox_0014]:[ICUMFW_UD_L_ToolBox_0014]
* Function name: L_Toolbox_MemCpy_Sec()
* Description  : Same as C Standard API memcpy() with the check of destination area
* Arguments    : dest   : Memory of destination
*                source : Copy this data to dest
*                size   : Size of data to be copied
* Return Value : ER_OK
*                ER_L_TOOLBOX_SYS_ERROR
***************************************************************************************/
uint32_t L_Toolbox_MemCpy_Sec(uint8_t *dest, const uint8_t *source, int16_t size)
{
    uint32_t err;
    uint32_t dest_max_size;
    uint32_t dest_end;
    uint32_t slram_start;
    uint32_t slram_end;

    err = ER_L_TOOLBOX_SYS_ERROR;
    dest_max_size = MAX_ADDR - (uint32_t)dest;

    /* overflow check */
    if (size <= dest_max_size)
    {
        /* get the end address */
        dest_end = (uint32_t)dest + size;
        if (0uL != dest_end)
        {
            dest_end--;
        }
        else
        {
            /* Do nothing */
        }

        /* get the address of the check area */
        (void)MM_FLS_GetMemBoundary(MM_FLS_LOWER_LIMIT_SLRAM, &slram_start);
        (void)MM_FLS_GetMemBoundary(MM_FLS_UPPER_LIMIT_SLRAM, &slram_end);

        /* check the destination area */
        if ((slram_start <= (uint32_t)dest) && (slram_end >= dest_end))
        {
            L_Toolbox_MemCpy(dest, source, size);
            err = ER_OK;
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

    return err;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_ToolBox_0004]:[ICUMFW_UD_L_ToolBox_0004]
* Function name: L_Toolbox_MemCmp()
* Description  : Same as C Standard API memcmp()
* Arguments    : data1 : Data1 to be compared
*                data2 : Data2 to be compared
*                size  : Size of data to be compared
* Return Value : 0 : Data1 and Data2 is same
*                1 : Data1 and Data2 is different
***************************************************************************************/
uint8_t L_Toolbox_MemCmp(const uint8_t *data1, const uint8_t *data2, int16_t size)
{
    uint8_t   compare = 0u;
    uint32_t  align1;
    uint32_t  align2;

    /* check 4bytes align */
    align1 = (uint32_t)data1 & ALIGN4;
    align2 = (uint32_t)data2 & ALIGN4;

    if((align1 == 0uL) && (align2 == 0uL))
    {
        while(((uint32_t)compare == 0uL) && ((int32_t)size >= 4L))
        {
            /* when size is over 4bytes, compare 4bytes at the time */
            if(*(const uint32_t *)data1 != *(const uint32_t *)data2)
            {
                compare = 1u;
            }
            else
            {
                data1 += 4uL;
                data2 += 4uL;
                size = (int16_t)((int32_t)size - 4L);
            }
        }
    }
    else if(align1 == align2)
    {
        while(((uint32_t)compare == 0uL) && (align1 < 4uL) && ((int32_t)size > 0L))
        {
            /* when dest address is not 4bytes align, conpare 1byte at the time */
            if((uint32_t)*data1 != (uint32_t)*data2)
            {
                compare = 1u;
            }
            else
            {
                data1++;
                data2++;
                size = (int16_t)((int32_t)size - 1L);
                align1++;
            }
        }
        while(((uint32_t)compare == 0uL) && ((int32_t)size >= 4L))
        {
            /* when size is over 4bytes, compare 4bytes at the time */
            if(*(const uint32_t *)data1 != *(const uint32_t *)data2)
            {
                compare = 1u;
            }
            else
            {
                data1 += 4uL;
                data2 += 4uL;
                size = (int16_t)((int32_t)size - 4L);
            }
        }
    }
    else
    {
        /* nothing */
    }

    while(((uint32_t)compare == 0uL) && ((int32_t)size > 0L))
    {
        /* compare 1byte at the time */
        if((uint32_t)*data1 != (uint32_t)*data2)
        {
            compare = 1u;
        }
        else
        {
            data1++;
            data2++;
            size = (int16_t)((int32_t)size - 1L);
        }
    }
    return(compare);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_ToolBox_0005]:[ICUMFW_UD_L_ToolBox_0005]
* Function name: L_Toolbox_StrLen()
* Description  : Same as C Standard API strlen()
* Arguments    : s : Data to be measured length
* Return Value : length of s
***************************************************************************************/
uint32_t L_Toolbox_StrLen(const char *s)
{
    uint32_t  len = 0uL;

    /* loop while s is NULL */
    while((int32_t)*s != 0L)
    {
        len++;
        s++;
    }
    return(len);
}

/***************************************************************************************
* Function ID  : (not supported)
* Function name: L_Toolbox_Strcpy()
* Description  : Same as C Standard API strcpy()
* Arguments    : s1 : Destination data
*              : s2 : Source data
* Return Value : void
***************************************************************************************/
void L_Toolbox_Strcpy(char *s1, const char *s2)
{
    /* loop while s2 is NULL */
    while((int32_t)*s2 != 0L)
    {
        *s1 = *s2;
        s1++;
        s2++;
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_ToolBox_0007]:[ICUMFW_UD_L_ToolBox_0007]
* Function name: L_Toolbox_Strcmp()
* Description  : Same as C Standard API strcmp()
* Arguments    : s1 : Data1 to be compared
*              : s2 : Data2 to be compared
* Return Value : 0  : Data1 and Data2 is same
*                -1 : Data1 and Data2 is different(Data1 < Data2)
*                1  : Data1 and Data2 is different(Data1 > Data2)
***************************************************************************************/
int32_t L_Toolbox_Strcmp(const char *s1, const char *s2)
{
    int32_t  cmp;

    /* loop to s1 and s2 are not equal, or s1 is NULL */
    while(((int32_t)*s1 == (int32_t)*s2) && ((int32_t)*s1 != 0L))
    {
        s1++;
        s2++;
    }

    if((int32_t)*s1 < (int32_t)*s2)
    {
         /* s1 is less than s2 */
         cmp = -1L;
    }
    else if((int32_t)*s1 > (int32_t)*s2)
    {
         /* s1 is bigger than s2 */
         cmp = 1L;
    }
    else
    {
        /* s1 and s2 are equal */
         cmp = 0L;
    }
    return(cmp);
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_ToolBox_0011]:[ICUMFW_UD_L_ToolBox_0011]
* Function name: R_RH850_MemSet()
* Description  : L_Toolbox_MemSet() wrapper API
* Arguments    : dest   : Memory of destination
*                mychar : Set this data to dest
*                size   : Size of data to be set
* Return Value : void
***************************************************************************************/
#pragma ghs reference R_RH850_MemSet
void R_RH850_MemSet(uint8_t *dest, uint8_t mychar, int16_t size)
{
    L_Toolbox_MemSet(dest, mychar, size);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_ToolBox_0012]:[ICUMFW_UD_L_ToolBox_0012]
* Function name: R_RH850_MemCpy()
* Description  : L_Toolbox_MemCpy() wrapper API
* Arguments    : dest   : Memory of destination
*                source : Copy this data to dest
*                size   : Size of data to be copied
* Return Value : void
***************************************************************************************/
#pragma ghs reference R_RH850_MemCpy
void R_RH850_MemCpy(uint8_t *dest, const uint8_t *source, int16_t size)
{
    L_Toolbox_MemCpy(dest, source, size);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_ToolBox_0013]:[ICUMFW_UD_L_ToolBox_0013]
* Function name: R_RH850_MemCmp()
* Description  : L_Toolbox_MemCmp() wrapper API
* Arguments    : data1 : Data1 to be compared
*                data2 : Data2 to be compared
*                size  : Size of data to be compared
* Return Value : 0 : Data1 and Data2 is same
*                1 : Data1 and Data2 is different
***************************************************************************************/
#pragma ghs reference R_RH850_MemCmp
uint8_t R_RH850_MemCmp(const uint8_t *data1, const uint8_t *data2, int16_t size)
{
    uint8_t   compare;
    compare = L_Toolbox_MemCmp(data1, data2, size);
    return(compare);
}

