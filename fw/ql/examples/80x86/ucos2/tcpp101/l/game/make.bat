@echo off
rem ==========================================================================
rem Product: "Fly 'n' Shoot" example for uC/OS-II port, Turbo C++ 1.01
rem Last Updated for Version: 4.0.00
rem Date of the Last Update:  Apr 07, 2008
rem
rem                    Q u a n t u m     L e a P s
rem                    ---------------------------
rem                    innovating embedded systems
rem
rem Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
rem
rem This software may be distributed and modified under the terms of the GNU
rem General Public License version 2 (GPL) as published by the Free Software
rem Foundation and appearing in the file GPL.TXT included in the packaging of
rem this file. Please note that GPL Section 2[b] requires that all works based
rem on this software must also be made publicly available under the terms of
rem the GPL ("Copyleft").
rem
rem Alternatively, this software may be distributed and modified under the
rem terms of Quantum Leaps commercial licenses, which expressly supersede
rem the GPL and are specifically designed for licensees interested in
rem retaining the proprietary status of their code.
rem
rem Contact information:
rem Quantum Leaps Web site:  http://www.quantum-leaps.com
rem e-mail:                  info@quantum-leaps.com
rem ==========================================================================
setlocal

rem adjust the following path to the location where you've installed
rem the Turbo C++ 1.01 toolset...
rem
set TCPP101_DIR=c:\tools\tcpp101\bin
set CC=tcc.exe
set LINK=tlink.exe

set PATH=%TCPP101_DIR%;%PATH%

# Memory model -- large
set MM=l

set QP_INCDIR=..\..\..\..\..\include
set QP_PRTDIR=.

if "%1"=="" (
    echo default selected
    set BINDIR=%QP_PRTDIR%\dbg
    set CCFLAGS=-c -v -m%MM%
    set LINKFLAGS=/m /v @link_dbg.rsp
)
if "%1"=="rel" (
    echo rel selected
    set BINDIR=%QP_PRTDIR%\rel
    set CCFLAGS=-c -m%MM% -DNDEBUG
    set LINKFLAGS=/m @link_rel.rsp
)
if "%1"=="spy" (
    echo spy selected
    set BINDIR=%QP_PRTDIR%\spy
    set CCFLAGS=-c -v -m%MM% -DQ_SPY
    set LINKFLAGS=/m /v @link_spy.rsp
)

set CCINC=@include.rsp

rem DPP ----------------------------------------------------------------------
:dpp
set SRCDIR=.

@echo on
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\main.obj     %SRCDIR%\main.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\bsp.obj      %SRCDIR%\bsp.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\ship.obj     %SRCDIR%\ship.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\missile.obj  %SRCDIR%\missile.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\tunnel.obj   %SRCDIR%\tunnel.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\mine1.obj    %SRCDIR%\mine1.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\mine2.obj    %SRCDIR%\mine2.cpp
%CC% %CCFLAGS% %CCINC% -o%BINDIR%\video.obj    %SRCDIR%\video.cpp

%LINK% %LINKFLAGS%
@echo off

endlocal