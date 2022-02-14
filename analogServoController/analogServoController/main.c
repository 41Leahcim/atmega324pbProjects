/*
 * analogServoController.c
 *
 * Created: 14-2-2022 11:05:58
 * Author : micha
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

// this ISR make sure the ADC interrupt happens
ISR(ADC_vect){}

uint16_t adc_read(uint8_t pin){
	// set the reference voltage to 5V and the channel to the selected pin
	ADMUX = (1 << REFS0) | (pin & 7);
	
	// enable the ADC, start the conversion, reset the ADC flag, enable the ADC interrupt, and set the prescaler to /128 for maximum precision
	ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADIF) | (1 << ADIE) | (7 << ADPS0);
	
	// enable interrupts
	sei();
	
	// enable ADC noise reduction sleep mode and start the sleep mode
	SMCR = (1 << SM0) | (1 << SE);
	
	// wait until the ADC is ready, in case the sleep mode stops unexpectedly or didn't start as soon as expected
	while(ADCSRA & (1 << ADSC));
	
	// disable the sleep mode
	SMCR &= ~(1 << SE);
	
	// disable the ADC
	ADCSRA &= ~(1 << ADEN);
	
	// return the result of the measurement
	return ADC;
}

void servo_setAngle(int32_t angle){
	// only change the angle if it is within range
	if(angle >= -90 && angle <= 90){
		// calculate the needed value to set the servo to the correct angle and store it in OCR1A
		OCR1A = 3000 + angle * 2000 / 90;
	}
}

void servo_init(){
	// set the servo angle to 0 degrees
	servo_setAngle(0);
	
	// set PORTD5 (OC1A) as output for the servo
	DDRD |= (1 << PORTD5);
	
	// 16 bit fast PWM, maximum ICR1, prescaler /8
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (3 << WGM12) | (1 << CS11);
	
	// set the maximum value to 40000
	ICR1 = 40000;
}

int main(void){
	int16_t angle, oldAngle = 0;
	// set PORTA0 as input
	DDRA &= ~(1 << PORTA0);
	
	// initialize the servo
	servo_init();
    while (1){
		// read the adc value and convert it to an angle
		angle = (int32_t)adc_read(0) * 180 / 1024 - 90;
		
		// if the angle is not equal to the old angle, change the angle to the new angle and store the new angle as the old angle
		if(angle != oldAngle){
			servo_setAngle(angle);
			oldAngle = angle;
		}
    }
}
