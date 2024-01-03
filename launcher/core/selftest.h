/*
  selftest.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SELFTEST_H
#define GAMMARAY_SELFTEST_H

#include "gammaray_launcher_export.h"

#include <QObject>

namespace GammaRay {
/** @brief Self-test functions for probes and injectors. */
class GAMMARAY_LAUNCHER_EXPORT SelfTest : public QObject
{
    Q_OBJECT
public:
    explicit SelfTest(QObject *parent = nullptr);
    ~SelfTest() override;

    /** Run all available tests. */
    bool checkEverything();

    /** Run tests for one specific injector only. */
    bool checkInjector(const QString &injectorType);

signals:
    /** Emitted for informational messages, ie. passing tests. */
    void information(const QString &msg);
    /** Emitted for failed tests. */
    void error(const QString &msg);

private:
    bool checkProbes();
    bool checkInjectors();
};
}

#endif // GAMMARAY_SELFTEST_H
