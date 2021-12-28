/*
 * ds18b20.c
 *
 *  Created on: 27 gru 2021
 *      Author: Szymek
 */


#include <string.h>
#include "ds18b20.h"
#include "wire.h"

#define DS18B20_SCRATCHPAD_SIZE    9

#define DS18B20_READ_ROM           0x33
#define DS18B20_SKIP_ROM           0xCC

#define DS18B20_CONVERT_T          0x44
#define DS18B20_READ_SCRATCHPAD    0xBE

//HAL_StatusTypeDef ds18b20_init(void)
//{
//  return wire_init();
//}

HAL_StatusTypeDef ds18b20_read_address(uint8_t* rom_code)
{
  int i;
  uint8_t crc;

  if (wire_reset() != HAL_OK)
    return HAL_ERROR;

  wire_write(DS18B20_READ_ROM);

  for (i = 0; i < DS18B20_ROM_CODE_SIZE; i++)
    rom_code[i] = wire_read();

  crc = wire_crc(rom_code, DS18B20_ROM_CODE_SIZE - 1);
  if (rom_code[DS18B20_ROM_CODE_SIZE - 1] == crc)
    return HAL_OK;
  else
    return HAL_ERROR;
}

static HAL_StatusTypeDef send_cmd(uint8_t cmd)
{
  if (wire_reset() != HAL_OK)
    return HAL_ERROR;

  wire_write(DS18B20_SKIP_ROM);

  wire_write(cmd);
  return HAL_OK;
}

HAL_StatusTypeDef ds18b20_start_measure()
{
  return send_cmd(DS18B20_CONVERT_T);
}

static HAL_StatusTypeDef ds18b20_read_scratchpad(uint8_t* scratchpad)
{
  int i;
  uint8_t crc;

  if (send_cmd(DS18B20_READ_SCRATCHPAD) != HAL_OK)
    return HAL_ERROR;

  for (i = 0; i < DS18B20_SCRATCHPAD_SIZE; i++)
    scratchpad[i] = wire_read();

  crc = wire_crc(scratchpad, DS18B20_SCRATCHPAD_SIZE - 1);
  if (scratchpad[DS18B20_SCRATCHPAD_SIZE - 1] == crc)
    return HAL_OK;
  else
    return HAL_ERROR;
}

float ds18b20_get_temp()
{
  uint8_t scratchpad[DS18B20_SCRATCHPAD_SIZE];
  int16_t temp;

  if (ds18b20_read_scratchpad(scratchpad) != HAL_OK)
    return 85.0f;

  memcpy(&temp, &scratchpad[0], sizeof(temp));

  return temp / 16.0f;
}
