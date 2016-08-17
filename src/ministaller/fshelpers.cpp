/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "fshelpers.h"
#include <QTemporaryDir>
#include <QStringList>
#include <quazip/JlCompress.h>

bool extractPackage(const QString &packagePath, QString &resultPath) {
    bool success = false;
    QTemporaryDir dir;

    if (dir.isValid()) {
        resultPath = dir.path();

        auto fileList = JlCompress::extractDir(packagePath, resultPath);
        success = !fileList.empty();
    }

    return success;
}
