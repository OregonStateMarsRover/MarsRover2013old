/**********************************************************************
 *
 * Basic C ring buffer. Designed to be portable between architectures
 * Written by Taj Morton for OSURC Mars Rover.
 * Copyright (C) 2010 OSURC.
 *
 **********************************************************************/

#ifndef HAVE_RINGBUFFER_H
#define HAVE_RINGBUFFER_H

#include <stdbool.h>

struct RingBuffer {
	unsigned char * data;
	unsigned short start, end, count;
	unsigned short size;
};

typedef struct RingBuffer RingBuffer;

void RingBufferInit(RingBuffer * buf, unsigned short size);
void RingBufferDelete(RingBuffer * buf);
unsigned short RingBufferAdd(RingBuffer * buf, unsigned char * data, unsigned short size);
bool RingBufferAddByte(RingBuffer * buf, unsigned char data);
unsigned char RingBufferGetByte(RingBuffer * buf);
unsigned short RingBufferGetData(RingBuffer * buf, unsigned char * dest_buf, unsigned short bytes);
unsigned short RingBufferBytesUsed(RingBuffer * buf);
unsigned short RingBufferBytesFree(RingBuffer * buf);

#endif

