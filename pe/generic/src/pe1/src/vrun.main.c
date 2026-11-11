/******************************************************************************/
/* Component Name   Testing the ICU-M                                         */
/******************************************************************************/
/* Product      : ICU-M Firmware                                              */
/******************************************************************************/
/* Copyright(C) 2021-2022 Renesas Electronics Corporation.                    */
/******************************************************************************/

/**********************************************************************************
* Include
***********************************************************************************/

#include "sys.api.h"
#include "vrun.api.h"

#include "pe1.h"
#include "pe_target_device.h"
#include "shared.h"
#include "icum_d_comm_pe_pub.h"
#include "pe_reg.h"

/**
 [Overview]\n
 Wait for ICU-M firmware to be ready

 [Description]\n
 Monitor the value of the ICUM_ICU2PES status register and wait until the 
 ICU-M is ready to start.

 In the ICU-M firmware, the most significant bit (31st bit) of the ICUM_ICU2PES 
 status register is used as a flag to notify the completion of boot preparation.

 [Parameters]\n
 None

 [Return value]\n
 None

 [Used Registers]\n
 ICUM_ICU2PES
*/
#ifdef FW_UPDATE_USED
#pragma ghs section text=".pe_ram"
#else
#pragma ghs section text=".pe_rom"
#endif
static void wait_for_icu(void)
{
    /* Fake Service_id because SERVICE_03_GENERATE_M1_3 not implemented, */
    /* wait until the ICU-M system initialization is complete */
    do
    {
        __asm("snooze");
    } while((REG_PE_ICU2PES0 & 0x80000000UL) == 0);
}
#pragma ghs section text=default

/**
 [Overview]\n
 Clear the GRAM area to zero
*/
static void clear_gram(void)
{
    uint64_t *p_data;
    uint32_t  size;

    p_data = (uint64_t *)ADDR_LOWER_CRAM;
    size = ((uint32_t)ADDR_UPPER_CRAM + 1U) - (uint32_t)ADDR_LOWER_CRAM;
    while(size) {*(p_data++) = 0; size -= 8;}

}

/**
 [Overview]\n
 Main function of sample application hello_world

 [Description]\n
 The main function of this sample application hallo_world.\n
 Performs PE initialization and ICU-M initialization processing required to 
 operate the sample application, and transitions to the main menu after the 
 initialization is completed. 

 [Parameters]\n
 None

 [Return value]\n
 None

 [Output variables]\n
 SYNC_PE
*/
int main(void)
{
    /* enable interrupts*/
    __EI();

    /* wait for the ICU to start-up */
    wait_for_icu();
    
    clear_gram();

    /* setup system clock */
    sys_clock_maxfreq();

    /* stop the watchdog */
    sys_watchdog_stop();

    /* setup the ports */
    sys_vport_config();

    /* initialization */
    vrun_init();

    /* sync with next PE */
    SYNC_PE = GET_MY_PEID + 1;

    /* use a big buffer for the UART */
    vrun_set_external_iobuff((char *)COM_BUFFER, SIZE_OF_COM_BUFFER);

    /* start here */
    vrun_interpreter();

    /* endless loop after exiting the interpreter loop */
    while(1);
}
