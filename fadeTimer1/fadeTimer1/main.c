/*
 * fadeTimer1.c
 *
 * Created: 13-2-2022 21:10:49
 * Author : micha
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

int main(void){
	int8_t direction = 1;
	// set PORTD5 (OC1A) as output
	DDRD |= (1 << PORTD5);
	
	// 10 bit pwm, set at bottom, clear at compare match OCR1A
	TCCR1A = (1 << COM1A1) | (3 << WGM10);
	TCCR1B = (1 << WGM12) | (1 << CS10);
	OCR1A = 0;
    while (1){
		_delay_ms(1);
		if((direction < 0 && OCR1A < -direction) || (direction > 0 && OCR1A > 1023 - direction)){
			direction = -direction;
		}
		OCR1A += direction;
    }
}

