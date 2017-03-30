DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

CONTIKI_PROJECT = light-control-closed-loop

CONTIKI_TARGET_SOURCEFILES += tsl256x.c
CONTIKI_TARGET_SOURCEFILES += PID.c

all: $(CONTIKI_PROJECT)

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include
