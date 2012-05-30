#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "CommInterface.h"

static unsigned long memd = 0;
static void *(*old_malloc_hook) (size_t);
static void *(*old_free_hook) (void*);

static void *my_malloc_hook (size_t size, const void * caller) {
	void *result;
	__malloc_hook = old_malloc_hook;
	
	if (memd>100 ) {
		result = 0;
	}
	else {
		memd++;
		result = malloc (size);
	}
	
	old_malloc_hook = __malloc_hook;
	__malloc_hook = my_malloc_hook;

	return result;
}

static void my_free_hook (void *ptr, const void *caller)
{
	memd--;
       /* Restore all old hooks */
       __malloc_hook = old_malloc_hook;
       __free_hook = old_free_hook;
       /* Call recursively */
       free (ptr);
       /* Save underlying hooks */
       old_malloc_hook = __malloc_hook;
       old_free_hook = __free_hook;
       /* printf might call free, so protect it too. */
       //printf ("freed pointer %p\n", ptr);
       /* Restore our own hooks */
       __malloc_hook = my_malloc_hook;
       __free_hook = my_free_hook;
}

static void my_init_hook (void) {
	old_malloc_hook = __malloc_hook;
	__malloc_hook = my_malloc_hook;
	
	old_free_hook = __free_hook;
	__free_hook = my_free_hook;
}

void (*__malloc_initialize_hook) (void) = my_init_hook;

void RXPkt() {
//	printf("rx'ed a packet\n");
}


void DispatchMessages(CommInterface * inf) {
	CommPacket * commPkt;
	
	while(CommRXPacketsAvailable(inf)) {
		commPkt = CommGetPacket(inf);
		if (!commPkt) { // there was a packet available, but we didn't get it returned to us - give up for now
			break;
		}
		
		if (commPkt->length==0) { // bad 0-length packet, skip to the next one
			CommDeletePacket(commPkt);
			free(commPkt);
			continue;
		}
		
		//DispatchMessage(rov,commPkt);
		printf("Got msg\n");
		
		CommDeletePacket(commPkt);
		free(commPkt);
	}
}

int main() {
	int i,j;
	CommInterface inf;
	CommPacket pkt;
	char * dat = "Hello, World\n";
	char dat2[19];
	CommInterfaceInit(&inf, 0);
	CommInterfaceSetRXCallback(&inf, &RXPkt);
	
	memset(dat2,'A',19);
	pkt.target=99;
	pkt.length=19;
	pkt.data = dat2;
	
	for (i=0;i<10;i++) {
		for (j=0;j<100;j++) {
			if (CommSendPacket(&inf, &pkt)) {
				//printf("packet sent %lu\n",memd);
			}
			else {
				printf("failed to send %lu\n",memd);
			}
		}
		printf("%d queued\n", inf.rx_pkts_queued);
		DispatchMessages(&inf);
	}
	DispatchMessages(&inf);
	printf("we have %d packets in the queue!\n",CommRXPacketsAvailable(&inf));
	
	CommDeleteInterface(&inf);
}

