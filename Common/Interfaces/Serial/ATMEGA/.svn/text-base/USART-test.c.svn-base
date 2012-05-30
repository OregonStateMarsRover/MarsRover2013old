#include "USART.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>

void led_off(char led) {
	if (led==0) {
		PORTD |= (1<<6);
	}
	else if (led==1) {
		PORTD |= (1<<7);
	}
}

void led_on(char led) {
	if (led==0) {
		PORTD &= ~(1<<6);
	}
	else if (led==1) {
		PORTD &= ~(1<<7);
	}
}

char get_next_led(char led) {
	if (led==0)
		return 1;
	else
		return 0;
}

unsigned char USART_Recv() {
	while ( !(UCSR0A & (1<<RXC0)) ) { //wait until data byte ready to be read
		/*led_on(0);
		led_on(1);*/
	}
	return UDR0;
}

void USART_Transmit(unsigned char dat) {
	while (! (UCSR0A & (1<<UDRE0)) );
	
	UDR0 = dat;
}

void USART_Init(unsigned int ubrr) {
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	
	UCSR0A &= ~(1<<U2X0);
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}

int main_old() {
	unsigned char rx;
	DDRD |= (1<<1) | (1<<2); // TXAT, TXENAT - output
	DDRD &= ~(1<<0); // RXAT - input
	
	PORTD |= (1<<2); // set TXENAT
	//PORTD &= ~(1<<2); // clear TXENAT
	
	DDRD |= (1<<6) | (1<<7); // LEDs - output
	USART_Init(12); // 4800 bps
	
	led_off(0);
	led_off(1);
	
	while(1) {
		USART_Transmit('X');
		_delay_ms(1000);
		USART_Transmit('Y');
		_delay_ms(1000);
		
		/*rx = USART_Recv();
		
		if (rx=='A') {
			led_on(1);
			led_off(0);
		}
		else if (rx=='B') {
			led_on(0);
			led_off(1);
		}
		else {
			led_on(0);
			led_on(1);
		}*/
	}
}

int main() {
	char rx;
	char buf[12];
	USART port0;
	USART_InitPortStructs();
	USART_Open(&port0, 0, USART_BAUD_4800, 128, 128, true);
	
	//DDRD |= (1<<1) | (1<<2); // TXAT, TXENAT - output
	DDRD |= (1<<6) | (1<<7); // LEDs - output
	
	//PORTD |= (1<<2); // set TXENAT
	
	while(1) {
		if (USART_RXUsed(&port0)==5) {
			USART_Read(&port0, &buf, 5);
			
			if (strncmp(buf,"XMEGA",5)==0) {
				USART_Write(&port0,"ATMEGA",6);
			}
		}
		
		/*USART_Write(&port0, "X", 1);
		_delay_ms(1000);
		USART_Write(&port0, "Y", 1);
		_delay_ms(1000);*/
		/*if (USART_RXUsed(&port0)) {
			rx = USART_ReadByte(&port0);	
			
			if (rx=='A') {
				led_on(1);
				led_off(0);
			}
			else if (rx=='B') {
				led_on(0);
				led_off(1);
			}
			else {
				led_on(0);
				led_on(1);
			}
		}*/
	}
}

