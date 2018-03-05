
CFLAGS +=  -I$(PUBLIC_INCLUDE_DIR) -O3 -Wall -static

TARGETS = smart_mount

LIBS =
CC = $(BUILD_TOOL_PREFIX)gcc
all:$(TARGETS)
		

%.o:%.c
	$(CC) $(CFLAGS) -g -c $^

install:
	cp -f $(TARGETS) $(EXEC_DIR)
	
smart_mount: main.o
	$(CC) $(LDFLAGS) -o $@ $^ 

	
clean:
	rm -f *.o *~
	rm -f $(TARGETS)
