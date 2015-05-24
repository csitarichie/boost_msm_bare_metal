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

//............................................................................
class AlarmClock : public QActive {  // the AlarmClock container active object
private:
    Alarm    m_alarmComp;                        // Alarm orthogonal component
    QTimeEvt m_timeEvt;                        // private time event generator
    uint32_t m_currentTime;                     // the current time in seconds

public:
    AlarmClock()                                               // default ctor
      : QActive((QStateHandler)&AlarmClock::initial),
        m_timeEvt(TIME_SIG) {}

private:
                                             // hierarchical state machine ...
    static QState initial    (AlarmClock *me, QEvent const *e);
    static QState timekeeping(AlarmClock *me, QEvent const *e);
    static QState mode12hr   (AlarmClock *me, QEvent const *e);
    static QState mode24hr   (AlarmClock *me, QEvent const *e);
    static QState final      (AlarmClock *me, QEvent const *e);
};

// HSM definition ------------------------------------------------------------
QState AlarmClock::initial(AlarmClock *me, QEvent const *) {
    me->m_currentTime = 0;

                      // take the initial transition in the alarm component...
    me->m_alarmComp.init();

    return Q_TRAN(&AlarmClock::timekeeping);
}
//............................................................................
QState AlarmClock::final(AlarmClock *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            printf("-> final\n");
            printf("\nBye!Bye!\n");
            QF::stop();                                 // stop QF and cleanup
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm::top);
}
//............................................................................
QState AlarmClock::timekeeping(AlarmClock *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
                                              // periodic timeout every second
            me->m_timeEvt.fireEvery(me, BSP_TICKS_PER_SEC);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            me->m_timeEvt.disarm();
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&AlarmClock::mode24hr);
        }
        case CLOCK_12H_SIG: {
            return Q_TRAN(&AlarmClock::mode12hr);
        }
        case CLOCK_24H_SIG: {
            return Q_TRAN(&AlarmClock::mode24hr);
        }
        case ALARM_SIG: {
            printf("Wake up!!!\n");
            return Q_HANDLED();
        }
        case ALARM_SET_SIG:
        case ALARM_ON_SIG:
        case ALARM_OFF_SIG: {
            me->m_alarmComp.dispatch(e);     // synchronously dispatch to comp.
            return Q_HANDLED();
        }
        case TERMINATE_SIG: {
            return Q_TRAN(&AlarmClock::final);
        }
    }
    return Q_SUPER(&QHsm::top);
}
//............................................................................
QState AlarmClock::mode24hr(AlarmClock *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            printf("*** 24-hour mode\n");
            return Q_HANDLED();
        }
        case TIME_SIG: {
            if (++me->m_currentTime == 24*60) {//roll over time in 24-hr mode?
                me->m_currentTime = 0;
            }
            printf("%02ld:%02ld\n",
                  me->m_currentTime / 60, me->m_currentTime % 60);
            TimeEvt pe;       // temporary synchronous event for the component
            pe.sig = TIME_SIG;
            pe.currentTime = me->m_currentTime;
            me->m_alarmComp.dispatch(&pe);  // synchronously dispatch to comp.
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&AlarmClock::timekeeping);
}
//............................................................................
QState AlarmClock::mode12hr(AlarmClock *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            printf("*** 12-hour mode\n");
            return Q_HANDLED();
        }
        case TIME_SIG: {
            if (++me->m_currentTime == 12*60) {//roll over time in 12-hr mode?
                me->m_currentTime = 0;
            }
            uint32_t h = me->m_currentTime/60;
            printf("%02ld:%02ld %s\n", (h % 12) ? (h % 12) : 12,
                   me->m_currentTime % 60, (h / 12) ? "PM" : "AM");
            TimeEvt pe;       // temporary synchronous event for the component
            pe.sig = TIME_SIG;
            pe.currentTime = me->m_currentTime;
            me->m_alarmComp.dispatch(&pe);  // synchronously dispatch to comp.
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&AlarmClock::timekeeping);
}

// test harness ==============================================================

// Local-scope objects -------------------------------------------------------
static AlarmClock    l_alarmClock;             // the AlarmClock active object
static QEvent const *l_alarmClockQSto[10];     // queue storage for AlarmClock
static TimeEvt       l_smlPoolSto[10];                     // small event pool

// Global-scope objects (opaque pointer to the AlarmClock container) ---------
QActive * const APP_alarmClock= &l_alarmClock;     // AlarmClock active object

//............................................................................
int main(int argc, char *argv[]) {
    printf("Reminder state pattern\nQEP version: %s\nQF  version: %s\n"
           "Press 'o' to turn the Alarm ON\n"
           "Press 'f' to turn the Alarm OFF\n"
           "Press '0'..'9' to set the Alarm time\n"
           "Press 'A' to set the Clock in 12-hour mode\n"
           "Press 'B' to set the Clock in 24-hour mode\n"
           "Press ESC to quit...\n",
           QEP::getVersion(), QF::getVersion());

    BSP_init(argc, argv);                                // initialize the BSP

    QF::init();       // initialize the framework and the underlying RT kernel

    // publish-subscribe not used, no call to QF::psInit()

                                                  // initialize event pools...
    QF::poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));

                                                // start the active objects...
    l_alarmClock.start(1, l_alarmClockQSto, Q_DIM(l_alarmClockQSto),
                       (void *)0, 0, (QEvent *)0);

    QF::run();                                       // run the QF application

    return 0;
}
//............................................................................
void onConsoleInput(uint8_t key) {
    switch (key) {
        case 'o': {                                       // Alarm 'o'n event?
            l_alarmClock.postFIFO(Q_NEW(QEvent, ALARM_ON_SIG));
            break;
        }
        case 'f': {                                      // Alarm o'f'f event?
            l_alarmClock.postFIFO(Q_NEW(QEvent, ALARM_OFF_SIG));
            break;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {                                        // Alarm set event?
            SetEvt *e = Q_NEW(SetEvt, ALARM_SET_SIG);
            e->digit = (uint8_t)(key - (uint8_t)'0');
            l_alarmClock.postFIFO(e);
            break;
        }
        case 'A': {                                        // Clock 12H event?
            l_alarmClock.postFIFO(Q_NEW(QEvent, CLOCK_12H_SIG));
            break;
        }
        case 'B': {                                        // Clock 24H event?
            l_alarmClock.postFIFO(Q_NEW(QEvent, CLOCK_24H_SIG));
            break;
        }
        case 0x1B: {                                           // ESC pressed?
            l_alarmClock.postFIFO(Q_NEW(QEvent, TERMINATE_SIG));
            break;
        }
    }
}
