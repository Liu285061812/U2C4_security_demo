/******************************************************************************/
/* Component Name   icup_B.ld                                                 */
/******************************************************************************/
/* Product      : ICU-M Firmware                                              */
/******************************************************************************/
/*******************************************************************************
* Copyright(C) 2019-2021 Renesas Electronics Corporation.                       
*                                                                               
*          RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY                     
*                                                                               
* These instructions, statements, and programs are the confidential information 
* of Renesas Electronics Corporation. They must be used and modified solely for 
* the purpose for which it was furnished by Renesas Electronics Corporation.    
* All part of them must not be reproduced nor disclosed to others in any form,  
* without the prior written permission of Renesas Electronics Corporation.      
*******************************************************************************/
#include "icup_user.ld"

#if defined (ICUM_CRAM_GUARD_CLUSTER1)
#warning "Incorrect icum cram guard area setting"
#endif

MEMORY
{
  iROM_BANK_A       : ORIGIN = 0x00000000,  LENGTH = 0M         // Code Flash Bank A
  sROM_SIDE_A       : ORIGIN = .         ,  LENGTH = 2M         // Secure Code Flash Side A
  iROM_BANK_A_end   : ORIGIN = .         ,  LENGTH = 0          // End of Code Flash Bank A
  iROM_BANK_B       : ORIGIN = 0x00400000,  LENGTH = 0M         // Code Flash Bank B
  sROM_SIDE_B       : ORIGIN = .         ,  LENGTH = 2M         // Secure Code Flash Side B
  iROM_BANK_B_end   : ORIGIN = .         ,  LENGTH = 0          // End of Code Flash Bank B
  iROM_BANK_C       : ORIGIN = .         ,  LENGTH = 0          // Code Flash Bank C
  iROM_BANK_C_end   : ORIGIN = .         ,  LENGTH = 0          // End of Code Flash Bank C
  iROM_BANK_D       : ORIGIN = .         ,  LENGTH = 0          // Code Flash Bank D
  iROM_BANK_D_end   : ORIGIN = .         ,  LENGTH = 0          // End of Code Flash Bank D
  iROM_BANK_E       : ORIGIN = .         ,  LENGTH = 0          // Code Flash Bank E        ( Dummy )
  iROM_BANK_E_end   : ORIGIN = .         ,  LENGTH = 0          // End of Code Flash Bank E ( Dummy )
  iROM_BANK_F       : ORIGIN = .         ,  LENGTH = 0          // Code Flash Bank F        ( Dummy )
  iROM_BANK_F_end   : ORIGIN = .         ,  LENGTH = 0          // End of Code Flash Bank F ( Dummy )

  iROM_BANK_S0      : ORIGIN = 0x08000000,  LENGTH = 64k        // Extended user area0 (User boot area0)
  iROM_BANK_S0_end  : ORIGIN = .         ,  LENGTH = 0          // End of Extended user area0
  iROM_BANK_S1      : ORIGIN = 0x08400000,  LENGTH = 64k        // Extended user area1 (User boot area1)
  iROM_BANK_S1_end  : ORIGIN = .         ,  LENGTH = 0          // End of Extended user area

  iRAM_PE2          : ORIGIN = 0xFDA00000,  LENGTH = 128k       // PE2 RAM
  iRAM_PE1          : ORIGIN = 0xFDC00000,  LENGTH = 128k       // PE1 RAM
  iRAM_end          : ORIGIN = .         ,  LENGTH = 0          // End of internal RAM
  iRAM_Self         : ORIGIN = 0xFDE00000,  LENGTH = 128k       // Self RAM PE1 to PE2
  iRAM_Self_end     : ORIGIN = .         ,  LENGTH = 0          // End of Self RAM

  gRAM_BANK_A       : ORIGIN = 0xFE000000,  LENGTH = 448k       // Cluster RAM 0
  gRAM_BANK_A_end   : ORIGIN = .         ,  LENGTH = 0          // End of internal Cluster RAM 0

  Prohibit_A1       : ORIGIN = 0xFE070000,  LENGTH = 576k       // Access Prohibited Area 1
  Prohibit_A1_end   : ORIGIN = .         ,  LENGTH = 0          // End of Access Prohibited Area 1

  Prohibit_A2       : ORIGIN = 0xFE100000,  LENGTH = 3072k      // Access Prohibited Area 2 (0xFE100000 -- 0xFE3FFFFF)
  Prohibit_A2_end   : ORIGIN = .         ,  LENGTH = 0          // End of Access Prohibited Area 2

  gRAM_BANK_C       : ORIGIN = 0xFE400000,  LENGTH = 0          // Unavailable cluster RAM
  gRAM_BANK_C_end   : ORIGIN = .         ,  LENGTH = 0          // End of internal Cluster RAM 2

  Prohibit_A3       : ORIGIN = 0xFE400000,  LENGTH = 4M         // Access Prohibited Area 3
  Prohibit_A3_end   : ORIGIN = .         ,  LENGTH = 0          // End of Access Prohibited Area 3

  rRAM              : ORIGIN = 0x12000000,  LENGTH = 32k        // Retention RAM
  rRAM_end          : ORIGIN = .         ,  LENGTH = 0          // End of internal Cluster RAM 3

  Prohibit_A4       : ORIGIN = 0xFE820000,  LENGTH = 5888k      // Access Prohibited Area 4
  Prohibit_A4_end   : ORIGIN = .         ,  LENGTH = 0          // End of Access Prohibited Area 4

  sRAM_0            : ORIGIN = 0xFEDE0000,  LENGTH = 128k       // ICUM Local RAM
  sRAM_end          : ORIGIN = .         ,  LENGTH = 0          // End of internal ICUM Local RAM

  Prohibit_A5       : ORIGIN = 0xFEE00000,  LENGTH = 2M         // Access Prohibited Area 5 (0xFEE00000 -- 0xFEFFFFFF)
  Prohibit_A5_end   : ORIGIN = .         ,  LENGTH = 0          // End of Access Prohibited Area 5

  DF_BANK_A         : ORIGIN = 0xFF200000,  LENGTH = 128k       // Data Flash DATA AREA 0
  DF_BANK_A_end     : ORIGIN = .         ,  LENGTH = 0          // End of Data Flash DATA AREA 0
  DF_BANK_B         : ORIGIN = 0xFF220000,  LENGTH = 128k       // Data Flash DATA AREA 1
  DF_BANK_B_end     : ORIGIN = .         ,  LENGTH = 0          // End of Data Flash DATA AREA 1
  DF_BANK_ICUM      : ORIGIN = 0xFF280000,  LENGTH = 64k        // Data Flash DATA AREA 2
  DF_BANK_ICUM_end  : ORIGIN = .         ,  LENGTH = 0          // End of Data Flash DATA AREA 2

  DF_end            : ORIGIN = .         ,  LENGTH = 0          // End of internal Data FlashNGTH = 0x80

  Prohibit_A6       : ORIGIN = 0xFFFD4000,  LENGTH = 104k       // Access Prohibited Area 6 (0xFFFD4000 -- 0xFFFEDFFF)
  Prohibit_A6_end   : ORIGIN = .         ,  LENGTH = 0          // End of Access Prohibited Area 6

  Prohibit_A7       : ORIGIN = 0xFFFF0000,  LENGTH = 64k        // Access Prohibited Area 7 (0xFFFF0000 -- 0xFFFFFFFF)
  Prohibit_A7_end   : ORIGIN = .         ,  LENGTH = 0          // End of Access Prohibited Area 7

  Protect_A1        : ORIGIN = 0x00600000,  LENGTH = 0          // Memory Protect Area 1
  Protect_A1_end    : ORIGIN = 0xFEDE0000,  LENGTH = 0          // End of Memory Protect Area 1
  Protect_A2        : ORIGIN = 0xFEE00000,  LENGTH = 0          // Memory Protect Area 2 (0xFEE00000 -- 0xFFFFFFFF)
  Protect_A2_end    : ORIGIN = 0x00000000,  LENGTH = 0          // End of Memory Protect Area 2

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
  size_code_flash_block = 64k
  nb_block_firmware = 6
  nb_block_startup = 1
  size_ram_intvec = 512
  size_ram_config = 5k
  //size_ram_cism   = 2048
  size_ram_cism   = 0
}


// ----------------------------------------------------------------------
// ROM
// ----------------------------------------------------------------------
#define SIZE_CISM     (SIZEOF(.cism.rodata) + SIZEOF(.cism.text))
#define N             (size_code_flash_block - (SIZE_CISM % size_code_flash_block))
#define CISM_DUMMY    MIN(48, (SIZE_CISM * 48))
#define SIZE_STARTUP  (nb_block_startup * size_code_flash_block)
#define SIZE_PART     (nb_block_firmware * size_code_flash_block)
#define SIZE_FW       (SIZE_PART + SIZE_STARTUP)

// ----------------------------------------------------------------------
// iROM_BANK_A
// ----------------------------------------------------------------------
SECTIONS
{
    .start.iROM_BANK_A      NOCHECKSUM :> iROM_BANK_A
    .end.iROM_BANK_A        NOCHECKSUM :> iROM_BANK_A_end
}
// ----------------------------------------------------------------------
// iROM_BANK_B
// ----------------------------------------------------------------------
SECTIONS
{
    .start.iROM_BANK_B      NOCHECKSUM :> iROM_BANK_B
    .end.iROM_BANK_B        NOCHECKSUM :> iROM_BANK_B_end
}
// ----------------------------------------------------------------------
// iROM_BANK_C
// ----------------------------------------------------------------------
SECTIONS
{
    .start.iROM_BANK_C      NOCHECKSUM :> iROM_BANK_C
    .end.iROM_BANK_C        NOCHECKSUM :> iROM_BANK_C_end
}
// ----------------------------------------------------------------------
// iROM_BANK_D
// ----------------------------------------------------------------------
SECTIONS
{
    .start.iROM_BANK_D      NOCHECKSUM :> iROM_BANK_D
    .end.iROM_BANK_D        NOCHECKSUM :> iROM_BANK_D_end
}
// ----------------------------------------------------------------------
// iROM_BANK_E
// ----------------------------------------------------------------------
SECTIONS
{
    .start.iROM_BANK_E      NOCHECKSUM :> iROM_BANK_E
    .end.iROM_BANK_E        NOCHECKSUM :> iROM_BANK_E_end
}
// ----------------------------------------------------------------------
// iROM_BANK_F
// ----------------------------------------------------------------------
SECTIONS
{
    .start.iROM_BANK_F      NOCHECKSUM :> iROM_BANK_F
    .end.iROM_BANK_F        NOCHECKSUM :> iROM_BANK_F_end
}
// ----------------------------------------------------------------------
// sROM_SIDE_A
// ----------------------------------------------------------------------
SECTIONS
{
    .start.sROM_SIDE_A      NOCHECKSUM :> sROM_SIDE_A
    .start.firmware_a       ENDADDR(iROM_BANK_A_end) - (SIZE_FW - SIZE_STARTUP) NOCHECKSUM :
    .end.firmware_a         ADDR(.start.firmware_a) + SIZE_PART :
    .end.sROM_SIDE_A        NOCHECKSUM :> iROM_BANK_A_end
}
// ----------------------------------------------------------------------
// sROM_SIDE_B
// ----------------------------------------------------------------------
SECTIONS
{
    .start.sROM_SIDE_B      NOCHECKSUM :> sROM_SIDE_B
    .start.firmware_b       ENDADDR(iROM_BANK_B_end) - SIZE_PART NOCHECKSUM :
    .end.firmware_b         ADDR(.start.firmware_b) + SIZE_PART :
    .end.sROM_SIDE_B        NOCHECKSUM :> iROM_BANK_B_end
}

SECTIONS
{
  .start.cism ADDR(.start.ROM) - SIZE_CISM - N  :
  .cism.rodata  ALIGN(4)                    :>.
  .cism.text    ALIGN(4)                    :>.
  .cism.dummy   MIN_SIZE(CISM_DUMMY)        :>.
  .end.cism                                 :>.

  .start.ROM ENDADDR(sROM_SIDE_B) - SIZE_FW :

  .start.icum_startup_protect ABS ALIGN(512):>.
  .intvect      ALIGN(512)   NOCHECKSUM     :>.          /* fatal & user exceptions */
  .startup.text ALIGN(512)   NOCHECKSUM     :>.
  .icum_startup_d ROM(.icum_startup_ram)    :>.
  .icum_startup_const_d ROM(.icum_startup_const_ram) :>.
  .end.icum_startup_protect                 :>.

  .start.firmware ADDR(.start.firmware_b)   :
  .rom.intvect  ALIGN(512)     NOCHECKSUM   :>.          /* fatal & user exceptions */

  .ghs.sys   ALIGN(4)     NOCHECKSUM        :
  {
     *(.secinfo)
     *(.syscall)
     *(.fixaddr)
     *(.fixtype)
     *(.text)
  }

  .fixaddr   ALIGN(4)          NOCHECKSUM   :>.          /* ghs internal (compiler) */
  .fixtype   ALIGN(4)          NOCHECKSUM   :>.          /* ghs internal (compiler) */
  .secinfo   ALIGN(4)          NOCHECKSUM   :>.          /* ghs internal (runtime library) */
  .syscall   ALIGN(4)          NOCHECKSUM   :>.          /* ghs internal (linker) */
                                              
  .rozdata   ALIGN(4)          NOCHECKSUM   :>.          /* constant datas in ZDA area */
  .robase    ALIGN(4)          NOCHECKSUM   :>.          /* initialize textpointer TP for SDA addressing */
  .rosdata   ALIGN(4)          NOCHECKSUM   :>.          /* constant datas in SDA area */
  .rodata    ALIGN(4)          NOCHECKSUM   :>.          /* constant datas in normal area */
                                                        
  .romdata   ROM(.data)        NOCHECKSUM   :>.          /* constant data to initialize variables (copied to RAM at startup)*/
  .romBEFORE_SECRET_DATA_SECTION   ROM(.BEFORE_SECRET_DATA_SECTION)        NOCHECKSUM   :>.          /* constant data to initialize variables (copied to RAM at startup)*/
  .romSECRET_DATA_SECTION   ROM(.SECRET_DATA_SECTION)        NOCHECKSUM   :>.          /* constant data to initialize variables (copied to RAM at startup)*/
  .romAFTER_SECRET_DATA_SECTION   ROM(.AFTER_SECRET_DATA_SECTION)        NOCHECKSUM   :>.          /* constant data to initialize variables (copied to RAM at startup)*/
  .romzdata  ROM(.zdata)       NOCHECKSUM   :>.          /* constant data to initialize variables in ZDA area (copied to RAM at startup)*/
  .romsdata  ROM(.sdata)       NOCHECKSUM   :>.          /* constant data to initialize variables in SDA area (copied to RAM at startup)*/
  .romtdata  ROM(.tdata)       NOCHECKSUM   :>.          /* constant data to initialize variables in TDA area (copied to RAM at startup)*/
  .romsldata ROM(.sldata)      NOCHECKSUM   :>.          /* constant data to initialize variables in LSDA area (copied to GRAM at startup)*/
                                                        
  .start.icum_init    ALIGN(4)              :>.
  .icum_init.romdata ROM(.icum_init.data)   :>.
  .icum_init.rodata   ALIGN(4)              :
  .icum_init.text     ALIGN(4)              :
  .end.icum_init                            :>.
  
  .start.icum_driver                        :>.
  .icum_driver.romdata ROM(.icum_driver.data)             :>.
  .icum_driver.rodata                       :>.
  .icum_driver.text                         :>.
  .end.icum_driver                          :>.
                                            
  .start.icum_library                       :>.
  .icum_library.romdata ROM(.icum_library.data)             :>.
  .icum_library.rodata                      :>.
  .icum_library.text                         :>.
  .end.icum_library                         :>.

  .start.icum_middle                       :>.
  .icum_middle.romdata ROM(.icum_middle.data)             :>.
  .icum_middle.rodata                      :>.
  .icum_middle.text                        :>.
  .end.icum_middle                         :>.

  .start.icum_kernel                       :>.
  .icum_kernel.romdata ROM(.icum_kernel.data)             :>.
  .icum_kernel.rodata                      :>.
  .icum_kernel.text                        :>.
  .end.icum_kernel                         :>.

  .start.icum_service                      :>.
  .icum_service.romdata ROM(.icum_service.data)             :>.
  .icum_service.rodata                     :>.
  .icum_service.text                       :>.
  .end.icum_service                        :>.

  .start.icum_handler                      :>.
  .icum_handler.romdata ROM(.icum_handler.data)             :>.
  .icum_handler.rodata                     :>.
  .icum_handler.text                       :>.
  .end.icum_handler                        :>.

// for RFD28F code module
  .start.rfd28f_code                       :>.
  .R_RFD_CODE_COMMON              ALIGN(4) :>.
  .R_RFD_CODE_DF                  ALIGN(4) :>.
  .R_RFD_CODE_EXTRA               ALIGN(4) :>.
  .R_RFD_CODE_EX_PROT             ALIGN(4) :>.
  .R_RFD_RODATA_EXTRA             ALIGN(4) :>.
  .R_RFD_RODATA_VERSION_COMMON    ALIGN(4) :>.
  .R_RFD_RODATA_VERSION_DF        ALIGN(4) :>.
  .R_RFD_RODATA_VERSION_CF        ALIGN(4) :>.
  .end.rfd28f_code                         :>.

// for ram exection module
  .icum_init_d        ROM(.icum_init_ram)     :>.
  .icum_driver_d      ROM(.icum_driver_ram)   :>.
  .icum_library_d     ROM(.icum_library_ram)  :>.
  .icum_middle_d      ROM(.icum_middle_ram)   :>.
  .icum_kernel_d      ROM(.icum_kernel_ram)   :>.
  .icum_service_d     ROM(.icum_service_ram)  :>.
  .icum_handler_d     ROM(.icum_handler_ram)  :>.
  
// for constant
  .icum_init_const_d        ROM(.icum_init_const_ram)     :>.
  .icum_driver_const_d      ROM(.icum_driver_const_ram)   :>.
  .icum_library_const_d     ROM(.icum_library_const_ram)  :>.
  .icum_middle_const_d      ROM(.icum_middle_const_ram)   :>.
  .icum_kernel_const_d      ROM(.icum_kernel_const_ram)   :>.
  .icum_service_const_d     ROM(.icum_service_const_ram)  :>.
  .icum_handler_const_d     ROM(.icum_handler_const_ram)  :>.
  .firmware.dummy     MIN_SIZE(48)          :>.
  .end.firmware                             :>.

  .end.ROM                                  :>iROM_BANK_B_end
}


// ----------------------------------------------------------------------
// iROM_BANK_S0
// ----------------------------------------------------------------------
SECTIONS
{
    .start.iROM_BANK_S0      NOCHECKSUM :> iROM_BANK_S0
    .end.iROM_BANK_S0        NOCHECKSUM :> iROM_BANK_S0_end
}

// ----------------------------------------------------------------------
// iROM_BANK_S1
// ----------------------------------------------------------------------
SECTIONS
{
    .start.iROM_BANK_S1      NOCHECKSUM :> iROM_BANK_S1
    .end.iROM_BANK_S1        NOCHECKSUM :> iROM_BANK_S1_end
}

// ----------------------------------------------------------------------
// self RAM
// ----------------------------------------------------------------------
SECTIONS
{
  .start.iRAM_Self                          :> sRAM_0
  .start.icum_ram_protect    ABS ALIGN(512) :>.
  // reserved RAM for exception handlers
  .icum_ram_intvec  MAX_SIZE(size_ram_intvec)  ABS ALIGN(512) :>.

  .icum_startup_ram (ADDR(.icum_ram_intvec) + size_ram_intvec)  ALIGN(4) :>.
  .icum_init_ram              ALIGN(4)      :>.
  .icum_driver_ram            ALIGN(4)      :
  {
    *(.icum_driver_ram)
    *(.R_RFD_CODE_COMMON_RAM_NO_BGO)
    *(.R_RFD_CODE_CF)
    *(.R_RFD_CODE_CF_RAM_NO_BGO)
    *(.R_RFD_CODE_USEROWN_COMMON)
  }
  .icum_library_ram           ALIGN(4)      :>.
  .icum_middle_ram            ALIGN(4)      :>.
  .icum_kernel_ram            ALIGN(4)      :>.
  .icum_service_ram           ALIGN(4)      :>.
  .icum_handler_ram           ALIGN(4)      :>.
  .icum_startup_const_ram     ALIGN(4)      :>.
  .icum_init_const_ram        ALIGN(4)      :>.
  .icum_driver_const_ram      ALIGN(4)      :>.
  .icum_library_const_ram     ALIGN(4)      :>.
  .icum_middle_const_ram      ALIGN(4)      :>.
  .icum_kernel_const_ram      ALIGN(4)      :>.
  .icum_service_const_ram     ALIGN(4)      :>.
  .icum_handler_const_ram     ALIGN(4)      :>.
  .end.icum_ram_protect                     :>.
                                            
  .data      ALIGN(4) NOCHECKSUM            :>.          /* initialized data */
  .BEFORE_SECRET_DATA_SECTION      ALIGN(4) NOCHECKSUM            :>.          /* initialized data */
  .SECRET_DATA_SECTION      ALIGN(4) NOCHECKSUM            :>.          /* initialized data */
  .AFTER_SECRET_DATA_SECTION      ALIGN(4) NOCHECKSUM            :>.          /* initialized data */
  .bss       ALIGN(4)                       :>.          /* zero initialized data*/
                                            
  .sdabase   ALIGN(4)                       :>.          /* initialize globalpointer GP for SDA addressing */
  .sdata     ALIGN(4)                       :>.          /* initialized data in SDA area*/
  .sbss      ALIGN(4)                       :>.          /* zero initialized data in SDA area*/
  .zdata     ALIGN(4)                       :>.          /* initialized data in ZDA area*/
  .zbss      ALIGN(4)                       :>.          /* zero initialized data in ZDA area*/
  .tdata     ALIGN(4) MAX_SIZE(0x0100)      :>.          /* initialized and zero-initialized data in TDA area */
  .noclear   ALIGN(4) NOCLEAR               :>.          

  .icum_startup.data    ALIGN(4) NOCHECKSUM :>.
  .icum_init.data       ALIGN(4) NOCHECKSUM :>.
  .icum_driver.data     ALIGN(4) NOCHECKSUM :>.
  .icum_library.data    ALIGN(4) NOCHECKSUM :>.
  .icum_middle.data     ALIGN(4) NOCHECKSUM :>.
  .icum_kernel.data     ALIGN(4) NOCHECKSUM :>.
  .icum_service.data    ALIGN(4) NOCHECKSUM :>.
  .icum_handler.data    ALIGN(4) NOCHECKSUM :>.

  .icum_startup.bss     ALIGN(4)            :>.
  .icum_init.bss        ALIGN(4)            :>.
  .icum_driver.bss      ALIGN(4)            :>.
  .icum_library.bss     ALIGN(4)            :>.
  .icum_middle.bss      ALIGN(4)            :>.
  .icum_kernel.bss      ALIGN(4)            :>.
  .icum_service.bss     ALIGN(4)            :>.
  .icum_handler.bss     ALIGN(4)            :>.

  /* for RFD28F ram area */
  .start.rfd28f_ram                         :>.
  .R_RFD_BSS            ALIGN(4)            :>.
  .end.rfd28f_ram                           :>.

  /* config ram area */
  .start.icum_config_ram    ABS ALIGN(4)                                :>.
  .icum_config_ram          MAX_SIZE(size_ram_config)                   :>.
  .end.icum_config_ram      (ADDR(.icum_config_ram) + size_ram_config)  :>.

  // stack definition
  .stackbase            ALIGN(4)                    :>.
  .stack PAD(ADDR(.stackend) - ADDR(.stackbase))    :>.
  .stackend ADDR(.cism.bss)                         :>.

  // reserved RAM for expansion services
  // note! the .cism.data section is set to 0 to yield an error whenever an initialized variable is declared
  .cism.bss   MIN_SIZE(size_ram_cism) MAX_SIZE(size_ram_cism)   (ADDR(sRAM_end) - size_ram_cism)   ABS ALIGN(4)   :>.
  .cism.data  MAX_SIZE(0) :>.

  .end.iRAM_Self          :> sRAM_end
}

// ----------------------------------------------------------------------
// RAM data of PE1 to PE2
// ----------------------------------------------------------------------
SECTIONS
{
  .data_PE1                                 :> iRAM_PE1
                                            
  .data_PE2                                 :> iRAM_PE2
                                            
  .end.iRAM                                 :> iRAM_end
}
                                                     
// ----------------------------------------------------------------------
// global RAM
// ----------------------------------------------------------------------
SECTIONS
{
  .start.gRAM             NOCHECKSUM        :>gRAM_BANK_A
  .start.gRAM_BANK_A      NOCHECKSUM        :>.
#if defined(ICUM_CRAM_GUARD_CLUSTER0)
  .start.icum_cram_guard_bank_x             :>.
#endif
                                          
  .sldata       ALIGN(4)  NOCHECKSUM        :>.          /* user defined segment for initialized data located in global RAM */
  .slbss        ALIGN(4)  NOCHECKSUM        :>.          /* user defined segment for uninitialized data located in global RAM */

#if defined(ICUM_CRAM_GUARD_CLUSTER0)
  .start.icum_cram_guard  ALIGN(256)    icum_cram_guard_start_address                     :>.
  .icum_cram_guard        MIN_SIZE(icum_cram_guard_size - SIZEOF(.start.icum_cram_guard)) MAX_SIZE(icum_cram_guard_size - SIZEOF(.start.icum_cram_guard)) :>.
  .end.icum_cram_guard                                                                    :>.
#endif
                                            
  .end.gRAM_BANK_A                          :>gRAM_BANK_A_end

  .start.gRAM_BANK_C      NOCHECKSUM        :>gRAM_BANK_C
#if defined(ICUM_CRAM_GUARD_CLUSTER2)
  .start.icum_cram_guard_bank_x             :>.

  .start.icum_cram_guard  ALIGN(256)    icum_cram_guard_start_address                     :>.
  .icum_cram_guard        MIN_SIZE(icum_cram_guard_size - SIZEOF(.start.icum_cram_guard)) MAX_SIZE(icum_cram_guard_size - SIZEOF(.start.icum_cram_guard)) :>.
  .end.icum_cram_guard                                                                    :>.
#endif

  .end.gRAM_BANK_C                          :>gRAM_BANK_C_end

  .start.rRAM             NOCHECKSUM        :>rRAM

  .end.rRAM                                 :>rRAM_end
  .end.gRAM                                 :>.

}

// ----------------------------------------------------------------------
// Data Flash
// ----------------------------------------------------------------------
SECTIONS
{                                        
  .start.DF_BANK_A                          :>DF_BANK_A
  .end.DF_BANK_A                            :>DF_BANK_A_end
  .start.DF_BANK_B                          :>DF_BANK_B
  .end.DF_BANK_B                            :>DF_BANK_B_end  
  .start.DF_BANK_ICUM                       :>DF_BANK_ICUM
  .end.DF_BANK_ICUM                         :>DF_BANK_ICUM_end  
  .end.DF                                   :>DF_end
}

// ----------------------------------------------------------------------
// memory protect Area
// ----------------------------------------------------------------------
SECTIONS
{
  .start.Protect_A1                         :>Protect_A1
  .end.Protect_A1                           :>Protect_A1_end
  .start.Protect_A2                         :>Protect_A2
  .end.Protect_A2                           :>Protect_A2_end
}

// ----------------------------------------------------------------------
// Access Inhibited Area #1
// ----------------------------------------------------------------------
SECTIONS
{
  .start.Prohibit_A1                       :>Prohibit_A1
  .end.Prohibit_A1                         :>Prohibit_A1_end
}

// ----------------------------------------------------------------------
// Access Inhibited Area #2
// ----------------------------------------------------------------------
SECTIONS
{
  .start.Prohibit_A2                       :>Prohibit_A2
  .end.Prohibit_A2                         :>Prohibit_A2_end
}

// ----------------------------------------------------------------------
// Access Inhibited Area #3
// ----------------------------------------------------------------------
SECTIONS
{
  .start.Prohibit_A3                       :>Prohibit_A3
  .end.Prohibit_A3                         :>Prohibit_A3_end
}

// ----------------------------------------------------------------------
// Access Inhibited Area #4
// ----------------------------------------------------------------------
SECTIONS
{
  .start.Prohibit_A4                       :>Prohibit_A4
  .end.Prohibit_A4                         :>Prohibit_A4_end
}

// ----------------------------------------------------------------------
// Access Inhibited Area #5
// ----------------------------------------------------------------------
SECTIONS
{
  .start.Prohibit_A5                       :>Prohibit_A5
  .end.Prohibit_A5                         :>Prohibit_A5_end
}

// ----------------------------------------------------------------------
// Access Inhibited Area #6
// ----------------------------------------------------------------------
SECTIONS
{
  .start.Prohibit_A6                       :>Prohibit_A6
  .end.Prohibit_A6                         :>Prohibit_A6_end
}

// ----------------------------------------------------------------------
// Access Inhibited Area #7
// ----------------------------------------------------------------------
SECTIONS
{
  .start.Prohibit_A7                       :>Prohibit_A7
  .end.Prohibit_A7                         :>Prohibit_A7_end
}

