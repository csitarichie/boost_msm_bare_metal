//////////////////////////////////////////////////////////////////////////////
// Product: qfsgen utility
// Last Updated for Version: 4.1.03
// Date of the Last Update:  Mar 10, 2010
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
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <direct.h>
#include "filesearch.h"

static char const *l_rootDir = "fs";
static FILE *l_file;
static bool  l_noHtml = false;
static int   l_nFiles;
static char  l_prevFile[256];

//............................................................................
static void dumpStrHex(char const *s) {
    int i = 0;
    while (*s != '\0') {
        if (i == 0) {
            fprintf(l_file, "    ");
        }
        fprintf(l_file, "0x%02X, ", ((unsigned)*s & 0xFF));
        ++i;
        if (i == 10) {
            fprintf(l_file, "\n");
            i = 0;
        }
        ++s;
    }
    if (i == 0) {
        fprintf(l_file, "    ");
    }
}
//............................................................................
int isMatching(struct _finddata_t const *fdata) {
    char const *fname = fdata->name;
                                            // skip SVN, CVS, or special files
    if ((strcmp(fname, ".svn") == 0)
        || (strcmp(fname, "CVS") == 0)
        || (strcmp(fname, "thumbs.db") == 0)
        || (strcmp(fname, "filelist.txt") == 0)
        || (strcmp(fname, "dirlist.txt") == 0))
    {
        return 0;
    }
    else {
        return 1;
    }
}
//............................................................................
void onMatchFound(char const *fullPath, int /*flags*/) {
    static char buf[1024];                                   // working buffer

    char const *fname = strstr(fullPath, l_rootDir);
    if (fname == 0) {
        return;
    }
    fname = strchr(fname, '\\');
    if (fname == 0) {
        return;
    }
    FILE *fin = fopen(fullPath, "rb");              // open for reading binary
    if (fin == (FILE *)0) {
        return;
    }

    ++l_nFiles;
                     // copy the file name into the buffer and masage it a bit
    char const *s = fname;
    char *d = buf;
    while (*s != '\0') {
        if (*s == '\\') {
            *d = '/';
        }
        else {
            *d = *s;
        }
        ++s;
        ++d;
    }
    *d = '\0';
    printf("    Adding: %s...\n", buf);
    fprintf(l_file, "/* %s */\n", buf);

                              // defive the C-variable name from the file name
    char fvar[256];
    s = buf + 1;                                         // skip the first '/'
    d = fvar;
    while (*s != '\0') {
        if ((*s == '/')  || (*s == '.')) {
            *d = '_';
        }
        else {
            *d = *s;
        }
        ++s;
        ++d;
    }
    *d = '\0';

    fprintf(l_file, "static unsigned char const data_%s[] = {\n", fvar);
    fprintf(l_file, "    /* name: */\n");
    dumpStrHex(buf);                                     // dump the file name
    fprintf(l_file, "0x00,\n");                              // zero-terminate

    if (!l_noHtml) {      // encode the HTML header, if option -h NOT provided
        if (strstr(fname, "404") != 0) {
            strcpy(buf, "HTTP/1.0 404 File not found\r\n");
        }
        else {
            strcpy(buf, "HTTP/1.0 200 OK\r\n");
        }
        strcat(buf, "Server: QP-lwIP (http://www.state-machine.com)\r\n");

                                                   // analyze the file type...
        s = strrchr(fname, '.');
        if (s != 0) {                                            // '.' found?
            if ((strcmp(s, ".htm") == 0)
                || (strcmp(s, ".html") == 0))           // .htm or .html files
            {
                strcat(buf, "Content-type: text/html\r\n");
            }
            else if ((strcmp(s, ".shtm") == 0)
                     || (strcmp(s, ".shtml") == 0))   // .shtm or .shtml files
            {
                strcat(buf, "Content-type: text/html\r\n"
                            "Pragma: no-cache\r\n\r\n");
            }
            else if (strcmp(s, ".css") == 0) {
                strcat(buf, "Content-type: text/css\r\n");
            }
            else if (strcmp(s, ".gif") == 0) {
                strcat(buf, "Content-type: image/gif\r\n");
            }
            else if (strcmp(s, ".png") == 0) {
                strcat(buf, "Content-type: image/png\r\n");
            }
            else if (strcmp(s, ".jpg") == 0) {
                strcat(buf, "Content-type: image/jpeg\r\n");
            }
            else if (strcmp(s, ".bmp") == 0) {
                strcat(buf, "Content-type: image/bmp\r\n");
            }
            else if (strcmp(s, ".class") == 0) {
                strcat(buf, "Content-type: application/octet-stream\r\n");
            }
            else if (strcmp(s, ".ram") == 0) {
                strcat(buf, "Content-type: audio/x-pn-realaudio\r\n");
            }
            else {
                strcat(buf, "Content-type: text/plain\r\n");
            }
        }
        else {
            strcat(buf, "Content-type: text/plain\r\n");
        }
        strcat(buf, "\r\n");

        fprintf(l_file, "    /* HTML header: */\n");
        dumpStrHex(buf);
        fprintf(l_file, "\n");
    }

    fprintf(l_file, "    /* data: */\n");
    int len = 0;
    int nBytes;
    int i = 0;
    while ((nBytes = fread(buf, 1, sizeof(buf), fin)) != 0) {
        len += nBytes;
        char const *s = buf;
        while (nBytes-- != 0) {
            if (i == 0) {
                fprintf(l_file, "    ");
            }
            fprintf(l_file, "0x%02X, ", ((unsigned)*s & 0xFF));
            ++i;
            if (i == 10) {
                fprintf(l_file, "\n");
                i = 0;
            }
            ++s;
        }
    }
    fprintf(l_file, "\n};\n\n");
    fclose(fin);

    fprintf(l_file, "struct fsdata_file const file_%s[] = {\n    {\n", fvar);
    fprintf(l_file, "        %s,\n",      l_prevFile);
    fprintf(l_file, "        data_%s,\n", fvar);
    fprintf(l_file, "        data_%s + %d,\n", fvar, strlen(fvar) + 2);
    fprintf(l_file, "        sizeof(data_%s) - %d\n", fvar, strlen(fvar) + 2);
    fprintf(l_file, "    }\n};\n\n");
    sprintf(l_prevFile, "file_%s", fvar);
}
//............................................................................
int main(int argc, char *argv[]) {
    char const *fileName = "fsdata.h";

    printf("qfsgen 4.1.03 (c) Quantum Leaps, LLC. www.quantum-leaps.com\n"
           "Usage: qfsgen [root-dir [output-file]] [-h]\n"
           "      -h suppresses generation of the HTML headers\n\n");

                                                  // parse the command line...
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0) {
            l_noHtml = true;
        }
        else {
            l_rootDir = argv[1];
        }
        if (argc > 2) {
            if (strcmp(argv[2], "-h") == 0) {
                l_noHtml = true;
            }
            else {
                fileName = argv[2];
            }
            if (argc > 3) {
                if (strcmp(argv[3], "-h") == 0) {
                    l_noHtml = true;
                }
            }
        }
    }

    l_file = fopen(fileName, "w");
    if (l_file == 0) {
        printf("The file %s could not be opened for writing.", fileName);
        return -1;
    }

    fprintf(l_file, "/* This file has been generated"
                    "with the qfsgen utility. */\n\n");
    l_nFiles = 0;
    strcpy(l_prevFile, "(struct fsdata_file *)0");
    _chdir(l_rootDir);
    filesearch();             // search through the file-system directory tree
    fprintf(l_file, "#define FS_ROOT %s\n\n", l_prevFile);
    fprintf(l_file, "#define FS_NUMFILES %d\n", l_nFiles);
    fclose(l_file);

    printf("\nFiles processed: %d", l_nFiles);
    return 0;
}

