/* 
 * File:   SPI_comms.h
 * Author: shuler18
 *
 * Created on February 14, 2019, 5:29 PM
 */

#ifndef SPI_COMMS_H
#define	SPI_COMMS_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

//function headers
void SPI_init_master(void);
void SPI_write_data(unsigned char dat);
//variables
#define CS LATA4 //chip select, used to enable slave. Active low
#define DC LATA5 //0=cmd 1=data

/*
 *Can be calculated for other clk speeds using the following formula:
 *1/SCK(Hz)*8 = time for 8 periods, aka 1 byte of data
 *Note that our measured clk is slightly faster than expected (for Fosc/64 15 works)
 */ 
#define SPI_TX_DELAY 1
#endif	/* SPI_COMMS_H */

