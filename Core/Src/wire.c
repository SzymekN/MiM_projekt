#include "usart.h"
#include "wire.h"
#include "gpio.h"

static void set_baudrate(uint32_t baudrate)
{
  huart3.Instance = USART3;
  huart3.Init.BaudRate = baudrate;          //ustawienie pr�dko�ci z jak� ma pracowa� UART3 za pomoc� zmiennej baudrate przekazywanej w argumencie funkcji
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  huart3.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;

  //sprawdzenie czy otrzymana warto�� z funkcji HAL_HalfDuplex_Init zwraca warto�� r�n� od HAL_OK, je�li tak wyrzucany jest b��d, w przeciwnym razie program kontynuuje dalsz� prac�  
  if (HAL_HalfDuplex_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
}


HAL_StatusTypeDef wire_reset(void)
{
  uint8_t data_out = 0xF0; //zmienna przechowuj�ca to co zostanie przes�ane przez UART (ni�sze cztery bity maj� warto�� 0, wy�sze - 1)
  uint8_t data_in = 0; //zmienna przechowuj�ca to co czujnik ds18b20 zinterpetuje jako sygna� reset

  set_baudrate(9600); //wysy�anie sygna�u z pr�dko�ci� 9600 bit�w na sekund� przez UART
  HAL_UART_Transmit(&huart3, &data_out, 1, HAL_MAX_DELAY);
  HAL_UART_Receive(&huart3, &data_in, 1, HAL_MAX_DELAY);
  set_baudrate(115200); //przy tej pr�dko�ci bit�w na sekund� UART generuje przebiegi pasuj�ce do transmisji przez 1-wire bit�w o warto�ci 0 i 1 

  //je�li uzyskana zosta�a odpowied� od czujnika zwracana jest warto�� HAL_OK, w przeciwnym razie zwracana jest warto�� HAL_ERROR
  if (data_in != 0xF0)
    return HAL_OK;
  else
    return HAL_ERROR;
}

static int read_bit(void)
{
  uint8_t data_out = 0xFF; //zmienna przechowuj�ca to co zostanie przes�ane przez UART (ni�sze cztery bity maj� warto�� 0, wy�sze - 1)
  uint8_t data_in = 0;  //zmienna przechowuj�ca to co czujnik ds18b20 zinterpetuje jako sygna� reset
  HAL_UART_Transmit(&huart3, &data_out, 1, HAL_MAX_DELAY);
  HAL_UART_Receive(&huart3, &data_in, 1, HAL_MAX_DELAY);

  return data_in & 0x01;
}

uint8_t wire_read(void)
{
  uint8_t value = 0; //zmienna, kt�ra przechowa odczytany bajt 
  int i; // zmienna umo�liwiaj�ca zako�czenie dzia�ania p�tli
  for (i = 0; i < 8; i++) {
    value >>= 1; //przesuni�cie si� o jeden bit w prawo
    if (read_bit())
      value |= 0x80; //je�li na danym bicie nie znajduje si� 1, wtedy nast�puje ustawienie 1 na �smym bicie, w przeciwnym razie nic si� nie dzieje 
  }
  return value;
}

static void write_bit(int value)
{
  if (value) {
      uint8_t data_out = 0xff;//zmienna przechowuj�ca to co zostanie przes�ane przez UART
    HAL_UART_Transmit(&huart3, &data_out, 1, HAL_MAX_DELAY);
  } else {
      uint8_t data_out = 0x0;//zmienna przechowuj�ca to co zostanie przes�ane przez UART
    HAL_UART_Transmit(&huart3, &data_out, 1, HAL_MAX_DELAY);
  }
}

void wire_write(uint8_t byte)
{
  int i; // zmienna umo�liwiaj�ca zako�czenie dzia�ania p�tli
  for (i = 0; i < 8; i++) {
    write_bit(byte & 0x01); //zapisywanie pojedynczych bit�w 
    byte >>= 1; //przesuni�cie si� o jeden bit w prawo
  }
}

static uint8_t byte_crc(uint8_t crc, uint8_t byte)
{
  int i; // zmienna umo�liwiaj�ca zako�czenie dzia�ania p�tli
  for (i = 0; i < 8; i++) {
    uint8_t b = crc ^ byte; //je�li poszczeg�lne bity w zmiennej crc oraz byte s� sobie r�wne, to w  miennej b na odpowiedniej pozycji pojawia si� 1, w przeciwym przypadku 0
    crc >>= 1;  //przesuni�cie si� o jeden bit w prawo
    if (b & 0x01)
      crc ^= 0x8c; //ta sama sytacja jak w przypadku przypisania warto�ci do zmiennej b
    byte >>= 1;  //przesuni�cie si� o jeden bit w prawo
  }
  return crc;
}

uint8_t wire_crc(const uint8_t* data, int len)
{
   int i; // zmienna, kt�ra umo�liwia zako�czenie dzia�ania p�tli oraz przesuwanie si� po kolejnych elementach bloku o nazwie data
   uint8_t crc = 0; //zmiennna przechowuj�ca warto�ci zwracane z funkcji byte_crc
   
   for (i = 0; i < len; i++)    //p�tla przechodz�ca po kolejnych elementach bloku danych
     crc = byte_crc(crc, data[i]); //zapisanie warto�ci zwracanej z funkcji byte_crc do zmiennej crc 

   return crc;
}
