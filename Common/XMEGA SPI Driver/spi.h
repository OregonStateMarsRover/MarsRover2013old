/*************************************************
 *
 * ATXMega A3 SPI Driver
 *
 * Written by Cody Hyman.
 * Copyright (C) 2011 OSURC.
 *************************************************/
 
 /*************************************************
 * TODO: 	Define any I/O as Slave Select Line
 *			Implement Read/Write Buffers
 *************************************************/
#ifndef HAVESPI
#define HAVESPI 

#define SPI_C 0
#define SPI_D 1
#define SPI_E 2

#include <avr/io.h>

#include "Common/RingBuffer/RingBuffer.h"

struct SPI {
	SPI_t spi;
	unsigned char port;
	int baud;
	
	RingBuffer tx_buffer;
	RingBuffer rx_buffer;
	
	//Slave select array
}
 
typedef struct SPI SPI;

SPI * SPI_Table[3];

void SPI_InitPortStructs(void);

void SPI_Open(SPI * spi, unsigned short baud);

void SPI_SetBaud(SPI * spi, unsigned short baud);

void SPI_WriteByte(SPI * spi, unsigned char data, unsigned char slave);

unsigned char SPI_ReadByte(SPI * spi);

unsigned short SPI_Read(SPI *spi, unsigned char * buf);

void SPI_Write(SPI * spi, unsigned char * buf, unsigned char slave);

void SPI_SetSlave(SPI * spi);

unsigned short SPI_RXFree(SPI * spi);

unsigned short SPI_TXFree(SPI * spi);

unsigned short SPI_TXFree(SPI * spi);

unsigned short SPI_TXUsed(SPI * spi);

#endif 