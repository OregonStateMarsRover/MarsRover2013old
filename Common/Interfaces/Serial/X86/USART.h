/**********************************************************
 *
 * Implements a USART stubs for X86 testing.
 *
 * Written by Taj Morton for OSURC Mars Rover.
 * Copyright (C) 2010-2011 OSURC.
 *
 * Licensed under the LGPL.
 *
 **********************************************************/

#ifndef HAVE_USART_H
#define HAVE_USART_H

#include <stdbool.h>

#include "Common/RingBuffer/RingBuffer.h"

struct USART_Port {
	unsigned char usart_num;
};

typedef struct USART_Port USART_Port;

struct USART_Port usart_ports[6];

struct USART {
	char port_num;
	int baud_rate;
	bool use_rs485;
	
	int fd;

	USART_Port port;
	
	RingBuffer tx_buffer;
	RingBuffer rx_buffer;
	
	void * ref;
	void (*CharacterReceived)(struct USART * port);
};
typedef struct USART USART;

USART * USART_Table[6];

#define USART_BAUD_2400 0
#define USART_BAUD_4800 1
#define USART_BAUD_9600 2
#define USART_BAUD_19200 3
#define USART_BAUD_38400 4
unsigned short USART_BAUD_TABLE[5];

void USART_InitPortStructs();

void USART_Open(USART * serial, unsigned char port, unsigned char baud_rate, unsigned short tx_buf, unsigned short rx_buf, bool use_rs485);

void USART_CalculateBaud(unsigned char * ctrlA, unsigned char * ctrlB, unsigned short baud);

unsigned short USART_Write(USART * serial, unsigned char * buf, unsigned short num_bytes);
void USART_WriteByte(USART * serial, unsigned char dat);
unsigned short USART_Read(USART * serial, char * buf, unsigned short num_bytes);
unsigned char USART_ReadByte(USART * serial);

unsigned short USART_TXFree(USART * serial);
unsigned short USART_TXUsed(USART * serial);

unsigned short USART_RXFree(USART * serial);
unsigned short USART_RXUsed(USART * serial);

void USART_RXIntComplete(unsigned char port);
void USART_TXIntComplete(unsigned char port);

#endif

