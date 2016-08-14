/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef PACKAGEPARSER_H
#define PACKAGEPARSER_H

#include <QString>
#include <QVector>
#include <QJsonArray>
#include "../common/fileentry.h"

class PackageParser
{
public:
    PackageParser(const QString &pathToConfig);

public:
    bool parsePackage();

public:
    const QVector<FileEntry> &getItemsToAdd() const { return m_ItemsToAdd; }
    const QVector<FileEntry> &getItemsToUpdate() const { return m_ItemsToUpdate; }
    const QVector<FileEntry> &getItemsToRemove() const { return m_ItemsToRemove; }

private:
    void parseJsonArray(const QJsonArray &array, QVector<FileEntry> &entryList);

private:
    const QString &m_ConfigPath;
    QVector<FileEntry> m_ItemsToAdd;
    QVector<FileEntry> m_ItemsToUpdate;
    QVector<FileEntry> m_ItemsToRemove;
};

#endif // PACKAGEPARSER_H
