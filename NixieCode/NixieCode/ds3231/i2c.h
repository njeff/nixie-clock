/*
 * i2c.h
 *
 * Created: 12/9/2015 5:28:09 PM
 *  Author: Jeffrey
 */ 
#include "inttypes.h"

#ifndef I2C_H_
#define I2C_H_

void TWIInit(void);
void TWIStart(void);
void TWIStop(void);
void TWIWrite(uint8_t u8data);
uint8_t TWIReadACK(void);
uint8_t TWIReadNACK(void);
uint8_t TWIGetStatus(void);

#endif /* I2C_H_ */