#ifndef ICUM_D_DMA_PUB_H
#define ICUM_D_DMA_PUB_H

#include "icum_common_pub.h"

#define ICUM_DMAC_CH_NUM (4U)
#define D_DMA_BUSY       (0x00000F01UL)
#define D_DMA_ADDR_ERROR (0x00000F02UL)
#define D_DMA_TIMEOUT    (0x00000F03UL)
#define D_DMA_STOP_ERROR (0x00000F04UL)

typedef struct
{
    uint8_t reserved0[16];
    uint32_t ICUM_DMAESTA;
    uint8_t reserved1[12];
    uint32_t ICUM_DMAISTA;
    uint8_t reserved2[28];
    uint32_t channel_priority;
    uint8_t reserved3[28];
    uint16_t ICUM_DMAOR;
    uint8_t reserved4[30];
    uint32_t channel_reset;
    uint8_t reserved5[104];
    uint32_t version;
} REG_ICUM_DMAC_t;

typedef struct
{
    uint32_t ICUM_DMASAR;
    uint32_t ICUM_DMADAR;
    uint32_t ICUM_DMATSR;
    uint32_t ICUM_DMATSRB;
    uint32_t ICUM_DMATMR;
    uint16_t ICUM_DMACHCR;
    uint16_t ICUM_DMACHSTP;
    uint32_t ICUM_DMACHSTA;
    uint32_t ICUM_DMACHFCR;
    uint8_t reserved0[32];
    uint32_t ICUM_DMARS;
    uint8_t reserved1[4];
    uint32_t ICUM_DMABUFCR;
    uint8_t reserved2[52];
} REG_ICUM_DMAC_CH_t;

#if defined(TARGET_DEVICE_S4X) || defined(TARGET_DEVICE_V4H)
#define REG_ICUM_DMAC_BASE (0xFF600000U)
#else
#define REG_ICUM_DMAC_BASE (0xFF1FA000U)
#endif
#define REG_ICUM_DMAC_CH_BASE (REG_ICUM_DMAC_BASE + 0x2000U)
#define REG_ICUM_DMAC (*(volatile REG_ICUM_DMAC_t *)REG_ICUM_DMAC_BASE)
#define REG_ICUM_DMAC_CH (*(volatile REG_ICUM_DMAC_CH_t (*)[ICUM_DMAC_CH_NUM])REG_ICUM_DMAC_CH_BASE)

uint32_t D_DMA_Reserve(uint32_t channel_mask);
uint32_t D_DMA_Release(uint32_t channel_mask);
uint32_t D_DMA_Stop(uint32_t channel_mask);
uint32_t D_DMA_Copy(uint8_t *p_dest, const uint8_t *p_src, uint32_t len);
bool D_DMA_IsFaulted(void);

#endif
