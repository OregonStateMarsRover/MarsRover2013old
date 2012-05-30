SYSTEM     = xmega
COMMON_DIR = ../Common
PROJECT    = Rover

OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c)) $(patsubst %.c,%.o,$(wildcard */*.c))

default: rover

include $(COMMON_DIR)/Makefile.app

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c  $< -o $@

rover: comm serial timer $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(COMM_LIB) $(SERIAL_LIB) $(TIMER_OBJS) $(LIBDIRS) $(LIBS) -o $(TARGET)

clean:
	rm -f $(OBJECTS)
	rm -f $(PROJECT).map $(PROJECT).elf $(PROJECT).hex $(PROJECT).eep
