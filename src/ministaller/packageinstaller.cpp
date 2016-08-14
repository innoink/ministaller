/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "packageinstaller.h"
#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <string.h>
#include <cerrno>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

bool waitForProcess(PLATFORM_PID pid) {
#if defined(Q_OS_UNIX)
    pid_t result = ::waitpid(pid, 0, 0);
    if (result < 0) {
        qWarning() << "waitpid() failed with error:" << QString::fromLocal8Bit(strerror(errno));
    }

    return result > 0;

#elif defined(Q_OS_WIN)
    HANDLE hProc;

    if (!(hProc = OpenProcess(SYNCHRONIZE, FALSE, pid))) {
        qWarning() << "Unable to get process handle for pid" << pid << "last error" << GetLastError();
        return false;
    }

    DWORD dwRet = WaitForSingleObject(hProc, INFINITE);
    CloseHandle(hProc);

    if (dwRet == WAIT_FAILED) {
        qWarning() << "WaitForSingleObject failed with error" << GetLastError();
    }

    return (dwRet == WAIT_OBJECT_0);
#endif
}

void cleanupEmptyDirectories(const QString &baseDirectory) {
    QDirIterator it(baseDirectory, QDir::NoDotAndDotDot | QDir::Dirs, QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
        QString subdirPath = it.next();

        cleanupEmptyDirectories(subdirPath);

        QDir subdir(subdirPath);
        if (subdir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).empty()) {
            qInfo() << "Removing empty directory:" << subdirPath;
            bool removeResult = subdir.removeRecursively();
            if (!removeResult) {
                qWarning() << "Failed to remove directory:" << subdirPath;
            }
        }
    }
}

PackageInstaller::PackageInstaller() {
}

void PackageInstaller::backupPath(const QString &path) {
    QFile originalFile(path);

    if (!originalFile.exists()) {
        return;
    }

    QString backupPath = path + ".bak";

    QFile backupFile(backupPath);
    if (backupFile.exists()) {
        backupFile.remove();
    }

    originalFile.rename(backupPath);
    m_BackupPaths[path] = backupPath;
}

void PackageInstaller::removeBackups() {
    QHashIterator<QString, QString> it(m_BackupPaths);
    int removedCount = 0;

    while (it.hasNext()) {
        it.next();

        QFile backupFile(it.value());
        if (backupFile.exists()) {
            if (backupFile.remove()) {
                removedCount++;
            } else {
                qWarning() << "Failed to cleanup backup:" << it.value();
            }
        } else {
            qDebug() << "Backup path does not exist anymore:" << it.value();
        }
    }

    qInfo() << removedCount << "files removed";
}

void PackageInstaller::restoreBackups() {
    QHashIterator<QString, QString> it(m_BackupPaths);
    int restoredCount = 0;

    while (it.hasNext()) {
        it.next();

        const QString &originalFilepath = it.key();

        QFile originalFile(originalFilepath);
        if (originalFile.exists()) {
            originalFile.remove();
        }

        QFile backupFile(it.value());
        if (backupFile.exists()) {
            if (backupFile.rename(originalFilepath)) {
                restoredCount++;
            } else {
                qWarning() << "Failed to restore file:" << originalFilepath;
            }
        } else {
            qDebug() << "Backup path does not exist anymore:" << it.value();
        }
    }

    qInfo() << restoredCount << "out of" << m_BackupPaths.size() << "files restored";
}

void PackageInstaller::ensureDirectoryExistsForFile(const QString &filepath) {
    QFileInfo fi(filepath);

    if (fi.exists()) { return; }

    QDir filesDir = fi.absoluteDir();
    if (!filesDir.exists()) {
        if (!filesDir.mkpath(".")) {
            qWarning() << "Failed to create a path:" << filesDir.path();
        }
    }
}
