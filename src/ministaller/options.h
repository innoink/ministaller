/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QString>

struct ParsedOptions {
    QString m_PackageConfigPath;
    QString m_InstallDir;
    QString m_PackagePath;
    int m_PidWaitFor;
    bool m_GenerateDiff;
    bool m_KeepMissing;
    bool m_ForceUpdate;
};

#endif // OPTIONS_H
