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

// ------------------------------------------------------------
// force the integration of the copyright notice
// ------------------------------------------------------------
#pragma ghs reference sys_get_package_info

// ************************************************************************************************************************
// very simple system services for minimal "hello word" type of application
// ************************************************************************************************************************
/*

    - RH850/U2C PE1 UART mapping:
        - VUART0    -> RLIN31
                       RLIN31TX -> P17_2 -> connecting to UART0TX on main board
                       RLIN31RX -> P17_3 -> connecting to UART0RX on main board

        - VPORT0    -> P21.5  -> connecting to LED7  on main board (CN1 DIGIO_3)
        - VPORT1    -> P21.4  -> connecting to LED8  on main board (CN1 DIGIO_2)
        - VPORT2    -> P21.3  -> connecting to LED9  on main board (CN1 DIGIO_1)
        - VPORT3    -> P21.2  -> connecting to LED10 on main board (CN1 DIGIO_0)

        - VTIMER0   -> OSTM0
        - VTIMER1   -> OSTM1

*/

// ************************************************************************************************************************
// NOTE !  The UART management is in a separate file
//         The reason being to remove this object (and reference to global vars) if the COM is not used
// ************************************************************************************************************************

#include DEVICE_FILE

#include "v850.toolbox.h"

#include "sys.config.h"
#include "sys.api.h"
#include <stdint.h>
#include "u2c4_clock.h"

#if (SYS_QUARTZ_FREQ != 20000000)
    #error INCORRECT FREQUENCY
#endif

// ------------------------------------------------------------
// local macros
// ------------------------------------------------------------

#define PASTER(x, y)        x ## y
#define EVALUATOR(x, y)     PASTER(x, y)

// defines the interrupt vectors for timers
#define INT_VTIMER0     EIINT63
#define INT_VTIMER1     EIINT484

#define protected_write(N, reg, value) \
    do{\
          (EVALUATOR(PROTCMD, N))=0xa5u; (reg)=(value); (reg)=~(value); (reg)=(value);\
    }while((EVALUATOR(PROTS, N))==1u)

// ------------------------------------------------------------
// local defines
// ------------------------------------------------------------

// defines the port mode control registers
#define VPORT0_MODECTL  (PORT0PMC21_5)
#define VPORT1_MODECTL  (PORT0PMC21_4)
#define VPORT2_MODECTL  (PORT0PMC21_3)
#define VPORT3_MODECTL  (PORT0PMC21_2)

// defines the port mode registers
#define VPORT0_MODE     (PORT0PM21_5)
#define VPORT1_MODE     (PORT0PM21_4)
#define VPORT2_MODE     (PORT0PM21_3)
#define VPORT3_MODE     (PORT0PM21_2)

#define VPORT0_SET(LEVLE)   (PORT0PSR21 = ((1<<5U) << 16U) | (LEVLE<<5U))
#define VPORT1_SET(LEVLE)   (PORT0PSR21 = ((1<<4U) << 16U) | (LEVLE<<4U))
#define VPORT2_SET(LEVLE)   (PORT0PSR21 = ((1<<3U) << 16U) | (LEVLE<<3U))
#define VPORT3_SET(LEVLE)   (PORT0PSR21 = ((1<<2U) << 16U) | (LEVLE<<2U))

// CLK_HSB value
#define CLK_HSB 80U

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long PLLENTRG    : 1;  /* b00     : PLL Enable Trigger  */
        unsigned long PLLDISTRG   : 1;  /* b01     : PLL Disable Trigger */
        unsigned long             :30;  /* b31-b02 : Reserved            */
    } bit;
} ST_PLLCLKE;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long PLLCLKEN    : 1;  /* b00     : PLL clock enable state */
        unsigned long PLLCLKSTAB  : 1;  /* b01     : PLL clock stable state */
        unsigned long             :30;  /* b31-b02 : Reserved               */
    } bit;
} ST_PLLCLKS;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long SELMPERCENT : 3;  /* b02-b00 : SSCG Modulation dithered range control        */
        unsigned long SELMFREQ    : 5;  /* b07-b03 : PDF frequency of the set modulation frequency */
        unsigned long             : 1;  /* b08     : Reserved                                      */
        unsigned long SSMODE1     : 1;  /* b09     : SSCG Control                                  */
        unsigned long             :22;  /* b31-b10 : Reserved                                      */
    } bit;
} ST_SSCGCTL;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long SELMPERCENT : 3;  /* b02-b00 : SSCG1 Modulation dithered range control       */
        unsigned long SELMFREQ    : 5;  /* b07-b03 : PDF frequency of the set modulation frequency */
        unsigned long             : 1;  /* b08     : Reserved                                      */
        unsigned long SSMODE1     : 1;  /* b09     : SSCG1 Control                                 */
        unsigned long             :22;  /* b31-b10 : Reserved                                      */
    } bit;
} ST_SSCG1CTL;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long CPUCLKSCSID : 1;  /* b00     : Clock source control for CLK_SYS */
        unsigned long             :31;  /* b31-b01 : Reserved                         */
    } bit;
} ST_CKSCCPUC;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long CPUCLKSACT  : 1;  /* b00     : Clock source status for CLK_SYS */
        unsigned long             :31;  /* b31-b01 : Reserved                        */
    } bit;
} ST_CKSCCPUS;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long SYSCLKSCSID : 1;  /* b00     : Clock source control for CLK_SYS_SSCG */
        unsigned long             :31;  /* b31-b01 : Reserved                              */
    } bit;
} ST_CKSSSCGC;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long SYSCLKSACT  : 1;  /* b00     : Clock source status for CLK_SYS_SSCG */
        unsigned long             :31;  /* b31-b01 : Reserved                             */
    } bit;
} ST_CKSSSCGS;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long SYSCLKSCSID : 1;  /* b00     : Clock source control for CLK_SYS_SSCG1 */
        unsigned long             :31;  /* b31-b01 : Reserved                               */
    } bit;
} ST_CKSSSCG1C;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long SYSCLKSACT  : 1;  /* b00     : Clock source status for CLK_SYS_SSCG1 */
        unsigned long             :31;  /* b31-b01 : Reserved                              */
    } bit;
} ST_CKSSSCG1S;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long PLLCLKDCSID : 4;  /* b03-b00 : Clock Divider control */
        unsigned long             :28;  /* b31-b04 : Reserved              */
    } bit;
} ST_CLKDPLLC;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long             : 1;  /* b00     : Reserved                   */
        unsigned long PLLCLKDSYNC : 1;  /* b01     : Divider clock synchronized */
        unsigned long             :30;  /* b31-b02 : Reserved                   */
    } bit;
} ST_CLKDPLLS;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long SSCGCLKDCSID : 4;  /* b03-b00 : Clock Divider control */
        unsigned long              :28;  /* b31-b04 : Reserved              */
    } bit;
} ST_CKDSSCGC;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long              : 1;  /* b00     : Reserved                   */
        unsigned long SSCGCLKDSYNC : 1;  /* b01     : Divider clock synchronized */
        unsigned long              :30;  /* b31-b02 : Reserved                   */
    } bit;
} ST_CKDSSCGS;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long SSCG1CLKDCSID : 4;  /* b03-b00 : Clock Divider control */
        unsigned long               :28;  /* b31-b04 : Reserved              */
    } bit;
} ST_CKDSSCG1C;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long               : 1;  /* b00     : Reserved                   */
        unsigned long SSCG1CLKDSYNC : 1;  /* b01     : Divider clock synchronized */
        unsigned long               :30;  /* b31-b02 : Reserved                   */
    } bit;
} ST_CKDSSCG1S;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long RLINSCSID   : 2;  /* b01-b00 : Clock source control for CLK_RLIN */
        unsigned long             :30;  /* b31-b02 : Reserved                          */
    } bit;
} ST_CKSCRLINC;
typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long RLINSACT    : 2;  /* b01-b00 : Clock source status for CLK_RLIN */
        unsigned long             :30;  /* b31-b02 : Reserved                         */
    } bit;
} ST_CKSCRLINS;
typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long KCE         : 1;  /* b00     : Key Code Enable bit                           */
        unsigned long KCPROT      :31;  /* b31-b01 : Enable or disable modification of the KCE bit */
    } bit;
} ST_CLKKCPROT1;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long MOSCENTRG   : 1;  /* b00     : Main OSC Enable Trigger  */
        unsigned long MOSCDISTRG  : 1;  /* b01     : Main OSC Disable Trigger */
        unsigned long             :30;  /* b31-b02 : Reserved                 */
    } bit;
} ST_MOSCE;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long MOSCEN      : 1;  /* b00     : Main OSC enable state */
        unsigned long MOSCSTAB    : 1;  /* b01     : Main OSC stable state */
        unsigned long             :30;  /* b31-b02 : Reserved              */
    } bit;
} ST_MOSCS;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long AWDTSCSID   : 1;  /* b00     : Clock source control for CLKA_WDT */
        unsigned long             :31;  /* b31-b01 : Reserved                          */
    } bit;
} ST_CKSCAWDTC;

typedef union
{
    unsigned long dword;
    struct
    {
        unsigned long AWDTSACT    : 1;  /* b00     : Clock source status for CLKA_WDT */
        unsigned long             :31;  /* b31-b01 : Reserved                         */
    } bit;
} ST_CKSCAWDTS;

#define CLK_BASE    (0xFF980000UL)
#define PLLCLKE     (*(volatile ST_PLLCLKE    *)(CLK_BASE + 0x0000U))   /* PLL Enable register                           */
#define PLLCLKS     (*(volatile ST_PLLCLKS    *)(CLK_BASE + 0x0004U))   /* PLL Status register                           */
#define SSCGCTL     (*(volatile ST_SSCGCTL    *)(CLK_BASE + 0x002CU))   /* SSCG Control Register                         */
#define SSCG1CTL    (*(volatile ST_SSCG1CTL   *)(CLK_BASE + 0x004CU))   /* SSCG1 Control Register                        */
#define CKSC_CPUC   (*(volatile ST_CKSCCPUC   *)(CLK_BASE + 0x0100U))   /* CLK_CPU Selector Control Register             */
#define CKSC_CPUS   (*(volatile ST_CKSCCPUS   *)(CLK_BASE + 0x0108U))   /* CLK_CPU Selector Status Register              */
#define CKS_SSCGC   (*(volatile ST_CKSSSCGC   *)(CLK_BASE + 0x0110U))   /* CLK_SYS_SSCG Selector Control Register        */
#define CKS_SSCGS   (*(volatile ST_CKSSSCGS   *)(CLK_BASE + 0x0114U))   /* CLK_SYS_SSCG Selector Status Register         */
#define CKS_SSCG1C  (*(volatile ST_CKSSSCG1C  *)(CLK_BASE + 0x0118U))   /* CLK_SYS_SSCG1 Selector Control Register       */
#define CKS_SSCG1S  (*(volatile ST_CKSSSCG1S  *)(CLK_BASE + 0x011CU))   /* CLK_SYS_SSCG1 Selector Status Register        */
#define CLKD_PLLC   (*(volatile ST_CLKDPLLC   *)(CLK_BASE + 0x0120U))   /* CLK_PLLO Divider Control Register             */
#define CLKD_PLLS   (*(volatile ST_CLKDPLLS   *)(CLK_BASE + 0x0128U))   /* CLK_PLLO Divider Status Register              */
#define CKD_SSCGC   (*(volatile ST_CKDSSCGC   *)(CLK_BASE + 0x0130U))   /* CLK_SSCG Divider Control Register             */
#define CKD_SSCGS   (*(volatile ST_CKDSSCGS   *)(CLK_BASE + 0x0134U))   /* CLK_SSCG Divider Status Register              */
#define CKD_SSCG1C  (*(volatile ST_CKDSSCG1C  *)(CLK_BASE + 0x0138U))   /* CLK_SSCG1 Divider Control Register            */
#define CKD_SSCG1S  (*(volatile ST_CKDSSCG1S  *)(CLK_BASE + 0x013CU))   /* CLK_SSCG1 Divider Status Register             */
#define CKSC_RLINC  (*(volatile ST_CKSCRLINC  *)(CLK_BASE + 0x0140U))   /* CLK_RLIN Selector Control Register            */
#define CKSC_RLINS  (*(volatile ST_CKSCRLINS  *)(CLK_BASE + 0x0148U))   /* CLK_RLIN Selector Status Register             */
#define CLKKCPROT1  (*(volatile ST_CLKKCPROT1 *)(CLK_BASE + 0x0700U))   /* Clock Ctrl Reg Key Code Protection Register 1 */
#define MOSCE       (*(volatile ST_MOSCE      *)(CLK_BASE + 0x8000U))   /* Main oscillation Enable register              */
#define MOSCS       (*(volatile ST_MOSCS      *)(CLK_BASE + 0x8004U))   /* Main oscillation Status register              */
#define CKSC_AWDTC  (*(volatile ST_CKSCAWDTC  *)(CLK_BASE + 0x8200U))   /* CLKA_WDT Selector Control Register            */
#define CKSC_AWDTS  (*(volatile ST_CKSCAWDTS  *)(CLK_BASE + 0x8208U))   /* CLKA_WDT Selector Status Register             */

/* ICU-M firmware status */
#define ICU2PES     (*(volatile unsigned long *)0xFF1F0010UL)           /* Register Address */
#define ST_PLLSET   (0x40000000UL)                                      /* PLL Set by ICU-M */

#define OSTM_MODULE_KEY SYSCTRLMSRKCPROT
#define OSTM_MODULE_CTRL SYSCTRLMSR_OSTM


// ------------------------------------------------------------
// local variables
// ------------------------------------------------------------

// clock frequency (for further action)
unsigned char VUART0_CLK;
unsigned char VTIMER0_CLK;
unsigned char VTIMER1_CLK;

// vtimer0_intcount is incremented by 1 when the vtimer0 interrupt is served
static unsigned long vtimer0_intcount;

// VTIMER1_1msec is Count up every 1 msec
static unsigned long  VTIMER1_1msec;

typedef struct
{
    void (*pfunc_callback)(void);
    unsigned long period_ms;
    unsigned long next_time;
} VTIMER1_CALLBACK;

#define VTIMER1_CALLBACK_ENTRY_MAX (2) 
static VTIMER1_CALLBACK vtimer_callback_entry[VTIMER1_CALLBACK_ENTRY_MAX] = {0};

// ************************************************************************************************************************
// watchdog management
// ************************************************************************************************************************

/*===========================================================================
  function: sys_watchdog_stop
  [PUBLIC]
  - stops the watchdog
  ===========================================================================*/
void sys_watchdog_stop(void)
{
    // option byte after reset should ensure that the WDTA does not start automatically
    // do nothing
}

// ************************************************************************************************************************
// clock tree configuration
// ************************************************************************************************************************

/*===========================================================================
  function: sys_clock_quartzfreq
  [PUBLIC]
  - returns the quartz frequency (in Hz) that configures the system
  ===========================================================================*/
unsigned long sys_clock_quartzfreq(void)
{
    return (SYS_QUARTZ_FREQ);
}

/*===========================================================================
  function: sys_clock_maxfreq
  [PUBLIC]
  - sets the CPU frequency to maximum allowed frequency
  ===========================================================================*/
void sys_clock_maxfreq(void)
{
#if defined(__RH850_U2C4__)
    if (u2c4_clock_init() == 0UL)
    {
        return;
    }
    U2C4_CLOCK_REG(0xFF980700UL) = 0xA5A5A501UL;
    SYSCTRLCKS_RLINC = 1UL;
    if (!u2c4_clock_wait((uint32_t)&SYSCTRLCKS_RLINS, 3UL, 1UL))
    {
        U2C4_CLOCK_REG(0xFF980700UL) = 0xA5A5A500UL;
        return;
    }
    SYSCTRLCKS_AWDTC = 0UL;
    U2C4_CLOCK_REG(0xFF980700UL) = 0xA5A5A500UL;
    VUART0_CLK = 80U;
    VTIMER0_CLK = 80U;
    VTIMER1_CLK = 80U;
#else
    unsigned long wait_cnt = 0U;

    /* Division ratio of clock source PLL is changed from 1/1 to 3/8(800/640MHz) or 4/8(480MHz). */
    unsigned char clkd_val      = 6U;    /* 3/8(800/640MHz) */
    unsigned char clkd_loop_cnt = 5U;    /* clock source change count(5th) */
      unsigned char i;

    /* Clock Controler Setup */
    /* CLK_CPU  =  400[MHz]  */
    /* CLK_SBUS =  200[MHz]  */
    /* CLK_HBUS =  100[MHz]  */
    /* CLK_UHSB =  160[MHz]  */
    /* CLK_SHSB =  133[MHz]  */
    /* CLK_HSB  =   80[MHz]  */
    /* CLK_LSB  =   40[MHz]  */
    /* CLK_RLIN =   80[MHz]  */
    /* CLKA_WDT = 0.24[MHz]  */

    /* Release the write protection of Clock controller register. */
    CLKKCPROT1.dword = 0xA5A5A501UL;

    /* Check the necessity of clock setting. */
    /* If already set by the ICU-M, the process is skipped. */
    if (ST_PLLSET != (ICU2PES & ST_PLLSET))
    {
        /* Confirm the necessity of issuing Main OSC Enable Trigger. */
        if (0U == MOSCS.bit.MOSCEN)
        {
            /* Wait for the Main OSC to stop. */
            while (0U != MOSCS.bit.MOSCSTAB) ;
            /* Start the Main OSC. */
            MOSCE.bit.MOSCENTRG = 1U;
        }

        /* Confirm that the Main OSC has been stable.  */
        /* Read the MOSCS.bit.MOSCSTAB = 1 four times. */
        while (4U > wait_cnt) {
            if ((1U == MOSCS.bit.MOSCEN) && (1U == MOSCS.bit.MOSCSTAB))
            {
                wait_cnt++;
            }
            else
            {
                wait_cnt = 0U;
            }
        }

        /* Confirm the necessity of issuing PLL Enable Trigger. */
        if (0U == PLLCLKS.bit.PLLCLKEN)
        {
            /* Wait for the PLL to stop. */
            while (0U != PLLCLKS.bit.PLLCLKSTAB) ;
            /* Start the PLL. */
            PLLCLKE.bit.PLLENTRG = 1U;
        }

        /* Confirm that the PLL has been stable.           */
        /* Read the PLLCLKS.bit.PLLCLKSTAB = 1 four times. */
        wait_cnt = 0U;
        while (4U > wait_cnt) {
            if ((1U == PLLCLKS.bit.PLLCLKEN) && (1U == PLLCLKS.bit.PLLCLKSTAB))
            {
                wait_cnt++;
            }
            else
            {
                wait_cnt = 0U;
            }
        }

        /* Division ratio of clock source PLL is changed from 1/1 to 3/8(800/640MHz). */
        CLKD_PLLC.bit.PLLCLKDCSID = clkd_val;

        /* Wait for the Divider clock synchronized. */
        while (1U != CLKD_PLLS.bit.PLLCLKDSYNC);

        /* The clock source for the System clock is changed from CLK_IOSC to CLK_PLLO. */
        CKSC_CPUC.bit.CPUCLKSCSID = 0U;

        /* The clock source for the System clock is changed from CLK_IOSC to CLK_SYS_SSCG. */
        CKS_SSCGC.bit.SYSCLKSCSID = 0U;

        /* Wait for the System clock is changed. */
        while ((0U != CKSC_CPUS.bit.CPUCLKSACT) &&
               (0U != CKS_SSCGS.bit.SYSCLKSACT)   );

        /* Wait 100us after verifying. */
        wait_cnt = 20000U;
        while (0U < wait_cnt)
        {
            wait_cnt--;
        }

        /* Division ratio of clock source PLL is changed from 1/1 to 3/8(800/640MHz) or 4/8(480MHz). */
        CLKD_PLLC.bit.PLLCLKDCSID = clkd_val;

        /* Division ratio of clock source SSCG is changed from 1/1 to 3/8(800/640MHz) or 4/8(480MHz). */
        CKD_SSCGC.bit.SSCGCLKDCSID = clkd_val;

        /* Wait for the Divider clock synchronized. */
        while ((1U != CLKD_PLLS.bit.PLLCLKDSYNC)  &&
               (1U != CKD_SSCGS.bit.SSCGCLKDSYNC)   );

        /* The clock source for the System clock is changed from CLK_IOSC to CLK_PLLO. */
        CKSC_CPUC.bit.CPUCLKSCSID = 0U;

        /* The clock source for the System clock is changed from CLK_IOSC to CLK_SYS_SSCG. */
        CKS_SSCGC.bit.SYSCLKSCSID = 0U;

        /* Wait for the System clock is changed. */
        while ((0U != CKSC_CPUS.bit.CPUCLKSACT) &&
               (0U != CKS_SSCGS.bit.SYSCLKSACT)   );

        /* Wait 100us after verifying. */
        wait_cnt = 20000U;
        while (0U < wait_cnt)
        {
            wait_cnt--;
        }

        /* Division retio of clock source PLL/SSCGi is changed.        */
        /* 4/8(1st)->5/8(2nd)->6/8(3rt)->7/8(4th)->1(5th) (800/640MHz) */
        /* 5/8(1st)->6/8(2nd)->7/8(3rd)->1(4th)           (480MHz)     */
        for (i = 0U; i < clkd_loop_cnt; i++)
        {

            if (clkd_val < 14U)
            {
                clkd_val+=2;
            } else {
                clkd_val = 0;
            }

            /* Division ratio of clock source PLL is changed  */
            CLKD_PLLC.bit.PLLCLKDCSID = clkd_val;

            /* Division ratio of clock source SSCG is changed  */
            CKD_SSCGC.bit.SSCGCLKDCSID = clkd_val;

            /* Wait for the Divider clock synchronized. */
            while ((1U != CLKD_PLLS.bit.PLLCLKDSYNC) && (1U != CKD_SSCGS.bit.SSCGCLKDSYNC)) ;
        }

        /* Wait 100us after verifying. */
        wait_cnt = 40000U;
        while (0U < wait_cnt)
        {
            wait_cnt--;
        }
    }

    /* The clock source for the RLIN Clock is changed from CLK_HSB to CLK_MOSC. */
    CKSC_RLINC.bit.RLINSCSID = 1U;      // CLK_HBUS(80MHz)

    /* Wait for the RLIN Clock is changed. */
    while (1U != CKSC_RLINS.bit.RLINSACT) ;

    /* The clock source for the WDTBA Clock is changed from CLK_LSIOSC/128 to CLK_LSIOSC/1. */
    CKSC_AWDTC.bit.AWDTSCSID = 0U;

    /* Wait for the WDTBA Clock is changed. */
    while (0U != CKSC_AWDTS.bit.AWDTSACT) ;

    /* Leave the clock source of the ICUM_WDTA0 clock at the default CLK_HSIOSC/640. */

    /* Set the write protection of Clock controller. */
    CLKKCPROT1.dword = 0xA5A5A500UL;

    /* CLK_RLIN */
    VUART0_CLK  = 80U;

    /* CLK_HSB */
    VTIMER0_CLK = 80U;

    VTIMER1_CLK = 80U;

    // disable interruptions in case of misaligned accesses
    {
        unsigned long tmp;
        tmp  = STSR(MCTL);
        tmp |= 0x0002;
        LDSR(tmp, MCTL);
    }
#endif
}
// ************************************************************************************************************************
// VTIMER0 timer management
//      - VTIMER0   -> OSTM0
// 
// ************************************************************************************************************************

/*===========================================================================
  function: INT_VTIMER0
  [PUBLIC]
  - interrupt vector INTOSTM0TINT (EIINT199)
  - define EIINT199_ENABLE in the file pe.intvect.850
  ===========================================================================*/
__interrupt void INT_VTIMER0(void)
{
    vtimer0_intcount++;
}

/*===========================================================================
  function: sys_vtimer0_start
  [PUBLIC]
  - starts vtimer0 (clears the clock counter)
  
  configuration:
  - vtimer0_intcount is incremented by overflow OS Timer 
  - timing in 
  - explanation:
        TAUJ0 is counting downwards from CRDx to 0
  ===========================================================================*/
void sys_vtimer0_start(void)
{
    /* OSTM0 module Active */
    OSTM_MODULE_KEY = 0xA5A5A501;
    OSTM_MODULE_CTRL = 0;
    OSTM_MODULE_KEY = 0xA5A5A500;

    /*******************/
    /* Timer  Setting  */
    /*******************/

    /* stop timers */
    OSTM0TT = 1;

    /* interrupts at count start enabled */
    OSTM0MD0 = 1;  

    /* Free-run compare mode             */
    OSTM0MD1 = 1;  

    /* Load 0 in OSTM0CNT at counter operation start */
    OSTM0MD2 = 0;  
     
    /* interrupt output is enabled */
    OSTM0IE  = 1;  

    /* Setting value Compare match */
    OSTM0CMP = 0xFFFFFFFF;

    /* Timer-output toggling mode */
    OSTM0TOE = 1;

    /*******************/
    /* Timer Start     */
    /*******************/
    /* start timer */
    OSTM0TS = 1;
}

/*===========================================================================
  function: sys_vtimer0_stop
  [PUBLIC]
  - stops vtimer0
  ===========================================================================*/
void sys_vtimer0_stop(void)
{
    /* stop timers */
    OSTM0TT = 1;
}

/*===========================================================================
  function: sys_vtimer0_cnv_pclk2time
  [PUBLIC]
  - in in_pclk_count the pclk count value
       pclk = VTIMER0_CLK[MHz]
  - returns the (raw) 32-bit counter value [usec]
  ===========================================================================*/
unsigned long sys_vtimer0_cnv_pclk2time(const unsigned long long in_pclk_count,time_t *outtime)
{
    unsigned long long row_count64_ns;
    unsigned long long temp;

    row_count64_ns  = (in_pclk_count * 1000);
    row_count64_ns /= VTIMER0_CLK;

    temp = row_count64_ns;

    outtime->sec    = temp / 1000000000U;
    temp            = temp % 1000000000U;

    outtime->msec   = temp / 1000000U;
    temp            = temp % 1000000U;

    outtime->usec   = temp / 1000U;
    outtime->nsec   = temp % 1000U;

    return row_count64_ns;
}

/*===========================================================================
    function: sys_vtimer0_cnv_pclk2time_pe2
    [PUBLIC]
    - in in_pclk_count the pclk count value
        pclk = VTIMER0_CLK[MHz]
    - returns the (raw) 32-bit counter value [usec]
  ===========================================================================*/
unsigned long sys_vtimer0_cnv_pclk2time_pe2(const unsigned long long in_pclk_count,time_t *outtime)
{
    unsigned long long row_count64_ns;
    unsigned long long temp;

    row_count64_ns  = (in_pclk_count * 1000);
    row_count64_ns /= CLK_HSB;

    temp = row_count64_ns;

    outtime->sec    = temp / 1000000000U;
    temp            = temp % 1000000000U;

    outtime->msec   = temp / 1000000U;
    temp            = temp % 1000000U;

    outtime->usec   = temp / 1000U;
    outtime->nsec   = temp % 1000U;

    return row_count64_ns;
}

/*===========================================================================
  function: sys_vtimer0_cnv_pclk2time
  [PUBLIC]
  - returns the (raw) 32-bit timer counter value [pclk]
  ===========================================================================*/
unsigned long sys_vtimer0_get_pclk_counter(void)
{
    return OSTM0CNT;
}

// ************************************************************************************************************************
// port management
// ************************************************************************************************************************

/*===========================================================================
  function: sys_vport_config
  [PUBLIC]
  - configure the virtual ports 0-3 in port mode + output
  ===========================================================================*/
void sys_vport_config(void)
{
    VPORT0_MODECTL = 0;
    VPORT1_MODECTL = 0;
    VPORT2_MODECTL = 0;
    VPORT3_MODECTL = 0;

    VPORT0_MODE  = 0;
    VPORT1_MODE  = 0;
    VPORT2_MODE  = 0;
    VPORT3_MODE  = 0;
}

/*===========================================================================
  function: sys_vport_set_high
  [PUBLIC]
  - sets the virtual port n to HIGH (1)
  ===========================================================================*/
void sys_vport_set_high(unsigned char n)
{
    switch(n)
    {
        case 0: VPORT0_SET(1); break;
        case 1: VPORT1_SET(1); break;
        case 2: VPORT2_SET(1); break;
        case 3: VPORT3_SET(1); break;
    }
}

/*===========================================================================
  function: sys_vport_set_low
  [PUBLIC]
  - sets the virtual port n to LOW (0)
  ===========================================================================*/
void sys_vport_set_low(unsigned char n)
{
    switch(n)
    {
        case 0: VPORT0_SET(0); break;
        case 1: VPORT1_SET(0); break;
        case 2: VPORT2_SET(0); break;
        case 3: VPORT3_SET(0); break;
    }
}

// ************************************************************************************************************************
// delayed callback function
//      - VTIMER1   -> TAUB0.2
// ************************************************************************************************************************

/*===========================================================================
  function: INT_VTIMER1
  [PUBLIC]
  - interrupt vector on VTIMER1 compare match
  - stop the timer
  - re-enable interrupt servicing (ei)
  ===========================================================================*/
__interrupt void INT_VTIMER1(void)
{
    unsigned int i;
    VTIMER1_CALLBACK *p;

    VTIMER1_1msec++;

    for (i = 0 ; i < VTIMER1_CALLBACK_ENTRY_MAX ; i++)
    {
        p = &vtimer_callback_entry[i];

        if (p->pfunc_callback != 0)
        {
            if (VTIMER1_1msec == p->next_time)
            {
                if (p->period_ms == 0)
                {
                    p->pfunc_callback = 0;
                }
                else
                {
                    p->pfunc_callback();
                    p->next_time += p->period_ms;
                }
            }
        }
    }
}

/*===========================================================================
  function: sys_vtimer1_set_func
  [PUBLIC]
  - sets the function that will be called when vtimer1 has elapsed
  ===========================================================================*/
void sys_vtimer1_set_func(void (*pfunc)(void),unsigned long time_ms)
{
    if (time_ms != 0)
    {
        __DI();
        vtimer_callback_entry[0].pfunc_callback = pfunc;
        vtimer_callback_entry[0].next_time      = VTIMER1_1msec + time_ms;
        vtimer_callback_entry[0].period_ms      = time_ms;
        __EI();
    }
    else
    {
        __DI();
        vtimer_callback_entry[0].pfunc_callback = 0;
        __EI();
        sys_vport_set_low(0);
        sys_vport_set_low(1);
        sys_vport_set_low(2);
        sys_vport_set_low(3);
    }
}

/*===========================================================================
function: sys_vtimer1_set_func (ID)
  [PUBLIC]
  - sets the function that will be called when vtimer1 has elapsed
  ===========================================================================*/
void sys_vtimer1_set_func_evt(void (*pfunc)(void),unsigned long time_ms,unsigned short id)
{
    if (time_ms != 0)
    {
        __DI();
        vtimer_callback_entry[id].pfunc_callback = pfunc;
        vtimer_callback_entry[id].next_time      = VTIMER1_1msec + time_ms;
        vtimer_callback_entry[id].period_ms      = time_ms;
        __EI();
    }
    else
    {
        __DI();
        vtimer_callback_entry[id].pfunc_callback = 0;
        __EI();
        sys_vport_set_low(0);
        sys_vport_set_low(1);
        sys_vport_set_low(2);
        sys_vport_set_low(3);
    }
}

/*===========================================================================
  function: sys_vtimer1_start
  [PUBLIC]
  - cycle handle 
  ===========================================================================*/
void sys_vtimer1_start(void)
{
    /* OSTM0 module Active */
    OSTM_MODULE_KEY = 0xA5A5A501;
    OSTM_MODULE_CTRL = 0;
    OSTM_MODULE_KEY = 0xA5A5A500;

    /*******************/
    /* Timer  Setting  */
    /*******************/

    /* stop timers */
    OSTM1TT = 1;

    /* interrupts at count start enabled */
    OSTM1MD0 = 1;

    /* Interval mode mode  */
    OSTM1MD1 = 0;

    /* OSTM1CMP value is loaded to OSTMnCNT[31:0] at counter start.t */
    OSTM1MD2 = 0;

    /* interrupt output is enabled */
    OSTM1IE  = 1;

    /* Setting value Compare match value for 1st time. */
    OSTM1CMP = (VTIMER1_CLK * 1000U) - 1U;

    /*******************/
    /* INTC  Setting   */
    /*******************/

    /* Detection with an edge */
    //INTC2EICT200 = 0U;

    /* Priority Lavel 0       */
    INTC2EIP484 = 0U;

    /* Table reference method */
    INTC2EITB484 = 1U;

    /* Interrupt processing is enabled */
    INTC2EIMK484 = 0U;

    /***************/
    /* start timer */
    /***************/
    VTIMER1_1msec = 0;
    OSTM1TS = 1;
}
