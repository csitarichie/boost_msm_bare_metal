//////////////////////////////////////////////////////////////////////////////
// Product:  Board Support Package (BSP) for the Calculator example
// Date of the Last Update:  Oct 30, 2007
//
// Copyright (C) 2002-2007 Miro Samek. All rights reserved.
//
// Contact information:
// e-mail: miro@quantum-leaps.com
//////////////////////////////////////////////////////////////////////////////
#include "bsp.h"

#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DISP_WIDTH  9

static char l_display[DISP_WIDTH + 1];               // the calculator display
static int  l_len;                           // number of displayed characters

//............................................................................
void BSP_clear(void) {
    memset(l_display, ' ', DISP_WIDTH - 1);
    l_display[DISP_WIDTH - 1] = '0';
    l_display[DISP_WIDTH] = '\0';
    l_len = 0;
}
//............................................................................
void BSP_insert(int keyId) {
    if (l_len == 0) {
        l_display[DISP_WIDTH - 1] = (char)keyId;
        ++l_len;
    }
    else if (l_len < (DISP_WIDTH - 1)) {
        memmove(&l_display[0], &l_display[1], DISP_WIDTH - 1);
        l_display[DISP_WIDTH - 1] = (char)keyId;
        ++l_len;
    }
}
//............................................................................
void BSP_negate(void) {
    BSP_clear();
    l_display[DISP_WIDTH - 2] = '-';
}
//............................................................................
void BSP_display(void) {
    cout << endl << '[' << l_display << "] ";
}
//............................................................................
void BSP_exit(void) {
    cout << endl << "Bye! Bye";
    _exit(0);
}
//............................................................................
double BSP_get_value(void) {
    return strtod(l_display, (char **)0);
}
//............................................................................
int BSP_eval(double operand1, int oper, double operand2) {
    int ok = 1;
    double result;
    switch (oper) {
        case KEY_PLUS: {
            result = operand1 + operand2;
            break;
        }
        case KEY_MINUS: {
            result = operand1 - operand2;
            break;
        }
        case KEY_MULT: {
            result = operand1 * operand2;
            break;
        }
        case KEY_DIVIDE: {
            if ((operand2 < -1e-30) || (1e-30 < operand2)) {
                result = operand1 / operand2;
            }
            else {
            strcpy(l_display, " Error 0 ");           // error: divide by zero
                ok = 0;
            }
            break;
        }
    }
    if (ok) {
        if ((-99999999.0 < result) && (result < 99999999.0)) {
            sprintf(l_display, "%9.6g", result);
        }
        else {
            strcpy(l_display, " Error 1 ");             // error: out of range
            ok = 0;
        }
    }
    return ok;
}
//............................................................................
void BSP_message(char const *msg) {
    cout << msg;
}
//............................................................................
// this function is used by the QP embedded systems-friendly assertions
extern "C" void Q_onAssert(char const * const file, int line) {
    cout << "Assertion failed in " << file << " , line " << line << endl;
    _exit(-1);
}
