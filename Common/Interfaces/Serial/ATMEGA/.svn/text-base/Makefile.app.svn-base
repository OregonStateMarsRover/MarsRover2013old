# Compiles USART-test.c test app

SYSTEM     = atmega48
COMMON_DIR = ../../..
OBJECTS    = USART-test.o
PROJECT    = USART-test

default: app

include $(COMMON_DIR)/Makefile.app

USART-test.o: USART-test.c
	$(CC) $(INCLUDES) $(CFLAGS) -c USART-test.c

app: serial USART-test.o
	$(CC) $(LDFLAGS) $(OBJECTS) $(SERIAL_LIB) $(LIBDIRS) $(LIBS) -o $(TARGET)

clean:
	rm -f USART-test.o
	rm -f $(PROJECT).map $(PROJECT).elf $(PROJECT).hex $(PROJECT).eep
