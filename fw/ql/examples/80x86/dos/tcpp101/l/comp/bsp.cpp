//////////////////////////////////////////////////////////////////////////////
// Product: DOS console-based BSP, Turbo C++ 1.01
// Last Updated for Version: 4.0.00
// Date of the Last Update:  Apr 07, 2008
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
//
// This software may be distributed and modified under the terms of the GNU
// General Public License version 2 (GPL) as published by the Free Software
// Foundation and appearing in the file GPL.TXT included in the packaging of
// this file. Please note that GPL Section 2[b] requires that all works based
// on this software must also be made publicly available under the terms of
// the GPL ("Copyleft").
//
// Alternatively, this software may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GPL and are specifically designed for licensees interested in
// retaining the proprietary status of their code.
//
// Contact information:
// Quantum Leaps Web site:  http://www.quantum-leaps.com
// e-mail:                  info@quantum-leaps.com
//////////////////////////////////////////////////////////////////////////////
#include "qp_port.h"
#include "bsp.h"

#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

// Local-scope objects -------------------------------------------------------
static void interrupt (*l_dosTmrISR)(...);
static void interrupt (*l_dosSpareISR)(...);

#define TMR_VECTOR      0x08
#define SPARE_VECTOR    0x81

//............................................................................
static void interrupt tmrISR(...) {
    QF_INT_UNLOCK(dummy);                                 // unlock interrupts
    QF::tick();                                //<-- process the QF clock tick
    QF_INT_LOCK(dummy);                               // lock interrupts again
    geninterrupt(SPARE_VECTOR);        // invoke the DOS timer ISR, see NOTE01
}
//............................................................................
void BSP_init(int, char *[]) {
}
//............................................................................
void QF::onStartup(void) {
                                         // save the origingal DOS vectors ...
    l_dosTmrISR   = getvect(TMR_VECTOR);
    l_dosSpareISR = getvect(SPARE_VECTOR);

    QF_INT_LOCK(dummy);
    setvect(TMR_VECTOR, &tmrISR);
    setvect(SPARE_VECTOR, l_dosTmrISR);
    QF_INT_UNLOCK(dummy);
}
//............................................................................
void QF::onCleanup(void) {
                                       // restore the original DOS vectors ...
    QF_INT_LOCK(dummy);
    setvect(TMR_VECTOR, l_dosTmrISR);
    setvect(SPARE_VECTOR, l_dosSpareISR);
    QF_INT_UNLOCK(dummy);

    _exit(0);                                                   // exit to DOS
}
//............................................................................
void QF::onIdle(void) {                // NOTE: entered with interrupts LOCKED
    QF_INT_UNLOCK(dummy);                   // must at least unlock interrupts

    if (kbhit()) {
        onConsoleInput((uint8_t)getch());
    }
}
//............................................................................
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    QF::stop();
}

//////////////////////////////////////////////////////////////////////////////
// NOTE01:
// The sofware interrupt generated at the end of the tick ISR invokes the
// original DOS tick ISR handler, which was installed at the spare vector
// 0x81. The DOS tick ISR handler generates the EOI (End-Of-Interrupt)
// to the master 8259A PIC.
