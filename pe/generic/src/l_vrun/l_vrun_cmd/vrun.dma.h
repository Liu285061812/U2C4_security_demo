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

#define vrun_dma_nbchannels         sys_dma_nbchannels
#define vrun_dma_config_addr        sys_dma_config_addr
#define vrun_dma_config_tr          sys_dma_config_tr
#define vrun_dma_set_counter        sys_dma_set_counter
#define vrun_dma_enable_tr          sys_dma_enable_tr
#define vrun_dma_resume_tr          sys_dma_resume
#define vrun_dma_abort_tr           sys_dma_abort
#define vrun_dma_force_start_tr     sys_dma_force_start
#define vrun_dma_tr_completed       sys_dma_tr_completed
#define vrun_dma_wait_completion    sys_dma_wait_completion
#define vrun_dma_set_irq            sys_dma_set_irq
#define vrun_dma_set_isr            sys_dma_mask_isr

unsigned char sys_dma_nbchannels(void);
void sys_dma_config_addr(unsigned char channel, unsigned long addr_src, unsigned long addr_dest);
void sys_dma_config_tr(unsigned char channel, unsigned short tr_config, unsigned short trig_src);
void sys_dma_set_counter(unsigned char channel, unsigned short nb_transfer);
void sys_dma_enable_tr(unsigned char channel);
void sys_dma_resume(unsigned char channel);
void sys_dma_abort(unsigned char channel);
void sys_dma_force_start(unsigned char channel);
unsigned char sys_dma_tr_completed(unsigned char channel);
void sys_dma_wait_completion(unsigned char channel);
void sys_dma_set_irq(unsigned char channel, unsigned char irq_value);
void sys_dma_mask_isr(unsigned char channel, unsigned char isr_mask);

#define DMA_TRIGSRC_INTLVIL            0x1
#define DMA_TRIGSRC_INTP0              0x2
#define DMA_TRIGSRC_INTP1              0x3
#define DMA_TRIGSRC_INTP2              0x4
#define DMA_TRIGSRC_INTP3              0x5
#define DMA_TRIGSRC_INTP4              0x6
#define DMA_TRIGSRC_INTP5              0x7
#define DMA_TRIGSRC_INTP6              0x8
#define DMA_TRIGSRC_INTP7              0x9
#define DMA_TRIGSRC_INTTAB0OV          0xA
#define DMA_TRIGSRC_INTTAB0CC0         0xB
#define DMA_TRIGSRC_INTTAB0CC1         0xC
#define DMA_TRIGSRC_INTTAB0CC2         0xD
#define DMA_TRIGSRC_INTTAB0CC3         0xE
#define DMA_TRIGSRC_INTTAA0OV          0xF
#define DMA_TRIGSRC_INTTAA0CC0         0x10
#define DMA_TRIGSRC_INTTAA0CC1         0x11
#define DMA_TRIGSRC_INTTAA1OV          0x12
#define DMA_TRIGSRC_INTTAA1CC0         0x13
#define DMA_TRIGSRC_INTTAA1CC1         0x14
#define DMA_TRIGSRC_INTTAA2OV          0x15
#define DMA_TRIGSRC_INTTAA2CC0         0x16
#define DMA_TRIGSRC_INTTAA2CC1         0x17
#define DMA_TRIGSRC_INTTAA3OV          0x18
#define DMA_TRIGSRC_INTTAA3CC0         0x19
#define DMA_TRIGSRC_INTTAA3CC1         0x1A
#define DMA_TRIGSRC_INTTM0EQ0          0x1B
#define DMA_TRIGSRC_INTCB0R            0x1C
#define DMA_TRIGSRC_INTCB0T            0x1D
#define DMA_TRIGSRC_INTCB1R            0x1E
#define DMA_TRIGSRC_INTCB1T            0x1F
#define DMA_TRIGSRC_INTUD0R            0x20
#define DMA_TRIGSRC_INTUD0T            0x21
#define DMA_TRIGSRC_INTUD1R            0x22
#define DMA_TRIGSRC_INTUD1T            0x23
#define DMA_TRIGSRC_INTAD              0x24
#define DMA_TRIGSRC_INTTAA4OV          0x25
#define DMA_TRIGSRC_INTTAA4CC0         0x26
#define DMA_TRIGSRC_INTTAA4CC1         0x27
#define DMA_TRIGSRC_INTIIC0            0x28
#define DMA_TRIGSRC_INTKR              0x29
#define DMA_TRIGSRC_INTTAB1OV          0x2A
#define DMA_TRIGSRC_INTTAB1CC0         0x2B
#define DMA_TRIGSRC_INTTAB1CC1         0x2C
#define DMA_TRIGSRC_INTTAB1CC2         0x2D
#define DMA_TRIGSRC_INTTAB1CC3         0x2E
#define DMA_TRIGSRC_INTUD2R            0x2F
#define DMA_TRIGSRC_INTUD2T            0x30
#define DMA_TRIGSRC_INTLVIH            0x31
#define DMA_TRIGSRC_INTUD3R            0x32
#define DMA_TRIGSRC_INTUD3T            0x33
#define DMA_TRIGSRC_INTTAB2OV          0x34
#define DMA_TRIGSRC_INTTAB2CC0         0x35
#define DMA_TRIGSRC_INTTAB2CC1         0x36
#define DMA_TRIGSRC_INTTAB2CC2         0x37
#define DMA_TRIGSRC_INTTAB2CC3         0x38
#define DMA_TRIGSRC_INTCB2R            0x39
#define DMA_TRIGSRC_INTCB2T            0x3A
#define DMA_TRIGSRC_INTUD4R            0x3B
#define DMA_TRIGSRC_INTUD4T            0x3C
#define DMA_TRIGSRC_INTUD5R            0x3D
#define DMA_TRIGSRC_INTUD5T            0x3E
#define DMA_TRIGSRC_INTAD1             0x3F

#define DMA_CONFIG_TRSIZE_8BITS        0x0000
#define DMA_CONFIG_TRSIZE_16BITS       0x4000
#define DMA_CONFIG_INCREMENT_SRC       0x0000
#define DMA_CONFIG_DECREMENT_SRC       0x0040
#define DMA_CONFIG_FIXED_SRC           0x0080
#define DMA_CONFIG_INCREMENT_DEST      0x0000
#define DMA_CONFIG_DECREMENT_DEST      0x0010
#define DMA_CONFIG_FIXED_DEST          0x0020
