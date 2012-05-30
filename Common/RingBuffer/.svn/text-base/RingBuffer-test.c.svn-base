#include "RingBuffer.h"

int main() {
	RingBuffer d;
	char buf[50];
	int i;
	
	RingBufferInit(&d, 255);
	
	for (i=0;i<80;i++)
		RingBufferAddByte(&d,'H');
	
	i=0;
	while(RingBufferBytesUsed(&d)) {
		printf("%d: %c\n",i,RingBufferGetByte(&d));
		i++;
	}
}

