/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "installationorchestrator.h"
#include <memory>
#include <QFileInfo>
#include <QDir>
#include <QTemporaryDir>
#include "fshelpers.h"
#include "packageparser.h"
#include "../common/diffgeneratorbase.h"
#include "packageinstaller.h"
#include "../common/ifilesprovider.h"

InstallationOrchestrator::InstallationOrchestrator(const ParsedOptions &options, QObject *parent) :
    QObject(parent),
    m_Options(options)
{
}

void InstallationOrchestrator::process() {
    doWork();
    emit finished();
}

void InstallationOrchestrator::doWork() {
    QString extractedDir;
    if (!extractPackage(m_Options.m_PackagePath, extractedDir)) {
        qWarning() << "Failed to extract archive";
        return;
    }

    QTemporaryDir backupDir;
    if (!backupDir.isValid()) {
        qWarning() << "Failed to create backup directory";
        return;
    }

    std::shared_ptr<IFilesProvider> filesProvider;
    if (m_Options.m_GenerateDiff) {
        std::shared_ptr<DiffGeneratorBase> diffGenerator(new DiffGeneratorBase(m_Options.m_PackagePath, m_Options.m_InstallDir,
                                                                               m_Options.m_ForceUpdate, m_Options.m_KeepMissing));
        diffGenerator->generateDiffs();
        filesProvider = std::dynamic_pointer_cast<IFilesProvider>(diffGenerator);
    } else {
        std::shared_ptr<PackageParser> packageParser(new PackageParser(m_Options.m_PackageConfigPath));

        if (!packageParser->parsePackage()) {
            qWarning() << "Cannot parse package config";
            return;
        }

        filesProvider = std::dynamic_pointer_cast<IFilesProvider>(packageParser);
    }

    PackageInstaller packageInstaller(filesProvider);
    packageInstaller.setInstallDir(m_Options.m_InstallDir);
    packageInstaller.setPackageDir(extractedDir);
    packageInstaller.setBackupDir(backupDir.path());

    packageInstaller.install();
}
