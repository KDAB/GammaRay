/*
  translatorinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "translatorinspectorwidget.h"
#include "ui_translatorinspectorwidget.h"
#include "translatorsmodel.h" // use only for its role enums
#include "translatorwrapperproxy.h"

#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>

#include <common/objectbroker.h>
#include <common/objectid.h>
#include <common/endpoint.h>

#include <QMenu>
#include <QIdentityProxyModel>

using namespace GammaRay;

TranslatorInspectorClient::TranslatorInspectorClient(const QString &name, QObject *parent)
    : TranslatorInspectorInterface(name, parent)
{
}

void TranslatorInspectorClient::sendLanguageChangeEvent()
{
    Endpoint::instance()->invokeObject(name(), "sendLanguageChangeEvent");
}

void TranslatorInspectorClient::resetTranslations()
{
    Endpoint::instance()->invokeObject(name(), "resetTranslations");
}

TranslatorInspectorWidget::TranslatorInspectorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TranslatorInspectorWidget)
    , m_stateManager(this)
{
    ui->setupUi(this);

    m_inspector = ObjectBroker::object<TranslatorInspectorInterface *>(QStringLiteral(
        "com.kdab.GammaRay.TranslatorInspector"));

    ui->translatorList->header()->setObjectName("translatorListHeader");
    ui->translatorList->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->translatorList->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->translatorList->setDeferredResizeMode(2, QHeaderView::ResizeToContents);
    ui->translatorList->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TranslatorsModel")));
    ui->translatorList->setSelectionModel(ObjectBroker::selectionModel(ui->translatorList->model()));
    connect(ui->translatorList, &QTreeView::customContextMenuRequested, this, &TranslatorInspectorWidget::translatorContextMenu);

    // searching for translations
    {
        auto proxy = new TranslatorWrapperProxy(this);
        proxy->setSourceModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TranslationsModel")));

        ui->translationsView->header()->setObjectName("translationsViewHeader");
        ui->translationsView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
        ui->translationsView->setModel(proxy);
        connect(ui->translationsView, &QTreeView::customContextMenuRequested, this, &TranslatorInspectorWidget::translationsContextMenu);

        const auto selectionModel = ObjectBroker::selectionModel(ui->translationsView->model());
        ui->translationsView->setSelectionModel(selectionModel);
        connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &TranslatorInspectorWidget::updateActions);

        new SearchLineController(ui->translationsSearchLine, ui->translationsView->model());
    }

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "50%"
                                                                    << "50%");

    connect(ui->actionSendLanguageChange, &QAction::triggered, m_inspector, &TranslatorInspectorInterface::sendLanguageChangeEvent);
    connect(ui->actionReset, &QAction::triggered, m_inspector, &TranslatorInspectorInterface::resetTranslations);

    addAction(ui->actionReset);
    addAction(ui->actionSendLanguageChange);

    updateActions();
}

TranslatorInspectorWidget::~TranslatorInspectorWidget() = default;

void TranslatorInspectorWidget::translatorContextMenu(QPoint pos)
{
    auto index = ui->translatorList->indexAt(pos);
    if (!index.isValid())
        return;
    index = index.sibling(index.row(), 0);

    const auto objectId = index.data(TranslatorsModel::ObjectIdRole).value<ObjectId>();
    if (objectId.isNull())
        return;

    QMenu menu;
    ContextMenuExtension ext(objectId);
    ext.populateMenu(&menu);
    menu.exec(ui->translatorList->viewport()->mapToGlobal(pos));
}

void TranslatorInspectorWidget::translationsContextMenu(QPoint pos)
{
    QMenu menu;
    menu.addAction(ui->actionReset);
    menu.exec(ui->translationsView->viewport()->mapToGlobal(pos));
}

void TranslatorInspectorWidget::updateActions()
{
    ui->actionReset->setEnabled(!ui->translationsView->selectionModel()->selectedRows().isEmpty());
}


static QObject *translatorInspectorClientFactory(const QString &name, QObject *parent)
{
    return new TranslatorInspectorClient(name, parent);
}

void TranslatorInspectorWidgetFactory::initUi()
{
    ObjectBroker::registerClientObjectFactoryCallback<TranslatorInspectorInterface *>(
        translatorInspectorClientFactory);
}
