#ifndef SRC_WIRE_H_
#define SRC_WIRE_H_

#include "stm32f3xx.h"

/*Funkcja przesyła sekwencję reset przez 1-wire.
@return Zwraca HAL_OK/HAL_ERROR w zależności od odpowiedzi czujnika
*/
HAL_StatusTypeDef wire_reset(void);

/* Funkcja odczytuje dane przez 1-wire (odczytuje 8 bitów i łaczy w bajt)
@return Zwraca odczytany bajt
*/
uint8_t wire_read(void);

/* Funkcja ta wysyła dane przez 1-wire.
 @param byte - bajt do wysłania
 */
void wire_write(uint8_t byte);

/*Funkcja liczy sumę kontrolną
@param data - blok danych
@param len - długość bloku danych
@return Zwraca sumę kontrolną
*/
uint8_t wire_crc(const uint8_t* data, int len);

#endif /* SRC_WIRE_H_ */
