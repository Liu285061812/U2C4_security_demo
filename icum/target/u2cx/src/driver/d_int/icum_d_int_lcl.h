/******************************************************************************/
/* Component Name   ICUM INT Driver                                           */
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

#if !defined(ICUM_D_INT_LCL_H)
#define ICUM_D_INT_LCL_H

/**************************************************************************************/
/* Extern references                                                                  */
/**************************************************************************************/

/* #warning "those references should be moved to the system layer (ideally)" */
#if (EINTPRIO_0_ENABLE_RAM > 0x00000000U)
extern uint8_t __ghsbegin_icum_ram_intvec[];
#endif

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
#if (EINTPRIO_0_ENABLE_RAM > INT_HNDL_NOT_REGIST)
#define p_ram_intvec        __ghsbegin_icum_ram_intvec
#endif

/* Internal status definition */
#define ICUM_INT_IDLE                  (0UL)         /* Indefinite state    */
#define ICUM_INT_ROM_MODE              (1UL)         /* Rom mode state      */
#define ICUM_INT_RAM_MODE              (2UL)         /* Ram mode state      */


/* Definition used by INT driver */
#define ICUM_INT_CHANNELMAX            (32UL)       /* Chl Number Max each INTC1 INTC2 */
#define ICUM_INT_EITBL_MAX             (64UL)       /* EI Interrupt table MAX number   */
#define ICUM_INT_FETBL_MAX             (17UL)       /* FE Interrupt table MAX number   */
#define ICUM_INT_INTERRUPTMAX          ((64U)-(1U)) /* Chl Number Max each INTC1 INTC2 */
#define ICUM_INT_RAMVCTER_MAX          ((32U)*(16U))  /* Ram Vector MAX Size           */

#define ICUM_INT_EI_INTOCC_LOW         (0x1000uL)   /* EI Interrupt cause code minimum value */
#define ICUM_INT_EI_INTOCC_HIGH        (0x11FFuL)   /* EI Interrupt cause code maximum value */

#define ICUM_INT_PRI_LVL0              (0U)         /* interrupt priority level 0    */
#define ICUM_INT_PRI_LVL1              (1U)         /* interrupt priority level 1    */
#define ICUM_INT_PRI_LVL2              (2U)         /* interrupt priority level 2    */
#define ICUM_INT_PRI_LVL3              (3U)         /* interrupt priority level 3    */
#define ICUM_INT_PRI_LVL4              (4U)         /* interrupt priority level 4    */
#define ICUM_INT_PRI_LVL5              (5U)         /* interrupt priority level 5    */
#define ICUM_INT_PRI_LVL6              (6U)         /* interrupt priority level 6    */
#define ICUM_INT_PRI_LVL7              (7U)         /* interrupt priority level 7    */
#define ICUM_INT_PRI_LVLCNT            (8U)         /* interrupt priority level CNT  */

#define ICUM_INT_V_DIRECT              (0U)         /* Direct jumping to an address  */
#define ICUM_INT_V_TABLE               (1U)         /* Referencing a table           */

#define ICUM_INT_MASK_ENA              (0U)         /* Enables interrupt processing  */
#define ICUM_INT_MASK_DIS              (1U)         /* Disables interrupt processing */

#define ICUM_INT_CTBIT_MSK             (0x8000U) /* ICUM_ICxxx register CT bit mask    */
#define ICUM_INT_RFBIT_MSK             (0x1000U) /* ICUM_ICxxx register RF bit mask    */
#define ICUM_INT_MKBIT_MSK             (0x0080U) /* ICUM_ICxxx register MK bit mask    */
#define ICUM_INT_TBBIT_MSK             (0x0040U) /* ICUM_ICxxx register TB bit mask    */
#define ICUM_INT_PRIBIT_MSK            (0x0007U) /* ICUM_ICxxx register P2-P0 bit mask */
#define ICUM_INT_RFBIT_SFT             (12U)     /* ICUM_ICxxx register RF bit mask    */
#define ICUM_INT_MKBIT_SFT             (7U)      /* ICUM_ICxxx register MK bit mask    */
#define ICUM_INT_TBBIT_SFT             (6U)      /* ICUM_ICxxx register TB bit mask    */
#define ICUM_INT_PRIBIT_SFT            (0U)      /* ICUM_ICxxx register P2-P0 bit mask */
#define ICUM_INT_TYPE_EDGE             (0U)      /* Interrupt type Edge                */

/* SYSERR Exception Notification Setting Register Interrupt UNMASK Set           */
/* BIT6:SEG_CONTRMWE(Local RAM read modify write error notification enabled)     */
/* BIT4:SEG_CONTRAME(Local RAM area error notification valid)                    */
/* BIT2:SEG_CONTEXTE(Bus access error notification enable)                       */
/* BIT1:SEG_CONTFCHE(Code flash error notification enabled)                      */
#define ICUM_INT_SYSERR_UNMUSK         (0x0056U)
#define ICUM_INT_SYSERR_MUSK           (0x0000U)

/* FEINT Factor Mask Register UNMASK Set                       */
/* BIT7:ICUM_STPRQFEIFMSK(INTSYSSTPRQ Clear interrupt mask)    */
/* BIT6:ICUM_ECCPKCCRAMFEIF(INTECCPKCCRAM interrupt mask)      */
/* BIT5:ICUM_ECCSICFEIFMSK(INTECCSIC Clear interrupt mask)     */
/* BIT4:ICUM_ECCSICABFEIFMSK(INTECCSICAB Clear interrupt mask) */
/* BIT3:ICUM_ECCCRAMFEIFMSK(INTECCCRAM Clear interrupt mask)   */
/* BIT2:ICUM_ECCLRAMFEIFMSK(INTECCLRAM Clear interrupt mask)   */
/* BIT1:INTECCDEEP0FEIFMSK(INTECCDEEP0 interrupt mask)         */
/* BIT0:ICUM_ECCAESAXIFEIFMSK( INTICUECCAESAXI interrupt mask) */
#define ICUM_INT_FEINT_UNMUSK          (0x00000302UL)
/* #define ICUM_INT_FEINT_UNMUSK          (0x00000300UL) [Use this after alpha] */
#define ICUM_INT_FEINT_MUSK            (0x000003FFUL)

/* ICUM_IMRm EI Level Interrupt Mask Register (m = 0, 1)  UNMASK Set                  */
/* BIT29:INTEXTDCU20RDRQ(ICUP/CPU(PE) Dbg Authentication response data comparison req)*/
/* BIT28:INTEXTDCU20CDRQ(ICUP Debug Authentication Challenge Data Request)            */
/* BIT26:INTECM0CFGERR(Code flash guard error)                                        */
/* BIT13:INTFACI0ENDNM(Exit the flash sequencer)                                      */
/* BIT9 :INTICUWDTA0(Watchdog timer 0 interrupt (at 75%)) <- Change Not Used          */
/* BIT8 :INTICUOSTM1(OS timer 1 overflow)                 <- Change Not Used          */
/* BIT7 :INTICUOSTM0(OS timer 0 overflow)                                             */
/* BIT6 :INTICUTRNGC0(Random number generation complete)                              */
/* BIT1 :INTPE(CPU (PE) interrupt request (selected single PE))                       */
#define ICUM_INT_IMR0_MUSK             (0xFFFFFFFFUL)
#define ICUM_INT_IMR1_MUSK             (0xFFFFFFFFUL)

#define ICUM_INT_IMR0_ICUTRNGC0        (0x00000040UL)

/* EBASE Register RINT BIT ON       */
#define ICUM_INT_EXTENDED_INTERRUPTS   (0x00000001UL)

/* ICUM_SEGFLAG - System Error Source Storing Register */
#define ICUM_INT_SEGFLAG_RMWF          (0x0040U)
#define ICUM_INT_SEGFLAG_RAMF          (0x0010U)
#define ICUM_INT_SEGFLAG_EXTF          (0x0004U)
#define ICUM_INT_SEGFLAG_FCHF          (0x0002U)

/* ICUM_FENMIICUM_FEINTF  - FEINT Factor Register       */
/* ICUM_FENMIICUM_FEINTFC - FEINT Factor Clear Register */
#define ICUM_INT_STPRQFEIFC            (0x00000080UL)
#define ICUM_INT_ECCPKCCRAMFEIF        (0x00000040UL)
#define ICUM_INT_ECCSICFEIFC           (0x00000020UL)
#define ICUM_INT_ECCSICABFEIF          (0x00000010UL)
#define ICUM_INT_ECCCRAMFEIF           (0x00000008UL)
#define ICUM_INT_ECCLRAMFEIF           (0x00000004UL)
#define ICUM_INT_INTECCDEEP0FEIF       (0x00000002UL)
#define ICUM_INT_ECCAESAXIFEIF         (0x00000001UL)

/* CUM_ICxxx register RF Bit MUSK/UNMUSK      */
#define ICUM_INT_RFBIT_SETINTRRUP      (0x1000UL)
#define ICUM_INT_RFBIT_CLRINTRRUP      (0xEFFFU)
#define ICUM_INT_RFBIT_RIGHT_SIFT12    (12U)

/* PSW - Program Status Word EBV bit                           */
/* EBV Bit : indicates either RBASE register or EBASE register */
#define ICUM_PSW_EBVBIT_U              (0x00008000uL)

/* FE level intrrupt max count for RAM mode */
#define ICUM_RAM_FEINT_MAXCNT          ICUM_INT_FETBL_MAX

/* RAM vector address MUSK */
#define ICUM_INT_RAMVECT_ADDRMUSK      (0xFFFFFE00uL)

#define ICUM_INT_RAMVECT_POS_0         (0u)
#define ICUM_INT_RAMVECT_POS_1         (1u)
#define ICUM_INT_RAMVECT_POS_2         (2u)

#define ICUM_INT_TABLEOFFSET_0         (0u)
#define ICUM_INT_TABLEOFFSET_1         (1u)

#define ICUM_INT_SET_BITSFT_1          (0x00000001uL)
#define ICUM_INT_CLEAR_TABLE           (0u)
#define ICUM_INT_SYSERR_FACT           (0uL)

/* Degradation bit:1 */     
#define ICUM_INT_DEG_PORTN_15          (1u)

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* A table for calling a function (per channel) defined by the interrupt type */
/* when an EI interrupt occurs during ROM operation.                          */
typedef void (*p_callback_t) (void);
typedef struct ICUM_INT_EI_FPTBL
{
    p_callback_t      p_callbackfunc;            /* Interrupt callback func    */
} ICUM_INT_EI_FPTBL_t;

/* A table for calling a function (per channel) defined by the interrupt type  */
/* when an FE interrupt occurs during RAM operation.                           */
typedef struct ICUM_INT_FE_FPTBL
{
    uint16_t          w_offset;                  /* Interrupt Vector offset    */
    uint8_t           b_jumpcode[2];             /* Program Jump code          */
    p_callback_t      p_callbackfunc;            /* Interrupt callback func    */
    uint32_t          l_reserve[2];
} ICUM_INT_FE_FPTBL_t;

/*====================================================================================*/
/* Union for pointer manipulation                                                     */
/*====================================================================================*/
typedef union ptr_manip
{
    void        *p_ptr;
    uint32_t    val;
} p_int_manip_t;

typedef union ptr_manip_cb
{
    p_callback_t p_ptr;
    uint32_t     val;
} p_int_calbk_t;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
static uint32_t d_INT_ChgRomToRam(void);
static uint32_t d_INT_ChgRamToRom(void);
static void d_INT_SetPriorityMask(void);

#endif /* #if !defined(ICUM_D_INT_LCL_H) */

