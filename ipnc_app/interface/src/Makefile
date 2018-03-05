
INCLUDES += -I./ -I$(PUBLIC_INCLUDE_DIR) -I$(LINUXKERNEL_INSTALL_DIR)/include -I$(CMEM_INC_DIR)

C_FLAGS += -Wall -O3 -lpthread
AR_FLAGS += -r

CC = $(BUILD_TOOL_PREFIX)gcc $(INCLUDES) $(C_FLAGS) -c
AR = $(BUILD_TOOL_PREFIX)ar

ifeq ($(SYSTEM_CFG),ipnc)
C_FLAGS += -DIPNC_GPIO_CFG
endif

ifneq ($(IPNC_DEVICE),DM8127)
C_FLAGS += -DIPNC_DM385
endif

ifeq ($(IPNC_CONFIG), LOW_POWER)
C_FLAGS += -DIPNC_LOW_POWER
endif

REL_EXE1 = reboot_ipnc

REL_LIB1 = file_msg_drv.a

REL_LIB2 = alarm_msg_drv.a

REL_LIB3 = share_mem.a

REL_LIB4 = sys_msg_drv.a

REL_LIB5 = msg_util.a

REL_LIB7 = Appro_interface.a

REL_LIB8 = ipnc_gio_util.a

REL_LIB9 = sem_util.a

REL_LIB10 = sysctrl.a

REL_LIB11 = netserver_msg_drv.a 

REL_LIB12 = remote_debug_msg_drv.a

REL_OBJ1 = file_msg_drv.o share_mem.o msg_util.o Frame_rate_ctrl.o

REL_OBJ2 = alarm_msg_drv.o msg_util.o

REL_OBJ3 = share_mem.o

REL_OBJ4 = sys_msg_drv.o share_mem.o msg_util.o

REL_OBJ5 = msg_util.o

REL_OBJ7 = ApproDrvMsg.o Appro_interface.o sem_util.o msg_util.o  

REL_OBJ8 = ipnc_gio_util.o

REL_OBJ9 = sem_util.o 

REL_OBJ10 = sysctrl.o sys_msg_drv.o share_mem.o msg_util.o sem_util.o

REL_OBJ11 = netserver_msg_drv.o  msg_util.o sem_util.o 

REL_OBJ12 = remote_debug_msg_drv.o socket_util.o 

INTERFACE_LIBS = $(REL_LIB9) $(REL_LIB3) $(REL_LIB1) $(REL_LIB2) $(REL_LIB4) $(REL_LIB5) $(REL_LIB6) $(REL_LIB7) $(REL_LIB8) $(REL_LIB10) $(REL_LIB11) $(REL_LIB12)  

INSTALL_LIBS := $(foreach libs,$(INTERFACE_LIBS),$(APP_LIB_DIR)/$(libs))

INTERFACE_EXES = $(REL_EXE1)

all: $(INTERFACE_LIBS) $(INTERFACE_EXES)
	if [ ! -d $(APP_LIB_DIR) ]; then \
		if [ -e $(APP_LIB_DIR) ]; then \
			$(RM) -f $(APP_LIB_DIR); \
		fi; \
		mkdir $(APP_LIB_DIR); \
	fi
	$(MAKE) install

$(REL_LIB1): $(REL_OBJ1)
	$(AR) $(AR_FLAGS) $(REL_LIB1) $(REL_OBJ1)

$(REL_LIB2): $(REL_OBJ2)
	$(AR) $(AR_FLAGS) $(REL_LIB2) $(REL_OBJ2)

$(REL_LIB3): $(REL_OBJ3)
	$(AR) $(AR_FLAGS) $(REL_LIB3) $(REL_OBJ3)

$(REL_LIB4): $(REL_OBJ4)
	$(AR) $(AR_FLAGS) $(REL_LIB4) $(REL_OBJ4)

$(REL_LIB5): $(REL_OBJ5)
	$(AR) $(AR_FLAGS) $(REL_LIB5) $(REL_OBJ5)
	
$(REL_LIB7): $(REL_OBJ7) 
	$(AR) $(AR_FLAGS) $(REL_LIB7) $(REL_OBJ7)
	
$(REL_LIB8): $(REL_OBJ8) 
	$(AR) $(AR_FLAGS) $(REL_LIB8) $(REL_OBJ8)

$(REL_LIB9): $(REL_OBJ9) 
	$(AR) $(AR_FLAGS) $(REL_LIB9) $(REL_OBJ9)

$(REL_LIB10): $(REL_OBJ10) 
	$(AR) $(AR_FLAGS) $(REL_LIB10) $(REL_OBJ10)

$(REL_LIB11): $(REL_OBJ11) 
	$(AR) $(AR_FLAGS) $(REL_LIB11) $(REL_OBJ11)

$(REL_LIB12): $(REL_OBJ12) 
	$(AR) $(AR_FLAGS) $(REL_LIB12) $(REL_OBJ12)
	
Frame_rate_ctrl.o : Frame_rate_ctrl.c $(PUBLIC_INCLUDE_DIR)/Frame_rate_ctrl.h
	$(CC) $(C_FLAGS) -o $@ $<
file_msg_drv.o : file_msg_drv.c $(PUBLIC_INCLUDE_DIR)/file_msg_drv.h $(PUBLIC_INCLUDE_DIR)/Msg_Def.h\
		$(PUBLIC_INCLUDE_DIR)/sys_env_type.h
	$(CC) $(C_FLAGS) -o $@ $<

alarm_msg_drv.o : alarm_msg_drv.c $(PUBLIC_INCLUDE_DIR)/alarm_msg_drv.h \
		$(PUBLIC_INCLUDE_DIR)/Msg_Def.h $(PUBLIC_INCLUDE_DIR)/sys_env_type.h
	$(CC) $(C_FLAGS) -o $@ $<

share_mem.o: share_mem.c $(PUBLIC_INCLUDE_DIR)/share_mem.h
	$(CC) $(C_FLAGS) -o $@ $<

sys_msg_drv.o : sys_msg_drv.c $(PUBLIC_INCLUDE_DIR)/sys_msg_drv.h $(PUBLIC_INCLUDE_DIR)/Msg_Def.h\
		$(PUBLIC_INCLUDE_DIR)/sys_env_type.h $(PUBLIC_INCLUDE_DIR)/share_mem.h
	$(CC) $(C_FLAGS) -o $@ $<

msg_util.o: msg_util.c $(PUBLIC_INCLUDE_DIR)/Msg_Def.h
	$(CC) $(C_FLAGS) -o $@ $<

ApproDrvMsg.o: ApproDrvMsg.c $(PUBLIC_INCLUDE_DIR)/ApproDrvMsg.h
	$(CC) $(C_FLAGS) -o $@ $<

Appro_interface.o: Appro_interface.c $(PUBLIC_INCLUDE_DIR)/Appro_interface.h
	$(CC) $(C_FLAGS) -o $@ $<

sem_util.o: sem_util.c $(PUBLIC_INCLUDE_DIR)/sem_util.h
	$(CC) $(C_FLAGS) -o $@ $<

sysctrl.o: sysctrl.c $(PUBLIC_INCLUDE_DIR)/sysctrl.h $(PUBLIC_INCLUDE_DIR)/sys_env_type.h
	$(CC) $(C_FLAGS) -o $@ $<
	
socket_util.o : socket_util.c $(PUBLIC_INCLUDE_DIR)/socket_util.h $(PUBLIC_INCLUDE_DIR)/sys_env_type.h
	$(CC) $(C_FLAGS) -o $@ $<	
remote_debug_msg_drv.o : remote_debug_msg_drv.c $(PUBLIC_INCLUDE_DIR)/remote_debug_msg_drv.h $(PUBLIC_INCLUDE_DIR)/sys_env_type.h
	$(CC) $(C_FLAGS) -o $@ $<		

reboot_ipnc: 
	$(BUILD_TOOL_PREFIX)gcc -g -Wall -static  -c -o reboot_ipnc.o reboot_ipnc.c
	$(BUILD_TOOL_PREFIX)gcc -o reboot_ipnc reboot_ipnc.o
	
clean:
	-$(RM) -f *.o
	-$(RM) -f *.a
	-$(RM) -f $(REL_EXE1)
ifneq ($(APP_LIB_DIR),)
	-$(RM) -Rf $(APP_LIB_DIR)
endif

$(APP_LIB_DIR)/%.a : %.a
	install $< $(APP_LIB_DIR)
	
install: $(INSTALL_LIBS)
	install $(REL_EXE1) $(EXEC_DIR)
