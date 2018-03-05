
CFLAGS +=  -Iinclude -I$(PUBLIC_INCLUDE_DIR) -I$(KERNELDIR)/include -I$(KERNELDIR)/arch/arm/include -O3 -Wall -static

TARGETS = flash_eraseall nandwrite nanddump

LIBS =
#LDFLAGS += -L. -lmtd
CC = $(BUILD_TOOL_PREFIX)gcc
USR_BIN = $(TARGET_FS)/usr/bin
USR_SBIN = $(TARGET_FS)/usr/sbin

all:$(TARGETS)
		

%.o:%.c
	$(CC) $(CFLAGS) -g -c $^

install:
	cp -f $(TARGETS) $(USR_BIN)
	cp -f $(TARGETS) $(USR_SBIN)

flash_eraseall: flash_eraseall.o libmtd.a
	$(CC) $(LDFLAGS) -o $@ $^

nandwrite: nandwrite.o libmtd.a
	$(CC) $(LDFLAGS) -o $@ $^

nanddump: nanddump.o libmtd.a
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o *~
	rm -f $(TARGETS)
