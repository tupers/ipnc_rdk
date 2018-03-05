
CFLAGS +=  -I$(PUBLIC_INCLUDE_DIR) -O3 -Wall -static
TARGETS = dummy_server
LIBS = -lio
CC = $(BUILD_TOOL_PREFIX)gcc
all:$(TARGETS)
		

%.o:%.c
	$(CC) $(CFLAGS) -g -c $^

install:
	cp -f $(TARGETS) $(EXEC_DIR)
	
dummy_server: main.o $(APP_LIB_DIR)/msg_util.a
	$(CC) $(LDFLAGS) -o $@ $^ 

	
clean:
	rm -f *.o *~
	rm -f $(TARGETS)
