/*
 * ds2312.c
 *
 * Created: 12/9/2015 5:35:02 PM
 *  Author: Jeffrey
 */ 

#include "i2c.h"
#include "ds3231.h"

int getTime(uint8_t in[]){
	TWIStart();
	if (TWIGetStatus() != 0x08)
		return -1;
	TWIWrite((DSADDRESS<<1) | 0);
	if (TWIGetStatus() != 0x18)
		return -1;
	TWIWrite(SECONDS);
	if (TWIGetStatus() != 0x28)
		return -1;
		
	TWIStart(); //repeated start
	if (TWIGetStatus() != 0x10)
		return -1;
	TWIWrite((DSADDRESS<<1) | 1);
	if (TWIGetStatus() != 0x18)
		return -1;
	uint8_t seconds = TWIReadACK();
	if (TWIGetStatus() != 0x50)
		return -1;
	uint8_t minutes = TWIReadACK();
	if (TWIGetStatus() != 0x50)
		return -1;
	uint8_t hours = TWIReadNACK();
	if (TWIGetStatus() != 0x58)
		return -1;
	TWIStop();
	
	in[0] = hours >> 4;
	in[1] = hours & 0x0F;
	in[2] = (minutes >> 4) & 0x07;
	in[3] = minutes & 0x0F;
	in[4] = (seconds >> 4) & 0x07;
	in[5] = seconds & 0x0F;
	
	return 0;
}

int getDate(uint8_t in[]){
	TWIStart();
	if (TWIGetStatus() != 0x08)
		return -1;
	TWIWrite((DSADDRESS<<1) | 0);
	if (TWIGetStatus() != 0x18)
		return -1;
	TWIWrite(DAY);
	if (TWIGetStatus() != 0x28)
		return -1;
	
	TWIStart();
	if (TWIGetStatus() != 0x10)
		return -1;
	TWIWrite((DSADDRESS<<1) | 1);
	if (TWIGetStatus() != 0x18)
		return -1;
	uint8_t day = TWIReadACK();
	if (TWIGetStatus() != 0x50)
		return -1;
	uint8_t month = TWIReadACK();
	if (TWIGetStatus() != 0x50)
		return -1;
	uint8_t year = TWIReadNACK();
	if (TWIGetStatus() != 0x58)
		return -1;
	TWIStop();
	
	in[0] = year >> 4;
	in[1] = year & 0x0F;
	in[2] = (month >> 4) & 0x01;
	in[3] = month & 0x0F;
	in[4] = (day >> 4) & 0x03;
	in[5] = day & 0x0F;
		
	return 0;
}

int getTemp(int *temp){
	TWIStart();
	if (TWIGetStatus() != 0x08)
		return -1;
	TWIWrite((DSADDRESS<<1) | 0);
	if (TWIGetStatus() != 0x18)
		return -1;
	TWIWrite(MSBTEMP);
	if (TWIGetStatus() != 0x28)
		return -1;
	
	TWIStart();
	if (TWIGetStatus() != 0x10)
		return -1;
	TWIWrite((DSADDRESS<<1) | 1);
	if (TWIGetStatus() != 0x18)
		return -1;
	uint8_t inttemp = TWIReadACK();
	if (TWIGetStatus() != 0x50)
		return -1;
	uint8_t fractemp = TWIReadNACK();
	if (TWIGetStatus() != 0x58)
		return -1;
	TWIStop();
	
	*temp = inttemp;
	
	return 0;
}

int setTime(uint8_t in[], int ampm){
	TWIStart();
	if (TWIGetStatus() != 0x08)
		return -1;
	TWIWrite((DSADDRESS<<1) | 0);
	if (TWIGetStatus() != 0x18)
		return -1;
	TWIWrite(SECONDS);
	if (TWIGetStatus() != 0x28)
		return -1;
	
	TWIWrite((in[4]<<4)|(in[5])); //set up seconds
	if (TWIGetStatus() != 0x28)
		return -1;
	
	TWIWrite((in[2]<<4)|(in[3])); //set up minutes
	if (TWIGetStatus() != 0x28)
		return -1;
	
	uint8_t hours = (in[0]<<4)|in[1]|(ampm << 6);
	TWIWrite(hours); //set up hours
	if (TWIGetStatus() != 0x28)
		return -1;
	
	TWIStop();
	return 0;
}

int setDate(int in[]){
	TWIStart();
	if (TWIGetStatus() != 0x08)
		return -1;
	TWIWrite((DSADDRESS<<1) | 0);
	if (TWIGetStatus() != 0x18)
		return -1;
	TWIWrite(SECONDS);
	if (TWIGetStatus() != 0x28)
		return -1;
	
	TWIWrite((in[4]<<4)|(in[5])); //set up day
	if (TWIGetStatus() != 0x28)
		return -1;
	
	TWIWrite((in[2]<<4)|(in[3])); //set up month
	if (TWIGetStatus() != 0x28)
		return -1;
	
	TWIWrite((in[0]<<4)|(in[1])); //set up year
	if (TWIGetStatus() != 0x28)
		return -1;
	
	TWIStop();
	return 0;
}

int setUp(){
	TWIStart();
	if (TWIGetStatus() != 0x08)
		return -1;
	TWIWrite((DSADDRESS<<1) | 0);
	if (TWIGetStatus() != 0x18)
		return -1;
	TWIWrite(CONTROL);
	if (TWIGetStatus() != 0x28)
		return -1;
	TWIWrite(0x00); //disable all alarms and set square wave to 1Hz
	if (TWIGetStatus() != 0x28)
		return -1;
	TWIStop();
	return 0;
}