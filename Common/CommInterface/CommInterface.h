/******************************************************
 * Implements a packet-based communication interface
 * for sending/receiving serial data.
 *
 * Copyright (C) OSURC 2010.
 ******************************************************/

#ifndef HAVE_COMMINTERFACE_H
#define HAVE_COMMINTERFACE_H

#include <stdbool.h>

#include "SerialProtocol.h"

#include "USART.h"

#define PACKET_RX_QUEUE_LENGTH 5

struct CommPacket {
	volatile unsigned char target; /* Device the packet is sent to (TX) or from (RX). */
	volatile unsigned char length; /* Number of bytes of data in packet (length of data) */
	volatile unsigned char * data; /* Data of packet. Must be freed. */
	
	struct CommPacket * next; // internally used for tracking RX queue. Do not manipulate.
};
typedef struct CommPacket CommPacket;

struct CommInterface;
typedef struct CommInterface CommInterface;
struct CommInterface {
	USART * port;
	SerialData serData;
	
	CommPacket * firstRX; // beginning of RX queue
	CommPacket * lastRX; // end of RX queue
	unsigned int rx_pkts_queued;
	
	void (*ReceivePacketComplete)(CommInterface * inf); // callback when a packet is rx'd and available for reading
	void (*TransmitPacketComplete)(CommInterface * inf); // callback when a packet is rx'd and available for reading
	
	CommPacket rcvdPacket;
	unsigned char rcvdData[20];
	volatile char hasRcvdPacket;
	
	volatile unsigned char errorCount[7];
};


/**
 * Initializes the communication interface.
 */
void CommInterfaceInit(CommInterface * inf, USART * serPort);

void CommInterfaceSetRXCallback(CommInterface * inf, void (*func)(CommInterface * inf));

void CommInterfaceSetTXCallback(CommInterface * inf, void (*func)(CommInterface * inf));

/**
 * Queues a packet for transmission. Data is deep-copied
 * out of the CommPacket, so it may be destroyed after
 * being queued.
 * Returns: True if packet is successfully queued, false otherwise
 * (out of buffer space, etc).
 */
bool CommSendPacket(CommInterface * inf, CommPacket * pkt);

/**
 * Gets the oldest packet out of the RX packet queue
 * and places the data into pkt. Data is deep-copied,
 * so it must be freed when no longer in use.
 * The packet returned is removed from the queue.
 *
 * Returns: A pointer to a new CommPacket if available, or
 * 0 if no packet is available in queue.
 */
char CommGetPacket(CommInterface * inf, CommPacket * pkt, unsigned char maxbuflen);

/**
 * Same as CommGetPacket(), but the packet is not removed
 * from the RX queue.
 */
char CommPeekPacket(CommInterface * inf, CommPacket * pkt, unsigned char maxbuflen);

unsigned int CommRXPacketsAvailable(CommInterface * inf);

/**
 * Convenience function for deleting a packet.
 */
void CommDeletePacket(CommPacket * pkt);

/**
 * Convenience function to delete a comm interface.
 * Frees memory used by all packets in RX queue.
 **/
void CommDeleteInterface(CommInterface * inf);

bool CommTransmissionInProgress(CommInterface * inf);

/**
 * Handles byte receive from serial interface.
 */
void CommByteReceived(USART * port, unsigned char dat);
void CommByteTransmitted(USART * port);

void CommQueueForTx(SerialData * ser, unsigned char data);
void CommPacketReceived(SerialData * ser);
void CommReceiveDataError(SerialData * ser, unsigned char errCode);
void CommPktTxComplete(SerialData * ser);

#endif

