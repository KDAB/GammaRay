/*
  quickinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "quickinspectorwidget.h"
#include "quickinspectorclient.h"
#include "ui_quickinspectorwidget.h"

#include <common/objectbroker.h>

#include <QSortFilterProxyModel>

using namespace GammaRay;


static QObject* createQuickInspectorClient(const QString &/*name*/, QObject *parent)
{
  return new QuickInspectorClient(parent);
}

QuickInspectorWidget::QuickInspectorWidget(QWidget* parent) :
  QWidget(parent),
  ui(new Ui::QuickInspectorWidget)
{
  ObjectBroker::registerClientObjectFactoryCallback<QuickInspectorInterface*>(createQuickInspectorClient);
  m_interface = ObjectBroker::object<QuickInspectorInterface*>();
  connect(m_interface, SIGNAL(sceneRendered(QImage)), this, SLOT(sceneRendered(QImage)));

  ui->setupUi(this);

  ui->windowComboBox->setModel(ObjectBroker::model("com.kdab.GammaRay.QuickWindowModel"));

  QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
  proxy->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.QuickItemModel"));
  proxy->setDynamicSortFilter(true);
  ui->itemTreeView->setModel(proxy);
  ui->itemTreeSearchLine->setProxy(proxy);
  ui->itemTreeView->setSelectionModel(ObjectBroker::selectionModel(proxy));

  ui->itemPropertyWidget->setObjectBaseName("com.kdab.GammaRay.QuickItem");
}

QuickInspectorWidget::~QuickInspectorWidget()
{
}

void QuickInspectorWidget::sceneRendered(const QImage& img)
{
  // ### only for testing
  ui->sceneView->setPixmap(QPixmap::fromImage(img));
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(QuickInspectorUiFactory)
#endif
