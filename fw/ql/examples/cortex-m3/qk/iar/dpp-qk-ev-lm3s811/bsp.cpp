//////////////////////////////////////////////////////////////////////////////
// Product: BSP for EV-LM3S811 board, QK kernel
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
#include "dpp.h"
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
static uint32_t l_delay = 0UL;    // limit for the loop counter in busyDelay()

#define PUSH_BUTTON             GPIO_PIN_4
#define USER_LED                GPIO_PIN_5

#ifdef Q_SPY

    #include "inc/hw_uart.h"
    #include "driverlib/uart.h"

    QSTimeCtr QS_tickTime_;
    QSTimeCtr QS_tickPeriod_;

    #define UART_TXFIFO_DEPTH 16

    enum AppRecords {                    // application-specific trace records
        PHILO_STAT = QS_USER
    };
#endif

//............................................................................
extern "C" void ISR_SysTick(void) {
    QK_ISR_ENTRY();                               // inform QK about ISR entry
#ifdef Q_SPY
    uint32_t dummy = HWREG(NVIC_ST_CTRL);     // clear NVIC_ST_CTRL_COUNT flag
    QS_tickTime_ += QS_tickPeriod_;          // account for the clock rollover
#endif
    QF::tick();                               // process all armed time events
    QK_ISR_EXIT();                                 // inform QK about ISR exit
}
//............................................................................
extern "C" void ISR_GPIOA(void) {
    QK_ISR_ENTRY();                               // inform QK about ISR entry
    AO_Table->postFIFO(Q_NEW(QEvent, MAX_PUB_SIG));             // for testing
    QK_ISR_EXIT();                                 // inform QK about ISR exit
}
//............................................................................
extern "C" void ISR_Nmi(void) {
    QK_ISR_ENTRY();                               // inform QK about ISR entry
    for (;;) {                                      // sit in an infinite loop
    }
    QK_ISR_EXIT();                                 // inform QK about ISR exit
}
//............................................................................
extern "C" void ISR_Fault(void) {
    QK_ISR_ENTRY();                               // inform QK about ISR entry
    for (;;) {                                      // sit in an infinite loop
    }
    QK_ISR_EXIT();                                 // inform QK about ISR exit
}
//............................................................................
extern "C" void ISR_DefaultHandler(void) {
    QK_ISR_ENTRY();                               // inform QK about ISR entry
    for (;;) {                                      // sit in an infinite loop
    }
    QK_ISR_EXIT();                                 // inform QK about ISR exit
}

//............................................................................
void BSP_init(int, char *[]) {
    // Set the clocking to run at 20MHz from the PLL.
    SysCtlClockSet(SYSCTL_SYSDIV_10  | SYSCTL_USE_PLL
                   | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);

    // Enable the peripherals used by the application.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    // Configure the LED, push button, and UART GPIOs as required.
    GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1,
                   GPIO_DIR_MODE_HW);
    GPIODirModeSet(GPIO_PORTC_BASE, PUSH_BUTTON, GPIO_DIR_MODE_IN);
    GPIODirModeSet(GPIO_PORTC_BASE, USER_LED, GPIO_DIR_MODE_OUT);
    GPIOPinWrite(GPIO_PORTC_BASE, USER_LED, 0);

    // Initialize the OSRAM OLED display.
    OSRAMInit(1);
    OSRAMStringDraw("Dining Philos", 0, 0);
    OSRAMStringDraw("0 ,1 ,2 ,3 ,4", 0, 1);

    if (QS_INIT((void *)0) == 0) {       // initialize the QS software tracing
        Q_ERROR();
    }
}
//............................................................................
void BSP_displyPhilStat(uint8_t n, char const *stat) {
    char str[2];
    str[0] = stat[0];
    str[1] = '\0';
    OSRAMStringDraw(str, (3*6*n + 6), 1);

    QS_BEGIN(PHILO_STAT, AO_Philo[n])     // application-specific record begin
        QS_U8(1, n);                                     // Philosopher number
        QS_STR(stat);                                    // Philosopher status
    QS_END()
}
//............................................................................
void BSP_driveLED(uint8_t state) {
    GPIOPinWrite(GPIO_PORTC_BASE, USER_LED, (state ? USER_LED : 0));
}
//............................................................................
void BSP_busyDelay(void) {
    uint32_t volatile i = l_delay;
    while (i-- > 0UL) {                                      // busy-wait loop
    }
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

    // GPIOA interrupt is used for testing preemptions
    IntEnable(INT_GPIOA);
    IntPrioritySet(INT_GPIOA, 0x80);

    QF_INT_UNLOCK(dummy);                 // set the interrupt flag in PRIMASK
}
//............................................................................
void QF::onCleanup(void) {
}
//............................................................................
void QK::onIdle(void) {
                            // toggle the User LED on and then off, see NOTE02
    QF_INT_LOCK(dummy);
    GPIOPinWrite(GPIO_PORTC_BASE, USER_LED, USER_LED);          // User LED on
    GPIOPinWrite(GPIO_PORTC_BASE, USER_LED, 0);                // User LED off
    QF_INT_UNLOCK(dummy);

#ifdef Q_SPY
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
}

//............................................................................
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
    static uint8_t qsBuf[4*256];                     // buffer for Quantum Spy
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

    return (uint8_t)1;                                       // return success
}
//............................................................................
void QS::onCleanup(void) {
}
//............................................................................
QSTimeCtr QS::onGetTime(void) {              // invoked with interrupts locked
    if ((HWREG(NVIC_ST_CTRL) & NVIC_ST_CTRL_COUNT) == 0) {    // COUNT no set?
        return QS_tickTime_ - (QSTimeCtr)HWREG(NVIC_ST_CURRENT);
    }
    else {        // the rollover occured, but the SysTick_ISR did not run yet
        return QS_tickTime_ + QS_tickPeriod_
               - (QSTimeCtr)HWREG(NVIC_ST_CURRENT);
    }
}
//............................................................................
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
//


