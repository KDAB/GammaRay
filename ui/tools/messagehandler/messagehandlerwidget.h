/*
  messagehandlerwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/
#ifndef GAMMARAY_MESSAGEHANDLERWIDGET_H
#define GAMMARAY_MESSAGEHANDLERWIDGET_H

#include <ui/uistatemanager.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QItemSelection;
class QTime;
QT_END_NAMESPACE

namespace GammaRay {
namespace Ui {
class MessageHandlerWidget;
}

class MessageHandlerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageHandlerWidget(QWidget *parent = nullptr);
    ~MessageHandlerWidget() override;

private slots:
    void fatalMessageReceived(const QString &app, const QString &message, const QTime &time,
                              const QStringList &backtrace);
    static void copyToClipboard(const QString &message);
    void messageContextMenu(const QPoint &pos);
    void stackTraceContextMenu(QPoint pos);
    void saveFileAllLogConfig();
    void saveFileModLogConfig();
    void saveFileLogConfig(bool all);
    void exportModLogConfig();

private:
    QScopedPointer<Ui::MessageHandlerWidget> ui;
    UIStateManager m_stateManager;
    QObject *m_backtraceFetchContext = nullptr;
};
}

#endif // MESSAGEHANDLERWIDGET_H
