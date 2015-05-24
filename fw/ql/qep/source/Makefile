##############################################################################
# Product:  QEP, Generic build
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
QEP_SRCDIR = .
QP_PRTDIR  = $(QL_PORT_DIR)
LIBDIR     = $(LIB_DIR)

ARM_CORE   = cortex-m3

LIBFLAGS   = rs

CCINC      = -I$(QP_PRTDIR) -I$(QP_INCDIR)


# dbg (default target) .......................................................

BINDIR  = $(OBJ_DIR)/qep
QP_DEP  = $(QP_INCDIR)/qassert.h

# gcc options
# -c    : compile only
# -g    : source level debugging
CCFLAGS  = -g -c -mcpu=$(ARM_CORE) -mthumb -Wall -O -fno-rtti -fno-exceptions -o$@
ASMFLAGS = -g -mcpu=$(ARM_CORE)

# spy ........................................................................
ifeq ($(MAKECMDGOALS), spy)

BINDIR  = $(OBJ_DIR)/qep
QP_DEP  = $(QP_INCDIR)/qassert.h

# gcc options
# -c    : compile only
# -g    : source level debugging
CCFLAGS  = -g -c -mcpu=$(ARM_CORE) -mthumb -Wall -DQ_SPY -O -fno-rtti -fno-exceptions -o$@
ASMFLAGS = -g -mcpu=$(ARM_CORE) -o$@

endif
# ............................................................................
dbg: $(BINDIR) $(LIBDIR) $(LIBDIR)/libqep.a

spy: $(BINDIR) $(LIBDIR) $(LIBDIR)/libqep.a

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

# QEP ========================================================================

$(LIBDIR)/libqep.a: \
	$(BINDIR)/qep.o \
	$(BINDIR)/qfsm_ini.o \
	$(BINDIR)/qfsm_dis.o \
	$(BINDIR)/qhsm_ini.o \
	$(BINDIR)/qhsm_dis.o \
	$(BINDIR)/qhsm_top.o \
	$(BINDIR)/qhsm_in.o
	-$(RM) -f $(LIBDIR)/libqep.a
	$(LIB) $(LIBFLAGS) $(LIBDIR)/libqep.a \
	$(BINDIR)/qep.o \
	$(BINDIR)/qfsm_ini.o \
	$(BINDIR)/qfsm_dis.o \
	$(BINDIR)/qhsm_ini.o \
	$(BINDIR)/qhsm_dis.o \
	$(BINDIR)/qhsm_top.o \
	$(BINDIR)/qhsm_in.o
	-$(RM) $(BINDIR)/*.o

$(BINDIR)/qep.o: $(QEP_SRCDIR)/qep.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QEP_SRCDIR) $<

$(BINDIR)/qfsm_ini.o: $(QEP_SRCDIR)/qfsm_ini.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QEP_SRCDIR) $<

$(BINDIR)/qfsm_dis.o: $(QEP_SRCDIR)/qfsm_dis.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QEP_SRCDIR) $<

$(BINDIR)/qhsm_ini.o: $(QEP_SRCDIR)/qhsm_ini.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QEP_SRCDIR) $<

$(BINDIR)/qhsm_dis.o: $(QEP_SRCDIR)/qhsm_dis.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QEP_SRCDIR) $<

$(BINDIR)/qhsm_top.o: $(QEP_SRCDIR)/qhsm_top.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QEP_SRCDIR) $<

$(BINDIR)/qhsm_in.o: $(QEP_SRCDIR)/qhsm_in.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QEP_SRCDIR) $<
	
clean:
	-$(RM) $(BINDIR)/*.o
	-$(RM) $(LIBDIR)/*.a




