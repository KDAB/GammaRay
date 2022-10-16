/*
  signalmonitortest.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_SIGNALMONITORTEST_H
#define GAMMARAY_SIGNALMONITORTEST_H

#include <QObject>

class SignalMonitorTest : public QObject
{
    Q_OBJECT

public:
    explicit SignalMonitorTest(QObject *parent = nullptr);

private:
    QString nextTimerName();

private slots:
    void onTimeout();

private:
    int m_timerCount;
};

#endif // GAMMARAY_SIGNALMONITORTEST_H
