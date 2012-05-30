COMMON_DIR = ../..
include $(COMMON_DIR)/Makefile

default: build-timer

Timer.o-$(SYSTEM): Timer.h Timer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c Timer.c -o Timer.o-$(SYSTEM)

build-timer: Timer.o-$(SYSTEM)
	
clean:
	rm Timer.o-$(SYSTEM)

