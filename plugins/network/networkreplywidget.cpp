/*
  networkreplywidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "networkreplywidget.h"
#include "clientnetworkreplymodel.h"
#include "networkreplymodeldefs.h"
#include "ui_networkreplywidget.h"
#include "networksupportclient.h"

#include <ui/contextmenuextension.h>

#include <common/objectbroker.h>

#include <QClipboard>
#include <QGuiApplication>
#include <QMenu>
#include <QPlainTextEdit>
#include <QJsonDocument>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
#else
#include <QStringDecoder>
#endif

using namespace GammaRay;

static QObject *createClientNetworkSupportInterface(const QString & /*name*/, QObject *parent)
{
    return new NetworkSupportClient(parent);
}

NetworkReplyWidget::NetworkReplyWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NetworkReplyWidget)
{
    ui->setupUi(this);

    ObjectBroker::registerClientObjectFactoryCallback<NetworkSupportInterface *>(
        createClientNetworkSupportInterface);
    auto interface = ObjectBroker::object<NetworkSupportInterface *>();

    auto srcModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.NetworkReplyModel"));
    auto proxy = new ClientNetworkReplyModel(this);
    proxy->setSourceModel(srcModel);
    ui->replyView->setModel(proxy);
    ui->replyView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->replyView->expandAll();

    // auto-expand parents of new replies
    connect(proxy, &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex &parent, int, int) {
        if (!parent.isValid()) {
            return;
        }
        ui->replyView->expand(parent);
    });

    connect(ui->replyView, &QWidget::customContextMenuRequested, this, &NetworkReplyWidget::contextMenu);
    connect(ui->replyView->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex &current, const QModelIndex &) {
        const auto objColumn = current.sibling(current.row(), NetworkReplyModelColumn::ObjectColumn);
        auto response = objColumn.data(NetworkReplyModelRole::ReplyResponseRole).toByteArray();
        const auto contentType = ( NetworkReply::ContentType )objColumn.data(NetworkReplyModelRole::ReplyContentType).toInt();

        if (contentType == NetworkReply::Json) {
            response = QJsonDocument::fromJson(response).toJson();
        }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QStringDecoder decoder(QStringDecoder::Utf8);
        QByteArrayView bav(response.constData(), response.size());
        const QString text = decoder.decode(bav);
        if (!decoder.hasError()) {
            ui->responseTextEdit->setPlainText(text);
        }
#else

            QTextCodec::ConverterState state;
            QTextCodec *codec = QTextCodec::codecForName("UTF-8");
            const QString text = codec->toUnicode(response.constData(), response.size(), &state);
            if (state.invalidChars > 0) {
                ui->responseTextEdit->setPlainText(tr("%1: Unable to show response preview").arg(qApp->applicationName()));
            } else {
                // TODO: Add support for pretty-printing XML etc
                ui->responseTextEdit->setPlainText(text);
            }
#endif
    });
    ui->responseTextEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    connect(ui->responseTextEdit, &QPlainTextEdit::textChanged, this, [this]() {
        ui->responseTextEdit->setVisible(!ui->responseTextEdit->toPlainText().isEmpty());
    });
    connect(ui->captureResponse, &QCheckBox::toggled, interface, [interface](bool checked) {
        interface->setProperty("captureResponse", checked);
    });
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    ui->captureResponse->setVisible(false);
#endif
}

NetworkReplyWidget::~NetworkReplyWidget() = default;

void NetworkReplyWidget::contextMenu(QPoint pos)
{
    const auto index = ui->replyView->indexAt(pos);
    if (!index.isValid())
        return;

    const auto objectId = index.sibling(index.row(), NetworkReplyModelColumn::ObjectColumn).data(NetworkReplyModelRole::ObjectIdRole).value<ObjectId>();
    const auto url = index.sibling(index.row(), NetworkReplyModelColumn::UrlColumn).data(Qt::DisplayRole).toString();

    QMenu menu;
    if (!url.isEmpty()) {
        auto action = menu.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), tr("Copy URL"));
        connect(action, &QAction::triggered, this, [url]() {
            QGuiApplication::clipboard()->setText(url);
        });
        menu.addSeparator();
    }

    ContextMenuExtension ext(objectId);
    ext.populateMenu(&menu);
    menu.exec(ui->replyView->viewport()->mapToGlobal(pos));
}
