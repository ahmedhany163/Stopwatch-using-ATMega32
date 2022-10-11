 /******************************************************************************
 *
 * File Name: Miniproject2
 *
 * Description: Interfacing with multi seven segment displays and timer module in AVR controllers
 *
 * Author: Ahmed Hany
 *
 *******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char hour_h=0,hour_l=0,min_h=0,min_l=0,sec_h=0,sec_l=0; // Variables to store time

ISR(TIMER1_COMPA_vect){
		/*T(timer)= 1 msec
		OCR1A compares 1000 time which equates to 1 sec
		Then every interrupt is 1 second. */
	sec_l++;
}

ISR(INT0_vect){
	hour_h=0,hour_l=0,min_h=0,min_l=0,sec_h=0,sec_l=0; // Resets all values

}

ISR(INT1_vect){
	TCCR1B = TCCR1B  & ~(1<<CS10) & ~(1<<CS12); // To stop the clock
}

ISR(INT2_vect){
	TCCR1B |= (1<<CS10) | (1<<CS12); // To initiate the clock back
}

void TIMER1_Init(void){
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12); // Selects CTC mode & 1024 Prescaler
	TCNT1 = 0; // Initial comparison value
	OCR1A = 1000; // Final value
	TIMSK = (1<<OCIE1A); // Enables compare mode interrupt
	SREG |= (1<<7); // Enables I-bit
}

void INT0_Init(void){
	DDRD &= ~(1<<2); // PIN 2 of PORT D as input for INT0
	PORTD |= (1<<2); // Activating internal pull up on PIN 2
	MCUCR |= (1<<ISC01); // Activating INT0 on falling edge
	GICR |= (1<<INT0); // Enables INT0
}

void INT1_Init(void){
	DDRD &= ~(1<<3); // PIN 3 of PORT D as input for INT1
	PINB |= (1<<3); // sets PIN 1 to detect rising edge
	MCUCR |= (1<<ISC11) | (1<<ISC10); // Activating INT1 on falling edge
	GICR |= (1<<INT1); // Enables INT1
}

void INT2_Init(void){
	DDRB &= ~(1<<2); // PIN 2 of PORT B as input
	PORTB |= (1<<2); // Activating internal pull up on PIN 2
	GICR |=  (1<<INT2); // Enables INT2
}

int main(void){

	TIMER1_Init(); 	// Calls Timer function
	INT0_Init();	// Calls INT0 function
	INT1_Init();	// Calls INT1 function
	INT2_Init();	// Calls INT2 function


	DDRA |= 0x3F; //First 6 pins on PORT A as output
	DDRC |= 0x0F; //First 4 pints of PORT C as output
	PORTA &= 0xC0; PORTC &= 0xF0; // Clearing all 10 PINS in the beginning


	while(1){

		if (sec_l ==10){		// Function that calculates a minute
			sec_l=0;
			sec_h++;
			if(sec_h==6){
				min_l++;
				sec_h=0;
			}
		}

		else if(min_l == 10){		// Function that calculates an hour
			min_h++;
			min_l=0;
			if(min_h==6){
				min_h=0;
				hour_l++;
			}
		}

		else if(hour_l==10){			// Function that calculates hours
			hour_l=0;
			hour_h++;
		}

		else if(hour_h==2 && hour_l==4){		// Auto reset function when an entire day has passed
			hour_h=0,hour_l=0,min_h=0,min_l=0,sec_h=0,sec_l=0;
		}
		// Display mechanism

		PORTA = 0x01;
		PORTC = (PORTC & 0xF0) | (sec_l & 0x0F);
		_delay_ms(2);
		PORTA = 0x02;
		PORTC = (PORTC & 0xF0) | (sec_h & 0x0F);
		_delay_ms(2);
		PORTA = 0x04;
		PORTC = (PORTC & 0xF0) | (min_l & 0x0F);
		_delay_ms(2);
		PORTA = 0x08;
		PORTC = (PORTC & 0xF0) | (min_h & 0x0F);
		_delay_ms(2);
		PORTA = 0x10;
		PORTC = (PORTC & 0xF0) | (hour_l & 0x0F);
		_delay_ms(2);
		PORTA = 0x20;
		PORTC = (PORTC & 0xF0) | (hour_h & 0x0F);
		_delay_ms(2);

	}

	return 0;

}
