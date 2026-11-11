/******************************************************************************/
/* Component Name   ICU-M COMM(PE) Driver                                     */
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

#if !defined(ICUM_D_COMM_PE_H)
#define ICUM_D_COMM_PE_H

/**************************************************************************************/
/* Global Pointer Variable                                                            */
/**************************************************************************************/

/*====================================================================================*/
/* Shared RAM area address                                                            */
/*====================================================================================*/
extern uint8_t  __ghsbegin_start_gRAM_BANK_A[];         /* Start address ( Bank_A )   */
extern uint8_t  __ghsbegin_end_gRAM_BANK_A[];           /* End address + 1 ( Bank_A ) */

extern uint8_t  __ghsbegin_start_gRAM_MEV[];            /* Start addr ( Exclusion )   */

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/

/* ICUMD Command Register */
#if defined(__RH850_U2C4__)
#include DEVICE_FILE
#define ICUM_CMDREGICUM_SEMAPHORE ICUM_CMDREGICUM_SEMAPE00
#define ICUM_CMDREGICUM_PE2ICUIE ICUM_CMDREGICUM_PE2ICUIE0
#else
#define ICUM_CMDREGICUM_ICU2PES0        (*(volatile uint32_t*)0xFF1F0010UL)
#define ICUM_CMDREGICUM_PE2ICUS0        (*(volatile uint32_t*)0xFF1F0014UL)
#define ICUM_CMDREGICUM_PE2ICUFS0       (*(volatile uint32_t*)0xFF1F0044UL)
#define ICUM_CMDREGICUM_PE2ICUIE        (*(volatile uint32_t*)0xFF1F004CUL)
#define ICUM_CMDREGICUM_ICU2PEF0        (*(volatile uint32_t*)0xFF1F0050UL)
#define ICUM_CMDREGICUM_ICU2PEFC0       (*(volatile uint32_t*)0xFF1F0058UL)
#define ICUM_CMDREGICUM_ICU2PEIE0       (*(volatile uint32_t*)0xFF1F005CUL)
#define ICUM_CMDREGICUM_PE2ICUSFS       (*(volatile uint32_t*)0xFF1F0074UL)
#define ICUM_CMDREGICUM_SEMAPHORE       (*(volatile uint32_t*)0xFF1F00A0UL)
#define ICUM_CMDREGICUM_PE2ICUFS1       (*(volatile uint32_t*)0xFF1F0104UL)
#define ICUM_CMDREGICUM_ICU2PEF1        (*(volatile uint32_t*)0xFF1F0110UL)
#define ICUM_CMDREGICUM_ICU2PEFC1       (*(volatile uint32_t*)0xFF1F0118UL)
#define ICUM_CMDREGICUM_ICU2PEIE1       (*(volatile uint32_t*)0xFF1F011CUL)
#define ICUM_CMDREGICUM_ICU2PES1        (*(volatile uint32_t*)0xFF1F0128UL)
#define ICUM_CMDREGICUM_PE2ICUS1        (*(volatile uint32_t*)0xFF1F012CUL)
#endif
#define ICUM_CMDREGICUM_PE2ICUFS2       (*(volatile uint32_t*)0xFF1F0134UL)
#define ICUM_CMDREGICUM_ICU2PEF2        (*(volatile uint32_t*)0xFF1F0140UL)
#define ICUM_CMDREGICUM_ICU2PEFC2       (*(volatile uint32_t*)0xFF1F0148UL)
#define ICUM_CMDREGICUM_ICU2PEIE2       (*(volatile uint32_t*)0xFF1F014CUL)
#define ICUM_CMDREGICUM_ICU2PES2        (*(volatile uint32_t*)0xFF1F0158UL)
#define ICUM_CMDREGICUM_PE2ICUS2        (*(volatile uint32_t*)0xFF1F015CUL)

/* Device definition ( MFIS ) */
#define MFIS_ERROR_STATUS_REG_8     ((volatile uint32_t *)0xE6260270UL)

/* Bit pattern of MFIS Error interrupt to receive ( MFIS_ERROR_INT_FROM_MODULE ) */
#define MFIS_ERR_MODULE_WDT_OVERFLOW    (0x00100000UL)

/* Upper limit of repetitions for clear interrupt factor of MFIS error interrupt */
#define MFIS_ERR_CLEAR_COUNT_MAX        (0x0007FFFFUL)

/* FLMD Register */
#if !defined(__RH850_U2C4__)
#define FLMDFPLOCK                      (*(volatile uint32_t*)0xFFA0000CUL)
#endif

/* EI Level Interrupt Control Register */
#define INTC1CPU0EIC0                   (*(volatile uint32_t*)0xFFFC4000UL)
#define INTC1CPU1EIC0                   (*(volatile uint32_t*)0xFFFC8000UL)
#define INTC1CPU2EIC0                   (*(volatile uint32_t*)0xFFFCC000UL)
#define INTC1CPU3EIC0                   (*(volatile uint32_t*)0xFFFD0000UL)

/* Bit position of exclusive control register ( MUTEX_TYPE ) */
#define SERV_REQUEST_QUEUE_MUTEX        (1U)
#define INIT_STATUS_INFO_MUTEX          (2U)
#define PE_STATUS_INFO_MUTEX            (3U)
#define SERV_RESPONSE_QUEUE_MUTEX       (4U)

/* Bit position of initialization process execution state ( INIT_PROCESS_TYPE ) */
#define PE0_QUE_INIT                    (0U)        /* PE0 queue init                 */
#define PE1_QUE_INIT                    (1U)        /* PE1 queue init                 */
#define PE2_QUE_INIT                    (2U)        /* PE2 queue init                 */
#define PE0_INTC_INIT                   (6U)        /* PE0 INTC initialization        */
#define PE1_INTC_INIT                   (7U)        /* PE1 INTC initialization        */
#define PE2_INTC_INIT                   (8U)        /* PE2 INTC initialization        */
#define PE0_ICU2PEI_INIT                (12U)       /* PE0 ICU2PEI initialization     */
#define PE1_ICU2PEI_INIT                (13U)       /* PE1 ICU2PEI initialization     */
#define PE2_ICU2PEI_INIT                (14U)       /* PE2 ICU2PEI initialization     */

/* Execution state of initialization process ( INIT_PROCESS_STATUS ) */
#define INIT_UNPROCESSED                (0UL)
#define INIT_PROCESSED                  (1UL)

/* Mask pattern of service queue address ( QUE_ADDR_MASK_PATTERN ) */
#define QUE_ADDR_LOWER_24BITS           (0x00FFFFFFUL)
#define QUE_ADDR_UPPER_8BITS_VALUE      (0xFE000000UL)

/* Channel number of INTC EIC register ( INTC_EIC_CHANNEL ) */
#define INTC_EIC_CH_INTICU2PE           (21u)    /* For service completion on PE side */

/* Set value of INTC EIC register ( INTC_EIC_SET_VALUE ) */
#define INTC_EIC_CT_EDGE                (0x0000U)       /* Detection with an edge     */
#define INTC_EIC_CT_LEVEL               (0x8000U)       /* Detection of the level     */
#define INTC_EIC_RF_NO_INT              (0x0000U)       /* No interrupt request       */
#define INTC_EIC_RF_INT_REQ             (0x1000U)       /* Interrupt request present  */
#define INTC_EIC_MK_INT_ENABLE          (0x0000U)       /* Interrupt is enabled       */
#define INTC_EIC_MK_INT_DISABLE         (0x0080U)       /* Interrupt is disabled      */
#define INTC_EIC_VM_DIRECT              (0x0000U)       /* Direct vector method       */
#define INTC_EIC_VM_TABLE               (0x0040U)       /* Table reference method     */
#define INTC_EIC_INT_PRI_0              (0x0000U)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_1              (0x0001U)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_2              (0x0002U)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_3              (0x0003U)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_4              (0x0004U)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_5              (0x0005U)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_6              (0x0006U)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_7              (0x0007U)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_8              (0x0008U)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_9              (0x0009U)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_10             (0x000AU)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_11             (0x000BU)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_12             (0x000CU)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_13             (0x000DU)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_14             (0x000EU)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_15             (0x000FU)       /* Interrupt priority levels  */
#define INTC_EIC_INT_PRI_HIGHEST        INTC_EIC_INT_PRI_0      /* Highest priority   */
#define INTC_EIC_INT_PRI_LOWEST         INTC_EIC_INT_PRI_15     /* Lowest priority    */

/* Destination PE set value of INTICUP interrupt ( INTICUP_PESEL_PATTERN ) */
#define INTICUP_PESEL_PE0_MASK          (0xFFFFFFF0UL)    /* Mask pattern (bit 3-0)   */
#define INTICUP_PESEL_TO_PE0            (0x00000000UL)    /* Set value (bit 3-0)      */
#define INTICUP_PESEL_PE1_MASK          (0xFFFFFF0FUL)    /* Mask pattern (bit 7-4)   */
#define INTICUP_PESEL_TO_PE1            (0x00000010UL)    /* Set value (bit 7-4)      */
#define INTICUP_PESEL_PE2_MASK          (0xFFFFF0FFUL)    /* Mask pattern (bit 11-8)  */
#define INTICUP_PESEL_TO_PE2            (0x00000200UL)    /* Set value (bit 11-8)     */

#define INTICUP_PESEL_NO_PE_MASK        (INTICUP_PESEL_PE0_MASK & \
                                         INTICUP_PESEL_PE1_MASK & \
                                         INTICUP_PESEL_PE2_MASK)

/* Bit position of INTICUP interrupt each PE ( INTICUP_BIT_POSITION ) */
#define INTICUP_PE0_POSITION            (0U)
#define INTICUP_PE1_POSITION            (4U)
#define INTICUP_PE2_POSITION            (8U)
#define INTICUP_EACH_PE_BIT_WIDTH       (4U)

/* PE Identifier value ( PEID_VALUE ) */
#define PEID_PE0                        (0U)
#define PEID_PE1                        (1U)
#define PEID_PE2                        (2U)

#define PEID_MASK                       (0x0000007UL)

/* Bit position of PE status information ( PE2ICUS_BIT_POSITION ) */
#define INFO_PE_ST_POSITION             (24U)

/* Bit pattern of PE status information ( INFO_PE_ST_BIT_PATTERN ) */
#define INFO_PE_ST_FACIACCDIS_BIT       (0x00000001UL)
#define INFO_PE_ST_FLS_OPRN_BIT         (0x00000001UL)
#define INFO_PE_ST_FLS_DF_PRG_BIT       (0x00000001UL)
#define INFO_PE_ST_FLS_CF_PRG_BIT       (0x00000001UL)

/* Section address information ( SECTION_ADDRESS_INFO ) */
#define ADDR_START_SHARED_RAM           ((uint32_t *)((void *)__ghsbegin_start_gRAM_BANK_A))
#define ADDR_END_PLUS1_SHARED_RAM       ((uint32_t *)((void *)__ghsbegin_end_gRAM_BANK_A))

#define PE_SEMAPHORE_RES              (0x01000000UL)
#define PE_SEMAPHORE_REQ              (0x02000000UL)
#define PE_SEMAPHORE_ERR              (0x20000000UL)
#define PE_SEMAPHORE_AUTH_MASK        (0xC0000000UL)
#define PE_SEMAPHORE_AUTH_FREE        (0x00000000UL)

#define PE_SEMAPHORE_AUTH_GET1ST      (0x00000000UL)
#define PE_SEMAPHORE_AUTH_GET2ND      (0x80000000UL)
#define PE_SEMAPHORE_WAIT             (0x0003A000UL)

#define PE_SEMAPHORE_ER_SYSTEM        (0x00000001UL)
#define PE_SEMAPHORE_ER_TIMEOUT       (0x00000002UL)

#define PE_SEMAPHORE_ON               (0x01U)
#define PE_SEMAPHORE_OFF              (0x00U)

#define QUEUE_INDEX_FIRST               (0x00U)
#define QUEUE_INDEX_MAX                 (0xFFU)

/* Size of service queue area ( SERVICE_QUEUE_AREA_SIZE ) */
#define SERV_REQUEST_QUEUE_AREA_SIZE    (16U)
#define SERV_RESPONSE_QUEUE_AREA_SIZE   (16U)

#define TOTAL_SERVICE_QUEUE_AREA_SIZE   (SERV_REQUEST_QUEUE_AREA_SIZE + SERV_RESPONSE_QUEUE_AREA_SIZE)

/* Bit pattern of FLMDFPLOCK Register */
#define FLMD_FACI0_LOCK               (0x00000001UL)


/* Value for RAM area check ( CHECK_VALUE ) */
#define VALUE_FOR_RAM_AREA_CHECK        (0xA5A5A5A5UL)

/*  System register definition */
/* interrupt information bit of PSW register */
#define PSW         5, 0

/**************************************************************************************/
/* Type definition                                                                    */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Enumeration                                                                        */
/**************************************************************************************/
/* None */

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
typedef struct ICUM_COMM_PE_INFO
{
    volatile uint32_t*  intc1cpueic0;
    uint32_t            inticup_pesel_pe_mask;
    uint32_t            inticup_pesel_to_pe;
    uint32_t            inticup_reqflg;
    uint8_t             pe_que_init;
    uint8_t             pe_intc_init;
    uint8_t             pe_icu2pei_init;
    uint8_t             inticup_pe_position;
} ICUM_COMM_PE_INFO_t;

/*====================================================================================*/
/* Union for pointer manipulation                                                     */
/*====================================================================================*/
typedef union ptr_manip
{
    void                *p_ptr;
    uint32_t            *p_uint32_ptr;
    volatile uint32_t   *p_vlt_ptr;
    uint32_t            val;
} ptr_manip_t;

/***************************************************************************************/
/* Prototypes                                                                          */
/***************************************************************************************/
static uint32_t d_COMM_PE_GetSemaphore(uint8_t target_pe, uint8_t request);
static uint32_t d_COMM_PE_FreeSemaphore(uint8_t target_pe, uint8_t response);

/**************************************************************************************/
/* ASM Macros                                                                         */
/**************************************************************************************/
/* ASM macro */
#define ASM_MACRO(x)   __asm(x)

/* Prototypes */
static void PE_DISABLE_INTERRUPT(void);
static void PE_ENABLE_INTERRUPT(void);
static uint32_t PE_STSR(int8_t regID, int8_t selID);
static void D_COMM_SYNC_PE(void);

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0020]:[ICUMFW_UD_S4_D_COMM_PE_0020]
* Function name: PE_DISABLE_INTERRUPT
* Description  : Disable EI Level Maskable Interrupt
* Arguments    : None
*                
* Return Value : None
***************************************************************************************/
static void PE_DISABLE_INTERRUPT(void)
{
    __asm("di");
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0021]:[ICUMFW_UD_S4_D_COMM_PE_0021]
* Function name: PE_ENABLE_INTERRUPT
* Description  : Enable EI Level Maskable Interrupt
* Arguments    : None
*                
* Return Value : None
***************************************************************************************/
static void PE_ENABLE_INTERRUPT(void)
{
    __asm("ei");
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0022]:[ICUMFW_UD_S4_D_COMM_PE_0022]
* Function name: PE_STSR
* Description  : Get Data from Basic System Register
* Arguments    : regID     : Register ID
*                selID     : Selection ID
* Return Value : Reading Data
***************************************************************************************/
static asm uint32_t PE_STSR(int8_t regID, int8_t selID)
{
%con regID, selID
    stsr    regID, r10, selID
%error
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0023]:[ICUMFW_UD_S4_D_COMM_PE_0023]
* Function name: D_COMM_PE_DIS_INT_CHECK
* Description  : Check ID bit and Set Disable Interrupt
* Arguments    : Output : X = PSW Register Value before executing this function
* Return Value : None
***************************************************************************************/
#define D_COMM_PE_DIS_INT_CHECK(X) \
    do\
    {\
        (X) = PE_STSR(PSW);\
        if (0UL == ((X) & 0x20UL))\
        {\
            PE_DISABLE_INTERRUPT();\
        }\
        else\
        {\
        }\
    } while (0)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0024]:[ICUMFW_UD_S4_D_COMM_PE_0024]
* Function name: D_COMM_PE_EN_INT_CHECK
* Description  : Check old ID bit and Set Enable Interrupt
* Arguments    : Input : X = PSW Register Value gotten
*                                       by DISABLE_INTERRUPT_WITH_CHECK Macro
* Return Value : None
***************************************************************************************/
#define D_COMM_PE_EN_INT_CHECK(X) \
    do\
    {\
        if (0UL == ((X) & 0x20UL))\
        {\
            PE_ENABLE_INTERRUPT();\
        }\
        else\
        {\
        }\
    } while (0)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_S4_D_COMM_PE_0025]:[ICUMFW_UD_S4_D_COMM_PE_0025]
* Function name: D_COMM_SYNC_PE
* Description  : Synchronize Pipeline
* Arguments    : None
*                
* Return Value : None
***************************************************************************************/
static void D_COMM_SYNC_PE(void)
{
    __asm("syncp");
}

#endif /* #if !defined(ICUM_D_COMM_PE_H) */

