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
QF_SRCDIR  = .
QP_PRTDIR  = $(QL_PORT_DIR)
LIBDIR     = $(LIB_DIR)

ARM_CORE   = cortex-m3

LIBFLAGS   = rs

CCINC      = -I$(QP_PRTDIR) -I$(QP_INCDIR)


# dbg (default target) .......................................................

BINDIR  = $(OBJ_DIR)/qf
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
dbg: $(BINDIR) $(LIBDIR) $(LIBDIR)/libqf.a

spy: $(BINDIR) $(LIBDIR) $(LIBDIR)/libqf.a

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

# QF =========================================================================

$(LIBDIR)/libqf.a: \
	$(BINDIR)/qa_defer.o \
	$(BINDIR)/qa_fifo.o \
	$(BINDIR)/qa_lifo.o \
	$(BINDIR)/qa_get_.o \
	$(BINDIR)/qa_run.o \
	$(BINDIR)/qa_sub.o \
	$(BINDIR)/qa_usub.o \
	$(BINDIR)/qa_usuba.o \
	$(BINDIR)/qeq_init.o \
	$(BINDIR)/qeq_fifo.o \
	$(BINDIR)/qeq_lifo.o \
	$(BINDIR)/qeq_get.o \
	$(BINDIR)/qf_act.o \
	$(BINDIR)/qf_gc.o \
	$(BINDIR)/qf_log2.o \
	$(BINDIR)/qf_new.o \
	$(BINDIR)/qf_pool.o \
	$(BINDIR)/qf_psini.o \
	$(BINDIR)/qf_pspub.o \
	$(BINDIR)/qf_pwr2.o \
	$(BINDIR)/qf_tick.o \
	$(BINDIR)/qmp_get.o \
	$(BINDIR)/qmp_init.o \
	$(BINDIR)/qmp_put.o \
	$(BINDIR)/qte_ctor.o \
	$(BINDIR)/qte_arm.o \
	$(BINDIR)/qte_darm.o \
	$(BINDIR)/qte_rarm.o \
	$(BINDIR)/qk_port.o
	-$(RM) -f $(LIBDIR)/libqf.a
	$(LIB) $(LIBFLAGS) $(LIBDIR)/libqf.a \
	$(BINDIR)/qa_defer.o \
	$(BINDIR)/qa_fifo.o \
	$(BINDIR)/qa_lifo.o \
	$(BINDIR)/qa_get_.o \
	$(BINDIR)/qa_run.o \
	$(BINDIR)/qa_sub.o \
	$(BINDIR)/qa_usub.o \
	$(BINDIR)/qa_usuba.o \
	$(BINDIR)/qeq_init.o \
	$(BINDIR)/qeq_fifo.o \
	$(BINDIR)/qeq_lifo.o \
	$(BINDIR)/qeq_get.o \
	$(BINDIR)/qf_act.o \
	$(BINDIR)/qf_gc.o \
	$(BINDIR)/qf_log2.o \
	$(BINDIR)/qf_new.o \
	$(BINDIR)/qf_pool.o \
	$(BINDIR)/qf_psini.o \
	$(BINDIR)/qf_pspub.o \
	$(BINDIR)/qf_pwr2.o \
	$(BINDIR)/qf_tick.o \
	$(BINDIR)/qmp_get.o \
	$(BINDIR)/qmp_init.o \
	$(BINDIR)/qmp_put.o \
	$(BINDIR)/qte_ctor.o \
	$(BINDIR)/qte_arm.o \
	$(BINDIR)/qte_darm.o \
	$(BINDIR)/qte_rarm.o \
	$(BINDIR)/qk_port.o
	-$(RM) $(BINDIR)/*.o

$(BINDIR)/qa_defer.o: $(QF_SRCDIR)/qa_defer.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qa_fifo.o: $(QF_SRCDIR)/qa_fifo.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qa_lifo.o: $(QF_SRCDIR)/qa_lifo.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qa_get_.o: $(QF_SRCDIR)/qa_get_.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qa_run.o: $(QF_SRCDIR)/qa_run.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qa_sub.o: $(QF_SRCDIR)/qa_sub.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qa_usub.o: $(QF_SRCDIR)/qa_usub.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qa_usuba.o: $(QF_SRCDIR)/qa_usuba.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qeq_init.o: $(QF_SRCDIR)/qeq_init.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qeq_fifo.o: $(QF_SRCDIR)/qeq_fifo.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qeq_lifo.o: $(QF_SRCDIR)/qeq_lifo.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qeq_get.o: $(QF_SRCDIR)/qeq_get.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qf_act.o: $(QF_SRCDIR)/qf_act.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qf_gc.o: $(QF_SRCDIR)/qf_gc.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qf_log2.o: $(QF_SRCDIR)/qf_log2.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qf_new.o: $(QF_SRCDIR)/qf_new.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qf_pool.o: $(QF_SRCDIR)/qf_pool.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qf_psini.o: $(QF_SRCDIR)/qf_psini.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qf_pspub.o: $(QF_SRCDIR)/qf_pspub.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qf_pwr2.o: $(QF_SRCDIR)/qf_pwr2.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qf_tick.o: $(QF_SRCDIR)/qf_tick.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qmp_get.o: $(QF_SRCDIR)/qmp_get.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qmp_init.o: $(QF_SRCDIR)/qmp_init.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qmp_put.o: $(QF_SRCDIR)/qmp_put.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qte_ctor.o: $(QF_SRCDIR)/qte_ctor.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qte_arm.o: $(QF_SRCDIR)/qte_arm.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qte_darm.o: $(QF_SRCDIR)/qte_darm.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qte_rarm.o: $(QF_SRCDIR)/qte_rarm.cpp
	$(CC) $(CCFLAGS) $(CCINC) -I$(QF_SRCDIR) $<

$(BINDIR)/qk_port.o: $(QP_PRTDIR)/src/qk_port.s
	$(ASM) $(ASMFLAGS) $<
		
clean:
	-$(RM) $(BINDIR)/*.o
	-$(RM) $(LIBDIR)/*.a