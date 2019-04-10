/* 
 * File:   lcd_display.h
 * Author: shuler18
 *
 * Created on February 15, 2019, 1:51 PM
 */

#ifndef LCD_DISPLAY_H
#define	LCD_DISPLAY_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#include <xc.h>
#include "SPI_comms.h"

#define RST LATB5
//#define _XTAL_FREQ 32000 //32kHz, default low power mode 
#define _XTAL_FREQ 8000000 //8MHz, uncomment for GOL

//function headers
void sendLCDcmd(char cmd);
void sendLCDdata(char data);
void sendLCDstring(char *str);
void fill_lcd(char wb);
void set_xy(char x, char y);
void init_5510(void);
void init_io(void);
char* itoa(int num, char* str);
void read_temp(void);
#endif	/* LCD_DISPLAY_H */

