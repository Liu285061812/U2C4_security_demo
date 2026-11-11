/******************************************************************************/
/* Component Name   Toolbox                                                   */
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
#include "icum_l_my_toolbox.h"

#define ALIGN4    (0x00000003u)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0050]:[ICUMFW_UD_L_CRY_BignumMath_0050]
* Function name: l_CMN_MemSet
* Description  : Set the value for the specified number of bytes in the memory.
*                substitute to memset()
* Arguments    : *dest        : Memory of destination
*                mychar       : Set this data to dest
*                               Range: 0x0 - 0xFF
*                size         : Size of data to be set
*                               Range: 0x0 - 0x7FFF
* Return Value : void
***************************************************************************************/
void l_CMN_MemSet(uint8_t *dest, uint8_t mychar, uint32_t size)
{
    uint32_t temp;
    uint32_t align;

    /* copy 4bytes mychar to temp */
    temp  = mychar | ((uint32_t)mychar << 8u);
    temp |= (temp << 16u);

    /* check 4bytes align */
    align = (uint32_t)dest & ALIGN4;

    if(align != 0u)
    {
        while((align < 4u) && (size > 0u))
        {
            /* when dest address is not 4bytes align, set 1byte at the time */
            *dest = mychar;
            dest++;
            size--;
            align++;
        }
    }
    else
    {
        /* Do nothing */
    }

    while(size >= 4u)
    {
        /* when size is over 4bytes, set 4bytes at the time */
        *(uint32_t *)dest = temp;
        dest += 4u;
        size -= 4u;
    }
    while(size > 0u)
    {
        /* when size is less than 4bytes, set 1byte at the time */
        *dest = mychar;
        dest++;
        size--;
    }
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0051]:[ICUMFW_UD_L_CRY_BignumMath_0051]
* Function name: l_CMN_MemCpy
* Description  : Copy the specified number of bytes of memory.
*                substitute to memcpy()
*                optimized memcpy() for a 32-bit CPU
* Arguments    : *p_dest      : Memory of destination
*                *p_source    : Copy this data to dest
*                size         : Size of data to be copied
*                               Range: 0x0 - 0x7FFF
* Return Value : void
***************************************************************************************/
void l_CMN_MemCpy(uint8_t *p_dest, const uint8_t *p_source, uint32_t size)
{
    uint32_t  align1;
    uint32_t  align2;

    /* check 4bytes align */
    align1 = (uint32_t)p_dest & ALIGN4;
    align2 = (uint32_t)p_source & ALIGN4;

    if((align1 == 0u) && (align2 == 0u))
    {
        while(size >= 4u)
        {
            /* when size is over 4bytes, copy 4bytes at the time */
            *(uint32_t *)p_dest = *(const uint32_t *)p_source;
            p_dest += 4u;
            p_source += 4u;
            size -= 4u;
        }
    }
    else if(align1 == align2)
    {
        while((align1 < 4u) && (size > 0u))
        {
            /* when dest address is not 4bytes align, copy 1byte at the time */
            *p_dest = *p_source;
            p_dest++;
            p_source++;
            size--;
            align1++;
        }
        while(size >= 4u)
        {
            /* when size is over 4bytes, copy 4bytes at the time */
            *(uint32_t *)p_dest = *(const uint32_t *)p_source;
            p_dest += 4u;
            p_source += 4u;
            size -= 4u;
        }
    }
    else
    {
        /* nothing */
    }

    while(size > 0u)
    {
        /* copy 1byte at the time */
        *p_dest = *p_source;
        p_dest++;
        p_source++;
        size--;
    }
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0052]:[ICUMFW_UD_L_CRY_BignumMath_0052]
* Function name: l_CMN_MemCmp
* Description  : Compare the two input memory blocks.
*                substitute to memcmp()
* Arguments    : *p_data1     : Data1 to be compared
*                *p_data2     : Data2 to be compared
*                size         : Size of data to be compared
*                               Range: 0x0 - 0x7FFF
* Return Value : 0  Data1 and Data2 is same
*                1  Data1 and Data2 is different
***************************************************************************************/
uint8_t l_CMN_MemCmp(const uint8_t *p_data1, const uint8_t *p_data2, uint32_t size)
{
    uint8_t compare = 0u;
    uint8_t  align1;
    uint8_t  align2;

    align1 = (uint8_t)((uint32_t)p_data1 & ALIGN4);
    align2 = (uint8_t)((uint32_t)p_data2 & ALIGN4);

    if(((uint32_t)align1 == 0u) && ((uint32_t)align2 == 0u))
    {
        while((compare == 0u) && (size >= 4u))
        {
            if(*(const uint32_t *)p_data1 != *(const uint32_t *)p_data2)
            {
                compare = 1u;
            }
            else
            {
                p_data1 += 4u;
                p_data2 += 4u;
                size -= 4u;
            }
        }
        while(((uint32_t)compare == 0u) && (size > 0u))
        {
            if(*p_data1 != *p_data2)
            {
                compare = 1;
            }
            else
            {
                p_data1++;
                p_data2++;
                size--;
            }
        }
    }
    else if((uint32_t)align1 == (uint32_t)align2)
    {
        while(((uint32_t)compare == 0u) && ((uint32_t)align1 < 4u) && (size > 0u))
        {
            if(*p_data1 != *p_data2)
            {
                compare = 1u;
            }
            else
            {
                p_data1++;
                p_data2++;
                size--;
                align1++;
            }
        }
        while(((uint32_t)compare == 0u) && (size >= 4u))
        {
            if(*(const uint32_t *)p_data1 != *(const uint32_t *)p_data2)
            {
                compare = 1u;
            }
            else
            {
                p_data1 += 4u;
                p_data2 += 4u;
                size -= 4u;
            }
        }
        while(((uint32_t)compare == 0u) && (size > 0u))
        {
            if(*p_data1 != *p_data2)
            {
                compare = 1u;
            }
            else
            {
                p_data1++;
                p_data2++;
                size--;
            }
        }
    }
    else
    {
        while(((uint32_t)compare == 0u) && (size > 0u))
        {
            if(*p_data1 != *p_data2)
            {
                compare = 1u;
            }
            else
            {
                p_data1++;
                p_data2++;
                size--;
            }
        }
    }
    return(compare);
}
/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0053]:[ICUMFW_UD_L_CRY_BignumMath_0053]
* Function name: l_CMN_MemXor
* Description  : performs a XOR between source and dest, store the result in dest.
* Arguments    : *p_dest     : destination
*                *p_source   : source
*                size        : size indicates the number of bytes to process
*                               Range: 0x0 - 0x7FFF
* Return Value : void
***************************************************************************************/
void l_CMN_MemXor(uint8_t *p_dest, const uint8_t *p_source, uint16_t size)
{
    uint32_t    align1;
    uint32_t    align2;

    align1 = (uint32_t)p_dest & ALIGN4;
    align2 = (uint32_t)p_source & ALIGN4;

    if((align1 == 0u) && (align2 == 0u))
    {
        while((uint32_t)size >= 4u)
        {
            *(uint32_t *)p_dest = (*(const uint32_t *)p_source) ^ (*(uint32_t *)p_dest);
            p_dest += 4u;
            p_source += 4u;
            size -= 4u;
        }
        while((uint32_t)size > 0u)
        {
            *p_dest = (*p_source) ^ (*p_dest);
            p_dest++;
            p_source++;
            size--;
        }
    }
    else if(align1 == align2)
    {
        while((align1 < 4u) && ((uint32_t)size > 0u))
        {
            *p_dest = (*p_source) ^ (*p_dest);
            p_dest++;
            p_source++;
            size--;
            align1++;
        }
        while((uint32_t)size >= 4u)
        {
            *(uint32_t *)p_dest = (*(const uint32_t *)p_source) ^ (*(uint32_t *)p_dest);
            p_dest += 4u;
            p_source += 4u;
            size -= 4u;
        }
        while((uint32_t)size > 0u)
        {
            *p_dest = (*p_source) ^ (*p_dest);
            p_dest++;
            p_source++;
            size--;
        }
    }
    else
    {
        while((uint32_t)size > 0u)
        {
            *p_dest = (*p_source) ^ (*p_dest);
            p_dest++;
            p_source++;
            size--;
        }
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0054]:[ICUMFW_UD_L_CRY_BignumMath_0054]
* Function name: l_CMN_StrLen
* Description  : Return the length of the input character string.
*                Same as C Standard API strlen()
* Arguments    : *p_s     : Data to be measured length
* Return Value : length of s
***************************************************************************************/
uint32_t l_CMN_StrLen(const char *p_s)
{
    uint32_t  len = 0u;

    /* loop while p_s is NULL */
    while(*p_s != (char)0)
    {
        len++;
        p_s++;
    }
    return(len);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_L_CRY_BignumMath_0055]:[ICUMFW_UD_L_CRY_BignumMath_0055]
* Function name: l_CMN_ToUpper
* Description  : Converts the entered lowercase alphabet to capital letters.
*                If the input character is not lowercase alphabet, return the 
*                input value as it is.
* Arguments    : c       : Character to convert
*                          Range: 'a' - 'z'
* Return Value : 'A' - 'Z'
***************************************************************************************/
char l_CMN_ToUpper(char c)
{
    if ('a' <= c && c <= 'z')
    {
        c -= (char) 0x20;
    }
    else
    {
        /* Do nothing */
    }
    return (c);
}

