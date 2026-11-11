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

/*===========================================================================*/
/* Enumerates: Timer ID                                                      */
/*===========================================================================*/
typedef enum {
    VRUN_TIMER_ID_PE1 = 0,
    VRUN_TIMER_ID_PE2,
    VRUN_TIMER_ID_PE3,
    VRUN_TIMER_ID_PE4,
    VRUN_TIMER_ID_PE5,
    VRUN_TIMER_ID_PE6,
    VRUN_TIMER_ID_NUM
} timer_id_t;

// ------------------------------------------------------------
// vrun user-level services
// ------------------------------------------------------------

void  vrun_console_get(void);
void  vrun_console_put(void);

void vrun_set_external_iobuff(char *ptr_external_iobuff, unsigned long buffsize);

void  vrun_iobuff_clear(void);
char *vrun_iobuff_readparam(unsigned char param_number);
void  vrun_iobuff_print_txt(const char *toprint);
void  vrun_iobuff_print_mem(const unsigned char *pmem, unsigned int nb_bytes);
unsigned long  vrun_iobuff_print_chrono(void);

void  vrun_chrono_start(void);
void  vrun_chrono_stop(void);
unsigned long  vrun_chrono_gettimestamp(void);

void  vrun_chrono_start_id(timer_id_t id);
void  vrun_chrono_stop_id(timer_id_t id);
unsigned long  vrun_chrono_gettimestamp_id(timer_id_t id);

unsigned long  vrun_chrono_getsec_id(timer_id_t id);
unsigned short vrun_chrono_getmsec_id(timer_id_t id);
unsigned short vrun_chrono_getusec_id(timer_id_t id);
unsigned short vrun_chrono_getnsec_id(timer_id_t id);

void  vrun_set_signal_high(unsigned char n);
void  vrun_set_signal_low(unsigned char n);

void  vrun_callback_set_func(void (*pfunc)(),unsigned long timing_ms);

void vrun_init(void);
void vrun_interpreter(void);

// ------------------------------------------------------------
// macro to redefine access to printing functions
// ------------------------------------------------------------
#define PTXT(X)         vrun_iobuff_print_txt(X)
#define PFORMAT(...)    vrun_iobuff_print_format(__VA_ARGS__)
#define PMEM(X, N)      vrun_iobuff_print_mem(X, N)
#define PCHRONO()       vrun_iobuff_print_chrono()
#define NL()            vrun_iobuff_print_txt("\r\n");
#define FLUSH()         vrun_console_put()

// ------------------------------------------------------------
// macro to declare the user commands
// ------------------------------------------------------------
// note: the following declaration fails to compile with
//       GHS v4.2.3 :
//           (void (*)(void))vrun_cmd_usr_1 = NULL;
//       the compiler does not return any error message...
//       it is just blocked for ever...
// ------------------------------------------------------------
#define VRUN_USR_CMD(n, func) \
    const char CMD_USR_##n[] = func; \
    void vrun_cmd_usr_##n(void)

#define VRUN_USR_CMD_UNUSED(n) \
    const char CMD_USR_##n[] = ""; \
    void vrun_cmd_usr_##n(void) {}


//    (void (*)(void))vrun_cmd_usr_##n## = NULL;

// ------------------------------------------------------------
// over ridding printf!
// ------------------------------------------------------------
#define vrun_iobuff_print_format(...) printf(__VA_ARGS__)
#define printf(...)  {sprintf(vrun_printf_buff, __VA_ARGS__); vrun_iobuff_print_txt(vrun_printf_buff); FLUSH();}

// ------------------------------------------------------------
// verbose flag (turned on/off by the verbose command)
// ------------------------------------------------------------
extern char vrun_printf_buff[];

// ------------------------------------------------------------
// chronometer values
// ------------------------------------------------------------
extern unsigned short vrun_chrono_sec;
extern unsigned short vrun_chrono_millisec;
extern unsigned short vrun_chrono_microsec;
extern unsigned short vrun_chrono_nanosec;

