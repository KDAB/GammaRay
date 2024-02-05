/*
  messagehandlerwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "messagehandlerwidget.h"
#include "ui_messagehandlerwidget.h"
#include "messagehandlerclient.h"
#include "messagedisplaymodel.h"

#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>
#include <ui/uiintegration.h>
#include <ui/propertyeditor/propertyeditordelegate.h>

#include <common/endpoint.h>
#include <common/objectbroker.h>
#include <common/tools/messagehandler/messagemodelroles.h>
#include <common/tools/messagehandler/messagehandlerinterface.h>

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
#include <QUrl>
#include <QFileDialog>
#include <QFile>

using namespace GammaRay;

static QObject *createClientMessageHandler(const QString & /*name*/, QObject *parent)
{
    return new MessageHandlerClient(parent);
}

MessageHandlerWidget::MessageHandlerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MessageHandlerWidget)
    , m_stateManager(this)
{
    ObjectBroker::registerClientObjectFactoryCallback<MessageHandlerInterface *>(
        createClientMessageHandler);
    MessageHandlerInterface *handler = ObjectBroker::object<MessageHandlerInterface *>();

    connect(handler, &MessageHandlerInterface::fatalMessageReceived,
            this, &MessageHandlerWidget::fatalMessageReceived);

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
    ui->messageView->setSelectionModel(ObjectBroker::selectionModel(displayModel));
    connect(ui->messageView, &QWidget::customContextMenuRequested, this,
            &MessageHandlerWidget::messageContextMenu);

    ui->backtraceView->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.MessageStackTraceModel")));
    ui->backtraceView->setVisible(handler->stackTraceAvailable());
    ui->backtraceView->setItemDelegate(new PropertyEditorDelegate(ui->backtraceView));
    connect(handler, &MessageHandlerInterface::stackTraceAvailableChanged, ui->backtraceView, &QWidget::setVisible);
    connect(ui->backtraceView, &QWidget::customContextMenuRequested, this, &MessageHandlerWidget::stackTraceContextMenu);
    connect(ui->saveAllConf, &QPushButton::clicked, this, &MessageHandlerWidget::saveFileAllLogConfig);
    connect(ui->saveModConf, &QPushButton::clicked, this, &MessageHandlerWidget::saveFileModLogConfig);
    connect(ui->copyModConf, &QPushButton::clicked, this, &MessageHandlerWidget::exportModLogConfig);

    ui->categoriesView->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.LoggingCategoryModel")));

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "50%"
                                                                    << "50%");
    m_stateManager.setDefaultSizes(ui->messageView->header(),
                                   UISizeVector() << -1 << 300 << -1 << -1 << -1);
}

MessageHandlerWidget::~MessageHandlerWidget() = default;

void MessageHandlerWidget::fatalMessageReceived(const QString &app, const QString &message,
                                                const QTime &time, const QStringList &backtrace)
{
    if (Endpoint::isConnected()
        && !qobject_cast<MessageHandlerClient *>(ObjectBroker::object<MessageHandlerInterface *>())) {
        // only show on remote side
        return;
    }
    QDialog dlg;
    dlg.setWindowTitle(tr("QFatal in %1 at %2").arg(app, time.toString()));

    auto *layout = new QGridLayout;

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

    auto *buttons = new QDialogButtonBox;

    if (!backtrace.isEmpty()) {
        auto *backtraceWidget = new QListWidget;
        for (const QString &frame : backtrace)
            backtraceWidget->addItem(frame);
        layout->addWidget(backtraceWidget, 1, 0, 1, 2);

        QPushButton *copyBacktraceButton = new QPushButton(tr("Copy Backtrace"));
        buttons->addButton(copyBacktraceButton, QDialogButtonBox::ActionRole);

        auto joinedBacktrace = backtrace.join(u'\n');
        connect(copyBacktraceButton, &QPushButton::clicked, this, [joinedBacktrace] { copyToClipboard(joinedBacktrace); });
    }

    buttons->addButton(QDialogButtonBox::Close);
    QObject::connect(buttons, &QDialogButtonBox::accepted,
                     &dlg, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected,
                     &dlg, &QDialog::reject);
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
    cme.setLocation(ContextMenuExtension::ShowSource, SourceLocation::fromOneBased(QUrl(fileName), line));
    cme.populateMenu(&contextMenu);

    MessageHandlerInterface *handler = ObjectBroker::object<MessageHandlerInterface *>();
    auto copyAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), tr("Copy Backtrace"));
    copyAction->setVisible(handler->stackTraceAvailable());
    connect(handler, &MessageHandlerInterface::stackTraceAvailableChanged, copyAction, &QAction::setVisible);

    connect(copyAction, &QAction::triggered, this, [this, handler] {
        delete m_backtraceFetchContext;
        m_backtraceFetchContext = new QObject(handler);

        connect(handler, &MessageHandlerInterface::fullTraceChanged, m_backtraceFetchContext, [handler] {
            qApp->clipboard()->setText(handler->fullTrace().join(QStringLiteral("\n")));
        });

        handler->generateFullTrace();
    });

    contextMenu.exec(ui->messageView->viewport()->mapToGlobal(pos));
}

void MessageHandlerWidget::stackTraceContextMenu(QPoint pos)
{
    const auto idx = ui->backtraceView->indexAt(pos);
    if (!idx.isValid())
        return;

    const auto loc = idx.sibling(idx.row(), 1).data().value<SourceLocation>();
    if (!loc.isValid())
        return;

    QMenu contextMenu;
    ContextMenuExtension cme;
    cme.setLocation(ContextMenuExtension::ShowSource, loc);
    cme.populateMenu(&contextMenu);
    contextMenu.exec(ui->backtraceView->viewport()->mapToGlobal(pos));
}

void MessageHandlerWidget::saveFileAllLogConfig()
{
    saveFileLogConfig(true);
}

void MessageHandlerWidget::saveFileModLogConfig()
{
    saveFileLogConfig(false);
}

void MessageHandlerWidget::saveFileLogConfig(bool all)
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save File"),
                                                    {},
                                                    tr("Config Files (*.ini)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QFile::WriteOnly)) {
            qWarning() << "Failed to save file" << fileName << file.errorString();
            return;
        }

        QByteArray config;
        auto model = ui->categoriesView->model();
        model->metaObject()->invokeMethod(model, "exportLoggingConfig", Q_RETURN_ARG(QByteArray, config), Q_ARG(bool, all), Q_ARG(bool, true));
        file.write(config);
    }
}

void MessageHandlerWidget::exportModLogConfig()
{
    QByteArray config;
    auto model = ui->categoriesView->model();
    model->metaObject()->invokeMethod(model, "exportLoggingConfig", Q_RETURN_ARG(QByteArray, config), Q_ARG(bool, false), Q_ARG(bool, false));
    QString env = QLatin1String("QT_LOGGING_RULES='") + QString::fromLatin1(config) + QLatin1Char('\'');
    QGuiApplication::clipboard()->setText(env);
}
