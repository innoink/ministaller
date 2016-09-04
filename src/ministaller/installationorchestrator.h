/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef INSTALLATIONORCHESTRATOR_H
#define INSTALLATIONORCHESTRATOR_H

#include <QObject>
#include "options.h"

class InstallationOrchestrator : public QObject
{
    Q_OBJECT
public:
    explicit InstallationOrchestrator(const ParsedOptions &options, QObject *parent = 0);

signals:
    void finished();

public slots:
    void process();

private:
    void doWork();

private:
    const ParsedOptions &m_Options;
};

#endif // INSTALLATIONORCHESTRATOR_H
