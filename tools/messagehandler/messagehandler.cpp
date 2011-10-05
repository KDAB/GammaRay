/*
  messagehandler.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "ui_messagehandler.h"

#include "messagemodel.h"

#include <QtGui/QSortFilterProxyModel>
#include <QtCore/QMutex>
#include <QDebug>

using namespace GammaRay;

static MessageModel *s_model = 0;
static QtMsgHandler s_handler = 0;
static QMutex s_mutex;

void handleMessage(QtMsgType type, const char *msg)
{
  ///WARNING: do not trigger *any* kind of debug output here
  ///         this would trigger an infinite loop and hence crash!

  MessageModel::Message message;
  message.type = type;
  message.message = QString::fromLocal8Bit(msg);
  message.time = QTime::currentTime();

  // reset msg handler so the app still works as usual
  // but make sure we don't let other threads bypass our
  // handler during that time
  QMutexLocker lock(&s_mutex);
  qInstallMsgHandler(s_handler);
  qt_message_output(type, msg);
  qInstallMsgHandler(handleMessage);
  lock.unlock();

  if (s_model) {
    // add directly from foreground thread, delay from background thread
    QMetaObject::invokeMethod(s_model, "addMessage", Qt::AutoConnection,
                              Q_ARG(MessageModel::Message, message));
  }
}

MessageHandler::MessageHandler(ProbeInterface */*probe*/, QWidget *parent)
  : QWidget(parent),
    ui(new Ui::MessageHandler),
    m_messageModel(0),
    m_messageProxy(new QSortFilterProxyModel(this))
{
  ui->setupUi(this);

  ui->messageSearchLine->setProxy(m_messageProxy);
  ui->messageView->setModel(m_messageProxy);
  ui->messageView->setIndentation(0);
  ui->messageView->setSortingEnabled(true);

  ///FIXME: implement this
  ui->backtraceView->hide();
}

void MessageHandler::setModel(MessageModel *model)
{
  m_messageModel = model;
  m_messageProxy->setSourceModel(m_messageModel);
}

MessageHandlerFactory::MessageHandlerFactory(QObject *parent)
  : QObject(parent),
    m_messageModel(new MessageModel(this))
{
  Q_ASSERT(s_model == 0);
  s_model = m_messageModel;
  s_handler = qInstallMsgHandler(handleMessage);
}

MessageHandlerFactory::~MessageHandlerFactory()
{
  s_model = 0;
  QtMsgHandler oldHandler = qInstallMsgHandler(s_handler);
  if (oldHandler != handleMessage) {
    // ups, the app installed it's own handler after ours...
    qInstallMsgHandler(oldHandler);
  }
  s_handler = 0;
}

QWidget *MessageHandlerFactory::createWidget(ProbeInterface *probe, QWidget *parentWidget)
{
  QWidget *widget = StandardToolFactory<QObject, MessageHandler >::createWidget(probe, parentWidget);
  MessageHandler *handler = qobject_cast<MessageHandler*>(widget);
  Q_ASSERT(handler);
  handler->setModel(m_messageModel);
  return widget;
}

#include "messagehandler.moc"
