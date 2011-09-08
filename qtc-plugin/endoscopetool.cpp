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

#include "endoscopetool.h"

#include "endoscopeengine.h"
#include "endoscopeconstants.h"

#include <analyzerbase/analyzermanager.h>

#include <QtGui/QWidget>

using namespace Analyzer;

namespace Endoscope {

EndoscopeTool::EndoscopeTool(QObject *parent)
: IAnalyzerTool(parent)
{
    setObjectName("EndoscopeTool");
}

void EndoscopeTool::startTool(StartMode mode)
{
    AnalyzerManager::startLocalTool(this, mode);
}

IAnalyzerEngine *EndoscopeTool::createEngine(const AnalyzerStartParameters &sp,
                                             ProjectExplorer::RunConfiguration *runConfiguration)
{
    EndoscopeEngine *engine = new EndoscopeEngine(this, sp, runConfiguration);

    AnalyzerManager::showStatusMessage(AnalyzerManager::msgToolStarted(displayName()));
    return engine;
}

QWidget *EndoscopeTool::createWidgets()
{
    return new QWidget;
}

void EndoscopeTool::extensionsInitialized()
{

}

IAnalyzerTool::ToolMode EndoscopeTool::toolMode() const
{
    return AnyMode;
}

QString EndoscopeTool::description() const
{
    return tr("TODO");
}

QString EndoscopeTool::displayName() const
{
    return tr("Endoscope");
}

QByteArray EndoscopeTool::id() const
{
    return Constants::TOOLID;
}

} // namespace Endoscope
