/*
 * blinkTimer.c
 *
 * Created: 11-2-2022 00:52:27
 * Author : Michael Scholten
 * Description: Led on PORTD5 blinks with a frequency of ~1hz
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

int main(void){
	// set DDRD5 as output for the led
	DDRD |= (1 << PORTD5);
	
	// CTC, prescaler /1024
	TCCR1A = (1<< COM0A0);
	TCCR1B = (5 << CS10) | (1 << WGM12);

	// CPU FREQ (16MHZ) / prescaler (1024) / maximum value (15625) = output frequency (~1hz)
	OCR1A = 15625;

    while (1) {
    }
}

