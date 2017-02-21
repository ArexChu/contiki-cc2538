all: ip64-router
CONTIKI=../../..
#DEFINES+=ROUTER=\"ip64-router.h\"
ROUTER = 1
ifdef ROUTER
MODULES += core/net/ip64 dev/enc28j60/ 

MODULES += apps/ip64-addr apps/mdns
CONTIKI_TARGET_SOURCEFILES+=enc28j60-arch.c
endif
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
include $(CONTIKI)/Makefile.include

