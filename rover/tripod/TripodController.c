#include <stdio.h>
#include <string.h>

#include "TripodController.h"
#include "../ProcessManager/ProcessManager.h"

void TripodControllerInit() {
	USART_Open(&tripodController.port, 1, USART_BAUD_38400, 10, 10, true, true); //from /Common/Interfaces/Serial/X86/USART.c. Opens up a USART serial to send packets over. Arguments are USART * serial, unsigned char port, unsigned char baud_rate, unsigned short tx_buf, unsigned short rx_buf, bool use_rs485.
	//Open up a USART on the tripodController port 1 with a baud rate of 38,400, transmit and receive buffer times of 10 (ms?), enable the receive buffer, enable rs485
	CommInterfaceInit(&tripodController.inf, &tripodController.port); //from /Common/CommInterface/CommInterface.c.
	//Initialize a comm interface between the tripod controller comm interface and the USART port.
	
	PacketQueueInit2(&tripodController.pktQueue, 6, 20, tripodController.queuedPackets, tripodController.queuedData); //from /rover/PacketQueuer.c
	//Initialize the packet queue on the tripod controller. Use 6 packets, a packet length of 20, the queued packets in the tripodController, and the queued data in the tripodController.
}

void TripodControllerHandleMessage(Rover * rov, CommPacket * pkt) {
	// simply pass this message through with a modified target
	/*pkt->target = 0x02;
	
	CommSendPacket(&tripodController.inf, pkt);
	
	pkt->target = TARGET_GUI_TRIPOD;
	pkt->length=1;
	SendMessage(rov, pkt);*/
	
	PacketQueueAddPacket(&tripodController.pktQueue, pkt); //from /rover/PacketQueuer.c.
	//Pretty self-explanatory; add incoming messages to the packet queue.
}

void TripodControllerTick(Rover * rov) {
	// maybe check in here if we've received data from the accelerometer or barometer
	CommPacket commPkt; //Struct CommPacket from /Common/CommInterface/CommInterface.h
	unsigned char data[20];
	commPkt.data=data;
	char ret;
	
	if (CommRXPacketsAvailable(&tripodController.inf) && !TransmissionInProgress(rov)) { //CommRXPacketsAvailable is from /Common/CommInterface/CommInterface.c. TransmissionInProgress() is from /rover/ProcessManager/ProcessManager.c.
		//if the tripodController received packets and there currently are no transmissions in progress to the rover?, continue.
		ret = CommGetPacket(&tripodController.inf, &commPkt, 20); //from /Common/CommInterface/CommInterface.c
		//get the packet from the tripodController's comm interface
		
		if (!ret) { //if there was no packet...
			data[0]=0xFF; //set the data to FF
			commPkt.length=1; //and the packet length to 1.
		}
		
		commPkt.target = TARGET_GUI_TRIPOD; //set the packet's target
		SendMessage(rov, &commPkt); //from /rover/ProcessManager/ProcessManager.c
		//Send the packet to the rover?
	} //is this if statement for incoming and the other for outgoing packets?
	
	if (PacketQueueCount(&tripodController.pktQueue) && !CommTransmissionInProgress(&tripodController.inf)) { // a queued packet is ready to be transmitted out. CommTransmissionInProgress is from /Common/CommInterface/CommInterface.c
		//if there are packets in the queue and a transmission is not currently in progress, continue.
		if (PacketQueueGetPacket(&tripodController.pktQueue, &commPkt)) { //from /Common/CommInterface/CommInterface.c
			//if PacketQueueGetPacket returned a packet, proceed to send it.
			commPkt.target = 0x02; // target the tripod board
			CommSendPacket(&tripodController.inf, &commPkt); //from /Common/CommInterface/CommInterface.c
			//send the packet to the tripod board
		}
	}
}

