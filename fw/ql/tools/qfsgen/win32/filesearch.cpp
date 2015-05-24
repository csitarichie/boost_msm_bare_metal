//////////////////////////////////////////////////////////////////////////////
// Product: filesarch utility
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
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <windows.h>
#include "filesearch.h"

void filesearch(void) {
    static char buffer[1024];
    struct _finddata_t fdata;
    long hnd;
    int flags;

    hnd = _findfirst("*", &fdata);    // set _findfirst to find everthing
    if (hnd == -1) {                     // if handle fails, drive is empty...
        return;
    }
    // get first entity on drive - check if it's a directory
    if ((::GetFileAttributes(fdata.name) & FILE_ATTRIBUTE_DIRECTORY) != 0
        && (::GetFileAttributes(fdata.name) & FILE_ATTRIBUTE_HIDDEN) == 0)
    {  // if so, change to that directory and recursively call SearchDirectory
        if (*fdata.name != '.') {
            _chdir(fdata.name);
            filesearch();
            _chdir("..");                    // go back up one directory level
        }
    }
    else {
                    // if it's not a directory and it matches what you want...
        flags = isMatching(&fdata);
        if (flags != 0) {
            _getcwd(buffer,  sizeof(buffer));
            strcat(buffer, "\\");
            strcat(buffer, fdata.name);
            onMatchFound(buffer, flags);
        }
    }

    while (!(_findnext(hnd,&fdata))) {
        if ((::GetFileAttributes(fdata.name) & FILE_ATTRIBUTE_DIRECTORY) != 0
            && (::GetFileAttributes(fdata.name) & FILE_ATTRIBUTE_HIDDEN) == 0)
        {
            if (*fdata.name != '.') {
                _chdir(fdata.name);
                filesearch();
                _chdir("..");
            }
        }
        else {
            flags = isMatching(&fdata);
            if (flags != 0) {
                _getcwd(buffer,  sizeof(buffer));
                strcat(buffer, "\\");
                strcat(buffer, fdata.name);
                onMatchFound(buffer, flags);
            }
        }
    }
    _findclose(hnd);
}

