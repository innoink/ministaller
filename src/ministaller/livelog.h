/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef LIVELOG_H
#define LIVELOG_H

#include <QObject>
#include <QAbstractListModel>

class LiveLog : public QAbstractListModel
{
    Q_OBJECT
private:
    explicit LiveLog(QObject *parent = 0):
        QAbstractListModel(parent)
    {
    }

    LiveLog(LiveLog const&);
    void operator=(LiveLog const&);

public:
    static LiveLog& getInstance()
    {
        static LiveLog instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    void log(const QString &message);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    QStringList m_LogLines;
};

#endif // LIVELOG_H
