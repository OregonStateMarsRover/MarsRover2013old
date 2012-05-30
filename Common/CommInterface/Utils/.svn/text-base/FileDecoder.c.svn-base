/****************************************
 * Decodes a stream or file of packets
 * and displays data from parsed packets.
 *
 *****************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "../SerialProtocol.h"

SerialData ser;

void Transmit(byte data)
{
    ProcessDataChar(&ser, data);
}

/* Called when the whole packet is transferred
 * Beware: This may be called in interrupt context
 * within a MCU, so don't take a long time
 * processing junk!
 */
void TransmitPacketComplete (void)
{
    printf("\n");
    printf("Packet Transfer Complete\n");
}

/* Called when the whole packet is received
 * Beware: This may be called in interrupt context
 * within a MCU, so don't take a long time
 * processing junk!
 */
void ReceivePacketComplete (SerialData * s)
{
    printf("\n\nSerial Receive Complete: Address = %d, Length = %d\n   Data: ", s->receive_address, s->receive_data_count);
    unsigned char i = 0;

    /* Do something with the data here!*/
    for (i = 0; i<s->receive_data_count; i++)
    {
        printf("%02X ", s->receive_data[i]);
    }
    printf("\n");
}

/* Called when serial data receive has an error
 * Beware: This may be called in interrupt context
 * within a MCU, so don't take a long time
 * processing junk!
 */
void ReceiveDataError (byte errCode)
{
    printf("Serial Receive Data Error: %d\n", errCode);
}

int main(int argc, char ** argv) {
	char data;
	if (argc<2) {
		printf("Usage: filedecoder [file]\n");
		return -1;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd==-1) {
		perror("open");
	}
	
	SerialDataInitialize(&ser);

	// Setup function pointers
	ser.TransmitPacketComplete = &TransmitPacketComplete;
	ser.Transmit = &Transmit;
	ser.ReceivePacketComplete = &ReceivePacketComplete;
	ser.ReceiveDataError = &ReceiveDataError;
	
	while (read(fd, &data,1)==1) {
		ProcessDataChar(&ser, data);
		printf("Processing byte 0x%x\n",data);
	}
	printf("Read complete\n");
}

