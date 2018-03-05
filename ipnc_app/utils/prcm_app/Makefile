
CFLAGS += -Wall -static

ifeq ($(POWER_OPT_DSP_OFF), YES)
CFLAGS += -DDSP_OFF
endif
ifeq ($(POWER_OPT_DSS_OFF), YES)
CFLAGS += -DDSS_OFF
endif
ifneq ($(IPNC_DEVICE), DM8127)
CFLAGS += -DDM385_CONFIG
endif

TARGETS = linux_prcm_ipcam
CC = $(BUILD_TOOL_PREFIX)gcc
all:$(TARGETS)
		
%.o:%.c
	$(CC) $(CFLAGS) -g -D__DEBUG -c $^

install:
	cp -f $(TARGETS) $(EXEC_DIR)
	
linux_prcm_ipcam: linux_prcm_ipcam.o
	$(CC) $(LDFLAGS) -o $@ $^

	
clean:
	rm -f *.o *~
	rm -f $(TARGETS)
