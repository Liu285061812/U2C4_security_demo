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
#include <stdlib.h>
#include <ctype.h>

#undef NULL
#include "r_icumif_api.h"
#include "r_icumif_pub.h"

#include "pe1.h"
#include "shared.h"

#include "vrun.api.h"
#include "r_aceu.h"

/**********************************************************************************
* vrun commands
***********************************************************************************/

VRUN_USR_CMD_UNUSED(3)
VRUN_USR_CMD_UNUSED(5)
VRUN_USR_CMD_UNUSED(6)
VRUN_USR_CMD_UNUSED(7)
VRUN_USR_CMD_UNUSED(8)
VRUN_USR_CMD_UNUSED(9)
VRUN_USR_CMD_UNUSED(10)

static void run_aceu_command(void)
{
    char *end;
    const char *argument = vrun_iobuff_readparam(1);
    long selection;
    if (argument == NULL)
    {
        printf("\nACEU: 0=all 1=ECB 2=CBC 3=CTR 4=CFB128 5=OFB 6=GCM\n"
               "      7=XTS 8=CCM 9=CMAC 10=GMAC 11=MP 12=PRNG (q=quit)\n?> ");
        vrun_console_get();
        argument = vrun_iobuff_readparam(0);
        if (argument != NULL && strcmp(argument, "q") == 0) return;
    }
    if (argument == NULL || vrun_iobuff_readparam(2) != NULL)
    {
        printf("Usage: aceu [0..12] (alias: aecu)\n");
        return;
    }
    selection = strtol(argument, &end, 10);
    if (end == argument)
    {
        printf("ACEU invalid selection\n");
        return;
    }
    while (isspace((unsigned char)*end)) end++;
    if (*end != '\0' || selection < 0 || selection > 12)
    {
        printf("ACEU selection must be 0..12\n");
        return;
    }
    aceu_test_command((int)selection);
}

VRUN_USR_CMD(1, "aceu")
{
    run_aceu_command();
}

VRUN_USR_CMD(2, "aecu")
{
    run_aceu_command();
}

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

static void icum_service_category_menu(int category)
{
    int selection;
    r_icumif_service_id_t service_id;

    while (1)
    {
        switch (category)
        {
            case 1:
                printf("\n| service_01 AES\n");
                printf("| 1 : 0x10 AES cipher (ECB/CBC/CFB/OFB/CTR/XTS)\n");
                printf("| 2 : 0x11 AES authenticated cipher (GCM/CCM)\n");
                printf("| 3 : 0x12 AES CMAC\n");
                break;
            case 2:
                printf("\n| service_02 RSA\n");
                printf("| 1 : 0x20 RSA verify (PSS/PKCS1 v1.5)\n");
                printf("| 2 : 0x21 RSA sign (PSS/PKCS1 v1.5)\n");
                printf("| 3 : 0x22 RSA encrypt (OAEP/PKCS1 v1.5)\n");
                printf("| 4 : 0x23 RSA decrypt (OAEP/PKCS1 v1.5)\n");
                printf("| 5 : 0x24 RSA key generation (2048-bit)\n");
                break;
            case 3:
                printf("\n| service_03 random number\n");
                printf("| 1 : 0x30 initialize RNG\n");
                printf("| 2 : 0x31 extend seed\n");
                printf("| 3 : 0x32 generate random data\n");
                break;
            case 4:
                printf("\n| service_04 ECC/ECDSA/ECDH/EdDSA\n");
                printf("| 1 : 0x40 ECDSA verify\n");
                printf("| 2 : 0x41 ECDSA sign\n");
                printf("| 3 : 0x42 ECDH\n");
                printf("| 4 : 0x43 EdDSA verify\n");
                printf("| 5 : 0x44 EdDSA sign\n");
                printf("| 6 : 0x45 ECC key generation (P-256)\n");
                break;
            case 5:
                printf("\n| service_05 HASH/HMAC\n");
                printf("| 1 : 0x50 SHA-256 hash\n");
                printf("| 2 : 0x51 HMAC-SHA256 generation/verification\n");
                break;
            default:
                return;
        }

        printf("| 0 : Back\n| ?> ");
        vrun_console_get();
        if (*(char *)vrun_iobuff_readparam(0) == 'q')
        {
            return;
        }
        if (sscanf(vrun_iobuff_readparam(0), "%i", &selection) != 1)
        {
            printf("Invalid selection.\n");
            continue;
        }
        if (selection == 0)
        {
            return;
        }

        service_id = LAST_ITEM_SERV_ID;
        switch (category)
        {
            case 1:
                if (selection == 1) service_id = SERVICE_01_AES_CIPHER;
                else if (selection == 2) service_id = SERVICE_01_AES_AUTH_CIPHER;
                else if (selection == 3) service_id = SERVICE_01_AES_CMAC;
                break;
            case 2:
                if (selection == 1) service_id = SERVICE_02_RSA_VERIFY;
                else if (selection == 2) service_id = SERVICE_02_RSA_SIGN;
                else if (selection == 3) service_id = SERVICE_02_RSA_ENCRYPT;
                else if (selection == 4) service_id = SERVICE_02_RSA_DECRYPT;
                else if (selection == 5) service_id = SERVICE_02_RSA_KEY_GENERATE;
                break;
            case 3:
                if (selection == 1) service_id = SERVICE_03_RAND_INIT_RNG;
                else if (selection == 2) service_id = SERVICE_03_RAND_EXTEND_SEED;
                else if (selection == 3) service_id = SERVICE_03_RAND_RND;
                break;
            case 4:
                if (selection == 1) service_id = SERVICE_04_ECDSA_VERIFY;
                else if (selection == 2) service_id = SERVICE_04_ECDSA_SIGN;
                else if (selection == 3) service_id = SERVICE_04_ECDH;
                else if (selection == 4) service_id = SERVICE_04_EDDSA_VERIFY;
                else if (selection == 5) service_id = SERVICE_04_EDDSA_SIGN;
                else if (selection == 6) service_id = SERVICE_04_ECC_KEY_GENERATE;
                break;
            case 5:
                if (selection == 1) service_id = SERVICE_05_HASH;
                else if (selection == 2) service_id = SERVICE_05_HMAC;
                break;
            default:
                break;
        }
        if (service_id == LAST_ITEM_SERV_ID)
        {
            printf("Invalid selection.\n");
            continue;
        }
        icum_crypto_service_test_service(service_id);
    }
}

static void icum_service_test_menu(void)
{
    int category;

    while (1)
    {
        printf("\n| ICUM service test menu\n");
        printf("| 1 : service_01 AES\n");
        printf("| 2 : service_02 RSA\n");
        printf("| 3 : service_03 random number\n");
        printf("| 4 : service_04 ECC/ECDSA/ECDH/EdDSA\n");
        printf("| 5 : service_05 HASH/HMAC\n");
        printf("| 6 : Test all algorithms (0x10 -> 0x51)\n");
        printf("| 0 or q : Back\n| ?> ");
        vrun_console_get();
        if (*(char *)vrun_iobuff_readparam(0) == 'q')
        {
            return;
        }
        if (sscanf(vrun_iobuff_readparam(0), "%i", &category) != 1)
        {
            printf("Invalid selection.\n");
            continue;
        }
        if (category == 0)
        {
            return;
        }
        if (category == 6)
        {
            icum_crypto_service_test();
            continue;
        }
        if ((category < 1) || (category > 5))
        {
            printf("Invalid selection.\n");
            continue;
        }
        icum_service_category_menu(category);
    }
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
            printf("\n\n| ICUM service tests\n");
            printf("|-------------------\n");
            printf("| 1    : service_01 AES\n");
            printf("| 2    : service_02 RSA\n");
            printf("| 3    : service_03 random number\n");
            printf("| 4    : service_04 ECC/ECDSA/ECDH/EdDSA\n");
            printf("| 5    : service_05 HASH/HMAC\n");
            printf("| 6    : Test all algorithms (0x10 -> 0x51)\n");
            printf("| 308  : open categorized service menu\n");
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
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                icum_service_category_menu(n);
                break;
            case 308:
                icum_service_test_menu();
                break;
            case 6:
                icum_crypto_service_test();
                break;
            default:
                break;
        }
    }
}
