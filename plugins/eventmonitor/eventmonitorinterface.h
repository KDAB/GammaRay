/*
  eventmonitorinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_EVENTMONITOR_EVENTMONITORINTERFACE_H
#define GAMMARAY_EVENTMONITOR_EVENTMONITORINTERFACE_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QSize;
QT_END_NAMESPACE

namespace GammaRay {
class EventMonitorInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isPaused READ isPaused WRITE setIsPaused NOTIFY isPausedChanged)

public:
    explicit EventMonitorInterface(QObject *parent = nullptr);
    ~EventMonitorInterface() override;

public slots:
    virtual void clearHistory() = 0;
    virtual void recordAll() = 0;
    virtual void recordNone() = 0;
    virtual void showAll() = 0;
    virtual void showNone() = 0;

    bool isPaused() const
    {
        return m_isPaused;
    }
    void setIsPaused(bool value);

signals:
    void isPausedChanged();

private:
    bool m_isPaused;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::EventMonitorInterface, "com.kdab.GammaRay.EventMonitorInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_EVENTMONITOR_EVENTMONITORINTERFACE_H
