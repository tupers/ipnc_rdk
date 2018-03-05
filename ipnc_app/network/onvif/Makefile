# Generated automatically from Makefile.in by configure.
# $Id: Makefile.in,v 1.59 2002/03/24 22:20:19 jnelson Exp $
	
.SUFFIXES:
.SUFFIXES: .o .c
.PHONY: clean depend all dist

ROOTDIR = $(IPNC_INSTALL_DIR)
#include $(ROOTDIR)/Rules.make

INSTALLDIR = $(EXEC_DIR)

GCC_FLAGS = -Wstrict-prototypes -Wpointer-arith -Wcast-align -Wcast-qual\
  -Wtraditional\
  -Wshadow\
  -Wconversion\
  -Waggregate-return\
  -Wmissing-prototypes\
  -Wnested-externs\
  -Wall \
  -Wundef -Wwrite-strings -Wredundant-decls -Winline

GSOAP_DIR = gsoap
GSOAP_GEN_DIR = soap_generated_files
CFLAGS = -O3 -pipe -Wall -DWITH_DOM -DWITH_OPENSSL -I./inc -I./soap_generated_files/ -I$(GSOAP_DIR)/plugin/ -I$(IPNC_DIR)/network/boa-0.94.13/src -I$(PUBLIC_INCLUDE_DIR)
ARFLAGS = rcs
ONVIF_LIBDIR = ./libs
ONVIF_LIB = $(ONVIF_LIBDIR)/libonvif.a

# Change these if necessary
CC = $(BUILD_TOOL_PREFIX)gcc 
CPP = $(BUILD_TOOL_PREFIX)gcc -E
AR = $(BUILD_TOOL_PREFIX)ar 

SOURCES = $(GSOAP_GEN_DIR)/soapC.c $(GSOAP_GEN_DIR)/soapServer.c $(GSOAP_GEN_DIR)/soapClient.c $(GSOAP_GEN_DIR)/stdsoap2.c $(GSOAP_GEN_DIR)/dom.c \
	  src/onvif.c src/onvifface.c src/onvif_func.c plugins/ws_discover.c plugins/security.c $(GSOAP_DIR)/custom/duration.c \
	  $(GSOAP_DIR)/plugin/dapi.c $(GSOAP_DIR)/plugin/wsaapi.c $(GSOAP_DIR)/plugin/threads.c $(GSOAP_DIR)/plugin/httpda.c \
	  $(GSOAP_DIR)/plugin/wsseapi.c $(GSOAP_DIR)/plugin/mecevp.c $(GSOAP_DIR)/plugin/smdevp.c $(GSOAP_DIR)/plugin/md5evp.c    

OBJS = $(SOURCES:.c=.o) 

all:	$(ONVIF_LIBDIR) $(OBJS)
	$(AR) $(ARFLAGS) $(ONVIF_LIB) $(OBJS) 

$(ONVIF_LIBDIR):
	mkdir $(ONVIF_LIBDIR)

install: $(ONVIF_LIBDIR)/libonvif.a
	install $(ONVIF_LIBDIR)/libonvif.a $(INSTALLDIR)
	
clean:
	rm -rf $(OBJS) $(ONVIF_LIBDIR)/*.a *~ .depend

