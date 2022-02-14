/*
 * analogServoController.c
 *
 * Created: 14-2-2022 11:05:58
 * Author : micha
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

ISR(ADC_vect){}

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

void servo_setAngle(int32_t angle){
	if(angle >= -90 && angle <= 90){
		OCR1A = 3000 + angle * 2000 / 90;
	}
}

void servo_init(){
	servo_setAngle(0);
	DDRD |= (1 << PORTD5);
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (3 << WGM12) | (1 << CS11);
	ICR1 = 40000;
}

int main(void){
	int16_t angle, oldAngle = 0;
	DDRA &= ~(1 << PORTA0);
	servo_init();
    while (1){
		angle = (int32_t)adc_read(0) * 180 / 1024 - 90;
		if(angle != oldAngle){
			servo_setAngle(angle);
			oldAngle = angle;
		}
    }
}
