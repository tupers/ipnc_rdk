# Makefile
#

TARGET = system_server

C_FLAGS += -Wall -O3

CPP_FLAGS += -I$(PUBLIC_INCLUDE_DIR) \
		-I../inc \
		-D_GNU_SOURCE \
		-DAVI_BIG_FILE \
		-D_LARGEFILE64_SOURCE \
		-D_FILE_OFFSET_BITS=64 \
                $(XDC_FLAGS)

ifeq ($(SYSTEM_CFG),ipnc)
C_FLAGS += -DIPNC_GPIO_CFG
endif

ifeq ($(AES_MODULE_MODE), ON)
CPP_FLAGS += -DENCRYPT_ON
endif

ifneq ($(IPNC_DEVICE),DM8127)
C_FLAGS += -DIPNC_DM385
endif

ifeq ($(IPNC_CONFIG), LOW_POWER)
C_FLAGS += -DIPNC_LOW_POWER
endif

ifneq ($(POWER_OPT_DSP_OFF), YES)
C_FLAGS += -DIPNC_DSP_ON
endif

ifeq ($(TEARDOWN_LOAD_UNLOAD), YES)
C_FLAGS += -DTEARDOWN_LOAD_UNLOAD
endif

ifeq ($(RVM_DEMO_MODE), YES)
C_FLAGS += -DRVM_DEMO_MODE
endif

ifeq ($(MEMORY_256MB), YES)
C_FLAGS += -DMEMORY_256MB
endif

ifeq ($(WDR_ON), YES)
C_FLAGS += -D_ONLY_TI2A
C_FLAGS += -DWDR_ON_MODE
endif

ifeq ($(SENSOR_BOARD_VENDOR), LIMG)
C_FLAGS += -D_ONLY_TI2A
endif

ifeq ($(SENSOR_BOARD_VENDOR), APPRO)
ifeq ($(IMGS_ID), IMGS_SONY_IMX136)
C_FLAGS += -D_ONLY_APPRO2A
endif
endif

ifeq ($(IPNC_DEVICE), DM388)
C_FLAGS += -DUSE_MCTNF
endif

ifeq ($(DMVA3_VNF_MODE), MCTNF)
C_FLAGS += -DUSE_MCTNF
endif

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
$(RELTARGET):   $(RELOBJFILES) $(APP_LIB_DIR)/file_msg_drv.a $(APP_LIB_DIR)/Appro_interface.a \
		$(APP_LIB_DIR)/alarm_msg_drv.a $(APP_LIB_DIR)/ipnc_gio_util.a $(CMEM_LIB_DIR)/cmem.a470MV $(APP_LIB_DIR)/netserver_msg_drv.a $(APP_LIB_DIR)/remote_debug_msg_drv.a 
		$(LINK.c) -o $@ $^ $(RELLDFLAGS)


$(RELOBJFILES): release/%.o: %.c $(HEADERS)
	@mkdir -p release
	$(COMPILE.c) $(RELCFLAGS) -o $@ $<

clean:
	-$(RM) -rf release  *.d
