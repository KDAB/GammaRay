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
#include "launcherwindow.h"

#include <QApplication>
#include <QDebug>
#include <QStringList>
#include <QFileInfo>

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
  out << " -h, --help               \tprint program help and exit" << endl;
  out << " -v, --version            \tprint program version and exit" << endl;
  out << endl
      << "When run without any options, " << argv0 << " will present a list of running\n"
      << "Qt-applications from which you can attach the selected injector. Else,\n"
      << "you can attach to a running process by specifying its pid, or you can\n"
      << "start a new Qt-application by specifying its name (and optional arguments)."
      << endl;
}

int main(int argc, char **argv)
{
  QApplication::setOrganizationName("KDAB");
  QApplication::setOrganizationDomain("kdab.com");
  QApplication::setApplicationName("GammaRay");

  QStringList args;
  for (int i = 1; i < argc; ++i) {
    args.push_back(QString::fromLocal8Bit(argv[i]));
  }
  QApplication app(argc, argv);

  QStringList builtInArgs = QStringList() << QLatin1String("-style")
                                          << QLatin1String("-stylesheet")
                                          << QLatin1String("-graphicssystem");

  QString injectorType;
  int pid = -1;
  while (!args.isEmpty() && args.first().startsWith('-')) {
    const QString arg = args.takeFirst();
    if ((arg == QLatin1String("-i") || arg == QLatin1String("--injector")) && !args.isEmpty()) {
      injectorType = args.takeFirst();
      continue;
    }
    if ((arg == QLatin1String("-p") || arg == QLatin1String("--pid")) && !args.isEmpty()) {
      pid = args.takeFirst().toInt();
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

  if (args.isEmpty() && pid <= 0) {
    LauncherWindow dialog;
    if (dialog.exec() == QDialog::Accepted) {
      args = dialog.launchArguments();
      bool ok;
      pid = dialog.pid().toInt(&ok);
      if (!ok && args.isEmpty()) {
        return 0;
      }
    } else {
      return 0;
    }
  }

  const QString probeDll = ProbeFinder::findProbe(QLatin1String("gammaray_probe"));
  qputenv("GAMMARAY_PROBE_PATH", QFileInfo(probeDll).absolutePath().toLocal8Bit());

  AbstractInjector::Ptr injector;
  if (injectorType.isEmpty()) {
    if (pid > 0) {
      injector = InjectorFactory::defaultInjectorForAttach();
    } else {
      injector = InjectorFactory::defaultInjectorForLaunch();
    }
  } else {
    injector = InjectorFactory::createInjector(injectorType);
  }

  if (injector) {
    if (pid > 0) {
      if (!injector->attach(pid, probeDll, QLatin1String("gammaray_probe_inject"))) {
        err << "Unable to attach injector " << injector->name() << endl;
        err << "Exit code: " << injector->exitCode() << endl;
        if (!injector->errorString().isEmpty()) {
          err << "Error: " << injector->errorString() << endl;
        }
        return 1;
      } else {
        return 0;
      }
    } else {
      if (!injector->launch(args, probeDll, QLatin1String("gammaray_probe_inject"))) {
        err << "Failed to launch injector " << injector->name() << endl;
        err << "Exit code: " << injector->exitCode() << endl;
        if (!injector->errorString().isEmpty()) {
          err << "Error: " << injector->errorString() << endl;
        }
        return 1;
      }
      return injector->exitCode();
    }
    return 1;
  }

  if (injectorType.isEmpty()) {
    if (pid > 0) {
#if defined(Q_OS_WIN)
      err << "Sorry, but at this time there is no attach injector on the Windows platform" << endl;
      err << "Only the launch injector windll is available on Windows" << endl;
#else
      err << "Uh-oh, there is no default attach injector" << endl;
#endif
    } else {
      err << "Uh-oh, there is no default launch injector" << endl;
    }
  } else {
    err << "Injector " << injectorType << " not found." << endl;
  }
  return 1;
}
