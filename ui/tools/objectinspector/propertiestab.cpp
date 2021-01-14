/*
  propertiestab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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
    connect(m_ui->newPropertyType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
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

    const int actions = index.data(PropertyModel::ActionRole).toInt();
    const auto objectId = index.data(PropertyModel::ObjectIdRole).value<ObjectId>();
    ContextMenuExtension ext(objectId);
    const bool canShow = actions != PropertyModel::NoAction
                         || ext.discoverPropertySourceLocation(ContextMenuExtension::GoTo, index);

    if (!canShow)
        return;

    QMenu contextMenu;

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

    const QByteArray editorPropertyName
        = PropertyEditorFactory::instance()->valuePropertyName(type);
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
