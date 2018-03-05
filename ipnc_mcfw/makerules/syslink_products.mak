#
#   Copyright (c) 2008-2012, Texas Instruments Incorporated
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#
#   *  Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#   *  Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#
#   *  Neither the name of Texas Instruments Incorporated nor the names of
#      its contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#


SYSLINK_INSTALL_DIR = $(syslink_PATH)

# List of supported devices (choose one): OMAP3530, OMAPL1XX, TI816X, TI814X
#    TI813X, TI811X
#
#DEVICE = 

# Master core (GPP) OS type (choose one): Linux, Qnx, Bios
#
GPPOS = Linux

# SysLink Linux driver DEBUG/TRACE options (choose one): 0 or 1
#
# Note that these options are only used by the Linux HLOS driver.  RTOS
# side libraries, Qnx libraries, and user mode Linux libraries are always
# built in both debug and release profiles, and users can switch between
# them when linking their executable (without rebuilding SysLink).
#
# Enabling SYSLINK_BUILD_DEBUG enables runtime assertion checking and makes
# some internal 'static' methods and objects global (so you can set
# breakpoints on them and/or inspect them in a debugger).
#
# Enabling SYSLINK_TRACE_ENABLE enables tracing throughout the SysLink
# code base.  This is especially helpful during development, but may not
# be necessary at production.
#
USE_SYSLINK_NOTIFY=0
SYSLINK_BUILD_DEBUG=0
SYSLINK_TRACE_ENABLE=1

# SysLink OPTIMIZE option (choose one): 0 or 1
#
# Enabling SYSLINK_BUILD_OPTIMIZE removes many runtime API checks.  As a
# result, if the user uses SysLink in an unsupported way, rather than getting
# a failing return value, the system may crash or become unstable.
# SYSLINK_BUILD_OPTIMIZED is intended to be used in a closed system where all
# code paths are known, and error conditions are ensured never to occur - so
# it's not for everyone.
#
# Note that SYSLINK_BUILD_OPTIMIZE removes these checks throughout the
# SysLink stack - that includes RTOS, user, and driver layers if applicable
# to your environment.
#
SYSLINK_BUILD_OPTIMIZE=1

# SysLink HLOS driver Notify options (choose one): NOTIFYDRIVERSHM,
#    NOTIFYDRIVERCIRC
#
SYSLINK_NOTIFYDRIVER=NOTIFYDRIVERSHM

# SysLink HLOS driver MessageQ Transport options (choose one): TRANSPORTSHM,
#    TRANSPORTSHMNOTIFY, TRANSPORTSHMCIRC
#
SYSLINK_TRANSPORT=TRANSPORTSHM

# Set SDK type when building for a TI SDK kit (choose one): EZSDK or NONE
#
SDK = NONE

# Define root dir to install SysLink driver and samples for target file-system
#
EXEC_DIR = _your_filesys_

# Define file format for loader and slave executables (choose one): COFF, ELF
#
LOADER = ELF

# Optional: recommended to install all dependent components in one folder.
#
DEPOT = _your_depot_folder_

# Define the product variables for the device you will be using.
#


LINUXKERNEL              = $(KERNELDIR)
CGT_ARM_INSTALL_DIR      = $(CODEGEN_PATH_A8)
CGT_ARM_PREFIX           = $(BUILD_TOOL_PREFIX)
IPC_INSTALL_DIR          = $(ipc_PATH)
BIOS_INSTALL_DIR         = $(bios_PATH)
XDC_INSTALL_DIR          = $(xdc_PATH)
CGT_C674_ELF_INSTALL_DIR = $(CODEGEN_PATH_DSP)
CGT_M3_ELF_INSTALL_DIR   = $(CODEGEN_PATH_M3)

######## For TI816X device ########
ifeq ("$(DEVICE)","TI816X")

# SYS/BIOS timer frequency (ti.sysbios.timers.dmtimer.Timer.intFreq)
TI81XXDSP_DMTIMER_FREQ  = 32768

######## For TI814X device ########
else ifeq ("$(DEVICE)","TI814X")

# SYS/BIOS timer frequency (ti.sysbios.timers.dmtimer.Timer.intFreq)
TI81XXDSP_DMTIMER_FREQ  = 20000000

######## For TI813X device ########
else ifeq ("$(DEVICE)","TI813X")


######## End of device specific variables ########
else ifeq ($(MAKECMDGOALS), clean)
else ifeq ($(MAKECMDGOALS), clobber)
else ifeq ($(MAKECMDGOALS), .show-products)
else ifeq ($(MAKECMDGOALS), help)
else
    $(error DEVICE is set to "$(DEVICE)", which is invalid. Set this in <SysLink Install>/products.mak. Refer to the SysLink Install Guide for more information)
endif

# Use this goal to print your product variables.
.show-products:
	@echo "DEPOT                    = $(DEPOT)"
	@echo "DEVICE                   = $(DEVICE)"
	@echo "GPPOS                    = $(GPPOS)"
	@echo "SDK                      = $(SDK)"
	@echo "TI81XXDSP_DMTIMER_FREQ   = $(TI81XXDSP_DMTIMER_FREQ)"
	@echo "SYSLINK_BUILD_DEBUG      = $(SYSLINK_BUILD_DEBUG)"
	@echo "SYSLINK_BUILD_OPTIMIZE   = $(SYSLINK_BUILD_OPTIMIZE)"
	@echo "SYSLINK_TRACE_ENABLE     = $(SYSLINK_TRACE_ENABLE)"
	@echo "LOADER                   = $(LOADER)"
	@echo "SYSLINK_INSTALL_DIR      = $(SYSLINK_INSTALL_DIR)"
	@echo "IPC_INSTALL_DIR          = $(IPC_INSTALL_DIR)"
	@echo "BIOS_INSTALL_DIR         = $(BIOS_INSTALL_DIR)"
	@echo "XDC_INSTALL_DIR          = $(XDC_INSTALL_DIR)"
	@echo "LINUXKERNEL              = $(LINUXKERNEL)"
	@echo "QNX_INSTALL_DIR          = $(QNX_INSTALL_DIR)"
	@echo "CGT_ARM_PREFIX           = $(CGT_ARM_PREFIX)"
	@echo "CGT_C64P_INSTALL_DIR     = $(CGT_C64P_INSTALL_DIR)"
	@echo "CGT_C64P_ELF_INSTALL_DIR = $(CGT_C64P_INSTALL_DIR)"
	@echo "CGT_C674_INSTALL_DIR     = $(CGT_C674_INSTALL_DIR)"
	@echo "CGT_C674_ELF_INSTALL_DIR = $(CGT_C674_ELF_INSTALL_DIR)"
	@echo "CGT_M3_ELF_INSTALL_DIR   = $(CGT_M3_ELF_INSTALL_DIR)"
	@echo "CGT_A8_ELF_INSTALL_DIR   = $(CGT_A8_ELF_INSTALL_DIR)"
	@echo "EXEC_DIR                 = $(EXEC_DIR)"
