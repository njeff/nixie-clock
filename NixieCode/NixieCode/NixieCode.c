/*
 * NixieCode.c
 *
 * Created: 12/2/2015 8:27:08 PM
 *  Author: Jeffrey
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>
#include <string.h>
#include "millis.h"
#include "inputs.h"
#include "ds3231/ds3231.h"
#include "ds3231/i2c.h"
#include "library-gcc/rtc.h"
#include "library-gcc/twi.h"

enum mode {clock,date,temperature,timer};

void writeOut(uint8_t in[], uint8_t c1, uint8_t c2){ //write buffer to nixie tubes
    uint64_t buffer = 0;
    for(int i = 5; i>=3; i--){ //seconds are on the left of the buffer
        if(in[i]<10){
            if(in[i]==0){
                buffer |= (1ULL << (i*10+11));
                } else {
                buffer |= (1ULL << (i*10+in[i]+1));
            }
        }
    }
    
    for(int i = 2; i>=0; i--){ //hours on the right of the buffer
        if(in[i]<10){
            if(in[i]==0){
                buffer |= (1ULL << ((i+1)*10-1));
                } else {
                buffer |= (1ULL << (i*10+in[i]-1));
            }
        }
    }
    
    if(c1){
        buffer |= 1ULL<<30;
    }
    if(c2){
        buffer |= 1ULL<<62;
    }
    for(int i = 63; i>=0; i--){ //write buffer in seconds first
        uint8_t bit = (buffer >> i) & 0x01;
        if(bit){
            IO_WR |= (1 << DATA);
        } else {
            IO_WR &= ~(1 << DATA);
        }
        
        IO_WR |= 1<<CLK;
        _delay_us(1);
        IO_WR &= ~(1<<CLK);
    }
    
    IO_WR |= 1<<LE; //latch out
    _delay_us(1);
    IO_WR ^= 1<<LE;
}

int main(void)
{
    initInputs(); //initialize
    millis_init();  
    twi_init_master();
    rtc_init();
    struct tm* t = NULL;  

    uint8_t output[6] = {0,0,0,0,0,0}; //output buffer
    
    /*
    t->hour = 18;
    t->min = 38;
    t->sec = 0;
    t->year = 2015;
    t->wday = 4;
    t->mon = 12;
    t->mday = 16;

    rtc_set_time(t);
    */
    
    uint8_t oldMin = 0;
    uint8_t oldSec = 0;
    uint8_t roll[6] = {1,2,3,4,5,6}; //temporary buffer for rolling digits animation
    uint8_t sepa = 0; //digit separators
    uint8_t rollover = 6; //what digit to roll over
    uint8_t doublecycle = 0; //to roll digits through two cycles
    uint8_t buttons = 0;
    uint8_t heldButtons = 0;
    uint8_t left = 0;
    uint8_t right = 0;
    
    uint8_t tOn = 0; //timer on/off
    uint8_t tMin = 0; //timer counts
    uint8_t tSec = 0;
    uint16_t tMil = 0;
    millis_t tStart = 0;
    millis_t tCurrent = 0;
    
    int8_t temp1 = 0; //signed upper half of temperature
    uint8_t temp2 = 0; //unsigned lower half of temperature
    millis_t entryTime = 0;
    enum mode state = clock;
    enum mode oldState = state;
    
    while(1)
    {   
        t = rtc_get_time();
        //update state of buttons
        buttons = getButtons();
        if(buttons & 0x01){
            if((heldButtons & 0x01) == 0){ //if mode button is pressed
                state = (state+1)%4;
            }
        }
        if(buttons & 0x02){
            if((heldButtons & 0x02) == 0){ //right
                right = 1;
            }
        }
        if(buttons & 0x04){
            if((heldButtons & 0x04) == 0){ //left
                left = 1;
            }
        }
        
        heldButtons = buttons; //update state of last buttons held down
        
        //go to correct mode
        if(state == clock) { //time
            output[1] = (t->hour)%10; //get time in HH/MM/SS
            output[0] = (t->hour)/10;
            output[3] = (t->min)%10;
            output[2] = (t->min)/10;
            output[5] = (t->sec)%10;
            output[4] = (t->sec)/10;
            
            //buttons update time
            if(right){
                t->min = (t->min+1)%60;
                rtc_set_time(t);
            }
            if(left){
                t->hour = (t->hour+1)%24;
                rtc_set_time(t);
            }
            
            if(oldSec != output[5]){ //if seconds change
                sepa ^= 1; //toggle the separator lights
            }
                    
            if(oldMin != output[3] && rollover == 0){ //if minutes change
                rollover = 6;
                roll[0] = 1;
                roll[1] = 2;
                roll[2] = 3;
                roll[3] = 4;
                roll[4] = 5;
                roll[5] = 6;
            }
                    
            if(rollover){ //if there are still digits to roll
                if(output[rollover-1]==roll[rollover-1]){ //if digits match
                    doublecycle++;
                    if(doublecycle == 2){
                        rollover--; //move to next digit
                        doublecycle = 0;
                    }
                }
                for(int i = rollover-1; i>=0; i--){ //roll all other digits
                    output[i] = roll[i];
                    roll[i]++;
                    roll[i] = roll[i]%10;
                }
                _delay_ms(1);
            }
            
            oldSec = output[5];
            oldMin = output[3];
        } 
        else if(state == date) { //date
            if(oldState!=state){
                entryTime = millis_get();
            }
            if(millis_get()-entryTime > 10000){ //after 10 seconds return to clock
                state = clock;
            }
            sepa = 1;
            output[1] = (t->mon)%10; //get date in MM/DD/YY
            output[0] = (t->mon)/10;
            output[3] = (t->mday)%10;
            output[2] = (t->mday)/10;
            output[5] = (t->year)%10;
            output[4] = ((t->year)/10)%10;      
        } 
        else if(state == temperature) { //temperature
            if(oldState!=state){
                entryTime = millis_get();
            }
            if(millis_get()-entryTime > 10000){ //after 10 seconds return to clock
                state = clock;
            }
            ds3231_get_temp_int(&temp1,&temp2);
            sepa = 0;
            output[0] = 11; //set blank
            output[1] = 11;
            output[2] = temp1/10;
            output[3] = temp1%10;
            output[4] = temp2/10;
            output[5] = temp2%10;
        } 
        else if(state == timer) {
            if(tOn){ //if the timer is running
                if(left){ //stop timer
                    tOn = 0;
                }
                if(right){
                    
                }
                tCurrent = millis_get() - tStart;
                tMil = tCurrent%1000;
                tSec = (tCurrent/1000)%60;
                tMin = tCurrent/60000;
                                
            } else { //if the timer is not running
                if(left){ //start timer
                    if(tMil == 0 && tSec == 0 && tMin == 0){
                        tStart = millis_get();
                    }
                    tOn = 1;
                }
                if(right){ //reset
                    tMil = 0;
                    tSec = 0;
                    tMin = 0;
                }
            }
            sepa = 1;
            output[0] = tMin/10;
            output[1] = tMin%10;
            output[2] = tSec/10;
            output[3] = tSec%10;
            output[4] = tMil/100;
            output[5] = (tMil%100)/10;
        }
        writeOut(output,sepa,sepa); //update display
        right = 0;
        left = 0;
        oldState = state;
    }
}