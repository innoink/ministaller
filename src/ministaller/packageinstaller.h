/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef PACKAGEINSTALLER_H
#define PACKAGEINSTALLER_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QHash>
#include "platform.h"
#include "../common/fileentry.h"

#ifdef Q_OS_WIN
// for the platform_pid DWORD
#include "windows.h"
#endif

class PackageInstaller : public QObject
{
    Q_OBJECT
public:
    PackageInstaller();

public:
    void setInstallDir(const QString &dir) { Q_ASSERT(!dir.isEmpty()); m_InstallDir = dir; }
    void setPackageDir(const QString &dir) { Q_ASSERT(!dir.isEmpty()); m_PackageDir = dir; }
    void setBackupDir(const QString &dir) { Q_ASSERT(!dir.isEmpty()); m_BackupDir = dir; }
    void setPidWaitFor(PLATFORM_PID pid) { m_PidWaitFor = pid; }
    void setItemsToAdd(const QVector<FileEntry> &items) { m_ItemsToAdd = items; }
    void setItemsToUpdate(const QVector<FileEntry> &items) { m_ItemsToUpdate = items; }
    void setItemsToRemove(const QVector<FileEntry> &items) { m_ItemsToRemove = items; }

public:
    void install();

private:
    void beforeInstall();
    bool installPackage();
    void afterSuccess();
    void afterFailure();

private:
    bool addFilesToAdd();
    bool updateFilesToUpdate();
    bool removeFilesToRemove();
    void removeFilesToAdd();
    void backupPath(const QString &path);
    void removeBackups();
    void restoreBackups();
    void ensureDirectoryExistsForFile(const QString &filepath);

private:
    QString m_InstallDir;
    QString m_PackageDir;
    QString m_BackupDir;
    PLATFORM_PID m_PidWaitFor;
    QVector<FileEntry> m_ItemsToAdd;
    QVector<FileEntry> m_ItemsToUpdate;
    QVector<FileEntry> m_ItemsToRemove;
    QHash<QString, QString> m_BackupPaths;
};

#endif // PACKAGEINSTALLER_H
