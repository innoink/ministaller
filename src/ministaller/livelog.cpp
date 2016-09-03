/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "livelog.h"

void LiveLog::log(const QString &message) {
    int length = m_LogLines.length();
    beginInsertRows(QModelIndex(), length, length);
    m_LogLines.append(message);
    endInsertRows();
}

int LiveLog::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_LogLines.length();
}

QVariant LiveLog::data(const QModelIndex &index, int role) const {
    int row = index.row();
    if (row < 0 || row >= m_LogLines.length()) { return QVariant(); }

    switch (role) {
    case Qt::DisplayRole:
        return m_LogLines.at(row);
    default:
        return QVariant();
    }
}
