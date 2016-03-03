/*
 * inputs.c
 *
 * Created: 12/2/2015 8:42:06 PM
 *  Author: Jeffrey
 */ 

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "millis.h"
#include "inputs.h"

static uint8_t button = 0; //current button state
static uint8_t buttonInt = 0; //which buttons have changed
static millis_t lastChange[3] = {0,0,0};

void initInputs(void){
	cli();
	IO_CTL |= ((1<<DATA)|(1<<CLK)|(1<<LE)|(1<<BL)|(1<<POL)); //set to output, the switches are default input (0)
	
	PCICR |= (1<<PCIE2); //enable PORTD pin change interrupt
		
	IO_WR |= ((1<<SW1)|(1<<SW2)|(1<<SW3)); //turn on pull up resistors
	PCMSK2 |= ((1<<SW1)|(1<<SW2)|(1<<SW3)); //setup interrupt mask
	
	PORTB |= 1; //set PB0 pull up
	
	IO_WR |=((1<<POL)|(1<<LE)); //set all to off, transparent latch
	
	IO_WR &= ~(1<<DATA); //write 0
	
	for(int i = 0; i<64; i++){ //clear out display
		IO_WR |= 1<<CLK;
		_delay_us(1);
		IO_WR ^= 1<<CLK;
		_delay_us(1);
	}
	
	IO_WR |= 1<<BL; //disable blanking
	IO_WR &= ~(1<<LE); //release latch
	
	sei();
}

uint8_t getButtons(){ //lsb is mode, msb is left
	uint8_t temp = buttonInt;
	//buttonInt = 0;
	return temp;
}

ISR(PCINT2_vect){ //when there is a change on the status buttons
	static uint8_t buttonOld = 0x07; //all buttons begin in the pulled up state
	
	button = (IO_RD >> 5) & 0x07; //mask out other pins and shift
	uint8_t changed = button ^ buttonOld; //XOR to see which states have changed
	
	millis_t now = millis_get();
	
	for(int i = 0; i<3; i++){
		if(changed & (1<<i)){ //if the state changed
			if(now-lastChange[i] >= 50){ //debounce (don't allow state to change too quickly)
				if((button & (1<<i)) == 0){ //on down press
					buttonInt |= (1<<i);
				} else { //on release
					buttonInt &= ~(1<<i);
				}
				lastChange[i] = now;
			}
		}
	}
	buttonOld = button;
}