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

#define RST LATB5

//function headers
void sendLCDcmd(char cmd);
void sendLCDdata(char data);
void sendLCDstring(char *str);
void set_pixel(char x, char y, char data);
void fill_lcd(char wb);
void set_xy(char x, char y);
void set_x(char x);
void set_y(char y);
void init_5510(void);

#endif	/* LCD_DISPLAY_H */

