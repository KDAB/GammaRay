/*
  messagehandlerwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "messagehandlerwidget.h"
#include "ui_messagehandlerwidget.h"
#include "messagehandlerclient.h"
#include "messagedisplaymodel.h"

#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>
#include <ui/uiintegration.h>

#include <common/endpoint.h>
#include <common/objectbroker.h>
#include <common/tools/messagehandler/messagemodelroles.h>

#include <QSortFilterProxyModel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QTime>
#include <QPushButton>
#include <QClipboard>
#include <QApplication>
#include <QSignalMapper>
#include <QStringListModel>
#include <QUrl>

using namespace GammaRay;

static QObject *createClientMessageHandler(const QString & /*name*/, QObject *parent)
{
    return new MessageHandlerClient(parent);
}

MessageHandlerWidget::MessageHandlerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MessageHandlerWidget)
    , m_stateManager(this)
    , m_backtraceModel(new QStringListModel(this))
{
    ObjectBroker::registerClientObjectFactoryCallback<MessageHandlerInterface *>(
        createClientMessageHandler);
    MessageHandlerInterface *handler = ObjectBroker::object<MessageHandlerInterface *>();

    connect(handler, SIGNAL(fatalMessageReceived(QString,QString,QTime,QStringList)),
            this, SLOT(fatalMessageReceived(QString,QString,QTime,QStringList)));

    ui->setupUi(this);

    ui->messageView->header()->setObjectName("messageViewHeader");
    ui->messageView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->messageView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);

    ui->backtraceView->header()->setObjectName("backtraceViewHeader");

    ui->categoriesView->header()->setObjectName("categoriesViewHeader");
    ui->categoriesView->setStretchLastSection(false);
    ui->categoriesView->setDeferredResizeMode(0, QHeaderView::Stretch);
    ui->categoriesView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->categoriesView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);
    ui->categoriesView->setDeferredResizeMode(3, QHeaderView::ResizeToContents);
    ui->categoriesView->setDeferredResizeMode(4, QHeaderView::ResizeToContents);

    auto messageModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.MessageModel"));
    auto displayModel = new MessageDisplayModel(this);
    displayModel->setSourceModel(messageModel);
    new SearchLineController(ui->messageSearchLine, displayModel);
    ui->messageView->setModel(displayModel);
    connect(ui->messageView, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(messageContextMenu(QPoint)));
    connect(ui->messageView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(messageSelected(QItemSelection)));

    ui->backtraceView->hide();
    ui->backtraceView->setModel(m_backtraceModel);

    ui->categoriesView->setModel(ObjectBroker::model(QStringLiteral(
                                                         "com.kdab.GammaRay.LoggingCategoryModel")));

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "50%" << "50%");
    m_stateManager.setDefaultSizes(ui->messageView->header(),
                                   UISizeVector() << -1 << 300 << -1 << -1 << -1);
}

MessageHandlerWidget::~MessageHandlerWidget()
{
}

void MessageHandlerWidget::fatalMessageReceived(const QString &app, const QString &message,
                                                const QTime &time, const QStringList &backtrace)
{
    if (Endpoint::isConnected()
        && !qobject_cast<MessageHandlerClient *>(ObjectBroker::object<MessageHandlerInterface *>()))
    {
        // only show on remote side
        return;
    }
    QDialog dlg;
    dlg.setWindowTitle(tr("QFatal in %1 at %2").arg(app, time.toString()));

    QGridLayout *layout = new QGridLayout;

    QLabel *iconLabel = new QLabel;
    QIcon icon = dlg.style()->standardIcon(QStyle::SP_MessageBoxCritical, nullptr, &dlg);
    int iconSize = dlg.style()->pixelMetric(QStyle::PM_MessageBoxIconSize, nullptr, &dlg);
    iconLabel->setPixmap(icon.pixmap(iconSize, iconSize));
    iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(iconLabel, 0, 0);

    QLabel *errorLabel = new QLabel;
    errorLabel->setTextFormat(Qt::PlainText);
    errorLabel->setWordWrap(true);
    errorLabel->setText(message);
    layout->addWidget(errorLabel, 0, 1);

    QDialogButtonBox *buttons = new QDialogButtonBox;

    if (!backtrace.isEmpty()) {
        QListWidget *backtraceWidget = new QListWidget;
        foreach (const QString &frame, backtrace)
            backtraceWidget->addItem(frame);
        layout->addWidget(backtraceWidget, 1, 0, 1, 2);

        QPushButton *copyBacktraceButton = new QPushButton(tr("Copy Backtrace"));
        buttons->addButton(copyBacktraceButton, QDialogButtonBox::ActionRole);

        QSignalMapper *mapper = new QSignalMapper(this);
        mapper->setMapping(copyBacktraceButton, backtrace.join(QStringLiteral("\n")));

        connect(copyBacktraceButton, SIGNAL(clicked()), mapper, SLOT(map()));
        connect(mapper, SIGNAL(mapped(QString)), this, SLOT(copyToClipboard(QString)));
    }

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

void MessageHandlerWidget::copyToClipboard(const QString &message)
{
#ifndef QT_NO_CLIPBOARD
    QApplication::clipboard()->setText(message);
#endif
}

void MessageHandlerWidget::messageContextMenu(const QPoint &pos)
{
    auto index = ui->messageView->indexAt(pos);
    if (!index.isValid())
        return;
    index = index.sibling(index.row(), MessageModelColumn::File);
    if (!index.isValid())
        return;

    const auto fileName = index.data(MessageModelRole::File).toString();
    if (fileName.isEmpty())
        return;
    const auto line = index.data(MessageModelRole::Line).toInt();

    QMenu contextMenu;
    ContextMenuExtension cme;
    cme.setLocation(ContextMenuExtension::ShowSource, SourceLocation(QUrl(fileName), line - 1));
    cme.populateMenu(&contextMenu);
    contextMenu.exec(ui->messageView->viewport()->mapToGlobal(pos));
}

void MessageHandlerWidget::messageSelected(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;

    auto index = selection.first().topLeft();
    if (!index.isValid())
        return;

    const auto bt = index.sibling(index.row(), 0).data(MessageModelRole::Backtrace).toStringList();
    if (bt.isEmpty()) {
        ui->backtraceView->hide();
    } else {
        ui->backtraceView->show();
        m_backtraceModel->setStringList(bt);
    }
}
