/*
  messagehandlerwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "messagehandlerwidget.h"
#include "ui_messagehandlerwidget.h"

#include <common/endpoint.h>
#include <common/objectbroker.h>
#include "messagehandlerclient.h"

#include <QSortFilterProxyModel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QTime>

using namespace GammaRay;

static QObject *createClientMessageHandler(const QString &/*name*/, QObject *parent)
{
  return new MessageHandlerClient(parent);
}

MessageHandlerWidget::MessageHandlerWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::MessageHandlerWidget)
{
  ObjectBroker::registerClientObjectFactoryCallback<MessageHandlerInterface*>(createClientMessageHandler);
  MessageHandlerInterface *handler = ObjectBroker::object<MessageHandlerInterface*>();

  connect(handler, SIGNAL(fatalMessageReceived(QString,QString,QTime,QStringList)),
          this, SLOT(fatalMessageReceived(QString,QString,QTime,QStringList)));

  ui->setupUi(this);

  QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
  proxy->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.MessageModel"));
  ui->messageSearchLine->setProxy(proxy);
  ui->messageView->setModel(proxy);
  ui->messageView->setIndentation(0);
  ui->messageView->setSortingEnabled(true);

  ///FIXME: implement this
  ui->backtraceView->hide();
}

MessageHandlerWidget::~MessageHandlerWidget()
{
}

void MessageHandlerWidget::fatalMessageReceived(const QString &app, const QString &message,
                                                const QTime &time, const QStringList &backtrace)
{
  if (Endpoint::isConnected() && !qobject_cast<MessageHandlerClient*>(ObjectBroker::object<MessageHandlerInterface*>())) {
    // only show on remote side
    return;
  }
  QDialog dlg;
  dlg.setWindowTitle(QObject::tr("QFatal in %1 at %2").arg(app).arg(time.toString()));

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
  errorLabel->setText(message);
  layout->addWidget(errorLabel, 0, 1);

  if (!backtrace.isEmpty()) {
    QListWidget *backtraceWidget = new QListWidget;
    foreach (const QString &frame, backtrace) {
      backtraceWidget->addItem(frame);
    }
    layout->addWidget(backtraceWidget, 1, 0, 1, 2);
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
}

