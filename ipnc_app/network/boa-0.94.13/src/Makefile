# Generated automatically from Makefile.in by configure.
# $Id: Makefile.in,v 1.59 2002/03/24 22:20:19 jnelson Exp $

.SUFFIXES:
.SUFFIXES: .o .c
.PHONY: clean mrclean distclean depend all dist

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

srcdir = .
VPATH = .:./../extras
LDFLAGS = -g -lpthread -lssl -lcrypto -s
ONVIF_DIR = $(IPNC_DIR)/network/onvif
ONVIF_INC_DIR = $(ONVIF_DIR)/inc
SOAP_INC_DIR = $(ONVIF_DIR)/soap_generated_files
ONVIF_LIB_DIR = $(ONVIF_DIR)/libs

LIBS = $(APP_LIB_DIR)/msg_util.a $(APP_LIB_DIR)/Appro_interface.a \
       $(APP_LIB_DIR)/file_msg_drv.a $(APP_LIB_DIR)/sysctrl.a \
       $(APP_LIB_DIR)/sys_msg_drv.a $(CMEM_LIB_DIR)/cmem.a470MV $(ONVIF_LIB_DIR)/libonvif.a

CFLAGS = -O3 -pipe -Wall 
ARFLAGS = rcs
ONVIF1 = -DWITH_COOKIES -DWITH_ZLIB 
ONVIF2 =  -lz

ifneq ($(IPNC_DEVICE), DM8127)
CFLAGS += -DIPNC_DM385
LDFLAGS += -DIPNC_DM385
endif

ifeq ($(IPNC_CONFIG), LOW_POWER)
CFLAGS += -DIPNC_LOW_POWER
endif

ifeq ($(CAMERA_TILER_ENABLE), YES)
CFLAGS += -DROTATE_ON
endif

CFLAGS += -I. -I$(PUBLIC_INCLUDE_DIR) -I$(ONVIF_INC_DIR) -I$(SOAP_INC_DIR) -I$(ONVIF_DIR)/gsoap/plugin

# Change these if necessary
YACC = bison -y 
LEX = flex 
CC = $(BUILD_TOOL_PREFIX)gcc 
CPP = $(BUILD_TOOL_PREFIX)gcc -E
AR = $(BUILD_TOOL_PREFIX)ar 

SOURCES = alias.c boa.c buffer.c cgi.c cgi_header.c config.c escape.c \
	get.c hash.c ip.c log.c mmap_cache.c pipe.c queue.c read.c \
	request.c response.c select.c signals.c util.c sublog.c appro_api.c \
	net_config.c web_translate.c file_list.c auth.c para_list.c syslog_list.c accesslog_list.c actvcon_list.c tree.c psia_root.c \
	index.c description.c xadd.c xparse.c iface.c \

OBJS = y.tab.o lex.yy.o $(SOURCES:.c=.o) timestamp.o


all:	boa boa_indexer

install: boa
	install boa $(INSTALLDIR)

boa:	$(OBJS) $(LIBS)
	$(CC) $(ONVIF1) -o $@ $^ $(LDFLAGS) $(CFLAGS)
	
boa_indexer:	index_dir.o escape.o $(LIBS)
	$(CC) $(ONVIF1) -o $@ $^ $(LDFLAGS) $(CFLAGS)

clean:
	rm -f $(OBJS) boa core lex.yy.c y.tab.c y.tab.h *~ boa_indexer index_dir.o $(ONVIF_LIB)

distclean:	mrclean

mrclean:	clean
	rm -f config.status config.cache config.h Makefile config.log

# parser dependencies

y.tab.c y.tab.h:	boa_grammar.y
	$(YACC) -d $^

lex.yy.c:	boa_lexer.l
	$(LEX) $^

# timestamp

timestamp.o:	$(SOURCES) boa_grammar.y boa_lexer.l

# depend stuff
.depend:
	$(CPP) -MM $(SOURCES) > .depend
        
depend:
	-rm -f .depend
	$(MAKE) .depend
        
include .depend

# tags
tags:	$(SOURCES)
	ctags -o tags $^ *.h

# dist
dist:
	$(MAKE) clean
	./makedist.sh
        
# object dump
boa.objdump:    boa
	objdump --disassemble-all --source boa > $@

