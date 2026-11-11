/******************************************************************************/
/* Component Name   shared.ld                                                 */
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

MEMORY
{
  iROM_BANK_A     : ORIGIN = 0x00000000,  LENGTH = 2M                     // Code Flash Bank A
  iROM_BANK_B     : ORIGIN = 0x00400000,  LENGTH = 2M                     // Code Flash Bank B
  iROM_end        : ORIGIN = .         ,  LENGTH = 0                      // End of internal ROM
  iROM_BANK_S     : ORIGIN = 0x08000000,  LENGTH = 64k                    // Extended user area (User boot area 0)
  iROM_BANK_T     : ORIGIN = 0x08400000,  LENGTH = 64k                    // Extended user area (User boot area 1)
  iRAM_PE2        : ORIGIN = 0xFDA00000,  LENGTH = 128k                   // PE2 RAM
  iRAM_PE1        : ORIGIN = 0xFDC00000,  LENGTH = 128k                   // PE1 RAM
  iRAM_end        : ORIGIN = .         ,  LENGTH = 0                      // End of internal RAM
  iRAM_Self       : ORIGIN = 0xFDE00000,  LENGTH = 128k                   // Self RAM PE1 to PE2
  iRAM_Self_end   : ORIGIN = .         ,  LENGTH = 0                      // End of Self RAM

  gRAM_BANK_A     : ORIGIN = 0xFE000000,  LENGTH = 158k                   // Cluster RAM Bank A
  gRAM_BANK_A_ed  : ORIGIN = .         ,  LENGTH = 0                      // End of Cluster RAM Bank A
  gRAM_MEV        : ORIGIN = 0xFE027800,  LENGTH = 2k                     // Cluster RAM for Mutual Exclusion
  gRAM_MEV_end    : ORIGIN = .         ,  LENGTH = 0                      // End of Cluster RAM for Mutual Exclusion
  gRAM_PE1        : ORIGIN = 0xFE028000,  LENGTH = 64k                    // Cluster RAM for PE1
  gRAM_PE1_end    : ORIGIN = .         ,  LENGTH = 0                      // End of Cluster RAM for PE1
  gRAM_PE2        : ORIGIN = 0xFE038000,  LENGTH = 64k                    // Cluster RAM for PE2
  gRAM_PE2_end    : ORIGIN = .         ,  LENGTH = 0                      // End of Cluster RAM for PE2
  gRAM_BANK_A_end : ORIGIN = .         ,  LENGTH = 0                      // End of Cluster RAM Bank A
  rRAM            : ORIGIN = 0x12000000,  LENGTH = 32k                    // Retention RAM

  DF_BANK_A       : ORIGIN = 0xFF200000,  LENGTH = 128k                   // Data Flash Bank A
  DF_BANK_B       : ORIGIN = 0xFF220000,  LENGTH = 128k                   // Data Flash Bank B
  DF_BANK_ICUM    : ORIGIN = 0xFF280000,  LENGTH = 64k                    // ICUM Data Flash 

  DF_end          : ORIGIN = .         ,  LENGTH = 0                      // End of internal Data Flash
  
  /* Hardware Property Area */
  DFHPA0EDA  : ORIGIN = 0xFF320000,  LENGTH = 2K
  DFHPA0CSA  : ORIGIN = 0xFF320800,  LENGTH = 4K
  DFHPA0SSA  : ORIGIN = 0xFF321800,  LENGTH = 4K
  DFHPA0BPA0 : ORIGIN = 0xFF322800,  LENGTH = 4K
  DFHPA0ECA0 : ORIGIN = 0xFF325000,  LENGTH = 6K
  DFHPA0ECA1 : ORIGIN = 0xFF326800,  LENGTH = 6K
}

CONSTANTS
{
  size_exec_ram   = 10k
}

// ----------------------------------------------------------------------
// ROM
// ----------------------------------------------------------------------
SECTIONS
{
  .start.ROM addr(iROM_BANK_S)          :>iROM_BANK_S

  .intvect0  align(4)                   :>.          /* common reset vector */

  .intvect   align(512)                 :>.          /* fatal & user exceptions */
             
  .rozdata   align(4)                   :>.          /* constant datas in ZDA area */
  .robase    align(4)                   :>.          /* initialize textpointer TP for SDA addressing */
  .rosdata   align(4)                   :>.          /* constant datas in SDA area */
  .rodata    align(4)                   :>.          /* constant datas in normal area */
                                                    
  .fixaddr   align(4)                   :>.          /* ghs internal (compiler) */
  .fixtype   align(4)                   :>.          /* ghs internal (compiler) */
  .secinfo   align(4)                   :>.          /* ghs internal (runtime library) */
  .syscall   align(4)                   :>.          /* ghs internal (linker) */
                                                    
  .romdata   ROM(.data)                 :>.          /* constant data to initialize variables (copied to RAM at startup)*/
  .romzdata  ROM(.zdata)                :>.          /* constant data to initialize variables in ZDA area (copied to RAM at startup)*/
  .romsdata  ROM(.sdata)                :>.          /* constant data to initialize variables in SDA area (copied to RAM at startup)*/
  .romtdata  ROM(.tdata)                :>.          /* constant data to initialize variables in TDA area (copied to RAM at startup)*/
  .romsldata ROM(.sldata)               :>.          /* constant data to initialize variables in LSDA area (copied to GRAM at startup)*/
                                                                                                   
  .text      align(4)                   :>.          /* program code area */

  // ----------------------------------------------------------------------
  // test ROM - should be placed at a specific ROM address
  // ----------------------------------------------------------------------
  .testconst0 (endaddr(iROM_BANK_A) - 648k - 32k) align(4) :
  .testconst  (endaddr(iROM_BANK_A) - 648k) align(4) :
}

// ----------------------------------------------------------------------
// extended user area
// ----------------------------------------------------------------------
///SECTIONS
//{
//  .bcode2    align(4)                   :>iROM_EXT   /* user defined segment for executable code located in user boot area */
//  .bconst2   align(4)                   :>.          /* user defined segment for constant data located in user boot area   */
//}

// ----------------------------------------------------------------------
// self RAM
// ----------------------------------------------------------------------
SECTIONS
{                                        
  .start.iRAM_Self                      :>iRAM_Self                                                    
  .end.iRAM_Self align(4)               :>iRAM_Self_end
}

// ----------------------------------------------------------------------
// RAM data of PE1 to PE2
// ----------------------------------------------------------------------
SECTIONS
{                                        
  .data_PE1                             :>iRAM_PE1

  .data_PE2                             :>iRAM_PE2

  .end.iRAM                             :>iRAM_end
}
                                                     
// ----------------------------------------------------------------------
// Cluster RAM
// ----------------------------------------------------------------------
SECTIONS
{
  .start.gRAM_BANK_A                    :>gRAM_BANK_A
                
  .data                                 :>.          /* initialized data */
  .bss       align(4)                   :>.          /* zero initialized data*/
  .sdabase   align(4)                   :>.          /* initialize globalpointer GP for SDA addressing */
  .sdata     align(4)                   :>.          /* initialized data in SDA area*/
  .sbss      align(4)                   :>.          /* zero initialized data in SDA area*/
  .zdata     align(4)                   :>.          /* initialized data in ZDA area*/
  .zbss      align(4)                   :>.          /* zero initialized data in ZDA area*/
  .tdata     align(4) MAX_SIZE(0x0100)  :>.          /* initialized and zero-initialized data in TDA area */
  .noclear   align(4) NOCLEAR           :>.          

  .sldata       align(4)                :>.          /* user defined segment for initialized data located in global RAM */
  .slbss        align(4)                :>.          /* user defined segment for uninitialized data located in global RAM */
  .bss.SHARED_DATA  align(4)            :>.          /* user defined segment for the same address data in each core */

  .end.gRAM_BANK_A                      :>gRAM_BANK_A_end

}

// ----------------------------------------------------------------------
// retention RAM
// ----------------------------------------------------------------------
SECTIONS
{                                        
  .start.rRAM                           :>rRAM
}

// ----------------------------------------------------------------------
// Data Flash
// ----------------------------------------------------------------------
SECTIONS
{                                        
  .start.DF_BANK_A                     :>DF_BANK_A
  .end.DF                              :>DF_end
}
