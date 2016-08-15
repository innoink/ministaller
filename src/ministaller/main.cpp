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
#include <QFileInfo>
#include <QDir>
#include "options.h"

enum CommandLineParseResult {
    CommandLineOk,
    CommandLineError,
    CommandLineHelpRequested
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, const QStringList &arguments, ParsedOptions &options) {
    QCommandLineOption updateConfigPathOption(QStringList() << "u" << "update-config",
                                        "Path to the update config",
                                        "filepath");
    parser.addOption(updateConfigPathOption);

    QCommandLineOption installPathOption(QStringList() << "i" << "install-path",
                                        "Path to the installed package",
                                        "directory");
    parser.addOption(installPathOption);

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
            options.m_PackageConfigPath = parser.value(updateConfigPathOption);
            if (!QFileInfo(options.m_PackageConfigPath).exists()) {
                std::cerr << "Update config can't be found" << std::endl;
                break;
            }
        } else {
            std::cerr << "Update config path is missing" << std::endl;
            break;
        }

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

        result = CommandLineOk;
    } while (false);

    return result;
}

int main(int argc, char *argv[]) {
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

    return app.exec();
}
