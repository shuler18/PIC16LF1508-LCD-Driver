#include <pic16lf1508.h>
#include <xc.h>
#include "SPI_comms.h"

#define _XTAL_FREQ 31000 //500kHz, default
/*
 * Function to initilze MSSP into SPI master mode
 * Process(from datasheet):
 * SSP1CON1bits.SSPEN needs to be set to configure MSSP in SPI mode
 * SDIx needs to have TRISx bit set
 * SDOx needs to have TRISx cleared
 * SCKx needs to have TRISx cleared (master mode)
 * SS needs to have TRISx bit set
 * 
 * Pins used: 
 * RB4 : SDA/SDI input
 * RC7 : SDO output
 * RB6 : SCL/SCK SPI clk
 * RA3 : SS slave select (set as output to ignore)
 */
void SPI_init_master(void)
{
    //mssp configuration
    SSP1CON1bits.SSPEN = 1; //enable serial port for SPI
    SSP1CON1bits.SSPM = 0x0; //assert dominance and set sck=Fosc/4 
    SSP1CON1bits.CKP = 0; //set idle mode of clk to be low
    SSP1STATbits.CKE = 1; //trasmit data on falling edge, LCD won't work otherwise
    
    //set TRIS bits to configure spi mode
    TRISBbits.TRISB4 = 1; //SDI
    TRISCbits.TRISC7 = 0; //SDO
    TRISBbits.TRISB6 = 0; //SCK
    TRISAbits.TRISA3 = 0; //SS
  
    //configure CS and CD pins as outputs
    CS=1;
    ANSELA = 0; //RA4 can be analog, specify digital
    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA5 = 0; 
    
    //turn off ADC to save power, may be relevant to spi?
    ADCON0 = 0; 
}



void SPI_write_data(unsigned char dat)
{
    CS = 0; //tell LCD it is about to receive data
    SSP1BUF = dat; 
    __delay_ms(SPI_TX_DELAY); //allow full transmission of data before sending again
    CS = 1; //disable the LCD
}
