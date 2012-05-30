COMMON_DIR = ../../..
include $(COMMON_DIR)/Makefile

default: build-serial

USART.o-$(SYSTEM): USART.h USART.c
	$(CC) $(INCLUDES) $(CFLAGS) -c USART.c -o USART.o-$(SYSTEM)

libUSART.a-$(SYSTEM): ringbuffer USART.o-$(SYSTEM)
	$(AR) rcs libUSART.a-$(SYSTEM) USART.o-$(SYSTEM) $(RINGBUFFER_OBJS)

build-serial: libUSART.a-$(SYSTEM)
	
clean:
	rm -f USART.o-$(SYSTEM) libUSART.a-$(SYSTEM)

