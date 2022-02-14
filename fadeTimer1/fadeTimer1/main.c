/*
 * fadeTimer1.c
 *
 * Created: 13-2-2022 21:10:49
 * Author : Michael Scholten
 * Description: turns a led brighter and less bright every 2 seconds
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

int main(void){
	int8_t direction = 1;

	// set PORTD5 (OC1A) as output
	DDRD |= (1 << PORTD5);
	
	// 10 bit fast pwm, set at bottom, clear at compare match OCR1A
	TCCR1A = (1 << COM1A1) | (3 << WGM10);
	TCCR1B = (1 << WGM12) | (1 << CS10);

	// initial brightness value = 0 (not entirely turned off, as I am using fast PWM)
	OCR1A = 0;
	
    while (1){
		// keep the led at the same brightness level for 1 millisecond
		_delay_ms(1);
		
		// if the next step is out of range and the direction value is not improving it, change the direction value
		if((direction < 0 && OCR1A < -direction) || (direction > 0 && OCR1A > 1023 - direction)){
			direction = -direction;
		}
		// add the direction value to the OCR1A to change the brightness value
		OCR1A += direction;
    }
}
