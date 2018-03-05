############################################################
# File: Makefile
#
############################################################

INSTALLDIR = $(EXEC_DIR)

CC = $(BUILD_TOOL_PREFIX)gcc -lpthread
CFLAGS += -Wall -O3 -I$(PUBLIC_INCLUDE_DIR) 
LDFLAGS += -s

BIN  = Ezftp
OBJS = ftp.o fileops.o ftp_thr.o net_config.o
LIBS = $(APP_LIB_DIR)/msg_util.a \
       $(APP_LIB_DIR)/sem_util.a

all: $(BIN)
	cp $(BIN) $(EXEC_DIR)
	rm *.o
$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) $(LDLIBS)

install: all
	cp $(BIN) $(INSTALLDIR)

uninstall:
	rm -rf $(INSTALLDIR)/$(BIN)

clean: 
	rm -f $(BIN) $(BIN).{elf,gdb} $(OBJS)
