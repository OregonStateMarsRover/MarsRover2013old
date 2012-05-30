COMMON_DIR = ..
include $(COMMON_DIR)/Makefile

default: build-commiface

SerialProtocol.o-$(SYSTEM): SerialProtocol.h SerialProtocol.c
	$(CC) $(INCLUDES) $(CFLAGS) -c SerialProtocol.c -o SerialProtocol.o-$(SYSTEM)
	
CommInterface.o-$(SYSTEM): CommInterface.h CommInterface.c
	$(CC) $(INCLUDES) $(CFLAGS) -c CommInterface.c -o CommInterface.o-$(SYSTEM)

libCommInterface.a-$(SYSTEM): SerialProtocol.o-$(SYSTEM) CommInterface.o-$(SYSTEM)
	$(AR) rcs libCommInterface.a-$(SYSTEM) SerialProtocol.o-$(SYSTEM) CommInterface.o-$(SYSTEM)

build-commiface: libCommInterface.a-$(SYSTEM)
	
clean:
	rm CommInterface.o-$(SYSTEM) SerialProtocol.o-$(SYSTEM) libCommInterface.a-$(SYSTEM)

