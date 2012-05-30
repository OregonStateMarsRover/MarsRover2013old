SYSTEM     = x86
COMMON_DIR = ..
OBJECTS    = CommInterfaceTest.o
PROJECT    = CommInterfaceTest

default: app

include $(COMMON_DIR)/Makefile.app

CommInterfaceTest.o: CommInterfaceTest.c
	$(CC) $(INCLUDES) $(CFLAGS) -c CommInterfaceTest.c

app: comm CommInterfaceTest.o
	$(CC) $(LDFLAGS) $(OBJECTS) $(COMM_LIB) $(SERIAL_LIB) $(LIBDIRS) $(LIBS) -o $(TARGET)

clean:
	rm -f CommInterfaceTest.o
	rm -f $(PROJECT).map $(PROJECT).elf $(PROJECT).hex $(PROJECT).eep
