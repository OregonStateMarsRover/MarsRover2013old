include $(COMMON_DIR)/buildspecs/Makefile.$(SYSTEM)

CFLAGS += -I$(COMMON_DIR)/..

default:

ringbuffer:
	@# Link Variable: $(RINGBUFFER_OBJS) - provides a .o file
	cd $(COMMON_DIR)/RingBuffer ; make SYSTEM=$(SYSTEM)

timer:
	@# Link Variable: $(TIMER_OBJS) - provides a .o file
	cd $(COMMON_DIR)/Timer/$(AVRTYPE) ; make SYSTEM=$(SYSTEM)
	
serial:
	@# Link Variable: $(SERIAL_LIB) - provides a .a file including all serial's dependencies
	cd $(COMMON_DIR)/Interfaces/Serial/$(AVRTYPE); make SYSTEM=$(SYSTEM)

comm:
	@# Link Variable: $(COMM_LIB) - provides a .a file including all of comm's dependencies
	cd $(COMMON_DIR)/CommInterface ; make SYSTEM=$(SYSTEM)
