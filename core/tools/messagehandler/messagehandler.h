/*
  messagehandler.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/
#ifndef GAMMARAY_MESSAGEHANDLER_MESSAGEHANDLER_H
#define GAMMARAY_MESSAGEHANDLER_MESSAGEHANDLER_H

#include "toolfactory.h"

#include <common/tools/messagehandler/messagehandlerinterface.h>

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
struct DebugMessage;
class MessageModel;
class StackTraceModel;

namespace Ui {
class MessageHandler;
}

class MessageHandler : public MessageHandlerInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::MessageHandlerInterface)
public:
    explicit MessageHandler(Probe *probe, QObject *parent = nullptr);
    ~MessageHandler() override;

public slots:
    void generateFullTrace() override;

private slots:
    static void ensureHandlerInstalled();
    void handleFatalMessage(const GammaRay::DebugMessage &message);
    void messageSelected(const QItemSelection &selection);

private:
    MessageModel *m_messageModel;
    StackTraceModel *m_stackTraceModel;
};

class MessageHandlerFactory : public QObject, public StandardToolFactory<QObject, MessageHandler>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
public:
    explicit MessageHandlerFactory(QObject *parent);
};
}

#endif // MESSAGEHANDLER_H
