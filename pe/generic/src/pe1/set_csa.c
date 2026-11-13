/*******************************************************************************
* Warranty Disclaimer :
*
* Because the Product(s) is licensed free of charge, there is no warranty of
* any kind whatsoever and expressly disclaimed and excluded by Renesas, either
* expressed or implied, including but not limited to those for non-infringement
* of intellectual property, merchantability and/or fitness for the particular
* purpose. Renesas shall not have any obligation to maintain, service or provide
* bug fixes for the supplied Product(s) and/or the Application.
*
* Each User is solely responsible for determining the appropriateness of using
* the Product(s) and assumes all risks associated with its exercise of rights
* under this Agreement, including, but not limited to the risks and costs of
* program errors, compliance with applicable laws, damage to or loss of data,
* programs or equipment, and unavailability or interruption of operations.
*
* Limitation of Liability :
*
* In no event shall Renesas be liable to the User for any incidental,
* consequential, indirect, or punitive damage (including but not limited to
* lost profits) regardless of whether such liability is based on breach of
* contract, tort, strict liability, breach of warranties, failure of essential
* purpose or otherwise and even if advised of the possibility of such damages.
* Renesas shall not be liable for any services or products provided by third
* party vendors, developers or consultants identified or referred to the User
* by Renesas in connection with the Product(s) and/or the Application.
*
* Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* File Name   : set_csa.c
* Version     : 0.01
* Device(s)   : RH850/U2Cx
* Description : This is the CSA (Configuration Setting Area) code for U2C
*             : Each section must be programmed to the following address.
*             :
*             : Section Name          Address     
*             : --------------------------------------------------------
*             : CSA_OFFSET_0x200      0xFF32_1200 (<CSAb_base> + 0x0200)
*             : CSA_OFFSET_0x300      0xFF32_1300 (<CSAb_base> + 0x0300)
*             :
*******************************************************************************
* History     : DD.MM.YYYY Version Description
*             : 10.02.2024 0.01    First Relase
******************************************************************************/

#define PE1_DCLS_DIS 0 // 0: DCLS Enabled, 1: DCLS Disabled 
#define PE0_DCLS_DIS 0 // 0: DCLS Enabled, 1: DCLS Disabled

#define PE1_DISABLE  0 // 0: Enable, 1: Disable

#define OPBT3_VAL  (0xFDFF7EFC | (PE1_DISABLE << 25))
#define OPBT9_VAL  (0xFFFCFFFF | (PE1_DCLS_DIS << 17) | (PE0_DCLS_DIS << 16))
#define OPBT24_VAL (0xFFFFFFFC | (PE1_DCLS_DIS << 1) | (PE0_DCLS_DIS))
#define OPBT36_VAL (0xFDFFFFFF | (PE1_DISABLE << 25))

//#define _DISABLE_ICUM_


#if __GHS__
#pragma ghs section rodata="CSA_OFFSET_0x300"
#else
#pragma section const "CSACON_SECTION"
#endif
const unsigned long CSA_OFFSET_0x380_to_0x47F[] =
{
//  Setting Value,     Value at the shipping,     Address,               Name,                                       Comment
    0x00000000,      //0x00000000,                <CSAb_base> +0x0380,   Reset Vector (PE0)
    0x00000000,      //0x00000000,                <CSAb_base> +0x0384,   Reset Vector (PE1)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0388,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x038C,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0390,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0394,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0398,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x039C,   Reserved (need to be programmed)
    0x3FE30010,      //0x3FE30010,                <CSAb_base> +0x03A0,   Option byte 0 (OPBT0)
    0xFC7B0000,      //0xFC7B0000,                <CSAb_base> +0x03A4,   Option byte 1 (OPBT1)
    0x7FFFFFFF,      //0x7FFFFFFF,                <CSAb_base> +0x03A8,   Option byte 2 (OPBT2)
    OPBT3_VAL,       //0xFDFF7EFE,                <CSAb_base> +0x03AC,   Option byte 3 (OPBT3),                      [Modified] (STMSEL1=0, STMSEL0=0)
    0x0C0C0C0B,      //0x0C0C0C0B,                <CSAb_base> +0x03B0,   Option byte 4 (OPBT4)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x03B4,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x03B8,   Option byte 6 (OPBT6)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x03BC,   Option byte 7 (OPBT7)
    0xF97FFFFF,      //0xF97FFFFF,                <CSAb_base> +0x03C0,   Option byte 8 (OPBT8)
    OPBT9_VAL ,      //0xFFFCFFFF,                <CSAb_base> +0x03C4,   Option byte 9 (OPBT9)
    0xFAFD288A,      //0xFAFD288A,                <CSAb_base> +0x03C8,   Option byte 10 (OPBT10)
    0xEFFFFFFF,      //0xEFFFFFFF,                <CSAb_base> +0x03CC,   Option byte 11 (OPBT11)
    0xFFFFFFFD,      //0xFFFFFFFD,                <CSAb_base> +0x03D0,   Option byte 12 (OPBT12)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x03D4,   Option byte 13 (OPBT13)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x03D8,   Reserved (need to be programmed)
    0xFFFF8E0C,      //0xFFFF8E0C,                <CSAb_base> +0x03DC,   Option byte 15 (OPBT15)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x03E0,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x03E4,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x03E8,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x03EC,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x03F0,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x03F4,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x03F8,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x03FC,   Reserved (need to be programmed)
    OPBT24_VAL,      //0xFFFFFFFC,                <CSAb_base> +0x0400,   Option byte 24 (OPBT24)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0404,   Reserved (need to be programmed)
    0xFFFF0010,      //0xFFFF0010,                <CSAb_base> +0x0408,   Option byte 26 (OPBT26)
    0xFFFF0000,      //0xFFFF0000,                <CSAb_base> +0x040C,   Option byte 27 (OPBT27)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0410,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0414,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0418,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x041C,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0420,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0424,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0428,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x042C,   Reserved (need to be programmed)
    OPBT36_VAL,      //0xFDFFFFFF,                <CSAb_base> +0x0430,   Option byte 36 (OPBT36)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0434,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0438,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x043C,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0440,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0444,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0448,   Reserved (need to be programmed)
    0x2B01B824,      //0x2B01B824,                <CSAb_base> +0x044C,   Option byte 43 (OPBT43)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0450,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0454,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0458,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x045C,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0460,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0464,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0468,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x046C,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0470,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0474,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x0478,   Reserved (need to be programmed)
    0xFFFFFFFF,      //0xFFFFFFFF,                <CSAb_base> +0x047C,   Reserved (need to be programmed)
};

#if __GHS__
#pragma ghs section rodata=default
#else
#pragma section default
#endif

#ifdef _DISABLE_ICUM_
#if __GHS__
#pragma ghs section rodata="SSACON_SECTION"
#else
#pragma section const "SSACON_SECTION"
#endif
const unsigned long SSA_OFFSET_0x0_to_0x1F[] =
{
  	0x0FFFFFFF,			// ICUM_OPBT0				  	Shipping : 0xFFFFFFFF
  	0xFFFFFFFF,			// ICUM_OPBT1         			Shipping : 0xFFFFFFFF
  	0xFFFFFFFF,			// ICUM_OPBT2        			Shipping : 0xFFFFFFFF
  	0xFFFFFFFF,			// ICUM_OPBT3         			Shipping : 0xFFFFFFFF
  	0xFFFF7FEF,			// ICUM_OPBT4         			Shipping : 0xFFFFFFFF
  	0xFFFFFFFF,			// ICUM_OPBT5         			Shipping : 0xFFFFFFFF
  	0xFFFFFFFF,			// ICUM_OPBT6         			Shipping : 0xFFFFFFFF
  	0xFFFFFFFF,			// ICUM_OPBT7         			Shipping : 0xFFFFFFFF
};

#if __GHS__
#pragma ghs section rodata=default
#else
#pragma section default
#endif
#endif
