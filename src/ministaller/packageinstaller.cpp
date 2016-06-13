/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
*/

#include "packageinstaller.h"
#include <QDebug>
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
