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
#ifndef ENDOSCOPETOOL_H
#define ENDOSCOPETOOL_H

#include <analyzerbase/ianalyzertool.h>

namespace Endoscope {

class EndoscopeTool : public Analyzer::IAnalyzerTool
{
  public:
    explicit EndoscopeTool(QObject *parent = 0);

    virtual void startTool(Analyzer::StartMode mode);

    virtual Analyzer::IAnalyzerEngine *createEngine(
      const Analyzer::AnalyzerStartParameters &sp,
      ProjectExplorer::RunConfiguration *runConfiguration = 0);

    virtual QWidget *createWidgets();
    virtual void extensionsInitialized();

    virtual Analyzer::IAnalyzerTool::ToolMode toolMode() const;

    virtual QString description() const;
    virtual QString displayName() const;
    virtual QByteArray id() const;
};

} // namespace Endoscope

#endif // ENDOSCOPETOOL_H
