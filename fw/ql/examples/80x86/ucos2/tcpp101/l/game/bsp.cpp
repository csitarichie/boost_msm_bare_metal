//////////////////////////////////////////////////////////////////////////////
// Product: "Fly 'n' Shoot" game example, BSP for uC/OS-II with Turbo C++ 1.01
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
#include "game.h"
#include "bsp.h"

#include <stdlib.h>                                             // for _exit()
#include <conio.h>                                              // for kbhit()

Q_DEFINE_THIS_FILE

// Local objects -------------------------------------------------------------
static void interrupt (*l_dosTickISR)(...);

#ifdef Q_SPY
    static uint16_t l_uart_base;           // QS data uplink UART base address
    static QSTimeCtr l_tickTime;                    // keeps timetsamp at tick

    #define UART_16550_TXFIFO_DEPTH 16

    enum AppRecords {                    // application-specific trace records
        PHILO_STAT = QS_USER
    };
#endif

#define TMR_VECTOR         0x08
#define DOS_CHAIN_VECTOR   0x81

//............................................................................
extern "C" void ucosTask(void *) {
    QF::onStartup();      // start interrupts including the clock tick, NOTE01

    for (;;) {

        OSTimeDly(OS_TICKS_PER_SEC/10);                    // sleep for 1/10 s

        if (kbhit()) {                              // poll for a new keypress
            uint8_t key = (uint8_t)getch();
            switch (key) {
                case 0x00:  {                                   // Special Key
                    static uint8_t ship_pos = GAME_SHIP_Y;
                    ObjectPosEvt *ope = Q_NEW(ObjectPosEvt,
                                              PLAYER_SHIP_MOVE_SIG);
                    key = (uint8_t)getch();
                    if ((key == (uint8_t)0x48) && (ship_pos > 0x00)) {
                        --ship_pos;
                    }
                    else if ((key == (uint8_t)0x50)
                             && (ship_pos < (GAME_SCREEN_HEIGHT - 3))) {
                        ++ship_pos;
                    }
                    ope->x = (uint8_t)GAME_SHIP_X;      // x-position is fixed
                    ope->y = (uint8_t)ship_pos;
                    AO_Ship->postFIFO(ope);

                    Video::printNumAt(24, 24, Video::FGND_YELLOW, ship_pos);
                    break;
                }
                case ' ':  {                                          // Space
                    static uint16_t ntrig = 0;
                    static QEvent const fireEvt = { PLAYER_TRIGGER_SIG, 0 };
                    QF::publish(&fireEvt);

                    Video::printNumAt(47, 24, Video::FGND_YELLOW, ++ntrig);
                    break;
                }
                case 0x1B: {                                        // ESC key
                    static QEvent const quitEvt = { PLAYER_QUIT_SIG, 0 };
                    QF::publish(&quitEvt);
                    break;
                }
            }
        }
    }
}

//............................................................................
void OSTimeTickHook(void) {
    static uint8_t tickCtr = 1;
    QF::tick();
    if (--tickCtr == 0) {
        static QEvent const tickEvt = { TIME_TICK_SIG, 0 };

        tickCtr = (BSP_TICKS_PER_SEC/30 + 1);
        QF::publish(&tickEvt);                       // publish the tick event
    }
}

//............................................................................
void BSP_init(int argc, char *argv[]) {
    char const *com = "COM1";

    if (argc > 1) {
        com = argv[1];
        (void)com;                 // avoid compiler warning if QS is not used
    }
    if (!QS_INIT(com)) {                                      // initialize QS
        Q_ERROR();
    }

    Video::clearScreen(Video::BGND_LIGHT_GRAY);
    Video::clearRect( 0,  0, 80,   1, Video::BGND_RED   | Video::BGND_BLINK);
    Video::clearRect( 0,  8, 80,  24, Video::BGND_BLACK | Video::FGND_WHITE);
    Video::clearRect( 0,  7, 80,   8, Video::BGND_BLUE);
    Video::clearRect( 0, 24, 80,  25, Video::BGND_BLUE);

    Video::clearRect(24, 24, 28,  25, Video::BGND_RED | Video::BGND_BLINK);
    Video::clearRect(24, 24, 28,  25, Video::BGND_RED | Video::BGND_BLINK);

    Video::printStrAt(35,  0, Video::FGND_WHITE, "FLY 'n' SHOOT");
    Video::printStrAt(15,  2, Video::FGND_BLACK,
                     "Press UP-arrow   to move the space ship up");
    Video::printStrAt(15,  3, Video::FGND_BLACK,
                     "Press DOWN-arrow to move the space ship down");
    Video::printStrAt(15,  4, Video::FGND_BLACK,
                     "Press SPACE      to fire the missile");
    Video::printStrAt(15,  5, Video::FGND_BLACK,
                     "Press ESC        to quit the game");

    // uC/OS-II version is returned as an integer value multiplied by 100
    Video::printStrAt(15,  6, Video::FGND_BLACK, "uC/OS-II version");
    Video::printNumAt(32,  6, Video::FGND_YELLOW, OSVersion()%100);
    Video::printStrAt(32,  6, Video::FGND_YELLOW, "2.");

    Video::printStrAt( 8, 24, Video::FGND_WHITE, "Ship Position:");
    Video::printStrAt(37, 24, Video::FGND_WHITE, "Triggers:");
    Video::printStrAt(61, 24, Video::FGND_WHITE, "Score:");

    Video::clearRect(24, 24, 28,  25, Video::BGND_RED);
    Video::clearRect(47, 24, 51,  25, Video::BGND_RED);
    Video::clearRect(68, 24, 72,  25, Video::BGND_RED);
    Video::printNumAt(24, 24, Video::FGND_YELLOW, 0);
    Video::printNumAt(47, 24, Video::FGND_YELLOW, 0);
    Video::printNumAt(68, 24, Video::FGND_YELLOW, 0);

}
//............................................................................
void BSP_drawBitmap(uint8_t const *bitmap, uint8_t width, uint8_t height) {
    Video::drawBitmapAt(0, 8, bitmap, width, height);
}
//............................................................................
void BSP_drawNString(uint8_t x, uint8_t y, char const *str) {
    Video::drawStringAt(x, 8 + y*8, str);
}
//............................................................................
void BSP_updateScore(uint16_t score) {
    if (score == 0) {
        Video::clearRect(68, 24, 72,  25, Video::BGND_RED);
    }
    Video::printNumAt(68, 24, Video::FGND_YELLOW, score);
}
//............................................................................
void QF::onStartup(void) {
                                         // divisor for the 8254 timer/counter
    uint16_t count = (uint16_t)(((1193180 * 2) / OS_TICKS_PER_SEC + 1) >> 1);
    QF_INT_KEY_TYPE int_key;

    QF_INT_LOCK(int_key);

    l_dosTickISR  = getvect(TMR_VECTOR);
                // install the original DOS timer vector for uC/OS-II to chain
    setvect(DOS_CHAIN_VECTOR, l_dosTickISR);
                                           // install the uC/OS-II tick vector
    setvect(TMR_VECTOR, (void interrupt (*)(...))&OSTickISR);

    outportb(0x43, 0x36);                // use mode-3 for timer 0 in the 8254
    outportb(0x40, count & 0xFF);                 // load low  byte of timer 0
    outportb(0x40, (count >> 8) & 0xFF);          // load high byte of timer 0
    QF_INT_UNLOCK(int_key);
}
//............................................................................
void QF::onCleanup(void) {
    QF_INT_KEY_TYPE int_key;
    QF_INT_LOCK(int_key);
    outportb(0x43, 0x36);                // use mode-3 for timer 0 in the 8254
    outportb(0x40, 0);                            // load low  byte of timer 0
    outportb(0x40, 0);                            // load high byte of timer 0
    setvect(TMR_VECTOR, l_dosTickISR);      // restore the original DOS vector
    QF_INT_UNLOCK(int_key);

    QS_EXIT();                               // cleanp the QS software tracing
    _exit(0);                                                   // exit to DOS
}
//............................................................................
void OSTaskIdleHook(void) {
#ifdef Q_SPY
    if ((inportb(l_uart_base + 5) & (1 << 5)) != 0) {        // Tx FIFO empty?
        uint16_t fifo = UART_16550_TXFIFO_DEPTH;        // 16550 Tx FIFO depth
        uint8_t const *block;
        QF_INT_KEY_TYPE int_key;
        QF_INT_LOCK(int_key);
        block = QS::getBlock(&fifo);      // try to get next block to transmit
        QF_INT_UNLOCK(int_key);
        while (fifo-- != 0) {                       // any bytes in the block?
            outportb(l_uart_base + 0, *block++);
        }
    }
#endif
}
//----------------------------------------------------------------------------
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    Video::clearRect ( 0, 24, 80, 25, Video::BGND_RED);
    Video::printStrAt( 0, 24, Video::FGND_WHITE, "ASSERTION FAILED in file:");
    Video::printStrAt(26, 24, Video::FGND_YELLOW, file);
    Video::printStrAt(57, 24, Video::FGND_WHITE, "line:");
    Video::printNumAt(62, 24, Video::FGND_YELLOW, line);

    QF::stop();                                         // stop QF and cleanup
}

//----------------------------------------------------------------------------
#ifdef Q_SPY                                            // define QS callbacks

//............................................................................
static uint8_t UART_config(char const *comName, uint32_t baud) {
    switch (comName[3]) {             // Set the base address of the COMx port
        case '1': l_uart_base = (uint16_t)0x03F8; break;               // COM1
        case '2': l_uart_base = (uint16_t)0x02F8; break;               // COM2
        case '3': l_uart_base = (uint16_t)0x03E8; break;               // COM3
        case '4': l_uart_base = (uint16_t)0x02E8; break;               // COM4
        default: return (uint8_t)0;           // COM port out of range failure
    }
    baud = (uint16_t)(115200UL / baud);               // divisor for baud rate
    outportb(l_uart_base + 3, (1 << 7));      // Set divisor access bit (DLAB)
    outportb(l_uart_base + 0, (uint8_t)baud);                  // Load divisor
    outportb(l_uart_base + 1, (uint8_t)(baud >> 8));
    outportb(l_uart_base + 3, (1 << 1) | (1 << 0));   // LCR:8-bits,no p,1stop
    outportb(l_uart_base + 4, (1 << 3) | (1 << 1) | (1 << 0));   //DTR,RTS,Out
    outportb(l_uart_base + 1, 0);       // Put UART into the polling FIFO mode
    outportb(l_uart_base + 2, (1 << 2) | (1 << 0));   // FCR: enable, TX clear

    return (uint8_t)1;                                              // success
}
//............................................................................
uint8_t QS::onStartup(void const *arg) {
    static uint8_t qsBuf[1*1024];                    // buffer for Quantum Spy
    initBuf(qsBuf, sizeof(qsBuf));
    return UART_config((char const *)arg, 115200UL);
}
//............................................................................
void QS::onCleanup(void) {
}
//............................................................................
QSTimeCtr QS::onGetTime(void) {              // invoked with interrupts locked
    static uint32_t l_lastTime;
    uint32_t now;
    uint16_t count16;                            // 16-bit count from the 8254

    outportb(0x43, 0);                     // latch the 8254's counter-0 count
    count16 = (uint16_t)inportb(0x40);       // read the low byte of counter-0
    count16 += ((uint16_t)inportb(0x40) << 8);           // add on the hi byte

    now = l_tickTime + (0x10000 - count16);

    if (l_lastTime > now) {                    // are we going "back" in time?
        now += 0x10000;                  // assume that there was one rollover
    }
    l_lastTime = now;

    return (QSTimeCtr)now;
}
//............................................................................
void QS::onFlush(void) {
    uint16_t fifo = UART_16550_TXFIFO_DEPTH;            // 16550 Tx FIFO depth
    uint8_t const *block;
    QF_INT_KEY_TYPE int_key;
    QF_INT_LOCK(int_key);
    while ((block = getBlock(&fifo)) != (uint8_t *)0) {
        QF_INT_UNLOCK(int_key);
                                              // busy-wait until TX FIFO empty
        while ((inportb(l_uart_base + 5) & (1 << 5)) == 0) {
        }

        while (fifo-- != 0) {                       // any bytes in the block?
            outportb(l_uart_base + 0, *block++);
        }
        fifo = UART_16550_TXFIFO_DEPTH;         // re-load 16550 Tx FIFO depth
        QF_INT_LOCK(int_key);
    }
    QF_INT_UNLOCK(int_key);
}
#endif                                                                // Q_SPY
//----------------------------------------------------------------------------
