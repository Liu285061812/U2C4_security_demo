#include "renesas_types.h"
#include MM_FLASH_PUBLIC_H
#include "icum_mm_flash.h"

#define RAM_FLS_CAPACITY (FLS_NB_BLOCK * FLS_ERASE_BLOCK_SIZE)
#define RAM_FLS_INVALID_OFFSET (0xFFFFFFFFuL)

typedef struct
{
    uint32_t offset;
    uint16_t size;
    uint8_t ds_type;
    uint8_t slot_num;
    uint8_t used;
} ram_fls_slot_t;

static uint8_t g_ram_fls_data[RAM_FLS_CAPACITY];
static ram_fls_slot_t g_ram_fls_slots[FLS_DATA_MAP_SIZE];
static uint32_t g_ram_fls_used;

static int32_t ram_FLS_FindSlot(const uint8_t ds_type, const uint8_t slot_num)
{
    uint32_t index;

    for (index = FLS_ZERO_U32; index < FLS_DATA_MAP_SIZE; index++)
    {
        if ((FLS_FLAG_ON_U8 == g_ram_fls_slots[index].used)
            && (ds_type == g_ram_fls_slots[index].ds_type)
            && (slot_num == g_ram_fls_slots[index].slot_num))
        {
            return (int32_t)index;
        }
    }

    return -1;
}

static int32_t ram_FLS_FindFreeSlot(void)
{
    uint32_t index;

    for (index = FLS_ZERO_U32; index < FLS_DATA_MAP_SIZE; index++)
    {
        if (FLS_ZERO_U8 == g_ram_fls_slots[index].used)
        {
            return (int32_t)index;
        }
    }

    return -1;
}

static void ram_FLS_Compact(void)
{
    uint32_t destination;
    uint32_t lower_bound;
    uint32_t old_used;
    uint32_t source;
    uint32_t size;
    uint32_t byte_index;
    uint32_t best_offset;
    uint32_t index;
    uint32_t best_index;

    destination = FLS_ZERO_U32;
    lower_bound = FLS_ZERO_U32;
    old_used = g_ram_fls_used;

    for (;;)
    {
        best_offset = RAM_FLS_INVALID_OFFSET;
        best_index = FLS_DATA_MAP_SIZE;
        for (index = FLS_ZERO_U32; index < FLS_DATA_MAP_SIZE; index++)
        {
            if ((FLS_FLAG_ON_U8 == g_ram_fls_slots[index].used)
                && (lower_bound <= g_ram_fls_slots[index].offset)
                && (g_ram_fls_slots[index].offset < best_offset))
            {
                best_offset = g_ram_fls_slots[index].offset;
                best_index = index;
            }
        }

        if (FLS_DATA_MAP_SIZE == best_index)
        {
            break;
        }

        source = best_offset;
        size = g_ram_fls_slots[best_index].size;
        if (destination < source)
        {
            for (byte_index = FLS_ZERO_U32; byte_index < size; byte_index++)
            {
                g_ram_fls_data[destination + byte_index] = g_ram_fls_data[source + byte_index];
            }
        }

        g_ram_fls_slots[best_index].offset = destination;
        lower_bound = source + size;
        destination += size;
    }

    for (byte_index = destination; byte_index < old_used; byte_index++)
    {
        g_ram_fls_data[byte_index] = FLS_ZERO_U8;
    }
    g_ram_fls_used = destination;
}

static void ram_FLS_Clear(void)
{
    uint32_t index;

    for (index = FLS_ZERO_U32; index < RAM_FLS_CAPACITY; index++)
    {
        g_ram_fls_data[index] = FLS_ZERO_U8;
    }
    for (index = FLS_ZERO_U32; index < FLS_DATA_MAP_SIZE; index++)
    {
        g_ram_fls_slots[index].offset = FLS_ZERO_U32;
        g_ram_fls_slots[index].size = FLS_ZERO_U16;
        g_ram_fls_slots[index].ds_type = FLS_INVALID_DS_TYPE;
        g_ram_fls_slots[index].slot_num = FLS_INVALID_SLOT_NUM;
        g_ram_fls_slots[index].used = FLS_ZERO_U8;
    }
    g_ram_fls_used = FLS_ZERO_U32;
}

static uint32_t ram_FLS_CheckSecureTarget(const uint8_t target_flash, const uint8_t target_area)
{
    if (((uint32_t)FLS_SELECT_ICUM_DF != (uint32_t)target_flash)
        || ((uint32_t)FLS_SELECT_SECURE != (uint32_t)target_area))
    {
        return ER_MM_FLS_UNAVAILABLE;
    }

    return ER_OK;
}

uint32_t mm_FLS_ConvertionError(const uint32_t err)
{
    switch (err)
    {
        case ER_L_FLS_ECC1EDF1:
            return ER_OK;
        case ER_L_FLS_ECC2EDF1:
            return ER_MM_FLS_DATA_INVALID;
        case ER_L_FLS_FLOW:
        case ER_L_FLS_REJECTED:
        case ER_L_FLS_INVALID_DATA:
        case ER_L_FLS_PROTECTION:
            return ER_MM_FLS_SYS;
        case ER_L_FLS_INTERNAL:
        case ER_L_FLS_CANCEL:
        case ER_L_FLS_FACI_TIME_OUT:
            return ER_MM_FLS_SYS_CRITICAL;
        case ER_L_FLS_PROGRAM:
            return ER_MM_FLS_PROG;
        case ER_L_FLS_UNAVAILABLE:
            return ER_MM_FLS_UNAVAILABLE;
        case ER_ML_DVC_ALIGN:
            return ER_MM_FLS_AES_PRM_ERROR;
        case ER_ML_DVC_AES_TIMEOUT:
            return ER_MM_FLS_AES_TIME_OUT;
        case ER_L_FLS_REMAP:
            return ER_MM_FLS_MEM_SOURCE;
        default:
            return err;
    }
}

uint32_t MM_FLS_Init(void)
{
    ram_FLS_Clear();
    return ER_OK;
}

uint32_t MM_FLS_Read(uint8_t target_flash, uint8_t target_area, uint8_t ds_type, uint8_t slot_num,
                     uint16_t size, uint8_t *p_data)
{
    int32_t slot_index;
    uint32_t index;
    uint32_t err;

    err = ram_FLS_CheckSecureTarget(target_flash, target_area);
    if ((ER_OK != err) || (NULL == p_data) || (FLS_ZERO_U16 == size))
    {
        return (ER_OK == err) ? ER_PARAM : err;
    }

    slot_index = ram_FLS_FindSlot(ds_type, slot_num);
    if (0 > slot_index)
    {
        return ER_MM_FLS_DATA_BLANK;
    }
    if (size > g_ram_fls_slots[slot_index].size)
    {
        return ER_MM_FLS_SIZE;
    }

    for (index = FLS_ZERO_U32; index < size; index++)
    {
        p_data[index] = g_ram_fls_data[g_ram_fls_slots[slot_index].offset + index];
    }

    return ER_OK;
}

uint32_t MM_FLS_Write(r_icumif_job_id_t job_id, uint8_t target_flash, uint8_t target_area,
                      uint8_t ds_type, uint8_t slot_num, uint16_t size, uint8_t *p_data)
{
    int32_t slot_index;
    uint32_t index;
    uint32_t offset;
    uint32_t previous_size;
    uint32_t err;

    (void)job_id;
    err = ram_FLS_CheckSecureTarget(target_flash, target_area);
    if ((ER_OK != err) || (NULL == p_data) || (FLS_ZERO_U16 == size))
    {
        return (ER_OK == err) ? ER_PARAM : err;
    }
    if (size > RAM_FLS_CAPACITY)
    {
        return ER_MM_FLS_SIZE;
    }

    slot_index = ram_FLS_FindSlot(ds_type, slot_num);
    if (0 <= slot_index)
    {
        previous_size = g_ram_fls_slots[slot_index].size;
        if (size == previous_size)
        {
            offset = g_ram_fls_slots[slot_index].offset;
        }
        else
        {
            if (size > (RAM_FLS_CAPACITY - (g_ram_fls_used - previous_size)))
            {
                return ER_MM_FLS_PROG;
            }
            g_ram_fls_slots[slot_index].used = FLS_ZERO_U8;
            g_ram_fls_used -= previous_size;
            ram_FLS_Compact();
            offset = g_ram_fls_used;
            g_ram_fls_used += size;
            g_ram_fls_slots[slot_index].offset = offset;
            g_ram_fls_slots[slot_index].size = size;
            g_ram_fls_slots[slot_index].used = FLS_FLAG_ON_U8;
        }
    }
    else
    {
        slot_index = ram_FLS_FindFreeSlot();
        if (0 > slot_index)
        {
            return ER_MM_FLS_FULL_DATA_MAP;
        }
        if (size > (RAM_FLS_CAPACITY - g_ram_fls_used))
        {
            ram_FLS_Compact();
            if (size > (RAM_FLS_CAPACITY - g_ram_fls_used))
            {
                return ER_MM_FLS_PROG;
            }
        }
        offset = g_ram_fls_used;
        g_ram_fls_used += size;
        g_ram_fls_slots[slot_index].offset = offset;
        g_ram_fls_slots[slot_index].size = size;
        g_ram_fls_slots[slot_index].ds_type = ds_type;
        g_ram_fls_slots[slot_index].slot_num = slot_num;
        g_ram_fls_slots[slot_index].used = FLS_FLAG_ON_U8;
    }

    for (index = FLS_ZERO_U32; index < size; index++)
    {
        g_ram_fls_data[offset + index] = p_data[index];
    }

    return ER_OK;
}

uint32_t MM_FLS_Erase(r_icumif_job_id_t job_id, uint8_t target_flash, uint8_t target_area,
                      uint8_t ds_type, uint8_t slot_num)
{
    int32_t slot_index;
    uint32_t index;
    uint32_t err;

    (void)job_id;
    err = ram_FLS_CheckSecureTarget(target_flash, target_area);
    if (ER_OK != err)
    {
        return err;
    }

    slot_index = ram_FLS_FindSlot(ds_type, slot_num);
    if (0 > slot_index)
    {
        return ER_MM_FLS_DATA_BLANK;
    }
    for (index = 0; index < g_ram_fls_slots[slot_index].size; index++)
    {
        g_ram_fls_data[g_ram_fls_slots[slot_index].offset + index] = FLS_ZERO_U8;
    }
    g_ram_fls_slots[slot_index].used = FLS_ZERO_U8;
    ram_FLS_Compact();
    return ER_OK;
}

uint32_t MM_FLS_WriteFlashCfg(r_icumif_job_id_t job_id, uint8_t target_func, uint32_t set_data)
{
    (void)job_id;
    (void)target_func;
    (void)set_data;
    return ER_MM_FLS_UNAVAILABLE;
}

uint32_t MM_FLS_MapDataSet(void)
{
    uint32_t index;

    for (index = 0; index < FLS_DATA_MAP_SIZE; index++)
    {
        if (FLS_FLAG_ON_U8 == g_ram_fls_slots[index].used)
        {
            return ER_OK;
        }
    }
    return ER_MM_FLS_ALL_BLANK;
}

uint32_t MM_FLS_EraseCF(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                        uint8_t target_area, const uint32_t cf_addr, uint32_t size)
{
    (void)job_id;
    (void)service_priority;
    (void)target_area;
    (void)cf_addr;
    (void)size;
    return ER_MM_FLS_UNAVAILABLE;
}

uint32_t MM_FLS_WriteCF(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                        uint8_t target_area, const uint32_t cf_addr, uint32_t size, const uint32_t *p_data)
{
    (void)job_id;
    (void)service_priority;
    (void)target_area;
    (void)cf_addr;
    (void)size;
    (void)p_data;
    return ER_MM_FLS_UNAVAILABLE;
}

uint32_t MM_FLS_CollectGarbage(r_icumif_job_id_t job_id)
{
    (void)job_id;
    return ER_MM_FLS_UNAVAILABLE;
}

uint32_t MM_FLS_CollectGarbageAll(r_icumif_job_id_t job_id)
{
    (void)job_id;
    return ER_MM_FLS_UNAVAILABLE;
}

uint32_t MM_FLS_DeleteMapData(uint8_t ds_type, uint8_t slot_num)
{
    int32_t slot_index;
    uint32_t index;

    slot_index = ram_FLS_FindSlot(ds_type, slot_num);
    if (0 <= slot_index)
    {
        for (index = 0; index < g_ram_fls_slots[slot_index].size; index++)
        {
            g_ram_fls_data[g_ram_fls_slots[slot_index].offset + index] = FLS_ZERO_U8;
        }
        g_ram_fls_slots[slot_index].used = FLS_ZERO_U8;
        ram_FLS_Compact();
    }
    return ER_OK;
}

uint8_t MM_FLS_GET_FormatPhase(void)
{
    return FLS_PHASE_0;
}

uint8_t MM_FLS_GET_WritePhase(void)
{
    return FLS_PHASE_0;
}

uint32_t MM_FLS_Format(r_icumif_job_id_t job_id)
{
    (void)job_id;
    ram_FLS_Clear();
    return ER_OK;
}

uint32_t MM_FLS_Config(void)
{
    return ER_OK;
}

uint32_t MM_FLS_GetFreeCapacity(free_block_t *p_free_block)
{
    uint32_t index;

    if (NULL == p_free_block)
    {
        return ER_PARAM;
    }
    p_free_block->total_free_block_num = (flash_block_idx_t)((RAM_FLS_CAPACITY - g_ram_fls_used) / FLS_DF_BLOCK_SIZE);
    for (index = 0; index < FLS_NB_BLOCK; index++)
    {
        p_free_block->free_block_num[index] = FLS_ZERO_U16;
    }
    return ER_OK;
}

uint32_t MM_FLS_GetFlashResource(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                                 uint8_t target_flash, uint8_t target_area, uint32_t addr, uint32_t size)
{
    (void)job_id;
    (void)service_priority;
    (void)target_flash;
    (void)target_area;
    (void)addr;
    (void)size;
    return ER_MM_FLS_UNAVAILABLE;
}

uint32_t MM_FLS_FreeFlashResource(uint8_t target_flash, uint8_t target_area, uint32_t addr, uint32_t size)
{
    (void)target_flash;
    (void)target_area;
    (void)addr;
    (void)size;
    return ER_MM_FLS_UNAVAILABLE;
}

uint32_t MM_FLS_WritePartitionInfo(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                                  uint8_t target, uint32_t info, uint32_t *p_data)
{
    (void)job_id;
    (void)service_priority;
    (void)target;
    (void)info;
    (void)p_data;
    return ER_MM_FLS_UNAVAILABLE;
}

uint32_t MM_FLS_ReadPartitionInfo(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                                  uint8_t target, uint32_t *p_info, uint32_t *p_data)
{
    (void)job_id;
    (void)service_priority;
    (void)target;
    (void)p_info;
    (void)p_data;
    return ER_MM_FLS_UNAVAILABLE;
}

uint32_t MM_FLS_CheckRamExecRequired(uint8_t target_flash, uint32_t *p_addr, uint8_t *p_result)
{
    (void)target_flash;
    (void)p_addr;
    if (NULL == p_result)
    {
        return ER_PARAM;
    }
    *p_result = (uint8_t)false;
    return ER_OK;
}

uint32_t MM_FLS_ReadUser(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                         uint8_t target_flash, uint8_t target_area, uint32_t *p_addr,
                         uint32_t size, uint8_t *p_data)
{
    (void)job_id;
    (void)service_priority;
    (void)target_flash;
    (void)target_area;
    (void)p_addr;
    (void)size;
    (void)p_data;
    return ER_MM_FLS_UNAVAILABLE;
}

uint32_t MM_FLS_EraseUser(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                          uint8_t target_flash, uint8_t target_area, uint32_t *p_addr, uint32_t size)
{
    (void)job_id;
    (void)service_priority;
    (void)target_flash;
    (void)target_area;
    (void)p_addr;
    (void)size;
    return ER_MM_FLS_UNAVAILABLE;
}

uint32_t MM_FLS_WriteUser(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                          uint8_t target_flash, uint8_t target_area, uint32_t *p_addr,
                          uint32_t size, const uint32_t *p_data)
{
    (void)job_id;
    (void)service_priority;
    (void)target_flash;
    (void)target_area;
    (void)p_addr;
    (void)size;
    (void)p_data;
    return ER_MM_FLS_UNAVAILABLE;
}

uint32_t MM_FLS_BlankCheckUser(r_icumif_job_id_t job_id, r_icumif_service_priority_t service_priority,
                               uint8_t target_flash, uint8_t target_area, uint32_t *p_addr,
                               uint32_t size, uint8_t *p_is_blank)
{
    (void)job_id;
    (void)service_priority;
    (void)target_flash;
    (void)target_area;
    (void)p_addr;
    (void)size;
    (void)p_is_blank;
    return ER_MM_FLS_UNAVAILABLE;
}
