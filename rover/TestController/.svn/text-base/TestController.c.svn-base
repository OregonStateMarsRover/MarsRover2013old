#include <stdio.h>
#include <string.h>

#include "TestController.h"
#include "../ProcessManager/ProcessManager.h"

void TestControllerInit() {
	testController.timeoutCounter = 0;
	
	//CommInterfaceInit(&testController.inf, &testController.port);
	StartTimer(&testController.pingTimer);
	testController.inc='A';
	
	USART_Open(&testController.cameraPort, 1, USART_BAUD_9600, 10, 10, true, true);
}

void TestControllerHandleMessage(Rover * rov, CommPacket * pkt) {
	char dat[10];
	
	if (pkt->data[0]=='T') { // timing packet
		dat[0]='T';
		dat[1]=pkt->data[1];
		dat[2]=pkt->data[2];
		
		CommPacket respPkt;
		respPkt.target = TARGET_GUI;
		respPkt.length = 5;
		respPkt.data = dat;
		SendMessage(rov,&respPkt);
	}
	else if (pkt->data[0]==0x01) { // error count
		CommPacket respPkt;
		respPkt.target = TARGET_GUI;
		respPkt.data = dat;
		respPkt.length = 8;
		respPkt.data[0] = 0x01;
		cli();
		memcpy(&(respPkt.data[1]), rov->dataIface.errorCount, 7);
		sei();
		SendMessage(rov,&respPkt);
	}
	else {
		dat[0]='X';
		dat[1]='M';
		dat[2]='E';
		dat[3]='G';
		dat[4]='A';
	
		dat[5]=pkt->data[0];
		dat[6]=pkt->data[1];
		dat[7]=pkt->data[2];
	
		CommPacket respPkt;
		respPkt.target = TARGET_GUI;
		respPkt.length = 8;
		respPkt.data = dat;
		SendMessage(rov,&respPkt);
	}
}

void TestControllerTick(Rover * rov) {
	char dat[2];
	char serDat[15];
	char len;
	
	if (GetSpanUs(&testController.pingTimer) > FROM_uS(1000000)) {
		dat[0]='Z';
		dat[1]=testController.inc++;
		if (dat[1]=='z')
			testController.inc='A';
	
		CommPacket respPkt;
		respPkt.target = TARGET_GUI;
		respPkt.length = 2;
		respPkt.data = dat;
		//SendMessage(rov,&respPkt);
		StartTimer(&testController.pingTimer);
	}
	
	if (USART_RXUsed(&testController.cameraPort)!=0) {
		len = USART_Read(&testController.cameraPort, serDat, 15);
		CommPacket respPkt;
		respPkt.target = TARGET_GUI;
		respPkt.length = 2;
		respPkt.data = dat;
		SendMessage(rov,&respPkt);
		
		dat[0]='S';
		dat[1]=serDat[0];
		SendMessage(rov,&respPkt);
	}

/*	char data[2] = { 'X', 'a' };
	CommPacket reqPacket;
	CommPacket confPacket;
	CommPacket * rcvdPacket;
	char confdata[6] = {'V','A','L','I','D', 0 };
	
	reqPacket.target = 1;
	reqPacket.length = 2;
	reqPacket.data = data;
	
	confPacket.target = TARGET_GUI;
	confPacket.length = 6;
	confPacket.data = confdata;
	
	if (testController.timeoutCounter>15000) { // time to send another message
		CommSendPacket(&testController.inf, &reqPacket);
		testController.timeoutCounter = 0;
	}
	else {
		testController.timeoutCounter++;
	}
	
	if (CommRXPacketsAvailable(&testController.inf)) { // do we have a reply packet to read?
		rcvdPacket = CommGetPacket(&testController.inf);
		if (rcvdPacket->target==0) {
				if (CommRXPacketsAvailable(&testController.inf)==4)
				confdata[3] = CommRXPacketsAvailable(&testController.inf);
			else
				confdata[3] = 'I';
		}
		confdata[3] = rcvdPacket->data[0];
		SendMessage(rov, &confPacket);
		
		CommDeletePacket(rcvdPacket);
		free(rcvdPacket);		
	}*/
}

