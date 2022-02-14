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
	int16_t angle = -90, direction = 1;
	servo_init();
    while (1){
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
		angle += direction;
		servo_setAngle(angle);
		_delay_ms(100);
    }
}

