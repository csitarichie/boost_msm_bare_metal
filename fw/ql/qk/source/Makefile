##############################################################################
# Product:  QK, Generic build
# Last Updated for Version: 1.0.00
# Date of the Last Update:  Apr 07, 2008
#
#                              Richard  Szabo
#                    --------------------------------
#                    protocol stacks embedded systems
#
# Copyright (C) 2002-2010 Richard Szabo, All rights reserved.
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
# Quantum Leaps Web site:  http://richieshome.org
# e-mail:                  richard.szabo@richieshome.org
##############################################################################
# general utilities
RM = rm

# The gcc compiler and ar librarian
CC  = $(COMPILER_ROOT)/bin/arm-none-eabi-g++
LIB = $(COMPILER_ROOT)/bin/arm-none-eabi-ar
ASM = $(COMPILER_ROOT)/bin/arm-none-eabi-as

QP_INCDIR  = $(QL_INCLUDE_DIR)
QF_SRCDIR  = .
QP_PRTDIR  = $(QL_PORT_DIR)
LIBDIR     = $(LIB_DIR)

ARM_CORE   = cortex-m3

LIBFLAGS   = rs

CCINC      = -I$(QP_PRTDIR) -I$(QP_INCDIR)


# dbg (default target) .......................................................

BINDIR  = $(OBJ_DIR)/qk
QP_DEP  = $(QP_INCDIR)/qassert.h

# gcc options
# -c    : compile only
# -g    : source level debugging
CCFLAGS  = -g -c -mcpu=$(ARM_CORE) -mthumb -Wall -O -fno-rtti -fno-exceptions -o$@
ASMFLAGS = -g -mcpu=$(ARM_CORE) -o$@

# spy ........................................................................
ifeq ($(MAKECMDGOALS), spy)

BINDIR  = $(OBJ_DIR)/qk
QP_DEP  = $(QP_INCDIR)/qassert.h

# gcc options
# -c    : compile only
# -g    : source level debugging
CCFLAGS  = -g -c -mcpu=$(ARM_CORE) -mthumb -Wall -DQ_SPY -O -fno-rtti -fno-exceptions -o$@
ASMFLAGS = -g -mcpu=$(ARM_CORE) -o$@

endif
# ............................................................................


dbg: $(BINDIR) $(LIBDIR) $(LIBDIR)/libqk.a

spy: $(BINDIR) $(LIBDIR) $(LIBDIR)/libqk.a

$(BINDIR):
	@echo
	mkdir -p $@

$(LIBDIR):
	@echo
	mkdir -p $@

clean_dbg: clean

clean_spy: clean

.PHONY: dbg
.PHONY: clean

# QK =========================================================================

$(LIBDIR)/libqk.a: \
	$(BINDIR)/qk_ext.o \
	$(BINDIR)/qk_mutex.o \
	$(BINDIR)/qk_sched.o \
	$(BINDIR)/qk.o \
	$(BINDIR)/qk_port.o
	-$(RM) -f $(LIBDIR)/libqk.a
	$(LIB) $(LIBFLAGS) $(LIBDIR)/libqk.a \
	$(BINDIR)/qk_ext.o \
	$(BINDIR)/qk_mutex.o \
	$(BINDIR)/qk_sched.o \
	$(BINDIR)/qk.o \
	$(BINDIR)/qk_port.o
	-$(RM) $(BINDIR)/*.o

$(BINDIR)/qk_ext.o: $(QF_SRCDIR)/qk_ext.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qk_mutex.o: $(QF_SRCDIR)/qk_mutex.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qk_sched.o: $(QF_SRCDIR)/qk_sched.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qk.o: $(QF_SRCDIR)/qk.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qk_port.o: $(QP_PRTDIR)/src/qk_port.s
	$(ASM) $(ASMFLAGS) $<
		
clean:
	-$(RM) $(BINDIR)/*.o
	-$(RM) $(LIBDIR)/*.a