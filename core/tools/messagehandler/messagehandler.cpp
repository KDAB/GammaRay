/*
  messagehandler.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "messagemodel.h"
#include "ui_messagehandler.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QMutex>
#include <QSortFilterProxyModel>
#include <QThread>

static QTextStream cerr(stdout);

using namespace GammaRay;

static MessageModel *s_model = 0;
static QtMsgHandler s_handler = 0;
static bool s_handlerDisabled = false;
static QMutex s_mutex(QMutex::Recursive);

void handleMessage(QtMsgType type, const char *msg)
{
  ///WARNING: do not trigger *any* kind of debug output here
  ///         this would trigger an infinite loop and hence crash!

  MessageModel::Message message;
  message.type = type;
  message.message = QString::fromLocal8Bit(msg);
  message.time = QTime::currentTime();

  if (type == QtCriticalMsg || type == QtFatalMsg || type == QtWarningMsg) {
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
    if (removeUntil != -1) {
      message.backtrace = message.backtrace.mid(removeUntil + 1);
    }
  }

  if (type == QtFatalMsg && qgetenv("GAMMARAY_GDB") != "1" && qgetenv("GAMMARAY_UNITTEST") != "1" &&
      QThread::currentThread() == QApplication::instance()->thread()) {
    foreach (QWidget *w, qApp->topLevelWidgets()) {
      w->setEnabled(false);
    }
    QDialog dlg;
    dlg.setWindowTitle(QObject::tr("QFatal in %1").
                       arg(qApp->applicationName().isEmpty() ?
                           qApp->applicationFilePath() :
                           qApp->applicationName()));
    QGridLayout *layout = new QGridLayout;
    QLabel *iconLabel = new QLabel;
    QIcon icon = dlg.style()->standardIcon(QStyle::SP_MessageBoxCritical, 0, &dlg);
    int iconSize = dlg.style()->pixelMetric(QStyle::PM_MessageBoxIconSize, 0, &dlg);
    iconLabel->setPixmap(icon.pixmap(iconSize, iconSize));
    iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(iconLabel, 0, 0);
    QLabel *errorLabel = new QLabel;
    errorLabel->setTextFormat(Qt::PlainText);
    errorLabel->setWordWrap(true);
    errorLabel->setText(message.message);
    layout->addWidget(errorLabel, 0, 1);
    if (!message.backtrace.isEmpty()) {
      QListWidget *backtrace = new QListWidget;
      foreach (const QString &frame, message.backtrace) {
        backtrace->addItem(frame);
      }
      layout->addWidget(backtrace, 1, 0, 1, 2);
    }
    QDialogButtonBox *buttons = new QDialogButtonBox;
    buttons->addButton(QDialogButtonBox::Close);
    QObject::connect(buttons, SIGNAL(accepted()),
                     &dlg, SLOT(accept()));
    QObject::connect(buttons, SIGNAL(rejected()),
                     &dlg, SLOT(reject()));
    layout->addWidget(buttons, 2, 0, 1, 2);
    dlg.setLayout(layout);
    dlg.adjustSize();
    dlg.exec();
  } else if (!message.backtrace.isEmpty() &&
             (qgetenv("GAMMARAY_UNITTEST") == "1" || type == QtFatalMsg)) {
    cerr << "START BACKTRACE:" << endl;
    int i = 0;
    foreach (const QString &frame, message.backtrace) {
      cerr << (++i) << "\t" << frame << endl;
    }
    cerr << "END BACKTRACE" << endl;
  }

  // reset msg handler so the app still works as usual
  // but make sure we don't let other threads bypass our
  // handler during that time
  QMutexLocker lock(&s_mutex);
  s_handlerDisabled = true;
  qInstallMsgHandler(s_handler);
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
  qt_message_output(type, QMessageLogContext(), msg);
#else
  qt_message_output(type, msg);
#endif
  qInstallMsgHandler(handleMessage);
  s_handlerDisabled = false;
  lock.unlock();

  if (s_model) {
    // add directly from foreground thread, delay from background thread
    QMetaObject::invokeMethod(s_model, "addMessage", Qt::AutoConnection,
                              Q_ARG(MessageModel::Message, message));
  }
}

MessageHandler::MessageHandler(ProbeInterface * /*probe*/, QWidget *parent)
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

  // install handler directly, catches most cases,
  // i.e. user has no special handler or the handler
  // is created before the QApplication
  ensureHandlerInstalled();
  // recheck when eventloop is entered, if the user
  // installs a handler after QApp but before .exec()
  QMetaObject::invokeMethod(this, "ensureHandlerInstalled", Qt::QueuedConnection);
}

void MessageHandlerFactory::ensureHandlerInstalled()
{
  QMutexLocker lock(&s_mutex);

  if (s_handlerDisabled) {
    return;
  }

  QtMsgHandler prevHandler = qInstallMsgHandler(handleMessage);

  if (prevHandler != handleMessage) {
    s_handler = prevHandler;
  }
}

MessageHandlerFactory::~MessageHandlerFactory()
{
  QMutexLocker lock(&s_mutex);

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
  QWidget *widget =
    StandardToolFactory<QObject, MessageHandler >::createWidget(probe, parentWidget);

  MessageHandler *handler = qobject_cast<MessageHandler*>(widget);
  Q_ASSERT(handler);
  handler->setModel(m_messageModel);
  return widget;
}

#include "messagehandler.moc"
