##############################################################################
# Product: Makefile for QP-lwIP, EK-LM36965, QK, GNU/Sourcery G++
# Last Updated for Version: 4.1.03
# Date of the Last Update:  Mar 11, 2010
#
#                    Q u a n t u m     L e a P s
#                    ---------------------------
#                    innovating embedded systems
#
# Copyright (C) 2002-2010 Quantum Leaps, LLC. All rights reserved.
#
# This software may be distributed and modified under the terms of the GNU
# General Public License version 2 (GPL) as published by the Free Software
# Foundation and appearing in the file GPL.TXT included in the packaging of
# this file. Please note that GPL Section 2[b] requires that all works based
# on this software must also be made publicly available under the terms of
# the GPL ("Copyleft").
#
# Alternatively, this software may be distributed and modified under the
# terms of Quantum Leaps commercial licenses, which expressly supersede
# the GPL and are specifically designed for licensees interested in
# retaining the proprietary status of their code.
#
# Contact information:
# Quantum Leaps Web site:  http://www.quantum-leaps.com
# e-mail:                  info@quantum-leaps.com
##############################################################################
#-----------------------------------------------------------------------------
# tools
#

CC    := $(COMPILER_ROOT)/bin/arm-none-eabi-gcc
CPP   := $(COMPILER_ROOT)/bin/arm-none-eabi-g++
AS    := $(COMPILER_ROOT)/bin/arm-none-eabi-as
LINK  := $(COMPILER_ROOT)/bin/arm-none-eabi-g++                       # for C++ programs
BIN   := $(COMPILER_ROOT)/bin/arm-none-eabi-objcopy
RM    := rm -rf
MKDIR := mkdir

#-----------------------------------------------------------------------------
# directories
#
QP_PORT_DIR := $(QL_PORT_DIR)
CMSIS_DIR   := cmsis
APP_DIR     := .

# source directories
VPATH = $(APP_DIR) \
        $(CMSIS_DIR) \
	    lwip_port/netif \
	    webserver

# Output file basename
OUTPUT    := lwip_qk

# include directories
INCLUDES  = -I$(QL_INCLUDE_DIR)	\
	-I$(QP_PORT_DIR) \
	-I. \
	-I$(CMSIS_DIR) \
	-Ilwip_port \
	-Iwebserver \
	-I$(ViEW_ROOT)/lwip-1.3.2/src/include \
	-I$(ViEW_ROOT)/lwip-1.3.2/src/include/ipv4 \
	-I$(ViEW_ROOT)/lwip-1.3.2/src

# defines
DEFINES =

#-----------------------------------------------------------------------------
# files
#

# assembler source files
ASM_SRCS :=

# C source files
C_SRCS := $(wildcard *.c) \
	startup_lm3s.c \
	core_cm3.c \
	system_lm3s.c \
	eth_driver.c \
	httpd.c \
	fs.c

# C++ source files
CPP_SRCS := $(wildcard *.cpp)


LD_SCRIPT := lm3s6965.ld

#-----------------------------------------------------------------------------
# build options for various configurations
#

ARM_CORE = cortex-m3

ifeq ($(MAKECMDGOALS), rel)     # Release configuration ............................

BIN_DIR := $(OBJ_DIR)/$(OUTPUT)
LIBS    := -lqk -lqf -lqep

ASFLAGS = -mcpu=$(ARM_CORE)

CFLAGS = -mcpu=$(ARM_CORE) -mthumb -Wall \
	-Os $(INCLUDES) $(DEFINES) -DNDEBUG

CPPFLAGS = -mcpu=$(ARM_CORE) -mthumb \
	-Wall -fno-rtti -fno-exceptions \
	-Os $(INCLUDES) $(DEFINES) -DNDEBUG

LINKFLAGS = -T$(LD_SCRIPT) -mcpu=$(ARM_CORE) -mthumb -nostdlib -Xlinker \
	-Map=$(EXE_DIR)/$(OUTPUT).map --gc-sections \
	-L$(LIB_DIR)

else ifeq ($(MAKECMDGOALS), spy) # Spy configuration ................................

BIN_DIR := $(OBJ_DIR)/$(OUTPUT)
LIBS    := -lqk -lqf -lqep -lqs

ASFLAGS = -g -mcpu=$(ARM_CORE)

CFLAGS = -mcpu=$(ARM_CORE) -mthumb -Wall \
	-g -O $(INCLUDES) $(DEFINES) -DQ_SPY

CPPFLAGS = -mcpu=$(ARM_CORE) -mthumb \
	-Wall -fno-rtti -fno-exceptions \
	-g -O $(INCLUDES) $(DEFINES) -DQ_SPY
	
LINKFLAGS = -T$(LD_SCRIPT) -mcpu=$(ARM_CORE) -mthumb -nostdlib -Xlinker \
	-Map=$(EXE_DIR)/$(OUTPUT).map --gc-sections \
	-L$(LIB_DIR)

else                     # default Debug configuration .......................

BIN_DIR := $(OBJ_DIR)/$(OUTPUT)
LIBS    := -lqk -lqf -lqep

ASFLAGS = -g -mcpu=$(ARM_CORE)

CFLAGS = -mcpu=$(ARM_CORE) -mthumb -Wall \
	-g -O $(INCLUDES) $(DEFINES)

CPPFLAGS = -mcpu=$(ARM_CORE) -mthumb \
	-Wall -fno-rtti -fno-exceptions \
	-g -O $(INCLUDES) $(DEFINES)

LINKFLAGS = -T$(LD_SCRIPT) -mcpu=$(ARM_CORE) -mthumb -nostdlib -Xlinker \
	-Map=$(EXE_DIR)/$(OUTPUT).map --gc-sections \
	-L$(LIB_DIR)

endif


ASM_OBJS     := $(patsubst %.s,%.o,$(ASM_SRCS))
C_OBJS       := $(patsubst %.c,%.o,$(C_SRCS))
CPP_OBJS     := $(patsubst %.cpp,%.o,$(CPP_SRCS))

TARGET_BIN   := $(EXE_DIR)/$(OUTPUT).ihex
TARGET_ELF   := $(EXE_DIR)/$(OUTPUT).elf
ASM_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(ASM_OBJS))
C_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT   := $(patsubst %.o, %.d, $(C_OBJS_EXT))
CPP_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT := $(patsubst %.o, %.d, $(CPP_OBJS_EXT))


#-----------------------------------------------------------------------------
# rules
#

dbg: $(EXE_DIR) $(BIN_DIR) $(TARGET_BIN)

spy: $(EXE_DIR) $(BIN_DIR) $(TARGET_BIN)

$(EXE_DIR):
	@echo
	mkdir -p $@

$(BIN_DIR):
	@echo
	mkdir -p $@

$(TARGET_BIN): $(TARGET_ELF)
	$(BIN) -O ihex $< $@

$(TARGET_ELF) : $(ASM_OBJS_EXT) $(C_OBJS_EXT) $(CPP_OBJS_EXT)
	$(LINK) $(LINKFLAGS) -o $@ $^ $(LIBS)

$(BIN_DIR)/%.d : %.c
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@

$(BIN_DIR)/%.d : %.cpp
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.o : %.s
	$(AS) $(ASFLAGS) $< -o $@

$(BIN_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

-include $(C_DEPS_EXT) $(CPP_DEPS_EXT)

.PHONY : clean
clean_dbg: clean
clean_spy: clean

clean:
	-$(RM) $(BIN_DIR)/*.o \
	$(BIN_DIR)/*.d \
	$(EXE_DIR)/*.hex \
	$(EXE_DIR)/*.elf \
	$(EXE_DIR)/*.map
	
show:
	@echo CONF = $(CONF)
	@echo ASM_SRCS = $(ASM_SRCS)
	@echo C_SRCS = $(C_SRCS)
	@echo CPP_SRCS = $(CPP_SRCS)
	@echo ASM_OBJS_EXT = $(ASM_OBJS_EXT)
	@echo C_OBJS_EXT = $(C_OBJS_EXT)
	@echo C_DEPS_EXT = $(C_DEPS_EXT)
	@echo CPP_DEPS_EXT = $(CPP_DEPS_EXT)
	@echo TARGET_ELF = $(TARGET_ELF)
