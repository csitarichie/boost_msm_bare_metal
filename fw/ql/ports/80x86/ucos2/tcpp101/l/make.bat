@echo off
:: ===========================================================================
:: Product: QP/C++ buld script for 80x86, uC/OS-II port, Turbo C++ 1.01
:: Last Updated for Version: 4.1.00
:: Date of the Last Update:  Oct 09, 2009
::
::                    Q u a n t u m     L e a P s
::                    ---------------------------
::                    innovating embedded systems
::
:: Copyright (C) 2002-2009 Quantum Leaps, LLC. All rights reserved.
::
:: This software may be distributed and modified under the terms of the GNU
:: General Public License version 2 (GPL) as published by the Free Software
:: Foundation and appearing in the file GPL.TXT included in the packaging of
:: this file. Please note that GPL Section 2[b] requires that all works based
:: on this software must also be made publicly available under the terms of
:: the GPL ("Copyleft").
::
:: Alternatively, this software may be distributed and modified under the
:: terms of Quantum Leaps commercial licenses, which expressly supersede
:: the GPL and are specifically designed for licensees interested in
:: retaining the proprietary status of their code.
::
:: Contact information:
:: Quantum Leaps Web site:  http://www.quantum-leaps.com
:: e-mail:                  info@quantum-leaps.com
:: ===========================================================================
setlocal

:: define the TCPP101_DIR environment variable to point to the location 
:: where you've installed the Turbo C++ 1.01 toolset or adjust the following 
:: set instruction 
if "%TCPP101_DIR%"=="" set TCPP101_DIR=c:\tools\tcpp101\bin

set PATH=%TCPP101_DIR%;%PATH%
set CC=tcc.exe
set LIB=tlib.exe

:: Memory model -- large
set MM=l

set QP_INCDIR=..\..\..\..\..\include
set QP_PRTDIR=.

if "%1"=="" (
    echo default selected
    set BINDIR=%QP_PRTDIR%\dbg
    set CCFLAGS=-c -v -m%MM%
)
if "%1"=="rel" (
    echo rel selected
    set BINDIR=%QP_PRTDIR%\rel
    set CCFLAGS=-c -m%MM% -DNDEBUG
)
if "%1"=="spy" (
    echo spy selected
    set BINDIR=%QP_PRTDIR%\spy
    set CCFLAGS=-c -v -m%MM% -DQ_SPY
)

set CCINC=@inc_qp.rsp
set LIBDIR=%BINDIR%

:: uC/OS-II -----------------------------------------------------------------
:: delete the following "goto qep" line to compile uC/OS-II from sources
goto qep 

:: the uC/OS-II source directory and port directory (You need to adjust)
set UCOSSRC=\software\uCOS-II\Source
set UCOSPRT=ucos2.86

@echo on
%CC% %CCFLAGS% -I%UCOSPRT% -I%UCOSSRC% -o%UCOSPRT%\os_cpu_c.obj %UCOSPRT%\os_cpu_c.c
%CC% %CCFLAGS% -I%UCOSPRT% -I%UCOSSRC% -o%UCOSPRT%\ucos_ii.obj  %UCOSSRC%\ucos_ii.c
@echo off

:: QEP ----------------------------------------------------------------------
:qep
set SRCDIR=..\..\..\..\..\qep\source

@echo on
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qep.obj      %SRCDIR%\qep.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qfsm_ini.obj %SRCDIR%\qfsm_ini.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qfsm_dis.obj %SRCDIR%\qfsm_dis.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qhsm_ini.obj %SRCDIR%\qhsm_ini.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qhsm_dis.obj %SRCDIR%\qhsm_dis.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qhsm_top.obj %SRCDIR%\qhsm_top.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qhsm_tra.obj %SRCDIR%\qhsm_tra.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qhsm_in.obj  %SRCDIR%\qhsm_in.cpp

erase %LIBDIR%\qep.lib
%LIB% %LIBDIR%\qep +%BINDIR%\qep
%LIB% %LIBDIR%\qep +%BINDIR%\qfsm_ini
%LIB% %LIBDIR%\qep +%BINDIR%\qfsm_dis
%LIB% %LIBDIR%\qep +%BINDIR%\qhsm_ini
%LIB% %LIBDIR%\qep +%BINDIR%\qhsm_dis
%LIB% %LIBDIR%\qep +%BINDIR%\qhsm_top
%LIB% %LIBDIR%\qep +%BINDIR%\qhsm_tra
%LIB% %LIBDIR%\qep +%BINDIR%\qhsm_in
@echo off

:: QF -----------------------------------------------------------------------
:qf
set SRCDIR=..\..\..\..\..\qf\source

@echo on
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qa_defer.obj %SRCDIR%\qa_defer.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qa_run.obj   %SRCDIR%\qa_run.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qa_sub.obj   %SRCDIR%\qa_sub.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qa_usub.obj  %SRCDIR%\qa_usub.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qa_usuba.obj %SRCDIR%\qa_usuba.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qf_act.obj   %SRCDIR%\qf_act.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qf_gc.obj    %SRCDIR%\qf_gc.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qf_log2.obj  %SRCDIR%\qf_log2.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qf_new.obj   %SRCDIR%\qf_new.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qf_pool.obj  %SRCDIR%\qf_pool.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qf_psini.obj %SRCDIR%\qf_psini.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qf_pspub.obj %SRCDIR%\qf_pspub.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qf_pwr2.obj  %SRCDIR%\qf_pwr2.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qf_tick.obj  %SRCDIR%\qf_tick.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qte_ctor.obj %SRCDIR%\qte_ctor.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qte_arm.obj  %SRCDIR%\qte_arm.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qte_darm.obj %SRCDIR%\qte_darm.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qte_rarm.obj %SRCDIR%\qte_rarm.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qf_port.obj  src\qf_port.cpp

erase %LIBDIR%\qf.lib
%LIB% %LIBDIR%\qf +%BINDIR%\qa_defer
%LIB% %LIBDIR%\qf +%BINDIR%\qa_get_
%LIB% %LIBDIR%\qf +%BINDIR%\qa_run
%LIB% %LIBDIR%\qf +%BINDIR%\qa_sub
%LIB% %LIBDIR%\qf +%BINDIR%\qa_usub
%LIB% %LIBDIR%\qf +%BINDIR%\qa_usuba
%LIB% %LIBDIR%\qf +%BINDIR%\qf_act
%LIB% %LIBDIR%\qf +%BINDIR%\qf_gc
%LIB% %LIBDIR%\qf +%BINDIR%\qf_log2
%LIB% %LIBDIR%\qf +%BINDIR%\qf_new
%LIB% %LIBDIR%\qf +%BINDIR%\qf_pool
%LIB% %LIBDIR%\qf +%BINDIR%\qf_psini
%LIB% %LIBDIR%\qf +%BINDIR%\qf_pspub
%LIB% %LIBDIR%\qf +%BINDIR%\qf_pwr2
%LIB% %LIBDIR%\qf +%BINDIR%\qf_tick
%LIB% %LIBDIR%\qf +%BINDIR%\qte_ctor
%LIB% %LIBDIR%\qf +%BINDIR%\qte_arm
%LIB% %LIBDIR%\qf +%BINDIR%\qte_darm
%LIB% %LIBDIR%\qf +%BINDIR%\qte_rarm
%LIB% %LIBDIR%\qf +%BINDIR%\qf_port
@echo off

if not "%1"=="spy" goto clean

:: QS -----------------------------------------------------------------------
:qs
set SRCDIR=..\..\..\..\..\qs\source

@echo on
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qs.obj      %SRCDIR%\qs.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qs_.obj     %SRCDIR%\qs_.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qs_blk.obj  %SRCDIR%\qs_blk.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qs_byte.obj %SRCDIR%\qs_byte.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qs_f32.obj  %SRCDIR%\qs_f32.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qs_f64.obj  %SRCDIR%\qs_f64.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qs_mem.obj  %SRCDIR%\qs_mem.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\qs_str.obj  %SRCDIR%\qs_str.cpp

erase %LIBDIR%\qs.lib
%LIB% %LIBDIR%\qs +%BINDIR%\qs.obj
%LIB% %LIBDIR%\qs +%BINDIR%\qs_.obj
%LIB% %LIBDIR%\qs +%BINDIR%\qs_blk.obj
%LIB% %LIBDIR%\qs +%BINDIR%\qs_byte.obj
%LIB% %LIBDIR%\qs +%BINDIR%\qs_f32.obj
%LIB% %LIBDIR%\qs +%BINDIR%\qs_f64.obj
%LIB% %LIBDIR%\qs +%BINDIR%\qs_mem.obj
%LIB% %LIBDIR%\qs +%BINDIR%\qs_str.obj
@echo off

:clean
@echo off

erase %BINDIR%\*.obj
erase %LIBDIR%\*.bak

endlocal