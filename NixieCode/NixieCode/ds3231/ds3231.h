/*
 * ds3231.h
 *
 * Created: 12/9/2015 5:35:15 PM
 *  Author: Jeffrey
 */ 


#ifndef DS3231_H_
#define DS3231_H_

#define DSADDRESS 0b1101000

#define CONTROL 0x0E

#define SECONDS 0x00
#define MINUTES 0x01
#define HOUR 0x02

#define DAY 0x03
#define MONTH 0x05
#define YEAR 0x06

#define MSBTEMP 0x11
#define LSBTEMP 0x12

int getTime(uint8_t in[]);
int getDate(uint8_t in[]);
int setTime(uint8_t in[], int ampm);
int setDate(int in[]);
int getTemp(int *temp);
int setUp();

#endif /* DS3231_H_ */