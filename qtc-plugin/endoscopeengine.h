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
#ifndef ENDOSCOPEENGINE_H
#define ENDOSCOPEENGINE_H

#include <analyzerbase/ianalyzerengine.h>

#include <QtCore/QFutureWatcher>

#include <utils/qtcprocess.h>

namespace Endoscope {

class EndoscopeBaseSettings;

class EndoscopeEngine : public Analyzer::IAnalyzerEngine
{
    Q_OBJECT

public:
    EndoscopeEngine(Analyzer::IAnalyzerTool *tool,
                    const Analyzer::AnalyzerStartParameters &sp,
                    ProjectExplorer::RunConfiguration *runConfiguration);
    ~EndoscopeEngine();

    virtual bool start();
    virtual void stop();

private slots:
    void handleProgressCanceled();
    void handleProgressFinished();
    void processFinished();
    void processError(QProcess::ProcessError error);

    void receiveStandardOutput();
    void receiveStandardError();

private:
    Utils::QtcProcess m_process;
    QFutureInterface<void> m_progress;
    QFutureWatcher<void> m_progressWatcher;
    EndoscopeBaseSettings *m_settings;
};

}

#endif // ENDOSCOPEENGINE_H
