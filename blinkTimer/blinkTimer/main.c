/*
 * blinkTimer.c
 *
 * Created: 11-2-2022 00:52:27
 * Author : micha
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

int main(void){
	DDRD |= (1 << PORTD5);
	TCCR1A = (1<< COM0A0);
	TCCR1B = (5 << CS10) | (1 << WGM12);
	// CPU FREQ (16MHZ) / prescaler (1024) / maximum value + 1 (15625 + 1 = 15626) = 
	OCR1A = 15625;
    while (1) {
    }
}

