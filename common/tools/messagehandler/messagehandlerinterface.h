/*
  messagehandlerinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_MESSAGEHANDLER_MESSAGEHANDLERINTERFACE_H
#define GAMMARAY_MESSAGEHANDLER_MESSAGEHANDLERINTERFACE_H

#include <QObject>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QTime;
QT_END_NAMESPACE

namespace GammaRay {
class MessageHandlerInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool stackTraceAvailable READ stackTraceAvailable WRITE setStackTraceAvailable NOTIFY stackTraceAvailableChanged)
    Q_PROPERTY(QStringList fullTrace READ fullTrace WRITE setFullTrace NOTIFY fullTraceChanged)
public:
    explicit MessageHandlerInterface(QObject *parent = nullptr);
    ~MessageHandlerInterface() override;

    bool stackTraceAvailable() const;
    void setStackTraceAvailable(bool available);

    QStringList fullTrace() const;
    void setFullTrace(const QStringList &newFullTrace);

public slots:
    virtual void generateFullTrace() = 0;

signals:
    void fatalMessageReceived(const QString &app, const QString &message, const QTime &time,
                              const QStringList &backtrace);
    void stackTraceAvailableChanged(bool available);
    void fullTraceChanged();

private:
    bool m_stackTraceAvailable;
    QStringList m_fullTrace;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::MessageHandlerInterface, "com.kdab.GammaRay.MessageHandler")
QT_END_NAMESPACE

#endif // GAMMARAY_MESSAGEHANDLERINTERFACE_H
