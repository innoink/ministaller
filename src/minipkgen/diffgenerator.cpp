/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "diffgenerator.h"
#include <QDirIterator>
#include <QCryptographicHash>
#include <QFile>
#include <QHash>
#include <QJsonObject>
#include "logger.h"

#define BLOCK_SIZE 8192

QHash<QString, QString> Sha1Cache;

QString getFileSha1(const QString &path) {
    if (Sha1Cache.contains(path)) {
        return Sha1Cache[path];
    }

    QCryptographicHash crypto(QCryptographicHash::Sha1);
    QFile file(path);
    file.open(QFile::ReadOnly);

    while (!file.atEnd()) {
        crypto.addData(file.read(BLOCK_SIZE));
    }

    QByteArray hash = crypto.result();
    QString hex = QString::fromLatin1(hash.toHex());
    Sha1Cache.insert(path, hex);
    return hex;
}

bool areFilesEqual(const QString &path1, const QString &path2) {
    return getFileSha1(path1) == getFileSha1(path2);
}

void appendToList(const QString &relativePath, const QString &fullPath, QVector<FileEntry> &list) {
    QString sha1 = getFileSha1(fullPath);
    list.append({relativePath, sha1});
}

void addToListRecursively(const QDir &relativeDir, const QString &path, QVector<FileEntry> &list) {
    QDirIterator it(path, QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString fullPath = it.next();
        QString path = relativeDir.relativeFilePath(fullPath);
        appendToList(path, fullPath, list);
    }
}

DiffGenerator::DiffGenerator(const ParsedOptions &options):
    m_BaseDir(options.m_BaseDir),
    m_NewDir(options.m_NewDir),
    m_Options(options)
{
    m_BaseDirPath = m_BaseDir.absolutePath();
    m_NewDirPath = m_NewDir.absolutePath();
}

void DiffGenerator::generateDiffs() {
    generateDirsDiff(m_BaseDirPath, m_NewDirPath);
}

QJsonDocument DiffGenerator::generateJson() {
    QJsonObject rootObject;

    QJsonArray itemsToRemove;
    listToJsonArray(m_ItemsToRemove, itemsToRemove);
    rootObject.insert("remove", itemsToRemove);

    QJsonArray itemsToUpdate;
    listToJsonArray(m_ItemsToUpdate, itemsToUpdate);
    rootObject.insert("update", itemsToUpdate);

    QJsonArray itemsToAdd;
    listToJsonArray(m_ItemsToAdd, itemsToAdd);
    rootObject.insert("add", itemsToAdd);

    QJsonDocument document(rootObject);
    return document;
}

void DiffGenerator::generateDirsDiff(const QString &baseDirPath, const QString &newDirPath) {
    LOG << "BaseDir:" << baseDirPath << "NewDir:" << newDirPath;

    findFilesToRemoveOrUpdate(baseDirPath, newDirPath);
    findFilesToAdd(baseDirPath, newDirPath);
}

void DiffGenerator::findFilesToRemoveOrUpdate(const QString &baseDirPath, const QString &newDirPath) {
    QDirIterator baseIt(baseDirPath, QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files, QDirIterator::NoIteratorFlags);
    QDir newDir(newDirPath);

    while (baseIt.hasNext()) {
        QString baseFilepath = baseIt.next();
        LOG << "Checking path:" << baseFilepath;
        QString baseFilename = baseIt.fileName();
        QString newFilepath = newDir.filePath(baseFilename);

        QFileInfo fi(newFilepath);
        if (!fi.exists()) {
            QFileInfo baseFi(baseFilepath);
            if (baseFi.isFile()) {
                LOG << "Removing file:" << baseFilepath;
                QString path = m_BaseDir.relativeFilePath(baseFilepath);
                appendToList(path, baseFilepath, m_ItemsToRemove);
            } else if (baseFi.isDir()) {
                LOG << "Removing dir:" << baseFilepath;
                addToListRecursively(m_BaseDir, baseFilepath, m_ItemsToRemove);
            }
        } else {
            if (fi.isFile()) {
                QString path = m_BaseDir.relativeFilePath(baseFilepath);
                if (m_Options.m_ForceUpdate || !areFilesEqual(baseFilepath, newFilepath)) {
                    LOG << "Updating the file:" << path;
                    appendToList(path, baseFilepath, m_ItemsToUpdate);
                } else {
                    LOG << "Skipping same file:" << path;
                }
            } else if (fi.isDir()) {
                Q_ASSERT(QFileInfo(baseFilepath).isDir());
                generateDirsDiff(baseFilepath, newFilepath);
            }
        }
    }
}

void DiffGenerator::findFilesToAdd(const QString &baseDirPath, const QString &newDirPath) {
    QDirIterator newIt(newDirPath);
    QDir baseDir(baseDirPath);

    while (newIt.hasNext()) {
        QString newFilepath = newIt.next();
        QString newFilename = newIt.fileName();
        QString baseFilepath = baseDir.filePath(newFilename);

        QFileInfo fi(baseFilepath);
        if (!fi.exists()) {
            LOG << "Adding file:" << newFilepath;
            addToListRecursively(m_NewDir, newFilepath, m_ItemsToAdd);
        }
    }
}

void DiffGenerator::listToJsonArray(const QVector<FileEntry> &list, QJsonArray &array) {
    for (auto &entry: list) {
        QJsonObject fileEntryObj;
        fileEntryObj.insert("path", QJsonValue(entry.m_Filepath));
        fileEntryObj.insert("sha1", QJsonValue(entry.m_Sha1));

        array.append(fileEntryObj);
    }
}
