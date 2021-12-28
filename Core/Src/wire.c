/*
 * wire.c
 *
 *  Created on: 27 gru 2021
 *      Author: Szymek
 */


#include "usart.h"
#include "wire.h"
#include "gpio.h"

static void set_baudrate(uint32_t baudrate)
{
  huart3.Instance = USART3;
  huart3.Init.BaudRate = baudrate;          //ustawienie prêdkoœci z jak¹ ma pracowaæ UART3 za pomoc¹ zmiennej baudrate przekazywanej w argumencie funkcji
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  huart3.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;

  //sprawdzenie czy otrzymana wartoœæ z funkcji HAL_HalfDuplex_Init zwraca wartoœæ ró¿n¹ od HAL_OK, jeœli tak wyrzucany jest b³¹d, w przeciwnym razie program kontynuuje dalsz¹ pracê  
  if (HAL_HalfDuplex_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
}


HAL_StatusTypeDef wire_reset(void)
{
  uint8_t data_out = 0xF0; //zmienna przechowuj¹ca to co zostanie przes³ane przez UART (ni¿sze cztery bity maj¹ wartoœæ 0, wy¿sze - 1)
  uint8_t data_in = 0; //zmienna przechowuj¹ca to co czujnik ds18b20 zinterpetuje jako sygna³ reset

  set_baudrate(9600); //wysy³anie sygna³u z prêdkoœci¹ 9600 bitów na sekundê przez UART
  HAL_UART_Transmit(&huart3, &data_out, 1, HAL_MAX_DELAY);
  HAL_UART_Receive(&huart3, &data_in, 1, HAL_MAX_DELAY);
  set_baudrate(115200); //przy tej prêdkoœci bitów na sekundê UART generuje przebiegi pasuj¹ce do transmisji przez 1-wire bitów o wartoœci 0 i 1 

  //jeœli uzyskana zosta³a odpowiedŸ od czujnika zwracana jest wartoœæ HAL_OK, w przeciwnym razie zwracana jest wartoœæ HAL_ERROR
  if (data_in != 0xF0)
    return HAL_OK;
  else
    return HAL_ERROR;
}

static int read_bit(void)
{
  uint8_t data_out = 0xFF; //zmienna przechowuj¹ca to co zostanie przes³ane przez UART (ni¿sze cztery bity maj¹ wartoœæ 0, wy¿sze - 1)
  uint8_t data_in = 0;  //zmienna przechowuj¹ca to co czujnik ds18b20 zinterpetuje jako sygna³ reset
  HAL_UART_Transmit(&huart3, &data_out, 1, HAL_MAX_DELAY);
  HAL_UART_Receive(&huart3, &data_in, 1, HAL_MAX_DELAY);

  return data_in & 0x01;
}

uint8_t wire_read(void)
{
  uint8_t value = 0; //zmienna, która przechowa odczytany bajt 
  int i; // zmienna umo¿liwiaj¹ca zakoñczenie dzia³ania pêtli
  for (i = 0; i < 8; i++) {
    value >>= 1; //przesuniêcie siê o jeden bit w prawo
    if (read_bit())
      value |= 0x80; //jeœli na danym bicie nie znajduje siê 1, wtedy nastêpuje ustawienie 1 na ósmym bicie, w przeciwnym razie nic siê nie dzieje 
  }
  return value;
}

static void write_bit(int value)
{
  if (value) {
      uint8_t data_out = 0xff;//zmienna przechowuj¹ca to co zostanie przes³ane przez UART
    HAL_UART_Transmit(&huart3, &data_out, 1, HAL_MAX_DELAY);
  } else {
      uint8_t data_out = 0x0;//zmienna przechowuj¹ca to co zostanie przes³ane przez UART
    HAL_UART_Transmit(&huart3, &data_out, 1, HAL_MAX_DELAY);
  }
}

void wire_write(uint8_t byte)
{
  int i; // zmienna umo¿liwiaj¹ca zakoñczenie dzia³ania pêtli
  for (i = 0; i < 8; i++) {
    write_bit(byte & 0x01); //zapisywanie pojedynczych bitów 
    byte >>= 1; //przesuniêcie siê o jeden bit w prawo
  }
}

static uint8_t byte_crc(uint8_t crc, uint8_t byte)
{
  int i; // zmienna umo¿liwiaj¹ca zakoñczenie dzia³ania pêtli
  for (i = 0; i < 8; i++) {
    uint8_t b = crc ^ byte; //jeœli poszczególne bity w zmiennej crc oraz byte s¹ sobie równe, to w  miennej b na odpowiedniej pozycji pojawia siê 1, w przeciwym przypadku 0
    crc >>= 1;  //przesuniêcie siê o jeden bit w prawo
    if (b & 0x01)
      crc ^= 0x8c; //ta sama sytacja jak w przypadku przypisania wartoœci do zmiennej b
    byte >>= 1;  //przesuniêcie siê o jeden bit w prawo
  }
  return crc;
}

uint8_t wire_crc(const uint8_t* data, int len)
{
   int i; // zmienna, która umo¿liwia zakoñczenie dzia³ania pêtli oraz przesuwanie siê po kolejnych elementach bloku o nazwie data
   uint8_t crc = 0; //zmiennna przechowuj¹ca wartoœci zwracane z funkcji byte_crc
   
   for (i = 0; i < len; i++)    //pêtla przechodz¹ca po kolejnych elementach bloku danych
     crc = byte_crc(crc, data[i]); //zapisanie wartoœci zwracanej z funkcji byte_crc do zmiennej crc 

   return crc;
}
