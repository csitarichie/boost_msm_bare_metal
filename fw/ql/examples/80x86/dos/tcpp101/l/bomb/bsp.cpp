//////////////////////////////////////////////////////////////////////////////
// Product: Board Support Package (BSP) for the Time Bomb example
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
#include "bsp.h"

#include <iostream.h>
#include <stdlib.h>

//............................................................................
void BSP_display(uint8_t timeout) {
    cout.width(3);
    cout << '[' << (int)timeout << "] ";
}
//............................................................................
void BSP_boom(void) {
    cout << "BOOM!!!";
    _exit(0);
}
//............................................................................
// this function is used by the QP embedded systems-friendly assertions
extern "C" void Q_onAssert(char const * const file, int line) {
    cout << "Assertion failed in " << file << " , line " << line << endl;
    _exit(-1);
}
