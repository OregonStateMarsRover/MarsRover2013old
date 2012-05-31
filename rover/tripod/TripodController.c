#include <stdio.h>
#include <string.h>

#include "TripodController.h"
#include "../ProcessManager/ProcessManager.h"

void TripodControllerInit() {
	USART_Open(&tripodController.port, 1, USART_BAUD_38400, 10, 10, true, true); //from /Common/Interfaces/Serial/X86/USART.c. Opens up a USART serial to send packets over. Arguments are USART * serial, unsigned char port, unsigned char baud_rate, unsigned short tx_buf, unsigned short rx_buf, bool use_rs485.
	CommInterfaceInit(&tripodController.inf, &tripodController.port); //from /Common/CommInterface/CommInterface.c.
	
	PacketQueueInit2(&tripodController.pktQueue, 6, 20, tripodController.queuedPackets, tripodController.queuedData); //from /rover/PacketQueuer.c
}

void TripodControllerHandleMessage(Rover * rov, CommPacket * pkt) {
	// simply pass this message through with a modified target
	/*pkt->target = 0x02;
	
	CommSendPacket(&tripodController.inf, pkt);
	
	pkt->target = TARGET_GUI_TRIPOD;
	pkt->length=1;
	SendMessage(rov, pkt);*/
	
	PacketQueueAddPacket(&tripodController.pktQueue, pkt); //from /rover/PacketQueuer.c.
}

void TripodControllerTick(Rover * rov) {
	// maybe check in here if we've received data from the accelerometer or barometer
	CommPacket commPkt; //Struct CommPacket from /Common/CommInterface/CommInterface.h
	unsigned char data[20];
	commPkt.data=data;
	char ret;
	
	if (CommRXPacketsAvailable(&tripodController.inf) && !TransmissionInProgress(rov)) { //CommRXPacketsAvailable is from /Common/CommInterface/CommInterface.c. 
		ret = CommGetPacket(&tripodController.inf, &commPkt, 20); //from /Common/CommInterface/CommInterface.c
		
		if (!ret) {
			data[0]=0xFF;
			commPkt.length=1;
		}
		
		commPkt.target = TARGET_GUI_TRIPOD;
		SendMessage(rov, &commPkt); //from /rover/ProcessManager/ProcessManager.c
	}
	
	if (PacketQueueCount(&tripodController.pktQueue) && !CommTransmissionInProgress(&tripodController.inf)) { // a queued packet is ready to be transmitted out
		if (PacketQueueGetPacket(&tripodController.pktQueue, &commPkt)) { //from /Common/CommInterface/CommInterface.c
			commPkt.target = 0x02; // target the tripod board
			CommSendPacket(&tripodController.inf, &commPkt); //from /Common/CommInterface/CommInterface.c
		}
	}
}

