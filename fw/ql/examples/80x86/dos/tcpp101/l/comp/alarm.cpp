//////////////////////////////////////////////////////////////////////////////
// Product: Orthogonal Component state pattern example
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
#include "alarm.h"
#include "clock.h"

#include <stdio.h>

Q_DEFINE_THIS_FILE

// HSM definition ------------------------------------------------------------
QState Alarm::initial(Alarm *me, QEvent const *) {
    me->m_alarmTime = 12*60;
    return Q_TRAN(&Alarm::alarm);
}
//............................................................................
QState Alarm::alarm(Alarm *me, QEvent const *e) {
    switch (e->sig) {
        case Q_INIT_SIG: {
            return Q_TRAN(&Alarm::on);
        }
        case ALARM_ON_SIG: {
            return Q_TRAN(&Alarm::on);
        }
        case ALARM_OFF_SIG: {
            return Q_TRAN(&Alarm::off);
        }
    }
    return Q_SUPER(&QHsm::top);
}
//............................................................................
QState Alarm::on(Alarm *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            printf("*** Alarm ON %02ld:%02ld\n",
                   me->m_alarmTime/60, me->m_alarmTime%60);
            return Q_HANDLED();
        }
        case ALARM_SET_SIG: {
            printf("*** Cannot set Alarm when it is ON\n");
            return Q_HANDLED();
        }
        case TIME_SIG: {
            if (((TimeEvt *)e)->currentTime == me->m_alarmTime) {
                printf("ALARM!!!\n");
                APP_alarmClock->postFIFO(Q_NEW(QEvent, ALARM_SIG));
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Alarm::alarm);
}
//............................................................................
QState Alarm::off(Alarm *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            // while in the off state, the alarm is kept in decimal format
            me->m_alarmTime = (me->m_alarmTime/60)*100 + me->m_alarmTime%60;
            printf("*** Alarm OFF %02ld:%02ld\n",
                   me->m_alarmTime/100, me->m_alarmTime%100);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            // upon exit, the alarm is converted to binary format
            me->m_alarmTime = (me->m_alarmTime/100)*60 + me->m_alarmTime%100;
            return Q_HANDLED();
        }
        case ALARM_SET_SIG: {
            // while setting, the alarm is kept in decimal format
            uint32_t alarm = (10 * me->m_alarmTime
                              + ((SetEvt const *)e)->digit) % 10000;
            if ((alarm / 100 < 24) && (alarm % 100 < 60)) { // alarm in range?
                me->m_alarmTime = alarm;
            }
            else {                         // alarm out of range -- start over
                me->m_alarmTime = 0;
            }
            printf("*** Alarm SET %02ld:%02ld\n",
                   me->m_alarmTime/100, me->m_alarmTime%100);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Alarm::alarm);
}
