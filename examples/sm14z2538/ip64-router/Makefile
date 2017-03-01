all: ip64-router
CONTIKI=../../..
#DEFINES+=ROUTER=\"ip64-router.h\"
MODULES += core/net/ip64 dev/enc28j60/ 

CONTIKI_TARGET_SOURCEFILES+=enc28j60-arch.c
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
include $(CONTIKI)/Makefile.include

