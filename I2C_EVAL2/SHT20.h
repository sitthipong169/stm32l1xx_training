#ifndef SHT20_H_
#define SHT20_H_

#include "stm32l1xx.h"
#include <stdbool.h>
#include "stm32l1xx_i2c.h"

bool SHT20ReadTemperature(I2C_TypeDef* i2c, float* raw);
bool SHT20ReadHumidity(I2C_TypeDef* i2c, float* raw);
bool SHT20CheckCRC(uint16_t data, uint8_t crc_value);
#endif