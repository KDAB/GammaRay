/*
  main.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "config-gammaray.h"
#include "config-gammaray-version.h"
#include "probefinder.h"
#include "injector/injectorfactory.h"
#include "launchoptions.h"
#include "clientlauncher.h"
#include "launcherfinder.h"

#ifdef HAVE_QT_WIDGETS
#include <QApplication>
#else
#include <QCoreApplication>
#endif

#include <QDebug>
#include <QFileInfo>
#include <QStringList>

using namespace GammaRay;

QTextStream out(stdout);
QTextStream err(stderr);

static void usage(const char *argv0)
{
  out << "Usage: " << argv0
      << " [--injector <injector>] [--pid <pid> | <application> <args>]" << endl;
  out << "" << endl;
  out << "Inspect runtime internals of a Qt-application, such as:" << endl;
  out << "  QObject tree, properties, signal/slots, widgets, models," << endl;
  out << "  graphics views, javascript debugger, resources," << endl;
  out << "  state machines, meta types, fonts, codecs, text documents" << endl;
  out << "" << endl;
  out << "Options:" << endl;
  out << " -i, --injector <injector>\tset injection type, possible values:" << endl;
  out << "                          \t" << InjectorFactory::availableInjectors().join(", ")
      << endl;
  out << " -p, --pid <pid>          \tattach to running Qt application" << endl;
  out << "     --inprocess          \tuse in-process UI" << endl;
  out << "     --inject-only        \tonly inject the probe, don't show the UI" << endl;
  out << " -h, --help               \tprint program help and exit" << endl;
  out << " -v, --version            \tprint program version and exit" << endl;
  out << endl
      << "When run without any options, " << argv0 << " will present a list of running\n"
      << "Qt-applications from which you can attach the selected injector. Else,\n"
      << "you can attach to a running process by specifying its pid, or you can\n"
      << "start a new Qt-application by specifying its name (and optional arguments)."
      << endl;
}

static bool startLauncher()
{
  const QString launcherPath = LauncherFinder::findLauncher(LauncherFinder::LauncherUI);
  QProcess proc;
  proc.setProcessChannelMode(QProcess::ForwardedChannels);
  proc.start(launcherPath);
  proc.waitForFinished(-1);
  return proc.exitCode() == 0;
}

AbstractInjector::Ptr createInjector(const LaunchOptions& options)
{
  if (options.injectorType().isEmpty()) {
    if (options.isAttach()) {
      return InjectorFactory::defaultInjectorForAttach();
    } else {
      return InjectorFactory::defaultInjectorForLaunch();
    }
  }
  return InjectorFactory::createInjector(options.injectorType());
}

int main(int argc, char **argv)
{
  QCoreApplication::setOrganizationName("KDAB");
  QCoreApplication::setOrganizationDomain("kdab.com");
  QCoreApplication::setApplicationName("GammaRay");

  QStringList args;
  for (int i = 1; i < argc; ++i) {
    args.push_back(QString::fromLocal8Bit(argv[i]));
  }
#ifdef HAVE_QT_WIDGETS
  QApplication app(argc, argv); // for splash screen and style inspector
#else
  QCoreApplication app(argc, argv);
#endif

  QStringList builtInArgs = QStringList() << QLatin1String("-style")
                                          << QLatin1String("-stylesheet")
                                          << QLatin1String("-graphicssystem");

  LaunchOptions options;
  while (!args.isEmpty() && args.first().startsWith('-')) {
    const QString arg = args.takeFirst();
    if ((arg == QLatin1String("-i") || arg == QLatin1String("--injector")) && !args.isEmpty()) {
      options.setInjectorType(args.takeFirst());
      continue;
    }
    if ((arg == QLatin1String("-p") || arg == QLatin1String("--pid")) && !args.isEmpty()) {
      options.setPid( args.takeFirst().toInt() );
      continue;
    }
    if (arg == QLatin1String("-h") || arg == QLatin1String("--help")) {
      usage(argv[0]);
      return 0;
    }
    if (arg == QLatin1String("-v") || arg == QLatin1String("--version")) {
      out << PROGRAM_NAME << " version " << GAMMARAY_VERSION_STRING << endl;
      out << "Copyright (C) 2010-2013 Klaralvdalens Datakonsult AB, "
          << "a KDAB Group company, info@kdab.com" << endl;
      return 0;
    }
    if (arg == QLatin1String("--inprocess")) {
      options.setUiMode(LaunchOptions::InProcessUi);
    }
    if (arg == QLatin1String("--no-inprocess")) {
      options.setUiMode(LaunchOptions::OutOfProcessUi);
    }
    if (arg == QLatin1String("--inject-only")) {
      options.setUiMode(LaunchOptions::NoUi);
    }
    if (arg == QLatin1String("-filtertest")) {
      qputenv("GAMMARAY_TEST_FILTER", "1");
    }
    if (arg == QLatin1String("-unittest")) {
      qputenv("GAMMARAY_UNITTEST", "1");
    }
    if (arg == QLatin1String("-modeltest")) {
      qputenv("GAMMARAY_MODELTEST", "1");
    }
    // built-in arguments of QApp, could be meant for us if we are showing the launcher window
    foreach (const QString &builtInArg, builtInArgs) {
      if (arg == builtInArg && !args.isEmpty()) {
        args.takeFirst();
      }
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

  const QString probeDll = ProbeFinder::findProbe(QLatin1String("gammaray_probe"));
  qputenv("GAMMARAY_PROBE_PATH", QFileInfo(probeDll).absolutePath().toLocal8Bit());
  options.sendProbeSettings();

  const AbstractInjector::Ptr injector = createInjector(options);
  if (injector) {
    ClientLauncher client;
    if (options.uiMode() == LaunchOptions::OutOfProcessUi && !client.launch("127.0.0.1")) {
      err << "Failed to launch GammaRay client!";
      return 1;
    }
    if (options.isAttach()) {
      if (!injector->attach(options.pid(), probeDll, QLatin1String("gammaray_probe_inject"))) {
        err << "Unable to attach injector " << injector->name() << endl;
        err << "Exit code: " << injector->exitCode() << endl;
        if (!injector->errorString().isEmpty()) {
          err << "Error: " << injector->errorString() << endl;
        }
        client.terminate();
        return 1;
      } else {
        client.waitForFinished();
        return 0;
      }
    } else {
      if (!injector->launch(options.launchArguments(), probeDll, QLatin1String("gammaray_probe_inject"))) {
        err << "Failed to launch injector " << injector->name() << endl;
        err << "Exit code: " << injector->exitCode() << endl;
        if (!injector->errorString().isEmpty()) {
          err << "Error: " << injector->errorString() << endl;
        }
        client.terminate();
        return 1;
      }
      client.waitForFinished();
      return injector->exitCode();
    }
  }

  if (options.injectorType().isEmpty()) {
    if (options.isAttach()) {
      err << "Uh-oh, there is no default attach injector" << endl;
    } else {
      err << "Uh-oh, there is no default launch injector" << endl;
    }
  } else {
    err << "Injector " << options.injectorType() << " not found." << endl;
  }
  return 1;
}
