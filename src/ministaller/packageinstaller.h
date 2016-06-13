/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
*/

#ifndef PACKAGEINSTALLER_H
#define PACKAGEINSTALLER_H

#include <QObject>
#include <QString>

class PackageInstaller : public QObject
{
    Q_OBJECT
public:
    PackageInstaller();

public:
    void setInstallDir(const QString &dir) { Q_ASSERT(!dir.isEmpty()); m_InstallDir = dir; }
    void setPackageDir(const QString &dir) { Q_ASSERT(!dir.isEmpty()); m_PackageDir = dir; }
    void setBackupDir(const QString &dir) { Q_ASSERT(!dir.isEmpty()); m_BackupDir = dir; }

private:
    QString m_InstallDir;
    QString m_PackageDir;
    QString m_BackupDir;
};

#endif // PACKAGEINSTALLER_H
