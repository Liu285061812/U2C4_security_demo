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
#include "pe1.int.h"
#include "shared.h"

#include "vrun.api.h"

#include "pe_target_device.h"

#include "renesas_types.h"

// ********************************************************************************
// Local constants
// ********************************************************************************

static const uint8_t hmac_key1[]= {
    0x82, 0xf3, 0xb6, 0x9a, 0x1b, 0xff, 0x4d, 0xe1, 0x5c, 0x33
};

static const uint8_t hmac_key2[]= {
    0x47, 0x66, 0xe6, 0xfe, 0x5d, 0xff, 0xc9, 0x8a, 0x5c, 0x50
};

static const uint8_t hmac_key3[]={
    0x37, 0x14, 0x70, 0x78, 0x39, 0xda, 0xf7, 0x91, 0x22, 0xc7,
    0x82, 0x41, 0x63, 0x51, 0x38, 0x5e, 0x88, 0xa8, 0x1d, 0x31,
    0xc9, 0xf6, 0x41, 0xd8, 0xdc, 0xe5, 0x38, 0xe9, 0x0e, 0x63,
    0xc9, 0x58, 0x92, 0xa2, 0xea, 0x9b, 0x19, 0x62, 0xed, 0x0b,
    0xa3, 0x72, 0xf4, 0x8e, 0x94, 0x74, 0xaa, 0x73, 0x0a, 0xe2
};

static const uint8_t hmac_key4[]={
    0xce, 0x4c, 0x92, 0x6c, 0x09, 0x22, 0xba, 0x36, 0x26, 0x9a,
    0x20, 0xd6, 0x0d, 0xcf, 0x08, 0xd4, 0x3a, 0x1c, 0xea, 0x12,
    0x0f, 0x26, 0x6a, 0xf7, 0x6f, 0x1c, 0x8a, 0xcd, 0x88, 0x3d,
    0x1f, 0x68, 0xf0, 0x9b, 0x82, 0x09, 0xf4, 0x1f, 0x87, 0x82,
    0x2d, 0xce, 0xb3, 0x9a, 0x54, 0x4a, 0xa9, 0xb2, 0x56, 0x9c
};

static const uint8_t hmac_key5[]={
    0x6f, 0x35, 0x62, 0x8d, 0x65, 0x81, 0x34, 0x35, 0x53, 0x4b,
    0x5d, 0x67, 0xfb, 0xdb, 0x54, 0xcb, 0x33, 0x40, 0x3d, 0x04,
    0xe8, 0x43, 0x10, 0x3e, 0x63, 0x99, 0xf8, 0x06, 0xcb, 0x5d,
    0xf9, 0x5f, 0xeb, 0xbd, 0xd6, 0x12, 0x36, 0xf3, 0x32, 0x45
};

static const uint8_t hmac_key6[]={
    0x17, 0xb5, 0x28, 0x58, 0xe3, 0xe1, 0x35, 0xbe, 0x44, 0x40,
    0xd7, 0xdf, 0x0c, 0xa9, 0x96, 0xf4, 0x1c, 0xcb, 0x78, 0xb7,
    0xd8, 0xcc, 0x19, 0x24, 0xd8, 0x30, 0xfe, 0x81, 0xe0, 0xfd,
    0x27, 0x9c, 0x13, 0x1c, 0xe3, 0x54, 0x63, 0x03, 0xe9, 0x5a
};

static const uint8_t hmac_key7[]={
    0xf1, 0x6a, 0xd7, 0x37, 0x90, 0xca, 0x39, 0xc7, 0xf9, 0x85,
    0x6c, 0x44, 0x83, 0x20, 0x2e, 0x7f, 0x8e, 0x0c, 0x82, 0x83,
    0xc7, 0xd5, 0x0d, 0x6d, 0xa7, 0x9c, 0xc0, 0x7d, 0x3d, 0xc7,
    0xb7, 0x6c, 0x2e, 0xf7, 0x61, 0x00, 0xfa, 0x3a, 0xe2, 0xdf,
    0x80, 0x83, 0xb5, 0xa1, 0xc5, 0x57, 0x96, 0x28, 0xf1, 0xc8
};

static const uint8_t hmac_key8[]={
    0xa5, 0x70, 0x9b, 0xa5, 0x52, 0x9c, 0xb9, 0xa1, 0xa2, 0x27,
    0xf0, 0xbe, 0x44, 0x8e, 0x11, 0x9a, 0x35, 0x6f, 0x92, 0xe1,
    0x3e, 0xfc, 0x34, 0x63, 0xbe, 0xaa, 0xe4, 0x6a, 0xa9, 0x29,
    0xdf, 0x4a, 0xd1, 0x99, 0x1a, 0x39, 0x64, 0xfb, 0xe1, 0x61,
    0xb6, 0xe5, 0xbe, 0x34, 0x41, 0x7a, 0x9c, 0x00, 0xeb, 0x9a
};

static const uint8_t hmac_key9[]={
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa                          
};

static const uint8_t hmac_key10[]={
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19
};


static const uint8_t p_msg1[]={
    0xfc, 0xd6, 0xd9, 0x8b, 0xef, 0x45, 0xed, 0x68,
    0x50, 0x80, 0x6e, 0x96, 0xf2, 0x55, 0xfa, 0x0c,
    0x81, 0x14, 0xb7, 0x28, 0x73, 0xab, 0xe8, 0xf4,
    0x3c, 0x10, 0xbe, 0xa7, 0xc1, 0xdf, 0x70, 0x6f,
    0x10, 0x45, 0x8e, 0x6d, 0x4e, 0x1c, 0x92, 0x01,
    0xf0, 0x57, 0xb8, 0x49, 0x2f, 0xa1, 0x0f, 0xe4,
    0xb5, 0x41, 0xd0, 0xfc, 0x9d, 0x41, 0xef, 0x83,
    0x9a, 0xcf, 0xf1, 0xbc, 0x76, 0xe3, 0xfd, 0xfe,
    0xbf, 0x22, 0x35, 0xb5, 0xbd, 0x03, 0x47, 0xa9,
    0xa6, 0x30, 0x3e, 0x83, 0x15, 0x2f, 0x9f, 0x8d,
    0xb9, 0x41, 0xb1, 0xb9, 0x4a, 0x8a, 0x1c, 0xe5,
    0xc2, 0x73, 0xb5, 0x5d, 0xc9, 0x4d, 0x99, 0xa1,
    0x71, 0x37, 0x79, 0x69, 0x23, 0x41, 0x34, 0xe7,
    0xda, 0xd1, 0xab, 0x4c, 0x8e, 0x46, 0xd1, 0x8d,
    0xf4, 0xdc, 0x01, 0x67, 0x64, 0xcf, 0x95, 0xa1,
    0x1a, 0xc4, 0xb4, 0x91, 0xa2, 0x64, 0x6b, 0xe1
};


// ********************************************************************************
// Test functions
// ********************************************************************************

/*===========================================================================
  ICU-M test function
  [PUBLIC]
  ===========================================================================*/

void icum_hmac_import(void){
    
    int n;
    
    while(1)
    {
        printf("\n| select hamc import test menu\n");
        printf("|----------------------------------\n");
        printf("|1        : No1.test\n");
        printf("|2        : No2.test\n");
        printf("|3        : No3.test\n");
        printf("|4        : No4.test\n");
        printf("|5        : No5.test\n");
        printf("|6        : No6.test\n");
        printf("|7        : No7.test\n");
        printf("|8        : No8.test\n");
        printf("|9        : No9.test\n");
        printf("|10       : No10.test\n");
        printf("|11       : No11.errortest\n");
        printf("|12       : No12.errortest\n");
        printf("|13       : No13.errortest\n");
        printf("|14       : No14.test\n");
        printf("|15       : No15.errortest\n");
        printf("|q        : Quit\n\n");
        printf("    ?> ");
        
        vrun_console_get();
        
        if(*(char *)vrun_iobuff_readparam(0) == 'q')
        {
            break;
        }
        else
        {
            sscanf(vrun_iobuff_readparam(0), "%i", &n);
        }
        // pause life signal management
        pause_life_signal();
        
        switch(n)
        {
            case 1: icum_hmac_import_test1();       break;
            case 2: icum_hmac_import_test2();       break;
            case 3: icum_hmac_import_test3();       break;
            case 4: icum_hmac_import_test4();       break;
            case 5: icum_hmac_import_test5();       break;
            case 6: icum_hmac_import_test6();       break;
            case 7: icum_hmac_import_test7();       break;
            case 8: icum_hmac_import_test8();       break;
            case 9: icum_hmac_import_test9();       break;
            case 10:icum_hmac_import_test10();      break;
            case 11:icum_hmac_import_test11();      break;
            case 12:icum_hmac_import_test12();      break;
            case 13:icum_hmac_import_test13();      break;
            case 14:icum_hmac_import_test14();      break;
            case 15:icum_hmac_import_test15();      break;
            default:
                goto end;
        }
        // restart life signal
        restart_life_signal();

end:
        ;
    }
}


void icum_hmac_import_test1(){
    
    r_icumif_isd_t     *p_ISD;
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    
    p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
    p_ISD->p_callbackfunc                       = 0;
    p_ISD->job_id                               = 0;
    p_ISD->prm.HMAC_IMPORT.key_id               = 0;
    p_ISD->prm.HMAC_IMPORT.p_key                = hmac_key1;
    p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = 10;
    
    printf("No1. hmac_import_test\n");
    printf("\n*------ Test No. 01\n");
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}


void icum_hmac_import_test2(){
    
    r_icumif_isd_t     *p_ISD;
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    
    p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
    p_ISD->p_callbackfunc                       = 0;
    p_ISD->job_id                               = 0;
    p_ISD->prm.HMAC_IMPORT.key_id               = 1;
    p_ISD->prm.HMAC_IMPORT.p_key                = hmac_key2;
    p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = 10;
    
    printf("No2. hmac_import_test\n");
    printf("*------ Test No. 02\n");
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}


void icum_hmac_import_test3(){
    
    r_icumif_isd_t     *p_ISD;
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
    p_ISD->p_callbackfunc                       = 0;
    p_ISD->job_id                               = 0;
    p_ISD->prm.HMAC_IMPORT.key_id               = 2;
    p_ISD->prm.HMAC_IMPORT.p_key                = hmac_key3;
    p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = 50;
    
    printf("No3. hmac_import_test\n");
    printf("*------ Test No. 03\n");
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}


void icum_hmac_import_test4(){
    
    r_icumif_isd_t     *p_ISD;
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
    p_ISD->p_callbackfunc                       = 0;
    p_ISD->job_id                               = 0;
    p_ISD->prm.HMAC_IMPORT.key_id               = 3;
    p_ISD->prm.HMAC_IMPORT.p_key                = hmac_key4;
    p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = 50;
    
    printf("No4. hmac_import_test\n");
    printf("*------ Test No. 04\n");
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

void icum_hmac_import_test5(){
    
    r_icumif_isd_t     *p_ISD;
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
    p_ISD->p_callbackfunc                       = 0;
    p_ISD->job_id                               = 0;
    p_ISD->prm.HMAC_IMPORT.key_id               = 0;
    p_ISD->prm.HMAC_IMPORT.p_key                = hmac_key5;
    p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = 40;
    
    printf("No5. hmac_import_test\n");
    printf("*------ Test No. 05\n");
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

void icum_hmac_import_test6(){
    
    r_icumif_isd_t     *p_ISD;
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
    p_ISD->p_callbackfunc                       = 0;
    p_ISD->job_id                               = 0;
    p_ISD->prm.HMAC_IMPORT.key_id               = 1;
    p_ISD->prm.HMAC_IMPORT.p_key                = hmac_key6;
    p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = 40;
    
    printf("No6. hmac_import_test\n");
    printf("*------ Test No. 06\n");
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

void icum_hmac_import_test7(){
    
    r_icumif_isd_t     *p_ISD;
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
    p_ISD->p_callbackfunc                       = 0;
    p_ISD->job_id                               = 0;
    p_ISD->prm.HMAC_IMPORT.key_id               = 2;
    p_ISD->prm.HMAC_IMPORT.p_key                = hmac_key7;
    p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = 50;
    
    printf("No7. hmac_import_test\n");
    printf("*------ Test No. 07\n");
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

void icum_hmac_import_test8(){
    
    r_icumif_isd_t     *p_ISD;
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
    p_ISD->p_callbackfunc                       = 0;
    p_ISD->job_id                               = 0;
    p_ISD->prm.HMAC_IMPORT.key_id               = 3;
    p_ISD->prm.HMAC_IMPORT.p_key                = hmac_key8;
    p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = 50;
    
    printf("No8. hmac_import_test\n");
    printf("*------ Test No. 08\n");
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

void icum_hmac_import_test9(){
    
    r_icumif_isd_t     *p_ISD;
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
    p_ISD->p_callbackfunc                       = 0;
    p_ISD->job_id                               = 0;
    p_ISD->prm.HMAC_IMPORT.key_id               = 0;
    p_ISD->prm.HMAC_IMPORT.p_key                = hmac_key9;
    p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = 20;
    
    printf("No9. hmac_import_test\n");
    printf("*------ Test No. 09\n");
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

void icum_hmac_import_test10(){
    
    r_icumif_isd_t     *p_ISD;
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
    p_ISD->p_callbackfunc                       = 0;
    p_ISD->job_id                               = 0;
    p_ISD->prm.HMAC_IMPORT.key_id               = 1;
    p_ISD->prm.HMAC_IMPORT.p_key                = hmac_key10;
    p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = 25;
    
    printf("No10. hmac_import_test\n");
    printf("*------ Test No. 10\n");
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

void icum_hmac_import_test11(){
    
    r_icumif_isd_t     *p_ISD;
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
    p_ISD->p_callbackfunc                       = 0;
    p_ISD->job_id                               = 0;
    p_ISD->prm.HMAC_IMPORT.key_id               = 4;
    p_ISD->prm.HMAC_IMPORT.p_key                = hmac_key1;
    p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = 10;
    
    printf("No11. hmac_import_test\n");
    printf("*------ Test No. 11\n");
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

void icum_hmac_import_test12(){
    
    uint8_t hmac_error_key[]= {
    0x82, 0xf3, 0xb6, 0x9a, 0x1b, 0xff, 0x4d, 0xe1, 0x5c, 0x33
    };
    
    r_icumif_isd_t     *p_ISD;
    
    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);
    
    p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
    p_ISD->p_callbackfunc                       = 0;
    p_ISD->job_id                               = 0;
    p_ISD->prm.HMAC_IMPORT.key_id               = 0;
    p_ISD->prm.HMAC_IMPORT.p_key                = hmac_error_key;
    p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = 10;
    
    printf("No12. hmac_import_test\n");
    printf("*------ Test No. 12\n");
    
    vrun_chrono_start();
    trigger_service_and_wait(p_ISD);
    vrun_chrono_stop();
    
    printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
}

void icum_hmac_import_test13(){
    
    r_icumif_isd_t     *p_ISD;
    unsigned int        idx, k_size[] = {0, 129};

    for (idx = 0; idx < 2; idx++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);

        p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
        p_ISD->p_callbackfunc                       = 0;
        p_ISD->job_id                               = 0;
        p_ISD->prm.HMAC_IMPORT.key_id               = 0;
        p_ISD->prm.HMAC_IMPORT.p_key                = hmac_key1;
        p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = k_size[idx];    //0;

        printf("No13. hmac_import_test : key size = %d\n", k_size[idx]);
        printf("*------ Test No. %02d\n", (13 + idx));

        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}


void icum_hmac_import_test14(){
    
    r_icumif_isd_t     *p_ISD;
    unsigned int        ptr_key[] = {ADDR_LOWER_CRAM, ADDR_UPPER_CRAM-9, ADDR_LOWER_CRAM+100, ADDR_UPPER_CRAM-8};
    unsigned int        idx;

    printf("No14. hmac_import_test\n");

    for (idx = 0; idx < 4; idx++)
    {
        p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
        memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER);

        p_ISD->service_id                           = SERVICE_03_HMAC_IMPORT;
        p_ISD->p_callbackfunc                       = 0;
        p_ISD->job_id                               = 0;
        p_ISD->prm.HMAC_IMPORT.key_id               = 0;
        p_ISD->prm.HMAC_IMPORT.p_key                = (void *)ptr_key[idx];
        p_ISD->prm.HMAC_IMPORT.key_size_in_bytes    = 10;

        printf("*------ Test No. %02d\n", (15 + idx));
        printf("[Count %d] : p_key %x\n", (idx + 1), ptr_key[idx]);

        vrun_chrono_start();
        trigger_service_and_wait(p_ISD);
        vrun_chrono_stop();

        printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD->service_result));
    }
}

void icum_hmac_import_test15(){

    r_icumif_isd_t     *p_ISD;
    r_icumif_isd_t     *p_ISD2;

    p_ISD = (r_icumif_isd_t *)ISD_BUFFER;
    p_ISD2  = p_ISD + 1;
    memset(ISD_BUFFER, 0, SIZE_OF_ISD_BUFFER * 2);


    p_ISD->service_id                           = SERVICE_01_HMAC;
    p_ISD->p_callbackfunc                       = 0;
    p_ISD->job_id                               = 0;
    p_ISD->prm.HMAC.key_id                      = 0;
    p_ISD->prm.HMAC.hmac_operation              = HMAC_GENERATION;
    p_ISD->prm.HMAC.hash_primitive              = HASH_PRIMITIVE_SHA1;
    p_ISD->prm.HMAC.p_msg                       = p_msg1;
    p_ISD->prm.HMAC.msg_size_in_bytes           = 128;
    p_ISD->prm.HMAC.hmac_size_in_bytes          = 10;
    p_ISD->prm.HMAC.job_slice                   = 2;
    p_ISD->prm.HMAC.p_hmac                      = BIG_BUFFER[0];

    p_ISD2->service_id                          = SERVICE_03_HMAC_IMPORT;
    p_ISD2->p_callbackfunc                      = 0;
    p_ISD2->job_id                              = 0;
    p_ISD2->prm.HMAC_IMPORT.key_id              = 0;
    p_ISD2->prm.HMAC_IMPORT.p_key               = hmac_key1;
    p_ISD2->prm.HMAC_IMPORT.key_size_in_bytes   = 10;

    printf("No15. hmac_import_test\n");

    printf("\n*------ Test No. 19\n");

    vrun_chrono_start();
    trigger_2service_and_wait(p_ISD, p_ISD2);
    vrun_chrono_stop();

        printf("result: %s\n\n",R_ICUMIF_GetErrorCodeName(p_ISD2->service_result));

}
