/******************************************************************************/
/* Component Name   ICUM CLK Driver                                           */
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

/* Header Files                         */
#include ICUM_DEVICE_FILE
#include "renesas_types.h"

#include "icum_common_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_clk_pub.h"
#include "icum_d_clk.h"
/* Include prototypes of interrupt handlers */
#include "icum_int_declaration_pub.h"

/***************************************************************************************/
/* Internal Definition                                                                 */
/***************************************************************************************/

#define MSRKCPROT           (*(volatile uint32_t *) 0xFF981710UL)
#define MSR_DFP             (*(volatile uint32_t *) 0xFF9811D0UL)

/***************************************************************************************/
/* Static                                                                              */
/***************************************************************************************/
/* Executing Result of D_CLK_Init */
static uint16_t s_clkinit_result = CLK_ERRFREQUENCY_25M;
/* A variable to hold the address of the callback function */
static icum_clma_cb_regist_t clma_cb_regist;
/***************************************************************************************/
/* Prototypes (Internal)                                                               */
/***************************************************************************************/
/* None */

/***************************************************************************************
* Functions
***************************************************************************************/

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_CLK_0001]:[ICUMFW_UD_U2B_D_CLK_0001]
* Function name: D_CLK_Init
* Description  : CLK Driver Initialization
* Arguments    : None
* Return Value : ER_OK
*                ER_PARAM
*                ER_D_CLK_WSTABLECLK_TOUT
*                ER_D_CLK_WSTABLEPLL_TOUT
*                ER_D_CLK_WSTABLEMULT_TOUT
***************************************************************************************/
#if defined(__RH850_U2C4__)
#include "u2c4_clock.h"
#endif

uint32_t D_CLK_Init(void)
{
#if defined(__RH850_U2C4__)
    uint32_t clock_result;
    clock_result = u2c4_clock_init();
    s_clkinit_result = (clock_result != 0UL) ? CLK_CLKFREQUENCY_80M : CLK_ERRFREQUENCY_25M;
    return (clock_result != 0UL) ? ER_OK : ER_D_CLK_WSTABLECLK_TOUT;
#else
    uint32_t             uiRet = ER_OK;
    volatile  uint32_t   opbt;
#ifdef _RH850_U2B_
    uint32_t             loop_c;
    volatile uint32_t    volloop_c;
    uint32_t             wait_cnt;

    /* SYSCTRL Register */
    volatile  uint32_t   *sysctrl_reg;
    volatile  uint32_t   *sysctrl_reg2;
    uint32_t             read_data;
    uint32_t             read_data2;

    uint32_t             clkd_set_div;
    uint32_t             clkd_chg_cnt;
    int32_t              i;


    // DFP module Active
    MSRKCPROT = 0xA5A5A501;
    MSR_DFP  |= 0x7FFFFFFFUL;
    MSRKCPROT = 0xA5A5A500;

    // Wait 100us after verifying.
    wait_cnt = 80000U;  // 200us
    while (0U < wait_cnt) { wait_cnt--; }


    /* Initialize the pointer to hold the address of the callback function */
    clma_cb_regist.p_cbfunc = NULL;

    /* 1. Get CPU Ckock from OPBBT11*/
    opbt = CLK_CFGSET_OPBT11;

    if (CLK_SYSCLCK_400M == (CLK_SYSCLCK_400M &  opbt))
    {
        s_clkinit_result = CLK_CLKFREQUENCY_100M;
    }
    else
    {
        s_clkinit_result = CLK_CLKFREQUENCY_80M;
    }

    /* 2. Unlock (Enables) write access of protected registers */
    UNLOCK_REG_PROT(SYSCTRLCLKKCPROT1);

    /* 3. Confirm the necessity of issuing Main OSC Enable Trigger & */
    /*    Confirm that the Main OSC has been stable                  */

    /* Read MOSCS : Main Oscillation Status Register */
    sysctrl_reg = (volatile uint32_t *)&SYSCTRLMOSCS;
    read_data = *sysctrl_reg;
    if (CLK_MOSCEN_ENABLE_SET != (read_data & CLK_MOSCEN_ENABLE_SET))
    {
        /* Wait for the Main OSC to stop */
        for (loop_c = 0UL; CLK_CLOCKSTABLE_COUNT > loop_c; loop_c++)
        {
            /* Read MOSCS : Main Oscillation Status Register */
            read_data = *sysctrl_reg;
            if (CLK_MOSCSTAB_STABLE != (read_data & CLK_MOSCSTAB_STABLE))
            {
                /* When Main OSC clock stable state (MOSCSTAB bit) is unstable */
                break;
            }
            else
            {
                /* Loop */
            }
        }
        /* Start the Main OSC */
        sysctrl_reg = (volatile uint32_t *)&SYSCTRLMOSCE;
        *sysctrl_reg = CLK_MOSCE_START;
    }

    /* Confirm that the Main OSC has been stable  */
    /* Read the MOSCS MOSCSTAB bit = 1 four times */
    wait_cnt = 0UL;
    sysctrl_reg = (volatile uint32_t *)&SYSCTRLMOSCS;
    for (loop_c = 0UL; CLK_CLOCKSTABLE_COUNT > loop_c; loop_c++)
    {
        /* Read MOSCS : Main Oscillation Status Register */
        read_data = *sysctrl_reg;
        if ((CLK_MOSCEN_ENABLE_SET | CLK_MOSCSTAB_STABLE) == 
            (read_data & (CLK_MOSCEN_ENABLE_SET | CLK_MOSCSTAB_STABLE)))
        {
            wait_cnt++;
            if (CLK_STABLE_WAIT_COUNT <= wait_cnt)
            {
                /* When Main OSC clock stable state (MOSCSTAB bit) is stable */
                break;
            }
            else
            {
                /* Loop */
            }
        }
        else
        {
            /* Reset is OSC Stable Confirmation Count */
            wait_cnt = 0UL;
            /* Loop */
        }
    }

    if (CLK_CLOCKSTABLE_COUNT > loop_c)
    {
        /* Main OSC state is stable in Time Count*/

        /* 4. Confirm the necessity of issuing PLL Enable Trigger & */
        /*    PLL Oscillation clock stable state                    */

        /* Read PLLCLKS : PLL Oscillation Status Register */
        sysctrl_reg = (volatile uint32_t *)&SYSCTRLPLLS;
        read_data = *sysctrl_reg;
        if (CLK_PLLS_ENABLE_SET != (read_data & CLK_PLLS_ENABLE_SET))
        {
            /* Wait for the PLL to stop */
            for (loop_c = 0UL; CLK_PLLSTABLE_COUNT > loop_c; loop_c++)
            {
                /* Read PLLCLKS : PLL Oscillation Status Register */
                read_data = *sysctrl_reg;
                if (CLK_PLLSTAB_STABLE != (read_data & CLK_PLLSTAB_STABLE))
                {
                    /* When PLL Oscillation clock stable state (CLKSTAB bit) */
                    /* is unstable                                           */
                    break;
                }
                else
                {
                    /* Loop */
                }
            }
            /* Start the PLL. */
            sysctrl_reg = (volatile uint32_t *)&SYSCTRLPLLE;
            *sysctrl_reg = CLK_PLLE_START;
        }

        /* Confirm that the PLL has been stable.           */
        /* Read the PLLCLKS.bit.PLLCLKSTAB = 1 four times. */
        wait_cnt = 0U;
        sysctrl_reg = (volatile uint32_t *)&SYSCTRLPLLS;
        for (loop_c = 0UL; CLK_PLLSTABLE_COUNT > loop_c; loop_c++)
        {
            /* Read PLLCLKS : PLL Oscillation Status Register */
            read_data = *sysctrl_reg;
            if ((CLK_PLLS_ENABLE_SET | CLK_PLLSTAB_STABLE) == 
                (read_data & (CLK_PLLS_ENABLE_SET | CLK_PLLSTAB_STABLE)))
            {
                wait_cnt++;
                if (CLK_STABLE_WAIT_COUNT <= wait_cnt)
                {
                    /* When PLL Oscillation clock stable state (CLKSTAB bit) */
                    /* is stable                                             */
                    break;
                }
                else
                {
                    /* Loop */
                }
            }
            else
            {
                /* Reset is PLL Stable Confirmation Count */
                wait_cnt = 0UL;
                /* Loop */
            }
        }

        sysctrl_reg  = (volatile uint32_t *)&SYSCTRLCKD_PLLC;
        *sysctrl_reg  = CLK_CLKDIV_DIV3BY8;  // 3/8(800/640MHz)

        sysctrl_reg  = (volatile uint32_t *)&SYSCTRLCKD_PLLS;

        while(1)
        {
            read_data  = *sysctrl_reg;

            if (CLK_SYNC_CORRESPOND == (read_data  & CLK_SYNC_CORRESPOND))
            {
                break;
            }
            else
            {
                ;
            }
        }

        sysctrl_reg = (volatile uint32_t *)&SYSCTRLCKS_CLEANC;
        *sysctrl_reg = CLK_CKSC_C_PLL;
        sysctrl_reg2 = (volatile uint32_t *)&SYSCTRLCKS_SSCGC;
        *sysctrl_reg2 = CLK_CKSC_C_SSCG;

        sysctrl_reg  = (volatile uint32_t *)&SYSCTRLCKS_CLEANS;
        sysctrl_reg2 = (volatile uint32_t *)&SYSCTRLCKS_SSCGS;

        while (1)
        {
            read_data = *sysctrl_reg;
            if (CLK_CKSC_S_PLL  == read_data)
            {
                break;
            }
            else
            {
                ;
            }
        }

        while (1)
        {
            read_data2 = *sysctrl_reg2;
            if (CLK_CKSC_S_SSCG  == read_data2)
            {
                break;
            }
            else
            {
                ;
            }
        }

        if (CLK_PLLSTABLE_COUNT > loop_c)
        {
            /* PLL Oscillation state is stable in Time Count*/

            /* 5. Division ratio of clock source PLL is changed from 1/1 to 1/2 */
            sysctrl_reg  = (volatile uint32_t *)&SYSCTRLCKD_PLLC;
            sysctrl_reg2 = (volatile uint32_t *)&SYSCTRLCKD_SSCGC;

            *sysctrl_reg  = CLK_CLKDIV_DIV3BY8;  // 3/8(800/640MHz)
            *sysctrl_reg2 = CLK_CLKDIV_DIV3BY8;  // 3/8(800/640MHz)
            clkd_set_div  = CLK_CLKDIV_DIV3BY8;
            clkd_chg_cnt  = CLK_CLKDIV_CHGCYCL5; // clock source change count(5th)

            /* 6. Wait for the Divider clock synchronized */
            sysctrl_reg  = (volatile uint32_t *)&SYSCTRLCKD_PLLS;
            sysctrl_reg2 = (volatile uint32_t *)&SYSCTRLCKD_SSCGS;

            for (loop_c = 0UL; CLK_DEMULTISTABLE_COUNT > loop_c; loop_c++)
            {
                /* Read PLL Clock Divider Status Register */
                read_data  = *sysctrl_reg;
                read_data2 = *sysctrl_reg2;
                if ((CLK_SYNC_CORRESPOND == (read_data  & CLK_SYNC_CORRESPOND)) &&
                    (CLK_SYNC_CORRESPOND == (read_data2 & CLK_SYNC_CORRESPOND))   )
                {
                    /* When Divider clock synchronized */
                    break;
                }
                else
                {
                    /* Loop */
                }
            }

            if (CLK_DEMULTISTABLE_COUNT > loop_c)
            {
                /* Divider clock synchronized is set in Time Count */

                /* 7. The clock source for the System clock is changed */
                /*    from CLK_IOSC to CLK_PLLO                        */
                sysctrl_reg = (volatile uint32_t *)&SYSCTRLCKS_CLEANC;
                *sysctrl_reg = CLK_CKSC_C_PLL;
                sysctrl_reg2 = (volatile uint32_t *)&SYSCTRLCKS_SSCGC;
                *sysctrl_reg2 = CLK_CKSC_C_SSCG;

                /* 8. Wait for the System clock is changed */
                sysctrl_reg  = (volatile uint32_t *)&SYSCTRLCKS_CLEANS;
                sysctrl_reg2 = (volatile uint32_t *)&SYSCTRLCKS_SSCGS;

                for (loop_c = 0UL;  CLK_PLLSTABLE_COUNT > loop_c; loop_c++)
                {
                    /* Read System Clock Selector Status Register */
                    read_data  = *sysctrl_reg;
                    if (CLK_CKSC_S_PLL   == read_data)
                    {
                        /* When Clock source status is PLL */
                        break;
                    }
                    else
                    {
                        /* Loop */
                    }
                }

                while (1)
                {
                    /* Read System Clock Selector Status Register */
                    read_data2 = *sysctrl_reg2;
                    if (CLK_CKSC_S_SSCG  == read_data2)
                    {
                        /* When Clock source status is PLL */
                        break;
                    }
                    else
                    {
                        /* Loop */
                    }
                }

                if (CLK_PLLSTABLE_COUNT > loop_c)
                {
                    /* 9. Wait for switching of the clock source */
                    /*    Provisional processing: The actual waiting time */
                    /*    is unknown                                      */
                    for (volloop_c = 0UL; volloop_c < CLK_PLL_VOLTSTABLE_COUNT; volloop_c++)
                    {
                        NOPS(1U);
                    }

                    /* PLL verification complete in Time Count */
                    /* Division retio of clock source PLL/SSCGi is changed.        */
                    /* 4/8(1st)->5/8(2nd)->6/8(3rt)->7/8(4th)->1(5th) (800/640MHz) */
                    /* 5/8(1st)->6/8(2nd)->7/8(3rd)->1(4th)           (480MHz)     */
                    for (i = 0U; i < clkd_chg_cnt; i++)
                    {
                        /* 10. Division ratio of clock source PLL is changed */
                        sysctrl_reg  = (volatile uint32_t *)&SYSCTRLCKD_PLLC;
                        sysctrl_reg2 = (volatile uint32_t *)&SYSCTRLCKD_SSCGC;

                        if (clkd_set_div == CLK_CLKDIV_DIV3BY8)  {
                            *sysctrl_reg  = CLK_CLKDIV_DIV4BY8;
                            *sysctrl_reg2 = CLK_CLKDIV_DIV4BY8;
                            clkd_set_div  = CLK_CLKDIV_DIV4BY8;
                        } else if (clkd_set_div == CLK_CLKDIV_DIV4BY8) {
                            *sysctrl_reg  = CLK_CLKDIV_DIV5BY8;
                            *sysctrl_reg2 = CLK_CLKDIV_DIV5BY8;
                            clkd_set_div  = CLK_CLKDIV_DIV5BY8;
                        } else if (clkd_set_div == CLK_CLKDIV_DIV5BY8) {
                            *sysctrl_reg  = CLK_CLKDIV_DIV6BY8;
                            *sysctrl_reg2 = CLK_CLKDIV_DIV6BY8;
                            clkd_set_div  = CLK_CLKDIV_DIV6BY8;
                        } else if (clkd_set_div == CLK_CLKDIV_DIV6BY8) {
                            *sysctrl_reg  = CLK_CLKDIV_DIV7BY8;
                            *sysctrl_reg2 = CLK_CLKDIV_DIV7BY8;
                            clkd_set_div  = CLK_CLKDIV_DIV7BY8;
                        } else {
                            *sysctrl_reg  = CLK_CLKDIV_NODIV;
                            *sysctrl_reg2 = CLK_CLKDIV_NODIV;
                            clkd_set_div  = CLK_CLKDIV_NODIV;
                        }

                        /* 11. Wait for the Divider clock synchronized */
                        sysctrl_reg = (volatile uint32_t *)&SYSCTRLCKD_PLLS;
                        sysctrl_reg2 = (volatile uint32_t *)&SYSCTRLCKD_SSCGS;

                        for (loop_c = 0UL; CLK_DEMULTISTABLE_COUNT > loop_c; loop_c++)
                        {
                            /* Read PLL Clock Divider 0 Status Register */
                            read_data  = *sysctrl_reg;
                            read_data2 = *sysctrl_reg2;
                            if ((CLK_SYNC_CORRESPOND == (read_data  & CLK_SYNC_CORRESPOND)) &&
                                (CLK_SYNC_CORRESPOND == (read_data2 & CLK_SYNC_CORRESPOND))   )
                            {
                                /* When Divider clock synchronized */
                                break;
                            }
                            else
                            {
                            /* Loop */
                            }
                        }
                        if (CLK_DEMULTISTABLE_COUNT > loop_c)
                        {
                            /* Divider clock synchronized is set in Time Count*/

                            /* 12. Wait for switching of the division ratio */
                            /*     Provisional processing: The actual waiting time */
                            /*     is unknown                                      */
                            for (volloop_c = 0UL; volloop_c < CLK_DIV_VOLTSTABLE_COUNT; volloop_c++)
                            {
                                NOPS(1U);
                            }
                        }
                        else
                        {
                            /* Case of occuring Time over when waiting for */
                            /* being set Divider clock synchronized        */
                            uiRet = ER_D_CLK_WSTABLEMULT_TOUT;
                            break;
                        }
                    }

                }
                else
                {
                    /* Case of occuring Time over when waiting for */
                    /* PLL verification completion                 */
                    uiRet = ER_D_CLK_WSTABLEPLL_TOUT;
                }
            }
            else
            {
                /* Case of occuring Time over when waiting for */
                /* being set Divider clock synchronized        */
                uiRet = ER_D_CLK_WSTABLEMULT_TOUT;
            }
        }
        else
        {
            /* Case of occuring Time over when waiting for */
            /* stable state of PLL Oscillation clock       */
            uiRet = ER_D_CLK_WSTABLEPLL_TOUT;
        }
    }
    else
    {
        /* Case of occuring Time over when waiting for */
        /* stable state of Main OSC clock              */
        uiRet = ER_D_CLK_WSTABLECLK_TOUT;
    }

    /* 13. Lock (Disable) write access of protected registers */
    LOCK_REG_PROT(SYSCTRLCLKKCPROT1);
#else
    opbt = CLK_CFGSET_OPBT11;
    int8_t i;
    /* HS IntOSC setting */
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_ENABLE;
    SYSCTRL.HSOSCSTPM.UINT32 = _CGC_HSOSC_REQUEST_STOP;
    while ((SYSCTRL.HSOSCS.UINT32 & _CGC_HSOSC_ACTIVE) != _CGC_HSOSC_ACTIVE)
    {
        NOPS(1);
    }
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_DISABLE;
    /* MainOSC setting */
    /* Please set source code at option byte */
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_ENABLE;
    SYSCTRL.MOSCSTPM.UINT32 = _CGC_MAINOSC_REQUEST_STOP;
    SYSCTRL.MOSCE.UINT32 = _CGC_MAINOSC_START;
    while ((SYSCTRL.MOSCS.UINT32 & _CGC_MAINOSC_ACTIVE) != _CGC_MAINOSC_ACTIVE)
    {
        NOPS(1);
    }
    for (i = 0; (i < 4) && (SYSCTRL.MOSCS.BIT.MOSCSTAB == 1); i++ )
    {
        NOPS(1);
    }
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_DISABLE;
    /* PLL setting */
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_ENABLE;
    SYSCTRL.PLLSTPM.UINT32 = _CGC_PLL_REQUEST_STOP;
    SYSCTRL.PLLE.UINT32 = _CGC_PLL_START;
    while ((SYSCTRL.PLLS.UINT32 & _CGC_PLL_ACTIVE) != _CGC_PLL_ACTIVE)
    {
        NOPS(1);
    }
    for (i = 0; (i < 4) && (SYSCTRL.PLLS.BIT.PLLCLKSTAB == 1); i++ )
    {
        NOPS(1);
    }
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_DISABLE;
    /* CPU clock setting */
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_ENABLE;
    SYSCTRL.CLKD_PLLC.UINT32 = _CGC_CLK_PLLO_DIVIDER_2;
    while (_CGC_PLLC_DIVIDER_SYNC != (SYSCTRL.CLKD_PLLS.UINT32 & _CGC_PLLC_DIVIDER_SYNC))
    {
        NOPS(1);
    }
    SYSCTRL.CKSC_CPUC.UINT32 = _CGC_CPU_CLK_SOURCE_PLLO;
    while (SYSCTRL.CKSC_CPUS.UINT32 != _CGC_CPU_CLK_SOURCE_PLLO)
    {
        NOPS(1);
    }
    i = 6667;
    while (i-- );    // Wait 100us, 6667 = (100us x 1000) / (2.5ns x 6 instruction cycles)
    SYSCTRL.CLKD_PLLC.UINT32 = _CGC_CLK_PLLO_DIVIDER_1;
    while (_CGC_PLLC_DIVIDER_SYNC != (SYSCTRL.CLKD_PLLS.UINT32 & _CGC_PLLC_DIVIDER_SYNC))
    {
        NOPS(1);
    }
    i = 6667;
    while (i-- );    // Wait 100us, 6667 = (100us x 1000) / (2.5ns x 6 instruction cycles)
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_DISABLE;
    /* RLIN clock domain setting */
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_ENABLE;
    SYSCTRL.CKSC_RLINC.UINT32 = _CGC_RLIN_CLK_SOURCE_HSB;
    while (SYSCTRL.CKSC_RLINS.UINT32 != _CGC_RLIN_CLK_SOURCE_HSB)
    {
        NOPS(1);
    }
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_DISABLE;
    /* RS-CANFD(V4) clock domains setting */
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_ENABLE;
    SYSCTRL.CKSC_RCANC.UINT32 = _CGC_RCAN_CLK_SOURCE_MAINOSC_1;
    while (SYSCTRL.CKSC_RCANS.UINT32 != _CGC_RCAN_CLK_SOURCE_MAINOSC_1)
    {
        NOPS(1);
    }
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_DISABLE;
    /* ADCJn clock domains setting */
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_ENABLE;
    SYSCTRL.CKSC_ADCC.UINT32 = _CGC_ADC_CLK_SOURCE_LSB_2;
    while (SYSCTRL.CKSC_ADCS.UINT32 != _CGC_ADC_CLK_SOURCE_LSB_2)
    {
        NOPS(1);
    }
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_DISABLE;
    /* MSPI clock domains setting */
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_ENABLE;
    SYSCTRL.CKSC_MSPIC.UINT32 = _CGC_MSPIS_CLK_SOURCE_HSB;
    while (SYSCTRL.CKSC_MSPIS.UINT32 != _CGC_MSPIS_CLK_SOURCE_HSB)
    {
        NOPS(1);
    }
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_DISABLE;
    /* WDTBA clock domains setting */
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_ENABLE;
    SYSCTRL.CKSC_AWDTC.UINT32 = _CGC_AWDT_CLK_SOURCE_LSOSC_128;
    while (SYSCTRL.CKSC_AWDTS.UINT32 != _CGC_AWDT_CLK_SOURCE_LSOSC_128)
    {
        NOPS(1);
    }
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_DISABLE;
    /* TAUJ clock domains setting */
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_ENABLE;
    SYSCTRL.CKSC_ATAUJC.UINT32 = _CGC_ATAUJ_CLK_SOURCE_HSOSC_20;
    while (SYSCTRL.CKSC_ATAUJS.UINT32 != _CGC_ATAUJ_CLK_SOURCE_HSOSC_20)
    {
        NOPS(1);
    }
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_DISABLE;
    /* RTCA clock domains setting */
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_ENABLE;
    SYSCTRL.CKSC_ARTCAC.UINT32 = _CGC_ARTCA_CLK_SOURCE_LSOSC;
    while (SYSCTRL.CKSC_ARTCAS.UINT32 != _CGC_ARTCA_CLK_SOURCE_LSOSC)
    {
        NOPS(1);
    }
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_DISABLE;
    /* ADCJ2 clock domains setting */
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_ENABLE;
    SYSCTRL.CKSC_AADCC.UINT32 = _CGC_AADC_CLK_SOURCE_HSOSC_20;
    while (SYSCTRL.CKSC_AADCS.UINT32 != _CGC_AADC_CLK_SOURCE_HSOSC_20)
    {
        NOPS(1);
    }
    SYSCTRL.CLKD_AADCC.UINT32 = _CGC_AADC_CLK_DIVIDER_1;
    while (SYSCTRL.CLKD_AADCS.UINT32 != _CGC_AADC_CLK_DIVIDER_1)
    {
        NOPS(1);
    }
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_DISABLE;
    /* WDTBn clock domains setting */
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_ENABLE;
    SYSCTRL.CKSC_WDTC.UINT32 = _CGC_WDT_CLK_SOURCE_HSOSC_640;
    while (SYSCTRL.CKSC_WDTS.UINT32 != _CGC_WDT_CLK_SOURCE_HSOSC_640)
    {
        NOPS(1);
    }
    SYSCTRL.CLKKCPROT1.UINT32 = _WRITE_PROTECT_DISABLE;
    /* Synchronization processing */
    __asm("syncp");

#endif
    /* 14. Save Initialization Result */
    if (ER_OK != uiRet)
    {
        /* When Error occurred during Initialization, Save Error Value */
        if ((CLK_SYSCLCK_400M == (CLK_SYSCLCK_400M & opbt)) ||
            (CLK_SYSCLCK_320M == (CLK_SYSCLCK_320M & opbt)))
        {
            s_clkinit_result = CLK_ERRFREQUENCY_25M;
        }
        else
        {
            s_clkinit_result = CLK_ERRFREQUENCY_33M;
        }
    }
    else
    {
        /* Do Nothing */
    }

    return (uiRet);
#endif
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_CLK_0002]:[ICUMFW_UD_U2B_D_CLK_0002]
* Function name: D_CLK_GetClkFreq
* Description  : Get Clock Frequency
* Arguments    : p_clkfreq  : Area Address for setting Clock Frequency
*                
* Return Value : ER_OK
*                ER_PARAM
***************************************************************************************/
uint32_t D_CLK_GetClkFreq(uint16_t *p_clkfreq)
{
    uint32_t             uiRet =  ER_OK;

    /* 1. Check Argument (p_clkfreq) */
    if (NULL != p_clkfreq)
    {
        /* 2. Set Initialization Result (CLK_HBUS Frequency or Error value) */
        *p_clkfreq = s_clkinit_result;
    }
    else
    {
        uiRet =  ER_PARAM;
    }
    return (uiRet);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_CLK_0003]:[ICUMFW_UD_U2B_D_CLK_0003]
* Function name: D_CLK_StartCLMA
* Description  : CLMAS Initialization
* Arguments    : icum_clma_cb_regist_t *p_regist_info
* Return Value : ER_OK
***************************************************************************************/
/* RAM statement */
CHANGE_SEC(text, ".icum_driver_ram")
CHANGE_SEC(rodata, ".icum_driver_const_ram")

uint32_t D_CLK_StartCLMA(icum_clma_cb_regist_t *p_regist_info)
{
    /* Check if the p_cbfunc is NULL */
    if (NULL != p_regist_info->p_cbfunc)
    {
        /* Register the callback function */
        clma_cb_regist.p_cbfunc = p_regist_info->p_cbfunc;
    }
    else
    {
        /* Do nothing */
    }
#if 0    
    /* CLMAS register write-protect OFF */
    UNLOCK_REG_PROT(CLMASICUMKCPROT1);

    /* If CLK_HBUS is 80 MHz*/
    if ((uint16_t)CLK_CLKFREQUENCY_80M == s_clkinit_result)
    {
        /* Set the lower threshold to detect an error */
        CLMASCMPL = CLK_CLMACMPL_U2B_HBUS80;
        /* Set the upper threshold to detect an error */
        CLMASCMPH = CLK_CLMACMPH_U2B_HBUS80;
        /* Enable CLMAS                               */
        CLMASCTL  = CLK_CLMACTL_ENABLED;
    }
    /* If CLK_HBUS is 100 MHz*/
    else if ((uint16_t)CLK_CLKFREQUENCY_100M == s_clkinit_result)
    {
        /* Set the lower threshold to detect an error */
        CLMASCMPL = CLK_CLMACMPL_U2B_HBUS100;
        /* Set the upper threshold to detect an error */
        CLMASCMPH = CLK_CLMACMPH_U2B_HBUS100;
        /* Enable CLMAS                               */
        CLMASCTL  = CLK_CLMACTL_ENABLED;
    }
    /* If clock gear-up failed: CLK_HBUS is 25 or 33 MHz */
    else
    {
        /* Do nothing */
    }

    /* CLMAS register write-protect ON */
    LOCK_REG_PROT(CLMASICUMKCPROT1);
#endif
    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_CLK_0004]:[ICUMFW_UD_U2B_D_CLK_0004]
* Function name: D_CLK_StopCLMA
* Description  : Disable CLMAS
* Arguments    : None
* Return Value : ER_OK
***************************************************************************************/
uint32_t D_CLK_StopCLMA(void)
{
    #if 0
    /** CLMA reset process **/
    /* CLMAS register write-protect OFF */
    UNLOCK_REG_PROT(CLMASICUMKCPROT1);
    /* Self-test ON                     */
    CLMASTEST |= CLK_CLMATEST_ENABLED;
    /* Mask the error notification      */
    CLMASTEST |= CLK_CLMATEST_MSKERR_ON;
    /* Signal to disable CLMAS sent     */
    CLMASTEST |= CLK_CLMATEST_RESCLM_SET;
    /* Wait for 2 cycles                */ 
    NOPS(2U);   
    /* No signal to disable CLMAS       */                                                                 
    CLMASTEST &= ~CLK_CLMATEST_RESCLM_SET;
    /* Remove the mask                  */
    CLMASTEST &= ~CLK_CLMATEST_MSKERR_ON;
    /* Self-test OFF                    */
    CLMASTEST &= ~CLK_CLMATEST_ENABLED;

    /* Dummy read & Synchronize pipeline to wait for the completion of */
    /* writing to the CLMASTEST Register */
    CLMASTEST;
    SYNC_PIPELINE();

    /* CLMAS register write-protect ON  */
    LOCK_REG_PROT(CLMASICUMKCPROT1);
    #endif
    return ER_OK;
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)
/* RAM statement */

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_CLK_0005]:[ICUMFW_UD_U2B_D_CLK_0005]
* Function name: D_CLK_StartCLMA
* Description  : CLMA Initialization
* Arguments    : icum_clma_cb_regist_t *p_regist_info
* Return Value : ER_OK
***************************************************************************************/
void INTICUCLMA(void)
{
    /* Check if the p_cbfunc is NULL */
    if (NULL != clma_cb_regist.p_cbfunc)
    {
        /* Call the registered callback function */
        (*clma_cb_regist.p_cbfunc)();
    }
    else
    {
        /* Do nothing */
    }
    
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_U2B_D_CLK_0006]:[ICUMFW_UD_U2B_D_CLK_0006]
* Function name: D_CLK_StartCLMA
* Description  : CLMA Initialization
* Arguments    : icum_clma_cb_regist_t *p_regist_info
* Return Value : ER_OK
***************************************************************************************/
/* Store in RAM statement */
CHANGE_SEC(text, ".icum_driver_ram")
CHANGE_SEC(rodata, ".icum_driver_const_ram")

void INTICUCLMA_RAM(void)
{
    /* Check if the p_cbfunc is NULL */
    if (NULL != clma_cb_regist.p_cbfunc)
    {
        /* Call the registered callback function */
        (*clma_cb_regist.p_cbfunc)();
    }
    else
    {
        /* Do nothing */
    }
}

CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)
/* Store in RAM statement ends*/
