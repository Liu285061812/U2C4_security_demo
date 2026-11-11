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
// force the integration of the copyright notice
#pragma ghs reference sys_get_package_info

#include DEVICE_FILE
#include "v850.toolbox.h"

#include "sys.config.h"

#if (SYS_QUARTZ_FREQ != 20000000)
    #error INCORRECT FREQUENCY
#endif

#undef PASTER
#undef EVALUATOR
/*
 [Overview]\n
 Connect a symbol

 [Description]\n
 Concatenate the two symbols specified by the arguments x and y to generate 
 one symbol.

 [Parameters(in)]\n
 x      - Symbols to connet (1st word)\n
 y      - Symbols to connet (2nd word)\n
 z      - Symbols to connet (last word)

 [Return value]\n
 Connected symbol
*/
#define PASTER(x, y, z)        PORT ## x ## y ## z
/** PASTER's alias */
#define EVALUATOR(x, y, z)     PASTER(x, y, z)

/* For port configuration, the following is set.   */
/*    the port unit (PU)  0 or 1                   */
/*    the port groups in the port unit (PG)        */
/*    the port number in the group (Pn)            */

/* Tx:P12_6 */
#define PG_VUART0_TX    17
#define Pn_VUART0_TX    2
/* Rx:P12_5 */
#define PG_VUART0_RX    17
#define Pn_VUART0_RX    3

#define INT_VUART0_RCV  EIINT180
#define INT_VUART0_SND  EIINT179
#define INT_VUART0_ERR  EIINT181


/* Note !! */
/* DEVICE_FILE definition is prohibited !!                                   */
/* Because the register address shifts due to erroneous boundary adjustment. */
/* Alternatively, address definition is performed for each register.         */

/**
     LIN/UART Interface (RLIN3)\n
     List of registers for RLIN30 port
*/
/** Base address */
#undef RLN35LWBR
#undef RLN35LBRP01
#undef RLN35LSTC
#undef RLN35LMD
#undef RLN35LBFC
#undef RLN35LSC
#undef RLN35LIE
#undef RLN35LEDE
#undef RLN35LCUC
#undef RLN35LTRC
#undef RLN35LMST
#undef RLN35LST
#undef RLN35LEST
#undef RLN35LUOER
#undef RLN35LUTDR
#undef RLN35LURDR
#define RLN35LWBR RLN31LWBR
#define RLN35LBRP01 RLN31LBRP01
#define RLN35LSTC RLN31LSTC
#define RLN35LMD RLN31LMD
#define RLN35LBFC RLN31LBFC
#define RLN35LSC RLN31LSC
#define RLN35LIE RLN31LIE
#define RLN35LEDE RLN31LEDE
#define RLN35LCUC RLN31LCUC
#define RLN35LTRC RLN31LTRC
#define RLN35LMST RLN31LMST
#define RLN35LST RLN31LST
#define RLN35LEST RLN31LEST
#define RLN35LUOER RLN31LUOER
#define RLN35LUTDR RLN31LUTDR
#define RLN35LURDR RLN31LURDR

#define INTC2_base    (0xFFF80000UL)
//#define INTC2EIC621   (*(volatile unsigned short *)(INTC2_base + 0x4DAU))
//#define INTC2EIC622   (*(volatile unsigned short *)(INTC2_base + 0x4DCU))
//#define INTC2EIC623   (*(volatile unsigned short *)(INTC2_base + 0x4DEU))

//#define INTC2EIC437   (*(volatile unsigned short *)(INTC2_base + 0x36AU))
//#define INTC2EIC438   (*(volatile unsigned short *)(INTC2_base + 0x36CU))
//#define INTC2EIC439   (*(volatile unsigned short *)(INTC2_base + 0x36EU))

#define MSRKCPROT SYSCTRLMSRKCPROT
#define MSR_RLIN3 SYSCTRLMSR_RLIN3

#define PORT_PWE PORT0PWE


// ------------------------------------------------------------
// external variables
// ------------------------------------------------------------

/** Clock frequency of VUART0 */
extern unsigned char VUART0_CLK;
/** Clock frequency of VTIMER0 */
extern unsigned char VTIMER0_CLK;
/** Clock frequency of VTIMER1 */
extern unsigned char VTIMER1_CLK;

// ************************************************************************************************************************
// VUART0 management
// ************************************************************************************************************************

/* the below global variables are declared in lib.vrun */
/** vrun_receiveflag is 1 when a byte has been received (update in irq function) */
extern volatile char vrun_receiveflag;
/* vrun_sendflag is 1 when a byte has been sent (update in irq function) */
extern volatile char vrun_sendflag;

/**
 [Overview]\n
 VUART interrupt vector upon transmission

 [Description]\n
 Interrupt occurs after correct character transmission

 [Parameters]\n
 None

 [Return value]\n
 None

 [Output variables]\n
 vrun_sendflag
*/
__interrupt void INT_VUART0_SND(void)
{
    /* indicate to the control layer that a byte has been sent */
    vrun_sendflag = 1;
}

/**
 [Overview]\n
 VUART interrupt vector upon reception

 [Description]\n
 Interrupt occurs after correct character reception

 [Parameters]\n
 None

 [Return value]\n
 None

 [Output variables]\n
 vrun_receiveflag
*/
__interrupt void INT_VUART0_RCV(void)
{
    /* indicate to the control layer that a byte has been received */
    vrun_receiveflag = 1;
}

/**
 [Overview]\n
 VUART interrupt vector upon status

 [Description]\n
 Interrupt occurs in case of error during reception

 [Parameters]\n
 None

 [Return value]\n
 None

 [Output variables]\n
 vrun_receiveflag
*/
__interrupt void INT_VUART0_ERR(void)
{
    /* indicate to the control layer that a byte has been received with error */
    vrun_receiveflag = -1;
}

/*
 [Overview]\n
 reads one byte from the VUART

 [Description]\n
 Check the UART status, and if there is received data, extract 1-byte of data
 from it and return it as a return value.\n
 If there is no received data, 0xFF is returned as a return value.

 [Parameters]\n
 None

 [Return value]\n
 0xFF           - No received data\n
 Other values   - Extracted 1-byte data

 [Used registers]\n
 SCI31SSR, SCI31RDRL
*/
unsigned char sys_vuart_getbyte(void)
{
    return (RLN35LURDR);
}

/**
[Overview]\n
 sends one byte via the VUART

 [Description]\n
 Monitor the UART status and wait for the transmittable status (transmission data
 register is empty).
 Send 1-byte data when it is ready to be sent.

 [Parameters]\n
 1-byte data to send

 [Return value]\n
 None

 [Used registers]\n
 SCI31SSR, SCI31TDRL

 [Remarks]\n
 In this implementation, the loop that monitors the transmittable status may
 become an endless loop. When using it as a customer, take measures such as timeout.
*/
void sys_vuart_sendbyte(unsigned char byte_to_send)
{
    RLN35LUTDR = byte_to_send;
}

/**
 [Overview]\n
 Configure the VUART

 [Description]\n
 Configure the logical UART(VUART) used in the VRUN module.
 In this implementation, SCI3(Serial Communication Interface 3) is used as VUART.

 [Parameters]\n
 None

 [Return value]\n
 None

 [Used registers]\n
 MSRKCPROT, MSR_SCI, PKCPROT, PWE,
 SCI31SCR, SCI31SMR, SCI31SCMR, SCI31SEMR, SCI31BRR, SCI31MDDR

 [Remarks]\n
 VUART is mainly used for console input/output. Make various settings according
 to the console tool used by the customer.
*/
void sys_vuart_config(void)
{
    /* RLN35 module Active */
    MSRKCPROT = 0xA5A5A501;
    MSR_RLIN3 |= 0x80000000;
    MSR_RLIN3 &= 0xFFFFFFDF;	// RLN35
    MSRKCPROT = 0xA5A5A500;

    /* RLN35 is configured in UART mode */
    // UART reset & disable
    RLN35LUOER  = 0x00;
    RLN35LCUC   = 0x00;
    
    while (RLN35LMST != 0 );
    
    // select UART mode
    RLN35LMD    = 0x01;

    // UART config
    // 8-bit communication
    // LSB first
    // Stop Bit 1 bit
    // Parity Disabled
    // Without RX inversion
    // Without RX inversion
    RLN35LBFC   = 0x00;

    // disable error detection
    RLN35LEDE   = 0x00;

    // UART Bps setup
#define SAMPLING    6
    /* RLN35LWB[7..4] Bit Sampling Count Select : 6 sampling */
    /* RLN35LWB[3..1] Prescaler Clock Select    : 1/1        */ 
    RLN35LWBR   = (SAMPLING - 1) << 4;

    /* RLN35LBRP01 = (PCLK /  (SYS_COM_SPEED * SAMPLING)) - 1 */
    RLN35LBRP01 = ((VUART0_CLK * 1000 * 1000) / (SYS_COM_SPEED * SAMPLING)) - 1;

    // release UART from reset
    RLN35LCUC   = 0x01;

    while (RLN35LMST != 1 );

    // enable UART (TR + RX)
    RLN35LUOER  = 0x03;

    /* Configure Pin Function */

    /* Port Keycode Protection Disable */
    PORT0PKCPROT = 0xA5A5A501;

    /* Port Write Enable */
    PORT_PWE |= 0x00000400;

    /* I/O mode is selected by PMn.PMn_m (S/W I/O control alternative mode). */
    EVALUATOR(0 , PIPC , PG_VUART0_RX) &=  ~(1 << Pn_VUART0_RX);
    EVALUATOR(0 , PIPC , PG_VUART0_TX) &=  ~(1 << Pn_VUART0_TX);

    /* Configure VUART0 TX port */
    /* ALT_OUT5    */
    /*  PFC    = 0 */
    /*  PFCE   = 0 */
    /*  PFCAE  = 1 */
    /*  PFCEAE = 0 ( not supported by HW ) */
    /*  PMC    = 1 */
    /*  PM     = 0 */
    EVALUATOR(0 , PFC   , PG_VUART0_TX) &= ~(1 << Pn_VUART0_TX);
    EVALUATOR(0 , PFCE  , PG_VUART0_TX) |=  (1 << Pn_VUART0_TX);
    EVALUATOR(0 , PFCAE , PG_VUART0_TX) &= ~(1 << Pn_VUART0_TX);
    EVALUATOR(0 , PFCEAE , PG_VUART0_TX) &= ~(1 << Pn_VUART0_TX);
    EVALUATOR(0 , PMC   , PG_VUART0_TX) |=  (1 << Pn_VUART0_TX);
    EVALUATOR(0 , PM    , PG_VUART0_TX) &= ~(1 << Pn_VUART0_TX);

    /* Configure VUART0 RX port */
    /* ALT_IN6     */
    /*  PFC    = 1 */
    /*  PFCE   = 0 */
    /*  PFCAE  = 1 */
    /*  PFCEAE = 0 ( not supported by HW ) */
    /*  PMC    = 1 */
    /*  PM     = 1 */
    EVALUATOR(0 , PFC   , PG_VUART0_RX) &= ~(1 << Pn_VUART0_RX);
    EVALUATOR(0 , PFCE  , PG_VUART0_RX) &= ~(1 << Pn_VUART0_RX);
    EVALUATOR(0 , PFCAE , PG_VUART0_RX) |=  (1 << Pn_VUART0_RX);
    EVALUATOR(0 , PFCEAE , PG_VUART0_RX) &= ~(1 << Pn_VUART0_RX);
    EVALUATOR(0 , PIBC , PG_VUART0_RX) |= (1 << Pn_VUART0_RX);
    EVALUATOR(0 , PMC   , PG_VUART0_RX) |=  (1 << Pn_VUART0_RX);
    EVALUATOR(0 , PM    , PG_VUART0_RX) |=  (1 << Pn_VUART0_RX);

    
    /* Port Write Disable */
    PORT_PWE &= ~(0x00000400);
    
    /* Port Keycode Protection Enable */
    PORT0PKCPROT = 0xA5A5A500;
    
    // wait for UART Tx line stabilization (help ext. receivers to detect first Byte correctly)
    {int i; for (i = 0x10000; i; i--) __asm("nop");}

    // unmask UART interrupts and set priority level (0 for trasmit, 1 for receive , 1 for error)
    INTC2EIC179 = 0x40;
    INTC2EIC180 = 0x41;
    INTC2EIC181 = 0x41;
}
