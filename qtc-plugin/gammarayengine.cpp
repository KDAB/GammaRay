/**************************************************************************
**
** This file is part of Qt Creator Instrumentation Tools
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Author: Milian Wolff, KDAB (milian.wolff@kdab.com)
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#include "gammarayengine.h"

#include "gammaraysettings.h"

#include <analyzerbase/analyzermanager.h>
#include <analyzerbase/analyzerstartparameters.h>

#include <coreplugin/icore.h>
#include <coreplugin/progressmanager/progressmanager.h>
#include <coreplugin/progressmanager/futureprogress.h>

#include <utils/qtcassert.h>
#include <utils/qtcprocess.h>

#include <projectexplorer/runconfiguration.h>

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QAction>

using namespace Analyzer;

namespace GammaRay {

GammaRayEngine::GammaRayEngine(IAnalyzerTool *tool,
                                 const AnalyzerStartParameters &sp,
                                 ProjectExplorer::RunConfiguration *runConfiguration)
: IAnalyzerEngine(tool, sp, runConfiguration), m_settings(0)
{
  AnalyzerSettings *settings = 0;
  if (runConfiguration) {
    settings = runConfiguration->extraAspect<AnalyzerProjectSettings>();
  }
  if (!settings) {
    settings = AnalyzerGlobalSettings::instance();
  }

  m_settings = settings->subConfig<GammaRayBaseSettings>();
  QTC_CHECK(m_settings);

  connect(&m_progressWatcher, SIGNAL(canceled()),
          this, SLOT(handleProgressCanceled()));
  connect(&m_progressWatcher, SIGNAL(finished()),
          this, SLOT(handleProgressFinished()));

  connect(&m_process, SIGNAL(readyReadStandardOutput()),
          SLOT(receiveStandardOutput()));
  connect(&m_process, SIGNAL(readyReadStandardError()),
          SLOT(receiveStandardError()));
  connect(&m_process, SIGNAL(finished(int)),
          SLOT(processFinished()));
  connect(&m_process, SIGNAL(error(QProcess::ProcessError)),
          SLOT(processError(QProcess::ProcessError)));

  connect(AnalyzerManager::stopAction(), SIGNAL(triggered()), this, SLOT(stopIt()));
}

GammaRayEngine::~GammaRayEngine()
{
}

void GammaRayEngine::handleProgressCanceled()
{
  AnalyzerManager::stopTool();
}

void GammaRayEngine::handleProgressFinished()
{
  QApplication::alert(Core::ICore::instance()->mainWindow(), 3000);
}

bool GammaRayEngine::start()
{
  emit starting(this);

  Core::FutureProgress *fp =
    Core::ICore::instance()->progressManager()->addTask(m_progress.future(),
                                                        tr("Investigating Application"),
                                                        "gammaray");
  fp->setKeepOnFinish(Core::FutureProgress::HideOnFinish);
  m_progress.reportStarted();
  m_progressWatcher.setFuture(m_progress.future());

  const AnalyzerStartParameters &sp = startParameters();
  m_process.setWorkingDirectory(sp.workingDirectory);

  QString exe = m_settings->gammarayExecutable();
  if (!sp.analyzerCmdPrefix.isEmpty()) {
    exe = sp.analyzerCmdPrefix + ' ' + exe;
  }

  QString arguments;
  switch(m_settings->injector()) {
  case Constants::DefaultInjector:
    // nothing
    break;
  case Constants::GDBInjector:
    Utils::QtcProcess::addArg(&arguments, "-i gdb");
  case Constants::StyleInjector:
    Utils::QtcProcess::addArg(&arguments, "-i style");
    break;
#ifndef Q_OS_WIN
  case Constants::PreloadInjector:
    Utils::QtcProcess::addArg(&arguments, "-i preload");
    break;
#else
  case Constants::WinDLLInjector:
    Utils::QtcProcess::addArg(&arguments, "-i windll");
    break;
  case Constants::DetourInjector:
    Utils::QtcProcess::addArg(&arguments, "-i detour");
    break;
#endif
  }

  Utils::QtcProcess::addArg(&arguments, sp.debuggee);
  Utils::QtcProcess::addArgs(&arguments, sp.debuggeeArgs);

  m_process.setCommand(exe, arguments);

  m_process.setEnvironment(sp.environment);

  m_process.start();

  return true;
}

void GammaRayEngine::stop()
{
  m_process.terminate();
}

void GammaRayEngine::stopIt()
{
  stop();
}

void GammaRayEngine::receiveStandardOutput()
{
  emit outputReceived(m_process.readAllStandardOutput(), Utils::StdOutFormat);
}

void GammaRayEngine::receiveStandardError()
{
  emit outputReceived(m_process.readAllStandardError(), Utils::StdErrFormat);
}

void GammaRayEngine::processFinished()
{
  emit outputReceived(tr("** Analyzing finished **\n"), Utils::NormalMessageFormat);
  emit finished();

  m_progress.reportFinished();
}

void GammaRayEngine::processError(QProcess::ProcessError error)
{
  emit outputReceived(m_process.errorString(), Utils::ErrorMessageFormat);
}

}
