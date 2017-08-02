/*
  timertopwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Thomas McGuire <thomas.mcguire@kdab.com>

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

#include "timertopwidget.h"
#include "ui_timertopwidget.h"
#include "timermodel.h"
#include "timertopclient.h"

#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>

#include <common/objectbroker.h>

#include <QMenu>
#include <QSortFilterProxyModel>
#include <QTimer>

using namespace GammaRay;

class ClientTimerModel : public QSortFilterProxyModel
{
public:
    explicit ClientTimerModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    { }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (hasIndex(index.row(), index.column()) && role == Qt::DisplayRole) {
            switch (static_cast<TimerModel::Columns>(index.column())) {
            case TimerModel::ColumnCount:
                Q_ASSERT(false);
            case TimerModel::ObjectNameColumn:
            case TimerModel::TimerIdColumn:
            case TimerModel::TotalWakeupsColumn:
                // Use source model data
                break;
            case TimerModel::StateColumn:
                return stateToString(QSortFilterProxyModel::data(index, role).toUInt());
            case TimerModel::WakeupsPerSecColumn:
                return wakeupsPerSecToString(QSortFilterProxyModel::data(index, role).toReal());
            case TimerModel::TimePerWakeupColumn:
                return timePerWakeupToString(QSortFilterProxyModel::data(index, role).toReal());
            case TimerModel::MaxTimePerWakeupColumn:
                return maxWakeupTimeToString(QSortFilterProxyModel::data(index, role).toUInt());
            }
        }

        return QSortFilterProxyModel::data(index, role);
    }

    static QString stateToString(uint state)
    {
        const int timerState = (state & 0xffff);
        const int timerInterval = (state >> 16);

        switch (timerState) {
        case 0: // None
            return tr("None");
        case 1: // Not Running
            return tr("Inactive (%1 ms)").arg(timerInterval);
        case 2: // Single Shot
            return tr("Singleshot (%1 ms)").arg(timerInterval);
        case 3: // Repeat
            return tr("Repeating (%1 ms)").arg(timerInterval);
        }

        return QString();
    }

    static QString wakeupsPerSecToString(qreal value)
    {
        return qFuzzyIsNull(value) ? tr("0") : QString::number(value, 'f', 1);

    }

    static QString timePerWakeupToString(qreal value)
    {
        return qFuzzyIsNull(value) ? QStringLiteral("N/A") : QString::number(value, 'f', 1);
    }

    static QString maxWakeupTimeToString(uint value)
    {
        return value == 0 ? tr("N/A") : QString::number(value);
    }
};

static QObject *createTimerTopClient(const QString & /*name*/, QObject *parent)
{
    return new TimerTopClient(parent);
}

TimerTopWidget::TimerTopWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimerTopWidget)
    , m_stateManager(this)
{
    ui->setupUi(this);

    ObjectBroker::registerClientObjectFactoryCallback<TimerTopInterface *>(
        createTimerTopClient);

    m_interface = ObjectBroker::object<TimerTopInterface *>();

    ui->timerView->header()->setObjectName("timerViewHeader");
    ui->timerView->setDeferredResizeMode(0, QHeaderView::Stretch);
    ui->timerView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->timerView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);
    ui->timerView->setDeferredResizeMode(3, QHeaderView::ResizeToContents);
    ui->timerView->setDeferredResizeMode(4, QHeaderView::ResizeToContents);
    ui->timerView->setDeferredResizeMode(5, QHeaderView::ResizeToContents);
    connect(ui->timerView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
    connect(ui->clearTimers, SIGNAL(clicked()), m_interface, SLOT(clearHistory()));

    auto * const sortModel = new ClientTimerModel(this);
    sortModel->setSourceModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TimerModel")));
    sortModel->setDynamicSortFilter(true);
    ui->timerView->setModel(sortModel);
    ui->timerView->setSelectionModel(ObjectBroker::selectionModel(sortModel));

    new SearchLineController(ui->timerViewFilter, sortModel);

    ui->timerView->sortByColumn(TimerModel::WakeupsPerSecColumn, Qt::DescendingOrder);
}

TimerTopWidget::~TimerTopWidget()
{
}

void TimerTopWidget::contextMenu(QPoint pos)
{
    auto index = ui->timerView->indexAt(pos);
    if (!index.isValid())
        return;
    index = index.sibling(index.row(), 0);

    const auto objectId = index.data(TimerModel::ObjectIdRole).value<ObjectId>();
    if (objectId.isNull())
        return;

    QMenu menu;
    ContextMenuExtension ext(objectId);
    ext.populateMenu(&menu);
    menu.exec(ui->timerView->viewport()->mapToGlobal(pos));
}


#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(TimerTopUiFactory)
#endif
