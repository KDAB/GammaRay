/*
  problemreporter.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/
#ifndef GAMMARAY_PROBLEMREPORTER_PROBLEMREPORTER_H
#define GAMMARAY_PROBLEMREPORTER_PROBLEMREPORTER_H

#include <core/toolfactory.h>
#include <common/tools/problemreporter/problemreporterinterface.h>

namespace GammaRay {

class ProblemModel;

class ProblemReporter : public ProblemReporterInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ProblemReporterInterface)
public:
    explicit ProblemReporter(Probe *probe, QObject *parent = nullptr);
    ~ProblemReporter() override;

public slots:
    void requestScan() override;

private:
    ProblemModel *m_problemModel;
};

class ProblemReporterFactory : public QObject, public StandardToolFactory<QObject, ProblemReporter>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
public:
    explicit ProblemReporterFactory(QObject *parent)
        : QObject(parent)
    {
    }
};
}

#endif // PROBLEMREPORTER_H
