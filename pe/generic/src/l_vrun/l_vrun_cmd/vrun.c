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
#pragma ghs reference vrun_get_package_info

#include <string.h>
#include <stdio.h>

#include "sys.api.h"
#include "vrun.config.h"
#include "vrun.cmd.h"
#include "vrun.api.h"

#define VRUN_CHAR_DEL           '\x08'
#define VRUN_CHAR_CR            '\x0D'
#define VRUN_CHAR_LF            '\x0A'
#define VRUN_CHAR_COMMA         '\x2C'

#define VRUN_STR_CRLF           "\x0D\x0A"  /* "^M^J" as sent by terminal */
#define VRUN_STR_CMD_DELIM      " \x0D\x0A"

/* vb_test */
#define VRUN_STR_VBTEST_CMD_SOFTRESET   "softreset\r\n"

#define VRUN_UART_CHANNEL        0

extern  void    proc_SOFTRESET(void);

/* vrun_cmd[] is the table that points to the command verb and the parameters */
static char *vrun_cmd[VRUN_MAX_NB_PARAM + 1];

/* vrun_iobuff[] is the input / output buffer for the interpreter */
static char vrun_iobuff[VRUN_IOBUFF_SIZE + sizeof(VRUN_STR_CRLF) + 1];

/* vrun_iobuff_idx is the character insertion point within the string vrun_iobuff[] */
static unsigned short vrun_iobuff_idx;

/* vrun_iobuff_external[] is an external input buffer used in place of vrun_iobuff[] */
/* if declared by the application, this buffer is filled with the characters received */
static char *vrun_iobuff_external;

/* vrun_size_iobuff_external the size of the external I/O buffer */
static unsigned long vrun_size_iobuff_external;

/* vrun_receiveflag is 1 when a byte has been received (update in irq function) */
/* NOTE: as the IRQ is now handled in the SYS module (HW dependent), this variable is now GLOBAL */
volatile char vrun_receiveflag;

/* vrun_sendflag is 1 when a byte has been sent (update in irq function) */
/* NOTE: as the IRQ is now handled in the SYS module (HW dependent), this variable is now GLOBAL */
volatile char vrun_sendflag;

/* the below variables are updated after a call to vrun_iobuff_print_chrono */
/* vrun_chrono_sec = seconds, vrun_chrono_millisec = milliseconds, vrun_chrono_microsec = microseconds */
unsigned short vrun_chrono_sec;
unsigned short vrun_chrono_millisec;
unsigned short vrun_chrono_microsec;
unsigned short vrun_chrono_nanosec;


unsigned long  vrun_stime;
unsigned long  vrun_etime;

unsigned long  vrun_chrono_sec_id[VRUN_TIMER_ID_NUM];
unsigned short vrun_chrono_millisec_id[VRUN_TIMER_ID_NUM];
unsigned short vrun_chrono_microsec_id[VRUN_TIMER_ID_NUM];
unsigned short vrun_chrono_nanosec_id[VRUN_TIMER_ID_NUM];
unsigned long  vrun_stime_id[VRUN_TIMER_ID_NUM];
unsigned long  vrun_etime_id[VRUN_TIMER_ID_NUM];


/* the below buffer is used for printf (PFORMAT) function */
char vrun_printf_buff[128];

/*===========================================================================
  function: vrun_console_get
  [PUBLIC]
  - gets a string from the console to be interpreted
  - returns with vrun_iobuff set with the string received
  ===========================================================================*/
void vrun_console_get()
{
    char *ptrbuff;
    register unsigned long buffsize;
    int i;
    char receivedbyte;

    /* determine where to store the bytes recevied */
    if(vrun_iobuff_external == NULL)
    {
        ptrbuff = vrun_iobuff;
        buffsize = VRUN_IOBUFF_SIZE;
    }
    else
    {
        ptrbuff = vrun_iobuff_external;
        buffsize = vrun_size_iobuff_external;
    }

    /* clear the i/o buffer */
    ptrbuff[0] = 0;
    i = 0;
    do
    {
        /* wait for the interrupt to set the receive flag */
        while(!vrun_receiveflag);
        /* clear the receive flag */
        vrun_receiveflag = 0;
        /* get the character received */
        receivedbyte = sys_vuart_getbyte();
        /* test for the del key */
        if(receivedbyte == VRUN_CHAR_DEL)
        {
            if(i > 0)
            {
                i -= 2;
            }
        }
        else
        {
            /* concatenate the character received */
            ptrbuff[i] = receivedbyte;
        }

        /* still enough space to store the bytes? */
        if((receivedbyte == VRUN_CHAR_CR) || (receivedbyte == VRUN_CHAR_LF) || (receivedbyte == VRUN_CHAR_COMMA))
        {
            /* there should be enough space to store CRLF in i/o buffer */
            /* increment the counter on the i/o buffer */
            i++;
        }
        else if(i < buffsize)
        {
            /* increment the counter on the i/o buffer */
            i++;
        }
    }
    while(receivedbyte != VRUN_CHAR_LF);
    /* terminate the string by chr(0) */
    ptrbuff[i] = 0;

    /* vb_test */
    if(strncmp(ptrbuff, VRUN_STR_VBTEST_CMD_SOFTRESET, 11) == 0) 
    { 
        proc_SOFTRESET(); 
    } 
}

/*===========================================================================
  function: vrun_set_external_iobuff
  [PUBLIC]
  - declares an alternative buffer to vrun_iobuff[] in case of buffer extbuff
  ===========================================================================*/
void vrun_set_external_iobuff(char *ptr_external_iobuff, unsigned long buffsize)
{
    vrun_iobuff_external = ptr_external_iobuff;
    vrun_size_iobuff_external = buffsize;
}

/*===========================================================================
  function: vrun_console_put
  [PUBLIC]
  - outputs the string vrun_iobuff to the console
  ===========================================================================*/
void vrun_console_put()
{
    int i;
    int timeout_counter;

    /* clear the send flag */
    vrun_sendflag = 0;
    
    /* parse all the characters to send */
    for(i = 0; i < vrun_iobuff_idx; i++)
    {
        timeout_counter = 1000000L;
        /* send next byte from the i/o buffer */
        sys_vuart_sendbyte(vrun_iobuff[i]);
        /* wait for the interrupt to set the send flag */
        while((!vrun_sendflag) && (--timeout_counter));
        /* cannot send? */
        if(!timeout_counter)
        {
            /* exit the for loop here */
            break;
        }
        /* clear the send flag */
        vrun_sendflag = 0;
    }

    /* clear the insertion point in the I/O buffer */
    vrun_iobuff_idx = 0;
}

/*===========================================================================
  function: vrun_iobuff_clear
  [PUBLIC]
  - clears vrun_iobuff
  ===========================================================================*/
void vrun_iobuff_clear()
{
    /* I/O buffer now contains an empty string */
    vrun_iobuff[0]  = 0;
    vrun_iobuff_idx = 0;
}

/*===========================================================================
  function: vrun_iobuff_print_txt
  [PUBLIC]
  - concatenate a text string into vrun_iobuff
  ===========================================================================*/
void vrun_iobuff_print_txt(const char *toprint)
{
    unsigned short n;
    
    /* concatenate in the I/O buffer the string to print */
    n = strlen(toprint);
    if((vrun_iobuff_idx + n) > VRUN_IOBUFF_SIZE)
    {
        /* first output what's in the I/O buffer */
        vrun_console_put();
    }
    if(n > VRUN_IOBUFF_SIZE)
    {
        /* need to split in several substring to print */
        int i;
        for(i = 0; i < n; i += VRUN_IOBUFF_SIZE)
        {
            if((i + VRUN_IOBUFF_SIZE) > n)
            {
                vrun_iobuff_print_txt(&toprint[i]);
            }
            else
            {
                memcpy(&vrun_iobuff[vrun_iobuff_idx], &toprint[i], VRUN_IOBUFF_SIZE);
                vrun_iobuff_idx += VRUN_IOBUFF_SIZE;
                vrun_console_put();
            }
        }
    }
    else
    {
        /* do the concatenation of the string to print */
        sprintf(&vrun_iobuff[vrun_iobuff_idx], toprint);
        vrun_iobuff_idx += n;
    }
}

/*===========================================================================
  function: vrun_iobuff_print_mem
  [PUBLIC]
  - concatenate a memory dump into vrun_iobuff
  - no memory limit
  ===========================================================================*/
void vrun_iobuff_print_mem(const unsigned char *pmem, unsigned int nb_bytes)
{
    char strbyte[3];

    while(nb_bytes > 0)
    {
        /* format the output string */
        sprintf(strbyte, "%02X", *(pmem++));
        /* concatenate in the i/o buffer */
        vrun_iobuff_print_txt(strbyte);
        /* decrement the number of bytes to send */
        nb_bytes--;
    }
}

/*===========================================================================
  function: vrun_iobuff_readparam
  [PUBLIC]
  - returns a pointer to the parameter string identified by param_number
  ===========================================================================*/
char *vrun_iobuff_readparam(unsigned char param_number)
{
    return(vrun_cmd[param_number]);
}

/*===========================================================================
  function: vrun_iobuff_print_chrono
  [PUBLIC]
  - output the chrono to the iobuffer
  - format: "---s ---ms ---us" => 16 characters max (not counting \0 at end)
  - returns the (raw) 32-bit counter value retrieved from the low-level driver
  ===========================================================================*/
unsigned long vrun_iobuff_print_chrono(void)
{
    unsigned long cnt;
    char temp[32];

    /* get elapsed time */
    cnt = vrun_chrono_gettimestamp();
    
    /* output to the I/O buffer */
    if((!vrun_chrono_sec) && (!vrun_chrono_millisec))
    {
        sprintf(temp, "%dus %dns", vrun_chrono_microsec, vrun_chrono_nanosec);
    }
    else if(!vrun_chrono_sec)
    {
        sprintf(temp, "%dms %03dus", vrun_chrono_millisec, vrun_chrono_microsec);
    }
    else
    {
        sprintf(temp, "%ds %03dms %03dus", vrun_chrono_sec, vrun_chrono_millisec, vrun_chrono_microsec);
    }
    vrun_iobuff_print_txt((const char *)temp);

    return(cnt);
}

/*===========================================================================
  function: vrun_chrono_start
  [PUBLIC]
  - starts the chrono
  ===========================================================================*/
void vrun_chrono_start()
{
    vrun_stime = sys_vtimer0_get_pclk_counter();
    vrun_etime = vrun_stime;
}

/*===========================================================================
  function: vrun_chrono_stop
  [PUBLIC]
  - stops the chrono
  ===========================================================================*/
void vrun_chrono_stop()
{
    vrun_etime = sys_vtimer0_get_pclk_counter();
}

/*===========================================================================
  function: vrun_chrono_gettimestamp
  [PUBLIC]
  get elapsed time in variables vrun_chrono_sec, vrun_chrono_millisec, vrun_chrono_microsec
  ===========================================================================*/
unsigned long vrun_chrono_gettimestamp()
{
    unsigned long  vrun_chrono_row_us;
    time_t time;

    vrun_chrono_row_us   = sys_vtimer0_cnv_pclk2time((unsigned long)(vrun_etime - vrun_stime),&time);
    vrun_chrono_sec      = time.sec;
    vrun_chrono_millisec = time.msec;
    vrun_chrono_microsec = time.usec;
    vrun_chrono_nanosec  = time.nsec;

    return(vrun_chrono_row_us);
}

/*===========================================================================
  function: vrun_callback_set_func
  [PUBLIC]
  - Regist Periodic operation callback function entry adderss
  ===========================================================================*/
void vrun_callback_set_func(void (*pfunc)(),unsigned long timing_ms)
{
    sys_vtimer1_set_func(pfunc,timing_ms);
}

/*===========================================================================
function: vrun_vrimer1_start
  [PUBLIC]
  - start system timer for Periodic operation callback function
 ===========================================================================*/
void vrun_vrimer1_start(void)
{
    sys_vtimer1_start();
}

/*===========================================================================
  function: vrun_set_signal_high
  [PUBLIC]
  - sets the signal n (n = 0,1,2 or 3) to high (1)
  ===========================================================================*/
void vrun_set_signal_high(unsigned char n)
{
    /* n should be between 0 and 3 */
    n &= 0x03;
    /* sets the port level to high */
    sys_vport_set_high(n);
}

/*===========================================================================
  function: vrun_set_signal_low
  [PUBLIC]
  - sets the signal n (n = 0,1,2 or 3) to low (0)
  ===========================================================================*/
void vrun_set_signal_low(unsigned char n)
{
    /* n should be between 0 and 3 */
    n &= 0x03;
    /* sets the port level to low */
    sys_vport_set_low(n);
}

/*===========================================================================
  function: vrun_init
  [PRIVATE]
  - initializes the interpreter
  - should be called once before calling vrun_interpreter
  ===========================================================================*/
void vrun_init()
{
    /* clear the send and receive flags */
    vrun_receiveflag = 0;
    vrun_sendflag = 0;
    
    /* clear the I/O buffer */
    vrun_iobuff_clear();
    
    /* configure the UART */
    sys_vuart_config();
    
    /* clear the address of external buffer */
    vrun_iobuff_external = NULL;

    /* Timer start */
    sys_vtimer1_start();
    sys_vtimer0_start();
    
    /* call the user-defined initialization function */
    vrun_cmd_init();
    
    vrun_iobuff_print_txt("vrun_cmd_init end \n");

}

/*===========================================================================
  function: vrun_interpreter
  [PRIVATE]
  - runs the command interpretation i.e.
    - gets a command string from the console
    - retrieve the command verb and the parameters
    - parse the command table to find the function to call
  ===========================================================================*/
void vrun_interpreter()
{
    int i;
    char *ptrbuff;
    
    while(1)
    {
        /* determine where to read the bytes recevied */
        if(vrun_iobuff_external == NULL)
        {
            ptrbuff = vrun_iobuff;
        }
        else
        {
            ptrbuff = vrun_iobuff_external;
        }

        /* loop on null string command */
        do
        {
            /* invite */
            vrun_iobuff_print_txt("vrun> ");
            vrun_console_put();
            /* read next command */
            vrun_console_get();
        }
        while(*ptrbuff == VRUN_CHAR_CR);
        
        /* retrieve the command verb */
        vrun_cmd[0] = strtok(ptrbuff, VRUN_STR_CMD_DELIM);
        
        /* scan for the command parameters */
        for(i = 1; i < (sizeof(vrun_cmd) / sizeof(vrun_cmd[0])); i++)
        {
            vrun_cmd[i] = strtok(NULL, VRUN_STR_CMD_DELIM);
        }
        
        /* scan the table of function hooks to find the command word */
        for(i = 0; i < (sizeof(vrun_command_hook) / sizeof(vrun_command_hook_type)); i++)
        {
            if(strcmp(vrun_cmd[0], vrun_command_hook[i].verb) == 0)
            {
                /* found known command word */
                /* call the associated function */
                vrun_command_hook[i].pfunc();
    
                /* exit the for loop here */
                /* set i to -1 to indicate that the command was found in the hook table */
                i = -1;
                break;
            }
        }
    
        /* if no command processed, terminate with an error */
        if(i != -1)
        {
            /* error: unknown command */
            vrun_iobuff_print_txt(vrun_cmd[0]);
            vrun_iobuff_print_txt(": unknown command" VRUN_STR_CRLF);
            vrun_console_put();
        }
    }
}

/*===========================================================================
  function: vrun_cmd_help
  [PUBLIC]
  - lists all commands supported
  - output the list to the console
  ===========================================================================*/
void vrun_cmd_help(void)
{
    int i;
    
    vrun_iobuff_clear();
    vrun_iobuff_print_txt("\r\n\r\nCommand catalog: \r\n");
    
    /* scan the table of function hooks to retrieve each command word */
    for(i = 0; i < (sizeof(vrun_command_hook) / sizeof(vrun_command_hook_type)); i++)
    {
        if(*vrun_command_hook[i].verb)
        {
            vrun_iobuff_print_txt(" : ");
            vrun_iobuff_print_txt(vrun_command_hook[i].verb);
            vrun_iobuff_print_txt("\r\n");
        }
    }
    vrun_iobuff_print_txt("\r\n");
    vrun_console_put();
}

/*===========================================================================
  function: vrun_cmd_get_cpu_speed
  [PUBLIC]
  - benchmark CPU speed
  ===========================================================================*/
void vrun_cmd_get_cpu_speed(void)
{
    char  strtemp[32];
    float freq;
    
    freq  = (float)sys_clock_quartzfreq();
    freq /= 1000000;
    vrun_iobuff_print_txt("\r\nsystem configured to run on quartz frequency of: ");
    sprintf(strtemp, "%3.3f MHz\r\n", freq);
    vrun_iobuff_print_txt((const char *)strtemp);

    vrun_iobuff_print_txt("\r\nbenchmarking CPU clock frequency...");
    vrun_iobuff_print_txt("\r\napprox. 900,000 CPU clocks executed in: ");
    
    __DI();
    vrun_set_signal_high(0);
    vrun_chrono_start();
    BENCH_CPU_SPEED(900000*2);
    vrun_chrono_stop();
    vrun_set_signal_low(0);
    __EI();

    vrun_iobuff_print_chrono();
    vrun_iobuff_print_txt("\r\napprox. CPU frequency: ");

    freq = 900000 / (float)((vrun_chrono_millisec * 1000) + vrun_chrono_microsec);
    sprintf(strtemp, "%f", freq);
    vrun_iobuff_print_txt((const char *)strtemp);

    vrun_iobuff_print_txt(" MHz\r\n");

    vrun_iobuff_print_txt("\r\n");
    vrun_console_put();
}

/*===========================================================================
  function: vrun_chrono_start_id
  [PUBLIC]
  - starts the chrono
  ===========================================================================*/
void vrun_chrono_start_id(timer_id_t id)
{
    vrun_stime_id[id] = sys_vtimer0_get_pclk_counter();
    vrun_etime_id[id] = vrun_stime_id[id];
}

/*===========================================================================
  function: vrun_chrono_stop_id
  [PUBLIC]
  - stops the chrono
  ===========================================================================*/
void vrun_chrono_stop_id(timer_id_t id)
{
    vrun_etime_id[id] = sys_vtimer0_get_pclk_counter();
}

/*===========================================================================
  function: vrun_chrono_gettimestamp_id
  [PUBLIC]
  get elapsed time in variables vrun_chrono_sec, vrun_chrono_millisec, vrun_chrono_microsec
  ===========================================================================*/
unsigned long vrun_chrono_gettimestamp_id(timer_id_t id)
{
    unsigned long  vrun_chrono_row_us;
    time_t time;

    if ( VRUN_TIMER_ID_PE1 == id ) {
        vrun_chrono_row_us          = sys_vtimer0_cnv_pclk2time((unsigned long)(vrun_etime_id[id] - vrun_stime_id[id]), &time);
    }
    else {
        vrun_chrono_row_us          = sys_vtimer0_cnv_pclk2time_pe2((unsigned long)(vrun_etime_id[id] - vrun_stime_id[id]), &time);
    }
    vrun_chrono_sec_id[id]      = time.sec;
    vrun_chrono_millisec_id[id] = time.msec;
    vrun_chrono_microsec_id[id] = time.usec;
    vrun_chrono_nanosec_id[id]  = time.nsec;

    return(vrun_chrono_row_us);
}

unsigned long  vrun_chrono_getsec_id(timer_id_t id)
{
    return vrun_chrono_sec_id[id];
}

unsigned short vrun_chrono_getmsec_id(timer_id_t id)
{
    return vrun_chrono_millisec_id[id];
}

unsigned short vrun_chrono_getusec_id(timer_id_t id)
{
    return vrun_chrono_microsec_id[id];
}

unsigned short vrun_chrono_getnsec_id(timer_id_t id)
{
    return vrun_chrono_nanosec_id[id];
}
