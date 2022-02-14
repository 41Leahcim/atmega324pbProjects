/*
 * voltageMeter.c
 *
 * Created: 13-2-2022 14:38:16
 * Author : micha
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(ADC_vect){}

void usart_init(uint32_t baudRate){
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
	char buffer[6];
	uint8_t i = 5;
	buffer[5] = '\0';
	do{
		buffer[--i] = '0' + number % 10;
		number /= 10;
	}while(number > 0);
	usart_sendString(buffer + i);
}

uint16_t adc_read(uint8_t pin){
	ADMUX = (1 << REFS0) | (pin & 7);
	ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADIF) | (1 << ADIE) | (7 << ADPS0);
	sei();
	SMCR = (1 << SM0) | (1 << SE);
	while(ADCSRA & (1 << ADSC));
	SMCR &= ~(1 << SE);
	ADCSRA &= ~(1 << ADEN);
	return ADC;
}

int main(void){
	uint32_t voltage = 0, lastVoltage = 0;
	uint8_t i, divider;
	usart_init(9600);
	DDRA &= ~(1 << PORTA0);
	while (1){
		voltage = adc_read(0);
		for(i = 1;i < 50;i++){
			voltage += adc_read(0);
		}
		voltage = voltage * 10 / 1024;
		if(voltage != lastVoltage){
			usart_sendUint16(voltage / 100);
			usart_sendChar('.');
			for(divider = 10;divider > 0;divider /= 10){
				usart_sendChar('0' + voltage / divider % 10);
			}
			usart_sendString(" V\r");
			lastVoltage = voltage;
		}
    }
}

