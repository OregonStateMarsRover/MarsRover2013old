#include "USART.h"
#include <util/delay.h>
#include <avr/interrupt.h>

void led_on(char led) {
	if (led==1) {
		PORTF.OUTSET = PIN5_bm;
	}
	else if (led==2) {
		PORTF.OUTSET = PIN6_bm;
	}
}

void led_off(char led) {
	if (led==1) {
		PORTF.OUTCLR = PIN5_bm;
	}
	else if (led==2) {
		PORTF.OUTCLR = PIN6_bm;
	}
}

void CharRecv(USART * port) {
	led_on(1);
}

int main() {
	USART port0;
	USART port6;
	USART gps;
	USART_InitPortStructs();
	USART_Open(&port6, 2, USART_BAUD_38400, 255, 1024, false);
	USART_Open(&port0, 0, USART_BAUD_4800, 255, 255, true);
	USART_Open(&gps, 4, USART_BAUD_4800, 255, 255, true);
	PORTE.DIRSET = 1<<1;
	unsigned short i=0;
	char buffer;
	unsigned char read = 0;
	char buf[12];
	char * seq1 = "XMEGA";
	char * seq2 = "ATMEGA";
	
	PORTC.DIRSET = PIN1_bm;
	PORTF.DIRSET = PIN5_bm | PIN6_bm;
	
	led_off(1);
	led_off(2);
	
	port6.CharacterReceived = &CharRecv;
	
	while(1) {
		if (USART_RXUsed(&port6)) {
			read = USART_ReadByte(&port6);
			USART_WriteByte(&port6, read);
		}
		
		/*USART_Write(&port0, seq1, 5);
		_delay_ms(1000);
		if (USART_RXUsed(&port0)==6) {
			USART_Read(&port0, &buf, 6);
			
			if (strncmp(buf,seq2,6)==0) {
				USART_Write(&port6,"PASS\n",5);
			}
			else {
				USART_Write(&port6,"NO RESP\n",8);
			}
		}*/
		
		/*if (USART_RXUsed(&port0)) {
			read = USART_ReadByte(&port0);
			
			if (read=='X') {
				led_on(1);
				led_off(2);
			}
			else if (read=='Y') {
				led_off(1);
				led_on(2);
			}
		}*/
		//PORTC.OUTSET = PIN1_bm;
		/*USART_Write(&port0, seq1, 1);
		_delay_ms(1000);
		USART_Write(&port0, seq2, 1);
		_delay_ms(1000);*/
		
		/*USART_Write(&port6,"a\n",2);
		_delay_ms(1000);*/
		/*while(USART_RXUsed(&port0)<7 && i<20000) {i++;} // wait until valid reply is seen
		if (i>=5000) {
			USART_Write(&port6,"Timeout\n",8);
			//continue;
		}
		
		read = USART_Read(&port0,buf,12);
		USART_Write(&port6, buf, read);
		_delay_ms(1000);*/
		
		/*if (USART_RXUsed(&gps)>=1) {
			buffer=USART_ReadByte(&gps);
			USART_Write(&port6, &buffer, 1);
			//_delay_ms(10);
		}*/
		
		/*if (USART_TXUsed(&port6)>0) {
			i=USART_Read(&port6, buf, 10);
			USART_Write(&port0, buf, i);
		}
		
		if (USART_RXUsed(&port0)>0) {
			i=USART_Read(&port0,buf,10);
			USART_Write(&port6, buf, i);
			
			if (USART_TXFree(&port6)<15) {
				while(USART_TXUsed(&port6)){}
			}
		}*/
		
		/*USART_Write(&port6,"Hello, World!\n",14);
		if (USART_TXFree(&port6)<15) {
			while(USART_TXUsed(&port6)){}
		}*/
		
		/*if (USART_RXUsed(&gps)>=10) {
			i=USART_Read(&gps,buf,10);
			//USART_Write(&port6, &buf[0], 1);
			//USART_Write(&port6, &buf[0], 1);
			//USART_WriteByte(&port6, buf[0]);
			//USART_WriteByte(&port6, buf[0]);
			USART_Write(&port6, buf, i);
			/*buffer='a';
			USART_WriteByte(&port6, buffer);
			buffer='b';
			USART_WriteByte(&port6, buffer);
			buffer='\n';
			USART_WriteByte(&port6, buffer);
			//USART_Write(&port6, buf, 2);
			/*while(USART_RXUsed(&gps)) {
				buffer=USART_ReadByte(&gps);
				USART_WriteByte(&port6, buffer);
			}
		}*/
		
		/*if (USART_RXUsed(&port0)) {
			buffer = USART_ReadByte(&port0);
			USART_Write(&port6, &buffer, 1);
		}*/
	}
		/*if (i%0x200) {
			_delay_ms(1000);
		}*/
		/*cli();
		USART_WriteByte(&port6, 'H');
		//_delay_us(50);
		USART_WriteByte(&port6, '\n');
		//_delay_ms(2);*/
	//}
}

