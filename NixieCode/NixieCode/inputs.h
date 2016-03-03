/*
 * inputs.h
 *
 * Created: 12/2/2015 8:32:08 PM
 *  Author: Jeffrey
 */ 


#ifndef INPUTS_H_
#define INPUTS_H_

//on PORTD
#define DATA 0
#define CLK 1
#define LE 2
#define BL 3
#define POL 4

//on PORTD
#define SW1 5 //mode
#define SW2 6 //right
#define SW3 7 //left

//on PORTB
#define SQW 0 //pin change interrupt 0

#define IO_CTL DDRD
#define IO_WR PORTD
#define IO_RD PIND

void initInputs(void);
uint8_t getButtons(void);

#endif /* INPUTS_H_ */