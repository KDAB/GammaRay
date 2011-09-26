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

#include "gammarayruncontrolfactory.h"

#include "gammarayconstants.h"

#include <utils/qtcassert.h>

#include <analyzerbase/analyzerstartparameters.h>
#include <analyzerbase/analyzerruncontrol.h>
#include <analyzerbase/analyzermanager.h>
#include <analyzerbase/analyzersettings.h>
#include <analyzerbase/analyzerrunconfigwidget.h>

#include <projectexplorer/applicationrunconfiguration.h>

using namespace GammaRay;
using namespace ProjectExplorer;
using namespace Analyzer;

GammaRayRunControlFactory::GammaRayRunControlFactory(QObject *parent)
  : IRunControlFactory(parent)
{
  setObjectName(QLatin1String("GammaRayRuncontrolFactory"));
}

QString GammaRayRunControlFactory::displayName() const
{
  return tr("GammaRay");
}

bool GammaRayRunControlFactory::canRun(RunConfiguration *runConfiguration,
                                        const QString &mode) const
{
  return
    mode == Constants::RUNMODE &&
    dynamic_cast<LocalApplicationRunConfiguration*>(runConfiguration);
}

RunControl *GammaRayRunControlFactory::create(RunConfiguration *runConfiguration,
                                               const QString &mode)
{
  QTC_ASSERT(canRun(runConfiguration, mode), return 0);

  AnalyzerStartParameters sp;
  sp.toolId = Constants::TOOLID;
  sp.startMode = StartLocal;

  LocalApplicationRunConfiguration *rc =
    dynamic_cast<LocalApplicationRunConfiguration*>(runConfiguration);
  sp.environment = rc->environment();
  sp.workingDirectory = rc->workingDirectory();
  sp.debuggee = rc->executable();
  sp.debuggeeArgs = rc->commandLineArguments();
  sp.displayName = rc->displayName();

  IAnalyzerTool *tool = AnalyzerManager::toolFromId(Constants::TOOLID);
  AnalyzerRunControl *ret = new AnalyzerRunControl(tool, sp, runConfiguration);
  /// TODO
//     QObject::connect(AnalyzerManager::stopAction(), SIGNAL(triggered()), rc, SLOT(stopIt()));
  return ret;
}

RunConfigWidget *
GammaRayRunControlFactory::createConfigurationWidget(RunConfiguration *runConfiguration)
{
  AnalyzerProjectSettings *settings = runConfiguration->extraAspect<AnalyzerProjectSettings>();
  if (!settings) {
    return 0;
  }

  AnalyzerRunConfigWidget *ret = new Analyzer::AnalyzerRunConfigWidget;
  ret->setRunConfiguration(runConfiguration);
  return ret;
}

IRunConfigurationAspect *GammaRayRunControlFactory::createRunConfigurationAspect()
{
  return new AnalyzerProjectSettings;
}
