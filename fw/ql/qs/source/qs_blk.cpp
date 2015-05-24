//////////////////////////////////////////////////////////////////////////////
// Product: QS/C++
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
#include "qs_pkg.h"
#include <stdio.h>

/// \file
/// \ingroup qs
/// \brief QS::getBlock() implementation

//............................................................................
// get up to *pn bytes of contiguous memory
uint8_t const *QS::getBlock(uint16_t *pNbytes) {
    /* printf("before QS_end_:%4u, QS_head_:%4u, QS_tail_:%4u, QS_used_:%4u, QS_seq_:%4u, QS_full_%4u.\n"
            , QS_end_  ///< offset of the end of the ring buffer
            , QS_head_ ///< offset to where next byte will be inserted
            , QS_tail_ ///< offset of where next event will be extracted
            , QS_used_ ///< number of bytes currently in the ring buffer
            , QS_seq_  ///< the record sequence number
            , QS_full_ ///< the ring buffer is temporarily full
            );*/
    if (QS_used_ == (QSCtr)0) {
        *pNbytes = (uint16_t)0;
        return (uint8_t *)0;                   // no bytes to return right now
    }
    QSCtr n = (QSCtr)(QS_end_ - QS_tail_);
    if (n > QS_used_) {
        n = QS_used_;
    }
    if (n > (QSCtr)(*pNbytes)) {
        n = (QSCtr)(*pNbytes);
    }
    *pNbytes = (uint16_t)n;
    QS_used_ = (QSCtr)(QS_used_ - n);
    QSCtr t = QS_tail_;
    QS_tail_ = (QSCtr)(QS_tail_ + n);
    if (QS_tail_ == QS_end_) {
        QS_tail_ = (QSCtr)0;
    }
    /* printf("after QS_end_:%4u, QS_head_:%4u, QS_tail_:%4u, QS_used_:%4u, QS_seq_:%4u, QS_full_%4u.\n"
            , QS_end_  ///< offset of the end of the ring buffer
            , QS_head_ ///< offset to where next byte will be inserted
            , QS_tail_ ///< offset of where next event will be extracted
            , QS_used_ ///< number of bytes currently in the ring buffer
            , QS_seq_  ///< the record sequence number
            , QS_full_ ///< the ring buffer is temporarily full
            ); */
    return &QS_ring_[t];
}
