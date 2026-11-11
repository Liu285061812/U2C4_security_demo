/******************************************************************************/
/* Component Name   ICU-M Crypto AES Driver                                   */
/******************************************************************************/
/* Product      : ICU-M Low Level Driver                                      */
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

#if !defined(ICUM_D_AES_H)
#define ICUM_D_AES_H
#include ICUM_DEVICE_FILE

/**************************************************************************************/
/* Definition                                                                         */
/**************************************************************************************/
/* ICUM_AES Use Register */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
#define ICUM_AESCTRL        ICUMX_AESD0CTRL
#define ICUM_AESDCTL        ICUMX_AESD0DCTL
#define ICUM_AESCCTL0       ICUMX_AESD0CCTL0
#define ICUM_AESCCTL1       ICUMX_AESD0CCTL1
#define ICUM_AESSTR         ICUMX_AESD0STR
#define ICUM_AESSTC         ICUMX_AESD0STC
#define ICUM_AESDID0        ICUMX_AESD0DID0
#define ICUM_AESDID1        ICUMX_AESD0DID1
#define ICUM_AESDID2        ICUMX_AESD0DID2
#define ICUM_AESDID3        ICUMX_AESD0DID3
#define ICUM_AESDOD0        ICUMX_AESD0DOD0
#define ICUM_AESDOD1        ICUMX_AESD0DOD1
#define ICUM_AESDOD2        ICUMX_AESD0DOD2
#define ICUM_AESDOD3        ICUMX_AESD0DOD3
#define ICUM_AESODAT        ICUMX_AESD0ODAT
#define ICUM_AESKEY         ICUMX_AESD0KEY
#define ICUM_AESSTC         ICUMX_AESD0STC
#define ICUM_AESDTFSTT      ICUMX_AESD0DMASTT
#define ICUM_AESDTFSTP      ICUMX_AESD0DMASTP
#define ICUM_AESDTFTRB      ICUMX_AESD0DMATRB
#define ICUM_AESDMAMOD      ICUMX_AESD0DMAMOD
#define ICUM_AESDMACLKRATE  ICUMX_DMACA0CLKRATE
#define ICUM_AESDMADSTCNT   ICUMX_DMACA0CH5CNT
#define ICUM_AESDMASRCCNT   ICUMX_DMACA0CH6CNT
#define ICUM_AESDMADSTTRBLK ICUMX_DMACA0CH5TRBLK
#define ICUM_AESDMASRCTRBLK ICUMX_DMACA0CH6TRBLK
#define ICUM_AESDMADSTAD    ICUMX_DMACA0CH5DSTAD
#define ICUM_AESDMASRCAD    ICUMX_DMACA0CH6SRCAD
#define ICUM_AESDMAINTF     ICUMX_DMACA0INT1F1
#else
#define ICUM_AESCTRL   ICUM_AESE0CTRL
#define ICUM_AESDCTL   ICUM_AESE0DCTL
#define ICUM_AESCCTL0  ICUM_AESE0CCTL0
#define ICUM_AESCCTL1  ICUM_AESE0CCTL1
#define ICUM_AESSTR    ICUM_AESE0STR
#define ICUM_AESSTC    ICUM_AESE0STC
#define ICUM_AESDID0   ICUM_AESE0DID0
#define ICUM_AESDID1   ICUM_AESE0DID1
#define ICUM_AESDID2   ICUM_AESE0DID2
#define ICUM_AESDID3   ICUM_AESE0DID3
#define ICUM_AESDOD0   ICUM_AESE0DOD0
#define ICUM_AESDOD1   ICUM_AESE0DOD1
#define ICUM_AESDOD2   ICUM_AESE0DOD2
#define ICUM_AESDOD3   ICUM_AESE0DOD3
#define ICUM_AESODAT   ICUM_AESE0ODAT
#define ICUM_AESKEY    ICUM_AESE0KEY
#define ICUM_AESSTC    ICUM_AESE0STC
#define ICUM_AESDTFSTT ICUM_AESE0DTFSTT
#define ICUM_AESDTFSTP ICUM_AESE0DTFSTP
#define ICUM_AESDTFTRB ICUM_AESE0DTFTRB
#define ICUM_AESDTFIAD ICUM_AESE0DTFIAD
#define ICUM_AESDTFOAD ICUM_AESE0DTFOAD
#endif

/* ICUM_AESCTRL Register Contents   */
#define ODATSRC                 (0x30000000u)  /* Output data source selection bit                                  */
#define IDATDEST                (0x03000000u)  /* Input data destination selection bit                              */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
#define DISTDTF                 (0x00400000u)  /* DMA transfer destination choice bit                               */
#else
#define DISTDTF                 (0x00100000u)  /* DTF transfer destination choice bit                               */
#endif
#define CTX                     (0x00070000u)  /* Context select bit                                                */
#define BYTE_ORDER              (0x00000200u)  /* Byte order selection bit                                          */
#define DATREGSW                (0x00000100u)  /* Data register selection bit                                       */
#define RST2                    (0x00000004u)  /* Context#2 soft reset bit                                          */
#define RST1                    (0x00000002u)  /* Context#1 soft reset bit                                          */
#define RST0                    (0x00000001u)  /* Context#0 soft reset bit                                          */
/* ICUM_AESDCTL Register Contents   */
#define ATTR                    (0x000F0000u)  /* Input data attribute bit                                          */
#define LAST                    (0x00008000u)  /* Last block bit                                                    */
#define REM                     (0x00007F00u)  /* Number of valid bits in the last block                            */
#define IVSEL                   (0x00000004u)  /* IV_CTR_i selection bit                                            */
#define CONT                    (0x00000002u)  /* Operation continuation bit                                        */
#define NEW_KEY                 (0x00000001u)  /* Key update specification bit                                      */
/* ICUM_AESCCTL0 Register Contents  */
#define K1SEL                   (0x07000000u)  /* Select key data for context #1                                    */
#define ENCMODE1                (0x00100000u)  /* Specify the selection of encryption/decryption for context #1.    */
#define MOD1                    (0x000F0000u)  /* Specify the block cipher mode for context #1.                     */
#define K0SEL                   (0x00000700u)  /* Select key data for context #0                                    */
#define ENCMODE0                (0x00000010u)  /* Specify the selection of encryption/decryption for context #0.    */
#define MOD0                    (0x0000000Fu)  /* Specify the block cipher mode for context #0.                     */
/* ICUM_AESCCTL1 Register Contents  */
#define K2SEL                   (0x00000700u)  /* Select key data for context #2                                    */
#define ENCMODE2                (0x00000010u)  /* Specify the selection of encryption/decryption for context #2.    */
#define MOD2                    (0x0000000Fu)  /* Specify the block cipher mode for context #2.                     */
/* ICUM_AESSTR Register Contents    */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
#define DTF_SUSP                (0x00100000u)  /* DTF suspend bit                                                   */
#define DTF_RUN                 (0x00010000u)  /* DTF run bit                                                       */
#else
#define DTF_SUSP                (0x10000000u)  /* DTF suspend bit                                                   */
#define DTF_RUN                 (0x01000000u)  /* DTF run bit                                                       */
#endif
#define GHSTS                   (0x00000100u)  /* GHASH status bit                                                  */
#define STS                     (0x00000001u)  /* Status bit                                                        */
/* ICUM_AESE0KEY Register Contents  */
#define EX_KEY                  (0x00000100u)  /* Key extension bit                                                */
#define KEYSEL                  (0x00000003u)  /* Key length select bit                                            */
/* ICUM_AESDTFSTP Register Contents */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
#define DTF_STOP                (0x00000001u)  /* DTF stop bit                                                     */
#else
#define DTF_STOP                (0x00000001u)  /* DTF stop bit                                                     */
#endif
/* ICUM_AESSTC Register Contents    */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
#define DTF_SUSP_C               (0x00100000u)  /* DTF suspend clear bit                                            */
#else
#define DTF_SUSP_C               (0x10000000u)  /* DTF suspend clear bit                                            */
#endif

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
/* ICUM_AEDDMAMOD Register Contents       */
#define DMA_BO                   (0x10000000u)  /* Byte order switching bit for DMA transfer                        */
#define DMA_LASTDIS              (0x01000000u)  /* Specifies wh-ether the next DMA transfer is the last transfer     */
/* ICUM_AESDMACLK Register Contents       */
#define DMA_DATA                 (0x00000001u)  /* DMA Clock Rate Data                                              */
/* ICUM_AESDMAxxxCNT Register Contents    */
#define DMA_DME                  (0x00000001u)  /* Enable DMA Transfers                                             */
/* ICUM_AESDMAINTF Register Contents      */
#define DMA_DST_END              (0x00000100u)  /* DMA Transfers Completion                                         */
#define DMA_SRC_END              (0x00010000u)  /* DMA Transfers Completion                                         */
#endif

/* Function selection bit */
#define PRM_ODATSRC_BCO         (0x00000000u)  /* 00: Block Cipher Output register (BCO)                            */
#define PRM_ODATSRC_IV_CTR      (0x20000000u)  /* 10: Initialization vector/counter register                        */
                                               /*     (IV_CTR_i [i = 0 to 2])                                       */
#define PRM_ODATSRC_GHASH       (0x30000000u)  /* 11: GHASH register (GHASH_i [i = 0 to 2])                         */
#define PRM_IDATDEST_BCI        (0x00000000u)  /* 00: Block Cipher Input register (BCI)                             */
#define PRM_IDATDEST_KEY        (0x01000000u)  /* 01: KEY register (KEY_i [i = 0 to 2])                             */
#define PRM_IDATDEST_IV_CTR     (0x02000000u)  /* 10: Initialization vector/counter register                        */
                                               /*     (IV_CTR_i [i = 0 to 2])                                       */
#define PRM_IDATDEST_GHASH      (0x03000000u)  /* 11: GHASH register (GHASH_i [i = 0 to 2])                         */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
#define PRM_DISTDTF             (0x00400000u)  /* 1: Do not transfer the tag data to RAM                            */
#else
#define PRM_DISTDTF             (0x00100000u)  /* 1: Do not transfer the tag data to RAM                            */
#endif
#define PRM_CTX_0               (0x00000000u)  /* 000: Context#0                                                    */
#define PRM_CTX_1               (0x00010000u)  /* 001: Context#1                                                    */
#define PRM_CTX_2               (0x00020000u)  /* 010: Context#2                                                    */
#define PRM_BYTE_ORDER_APPLY    (0x00000000u)  /* 0: Apply byte order conversion                                    */
#define PRM_DATREGSW_DIDI_DODI  (0x00000100u)  /* 1: Use ICUM_AESDIDi/ICUM_AESDODi [i = 0 to 3]                     */
#define PRM_RST2_INIT           (0x00000004u)  /* 1: Initialize KEY_2, IV_CTR_2, GHASH_2 and SubKEY_2.              */
                                               /*    This bit is automatically cleared to 0 by hardware in 1 cycle. */
#define PRM_RST1_INIT           (0x00000002u)  /* 1: Initialize KEY_1, IV_CTR_1, GHASH_1 and SubKEY_1.              */
                                               /*    This bit is automatically cleared to 0 by hardware in 1 cycle. */
#define PRM_RST0_INIT           (0x00000001u)  /* 1: Initialize KEY_0, IV_CTR_0, GHASH_0 and SubKEY_0.              */
                                               /*    This bit is automatically cleared to 0 by hardware in 1 cycle. */
#define PRM_ATTR_NORMAL         (0x00000000u)  /* When the block cipher mode is other than GCM, XTS, CCM and CMAC,  */
                                               /* write 0.                                                          */
#define PRM_ATTR_GCM_GHASH      (0x00000000u)  /* Input data GHASH function                                         */
#define PRM_ATTR_GCM_GCTR       (0x00010000u)  /* Input data CTR function                                           */
#define PRM_ATTR_GCM_TAG        (0x00020000u)  /* Input data for tag generation                                     */
#define PRM_ATTR_XTS_TEXT       (0x00000000u)  /* Plaintext/ciphertext                                              */
#define PRM_ATTR_XTS_TWEAK      (0x00010000u)  /* tweak value                                                       */
#define PRM_ATTR_XTS_SHORT_LAST (0x00020000u)  /* Next to last block when the decryption operation is in progress   */
                                               /* and the length of the last block is 127 or shorter.               */
#define PRM_ATTR_CCM_B0         (0x00010000u)  /* Input data for parameter B0 used as a format function.            */
#define PRM_ATTR_CCM_DEC_CIPHER (0x00020000u)  /* Decryption: Ciphertext                                            */
#define PRM_ATTR_CCM_A          (0x00030000u)  /* Input data for associated data A used as a format function.       */
#define PRM_ATTR_CCM_PLAIN      (0x00040000u)  /* Plaintext                                                         */
#define PRM_ATTR_CCM_ENC_DUMMY  (0x00080000u)  /* Encryption: Input data for a dummy T XOR MSBTlen(S0)              */
#define PRM_ATTR_CCM_DEC_T_X_S0 (0x000D0000u)  /* Decryption: Input data for T XOR MSBTlen(S0)                      */
#define PRM_ATTR_CMAC_NO_ZERO   (0x00000000u)  /* Valid bit number of input data is not 0.                          */
#define PRM_ATTR_CMAC_ZERO      (0x00010000u)  /* Valid bit number of input data is 0.                              */
#define PRM_NO_LAST_BLOCK       (0x00000000u)  /* 0: not Last block                                                 */
#define PRM_LAST_BLOCK          (0x00008000u)  /* 1: Last block                                                     */
#define PRM_REM_128BIT          (0x00000000u)  /* 0000000: 128-bit                                                  */
#define PRM_IVSEL_NO_USE        (0x00000000u)  /* 0: Do not use IV_CTR_i value                                      */
#define PRM_IVSEL_USE           (0x00000004u)  /* 1: Use IV_CTR_i value                                             */
#define PRM_CONT_NEW_CONTINUE   (0x00000000u)  /* 0: The next operation is a new or continuous operation.           */
#define PRM_CONT_RESUME         (0x00000002u)  /* 1: The next operation is the resumption of a suspended operation. */
#define PRM_NEW_KEY_NO_USE      (0x00000000u)  /* 0: Use the key data in the AES core that is used in the preceding */
                                               /*    operation.                                                     */
#define PRM_NEW_KEY_USE         (0x00000001u)  /* 1: Update the key data in the AES core with the key data output   */
                                               /*    from the KEY block.                                            */
#define PRM_K1SEL_PLAIN         (0x00000000u)  /* 000: ICUM_AESIDAT/ICUM_AESDIDi [i = 0 to 3]                       */
#define PRM_ENCMODE1_ENC        (0x00000000u)  /* 0: Encryption                                                     */
#define PRM_MOD1_CMAC           (0x00080000u)  /* Specify the block cipher mode CMAC for context #1.                */
#define PRM_K0SEL_PLAIN         (0x00000000u)  /* 000: ICUM_AESIDAT/ICUM_AESDIDi [i = 0 to 3]                       */
#define PRM_ENCMODE0_ENC        (0x00000000u)  /* 0: Encryption                                                     */
#define PRM_ENCMODE0_DEC        (0x00000010u)  /* 1: Decryption                                                     */
#define PRM_MOD0_CMAC           (0x00000008u)  /* Specify the block cipher mode CMAC for context #0.                */
#define PRM_MOD0_MP             (0x00000009u)  /* Specify the block cipher mode MP for context #0.                  */
#define PRM_K2SEL_PLAIN         (0x00000000u)  /* 000: ICUM_AESIDAT/ICUM_AESDIDi [i = 0 to 3]                       */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
#define PRM_DTF_SUSP            (0x00100000u)  /* 1: DMA transfer is suspend state                                  */
#define PRM_DTF_RUN             (0x00010000u)  /* 1: DMA transfer is in transfer state suspend state                */
#else
#define PRM_DTF_SUSP            (0x10000000u)  /* 1: DTF transfer is suspend state                                  */
#define PRM_DTF_RUN             (0x01000000u)  /* 1: DTF transfer is in transfer state suspend state                */
#endif
#define PRM_GHSTS_VALID         (0x00000100u)  /* 1: Hash value on GHASH_CTR register is valid.                     */
#define PRM_STS_IDLE            (0x00000000u)  /* 0: ICUM_AES is in idle state.                                     */
#define PRM_EX_KEY_SET_127_0    (0x00000000u)  /* 0: Set key bit [127:0]                                            */
#define PRM_EX_KEY_SET_256_128  (0x00000100u)  /* 1: Set key bit [191:128] or [256:128]                             */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
#define PRM_DTF_STOP            (0x00000001u)  /* 1: DMA transfer in transfer state is suspended                    */
#define PRM_DTF_SUSP_RESUME     (0x00100000u)  /* 1: DMA transfer in suspension state is resumed                    */
#else
#define PRM_DTF_STOP            (0x00000001u)  /* 1: DTF transfer in transfer state is suspended                    */
#define PRM_DTF_SUSP_RESUME     (0x10000000u)  /* 1: DTF transfer in suspension state is resumed                    */
#endif
#define KEY_LENGTH              (0x00000003u)  /* Key length reset                                                  */
#define PRM_KEY_LENGTH_128      (0x00000000u)  /* 00: key_length 128bit                                             */
#define PRM_KEY_LENGTH_192      (0x00000001u)  /* 01: key_length 192bit                                             */
#define PRM_KEY_LENGTH_256      (0x00000002u)  /* 10: key_length 256bit                                             */

#define PRM_MOD2_CCM            (0x00000007u)  /* 0111: mode CCM                                                    */

#define PRM_ENCMODE2_ENC        (0x00000000u)  /* 0: Encryption                                                     */
#define PRM_ENCMODE2_DEC        (0x00000010u)  /* 1: Decryption                                                     */

#define PRM_MOD0_ECB            (0x00000000u)  /* Specify the block cipher mode ECB for context #0.                  */
#define PRM_MOD0_CBC            (0x00000001u)  /* Specify the block cipher mode CBC for context #0.                  */
#define PRM_MOD0_CTR            (0x00000002u)  /* Specify the block cipher mode CTR for context #0.                  */
#define PRM_MOD0_CBF_128        (0x00000003u)  /* Specify the block cipher mode CFB_128 for context #0.              */
#define PRM_MOD0_OFB            (0x00000004u)  /* Specify the block cipher mode OFB for context #0.                  */
#define PRM_MOD0_GCM            (0x00000005u)  /* Specify the block cipher mode PRM_MOD0_GCM for context #0.         */
#define PRM_MOD0_XTS            (0x00000006u)  /* Specify the block cipher mode XTS for context #0.                  */

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
#define RSTDTF                  (0x00000010u)  /* DMA RSTDMA soft reset bit                                          */
#define RSTDTF_RESET            (0x00000010u)  /* DMA RSTDMA reset bit                                               */
#else
#define RSTDTF                  (0x00000020u)  /* DTF RSTDTF soft reset bit                                          */
#define RSTDTF_RESET            (0x00000020u)  /* DTF RSTDTF reset bit                                               */
#endif
#define DTF_TRBLK_RESET         (0x0FFFFFFFu)  /* SET : cipher block count                                           */
#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
#define DMASRCAD_MASK           (0xFFFFFF00u)  /* DMA Destination Address Mask                                       */
#define DMADSTAD_MASK           (0xFFFFFF00u)  /* DMA Destination Address Mask                                       */
#define DTFIAD_RESET            (0xFFFFFF00u)  /* DTFIAD reset bit  SET : source address                             */
#define DTFOAD_RESET            (0xFFFFFF00u)  /* DTFOAD reset bit  SET : destination address                        */
#define PRM_DTF_START           (0x00000001u)  /* 1: Start DTF transfer                                              */
#else
#define DTFIAD_RESET            (0xFFFFFFFFu)  /* DTFIAD reset bit  SET : source address                             */
#define DTFOAD_RESET            (0xFFFFFFFFu)  /* DTFOAD reset bit  SET : destination address                        */
#define PRM_DTF_START           (0x00010000u)  /* 1: Start DTF transfer                                              */
#endif
#define PRM_ODATSRC_BCO_DTF     (0x10000000u)  /* 01: Block Cipher Output register by DTF transfer (BCO_DTF)         */

#define PRM_REM_ZERO            (0x00000000u)  /* XTS REM 0                                                          */

#define RST3                    (0x00000008u)  /* Context#3 soft reset bit                                           */
#define PRM_RST3_INIT           (0x00000008u)  /* 1: Initialize PRNG_KEY and IV_PRNG.                                */

/* Bit shift of setting value */
#define SHIFT_REM               (8u)           /* Shift Number of valid bits in the last block                      */
#define SHIFT_K0SEL             (8u)           /* Shift key data for context #0                                     */
#define SHIFT_K2SEL             (8u)           /* Shift key data for context #2                                     */
#define SHIFT_ENCMODE0          (4u)           /* Shift the selection of encryption/decryption for context #0.      */
#define SHIFT_ENCMODE2          (4u)           /* Shift the selection of encryption/decryption for context #2.      */

/* timeout of crypto */
/* A value that realizes a value (actual measurement value) that can secure 10 times of 300ns (worst time). */
#define TIMEOUT_INIT            ((uint32_t)16u)
#define TIMEOUT_ELAPSED         ((uint32_t)0u)

/* Address of data */
#define ADDR_0TH                ((uint32_t)0u)
#define ADDR_1ST                ((uint32_t)1u)
#define ADDR_2ND                ((uint32_t)2u)
#define ADDR_3RD                ((uint32_t)3u)
#define ADDR_4TH                ((uint32_t)4u)
#define ADDR_5TH                ((uint32_t)5u)
#define ADDR_6TH                ((uint32_t)6u)
#define ADDR_7TH                ((uint32_t)7u)

/* used IV */
#define NO_USE_IV               ((uint32_t)0u)
#define USE_IV                  ((uint32_t)1u)

/* block type */
#define NUM_FIRST_BLOCK         ((uint32_t)1u)
#define NUM_PRECEDING_BLOCK     ((uint32_t)2u)
#define NUM_LAST_BLOCK          ((uint32_t)1u)
#define NUM_XTS_SPECIAL_BLOCK   (NUM_FIRST_BLOCK + NUM_PRECEDING_BLOCK)
#define NUM_GCM_SPECIAL_BLOCK   (NUM_FIRST_BLOCK + NUM_LAST_BLOCK)
#define NUM_CMAC_SPECIAL_BLOCK  (NUM_FIRST_BLOCK + NUM_LAST_BLOCK)

/* bit size of the total input block */
#define BIT_SIZE_128_OR_MORE    ((uint32_t)0u)
#define BIT_SIZE_127_OR_LESS    ((uint32_t)1u)

/* last block */
#define BLOCK_INFO_INPUT_UNIT   ((uint32_t)32u)
#define BLOCK_INFO_ACCESS_UNIT  ((uint32_t)8u)

/* Check calculation completion */
#define END_CALC                ((uint32_t)0u)

/* all 0bit */
#define ALL_ZERO                ((uint32_t)0x00000000u)

/* Alignment check */
#define ALIGN_OK                ((uint32_t)0x00000000u)

/* Bit shift */
#define SHIFT_AES_FIRST_CIPHER  ((uint32_t)8u)
#define SHIFT_AES_DIRECTION     ((uint32_t)9u)

/* use context#2 */
#define NO_USE_CTX2             ((uint32_t)0u)
#define USE_CTX2                ((uint32_t)1u)

/* B0 format*/
#define B0_RESERVED             ((uint32_t)0x00000000u)
#define B0_ALEN                 ((uint32_t)0xF8FF0700u)
#define B0_NLEN                 ((uint32_t)0x00007000u)
#define B0_TLEN                 ((uint32_t)0x00000700u)
#define B0_PLEN0                ((uint32_t)0x07000000u)
#define B0_PLEN1                ((uint32_t)0xFFFFFFFFu)
#define B0_PLEN2                ((uint32_t)0xF8FFFFFFu)

/* AES control status */
#define D_AES_STS_NONE          (0x00000000u)
#define D_AES_STS_FIRST         ((uint32_t)D_AES_FIRST_CIPHER)
#define D_AES_STS_EXECUTE       (0x00000002u)
#define D_AES_STS_SUSPEND       (0x00000004u)

/* Bit shift */
#define SHIFT_AES_FIRST_CIPHER  ((uint32_t)8u)
#define SHIFT_AES_DIRECTION     ((uint32_t)9u)

/* XTS - last block in bits */
#define D_AES_XTS_LAST_BLOCK_IN_BIT_MIN ((uint32_t)1u)
#define D_AES_XTS_LAST_BLOCK_IN_BIT_MAX ((uint32_t)128u)

/* GCM - IV size in bits */
#define D_AES_GCM_IV_SIZE_IN_BITS_MIN ((uint32_t)1u)
#define D_AES_GCM_IV_SIZE_IN_BITS_MAX ((uint32_t)1024u)

/* GCM - authentication tag size in bits */
#define D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_32  ((uint32_t)32u)
#define D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_64  ((uint32_t)64u)
#define D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_96  ((uint32_t)96u)
#define D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_104 ((uint32_t)104u)
#define D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_112 ((uint32_t)112u)
#define D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_120 ((uint32_t)120u)
#define D_AES_GCM_AUTH_TAG_SIZE_IN_BITS_128 ((uint32_t)128u)

/* CCM - IV size in bits */
#define D_AES_CCM_IV_SIZE_IN_BITS_56  ((uint32_t)56u)
#define D_AES_CCM_IV_SIZE_IN_BITS_64  ((uint32_t)64u)
#define D_AES_CCM_IV_SIZE_IN_BITS_72  ((uint32_t)72u)
#define D_AES_CCM_IV_SIZE_IN_BITS_80  ((uint32_t)80u)
#define D_AES_CCM_IV_SIZE_IN_BITS_88  ((uint32_t)88u)
#define D_AES_CCM_IV_SIZE_IN_BITS_96  ((uint32_t)96u)
#define D_AES_CCM_IV_SIZE_IN_BITS_104 ((uint32_t)104u)

/* CCM - authentication data size in bits */
#define D_AES_CCM_AUTH_DATA_SIZE_IN_BITS_MAX  ((uint32_t)0x0007f7f8u)

/* CCM - authentication tag size in bits */
#define D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_32  ((uint32_t)32u)
#define D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_48  ((uint32_t)48u)
#define D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_64  ((uint32_t)64u)
#define D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_80  ((uint32_t)80u)
#define D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_96  ((uint32_t)96u)
#define D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_112 ((uint32_t)112u)
#define D_AES_CCM_AUTH_TAG_SIZE_IN_BITS_128 ((uint32_t)128u)

#define D_AES_BYTE_SIZE_IN_BITS    (8u)       /* 1 byte is 8 bits   */

/* AES DTF operation */
#define D_AES_DTF_XTS_TW               (0u)
#define D_AES_DTF_XTS_ECDC1            (1u)
#define D_AES_DTF_XTS_ECDC2            (2u)
#define D_AES_DTF_XTS_ECDC3            (3u)
#define D_AES_DTF_XTS_DEC              (4u)
#define D_AES_DTF_GCM_IV               (5u)
#define D_AES_DTF_GCM_AAD              (6u)
#define D_AES_DTF_GCM_PC               (7u)
#define D_AES_DTF_CCM_ENC1             (8u)
#define D_AES_DTF_CCM_ENC2             (9u)
#define D_AES_DTF_CCM_DEC1             (10u)
#define D_AES_DTF_CCM_DEC2             (11u)
#define D_AES_DTF_CCM_DEC3             (12u)
#define D_AES_DTF_CCM_C                (13u)

/* Flag ON/OFF */
#define FLG_OFF (0U)
#define FLG_ON  (1U)


#define TAG_LENGTH_32           (32u)
#define TAG_LENGTH_48           (48u)
#define TAG_LENGTH_64           (64u)
#define TAG_LENGTH_80           (80u)
#define TAG_LENGTH_96           (96u)
#define TAG_LENGTH_112          (112u)
#define TAG_LENGTH_128          (128u)

#define IV_LENGTH_56            (56u)
#define IV_LENGTH_64            (64u)
#define IV_LENGTH_72            (72u)
#define IV_LENGTH_80            (80u)
#define IV_LENGTH_88            (88u)
#define IV_LENGTH_96            (96u)
#define IV_LENGTH_104           (104u)


#define TLEN_32                 (0x00000000u)
#define TLEN_48                 (0x00010000u)
#define TLEN_64                 (0x00020000u)
#define TLEN_80                 (0x00030000u)
#define TLEN_96                 (0x00040000u)
#define TLEN_112                (0x00050000u)
#define TLEN_128                (0x00060000u)

#define NLEN_56                 (0x00000000u)
#define NLEN_64                 (0x00100000u)
#define NLEN_72                 (0x00200000u)
#define NLEN_80                 (0x00300000u)
#define NLEN_88                 (0x00400000u)
#define NLEN_96                 (0x00500000u)
#define NLEN_104                (0x00600000u)

#define COUNTER_MAX             ((uint32_t)0xFFFFFFFFu)  /* unsigned int max number */

#define D_AES_ZERO_U8           ((uint8_t)0)
#define D_AES_ZERO_U16          ((uint16_t)0U)
#define D_AES_ZERO_U32          ((uint32_t)0U)
#define D_AES_SET1_U32          ((uint32_t)1U)
#define D_AES_SET96             (96U)
#define D_AES_SET127            (127U)
#define D_AES_SET128            (128U)

#define D_AES_SET_KEYSEL_0      (0U)
#define D_AES_SET_KEYSEL_1      (1U)
#define D_AES_SET_KEYSEL_2      (2U)

#define D_AES_IV_MAKE_SET_1     (0x01000000U)
#define D_AES_IV_MAKE_SET_2     (0x02000000U)
#define D_AES_IV_SIZE_CHECK_32  (32U)
#define D_AES_SET_ZERO_16       (0x00U)
#define D_AES_SET_07_16         (0x07U)
#define D_AES_SET_FF_16         (0xFFU)
#define D_AES_SET_VALID_BIT     (0xFF00U)

#define D_AES_SET_4BYTE         (4U)
#define D_AES_SET_8BYTE         (8U)
#define D_AES_SET_12BYTE        (12U)
#define D_AES_SET_16BYTE        (16U)

#define D_AES_BLOCK_SIZE_1      (1U)
#define D_AES_BLOCK_SIZE_2      (2U)
#define D_AES_BLOCK_SIZE_3      (3U)
#define D_AES_BLOCK_SIZE_4      (4U)
#define D_AES_SET_LOOP_4        (4U)

#define D_AES_SET_4             (4U)
#define D_AES_SET_8             (8U)
#define D_AES_SET_7             (7U)
#define D_AES_SET_16            (16U)
#define D_AES_SET_31            (31U)
#define D_AES_SET_32            (32U)

#define D_AES_BIT_CONVERSION    (8U)
#define D_AES_BYTE_CONVERSION   (16U)

#define D_AES_CHK_OK            (1u)
#define D_AES_CHK_NG            (0u)
#define D_AES_MAX_ADDRESS       (0xFFFFFFFFU)

/*  key length id use management table (AES) */
#define D_AES_KEYLEN_ID_USE_MNG (16U)
/*  key length id unused (AES) */
#define D_AES_KEYLEN_ID_FREE    (0U)
/*  key length id Re-roll from 1 */
#define D_AES_KEYLEN_ID_RETURN1 (1U)
/*  key length id MAX */
#define D_AES_KEYLEN_MAX_ID     (0xFFFFFFFFU)

/* number of key */
#define D_AES_KEY_NUM           (uint32_t)(0x00000004u)

/* wait until the specified register value */
#define D_AES_WAIT_REG_VALUE(value, reg_item, msk_bit) \
    do\
    {\
    } while ((value) == ((reg_item) & (msk_bit)))\

/********************************************************************************
 Link to external constants
********************************************************************************/
extern uint8_t  __ghsbegin_start_iRAM_Self[];
extern uint8_t  __ghsbegin_end_iRAM_Self[];

#define D_AES_ADDR_LOWER_ICUM_LOCAL_RAM    ((uint32_t)__ghsbegin_start_iRAM_Self)
#define D_AES_ADDR_UPPER_ICUM_LOCAL_RAM    (((uint32_t)__ghsbegin_end_iRAM_Self) - 1u)

/**************************************************************************************/
/* Structure Definition                                                               */
/**************************************************************************************/
/* AES context param */
typedef struct
{
    uint32_t               key_length_id;
    uint16_t               key_length;
    uint8_t                cipher_mode;
    uint8_t                direction;
    uint32_t               aes_key[D_AES_KEY_SIZE_COMMON];
    uint32_t               aes_iv[AES_BLOCK_SIZE];
} aes_context_t;

/* GCM context param */
typedef struct
{
    uint32_t               key_length_id;
    uint16_t               key_length;
    uint8_t                direction;
    uint32_t               aes_key[D_AES_KEY_SIZE_COMMON];
    uint32_t               ghash[AES_BLOCK_SIZE];
    uint32_t               ctr_j[AES_BLOCK_SIZE];
} gcm_context_t;

/* CCM context param */
typedef struct
{
    uint32_t               key_length_id;
    uint16_t               key_length;
    uint8_t                direction;
} ccm_context_t;

/* Aes driver control structure */
typedef struct {
    uint32_t status;          /* AES driver status */
    uint32_t reg_ctrl;        /* AES control register */
    uint32_t reg_dctl;        /* AES data control register */
} aes_drv_ctrl_t;

/* AES DTF parameters */
typedef struct
{
    uint32_t               number;
    uint32_t*              p_aes_src;
    uint32_t*              p_aes_dest;
    uint8_t                async;
}aes_transfer_dtf_t;

/* AES DTF INFO parameters */
typedef struct
{
    uint8_t                cipher_mode;
    uint16_t               direction;
    uint32_t               number;
    uint32_t               *p_aes_src;
    uint32_t               *p_aes_dest;
    uint8_t                async;
    uint8_t                subroutine;
    uint16_t               last_block_in_bits;
    uint32_t               iv_size_in_bits;
    uint32_t*              p_auth_data;
    uint32_t               auth_data_size_in_bits;
    uint32_t               data_size_in_bit;
    uint32_t               auth_tag_size_in_bit;
    uint32_t*              p_auth_tag;
    uint8_t                dtf_transfer_status;
    uint16_t               key_length;
    uint32_t*              p_aes_iv;
    uint32_t*              p_auth_yr;
}aes_dtf_info_t;

/* Key restriction status param */
typedef struct
{
    uint16_t               counter;
    uint16_t               key_length;
    uint32_t               key_length_id[D_AES_KEYLEN_ID_USE_MNG];
} aes_key_restriction_t;


/**************************************************************************************/
/* Global Variables                                                                   */
/**************************************************************************************/
extern uint32_t g_ctx2_state;
extern uint32_t g_aes_dtf_flag;
extern uint32_t g_aes_atonce_id_use_mng;
extern init_callback_t *g_p_aes_callbackFunc;

/**************************************************************************************/
/* Prototypes                                                                         */
/**************************************************************************************/
uint32_t d_AES_ExecutionAtOnceDtf(aes_all_at_once_t* p_param);
uint32_t d_AES_ExecutionAtOnceXtsDtf(aes_all_at_once_xts_t* p_param);
uint32_t d_AES_ExecutionAtOnceGcmDtf(aes_all_at_once_gcm_t* p_param);
uint32_t d_AES_ExecutionAtOnceCcmDtf(aes_all_at_once_ccm_t* p_param);
uint32_t d_AES_FreeKeyRestriction(uint16_t key_length, uint32_t key_length_id);
uint32_t d_AES_LastBlock(uint32_t* p_in_data, uint32_t* p_aes_src, uint16_t last_block_size);
uint32_t d_AES_AddCounter(uint32_t *p_counter, uint32_t add_counter);
uint32_t d_AES_Cipher(const uint32_t* p_aes_src, uint32_t* p_aes_dest);
uint32_t d_AES_CipherDummyRead(const uint32_t* p_aes_src);
uint32_t d_AES_CipherNoRead(const uint32_t* p_aes_src);
uint32_t d_AES_GenerateB0(uint8_t *p_B0, uint32_t iv_size_in_bits,
                          uint32_t auth_tag_size_in_bits,
                          uint32_t data_size_in_bits,
                          uint32_t auth_data_size_in_bits);


#endif /* ICUM_D_AES_H */
