/*
  messagehandler.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "messagehandler.h"
#include "messagemodel.h"
#include "loggingcategorymodel.h"

#include <core/execution.h>
#include <core/probeguard.h>
#include <core/remote/serverproxymodel.h>
#include <core/stacktracemodel.h>

#include "common/objectbroker.h"
#include "common/endpoint.h"

#include <QCoreApplication>
#include <QDebug>
#include <QItemSelectionModel>
#include <QMutex>
#include <QSortFilterProxyModel>
#include <QThread>

#include <iostream>

using namespace GammaRay;

using MessageHandlerCallback = QtMessageHandler;
static MessageHandlerCallback (*const installMessageHandler)(MessageHandlerCallback) = qInstallMessageHandler;

static MessageModel *s_model = nullptr;
static MessageHandlerCallback s_handler = nullptr;
static bool s_handlerDisabled = false;
static QRecursiveMutex s_mutex;

static void handleMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    /// WARNING: do not trigger *any* kind of debug output here
    ///          this would trigger an infinite loop and hence crash!

    if (s_handlerDisabled) // recursion detected
        return;

    DebugMessage message;
    message.type = type;
    message.message = msg;
    message.time = QTime::currentTime();
    message.category = QString::fromUtf8(context.category);
    message.file = QString::fromUtf8(context.file);
    message.function = QString::fromUtf8(context.function);
    message.line = context.line;

    if (type == QtCriticalMsg || type == QtFatalMsg || (type == QtWarningMsg && !ProbeGuard::insideProbe())) {
        // TODO: go even higher until qWarning/qFatal/qDebug/... ?
        message.backtrace = Execution::stackTrace(50, 1); // skip this, ie. start at our caller
    }

    if (!message.backtrace.empty()
        && (qgetenv("GAMMARAY_UNITTEST") == "1" || type == QtFatalMsg)) {
        if (type == QtFatalMsg)
            std::cerr << "QFatal in " << qPrintable(qApp->applicationName()) << " (" << qPrintable(qApp->applicationFilePath()) << ')' << std::endl;
        std::cerr << "START BACKTRACE:" << std::endl;
        int i = 0;
        foreach (const auto &frame, Execution::resolveAll(message.backtrace))
            std::cerr << (++i) << "\t" << qPrintable(frame.name) << " (" << qPrintable(frame.location.displayString()) << ")" << std::endl;
        std::cerr << "END BACKTRACE" << std::endl;
    }

    if (type == QtFatalMsg && qgetenv("GAMMARAY_GDB") != "1"
        && qgetenv("GAMMARAY_UNITTEST") != "1") {
        // Enforce handling on the GUI thread and block until we are done.
        QMetaObject::invokeMethod(static_cast<QObject *>(s_model)->parent(), "handleFatalMessage",
                                  qApp->thread() == QThread::currentThread() ? Qt::DirectConnection : Qt::BlockingQueuedConnection,
                                  Q_ARG(GammaRay::DebugMessage, message));
    }

    // reset msg handler so the app still works as usual
    // but make sure we don't let other threads bypass our
    // handler during that time
    QMutexLocker lock(&s_mutex);
    s_handlerDisabled = true;
    if (s_handler) { // try a direct call to the previous handler first, that avoids triggering the recursion detection in Qt5
        s_handler(type, context, msg);
    } else {
        installMessageHandler(s_handler);
        qt_message_output(type, context, msg);
        installMessageHandler(handleMessage);
    }
    s_handlerDisabled = false;
    lock.unlock();

    if (s_model) {
        // add directly from foreground thread, delay from background thread
        QMetaObject::invokeMethod(s_model, "addMessage", Qt::AutoConnection,
                                  Q_ARG(GammaRay::DebugMessage, message));
    }
}

MessageHandler::MessageHandler(Probe *probe, QObject *parent)
    : MessageHandlerInterface(parent)
    , m_messageModel(new MessageModel(this))
    , m_stackTraceModel(new StackTraceModel(this))
{
    Q_ASSERT(s_model == nullptr);
    s_model = m_messageModel;

    auto proxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    proxy->addRole(MessageModelRole::Type);
    proxy->addRole(MessageModelRole::Line);
    proxy->setSourceModel(m_messageModel);
    proxy->setSortRole(MessageModelRole::Sort);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.MessageModel"), proxy);

    auto selModel = ObjectBroker::selectionModel(proxy);
    connect(selModel, &QItemSelectionModel::selectionChanged, this, &MessageHandler::messageSelected);

    probe->registerModel(QStringLiteral("com.kdab.GammaRay.MessageStackTraceModel"), m_stackTraceModel);

    // install handler directly, catches most cases,
    // i.e. user has no special handler or the handler
    // is created before the QApplication
    ensureHandlerInstalled();
    // recheck when eventloop is entered, if the user
    // installs a handler after QApp but before .exec()
    QMetaObject::invokeMethod(this, "ensureHandlerInstalled", Qt::QueuedConnection);

    auto catModel = new LoggingCategoryModel(this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.LoggingCategoryModel"), catModel);
}

MessageHandler::~MessageHandler()
{
    QMutexLocker lock(&s_mutex);

    s_model = nullptr;
    MessageHandlerCallback oldHandler = installMessageHandler(s_handler);
    if (oldHandler != handleMessage) {
        // ups, the app installed it's own handler after ours...
        installMessageHandler(oldHandler);
    }
    s_handler = nullptr;
}

void MessageHandler::generateFullTrace()
{
    setFullTrace(m_stackTraceModel->fullTrace());
}

void MessageHandler::ensureHandlerInstalled()
{
    QMutexLocker lock(&s_mutex);

    if (s_handlerDisabled)
        return;

    MessageHandlerCallback prevHandler = installMessageHandler(handleMessage);

    if (prevHandler != handleMessage)
        s_handler = prevHandler;
}

void MessageHandler::handleFatalMessage(const DebugMessage &message)
{
    const QString app = qApp->applicationName().isEmpty()
        ? qApp->applicationFilePath()
        : qApp->applicationName();
    QStringList bt;
    bt.reserve(message.backtrace.size());
    foreach (const auto &frame, Execution::resolveAll(message.backtrace)) {
        if (frame.location.isValid())
            bt.push_back(frame.name + QLatin1String(" (") + frame.location.displayString() + QLatin1Char(')'));
        else
            bt.push_back(frame.name);
    }
    emit fatalMessageReceived(app, message.message, message.time, bt);
    if (Endpoint::isConnected())
        Endpoint::instance()->waitForMessagesWritten();
}

void MessageHandler::messageSelected(const QItemSelection &selection)
{
    if (selection.isEmpty()) {
        setStackTraceAvailable(false);
        return;
    }

    const auto idx = selection.at(0).topLeft();
    m_stackTraceModel->setStackTrace(idx.data(MessageModelRole::Backtrace).value<Execution::Trace>());
    setStackTraceAvailable(m_stackTraceModel->rowCount() > 0);
}

MessageHandlerFactory::MessageHandlerFactory(QObject *parent)
    : QObject(parent)
{
}
