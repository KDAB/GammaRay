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
#include "quickclientitemmodel.h"
#include "quickitemtreewatcher.h"
#include "ui_quickinspectorwidget.h"

#include <common/objectbroker.h>
#include <ui/deferredresizemodesetter.h>

#include <QLabel>

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
  connect(ui->windowComboBox, SIGNAL(currentIndexChanged(int)), m_interface, SLOT(selectWindow(int)));
  if (ui->windowComboBox->currentIndex() >= 0)
    m_interface->selectWindow(ui->windowComboBox->currentIndex());

  QSortFilterProxyModel *proxy = new QuickClientItemModel(this);
  proxy->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.QuickItemModel"));
  proxy->setDynamicSortFilter(true);
  ui->itemTreeView->setModel(proxy);
  ui->itemTreeSearchLine->setProxy(proxy);
  QItemSelectionModel* selectionModel = ObjectBroker::selectionModel(proxy);
  ui->itemTreeView->setSelectionModel(selectionModel);
  connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          this, SLOT(itemSelectionChanged(QItemSelection)));

  new QuickItemTreeWatcher(ui->itemTreeView);
  new DeferredResizeModeSetter(ui->itemTreeView->header(), 0, QHeaderView::ResizeToContents);

  ui->itemPropertyWidget->setObjectBaseName("com.kdab.GammaRay.QuickItem");

  m_sceneImage = new QLabel;
  ui->sceneView->setWidget(m_sceneImage);
  ui->sceneView->setBackgroundRole(QPalette::Dark);
}

QuickInspectorWidget::~QuickInspectorWidget()
{
}

void QuickInspectorWidget::sceneRendered(const QImage& img)
{
  // ### only for testing
  m_sceneImage->resize(img.size());
  m_sceneImage->setPixmap(QPixmap::fromImage(img));
}

void QuickInspectorWidget::itemSelectionChanged(const QItemSelection& selection)
{
  if (selection.isEmpty())
    return;
  const QModelIndex &index = selection.first().topLeft();
  ui->itemTreeView->scrollTo(index);
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(QuickInspectorUiFactory)
#endif
