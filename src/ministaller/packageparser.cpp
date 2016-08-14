/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "packageparser.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include "../common/logger.h"

PackageParser::PackageParser(const QString &pathToConfig):
    m_ConfigPath(pathToConfig)
{
}

bool PackageParser::parsePackage() {
    QFile file(m_ConfigPath);

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    QJsonObject rootObject = document.object();

    bool anyFault = false;

    do {
        if (rootObject.contains(ADD_ITEM)) {
            QJsonValue addList = rootObject[ADD_ITEM];
            if (addList.isArray()) {
                parseJsonArray(addList.toArray(), m_ItemsToAdd);
            } else {
                anyFault = true;
                break;
            }
        }

        if (rootObject.contains(UPDATE_ITEM)) {
            QJsonValue updateList = rootObject[UPDATE_ITEM];
            if (updateList.isArray()) {
                parseJsonArray(updateList.toArray(), m_ItemsToUpdate);
            } else {
                anyFault = true;
                break;
            }
        }

        if (rootObject.contains(REMOVE_ITEM)) {
            QJsonValue removeList = rootObject[REMOVE_ITEM];
            if (removeList.isArray()) {
                parseJsonArray(removeList.toArray(), m_ItemsToRemove);
            } else {
                anyFault = true;
                break;
            }
        }
    } while (false);

    return !anyFault;
}

void PackageParser::parseJsonArray(const QJsonArray &array, QVector<FileEntry> &entryList) {
    int size = array.size();
    entryList.reserve(size);

    for (int i = 0; i < size; ++i) {
        const auto &item = array.at(i);
        if (!item.isObject()) { continue; }

        auto object = item.toObject();
        if (!object.contains(PATH_KEY) ||
                !object.contains(SHA1_KEY)) {
            continue;
        }

        QString relativePath = object.value(PATH_KEY).toString();
        QString hashSum = object.value(SHA1_KEY).toString();
        entryList.append({relativePath, hashSum});
    }
}
