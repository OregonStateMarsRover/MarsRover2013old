#include <stdio.h>
#include <string.h>

#include "ArmController.h"
#include "../ProcessManager/ProcessManager.h"

void ArmControllerInit() {
	USART_Open(&armController.port, 6, USART_BAUD_250000, 10, 10, true, true);
	CommInterfaceInit(&armController.inf, &armController.port);
	
	
	armController.sendData.state = STATE_IDLE;
	
	PacketQueueInit2(&armController.pktQueue, ARM_RX_PKT_QUEUE, 20, armController.queuedPackets, armController.queuedData);
}

void ArmControllerHandleMessage(Rover * rov, CommPacket * pkt) 
{
	if (PacketQueueCount(&armController.pktQueue)==ARM_RX_PKT_QUEUE) 
	{
		PacketQueueDropFront(&armController.pktQueue);
		//PORTA.OUT ^= (1<<0);
	}
	
	if (!PacketQueueAddPacket(&armController.pktQueue, pkt)) { // still couldn't add to queue--flag an error
		//PORTA.OUT ^= (1<<0);
	}
}

void ArmControllerSetupSendCommandTask(ArmSendCommandData * data) 
{
	data->timeoutCount = 0;
	data->state = STATE_SEND_COMMAND;
}

int8_t ArmControllerProcessSendCommandTask(ArmSendCommandData * data) {
	CommPacket pkt;
	char ret;
	
	switch (data->state)
	{
		case STATE_IDLE:
			return ARM_TASK_IDLE;
		break;
		
		case STATE_SEND_COMMAND: // Send command
		{
			// TODO: only send if interface is available
			if (!CommTransmissionInProgress(&armController.inf))
			{
				pkt.length = data->dataArrayLength;
				pkt.target = data->address;
				pkt.data = data->dataArray;
			
				


				CommSendPacket(&armController.inf, &pkt);
				StartTimer (&(data->timer));
				data->state = STATE_AWAIT_REPLY;
			}
			break;
		}
		
		case STATE_AWAIT_REPLY: // Wait for reply
		{
			unsigned char pktdata[20];
			pkt.data = pktdata;
			
			if (CommRXPacketsAvailable(&armController.inf)) 
			{
				ret = CommGetPacket(&armController.inf, &pkt, 20);
				
				if (pkt.data[0]==ARM_DATA_NOT_READY) 
				{
					ArmControllerSetupSendCommandTask(data); // resend data if data not ready
				}
				else 
				{
					//data->dataArray[0] = 74;
					//memset(&(data->dataArray[1]), 0, 14);
					//data->dataArrayLength =15;
					memcpy(data->dataArray, pkt.data, pkt.length);
					data->dataArrayLength = pkt.length;
					data->state = STATE_COMPLETE;
				}
			}
			else
			{
				if (GetSpanUs(&(data->timer)) > FROM_uS(SERIAL_TIMEOUT_US)) // Timeout!
				{
					data->state = STATE_SLAVE_TIMEOUT; // Goto slave timeout state
				}
			}
			break;
		}
			
		case STATE_COMPLETE: // Done
		{
			return ARM_TASK_COMPLETE;
		}
		
		case STATE_SLAVE_TIMEOUT: // slave timeout
		{
			//LedOn(data->timeoutCount); // TODO: Remove! (leave for debugging...)
			
			PORTA.OUT ^= (1<<4);
			data->timeoutCount++;
			if (data->timeoutCount >= TIMEOUT_LIMIT)
			{
				data->state = STATE_SEND_SLAVE_TIMEOUT;
			}
			else
			{
				data->state = STATE_SEND_COMMAND; // Send the command again
			}
			break;
		}
		
		case STATE_SEND_SLAVE_TIMEOUT:
		{
			return ARM_SLAVE_TIMEOUT;
		}
		
		default:
		{
			return ARM_IN_PROCESS; // Should return an error...  Somebody probably forgot a break...
		}
	}
	return ARM_IN_PROCESS;
}
/*
void ArmControllerSendToBase(Rover * rov, CommPacket * pkt) {
	if (TransmissionInProgress(rov) && !armController.txPktQueued) { // drop packet if arm already has one queued
		memcpy(armController.queuedTxPktData, pkt->data, pkt->length);
		armController.queuedTxPkt.target = pkt->target;
		armController.queuedTxPkt.length = pkt->length;
		armController.txPktQueued = true;
	}
	else {
		SendMessage(rov, pkt);
	}
}
*/

void ArmControllerTick(Rover * rov) 
{
	char ret;
	CommPacket pkt;
	
	ret = ArmControllerProcessSendCommandTask(&armController.sendData);
	switch(ret) {
		case ARM_TASK_COMPLETE:
			if (!TransmissionInProgress(rov))
			{
				pkt.data = armController.sendData.dataArray;
				pkt.length = armController.sendData.dataArrayLength;
				pkt.target = TARGET_GUI_ARM;
				if (SendMessage(rov, &pkt))
				{
					armController.sendData.state = STATE_IDLE;
				}
			}
			
			//ArmControllerSendToBase(rov, &pkt);
		break;
		
		case ARM_SLAVE_TIMEOUT:
			//if (!TransmissionInProgress(rov))
			//{
		//		pkt.data = armController.sendData.dataArray;
	//			pkt.length = 2;
	//			pkt.target = TARGET_GUI_ARM;
	//			pkt.data[0]=ARM_DATA_ERROR; // timeout
	//			pkt.data[1]=0xCD;
	//		
	//			if (SendMessage(rov, &pkt))
	//			{
	//				armController.sendData.state = STATE_IDLE;
	//			}
	//		}
			armController.sendData.state = STATE_IDLE;
		break;
		
		case ARM_IN_PROCESS:
		break;
	}
	
	if (armController.sendData.state == STATE_IDLE && PacketQueueCount(&armController.pktQueue)>0) 
	{
		//char data[20];
		pkt.data = armController.sendData.dataArray;
		
		if (PacketQueueGetPacket(&armController.pktQueue, &pkt)) 
		{
			armController.sendData.address = 0x77;
			//memcpy(armController.sendData.dataArray,pkt.data,pkt.length);
			armController.sendData.dataArrayLength = pkt.length;

				

			ArmControllerSetupSendCommandTask(&armController.sendData);
		}
	}
}

