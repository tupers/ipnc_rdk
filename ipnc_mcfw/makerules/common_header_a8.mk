# (c) Texas Instruments

ifndef $(COMMON_HEADER_MK)
COMMON_HEADER_MK = 1


CC=$(BUILD_TOOL_PREFIX)gcc
AR=$(BUILD_TOOL_PREFIX)ar
LD=$(BUILD_TOOL_PREFIX)gcc

LIB_BASE_DIR=$(MCFW_ROOT_PATH)/build/lib
OBJ_BASE_DIR=$(MCFW_ROOT_PATH)/build/lib/obj
EXE_BASE_DIR=$(TARGET_MCFW_DIR)/bin

ifeq ($(CONFIG),)
LIB_DIR=$(LIB_BASE_DIR)
else
LIB_DIR=$(LIB_BASE_DIR)/$(CONFIG)
endif

CC_OPTS=-c -Wall

ifeq ($(TREAT_WARNINGS_AS_ERROR), yes)
CC_OPTS+= -Werror
endif

ifeq ($(CONFIG), debug)

CC_OPTS+=-g
OPTI_OPTS=
DEFINE=-DDEBUG

else

CC_OPTS+=
OPTI_OPTS=-O3
DEFINE=

endif

AR_OPTS=-rc
LD_OPTS=-lpthread -lm

DEFINE += $(IPNC_RDK_CFLAGS)
FILES=$(subst ./, , $(foreach dir,.,$(wildcard $(dir)/*.c)) ) 

vpath %.a $(LIB_DIR)

include $(MCFW_ROOT_PATH)/makerules/includes_a8.mk

INCLUDE=
INCLUDE+=$(KERNEL_INC)
INCLUDE+=$(COMMON_INC)
INCLUDE+=$(IPNC_APP_INC)

endif # ifndef $(COMMON_HEADER_MK)

