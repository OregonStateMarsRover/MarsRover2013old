#include <stdio.h>
#include <string.h>

#include "TripodController.h"
#include "../ProcessManager/ProcessManager.h"

void TripodControllerInit() {
	USART_Open(&tripodController.port, 1, USART_BAUD_38400, 10, 10, true, true);
	CommInterfaceInit(&tripodController.inf, &tripodController.port);
	
	PacketQueueInit2(&tripodController.pktQueue, 6, 20, tripodController.queuedPackets, tripodController.queuedData);
}

void TripodControllerHandleMessage(Rover * rov, CommPacket * pkt) {
	// simply pass this message through with a modified target
	/*pkt->target = 0x02;
	
	CommSendPacket(&tripodController.inf, pkt);
	
	pkt->target = TARGET_GUI_TRIPOD;
	pkt->length=1;
	SendMessage(rov, pkt);*/
	
	PacketQueueAddPacket(&tripodController.pktQueue, pkt);
}

void TripodControllerTick(Rover * rov) {
	// maybe check in here if we've received data from the accelerometer or barometer
	CommPacket commPkt;
	unsigned char data[20];
	commPkt.data=data;
	char ret;
	
	if (CommRXPacketsAvailable(&tripodController.inf) && !TransmissionInProgress(rov)) {
		ret = CommGetPacket(&tripodController.inf, &commPkt, 20);
		
		if (!ret) {
			data[0]=0xFF;
			commPkt.length=1;
		}
		
		commPkt.target = TARGET_GUI_TRIPOD;
		SendMessage(rov, &commPkt);
	}
	
	if (PacketQueueCount(&tripodController.pktQueue) && !CommTransmissionInProgress(&tripodController.inf)) { // a queued packet is ready to be transmitted out
		if (PacketQueueGetPacket(&tripodController.pktQueue, &commPkt)) {
			commPkt.target = 0x02; // target the tripod board
			CommSendPacket(&tripodController.inf, &commPkt);
		}
	}
}

