/*
  problemreporterinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROBLEMREPORTERINTERFACE_H
#define GAMMARAY_PROBLEMREPORTERINTERFACE_H

#include <QObject>

namespace GammaRay {

/*! communication interface for the problem reporter tool. */
class ProblemReporterInterface : public QObject
{
    Q_OBJECT
public:
    explicit ProblemReporterInterface(QObject *parent = nullptr);
    ~ProblemReporterInterface() override;

signals:
    void problemScansFinished();

public slots:
    virtual void requestScan() = 0;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::ProblemReporterInterface, "com.kdab.GammaRay.ProblemReporterInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_PROBLEMREPORTERINTERFACE_H
