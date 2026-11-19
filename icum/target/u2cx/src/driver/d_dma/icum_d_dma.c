#include "icum_d_dma_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_d_wdt_pub.h"

#define DMA_WAIT_LIMIT (1000000UL)
#define DMA_STA_BUSY   (0x00000001UL)
#define DMA_STA_TE     (0x00000002UL)
#define DMA_STA_CAE    (0x00000008UL)
#define DMA_CLEAR_ALL  (0x0000300BUL)

static uint32_t s_dma_reserved;
static bool s_dma_faulted;

uint32_t D_DMA_Reserve(uint32_t channel_mask)
{
    uint32_t psw;
    uint32_t channel;
    uint32_t result = ER_OK;

    if (s_dma_faulted)
    {
        return D_DMA_STOP_ERROR;
    }
    if ((channel_mask == 0u) || ((channel_mask & ~0x0Fu) != 0u))
    {
        return ER_PARAM;
    }
    DISABLE_INTERRUPT_WITH_CHECK(psw);
    if ((s_dma_reserved & channel_mask) != 0u)
    {
        result = D_DMA_BUSY;
    }
    for (channel = 0u; channel < ICUM_DMAC_CH_NUM; channel++)
    {
        if (((channel_mask & (1u << channel)) != 0u) &&
            (((REG_ICUM_DMAC_CH[channel].ICUM_DMACHCR & 1u) != 0u) ||
             ((REG_ICUM_DMAC_CH[channel].ICUM_DMACHSTA & DMA_STA_BUSY) != 0u)))
        {
            result = D_DMA_BUSY;
        }
    }
    if (result == ER_OK)
    {
        s_dma_reserved |= channel_mask;
    }
    ENABLE_INTERRUPT_WITH_CHECK(psw);
    return result;
}

uint32_t D_DMA_Release(uint32_t channel_mask)
{
    uint32_t psw;
    uint32_t channel;
    uint32_t result = ER_OK;

    if ((channel_mask == 0u) || ((channel_mask & ~0x0Fu) != 0u))
    {
        return ER_PARAM;
    }
    DISABLE_INTERRUPT_WITH_CHECK(psw);
    for (channel = 0u; channel < ICUM_DMAC_CH_NUM; channel++)
    {
        if (((channel_mask & (1u << channel)) != 0u) &&
            (((REG_ICUM_DMAC_CH[channel].ICUM_DMACHCR & 1u) != 0u) ||
             ((REG_ICUM_DMAC_CH[channel].ICUM_DMACHSTA & DMA_STA_BUSY) != 0u)))
        {
            result = D_DMA_BUSY;
        }
    }
    if (result == ER_OK)
    {
        s_dma_reserved &= ~channel_mask;
    }
    ENABLE_INTERRUPT_WITH_CHECK(psw);
    return result;
}

bool D_DMA_IsFaulted(void)
{
    return s_dma_faulted;
}

uint32_t D_DMA_Stop(uint32_t channel_mask)
{
    uint32_t channel;
    uint32_t count;
    volatile REG_ICUM_DMAC_CH_t *p_channel;

    if ((channel_mask == 0u) || ((channel_mask & ~0x0Fu) != 0u))
    {
        return ER_PARAM;
    }
    for (channel = 0u; channel < ICUM_DMAC_CH_NUM; channel++)
    {
        if ((channel_mask & (1u << channel)) != 0u)
        {
            p_channel = &REG_ICUM_DMAC_CH[channel];
            p_channel->ICUM_DMACHSTP = 1u;
            p_channel->ICUM_DMACHFCR = 1u;
        }
    }
    /* Stop issuing requests and drain outstanding writes before releasing any buffer. */
    for (channel = 0u; channel < ICUM_DMAC_CH_NUM; channel++)
    {
        if ((channel_mask & (1u << channel)) == 0u)
        {
            continue;
        }
        p_channel = &REG_ICUM_DMAC_CH[channel];
        for (count = 0u; count < DMA_WAIT_LIMIT; count++)
        {
            if ((p_channel->ICUM_DMACHSTA & DMA_STA_BUSY) == 0u)
            {
                break;
            }
            if ((count & 0x3FFu) == 0u)
            {
                D_WDT_StartIfNeeded();
            }
        }
        if (count == DMA_WAIT_LIMIT)
        {
            s_dma_faulted = true;
            return D_DMA_STOP_ERROR;
        }
        p_channel->ICUM_DMACHFCR = DMA_CLEAR_ALL;
        p_channel->ICUM_DMACHSTP = 0u;
    }
    SYNC_MEMORY();
    return ER_OK;
}

static uint32_t dma_transaction_size(uint32_t address)
{
    /* Local RAM only accepts 32-bit DMA transactions (hardware manual, table 1.300). */
    return ((address >= 0xFEDE0000UL) && (address < 0xFEE00000UL)) ? 2u : 6u;
}

uint32_t D_DMA_Copy(uint8_t *p_dest, const uint8_t *p_src, uint32_t len)
{
    uint32_t channel;
    uint32_t count;
    uint32_t status;
    uint32_t result = D_DMA_TIMEOUT;
    volatile REG_ICUM_DMAC_CH_t *p_channel;

    if (s_dma_faulted)
    {
        return D_DMA_STOP_ERROR;
    }
    if ((p_dest == NULL) || (p_src == NULL) || (len == 0u) || ((len & 7u) != 0u) ||
        ((((uint32_t)p_dest | (uint32_t)p_src) & 7u) != 0u) ||
        ((uint32_t)p_dest > 0xFFFFFFFFUL - len) ||
        ((uint32_t)p_src > 0xFFFFFFFFUL - len))
    {
        return ER_PARAM;
    }
    for (channel = 0u; channel < ICUM_DMAC_CH_NUM; channel++)
    {
        if (D_DMA_Reserve(1u << channel) == ER_OK)
        {
            break;
        }
    }
    if (channel == ICUM_DMAC_CH_NUM)
    {
        return D_DMA_BUSY;
    }
    p_channel = &REG_ICUM_DMAC_CH[channel];
    REG_ICUM_DMAC.ICUM_DMAOR |= 1u;
    REG_ICUM_DMAC.channel_reset = 1u << channel;
    p_channel->ICUM_DMASAR = (uint32_t)p_src;
    p_channel->ICUM_DMADAR = (uint32_t)p_dest;
    p_channel->ICUM_DMATSR = len;
    p_channel->ICUM_DMATMR = 0x00000500UL |
        (dma_transaction_size((uint32_t)p_dest) << 4u) | dma_transaction_size((uint32_t)p_src);
    p_channel->ICUM_DMARS = 0x00010000UL;
    p_channel->ICUM_DMACHFCR = DMA_CLEAR_ALL;
    SYNC_MEMORY();
    p_channel->ICUM_DMACHCR = 1u;
    for (count = 0u; count < DMA_WAIT_LIMIT; count++)
    {
        status = p_channel->ICUM_DMACHSTA;
        if ((status & DMA_STA_CAE) != 0u)
        {
            result = D_DMA_ADDR_ERROR;
            break;
        }
        if (((status & (DMA_STA_TE | DMA_STA_BUSY)) == DMA_STA_TE) &&
            (p_channel->ICUM_DMATSR == 0u))
        {
            result = ER_OK;
            break;
        }
        if ((count & 0x3FFu) == 0u)
        {
            D_WDT_StartIfNeeded();
        }
    }
    if (D_DMA_Stop(1u << channel) != ER_OK)
    {
        return D_DMA_STOP_ERROR;
    }
    if (D_DMA_Release(1u << channel) != ER_OK)
    {
        s_dma_faulted = true;
        return D_DMA_STOP_ERROR;
    }
    return result;
}
