//////////////////////////////////////////////////////////////////////////////
// Product: BSP for DPP application with lwIP on EV-LM3S9665 board
// Last Updated for Version: 4.1.03
// Date of the Last Update:  Mar 11, 2010
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2010 Quantum Leaps, LLC. All rights reserved.
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
#include "qp_port.h"                                    // QP port header file
#include "dpp.h"                      // application events and active objects
#include "bsp.h"                          // Board Support Package header file

extern "C" {
    #include "lm3s_cmsis.h"
}

Q_DEFINE_THIS_FILE

#define USER_LED           (1 << 0)
#define USER_BTN           (1 << 1)

enum ISR_Priorities {      // ISR priorities starting from the highest urgency
    SYSTICK_PRIO,
    ETHERNET_PRIO,
    // ...
};

#ifdef Q_SPY

    QSTimeCtr QS_tickTime_;
    QSTimeCtr QS_tickPeriod_;

    #define UART_BAUD_RATE      115200
    #define UART_TXFIFO_DEPTH   16
    #define UART_FR_TXFE        (1 << 7)
    #define SYSTICK_COUNTFLAG   (1 << 8)

#endif

//............................................................................
extern "C" void SysTick_Handler(void) {
    static uint32_t btn_debounced  = 0;
    static uint8_t  debounce_state = 0;
    uint32_t tmp;

    QK_ISR_ENTRY();                               // inform QK about ISR entry

#ifdef Q_SPY
    tmp = SysTick->CTRL;                      // clear NVIC_ST_CTRL_COUNT flag
    QS_tickTime_ += QS_tickPeriod_;          // account for the clock rollover
#endif

    QF::tick();                               // process all armed time events

    tmp = GPIOF->DATA_Bits[USER_BTN];                  // read the User Button
    switch (debounce_state) {
        case 0:
            if (tmp != btn_debounced) {
                debounce_state = 1;            // transition to the next state
            }
            break;
        case 1:
            if (tmp != btn_debounced) {
                debounce_state = 2;            // transition to the next state
            }
            else {
                debounce_state = 0;              // transition back to state 0
            }
            break;
        case 2:
            if (tmp != btn_debounced) {
                debounce_state = 3;            // transition to the next state
            }
            else {
                debounce_state = 0;              // transition back to state 0
            }
            break;
        case 3:
            if (tmp != btn_debounced) {
                btn_debounced = tmp;        // save the debounced button value
                if (tmp == 0) {                    // is the button depressed?
                    static QEvent const bd = { BTN_DOWN_SIG, 0 };
                    QF::publish(&bd);            // publish to all subscribers
                }
                else {
                    static QEvent const bu = { BTN_UP_SIG, 0 };
                    QF::publish(&bu);            // publish to all subscribers
                }
            }
            debounce_state = 0;                  // transition back to state 0
            break;
    }

    QK_ISR_EXIT();                                 // inform QK about ISR exit
}

//............................................................................
void BSP_init(void) {
       // set the system clock as specified in lm3s_config.h (20MHz from PLL)
    SystemInit();

    // Enable Port F for Ethernet LEDs.
    //  LED0        Bit 3   Output
    //  LED1        Bit 2   Output
    //
    SYSCTL->RCGC2 |= (1 << 5);                       // enable clock to GPIOF
    __NOP();
    __NOP();

                                     // configure the pin driving the User LED
    GPIOF->DIR   |=  USER_LED;                        // set direction: output
    GPIOF->DR2R  |=  USER_LED;
    GPIOF->DEN   |=  USER_LED;
    GPIOF->AMSEL &= ~USER_LED;
    GPIOF->DATA_Bits[USER_LED] = 0;                        // turn the LED off

                                 // configure the pin connected to the Buttons
    GPIOF->DIR   &= ~USER_BTN;                         // set direction: input
    GPIOF->DR2R  |=  USER_BTN;
    GPIOF->ODR   &= ~USER_BTN;
    GPIOF->PUR   |=  USER_BTN;
    GPIOF->PDR   &= ~USER_BTN;
    GPIOF->DEN   |=  USER_BTN;
    GPIOF->AMSEL &= ~USER_BTN;

    if (QS_INIT((void *)0) == 0) {       // initialize the QS software tracing
        Q_ERROR();
    }
}
//............................................................................
void QF::onStartup(void) {
                 // set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate
    SysTick_Config(SystemFrequency / BSP_TICKS_PER_SEC);

                          // set priorities of all interrupts in the system...
    NVIC_SetPriority(SysTick_IRQn,   SYSTICK_PRIO);
    NVIC_SetPriority(Ethernet_IRQn,  ETHERNET_PRIO);

    NVIC_EnableIRQ(Ethernet_IRQn);            // enable the Ethernet Interrupt
}
//............................................................................
void QF::onCleanup(void) {
}
//............................................................................
void QK::onIdle(void) {

    // toggle the User LED on and then off, see NOTE01
    QF_INT_LOCK(dummy);
    GPIOF->DATA_Bits[USER_LED] = USER_LED;            // turn the User LED on
    GPIOF->DATA_Bits[USER_LED] = 0;                   // turn the User LED off
    QF_INT_UNLOCK(dummy);

#ifdef Q_SPY
    if ((UART0->FR & UART_FR_TXFE) != 0) {                         // TX done?
        uint16_t fifo = UART_TXFIFO_DEPTH;          // max bytes we can accept
        uint8_t const *block;
        QF_INT_LOCK(dummy);
        block = QS::getBlock(&fifo);      // try to get next block to transmit
        QF_INT_UNLOCK(dummy);
        while (fifo-- != 0) {                       // any bytes in the block?
            UART0->DR = *block++;                         // put into the FIFO
        }
    }
#elif defined NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular MCU.
    //
    __WFI();                                             // Wait-For-Interrupt
#endif
}

//............................................................................
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                      // avoid compiler warning
    (void)line;                                      // avoid compiler warning
    QF_INT_LOCK(dummy);         // make sure that all interrupts are disabled
    for (;;) {          // NOTE: replace the loop with reset for final version
    }
}
//............................................................................
// error routine that is called if the CMSIS library encounters an error
extern "C" void assert_failed(char const *file, int line) {
    Q_onAssert(file, line);
}


//----------------------------------------------------------------------------
#ifdef Q_SPY
//............................................................................
uint8_t QS::onStartup(void const *arg) {
    static uint8_t qsBuf[6*256];                     // buffer for Quantum Spy
    uint32_t tmp;
    initBuf(qsBuf, sizeof(qsBuf));

                                   // enable the peripherals used by the UART0
    SYSCTL->RCGC1 |= (1 << 0);                        // enable clock to UART0
    SYSCTL->RCGC2 |= (1 << 0);                        // enable clock to GPIOA
    __NOP();                                     // wait after enabling clocks
    __NOP();
    __NOP();

                                    // configure UART0 pins for UART operation
    tmp = (1 << 0) | (1 << 1);
    GPIOA->DIR   &= ~tmp;
    GPIOA->AFSEL |=  tmp;
    GPIOA->DR2R  |=  tmp;          // set 2mA drive, DR4R and DR8R are cleared
    GPIOA->SLR   &= ~tmp;
    GPIOA->ODR   &= ~tmp;
    GPIOA->PUR   &= ~tmp;
    GPIOA->PDR   &= ~tmp;
    GPIOA->DEN   |=  tmp;
    GPIOA->AMSEL &= ~tmp;

              // configure the UART for the desired baud rate, 8-N-1 operation
    tmp = (((SystemFrequency * 8) / UART_BAUD_RATE) + 1) / 2;
    UART0->IBRD   = tmp / 64;
    UART0->FBRD   = tmp % 64;
    UART0->LCRH   = 0x60;                         // configure 8-bit operation
    UART0->LCRH  |= 0x10;                                      // enable FIFOs
    UART0->CTL   |= (1 << 0) | (1 << 8) | (1 << 9);

    QS_tickPeriod_ = SystemFrequency / BSP_TICKS_PER_SEC;
    QS_tickTime_ = QS_tickPeriod_;           // to start the timestamp at zero

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

    return (uint8_t)1;                                       // return success
}
//............................................................................
void QS::onCleanup(void) {
}
//............................................................................
QSTimeCtr QS::onGetTime(void) {              // invoked with interrupts locked
    if ((SysTick->CTRL & SYSTICK_COUNTFLAG) == 0) {       // COUNTFLAG no set?
        return QS_tickTime_ - (QSTimeCtr)SysTick->VAL;
    }
    else {        // the rollover occured, but the SysTick_ISR did not run yet
        return QS_tickTime_ + QS_tickPeriod_ - (QSTimeCtr)SysTick->VAL;
    }
}
//............................................................................
void QS::onFlush(void) {
    uint16_t fifo = UART_TXFIFO_DEPTH;                        // Tx FIFO depth
    uint8_t const *block;
    QF_INT_LOCK(dummy);
    while ((block = QS::getBlock(&fifo)) != (uint8_t *)0) {
        QF_INT_UNLOCK(dummy);
                                              // busy-wait until TX FIFO empty
        while ((UART0->FR & UART_FR_TXFE) == 0) {
        }

        while (fifo-- != 0) {                       // any bytes in the block?
            UART0->DR = *block++;                      // put into the TX FIFO
        }
        fifo = UART_TXFIFO_DEPTH;                 // re-load the Tx FIFO depth
        QF_INT_LOCK(dummy);
    }
    QF_INT_UNLOCK(dummy);
}
#endif                                                                // Q_SPY
//--------------------------------------------------------------------------*/

//////////////////////////////////////////////////////////////////////////////
// NOTE01:
// The User LED is used to visualize the idle loop activity. The brightness
// of the LED is proportional to the frequency of invcations of the idle loop.
// Please note that the LED is toggled with interrupts locked, so no interrupt
// execution time contributes to the brightness of the User LED.
//
