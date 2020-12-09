/*
  main.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <config-gammaray.h>
#include "config-gammaray-version.h"

#include <launcher/core/injector/injectorfactory.h>
#include <launcher/core/clientlauncher.h>
#include <launcher/core/launchoptions.h>
#include <launcher/core/launcherfinder.h>
#include <launcher/core/launcher.h>
#include <launcher/core/probeabi.h>
#include <launcher/core/probeabidetector.h>
#include <launcher/core/probefinder.h>
#include <launcher/core/selftest.h>

#include <common/paths.h>
#include <common/protocol.h>

#ifdef HAVE_QT_WIDGETS
#include <QApplication>
#else
#include <QCoreApplication>
#endif

#include <QDebug>
#include <QDir>
#include <QUrl>
#include <QStringList>
#include <QVariant>
#include <QProcessEnvironment>

#include <csignal>

namespace {
void shutdownGracefully(int sig)
{
    static volatile std::sig_atomic_t handlingSignal = 0;

    if (!handlingSignal) {
        handlingSignal = 1;
        qDebug() << "Signal" << sig << "received, shutting down gracefully.";
        QCoreApplication *app = QCoreApplication::instance();
        app->quit();
        return;
    }

    // re-raise signal with default handler and trigger program termination
    std::signal(sig, SIG_DFL);
    std::raise(sig);
}

void installSignalHandler()
{
#ifdef SIGHUP
    std::signal(SIGHUP, shutdownGracefully);
#endif
#ifdef SIGINT
    std::signal(SIGINT, shutdownGracefully);
#endif
#ifdef SIGTERM
    std::signal(SIGTERM, shutdownGracefully);
#endif
}
}

using namespace GammaRay;

QTextStream out(stdout);
QTextStream err(stderr);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
using Qt::endl;
#endif

static void usage(const char *argv0)
{
    out << "Usage: " << argv0
        << " [options] [--pid <pid> | <application> <args> | --connect <host>[:<port>]]" << endl;
    out << "" << endl;
    out << "Inspect runtime internals of a Qt-application, such as:" << endl;
    out << "  QObject tree, properties, signal/slots, widgets, models," << endl;
    out << "  graphics views, javascript debugger, resources," << endl;
    out << "  state machines, meta types, fonts, codecs, text documents" << endl;
    out << "" << endl;
    out << "Options:" << endl;
    out << " -i, --injector <injector>           \tset injection type, possible values:" << endl;
    out << "                                     \t" << InjectorFactory::availableInjectors().join(QStringLiteral(
                                                                                                       ", "))
        << endl;
    out
        <<
    " -o, --injector-override <executable>\tOverride the injector executable if handled (requires -i/--injector)"
        << endl;
    out << " -p, --pid <pid>                     \tattach to running Qt application" << endl;
    out << "     --inprocess                     \tuse in-process UI" << endl;
    out << "     --inject-only                   \tonly inject the probe, don't show the UI"
        << endl;
    out
        <<
    "     --listen <address>              \tspecify the address the server should listen on [default: "
        << GAMMARAY_DEFAULT_ANY_TCP_URL << "]" << endl;
    out
        <<
    "     --no-listen                     \tdisables remote access entirely (implies --inprocess)"
        << endl;
    out << "     --list-probes                   \tlist all installed probes" << endl;
    out << "     --probe <abi>                   \tspecify which probe to use" << endl;
    out << "     --connect <host>[:port]         \tconnect to an already injected target" << endl;
    out
        <<
    "     --self-test [injector]          \trun self tests, of everything or the specified injector"
        << endl;
    out << " -h, --help                          \tprint program help and exit" << endl;
    out << " -v, --version                       \tprint program version and exit" << endl;
#ifdef HAVE_QT_WIDGETS
    out << endl
        << "When run without any options, " << argv0 << " will present a list of running\n"
        << "Qt-applications from which you can attach the selected injector. Else,\n"
        << "you can attach to a running process by specifying its pid, or you can\n"
        << "start a new Qt-application by specifying its name (and optional arguments)."
        << endl;
#endif
}

static bool startLauncher()
{
    const QString launcherPath = LauncherFinder::findLauncher(LauncherFinder::LauncherUI);
    QProcess proc;
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start(launcherPath);
    if (!proc.waitForFinished(-1))
        return false;
    return proc.exitCode() == 0;
}

static QUrl urlFromUserInput(const QString &s)
{
    QUrl url(s);
    if (url.scheme().isEmpty()) { // backward compat: map input without a scheme to tcp + hostname
        url.setScheme(QStringLiteral("tcp"));
        QString host = url.path();
        int port = -1;
        const int pos = host.lastIndexOf(':');
        if (pos > 0) {
            port = host.mid(pos + 1).toUShort(); // clazy:exclude=qstring-ref due to Qt4 support
            host = host.left(pos);
        }
        url.setHost(host);
        url.setPort(port);
        url.setPath(QString());
    }

    return url;
}

int main(int argc, char **argv)
{
    QCoreApplication::setOrganizationName(QStringLiteral("KDAB"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kdab.com"));
    QCoreApplication::setApplicationName(QStringLiteral("GammaRay"));
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    installSignalHandler();

    QStringList args;
    args.reserve(argc);
    for (int i = 1; i < argc; ++i)
        args.push_back(QString::fromLocal8Bit(argv[i]));

#ifndef GAMMARAY_CORE_ONLY_LAUNCHER
    QApplication app(argc, argv); // for style inspector
#else
    QCoreApplication app(argc, argv);
#endif
    Paths::setRelativeRootPath(GAMMARAY_INVERSE_BIN_DIR);

    QStringList builtInArgs = QStringList() << QStringLiteral("-style")
                                            << QStringLiteral("-stylesheet")
                                            << QStringLiteral("-graphicssystem");

    LaunchOptions options;
    while (!args.isEmpty() && args.first().startsWith('-')) {
        const QString arg = args.takeFirst();
        if ((arg == QLatin1String("-i") || arg == QLatin1String("--injector")) && !args.isEmpty()) {
            options.setInjectorType(args.takeFirst());
            continue;
        }
        if ((arg == QLatin1String("-o") || arg == QLatin1String("--injector-override"))
            && !args.isEmpty()) {
            options.setInjectorTypeExecutableOverride(args.takeFirst());
            continue;
        }
        if ((arg == QLatin1String("-p") || arg == QLatin1String("--pid")) && !args.isEmpty()) {
            options.setPid(args.takeFirst().toInt());
            continue;
        }
        if (arg == QLatin1String("-h") || arg == QLatin1String("--help")) {
            usage(argv[0]);
            return 0;
        }
        if (arg == QLatin1String("-v") || arg == QLatin1String("--version")) {
            out << "GammaRay version " << GAMMARAY_VERSION_STRING << endl;
            out << "Copyright (C) 2010-2021 Klaralvdalens Datakonsult AB, "
                << "a KDAB Group company, info@kdab.com" << endl;
            out << "Protocol version " << Protocol::version() << endl;
            out << "Broadcast version " << Protocol::broadcastFormatVersion() << endl;
            return 0;
        }
        if (arg == QLatin1String("--inprocess"))
            options.setUiMode(LaunchOptions::InProcessUi);
        if (arg == QLatin1String("--inject-only"))
            options.setUiMode(LaunchOptions::NoUi);
        if (arg == QLatin1String("--listen") && !args.isEmpty())
            options.setProbeSetting(QStringLiteral("ServerAddress"),
                                    urlFromUserInput(args.takeFirst()).toString());
        if (arg == QLatin1String("--no-listen")) {
            options.setProbeSetting(QStringLiteral("RemoteAccessEnabled"), false);
            options.setUiMode(LaunchOptions::InProcessUi);
        }
        if (arg == QLatin1String("--list-probes")) {
            foreach (const ProbeABI &abi, ProbeFinder::listProbeABIs())
                out << abi.id() << " (" << abi.displayString() << ")" << endl;
            return 0;
        }
        if (arg == QLatin1String("--probe") && !args.isEmpty()) {
            const ProbeABI abi = ProbeABI::fromString(args.takeFirst());
            if (!abi.isValid()) {
                out << "Invalid probe ABI specified, see --list-probes for valid ones." << endl;
                return 1;
            }
            if (ProbeFinder::findProbe(abi).isEmpty()) {
                out << abi.id() << " is not a known probe, see --list-probes." << endl;
                return 1;
            }
            options.setProbeABI(abi);
        }
        if (arg == QLatin1String("--connect") && !args.isEmpty()) {
            const QUrl url = urlFromUserInput(args.takeFirst());
            ClientLauncher client;
            client.launch(url);
            client.waitForFinished();
            return 0;
        }
        if (arg == QLatin1String("--self-test")) {
            SelfTest selfTest;
            QObject::connect(&selfTest, &SelfTest::information, [](const QString &msg) {
                out << msg << endl;
            });
            QObject::connect(&selfTest, &SelfTest::error, [](const QString &msg) {
                err << "Error: " << msg << endl;
            });
            if (args.isEmpty() || args.first().startsWith('-'))
                return selfTest.checkEverything() ? 0 : 1;
            const auto injectorType = args.takeFirst();
            return selfTest.checkInjector(injectorType) ? 0 : 1;
        }

        // debug/test options
        if (arg == QLatin1String("-unittest"))
            qputenv("GAMMARAY_UNITTEST", "1");
        // built-in arguments of QApp, could be meant for us if we are showing the launcher window
        foreach (const QString &builtInArg, builtInArgs) {
            if (arg == builtInArg && !args.isEmpty())
                args.takeFirst();
        }
    }
    options.setLaunchArguments(args);

    if (!options.isValid()) {
        if (startLauncher())
            return 0;
        usage(argv[0]);
        return 1;
    }

    if (!options.isValid())
        return 0;
    Q_ASSERT(options.isValid());

    // attempt to autodetect probe ABI, if not set explicitly
    if (!options.probeABI().isValid()) {
        ProbeABIDetector detector;
        if (options.isLaunch())
            options.setProbeABI(detector.abiForExecutable(options.absoluteExecutablePath()));
        else
            options.setProbeABI(detector.abiForProcess(options.pid()));
    }

    // find a compatible probe
    if (options.probeABI().isValid()) {
        const ProbeABI bestABI = ProbeFinder::findBestMatchingABI(options.probeABI());
        if (!bestABI.isValid()) {
            out << "No probe found for ABI " << options.probeABI().id() << endl;
            return 1;
        }
        out << "Detected ABI " << options.probeABI().id() << ", using ABI " << bestABI.id()
            << "." << endl;
        options.setProbeABI(bestABI);
    } else {
        const QVector<ProbeABI> availableProbes = ProbeFinder::listProbeABIs();
        if (availableProbes.isEmpty()) {
            out << "No probes found, this is likely an installation problem." << endl;
            return 1;
        }
        if (availableProbes.size() > 1) {
            out << "No probe ABI specified and ABI auto-detection failed, picking "
                << availableProbes.first().id() << " at random." << endl;
            out
                <<
            "To specify the probe ABI explicitly use --probe <abi>, available probes are listed using the --list-probes option."
                << endl;
        }
        options.setProbeABI(availableProbes.first());
    }

    // use a local connection when starting gui locally with this launcher
    if (!options.probeSettings().contains(QStringLiteral("ServerAddress").toUtf8())
        && options.uiMode() != LaunchOptions::NoUi
        && !LauncherFinder::findLauncher(LauncherFinder::LauncherUI).isEmpty()) {
        options.setProbeSetting(QStringLiteral("ServerAddress"), "tcp://127.0.0.1");
    }

    Launcher launcher(options);
    QObject::connect(&launcher, SIGNAL(finished()), &app, SLOT(quit()));
    QObject::connect(&launcher, SIGNAL(attached()), &app, SLOT(quit()));
    if (!launcher.start())
        return launcher.exitCode();
    auto result = app.exec();
    return result == 0 ? launcher.exitCode() : result;
}
