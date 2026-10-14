/*
 * nvm_block.h
 *
 *  Created on: 09-Oct-2026
 *      Author: acer
 */

#ifndef NVM_BLOCK_H_
#define NVM_BLOCK_H_
#include <stdint.h>

#define NVM_MAX_BLOCKS       10U
#define NVM_EEPROM_SIZE      512U
#define NVM_PAGE_SIZE        8U

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint8_t  Std_ReturnType;

#define E_OK       ((Std_ReturnType)0U)
#define E_NOT_OK   ((Std_ReturnType)1U)

typedef struct
{
    uint16 BlockId;
    uint16 EepromOffset;
    uint16 Size;
    uint8 *RamMirrorPtr;
} NvM_BlockDescType;

extern NvM_BlockDescType NvM_BlockTable[NVM_MAX_BLOCKS];
extern volatile uint16 NvM_LastDetError;
extern volatile uint16 Det_ErrorCount;

Std_ReturnType NvM_RegisterBlock(
    uint16 BlockId,
    uint16 Offset,
    uint16 Size);

Std_ReturnType NvM_ReadVariable(
    uint16 BlockId,
    void *DestRamPtr);

Std_ReturnType NvM_WriteVariable(
    uint16 BlockId,
    const void *SrcRamPtr);





#endif /* NVM_BLOCK_H_ */
