/*
 * servoTest.c
 *
 * Created: 13-2-2022 21:47:37
 * Author : micha
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

void servo_setAngle(int32_t angle){
	// we only change the angle if it is within the range of the SG90 servo
	if(angle >= -90 && angle <= 90){
		// calculate the value needed to reach the requested angle and store it in OCR1A
		OCR1A = 3000 + angle * 2000 / 90;
	}
}

void servo_init(){
	// set the servo angle to the 0 degrees
	servo_setAngle(0);
	
	// set the servo pin (PORTD5, OC1A) as output
	DDRD |= (1 << PORTD5);
	
	// 16 bit fast PWM with ICR1 maximum and prescaler /8
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (3 << WGM12) | (1 << CS11);
	
	// set the maximum value to 40000
	ICR1 = 40000;
}

int main(void){
	int16_t angle = -90, direction = 1;
	
	// initialize the servo
	servo_init();
    while (1){
		// if the angle is going out of range, set it back within range and change the direction if needed
		if(angle + direction > 90){
			angle = 90;
			if(direction > 0){
				direction = -direction;
			}
		}else if(angle + direction < -90){
			angle = -90;
			if(direction < 0){
				direction = -direction;
			}
		}
		
		// change the angle
		angle += direction;
		
		// set the servo to the new angle
		servo_setAngle(angle);
		
		// wait for 100 milliseconds (5 timer cycles)
		_delay_ms(100);
    }
}

