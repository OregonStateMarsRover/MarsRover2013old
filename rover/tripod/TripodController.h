#include "../ProcessManager/ProcessManager.h"

#include "USART.h"
#include "Timer.h"
#include "Common/CommInterface/CommInterface.h"
#include "../PacketQueuer/PacketQueuer.h"

struct TripodControllerData {
	CommInterface inf; //Tripod comm interface;
	USART port; //instantiation of the USART
	
	PacketQueue pktQueue; //TripodController packet queue for sending.
	CommPacket queuedPackets[6]; //instantiate 6 CommPackets
	unsigned char queuedData[6 * 20]; //allocate a queuedData variable with 20 chars for data for each of the 6 packets.
};

struct TripodControllerData tripodController;

//function prototypes for TripodController.c
void TripodControllerInit(); 
void TripodControllerHandleMessage(Rover * rov, CommPacket * pkt);
void TripodControllerTick(Rover * rov);

