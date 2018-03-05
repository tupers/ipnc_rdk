# Makefile for update
#

CC = $(BUILD_TOOL_PREFIX)gcc
	
EXEC = update.cgi

OBJS = update.o crc32.o
INSTALLDIR = $(EXEC_DIR)
CFLAGS += -Wall -O3 -I ./ -I$(PUBLIC_INCLUDE_DIR)
LDFLAGS += -s

ifeq ($(IPNC_FILE_SYSTEM), JFFS2)
CFLAGS += -DIPNC_JFFS2_ON
endif

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS$(LDLIBS_$@))

install: $(EXEC)
	cp $(EXEC) $(ROOT_FILE_SYS)/var/www/

clean:
	rm -f $(EXEC) *.gdb *.elf *.o
