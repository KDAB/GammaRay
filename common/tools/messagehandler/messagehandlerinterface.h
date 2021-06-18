/*
  messagehandlerinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAMMARAY_MESSAGEHANDLER_MESSAGEHANDLERINTERFACE_H
#define GAMMARAY_MESSAGEHANDLER_MESSAGEHANDLERINTERFACE_H

#include <QObject>

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
