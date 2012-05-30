#include "../ProcessManager/ProcessManager.h"

#include "USART.h"
#include "Timer.h"
#include "Common/CommInterface/CommInterface.h"
#include "../PacketQueuer/PacketQueuer.h"

#define STATE_SEND_COMMAND 1
#define STATE_AWAIT_REPLY 2
#define STATE_COMPLETE 3
#define STATE_SLAVE_TIMEOUT 4
#define STATE_SEND_SLAVE_TIMEOUT 5
#define STATE_IDLE 6

#define ARM_TASK_COMPLETE 1
#define ARM_SLAVE_TIMEOUT 2
#define ARM_IN_PROCESS 3
#define ARM_TASK_IDLE 4

#define TIMEOUT_LIMIT 8

#define SERIAL_TIMEOUT_US 5000 //100000

#define ARM_DATA_NOT_READY 0x01
#define ARM_DATA_ERROR 0x00

#define ARM_RX_PKT_QUEUE 4

typedef struct SArmSendCommandData
{
	// Public data
	uint8_t address;      	// Address to send to
	
	uint8_t dataArray[20];	// Buffer to store send data
	uint8_t dataArrayLength;// Length of used buffer (for array type data)

	// Private state data
	uint8_t state;				// Current State
	Timer timer;				// Timeout Timer
	uint8_t timeoutCount; 	// Number of timeouts
	
} ArmSendCommandData;

struct ArmControllerData {
	CommInterface inf;
	USART port;
	//CommPacket queuedTxPkt;
	//char queuedTxPktData[20];
	//bool txPktQueued;
	
	ArmSendCommandData sendData;
	
	PacketQueue pktQueue;
	CommPacket queuedPackets[ARM_RX_PKT_QUEUE];
	unsigned char queuedData[ARM_RX_PKT_QUEUE * 20];
};

struct ArmControllerData armController;

void ArmControllerInit();
void ArmControllerHandleMessage(Rover * rov, CommPacket * pkt);
void ArmControllerTick(Rover * rov);

void ArmControllerSetupSendCommandTask(ArmSendCommandData * data);
int8_t ArmControllerProcessSendCommandTask(ArmSendCommandData * data);

