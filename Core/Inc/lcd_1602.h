#ifndef __LCD_1602_H
#define __LCD_1602_H
#include "stm32f3xx_hal.h"
#include "main.h"

#define rs(x)      x?HAL_GPIO_WritePin(RS_GPIO_Port,RS_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(RS_GPIO_Port,RS_Pin,GPIO_PIN_RESET)
#define rw(x)      x?HAL_GPIO_WritePin(RW_GPIO_Port,RW_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(RW_GPIO_Port,RW_Pin,GPIO_PIN_RESET)
#define en(x)      x?HAL_GPIO_WritePin(EN_GPIO_Port,EN_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(EN_GPIO_Port,EN_Pin,GPIO_PIN_RESET)

#define d4(x)      x?HAL_GPIO_WritePin(D4_GPIO_Port,D4_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(D4_GPIO_Port,D4_Pin,GPIO_PIN_RESET)
#define d5(x)      x?HAL_GPIO_WritePin(D5_GPIO_Port,D5_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(D5_GPIO_Port,D5_Pin,GPIO_PIN_RESET)
#define d6(x)      x?HAL_GPIO_WritePin(D6_GPIO_Port,D6_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(D6_GPIO_Port,D6_Pin,GPIO_PIN_RESET)
#define d7(x)      x?HAL_GPIO_WritePin(D7_GPIO_Port,D7_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(D7_GPIO_Port,D7_Pin,GPIO_PIN_RESET)

// prześlij 4 bity danych do wyświetlacza
// data - dane do przesłania
static void lcd_send_4bit(uint8_t data);

// prześlij 8 bitów danych do wyświetlacza
// rs - komendy / manipulacja danymi
// data - dane do przesłania
static void lcd_send(int8_t rs,uint8_t data);
//void lcd_cmd(uint8_t command);
//void lcd_data(char c);

// inicjalizuj wyświetlacz
void lcd_init(void);

// wyczyść wyświetlacz
void lcd_clear(void);

// przesuń kursor
// x - pozycja horyzontalna kursora
// y - pozycja wertykalna kursora
void lcd_move_cursor(char x, char y);

// wyświetl tekst
// text - tekst do wyświetlenia
void lcd_print(char *text);

#endif
