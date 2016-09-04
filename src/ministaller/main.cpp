/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include <iostream>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QQmlContext>
#include <QThread>
#include <QFileInfo>
#include <QDir>
#include "installationorchestrator.h"
#include "options.h"
#include "livelog.h"

enum CommandLineParseResult {
    CommandLineOk,
    CommandLineError,
    CommandLineHelpRequested
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, const QStringList &arguments, ParsedOptions &options) {
    QCommandLineOption updateConfigPathOption(QStringList() << "u" << "update-config",
                                        "Path to the update config if available",
                                        "filepath");
    parser.addOption(updateConfigPathOption);

    QCommandLineOption installPathOption(QStringList() << "i" << "install-path",
                                        "Path to the installed package",
                                        "directory");
    parser.addOption(installPathOption);

    QCommandLineOption packagePathOption(QStringList() << "p" << "package-path",
                                         "Path to the package to install",
                                         "filepath");
    parser.addOption(packagePathOption);

    QCommandLineOption pidWaitForOption(QStringList() << "w" << "wait-pid",
                                        "Pid of the process to wait for",
                                        "number");
    parser.addOption(pidWaitForOption);

    QCommandLineOption forceUpdateOption(QStringList() << "f" << "force-update",
                                        "Don't skip same files");
    parser.addOption(forceUpdateOption);

    QCommandLineOption dontRemoveOption(QStringList() << "k" << "keep-missing",
                                        "Do not remove missing files in new package");
    parser.addOption(dontRemoveOption);

    const QCommandLineOption helpOption = parser.addHelpOption();

    CommandLineParseResult result = CommandLineError;

    do {
        if (!parser.parse(arguments)) {
            std::cerr << parser.errorText().toStdString();
            break;
        }

        if (parser.isSet(helpOption)) {
            result = CommandLineHelpRequested;
            break;
        }

        if (parser.isSet(updateConfigPathOption)) {
            options.m_GenerateDiff = false;
            options.m_PackageConfigPath = parser.value(updateConfigPathOption);
            if (!QFileInfo(options.m_PackageConfigPath).exists()) {
                std::cerr << "Update config can't be found" << std::endl;
                break;
            }
        } else {
            options.m_GenerateDiff = true;
        }

        options.m_ForceUpdate = parser.isSet(forceUpdateOption);
        options.m_KeepMissing = parser.isSet(dontRemoveOption);

        if (parser.isSet(installPathOption)) {
            options.m_InstallDir = parser.value(installPathOption);
            if (!QDir(options.m_InstallDir).exists()) {
                std::cerr << "Install dir can't be found" << std::endl;
                break;
            }
        } else {
            std::cerr << "Install directory is missing" << std::endl;
            break;
        }

        if (parser.isSet(packagePathOption)) {
            options.m_PackagePath = parser.value(packagePathOption);
            if (!QFileInfo(options.m_PackagePath).exists()) {
                std::cerr << "Package can't be found" << std::endl;
                break;
            }
        } else {
            std::cerr << "Package path is missing" << std::endl;
            break;
        }

        if (parser.isSet(pidWaitForOption)) {
            bool ok = false;
            options.m_PidWaitFor = parser.value(pidWaitForOption).toInt(&ok);
            if (!ok) {
                options.m_PidWaitFor = 0;
            }
        }

        result = CommandLineOk;
    } while (false);

    return result;
}

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(context);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QString logLine = qFormatLogMessage(type, context, msg);
#else
    QString msgType;
    switch (type) {
        case QtDebugMsg:
            msgType = "debug";
            break;
        case QtWarningMsg:
            msgType = "warning";
            break;
        case QtCriticalMsg:
            msgType = "critical";
            break;
        case QtFatalMsg:
            msgType = "fatal";
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 1))
        case QtInfoMsg:
            msgType = "info";
            break;
#endif
    }

    // %{time hh:mm:ss.zzz} %{type} T#%{threadid} %{function} - %{message}
    QString time = QDateTime::currentDateTimeUtc().toString("hh:mm:ss.zzz");
    QString logLine = QString("%1 %2 T#%3 %4 - %5")
                          .arg(time).arg(msgType)
                          .arg(0).arg(context.function)
                          .arg(msg);
#endif

    auto &liveLog = LiveLog::getInstance();
    liveLog.log(logLine);

    if (type == QtFatalMsg) {
        abort();
    }
}

int main(int argc, char *argv[]) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    qSetMessagePattern("%{time hh:mm:ss.zzz} %{type} T#%{threadid} %{function} - %{message}");
#endif

    qInstallMessageHandler(myMessageHandler);

    QGuiApplication app(argc, argv);

    QCommandLineParser optionsParser;
    optionsParser.setApplicationDescription("Minimal intaller for the desktop Qt application");
    ParsedOptions options;

    switch (parseCommandLine(optionsParser, app.arguments(), options)) {
    case CommandLineOk:
        break;
    case CommandLineError:
        std::cout << optionsParser.helpText().toStdString() << std::endl;
        return 1;
    case CommandLineHelpRequested:
        std::cout << optionsParser.helpText().toStdString() << std::endl;
        return 0;
    }

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QQmlContext *rootContext = engine.rootContext();
    auto &liveLog = LiveLog::getInstance();
    rootContext->setContextProperty("liveLog", &liveLog);

    InstallationOrchestrator *orchestrator = new InstallationOrchestrator(options);
    QThread *thread = new QThread();
    orchestrator->moveToThread(thread);

    QObject::connect(thread, SIGNAL(started()), orchestrator, SLOT(process()));
    QObject::connect(orchestrator, SIGNAL(finished()), thread, SLOT(quit()));

    QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    QObject::connect(orchestrator, SIGNAL(finished()), orchestrator, SLOT(deleteLater()));

    thread->start();

    return app.exec();
}
