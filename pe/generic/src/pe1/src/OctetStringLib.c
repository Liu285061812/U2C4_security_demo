/******************************************************************************/
/* Component Name   OctetStringLib                                            */
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

#include "OctetStringLib.h"

/* Data handled by OCTET_STRING_xxx () is a byte array of BIG_ENDIAN */
void OCTET_STRING_setbit(unsigned char *data, int bitPosition, int datalen)
{
    int bytes = bitPosition / 8;
    int bits = bitPosition % 8;

    data[datalen - bytes - 1] |= (1 << bits);
}
void OCTET_STRING_sub(unsigned char *data, unsigned char *sub, int datalen)
{
    int i;
    int cy = 0;
    for(i=datalen-1; i>=0; i-- )
    {
        if(cy == 0){
            if(data[i] < sub[i])
            {
                cy = 1;
            }else{
                cy = 0;
            }
            data[i] -= sub[i];
        }else{
            switch(data[i]){
            case 0:
                /* cy = 1; */      /* not change */
                break;

            default:
                if((data[i] -1) < sub[i])
                {
                    cy = 1;
                }else{
                    cy = 0;
                }
                break;
            }
            data[i] -= (sub[i]+1);
        }
    }
}
void OCTET_STRING_inc(unsigned char *data, int datalen)
{
    int i;
    for(i=datalen-1; i>=0; i-- )
    {
        if( ++data[i] != 0 )
            break;
    }
}
int OCTET_STRING_modByDigit(unsigned char *dividend, int divisor, int datalen)
{
    int i;
    unsigned short c = 0;
    for(i=0; i<datalen; i++ )
    {
        c <<= 8;
        c += dividend[i];
        c %= divisor;
    }
    return(c);
}
void OCTET_STRING_divByDigit(unsigned char *dividend, int divisor, int datalen)
{
    int i;
    unsigned short c = 0;
    for(i=0; i<datalen; i++ )
    {
        c <<= 8;
        c += dividend[i];
        dividend[i] = (c / divisor);
        c %= divisor;
    }
}

void OCTET_STRING_L_shift(unsigned char *data, int shiftbits, int datalen)
{
    int i;
    int bytes = shiftbits / 8;
    int bits = shiftbits % 8;
    unsigned short int c = 0;
    
    if(bytes != 0)
    {
        for(i=0; i<(datalen - bytes); i++ )
        {
            data[i] = data[i + bytes];
        }
        for(; i<datalen; i++ )
        {
            data[i] = 0;
        }
    }
    if(bits != 0)
    {
        c = data[0];
        for(i=1; i<datalen; i++ )
        {
            c <<= 8;
            c |= data[i];
            data[i-1] = (c >> (8-bits));
        }
        data[i] = (c << bits);
    }
}

void OCTET_STRING_R_shift(unsigned char *data, int shiftbits, int datalen)
{
    int i;
    int bytes = shiftbits / 8;
    int bits = shiftbits % 8;

    if(bytes != 0)
    {
        for(i=datalen-1; i>=bytes; i-- )
        {
            data[i] = data[i - bytes];
        }
        for( ; i>=0; i-- )
        {
            data[i] = 0;
        }
    }
    if(bits != 0)
    {
        /* T.B.D. */
    }
}
