/*
 * eeprom_i2c.h
 *
 *  Created on: 09-Oct-2026
 *      Author: acer
 */



#ifndef EEPROM_I2C_H
#define EEPROM_I2C_H

#include <stdint.h>

typedef uint8_t EepromStatusType;

#define EEPROM_OK       0U
#define EEPROM_NOT_OK   1U

void EEPROM_I2C_Init(void);

EepromStatusType EEPROM_ReadBytes(
    uint16_t Offset,
    uint8_t *Dest,
    uint16_t Size);

EepromStatusType EEPROM_WriteBytes(
    uint16_t Offset,
    const uint8_t *Src,
    uint16_t Size);





#endif /* EEPROM_I2C_H_ */
