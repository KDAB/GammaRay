/*
  signalmonitortest.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
