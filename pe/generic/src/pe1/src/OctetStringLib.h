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
/* Data handled by OCTET_STRING_xxx () is a byte array of BIG_ENDIAN */
void OCTET_STRING_setbit(unsigned char *data, int bitPosition, int datalen);
void OCTET_STRING_sub(unsigned char *data, unsigned char *sub, int datalen);
void OCTET_STRING_inc(unsigned char *data, int datalen);
int OCTET_STRING_modByDigit(unsigned char *dividend, int divisor, int datalen);
void OCTET_STRING_divByDigit(unsigned char *dividend, int divisor, int datalen);
void OCTET_STRING_L_shift(unsigned char *data, int shiftbits, int datalen);
void OCTET_STRING_R_shift(unsigned char *data, int shiftbits, int datalen);
