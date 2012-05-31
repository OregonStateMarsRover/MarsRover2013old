#include "PacketQueuer.h"
#include <string.h>

// Broken, semi-working queue thing
void PacketQueueInit(PacketQueue * queue, unsigned char numPackets, unsigned char packetLength, CommPacket * packets, unsigned char ** packetData) {
	unsigned char i;
	
	queue->queueLength = numPackets;
	queue->dataLength = packetLength;
	
	queue->packets = packets;
	
	queue->head = 0;
	queue->tail = 0;
	queue->count = 0;

	for (i=0;i<numPackets;i++) 
	{
		queue->packets[i].data = packetData[i];
	}
}

// Less broken queue initialization.  Make packetData be "unsigned char data[packetLength * numPackets]" and all will be well.
void PacketQueueInit2(PacketQueue * queue, unsigned char numPackets, unsigned char packetLength, CommPacket * packets, unsigned char * packetData) {
	unsigned char i;
	
	queue->queueLength = numPackets;
	queue->dataLength = packetLength;
	
	queue->packets = packets;
	
	queue->head = 0;
	queue->tail = 0;
	queue->count = 0;
	
	for (i=0;i<numPackets;i++) 
	{
		queue->packets[i].data = &(packetData[i * packetLength]);
	}
}

bool PacketQueueAddPacket(PacketQueue * queue, CommPacket * pkt) 
{
	if (queue->count < queue->queueLength) 
	{
		CommPacket * tailPacket = &(queue->packets[queue->tail]);
		
		queue->tail = (queue->tail + 1) % (queue->queueLength);
		queue->count++;
		
		// copy packet
		tailPacket->target = pkt->target;
		tailPacket->length = pkt->length;
		
		// safety--truncate packet if over max length
		if (pkt->length > queue->dataLength)
		{
			tailPacket->length = queue->dataLength;
		}
		
		unsigned char i;
		for (i = 0; i<pkt->length; i++)
		{
			tailPacket->data[i] = pkt->data[i];
		}
		//memcpy(queue->packets[queue->tail].data, pkt->data, queue->packets[queue->tail].length);
		
		

		return true;
	}
	else 
	{ 
		// queue full
		return false;
	}
}

bool PacketQueueGetPacket(PacketQueue * queue, CommPacket * pkt) 
{
	if (queue->count > 0) 
	{
		CommPacket * headPacket = &(queue->packets[queue->head]);
		
		queue->head = (queue->head+1) % (queue->queueLength);
		queue->count--;

		pkt->target = headPacket->target;
		pkt->length = headPacket->length;
		
		unsigned char i;
		for (i = 0; i < pkt->length; i++)
		{
			pkt->data[i] = headPacket->data[i];
		}
//memcpy(pkt->data, queue->packets[queue->head].data, pkt->length);

		return true;
	}
	else 
	{
		return false;
	}
}

bool PacketQueueDropFront(PacketQueue * queue) 
{
	if (queue->count>0) {
		queue->head = (queue->head+1) % queue->queueLength; // advance head
		queue->count--;
		return true;
	}
	else {
		return false;
	}
}

unsigned char PacketQueueCount(PacketQueue * queue) {
	return queue->count;
}

