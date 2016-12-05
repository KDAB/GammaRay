/*
  messagehandler.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "messagehandler.h"
#include "messagemodel.h"
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
#include "loggingcategorymodel.h"
#endif

#include "backtrace.h"

#include <core/probeguard.h>
#include <core/remote/serverproxymodel.h>

#include "common/objectbroker.h"
#include "common/endpoint.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMutex>
#include <QSortFilterProxyModel>
#include <QThread>

#include <iostream>

using namespace GammaRay;

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
typedef QtMsgHandler MessageHandlerCallback;
static MessageHandlerCallback(*const installMessageHandler)(MessageHandlerCallback)
    = qInstallMsgHandler;
#else
typedef QtMessageHandler MessageHandlerCallback;
static MessageHandlerCallback(*const installMessageHandler)(MessageHandlerCallback)
    = qInstallMessageHandler;
#endif

static MessageModel *s_model = nullptr;
static MessageHandlerCallback s_handler = nullptr;
static bool s_handlerDisabled = false;
static QMutex s_mutex(QMutex::Recursive);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
static void handleMessage(QtMsgType type, const char *rawMsg)
#else
static void handleMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
#endif
{
    ///WARNING: do not trigger *any* kind of debug output here
    ///         this would trigger an infinite loop and hence crash!

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    const QString msg = QString::fromLocal8Bit(rawMsg);
#endif

    DebugMessage message;
    message.type = type;
    message.message = msg;
    message.time = QTime::currentTime();
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    message.category = QString::fromUtf8(context.category);
    message.file = QString::fromUtf8(context.file);
    message.function = QString::fromUtf8(context.function);
    message.line = context.line;
#endif

    if (type == QtCriticalMsg || type == QtFatalMsg
        || (type == QtWarningMsg && !ProbeGuard::insideProbe())) {
        message.backtrace = getBacktrace(50);
        // remove trailing internal functions
        // be a bit careful and first make sure that we find this function...
        // TODO: go even higher until qWarning/qFatal/qDebug/... ?
        int removeUntil = -1;
        for (int i = 0; i < message.backtrace.size(); ++i) {
            if (message.backtrace.at(i).contains(QLatin1String("handleMessage"))) {
                removeUntil = i;
                break;
            }
        }
        if (removeUntil != -1)
            message.backtrace = message.backtrace.mid(removeUntil + 1);
    }

    if (!message.backtrace.isEmpty()
        && (qgetenv("GAMMARAY_UNITTEST") == "1" || type == QtFatalMsg)) {
        if (type == QtFatalMsg)
            std::cerr << "QFatal in " << qPrintable(qApp->applicationName()) << " (" << qPrintable(
                qApp->applicationFilePath()) << ')' << std::endl;
        std::cerr << "START BACKTRACE:" << std::endl;
        int i = 0;
        foreach (const QString &frame, message.backtrace)
            std::cerr << (++i) << "\t" << qPrintable(frame) << std::endl;
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        s_handler(type, context, msg);
#else
        s_handler(type, rawMsg);
#endif
    } else {
        installMessageHandler(s_handler);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        qt_message_output(type, context, msg);
#else
        qt_message_output(type, rawMsg);
#endif
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

MessageHandler::MessageHandler(ProbeInterface *probe, QObject *parent)
    : MessageHandlerInterface(parent)
    , m_messageModel(new MessageModel(this))
{
    Q_ASSERT(s_model == nullptr);
    s_model = m_messageModel;

    auto proxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    proxy->addRole(MessageModelRole::Type);
    proxy->addRole(MessageModelRole::Line);
    proxy->addRole(MessageModelRole::Backtrace);
    proxy->setSourceModel(m_messageModel);
    proxy->setSortRole(MessageModelRole::Sort);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.MessageModel"), proxy);

    // install handler directly, catches most cases,
    // i.e. user has no special handler or the handler
    // is created before the QApplication
    ensureHandlerInstalled();
    // recheck when eventloop is entered, if the user
    // installs a handler after QApp but before .exec()
    QMetaObject::invokeMethod(this, "ensureHandlerInstalled", Qt::QueuedConnection);

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    auto catModel = new LoggingCategoryModel(this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.LoggingCategoryModel"), catModel);
#endif
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
    emit fatalMessageReceived(app, message.message, message.time, message.backtrace);
    if (Endpoint::isConnected())
        Endpoint::instance()->waitForMessagesWritten();
}

MessageHandlerFactory::MessageHandlerFactory(QObject *parent)
    : QObject(parent)
{
}
