
CFLAGS +=  -I$(PUBLIC_INCLUDE_DIR) -I. -O3 -Wall -static

TARGETS = filesys_update
LIBS =
CC = $(BUILD_TOOL_PREFIX)gcc
all:$(TARGETS)
		

%.o:%.c
	$(CC) $(CFLAGS) -g -c $^

install:
	cp -f $(TARGETS) $(EXEC_DIR)
	
filesys_update: main.o
	$(CC) $(LDFLAGS) -o $@ $^ 

	
clean:
	rm -f *.o *~
	rm -f $(TARGETS)
