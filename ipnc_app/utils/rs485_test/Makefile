
CFLAGS +=  -I$(PUBLIC_INCLUDE_DIR) -O3 -Wall -static

TARGETS = rs485_test

LIBS =
CC = $(BUILD_TOOL_PREFIX)gcc
all:$(TARGETS)
		

%.o:%.c
	$(CC) $(CFLAGS) -g -c $^

install:
	cp -f $(TARGETS) $(EXEC_DIR)
	
rs485_test: main.o
	$(CC) $(LDFLAGS) -o $@ $^ 

	
clean:
	rm -f *.o *~
	rm -f $(TARGETS)
