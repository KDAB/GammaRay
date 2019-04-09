/*
  signalmonitorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

    ui->objectTreeView->header()->setObjectName("objectTreeViewHeader");
    ui->objectTreeView->setModel(signalHistoryProxyModel);
    ui->objectTreeView->setEventScrollBar(ui->eventScrollBar);
    connect(ui->objectTreeView, &QWidget::customContextMenuRequested, this, &SignalMonitorWidget::contextMenu);
    auto selectionModel = ObjectBroker::selectionModel(signalHistoryProxyModel);
    ui->objectTreeView->setSelectionModel(selectionModel);
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &SignalMonitorWidget::selectionChanged);

    connect(ui->pauseButton, &QAbstractButton::toggled, this, &SignalMonitorWidget::pauseAndResume);
    connect(ui->intervalScale, &QAbstractSlider::valueChanged, this,
            &SignalMonitorWidget::intervalScaleValueChanged);
    connect(ui->objectTreeView->eventDelegate(), &SignalHistoryDelegate::isActiveChanged, this,
            &SignalMonitorWidget::eventDelegateIsActiveChanged);
    connect(ui->objectTreeView->header(), &QHeaderView::sectionResized, this,
            &SignalMonitorWidget::adjustEventScrollBarSize);

    m_stateManager.setDefaultSizes(ui->objectTreeView->header(),
                                   UISizeVector() << 200 << 200 << -1);
}

SignalMonitorWidget::~SignalMonitorWidget() = default;

void SignalMonitorWidget::intervalScaleValueChanged(int value)
{
    // FIXME: Define a more reasonable formula.
    qint64 i = 5000 / std::pow(1.07, value);
    ui->objectTreeView->eventDelegate()->setVisibleInterval(i);
}

void SignalMonitorWidget::adjustEventScrollBarSize()
{
    // FIXME: Would like to have this in SignalHistoryView, but letting that
    // widget manage layouts of this widget would be nasty. Still I also I don't
    // feel like hooking a custom scrollbar into QTreeView. Sleeping between a
    // rock and a hard place.
    const QWidget * const scrollBar = ui->objectTreeView->verticalScrollBar();
    const QWidget * const viewport = ui->objectTreeView->viewport();

    const int eventColumnLeft = ui->objectTreeView->eventColumnPosition();
    const int scrollBarLeft = scrollBar->mapTo(this, scrollBar->pos()).x();
    const int viewportLeft = viewport->mapTo(this, viewport->pos()).x();
    const int viewportRight = viewportLeft + viewport->width();

    ui->eventScrollBarLayout->setContentsMargins(eventColumnLeft,
                                                 scrollBarLeft - viewportRight,
                                                 width() - viewportRight,
                                                 0);
}

void SignalMonitorWidget::pauseAndResume(bool pause)
{
    ui->objectTreeView->eventDelegate()->setActive(!pause);
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
