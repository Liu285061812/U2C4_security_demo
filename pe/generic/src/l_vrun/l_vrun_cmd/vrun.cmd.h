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
// vrun commands
// ------------------------------------------------------------
extern void vrun_cmd_init(void);

extern void vrun_cmd_help(void);
extern void vrun_cmd_verbose(void);
extern void vrun_cmd_get_cpu_speed(void);

extern void vrun_cmd_usr_1(void);
extern void vrun_cmd_usr_2(void);
extern void vrun_cmd_usr_3(void);
extern void vrun_cmd_usr_4(void);
extern void vrun_cmd_usr_5(void);
extern void vrun_cmd_usr_6(void);
extern void vrun_cmd_usr_7(void);
extern void vrun_cmd_usr_8(void);
extern void vrun_cmd_usr_9(void);
extern void vrun_cmd_usr_10(void);

extern const char CMD_USR_1[];
extern const char CMD_USR_2[];
extern const char CMD_USR_3[];
extern const char CMD_USR_4[];
extern const char CMD_USR_5[];
extern const char CMD_USR_6[];
extern const char CMD_USR_7[];
extern const char CMD_USR_8[];
extern const char CMD_USR_9[];
extern const char CMD_USR_10[];

// ------------------------------------------------------------
// vrun command type
// ------------------------------------------------------------
typedef struct
{
    const char *verb;
    void (*pfunc)();
} vrun_command_hook_type;

// ------------------------------------------------------------
// vrun command table
// ------------------------------------------------------------

const vrun_command_hook_type vrun_command_hook[] = 
{
// standard command list
    "help", &vrun_cmd_help,
    "get_cpu_speed", &vrun_cmd_get_cpu_speed,

// custom command list
    CMD_USR_1,  &vrun_cmd_usr_1,
    CMD_USR_2,  &vrun_cmd_usr_2,
    CMD_USR_3,  &vrun_cmd_usr_3,
    CMD_USR_4,  &vrun_cmd_usr_4,
    CMD_USR_5,  &vrun_cmd_usr_5,
    CMD_USR_6,  &vrun_cmd_usr_6,
    CMD_USR_7,  &vrun_cmd_usr_7,
    CMD_USR_8,  &vrun_cmd_usr_8,
    CMD_USR_9,  &vrun_cmd_usr_9,
    CMD_USR_10, &vrun_cmd_usr_10
};

