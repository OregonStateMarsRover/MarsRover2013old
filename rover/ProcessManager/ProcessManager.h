#ifndef HAVE_PROCESSMANAGER_H
#define HAVE_PROCESSMANAGER_H

#include <stdlib.h>
#include "Common/CommInterface/CommInterface.h"
#include "Timer.h"

#define GPS_CONTROLLER_INDEX 0
#define TEST_CONTROLLER_INDEX 1
#define MOTOR_CONTROLLER_INDEX 2
#define TRIPOD_CONTROLLER_INDEX 3
#define ARM_CONTROLLER_INDEX 4

// This list should be kept in sync with the one in Software/Interface/VirtualRover/AbstractController.h
#define TARGET_GPS_CONTROLLER 1
#define TARGET_MOTOR_CONTROLLER 2
#define TARGET_TRIPOD_CONTROLLER 3
#define TARGET_ARM_CONTROLLER 10

#define TARGET_GUI 4 // use for generic GUI messages. Use a more specific TARGET_GUI_* module if possible
#define TARGET_TEST_CONTROLLER 5
#define TARGET_GUI_MOTOR 6
#define TARGET_GUI_TRIPOD 7
#define TARGET_GUI_ARM 8
#define TARGET_GUI_GPS 9

/*
 * NOTE: In the context of the Process Manager, a wireless messages
 * TARGET is referred to as the CommPacket.target.
 */

struct Rover; // forward declaration for callback typedefs
typedef struct Rover Rover;

typedef void (*MessageHandler)(Rover * rov, CommPacket * pkt);
typedef void (*ModuleTicker)(Rover * rov);

#define NUM_MODULES 5

#define WIRELESS_MESSAGE_MAXLEN 20
struct WirelessMessage {
	volatile bool isProcessed;
	CommPacket pkt;
	char data[WIRELESS_MESSAGE_MAXLEN];
};
typedef struct WirelessMessage WirelessMessage;

struct Rover {
	USART dataSerial;
	CommInterface dataIface;
	
	MessageHandler messageHandlers[NUM_MODULES];
	ModuleTicker moduleTickers[NUM_MODULES];
	WirelessMessage messageTargets[NUM_MODULES];
	Timer pktTxTimer;
};

char TargetToIndex(unsigned char target);
void InitRover(Rover * rov);
void RunModules(Rover * rov);
void DispatchMessage(Rover * rov, CommPacket * pkt);
void DispatchMessages(Rover * rov);
bool SendMessage(Rover * rov, CommPacket * pkt);
bool TransmissionInProgress();
void WirelessMessageReceived(CommInterface * inf);
void WirelessMessageTransmitComplete(CommInterface * inf);

void RegisterModule(Rover * rov, unsigned char module_target, MessageHandler msgHandler, ModuleTicker ticker);

void led_on(char led);
void led_off(char led);

#endif

