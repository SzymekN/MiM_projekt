#ifndef SRC_DS18B20_H_
#define SRC_DS18B20_H_


#include "stm32f3xx.h"

#define DS18B20_ROM_CODE_SIZE		8  // wielkość tablicy przechowującej adres czujnika

// Odczytaj adres czujnika i policz CRC
// return - HAL_OK/HAL_ERROR
HAL_StatusTypeDef ds18b20_read_address(uint8_t* rom_code);

// Rozpocznij pomiar temperatury
// return - HAL_OK/HAL_ERROR
HAL_StatusTypeDef ds18b20_start_measure();

// Pobierz temperaturę
// return - temperatura w stopniach Celsjusza
float ds18b20_get_temp();


#endif /* SRC_DS18B20_H_ */
