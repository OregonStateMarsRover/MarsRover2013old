/**********************************************************
 *
 * Implements a USART on the ATMega.
 *
 * Written by Taj Morton for OSURC Mars Rover.
 * Copyright (C) 2010 OSURC.
 *
 **********************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "USART.h"

#define USART_Format_Set(_usart, _charSize, _parityMode, _twoStopBits)         \
	(*((_usart).UCSRC) = (uint8_t) _charSize | _parityMode |                      \
	                  (_twoStopBits ? USART_SBMODE_bm : 0)
	                  
#define USART_Rx_Enable(_usart) (*((_usart).UCSRB) |= 1<<RXEN0)
#define USART_Rx_Disable(_usart) (*((_usart).UCSRB) &= ~(1<<RXEN0))
#define USART_Tx_Enable(_usart)	(*((_usart).UCSRB) |= 1<<TXEN0)
#define USART_Tx_Disable(_usart) (*((_usart).UCSRB) &= ~(1<<TXEN0))

#define USART_RxdInterrupt_Enable(_usart)                      \
	(*((_usart).UCSRB) |= (1<<RXCIE0))

#define USART_RxdInterrupt_Disable(_usart)                      \
	(*((_usart).UCSRB) &= ~(1<<RXCIE0))

#define USART_TxdInterrupt_Enable(_usart)                      \
	(*((_usart).UCSRB) |= (1<<TXCIE0))

#define USART_TxdInterrupt_Disable(_usart)                      \
	(*((_usart).UCSRB) &= ~(1<<TXCIE0))
	
#define USART_UdeInterrupt_Enable(_usart)                      \
	(*((_usart).UCSRB) |= (1<<UDRIE0))

#define USART_UdeInterrupt_Disable(_usart)                      \
	(*((_usart).UCSRB) &= ~(1<<UDRIE0))

#define USART_IsTXDataRegisterEmpty(_usart) ((*((_usart).UCSRA) & (1<<UDRE0)) != 0)

void USART_InitPortStructs() {
	// These values taken from pg194 of atmega48 datasheet
	USART_BAUD_TABLE[USART_BAUD_2400] = 207;
	USART_BAUD_TABLE[USART_BAUD_4800] = 103;
	USART_BAUD_TABLE[USART_BAUD_9600] = 51;
	USART_BAUD_TABLE[USART_BAUD_19200] = 25;
	USART_BAUD_TABLE[USART_BAUD_38400] = 12;
	
	// USART0
	usart_ports[0].usart_num = 0;
	usart_ports[0].tx_pin_bm = 1<<1;
	usart_ports[0].rx_pin_bm = 1<<0;
	usart_ports[0].txen_pin_bm = 1<<6;
	
	usart_ports[0].GPIO = &PORTD;
	usart_ports[0].GPIODDR = &DDRD;
	
	usart_ports[0].DATA = &UDR0;
	usart_ports[0].UCSRA = &UCSR0A;
	usart_ports[0].UCSRB = &UCSR0B;
	usart_ports[0].UCSRC = &UCSR0C;
	
	usart_ports[0].UBRRL = &UBRR0L;
	usart_ports[0].UBRRH = &UBRR0H;
	
	// USART1
	usart_ports[1].rx_pin_bm = 1<<2;
	usart_ports[1].tx_pin_bm = 1<<3;
	usart_ports[1].txen_pin_bm = 0;
	
	usart_ports[1].GPIO = &PORTD;
	usart_ports[1].GPIODDR = &DDRD;
	
	usart_ports[1].DATA = &UDR1;
	usart_ports[1].UCSRA = &UCSR1A;
	usart_ports[1].UCSRB = &UCSR1B;
	usart_ports[1].UCSRC = &UCSR1C;
	
	usart_ports[1].UBRRL = &UBRR1L;
	usart_ports[1].UBRRH = &UBRR1H;
}

void USART_Open(USART * serial, unsigned char port, unsigned char baud_rate, unsigned short tx_buf, unsigned short rx_buf, bool use_rs485) {
	serial->ref = 0;
	serial->CharacterReceived = 0;
	
	serial->port_num = port;
	serial->port = usart_ports[port];
	serial->baud_rate = baud_rate;
	serial->use_rs485 = use_rs485; 
	
	//if (port==1){
	RingBufferInit(&serial->tx_buffer, tx_buf);
	RingBufferInit(&serial->rx_buffer, rx_buf);
	//}	
	USART_Table[port] = serial;
	
	*(serial->port.GPIODDR) |= serial->port.tx_pin_bm;
	*(serial->port.GPIODDR) &= ~(serial->port.rx_pin_bm);
	
	if (use_rs485) {
		*(serial->port.GPIODDR) |= serial->port.txen_pin_bm;
		*(serial->port.GPIO) &= ~(serial->port.txen_pin_bm); // put RS485 transceiver in receive mode by default
	}
	cli();
	
	*(serial->port.UBRRH) = (USART_BAUD_TABLE[baud_rate] >> 8);
	*(serial->port.UBRRL) = USART_BAUD_TABLE[baud_rate] & 0xFF;
	USART_Tx_Enable(serial->port);
	USART_Rx_Enable(serial->port);
	USART_RxdInterrupt_Enable(serial->port);
	USART_UdeInterrupt_Enable(serial->port);
	if (use_rs485) { // only enable the TXC interrupt if in RS485 mode, since it's only used to clear the TXEN line
		USART_TxdInterrupt_Enable(serial->port);
		//*(serial->port.GPIO) |= serial->port.txen_pin_bm;
	}
	
	sei();
}

unsigned short USART_Write(USART * serial, unsigned char * buf, unsigned short num_bytes) {
	unsigned short written;
	
	USART_UdeInterrupt_Disable(serial->port);
	
	written = RingBufferAdd(&serial->tx_buffer, buf, num_bytes);
	if (serial->use_rs485) { // assert if necessary
		//*(serial->port.GPIO) |= serial->port.txen_pin_bm;
		USART_TransmitMode(serial, true);
	}
	
	USART_UdeInterrupt_Enable(serial->port);

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
	RingBufferAddByte(&USART_Table[port]->rx_buffer, *(USART_Table[port]->port.DATA));
	
	if (USART_Table[port]->CharacterReceived!=0) {
		USART_Table[port]->CharacterReceived(USART_Table[port]);
	}
}

void USART_TXIntComplete(unsigned char port) {
	if (RingBufferBytesUsed(&USART_Table[port]->tx_buffer)==0) { // TX buffer empty, disable interrupts
		USART_UdeInterrupt_Disable(USART_Table[port]->port);
		USART_TransmitMode(USART_Table[port], false);				
	}
	else {
		*(USART_Table[port]->port.DATA) = RingBufferGetByte(&USART_Table[port]->tx_buffer);
	}
}

void USART_TransmitMode(USART * serial, bool doTx) {
	if (serial->use_rs485) {
		if (doTx) {
			USART_Rx_Disable(serial->port);
			*(serial->port.GPIO) |= serial->port.txen_pin_bm;
		}
		else { // going into RX mode
			*(serial->port.GPIO) &= ~(serial->port.txen_pin_bm);
			USART_Rx_Enable(serial->port);
		}
	}
}

void USART_WriteByte(USART * serial, unsigned char dat) {
	USART_Write(serial,&dat,1);
}


ISR(USART0_RX_vect)
{
	USART_RXIntComplete(0);
}

ISR(USART1_RX_vect)
{
	USART_RXIntComplete(1);
}

ISR(USART0_TX_vect) {
	if (USART_Table[0]->use_rs485) {
		//*(USART_Table[0]->port.GPIO) &= ~(USART_Table[0]->port.txen_pin_bm);
		USART_TransmitMode(USART_Table[0], false);
	}
}

ISR(USART1_TX_vect) {
	if (USART_Table[1]->use_rs485) {
		USART_TransmitMode(USART_Table[0], false);
	}
}


ISR(USART0_UDRE_vect)
{
	USART_TXIntComplete(0);
}


ISR(USART1_UDRE_vect)
{
	USART_TXIntComplete(1);
}


