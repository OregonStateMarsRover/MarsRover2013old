#include "../ProcessManager/ProcessManager.h"

#include "USART.h"
#include "Timer.h"
#include "Common/CommInterface/CommInterface.h"
#include "../PacketQueuer/PacketQueuer.h"

struct TripodControllerData {
	CommInterface inf;
	USART port;
	
	PacketQueue pktQueue;
	CommPacket queuedPackets[6];
	unsigned char queuedData[6 * 20];
};

struct TripodControllerData tripodController;

void TripodControllerInit();
void TripodControllerHandleMessage(Rover * rov, CommPacket * pkt);
void TripodControllerTick(Rover * rov);

