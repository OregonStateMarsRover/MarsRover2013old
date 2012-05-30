#include "../ProcessManager/ProcessManager.h"

#include "USART.h"
#include "Timer.h"
#include "Common/CommInterface/CommInterface.h"

struct TestControllerData {
	CommInterface inf;
	USART port;
	USART cameraPort;
	unsigned short timeoutCounter;
	
	Timer pingTimer;
	char inc;
};

struct TestControllerData testController;

void TestControllerInit();
void TestControllerHandleMessage(Rover * rov, CommPacket * pkt);
void TestControllerTick(Rover * rov);

