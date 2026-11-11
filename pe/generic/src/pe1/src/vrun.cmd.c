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

#include <stdio.h>
#include <string.h>

#undef NULL
#include "r_icumif_api.h"
#include "r_icumif_pub.h"

#include "pe1.h"
#include "shared.h"

#include "vrun.api.h"

/**********************************************************************************
* vrun commands
***********************************************************************************/

VRUN_USR_CMD_UNUSED(1)
VRUN_USR_CMD_UNUSED(2)
VRUN_USR_CMD_UNUSED(3)
VRUN_USR_CMD_UNUSED(5)
VRUN_USR_CMD_UNUSED(6)
VRUN_USR_CMD_UNUSED(7)
VRUN_USR_CMD_UNUSED(8)
VRUN_USR_CMD_UNUSED(9)
VRUN_USR_CMD_UNUSED(10)

/*===========================================================================
  Global variable
  ===========================================================================*/
unsigned short job_slice[NB_JOB_TYPE];

/*===========================================================================
  Life signal! (led management)
  ===========================================================================*/
/** LED blinking interval (millisec) */
static unsigned short led_freq;
/** LED number (0/1/2/3) */
static volatile unsigned char led_i;
/** LED blinking stop flag (stops other than 0) */
static volatile unsigned char stop_life_signal;
/** LED blinking direction (1:addition/-1:subtraction) */
static volatile int led_dir;

/*===========================================================================
  Static function prototype
  ===========================================================================*/
static void life_signal(void);

/**
 [Overview]\n
 Pause life signal

 [Description]\n
 Pauses the blinking of the LED used to confirm the existence of this sample 
 application.

 [Parameters]\n
 None

 [Return value]\n
 None

 [Input/Output variables]\n
 stop_life_signal
*/
void pause_life_signal(void)
{
    stop_life_signal = 1;
    while(stop_life_signal == 1);
}

/**
 [Overview]\n
 Restart life signal

 [Description]\n
 Restart the blinking of the LED used to confirm the survival of this sample 
 application.

 Since VRUN's periodic handler is used as the trigger for blinking, set the 
 callback function (life_signal) and interval (led_freq) for LED blinking with 
 the vrun_callback_set_func function.

 [Parameters]\n
 None

 [Return value]\n
 None

 [Input variables]\n
 led_freq

 [Output variables]\n
 stop_life_signal
*/
void restart_life_signal(void)
{
    stop_life_signal = 0;
    vrun_callback_set_func(&life_signal,led_freq);
}

/**
 [Overview]\n
 Life signal update callback

 [Description]\n
 Callback function for turning ON/OFF the life signal. Called by the VRUN 
 periodic handler.

 Turn the four life signals ON/OFF in the following order using the 
 vrun_set_signal_high and vrun_set_signal_low functions.\n
 1 -> 2 -> 3 -> 4 -> 3 -> 2 -> 1 (return to the beginning)\n
 * In this sample, the life signal is assigned to the LED on the development 
   board, so the LED blinks according to the state of the life signal. 

 [Parameters]\n
 None

 [Return value]\n
 None

 [Input/Output variables]\n
 stop_life_signal, led_i, led_dir
*/
static void life_signal(void)
{
    vrun_set_signal_low(led_i);
    led_i += led_dir;
    if(led_i == 255)
    {
        led_i = 1;
        led_dir = 1;
    }
    else if(led_i == 4)
    {
        led_i = 2;
        led_dir = -1;
    }
    vrun_set_signal_high(led_i);
    if(stop_life_signal)
    {
        vrun_set_signal_low(led_i);
        led_i = 0;
        led_dir = -1;
        stop_life_signal = 2;
    }
    else
    {
        /* restart_life_signal(); */
    }
}

/**
 [Overview]\n
 Initialize the VRUN descriptor

 [Description]\n
 Performs initialization processing to operate the VRUN descriptor.\n
  - Start updating the life signal\n
  - Display welcome message\n
  - Call the system_init function\n
  - Display system information

 [Parameters]\n
 None

 [Return value]\n
 None

 [Output variables]\n
 led_freq, led_i, led_dir
*/
void vrun_cmd_init(void)
{
    /* initialize the life signal */
    /* led_freq is the lighting time in millisec for each led */
    led_freq = 125;
    led_i = 0;
    led_dir = -1;
    restart_life_signal();

    /* system initialization */
    system_init();

    /* welcome message */
    PTXT("\n\n****************************************\n");
    PTXT("Testing ICU-M firmware\n");
    PTXT("****************************************\n\n");
    FLUSH();

    NL(); FLUSH();
}

/**
 [Overview]\n
 "icum" sub-menu

 [Description]\n
 "icum" sub-menu entry
 The following function names are expanded by the VRUN_USR_CMD macro.\n
    const char CMD_USR_4 [] = "icum";\n
    void vrun_cmd_usr_4 (void)\n
 The above function is called from the vrun module according to the selection 
 from the main menu.

 In this function, the submenu screen is displayed, and the target service is 
 displayed from the numerical value entered according to the screen.\n
 Call a function group that describes sample code that uses a command.

 [Parameters]\n
 None

 [Return value]\n
 None
*/
VRUN_USR_CMD(4, "icum")
{
    int n;
    int exit_loop = 0;

    while(!exit_loop)
    {
        if(vrun_iobuff_readparam(1) != 0)
        {
            /* 1st command parameter directly supplied after "icum" keyword */
            sscanf(vrun_iobuff_readparam(1), "%i", &n);
            exit_loop = 1;
        }
        else
        {
            printf("\n\n| ICU-M Hello!\n");
            printf("|----------------\n");
            printf("| 141  : AES plain key update\n");
            printf("| 171  : AES cipher 128bit key\n");
            printf("| 200  : AES Auth cipher\n");
            printf("| 243  : AES CMAC generation\n");
            printf("| 301  : ecc key generate\n");
            printf("| 302  : ecdsa sign\n");
            printf("| 303  : ecdsa verify\n");
            printf("| 304  : measure ecc\n");
            printf("| 305  : icum ecdh\n");
            printf("| 306  : icum ecc_key_pair import\n");
            printf("| 307  : icum test hash\n");
            printf("| q    : Quit\n\n");
            printf("  ?> ");
            vrun_console_get();
            if(*(char *)vrun_iobuff_readparam(0) == 'q')
            {
                n = -1;
                exit_loop = 1;
            }
            else
            {
                sscanf(vrun_iobuff_readparam(0), "%i", &n);
            }
        }
        switch(n)
        {
            case 141:
                icum_key_update_test();
                break;
            case 171:
                icum_aes_cipher_test();
                break;
            case 200:
                icum_test_aes_auth_cipher();
                break;
            case 243:
                icum_aes_cmac_test();
                break;
            case 301:
                icum_ecc_key_generate();
                break;
            case 302:
                icum_ecdsa_sign();
                break;
            case 303:
                show_submenu_for_ecdsa_verify();
                break;
            case 304:
                measure_ecc();
                break;
            case 305:
                icum_ecdh();
                break;
            case 306:
                icum_ecc_key_pair_import();
                break;
            case 307:
                icum_test_hash();
                break;
            default:
                break;
        }
    }
}
