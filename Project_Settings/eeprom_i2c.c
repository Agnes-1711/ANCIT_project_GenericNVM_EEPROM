/*
 * eeprom_i2c.c
 *
 *  Created on: 09-Oct-2026
 *      Author: acer
 */

#include "eeprom_i2c.h"
#include <stddef.h>
#include <string.h>

#define EEPROM_CAPACITY 512U
#define EEPROM_PAGE_SIZE 8U

static uint8_t EepromMemory[EEPROM_CAPACITY];
static uint8_t EepromInitialized = 0U;

volatile uint16_t Eeprom_WriteCount = 0U;

void EEPROM_I2C_Init(void)
{
    if (EepromInitialized == 0U)
    {
        memset(EepromMemory, 0xFF, sizeof(EepromMemory));
        EepromInitialized = 1U;
        Eeprom_WriteCount = 0U;
    }
}

EepromStatusType EEPROM_ReadBytes(
    uint16_t Offset,
    uint8_t *Dest,
    uint16_t Size)
{
    if ((Dest == NULL) ||
        (Size == 0U) ||
        (Offset >= EEPROM_CAPACITY) ||
        (Size > (EEPROM_CAPACITY - Offset)))
    {
        return EEPROM_NOT_OK;
    }

    memcpy(Dest, &EepromMemory[Offset], Size);
    return EEPROM_OK;
}

EepromStatusType EEPROM_WriteBytes(
    uint16_t Offset,
    const uint8_t *Src,
    uint16_t Size)
{
    uint16_t pageRemaining;

    if ((Src == NULL) ||
        (Size == 0U) ||
        (Offset >= EEPROM_CAPACITY) ||
        (Size > (EEPROM_CAPACITY - Offset)))
    {
        return EEPROM_NOT_OK;
    }

    pageRemaining = EEPROM_PAGE_SIZE -
                    (Offset % EEPROM_PAGE_SIZE);

    /* Each driver call must stay inside one 8-byte page. */
    if (Size > pageRemaining)
    {
        return EEPROM_NOT_OK;
    }

    memcpy(&EepromMemory[Offset], Src, Size);
    Eeprom_WriteCount++;

    return EEPROM_OK;
}


