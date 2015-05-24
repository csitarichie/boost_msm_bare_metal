//////////////////////////////////////////////////////////////////////////////
// Product: Product: "Fly'n'Shoot" game, BSP for LM3S811
// Last Updated for Version: 4.1.00
// Date of the Last Update:  Oct 09, 2009
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2009 Quantum Leaps, LLC. All rights reserved.
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

extern "C" {
    #include "inc/hw_types.h"                                  // for tBoolean
    #include "osram96x16x1.h"                                   // OLED driver

    #include "inc/hw_ints.h"
    #include "inc/hw_memmap.h"
    #include "inc/hw_nvic.h"
    #include "driverlib/adc.h"
    #include "driverlib/gpio.h"
    #include "driverlib/interrupt.h"
    #include "driverlib/timer.h"
    #include "driverlib/sysctl.h"
    #include "driverlib/systick.h"
}

Q_DEFINE_THIS_FILE

// Local-scope objects -------------------------------------------------------
#define PUSH_BUTTON             GPIO_PIN_4
#define USER_LED                GPIO_PIN_5

#ifdef Q_SPY

#include "inc/hw_uart.h"
#include "driverlib/uart.h"

QSTimeCtr QS_tickTime_;
QSTimeCtr QS_tickPeriod_;

#define UART_TXFIFO_DEPTH 16

#endif

//............................................................................
extern "C" void ISR_SysTick(void) {
    static QEvent const tickEvt = { TIME_TICK_SIG, 0 };
#ifdef Q_SPY
    uint32_t dummy = HWREG(NVIC_ST_CTRL);     // clear NVIC_ST_CTRL_COUNT flag
    QS_tickTime_ += QS_tickPeriod_;          // account for the clock rollover
#endif

    QF::tick();                               // process all armed time events
    QF::publish(&tickEvt);        // publish the tick event to all subscribers
}
//............................................................................
extern "C" void ISR_ADC(void) {
    static uint32_t adcLPS = 0;               // Low-Pass-Filtered ADC reading
    static uint32_t wheel = 0;                      // the last wheel position

    static uint32_t btn_debounced  = 0;
    static uint8_t  debounce_state = 0;

    unsigned long tmp;

    ADCIntClear(ADC_BASE, 3);                       // clear the ADC interrupt
    ADCSequenceDataGet(ADC_BASE, 3, &tmp);       // read the data from the ADC

     // 1st order low-pass filter: time constant ~= 2^n samples
     // TF = (1/2^n)/(z-((2^n - 1)/2^n)),
     // eg, n=3, y(k+1) = y(k) - y(k)/8 + x(k)/8 => y += (x - y)/8
    adcLPS += (((int)tmp - (int)adcLPS + 4) >> 3);          // Low-Pass-Filter

       // compute the next position of the wheel
    tmp = (((1 << 10) - adcLPS)*(BSP_SCREEN_HEIGHT - 2)) >> 10;

    if (tmp != wheel) {                      // did the wheel position change?
        ObjectPosEvt *ope = Q_NEW(ObjectPosEvt, PLAYER_SHIP_MOVE_SIG);
        ope->x = (uint8_t)GAME_SHIP_X;                  // x-position is fixed
        ope->y = (uint8_t)tmp;
        AO_Ship->postFIFO(ope);                            // post to the Ship
        wheel = tmp;                    // save the last position of the wheel
    }

    tmp = GPIOPinRead(GPIO_PORTC_BASE, PUSH_BUTTON);      // read the push btn
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
                    static QEvent const fireEvt = { PLAYER_TRIGGER_SIG, 0 };
                    QF::publish(&fireEvt);       // publish to all subscribers
                }
            }
            debounce_state = 0;                  // transition back to state 0
            break;
    }
}
//............................................................................
extern "C" void ISR_Nmi(void) {
    for (;;) {                                      // sit in an infinite loop
    }
}
//............................................................................
extern "C" void ISR_Fault(void) {
    for (;;) {                                      // sit in an infinite loop
    }
}
//............................................................................
extern "C" void ISR_DefaultHandler(void) {
    for (;;) {                                      // sit in an infinite loop
    }
}

//............................................................................
void BSP_init(int, char *[]) {

    // Set the clocking to run at 20MHz from the PLL.
    SysCtlClockSet(SYSCTL_SYSDIV_10  | SYSCTL_USE_PLL
                   | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);

    // Enable the peripherals used by the application.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    // Configure the ADC to sample the potentiometer when the timer expires.
    // After sampling, the ADC will interrupt the processor; this is used as
    // the system time tick.
    ADCSequenceConfigure(ADC_BASE, 3, ADC_TRIGGER_TIMER, 0);
    ADCSequenceStepConfigure(ADC_BASE, 3, 0,
                             ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC_BASE, 3);

    // Configure the timer to generate triggers to the ADC to sample the
    // potentiometer.
    TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);
    TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 120);
    TimerControlStall(TIMER1_BASE, TIMER_A, true);
    TimerControlTrigger(TIMER1_BASE, TIMER_A, true);

    // Configure the LED, push button, and UART GPIOs as required.
    GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1,
                   GPIO_DIR_MODE_HW);
    GPIODirModeSet(GPIO_PORTC_BASE, PUSH_BUTTON, GPIO_DIR_MODE_IN);
    GPIODirModeSet(GPIO_PORTC_BASE, USER_LED, GPIO_DIR_MODE_OUT);
    GPIOPinWrite(GPIO_PORTC_BASE, USER_LED, 0);

    // Initialize the OSRAM OLED display.
    OSRAMInit(1);

    if (QS_INIT((void *)0) == 0) {       // initialize the QS software tracing
        Q_ERROR();
    }
}
//............................................................................
void BSP_drawBitmap(uint8_t const *bitmap, uint8_t width, uint8_t height) {
    OSRAMImageDraw(bitmap, 0, 0, width, (height >> 3));
}
//............................................................................
void BSP_drawNString(uint8_t x, uint8_t y, char const *str) {
    OSRAMStringDraw(str, x, y);
}
//............................................................................
void BSP_updateScore(uint16_t score) {
       // not implemented on the EV-LM3S811 board
}
//............................................................................
void QF::onStartup(void) {

    // Set up and enable the SysTick timer.  It will be used as a reference
    // for delay loops in the interrupt handlers.  The SysTick timer period
    // will be set up for BSP_TICKS_PER_SEC.
    SysTickPeriodSet(SysCtlClockGet() / BSP_TICKS_PER_SEC);
    SysTickEnable();
    IntPrioritySet(FAULT_SYSTICK, 0xC0);        // set the priority of SysTick
    SysTickIntEnable();                       // Enable the SysTick interrupts

    ADCIntEnable(ADC_BASE, 3);
    IntEnable(INT_ADC3);
    TimerEnable(TIMER1_BASE, TIMER_A);

    QF_INT_UNLOCK(dummy);                 // set the interrupt flag in PRIMASK
}
//............................................................................
void QF::onCleanup(void) {
}
//............................................................................
void QF::onIdle(void) {

    // toggle the User LED on and then off, see NOTE02
    GPIOPinWrite(GPIO_PORTC_BASE, USER_LED, USER_LED);          // User LED on
    GPIOPinWrite(GPIO_PORTC_BASE, USER_LED, 0);                // User LED off

#ifdef Q_SPY
    QF_INT_UNLOCK(dummy);
    if ((HWREG(UART0_BASE + UART_O_FR) & UART_FR_TXFE) != 0) {     // TX done?
        uint16_t fifo = UART_TXFIFO_DEPTH;          // max bytes we can accept
        uint8_t const *block;
        QF_INT_LOCK(dummy);
        block = QS::getBlock(&fifo);      // try to get next block to transmit
        QF_INT_UNLOCK(dummy);
        while (fifo-- != 0) {                       // any bytes in the block?
            HWREG(UART0_BASE + UART_O_DR) = *block++;     // put into the FIFO
        }
    }
#elif defined NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular Cortex-M3 MCU.
    __asm("WFI");                                        // Wait-For-Interrupt
#endif
    QF_INT_UNLOCK(dummy);                      // always unlock the interrupts
}
//----------------------------------------------------------------------------
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR, int) {
    QF_INT_LOCK(dummy);         // make sure that all interrupts are disabled
    for (;;) {          // NOTE: replace the loop with reset for final version
    }
}
// error routine that is called if the Luminary library encounters an error...
void __error__(char *pcFilename, unsigned long ulLine) {
    Q_onAssert(pcFilename, ulLine);
}


//----------------------------------------------------------------------------
#ifdef Q_SPY
//............................................................................
uint8_t QS::onStartup(void const *arg) {
    static uint8_t qsBuf[6*256];                     // buffer for Quantum Spy
    initBuf(qsBuf, sizeof(qsBuf));

                                    // enable the peripherals used by the UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

                            // configure the UART for 115,200, 8-N-1 operation
    UARTConfigSet(UART0_BASE, 115200, (UART_CONFIG_WLEN_8 |
                                       UART_CONFIG_STOP_ONE |
                                       UART_CONFIG_PAR_NONE));
    UARTEnable(UART0_BASE);           // set UARTEN, TXE, RXE and enable FIFOs

    QS_tickPeriod_ = (QSTimeCtr)(SysCtlClockGet() / BSP_TICKS_PER_SEC);
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
   //.........................................................................
QSTimeCtr QS::onGetTime(void) {              // invoked with interrupts locked
    if ((HWREG(NVIC_ST_CTRL) & NVIC_ST_CTRL_COUNT) == 0) {    // COUNT no set?
        return QS_tickTime_ - (QSTimeCtr)HWREG(NVIC_ST_CURRENT);
    }
    else {        // the rollover occured, but the SysTick_ISR did not run yet
        return QS_tickTime_ + QS_tickPeriod_
               - (QSTimeCtr)HWREG(NVIC_ST_CURRENT);
    }
}
   //.........................................................................
void QS::onFlush(void) {
    uint16_t fifo = UART_TXFIFO_DEPTH;                        // Tx FIFO depth
    uint8_t const *block;
    QF_INT_LOCK(dummy);
    while ((block = getBlock(&fifo)) != (uint8_t *)0) {
        QF_INT_UNLOCK(dummy);
                                              // busy-wait until TX FIFO empty
        while ((HWREG(UART0_BASE + UART_O_FR) & UART_FR_TXFE) == 0) {
        }

        while (fifo-- != 0) {                       // any bytes in the block?
            HWREG(UART0_BASE + UART_O_DR) = *block++;     // put into the FIFO
        }
        fifo = UART_TXFIFO_DEPTH;                 // re-load the Tx FIFO depth
        QF_INT_LOCK(dummy);
    }
    QF_INT_UNLOCK(dummy);
}
#endif                                                                // Q_SPY
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// NOTE01:
// The QF::onIdle() callback is called with interrupts locked, because the
// determination of the idle condition might change by any interrupt posting
// an event. QF::onIdle() must internally unlock interrupts, ideally
// atomically with putting the CPU to the power-saving mode.
//
// NOTE02:
// The User LED is used to visualize the idle loop activity. The brightness
// of the LED is proportional to the frequency of invcations of the idle loop.
// Please note that the LED is toggled with interrupts locked, so no interrupt
// execution time contributes to the brightness of the User LED.

