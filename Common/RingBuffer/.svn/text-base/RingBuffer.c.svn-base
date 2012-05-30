/**********************************************************************
 *
 * Basic C ring buffer. Designed to be portable between architectures
 * Written by Taj Morton for OSURC Mars Rover.
 * Copyright (C) 2010 OSURC.
 *
 **********************************************************************/

#include <stdlib.h>

#include "RingBuffer.h"

void RingBufferInit(RingBuffer * buf, unsigned short size) {
	buf->data = (unsigned char*)malloc(sizeof(unsigned char)*size);
	
	buf->start = 0;
	buf->end = 0;
	buf->count = 0;
	buf->size = size;
}

void RingBufferDelete(RingBuffer * buf) {
	free(buf->data);
	
	buf->data = 0;
	buf->start = 0;
	buf->end = 0;
}

unsigned short RingBufferAdd(RingBuffer * buf, unsigned char * data, unsigned short size) {
	unsigned short i;
	// TODO When I'm more awake: Don't use a stupid loop. Use memcpy.
	for (i=0;i< size; i++) {
		if (!RingBufferAddByte(buf, data[i]))
			break;
	}
	
	return i;
}

bool RingBufferAddByte(RingBuffer * buf, unsigned char data) {
	if (buf->end+1 % buf->size != buf->start) {
		buf->count++;
		buf->data[buf->end] = data;
		
		buf->end = (buf->end+1) % buf->size;
		return true;
	}
	return false; // buffer full, failed to add
}

unsigned char RingBufferGetByte(RingBuffer * buf) {
	if (buf->end!=buf->start) {
		unsigned char dat = buf->data[buf->start];
		buf->start = (buf->start+1) % buf->size;
		buf->count--;
		return dat;
	}
	
	return 0;
}

unsigned short RingBufferGetData(RingBuffer * buf, unsigned char * dest_buf, unsigned short bytes) {
	unsigned short i, toRead;
	// TODO When I'm more awake: Don't use a stupid loop. Use memcpy.
	
	if (RingBufferBytesUsed(buf) < bytes) {
		toRead = RingBufferBytesUsed(buf);
	}
	else {
		toRead = bytes;
	}
	
	for (i=0;i<toRead;i++) {
		dest_buf[i] = RingBufferGetByte(buf);
	}
	
	return toRead; 
}

unsigned short RingBufferBytesUsed(RingBuffer * buf) {
	return buf->count;
}

unsigned short RingBufferBytesFree(RingBuffer * buf) {
	return buf->size - buf->count;
}

