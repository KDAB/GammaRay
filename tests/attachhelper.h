/*
  attachhelper.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef TEST_GAMMARAY_ATTACHHELPER_H
#define TEST_GAMMARAY_ATTACHHELPER_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QTimer;
class QProcess;
QT_END_NAMESPACE

class AttachHelper : public QObject
{
    Q_OBJECT
public:
    explicit AttachHelper(const QString &gammaray, const QString &injector, const QString &debuggee,
                          const QStringList &arguments, QObject *parent = nullptr);

public slots:
    void attach();
    void processStarted();
    static void processFinished(int);

private:
    QTimer *m_timer;
    QProcess *m_proc;
    QString m_gammaray;
    QString m_injector;
};

#endif // ATTACHHELPER_H
