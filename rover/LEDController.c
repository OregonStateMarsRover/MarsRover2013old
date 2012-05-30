/*************************************************
 *
 * Controls Main XMEGA LEDs on the Rover Mainboard.
 *
 * Written by Taj Morton.
 * Copyright (C) 2011 OSURC.
 *************************************************/

#include "LEDController.h"

void LedInit() {
	LedGetPort(LED_TRI)->DIRSET |= GetLedMask(LED_TRI);
	LedGetPort(LED_RBOG)->DIRSET |= GetLedMask(LED_RBOG);
	LedGetPort(LED_BBOG)->DIRSET |= GetLedMask(LED_BBOG);
	LedGetPort(LED_LBOG)->DIRSET |= GetLedMask(LED_LBOG);
	LedGetPort(LED_FWV)->DIRSET |= GetLedMask(LED_FWV);
	LedGetPort(LED_ARM)->DIRSET |= GetLedMask(LED_ARM);
	LedGetPort(LED_INT)->DIRSET |= GetLedMask(LED_INT);
	LedGetPort(LED_GPS)->DIRSET |= GetLedMask(LED_GPS);
}

void LedAllOn() {
	LedOn(LED_TRI);
	LedOn(LED_RBOG);
	LedOn(LED_BBOG);
	LedOn(LED_LBOG);
	LedOn(LED_FWV);
	LedOn(LED_ARM);
	LedOn(LED_INT);
	LedOn(LED_GPS);
}

void LedAllOff() {
	LedOff(LED_TRI);
	LedOff(LED_RBOG);
	LedOff(LED_BBOG);
	LedOff(LED_LBOG);
	LedOff(LED_FWV);
	LedOff(LED_ARM);
	LedOff(LED_INT);
	LedOff(LED_GPS);
}

void LedAllToggle() {
	LedToggle(LED_TRI);
	LedToggle(LED_RBOG);
	LedToggle(LED_BBOG);
	LedToggle(LED_LBOG);
	LedToggle(LED_FWV);
	LedToggle(LED_ARM);
	LedToggle(LED_INT);
	LedToggle(LED_GPS);
}

void LedOn(char which) {
	LedGetPort(which)->OUTSET |= GetLedMask(which);
}

void LedOff(char which) {
	LedGetPort(which)->OUTCLR |= GetLedMask(which);
}

void LedToggle(char which) {
	LedGetPort(which)->OUT ^= GetLedMask(which);
}

PORT_t * LedGetPort(char which) {
	switch (which) {
		case LED_FWV:
			return &PORTB;
		case LED_RBOG:
			return &PORTB;
		case LED_BBOG:
			return &PORTB;
		case LED_LBOG:
			return &PORTB;
		case LED_TRI:
			return &PORTB;
		case LED_ARM:
			return &PORTB;
		case LED_INT:
			return &PORTB;
		case LED_GPS:
		default:
			return &PORTB;
	}
}

char GetLedMask(char which) {
	switch (which) {
		case LED_FWV:
			return PIN0_bm;
		case LED_RBOG:
			return PIN1_bm;
		case LED_BBOG:
			return PIN2_bm;
		case LED_LBOG:
			return PIN3_bm;
		case LED_TRI:
			return PIN4_bm;
		case LED_ARM:
			return PIN5_bm;
		case LED_INT:
			return PIN6_bm;
		case LED_GPS:
		default:
			return PIN7_bm;
	}
}

