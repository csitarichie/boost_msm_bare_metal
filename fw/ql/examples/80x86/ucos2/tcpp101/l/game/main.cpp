//////////////////////////////////////////////////////////////////////////////
// Product: "Fly'n'Shoot" game example for uC/OS-II
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
#include "game.h"

// Local-scope objects -------------------------------------------------------
static QEvent const * l_missileQueueSto[2];
static QEvent const * l_shipQueueSto[3];
static QEvent const * l_tunnelQueueSto[GAME_MINES_MAX + 5];
static ObjectPosEvt   l_smlPoolSto[GAME_MINES_MAX + 8];    // small event pool
static ObjectImageEvt l_medPoolSto[GAME_MINES_MAX + 8];   // medium event pool
static QSubscrList    l_subscrSto[MAX_PUB_SIG];

static OS_STK l_missileStk[256];                          // stack for Missile
static OS_STK l_shipStk[256];                                // stack for Ship
static OS_STK l_tunnelStk[256];                        // stack for the Tunnel
static OS_STK l_ucosTaskStk[256];                    // stack for the ucosTask

//............................................................................
int main(int argc, char *argv[]) {

    BSP_init(argc, argv);              // initialize the Board Support Package

    QF::init();       // initialize the framework and the underlying RT kernel

                                              // initialize the event pools...
    QF::poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
    QF::poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));

    QF::psInit(l_subscrSto, Q_DIM(l_subscrSto));     // init publish-subscribe

                                                    // setup the QS filters...
    QS_FILTER_ON(QS_ALL_RECORDS);

//    QS_FILTER_OFF(QS_QEP_STATE_EMPTY);
//    QS_FILTER_OFF(QS_QEP_STATE_ENTRY);
//    QS_FILTER_OFF(QS_QEP_STATE_EXIT);
//    QS_FILTER_OFF(QS_QEP_STATE_INIT);
//    QS_FILTER_OFF(QS_QEP_INIT_TRAN);
//    QS_FILTER_OFF(QS_QEP_INTERN_TRAN);
//    QS_FILTER_OFF(QS_QEP_TRAN);
//    QS_FILTER_OFF(QS_QEP_IGNORED);

    QS_FILTER_OFF(QS_QF_ACTIVE_ADD);
    QS_FILTER_OFF(QS_QF_ACTIVE_REMOVE);
    QS_FILTER_OFF(QS_QF_ACTIVE_SUBSCRIBE);
    QS_FILTER_OFF(QS_QF_ACTIVE_UNSUBSCRIBE);
    QS_FILTER_OFF(QS_QF_ACTIVE_POST_FIFO);
    QS_FILTER_OFF(QS_QF_ACTIVE_POST_LIFO);
    QS_FILTER_OFF(QS_QF_ACTIVE_GET);
    QS_FILTER_OFF(QS_QF_ACTIVE_GET_LAST);
    QS_FILTER_OFF(QS_QF_EQUEUE_INIT);
    QS_FILTER_OFF(QS_QF_EQUEUE_POST_FIFO);
    QS_FILTER_OFF(QS_QF_EQUEUE_POST_LIFO);
    QS_FILTER_OFF(QS_QF_EQUEUE_GET);
    QS_FILTER_OFF(QS_QF_EQUEUE_GET_LAST);
    QS_FILTER_OFF(QS_QF_MPOOL_INIT);
    QS_FILTER_OFF(QS_QF_MPOOL_GET);
    QS_FILTER_OFF(QS_QF_MPOOL_PUT);
    QS_FILTER_OFF(QS_QF_PUBLISH);
    QS_FILTER_OFF(QS_QF_NEW);
    QS_FILTER_OFF(QS_QF_GC_ATTEMPT);
    QS_FILTER_OFF(QS_QF_GC);
//    QS_FILTER_OFF(QS_QF_TICK);
    QS_FILTER_OFF(QS_QF_TIMEEVT_ARM);
    QS_FILTER_OFF(QS_QF_TIMEEVT_AUTO_DISARM);
    QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM_ATTEMPT);
    QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM);
    QS_FILTER_OFF(QS_QF_TIMEEVT_REARM);
    QS_FILTER_OFF(QS_QF_TIMEEVT_POST);
    QS_FILTER_OFF(QS_QF_INT_LOCK);
    QS_FILTER_OFF(QS_QF_INT_UNLOCK);
    QS_FILTER_OFF(QS_QF_ISR_ENTRY);
    QS_FILTER_OFF(QS_QF_ISR_EXIT);

                               // send object dictionaries for event queues...
    QS_OBJ_DICTIONARY(l_missileQueueSto);
    QS_OBJ_DICTIONARY(l_shipQueueSto);
    QS_OBJ_DICTIONARY(l_tunnelQueueSto);

                                // send object dictionaries for event pools...
    QS_OBJ_DICTIONARY(l_smlPoolSto);
    QS_OBJ_DICTIONARY(l_medPoolSto);

                  // send signal dictionaries for globally published events...
    QS_SIG_DICTIONARY(TIME_TICK_SIG,      0);
    QS_SIG_DICTIONARY(PLAYER_TRIGGER_SIG, 0);
    QS_SIG_DICTIONARY(PLAYER_QUIT_SIG,    0);
    QS_SIG_DICTIONARY(GAME_OVER_SIG,      0);

                                                // start the active objects...
    AO_Missile->start(1,                                           // priority
                  l_missileQueueSto, Q_DIM(l_missileQueueSto),    // evt queue
                  l_missileStk, sizeof(l_missileStk),        // uC/OS-II stack
                  (QEvent *)0);                     // no initialization event

    AO_Ship->start(2,                                              // priority
                  l_shipQueueSto,    Q_DIM(l_shipQueueSto),       // evt queue
                  l_shipStk, sizeof(l_shipStk),              // uC/OS-II stack
                  (QEvent *)0);                     // no initialization event

    AO_Tunnel->start(3,                                            // priority
                  l_tunnelQueueSto,  Q_DIM(l_tunnelQueueSto),     // evt queue
                  l_tunnelStk, sizeof(l_tunnelStk),          // uC/OS-II stack
                  (QEvent *)0);                     // no initialization event

           // create a uC/OS-II task to start interrupts and poll the keyboard
    OSTaskCreate(&ucosTask,
                 (void *)0,                                           // pdata
                 &l_ucosTaskStk[Q_DIM(l_ucosTaskStk) - 1],
                 0);                          // the highest uC/OS-II priority

    QF::run();                                       // run the QF application

    return 0;
}
