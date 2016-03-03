/*
 * i2c.c
 *
 * Created: 12/9/2015 5:27:49 PM
 *  Author: Jeffrey
 */ 

#include "avr/io.h"
#include "i2c.h"

void TWIInit(void)
{
	//set SCL to 400kHz with 8 MHz clock freq
	TWSR = 0x00;
	TWBR = 0x04;
	//enable TWI
	TWCR = (1<<TWEN);
}

void TWIStart(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}

//send stop signal
void TWIStop(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

void TWIWrite(uint8_t u8data)
{
	TWDR = u8data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}

uint8_t TWIReadACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

//read byte with NACK
uint8_t TWIReadNACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

uint8_t TWIGetStatus(void)
{
	uint8_t status;
	//mask status, lower bits are prescaler
	status = TWSR & 0xF8;
	return status;
}