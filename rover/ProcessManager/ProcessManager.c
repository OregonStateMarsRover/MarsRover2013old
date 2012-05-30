#include "ProcessManager.h"
#include <avr/interrupt.h>

Rover * GlobalRover = 0;

char TargetToIndex(unsigned char target) {
	// TODO: Replace this with an array for speed
	if (target==TARGET_GPS_CONTROLLER) {
		return GPS_CONTROLLER_INDEX;
	}
	else if (target==TARGET_TEST_CONTROLLER) {
		return TEST_CONTROLLER_INDEX;
	}
	else if (target==TARGET_MOTOR_CONTROLLER) {
		return MOTOR_CONTROLLER_INDEX;
	}
	else if (target==TARGET_TRIPOD_CONTROLLER) {
		return TRIPOD_CONTROLLER_INDEX;
	}
	else if (target==TARGET_ARM_CONTROLLER) {
		return ARM_CONTROLLER_INDEX;
	}
	else {
		return -1;
	}
}

void InitRover(Rover * rov) {
	unsigned char i=0;
	for (i=0; i<NUM_MODULES; i++) { // init 
		rov->messageHandlers[i] = 0;
		rov->moduleTickers[i] = 0;
		
		rov->messageTargets[i].pkt.data = rov->messageTargets[i].data;
	}
	
	USART_Open(&rov->dataSerial, 5, USART_BAUD_115200, 255, 255, false, true);
	CommInterfaceInit(&rov->dataIface, &rov->dataSerial);
	CommInterfaceSetRXCallback(&rov->dataIface, &WirelessMessageReceived);
	CommInterfaceSetTXCallback(&rov->dataIface, &WirelessMessageTransmitComplete);
	
	StartTimer(&(GlobalRover->pktTxTimer));
	
	GlobalRover = rov;
}

void RegisterModule(Rover * rov, unsigned char module_target, MessageHandler msgHandler, ModuleTicker ticker) {
	rov->messageHandlers[TargetToIndex(module_target)] = msgHandler;
	rov->moduleTickers[TargetToIndex(module_target)] = ticker;
	
	//rov->messageTargets[TargetToIndex(module_target)] = messageTarget;
	rov->messageTargets[TargetToIndex(module_target)].pkt.data = rov->messageTargets[TargetToIndex(module_target)].data;
	rov->messageTargets[TargetToIndex(module_target)].isProcessed = true; // is ready to receive first packet
}

void RunModules(Rover * rov) {
	unsigned char i;
	
	for (i=0;i<NUM_MODULES;i++) {
		if (rov->moduleTickers[i]!=0) {
			rov->moduleTickers[i](rov);
		}
	}
}

void DispatchMessage(Rover * rov, CommPacket * pkt) {
	char idx = TargetToIndex(pkt->target);
	
	if (idx<0) {
		//printf("Unknown target %d. Ignoring.\n",pkt->target);
	}
	else {
		if (rov->messageHandlers[idx]!=0)
			rov->messageHandlers[idx](rov,pkt);
	}
}

// OCCURS IN INTERRUPT CONTEXT!!!  CAREFUL WITH VARIABLES MODIFIED HERE!
void WirelessMessageReceived(CommInterface * inf) {
	//PORTA.OUT |= (1<<0);
	if (!GlobalRover)
		return;
	
	char idx = TargetToIndex(inf->rcvdPacket.target);
	if (idx<0) { // unknown index, drop pkt
		inf->hasRcvdPacket = false;
		return;
	}
	
	if (!GlobalRover->messageTargets[idx].isProcessed) { // not ready to copy in data, drop packet
		inf->hasRcvdPacket = false;
		//PORTA.OUT ^= (1<<4);
		return;
	}
	
	if (CommGetPacket(inf, &(GlobalRover->messageTargets[idx].pkt), WIRELESS_MESSAGE_MAXLEN)) {
		GlobalRover->messageTargets[idx].isProcessed = false;
	}
	//PORTA.OUT &= ~(1<<0);
}

void WirelessMessageTransmitComplete(CommInterface * inf) {
	if (!GlobalRover)
		return;
	
	StartTimer(&(GlobalRover->pktTxTimer));
}

void DispatchMessages(Rover * rov) {
	CommPacket commPkt;
	unsigned char data[20];
	commPkt.data=data;
	char ret;
	char i; 




	// check through each module to see if it's received a message
	for (i=0; i<NUM_MODULES; i++) 
	{	
		// To isolate an interface for testing...
		//if (i != ARM_CONTROLLER_INDEX)
		//	continue;
		
		// have a message ready for processing	
		if (rov->messageTargets[i].isProcessed==false) 
		{ 			

		char dat[10];
		//dat[0]=i;
		dat[0]='R';
		dat[1]='E';
		dat[2]='C';
		//dat[3]=rov->messageTargets[i].pkt.target;
		dat[3]=2;
		dat[4]=rov->messageTargets[i].pkt.data[0];

		CommPacket respPkt;
		respPkt.target = TARGET_GUI;
		respPkt.length = 5;
		respPkt.data = dat;
		//SendMessage(rov,&respPkt);


			//commPkt.target = rov->messageTargets[i].pkt.target;
			
			commPkt.target = 2;
			commPkt.length = rov->messageTargets[i].pkt.length;
			memcpy(commPkt.data, rov->messageTargets[i].pkt.data, commPkt.length);
			rov->messageTargets[i].isProcessed=true;
			DispatchMessage(rov, &commPkt);
		}
	}
	
	/*while(CommRXPacketsAvailable(&rov->dataIface)) {
		ret = CommGetPacket(&rov->dataIface, &commPkt, 20);
		if (!ret) { // there was a packet available, but we didn't get it returned to us - give up for now
			break;
		}
		
		if (commPkt.length==0) { // bad 0-length packet, skip to the next one
			continue;
		}
		DispatchMessage(rov,&commPkt);
	}*/
	
	/*cli();
	CommPacket * cur;
	CommPacket * nextPkt;
	nextPkt = rov->dataIface.firstRX;
	while(nextPkt) {
		cur = nextPkt;
		nextPkt = nextPkt->next;
		CommDeletePacket(cur);
		free(cur);
	}
	rov->dataIface.rx_pkts_queued=0;
	rov->dataIface.firstRX = 0;
	rov->dataIface.lastRX = 0;
	sei();*/
}

bool SendMessage(Rover * rov, CommPacket * pkt) {
	if (GetSpanUs(&rov->pktTxTimer)<FROM_uS(200))
		return false;
	
	return CommSendPacket(&rov->dataIface, pkt);
}

bool TransmissionInProgress(Rover * rov) {
	return CommTransmissionInProgress(&rov->dataIface) || (GetSpanUs(&rov->pktTxTimer)<FROM_uS(200));
}

