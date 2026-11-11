/******************************************************************************/
/* Component Name   ICU-M Interface Library (ICUMIF)                          */
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

#if !defined(R_ICUMIF_PUB_H)
#define R_ICUMIF_PUB_H

/**************************************************************************************/
/* Return Value Definition                                                            */
/**************************************************************************************/
/*===========================================================================*/
/* Definition of return value of ICU-M interface primitive                   */
/*===========================================================================*/
#define R_ICUMIF_ER_OK                      (0)         /* Successful completion                   */
#define R_ICUMIF_ER_PARAM_ALIGNMENT         (-1)        /* Parameter alignment error               */
#define R_ICUMIF_ER_PARAM_RANGE             (-2)        /* Parameter range error                   */
#define R_ICUMIF_ER_ADDR_NOT_G_RAM          (-3)        /* Address not within global RAM area    */
#define R_ICUMIF_ER_IF_LIB_NOT_INIT         (-4)        /* ICU-M interface library not initialized */
#define R_ICUMIF_ER_REQ_QUEUE_FULL          (-5)        /* Service request queue FULL              */

#define R_ICUMIF_ER_SOFTWARE_ERROR          (-99)       /* Software error                          */

/* Used only by R_ICUMIF_IsServiceCompleted */
#define R_ICUMIF_RTN_SERV_RUNNING           (0)         /* Service running                         */
#define R_ICUMIF_RTN_SERV_COMPLETION        (1)         /* Service execution completion            */

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* None */


/**************************************************************************************/
/* Macros                                                                             */
/**************************************************************************************/
/* None */


/**************************************************************************************/
/* Type definition                                                                    */
/**************************************************************************************/
/* None */


/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/*===========================================================================*/
/* Enumerates: Flash type to processed                                       */
/*===========================================================================*/

typedef enum
{
    E_ICUMIF_FLASH_TYPE_DF      = 1,            /* Data Flash   */
    E_ICUMIF_FLASH_TYPE_RAM,                    /* Shift to RAM */
    E_ICUMIF_FLASH_TYPE_DF_PRG,                 /* Data Flash Programming */
    E_ICUMIF_FLASH_TYPE_CF_PRG,                 /* Code Flash Programming */

    LAST_ITEM_FLASH_TYPE = 0x01000000
} r_icumif_flash_type_t;

/*===========================================================================*/
/* Enumerates: Flash access status                                           */
/*===========================================================================*/

typedef enum
{
    E_ICUMIF_ACCESS_OFF     = 0,                /* Non-accessing */
    E_ICUMIF_ACCESS_ON,                         /* Accessing     */

    LAST_ITEM_ACCESS_STATUS = 0x01000000
} r_icumif_access_status_t;

/*===========================================================================*/
/* Enumerates: Flash related interrupt Notification type                     */
/*===========================================================================*/

typedef enum
{
    E_ICUMIF_NOTIFY_CF_PRG_ACK      = 2,        /* Code Flash mode switching ACK notification  */
    E_ICUMIF_NOTIFY_DF_PRG_ACK      = 3,        /* Data Flash mode switching ACK notification  */
    E_ICUMIF_NOTIFY_CF_PRG_NACK     = 4,        /* Code Flash mode switching NACK notification */
    E_ICUMIF_NOTIFY_DF_PRG_NACK     = 5,        /* Data Flash mode switching NACK notification */
    E_ICUMIF_NOTIFY_OPERATION_END   = 6,        /* Flash access completion notification        */

    LAST_ITEM_NOTIFY_TYPE = 0x01000000
} r_icumif_notify_type_t;


/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/*===========================================================================*/
/* Type definition: Registration information of call back function           */
/*===========================================================================*/

typedef struct R_ICUMIF_CB_REGIST
{
    void    (*p_func_addr)(void);
} R_ICUMIF_CB_REGIST_t;

/**********************************************************************************/
/* Prototypes                                                                     */
/**********************************************************************************/

int32_t R_ICUMIF_Init(uint32_t *p_request_queue);
int32_t R_ICUMIF_ServiceRequest(r_icumif_isd_t *p_ISD);
int32_t R_ICUMIF_ServiceResponse(void);
int32_t R_ICUMIF_IsServiceCompleted(r_icumif_isd_t *p_ISD);
r_icumif_sts_t *R_ICUMIF_GetStatus(void);

int32_t R_ICUMIF_SetFlashAccessStatus(r_icumif_flash_type_t flash_type,
                                      r_icumif_access_status_t status);
int32_t R_ICUMIF_NotifyFlashInterrupt(r_icumif_notify_type_t notify_type);

int32_t R_ICUMIF_SetSystemCallBackFunc(R_ICUMIF_CB_REGIST_t *p_regist_info);
int32_t R_ICUMIF_SetFlashCallBackFunc(R_ICUMIF_CB_REGIST_t *p_regist_info);

void R_ICUMIF_IRQ_Handler(void);

#endif /* #if !defined(R_ICUMIF_PUB_H) */
