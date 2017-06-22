/*
  signalmonitorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

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

#include "signalmonitorwidget.h"
#include "ui_signalmonitorwidget.h"
#include "signalhistorydelegate.h"
#include "signalhistorymodel.h"
#include "signalmonitorclient.h"
#include "signalmonitorcommon.h"

#include <ui/clientdecorationidentityproxymodel.h>
#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>

#include <common/objectbroker.h>

#include <QMenu>

#include <cmath>

using namespace GammaRay;

static QObject *signalMonitorClientFactory(const QString &, QObject *parent)
{
    return new SignalMonitorClient(parent);
}

SignalMonitorWidget::SignalMonitorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SignalMonitorWidget)
    , m_stateManager(this)
{
    StreamOperators::registerSignalMonitorStreamOperators();

    ObjectBroker::registerClientObjectFactoryCallback<SignalMonitorInterface *>(
        signalMonitorClientFactory);

    ui->setupUi(this);
    ui->pauseButton->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPause));

    QAbstractItemModel * const signalHistory
        = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.SignalHistoryModel"));
    auto *signalHistoryProxyModel = new ClientDecorationIdentityProxyModel(this);
    signalHistoryProxyModel->setSourceModel(signalHistory);
    new SearchLineController(ui->objectSearchLine, signalHistoryProxyModel);

    auto header = ui->objectTreeView->header();
    header->setObjectName("objectTreeViewHeader");
    ui->objectTreeView->setModel(signalHistoryProxyModel);
    connect(ui->objectTreeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
    auto selectionModel = ObjectBroker::selectionModel(signalHistoryProxyModel);
    ui->objectTreeView->setSelectionModel(selectionModel);
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged(QItemSelection)));

    connect(ui->pauseButton, SIGNAL(toggled(bool)), this, SLOT(pauseAndResume(bool)));
    connect(ui->intervalScale, SIGNAL(valueChanged(int)), this,
            SLOT(intervalScaleValueChanged(int)));
    connect(ui->objectTreeView, SIGNAL(delegateIsActiveChanged(bool)), this,
            SLOT(eventDelegateIsActiveChanged(bool)));

    m_stateManager.setDefaultSizes(header,
                                   UISizeVector() << ui->objectTreeView->sizeHintForColumn(0) << 200 << 200 << -1);
    header->setSortIndicator(1, header->sortIndicatorOrder());
}

SignalMonitorWidget::~SignalMonitorWidget()
{
}

void SignalMonitorWidget::intervalScaleValueChanged(int value)
{
    // FIXME: Define a more reasonable formula.
    qint64 i = 5000 / std::pow(1.07, value);
    ui->objectTreeView->setDelegateVisibleInterval(i);
}

void SignalMonitorWidget::pauseAndResume(bool pause)
{
    ui->objectTreeView->setDelegateActive(!pause);
}

void SignalMonitorWidget::eventDelegateIsActiveChanged(bool active)
{
    ui->pauseButton->setChecked(!active);
}

void SignalMonitorWidget::contextMenu(QPoint pos)
{
    auto index = ui->objectTreeView->indexAt(pos);
    if (!index.isValid())
        return;
    index = index.sibling(index.row(), 0);

    const auto objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
    if (objectId.isNull())
        return;

    QMenu menu;
    ContextMenuExtension ext(objectId);
    ext.populateMenu(&menu);
    menu.exec(ui->objectTreeView->viewport()->mapToGlobal(pos));
}

void SignalMonitorWidget::selectionChanged(const QItemSelection& selection)
{
    if (selection.isEmpty())
        return;
    const auto idx = selection.at(0).topLeft();
    ui->objectTreeView->scrollTo(idx);
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(SignalMonitorUiFactory)
#endif
