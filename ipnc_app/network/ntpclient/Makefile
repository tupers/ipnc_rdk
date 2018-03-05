# A long time ago, far, far away, under Solaris, you needed to
#    CFLAGS += -xO2 -Xc
#    LDLIBS += -lnsl -lsocket
# To cross-compile
#    CC = arm-linux-gcc
# To check for lint
#    CFLAGS += -Wpointer-arith -Wcast-align -Wcast-qual -Wshadow -Wundef \
#     -Waggregate-return -Wnested-externs -Winline -Wwrite-strings -Wstrict-prototypes

CC = $(BUILD_TOOL_PREFIX)gcc
	
EXEC = ntpclient

OBJS = ntpclient.o phaselock.o
INSTALLDIR = $(EXEC_DIR)

# This is old-school networking code, making the traditional cast between
# struct sockaddr* and struct sockaddr_in*.  Thus a modern gcc needs:
CFLAGS += -fno-strict-aliasing

CFLAGS += -std=c89
CFLAGS += -W -Wall
CFLAGS += -O3
# CFLAGS += -DPRECISION_SIOCGSTAMP
CFLAGS += -DENABLE_DEBUG
CFLAGS += -DENABLE_REPLAY
# CFLAGS += -DUSE_OBSOLETE_GETTIMEOFDAY

LDFLAGS += -lrt -s

all: $(EXEC)

$(EXEC): $(OBJS)

$(OBJS): ntpclient.h

adjtimex: adjtimex.o

install: $(EXEC)
	install -d $(INSTALLDIR)
	install $(EXEC) $(INSTALLDIR)

clean:
	rm -f $(EXEC) adjtimex *.o
