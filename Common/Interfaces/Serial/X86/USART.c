/**********************************************************
 *
 * Implements a USART stubs for testing on X86.
 *
 * Written by Taj Morton for OSURC Mars Rover.
 * Copyright (C) 2010-2011 OSURC.
 *
 **********************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "USART.h"

void USART_InitPortStructs() {

}

void USART_Open(USART * serial, unsigned char port, unsigned char baud_rate, unsigned short tx_buf, unsigned short rx_buf, bool use_rs485) {
	char portname[10];	
	
	serial->ref = 0;
	serial->CharacterReceived = 0;
	
	serial->port_num = port;
	serial->port = usart_ports[port];
	serial->baud_rate = baud_rate;
	serial->use_rs485 = use_rs485; 
	
	RingBufferInit(&serial->tx_buffer, tx_buf);
	RingBufferInit(&serial->rx_buffer, rx_buf);
	
	USART_Table[port] = serial;
	
	sprintf(portname,"serial.%d",port);
	serial->fd = open(portname, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU | S_IRGRP);
}

void USART_Close(USART * serial) {
	RingBufferDelete(&serial->tx_buffer);
	RingBufferDelete(&serial->rx_buffer);
}

unsigned short USART_Write(USART * serial, unsigned char * buf, unsigned short num_bytes) {
	unsigned short written;
	written = RingBufferAdd(&serial->tx_buffer, buf, num_bytes);
	
	
	
	// STUB: Start transmission
	USART_TXIntComplete(serial->port_num);
	
	return written;
}

unsigned short USART_Read(USART * serial, char * buf, unsigned short max_bytes) {
	return RingBufferGetData(&serial->rx_buffer, buf, max_bytes);
}

unsigned char USART_ReadByte(USART * serial) {
	return RingBufferGetByte(&serial->rx_buffer);
}

unsigned short USART_TXFree(USART * serial) {
	return RingBufferBytesFree(&serial->tx_buffer);
}

unsigned short USART_TXUsed(USART * serial) {
	return RingBufferBytesUsed(&serial->tx_buffer);
}

unsigned short USART_RXFree(USART * serial) {
	return RingBufferBytesFree(&serial->rx_buffer);
}

unsigned short USART_RXUsed(USART * serial) {
	return RingBufferBytesUsed(&serial->rx_buffer);
}

void USART_RXIntComplete(unsigned char port) {
	RingBufferAddByte(&USART_Table[port]->rx_buffer, 0x00);
	
	if (USART_Table[port]->CharacterReceived!=0) {
		USART_Table[port]->CharacterReceived(USART_Table[port]);
	}
}

void USART_TXIntComplete(unsigned char port) {
	if (RingBufferBytesUsed(&USART_Table[port]->tx_buffer)==0) { // TX buffer empty, disable interrupts
	}
	else {
		unsigned char dat = RingBufferGetByte(&USART_Table[port]->tx_buffer);
		write(USART_Table[port]->fd, &dat, 1);
		printf("S[%d]: 0x%x\n",port,dat);
	}
}

void USART_WriteByte(USART * serial, unsigned char dat) {
	USART_Write(serial,&dat,1);
}
/*
ISR(USART_RX_vect)
{
	USART_RXIntComplete(0);
}

ISR(USART_TX_vect) {
	if (USART_Table[0]->use_rs485) {
		*(USART_Table[0]->port.GPIO) &= ~(USART_Table[0]->port.txen_pin_bm);
	}
}

ISR(USART_UDRE_vect)
{
	USART_TXIntComplete(0);
}
*/

