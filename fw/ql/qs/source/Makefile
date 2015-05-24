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
QS_SRCDIR  = .
QP_PRTDIR  = $(QL_PORT_DIR)
LIBDIR     = $(LIB_DIR)

ARM_CORE   = cortex-m3

LIBFLAGS   = rs

CCINC      = -I$(QP_PRTDIR) -I$(QP_INCDIR)


# spy (default target) .......................................................

BINDIR  = $(OBJ_DIR)/qs
QP_DEP  = $(QP_INCDIR)/qassert.h

# gcc options
# -c    : compile only
# -g    : source level debugging
CCFLAGS  = -g -c -mcpu=$(ARM_CORE) -mthumb -Wall -DQ_SPY -O -fno-rtti -fno-exceptions -o$@
ASMFLAGS = -g -mcpu=$(ARM_CORE) -o$@

# ............................................................................
spy: $(BINDIR) $(LIBDIR) $(LIBDIR)/libqs.a

$(BINDIR):
	@echo
	mkdir -p $@

$(LIBDIR):
	@echo
	mkdir -p $@

clean_spy: clean

.PHONY: spy
.PHONY: clean

# QSPY =========================================================================

$(LIBDIR)/libqs.a: \
	$(BINDIR)/qs.o \
	$(BINDIR)/qs_.o \
	$(BINDIR)/qs_blk.o \
	$(BINDIR)/qs_byte.o \
	$(BINDIR)/qs_f32.o \
	$(BINDIR)/qs_f64.o \
	$(BINDIR)/qs_mem.o \
	$(BINDIR)/qs_str.o 
	-$(RM) -f $(LIBDIR)/libqs.a
	$(LIB) $(LIBFLAGS) $(LIBDIR)/libqs.a \
	$(BINDIR)/qs.o \
	$(BINDIR)/qs_.o \
	$(BINDIR)/qs_blk.o \
	$(BINDIR)/qs_byte.o \
	$(BINDIR)/qs_f32.o \
	$(BINDIR)/qs_f64.o \
	$(BINDIR)/qs_mem.o \
	$(BINDIR)/qs_str.o
	-$(RM) $(BINDIR)/*.o

$(BINDIR)/qs.o: $(QS_SRCDIR)/qs.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QS_SRCDIR) $<

$(BINDIR)/qs_.o: $(QS_SRCDIR)/qs_.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QS_SRCDIR) $<

$(BINDIR)/qs_blk.o: $(QS_SRCDIR)/qs_blk.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QS_SRCDIR) $<

$(BINDIR)/qs_byte.o: $(QS_SRCDIR)/qs_byte.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QS_SRCDIR) $<

$(BINDIR)/qs_f32.o: $(QS_SRCDIR)/qs_f32.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QS_SRCDIR) $<

$(BINDIR)/qs_f64.o: $(QS_SRCDIR)/qs_f64.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QS_SRCDIR) $<

$(BINDIR)/qs_mem.o: $(QS_SRCDIR)/qs_mem.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QS_SRCDIR) $<

$(BINDIR)/qs_str.o: $(QS_SRCDIR)/qs_str.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QS_SRCDIR) $<

# Does not exists for target
# $(BINDIR)/qs_port.o: src/qs_port.cpp
# 	$(CC) $(CCFLAGS) $(CCINC) -I$(QS_SRCDIR) $<
		
clean:
	-$(RM) $(BINDIR)/*.o
	-$(RM) $(LIBDIR)/*.a