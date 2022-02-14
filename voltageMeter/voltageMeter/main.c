/*
 * voltageMeter.c
 *
 * Created: 13-2-2022 14:38:16
 * Author : micha
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>

// this ISR is just to make sure, the ADC interrupt happens and deactivates the sleep mode
ISR(ADC_vect){}

void usart_init(uint32_t baudRate){
	// enable the transmitter, we won't read data so keep the receiver disabled
	UCSR1B = (1 << TXEN);
	
	// set the number of data bits to 8 (wastes 1 bit)
	UCSR1C = (3 << UCSZ0);
	
	// calculate the value for the UBBR1 register, based on the given baud rate
	UBRR1 = F_CPU / (baudRate << 4) - 1;
}

void usart_sendChar(char c){
	// wait until the data register is empty and we can send the next character
	while(~UCSR1A & (1 << UDRE));
	
	// send the given character
	UDR1 = c;
}

void usart_sendString(const char *string){
	// send every character over the usart
	while(*string != '\0') usart_sendChar(*(string++));
}

void usart_sendUint16(uint16_t number){
	// declare a buffer string for the number and an uint8_t for storing every digit of the number in the string
	char buffer[6];
	uint8_t i = 5;
	
	// store a '\0' (terminating null character) at the end of the string
	buffer[5] = '\0';
	
	// calculate and store every digit of the number
	do{
		buffer[--i] = '0' + number % 10;
		number /= 10;
	}while(number > 0);
	
	// calculate the start of the string and send it over the usart
	usart_sendString(buffer + i);
}

uint16_t adc_read(uint8_t pin){
	// use the standard ADC reference voltage and select the channel
	ADMUX = (1 << REFS0) | (pin & 7);
	
	// enable the ADC, start the conversion, reset the interrupt flag, enable the ADC complete interrupt, set the ADC prescaler to /128 for maximum precission
	ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADIF) | (1 << ADIE) | (7 << ADPS0);
	
	// enable interrupts
	sei();
	
	// set the sleep mode to ADC noise reduction
	SMCR = (1 << SM0) | (1 << SE);
	
	// wait until the conversion has been completed (in case the sleep stopped earlier than expected or starts later than expected)
	while(ADCSRA & (1 << ADSC));
	
	// disable sleep mode, to make sure it won't start unexpectedly 
	SMCR &= ~(1 << SE);
	
	// disable the ADC to safe energy
	ADCSRA &= ~(1 << ADEN);
	
	// return the result of the measurement
	return ADC;
}

int main(void){
	uint32_t voltage = 0, lastVoltage = 0;
	uint8_t i, divider;
	
	// initialize the usart
	usart_init(9600);
	
	// set PORTA0 (the pin where we want to read) as input
	DDRA &= ~(1 << PORTA0);
	while (1){
		// read the analog input value at PORTA0 50 times for more precision and stability
		voltage = adc_read(0);
		for(i = 1;i < 50;i++){
			voltage += adc_read(0);
		}
		
		// convert the voltage to an easy to the voltage
		voltage = voltage * 10 / 1024;
		
		// only send the voltage if it isn't equal to the previous measured voltage
		if(voltage != lastVoltage){
			// send the integral value of the voltage
			usart_sendUint16(voltage / 100);
			
			// send the decimal point
			usart_sendChar('.');
			
			// send the two digits after the decimal point of the voltage
			for(divider = 10;divider > 0;divider /= 10){
				usart_sendChar('0' + voltage / divider % 10);
			}
			
			// send a V to tell the user it is a voltage and go back to the start of the line
			usart_sendString(" V\r");
			
			// store the voltage as the last voltage
			lastVoltage = voltage;
		}
    }
}

