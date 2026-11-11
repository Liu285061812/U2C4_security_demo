/******************************************************************************/
/* Component Name   Testing the ICU-M                                         */
/******************************************************************************/
/* Product      : ICU-M Firmware                                              */
/******************************************************************************/
/*******************************************************************************
* Copyright(C) 2021-2022 Renesas Electronics Corporation.
*
*          RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
*
* These instructions, statements, and programs are the confidential information
* of Renesas Electronics Corporation. They must be used and modified solely for
* the purpose for which it was furnished by Renesas Electronics Corporation.
* All part of them must not be reproduced nor disclosed to others in any form,
* without the prior written permission of Renesas Electronics Corporation.
*******************************************************************************/

// ********************************************************************************
// Headers
// ********************************************************************************

#include <stdio.h>
#include <string.h>

#undef NULL
#include "r_icumif_api.h"
#include "r_icumif_pub.h"
#include "sys.api.h"

#include "pe1.h"
#include "pe1.int.h"
#include "shared.h"

#include "vrun.api.h"
#include "pe_target_device.h"
#include "pe_reg.h"
#include "icum_d_comm_pe.h"
#include "icum_d_comm_pe_pub.h"
#include "icum_d_comm_pe.h"

#include "renesas_types.h"

/* vb_test */
extern char *DivideCommand(char *commandP, unsigned int unitNo, unsigned int *len);
extern void ConvertStrToArrayAddr(unsigned int *dst, char *src, unsigned int len);


/********************************************************************
* function name : faci_data_write
* input         : void
* output        : void
* note          : need to enter, start address, and data size
********************************************************************/
void faci_data_write()
{
    int size;
    int write_data;
    int i;
    uint32_t wdata;
    uint32_t addr;
    uint32_t select_addr = 0;
    uint32_t addr_temp;
    uint32_t exit_loop = 0;

    write_data = 0;

    while(!exit_loop){

        // Select Write Area
        printf("\n| Please Select Write Start Area\n");
        printf("|---------------------------------\n");
        printf("| Data Flash : 0xFF200000-0xFF22FFFF\n");
        printf("  ?> ");

        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &select_addr);

        if( (select_addr >= 0xFF200000) && (select_addr <= 0xFF22FFFF) )
        {
            exit_loop = 1;
        }
        else
        {
            printf("Select Error!!\n\n");
        }
    }
    addr = select_addr;
    addr_temp = select_addr;

    exit_loop = 0;

    while(!exit_loop){

        // Write Byte Size
        printf("\n\n| Please Select Write Size\n");
        printf("|---------------------------------\n");
        printf("|Range:1-196608 \n");
        printf("  ?> ");

        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &size);

        if( (size >= 1) && (size <= 196608))
        {
            exit_loop = 1;
        }
        else
        {
            printf("Parameter Error!!\n\n");
        }
    }
    // target area erase
    faci_block_erase(addr,size);

    while (write_data < size)
    {
        // initialize write data
        wdata = 0;

        // split write data in bytes
        for (i = 0; i < DF_WRITE_SIZE; i++)
        {
            if (write_data + i <= size)
            {
                // create write data
                wdata = wdata + (BIG_BUFFER[0][write_data + i] << (i*8));
            }
            else
            {
                // if the index exeeeds size, skip process
            }
        }
        // data write
        faci_programing_data_flash(addr_temp, wdata);
        // write_data index increment
        write_data = write_data + DF_WRITE_SIZE;
        // write address increment
        addr_temp = addr_temp + DF_WRITE_SIZE;
    }

    return;
}
/********************************************************************
* function name : faci_data_erase
* input         : uint32_t addr
*                 uint32_t size
* output        : void
********************************************************************/
void faci_block_erase(uint32_t addr,uint32_t size)
{

    // Change to data flash programming/erasure mode
    REG_PE_FACIFENTRYR = 0xAA80;
    while( REG_PE_FACIFENTRYR != 0x0080 ){};
    while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};

    // Programming/erasure/CodeFlashBlankChecking enabled
    REG_PE_FACIFHVE3  = 0x01;
    REG_PE_FACIFHVE15 = 0x01;

    // issue CF block erase command
    REG_PE_FACIFSADDR  = addr;
    REG_PE_FACICMD = 0x20;
    REG_PE_FACICMD = 0xD0;

    // wait for e/o blank check operation
    while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};

    // check for errors
    if( (REG_PE_FACIFASTAT&0x10/*CMDLK*/) || (REG_PE_FACIFSTATR&(0x00004000/*ILGLERR*/|0x00002000/*ERSERR*/|0x00001000/*PRGERR*/)) )
    {
        printf("\n");
        printf("!!  Error caused in issuing DF program command  !!\n");
        goto end;
    }

end:

    // Programming/erasure/DataFlashBlankChecking disabled
    REG_PE_FACIFHVE3  = 0x00;
    REG_PE_FACIFHVE15 = 0x00;

    // Change to Code flash read mode
    REG_PE_FACIFENTRYR = 0xAA00;
    while( REG_PE_FACIFENTRYR!= 0x0000 ){};

    printf( "\n" );
}

/********************************************************************
* function name : faci_programing_data_flash
* input         : uint32_t addr
*                 uint32_t wdata
* output        : void
********************************************************************/
void faci_programing_data_flash(uint32_t addr,uint32_t wdata)
{

    // Change to data flash programming/erasure mode
    REG_PE_FACIFENTRYR = 0xAA80;
    while( REG_PE_FACIFENTRYR != 0x0080 ){};
    while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};

    // Programming/erasure/CodeFlashBlankChecking enabled
    REG_PE_FACIFHVE3  = 0x01;
    REG_PE_FACIFHVE15 = 0x01;

    // issue DF program command
    REG_PE_FACIFSADDR  = addr;
    REG_PE_FACICMD = 0xE8;
    REG_PE_FACICMD = 0x01;
    REG_PE_FACICMDDT = wdata;
    REG_PE_FACICMD = 0xD0;

    // wait for e/o blank check operation
    while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};

    // check for errors
    if( (REG_PE_FACIFASTAT&0x10/*CMDLK*/) || (REG_PE_FACIFSTATR&(0x00004000/*ILGLERR*/|0x00002000/*ERSERR*/|0x00001000/*PRGERR*/)) )
    {
        printf("\n");
        printf("!!  Error caused in issuing DF program command  !!\n");
        goto end;
    }

end:

    // Programming/erasure/DataFlashBlankChecking disabled
    REG_PE_FACIFHVE3  = 0x00;
    REG_PE_FACIFHVE15 = 0x00;

    // Change to Code flash read mode
    REG_PE_FACIFENTRYR = 0xAA00;
    while( REG_PE_FACIFENTRYR!= 0x0000 ){};

    printf( "\n" );
}

/********************************************************************
* function name : faster_cmac_in_ram_execution
* input         : void
* output        : void
********************************************************************/
void faster_cmac_in_ram_execution ()
{
    char flag = 0;
    char exec_flag =0;
    r_icumif_isd_t *p_ISD[4];

    p_ISD[0] = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD[1] = p_ISD[0] + 1;
    p_ISD[2] = p_ISD[0] + 2;
    p_ISD[3] = p_ISD[0] + 3;

    memset(p_ISD[0], 0, sizeof(r_icumif_isd_t));
    memset(p_ISD[1], 0, sizeof(r_icumif_isd_t));
    memset(p_ISD[2], 0, sizeof(r_icumif_isd_t));
    memset(p_ISD[3], 0, sizeof(r_icumif_isd_t));

    // update service execute
    R_ICUMIF_ServiceRequest(p_ISD[0]);

    while(1)
    {
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD[0]->req_res_status )
        {
            if( SERV_REQUEST > p_ISD[0]->service_result )
            {
                break;
            }
        }
        else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD[0]->req_res_status)
                && (p_ISD[0]->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
        {
            break;
        }

        else
        {
            // check state of RAM execute
            trigger_service_and_wait(p_ISD[1]);

            if ( 1 == *(int *)p_ISD[1]->prm.GET_INFO.p_info && exec_flag == 0) {

                // execute CMAC
                trigger_service_and_wait(p_ISD[2]);
                if ( SERV_OK == p_ISD[2]->service_result )
                {
                    printf("OK, CMAC in RAM execute\n");

                    trigger_service_and_wait(p_ISD[3]);
                    exec_flag = 1;

                }
                else
                {
                    printf("NG, CMAC in RAM execute\n");
                }
                flag = 1;
            }
            else
            {
            }
        }
    }
    if ( 0 == flag )
    {
        printf("not_RAM_execute\n");
    }
    else
    {
        // ram_execute
    }
    return;
}
/********************************************************************
* function name : service_registration
* input         : r_icumif_isd_t *p_ISD[4]
* output        : unsigned int
* note          : need to enter, service parameters
*                 1 : SERVICE_06_HSM_FW_UPDATE
*                 2 : SERVICE_00_GET_INFO
*                 3 : SERVICE_01_AES_CMAC (GENERATION)
*                 4 : SERVICE_01_AES_CMAC (VERIFICATION)
*                 5 : registration_end,job_id strage location
********************************************************************/
unsigned int service_registration(r_icumif_isd_t *p_ISD[4])
{
    int n;
    int ret;
    unsigned int len;
    char *p, *p_cmd_str;
    bool  flg_menu_display = true;
    int exit_loop = 0;
    unsigned int value;
    value = 0;
    // service registration
    while(!exit_loop)
    {
        if(vrun_iobuff_readparam(1) != 0)
        {
            // 1st command parameter directly supplied after "icum" keyword
            sscanf(vrun_iobuff_readparam(1), "%i", &n);
            exit_loop = 1;
        }
        else
        {
            if(flg_menu_display)
            {
                printf("\n\n| Please input parameter\n");
                printf("| q    : Quit\n\n");
                printf("  ?> ");
            }
            else
            {
                printf("\n\n  ?> ");
            }
            vrun_console_get();
            if(*(char *)vrun_iobuff_readparam(0) == 'q')
            {
                n = -1;
                exit_loop = 1;
                flg_menu_display = true;
            }
            else
            {
                p_cmd_str = vrun_iobuff_readparam(0);
                p = DivideCommand((char *)p_cmd_str, 0, &len);
                if (strncmp(p,"registration_end",16) == 0)
                {
                    p = DivideCommand((char *)p_cmd_str, 1, &len);
                    if(strncmp(p, "BIG_BUFFER", 10) == 0)
                    {
                        ConvertStrToArrayAddr(&value, p, len);
                    }
                    exit_loop = 1;
                }
                else
                {
                    p = DivideCommand((char *)p_cmd_str, 1, &len);

                    if (strncmp(p, "SERVICE_01_AES_CMAC", 19) == 0)
                    {
                        p = DivideCommand((char *)p_cmd_str, 13, &len);
                        if (strncmp(p, "CMAC_GENERATION", 15) == 0)
                        {
                            ret = icum_vbtest_setparam_SERVICE_01_AES_CMAC(p_cmd_str, p_ISD[2]);
                        }
                        else
                        {
                            ret = icum_vbtest_setparam_SERVICE_01_AES_CMAC(p_cmd_str, p_ISD[3]);
                        }
                    }
                    if(ret != 0)
                    {
                        printf("registration error\n");
                    }
                }
            }
        }
    }
    return (value);
}

/********************************************************************
* function name : fast_cmac_execution
* input         : void
* output        : void
********************************************************************/
void fast_cmac_execution (void)
{
    unsigned int    service_time_start_end[2][2] = {{0,0},{0,0}};
    r_icumif_isd_t  *p_ISD[2];
    char            exec_flag =0;
    bool            queue_exist;
    int             check_mode;
    int             i;
    unsigned int    len;
    char            *p, *p_cmd_str;

    p_ISD[0] = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD[1] = p_ISD[0] + 1;

    memset(p_ISD[0], 0, sizeof(r_icumif_isd_t));
    memset(p_ISD[1], 0, sizeof(r_icumif_isd_t));

    printf("\n\n| Please input faster or fastest\n");
    printf("  ?> ");

    vrun_console_get();
    // select fast CMAC mode (faseter or fastest)
    p_cmd_str = vrun_iobuff_readparam(0);
    p = DivideCommand((char *)p_cmd_str, 0, &len);

    if (strncmp(p, "faster", 6) == 0)
    {
        check_mode = 2;
    }
    else if (strncmp(p, "fastest", 7) == 0)
    {
        check_mode = 1;
    }

    // input parameter for before service
    service_registration_for_fastCMAC(p_ISD[0]);

    // input parameter for CMAC (Generation)
    service_registration_for_fastCMAC(p_ISD[1]);

    // before service execution
    service_time_start_end[0][0] = sys_vtimer0_get_pclk_counter();
    R_ICUMIF_ServiceRequest(p_ISD[0]);

    while(1)
    {
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD[0]->req_res_status )
        {
            if( SERV_REQUEST > p_ISD[0]->service_result && service_time_start_end[0][1] == 0 )
            {
                service_time_start_end[0][1] = sys_vtimer0_get_pclk_counter();
            }
        }
        else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD[0]->req_res_status)
                && (p_ISD[0]->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
        {
            service_time_start_end[0][1] = sys_vtimer0_get_pclk_counter();
        }
        else
        {
            if (exec_flag == 0)
            {
                // request queue exist check
                queue_exist = req_queue_exist_check(check_mode);
                if ((check_mode == 1 && queue_exist == 0) || (check_mode == 2 && queue_exist == 1))
                {
                    // CMAC execution (Generation)
                    service_time_start_end[1][0] = sys_vtimer0_get_pclk_counter();
                    R_ICUMIF_ServiceRequest(p_ISD[1]);

                    exec_flag = 1;

                }
            }
        }
        // check for second service status
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD[1]->req_res_status )
        {
            if( SERV_REQUEST > p_ISD[1]->service_result && service_time_start_end[1][1] == 0)
            {
                service_time_start_end[1][1] = sys_vtimer0_get_pclk_counter();
            }
        }
        else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD[1]->req_res_status)
                && (p_ISD[1]->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
        {
            service_time_start_end[1][1] = sys_vtimer0_get_pclk_counter();
        }
        // Loop ends if the first and second services are over
        if (service_time_start_end[0][1] != 0 && service_time_start_end[1][1] != 0)
        {
            break;
        }
    }
    // result output
    for (i = 0 ; i < 2 ; i++)
    {
        result_output_for_fastCMAC(p_ISD[i],service_time_start_end[i][0], service_time_start_end[i][1], i+1);
    }
}

/********************************************************************
* function name : faster_cmac_for_suspend_check
* input         : void
* output        : void
********************************************************************/
void faster_cmac_for_suspend_check (void)
{
    unsigned int    service_time_start_end[3][2] = {{0,0},{0,0},{0,0}};
    r_icumif_isd_t  *p_ISD[3];
    char            exec_flag =0;
    bool            queue_exist;
    int             i;

    p_ISD[0] = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD[1] = p_ISD[0] + 1;
    p_ISD[2] = p_ISD[0] + 2;


    memset(p_ISD[0], 0, sizeof(r_icumif_isd_t));
    memset(p_ISD[1], 0, sizeof(r_icumif_isd_t));
    memset(p_ISD[2], 0, sizeof(r_icumif_isd_t));

    // input parameter for normal CMAC
    service_registration_for_fastCMAC(p_ISD[0]);

    // input parameter for GET INFO
    service_registration_for_fastCMAC(p_ISD[1]);

    // input parameter for CMAC (Generation)
    service_registration_for_fastCMAC(p_ISD[2]);

    // before service execution
    service_time_start_end[0][0] = sys_vtimer0_get_pclk_counter();
    R_ICUMIF_ServiceRequest(p_ISD[0]);

    while(1)
    {
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD[0]->req_res_status )
        {
            if( SERV_REQUEST > p_ISD[0]->service_result && service_time_start_end[0][1] == 0)
            {
                service_time_start_end[0][1] = sys_vtimer0_get_pclk_counter();
            }
        }
        else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD[0]->req_res_status)
                && (p_ISD[0]->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
        {
            service_time_start_end[0][1] = sys_vtimer0_get_pclk_counter();
        }
        else
        {
            if (exec_flag == 0)
            {
                // request queue exist check
                queue_exist = req_queue_exist_check(1);
                if (queue_exist == 0)
                {
                    service_time_start_end[1][0] = sys_vtimer0_get_pclk_counter();
                    R_ICUMIF_ServiceRequest(p_ISD[1]);
                    queue_exist = req_queue_exist_check(2);
                    if (queue_exist == 1)
                    {
                        // CMAC execution (Generation)
                        service_time_start_end[2][0] = sys_vtimer0_get_pclk_counter();
                        R_ICUMIF_ServiceRequest(p_ISD[2]);

                        exec_flag = 1;
                    }
                }
            }
        }
        // check for second service status
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD[1]->req_res_status )
        {
            if( SERV_REQUEST > p_ISD[1]->service_result && service_time_start_end[1][1] == 0)
            {
                service_time_start_end[1][1] = sys_vtimer0_get_pclk_counter();
            }
        }
        else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD[1]->req_res_status)
                && (p_ISD[1]->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
        {
            service_time_start_end[1][1] = sys_vtimer0_get_pclk_counter();
        }
        // check for therd service status
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD[2]->req_res_status )
        {
            if( SERV_REQUEST > p_ISD[2]->service_result && service_time_start_end[2][1] == 0)
            {
                service_time_start_end[2][1] = sys_vtimer0_get_pclk_counter();
            }
        }
        else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD[2]->req_res_status)
                && (p_ISD[2]->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
        {
            service_time_start_end[2][1] = sys_vtimer0_get_pclk_counter();
        }
        // Loop ends if all services are over
        if (   service_time_start_end[0][1] != 0
            && service_time_start_end[1][1] != 0
            && service_time_start_end[2][1] != 0)
        {
            break;
        }
    }
    // result output
    for (i = 0 ; i < 3 ; i++)
    {
        result_output_for_fastCMAC(p_ISD[i],service_time_start_end[i][0], service_time_start_end[i][1], i+1);
    }
}

/********************************************************************
* function name : aes_parallel_procesing_check
* input         : void
* output        : void
********************************************************************/
void aes_parallel_procesing_check (void)
{
    unsigned int    service_time_start_end[3][2] = {{0,0},{0,0},{0,0}};
    r_icumif_isd_t  *p_ISD[3];
    char            exec_flag =0;
    bool            queue_exist;
    int             i;

    p_ISD[0] = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD[1] = p_ISD[0] + 1;
    p_ISD[2] = p_ISD[0] + 2;


    memset(p_ISD[0], 0, sizeof(r_icumif_isd_t));
    memset(p_ISD[1], 0, sizeof(r_icumif_isd_t));
    memset(p_ISD[2], 0, sizeof(r_icumif_isd_t));

    // input parameter for normal CMAC
    service_registration_for_fastCMAC(p_ISD[0]);

    // input parameter for GET INFO
    service_registration_for_fastCMAC(p_ISD[1]);

    // input parameter for CMAC (Generation)
    service_registration_for_fastCMAC(p_ISD[2]);

    // before service execution
    service_time_start_end[0][0] = sys_vtimer0_get_pclk_counter();
    R_ICUMIF_ServiceRequest(p_ISD[0]);

    while(1)
    {
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD[0]->req_res_status )
        {
            if( SERV_REQUEST > p_ISD[0]->service_result && service_time_start_end[0][1] == 0)
            {
                service_time_start_end[0][1] = sys_vtimer0_get_pclk_counter();
            }
        }
        else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD[0]->req_res_status)
                && (p_ISD[0]->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
        {
            service_time_start_end[0][1] = sys_vtimer0_get_pclk_counter();
        }
        else
        {
            if (exec_flag == 0)
            {
                // request queue exist check
                queue_exist = req_queue_exist_check(1);
                if (SERV_IN_PROCESS == p_ISD[0]->service_result)
                {
                    service_time_start_end[1][0] = sys_vtimer0_get_pclk_counter();
                    R_ICUMIF_ServiceRequest(p_ISD[1]);
                    queue_exist = req_queue_exist_check(2);
                    if (queue_exist == 1)
                    {
                        // CMAC execution (Generation)
                        service_time_start_end[2][0] = sys_vtimer0_get_pclk_counter();
                        R_ICUMIF_ServiceRequest(p_ISD[2]);

                        exec_flag = 1;
                    }
                }
            }
        }
        // check for second service status
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD[1]->req_res_status )
        {
            if( SERV_REQUEST > p_ISD[1]->service_result && service_time_start_end[1][1] == 0)
            {
                service_time_start_end[1][1] = sys_vtimer0_get_pclk_counter();
            }
        }
        else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD[1]->req_res_status)
                && (p_ISD[1]->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
        {
            service_time_start_end[1][1] = sys_vtimer0_get_pclk_counter();
        }
        // check for therd service status
        if( R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD[2]->req_res_status )
        {
            if( SERV_REQUEST > p_ISD[2]->service_result && service_time_start_end[2][1] == 0)
            {
                service_time_start_end[2][1] = sys_vtimer0_get_pclk_counter();
            }
        }
        else if((R_ICUMIF_REQRES_PE_REQ_ENQUE == p_ISD[2]->req_res_status)
                && (p_ISD[2]->req_nointerrupt == R_ICUMIF_REQRES_NOINTERRPUT))
        {
            service_time_start_end[2][1] = sys_vtimer0_get_pclk_counter();
        }
        // Loop ends if all services are over
        if (   service_time_start_end[0][1] != 0
            && service_time_start_end[1][1] != 0
            && service_time_start_end[2][1] != 0)
        {
            break;
        }
    }
    // result output
    for (i = 0 ; i < 3 ; i++)
    {
        result_output_for_fastCMAC(p_ISD[i],service_time_start_end[i][0], service_time_start_end[i][1], i+1);
    }
}

/********************************************************************
* function name : service_registration_for_fastCMAC
* input         : r_icumif_isd_t *p_ISD
* output        : void
********************************************************************/
void service_registration_for_fastCMAC(r_icumif_isd_t *p_ISD)
{
    int           n;
    int           ret;
    unsigned int  len;
    char          *p, *p_cmd_str;
    bool          flg_menu_display = true;
    int           exit_loop = 0;
    // service registration
    while(!exit_loop)
    {
        if(vrun_iobuff_readparam(1) != 0)
        {
            // 1st command parameter directly supplied after "icum" keyword
            sscanf(vrun_iobuff_readparam(1), "%i", &n);
            exit_loop = 1;
        }
        else
        {
            if(flg_menu_display)
            {
                printf("\n\n| Please input parameter\n");
                printf("| q    : Quit\n\n");
                printf("  ?> ");
            }
            else
            {
                printf("\n\n  ?> ");
            }
            vrun_console_get();
            if(*(char *)vrun_iobuff_readparam(0) == 'q')
            {
                n = -1;
                exit_loop = 1;
                flg_menu_display = true;
            }
            else
            {
                p_cmd_str = vrun_iobuff_readparam(0);
                p = DivideCommand((char *)p_cmd_str, 1, &len);

                if (strncmp(p, "SERVICE_01_AES_CMAC", 19) == 0)
                {
                    ret = icum_vbtest_setparam_SERVICE_01_AES_CMAC(p_cmd_str, p_ISD);
                }
                else if (strncmp(p, "SERVICE_03_PK_IMPORT", 20) == 0)
                {
                    ret = icum_vbtest_setparam_SERVICE_03_PK_IMPORT(p_cmd_str, p_ISD);
                }
                if(ret != 0)
                {
                    printf("registration error\n");
                }
                else
                {
                    exit_loop = 1;
                }
            }
        }
    }
    return;
}
/********************************************************************
* function name : req_queue_exist_check
* input         : int check_mode
*               : 1 : wait until reqest queue is flg_empty
*                 2 : queue exist check
* output        : bool queue_exist
********************************************************************/
bool req_queue_exist_check(int check_mode)
{
    volatile uint32_t   *p_reg;
    ptr_manip_t         queptr;
    uint32_t            *p_queptr;
    bool                queue_exist = 1;

    /* ICUM_CMDREGICUM_SEMAPHORE */
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;

    /* QUE_ADDR_LOWER_24BITS,QUE_ADDR_UPPER_8BITS_VALUE */
    queptr.val = (*p_reg & QUE_ADDR_LOWER_24BITS) | QUE_ADDR_UPPER_8BITS_VALUE;
    p_queptr = (uint32_t *)queptr.p_ptr;

    if (check_mode == 1)
    {
        /* wait until request queue is empty */
        while (queue_exist == 1)
        {
            /* Wait 1micro sec */
            wait_micro_sec(1);
            /* check request queue */
            if (get_semaphore())
            {
                if (0 == *p_queptr)
                {
                    queue_exist = 0;
                }
                free_semaphore();
            }
        }

    }
    else if (check_mode == 2)
    {
        /* request queue check */
        if (get_semaphore())
        {
            if (0 == *p_queptr)
            {
                queue_exist = 0;
            }
            free_semaphore();
        }
    }
    else
    {

    }
    return (queue_exist);
}
/********************************************************************
* function name : result_output_for_fastCMAC
* input         : r_icumif_isd_t *p_ISD
*               : int service_num
* output        : void
********************************************************************/
void result_output_for_fastCMAC(r_icumif_isd_t *p_ISD, unsigned long vrun_stime, unsigned long vrun_etime, int service_num)
{
    if (R_ICUMIF_REQRES_PE_RES_DEQUE == p_ISD->req_res_status)
    {
        /* output */
        if(p_ISD->service_result == SERV_OK)
        {
            printf("Service%d SERV_OK\n",service_num);
            printf("Processed in : ");
            vrun_iobuff_print_chrono_input_time(vrun_stime,vrun_etime);NL();FLUSH();
            printf("Service start time : ");
            vrun_iobuff_print_timestamp_input_time(vrun_stime);NL();FLUSH();
            printf("Service end time : ");
            vrun_iobuff_print_timestamp_input_time(vrun_etime);NL();FLUSH();
        }
        else
        {
            printf("Service%d not SERV_OK :0x%x\n",service_num, p_ISD->service_result);
        }
    }
    else
    {
        printf("Service%d not execute\n",service_num);
    }
}
/********************************************************************
* function name : vrun_iobuff_print_chrono_input_time
* input         : unsigned long vrun_stime
*               : unsigned long vrun_etime
* output        : void
********************************************************************/
void vrun_iobuff_print_chrono_input_time(unsigned long vrun_stime, unsigned long vrun_etime)
{
    time_t time;
    char temp[32];
    unsigned long local_vrun_chrono_sec;
    unsigned long local_vrun_chrono_millisec;
    unsigned long local_vrun_chrono_microsec;
    unsigned long local_vrun_chrono_nanosec;

    sys_vtimer0_cnv_pclk2time((unsigned long)(vrun_etime - vrun_stime),&time);
    local_vrun_chrono_sec      = time.sec;
    local_vrun_chrono_millisec = time.msec;
    local_vrun_chrono_microsec = time.usec;
    local_vrun_chrono_nanosec  = time.nsec;

    /* output to the I/O buffer */
    if((!local_vrun_chrono_sec) && (!local_vrun_chrono_millisec))
    {
        sprintf(temp, "%dus %dns", local_vrun_chrono_microsec, local_vrun_chrono_nanosec);
    }
    else if(!vrun_chrono_sec)
    {
        sprintf(temp, "%dms %03dus", local_vrun_chrono_millisec, local_vrun_chrono_microsec);
    }
    else
    {
        sprintf(temp, "%ds %03dms %03dus", vrun_chrono_sec, local_vrun_chrono_millisec, local_vrun_chrono_microsec);
    }
    vrun_iobuff_print_txt((const char *)temp);
    return;
}
/********************************************************************
* function name : vrun_iobuff_print_timestamp_input_time
* input         : unsigned long vrun_time
* output        : void
********************************************************************/
void vrun_iobuff_print_timestamp_input_time(unsigned long vrun_time)
{
    time_t time;
    char temp[32];
    unsigned long local_vrun_chrono_sec;
    unsigned long local_vrun_chrono_millisec;
    unsigned long local_vrun_chrono_microsec;
    unsigned long local_vrun_chrono_nanosec;

    sys_vtimer0_cnv_pclk2time((unsigned long)vrun_time,&time);
    local_vrun_chrono_sec      = time.sec;
    local_vrun_chrono_millisec = time.msec;
    local_vrun_chrono_microsec = time.usec;
    local_vrun_chrono_nanosec  = time.nsec;

    /* output to the I/O buffer */
    if((!local_vrun_chrono_sec) && (!local_vrun_chrono_millisec))
    {
        sprintf(temp, "%dus %dns", local_vrun_chrono_microsec, local_vrun_chrono_nanosec);
    }
    else if(!vrun_chrono_sec)
    {
        sprintf(temp, "%dms %03dus", local_vrun_chrono_millisec, local_vrun_chrono_microsec);
    }
    else
    {
        sprintf(temp, "%ds %03dms %03dus", vrun_chrono_sec, local_vrun_chrono_millisec, local_vrun_chrono_microsec);
    }
    vrun_iobuff_print_txt((const char *)temp);
    return;
}

/********************************************************************
* get semephore for CMAC fast (INTPES)
*   success: return 1
*   fail:    return 0
********************************************************************/
int get_semaphore()
{
    register volatile uint32_t *p_reg;
    uint32_t uiAuth;
    uint32_t uiIcusem;
    uint32_t uiCount;
    int8_t  ret;

    uiAuth = 0UL;
    uiIcusem = 0UL;
    ret = 0;

    /* Get the Semaphore register address */
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;
    uiCount = 0UL;

    /* Wait for the Semaphore */
    do
    {
        /* Try to get the Semaphore as step1 */
        uiIcusem = *p_reg;
        uiAuth   = uiIcusem & PE_SEMAPHORE_AUTH_MASK;

        if (PE_SEMAPHORE_AUTH_GET1ST == uiAuth)
        {
            /* Try to get the Semaphore as step2 */
            uiIcusem = *p_reg;
            uiAuth   = (uiIcusem & PE_SEMAPHORE_AUTH_MASK);

            /*PE_SEMAPHORE_AUTH_GET2ND */
            if (PE_SEMAPHORE_AUTH_GET2ND != uiAuth)
            {
                /* Error case. retry to get the Semaphore */
                /* Do nothing */
            }
            else
            {
                /* Got the Semaphore */
                *p_reg |= PE_SEMAPHORE_REQ;
            }
            break;
        }
        else if (PE_SEMAPHORE_AUTH_GET2ND == uiAuth)
        {
            /* Got the Semaphore */
            *p_reg |= PE_SEMAPHORE_REQ;
            break;
        }
        else
        {
            /* Do nothing */
        }
        uiCount++;
    } while (uiCount < PE_SEMAPHORE_WAIT);

    if (uiCount < PE_SEMAPHORE_WAIT)
    {
        /* successed */
        ret = 1;
    } else
    {
        /* failed */
        ret = 0;
    }
    return ret;
}
/********************************************************************
* free semaphore for CMAC fast (INTPES)
********************************************************************/
void free_semaphore()
{
    register volatile uint32_t *p_reg;
    uint32_t uiAuth;

    /* Get the Semaphore register address */
    p_reg = (volatile uint32_t *)&ICUM_CMDREGICUM_SEMAPHORE;
    uiAuth   = (*p_reg & PE_SEMAPHORE_AUTH_MASK);

    /* Check the Semaphore register status */
    if (PE_SEMAPHORE_AUTH_GET2ND == uiAuth)
    {
        /* Clear Request bit in the Semaphore register */
        *p_reg &= ~PE_SEMAPHORE_RES;
    }
    else
    {
        /* Do nothing */
    }
    /* Free the Semaphore */
    *p_reg &= ~PE_SEMAPHORE_AUTH_MASK;
}

/********************************************************************
* Wait micro usec
********************************************************************/
static void wait_micro_sec(int wait_usec)
{
    int timestamp_usec = 0;
    vrun_chrono_start();
    while(1)
    {
        vrun_chrono_stop();
        vrun_chrono_gettimestamp();
        timestamp_usec = vrun_chrono_microsec;
        if( timestamp_usec < wait_usec )
        {
            __asm("nop");
        }
        else
        {
            break;
        }
    }
}
