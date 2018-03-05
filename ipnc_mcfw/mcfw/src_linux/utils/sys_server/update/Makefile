# Makefile
#

TARGET = updateCopy

C_FLAGS += -Wall -O3

CPP_FLAGS += -I$(PUBLIC_INCLUDE_DIR) \
		-I../inc \
		-D_GNU_SOURCE \
		-DAVI_BIG_FILE \
		-D_LARGEFILE64_SOURCE \
		-D_FILE_OFFSET_BITS=64 \
                $(XDC_FLAGS)

LD_FLAGS += -lpthread -s

COMPILE.c = $(BUILD_TOOL_PREFIX)gcc $(C_FLAGS) $(CPP_FLAGS) -c
LINK.c = $(BUILD_TOOL_PREFIX)gcc $(LD_FLAGS)

RELTARGET = release/$(TARGET)

RELCFLAGS = -fno-strict-aliasing

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h) $(PUBLIC_INCLUDE_DIR)/*.h

RELOBJFILES = $(SOURCES:%.c=release/%.o)

RELLDFLAGS =

.PHONY: clean release install

all:    release

install: release
	install -d $(EXEC_DIR)
	install $(RELTARGET) $(EXEC_DIR)
#	ln -sf /tmp/localtime $(TARGET_FS)/usr/share/zoneinfo/localtime
#	ln -sf /tmp/localtime $(TARGET_FS)/usr/share/zoneinfo/posixrules
#	ln -sf /tmp/localtime $(TARGET_FS)/etc/localtime

release:    $(RELTARGET)
$(RELTARGET):   $(RELOBJFILES) 
		$(LINK.c) -o $@ $^ $(RELLDFLAGS)


$(RELOBJFILES): release/%.o: %.c $(HEADERS)
	@mkdir -p release
	$(COMPILE.c) $(RELCFLAGS) -o $@ $<

clean:
	-$(RM) -rf release  *.d
