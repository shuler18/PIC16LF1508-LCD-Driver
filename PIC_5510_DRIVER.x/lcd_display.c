/*
 * File:   lcd_display.c
 * Author: Dylan Shuler
 * Email: dylanshuler11@gmail.com
 *
 * Created on February 7, 2019, 3:39 PM
 */

// PIC16LF1508 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF        // Watchdog Timer Enable (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover Mode (Internal/External Switchover Mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
//#define _XTAL_FREQ 32000 //32kHz, default low power mode 
#define _XTAL_FREQ 8000000 //8MHz, uncomment for GOL
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include "lcd_display.h"
#include "SPI_comms.h"
//#include "nokiaFont.h" //comment this out if it takes too much memory
#include "gol.h" //GOL demo function headers

void main(void) {
    
    //OSCCONbits.IRCF=0x00; //start lf 31khz clk
    OSCCONbits.IRCF=0x0E; //start 8MHZ clock for GOL
    SCS1 = 1; //using internal oscillator block
    RST=1; //make sure reset is high at the start of execution
    
    //io configuration
    init_io();
    
    //init MSSP and LCD
    SPI_init_master();
    init_5510();
    set_xy(0,0);
    fill_lcd(0);
    
    //fun stuff:
    print_board();
    __delay_ms(2000);
    while(1) {
        evolve();
        print_board();
        __delay_ms(2000);
    }
}


// ******************* init and utility functions ******************* //

void init_io()
{
    ANSELBbits.ANSB5=0; //set reset pin to be digital
    TRISBbits.TRISB5 = 0; //set reset pin to be output
    
    ANSELCbits.ANSC2 = 1; //set thermistor pin to be analog input
    TRISCbits.TRISC2 = 1; //set c2 to be input
    
    ADCON0bits.ADON  = 1; //enable ADC
    ADCON1bits.ADCS = 0; //adc clock = Fosc/2
    ADCON0bits.CHS = 0x6; //select input channel AN6
    
    
}

/*
 * Function to send LCD commands to LCD via SPI bus
 */
void sendLCDcmd(char cmd)
{
    DC = 0;
    SPI_write_data(cmd);
}

/*
 *send LCD a single character using font table
 */
void sendLCDdata(char data)
{
    int i;
    DC = 1;
	for(i = 0; i < 5; i++)
    {
        //SPI_write_data(font[data - 0x30][i]);
    }
}

/*
 *function to send init commands to the lcd
 */
void init_5510(void)
{
    //reset LCD on startup. Takes t(ms)>100
    __delay_ms(10); //likely not needed, but ensures the negative edge is seen
    RST = 0;
    __delay_ms(150);
    RST = 1;
    
    //lcd init commands
    sendLCDcmd(0x21); //extended cmd's follow
    sendLCDcmd(0xC0); //set Vop aka contrast, this was the best value found
    sendLCDcmd(0x07); //set tempature coef
    sendLCDcmd(0x13); //set LCD Bias to 1:65 maybe try 1:48?
    sendLCDcmd(0x20); //must send 0x20 to switch back to normal cmd mode
    sendLCDcmd(0x0C); //normal display mode
}


/*
 * function to set the position of both X and Y cursors
 */
void set_xy(char x, char y)
{
    sendLCDcmd(x | 0x80);
    sendLCDcmd(y | 0x40);
}

/*
 * function to set the position of LCD's X cursor
 */
void set_x(char x)
{
    sendLCDcmd(x | 0x80);
}

/*
 * function to set the position LCD's Y cursor
 */
void set_y(char y)
{
    sendLCDcmd(y | 0x40);
}


// ******************* drawing functions ******************* //

/*
 *function to fill lcd screen with a single value
 * 
 * int wb - 0=white ff = black
 */
void fill_lcd(char wb)
{
    int i, j;
    DC = 1; //sending data
    for(i=0; i<6; i++)
    {
        for(j=0; j<84; j++)
        {
            SPI_write_data(wb);
        }
    }
}


/*
 *send LCD a a string using the font table
 */
void sendLCDstring(char *str) {
	while((*str)!=0)
    {   
        sendLCDdata(*str);
        str++;
    }
}

/*
 * function to set cursor to specific row and write data to it
 */
void set_pixel(char x, char y, char data)
{
    set_x(x);
    set_y(y);
    SPI_write_data(data);
}


/********************************** Game Of Life ******************************/

unsigned long board = 0b0011100010000000;
unsigned long copy_board = 0b0011100010000000;
void evolve()
{
	short x, y, count;
	for(x = 0; x < 5; x++)
	{
		for(y = 0; y < 5; y++)
		{
			count = count_neighbors(x,y);
			if(count == 3 && get_cell_value(x,y) == 0)
			{
				copy_board |= (0x01UL<<(5*x+y)); //the conversion to UL is needed for values > 16
			}
			else if((count < 2 || count > 3) && get_cell_value(x,y) == 1)
			{
				copy_board &= ~(0x01UL<<(5*x+y));
			}
		}
	}
	board = copy_board;
}

short count_neighbors(short x, short y)
{
	short count = 0;
	count += get_cell_value(x-1,y-1);
	count += get_cell_value(x-1,y);
	count += get_cell_value(x-1,y+1);
	count += get_cell_value(x,y-1);
	count += get_cell_value(x,y+1);
	count += get_cell_value(x+1,y-1);
	count += get_cell_value(x+1,y);
	count += get_cell_value(x+1,y+1);
	return count;
}

short get_cell_value(short x, short y)
{
	//ensure valid coordinates
	if(x >= 0 && x <= 4 && y >= 0 && y <= 4)
	{
		return ((board & (0x01UL << (x*5+y))) >> (x*5+y));
	}
	else
	{
		return 0;
	}
}



void print_board()
{
    short i,j;
    for(i = 0; i < 5; i++)
    {
        for(j = 0; j < 5; j++)
        {
            draw_cell(i,j, get_cell_value(i,j)); 
        }
    }
    
}

/*
 * Function to draw a cell if it is alive
 * x and y are the indices of a living cell
 * there is a 22 pixel padding added to each side of the board, so the x coor
 * must be converted to account for this
 * 
 * A cell is 8x8 pixels, so the loop draws 8 filled cols
 */
void draw_cell(short x, short y, short val)
{
    short i;
    set_xy(21+(y*8),x); //this is wack, but the x and y values are actually flipped
    DC = 1; //sending data
    for(i = 0; i < 8; i++)
    {
        if(val == 1)
        {
            SPI_write_data(0xFF); //fill column
        }
        else
        {
            SPI_write_data(0); //fill column
        }
    }
}


