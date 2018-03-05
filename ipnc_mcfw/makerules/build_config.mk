# Filename: build_config.mk
#
# Build configuration make file - This file contains all the options that are
#                                 frequently changed
#
# This file changes when:
#     1. format needs to change (eg: COFF/ELF)
#     2. Endian-ness needs to change (little/big)
#     3. global compiler and linker options need to change (one entry for each
#                                                           core)
#     4. Profile needs to change (debug/whole_program_debug/release)
#     5. Build option changes (xdc/make/full)
#

# Global options

# Build - Allowed values = full | xdc | make
#         This "builds" packages based on the setting:
#                 full - build both XDC and make (all the packages/components)
#                 xdc  - build and configure only XDC packages
#                 make - build only make based components and link everything
# NOTE: BUILD need not be defined at all, which case it behaves like it is set
#       to "full". This is to ensure backward compatibility.
BUILD = full

# Endianness : Allowed values = little | big
ENDIAN = little

# Format : Allowed values = COFF | ELF
FORMAT = ELF

# Platform memory: available external memory
PLATFORM_MEM = 1024M

#
# Platform specific
#
ifeq ($(IPNC_DEVICE), DM8127)
CFLAGS_GLOBAL_ti814x-evm = -DTI_814X_BUILD -DPLATFORM_EVM_SI -D_IPNC_HW_PLATFORM_
else
CFLAGS_GLOBAL_ti810x-evm = -DTI_8107_BUILD -DPLATFORM_EVM_SI -D_IPNC_HW_PLATFORM_ -DIPNC_DM385
endif

ifeq ($(SYSTEM_CFG),evm)
CFLAGS_GLOBAL_ti810x-evm += -D_IPNC_HW_PLATFORM_EVM_
endif
#
# Core specific options - Cores of ti814x (Centaurus)
#

# m3video - Ducati - Core 0 (Cortex-M3)

# Profile: Allowed values = debug | whole_program_debug | release
CFLAGS_GLOBAL_m3video =  -ms --gcc -D___DSPBIOS___ -D___DUCATI_FW___ -DSYSLINK_USE_TILER -DMULTICHANNEL_OPT=1 -D$(IMGS_ID) -D$(SENSOR_DATA_OP)
LNKFLAGS_GLOBAL_m3video = -x --zero_init=off --retain=_Ipc_ResetVector

# m3vpss - Ducati - Core 1 (Cortex-M3)

# Profile: Allowed values = debug | whole_program_debug | release
CFLAGS_GLOBAL_m3vpss =  -ms --gcc -D___DSPBIOS___ -D___DUCATI_FW___ -DSYSLINK_USE_TILER -DMULTICHANNEL_OPT=1 -D$(IMGS_ID) -D$(SENSOR_DATA_OP)
LNKFLAGS_GLOBAL_m3vpss = -x --zero_init=off --retain=_Ipc_ResetVector

# a8host - Cortex-A8

# Profile: Allowed values = debug
PROFILE_a8host = debug
CFLAGS_GLOBAL_a8host = -c -x c -Wunused -Wall -g
LNKFLAGS_GLOBAL_a8host =

# c6xdsp - DSP core
# Profile: Allowed values = debug | whole_program_debug | release
CFLAGS_GLOBAL_c6xdsp =  -ms --gcc -D___DSPBIOS___ 
LNKFLAGS_GLOBAL_c6xdsp = -x --zero_init=off --retain=_Ipc_ResetVector

# Nothing beyond this point
