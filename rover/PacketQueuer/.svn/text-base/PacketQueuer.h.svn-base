/**************************************
 *
 * Packet Queuing System
 *
 **************************************/

#ifndef HAVE_PACKETQUEUER_H
#define HAVE_PACKETQUEUER_H

#include <stdbool.h>

#include "Common/CommInterface/CommInterface.h"

struct PacketQueue {
	unsigned char queueLength;
	unsigned char dataLength;
	
	CommPacket *packets;
	//unsigned char **packetData;
	unsigned char head, tail, count;
};

typedef struct PacketQueue PacketQueue;

void PacketQueueInit(PacketQueue * queue, unsigned char numPackets, unsigned char packetLength, CommPacket * packets, unsigned char ** packetData);
void PacketQueueInit2(PacketQueue * queue, unsigned char numPackets, unsigned char packetLength, CommPacket * packets, unsigned char * packetData);
bool PacketQueueAddPacket(PacketQueue * queue, CommPacket * pkt);
bool PacketQueueGetPacket(PacketQueue * queue, CommPacket * pkt);
bool PacketQueueDropFront(PacketQueue * queue);
unsigned char PacketQueueCount(PacketQueue * queue);

#endif

