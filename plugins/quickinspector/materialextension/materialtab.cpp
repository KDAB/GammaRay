/*
  materialtab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "materialtab.h"
#include "materialextensioninterface.h"
#include "ui_materialtab.h"

#include <ui/clientpropertymodel.h>
#include <ui/contextmenuextension.h>
#include <ui/propertywidget.h>
#include <ui/propertyeditor/propertyeditordelegate.h>

#include <common/objectbroker.h>
#include <common/objectid.h>
#include <common/propertymodel.h>

#include <QMenu>

using namespace GammaRay;

MaterialTab::MaterialTab(PropertyWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui_MaterialTab)
    , m_interface(nullptr)
{
    m_ui->setupUi(this);
    m_ui->materialPropertyView->setItemDelegate(new PropertyEditorDelegate(this));
    m_ui->materialPropertyView->header()->setObjectName("materialPropertyViewHeader");
    connect(m_ui->materialPropertyView, &QTreeView::customContextMenuRequested, this, &MaterialTab::propertyContextMenu);

    setObjectBaseName(parent->objectBaseName());
    connect(m_ui->shaderList, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MaterialTab::shaderSelectionChanged);

    m_ui->shaderEdit->setSyntaxDefinition(QLatin1String("GLSL"));

    m_ui->splitter->setStretchFactor(0, 1);
    m_ui->splitter->setStretchFactor(1, 3);
}

MaterialTab::~MaterialTab() = default;

void MaterialTab::setObjectBaseName(const QString &baseName)
{
    if (m_interface)
        disconnect(m_interface, nullptr, this, nullptr);

    m_interface = ObjectBroker::object<MaterialExtensionInterface *>(baseName + ".material");
    connect(m_interface, &MaterialExtensionInterface::gotShader, this, &MaterialTab::showShader);

    auto clientPropModel = new ClientPropertyModel(this);
    clientPropModel->setSourceModel(ObjectBroker::model(baseName + ".materialPropertyModel"));
    m_ui->materialPropertyView->setModel(clientPropModel);
    m_ui->shaderList->setModel(ObjectBroker::model(baseName + ".shaderModel"));
}

void MaterialTab::shaderSelectionChanged(int idx)
{
    m_ui->shaderEdit->clear();
    if (idx < 0)
        return;
    m_interface->getShader(idx);
}

void MaterialTab::showShader(const QString &shaderSource)
{
    m_ui->shaderEdit->setPlainText(shaderSource);
}

void MaterialTab::propertyContextMenu(QPoint pos)
{
    const auto idx = m_ui->materialPropertyView->indexAt(pos);
    if (!idx.isValid())
        return;

    const auto actions = idx.data(PropertyModel::ActionRole).toInt();
    const auto objectId = idx.data(PropertyModel::ObjectIdRole).value<ObjectId>();
    ContextMenuExtension ext(objectId);
    const bool canShow = (actions == PropertyModel::NavigateTo && !objectId.isNull())
        || ext.discoverPropertySourceLocation(ContextMenuExtension::GoTo, idx);

    if (!canShow)
        return;

    QMenu contextMenu;
    ext.populateMenu(&contextMenu);
    contextMenu.exec(m_ui->materialPropertyView->viewport()->mapToGlobal(pos));
}
