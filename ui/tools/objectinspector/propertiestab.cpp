/*
  propertiestab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "propertiestab.h"
#include "ui_propertiestab.h"
#include "propertywidget.h"
#include "editabletypesmodel.h"

#include <ui/clientpropertymodel.h>
#include <ui/contextmenuextension.h>
#include <ui/propertyeditor/propertyeditordelegate.h>
#include <ui/propertyeditor/propertyeditorfactory.h>
#include <ui/searchlinecontroller.h>
#include <propertybinder.h>

#include <common/objectbroker.h>
#include <common/propertymodel.h>
#include <common/tools/objectinspector/propertiesextensioninterface.h>

#include <QSortFilterProxyModel>
#include <QMenu>
#include <QDebug>
#include <QClipboard>

using namespace GammaRay;

PropertiesTab::PropertiesTab(PropertyWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui_PropertiesTab)
    , m_interface(nullptr)
    , m_newPropertyValue(nullptr)
{
    m_ui->setupUi(this);
    m_ui->propertyView->header()->setObjectName("propertyViewHeader");

    m_ui->newPropertyButton->setIcon(QIcon::fromTheme(QStringLiteral("list-add")));

    setObjectBaseName(parent->objectBaseName());
}

PropertiesTab::~PropertiesTab() = default;

void PropertiesTab::setObjectBaseName(const QString &baseName)
{
    auto model = ObjectBroker::model(baseName + '.' + "properties");
    auto clientModel = new ClientPropertyModel(this);
    clientModel->setSourceModel(model);

    auto proxy = new QSortFilterProxyModel(this);
    proxy->setDynamicSortFilter(true);
    proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxy->setSourceModel(clientModel);
    m_ui->propertyView->setModel(proxy);
    m_ui->propertyView->sortByColumn(0, Qt::AscendingOrder);
    m_ui->propertyView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    new SearchLineController(m_ui->propertySearchLine, proxy);
    m_ui->propertyView->setItemDelegate(new PropertyEditorDelegate(this));
    connect(m_ui->propertyView, &QWidget::customContextMenuRequested,
            this, &PropertiesTab::propertyContextMenu);

    auto typesModel = new EditableTypesModel(this);
    proxy = new QSortFilterProxyModel(this);
    proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxy->setSourceModel(typesModel);
    proxy->sort(0);
    m_ui->newPropertyType->setModel(proxy);
    connect(m_ui->newPropertyType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &PropertiesTab::updateNewPropertyValueEditor);
    updateNewPropertyValueEditor();
    connect(m_ui->newPropertyName, &QLineEdit::textChanged,
            this, &PropertiesTab::validateNewProperty);
    validateNewProperty();
    connect(m_ui->newPropertyButton, &QAbstractButton::clicked,
            this, &PropertiesTab::addNewProperty);

    m_interface = ObjectBroker::object<PropertiesExtensionInterface *>(
        baseName + ".propertiesExtension");
    new PropertyBinder(m_interface, "canAddProperty", m_ui->newPropertyBar, "visible");
    m_ui->propertyView->setDeferredHidden(1, !m_interface->hasPropertyValues());
    m_ui->propertyView->setRootIsDecorated(m_interface->hasPropertyValues());
    connect(m_interface, &PropertiesExtensionInterface::hasPropertyValuesChanged, this, &PropertiesTab::hasValuesChanged);
}

static PropertyEditorFactory::TypeId selectedTypeId(QComboBox *box)
{
    return static_cast<PropertyEditorFactory::TypeId>(
        box->itemData(box->currentIndex(), Qt::UserRole).toInt());
}

void PropertiesTab::updateNewPropertyValueEditor()
{
    delete m_newPropertyValue;

    const PropertyEditorFactory::TypeId type = selectedTypeId(m_ui->newPropertyType);

    m_newPropertyValue = PropertyEditorFactory::instance()->createEditor(type, this);
    static_cast<QHBoxLayout *>(m_ui->newPropertyBar->layout())->insertWidget(5, m_newPropertyValue);
    m_ui->newPropertyValueLabel->setBuddy(m_newPropertyValue);
}

void PropertiesTab::validateNewProperty()
{
    Q_ASSERT(m_newPropertyValue);
    m_ui->newPropertyButton->setEnabled(!m_ui->newPropertyName->text().isEmpty());
}

void PropertiesTab::propertyContextMenu(const QPoint &pos)
{
    const QModelIndex index = m_ui->propertyView->indexAt(pos);
    if (!index.isValid())
        return;

    auto getPropertyNameAndValue = [](const QModelIndex &idx) {
        if (!idx.isValid())
            return QString();
        const auto nameIdx = idx.sibling(idx.row(), PropertyModel::PropertyColumn);
        const auto valIdx = idx.sibling(idx.row(), PropertyModel::ValueColumn);
        const QString value = valIdx.data().toString();
        if (value.isEmpty())
            return QString();
        QString ret = nameIdx.data().toString() + QStringLiteral(": ") + value;
        return ret;
    };

    const int actions = index.data(PropertyModel::ActionRole).toInt();
    const auto objectId = index.data(PropertyModel::ObjectIdRole).value<ObjectId>();
    ContextMenuExtension ext(objectId);
    const QString property = getPropertyNameAndValue(index);

    const bool canShow = actions != PropertyModel::NoAction
        || ext.discoverPropertySourceLocation(ContextMenuExtension::GoTo, index)
        || !property.isEmpty();

    if (!canShow)
        return;

    QMenu contextMenu;

    if (!property.isEmpty()) {
#ifndef QT_NO_CLIPBOARD
        contextMenu.addAction(tr("Copy"), this, [property] {
            qApp->clipboard()->setText(property);
        });
#endif
    }

    if (actions & PropertyModel::Delete) {
        QAction *action = contextMenu.addAction(tr("Remove"));
        action->setData(PropertyModel::Delete);
    }
    if (actions & PropertyModel::Reset) {
        QAction *action = contextMenu.addAction(tr("Reset"));
        action->setData(PropertyModel::Reset);
    }

    ext.populateMenu(&contextMenu);

    if (QAction *action = contextMenu.exec(m_ui->propertyView->viewport()->mapToGlobal(pos))) {
        switch (action->data().toInt()) {
        case PropertyModel::Delete:
            m_ui->propertyView->model()->setData(index, QVariant(), Qt::EditRole);
            break;
        case PropertyModel::Reset:
            m_ui->propertyView->model()->setData(index, QVariant(), PropertyModel::ResetActionRole);
            break;
        }
    }
}

void PropertiesTab::addNewProperty()
{
    Q_ASSERT(m_interface->canAddProperty());
    const PropertyEditorFactory::TypeId type = selectedTypeId(m_ui->newPropertyType);

    const QByteArray editorPropertyName = PropertyEditorFactory::instance()->valuePropertyName(type);
    const QVariant value = m_newPropertyValue->property(editorPropertyName);
    m_interface->setProperty(m_ui->newPropertyName->text(), value);

    m_ui->newPropertyName->clear();
    updateNewPropertyValueEditor();
}

void PropertiesTab::hasValuesChanged()
{
    m_ui->propertyView->setDeferredHidden(1, !m_interface->hasPropertyValues());
    m_ui->propertyView->setRootIsDecorated(m_interface->hasPropertyValues());
}
