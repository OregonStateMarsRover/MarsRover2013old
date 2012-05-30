#include <stdio.h>
#include <string.h>

#include "MotorController.h"
#include "../ProcessManager/ProcessManager.h"

void MotorControllerInit() {
	char i;
	USART_Open(&motorController.rbogPort, 2, MOTOR_CONTROLLER_BAUD, 10, 10, true, true);
	USART_Open(&motorController.lbogPort, 4, MOTOR_CONTROLLER_BAUD, 10, 10, true, true);
	USART_Open(&motorController.bbogPort, 3, MOTOR_CONTROLLER_BAUD, 10, 10, true, true);
	
	CommInterfaceInit(&motorController.rbog, &motorController.rbogPort);
	CommInterfaceInit(&motorController.lbog, &motorController.lbogPort);
	CommInterfaceInit(&motorController.bbog, &motorController.bbogPort);
	
	motorController.currentSpeed = 0;
	motorController.currentHeading = 0;
	
	motorController.strafeX = 0;
	motorController.strafeY = 0;
	motorController.strafeRot = 0;
	motorController.inStrafeMode = 0;
	
	motorController.lbogBus.inf = &motorController.lbog;
	motorController.rbogBus.inf = &motorController.rbog;
	motorController.bbogBus.inf = &motorController.bbog;
	
	motorController.lbogBus.transmitState = motorController.rbogBus.transmitState = motorController.bbogBus.transmitState = MOTOR_STATE_NOTHING;
	motorController.lbogBus.restartTransmitIndex = motorController.rbogBus.restartTransmitIndex = motorController.bbogBus.restartTransmitIndex = -1;
	
	motorController.lbogBus.motorIds[0]=1;
	motorController.lbogBus.motorIds[1]=2;
	motorController.lbogBus.motorIds[2]=0x40;
	
	motorController.bbogBus.motorIds[0]=3;
	motorController.bbogBus.motorIds[1]=4;
	motorController.bbogBus.motorIds[2]=0x41;
	motorController.bbogBus.motorIds[3]=0x42;
	
	motorController.rbogBus.motorIds[0]=5;
	motorController.rbogBus.motorIds[1]=6;
	motorController.rbogBus.motorIds[2]=0x43;
	
	memset(motorController.rbogBus.motorSpeeds, 0, sizeof(short)*4);
	memset(motorController.rbogBus.newMotorSpeeds, 0, sizeof(short)*4);
	memset(motorController.rbogBus.targetMotorSpeeds, 0, sizeof(short)*4);
	memset(motorController.rbogBus.motorUpdateTimeouts, 0, sizeof(char)*4);
	
	memset(motorController.lbogBus.motorSpeeds, 0, sizeof(short)*4);
	memset(motorController.lbogBus.newMotorSpeeds, 0, sizeof(short)*4);
	memset(motorController.lbogBus.targetMotorSpeeds, 0, sizeof(short)*4);
	memset(motorController.lbogBus.motorUpdateTimeouts, 0, sizeof(char)*4);
	
	memset(motorController.bbogBus.motorSpeeds, 0, sizeof(short)*4);
	memset(motorController.bbogBus.newMotorSpeeds, 0, sizeof(short)*4);
	memset(motorController.bbogBus.targetMotorSpeeds, 0, sizeof(short)*4);
	memset(motorController.bbogBus.motorUpdateTimeouts, 0, sizeof(char)*4);
	
	// make all actuators start out straight
	motorController.rbogBus.targetMotorSpeeds[2] = 1500;
	motorController.lbogBus.targetMotorSpeeds[2] = 1500;
	motorController.bbogBus.targetMotorSpeeds[2] = 1500;
	motorController.bbogBus.targetMotorSpeeds[3] = 1500;
	
	motorController.rbogBus.doDisableMosfets=false;
	motorController.bbogBus.doDisableMosfets=false;
	motorController.lbogBus.doDisableMosfets=false;
	
	motorController.doPwmMode=false;
	
	motorController.wantTempReport = motorController.wantCurrentReport = false;
	motorController.tempReportReady = motorController.currentReportReady = false;
	
	MotorControllerInitBus(&motorController.lbogBus);
	MotorControllerInitBus(&motorController.rbogBus);
	MotorControllerInitBus(&motorController.bbogBus);
	
	StartTimer(&motorController.watchdogTimer);
	StartTimer(&motorController.statusTimer);
}

void MotorControllerInitBus(MotorBus * bus) {
	bus->transmitState = MOTOR_STATE_NOTHING;
	bus->restartTransmitIndex = -1;
	
	memset(bus->motorSpeeds, 0, sizeof(short)*4);
	memset(bus->newMotorSpeeds, 0, sizeof(short)*4);
	memset(bus->targetMotorSpeeds, 0, sizeof(short)*4);
	memset(bus->motorUpdateTimeouts, 0, sizeof(char)*4);
	
	bus->doDisableMosfets = false;
	
	memset(bus->motorCommand, 0, sizeof(char)*BUS_MAX_MOTORS);
	memset(bus->newMotorCommand, 0, sizeof(char)*BUS_MAX_MOTORS);
	memset(bus->targetMotorCommand, 0, sizeof(char)*BUS_MAX_MOTORS);
	
	memset(bus->motorArguments, 0, sizeof(char)*BUS_MAX_ARGLENGTH*BUS_MAX_MOTORS);
	memset(bus->targetMotorArguments, 0, sizeof(char)*BUS_MAX_ARGLENGTH*BUS_MAX_MOTORS);
	memset(bus->newMotorArguments, 0, sizeof(char)*BUS_MAX_ARGLENGTH*BUS_MAX_MOTORS);
	
	memset(bus->motorArgumentLength, 0, sizeof(char)*BUS_MAX_MOTORS);
	memset(bus->newMotorArgumentLength, 0, sizeof(char)*BUS_MAX_MOTORS);
	memset(bus->targetMotorArgumentLength, 0, sizeof(char)*BUS_MAX_MOTORS);
}

void MotorControllerHandleMessage(Rover * rov, CommPacket * pkt) {
	char command = pkt->data[0];
	short speed;
	short heading;
	short val;
	short speeds[6];
	char i;
	
	switch (command) {
		case MOTOR_CONTROLLER_ATOGGLE:
			MotorControllerToggleActuators();
		break;
		
		case MOTOR_CONTROLLER_SET_SPEED:
			speed = MotorControllerBufToShort(&pkt->data[1]);
			MotorControllerSetSpeed(speed);
		break;
		
		case MOTOR_CONTROLLER_SET_HEADING:
			heading = MotorControllerBufToShort(&pkt->data[1]);
			MotorControllerSetHeading(heading);
		break;
		
		case MOTOR_CONTROLLER_SET_STRAFE:
		break;
		
		case MOTOR_CONTROLLER_ESTOP:
			MotorControllerEStop();
			break;
		
		case MOTOR_CONTROLLER_STATUS:
			MotorControllerSendStatus();
		break;
		
		case MOTOR_CONTROLLER_SET_MOTORS:
			for (i=0;i<6;i++) {
				speeds[i]=MotorControllerBufToShort(&pkt->data[1+(i*2)]);
			}
			MotorControllerUpdateMotors(speeds,0,5);
		break;
		
		case MOTOR_CONTROLLER_PASS_THRU:
			MotorControllerPassThru(pkt, rov);
			break;
		
		case MOTOR_CONTROLLER_DISABLE_MOSFETS:
			MotorControllerDisableMosfets(pkt->data[1]);
			break;
		
		case MOTOR_CONTROLLER_PWM_MODE:
			MotorControllerPWMMode(pkt->data[1]);
			break;
		
		case MOTOR_CONTROLLER_RESET_WATCHDOG: // if we get a watchdog reset, restart the timer
			StartTimer(&motorController.watchdogTimer);
			break;
		
		case MOTOR_CONTROLLER_SET_STALL_CURRENT:
			val = MotorControllerBufToShort(&pkt->data[1]);
			MotorControllerSetStallCurrent(val);
			break;
		
		case MOTOR_CONTROLLER_GET_TEMP:
			MotorControllerGetTemperatures();
			motorController.tempReportReady = false;
			motorController.wantTempReport = true;
			StartTimer(&motorController.tempSendTimer);
			break;
		
		case MOTOR_CONTROLLER_GET_CURRENT:
			MotorControllerGetCurrent();
			motorController.wantCurrentReport = true;
			motorController.currentReportReady = false;
			StartTimer(&motorController.currentSendTimer);
			break;
		
		case MOTOR_CONTROLLER_DISABLE_CORNER_MOSFETS:
			MotorControllerDisableCornerMosfets(pkt->data[1]);
			break;
		
		case MOTOR_CONTROLLER_SET_SPEED_HEADING:
			speed = MotorControllerBufToShort(&pkt->data[1]);
			heading = MotorControllerBufToShort(&pkt->data[3]);
			MotorControllerSetSpeedHeading(speed, heading);
			break;
		
		default:
			return;
	}
	
	StartTimer(&motorController.watchdogTimer); // reset timer since we got this last packet
}

void MotorControllerPassThru(CommPacket * pkt, Rover * rov) {
	CommPacket mtrPkt;
	char data[20];
	
	mtrPkt.target = pkt->data[1];
	mtrPkt.data = data;
	mtrPkt.length = pkt->length-2;
	
	memcpy(data, pkt->data+2, pkt->length-2);
	CommSendPacket(MotorControllerInfForDevice(mtrPkt.target), &mtrPkt);
	//CommSendPacket(&motorController.rbog, &mtrPkt);
	
	//void MotorControllerSendCommand(char device, char cmd, char length, char * dat)
	//MotorControllerSendCommand(pkt->data[1], pkt->data[2], pkt->length-3, &(pkt->data[3]));
	
	mtrPkt.target = TARGET_GUI_MOTOR;
	mtrPkt.data[0]=0xFC;
	mtrPkt.length=1;
	SendMessage(rov, &mtrPkt);
}

void RunMotorBus(MotorBus * s, Rover * rov) {
	CommPacket cmdPacket;
	CommPacket commPkt;
	CommInterface * iface;
	
	char ret;
	char i;
	char stopMotor, startMotor;
	char expectedReturnCode, advanceMotor;
	stopMotor = startMotor = -1; // don't do any updates by default
	
	unsigned char data[12];
	unsigned char readdata[20];
	
	commPkt.data = readdata;
	
	advanceMotor = 0;
	
	switch (s->transmitState) {
		case MOTOR_STATE_NOTHING: // waiting for something to change
			// 0. Check to see if there are any messages (reply to pass-through)
			iface = MotorControllerInfForDevice(s->motorIds[0]);
			if (CommRXPacketsAvailable(iface)) { // message waiting on bus
				ret = CommGetPacket(iface, &commPkt, 20);
				
				commPkt.target = TARGET_GUI_MOTOR;
				if (!ret) {
					commPkt.length = 1;
					commPkt.data[0]=0xFF; // failed to read back
				}
				// else, just send back as-is
				
				SendMessage(rov, &commPkt);
			}
			
			//break;
			// A. Check if any command or argument in s->new* has changed
			// B. Determine what indicies have changed (make sure to update all in one swoop,
			//    otherwise one motor that times out will block and keep others from updating.
			
			for (i=0;i<=3;i++) {
				if (s->newMotorCommand[i]!=s->motorCommand[i] || memcmp(s->newMotorArguments[i],s->motorArguments[i], s->newMotorArgumentLength[i])!=0) { // motor command or argument has changed
					if (startMotor==-1)
						startMotor=i;
					
					if (i>stopMotor)
						stopMotor=i;
				}
			}
			
			if (startMotor != -1 && stopMotor!=-1 && startMotor < BUS_MAX_MOTORS && stopMotor < BUS_MAX_MOTORS) {
				// these had better both update and !=-1 - if they're a retarded number, let's just skip it (which they should never be)
				
				s->updateMotorIndex = startMotor;
				s->updateMotorStop = stopMotor;
				s->transmitState = MOTOR_STATE_TRANSMIT;

				for (i=startMotor; i<=stopMotor; i++) {
					s->targetMotorCommand[i] = s->newMotorCommand[i];
					memcpy(s->targetMotorArguments[i], s->newMotorArguments[i], s->newMotorArgumentLength[i]);
					s->targetMotorArgumentLength[i] = s->newMotorArgumentLength[i];
				}
			}
		break;
		
		case MOTOR_STATE_TRANSMIT: // need to transmit to next motor on bus
			// Send message out to motor controller, switch state to expecting reply, restart timer
			cmdPacket.target = UPDATE_MOTOR_ID(s);
			
			cmdPacket.data=data;
			cmdPacket.target = UPDATE_MOTOR_ID(s);
			cmdPacket.length = s->targetMotorArgumentLength[s->updateMotorIndex]+1;
			
			cmdPacket.data[0]=s->targetMotorCommand[s->updateMotorIndex];
			memcpy(cmdPacket.data+1, s->targetMotorArguments[s->updateMotorIndex], cmdPacket.length-1);
			
			/*cmdPacket.data[0]=0x20;
			cmdPacket.length=1;*/
			
			CommSendPacket(MotorControllerInfForDevice(UPDATE_MOTOR_ID(s)), &cmdPacket);
			
			StartTimer(&s->replyTimer);
			s->transmitState=MOTOR_STATE_WAITING_REPLY;
			
		break;
		
		case MOTOR_STATE_WAITING_REPLY:
			// A. Check if we have received a reply. Check it's the command we're expecting
			//    If so, update status array and increment updateMotorIndex (and switch to NOTHING state if necessary)
			// B. Check if we have timed out yet.
			//    If so, update status array and increment updateMotorIndex (and switch to NOTHING state if necessary)
			
			iface = MotorControllerInfForDevice(UPDATE_MOTOR_ID(s));
			if (CommRXPacketsAvailable(iface)) { // message waiting on bus
				ret = CommGetPacket(iface, &commPkt, 20);
				if (!ret) // failed to get packet, try again later
					break;
				
				expectedReturnCode = s->targetMotorCommand[s->updateMotorIndex];
				
				if (commPkt.data[0]==expectedReturnCode) { // valid response, update status
					s->motorCommand[s->updateMotorIndex] = s->targetMotorCommand[s->updateMotorIndex];
					memcpy(s->motorArguments[s->updateMotorIndex], s->targetMotorArguments[s->updateMotorIndex], s->targetMotorArgumentLength[s->updateMotorIndex]);
					s->motorArgumentLength[s->updateMotorIndex] = s->targetMotorArgumentLength[s->updateMotorIndex];
					
					if (expectedReturnCode==MOTOR_CONTROL_DEV_GETCURRENT) {
						s->motorCurrent[s->updateMotorIndex] = MotorControllerBufToShort(&commPkt.data[1]); //expectedReturnCode;
					}
					else if (expectedReturnCode==MOTOR_CONTROL_DEV_GETTEMP) {
						s->motorTemperature[s->updateMotorIndex] = MotorControllerBufToShort(&commPkt.data[1]); //expectedReturnCode;
					}
					else {
						s->motorStatus[s->updateMotorIndex] = expectedReturnCode;
					}
					
					advanceMotor = 1;
				}
				// else, got a garbage reply, just wait around until we get either timeout or get the one we're looking for
			}
			else { // check if we have a timeout yet
				if (GetSpanUs(&s->replyTimer)>FROM_uS(200000)) { // timeout
					s->motorStatus[s->updateMotorIndex] = 0; // no reply from this motor
					advanceMotor = 1;
					s->motorUpdateTimeouts[s->updateMotorIndex]++;
				}
			}
			
			if (advanceMotor) {
				s->updateMotorIndex++;
		
				if (s->updateMotorIndex > s->updateMotorStop) { // done sending updates
					s->transmitState = MOTOR_STATE_NOTHING;
				}
				else {
					s->transmitState = MOTOR_STATE_TRANSMIT; // send next update
				}
			}
		break;
		
	}
}

void MotorControllerTick(Rover * rov) {
	CommPacket cmdPacket;
	CommPacket commPkt;
	CommInterface * iface;
	unsigned char data[12];
	unsigned char readdata[20];
	char ret, i;
	
	PORTA.OUTSET = PIN3_bm;
	commPkt.data=readdata;
	
	RunMotorBus(&motorController.lbogBus, rov);
	RunMotorBus(&motorController.rbogBus, rov);
	RunMotorBus(&motorController.bbogBus, rov);
	
	if (GetSpanUs(&motorController.watchdogTimer)>FROM_uS(3000000)) { // timeout every 3 seconds
		MotorControllerEStop(); // timeout, stop mr rover! :(
		StartTimer(&motorController.watchdogTimer);
		
		cmdPacket.target = TARGET_GUI_MOTOR;
		cmdPacket.length = 1;
		cmdPacket.data = data;
		data[0] = 0xFE;
		SendMessage(rov, &cmdPacket); // send a watchdog timeout message
	}
	
	/*if (GetSpanUs(&motorController.statusTimer)>FROM_uS(1000000) && !TransmissionInProgress(rov)) { // time to send a status update to the GUI
		data[1]=motorController.lbogBus.motorStatus[0];
		data[2]=motorController.lbogBus.motorStatus[1];
		
		data[3]=motorController.bbogBus.motorStatus[0];
		data[4]=motorController.bbogBus.motorStatus[1];
		
		data[5]=motorController.rbogBus.motorStatus[0];
		data[6]=motorController.rbogBus.motorStatus[1];
		
		data[7]=motorController.lbogBus.motorStatus[2];
		data[8]=motorController.bbogBus.motorStatus[2];
		data[9]=motorController.bbogBus.motorStatus[3];
		data[10]=motorController.rbogBus.motorStatus[2];
		
		StartTimer(&motorController.statusTimer);
		cmdPacket.target = TARGET_GUI_MOTOR;
		cmdPacket.length = 1+10+1; // start byte, motor status, and actuator status
		cmdPacket.data = data;
		data[0] = 0xFD;
		data[11] = motorController.inStrafeMode;
		
		SendMessage(rov, &cmdPacket);
	}*/
	
	if (GetSpanUs(&motorController.statusTimer)>FROM_uS(1000000) && !TransmissionInProgress(rov)) { // time to send a status update to the GUI
		data[1]=motorController.lbogBus.motorUpdateTimeouts[0];
		data[2]=motorController.lbogBus.motorUpdateTimeouts[1];
		
		data[3]=motorController.bbogBus.motorUpdateTimeouts[0];
		data[4]=motorController.bbogBus.motorUpdateTimeouts[1];
		
		data[5]=motorController.rbogBus.motorUpdateTimeouts[0];
		data[6]=motorController.rbogBus.motorUpdateTimeouts[1];
		
		data[7]=motorController.lbogBus.motorUpdateTimeouts[2];
		data[8]=motorController.bbogBus.motorUpdateTimeouts[2];
		data[9]=motorController.bbogBus.motorUpdateTimeouts[3];
		data[10]=motorController.rbogBus.motorUpdateTimeouts[2];
		
		StartTimer(&motorController.statusTimer);
		cmdPacket.target = TARGET_GUI_MOTOR;
		cmdPacket.length = 11;
		cmdPacket.data = data;
		data[0] = 0xFB;
		
		SendMessage(rov, &cmdPacket);
	}
	
	/*if (motorController.wantTempReport && GetSpanUs(&motorController.tempSendTimer)>FROM_uS(MOTOR_CONTROL_GETDATA_TIMEOUT)) {
		motorController.tempReportReady = true;
		motorController.wantTempReport = false;
	}
	
	if (motorController.wantCurrentReport && GetSpanUs(&motorController.currentSendTimer)>FROM_uS(MOTOR_CONTROL_GETDATA_TIMEOUT)) {
		motorController.currentReportReady = true;
		motorController.wantCurrentReport = false;
	}
	
	if (!TransmissionInProgress(rov) && motorController.tempReportReady) { // have a temp report ready to send
		cmdPacket.target = TARGET_GUI_MOTOR;
		cmdPacket.length = 13;
		cmdPacket.data = data;
		
		data[0] = 0xFA;
		for (i=0;i<6;i++) {
			data[1+2*i] = (MotorControllerBusForId(i+1)->motorTemperature[MotorControllerBusPosForId(i+1)]>>8) & 0xFF;
			data[2+2*i] = MotorControllerBusForId(i+1)->motorTemperature[MotorControllerBusPosForId(i+1)] & 0xFF;
		}
	}
	
	if (!TransmissionInProgress(rov) && motorController.currentReportReady) { // have a current report ready to send
		cmdPacket.target = TARGET_GUI_MOTOR;
		cmdPacket.length = 13;
		cmdPacket.data = data;
		
		data[0] = 0xF9;
		for (i=0;i<6;i++) {
			data[1+2*i] = (MotorControllerBusForId(i+1)->motorCurrent[MotorControllerBusPosForId(i+1)]>>8) & 0xFF;
			data[2+2*i] = MotorControllerBusForId(i+1)->motorCurrent[MotorControllerBusPosForId(i+1)] & 0xFF;
		}
		
		motorController.wantCurrentReport = false;
		motorController.currentReportReady = false;
		SendMessage(rov, &cmdPacket);
	}*/
	
	PORTA.OUTCLR = PIN3_bm;
}

void MotorControllerToggleActuators() {
	motorController.inStrafeMode = !motorController.inStrafeMode;
	
	MotorControllerUpdateActuators();
}

void MotorControllerPWMMode(char enablePWM) {
	motorController.doPwmMode=enablePWM;
}

void MotorControllerDisableMosfets(char disableMosfets) {
	char i;
	if (disableMosfets) {
		for (i=1;i<7;i++) {
			MotorControllerSendCommand(i, MOTOR_CONTROL_DEV_DISABLE_MOSFETS, 0, 0);
		}
	}
}

void MotorControllerSetStallCurrent(short stallCurrent) {
	char i;
	for (i=1;i<7;i++) {
		MotorControllerSendShort(i, MOTOR_CONTROL_DEV_SETMAXCURRENT, stallCurrent);
	}
}

void MotorControllerGetCurrent() {
	char i;
	for (i=1;i<7;i++) {
		MotorControllerSendCommand(i, MOTOR_CONTROL_DEV_GETCURRENT, 0, 0);
	}
	
	for (i=0x40;i<=0x43;i++) {
		MotorControllerSendCommand(i, MOTOR_CONTROL_DEV_GETCURRENT, 0, 0);
	}
}

void MotorControllerGetTemperatures() {
	char i;
	for (i=1;i<7;i++) {
		MotorControllerSendCommand(i, MOTOR_CONTROL_DEV_GETTEMP, 0, 0);
	}
	
	for (i=0x40;i<=0x43;i++) {
		MotorControllerSendCommand(i, MOTOR_CONTROL_DEV_GETTEMP, 0, 0);
	}
}

void MotorControllerDisableMosfetsOld(char disableMosfets) {
	motorController.lbogBus.doDisableMosfets=disableMosfets;
	motorController.rbogBus.doDisableMosfets=disableMosfets;
	motorController.bbogBus.doDisableMosfets=disableMosfets;
	
	if (disableMosfets) { // if we need to send the command to disable stuff
		motorController.lbogBus.newMotorSpeeds[0]=motorController.lbogBus.newMotorSpeeds[0]+1; // change all these by just a bit to force an update
		motorController.lbogBus.newMotorSpeeds[1]=motorController.lbogBus.newMotorSpeeds[1]+1;
		//motorController.lbogBus.newMotorSpeeds[2]=motorController.lbogBus.newMotorSpeeds[2]+1;
	
		motorController.rbogBus.newMotorSpeeds[0]=motorController.rbogBus.newMotorSpeeds[0]+1;
		motorController.rbogBus.newMotorSpeeds[1]=motorController.rbogBus.newMotorSpeeds[1]+1;
		//motorController.rbogBus.newMotorSpeeds[2]=motorController.rbogBus.newMotorSpeeds[2]+1;
	
		motorController.bbogBus.newMotorSpeeds[0]=motorController.bbogBus.newMotorSpeeds[0]+1;
		motorController.bbogBus.newMotorSpeeds[1]=motorController.bbogBus.newMotorSpeeds[1]+1;
		//motorController.bbogBus.newMotorSpeeds[2]=motorController.bbogBus.newMotorSpeeds[2]+1;
		//motorController.bbogBus.newMotorSpeeds[3]=motorController.bbogBus.newMotorSpeeds[3]+1;
	}
}

void MotorControllerSetSpeed(short speed) {
	motorController.currentSpeed = speed;
	
	if (!motorController.inStrafeMode)
		MotorControllerSetMotors(motorController.currentSpeed, motorController.currentHeading);
	else
		return; // tried to update motors, but in strafe mode - should probably send error back to base
}

void MotorControllerSetHeading(short heading) {
	motorController.currentHeading = heading;
	
	if (!motorController.inStrafeMode)
		MotorControllerSetMotors(motorController.currentSpeed, motorController.currentHeading);
	else
		return; // tried to update motors, but in strafe mode - should probably send error back to base
}

void MotorControllerSetSpeedHeading(short speed, short heading) {
	motorController.currentSpeed = speed;
	motorController.currentHeading = heading;
	
	if (!motorController.inStrafeMode)
		MotorControllerSetMotors(motorController.currentSpeed, motorController.currentHeading);
	else
		return; // tried to update motors, but in strafe mode - should probably send error back to base
}

void MotorControllerSetMotors(short speed, short heading) {
	short speeds[6];
	
	// FRONT
	// 1   6
	// 2   5
	// 3   4
	// BACK

	speeds[0] = speed + heading;
	speeds[1] = speed + heading;
	speeds[2] = speed + heading;
	
	speeds[5] = speed - heading;
	speeds[4] = speed - heading;
	speeds[3] = speed - heading;
	
	MotorControllerUpdateMotors(speeds,0,5);
}

void MotorControllerUpdateActuators() {
	short speed;
	char i;
	
	if (motorController.inStrafeMode) {
		speed = -1500;
	}
	else {
		speed = 1500;
	}
	
	for (i=0x40; i<=0x43; i++) {
		MotorControllerSendShort(i, MOTOR_CONTROL_DEV_SETVEL, speed);
	}
}

#ifdef MOTOR_UNTEST
void MotorControllerUpdateActuators_Old() {
	if (motorController.inStrafeMode) {
		motorController.lbogBus.newMotorSpeeds[2] = -1500;
		motorController.bbogBus.newMotorSpeeds[2] = -1500;
		motorController.bbogBus.newMotorSpeeds[3] = -1500;
		motorController.rbogBus.newMotorSpeeds[2] = -1500;
	}
	else {
		motorController.lbogBus.newMotorSpeeds[2] = 1500;
		motorController.bbogBus.newMotorSpeeds[2] = 1500;
		motorController.bbogBus.newMotorSpeeds[3] = 1500;
		motorController.rbogBus.newMotorSpeeds[2] = 1500;
	}
	
	/*if (motorController.lbogBus.transmitState!=MOTOR_STATE_NOTHING) {
		motorController.lbogBus.restartTransmitIndex=2;
	}
	else {
		motorController.lbogBus.transmitState = MOTOR_STATE_TRANSMIT;
		motorController.lbogBus.updateMotorIndex=2;
	}
	motorController.lbogBus.updateMotorStop=2;
	
	if (motorController.rbogBus.transmitState!=MOTOR_STATE_NOTHING) {
		motorController.rbogBus.restartTransmitIndex=2;
	}
	else {
		motorController.rbogBus.transmitState = MOTOR_STATE_TRANSMIT;
		motorController.rbogBus.updateMotorIndex=2;
	}
	motorController.rbogBus.updateMotorStop=2;
	
	if (motorController.bbogBus.transmitState!=MOTOR_STATE_NOTHING) {
		motorController.bbogBus.restartTransmitIndex=2;
	}
	else {
		motorController.bbogBus.transmitState = MOTOR_STATE_TRANSMIT;
		motorController.rbogBus.updateMotorIndex=2;
	}
	motorController.bbogBus.updateMotorStop=3;*/
}
#endif

void MotorControllerUpdateMotors_New(short speeds[6], unsigned char startMotor, unsigned char stopMotor) {
	
}

void MotorControllerUpdateMotors(short speeds[6], unsigned char startMotor, unsigned char stopMotor) {
	char cmd;
	char i;
	
	if (motorController.doPwmMode) {
		cmd = MOTOR_CONTROL_DEV_SETPWM;
	}
	else {
		cmd = MOTOR_CONTROL_DEV_SETVEL;
	}	
	
	for (i=0;i<6;i++) {
		MotorControllerSendShort(i+1, cmd, speeds[i]);
	}
	
	/*
	// motor 1
	motorController.lbogBus.newMotorCommand[0] = cmd;
	motorController.lbogBus.newMotorArgumentLength[0] = 2;
	motorController.lbogBus.newMotorArguments[0][0] = (speeds[0]>>8) & 0xFF;
	motorController.lbogBus.newMotorArguments[0][1] = speeds[0] & 0xFF;
	
	// motor 2
	motorController.lbogBus.newMotorCommand[1] = cmd;
	motorController.lbogBus.newMotorArgumentLength[1] = 2;
	motorController.lbogBus.newMotorArguments[1][0] = (speeds[1]>>8) & 0xFF;
	motorController.lbogBus.newMotorArguments[1][1] = speeds[1] & 0xFF;
	
	// motor 3
	motorController.bbogBus.newMotorCommand[0] = cmd;
	motorController.bbogBus.newMotorArgumentLength[0] = 2;
	motorController.bbogBus.newMotorArguments[0][0] = (speeds[2]>>8) & 0xFF;
	motorController.bbogBus.newMotorArguments[0][1] = speeds[2] & 0xFF;
	
	// motor 4
	motorController.bbogBus.newMotorCommand[1] = cmd;
	motorController.bbogBus.newMotorArgumentLength[1] = 2;
	motorController.bbogBus.newMotorArguments[1][0] = (speeds[3]>>8) & 0xFF;
	motorController.bbogBus.newMotorArguments[1][1] = speeds[3] & 0xFF;
	
	// motor 5
	motorController.rbogBus.newMotorCommand[0] = cmd;
	motorController.rbogBus.newMotorArgumentLength[0] = 2;
	motorController.rbogBus.newMotorArguments[0][0] = (speeds[4]>>8) & 0xFF;
	motorController.rbogBus.newMotorArguments[0][1] = speeds[4] & 0xFF;
	
	// motor 6
	motorController.rbogBus.newMotorCommand[1] = cmd;
	motorController.rbogBus.newMotorArgumentLength[1] = 2;
	motorController.rbogBus.newMotorArguments[1][0] = (speeds[5]>>8) & 0xFF;
	motorController.rbogBus.newMotorArguments[1][1] = speeds[5] & 0xFF;*/
}

void MotorControllerUpdateMotors_Old(short speeds[6], unsigned char startMotor, unsigned char stopMotor) {
	int i;
	
	#ifdef MOTOR_UNTEST

	motorController.lbogBus.newMotorSpeeds[0] = speeds[0];
	motorController.lbogBus.newMotorSpeeds[1] = speeds[1];
	
	motorController.bbogBus.newMotorSpeeds[0] = speeds[2];
	motorController.bbogBus.newMotorSpeeds[1] = speeds[3];
	
	motorController.rbogBus.newMotorSpeeds[0] = speeds[4];
	motorController.rbogBus.newMotorSpeeds[1] = speeds[5];
	
	/*if (motorController.lbogBus.transmitState==MOTOR_STATE_NOTHING) {
		motorController.lbogBus.updateMotorIndex=0;
		motorController.lbogBus.updateMotorStop=1;
		motorController.lbogBus.transmitState = MOTOR_STATE_TRANSMIT;
	}
	
	if (motorController.rbogBus.transmitState==MOTOR_STATE_NOTHING) {
		motorController.rbogBus.updateMotorIndex=0;
		motorController.rbogBus.updateMotorStop=1;
		motorController.rbogBus.transmitState = MOTOR_STATE_TRANSMIT;
	}
	
	if (motorController.bbogBus.transmitState==MOTOR_STATE_NOTHING) {
		motorController.bbogBus.updateMotorIndex=0;
		motorController.bbogBus.updateMotorStop=1;
		motorController.bbogBus.transmitState = MOTOR_STATE_TRANSMIT;
	}*/
	#else
	if (motorController.transmitState!=MOTOR_STATE_NOTHING) { // not ready to accept a new command (currently waiting on another to finish)
		/* IDEA:
		 * If the current motor index being transmitted to (or being waited on) is less than the startMotor we're adding, we
		 * can add the new motor speeds and change updateMotorStop.
		 */
		return;
	}
	else {
	
		memcpy(&motorController.motorSpeeds[startMotor],speeds,sizeof(short)*(stopMotor-startMotor+1)); // copy over the new speed updates
		motorController.updateMotorIndex=startMotor;
		motorController.updateMotorStop=stopMotor;
		motorController.transmitState = MOTOR_STATE_TRANSMIT;
		return;
	}
	#endif
}

void MotorControllerEStop() {
	short speeds[6] = {0, 0, 0, 0, 0, 0};
	
	MotorControllerUpdateMotors(speeds, 0, 5);
}

CommInterface * MotorControllerInfForDevice(char device) {
	switch (device) {
		case 1:
			return &motorController.lbog;
		case 2:
			return &motorController.lbog;
		case 3:
			return &motorController.bbog;
		case 4:
			return &motorController.bbog;
		case 5:
			return &motorController.rbog;
		case 6:
			return &motorController.rbog;
		
		// actuators:
		case 0x40:
			return &motorController.rbog;
			
		case 0x41:
			return &motorController.bbog;
					
		case 0x42:
			return &motorController.bbog;
			
		case 0x43:
			return &motorController.lbog;
		
		default: // unknown
			return &motorController.lbog;
//			return 0;
			
	}
}

MotorBus * MotorControllerBusForId(char device) {
	switch(device) {
		case 1:
			return &motorController.lbogBus;
		case 2:
			return &motorController.lbogBus;
		case 3:
			return &motorController.bbogBus;
		case 4:
			return &motorController.bbogBus;
		case 5:
			return &motorController.rbogBus;
		case 6:
			return &motorController.rbogBus;
		
		// actuators:
		case 0x40:
			return &motorController.rbogBus;
			
		case 0x41:
			return &motorController.bbogBus;
					
		case 0x42:
			return &motorController.bbogBus;
			
		case 0x43:
			return &motorController.lbogBus;
		
		default: // unknown
			return &motorController.lbogBus;
//			return 0;
	}
}

char MotorControllerBusPosForId(char device) {
	switch (device) {
		case 1:
			return 0;
		case 2:
			return 1;
		case 3:
			return 0;
		case 4:
			return 1;
		case 5:
			return 0;
		case 6:
			return 1;
		
		// actuators:
		case 0x40:
			return 2;
			
		case 0x41:
			return 2;
					
		case 0x42:
			return 3;
			
		case 0x43:
			return 2;
		
		default: // unknown
			return 0;
	}
}

void MotorControllerSendShort(char device, char cmd, short dat) {
	char idx = MotorControllerBusPosForId(device);
	MotorBus * bus = MotorControllerBusForId(device);
	bus->newMotorCommand[idx] = cmd;
	bus->newMotorArgumentLength[idx] = 2;
	bus->newMotorArguments[idx][0] = (dat>>8) & 0xFF;
	bus->newMotorArguments[idx][1] = dat & 0xFF;
}

void MotorControllerSendCommand(char device, char cmd, char length, char * dat) {
	char idx = MotorControllerBusPosForId(device);
	MotorBus * bus = MotorControllerBusForId(device);
	bus->newMotorCommand[idx] = cmd;
	bus->newMotorArgumentLength[idx] = length;
	memcpy(bus->newMotorArguments[idx], dat, length);
}

short MotorControllerBufToShort(char * buf) {
	return (short)(((buf[0]<<8)&0xFF00) | (buf[1]&0x00FF));
}

unsigned short MotorControllerBufToUShort(char * buf) {
	return (unsigned short)(((buf[0]<<8)&0xFF00) | (buf[1]&0x00FF));
}

void MotorControllerSendStatus() {
}

void MotorControllerDisableCornerMosfets(char disableMosfets) {
	char i;
	short speeds[6];
	if (disableMosfets) {
		MotorControllerSendCommand(1, MOTOR_CONTROL_DEV_DISABLE_MOSFETS, 0, 0);
		MotorControllerSendCommand(3, MOTOR_CONTROL_DEV_DISABLE_MOSFETS, 0, 0);
		MotorControllerSendCommand(4, MOTOR_CONTROL_DEV_DISABLE_MOSFETS, 0, 0);
		MotorControllerSendCommand(6, MOTOR_CONTROL_DEV_DISABLE_MOSFETS, 0, 0);
	}
	else {
		/*for (i=0;i<6;i++) {
			speeds[i]=0;
		}
		MotorControllerUpdateMotors(speeds,0,5);*/
	}
}

