#include "lcd_1602.h"
#include "tim.h"

void delay_us (uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim6,0);  // ustaw timer6 na 0
	while (__HAL_TIM_GET_COUNTER(&htim6) < us){	};  // czekaj określoną ilość czasu
}

static void lcd_send_4bit(uint8_t data){	

	//sprawdzanie kolejno wartości bitów 5, 6, 7 i 8
	if(data & 0x10) d4(1); else d4(0);
	if(data & 0x20) d5(1); else d5(0);
	if(data & 0x40) d6(1); else d6(0);
	if(data & 0x80) d7(1); else d7(0);
	
}

static void lcd_send(int8_t rs,uint8_t data){
	// wysyłanie pierwszych 4 bitów
	rs(rs);
	rw(0);
	lcd_send_4bit(data);
	en(1);
	delay_us(100);
	en(0);
	// przesunięcie bitowe i przesłanie kolejnych 4 bitów
	lcd_send_4bit(data<<4);
	en(1);
	delay_us(100);
	en(0);
}

void lcd_init(void){

	lcd_send(0,0x28); // ustaw tryb 4 bitowy i dwie linie do wyświetlania danych
	lcd_send(0,0x33); // inicjalizacja stanu początkowego
	lcd_send(0,0x32); // wyłączenie mrugania kursora
	lcd_send(0,0x0C); // włącz wyświetlacz
	lcd_send(0,0x01); // czyszczenie wyświetlacza
	lcd_send(0,0x02); // ustawienie kursora na początek
	HAL_Delay(2);
}

void lcd_clear(void){
    lcd_send(0,0x01); // czyszczenie wyświetlacza
    HAL_Delay(2);
}


void lcd_move_cursor(char x, char y){
	// przesunięcie kursora
    lcd_send(0,0x80+x+(y*0x40));
}

void lcd_print(char *text){
	// dopóki dane w tablicy istnieją przesyłaj do wyświetlacza
    while(*text){
        lcd_send(1,(uint8_t)*text);
        text++;
    }
}
