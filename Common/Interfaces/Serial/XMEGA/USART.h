/**********************************************************
 *
 * Implements a USART on the XMEGA.
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
	USART_t * usart_port;
	PORT_t * gpio_port;
	
	unsigned char tx_pin_bm;
	unsigned char rx_pin_bm;
	unsigned char txen_pin_bm;
};
typedef struct USART_Port USART_Port;

struct USART_Port usart_ports[7];

struct USART {
	char port_num;
	int baud_rate;
	bool use_rs485;

	USART_Port port;
	
	RingBuffer tx_buffer;
	RingBuffer rx_buffer;
	
	void * ref;
	void (*CharacterReceived)(struct USART * port, unsigned char dat);
	void (*CharacterTransmitted)(struct USART * port);
	bool isSerialProtocol;
	char charCtr;
};
typedef struct USART USART;

USART * USART_Table[7];

#define USART_BAUD_2400 0
#define USART_BAUD_4800 1
#define USART_BAUD_9600 2
#define USART_BAUD_19200 3
#define USART_BAUD_38400 4
#define USART_BAUD_115200 5
#define USART_BAUD_76800 6
#define USART_BAUD_250000 7
unsigned short USART_BAUD_TABLE[8];
char USART_BSCALE_TABLE[8];

/**
 * This method should be called once in your program.
 * It initializes a global pointer table and loads data
 * on serial port configuration on the board.
 * Call this in the initialize section of your code.
 * 
 * Do not call it more than once in your program.
 */
void USART_InitPortStructs();

/**
 * Initializes a serial port for communication.
 * serial: Pointer to USART struct for port.
 * port: Port number, as defined by hardware (0-6).
 * baud_rate: Baud rate for connection. This is NOT the raw baud rate, but one of:
 * - USART_BAUD_2400
 * - USART_BAUD_4800
 * - USART_BAUD_9600
 * - USART_BAUD_19200
 * - USART_BAUD_38400
 * tx_buf: Number of bytes to allocate for TX buffer.
 * rx_buf: Number of bytes to allocate for RX buffer.
 * use_rs485: If true, serial port is half-duplex and the RS-485 transceiver hardware will enabled before each transmission. 
 */
void USART_Open(USART * serial, unsigned char port, unsigned char baud_rate, unsigned short tx_buf, unsigned short rx_buf, bool use_rs485, bool isSerialProtocol);

/**
 * Add data to the TX buffer to be transmitted.
 * serial: Serial device to write data to.
 * buf: Pointer to data to write.
 * num_bytes: Number of bytes to write.
 *
 * Returns: Number of bytes queued for transmission (this may be less than num_bytes if the TX queue is full).
 */
unsigned short USART_Write(USART * serial, unsigned char * buf, unsigned short num_bytes);

/**
 * Convenience function that queues a single byte for transmission.
 * serial: Serial device to write to.
 * dat: Byte to write.
 */
void USART_WriteByte(USART * serial, unsigned char dat);

/**
 * Reads up to num_bytes out of the receive buffer and places them in buf.
 * serial: Serial device to read from.
 * buf: Buffer to place received data in.
 * num_bytes: Maximum number of bytes to read. buf should be at least this large.
 * 
 * Returns: Number of bytes read. This will be less than or equal to num_bytes, depending
 * on how many bytes were available.
 *
 */
unsigned short USART_Read(USART * serial, unsigned char * buf, unsigned short num_bytes);

/**
 * Convenience function for reading a single byte.
 * If no bytes are available for reading, 0 is returned,
 * so checking the number of available bytes with USART_RXUsed() is recommended.
 * serial: Serial device to read from.
 * Returns: First byte in RX buffer (or 0 if no data available).
 */
unsigned char USART_ReadByte(USART * serial);

/**
 * Returns: Number of available bytes (free space) in TX buffer.
 */
unsigned short USART_TXFree(USART * serial);

/**
 * Returns: Number of bytes queued for transmission in TX buffer.
 */
unsigned short USART_TXUsed(USART * serial);

/**
 * Returns: Amount of free space (in bytes) in RX buffer.
 */
unsigned short USART_RXFree(USART * serial);
/**
 * Returns: Number of bytes available for reading in RX buffer.
 */
unsigned short USART_RXUsed(USART * serial);

void USART_TransmitMode(USART * serial, bool doTx);

/**
 * Private method for receiving data. Do not call.
 * (Used by interrupts).
 */
void USART_RXIntComplete(unsigned char port);

/**
 * Private method for transmitting queued data. Do not call.
 * (Used by interrupts).
 */
void USART_TXIntComplete(unsigned char port);

#endif

