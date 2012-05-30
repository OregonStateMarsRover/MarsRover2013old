/**********************************************************
 *
 * Implements a USART on the ATMEGA.
 *
 * Written by Taj Morton for OSURC Mars Rover.
 * Copyright (C) 2010 OSURC.
 *
 * Licensed under the LGPL.
 *
 **********************************************************/

#ifndef HAVE_USART_H
#define HAVE_USART_H

#include <avr/io.h>

#include <stdbool.h>

#include "Common/RingBuffer/RingBuffer.h"

struct USART_Port {
	unsigned char usart_num;
	
	unsigned char tx_pin_bm;
	unsigned char rx_pin_bm;
	unsigned char txen_pin_bm;
	
	volatile uint8_t * GPIO;
	volatile uint8_t * GPIODDR;
	
	volatile uint8_t * DATA;
	volatile uint8_t * UCSRA;
	volatile uint8_t * UCSRB;
	volatile uint8_t * UCSRC;
	
	volatile uint8_t * UBRRL;
	volatile uint8_t * UBRRH;
};

typedef struct USART_Port USART_Port;

struct USART_Port usart_ports[2];

struct USART {
	char port_num;
	int baud_rate;
	bool use_rs485;

	USART_Port port;
	
	RingBuffer tx_buffer;
	RingBuffer rx_buffer;
	
	void * ref;
	void (*CharacterReceived)(struct USART * port);
	void (*CharacterTransmitted)(struct USART * port);
	bool isSerialProtocol;
};
typedef struct USART USART;

USART * USART_Table[2];

#define USART_BAUD_2400 0
#define USART_BAUD_4800 1
#define USART_BAUD_9600 2
#define USART_BAUD_19200 3
#define USART_BAUD_38400 4
unsigned short USART_BAUD_TABLE[5];

void USART_InitPortStructs();

void USART_Open(USART * serial, unsigned char port, unsigned char baud_rate, unsigned short tx_buf, unsigned short rx_buf, bool use_rs485);

void USART_CalculateBaud(unsigned char * ctrlA, unsigned char * ctrlB, unsigned short baud);

void USART_TransmitMode(USART * serial, bool doTx);
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

