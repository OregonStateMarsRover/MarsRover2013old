#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "USART.h"
#include "Common/CommInterface/CommInterface.h"
#include "ProcessManager/ProcessManager.h"
#include "GPSController/GPSController.h"
#include "TestController/TestController.h"
#include "MotorController/MotorController.h"
#include "TripodController/TripodController.h"
#include "ArmController/ArmController.h"
#include "LEDController.h"

#define AVR_ENTER_CRITICAL_REGION( ) uint8_t volatile saved_sreg = SREG; \
                                     cli();

#define AVR_LEAVE_CRITICAL_REGION( ) SREG = saved_sreg;

void CCPWrite( volatile uint8_t * address, uint8_t value )
{
#if defined __GNUC__
	AVR_ENTER_CRITICAL_REGION( );
	volatile uint8_t * tmpAddr = address;
#ifdef RAMPZ
	RAMPZ = 0;
#endif
	asm volatile(
		"movw r30,  %0"	      "\n\t"
		"ldi  r16,  %2"	      "\n\t"
		"out   %3, r16"	      "\n\t"
		"st     Z,  %1"       "\n\t"
		:
		: "r" (tmpAddr), "r" (value), "M" (CCP_IOREG_gc), "i" (&CCP)
		: "r16", "r30", "r31"
		);

	AVR_LEAVE_CRITICAL_REGION( );
#endif
}

/**
 * ISR to handle external oscillator failure.
 */
ISR(OSC_XOSCF_vect)
{
	cli();
	while(1)
	{
/*		LedOff(LED_STATUS);
		_delay_ms(500);
		LedOn(LED_STATUS);
		_delay_ms(500);

		LedOff(LED_DNT);
		_delay_ms(100);	*/
		LedOff(LED_LBOG);
		_delay_ms(100);
		LedOff(LED_BBOG);
		_delay_ms(100);
		LedOff(LED_RBOG);
		_delay_ms(100);
/*		
		LedOn(LED_DNT);
		_delay_ms(100);
		
		LedOn(LED_LBOG);
		_delay_ms(100);
		LedOn(LED_BBOG);
		_delay_ms(100);
		LedOn(LED_RBOG);
		_delay_ms(100);*/
	}
}

/*
 * ISR handler for non-existant ISRs
 */
ISR(BADISR_vect) {
	cli();
	while(1) {
/*		LedOff(LED_DNT);
		
		LedOff(LED_STATUS);
		_delay_ms(500);
		LedOn(LED_STATUS);
		_delay_ms(500);*/
		
		LedOff(LED_LBOG);
		_delay_ms(100);
		LedOff(LED_BBOG);
		_delay_ms(100);
		LedOff(LED_RBOG);
		_delay_ms(100);
		
		LedOn(LED_LBOG);
		_delay_ms(100);
		LedOn(LED_BBOG);
		_delay_ms(100);
		LedOn(LED_RBOG);
		_delay_ms(100);
	}
}

void ChangeClockFreq() {
	uint8_t clkCtrl;
	LedOff(LED_INT);
	CCPWrite( &OSC.XOSCFAIL, ( OSC_XOSCFDIF_bm | OSC_XOSCFDEN_bm ) );
	char failed = 0;
	
	do
	{
		failed = 0;
		
		OSC.XOSCCTRL = (uint8_t) OSC_FRQRANGE_12TO16_gc | 0b00001011;
		OSC.CTRL |= OSC_XOSCEN_bm;
		while ( (OSC.STATUS & OSC_XOSCRDY_bm) == 0 && failed==0) // wait for clock to stabilize
		{
			if (OSC.XOSCFAIL & OSC_XOSCFDIF_bm)
			{
				OSC.XOSCFAIL |= OSC_XOSCFDIF_bm;
				failed = 1;
			}
		}
	}
	while (failed);
	
	clkCtrl = ( CLK.CTRL & ~CLK_SCLKSEL_gm ) | CLK_SCLKSEL_XOSC_gc;
	CCPWrite( &CLK.CTRL, clkCtrl );
	
	LedOn(LED_INT);
}

int main(void) {
	unsigned int ledTimer = 0;
	Rover rov;
	USART debugPort;
	USART gps_in;
	char data[10];
	int readlen;
	char runTimes=50;
	CommPacket sendPkt;	

	//_delay_ms(500);
	LedInit();
	LedAllOff();
	LedOn(LED_TRI);
	ChangeClockFreq(); // use 16MHz clock
	sei(); // Enables global interrupt
	
	PORTA.DIRSET = PIN0_bm;
	PORTA.DIRSET = PIN1_bm;
	PORTA.DIRSET = PIN2_bm;
	PORTA.DIRSET = PIN3_bm;

	// Use PORTA PIN0 and 4 for debugging
	PORTA.DIRSET = PIN4_bm;

	USART_InitPortStructs();
	InitTimers();
	InitRover(&rov);
	TestControllerInit();
	MotorControllerInit();
	/*TripodControllerInit();
	GPSInitialize();
	ArmControllerInit();

	RegisterModule(&rov, TARGET_GPS_CONTROLLER, &GPSControllerHandleMessage, &GPSControllerTick);
	*/
	RegisterModule(&rov, TARGET_MOTOR_CONTROLLER, &MotorControllerHandleMessage, &MotorControllerTick);
	/*RegisterModule(&rov, TARGET_TEST_CONTROLLER, &TestControllerHandleMessage, &TestControllerTick);
	RegisterModule(&rov, TARGET_TRIPOD_CONTROLLER, &TripodControllerHandleMessage, &TripodControllerTick);
	RegisterModule(&rov, TARGET_ARM_CONTROLLER, &ArmControllerHandleMessage, &ArmControllerTick);
*/	

	RegisterModule(&rov, TARGET_TEST_CONTROLLER, &TestControllerHandleMessage, &TestControllerTick);
	//USART_Open(&debugPort, 5, USART_BAUD_115200, 255, 255, false, false);
	_delay_ms(500);
	LedOff(LED_TRI);
	int i = 0;
	while(1) {
		
		DispatchMessages(&rov); // allow all modules to process received packets
		RunModules(&rov); // run each module's Tick() method


		//USART_Write(&debugPort, "A", 1);
		//_delay_ms(500);

		char dat[10];
		//dat[0]=i;
		dat[0]='X';
		dat[1]='M';
		dat[2]='E';
		dat[3]='G';
		dat[4]='A';
		dat[5]= i+48;
		i++;
		if(i == 10){
			i = 0;
		}

		CommPacket respPkt;
		respPkt.target = TARGET_GUI;
		respPkt.length = 6;
		respPkt.data = dat;
		//SendMessage(&rov,&respPkt);

		ledTimer++;
		if (ledTimer>5000) 
		{
			ledTimer = 0;
			cli();
			LedToggle(LED_FWV);
			LedToggle(LED_BBOG);
			LedToggle(LED_LBOG);
			sei();
		}
	}
	
	CommDeleteInterface(&rov.dataIface);
}

