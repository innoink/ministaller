/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "livelog.h"

void LiveLog::log(const QString &message) {
    m_JoinedLog.append(message);
    m_JoinedLog.append(QChar::LineSeparator);
}
