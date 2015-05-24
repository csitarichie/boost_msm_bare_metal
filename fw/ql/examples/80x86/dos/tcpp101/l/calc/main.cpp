//////////////////////////////////////////////////////////////////////////////
// Product:  Calculator Example
// Date of the Last Update:  Oct 30, 2007
//
// Copyright (C) 2002-2007 Miro Samek. All rights reserved.
//
// Contact information:
// e-mail: miro@quantum-leaps.com
//////////////////////////////////////////////////////////////////////////////
#include "qp_port.h"                           // the port of the QP framework
#include "bsp.h"                                      // board support package
#include "calc.h"

#include <iostream.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>

//............................................................................
void main() {

    cout << "Calculator example, QEP version: "
         << QEP::getVersion() << endl
         << "Press '0' .. '9'     to enter a digit\n"
            "Press '.'            to enter the decimal point\n"
            "Press '+'            to add\n"
            "Press '-'            to subtract or negate a number\n"
            "Press '*'            to multiply\n"
            "Press '/'            to divide\n"
            "Press '=' or <Enter> to get the result\n"
            "Press 'c' or 'C'     to Cancel\n"
            "Press 'e' or 'E'     to Cancel Entry\n"
            "Press <Esc>          to quit.\n\n";

    the_calc->init();                            // trigger initial transition

    for (;;) {                                                   // event loop
        CalcEvt e;                                         // Calculator event

        BSP_display();                                     // show the display

        e.key_code = (uint8_t)getche();                // get a char with echo
        cout << ": ";

        switch (e.key_code) {
            case 'c':                            // intentionally fall through
            case 'C': {
                ((QEvent *)&e)->sig = C_SIG;
                break;
            }
            case 'e':                            // intentionally fall through
            case 'E': {
                ((QEvent *)&e)->sig = CE_SIG;
                break;
            }
            case '0': {
                ((QEvent *)&e)->sig = DIGIT_0_SIG;
                break;
            }
            case '1':                            // intentionally fall through
            case '2':                            // intentionally fall through
            case '3':                            // intentionally fall through
            case '4':                            // intentionally fall through
            case '5':                            // intentionally fall through
            case '6':                            // intentionally fall through
            case '7':                            // intentionally fall through
            case '8':                            // intentionally fall through
            case '9': {
                ((QEvent *)&e)->sig = DIGIT_1_9_SIG;
                break;
            }
            case '.': {
                ((QEvent *)&e)->sig = POINT_SIG;
                break;
            }
            case '+':                            // intentionally fall through
            case '-':                            // intentionally fall through
            case '*':                            // intentionally fall through
            case '/': {
                ((QEvent *)&e)->sig = OPER_SIG;
                break;
            }
            case '=':                            // intentionally fall through
            case '\r': {                                          // Enter key
                ((QEvent *)&e)->sig = EQUALS_SIG;
                break;
            }
            case '\33': {                                           // ESC key
                ((QEvent *)&e)->sig = OFF_SIG;
                break;
            }
            default: {
                ((QEvent *)&e)->sig = 0;                      // invalid event
                break;
            }
        }

        if (((QEvent *)&e)->sig != 0) {              // valid event generated?
            the_calc->dispatch(&e);                          // dispatch event
        }
    }
}
