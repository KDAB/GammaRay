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

#include "gammaraytool.h"

#include "gammarayengine.h"
#include "gammarayconstants.h"

#include <analyzerbase/analyzermanager.h>

#include <QtGui/QWidget>

using namespace Analyzer;

namespace GammaRay {

GammaRayTool::GammaRayTool(QObject *parent)
: IAnalyzerTool(parent)
{
    setObjectName("GammaRayTool");
}

void GammaRayTool::startTool(StartMode mode)
{
    AnalyzerManager::startLocalTool(this, mode);
}

IAnalyzerEngine *GammaRayTool::createEngine(const AnalyzerStartParameters &sp,
                                             ProjectExplorer::RunConfiguration *runConfiguration)
{
    GammaRayEngine *engine = new GammaRayEngine(this, sp, runConfiguration);

    AnalyzerManager::showStatusMessage(AnalyzerManager::msgToolStarted(displayName()));
    return engine;
}

QWidget *GammaRayTool::createWidgets()
{
    return new QWidget;
}

void GammaRayTool::extensionsInitialized()
{

}

IAnalyzerTool::ToolMode GammaRayTool::toolMode() const
{
    return AnyMode;
}

QString GammaRayTool::description() const
{
    return tr("TODO");
}

QString GammaRayTool::displayName() const
{
    return tr("GammaRay");
}

QByteArray GammaRayTool::id() const
{
    return Constants::TOOLID;
}

} // namespace GammaRay
