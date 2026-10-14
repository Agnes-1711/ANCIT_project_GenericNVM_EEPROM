/*
 * nvm_block.c
 *
 *  Created on: 09-Oct-2026
 *      Author: acer
 */

#include "nvm_block.h"
#include "eeprom_i2c.h"
#include <stddef.h>
#include <string.h>

/*
 * Implement these two functions in eeprom_i2c.c
 * using your SDK's configured LPI2C driver.
 */


/* Simplified DET reporting hook for this exercise. */
volatile uint16 NvM_LastDetError = 0U;
volatile uint16 Det_ErrorCount = 0U;

#define NVM_DET_INVALID_BLOCK  1U
#define NVM_DET_INVALID_RANGE  2U
#define NVM_DET_NULL_POINTER   3U
#define NVM_DET_TABLE_FULL     4U
#define NVM_DET_EEPROM_FAILURE  5U

static void NvM_ReportError(uint16 ErrorId)
{
    NvM_LastDetError = ErrorId;
    Det_ErrorCount++;
}

NvM_BlockDescType NvM_BlockTable[NVM_MAX_BLOCKS];

static int32_t NvM_FindBlock(uint16 BlockId)
{
    uint16 i;

    for (i = 0U; i < NVM_MAX_BLOCKS; i++)
    {
        if ((NvM_BlockTable[i].Size != 0U) &&
            (NvM_BlockTable[i].BlockId == BlockId))
        {
            return (int32_t)i;
        }
    }

    return -1;
}

Std_ReturnType NvM_RegisterBlock(
    uint16 BlockId,
    uint16 Offset,
    uint16 Size)
{
    uint16 i;

    if ((Size == 0U) ||
        (Offset >= NVM_EEPROM_SIZE) ||
        (Size > (NVM_EEPROM_SIZE - Offset)))
    {
        NvM_ReportError(NVM_DET_INVALID_RANGE);
        return E_NOT_OK;
    }

    if (NvM_FindBlock(BlockId) >= 0)
    {
        NvM_ReportError(NVM_DET_INVALID_BLOCK);
        return E_NOT_OK;
    }

    /* Reject overlapping EEPROM regions. */
    for (i = 0U; i < NVM_MAX_BLOCKS; i++)
    {
        uint32_t newEnd;
        uint32_t oldEnd;

        if (NvM_BlockTable[i].Size == 0U)
        {
            continue;
        }

        newEnd = (uint32_t)Offset + Size;
        oldEnd = (uint32_t)NvM_BlockTable[i].EepromOffset
               + NvM_BlockTable[i].Size;

        if (((uint32_t)Offset < oldEnd) &&
            ((uint32_t)NvM_BlockTable[i].EepromOffset < newEnd))
        {
            NvM_ReportError(NVM_DET_INVALID_RANGE);
            return E_NOT_OK;
        }
    }

    for (i = 0U; i < NVM_MAX_BLOCKS; i++)
    {
        if (NvM_BlockTable[i].Size == 0U)
        {
            NvM_BlockTable[i].BlockId = BlockId;
            NvM_BlockTable[i].EepromOffset = Offset;
            NvM_BlockTable[i].Size = Size;
            NvM_BlockTable[i].RamMirrorPtr = NULL;
            return E_OK;
        }
    }

    NvM_ReportError(NVM_DET_TABLE_FULL);
    return E_NOT_OK;
}

Std_ReturnType NvM_ReadVariable(
    uint16 BlockId,
    void *DestRamPtr)
{
    int32_t index;
    NvM_BlockDescType *block;
    Std_ReturnType result;

    if (DestRamPtr == NULL)
    {
        NvM_ReportError(NVM_DET_NULL_POINTER);
        return E_NOT_OK;
    }

    index = NvM_FindBlock(BlockId);

    if (index < 0)
    {
        NvM_ReportError(NVM_DET_INVALID_BLOCK);
        return E_NOT_OK;
    }

    block = &NvM_BlockTable[index];

    result = EEPROM_ReadBytes(
        block->EepromOffset,
        (uint8 *)DestRamPtr,
        block->Size);

    if (result != E_OK)
    {
    	 NvM_ReportError(NVM_DET_EEPROM_FAILURE);
    	 return E_NOT_OK;
    }

    if (block->RamMirrorPtr != NULL)
    {
        memcpy(block->RamMirrorPtr, DestRamPtr, block->Size);
    }

    return E_OK;
}

Std_ReturnType NvM_WriteVariable(
    uint16 BlockId,
    const void *SrcRamPtr)
{
    int32_t index;
    NvM_BlockDescType *block;
    const uint8 *src;
    uint16 remaining;
    uint16 offset;
    uint16 chunk;

    if (SrcRamPtr == NULL)
    {
        NvM_ReportError(NVM_DET_NULL_POINTER);
        return E_NOT_OK;
    }

    index = NvM_FindBlock(BlockId);

    if (index < 0)
    {
        NvM_ReportError(NVM_DET_INVALID_BLOCK);
        return E_NOT_OK;
    }

    block = &NvM_BlockTable[index];

    src = (const uint8 *)SrcRamPtr;
    offset = block->EepromOffset;
    remaining = block->Size;

    while (remaining > 0U)
    {
        uint16 pageRemaining =
            NVM_PAGE_SIZE - (offset % NVM_PAGE_SIZE);

        chunk = (remaining < pageRemaining)
              ? remaining
              : pageRemaining;

        if (EEPROM_WriteBytes(offset, src, chunk) != E_OK)
        {
        	NvM_ReportError(NVM_DET_EEPROM_FAILURE);
        	return E_NOT_OK;
        }

        offset += chunk;
        src += chunk;
        remaining -= chunk;
    }

    if (block->RamMirrorPtr != NULL)
    {
        memcpy(block->RamMirrorPtr,
               SrcRamPtr,
               block->Size);
    }

    return E_OK;
}


