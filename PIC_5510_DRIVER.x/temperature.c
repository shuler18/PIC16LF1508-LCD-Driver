/*
 * File:   temperature.c
 * Author: shule
 *
 * Created on April 10, 2019, 3:28 PM
 */


#include "lcd_display.h"
//#include <math.h> //uncomment if memory is available
void read_temp()
{
    short res = 0;
    float resistance = 0;
    float temp = 0;
    ADCON1bits.ADCS = 0; //adc clock = Fosc/2
    ADCON0bits.ADON  = 1; //turn on ADC
    ADCON0bits.CHS = 0x6; //select input channel AN6
    __delay_ms(1); //wait enough time for sample to be acquired
    GO_nDONE = 1; //start conversion process
    while(GO_nDONE); //wait for conversion to be done
    res =  (ADRESH << 2 ) + (ADRESL >> 6);
    resistance = 1023.0 / res - 1;
    resistance = 10000.0 / resistance;
    
    temp = resistance / 10000;
    //temp = log(temp); //UNCOMMENT TO USE TEMP FUNCTION
    temp /= 4000.0; //b coefficient guess
    temp += 1.0 / (25 + 273.15);
    temp  = 1.0 / temp;
    temp -= 282.15;
    char buf[3];
    itoa((int)temp, buf);
    sendLCDstring(buf);
}

// Implementation of itoa(), expects 3 char buffer 
char* itoa(int num, char* str) 
{ 
    short i = 1;
  
    // Process individual digits 
    while (num != 0) 
    { 
        int rem = num % 10; 
        str[i] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/10; 
        i = i-1;
    }
  
    str[2] = '\0'; // Append string terminator 
    //technically need to reverse this
  
    return str; 
} 