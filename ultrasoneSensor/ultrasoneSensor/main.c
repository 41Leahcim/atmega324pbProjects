/*
 * ultrasoneSensor.c
 *
 * Created: 11-2-2022 10:42:14
 * Author : micha
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

void timer1_init(){
	TCCR1A = 0;
	TCCR1B = (1 << CS11);
}

void usart_init(uint32_t baudRate){
	UCSR1A = 0;
	UCSR1B = (1 << TXEN);
	UCSR1C = (3 << UCSZ0);
	UBRR1 = F_CPU / (baudRate << 4) - 1;
}

void usart_sendChar(char c){
	while(~UCSR1A & (1 << UDRE));
	UDR1 = c;
}

void usart_sendString(const char *string){
	while(*string != '\0') usart_sendChar(*(string++));
}

void usart_sendUint16(uint16_t number){
	char string[6];
	uint8_t i = 5;
	string[5] = '\0';
	do{
		string[--i] = '0' + number % 10;
		number /= 10;
	}while(number > 0);
	usart_sendString(string);
}

int main(void){
	timer1_init();
	usart_init(9600);
	DDRB = (DDRB & ~(1 << PORTB2)) | (1 << PORTB3);
    while (1){
		PORTB |= (1 << PORTB3);
		_delay_ms(10);
		PORTB &= ~(1 << PORTB3);
		TCNT1 = 0;
		while(~PINB & (1 << PORTB2));
		while(PINB & (1 << PORTB2));
		usart_sendString("\r        \r");
		usart_sendUint16(TCNT1 / 116);
		_delay_ms(60);
    }
}

