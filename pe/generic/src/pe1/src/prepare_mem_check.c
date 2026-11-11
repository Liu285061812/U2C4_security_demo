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
#include "pe1.h"
#include "shared.h"

#include "vrun.api.h"
#include "pe_target_device.h"
#include "pe_reg.h"


// ********************************************************************************
// Local defines
// ********************************************************************************

static void init_data_flash_user_area(void)
{
    uint32_t addr;
    uint32_t wdata = 0xFFFFFFFF;

    // Change to data flash programming/erasure mode
    REG_PE_FACIFENTRYR = 0xAA80;
    while( REG_PE_FACIFENTRYR != 0x0080 ){};
    while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};

    // Programming/erasure/DataFlashBlankChecking enabled
    REG_PE_FACIFHVE3  = 0x01;
    REG_PE_FACIFHVE15 = 0x01;

    // Initialize user DF 0xFF200000-0xFF22FFFF
    printf("## + Initialize user DF 0x%08X-0x%08X   ##\n", ADDR_LOWER_DF_BANKA, ADDR_UPPER_DF_BANKB);

    for(addr = ADDR_LOWER_DF_BANKA; addr < ADDR_UPPER_DF_BANKB; addr += 4){
        // issue DF program command
        REG_PE_FACIFSADDR  = addr;
        REG_PE_FACICMD = 0xE8;
        REG_PE_FACICMD = 0x01;
        REG_PE_FACICMDDT = wdata;
        REG_PE_FACICMD = 0xD0;

        // wait for e/o blank check operation
        while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};

        // check for errors
        if( (REG_PE_FACIFASTAT&0x10/*CMDLK*/) || (REG_PE_FACIFSTATR&(0x00004000/*ILGLERR*/|0x00002000/*ERSERR*/|0x00001000/*PRGERR*/)) ){
            printf("\n");
            printf("!!  Error caused in issuing DF program command  !!\n");
            break;
        }
    }

    // Programming/erasure/DataFlashBlankChecking disabled
    REG_PE_FACIFHVE3  = 0x00;
    REG_PE_FACIFHVE15 = 0x00;

    // Change to Data flash read mode
    REG_PE_FACIFENTRYR = 0xAA00;
    while( REG_PE_FACIFENTRYR!= 0x0000 ){};

    printf( "\n" );
}

static void block_erase_code_flash(void)
{
    uint32_t addr;
    uint32_t select_addr = 0;
    uint32_t exit_loop = 0;
    uint32_t n = 0;

    while(!exit_loop){
    
        // Select Erase Area
        printf("\n| Please Select Erase Start Area\n");
        printf("|---------------------------------\n");
        printf("| Code Flash Bank B : 0x00300000-0x005FFFFF\n");
        printf("| Code Flash Bank C : 0x00600000-0x008FFFFF\n");
        printf("| Code Flash Bank D : 0x00900000-0x00BFFFFF\n");
        printf("| Code Flash Bank E : 0x00C00000-0x00EFFFFF\n");
        printf("  ?> ");

        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &select_addr);

        if( (select_addr >= 0x00300000) && (select_addr <= 0x00F00000) )
        {
            exit_loop = 1;
        }
        else
        {
            printf("Select Error!!\n\n");
        }
    }

    exit_loop = 0;

    while(!exit_loop){

        // Erase Block Size
        printf("\n\n| Please Select Erase Block Size\n");
        printf("|---------------------------------\n");
        printf("|Erase some block?(Code Flash Bank = 48 Block) \n");
        printf("|Range:1-192 \n");
        printf("  ?> ");
    
        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &n);

        if( (n >= 1) && (n <= 192))
        {
            exit_loop = 1;
        }
        else
        {
            printf("Parameter Error!!\n\n");
        }
    }

    // Change to code flash programming/erasure mode
    REG_PE_FACIFENTRYR = 0xAA01;
    while( REG_PE_FACIFENTRYR != 0x0001 ){};
    while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};

    // Programming/erasure/CodeFlashBlankChecking enabled
    REG_PE_FACIFHVE3  = 0x01;
    REG_PE_FACIFHVE15 = 0x01;

    // Erasure CF RANGE
    printf("## + Erasure CF 0x%08X-0x%08X   ##\n", select_addr, select_addr+n*CF_BLOCK_SIZE-1);

    for(addr = select_addr; addr < select_addr+n*CF_BLOCK_SIZE; addr += CF_BLOCK_SIZE){
        // issue CF block erase command
        REG_PE_FACIFSADDR  = addr;
        REG_PE_FACICMD = 0x20;
        REG_PE_FACICMD = 0xD0;

        // wait for e/o blank check operation
        while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};

        // check for errors
        if( (REG_PE_FACIFASTAT&0x10/*CMDLK*/) || (REG_PE_FACIFSTATR&(0x00004000/*ILGLERR*/|0x00002000/*ERSERR*/|0x00001000/*PRGERR*/)) ){
            printf("\n");
            printf("!!  Error caused in issuing CF program command  !!\n");
            goto end;
        }
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

static void init_retention_ram(void)
{
    uint64_t *p_data;
    uint32_t  size;

    p_data = (uint64_t *)ADDR_LOWER_RRAM;
    size = (uint32_t)ADDR_UPPER_CRAM - (uint32_t)ADDR_LOWER_RRAM + 1;
    while(size) {*(p_data++) = 0; size -= 8;}

    printf( "\n" );
}


static void block_erase_code_flash_read_change(void)
{
    uint32_t addr;
    uint32_t select_addr = 0;
    uint32_t exit_loop = 0;
    uint32_t n = 0;

    while(!exit_loop){
    
        // Select Erase Area
        printf("\n| Please Select Erase Start Area\n");
        printf("|---------------------------------\n");
        printf("| Code Flash Bank B : 0x00300000-0x005FFFFF\n");
        printf("| Code Flash Bank C : 0x00600000-0x008FFFFF\n");
        printf("| Code Flash Bank D : 0x00900000-0x00BFFFFF\n");
        printf("| Code Flash Bank E : 0x00C00000-0x00EFFFFF\n");
        printf("  ?> ");

        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &select_addr);

        if( (select_addr >= 0x00300000) && (select_addr <= 0x00F00000) )
        {
            exit_loop = 1;
        }
        else
        {
            printf("Select Error!!\n\n");
        }
    }

    exit_loop = 0;

    while(!exit_loop){

        // Erase Block Size
        printf("\n\n| Please Select Erase Block Size\n");
        printf("|---------------------------------\n");
        printf("|Erase some block?(Code Flash Bank = 48 Block) \n");
        printf("|Range:1-192 \n");
        printf("  ?> ");
    
        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &n);

        if( (n >= 1) && (n <= 192))
        {
            exit_loop = 1;
        }
        else
        {
            printf("Parameter Error!!\n\n");
        }
    }

    // Erasure CF RANGE
    printf("## + Erasure CF 0x%08X-0x%08X   ##\n", select_addr, select_addr+n*CF_BLOCK_SIZE-1);

    for(addr = select_addr; addr < select_addr+n*CF_BLOCK_SIZE; addr += CF_BLOCK_SIZE){
        // Change to code flash programming/erasure mode
        REG_PE_FACIFENTRYR = 0xAA01;
        // (FENTRYR_0 bit 0 = 1 P/E mode)
        while( REG_PE_FACIFENTRYR != 0x0001 ){};
        while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};
        printf("## P/E  mode ##\n");

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
        if( (REG_PE_FACIFASTAT&0x10/*CMDLK*/) || (REG_PE_FACIFSTATR&(0x00004000/*ILGLERR*/|0x00002000/*ERSERR*/|0x00001000/*PRGERR*/)) ){
            printf("\n");
            printf("!!  Error caused in issuing CF program command  !!\n");
            // Programming/erasure/DataFlashBlankChecking disabled
            REG_PE_FACIFHVE3  = 0x00;
            REG_PE_FACIFHVE15 = 0x00;

            // Change to Code flash read mode
            REG_PE_FACIFENTRYR = 0xAA00;
            while( REG_PE_FACIFENTRYR!= 0x0000 ){};
            printf( "\n" );
            goto end;
        }
        // Programming/erasure/DataFlashBlankChecking disabled
        REG_PE_FACIFHVE3  = 0x00;
        REG_PE_FACIFHVE15 = 0x00;

        // Change to Code flash read mode
        REG_PE_FACIFENTRYR = 0xAA00;
        // (FENTRYR_0 bit 0 = 0 Read mode)
        while( REG_PE_FACIFENTRYR!= 0x0000 ){};
        printf("## Read mode ##\n");
    }

end:

    printf( "\n" );
}

static void init_code_flash_user_area(void)
{
    int i;
    uint32_t addr;
    uint32_t select_addr = 0;
    uint32_t exit_loop = 0;
    uint32_t n = 0;
    uint32_t wdata = 0x10000000;

    while(!exit_loop){
    
        // Select Write Area
        printf("\n| Please Select Write Start Area\n");
        printf("|---------------------------------\n");
        printf("| Code Flash Bank B : 0x00300000-0x005FFFFF\n");
        printf("| Code Flash Bank C : 0x00600000-0x008FFFFF\n");
        printf("| Code Flash Bank D : 0x00900000-0x00BFFFFF\n");
        printf("| Code Flash Bank E : 0x00C00000-0x00EFFFFF\n");
        printf("  ?> ");

        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &select_addr);

        if( (select_addr >= 0x00300000) && (select_addr <= 0x00F00000) )
        {
            exit_loop = 1;
        }
        else
        {
            printf("Select Error!!\n\n");
        }
    }

    exit_loop = 0;

    while(!exit_loop){

        // Write Byte Size
        printf("\n\n| Please Select Write Size\n");
        printf("|---------------------------------\n");
        printf("|Write Multiple of 512 Byte(Code Flash Bank :6144) \n");
        printf("|Range:1-24576 \n");
        printf("  ?> ");
    
        vrun_console_get();
        sscanf(vrun_iobuff_readparam(0), "%i", &n);

        if( (n >= 1) && (n <= 24576))
        {
            exit_loop = 1;
        }
        else
        {
            printf("Parameter Error!!\n\n");
        }
    }


    // Change to code flash programming/erasure mode
    REG_PE_FACIFENTRYR = 0xAA01;
    while( REG_PE_FACIFENTRYR != 0x0001 ){};
    while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};

    // Programming/erasure/CodeFlashBlankChecking enabled
    REG_PE_FACIFHVE3  = 0x01;
    REG_PE_FACIFHVE15 = 0x01;


   // Write CF Range
   printf("## + Write CF 0x%08X-0x%08X   ##\n", select_addr, select_addr+n*CF_WRITE_SIZE-1);

    // issue CF program command
    for(addr = select_addr; addr < select_addr+n*CF_WRITE_SIZE; addr += CF_WRITE_SIZE){
        REG_PE_FACIFSADDR  = addr;
        REG_PE_FACICMD = 0xE8;
        REG_PE_FACICMD = 0x80;
        for( i=0; i<128; i++){
            REG_PE_FACICMDDT = wdata++;
        }
        REG_PE_FACICMD = 0xD0;

        // wait for e/o blank check operation
        while( (REG_PE_FACIFSTATR&0x00008000/*FRDY*/) == 0 ){};

        // check for errors
        if( (REG_PE_FACIFASTAT&0x10/*CMDLK*/) || (REG_PE_FACIFSTATR&(0x00004000/*ILGLERR*/|0x00002000/*ERSERR*/|0x00001000/*PRGERR*/)) ){
            printf("\n");
            printf("!!  Error caused in issuing CF program command  !!\n");
            goto end;
        }
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

/*===========================================================================
  Sub routine for mem_check
  ===========================================================================*/
void prepare_mem_check(void)
{
    int n;
    int exit_loop = 0;
    int f_dispskip=0;
    char dt_char;

    while(!exit_loop)
    {
        if(f_dispskip==0){
        if(flg_menu_display){
            printf("\n\n");
            printf("|-------------------------------------\n");
            printf("| Set Data for Memory Check Test\n");
            printf("|-------------------------------------\n");
            printf("| (100) : Init DataFlash User Area; \n" );
            printf("| (200) : Init Retention RAM; \n" );
            printf("| (300) : Block CodeFlash Erasure; \n" );
            printf("| (350) : Block CodeFlash Erasure Read Change; \n" );
            printf("| (400) : Init CodeFlash User Area; \n" );
            printf("|-------------------------------------\n");
            printf("| q    : Quit\n\n");
        }
        else{
            printf("\n");
        }

        }
        f_dispskip=1;

        printf("  ?> ");
        vrun_console_get();
        dt_char = *(char *)vrun_iobuff_readparam(0);
        if( dt_char == 'q')
        {
            n = -1;
            exit_loop = 1;
        }
        else if( dt_char == '\r' || dt_char == '?' )
        {
            n = -1;
            f_dispskip = 0;
        }
        else
        {
            sscanf(vrun_iobuff_readparam(0), "%i", &n);
        }
        
        switch(n)
        {
            case 100:
                init_data_flash_user_area();
                break;
            case 200:
                init_retention_ram();
                break;
            case 300:
                block_erase_code_flash();
                break;
            case 350:
                block_erase_code_flash_read_change();
                break;
            case 400:
                init_code_flash_user_area();
                break;
           default:
                break;
        }
    }
}
