COMMON_DIR = ..
include $(COMMON_DIR)/Makefile

DEPOBJS += $(COMMON_DIR)/Interfaces/RingBuffer/RingBuffer.o-$(SYSTEM)

default: build-ringbuffer

RingBuffer.o-$(SYSTEM): RingBuffer.c RingBuffer.h
	$(CC) $(INCLUDES) $(CFLAGS) -c RingBuffer.c -o RingBuffer.o-$(SYSTEM)

build-ringbuffer: RingBuffer.o-$(SYSTEM)

clean:
	rm RingBuffer.o-$(SYSTEM)

